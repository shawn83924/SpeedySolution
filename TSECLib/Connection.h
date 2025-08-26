/**@file Connection.h
*  @brief Define generic connection class
*		  and the X.25 PVC connection class.
*  @author Simon Chaang
*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _TWSE_CONNECTION_H
#define _TWSE_CONNECTION_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _WIN32
	#include <fcntl.h>
	#include <errno.h>
        //#ifndef __LINUX
        //    #include <sys/stream.h>
        //#endif
	#include <sys/time.h>
	#include <sys/types.h>
	//#include <stropts.h>
	#include <unistd.h>
	#include <strings.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "../UFC/Exception.h"
#include "../UFC/AnsiString.h"
#include "../UFC/NetUtility.h"
#include "../UFC/PSocket.h"
#include "../ExchangeUtility/TFXException.h"
#include "../ExchangeUtility/Utility.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "ExchangeConst.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TCP_TSEC_ERROR	 	 -7
#define TCP_HEARTBEAT 	 	 -6
#define TCP_ACCEPT	  	 -5
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define PVC_TIMEOUT    		 -2
#define PVC_DISCONNECT 		 -3
#define PVC_COMMAND    		 -4
#define PVC_RESET    		 -4
#define PVC_OK		  	  0
#define FIX_SESSION_PVC          99
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//-----------------------------------------------------------------------------------------------------------------------
class ConnectionParameter
{
private:
    int  FLink;
    int  FPVC;
    char FAPCode;
    UFC::AnsiString FBrokerID;
    UFC::AnsiString FPassword;
    UFC::AnsiString FAddress;
    UFC::AnsiString FPVCID;
    int  FLocalPort;
    BOOL FActive;
    int  FThroughput;
public:
    ///< Constractor
    ConnectionParameter( int Link,
                         int PVC,
                         char APCode,
                         UFC::AnsiString Broker,
                         UFC::AnsiString Password,
                         UFC::AnsiString IP = "NULL",
                         int LocalPort = -1,
                         UFC::AnsiString PVCID = "00",
                         BOOL Active = TRUE,
                         int Throughput = 0 )
    :FLink( Link ),
     FPVC( PVC ),
     FAPCode( APCode ),
     FBrokerID( Broker ),
     FPassword( Password ),
     FAddress( IP ),
     FPVCID( PVCID ),
     FLocalPort( LocalPort ),
     FActive( Active ),
     FThroughput( Throughput ){}
    ///< Copy constractor
    ConnectionParameter( const ConnectionParameter& Src )
    :FLink( Src.FLink ),
     FPVC( Src.FPVC ),
     FAPCode( Src.FAPCode ),
     FBrokerID( Src.FBrokerID ),
     FPassword( Src.FPassword ),
     FAddress( Src.FAddress ),
     FPVCID( Src.FPVCID ),
     FLocalPort( Src.FLocalPort ),
     FActive( Src.FActive ),
     FThroughput( Src.FThroughput ){}
    int 			 GetLink( void )         { return FLink; }
    int 			 GetPort( void )         { return FLink; }
    int 			 GetLocalPort( void )    { return FLocalPort; }
    int 			 GetPVC( void )          { return FPVC; }
    char 			 GetAPCode( void )       { return FAPCode; }    
    UFC::AnsiString& GetBrokerID( void )     { return FBrokerID; }
    UFC::AnsiString& GetPassword( void )     { return FPassword; }
    UFC::AnsiString& GetAddress( void )      { return FAddress; }
    UFC::AnsiString& GetPVCID( void )        { return FPVCID; }
    void 	     SetAPCode( char NewAP ) { FAPCode = NewAP;  }
    BOOL             IsActive( void )        { return FActive; }  
    BOOL             IsFIX( void )           { return (FPVC== FIX_SESSION_PVC); }    
    int              GetThroughput( void )   { return FThroughput; }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Connection
{
protected:
        BOOL FIsOpen; ///< Is connected flag.
public:
        Connection():FIsOpen( FALSE ){}
        virtual ~Connection(){}
        ///< Is connected?
        BOOL IsOpened() { return FIsOpen; }
        ///< Open the connection.
        virtual void Open( int Port, int PVC, char* IP = NULL, int LocalPort = -1 ) = 0;
        ///< Open the connection.( for Server )
        virtual void Open( int FD ) = 0;
        ///< Close the PVC connection.
        virtual void Close( void )=0;
        ///< Send data.
        virtual int Send( char* Buffer, int Length,long TimeOut ) = 0;
        ///< Receive data.
        virtual int Receive( char* Buffer,int Length,long TimeOut ) = 0;
        ///< Return the connection handle.(FD)
        virtual int GetConnectionID( void ) = 0;
};
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
