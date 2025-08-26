/* 
 * File:   RiskManagerAbstract.h
 * Author: Zhen Fan
 *
 * Created on 2016/02/03, 10:02
 */

#include "RiskManagerAbstract.h"

namespace rm
{
IBasicRiskManagerObject *rmObjPtr;
UFC::PCriticalSection RMLogFileLock;
UFC::FileStreamEx* RMLogFilePtr;
Int32 RMLogLevel;

//------------------------------------------------------------------------------
UFC::AnsiString DescribeBoolean(bool booleanValue)
{
    UFC::AnsiString boolStr = "";
    if (booleanValue)
        boolStr = "true";
    else
        boolStr = "false";
    return boolStr;
}  //DescribeBoolean()
//------------------------------------------------------------------------------
UFC::AnsiString DescribeBoolean(BOOL boolValue)
{
    UFC::AnsiString boolStr = "";
    if (boolValue)
        boolStr = "TRUE";
    else
        boolStr = "FALSE";
    return boolStr;
}  //DescribeBoolean()
//------------------------------------------------------------------------------
double Parse94v99PriceStr(const UFC::AnsiString& PriceStr)
{
    double price = 0.0;
    if (PriceStr.Length() != 6) return price;
    UFC::AnsiString intPartStr = PriceStr.SubString(0, 4);
    UFC::AnsiString fractionPartStr = PriceStr.SubString(4, 2);
    price = intPartStr.ToDouble() + fractionPartStr.ToDouble() / 100.0;
    return price;
}  //Parse94v99PriceStr()
//------------------------------------------------------------------------------
double Parse95v9999PriceStr(const UFC::AnsiString& PriceStr)
{
    double price = 0.0;
    if (PriceStr.Length() != 9) return price;
    UFC::AnsiString intPartStr = PriceStr.SubString(0, 5);
    UFC::AnsiString fractionPartStr = PriceStr.SubString(5, 4);
    price = intPartStr.ToDouble() + fractionPartStr.ToDouble() / 10000.0;
    return price;
}  //Parse95v9999PriceStr()
//------------------------------------------------------------------------------
bool OpenRiskLogFile(BOOL IsTSEC, const UFC::AnsiString& RiskManagerName, const UFC::AnsiString& LogFilePath)
{
    UFC::AnsiString toDayStr;
    UFC::GetYYYYMMDD(toDayStr, FALSE);
    UFC::AnsiString marketName = "";
    if (IsTSEC == TRUE)
        marketName = "TSE";
    else
        marketName = "OTC";
    UFC::AnsiString fullFileName;
    
    if (LogFilePath.Length() > 0)
    {
        if (LogFilePath[LogFilePath.Length() - 1] != '/')
            fullFileName.Printf("%s/RM%s%s.%s.log", LogFilePath.c_str(), RiskManagerName.c_str(), marketName.c_str(), toDayStr.c_str());
        else
            fullFileName.Printf("%sRM%s%s.%s.log", LogFilePath.c_str(), RiskManagerName.c_str(), marketName.c_str(), toDayStr.c_str());
    }
    else
        fullFileName.Printf("RM%s%s%s.log", RiskManagerName.c_str(), marketName.c_str(), toDayStr.c_str());
    
    try
    {
        if (UFC::FileExists(fullFileName) == TRUE) 
            RMLogFilePtr = new UFC::FileStreamEx(fullFileName, "a");
        else
            RMLogFilePtr = new UFC::FileStreamEx(fullFileName, "w");
        
        WriteRiskLog("", " ");
        WriteRiskLog("", " ");
        WriteRiskLog("", "=============== %s %s Risk Manager Start ===============", UFC::Hostname, RiskManagerName.c_str());
        FlushRiskLog();
        return true;
    }
    catch(UFC::FileException& e)
    {
        UFC::BufferedLog::Printf(" OpenRiskLogFile() FileException:%s", e.what());
        RMLogFilePtr = 0;
    }
    return false;
}  //OpenRiskLogFile()
//------------------------------------------------------------------------------
void SetRiskLogLevel(Int32 NewValue) 
{
    RMLogLevel = NewValue;
}  //SetRiskLogLevel()
//------------------------------------------------------------------------------
bool NeedWriteRiskLog(Int32 RiskLogLevel)
{
    bool isNeeded = false;
    RMLogFileLock.Acquire();
    Int32 checkResult = RiskLogLevel & RMLogLevel;
    if (checkResult == 0)
        isNeeded = false;
    else
        isNeeded = true;
    RMLogFileLock.Release();
    return isNeeded;
}  //NeedWriteRiskLog()
//------------------------------------------------------------------------------
void WriteRiskLog(const UFC::AnsiString& LogHeadStr, const char* FormatStr ...)
{
    if (RMLogFilePtr == 0) return;
    va_list va;
    va_start(va, FormatStr);
    const int outputBufferSize = 2048;
    char outputBuffer[outputBufferSize];
    int outputCount = vsnprintf(outputBuffer, outputBufferSize, FormatStr, va);
    va_end(va);
    if (outputCount <= 0) 
    {
        UFC::BufferedLog::Printf(" WriteLevelRiskLog() failed, LogHeadStr[%s], vsnprintf() return %d.", LogHeadStr.c_str(), outputCount);
        return;
    }
    
    char space = ' ';
    char newLine = '\n';
    char timeBuffer[20];
    UFC::GetTimeString(timeBuffer, TRUE);
    RMLogFileLock.Acquire();
    try
    {
        RMLogFilePtr->Write(timeBuffer, 12);
        RMLogFilePtr->Write(&space, 1);
        if (LogHeadStr.Length() > 0)
        {
            RMLogFilePtr->Write(LogHeadStr.c_str(), LogHeadStr.Length());
            RMLogFilePtr->Write(&space, 1);
        }
        RMLogFilePtr->Write(outputBuffer, outputCount);
        RMLogFilePtr->Write(&newLine, 1);
    }
    catch(UFC::FileException& e)
    {
        UFC::BufferedLog::Printf(" WriteLevelRiskLog() FileException:%s", e.what());
    }
    RMLogFileLock.Release();
}  //WriteRiskLog()
//------------------------------------------------------------------------------
void WriteLevelRiskLog(Int32 LogLevel, const UFC::AnsiString& LogHeadStr, const char* FormatStr ...)
{
    if (RMLogFilePtr == 0) return;
    if (!NeedWriteRiskLog(LogLevel)) return;
    va_list va;
    va_start(va, FormatStr);
    const int outputBufferSize = 2048;
    char outputBuffer[outputBufferSize];
    int outputCount = vsnprintf(outputBuffer, outputBufferSize, FormatStr, va);
    va_end(va);
    if (outputCount <= 0) 
    {
        UFC::BufferedLog::Printf(" WriteLevelRiskLog() failed, LogHeadStr[%s], vsnprintf() return %d.", LogHeadStr.c_str(), outputCount);
        return;
    }
    
    char space = ' ';
    char newLine = '\n';
    char timeBuffer[20];
    UFC::GetTimeString(timeBuffer, TRUE);
    RMLogFileLock.Acquire();
    try
    {
        RMLogFilePtr->Write(timeBuffer, 12);
        RMLogFilePtr->Write(&space, 1);
        if (LogHeadStr.Length() > 0)
        {
            RMLogFilePtr->Write(LogHeadStr.c_str(), LogHeadStr.Length());
            RMLogFilePtr->Write(&space, 1);
        }
        RMLogFilePtr->Write(outputBuffer, outputCount);
        RMLogFilePtr->Write(&newLine, 1);
    }
    catch(UFC::FileException& e)
    {
        UFC::BufferedLog::Printf(" WriteLevelRiskLog() FileException:%s", e.what());
    }
    RMLogFileLock.Release();
}  //WriteLevelRiskLog()
//------------------------------------------------------------------------------
void FlushRiskLog()
{
    RMLogFileLock.Acquire();
    if (RMLogFilePtr != 0) RMLogFilePtr->Flush();  
    RMLogFileLock.Release();
}  //FlushRiskLog()
//------------------------------------------------------------------------------
void CloseRiskLogFile()
{
    RMLogFileLock.Acquire();
    if (RMLogFilePtr != 0) 
    {
        RMLogFilePtr->Flush();
        delete RMLogFilePtr;
        RMLogFilePtr = 0;
    }
    RMLogFileLock.Release();
}  //CloseRiskLogFile()

//------------------------------- CRMBasicObject -------------------------------
//------------------------------------------------------------------------------
CRMBasicObject::CRMBasicObject()
:FIndexKey("")
{
}  //CRMBasicObject::CRMBasicObject()
//------------------------------------------------------------------------------
CRMBasicObject::CRMBasicObject(const UFC::AnsiString& IndexKey)
:FIndexKey(IndexKey)
{
}  //CRMBasicObject::CRMBasicObject()
//------------------------------------------------------------------------------
bool CRMBasicObject::IncreaseMemberValue(int IncreaseValue, int& Member, bool CheckNegative, UFC::RWLockTypeEnum RWLockType)
{
    bool isSuccess = true;
    FLock.Lock(RWLockType);
    int newValue = Member + IncreaseValue;
    if (CheckNegative && (newValue < 0))
    {
        newValue = 0;
        isSuccess = false;
    }
    Member = newValue;
    FLock.Unlock(RWLockType);
    return isSuccess;
}  //CRMBasicObject::IncreaseMemberValue()
//------------------------------------------------------------------------------
bool CRMBasicObject::IncreaseMemberValue(double IncreaseValue, double& Member, bool CheckNegative, UFC::RWLockTypeEnum RWLockType)
{
    bool isSuccess = true;
    FLock.Lock(RWLockType);
    double newValue = Member + IncreaseValue;
    if (CheckNegative && (newValue < 0.0))
    {
        newValue = 0.0;
        isSuccess = false;
    }
    Member = newValue;
    FLock.Unlock(RWLockType);
    return isSuccess;
}  //CRMBasicObject::IncreaseMemberValue()

//------------------------------- CRMAccountNoKey ------------------------------
//------------------------------------------------------------------------------
CRMAccountNoKey::CRMAccountNoKey()
:CRMBasicObject()
,FAccountNo("")
{
}  //CRMAccountNoKey::CRMAccountNoKey()
//------------------------------------------------------------------------------
CRMAccountNoKey::CRMAccountNoKey(const UFC::AnsiString& AccountNo)
:CRMBasicObject(AccountNo)
,FAccountNo(AccountNo)
{
}  //CRMAccountNoKey::CRMAccountNoKey()

//---------------------------- CRMAccountStockNoKey ----------------------------
UFC::AnsiString CRMAccountStockNoKey::BuildIndexKey(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo)
{
    UFC::AnsiString indexKey = AccountNo + StockNo;
    return indexKey;
}  //CRMAccountStockNoKey::BuildIndexKey()
//------------------------------------------------------------------------------
CRMAccountStockNoKey::CRMAccountStockNoKey()
:CRMAccountNoKey()
,FStockNo("")
{
}  //CRMAccountStockNoKey::CRMAccountStockNoKey()
//------------------------------------------------------------------------------
CRMAccountStockNoKey::CRMAccountStockNoKey(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo)
:CRMAccountNoKey()
,FStockNo(StockNo)
{
    FAccountNo = AccountNo;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo);
}  //CRMAccountStockNoKey::CRMAccountStockNoKey()
//------------------------------------------------------------------------------
void CRMAccountStockNoKey::SetAccountNo(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    FAccountNo = NewValue;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo);
    FLock.Unlock(RWLockType);
}  //CRMAccountStockNoKey::SetAccountNo()
//------------------------------------------------------------------------------
void CRMAccountStockNoKey::SetStockNo(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    FStockNo = NewValue;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo);
    FLock.Unlock(RWLockType);
}  //CRMAccountStockNoKey::SetStockNo()
//------------------------------------------------------------------------------
UFC::AnsiString CRMAccountStockNoKey::ToString(UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString objStr = "";
    FLock.Lock(RWLockType);
    objStr.Printf("Account[%s] Stock[%s]", FAccountNo.c_str(), FStockNo.c_str());
    FLock.Unlock(RWLockType);
    return objStr;
}  //CRMAccountStockNoKey::ToString()

//------------------------------ CRMOrderSummary -------------------------------
//------------------------------------------------------------------------------
CRMOrderSummary::CRMOrderSummary(const UFC::AnsiString& OrderId, char Side, const UFC::AnsiString& StockNo,
                                 const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char OrderType, 
                                 double OrderPrice, int OrderQty, char ExchangeCode, TWSE::TMessageType MsgType, const UFC::AnsiString& AccountNo)
:CRMBasicObject()
,FOrderId(OrderId)
,FSide(Side)
,FStockNo(StockNo)
,FOrderPrice(OrderPrice)
,FOrderQty(OrderQty)
,FLeavesQty(OrderQty)
,FWarrantNo(WarrantNo)
,FBrokerId(BrokerId)
,FTWSEOrderType(OrderType)
,FExchangeCode(ExchangeCode)
,FMsgType(MsgType)
,FAccountNo(AccountNo)
,FOrderAmount(0.0)
,FOrderKey("")
,FTraderID("")
{
}  //CRMOrderSummary::CRMOrderSummary()
//------------------------------------------------------------------------------
UFC::AnsiString CRMOrderSummary::ToString(UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString objStr = "";
    FLock.Lock(RWLockType);
    objStr.Printf("OrderId[%s] Stock[%s][%s][%s]%c %c OrdPx[%10.4lf] OrdQty[%d] LeavesQty[%d] ExCode[%c] Account[%s] Amount[%15.4lf] OrderKey[%s]", 
                  FOrderId.c_str(), FStockNo.c_str(), FWarrantNo.c_str(), FBrokerId.c_str(), FTWSEOrderType, FSide, FOrderPrice, FOrderQty, FLeavesQty, FExchangeCode, FAccountNo.c_str(), FOrderAmount, FOrderKey.c_str());
    FLock.Unlock(RWLockType);
    return objStr;
}  //CRMOrderSummary::ToString()

//---------------------------- CRMStockTradingData -----------------------------
double CRMStockTradingData::FInitialMinOrderPrice = 9999999.0;

//------------------------------------------------------------------------------
CRMStockTradingData::CRMStockTradingData(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo)
:CRMAccountStockNoKey(AccountNo, StockNo)
,FMaxNormalBuyOrderPtr(0)
,FMaxNormalBuyOrderPrice(0.0)
,FMinNormalSellOrderPtr(0)
,FMinNormalSellOrderPrice(FInitialMinOrderPrice)
,FMaxBuyOrderPtr(0)
,FMaxBuyOrderPrice(0.0)
,FMinSellOrderPtr(0)
,FMinSellOrderPrice(FInitialMinOrderPrice)
,FBuyAmount(0.0)
,FSellAmount(0.0)
{
}  //CRMStockTradingData::CRMStockTradingData()
//------------------------------------------------------------------------------
CRMStockTradingData::~CRMStockTradingData()
{
    FLock.LockForWrite();
    ClearNormalBuyOrder(cstNotDelete, UFC::rwNotLock);
    ClearNormalSellOrder(cstNotDelete, UFC::rwNotLock);
    ClearBuyOrder(cstNotDelete, UFC::rwNotLock);
    ClearSellOrder(cstNotDelete, UFC::rwNotLock);
    FLock.UnlockForWrite();
}  //CRMStockTradingData::~CRMStockTradingData()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::FindMaxOrderPriceOrder(UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*>& OrderIdIndex, UFC::RWLockTypeEnum RWLockType)
{
    double maxOrderPrice = 0.0;
    CRMOrderSummary *maxOrderPtr = 0;
    FLock.Lock(RWLockType);
    CRMOrderSummary *curOrderPtr = OrderIdIndex.First();
    while(curOrderPtr != 0)
    {
        double curOrderPrice = curOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        if (curOrderPrice > maxOrderPrice)
        {
            maxOrderPtr = curOrderPtr;
            maxOrderPrice = curOrderPrice;
        }
        curOrderPtr = OrderIdIndex.Next();
    }  //while(curOrderPtr != 0)
    FLock.Unlock(RWLockType);
    return maxOrderPtr;
}  //CRMStockTradingData::FindMaxOrderPriceOrder()
//------------------------------------------------------------------------------
double CRMStockTradingData::FindMaxNormalBuyOrderPrice(UFC::RWLockTypeEnum RWLockType)
{
    double maxBuyOrderPrice = 0.0;
    CRMOrderSummary *maxOrderPtr = FindMaxOrderPriceOrder(FNormalBuyOrderIdIndex, RWLockType);
    if (maxOrderPtr != 0) maxBuyOrderPrice = maxOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return maxBuyOrderPrice;
}  //CRMStockTradingData::FindMaxNormalBuyOrderPrice()
//------------------------------------------------------------------------------
double CRMStockTradingData::FindMaxBuyOrderPrice(UFC::RWLockTypeEnum RWLockType)
{
    double maxBuyOrderPrice = 0.0;
    CRMOrderSummary *maxOrderPtr = FindMaxOrderPriceOrder(FBuyOrderIdIndex, RWLockType);
    if (maxOrderPtr != 0) maxBuyOrderPrice = maxOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return maxBuyOrderPrice;
}  //CRMStockTradingData::FindMaxBuyOrderPrice()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::FindMinOrderPriceOrder(UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*>& OrderIdIndex, UFC::RWLockTypeEnum RWLockType)
{
    double minOrderPrice = FInitialMinOrderPrice;
    CRMOrderSummary *minOrderPtr = 0;
    FLock.Lock(RWLockType);
    CRMOrderSummary *curOrderPtr = OrderIdIndex.First();
    while(curOrderPtr != 0)
    {
        double curOrderPrice = curOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        if (curOrderPrice < minOrderPrice)
        {
            minOrderPtr = curOrderPtr;
            minOrderPrice = curOrderPrice;
        }
        curOrderPtr = static_cast<CRMOrderSummary*>(OrderIdIndex.Next());
    }  //while(curOrderPtr != 0)
    FLock.Unlock(RWLockType);
    return minOrderPtr;
}  //CRMStockTradingData::FindMinOrderPriceOrder()
//------------------------------------------------------------------------------
double CRMStockTradingData::FindMinNormalSellOrderPrice(UFC::RWLockTypeEnum RWLockType)
{
    double minSellOrderPrice = FInitialMinOrderPrice;
    CRMOrderSummary *minOrderPtr = FindMinOrderPriceOrder(FNormalSellOrderIdIndex, RWLockType);
    if (minOrderPtr != 0) minSellOrderPrice = minOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return minSellOrderPrice;
}  //CRMStockTradingData::FindMinNormalSellOrderPrice()
//------------------------------------------------------------------------------
double CRMStockTradingData::FindMinSellOrderPrice(UFC::RWLockTypeEnum RWLockType)
{
    double minSellOrderPrice = FInitialMinOrderPrice;
    CRMOrderSummary *minOrderPtr = FindMinOrderPriceOrder(FSellOrderIdIndex, RWLockType);
    if (minOrderPtr != 0) minSellOrderPrice = minOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return minSellOrderPrice;
}  //CRMStockTradingData::FindMinSellOrderPrice()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertNormalBuyOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    bool isSuccess = InsertElementIntoHMap(FNormalBuyOrderIdIndex, OrderID, OrderSummaryPtr, DoCheckExist, &FLock, UFC::rwNotLock);
    if (isSuccess)
    {
        double orderPrice = OrderSummaryPtr->GetOrderPrice(UFC::rwLockForRead);
        if (orderPrice > FMaxNormalBuyOrderPrice) 
        {
            FMaxNormalBuyOrderPrice = orderPrice;
            FMaxNormalBuyOrderPtr = OrderSummaryPtr;
        }
    }
    FLock.Unlock(RWLockType);
    return isSuccess;
}  //CRMStockTradingData::InsertNormalBuyOrder()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::RemoveNormalBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    CRMOrderSummary *targetOrderPtr = RemoveElementFromHMap(FNormalBuyOrderIdIndex, OrderID, &FLock, UFC::rwNotLock);
    if (targetOrderPtr != 0) 
    {
        FMaxNormalBuyOrderPtr = FindMaxOrderPriceOrder(FNormalBuyOrderIdIndex, UFC::rwNotLock);
        if (FMaxNormalBuyOrderPtr != 0) 
            FMaxNormalBuyOrderPrice = FMaxNormalBuyOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        else
            FMaxNormalBuyOrderPrice = 0.0;
    }
    FLock.Unlock(RWLockType);
    return targetOrderPtr;
}  //CRMStockTradingData::RemoveNormalBuyOrder()
//------------------------------------------------------------------------------
void CRMStockTradingData::DumpNormalBuyOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    UFC::BufferedLog::Printf(" %d Normal Buy Order", FNormalBuyOrderIdIndex.ItemCount());
    DumpRMObjHMap(FNormalBuyOrderIdIndex, DoFlush, &FLock, UFC::rwNotLock);
    FLock.Unlock(RWLockType);
}  //CRMStockTradingData::DumpNormalBuyOrder()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertNormalSellOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    bool isSuccess = InsertElementIntoHMap(FNormalSellOrderIdIndex, OrderID, OrderSummaryPtr, DoCheckExist, &FLock, UFC::rwNotLock);
    if (isSuccess)
    {
        double orderPrice = OrderSummaryPtr->GetOrderPrice(UFC::rwLockForRead);
        if (orderPrice < FMinNormalSellOrderPrice) 
        {
            FMinNormalSellOrderPrice = orderPrice;
            FMinNormalSellOrderPtr = OrderSummaryPtr;
        }
    }
    FLock.Unlock(RWLockType);
    return isSuccess;
}  //CRMStockTradingData::InsertNormalSellOrder()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::RemoveNormalSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    CRMOrderSummary *targetOrderPtr = RemoveElementFromHMap(FNormalSellOrderIdIndex, OrderID, &FLock, UFC::rwNotLock);
    if (targetOrderPtr != 0) 
    {
        FMinNormalSellOrderPtr = FindMinOrderPriceOrder(FNormalSellOrderIdIndex, UFC::rwNotLock);
        if (FMinNormalSellOrderPtr != 0) 
            FMinNormalSellOrderPrice = FMinNormalSellOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        else
            FMinNormalSellOrderPrice = FInitialMinOrderPrice;
    }
    FLock.Unlock(RWLockType);
    return targetOrderPtr;
}  //CRMStockTradingData::RemoveNormalSellOrder()
//------------------------------------------------------------------------------
void CRMStockTradingData::DumpNormalSellOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    UFC::BufferedLog::Printf(" %d Normal Sell Order", FNormalSellOrderIdIndex.ItemCount());
    DumpRMObjHMap(FNormalSellOrderIdIndex, DoFlush, &FLock, UFC::rwNotLock);
    FLock.Unlock(RWLockType);
}  //CRMStockTradingData::DumpNormalSellOrder()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertBuyOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    bool isSuccess = InsertElementIntoHMap(FBuyOrderIdIndex, OrderID, OrderSummaryPtr, DoCheckExist, &FLock, UFC::rwNotLock);
    if (isSuccess)
    {
        double orderPrice = OrderSummaryPtr->GetOrderPrice(UFC::rwLockForRead);
        if (orderPrice > FMaxBuyOrderPrice) 
        {
            FMaxBuyOrderPrice = orderPrice;
            FMaxBuyOrderPtr = OrderSummaryPtr;
        }
    }
    FLock.Unlock(RWLockType);
    return isSuccess;
}  //CRMStockTradingData::InsertBuyOrder()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::RemoveBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    CRMOrderSummary *targetOrderPtr = RemoveElementFromHMap(FBuyOrderIdIndex, OrderID, &FLock, UFC::rwNotLock);
    if (targetOrderPtr != 0) 
    {
        FMaxBuyOrderPtr = FindMaxOrderPriceOrder(FBuyOrderIdIndex, UFC::rwNotLock);
        if (FMaxBuyOrderPtr != 0) 
            FMaxBuyOrderPrice = FMaxBuyOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        else
            FMaxBuyOrderPrice = 0.0;
    }
    FLock.Unlock(RWLockType);
    return targetOrderPtr;
}  //CRMStockTradingData::RemoveBuyOrder()
//------------------------------------------------------------------------------
void CRMStockTradingData::DumpBuyOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    UFC::BufferedLog::Printf(" %d Buy Order", FBuyOrderIdIndex.ItemCount());
    DumpRMObjHMap(FBuyOrderIdIndex, DoFlush, &FLock, RWLockType);
    FLock.Unlock(RWLockType);
}  //CRMStockTradingData::DumpBuyOrder()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertSellOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    bool isSuccess = InsertElementIntoHMap(FSellOrderIdIndex, OrderID, OrderSummaryPtr, DoCheckExist, &FLock, UFC::rwNotLock);
    if (isSuccess)
    {
        double orderPrice = OrderSummaryPtr->GetOrderPrice(UFC::rwLockForRead);
        if (orderPrice < FMinSellOrderPrice) 
        {
            FMinSellOrderPrice = orderPrice;
            FMinSellOrderPtr = OrderSummaryPtr;
        }
    }
    FLock.Unlock(RWLockType);
    return isSuccess;
}  //CRMStockTradingData::InsertSellOrder()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::RemoveSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    CRMOrderSummary *targetOrderPtr = RemoveElementFromHMap(FSellOrderIdIndex, OrderID, &FLock, UFC::rwNotLock);
    if (targetOrderPtr != 0) 
    {
        FMinSellOrderPtr = FindMinOrderPriceOrder(FSellOrderIdIndex, UFC::rwNotLock);
        if (FMinSellOrderPtr != 0) 
            FMinSellOrderPrice = FMinSellOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        else
            FMinSellOrderPrice = FInitialMinOrderPrice;
    }
    FLock.Unlock(RWLockType);
    return targetOrderPtr;
}  //CRMStockTradingData::RemoveSellOrder()
//------------------------------------------------------------------------------
void CRMStockTradingData::DumpSellOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    UFC::BufferedLog::Printf(" %d Sell Order", FSellOrderIdIndex.ItemCount());
    DumpRMObjHMap(FSellOrderIdIndex, DoFlush, &FLock, UFC::rwNotLock);
    FLock.Unlock(RWLockType);
}  //CRMStockTradingData::DumpSellOrder()
//------------------------------------------------------------------------------
UFC::AnsiString CRMStockTradingData::ToString(UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString objStr = "";
    FLock.Lock(RWLockType);
    UFC::AnsiString normalMaxBuyStr;
    if (FMaxNormalBuyOrderPtr != 0)
        normalMaxBuyStr.Printf("N:%s:%10.4lf", FMaxNormalBuyOrderPtr->GetOrderId(UFC::rwLockForRead).c_str(), FMaxNormalBuyOrderPrice);
    else
        normalMaxBuyStr.Printf("N:%10.4lf", FMaxNormalBuyOrderPrice);
        
    UFC::AnsiString maxBuyStr;
    if (FMaxBuyOrderPtr != 0)
        maxBuyStr.Printf("%s:%10.4lf", FMaxBuyOrderPtr->GetOrderId(UFC::rwLockForRead).c_str(), FMaxBuyOrderPrice);
    else
        maxBuyStr.Printf("%10.4lf", FMaxBuyOrderPrice);

    UFC::AnsiString normalMinSellStr;
    if (FMinNormalSellOrderPtr != 0)
        normalMinSellStr.Printf("N:%s:%10.4lf", FMinNormalSellOrderPtr->GetOrderId(UFC::rwLockForRead).c_str(), FMinNormalSellOrderPrice);
    else
        normalMinSellStr.Printf("N:%10.4lf", FMinNormalSellOrderPrice);
    
    UFC::AnsiString minSellStr;
    if (FMinSellOrderPtr != 0)
        minSellStr.Printf("%s:%10.4lf", FMinSellOrderPtr->GetOrderId(UFC::rwLockForRead).c_str(), FMinSellOrderPrice);
    else
        minSellStr.Printf("%10.4lf", FMinSellOrderPrice);
    
    objStr.Printf("Acc[%s] Stock[%s] MaxBuy[%s, %s] MinSell[%s, %s] BuyAmt[%15.4lf] SellAmt[%15.4lf]", FAccountNo.c_str(), FStockNo.c_str(), normalMaxBuyStr.c_str(), maxBuyStr.c_str(), normalMinSellStr.c_str(), minSellStr.c_str(), FBuyAmount, FSellAmount);
    FLock.Unlock(RWLockType);
    return objStr;
}  //CRMStockTradingData::ToString()
//------------------------------------------------------------------------------
void CRMStockTradingData::DumpData(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    UFC::AnsiString objStr = ToString(UFC::rwNotLock);
    UFC::BufferedLog::Printf(" %s", objStr.c_str());
    DumpBuyOrder(cstNotFlush, UFC::rwNotLock);
    DumpSellOrder(cstNotFlush, UFC::rwNotLock);
    FLock.Unlock(RWLockType);
    if (DoFlush) UFC::BufferedLog::FlushToFile();
}  //CRMStockTradingData::DumpData()

//----------------------------- CRMInvestorAccount -----------------------------
//------------------------------------------------------------------------------
CRMInvestorAccount::CRMInvestorAccount(const UFC::AnsiString& AccountNo, int DayTradeType, int SelfTradeType, int SellPositionCheckType)
:CRMAccountNoKey(AccountNo)
,FDayTradeType(DayTradeType)
,FSelfTradeType(SelfTradeType)
,FSellPositionCheckType(SellPositionCheckType)
{    
}  //CRMInvestorAccount::CRMInvestorAccount()
//------------------------------------------------------------------------------
CRMInvestorAccount::~CRMInvestorAccount()
{
}  //CRMInvestorAccount::~CRMInvestorAccount()
//------------------------------------------------------------------------------
CRMStockTradingData *CRMInvestorAccount::FindOrCreateStockData(const UFC::AnsiString& StockNo, bool& IsNewStockData, UFC::RWLockTypeEnum RWLockType)
{
    IsNewStockData = false;
    Lock(RWLockType);
    CRMStockTradingData *stockDataPtr = FindStockData(StockNo, UFC::rwNotLock);
    if (stockDataPtr == 0)
    {
        stockDataPtr = new CRMStockTradingData(FAccountNo, StockNo);
        if (InsertStockData(StockNo, stockDataPtr, cstNotCheck, UFC::rwNotLock))
            IsNewStockData = true;
        else
        {
            UFC::BufferedLog::Printf(" FindOrCreateStockData() Insert Stock[%s] data into Account[%s] failed.", StockNo.c_str(), FAccountNo.c_str());
            delete stockDataPtr;
            stockDataPtr = 0;
        }
    }  //if (stockDataPtr == 0)
    Unlock(RWLockType);
    return stockDataPtr;
}  //CRMInvestorAccount::FindOrCreateStockData()
//------------------------------------------------------------------------------
CRMStockTradingData *CRMInvestorAccount::FindOrCreateStockData(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType)
{
    bool isNewStockData = false;
    return FindOrCreateStockData(StockNo, isNewStockData, RWLockType);
}  //CRMInvestorAccount::FindOrCreateStockData()
//------------------------------------------------------------------------------
void CRMInvestorAccount::DumpStockData(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    UFC::BufferedLog::Printf("Dump %d Stock Data", FStockNoIndex.ItemCount());
    DumpRMObjHMap(FStockNoIndex, cstNotFlush, &FLock, UFC::rwNotLock);
    FLock.Unlock(RWLockType);
    if (DoFlush) UFC::BufferedLog::FlushToFile();
}  //CRMInvestorAccount::DumpStockData()
//------------------------------------------------------------------------------
UFC::AnsiString CRMInvestorAccount::ToString(UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString objStr = "";
    FLock.Lock(RWLockType);
    objStr.Printf("Account[%s] DayTrade[%d] SelfTrade[%d] SellPositionCheck[%d]", FAccountNo.c_str(), FDayTradeType, FSelfTradeType, FSellPositionCheckType);
    FLock.Unlock(RWLockType);
    return objStr;
}  //CRMInvestorAccount::ToString()

//-------------------------- IBasicRiskManagerObject ---------------------------
bool IBasicRiskManagerObject::FIsRMControlSetting = false;
bool IBasicRiskManagerObject::FNeedCheckSelfTrading = false;

//------------------------------------------------------------------------------
IBasicRiskManagerObject::IBasicRiskManagerObject(const UFC::AnsiString& RiskManagerName, const UFC::AnsiString& ConfigureFileName, bool IsTSEC)
:FRMName(RiskManagerName)
,FConfigureFileName(ConfigureFileName)
,FRMFilePath("../cfg")
,FIsTSEC(IsTSEC)
,FMarketName("")
{
    if (FIsTSEC) 
        FMarketName = "TSE";
    else
        FMarketName = "OTC";
}  //IBasicRiskManagerObject::IBasicRiskManagerObject()
//------------------------------------------------------------------------------
IBasicRiskManagerObject::~IBasicRiskManagerObject()
{
}  //IBasicRiskManagerObject::~IBasicRiskManagerObject()

//----------------------------------------------------------------------------------------------------------------------
bool IBasicRiskManagerObject::IsFillRptExecExist(const UFC::AnsiString& ExecKey, UFC::RWLockTypeEnum LockType)
{
    bool isExist = false;
    FillRptExecLock.Lock(LockType);
    if (FillRptExecKey.Exists(ExecKey) == TRUE) isExist = true;
    FillRptExecLock.Unlock(LockType);
    return isExist;
}  //IBasicRiskManagerObject::IsFillRptExecExist()
//----------------------------------------------------------------------------------------------------------------------
void IBasicRiskManagerObject::AddFillRptExec(const UFC::AnsiString& ExecKey, UFC::RWLockTypeEnum LockType)
{
    if (ExecKey.Length() <= 0) return;
    FillRptExecLock.Lock(LockType);
    FillRptExecKey.Add(ExecKey);
    FillRptExecLock.Unlock(LockType);
}  //IBasicRiskManagerObject::AddFillRptExec()
//------------------------------------------------------------------------------
void IBasicRiskManagerObject::DumpInvestorAccount(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    WriteRiskLog("", "Dump %d InvestorAccount Data", FInvestorAccountNoIndex.ItemCount());
    CRMInvestorAccount *curAccountPtr = FInvestorAccountNoIndex.First();
    while (curAccountPtr != 0)
    {
        WriteRiskLog("", "%s", curAccountPtr->ToString(UFC::rwLockForRead).c_str());
        curAccountPtr = FInvestorAccountNoIndex.Next();
    }
    if (DoFlush) FlushRiskLog();
    FLock.Unlock(RWLockType);
}  //IBasicRiskManagerObject::DumpInvestorAccount()
//------------------------------------------------------------------------------
bool IBasicRiskManagerObject::CheckRiskCondition(TWSE::MessageHeader*& Msg, OrderRecord& OI, const UFC::AnsiString& PVCID, UFC::AnsiString& CheckMessage)
{
    return true;
}  //SpeedyOrderConnectionObject::CheckRiskCondition()

}  //rm
