//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _TWSE_SOCKET_CONNECTION_H
#define _TWSE_SOCKET_CONNECTION_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _WIN32
    #include <winsock.h>
#else
    #include <sys/socket.h>		/* basic socket definitions */
    #include <netinet/in.h>		/* sockaddr_in{} and other Internet defns */
    #include <netinet/tcp.h>
    #include <netdb.h>
    #include <arpa/inet.h>		/* inet(3) functions */
    #include <fcntl.h>
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Connection.h"
#include "../UFC/PSocket.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define MAX_TCP_PACKET 4096
#define TWSE_MAX_DATA_SIZE  8192    
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SocketData
{
public:
    UInt8 RowData[MAX_TCP_PACKET];
    Int16 Length;
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SocketConnection : public Connection, public UFC::PThread
{
private:
    static int                FSocketBufferSize;    
private:
    UFC::PtrQueue<SocketData> FReadQueue;
    UFC::PSocket              FSocket;
    Int8*                     FSendBuffer;
    Int32                     FSendHeartbeat;
    Int32                     FRecvHeartbeat;
    BOOL                      FIsExchange;
    UFC::PCriticalSection     FSocketIOLock;
private:
    void SendHeartbeat( void );
    void SendAcceptAck( void ); ///< for Server
    BOOL ReadyToRead( long TimeOut, long us = 0 );
    void Execute( void );
    int  ReadData( long TimeOutSec );
    int  ProcessHeartbeat( SocketData* Data );
    int  ProcessAcceptMessage( SocketData* Data );
    int  ProcessTMPMessage( SocketData* Data, Int16 DataLength );
    void SocketClose( BOOL ThrowException );
public:
    SocketConnection( );
    virtual ~SocketConnection( );
    virtual void Open( int FD );
    virtual void Open( int Port, int PVCID, char* IP, int LocalPort );
    virtual void Close( void );
    virtual int Send( char* Data, int Length, long TimeOutSec = 5L );
    virtual int Receive( char* Data, int Length, long TimeOutSec = 5L  );
    virtual int GetConnectionID( void ) { return FSocket.GetHandle(); }
public:    
    static void SetTMPBufferSize( int Size );
    static int  GetTMPBufferSize( void );
};
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
