#include "FubonTAIFEXConnection.h"
#include "../../UFC/NetUtility.h"
#include "../../UFC/FileStream.h"
#include "../../Migo/Sigo.h"
//------------------------------------------------------------------------------
//  Place order
//  Subject:    "ORDER.TSE" "ORDER.OTC" "ORDER.FUT" "ORDER.OPT"
//  Key:        "S889000" BrokerID
//  Value NAme: "DATA"
//  Value:      Order/Cancel/Replace string  
//
//  Order Confirm
//  Subject:    "TSE.CONFIRM" "OTC.CONFIRM"
//  Key:        "880T" BrokerID
//  Value NAme: "DATA"
//  Value:       Exchange confirm string  
//
//  Fill
//  Subject:    "TSE.FILL" "OTC.FILL"
//  Key:        "880T" BrokerID
//  Value NAme: "DATA"
//  Value:       Exchange fill string  
//
//------------------------------------------------------------------------------
nsOrderMessageDefine::OrderSourceEnum ToOrderSourceEnum( char ch)
{
    nsOrderMessageDefine::OrderSourceEnum OS = nsOrderMessageDefine::osGeneral;
    switch( ch )
    {
        case 'D':OS = nsOrderMessageDefine::osDedicatedLine;break;
	case 'A':OS = nsOrderMessageDefine::osAPI;break;
	case 'M':OS = nsOrderMessageDefine::osMobile;break;
	case 'W':OS = nsOrderMessageDefine::osWeb;break;
	case 'P':OS = nsOrderMessageDefine::osPCApplication;break;
	case 'V':OS = nsOrderMessageDefine::osVoice;break;
        case 'G':OS = nsOrderMessageDefine::osGeneral;break;
    }
    UFC::BufferedLog::Printf( " OrderSource[%c] index[%d]", ch, (int)OS );
    return OS;
}
//------------------------------------------------------------------------------
extern UFC::AnsiString FLogonID;
extern UFC::AnsiString GBrokerID;
extern UFC::PInt32     FRecoverBeginTime;
extern Int32           FOverlapTime;
Int32  WAIT_US = 100;
//------------------------------------------------------------------------------
TFubonTAIFEXConnection::TFubonTAIFEXConnection( const UFC::AnsiString& ConfigFileName )
:UFC::PThread( NULL, FALSE )
,FTFXCMID( "" )
,FDebug( "" )
,FLogFile( "" )
,FAppName( "" )
,FIP( "" )
,FPort( 0 )
,FID( FLogonID )
,FPassword( "" )
,FTradeTimeout( 60 )
,FRecoverCount( 0 )
,FCheckTradeDate( TRUE )
,FToday( "" )
,FConnection( NULL )
,FConnectionState( focDisconnect )
{
    UFC::GetTradeYYYYMMDD( FToday );
    ReadConfigFile( ConfigFileName );
    LockFile();
        
    FReadBuffer   = new char[ 1024 ];
    FConnection   = new TTaifexConnection( FAppName, this );
    FOrderQueue   = new TSysque( "", 0, FOrderQueName );
    FConfirmQueue = new TSysque( "", 0, FConfirmQueName );
    FFilledQueue  = new TSysque( "", 0, FFilledQueName );
    FOHConfirmQueue = new TSysque( "", 0, FOHConfirmQueName );
    FOHFilledQueue  = new TSysque( "", 0, FOHFilledQueName );

    if( FOrderQueue->Open() && FConfirmQueue->Open() && FFilledQueue->Open() && 
        FOHConfirmQueue->Open() && FOHFilledQueue->Open() )
    {
        FConnection->SetUseNewFuturesSymbol( TRUE ); 
        FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, GBrokerID.c_str() );
        FConnection->SetCMID( FTFXCMID.c_str() );
        ///< Load TAIFEX message format config files.
        FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
        FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );                                
        
        Start();
    }
    else
    {
        UFC::BufferedLog::Printf( " Open sysqueue failed." );
        exit(0);    
    }
}
//------------------------------------------------------------------------------
TFubonTAIFEXConnection::~TFubonTAIFEXConnection( void )
{
    if( FConnection != NULL )
        delete FConnection;   
    if( FOrderQueue != NULL )
        delete FOrderQueue;    
    if( FConfirmQueue != NULL )
        delete FConfirmQueue;    
    if( FFilledQueue != NULL )
        delete FFilledQueue;  
    if( FOHConfirmQueue != NULL )
        delete FOHConfirmQueue;    
    if( FOHFilledQueue != NULL )
        delete FOHFilledQueue;  
    for( int i = 0; i < FIPPool.ItemCount(); i++ )
    {
        UFC::AnsiString* str = FIPPool.DequeueNoWait();
        if( str )
            delete str;
    }
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::LockFile( void )
{
    UFC::AnsiString LockFileName;
    
    LockFileName.Printf( "%sFFOC_%s_%s_lock", FLockFile.c_str(), GBrokerID.c_str(), FLogonID.c_str()  );
    UFC::BufferedLog::DebugPrintf( " Try to open and lock file:[%s].", LockFileName.c_str() );
    
    int fd = open( LockFileName.c_str(), O_WRONLY | O_CREAT, 664 );//0600 );
    if( fd > 0 )
    {
        flock fl;
        fl.l_type   = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start  = 0;
        fl.l_len    = 0;
        fl.l_pid    = getpid();
        
        if( fcntl( fd, F_SETLKW, &fl ) == 0 )
            UFC::BufferedLog::DebugPrintf( " File:[%s] locked.", LockFileName.c_str() );
        else
        {
            UFC::BufferedLog::DebugPrintf( " Unable to lock file:[%s].", LockFileName.c_str() );
            exit(0);
        }
    }
    else
    {
        UFC::BufferedLog::DebugPrintf( " Can not open file:[%s]", LockFileName.c_str() );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::SetLogObject( void )
{
    UFC::AnsiString LogFileName;
    LogFileName.Printf( "%s%s.%s.log", FLogFile.c_str(),FAppName.c_str(), FToday.c_str() );

    UFC::BufferedLog::DebugPrintf( " Write log to file:[%s]", LogFileName.c_str() );    
    UFC::BufferedLog::SetDebugMode( (FDebug == "1") );
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogFileName, 102400, TRUE ) );
}
//------------------------------------------------------------------------------
//
// Config file sample:
//
// [main]
// debug =0
// log_path=../log
// TAIFEX_CMID=S889
//
// [speedy]
// ip=192.168.0.3,192.168.0.120
// id=S889000
// port=23456
// password=1234rewq
//
// [sysque]
// trade_timeout=60
//
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::ReadConfigFile( const UFC::AnsiString& ConfigFileName )
{
    try
    {
        UFC::UiniFile iniFile( ConfigFileName );
        UFC::AnsiString Value;

        iniFile.GetValue( "main", "debug", FDebug );                        
        FAppName.Printf( "FTC_TFX_%s_%s",GBrokerID.c_str(), FID.c_str() );
        if( iniFile.GetValue( "main", "TAIFEX_CMID", FTFXCMID ) == FALSE )         
            FTFXCMID = GBrokerID.SubString( 0, 4 );        
        if( iniFile.GetValue( "main", "CheckTradeDate", Value ) == TRUE )                 
            FCheckTradeDate = Value.ToInt();
        iniFile.GetValue( "speedy", "ip", FIP );
        iniFile.GetValue( "speedy", "port", Value );
        FPort = Value.ToInt();
        Value = "";
        iniFile.GetValue( "speedy", "password", FPassword );             
        iniFile.GetValue( "speedy", "lock_path", FLockFile );             
        iniFile.GetValue( "sysque", "trade_timeout", Value );
        FTradeTimeout = Value.ToInt();
        Value = "";
        if( iniFile.GetValue( "sysque", "wait_us", Value ) == true )        
            WAIT_US = Value.ToInt();
        Value = "";

        if( FLockFile.Length() <= 0 )
            FLockFile = "./";                    
        if( FDebug.Length() <= 0 )
        {
            FDebug = "0";
            iniFile.SetValue( "main", "debug", FDebug );
        }        
        if( FLogFile.Length() <= 0 )
        {
            FLogFile = "../log/";
            iniFile.SetValue( "main", "log_path", FLogFile );
        }                
        if( FIP.Length() <= 0 )
        {
            FIP = "127.0.0.1";
            iniFile.SetValue( "speedy", "ip", FIP );
        }
        if( FPort <= 0 )
        {
            Value = "23456";
            FPort = Value.ToInt();
            iniFile.SetValue( "speedy", "port", Value );
        }
        if( FPassword.Length() <= 0 )
        {
            FPassword = "mdbs";
            iniFile.SetValue( "speedy", "password", FPassword );
        }
        if( FTradeTimeout <= 0 )
        {
            Value = "60";
            FTradeTimeout = Value.ToInt();
            iniFile.SetValue( "sysque", "trade_timeout", Value );
        }                
        FOrderQueName.Printf( "%s_send", GBrokerID.c_str() );
        
        FConfirmQueName.Printf( "%s_recv", GBrokerID.c_str() );
        FFilledQueName.Printf( "%s_recv", GBrokerID.c_str() );
        
        FOHConfirmQueName.Printf( "%s_recv1", GBrokerID.c_str() );
        FOHFilledQueName.Printf( "%s_recv1", GBrokerID.c_str() );
        SetLogObject();        
        
        UFC::BufferedLog::Printf(" _________________________________________________");
        UFC::BufferedLog::Printf("                                                  ");
        UFC::BufferedLog::Printf("    Fubon TAIFEX Order Connection                 ");
        UFC::BufferedLog::Printf("    Copyright 2004-%d by MDBS Software Inc.       ",UFC::ThisYear());
        UFC::BufferedLog::Printf("    All right reserved.                           ");
        UFC::BufferedLog::Printf("                                                  ");
        UFC::BufferedLog::Printf("    Ver:%s Build Date:%s","1.2",__DATE__ );
        UFC::BufferedLog::Printf("                                             ");
        UFC::BufferedLog::Printf( " [ main ]" );
        UFC::BufferedLog::Printf( " debug            = [%s]", FDebug.c_str() );        
        UFC::BufferedLog::Printf( " BrokerID         = [%s]", GBrokerID.c_str() );        
        UFC::BufferedLog::Printf( " TAIFEX_CMID      = [%s]", FTFXCMID.c_str() );
        UFC::BufferedLog::Printf( " lock_path        = [%s]", FLockFile.c_str() );        
        UFC::BufferedLog::Printf( " CheckTradeDate   = [%s]", (int)FCheckTradeDate );                
        UFC::BufferedLog::Printf( " " );        
        UFC::BufferedLog::Printf( " [speedy]" );
        UFC::BufferedLog::Printf( " AppName          = [%s]", FAppName.c_str() );
        UFC::BufferedLog::Printf( " ip               = [%s]", FIP.c_str() );
        UFC::BufferedLog::Printf( " port             = [%d]", FPort );
        UFC::BufferedLog::Printf( " id               = [%s]", FID.c_str() );
        UFC::BufferedLog::Printf( " password         = [%s]", FPassword.c_str() );        
        UFC::BufferedLog::Printf( " " );
        UFC::BufferedLog::Printf( " [sysque]" );       
        UFC::BufferedLog::Printf( " order_queue_name   = [%s]", FOrderQueName.c_str() );
        UFC::BufferedLog::Printf( " confirm_queue_name = [%s]", FConfirmQueName.c_str() );
        UFC::BufferedLog::Printf( " filled_queue_name  = [%s]", FFilledQueName.c_str() );        
        UFC::BufferedLog::Printf( " off_hour_confirm_queue_name = [%s]", FOHConfirmQueName.c_str() );
        UFC::BufferedLog::Printf( " off_hour_filled_queue_name  = [%s]", FOHFilledQueName.c_str() );        
        UFC::BufferedLog::Printf( " trade_timeout      = [%d]", FTradeTimeout );
        UFC::BufferedLog::Printf( " wait_us            = [%d]", WAIT_US );
        UFC::BufferedLog::Printf( " " );
        UFC::BufferedLog::Printf(" _________________________________________________");                        
        
    }
    catch( UFC::Exception& ex )
    {
        UFC::BufferedLog::DebugPrintf( " %s\n", ex.what() );
    }
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " Logon( id=[%s], password=[%s] )", FID.c_str(), FPassword.c_str() );
    SetConnectionState( focConnected );        
    FConnection->Logon( FID.c_str(), FPassword.c_str(), "0000000" );
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *Connection broken, Wait for 2 sec to reconnect." );    
    SetConnectionState( focDisconnect );
    UFC::SleepMS( 2000 );
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::OnLogonReply( const char* ReplyString, LogonResult IsAccepted, int CID )
{
    if( IsAccepted == lrOk )
    {
        UFC::BufferedLog::Printf( " %s( CID=%d )....logon ok", ReplyString, CID );        
        UFC::AnsiString BeginTime;
        UFC::UTime      RecoverTime; ///< Create last time.        
        Int32           LastTime = FRecoverBeginTime.ToInt32(); 
        
        RecoverTime.setHour( LastTime / 10000 );
        RecoverTime.setMinute( (LastTime % 10000) / 100 );
        RecoverTime.setSecond(  LastTime % 100 );
        RecoverTime -= FOverlapTime;
        LastTime =  (RecoverTime.getHour()*10000) + (RecoverTime.getMinute()* 100) + RecoverTime.getSecond();
        
        BeginTime.Printf( "%06d",  LastTime );                
        UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdBoth,mOptions & mFutures )", BeginTime.c_str() );
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmFutures );
        FConnection->Recover( BeginTime.c_str(), rdBoth,rmOption );        
    }
    else
    {
        UFC::BufferedLog::Printf( " %s....logon failed", ReplyString );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::Recover( UFC::AnsiString BeginTime )
{
    if( GetConnectionState() == focReady )
    {
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmFutures );
        FConnection->Recover( BeginTime.c_str(), rdBoth,rmOption );        
        UFC::BufferedLog::Printf( " Recover from:%s", BeginTime.c_str() );
    }
}    
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::OnRecoverFinished( int Count )
{
    FRecoverCount++;
    UFC::BufferedLog::DebugPrintf( " TFubonTAIFEXConnection::OnRecoverFinished( Count = %d )", Count );
    
    if( FRecoverCount == 2 )
    {
        SetConnectionState( focReady );        
        UFC::BufferedLog::DebugPrintf( " Ready to place order!");        
        FRecoverCount = 0;
        UFC::SleepMS( 1000 );
    }
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::TFXOrderStatus( TExecutionReportMessage* Msg, TSQMessage& SQMessage )
{
    int Src3 = UFC::AnsiString( Msg->GetSrc(), 3 ).ToInt();
            
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osPendingNew:        		
            SQMessage.Append( "00", "2" );
            SQMessage.Append( "14", "I" );
            SQMessage.Append( "19", "S" );
            break;
        case nsOrderMessageDefine::osPendingReplace:
            SQMessage.Append( "00", "3" );
            SQMessage.Append( "14", "C" );
            SQMessage.Append( "19", "R" );
            break;
        case nsOrderMessageDefine::osPendingCancel:
            SQMessage.Append( "00", "3" );
            SQMessage.Append( "14", "D" );
            SQMessage.Append( "19", "R" );
            break;
        case nsOrderMessageDefine::osQuoteAccept:
        case nsOrderMessageDefine::osNew:
            if( Msg->GetExecType( ) == nsOrderMessageDefine::etOrderStatus )
                SQMessage.Append( "00", "9" );
            else
                SQMessage.Append( "00", "3" );        	
            SQMessage.Append( "14", "I" );
            SQMessage.Append( "19", "R" );
            break;
        case nsOrderMessageDefine::osReplaced:
            SQMessage.Append( "00", "4" );
            if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||  Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )             
                SQMessage.Append( "14", "M" );
            else
                SQMessage.Append( "14", "C" );
            SQMessage.Append( "19", "R" );
            break;
        case nsOrderMessageDefine::osCanceled:
            SQMessage.Append( "00", "4" );
            SQMessage.Append( "14", "D" );
            SQMessage.Append( "19", "R" );
            break;
        case nsOrderMessageDefine::osPartiallyFilled:            
        case nsOrderMessageDefine::osFilled:
            SQMessage.Append( "00", "4" );
            SQMessage.Append( "14", "I" );
            break;
        case nsOrderMessageDefine::osQuoteRequestAccept:
            SQMessage.Append( "00", "3" );
            SQMessage.Append( "14", "I" );
            break;
        case nsOrderMessageDefine::osRejected:
            UFC::BufferedLog::DebugPrintf( " Status:osRejected ResponseTo[%d]",Msg->GetCxlRejResponseTo() );                
            switch( Msg->GetCxlRejResponseTo() )
            {
                case nsOrderMessageDefine::crrQuote:    
                case nsOrderMessageDefine::crrNew:
                    SQMessage.Append( "00", "3" );
                    SQMessage.Append( "14", "I" );
                    SQMessage.Append( "19", "E" );
                    break;
                case nsOrderMessageDefine::crrReplace:
                    if( Src3 == 300 ) ///< Reject in Confirm message
                        SQMessage.Append( "00", "3" );
                    else             ///< Reject in Fill message
                        SQMessage.Append( "00", "4" );
                    if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||  Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )             
                			  SQMessage.Append( "14", "M" );
            				else
                				SQMessage.Append( "14", "C" );                    
                    SQMessage.Append( "19", "E" );
                    break;
                case nsOrderMessageDefine::crrQuoteCancel:
                case nsOrderMessageDefine::crrCancel:
                    if( Src3 == 300 ) ///< Reject in Confirm message
                        SQMessage.Append( "00", "3" );
                    else             ///< Reject in Fill message
                        SQMessage.Append( "00", "4" );
                    SQMessage.Append( "14", "D" );
                    SQMessage.Append( "19", "E" );
                    break;
                case nsOrderMessageDefine::crrOrderStatus:
                    SQMessage.Append( "00", "9" );
                    SQMessage.Append( "14", "I" );
                    SQMessage.Append( "19", "E" );                    
                    break;                    
                case nsOrderMessageDefine::crrQuoteRequest:
                    SQMessage.Append( "00", "3" );
                    SQMessage.Append( "14", "I" );
                    SQMessage.Append( "19", "E" );
                    break;
                default: break;
            }
            break;
        default: break;
    }    
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::QuoteRequestConfirm( TExecutionReportMessage* Msg,  TSQMessage& SQMessage )
{
    UFC::AnsiString PBNO( Msg->GetLINBRN() );
    UFC::AnsiString PVC( Msg->GetPVC() );
    UFC::AnsiString PHost( Msg->GetProcessHost() );
    UFC::AnsiString TimeString;
        
    SQMessage.Append( "29", Msg->GetSymbol() );           
    SQMessage.Append( "40", Msg->GetStatusCode() );            
    SQMessage.Append( "36", Msg->GetOrderID() );
    ///< Sender LineNo 
    SQMessage.Append( "VJ", "00" );
    ///< Line No processed this order.
    SQMessage.Append( "VH", PBNO.SubString(2,2) );
    ///< PVC ID processed this order.
    if( PVC.Length() > 0 ) 
        SQMessage.Append( "VI", PVC.c_str() );
    else
        SQMessage.Append( "VI", "xx" );    	                    
    SQMessage.Append( "21", FToday );    
    TimeString.Printf( "%s", Msg->GetTransactTime());
    TimeString.PadThis( 8, '0' );
    SQMessage.Append( "22", TimeString.c_str() );
    SQMessage.Append( "O3", "3" );            
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsNormal )
        SQMessage.Append( "TS", "0" );    	            
    else if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour )
        SQMessage.Append( "TS", "1" );    	            
    ///SQMessage.Append( "65", "00000000" );                
    UFC::AnsiString MessageString = SQMessage.ToString();
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour )
    {
        FOHConfirmQueue->Write( (char*)(MessageString.c_str()), MessageString.Length() );    
        UFC::BufferedLog::DebugPrintf( " Send off-hour Confirm[%s]",MessageString.c_str() );
    }
    else
    {
        FConfirmQueue->Write( (char*)(MessageString.c_str()), MessageString.Length() );    
        UFC::BufferedLog::DebugPrintf( " Send normal Confirm[%s]",MessageString.c_str() );
    }    
}    
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{
    TSQMessage SQMessage;
    UFC::PLockObject Lock( FAPIIOCS );
    
    UFC::BufferedLog::DebugPrintf( " TSystexOrderConnection::OnExecutionReport" );    
    
    TFXOrderStatus( Msg, SQMessage );
    UFC::AnsiString Account( Msg->GetAccount() );
    UFC::AnsiString AccountCode, AccountChecksum;    
    UFC::AnsiString BrokerID( Msg->GetBrokerID() );    
    UFC::AnsiString PartID;
     ///< Futures/Option    
    UFC::AnsiString TFXType   = BrokerID.SubString( 0, 1 ); ///< F or S
    UFC::AnsiString CompanyID = BrokerID.SubString( 1, 3 );
    UFC::AnsiString BranchID  = BrokerID.SubString( 4, 3 );
    SQMessage.Append( "07", TFXType );
    SQMessage.Append( "08", CompanyID );
    SQMessage.Append( "09", BranchID );    
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTWFutures: SQMessage.Append( "13", "FUT" );break;
        case nsOrderMessageDefine::mTWOptions: SQMessage.Append( "13", "OPT" );break;
        default:break;
    }
    if( Msg->GetOrderStatus() == nsOrderMessageDefine::osQuoteRequestAccept )
    {
        QuoteRequestConfirm( Msg, SQMessage );
        return;
    }               
    AccountCode.Printf( "%06d", Account.ToInt() / 10 );
    AccountChecksum.Printf( "%d", Account.ToInt() % 10 );    
    SQMessage.Append( "17", AccountCode );
    SQMessage.Append( "18", AccountChecksum );
    SQMessage.Append( "27", (long)Msg->GetOrderType() );
     ///< For Futures
    switch( Msg->GetTimeInForce() )
    {    
        case nsOrderMessageDefine::tifIOC:SQMessage.Append( "O4",(long) 0 ); 
                                          SQMessage.Append( "V5",(long) 1 ); 
                                          break;
        case nsOrderMessageDefine::tifFOK:SQMessage.Append( "O4",(long) 1 ); 
                                          SQMessage.Append( "V5",(long) 0 ); 
                                          break;
        default:                          SQMessage.Append( "O4",(long) 0 ); 
                                          SQMessage.Append( "V5",(long) 0 ); 
                                           break;
    } 
    SQMessage.Append( "W3", (long)Msg->GetPositionEffect() );        
    SQMessage.Append( "VT", FConnection->GetCMID() );        
    SQMessage.Append( "29", Msg->GetSymbol() );    
    SQMessage.Append( "36", Msg->GetOrderID() );
    SQMessage.Append( "40", Msg->GetStatusCode() );    
    
    UFC::AnsiString PBNO( Msg->GetLINBRN() );
    UFC::AnsiString PVC( Msg->GetPVC() );
    UFC::AnsiString PHost( Msg->GetProcessHost() );
    UFC::AnsiString Data( Msg->GetData());
    
    ///< Data attach to the order.
    if( Data.Length() > 0 ) 
      	SQMessage.Append( "VK", Data.c_str() );
    ///< Sender LineNo 
    SQMessage.Append( "VJ", "00" );
    ///< Line No processed this order.
    SQMessage.Append( "VH", PBNO.SubString(2,2) );
    ///< PVC ID processed this order.
    if( PVC.Length() > 0 ) 
      	SQMessage.Append( "VI", PVC.c_str() );
    else
        SQMessage.Append( "VI", "xx" );    	            
    
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsNormal )
        SQMessage.Append( "TS", "0" );    	            
    else if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour )
        SQMessage.Append( "TS", "1" );    	            
        
    PartID.Printf( "%03d", Msg->GetTMPPartID());
    SQMessage.Append( "PI", PartID.c_str() );    	            
    int Src3 = UFC::AnsiString( Msg->GetSrc(), 3 ).ToInt();
    UFC::AnsiString LineBrokerID = Msg->GetLINBRN();
        
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osCanceled:
        case nsOrderMessageDefine::osReplaced:
        case nsOrderMessageDefine::osPartiallyFilled:
        case nsOrderMessageDefine::osFilled:
             SQMessage.Append( "32", (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
             SQMessage.Append( "LQ", Msg->GetLeavesQty() );
             OnTFXFilled(  Msg->GetMarket(), LineBrokerID, Msg, SQMessage );
             break;
        case nsOrderMessageDefine::osRejected:
             if( Msg->GetCxlRejResponseTo() == nsOrderMessageDefine::crrCancel || Msg->GetCxlRejResponseTo() == nsOrderMessageDefine::crrReplace )
             {
                 if( Src3 == 300 )
                     OnTFXConfirm( Msg->GetMarket(), GBrokerID, Msg, SQMessage );
                 else
                     OnTFXFilled(  Msg->GetMarket(), LineBrokerID, Msg, SQMessage );
             }
             else
                  OnTFXConfirm( Msg->GetMarket(), GBrokerID, Msg, SQMessage );
             break;
       default:
             SQMessage.Append( "LQ", Msg->GetLeavesQty() );
             OnTFXConfirm( Msg->GetMarket(), GBrokerID, Msg, SQMessage );
             break;        
    }
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::OnTFXConfirm( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& BrokerID, TExecutionReportMessage* Msg, TSQMessage& SQMessage )
{
    MTree ReplyData;
    UFC::AnsiString TimeString;

    SQMessage.Append( "21", FToday );
    TimeString.Printf( "%s", Msg->GetTransactTime());
    TimeString.PadThis( 8, '0' );
    SQMessage.Append( "32", (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
    SQMessage.Append( "22", TimeString.c_str() );
    SQMessage.Append( "30", Msg->GetOrderQty() );
    SQMessage.Append( "42", Msg->GetBeforeQty() );
    SQMessage.Append( "43", Msg->GetAfterQty() );
    SQMessage.Append( "31", Msg->GetPrice() ); 
    if( Msg->GetOrderStatus() == nsOrderMessageDefine::osQuoteAccept )
        SQMessage.Append( "O3", "4" );    
    if( Msg->GetOrderStatus() == nsOrderMessageDefine::osRejected && 
        Msg->GetCxlRejResponseTo() == nsOrderMessageDefine::crrQuoteRequest )
        SQMessage.Append( "O3", "3" );    
    else
    {
        int SymLen = strlen( Msg->GetSymbol() );
        if( SymLen == 5 || SymLen == 10 )
            SQMessage.Append( "O3", "1" );    
        else
            SQMessage.Append( "O3", "2" );    
    }
    UFC::AnsiString ExecID;
    
    ExecID.Printf( "%08d", Msg->GetReportSequence() ); 
    SQMessage.Append( "65", ExecID.c_str() );
    SQMessage.Append( "Q5", ExecID.c_str() );    
    
    UFC::AnsiString MessageString = SQMessage.ToString();
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour )
    {
        FOHConfirmQueue->Write( (char*)(MessageString.c_str()), MessageString.Length() );        
        UFC::BufferedLog::DebugPrintf( " Send off-hour Confirm[%s]",SQMessage.ToString().c_str() );
    }
    else
    {
        FConfirmQueue->Write( (char*)(MessageString.c_str()), MessageString.Length() );        
        UFC::BufferedLog::DebugPrintf( " Send normal Confirm[%s]",SQMessage.ToString().c_str() );
    }
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::OnTFXFilled( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& BrokerID, TExecutionReportMessage* Msg, TSQMessage& SQMessage )
{
    UFC::AnsiString Type( Msg->GetSrc(),1 );
            
    switch( Msg->GetOrderStatus() )
    {
       case nsOrderMessageDefine::osCanceled:
            SQMessage.Append( "47", "0");
            SQMessage.Append( "42", Msg->GetOrderQty() );
            SQMessage.Append( "43", "0");
            break;
       case nsOrderMessageDefine::osReplaced:
            SQMessage.Append( "47", "0");
            SQMessage.Append( "42", Msg->GetBeforeQty() );
            SQMessage.Append( "43", Msg->GetAfterQty() );
            break;
       case nsOrderMessageDefine::osPartiallyFilled:
       case nsOrderMessageDefine::osFilled:
            SQMessage.Append( "47", Msg->GetOrderQty() );
            SQMessage.Append( "42", Msg->GetBeforeQty() );
            SQMessage.Append( "43", Msg->GetAfterQty() );
            break;
       default:break;
    }    
    SQMessage.Append( "31", Msg->GetPrice() ); 
    SQMessage.Append( "48", Msg->GetPrice() );
    SQMessage.Append( "50", FToday );
    SQMessage.Append( "51", Msg->GetTransactTime() );
    SQMessage.Append( "72", Msg->GetRecNo() );        
    if( Type =="1" || Type =="2")
        SQMessage.Append( "O3", Type.c_str() );        
    else
    {
        int SymLen = strlen( Msg->GetSymbol() );
        if( SymLen == 5 || SymLen == 10 )
            SQMessage.Append( "O3", "1" );    
        else
            SQMessage.Append( "O3", "2" );    
    }
    UFC::AnsiString ExecID;
    
    ExecID.Printf( "%08d", Msg->GetReportSequence() ); 
    SQMessage.Append( "Q5", ExecID.c_str() );    
    
    if( Type == "2" )///< Multileg 
    {
        SQMessage.Append( "33", Msg->GetLegSymbol1() );    
        SQMessage.Append( "V9", Msg->GetLegSymbol2() );    
        if( Msg->GetLegSide1( ) == nsOrderMessageDefine::sBuy ) 
            SQMessage.Append( "EL","B" );    
        else
            SQMessage.Append( "EL","S" );    
        SQMessage.Append( "W4", Msg->GetLegPrice1() );    
        SQMessage.Append( "W5", Msg->GetLegPrice2() );    
        SQMessage.Append( "NA", Msg->GetLegQty1());    
        SQMessage.Append( "NB", Msg->GetLegQty2());    
    }
    SQMessage.Append( "HR", Msg->GetAccountFlag() );
    UFC::AnsiString MessageString = SQMessage.ToString();
    
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOffHour )
    {
        FOHFilledQueue->Write( (char*)(MessageString.c_str()), MessageString.Length() );    
        UFC::BufferedLog::DebugPrintf( " Send off-hour Fill[%s]",SQMessage.ToString().c_str() );
    }
    else
    {
        FFilledQueue->Write( (char*)(MessageString.c_str()), MessageString.Length() );    
        UFC::BufferedLog::DebugPrintf( " Send normal Fill[%s]",SQMessage.ToString().c_str() );
    }
}
//------------------------------------------------------------------------------
//  Order listener
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::OnData( void )
{
    TSQMessage      SQMessage;
    UFC::AnsiString MessageString( FReadBuffer );
    
    SQMessage.FromString( MessageString );
    UFC::BufferedLog::DebugPrintf( " Order Data[%s]",MessageString.c_str() );          
    if( CheckTradeDate(SQMessage) ) 
    {
        if( CheckTradeTime(SQMessage) ) 
        {
            UFC::AnsiString OrderType, Session;
            
            if( SQMessage.Get( "14", OrderType ) && SQMessage.Get( "O3", Session )) 
            {
                switch( OrderType[ 0 ] )
                {
                    case 'I': /// 1:NewOrder 2:Multi-leg 3:QuoteRequest 4:Quote
                              if( Session == "3" )
                                  QuoteRequest( SQMessage );
                              else if( Session == "4" )
                                  NewQuote( SQMessage );
                              else
                                  New( SQMessage );
                              break;
                    case 'D': if( Session == "4" )
                                  QuoteCancel( SQMessage );
                              else
                                  Cancel( SQMessage );
                              break;
                    case 'm':
                    case 'M': Replace( SQMessage, TRUE ); ///< Replace Px 
                              break;                    
                    case 'C': Replace( SQMessage, FALSE );///< Replace Qty
                              break;                    
                    case 'Q': OrderStatusRequest( SQMessage );
                              break;
                    default:  UFC::BufferedLog::Printf( " OrderType 14=%s not handle.", OrderType.c_str() );
                              break;  
                }
            }
            else
                UFC::BufferedLog::Printf( " Missing requried tag(14 or 03)");
        }
        else
        {
            RejectOvertimeOrder( SQMessage );
            UFC::BufferedLog::Printf( " Order Expired.");
        }            
    }
    else
    {
        RejectOvertimeOrder( SQMessage );
        UFC::BufferedLog::Printf( " Wrong trade date.");
    }
        
 }
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::TFXNew( TSQMessage& SQMessage, TNewOrderMessage& Msg )
{
    UFC::AnsiString FOK, IOC, OrderType,PosEffect,CM,Quantity,AccountFlag, IsOffHour;
    ///< Set Clear member ID.
    if( SQMessage.Get( "VT", CM ) ) 
        FConnection->SetCMID( CM );        
    ///< Set Order Type ( Limit or market )
    if( SQMessage.Get( "27", OrderType ) ) 
    {
        switch( OrderType.ToInt() )
        {
            case 1: Msg.SetOrderType( nsOrderMessageDefine::otMarket );break;
            case 3: Msg.SetOrderType( nsOrderMessageDefine::otMarketWithProtection);break;
            case 2:
            default:Msg.SetOrderType( nsOrderMessageDefine::otLimit );break;
        }
    }
    else
    {
        Msg.SetOrderType( nsOrderMessageDefine::otLimit );
        UFC::BufferedLog::DebugPrintf( " Missing OrderType tag(27) use default:otLimit." );
    }
    ///< Set TimeInForce ( FOK,IOC,ROD )
    if( SQMessage.Get( "V5", IOC ) &&  SQMessage.Get( "O4", FOK ) ) 
    {
        if( FOK.ToInt() == 1 )
            Msg.SetTimeInForce( nsOrderMessageDefine::tifFOK );
        else if( IOC.ToInt() == 1 )
            Msg.SetTimeInForce( nsOrderMessageDefine::tifIOC );
        else
            Msg.SetTimeInForce( nsOrderMessageDefine::tifROD );
    }
    else 
    {
        UFC::BufferedLog::DebugPrintf( " Missing TimeInForce tag(V5 or O4)" );
        return;
    }
    ///< Set PositionEffect ( Open,Close,DayTrade )
    if( SQMessage.Get( "W3", PosEffect ) )    
    {
        if( PosEffect == "1")        
            Msg.SetPositionEffect(nsOrderMessageDefine::peClose);
        else if( PosEffect == "2")        
            Msg.SetPositionEffect(nsOrderMessageDefine::peDayTrade);
        else if( PosEffect == "3")        
            Msg.SetPositionEffect(nsOrderMessageDefine::peDayTradeOpen );
        else if( PosEffect == "7")        
            Msg.SetPositionEffect(nsOrderMessageDefine::peForceClose);
        else if( PosEffect == "9")        
            Msg.SetPositionEffect(nsOrderMessageDefine::peTMPMarketMaker );
        else
            Msg.SetPositionEffect(nsOrderMessageDefine::peOpen);                
    }
    else 
    {
        UFC::BufferedLog::DebugPrintf( " Missing PositionEffect tag(W3)" );
        return;
    }
    if( SQMessage.Get( "HR", AccountFlag ) )    
    {
        Msg.SetAccountFlag( AccountFlag.c_str() );
    }
    else 
    {
        UFC::BufferedLog::DebugPrintf( " Missing account flag tag(HR)" );
        return;
    }
    if( SQMessage.Get( "30", Quantity ) == FALSE )
        Quantity = "0";
    Msg.SetOrderQty( Quantity.ToInt() );
    if( SQMessage.Get( "TS", IsOffHour ) == TRUE )
    {
        switch( IsOffHour[ 0 ] )
        {
            case '0': Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );break;
            case '1': Msg.SetTradingSessionID( nsOrderMessageDefine::tsOffHour );break;
            default:  Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuto );break;
        }
    }
    else
        Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuto );    
    FConnection->NewOrder( &Msg );
}        
//------------------------------------------------------------------------------
nsOrderMessageDefine::MarketEnum TFubonTAIFEXConnection::GetMarket( TSQMessage& SQMessage )
{
    UFC::AnsiString Market;
    
    if( SQMessage.Get( "13", Market ) == TRUE) 
    {
        if( Market == "FUT" )
            return nsOrderMessageDefine::mTWFutures;            
        else if( Market == "OPT" )
            return nsOrderMessageDefine::mTWOptions;        
    }                                 
    UFC::BufferedLog::DebugPrintf( " Invalid or missing Market tag(13) use default mFutures." );        
    return nsOrderMessageDefine::mTWFutures;            
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::New( TSQMessage& SQMessage )
{
    UFC::BufferedLog::DebugPrintf( " TSystexOrderConnection::New" );        
    TNewOrderMessage Msg;
    UFC::AnsiString TFXSys, CompanyID, BranchID, AccountCode, AccountChecksum, Symbol, Quantity, Price, Side, OrderID;
    
    if( SQMessage.Get( "07", TFXSys ) &&
        SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "17", AccountCode ) &&
        SQMessage.Get( "18", AccountChecksum ) &&
        SQMessage.Get( "29", Symbol ) &&
        SQMessage.Get( "30", Quantity ) &&
        SQMessage.Get( "31", Price ) &&
        SQMessage.Get( "32", Side ) &&
        SQMessage.Get( "36", OrderID ) ) 
    {
        UFC::AnsiString BrokerID, Account, Data,OS,IS;
        nsOrderMessageDefine::MarketEnum Market = GetMarket( SQMessage );        
        
        Msg.SetMarket( Market );        
        BrokerID.Printf( "%s%s%s", TFXSys.c_str(), CompanyID.c_str(), BranchID.c_str() );
        Account.Printf( "%s%s", AccountCode.c_str(), AccountChecksum.c_str() );        
        Msg.SetBrokerID( BrokerID.c_str() );
        Msg.SetAccount( Account.c_str() );        
        Msg.SetSymbol( Symbol.c_str() );
        Msg.SetPrice( Price.ToDouble() );        
        Msg.SetSide( (Side == "B") ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
        Msg.SetOrderID( OrderID.c_str() );                
        if( SQMessage.Get( "VK", Data ) )
            Msg.SetData( Data.c_str() );       
        if( SQMessage.Get( "OS", OS ) && OS.Length() >= 1 )
            Msg.SetOrderSource( ToOrderSourceEnum( OS[0]) );
        if( SQMessage.Get( "IS", IS ) && IS.Length() == 3 )
            Msg.SetMarketDataSource( IS.c_str() );
        TFXNew( SQMessage, Msg );
    }
    else
        UFC::BufferedLog::DebugPrintf( " Missing required tag(08,09,17,18,29,30,31,32,36 or 03)." );        
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::NewQuote( TSQMessage& SQMessage )
{
    UFC::BufferedLog::DebugPrintf( " TSystexOrderConnection::NewQuote" );        
    TQuoteMessage Msg;
    UFC::AnsiString TFXSys, CompanyID, BranchID, AccountCode, AccountChecksum, Symbol, BidQty, AskQty, Price, Side, OrderID, BidPx, AskPx;
    
    if( SQMessage.Get( "07", TFXSys ) &&
        SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "17", AccountCode ) &&
        SQMessage.Get( "18", AccountChecksum ) &&
        SQMessage.Get( "29", Symbol ) &&
        SQMessage.Get( "30", BidQty ) &&
        SQMessage.Get( "EM", AskQty ) &&
        SQMessage.Get( "T4", BidPx ) &&
        SQMessage.Get( "T5", AskPx ) &&
        SQMessage.Get( "36", OrderID ) ) 
    {
        UFC::AnsiString BrokerID, Account, Data;
        UFC::AnsiString FOK, IOC, CM, AccountFlag;        
        nsOrderMessageDefine::MarketEnum Market = GetMarket( SQMessage );        
        
        
        Msg.SetMarket( Market );        
        BrokerID.Printf( "%s%s%s", TFXSys.c_str(), CompanyID.c_str(), BranchID.c_str() );
        Account.Printf( "%s%s", AccountCode.c_str(), AccountChecksum.c_str() );        
        Msg.SetBrokerID( BrokerID.c_str() );
        Msg.SetAccount( Account.c_str() );        
        Msg.SetSymbol( Symbol.c_str() );
        Msg.SetBidPrice( BidPx.ToDouble() );        
        Msg.SetAskPrice( AskPx.ToDouble() );                
        Msg.SetBidQty( BidQty.ToInt() );
        Msg.SetAskQty( AskQty.ToInt() );
        Msg.SetQuoteID( OrderID.c_str() );                
        if( SQMessage.Get( "VK", Data ) )
            Msg.SetData( Data.c_str() );       
        ///< Set Clear member ID.
        if( SQMessage.Get( "VT", CM ) ) 
            FConnection->SetCMID( CM );            
        ///< Set TimeInForce ( Q, ROD )
        if( SQMessage.Get( "V5", IOC ) &&  SQMessage.Get( "O4", FOK ) ) 
        {
            if( FOK.ToInt() == 1 ||  IOC.ToInt() == 1 )
                Msg.SetTimeInForce( nsOrderMessageDefine::tifTFXQ );            
            else
                Msg.SetTimeInForce( nsOrderMessageDefine::tifROD );
        }
        else 
        {
            UFC::BufferedLog::DebugPrintf( " Missing TimeInForce tag(V5 or O4)" );
            return;
        }    
        if( SQMessage.Get( "HR", AccountFlag ) == FALSE )            
        {
            UFC::BufferedLog::DebugPrintf( " Missing account flag tag(HR)" );
            return;
        }
        else
            Msg.SetAccountFlag( AccountFlag.c_str() );        
        FConnection->Quote( &Msg );
    }
    else
        UFC::BufferedLog::DebugPrintf( " Missing required tag(08,09,17,18,29,30,EM,36 or 03)." );        
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::QuoteRequest( TSQMessage& SQMessage )
{
    UFC::BufferedLog::DebugPrintf( " TSystexOrderConnection::QuoteRequest" );        
    TNewOrderMessage Msg;
    UFC::AnsiString TFXSys, CompanyID, BranchID, AccountCode, AccountChecksum, Symbol, Quantity, Price, Side, OrderID,IsOffHour;
    
    if( SQMessage.Get( "07", TFXSys ) &&
        SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "29", Symbol ) && 
        SQMessage.Get( "36", OrderID ) )
    {
        UFC::AnsiString BrokerID, Data;
        TQuoteRequestMessage Msg;
        nsOrderMessageDefine::MarketEnum Market = GetMarket( SQMessage );        
        
        Msg.SetMarket( Market );        
        BrokerID.Printf( "%s%s%s", TFXSys.c_str(), CompanyID.c_str(), BranchID.c_str() );
        Msg.SetBrokerID( BrokerID.c_str() );
        Msg.SetSymbol( Symbol.c_str() );
        Msg.SetOrderID( OrderID.c_str() );
        if( SQMessage.Get( "TS", IsOffHour ) == TRUE )
        {
            switch( IsOffHour[ 0 ] )
            {
                case '0': Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );break;
                case '1': Msg.SetTradingSessionID( nsOrderMessageDefine::tsOffHour );break;
                default:  Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuto );break;
            }
        }
        else
            Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuto );
        FConnection->QuoteRequest( &Msg );
    }
    else
        UFC::BufferedLog::DebugPrintf( " Missing required tag(07,08,09,29,36)." );        
}    
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::TFXReplace( TSQMessage& SQMessage, TReplaceOrderMessage& Msg,BOOL IsReplacePx )
{
    UFC::AnsiString FOK, IOC, OrderType,PosEffect,CM,Quantity,AccountFlag,IsOffHour;
    ///< Set Clear member ID.
    if( SQMessage.Get( "VT", CM ) ) 
        FConnection->SetCMID( CM );        
    ///< Set Order Type ( Limit or market )
    if( SQMessage.Get( "27", OrderType ) ) 
    {
        switch( OrderType.ToInt() )
        {
            case 1: Msg.SetOrderType( nsOrderMessageDefine::otMarket );break;
            case 3: Msg.SetOrderType( nsOrderMessageDefine::otMarketWithProtection);break;
            case 2:
            default:Msg.SetOrderType( nsOrderMessageDefine::otLimit );break;
        }
    }
    else
    {
        Msg.SetOrderType( nsOrderMessageDefine::otLimit );
        UFC::BufferedLog::DebugPrintf( " Missing OrderType tag(27) use default:otLimit." );
    }
    ///< Set TimeInForce ( FOK,IOC,ROD )
    if( SQMessage.Get( "V5", IOC ) &&  SQMessage.Get( "O4", FOK ) ) 
    {
        if( FOK.ToInt() == 1 )
            Msg.SetTimeInForce( nsOrderMessageDefine::tifFOK );
        else if( IOC.ToInt() == 1 )
            Msg.SetTimeInForce( nsOrderMessageDefine::tifIOC );
        else
            Msg.SetTimeInForce( nsOrderMessageDefine::tifROD );
    }
    else 
    {
        UFC::BufferedLog::DebugPrintf( " Missing TimeInForce tag(V5 or O4)" );
        return;
    }
    ///< Set PositionEffect ( Open,Close,DayTrade )
    if( SQMessage.Get( "W3", PosEffect ) )    
    {
        if( PosEffect == "1")        
            Msg.SetPositionEffect(nsOrderMessageDefine::peClose);
        else if( PosEffect == "2")        
            Msg.SetPositionEffect(nsOrderMessageDefine::peDayTrade);
        else if( PosEffect == "3")        
            Msg.SetPositionEffect(nsOrderMessageDefine::peDayTradeOpen );
        else if( PosEffect == "7")        
            Msg.SetPositionEffect(nsOrderMessageDefine::peForceClose);
        else if( PosEffect == "9")        
            Msg.SetPositionEffect(nsOrderMessageDefine::peTMPMarketMaker );
        else if( PosEffect == "A")        
            Msg.SetPositionEffect(nsOrderMessageDefine::peTMPMarketMaker );
        else
            Msg.SetPositionEffect(nsOrderMessageDefine::peOpen);        
    }
    else 
    {
        UFC::BufferedLog::DebugPrintf( " Missing PositionEffect tag(W3)" );
        return;
    }
    if( SQMessage.Get( "HR", AccountFlag ) )    
    {
        Msg.SetAccountFlag( AccountFlag.c_str() );
    }
    else 
    {
        UFC::BufferedLog::DebugPrintf( " Missing account flag tag(HR)" );
        return;
    }
    if( IsReplacePx == FALSE )
    {
        if( SQMessage.Get( "30", Quantity ) == FALSE ) 
            Msg.SetOrderQty( 0 );
        else
            Msg.SetOrderQty( Quantity.ToInt() );
    }
    else
        Msg.SetOrderQty( 0 );        
    if( SQMessage.Get( "TS", IsOffHour ) == TRUE )
    {
        switch( IsOffHour[ 0 ] )
        {
            case '0': Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );break;
            case '1': Msg.SetTradingSessionID( nsOrderMessageDefine::tsOffHour );break;
            default:  Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuto );break;
        }
    }
    else
        Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuto );
    FConnection->ReplaceOrder( &Msg );
}        
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::Replace( TSQMessage& SQMessage, BOOL IsReplacePx )
{
    UFC::BufferedLog::DebugPrintf( " TSystexOrderConnection::Replace" );
    TReplaceOrderMessage Msg;
    UFC::AnsiString TFXSys,CompanyID, BranchID, AccountCode, AccountChecksum, Symbol, Quantity, Price, Side, OrderID;
        
    if( SQMessage.Get( "07", TFXSys ) && 
        SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "17", AccountCode ) &&
        SQMessage.Get( "18", AccountChecksum ) &&
        SQMessage.Get( "29", Symbol ) &&
        //SQMessage.Get( "30", Quantity ) &&
        SQMessage.Get( "31", Price ) &&
        SQMessage.Get( "32", Side ) &&
        SQMessage.Get( "36", OrderID ) ) 
    {
        UFC::AnsiString  BrokerID, Account, Data,OS,IS;
        nsOrderMessageDefine::MarketEnum Market = GetMarket( SQMessage );        
        
        Msg.SetMarket( Market );
        BrokerID.Printf( "%s%s%s", TFXSys.c_str(), CompanyID.c_str(), BranchID.c_str() );        
        Account.Printf( "%s%s", AccountCode.c_str(), AccountChecksum.c_str() );        
        Msg.SetBrokerID( BrokerID.c_str() );
        Msg.SetAccount( Account.c_str() );        
        Msg.SetSymbol( Symbol.c_str() );
        Msg.SetPrice( Price.ToDouble() );        
        Msg.SetSide( (Side == "B") ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
        Msg.SetOrderID( OrderID.c_str() );                
        if( SQMessage.Get( "VK", Data ) )
            Msg.SetData( Data.c_str() );      
        if( SQMessage.Get( "OS", OS ) && OS.Length() >= 1 )
            Msg.SetOrderSource( ToOrderSourceEnum( OS[ 0 ]) );
        if( SQMessage.Get( "IS", IS ) && IS.Length() == 3 )
            Msg.SetMarketDataSource( IS.c_str() );
        TFXReplace( SQMessage, Msg, IsReplacePx );
    }
    else
        UFC::BufferedLog::DebugPrintf( " Missing required tag(08,09,17,18,29,30,31,32,36 or 03)." );        
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::TFXCancel( TSQMessage& SQMessage, TCancelOrderMessage& Msg )
{
    UFC::AnsiString FOK, IOC, OrderType,PosEffect,CM,AccountFlag,IsOffHour;
    ///< Set Clear member ID.
    if( SQMessage.Get( "VT", CM ) ) 
        FConnection->SetCMID( CM );        
    ///< Set Order Type ( Limit or market )
    if( SQMessage.Get( "27", OrderType ) ) 
    {
        switch( OrderType.ToInt() )
        {
            case 1: Msg.SetOrderType( nsOrderMessageDefine::otMarket );break;
            case 3: Msg.SetOrderType( nsOrderMessageDefine::otMarketWithProtection);break;
            case 2:
            default:Msg.SetOrderType( nsOrderMessageDefine::otLimit );break;
        }
    }
    else
    {
        Msg.SetOrderType( nsOrderMessageDefine::otLimit );
        UFC::BufferedLog::DebugPrintf( " Missing OrderType tag(27) use default:otLimit." );
    }
    ///< Set TimeInForce ( FOK,IOC,ROD )
    if( SQMessage.Get( "V5", IOC ) &&  SQMessage.Get( "O4", FOK ) ) 
    {
        if( FOK.ToInt() == 1 )
            Msg.SetTimeInForce( nsOrderMessageDefine::tifFOK );
        else if( IOC.ToInt() == 1 )
            Msg.SetTimeInForce( nsOrderMessageDefine::tifIOC );
        else
            Msg.SetTimeInForce( nsOrderMessageDefine::tifROD );
    }
    else 
    {
        UFC::BufferedLog::DebugPrintf( " Missing TimeInForce tag(V5 or O4)" );
        return;
    }    
    if( SQMessage.Get( "HR", AccountFlag ) )    
    {
        Msg.SetAccountFlag( AccountFlag.c_str() );
    }
    else 
    {
        UFC::BufferedLog::DebugPrintf( " Missing account flag tag(HR)" );
        return;
    }
    if( SQMessage.Get( "TS", IsOffHour ) == TRUE )
    {
        switch( IsOffHour[ 0 ] )
        {
            case '0': Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal ); break;
            case '1': Msg.SetTradingSessionID( nsOrderMessageDefine::tsOffHour );break;
            default:  Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuto );   break;
        }
    }
    else
        Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuto );
    FConnection->CancelOrder( &Msg );
}        
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::Cancel( TSQMessage& SQMessage )
{
    UFC::BufferedLog::DebugPrintf( " TSystexOrderConnection::Cancel" );    
    TCancelOrderMessage Msg;
    UFC::AnsiString TFXSys, CompanyID, BranchID, AccountCode, AccountChecksum, Symbol, Quantity, Price, Side, OrderID;
    
    if( SQMessage.Get( "07", TFXSys ) &&
        SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "17", AccountCode ) &&
        SQMessage.Get( "18", AccountChecksum ) &&
        SQMessage.Get( "29", Symbol ) &&
        SQMessage.Get( "31", Price ) &&
        SQMessage.Get( "32", Side ) &&
        SQMessage.Get( "36", OrderID ) ) 
    {
        UFC::AnsiString BrokerID, Account, Data,OS,IS;
        nsOrderMessageDefine::MarketEnum Market = GetMarket( SQMessage );        
        
        Msg.SetMarket( Market );
        BrokerID.Printf( "%s%s%s", TFXSys.c_str(), CompanyID.c_str(), BranchID.c_str() );        
        Account.Printf( "%s%s", AccountCode.c_str(), AccountChecksum.c_str() );        
        Msg.SetBrokerID( BrokerID.c_str() );
        Msg.SetAccount( Account.c_str() );        
        Msg.SetSymbol( Symbol.c_str() );
        Msg.SetPrice( Price.ToDouble() );        
        Msg.SetSide( (Side == "B") ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
        Msg.SetOrderID( OrderID.c_str() );                
        if( SQMessage.Get( "VK", Data ) )
            Msg.SetData( Data.c_str() );       
        if( SQMessage.Get( "OS", OS ) && OS.Length() >= 1 )
            Msg.SetOrderSource( ToOrderSourceEnum( OS[ 0 ] ) );
        if( SQMessage.Get( "IS", IS ) && IS.Length() == 3 )
            Msg.SetMarketDataSource( IS.c_str() );
        TFXCancel( SQMessage, Msg );
    }
    else
        UFC::BufferedLog::DebugPrintf( " Missing required tag(08,09,17,18,29,30,31,32,36 or 03)." );            
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::QuoteCancel( TSQMessage& SQMessage )
{
    UFC::BufferedLog::DebugPrintf( " TSystexOrderConnection::QuoteCancel" );    
    TQuoteCancelMessage Msg;
    UFC::AnsiString TFXSys, CompanyID, BranchID, AccountCode, AccountChecksum, Symbol, OrderID, BidPx, AskPx;
    
    if( SQMessage.Get( "07", TFXSys ) &&
        SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "17", AccountCode ) &&
        SQMessage.Get( "18", AccountChecksum ) &&
        SQMessage.Get( "29", Symbol ) &&        
        SQMessage.Get( "T4", BidPx ) &&
        SQMessage.Get( "T5", AskPx ) &&
        SQMessage.Get( "36", OrderID ) ) 
    {
        nsOrderMessageDefine::MarketEnum Market = GetMarket( SQMessage );                
        UFC::AnsiString BrokerID, Account, Data;
        UFC::AnsiString FOK, IOC, CM, AccountFlag;        
        
        Msg.SetMarket( Market );        
        BrokerID.Printf( "%s%s%s", TFXSys.c_str(), CompanyID.c_str(), BranchID.c_str() );
        Account.Printf( "%s%s", AccountCode.c_str(), AccountChecksum.c_str() );        
        Msg.SetBrokerID( BrokerID.c_str() );
        Msg.SetAccount( Account.c_str() );        
        Msg.SetSymbol( Symbol.c_str() );
        Msg.SetBidPrice( BidPx.ToDouble() );        
        Msg.SetAskPrice( AskPx.ToDouble() );                
        Msg.SetQuoteID( OrderID.c_str() );                
        if( SQMessage.Get( "VK", Data ) )
            Msg.SetData( Data.c_str() );       
        ///< Set Clear member ID.
        if( SQMessage.Get( "VT", CM ) ) 
            FConnection->SetCMID( CM );                    
        if( SQMessage.Get( "HR", AccountFlag ) == FALSE )            
        {
            UFC::BufferedLog::DebugPrintf( " Missing account flag tag(HR)" );
            return;
        }
        else
            Msg.SetAccountFlag( AccountFlag.c_str() );        
        FConnection->QuoteCancel( &Msg );
    }
    else
        UFC::BufferedLog::DebugPrintf( " Missing required tag(08,09,17,18,29,36 or 03)." );        
    
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::TFXOrderStatusRequest( TSQMessage& SQMessage, TOrderStatusRequest& Msg )
{
    UFC::AnsiString IOC, OrderType,PosEffect,CM,Quantity,AccountFlag, IsOffHour;
    ///< Set Clear member ID.
    if( SQMessage.Get( "VT", CM ) ) 
        FConnection->SetCMID( CM );        
    ///< Set PositionEffect ( Open,Close,DayTrade )
    
    if( SQMessage.Get( "HR", AccountFlag ) )    
    {
        Msg.SetAccountFlag( AccountFlag.c_str() );
    }
    else 
    {
        UFC::BufferedLog::DebugPrintf( " Missing account flag tag(HR)" );
        return;
    }
    if( SQMessage.Get( "30", Quantity ) == FALSE )
        Quantity = "0";
    if( SQMessage.Get( "TS", IsOffHour ) == TRUE )
    {
        switch( IsOffHour[ 0 ] )
        {
            case '0': Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );break;
            case '1': Msg.SetTradingSessionID( nsOrderMessageDefine::tsOffHour );break;
            default:  Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuto );break;
        }
    }
    else
        Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuto );
    FConnection->OrderStatusRequest( &Msg );
}        
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::OrderStatusRequest( TSQMessage& SQMessage )
{
    UFC::BufferedLog::DebugPrintf( " TSystexOrderConnection::OrderStatusRequest" );        
    TOrderStatusRequest Msg;
    UFC::AnsiString TFXSys, CompanyID, BranchID, AccountCode, AccountChecksum, Symbol, Quantity, Price, Side, OrderID;
    
    if( SQMessage.Get( "07", TFXSys ) && 
        SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "17", AccountCode ) &&
        SQMessage.Get( "18", AccountChecksum ) &&
        SQMessage.Get( "29", Symbol ) &&
        SQMessage.Get( "30", Quantity ) &&
        SQMessage.Get( "31", Price ) &&
        SQMessage.Get( "32", Side ) &&
        SQMessage.Get( "36", OrderID ) ) 
    {
        UFC::AnsiString  BrokerID, Account, Data;
        nsOrderMessageDefine::MarketEnum Market = GetMarket( SQMessage );        
        
        Msg.SetMarket( Market );
        BrokerID.Printf( "%s%s%s", TFXSys.c_str(), CompanyID.c_str(), BranchID.c_str() );       
        Account.Printf( "%s%s", AccountCode.c_str(), AccountChecksum.c_str() );        
        Msg.SetBrokerID( BrokerID.c_str() );
        Msg.SetAccount( Account.c_str() );        
        Msg.SetSymbol( Symbol.c_str() );
        Msg.SetPrice( Price.ToDouble() );        
        Msg.SetSide( (Side == "B") ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
        Msg.SetOrderID( OrderID.c_str() );                
        if( SQMessage.Get( "VK", Data ) )
            Msg.SetData( Data.c_str() );       
        TFXOrderStatusRequest( SQMessage, Msg );
    }
    else
        UFC::BufferedLog::DebugPrintf( " Missing required tag(08,09,17,18,29,30,31,32,36 or 03)." );        
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::SetConnectionState( TFubonTAIFEXConnectionStateEnum ConnectionState )
{
    UFC::PLockObject LockObject( FConnectionStateCS );
    FConnectionState = ConnectionState;
}
//------------------------------------------------------------------------------
TFubonTAIFEXConnectionStateEnum TFubonTAIFEXConnection::GetConnectionState( void )
{
    UFC::PLockObject LockObject( FConnectionStateCS );
    return FConnectionState;
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::CheckConnectionState( void )
{
    if( GetConnectionState() == focDisconnect ) 
    {
        UFC::AnsiString* IP = FIPPool.DequeueNoWait();
        
        while( IP == NULL )
        {
            SetConnectionState( focDisconnect );
            UFC::PStringList IPList;

            IPList.SetStrings( FIP, ", " );
            for( int i = 0; i < IPList.ItemCount(); i++ ) 
            {
                IP = new UFC::AnsiString( IPList[i] );
                UFC::BufferedLog::Printf( " FIPPool.InqueueNoWait( %s )", IP->c_str() );
                FIPPool.InqueueNoWait( IP );
            }            
            IP = FIPPool.DequeueNoWait();
        }
        UFC::BufferedLog::Printf( " Try to connect to IP[%s,%d]", IP->c_str(), FPort );
        UFC::BufferedLog::Printf( " FConnection->Connect( %s, %d )", IP->c_str(), FPort );
        FConnection->Connect( IP->c_str(), FPort, 10 );
        delete IP;        
    }    
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::Execute( void )
{
    UFC::BufferedLog::DebugPrintf( " TFubonTAIFEXConnection::Execute" );

    while( !IsTerminated() )
    {
        CheckConnectionState();
        if( GetConnectionState() == focReady ) 
        {
            Dequeue();            
            UFC::SleepUS( WAIT_US );
        }
        else
            sleep( 10 ); ///< sleep 10 sec
    }
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::Dequeue( void )
{
    int ReadSize;
    
    while( GetConnectionState() == focReady )
    {
        ReadSize = FOrderQueue->Read( FReadBuffer, 1024 );
        if( ReadSize > 0 )
        {
            FReadBuffer[ReadSize] = 0x00;
            OnData();
        }
        else
            return;
    }
}
//------------------------------------------------------------------------------
BOOL TFubonTAIFEXConnection::CheckTradeDate( TSQMessage& SQMessage )
{
    if( FCheckTradeDate == TRUE )
    {
        UFC::AnsiString TradeDate;

        if( SQMessage.Get( "21", TradeDate ) )
        {				
            if( FToday.ToInt() == TradeDate.ToInt() )
                return TRUE;
        }	
        else
            UFC::BufferedLog::Printf( " Miss tag 21 (Trade date)." );
        UFC::BufferedLog::Printf( " Trade date error[%s].", TradeDate.c_str() );
        return FALSE;    
    }
    return TRUE;
}
//------------------------------------------------------------------------------
BOOL TFubonTAIFEXConnection::CheckTradeTime( TSQMessage& SQMessage )
{
    UFC::AnsiString TradeTime;
    
    if( SQMessage.Get( "22", TradeTime ) ) 
    {
        int HHMMSS = TradeTime.ToInt();
        UFC::UTime CheckTime( HHMMSS / 10000, (HHMMSS % 10000) / 100, HHMMSS % 100 );
        UFC::UTime CurrentTime;
        
        if( (CurrentTime - CheckTime) > FTradeTimeout )
            return FALSE;
    }    
    return TRUE;
}
//------------------------------------------------------------------------------
void TFubonTAIFEXConnection::RejectOvertimeOrder( TSQMessage& OvertimeOrder )
{
    TSQMessage RejectMessage;
    UFC::AnsiString Value,TFXID,CompanyID,BranchID,Market;    
    
    RejectMessage.Append( "00", "3" );    
    if( OvertimeOrder.Get( "07", TFXID ) )
        RejectMessage.Append( "07", TFXID );
    if( OvertimeOrder.Get( "08", CompanyID ) )
        RejectMessage.Append( "08", CompanyID );
    if( OvertimeOrder.Get( "09", BranchID ) )
        RejectMessage.Append( "09", BranchID );    
    if( OvertimeOrder.Get( "13", Market ) )
        RejectMessage.Append( "13", Market );
    if( OvertimeOrder.Get( "14", Value ) )
        RejectMessage.Append( "14", Value );
    if( OvertimeOrder.Get( "17", Value ) )
        RejectMessage.Append( "17", Value );
    if( OvertimeOrder.Get( "18", Value ) )
        RejectMessage.Append( "18", Value );
    RejectMessage.Append( "19", "E" ); ///< It's a error execution.
    RejectMessage.Append( "40", "97" );///< Order Expired.
    RejectMessage.Append( "21", FToday );
    UFC::UTime CurrentTime;
    UFC::AnsiString TimeString;
    TimeString.Printf( "%02d%02d%02d%02d", CurrentTime.getHour(),  CurrentTime.getMinute(),  CurrentTime.getSecond(),  CurrentTime.getMillisecond()/10 );
    RejectMessage.Append( "22", TimeString.c_str() );
    if( OvertimeOrder.Get( "27", Value ) )
        RejectMessage.Append( "27", Value );
    if( OvertimeOrder.Get( "29", Value ) )
        RejectMessage.Append( "29", Value );
    if( OvertimeOrder.Get( "30", Value ) )
        RejectMessage.Append( "30", Value );
    if( OvertimeOrder.Get( "31", Value ) )
        RejectMessage.Append( "31", Value.ToDouble() );
    if( OvertimeOrder.Get( "32", Value ) )
        RejectMessage.Append( "32", Value );
    if( OvertimeOrder.Get( "36", Value ) )
        RejectMessage.Append( "36", Value );
    if( OvertimeOrder.Get( "42", Value ) )
        RejectMessage.Append( "42", Value );
    if( OvertimeOrder.Get( "43", Value ) )
        RejectMessage.Append( "43", Value );
    if( OvertimeOrder.Get( "O3", Value ) )
        RejectMessage.Append( "O3", Value );
    if( OvertimeOrder.Get( "VK", Value ) )
        RejectMessage.Append( "VK", Value );        
    ///< For Futures
    if( OvertimeOrder.Get( "O4", Value ) )
        RejectMessage.Append( "O4", Value );        
    if( OvertimeOrder.Get( "V5", Value ) )
        RejectMessage.Append( "V5", Value );        
    if( OvertimeOrder.Get( "W3", Value ) )
        RejectMessage.Append( "W3", Value );        
    if( OvertimeOrder.Get( "VT", Value ) )
        RejectMessage.Append( "VT", Value );        
    RejectMessage.Append( "PI", "000" );
    ///< Send to MBus message.
    UFC::AnsiString MessageString = RejectMessage.ToString();
    FConfirmQueue->Write( (char*)(MessageString.c_str()), MessageString.Length() );        
}
//------------------------------------------------------------------------------
