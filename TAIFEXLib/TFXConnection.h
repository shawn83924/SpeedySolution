/**@file Connection.h
*  @brief Define generic connection class
*		  and the X.25 PVC connection class.
*  @author Simon Chaang
*
*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _CONNECTION_H
#define _CONNECTION_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "../UFC/Exception.h"
#include "../UFC/AnsiString.h"
#include "../ExchangeUtility/TFXException.h"
#include "../ExchangeUtility/Utility.h"
#include "TMPMessage.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define PVC_TIMEOUT    		 -2
#define PVC_DISCONNECT 		 -3
#define PVC_COMMAND    		 -4
#define PVC_RESET    		 -4
#define PVC_OK		  	  0
//-----------------------------------------------------------------------------------------
namespace TFX
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
using namespace UFCType;
//-----------------------------------------------------------------------------------------------------------------------
class ConnectionParameter
{
private:
    int             FLink; ///< as Port in TMP
    int             FPVC;  ///< as SessionID in TMP
    int             FGroup;
    BOOL            FActive;
    BOOL            FIsOffHour;
    char            FAPCode;
    UFC::AnsiString FBrokerID;
    UFC::AnsiString FPassword;
private:    
    UFC::PStringList FURLs;
    UFC::AnsiString FURL;              ///< For TCP/IP TMP only
    int             FBrokerIndex;      ///< For TCP/IP TMP only
    UInt8           FHeartbeatInverval;///< For TCP/IP TMP only
    UInt16          FMaxThroughput;    ///< For TCP/IP TMP only
    BOOL            FEnableTFXR14;
    BOOL            FIsOptions;
public:
    ///< Copy constructor
    ConnectionParameter( const ConnectionParameter& Src )
    :FLink( Src.FLink ),
     FPVC(Src.FPVC),
     FGroup(Src.FGroup),
     FActive(Src.FActive),
     FIsOffHour( Src.FIsOffHour),
     FAPCode(Src.FAPCode),
     FBrokerID(Src.FBrokerID),
     FPassword(Src.FPassword),      
     FURLs(Src.FURLs ),
     FURL(Src.FURL ),
     FBrokerIndex( Src.FBrokerIndex ),
     FHeartbeatInverval( Src.FHeartbeatInverval ),
     FMaxThroughput( Src.FMaxThroughput ),
     FEnableTFXR14(Src.FEnableTFXR14 ),
     FIsOptions( Src.FIsOptions )
    {

    }    
    ///< Constructor for both X.25 and TCP connection.
    ConnectionParameter( BOOL _IsOptions,
			             int LocalPort,            ///< TCP/IP Local port.
                         int SessionID,            ///< TCP/IP Session ID.
                         char APCode,              ///< Application Code
                         BOOL OffHour,             ///< Off-Hour trading session 
                         UFC::AnsiString BrokerID, ///< BrokerID
                         UFC::AnsiString Password, ///< Password
                         int  Group        = 0,
                         BOOL Active       = TRUE,
                         const UFC::AnsiString& URL = "NULL",
                         UInt8 HeartBtInt  = 30,
                         UInt16 Throughput = 20,
                         BOOL EnableTFXR14 = FALSE )
    :FLink( LocalPort ),
     FPVC( SessionID ),
     FGroup( Group ),
     FActive( Active),
     FIsOffHour( OffHour ),
     FAPCode( APCode ),
     FBrokerID( BrokerID ),
     FPassword( Password ),     
     FURL( URL ),
     FBrokerIndex( TMP::TMPMessage::GetBrokerID( BrokerID )),
     FHeartbeatInverval( HeartBtInt ),
     FMaxThroughput( Throughput ),
     FEnableTFXR14( EnableTFXR14 ),
     FIsOptions( _IsOptions )
    {        
        FURLs.SetStrings( FURL, "^" );
    }    
public:    
    ///< For Both TMP/X.25
    int                    GetGroup( void )       { return FGroup; }
    char                   GetAPCode( void )      { return FAPCode; }
    const UFC::AnsiString& GetBrokerID( void )    { return FBrokerID;}
    const UFC::AnsiString& GetPassword( void )    { return FPassword;}
    BOOL                   IsTCPConnection( void ){ return !(FURL == "NULL");}
    BOOL                   IsActive( void )       { return FActive; }
    ///< For X.25
    int                    GetLink( void )        { return FLink;}
    int                    GetPVC( void )         { return FPVC;}
    ///< for Support TMP protocol
    int                    GetBrokerIndex( void ) { return FBrokerIndex;}    
    int                    GetPort( void )        { return FLink;}
    int                    GetSessionID( void )   { return FPVC;}    
    UInt8                  GetHeartBtInt( void )  { return FHeartbeatInverval; }
    UInt16                 GetThroughput( void )  { return FMaxThroughput; }
    const UFC::AnsiString& GetURL( void )         { return FURL;}
    int                    GetURLCount( void )    { return FURLs.ItemCount();}
    const UFC::AnsiString& GetBackupURL( int i )  { return FURLs[ i ];}
    BOOL                   IsOptions( void )      { return FIsOptions; }
    BOOL                   RecvBoardcast( void )  { return FEnableTFXR14; }
    BOOL                   IsOffHour( void )      { return FIsOffHour; }  
    
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
    virtual void Open( int Link, int PVC , char* URL = NULL ) = 0; 	  	  
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
