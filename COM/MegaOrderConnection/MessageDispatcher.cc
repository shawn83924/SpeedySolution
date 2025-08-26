#include "MessageDispatcher.h" 
//------------------------------------------------------------------------------
extern UFC::PInt32           FRecoverBeginTime;
extern Int32                 FGroup;
extern UFC::AnsiString       GAppName;
extern UFC::AnsiString       CONFIG_FILE;
extern TConnectionStateEnum  GConnectionState;
//------------------------------------------------------------------------------
MessageDispatcher::MessageDispatcher()
:FIP( "127.0.0.1" )
,FPort( 23456 )
,FID( "yuan" )
,FPassword( "1234rewq" )
,FAccount( "0000000" )
,FBrokerID( "7000" )
,FTFXBrokerID("F030000")
,FTFXCMID("F030")
{
    FConnection = new TTaifexConnection( GAppName, this );
    LoadSetting();    
    UFC::AnsiString LogName = "../log/" + GAppName  + UFC::GetDateString() + ".log";    
    
    ///< Set the broker ID of this connection.    
    FConnection->SetBrokerID( nsOrderMessageDefine::mTSE, FBrokerID.c_str() );       
    ///< Set the broker ID of this connection.   ( TAIFEX)
    FConnection->SetUseNewFuturesSymbol( TRUE ); 
    FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, FTFXBrokerID.c_str() );
    FConnection->SetCMID( FTFXCMID.c_str() );
    ///< Load TSE/OTC message format config files.
    FConnection->SetTSEDataFormatResourceID( IDR_TSE_T010, IDR_TSE_O010, IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
    FConnection->SetOTCDataFormatResourceID( IDR_OTC_T010, IDR_OTC_O010, IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
    FConnection->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010, IDR_TSE_E010, IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020, IDR_TSE_Ex020, "INI" );
    FConnection->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );
    ///< Load TAIFEX message format config files.
    FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );                                
    ///< Set log file name.    
    FConnection->SetLogFile( LogName );
}
//------------------------------------------------------------------------------
void MessageDispatcher::LoadSetting()
{
    UFC::UiniFile Config( CONFIG_FILE );
    UFC::AnsiString Value;
    
    /// Load information to Login Speedy Gateway 
    if( Config.GetValue( "Setting", "IP", FIP ) == FALSE )
    {
        UFC::BufferedLog::Printf(" Speedy Server IP not set.");
        exit(1);
    }
    if( Config.GetValue( "Setting", "ID",FID ) == FALSE )
    {
        UFC::BufferedLog::Printf(" Login ID not set.");
        exit(1);
    }    
    if( Config.GetValue( "Setting", "Password", FPassword ) == FALSE )
    {
        UFC::BufferedLog::Printf(" Login Password not set.");
        exit(1);
    }    
    if( Config.GetValue( "Setting", "Account", FAccount ) == FALSE )
    {
        UFC::BufferedLog::Printf(" Login Account not set.");
        exit(1);
    }    
    if( Config.GetValue( "Setting", "BrokerID", FBrokerID ) == FALSE )
    {
        UFC::BufferedLog::Printf(" BrokerID not set.");
        exit(1);
    } 
    if( Config.GetValue( "Setting", "Group", Value ) == TRUE )    
        FGroup = Value.ToInt();   
    
    if( Config.GetValue( "Setting", "TAIFEXBrokerID", FTFXBrokerID ) == FALSE )
    {
        UFC::BufferedLog::Printf(" TAIFEX BrokerID not set.");
        exit(1);
    } 
    if( Config.GetValue( "Setting", "TAIFEXCMID", FTFXCMID ) == FALSE )
    {
        FTFXCMID = FTFXBrokerID.SubString( 0, 4 );
    } 
    
    key_t  TSEQueueC = 0,OTCQueueC = 0,TSEQueueF = 0,OTCQueueF = 0, TFXExecQueue = 0, OrderQueue = 0;    
    UFC::AnsiString SectionName( "Queue" );	
    	    
    if( Config.GetValue( SectionName, "TSEConfirmQueue", Value )== FALSE )
    {
        UFC::BufferedLog::Printf(" TSEConfirmQueue not set.");
        exit(1);
    } 
    sscanf( Value.c_str(), "%x", &TSEQueueC );
    if( Config.GetValue( SectionName, "TSEFillQueue", Value )== FALSE )
    {
        UFC::BufferedLog::Printf(" TSEFillQueue not set.");
        exit(1);
    } 
    sscanf( Value.c_str(), "%x", &TSEQueueF );
    if( Config.GetValue( SectionName, "OTCConfirmQueue", Value )== FALSE )
    {
        UFC::BufferedLog::Printf(" OTCConfirmQueue not set.");
        exit(1);
    } 	
    sscanf( Value.c_str(), "%x", &OTCQueueC );
    if( Config.GetValue( SectionName, "OTCFillQueue", Value )== FALSE )
    {
        UFC::BufferedLog::Printf(" OTCFillQueue not set.");
        exit(1);
    } 	
    sscanf( Value.c_str(), "%x", &OTCQueueF );
    if( Config.GetValue( SectionName, "OrderQueue", Value )== FALSE )
    {   
        UFC::BufferedLog::Printf(" OrderQueue not set.");
        exit(1);
    } 	
    sscanf( Value.c_str(), "%x", &OrderQueue );
    
    if( Config.GetValue( SectionName, "TAIFEXExecQueue", Value )== FALSE )
    {   
        UFC::BufferedLog::Printf(" TAIFEXExecQueue not set.");
        exit(1);
    } 	
    sscanf( Value.c_str(), "%x", &TFXExecQueue );    
    if( OrderQueue==0|| TSEQueueC==0|| TSEQueueF==0|| OTCQueueC==0|| OTCQueueF==0 || TFXExecQueue ==0 )
    {
        UFC::BufferedLog::Printf(" Invalid Queue Key.");
        exit(1);	
    }
    FPipe = new SpeedyQueueOrderConnection( FConnection, OrderQueue, TSEQueueC, TSEQueueF, OTCQueueC, OTCQueueF,TFXExecQueue  );	    
    UFC::BufferedLog::Printf( " [%s] Order Queue       [0x%08x]", SectionName.c_str(), OrderQueue );    
    UFC::BufferedLog::Printf( " [%s] TSE Confirm Queue [0x%08x]", SectionName.c_str(), TSEQueueC );
    UFC::BufferedLog::Printf( " [%s] TSE Fill Queue    [0x%08x]", SectionName.c_str(), TSEQueueF );
    UFC::BufferedLog::Printf( " [%s] OTC Confirm Queue [0x%08x]", SectionName.c_str(), OTCQueueC );
    UFC::BufferedLog::Printf( " [%s] OTC Fill Queue    [0x%08x]", SectionName.c_str(), OTCQueueF );    
    UFC::BufferedLog::Printf( " [%s] TAIFEX Exec Queue [0x%08x]", SectionName.c_str(), TFXExecQueue );    
    UFC::BufferedLog::Printf( " Speedy Gateway Order Group[%d]", FGroup );    
}
//------------------------------------------------------------------------------
void MessageDispatcher::OnConnected( void )
{
    UFC::BufferedLog::Printf( " Login( ID[%s], Password[%s] )", FID.c_str(), FPassword.c_str() );
    GConnectionState = csConnected;
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str() );    
}
//------------------------------------------------------------------------------
void MessageDispatcher::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    GConnectionState = csDisconnect;
}
//------------------------------------------------------------------------------
void MessageDispatcher::OnLogonReply( const char* ReplyString, LogonResult Accepted, int CID )
{
    if( Accepted == lrOk )
    {        
        UFC::BufferedLog::Printf( " %s( CID=%d )....Login ok", ReplyString, CID );
        
        UFC::AnsiString BeginTime;
        BeginTime.Printf( "%06d", FRecoverBeginTime.ToInt32() );
        
        UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdBoth, rmAll )", BeginTime.c_str() );
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );
    }
    else
    {
        UFC::BufferedLog::Printf( " %s....Login failed", ReplyString );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void MessageDispatcher::Recover( const UFC::AnsiString& BeginTime )
{
    FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );    
    UFC::BufferedLog::Printf( " Recover from:%s", BeginTime.c_str() );    
}    
//------------------------------------------------------------------------------
void MessageDispatcher::OnRecoverFinished( int Count )
{
    UFC::BufferedLog::DebugPrintf( " OnRecoverFinished( Count = %d )", Count );            
    GConnectionState = csReady;
    UFC::SleepMS( 1000 );    
}
//------------------------------------------------------------------------------
void MessageDispatcher::CheckConnection( void )
{
    if( GConnectionState == csDisconnect )
    {
	GConnectionState = csConnecting;
        FConnection->Connect( FIP.c_str() , FPort, 10 );    
    }
}
//------------------------------------------------------------------------------
void MessageDispatcher::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{
    switch( Msg->GetOrderStatus() )
    {
	case nsOrderMessageDefine::osNew:
	case nsOrderMessageDefine::osRejected:  
             FPipe->ConfirmToBackend( Msg ); 
             break;
        case nsOrderMessageDefine::osCanceled:            
            if( Msg->GetMarket() == nsOrderMessageDefine::mTWFutures || Msg->GetMarket() == nsOrderMessageDefine::mTWOptions)
                FPipe->FillToBackend( Msg ); 
            else
                FPipe->ConfirmToBackend( Msg ); 
	     break;             
	case nsOrderMessageDefine::osReplaced:
             if( Msg->GetMarket() == nsOrderMessageDefine::mTWFutures || Msg->GetMarket() == nsOrderMessageDefine::mTWOptions)
             {
                if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
                    Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 ) 
                    FPipe->ConfirmToBackend( Msg ); 
                else
                    FPipe->FillToBackend( Msg ); 
             }
             else
                 FPipe->ConfirmToBackend( Msg ); 
	     break;
	case nsOrderMessageDefine::osPartiallyFilled:
	case nsOrderMessageDefine::osFilled:                 
	     FPipe->FillToBackend( Msg ); 
	     break;
	case nsOrderMessageDefine::osPendingReplace:            
	case nsOrderMessageDefine::osPendingCancel:             
	case nsOrderMessageDefine::osPendingNew:                   
	     break;
        case nsOrderMessageDefine::osQuoteRequestAccept:
             FPipe->ConfirmToBackend( Msg );    
	default: break;
    }
}
//------------------------------------------------------------------------------





