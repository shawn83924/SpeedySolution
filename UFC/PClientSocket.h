//---------------------------------------------------------------------------
// This is a part of the UNIX Foundation Classes Library for UNIX platforms.
// Copyright (c) 2003 MobileDigital Development Inc.
// All rights reserved.
//
// Description: Client side socket class.
// Author: Frank and Simon
//
// This source code is only intended as a supplement to the
// Object Class Library for UNIX platforms Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// UNIX Foundation Classes Library for UNIX platforms product.
//---------------------------------------------------------------------------
#ifndef __UFC_PCLIENTSOCKET_H
#define __UFC_PCLIENTSOCKET_H
//------------------------------------------------------------------------------
#include <fcntl.h>
#include <queue>
//------------------------------------------------------------------------------
#include "AnsiString.h"
#include "Exception.h"
#include "List.h"
#include "PThread.h"
#include "Stream.h"
#include "PSocket.h"
//------------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
class PClientSocket;
//------------------------------------------------------------------------------
class SocketClientListener
{
public:
    virtual ~SocketClientListener() {}
    virtual void OnConnect( PClientSocket * Socket ) = 0;
    virtual void OnDisconnect( PClientSocket * Socket, BOOL NeedReconnect = FALSE ) = 0;
    virtual BOOL OnDataArrived( PClientSocket * Socket ) = 0;
    virtual void OnIdle( PClientSocket * Socket ) = 0;
};
//------------------------------------------------------------------------------
class PClientSocket : public PSocket, public PThread
{
private:
    typedef std::allocator< std::string >            STRING_ALLOCATOR;
    typedef std::deque<std::string, STRING_ALLOCATOR> QUEUE;
protected:
    AnsiString              FSocketIPAddress; ///< Store the local IP.
    AnsiString              FIPAddress;       ///< Store the peer IP address.
    Int32                   FPort;            ///< Connection port.
    volatile BOOL           FIsConnected;
    volatile BOOL           FBusyCheck;
    SocketClientListener*   FListener;
private:
    Int64                   FData;
    QUEUE                   FWriteQueue;
    bool                    TryPopFront( std::string& Data );
private:
    void                    Execute( void );
    void                    UpdateLocalIPAddress();
    void                    UpdatePeerIPAddress();
    BOOL                    CheckDataArrived( struct timeval& SelectTime );
    BOOL                    CheckDataArrivedBusy( void );
    void                    Process( int& Count );
    void                    ProcessBusy( int& Count );
public:
    ///< Create a new ClientSocket.
    PClientSocket( );
    PClientSocket( const AnsiString& Address, Int32 Port, BOOL ThreadMode = TRUE );
    ///< Create a ClientSocket from existing socket.
    PClientSocket( Int32 FD );
    virtual ~PClientSocket( void );
    void    SendQueue( const UFCType::Int8* SendData, UFCType::Int32 Size );
    void    SendQueue( const std::string& SendData );
    Int32   ProcessQueue( void );
    void    ClearQueue( void );
    BOOL    IsQueueEmpty( void );
public:
    void                Connect( const AnsiString& BindAddress, Int32 BindPort, const AnsiString& TargetAddress, Int32 TargetPort, const Int32 TimoOutSec = 10 );
    void                Connect( const AnsiString& HostAddress, Int32 Port, const Int32 Sec = 10 );
    void                Connect( const Int32 Sec = 10 );
    void                Disconnect( BOOL TriggerOnDisconnectEvent = TRUE, BOOL NeedReconnect = FALSE );
    void                SetListener( SocketClientListener* Listener ) { FListener = Listener; }
    const BOOL          IsConnect( void );
    const Int32         GetPort( void ) const       { return FPort; }
    void                SetPort( int NewPort )       { FPort = NewPort; }
    void                SetHost( const AnsiString& Host ) { FIPAddress = Host; }
    ///< Functions to get peer IP and ID.
    const Int32         GetPeerID( void );
    const Int32         GetPeerPort( void );
    const AnsiString&   GetPeerIPAddress( void ) const  { return FIPAddress; }
    ///< Functions to get local IP and ID.
    const Int32         GetSocketID( void );
    const Int32         GetSocketPort( void );
    const AnsiString&   GetSocketIPAddress( void ) const { return FSocketIPAddress; }
    void                UpdateIPAddress( const UFC::AnsiString& LocalIP, const UFC::AnsiString& PeerIP );
    ///< Function to Get/Set user data.
    Int64               GetUserData()               { return FData; }
    void                SetUserData( Int64 Data )   { FData = Data; }
    void                SetThreadAffinity( int CPUID );
    void                SetBusyloopCheck( bool IsBusy );
};
//------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
#endif // PClientSocket_H
//------------------------------------------------------------------------------
