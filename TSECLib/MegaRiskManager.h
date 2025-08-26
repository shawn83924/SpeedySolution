/*
 * File:   MegaRiskManager.h
 * Author: Zhen Fan
 *
 * Created on 2016/02/03, 11:16
 */

#ifndef MEGARISKMANAGER_H
#define MEGARISKMANAGER_H

#include "RiskManagerAbstract.h"

namespace rm
{
//------------------------------------------------------------------------------
class CRMBrokerStockNoKey : public CRMAccountStockNoKey
{
public:
    static UFC::AnsiString BuildIndexKey(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char OrderType);
    
protected:
    UFC::AnsiString FWarrantNo;  //權證別
    UFC::AnsiString FBrokerId;   //券商代號
    char            FOrderType;  //'0':一般 '1':融資(證金) '2':融券(證金) '3':融資(自辦) '4':融券(自辦) '5':借券賣出(一般策略) '6':借券賣出(權證策略)
    
public:
    CRMBrokerStockNoKey();
    CRMBrokerStockNoKey(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char OrderType);
    
    virtual void SetAccountNo(const UFC::AnsiString& AccountNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    virtual void SetStockNo(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);

    UFC::AnsiString GetWarrantNo(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FWarrantNo, &FLock, RWLockType);};
    virtual void SetWarrantNo(const UFC::AnsiString& WarrantNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    UFC::AnsiString GetBrokerId(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FBrokerId, &FLock, RWLockType);};
    virtual void SetBrokerId(const UFC::AnsiString& BrokerId, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    char GetOrderType(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<char>(FOrderType, &FLock, RWLockType);};
    virtual void SetOrderType(char OrderType, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMBrokerStockNoKey

//------------------------------------------------------------------------------
class CRMBrokerStockCondition : public CRMBrokerStockNoKey
{
protected:
    int             FAvailableSellQty;    //可賣股數
    int             FAvailableBuyQty;     //可買股數
    int             FNormalMatchBuyQty;   //今日一般成交買進股數
    int             FNormalMatchSellQty;  //今日一般成交賣出股數
    int             FMatchBuyQty;         //今日成交買進股數
    int             FMatchSellQty;        //今日成交賣出股數
    int             FNormalOrderBuyQty;   //今日一般委託買進股數
    int             FNormalOrderSellQty;  //今日一般委託賣出股數
    int             FOrderBuyQty;         //今日委託買進股數
    int             FOrderSellQty;        //今日委託賣出股數
    UFC::AnsiString FDataDate;            //資料日期
    bool            FIsSellShort;         //是否為借券
    
public:
    CRMBrokerStockCondition(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo, 
                            const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char OrderType,
                            int AvailableSellQty, int AvailableBuyQty, 
                            const UFC::AnsiString& DataDate = "", bool IsSellShort = false);
    
    int GetAvailableSellQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FAvailableSellQty, &FLock, RWLockType);};
    void SetAvailableSellQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FAvailableSellQty, &FLock, RWLockType);};

    int GetAvailableBuyQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FAvailableBuyQty, &FLock, RWLockType);};
    void SetAvailableBuyQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FAvailableBuyQty, &FLock, RWLockType);};
    
    int GetNormalMatchBuyQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FNormalMatchBuyQty, &FLock, RWLockType);};
    void SetNormalMatchBuyQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FNormalMatchBuyQty, &FLock, RWLockType);};
    int IncreaseNormalMatchBuyQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseQty, FNormalMatchBuyQty, cstNotCheck, RWLockType);};
    
    int GetNormalMatchSellQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FNormalMatchSellQty, &FLock, RWLockType);};
    void SetNormalMatchSellQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FNormalMatchSellQty, &FLock, RWLockType);};
    int IncreaseNormalMatchSellQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseQty, FNormalMatchSellQty, cstNotCheck, RWLockType);};
    
    int GetMatchBuyQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FMatchBuyQty, &FLock, RWLockType);};
    void SetMatchBuyQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FMatchBuyQty, &FLock, RWLockType);};
    int IncreaseMatchBuyQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseQty, FMatchBuyQty, cstNotCheck, RWLockType);};
    
    int GetMatchSellQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FMatchSellQty, &FLock, RWLockType);};
    void SetMatchSellQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FMatchSellQty, &FLock, RWLockType);};
    int IncreaseMatchSellQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseQty, FMatchSellQty, cstNotCheck, RWLockType);};
    
    int GetNormalOrderBuyQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FNormalOrderBuyQty, &FLock, RWLockType);};
    void SetNormalOrderBuyQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FNormalOrderBuyQty, &FLock, RWLockType);};
    int IncreaseNormalOrderBuyQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseQty, FNormalOrderBuyQty, cstNotCheck, RWLockType);};
    
    int GetNormalOrderSellQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FNormalOrderSellQty, &FLock, RWLockType);};
    void SetNormalOrderSellQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FNormalOrderSellQty, &FLock, RWLockType);};
    int IncreaseNormalOrderSellQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseQty, FNormalOrderSellQty, cstNotCheck, RWLockType);};
    
    int GetOrderBuyQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FOrderBuyQty, &FLock, RWLockType);};
    void SetOrderBuyQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FOrderBuyQty, &FLock, RWLockType);};
    int IncreaseOrderBuyQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseQty, FOrderBuyQty, cstNotCheck, RWLockType);};
    
    int GetOrderSellQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FOrderSellQty, &FLock, RWLockType);};
    void SetOrderSellQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FOrderSellQty, &FLock, RWLockType);};
    int IncreaseOrderSellQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseQty, FOrderSellQty, cstNotCheck, RWLockType);};
    
    UFC::AnsiString GetDataDate(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FDataDate, &FLock, RWLockType);};
    virtual void SetDataDate(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<UFC::AnsiString>(NewValue, FDataDate, &FLock, RWLockType);};

    bool IsSellShort(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<bool>(FIsSellShort, &FLock, RWLockType);};
    virtual void SetSellShort(bool NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<bool>(NewValue, FIsSellShort, &FLock, RWLockType);};
    
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMBrokerStockCondition

//------------------------------------------------------------------------------
class CRMMegaStockTradingData : public CRMStockTradingData
{    
protected:
    UFC::PHashMap<UFC::AnsiString, CRMBrokerStockCondition*> FBrokerStockNoIndex;
    
public:
    CRMMegaStockTradingData(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo);
    ~CRMMegaStockTradingData();
     
    int GetBrokerStockConditionCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return GetHMapElementCount(FBrokerStockNoIndex, &FLock, RWLockType);};
    CRMBrokerStockCondition *FindBrokerStockCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char OrderType, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    bool InsertBrokerStockCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char OrderType, CRMBrokerStockCondition *ConditionPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMBrokerStockCondition *RemoveBrokerStockCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char OrderType, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    int ClearBrokerStockCondition(bool DoDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return ClearElementHMap(FBrokerStockNoIndex, DoDelete, &FLock, RWLockType);};
    void DumpBrokerStockCondition(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);    
    
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    virtual void DumpData(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMStockTradingData

//------------------------------------------------------------------------------
class CMegaRiskManagerObject : public IBasicRiskManagerObject
{
private:
    static bool FNeedCheckDataDate;
    
public:
    static void SetNeedCheckRiskDataDate(bool NeedCheckDataDate);
    static bool NeedCheckRiskDataDate();
    static void ParseRMDataFromUserData(const UFC::AnsiString& Key, UFC::AnsiString& WarrantNo, UFC::AnsiString& BrokerId);

protected:
    UFC::AnsiString FRMControlFilePrefix;
    UFC::AnsiString FRMControlFileExtension;
    int             FRMCurrentControlFileSeqNo;
    UFC::AnsiString FRMDeleteFilePrefix;
    UFC::AnsiString FRMDeleteFileExtension;
    UFC::AnsiString FRMHedgeStockFileName;
    int             FRMCurrentDeleteFileSeqNo;    
    UFC::AnsiString FRMMainBrokerID;
    double          FRMAccountStockAmountLimit;
    
    UFC::PHashedSet<UFC::AnsiString> FRMHedgeStockSet;
    
    UFC::PReadWriteLock    FRMExecRptNIDLock;
    UFC::PHashedSet<Int32> FRMExecRptNIDSet;    
    
public:
    CMegaRiskManagerObject(const UFC::AnsiString& RiskManagerName, const UFC::AnsiString& ConfigureFileName, bool IsTSEC);
    virtual ~CMegaRiskManagerObject();

    UFC::AnsiString GetRMControlFilePrefix() {return FRMControlFilePrefix;};
    UFC::AnsiString GetRMControlFileExtension() {return FRMControlFileExtension;};
    UFC::AnsiString GetRMDeleteFilePrefix() {return FRMDeleteFilePrefix;};
    UFC::AnsiString GetRMDeleteFileExtension() {return FRMDeleteFileExtension;};
    UFC::AnsiString GetRMMainBrokerID() {return FRMMainBrokerID;};
    double GetRMAccountStockAmountLimit() {return FRMAccountStockAmountLimit;};

    int GetRMHedgeStockCount() {return FRMHedgeStockSet.ItemCount();};
    bool IsRMHedgeStockExists(const UFC::AnsiString& StockNo);
    bool AddRMHedgeStockToSet(const UFC::AnsiString& StockNo);
    void DumpRMHedgeStockSet();
    
    UFC::AnsiString ComposeFullFileName(const UFC::AnsiString FilePath, const UFC::AnsiString FilePrefix, const UFC::AnsiString FileExtension, int FileSeqNo);
    void IncreaseRMDeleteFileSeqNo() {FRMCurrentDeleteFileSeqNo++;};
    bool LoadOrderDeleteData();
    void IncreaseRMControlFileSeqNo() {FRMCurrentControlFileSeqNo++;};
    bool LoadRiskManagerControlData(bool IsUpdate);
    virtual bool LoadSetting();
    virtual void PrintSetting();
    
    virtual bool CheckRiskCondition(TWSE::MessageHeader*& Msg, OrderRecord& OI, const UFC::AnsiString& PVCID, UFC::AnsiString& CheckMessage);
    virtual void AddRiskMatchQty(TWSE::R3Body *R3MsgBodyPtr, const UFC::AnsiString& PVCId, const UFC::AnsiString& ExecKey);
    virtual void ProcessRiskCancelReplaceMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI, const UFC::AnsiString PVCID);
    virtual void ProcessRiskRejectMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI, const UFC::AnsiString PVCID);
    
    void RecoverRMConfirmData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, Int32 NetworkID, bool IsSynchronize = false);
    void RecoverRMFillData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, bool IsSynchronize = false);
    virtual void ParseExecRtpLogStr(const UFC::AnsiString& ExecRptLogStr, bool IsSynchronize = false);
    virtual void RecoverRMData(BOOL IsTSE);
};  //CMegaRiskManagerObject
}  //namespace rm


#endif /* MEGARISKMANAGER_H */

