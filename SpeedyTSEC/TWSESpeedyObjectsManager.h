#ifndef __TWSE_ORDER_Objects_Manager_H
#define __TWSE_ORDER_Objects_Manager_H
//--------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../UFC/iniFile.h"
#include "../Migo/Sigo.h"
#include "ContainerTemplate.h"  //UFC
#include "../TSECLib/ConnectionObject.h"
//--------------------------------------------------------------------------------------------------------------------
#include "TWSESpeedyConfirmConnectionObject.h"     ///< AP Code  3 connection object
#include "TWSESpeedyOrderConnectionObject.h"       ///< AP Code  4 connection object
#include "TWSEOrderInfoStore.h"
#include "TWSESpeedyFIXManager.h"
#include "MarketSchedule.h"
#include "SMPMessage.h"
//RM
#include "../TSECLib/TWSERMClass.h"  //TSECLib

#ifdef __RMMega
#include "RiskManagerObjects.h"
#endif
//RM
//----------------------------------------------------------------------------------------------------------------------
#define SPEEDY_ERROR_NOT_ENOUGH_QTY_FOR_SELL 97
#define SPEEDY_ERROR_NOT_SPEEDY              90
#define SPEEDY_ERROR_MARKET_CLOSED           91
#define SPEEDY_ERROR_CONNECTION              88
#define SPEEDY_ERROR_TIMEOUT                 87
#define SPEEDY_ERROR_ORDERID_MISS            86
#define SPEEDY_ERROR_QUEUE_FILLED            85
#define SPEEDY_ERROR_STOCK_RUSH              84
#define SPEEDY_ERROR_NO_SPEEDY               83
#define SPEEDY_ERROR_NO_PVC                  82
#define SPEEDY_ERROR_MAPPER_REJECT           81
//--------------------------------------------------------------------------------------------------------------------
#define PVC_NOT_EXISTS  9999999
//--------------------------------------------------------------------------------------------------------------------
class TSECLines;
class OrderRecord;
//-----------------------------------------------------------------------------------------
namespace TWSE
{   
//--------------------------------------------------------------------------------------------------------------------
class SpeedyObjectsManager;
class SpeedyOrderConnectionObject;
UFC::AnsiString DescribeMarketStatus(TMarketStatus MarketStatus);
UFC::AnsiString DescribeMarketType(TMarketType MarketType);

//---------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
class SimPVCListener : public MessageListener
{
private:
      SpeedyObjectsManager* FManager;
public:
      SimPVCListener( SpeedyObjectsManager* Manager ):FManager( Manager ) {}
      virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data );
};
//--------------------------------------------------------------------------------------------------------------------
class TransferOrderThread : public UFC::PThread
{
private:
    Int32           FSystem;
    Int32           FBroker;
    SpeedyObjectsManager* FObjectsManager;
public:    
    TransferOrderThread( char AP, UFC::AnsiString& BrokerID, SpeedyObjectsManager* ObjectsManager  );
    void Execute( void );
};
//--------------------------------------------------------------------------------------------------------------------
class SpeedyObjectsManager : public UFC::ThreadListener, public MessageListener, public ConnectionListener
{
public:
    UFC::BufferedLog *OrderLog;
    UFC::BufferedLog *FillLog;
    Int32             OrderQueueGroupCount;
    Int32             OddGroup;      
private:
    UFC::PHashedList<UFC::AnsiString, ConnectionObjectBase*> FConnectionObjects;
    UFC::PHashMap<Int32, OrderRecord*> FOrderRecords;
    UFC::PHashMap<UFC::AnsiString, OrderRecord*> FOrderRecordsGen5;
    UFC::List<SpeedyOrderConnectionObject*> FFIXSession;
    int                   FSessionindex;
    int                   FFCMOrderPVCCount;
    int                   FFCMConfirmPVCCount;
    int                   FCMConfirmPVCCount;
    int                   FCMOrderConfirmPVCCount;
    UFC::SInt             FTotalOrder;        
    long	          FTotalTime;
    int                   FMaxTime;
    int 		  FMsgCount;
    int                   FFIXReadyCount;
    long 		  FPrevTime;
    UFC::PCriticalSection FCriticalSection; ///< Protect connection list.   
    UFC::PCriticalSection FLockPVCCS;       ///< Protect rush locled PVC count.   
    UFC::PCriticalSection FMarketStatusCS;  ///< Protect market session status.
    UFC::PCriticalSection FPVCCountCS;      ///< Protect avaliable PVC count.   
    UFC::PCriticalSection FOrdRecordCS;
    UFC::PQueue           FQueue;
    UFC::PMPMCQueue<OrderRecord>* FMPMCQueue;
    Int16*                FStockTable;
    Int32                 FRushPVCCount;                
    bool                  FOverLimitStop;
    UFC::PStringHashedSet FPVCSet;    
    UFC::AnsiString       FDealerID;
    UFC::AnsiString       FCfgFileName;	
    time_t                FModifyTime;
    SpeedyFIXManager*     FFIXManager;            
    UFC::PHashedSet<UFC::AnsiString> FWarningCode;
private:    
    UFC::AnsiString FOrderSubject;
    UFC::AnsiString FOrderConfirmSubject;
    UFC::AnsiString FFillReportSubject;
    UFC::AnsiString FNotSpeedyFillSubject;
    UFC::AnsiString FSyncSubject;    
    UFC::AnsiString FSimPVCSubject;    
    BOOL            FIsTWSE;
    int             FLoopCount;
    int             FProfileID;
public:    
    const UFC::AnsiString& GetOrderSubject( void )           { return FOrderSubject; }
    const UFC::AnsiString& GetConfirmSubject( void )         { return FOrderConfirmSubject; }
    const UFC::AnsiString& GetFillSubject( void )            { return FFillReportSubject; }
    const UFC::AnsiString& GetNotSpeedyFillSubject( void )   { return FNotSpeedyFillSubject; }
    const UFC::AnsiString& GetSyncPSubject( void )           { return FSyncSubject; }    
    const UFC::AnsiString& GetSimPVCSubject( void )          { return FSimPVCSubject; }
    bool  IsDynamicSession( int SubSystem );
    bool  IsIntradayOddSession( int SubSystem );
//RM
    bool                           FIsRiskManagerEnabled;
    UFC::AnsiString                FRiskManagerID;
    UFC::AnsiString                FRiskManagerConfigureFilePathName;
    twserm::CTWSERiskManagerClass* FRMObjPtr;
    
private: //Members for RiskManager
#ifdef __RMMega
    bool                   FRMControlIsSetting;
    bool                   FNeedCheckSelfTrading;
    UFC::AnsiString        FStockFormatFilePath;
    UFC::AnsiString        FTSEStockT30FormatFileName;
    UFC::AnsiString        FTSEStockT32FormatFileName;
    UFC::AnsiString        FTSEStockT33FormatFileName;
    UFC::AnsiString        FTSEStockO40FormatFileName;
    UFC::AnsiString        FOTCStockT30FormatFileName;
    UFC::AnsiString        FOTCStockT32FormatFileName;
    UFC::AnsiString        FOTCStockT33FormatFileName;
    UFC::AnsiString        FOTCStockO40FormatFileName;
    UFC::AnsiString        FTSEStockFilePath;
    UFC::AnsiString        FTSEStockT30FileName;
    UFC::AnsiString        FTSEStockT32FileName;
    UFC::AnsiString        FTSEStockT33FileName;
    UFC::AnsiString        FTSEStockO40FileName;
    UFC::AnsiString        FOTCStockFilePath;
    UFC::AnsiString        FOTCStockT30FileName;
    UFC::AnsiString        FOTCStockT32FileName;
    UFC::AnsiString        FOTCStockT33FileName;
    UFC::AnsiString        FOTCStockO40FileName;
    UFC::AnsiString        FRMFilePath;
    UFC::AnsiString        FRMControlFilePrefix;
    UFC::AnsiString        FRMControlFileExtension;
    int                    FRMCurrentControlFileSeqNo;
    UFC::AnsiString        FRMDeleteFilePrefix;
    UFC::AnsiString        FRMDeleteFileExtension;
    UFC::AnsiString        FRMHedgeStockFileName;
    int                    FRMCurrentDeleteFileSeqNo;
    UFC::AnsiString        FRMMainBrokerId;
    double                 FRMAccountStockAmountLimit;
    UFC::PReadWriteLock    FRMAccountLock;
    CStrRiskManagerObjHMap FRMAccountNoIndex;
    UFC::PReadWriteLock    FRMOrderLock;
    CStrRiskManagerObjHMap FRMOrderIdIndex;
    CStringHSet            FRMHedgeStockSet;
    UFC::PReadWriteLock    FRMExecRptNIDLock;
    CInt32HSet             FRMExecRptNIDSet;
    
    UFC::PReadWriteLock FStockLock;
    UFC::PHashMap<UFC::AnsiString, CStockObject*> FStockNoIndex;
    
#endif
    
#ifdef __RMLib    
    UFC::AnsiString        FRMName;
    UFC::AnsiString        FRMConfigFileName;
#endif    
//RM
    
public:
    MarketSchedule*       FNormalMarket;
    MarketSchedule*       FOddMarket;  
    MarketSchedule*       FOddExMarket;  
    MarketSchedule*       FPostMarket;        
    MarketSchedule*       FLendMarket;                
    MarketSchedule*       FTenderMarket;                
    MarketSchedule*       FTenderExMarket;                
    MarketSchedule*       FAuctionMarket;                            
public:
    OrderInfoStore  OrderInfo;
    UFC::PQueue     ExecutionQueue;
    
    UFC::PReadWriteLock FillRptExecLock;
    UFC::PHashedSet<UFC::AnsiString> FillRptExecKey;
    bool IsFillRptExecExist(const UFC::AnsiString& ExecKey, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    void AddFillRptExec(const UFC::AnsiString& ExecKey, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    
public:
    int  FBrokerID2;
    int  TransferAll( int Broker, int System  );
    int  TransferAllEx( int Broker, int System );
    void CheckPendingOrder( void );
    void ProcessPendingOrder( char AP );
    void Alarm( const UFC::AnsiString& Msg );
    void WarnningStatusCode( const UFC::AnsiString& FIXText );
    bool StopOnOverLimit( void ) { return FOverLimitStop; } 
private:
    BOOL IsMarketOrder( const char* Order, int Size );
    BOOL CheckConfigModifiaction( void );
    void RemoveOldLogFile();
    ConnectionObjectBase* GetConnection( const UFC::AnsiString& BrokerID, int Link, const UFC::AnsiString& PVCID );
    void ReportPerformance( void );
    void SendRWDataToBus( char* Data);        
    Int32 PVCStateToTradeState( char AP, Int32 State );
    void BindingCore( void );

//RM
public:
    bool IsRiskManagerEnabled() {return FIsRiskManagerEnabled;};
    
private:
#ifdef __RMMega
    UFC::AnsiString ComposeFullFileName( const UFC::AnsiString FilePath, const UFC::AnsiString FileName);
    UFC::AnsiString ComposeFullFileName( const UFC::AnsiString FilePath, const UFC::AnsiString FilePrefix, const UFC::AnsiString FileExtension, int FileSeqNo = -1 );
    int LoadStockT30Data( const UFC::AnsiString FormatFilePath, const UFC::AnsiString FormatFileName,
                          const UFC::AnsiString StockFilePath, const UFC::AnsiString StockFileName);
    int LoadStockT32Data( const UFC::AnsiString FormatFilePath, const UFC::AnsiString FormatFileName,
                          const UFC::AnsiString StockFilePath, const UFC::AnsiString StockFileName);
    int LoadStockT33Data( const UFC::AnsiString FormatFilePath, const UFC::AnsiString FormatFileName,
                          const UFC::AnsiString StockFilePath, const UFC::AnsiString StockFileName);
    int LoadStockO40Data( const UFC::AnsiString FormatFilePath, const UFC::AnsiString FormatFileName,
                          const UFC::AnsiString StockFilePath, const UFC::AnsiString StockFileName);
    bool LoadOrderDeleteData( );
    bool LoadRiskManagerControlData( bool IsUpdate );
    bool LoadRiskManagerHedgeStock( );
#endif    
    void LoadRiskManagerSettings( UFC::UiniFile& cfg );
//RM
    void LoadSettings( UFC::UiniFile& cfg );
    BOOL PVCReady( ConnectionParameter* CObj );
    BOOL PVCBroken( ConnectionParameter* CObj );        
    BOOL IsOrder( Int32 Type, MarketSchedule*& Market );
    void CheckPVC( const char* APCodes );
    void CheckPVCBroken( ConnectionParameter * cp );
    void OnPVCReady( ConnectionParameter * cp );
    void ModifyOID( char* Order, const UFC::AnsiString& OID );
    UInt32 UniqueSHMID( ConnectionParameter& cp );
public:                
    MarketSchedule* GetScheduleObject( char APCode );
    MarketSchedule* GetScheduleObjectBySubsystemID( int Subsys );        
    BOOL FetchOrderInfo( R3Body* R3, Int32& OrigNID, UFC::AnsiString& Key, UFC::AnsiString& OID );
    void ReceiveR3( char* Data, char* LinkBrokerID, char* PVCID );        
public:        
    SpeedyObjectsManager( BOOL IsTWSE );
    virtual ~SpeedyObjectsManager();
    void PushOrderRecord( Int32 NID, OrderRecord* OR );
    OrderRecord* PopOrderRecord( Int32 NID );
    void PushOrderRecord( const UFC::AnsiString& ClOrdID, OrderRecord* OR );
    OrderRecord* PopOrderRecord( const UFC::AnsiString& ClOrdID );
    void ReportPVCState(  ConnectionObjectBase * CObj, int state  );        
    void CreateConnectionObjects( int MaxPVC = 0 );
    void IncreaceOrderCount(  ) { FTotalOrder++; }
        
    int  GetLineState( const UFC::AnsiString& BrokerID, int Link, const UFC::AnsiString& PVCID,int& inSeq,int& OutSeq );
    void ReplyLineState( const UFC::AnsiString& BrokerID, int Link, const UFC::AnsiString& PVCID );
    void ListenConnection(  const UFC::AnsiString& BrokerID, int Link, const UFC::AnsiString& PVCID );
    void EnableConnection(  const UFC::AnsiString& BrokerID, int Link, const UFC::AnsiString& PVCID, BOOL Enabled );
    void SetAPCode(  const UFC::AnsiString& BrokerID, int Link, const UFC::AnsiString& PVCID, char NewAPCode );
    void SetFIXSequence( const UFC::AnsiString& BrokerID, int Link, const UFC::AnsiString& PVCID, int InSeq, int OurSeq );
    void SendExecutionToQueue( long MType, MTree* Data );
    void RejectAllOrders( Int32 Error );
    void OtherSysExecution( const UFC::AnsiString& Subject, const UFC::AnsiString& AE,  Int32 NID, const UFC::AnsiString& Msg, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrokerID, const UFC::AnsiString& ExTime, Int32 IsConfirm );    
    void SendReply(  const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  Int32 NID, const UFC::AnsiString& Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrokerID, const UFC::AnsiString& ExTime );
    void SendFill( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  Int32 NID, const UFC::AnsiString& Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrokerID, const UFC::AnsiString& ExTime );
    TWSE::OrderInfo* AddTOPRecordInfo( Int32 Func, const UFC::AnsiString& OID, const UFC::AnsiString& BrokerID, OrderRecord& OI, BOOL& IsNew, BOOL Force );
    TWSE::OrderInfo* AddAEVRecordInfo( Int32 Func, const UFC::AnsiString& OID, const UFC::AnsiString& BrokerID, OrderRecord& OI, BOOL& IsNew, BOOL Force );
    TWSE::OrderInfo* RecordInfo( MessageHeader* OrderMessage, OrderRecord& OI, BOOL& IsNew, BOOL Force );
    BOOL SendFIXOrder( OrderRecord* OI );
    BOOL SendFIXOrder( OrderRecord* OI, Int64& UseUS );    
    BOOL SendFIXOrder( SMPMessage& SMPMsg, BOOL AutoSession );
    SpeedyOrderConnectionObject*  NextSession( void );
    void Run();
    void Process( int UseMS, int Times1Sec );    
    void Download( const UFC::AnsiString & FileCode, int StartSeq, int EndSeq );
    void SpeedyReject( OrderRecord& OrderObj, Int32 ErrCode, BOOL AsConfirm = FALSE, const UFC::AnsiString& ErrorMessage = "" );
    void IncreaseLockPVC( Int32 StockID );
    void DecreaseLockPVC( Int32 StockID );                
    void NeedSessionCount( MarketSchedule* Market, Int32 Broker, int& Need, int& Changing );
    BOOL SwitchPVC( MarketSchedule* Market, Int32 Broker );
    BOOL IsStockRush( Int32 StockID );
    BOOL CheckQueueFill(  Int32 Type );
    BOOL CheckPVC( Int32 Type , Int32 Broker, Int32& Reason, BOOL& AsConfirm );
    BOOL NoFIXSession( void ); 
    BOOL GetMType( Int32 SubSystem, Int32  Broker, OrderRecord& OrderObj, long& MType );
    BOOL IsTWSE( void )   { return FIsTWSE; }
    BOOL          CanITry( char APCode );
    Int32         RetryInterval( char APCode );
    TMarketStatus GetMarketStatus( char APCode ); 
    TMarketStatus SetMarketStatus( char APCode, TMarketStatus NewStatus ); 	
	
    // Implement ThreadListener interface.
    virtual void OnTerminate( UFC::PThread* TerminateThread  ){}
    // Implement ConnectionListener interface.
    virtual void OnConnectionNotify( ConnectionObjectBase* CObj, int Reason, void* Data  = NULL);
    // Interface MessageListener
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* );
        
    SpeedyFIXManager* GetFIXManager( void ) { return FFIXManager; }
    int ToSubsystemID( char AP );
//RM
#ifdef __RMMega
    //Stock Function
    void LockStock(UFC::RWLockTypeEnum LockType) {FStockLock.Lock(LockType);};
    void UnlockStock(UFC::RWLockTypeEnum LockType) {FStockLock.Unlock(LockType);};
    int GetStockNoIndexCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::GetStrIdxHMapElementPtrCount(FStockNoIndex, &FStockLock, LockType);};
    CStockObject* FindStockFromNoIndex(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::FindElementPtrFromStrIdxHMap(FStockNoIndex, StockNo, &FStockLock, LockType);};
    bool InsertStockToNoIndex(const UFC::AnsiString& StockNo, CStockObject* StockPtr, bool CheckExist = UFC::cstNotCheck, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::InsertElementPtrIntoStrIdxHMap(FStockNoIndex, StockNo, StockPtr, CheckExist, &FStockLock, LockType);};
    CStockObject* RemoveStockFromNoIndex(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::RemoveElementPtrFromStrIdxHMap(FStockNoIndex, StockNo, &FStockLock, LockType);};
    int ClearStockNoIndex(bool DoDelete = UFC::cstNotDelete, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::ClearElementPtrStrIdxHMap(FStockNoIndex, DoDelete, &FStockLock, LockType);};
        
    bool RMControlIsSetting(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock); 
    bool NeedCheckSelfTrading(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock); 
    UFC::AnsiString GetRMMainBrokerId(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock); 
    double GetRMAccountStockAmountLimit(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock); 
    
    int GetRMAccountCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMInvestorAccount *FindRMAccount(const UFC::AnsiString& AccountNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    bool InsertRMAccount(const UFC::AnsiString& AccountNo, CRMInvestorAccount *AccountPtr, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    int ClearRMAccount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    void DumpRMAccount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);    

    int GetRMOrderCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMOrderSummary *FindRMOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    bool InsertRMOrder(const UFC::AnsiString& OrderId, CRMOrderSummary *OrderPtr, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    int ClearRMOrder(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    void DumpRMOrder(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    void RecoverRMConfirmData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, Int32 NetworkID, bool IsSynchronize = false);
    void RecoverRMFillData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, bool IsSynchronize = false);
    void ParseExecRtpLogStr(const UFC::AnsiString& ExecRptLogStr, bool IsSynchronize = false);
    void RecoverRMData(BOOL IsTSE);
    void AddRiskMatchQty(R3Body *R3MsgBodyPtr, const UFC::AnsiString& PVCId = "", const UFC::AnsiString& ExecKey = "");
    int GetRMHedgeStockCount() {return FRMHedgeStockSet.ItemCount();};
    bool IsRMHedgeStockExists(const UFC::AnsiString& StockNo);
    bool AddRMHedgeStockToSet(const UFC::AnsiString& StockNo);
    void DumpRMHedgeStockSet();
    
#endif
//RM
};
//-----------------------------------------------------------------------------------------
}
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
