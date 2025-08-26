//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// This is a part of the UNIX Foundation Classes Library for UNIX platforms.
// Copyright (c) 2003 MobileDigital Development Inc.
// All rights reserved.
//
// Description: Multiplexing Server Class.
//              implement a multiplexing socket server
//              with thread pool.
// Author: Simon Chang
//
// This source code is only intended as a supplement to the
// Object Class Library for UNIX platforms Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// UNIX Foundation Classes Library for UNIX platforms product.
//-----------------------------------------------------------------------------------------
#ifndef __UFC_MultiplexingServer_H
#define __UFC_MultiplexingServer_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "AnsiString.h"
#include "Exception.h"
#include "List.h"
#include "PThread.h"
#include "Stream.h"
#include "PSocket.h"
#include "PClientSocket.h"
#include "PServerSocket.h"
#include "ThreadPool.h"
#include "SocketHeartBeat.h"
#ifndef _WIN32
    #include <poll.h>
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Thread Idle threshold
//
//  if a connection is idle more than THREAD_IDLE_TIME_MS ms
//  The thread pool will take back this thread.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const Int32 THREAD_IDLE_TIME_MS = 16000;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//	MultiplexingServer
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MultiplexingServer;
class SocketServerListener
{
public:
    virtual ~SocketServerListener() {}
    virtual void OnListen( MultiplexingServer * ServSocket ) = 0;
    virtual void OnClientConnect( MultiplexingServer * ServSocket, PClientSocket * ClientSocket ) = 0;
    virtual void OnClientDisconnect( MultiplexingServer * ServSocket, PClientSocket * ClientSocket ) = 0;
    virtual BOOL OnClientWrite( MultiplexingServer * ServSocket, PClientSocket * ClientSocket ) = 0;
    virtual void OnServerIdle( MultiplexingServer * ServSocket ) = 0;
    virtual BOOL OnHeartbeat( PClientSocket* pSocket, BOOL IsRead  ) = 0;
    virtual void OnAcceptFailed( MultiplexingServer * ServSocket, PClientSocket * ClientSocket ) { return; };
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum pollresult
{
    prContinue,
    prBreak,
    prError,
    prDataArrived
            
}PollResult;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MultiplexingServer : public PThread, public PTaskObject, public PServerSocketListener
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Server Idle time
//
// if no incoming data during SERVER_IDLE_TIME_MS period
// OnIdle event will be triggered.
// This time is also the timeout time for server "select" function.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
private:    
    Int32 SERVER_IDLE_TIME_MS;    
private:
    SocketIoMonitor*        FSocketMonitor;
    SocketServerListener*   FServerListener;
    PtrList<PClientSocket>  FAllClients;    
    PCriticalSection        FClientListCS;
    PServerSocket*	    FServerSocket;    
    struct timeval	    FIdleTime;
    PThreadPool*            FThreadPool;
    AnsiString		    FIPAddress;
    BOOL		    FIsRunning;
    Int32		    FPort;
    Int32		    FMaxConnection;
    Int32                   FThreadCount;
    Int32                   FThreadIdleTimeMS;
    Int32                   FCount;
private:
    fd_set		    FReadSet;
    Int32                   FSelectCount;
    Int32                   FProcessedMessageCount;
    UInt32                  FLastCheckHeartbeatTick;
    BOOL                    FEnableHeartbeat;    
    BOOL                    FUseBusyLoop;
#ifdef __USE_EPOLL       
    int                     FEPollFD;  
    void PerformEpoll( void );
#else
    PEvent                  FAddToidleListEvent;
    PtrList<PClientSocket>  FIdleList;
    Int16 PerformSelect( void );    
    void PrepareSelectSetAndSelect( void );
    void RemoveErrorSocket( void ); ///< If select function return error code, remove the error connection from the select list.
#endif    
private:
    /**
     *  Add a PClientSocket object to the busy list.
     */
    void AddToIdleList( PClientSocket* NewFD );
    /**
     *  Remove a PClientSocket object from the busy list.
     */
    void RemoveFromIdleList( PClientSocket* DelClient );
    /**
     *  Add a TCP/IP socket connection. (PClientSocket object)
     */
    void AddConnection( PClientSocket* NewClientSocket );
    /**
     *  Prepare the select set and call the select function.
     */
    
    /**
     *  Check the select set to detect whether the data is comming.
     */
    void CheckDataArrive( void );
    void CheckHeartbeat( void );
    int ForceSendHeartbeat( void );
    BOOL DataArrived( PClientSocket* ClientSocket );
    BOOL CheckExceptionSet( Int32 Index, PClientSocket* ClientSocket ); ///< Check any exception in this connection.
    BOOL TriggerDataArrived( Int32 Index, PClientSocket* ClientSocket );///< If data is comming, trigger the OnDataArrived event.
    BOOL TriggerHeartbeat( Int32 Index, PClientSocket* ClientSocket );
    bool TriggerForceHeartbeat( Int32 Index, PClientSocket* ClientSocketPtr );
    /**
     *  Broken all connections and delete all the PClientSocket objects.
     */
    void ClearAllClients( void );
private:
    /**
     * Implement the PThread Execute virtual function.
     */
    virtual void Execute( void );
    /**
     * Implement the PTaskObject interface.
     */
    virtual Int32 DoTask( void* Param );
    /**
     * Implement the PServerSocketListener interface.
     */
    virtual void OnAccept( PServerSocket* ServerSocket );
    PollResult PollCheckData( struct pollfd& PollFD );
    PollResult BusyCheckData( int FD );
public: ///< Functions to control the server.
    void		Run( void );
    void		Stop( void );
    void                EnableHeartbeat( BOOL Enable );
    void		SetListener( SocketServerListener * Listener ) { FServerListener = Listener; }
    void                ToTrashcan( PClientSocket* DeleteSocket, int DelaySec = 3600 );
    /**
     *  Remove a TCP/IP socket connection. (PClientSocket object)
     */
    void                RemoveConnection( PClientSocket* DelClient, Int32 DelTime = 3600 );
    BOOL                IsRunning() {return FIsRunning;}
public: ///< Functions to access the server properties.
    const AnsiString&	GetIPAddress( void ) const  { return FServerSocket->GetIPAddress(); }
    const Int32		GetHandle( void )           { return FServerSocket->GetHandle(); }
    const Int32		GetPort( void ) const       { return FServerSocket->GetPort(); }
    const Int32		GetConnectionCount( void )  { return FAllClients.ItemCount(); }    
    PClientSocket*      GetConnection( int i );
    void		SetIdelInterval( Int32 ms ) { SERVER_IDLE_TIME_MS = ms; }
public: ///< Functions to moniter the server performance.
    const Int32		GetMessageCount() const     { return FProcessedMessageCount; }
    const Int32         AvailableThread( void )     { return ( FThreadPool == NULL )? 1 : FThreadPool->AvailableThread(); }
public: ///< Constrator and destroctor.
    MultiplexingServer( Int32 ListenPort, 
                        Int32 MaxConnection = MAX_CONNECTION, 
                        Int32 ThrCount = 15,
                        Int32 SwitchTimeMS = THREAD_IDLE_TIME_MS,
                        const UFC::AnsiString& Rule = "Def",
                        bool Busy = false );
    MultiplexingServer( const AnsiString & Address, 
                        Int32 ListenPort, 
                        Int32 MaxConnection = MAX_CONNECTION, 
                        Int32 ThrCount = 15,
                        Int32 SwitchTimeMS = THREAD_IDLE_TIME_MS,
                        const UFC::AnsiString& Rule = "Def",
                        bool Busy = false );
    virtual ~MultiplexingServer( void );
    Int32   GetThreadSwitchTimeMS( )         { return FThreadIdleTimeMS; }
    void    SetThreadSwitchTimeMS( int ms )  { FThreadIdleTimeMS = ms;  }

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
} ///< Close namespace UFC
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
