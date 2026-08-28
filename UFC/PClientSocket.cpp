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
,FBusyCheck(FALSE)
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
,FBusyCheck(FALSE)
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
,FBusyCheck( FALSE )
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
    char ipaddress[INET_ADDRSTRLEN];

    memset( &sin, 0, sizeof( sin ) );
    if( getsockname( FFD, (struct sockaddr*)&sin, &len) != -1 )
    {
        if( inet_ntop( AF_INET, &sin.sin_addr, ipaddress, sizeof(ipaddress) ) != NULL )
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
    char      ipaddress[INET_ADDRSTRLEN];

    memset( &sin, 0, sizeof( sin ) );
    if( getpeername( FFD, (struct sockaddr*)&sin, &len) != -1 )
    {
        FPort = ntohs( sin.sin_port );
        if( inet_ntop( AF_INET, &sin.sin_addr, ipaddress, sizeof(ipaddress) ) != NULL )
            FIPAddress = ipaddress;
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
    catch( ... )
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
                SetBufferSize( SO_RCVBUF, 65536 );
				SetBufferSize( SO_SNDBUF, 65536 );
				if( FListener != NULL )
					FListener->OnConnect( this );
			}
            catch( SocketException &e )
            {
                FIsConnected = FALSE;
                CloseSocket();
                throw;
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
                PSocket::SetReuse( TRUE );
                PSocket::SetLinger( TRUE, 0 );
                UFC::BufferedLog::DebugPrintf( " Connect to[%s:%d] timeout[%d]",FIPAddress.c_str(), FPort, TimoOutSec   );
				PSocket::Connect( FIPAddress, FPort, TimoOutSec );
				UpdateLocalIPAddress();
				UpdatePeerIPAddress( );
				FIsConnected = TRUE;
				SetBufferSize( SO_RCVBUF, 65536 );
				SetBufferSize( SO_SNDBUF, 65536 );
                if( FListener != NULL )
					FListener->OnConnect( this );
            }
            catch( SocketException &e )
            {
                FIsConnected = FALSE;
                CloseSocket();
                throw;
            }
        }
    }
}
//---------------------------------------------------------------------------
void PClientSocket::Disconnect( BOOL TriggerEvent, BOOL NeedReconnect )
{
    try
    {
		if( FIsConnected == TRUE )
		{
			FIsConnected = FALSE;
			if( TriggerEvent == TRUE && FListener != NULL )
                FListener->OnDisconnect( this, NeedReconnect );
			CloseSocket();
        }
    }
	catch( SocketException &e )
    {
        BufferedLog::Printf(" Disconnect failed:%s\n",e.what());
        throw;
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

        fd_set ReadSet;
        FD_ZERO( &ReadSet );
        FD_SET( (u_int)FFD, &ReadSet );
        if( (Count = select( FFD + 1, &ReadSet, NULL, NULL, &SelectTime)) < 0 )
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
BOOL PClientSocket::CheckDataArrivedBusy( void )
{
#ifdef __LINUX
    char OneByte;

    Int32 RecvSize = recv( FFD, (char*)&OneByte, 1, MSG_PEEK|MSG_DONTWAIT );
    if( RecvSize < 0 )
    {            
        if( errno == EINTR )
        {
            BufferedLog::Printf("####### Interrupt #########");
            return FALSE;
        }
        else if( errno == EAGAIN )
        {
            return FALSE;
        }
        else if( errno == EPIPE )
            throw( SocketException( "RecvBuffer from a broken Pipe." ) );
        else
            throw( SocketException( errno ) );
    }
    else if( RecvSize == 0 )
    {
        BufferedLog::DebugPrintf(" [BusyCheckData] The connection has been gracefully closed.");
        return TRUE;
    }
    return TRUE;
#else
    struct timeval SelectTimeout = {1,0};///< Select timeout 1 sec

    return CheckDataArrived( SelectTimeout );
#endif    
}
//---------------------------------------------------------------------------
void PClientSocket::Process( int& Count )
{
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
        Disconnect( TRUE, TRUE );
    }
}
//---------------------------------------------------------------------------
void PClientSocket::ProcessBusy( int& Count )
{
    try
    {
        if( CheckDataArrivedBusy( ) == TRUE )
        {
            Count++;
            if( FListener != NULL )
            {
                if( FListener->OnDataArrived( this ) == FALSE )
                    Disconnect();
            }
            else
                this->Purge();
        }
        else
        {
            Count = 0;
        }
    }
    catch( SocketException & e )
    {
        BufferedLog::Printf(" Socket exception in Excute function. what(): [%s]", e.what() );
        Disconnect( TRUE, TRUE );
    }
}
//---------------------------------------------------------------------------
void PClientSocket::Execute( void )
{
    Int32 Count = 0;

    while ( IsTerminated() == FALSE )
    {
        if( FIsConnected == TRUE  )
        {
            if( FBusyCheck == FALSE )
                Process( Count );
            else
                ProcessBusy( Count );
        }
        else
            UFC::SleepMS( 100 );
    }
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
bool PClientSocket::TryPopFront( std::string& Data )
{
    UFC::PLockObject LockObj( IOLock );

    if( FWriteQueue.empty() )
        return false;
    Data = FWriteQueue.front();
    FWriteQueue.pop_front();
    return true;
}
//---------------------------------------------------------------------------
BOOL  PClientSocket::IsQueueEmpty( void )
{
    UFC::PLockObject LockObj( IOLock );
    return FWriteQueue.empty();
}
//---------------------------------------------------------------------------
Int32 PClientSocket::ProcessQueue( void)
{
    Int32 TotalSize = 0;
    std::string WriteData;

    while( TryPopFront( WriteData ) )
    {
        Int32 WriteSize = (UFCType::Int32)WriteData.size();
        if( WriteSize > 0 )
        {
            BlockSend( (char*)WriteData.c_str(), WriteSize );
            TotalSize += WriteSize;
        }
    }
    return TotalSize;
}
//---------------------------------------------------------------------------
// Batch version of ProcessQueue: take the whole queue in one lock, then
// coalesce messages up to BATCH_SEND_LIMIT bytes per BlockSend call.
// Byte order on the wire is identical to ProcessQueue; only the number of
// lock acquisitions and send syscalls differs. Messages enqueued while
// sending are picked up by the caller's next round (same contract).
//---------------------------------------------------------------------------
static const size_t BATCH_SEND_LIMIT = 65536; ///< 64KB per BlockSend
//---------------------------------------------------------------------------
Int32 PClientSocket::ProcessQueueBatch( void )
{
    QUEUE       WorkQueue;
    std::string Buffer;
    Int32       TotalSize = 0;

    {
        UFC::PLockObject Lock( IOLock );
        WorkQueue.swap( FWriteQueue );
    }
    for( QUEUE::iterator It = WorkQueue.begin(); It != WorkQueue.end(); ++It )
    {
        if( It->empty() )
            continue;
        Buffer.append( *It );
        if( Buffer.size() >= BATCH_SEND_LIMIT )
        {
            BlockSend( (char*)Buffer.c_str(), (Int32)Buffer.size() );
            TotalSize += (Int32)Buffer.size();
            Buffer.clear();
        }
    }
    if( !Buffer.empty() )
    {
        BlockSend( (char*)Buffer.c_str(), (Int32)Buffer.size() );
        TotalSize += (Int32)Buffer.size();
    }
    return TotalSize;
}
//---------------------------------------------------------------------------
void PClientSocket::ClearQueue( void )
{
    UFC::PLockObject Lock( IOLock );

    UFC::BufferedLog::Printf( " Empty send queue size:%d FD:%d", (int)FWriteQueue.size(), GetHandle() );
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
    FSocketIPAddress = LocalIP;
    FIPAddress = PeerIP;
}
//---------------------------------------------------------------------------
void  PClientSocket::SetThreadAffinity( int CPUID )
{
    this->PThread_setaffinity( CPUID );
}        
//---------------------------------------------------------------------------
void PClientSocket::SetBusyloopCheck( bool IsBusy )
{
    FBusyCheck = IsBusy;
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
