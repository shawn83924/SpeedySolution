#ifndef _X25_CONNECTION_H
#define _X25_CONNECTION_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Connection.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef __AIX ///< Works on IBM AIX
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _TCP_ONLY
    class X25Connection : public Connection
    {
    public:
            X25Connection( ){}
            virtual ~X25Connection( ){}
            virtual void Open( int LineNo, int Pvc, char*  ){}
            virtual void Open( int /*FD*/  ){}
            virtual void Close( void ){}
            virtual int Send( char* Data, int Length, long TimeOutSec = 5L ){return 0;}
            virtual int Receive( char* Data, int Length, long TimeOutSec = 5L  ){return 0;}
            virtual int GetConnectionID( void ) { return 0; }
            static  void CloseX25( void ) {}
    };
#else
    #define NPI_20 ///< Force use NPI 2.0
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
    #include <sys/twtypes.h>
    #include <sys/twlib.h>
    #include <sys/stropts.h>
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
            virtual void Open( int LineNo, int Pvc, char *  );  ///< Ok
            virtual void Open( int FD  ){}
            virtual void Close( void );                 ///< Ok
            virtual int Send( char* Data, int Length, long TimeOutSec = 5L );     ///< Ok
            virtual int Receive( char* Data, int Length, long TimeOutSec = 5L  );
            virtual int GetConnectionID( void ) { return FX25FD; }                ///<OK
            static  void CloseX25( void ) {}
    };
#endif  ///< _TCP_ONLY  
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------    
#endif  ///< __AIX
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------    
#ifdef __SOLARIS
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Works on Solaris
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _TCP_ONLY
    class X25Connection : public Connection
    {
    public:
            X25Connection( ){}
            virtual ~X25Connection( ){}
            virtual void Open( int LineNo, int Pvc, char*  ){}
            virtual void Open( int /*FD*/  ){}
            virtual void Close( void ){}
            virtual int Send( char* Data, int Length, long TimeOutSec = 5L ){return 0;}
            virtual int Receive( char* Data, int Length, long TimeOutSec = 5L  ){return 0;}
            virtual int GetConnectionID( void ) { return 0; }
            static  void CloseX25( void ) {}
    };
#else
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
            virtual void Open( int LineNo, int Pvc, char*  );
            virtual void Open( int /*FD*/  ){}
            virtual void Close( void );
            virtual int Send( char* Data, int Length, long TimeOutSec = 5L );
            virtual int Receive( char* Data, int Length, long TimeOutSec = 5L  );
            virtual int GetConnectionID( void ) { return FX25FD; }
            static  void CloseX25( void ) {}
    };
#endif ///< _TCP_ONLY
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif ///< __SOLARIS
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _WIN32
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Works on Windows
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <x25/x25.h>
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
private:
	int  FX25FD;
	char OSDTPTR FSendBuff;	///< Send Buffer Pointer
	char OSDTPTR FRecvBuff;	///< Receive Buffer Pointer
private:
	int CheckError( void );
	int WritPVCData( char*Data, int Len, int info, long timeout );
	int ReadPVCData( char*Data, int& Len,BOOL& More, long timeout );
public:
	X25Connection( );
	virtual ~X25Connection( );
	virtual void Open( int LineNo, int Pvc, char*  = NULL );
        virtual void Open( int FD  ){}
	virtual void Close( void );
	virtual int Send( char* Data, int Length, long TimeOutSec = 5L );
	virtual int Receive( char* Data, int Length, long TimeOutSec = 5L  );
	virtual int GetConnectionID( void ) { return FX25FD; }
	static  void CloseX25( void );
};
#endif
#ifdef __LINUX
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Works on Linux
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _TCP_ONLY
    class X25Connection : public Connection
    {
    public:
            X25Connection( ){}
            virtual ~X25Connection( ){}
            virtual void Open( int LineNo, int Pvc, char*  ){}
            virtual void Open( int /*FD*/  ){}
            virtual void Close( void ){}
            virtual int Send( char* Data, int Length, long TimeOutSec = 5L ){return 0;}
            virtual int Receive( char* Data, int Length, long TimeOutSec = 5L  ){return 0;}
            virtual int GetConnectionID( void ) { return 0; }
            static  void CloseX25( void ) {}
    };
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <linux/if_ether.h>
    #include <netinet/in.h>
    #include <sys/ioctl.h>
    #include <fcntl.h>
    #include <string.h>
    #include <unistd.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>

    #include <linux/x25.h>
    #include "fsx25sockapi.h"
    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    class X25Connection : public Connection
    {
    private:
            int   FX25FD;
            int   FPort;        
            int   FPVC;
    private:
            void CreateSocket( void );
            void SelectPort( int Port );
            void AttachPVC( int PVC );
            BOOL ReadyToWrite( long timeout, long us = 0 );
            BOOL ReadyToRead( long timeout, long us = 0 );
            Int32 SendBuffer( const Int8* SendData, Int32 Size );
            Int32 ReceiveBuffer( char* RecvData, Int32 Size );
    public:
            X25Connection( );
            virtual ~X25Connection( );
            virtual void Open( int LineNo, int Pvc, char*  = NULL );
            virtual void Open( int FD ){}
            virtual void Close( void );
            virtual int Send( char* Data, int Length, long TimeOutSec = 5L );
            virtual int Receive( char* Data, int Length, long TimeOutSec = 5L  );
            virtual int GetConnectionID( void ) { return FX25FD; }
            static  void CloseX25( void ){}
    };
#endif  ///< _TCP_ONLY   
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif  ///< __LINUX
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
