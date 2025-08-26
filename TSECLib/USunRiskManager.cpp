/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "USunRiskManager.h"
#include "../UFC/iniFile.h"
#include "TMessages.h"
#include "OMessages.h"
#include "PMessages.h"

namespace rm
{
//------------------------------ CRMReservedStock ------------------------------
CRMReservedStock::CRMReservedStock(const UFC::AnsiString& StockNo, const UFC::AnsiString& TraderID, bool IsSellShort)
:CRMBasicObject(StockNo)
,FStockNo(StockNo)
,FAvailableQty(0)
,FReservedQty(0)
,FReservedCost(0.0)
,FSettleDate("")
,FModifyDate("")
,FModifyTime("")
,FTraderID(TraderID)
,FIsSellShort(IsSellShort)
{
}  //CRMReservedStock::CRMReservedStock()
//------------------------------------------------------------------------------
CRMReservedStock::CRMReservedStock(const UFC::AnsiString& RecordStr, bool IsSellShort)
:CRMBasicObject()
,FStockNo("")
,FAvailableQty(0)
,FReservedQty(0)
,FReservedCost(0.0)
,FSettleDate("")
,FModifyDate("")
,FModifyTime("")
,FTraderID("")
,FIsSellShort(IsSellShort)
{
    ParseControlFileRecord(RecordStr);
}  //CRMReservedStock::CRMReservedStock()
//------------------------------------------------------------------------------
void CRMReservedStock::ParseControlFileRecord(const UFC::AnsiString& RecordStr)
{
    FSettleDate = RecordStr.SubString(0, 8);
    FTraderID = RecordStr.SubString(8, 3);
    FStockNo = RecordStr.SubString(11, 6);
    FAvailableQty = RecordStr.SubString(17, 11).ToInt();
    FReservedQty = RecordStr.SubString(28, 11).ToInt();
    FReservedCost = RecordStr.SubString(39, 11).ToDouble();
    FModifyDate = RecordStr.SubString(50, 8);
    FModifyTime = RecordStr.SubString(58, 6);
    FStockNo.TrimRight(' ');
}  //CRMReservedStock::ParseControlFileRecord()
//------------------------------------------------------------------------------
UFC::AnsiString CRMReservedStock::ToString(UFC::RWLockTypeEnum RWLockType) 
{
    UFC::AnsiString objStr = "";
    this->Lock(RWLockType);
    objStr.Printf("Reserved Trader[%s] Stock[%s] AvaQty[%d] ResvQty[%d] Cost[%15.0lf] SettleDate[%s] Modify[%s %s]", FTraderID.c_str(), FStockNo.c_str(), FAvailableQty, FReservedQty, FReservedCost, FSettleDate.c_str(), FModifyDate.c_str(), FModifyTime.c_str());
    this->Unlock(RWLockType);
    return objStr;
}  //CRMReservedStock::ToString()
    
//---------------------------- CRMBrokerTraderQuota ----------------------------
//------------------------------------------------------------------------------
CRMBrokerTraderQuota::CRMBrokerTraderQuota(const UFC::AnsiString& RecordStr)
:CRMBasicObject()
,FTraderID("")
,FTraderName("")
,FAuthorizedAmount(0.0)
,FInvestorAccount("")
,FTransferDate("")
,FTransferTime("")
{
    ParseControlFileRecord(RecordStr);
}  //CRMBrokerTraderQuota::CRMBrokerTraderQuota()
//------------------------------------------------------------------------------
void CRMBrokerTraderQuota::ParseControlFileRecord(const UFC::AnsiString& RecordStr)
{
    FTraderID = RecordStr.SubString(0, 3);
    FTraderName = RecordStr.SubString(3, 12);
    FAuthorizedAmount = RecordStr.SubString(15, 7).ToDouble() * 10000.0;
    FInvestorAccount = RecordStr.SubString(22, 7);
    FTransferDate = RecordStr.SubString(29, 8);
    FTransferTime = RecordStr.SubString(37, 6);
}  //CRMReservedStock::ParseControlFileRecord()
//------------------------------------------------------------------------------
UFC::AnsiString CRMBrokerTraderQuota::ToString(UFC::RWLockTypeEnum RWLockType) 
{
    UFC::AnsiString objStr = "";
    this->Lock(RWLockType);
    objStr.Printf("Trader[%s] Account[%s] Quota Amount[%15.0lf] Transfer[%s %s]", FTraderID.c_str(), FInvestorAccount.c_str(), FAuthorizedAmount, FTransferDate.c_str(), FTransferTime.c_str());
    this->Unlock(RWLockType);
    return objStr;
}  //CRMBrokerTraderQuota::ToString()

//-------------------------- CRMTraderStockCondition ---------------------------
//------------------------------------------------------------------------------
CRMTraderStockCondition::CRMTraderStockCondition(const UFC::AnsiString& StockNo, const UFC::AnsiString& TraderID)
:CRMBasicObject()
,FStockNo(StockNo)
,FAvailableQty(0)
,FReservedQty(0)
,FReservedCost(0.0)
,FSellOrderQty(0)
,FSellOrderAmount(0.0)
,FSellActiveQty(0)
,FSellActiveAmount(0.0)
,FSellMatchQty(0)
,FSellMatchAmount(0.0)
,FBuyOrderQty(0)
,FBuyOrderAmount(0.0)
,FBuyActiveQty(0)
,FBuyActiveAmount(0.0)
,FBuyMatchQty(0)
,FBuyMatchAmount(0.0)
,FAvailableSellShortQty(0)
,FReservedSellShortQty(0)
,FReservedSellShortCost(0.0)
,FSellShortOrderQty(0)
,FSellShortOrderAmount(0.0)
,FSellShortActiveQty(0)
,FSellShortActiveAmount(0.0)
,FSellShortMatchQty(0)
,FSellShortMatchAmount(0.0)
,FBuyShortOrderQty(0)
,FBuyShortOrderAmount(0.0)
,FBuyShortActiveQty(0)
,FBuyShortActiveAmount(0.0)
,FBuyShortMatchQty(0)
,FBuyShortMatchAmount(0.0)
,FTraderID(TraderID)
{
}  //CRMTraderStockCondition::CRMTraderStockCondition()
//------------------------------------------------------------------------------
void CRMTraderStockCondition::AddNewOrderData(int OrderQty, double OrderPrice, char Side, char OrderType, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    double amount = OrderPrice * OrderQty;
    if ((OrderType == '5') || (OrderType == '6'))
    {
        if (Side == 'B')
        {
            FBuyShortOrderQty += OrderQty;
            FBuyShortOrderAmount += amount;
            FBuyShortActiveQty += OrderQty;
            FBuyShortActiveAmount += amount;
        }
        else
        {
            FSellShortOrderQty += OrderQty;
            FSellShortOrderAmount += amount;
            FSellShortActiveQty += OrderQty;
            FSellShortActiveAmount += amount;
        }
    }
    else
    {
        if (Side == 'B')
        {
            FBuyOrderQty += OrderQty;
            FBuyOrderAmount += amount;
            FBuyActiveQty += OrderQty;
            FBuyActiveAmount += amount;
        }
        else
        {
            FSellOrderQty += OrderQty;
            FSellOrderAmount += amount;
            FSellActiveQty += OrderQty;
            FSellActiveAmount += amount;
        }
    }  //if ((OrderType == '5') || (OrderType == '6'))
    FLock.Unlock(RWLockType);
}  //CRMTraderStockCondition::AddNewOrderData()
//------------------------------------------------------------------------------
void CRMTraderStockCondition::AddMatchOrderData(int MatchQty, double LastPrice, double OrderPrice, char Side, char OrderType, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    double matchAmount = LastPrice * MatchQty;
    if ((OrderType == '5') || (OrderType == '6'))
    {
        if (Side == 'B')
        {
            FBuyShortActiveQty -= MatchQty;
            if (FBuyShortActiveQty < 0) FBuyShortActiveQty = 0;
            FBuyShortActiveAmount -= matchAmount;
            if (FBuyShortActiveAmount < 0.0) FBuyShortActiveAmount = 0.0;
            FBuyShortMatchQty += MatchQty;
            FBuyShortMatchAmount += matchAmount;
        }
        else
        {
            FSellShortActiveQty -= MatchQty;
            if (FSellShortActiveQty < 0) FSellShortActiveQty = 0;
            FSellShortActiveAmount -= matchAmount;
            if (FSellShortActiveAmount < 0.0) FSellShortActiveAmount = 0.0;
            FSellShortMatchQty += MatchQty;
            FSellShortMatchAmount += matchAmount;
        }
    }
    else
    {
        if (Side == 'B')
        {
            FBuyActiveQty -= MatchQty;
            if (FBuyActiveQty < 0) FBuyActiveQty = 0;
            FBuyActiveAmount -= matchAmount;
            if (FBuyActiveAmount < 0.0) FBuyActiveAmount = 0.0;
            FBuyMatchQty += MatchQty;
            FBuyMatchAmount += matchAmount;
        }
        else
        {
            FSellActiveQty -= MatchQty;
            if (FSellActiveQty < 0) FSellActiveQty = 0;
            FSellActiveAmount -= matchAmount;
            if (FSellActiveAmount < 0.0) FSellActiveAmount = 0.0;
            FSellMatchQty += MatchQty;
            FSellMatchAmount += matchAmount;
        }
    }  //if ((OrderType == '5') || (OrderType == '6'))
    FLock.Unlock(RWLockType);
}  //CRMTraderStockCondition::AddMatchOrderData()
//------------------------------------------------------------------------------
void CRMTraderStockCondition::CancelOrderData(int CancelQty, double OrderPrice, char Side, char OrderType, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    double amount = OrderPrice * CancelQty;
    if ((OrderType == '5') || (OrderType == '6'))
    {
        if (Side == 'B')
        {
            FBuyShortOrderQty -= CancelQty;
            if (FBuyShortOrderQty < 0) FBuyShortOrderQty = 0;
            FBuyShortOrderAmount -= amount;
            if (FBuyShortOrderAmount < 0.0) FBuyShortOrderAmount = 0.0;
            FBuyShortActiveQty -= CancelQty;
            if (FBuyShortActiveQty < 0) FBuyShortActiveQty = 0;
            FBuyShortActiveAmount -= amount;
            if (FBuyShortActiveAmount < 0.0) FBuyShortActiveAmount = 0.0;
        }
        else
        {
            FSellShortOrderQty -= CancelQty;
            if (FSellShortOrderQty < 0) FSellShortOrderQty = 0;
            FSellShortOrderAmount -= amount;
            if (FSellShortOrderAmount < 0.0) FSellShortOrderAmount = 0.0;
            FSellShortActiveQty -= CancelQty;
            if (FSellShortActiveQty < 0) FSellShortActiveQty = 0;
            FSellShortActiveAmount -= amount;
            if (FSellShortActiveAmount < 0.0) FSellShortActiveAmount = 0.0;
        }
    }
    else
    {
        if (Side == 'B')
        {
            FBuyOrderQty -= CancelQty;
            if (FBuyOrderQty < 0) FBuyOrderQty = 0;
            FBuyOrderAmount -= amount;
            if (FBuyOrderAmount < 0.0) FBuyOrderAmount = 0.0;
            FBuyActiveQty -= CancelQty;
            if (FBuyActiveQty < 0) FBuyActiveQty = 0;
            FBuyActiveAmount -= amount;
            if (FBuyActiveAmount < 0.0) FBuyActiveAmount = 0.0;
        }
        else
        {
            FSellOrderQty -= CancelQty;
            if (FSellOrderQty < 0) FSellOrderQty = 0;
            FSellOrderAmount -= amount;
            if (FSellOrderAmount < 0.0) FSellOrderAmount = 0.0;
            FSellActiveQty -= CancelQty;
            if (FSellActiveQty < 0) FSellActiveQty = 0;
            FSellActiveAmount -= amount;
            if (FSellActiveAmount < 0.0) FSellActiveAmount = 0.0;
        }
    }  //if ((OrderType == '5') || (OrderType == '6'))
    FLock.Unlock(RWLockType);
}  //CRMTraderStockCondition::CancelOrderData()
//------------------------------------------------------------------------------
UFC::AnsiString CRMTraderStockCondition::ToString(UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString line1Str, line2Str, line3Str, line4Str, line5Str, line6Str, line7Str;
    FLock.Lock(RWLockType);
    line1Str.Printf("Trader[%s] Stock[%s] Condition: ", FTraderID.c_str(), FStockNo.c_str());
    line2Str.Printf("           Reserved: Qty[%10d] AvaQty[%10d] Cost[%15.0lf],", FReservedQty, FAvailableQty, FReservedCost);
    line3Str.Printf("Sell Short Reserved: Qty[%10d] AvaQty[%10d] Cost[%15.0lf]", FReservedSellShortQty, FAvailableSellShortQty, FReservedSellShortCost);
    line4Str.Printf("       Buy: Ord(Qty[%10d] Amt[%15.0lf]) Active(Qty[%10d] Amt[%15.0lf]) Fill(Qty[%10d] Amt[%15.0lf])", FBuyOrderQty, FBuyOrderAmount, FBuyActiveQty, FBuyActiveAmount, FBuyMatchQty, FBuyMatchAmount);
    line5Str.Printf("      Sell: Ord(Qty[%10d] Amt[%15.0lf]) Active(Qty[%10d] Amt[%15.0lf]) Fill(Qty[%10d] Amt[%15.0lf])", FSellOrderQty, FSellOrderAmount, FSellActiveQty, FSellActiveAmount, FSellMatchQty, FSellMatchAmount);
    line6Str.Printf(" Buy Short: Ord(Qty[%10d] Amt[%15.0lf]) Active(Qty[%10d] Amt[%15.0lf]) Fill(Qty[%10d] Amt[%15.0lf])", FBuyShortOrderQty, FBuyShortOrderAmount, FBuyShortActiveQty, FBuyShortActiveAmount, FBuyShortMatchQty, FBuyShortMatchAmount);
    line7Str.Printf("Sell Short: Ord(Qty[%10d] Amt[%15.0lf]) Active(Qty[%10d] Amt[%15.0lf]) Fill(Qty[%10d] Amt[%15.0lf])", FSellShortOrderQty, FSellShortOrderAmount, FSellShortActiveQty, FSellShortActiveAmount, FSellShortMatchQty, FSellShortMatchAmount);
    FLock.Unlock(RWLockType);
    UFC::AnsiString objStr = "";
    objStr.Printf("%s\n%s\n%s\n%s\n%s\n%s\n%s", line1Str.c_str(), line2Str.c_str(), line3Str.c_str(), line4Str.c_str(), line5Str.c_str(), line6Str.c_str(), line7Str.c_str());
    return objStr;
}  //CRMTraderStockCondition::ToString()

//------------------------------ CRMBrokerTrader -------------------------------
//------------------------------------------------------------------------------
CRMBrokerTrader::CRMBrokerTrader(const UFC::AnsiString& TraderID, const UFC::AnsiString& TraderName, bool NeedCheckQuota)
:CRMBasicObject(TraderID)
,FTraderID(TraderID)
,FTraderName(TraderName)
,FNeedCheckQuota(NeedCheckQuota)
,FAuthorizedAmount(0.0)
,FReservedCost(0.0)
,FSellOrderAmount(0.0)
,FSellActiveAmount(0.0)
,FSellMatchAmount(0.0)
,FBuyOrderAmount(0.0)
,FBuyActiveAmount(0.0)
,FBuyMatchAmount(0.0)   
,FReservedSellShortCost(0.0)
,FSellShortOrderAmount(0.0)
,FSellShortActiveAmount(0.0)
,FSellShortMatchAmount(0.0)
,FBuyShortOrderAmount(0.0)
,FBuyShortActiveAmount(0.0)
,FBuyShortMatchAmount(0.0)
{
}  //CRMBrokerTrader::CRMBrokerTrader()
//------------------------------------------------------------------------------
CRMBrokerTrader::~CRMBrokerTrader()
{
    FLock.LockForWrite();
    ClearReservedStockNoIndex(cstDelete, UFC::rwNotLock);
    ClearAccQuotaNoIndex(cstDelete, UFC::rwNotLock);
    ClearReservedSellShortStockNoIndex(cstDelete, UFC::rwNotLock);    
    ClearConditionStockNoIndex(cstDelete, UFC::rwNotLock);
    FLock.UnlockForWrite();
}  //CRMBrokerTrader::~CRMBrokerTrader()
//------------------------------------------------------------------------------
void CRMBrokerTrader::AddNewOrderData(int OrderQty, double OrderPrice, char Side, char OrderType, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    double amount = OrderPrice * OrderQty;
    if ((OrderType == '5') || (OrderType == '6'))
    {
        if (Side == 'B')
        {
            FBuyShortOrderAmount += amount;
            FBuyShortActiveAmount += amount;
        }
        else
        {
            FSellShortOrderAmount += amount;
            FSellShortActiveAmount += amount;
        }
    }
    else
    {
        if (Side == 'B')
        {
            FBuyOrderAmount += amount;
            FBuyActiveAmount += amount;
        }
        else
        {
            FSellOrderAmount += amount;
            FSellActiveAmount += amount;
        }
    }  //if ((OrderType == '5') || (OrderType == '6'))
    FLock.Unlock(RWLockType);
}  //CRMBrokerTrader::AddNewOrderData()
//------------------------------------------------------------------------------
void CRMBrokerTrader::AddMatchOrderData(int MatchQty, double LastPrice, double OrderPrice, char Side, char OrderType, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    double matchAmount = LastPrice * MatchQty;
    
    if ((OrderType == '5') || (OrderType == '6'))
    {
        if (Side == 'B')
        {
            FBuyShortActiveAmount -= matchAmount;
            if (FBuyShortActiveAmount < 0.0) FBuyShortActiveAmount = 0.0;
            FBuyShortMatchAmount += matchAmount;
        }
        else
        {
            FSellShortActiveAmount -= matchAmount;
            if (FSellShortActiveAmount < 0.0) FSellShortActiveAmount = 0.0;
            FSellShortMatchAmount += matchAmount;
        }
    }
    else
    {
        if (Side == 'B')
        {
            FBuyActiveAmount -= matchAmount;
            if (FBuyActiveAmount < 0.0) FBuyActiveAmount = 0.0;
            FBuyMatchAmount += matchAmount;
        }
        else
        {
            FSellActiveAmount -= matchAmount;
            if (FSellActiveAmount < 0.0) FSellActiveAmount = 0.0;
            FSellMatchAmount += matchAmount;
        }
    }  //if ((OrderType == '5') || (OrderType == '6'))
    FLock.Unlock(RWLockType);
}  //CRMBrokerTrader::AddMatchOrderData()
//------------------------------------------------------------------------------
void CRMBrokerTrader::CancelOrderData(int CancelQty, double OrderPrice, char Side, char OrderType, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    double amount = OrderPrice * CancelQty;
    if ((OrderType == '5') || (OrderType == '6'))
    {
        if (Side == 'B')
        {
            FBuyShortOrderAmount -= amount;
            if (FBuyShortOrderAmount < 0.0) FBuyShortOrderAmount = 0.0;
            FBuyShortActiveAmount -= amount;
            if (FBuyShortActiveAmount < 0.0) FBuyShortActiveAmount = 0.0;
        }
        else
        {
            FSellShortOrderAmount -= amount;
            if (FSellShortOrderAmount < 0.0) FSellShortOrderAmount = 0.0;
            FSellShortActiveAmount -= amount;
            if (FSellShortActiveAmount < 0.0) FSellShortActiveAmount = 0.0;
        }
    }
    else
    {
        if (Side == 'B')
        {
            FBuyOrderAmount -= amount;
            if (FBuyOrderAmount < 0.0) FBuyOrderAmount = 0.0;
            FBuyActiveAmount -= amount;
            if (FBuyActiveAmount < 0.0) FBuyActiveAmount = 0.0;
        }
        else
        {
            FSellOrderAmount -= amount;
            if (FSellOrderAmount < 0.0) FSellOrderAmount = 0.0;
            FSellActiveAmount -= amount;
            if (FSellActiveAmount < 0.0) FSellActiveAmount = 0.0;
        }
    }  //if ((OrderType == '5') || (OrderType == '6'))
    FLock.Unlock(RWLockType);
}  //CRMBrokerTrader::CancelOrderData()
//------------------------------------------------------------------------------
void CRMBrokerTrader::DumpReservedStock(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    WriteRiskLog("", "Dump %d Reserved Stock Data", FReservedStockNoIndex.ItemCount());
    CRMReservedStock* curResvStockPtr = FReservedStockNoIndex.First();
    while (curResvStockPtr != 0)
    {
        WriteRiskLog("", curResvStockPtr->ToString(UFC::rwLockForRead).c_str());
        curResvStockPtr = FReservedStockNoIndex.Next();
    }
    if (DoFlush) FlushRiskLog();
    FLock.Unlock(RWLockType);
}  //CRMBrokerTrader::DumpReservedStock()
//------------------------------------------------------------------------------
void CRMBrokerTrader::DumpReservedSellShortStock(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    WriteRiskLog("", "Dump %d Reserved Sell Short Stock Data", FReservedSellShortStockNoIndex.ItemCount());
    CRMReservedStock* curResvStockPtr = FReservedSellShortStockNoIndex.First();
    while (curResvStockPtr != 0)
    {
        WriteRiskLog("", curResvStockPtr->ToString(UFC::rwLockForRead).c_str());
        curResvStockPtr = FReservedSellShortStockNoIndex.Next();
    }
    if (DoFlush) FlushRiskLog();
    FLock.Unlock(RWLockType);
}  //CRMBrokerTrader::DumpReservedSellShortStock()
//------------------------------------------------------------------------------
void CRMBrokerTrader::DumpAccQuota(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    WriteRiskLog("", "Dump %d Account Quota Data", FAccQuotaNoIndex.ItemCount());
    CRMBrokerTraderQuota* curTraderQuotaPtr = FAccQuotaNoIndex.First();
    while (curTraderQuotaPtr != 0)
    {
        WriteRiskLog("", curTraderQuotaPtr->ToString(UFC::rwLockForRead).c_str());
        curTraderQuotaPtr = FAccQuotaNoIndex.Next();
    }
    if (DoFlush) FlushRiskLog();
    FLock.Unlock(RWLockType);
}  //CRMBrokerTrader::DumpAccQuota()
//------------------------------------------------------------------------------
void CRMBrokerTrader::DumpCondition(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    WriteRiskLog("", "Dump %d Stock Condition Data", FConditionStockNoIndex.ItemCount());
    CRMTraderStockCondition* curConditionPtr = FConditionStockNoIndex.First();
    while (curConditionPtr != 0)
    {
        WriteRiskLog("", curConditionPtr->ToString(UFC::rwLockForRead).c_str());
        curConditionPtr = FConditionStockNoIndex.Next();
    }
    if (DoFlush) FlushRiskLog();
    FLock.Unlock(RWLockType);
}  //CRMBrokerTrader::DumpCondition()
//------------------------------------------------------------------------------
UFC::AnsiString CRMBrokerTrader::ToString(UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString line1Str, line2Str, line3Str, line4Str;
    FLock.Lock(RWLockType);
    if (FNeedCheckQuota)
        line1Str.Printf("BrokerTrader [%s] Authorize[%15.0lf] Cost([%15.0lf] Short[%15.0lf]) Need Check Quota.", FTraderID.c_str(), FAuthorizedAmount, FReservedCost, FReservedSellShortCost);
    else
        line1Str.Printf("BrokerTrader [%s] Authorize[%15.0lf] Cost([%15.0lf] Short[%15.0lf]) Need Check Quota.", FTraderID.c_str(), FAuthorizedAmount, FReservedCost, FReservedSellShortCost);
    line2Str.Printf(" Order Amount: Buy[%15.0lf] Sell[%15.0lf] BuyShort[%15.0lf] SellShort[%15.0lf]", FBuyOrderAmount, FSellOrderAmount, FBuyShortOrderAmount, FSellShortOrderAmount);
    line3Str.Printf("Active Amount: Buy[%15.0lf] Sell[%15.0lf] BuyShort[%15.0lf] SellShort[%15.0lf]", FBuyActiveAmount, FSellActiveAmount, FBuyShortActiveAmount, FSellShortActiveAmount);
    line4Str.Printf(" Match Amount: Buy[%15.0lf] Sell[%15.0lf] BuyShort[%15.0lf] SellShort[%15.0lf]", FBuyMatchAmount, FSellMatchAmount, FBuyShortMatchAmount, FSellShortMatchAmount);
    FLock.Unlock(RWLockType);
    UFC::AnsiString objStr = "";
    objStr.Printf("%s\n%s\n%s\n%s", line1Str.c_str(), line2Str.c_str(), line3Str.c_str(), line4Str.c_str());
    return objStr;    
}  //CRMBrokerTrader::ToString()
//------------------------------------------------------------------------------
void CRMBrokerTrader::DumpData(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    WriteRiskLog("", ToString(UFC::rwNotLock).c_str());
    DumpReservedStock(cstNotFlush, UFC::rwNotLock);
    DumpReservedSellShortStock(cstNotFlush, UFC::rwNotLock);
    DumpAccQuota(cstNotFlush, UFC::rwNotLock);
    DumpCondition(cstNotFlush, UFC::rwNotLock);
    WriteRiskLog("", " ");
    if (DoFlush) FlushRiskLog();
    FLock.Unlock(RWLockType);
}  //CRMBrokerTrader::DumpData()

//--------------------------- CUSunRiskManagerObject ---------------------------
bool CUSunRiskManagerObject::DivideStrByFirstDelimiter(const UFC::AnsiString& OrigStr, char Delimiter, UFC::AnsiString& BeforeStr, UFC::AnsiString& AfterStr)
{
    int origStrLen = OrigStr.Length();
    if (origStrLen <= 0) return false;
    
    int delimiterPos = OrigStr.AnsiPos(Delimiter);
    if (delimiterPos < 0)
    {   //Delimiter not Exist
        BeforeStr = "";
        AfterStr = OrigStr;
    }
    else if (delimiterPos == 0)
    {   //Delimiter is first Character
        BeforeStr = "";
        AfterStr = OrigStr.SubString(delimiterPos + 1, origStrLen - 1);
    }
    else
    {
        BeforeStr = OrigStr.SubString(0, delimiterPos);
        AfterStr = OrigStr.SubString(delimiterPos + 1, origStrLen - delimiterPos - 1);
    }
    return true;    
}  //CUSunRiskManagerObject::DivideStrByFirstDelimiter()
//------------------------------------------------------------------------------
bool CUSunRiskManagerObject::ParseOrderKey(const UFC::AnsiString& OrderKey, UFC::AnsiString& AE, UFC::AnsiString& TraderID)
{
    UFC::AnsiString userData = "";
    if (!DivideStrByFirstDelimiter(OrderKey, ',', AE, userData)) return false;
    
    UFC::AnsiString afterStr = "";
    if (userData.Length() > 0)
    {
        if (DivideStrByFirstDelimiter(userData, '^', TraderID, afterStr))
        {
            if (TraderID.Length() > 3)
                TraderID.SetLength(3);
            else
                TraderID.PadThis(3, ' ');
        }
        else
            return false;
    }
    return true;
}  //CUSunRiskManagerObject::ParseUserData()
//------------------------------------------------------------------------------
CUSunRiskManagerObject::CUSunRiskManagerObject(const UFC::AnsiString& RiskManagerName, const UFC::AnsiString& ConfigureFileName, bool IsTSEC)
:IBasicRiskManagerObject(RiskManagerName, ConfigureFileName, IsTSEC)
,FOperatorCostFileName("")
,FOperatorSellShortCostFileName("")
,FOperatorQuotaFileName("")
,FMainBrokerID("")
{
    LoadSetting();
    LoadOperatorCostData(false);
    LoadOperatorCostData(true);
    LoadOperatorQuotaData();
    PrintSetting();
}  //CUSunRiskManagerObject::CUSunRiskManagerObject()
//------------------------------------------------------------------------------
CUSunRiskManagerObject::~CUSunRiskManagerObject()
{
    ClearBrokerTraderIDIndex(cstDelete);
}  //CUSunRiskManagerObject::~CUSunRiskManagerObject()
//------------------------------------------------------------------------------
bool CUSunRiskManagerObject::LoadOperatorCostData(bool IsSellShort)
{
    UFC::AnsiString logHead = "LoadOperatorCostData()";
    UFC::AnsiString fullFileName;
    if (IsSellShort)
    {
        if (FOperatorSellShortCostFileName.Length() <= 0) return false;
        logHead = "LoadOperatorSellShortCostData()";
        fullFileName.Printf("%s/%s", FRMFilePath.c_str(), FOperatorSellShortCostFileName.c_str());        
    }
    else
    {
        if (FOperatorCostFileName.Length() <= 0) return false;
        logHead = "LoadOperatorCostData()";
        fullFileName.Printf("%s/%s", FRMFilePath.c_str(), FOperatorCostFileName.c_str());
    }

    if (UFC::FileExists(fullFileName) == FALSE)
    {
        WriteRiskLog(logHead, "Operator Cost File[%s] Not Exist.", fullFileName.c_str());
        FlushRiskLog();
        return false;
    }  //if (UFC::FileExists(fullFileName) == FALSE)

    WriteRiskLog(logHead, "Load Operator Cost File[%s].", fullFileName.c_str());
    FlushRiskLog();
    try 
    {
        UFC::FileStreamEx *ctrlFilePtr = new UFC::FileStreamEx(fullFileName, "r", FALSE);
        UFC::AnsiString ctrlDataStr = "";
        int ctrlRecordCount = 0;
        BOOL isNotEndOfFile = FALSE;
        do {
            isNotEndOfFile = ctrlFilePtr->ReadLine(ctrlDataStr);
            int recordLength = ctrlDataStr.Length();
            if (recordLength <= 0) continue;
            if (ctrlDataStr[recordLength - 1] == '\n') ctrlDataStr.SetLength(--recordLength); 
            if (ctrlDataStr[recordLength - 1] == '\r') ctrlDataStr.SetLength(--recordLength); 
            if (recordLength <= 0) continue;
            if (recordLength < 64) 
            {
                WriteRiskLog(logHead, "%d [%s]%d Wrong Length.", ctrlRecordCount, ctrlDataStr.c_str(), recordLength);
                continue;
            }
            
            ctrlRecordCount++;
            CRMReservedStock *newResvStockPtr = new CRMReservedStock(ctrlDataStr, IsSellShort);
            UFC::AnsiString curTraderID = newResvStockPtr->GetTraderID(UFC::rwNotLock);
            UFC::AnsiString curStockNo = newResvStockPtr->GetStockNo(UFC::rwNotLock);
            CRMBrokerTrader* curTraderPtr = FindBrokerTraderFromIDIndex(curTraderID, UFC::rwLockForRead);
            if (curTraderPtr == 0)
            {
                CRMBrokerTrader* newTraderPtr = new CRMBrokerTrader(curTraderID, ""); 
                if (!InsertBrokerTraderToIDIndex(curTraderID, newTraderPtr, cstCheck, UFC::rwLockForWrite))
                {
                    WriteRiskLog(logHead, "%d Insert Broker Trader[%s] failed.", ctrlRecordCount, curTraderID.c_str());
                    delete newTraderPtr;
                }
                else
                    curTraderPtr = newTraderPtr;
            }  //if (curTraderPtr == 0)
            
            if (curTraderPtr != 0)
            {                    
                CRMReservedStock *curResvStockPtr = 0;
                if (IsSellShort)
                    curResvStockPtr = curTraderPtr->FindReservedSellShortStockFromNoIndex(curStockNo, UFC::rwLockForRead);
                else
                    curResvStockPtr = curTraderPtr->FindReservedStockFromNoIndex(curStockNo, UFC::rwLockForRead);
                
                if (curResvStockPtr != 0)
                {
                    WriteRiskLog(logHead, "%d Broker Reserved Stock[%s %s] exists.", ctrlRecordCount, curTraderID.c_str(), curStockNo.c_str());
                    delete newResvStockPtr;
                    newResvStockPtr = 0;
                }
                else
                {
                    bool insertSuccess = false;
                    if (IsSellShort)
                        insertSuccess = curTraderPtr->InsertReservedSellShortStockToNoIndex(curStockNo, newResvStockPtr, cstNotCheck, UFC::rwLockForWrite);
                    else
                        insertSuccess = curTraderPtr->InsertReservedStockToNoIndex(curStockNo, newResvStockPtr, cstNotCheck, UFC::rwLockForWrite);
                        
                    if (insertSuccess)
                        WriteRiskLog(logHead, "%d [%s].", ctrlRecordCount, newResvStockPtr->ToString(UFC::rwNotLock).c_str());
                    else
                    {
                        WriteRiskLog(logHead, "%d Insert Broker Reserved Stock[%s] To [%s] failed.", ctrlRecordCount, curStockNo.c_str(), curTraderID.c_str());
                        delete newResvStockPtr;
                        newResvStockPtr = 0;
                    }
                }
            }  //if (curTraderPtr != 0)
            
            if (newResvStockPtr != 0)
            {
                CRMTraderStockCondition *curConditionPtr = curTraderPtr->FindConditionFromStockNoIndex(curStockNo, UFC::rwLockForRead);
                if (curConditionPtr == 0)
                {
                    CRMTraderStockCondition* newConditionPtr = new CRMTraderStockCondition(curStockNo, curTraderID); 
                    if (!curTraderPtr->InsertConditionToStockNoIndex(curStockNo, newConditionPtr, cstNotCheck, UFC::rwLockForWrite))
                    {
                        WriteRiskLog(logHead, "%d Insert Stock Condition[%s] To Trader[%s].", ctrlRecordCount, curStockNo.c_str(), curTraderID.c_str());
                        delete newConditionPtr;
                    }
                    else
                        curConditionPtr = newConditionPtr;
                }
                
                if (curConditionPtr != 0)
                {
                    if (IsSellShort)
                    {
                        curConditionPtr->SetAvailableSellShortQty(newResvStockPtr->GetAvailableQty(UFC::rwLockForRead), UFC::rwLockForWrite);
                        curConditionPtr->SetReservedSellShortQty(newResvStockPtr->GetReservedQty(UFC::rwLockForRead), UFC::rwLockForWrite);
                        curConditionPtr->SetReservedSellShortCost(newResvStockPtr->GetReservedCost(UFC::rwLockForRead), UFC::rwLockForWrite);
                        curTraderPtr->IncreaseReservedSellShortCost(newResvStockPtr->GetReservedCost(UFC::rwLockForRead), UFC::rwLockForWrite);
                    }
                    else
                    {
                        curConditionPtr->SetAvailableQty(newResvStockPtr->GetAvailableQty(UFC::rwLockForRead), UFC::rwLockForWrite);
                        curConditionPtr->SetReservedQty(newResvStockPtr->GetReservedQty(UFC::rwLockForRead), UFC::rwLockForWrite);
                        curConditionPtr->SetReservedCost(newResvStockPtr->GetReservedCost(UFC::rwLockForRead), UFC::rwLockForWrite);
                        curTraderPtr->IncreaseReservedCost(newResvStockPtr->GetReservedCost(UFC::rwLockForRead), UFC::rwLockForWrite);
                    }  //if (IsSellShort)
                }  //if (curConditionPtr != 0)
            }
            else
                WriteRiskLog(logHead, "%d Process [%s]%d failed.", ctrlRecordCount, ctrlDataStr.c_str(), recordLength);           
        } while(isNotEndOfFile == TRUE);
        delete ctrlFilePtr;
    }
    catch(UFC::FileException& e)
    {
        WriteRiskLog(logHead, "Process Risk Control File[%s] Exception:[%s].", fullFileName.c_str(), e.what());
    }  //try
    
    WriteRiskLog(logHead, " ");
    FlushRiskLog();
    return true;
}  //CUSunRiskManagerObject::LoadOperatorCostData()
//------------------------------------------------------------------------------
bool CUSunRiskManagerObject::LoadOperatorQuotaData()
{
    UFC::AnsiString logHead = "LoadOperatorQuotaData()";
    UFC::AnsiString fullFileName;
    fullFileName.Printf("%s/%s", FRMFilePath.c_str(), FOperatorQuotaFileName.c_str());

    if (UFC::FileExists(fullFileName) == FALSE)
    {
        WriteRiskLog(logHead, "Operator Quota File[%s] Not Exist.", fullFileName.c_str());
        FlushRiskLog();
        return false;
    }  //if (UFC::FileExists(fullFileName) == FALSE)

    WriteRiskLog(logHead, "Load Operator Quota File[%s].", fullFileName.c_str());
    FlushRiskLog();
    try 
    {
        UFC::FileStreamEx *ctrlFilePtr = new UFC::FileStreamEx(fullFileName, "r", FALSE);
        
        UFC::AnsiString ctrlDataStr = "";
        int ctrlRecordCount = 0;
        BOOL isNotEndOfFile = FALSE;
        do {
            isNotEndOfFile = ctrlFilePtr->ReadLine(ctrlDataStr);
            int recordLength = ctrlDataStr.Length();
            if (recordLength <= 0) continue;
            if (ctrlDataStr[recordLength - 1] == '\n') ctrlDataStr.SetLength(--recordLength); 
            if (ctrlDataStr[recordLength - 1] == '\r') ctrlDataStr.SetLength(--recordLength); 
            if (recordLength <= 0) continue;
            if (recordLength < 43) 
            {
                WriteRiskLog(logHead, "%d [%s]%d Wrong Length.", ctrlRecordCount, ctrlDataStr.c_str(), recordLength);
                continue;
            }
            
            ctrlRecordCount++;
            CRMBrokerTraderQuota* newQuotaPtr = new CRMBrokerTraderQuota(ctrlDataStr);
            UFC::AnsiString curTraderID = newQuotaPtr->GetTraderID(UFC::rwNotLock);
            UFC::AnsiString curAccountNo = newQuotaPtr->GetInvestorAccount(UFC::rwNotLock);
            double curAuthorizedAmount = newQuotaPtr->GetAuthorizedAmount(UFC::rwNotLock);
            CRMBrokerTrader* curTraderPtr = FindBrokerTraderFromIDIndex(curTraderID, UFC::rwLockForRead);
            if (curTraderPtr == 0)
            {
                CRMBrokerTrader* newTraderPtr = new CRMBrokerTrader(curTraderID, newQuotaPtr->GetTraderName(UFC::rwNotLock)); 
                if (!InsertBrokerTraderToIDIndex(curTraderID, newTraderPtr, cstCheck, UFC::rwLockForWrite))
                {
                    WriteRiskLog(logHead, "%d Insert Broker Trader[%s] failed.", ctrlRecordCount, curTraderID.c_str());
                    delete newTraderPtr;
                }
                else
                    curTraderPtr = newTraderPtr;
            }
            else
                curTraderPtr->SetTraderName(newQuotaPtr->GetTraderName(UFC::rwNotLock), UFC::rwLockForWrite);
            
            if (curTraderPtr != 0)
            {
                CRMBrokerTraderQuota* curQuotaPtr = curTraderPtr->FindAccQuotaFromNoIndex(curAccountNo, UFC::rwLockForRead);
                if (curQuotaPtr != 0)
                {
                    WriteRiskLog(logHead, "%d Broker Trader Quota[%s %s] exists.", ctrlRecordCount, curTraderID.c_str(), curAccountNo.c_str());
                    delete newQuotaPtr;
                    newQuotaPtr = 0;
                }
                else
                {
                    if (curTraderPtr->InsertAccQuotaToNoIndex(curAccountNo, newQuotaPtr, cstNotCheck, UFC::rwLockForWrite))
                    {
                        double origAuthorizedAmount = curTraderPtr->GetAuthorizedAmount(UFC::rwLockForRead);
                        double newAuthorizedAmount = origAuthorizedAmount + curAuthorizedAmount; 
                        curTraderPtr->SetAuthorizedAmount(newAuthorizedAmount, UFC::rwLockForWrite);
                        WriteRiskLog(logHead, "%d [%s]. AuthorizedAmount[%15.0lf]->[%15.0lf]", ctrlRecordCount, newQuotaPtr->ToString(UFC::rwNotLock).c_str(), origAuthorizedAmount, newAuthorizedAmount);            
                    }                        
                    else
                    {
                        WriteRiskLog(logHead, "%d Insert Broker Trader Quota[%s] To [%s] failed.", ctrlRecordCount, curAccountNo.c_str(), curTraderID.c_str());
                        delete newQuotaPtr;
                        newQuotaPtr = 0;
                    }
                }
            }  //if (curTraderPtr != 0)
            
            if (newQuotaPtr == 0)
                WriteRiskLog(logHead, "%d Process [%s]%d failed.", ctrlRecordCount, ctrlDataStr.c_str(), recordLength);           
        } while(isNotEndOfFile == TRUE);
        delete ctrlFilePtr;
    }
    catch(UFC::FileException& e)
    {
        WriteRiskLog(logHead, "Process Risk Control File[%s] Exception:[%s].", fullFileName.c_str(), e.what());
    }  //try
    DumpBrokerTrader(true, UFC::rwLockForWrite);
    
    WriteRiskLog(logHead, " ");
    FlushRiskLog();
    return true;
}  //CUSunRiskManagerObject::LoadOperatorQuotaData()
//------------------------------------------------------------------------------
bool CUSunRiskManagerObject::LoadSetting()
{
    UFC::AnsiString logHead = "CUSunRiskManagerObject::LoadSetting()";
    UFC::AnsiString fullConfigureFileName;
    fullConfigureFileName.Printf("../cfg/%s", FConfigureFileName.c_str());
    UFC::BufferedLog::Printf(" %s Open ConfigureFile[%s]", logHead.c_str(), fullConfigureFileName.c_str());
    UFC::UiniFile *cfgFilePtr = 0;
    try
    {
        cfgFilePtr = new UFC::UiniFile(fullConfigureFileName, FALSE);
    }
    catch (UFC::FileException& e)
    {
        UFC::BufferedLog::Printf(" %s Open ConfigureFile[%s] Failed:%s", logHead.c_str(), fullConfigureFileName.c_str(), e.what());
        if (cfgFilePtr != 0) delete cfgFilePtr;
        return false;
    }
    
    UFC::AnsiString settingSectionName = "Setting";
    UFC::Section *settingSecPtr = cfgFilePtr->GetSection(settingSectionName);
    if (settingSecPtr == 0)
    {
        FIsRMControlSetting = false;
        UFC::BufferedLog::Printf(" %s Section[%s] Not Exist.", logHead.c_str(), settingSectionName.c_str() );        
        return false;
    }
    
    FIsRMControlSetting = true;
    UFC::AnsiString fieldName, fieldValue;
    fieldName = "Name";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FRMName = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value[%s]", logHead.c_str(), fieldName.c_str(), FRMName.c_str());
    
    fieldName = "RMFilePath";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FRMFilePath = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value[%s]", logHead.c_str(), fieldName.c_str(), FRMFilePath.c_str());
    
    fieldName = "OperatorCostFileName";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FOperatorCostFileName = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value[%s]", logHead.c_str(), fieldName.c_str());
    
    fieldName = "OperatorSellShortCostFileName";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FOperatorSellShortCostFileName = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value[%s]", logHead.c_str(), fieldName.c_str());
    
    fieldName = "OperatorQuotaFileName";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FOperatorQuotaFileName = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value[%s]", logHead.c_str(), fieldName.c_str(), FOperatorQuotaFileName.c_str());
        
    fieldName = "MainBrokerID";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FMainBrokerID = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value[%s]", logHead.c_str(), fieldName.c_str(), FMainBrokerID.c_str());
    
    fieldName = "CheckSelfTrading";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
    {
        if ((fieldValue == "Y") || (fieldValue == "y")) 
            FNeedCheckSelfTrading = true;
        else
            FNeedCheckSelfTrading = false;
    }
    else
        UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value [%s]", logHead.c_str(), fieldName.c_str(), DescribeBoolean(FNeedCheckSelfTrading).c_str());
    
    UFC::AnsiString operatorAccountSectionName = "OperatorAccount";
    UFC::Section* operatorAccountSecPtr = cfgFilePtr->GetSection(operatorAccountSectionName);
    if (operatorAccountSecPtr != 0)
    {
        fieldName = "DisableQuotaCheck";
        if (operatorAccountSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        {
            UFC::PStringList valueList;
            valueList.SetStrings(fieldValue, ",\n");
            for (int i = 0; i < valueList.ItemCount(); i++)
            {
                UFC::AnsiString curTraderID = valueList[i];
                UFC::BufferedLog::Printf(" %s Field[%s] %d[%s].", logHead.c_str(), fieldName.c_str(), i, curTraderID.c_str());
                CRMBrokerTrader* curTraderPtr = FindBrokerTraderFromIDIndex(curTraderID, UFC::rwLockForRead);
                if (curTraderPtr == 0)
                {
                    CRMBrokerTrader* newTraderPtr = new CRMBrokerTrader(curTraderID, "", false); 
                    if (!InsertBrokerTraderToIDIndex(curTraderID, newTraderPtr, cstCheck, UFC::rwLockForWrite))
                    {
                        WriteRiskLog(logHead, "%2d Insert Broker Trader[%s] failed.", i, curTraderID.c_str());
                        delete newTraderPtr;
                    }
                }
                else
                    curTraderPtr->SetNeedCheckQuota(false, UFC::rwLockForWrite);
            }
        }
        else
            UFC::BufferedLog::Printf(" %s Field[%s] Not Exist.", logHead.c_str(), fieldName.c_str());
    }
    else
    {
        UFC::BufferedLog::Printf(" %s Section[%s] Not Exist.", logHead.c_str(), operatorAccountSectionName.c_str());
    }
    
    UFC::AnsiString investorAccountSectionName = "InvestorAccount";
    UFC::Section* investorAccountSecPtr = cfgFilePtr->GetSection(investorAccountSectionName);
    if (investorAccountSecPtr == 0)
    {
        UFC::BufferedLog::Printf(" %s Section[%s] Not Exist.", logHead.c_str(), investorAccountSectionName.c_str());
        return false;
    }
    
    ClearInvestorAccountNoIndex(cstDelete, UFC::rwLockForWrite);
    for (int i = 0; i < investorAccountSecPtr->ItemCount(); i++)
    {
        if (investorAccountSecPtr->GetNameValue(i, fieldName, fieldValue) == FALSE) continue;
        int dayTradeType = 0;   //no DayTrade
        int selfTradeType = 0;  //not Check Self Trade
        int sellPositionCheckType = 1;  //Check Position
        UFC::PStringList valueList;
        valueList.SetStrings(fieldValue, ",\n");
        if (valueList.ItemCount() >= 1) dayTradeType = valueList[0].ToInt();
        if (valueList.ItemCount() >= 2) selfTradeType = valueList[1].ToInt();
        if (valueList.ItemCount() >= 3) sellPositionCheckType = valueList[2].ToInt();
        CRMInvestorAccount *accountPtr = new CRMInvestorAccount(fieldName, dayTradeType, selfTradeType, sellPositionCheckType);
        if (!InsertInvestorAccountToNoIndex(fieldName, accountPtr, cstCheck, UFC::rwLockForWrite))
        {
            UFC::BufferedLog::Printf(" %s Insert Account[%s] dayTrade[%d] selfTrade[%d] failed.", logHead.c_str(), fieldName.c_str(), dayTradeType, selfTradeType);
            delete accountPtr;
        }
    }  //for (int i = 0; i < investorAccountSecPtr->ItemCount(); i++)
    
    return true;
}  //CUSunRiskManagerObject::LoadSetting()
//------------------------------------------------------------------------------
void CUSunRiskManagerObject::PrintSetting()
{
    WriteRiskLog("", "---------- %s RiskManager ----------", FRMName.c_str());
    WriteRiskLog("", "Risk Manager File Path:[%s]", FRMFilePath.c_str());
    WriteRiskLog("", "Control File: Cost[%s]", FOperatorCostFileName.c_str());
    WriteRiskLog("", "Control File: Sell Short Cost[%s]", FOperatorSellShortCostFileName.c_str());
    WriteRiskLog("", "Control File: Quota[%s]", FOperatorQuotaFileName.c_str());
    WriteRiskLog("", "Broker ID[%s]", FMainBrokerID.c_str());
    if (FNeedCheckSelfTrading)
        WriteRiskLog("", "Check Self Trading");
    DumpBrokerTrader(cstNotFlush, UFC::rwLockForRead);
    DumpInvestorAccount(cstFlush, UFC::rwLockForRead);
}  //CUSunRiskManagerObject::PrintSetting()
//------------------------------------------------------------------------------
void CUSunRiskManagerObject::DumpBrokerTrader(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    WriteRiskLog("", "Dump %d BrokerTrader Data", FBrokerTraderIDIndex.ItemCount());
    CRMBrokerTrader *curTraderPtr = FBrokerTraderIDIndex.First();
    while (curTraderPtr != 0)
    {
        curTraderPtr->DumpData(cstNotFlush, UFC::rwLockForRead);
        curTraderPtr = FBrokerTraderIDIndex.Next();
    }
    if (DoFlush) FlushRiskLog();
    FLock.Unlock(RWLockType);
}  //CUSunRiskManagerObject::DumpBrokerTrader()
//------------------------------------------------------------------------------
bool CUSunRiskManagerObject::CheckRiskCondition(TWSE::MessageHeader*& Msg, OrderRecord& OI, const UFC::AnsiString& PVCID, UFC::AnsiString& CheckMessage)
{
    int functionCode = OI.FuncCode;
    UFC::AnsiString orderKey = OI.Key;
    TWSE::TMessageType msgType = static_cast<TWSE::TMessageType>(OI.MsgType);
    if (((msgType != TWSE::mtT010) && (msgType != TWSE::mtO010) && (msgType != TWSE::mtP010)) || 
        ((functionCode != 1) && (functionCode != 2))) 
        return true;

    CheckMessage = "";
    UFC::AnsiString logHead;
    logHead.Printf("CheckRiskCondition() PVC:%s", PVCID.c_str());
    UFC::AnsiString warrantNo = "";
    UFC::AnsiString brokerId = "";
    
    UFC::AnsiString orderId, accountNo, stockNo, orderQtyStr, priceStr;
    char buySellCode = ' ', exchangeCode = ' ', orderType = ' ';

    if (msgType == TWSE::mtT010)
    {
        TWSE::MessageT010 *t010MsgPtr = (TWSE::MessageT010*)Msg;
        orderId = UFC::AnsiString(t010MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(t010MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(t010MsgPtr->StockID, 6);
        buySellCode = t010MsgPtr->BuySellCode;
        exchangeCode = t010MsgPtr->ExchangeCode;
        orderType = t010MsgPtr->OrderType;
        orderQtyStr = UFC::AnsiString(t010MsgPtr->OrderQty, 3);
        priceStr = UFC::AnsiString(t010MsgPtr->OrderPrice, 6);
    }
    else if (msgType == TWSE::mtO010)
    {
        TWSE::MessageO010 *o010MsgPtr = (TWSE::MessageO010*)Msg;
        orderId = UFC::AnsiString(o010MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(o010MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(o010MsgPtr->StockID, 6);
        buySellCode = o010MsgPtr->BuySellCode;
        exchangeCode = '2';
        orderType = '0';
        orderQtyStr = UFC::AnsiString(o010MsgPtr->OrderQty, 3);
        priceStr = UFC::AnsiString(o010MsgPtr->OrderPrice, 6);
    }
    else if (msgType == TWSE::mtP010)
    {
        TWSE::MessageP010 *p010MsgPtr = (TWSE::MessageP010*)Msg;
        orderId = UFC::AnsiString(p010MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(p010MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(p010MsgPtr->StockID, 6);
        buySellCode = p010MsgPtr->BuySellCode;
        exchangeCode = p010MsgPtr->ExchangeCode;
        orderType = p010MsgPtr->OrderType;
        orderQtyStr = UFC::AnsiString(p010MsgPtr->OrderQty, 3);
        priceStr = UFC::AnsiString(p010MsgPtr->OrderPrice, 6);
    }
    accountNo.TrimRight(' ');
    stockNo.TrimRight(' ');
    int orderQty = orderQtyStr.ToInt();
    if (exchangeCode != '2') orderQty *= 1000;
    double orderPrice = Parse94v99PriceStr(priceStr); 
    double orderAmount = orderPrice * orderQty;    
    WriteLevelRiskLog(UFC::dlInformation, logHead, "Check Order[%s] Stock[%s]%c %c Qty[%d] Px[%11.3lf] func[%d] ExecCode[%c] Acc[%s] msgType[%d] OrderKey[%s]", 
                      orderId.c_str(), stockNo.c_str(), orderType, buySellCode, orderQty, orderPrice, functionCode, exchangeCode, accountNo.c_str(), msgType, orderKey.c_str());
    
    CRMInvestorAccount *accountPtr = FindInvestorAccountFromNoIndex(accountNo, UFC::rwLockForRead);
    if (accountPtr == 0)
    {
        CheckMessage.Printf("Reject[%s]: Account[%s] not found", orderId.c_str(), accountNo.c_str());
        WriteRiskLog(logHead, CheckMessage);
        FlushRiskLog();
        return false;
    }  //if (accountPtr == 0)
    
    CRMStockTradingData *stockDataPtr = accountPtr->FindOrCreateStockData(stockNo, UFC::rwLockForWrite);
    if (stockDataPtr == 0)
    {
        CheckMessage.Printf("Reject[%s]: Wrong Account[%s] Stock[%s]", orderId.c_str(), accountNo.c_str(), stockNo.c_str());
        WriteRiskLog(logHead, CheckMessage);
        FlushRiskLog();
        return false;
    }
        
    bool isPassed = true;
    int selfTradeType = accountPtr->GetSelfTradeType(UFC::rwLockForRead);
    if (NeedCheckSelfTrading() && (selfTradeType == 1))
    {
        if (buySellCode == 'B')
        {
            double minSellOrderPrice = 0.0;
            if (msgType == TWSE::mtT010)
                minSellOrderPrice = stockDataPtr->GetMinNormalSellOrderPrice(UFC::rwLockForRead);
            else
                minSellOrderPrice = stockDataPtr->GetMinSellOrderPrice(UFC::rwLockForRead);
            if (orderPrice < minSellOrderPrice)
                WriteLevelRiskLog(UFC::dlPerformance, logHead, "[%s] Passed: Px[%11.3lf] < MinSellOrderPx[%11.3lf]", orderId.c_str(), orderPrice, minSellOrderPrice);
            else
            {
                CheckMessage.Printf("Reject[%s]: Px[%11.3lf] >= MinSellOrderPx[%11.3lf]", orderId.c_str(), orderPrice, minSellOrderPrice);
                isPassed = false;
            }
        }
        else if (buySellCode == 'S')
        {
            double maxBuyOrderPrice = 0.0;
            if (msgType == TWSE::mtT010)
                maxBuyOrderPrice = stockDataPtr->GetMaxNormalBuyOrderPrice(UFC::rwLockForRead);
            else
                maxBuyOrderPrice = stockDataPtr->GetMaxBuyOrderPrice(UFC::rwLockForRead);
            if (orderPrice > maxBuyOrderPrice)
                WriteLevelRiskLog(UFC::dlPerformance, logHead, "[%s] Passed: Px[%11.3lf] > MinBuyOrderPx[%11.3lf]", orderId.c_str(), orderPrice, maxBuyOrderPrice);
            else
            {
                CheckMessage.Printf("Reject[%s]: Px[%11.3lf] <= MaxBuyOrderPx[%11.3lf]", orderId.c_str(), orderPrice, maxBuyOrderPrice);
                isPassed = false;
            }
        }
        else
        {
            CheckMessage.Printf("Reject[%s]: Wrong Side[%c]", orderId.c_str(), buySellCode);
            isPassed = false;            
        }  //if (buySellCode == 'B')
    }  //if (ObjectManager->NeedCheckSelfTrading(UFC::rwLockForRead) && (selfTradeType == 1))

    if (!isPassed) 
    {
        WriteRiskLog(logHead, "%s, MsgType[%d]", CheckMessage.c_str(), msgType);        
        FlushRiskLog();
        return isPassed;
    }
    
    isPassed = false;
    UFC::AnsiString ae, traderID;
    CRMBrokerTrader* traderPtr = 0;
    CRMTraderStockCondition* conditionPtr = 0;
    if (ParseOrderKey(orderKey, ae, traderID))
    {
        if (traderID.Length() > 0)
        {
            traderPtr = FindBrokerTraderFromIDIndex(traderID, UFC::rwLockForRead);
            if (traderPtr != 0)
            {
                conditionPtr = traderPtr->FindConditionFromStockNoIndex(stockNo, UFC::rwLockForRead);
                if (traderPtr->NeedCheckQuota(UFC::rwLockForRead))
                {
                    int availableQty = 0, confirmedOrderQty = 0, matchQty = 0;
                    bool needCheckQty = true;
                    if ((orderType == '0') && (buySellCode == 'S'))
                    {
                        if (conditionPtr != 0)
                        {
                            availableQty = conditionPtr->GetAvailableQty(UFC::rwLockForRead);
                            confirmedOrderQty = conditionPtr->GetSellOrderQty(UFC::rwLockForRead);
                            matchQty = conditionPtr->GetSellMatchQty(UFC::rwLockForRead);
                        }
                    }
                    else if ((orderType == '5') || (orderType == '6'))
                    {
                        if (conditionPtr != 0)
                        {
                            if (buySellCode == 'S')
                            {
                                availableQty = conditionPtr->GetAvailableSellShortQty(UFC::rwLockForRead);
                                confirmedOrderQty = conditionPtr->GetSellShortOrderQty(UFC::rwLockForRead);
                                matchQty = conditionPtr->GetSellShortMatchQty(UFC::rwLockForRead);
                            }
                            else
                            {
                                availableQty = conditionPtr->GetReservedSellShortQty(UFC::rwLockForRead);
                                confirmedOrderQty = conditionPtr->GetBuyShortOrderQty(UFC::rwLockForRead);
                                matchQty = conditionPtr->GetBuyShortMatchQty(UFC::rwLockForRead);
                            }
                        }
                    }
                    else
                    {
                        needCheckQty = false;
                        WriteRiskLog(logHead, "Trader[%s] Stock[%s] OrderType[%c] Side[%c] need not Check Quota.", traderID.c_str(), stockNo.c_str(), orderType, buySellCode);        
                        isPassed = true;
                    }
                        
                    if (needCheckQty)
                    {
                        if (conditionPtr != 0)
                        {
                            if (availableQty >= confirmedOrderQty + matchQty + orderQty)
                            {
                                isPassed = true;
                                WriteLevelRiskLog(UFC::dlPerformance, logHead, "[%s] Passed: AvailableQty[%d] >= OrderQty[%d] + MatchQty[%d] + NewQty[%d]", orderId.c_str(), availableQty, confirmedOrderQty, matchQty, orderQty);
                            }
                            else
                                CheckMessage.Printf("Reject[%s]: AvailableQty[%d] < OrderQty[%d] + MatchQty[%d] + NewQty[%d]", orderId.c_str(), availableQty, confirmedOrderQty, matchQty, orderQty);
                        }
                        else
                            CheckMessage.Printf("Reject[%s]: Trader[%s] Stock[%s] Condition not Exist", orderId.c_str(), traderID.c_str(), stockNo.c_str());
                    }  //if (needCheckQty)
                    
                    if (isPassed)
                    {
                        traderPtr->LockForRead();
                        double authorizedAmount = traderPtr->GetAuthorizedAmount(UFC::rwNotLock);
                        double reservedCost = traderPtr->GetReservedCost(UFC::rwNotLock);
                        double sellMatchAmount = traderPtr->GetSellMatchAmount(UFC::rwNotLock);
                        double buyMatchAmount = traderPtr->GetBuyMatchAmount(UFC::rwNotLock);
                        double buyOrderAmount = traderPtr->GetBuyOrderAmount(UFC::rwNotLock);
                        double sellShortMatchAmount = traderPtr->GetSellShortMatchAmount(UFC::rwNotLock);
                        double buyShortMatchAmount = traderPtr->GetBuyShortMatchAmount(UFC::rwNotLock);
                        double sellShortOrderAmount = traderPtr->GetSellShortOrderAmount(UFC::rwNotLock);
                        traderPtr->UnlockForRead();
                        double availableAmount = authorizedAmount - reservedCost + sellMatchAmount - buyMatchAmount - buyOrderAmount - sellShortMatchAmount + buyShortMatchAmount - sellShortOrderAmount;
                        WriteLevelRiskLog(UFC::dlInformation, logHead, 
                                          "available[%15.0lf] = authorized[%15.0lf] - reserved[%15.0lf] + sellMatch[%15.0lf] - buyMatch[%15.0lf] - buyOrder[%15.0lf] - sellShortMatch[%15.0lf] + buyShortMatch[%15.0lf] - sellShortOrderA[%15.0lf]", 
                                          availableAmount, authorizedAmount, reservedCost, sellMatchAmount, buyMatchAmount, buyOrderAmount, sellShortMatchAmount, buyShortMatchAmount, sellShortOrderAmount);
                        if (availableAmount >= orderAmount)
                            WriteLevelRiskLog(UFC::dlPerformance, logHead, "[%s] Passed: availableAmount[%15.0lf] >= orderAmount[%15.0lf]", orderId.c_str(), availableAmount, orderAmount);
                        else
                        {
                            isPassed = false;
                            CheckMessage.Printf("Reject[%s]: availableAmount[%15.0lf] < orderAmount[%15.0lf]", orderId.c_str(), availableAmount, orderAmount);
                        }
                    }  //if (isPassed)
                }
                else
                {
                    isPassed = true;
                    WriteRiskLog(logHead, "Trader[%s] Need not Check Quota.", traderID.c_str(), stockNo.c_str());        
                }
            }
            else
                CheckMessage.Printf("Reject[%s]: Trader[%s] not Exist", orderId.c_str(), traderID.c_str());
        }
        else
            CheckMessage.Printf("Reject[%s]: TraderID is empty.", orderId.c_str());
    }
    else
        CheckMessage.Printf("Reject[%s]: Parse Order Key[%s] failed.", orderId.c_str(), orderKey.c_str());
    
    if (isPassed)
    {
        CRMOrderSummary *orderPtr = new CRMOrderSummary(orderId, buySellCode, stockNo, warrantNo, brokerId, orderType, orderPrice, orderQty, exchangeCode, msgType, accountNo);
        orderPtr->SetOrderKey(orderKey, UFC::rwNotLock);
        orderPtr->SetTraderID(traderID, UFC::rwNotLock);
        if (!InsertOrderIntoIDIndex(orderId, orderPtr, cstCheck, UFC::rwLockForWrite))
            WriteRiskLog(logHead, "Insert Order[%s] into failed.", orderId.c_str());
        
        bool isInsertOrderSuccess = false;
        if (functionCode == 1)
        {
            if (msgType == TWSE::mtT010)
                isInsertOrderSuccess = stockDataPtr->InsertNormalBuyOrder(orderId, orderPtr, cstCheck, UFC::rwLockForWrite);
            else
                isInsertOrderSuccess = stockDataPtr->InsertBuyOrder(orderId, orderPtr, cstCheck, UFC::rwLockForWrite);
        }
        else
        {
            if (msgType == TWSE::mtT010)
                isInsertOrderSuccess = stockDataPtr->InsertNormalSellOrder(orderId, orderPtr, cstCheck, UFC::rwLockForWrite);
            else
                isInsertOrderSuccess = stockDataPtr->InsertSellOrder(orderId, orderPtr, cstCheck, UFC::rwLockForWrite);
        }
        
        if (isInsertOrderSuccess)
            WriteLevelRiskLog(UFC::dlInformation, logHead, "Stock: %s, Order[%s], MsgType[%d]", stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str(), OI.MsgType);
        else
            WriteRiskLog(logHead, "Insert Order[%s] into Stock[%s] Data failed.", orderId.c_str(), stockNo.c_str());
        
        if (conditionPtr != 0)
        {
            conditionPtr->AddNewOrderData(orderQty, orderPrice, buySellCode, orderType, UFC::rwLockForWrite);
            WriteLevelRiskLog(UFC::dlInformation, logHead, "%s", conditionPtr->ToString(UFC::rwLockForRead).c_str());
        }  //if (conditionPtr != 0)
        
        traderPtr->AddNewOrderData(orderQty, orderPrice, buySellCode, orderType, UFC::rwLockForWrite);
        WriteLevelRiskLog(UFC::dlInformation, logHead, "%s", traderPtr->ToString(UFC::rwLockForRead).c_str());
    }
    else
        WriteRiskLog(logHead, "%s, MsgType[%d]", CheckMessage.c_str(), msgType);        
    FlushRiskLog();
    return isPassed;
}  //CUSunRiskManagerObject::CheckRiskCondition()
//----------------------------------------------------------------------------------------------------------------------
void CUSunRiskManagerObject::AddRiskMatchQty(TWSE::R3Body *R3MsgBodyPtr, const UFC::AnsiString& PVCId, const UFC::AnsiString& ExecKey)
{
    UFC::AnsiString logHead;
    if (PVCId.Length() > 0)
        logHead.Printf("AddRiskMatchQty() PVC:%s", PVCId.c_str());
    else
        logHead.Printf("AddRiskMatchQty()");
    UFC::AnsiString logMsg = "";
    
    UFC::AnsiString orderId(R3MsgBodyPtr->FOrderNo, 5);
    char exchangeCode = R3MsgBodyPtr->FEXCD;
    
    UFC::AnsiString matchQtyStr(R3MsgBodyPtr->FQuntity, 8);
    int matchQty = matchQtyStr.ToInt();
    if (exchangeCode != '2') matchQty *= 1000;
    
    UFC::AnsiString matchPriceStr(R3MsgBodyPtr->FPrice, 6);
    double matchPrice = Parse94v99PriceStr(matchPriceStr); 
    WriteLevelRiskLog(UFC::dlInformation, logHead, "Process Match Order[%s] matchQty=%d matchPrice=%15.0lf", orderId.c_str(), matchQty, matchPrice);
    
    CRMOrderSummary *orderPtr = FindOrderFromIDIndex(orderId, UFC::rwLockForRead);
    if (orderPtr != 0)
    {
        orderPtr->LockForRead();
        UFC::AnsiString accountNo = orderPtr->GetAccountNo(UFC::rwNotLock);
        UFC::AnsiString stockNo = orderPtr->GetStockNo(UFC::rwNotLock);
        UFC::AnsiString warrantNo = orderPtr->GetWarrantNo(UFC::rwNotLock);
        UFC::AnsiString brokerId = orderPtr->GetBrokerId(UFC::rwNotLock);
        char side = orderPtr->GetSide(UFC::rwNotLock);
        char twseOrderType = orderPtr->GetTWSEOrderType(UFC::rwNotLock);
        int origLeavesQty = orderPtr->GetLeavesOrderQty(UFC::rwNotLock);
        double orderPrice = orderPtr->GetOrderPrice(UFC::rwNotLock);
        TWSE::TMessageType msgType = orderPtr->GetMsgType(UFC::rwNotLock);
        UFC::AnsiString traderID = orderPtr->GetTraderID(UFC::rwNotLock);
        orderPtr->UnlockForRead();
        
        if (origLeavesQty > 0)
        {
            if (origLeavesQty >= matchQty)
            {
                int newLeavesQty = origLeavesQty - matchQty;
                orderPtr->SetLeavesQty(newLeavesQty, UFC::rwLockForWrite);
                CRMInvestorAccount *accountPtr = FindInvestorAccountFromNoIndex(accountNo, UFC::rwLockForRead);
                if (accountPtr != 0)
                {
                    CRMStockTradingData *stockDataPtr = accountPtr->FindStockData(stockNo, UFC::rwLockForRead);
                    if (stockDataPtr != 0)
                    {
                        if (newLeavesQty <= 0)
                        {
                            CRMOrderSummary *filledOrderPtr = 0;
                            if (side == 'B')
                            {
                                if (msgType == TWSE::mtT010)
                                    filledOrderPtr = stockDataPtr->RemoveNormalBuyOrder(orderId, UFC::rwLockForWrite);
                                else
                                    filledOrderPtr = stockDataPtr->RemoveBuyOrder(orderId, UFC::rwLockForWrite);
                            }
                            else
                            {
                                if (msgType == TWSE::mtT010)
                                    filledOrderPtr = stockDataPtr->RemoveNormalSellOrder(orderId, UFC::rwLockForWrite);
                                else
                                    filledOrderPtr = stockDataPtr->RemoveSellOrder(orderId, UFC::rwLockForWrite);
                            }
                    
                            if (filledOrderPtr != 0)
                                WriteLevelRiskLog(UFC::dlInformation, logHead, "StockData: %s, Match Order[%s] Qty[%d] ExecKey[%s]", stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str(), matchQty, ExecKey.c_str());
                            else
                                WriteRiskLog(logHead, "Remove Order[%s] from account[%s] Stock[%s] failed, Match Order[%s]", orderId.c_str(), accountNo.c_str(), stockNo.c_str(), orderId.c_str());
                        }  //if (newLeavesQty <= 0)                        
                    }
                    else
                        WriteRiskLog(logHead, "Account[%s] has no Stock[%s] data, Match Order[%s]", accountNo.c_str(), stockNo.c_str(), orderId.c_str());
                }
                else
                    WriteRiskLog(logHead, "Account[%s] of Match Order[%s] not found", accountNo.c_str(), orderId.c_str());
                
                if (traderID.Length() > 0)
                {
                    CRMBrokerTrader* traderPtr = FindBrokerTraderFromIDIndex(traderID, UFC::rwLockForRead);
                    if (traderPtr != 0)
                    {
                        CRMTraderStockCondition* conditionPtr = traderPtr->FindConditionFromStockNoIndex(stockNo, UFC::rwLockForRead);
                        if (conditionPtr != 0)
                        {
                            conditionPtr->AddMatchOrderData(matchQty, matchPrice, orderPrice, side, twseOrderType, UFC::rwLockForWrite);
                            WriteLevelRiskLog(UFC::dlInformation, logHead, "%s", conditionPtr->ToString(UFC::rwLockForRead).c_str());
                        }
                        else
                            WriteRiskLog(logHead, "Trader[%s] Stock[%s] Condition not Found", traderID.c_str(), stockNo.c_str());  
                        
                        traderPtr->AddMatchOrderData(matchQty, matchPrice, orderPrice, side, twseOrderType, UFC::rwLockForWrite);
                        WriteLevelRiskLog(UFC::dlInformation, logHead, "%s", traderPtr->ToString(UFC::rwLockForRead).c_str());
                    }
                    else
                        WriteRiskLog(logHead, "Trader[%s] not Found", traderID.c_str());                
                }
                else
                    WriteRiskLog(logHead, "TraderID is Empty");
            }
            else
                WriteRiskLog(logHead, "Wrong Order[%s]. LeavesQty[%d] < MatchQty[%d]", orderId.c_str(), origLeavesQty, matchQty);
        }
        else
            WriteRiskLog(logHead, "Order[%s] is Filled. LeavesQty[%d] <= 0", orderId.c_str(), origLeavesQty);
    }
    else
        WriteRiskLog(logHead, "Order[%s] not Found", orderId.c_str());
    
    FlushRiskLog();
}  //CUSunRiskManagerObject::AddRiskMatchQty()
//---------------------------------------------------------------------------------------------------------------------
void CUSunRiskManagerObject::ProcessRiskCancelReplaceMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI, const UFC::AnsiString PVCID)
{
    int functionCode = OI.FuncCode;
    if ((functionCode != 3) && (functionCode != 4)) return;
    
    UFC::AnsiString logHead;
    logHead.Printf("ProcessRiskCancelReplaceMessage() PVC:%s", PVCID.c_str());
    TWSE::TMessageType msgType = ReplyMsg->MessageType();
    int msgStatusCode = ReplyMsg->GetStatusCode();
    UFC::AnsiString accountNo, stockNo, orderId, orderQtyStr, beforeQtyStr, afterQtyStr, orderPriceStr;
    char buySellCode, exchangeCode, orderType;
    if (msgType == TWSE::mtT020)
    {
        TWSE::MessageT020 *t020MsgPtr = static_cast<TWSE::MessageT020*>(ReplyMsg);
        accountNo = UFC::AnsiString(t020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(t020MsgPtr->StockID, 6);
        orderId = UFC::AnsiString(t020MsgPtr->OrderNo, 5);
        buySellCode = t020MsgPtr->BuySellCode;
        exchangeCode = t020MsgPtr->ExchangeCode;
        orderType = t020MsgPtr->OrderType;
        orderQtyStr = UFC::AnsiString(t020MsgPtr->OrderQty, 3);
        beforeQtyStr = UFC::AnsiString(t020MsgPtr->BeforeQty, 3);
        afterQtyStr = UFC::AnsiString(t020MsgPtr->AfterQty, 3);
        orderPriceStr = UFC::AnsiString(t020MsgPtr->OrderPrice, 6);
    }
    else if (msgType == TWSE::mtO020)
    {
        TWSE::MessageO020 *o020MsgPtr = static_cast<TWSE::MessageO020*>(ReplyMsg);
        accountNo = UFC::AnsiString(o020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(o020MsgPtr->StockID, 6);
        orderId = UFC::AnsiString(o020MsgPtr->OrderNo, 5);
        buySellCode = o020MsgPtr->BuySellCode;
        exchangeCode = '2';
        orderType = '0';
        orderQtyStr = UFC::AnsiString(o020MsgPtr->OrderQty, 8);
        beforeQtyStr = UFC::AnsiString(o020MsgPtr->BeforeQty, 8);
        afterQtyStr = UFC::AnsiString(o020MsgPtr->AfterQty, 8);
        orderPriceStr = UFC::AnsiString(o020MsgPtr->OrderPrice, 6);
    }
    else if (msgType == TWSE::mtP020)
    {
        TWSE::MessageP020 *p020MsgPtr = static_cast<TWSE::MessageP020*>(ReplyMsg);
        accountNo = UFC::AnsiString(p020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(p020MsgPtr->StockID, 6);
        orderId = UFC::AnsiString(p020MsgPtr->OrderNo, 5);
        buySellCode = p020MsgPtr->BuySellCode;
        exchangeCode = p020MsgPtr->ExchangeCode;
        orderType = p020MsgPtr->OrderType;
        orderQtyStr = UFC::AnsiString(p020MsgPtr->OrderQty, 3);
        beforeQtyStr = UFC::AnsiString(p020MsgPtr->BeforeQty, 3);
        afterQtyStr = UFC::AnsiString(p020MsgPtr->AfterQty, 3);
        orderPriceStr = UFC::AnsiString(p020MsgPtr->OrderPrice, 6);
    }
    else
    {
        WriteRiskLog(logHead, "Order[%s] Wrong msgType[%d].", OI.GetOrderID()/*.c_str()*/, msgType);
        return;
    }
        
    accountNo.TrimRight(' ');
    stockNo.TrimRight(' ');
    int orderQty = orderQtyStr.ToInt();
    int beforeQty = beforeQtyStr.ToInt();
    int afterQty = afterQtyStr.ToInt();
    if (exchangeCode != '2') 
    {
        orderQty *= 1000;
        beforeQty *= 1000;
        afterQty *= 1000;
    }
    double orderPrice = Parse94v99PriceStr(orderPriceStr);
    int diffenentQty = beforeQty - afterQty;
    WriteLevelRiskLog(UFC::dlInformation, logHead, "Order[%s] func[%d] %c Px[%10.3lf] Qty[Order=%d before=%d, after=%d] status[%d] msgType[%d]", orderId.c_str(), functionCode, buySellCode, orderPrice, orderQty, beforeQty, afterQty, msgStatusCode, msgType);

    CRMOrderSummary *orderPtr = FindOrderFromIDIndex(orderId, UFC::rwLockForRead);
    if (orderPtr != 0)
    {
        orderPtr->LockForRead();
        UFC::AnsiString warrantNo = orderPtr->GetWarrantNo(UFC::rwNotLock);
        UFC::AnsiString brokerId = orderPtr->GetBrokerId(UFC::rwNotLock);
        int origLeavesQty = orderPtr->GetLeavesOrderQty(UFC::rwNotLock);
        int newLeavesQty = origLeavesQty - diffenentQty;
        UFC::AnsiString traderID = orderPtr->GetTraderID(UFC::rwNotLock);
        orderPtr->UnlockForRead();
        CRMInvestorAccount *accountPtr = FindInvestorAccountFromNoIndex(accountNo, UFC::rwLockForRead);
        if (accountPtr != 0)
        {
            CRMStockTradingData *stockDataPtr = accountPtr->FindStockData(stockNo, UFC::rwLockForRead);
            if (stockDataPtr != 0)
            {
                orderPtr->SetLeavesQty(newLeavesQty, UFC::rwLockForWrite);
                if (newLeavesQty <= 0)
                {
                    CRMOrderSummary *cancelOrderPtr = 0;
                    if (buySellCode == 'B')
                    {
                        if (msgType == TWSE::mtT020)
                            cancelOrderPtr = stockDataPtr->RemoveNormalBuyOrder(orderId, UFC::rwLockForWrite);
                        else
                            cancelOrderPtr = stockDataPtr->RemoveBuyOrder(orderId, UFC::rwLockForWrite);
                    }
                    else if (buySellCode == 'S')
                    {
                        if (msgType == TWSE::mtT020)
                            cancelOrderPtr = stockDataPtr->RemoveNormalSellOrder(orderId, UFC::rwLockForWrite);
                        else
                            cancelOrderPtr = stockDataPtr->RemoveSellOrder(orderId, UFC::rwLockForWrite);
                    }
            
                    if (cancelOrderPtr != 0)
                        WriteLevelRiskLog(UFC::dlInformation, logHead, "Stock: %s, Order[%s], func[%d], msgType[%d], bQty[%d], aQty[%d]", stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str(), functionCode, msgType, beforeQty, afterQty);
                    else
                        WriteRiskLog(logHead, "Remove Order[%s] from account[%s] Stock[%s] failed.", orderId.c_str(), accountNo.c_str(), stockNo.c_str());
                }
                else
                    WriteLevelRiskLog(UFC::dlInformation, logHead, "Set Order[%s] LeavesQty from %d to %d, account[%s], Stock[%s], func[%d], msgType[%d], bQty[%d], aQty[%d]", orderId.c_str(), origLeavesQty, newLeavesQty, accountNo.c_str(), stockNo.c_str(), functionCode, msgType, beforeQty, afterQty);
            }
            else
                WriteRiskLog(logHead, "Account[%s] has no Stock[%s] data, Order[%s]", accountNo.c_str(), stockNo.c_str(), orderId.c_str());
        }
        else
            WriteRiskLog(logHead, "Account[%s] of Cancel/Replace Order[%s] not found", accountNo.c_str(), orderId.c_str());
        
        if (traderID.Length() > 0)
        {
            CRMBrokerTrader* traderPtr = FindBrokerTraderFromIDIndex(traderID, UFC::rwLockForRead);
            if (traderPtr != 0)
            {
                CRMTraderStockCondition* conditionPtr = traderPtr->FindConditionFromStockNoIndex(stockNo, UFC::rwLockForRead);
                if (conditionPtr != 0)
                {
                    conditionPtr->CancelOrderData(diffenentQty, orderPrice, buySellCode, orderType, UFC::rwLockForWrite);
                    WriteLevelRiskLog(UFC::dlInformation, logHead, "%s", conditionPtr->ToString(UFC::rwLockForRead).c_str());
                }
                else
                    WriteRiskLog(logHead, "Trader[%s] Stock[%s] Condition not Found", traderID.c_str(), stockNo.c_str());                
                
                traderPtr->CancelOrderData(diffenentQty, orderPrice, buySellCode, orderType, UFC::rwLockForWrite);
                WriteLevelRiskLog(UFC::dlInformation, logHead, "%s", traderPtr->ToString(UFC::rwLockForRead).c_str());
            }
            else
                WriteRiskLog(logHead, "Trader[%s] not Found", traderID.c_str());                
        }
        else
            WriteRiskLog(logHead, "TraderID is Empty");
    }
    else
        WriteRiskLog(logHead, "Order[%s] not Found", orderId.c_str());
    FlushRiskLog();
}  //CUSunRiskManagerObject::ProcessRiskCancelReplaceMessage()
//---------------------------------------------------------------------------------------------------------------------
void CUSunRiskManagerObject::ProcessRiskRejectMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI, const UFC::AnsiString PVCID)
{
    int functionCode = OI.FuncCode;
    if ((functionCode != 1) && (functionCode != 2)) return;

    UFC::AnsiString logHead;
    logHead.Printf("ProcessRiskRejectMessage() PVC:%s", PVCID.c_str());
    UFC::AnsiString orderId = OI.GetOrderID();
    TWSE::TMessageType msgType = static_cast<TWSE::TMessageType>(OI.MsgType);
    WriteLevelRiskLog(UFC::dlInformation, logHead, "Process Reject Order[%s]", orderId.c_str());
    
    CRMOrderSummary *orderPtr = FindOrderFromIDIndex(orderId, UFC::rwLockForRead);
    if (orderPtr != 0)
    {
        orderPtr->LockForRead();
        UFC::AnsiString accountNo = orderPtr->GetAccountNo(UFC::rwNotLock);
        UFC::AnsiString stockNo = orderPtr->GetStockNo(UFC::rwNotLock);
        UFC::AnsiString warrantNo = orderPtr->GetWarrantNo(UFC::rwNotLock);
        UFC::AnsiString brokerId = orderPtr->GetBrokerId(UFC::rwNotLock);
        char twseOrderType = orderPtr->GetTWSEOrderType(UFC::rwNotLock);
        char side = orderPtr->GetSide(UFC::rwNotLock);
        //char exchangeCode = orderPtr->GetExchangeCode(UFC::rwNotLock);
        int orderOty = orderPtr->GetOrderQty(UFC::rwNotLock);
        double orderPrice = orderPtr->GetOrderPrice(UFC::rwNotLock);
        UFC::AnsiString traderID = orderPtr->GetTraderID(UFC::rwNotLock);
        orderPtr->UnlockForRead();
        CRMInvestorAccount *accountPtr = FindInvestorAccountFromNoIndex(accountNo, UFC::rwLockForRead);
        if (accountPtr != 0)
        {
            CRMStockTradingData *stockDataPtr = accountPtr->FindStockData(stockNo, UFC::rwLockForRead);
            if (stockDataPtr != 0)
            {
                CRMOrderSummary *removeOrderPtr = 0;
                if (functionCode == 1)
                {
                    if (msgType == TWSE::mtT010)
                        removeOrderPtr = stockDataPtr->RemoveNormalBuyOrder(orderId, UFC::rwLockForWrite);
                    else
                        removeOrderPtr = stockDataPtr->RemoveBuyOrder(orderId, UFC::rwLockForWrite);
                }
                else
                {
                    if (msgType == TWSE::mtT010)
                        removeOrderPtr = stockDataPtr->RemoveNormalSellOrder(orderId, UFC::rwLockForWrite);
                    else
                        removeOrderPtr = stockDataPtr->RemoveSellOrder(orderId, UFC::rwLockForWrite);
                }
                if (removeOrderPtr != 0)
                    WriteLevelRiskLog(UFC::dlInformation, logHead, "Stock: %s, Order[%s]", stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str());
                else
                    WriteRiskLog(logHead, "Remove order[%s] from Account[%s] Stock[%s] failed", orderId.c_str(), accountNo.c_str(), stockNo.c_str());
            }
            else
                WriteRiskLog(logHead, "Account[%s] has no Stock[%s] data, Order[%s]", accountNo.c_str(), stockNo.c_str(), orderId.c_str());
        }
        else
            WriteRiskLog(logHead, "Account[%s] of Rejected Order[%s] not found", accountNo.c_str(), orderId.c_str());
        
        if (traderID.Length() > 0)
        {
            CRMBrokerTrader* traderPtr = FindBrokerTraderFromIDIndex(traderID, UFC::rwLockForRead);
            if (traderPtr != 0)
            {
                CRMTraderStockCondition* conditionPtr = traderPtr->FindConditionFromStockNoIndex(stockNo, UFC::rwLockForRead);
                if (conditionPtr != 0)
                {
                    conditionPtr->CancelOrderData(orderOty, orderPrice, side, twseOrderType, UFC::rwLockForWrite);
                    WriteLevelRiskLog(UFC::dlInformation, logHead, "%s", conditionPtr->ToString(UFC::rwLockForRead).c_str());
                }
                else
                    WriteRiskLog(logHead, "Trader[%s] Stock[%s] Condition not Found", traderID.c_str(), stockNo.c_str());
                
                traderPtr->CancelOrderData(orderOty, orderPrice, side, twseOrderType, UFC::rwLockForWrite);
                WriteLevelRiskLog(UFC::dlInformation, logHead, "%s", traderPtr->ToString(UFC::rwLockForRead).c_str());
            }
            else
                WriteRiskLog(logHead, "Trader[%s] not Found", traderID.c_str());                
        }
        else
            WriteRiskLog(logHead, "TraderID is Empty");
    }
    else
        WriteRiskLog(logHead, "Rejected Order[%s] not Found", orderId.c_str());
    FlushRiskLog();
}  //CUSunRiskManagerObject::ProcessRiskRejectMessage()
//------------------------------------------------------------------------------
void CUSunRiskManagerObject::RecoverRMConfirmData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& AE, const UFC::AnsiString& TraderID, const UFC::AnsiString& OrderKey, bool IsSynchronize)
{
    UFC::AnsiString logHead = "";
    if (IsSynchronize)
        logHead = "SynchronizeRMConfirmData()";
    else
        logHead = "RecoverRMConfirmData()";
    TWSE::MessageHeader *msgPtr = TWSE::MessageHeader::CreateMessage(ExecRptDataStr.c_str(), ExecRptDataStr.Length());
    int subSystem = msgPtr->GetSubSystem();
    int functionCode = msgPtr->GetFunctionCode();
    int msgStatusCode = msgPtr->GetStatusCode();    
    TWSE::TMessageType msgType = msgPtr->MessageType();
    if (((msgType != TWSE::mtT020) && (msgType != TWSE::mtO020) && (msgType != TWSE::mtP020)) ||
        ((functionCode != 1) && (functionCode != 2) && (functionCode != 3) && (functionCode != 4)) || 
        (((functionCode == 1) || (functionCode == 2) || (functionCode == 4)) && (msgStatusCode != 0)) ||
        ((functionCode == 3) && ((msgStatusCode != 0) && (msgStatusCode != 31) && (msgStatusCode != 32))))
    {
        WriteRiskLog(logHead, "Ignore Order[%s], Func[%d], Status[%d], Type[%d]", ExecRptDataStr.c_str(), functionCode, msgStatusCode, msgType);
        delete msgPtr;
        return;
    }
    
    UFC::AnsiString orderId, accountNo, stockNo, orderQtyStr, beforeQtyStr, afterQtyStr, orderPriceStr;
    char buySellCode = ' ', exchangeCode = ' ', orderType = ' ';
    TWSE::TMessageType orderMsgType = TWSE::mtUnknown;
    if (msgType == TWSE::mtT020)
    {
        orderMsgType = TWSE::mtT010;
        TWSE::MessageT020 *t020MsgPtr = static_cast<TWSE::MessageT020*>(msgPtr);
        orderId = UFC::AnsiString(t020MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(t020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(t020MsgPtr->StockID, 6);
        buySellCode = t020MsgPtr->BuySellCode;
        exchangeCode = t020MsgPtr->ExchangeCode;
        orderType = t020MsgPtr->OrderType;
        orderQtyStr = UFC::AnsiString(t020MsgPtr->OrderQty, 3);
        beforeQtyStr = UFC::AnsiString(t020MsgPtr->BeforeQty, 3);
        afterQtyStr = UFC::AnsiString(t020MsgPtr->AfterQty, 3);
        orderPriceStr = UFC::AnsiString(t020MsgPtr->OrderPrice, 6);
    }
    else if (msgType == TWSE::mtO020)
    {
        orderMsgType = TWSE::mtO010;
        TWSE::MessageO020 *o020MsgPtr = static_cast<TWSE::MessageO020*>(msgPtr);
        orderId = UFC::AnsiString(o020MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(o020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(o020MsgPtr->StockID, 6);
        buySellCode = o020MsgPtr->BuySellCode;
        exchangeCode = '2';
        orderType = '0';
        orderQtyStr = UFC::AnsiString(o020MsgPtr->OrderQty, 8);
        beforeQtyStr = UFC::AnsiString(o020MsgPtr->BeforeQty, 8);
        afterQtyStr = UFC::AnsiString(o020MsgPtr->AfterQty, 8);
        orderPriceStr = UFC::AnsiString(o020MsgPtr->OrderPrice, 6);
    }
    else if (msgType == TWSE::mtP020)
    {
        orderMsgType = TWSE::mtP010;
        TWSE::MessageP020 *p020MsgPtr = static_cast<TWSE::MessageP020*>(msgPtr);
        orderId = UFC::AnsiString(p020MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(p020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(p020MsgPtr->StockID, 6);
        buySellCode = p020MsgPtr->BuySellCode;
        exchangeCode = p020MsgPtr->ExchangeCode;
        orderType = p020MsgPtr->OrderType;
        orderQtyStr = UFC::AnsiString(p020MsgPtr->OrderQty, 3);
        beforeQtyStr = UFC::AnsiString(p020MsgPtr->BeforeQty, 3);
        afterQtyStr = UFC::AnsiString(p020MsgPtr->AfterQty, 3);
        orderPriceStr = UFC::AnsiString(p020MsgPtr->OrderPrice, 6);
    }
    delete msgPtr;
    accountNo.TrimRight(' ');
    stockNo.TrimRight(' ');
    int orderQty = orderQtyStr.ToInt();
    int beforeQty = beforeQtyStr.ToInt();
    int afterQty = afterQtyStr.ToInt();
    if (exchangeCode != '2') 
    {
        orderQty *= 1000;
        beforeQty *= 1000;
        afterQty *= 1000;
    }
    double orderPrice = Parse94v99PriceStr(orderPriceStr);
    int diffenentQty = beforeQty - afterQty;
    WriteLevelRiskLog(UFC::dlInformation, logHead, "Order[%s] Stock[%s][%s][%s]%c %c Qty[%d] Px[%11.3lf] bQty[%d] aQty[%d] execCode[%c] Acc[%s] func[%d] status[%d] SubSys[%d] msgType[%d] OrderKey[%s]", 
                      orderId.c_str(), stockNo.c_str(), AE.c_str(), TraderID.c_str(), orderType, buySellCode, orderQty, orderPrice, beforeQty, afterQty, exchangeCode, accountNo.c_str(), functionCode, msgStatusCode, subSystem, msgType, OrderKey.c_str());
    CRMOrderSummary *targetOrderPtr = FindOrderFromIDIndex(orderId, UFC::rwLockForRead);
    UFC::AnsiString WarrantNo = "", BrokerId = "";
    if ((functionCode == 1) || (functionCode == 2))
    {
        if (targetOrderPtr == 0)
        {
            CRMOrderSummary *newOrderPtr = new CRMOrderSummary(orderId, buySellCode, stockNo, WarrantNo, BrokerId, orderType, orderPrice, orderQty, exchangeCode, orderMsgType, accountNo);
            newOrderPtr->SetOrderKey(OrderKey, UFC::rwNotLock);
            newOrderPtr->SetTraderID(TraderID, UFC::rwNotLock);
            if (InsertOrderIntoIDIndex(orderId, newOrderPtr, cstNotCheck, UFC::rwLockForWrite))
                targetOrderPtr = newOrderPtr;
            else
            {
                WriteRiskLog(logHead, "Insert Order[%s] into Object Manager failed.", orderId.c_str());
                delete newOrderPtr;
            }
        }
        else
        {
            targetOrderPtr = 0;
            WriteRiskLog(logHead, "Duplicate Order[%s].", orderId.c_str());
        }
    }
    else if ((functionCode == 3) || (functionCode == 4))
    {
        if (targetOrderPtr == 0)
            WriteRiskLog(logHead, "Order[%s] Not Found.", orderId.c_str());
    }  //if ((functionCode == 1) || (functionCode == 2))
    
    if (targetOrderPtr != 0)
    {
        CRMInvestorAccount *accountPtr = FindInvestorAccountFromNoIndex(accountNo, UFC::rwLockForRead);
        if (accountPtr != 0)
        {
            bool isNewStockData = false;
            CRMStockTradingData *stockDataPtr = accountPtr->FindOrCreateStockData(stockNo, isNewStockData, UFC::rwLockForWrite);
            if (stockDataPtr != 0)
            {
                if (isNewStockData)
                    WriteLevelRiskLog(UFC::dlPerformance, logHead, "Create Account[%s] Stock[%s].", accountNo.c_str(), stockNo.c_str());
                
                bool isInsertOrderSuccess = false;
                if ((functionCode == 1) || (functionCode == 2))
                {
                    if (functionCode == 1)
                    {
                        if (msgType == TWSE::mtT020)
                            isInsertOrderSuccess = stockDataPtr->InsertNormalBuyOrder(orderId, targetOrderPtr, cstCheck, UFC::rwLockForWrite);
                        else
                            isInsertOrderSuccess = stockDataPtr->InsertBuyOrder(orderId, targetOrderPtr, cstCheck, UFC::rwLockForWrite);
                    }
                    else if (functionCode == 2)
                    {
                        if (msgType == TWSE::mtT020)
                            isInsertOrderSuccess = stockDataPtr->InsertNormalSellOrder(orderId, targetOrderPtr, cstCheck, UFC::rwLockForWrite);
                        else
                            isInsertOrderSuccess = stockDataPtr->InsertSellOrder(orderId, targetOrderPtr, cstCheck, UFC::rwLockForWrite);
                    }
                    
                    if (isInsertOrderSuccess)
                        WriteLevelRiskLog(UFC::dlInformation, logHead, "StockData: %s, Order[%s], msgType[%d]", stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str(), msgType);
                    else
                        WriteRiskLog(logHead, "Insert Order[%s] into Account[%s] Stock[%s] failed.", orderId.c_str(), accountNo.c_str(), stockNo.c_str());
                }
                else if ((functionCode == 3) || (functionCode == 4))
                {
                    int origLeavesQty = targetOrderPtr->GetLeavesOrderQty(UFC::rwLockForRead);
                    int newLeavesQty = origLeavesQty - diffenentQty;
                    targetOrderPtr->SetLeavesQty(newLeavesQty, UFC::rwLockForWrite);
                    if (newLeavesQty <= 0)
                    {
                        CRMOrderSummary *cancelOrderPtr = 0;
                        if (buySellCode == 'B')
                        {
                            if (msgType == TWSE::mtT020)
                                cancelOrderPtr = stockDataPtr->RemoveNormalBuyOrder(orderId, UFC::rwLockForWrite);
                            else
                                cancelOrderPtr = stockDataPtr->RemoveBuyOrder(orderId, UFC::rwLockForWrite);
                        }
                        else if (buySellCode == 'S')
                        {
                            if (msgType == TWSE::mtT020)
                                cancelOrderPtr = stockDataPtr->RemoveNormalSellOrder(orderId, UFC::rwLockForWrite);
                            else
                                cancelOrderPtr = stockDataPtr->RemoveSellOrder(orderId, UFC::rwLockForWrite);
                        }
                                    
                        if (cancelOrderPtr != 0)
                            WriteLevelRiskLog(UFC::dlInformation, logHead, "StockData: %s, Order[%s], msgType[%d], bQty[%d], aQty[%d]", stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str(), msgType, beforeQty, afterQty);
                        else
                            WriteRiskLog(logHead, "Remove Order[%s] from Account[%s] Stock[%s] failed.", orderId.c_str(), accountNo.c_str(), stockNo.c_str());
                    }  //if (afterQty == 0)
                    else
                        WriteLevelRiskLog(UFC::dlPerformance, logHead, "Set Order[%s] LeavesQty from %d to %d, msgType[%d], bQty[%d], aQty[%d]", orderId.c_str(), origLeavesQty, newLeavesQty, msgType, beforeQty, afterQty);
                }  //if ((curFunctionCode == 3) || (curFunctionCode == 4))
            }
            else
                WriteRiskLog(logHead, "Wrong Account[%s] Stock[%s], Order[%s]", accountNo.c_str(), stockNo.c_str(), orderId.c_str());
        }
        else
            WriteRiskLog(logHead, "Account[%s] not Found, Order[%s]", accountNo.c_str(), orderId.c_str());

        CRMBrokerTrader *traderPtr = FindBrokerTraderFromIDIndex(TraderID, UFC::rwLockForRead);
        if (traderPtr != 0)
        {
            CRMTraderStockCondition *conditionPtr = traderPtr->FindConditionFromStockNoIndex(stockNo, UFC::rwLockForRead);
            if (conditionPtr != 0)
            {
                conditionPtr->AddNewOrderData(orderQty, orderPrice, buySellCode, orderType, UFC::rwLockForWrite);
                WriteLevelRiskLog(UFC::dlInformation, logHead, "%s", conditionPtr->ToString(UFC::rwLockForRead).c_str());        
            }
            else
                WriteRiskLog(logHead, "Trader[%s] Stock[%s] Condition not Found, Order[%s]", TraderID.c_str(), stockNo.c_str(), orderId.c_str());            
            
            traderPtr->AddNewOrderData(orderQty, orderPrice, buySellCode, orderType, UFC::rwLockForWrite);
            WriteLevelRiskLog(UFC::dlInformation, logHead, "%s", traderPtr->ToString(UFC::rwLockForRead).c_str());
        }
        else
            WriteRiskLog(logHead, "Trader[%s] not Found, Order[%s]", TraderID.c_str(), orderId.c_str());        
    }  //if (targetOrderPtr != 0)
    FlushRiskLog();
}  //CUSunRiskManagerObject::RecoverRMConfirmData()
//------------------------------------------------------------------------------
void CUSunRiskManagerObject::RecoverRMFillData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& AE, const UFC::AnsiString& TraderID, const UFC::AnsiString& OrderKey, bool IsSynchronize)
{
    UFC::AnsiString logHead = "";
    if (IsSynchronize)
        logHead = "SynchronizeRMFillData()";
    else
        logHead = "RecoverRMFillData()";
    UFC::AnsiString logMsg = "";
    int r3Offset = 0;
    TWSE::R3Body *r3BodyPtr = new TWSE::R3Body(ExecRptDataStr.c_str(), r3Offset);
    UFC::AnsiString orderId(r3BodyPtr->FOrderNo, 5);
                
    UFC::AnsiString accountNo(r3BodyPtr->FInvestorACNo, 7);
    accountNo.TrimRight(' ');
                
    UFC::AnsiString stockNo(r3BodyPtr->FStockID, 6);
    stockNo.TrimRight(' ');

    char buySellCode = r3BodyPtr->FBuySellCode;
    char exchangeCode = r3BodyPtr->FEXCD;
            
    UFC::AnsiString matchQtyStr(r3BodyPtr->FQuntity, 8);
    int matchQty = matchQtyStr.ToInt();
    if (exchangeCode != '2') matchQty *= 1000;
            
    UFC::AnsiString matchPriceStr(r3BodyPtr->FPrice, 6);
    double matchPrice = Parse94v99PriceStr(matchPriceStr);

    int matchSeqNo = r3BodyPtr->GetSeq();
    int matchTime = r3BodyPtr->ProcessingTime();
    char side = r3BodyPtr->FBuySellCode;
    ///< Use line broker ID + report sequence + match time as unique key.
    UFC::AnsiString execKey;
    execKey.Printf("%08d_%08d%c", matchTime, matchSeqNo, side);
    
    delete r3BodyPtr;
    
    if (IsSynchronize)
    {
        if (IsFillRptExecExist(execKey, UFC::rwLockForRead))
        {
            WriteRiskLog(logHead, "[%s] has Processed. skip it.", execKey.c_str());
            return;
        }
        else
            AddFillRptExec(execKey, UFC::rwLockForWrite);
    }  //if (IsSynchronize)
    
    CRMOrderSummary *orderPtr = FindOrderFromIDIndex(orderId, UFC::rwLockForRead);
    if (orderPtr != 0)
    {
        char twseOrderType = orderPtr->GetTWSEOrderType(UFC::rwLockForRead);
        TWSE::TMessageType msgType = orderPtr->GetMsgType(UFC::rwLockForRead);
        WriteLevelRiskLog(UFC::dlInformation, logHead, "Order[%s] Stock[%s][%s][%s]%c %c MatchQty[%d] MatchPx[%11.3lf] ExecCode[%c] msgType[%d] Account[%s] OrderKey[%s]", 
                          orderId.c_str(), stockNo.c_str(), AE.c_str(), TraderID.c_str(), twseOrderType, buySellCode, matchQty, matchPrice, exchangeCode, msgType, accountNo.c_str(), OrderKey.c_str());
                
        int origLeavesOrderQty = orderPtr->GetLeavesOrderQty(UFC::rwLockForRead);
        if (origLeavesOrderQty > 0)
        {
            if (origLeavesOrderQty >= matchQty)
            {
                orderPtr->IncreaseLeavesQty(-matchQty, UFC::rwLockForWrite);
                CRMInvestorAccount *accountPtr = FindInvestorAccountFromNoIndex(accountNo, UFC::rwLockForRead);
                if (accountPtr != 0)
                {
                    CRMStockTradingData *stockDataPtr = accountPtr->FindStockData(stockNo, UFC::rwLockForRead);
                    if (stockDataPtr != 0)
                    {
                        int newLeavesOrderQty = orderPtr->GetLeavesOrderQty(UFC::rwLockForRead);
                        if (newLeavesOrderQty <= 0)
                        {
                            CRMOrderSummary *filledOrderPtr = 0;
                            if (buySellCode == 'B')
                            {
                                if (msgType == TWSE::mtT010)
                                    filledOrderPtr = stockDataPtr->RemoveNormalBuyOrder(orderId, UFC::rwLockForWrite);
                                else
                                    filledOrderPtr = stockDataPtr->RemoveBuyOrder(orderId, UFC::rwLockForWrite);                                    
                            }
                            else
                            {
                                if (msgType == TWSE::mtT010)
                                    filledOrderPtr = stockDataPtr->RemoveNormalSellOrder(orderId, UFC::rwLockForWrite);
                                else
                                    filledOrderPtr = stockDataPtr->RemoveSellOrder(orderId, UFC::rwLockForWrite);
                            }
                            
                            if (filledOrderPtr != 0)
                                WriteLevelRiskLog(UFC::dlInformation, logHead, "StockData: %s, Order[%s]", stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str());
                            else
                                WriteRiskLog(logHead, "Remove Filled Order[%s] from account[%s] Stock[%s] failed.", orderId.c_str(), accountNo.c_str(), stockNo.c_str());
                        }  //if (newLeavesOrderQty <= 0)
                    }
                    else
                        WriteRiskLog(logHead, "Account[%s] Stock[%s] data not Found, Order[%s]", accountNo.c_str(), stockNo.c_str(), orderId.c_str());
                }                
                else
                    WriteRiskLog(logHead, "Account[%s] not Found, Order[%s]", accountNo.c_str(), orderId.c_str());
                
                CRMBrokerTrader *traderPtr = FindBrokerTraderFromIDIndex(TraderID, UFC::rwLockForRead);
                if (traderPtr != 0)
                {
                    CRMTraderStockCondition *conditionPtr = traderPtr->FindConditionFromStockNoIndex(stockNo, UFC::rwLockForRead);
                    if (conditionPtr != 0)
                    {
                       conditionPtr->AddMatchOrderData(matchQty, matchPrice, orderPtr->GetOrderPrice(UFC::rwLockForRead), side, twseOrderType, UFC::rwLockForWrite);
                        WriteLevelRiskLog(UFC::dlInformation, logHead, "%s", conditionPtr->ToString(UFC::rwLockForRead).c_str());        
                    }
                    else
                        WriteRiskLog(logHead, "Trader[%s] Stock[%s] Condition not Found, Order[%s]", TraderID.c_str(), stockNo.c_str(), orderId.c_str());            
            
                    traderPtr->AddMatchOrderData(matchQty, matchPrice, orderPtr->GetOrderPrice(UFC::rwLockForRead), side, twseOrderType, UFC::rwLockForWrite);
                    WriteLevelRiskLog(UFC::dlInformation, logHead, "%s", traderPtr->ToString(UFC::rwLockForRead).c_str());
                }
                else
                    WriteRiskLog(logHead, "Trader[%s] not Found, Order[%s]", TraderID.c_str(), orderId.c_str());        
            }
            else
                WriteRiskLog(logHead, "Order[%s] LeavesQty[%d] < MatchQty[%d].", orderId.c_str(), origLeavesOrderQty, matchQty);
        }    
        else
            WriteRiskLog(logHead, "Order[%s] is Filled.", orderId.c_str());
    }
    else
        WriteRiskLog(logHead, "Order[%s] not Found.", orderId.c_str());
}  //CUSunRiskManagerObject::RecoverRMFillData()
//------------------------------------------------------------------------------
void CUSunRiskManagerObject::ParseExecRtpLogStr(const UFC::AnsiString& ExecRptLogStr, bool IsSynchronize)
{
    UFC::AnsiString logHead = "ParseExecRtpLogStr()";
    ExecutionParser *execParserPtr = 0;
    try 
    {
        execParserPtr = new ExecutionParser(ExecRptLogStr);
    }
    catch (UFC::Exception& e)
    {
        WriteRiskLog(logHead, "Parse Execution[%s] Exception:%s", ExecRptLogStr.c_str(), e.what());
        FlushRiskLog();
        return;
    }
    
    UFC::AnsiString userKey = execParserPtr->GetKey();
    UFC::AnsiString ae, traderID;
    ParseOrderKey(userKey, ae, traderID);
    UFC::AnsiString execRptData = execParserPtr->GetData();
    BOOL isConfirm = execParserPtr->IsConfirm();
    delete execParserPtr;
    
    if (isConfirm == TRUE)
        RecoverRMConfirmData(execRptData, ae, traderID, userKey, IsSynchronize);
    else
        RecoverRMFillData(execRptData, ae, traderID, userKey, IsSynchronize);
    FlushRiskLog();
}  //CUSunRiskManagerObject::ParseExecRtpLogStr()
//------------------------------------------------------------------------------
void CUSunRiskManagerObject::RecoverRMData(BOOL IsTSE)
{
    UFC::AnsiString logHead = "RecoverRMData()";
    UFC::AnsiString executionLogFileName = "";
    if (IsTSE == TRUE)
        executionLogFileName = "SpeedyTSE.Execution." + UFC::GetDateString() + ".DATA";
    else
        executionLogFileName = "SpeedyOTC.Execution." + UFC::GetDateString() + ".DATA";
    FILE *fileHandlePtr = fopen(executionLogFileName.c_str(), "r");
    if (fileHandlePtr == NULL)
    {
        UFC::BufferedLog::Printf(" %s Can not open recover file %s", logHead.c_str(), executionLogFileName.c_str());
        return;
    }  //if (fileHandlePtr == NULL)
    
    char readBuffer[512];
    while (fgets(readBuffer, 512, fileHandlePtr) != NULL)
    {
        UFC::AnsiString curReadStr(readBuffer);
        ParseExecRtpLogStr(curReadStr, false);
    }  //while (fgets(readBuffer, 512, fileHandlePtr) != NULL)
    fclose(fileHandlePtr);
}  //CUSunRiskManagerObject::RecoverRMData()
}  //namespace rm

