#include "Stream.h"
//---------------------------------------------------------------------------
// PClientSocket
//---------------------------------------------------------------------------
#include "PClientSocket.h"
#include "BufferedLog.h"
#include "NetUtility.h"
#ifndef _WIN32
    #include <poll.h>
#endif
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
//
//	PClientSocket
//
//---------------------------------------------------------------------------
PClientSocket::PClientSocket(  )
:PSocket( SOCK_STREAM, BLOCKING_SOCKET )
,PThread( NULL, FALSE )
,FIPAddress( "" )
,FPort( 0 )
,FIsConnected( FALSE )
,FListener( NULL )
,FData( 0 )
{
    UpdateLocalIPAddress();
    Start( );          ///<  Start the polling thread.
}
//---------------------------------------------------------------------------
PClientSocket::PClientSocket( const AnsiString & Address, Int32 Port, BOOL ThreadMode )
:PSocket( SOCK_STREAM, BLOCKING_SOCKET )
,PThread( NULL, FALSE )
,FIPAddress( Address )
,FPort( Port )
,FIsConnected( FALSE )
,FListener( NULL )
,FData( 0 )
{
    UpdateLocalIPAddress();
    if( ThreadMode == TRUE )
	Start( );
}
//---------------------------------------------------------------------------
PClientSocket::PClientSocket( Int32 FD )
:PSocket( FD, SOCK_STREAM, BLOCKING_SOCKET )
,PThread( NULL, FALSE )
,FIPAddress( "" )
,FPort( 0 )
,FIsConnected( TRUE )
,FListener( NULL )
,FData( 0 )
{
    UpdateLocalIPAddress();
    UpdatePeerIPAddress();
}
//---------------------------------------------------------------------------
void PClientSocket::UpdateLocalIPAddress()
{
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    char* ipaddress;

    memset( &sin, 0, sizeof( sin ) );
    if( getsockname( FFD, (struct sockaddr*)&sin, &len) != -1 )
    {
        ipaddress = inet_ntoa( sin.sin_addr );
        if( ipaddress != NULL && strlen(ipaddress) <= 15 ) ///< xxx.xxx.xxx.xxx length =15
            FSocketIPAddress = ipaddress;
        else
            throw( SocketException( "error ip address string " ) );
    }
    else
        FSocketIPAddress = "127.0.0.1";
}
//---------------------------------------------------------------------------
void PClientSocket::UpdatePeerIPAddress()
{
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    char*     ipaddress;

    memset( &sin, 0, sizeof( sin ) );
    if( getpeername( FFD, (struct sockaddr*)&sin, &len) != -1 )
    {
        FPort = ntohs( sin.sin_port );
        ipaddress = inet_ntoa( sin.sin_addr );
        if( ipaddress != NULL && strlen(ipaddress) <= 15 ) ///< xxx.xxx.xxx.xxx length =15
            FIPAddress  =  ipaddress;
        else
            throw( SocketException( "error ip address string " ) );
    }
    else
        FIPAddress = "127.0.0.1";
}
//---------------------------------------------------------------------------
PClientSocket::~PClientSocket( void )
{
    try
    {
        Disconnect();    ///< Disconnect the socket connection.
    }
    catch( SocketException & )
    {
    }
    if( IsTerminated() == FALSE ) ///< If it's a Non-blocking socket.
    {
        Terminate(); ///< Terminate the socket polling thread.
        WaitFor();   ///< Wait for thread terminated.
    }
}
//------------------------------------------------------------------------------
void PClientSocket::Connect( const AnsiString& BindAddress, Int32 BindPort, const AnsiString& TargetAddress, Int32 TargetPort, const Int32 TimoOutSec )
{
    FPort      = TargetPort;
    FIPAddress = TargetAddress;

    if( FPort <= 0 || FIPAddress.Length() == 0 )
        throw( SocketException( "Invalidate port number or IP address." ) );
    
    if( FIsConnected == FALSE )
    {
        if( OpenSocket() > 0 )
        {
            try
            {
                PSocket::SetReuse( TRUE );
                PSocket::SetLinger( TRUE, 0 );
                PSocket::Bind( BindAddress, BindPort );
				PSocket::Connect( FIPAddress, FPort, TimoOutSec );
				UpdateLocalIPAddress();
                UpdatePeerIPAddress();
				FIsConnected = TRUE;
                SetBufferSize( SO_RCVBUF, 8192 );
				SetBufferSize( SO_SNDBUF, 8192 );
				if( FListener != NULL )
					FListener->OnConnect( this );
			}
            catch( SocketException &e )
            {
                FIsConnected = FALSE;
                CloseSocket();
                throw e;
            }
        }
    }
}
//------------------------------------------------------------------------------
void PClientSocket::Connect(  const AnsiString & Address, Int32 Port, const Int32 Sec )
{
    FPort      = Port;
    FIPAddress = Address;
    Connect( Sec );
}
//------------------------------------------------------------------------------
void PClientSocket::Connect( const Int32 TimoOutSec )
{
    if( FPort <= 0 || FIPAddress.Length() == 0 )
        throw( SocketException( "Invalidate port number or IP address." ) );
    if( FIsConnected == FALSE )
    {
        if( OpenSocket() > 0 )
        {
            try
            {
                UFC::BufferedLog::DebugPrintf( " Connect to[%s:%d] timeout[%d]",FIPAddress.c_str(), FPort, TimoOutSec   );
				PSocket::Connect( FIPAddress, FPort, TimoOutSec );
				UpdateLocalIPAddress();
				UpdatePeerIPAddress( );
				FIsConnected = TRUE;
				SetBufferSize( SO_RCVBUF, 8192 );
				SetBufferSize( SO_SNDBUF, 8192 );
                if( FListener != NULL )
					FListener->OnConnect( this );
            }
            catch( SocketException &e )
            {
                FIsConnected = FALSE;
                CloseSocket();
                throw e;
            }
        }
    }
}
//---------------------------------------------------------------------------
//void PClientSocket::Disconnect( BOOL TriggerEvent )
void PClientSocket::Disconnect( BOOL TriggerEvent, BOOL MeedReconnect ) // modify by joe
{
    try
    {
		if( FIsConnected == TRUE )
		{
			FIsConnected = FALSE;
			if( TriggerEvent == TRUE && FListener != NULL )
				//FListener->OnDisconnect( this );
                FListener->OnDisconnect( this, MeedReconnect );
			CloseSocket();
        }
    }
	catch( SocketException &e )
    {
        BufferedLog::Printf(" Disconnect failed:%s\n",e.what());
        throw( e );
    }
}
//---------------------------------------------------------------------------
BOOL PClientSocket::CheckDataArrived( struct timeval& SelectTime )
{
    if( GetBlockMode() == NONBLOCKING_SOCKET )
    {
    #ifndef _WIN32
        Int32         PollCount;
        struct pollfd PollFD;
        int           WaitTime = SelectTime.tv_sec*1000 + SelectTime.tv_usec/1000;

        PollFD.fd     = FFD;
        PollFD.events = POLLRDNORM;
        if( (PollCount = poll( &PollFD,1, WaitTime )) == 0 ) ///< Time is up or interrupt by signal.
        {
             return FALSE; ///< time is up.
        }
        else if( PollCount > 0 )
        {
             return TRUE; ///< Ready to read.Data arrived!
        }
        else
        {
             if( errno == EINTR )
             {
                 UFC::BufferedLog::DebugPrintf("### poll Interrupt by signal (read) ###");
                 return FALSE;
             }
             else if( errno == EAGAIN )
             {
                      return FALSE;
             }
             else
             {
                 if( FIsConnected == FALSE )
                     return FALSE;
                throw( SocketException( " call poll failed.(Read)" ) );
             }
        }
    #else
        Int32  Count;

        FD_ZERO( &FReadSet );       ///< Empty the Read set.
        FD_SET( (u_int)FFD, &FReadSet );
        if( (Count = select( FFD + 1, &FReadSet, NULL, NULL, &SelectTime)) < 0 )
        {
            if( FIsConnected == FALSE )
                return FALSE;
            throw( SocketException( "ClientSocket select failed.\n" ) );
        }
        else if( Count == 0 )
			return FALSE;  ///< No data. Select time out.
		else
			return TRUE;   ///< Data arrived in non-blocking mode.
    #endif
    }
    else ///< Blocking mode socket.
    {
		char OneByte;

		Int32 RecvSize = recv( FFD, (char*)&OneByte, 1, MSG_PEEK );
		if( RecvSize < 0 )
		{
			if( FIsConnected == FALSE )
				return FALSE;
		#ifdef _WIN32
			Int32 ErrNo = WSAGetLastError();

			if( ErrNo == WSAEWOULDBLOCK )
			{
				BufferedLog::DebugPrintf("######## R:Nonblocking #########");
				return FALSE;
			}
			else
			{
				AnsiString Reason;
				Reason.Printf( "RecvBuffer failed. Error code:%d", ErrNo );
				throw( SocketException( Reason.c_str() ) );
			}
		#else
			if( errno == EINTR )
			{
				BufferedLog::DebugPrintf("####### Interrupt #########");
				return FALSE;
			}
			else if( errno == EPIPE )
				throw( SocketException( "RecvBuffer from a broken Pipe." ) );
			else
				throw( SocketException( errno ) );//"RecvBuffer failed." ) );
		#endif
		}
		else if( RecvSize == 0 )
		{
			BufferedLog::DebugPrintf(" The connection has been gracefully closed.");
			return TRUE;
		}
		// Data arrived in blocking mode.
		return TRUE;
    }
}
//---------------------------------------------------------------------------
void PClientSocket::Execute( void )
{
	Int32 Count = 0;

	while ( IsTerminated() == FALSE )
	{
		if ( FIsConnected == FALSE  )
		{
			UFC::SleepMS( 100 );
			continue;
		}
		try
		{
			struct timeval SelectTimeout = {1,0};///< Select timeout 1 sec

			if( CheckDataArrived( SelectTimeout ) == TRUE )
			{
                Count++;
                if( FListener != NULL )
                {
                    if( FListener->OnDataArrived( this ) == FALSE )
                        Disconnect();
                }
                else
                    this->Purge();
                if( Count%50 == 0 )
                    UFC::PThread::PThread_Yield( );
            }
            else
            {
                Count = 0;
                if( FListener != NULL )
                    FListener->OnIdle( this );
            }
        }
        catch( SocketException & e )
        {
            BufferedLog::Printf(" Socket exception in Excute function. what(): [%s]", e.what() );
			//Disconnect();
            Disconnect( TRUE, TRUE ); // modify by joe
		}
	};
}
//---------------------------------------------------------------------------
const Int32 PClientSocket::GetPeerID( void )
{
    return (Int32)inet_addr( FIPAddress.c_str() );
}
//---------------------------------------------------------------------------
const Int32 PClientSocket::GetSocketID( void )
{
    return (Int32)inet_addr( FSocketIPAddress.c_str() );
}
//---------------------------------------------------------------------------
void PClientSocket::SendQueue( const Int8* SendData, Int32 Size )
{
    UFC::PLockObject    LockObj( IOLock );

    FWriteQueue.push_back( std::string( SendData, Size ) );        ///< Add a MigoHeader object to queue.
}
//---------------------------------------------------------------------------
void PClientSocket::SendQueue( const std::string& SendData )
{
    UFC::PLockObject    LockObj( IOLock );

    FWriteQueue.push_back( SendData );        ///< Add a MigoHeader object to queue.
}
//---------------------------------------------------------------------------
inline void PClientSocket::PickFront( std::string& Data )
{
    UFC::PLockObject    LockObj( IOLock );

    if( FWriteQueue.empty() == FALSE )
    {
        Data = FWriteQueue.front();
    }
    else
    {
        Data = "" ;
    }
    return ;
}
//---------------------------------------------------------------------------
inline void PClientSocket::PopFront( void )
{
    UFC::PLockObject    LockObj( IOLock );

    if( FWriteQueue.empty() == FALSE )
        FWriteQueue.pop_front();
}    
//---------------------------------------------------------------------------
BOOL  PClientSocket::IsQueueEmpty( void )
{
    return FWriteQueue.empty();
}
//---------------------------------------------------------------------------
Int32 PClientSocket::ProcessQueue( void)
{    
    Int32 TotalSize = 0;
    Int32 WriteSize = 0;
    std::string WriteData;

    while( FWriteQueue.empty() == FALSE ) ///< Queue is empty, all data sent out! 
    {
        PickFront( WriteData );  ///< Get a message from write queue
        WriteSize = (UFCType::Int32)WriteData.size();

        if( WriteSize > 0 )
        {
            BlockSend( (char*)WriteData.c_str(), WriteSize );
            PopFront(); ///< pop it from queue
            TotalSize += WriteSize;
        }
        //PThread_Yield();
    }
    return TotalSize;            
}
//---------------------------------------------------------------------------
void PClientSocket::ClearQueue( void )
{
    UFC::PLockObject Lock( IOLock );

    UFC::BufferedLog::Printf( " Empty send queue size:%d FD:%d", FWriteQueue.size(), GetHandle() );
    FWriteQueue.clear();
}
//---------------------------------------------------------------------------
const Int32 PClientSocket::GetSocketPort( void )
{
    struct sockaddr_in name;
    socklen_t namelen = sizeof( sockaddr_in );

    if( getsockname(FFD, (struct sockaddr*)&name, &namelen )== 0 )
	return ntohs( name.sin_port);
    return -1;
}
//---------------------------------------------------------------------------
const Int32 PClientSocket::GetPeerPort( void )
{
    struct sockaddr_in name;
    socklen_t namelen = sizeof( sockaddr_in );

    if( getpeername(FFD, (struct sockaddr*)&name, &namelen )== 0 )
	return ntohs( name.sin_port );
    return -1;
}
//---------------------------------------------------------------------------
const BOOL  PClientSocket::IsConnect( void )
{
    return FIsConnected;
}
//---------------------------------------------------------------------------
void PClientSocket::UpdateIPAddress( const UFC::AnsiString& LocalIP,const UFC::AnsiString& PeerIP )
{
    FIPAddress = LocalIP;
    FSocketIPAddress = PeerIP;
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
