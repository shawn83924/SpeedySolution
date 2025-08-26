#include "YuantaExecConnection.h"
#include "../../UFC/NetUtility.h"
//------------------------------------------------------------------------------
extern BOOL            DumpOnly;
extern BOOL            DisableLog;
//------------------------------------------------------------------------------
YuantaExecConnection::YuantaExecConnection( void )
:UFC::PThread( NULL, FALSE )
,FAppName( "SpeedyToYB" )
,FIP( "127.0.0.1" )
,FPort( 23456 )
,FID( "yuan" )
,FPassword( "1234rewq" )
,FAccount( "0000001" )
,FBrokerID( "F006000" )
,FIsLogon( FALSE )
,FToBackend( TRUE )
,FToChanter( FALSE )
,FFillToChanterOnly( FALSE )
,FFUTExecutionQueue( FALSE )
,FOPTExecutionQueue( FALSE )
,FChanterReportQueue( FALSE )
{     
    UFC::AnsiString LogName = "../log/SToYB" + UFC::GetDateString() + ".log";
    
    LoadSetting( ); ///< Load setting from config file.
    if( !FOPTExecutionQueue.Open( YUANTA_OPT_QUEUE, TRUE ) )
    {
        UFC::BufferedLog::Printf( " Open Options Execution queue 0x%x failed.", YUANTA_OPT_QUEUE);
        exit( 0 );
    }    
    if( !FFUTExecutionQueue.Open( YUANTA_FUT_QUEUE, TRUE ) )
    {
        UFC::BufferedLog::Printf( " Open Futures Execution queue 0x%x failed.", YUANTA_FUT_QUEUE);
        exit( 0 );
    } 
    if( FToChanter == true )
    {  
    	if( !FChanterReportQueue.Open( YUANTA_CHANTER_REPORT_QUEUE, TRUE ) )     
    	{
            UFC::BufferedLog::Printf( " Open Chanter Report Queue 0x%x failed.", YUANTA_CHANTER_REPORT_QUEUE);
            exit( 0 );
    	} 
    }
    SplashScreen( );
    ///< Create the connection object.
    FConnection = new TTaifexConnection( FAppName, this );
    FConnection->SetUseNewFuturesSymbol( TRUE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, FBrokerID.c_str() );
    FConnection->SetCMID( FBrokerID.SubString(0,4).c_str() );
    ///< Load TAIFEX message format config files.
    FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );
    ///< Set log file name.    
    FConnection->SetLogFile( LogName );
    ///< Start running thread.
    Start();
}
//------------------------------------------------------------------------------
void YuantaExecConnection::SplashScreen( void )
{
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   Speedy To Yuanta Backend module        " );
    UFC::BufferedLog::Printf( "   Startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Mode:On (Dump to stdout only)");    
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off (Write to queue)");    
    if( DisableLog == TRUE ) 
        UFC::BufferedLog::Printf( "   Execution Log:Off");    
    else
        UFC::BufferedLog::Printf( "   Execution log:On");        
    UFC::BufferedLog::Printf( "__________________________________________" );           
}
//------------------------------------------------------------------------------
void YuantaExecConnection::LoadSetting( void )
{
    try
    {   
        UFC::AnsiString Value;
        UFC::UiniFile   Config( "../cfg/SpeedyToYB.cfg" );
        
        if( Config.GetValue( "Setting", "IP", FIP ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Speedy Server IP not set in [Setting].");
            exit(1);
        }
        if( Config.GetValue( "Setting", "ID",FID ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Logon ID not set in [Setting].");
            exit(1);
        }    
        if( Config.GetValue( "Setting", "Password", FPassword ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Logon Password not set in [Setting].");
            exit(1);
        }    
        if( Config.GetValue( "Setting", "Account", FAccount ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Logon Account not set in [Setting].");
            exit(1);
        }    
        if( Config.GetValue( "Setting", "BrokerID", FBrokerID ) == FALSE )
        {
            UFC::BufferedLog::Printf(" BrokerID not set in [Setting].");
            exit(1);
        }
        if( Config.GetValue( "Setting", "IB", FIB ) == FALSE )
        {
            UFC::BufferedLog::Printf(" IB not set in [Setting].");
            exit(1);
        }
        if( Config.GetValue( "Setting", "ChannelCode", FChannelCode ) == FALSE )
        {
            UFC::BufferedLog::Printf(" ChannelCode not set in [Setting].");
            exit(1);
        }
        
        if( Config.GetValue( "SpeedyToYB", "IsSendBackend", Value  ) == FALSE )
        {
            UFC::BufferedLog::Printf(" IsSendBackend not set in [SpeedyToYB].");
            exit(1);
        }
        else
        {
            if( Value == "N" )
                FToBackend = FALSE;
        }
        if( Config.GetValue( "SpeedyToYB", "IsSendChanterReport", Value  ) == FALSE )
        {
            UFC::BufferedLog::Printf(" IsSendChanterReport not set in [SpeedyToYB].");
            exit(1);
        }
        else
        {
            if( Value == "Y" )
                FToChanter = TRUE;
        }
        if( Config.GetValue( "SpeedyToYB", "IsFillChanterReport", Value  ) == FALSE )
        {
            UFC::BufferedLog::Printf(" IsFillChanterReport not set in [SpeedyToYB].");
            exit(1);
        }
        else
        {
            if( Value == "Y" )
                FFillToChanterOnly = TRUE;
        }
        FIB.PadThis( 3, '0' ); 
        FChannelCode.PadThis( 3, ' ' ); 
        FBrokerID.PadThis( 7, '0' );         
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" SpeedyToYB.cfg not found.");
        exit(1);
    }
}
//------------------------------------------------------------------------------
YuantaExecConnection::~YuantaExecConnection( void )
{
    if( FConnection != NULL )
        delete FConnection;     
}
//------------------------------------------------------------------------------
void YuantaExecConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " Try to Logon ID[%s] Password[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str(), ctBoth );    
}
//------------------------------------------------------------------------------
void YuantaExecConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void YuantaExecConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
    if( Result == lrOk )
    {
        FIsLogon = TRUE;
        UFC::BufferedLog::Printf( " %s( CID=%d )....logon ok", ReplyString, CID );       
    }
    else
    {
        UFC::BufferedLog::Printf( " Logon failed. Reason:%s", ReplyString );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void YuantaExecConnection::GenerateR020( int Func, TExecutionReportMessage* Msg, char* DataBuffer )
{    
    UFC::AnsiString TFXSymbol( Msg->GetSymbol() );
    UFC::AnsiString TFXBrokerID( Msg->GetBrokerID());
    UFC::AnsiString TFXOrderID( Msg->GetOrderID());
    UFC::AnsiString TFXAccount( Msg->GetAccount());
    UFC::AnsiString TFXAccountFlag( Msg->GetAccountFlag());
    UFC::AnsiString TFXCMID( Msg->GetCMID());
    UFC::AnsiString TFXTransTime( Msg->GetTransactTime(), 8 );
    UFC::AnsiString TimeNow;
    char            Side,TIF,PE,OrdType;
    int             PxInt = UFC::DoubleToInt( Msg->GetPrice(), Msg->GetPxDigit() );
    
    UFC::GetHHMMSS( TimeNow );
    TFXSymbol.PadThis( 20 ,' ' );
    TFXBrokerID.PadThis( 7 ,'0' );
    
    if( Msg->GetSide() == nsOrderMessageDefine::sBuy )
        Side = 'B';
    else
        Side = 'S';
    if( Msg->GetOrderType( ) == nsOrderMessageDefine::otLimit )
        OrdType = 'L';
    else
        OrdType = 'M';
    switch( Msg->GetTimeInForce( ))
    { 
        case nsOrderMessageDefine::tifIOC: TIF = 'I';break; 
        case nsOrderMessageDefine::tifFOK: TIF = 'F';break;
        default:                           TIF = 'R';break;
    }
    switch( Msg->GetPositionEffect( ))
    { 
        case nsOrderMessageDefine::peOpen:          PE= '0';break;
        case nsOrderMessageDefine::peClose:         PE= '1';break;
        case nsOrderMessageDefine::peRolled:        PE= '2';break;
        case nsOrderMessageDefine::peDayTradeOpen:  PE= '2';break;
        case nsOrderMessageDefine::peTMPMarketMaker:PE= '9';break;
        default:                                    PE= '0';break;
    }
    sprintf(DataBuffer, "30%02d01%s00%s%s%s%c%s%c%c%09d%04d%c%c%s%04d%04d%s%08d",
            Func,
            TimeNow.c_str(),
            TFXBrokerID.c_str(),
            TFXOrderID.c_str(),
            TFXAccount.c_str(),
            TFXAccountFlag[0],
            TFXSymbol.c_str(),
            Side,
            OrdType,
            PxInt,
            (int)Msg->GetTMPQty(),            
            TIF,
            PE,
            TFXCMID.c_str(),
            (int)Msg->GetBeforeQty(),
            (int)Msg->GetAfterQty(),
            TFXTransTime.c_str(),
            (int)Msg->GetReportSequence());
}
//------------------------------------------------------------------------------
void YuantaExecConnection::ToBackend( TExecutionReportMessage* Msg )
{
    UFC::AnsiString SID( Msg->GetPVC() );
    UFC::AnsiString AE( Msg->GetAE() );
    UFC::AnsiString IB( FIB );
    UFC::AnsiString CHC( FChannelCode );
    UFC::AnsiString UsrData( Msg->GetData());
    UFC::AnsiString ErrorCode( Msg->GetStatusCode() );
    UFC::AnsiString CA,UDD,CAUDDToChanter,CAUDD,IP,ID,GWID;
    char            DataBuffer[ 256 ];   
    char            R020Buffer[ 256 ];   
    long            MType;           
    
    ///< Begin to YuanTa backend
    ///< YuanTa UsrData format
    ///< IB(3) + Channel code(3) + IP(15) + UserDef(23)
    if( UsrData.Length() >= 44 )
    {
    	IB   = UsrData.SubString( 0, 3 );
    	CHC  = UsrData.SubString( 3, 3 );
        IP   = UsrData.SubString( 6, 15 );
        GWID = UsrData.SubString( 36, 8 );
    }
    IB.PadThis( 3, '0' );
    CHC.PadThis( 3, '0' );
    IP.PadThis( 15, ' ' );
    SID.PadThis( 3, '0' );    
    AE.PadThis( 4, ' ' );
    CA.PadThis( 23 , ' ' );///< 23 for CA
    UDD.PadThis( 15, ' ' );///< 15 for UDD
    ID.PadThis( 11, ' ' );
    GWID.PadThis( 8, '0' );
    CAUDD.Printf( "%s%s", CA.c_str(), UDD.c_str() );
    MType = 0x70000000 + (((long)(SID[0]-'0'))<<8) + (((long)(SID[1]-'0'))<<4)+(long)(SID[2]-'0');
    
    switch( Msg->GetOrderStatus() ) 
    {
            
            case nsOrderMessageDefine::osNew:            sprintf( DataBuffer, "%s%010d%s%s%s%s%s%s%s", Msg->GetSrc(), (int)Msg->GetTMPUniqueID(), AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDD.c_str(), IP.c_str(), ID.c_str() );
                                                         break;
            case nsOrderMessageDefine::osQuoteAccept:    sprintf( DataBuffer, "%s%010d%s%s%s%s%s%s%s", Msg->GetSrc(), (int)0, AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDD.c_str(), IP.c_str(), ID.c_str() );
                                                         break;
            case nsOrderMessageDefine::osCanceled:       if( Msg->GetPositionEffect() == nsOrderMessageDefine::peTMPMarketMaker )
                                                         {
                                                             sprintf( DataBuffer, "%s%s%s%s%s%s%s%s", Msg->GetSrc(), AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDD.c_str(), IP.c_str(), ID.c_str() );
                                                         }           
                                                         else if( Msg->GetSrc()[0] == '1' || Msg->GetSrc()[0] == '2' )
                                                         {
                                                            sprintf( DataBuffer, "%s%s%s%s%s%s%s%s", Msg->GetSrc(), AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDD.c_str(), IP.c_str(), ID.c_str() );
                                                         }
                                                         else
                                                         {                                           
                                                            GenerateR020( 3, Msg, R020Buffer );       
                                                            sprintf( DataBuffer, "%s%010d%s%s%s%s%s%s%s", R020Buffer, (int)Msg->GetTMPUniqueID(), AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDD.c_str(), IP.c_str(), ID.c_str() );
                                                         }
                                                         break;                                                         
            case nsOrderMessageDefine::osReplaced:       if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
                                                             Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )///< ExecType = 'M' or 'm'
                                                         {
                                                            sprintf( DataBuffer, "%s%010d%s%s%s%s%s%s%s", Msg->GetSrc(),(int)Msg->GetTMPUniqueID(), AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDD.c_str(), IP.c_str(), ID.c_str() );
                                                         }   
                                                         else  ///< Reduce Qty
                                                         {
                                                             GenerateR020( 2, Msg, R020Buffer );       
                                                             sprintf( DataBuffer, "%s%010d%s%s%s%s%s%s%s", R020Buffer, (int)Msg->GetTMPUniqueID(), AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDD.c_str(), IP.c_str(), ID.c_str() );
                                                         }
                                                         break;                                                                     
            case nsOrderMessageDefine::osPartiallyFilled:
            case nsOrderMessageDefine::osFilled:         sprintf( DataBuffer, "%s%s%s%s%s%s%s%s", Msg->GetSrc(), AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDD.c_str(), IP.c_str(), ID.c_str() );
                                                         break;
            case nsOrderMessageDefine::osRejected:       if( ErrorCode.ToInt() == 97 )
                                                             return;
                                                         sprintf( DataBuffer, "%s%010d%s%s%s%s%s%s%s", Msg->GetSrc(), (int)Msg->GetTMPUniqueID(), AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDD.c_str(), IP.c_str(), ID.c_str() );
                                                         break;
            default: return; ///< Not handled
    }        
    UFC::UQueueData OrderData( MType, DataBuffer, strlen( DataBuffer ) );                                    
    
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTWFutures:
            UFC::BufferedLog::DebugPrintf( " <FUT Execution> <Mtype:0x%x> <%s>", MType, DataBuffer );
            if( FToBackend == TRUE && DumpOnly == FALSE )
                FFUTExecutionQueue.Send( OrderData );     
            break; 
        case nsOrderMessageDefine::mTWOptions:
            UFC::BufferedLog::DebugPrintf( " <OPT Execution> <Mtype:0x%x> <%s>",MType, DataBuffer );
            if( FToBackend == TRUE &&  DumpOnly == FALSE )
                FOPTExecutionQueue.Send( OrderData );     
            break;
        default:
            break;
    }        
    ///< End to YuanTa backend
    if( FToChanter == true ) 
    {    	
        BOOL IsConfirm = TRUE;
        
        CAUDDToChanter.PadThis( 23 + 15, '0' );
        switch( Msg->GetOrderStatus() ) 
        {
            
            case nsOrderMessageDefine::osNew:            IsConfirm = true;
                                                         sprintf( DataBuffer, "%s%010d%s%s%s%s%s%s", Msg->GetSrc(), (int)Msg->GetTMPUniqueID(), AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDDToChanter.c_str(), SID.c_str() );
            						 break;
            case nsOrderMessageDefine::osQuoteAccept:    IsConfirm = true;
            						 GenerateR020( 1, Msg, R020Buffer );         			
            						 sprintf( DataBuffer, "%s%010d%s%s%s%s%s%s", R020Buffer, 0, AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDDToChanter.c_str(), SID.c_str() );
                                                         break;
            case nsOrderMessageDefine::osReplaced:       IsConfirm = true;
                                                         if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
                                                             Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )///< ExecType = 'M' or 'm'
                                                         {
                                                            sprintf( DataBuffer, "%s%010d%s%s%s%s%s%s", Msg->GetSrc(), (int)Msg->GetTMPUniqueID(), AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDDToChanter.c_str(), SID.c_str() );
                                                         }   
                                                         else  ///< Reduce Qty
                                                         {
                                                            GenerateR020( 2, Msg, R020Buffer );       
                                                            sprintf( DataBuffer, "%s%010d%s%s%s%s%s%s", R020Buffer, (int)Msg->GetTMPUniqueID(), AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDDToChanter.c_str(), SID.c_str() );
                                                         }
                                                         break;     
            case nsOrderMessageDefine::osCanceled:       IsConfirm = true;
                                                         GenerateR020( 3, Msg, R020Buffer );       
                                                         sprintf( DataBuffer, "%s%010d%s%s%s%s%s%s", R020Buffer, (int)Msg->GetTMPUniqueID(), AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDDToChanter.c_str(), SID.c_str() );
                                                         break;
            case nsOrderMessageDefine::osPartiallyFilled:
            case nsOrderMessageDefine::osFilled:         IsConfirm = false;
            						 sprintf( DataBuffer, "%s%s%s%s%s%s%s", Msg->GetSrc(), AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDDToChanter.c_str(), SID.c_str() );
                                                         break;///< Send to backend.
            case nsOrderMessageDefine::osRejected:       if( ErrorCode.ToInt() == 97 )
                                                             return;
                                                         IsConfirm = true;
                                                         sprintf( DataBuffer, "%s%010d%s%s%s%s%s%s", Msg->GetSrc(), (int)Msg->GetTMPUniqueID(), AE.c_str(), IB.c_str(), GWID.c_str(), CHC.c_str(), CAUDDToChanter.c_str(), SID.c_str() );
                                                         break;
            default: return; ///< Not handled
        }
        if( IsConfirm == TRUE ) ///< Confirm
        {
            switch( Msg->GetMarket() ) 
            {
                case nsOrderMessageDefine::mTWFutures: MType = 0x75;break;
                case nsOrderMessageDefine::mTWOptions: MType = 0x77;break;
                default: return;
            }
            if( FFillToChanterOnly == false ) 
            {
                UFC::UQueueData OrderData( MType, DataBuffer, strlen( DataBuffer ) );
                
                UFC::BufferedLog::DebugPrintf( " <Chanter Report Confirm> <Mtype:0x%x> <%s>", MType, DataBuffer );
                if( DumpOnly == FALSE )
                    FChanterReportQueue.Send( OrderData );
            }
        }
        else ///< Filled
        {
            switch( Msg->GetMarket() ) 
            {
                case nsOrderMessageDefine::mTWFutures: MType = 0x76;break;
                case nsOrderMessageDefine::mTWOptions: MType = 0x78;break;
                default: return;
            }
            UFC::UQueueData OrderData( MType, DataBuffer, strlen( DataBuffer ) );
            
            UFC::BufferedLog::DebugPrintf( " <Chanter Report Fill> <Mtype:0x%x> <%s>", MType, DataBuffer );
            if( DumpOnly == FALSE )
                FChanterReportQueue.Send( OrderData );
        }
    }
}
//------------------------------------------------------------------------------
void YuantaExecConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{ 
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osNew:
        case nsOrderMessageDefine::osQuoteAccept:
        case nsOrderMessageDefine::osExpired:
        case nsOrderMessageDefine::osReplaced:            
        case nsOrderMessageDefine::osCanceled:
        case nsOrderMessageDefine::osPartiallyFilled:
        case nsOrderMessageDefine::osFilled:
        case nsOrderMessageDefine::osRejected:            
             ToBackend( Msg );
             break;///< Send to backend.                
        case nsOrderMessageDefine::osPendingNew:
        case nsOrderMessageDefine::osPendingReplace:            
        case nsOrderMessageDefine::osPendingCancel:           
        default: break; ///< Not handled
    }           
}
//------------------------------------------------------------------------------
void YuantaExecConnection::Execute( void )
{      
    while( !IsTerminated() )
    {
        if( FIsLogon == FALSE )
            FConnection->Connect( FIP.c_str() , FPort, 3 );    
        sleep( 5 ); ///< wait 10 sec.
    }
}
//------------------------------------------------------------------------------
