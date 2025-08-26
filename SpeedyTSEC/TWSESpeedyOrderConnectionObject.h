#ifndef __FCM_ORDER_CONNECTION_OBJECT_H
#define __FCM_ORDER_CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "../TSECLib/ConnectionObject.h"
#include "../TSECLib/MessageHeader.h"
#include "../UFC/UFC.h"
#include "../UFC/PInt32.h"
#include "../UFC/PQueue.h"
#include "TWSEOrderInfoStore.h"
//----------------------------------------------------------------------------------------------------------------------
//const key_t OPT_X25_QUEUE =   0x73A10000;
//const key_t FUT_X25_QUEUE =   0x72A10000;
const key_t STOCK_ORD_QUEUE = 0x74A10000;
const key_t OTC_ORD_QUEUE =   0x75A10000;
const key_t TSE_ORD_SHM =     0x10000000;
const key_t OTC_ORD_SHM =     0x20000000;
//-----------------------------------------------------------------------------------------------------------------------
const int PVC_ALL_GROUP    = 99;
const int PVC_MARKET_GROUP = 98;
//-----------------------------------------------------------------------------------------------------------------------
extern Int32                  MAX_DATA_SIZE;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
class OrderListener;
class ConnectionListener;
class SpeedyObjectsManager;
//-----------------------------------------------------------------------------------------------------------------------
//   FCM Side: Connection Object for AP code 4
//-----------------------------------------------------------------------------------------------------------------------
class SpeedyOrderConnectionObject : public ConnectionObjectBase,public UFC::PThread
{    
private:    
    typedef BOOL (SpeedyOrderConnectionObject::*ProcessOrderFunc)( void );    
private:
    OrderListener*       FOrderFCM;
    ConnectionListener*  FConnectionListener;
    int		         FOrderNumber;
    int                  FSubSystem;
    int                  FBrokerInt;
    int                  FPVCGroup;
    BOOL                 FReconnect;
    BOOL                 FHandshake;
    BOOL                 FEnabled;
    BOOL                 FBrokenRequest;
    //ConnectionParameter& FParams;
    UFC::PCharArray      FShareMemData;
    UFC::PQueue          FQueue;
    UFC::PMPMCQueue<OrderRecord>* FLFQueue;
    UFC::UQueueData      FQueueData;
    char                 FOrigAPCode;
    char                 FDataStr[ 512 ];
    BOOL                 FFirstTimeDisable;
    BOOL                 FICanTry;
    BOOL                 FIsFIX;
    BOOL                 FBusying;
    char                 FPVCID[ 3 ]; 
    UFC::AnsiString      FLineBrokerID;
    SpeedyObjectsManager* FObjectManager; 
    BOOL                  FIsTWSE;
    Int32                 FProfileID;
    ProcessOrderFunc      FProcessOrder;
private: ///< For FIX throughput control.   
    Int32                FOutCount;
    Int32                FFirstOrderTick;
    Int32                FOrderPerSec;
    void                 ControlFIXTroughput( void );
private:    
    int FNormalSystem; ///< Normal
    int FOddSystem; ///< Odd stock
    int FOddExSystem; ///< OddEx stock
    int FPostSystem;
    int FLendSystem;
    int FAuctionSystem;
    int FTenderSystem;
    int FTenderExSystem;
private: ///< Implement PThread
    virtual void Execute( void );
private:
    BOOL OpenMarket();
    BOOL ProcessOrder();
    BOOL ProcessOrderEx();
    BOOL ProcessFIXOrder();    
    
    BOOL ProcessFIXTryOrderEx();    
    BOOL ProcessFIXNormalOrderEx();
    BOOL ProcessFIXNormalOrderExBenchmark();
    BOOL ProcessFIXOrderEx();    
    
    BOOL WaitMarketOpen();
    BOOL WaitTry( void );
    void DisableLoop( void );
    void WaitingLoop( void );	
    void SessionWaitingLoop( char APCode );        
    void SendReply( Int32 NID, Int32 OrigNID,const UFC::AnsiString& Key,const UFC::AnsiString& Host, MessageHeader* ReplyMsg, BOOL NeedLog,BOOL IsCD, const UFC::AnsiString& ExecKey, const UFC::AnsiString& OrigKey, const UFC::AnsiString& ErrMsg = "" );
    void LogTimeoutOrder( OrderRecord& OI );
    void ReplyFateX020( Int32 NID, TMessageType Type, const UFC::AnsiString& Key, const UFC::AnsiString& Host,const UFC::AnsiString& Message, Int32 Code );
    //void GetOrderID( TMessageType Type, MessageHeader* Message, UFC::AnsiString& OID  );
    void GetOrderID( TMessageType Type, MessageHeader* Message, char* OID  );
    void SetAPCode( char APCode );
    void CheckAPCodeNow( void );
    BOOL IsMarketClosed();
    OrderInfo* RecordInfo( MessageHeader* OrderMessage, OrderRecord& OI, BOOL& IsNew, BOOL Force );
    OrderInfo* AddTOPRecordInfo( Int32 Func, const char* oid, const char* brokerid, OrderRecord& OI, BOOL& IsNew, BOOL Force );
    OrderInfo* AddAEVRecordInfo( Int32 Func, const char* oid, const char* brokerid, OrderRecord& OI, BOOL& IsNew, BOOL Force );
private:        
    UFC::PCriticalSection FTryCS;
    UFC::PEvent           FTryEvent;
    BOOL FIsTrying;        
public:    
    void SetTrying( BOOL Trying ) 
    { 
        UFC::PLockObject L( FTryCS ); 
        FIsTrying = Trying; 
    }
    BOOL IsTrying( void ) 
    { 
        UFC::PLockObject L( FTryCS ); 
        return FIsTrying; 
    }
private:        
    MessageHeader* RebuildIncompleteOrderAndGenerateBrokenQueryMessage( OrderRecord& OI );
    MessageHeader* BeginTransaction(  OrderRecord& OI  );
    MessageHeader* BeginTransactionEx(  OrderRecord*& OI , bool KeepOrder );
    MessageHeader* RollbackTransaction(  OrderRecord& OI  );
    void           OpenTransaction( void );
    void           EndTransaction( void );
    void           FetchOrderFromQueue( OrderRecord& OI );
    void           FetchOrderFromSHM( OrderRecord& OI );
    void           SaveOrderToSHM( OrderRecord& OI );
    
    
    MessageHeader* GenerateTryOrder( OrderRecord& OI );
    MessageHeader* GenerateTryOrderEx( OrderRecord*& OI );
    BOOL           GenerateOrderMessageFromQueue( MessageHeader*& Msg, OrderRecord& OI );
    BOOL           GenerateOrderMessageFromQueueEx( MessageHeader*& Msg, OrderRecord*& OI, bool KeepOrder );
    void           HandleExchangeReplyTimeout( OrderRecord& OI, int ErrorCode = 87 );    
    void           MTypeSet( UFC::PLongList& MTypes );
    void           AddFIXMTypeSet( int Group, UFC::PLongList& MTypes );
    void           MTypeSet2( UFC::PLongList& MTypes );
    BOOL           IsTransactionComplete( void );    
    bool           IsDynamicSession( void );
    void           FinishTry( void );  
#ifdef __RMMega    
    bool           CheckRiskCondition( MessageHeader*& Msg, OrderRecord& OI, UFC::AnsiString& CheckMessage );  //RM
    void           ProcessRiskRejectMessage( MessageHeader*& ReplyMsg, OrderRecord& OI );    //RM
    void           ProcessRiskBuySellMessage( MessageHeader*& ReplyMsg, OrderRecord& OI );    //RM
    void           ProcessRiskCancelReplaceMessage( MessageHeader*& ReplyMsg, OrderRecord& OI );    //RM
#endif
    
public:
    SpeedyOrderConnectionObject( SpeedyObjectsManager* ObjManager,
                                 ConnectionParameter&  info,
                                 ConnectionListener*   CListener,
                                 UFC::ThreadListener*  TListener,
                                 UInt32                SHNID,
                                 int                   Group,
                                 BOOL                  IsTWSE );
    virtual ~SpeedyOrderConnectionObject();
    virtual void HandleExchangeReplyMessage( MessageHeader* ReplyMsg, OrderRecord& OI, Int32 OrigNID, TMessageType Type, BOOL NeedLog,BOOL IsCD, const UFC::AnsiString& ExecKey, const UFC::AnsiString& OrigKey, const UFC::AnsiString& ErrMsg = "" );
    virtual void HandleExchangeReplyMessage( MessageHeader* ReplyMsg, OrderInfo* OI, TMessageType Type,  BOOL NeedLog,  BOOL IsCD, const UFC::AnsiString& ExecKey,   const UFC::AnsiString& OrigKey,  const UFC::AnsiString& ErrMsg = "" );
    void         HandleExchangeReplyTryMessage( MessageHeader* ReplyMsg, OrderRecord& OI, TMessageType Type, BOOL NeedLog, BOOL IsCD, const UFC::AnsiString& ExecKey, const UFC::AnsiString& OrigKey/*, UFC::AnsiString& BrokerID*/ );    
    BOOL         IsFIXSession( void ) { return FIsFIX; }
    BOOL         SendFIXOrder(  OrderRecord*  OI, Int64& SendOrderTick );
    BOOL         SendFIXOrder(  OrderRecord*  OI );
public: ///< Implement ConnectionObjectBase
    virtual void Active( void ) { UFC::PThread::Start( 1024*512 ); }
    virtual void Close( void ){}
    virtual void OnCommand( int , void*  );
    virtual void UpdateConnectionParameter( ConnectionParameter& ){}
    virtual void Enable( BOOL );
    virtual BOOL IsEnable( void );
    virtual void Listen( BOOL IsListen );
    virtual void SetFIXSeq( int InSeq, int OutSeq );
    virtual void SetTradeSession( char APCode );
};
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------
