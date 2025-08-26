//----------------------------------------------------------------------------
#ifndef __LINKSUBSYSTEM_H
#define __LINKSUBSYSTEM_H
//--------------------------------------------------------------------------------------------------------------
#include "TFXConnection.h"
#include "TFXMessages.h"
#include "TMPMessage.h"
#include "../UFC/AnsiString.h"
#include "../UFC/PInt32.h"
#include "../ExchangeUtility/Utility.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//extern int   MAX_MESSAGE_SIZE;
//extern long  TXIFEX_PULL_INTERVAL;
//extern long  LINKSUBSYSTEM_TIMEOUT; 
//extern int   PVC_BUSY_RETRY_TIMES;
const int        MAX_MESSAGE_SIZE         = 1025;
const long       TXIFEX_PULL_INTERVAL     = 100L;
const long       LINKSUBSYSTEM_TIMEOUT    = 180L; 
const int        PVC_BUSY_RETRY_TIMES     = 50000;
//-----------------------------------------------------------------------------------------
namespace TFX
{
//--------------------------------------------------------------------------------------------------------------
#define PVC_LOCK_TIMES      		  3
//--------------------------------------------------------------------------------------------------------------
#define PVC_STATUS_UNKNOWN      	  256///< Unknow state.
#define PVC_STATUS_UNKNOWNPVC   	  128///< Unknow PVC.
//--------------------------------------------------------------------------------------------------------------
#define PVC_STATUS_THROUGHPUT_ERR  	  22 ///< System Throughput exceed.
#define PVC_STATUS_SYS_ERR       	  21 ///< System Type error.
#define PVC_STATUS_SID_ERR       	  20 ///< Session ID error.
//--------------------------------------------------------------------------------------------------------------
#define PVC_STATUS_WAIT_TRY 		  19
#define PVC_STATUS_TRY_OPEN 		  18
#define PVC_STATUS_WAIT_OPEN		  17
#define PVC_STATUS_SLEEPING		  16
#define PVC_STATUS_DISABLEING	          15
#define PVC_STATUS_ENABLE  		  14
#define PVC_STATUS_DISABLE 		  13
#define PVC_STATUS_BUSY   		  12
#define PVC_STATUS_IDLE   		  11
//--------------------------------------------------------------------------------------------------------------
#define PVC_STATUS_SUBSYS_ERR   	  10
#define PVC_STATUS_FINISHED      	  9
#define PVC_STATUS_BACKUP      		  8
//--------------------------------------------------------------------------------------------------------------
#define PVC_STATUS_PW_ERR       	  7 ///< Password error.
#define PVC_STATUS_FCM_ERR      	  6 ///< Invalidate Broker ID.
#define PVC_STATUS_AP_ERR       	  5 ///< Invalidate AP Code.
//--------------------------------------------------------------------------------------------------------------
#define PVC_STATUS_BROKEN       	  4 ///< Link Subsystem Broken.
//--------------------------------------------------------------------------------------------------------------
#define PVC_STATUS_OFFLINE_BUSY    	  3
#define PVC_STATUS_OFFLINE_READY          2
//--------------------------------------------------------------------------------------------------------------
#define PVC_STATUS_ONLINE_BUSY   	  1
#define PVC_STATUS_ONLINE_READY           0
//--------------------------------------------------------------------------------------------------------------
const long TMP_TIMEOUT = 10L;
//--------------------------------------------------------------------------------------------------------------
class ConnectionObjectBase;
//--------------------------------------------------------------------------------------------------------------
typedef enum
{
    lb128K = 128,
    lb256K = 256,
    lb512K = 512,
    lbE1   = 2048
            
} LineBandwidth;
//--------------------------------------------------------------------------------------------------------------
class LinkSubSystemBase
{
protected:
    static BOOL         UseBinaryLog;
    static BOOL         FDisableLog;
    Connection*  	FConnection;
    int                 FLine;
    int                 FPVCNumber;
    char                FAPCode;
    UFC::AnsiString	FBrokerID;
    UFC::AnsiString	FPassword;
    ///< For TCP/IP        
    UFC::AnsiString	FURL;
    UInt16              FBrokerIndex;
    UInt16              FBackupIndex;
#ifdef _FOR_FCM
    UFC::PInt32Array    FSequenceIn;
#else
    Int32               FSequenceIn;
#endif
    Int32               FSequenceOut;
    UInt8               FHeartBtInt;
    UInt16              FMaxThroughput;  
protected:
    int                 FStatus;
    int                 FLastError;
    int                 FLockReason;
    int                 FSocketFD;  
    BOOL		FLinkSubSysStatus;
    BOOL                FStopHandShake;
    BOOL                FIsTCP;
    BOOL                FIsOptions;
    BOOL                FIsTAIFEXSide;
    BOOL                FIsListen;
    BOOL                FIsOffHour;    
    int                 FLogBufferSize;
    int                 FLogDataSize;
protected:
    void                  SetConnectionState( BOOL IsConnected ,int PVCState );
    void  	          SetConnectionState( int PVCState );
    void                  SendMessageL010( UInt8 StatusCode );    
    UFC::AnsiString       GetUniqueID( void );
    ConnectionObjectBase* FListener;        
private:
    UFC::LogObject*       FLogManager;
    char                  FLinkPVCSendStr[ 32 ];
    char                  FLinkPVCRecvStr[ 32 ];
    char*                 FBuffer;
    char*                 FSendLogBuffer;
    char*                 FRecvLogBuffer;
    char                  FRecvTime[16];
protected:
    UInt32                FSendTick;
    LineBandwidth         FBandwidth;
    Int32                 FPrefixIndex;
    Int32                 FLogPrefixLen;
public:
    /**
     *  Constructor
     *  Construct a LinkSubSystemBase from a ConnectionParameter object.
     */
    LinkSubSystemBase( BOOL IsTAIFEX, BOOL IsOptions, ConnectionParameter& Params );
    /**
     * Constrictor
     * 
     * @Param For X.25 Connection
     *        Line: X.25 Port
     *        For TCP Connection
     *        Line: TCP port which TAIFEX TMP server listen. 
     * @Param For X.25 Connection
     *        PVCNumber: X.25 PVC number
     *        For TCP Connection
     *        PVCNumber: Session ID of this TMP connection.
     * @Param BrokerID, Broker Id which own this connection.
     * @Param Password, connection password.
     * @Param APCode, Application code
     * @Param URL, for TCP connection.
     */
    LinkSubSystemBase( BOOL IsTAIFEX, BOOL IsOptions, BOOL IsOffHour, int Line, int PVCNumber, UFC::AnsiString BrokerID, UFC::AnsiString Password, char APCode, const char* URL = "NULL" );
    virtual ~LinkSubSystemBase();
    virtual BOOL   OnlineHandshake( BOOL ReceivedL010, UInt8 StatusCode ) = 0;
    virtual void   SessionEOD( UInt8 PartID, Int32 Seq = 0 ) = 0;
    void           Open( void );
    void 	   Open( ConnectionParameter& Params );
    void 	   Close( void );
    void           Listen( BOOL Enable )                        { FIsListen = Enable; }
    int            GetConnectionID( void )                      { return FConnection->GetConnectionID(); }
    BOOL           IsErrorStat( int Stat );
    BOOL           IsBrokenStat( int PVCState );
    void           ResetStat( void );
    int            GetStatus( void );
    void           SetStatus( int Status )                      { FStatus = Status; }
    char           GetAPCode( void )                            { return FAPCode; }
    void           StopHandshake( bool COD );
    void           SetLogManager( UFC::LogObject* LogObj )      { FLogManager = LogObj; }
    void           SetListener( ConnectionObjectBase* Listener) { FListener = Listener; }
    void           SetHeartBeatInterval( UInt8 Interval )       { FHeartBtInt = Interval; }
    void           SetThroughput( UInt16 OrderPerSec )          { FMaxThroughput = OrderPerSec; }
    BOOL           IsReady( void )                              { return FLinkSubSysStatus; }
    UInt8          GetHeartBeatInterval( void )                 { return FHeartBtInt; }
    UInt16         GetThroughput( void )                        { return FMaxThroughput; }
    void           CheckMessageChecksum( UInt8* Buffer, Int16 Len );
    void           CheckTMPMessageSeq( TMP::TMPMessage* Msg );
    void           CheckTMPMessage( TMP::TMPMessage* Msg, BOOL ThrowReconnectException );
public: ///< I/O functions for X.25
    void           WriteTMPMessageLog( TMP::TMPMessage* Msg, Int32 size, BOOL UseOrginalTime = FALSE );    
    BOOL             SendMessage( MessageHeader* Message, long TimeoutSec,  BOOL ThrowReconnectException = TRUE );
    MessageHeader*   ReceiveMessage( long TimeOutSec, BOOL ThrowReconnectException = TRUE );
    MessageHeader*   ReceiveMessage( TMessageType RequireType, int Timeout );
public: ///< I/O functions for TCP/IP
    BOOL             SendTMPMessage( TMP::TMPMessage* Message, long TimeoutSec, BOOL ThrowReconnectException = TRUE, BOOL WithSeq = FALSE );    
    BOOL             SendTMPMessage( TMP::TMPMessage* Message, long TimeoutSec, long Group, Int8 FlagID, const UFC::AnsiString& Key1, const UFC::AnsiString& Key2 );        
    BOOL             SendTMPMessage( TMP::FastTMPMessage* Message, long Timeout, BOOL ThrowReconnectException = TRUE, BOOL WithSeq = FALSE );    
    BOOL             SendTMPMessage( TMP::FastTMPMessage* Message, long TimeoutSec, long Group, Int8 FlagID, const UFC::AnsiString& Key1, const UFC::AnsiString& Key2 );        
    BOOL             SendTMPMessages( UFC::List< TMP::TMPMessage*>& Messages, long Timeout, BOOL ThrowReconnectException = TRUE, BOOL WithSeq = FALSE );
    void             LogSendTMPMessage( TMP::FastTMPMessage* Message );            
    TMP::TMPMessage* ReceiveTMPMessage( long TimeOutSec,  BOOL ThrowReconnectException /*= TRUE*/ );
    TMP::TMPMessage* ReceiveTMPMessageNotLog( long TimeOutSec,  int& Result, BOOL ThrowReconnectException /*= TRUE*/ );
    //TMP::TMPMessage* ReceiveTMPMessage( UInt8 RequireType, int Timeout );
    void             WriteLog( BOOL IsSend, UInt8 *Msg, int Length, BOOL UseOrginalTime = FALSE );
public:         
    void           DumpData( UInt8* Data, Int32 Len );
    void           SetSocketFD( int SocketFD ){ FSocketFD = SocketFD; } 
    int            GetSocketFD( void ){ return FSocketFD; }
    LineBandwidth  GetSessionBandwidth( void ) { return FBandwidth; }
    int            GetInboundSequence( UInt8 PartID ) { return (Int32) FSequenceIn[ PartID ]; }
    int            GetTMPSystemType( void );
    void           SetBackupIndex( int index );
    int            GetBackupIndex( void )      { return FBackupIndex; }
    void           SetIsOffHour( BOOL IsOffHour )  { FIsOffHour = IsOffHour; }
    BOOL           GetIsOffHour( void )            { return FIsOffHour; }
public: 
    static void SetLogMode( BOOL IsBinary ) { UseBinaryLog = IsBinary; }
    static void DisableLog( BOOL Disable )  { FDisableLog  = Disable; }
};
//--------------------------------------------------------------------------------------------------------------
class TMPRecoverMessages
{
public:
       UInt32 BeginSeq;
       UInt32 EndSeq;
       UInt32 BinSize;
       BOOL   CompactFormat;
       UFC::List<TMP::TMPMessage*> Messages;
public:       
       TMPRecoverMessages( UInt32 Begin, UInt32 End, BOOL Compact ):BeginSeq( Begin ),EndSeq( End ),BinSize( 0 ),CompactFormat( Compact ){}
       ~TMPRecoverMessages( void )
       {
           for( int i =0 ;i< Messages.ItemCount(); i++ )
               delete Messages.GetItem( i );
       }
       
};
//-------------------------------------------------------------------------------
class LinkSubSystemTAIFEX  : public LinkSubSystemBase
{
private:
    UInt16 FKeyValue;
    UInt8  FAppendNo;
    UInt32 FTAIFEXOutSeq;
private:        
    BOOL WakeUpHandshake( void );
    BOOL LoginHandshake( int ErrorCode = 0 );
    int  SubsystemStartupHandshake( );///< return -1 means failed. 0 for succeed 1,2,3,4 for errorcode
    int  Authentication( MessageL040* L040 ); ///< return 0 for succeed 1,2,3,4 for errorcode
    int  Authentication( TMP::TMPMessage* L040 );
    void ConfirmSubSysStartup( void );
    void RecoverMissingMessages( TMP::TMPMessage* L040 );    
    void SendL41( UFC::MemoryStream& Stream, UInt32 Totalsize, BOOL IsEOF  );        
public:
    LinkSubSystemTAIFEX( ConnectionParameter& Params ):LinkSubSystemBase(TRUE, Params.IsOptions(), Params){}
    LinkSubSystemTAIFEX( BOOL IsOptions, int Line, int PVCNumber, char* Password, char* BrokerID,char APCode, const char* URL = "NULL"  );
    virtual BOOL OnlineHandshake(  BOOL ReceivedL010 = FALSE, UInt8 StatusCode = 0 );
    virtual void SessionEOD( UInt8 PartID, Int32 Seq = 0 );
    Int32        GetOutboundSeq( void ) { return FTAIFEXOutSeq; }
};
//-------------------------------------------------------------------------------
class LinkSubSystemFCM : public LinkSubSystemBase
{
private:
    UFC::AnsiString   FAppendNo;
    Int32             FCancelOrderSec;  
    UFC::PInt8List    FPartIDs;
private:
    BOOL WakeUpHandshake( void );
    BOOL LoginHandshake( void );
    BOOL SubsystemStartupHandshake( void );
private:
    BOOL WakeUpHandshakeTMP( void );
    BOOL LoginHandshakeTMP( void );
    BOOL SubsystemStartupHandshakeTMP( Int32 ReqSeq );        
    Int32 OnRecoverMessage( UInt8 PartID, UFC::MemoryStream& Stream );
    void Bendwidth( void );
    BOOL RecoverFLEX( void );
public:
    LinkSubSystemFCM( ConnectionParameter& Params ):LinkSubSystemBase( FALSE, Params.IsOptions(), Params){}
    LinkSubSystemFCM( BOOL IsOptions, BOOL IsOffHour, int Line, int PVCNumber, UFC::AnsiString Password, UFC::AnsiString BrokerID,char APCode, const char* URL, Int32 CancelOrderSec  );
    void OnRecoverFLEX( TMP::TMPMessage* RX41 );
    virtual BOOL OnlineHandshake(  BOOL ReceivedL010 = FALSE, UInt8 StatusCode = 0 );
    virtual void SessionEOD( UInt8 PartID, Int32 Seq );
};
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------
