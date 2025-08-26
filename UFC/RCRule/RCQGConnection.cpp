//------------------------------------------------------------------------------------------------------------------------
#include "RCQGConnection.h"
//------------------------------------------------------------------------------------------------------------------------
// class RCQGConnection
//------------------------------------------------------------------------------------------------------------------------
RCQGConnection::RCQGConnection( RCQGConnectionEventListener* Listener )
: FListener( Listener )
{
	FClientSocket = new UFC::PClientSocket( );
	FClientSocket->SetListener( this );

	ClearSyncData( ); ///< Clear Sync Flag
}
//------------------------------------------------------------------------------------------------------------------------
RCQGConnection::~RCQGConnection()
{
	FClientSocket->Disconnect( );
	delete FClientSocket;
}
//------------------------------------------------------------------------------------------------------------------------
void RCQGConnection::OnConnect( UFC::PClientSocket* Socket )
{
	// Do nothing
}
//------------------------------------------------------------------------------------------------------------------------
void RCQGConnection::OnDisconnect( UFC::PClientSocket* Socket, BOOL NeedReconnect )
{
	if ( FListener != NULL )
		FListener->OnDisconnect( this );
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCQGConnection::OnDataArrived( UFC::PClientSocket* Socket )
{
	RCQGPacket PacketData;

	// Receive RCQGPacket from socket
	try
	{
		PacketData.LoadFromSocket( Socket );
	}
	catch ( UFC::SocketException & )
	{
		AckErrSyncData( );
		return FALSE;
	}

	// Check Data type
	switch ( PacketData.GetAction() )
	{
	case RCQGP_QUERY_RESULT: ///< Query Result
		ProcQueryResult( PacketData.GetData() );
		break;
	case RCQGP_LOGOUT: ///< Logout
		AckErrSyncData( );
		Disconnect();
		break;
	}

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void RCQGConnection::OnIdle( UFC::PClientSocket* Socket )
{
	// Do nothing
}
//------------------------------------------------------------------------------------------------------------------------
void RCQGConnection::LockAndSend( RCQGPacket& PacketData )
{
	UFC::PLockObject Locker( FClientSocket->IOLock ); ///< Enter socket write Critical session.
	PacketData.SerializeToSocket( FClientSocket );    ///< Send data through FClientSocket
}
//------------------------------------------------------------------------------------------------------------------------
void RCQGConnection::ClearSyncData( void )
{
	UFC::PLockObject Locker( FSyncCS );
	FSyncMode   = FALSE;
	FSyncResult = TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void RCQGConnection::SetSyncData( void )
{
	UFC::PLockObject Locker( FSyncCS );
	FSyncMode   = TRUE;
	FSyncResult = TRUE;
	FSyncEvent.ResetEvent( );
}
//------------------------------------------------------------------------------------------------------------------------
void RCQGConnection::AckErrSyncData( void )
{
	UFC::PLockObject Locker( FSyncCS );
	if ( FSyncMode == TRUE )
	{
		FSyncMode   = FALSE;
		FSyncResult = FALSE;
		FSyncEvent.SetEvent( );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void RCQGConnection::ProcQueryResult( RCQGNameValueSet* Data )
{
	UFC::PLockObject Locker ( FSyncCS );
	if ( FSyncMode == TRUE ) ///< Sync Mode
	{
		FSyncNameValue = *Data;
		FSyncMode      = FALSE;
		FSyncResult    = TRUE;
		FSyncEvent.SetEvent( );
	}
	else ///< ASync Mode
        FListener->OnDataArrived( this, Data );
}
//------------------------------------------------------------------------------------------------------------------------
int RCQGConnection::Connect( char* IP, int Port, int TimeoutSec, RCQGNameValueSet& LogonConfirm )
{
	if ( TimeoutSec > 100 )
		TimeoutSec = 100;

	try
	{
		FClientSocket->Connect( IP, Port, TimeoutSec ); ///< Connect
                FClientSocket->SetWriteTimeout( 2000 );
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
	}
	catch ( UFC::Exception& e )
	{
		UFC::AnsiString Msg;
		Msg.Printf( "Connect to RCServer failed. Reason:%s", e.what() );
		LogonConfirm.SetValue( "YN", "N" );
		LogonConfirm.SetValue( "MSG", Msg.c_str() );
		return -1; ///< rcrFailed
	}

	ClearSyncData( ); ///< Clear Sync Flag

	return 0;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCQGConnection::Disconnect()
{
	if ( IsConnected() == TRUE )
		FClientSocket->Disconnect( );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCQGConnection::IsConnected()
{
	return FClientSocket->IsConnect( );
}
//------------------------------------------------------------------------------------------------------------------------
int RCQGConnection::SendNameValue( RCQGNameValueSet& Data )
{
	if ( FClientSocket->IsConnect() == FALSE ) ///< Not connnected yet.
		return -1; ///< srNotConnected

	try ///< Try to Send NameValue message
	{
		RCQGPacket PacketData( RCQGP_QUERY_COMMAND, Data );
		LockAndSend( PacketData );
	}
	catch ( UFC::SocketException& )
	{
		FClientSocket->Disconnect( );
		return -3; ///< rsrSocketError  ///< Write data to socket failed. Fatal Error! disconnect.
	}

	return 0;
}
//------------------------------------------------------------------------------------------------------------------------
int RCQGConnection::SyncSendNameValue( RCQGNameValueSet& Data, RCQGNameValueSet& ResultData, int TimeOutSec )
{
	if ( FClientSocket->IsConnect() == FALSE ) ///< Not connnected yet.
		return -1; ///< srNotConnected

	// Set Sync Data
	SetSyncData( );

	try ///< Try to Send NameValue message
	{
		RCQGPacket PacketData( RCQGP_QUERY_COMMAND, Data );
		LockAndSend( PacketData );
	}
	catch ( UFC::SocketException& )
	{
		ClearSyncData( ); ///< Clear Sync Flag
		FClientSocket->Disconnect( );
		return -3; ///< rsrSocketError  ///< Write data to socket failed. Fatal Error! disconnect.
	}

	// WaitFor FSyncEvent
	if ( FSyncEvent.WaitFor( TimeOutSec ) == FALSE )
	{
		ClearSyncData( ); ///< Clear Sync Flag
		FClientSocket->Disconnect( );
		return -4; ///< rsrQueryTimeout  ///< Wait for Server ack timeout!
	}

	// Check OnDataArrived thread if process OK
	UFC::PLockObject Locker( FSyncCS );
	if ( FSyncResult == FALSE )
	{
		ClearSyncData( ); ///< Clear Sync Flag
		FClientSocket->Disconnect( );
		return -5; ///< rsrErrorResponse  ///< Error Response!
	}

	ResultData = FSyncNameValue;
	return 0;
}
//------------------------------------------------------------------------------------------------------------------------
