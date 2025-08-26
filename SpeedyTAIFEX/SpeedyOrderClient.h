//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __FCMCLIENT_H
#define __FCMCLIENT_H
#include "../UFC/UFC.h"
#include "../Migo/Sigo.h"
#include "../TAIFEXLib/Messages.h"
#include "../TAIFEXLib/TMPMessage.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define PVC_BUFFER_SIZE 256
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MessageHeader;
class LinkSubSystemFCM;
class LogFile;
class ConnectionParameter;
class ConnectionListener;
class ConnectionObjectBase;
class SpeedyObjectsManager;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class OrderListener // Abstract class
{
public:
    virtual ~OrderListener( void ){}
    // stage control
    //virtual void  OnReconnect( void ) = 0;
    virtual void  OnOpenMarket( void ) = 0;
    virtual void  OnCloseMarket( void ) = 0;
    virtual void  OnIdle( void ) = 0;
    virtual void  Handshake( BOOL ReceiveL010 = FALSE ) = 0;
    virtual BOOL  IsReady( void ) = 0;
    virtual UInt8 GetHeartBeatInterval( void ) = 0;
    // Message handler
    virtual BOOL           OnOrder( MessageHeader* CtlHeader, Int64 NID = 0 ) = 0;
    virtual BOOL           OnOrder( TMP::TMPMessage* Order, Int64 NID = 0 ) = 0;
    virtual MessageHeader* OnHandleReplyMessage( TMessageType& Type , BOOL& NeedLog, UFC::AnsiString& ExecKey ) = 0;
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SpeedyOrderClient :public OrderListener
{
private:
        LinkSubSystemFCM*     FX25OrderLine;
        LogFile* 	      FLogFile;
        UFC::AnsiString	      FBrokerID;
        UFC::AnsiString       FLinePassword;
        int   		      FLine;
        int    		      FPVCNumber;
        int 		      FOrderCount;
        BOOL                  FIsConnected;
        ConnectionListener*   FConnectionListener;
        ConnectionObjectBase* FOwner;
public:
        SpeedyOrderClient( ConnectionParameter &Param,
                           ConnectionListener* Listener,
                           ConnectionObjectBase* Owner,
                           LogManager* OrderLogManager );
        virtual ~SpeedyOrderClient();
public:
        // OrderListener interface
        //void  OnReconnect();    ///< Reconnect to TAIFEX.
        void  OnOpenMarket();   ///< Call this function when open market.
        void  OnCloseMarket(){}	///< Call this function when close market.
        void  OnIdle();        ///< Call this function to perform idle handshake.
        void  Handshake( BOOL ReceiveL010 = FALSE );
        BOOL  IsReady();
        UInt8 GetHeartBeatInterval( void);
public:
        BOOL  OnOrder( MessageHeader* CtlHeader, Int64 NID = 0 );
        BOOL  OnOrder( TMP::TMPMessage* Order, Int64 NID = 0 ) { return FALSE; }
        MessageHeader* OnHandleReplyMessage( TMessageType& Type ,BOOL& NeedLog, UFC::AnsiString& ExecKey );
        void  StopHandshake();
        Int32 GetStatus();
        void SetStatus( Int32 );
        void Listen( BOOL );
        void Open();
        void Close();
};
//-----------------------------------------------------------------------------------------------------------------------
class ThroughputCum
{
private:
    Int32* FThroughputs;
    Int32  FPos;
    Int32  FCount;
public:
    ThroughputCum( Int32 Sec );
    ~ThroughputCum( void );
    void  PutThroughput( Int32 Orders );
    Int32 GetCumThroughput( void );
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SpeedyTMPOrderClient :public OrderListener, public MessageListener, public UFC::PThread
{
private:
        SpeedyObjectsManager* FObjectManager;
        LinkSubSystemFCM*     FTMPOrderLine;
        LogFile*              FLogFile;
        UFC::AnsiString	      FBrokerID;
        UFC::AnsiString	      FPBNO;
        UFC::AnsiString       FLinePassword;
        UFC::AnsiString       FURL;
        UFC::AnsiString       FSID;
        UFC::AnsiString       FSyncSubject;
        UFC::AnsiString       FSyncKey;
        int                   FLine;
        int                   FLineBrokerIndex;
        int                   FSessionID;
        int                   FOrderCount;
        int                   FSendHeartbeat;
        int                   FRecvHeartbeat;
        BOOL                  FIsConnected;
        BOOL                  FIsBinary;
        char*                 FDataStr;
        ConnectionListener*   FConnectionListener;
        ConnectionObjectBase* FOwner;
        Int32                 FOutCount;
        Int32                 FFirstOrderTick;
        Int32                 FOrderPerSec;
        Int32                 FSlowDownFlag;
        Int32                 FOrderSleepNS;
        Int32                 FPrevSeq;
        ThroughputCum         FThroughputCum;
        int                   FPerforanceGroup;
        UFC::PCriticalSection                               FMsgCS;
        UFC::PHashedList<UFC::AnsiString, TMP::TMPMessage*> FMessages;
private:
        void AddToSentList( TMP::TMPMessage& Order );
        void RemoveFromSentList( TMP::TMPMessage& Exec );
        void RemoveFromSentList( TMP::TMPMessage& Exec, UFC::AnsiString&  );
public:
        SpeedyTMPOrderClient( SpeedyObjectsManager*  ObjManager,
                              ConnectionParameter &Param,
                              ConnectionListener* Listener,
                              ConnectionObjectBase* Owner,
                              BOOL BinaryMsg = FALSE  );
        virtual ~SpeedyTMPOrderClient();
        BOOL SendR11( void );
        BOOL SendR01( MessageR010* R010, Int64 NID );
        BOOL SendR07( MessageR070* R070, Int64 NID );
        BOOL SendR09( MessageR090* R090, Int64 NID );
        void OnTMPMessage( TMP::TMPMessage* Msg );
private:
        void ReplyR02( TMP::TMPMessage* R02 );
        void ReplyConfirm( TMP::TMPMessage& R02, Int32 Func, BOOL WithSeq );
        void ReplyOrderConfirm( TMP::TMPMessage& R02, Int32 Func, BOOL WithSeq );
        void ReplyQuoteConfirm( TMP::TMPMessage& R02, Int32 Func, BOOL WithSeq );
private:
        void ReplyR03( TMP::TMPMessage* R03 );
        void FTPReply( TMP::TMPMessage* R14 );
        void ReplyReject( TMP::TMPMessage& R03, Int32 Func, const UFC::AnsiString& Symbol );
        void ReplyOrderReject( TMP::TMPMessage& R03, Int32 Func, const UFC::AnsiString& Symbol );
        void ReplyQuoteReject( TMP::TMPMessage& R03, Int32 Func, const UFC::AnsiString& Symbol );
private:
        BOOL IsIOCPartialFill( TMP::TMPMessage& R02 );
        void FillExecution( TMP::TMPMessage& R02 , BOOL CheckExists = TRUE );
        void CancelReplaceExecution( TMP::TMPMessage& R02, BOOL IsCancel );
        void QuoteCancelReplaceExecution( TMP::TMPMessage& R02,  BOOL IsCancel );
private:
        char GetBSCode( TMP::TMPMessage& R02 );
        char GetOrderType( TMP::TMPMessage& R02 );
        char GetPositionEffect( TMP::TMPMessage& R02 );
private:
        void  ControlTroughput( void );
        UInt8 HandleSlowdownWarning( UInt8 Status );
        void  QueryUnknownOrders( void );
        void  SendSessionSequence( void );
        // Interface MessageListener
        virtual void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* );
public:
        void SendConfirmMessage( Int32 Seq, MessageHeader* Msg, Int32 NID, UFC::AnsiString& Key, UFC::AnsiString& TFXTime, BOOL NeedLog, Int32 Status, Int32 UID,Int32 CumQty,double PxSubTotal  );
        void SendConfirmMessage( Int32 Seq, MessageHeader* Msg, Int32 NID, UFC::AnsiString& Key,UFC::AnsiString& CKey, UFC::AnsiString& TFXTime, BOOL NeedLog, Int32 Status, Int32 UID,Int32 CumQty,double PxSubTotal  );
        void SendFillMessage( Int32 Seq, Int32 OrigNID, const UFC::AnsiString& TAIFEXTime, const UFC::AnsiString& X25Time, const UFC::AnsiString& Key, const UFC::AnsiString& CKey, Int32 UID,Int32 CumQty,double PxSubTotal, BOOL IsCR = FALSE );
public:
        // OrderListener interface
        void  OnOpenMarket( void );   ///< Call this function when open market.
        void  OnCloseMarket( void ){}	///< Call this function when close market.
        void  OnIdle( void ){}	        ///< Call this function to perform idle handshake.
        void  Handshake( BOOL ReceiveL010 = FALSE );
        BOOL  IsReady( void );
        UInt8 GetHeartBeatInterval( void );
        void  Execute( void );        
public:
        BOOL  OnOrder( MessageHeader* CtlHeader, Int64 NID = 0 );
        BOOL  OnOrder( TMP::TMPMessage* Order, Int64 NID = 0 );
        MessageHeader* OnHandleReplyMessage( TMessageType& Type ,BOOL& NeedLog, UFC::AnsiString& ExecKey ) { return NULL; }
        void  StopHandshake();
        Int32 GetStatus();
        void SetStatus( Int32 );
        void Listen( BOOL );
        void Open();
        void Close();
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

