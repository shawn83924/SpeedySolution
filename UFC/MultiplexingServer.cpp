#include "MultiplexingServer.h"
#include "BufferedLog.h"
#include "NetUtility.h"
#ifdef __LINUX
    #include <sys/epoll.h>
#endif
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
//
//	Class MultiplexingServer
//
//---------------------------------------------------------------------------
MultiplexingServer::MultiplexingServer( Int32 ListenPort, Int32 MaxConnection, Int32 ThrCount,Int32 SwitchTimeMS, const UFC::AnsiString& Rule, bool Busy  )
:PThread( NULL, FALSE ),
 SERVER_IDLE_TIME_MS( 800 ),
 FSocketMonitor(NULL),
 FServerListener( NULL ),
 FIPAddress( "" ),
  FIsRunning( FALSE ),
 FPort( ListenPort ),
 FMaxConnection( MaxConnection ),
 FThreadCount( ThrCount ),
 FThreadIdleTimeMS( SwitchTimeMS ),
 FCount( 0 ),
 FProcessedMessageCount( 0 ),
 FLastCheckHeartbeatTick( 0 ),
 FEnableHeartbeat( TRUE ),
 FUseBusyLoop( Busy )
{
    FSocketMonitor = new SocketIoMonitor( &FAllClients, &FClientListCS );
    FServerSocket  = new PServerSocket( this );
    FIdleTime.tv_sec   = 0;
    FIdleTime.tv_usec = 1000;
    if( FThreadCount > 0 )
        FThreadPool = new PThreadPool( FThreadCount, Rule );
    else
        FThreadPool = NULL;
#ifdef __USE_EPOLL    
    FEPollFD = epoll_create( 4096 );
#endif      
}
//---------------------------------------------------------------------------
//  Constructor
//---------------------------------------------------------------------------
MultiplexingServer::MultiplexingServer( const AnsiString & Address, Int32 ListenPort, Int32 MaxConnection, Int32 ThrCount, Int32 SwitchTimeMS, const UFC::AnsiString& Rule, bool Busy  )
:PThread( NULL, FALSE ),
 SERVER_IDLE_TIME_MS( 800 ),
 FSocketMonitor(NULL),
 FServerListener( NULL ),
 FIPAddress( Address ),
 FIsRunning( FALSE ),
 FPort( ListenPort ),
 FMaxConnection( MaxConnection ),
 FThreadCount( ThrCount ),
 FThreadIdleTimeMS( SwitchTimeMS ),
 FCount( 0 ),
 FProcessedMessageCount( 0 ),
 FLastCheckHeartbeatTick( 0 ),
 FEnableHeartbeat( TRUE ),
 FUseBusyLoop( Busy )
{
    FSocketMonitor = new SocketIoMonitor( &FAllClients, &FClientListCS );
    FServerSocket  = new PServerSocket( this );
    FIdleTime.tv_sec   = 0;
    FIdleTime.tv_usec = 1000;
    if( FThreadCount > 0 )  ///< Use thread pool
        FThreadPool = new PThreadPool( FThreadCount , Rule );
    else                    ///< Use signal thread
        FThreadPool = NULL;
#ifdef __USE_EPOLL    
    FEPollFD = epoll_create( 4096 );
#endif          
}
//---------------------------------------------------------------------------
//  Destructor
//---------------------------------------------------------------------------
MultiplexingServer::~MultiplexingServer( void )
{
#ifdef __USE_EPOLL
    close( FEPollFD );
#endif    
    delete FServerSocket;    ///< Delete server socket.(accept thread)
    FServerListener = NULL;
    Stop( );
    delete FSocketMonitor;
    if( FThreadPool != NULL )
        delete FThreadPool;
}
//---------------------------------------------------------------------------
//  Add a FClientSocket to the idle list.
//---------------------------------------------------------------------------
void MultiplexingServer::AddToIdleList( PClientSocket * ClientSocket )
{
    if( ClientSocket != NULL )
    {
    #ifdef __USE_EPOLL    
        struct epoll_event event;
        
        event.data.fd  = ClientSocket->GetHandle();
        event.data.ptr = ClientSocket;
        event.events   = EPOLLIN | EPOLLET;
        if( epoll_ctl ( FEPollFD, EPOLL_CTL_ADD, ClientSocket->GetHandle(), &event) == -1 )
        {
            UFC::AnsiString ErrMsg;
            
            ErrMsg.Printf( "AddToIdleList epoll_ctl add failed, Code[%d][%s].\n", errno, strerror( errno ) );
            throw( SocketException( ErrMsg.c_str() ) );              
        }        
    #else            
        PLockObject Lock( FClientListCS );
        
        FIdleList.Add( ClientSocket );      ///< Add to idle list
        if( FIdleList.ItemCount() == 1 )
            FAddToidleListEvent.SetEvent();
    #endif            
    }
}
//---------------------------------------------------------------------------
//  Remove a FClientSocket from the idle list.
//---------------------------------------------------------------------------
void MultiplexingServer::RemoveFromIdleList( PClientSocket* DelClient )
{
    #ifdef __USE_EPOLL    
        if( epoll_ctl ( FEPollFD, EPOLL_CTL_DEL, DelClient->GetHandle(), NULL ) == -1 )
            BufferedLog::Printf( " RemoveFromIdleList epoll_ctl delete failed, Code[%d][%s].", errno, strerror( errno ) );           
    #else                
        PLockObject Lock( FClientListCS );
        Int32       Index = FIdleList.IndexOf( DelClient );

        if( Index != -1 )
            FIdleList.Delete( Index );
    #endif
}
//---------------------------------------------------------------------------
//
//  Call this function when a new client connected.
//
//  Add a new PClientSocket to both clients and select list.
//
//---------------------------------------------------------------------------
void MultiplexingServer::AddConnection( PClientSocket* NewClientSocket )
{
    if( NewClientSocket != NULL )
    {
    #ifdef __USE_EPOLL    
        {
            PLockObject  Lock( FClientListCS );
            
            FAllClients.Add( NewClientSocket );        ///< Add to all client list.
        }        
        AddToIdleList( NewClientSocket );                
    #else                        
        PLockObject  Lock( FClientListCS );

        FAllClients.Add( NewClientSocket );        ///< Add to all client list.
        FIdleList.Add( NewClientSocket );          ///< Add to idle list.
        if( FIdleList.ItemCount() == 1 )
            FAddToidleListEvent.SetEvent();
    #endif
    }
    else
        BufferedLog::Printf( " Accept failed! NULL Connection object." );           
}
//------------------------------------------------------------------------------
//
//  Call this function when a client disconnected.
//
//  Remove the PClientSocket object in both all clients list and idle list.
//  and put this object into PClientSocket trash-can.
//  Call this function in following conditions:
//
//  ----------- In task threads scope -----------------
//  1. In DataArrived function, call OnClientWrite return FALSE.(Client request disconnect )
//  2. In DataArrived function, call OnClientWrite throw exception.(Read socket error)
//  ----------- In select thread scope ----------------
//  3. When select failed call RemoveErrorSocket.
//  4. In TriggerHeartbeat, call OnHeartbert return FALSE.
//  5. In TriggerHeartbeat, call OnHeartbert throw exception
//------------------------------------------------------------------------------
void MultiplexingServer::RemoveConnection( PClientSocket* DelClient, Int32 DelTime )
{
    Int32        Index;
    PLockObject  Lock( FClientListCS );
    
    #ifdef __USE_EPOLL    
        if( DelClient->GetHandle() != -1 )
        {
            if( epoll_ctl ( FEPollFD, EPOLL_CTL_DEL, DelClient->GetHandle(), NULL ) == -1 )
                BufferedLog::Printf( " RemoveConnection epoll_ctl delete FD:%d failed, Code[%d][%s].", DelClient->GetHandle(), errno, strerror( errno ) );           
        }
    #else       
        if( ( Index = FIdleList.IndexOf( DelClient ) ) != -1 )     ///< Is it in idle list ??
            FIdleList.Delete( Index );                             ///< Yes! remove it from idle list.
    #endif

    if( ( Index = FAllClients.IndexOf( DelClient ) ) != -1 )   ///< Is it in all clients list ??
        FAllClients.Delete( Index );                           ///< Yes! remove it from all cliets list.

    if( FThreadPool != NULL )                                  ///< Use the thread pool mechanism ??
        FThreadPool->CancelJob( this, (void*)DelClient );      ///< Yes! remove it from job queue.

    if( FServerListener != NULL )                              ///< Listener object exists ??
        FServerListener->OnClientDisconnect( this, DelClient );///< Yes! Trigger the OnClientDisconnect.

    FSocketMonitor->ToTrashCan( DelClient, DelTime );          ///< Delete the socket object after 1 hr.
    if( DelClient->IsConnect( ) == TRUE )                      ///< Socket connected ?
        DelClient->Disconnect( FALSE );                        ///< Disconnect it.
}
//------------------------------------------------------------------------------
void  MultiplexingServer::ToTrashcan( PClientSocket* DeleteSocket, int DelaySec )
{
     FSocketMonitor->ToTrashCan( DeleteSocket, DelaySec );         ///< Delete the socket object after 5 sec.
}
//------------------------------------------------------------------------------
// Delete all client connections.
//------------------------------------------------------------------------------
void MultiplexingServer::ClearAllClients( void )
{
    PLockObject    Lock( FClientListCS );
    PClientSocket* ClientSocket;

    #ifndef __USE_EPOLL    
        FIdleList.Clear();
    #endif
    for( Int32 i = FAllClients.ItemCount() - 1; i >= 0 ; i-- )
    {
        if( ( ClientSocket = (PClientSocket*)FAllClients.GetItem( i ) ) != NULL )
        {
            ClientSocket->Disconnect( TRUE );
            delete ClientSocket;
        }
    }
    FAllClients.Clear();
}
//------------------------------------------------------------------------------
// Implement PServerSocketListener
// The accept thread will call this function when a client try to connect to this server.
//------------------------------------------------------------------------------
void MultiplexingServer::OnAccept( PServerSocket* ServerSocket )
{
    PClientSocket * NewClient = NULL;
    try
    {
        NewClient = ServerSocket->Accept();
        AddConnection( NewClient );
        if( FServerListener != NULL )
            FServerListener->OnClientConnect( this, NewClient );        
    }
    catch( SocketException& e)
    {
        BufferedLog::DebugPrintf( " ### Thread %lu listen[%s:%d], Accept connection failed ! Total %d Connections ### %s", ServerSocket->GetThreadID(), ServerSocket->GetLocalIP().c_str(), ServerSocket->GetPort(), GetConnectionCount(), e.what());
        BufferedLog::FlushToFile();
        if ( FServerListener != NULL ) FServerListener->OnAcceptFailed( this, NewClient );
    }   
}
//------------------------------------------------------------------------------
BOOL MultiplexingServer::DataArrived( PClientSocket* ClientSocket )
{
     FProcessedMessageCount++;
     try
     {
        if( FServerListener != NULL )
        {
            if( FServerListener->OnClientWrite( this, ClientSocket ) != TRUE )
            {
                BufferedLog::DebugPrintf(" ### OnClientWrite return false ### - RemoveConnection");
                RemoveConnection( ClientSocket ); ///< Client request dissconnect
                return FALSE;                     ///< remove this connection.
            }
            return TRUE;
        }
        ClientSocket->Purge();
        return TRUE;
     }
     catch( SocketException & )
     {
        BufferedLog::DebugPrintf(" ### OnClientWrite throw exception ### - RemoveConnection");
        RemoveConnection( ClientSocket ); ///< Occur exception.
        return FALSE;                     ///< remove this connection.
     }
}
//---------------------------------------------------------------------------

PollResult MultiplexingServer::PollCheckData( struct pollfd& PollFD ) 
{
#ifndef _WIN32
    Int32           PoolCount;
    
    PoolCount = poll( &PollFD, 1, FThreadIdleTimeMS  );
    if( PoolCount > 0 )///< Data arrived! process again!
    {        
        return prDataArrived;      ///< Trigger OnDataArrived.
    }
    else if( PoolCount < 0 )
    {
        if( errno == EINTR )///< Modify by Simon 2009/2/11
        {
            UFC::BufferedLog::DebugPrintf("### poll Interrupt by signal ###");
            return prContinue;
        }
        else if( errno == EAGAIN )
            return prContinue;
        BufferedLog::DebugPrintf(" ### Poll failed,error code:%d ###", errno );
        return prError;
    }
    else              ///<  PoolCount =0, means this connection is idle now.
        return prBreak;       ///< Release this thread    
#else
    return prError;
#endif
}    
//---------------------------------------------------------------------------
PollResult MultiplexingServer::BusyCheckData( int FD ) 
{
#ifdef __LINUX
    char OneByte;

    Int32 RecvSize = recv( FD, (char*)&OneByte, 1, MSG_PEEK|MSG_DONTWAIT );
    if( RecvSize < 0 )
    {            
        if( errno == EINTR )
        {
            BufferedLog::Printf("####### Interrupt #########");
            return prContinue;
        }
        else if( errno == EAGAIN )
        {
            return prContinue;
        }
        else if( errno == EPIPE )
        {
            BufferedLog::Printf( " [BusyCheckData] RecvBuffer from a broken Pipe." );
            return prError;
        }
        else
        {
            BufferedLog::Printf( " [BusyCheckData] Recv error code[%d].", errno );
            return prError;
        }
    }
    else if( RecvSize == 0 )
    {
        BufferedLog::Printf(" [BusyCheckData] The connection has been gracefully closed.");
        return prError;
    }
    // Data arrived in blocking mode.
    return prDataArrived;
#else
    return prError;
#endif
}                
//---------------------------------------------------------------------------
Int32 MultiplexingServer::DoTask( void* Client )
{
     PClientSocket*  ClientSocket = (PClientSocket*) Client;
     Int32           Count = 1;       
#ifdef _WIN32
     Int32          SelectFD = ClientSocket->GetHandle();
     fd_set         ReadSet;
     struct timeval ThreadSwitchTime;
     
     while( TRUE )
     {
        Count++;
		ThreadSwitchTime.tv_sec  = FThreadIdleTimeMS/1000;
		ThreadSwitchTime.tv_usec = (FThreadIdleTimeMS%1000)*1000;
        if( DataArrived( (PClientSocket*) Client ) == FALSE )///< Trigger the OnDataArrived event.
            return FALSE;                                    ///< Client request disconnection.
        FD_ZERO( &ReadSet );       ///< Empty the Read set.
		FD_SET( (u_int)SelectFD, &ReadSet );  ///< Add the connection FD to set.
		if( select( SelectFD + 1, &ReadSet, NULL, NULL, &ThreadSwitchTime ) == 1 )
		{
			 if( FD_ISSET( SelectFD, &ReadSet ) ) ///< Data arrived! process again!
             {
                 if( Count %50 == 0 )
					 UFC::PThread::PThread_Yield();
                 continue;
             }
        }
        break; ///< This connection is idle now.
     }
#else     
     struct pollfd   PollFD;
     PollResult Result = prDataArrived;
     PollFD.fd         = ClientSocket->GetHandle();
     PollFD.events     = POLLRDNORM;
     while( TRUE )
     {
          
          if( Result == prDataArrived)
          {
            if( DataArrived( (PClientSocket*) Client ) == FALSE )///< Trigger the OnDataArrived event.
                return FALSE; ///< Client request disconnection.
          }
          else
              Count++;
          if( FUseBusyLoop == FALSE )     
          {
              if( Count %50 == 0 )
                  UFC::PThread::PThread_Yield();   
              Result = PollCheckData( PollFD );
          }
          else
              Result = BusyCheckData( PollFD.fd );          
          if( Result == prError )///< Poll failed
          {
              RemoveConnection( (PClientSocket*) Client  ); ///< Client request dissconnect
              return FALSE;                  
          }
          if( Result == prBreak ) ///< PoolCount =0, means this connection is idle now.
              break;                   ///< Release this thread
          
     }
#endif
     AddToIdleList( ClientSocket );  ///< This connection is idle now, add it to idle list.
     return TRUE;                    ///< Finished the task and release the job thread.
}
//---------------------------------------------------------------------------
BOOL MultiplexingServer::TriggerDataArrived( Int32 /*Index*/, PClientSocket* ClientSocket )
{
    if( FD_ISSET( ClientSocket->GetHandle(), &FReadSet ) )
    {
        FSelectCount--;
        if( FThreadPool == NULL )                     ///< Is a single thread socket server.
            return DataArrived( ClientSocket );
        // Remove the connection from the idle list to prevent
        // to trigger the OnDataArrived event more than one time.
        RemoveFromIdleList( ClientSocket );
        // Thread pool will let a thread to perform the task.
        // Return false means that no more thread available. add it to job queue to wait.
        FThreadPool->ExecuteTask( this, ClientSocket );                
    }
    return TRUE;
}
//---------------------------------------------------------------------------
BOOL MultiplexingServer::TriggerHeartbeat( Int32 /*Index*/, PClientSocket* ClientSocket )
{
    if (FEnableHeartbeat == TRUE)
    {
        if (ClientSocket->NeedKill() == TRUE)
        {
            BufferedLog::Printf(" Heartbeat timeout! Remove connection. FD:%d", ClientSocket->GetHandle());
            RemoveConnection( ClientSocket );
            return FALSE;
        }
        if (ClientSocket->NeedSendHeartbeat(TRUE)) ///< Read time is up, send heart beat.
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Read heartbeat time is up, trigger OnHeartbeat. FD:%d ", ClientSocket->GetHandle() );
            if (FServerListener->OnHeartbeat(ClientSocket, TRUE) == FALSE)
            {
                BufferedLog::Printf(" TriggerHeartbeat return false, Remove connection. FD:%d", ClientSocket->GetHandle());
                RemoveConnection(ClientSocket); ///< Send heartbeat failed, remove this connection.
                return FALSE;
            }
            ClientSocket->WaitForHeartbeatReplyState(TRUE); ///< Set to waiting for reply state.
        }
        if (ClientSocket->NeedSendHeartbeat(FALSE)) ///< Write time is up, send heart beat.
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Write heartbeat time is up, trigger OnHeartbeat. FD:%d ", ClientSocket->GetHandle() );
            if (FServerListener->OnHeartbeat(ClientSocket, FALSE) == FALSE)
            {
                BufferedLog::Printf(" TriggerHeartbeat return false, Remove connection. FD:%d", ClientSocket->GetHandle());                    
                RemoveConnection(ClientSocket); ///< Send heartbeat failed, remove this connection.
                return FALSE;
            }
            ClientSocket->WaitForHeartbeatReplyState(FALSE); ///< Set to waiting for reply state.
        }
    }
    return TRUE;
}
//---------------------------------------------------------------------------
bool MultiplexingServer::TriggerForceHeartbeat( Int32 Index, PClientSocket* ClientSocketPtr )
{
    HeartbeatType checkHeartbeatType = ClientSocketPtr->GetCheckHeartbeatType();
    if( ( checkHeartbeatType == htCheckRead ) || ( checkHeartbeatType == htCheckBoth ) )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Force Read heartbeat, trigger OnHeartbeat. FD:%d ", ClientSocketPtr->GetHandle() );
        if (FServerListener->OnHeartbeat(ClientSocketPtr, TRUE) == FALSE)
        {
            BufferedLog::Printf(" TriggerForceHeartbeat return false, Remove connection. FD:%d", ClientSocketPtr->GetHandle());
            RemoveConnection(ClientSocketPtr); ///< Send heartbeat failed, remove this connection.
            return FALSE;
        }
        ClientSocketPtr->WaitForHeartbeatReplyState(TRUE); ///< Set to waiting for reply state.
    }
    
    if( ( checkHeartbeatType == htCheckWrite ) || ( checkHeartbeatType == htCheckBoth ) )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Force Write heartbeat, trigger OnHeartbeat. FD:%d ", ClientSocketPtr->GetHandle() );
        if (FServerListener->OnHeartbeat(ClientSocketPtr, FALSE) == FALSE)
        {
            BufferedLog::Printf(" TriggerHeartbeat return false, Remove connection. FD:%d",  ClientSocketPtr->GetHandle());                    
            RemoveConnection(ClientSocketPtr); ///< Send heartbeat failed, remove this connection.
            return FALSE;
        }
        ClientSocketPtr->WaitForHeartbeatReplyState(FALSE); ///< Set to waiting for reply state.
    }
    return TRUE;
}  //MultiplexingServer::TriggerForceHeartbeat()
//---------------------------------------------------------------------------
void MultiplexingServer::CheckHeartbeat( void )
{
    ///< Modify by Simon 2010/1/8
    ///< Check connections in FAllClients original check FIdleList
    register PClientSocket* clientSocket;
    Int32        Count = FAllClients.ItemCount() - 1;     
    PLockObject  Lock( FClientListCS );
    
    for( register Int32 i = Count; i >= 0; i-- )
    {
        if( ( clientSocket = (PClientSocket*)FAllClients.GetItem( i ) ) != NULL )
        {
            if( TriggerHeartbeat( i, clientSocket ) == FALSE )
                return;
        }
    }
}
//---------------------------------------------------------------------------
int MultiplexingServer::ForceSendHeartbeat( void )
{   ///< Add by Zhen 2024/04/17
    ///< Force connections Send Heart in FAllClients original check FIdleList
    register PClientSocket* curClientSocketPtr = 0;
    Int32        socketCount = FAllClients.ItemCount() - 1;     
    int          sentCount = 0;
    PLockObject  Lock( FClientListCS );
    
    for( register Int32 i = socketCount; i >= 0; i-- )
    {
        if( ( curClientSocketPtr = (PClientSocket*)FAllClients.GetItem( i ) ) != NULL )
        {
            if( TriggerForceHeartbeat( i, curClientSocketPtr ) == TRUE )
                sentCount++;
        }
    }
    return sentCount;
}  //MultiplexingServer::ForceSendHeartbeat()
//---------------------------------------------------------------------------
PClientSocket*  MultiplexingServer::GetConnection( int i )
{
    if( i >=0 && i <  FAllClients.ItemCount() )    
        return (PClientSocket*)FAllClients.GetItem( i );
    return NULL;
}    
//---------------------------------------------------------------------------
void MultiplexingServer::EnableHeartbeat( BOOL Enable )
{
    FEnableHeartbeat = Enable;
    FSocketMonitor->EnableHeartbeat(Enable);
}
//---------------------------------------------------------------------------
#ifdef __USE_EPOLL   ///< Use linux epoll    

void MultiplexingServer::PerformEpoll( void )
{    
    const int MAXEVENTS = 256;
    int SelectCount;
    struct epoll_event events[ MAXEVENTS ];

    SelectCount = epoll_wait ( FEPollFD, events, MAXEVENTS, 1 );
    if( SelectCount < 0 )       ///< Call system call "select" failed.
    {
        Int32 ErrNO = errno;
        if( ErrNO == EBADF ) ///< One or more of the socket descriptor sets specified a
            return;       ///  socket descriptor that is not a valid socket descriptor.    
        BufferedLog::Printf( " ###########################################################");
        BufferedLog::Printf( "  MultiplexingServer epoll_wait failed. Code[%d][%s]", ErrNO, strerror( ErrNO )   );
        BufferedLog::Printf( " ###########################################################");
    }
    else if( SelectCount == 0 )  ///< No data comming.
    {
        FCount ++;
        if( FServerListener != NULL &&  FCount % SERVER_IDLE_TIME_MS == 0 )
            FServerListener->OnServerIdle( this );
    }
    else ///< Data comming.
    {    
        for( register int i = 0; i < SelectCount; i++)
        {
              if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))
              {
                  /* An error has occured on this fd, or the socket is not
                     ready for reading (why were we notified then?) */
                  RemoveConnection( (PClientSocket*) events[i].data.ptr );	      
              }
              else
              {
                 PClientSocket* ClientSocket =  (PClientSocket*) events[i].data.ptr; 
                 if( FThreadPool == NULL ) ///< Is a single thread socket server.
                     DataArrived( ClientSocket );
                // Remove the connection from the idle list to prevent
                // to trigger the OnDataArrived event more than one time.
                RemoveFromIdleList( ClientSocket );
                // Thread pool will let a thread to perform the task.
                // Return false means that no more thread available. add it to job queue to wait.
                if( FThreadPool != NULL ) 
                    FThreadPool->ExecuteTask( this, ClientSocket );                
              }
        }
    }    
}
#else ///< Use select
//------------------------------------------------------------------------------
// Prepare the FD set for select and call the "select" system call
//------------------------------------------------------------------------------
void MultiplexingServer::PrepareSelectSetAndSelect( void )
{
    register PClientSocket* ClientSocket;
    register Int32          MaxFD = 0;
    register Int32          CurrentFD;

    FD_ZERO( &FReadSet );   ///< Reset the read set.
    FClientListCS.Enter();  ///< Enter critical section to prevent adding the invalidate FD to the set.
    for( register Int32 i = FIdleList.ItemCount() -1; i >= 0 ; i-- )
    {
        if( ( ClientSocket = FIdleList.GetItem( i ) ) != NULL )
        {
           if( (CurrentFD = ClientSocket->GetHandle()) != -1 )
           {
			   FD_SET( (u_int)CurrentFD, & FReadSet );
               if( CurrentFD > MaxFD )
                       MaxFD = CurrentFD;
           }
        }
    }
    FClientListCS.Leave();

    if( MaxFD == 0 ) ///< No client connected.
    {
        FSelectCount = 0;
        FAddToidleListEvent.WaitFor( 1 );///< Sleep FIdleTime interval.
    }
    else ///< Perform the select system call.
    {
        struct timeval SelectTime = FIdleTime;
        FSelectCount = select( MaxFD + 1, &FReadSet, NULL, NULL, &SelectTime );
    }
}
//------------------------------------------------------------------------------
Int16 MultiplexingServer::PerformSelect( void )
{
    PrepareSelectSetAndSelect( );///< Perform system call 'select'
    if( FSelectCount < 0 )       ///< Call system call "select" failed.
    {
    #ifdef _WIN32
        Int32 ErrNO = WSAGetLastError();
        if( ErrNO == WSAENOTSOCK ) ///< One of the descriptor sets contains
            return -1;             ///  an entry that is not a socket.
    #else
        Int32 ErrNO = errno;
        if( ErrNO == EBADF ) ///< One or more of the socket descriptor sets specified a
            return -1;       ///  socket descriptor that is not a valid socket descriptor.
                             ///  return FALSE to select again.
    #endif
        BufferedLog::Printf( " ###########################################################");
        BufferedLog::Printf( "      MultiplexingServer select failed. Errno: %d.", ErrNO   );
        BufferedLog::Printf( " ###########################################################");
        RemoveErrorSocket( );
        return -1;
    }
    else if( FSelectCount == 0 )  ///< No data comming.
    {
        FCount ++;
        if( FServerListener != NULL && 
            FCount%SERVER_IDLE_TIME_MS == 0 &&
            FAllClients.ItemCount() == FIdleList.ItemCount() )
            FServerListener->OnServerIdle( this );
        return 0;
    }
    return 1;
}
//------------------------------------------------------------------------------
// Check and remove the bad connection
//------------------------------------------------------------------------------
void MultiplexingServer::RemoveErrorSocket( void )
{
    PClientSocket* ClientSocket = NULL;
    FClientListCS.Enter();
    Int32 Count = FIdleList.ItemCount() -1;
    FClientListCS.Leave();

    for( Int32 i = Count; i >= 0 ; i-- )
    {
        try
        {
            if( (ClientSocket = (PClientSocket*)FIdleList.GetItem( i )) != NULL )
                 ClientSocket->CheckConnection( 100000 ); ///< Check the socket connection.
        }
        catch( ... )
        {
            BufferedLog::DebugPrintf(" ### RemoveErrorSocket ### - RemoveConnection");
            RemoveConnection( ClientSocket ); ///< Exception! remove it.
        }
    }
}
//---------------------------------------------------------------------------
void MultiplexingServer::CheckDataArrive( void )
{
    PClientSocket* clientSocket;
    ///< Modify by Simon 2009/1/22
    PLockObject  Lock( FClientListCS );
    Int32 Count = FIdleList.ItemCount() - 1;

    for( Int32 i = Count; i >= 0; i-- )
    {
        if( ( clientSocket = (PClientSocket*)FIdleList.GetItem( i ) ) != NULL )
        {
            if (clientSocket->GetHandle() == -1) continue;
            if( TriggerDataArrived( i, clientSocket ) == FALSE )
                return;
            if( FSelectCount <= 0 )
                return;
        } 
    } 
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
// Implement PThread Execute function
// This thread is the Multiplexing thread.(select thread)
//---------------------------------------------------------------------------
void MultiplexingServer::Execute( void )
{
    while ( IsTerminated() != TRUE )
    {
        if ( FIsRunning == TRUE )
        {            
            try
            {
            #ifdef __USE_EPOLL     
                PerformEpoll();
            #else
                Int16 SelectResult;
                
                SelectResult = PerformSelect(); ///< Perform select to check any data arrived. 
                if( SelectResult == 1 ) ///< Data arrived.
                    CheckDataArrive();
            #endif
                if( FEnableHeartbeat == TRUE ) ///< Heartbeat function enabled.
                {
                    UInt32 TickNow = UFC::GetTickCountMS( );
                    if( TickNow > FLastCheckHeartbeatTick )
                    {
                        if( ( TickNow - FLastCheckHeartbeatTick ) >= 1000 )
                        {
                            FLastCheckHeartbeatTick = TickNow;
                            CheckHeartbeat();
                        }
                    }
                    else
                    {
//                        if( TickNow < FLastCheckHeartbeatTick )
//                            BufferedLog::Printf( " MultiplexingServer::%s() Tick Now[%u] < LastCheckHeartbeat[%u]", __func__, TickNow, FLastCheckHeartbeatTick );
                        FLastCheckHeartbeatTick = TickNow;
                    }
                }
            }
            catch( SocketException & e )
            {
                BufferedLog::Printf( " ### MultiplexingServer Socket Exception ###");
                BufferedLog::Printf( " Exception:%s", e.what() );
            }
            catch( ... )
            {
                BufferedLog::Printf( " ######################################################");
                BufferedLog::Printf( " ### Unknown exception! MultiplexingServer shutdown ###");
                BufferedLog::Printf( " ######################################################");
                FIsRunning = FALSE;
                FServerSocket->Stop( );
                ClearAllClients();
            }
        }
        else
            UFC::SleepMS( 1000 );
    }
}
//---------------------------------------------------------------------------
// Start running the server.
//---------------------------------------------------------------------------
void MultiplexingServer::Run( void )
{
    if( FIsRunning == FALSE )
    {
        try
        {   ///< Setup IP,Port and max connections.
            FServerSocket->SetIPAddress( FIPAddress );
            FServerSocket->SetPort( FPort );
            FServerSocket->SetMaxConnection( FMaxConnection );
            FServerSocket->Listen();  ///< Start the server listen socket.
            if( FServerListener != NULL )
                FServerListener->OnListen( this ); ///< Trigger OnListen event.
            Start();    ///< Start the server thread.
            FSocketMonitor->Run();
            FIsRunning = TRUE;
        }
        catch( SocketException & e )
        {
            BufferedLog::Printf( " MultiplexingServer exception:%s", e.what() );
            FSocketMonitor->Stop();
            FServerSocket->Stop();
            FIsRunning = FALSE;            
        }
    }
}
//---------------------------------------------------------------------------
// Stop the server.
//---------------------------------------------------------------------------
void MultiplexingServer::Stop( void )
{
    if( FIsRunning != FALSE )
    {
        FSocketMonitor->Stop();
        FIsRunning = FALSE;
        if( IsTerminated() == FALSE )
        {
            Terminate();
            WaitFor();
        }
        FServerSocket->Stop();
        ClearAllClients();
    }
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

