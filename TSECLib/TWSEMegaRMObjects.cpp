/* 
 * File:   TWSEMegaRMObjects.cpp
 * Author: Zhen Fan
 *
 * Created on 2021/01/15 11:40
 */

#include "TWSEMegaRMObjects.h"

namespace twserm
{
//------------------------------- CRMHedgeStock --------------------------------
//------------------------------------------------------------------------------
CRMHedgeStock::CRMHedgeStock(const UFC::AnsiString& StockNo)
:FStockNo(StockNo)
{
}  //CRMHedgeStock::CRMHedgeStock()
//------------------------------------------------------------------------------
bool CRMHedgeStock::IsAccountExists(const UFC::AnsiString& AccountNo)
{
    if ((AccountNo.Length() > 0) && (FAccountSet.Exists(AccountNo) == TRUE))
        return true;
    else
        return false;
}  //CRMHedgeStock::IsAccountExists()
//------------------------------------------------------------------------------
bool CRMHedgeStock::AddAccountToSet(const UFC::AnsiString& AccountNo)
{
    if ((AccountNo.Length() > 0) && (FAccountSet.Exists(AccountNo) == FALSE))
    {
        FAccountSet.Add(AccountNo);
        return true;
    }
    else
        return false;
}  //CRMHedgeStock::AddAccountToSet()
//------------------------------------------------------------------------------
UFC::AnsiString CRMHedgeStock::ToAccountCSV()
{
    UFC::AnsiString csvStr = "";
    if (FAccountSet.ItemCount() <= 0) return csvStr;
    
    UFC::AnsiString curAccountNo;
    if (FAccountSet.First(curAccountNo) == TRUE)
    {
        UFC::PStringBuffer objBuffer;
        do 
        {
            if (objBuffer.Length() > 0) objBuffer.Append(',');
            objBuffer.Append(curAccountNo);
        } while(FAccountSet.Next(curAccountNo) == TRUE);
        csvStr = objBuffer.ToString();
    }
    return csvStr;
}  //CRMHedgeStock::ToAccountCSV()
//------------------------------------------------------------------------------
UFC::AnsiString CRMHedgeStock::ToString()
{
    UFC::AnsiString accountCSV = ToAccountCSV();
    UFC::AnsiString objStr = "";
    if (accountCSV.Length() > 0)
        objStr.Printf("HedgeStock[%s]:%s", FStockNo.c_str(), accountCSV.c_str());
    else
        objStr.Printf("HedgeStock[%s]", FStockNo.c_str());
    return objStr;
}  //CRMHedgeStock::ToString()

//------------------------------ CRMOrderSummary -------------------------------
//------------------------------------------------------------------------------
void CRMOrderSummary::ParseUserData(const UFC::AnsiString& OrderKey, UFC::AnsiString& WarrantNo, UFC::AnsiString& BrokerID)
{
    WarrantNo = "";
    BrokerID = "";
    int keyLength = OrderKey.Length();
    if (keyLength <= 0) return;

    int commaPos = OrderKey.AnsiPos(',');
    if (commaPos < 0) return;
    
    int userDataLength = keyLength - (commaPos + 1);
    if (userDataLength <= 0) return;        
    UFC::AnsiString userData = OrderKey.SubString(commaPos + 1, userDataLength);
    
    UFC::PStringList riskManagerFieldList;
    riskManagerFieldList.SetStrings(userData, ";\n", true);  //Keep empty Field
    
    if (riskManagerFieldList.ItemCount() >= 1) WarrantNo = riskManagerFieldList[0];
    if (riskManagerFieldList.ItemCount() >= 2) BrokerID = riskManagerFieldList[1];
}  //CRMOrderSummary::ParseRMDataFromUserData()
//------------------------------------------------------------------------------
double CRMOrderSummary::Parse95v9999PriceStr(const UFC::AnsiString& PriceStr)
{
    double price = 0.0;
    if (PriceStr.Length() != 9) return price;
    UFC::AnsiString intPartStr = PriceStr.SubString(0, 5);
    UFC::AnsiString fractionPartStr = PriceStr.SubString(5, 4);
    price = intPartStr.ToDouble() + fractionPartStr.ToDouble() / 10000.0;
    return price;
}  //CRMOrderSummary::Parse95v9999PriceStr(()
//------------------------------------------------------------------------------
CRMOrderSummary::CRMOrderSummary(int FunctionCode, TWSE::TMessageType MsgType, const UFC::AnsiString& OrderKey, TWSE::MessageHeader*& OrderMsg)
:UFC::CMainKeyObject(UFC::cstCreateLock)
,FNetworkID(0)
,FFunctionCode(FunctionCode)
,FOrderKey(OrderKey)
,FMsgType(MsgType)    
,FBrokerID("")
,FOrderID("")
,FAccountNo("")
,FStockNo("")
,FOrderPrice(0.0)
,FOrigOrderQty(0)
,FOrderQty(0)
,FSide(0)
,FExchangeCode(0)
,FTWSEOrderType(0)
,FTWSEPriceType(0)
,FTWSETimeInForce(0)
,FTradingSessionID(nsOrderMessageDefine::tsAuto)
,FSessionName("")
,FWarrantNo("")
,FLeavesQty(0)
,FCumQty(0)
,FAmountPrice(0.0)
,FOrderAmount(0.0)
,FCumAmount(0.0)
,FTradeUnit(1000)
,FOrigOrderPrice(0.0)
,FOrigOrderAmount(0.0)
,FIsPassed(false)
{
    ParseUserData(FOrderKey, FWarrantNo, FBrokerID);
    if (FMsgType == TWSE::mtT010)
        SetRegularOrderFields(static_cast<TWSE::MessageT010*>(OrderMsg));
    else if (FMsgType == TWSE::mtO110)
        SetInterDayOddOrderFields(static_cast<TWSE::MessageO110*>(OrderMsg));
    else if (FMsgType == TWSE::mtO010)
        SetOddOrderFields(static_cast<TWSE::MessageO010*>(OrderMsg));
    else if (FMsgType == TWSE::mtP010)
        SetFixPriceOrderFields(static_cast<TWSE::MessageP010*>(OrderMsg));
    
    FAccountNo.TrimRight(' ');
    FStockNo.TrimRight(' ');
    FLeavesQty = FOrderQty;
    FAmountPrice = FOrderPrice;
    FOrderAmount = static_cast<double>(FOrderQty) * FAmountPrice;
}  //CRMOrderSummary::CRMOrderSummary()
//------------------------------------------------------------------------------
CRMOrderSummary::CRMOrderSummary(const UFC::AnsiString& OrderID, char Side, const UFC::AnsiString& StockNo,
                                 const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, char TWSEOrderType, 
                                 double OrderPrice, int OrderQty, char ExchangeCode, TWSE::TMessageType MsgType, const UFC::AnsiString& AccountNo)
:UFC::CMainKeyObject(UFC::cstCreateLock, OrderID)
,FNetworkID(0)
,FFunctionCode(0)
,FOrderKey("")
,FMsgType(MsgType)
,FBrokerID(BrokerID)
,FOrderID(OrderID)
,FAccountNo(AccountNo)
,FStockNo(StockNo)
,FOrderPrice(OrderPrice)
,FOrigOrderQty(0)
,FOrderQty(OrderQty)
,FSide(Side)
,FExchangeCode(ExchangeCode)
,FTWSEOrderType(TWSEOrderType)
,FTWSEPriceType(0)
,FTWSETimeInForce(0)
,FTradingSessionID(nsOrderMessageDefine::tsAuto)
,FSessionName("")
,FWarrantNo(WarrantNo)
,FLeavesQty(OrderQty)
,FCumQty(0)
,FAmountPrice(0.0)
,FOrderAmount(0.0)
,FCumAmount(0.0)
,FTradeUnit(1000)
,FOrigOrderPrice(0.0)
,FOrigOrderAmount(0.0)
,FIsPassed(false)
{
    FAmountPrice = FOrderPrice;
    FOrderAmount = static_cast<double>(FOrderQty) * FAmountPrice;
}  //CRMOrderSummary::CRMOrderSummary()
//------------------------------------------------------------------------------
void CRMOrderSummary::SetRegularOrderFields(TWSE::MessageT010 *T010MsgPtr)
{
    FTradingSessionID = nsOrderMessageDefine::tsNormal;
    FSessionName = "Regular";
    FOrderID = UFC::AnsiString(T010MsgPtr->OrderNo, 5);
    FAccountNo = UFC::AnsiString(T010MsgPtr->InvestorAcno, 7);
    FStockNo = UFC::AnsiString(T010MsgPtr->StockID, 6);
    UFC::AnsiString orderQtyStr = UFC::AnsiString(T010MsgPtr->OrderQty, 6);
    FOrigOrderQty = orderQtyStr.ToInt();
    
    FSide = T010MsgPtr->BuySellCode;
    FExchangeCode = T010MsgPtr->ExchangeCode;
    FTWSEOrderType = T010MsgPtr->TWSEOrderType;
    FTWSEPriceType = T010MsgPtr->OrderType;
    FTWSETimeInForce = T010MsgPtr->TimeInForce;

    if (FTWSEPriceType == '2')  //Limit
    {
        UFC::AnsiString orderPriceStr = UFC::AnsiString(T010MsgPtr->OrderPrice, 9);
        if (orderPriceStr.Length() > 0)
            FOrderPrice = Parse95v9999PriceStr(orderPriceStr);
    }
    
    FOrderQty = FOrigOrderQty * FTradeUnit;
}  //CRMOrderSummary::SetRegularOrderFields()
//------------------------------------------------------------------------------
void CRMOrderSummary::SetInterDayOddOrderFields(TWSE::MessageO110 *O110MsgPtr)
{
    FTradingSessionID = nsOrderMessageDefine::tsIntradayOdd;
    FSessionName = "InterDayOdd";
    FOrderID = UFC::AnsiString(O110MsgPtr->OrderNo, 5);
    FAccountNo = UFC::AnsiString(O110MsgPtr->InvestorAcno, 7);
    FStockNo = UFC::AnsiString(O110MsgPtr->StockID, 6);
    UFC::AnsiString orderQtyStr = UFC::AnsiString(O110MsgPtr->OrderQty, 6);
    FOrigOrderQty = orderQtyStr.ToInt();
    
    FSide = O110MsgPtr->BuySellCode;
    FExchangeCode = '2';  //Odd
    FTWSEOrderType = '0';
    FTWSEPriceType = '2';    //Limit
    FTWSETimeInForce = '0';  //ROD

    if (FTWSEPriceType == '2')  //Limit
    {
        UFC::AnsiString orderPriceStr = UFC::AnsiString(O110MsgPtr->OrderPrice, 9);
        if (orderPriceStr.Length() > 0)
            FOrderPrice = Parse95v9999PriceStr(orderPriceStr);
    }
    
    FOrderQty = FOrigOrderQty;
}  //CRMOrderSummary::SetInterDayOddOrderFields()
//------------------------------------------------------------------------------
void CRMOrderSummary::SetOddOrderFields(TWSE::MessageO010 *O010MsgPtr)
{
    FTradingSessionID = nsOrderMessageDefine::tsOddLot;
    FSessionName = "Odd";
    FOrderID = UFC::AnsiString(O010MsgPtr->OrderNo, 5);
    FAccountNo = UFC::AnsiString(O010MsgPtr->InvestorAcno, 7);
    FStockNo = UFC::AnsiString(O010MsgPtr->StockID, 6);
    UFC::AnsiString orderQtyStr = UFC::AnsiString(O010MsgPtr->OrderQty, 6);
    FOrigOrderQty = orderQtyStr.ToInt();
    
    FSide = O010MsgPtr->BuySellCode;
    FExchangeCode = '2';  //Odd
    FTWSEOrderType = '0';
    FTWSEPriceType = '2';    //Limit
    FTWSETimeInForce = '0';  //ROD

    if (FTWSEPriceType == '2')  //Limit
    {
        UFC::AnsiString orderPriceStr = UFC::AnsiString(O010MsgPtr->OrderPrice, 9);
        if (orderPriceStr.Length() > 0)
            FOrderPrice = Parse95v9999PriceStr(orderPriceStr);
    }
    
    FOrderQty = FOrigOrderQty;
}  //CRMOrderSummary::SetOddOrderFields()
//------------------------------------------------------------------------------
void CRMOrderSummary::SetFixPriceOrderFields(TWSE::MessageP010 *P010MsgPtr)
{
    FTradingSessionID = nsOrderMessageDefine::tsOffHour;
    FSessionName = "FixPrice";
    FOrderID = UFC::AnsiString(P010MsgPtr->OrderNo, 5);
    FAccountNo = UFC::AnsiString(P010MsgPtr->InvestorAcno, 7);
    FStockNo = UFC::AnsiString(P010MsgPtr->StockID, 6);
    UFC::AnsiString orderQtyStr = UFC::AnsiString(P010MsgPtr->OrderQty, 6);
    FOrigOrderQty = orderQtyStr.ToInt();
    
    FSide = P010MsgPtr->BuySellCode;
    FExchangeCode = P010MsgPtr->ExchangeCode;
    FTWSEOrderType = P010MsgPtr->TWSEOrderType;
    FTWSEPriceType = '2';    //Limit
    FTWSETimeInForce = '0';  //ROD

    if (FTWSEPriceType == '2')  //Limit
    {
        UFC::AnsiString orderPriceStr = UFC::AnsiString(P010MsgPtr->OrderPrice, 9);
        if (orderPriceStr.Length() > 0)
            FOrderPrice = Parse95v9999PriceStr(orderPriceStr);
    }
    
    FOrderQty = FOrigOrderQty * FTradeUnit;
}  //CRMOrderSummary::SetFixPriceOrderFields()
//------------------------------------------------------------------------------
int CRMOrderSummary::IncreaseOrderQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FOrderQty + IncreaseQty;
    FOrderQty = newValue;
    Unlock(LockType);
    return newValue;    
}  //CRMOrderSummary::IncreaseOrderQty()
//------------------------------------------------------------------------------
int CRMOrderSummary::IncreaseLeavesQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FLeavesQty + IncreaseQty;
    FLeavesQty = newValue;
    Unlock(LockType);
    return newValue;    
}  //CRMOrderSummary::IncreaseOrderQty()
//------------------------------------------------------------------------------
int CRMOrderSummary::IncreaseCumQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FCumQty + IncreaseQty;
    FCumQty = newValue;
    Unlock(LockType);
    return newValue;    
}  //CRMOrderSummary::IncreaseOrderQty()
//------------------------------------------------------------------------------
void CRMOrderSummary::SetTradeUnit(int NewValue, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    FTradeUnit = NewValue;
    if ((FMsgType == TWSE::mtT010) || (FMsgType == TWSE::mtP010))  //Regular or Fix Price
        FOrderQty = FOrigOrderQty * FTradeUnit;
    Unlock(LockType);
}  //CRMOrderSummary::SetTradeUnit()
//------------------------------------------------------------------------------
UFC::AnsiString CRMOrderSummary::ToString(UFC::RWLockTypeEnum LockType)
{
    UFC::PStringBuffer objBuffer;
    Lock(LockType);
    if (FSessionName.Length() > 0) objBuffer.AppendPrintf("%s", FSessionName.c_str());
    objBuffer.AppendPrintf(" NID[%010d] Order[%s] Stk[%s][%s][%s]", FNetworkID, FOrderID.c_str(), FStockNo.c_str(), FWarrantNo.c_str(), FBrokerID.c_str());
    
    if (FTWSEOrderType != 0) 
        objBuffer.AppendPrintf(" OrdType[%c]", FTWSEOrderType);
    else
        objBuffer.AppendPrintf(" OrdType[]");
        
    if (FAccountNo.Length() > 0)
        objBuffer.AppendPrintf(" Acc[%s]", FAccountNo.c_str());
    else
        objBuffer.AppendPrintf(" Acc[]");

    if (FSide != 0) 
        objBuffer.AppendPrintf(" Side[%c]", FSide);
    else
        objBuffer.AppendPrintf(" Side[]");
        
    if (FTWSEPriceType == '2')  //Limit
        objBuffer.AppendPrintf(" OrdPx[%10.4lf]", FOrderPrice);
    else
        objBuffer.AppendPrintf(" OrdPx[]");
    
    objBuffer.AppendPrintf(" OrdQty[%d]", FOrderQty);
    objBuffer.AppendPrintf(" LeavesQty[%d]", FLeavesQty);
    objBuffer.AppendPrintf(" CumQty[%d]", FCumQty);
    
    if (FExchangeCode != 0)
        objBuffer.AppendPrintf(" ExCode[%c]", FExchangeCode);
    else
        objBuffer.AppendPrintf(" ExCode[]");
        
    if (FTWSEPriceType != 0)
        objBuffer.AppendPrintf(" PxType[%c]", FTWSEPriceType);
    else
        objBuffer.AppendPrintf(" PxType[]");
        
    if (FTWSETimeInForce != 0)
        objBuffer.AppendPrintf(" TIF[%c]", FTWSETimeInForce);
    else
        objBuffer.AppendPrintf(" TIF[]");
        
    objBuffer.AppendPrintf(" AmtPx[%10.4lf]", FAmountPrice);
    objBuffer.AppendPrintf(" OrdAmt[%15.4lf]", FOrderAmount);
    objBuffer.AppendPrintf(" CumAmt[%15.4lf]", FCumAmount);
    Unlock(LockType);
    
    return objBuffer.ToString();
}  //CRMOrderSummary::ToString()
    
//-------------------------- CRMBrokerStockCondition ---------------------------
UFC::AnsiString CRMBrokerStockCondition::BuildIndexKey(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, char TWSEOrderType)
{
    UFC::AnsiString indexKey;
    indexKey.Printf("%s%s%c", WarrantNo.c_str(), BrokerID.c_str(), TWSEOrderType);
    return indexKey;
}  //CRMBrokerStockCondition::BuildIndexKey()
//------------------------------------------------------------------------------
CRMBrokerStockCondition::CRMBrokerStockCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, char TWSEOrderType,
                                                 int AvailableSellQty, int AvailableBuyQty,
                                                 const UFC::AnsiString& DataDate, bool IsSellShort)
:UFC::CMainKeyObject(UFC::cstCreateLock)
,FWarrantNo(WarrantNo)
,FBrokerID(BrokerID)
,FTWSEOrderType(TWSEOrderType)
,FAvailableSellQty(AvailableSellQty)
,FAvailableBuyQty(AvailableBuyQty)
,FDataDate(DataDate)
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
{
    FMainIndexKey = BuildIndexKey(FWarrantNo, FBrokerID, FTWSEOrderType);
}  //CRMBrokerStockCondition::CRMBrokerStockCondition()
//------------------------------------------------------------------------------
int CRMBrokerStockCondition::IncreaseRegularMatchBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FRegularMatchBuyQty + IncreaseQty;
    FRegularMatchBuyQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMBrokerStockCondition::IncreaseRegularMatchBuyQty()
//------------------------------------------------------------------------------
int CRMBrokerStockCondition::IncreaseRegularMatchSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FRegularMatchSellQty + IncreaseQty;
    FRegularMatchSellQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMBrokerStockCondition::IncreaseRegularMatchSellQty()
//------------------------------------------------------------------------------
int CRMBrokerStockCondition::IncreaseIntraDayOddMatchBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FIntraDayOddMatchBuyQty + IncreaseQty;
    FIntraDayOddMatchBuyQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMBrokerStockCondition::IncreaseIntraDayOddMatchBuyQty()
//------------------------------------------------------------------------------
int CRMBrokerStockCondition::IncreaseIntraDayOddMatchSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FIntraDayOddMatchSellQty + IncreaseQty;
    FIntraDayOddMatchSellQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMBrokerStockCondition::IncreaseIntraDayOddMatchSellQty()
//------------------------------------------------------------------------------
int CRMBrokerStockCondition::IncreaseMatchBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FMatchBuyQty + IncreaseQty;
    FMatchBuyQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMBrokerStockCondition::IncreaseMatchBuyQty()
//------------------------------------------------------------------------------
int CRMBrokerStockCondition::IncreaseMatchSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FMatchSellQty + IncreaseQty;
    FMatchSellQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMBrokerStockCondition::IncreaseMatchSellQty()
//------------------------------------------------------------------------------
int CRMBrokerStockCondition::IncreaseRegularOrderBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FRegularOrderBuyQty + IncreaseQty;
    FRegularOrderBuyQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMBrokerStockCondition::IncreaseRegularOrderBuyQty()
//------------------------------------------------------------------------------
int CRMBrokerStockCondition::IncreaseRegularOrderSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FRegularOrderSellQty + IncreaseQty;
    FRegularOrderSellQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMBrokerStockCondition::IncreaseRegularOrderSellQty()
//------------------------------------------------------------------------------
int CRMBrokerStockCondition::IncreaseIntraDayOddOrderBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FIntraDayOddOrderBuyQty + IncreaseQty;
    FIntraDayOddOrderBuyQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMBrokerStockCondition::IncreaseIntraDayOddOrderBuyQty()
//------------------------------------------------------------------------------
int CRMBrokerStockCondition::IncreaseIntraDayOddOrderSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FIntraDayOddOrderSellQty + IncreaseQty;
    FIntraDayOddOrderSellQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMBrokerStockCondition::IncreaseIntraDayOddOrderSellQty()
//------------------------------------------------------------------------------
int CRMBrokerStockCondition::IncreaseOrderBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FOrderBuyQty + IncreaseQty;
    FOrderBuyQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMBrokerStockCondition::IncreaseOrderBuyQty()
//------------------------------------------------------------------------------
int CRMBrokerStockCondition::IncreaseOrderSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int newValue = FOrderSellQty + IncreaseQty;
    FOrderSellQty = newValue;
    Unlock(LockType);
    return newValue;
}  //CRMBrokerStockCondition::IncreaseOrderSellQty()
//------------------------------------------------------------------------------
UFC::AnsiString CRMBrokerStockCondition::ToString(UFC::RWLockTypeEnum LockType)
{
    UFC::AnsiString objStr = "";
    UFC::AnsiString sellShortStr = " ";
    Lock(LockType);
    if (FIsSellShort) sellShortStr = "Sell Short";
    objStr.Printf("Condition:Acc[%s] Stk[%s][%s][%s]%c available[S %d, B %d] match[RS %d, RB %d; OS %d, OB %d; S %d, B %d] order[RS %d, RB %d; OS %d, OB %d; S %d, B %d] Date[%s] %s", 
                  FAccountNo.c_str(), FStockNo.c_str(), FWarrantNo.c_str(), FBrokerID.c_str(), FTWSEOrderType,
                  FAvailableSellQty, FAvailableBuyQty,
                  FRegularMatchSellQty, FRegularMatchBuyQty, FIntraDayOddMatchSellQty, FIntraDayOddMatchBuyQty, FMatchSellQty, FMatchBuyQty, 
                  FRegularOrderSellQty, FRegularOrderBuyQty, FIntraDayOddOrderSellQty, FIntraDayOddOrderBuyQty, FOrderSellQty, FOrderBuyQty, 
                  FDataDate.c_str(), sellShortStr.c_str());
    Unlock(LockType);
    return objStr;
}  //CRMBrokerStockCondition::ToString()

//---------------------------- CRMStockTradingData -----------------------------
//------------------------------------------------------------------------------
CRMStockTradingData::CRMStockTradingData(const UFC::AnsiString& StockNo)
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
}  //CRMStockTradingData::CRMStockTradingData(
//------------------------------------------------------------------------------
CRMStockTradingData::~CRMStockTradingData()
{
}  //CRMStockTradingData::~CRMStockTradingData()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::FindMaxOrderPriceOrder(UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*>& OrderIDIndex, UFC::RWLockTypeEnum LockType)
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
}  //CRMStockTradingData::FindMaxOrderPriceOrder()
//------------------------------------------------------------------------------
double CRMStockTradingData::FindMaxRegularBuyOrderPrice(UFC::RWLockTypeEnum LockType)
{
    double maxBuyOrderPrice = 0.0;
    CRMOrderSummary* maxOrderPtr = FindMaxOrderPriceOrder(FRegularBuyOrderIDIndex, LockType);
    if (maxOrderPtr != 0) maxBuyOrderPrice = maxOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return maxBuyOrderPrice;
}  //CRMStockTradingData::FindMaxRegularBuyOrderPrice()
//------------------------------------------------------------------------------
double CRMStockTradingData::FindMaxIntraDayOddBuyOrderPrice(UFC::RWLockTypeEnum LockType)
{
    double maxBuyOrderPrice = 0.0;
    CRMOrderSummary* maxOrderPtr = FindMaxOrderPriceOrder(FIntraDayOddBuyOrderIDIndex, LockType);
    if (maxOrderPtr != 0) maxBuyOrderPrice = maxOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return maxBuyOrderPrice;
}  //CRMStockTradingData::FindMaxIntraDayOddBuyOrderPrice()
//------------------------------------------------------------------------------
double CRMStockTradingData::FindMaxBuyOrderPrice(UFC::RWLockTypeEnum LockType)
{
    double maxBuyOrderPrice = 0.0;
    CRMOrderSummary* maxOrderPtr = FindMaxOrderPriceOrder(FBuyOrderIDIndex, LockType);
    if (maxOrderPtr != 0) maxBuyOrderPrice = maxOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return maxBuyOrderPrice;
}  //CRMStockTradingData::FindMaxBuyOrderPrice()
//------------------------------------------------------------------------------
CRMOrderSummary *CRMStockTradingData::FindMinOrderPriceOrder(UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*>& OrderIDIndex, UFC::RWLockTypeEnum LockType)
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
}  //CRMStockTradingData::FindMinOrderPriceOrder()
//------------------------------------------------------------------------------
double CRMStockTradingData::FindMinRegularSellOrderPrice(UFC::RWLockTypeEnum LockType)
{
    double minSellOrderPrice = 100000.0;
    CRMOrderSummary *minOrderPtr = FindMinOrderPriceOrder(FRegularSellOrderIDIndex, LockType);
    if (minOrderPtr != 0) minSellOrderPrice = minOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return minSellOrderPrice;
}  //CRMStockTradingData::FindMinRegularSellOrderPrice()
//------------------------------------------------------------------------------
double CRMStockTradingData::FindMinIntraDayOddSellOrderPrice(UFC::RWLockTypeEnum LockType)
{
    double minSellOrderPrice = 100000.0;
    CRMOrderSummary *minOrderPtr = FindMinOrderPriceOrder(FIntraDayOddSellOrderIDIndex, LockType);
    if (minOrderPtr != 0) minSellOrderPrice = minOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return minSellOrderPrice;
}  //CRMStockTradingData::FindMinIntraDayOddSellOrderPrice()
//------------------------------------------------------------------------------
double CRMStockTradingData::FindMinSellOrderPrice(UFC::RWLockTypeEnum LockType)
{
    double minSellOrderPrice = 100000.0;
    CRMOrderSummary *minOrderPtr = FindMinOrderPriceOrder(FSellOrderIDIndex, LockType);
    if (minOrderPtr != 0) minSellOrderPrice = minOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    return minSellOrderPrice;
}  //CRMStockTradingData::FindMinSellOrderPrice()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertRegularBuyOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType)
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
}  //CRMStockTradingData::InsertRegularBuyOrder()
//------------------------------------------------------------------------------
CRMOrderSummary* CRMStockTradingData::RemoveRegularBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType)
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
}  //CRMStockTradingData::RemoveRegularBuyOrder()
//------------------------------------------------------------------------------
void CRMStockTradingData::UpdateMaxRegularBuyOrderPrice(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    FMaxRegularBuyOrderPtr = FindMaxOrderPriceOrder(FRegularBuyOrderIDIndex, UFC::rwNotLock);
    if (FMaxRegularBuyOrderPtr != 0) 
        FMaxRegularBuyOrderPrice = FMaxRegularBuyOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    else
        FMaxRegularBuyOrderPrice = 0.0;
    Unlock(LockType);
}  //CRMStockTradingData::UpdateMaxRegularBuyOrderPrice()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertRegularSellOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType)
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
}  //CRMStockTradingData::InsertRegularSellOrder()
//------------------------------------------------------------------------------
CRMOrderSummary* CRMStockTradingData::RemoveRegularSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType)
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
}  //CRMStockTradingData::RemoveRegularSellOrder()
//------------------------------------------------------------------------------
void CRMStockTradingData::UpdateMinRegularSellOrderPrice(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    FMinRegularSellOrderPtr = FindMinOrderPriceOrder(FRegularSellOrderIDIndex, UFC::rwNotLock);
    if (FMinRegularSellOrderPtr != 0) 
        FMinRegularSellOrderPrice = FMinRegularSellOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    else
        FMinRegularSellOrderPrice = 100000.0;
    Unlock(LockType);
}  //CRMStockTradingData::UpdateMinRegularSellOrderPrice()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertIntraDayOddBuyOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType)
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
}  //CRMStockTradingData::InsertIntraDayOddBuyOrder()
//------------------------------------------------------------------------------
CRMOrderSummary* CRMStockTradingData::RemoveIntraDayOddBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType)
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
}  //CRMStockTradingData::RemoveIntraDayOddBuyOrder()
//------------------------------------------------------------------------------
void CRMStockTradingData::UpdateMaxIntraDayOddBuyOrderPrice(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    FMaxIntraDayOddBuyOrderPtr = FindMaxOrderPriceOrder(FIntraDayOddBuyOrderIDIndex, UFC::rwNotLock);
    if (FMaxIntraDayOddBuyOrderPtr != 0) 
        FMaxIntraDayOddBuyOrderPrice = FMaxIntraDayOddBuyOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    else
        FMaxIntraDayOddBuyOrderPrice = 0.0;
    Unlock(LockType);
}  //CRMStockTradingData::UpdateMaxIntraDayOddBuyOrderPrice()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertIntraDayOddSellOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType)
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
}  //CRMStockTradingData::InsertIntraDayOddSellOrder()
//------------------------------------------------------------------------------
CRMOrderSummary* CRMStockTradingData::RemoveIntraDayOddSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType)
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
}  //CRMStockTradingData::RemoveIntraDayOddSellOrder()
//------------------------------------------------------------------------------
void CRMStockTradingData::UpdateMinIntraDayOddSellOrderPrice(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    FMinIntraDayOddSellOrderPtr = FindMinOrderPriceOrder(FIntraDayOddSellOrderIDIndex, UFC::rwNotLock);
    if (FMinIntraDayOddSellOrderPtr != 0) 
        FMinIntraDayOddSellOrderPrice = FMinIntraDayOddSellOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    else
        FMinIntraDayOddSellOrderPrice = 100000.0;
    Unlock(LockType);
}  //CRMStockTradingData::UpdateMinIntraDayOddSellOrderPrice()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertBuyOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType)
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
}  //CRMStockTradingData::InsertBuyOrder()
//------------------------------------------------------------------------------
CRMOrderSummary* CRMStockTradingData::RemoveBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType)
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
}  //CRMStockTradingData::RemoveBuyOrder()
//------------------------------------------------------------------------------
void CRMStockTradingData::UpdateMaxBuyOrderPrice(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    FMaxBuyOrderPtr = FindMaxOrderPriceOrder(FBuyOrderIDIndex, UFC::rwNotLock);
    if (FMaxBuyOrderPtr != 0) 
        FMaxBuyOrderPrice = FMaxBuyOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    else
        FMaxBuyOrderPrice = 0.0;
    Unlock(LockType);
}  //CRMStockTradingData::UpdateMaxBuyOrderPrice()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertSellOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType)
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
}  //CRMStockTradingData::InsertSellOrder()
//------------------------------------------------------------------------------
CRMOrderSummary* CRMStockTradingData::RemoveSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType)
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
}  //CRMStockTradingData::RemoveSellOrder()
//------------------------------------------------------------------------------
void CRMStockTradingData::UpdateMinSellOrderPrice(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    FMinSellOrderPtr = FindMinOrderPriceOrder(FSellOrderIDIndex, UFC::rwNotLock);
    if (FMinSellOrderPtr != 0) 
        FMinSellOrderPrice = FMinSellOrderPtr->GetOrderPrice(UFC::rwLockForRead);
    else
        FMinSellOrderPrice = 100000.0;
    Unlock(LockType);
}  //CRMStockTradingData::UpdateMinSellOrderPrice()
//------------------------------------------------------------------------------
CRMBrokerStockCondition* CRMStockTradingData::FindCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, char TWSEOrderType, UFC::RWLockTypeEnum LockType)
{
    UFC::AnsiString keyValue = CRMBrokerStockCondition::BuildIndexKey(WarrantNo, BrokerID, TWSEOrderType);
    return UFC::FindElementPtrFromStrIdxHMap(FConditionWarrantBrokerOrdTpeIndex, keyValue, FRWLockPtr, LockType);
}  //CRMStockTradingData::FindCondition()
//------------------------------------------------------------------------------
bool CRMStockTradingData::InsertCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, char TWSEOrderType, CRMBrokerStockCondition *ConditionPtr, bool CheckExist, UFC::RWLockTypeEnum LockType)
{
    UFC::AnsiString keyValue = CRMBrokerStockCondition::BuildIndexKey(WarrantNo, BrokerID, TWSEOrderType);
    return UFC::InsertElementPtrIntoStrIdxHMap(FConditionWarrantBrokerOrdTpeIndex, keyValue, ConditionPtr, CheckExist, FRWLockPtr, LockType);            
}  //CRMStockTradingData::InsertCondition()
//------------------------------------------------------------------------------
CRMBrokerStockCondition* CRMStockTradingData::RemoveCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, char TWSEOrderType, UFC::RWLockTypeEnum LockType)
{
    UFC::AnsiString keyValue = CRMBrokerStockCondition::BuildIndexKey(WarrantNo, BrokerID, TWSEOrderType);
    return UFC::RemoveElementPtrFromStrIdxHMap(FConditionWarrantBrokerOrdTpeIndex, keyValue, FRWLockPtr, LockType);
}  //CRMStockTradingData::RemoveCondition()
//------------------------------------------------------------------------------
void CRMStockTradingData::DumpCondition(UFC::BufferedLog* LogPtr, UFC::RWLockTypeEnum LockType)
{
    int dumpCount = 0;
    Lock(LockType);
    CRMBrokerStockCondition* curConditionPtr = FConditionWarrantBrokerOrdTpeIndex.First();
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
}  //CRMStockTradingData::DumpCondition()
//------------------------------------------------------------------------------
UFC::AnsiString CRMStockTradingData::ToString(UFC::RWLockTypeEnum LockType)
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
}  //CRMStockTradingData::ToString()

//----------------------------- CRMInvestorAccount -----------------------------
//------------------------------------------------------------------------------
CRMInvestorAccount::CRMInvestorAccount(const UFC::AnsiString& AccountNo)
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
}  //CRMInvestorAccount::CRMInvestorAccount()
//------------------------------------------------------------------------------
CRMInvestorAccount::~CRMInvestorAccount()
{    
}  //CRMInvestorAccount::~CRMInvestorAccount()
//------------------------------------------------------------------------------
CRMStockTradingData *CRMInvestorAccount::FindOrCreateStockTradingData(const UFC::AnsiString& StockNo, bool& IsNewStockData, UFC::RWLockTypeEnum LockType)
{
    IsNewStockData = false;
    CRMStockTradingData *stockDataPtr = 0;
    if (StockNo.Length() <= 0) return stockDataPtr;
    Lock(LockType);
    if (FStockNoIndex.GetObjectByKey(StockNo, stockDataPtr) == FALSE)
    {
        stockDataPtr = new CRMStockTradingData(StockNo);
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
}  //CRMInvestorAccount::FindOrCreateStockTradingData()
//------------------------------------------------------------------------------
CRMStockTradingData *CRMInvestorAccount::FindOrCreateStockTradingData(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum LockType)
{
    CRMStockTradingData *stockDataPtr = 0;
    if (StockNo.Length() <= 0) return stockDataPtr;
    Lock(LockType);
    if (FStockNoIndex.GetObjectByKey(StockNo, stockDataPtr) == FALSE)
    {
        stockDataPtr = new CRMStockTradingData(StockNo);
        stockDataPtr->SetAccountNo(FAccountNo, UFC::rwNotLock);
        if (FStockNoIndex.Add(StockNo, stockDataPtr) == FALSE)
        {
            delete stockDataPtr;
            stockDataPtr = 0;
        }
    }
    
    Unlock(LockType);
    return stockDataPtr;
}  //CRMInvestorAccount::FindOrCreateStockTradingData()
//------------------------------------------------------------------------------
void CRMInvestorAccount::DumpStockTradingData(UFC::BufferedLog* LogPtr, UFC::RWLockTypeEnum LockType)
{
    int dumpCount = 0;
    Lock(LockType);
    CRMStockTradingData* curStockTradingPtr = FStockNoIndex.First();
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
}  //CRMInvestorAccount::DumpStockTradingData()
//----------------------------------------------------------------------------------------------------------------------
int CRMInvestorAccount::GetStockWhiteListCount(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    int stockCount = FStockWhiteListSet.ItemCount();
    Unlock(LockType);
    return stockCount;
}  //CRMInvestorAccount::GetStockWhiteListCount()
//----------------------------------------------------------------------------------------------------------------------
bool CRMInvestorAccount::IsStockInWhiteList(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum LockType)
{
    bool isExist = false;
    if (StockNo.Length() > 0)
    {
        Lock(LockType);
        if (FStockWhiteListSet.Exists(StockNo) == TRUE) isExist = true;
        Unlock(LockType);
    }
    return isExist;
}  //CRMInvestorAccount::IsStockInWhiteList()
//----------------------------------------------------------------------------------------------------------------------
void CRMInvestorAccount::AddStockToWhiteList(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum LockType)
{
    if (StockNo.Length() <= 0) return;
    Lock(LockType);
    FStockWhiteListSet.Add(StockNo);
    Unlock(LockType);
}  //CRMInvestorAccount::AddStockToWhiteList()
//------------------------------------------------------------------------------
void CRMInvestorAccount::ClearStockWhiteList(UFC::RWLockTypeEnum LockType)
{
    Lock(LockType);
    FStockWhiteListSet.Clear();
    Unlock(LockType);
}  //CRMInvestorAccount::ClearStockWhiteList()
//------------------------------------------------------------------------------
UFC::AnsiString CRMInvestorAccount::GetStockWhiteListCSV(UFC::RWLockTypeEnum LockType)
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
}  //CRMInvestorAccount::GetStockWhiteListCSV()
//------------------------------------------------------------------------------
bool CRMInvestorAccount::CheckOrderAmount(const UFC::AnsiString& OrderID, char BuySellCode, double OrderAmount, UFC::AnsiString& CheckMessage, UFC::RWLockTypeEnum LockType)
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
}  //CRMInvestorAccount::CheckOrderAmount()
//------------------------------------------------------------------------------
UFC::AnsiString CRMInvestorAccount::ToString(UFC::RWLockTypeEnum LockType)
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
}  //CRMInvestorAccount::ToString()
}  //namespace twserm
