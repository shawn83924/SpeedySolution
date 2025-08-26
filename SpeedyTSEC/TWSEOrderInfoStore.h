//---------------------------------------------------------------------------

#ifndef TWSE_OIDToClientIDH
#define TWSE_OIDToClientIDH
#include "../UFC/UFC.h"
#include "../UFC/PInt32.h"
#include "../UFC/ThreadObjectPool.h"
//---------------------------------------------------------------------------
class OrderRecord;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//---------------------------------------------------------------------------
class SyncExecThread;    
//---------------------------------------------------------------------------
const int MAX_OID_LEN  = 6;
const int MAX_UDD_LEN  = 384;
const int MAX_BRK_LEN  = 16;
const int MAX_HOST_LEN = 32;
//---------------------------------------------------------------------------
class OrderInfo
{
friend class OrderInfoStore;
private:
    Int32            FNewOrderNID;
    Int32            FMsgType;    
    Int32            FFuncCode;    
private:    
    Int32            FLeavesQty;
    double           FLastPx;
private:    
    char             FOrderID[ MAX_OID_LEN ];
    char             FKeyString[MAX_UDD_LEN];
    char             FBPNO[ MAX_BRK_LEN ];    
    char             FHost[ MAX_HOST_LEN ];    
public:
    OrderInfo( const UFC::AnsiString& OID,
               const UFC::AnsiString& BrokerID, 
               OrderRecord& OI );
    OrderInfo( const UFC::AnsiString& OID,
               const UFC::AnsiString& BrokerID, 
               int Func,
               int MsgType,
               int NID, 
               const UFC::AnsiString& Key,
               const UFC::AnsiString& Host );    
    void Init( const UFC::AnsiString& OID, const UFC::AnsiString& BPNO,  OrderRecord& OI );    
    OrderInfo( const UFC::AnsiString& DataString );
    OrderInfo( void );
    void FromString( const UFC::AnsiString& DataString );
    void ToString( UFC::AnsiString& DataString );
    void ToString( char* DataString,int len  );
public:
    Int32       GetNID( void )       { return FNewOrderNID; }
    Int32       GetMsgType( void )   { return FMsgType; }
    Int32       GetFuncCode( void )  { return FFuncCode; }
    Int32       GetLeavesQty( void ) { return FLeavesQty; }
    double      GetLastPx( void )    { return FLastPx; }
    void        SetLeavesQty( Int32 Val ) { FLeavesQty = Val; }
    void        SetLastPx( double Val )   { FLastPx = Val; }
    const char* GetOrderID( void )   { return FOrderID; }
    const char* GetKeyString( void ) { return FKeyString; }
    const char* GetBrokerID( void )  { return FBPNO; }
    const char* GetHostname( void )  { return FHost; }
};
//---------------------------------------------------------------------------
class ExecInfo
{
private:
    Int32 FNID;
    UFC::AnsiString FOID;
    UFC::AnsiString FKey;
    UFC::AnsiString FBPNO;
    UFC::AnsiString FBrokerID;
    BOOL FIsNewOrder;
    Int32 FFunc;
    Int32 FMsgType;
public:        
    UFC::AnsiString FLog;        
public:
    ///< To construct a execution ExecInfo object.
    ExecInfo( const UFC::AnsiString& Log = "" )
    :FIsNewOrder( FALSE )
    ,FLog( Log ){}
    ///< To construct a new order confirm ExecInfo object.
    ExecInfo( Int32 NID, const UFC::AnsiString& OID,const UFC::AnsiString& Key,const UFC::AnsiString& Log, const UFC::AnsiString& BPNO,const UFC::AnsiString& BrokerID, Int32 Func, Int32 MsgType )
    :FNID( NID ),
     FOID( OID ),
     FKey( Key ),	 
     FBPNO( BPNO ),
     FBrokerID( BrokerID ),
     FIsNewOrder( TRUE ),
     FFunc( Func ),
     FMsgType( MsgType ),
     FLog( Log ){}
    BOOL                   IsNewOrder( void ) { return FIsNewOrder; }
    Int32                  GetNID( void )	  { return FNID; }
    Int32                  GetFunc( void )	  { return FFunc; }
    Int32                  GetMsgType( void )	  { return FMsgType; }
    const UFC::AnsiString& GetOID( void )	  { return FOID; }
    const UFC::AnsiString& GetKey( void )	  { return FKey; }
    const UFC::AnsiString& GetLog( void )	  { return FLog; }
    const UFC::AnsiString& GetBPNO( void )	  { return FBPNO; }
    const UFC::AnsiString& GetBrokerID( void ){ return FBrokerID; }
};
//---------------------------------------------------------------------------
class OrderInfoStore
{
friend class SyncExecThread;
private:
    UFC::PCriticalSection                               FUpdateCS;  ///< for  FOIDMap, FNIDMap    
    UFC::PCriticalSection                               FExecKeyCS; ///< for FExecKeySet
    UFC::PCriticalSection                               FExecCS;    ///< for FExecutions
private:    
    UFC::LogObject*                                     FOrderInfoFile;    
    UFC::LogObject*                                     FExecutionIDFile;
    UFC::LogObject*                                     FExecutionLog;
private:        
    UFC::PHashedList< UFC::AnsiString, OrderInfo* >     FOIDMap;
    UFC::PHashedList< Int32, OrderInfo* >               FNIDMap;
    UFC::PHashedList< UFC::AnsiString, SyncExecThread*> FHostToSyncObj;    
    UFC::PStringHashedSet                               FExecKeySet;
    UFC::PtrList<ExecInfo>	               		FExecutions;		
private:    
    BOOL                                                FCluster;    
    UFC::AnsiString                                     FNewOrderRecoverFile;
    UFC::AnsiString                                     FExecutionIDRecoverFile;
    UFC::AnsiString                                     FExecutionLogFile;
    UFC::PThreadObjectPool<OrderInfo>*                  FOrderInfoPoolPtr;
private:    
    void SetupCluster(  BOOL RecoverMode, BOOL IsTSE  );    
    void Recover();
    void RecoverExecutionID();
    void RecoverExecutions();    
public:
    /**
     * Constructor
     */
    OrderInfoStore( Int32 TransactionMax, BOOL IsTSE, BOOL RecoverMode );
    /**
     *  Add an order record to the order store.
     */
    OrderInfo* AddOrder( const UFC::AnsiString& OID,const UFC::AnsiString& BrokerID, OrderRecord& OI, BOOL Force );
    void LogNewOrder( OrderInfo* NewOrderInfo );    
    ///< Sync from AA server
    OrderInfo* AddOrder( const UFC::AnsiString& OID,const UFC::AnsiString& BrokerID, Int32 Func, Int32 Msgtype, Int32 NewOrderNID, const UFC::AnsiString& Key, const UFC::AnsiString& Host );    
    /**
     * Use OrderID +BPNO as key to find order
     */
    BOOL GetOrderInfo( const UFC::AnsiString& OID, const UFC::AnsiString& BPNO, UFC::AnsiString& Key, Int32& NewOrderNID );
    BOOL GetOrderInfo( const UFC::AnsiString& OID, const UFC::AnsiString& OrderBrokerID, OrderInfo*& OrigOrderInfo );    
    /**
     * Use NID to find the OrderID.( Use NID to cancel the original Order.)
     */
    BOOL GetOID( Int32 NewOrderNID, UFC::AnsiString& OID, UFC::AnsiString& IB, bool NeedLock );
    /**
     * Check whether the execution exists.
     */
    BOOL IsExecExist( const UFC::AnsiString& Key );    
    /**
     * Check whether the Order exists.
     */
    BOOL IsOrderExists( const UFC::AnsiString& OID, const UFC::AnsiString& BPNO );
    /**
     * Write to Execution file.(Executions sync from other Speedy.)
     */
    void WriteExecutionLog( Int32 Seq, const UFC::AnsiString& Host, const UFC::AnsiString& Log );
    /**
     * Write to Execution file.
     */
    void WriteExecutionLog( BOOL IsConfirm, Int32 NID,const UFC::AnsiString& Key, const UFC::AnsiString& Data, const UFC::AnsiString& PBNO, const UFC::AnsiString& Host, const UFC::AnsiString& PVC );    
public:   
    BOOL ClusterEnable( void ) { return FCluster; }
    void OnSpeedyStartup( const UFC::AnsiString& Host);
    void OnSpeedyStop( const UFC::AnsiString& Host);
    void StopAllSyncThread( void );
    void RecoverExecFromSequence( const UFC::AnsiString& Host, Int32 Seq );
};
//---------------------------------------------------------------------------
class SyncExecThread : public UFC::PThread
{
private:
    UFC::PInt32*          FOutSeq;
    UFC::AnsiString       FHostname;
    BOOL                  FIsActive;
    BOOL                  FRecovering;
    OrderInfoStore*       FStore;
    UFC::PInt32HashedSet  FSeqSet;
    UFC::PCriticalSection FSeqCS;
    Int32                 FBeginTime;
    UFC::AnsiString       FSyncSubject;
private:    
    void         SendSyncMessage( void );    
    ExecInfo*    GetExecInfo( Int32 MsgSeq );
    virtual void Execute( void );    
public:
    SyncExecThread( BOOL IsTSE, BOOL RecoverMode, const UFC::AnsiString& Host, OrderInfoStore* Store );
    void  SetActive( BOOL Active )     { FIsActive = Active; }
    Int32 GetSequence( void )          { return FOutSeq->ToInt32(); }
    BOOL  MessageSeqExists( Int32 Seq );        
    void  SetRecoverFrom( Int32 Seq );
    void  SetMessageSeq( Int32 Seq );
    void  AskSequenceFrom( void );
};
//-----------------------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
