//------------------------------------------------------------------------------------------------------------------------
#include "RCRoutingNodeRemote.h"
//------------------------------------------------------------------------------------------------------------------------
// class RCRoutingNodeRemote
//------------------------------------------------------------------------------------------------------------------------
RCRoutingNodeRemote::RCRoutingNodeRemote( enRoutingType RoutingType, RCSpeedyCheckResultListener* SpeedyResultListener, RCReportReplyListener* ReplyListener, const UFC::AnsiString& RoutingIP, int RCSourceTag )
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
        // Set RCSource format:xxx.xxx.xxx.xxx-xx 
	FRCSourceStr.Printf( "%s-%02d", UFC::PSocket::GetLocalIP().c_str(), FRCSourceTag );
	// Create PClientSocket & set listener
        FClientSocket = new UFC::PClientSocket( FRoutingIP, RCGatewayPortNumber, TRUE );
	FClientSocket->SetListener( this );
	// Connect and Logon RCGateway
        ConnectAndLogonRCGateway();
	// Invoke thread start to run Execute()
	Start();
}
//------------------------------------------------------------------------------------------------------------------------
RCRoutingNodeRemote::RCRoutingNodeRemote( enRoutingType RoutingType, RCCheckResultListener* ResultListener, RCReportReplyListener* ReplyListener, const UFC::AnsiString& RoutingIP, int RCSourceTag )
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
	// Set RCSource format:xxx.xxx.xxx.xxx-xx 
	FRCSourceStr.Printf( "%s-%02d", UFC::PSocket::GetLocalIP().c_str(), FRCSourceTag );
	// Create PClientSocket & set listener
	FClientSocket = new UFC::PClientSocket( FRoutingIP, RCGatewayPortNumber, TRUE );
	FClientSocket->SetListener( this );
	// Connect and Logon RCGateway
	ConnectAndLogonRCGateway();
	// Invoke thread start to run Execute()
	Start();
}
//------------------------------------------------------------------------------------------------------------------------
RCRoutingNodeRemote::~RCRoutingNodeRemote()
{
	delete FClientSocket;
	Terminate(); ///< Call again, prevent be deleted directly
	WaitFor(); ///< Wait for thread terminated.
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCRoutingNodeRemote::ConnectAndLogonRCGateway( void )
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
                        UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Error! connect socket failed! <Reason:%s> (%s:%d)"
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
                        UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Error! Logon socket failed! <Reason:%s> (%s:%d)"
                                                    , e.what()
                                                    , FClientSocket->GetPeerIPAddress().c_str()
                                                    , FClientSocket->GetPort() );
                        FLogConnectFailedStatus = 2;
                }                 
		FClientSocket->Disconnect( ); ///< Logon failed, Disconnect socket
		return FALSE; ///< When Connect failed, no log to prevent log file too much
	}

	UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Logon command send complete. (%s:%d)", FClientSocket->GetPeerIPAddress().c_str(), FClientSocket->GetPort() );
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeRemote::ReSendRemainDataQueue( void )
{
	UFC::PLockObject Locker( FCS_RemainDataQueue ); ///< Enter Critical session.
	if ( FRemainDataQueue.size() <= 0 )
		return;

	// Log Begin -- Auto ReSend
	UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Begin -- Auto ReSend ReportData(%d) (%s:%d)"
                                    , FRemainDataQueue.size()
                                    , FClientSocket->GetPeerIPAddress().c_str()
                                    , FClientSocket->GetPort() );

	Int32 Count = 0;
	while ( FRemainDataQueue.size() > 0 )
	{
		Count++;
		UFC::AnsiString* Data = FRemainDataQueue.front();
		// Prepare Packet
		RCGPacket ReportReport( RCGP_ACT_DATA_REPORT, *Data );
		try
		{
			ReportReport.SerializeToSocket( FClientSocket ); ///< SerializeToSocket will Lock by IOLock of Socket
			FHeartbeatCountDown = RCGP_HEARTBEAT_PERIOD_CLIENT; ///< Reset Heartbeat CountDown
		}
		catch ( UFC::SocketException& e )
		{
			// Log Exception break
			UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Exception break -- Auto ReSend ReportData(%d) <Reason:%s> (%s:%d)"
                                                    , FRemainDataQueue.size()
                                                    , e.what()
                                                    , FClientSocket->GetPeerIPAddress().c_str()
                                                    , FClientSocket->GetPort() );

			FClientSocket->Disconnect();
			return;
		}
		delete Data;
		FRemainDataQueue.pop( );

		// Prevent this thread hold resource too long time.
		if ( Count%30 == 0 )
			UFC::PThread::PThread_Yield();
	}

	// Log End -- Auto ReSend
	UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] End -- Auto ReSend ReportData(%d) (%s:%d)"
                                    , FRemainDataQueue.size()
                                    , FClientSocket->GetPeerIPAddress().c_str()
                                    , FClientSocket->GetPort() );
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeRemote::PushToRemainDataQueue( UFC::AnsiString* DataPtr )
{
	UFC::PLockObject Locker( FCS_RemainDataQueue ); ///< Enter Critical session.

	// When queue size over Max Limit, don't keep any more ( just keep last RCRemainQueueMaxLimit records )
	if ( FRemainDataQueue.size() >= RCRemainQueueMaxLimit )
		FRemainDataQueue.pop( );
	FRemainDataQueue.push( DataPtr );
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCRoutingNodeRemote::DoRecvLogonConfirm( RCGPacket& Packet, UFC::PClientSocket* Socket )
{
	UFC::AnsiString* BodyStrPtr = Packet.GetBody();
	// Check Body length
	if ( BodyStrPtr->Length() < 3 )
	{
		UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Error! Receive <LogonConfirm Packet> Body Length:%d < 3 (%s:%d)"
                                        , BodyStrPtr->Length()
                                        , Socket->GetPeerIPAddress().c_str()
                                        , Socket->GetPort() );
		return FALSE;
	}

	// Check Server Logon Confirm return Code
	UFC::AnsiString Code, Msg;
	Code = BodyStrPtr->SubString( 0, 3 );
	Msg  = BodyStrPtr->SubString( 3, 0 );
	UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Receive <LogonConfirm Packet> <Code:%s> <Msg:%s> (%s:%d)"
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
BOOL RCRoutingNodeRemote::DoRecvCheckResult( RCGPacket& Packet, UFC::PClientSocket* Socket )
{
	UFC::AnsiString* BodyStrPtr = Packet.GetBody();
	// Check Body length
	if ( BodyStrPtr->Length() < 33 )
	{
		UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Error! Receive <Check Result> Body Length:%d < 33 (%s:%d)"
                                        , BodyStrPtr->Length()
                                        , Socket->GetPeerIPAddress().c_str()
                                        , Socket->GetPort() );
		return FALSE;
	}

	char MTypeStr[11];
	char* stopstring;
	strncpy_zero( MTypeStr, BodyStrPtr->c_str()+20, 10 ); ///< Get original mtype

	if ( FIsSpeedyUse )
	{
		RCSpeedyCheckResultParm SpeedyParm;
		SpeedyParm.IsOpt = ( (*BodyStrPtr)[30] == 'O' ) ? TRUE : FALSE;
		if ( SpeedyParm.IsOpt == TRUE )
			SpeedyParm.MarketType = rcmTWOptions;
		else
			SpeedyParm.MarketType = rcmTWFutures;			
		SpeedyParm.Code  = UFC::AnsiString( BodyStrPtr->c_str()+34, 3 );
		if ( BodyStrPtr->Length() == LEN_SpeedyCheckResult_R010 )
		{
			SpeedyParm.Order     =  UFC::AnsiString( BodyStrPtr->c_str()+37,  75 );
			SpeedyParm.Tick      =  UFC::AnsiString( BodyStrPtr->c_str()+238, 11 ).ToInt64( );
			SpeedyParm.IsOffHour = (UFC::AnsiString( BodyStrPtr->c_str()+37+75+84,  1 ) == "1" ) ;
			SpeedyParm.Msg       =  UFC::AnsiString( BodyStrPtr->c_str()+254, 81 );
			SpeedyParm.SpeedyNID =  UFC::AnsiString( BodyStrPtr->c_str()+335, 11 ).ToInt( );
            SpeedyParm.Source    =  UFC::AnsiString( BodyStrPtr->c_str()+346, 4 );
            UFC::AnsiString         SpeedyKeyLen( BodyStrPtr->c_str()+350,  3 );
			SpeedyParm.Key       =  UFC::AnsiString( BodyStrPtr->c_str()+353, SpeedyKeyLen.ToInt() );                        
		}
		else if ( BodyStrPtr->Length() == LEN_SpeedyCheckResult_R090 )
		{
			SpeedyParm.Order     = UFC::AnsiString( BodyStrPtr->c_str()+37,  86 );
			SpeedyParm.Tick      = UFC::AnsiString( BodyStrPtr->c_str()+249, 11 ).ToInt64( );
            SpeedyParm.IsOffHour =(UFC::AnsiString( BodyStrPtr->c_str()+37+86+84,  1 ) == "1" ) ;
			SpeedyParm.Msg       = UFC::AnsiString( BodyStrPtr->c_str()+265, 81 );
			SpeedyParm.SpeedyNID = UFC::AnsiString( BodyStrPtr->c_str()+346, 11 ).ToInt( );
                        SpeedyParm.Source    = UFC::AnsiString( BodyStrPtr->c_str()+357, 4 );
                        UFC::AnsiString        SpeedyKeyLen( BodyStrPtr->c_str()+361,  3 );
			SpeedyParm.Key       = UFC::AnsiString( BodyStrPtr->c_str()+364, SpeedyKeyLen.ToInt() ) ;
		}
		SpeedyParm.Msg.TrimRight( );
		SpeedyParm.Key.TrimRight( );
		FSpeedyResultListener->OnCheckResultArrive( SpeedyParm );
	}
	else
	{
		// Initial queue data
		UFC::UQueueData Data( strtol( MTypeStr, &stopstring, 16 ), (char*)BodyStrPtr->c_str()+32, BodyStrPtr->Length()-32 );
		// Hook RCCheckResultListener::OnCheckResultArrive()
		BOOL IsOpt = ( (*BodyStrPtr)[30] == 'O' ) ? TRUE : FALSE;
		FResultListener->OnCheckResultArrive( IsOpt, &Data, BodyStrPtr->Length()-32 );
	}

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCRoutingNodeRemote::DoRecvReportReply( RCGPacket& Packet, UFC::PClientSocket* Socket )
{
	UFC::AnsiString* BodyStrPtr = Packet.GetBody();

	// Check Body length
	if ( BodyStrPtr->Length() < LEN_ReportReply )
	{
		UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Error! Receive <Report Reply> Body Length:%d < 79 (%s:%d)"
								  , BodyStrPtr->Length()
								  , Socket->GetPeerIPAddress().c_str()
								  , Socket->GetPort() );
		return FALSE;
	}

	char MTypeStr[11];
	char* stopstring;
	strncpy_zero( MTypeStr, BodyStrPtr->c_str()+20, 10 ); ///< Get original mtype
        // Initial queue data
        UFC::UQueueData Data( strtol( MTypeStr, &stopstring, 16 ), (char*)BodyStrPtr->c_str()+31, BodyStrPtr->Length()-31 );
        // Hook RCReportReplyListener::OnReportReplyArrive()
        BOOL IsOpt = ( (*BodyStrPtr)[20] == 'O' ) ? TRUE : FALSE;
        FReplyListener->OnReportReplyArrive( IsOpt, &Data, BodyStrPtr->Length()-31 );
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeRemote::DoSendHeartbeat( UFC::PClientSocket* Socket )
{
	RCGPacket Packet( RCGP_ACT_HEARTBEAT );

	try
	{
		Packet.SerializeToSocket( Socket );
	}
	catch ( UFC::SocketException& e )
	{
		UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Error! Send Heartbeat socket failed! <Reason:%s> (%s:%d)"
                                            , e.what()
                                            , Socket->GetPeerIPAddress().c_str()
                                            , Socket->GetPort() );
		FClientSocket->Disconnect();
	}
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeRemote::DoSendHeartbeatConfirm( UFC::PClientSocket* Socket )
{
	RCGPacket Packet( RCGP_ACT_HEARTBEAT_CONFIRM );

	try
	{
		Packet.SerializeToSocket( Socket );
	}
	catch ( UFC::SocketException& e )
	{
		UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Error! Send HeartbeatConfirm socket failed! <Reason:%s> (%s:%d)"
                                            , e.what()
                                            , Socket->GetPeerIPAddress().c_str()
                                            , Socket->GetPort() );
		FClientSocket->Disconnect();
	}
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeRemote::OnConnect( UFC::PClientSocket * Socket )
{
	UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Connected (%s:%d)", Socket->GetPeerIPAddress().c_str(), Socket->GetPort() );
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeRemote::OnDisconnect( UFC::PClientSocket * Socket, BOOL NeedReconnect )
{
	FIsLogon = FALSE;
        FLogConnectFailedStatus = 0; ///< Reset log once
	UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Connection broken. (%s:%d)", Socket->GetPeerIPAddress().c_str(), Socket->GetPort() );
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCRoutingNodeRemote::OnDataArrived( UFC::PClientSocket * Socket )
{
	BOOL RtnVal = TRUE;
	RCGPacket Packet;
	try
	{
		Packet.LoadFromSocket( Socket );
	}
	catch ( UFC::SocketException& e )
	{
		UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Error! OnDataArrived throw socket exception! <Reason:%s> (%s:%d)"
                                            , e.what()
                                            , Socket->GetPeerIPAddress().c_str()
                                            , Socket->GetPort() );
		return FALSE;
	}
	switch ( Packet.GetAction() )
	{
	// Check Result
	case RCGP_ACT_DATA_CHECK_RESULT:
		FHeartbeatCountDown = RCGP_HEARTBEAT_PERIOD_CLIENT; ///< Reset Heartbeat CountDown
		RtnVal = DoRecvCheckResult( Packet, Socket );
		break;
        // Report Reply
	case RCGP_ACT_DATA_REPORT_REPLY:
		FHeartbeatCountDown = RCGP_HEARTBEAT_PERIOD_CLIENT; ///< Reset Heartbeat CountDown
		RtnVal = DoRecvReportReply( Packet, Socket );
		break;                
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
void RCRoutingNodeRemote::OnIdle( UFC::PClientSocket * Socket )
{
	FHeartbeatCountDown--;
	if ( FHeartbeatCountDown == 0 )
	{
		DoSendHeartbeat( Socket );
		FHeartbeatCountDown = -1;
	}
	else if ( FHeartbeatCountDown < -RCGP_HEARTBEAT_TIMEOUT_CLIENT )
	{
		UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Error! Wait Server side Heartbeat Confirm timeout! (%s:%d)"
                                            , Socket->GetPeerIPAddress().c_str()
                                            , Socket->GetPort() );
		Socket->Disconnect();
	}
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeRemote::Execute( void )
{
	while ( IsTerminated() == FALSE ) ///< Check PThread::IsTerminated() to Stop Thread
	{
		// Check connection if disconnect then connect and resend data in memory queue if it have data
		if ( FClientSocket->IsConnect() == FALSE )
		{
			if ( ConnectAndLogonRCGateway() == TRUE )
			{
				// Speedy no SendOrderReport, so, no need to Resend report
				if ( FIsSpeedyUse == FALSE )
					ReSendRemainDataQueue( );
			}
		}
		UFC::SleepMS( 1000 );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingNodeRemote::StopInternalThread( void )
{
	Terminate(); ///< Terminate the resend data polling thread.
}
//------------------------------------------------------------------------------------------------------------------------
enRtnCheckRequest RCRoutingNodeRemote::SendCheckRequest( BOOL IsOpt, int CheckLevel /* 1 ~ 9 */,
							 UFC::UQueueStruct *QueueData /* A3 format */, int Len )
{
	if ( FClientSocket->IsConnect() == FALSE )
		return crErr_NotConnected;
	// Prepare Packet
	RCGPacket CheckPacket( RCGP_ACT_DATA_CHECK );
	CheckPacket.GetBody()->Printf( "%-20.20s0x%08x%c%c%s"
                                    , FRCSourceStr.c_str()
                                    , QueueData->mtype
                                    , ( IsOpt ? 'O' : 'F' )
                                    , (CheckLevel+48) ///< ASCII '1' = 49
                                    , QueueData->mtext );
	try
	{
		// Send socket
		CheckPacket.SerializeToSocket( FClientSocket ); ///< SerializeToSocket will Lock by IOLock of Socket
		FHeartbeatCountDown = RCGP_HEARTBEAT_PERIOD_CLIENT; ///< Reset Heartbeat CountDown
	}
	catch ( UFC::SocketException& e )
	{
		FClientSocket->Disconnect();
		UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Error! Send Check socket failed! <Reason:%s> (%s:%d)"
                                            , e.what()
                                            , FClientSocket->GetPeerIPAddress().c_str()
                                            , FClientSocket->GetPort() );
		return crErr_SendSocketError;
	}
	return (enRtnCheckRequest)CheckLevel;
}
//------------------------------------------------------------------------------------------------------------------------
enRtnOrderReport RCRoutingNodeRemote::SendOrderReport( BOOL IsOpt, int CheckLevel, ///< 1 ~ 9
                                                       UFC::UQueueStruct *QueueData /* A5 format */, int Len )
{
	// Prepare Packet
	RCGPacket ReportPacket( RCGP_ACT_DATA_REPORT );
	ReportPacket.GetBody()->Printf( "%-20.20s0x%08x%c%c%s"
                                        , FRCSourceStr.c_str()
                                        , QueueData->mtype
                                        , ( IsOpt ? 'O' : 'F' )
                                        , (CheckLevel+48) ///< ASCII '1' = 49
                                        , QueueData->mtext );
	if ( FClientSocket->IsConnect() == FALSE )
	{
		// Only for socket OrderReport to push RemainDataQueue
		UFC::AnsiString* StrPtr = ReportPacket.GetBody();
		PushToRemainDataQueue( new UFC::AnsiString( StrPtr->c_str(), StrPtr->Length() ) );

		return orErr_NotConnected;
	}

	try
	{
		// Send socket
		ReportPacket.SerializeToSocket( FClientSocket ); ///< SerializeToSocket will Lock by IOLock of Socket
		FHeartbeatCountDown = RCGP_HEARTBEAT_PERIOD_CLIENT; ///< Reset Heartbeat CountDown
	}
	catch ( UFC::SocketException& e )
	{
		FClientSocket->Disconnect();
		UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Error! Send Report socket failed! <Reason:%s> (%s:%d)"
                                            , e.what()
                                            , FClientSocket->GetPeerIPAddress().c_str()
                                            , FClientSocket->GetPort() );

		// Only for socket OrderReport to push RemainDataQueue
		UFC::AnsiString* StrPtr = ReportPacket.GetBody();
		PushToRemainDataQueue( new UFC::AnsiString( StrPtr->c_str(), StrPtr->Length() ) );

		return orErr_SendSocketError;
	}
	return orSend_InternalOK;
}
//------------------------------------------------------------------------------------------------------------------------
enRtnCheckRequest RCRoutingNodeRemote::SpeedySendCheckRequest( int CheckLevel /* 1 ~ 9 */,
                                                               RCSpeedyCheckParm& CheckParm )
{
	if ( FClientSocket->IsConnect() == FALSE )
		return crErr_NotConnected;

	// Prepare Packet
	RCGPacket       CheckPacket( RCGP_ACT_DATA_CHECK );
	CheckPacket.GetBody()->Printf( "%-20.20s0x78888866%c%c%s%-94.94s%-32.32s%011lld%c%011d%-4.4s%03d%-200.200s"
                                        , FRCSourceStr.c_str()
                                        , ( CheckParm.IsOpt ? 'O' : 'F' )
                                        , (CheckLevel+48) ///< ASCII '1' = 49
                                        , CheckParm.Order.c_str()
                                        , (CheckParm.GDD.IsEmpty()) ? "" : CheckParm.GDD.c_str()	///< GDD
                                        , (CheckParm.UDD.IsEmpty()) ? "" : CheckParm.UDD.c_str()	///< UDD
                                        , CheckParm.Tick                ///< TICK
                                        , 'N'                           ///< YN_FO
                                        , CheckParm.SpeedyNID           ///< SPEEDY_NID
                                        , CheckParm.Source.c_str()      ///< SPEEDY_SOURCE
                                        , CheckParm.Key.Length()        ///< SPEEDY_KEY length        
                                        , CheckParm.Key.c_str() );      ///< SPEEDY_KEY
	try
	{
            UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " RCRoutingNodeRemote\t[%s](%d)", CheckPacket.GetBody()->c_str(), CheckPacket.GetBody()->Length() );
            // Send socket
            CheckPacket.SerializeToSocket( FClientSocket ); ///< SerializeToSocket will Lock by IOLock of Socket
            FHeartbeatCountDown = RCGP_HEARTBEAT_PERIOD_CLIENT; ///< Reset Heartbeat CountDown
	}
	catch ( UFC::SocketException& e )
	{
            FClientSocket->Disconnect();
            UFC::BufferedLog::Printf( " [RCRoutingNodeRemote] Error! Send Check socket failed! <Reason:%s> (%s:%d)"
                                        , e.what()
                                        , FClientSocket->GetPeerIPAddress().c_str()
                                        , FClientSocket->GetPort() );
            return crErr_SendSocketError;
	}

	return (enRtnCheckRequest)CheckLevel;
}
//------------------------------------------------------------------------------------------------------------------------
