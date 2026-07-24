//---------------------------------------------------------------------------
// This is a part of the UNIX Foundation Classes Library for UNIX platforms.
// Copyright (c) 2003 MobileDigital Development Inc.
// All rights reserved.
//
// Description: Socket Manipulating Class.
// Author: Frank and Simon
//
// This source code is only intended as a supplement to the
// Object Class Library for UNIX platforms Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// UNIX Foundation Classes Library for UNIX platforms product.
//---------------------------------------------------------------------------
#ifndef __UFC_PSOCKET_H
#define __UFC_PSOCKET_H
//---------------------------------------------------------------------------
#include "Exception.h"
#include "List.h"
#include "PThread.h"

#ifdef _WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
    #include <sys/socket.h>		/* basic socket definitions */
	#include <netinet/in.h>		/* sockaddr_in{} and other Internet defns */
	#include <netinet/tcp.h>
    #include <netdb.h>
    #include <arpa/inet.h>		/* inet(3) functions */
    #include <fcntl.h>    
#endif

#ifdef __SOLARIS
        #include <sys/sockio.h>
#endif    

#ifdef __LINUX        
    #include <sys/ioctl.h>        
    #include <net/if.h>        
#endif            
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
#define NONBLOCKING_SOCKET  1
#define BLOCKING_SOCKET     0
#define ANY_LOCAL_PORT     -1
//---------------------------------------------------------------------------
typedef enum
{
   htCheckRead,
   htCheckWrite,
   htCheckBoth
   
}HeartbeatType;
//---------------------------------------------------------------------------
class PSocket
{
private:
        static Int32 GHeartBeatTime;
        static Int32 GHeartBeatTimeout;
        static Int32 GRecycleTime;
protected: /// Member variables.
        Int32         FFD;           ///< Socket file descripter.
        Int32         FSocketType;   ///< TCP: SOCK_STREAM  UDP: SOCK_DGRAM
        Int32         FBlockMode;    ///<
        Int32         FHostID;
        Int32         FWriteTimeout;        
private:
        Int16         FHeartbeatInterval;
        Int16         FTimeout;
        Int16         FTimerCountRead;
        Int16         FTimerCountWrite;
        Int16         FDeleteCounter;
        HeartbeatType FCheckHeartbeat;
        void          ReadyToWrite( void );
private: ///< Opt-in user-space receive buffer. One large recv serves many BlockRecv calls.
        UInt8*        FRxBuffer;
        Int32         FRxCapacity;
        Int32         FRxHead;
        Int32         FRxTail;
public: /// Constructors & destructor
        PSocket( void );
        PSocket( PSocket* Socket );
        PSocket( Int32 SocketType, BOOL BlockMode );
        PSocket( Int32 FD, Int32 SocketType, Int32 BlockMode );
        virtual ~PSocket( void );
        PCriticalSection   ReadLock;
        PCriticalSection   IOLock;
public: ///< Functions about hearbeat.
        void  SetCheckHeartbeatType( HeartbeatType Type );
        void  HeartbeatTimerCountDown( void );
        void  SetHeartbeatInterval( Int16 TimeInterval, Int16 TimeOut );
        void  ResetHeartbeatTimer( BOOL IsRead );
        void  WaitForHeartbeatReplyState( BOOL IsRead );
        BOOL  NeedSendHeartbeat( BOOL IsRead );
        BOOL  NeedKill( void );
        void  SetDeleteCounter( Int16 Value );
        Int16 DeleteCountDown( void );
        Int32 Receive( UInt8* RecvData, Int32 Size );
public:
        Int32       OpenSocket( void );
        void        CloseSocket( void );
        void        Bind( const AnsiString& Address, UFCType::Int32 Port );
        void        Connect( const AnsiString& Address, const UFCType::Int32 Port, const UFCType::Int32 USec, const UFCType::Int32 LocalPort = ANY_LOCAL_PORT );
        /// Socket options manipulate functions.
        void        SetOption( UFCType::Int32 Level, UFCType::Int32 OptionName, const void* OptionValue, UFCType::Int32 OptionLength );
        void        GetOption( UFCType::Int32 Level, UFCType::Int32 OptionName, UFCType::Int8* OptionValue, UFCType::Int32* OptionLength );
        void        SetReuse( BOOL IsReuse );
        void        SetLinger( BOOL OnOff, int Time );
        void        SetBlockMode( BOOL IsNonBlocking );
        void        SetTCPNoDelay( BOOL IsNoDelay );
        Int32       SetBufferSize( Int32 Option, Int32 Size );
        void        SetBroadcast( Int32 Option );
        void        SetWriteTimeout( Int32 TimeoutMS );
        /// Socket I/O functions.
        Int32       SendBuffer( const UFCType::Int8* SendData, UFCType::Int32 Size );
        Int32       SendTo( const AnsiString& Address, UFCType::Int32 Port, const char* SendData, UFCType::Int32 Size );
        void        BlockSend( UFCType::Int8* SendData, UFCType::Int32 Size );
        Int32       RecvBuffer( UFCType::UInt8* RecvData, UFCType::Int32 Size );
        void        BlockRecv( UFCType::UInt8* RecvData, UFCType::Int32 Size );
        void        EnableRecvBuffer( UFCType::Int32 Size );
        BOOL        HasBufferedRx( void ) { return ( FRxBuffer != NULL && FRxHead < FRxTail ) ? TRUE : FALSE; }
        Int32       ReceiveWithTimeout( UFCType::UInt8* RecvData, UFCType::Int32 Size, UFCType::Int16 TimeOut );
        Int32       RecvBufferWithTimeout( UFCType::UInt8* RecvData, UFCType::Int32 Size, UFCType::Int16 TimeOut );
        void        Purge( void );
        /// Access class member variable functions.
        const Int32 GetHandle( void )     { return FFD; }
        const Int32 GetSocketType( void ) { return FSocketType; }
        const Int32 GetBlockMode( void )  { return FBlockMode; }
        HeartbeatType GetCheckHeartbeatType( void ) { return FCheckHeartbeat; };
        void        CheckConnection( const Int32 TimeUSec );
        BOOL        IsBadSocket( void );
        void        Attach( int FD );
        void        Detach( void );
public:
        static void         SetHeartbeatTime( Int32 Sec )    { GHeartBeatTime = Sec; }
        static void         SetHeartbeatTimeout( Int32 Sec ) { GHeartBeatTimeout = Sec; }
        static AnsiString   GetLocalHostName( void );
        static Int32        GetLocalIPAddress( void );
        static AnsiString   GetLocalIP( void );
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
