/* 
 * File:   RiskManagerObjects.cpp
 * Author: Zhen Fan
 *
 * Created on 2014/11/14 12:11
 */
#include <stdio.h>
#include <stdarg.h>
#include "RiskManagerObjects.h"

//------------------------------------------------------------------------------
UFC::FileStreamEx *rmLogFilePtr = 0;
UFC::PCriticalSection rmLogFileLock;
Int32 rmLogLevel = 0;
bool rmNeedCheckDataDate = true;

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
void ParseRMDataFromUserData(const UFC::AnsiString& Key, UFC::AnsiString& WarrantNo, UFC::AnsiString& BrokerId)
{
    WarrantNo = "";
    BrokerId = "";
    int keyLength = Key.Length();
    if (keyLength <= 0) return;

    int commaPos = Key.AnsiPos(',');
    if (commaPos < 0) return;
    
    int userDataLength = keyLength - (commaPos + 1);
    if (userDataLength <= 0) return;        
    UFC::AnsiString userData = Key.SubString(commaPos + 1, userDataLength);
    
    UFC::PStringList riskManagerFieldList;
    riskManagerFieldList.SetStrings(userData, ";\n");
    
    if (riskManagerFieldList.ItemCount() >= 1) WarrantNo = riskManagerFieldList[0];
    if (riskManagerFieldList.ItemCount() >= 2) BrokerId = riskManagerFieldList[1];
}  //ParseRMDataFromUserData()
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
void SetNeedCheckRiskDataDate(bool NeedCheckDataDate)
{
    rmNeedCheckDataDate = NeedCheckDataDate;
}  //SetNeedCheckDataDate()
//------------------------------------------------------------------------------
bool NeedCheckRiskDataDate()
{
    return rmNeedCheckDataDate;
}  //NeedCheckDataDate()
//------------------------------------------------------------------------------
bool OpenRiskLogFile(BOOL IsTWSE, const UFC::AnsiString& LogFilePath)
{
    UFC::AnsiString fileNamePrefix = "";
    if (IsTWSE == TRUE)
        fileNamePrefix = "rmTSE";
    else
        fileNamePrefix = "rmOTC";
    UFC::AnsiString fullFileName;
    UFC::AnsiString toDayStr;
    UFC::GetYYYYMMDD(toDayStr, FALSE);
    
    if (LogFilePath.Length() > 0)
    {
        if (LogFilePath[LogFilePath.Length() - 1] != '/')
            fullFileName.Printf("%s/%s.%s.log", LogFilePath.c_str(), fileNamePrefix.c_str(), toDayStr.c_str());
        else
            fullFileName.Printf("%s%s.%s.log", LogFilePath.c_str(), fileNamePrefix.c_str(), toDayStr.c_str());
    }
    else
        fullFileName.Printf("rm%s.log", toDayStr.c_str());
    
    try
    {
        if (UFC::FileExists(fullFileName) == TRUE) 
            rmLogFilePtr = new UFC::FileStreamEx(fullFileName, "a");
        else
            rmLogFilePtr = new UFC::FileStreamEx(fullFileName, "w");
        UFC::BufferedLog::Printf(" OpenRiskLogFile() %s", fullFileName.c_str() );
        
        WriteRiskLog("", " ");
        WriteRiskLog("", " ");
        WriteRiskLog("", "=============== %s %s Start ===============", UFC::Hostname, fileNamePrefix.c_str());
        FlushRiskLog();
        return true;
    }
    catch(UFC::FileException& e)
    {
        UFC::BufferedLog::Printf(" OpenRiskLogFile() FileException:%s", e.what());
        rmLogFilePtr = 0;
    }
    return false;
}  //OpenRiskLogFile()
//------------------------------------------------------------------------------
void SetRiskLogLevel(Int32 RiskLogLevel)
{
    rmLogFileLock.Acquire();
    rmLogLevel = RiskLogLevel;
    rmLogFileLock.Release();
}  //SetRiskLogLevel()
//------------------------------------------------------------------------------
bool NeedWriteRiskLog(Int32 RiskLogLevel)
{
    bool isNeeded = false;
    rmLogFileLock.Acquire();
    Int32 checkResult = RiskLogLevel & rmLogLevel;
    if (checkResult == 0)
        isNeeded = false;
    else
        isNeeded = true;
    rmLogFileLock.Release();
    return isNeeded;
}  //NeedWriteRiskLog()
//------------------------------------------------------------------------------
void WriteRiskLog(const UFC::AnsiString& LogHeadStr, const char* FormatStr ...)
{
    if (rmLogFilePtr == 0) return;
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
    rmLogFileLock.Acquire();
    try
    {
        rmLogFilePtr->Write(timeBuffer, 12);
        rmLogFilePtr->Write(&space, 1);
        if (LogHeadStr.Length() > 0)
        {
            rmLogFilePtr->Write(LogHeadStr.c_str(), LogHeadStr.Length());
            rmLogFilePtr->Write(&space, 1);
        }
        rmLogFilePtr->Write(outputBuffer, outputCount);
        rmLogFilePtr->Write(&newLine, 1);
    }
    catch(UFC::FileException& e)
    {
        UFC::BufferedLog::Printf(" WriteLevelRiskLog() FileException:%s", e.what());
    }
    rmLogFileLock.Release();
}  //WriteRiskLog()
//------------------------------------------------------------------------------
void WriteLevelRiskLog(Int32 LogLevel, const UFC::AnsiString& LogHeadStr, const char* FormatStr ...)
{
    if (rmLogFilePtr == 0) return;
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
    rmLogFileLock.Acquire();
    try
    {
        rmLogFilePtr->Write(timeBuffer, 12);
        rmLogFilePtr->Write(&space, 1);
        if (LogHeadStr.Length() > 0)
        {
            rmLogFilePtr->Write(LogHeadStr.c_str(), LogHeadStr.Length());
            rmLogFilePtr->Write(&space, 1);
        }
        rmLogFilePtr->Write(outputBuffer, outputCount);
        rmLogFilePtr->Write(&newLine, 1);
    }
    catch(UFC::FileException& e)
    {
        UFC::BufferedLog::Printf(" WriteLevelRiskLog() FileException:%s", e.what());
    }
    rmLogFileLock.Release();
}  //WriteLevelRiskLog()
//------------------------------------------------------------------------------
void FlushRiskLog()
{
    if (rmLogFilePtr != 0) rmLogFilePtr->Flush();  
}  //FlushRiskLog()
//------------------------------------------------------------------------------
void CloseRiskLogFile()
{
    if (rmLogFilePtr != 0) 
    {
        rmLogFilePtr->Flush();
        delete rmLogFilePtr;
    }
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
void CRMBasicObject::DumpData(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString dumpStr = ToString(RWLockType);
    UFC::BufferedLog::Printf(" %s", dumpStr.c_str());
    if (DoFlush) UFC::BufferedLog::FlushToFile();
}  //CRMBasicObject::DumpData()

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
//------------------------------------------------------------------------------
void CRMAccountNoKey::SetAccountNo(const UFC::AnsiString& AccountNo, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    FAccountNo = AccountNo;
    FIndexKey = FAccountNo;
    FObjectLock.Unlock(RWLockType);
}  //CRMAccountNoKey::SetAccountNo()

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
void CRMAccountStockNoKey::SetAccountNo(const UFC::AnsiString& AccountNo, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    FAccountNo = AccountNo;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo);
    FObjectLock.Unlock(RWLockType);
}  //CRMAccountStockNoKey::SetAccountNo()
//------------------------------------------------------------------------------
void CRMAccountStockNoKey::SetStockNo(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    FStockNo = StockNo;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo);
    FObjectLock.Unlock(RWLockType);
}  //CRMAccountStockNoKey::SetStockNo()
//------------------------------------------------------------------------------
UFC::AnsiString CRMAccountStockNoKey::ToString(UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString objStr = "";
    FObjectLock.Lock(RWLockType);
    objStr.Printf("Account[%s] Stock[%s]", FAccountNo.c_str(), FStockNo.c_str());
    FObjectLock.Unlock(RWLockType);
    return objStr;
}  //CRMAccountStockNoKey::ToString()

//---------------------------- CRMBrokerStockNoKey -----------------------------
//------------------------------------------------------------------------------
UFC::AnsiString CRMBrokerStockNoKey::BuildIndexKey(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char TWSEOrderType)
{
    UFC::AnsiString indexKey;
    indexKey.Printf("%s%s%s%s%c", AccountNo.c_str(), StockNo.c_str(), WarrantNo.c_str(), BrokerId.c_str(), TWSEOrderType);
    return indexKey;
}  //CRMBrokerStockNoKey::BuildIndexKey()
//------------------------------------------------------------------------------
CRMBrokerStockNoKey::CRMBrokerStockNoKey()
:CRMAccountStockNoKey()
,FWarrantNo("")
,FBrokerId("")
,FTWSEOrderType(' ')
{    
}  //CRMBrokerStockNoKey::CRMBrokerStockNoKey()
//------------------------------------------------------------------------------
CRMBrokerStockNoKey::CRMBrokerStockNoKey(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char TWSEOrderType)
:CRMAccountStockNoKey()
,FWarrantNo(WarrantNo)
,FBrokerId(BrokerId)
,FTWSEOrderType(TWSEOrderType)
{
    FStockNo = StockNo;
    FAccountNo = AccountNo;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo, FWarrantNo, FBrokerId, FTWSEOrderType);
}  //CRMBrokerStockNoKey::CRMBrokerStockNoKey()
//------------------------------------------------------------------------------
void CRMBrokerStockNoKey::SetAccountNo(const UFC::AnsiString& AccountNo, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    FAccountNo = AccountNo;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo, FWarrantNo, FBrokerId, FTWSEOrderType);
    FObjectLock.Unlock(RWLockType);
}  //CRMBrokerStockNoKey::SetAccountNo()
//------------------------------------------------------------------------------
void CRMBrokerStockNoKey::SetStockNo(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    FStockNo = StockNo;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo, FWarrantNo, FBrokerId, FTWSEOrderType);
    FObjectLock.Unlock(RWLockType);
}  //CRMBrokerStockNoKey::SetStockNo()
//------------------------------------------------------------------------------
void CRMBrokerStockNoKey::SetWarrantNo(const UFC::AnsiString& WarrantNo, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    FWarrantNo = WarrantNo;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo, FWarrantNo, FBrokerId, FTWSEOrderType);
    FObjectLock.Unlock(RWLockType);
}  //CRMBrokerStockNoKey::SetWarrantNo()
//------------------------------------------------------------------------------
void CRMBrokerStockNoKey::SetBrokerId(const UFC::AnsiString& BrokerId, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    FBrokerId = BrokerId;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo, FWarrantNo, FBrokerId, FTWSEOrderType);
    FObjectLock.Unlock(RWLockType);
}  //CRMBrokerStockNoKey::SetBrokerId()
//------------------------------------------------------------------------------
void CRMBrokerStockNoKey::SetTWSEOrderType(char TWSEOrderType, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    FTWSEOrderType = TWSEOrderType;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo, FWarrantNo, FBrokerId, FTWSEOrderType);
    FObjectLock.Unlock(RWLockType);
}  //CRMBrokerStockNoKey::SetTWSEOrderType()
//------------------------------------------------------------------------------
UFC::AnsiString CRMBrokerStockNoKey::ToString(UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString objStr = "";
    FObjectLock.Lock(RWLockType);
    objStr.Printf("Account[%s] Stock[%s] WarrantNo[%s] BrokerId[%s] OrderType[%c]", FAccountNo.c_str(), FStockNo.c_str(), FWarrantNo.c_str(), FBrokerId.c_str(), FTWSEOrderType);
    FObjectLock.Unlock(RWLockType);
    return objStr;    
}  //CRMBrokerStockNoKey::ToString()

//-------------------------------- CStockObject --------------------------------
//------------------------------------------------------------------------------
CStockObject::CStockObject()
:UFC::CMainKeyObject(UFC::cstCreateLock)
,FStockNo("")
,FBullPrice(0.0)
,FLDCPrice(0.0)
,FBearPrice(0.0)
,FLastMatchDate("")
,FSetType(0)
,FMarkW(0)
,FMarkP(0)
,FMarkL(0)
,FINDCode("")
,FSTKCode("")
,FMarkM(0)
,FStockName("")
,FMarkWMarchInterval(0)
,FMarkWOrderLimit(0)
,FMarkWOrdersLimit(0)
,FMarkWPreparRate(0)
,FMarkS(0)
,FMarkF(0)
,FMarkDayTrade(0)
,FTradeUnit(1000)
,FCurrency("")
,FIsOffHourEnabled(false)
,FOffHourClosePrice(0.0)
,FIsOddEnabled(false)
,FOddBullPrice(0.0)
,FOddBearPrice(0.0)
,FOddReferencePrice(0.0)
{   
}  //CStockObject::CStockObject()
//------------------------------------------------------------------------------
CStockObject::CStockObject(const UFC::AnsiString StockNo)
:UFC::CMainKeyObject(StockNo, UFC::cstCreateLock)
,FStockNo(StockNo)
,FBullPrice(0.0)
,FLDCPrice(0.0)
,FBearPrice(0.0)
,FLastMatchDate("")
,FSetType(0)
,FMarkW(0)
,FMarkP(0)
,FMarkL(0)
,FINDCode("")
,FSTKCode("")
,FMarkM(0)
,FStockName("")
,FMarkWMarchInterval(0)
,FMarkWOrderLimit(0)
,FMarkWOrdersLimit(0)
,FMarkWPreparRate(0)
,FMarkS(0)
,FMarkF(0)
,FMarkDayTrade(0)
,FTradeUnit(1000)
,FCurrency("")
,FIsOffHourEnabled(false)
,FOffHourClosePrice(0.0)
,FIsOddEnabled(false)
,FOddBullPrice(0.0)
,FOddBearPrice(0.0)
,FOddReferencePrice(0.0)
{    
}  //CStockObject::CStockObject()
//------------------------------------------------------------------------------
bool CStockObject::IsMeasuringStock(UFC::RWLockTypeEnum LockType)
{
    bool isMeasuringStock = false;
    Lock(LockType);
    if ((FMarkW == '1') || (FMarkW == '2') || (FMarkW == '3')) isMeasuringStock = true;
    Unlock(LockType);
    return isMeasuringStock;
}  //CStockObject::IsMeasuringStock()
//------------------------------------------------------------------------------
UFC::AnsiString CStockObject::ToString(UFC::RWLockTypeEnum LockType)
{
    UFC::PStringBuffer stkBuffer;
    Lock(LockType);
    stkBuffer.Append("STK[");
    stkBuffer.Append(FStockNo);
    stkBuffer.AppendPrintf("] Px[Bull:%10.4lf,Bear:%10.4lf,LDC:%10.4lf] [", FBullPrice, FBearPrice, FLDCPrice);
    if (FLastMatchDate.Length() > 0) stkBuffer.Append(FLastMatchDate);
    stkBuffer.AppendPrintf("] SetType:'%c' Mark[W:'%c',P:'%c',L:'%c',M:'%c',S:'%c',F:'%c', DT:'%c']", FSetType, FMarkW, FMarkP, FMarkL, FMarkM, FMarkS, FMarkF, FMarkDayTrade);
    stkBuffer.Append(" Code[IND:");
    if (FINDCode.Length() > 0) stkBuffer.Append(FINDCode);
    stkBuffer.Append(", STK:");
    if (FSTKCode.Length() > 0) stkBuffer.Append(FSTKCode);
    stkBuffer.Append("]");
    stkBuffer.AppendPrintf(" Unit[%5d]", FTradeUnit);
    if (FCurrency.Length() > 0) 
    {
        stkBuffer.Append(" ");
        stkBuffer.Append(FCurrency);
    }
    if ((FMarkW == '1') || (FMarkW == '2') || (FMarkW == '3'))
        stkBuffer.AppendPrintf(" Measuring[Match:%3d Ord:%6d Ords:%6d Rate:%3d]", FMarkWMarchInterval, FMarkWOrderLimit, FMarkWOrdersLimit, FMarkWPreparRate);
    
    if (FIsOffHourEnabled) 
        stkBuffer.AppendPrintf(" OffHR ClosePx:%10.4lf", FOffHourClosePrice);
    
    if (FIsOddEnabled)
        stkBuffer.AppendPrintf(" OddPx[Bull:%10.4lf, Bear:%10.4lf Ref:%10.4lf]", FOddBullPrice, FOddBearPrice, FOddReferencePrice);
    Unlock(LockType);
    return stkBuffer.ToString();
}  //CStockObject::ToString()

//------------------------------ CRMOrderSummary -------------------------------
//------------------------------------------------------------------------------
CRMOrderSummary::CRMOrderSummary(const UFC::AnsiString& OrderId, char Side, const UFC::AnsiString& StockNo,
                                 const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char TWSEOrderType, 
                                 double OrderPrice, int OrderQty, char ExchangeCode, TWSE::TMessageType MsgType, const UFC::AnsiString& AccountNo)
:CRMBasicObject(OrderId)
,FOrderId(OrderId)
,FSide(Side)
,FStockNo(StockNo)
,FOrderPrice(OrderPrice)
,FOrderQty(OrderQty)
,FLeavesQty(OrderQty)
,FWarrantNo(WarrantNo)
,FBrokerId(BrokerId)
,FTWSEOrderType(TWSEOrderType)
,FExchangeCode(ExchangeCode)
,FMsgType(MsgType)
,FAccountNo(AccountNo)
,FOrderAmount(0.0)
{
    FOrderAmount = static_cast<double>(FOrderQty) * FOrderPrice;
}  //CRMOrderSummary::CRMOrderSummary()
//------------------------------------------------------------------------------
UFC::AnsiString CRMOrderSummary::ToString(UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString objStr = "";
    FObjectLock.Lock(RWLockType);
    objStr.Printf("OrderId[%s] Stock[%s][%s][%s]%c %c OrdPx[%10.4lf] OrdQty[%d] LeavesQty[%d] ExCode[%c] Account[%s] Amount[%15.4lf]", 
                  FOrderId.c_str(), FStockNo.c_str(), FWarrantNo.c_str(), FBrokerId.c_str(), FTWSEOrderType, FSide, FOrderPrice, FOrderQty, FLeavesQty, FExchangeCode, FAccountNo.c_str(), FOrderAmount);
    FObjectLock.Unlock(RWLockType);
    return objStr;
}  //CRMOrderSummary::ToString()

//-------------------------- CRMBrokerStockCondition ---------------------------
//------------------------------------------------------------------------------
CRMBrokerStockCondition::CRMBrokerStockCondition(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo, 
                                                 const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char TWSEOrderType,
                                                 int AvailableSellQty, int AvailableBuyQty, 
                                                 const UFC::AnsiString& DataDate, bool IsSellShort)
:CRMBrokerStockNoKey(AccountNo, StockNo, WarrantNo, BrokerId, TWSEOrderType)
,FAvailableSellQty(AvailableSellQty)
,FAvailableBuyQty(AvailableBuyQty)
,FNormalMatchBuyQty(0)
,FNormalMatchSellQty(0)
,FMatchBuyQty(0)
,FMatchSellQty(0)
,FNormalOrderBuyQty(0)
,FNormalOrderSellQty(0)
,FOrderBuyQty(0)
,FOrderSellQty(0)
,FDataDate(DataDate)
,FIsSellShort(IsSellShort)
{
}  //CRMBrokerStockCondition::CRMBrokerStockCondition()
//------------------------------------------------------------------------------
UFC::AnsiString CRMBrokerStockCondition::ToString(UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString objStr = "";
    UFC::AnsiString sellShortStr = " ";
    FObjectLock.Lock(RWLockType);
    if (FIsSellShort) sellShortStr = "Sell Short";
    objStr.Printf("Acc[%s] Stock[%s][%s][%s]%c available[S %d, B %d] match[NS %d, NB %d; S %d, B %d] order[NS %d, NB %d; S %d, B %d] Date[%s] %s", 
                  FAccountNo.c_str(), FStockNo.c_str(), FWarrantNo.c_str(), FBrokerId.c_str(), FTWSEOrderType,
                  FAvailableSellQty, FAvailableBuyQty, 
                  FNormalMatchSellQty, FNormalMatchBuyQty, FMatchSellQty, FMatchBuyQty, 
                  FNormalOrderSellQty, FNormalOrderBuyQty, FOrderSellQty, FOrderBuyQty, 
                  FDataDate.c_str(), sellShortStr.c_str());
    FObjectLock.Unlock(RWLockType);
    return objStr;    
}  //CRMBrokerStockCondition::ToString()

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
    FObjectLock.LockForWrite();
    ClearNormalBuyOrder(rmNotDelete, UFC::rwNotLock);
    ClearNormalSellOrder(rmNotDelete, UFC::rwNotLock);
    ClearBuyOrder(rmNotDelete, UFC::rwNotLock);
    ClearSellOrder(rmNotDelete, UFC::rwNotLock);
    FObjectLock.UnlockForWrite();
}  //CRMStockTradingData::~CRMStockTradingData()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::FindMaxOrderPriceOrder(CStrRiskManagerObjHMap& OrderIdIndex, UFC::RWLockTypeEnum RWLockType)
{
    double maxOrderPrice = 0.0;
    CRMOrderSummary *maxOrderPtr = 0;
    FObjectLock.Lock(RWLockType);
    CRMOrderSummary *curOrderPtr = static_cast<CRMOrderSummary*>(OrderIdIndex.First());
    while(curOrderPtr != 0)
    {
        double curOrderPrice = curOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        if (curOrderPrice > maxOrderPrice)
        {
            maxOrderPtr = curOrderPtr;
            maxOrderPrice = curOrderPrice;
        }
        curOrderPtr = static_cast<CRMOrderSummary*>(OrderIdIndex.Next());
    }  //while(curOrderPtr != 0)
    FObjectLock.Unlock(RWLockType);
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
CRMOrderSummary *CRMStockTradingData::FindMinOrderPriceOrder(CStrRiskManagerObjHMap& OrderIdIndex, UFC::RWLockTypeEnum RWLockType)
{
    double minOrderPrice = FInitialMinOrderPrice;
    CRMOrderSummary *minOrderPtr = 0;
    FObjectLock.Lock(RWLockType);
    CRMOrderSummary *curOrderPtr = static_cast<CRMOrderSummary*>(OrderIdIndex.First());
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
    FObjectLock.Unlock(RWLockType);
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
int CRMStockTradingData::GetNormalBuyOrderCount(UFC::RWLockTypeEnum RWLockType)
{
    return GetRiskManagerObjMapCount(FNormalBuyOrderIdIndex, &FObjectLock, RWLockType);
}  //CRMStockTradingData::GetNormalBuyOrderCount()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::FindNormalBuyOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType)
{
    return static_cast<CRMOrderSummary*>(FindRiskManagerObjFromMap(FNormalBuyOrderIdIndex, OrderId, &FObjectLock, RWLockType));
}  //CRMStockTradingData::FindNormalBuyOrder()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertNormalBuyOrder(const UFC::AnsiString& OrderId, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    bool isSuccess = InsertRiskManagerObjIntoMap(FNormalBuyOrderIdIndex, OrderId, OrderSummaryPtr, DoCheckExist, &FObjectLock, UFC::rwNotLock);
    if (isSuccess)
    {
        double orderPrice = OrderSummaryPtr->GetOrderPrice(UFC::rwLockForRead);
        if (orderPrice > FMaxNormalBuyOrderPrice) 
        {
            FMaxNormalBuyOrderPrice = orderPrice;
            FMaxNormalBuyOrderPtr = OrderSummaryPtr;
        }
    }
    FObjectLock.Unlock(RWLockType);
    return isSuccess;
}  //CRMStockTradingData::InsertNormalBuyOrder()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::RemoveNormalBuyOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    CRMOrderSummary *targetOrderPtr = static_cast<CRMOrderSummary*>(RemoveRiskManagerObjFromMap(FNormalBuyOrderIdIndex, OrderId, &FObjectLock, UFC::rwNotLock));
    if (targetOrderPtr != 0) 
    {
        FMaxNormalBuyOrderPtr = FindMaxOrderPriceOrder(FNormalBuyOrderIdIndex, UFC::rwNotLock);
        if (FMaxNormalBuyOrderPtr != 0) 
            FMaxNormalBuyOrderPrice = FMaxNormalBuyOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        else
            FMaxNormalBuyOrderPrice = 0.0;
    }
    FObjectLock.Unlock(RWLockType);
    return targetOrderPtr;
}  //CRMStockTradingData::RemoveNormalBuyOrder()
//------------------------------------------------------------------------------
int CRMStockTradingData::ClearNormalBuyOrder(bool DoDelete, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    int clearCount = ClearRiskManagerObjMap(FNormalBuyOrderIdIndex, DoDelete, &FObjectLock, UFC::rwNotLock);
    FObjectLock.Unlock(RWLockType);
    return clearCount;
}  //CRMStockTradingData::ClearNormalBuyOrder()
//------------------------------------------------------------------------------
void CRMStockTradingData::DumpNormalBuyOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    UFC::BufferedLog::Printf(" %d Normal Buy Order", FNormalBuyOrderIdIndex.ItemCount());
    DumpRiskManagerObjMap(FNormalBuyOrderIdIndex, DoFlush, &FObjectLock, UFC::rwNotLock);
    FObjectLock.Unlock(RWLockType);
}  //CRMStockTradingData::DumpNormalBuyOrder()
//------------------------------------------------------------------------------
int CRMStockTradingData::GetNormalSellOrderCount(UFC::RWLockTypeEnum RWLockType)
{
    return GetRiskManagerObjMapCount(FNormalSellOrderIdIndex, &FObjectLock, RWLockType);
}  //CRMStockTradingData::GetNormalSellOrderCount()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::FindNormalSellOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType)
{
    return static_cast<CRMOrderSummary*>(FindRiskManagerObjFromMap(FNormalSellOrderIdIndex, OrderId, &FObjectLock, RWLockType));
}  //CRMStockTradingData::FindNormalSellOrder()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertNormalSellOrder(const UFC::AnsiString& OrderId, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    bool isSuccess = InsertRiskManagerObjIntoMap(FNormalSellOrderIdIndex, OrderId, OrderSummaryPtr, DoCheckExist, &FObjectLock, UFC::rwNotLock);
    if (isSuccess)
    {
        double orderPrice = OrderSummaryPtr->GetOrderPrice(UFC::rwLockForRead);
        if (orderPrice < FMinNormalSellOrderPrice) 
        {
            FMinNormalSellOrderPrice = orderPrice;
            FMinNormalSellOrderPtr = OrderSummaryPtr;
        }
    }
    FObjectLock.Unlock(RWLockType);
    return isSuccess;
}  //CRMStockTradingData::InsertNormalSellOrder()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::RemoveNormalSellOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    CRMOrderSummary *targetOrderPtr = static_cast<CRMOrderSummary*>(RemoveRiskManagerObjFromMap(FNormalSellOrderIdIndex, OrderId, &FObjectLock, UFC::rwNotLock));
    if (targetOrderPtr != 0) 
    {
        FMinNormalSellOrderPtr = FindMinOrderPriceOrder(FNormalSellOrderIdIndex, UFC::rwNotLock);
        if (FMinNormalSellOrderPtr != 0) 
            FMinNormalSellOrderPrice = FMinNormalSellOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        else
            FMinNormalSellOrderPrice = FInitialMinOrderPrice;
    }
    FObjectLock.Unlock(RWLockType);
    return targetOrderPtr;
}  //CRMStockTradingData::RemoveNormalSellOrder()
//------------------------------------------------------------------------------
int CRMStockTradingData::ClearNormalSellOrder(bool DoDelete, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    int clearCount = ClearRiskManagerObjMap(FNormalSellOrderIdIndex, DoDelete, &FObjectLock, UFC::rwNotLock);
    FObjectLock.Unlock(RWLockType);
    return clearCount;
}  //CRMStockTradingData::ClearNormalSellOrder()
//------------------------------------------------------------------------------
void CRMStockTradingData::DumpNormalSellOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    UFC::BufferedLog::Printf(" %d Normal Sell Order", FNormalSellOrderIdIndex.ItemCount());
    DumpRiskManagerObjMap(FNormalSellOrderIdIndex, DoFlush, &FObjectLock, UFC::rwNotLock);
    FObjectLock.Unlock(RWLockType);
}  //CRMStockTradingData::DumpNormalSellOrder()
//------------------------------------------------------------------------------
int CRMStockTradingData::GetBuyOrderCount(UFC::RWLockTypeEnum RWLockType)
{
    return GetRiskManagerObjMapCount(FBuyOrderIdIndex, &FObjectLock, RWLockType);
}  //CRMStockTradingData::GetBuyOrderCount()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::FindBuyOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType)
{
    return static_cast<CRMOrderSummary*>(FindRiskManagerObjFromMap(FBuyOrderIdIndex, OrderId, &FObjectLock, RWLockType));
}  //CRMStockTradingData::FindBuyOrder()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertBuyOrder(const UFC::AnsiString& OrderId, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    bool isSuccess = InsertRiskManagerObjIntoMap(FBuyOrderIdIndex, OrderId, OrderSummaryPtr, DoCheckExist, &FObjectLock, UFC::rwNotLock);
    if (isSuccess)
    {
        double orderPrice = OrderSummaryPtr->GetOrderPrice(UFC::rwLockForRead);
        if (orderPrice > FMaxBuyOrderPrice) 
        {
            FMaxBuyOrderPrice = orderPrice;
            FMaxBuyOrderPtr = OrderSummaryPtr;
        }
    }
    FObjectLock.Unlock(RWLockType);
    return isSuccess;
}  //CRMStockTradingData::InsertBuyOrder()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::RemoveBuyOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    CRMOrderSummary *targetOrderPtr = static_cast<CRMOrderSummary*>(RemoveRiskManagerObjFromMap(FBuyOrderIdIndex, OrderId, &FObjectLock, UFC::rwNotLock));
    if (targetOrderPtr != 0) 
    {
        FMaxBuyOrderPtr = FindMaxOrderPriceOrder(FBuyOrderIdIndex, UFC::rwNotLock);
        if (FMaxBuyOrderPtr != 0) 
            FMaxBuyOrderPrice = FMaxBuyOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        else
            FMaxBuyOrderPrice = 0.0;
    }
    FObjectLock.Unlock(RWLockType);
    return targetOrderPtr;
}  //CRMStockTradingData::RemoveBuyOrder()
//------------------------------------------------------------------------------
int CRMStockTradingData::ClearBuyOrder(bool DoDelete, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    int clearCount = ClearRiskManagerObjMap(FBuyOrderIdIndex, DoDelete, &FObjectLock, UFC::rwNotLock);
    FObjectLock.Unlock(RWLockType);
    return clearCount;
}  //CRMStockTradingData::ClearBuyOrder()
//------------------------------------------------------------------------------
void CRMStockTradingData::DumpBuyOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    UFC::BufferedLog::Printf(" %d Buy Order", FBuyOrderIdIndex.ItemCount());
    DumpRiskManagerObjMap(FBuyOrderIdIndex, DoFlush, &FObjectLock, UFC::rwNotLock);
    FObjectLock.Unlock(RWLockType);
}  //CRMStockTradingData::DumpBuyOrder()
//------------------------------------------------------------------------------
int CRMStockTradingData::GetSellOrderCount(UFC::RWLockTypeEnum RWLockType)
{
    return GetRiskManagerObjMapCount(FSellOrderIdIndex, &FObjectLock, RWLockType);
}  //CRMStockTradingData::GetSellOrderCount()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::FindSellOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType)
{
    return static_cast<CRMOrderSummary*>(FindRiskManagerObjFromMap(FSellOrderIdIndex, OrderId, &FObjectLock, RWLockType));
}  //CRMStockTradingData::FindSellOrder()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertSellOrder(const UFC::AnsiString& OrderId, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    bool isSuccess = InsertRiskManagerObjIntoMap(FSellOrderIdIndex, OrderId, OrderSummaryPtr, DoCheckExist, &FObjectLock, UFC::rwNotLock);
    if (isSuccess)
    {
        double orderPrice = OrderSummaryPtr->GetOrderPrice(UFC::rwLockForRead);
        if (orderPrice < FMinSellOrderPrice) 
        {
            FMinSellOrderPrice = orderPrice;
            FMinSellOrderPtr = OrderSummaryPtr;
        }
    }
    FObjectLock.Unlock(RWLockType);
    return isSuccess;
}  //CRMStockTradingData::InsertSellOrder()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::RemoveSellOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    CRMOrderSummary *targetOrderPtr = static_cast<CRMOrderSummary*>(RemoveRiskManagerObjFromMap(FSellOrderIdIndex, OrderId, &FObjectLock, UFC::rwNotLock));
    if (targetOrderPtr != 0) 
    {
        FMinSellOrderPtr = FindMinOrderPriceOrder(FSellOrderIdIndex, UFC::rwNotLock);
        if (FMinSellOrderPtr != 0) 
            FMinSellOrderPrice = FMinSellOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        else
            FMinSellOrderPrice = FInitialMinOrderPrice;
    }
    FObjectLock.Unlock(RWLockType);
    return targetOrderPtr;
}  //CRMStockTradingData::RemoveSellOrder()
//------------------------------------------------------------------------------
int CRMStockTradingData::ClearSellOrder(bool DoDelete, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    int clearCount = ClearRiskManagerObjMap(FSellOrderIdIndex, DoDelete, &FObjectLock, UFC::rwNotLock);
    FObjectLock.Unlock(RWLockType);
    return clearCount;
}  //CRMStockTradingData::ClearSellOrder()
//------------------------------------------------------------------------------
void CRMStockTradingData::DumpSellOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    UFC::BufferedLog::Printf(" %d Sell Order", FSellOrderIdIndex.ItemCount());
    DumpRiskManagerObjMap(FSellOrderIdIndex, DoFlush, &FObjectLock, UFC::rwNotLock);
    FObjectLock.Unlock(RWLockType);
}  //CRMStockTradingData::DumpSellOrder()
//------------------------------------------------------------------------------
int CRMStockTradingData::GetBrokerStockConditionCount(UFC::RWLockTypeEnum RWLockType)
{
    return GetRiskManagerObjMapCount(FBrokerStockNoIndex, &FObjectLock, RWLockType);
}  //CRMStockTradingData::GetBrokerStockConditionCount()
//------------------------------------------------------------------------------
CRMBrokerStockCondition *CRMStockTradingData::FindBrokerStockCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char TWSEOrderType, UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString keyValue = CRMBrokerStockNoKey::BuildIndexKey(FAccountNo, FStockNo, WarrantNo, BrokerId, TWSEOrderType);
    return static_cast<CRMBrokerStockCondition*>(FindRiskManagerObjFromMap(FBrokerStockNoIndex, keyValue, &FObjectLock, RWLockType));
}  //CRMStockTradingData::FindBrokerStockCondition()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertBrokerStockCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char TWSEOrderType, CRMBrokerStockCondition *ConditionPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString keyValue = CRMBrokerStockNoKey::BuildIndexKey(FAccountNo, FStockNo, WarrantNo, BrokerId, TWSEOrderType);
    return InsertRiskManagerObjIntoMap(FBrokerStockNoIndex, keyValue, ConditionPtr, DoCheckExist, &FObjectLock, RWLockType);
}  //CRMStockTradingData::InsertBrokerStockCondition()
//------------------------------------------------------------------------------
CRMBrokerStockCondition *CRMStockTradingData::RemoveBrokerStockCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char TWSEOrderType, UFC::RWLockTypeEnum RWLockType)
{    
    UFC::AnsiString keyValue = CRMBrokerStockNoKey::BuildIndexKey(FAccountNo, FStockNo, WarrantNo, BrokerId, TWSEOrderType);
    return static_cast<CRMBrokerStockCondition*>(RemoveRiskManagerObjFromMap(FBrokerStockNoIndex, keyValue, &FObjectLock, RWLockType));
}  //CRMStockTradingData::RemoveBrokerStockCondition()
//------------------------------------------------------------------------------
int CRMStockTradingData::ClearBrokerStockCondition(bool DoDelete, UFC::RWLockTypeEnum RWLockType)
{    
    return ClearRiskManagerObjMap(FBrokerStockNoIndex, DoDelete, &FObjectLock, RWLockType);
}  //CRMStockTradingData::ClearBrokerStockCondition()
//------------------------------------------------------------------------------
void CRMStockTradingData::DumpBrokerStockCondition(bool DoFlush, UFC::RWLockTypeEnum RWLockType)    
{    
    FObjectLock.Lock(RWLockType);
    UFC::BufferedLog::Printf(" %d Broker Stock Condition", FBrokerStockNoIndex.ItemCount());
    DumpRiskManagerObjMap(FBrokerStockNoIndex, rmNotFlush, &FObjectLock, UFC::rwNotLock);
    FObjectLock.Unlock(RWLockType);
    UFC::BufferedLog::Printf(" ");
    if (DoFlush) UFC::BufferedLog::FlushToFile();
}  //CRMStockTradingData::DumpBrokerStockCondition()
//------------------------------------------------------------------------------
UFC::AnsiString CRMStockTradingData::ToString(UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString objStr = "";
    FObjectLock.Lock(RWLockType);
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
    FObjectLock.Unlock(RWLockType);
    return objStr;
}  //CRMStockTradingData::ToString()
//------------------------------------------------------------------------------
void CRMStockTradingData::DumpData(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    UFC::AnsiString objStr = ToString(UFC::rwNotLock);
    UFC::BufferedLog::Printf(" %s", objStr.c_str());
    DumpBrokerStockCondition(rmNotFlush, UFC::rwNotLock);
    DumpBuyOrder(rmNotFlush, UFC::rwNotLock);
    DumpSellOrder(rmNotFlush, UFC::rwNotLock);
    FObjectLock.Unlock(RWLockType);
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
    ClearStockData(true, UFC::rwLockForWrite);
}  //CRMInvestorAccount::~CRMInvestorAccount()
//------------------------------------------------------------------------------
int CRMInvestorAccount::GetStockDataCount(UFC::RWLockTypeEnum RWLockType)
{
    return GetRiskManagerObjMapCount(FStockNoIndex, &FObjectLock, RWLockType);
}  //CRMInvestorAccount::GetStockDataCount()
//------------------------------------------------------------------------------
CRMStockTradingData *CRMInvestorAccount::FindStockData(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType)
{
    return static_cast<CRMStockTradingData*>(FindRiskManagerObjFromMap(FStockNoIndex, StockNo, &FObjectLock, RWLockType));
}  //CRMInvestorAccount::FindStockData()
//------------------------------------------------------------------------------
CRMStockTradingData *CRMInvestorAccount::FindOrCreateStockData(const UFC::AnsiString& StockNo, bool& IsNewStockData, UFC::RWLockTypeEnum RWLockType)
{
    IsNewStockData = false;
    Lock(RWLockType);
    CRMStockTradingData *stockDataPtr = FindStockData(StockNo, UFC::rwNotLock);
    if (stockDataPtr == 0)
    {
        stockDataPtr = new CRMStockTradingData(FAccountNo, StockNo);
        if (InsertStockData(StockNo, stockDataPtr, rmNotCheck, UFC::rwNotLock))
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
bool CRMInvestorAccount::InsertStockData(const UFC::AnsiString& StockNo, CRMStockTradingData *StockDataPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType)
{
    return InsertRiskManagerObjIntoMap(FStockNoIndex, StockNo, StockDataPtr, DoCheckExist, &FObjectLock, RWLockType);
}  //CRMInvestorAccount::InsertStockData()
//------------------------------------------------------------------------------
int CRMInvestorAccount::ClearStockData(bool DoDelete, UFC::RWLockTypeEnum RWLockType)
{
    return ClearRiskManagerObjMap(FStockNoIndex, DoDelete, &FObjectLock, RWLockType);
}  //CRMInvestorAccount::ClearStockData()
//------------------------------------------------------------------------------
void CRMInvestorAccount::DumpStockData(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    UFC::BufferedLog::Printf(" %d Stock Data", FStockNoIndex.ItemCount());
    
    DumpRiskManagerObjMap(FStockNoIndex, rmNotFlush, &FObjectLock, UFC::rwNotLock);
    FObjectLock.Unlock(RWLockType);
    if (DoFlush) UFC::BufferedLog::FlushToFile();
}  //CRMInvestorAccount::DumpStockData()
//------------------------------------------------------------------------------
UFC::AnsiString CRMInvestorAccount::ToString(UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString objStr = "";
    objStr.Printf("Account[%s] DayTrade[%d] SelfTrade[%d] SellPositionCheck[%d]", FAccountNo.c_str(), FDayTradeType, FSelfTradeType, FSellPositionCheckType);
    return objStr;
}  //CRMInvestorAccount::ToString()
//------------------------------------------------------------------------------
void CRMInvestorAccount::DumpData(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FObjectLock.Lock(RWLockType);
    UFC::AnsiString objStr = ToString(UFC::rwNotLock);
    UFC::BufferedLog::Printf(" %s", objStr.c_str());
    UFC::BufferedLog::Printf(" %d Stock Data", FStockNoIndex.ItemCount());
    int curStockCount = 0;
    CRMStockTradingData *curStockPtr = static_cast<CRMStockTradingData*>(FStockNoIndex.First());
    while(curStockPtr != 0)
    {
        objStr = curStockPtr->ToString(UFC::rwLockForRead);
        UFC::BufferedLog::Printf(" %5d %s", ++curStockCount, objStr.c_str());
curStockPtr->DumpBrokerStockCondition(false, UFC::rwLockForRead);
        curStockPtr = static_cast<CRMStockTradingData*>(FStockNoIndex.Next());
    }  //while(curStockPtr != 0)    
    FObjectLock.Unlock(RWLockType);
    UFC::BufferedLog::Printf(" ");
    if (DoFlush) UFC::BufferedLog::FlushToFile();
}  //CRMInvestorAccount::DumpData()
