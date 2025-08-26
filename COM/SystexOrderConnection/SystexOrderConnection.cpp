#include "SystexOrderConnection.h"
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
extern MessageObject*  MessageObj;
extern UFC::AnsiString GBrokerID;
extern UFC::AnsiString GLogonID;
extern BOOL            GIsTAIFEX;
extern BOOL            GFillOnly;
extern BOOL            GConfirmOnly;
//------------------------------------------------------------------------------
TSystexOrderConnection::TSystexOrderConnection( const UFC::AnsiString& ConfigFileName, int RecoverBeginTime )
:UFC::PThread( NULL, FALSE )
,FTFXCMID( "" )
,FDebug( "" )
,FLogFile( "" )
,FAppName( "" )
,FIP( "" )
,FPort( 0 )
,FPassword( "" )
,FTradeTimeout( 60 )
,FRecoverCount( 0 )
,FToday( "" )
,FConnection( NULL )
,FRecoverBeginTime( RecoverBeginTime )
,FConnectionState( focDisconnect )
{
    UFC::GetYYYYMMDD( FToday );
    ReadConfigFile( ConfigFileName );
    
    if( GIsTAIFEX == TRUE )
    {
        MessageObj = new MessageObject( FAppName, "1.0", "SOC for TAIFEX" );
        if( GFillOnly == FALSE )
        {
            MessageObj->AddListener( "ORDER.FUT", GLogonID, this );
            MessageObj->AddListener( "ORDER.OPT", GLogonID, this );
        }
        FConnection   = new TTaifexConnection( FAppName, this, FALSE );        
        FConnection->SetUseNewFuturesSymbol( TRUE ); 
        FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, GBrokerID.c_str() );
        FConnection->SetCMID( FTFXCMID.c_str() );
        ///< Load TAIFEX message format config files.
        FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
        FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );
    }
    else
    {
        MessageObj = new MessageObject( FAppName, "1.0", "SOC for TSE,OTC" );
        if( GFillOnly == FALSE )
        {
            MessageObj->AddListener( "ORDER.TSE", GLogonID, this );
            MessageObj->AddListener( "ORDER.OTC", GLogonID, this );
        }
        MessageObj->AddListener( "NOTSPEEDY.FILL.TSE", UFC::Hostname, this );
        MessageObj->AddListener( "NOTSPEEDY.FILL.OTC", UFC::Hostname, this );
        FConnection   = new TTaifexConnection( FAppName, this, FALSE );        
        ///< Set the broker ID of this connection.
        FConnection->SetBrokerID( nsOrderMessageDefine::mTSE, GBrokerID.c_str() );
        ///< Load TSE/OTC message format config files.
        FConnection->SetTSEDataFormatResourceID( IDR_TSE_T010, IDR_TSE_O010, IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
        FConnection->SetOTCDataFormatResourceID( IDR_OTC_T010, IDR_OTC_O010, IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
        FConnection->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010, IDR_TSE_E010, IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020, IDR_TSE_Ex020, "INI" );
        FConnection->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );
    }
    Start();
}
//------------------------------------------------------------------------------
TSystexOrderConnection::~TSystexOrderConnection( void )
{
    if( FConnection != NULL )
        delete FConnection;       
    for( int i = 0; i < FIPPool.ItemCount(); i++ )
    {
        UFC::AnsiString* str = FIPPool.DequeueNoWait();
        if( str )
            delete str;
    }
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::SetLogObject( void )
{
    UFC::AnsiString LogFileName;
    LogFileName.Printf( "%s%s.%s.log", FLogFile.c_str(),FAppName.c_str(), FToday.c_str() );

    UFC::BufferedLog::DebugPrintf( " Write log to file:[%s]", LogFileName.c_str() );    
    UFC::BufferedLog::SetDebugMode( (FDebug == "1") );
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogFileName, 1024, TRUE ) );
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
void TSystexOrderConnection::ReadConfigFile( const UFC::AnsiString& ConfigFileName )
{
    try
    {
        UFC::UiniFile iniFile( ConfigFileName );
        UFC::AnsiString Value;

        iniFile.GetValue( "main", "debug", FDebug );                        
        if( GIsTAIFEX == TRUE )        
        {
            FAppName.Printf( "SOC_TFX_%s",GLogonID.c_str() );
            if( iniFile.GetValue( "main", "TAIFEX_CMID", FTFXCMID ) == FALSE )         
                FTFXCMID = GBrokerID.SubString( 0, 4 );
        }
        else
             FAppName.Printf( "SOC_TSE_%s",GLogonID.c_str() );
        iniFile.GetValue( "speedy", "ip", FIP );
        iniFile.GetValue( "speedy", "port", Value );
        FPort = Value.ToInt();
        Value = "";
        iniFile.GetValue( "speedy", "password", FPassword );             
        iniFile.GetValue( "sysque", "trade_timeout", Value );
        FTradeTimeout = Value.ToInt();
        Value = "";

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
        UFC::BufferedLog::Printf( " [ main ]" );
        UFC::BufferedLog::Printf( " debug            = [%s]", FDebug.c_str() );        
        UFC::BufferedLog::Printf( " BrokerID         = [%s]", GBrokerID.c_str() );        
        UFC::BufferedLog::Printf( " LogonID          = [%s]", GLogonID.c_str() );
        if( GIsTAIFEX == TRUE )
            UFC::BufferedLog::Printf( " TAIFEX_CMID      = [%s]", FTFXCMID.c_str() );
        UFC::BufferedLog::Printf( " " );        
        UFC::BufferedLog::Printf( " [speedy]" );
        UFC::BufferedLog::Printf( " AppName          = [%s]", FAppName.c_str() );
        UFC::BufferedLog::Printf( " ip               = [%s]", FIP.c_str() );
        UFC::BufferedLog::Printf( " port             = [%d]", FPort );
        UFC::BufferedLog::Printf( " password         = [%s]", FPassword.c_str() );        
        UFC::BufferedLog::Printf( " " );
        UFC::BufferedLog::Printf( " [sysque]" );        
        UFC::BufferedLog::Printf( " trade_timeout    = [%d]", FTradeTimeout );
        UFC::BufferedLog::Printf( " " );
        SetLogObject();        
    }
    catch( UFC::Exception& ex )
    {
        UFC::BufferedLog::DebugPrintf( " %s\n", ex.what() );
    }
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::OnConnected( void )
{    
    UFC::BufferedLog::Printf( " Logon( id=[%s], password=[%s] )", GLogonID.c_str(), FPassword.c_str() );
    SetConnectionState( focConnected );    
    if( GFillOnly == TRUE )
    {
        UFC::BufferedLog::Printf( " *** Receive fill executions only *** " );
        FConnection->Logon( GLogonID.c_str(), FPassword.c_str(), "0000000", ctRecvFillOnly );    
    }
    else if( GConfirmOnly == TRUE )
    {
        UFC::BufferedLog::Printf( " *** Receive Confirm executions only *** " );
        FConnection->Logon( GLogonID.c_str(), FPassword.c_str(), "0000000", ctRecvComfirmOnly );    
    }
    else
    {
        UFC::BufferedLog::Printf( " *** Receive all executions *** " );        
        FConnection->Logon( GLogonID.c_str(), FPassword.c_str(), "0000000", ctBoth );    
    }
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *Connection broken, Wait for 2 sec to reconnect." );    
    SetConnectionState( focDisconnect );
    UFC::SleepMS( 2000 );    
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::OnLogonReply( const char* ReplyString, bool IsAccepted, int CID )
{
    if( IsAccepted )
    {
        UFC::BufferedLog::Printf( " %s( CID=%d )....logon ok", ReplyString, CID );        
        UFC::AnsiString BeginTime;
        
        BeginTime.Printf( "%06d", FRecoverBeginTime );        
        if( GIsTAIFEX == TRUE )
        {
            UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdBoth,mOptions & mFutures )", BeginTime.c_str() );
            FConnection->Recover( BeginTime.c_str(), rdFill, rmFutures );
            FConnection->Recover( BeginTime.c_str(), rdFill, rmOption );
        }
        else
        {
            UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdBoth,mTSE & mOTC )", BeginTime.c_str() );
            FConnection->Recover( BeginTime.c_str(), rdFill, rmTSE );
            FConnection->Recover( BeginTime.c_str(), rdFill, rmOTC );
        }        
    }
    else
    {
        UFC::BufferedLog::Printf( " %s....logon failed", ReplyString );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::Recover( UFC::AnsiString BeginTime )
{
    if( GetConnectionState() == focReady )
    {
        if( GIsTAIFEX == TRUE )
        {
            FConnection->Recover( BeginTime.c_str(), rdFill, rmFutures );
            FConnection->Recover( BeginTime.c_str(), rdFill, rmOption );
        }
        else
        {
            FConnection->Recover( BeginTime.c_str(), rdFill, rmTSE );
            FConnection->Recover( BeginTime.c_str(), rdFill, rmOTC );
        }        
        UFC::BufferedLog::Printf( " Recover from:%s", BeginTime.c_str() );
    }
}    
//------------------------------------------------------------------------------
void TSystexOrderConnection::OnRecoverFinished( int Count )
{
    FRecoverCount++;
    UFC::BufferedLog::DebugPrintf( " TSystexOrderConnection::OnRecoverFinished( Count = %d )", Count );
    
    if( FRecoverCount == 2 )
    {
        SetConnectionState( focReady );
        UFC::BufferedLog::DebugPrintf( " Ready to place order!");
        FRecoverCount = 0;
        UFC::SleepMS( 1000 );        
    }
}
//------------------------------------------------------------------------------
// Handle confirm execution.
//
// For OTC NegotiatePx trading session only
//------------------------------------------------------------------------------
void TSystexOrderConnection::OnNegotiatePxConfirm( TExecutionReportMessage* Msg )
{
    TSQMessage      SQMessage;
    UFC::UTime      CurrentTime;
    UFC::AnsiString Data( Msg->GetData() );   
    UFC::AnsiString BrokerID = Data.SubString( 0, 4 ); 
    UFC::AnsiString CompanyID = Data.SubString( 0, 2 );
    UFC::AnsiString BranchID = Data.SubString( 2, 2 );
    UFC::AnsiString UniqueKey( Data.c_str() + 4 );   
    UFC::AnsiString PBNO( Msg->GetLINBRN() );
    UFC::AnsiString PVC( Msg->GetPVC() );
    UFC::AnsiString PHost( Msg->GetProcessHost() );
    UFC::AnsiString LineNo,TimeString;
    
    TimeString.Printf( "%02d%02d%02d%02d", CurrentTime.getHour(),  CurrentTime.getMinute(),  CurrentTime.getSecond(),  CurrentTime.getMillisecond()/10 );
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osPendingNew:        		
            SQMessage.Append( "00", "2" );            
            SQMessage.Append( "14", "I" );
            SQMessage.Append( "19", "S" );
            break;    
        case nsOrderMessageDefine::osNew:
            SQMessage.Append( "00", "3" );        	
            
            SQMessage.Append( "14", "I" );
            SQMessage.Append( "19", "R" );
            break;
        default: break;
    }
    SQMessage.Append( "08", CompanyID );
    SQMessage.Append( "09", BranchID );
    SQMessage.Append( "13", "O" ); ///< Market, always OTC        
    SQMessage.Append( "21", FToday );
    SQMessage.Append( "22", TimeString.c_str() );    
    SQMessage.Append( "O3", "8" ); ///< Trade Session, 8 for Negotiate Price   
    SQMessage.Append( "VJ", "00" );///< Sender LineNo 
    ///< PVC ID processed this order.
    if( PVC.Length() > 0 ) 
      	SQMessage.Append( "VI", PVC.c_str() );
    else
        SQMessage.Append( "VI", "xx" );    	
    ///< Line No processed this order.
    SQMessage.Append( "VH", "XX" );            
    SQMessage.Append( "VL", Msg->GetSrc() );        
    SQMessage.Append( "VM", UniqueKey );            
    MTree ReplyData;
    
    ReplyData.append( "DATA", SQMessage.ToString() );
    MessageObj->Send( "OTC.CONFIRM", GLogonID, ReplyData, FALSE );
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::TSEOrderStatus( TExecutionReportMessage* Msg, TSQMessage& SQMessage )
{
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osPendingNew:        		
            SQMessage.Append( "00", "2" );
            SQMessage.Append( "14", "I" );
            SQMessage.Append( "19", "S" );
            break;
        case nsOrderMessageDefine::osPendingReplace:
            SQMessage.Append( "00", "2" );
            SQMessage.Append( "14", "C" );
            SQMessage.Append( "19", "R" );
            break;
        case nsOrderMessageDefine::osPendingCancel:
            SQMessage.Append( "00", "2" );
            SQMessage.Append( "14", "D" );
            SQMessage.Append( "19", "R" );
            break;
        case nsOrderMessageDefine::osNew:
            if( Msg->GetExecType( ) == nsOrderMessageDefine::etOrderStatus )
                SQMessage.Append( "00", "9" );
            else
                SQMessage.Append( "00", "3" );        	
            SQMessage.Append( "14", "I" );
            SQMessage.Append( "19", "R" );
            break;
        case nsOrderMessageDefine::osReplaced:
            SQMessage.Append( "00", "3" );
            SQMessage.Append( "14", "C" );
            SQMessage.Append( "19", "R" );
            break;
        case nsOrderMessageDefine::osCanceled:
            SQMessage.Append( "00", "3" );
            SQMessage.Append( "14", "D" );
            SQMessage.Append( "19", "R" );
            break;
        case nsOrderMessageDefine::osPartiallyFilled:            
        case nsOrderMessageDefine::osFilled:
            SQMessage.Append( "00", "4" );
            SQMessage.Append( "14", "I" );
            break;
        case nsOrderMessageDefine::osRejected:
            switch( Msg->GetCxlRejResponseTo() )
            {
                UFC::BufferedLog::DebugPrintf( " Status:osRejected ResponseTo[%d]",Msg->GetCxlRejResponseTo() );
                case nsOrderMessageDefine::crrNew:
                    SQMessage.Append( "00", "3" );
                    SQMessage.Append( "14", "I" );
                    SQMessage.Append( "19", "E" );
                    break;
                case nsOrderMessageDefine::crrReplace:
                    SQMessage.Append( "00", "3" );
                    SQMessage.Append( "14", "C" );
                    SQMessage.Append( "19", "E" );
                    break;
                case nsOrderMessageDefine::crrCancel:
                    SQMessage.Append( "00", "3" );
                    SQMessage.Append( "14", "D" );
                    SQMessage.Append( "19", "E" );
                    break;
                case nsOrderMessageDefine::crrOrderStatus:
                    SQMessage.Append( "00", "9" );
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
void TSystexOrderConnection::TFXOrderStatus( TExecutionReportMessage* Msg, TSQMessage& SQMessage )
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
        case nsOrderMessageDefine::osRejected:
            switch( Msg->GetCxlRejResponseTo() )
            {
                UFC::BufferedLog::DebugPrintf( " Status:osRejected ResponseTo[%d]",Msg->GetCxlRejResponseTo() );
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
                    SQMessage.Append( "14", "C" );
                    SQMessage.Append( "19", "E" );
                    break;
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
                default: break;
            }
            break;
        default: break;
    }    
}    
//------------------------------------------------------------------------------
void TSystexOrderConnection::OnExecutionReport( TExecutionReportMessage* Msg, bool PossDup )
{
    UFC::PLockObject Lock( FAPIIOCS );
    UFC::BufferedLog::DebugPrintf( " TSystexOrderConnection::OnExecutionReport" );
    
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsNegotiatePx )
    {
        OnNegotiatePxConfirm( Msg );
        return;
    }
    TSQMessage SQMessage;
    if( GIsTAIFEX == FALSE )            
    {
        TSEOrderStatus( Msg, SQMessage );
        UFC::AnsiString BrokerID( Msg->GetBrokerID() );
        if( BrokerID.Length() == 4 ) ///< TSE/OTC
        {
            UFC::AnsiString CompanyID = BrokerID.SubString( 0, 2 );
            UFC::AnsiString BranchID = BrokerID.SubString( 2, 2 );
            SQMessage.Append( "08", CompanyID );
            SQMessage.Append( "09", BranchID );
        }
    }
    else
    {
        TFXOrderStatus( Msg, SQMessage );
        UFC::AnsiString BrokerID( Msg->GetBrokerID() );    
        if( BrokerID.Length() == 7 ) ///< Futures/Option
        {
            UFC::AnsiString TFXType   = BrokerID.SubString( 0, 1 );
            UFC::AnsiString CompanyID = BrokerID.SubString( 1, 3 );
            UFC::AnsiString BranchID  = BrokerID.SubString( 4, 3 );
            SQMessage.Append( "07", TFXType );
            SQMessage.Append( "08", CompanyID );
            SQMessage.Append( "09", BranchID );
        }
    }
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTSE:     SQMessage.Append( "13", "T" );break;
        case nsOrderMessageDefine::mOTC:     SQMessage.Append( "13", "O" );break;
        case nsOrderMessageDefine::mTWFutures: SQMessage.Append( "13", "FUT" );break;
        case nsOrderMessageDefine::mTWOptions: SQMessage.Append( "13", "OPT" );break;
        default:break;
    }

    UFC::AnsiString Account( Msg->GetAccount() );
    UFC::AnsiString AccountCode, AccountChecksum;
    
    AccountCode.Printf( "%06d", Account.ToInt() / 10 );
    AccountChecksum.Printf( "%d", Account.ToInt() % 10 );    
    SQMessage.Append( "17", AccountCode );
    SQMessage.Append( "18", AccountChecksum );
    if( Msg->GetMarket() == nsOrderMessageDefine::mTSE ||Msg->GetMarket() == nsOrderMessageDefine::mOTC )
        SQMessage.Append( "27", Msg->GetTSEOrderType() );
    else
    {
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
    }
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
    
    if( Msg->GetMarket() == nsOrderMessageDefine::mTSE ||Msg->GetMarket() == nsOrderMessageDefine::mOTC )
    {
        switch( Msg->GetOrderStatus() )
        {
            case nsOrderMessageDefine::osPartiallyFilled:
            case nsOrderMessageDefine::osFilled:
            	 SQMessage.Append( "32", (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
                 OnTSEFilled(  Msg->GetMarket(), GBrokerID, Msg, SQMessage );
                 break;
            default:
                 OnTSEConfirm( Msg->GetMarket(), GBrokerID, Msg, SQMessage );
                 break;
        }
    }
    else
    {
        int Src3 = UFC::AnsiString( Msg->GetSrc(), 3 ).ToInt();
        UFC::AnsiString LineBrokerID = Msg->GetLINBRN();
        
        switch( Msg->GetOrderStatus() )
        {
            case nsOrderMessageDefine::osCanceled:
            case nsOrderMessageDefine::osReplaced:
            case nsOrderMessageDefine::osPartiallyFilled:
            case nsOrderMessageDefine::osFilled:
            	 SQMessage.Append( "32", (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
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
                 OnTFXConfirm( Msg->GetMarket(), GBrokerID, Msg, SQMessage );
                 break;
        }
    }
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::OnTSEConfirm( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& BrokerID, TExecutionReportMessage* Msg, TSQMessage& SQMessage )
{
    SQMessage.Append( "21", FToday );
    UFC::UTime CurrentTime;
    UFC::AnsiString TimeString;
    TimeString.Printf( "%02d%02d%02d%02d", CurrentTime.getHour(),  CurrentTime.getMinute(),  CurrentTime.getSecond(),  CurrentTime.getMillisecond()/10 );
    SQMessage.Append( "22", TimeString.c_str() );
    
    switch( Msg->GetTradingSessionID() )
    {
        case nsOrderMessageDefine::tsNormal:
        case nsOrderMessageDefine::tsOffHour:
        case nsOrderMessageDefine::tsAuction:
        case nsOrderMessageDefine::tsLend:
        case nsOrderMessageDefine::tsTender:
        case nsOrderMessageDefine::tsTenderEx:
                SQMessage.Append( "30", Msg->GetOrderQty() );
                SQMessage.Append( "42", Msg->GetBeforeQty() );
                SQMessage.Append( "43", Msg->GetAfterQty() );
                break;
        case nsOrderMessageDefine::tsOddLot:
                SQMessage.Append( "30", Msg->GetOrderQty() );
                SQMessage.Append( "42", Msg->GetBeforeQty() );
                SQMessage.Append( "43", Msg->GetAfterQty() );
                SQMessage.Append( "EO", Msg->GetStockSeqNo() );
                break;  
        default:break;
    }    
    switch( Msg->GetTradingSessionID() )
    {
        case nsOrderMessageDefine::tsNormal:
            SQMessage.Append( "O3", "0" );
            SQMessage.Append( "32", (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
            SQMessage.Append( "28", Msg->GetTSEExchangeCode() );
            break;
        case nsOrderMessageDefine::tsOddLot:
            SQMessage.Append( "O3", "2" );
            SQMessage.Append( "32", (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
            SQMessage.Append( "28", Msg->GetTSEExchangeCode() );
            break;
        case nsOrderMessageDefine::tsOffHour:
            SQMessage.Append( "O3", "3" );
            SQMessage.Append( "32", (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
            SQMessage.Append( "28", Msg->GetTSEExchangeCode() );
            break;
        case nsOrderMessageDefine::tsAuction:            
            SQMessage.Append( "32", "B" );
            SQMessage.Append( "O3", "4" );
            break;
        case nsOrderMessageDefine::tsLend:
            SQMessage.Append( "O3", "5" );
            SQMessage.Append( "32", (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
            break;
        case nsOrderMessageDefine::tsTender:
            SQMessage.Append( "O3", "6" );
            SQMessage.Append( "32", (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
            break;
        case nsOrderMessageDefine::tsTenderEx:
            SQMessage.Append( "O3", "7" );
            SQMessage.Append( "32", (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
            break;
        default:
            SQMessage.Append( "O3", "0" );
            SQMessage.Append( "32", (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
            break;            
    }             
    SQMessage.Append( "P2", Msg->GetAccountFlag( ));        
    SQMessage.Append( "31", Msg->GetPrice() );
    MTree ReplyData;
    UFC::AnsiString ReplyMsg( SQMessage.ToString() ) ;
    
    ReplyData.append( "DATA",  ReplyMsg );    
    UFC::BufferedLog::DebugPrintf( " Send Confirm[%s]",ReplyMsg.c_str() );
    switch( Market )
    {
        case nsOrderMessageDefine::mTSE:     MessageObj->Send( "TSE.CONFIRM", GLogonID, ReplyData, FALSE );break;
        case nsOrderMessageDefine::mOTC:     MessageObj->Send( "OTC.CONFIRM", GLogonID, ReplyData, FALSE );break;
        default:	break;
    }
    
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::OnTFXConfirm( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& BrokerID, TExecutionReportMessage* Msg, TSQMessage& SQMessage )
{
    MTree ReplyData;
    
    SQMessage.Append( "21", FToday );
    UFC::UTime CurrentTime;
    UFC::AnsiString TimeString;
    TimeString.Printf( "%02d%02d%02d%02d", CurrentTime.getHour(),  CurrentTime.getMinute(),  CurrentTime.getSecond(),  CurrentTime.getMillisecond()/10 );
    SQMessage.Append( "32", (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
    SQMessage.Append( "22", TimeString.c_str() );
    SQMessage.Append( "30", Msg->GetOrderQty() );
    SQMessage.Append( "42", Msg->GetBeforeQty() );
    SQMessage.Append( "43", Msg->GetAfterQty() );
    SQMessage.Append( "31", Msg->GetPrice() ); 
    if( Msg->GetOrderStatus() == nsOrderMessageDefine::osQuoteAccept )
        SQMessage.Append( "O3", "4" );    
    else
        SQMessage.Append( "O3", "1" );    
    UFC::AnsiString ExecID( Msg->GetExecID() );
    if( ExecID.Length() > 8  )
        SQMessage.Append( "65", ExecID.SubString( ExecID.Length() - 8, 8 ).c_str() );
    else
        SQMessage.Append( "65", ExecID.c_str() );    
    ReplyData.append( "DATA", SQMessage.ToString() );    
    switch( Market )
    {
        case nsOrderMessageDefine::mTWFutures: MessageObj->Send( "FUT.CONFIRM", GLogonID, ReplyData, FALSE );break;
        case nsOrderMessageDefine::mTWOptions: MessageObj->Send( "OPT.CONFIRM", GLogonID, ReplyData, FALSE );break;
        default:	break;
    }
    UFC::BufferedLog::DebugPrintf( " Send Confirm[%s]",SQMessage.ToString().c_str() );
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::OnTSEFilled( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& BrokerID, TExecutionReportMessage* Msg, TSQMessage& SQMessage )
{
    switch( Msg->GetTradingSessionID() )
    {
        case nsOrderMessageDefine::tsNormal:
            SQMessage.Append( "47", Msg->GetOrderQty() );            
            SQMessage.Append( "42", Msg->GetBeforeQty());
            SQMessage.Append( "43", Msg->GetAfterQty() );
            SQMessage.Append( "O3", "0" );
            break;
        case nsOrderMessageDefine::tsOddLot:
            SQMessage.Append( "47", Msg->GetOrderQty() );
            SQMessage.Append( "42", Msg->GetBeforeQty() );
            SQMessage.Append( "43", Msg->GetAfterQty() );
            SQMessage.Append( "O3", "2" );
            break;
        case nsOrderMessageDefine::tsOffHour:
            SQMessage.Append( "47", Msg->GetOrderQty() );
            SQMessage.Append( "42", Msg->GetBeforeQty() );
            SQMessage.Append( "43", Msg->GetAfterQty() );
            SQMessage.Append( "O3", "3" );
            break;        
        default:
            break;            
    }             
    SQMessage.Append( "48", Msg->GetPrice() );
    SQMessage.Append( "50", FToday );
    SQMessage.Append( "51", Msg->GetTransactTime() );
    SQMessage.Append( "72", Msg->GetRecNo() );    
    UFC::AnsiString ExecID( Msg->GetExecID() );
    if( ExecID.Length() > 6  )
    	SQMessage.Append( "Q5", ExecID.SubString( ExecID.Length() - 6, 6 ).c_str() );
    else
    	SQMessage.Append( "Q5", ExecID.c_str() );    
    MTree ReplyData;
    
    ReplyData.append( "DATA", SQMessage.ToString() );
    switch( Market )
    {
        case nsOrderMessageDefine::mTSE:     MessageObj->Send( "TSE.FILL", GLogonID, ReplyData, FALSE );break;
        case nsOrderMessageDefine::mOTC:     MessageObj->Send( "OTC.FILL", GLogonID, ReplyData, FALSE );break;
        default: break;
    }
    UFC::BufferedLog::DebugPrintf( " Send Fill[%s] to [%s]",SQMessage.ToString().c_str(), GLogonID.c_str() );
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::OnTFXFilled( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& BrokerID, TExecutionReportMessage* Msg, TSQMessage& SQMessage )
{
    UFC::AnsiString Type( Msg->GetSrc(),1 );
            
    switch( Msg->GetOrderStatus() )
    {
       case nsOrderMessageDefine::osCanceled:
            SQMessage.Append( "47", Msg->GetOrderQty() );
            SQMessage.Append( "42", Msg->GetBeforeQty() );
            //SQMessage.Append( "47", "0");
            //SQMessage.Append( "42", Msg->GetOrderQty() );
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
    SQMessage.Append( "48", Msg->GetPrice() );
    SQMessage.Append( "50", FToday );
    SQMessage.Append( "51", Msg->GetTransactTime() );
    SQMessage.Append( "72", Msg->GetRecNo() );    
    SQMessage.Append( "O3", Type.c_str() );    
    UFC::AnsiString ExecID( Msg->GetExecID() );
    if( ExecID.Length() > 8  )
    	SQMessage.Append( "Q5", ExecID.SubString( ExecID.Length() - 8, 8 ).c_str() );
    else
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
    MTree ReplyData;
    
    ReplyData.append( "DATA", SQMessage.ToString() );
    switch( Market )
    {
        case nsOrderMessageDefine::mTWFutures: MessageObj->Send( "FUT.FILL", GLogonID, ReplyData, FALSE );
                                             break;
        case nsOrderMessageDefine::mTWOptions: MessageObj->Send( "OPT.FILL", GLogonID, ReplyData, FALSE );
                                             break;
        default: break;
    }
    UFC::BufferedLog::DebugPrintf( " Send Fill[%s]",SQMessage.ToString().c_str() );
}
//------------------------------------------------------------------------------
//  Order listener
//  Handle both TSE/OTC orders.
//------------------------------------------------------------------------------
void TSystexOrderConnection::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
    TSQMessage      SQMessage;
    UFC::AnsiString SystexMessage;   

    if( Subject == "NOTSPEEDY.FILL.OTC" )
    {
        UFC::BufferedLog::DebugPrintf( " Received other system OTC fill execution."  );        
        FConnection->OnMigoMessage( "FILL.OTC", GLogonID, Data );
    }
    else if(  Subject == "NOTSPEEDY.FILL.TSE" )
    {
        UFC::BufferedLog::DebugPrintf( " Received other system TSE fill execution."  );        
        FConnection->OnMigoMessage( "FILL.TSE", GLogonID, Data );
    }
    else
    {
        UFC::BufferedLog::DebugPrintf( " Receive Message subject[%s] key[%s]",Subject.c_str(), Key.c_str()  );        
        if( Data->get( "DATA", SystexMessage ) == TRUE )
        {
	    			SystexMessage.TrimRight( );
            UFC::BufferedLog::DebugPrintf( " DATA[%s]",SystexMessage.c_str() );
            SQMessage.FromString( SystexMessage );
    
            if( GetConnectionState() != focReady ) 
            {
                RejectOrder( SQMessage, 98 );
    		return;
            } 
            if( CheckTradeDate(SQMessage) )
            {
                if( CheckTradeTime(SQMessage) )
                {
                    UFC::AnsiString OrderType,Session;
            
                    if( SQMessage.Get( "14", OrderType ) && SQMessage.Get( "O3", Session )) 
                    {
                        if( Session == "8" ) ///< Negotiate Price
                            NegotiatePx( SQMessage );
                        else if( OrderType == "I" )
                            New( SQMessage );
                        else if( OrderType == "C" )
                            Replace( SQMessage );
                        else if( OrderType == "D" )
                            Cancel( SQMessage );
                        else if( OrderType == "Q" )
                            OrderStatusRequest( SQMessage );
                    }
                    else
                    {
                        UFC::BufferedLog::Printf( " Missing requried tag(14 or 03)");
                        RejectOrder( SQMessage, 96 );
                    }
                }
                else
                    RejectOrder( SQMessage, 97 );
            }
            else
            {
                UFC::BufferedLog::Printf( " Wrong trade date,Message:[%s].", SystexMessage.c_str() );
                RejectOrder( SQMessage, 97 );
            }
        }
    }
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::NegotiatePx( TSQMessage& SQMessage )
{
    TNewOrderMessage Msg;
    UFC::AnsiString CompanyID, BranchID, Body, UniqueKey, Data;
    
    if( SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "VL", Body ) &&
        SQMessage.Get( "VM", UniqueKey ) )
    {
        Data.Printf( "%s%s%s",CompanyID.c_str(),BranchID.c_str(),UniqueKey.c_str() ); 
        FConnection->NegotiatePriceMessage( Body, Data );
    }
}            
//------------------------------------------------------------------------------
nsOrderMessageDefine::TradingSessionIDEnum TSystexOrderConnection::GetTradeSession( TSQMessage& SQMessage )
{
   UFC::AnsiString OrderType; 
   
   if( SQMessage.Get( "O3", OrderType ) ) 
   {
        switch( OrderType.ToInt() ) 
        {
            case 0: return nsOrderMessageDefine::tsNormal;break;
            case 2: return nsOrderMessageDefine::tsOddLot;break;
            case 3: return nsOrderMessageDefine::tsOffHour;break;
            case 4: return nsOrderMessageDefine::tsAuction;break;
            case 5: return nsOrderMessageDefine::tsLend;break;
            case 6: return nsOrderMessageDefine::tsTender;break;
            case 7: return nsOrderMessageDefine::tsTenderEx;break;            
            case 8: return nsOrderMessageDefine::tsNegotiatePx;break;            
        }        
   }    
   UFC::BufferedLog::DebugPrintf( " Invalid or missing Order Kind tag(03) use default:tsNormal." );        
   return nsOrderMessageDefine::tsNormal;   
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::TSENew( TSQMessage& SQMessage, TNewOrderMessage& Msg )
{
    UFC::AnsiString TSEOrderType, StockSeqNo, Keep, Quantity, TSEExchangeCode, fflag;
    nsOrderMessageDefine::TradingSessionIDEnum  Session = GetTradeSession( SQMessage );
    
    if( SQMessage.Get( "30", Quantity ) == FALSE )
        Quantity = "0";
    if( SQMessage.Get( "27", TSEOrderType ) )
        Msg.SetTSEOrderType( TSEOrderType );
    else
        Msg.SetTSEOrderType( "0" );
    if( SQMessage.Get( "28", TSEExchangeCode ) )
    {
        Msg.SetTSETradeKind( TSEExchangeCode );
        Msg.SetTSEExchangeCode( TSEExchangeCode );
    }
    else
    {
        Msg.SetTSETradeKind( "0" );
        Msg.SetTSEExchangeCode( "0" );        
    }
    if( SQMessage.Get( "EN", Keep ) )
        Msg.SetKeep( Keep );
    if( SQMessage.Get( "EO", StockSeqNo ) )
        Msg.SetStockSeqNo( StockSeqNo.ToInt() );
    Msg.SetTradingSessionID( Session );
    if( Session == nsOrderMessageDefine::tsOddLot )
        Msg.SetOrderQty( Quantity.ToInt() );
    else
        Msg.SetOrderQty( Quantity.ToInt() );    
    if( SQMessage.Get( "P2",fflag))
        Msg.SetAccountFlag( fflag );
    else
        Msg.SetAccountFlag( " " );    
    FConnection->NewOrder( &Msg );    
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::TFXNew( TSQMessage& SQMessage, TNewOrderMessage& Msg )
{
    UFC::AnsiString FOK, IOC, OrderType,PosEffect,CM,Quantity,AccountFlag;
    ///< Set Clear member ID.
    if( SQMessage.Get( "VT", CM ) ) 
        FConnection->SetCMID( CM );        
    ///< Set Order Type ( Limit or market )
    if( SQMessage.Get( "27", OrderType ) ) 
    {
        if( OrderType.ToInt() == 1 )
            Msg.SetOrderType( nsOrderMessageDefine::otMarket );
        else
            Msg.SetOrderType( nsOrderMessageDefine::otLimit );
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
        switch( PosEffect.ToInt() )
        {
            case 1:Msg.SetPositionEffect(nsOrderMessageDefine::peClose);break;
            case 2:Msg.SetPositionEffect(nsOrderMessageDefine::peRolled);break;
            default: Msg.SetPositionEffect(nsOrderMessageDefine::peOpen);break;
        }
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
    Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
    FConnection->NewOrder( &Msg );
}        
//------------------------------------------------------------------------------
nsOrderMessageDefine::MarketEnum TSystexOrderConnection::GetMarket( TSQMessage& SQMessage )
{
    UFC::AnsiString Market = "";
    
    SQMessage.Get( "13", Market );
    if( Market == "T" )
        return nsOrderMessageDefine::mTSE;            
    else if( Market == "O" )
        return nsOrderMessageDefine::mOTC;            
    else if( Market == "FUT" )
        return nsOrderMessageDefine::mTWFutures;
    else if( Market == "OPT" )
        return nsOrderMessageDefine::mTWOptions;
    UFC::BufferedLog::DebugPrintf( " Invalid or missing Market tag(13) use default mTSE." );        
    return nsOrderMessageDefine::mTSE;            
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::New( TSQMessage& SQMessage )
{
    UFC::BufferedLog::DebugPrintf( " TSystexOrderConnection::New" );        
    TNewOrderMessage Msg;
    UFC::AnsiString CompanyID, BranchID, AccountCode, AccountChecksum, Symbol, Quantity, Price, Side, OrderID;
    
    if( SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "17", AccountCode ) &&
        SQMessage.Get( "18", AccountChecksum ) &&
        SQMessage.Get( "29", Symbol ) &&
        SQMessage.Get( "30", Quantity ) &&
        SQMessage.Get( "31", Price ) &&
        SQMessage.Get( "32", Side ) &&
        SQMessage.Get( "36", OrderID ) ) 
    {
        UFC::AnsiString TFXSys, BrokerID, Account, Data;
        nsOrderMessageDefine::MarketEnum Market = GetMarket( SQMessage );        
        
        Msg.SetMarket( Market );
        if( Market == nsOrderMessageDefine::mTWFutures ||Market == nsOrderMessageDefine::mTWOptions )
        {
            if( SQMessage.Get( "07", TFXSys ) == FALSE )
            {
                UFC::BufferedLog::DebugPrintf( " TAIFEX order missing tag(07)." );        
                return;
            }
            BrokerID.Printf( "%s%s%s", TFXSys.c_str(), CompanyID.c_str(), BranchID.c_str() );
        }
        else
            BrokerID.Printf( "%s%s", CompanyID.c_str(), BranchID.c_str() );
        Account.Printf( "%s%s", AccountCode.c_str(), AccountChecksum.c_str() );        
        Msg.SetBrokerID( BrokerID.c_str() );
        Msg.SetAccount( Account.c_str() );        
        Msg.SetSymbol( Symbol.c_str() );
        Msg.SetPrice( Price.ToDouble() );        
        Msg.SetSide( (Side == "B") ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
        Msg.SetOrderID( OrderID.c_str() );                
        if( SQMessage.Get( "VK", Data ) )
            Msg.SetData( Data.c_str() );       
        switch( Market )        
        {
            case nsOrderMessageDefine::mTSE:
            case nsOrderMessageDefine::mOTC:     TSENew( SQMessage, Msg );break;
            case nsOrderMessageDefine::mTWFutures:
            case nsOrderMessageDefine::mTWOptions: TFXNew( SQMessage, Msg );break;
        }        
    }
    else
    {
        UFC::BufferedLog::DebugPrintf( " Missing required tag(08,09,17,18,29,30,31,32,36 or 03)." );        
        RejectOrder( SQMessage, 96 );
    }        
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::TSEReplace( TSQMessage& SQMessage, TReplaceOrderMessage& Msg )
{
    UFC::AnsiString TSEOrderType, StockSeqNo, Keep, Quantity, fflag;
    nsOrderMessageDefine::TradingSessionIDEnum  Session = GetTradeSession( SQMessage );
    
    if( SQMessage.Get( "30", Quantity ) == FALSE )
        Quantity = "0";
    if( SQMessage.Get( "27", TSEOrderType ) )
        Msg.SetTSEOrderType( TSEOrderType );
    else
        Msg.SetTSEOrderType( "0" );
    if( SQMessage.Get( "EN", Keep ) )
        Msg.SetKeep( Keep );
    if( SQMessage.Get( "EO", StockSeqNo ) )
        Msg.SetStockSeqNo( StockSeqNo.ToInt() );
    Msg.SetTradingSessionID( Session );
    if( Session == nsOrderMessageDefine::tsOddLot )
        Msg.SetOrderQty( Quantity.ToInt() );
    else
        Msg.SetOrderQty( Quantity.ToInt() );    
    if( SQMessage.Get( "P2",fflag))
        Msg.SetAccountFlag( fflag );
    else
        Msg.SetAccountFlag( " " );        
    FConnection->ReplaceOrder( &Msg );    
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::TFXReplace( TSQMessage& SQMessage, TReplaceOrderMessage& Msg )
{
    UFC::AnsiString FOK, IOC, OrderType,PosEffect,CM,Quantity,AccountFlag;
    ///< Set Clear member ID.
    if( SQMessage.Get( "VT", CM ) ) 
        FConnection->SetCMID( CM );        
    ///< Set Order Type ( Limit or market )
    if( SQMessage.Get( "27", OrderType ) ) 
    {
        if( OrderType.ToInt() == 1 )
            Msg.SetOrderType( nsOrderMessageDefine::otMarket );
        else
            Msg.SetOrderType( nsOrderMessageDefine::otLimit );
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
        switch( PosEffect.ToInt() )
        {
            case 1:Msg.SetPositionEffect(nsOrderMessageDefine::peClose);break;
            case 2:Msg.SetPositionEffect(nsOrderMessageDefine::peRolled);break;
            default: Msg.SetPositionEffect(nsOrderMessageDefine::peOpen);break;
        }
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
    Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );    
    FConnection->ReplaceOrder( &Msg );
}        
//------------------------------------------------------------------------------
void TSystexOrderConnection::Replace( TSQMessage& SQMessage )
{
    UFC::BufferedLog::DebugPrintf( " TSystexOrderConnection::Replace" );
    TReplaceOrderMessage Msg;
    UFC::AnsiString CompanyID, BranchID, AccountCode, AccountChecksum, Symbol, Quantity, Price, Side, OrderID;
        
    if( SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "17", AccountCode ) &&
        SQMessage.Get( "18", AccountChecksum ) &&
        SQMessage.Get( "29", Symbol ) &&
        SQMessage.Get( "30", Quantity ) &&
        SQMessage.Get( "31", Price ) &&
        SQMessage.Get( "32", Side ) &&
        SQMessage.Get( "36", OrderID ) ) 
    {
        UFC::AnsiString TFXSys, BrokerID, Account, Data;
        nsOrderMessageDefine::MarketEnum Market = GetMarket( SQMessage );        
        
        Msg.SetMarket( Market );
        if( Market == nsOrderMessageDefine::mTWFutures ||Market == nsOrderMessageDefine::mTWOptions )
        {
            if( SQMessage.Get( "07", TFXSys ) == FALSE )
            {
                UFC::BufferedLog::DebugPrintf( " TAIFEX order missing tag(07)." );        
                return;
            }
            BrokerID.Printf( "%s%s%s", TFXSys.c_str(), CompanyID.c_str(), BranchID.c_str() );
        }
        else
            BrokerID.Printf( "%s%s", CompanyID.c_str(), BranchID.c_str() );
        Account.Printf( "%s%s", AccountCode.c_str(), AccountChecksum.c_str() );        
        Msg.SetBrokerID( BrokerID.c_str() );
        Msg.SetAccount( Account.c_str() );        
        Msg.SetSymbol( Symbol.c_str() );
        Msg.SetPrice( Price.ToDouble() );        
        Msg.SetSide( (Side == "B") ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
        Msg.SetOrderID( OrderID.c_str() );                
        if( SQMessage.Get( "VK", Data ) )
            Msg.SetData( Data.c_str() );       
        switch( Market )        
        {
            case nsOrderMessageDefine::mTSE:
            case nsOrderMessageDefine::mOTC:     TSEReplace( SQMessage, Msg );break;
            case nsOrderMessageDefine::mTWFutures:
            case nsOrderMessageDefine::mTWOptions: TFXReplace( SQMessage, Msg );break;
        }        
    }
    else
    {
        UFC::BufferedLog::DebugPrintf( " Missing required tag(08,09,17,18,29,30,31,32,36 or 03)." );        
        RejectOrder( SQMessage, 96 );
    }
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::TSECancel( TSQMessage& SQMessage, TCancelOrderMessage& Msg )
{
    UFC::AnsiString TSEOrderType, StockSeqNo, Keep, Quantity,fflag;
    nsOrderMessageDefine::TradingSessionIDEnum  Session = GetTradeSession( SQMessage );
    
    if( SQMessage.Get( "30", Quantity ) == FALSE )
        Quantity = "0";
    if( SQMessage.Get( "27", TSEOrderType ) )
        Msg.SetTSEOrderType( TSEOrderType );
    else
        Msg.SetTSEOrderType( "0" );
    if( SQMessage.Get( "EN", Keep ) )
        Msg.SetKeep( Keep );
    if( SQMessage.Get( "EO", StockSeqNo ) )
        Msg.SetStockSeqNo( StockSeqNo.ToInt() );
    if( SQMessage.Get( "P2",fflag))
        Msg.SetAccountFlag( fflag );
    else
        Msg.SetAccountFlag( " " );            
    Msg.SetTradingSessionID( Session );
    FConnection->CancelOrder( &Msg );    
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::TFXCancel( TSQMessage& SQMessage, TCancelOrderMessage& Msg )
{
    UFC::AnsiString FOK, IOC, OrderType,PosEffect,CM,AccountFlag;
    ///< Set Clear member ID.
    if( SQMessage.Get( "VT", CM ) ) 
        FConnection->SetCMID( CM );        
    ///< Set Order Type ( Limit or market )
    if( SQMessage.Get( "27", OrderType ) ) 
    {
        if( OrderType.ToInt() == 1 )
            Msg.SetOrderType( nsOrderMessageDefine::otMarket );
        else
            Msg.SetOrderType( nsOrderMessageDefine::otLimit );
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
    Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
    FConnection->CancelOrder( &Msg );
}        
//------------------------------------------------------------------------------
void TSystexOrderConnection::Cancel( TSQMessage& SQMessage )
{
    UFC::BufferedLog::DebugPrintf( " TSystexOrderConnection::Cancel" );    
    TCancelOrderMessage Msg;
    UFC::AnsiString CompanyID, BranchID, AccountCode, AccountChecksum, Symbol, Quantity, Price, Side, OrderID;
    
    if( SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "17", AccountCode ) &&
        SQMessage.Get( "18", AccountChecksum ) &&
        SQMessage.Get( "29", Symbol ) &&
        SQMessage.Get( "31", Price ) &&
        SQMessage.Get( "32", Side ) &&
        SQMessage.Get( "36", OrderID ) ) 
    {
        UFC::AnsiString TFXSys, BrokerID, Account, Data;
        nsOrderMessageDefine::MarketEnum Market = GetMarket( SQMessage );        
        
        Msg.SetMarket( Market );
        if( Market == nsOrderMessageDefine::mTWFutures ||Market == nsOrderMessageDefine::mTWOptions )
        {
            if( SQMessage.Get( "07", TFXSys ) == FALSE )
            {
                UFC::BufferedLog::DebugPrintf( " TAIFEX order missing tag(07)." );        
                return;
            }
            BrokerID.Printf( "%s%s%s", TFXSys.c_str(), CompanyID.c_str(), BranchID.c_str() );
        }
        else
            BrokerID.Printf( "%s%s", CompanyID.c_str(), BranchID.c_str() );
        Account.Printf( "%s%s", AccountCode.c_str(), AccountChecksum.c_str() );        
        Msg.SetBrokerID( BrokerID.c_str() );
        Msg.SetAccount( Account.c_str() );        
        Msg.SetSymbol( Symbol.c_str() );
        Msg.SetPrice( Price.ToDouble() );        
        Msg.SetSide( (Side == "B") ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
        Msg.SetOrderID( OrderID.c_str() );                
        if( SQMessage.Get( "VK", Data ) )
            Msg.SetData( Data.c_str() );       
        switch( Market )        
        {
            case nsOrderMessageDefine::mTSE:
            case nsOrderMessageDefine::mOTC:     TSECancel( SQMessage, Msg );break;
            case nsOrderMessageDefine::mTWFutures:
            case nsOrderMessageDefine::mTWOptions: TFXCancel( SQMessage, Msg );break;
        }        
    }
    else
    {
        UFC::BufferedLog::DebugPrintf( " Missing required tag(08,09,17,18,29,30,31,32,36 or 03)." );            
        RejectOrder( SQMessage, 96 );
    }
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::TSEOrderStatusRequest( TSQMessage& SQMessage, TOrderStatusRequest& Msg )
{
    UFC::AnsiString TSEOrderType, StockSeqNo, Keep, Quantity,fflag;
    nsOrderMessageDefine::TradingSessionIDEnum  Session = GetTradeSession( SQMessage );
    
    if( SQMessage.Get( "30", Quantity ) == FALSE )
        Quantity = "0";
    if( SQMessage.Get( "27", TSEOrderType ) )
        Msg.SetTSEOrderType( TSEOrderType );
    else
        Msg.SetTSEOrderType( "0" );
    if( SQMessage.Get( "EN", Keep ) )
        Msg.SetKeep( Keep );
    if( SQMessage.Get( "EO", StockSeqNo ) )
        Msg.SetStockSeqNo( StockSeqNo.ToInt() );
    if( SQMessage.Get( "P2",fflag))
        Msg.SetAccountFlag( fflag );
    else
        Msg.SetAccountFlag( " " );    
    Msg.SetTradingSessionID( Session );    
    FConnection->OrderStatusRequest( &Msg );    
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::TFXOrderStatusRequest( TSQMessage& SQMessage, TOrderStatusRequest& Msg )
{
    UFC::AnsiString IOC, OrderType,PosEffect,CM,Quantity,AccountFlag;
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
    Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
    FConnection->OrderStatusRequest( &Msg );
}        
//------------------------------------------------------------------------------
void TSystexOrderConnection::OrderStatusRequest( TSQMessage& SQMessage )
{
    UFC::BufferedLog::DebugPrintf( " TSystexOrderConnection::OrderStatusRequest" );        
    TOrderStatusRequest Msg;
    UFC::AnsiString CompanyID, BranchID, AccountCode, AccountChecksum, Symbol, Quantity, Price, Side, OrderID;
    
    if( SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "17", AccountCode ) &&
        SQMessage.Get( "18", AccountChecksum ) &&
        SQMessage.Get( "29", Symbol ) &&
        SQMessage.Get( "30", Quantity ) &&
        SQMessage.Get( "31", Price ) &&
        SQMessage.Get( "32", Side ) &&
        SQMessage.Get( "36", OrderID ) ) 
    {
        UFC::AnsiString TFXSys, BrokerID, Account, Data;
        nsOrderMessageDefine::MarketEnum Market = GetMarket( SQMessage );        
        
        Msg.SetMarket( Market );
        if( Market == nsOrderMessageDefine::mTWFutures ||Market == nsOrderMessageDefine::mTWOptions )
        {
            if( SQMessage.Get( "07", TFXSys ) == FALSE )
            {
                UFC::BufferedLog::DebugPrintf( " TAIFEX order missing tag(07)." );        
                return;
            }
            BrokerID.Printf( "%s%s%s", TFXSys.c_str(), CompanyID.c_str(), BranchID.c_str() );
        }
        else
            BrokerID.Printf( "%s%s", CompanyID.c_str(), BranchID.c_str() );
        Account.Printf( "%s%s", AccountCode.c_str(), AccountChecksum.c_str() );        
        Msg.SetBrokerID( BrokerID.c_str() );
        Msg.SetAccount( Account.c_str() );        
        Msg.SetSymbol( Symbol.c_str() );
        Msg.SetPrice( Price.ToDouble() );        
        Msg.SetSide( (Side == "B") ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
        Msg.SetOrderID( OrderID.c_str() );                
        if( SQMessage.Get( "VK", Data ) )
            Msg.SetData( Data.c_str() );       
        switch( Market )        
        {
            case nsOrderMessageDefine::mTSE:
            case nsOrderMessageDefine::mOTC:     TSEOrderStatusRequest( SQMessage, Msg );break;
            case nsOrderMessageDefine::mTWFutures:
            case nsOrderMessageDefine::mTWOptions: TFXOrderStatusRequest( SQMessage, Msg );break;
        }        
    }
    else
        UFC::BufferedLog::DebugPrintf( " Missing required tag(08,09,17,18,29,30,31,32,36 or 03)." );        
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::SetConnectionState( TFubonOrderConnectionStateEnum ConnectionState )
{
    UFC::PLockObject LockObject( FConnectionStateCS );
    FConnectionState = ConnectionState;
}
//------------------------------------------------------------------------------
TFubonOrderConnectionStateEnum TSystexOrderConnection::GetConnectionState( void )
{
    UFC::PLockObject LockObject( FConnectionStateCS );
    return FConnectionState;
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::Execute( void )
{
    UFC::BufferedLog::DebugPrintf( " TSystexOrderConnection::Execute" );

    while( !IsTerminated() )
    {        
        CheckConnectionState();        
        UFC::SleepMS( 5000 );
    }    
}
//------------------------------------------------------------------------------
void TSystexOrderConnection::CheckConnectionState( void )
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
        FConnection->Connect( IP->c_str(), FPort );
        delete IP;        
    }    
}
//------------------------------------------------------------------------------
BOOL TSystexOrderConnection::CheckTradeDate( TSQMessage& SQMessage )
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
//------------------------------------------------------------------------------
BOOL TSystexOrderConnection::CheckTradeTime( TSQMessage& SQMessage )
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
void TSystexOrderConnection::RejectOrder( TSQMessage& OvertimeOrder, int Status )
{
    TSQMessage RejectMessage;
    UFC::AnsiString Value,TFXID,CompanyID,BranchID,Market;    
    UFC::AnsiString ErrStr( Status );
    
    ErrStr.PadThis( 2, '0' );
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
    RejectMessage.Append( "40", ErrStr );///< Order Expired.
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
        RejectMessage.Append( "31", Value );
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
    if( OvertimeOrder.Get( "P2",Value))
        RejectMessage.Append( "P2", Value );    
    ///< For Futures
    if( OvertimeOrder.Get( "O4", Value ) )
        RejectMessage.Append( "O4", Value );        
    if( OvertimeOrder.Get( "V5", Value ) )
        RejectMessage.Append( "V5", Value );        
    if( OvertimeOrder.Get( "W3", Value ) )
        RejectMessage.Append( "W3", Value );        
    if( OvertimeOrder.Get( "VT", Value ) )
        RejectMessage.Append( "VT", Value );        
    ///< Send to MBus message.
    MTree ReplyData;    
    
    ReplyData.append( "DATA", RejectMessage.ToString() );
    MessageObj->Send( "CONFIRM", "To400", ReplyData, FALSE );    
    if( Market == "T" ) 
        MessageObj->Send( "TSE.CONFIRM", GLogonID, ReplyData, FALSE );
    else if( Market == "O" ) 
       MessageObj->Send( "OTC.CONFIRM", GLogonID, ReplyData, FALSE );    
    else if( Market == "FUT" ) 
        MessageObj->Send( "FUT.CONFIRM", GLogonID, ReplyData, FALSE );
    else if( Market == "OPT" ) 
        MessageObj->Send( "OPC.CONFIRM", GLogonID, ReplyData, FALSE );    
}
//------------------------------------------------------------------------------
