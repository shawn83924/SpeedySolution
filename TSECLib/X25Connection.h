#ifndef _X25_CONNECTION_H
#define _X25_CONNECTION_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Connection.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef __AIX ///< Works on IBM AIX
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Empty implement
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class X25Connection : public Connection
{
private:
        int FX25FD;
public:
        X25Connection( ){}
        virtual ~X25Connection( ){}
        virtual void Open( int ) {}
        virtual void Open( int LineNo, int Pvc, char* IP = NULL, int LocalPort = -1 ){}
        virtual void Close( void ){}
        virtual int Send( char* Data, int Length, long TimeOutSec = 5L ){ return 0;}
        virtual int Receive( char* Data, int Length, long TimeOutSec = 5L  ){ return 0; }
	virtual int GetConnectionID( void ) { return FX25FD; }
        static  void CloseX25( void ) {}
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*#define NPI_20 ///< Force use NPI 2.0
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stream.h>
#include <sys/stropts.h>

#include <sys/twtypes.h>
#include <sys/twlib.h>
#include <sys/npi_20.h>
#include <sys/npiapi.h>
#include <sys/pktintf.h>
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class X25Connection : public Connection
{
private:
        int    FX25FD;
        int    FToken;
        fd_set FPutfd;
	fd_set FGetfd;                        
        struct timeval FTimeout;
        char*  FDataBuffer; /// Buffer size = PVC_MAX_PACKET_SIZE
        char*  FCtrlBlock;
        int    FPVC;
        int    FPort;        
private:
        BOOL ReadyToWrite( long timeout, long us = 0 );///< OK
        BOOL ReadyToRead( long timeout, long us = 0 ); ///< OK
        int  WritPVCData( struct strbuf *Control, struct strbuf *Data, long timeout = 5L  ); ///< OK
        int  ReadPVCData( struct strbuf *Control, struct strbuf *Data, long timeout = 5L  ); ///< OK
        int  CheckPVCForWrite( void );  ///< Ok
        int  CheckPVCAndRead( struct strbuf *Control, struct strbuf *Data, BOOL& MoreData, long timeout = 5L  ); ///< Ok
        int  CheckCTLCommand( struct strbuf *Control, BOOL& MoreData ); ///< Ok
        // private functions for Open PVC
        void OpenStrem( void );                     ///< OK
        void SendAttachMessage( int Line, int Pvc );///< Ok
        BOOL GetAttachRespsonseMessage( void );     ///< Ok
        int  SendResetConfirm( void );              ///< Ok
        // private functions for Close PVC
	void SendDetachMessage( void );             ///< OK
	void GetDetachRespsonseMessage( void );     ///< Ok
public:
	X25Connection( );						    ///< OK
	virtual ~X25Connection( );				    ///< OK
        virtual void Open( int ) {}
        virtual void Open( int Port, int PVC, char* IP = NULL, int LocalPort = -1 );        	
	virtual void Close( void );                 ///< Ok
	virtual int Send( char* Data, int Length, long TimeOutSec = 5L );     ///< Ok
	virtual int Receive( char* Data, int Length, long TimeOutSec = 5L  );
	virtual int GetConnectionID( void ) { return FX25FD; }                ///<OK
	static  void CloseX25( void ) {}
};*/
#endif
#ifdef __SOLARIS
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Works on Solaris
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <netx25/uint.h>
#include <netx25/x25_proto.h>
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class X25Connection : public Connection
{
private:
	int    FX25FD;
        fd_set FPutfd;
	fd_set FGetfd;                                
	struct timeval FTimeout;
	char*  FDataBuffer;//PVC_MAX_PACKET_SIZE
	char*  FCtrlBlock;//PVC_MAX_CONTROLBUFFER
        int    FPVC;
        int    FPort;                
private:
	BOOL ReadyToWrite( long timeout, long us = 0 );
	BOOL ReadyToRead( long timeout, long us = 0 );
	int  WritPVCData( struct strbuf *Control, struct strbuf *Data, long timeout = 5L  );
	int  ReadPVCData( struct strbuf *Control, struct strbuf *Data, long timeout = 5L  );
	int  CheckPVCForWrite( void );
	int  CheckPVCAndRead( struct strbuf *Control, struct strbuf *Data,long timeout = 5L  );
	int  SendExpeditedDataAck( void );
	int  CheckCTLCommand( S_X25_HDR*  X25Result );
	// private functions for Open PVC
	void OpenStrem( void );
	void SendAttachMessage( int Line, int Pvc );
	BOOL GetAttachRespsonseMessage( void );
	int  SendResetConfirm( void );
	// private functions for Close PVC
	void SendDetachMessage( void );
	void GetDetachRespsonseMessage( void );
public:
	X25Connection( );
	virtual ~X25Connection( );
        virtual void Open( int ) {}
	virtual void Open( int LineNo, int Pvc, char* IP = NULL, int LocalPort = -1   );
	virtual void Close( void );
	virtual int Send( char* Data, int Length, long TimeOutSec = 5L );
	virtual int Receive( char* Data, int Length, long TimeOutSec = 5L  );
	virtual int GetConnectionID( void ) { return FX25FD; }
	static  void CloseX25( void ) {}
};
#endif
#ifdef _WIN32
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Works on Windows
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*#include <x25/x25.h>
#include <x25/neterr.h>
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WinX25Lib
{
public:
	WinX25Lib();
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class X25Connection : public Connection
{
private:
	static WinX25Lib FX25Lib;
	int  FX25FD;
	char OSDTPTR FSendBuff;	///< Send Buffer Pointer
	char OSDTPTR FRecvBuff;	///< Receive Buffer Pointer
private:
	int WritPVCData( char*Data, int Len, int info, long timeout );
	int ReadPVCData( char*Data, int& Len,BOOL& More, long timeout );
public:
	X25Connection( );
	virtual ~X25Connection( );
        virtual void Open( int ) {}
	virtual void Open( int LineNo, int Pvc, char* IP = NULL, int LocalPort = -1   );
	virtual void Close( void );
	virtual int Send( char* Data, int Length, long TimeOutSec = 5L );
	virtual int Receive( char* Data, int Length, long TimeOutSec = 5L  );
	virtual int GetConnectionID( void ) { return FX25FD; }
	static  void CloseX25( void );
};*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Empty implement
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class X25Connection : public Connection
{
private:
        int FX25FD;
public:
        X25Connection( ){}
        virtual ~X25Connection( ){}
        virtual void Open( int ) {}
        virtual void Open( int LineNo, int Pvc, char* IP = NULL, int LocalPort = -1 ){}
        virtual void Close( void ){}
        virtual int Send( char* Data, int Length, long TimeOutSec = 5L ){ return 0;}
        virtual int Receive( char* Data, int Length, long TimeOutSec = 5L  ){ return 0; }
	virtual int GetConnectionID( void ) { return FX25FD; }
        static  void CloseX25( void ) {}
};
#endif
#ifdef __LINUX
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Empty implement
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class X25Connection : public TWSE::Connection
{
private:
        int FX25FD;
public:
        X25Connection( ){}
        virtual ~X25Connection( ){}
        virtual void Open( int ) {}
        virtual void Open( int LineNo, int Pvc, char* IP = NULL, int LocalPort = -1 ){}
        virtual void Close( void ){}
        virtual int Send( char* Data, int Length, long TimeOutSec = 5L ){ return 0;}
        virtual int Receive( char* Data, int Length, long TimeOutSec = 5L  ){ return 0; }
	virtual int GetConnectionID( void ) { return FX25FD; }
        static  void CloseX25( void ) {}
};
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
