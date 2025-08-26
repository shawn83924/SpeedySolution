/**@file OnlineSunSystem.h
*  @brief Declare class Online subsystem.
*  @author Simon Chaang
*
*/
//----------------------------------------------------------------------------
#ifndef __TWSE_LINKSUBSYSTEM_H
#define __TWSE_LINKSUBSYSTEM_H
//--------------------------------------------------------------------------------------------------------------
#include "../UFC/AnsiString.h"
#include "Connection.h"
#include "Messages.h"
#include "ExchangeConst.h"
//#include "../ExchangeUtility/Utility.h"
//--------------------------------------------------------------------------------------------------------------
#define PVC_LOCK_TIMES      		      3
//--------------------------------------------------------------------------------------------------------------
#define PVC_STATUS_UNKNOWN      		  256///< Unknow state.
#define PVC_STATUS_UNKNOWNPVC   		  128///< Unknow PVC.
//--------------------------------------------------------------------------------------------------------------
#define PVC_STATUS_WAIT_TRY 			  19
#define PVC_STATUS_TRY_OPEN 			  18
#define PVC_STATUS_WAIT_OPEN			  17
#define PVC_STATUS_SLEEPING			      16
#define PVC_STATUS_DISABLEING			  15
#define PVC_STATUS_ENABLE			      14
#define PVC_STATUS_DISABLE 		          13
#define PVC_STATUS_BUSY   		          12
#define PVC_STATUS_IDLE   		          11
#define PVC_STATUS_SUBSYS_ERR   		  10
#define PVC_STATUS_FINISHED      		  9
#define PVC_STATUS_BACKUP      		      8
#define PVC_STATUS_PW_ERR       		  7 ///< Password error.
#define PVC_STATUS_FCM_ERR      		  6 ///< Invalidate Broker ID.
#define PVC_STATUS_AP_ERR       		  5 ///< Invalidate AP Code.
//--------------------------------------------------------------------------------------------------------------
#define PVC_STATUS_BROKEN       		  4 ///< Link Subsystem Broken.
//--------------------------------------------------------------------------------------------------------------
#define PVC_STATUS_OFFLINE_BUSY    	      3
#define PVC_STATUS_OFFLINE_READY          2
//-------------------------------------------------------------------------------------------------------------------
#define PVC_STATUS_ONLINE_BUSY   		  1
#define PVC_STATUS_ONLINE_READY           0
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//--------------------------------------------------------------------------------------------------------------
class ConnectionObjectBase;
//-------------------------------------------------------------------------------
class LinkSubSystemBase
{
protected:
    Connection*  	    FConnection;
    int 		        FLine;
    int                 FLocalPort;
    int 		        FPVCNumber;
    char		        FAPCode;
    UFC::AnsiString	    FBrokerID;
    UFC::AnsiString	    FPassword;
    UFC::AnsiString	    FAddress;
    UFC::AnsiString	    FPVCID;
protected:
    int                 FSocketFD;
    int		            FStatus;
    int                 FLockReason;
    BOOL		        FLinkSubSysStatus;
    BOOL                FStopHandShake;
    BOOL                FIsTCP;
        BOOL                FIsTWSE;
protected:
    void                SetConnectionState( BOOL IsConnected ,int PVCState );
    void  		        SetConnectionState( int PVCState );
    char 		        ToExchangeAPCode( char SpeedyAPCode );
private:
    ConnectionObjectBase* FLintener;
    UFC::BufferedLog*     FLogManager;
    char                  FLinkPVCSendStr[ 32 ];
    char                  FLinkPVCRecvStr[ 32 ];
    char*                 FBuffer;
    char*                 FLogBuffer;
    void                  WriteLog( BOOL IsSend, char *Msg, int Length );
public:
    LinkSubSystemBase( BOOL IsTWSE,
                       int Line,
                       int PVCNumber,
                       const char* BrokerID,
                       const char* Password,                            
                       char APCode,
                       const char* IP = "NULL",
                       int LocalPort = ANY_LOCAL_PORT,
                       const char* PVCID = "00" );
    LinkSubSystemBase( BOOL IsTWSE, ConnectionParameter& Params, int FD = -1 );        
    virtual ~LinkSubSystemBase();
    virtual BOOL   OnlineHandshake( BOOL ReceivedL010 ) = 0;
    void           Open( void );
    void 	       Open( ConnectionParameter& Params );
    void 	       Close( void );
    int            GetConnectionID(){ return FConnection->GetConnectionID(); }
    BOOL           SendMessage( MessageHeader* Message, long TimeoutSec,  BOOL ThrowReconnectException = TRUE );
    MessageHeader* ReceiveMessage( long TimeOutSec, BOOL ThrowReconnectException = TRUE );
    MessageHeader* ReceiveMessage( TMessageType RequireType, int Timeout );
    int            GetStatus( void ){ return FStatus; }
    void           SetStatus( int Status ){ FStatus = Status; }
    void           StopHandshake(){ FStopHandShake = TRUE; }
    void           SetLogManager( UFC::BufferedLog* LogManagerObj ) { FLogManager = LogManagerObj; }
    void           SetListener( ConnectionObjectBase* Lintener) { FLintener = Lintener; }
    void           SetAPCode( char NewAPCode ){ FAPCode = NewAPCode; } 
    void           SetSocketFD( int SocketFD ){ FSocketFD = SocketFD; } 
    int            GetSocketFD( void ){ return FSocketFD; } 
    BOOL           IsTCPConnection( void ) { return FIsTCP;}
};
//-------------------------------------------------------------------------------
class LinkSubSystemTAIFEX  : public LinkSubSystemBase
{
private:
    char FKeyValueStr[3];
    char FAppendNo[4];
private:
    BOOL WakeUpHandshake( void );
    BOOL LoginHandshake( int ErrorCode = 0 );
    int  SubsystemStartupHandshake( );///< return -1 means failed. 0 for succeed 1,2,3,4 for errorcode
    int  Authentication( MessageL040* L040 ); ///< return 0 for succeed 1,2,3,4 for errorcode
    void ConfirmSubSysStartup( void );
public:
    LinkSubSystemTAIFEX( BOOL IsTWSE, ConnectionParameter& Params, int FD = -1 ):LinkSubSystemBase( IsTWSE, Params, FD ){}
    LinkSubSystemTAIFEX( BOOL IsTWSE, int Line, int PVCNumber, char* Password, char* BrokerID,char APCode );
    virtual BOOL OnlineHandshake(  BOOL ReceivedL010 = FALSE );
};
//-------------------------------------------------------------------------------
class LinkSubSystemFCM : public LinkSubSystemBase
{
private:
    UFC::AnsiString	FAppendNo;
private:
    BOOL WakeUpHandshake( );
    BOOL LoginHandshake( );
    BOOL SubsystemStartupHandshake( );
public:
    LinkSubSystemFCM( BOOL IsTWSE, ConnectionParameter& Params ):LinkSubSystemBase( IsTWSE, Params ){}
    LinkSubSystemFCM( BOOL IsTWSE, int Line, int PVCNumber, UFC::AnsiString Password, UFC::AnsiString BrokerID,char APCode );
    virtual BOOL OnlineHandshake(  BOOL ReceivedL010  = FALSE );
};
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------
