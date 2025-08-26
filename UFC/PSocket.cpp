#include "PSocket.h"
#include "BufferedLog.h"
#include "NetUtility.h"
#include <stdio.h>
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------    
Int32 PSocket::GHeartBeatTime    = 40;   ///< Heartbeat interval 40 sec
Int32 PSocket::GHeartBeatTimeout = 5;    ///< Heartbeat timeout 5 sec
Int32 PSocket::GRecycleTime      = 3600; ///< Socket recycle time 1 hour( 3600 sec)
//---------------------------------------------------------------------------
PSocket::PSocket( void )
: FFD( -1 )
, FSocketType( SOCK_STREAM )
, FBlockMode( BLOCKING_SOCKET )
, FHostID( 0 )
, FWriteTimeout( 0 )
, FHeartbeatInterval( GHeartBeatTime )
, FTimeout( -GHeartBeatTimeout )
, FTimerCountRead( GHeartBeatTime )
, FTimerCountWrite( GHeartBeatTime - 1 )
, FDeleteCounter( GRecycleTime )
, FCheckHeartbeat( htCheckBoth )
{
}
//---------------------------------------------------------------------------
PSocket::PSocket( PSocket* Socket )
: FFD( Socket->FFD )
, FSocketType( Socket->FSocketType )
, FBlockMode( Socket->FBlockMode )
, FHostID( 0 )
, FWriteTimeout( 0 )
, FHeartbeatInterval( GHeartBeatTime )
, FTimeout( -GHeartBeatTimeout )
, FTimerCountRead( GHeartBeatTime )
, FTimerCountWrite( GHeartBeatTime - 1 )
, FDeleteCounter( GRecycleTime )
, FCheckHeartbeat( htCheckBoth )
{
}
//---------------------------------------------------------------------------
PSocket::PSocket( Int32 SocketType, BOOL BlockMode )
: FFD( -1 )
, FSocketType( SocketType )
, FBlockMode( BlockMode )
, FHostID( 0 )
, FWriteTimeout( 0 )
, FHeartbeatInterval( GHeartBeatTime )
, FTimeout( -GHeartBeatTimeout )
, FTimerCountRead( GHeartBeatTime )
, FTimerCountWrite( GHeartBeatTime - 1 )
, FDeleteCounter( GRecycleTime )
, FCheckHeartbeat( htCheckBoth )
{
}
//---------------------------------------------------------------------------
PSocket::PSocket( Int32 FD, Int32 SocketType, Int32 BlockMode )
: FFD( FD )
, FSocketType( SocketType )
, FBlockMode( BlockMode )
, FHostID( 0 )
, FWriteTimeout( 0 )
, FHeartbeatInterval( GHeartBeatTime )
, FTimeout( -GHeartBeatTimeout )
, FTimerCountRead( GHeartBeatTime )
, FTimerCountWrite( GHeartBeatTime - 1 )
, FDeleteCounter( GRecycleTime )
, FCheckHeartbeat( htCheckBoth )
{
}
//---------------------------------------------------------------------------
PSocket::~PSocket( void )
{
	CloseSocket();
}
//---------------------------------------------------------------------------
// Set the heartbeat type
// htCheckRead --> Check read I/O only
// htCheckRead --> Check write I/O only
// htCheckBoth --> Check both read and write I/O
//---------------------------------------------------------------------------
void PSocket::SetCheckHeartbeatType( HeartbeatType NewType )
{
    FCheckHeartbeat = NewType;
}
//---------------------------------------------------------------------------
// Set the heartbeat interval
// TimeInterval --> perform heartbeat after idle more then "TimeInterval" sec.
// TimeOut --> If the program don't receive a heartbeat reply after "TimeOut" sec,
//             the program will close the connection.
//---------------------------------------------------------------------------
void PSocket::SetHeartbeatInterval( Int16 TimeInterval, Int16 TimeOut )
{
    FTimeout           = -TimeOut - 1;
    FHeartbeatInterval = TimeInterval;
    FTimerCountRead    = TimeInterval;
    FTimerCountWrite   = TimeInterval - 1;
}
//---------------------------------------------------------------------------
// Reset the heartbeat counter.
//---------------------------------------------------------------------------
void  PSocket::ResetHeartbeatTimer( BOOL IsRead )
{
   if( IsRead == TRUE )
        FTimerCountRead  = FHeartbeatInterval + 1; ///< Reset the counter to heartbeat interval.
   else
        FTimerCountWrite = FHeartbeatInterval - 1; ///< Reset the counter to heartbeat interval.
}
//---------------------------------------------------------------------------
// Set the socket state to waiting for heartbeat reply state.
//---------------------------------------------------------------------------
void  PSocket::WaitForHeartbeatReplyState( BOOL IsRead )
{
    if( IsRead == TRUE )
    {
        if( FTimerCountRead == 0 ) ///< Needs to heartbeat state
            FTimerCountRead  = -1; ///< -1 means begin to countdown hearbeat timeout.
    }
    else
    {
        if( FTimerCountWrite == 0 ) ///< Needs to heartbeat state
            FTimerCountWrite = -1; ///< -1 means begin to countdown hearbeat timeout.
    }
}
//---------------------------------------------------------------------------
// Countdown the heartbeat counter.
//---------------------------------------------------------------------------
void PSocket::HeartbeatTimerCountDown( void )
{
    if( FTimerCountRead > FTimeout && FTimerCountRead != 0 )///< Zero means need to send heartbeat.
        FTimerCountRead--;
    if( FTimerCountWrite > FTimeout && FTimerCountWrite != 0 )///< Zero means need to send heartbeat.
        FTimerCountWrite--;
}
//---------------------------------------------------------------------------
// It's time to send heartbeat?
//---------------------------------------------------------------------------
BOOL  PSocket::NeedSendHeartbeat( BOOL IsRead )
{
    if( IsRead )
    {
        if( FCheckHeartbeat == htCheckWrite ) ///< Check write only!
            return FALSE;                     ///< didn't need to check read countor.
        return ( FTimerCountRead == 0 );
    }
    else
    {
	if( FCheckHeartbeat == htCheckRead )  ///< Check Read only!
            return FALSE;                     ///< didn't need to check write countor.
        return ( FTimerCountWrite == 0 );
    }
}
//---------------------------------------------------------------------------
// Reply heartbeat timeout ??
//---------------------------------------------------------------------------
BOOL  PSocket::NeedKill( void )
{
    switch( FCheckHeartbeat )
    {
        case htCheckRead:  return ( FTimerCountRead <= FTimeout );
        case htCheckWrite: return ( FTimerCountWrite <= FTimeout );
        default:  return ( FTimerCountRead <= FTimeout || FTimerCountWrite <= FTimeout );
    }
}
//---------------------------------------------------------------------------
void  PSocket::SetDeleteCounter( Int16 Value )
{
    FDeleteCounter = Value;
}
//---------------------------------------------------------------------------
Int16 PSocket::DeleteCountDown( void )
{
    FDeleteCounter--;
    return FDeleteCounter;
}
//---------------------------------------------------------------------------
Int32 PSocket::OpenSocket( void )
{
    if( FFD > 0 )
        CloseSocket( );
    if( (FFD = socket( AF_INET, FSocketType, 0 )) > 0 )
    {
        SetBlockMode( FBlockMode );
    }
    else
    {
        #ifdef _WIN32
        	throw( SocketException( WSAGetLastError() ) );
        #else
			throw( SocketException( errno ) );
        #endif
    }
    return FFD;
}
//---------------------------------------------------------------------------
void PSocket::CloseSocket( void )
{
    if( FFD > 0 )
    {
        Int32 ErrNO;
    #ifdef _WIN32
        shutdown( FFD, 2 );
        if( closesocket( FFD ) != 0 )
        {
            ErrNO = WSAGetLastError();
    #else
        shutdown( FFD, SHUT_RDWR );
        if( close( FFD ) != 0 )
        {
            ErrNO = errno;
    #endif
            BufferedLog::Printf(" Close socket failed FD:%d ErrNO:%d", FFD, ErrNO );
        }
        FFD = -1;
    }
}
//---------------------------------------------------------------------------
void PSocket::Bind( const AnsiString& Address, Int32 Port )
{
	if( FFD <= 0 )
        throw( SocketException( "Invalid socket handle." ) );
    struct sockaddr_in addr_sin;

    memset( &addr_sin, 0, sizeof(addr_sin) );
    addr_sin.sin_family = AF_INET;
    addr_sin.sin_port   = htons( (UInt16)Port );

    if( Address.Length() == 0 || Address == "any" )
        addr_sin.sin_addr.s_addr = htonl( INADDR_ANY );
    else
        addr_sin.sin_addr.s_addr = inet_addr( Address.c_str() );

    if( bind( FFD, (struct sockaddr*)&addr_sin, sizeof(addr_sin) ) == SOCKET_ERROR )
    {
        AnsiString Reson;
        #ifdef _WIN32
            Int32 ErrNO = WSAGetLastError();
        #else
            Int32 ErrNO = errno;
        #endif
        Reson.Printf( "bind fail.Address:[%s] Port:[%d] ErrNo:[%d]", Address.c_str(), Port, ErrNO );
        throw( SocketException( Reson.c_str() ) );
    }
}
//------------------------------------------------------------------------------
BOOL PSocket::IsBadSocket()
{
    struct stat Buf;
    fstat( FFD, &Buf );
    return (errno == EBADF);
}
//------------------------------------------------------------------------------
void PSocket::CheckConnection( const Int32 TimeUSec )
{
	AnsiString Reason;
    struct timeval tval;
    fd_set rset, wset;
    Int32  RtnCode;

    FD_ZERO( &rset );
    FD_ZERO( &wset );
    FD_SET( (u_int)FFD, &rset );
    FD_SET( (u_int)FFD, &wset );
    tval.tv_sec  = TimeUSec / 1000000;
    tval.tv_usec = TimeUSec % 1000000;
    RtnCode = select( FFD + 1, &rset, &wset, NULL, &tval );
    if( RtnCode == 0 )
    {
		Reason.Printf( "Connect Timeout %d seconds", (TimeUSec/1000000) );
        throw( SocketException( Reason.c_str() ) );
    }
    else if( RtnCode == -1 )
        throw( SocketException( "Select failed in nonblocking connect call") );
    else
    {
        Int32 error = 0;
        Int32 len = sizeof( Int32 );

        GetOption( SOL_SOCKET, SO_ERROR, (Int8*)&error, &len );
        /// Check error through getsockopt
        if( error != 0 )
            throw( SocketException("connect exception in non-blocking mode") );
    }
}
//------------------------------------------------------------------------------
void PSocket::Connect( const AnsiString& Address, const Int32 Port, const Int32 Sec, const Int32 LocalPort )
{
    AnsiString Reason;
    struct sockaddr_in sin;
#ifdef  _WIN32
    struct hostent*    hosts;    
    unsigned long      InternetAddr;
#else
    in_addr_t          InternetAddr;
#endif
    Int32              tempBlockMode = FBlockMode;    
    
    ///< Is a valid socket FD.
    if( FFD <= 0 )
        throw( SocketException( "Invalid socket handle." ) );
    // Bind to a local port.
    if( LocalPort != ANY_LOCAL_PORT )
        Bind( "any", LocalPort );
    ///< complete the socket structure
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons( (UInt16)Port );

#ifdef  _WIN32 ///< Windows XP not support  getaddrinfo
	if( (InternetAddr = inet_addr( Address.c_str() )) == (unsigned long)-1 ) ///< It's IP fromat string?
    {
        if( (hosts = gethostbyname( Address.c_str() )) != NULL ) // gets host information corresponding to a hostname
        {
            AnsiString IPStr( inet_ntoa( *(struct in_addr*)(void*)hosts->h_addr_list[0] ) );
        
            BufferedLog::Printf( " Resolve url[%s]->IP[%s]", Address.c_str(), IPStr.c_str() );
            sin.sin_addr.s_addr = inet_addr( IPStr.c_str() );
        }
        else
        {
            Reason.Printf( "Resolve URL[%s] failed.", Address.c_str() );
            throw( SocketException( Reason.c_str() ) );
        }
    }
    else
        sin.sin_addr.s_addr = InternetAddr;//inet_addr( IPStr.c_str() );
#else
    if( (InternetAddr = inet_addr( Address.c_str() )) == (in_addr_t)(-1) ) ///< It's IP fromat string?
    {
		struct addrinfo hints, *res;
		int    status;
		char   ipstr[ INET6_ADDRSTRLEN ];

		memset(&hints, 0, sizeof hints);
		hints.ai_family   = AF_INET; //AF_UNSPEC:All AF_INET:ipV4 or AF_INET6:ipV6
		hints.ai_socktype = SOCK_STREAM;
		if( (status = getaddrinfo( Address.c_str(), NULL, &hints, &res)) == 0 )
		{
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
			void*  addr = &(ipv4->sin_addr);

			inet_ntop( res->ai_family, addr, ipstr, sizeof ipstr );
			BufferedLog::Printf( " Resolve url[%s]->IP[%s]", Address.c_str(), ipstr );
			sin.sin_addr.s_addr = inet_addr( ipstr );
			freeaddrinfo( res );
		}
		else
		{
			Reason.Printf( "Resolve URL[%s] failed,Reason[%s]", Address.c_str(), gai_strerror( status ) );
			throw( SocketException( Reason.c_str() ) );
		}
    }
    else
        sin.sin_addr.s_addr = InternetAddr;//inet_addr( IPStr.c_str() );
#endif
	// Change to Nonblocking mode for connect with timeout.
	if ( tempBlockMode == BLOCKING_SOCKET )
		SetBlockMode( NONBLOCKING_SOCKET );
    if( connect( FFD, (struct sockaddr*)&sin, sizeof(sin)) != 0 )
    {
        #ifdef _WIN32
            Int32 ErrNo = WSAGetLastError( );
            if( ErrNo != WSAEWOULDBLOCK ) /// As an nonblocking socket the connection cannot be completed immediately.
            {
                // Restore BlockMode for Blocking
                if ( tempBlockMode == BLOCKING_SOCKET )
                    SetBlockMode( BLOCKING_SOCKET );
                Reason.Printf( "Connect to %s:%d fail code:%d FD:%d", Address.c_str(), Port, ErrNo, FFD );
                throw( SocketException( Reason.c_str() ) );
            }
        #else
            if( errno != EINPROGRESS )    /// In Win32 equals to WSAEWOULDBLOCK.
            {
                // Restore BlockMode for Blocking
                if ( tempBlockMode == BLOCKING_SOCKET )
                    SetBlockMode( BLOCKING_SOCKET );
                Reason.Printf( "Connect to %s:%d fail code:%d FD:%d", Address.c_str(), Port, errno, FFD );
                throw( SocketException( Reason.c_str() ) );
            }
        #endif
		try
		{
			CheckConnection( Sec * 1000000 );
		}
		catch( SocketException & e )
		{
			// When CheckConnection() throw exception
			// Restore BlockMode for Blocking
			if ( tempBlockMode == BLOCKING_SOCKET )
				SetBlockMode( BLOCKING_SOCKET );
			throw( e );
		}
	}
	// Restore BlockMode for Blocking
	if ( tempBlockMode == BLOCKING_SOCKET )
		SetBlockMode( BLOCKING_SOCKET );
}
//---------------------------------------------------------------------------
void PSocket::SetOption( Int32 Level, Int32 OptionName, const void* OptionValue, Int32 OptionLength )
{
    if( FFD <= 0 )
        throw( SocketException( "Invalid socket handle." ) );

    Int32 ErrNo = 0;
    #ifndef _WIN32
    if( setsockopt( FFD, Level, OptionName, OptionValue, (socklen_t)OptionLength ) == SOCKET_ERROR )
        ErrNo = errno;
    #else
    if( setsockopt( FFD, Level, OptionName, (const char FAR*)OptionValue, OptionLength ) == SOCKET_ERROR )
        ErrNo = WSAGetLastError();
    #endif
    if( ErrNo != 0 )
    {
        BufferedLog::Printf( " Set socket option error, Level:0x%X, Name:0x%X, Errno:%d", Level, OptionName, ErrNo );
        throw( SocketException( "Set socket options failed." ) );
    }

}
//---------------------------------------------------------------------------
void PSocket::GetOption( Int32 Level, Int32 OptionName, Int8* OptionValue, Int32* OptionLength )
{
    if( FFD <= 0 )
        throw( SocketException( "Invalid socket handle." ) );

    #ifndef _WIN32
    if( getsockopt( FFD, Level, OptionName, OptionValue, (socklen_t*)OptionLength) == SOCKET_ERROR )
    #else
    if( getsockopt( FFD, Level, OptionName, OptionValue, OptionLength) == SOCKET_ERROR )
    #endif
        throw( SocketException( "throw exception when getting socket option" ) );
}
//---------------------------------------------------------------------------
void PSocket::SetReuse( BOOL IsReuse )
{
    SetOption( SOL_SOCKET, SO_REUSEADDR, &IsReuse, sizeof(Int32) );
}
//---------------------------------------------------------------------------
void PSocket::SetLinger( BOOL OnOff, int Time )
{
    struct linger lingeropt;

    lingeropt.l_onoff  = OnOff;///< Turn On/off the Linger
    lingeropt.l_linger = Time; ///< Set timeout.
    SetOption( SOL_SOCKET,SO_LINGER,&lingeropt, sizeof( lingeropt ));
}    
//---------------------------------------------------------------------------
void PSocket::SetTCPNoDelay(  BOOL IsNoDelay )
{
    Int32 NoDelay = IsNoDelay;
    Int32 ErrNo = 0;
    #ifndef _WIN32
    if( setsockopt( FFD, IPPROTO_TCP, TCP_NODELAY, &NoDelay, (socklen_t)sizeof(Int32) ) == SOCKET_ERROR )
        ErrNo = errno;
    #else
    if( setsockopt( FFD, IPPROTO_TCP, TCP_NODELAY, (const char*)&NoDelay, sizeof(Int32) ) == SOCKET_ERROR )
        ErrNo = WSAGetLastError();
    #endif
    if( ErrNo != 0 )
        throw( SocketException( "Set socket option TCP_NODELAY failed." ) );
}
//---------------------------------------------------------------------------
//
// Param: IsNonBlocking
//
// SetBlockMode( TRUE )   <--- Non-Blocking mode socket
// SetBlockMode( FALSE )  <--- Non-Blocking mode socket
//---------------------------------------------------------------------------
void PSocket::SetBlockMode( BOOL IsNonBlocking )
{
    unsigned long Option;

    if( FFD <= 0 )
        throw( SocketException( "Invalid socket handle." ) );
    if( IsNonBlocking == TRUE )
        Option = NONBLOCKING_SOCKET;///< Nonblocking mode.
    else
        Option = BLOCKING_SOCKET;   ///< Blocking mode.
    #ifdef _WIN32
    if( ioctlsocket( FFD, FIONBIO, &Option ) == SOCKET_ERROR )
        throw( SocketException( "throw exception when setting block mode" ) );
    #else
    Int32 iFlags = fcntl( FFD, F_GETFL, 0 );
    if( Option == NONBLOCKING_SOCKET )
        iFlags = iFlags | O_NONBLOCK;
    else
        iFlags = iFlags &~ O_NONBLOCK;
    if( fcntl( FFD, F_SETFL, iFlags ) == SOCKET_ERROR )
        throw( SocketException( errno ) );
    #endif
    FBlockMode = Option;
}
//------------------------------------------------------------------------------
Int32 PSocket::SetBufferSize( Int32 Option, Int32 Size )
{
    Int32 FinalSize;
    Int32 IntLen = sizeof( Int32 );

    SetOption( SOL_SOCKET, Option, &Size, IntLen);
    GetOption( SOL_SOCKET, Option, (Int8*)&FinalSize, &IntLen);

    return FinalSize;
}
//------------------------------------------------------------------------------
void PSocket::SetBroadcast( Int32 Option )
{
	SetOption( SOL_SOCKET, SO_BROADCAST, &Option, sizeof( Int32 ));
}
//-----------------------------------------------------------------------------------------------------------------------
void PSocket::SetWriteTimeout( Int32 TimeoutMS )
{
    FWriteTimeout = TimeoutMS;
#ifndef  __SOLARIS    
    struct timeval tv;

    tv.tv_sec  = TimeoutMS/1000;
    tv.tv_usec = (TimeoutMS%1000)*1000;
    SetOption( SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof( tv ) );	    
#endif    
}    
//-----------------------------------------------------------------------------------------------------------------------
// TCP write buffer full?
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void PSocket::ReadyToWrite( void )
{   
///< Solaris not support SO_SNDTIMEO socket option.
///< On Solaris, use poll to replace the SO_SNDTIMEO socket option.    
#ifdef  __SOLARIS
    if( FWriteTimeout == 0 )
        return; ///< Timeout not set, Use system default
    Int32         PollCount;
    struct pollfd PollFD;    
    Int32         BeginTime = UFC::GetTickCountMS();
    Int32         WaitTime  = FWriteTimeout;
    Int32         UseTime;  
    
    while( WaitTime > 0 ) ///< Still need to wait.
    {
        PollFD.fd     = FFD;
        PollFD.events = POLLWRNORM;                
        if( (PollCount = poll( &PollFD, 1, WaitTime )) == 0 ) ///< Time is up or interrupt by signal.
        {
            UseTime = UFC::GetTickCountMS() - BeginTime;
            BeginTime += UseTime;
            WaitTime  -= UseTime;
            if( WaitTime <= 0 )
                throw( SocketException( "call socket send timeout." ) ); ///< time is up.                            
        }
        else if( PollCount > 0 )
        {
            return; ///< Ready to write.
        }
        else
        {
            if( errno == EINTR ) ///< Interrupt by signal
            {
                UFC::BufferedLog::DebugPrintf("### poll Interrupt by signal (write) ###");
                continue;
            }
            else if( errno == EAGAIN )
                continue;
            throw( SocketException( errno ) );
        }
    }
#else
    ///< AIX and Linux support SO_SNDTIMEO socket option.    
    ///< return directly, use Socket layer write timeout.
    return;
#endif    
}
//------------------------------------------------------------------------------
Int32 PSocket::SendBuffer( const Int8* SendData, Int32 Size )
{
    while( TRUE )
    {
#ifdef  __SOLARIS        
        ReadyToWrite();
#endif            
        Int32 SendSize = send( FFD, SendData, Size, 0  );
        if( SendSize < 0 )
        {
        #ifdef _WIN32
            Int32 ErrNo = WSAGetLastError();

            if( ErrNo == WSAEWOULDBLOCK )
            {
                BufferedLog::DebugPrintf("######## S:Nonblocking ###########");
                return 0;
            }
            else
            {
                AnsiString Reason;
                Reason.Printf( "SendBuffer failed. Error code:%d", ErrNo );
                throw( SocketException( Reason.c_str() ) );
            }
        #else
            if( errno == EINTR )
            {
                BufferedLog::DebugPrintf("####### Interrupt ###########");
                continue;
            }
            else if( errno == EPIPE )
                throw( SocketException( "SendBuffer to a broken Pipe." ) );
            else
                throw( SocketException( errno ) );
        #endif
        }
        ResetHeartbeatTimer( FALSE );
        return SendSize;
    }
}
//---------------------------------------------------------------------------
Int32 PSocket::SendTo( const AnsiString& Address, Int32 Port, const char* SendData, Int32 Size )
{
    struct sockaddr_in	SendAddress;
    Int32 len = sizeof( struct sockaddr_in );
    memset( &SendAddress, 0, len );
    SendAddress.sin_family = AF_INET;
    SendAddress.sin_port = htons( (UInt16)Port );
    SendAddress.sin_addr.s_addr = inet_addr( Address.c_str() );

    while( TRUE )
    {
        Int32 SendSize = sendto( FFD, SendData, Size, 0, (sockaddr*)&SendAddress, len );
        if( SendSize < 0 )
        {
        #ifdef _WIN32
            Int32 ErrNo = WSAGetLastError();

            if( ErrNo == WSAEWOULDBLOCK )
            {
                BufferedLog::DebugPrintf("######## S:Nonblocking ##########");
                continue;
            }
            else
            {
                AnsiString Reason;
                Reason.Printf( "SendTo failed. Error code:%d", ErrNo );
                throw( SocketException( Reason.c_str() ) );
            }
        #else
            if( errno == EINTR )
            {
                BufferedLog::DebugPrintf("####### Interrupt ###########");
                continue;
            }
            else if( errno == EPIPE )
                throw( SocketException( "SendTo to a broken Pipe." ) );
            else
                throw( SocketException( errno ) );
        #endif
        }
        ResetHeartbeatTimer( FALSE );
        return SendSize;
    }
}
//------------------------------------------------------------------------------
void PSocket::BlockSend( Int8* SendData, Int32 Size )
{
    char* DataBuffer = SendData;
    Int32 SentSize = Size;
    Int32 WriteSize;

    while( SentSize > 0 )
    {
        WriteSize = SendBuffer( DataBuffer, SentSize );
        DataBuffer += WriteSize;
        SentSize   -= WriteSize;
    }
}
//---------------------------------------------------------------------------
Int32 PSocket::Receive( UInt8* RecvData, Int32 Size )
{
    while( TRUE )
    {
        Int32 RecvSize = recv( FFD, (char*)RecvData, Size, 0 );
        if( RecvSize < 0 )
        {
        #ifdef _WIN32
            Int32 ErrNo = WSAGetLastError();

            if( ErrNo == WSAEWOULDBLOCK )
            {
                BufferedLog::DebugPrintf("######## R:Nonblocking #########");
                return 0;
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
                continue;
            }
            else if( errno == EPIPE )
                throw( SocketException( "RecvBuffer from a broken Pipe." ) );
            else
            {
                throw( SocketException( errno ) );//"RecvBuffer failed." ) );
            }
        #endif
        }
        else if( RecvSize == 0 )
            throw( SocketException("the connection has been gracefully closed") );
        ResetHeartbeatTimer( TRUE );
        return RecvSize;
    }
}
//---------------------------------------------------------------------------
Int32 PSocket::RecvBuffer( UInt8* RecvData, Int32 Size )
{
    Int32 Result = Receive( RecvData, Size );
    return Result;
}
//---------------------------------------------------------------------------
void PSocket::BlockRecv( UInt8* RecvData, Int32 Size )
{
    UInt8* DataBuffer = RecvData;
    Int32 RecvedSize = Size;
    Int32 ReadSize;

    while( RecvedSize > 0 )
    {
        ReadSize = RecvBuffer( (UInt8*)DataBuffer, RecvedSize );
        DataBuffer += ReadSize;
        RecvedSize -= ReadSize;
    }
}
//---------------------------------------------------------------------------
void PSocket::Purge( void )
{
    UInt8 ReadByte;

    for(;;)
    {
        if( Receive(&ReadByte, 1) == 1 )
			continue;
        else
            break;
    }
}
//---------------------------------------------------------------------------
Int32 PSocket::ReceiveWithTimeout( UInt8* RecvData, Int32 Size, Int16 TimeOut )
{
    AnsiString Reason;
    struct timeval tval;
    fd_set rset;
    Int32  RtnCode;

	// Check if data arrive by TimeOut
    FD_ZERO( &rset );
    FD_SET( (u_int)FFD, &rset );
    tval.tv_sec  = TimeOut;
    tval.tv_usec = 0;
    RtnCode = select( FFD + 1, &rset, NULL, NULL, &tval );
    if( RtnCode == 0 )
	{
		Reason.Printf( "Receive data Timeout %d seconds", tval.tv_sec );
        throw( SocketException( Reason.c_str() ) );
	}
    else if( RtnCode == -1 )
        throw( SocketException( "Select failed in receive call") );
    else if( !(FD_ISSET( FFD, &rset )) )
	{
		Reason.Printf( "Receive data Timeout %d seconds", tval.tv_sec );
		throw( SocketException( Reason.c_str() ) );
    }
	// When data arrive call Receive
	return Receive( RecvData, Size );
}
//---------------------------------------------------------------------------
Int32 PSocket::RecvBufferWithTimeout( UInt8* RecvData, Int32 Size, Int16 TimeOut )
{
	Int32 Result = ReceiveWithTimeout( RecvData, Size, TimeOut );
	return Result;
}
//---------------------------------------------------------------------------
AnsiString PSocket::GetLocalHostName( void )
{
    char HostName[ 128 ];

    memset( HostName, 0, 128 * sizeof(char) );
    gethostname( HostName, sizeof(HostName) );
    return AnsiString( HostName );
}
//---------------------------------------------------------------------------
Int32 PSocket::GetLocalIPAddress( void )
{
#ifndef __LINUX
	AnsiString HostName = GetLocalHostName();

    if( HostName.Length() > 0 )
    {
        struct sockaddr_in SocketAddress;
        struct hostent     *pHost;

        if( (pHost = gethostbyname( HostName.c_str() )) != NULL )
        {
            for( Int32 i = 0; ((pHost->h_addr_list[i]) && (i < 10)); ++i)
            {
                memcpy( &SocketAddress.sin_addr, pHost->h_addr_list[i], pHost->h_length );

                AnsiString LocalAddress( inet_ntoa(SocketAddress.sin_addr) );

                if( LocalAddress != "127.0.0.1" && LocalAddress != "0.0.0.0" )
                    return (Int32)inet_addr( LocalAddress.c_str() );
            }
        }
    }
    return -1;
#else
	long ip = -1;
	int fd, intrface;
	struct ifreq buf[256]; ///if.h
	struct ifconf ifc; ///if.h

	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) //socket.h
	{
		ifc.ifc_len = sizeof buf;
		ifc.ifc_buf = (caddr_t) buf;
		if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) //ioctl.h
		{
			intrface = ifc.ifc_len / sizeof (struct ifreq);
			while (intrface-- > 0)
			{
				if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
				{
					ip=inet_addr( inet_ntoa( (struct in_addr) ((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr) );//types
					break;
				}
			}
		}
		close (fd);
	}
	Int32 Addr;

	NInt32::ToInt32( ip , (UInt8*)&Addr );
	return Addr;
 
#endif
}
//---------------------------------------------------------------------------
AnsiString PSocket::GetLocalIP( void )
{
#ifndef __LINUX
	AnsiString HostName = GetLocalHostName();

	if( HostName.Length() > 0 )
	{
		struct sockaddr_in SocketAddress;
		struct hostent     *pHost;

		if( (pHost = gethostbyname( HostName.c_str() )) != NULL )
		{
			for( Int32 i = 0; ((pHost->h_addr_list[i]) && (i < 10)); ++i)
			{
				memcpy( &SocketAddress.sin_addr, pHost->h_addr_list[i], pHost->h_length );

				AnsiString LocalAddress( inet_ntoa(SocketAddress.sin_addr) );

				if( LocalAddress != "127.0.0.1" && LocalAddress != "0.0.0.0" )
					return LocalAddress;
			}
		}
	}
	return "127.0.0.1";
#else
	int fd, intrface;
	struct ifreq buf[256]; ///if.h
	struct ifconf ifc; ///if.h

	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) //socket.h
	{
		ifc.ifc_len = sizeof buf;
		ifc.ifc_buf = (caddr_t) buf;
		if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) //ioctl.h
		{
			intrface = ifc.ifc_len / sizeof (struct ifreq);
			while (intrface-- > 0)
			{
				if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
				{
					AnsiString LocalAddress( inet_ntoa( (struct in_addr) ((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr) );//types
					if ( LocalAddress != "127.0.0.1" && LocalAddress != "0.0.0.0" )
                    {
                        close( fd );
						return LocalAddress;
                    }
				}
			}
		}
		close (fd);
	}
	return "127.0.0.1";
#endif
}
//---------------------------------------------------------------------------
void  PSocket::Attach( int FD )
{
    FFD = FD;
}
//---------------------------------------------------------------------------
void  PSocket::Detach( void )
{
    FFD = -1;
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

