/* 
 * File:   TWSEEntrustRMObjects.h
 * Author: Zhen Fan
 *
 * Created on 2021/05/03 11:01
 */

#include "TWSEEntrustRMObjects.h"

namespace twserm
{
//--------------------------- CRMInvestorSubAccount ----------------------------
UFC::AnsiString CRMInvestorSubAccount::BuildSubAccountKey(const UFC::AnsiString& AccountNo, const UFC::AnsiString& SubAccountNo)
{
    return AccountNo + SubAccountNo;
}  //CRMInvestorSubAccount::BuildSubAccountKey()
//------------------------------------------------------------------------------
CRMInvestorSubAccount::CRMInvestorSubAccount(const UFC::AnsiString& AccountNo, const UFC::AnsiString& SubAccountNo)
:UFC::CMainKeyObject(UFC::cstCreateLock)
,FAccountNo(AccountNo)
,FSubAccountNo(SubAccountNo)
,FStockAmountLimit(-1.0)
,FPositionAmount(0.0)
,FBuyOrderAmount(0.0)
,FSellOrderAmount(0.0)
,FBuyMatchAmount(0.0)
,FSellMatchAmount(0.0)
{
    FMainIndexKey = BuildSubAccountKey(FAccountNo, FSubAccountNo);
}  //CRMInvestorSubAccount::CRMInvestorSubAccount()
//------------------------------------------------------------------------------
CRMInvestorSubAccount::CRMInvestorSubAccount(const UFC::AnsiString& AccountNo, const UFC::AnsiString& SubAccountNo, double StockAmountLimit)
:UFC::CMainKeyObject(UFC::cstCreateLock)
,FAccountNo(AccountNo)
,FSubAccountNo(SubAccountNo)
,FStockAmountLimit(StockAmountLimit)
,FPositionAmount(0.0)
,FBuyOrderAmount(0.0)
,FSellOrderAmount(0.0)
,FBuyMatchAmount(0.0)
,FSellMatchAmount(0.0)
{
    FMainIndexKey = BuildSubAccountKey(FAccountNo, FSubAccountNo);
}  //CRMInvestorSubAccount::CRMInvestorSubAccount()
//------------------------------------------------------------------------------
CRMInvestorSubAccount::~CRMInvestorSubAccount()
{    
}  //CRMInvestorSubAccount::~CRMInvestorSubAccount()
//------------------------------------------------------------------------------
bool CRMInvestorSubAccount::CheckOrderAmount(const UFC::AnsiString& OrderID, char BuySellCode, double OrderAmount, UFC::AnsiString& CheckMessage, UFC::RWLockTypeEnum LockType)
{
    bool isSuccess = true;
    Lock(LockType);
    if (FStockAmountLimit > 0.0)
    {
        double usedAmount = 0.0;
        UFC::AnsiString usedAmountStr = "";
        if (BuySellCode == 'B')
        {
            usedAmount = FPositionAmount + FBuyOrderAmount + FBuyMatchAmount - FSellMatchAmount;
            usedAmountStr.Printf("UsedAmt[%15.4lf](PosAmt[%15.4lf] + BuyOrdAmt[%15.4lf] + BuyMatchAmt[%15.4lf] - SellMatchAmt[%15.4lf])", usedAmount, FPositionAmount, FBuyOrderAmount, FBuyMatchAmount, FSellMatchAmount);
            if ((usedAmount + OrderAmount) > FStockAmountLimit)
            {
                CheckMessage.Printf("Reject[%s]: %s + OrderAmt[%15.4lf] > AmtLimit[%15.4lf]", OrderID.c_str(), usedAmountStr.c_str(), OrderAmount, FStockAmountLimit);
                isSuccess = false;
            }
        }
    }  //if (FStockAmountLimit > 0.0)
    Unlock(LockType);
    return isSuccess;
}  //CRMInvestorSubAccount::CheckOrderAmount()
//------------------------------------------------------------------------------
UFC::AnsiString CRMInvestorSubAccount::ToString(UFC::RWLockTypeEnum LockType)
{
    UFC::PStringBuffer objBuffer;
    Lock(LockType);
    objBuffer.AppendPrintf("RMAccount[%s][%s] StkAmtLimit[%12.1lf]", FAccountNo.c_str(), FSubAccountNo.c_str(), FStockAmountLimit);
    objBuffer.AppendPrintf(" Amount{Pos[%15.4lf] BuyOrd[%15.4lf] SellOrd[%15.4lf] BuyMatch[%15.4lf] SellMatch[%15.4lf]}", FPositionAmount, FBuyOrderAmount, FSellOrderAmount, FBuyMatchAmount, FSellMatchAmount);
    Unlock(LockType);
    return objBuffer.ToString();
}  //CRMInvestorSubAccount::ToString()
    
//-------------------------- CRMEntrustStockCondition --------------------------
UFC::AnsiString CRMEntrustStockCondition::BuildIndexKey(const UFC::AnsiString& SubaccountNo, char TWSEOrderType)
{
    UFC::AnsiString indexKey;
    indexKey.Printf("%s%c", SubaccountNo.c_str(), TWSEOrderType);
    return indexKey;
}  //CRMEntrustStockCondition::BuildIndexKey()
//------------------------------------------------------------------------------
CRMEntrustStockCondition::CRMEntrustStockCondition(const UFC::AnsiString& SubaccountNo, char TWSEOrderType,
                                                   int AvailableSellQty, int AvailableBuyQty, bool IsSellShort)
:UFC::CMainKeyObject(UFC::cstCreateLock)
,FSubaccountNo(SubaccountNo)
,FTWSEOrderType(TWSEOrderType)
,FAvailableSellQty(AvailableSellQty)
,FAvailableBuyQty(AvailableBuyQty)
,FIsSellShort(IsSellShort)
,FAccountNo("")
,FStockNo("")
,FRegularMatchBuyQty(0)
,FRegularMatchSellQty(0)
,FIntraDayOddMatchBuyQty(0)
,FIntraDayOddMatchSellQty(0)
,FMatchBuyQty(0)
,FMatchSellQty(0)
,FRegularOrderBuyQty(0)
,FRegularOrderSellQty(0)
,FIntraDayOddOrderBuyQty(0)
,FIntraDayOddOrderSellQty(0)
,FOrderBuyQty(0)
,FOrderSellQty(0)
,FEntrustOrderType(TWSEOrderType)
{
    FMainIndexKey = BuildIndexKey(FSubaccountNo, FTWSEOrderType);
}  //CRMEntrustStockCondition::CRMEntrustStockCondition()
//------------------------------------------------------------------------------
int CRMEntrustStockCondition::IncreaseRegularMatchBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FRegularMatchBuyQty + IncreaseQty;
    FRegularMatchBuyQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMEntrustStockCondition::IncreaseRegularMatchBuyQty()
//------------------------------------------------------------------------------
int CRMEntrustStockCondition::IncreaseRegularMatchSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FRegularMatchSellQty + IncreaseQty;
    FRegularMatchSellQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMEntrustStockCondition::IncreaseRegularMatchSellQty()
//------------------------------------------------------------------------------
int CRMEntrustStockCondition::IncreaseIntraDayOddMatchBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FIntraDayOddMatchBuyQty + IncreaseQty;
    FIntraDayOddMatchBuyQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMEntrustStockCondition::IncreaseIntraDayOddMatchBuyQty()
//------------------------------------------------------------------------------
int CRMEntrustStockCondition::IncreaseIntraDayOddMatchSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FIntraDayOddMatchSellQty + IncreaseQty;
    FIntraDayOddMatchSellQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMEntrustStockCondition::IncreaseIntraDayOddMatchSellQty()
//------------------------------------------------------------------------------
int CRMEntrustStockCondition::IncreaseMatchBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FMatchBuyQty + IncreaseQty;
    FMatchBuyQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMEntrustStockCondition::IncreaseMatchBuyQty()
//------------------------------------------------------------------------------
int CRMEntrustStockCondition::IncreaseMatchSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FMatchSellQty + IncreaseQty;
    FMatchSellQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMEntrustStockCondition::IncreaseMatchSellQty()
//------------------------------------------------------------------------------
int CRMEntrustStockCondition::IncreaseRegularOrderBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FRegularOrderBuyQty + IncreaseQty;
    FRegularOrderBuyQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMEntrustStockCondition::IncreaseRegularOrderBuyQty()
//------------------------------------------------------------------------------
int CRMEntrustStockCondition::IncreaseRegularOrderSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FRegularOrderSellQty + IncreaseQty;
    FRegularOrderSellQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMEntrustStockCondition::IncreaseRegularOrderSellQty()
//------------------------------------------------------------------------------
int CRMEntrustStockCondition::IncreaseIntraDayOddOrderBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FIntraDayOddOrderBuyQty + IncreaseQty;
    FIntraDayOddOrderBuyQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMEntrustStockCondition::IncreaseIntraDayOddOrderBuyQty()
//------------------------------------------------------------------------------
int CRMEntrustStockCondition::IncreaseIntraDayOddOrderSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FIntraDayOddOrderSellQty + IncreaseQty;
    FIntraDayOddOrderSellQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMEntrustStockCondition::IncreaseIntraDayOddOrderSellQty()
//------------------------------------------------------------------------------
int CRMEntrustStockCondition::IncreaseOrderBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FOrderBuyQty + IncreaseQty;
    FOrderBuyQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMEntrustStockCondition::IncreaseOrderBuyQty()
//------------------------------------------------------------------------------
int CRMEntrustStockCondition::IncreaseOrderSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FOrderSellQty + IncreaseQty;
    FOrderSellQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMEntrustStockCondition::IncreaseOrderSellQty()
//------------------------------------------------------------------------------
UFC::AnsiString CRMEntrustStockCondition::ToString(UFC::RWLockTypeEnum LockType)
{
    UFC::AnsiString objStr = "";
    UFC::AnsiString sellShortStr = " ";
    Lock(LockType);
    if (FIsSellShort) sellShortStr = "Sell Short";
    objStr.Printf("Condition:Acc[%s] Stk[%s][%s]%c available[S %d, B %d] match[RS %d, RB %d; OS %d, OB %d; S %d, B %d] order[RS %d, RB %d; OS %d, OB %d; S %d, B %d] %s", 
                  FAccountNo.c_str(), FStockNo.c_str(), FSubaccountNo.c_str(), FTWSEOrderType,
                  FAvailableSellQty, FAvailableBuyQty,
                  FRegularMatchSellQty, FRegularMatchBuyQty, FIntraDayOddMatchSellQty, FIntraDayOddMatchBuyQty, FMatchSellQty, FMatchBuyQty, 
                  FRegularOrderSellQty, FRegularOrderBuyQty, FIntraDayOddOrderSellQty, FIntraDayOddOrderBuyQty, FOrderSellQty, FOrderBuyQty, 
                  sellShortStr.c_str());
    Unlock(LockType);
    return objStr;
}  //CRMEntrustStockCondition::ToString()

//---------------------------- CRMEntrustStockTradingData -----------------------------
//------------------------------------------------------------------------------
CRMEntrustStockTradingData::CRMEntrustStockTradingData(const UFC::AnsiString& StockNo)
:UFC::CMainKeyObject(UFC::cstCreateLock, StockNo)
,FStockNo(StockNo)   
,FMaxRegularBuyOrderPtr(0)
,FMaxRegularBuyOrderPrice(0.0)
,FMinRegularSellOrderPtr(0)
,FMinRegularSellOrderPrice(100000.0)
,FMaxIntraDayOddBuyOrderPtr(0)
,FMaxIntraDayOddBuyOrderPrice(0.0)
,FMinIntraDayOddSellOrderPtr(0)
,FMinIntraDayOddSellOrderPrice(100000.0)
,FMaxBuyOrderPtr(0)
,FMaxBuyOrderPrice(0.0)
,FMinSellOrderPtr(0)
,FMinSellOrderPrice(0.0)
,FBuyAmount(0.0)
,FSellAmount(0.0)
,FAccountNo("")
{
}  //CRMEntrustStockTradingData::CRMEntrustStockTradingData(
//------------------------------------------------------------------------------
CRMEntrustStockTradingData::~CRMEntrustStockTradingData()
{
}  //CRMEntrustStockTradingData::~CRMEntrustStockTradingData()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMEntrustStockTradingData::FindMaxOrderPriceOrder(UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*>& OrderIDIndex, UFC::RWLockTypeEnum LockType)
{
    double maxOrderPrice = 0.0;
    CRMOrderSummary* maxOrderPtr = 0;
    Lock(LockType);
    CRMOrderSummary* curOrderPtr = OrderIDIndex.First();
    while(curOrderPtr != 0)
    {
        double curOrderPrice = curOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        if (curOrderPrice > maxOrderPrice)
        {
            maxOrderPtr = curOrderPtr;
            maxOrderPrice = curOrderPrice;
        }
        curOrderPtr = OrderIDIndex.Next();
    }  //while(curOrderPtr != 0)
    Unlock(LockType);
    return maxOrderPtr;
}  //CRMEntrustStockTradingData::FindMaxOrderPriceOrder()
//------------------------------------------------------------------------------
double CRMEntrustStockTradingData::FindMaxRegularBuyOrderPrice(UFC::RWLockTypeEnum LockType)
{
    double maxBuyOrderPrice = 0.0;
    CRMOrderSummary* maxOrderPtr = FindMaxOrderPriceOrder(FRegularBuyOrderIDIndex, LockType);
    if (maxOrderPtr != 0) maxBuyOrderPrice = maxOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return maxBuyOrderPrice;
}  //CRMEntrustStockTradingData::FindMaxRegularBuyOrderPrice()
//------------------------------------------------------------------------------
double CRMEntrustStockTradingData::FindMaxIntraDayOddBuyOrderPrice(UFC::RWLockTypeEnum LockType)
{
    double maxBuyOrderPrice = 0.0;
    CRMOrderSummary* maxOrderPtr = FindMaxOrderPriceOrder(FIntraDayOddBuyOrderIDIndex, LockType);
    if (maxOrderPtr != 0) maxBuyOrderPrice = maxOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return maxBuyOrderPrice;
}  //CRMEntrustStockTradingData::FindMaxIntraDayOddBuyOrderPrice()
//------------------------------------------------------------------------------
double CRMEntrustStockTradingData::FindMaxBuyOrderPrice(UFC::RWLockTypeEnum LockType)
{
    double maxBuyOrderPrice = 0.0;
    CRMOrderSummary* maxOrderPtr = FindMaxOrderPriceOrder(FBuyOrderIDIndex, LockType);
    if (maxOrderPtr != 0) maxBuyOrderPrice = maxOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return maxBuyOrderPrice;
}  //CRMEntrustStockTradingData::FindMaxBuyOrderPrice()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMEntrustStockTradingData::FindMinOrderPriceOrder(UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*>& OrderIDIndex, UFC::RWLockTypeEnum LockType)
{
    double minOrderPrice = 100000.0;
    CRMOrderSummary* minOrderPtr = 0;
    Lock(LockType);
    CRMOrderSummary* curOrderPtr = static_cast<CRMOrderSummary*>(OrderIDIndex.First());
    while(curOrderPtr != 0)
    {
        double curOrderPrice = curOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        if (curOrderPrice < minOrderPrice)
        {
            minOrderPtr = curOrderPtr;
            minOrderPrice = curOrderPrice;
        }
        curOrderPtr = static_cast<CRMOrderSummary*>(OrderIDIndex.Next());
    }  //while(curOrderPtr != 0)
    Unlock(LockType);
    return minOrderPtr;
}  //CRMEntrustStockTradingData::FindMinOrderPriceOrder()
//------------------------------------------------------------------------------
double CRMEntrustStockTradingData::FindMinRegularSellOrderPrice(UFC::RWLockTypeEnum LockType)
{
    double minSellOrderPrice = 100000.0;
    CRMOrderSummary *minOrderPtr = FindMinOrderPriceOrder(FRegularSellOrderIDIndex, LockType);
    if (minOrderPtr != 0) minSellOrderPrice = minOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return minSellOrderPrice;
}  //CRMEntrustStockTradingData::FindMinRegularSellOrderPrice()
//------------------------------------------------------------------------------
double CRMEntrustStockTradingData::FindMinIntraDayOddSellOrderPrice(UFC::RWLockTypeEnum LockType)
{
    double minSellOrderPrice = 100000.0;
    CRMOrderSummary *minOrderPtr = FindMinOrderPriceOrder(FIntraDayOddSellOrderIDIndex, LockType);
    if (minOrderPtr != 0) minSellOrderPrice = minOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return minSellOrderPrice;
}  //CRMEntrustStockTradingData::FindMinIntraDayOddSellOrderPrice()
//------------------------------------------------------------------------------
double CRMEntrustStockTradingData::FindMinSellOrderPrice(UFC::RWLockTypeEnum LockType)
{
    double minSellOrderPrice = 100000.0;
    CRMOrderSummary *minOrderPtr = FindMinOrderPriceOrder(FSellOrderIDIndex, LockType);
    if (minOrderPtr != 0) minSellOrderPrice = minOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return minSellOrderPrice;
}  //CRMEntrustStockTradingData::FindMinSellOrderPrice()
//------------------------------------------------------------------------------
bool CRMEntrustStockTradingData::InsertRegularBuyOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    bool isSuccess = UFC::InsertElementPtrIntoStrIdxHMap(FRegularBuyOrderIDIndex, OrderID, OrderPtr, CheckExist, FRWLockPtr, UFC::rwNotLock);
    if (isSuccess)
    {
        double orderPrice = OrderPtr->GetOrderPrice(UFC::rwLockForRead);
        if (orderPrice > FMaxRegularBuyOrderPrice) 
        {
            FMaxRegularBuyOrderPrice = orderPrice;
            FMaxRegularBuyOrderPtr = OrderPtr;
        }
    }
    Unlock(LockType);
    return isSuccess;
}  //CRMEntrustStockTradingData::InsertRegularBuyOrder()
//------------------------------------------------------------------------------
CRMOrderSummary* CRMEntrustStockTradingData::RemoveRegularBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    CRMOrderSummary *targetOrderPtr = UFC::RemoveElementPtrFromStrIdxHMap(FRegularBuyOrderIDIndex, OrderID, FRWLockPtr, UFC::rwNotLock);
    if (targetOrderPtr != 0) 
    {
        FMaxRegularBuyOrderPtr = FindMaxOrderPriceOrder(FRegularBuyOrderIDIndex, UFC::rwNotLock);
        if (FMaxRegularBuyOrderPtr != 0) 
            FMaxRegularBuyOrderPrice = FMaxRegularBuyOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        else
            FMaxRegularBuyOrderPrice = 0.0;
    }
    Unlock(LockType);
    return targetOrderPtr;
}  //CRMEntrustStockTradingData::RemoveRegularBuyOrder()
//------------------------------------------------------------------------------
void CRMEntrustStockTradingData::UpdateMaxRegularBuyOrderPrice(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    FMaxRegularBuyOrderPtr = FindMaxOrderPriceOrder(FRegularBuyOrderIDIndex, UFC::rwNotLock);
    if (FMaxRegularBuyOrderPtr != 0) 
        FMaxRegularBuyOrderPrice = FMaxRegularBuyOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    else
        FMaxRegularBuyOrderPrice = 0.0;
    Unlock(LockType);
}  //CRMEntrustStockTradingData::UpdateMaxRegularBuyOrderPrice()
//------------------------------------------------------------------------------
bool CRMEntrustStockTradingData::InsertRegularSellOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    bool isSuccess = UFC::InsertElementPtrIntoStrIdxHMap(FRegularSellOrderIDIndex, OrderID, OrderPtr, CheckExist, FRWLockPtr, UFC::rwNotLock);
    if (isSuccess)
    {
        double orderPrice = OrderPtr->GetOrderPrice(UFC::rwLockForRead);
        if (orderPrice < FMinRegularSellOrderPrice) 
        {
            FMinRegularSellOrderPrice = orderPrice;
            FMinRegularSellOrderPtr = OrderPtr;
        }
    }
    Unlock(LockType);
    return isSuccess;
}  //CRMEntrustStockTradingData::InsertRegularSellOrder()
//------------------------------------------------------------------------------
CRMOrderSummary* CRMEntrustStockTradingData::RemoveRegularSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    CRMOrderSummary *targetOrderPtr = UFC::RemoveElementPtrFromStrIdxHMap(FRegularSellOrderIDIndex, OrderID, FRWLockPtr, UFC::rwNotLock);
    if (targetOrderPtr != 0) 
    {
        FMinRegularSellOrderPtr = FindMinOrderPriceOrder(FRegularSellOrderIDIndex, UFC::rwNotLock);
        if (FMinRegularSellOrderPtr != 0) 
            FMinRegularSellOrderPrice = FMinRegularSellOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        else
            FMinRegularSellOrderPrice = 100000.0;
    }
    Unlock(LockType);
    return targetOrderPtr;
}  //CRMEntrustStockTradingData::RemoveRegularSellOrder()
//------------------------------------------------------------------------------
void CRMEntrustStockTradingData::UpdateMinRegularSellOrderPrice(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    FMinRegularSellOrderPtr = FindMinOrderPriceOrder(FRegularSellOrderIDIndex, UFC::rwNotLock);
    if (FMinRegularSellOrderPtr != 0) 
        FMinRegularSellOrderPrice = FMinRegularSellOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    else
        FMinRegularSellOrderPrice = 100000.0;
    Unlock(LockType);
}  //CRMEntrustStockTradingData::UpdateMinRegularSellOrderPrice()
//------------------------------------------------------------------------------
bool CRMEntrustStockTradingData::InsertIntraDayOddBuyOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    bool isSuccess = UFC::InsertElementPtrIntoStrIdxHMap(FIntraDayOddBuyOrderIDIndex, OrderID, OrderPtr, CheckExist, FRWLockPtr, UFC::rwNotLock);
    if (isSuccess)
    {
        double orderPrice = OrderPtr->GetOrderPrice(UFC::rwLockForRead);
        if (orderPrice > FMaxIntraDayOddBuyOrderPrice) 
        {
            FMaxIntraDayOddBuyOrderPrice = orderPrice;
            FMaxIntraDayOddBuyOrderPtr = OrderPtr;
        }
    }
    Unlock(LockType);
    return isSuccess;
}  //CRMEntrustStockTradingData::InsertIntraDayOddBuyOrder()
//------------------------------------------------------------------------------
CRMOrderSummary* CRMEntrustStockTradingData::RemoveIntraDayOddBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    CRMOrderSummary *targetOrderPtr = UFC::RemoveElementPtrFromStrIdxHMap(FIntraDayOddBuyOrderIDIndex, OrderID, FRWLockPtr, UFC::rwNotLock);
    if (targetOrderPtr != 0) 
    {
        FMaxIntraDayOddBuyOrderPtr = FindMaxOrderPriceOrder(FIntraDayOddBuyOrderIDIndex, UFC::rwNotLock);
        if (FMaxIntraDayOddBuyOrderPtr != 0) 
            FMaxIntraDayOddBuyOrderPrice = FMaxIntraDayOddBuyOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        else
            FMaxIntraDayOddBuyOrderPrice = 0.0;
    }
    Unlock(LockType);
    return targetOrderPtr;
}  //CRMEntrustStockTradingData::RemoveIntraDayOddBuyOrder()
//------------------------------------------------------------------------------
void CRMEntrustStockTradingData::UpdateMaxIntraDayOddBuyOrderPrice(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    FMaxIntraDayOddBuyOrderPtr = FindMaxOrderPriceOrder(FIntraDayOddBuyOrderIDIndex, UFC::rwNotLock);
    if (FMaxIntraDayOddBuyOrderPtr != 0) 
        FMaxIntraDayOddBuyOrderPrice = FMaxIntraDayOddBuyOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    else
        FMaxIntraDayOddBuyOrderPrice = 0.0;
    Unlock(LockType);
}  //CRMEntrustStockTradingData::UpdateMaxIntraDayOddBuyOrderPrice()
//------------------------------------------------------------------------------
bool CRMEntrustStockTradingData::InsertIntraDayOddSellOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    bool isSuccess = UFC::InsertElementPtrIntoStrIdxHMap(FIntraDayOddSellOrderIDIndex, OrderID, OrderPtr, CheckExist, FRWLockPtr, UFC::rwNotLock);
    if (isSuccess)
    {
        double orderPrice = OrderPtr->GetOrderPrice(UFC::rwLockForRead);
        if (orderPrice < FMinIntraDayOddSellOrderPrice) 
        {
            FMinIntraDayOddSellOrderPrice = orderPrice;
            FMinIntraDayOddSellOrderPtr = OrderPtr;
        }
    }
    Unlock(LockType);
    return isSuccess;
}  //CRMEntrustStockTradingData::InsertIntraDayOddSellOrder()
//------------------------------------------------------------------------------
CRMOrderSummary* CRMEntrustStockTradingData::RemoveIntraDayOddSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    CRMOrderSummary *targetOrderPtr = UFC::RemoveElementPtrFromStrIdxHMap(FIntraDayOddSellOrderIDIndex, OrderID, FRWLockPtr, UFC::rwNotLock);
    if (targetOrderPtr != 0) 
    {
        FMinIntraDayOddSellOrderPtr = FindMinOrderPriceOrder(FIntraDayOddSellOrderIDIndex, UFC::rwNotLock);
        if (FMinIntraDayOddSellOrderPtr != 0) 
            FMinIntraDayOddSellOrderPrice = FMinIntraDayOddSellOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        else
            FMinIntraDayOddSellOrderPrice = 100000.0;
    }
    Unlock(LockType);
    return targetOrderPtr;
}  //CRMEntrustStockTradingData::RemoveIntraDayOddSellOrder()
//------------------------------------------------------------------------------
void CRMEntrustStockTradingData::UpdateMinIntraDayOddSellOrderPrice(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    FMinIntraDayOddSellOrderPtr = FindMinOrderPriceOrder(FIntraDayOddSellOrderIDIndex, UFC::rwNotLock);
    if (FMinIntraDayOddSellOrderPtr != 0) 
        FMinIntraDayOddSellOrderPrice = FMinIntraDayOddSellOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    else
        FMinIntraDayOddSellOrderPrice = 100000.0;
    Unlock(LockType);
}  //CRMEntrustStockTradingData::UpdateMinIntraDayOddSellOrderPrice()
//------------------------------------------------------------------------------
bool CRMEntrustStockTradingData::InsertBuyOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    bool isSuccess = UFC::InsertElementPtrIntoStrIdxHMap(FBuyOrderIDIndex, OrderID, OrderPtr, CheckExist, FRWLockPtr, UFC::rwNotLock);
    if (isSuccess)
    {
        double orderPrice = OrderPtr->GetOrderPrice(UFC::rwLockForRead);
        if (orderPrice > FMaxBuyOrderPrice) 
        {
            FMaxBuyOrderPrice = orderPrice;
            FMaxBuyOrderPtr = OrderPtr;
        }
    }
    Unlock(LockType);
    return isSuccess;
}  //CRMEntrustStockTradingData::InsertBuyOrder()
//------------------------------------------------------------------------------
CRMOrderSummary* CRMEntrustStockTradingData::RemoveBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    CRMOrderSummary *targetOrderPtr = UFC::RemoveElementPtrFromStrIdxHMap(FBuyOrderIDIndex, OrderID, FRWLockPtr, UFC::rwNotLock);
    if (targetOrderPtr != 0) 
    {
        FMaxBuyOrderPtr = FindMaxOrderPriceOrder(FBuyOrderIDIndex, UFC::rwNotLock);
        if (FMaxBuyOrderPtr != 0) 
            FMaxBuyOrderPrice = FMaxBuyOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        else
            FMaxBuyOrderPrice = 0.0;
    }
    Unlock(LockType);
    return targetOrderPtr;
}  //CRMEntrustStockTradingData::RemoveBuyOrder()
//------------------------------------------------------------------------------
void CRMEntrustStockTradingData::UpdateMaxBuyOrderPrice(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    FMaxBuyOrderPtr = FindMaxOrderPriceOrder(FBuyOrderIDIndex, UFC::rwNotLock);
    if (FMaxBuyOrderPtr != 0) 
        FMaxBuyOrderPrice = FMaxBuyOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    else
        FMaxBuyOrderPrice = 0.0;
    Unlock(LockType);
}  //CRMEntrustStockTradingData::UpdateMaxBuyOrderPrice()
//------------------------------------------------------------------------------
bool CRMEntrustStockTradingData::InsertSellOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    bool isSuccess = UFC::InsertElementPtrIntoStrIdxHMap(FSellOrderIDIndex, OrderID, OrderPtr, CheckExist, FRWLockPtr, UFC::rwNotLock);
    if (isSuccess)
    {
        double orderPrice = OrderPtr->GetOrderPrice(UFC::rwLockForRead);
        if (orderPrice < FMinSellOrderPrice) 
        {
            FMinSellOrderPrice = orderPrice;
            FMinSellOrderPtr = OrderPtr;
        }
    }
    Unlock(LockType);
    return isSuccess;
}  //CRMEntrustStockTradingData::InsertSellOrder()
//------------------------------------------------------------------------------
CRMOrderSummary* CRMEntrustStockTradingData::RemoveSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    CRMOrderSummary *targetOrderPtr = UFC::RemoveElementPtrFromStrIdxHMap(FSellOrderIDIndex, OrderID, FRWLockPtr, UFC::rwNotLock);
    if (targetOrderPtr != 0) 
    {
        FMinSellOrderPtr = FindMinOrderPriceOrder(FSellOrderIDIndex, UFC::rwNotLock);
        if (FMinSellOrderPtr != 0) 
            FMinSellOrderPrice = FMinSellOrderPtr->GetOrderPrice(UFC::rwLockForRead);
        else
            FMinSellOrderPrice = 100000.0;
    }
    Unlock(LockType);
    return targetOrderPtr;
}  //CRMEntrustStockTradingData::RemoveSellOrder()
//------------------------------------------------------------------------------
void CRMEntrustStockTradingData::UpdateMinSellOrderPrice(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    FMinSellOrderPtr = FindMinOrderPriceOrder(FSellOrderIDIndex, UFC::rwNotLock);
    if (FMinSellOrderPtr != 0) 
        FMinSellOrderPrice = FMinSellOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    else
        FMinSellOrderPrice = 100000.0;
    Unlock(LockType);
}  //CRMEntrustStockTradingData::UpdateMinSellOrderPrice()
//------------------------------------------------------------------------------
CRMEntrustStockCondition* CRMEntrustStockTradingData::FindCondition(const UFC::AnsiString& SubaccountNo, char TWSEOrderType, UFC::RWLockTypeEnum LockType)
{
    UFC::AnsiString keyValue = CRMEntrustStockCondition::BuildIndexKey(SubaccountNo, TWSEOrderType);
    return UFC::FindElementPtrFromStrIdxHMap(FConditionWarrantBrokerOrdTpeIndex, keyValue, FRWLockPtr, LockType);
}  //CRMEntrustStockTradingData::FindCondition()
//------------------------------------------------------------------------------
bool CRMEntrustStockTradingData::InsertCondition(const UFC::AnsiString& SubaccountNo, char TWSEOrderType, CRMEntrustStockCondition *ConditionPtr, bool CheckExist, UFC::RWLockTypeEnum LockType)
{
    UFC::AnsiString keyValue = CRMEntrustStockCondition::BuildIndexKey(SubaccountNo, TWSEOrderType);
    return UFC::InsertElementPtrIntoStrIdxHMap(FConditionWarrantBrokerOrdTpeIndex, keyValue, ConditionPtr, CheckExist, FRWLockPtr, LockType);            
}  //CRMEntrustStockTradingData::InsertCondition()
//------------------------------------------------------------------------------
CRMEntrustStockCondition* CRMEntrustStockTradingData::RemoveCondition(const UFC::AnsiString& SubaccountNo, char TWSEOrderType, UFC::RWLockTypeEnum LockType)
{
    UFC::AnsiString keyValue = CRMEntrustStockCondition::BuildIndexKey(SubaccountNo, TWSEOrderType);
    return UFC::RemoveElementPtrFromStrIdxHMap(FConditionWarrantBrokerOrdTpeIndex, keyValue, FRWLockPtr, LockType);
}  //CRMEntrustStockTradingData::RemoveCondition()
//------------------------------------------------------------------------------
void CRMEntrustStockTradingData::DumpCondition(UFC::BufferedLog* LogPtr, UFC::RWLockTypeEnum LockType)
{
    int dumpCount = 0;
    Lock(LockType);
    CRMEntrustStockCondition* curConditionPtr = FConditionWarrantBrokerOrdTpeIndex.First();
    while (curConditionPtr != 0)
    {
        dumpCount++;
        curConditionPtr->LockForRead();
        UFC::AnsiString objLogStr = curConditionPtr->ToString(UFC::rwNotLock);
        curConditionPtr->UnLockForRead();
        LogPtr->fprintf(" %4d %s", dumpCount, objLogStr.c_str());
        curConditionPtr = FConditionWarrantBrokerOrdTpeIndex.Next();
    }
    Unlock(LockType);
}  //CRMEntrustStockTradingData::DumpCondition()
//------------------------------------------------------------------------------
UFC::AnsiString CRMEntrustStockTradingData::ToString(UFC::RWLockTypeEnum LockType)
{
    UFC::AnsiString objStr = "";
    Lock(LockType);
    UFC::AnsiString regularMaxBuyStr;
    if (FMaxRegularBuyOrderPtr != 0)
        regularMaxBuyStr.Printf("R:%s:%10.4lf", FMaxRegularBuyOrderPtr->GetOrderID(UFC::rwLockForRead).c_str(), FMaxRegularBuyOrderPrice);
    else
        regularMaxBuyStr.Printf("R:%10.4lf", FMaxRegularBuyOrderPrice);

    UFC::AnsiString intraDayOddMaxBuyStr;
    if (FMaxIntraDayOddBuyOrderPtr != 0)
        intraDayOddMaxBuyStr.Printf("O:%s:%10.4lf", FMaxIntraDayOddBuyOrderPtr->GetOrderID(UFC::rwLockForRead).c_str(), FMaxIntraDayOddBuyOrderPrice);
    else
        intraDayOddMaxBuyStr.Printf("O:%10.4lf", FMaxIntraDayOddBuyOrderPrice);     
    
    UFC::AnsiString maxBuyStr;
    if (FMaxBuyOrderPtr != 0)
        maxBuyStr.Printf("%s:%10.4lf", FMaxBuyOrderPtr->GetOrderID(UFC::rwLockForRead).c_str(), FMaxBuyOrderPrice);
    else
        maxBuyStr.Printf("%10.4lf", FMaxBuyOrderPrice);

    UFC::AnsiString regularMinSellStr;
    if (FMinRegularSellOrderPtr != 0)
        regularMinSellStr.Printf("R:%s:%10.4lf", FMinRegularSellOrderPtr->GetOrderID(UFC::rwLockForRead).c_str(), FMinRegularSellOrderPrice);
    else
        regularMinSellStr.Printf("R:%10.4lf", FMinRegularSellOrderPrice);
    
    UFC::AnsiString intraDayOddMinSellStr;
    if (FMinIntraDayOddSellOrderPtr != 0)
        intraDayOddMinSellStr.Printf("O:%s:%10.4lf", FMinIntraDayOddSellOrderPtr->GetOrderID(UFC::rwLockForRead).c_str(), FMinIntraDayOddSellOrderPrice);
    else
        intraDayOddMinSellStr.Printf("O:%10.4lf", FMinIntraDayOddSellOrderPrice);
    
    UFC::AnsiString minSellStr;
    if (FMinSellOrderPtr != 0)
        minSellStr.Printf("%s:%10.4lf", FMinSellOrderPtr->GetOrderID(UFC::rwLockForRead).c_str(), FMinSellOrderPrice);
    else
        minSellStr.Printf("%10.4lf", FMinSellOrderPrice);
    
    objStr.Printf("Acc[%s] Stock[%s] MaxBuy[%s, %s, %s] MinSell[%s, %s, %s]",
                  FAccountNo.c_str(), FStockNo.c_str(),
                  regularMaxBuyStr.c_str(), intraDayOddMaxBuyStr.c_str(), maxBuyStr.c_str(),
                  regularMinSellStr.c_str(), intraDayOddMinSellStr.c_str(), minSellStr.c_str());
    Unlock(LockType);
    return objStr;
}  //CRMEntrustStockTradingData::ToString()

//----------------------------- CRMEntrustInvestorAccount -----------------------------
//------------------------------------------------------------------------------
CRMEntrustInvestorAccount::CRMEntrustInvestorAccount(const UFC::AnsiString& AccountNo)
:UFC::CMainKeyObject(UFC::cstCreateLock, AccountNo)
,FAccountNo(AccountNo)
,FDayTradeType(0)
,FNeedCheckSelfTrade(false)
,FNeedCheckSellPosition(true)
,FStockAmountLimit(-1.0)
,FNeedCheckStockWhiteList(false)
,FPositionAmount(0.0)
,FBuyOrderAmount(0.0)
,FSellOrderAmount(0.0)
,FBuyMatchAmount(0.0)
,FSellMatchAmount(0.0)
{
}  //CRMEntrustInvestorAccount::CRMEntrustInvestorAccount()
//------------------------------------------------------------------------------
CRMEntrustInvestorAccount::~CRMEntrustInvestorAccount()
{    
}  //CRMEntrustInvestorAccount::~CRMEntrustInvestorAccount()
//------------------------------------------------------------------------------
CRMEntrustStockTradingData *CRMEntrustInvestorAccount::FindOrCreateStockTradingData(const UFC::AnsiString& StockNo, bool& IsNewStockData, UFC::RWLockTypeEnum LockType)
{
    IsNewStockData = false;
    CRMEntrustStockTradingData *stockDataPtr = 0;
    if (StockNo.Length() <= 0) return stockDataPtr;
    Lock(LockType);
    if (FStockNoIndex.GetObjectByKey(StockNo, stockDataPtr) == FALSE)
    {
        stockDataPtr = new CRMEntrustStockTradingData(StockNo);
        stockDataPtr->SetAccountNo(FAccountNo, UFC::rwNotLock);
        if (FStockNoIndex.Add(StockNo, stockDataPtr) == TRUE)
            IsNewStockData = true;
        else
        {
            delete stockDataPtr;
            stockDataPtr = 0;
        }
    }
    
    Unlock(LockType);
    return stockDataPtr;
}  //CRMEntrustInvestorAccount::FindOrCreateStockTradingData()
//------------------------------------------------------------------------------
CRMEntrustStockTradingData *CRMEntrustInvestorAccount::FindOrCreateStockTradingData(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum LockType)
{
    CRMEntrustStockTradingData *stockDataPtr = 0;
    if (StockNo.Length() <= 0) return stockDataPtr;
    Lock(LockType);
    if (FStockNoIndex.GetObjectByKey(StockNo, stockDataPtr) == FALSE)
    {
        stockDataPtr = new CRMEntrustStockTradingData(StockNo);
        stockDataPtr->SetAccountNo(FAccountNo, UFC::rwNotLock);
        if (FStockNoIndex.Add(StockNo, stockDataPtr) == FALSE)
        {
            delete stockDataPtr;
            stockDataPtr = 0;
        }
    }
    
    Unlock(LockType);
    return stockDataPtr;
}  //CRMEntrustInvestorAccount::FindOrCreateStockTradingData()
//------------------------------------------------------------------------------
void CRMEntrustInvestorAccount::DumpStockTradingData(UFC::BufferedLog* LogPtr, UFC::RWLockTypeEnum LockType)
{
    int dumpCount = 0;
    Lock(LockType);
    CRMEntrustStockTradingData* curStockTradingPtr = FStockNoIndex.First();
    while (curStockTradingPtr != 0)
    {
        dumpCount++;
        curStockTradingPtr->LockForRead();
        LogPtr->fprintf(" %4d %s", dumpCount, curStockTradingPtr->ToString(UFC::rwNotLock).c_str());
        curStockTradingPtr->DumpCondition(LogPtr, UFC::rwNotLock);
        curStockTradingPtr->UnLockForRead();
        curStockTradingPtr = FStockNoIndex.Next();
    }
    Unlock(LockType);
}  //CRMEntrustInvestorAccount::DumpStockTradingData()
//----------------------------------------------------------------------------------------------------------------------
int CRMEntrustInvestorAccount::GetStockWhiteListCount(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int stockCount = FStockWhiteListSet.ItemCount();
    Unlock(LockType);
    return stockCount;
}  //CRMEntrustInvestorAccount::GetStockWhiteListCount()
//----------------------------------------------------------------------------------------------------------------------
bool CRMEntrustInvestorAccount::IsStockInWhiteList(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum LockType)
{
    bool isExist = false;
    if (StockNo.Length() > 0)
    {
        Lock(LockType);
        if (FStockWhiteListSet.Exists(StockNo) == TRUE) isExist = true;
        Unlock(LockType);
    }
    return isExist;
}  //CRMEntrustInvestorAccount::IsStockInWhiteList()
//----------------------------------------------------------------------------------------------------------------------
void CRMEntrustInvestorAccount::AddStockToWhiteList(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum LockType)
{
    if (StockNo.Length() <= 0) return;
    Lock(LockType);
    FStockWhiteListSet.Add(StockNo);
    Unlock(LockType);
}  //CRMEntrustInvestorAccount::AddStockToWhiteList()
//------------------------------------------------------------------------------
void CRMEntrustInvestorAccount::ClearStockWhiteList(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    FStockWhiteListSet.Clear();
    Unlock(LockType);
}  //CRMEntrustInvestorAccount::ClearStockWhiteList()
//------------------------------------------------------------------------------
UFC::AnsiString CRMEntrustInvestorAccount::GetStockWhiteListCSV(UFC::RWLockTypeEnum LockType)
{
    UFC::AnsiString whiteListCSV = "";
    UFC::PStringBuffer whiteListBuffer;
    UFC::AnsiString curStockNo;
    Lock(LockType);
    if (FStockWhiteListSet.First(curStockNo) == TRUE)
    {
        do 
        {
            if (whiteListBuffer.Length() > 0) whiteListBuffer.Append(',');
            whiteListBuffer.Append(curStockNo);
        } while(FStockWhiteListSet.Next(curStockNo) == TRUE);
        
        if (whiteListBuffer.Length() > 0)
            whiteListCSV = whiteListBuffer.ToString();
    }  //if (FStockWhiteListSet.First(curStockNo) == TRUE)
    Unlock(LockType);
    return whiteListCSV;
}  //CRMEntrustInvestorAccount::GetStockWhiteListCSV()
//------------------------------------------------------------------------------
bool CRMEntrustInvestorAccount::CheckOrderAmount(const UFC::AnsiString& OrderID, char BuySellCode, double OrderAmount, UFC::AnsiString& CheckMessage, UFC::RWLockTypeEnum LockType)
{
    bool isSuccess = true;
    Lock(LockType);
    if (FStockAmountLimit > 0.0)
    {
        double usedAmount = 0.0;
        UFC::AnsiString usedAmountStr = "";
        if (BuySellCode == 'B')
        {
            usedAmount = FPositionAmount + FBuyOrderAmount + FBuyMatchAmount - FSellMatchAmount;
            usedAmountStr.Printf("UsedAmt[%15.4lf](PosAmt[%15.4lf] + BuyOrdAmt[%15.4lf] + BuyMatchAmt[%15.4lf] - SellMatchAmt[%15.4lf])", usedAmount, FPositionAmount, FBuyOrderAmount, FBuyMatchAmount, FSellMatchAmount);
            if ((usedAmount + OrderAmount) > FStockAmountLimit)
            {
                CheckMessage.Printf("Reject[%s]: %s + OrderAmt[%15.4lf] > AmtLimit[%15.4lf]", OrderID.c_str(), usedAmountStr.c_str(), OrderAmount, FStockAmountLimit);
                isSuccess = false;
            }
        }
    }  //if (FStockAmountLimit > 0.0)
    Unlock(LockType);
    return isSuccess;
}  //CRMEntrustInvestorAccount::CheckOrderAmount()
//------------------------------------------------------------------------------
UFC::AnsiString CRMEntrustInvestorAccount::ToString(UFC::RWLockTypeEnum LockType)
{
    UFC::PStringBuffer objBuffer;
    Lock(LockType);
    objBuffer.AppendPrintf("RMAccount[%s] DT[%d] StkAmtLimit[%12.1lf]", FAccountNo.c_str(), FDayTradeType, FStockAmountLimit);
    if (FNeedCheckSelfTrade) objBuffer.Append(" CheckSelfTrade");
    if (FNeedCheckSellPosition) objBuffer.Append(" CheckSellPosition");
    if (FNeedCheckStockWhiteList) objBuffer.Append(" CheckStkWhiteList");
    objBuffer.AppendPrintf(" Amount{Pos[%15.4lf] BuyOrd[%15.4lf] SellOrd[%15.4lf] BuyMatch[%15.4lf] SellMatch[%15.4lf]}", FPositionAmount, FBuyOrderAmount, FSellOrderAmount, FBuyMatchAmount, FSellMatchAmount);
    Unlock(LockType);
    return objBuffer.ToString();
}  //CRMEntrustInvestorAccount::ToString()
}  //namespace twserm