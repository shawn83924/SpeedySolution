/* 
 * File:   TWSEMegaRMDataPool.h
 * Author: Zhen Fan
 *
 * Created on 2021/01/11 14:52
 */

#ifndef TWSEMEGARMDATAPOOL_H
#define TWSEMEGARMDATAPOOL_H
#include "TWSERMDataPool.h"

#include "TWSEMegaRMObjects.h"


namespace twserm
{
//------------------------------------------------------------------------------
class CMegaRiskManagerDataPool : public CRiskManagerDataPool
{
protected:
    static UFC::AnsiString FRiskManagerDataPath;
    static UFC::AnsiString FRMControlFilePrefix;
    static UFC::AnsiString FRMControlFileExtension;
    static UFC::AnsiString FRMDeleteFilePrefix;
    static UFC::AnsiString FRMDeleteFileExtension;
    static UFC::AnsiString FRMHedgeStockFileName;
    static UFC::AnsiString FRMStockWhiteListFileName;
    static UFC::AnsiString FRMMainBrokerID;
    static bool            FNeedCheckSelfTrading;
    static bool            FNeedCheckRiskDataDate;
    static int             FCheckFileInterval;
    static UFC::AnsiString FRMStockWhiteListFilePathName;
    
//    static UFC::PHashedSet<UFC::AnsiString> FRMHedgeStockSet;
    static UFC::PHashMap<UFC::AnsiString, CRMHedgeStock*> FHedgeStockNoIndex;
    
    static int FRMCurrentControlFileSeqNo;
    static int FRMCurrentDeleteFileSeqNo;
    
public:
    static UFC::AnsiString GetRiskManagerDataPath() {return FRiskManagerDataPath;};
    static void SetRiskManagerDataPath(const UFC::AnsiString& NewValue) {FRiskManagerDataPath = NewValue;};

    static UFC::AnsiString GetRMControlFilePrefix() {return FRMControlFilePrefix;};
    static void SetRMControlFilePrefix(const UFC::AnsiString& NewValue) {FRMControlFilePrefix = NewValue;};

    static UFC::AnsiString GetRMControlFileExtension() {return FRMControlFileExtension;};
    static void SetRMControlFileExtension(const UFC::AnsiString& NewValue) {FRMControlFileExtension = NewValue;};

    static UFC::AnsiString GetRMDeleteFilePrefix() {return FRMDeleteFilePrefix;};
    static void SetRMDeleteFilePrefix(const UFC::AnsiString& NewValue) {FRMDeleteFilePrefix = NewValue;};

    static UFC::AnsiString GetRMDeleteFileExtension() {return FRMDeleteFileExtension;};
    static void SetRMDeleteFileExtension(const UFC::AnsiString& NewValue) {FRMDeleteFileExtension = NewValue;};

    static UFC::AnsiString GetRMHedgeStockFileName() {return FRMHedgeStockFileName;};
    static void SetRMHedgeStockFileName(const UFC::AnsiString& NewValue) {FRMHedgeStockFileName = NewValue;};
    
    static UFC::AnsiString GetRMStockWhiteListFileName() {return FRMStockWhiteListFileName;};
    static void SetRMStockWhiteListFileName(const UFC::AnsiString& NewValue) {FRMStockWhiteListFileName = NewValue;};
    
    static UFC::AnsiString GetRMMainBrokerID() {return FRMMainBrokerID;};
    static void SetRMMainBrokerID(const UFC::AnsiString& NewValue) {FRMMainBrokerID = NewValue;};

    static bool NeedCheckSelfTrading() {return FNeedCheckSelfTrading;};
    static void SetNeedCheckSelfTrading(bool NewValue) {FNeedCheckSelfTrading = NewValue;};

    static int GetCheckFileInterval() {return FCheckFileInterval;};
    static void SetCheckFileInterval(int NewValue) {FCheckFileInterval = NewValue;};

    static UFC::AnsiString GetRMStockWhiteListFilePathName() {return FRMStockWhiteListFilePathName;};
    static void SetRMStockWhiteListFilePathName(const UFC::AnsiString& NewValue) {FRMStockWhiteListFilePathName = NewValue;};
    
//    static bool IsRMHedgeStockExists(const UFC::AnsiString& StockNo);
//    static bool AddRMHedgeStockToSet(const UFC::AnsiString& StockNo);
//    static void DumpRMHedgeStockSet();
    static int GetHedgeStockNoIndexCount() 
        {return UFC::GetStrIdxHMapElementPtrCount(FHedgeStockNoIndex, 0, UFC::rwNotLock);};
    static CRMHedgeStock* FindHedgeStockFromNoIndex(const UFC::AnsiString& StockNo)
        {return UFC::FindElementPtrFromStrIdxHMap(FHedgeStockNoIndex, StockNo, 0, UFC::rwNotLock);};
    static bool InsertHedgeStockToNoIndex(const UFC::AnsiString& StockNo, CRMHedgeStock* StockPtr, bool CheckExist = UFC::cstNotCheck)
        {return UFC::InsertElementPtrIntoStrIdxHMap(FHedgeStockNoIndex, StockNo, StockPtr, CheckExist, 0, UFC::rwNotLock);};
    static CRMHedgeStock* RemoveHedgeStockFromNoIndex(const UFC::AnsiString& StockNo)
        {return UFC::RemoveElementPtrFromStrIdxHMap(FHedgeStockNoIndex, StockNo, 0, UFC::rwNotLock);};
    static int ClearHedgeStockNoIndex(bool DoDelete = UFC::cstNotDelete)
        {return UFC::ClearElementPtrStrIdxHMap(FHedgeStockNoIndex, DoDelete, 0, UFC::rwNotLock);};
    static void DumpRMHedgeStockNoIndex();
    static bool LoadRiskManagerHedgeStock();
    static bool IsRMHedgeStock(const UFC::AnsiString& StockNo, const UFC::AnsiString& AccountNo);

    static int GetRMCurrentControlFileSeqNo() {return FRMCurrentControlFileSeqNo;};
    static void SetRMCurrentControlFileSeqNo(int NewValue) {FRMCurrentControlFileSeqNo = NewValue;};
    static int IncreaseRMCurrentControlFileSeqNo();
    
    static int GetRMCurrentDeleteFileSeqNo() {return FRMCurrentDeleteFileSeqNo;};
    static void SetRMCurrentDeleteFileSeqNo(int NewValue) {FRMCurrentDeleteFileSeqNo = NewValue;};
    static int IncreaseRMCurrentDeleteFileSeqNo();
    
protected:
    UFC::PReadWriteLock FAccountLock;
    UFC::PHashMap<UFC::AnsiString, CRMInvestorAccount*> FAccountNoIndex;
    
    UFC::PReadWriteLock FOrderLock;
    UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*> FOrderIDIndex;
    
    UFC::PReadWriteLock FillRptExecLock;
    UFC::PHashedSet<UFC::AnsiString> FillRptExecKey;
    
public:
    CMegaRiskManagerDataPool();
    virtual ~CMegaRiskManagerDataPool();
    
    void LockAccounts(UFC::RWLockTypeEnum LockType) {FAccountLock.Lock(LockType);};
    void UnlockAccounts(UFC::RWLockTypeEnum LockType) {FAccountLock.Unlock(LockType);};
    int GetAccountNoIndexCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) 
        {return UFC::GetStrIdxHMapElementPtrCount(FAccountNoIndex, &FAccountLock, LockType);};
    CRMInvestorAccount* FindAccountFromNoIndex(const UFC::AnsiString& AccountNo, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::FindElementPtrFromStrIdxHMap(FAccountNoIndex, AccountNo, &FAccountLock, LockType);};
    bool InsertAccountToNoIndex(const UFC::AnsiString& AccountNo, CRMInvestorAccount* AccountPtr, bool CheckExist = UFC::cstNotCheck, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::InsertElementPtrIntoStrIdxHMap(FAccountNoIndex, AccountNo, AccountPtr, CheckExist, &FAccountLock, LockType);};
    CRMInvestorAccount* RemoveAccountFromNoIndex(const UFC::AnsiString& AccountNo, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::RemoveElementPtrFromStrIdxHMap(FAccountNoIndex, AccountNo, &FAccountLock, LockType);};
    int ClearAccountNoIndex(bool DoDelete = UFC::cstNotDelete, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::ClearElementPtrStrIdxHMap(FAccountNoIndex, DoDelete, &FAccountLock, LockType);};
    int ClearAccountStockWhiteList(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    void DumpAccountStockWhiteList(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    void DumpAccountNoIndex(bool DumpDetail, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    
    void LockOrders(UFC::RWLockTypeEnum LockType) {FOrderLock.Lock(LockType);};
    void UnlockOrders(UFC::RWLockTypeEnum LockType) {FOrderLock.Unlock(LockType);};
    int GetOrderIDIndexCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) 
        {return UFC::GetStrIdxHMapElementPtrCount(FOrderIDIndex, &FOrderLock, LockType);};
    CRMOrderSummary* FindOrderFromIDIndex(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::FindElementPtrFromStrIdxHMap(FOrderIDIndex, OrderID, &FOrderLock, LockType);};
    bool InsertOrderToIDIndex(const UFC::AnsiString& OrderID, CRMOrderSummary* OrderPtr, bool CheckExist = UFC::cstNotCheck, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::InsertElementPtrIntoStrIdxHMap(FOrderIDIndex, OrderID, OrderPtr, CheckExist, &FOrderLock, LockType);};
    CRMOrderSummary* RemoveOrderFromIDIndex(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::RemoveElementPtrFromStrIdxHMap(FOrderIDIndex, OrderID, &FOrderLock, LockType);};
    int ClearOrderIDIndex(bool DoDelete = UFC::cstNotDelete, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::ClearElementPtrStrIdxHMap(FOrderIDIndex, DoDelete, &FOrderLock, LockType);};
        
    virtual void LoadSetting();
    virtual void PrintSetting();
    
    UFC::AnsiString ComposeDataFilePathName(const UFC::AnsiString& FilePath, const UFC::AnsiString& FileNamePrefix, const UFC::AnsiString& FileNameExtension, int FileSeqNo);
    bool LoadRiskManagerControlData(bool IsUpdate);
    bool LoadOrderDeleteData();
    bool LoadStockWhiteList();
};  //CMegaRiskManagerDataPool

extern CMegaRiskManagerDataPool* gMegaRMDataPoolPtr;
}  //namespace twserm
#endif /* TWSEMEGARMDATAPOOL_H */

