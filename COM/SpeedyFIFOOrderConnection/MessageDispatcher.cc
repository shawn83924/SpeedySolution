#include "MessageDispatcher.h" 
//------------------------------------------------------------------------------
const UFC::AnsiString CONFIG_FILE( "../cfg/SpeedyFIFOOrderConnection.cfg" );
extern BOOL IsRecoverFinished;   
extern UFC::PInt32 FRecoverBeginTime;
BOOL IsLogon;
//------------------------------------------------------------------------------
MessageDispatcher::MessageDispatcher()
:FAppName( "SpeedyFIFOOrderConnection" )
,FIP( "127.0.0.1" )
,FPort( 23456 )
,FID( "yuan" )
,FPassword( "1234rewq" )
,FAccount( "0000001" )
,FBrokerID( "F021999" )
{
    FConnection = new TTaifexConnection( FAppName, this );
    LoadSetting();
    IsLogon = FALSE;
    UFC::AnsiString LogName = "../log/SpeedyFIFOOrderConnection" + UFC::GetDateString() + ".log";    
    
    FConnection->SetUseNewFuturesSymbol( TRUE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, FBrokerID.c_str() );
    FConnection->SetCMID( FBrokerID.SubString(0,4).c_str() );
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
    
    /// Load infomation to Logon Speedy Gateway 
    if( Config.GetValue( "Setting", "IP", FIP ) == FALSE )
    {
        UFC::BufferedLog::Printf(" Speedy Server IP not set.");
        exit(1);
    }
    if( Config.GetValue( "Setting", "ID",FID ) == FALSE )
    {
        UFC::BufferedLog::Printf(" Logon ID not set.");
        exit(1);
    }    
    if( Config.GetValue( "Setting", "Password", FPassword ) == FALSE )
    {
        UFC::BufferedLog::Printf(" Logon Password not set.");
        exit(1);
    }    
    if( Config.GetValue( "Setting", "Account", FAccount ) == FALSE )
    {
        UFC::BufferedLog::Printf(" Logon Account not set.");
        exit(1);
    }    
    if( Config.GetValue( "Setting", "BrokerID", FBrokerID ) == FALSE )
    {
        UFC::BufferedLog::Printf(" BrokerID not set.");
        exit(1);
    } 
    
    
    for( register int i = 1; i < Config.SectionCount(); i++ )
    {
        UFC::AnsiString SectionName, Branch, Temp, FUTPrefix, OPTPrefix, OrderFile, FUTExecFile, OPTExecFile, Name;
	UFC::AnsiString OrderSubFile, FUTSubPrefix, OPTSubPrefix, FUTSubExecFile, OPTSubExecFile;
        SectionName.Printf( "Pipe%d", i );
        UFC::Section* pSection = Config.GetSection( i );
        if( pSection != NULL )
        { 
	    SpeedyFIFOOrderConnection* SubConnection = NULL;
	    SpeedyFIFOOrderConnection* Connection = NULL;
	    
            pSection->GetValue( "FUTSendFilePrefix", FUTPrefix );
            FUTExecFile.Printf( "%s.%s", FUTPrefix.c_str(), UFC::GetDateString().c_str() );
            
            pSection->GetValue( "OPTSendFilePrefix", OPTPrefix );
            OPTExecFile.Printf( "%s.%s", OPTPrefix.c_str(), UFC::GetDateString().c_str() );
            
            pSection->GetValue( "RecvFileName", OrderFile );
	    
	    Connection = new SpeedyFIFOOrderConnection( FConnection, OrderFile, FUTExecFile, OPTExecFile );
	    
	    UFC::BufferedLog::Printf( " %s FUT Send File:[%s]", SectionName.c_str(), FUTExecFile.c_str() );
	    UFC::BufferedLog::Printf( " %s OPT Send File:[%s]", SectionName.c_str(), OPTExecFile.c_str() );
	    UFC::BufferedLog::Printf( " %s Exec FIFO:[%s]", SectionName.c_str(), OrderFile.c_str() );
	    
	    ///< Load SubAccount
	    if(  pSection->GetValue( "SubRecvFileName", OrderSubFile ) &&
		    pSection->GetValue( "FUTSubSendFilePrefix", FUTSubPrefix ) &&
		    pSection->GetValue( "OPTSubSendFilePrefix", OPTSubPrefix ) )
	    {
		    
		FUTSubExecFile.Printf( "%s.%s", FUTSubPrefix.c_str(), UFC::GetDateString().c_str() );
		OPTSubExecFile.Printf( "%s.%s", OPTSubPrefix.c_str(), UFC::GetDateString().c_str() );
            
		UFC::BufferedLog::Printf( " %s FUT Sub Send File:[%s]", SectionName.c_str(), FUTSubExecFile.c_str() );
		UFC::BufferedLog::Printf( " %s OPT Sub Send File:[%s]", SectionName.c_str(), OPTSubExecFile.c_str() );
		UFC::BufferedLog::Printf( " %s Sub Exec FIFO:[%s]", SectionName.c_str(), OrderSubFile.c_str() );
            
		SubConnection = new SpeedyFIFOOrderConnection( FConnection, OrderSubFile, FUTSubExecFile, OPTSubExecFile );
	    }
            if( pSection->GetValue( "Branch", Temp ) == FALSE )
            {
                exit(0);
            }
            else
            {
                UFC::PStringList StringList;
                StringList.SetStrings( Temp, "," );
                for( register int i = 0; i < StringList.ItemCount(); i++ )
                {
                    UFC::BufferedLog::DebugPrintf( " Branch%d:[%s]", i + 1, StringList.GetItem(i).c_str() );
                    PBrokerList.Add( StringList.GetItem(i), Connection );
		    if( SubConnection != NULL )
			PBrokerSubList.Add( StringList.GetItem(i), SubConnection );
                }
            }
        }
        else
            continue;
    }
}
//------------------------------------------------------------------------------
void MessageDispatcher::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str() );    
}
//------------------------------------------------------------------------------
void MessageDispatcher::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    IsLogon = FALSE;
}
//------------------------------------------------------------------------------
void MessageDispatcher::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
    if( Result == lrOk )
    {
        IsLogon = TRUE;
        UFC::BufferedLog::Printf( " %s( CID=%d )....logon ok", ReplyString, CID );
        
        UFC::AnsiString BeginTime;
        BeginTime.Printf( "%06d", FRecoverBeginTime.ToInt32() );
        
        UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdBoth, rmAll )", BeginTime.c_str() );
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );
    }
    else
    {
        UFC::BufferedLog::Printf( " %s....logon failed", ReplyString );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void MessageDispatcher::Recover( const UFC::AnsiString& BeginTime )
{
    if( IsLogon == TRUE )
    {
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );    
        UFC::BufferedLog::Printf( " Recover from:%s", BeginTime.c_str() );
    }
}    
//------------------------------------------------------------------------------
void MessageDispatcher::OnRecoverFinished( int Count )
{
    UFC::BufferedLog::DebugPrintf( " OnRecoverFinished( Count = %d )", Count );    
    IsRecoverFinished = TRUE;
    UFC::SleepMS( 1000 );    
}
//------------------------------------------------------------------------------
void MessageDispatcher::CheckConnection( void )
{
    if( IsLogon == FALSE )
        FConnection->Connect( FIP.c_str(), FPort, 10 );
}
//------------------------------------------------------------------------------
void MessageDispatcher::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{
    UFC::AnsiString BrokerID( Msg->GetBrokerID() );
    UFC::AnsiString UserData( Msg->GetData() );
    UFC::AnsiString SubAccount = UserData.SubString( 2, 7 );
    SpeedyFIFOOrderConnection* Connection = NULL;
    
    UFC::BufferedLog::DebugPrintf( " BrokerID:[%s]", BrokerID.c_str() );
    UFC::BufferedLog::DebugPrintf( " UserData:[%s]", UserData.c_str() );
    UFC::BufferedLog::DebugPrintf( " SubAccount:[%s]", SubAccount.c_str() );
    
    if( SubAccount == "       " )
	Connection = PBrokerList.GetObjectByKey( BrokerID );
    else
	Connection = PBrokerSubList.GetObjectByKey( BrokerID );
    
    if( Connection != NULL )
    {
	switch( Msg->GetOrderStatus() )
	{
	    case nsOrderMessageDefine::osNew:
	    case nsOrderMessageDefine::osRejected:  
	    case nsOrderMessageDefine::osReplaced:                         
	    case nsOrderMessageDefine::osCanceled:
	    case nsOrderMessageDefine::osPendingReplace:            
	    case nsOrderMessageDefine::osPendingCancel:
		 Connection->ConfirmToBackend( Msg ); 
		 break;
	    case nsOrderMessageDefine::osPartiallyFilled:
	    case nsOrderMessageDefine::osFilled:                 
	    case nsOrderMessageDefine::osPendingNew:                   
		 break;
	    default: break;
	}
    }      
}
//------------------------------------------------------------------------------





