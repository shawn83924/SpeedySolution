//---------------------------------------------------------------------------
// PServerSocket.cpp
//---------------------------------------------------------------------------
#include "PServerSocket.h"
#include "NetUtility.h"
#include "BufferedLog.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
//
//  PServerSocket
//
//---------------------------------------------------------------------------
PServerSocket::PServerSocket( PServerSocketListener* Linstener )
:PSocket( SOCK_STREAM, NONBLOCKING_SOCKET )
,PThread( NULL, FALSE )
,FListener( Linstener )
,FIPAddress("")
,FPort(0)
,FMaxConnection( MAX_CONNECTION )
,FIsRunning( FALSE )
{
#ifdef __USE_EPOLL    
    FEpollFD = epoll_create( 1024 );
#endif        
    Start();
}
//---------------------------------------------------------------------------
PServerSocket::PServerSocket( PServerSocketListener* Linstener, Int32 Port, Int32 MaxConnections )
:PSocket( SOCK_STREAM, NONBLOCKING_SOCKET )
,PThread( NULL, FALSE )
,FListener( Linstener )
,FIPAddress( "" )
,FPort( Port )
,FMaxConnection( MaxConnections )
,FIsRunning( FALSE )
{
#ifdef __USE_EPOLL    
    FEpollFD = epoll_create( 1024 );
#endif    
    Start();
}
//---------------------------------------------------------------------------
PServerSocket::PServerSocket( PServerSocketListener* Linstener, const AnsiString & Address, Int32 Port, Int32 MaxConnections )
:PSocket( SOCK_STREAM, NONBLOCKING_SOCKET )
,PThread( NULL, FALSE )
,FListener( Linstener )
,FIPAddress( Address )
,FPort( Port )
,FMaxConnection( MaxConnections )
,FIsRunning( FALSE )
{
#ifdef __USE_EPOLL    
    FEpollFD = epoll_create( 1024 );
#endif    
    Start();
}
//---------------------------------------------------------------------------
PServerSocket::~PServerSocket()
{
    Stop();      ///< Stop the listen socket.
    Terminate(); ///< Try to terminate the accept thread.
    WaitFor();   ///< Wait for thread terminated.
#ifdef __USE_EPOLL        
    close( FEpollFD );    
#endif                  
}
//---------------------------------------------------------------------------
void PServerSocket::Listen( void )
{    
    OpenSocket();
    SetReuse( 1 );
    Bind( FIPAddress, FPort );
    if( listen( FFD, FMaxConnection ) == -1 )
        throw( SocketException( "PServerSocket listen failed!\n") );
    
#ifdef __USE_EPOLL    
    struct epoll_event event;
    
    SetBlockMode( TRUE );
    event.data.fd = FFD;
    event.events = EPOLLIN; ///< Level trigger 
    if( epoll_ctl ( FEpollFD, EPOLL_CTL_ADD, FFD, &event) == -1 )
    {
      BufferedLog::Printf( " PServerSocket epoll_ctl failed!" );
      throw( SocketException( errno ) );  
    }
#endif    
    
    FIsRunning = TRUE;  ///< Let the thread begin selecting.
}
//---------------------------------------------------------------------------
void PServerSocket::Stop( void )
{   
    FIsRunning = FALSE; ///< Let the thread in sleeping mode.
}
//---------------------------------------------------------------------------
PClientSocket * PServerSocket::Accept( void )
{
    struct sockaddr SocketAddress;
    socklen_t       SocketAddressSize = sizeof( sockaddr );
    Int32           NewConnectedFD;

    NewConnectedFD = accept( FFD, &SocketAddress, &SocketAddressSize );
    if( NewConnectedFD < 0 )
    {
	#ifdef _WIN32
            throw( SocketException( WSAGetLastError() ) );
        #else
            throw( SocketException( errno ) );
	#endif

    }
    return new PClientSocket( NewConnectedFD );
}
//---------------------------------------------------------------------------
void PServerSocket::CheckConnection( struct timeval& SelectTime )
{
#ifdef __USE_EPOLL
    int SelectCount;
    struct epoll_event events[ MAX_CONNECTION ];

    if( (SelectCount = epoll_wait (FEpollFD, events, MAX_CONNECTION, SelectTime.tv_sec*1000)) < 0 )
    {
        BufferedLog::Printf( " PServerSocket epoll_wait failed!" );
        throw( SocketException( errno ) );    
    }
    else if( SelectCount > 0 )
    {
        for( int i=0; i< SelectCount; i++ )
        {
            if( FFD == events[i].data.fd )
                FListener->OnAccept( this ); ///< Trigger OnAccept event.    
        }
    }
    ///< SelectCount == 0 ->No connection.
#else 
    Int32  SelectCount;

    FD_ZERO( &FReadSet );       ///< Empty the Read set.
    FD_SET( (u_int)FFD, &FReadSet );   ///< Add the server FD to set.
    if( (SelectCount = select( FFD + 1, &FReadSet, NULL, NULL, &SelectTime )) < 0 )
        throw( SocketException( "ServerSocket select failed.\n" ) );
    else if( SelectCount == 0 )
        return;           ///< No connection.
    else if( FD_ISSET( FFD, &FReadSet ) )
        FListener->OnAccept( this ); ///< Trigger OnAccept event.
#endif    
}
//---------------------------------------------------------------------------
void PServerSocket::Execute( void )
{
    struct timeval SelectTimeout;
      
    while( IsTerminated() == FALSE )
    {
        if ( FIsRunning == FALSE ) ///< Not running, sleep for a while.
        {
            UFC::SleepMS( 200 );
            continue;
        }        
        try
        {
            SelectTimeout.tv_sec  = 10;
            SelectTimeout.tv_usec = 0;
            CheckConnection( SelectTimeout ); ///< A client try to connect?
                
        }
        catch( SocketException & e )
        {
            BufferedLog::Printf("Accept failed, reason:%s\n", e.what() );
            Stop();
            throw( e );
        }
    }
    CloseSocket();
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
