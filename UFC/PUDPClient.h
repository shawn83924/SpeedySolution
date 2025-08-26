//---------------------------------------------------------------------------
// This is a part of the UNIX Foundation Classes Library for UNIX platforms.
// Copyright (c) 2003 MobileDigital Development Inc.
// All rights reserved.
//
// Description: A UDP client side socket class.
// Author: Frank and Simon
//
// This source code is only intended as a supplement to the
// Object Class Library for UNIX platforms Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// UNIX Foundation Classes Library for UNIX platforms product.
//---------------------------------------------------------------------------
#ifndef __UFC_PUDPCLIENT_H
#define __UFC_PUDPCLIENT_H
//---------------------------------------------------------------------------
#include "PSocket.h"
#include "PThread.h"
#include "MemoryStream.h"
//---------------------------------------------------------------------------
#define DEFAULT_BUFFER_SIZE 4096
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
class PUDPClient;
class PUDPListener
{
public:
    virtual ~PUDPListener() {}
    virtual void OnDataArrived( UFC::PUDPClient* Socket
                              , UFC::MemoryStream* Stream
                              , const UFC::AnsiString& Address ) = 0;
};
//----------------------------------------------------------------------------
class PUDPClient : public PSocket, public PThread
{
private:
    AnsiString          FAddress;
    Int32               FPort;
    PUDPListener*       FListener;
    Int32               FRecvDataSize;
    struct sockaddr_in  FRecvAddress;
    Int32               FSendBufferSize;
    Int32               FRecvBufferSize;
    Int32               FCheckInterval;
    AnsiString          FMulticastAddress;
    BOOL                FIsBind;
    BOOL                FCopyData;
    UInt8*              FRecvBuffer;
    UFC::MemoryStream   FRecvStream;    
private:
    void                Execute( void );
    Int32               RecvUDPData( MemoryStream*& OutDataStream );
public:
    static const Int32  ANY_PORT;
public:
    PUDPClient( void ); ///< Only for SendTo
    PUDPClient( const AnsiString& Address, Int32 Port = ANY_PORT, BOOL ThreadMode = TRUE, int core = -1 );
    virtual ~PUDPClient();
public:
    const AnsiString&   GetAddress( void ) { return FAddress; }
    const Int32         GetPort( void ) { return FPort; }
    const Int32         GetSendBufferSize( void ) { return FSendBufferSize; }
    const Int32         GetRecvBufferSize( void ) { return FRecvBufferSize; }    
public:
    void                Open( void );
    virtual void        Close( void );
    void                OpenForMulticast( void );
    void                BindForMulticast( const AnsiString& MulticastAddress /* "any" or multi-cast IP address */ );
    void                NewStreamOnDataArrived( BOOL Copy )   { FCopyData = Copy; }
    void                SetListener( PUDPListener* Listener ) { FListener = Listener; }
    Int32               SetSendBufferSize( Int32 Size );
    Int32               SetRecvBufferSize( Int32 Size );
    void                SetCheckDataInterval( Int32 IntervalMS ) { FCheckInterval = IntervalMS; }
    void                SetMulticastTTL( Int32 TTL );
    void                SetMulticastAddress( const AnsiString& Address, Int32 LoopBack = 0 );    
    void                DropMulticastAddress(  const AnsiString& Address  );
public:
    Int32               RecvData( MemoryStream*& OutDataStream  );
    UInt8*              GetBuffer( void ){ return FRecvBuffer; }
    static BOOL         GetInterfaceIP( const AnsiString& Interface, AnsiString& IP );
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
