//---------------------------------------------------------------------------
// This is a part of the UNIX Foundation Classes Library for UNIX platforms.

// Copyright (c) 2003 MobileDigital Development Inc.

// All rights reserved.

//

// Description: A server side socket class and a multiplexing server class.

// Author: Frank and Simon

//

// This source code is only intended as a supplement to the

// Object Class Library for UNIX platforms Reference and related

// electronic documentation provided with the library.

// See these sources for detailed information regarding the

// UNIX Foundation Classes Library for UNIX platforms product.
//---------------------------------------------------------------------------
#ifndef __UFC_SOCKETSERVER_H
#define __UFC_SOCKETSERVER_H
#include <fcntl.h>
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "AnsiString.h"
#include "Exception.h"
#include "List.h"
#include "PThread.h"
#include "Stream.h"
#include "PClientSocket.h"
#include "PSocket.h"

//#ifdef __LINUX
//    #include <sys/epoll.h>
//    #define __USE_EPOLL
//#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//	PServerSocket
//
//---------------------------------------------------------------------------
const Int32 MAX_CONNECTION = 128;
//---------------------------------------------------------------------------
class PServerSocket;
//---------------------------------------------------------------------------
class PServerSocketListener
{
public:
    virtual void OnAccept( PServerSocket* ServerSocket ) = 0;
    virtual ~PServerSocketListener( void ){}
};
//---------------------------------------------------------------------------
class PServerSocket : public PSocket, public PThread
{
private:
    PServerSocketListener* FListener;
    AnsiString	           FIPAddress;
    Int32		   FPort;
    Int32		   FMaxConnection;
    BOOL                   FIsRunning;
private:
    int                    FEpollFD;
private:
    fd_set		   FReadSet;
    void                   CheckConnection( struct timeval& SelectTime );
    virtual void           Execute( void );
public:
    PServerSocket( PServerSocketListener* Linstener );
    PServerSocket( PServerSocketListener* Linstener, Int32 Port, Int32 MaxConnections = MAX_CONNECTION );
    PServerSocket( PServerSocketListener* Linstener, const AnsiString & Address, Int32 Port, Int32 MaxConnections = MAX_CONNECTION );
    virtual ~PServerSocket();
    void                Listen( void );
    void                Stop( void );
    PClientSocket*	Accept( void );
    const Int32		GetPort( void )const { return FPort; }
    const AnsiString&	GetIPAddress( void )const { return FIPAddress; }
    void   		SetPort( Int32 Port ){ FPort = Port; }
    void             	SetIPAddress( AnsiString& IP ){ FIPAddress = IP; }
    void                SetMaxConnection( Int32 Max ){ FMaxConnection = Max; }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
} ///< Close namespace UFC
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif // PServerSocket_H
