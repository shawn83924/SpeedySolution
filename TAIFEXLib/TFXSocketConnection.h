// 
// File:   SocketConnection.h
// Author: yuan
//
// Created on June 28, 2008, 1:13 AM
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _SOCKETCONNECTION_H
#define	_SOCKETCONNECTION_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "TFXConnection.h"
#include "TMPDefines.h"
//-----------------------------------------------------------------------------------------
namespace TFX
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SocketConnection : public Connection
{
private:
    static int                FSocketBufferSize;
private:    
    UFC::PSocket              FSocket;
    Int8*                     FSendBuffer;
    BOOL                      FIsExchange;
    UFC::PCriticalSection     FSocketIOLock;
private:
    BOOL ReadyToRead( long TimeOut, long us = 0 );
    void SocketClose( BOOL ThrowException );
public:
    SocketConnection( void );
    virtual ~SocketConnection( void );
    virtual void Open( int Port, int PVCID, char* URL );
    virtual void Open( int FD );    
    virtual void Close( void );
    virtual int Send( char* Data, int Length, long TimeOutSec = 5L );
    virtual int Receive( char* Data, int Length, long TimeOutSec = 5L  );
    virtual int GetConnectionID( void ) { return FSocket.GetHandle(); }
    static void SetTMPBufferSize( int Size );
    static int  GetTMPBufferSize( void );
};
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif	/* _SOCKETCONNECTION_H */
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

