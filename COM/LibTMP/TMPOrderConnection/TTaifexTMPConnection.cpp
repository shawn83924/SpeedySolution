#include "TTaifexTMPConnection.h"
//------------------------------------------------------------------------------
const UFC::AnsiString SUBJECT_RECOVER("RECOVER");
const UFC::AnsiString SUBJECT_ADMIN("OG.ADMIN");
//------------------------------------------------------------------------------
const UFC::AnsiString SUBJECT_ORDER_FUT("TO.TMP.FUT");
const UFC::AnsiString SUBJECT_ORDER_OPT("TO.TMP.OPT");
//------------------------------------------------------------------------------
const UFC::AnsiString SUBJECT_CONFIRM_FUT("CONFIRM.FUT");
const UFC::AnsiString SUBJECT_CONFIRM_OPT("CONFIRM.OPT");
//------------------------------------------------------------------------------
const UFC::AnsiString SUBJECT_FILL_FUT("FILL.FUT");
const UFC::AnsiString SUBJECT_FILL_OPT("FILL.OPT");
//------------------------------------------------------------------------------
const UFC::AnsiString SUBJECT_FT_FUT("TO.FT.FUT");
const UFC::AnsiString SUBJECT_FT_OPT("TO.FT.OPT");
//------------------------------------------------------------------------------
TTaifexTMPConnection::TTaifexTMPConnection( HINSTANCE AppInstance,
											HMODULE ResourceInstance,
											const char* AppName,
											IOrderConnectionEventListener* pOrderConnectionEventListener,
											BOOL  IsWin32GUIApp )
:FInstance( AppInstance )
,FResourceInstance( ResourceInstance )
,FAppName( AppName )
,FID( "" )
,FUniquekey( "" )
,FTAIFEXBrokerID( "F099000" )
,FCMID( "F099" )
,FCMIDIndex( 0 )
,FBrokerIndex( 0 )
,FAdmin( 0 )
,FCurrentConnectionID( 0 )
,FIsLogon( false)
,FVersion( 10000 )
,FTriggerExec( TRUE )
,FCanSendOrder( TRUE )
,FListener( pOrderConnectionEventListener )
,FTransport( NULL )
,FAdminListener( NULL )
,FRecoverListener( NULL )
,FFutConfirmListener( NULL )
,FOptConfirmListener( NULL )
,FFutExecuteListener( NULL )
,FOptExecuteListener( NULL )
,FTAIFEXError( NULL )
,FSendEventListener( NULL )
,FOnOptConfirm( &TTaifexTMPConnection::ReceiveOptConfirmMessage )
,FOnFutConfirm( &TTaifexTMPConnection::ReceiveFutConfirmMessage )
,FOnOptFill( &TTaifexTMPConnection::ReceiveOptExecuteMessage )
,FOnFutFill( &TTaifexTMPConnection::ReceiveFutExecuteMessage)
,FOnAdminMsg( &TTaifexTMPConnection::ReceiveAdminMessage )
,FOnRecoverMsg( &TTaifexTMPConnection::ReceiveRecoverExecution )
{
	FTransport = new TMdTransport( FInstance, this, FAppName,  IsWin32GUIApp );
	SetLanguage( mlEnglish );
	FCallbackFuncs.Add( SUBJECT_CONFIRM_FUT, &FOnFutConfirm );
	FCallbackFuncs.Add( SUBJECT_CONFIRM_OPT, &FOnOptConfirm );
	FCallbackFuncs.Add( SUBJECT_FILL_FUT, &FOnFutFill );
	FCallbackFuncs.Add( SUBJECT_FILL_OPT, &FOnOptFill );
	FCallbackFuncs.Add( SUBJECT_ADMIN,    &FOnAdminMsg   );
	FCallbackFuncs.Add( SUBJECT_RECOVER,  &FOnRecoverMsg );
}
//------------------------------------------------------------------------------
TTaifexTMPConnection::TTaifexTMPConnection( const char* AppName,
											IOrderConnectionEventListener* pOrderConnectionEventListener,
											BOOL  IsWin32GUIApp )
:FInstance( NULL )
,FResourceInstance( NULL)
,FAppName( AppName )
,FID( "" )
,FUniquekey( "" )
,FTAIFEXBrokerID( "F099000" )
,FCMID( "F099" )
,FCMIDIndex( 0 )
,FBrokerIndex( 0 )
,FAdmin( 0 )
,FCurrentConnectionID( 0 )
,FIsLogon( false)
,FVersion( 10000 )
,FTriggerExec( TRUE )
,FCanSendOrder( TRUE )
,FListener( pOrderConnectionEventListener )
,FTransport( NULL )
,FAdminListener( NULL )
,FRecoverListener( NULL )
,FFutConfirmListener( NULL )
,FOptConfirmListener( NULL )
,FFutExecuteListener( NULL )
,FOptExecuteListener( NULL )
,FTAIFEXError( NULL )
,FOnOptConfirm( &TTaifexTMPConnection::ReceiveOptConfirmMessage )
,FOnFutConfirm( &TTaifexTMPConnection::ReceiveFutConfirmMessage )
,FOnOptFill( &TTaifexTMPConnection::ReceiveOptExecuteMessage )
,FOnFutFill( &TTaifexTMPConnection::ReceiveFutExecuteMessage)
,FOnAdminMsg( &TTaifexTMPConnection::ReceiveAdminMessage )
,FOnRecoverMsg( &TTaifexTMPConnection::ReceiveRecoverExecution )
{
	FTransport = new TMdTransport( FInstance, this, FAppName,  IsWin32GUIApp  );
	SetLanguage( mlEnglish );
	FCallbackFuncs.Add( SUBJECT_CONFIRM_FUT, &FOnFutConfirm );
	FCallbackFuncs.Add( SUBJECT_CONFIRM_OPT, &FOnOptConfirm );
	FCallbackFuncs.Add( SUBJECT_FILL_FUT, &FOnFutFill );
	FCallbackFuncs.Add( SUBJECT_FILL_OPT, &FOnOptFill );
	FCallbackFuncs.Add( SUBJECT_ADMIN,    &FOnAdminMsg   );
	FCallbackFuncs.Add( SUBJECT_RECOVER,  &FOnRecoverMsg );
}
//------------------------------------------------------------------------------
TTaifexTMPConnection::~TTaifexTMPConnection( void )
{
	Disconnect();
	delete FTransport;
}
//------------------------------------------------------------------------------
void TTaifexTMPConnection::CreateFormatFiles( void )
{
	CreateFileFromResource( "../TMPFormats",IDR_TMP_R01, "INI" );
	CreateFileFromResource( "../TMPFormats",IDR_TMP_R02, "INI" );
	CreateFileFromResource( "../TMPFormats",IDR_TMP_R22, "INI" );
	CreateFileFromResource( "../TMPFormats",IDR_TMP_R03, "INI" );
	CreateFileFromResource( "../TMPFormats",IDR_TMP_R07, "INI" );
	CreateFileFromResource( "../TMPFormats",IDR_TMP_R08, "INI" );
	CreateFileFromResource( "../TMPFormats",IDR_TMP_R09, "INI" );
	CreateFileFromResource( "../TMPFormats",IDR_TMP_R11, "INI" );
	CreateFileFromResource( "../TMPFormats",IDR_TMP_R12, "INI" );
	CreateFileFromResource( "../TMPFormats",IDR_TMP_R13, "INI" );
	CreateFileFromResource( "../TMPFormats",IDR_TMP_R14, "INI" );
}
//------------------------------------------------------------------------------
void TTaifexTMPConnection::SetLogFile( const char* FileName )
{
	try
	{
		UFC::BufferedLog* LogObject;

		UFC::BufferedLog::SetDebugMode( TRUE );
		UFC::BufferedLog::SetDebugMode( UFC::dlInformation );

		LogObject = new UFC::BufferedLog( FileName, 4096, TRUE );
		UFC::BufferedLog::SetLogObject( LogObject );
	}
	catch( UFC::FileException&  )
	{
		UFC::BufferedLog::Printf( " Open log file [%s] failed.", FileName );
	}
}
//------------------------------------------------------------------------------
void TTaifexTMPConnection::CreateSymbolFile( MTree* Data )
{
	UFC::CreateDir( "../ftFUT" );
	CreateFileFromMNode( Data, "../ftFUT","P06.20" );
	CreateFileFromMNode( Data, "../ftFUT","P08.20" );

	UFC::CreateDir( "../ftOPT" );
	CreateFileFromMNode( Data, "../ftOPT","P06.10" );
	CreateFileFromMNode( Data, "../ftOPT","P08.10" );
}
//------------------------------------------------------------------------------
void TTaifexTMPConnection::SetLanguage( MessageLanguage Lenguage )
{
	UFC::AnsiString TempFileName;

	if( FTAIFEXError != NULL )
		delete FTAIFEXError;
	UFC::CreateDir( "Temp" );
	if( Lenguage == mlEnglish )
		TempFileName = CreateFileFromResource( "Temp", IDR_ERROR_ENG, "INI" );
	else
		TempFileName = CreateFileFromResource( "Temp", IDR_ERROR_CHT, "INI" );
	if( TempFileName.Length() > 0 )
	{
		FTAIFEXError = new UFC::UiniFile( TempFileName );
		UFC::DeleteFile( TempFileName.c_str() );
	}
}
//---------------------------------------------------------------------------
UFC::AnsiString TTaifexTMPConnection::CreateFileFromMNode( MTree* Data, const char* Dir, const char* Name )
{
	char* Buffer;
	int   Size;

	if( Data->get( Name, Buffer, Size ) && Size > 0 )
	{
		UFC::AnsiString FileName;
		UFC::CreateDir( Dir );
		FileName.Printf( "%s/%s", Dir, Name );

		try
		{
			UFC::FileStreamEx TempFile( FileName.c_str(), "w");
			TempFile.Write( Buffer, Size );
			TempFile.Flush();
			return FileName;
		}
		catch( UFC::Exception& e )
		{
			UFC::BufferedLog::Printf( " open file %s error:%s.", FileName.c_str(), e.what() );
		}
	}
	else
		UFC::BufferedLog::Printf( " Node %s not found in tree.", Name );
	return "";
}
//---------------------------------------------------------------------------
UFC::AnsiString TTaifexTMPConnection::CreateFileFromResource( const char* Dir, DWORD ResourceID, const char* ResourceType )
{
	UFC::AnsiString ResourceName, FileName, ResourceFileName;
	try
	{

		if( GetResourceName( ResourceID, ResourceFileName ) == TRUE )
		{
			UFC::CreateDir( Dir );
			ResourceName.Printf( "#%d", ResourceID );
			FileName.Printf( "%s/%s", Dir, ResourceFileName.c_str() );
			TResource Resource( FResourceInstance, ResourceName.c_str(), ResourceType );
			Resource.SaveToFile( FileName );
			return FileName;
		}
		else
		{
			UFC::BufferedLog::Printf( " Resource:[%d] not exists.", ResourceID );
			return "";
		}
	}
	catch( UFC::Exception& e )
	{
		UFC::BufferedLog::Printf( " Get Resource [%d]failed:%s", ResourceID, e.what() );
        return "";
    }    
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::OnMAppConnected( void )
{
    if( FListener != NULL )
    {
        UFC::SleepMS( 330 );
        FListener->OnConnected();
    }
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::OnMAppDisconnected( void )
{
    if( FListener != NULL )
        FListener->OnDisconnected();
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::OnMAppError( PMAppError )
{
    if( FListener != NULL )
        FListener->OnDisconnected();
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString&,  MTree* pTree )
{
	MessageHandlerFunc* Callback;

	if( FListener != NULL && (Callback = FCallbackFuncs.GetObjectByKey(Subject)) != NULL )
		(this->**Callback)( pTree );
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::ReceiveRecoverExecution( MTree* pTree )
{
    Int32 Market,Type;

    if( pTree->get( "MARKET", Market ) && pTree->get( "TYPE", Type ) )
    {
        if( Market == 0 )///< Futures
        {
            if( Type == 0 )
                ReceiveFutConfirmMessage( pTree ); ///< Order confirmed
            else
                ReceiveFutExecuteMessage( pTree ); ///< Execution Report
        }
        else if( Market == 1 )///< Option
        {
            if( Type == 0 )
                ReceiveOptConfirmMessage( pTree ); ///< Order confirmed
			else
                ReceiveOptExecuteMessage( pTree ); ///< Execution Report
		}
	}
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::Connect( const char* RemoteIP, long RemotePort )
{
    FFUTExecIDCS.Enter();
    FFUTExecIDSet.Clear();
    FFUTExecIDCS.Leave();
    
    FOPTExecIDCS.Enter();
    FOPTExecIDSet.Clear();
	FOPTExecIDCS.Leave();
    
    FCurrentConnectionID = 0;
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation," FTransport->Connect( %s, %d )", RemoteIP, RemotePort );
	FTransport->Connect( RemoteIP, RemotePort );
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::Disconnect( void )
{
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " FTransport->Disconnect(  )");
    Logoff();
    FTransport->Disconnect();
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::Logon( int Version, const char* ID, const char* PASSWD, const char* ACCOUNT, ConnectionType Type )
{
	UFC::AnsiString LocalIpAddress = UFC::PSocket::GetLocalIP();
	UFC::AnsiString id,passwd;
	UFC::AnsiString Account( ACCOUNT );

	FVersion = Version;
	if( strlen( ID ) > 0 )
		id = UFC::AnsiString( ID );
	else
		id = "NULL";
	if( strlen( PASSWD ) > 0 )
		passwd = UFC::AnsiString( PASSWD );
	else
		passwd = "NULL";
	Account.PadThis( 7, '0' );
	FUniquekey.Printf( "%s@%s.%d", id.c_str(), UFC::Hostname, UFC::GetTickCountMS() );
	Logoff();
	FID = id;
	if( Type == ctSendOrder )
		FTriggerExec = FALSE;
	else
		FTriggerExec = TRUE;
	if( Type == ctRecvExecution )
		FCanSendOrder = FALSE;
	else
		FCanSendOrder = TRUE;
	FAdminListener = new TMdListener( FTransport, this, SUBJECT_ADMIN, FUniquekey.c_str());
	///< Create a thread to send the logon message.
	UFC::BufferedLog::Printf( " User:[%s] create logon thread.", FID.c_str() );
	new LogonThread( FTransport, &FLogonEvent, FVersion, SUBJECT_ADMIN, FUniquekey, id, passwd,Account );
}
//---------------------------------------------------------------------------
LogonThread::LogonThread( TMdTransport* Transport,
						  UFC::PEvent*  LogonEvent,
						  int Version,
                          const UFC::AnsiString& Subject,
                          const UFC::AnsiString& Key ,
                          const UFC::AnsiString& ID ,
                          const UFC::AnsiString& Passwd ,
                          const UFC::AnsiString& Account )
:UFC::PThread( NULL )
,FTransport( Transport )
,FLogonEvent( LogonEvent )
,FVersion( Version )
,FSubject( Subject )
,FKey( Key )
,FID( ID )
,FPasswd( Passwd )
,FAccount( Account )
{
	Start();
}
//---------------------------------------------------------------------------
void LogonThread::Execute( void )
{
	UFC::AnsiString LocalIpAddress = UFC::PSocket::GetLocalIP();
	TMdMessage      LogonMsg;

	while( FTransport->IsConnected() == FALSE )
		UFC::SleepMS( 30 ); ///< Wait for connected to server.
	UFC::SleepMS( 330 );
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " User:[%s] send logon message.", FID.c_str() );
	FLogonEvent->ResetEvent();
	LogonMsg.SetIntegerValue( "CMD", CMD_LOGON_REQUEST );
	LogonMsg.SetIntegerValue( "Ver", FVersion );
	LogonMsg.SetStringValue( "ID", FID.c_str() );
	LogonMsg.SetStringValue( "PASSWD", FPasswd.c_str() );
	LogonMsg.SetStringValue( "ACCOUNT", FAccount.c_str() );
	LogonMsg.SetStringValue( "IP", LocalIpAddress.c_str() );
	FTransport->Send( FSubject, FKey, &LogonMsg );
    if( FLogonEvent->WaitFor( 10 ) == FALSE )
    {
		UFC::BufferedLog::DebugPrintf( UFC::dlInformation," Wait for logon reply timeout.(10sec)");
        FTransport->Disconnect();
    }
    else
        FLogonEvent->ResetEvent();
}
//---------------------------------------------------------------------------
LogoffThread::LogoffThread( TTaifexTMPConnection* Connection )
:UFC::PThread( NULL ),
 FConnection( Connection )
{
    Start();
}
//---------------------------------------------------------------------------
void LogoffThread::Execute( void )
{
    UFC::SleepMS( 500 );
    FConnection->Disconnect();
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::ReceiveAdminMessage( MTree* pTree )
{
	TMdMessage Msg;
	Msg.Attach( pTree );
	int CMD = Msg.GetIntegerValue( "CMD", 0 );

	if( CMD == CMD_LOGON_REPLY )
	{
		int             CIDBits;
		LogonResult     Result = lrFailed;
		UFC::AnsiString ReplyString( "" );

		FLogonEvent.SetEvent();
		Msg.GetStringValue( "MSG", ReplyString );
		CIDBits  = Msg.GetIntegerValue( "BITS", 9 );
		FCurrentConnectionID  = Msg.GetIntegerValue( "CID", 0 );
        if( FCurrentConnectionID > 0 ) 
        {
			FAdmin   = Msg.GetIntegerValue( "ADMIN", 0 );
			Result   = lrOk;
			FIsLogon = true;
			UFC::AnsiString Key = FID + UFC::AnsiString( FCurrentConnectionID );

			FRecoverListener = new TMdListener( FTransport, this, SUBJECT_RECOVER, Key.c_str() );
			if( FTriggerExec == TRUE )
			{
				if( FAdmin == 0 )
					CreateReportListener( FID.c_str() );
				else
					CreateReportListener( "all" );
				UFC::BufferedLog::Printf( " %s logon,CID:[%d] Admin:[%s] Key:[%s]", FID.c_str(), FCurrentConnectionID, ( FAdmin == 0 )? "false" : "true", Key.c_str() );
			}
			try
			{
				UFC::AnsiString ShmPreFix;
				ShmPreFix.Printf( "%s.%s", FAppName.c_str(), FID.c_str());
				TNetworkID::Init( ShmPreFix, FCurrentConnectionID, CIDBits, true );
				CreateFormatFiles();
				CreateSymbolFile( pTree );
				TMP::TMPMessage::InitTMPMessageLibrary( TMP::tlmBoth );
			}
			catch( UFC::Exception& ex )
			{
				UFC::BufferedLog::Printf( " Exception[%s]", ex.what() );
				ReplyString.Printf("User %s already logon.", FID.c_str() );
				Result   = lrAlreadyLogon;
				FIsLogon = false;
				FCurrentConnectionID = 0;
			}
		}
		if( FListener != NULL )
			FListener->OnLogonReply( ReplyString, Result, FCurrentConnectionID );
		if( FCurrentConnectionID <= 0 )
			new LogoffThread( this );
	}
	else if( CMD == CMD_RECOVER_FINISHED )
	{
		int COUNT = Msg.GetIntegerValue( "COUNT", 0 );
        if( FListener != NULL )
            FListener->OnRecoverFinished( COUNT );
    }
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::CreateReportListener( const char* ListenKey )
{
	FFutConfirmListener = new TMdListener( FTransport, this, SUBJECT_CONFIRM_FUT, ListenKey );
	FOptConfirmListener = new TMdListener( FTransport, this, SUBJECT_CONFIRM_OPT, ListenKey );
	FFutExecuteListener = new TMdListener( FTransport, this, SUBJECT_FILL_FUT, ListenKey );
	FOptExecuteListener = new TMdListener( FTransport, this, SUBJECT_FILL_OPT, ListenKey );
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::RemoveListener( TMdListener*& Listener )
{
    if( Listener != NULL ) 
    {
        delete Listener;
        Listener = NULL;
    }
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::Logoff( void )
{
    FIsLogon = false;
    FID      = "";
    FAdmin   = 0;
    TNetworkID::Uninit();
	///< Listeners for Administration.
	RemoveListener( FAdminListener );
	///< Listener for Recover.
	RemoveListener( FRecoverListener );
	if( FTriggerExec == TRUE )
	{
		///< Listeners for Order confirm.
		RemoveListener( FFutConfirmListener );
		RemoveListener( FOptConfirmListener );
		///< Listeners for Order  execution.
		RemoveListener( FFutExecuteListener );
		RemoveListener( FOptExecuteListener );
	}
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::Recover( const char* BeginTime, RecoverDataType Type, RecoverMarket Market  )
{
    if( FIsLogon == true ) 
    {
        TMdMessage Msg;

		Msg.SetIntegerValue( "CMD", CMD_RECOVER_REQUEST );
		Msg.SetStringValue( "BEGINTIME", BeginTime );
		Msg.SetIntegerValue( "TYPE", (Int32)Type );
		Msg.SetIntegerValue( "MARKET", (Int32)Market );
		Msg.SetIntegerValue( "CID", FCurrentConnectionID );
		Msg.SetStringValue( "ID", FID.c_str() );
		Msg.SetIntegerValue( "ADMIN", FAdmin );
		FTransport->Send( SUBJECT_ADMIN, FUniquekey.c_str(), &Msg );
	}
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::Recover( const char* BeginTime, const char* EndTime, RecoverDataType Type, RecoverMarket Market  )
{
    if( FIsLogon == true )
    {
        TMdMessage Msg;

		Msg.SetIntegerValue( "CMD", CMD_RECOVER_REQUEST );
		Msg.SetStringValue( "BEGINTIME", BeginTime );
		Msg.SetStringValue( "ENDTIME", EndTime );
		Msg.SetIntegerValue( "TYPE", (Int32)Type );
		Msg.SetIntegerValue( "MARKET", (Int32)Market );
		Msg.SetIntegerValue( "CID", FCurrentConnectionID );
		Msg.SetStringValue( "ID", FID.c_str() );
		Msg.SetIntegerValue( "ADMIN", FAdmin );
		FTransport->Send( SUBJECT_ADMIN, FUniquekey.c_str(), &Msg );
    }
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::RequestFile( nsOrderMessageDefine::MarketEnum Market, BOOL IsMorningSession, Int32 ReqID, const char* Body )
{
    TMdMessage Msg;

    if( IsMorningSession == TRUE )
        Msg.SetIntegerValue( "TradeKind", 0 );
    else
		Msg.SetIntegerValue( "TradeKind", 1 );
    Msg.SetIntegerValue( "ReqID", ReqID );
    Msg.SetStringValue( "Body", Body );
	if( Market  == nsOrderMessageDefine::mTWFutures )
		FTransport->Send( SUBJECT_FT_FUT, UFC::Hostname, &Msg );
	else if( Market  == nsOrderMessageDefine::mTWOptions )
		FTransport->Send( SUBJECT_FT_OPT, UFC::Hostname, &Msg );
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::Reject( nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo,
								   const UFC::AnsiString& Text,
								   const UFC::AnsiString& ErrMsg,
								   TExecutionReportMessage* Msg )
{
	UFC::AnsiString ErrorStr;
	int             x25Code = Text.ToInt();

	Msg->SetOrderStatus( nsOrderMessageDefine::osRejected );
	Msg->SetExecType( nsOrderMessageDefine::etRejected );
	Msg->SetCxlRejResponseTo( CxlRejResponseTo );
	if( x25Code > 99 ) ///< It's TMP Error?
	{
		x25Code = 97;
		Msg->SetStatusCode( "97" );
	}
	else
		Msg->SetStatusCode(Text.c_str());
	if( FTAIFEXError->GetValue("TAIFEX", Text, ErrorStr) == TRUE )
	{
		if( x25Code == 97 )
		{
			UFC::AnsiString ErrorMessage;
			ErrorMessage.Printf("%s(%s)",ErrorStr.c_str(),ErrMsg.c_str());
			ErrorStr = ErrorMessage;
		}
	}
	else
		ErrorStr.Printf("Exchange error code:%s", Text.c_str());
	Msg->SetText( ErrorStr );
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::TrigerOnExecutionReport( TExecutionReportMessage* ExecutionReport, ExecDup PosDup )
{
	UFC::PLockObject Lock( FExecCS );

	if( FListener != NULL )
		FListener->OnExecutionReport( ExecutionReport, PosDup );
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::Reject( nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo,
								const UFC::AnsiString& ErrMsg,
								TBaseMessage* Msg )
{
	TExecutionReportMessage ExecutionReport;
	UFC::AnsiString SendTime;

	GetTimeString( SendTime , false );
	SendTime.PadThis( 8,'0' );
	ExecutionReport.SetAccount( Msg->GetAccount() );
	ExecutionReport.SetBrokerID( Msg->GetBrokerID() );
	ExecutionReport.SetSymbol( Msg->GetSymbol() );
	ExecutionReport.SetMarket( Msg->GetMarket() );
	ExecutionReport.SetNID( Msg->GetNID() );
	ExecutionReport.SetOrderID( Msg->GetOrderID() );
	ExecutionReport.SetPositionEffect( Msg->GetPositionEffect() );
	ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
	ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
	ExecutionReport.SetCxlRejResponseTo( CxlRejResponseTo );
	ExecutionReport.SetTransactTime( SendTime.c_str());
	ExecutionReport.SetText( ErrMsg.c_str() );
	TrigerOnExecutionReport( &ExecutionReport, edSpeedyGenerate );
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::SetCMID( const char* CMID )
{
	FCMID = CMID;
	FCMID.PadThis( 4, '0' );
	FCMIDIndex = TMP::TMPMessage::GetCMID( FCMID );
}
//------------------------------------------------------------------------------
void TTaifexTMPConnection::SetBrokerID( const char* FCMID)
{
	FTAIFEXBrokerID = FCMID;
	FTAIFEXBrokerID.PadThis( 7, '0' );
	FBrokerIndex = TMP::TMPMessage::GetBrokerID( FTAIFEXBrokerID );
}
//------------------------------------------------------------------------------
bool TTaifexTMPConnection::IsExecutionDup( nsOrderMessageDefine::MarketEnum  Market, const UFC::AnsiString& ExecID )
{
	if( Market == nsOrderMessageDefine::mTWFutures )
	{
		UFC::PLockObject Lock( FFUTExecIDCS );
		if( FFUTExecIDSet.Exists( ExecID ) == FALSE )
		{
			FFUTExecIDSet.Add( ExecID ); ///< Add to hash set.
			return false;
		}
	}
	else if( Market == nsOrderMessageDefine::mTWOptions )
	{
		UFC::PLockObject Lock( FOPTExecIDCS );
		if( FOPTExecIDSet.Exists( ExecID ) == FALSE )
		{
			FOPTExecIDSet.Add( ExecID ); ///< Add to hash set.
			return false;
		}
	}
	return true;
}
//---------------------------------------------------------------------------

