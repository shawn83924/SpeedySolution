//------------------------------------------------------------------------------------------------------------------------
#include "RCRoutingNodeLocal.h"
//------------------------------------------------------------------------------------------------------------------------
// Class ReportReplyReceiver
//------------------------------------------------------------------------------------------------------------------------
RCRoutingReplyReceiver::RCRoutingReplyReceiver( RCReportReplyListener* ReplyListener )
: FReplyListener( ReplyListener )
{
}
//------------------------------------------------------------------------------------------------------------------------
RCRoutingReplyReceiver::~RCRoutingReplyReceiver( )
{
        if ( FQueueSubject != NULL )
                delete FQueueSubject;
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingReplyReceiver::Run( void )
{
	FQueueSubject = new UQueueSubject( 0x78DE0000, RCMTypeFromLocal, 0666 );
	FQueueSubject->SetQueueListener( this );
	FQueueSubject->Run();
	// Print start up message in main log
	 UFC::BufferedLog::Printf( " [RCRoutingReplyReceiver] thread is start up." );
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingReplyReceiver::OnQueueDataArrived( UQueueStruct *QueueData, int Len )
{
	// Check Body length
        if ( Len < LEN_ReportReply ) ///< garbage data
        {
		 UFC::BufferedLog::Printf( " [RCRoutingReplyReceiver] Receive garbage data <%d|%s(%d)>"
							, QueueData->mtype
							, QueueData->mtext
							, Len );
		return;
        }
        
        // Initial queue data
        UFC::UQueueData Data( QueueData->mtype, QueueData->mtext+31, Len-31 );
        // Hook RCReportReplyListener::OnReportReplyArrive()
        BOOL IsOpt = ( QueueData->mtext[20] == 'O' ) ? TRUE : FALSE;
        FReplyListener->OnReportReplyArrive( IsOpt, &Data, Len - 31 );
}
//------------------------------------------------------------------------------------------------------------------------
// class RCRoutingNodeLocal
//------------------------------------------------------------------------------------------------------------------------
RCRoutingNodeLocal::RCRoutingNodeLocal( enRoutingType RoutingType, RCSpeedyCheckResultListener* SpeedyResultListener, RCReportReplyListener* ReplyListener, const UFC::AnsiString& RoutingIP, int RCSourceTag )
: UFC::PThread( NULL )
, FRoutingType( RoutingType )
, FIsSpeedyUse( TRUE )
, FResultListener( NULL )
, FReplyListener( ReplyListener )
, FSpeedyResultListener( SpeedyResultListener )
, FRoutingIP( RoutingIP )
, FRCSourceTag( RCSourceTag )
, FIsLogon( FALSE )
, FLogConnectFailedStatus( 0 )
{
	// 1. Create Detect connection
	// Set RCSource format:xxx.xxx.xxx.xxx-xx 
	FRCSourceStr.Printf( "%s-%02d", UFC::PSocket::GetLocalIP().c_str(), FRCSourceTag );
	// Create PClientSocket & set listener
	FClientSocket = new UFC::PClientSocket( FRoutingIP, RCGatewayPortNumber, TRUE );
	FClientSocket->SetListener( this );
	// Connect and Logon RCGateway
	ConnectAndLogonRCGateway();
	// Invoke thread start to run Execute()
	Start();

	// 2. Create Queue listener
	if ( FRoutingType == rtCheck )
	{
                FQueueSubject = new UFC::UQueueSubject( 0x78DA0000, RCMTypeFromLocal, 0666 );
                FQueueSubject->SetQueueListener( this );
                FQueueSubject->Run( );
                // Open RCCheck queue
                if ( !FQueueRCCheck.Open( 0x78D90000, true ) )
                {
                        UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Open RCCheck queue(0x78D90000) failed.\n" );
                        exit(-1);
                }
                // Open RCCheckResult queue
                if ( !FQueueRCCheckResult.Open( 0x78DA0000, true ) )
                {
                        UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Open RCCheckResult queue(0x78DA0000) failed.\n" );
                        exit(-1);
                }
        }
	else if ( FRoutingType == rtReport )
	{
                // Create a listener thread
                FReportReply = new RCRoutingReplyReceiver( FReplyListener );
                FReportReply->Run();
		// Open RCReport queue
		if ( !FQueueRCReport.Open( 0x78DC0000, true ) )
		{
			UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Open RCReport queue(0x78DC0000) failed.\n" );
			exit(-1);
		}                
                // Open RCReportReply queue
                if ( !FQueueRCReportReply.Open( 0x78DE0000, true ) )
                {
                        UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Open RCReportReply queue(0x78DE0000) failed.\n" );
                        exit(-1);
                }
        }
}
//------------------------------------------------------------------------------------------------------------------------
RCRoutingNodeLocal::RCRoutingNodeLocal( enRoutingType RoutingType, RCCheckResultListener* ResultListener, RCReportReplyListener* ReplyListener, const UFC::AnsiString& RoutingIP, int RCSourceTag )
: UFC::PThread( NULL )
, FRoutingType( RoutingType )
, FIsSpeedyUse( FALSE )
, FResultListener( ResultListener )
, FReplyListener( ReplyListener )
, FSpeedyResultListener( NULL )
, FRoutingIP( RoutingIP )
, FRCSourceTag( RCSourceTag )
, FIsLogon( FALSE )
, FLogConnectFailedStatus( 0 )
{
	// 1. Create Detect connection
	// Set RCSource format:xxx.xxx.xxx.xxx-xx 
	FRCSourceStr.Printf( "%s-%02d", UFC::PSocket::GetLocalIP().c_str(), FRCSourceTag );
	// Create PClientSocket & set listener
	FClientSocket = new UFC::PClientSocket( FRoutingIP, RCGatewayPortNumber, TRUE );
	FClientSocket->SetListener( this );
	// Connect and Logon RCGateway
	ConnectAndLogonRCGateway();
	// Invoke thread start to run Execute()
	Start();

	// 2. Create Queue listener
	if ( FRoutingType == rtCheck )
	{
		// When Type is rtCheck --> Create a UQueueSubject & set listener
		FQueueSubject = new UFC::UQueueSubject( 0x78DA0000, RCMTypeFromLocal, 0666 );
		FQueueSubject->SetQueueListener( this );
		FQueueSubject->Run( );
		// Open RCCheck queue
		if ( !FQueueRCCheck.Open( 0x78D90000, true ) )
		{
			UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Open RCCheck queue(0x78D90000) failed.\n" );
			exit(-1);
		}
		// Open RCCheckResult queue
		if ( !FQueueRCCheckResult.Open( 0x78DA0000, true ) )
		{
			UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Open RCCheckResult queue(0x78DA0000) failed.\n" );
			exit(-1);
		}
	}
	else if ( FRoutingType == rtReport )
	{
                // Create a listener thread
                FReportReply = new RCRoutingReplyReceiver( FReplyListener );
                FReportReply->Run();
		// Open RCReport queue
		if ( !FQueueRCReport.Open( 0x78DC0000, true ) )
		{
			UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Open RCReport queue(0x78DC0000) failed.\n" );
			exit(-1);
		}
                // Open RCCheckReply queue
                if ( !FQueueRCReportReply.Open( 0x78DE0000, true ) )
                {
                        UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Open RCReportReply queue(0x78DE0000) failed.\n" );
                        exit(-1);
                }
	}
}
//------------------------------------------------------------------------------------------------------------------------
RCRoutingNodeLocal::~RCRoutingNodeLocal()
{
	if ( FRoutingType == rtCheck )
		delete FQueueSubject;
        if ( FRoutingType == rtReport )
		delete FReportReply;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCRoutingNodeLocal::ConnectAndLogonRCGateway( void )
{
	try
	{
		FClientSocket->Connect( 2 ); ///< Connect
                FClientSocket->SetWriteTimeout( 2000 );
	}
	catch( UFC::SocketException& e )
	{
               if ( FLogConnectFailedStatus < 1 ) ///< log once
                {
                        UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Error! connect socket failed! <Reason:%s> (%s:%d)"
                                                    , e.what()
                                                    , FClientSocket->GetPeerIPAddress().c_str()
                                                    , FClientSocket->GetPort() );
                        FLogConnectFailedStatus = 1;
                }                   
		return FALSE; ///< When Connect failed, no log to prevent log file too much
	}

	try
	{
		// Set TCPNoDelay & KeepAlive option
		FClientSocket->SetTCPNoDelay( TRUE );
                FClientSocket->SetBufferSize( SO_RCVBUF, 1048576 );
                FClientSocket->SetBufferSize( SO_SNDBUF, 1048576 ); 

                //KeepAlive
                UFC::Int32 optval = 1;
                socklen_t optlen = sizeof(optval);
                optval = 1; ///< enable keepalive
                FClientSocket->SetOption(SOL_SOCKET, SO_KEEPALIVE, &optval, optlen); ///< must use SOL_SOCKET        
                optval = 10;///< after FKeepAlive sec while idle
                FClientSocket->SetOption( 6, TCP_KEEPIDLE, &optval, optlen); ///< set 10 sec
                optval = 2;///< how many the keepalive probe ?
                FClientSocket->SetOption( 6, TCP_KEEPCNT, &optval, optlen);
                optval = 1;///< interval=1 sec
                FClientSocket->SetOption( 6, TCP_KEEPINTVL, &optval, optlen);
                
		// Logon
		RCGPacket LogonPacket( RCGP_ACT_LOGON );
		LogonPacket.GetBody()->Printf( "RC%-20.20s", FRCSourceStr.c_str() );
		LogonPacket.SerializeToSocket( FClientSocket ); ///< SerializeToSocket will Lock by IOLock of Socket
		FHeartbeatCountDown = RCGP_HEARTBEAT_PERIOD_CLIENT; ///< Reset Heartbeat CountDown
	}
	catch( UFC::SocketException& e )
	{
                if ( FLogConnectFailedStatus < 2 ) ///< log once
                {
                        UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Error! Logon socket failed! (<Reason:%s> (%s:%d)"
                                                    , e.what()
                                                    , FClientSocket->GetPeerIPAddress().c_str()
                                                    , FClientSocket->GetPort() );
                        FLogConnectFailedStatus = 2;
                }                
		FClientSocket->Disconnect( ); ///< Logon failed, Disconnect socket
		return FALSE; ///< When Connect failed, no log to prevent log file too much
	}

	UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Logon command send complete. (%s:%d)", FClientSocket->GetPeerIPAddress().c_str(), FClientSocket->GetPort() );
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCRoutingNodeLocal::DoRecvLogonConfirm( RCGPacket& Packet, UFC::PClientSocket* Socket )
{
	UFC::AnsiString* BodyStrPtr = Packet.GetBody();
	// Check Body length
	if ( BodyStrPtr->Length() < 3 )
	{
		UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Error! Receive <LogonConfirm Packet> Body Length:%d < 3 (%s:%d)"
                                            , BodyStrPtr->Length()
                                            , Socket->GetPeerIPAddress().c_str()
                                            , Socket->GetPort() );
		return FALSE;
	}

	// Check Server Logon Confirm return Code
	UFC::AnsiString Code, Msg;
	Code = BodyStrPtr->SubString( 0, 3 );
	Msg  = BodyStrPtr->SubString( 3, 0 );
	UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Receive <LogonConfirm Packet> <Code:%s> <Msg:%s> (%s:%d)"
                                , Code.c_str()
                                , Msg.c_str()
                                , Socket->GetPeerIPAddress().c_str()
                                , Socket->GetPort() );
	if ( Code != "000" )
		return FALSE;

	FIsLogon = TRUE;
        // Query Threshold from RC
        if ( gThresholdManager != NULL && FRoutingType == rtReport )
        {
                gThresholdManager->QueryRCThreshodByIP( Socket->GetPeerIPAddress().c_str() );
        }
        
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeLocal::DoSendHeartbeat( UFC::PClientSocket* Socket )
{
	RCGPacket Packet( RCGP_ACT_HEARTBEAT );

	try
	{
		Packet.SerializeToSocket( Socket );
	}
	catch ( UFC::SocketException& e )
	{
		UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Error! Send Heartbeat socket failed! <Reason:%s> (%s:%d)"
                                        , e.what()
                                        , Socket->GetPeerIPAddress().c_str()
                                        , Socket->GetPort() );
		FClientSocket->Disconnect();
	}
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeLocal::DoSendHeartbeatConfirm( UFC::PClientSocket* Socket )
{
	RCGPacket Packet( RCGP_ACT_HEARTBEAT_CONFIRM );

	try
	{
		Packet.SerializeToSocket( Socket );
	}
	catch ( UFC::SocketException& e )
	{
		UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Error! Send HeartbeatConfirm socket failed! <Reason:%s> (%s:%d)"
                                        , e.what()
                                        , Socket->GetPeerIPAddress().c_str()
                                        , Socket->GetPort() );
		FClientSocket->Disconnect();
	}
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeLocal::DoRejectDataRemainInQueue( void )
{
	if ( FRoutingType != rtCheck )
		return;

	UFC::UQueueData Data( RCMTypeFromLocal, IPCSMQ_DEFAULTSIZE );
	UFC::AnsiString ResultStr;

	try
	{
		while ( FQueueRCCheck.RecvNoWait( Data ) == TRUE )
		{
			// 1. Check Length and Set ResultStr
			int Len = Data.GetSize();
			if ( Len == LEN_CheckData_R010 ) ///< Order Check
			{
				ResultStr.Printf( "%-32.32s%3.3s%-2.2s%-213.213s    %-81.81s"
                                                , Data.GetData()
                                                , "05" ///< Status
                                                , "999" ///< ErrCode
                                                , (char*)Data.GetData()+32
                                                , "RCServer System down." );
			}
			else if ( Len == LEN_CheckData_R090 ) ///< Quote Order
			{
				ResultStr.Printf( "%-32.32s%03.3s%-2.2s%-224.224s    %-81.81s"
                                                , Data.GetData()
                                                , "05" ///< Status
                                                , "999" ///< ErrCode
                                                , (char*)Data.GetData()+32
                                                , "RCServer System down." );
			}
			else if ( Len == LEN_SpeedyCheck_R010 ) ///< SpeedyGateway Order Check
			{
				ResultStr.Printf( "%-32.32s%3.3s%-2.2s%-213.213s    %-81.81s%-218.218s"
                                                , Data.GetData()
                                                , "05" ///< Status
                                                , "999" ///< ErrCode
                                                , (char*)Data.GetData()+32
                                                , "RCServer System down."
                                                , (char*)Data.GetData()+297 ); ///< SPEEDY_NID + SPEEDY_KEY
			}
			else if ( Len == LEN_SpeedyCheck_R090 ) ///< SpeedyGateway Quote Check
			{
				ResultStr.Printf( "%-32.32s%3.3s%-2.2s%-224.224s    %-81.81s%-218.218s"
                                                , Data.GetData()
                                                , "05" ///< Status
                                                , "999" ///< ErrCode
                                                , (char*)Data.GetData()+32
                                                , "RCServer System down."
                                                , (char*)Data.GetData()+308 ); ///< SPEEDY_NID + SPEEDY_KEY
			}

			// 2. Reply ResultStr to RCCheckResultQueue
			try
			{
				UFC::UQueueData ResultData( RCMTypeFromLocal, (char*)ResultStr.c_str(), ResultStr.Length() );
				FQueueRCCheckResult.Send( ResultData );
			}
			catch ( UFC::QueueException& e )
			{
				UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Error! Write to RCCheckResult queue(0x78DA0000) failed! <Reason:%s>", e.what() );
				return;
			}
		}
	}
	catch ( UFC::QueueException& e )
	{
		UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Error! Read from RCCheck queue(0x78D90000) failed when DoRejectDataRemainInQueue() <Reason:%s>", e.what() );
		return;
	}
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeLocal::OnConnect( UFC::PClientSocket * Socket )
{
        
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeLocal::OnDisconnect( UFC::PClientSocket * Socket, BOOL NeedReconnect )
{
	FIsLogon = FALSE;
        FLogConnectFailedStatus = 0; ///< Reset log once
	UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Connection broken. (%s:%d)", Socket->GetPeerIPAddress().c_str(), Socket->GetPort() );
	DoRejectDataRemainInQueue( );
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCRoutingNodeLocal::OnDataArrived( UFC::PClientSocket * Socket )
{
	BOOL RtnVal = TRUE;
	RCGPacket Packet;
	try
	{
		Packet.LoadFromSocket( Socket );
	}
	catch ( UFC::SocketException& e )
	{
		UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Error! OnDataArrived throw socket exception! <Reason:%s> (%s:%d)"
								  , e.what()
								  , Socket->GetPeerIPAddress().c_str()
								  , Socket->GetPort() );
		return FALSE;
	}
	switch ( Packet.GetAction() )
	{
	// Heartbeat
	case RCGP_ACT_HEARTBEAT:
		FHeartbeatCountDown = RCGP_HEARTBEAT_PERIOD_CLIENT; ///< Reset Heartbeat CountDown
		DoSendHeartbeatConfirm( Socket );
		break;
	// Heartbeat Confirm
	case RCGP_ACT_HEARTBEAT_CONFIRM:
		FHeartbeatCountDown = RCGP_HEARTBEAT_PERIOD_CLIENT; ///< Reset Heartbeat CountDown
		break;
	// Logon Confirm
	case RCGP_ACT_LOGON_CONFIRM:
		FHeartbeatCountDown = RCGP_HEARTBEAT_PERIOD_CLIENT; ///< Reset Heartbeat CountDown
		RtnVal = DoRecvLogonConfirm( Packet, Socket );
		break;
	default: ///< Unknow stream, ignore.
		return TRUE;
	}

	return RtnVal;
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeLocal::OnIdle( UFC::PClientSocket * Socket )
{
	FHeartbeatCountDown--;
	if ( FHeartbeatCountDown == 0 )
	{
		DoSendHeartbeat( Socket );
		FHeartbeatCountDown = -1;
	}
	else if ( FHeartbeatCountDown < -RCGP_HEARTBEAT_TIMEOUT_CLIENT )
	{
		UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Error! Wait Server side Heartbeat Confirm timeout! (%s:%d)"
								  , Socket->GetPeerIPAddress().c_str()
								  , Socket->GetPort() );
		Socket->Disconnect();
	}
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeLocal::OnQueueDataArrived( UFC::UQueueStruct *QueueData, int Len )
{
	char MTypeStr[11];
	char* stopstring;
	strncpy_zero( MTypeStr, QueueData->mtext+20, 10 ); ///< Get original mtype

	if ( FIsSpeedyUse )
	{
		RCSpeedyCheckResultParm SpeedyParm;
		SpeedyParm.IsOpt = ( QueueData->mtext[30] == 'O' ) ? TRUE : FALSE;
		if ( SpeedyParm.IsOpt == TRUE )
			SpeedyParm.MarketType = rcmTWOptions;
		else
			SpeedyParm.MarketType = rcmTWFutures;			
		SpeedyParm.Code = UFC::AnsiString( QueueData->mtext+34, 3 );
		if ( Len == LEN_SpeedyCheckResult_R010 )
		{
			SpeedyParm.Order     = UFC::AnsiString( QueueData->mtext+37,  75 );
			SpeedyParm.Tick      = UFC::AnsiString( QueueData->mtext+238, 11 ).ToInt64();
            SpeedyParm.IsOffHour =(UFC::AnsiString( QueueData->mtext+37+75+84,  1 ) == "1" ) ;
			SpeedyParm.Msg       = UFC::AnsiString( QueueData->mtext+254, 81 );
			SpeedyParm.SpeedyNID = UFC::AnsiString( QueueData->mtext+335, 11 ).ToInt( );
                        SpeedyParm.Source    = UFC::AnsiString( QueueData->mtext+346, 4 );
                        UFC::AnsiString        SpeedyKeyLen( QueueData->mtext+350,    3 );
			SpeedyParm.Key       = UFC::AnsiString( QueueData->mtext+353, SpeedyKeyLen.ToInt() );                        
		}
		else if ( Len == LEN_SpeedyCheckResult_R090 )
		{
			SpeedyParm.Order     = UFC::AnsiString( QueueData->mtext+37,  86 );
			SpeedyParm.Tick      = UFC::AnsiString( QueueData->mtext+249, 11 ).ToInt64( );
            SpeedyParm.IsOffHour =(UFC::AnsiString( QueueData->mtext+37+86+84,  1 ) == "1" ) ;
			SpeedyParm.Msg       = UFC::AnsiString( QueueData->mtext+265, 81 );
			SpeedyParm.SpeedyNID = UFC::AnsiString( QueueData->mtext+346, 11 ).ToInt( );
                        SpeedyParm.Source    = UFC::AnsiString( QueueData->mtext+357, 4 );
                        UFC::AnsiString        SpeedyKeyLen( QueueData->mtext+361,    3 );
			SpeedyParm.Key       = UFC::AnsiString( QueueData->mtext+364, SpeedyKeyLen.ToInt() ) ;
		}
		SpeedyParm.Msg.TrimRight( );
		SpeedyParm.Key.TrimRight( );
		FSpeedyResultListener->OnCheckResultArrive( SpeedyParm );
	}
	else
	{
		// Initial queue data
		UFC::UQueueData Data( strtol( MTypeStr, &stopstring, 16 ), QueueData->mtext+32, Len-32 );
		// Hook RCCheckResultListener::OnCheckResultArrive()
		BOOL IsOpt = ( QueueData->mtext[30] == 'O' ) ? TRUE : FALSE;
		FResultListener->OnCheckResultArrive( IsOpt, &Data, Len-32 );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeLocal::Execute( void )
{
	while ( IsTerminated() == FALSE ) ///< Check PThread::IsTerminated() to Stop Thread
	{
		// Check connection if disconnect then connect
		if ( FClientSocket->IsConnect() == FALSE )
			ConnectAndLogonRCGateway();
		UFC::SleepMS( 1000 );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeLocal::StopInternalThread( void )
{
	Terminate(); ///< Terminate the reconnect data polling thread.
}
//------------------------------------------------------------------------------------------------------------------------
enRtnCheckRequest RCRoutingNodeLocal::SendCheckRequest( BOOL IsOpt, int CheckLevel /* 1 ~ 9 */,
							UFC::UQueueStruct *QueueData /* A3 format */, int Len )
{
	if ( FClientSocket->IsConnect() == FALSE )
		return crErr_NotConnected;

	UFC::AnsiString CheckStr;
	CheckStr.Printf( "%-20.20s0x%08x%c%c%s"
                        , "local"
                        , QueueData->mtype
                        , ( IsOpt ? 'O' : 'F' )
                        , (CheckLevel+48) ///< ASCII '1' = 49
                        , QueueData->mtext );
	try
	{
		UFC::UQueueData Data( RCMTypeFromLocal, (char*)CheckStr.c_str(), CheckStr.Length() );
		FQueueRCCheck.Send( Data );
	}
	catch ( UFC::QueueException& e )
	{
		UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Error! Write to RCCheck queue(0x78D90000) failed! <Reason:%s>", e.what() );
		return crErr_SendQueueError;
	}

	return (enRtnCheckRequest)CheckLevel;
}
//------------------------------------------------------------------------------------------------------------------------
enRtnOrderReport RCRoutingNodeLocal::SendOrderReport( BOOL IsOpt, int CheckLevel /* 1 ~ 9 */,
                                                      UFC::UQueueStruct *QueueData /* A5 format */, int Len )
{
	if ( FClientSocket->IsConnect() == FALSE )
		return orErr_NotConnected;

	UFC::AnsiString ReportStr;
	ReportStr.Printf( "%-20.20s0x%08x%c%c%s"
                        , "local"
                        , QueueData->mtype
                        , ( IsOpt ? 'O' : 'F' )
                        , (CheckLevel+48) ///< ASCII '1' = 49
                        , QueueData->mtext );
	try
	{
		UFC::UQueueData Data( RCMTypeFromLocal, (char*)ReportStr.c_str(), ReportStr.Length() );
		FQueueRCReport.Send( Data );
	}
	catch ( UFC::QueueException& e )
	{
		UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Error! Write to RCReport queue(0x78DC0000) failed! <Reason:%s>", e.what() );
		return orErr_SendQueueError;
	}

	return orSend_InternalOK;
}
//------------------------------------------------------------------------------------------------------------------------
enRtnCheckRequest RCRoutingNodeLocal::SpeedySendCheckRequest( int CheckLevel /* 1 ~ 9 */,
                                                              RCSpeedyCheckParm& CheckParm )
{
	if ( FClientSocket->IsConnect() == FALSE )
		return crErr_NotConnected;

	UFC::AnsiString CheckStr;
	CheckStr.Printf( "%-20.20s0x78888866%c%c%s%-94.94s%-32.32s%011lld%c%011d%-4.4s%03d%-200.200s"
                        , "local"
                        , ( CheckParm.IsOpt ? 'O' : 'F' )
                        , (CheckLevel+48)              ///< ASCII '1' = 49
                        , CheckParm.Order.c_str()
                        , (CheckParm.GDD.IsEmpty()) ? "" : CheckParm.GDD.c_str()	///< GDD
                        , (CheckParm.UDD.IsEmpty()) ? "" : CheckParm.UDD.c_str()	///< UDD
                        , CheckParm.Tick		///< TICK
                        , 'N'				///< YN_FO
                        , CheckParm.SpeedyNID		///< SPEEDY_NID
                        , CheckParm.Source.c_str()     ///< SPEEDY_SOURCE
                        , CheckParm.Key.Length()       ///< SPEEDY_KEY length        
                        , CheckParm.Key.c_str() );     ///< SPEEDY_KEY
        
	try
	{
            UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " RCRoutingNodeLocal\t[%s](%d)", CheckStr.c_str(), CheckStr.Length() );
            
            UFC::UQueueData Data( RCMTypeFromLocal, (char*)CheckStr.c_str(), CheckStr.Length() );
            FQueueRCCheck.Send( Data );
	}
	catch ( UFC::QueueException& e )
	{
            UFC::BufferedLog::Printf( " [RCRoutingNodeLocal] Error! Write to RCCheck queue(0x78D90000) failed! <Reason:%s>", e.what() );
            return crErr_SendQueueError;
	}

	return (enRtnCheckRequest)CheckLevel;
}
//------------------------------------------------------------------------------------------------------------------------
