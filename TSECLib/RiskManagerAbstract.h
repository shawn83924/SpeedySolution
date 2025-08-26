/* 
 * File:   RiskManagerAbstract.h
 * Author: Zhen Fan
 *
 * Created on 2016/02/03, 10:02
 */

#ifndef RISKMANAGERABSTRACT_H
#define RISKMANAGERABSTRACT_H

#include "../UFC/UFC.h"
#include "MessageHeader.h"
#include "RMessages.h"

namespace rm
{
//------------------------------------------------------------------------------
const bool cstCheck     = true;
const bool cstNotCheck  = false;
const bool cstDelete    = true;
const bool cstNotDelete = false;
const bool cstFlush     = true;
const bool cstNotFlush  = false;
    
//------------------------------------------------------------------------------
template<typename fieldType> fieldType IncreaseVariableValue(const fieldType& IncreaseValue, fieldType& MemberField, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum RWLockType)
{
    UFC::DoReadWriteLock(LockPtr, RWLockType);
    fieldType sumValue = MemberField + IncreaseValue;
    MemberField = sumValue;
    UFC::DoReadWriteUnlock(LockPtr, RWLockType);
    return sumValue;
}  //IncreaseVariableValue()
//declare Hash Map Function template

//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
int GetHMapElementCount(UFC::PHashMap<MapKeyType, MapElementType*>& ElementMap, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    int elementCount = ElementMap.ItemCount();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementCount;
}  //GetHMapElementCount()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
MapElementType *FindElementFromHMap(UFC::PHashMap<MapKeyType, MapElementType*>& ElementMap, 
                                    const MapKeyType& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    MapElementType *elementPtr = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (ElementMap.GetObjectByKey(KeyValue, elementPtr) == FALSE) elementPtr = 0;
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //FindElementFromHMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
bool InsertElementIntoHMap(UFC::PHashMap<MapKeyType, MapElementType*>& ElementMap, 
                           const MapKeyType& KeyValue, MapElementType *ElementPtr, bool DoCheckExist, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    bool isSuccess = false;
    if (ElementPtr == 0) return isSuccess;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (DoCheckExist && (FindElementFromHMap(ElementMap, KeyValue, LockPtr, UFC::rwNotLock) != 0)) 
    {
        UFC::DoReadWriteUnlock(LockPtr, LockType);
        return isSuccess;
    }
    
    if (ElementMap.Add(KeyValue, ElementPtr) == TRUE) isSuccess = true;
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return isSuccess;
}  //InsertElementIntoHMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
MapElementType *RemoveElementFromHMap(UFC::PHashMap<MapKeyType, MapElementType*>& ElementMap, 
                                      const MapKeyType& KeyValue, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    MapElementType *elementPtr = 0;
    if (ElementMap.GetObjectByKey(KeyValue, elementPtr) == TRUE) 
        ElementMap.DeleteByKey(KeyValue);
    else
        elementPtr = 0;
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return elementPtr;
}  //RemoveElementFromHMap()
//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
int ClearElementHMap(UFC::PHashMap<MapKeyType, MapElementType*>& ElementMap, 
                     bool DoDelete, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    int deleteCount = 0;
    UFC::DoReadWriteLock(LockPtr, LockType);
    if (DoDelete)
    {
        MapElementType *curElementPtr = ElementMap.First();
        while (curElementPtr != 0)
        {
            delete curElementPtr;
            deleteCount++;
            curElementPtr = ElementMap.Next();
        }
    }  //if (DoDelete)
    
    ElementMap.Clear();
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    return deleteCount;
}  //ClearElementHMap()

//------------------------------------------------------------------------------
UFC::AnsiString DescribeBoolean(bool booleanValue);
UFC::AnsiString DescribeBoolean(BOOL boolValue);
double Parse94v99PriceStr(const UFC::AnsiString& PriceStr);
double Parse95v9999PriceStr(const UFC::AnsiString& PriceStr);
bool OpenRiskLogFile(BOOL IsTSEC, const UFC::AnsiString& RiskManagerName, const UFC::AnsiString& LogFilePath);
void SetRiskLogLevel(Int32 NewValue);
bool NeedWriteRiskLog(Int32 RiskLogLevel);
void WriteRiskLog(const UFC::AnsiString& LogHeadStr, const char* FormatStr ...);
void WriteLevelRiskLog(Int32 LogLevel, const UFC::AnsiString& LogHeadStr, const char* FormatStr ...);
void FlushRiskLog();
void CloseRiskLogFile();
    
//------------------------------------------------------------------------------
class CRMBasicObject
{
protected:
    UFC::PReadWriteLock FLock;
    UFC::AnsiString     FIndexKey;
    
    bool IncreaseMemberValue(int IncreaseValue, int& Member, bool CheckNegative, UFC::RWLockTypeEnum RWLockType);
    bool IncreaseMemberValue(double IncreaseValue, double& Member, bool CheckNegative, UFC::RWLockTypeEnum RWLockType);
    
public:        
    CRMBasicObject();
    CRMBasicObject(const UFC::AnsiString& IndexKey);
    virtual ~CRMBasicObject(){}
    
    void LockForRead() {FLock.LockForRead();};
    void UnlockForRead() {FLock.UnlockForRead();};
    void Lock(UFC::RWLockTypeEnum RWLockType) {FLock.Lock(RWLockType);};
    
    void LockForWrite() {FLock.LockForWrite();};
    void UnlockForWrite() {FLock.UnlockForWrite();};
    void Unlock(UFC::RWLockTypeEnum RWLockType) {FLock.Unlock(RWLockType);};

    UFC::AnsiString GetIndexKey(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FIndexKey, &FLock, RWLockType);};
    virtual void SetIndexKey(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<UFC::AnsiString>(NewValue, FIndexKey, &FLock, RWLockType);};;
    
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FIndexKey, &FLock, RWLockType);};
};  //CRMBasicObject

//---------------------------------------------------------------------------
template<typename MapKeyType, typename MapElementType>
void DumpRMObjHMap(UFC::PHashMap<MapKeyType, MapElementType>& ElementMap, 
                   bool DoFlush, UFC::PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    UFC::DoReadWriteLock(LockPtr, LockType);
    CRMBasicObject *curElementPtr = ElementMap.First();
    while (curElementPtr != 0)
    {
        UFC::BufferedLog::Printf(" %s", curElementPtr->ToString(UFC::rwLockForRead).c_str());
        curElementPtr = ElementMap.Next();
    }
    UFC::DoReadWriteUnlock(LockPtr, LockType);
    if (DoFlush) UFC::BufferedLog::FlushToFile();
}  //DumpElementHMap()

//------------------------------------------------------------------------------
class CRMAccountNoKey : public CRMBasicObject
{
protected:
    UFC::AnsiString FAccountNo;  //集保帳號
    
public:
    CRMAccountNoKey();
    CRMAccountNoKey(const UFC::AnsiString& AccountNo);
    
    UFC::AnsiString GetAccountNo(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FAccountNo, &FLock, RWLockType);};
    virtual void SetAccountNo(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<UFC::AnsiString>(NewValue, FAccountNo, &FLock, RWLockType);};

    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FAccountNo, &FLock, RWLockType);};
};  //CRMAccountNoKey

//------------------------------------------------------------------------------
class CRMAccountStockNoKey : public CRMAccountNoKey
{
public:
    static UFC::AnsiString BuildIndexKey(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo);
    
protected:
    UFC::AnsiString FStockNo;  //股票代號
    
public:
    CRMAccountStockNoKey();
    CRMAccountStockNoKey(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo);
    
    virtual void SetAccountNo(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    UFC::AnsiString GetStockNo(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FStockNo, &FLock, RWLockType);};
    virtual void SetStockNo(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);

    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMAccountStockNoKey

//------------------------------------------------------------------------------
class CRMOrderSummary : public CRMBasicObject
{
protected:
    UFC::AnsiString    FOrderId;     
    char               FSide;
    UFC::AnsiString    FStockNo;      //股票代號
    double             FOrderPrice;   //委託價
    int                FOrderQty;     //委託量
    int                FLeavesQty;    //剩餘量
    UFC::AnsiString    FWarrantNo;    //權證別
    UFC::AnsiString    FBrokerId;     //券商代號
    char               FTWSEOrderType;
    char               FExchangeCode; //交易碼 0:一般 1：鉅額 2：零股 3：外國股票價確認碼
    TWSE::TMessageType FMsgType;
    UFC::AnsiString    FAccountNo;    //集保帳號
    double             FOrderAmount;
    UFC::AnsiString    FOrderKey;     //客戶定義資料
    UFC::AnsiString    FTraderID;     //操盤人代碼

public:
    CRMOrderSummary(const UFC::AnsiString& OrderId, char Side, const UFC::AnsiString& StockNo,
                    const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char OrderType, 
                    double FOrderPrice, int OrderQty, char ExchangeCode, TWSE::TMessageType MsgType, const UFC::AnsiString& AccountNo = "");
   
    UFC::AnsiString GetOrderId(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FOrderId, &FLock, RWLockType);};

    char GetSide(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<char>(FSide, &FLock, RWLockType);};
    
    UFC::AnsiString GetStockNo(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FStockNo, &FLock, RWLockType);};
    
    double GetOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FOrderPrice, &FLock, RWLockType);};
    
    int GetOrderQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FOrderQty, &FLock, RWLockType);};
    void SetOrderQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FOrderQty, &FLock, RWLockType);};
    int IncreaseOrderQty(int IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseVariableValue(IncreaseValue, FOrderQty, &FLock, RWLockType);};
    
    int GetLeavesOrderQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FLeavesQty, &FLock, RWLockType);};
    void SetLeavesQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FLeavesQty, &FLock, RWLockType);};
    int IncreaseLeavesQty(int IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseVariableValue(IncreaseValue, FLeavesQty, &FLock, RWLockType);};
    
    UFC::AnsiString GetWarrantNo(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FWarrantNo, &FLock, RWLockType);};
    UFC::AnsiString GetBrokerId(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FBrokerId, &FLock, RWLockType);};
    char GetTWSEOrderType(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue(FTWSEOrderType, &FLock, RWLockType);};
    char GetExchangeCode(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<char>(FExchangeCode, &FLock, RWLockType);};
    TWSE::TMessageType GetMsgType(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<TWSE::TMessageType>(FMsgType, &FLock, RWLockType);};
    UFC::AnsiString GetAccountNo(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FAccountNo, &FLock, RWLockType);};
    double GetOrderAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue(FOrderAmount, &FLock, RWLockType);};

    UFC::AnsiString GetOrderKey(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FOrderKey, &FLock, RWLockType);};
    void SetOrderKey(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<UFC::AnsiString>(NewValue, FOrderKey, &FLock, RWLockType);};
    
    UFC::AnsiString GetTraderID(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FTraderID, &FLock, RWLockType);};
    void SetTraderID(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<UFC::AnsiString>(NewValue, FTraderID, &FLock, RWLockType);};
    
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMOrderSummary

//------------------------------------------------------------------------------
class CRMStockTradingData : public CRMAccountStockNoKey
{
private:
    static double FInitialMinOrderPrice;
    
protected:
    CRMOrderSummary *FMaxNormalBuyOrderPtr;    //最高一般買進委託單
    double          FMaxNormalBuyOrderPrice;   //最高一般買進委託價
    CRMOrderSummary *FMinNormalSellOrderPtr;   //最高一般賣出委託單
    double          FMinNormalSellOrderPrice;  //最低一般賣出委託價
    CRMOrderSummary *FMaxBuyOrderPtr;          //最高買進委託單
    double          FMaxBuyOrderPrice;         //最高買進委託價
    CRMOrderSummary *FMinSellOrderPtr;         //最高賣出委託單
    double          FMinSellOrderPrice;        //最低賣出委託價

    UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*> FNormalBuyOrderIdIndex;
    UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*> FNormalSellOrderIdIndex;
    UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*> FBuyOrderIdIndex;
    UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*> FSellOrderIdIndex;

    double FBuyAmount;
    double FSellAmount;
    
    CRMOrderSummary *FindMaxOrderPriceOrder(UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*>& OrderIdIndex, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    double FindMaxNormalBuyOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    double FindMaxBuyOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMOrderSummary *FindMinOrderPriceOrder(UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*>& OrderIdIndex, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    double FindMinNormalSellOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    double FindMinSellOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
public:
    CRMStockTradingData(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo);
    ~CRMStockTradingData();
     
    virtual CRMOrderSummary *GetMaxNormalBuyOrder(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<CRMOrderSummary*>(FMaxNormalBuyOrderPtr, &FLock, RWLockType);};
    virtual double GetMaxNormalBuyOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FMaxNormalBuyOrderPrice, &FLock, RWLockType);};
    virtual void SetMaxNormalBuyOrderPrice(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FMaxNormalBuyOrderPrice, &FLock, RWLockType);};    
    
    virtual CRMOrderSummary *GetMinNormalSellOrder(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<CRMOrderSummary*>(FMinNormalSellOrderPtr, &FLock, RWLockType);};
    virtual double GetMinNormalSellOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FMinNormalSellOrderPrice, &FLock, RWLockType);};
    virtual void SetMinNormalSellOrderPrice(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FMinNormalSellOrderPrice, &FLock, RWLockType);};    
    
    virtual CRMOrderSummary *GetMaxBuyOrder(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<CRMOrderSummary*>(FMaxBuyOrderPtr, &FLock, RWLockType);};
    virtual double GetMaxBuyOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FMaxBuyOrderPrice, &FLock, RWLockType);};
    virtual void SetMaxBuyOrderPrice(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FMaxBuyOrderPrice, &FLock, RWLockType);};    
    
    virtual CRMOrderSummary *GetMinSellOrder(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<CRMOrderSummary*>(FMinSellOrderPtr, &FLock, RWLockType);};
    virtual double GetMinSellOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FMinSellOrderPrice, &FLock, RWLockType);};
    virtual void SetMinSellOrderPrice(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FMinSellOrderPrice, &FLock, RWLockType);};    

    virtual double GetBuyAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FBuyAmount, &FLock, RWLockType);};
    virtual void SetBuyAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FBuyAmount, &FLock, RWLockType);};    
    
    virtual double GetSellAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FSellAmount, &FLock, RWLockType);};
    virtual void SetSellAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FSellAmount, &FLock, RWLockType);};    
    
    int GetNormalBuyOrderCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return GetHMapElementCount(FNormalBuyOrderIdIndex, &FLock, RWLockType);};
    CRMOrderSummary *FindNormalBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return FindElementFromHMap(FNormalBuyOrderIdIndex, OrderID, &FLock, RWLockType);};
    bool InsertNormalBuyOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMOrderSummary *RemoveNormalBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    int ClearNormalBuyOrder(bool DoDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return ClearElementHMap(FNormalBuyOrderIdIndex, DoDelete, &FLock, RWLockType);};
    void DumpNormalBuyOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);

    int GetNormalSellOrderCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return GetHMapElementCount(FNormalSellOrderIdIndex, &FLock, RWLockType);};
    CRMOrderSummary *FindNormalSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return FindElementFromHMap(FNormalSellOrderIdIndex, OrderID, &FLock, RWLockType);};
    bool InsertNormalSellOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMOrderSummary *RemoveNormalSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    int ClearNormalSellOrder(bool DoDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return ClearElementHMap(FNormalSellOrderIdIndex, DoDelete, &FLock, RWLockType);};
    void DumpNormalSellOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);    

    int GetBuyOrderCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return GetHMapElementCount(FBuyOrderIdIndex, &FLock, RWLockType);};
    CRMOrderSummary *FindBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return FindElementFromHMap(FBuyOrderIdIndex, OrderID, &FLock, RWLockType);};
    bool InsertBuyOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMOrderSummary *RemoveBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    int ClearBuyOrder(bool DoDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return ClearElementHMap(FBuyOrderIdIndex, DoDelete, &FLock, RWLockType);};
    void DumpBuyOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    int GetSellOrderCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return GetHMapElementCount(FSellOrderIdIndex, &FLock, RWLockType);};
    CRMOrderSummary *FindSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return FindElementFromHMap(FSellOrderIdIndex, OrderID, &FLock, RWLockType);};
    bool InsertSellOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMOrderSummary *RemoveSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    int ClearSellOrder(bool DoDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return ClearElementHMap(FSellOrderIdIndex, DoDelete, &FLock, RWLockType);};
    void DumpSellOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);    

    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    void DumpData(bool DoFlush, UFC::RWLockTypeEnum RWLockType);
};  //CRMStockTradingData

//------------------------------------------------------------------------------
class CRMInvestorAccount : public CRMAccountNoKey
{
protected:
    int FDayTradeType;   //0：不可賣當日買股票，1：可賣當日買股票
    int FSelfTradeType;  //0：不檢查自行成交，　1：檢查自行成交
    int FSellPositionCheckType;  //0:不檢查賣出部位，1：檢查賣出部位
    
    UFC::PHashMap<UFC::AnsiString, CRMStockTradingData*> FStockNoIndex;
    
public:
    CRMInvestorAccount(const UFC::AnsiString& AccountNo, int DayTradeType, int SelfTradeType, int SellPositionCheckType);
    ~CRMInvestorAccount();
    
    int GetDayTradeType(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FDayTradeType, &FLock, RWLockType);};
    int GetSelfTradeType(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FSelfTradeType, &FLock, RWLockType);};
    int GetSellPositionCheckType(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FSellPositionCheckType, &FLock, RWLockType);};
    
    int GetStockDataCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return GetHMapElementCount(FStockNoIndex, &FLock, RWLockType);};
    CRMStockTradingData *FindStockData(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return FindElementFromHMap(FStockNoIndex, StockNo, &FLock, RWLockType);};
    CRMStockTradingData *FindOrCreateStockData(const UFC::AnsiString& StockNo, bool& IsNewStockData, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMStockTradingData *FindOrCreateStockData(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    bool InsertStockData(const UFC::AnsiString& StockNo, CRMStockTradingData *StockDataPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return InsertElementIntoHMap(FStockNoIndex, StockNo, StockDataPtr, DoCheckExist, &FLock, RWLockType);};
    int ClearStockData(bool DoDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return ClearElementHMap(FStockNoIndex, DoDelete, &FLock, RWLockType);};
    void DumpStockData(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMInvestorAccount

//------------------------------------------------------------------------------
class IBasicRiskManagerObject
{
protected:    
    static bool FIsRMControlSetting;
    static bool FNeedCheckSelfTrading;
    
public:
    static bool IsRMControlSetting() {return FIsRMControlSetting;};
    static bool NeedCheckSelfTrading() {return FNeedCheckSelfTrading;};
    
protected:
    UFC::PReadWriteLock FLock;
    UFC::AnsiString FRMName;
    UFC::AnsiString FConfigureFileName;
    UFC::AnsiString FRMFilePath;
    bool FIsTSEC;
    UFC::AnsiString FMarketName;
    
    UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*> FOrderIDIndex;
    
    UFC::PReadWriteLock FillRptExecLock;
    UFC::PHashedSet<UFC::AnsiString> FillRptExecKey;
    
    UFC::PHashMap<UFC::AnsiString, CRMInvestorAccount*> FInvestorAccountNoIndex;
    
public:
    IBasicRiskManagerObject(const UFC::AnsiString& RiskManagerName, const UFC::AnsiString& ConfigureFileName, bool IsTSEC);
    virtual ~IBasicRiskManagerObject();

    UFC::AnsiString GetRiskManagerName() {return FRMName;};
    UFC::AnsiString GetConfigureFileName() {return FConfigureFileName;};
    UFC::AnsiString GetRiskManagerFilePath() {return FRMFilePath;};
    
    virtual bool LoadSetting() = 0;
    virtual void PrintSetting() = 0;

    int GetOrderIDIndexCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return GetHMapElementCount(FOrderIDIndex, &FLock, RWLockType);};
    CRMOrderSummary *FindOrderFromIDIndex(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return FindElementFromHMap(FOrderIDIndex, OrderID, &FLock, RWLockType);};
    bool InsertOrderIntoIDIndex(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return InsertElementIntoHMap(FOrderIDIndex, OrderID, OrderSummaryPtr, DoCheckExist, &FLock, RWLockType);};    
    CRMOrderSummary *RemoveOrderFromIDIndex(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return RemoveElementFromHMap(FOrderIDIndex, OrderID, &FLock, RWLockType);};
    int ClearOrderIDIndex(bool DoDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return ClearElementHMap(FOrderIDIndex, DoDelete, &FLock, RWLockType);};

    void LockFillRptExec(UFC::RWLockTypeEnum LockType) {FillRptExecLock.Lock(LockType);};
    void UnlockFillRptExec(UFC::RWLockTypeEnum LockType) {FillRptExecLock.Unlock(LockType);};
    bool IsFillRptExecExist(const UFC::AnsiString& ExecKey, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    void AddFillRptExec(const UFC::AnsiString& ExecKey, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    
    int GetInvestorAccountNoCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetHMapElementCount(FInvestorAccountNoIndex, &FLock, RWLockType);};
    CRMInvestorAccount *FindInvestorAccountFromNoIndex(const UFC::AnsiString& AccountNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) 
        {return FindElementFromHMap(FInvestorAccountNoIndex, AccountNo, &FLock, RWLockType);};
    bool InsertInvestorAccountToNoIndex(const UFC::AnsiString& AccountNo, CRMInvestorAccount *AccountPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return InsertElementIntoHMap(FInvestorAccountNoIndex, AccountNo, AccountPtr, DoCheckExist, &FLock, RWLockType);};
    int ClearInvestorAccountNoIndex(bool DoDelete = cstNotDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return ClearElementHMap(FInvestorAccountNoIndex, DoDelete, &FLock, RWLockType);};
    void DumpInvestorAccount(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);    
    
    virtual bool CheckRiskCondition(TWSE::MessageHeader*& Msg, OrderRecord& OI, const UFC::AnsiString& PVCID, UFC::AnsiString& CheckMessage);
    virtual void AddRiskMatchQty(TWSE::R3Body *R3MsgBodyPtr, const UFC::AnsiString& PVCId, const UFC::AnsiString& ExecKey) {};
    virtual void ProcessRiskCancelReplaceMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI, const UFC::AnsiString PVCID) {};
    virtual void ProcessRiskRejectMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI, const UFC::AnsiString PVCID) {};
    
    virtual void ParseExecRtpLogStr(const UFC::AnsiString& ExecRptLogStr, bool IsSynchronize = false) {};
    virtual void RecoverRMData(BOOL IsTSE) {};
};  //IBasicRiskManagerObject
extern IBasicRiskManagerObject *rmObjPtr;
}  //rm
#endif /* RISKMANAGERABSTRACT_H */

