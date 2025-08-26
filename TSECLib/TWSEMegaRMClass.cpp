/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "TWSEMegaRMClass.h"
#include "TWSEMegaRMDataPool.h"

#include "TMessages.h"
#include "PMessages.h"
#include "OMessages.h"
#include "O1Messages.h"

namespace twserm
{
//--------------------------- CMegaRiskManagerClass ----------------------------
//------------------------------------------------------------------------------
CMegaRiskManagerClass::CMegaRiskManagerClass(bool IsRecoverMode)
:CTWSERiskManagerClass(IsRecoverMode)
,FWhiteListFileSecond(0LL)
{
    UFC::BufferedLog::Printf(" CMegaRiskManagerClass::%s()", __func__);
}  //CMegaRiskManagerClass::CMegaRiskManagerClass()
//------------------------------------------------------------------------------
CMegaRiskManagerClass::~CMegaRiskManagerClass()
{
}  //CMegaRiskManagerClass::~CMegaRiskManagerClass()
//------------------------------------------------------------------------------
void CMegaRiskManagerClass::Check(CheckData& Obj)
{
}  //CMegaRiskManagerClass::Check()
//---------------------------------------------------------------------------------------------------------------------
void CMegaRiskManagerClass::ParseRMDataFromUserData(const UFC::AnsiString& Key, UFC::AnsiString& WarrantNo, UFC::AnsiString& BrokerID)
{
    WarrantNo = "";
    BrokerID = "";
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
    if (riskManagerFieldList.ItemCount() >= 2) BrokerID = riskManagerFieldList[1];
}  //CMegaRiskManagerClass::ParseRMDataFromUserData()
//------------------------------------------------------------------------------
void CMegaRiskManagerClass::RecoverRMConfirmData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, Int32 NetworkID, bool IsSynchronize)
{
    UFC::AnsiString logHead = "";
    if (IsSynchronize)
        logHead = "SynchronizeRMConfirmData()";
    else
        logHead = "RecoverRMConfirmData()";
    FLogPtr->fprintf(" %s.", logHead.c_str());
    TWSE::MessageHeader *msgPtr = TWSE::MessageHeader::CreateMessage(ExecRptDataStr.c_str(), ExecRptDataStr.Length());
    int subSystem = msgPtr->GetSubSystem();
    int functionCode = msgPtr->GetFunctionCode();
    int msgStatusCode = msgPtr->GetStatusCode();    
    TWSE::TMessageType msgType = msgPtr->MessageType();
    UFC::AnsiString funcStr = "";
    if (functionCode == 1)
        funcStr = "Buy";
    else if (functionCode == 2)
        funcStr = "Sell";
    else if (functionCode == 3)
        funcStr = "Reduce";
    else if (functionCode == 4)
        funcStr = "Cancel";
    else if (functionCode == 5)
        funcStr = "Query";
    else if (functionCode == 6)
        funcStr = "Replace Px";
    else
        funcStr = "Unknown";
        
    if (((msgType != TWSE::mtT020) && (msgType != TWSE::mtO020) && (msgType != TWSE::mtO120) && (msgType != TWSE::mtP020)) ||
        ((functionCode != 1) && (functionCode != 2) && (functionCode != 3) && (functionCode != 4) && (functionCode != 6)))
    {
        FLogPtr->fprintf(" %s Ignore %s Order[%s], Func[%d], Status[%d], MsgType[%d].", logHead.c_str(),
                         funcStr.c_str(), ExecRptDataStr.c_str(), functionCode, msgStatusCode, msgType);
        delete msgPtr;
        return;
    }
    
    if (((functionCode == 1) || (functionCode == 2)) && 
        ((msgStatusCode != 0) && (msgStatusCode != 31) && (msgStatusCode != 51)))
    {
        FLogPtr->fprintf(" %s Ignore %s Order[%s], Func[%d], Status[%d], MsgType[%d].", logHead.c_str(),
                         funcStr.c_str(), ExecRptDataStr.c_str(), functionCode, msgStatusCode, msgType);
        delete msgPtr;
        return;
    }
    
    if (((functionCode == 3) && ((msgStatusCode != 0) && (msgStatusCode != 31) && (msgStatusCode != 32))) ||  //Reduce
        ((functionCode == 4) && (msgStatusCode != 0)))  //Cancel
    {
        FLogPtr->fprintf(" %s Ignore %s Order[%s], Func[%d], Status[%d], MsgType[%d].", logHead.c_str(),
                         funcStr.c_str(), ExecRptDataStr.c_str(), functionCode, msgStatusCode, msgType);
        delete msgPtr;
        return;
    }
    
    UFC::AnsiString orderID, accountNo, stockNo, orderQtyStr, beforeQtyStr, afterQtyStr, orderPriceStr;
    char buySellCode = ' ', exchangeCode = ' ', twseOrderType = ' ', twsePriceType = ' ', twseTimeInForce = ' ';
    TWSE::TMessageType orderMsgType = TWSE::mtUnknown;
    nsOrderMessageDefine::TradingSessionIDEnum tradingSession = nsOrderMessageDefine::tsAuto;
    double orderPrice = 0.0;
    if (msgType == TWSE::mtT020)
    {
        orderMsgType = TWSE::mtT010;
        tradingSession = nsOrderMessageDefine::tsNormal;
        TWSE::MessageT020 *t020MsgPtr = static_cast<TWSE::MessageT020*>(msgPtr);
        orderID = UFC::AnsiString(t020MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(t020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(t020MsgPtr->StockID, 6);
        buySellCode = t020MsgPtr->BuySellCode;
        exchangeCode = t020MsgPtr->ExchangeCode;
        twseOrderType = t020MsgPtr->TWSEOrderType;
        orderQtyStr = UFC::AnsiString(t020MsgPtr->OrderQty, 6);
        beforeQtyStr = UFC::AnsiString(t020MsgPtr->BeforeQty, 6);
        afterQtyStr = UFC::AnsiString(t020MsgPtr->AfterQty, 6);
        orderPriceStr = UFC::AnsiString(t020MsgPtr->OrderPrice, 9);
        orderPrice = Parse95v9999PriceStr(orderPriceStr);
        twsePriceType = t020MsgPtr->OrderType;      //'1':Market '2':Limit
        twseTimeInForce = t020MsgPtr->TimeInForce;  //'0':ROD '3':IOC '4':FOK
    }
    else if (msgType == TWSE::mtO120)  //IntraDay Odd
    {
        orderMsgType = TWSE::mtO110;
        tradingSession = nsOrderMessageDefine::tsIntradayOdd;
        TWSE::MessageO120 *o120MsgPtr = static_cast<TWSE::MessageO120*>(msgPtr);
        orderID = UFC::AnsiString(o120MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(o120MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(o120MsgPtr->StockID, 6);
        buySellCode = o120MsgPtr->BuySellCode;
        exchangeCode = '2';     //Odd
        twseOrderType = '0';
        twsePriceType = '2';    //Limit
        twseTimeInForce = '0';  //ROD
        orderQtyStr = UFC::AnsiString(o120MsgPtr->OrderQty, 6);
        beforeQtyStr = UFC::AnsiString(o120MsgPtr->BeforeQty, 6);
        afterQtyStr = UFC::AnsiString(o120MsgPtr->AfterQty, 6);
        orderPriceStr = UFC::AnsiString(o120MsgPtr->OrderPrice, 9);
        orderPrice = Parse95v9999PriceStr(orderPriceStr);
    }
    else if (msgType == TWSE::mtO020)  //Odd
    {
        orderMsgType = TWSE::mtO010;
        tradingSession = nsOrderMessageDefine::tsOddLot;
        TWSE::MessageO020 *o020MsgPtr = static_cast<TWSE::MessageO020*>(msgPtr);
        orderID = UFC::AnsiString(o020MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(o020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(o020MsgPtr->StockID, 6);
        buySellCode = o020MsgPtr->BuySellCode;
        exchangeCode = '2';     //Odd
        twseOrderType = '0';
        twsePriceType = '2';    //Limit
        twseTimeInForce = '0';  //ROD
        orderQtyStr = UFC::AnsiString(o020MsgPtr->OrderQty, 6);
        beforeQtyStr = UFC::AnsiString(o020MsgPtr->BeforeQty, 6);
        afterQtyStr = UFC::AnsiString(o020MsgPtr->AfterQty, 6);
        orderPriceStr = UFC::AnsiString(o020MsgPtr->OrderPrice, 9);
        orderPrice = Parse95v9999PriceStr(orderPriceStr);
    }
    else if (msgType == TWSE::mtP020)  //OffHour
    {
        orderMsgType = TWSE::mtP010;
        tradingSession = nsOrderMessageDefine::tsOffHour;
        TWSE::MessageP020 *p020MsgPtr = static_cast<TWSE::MessageP020*>(msgPtr);
        orderID = UFC::AnsiString(p020MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(p020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(p020MsgPtr->StockID, 6);
        buySellCode = p020MsgPtr->BuySellCode;
        exchangeCode = p020MsgPtr->ExchangeCode;
        twseOrderType = p020MsgPtr->OrderType;
        twsePriceType = '2';    //Limit
        twseTimeInForce = '0';  //ROD
        orderQtyStr = UFC::AnsiString(p020MsgPtr->OrderQty, 6);
        beforeQtyStr = UFC::AnsiString(p020MsgPtr->BeforeQty, 6);
        afterQtyStr = UFC::AnsiString(p020MsgPtr->AfterQty, 6);
        orderPriceStr = UFC::AnsiString(p020MsgPtr->OrderPrice, 9);
        orderPrice = Parse95v9999PriceStr(orderPriceStr);
    }
    delete msgPtr;
    accountNo.TrimRight(' ');
    stockNo.TrimRight(' ');
    SecurityInfo* stockPtr = CRiskManagerDataPool::FindStock(stockNo);
    
    int tradeUnit = 1000;
    double amountPrice = orderPrice;
    if (stockPtr != 0) 
    {
        tradeUnit = stockPtr->GetTradeUnit();
        
        if (twsePriceType == '1')  //Market
        {
            double bullPrice = 0.0, bearPrice = 0.0;
            stockPtr->GetSessionPriceLimit(tradingSession, bullPrice, bearPrice);
            if (buySellCode == 'B')
                amountPrice = bullPrice;
            else
                amountPrice = bearPrice;
            FLogPtr->fprintf(" %s Set Market Order[%s] Stock[%s] Px[%15.4lf] AmtPx[%15.4lf].", logHead.c_str(),
                             orderID.c_str(), stockNo.c_str(), orderPrice, amountPrice);
        }  //if (twsePriceType == '1')
    }
    else
        FLogPtr->fprintf(" %s Order[%s] Stock[%s] not Found.", logHead.c_str(), orderID.c_str(), stockNo.c_str());

    int origMsgOrderQty = orderQtyStr.ToInt();
    int origMsgBeforeQty = beforeQtyStr.ToInt();
    int origMsgAfterQty = afterQtyStr.ToInt();
    int orderQty = origMsgOrderQty;
    int beforeQty = origMsgBeforeQty;
    int afterQty = origMsgAfterQty;
    if (exchangeCode != '2') 
    {
        orderQty = origMsgOrderQty * static_cast<double>(tradeUnit);
        beforeQty = origMsgBeforeQty * static_cast<double>(tradeUnit);
        afterQty = origMsgAfterQty * static_cast<double>(tradeUnit);
    }
    
    int differentQty = beforeQty - afterQty;
    if (((functionCode == 1) || (functionCode == 2)) &&
        ((msgStatusCode == 31) && (msgStatusCode == 51)))
    {
        origMsgOrderQty = origMsgAfterQty;
        orderQty = afterQty;
        FLogPtr->fprintf(" %s Set OrderQty from [%d] to [%d].", logHead.c_str(), orderQty, afterQty);
    }
    double orderAmount = amountPrice * static_cast<double>(orderQty);
    double differentAmount = amountPrice * static_cast<double>(differentQty);
    
    FLogPtr->fprintf(" %s Order[%s] Func[%d] Stock[%s][%s][%s]%c %c Qty[%d] Px[%11.4lf] AmtPx[%11.4lf] bQty[%d] aQty[%d] execCode[%c] Acc[%s] func[%d] status[%d] SubSys[%d] msgType[%d] NID[%ld] PxType[%c] TIF[%c] TradeUnit[%d].", logHead.c_str(),
                     orderID.c_str(), functionCode, stockNo.c_str(), WarrantNo.c_str(), BrokerID.c_str(), twseOrderType, buySellCode, orderQty, orderPrice, amountPrice, beforeQty, afterQty, exchangeCode, accountNo.c_str(), functionCode, msgStatusCode, subSystem, msgType, (long)NetworkID, twsePriceType, twseTimeInForce, tradeUnit);
    
    CRMOrderSummary *targetOrderPtr = gMegaRMDataPoolPtr->FindOrderFromIDIndex(orderID, UFC::rwLockForRead);
    if ((functionCode == 1) || (functionCode == 2))
    {
        if (targetOrderPtr == 0)
        {
            CRMOrderSummary *newOrderPtr = new CRMOrderSummary(orderID, buySellCode, stockNo, WarrantNo, BrokerID, twseOrderType, orderPrice, orderQty, exchangeCode, orderMsgType, accountNo);
            newOrderPtr->SetOrigOrderQty(origMsgOrderQty, UFC::rwNotLock);
            newOrderPtr->SetTradeUnit(tradeUnit, UFC::rwNotLock);
            newOrderPtr->SetAmountPrice(amountPrice, UFC::rwNotLock);
            newOrderPtr->SetOrderAmount(orderAmount, UFC::rwNotLock);
            
            if (gMegaRMDataPoolPtr->InsertOrderToIDIndex(orderID, newOrderPtr, UFC::cstCheck, UFC::rwLockForWrite))
                targetOrderPtr = newOrderPtr;
            else
            {
                FLogPtr->fprintf(" %s Insert Order[%s] into Object Manager failed.", logHead.c_str(), orderID.c_str());
                delete newOrderPtr;
            }
        }
        else
        {
            targetOrderPtr = 0;
            FLogPtr->fprintf(" %s Duplicate Order[%s].", logHead.c_str(), orderID.c_str());
        }
    }
    else if ((functionCode == 3) || (functionCode == 4) || (functionCode == 6))  //Reduce or Cancel or Replace Price
    {
        if (targetOrderPtr == 0)
            FLogPtr->fprintf(" %s Order[%s] Not Found.", logHead.c_str(), orderID.c_str());
    }  //if ((functionCode == 1) || (functionCode == 2))
    
    if (targetOrderPtr != 0)
    {
        CRMInvestorAccount *accountPtr = gMegaRMDataPoolPtr->FindAccountFromNoIndex(accountNo, UFC::rwLockForRead);
        if (accountPtr != 0)
        {
            bool isNewStockData = false;
            CRMStockTradingData *stockDataPtr = accountPtr->FindOrCreateStockTradingData(stockNo, isNewStockData, UFC::rwLockForWrite);
            if (stockDataPtr != 0)
            {
                if (isNewStockData)
                    FLogPtr->fprintf(" %s Create Account[%s] Stock[%s].", logHead.c_str(), accountNo.c_str(), stockNo.c_str());
                
                bool isInsertOrderSuccess = false;
                if ((functionCode == 1) || (functionCode == 2))
                {
                    orderAmount = amountPrice * static_cast<double>(orderQty);
                    targetOrderPtr->SetOrderAmount(orderAmount, UFC::rwLockForWrite);
                    if (functionCode == 1)
                    {
                        if (msgType == TWSE::mtT020)
                            isInsertOrderSuccess = stockDataPtr->InsertRegularBuyOrder(orderID, targetOrderPtr, UFC::cstCheck, UFC::rwLockForWrite);
                        else if (msgType == TWSE::mtO120)
                            isInsertOrderSuccess = stockDataPtr->InsertIntraDayOddBuyOrder(orderID, targetOrderPtr, UFC::cstCheck, UFC::rwLockForWrite);
                        else
                            isInsertOrderSuccess = stockDataPtr->InsertBuyOrder(orderID, targetOrderPtr, UFC::cstCheck, UFC::rwLockForWrite);
                    }
                    else if (functionCode == 2)
                    {
                        if (msgType == TWSE::mtT020)
                            isInsertOrderSuccess = stockDataPtr->InsertRegularSellOrder(orderID, targetOrderPtr, UFC::cstCheck, UFC::rwLockForWrite);
                        else if (msgType == TWSE::mtO120)
                            isInsertOrderSuccess = stockDataPtr->InsertIntraDayOddSellOrder(orderID, targetOrderPtr, UFC::cstCheck, UFC::rwLockForWrite);
                        else
                            isInsertOrderSuccess = stockDataPtr->InsertSellOrder(orderID, targetOrderPtr, UFC::cstCheck, UFC::rwLockForWrite);
                    }
                    
                    if (isInsertOrderSuccess)
                    {
                        accountPtr->LockForWrite();
                        if (functionCode == 1)
                        {
                            double origBuyOrderAmount = accountPtr->GetBuyOrderAmount(UFC::rwNotLock);
                            double newBuyOrderAmount = origBuyOrderAmount + orderAmount;
                            accountPtr->SetBuyOrderAmount(newBuyOrderAmount, UFC::rwNotLock);
                            FLogPtr->fprintf(" %s Set Account[%s] BuyOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origBuyOrderAmount, newBuyOrderAmount);
                        }
                        else
                        {
                            double origSellOrderAmount = accountPtr->GetSellOrderAmount(UFC::rwNotLock);
                            double newSellOrderAmount = origSellOrderAmount + orderAmount;
                            accountPtr->SetSellOrderAmount(newSellOrderAmount, UFC::rwNotLock);
                            FLogPtr->fprintf(" %s Set Account[%s] SellOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origSellOrderAmount, newSellOrderAmount);
                        }
                        FLogPtr->fprintf(" %s %s.", logHead.c_str(), accountPtr->ToString(UFC::rwNotLock).c_str());
                        accountPtr->UnLockForWrite();
                        FLogPtr->fprintf(" %s StockData: %s, Order[%s], msgType[%d].", logHead.c_str(), stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderID.c_str(), msgType);
                    }
                    else
                        FLogPtr->fprintf(" %s Insert Order[%s] into Account[%s] Stock[%s] failed.", logHead.c_str(), orderID.c_str(), accountNo.c_str(), stockNo.c_str());
                }
                else if ((functionCode == 3) || (functionCode == 4))  //Reduce or Cancel
                {
                    targetOrderPtr->LockForWrite();
                    int origOrderQty = targetOrderPtr->GetOrderQty(UFC::rwNotLock);
                    int origLeavesQty = targetOrderPtr->GetLeavesOrderQty(UFC::rwNotLock);
                    int newLeavesQty = origLeavesQty - differentQty;
                    targetOrderPtr->SetLeavesQty(newLeavesQty, UFC::rwNotLock);
                    double origOrderAmount = targetOrderPtr->GetOrderAmount(UFC::rwNotLock); 
                    double newOrderAmount = 0.0;
                    if (origOrderAmount >= differentAmount) newOrderAmount = origOrderAmount - differentAmount;
                    targetOrderPtr->SetOrderAmount(newOrderAmount, UFC::rwNotLock);
                    targetOrderPtr->UnLockForWrite();
                    
                    if (newLeavesQty <= 0)
                    {
                        CRMOrderSummary *cancelOrderPtr = 0;
                        if (buySellCode == 'B')
                        {
                            if (msgType == TWSE::mtT020)
                                cancelOrderPtr = stockDataPtr->RemoveRegularBuyOrder(orderID, UFC::rwLockForWrite);
                            else if (msgType == TWSE::mtO120)
                                cancelOrderPtr = stockDataPtr->RemoveIntraDayOddBuyOrder(orderID, UFC::rwLockForWrite);
                            else
                                cancelOrderPtr = stockDataPtr->RemoveBuyOrder(orderID, UFC::rwLockForWrite);
                        }
                        else if (buySellCode == 'S')
                        {
                            if (msgType == TWSE::mtT020)
                                cancelOrderPtr = stockDataPtr->RemoveRegularSellOrder(orderID, UFC::rwLockForWrite);
                            else if (msgType == TWSE::mtO120)
                                cancelOrderPtr = stockDataPtr->RemoveIntraDayOddSellOrder(orderID, UFC::rwLockForWrite);
                            else
                                cancelOrderPtr = stockDataPtr->RemoveSellOrder(orderID, UFC::rwLockForWrite);
                        }
                                    
                        if (cancelOrderPtr != 0)
                            FLogPtr->fprintf(" %s StockData: %s, Order[%s], msgType[%d], bQty[%d], aQty[%d].", logHead.c_str(), stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderID.c_str(), msgType, beforeQty, afterQty);
                        else
                            FLogPtr->fprintf(" %s Remove Order[%s] from Account[%s] Stock[%s] failed.", logHead.c_str(), orderID.c_str(), accountNo.c_str(), stockNo.c_str());
                    }  //if (afterQty <= 0)
                    
                    if (functionCode == 3)  //Reduce
                    {
                        int newOrderQty = origOrderQty - differentQty;
                        if (newOrderQty < 0) newOrderQty = 0;
                        double newOrderAmount = amountPrice * afterQty;
                        targetOrderPtr->LockForWrite();
                        double origOrderAmount = targetOrderPtr->GetOrderAmount(UFC::rwNotLock);
                        targetOrderPtr->SetOrderQty(newOrderQty, UFC::rwNotLock);
                        targetOrderPtr->SetOrderAmount(newOrderAmount, UFC::rwNotLock);
                        targetOrderPtr->UnLockForWrite();
                        FLogPtr->fprintf(" %s Reduce Order[%s] Qty from [%d] to [%d], LeavesQty from [%d] to [%d], Amount from [%15.4lf] to [%15.4lf].", logHead.c_str(),
                                         orderID.c_str(), origOrderQty, newOrderQty, origLeavesQty, newLeavesQty, origOrderAmount, newOrderAmount);
                    }  //if (functionCode == 3)
                    
                    accountPtr->LockForWrite();
                    if (buySellCode == 'B')
                    {
                        double origBuyOrderAmount = accountPtr->GetBuyOrderAmount(UFC::rwNotLock);
                        double newBuyOrderAmount = 0.0;
                        if (origBuyOrderAmount >= differentAmount)
                            newBuyOrderAmount = origBuyOrderAmount - differentAmount;
                        else
                            FLogPtr->fprintf(" %s OrigBuyOrderAmount[%15.4lf] < DifferentAmount[%15.4lf], Set BuyOrderAmount to 0.0.", logHead.c_str(), origBuyOrderAmount, differentAmount);
                        accountPtr->SetBuyOrderAmount(newBuyOrderAmount, UFC::rwNotLock);
                        FLogPtr->fprintf(" %s Set Account[%s] BuyOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origBuyOrderAmount, newBuyOrderAmount);
                    }
                    else
                    {
                        double origSellOrderAmount = accountPtr->GetSellOrderAmount(UFC::rwNotLock);
                        double newSellOrderAmount = 0.0;
                        if (origSellOrderAmount >= differentAmount)
                            newSellOrderAmount = origSellOrderAmount - differentAmount;
                        else
                            FLogPtr->fprintf(" %s OrigSellOrderAmount[%15.4lf] < DifferentAmount[%15.4lf], Set SellOrderAmount to 0.0.", logHead.c_str(), origSellOrderAmount, differentAmount);
                        accountPtr->SetSellOrderAmount(newSellOrderAmount, UFC::rwNotLock);
                        FLogPtr->fprintf(" %s Set Account[%s] SellOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origSellOrderAmount, newSellOrderAmount);
                    }
                    FLogPtr->fprintf(" %s %s.", logHead.c_str(), accountPtr->ToString(UFC::rwNotLock).c_str());
                    accountPtr->UnLockForWrite();
                    FLogPtr->fprintf(" %s Set Order[%s] LeavesQty from %d to %d, Amount from %15.4lf to %15.4lf.", logHead.c_str(),
                                     orderID.c_str(), origLeavesQty, newLeavesQty, origOrderAmount, newOrderAmount);
                }
                else if (functionCode == 6)
                {
                    targetOrderPtr->LockForWrite();
                    double origOrderPrice = targetOrderPtr->GetOrderPrice(UFC::rwNotLock);
                    double origOrderAmount = targetOrderPtr->GetOrderAmount(UFC::rwNotLock);
                    targetOrderPtr->SetOrderPrice(orderPrice, UFC::rwNotLock);
                    targetOrderPtr->SetAmountPrice(orderPrice, UFC::rwNotLock);
                    targetOrderPtr->SetOrderAmount(orderAmount, UFC::rwNotLock);
                    targetOrderPtr->UnLockForWrite();
                    FLogPtr->fprintf(" %s Set Order[%s] Price from [%10.4lf] to [%10.4lf], Amount from [%15.4lf] to [%15.4lf].", logHead.c_str(),
                                     orderID.c_str(), origOrderPrice, orderPrice, origOrderAmount, orderAmount);
                    
                    accountPtr->LockForWrite();
                    if (buySellCode == 'B')
                    {
                        double origBuyOrderAmount = accountPtr->GetBuyOrderAmount(UFC::rwNotLock);
                        double newBuyOrderAmount = origBuyOrderAmount - origOrderAmount + orderAmount;
                        if (newBuyOrderAmount < 0.0) newBuyOrderAmount = 0.0;
                        accountPtr->SetBuyOrderAmount(newBuyOrderAmount, UFC::rwNotLock);
                        FLogPtr->fprintf(" %s Set Account[%s] BuyOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origBuyOrderAmount, newBuyOrderAmount);
                    }
                    else
                    {
                        double origSellOrderAmount = accountPtr->GetSellOrderAmount(UFC::rwNotLock);
                        double newSellOrderAmount = origSellOrderAmount - origOrderAmount + orderAmount;
                        if (newSellOrderAmount < 0.0) newSellOrderAmount = 0.0;
                        accountPtr->SetSellOrderAmount(newSellOrderAmount, UFC::rwNotLock);
                        FLogPtr->fprintf(" %s Set Account[%s] SellOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origSellOrderAmount, newSellOrderAmount);
                    }
                    FLogPtr->fprintf(" %s %s.", logHead.c_str(), accountPtr->ToString(UFC::rwNotLock).c_str());
                    accountPtr->UnLockForWrite();
                    
                    if (buySellCode == 'B')
                    {
                        if (msgType == TWSE::mtT020)
                            stockDataPtr->UpdateMaxRegularBuyOrderPrice(UFC::rwLockForWrite);
                        else if (msgType == TWSE::mtO120)
                            stockDataPtr->UpdateMaxIntraDayOddBuyOrderPrice(UFC::rwLockForWrite);
                        else
                            stockDataPtr->UpdateMaxBuyOrderPrice(UFC::rwLockForWrite);
                    }
                    else if (buySellCode == 'S')
                    {
                        if (msgType == TWSE::mtT020)
                            stockDataPtr->UpdateMinRegularSellOrderPrice(UFC::rwLockForWrite);
                        else if (msgType == TWSE::mtO120)
                            stockDataPtr->UpdateMinIntraDayOddSellOrderPrice(UFC::rwLockForWrite);
                        else
                            stockDataPtr->UpdateMinSellOrderPrice(UFC::rwLockForWrite);
                    }
                    FLogPtr->fprintf(" %s StockData: %s, Order[%s], msgType[%d], bQty[%d], aQty[%d].", logHead.c_str(), stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderID.c_str(), msgType, beforeQty, afterQty);
                }  //if ((functionCode == 1) || (functionCode == 2))
                
                CRMBrokerStockCondition *conditionPtr = stockDataPtr->FindCondition(WarrantNo, BrokerID, twseOrderType, UFC::rwLockForRead);
                bool isSellShort = BrokerID != CMegaRiskManagerDataPool::GetRMMainBrokerID();  //是否為借券
                if ((conditionPtr == 0) && !isSellShort)  //非借券
                {
                    UFC::AnsiString todayStr;
                    UFC::GetYYYYMMDD(todayStr, FALSE);
                    conditionPtr = new CRMBrokerStockCondition(WarrantNo, BrokerID, twseOrderType, 0, 0, todayStr, isSellShort);
                    conditionPtr->SetAccountNo(accountNo, UFC::rwNotLock);
                    conditionPtr->SetStockNo(stockNo, UFC::rwNotLock);
                    if (!stockDataPtr->InsertCondition(WarrantNo, BrokerID, twseOrderType, conditionPtr, UFC::cstNotCheck, UFC::rwLockForWrite))
                    {
                        FLogPtr->fprintf(" %s Insert Account[%s] Stock[%s][%s][%s]%c condition failed.", logHead.c_str(), accountNo.c_str(), orderID.c_str(), stockNo.c_str(), WarrantNo.c_str(), BrokerID.c_str(), twseOrderType);
                        delete conditionPtr;
                        conditionPtr = 0;
                    }
                }  //if (!isSellShort)
                
                if (conditionPtr != 0)
                {
                    if (functionCode == 1)
                    {
                        if (msgType == TWSE::mtT020)
                            conditionPtr->IncreaseRegularOrderBuyQty(orderQty, UFC::rwLockForWrite);
                        else if (msgType == TWSE::mtO120)
                            conditionPtr->IncreaseIntraDayOddOrderBuyQty(orderQty, UFC::rwLockForWrite);
                        else
                            conditionPtr->IncreaseOrderBuyQty(orderQty, UFC::rwLockForWrite);
                    }
                    else if (functionCode == 2)
                    {
                        if (msgType == TWSE::mtT020)
                            conditionPtr->IncreaseRegularOrderSellQty(orderQty, UFC::rwLockForWrite);
                        else if (msgType == TWSE::mtO120)
                            conditionPtr->IncreaseIntraDayOddOrderSellQty(orderQty, UFC::rwLockForWrite);
                        else
                            conditionPtr->IncreaseOrderSellQty(orderQty, UFC::rwLockForWrite);
                    }
                    else if ((functionCode == 3) || (functionCode == 4))
                    {
                        if (buySellCode == 'B')
                        {
                            if (msgType == TWSE::mtT020)
                            {
                                int normalOrderBuyQty = conditionPtr->GetRegularOrderBuyQty(UFC::rwLockForRead);
                                if (normalOrderBuyQty >= differentQty)
                                    conditionPtr->IncreaseRegularOrderBuyQty(-differentQty, UFC::rwLockForWrite);
                                else
                                {
                                    conditionPtr->SetRegularOrderBuyQty(0, UFC::rwLockForWrite);
                                    FLogPtr->fprintf(" %s RegularOrderBuyQty[%d] is Less then deleteQty[%d], set NormalOrderBuyQty to 0.", logHead.c_str(), normalOrderBuyQty, differentQty);
                                }
                            }
                            else if (msgType == TWSE::mtO120)
                            {
                                int normalOrderBuyQty = conditionPtr->GetIntraDayOddOrderBuyQty(UFC::rwLockForRead);
                                if (normalOrderBuyQty >= differentQty)
                                    conditionPtr->IncreaseIntraDayOddOrderBuyQty(-differentQty, UFC::rwLockForWrite);
                                else
                                {
                                    conditionPtr->SetRegularOrderBuyQty(0, UFC::rwLockForWrite);
                                    FLogPtr->fprintf(" %s IntrrDayOddOrderBuyQty[%d] is Less then deleteQty[%d], set NormalOrderBuyQty to 0.", logHead.c_str(), normalOrderBuyQty, differentQty);
                                }
                            }
                            else
                            {
                                int orderBuyQty = conditionPtr->GetOrderBuyQty(UFC::rwLockForRead);
                                if (orderBuyQty >= differentQty)
                                    conditionPtr->IncreaseOrderBuyQty(-differentQty, UFC::rwLockForWrite);
                                else
                                {
                                    conditionPtr->SetOrderBuyQty(0, UFC::rwLockForWrite);
                                    FLogPtr->fprintf(" %s OrderBuyQty[%d] is Less then deleteQty[%d], set OrderBuyQty to 0.", logHead.c_str(), orderBuyQty, differentQty);
                                }
                            }  //if (msgType == mtT020)
                        }
                        else
                        {
                            if (msgType == TWSE::mtT020)
                            {
                                int normalOrderSellQty = conditionPtr->GetRegularOrderSellQty(UFC::rwLockForRead);
                                if (normalOrderSellQty >= differentQty)
                                    conditionPtr->IncreaseRegularOrderSellQty(-differentQty, UFC::rwLockForWrite);
                                else
                                {
                                    conditionPtr->SetRegularOrderSellQty(0, UFC::rwLockForWrite);
                                    FLogPtr->fprintf(" %s RegularOrderSellQty[%d] is Less then deleteQty[%d], set NormalOrderSellQty to 0.", logHead.c_str(), normalOrderSellQty, differentQty);
                                }
                            }
                            else if (msgType == TWSE::mtO120)
                            {
                                int normalOrderSellQty = conditionPtr->GetIntraDayOddOrderSellQty(UFC::rwLockForRead);
                                if (normalOrderSellQty >= differentQty)
                                    conditionPtr->IncreaseIntraDayOddOrderSellQty(-differentQty, UFC::rwLockForWrite);
                                else
                                {
                                    conditionPtr->SetRegularOrderSellQty(0, UFC::rwLockForWrite);
                                    FLogPtr->fprintf(" %s IntraDayOddOrderSellQty[%d] is Less then deleteQty[%d], set NormalOrderSellQty to 0.", logHead.c_str(), normalOrderSellQty, differentQty);
                                }
                            }
                            else
                            {
                                int orderSellQty = conditionPtr->GetOrderSellQty(UFC::rwLockForRead);
                                if (orderSellQty >= differentQty)
                                    conditionPtr->IncreaseOrderSellQty(-differentQty, UFC::rwLockForWrite);
                                else
                                {
                                    conditionPtr->SetOrderSellQty(0, UFC::rwLockForWrite);
                                    FLogPtr->fprintf(" %s OrderSellQty[%d] is Less then deleteQty[%d], set OrderSellQty to 0.", logHead.c_str(), orderSellQty, differentQty);
                                }
                            }  //if (msgType == mtT020)
                        }  //if (buySellCode == 'B')
                    }  //if (functionCode == 1)
                    FLogPtr->fprintf(" %s Condition: %s, Order[%s], msgType[%d], bQty[%d], aQty[%d].", logHead.c_str(), conditionPtr->ToString(UFC::rwLockForRead).c_str(), orderID.c_str(), msgType, beforeQty, afterQty);
                }
                else
                    FLogPtr->fprintf(" %s Account[%s] Stock[%s][%s][%s]%c not Exist, Order[%s].", logHead.c_str(), accountNo.c_str(), stockNo.c_str(), WarrantNo.c_str(), BrokerID.c_str(), twseOrderType, orderID.c_str());
            }
            else
                FLogPtr->fprintf(" %s Wrong Account[%s] Stock[%s], Order[%s].", logHead.c_str(), accountNo.c_str(), stockNo.c_str(), orderID.c_str());
        }
        else
            FLogPtr->fprintf(" %s Account[%s] not Found, Order[%s].", logHead.c_str(), accountNo.c_str(), orderID.c_str());
    }  //if (targetOrderPtr != 0)
}  //CMegaRiskManagerClass::RecoverRMConfirmData()
//------------------------------------------------------------------------------
void CMegaRiskManagerClass::RecoverRMFillData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, bool IsSynchronize)
{
    UFC::AnsiString logHead = "";
    if (IsSynchronize)
        logHead = "SynchronizeRMFillData()";
    else
        logHead = "RecoverRMFillData()";
    FLogPtr->fprintf(" %s.", logHead.c_str());
    int r3Offset = 0;
    TWSE::R3Body *r3BodyPtr = new TWSE::R3Body(ExecRptDataStr.c_str(), r3Offset);
    UFC::AnsiString orderID(r3BodyPtr->FOrderNo, 5);
                
    UFC::AnsiString accountNo(r3BodyPtr->FInvestorACNo, 7);
    accountNo.TrimRight(' ');
                
    UFC::AnsiString stockNo(r3BodyPtr->FStockID, 6);
    stockNo.TrimRight(' ');
    SecurityInfo* stockPtr = CRiskManagerDataPool::FindStock(stockNo);

    char buySellCode = r3BodyPtr->FBuySellCode;
    char exchangeCode = r3BodyPtr->FEXCD;
    
    int matchQty = r3BodyPtr->GetQty();
    int tradeUnit = 1000;
    if (stockPtr != 0) tradeUnit = stockPtr->GetTradeUnit();
    if (exchangeCode != '2') matchQty *= tradeUnit;            
    
    double matchPrice = r3BodyPtr->GetPrice();
    double matchAmount = matchPrice * matchQty;

    int matchRecNo = r3BodyPtr->GetRecNo();
    int matchSeqNo = r3BodyPtr->GetSeq();
    int matchTime = r3BodyPtr->ProcessingTime();
    char side = r3BodyPtr->FBuySellCode;
    ///< Use line broker ID + report sequence + match time as unique key.
    UFC::AnsiString execKey;
//    execKey.Printf( "%s_%08d_%08d%c", BrokerID.c_str(), matchTime, matchSeqNo, side );
    if (side == 'B')
        execKey.Printf( "%s_%s_1%08d", orderID.c_str(), stockNo.c_str(), matchRecNo );
    else if (side == 'S')
        execKey.Printf( "%s_%s_2%08d", orderID.c_str(), stockNo.c_str(), matchRecNo );
    else
        execKey.Printf( "%s_%s_%c%08d", orderID.c_str(), stockNo.c_str(), side, matchRecNo );
FLogPtr->fprintf( " %s Execution Key:%s.", logHead.c_str(), execKey.c_str() );
    delete r3BodyPtr;
    
    if (IsFillRptExecExist(execKey, UFC::rwLockForRead))
    {
        FLogPtr->fprintf(" %s [%s] has Processed. skip it.", logHead.c_str(), execKey.c_str());
        return;
    }
    else
        AddFillRptExec(execKey, UFC::rwLockForWrite);
    
    CRMOrderSummary *orderPtr = gMegaRMDataPoolPtr->FindOrderFromIDIndex(orderID, UFC::rwLockForRead);
    if (orderPtr != 0)
    {
        orderPtr->LockForWrite();
        UFC::AnsiString orderWarrantNo = WarrantNo;
        if (WarrantNo.Length() <= 0) orderWarrantNo = orderPtr->GetWarrantNo(UFC::rwNotLock);
        UFC::AnsiString orderBrokerID = BrokerID;
        if (orderBrokerID.Length() <= 0) orderBrokerID = orderPtr->GetBrokerID(UFC::rwNotLock);
        char twseOrderType = orderPtr->GetTWSEOrderType(UFC::rwNotLock);
        TWSE::TMessageType msgType = orderPtr->GetMsgType(UFC::rwNotLock);
        FLogPtr->fprintf(" %s Order[%s] Stock[%s][%s][%s]%c %c MatchQty[%d] MatchPx[%10.4lf] ExecCode[%c] msgType[%d] Account[%s].", logHead.c_str(),
                         orderID.c_str(), stockNo.c_str(), orderWarrantNo.c_str(), orderBrokerID.c_str(), twseOrderType, buySellCode, matchQty, matchPrice, exchangeCode, msgType, accountNo.c_str());
                
        int origLeavesOrderQty = orderPtr->GetLeavesOrderQty(UFC::rwNotLock);
        int origCumQty = orderPtr->GetCumQty(UFC::rwNotLock);
        double origCumAmount = orderPtr->GetCumAmount(UFC::rwNotLock);
        double amountPrice = orderPtr->GetAmountPrice(UFC::rwNotLock);
        if (origLeavesOrderQty > 0)
        {
            if (origLeavesOrderQty >= matchQty)
            {
                int newLeavesQty = origLeavesOrderQty - matchQty;
                orderPtr->SetLeavesQty(newLeavesQty, UFC::rwNotLock);
                double origOrderAmount = orderPtr->GetOrderAmount(UFC::rwNotLock);
                double newOrderAmount = amountPrice * newLeavesQty;
                orderPtr->SetOrderAmount(newOrderAmount, UFC::rwNotLock);
                
                int newCumQty = origCumQty + matchQty;
                orderPtr->SetCumQty(newCumQty, UFC::rwNotLock);
                double newCumAmount = origCumAmount + matchAmount;
                orderPtr->SetCumAmount(newCumAmount, UFC::rwNotLock);
                FLogPtr->fprintf(" %s Order[%s] Set LeavesQty from [%d] to [%d], CumQty from [%d] to [%d], OrdAmt from [%15.4lf] to [%15.4lf], MatchAmt from [%15.4lf] to [%15.4lf].", logHead.c_str(),
                                 orderID.c_str(), origLeavesOrderQty, newLeavesQty, origCumQty, newCumQty, origOrderAmount, newOrderAmount, origCumAmount, newCumAmount);
                int newLeavesOrderQty = orderPtr->GetLeavesOrderQty(UFC::rwNotLock);
                orderPtr->UnLockForWrite();
                
                CRMInvestorAccount *accountPtr = gMegaRMDataPoolPtr->FindAccountFromNoIndex(accountNo, UFC::rwLockForRead);
                if (accountPtr != 0)
                {
                    accountPtr->LockForWrite();
                    CRMStockTradingData *stockDataPtr = accountPtr->FindStockTradingData(stockNo, UFC::rwNotLock);
                    if (stockDataPtr != 0)
                    {
                        stockDataPtr->LockForWrite();
                        if (newLeavesOrderQty <= 0)
                        {
                            CRMOrderSummary *filledOrderPtr = 0;
                            if (buySellCode == 'B')
                            {
                                if (msgType == TWSE::mtT010)
                                    filledOrderPtr = stockDataPtr->RemoveRegularBuyOrder(orderID, UFC::rwNotLock);
                                else if (msgType == TWSE::mtO110)
                                    filledOrderPtr = stockDataPtr->RemoveIntraDayOddBuyOrder(orderID, UFC::rwNotLock);
                                else
                                    filledOrderPtr = stockDataPtr->RemoveBuyOrder(orderID, UFC::rwNotLock);                                    
                            }
                            else
                            {
                                if (msgType == TWSE::mtT010)
                                    filledOrderPtr = stockDataPtr->RemoveRegularSellOrder(orderID, UFC::rwNotLock);
                                else if (msgType == TWSE::mtO110)
                                    filledOrderPtr = stockDataPtr->RemoveIntraDayOddSellOrder(orderID, UFC::rwNotLock);
                                else
                                    filledOrderPtr = stockDataPtr->RemoveSellOrder(orderID, UFC::rwNotLock);
                            }
                            
                            if (filledOrderPtr != 0)
                                FLogPtr->fprintf(" %s StockData: %s, Order[%s].", logHead.c_str(), stockDataPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str());
                            else
                                FLogPtr->fprintf(" %s Remove Filled Order[%s] from account[%s] Stock[%s] failed.", logHead.c_str(), orderID.c_str(), accountNo.c_str(), stockNo.c_str());
                        }
                        
                        CRMBrokerStockCondition *conditionPtr = stockDataPtr->FindCondition(orderWarrantNo, orderBrokerID, twseOrderType, UFC::rwNotLock);
                        stockDataPtr->UnLockForWrite();
                        if (conditionPtr != 0)
                        {
                            conditionPtr->LockForWrite();
                            if (buySellCode == 'B')
                            {
                                if (msgType == TWSE::mtT010)
                                    conditionPtr->IncreaseRegularMatchBuyQty(matchQty, UFC::rwNotLock);
                                else if (msgType == TWSE::mtO110)
                                    conditionPtr->IncreaseIntraDayOddMatchBuyQty(matchQty, UFC::rwNotLock);
                                else
                                    conditionPtr->IncreaseMatchBuyQty(matchQty, UFC::rwNotLock);
                            }
                            else
                            {
                                if (msgType == TWSE::mtT010)
                                    conditionPtr->IncreaseRegularMatchSellQty(matchQty, UFC::rwNotLock);
                                else if (msgType == TWSE::mtO110)
                                    conditionPtr->IncreaseIntraDayOddMatchSellQty(matchQty, UFC::rwNotLock);
                                else
                                    conditionPtr->IncreaseMatchSellQty(matchQty, UFC::rwNotLock);
                            }
                            FLogPtr->fprintf(" %s Condition: %s, Order[%s].", logHead.c_str(), conditionPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str());
                            conditionPtr->UnLockForWrite();
                        }
                        else
                            FLogPtr->fprintf(" %s Account[%s] Stock[%s][%s][%s]%c not found, Order[%s].", logHead.c_str(), accountNo.c_str(), stockNo.c_str(), orderWarrantNo.c_str(), orderBrokerID.c_str(), twseOrderType, orderID.c_str());
                    }
                    else
                        FLogPtr->fprintf(" %s Account[%s] Stock[%s] data not Found, Order[%s].", logHead.c_str(), accountNo.c_str(), stockNo.c_str(), orderID.c_str());
                    
                    if (buySellCode == 'B')
                    {
                        double origBuyMatchAmount = accountPtr->GetBuyMatchAmount(UFC::rwNotLock);
                        double newBuyMatchAmount = origBuyMatchAmount + matchAmount;
                        accountPtr->SetBuyMatchAmount(newBuyMatchAmount, UFC::rwNotLock);

                        double origBuyOrderAmount = accountPtr->GetBuyOrderAmount(UFC::rwNotLock);
                        double newBuyOrderAmount = origBuyOrderAmount - origOrderAmount + newOrderAmount;
                        if (newBuyOrderAmount < 0.0)
                        {
                            newBuyOrderAmount = 0;
                            FLogPtr->fprintf(" %s not enough BuyOrderAmount[%15.4lf], Set BuyOrderAmount to 0.0.", logHead.c_str(), origBuyOrderAmount);
                        }
                        accountPtr->SetBuyOrderAmount(newBuyOrderAmount, UFC::rwNotLock);
                        FLogPtr->fprintf(" %s Set Account[%s] BuyMatchAmount from [%15.4lf] to [%15.4lf], BuyOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(),
                                         accountNo.c_str(), origBuyMatchAmount, newBuyMatchAmount, origBuyOrderAmount, newBuyOrderAmount);                        
                    }
                    else
                    {
                        double origSellMatchAmount = accountPtr->GetSellMatchAmount(UFC::rwNotLock);
                        double newSellMatchAmount = origSellMatchAmount + matchAmount;
                        accountPtr->SetSellMatchAmount(newSellMatchAmount, UFC::rwNotLock);
                        FLogPtr->fprintf(" %s Set Account[%s] SellMatchAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origSellMatchAmount, newSellMatchAmount);
                        
                        double origSellOrderAmount = accountPtr->GetSellOrderAmount(UFC::rwNotLock);
                        double newSellOrderAmount = origSellOrderAmount - origOrderAmount + newOrderAmount;
                        if (newSellOrderAmount < 0.0)
                        {
                            newSellOrderAmount = 0;
                            FLogPtr->fprintf(" %s not enough SellOrderAmount[%15.4lf], Set SellOrderAmount to 0.0.", logHead.c_str(), origSellOrderAmount);
                        }
                        accountPtr->SetSellOrderAmount(newSellOrderAmount, UFC::rwNotLock);
                        FLogPtr->fprintf(" %s Set Account[%s] SellMatchAmount from [%15.4lf] to [%15.4lf], SellOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(),
                                         accountNo.c_str(), origSellMatchAmount, newSellMatchAmount, origSellOrderAmount, newSellOrderAmount);                        
                    }  //if (buySellCode == 'B')
                    FLogPtr->fprintf(" %s %s.", logHead.c_str(), accountPtr->ToString(UFC::rwNotLock).c_str());
                    accountPtr->UnLockForWrite();
                }
                else
                    FLogPtr->fprintf(" %s Account[%s] not Found, Order[%s].", logHead.c_str(), accountNo.c_str(), orderID.c_str());
            }
            else
            {
                FLogPtr->fprintf(" %s Order[%s] LeavesQty[%d] < MatchQty[%d].", logHead.c_str(), orderID.c_str(), origLeavesOrderQty, matchQty);
                orderPtr->UnLockForWrite();
            }
        }
        else
        {
            FLogPtr->fprintf(" %s Order[%s] is Filled.", logHead.c_str(), orderID.c_str());
            orderPtr->UnLockForWrite();
        }
    }
    else
        FLogPtr->fprintf(" %s Order[%s] not Found.", logHead.c_str(), orderID.c_str());
}  //CMegaRiskManagerClass::RecoverRMFillData()
//------------------------------------------------------------------------------
void CMegaRiskManagerClass::ParseExecRtpLogStr(const UFC::AnsiString& ExecRptLogStr, bool IsSynchronize)
{
    UFC::AnsiString logHead = "CMegaRiskManagerClass::ParseExecRtpLogStr()";
    ExecutionParser *execParserPtr = 0;
    try 
    {
        execParserPtr = new ExecutionParser(ExecRptLogStr);
//        WriteRiskLog(logHead, "Parse Execution[%s]", ExecRptLogStr.c_str());
    }
    catch (UFC::Exception& e)
    {
        FLogPtr->fprintf(" %s Parse Execution[%s] Exception:%s", logHead.c_str(), ExecRptLogStr.c_str(), e.what());
        FLogPtr->Flush();
        return;
    }
    Int32 networkID = execParserPtr->GetNID();
    UFC::AnsiString userKey = execParserPtr->GetKey();
    UFC::AnsiString warrantNo = ""; 
    UFC::AnsiString brokerID = "";
    if (userKey.Length() > 0)
        ParseRMDataFromUserData(userKey, warrantNo, brokerID);
    else
        FLogPtr->fprintf(" %s Execution[%s] has no UserKey.", logHead.c_str(), ExecRptLogStr.c_str());
        
    UFC::AnsiString execRptData = execParserPtr->GetData();
    BOOL isConfirm = execParserPtr->IsConfirm();
    delete execParserPtr;
    
    if (isConfirm == TRUE)
    {
        bool isNetworkIDExists = false;
        FRMExecRptNIDLock.LockForWrite();
        if (FRMExecRptNIDSet.Exists(networkID) == TRUE)
        {
            FLogPtr->fprintf(" %s Ignore Duplicate Confirm Report NetworkID[%ld].", logHead.c_str(), networkID);
            isNetworkIDExists = true;
        }
        else
            FRMExecRptNIDSet.Add(networkID);
        FRMExecRptNIDLock.UnlockForWrite();
        
        if (!isNetworkIDExists) RecoverRMConfirmData(execRptData, warrantNo, brokerID, networkID, IsSynchronize);
    }
    else
        RecoverRMFillData(execRptData, warrantNo, brokerID, IsSynchronize);
    FLogPtr->Flush();
}  //CMegaRiskManagerClass::ParseExecRtpLogStr()
//------------------------------------------------------------------------------
void CMegaRiskManagerClass::RecoverRMData()
{
    UFC::AnsiString logHead = "CMegaRiskManagerClass::RecoverRMData()";
    UFC::AnsiString executionLogFileName = "";
    if (FIsTWSE == TRUE)
        executionLogFileName = "SpeedyTSE.Execution." + UFC::GetDateString() + ".DATA";
    else
        executionLogFileName = "SpeedyOTC.Execution." + UFC::GetDateString() + ".DATA";
    FILE *fileHandlePtr = fopen(executionLogFileName.c_str(), "r");
    if (fileHandlePtr == NULL)
    {
        FLogPtr->fprintf(" %s Can not open recover file %s", logHead.c_str(), executionLogFileName.c_str());
        return;
    }  //if (fileHandlePtr == NULL)
    
    char readBuffer[512];
    while (fgets(readBuffer, 512, fileHandlePtr) != NULL)
    {
        UFC::AnsiString curReadStr(readBuffer);
        ParseExecRtpLogStr(curReadStr, false);
    }  //while (fgets(readBuffer, 512, fileHandlePtr) != NULL)
    fclose(fileHandlePtr);    
}  //CMegaRiskManagerClass::RecoverRMData()
//---------------------------------------------------------------------------------------------------------------------
bool CMegaRiskManagerClass::CheckBuyOrder(CRMOrderSummary *OrderPtr, CRMInvestorAccount *AccountPtr, CRMStockTradingData *StockDataPtr, UFC::AnsiString& CheckMessage)
{
    UFC::AnsiString logHead = "CMegaRiskManagerClass::CheckBuyOrder()";
    bool isPassed = true;
    OrderPtr->LockForRead();
    TWSE::TMessageType msgType = OrderPtr->GetMsgType(UFC::rwNotLock);
    UFC::AnsiString orderID = OrderPtr->GetOrderID(UFC::rwNotLock);
    UFC::AnsiString stockNo = OrderPtr->GetStockNo(UFC::rwNotLock);
    UFC::AnsiString accountNo = OrderPtr->GetAccountNo(UFC::rwNotLock);
    UFC::AnsiString warrantNo = OrderPtr->GetWarrantNo(UFC::rwNotLock);
    UFC::AnsiString brokerID = OrderPtr->GetBrokerID(UFC::rwNotLock);
    char buySellCode = OrderPtr->GetSide(UFC::rwNotLock);
    char twsePriceType = OrderPtr->GetTWSEPriceType(UFC::rwNotLock);
    char twseOrderType = OrderPtr->GetTWSEOrderType(UFC::rwNotLock);
    int orderQty = OrderPtr->GetOrderQty(UFC::rwNotLock);
    int tradeUnit = OrderPtr->GetTradeUnit(UFC::rwNotLock);
    double orderPrice = OrderPtr->GetOrderPrice(UFC::rwNotLock);    
    double amountPrice = OrderPtr->GetAmountPrice(UFC::rwNotLock);
    double orderAmount = OrderPtr->GetOrderAmount(UFC::rwNotLock);
    nsOrderMessageDefine::TradingSessionIDEnum tradingSessionID = OrderPtr->GetSessionID(UFC::rwNotLock);
    OrderPtr->UnLockForRead();

    int availableSellQty = 0;  //可賣股數
    int availableBuyQty  = 0;  //可買股數
    bool isSellShort = false;  
    if ((msgType != TWSE::mtO010) && (msgType != TWSE::mtO110))  //零股沒有借券
        isSellShort = brokerID != CMegaRiskManagerDataPool::GetRMMainBrokerID();  //是否為借券
    CRMBrokerStockCondition *conditionPtr = StockDataPtr->FindCondition(warrantNo, brokerID, twseOrderType, UFC::rwNotLock);
    if (conditionPtr == 0)
    {
        if (!isSellShort)
        {
            FLogPtr->fprintf(" %s Sell Condition Account[%s] Stock[%s] WarrantNo[%s] BrokerID[%s] TWSEOrdType[%c] not Found.",
                             logHead.c_str(), accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerID.c_str(), twseOrderType);
            UFC::AnsiString todayStr;
            UFC::GetYYYYMMDD(todayStr, FALSE);
            conditionPtr = new CRMBrokerStockCondition(warrantNo, brokerID, twseOrderType, availableSellQty, availableBuyQty, todayStr, isSellShort);
            conditionPtr->SetAccountNo(accountNo, UFC::rwNotLock);
            conditionPtr->SetStockNo(stockNo, UFC::rwNotLock);
            if (!StockDataPtr->InsertCondition(warrantNo, brokerID, twseOrderType, conditionPtr, UFC::cstNotCheck, UFC::rwNotLock))
            {
                CheckMessage.Printf("Reject[%s]: Insert Account[%s] Stock[%s][%s][%s]%c condition failed", orderID.c_str(), accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerID.c_str(), twseOrderType);
                delete conditionPtr;
                conditionPtr = 0;
            }
        }
        else
            CheckMessage.Printf("Reject[%s]: Account[%s] Stock[%s][%s][%s]%c not found", orderID.c_str(), accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerID.c_str(), twseOrderType);
            
        if (conditionPtr == 0) 
        {
            FLogPtr->fprintf(" %s %s, MsgType[%d].", logHead.c_str(), CheckMessage.c_str(), msgType);
            FLogPtr->Flush();
            return false;
        }
    }  //if (conditionPtr == 0)
    
    conditionPtr->LockForWrite();
    availableSellQty = conditionPtr->GetAvailableSellQty(UFC::rwNotLock);                    //可賣股數
    availableBuyQty = conditionPtr->GetAvailableBuyQty(UFC::rwNotLock);                      //可買股數
//    isSellShort = conditionPtr->IsSellShort(UFC::rwNotLock);                                 //是否為借券
    int regularMatchBuyQty = conditionPtr->GetRegularMatchBuyQty(UFC::rwNotLock);            //今日日盤整股買進成交股數
    int regularMatchSellQty = conditionPtr->GetRegularMatchSellQty(UFC::rwNotLock);          //今日日盤整股賣出成交股數
    int intraDayOddMatchSellQty = conditionPtr->GetIntraDayOddMatchSellQty(UFC::rwNotLock);  //今日日盤整股賣出成交股數
    
    int matchBuyQty = conditionPtr->GetMatchBuyQty(UFC::rwNotLock);                          //今日成交買進股數
//    int matchSellQty = conditionPtr->GetMatchSellQty(UFC::rwNotLock);                      //今日成交賣出股數
    int regularOrderBuyQty = conditionPtr->GetRegularOrderBuyQty(UFC::rwNotLock);            //今日日盤整股委託買進股數
    int regularOrderSellQty = conditionPtr->GetRegularOrderSellQty(UFC::rwNotLock);          //今日日盤整股委託賣出股數
//    int intraDayOddOrderBuyQty = conditionPtr->GetIntraDayOddOrderBuyQty(UFC::rwNotLock);    //今日日盤零股委託買進股數
    int intraDayOddOrderSellQty = conditionPtr->GetIntraDayOddOrderSellQty(UFC::rwNotLock);  //今日日盤零股委託賣出股數
    int orderBuyQty = conditionPtr->GetOrderBuyQty(UFC::rwNotLock);                          //今日委託買進股數
    int orderSellQty = conditionPtr->GetOrderSellQty(UFC::rwNotLock);                        //今日委託賣出股數
//    FLogPtr->fprintf(" %s availableSellQty[%d] normalMatchSellQty[%d] normalMatchBuyQty[%d] matchBuyQty[%d] normalOrderSellQty[%d] orderSellQty[%d].",
//                     logHead.c_str(), availableSellQty, normalMatchSellQty, normalMatchBuyQty, matchBuyQty, normalOrderSellQty, orderSellQty);
    FLogPtr->fprintf(" %s Condition: %s, Order[%s], MsgType[%d].",
                     logHead.c_str(), conditionPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), msgType);
        
    UFC::PStringBuffer availableBuffer;
    if (isSellShort)
    {
        int totalAvailableBuyQty = 0;
        if (msgType == TWSE::mtT010)  //Regular
        {
            totalAvailableBuyQty = availableBuyQty - regularOrderBuyQty;
            availableBuffer.Printf("Position[%d] - RegularOrderBuyQty[%d]", availableBuyQty, regularOrderBuyQty);
        }
        else
        {
            totalAvailableBuyQty = availableBuyQty - regularMatchBuyQty - orderBuyQty;
            availableBuffer.Printf("Position[%d] - RegularMatchBuyQty[%d] - OrderBuyQty[%d]", availableBuyQty, regularMatchBuyQty, orderBuyQty);
        }
        FLogPtr->fprintf(" %s AvailableBuyQty[%d] = %s.", logHead.c_str(), totalAvailableBuyQty, availableBuffer.c_str());
        if (totalAvailableBuyQty < 0) totalAvailableBuyQty = 0;
        if (orderQty > totalAvailableBuyQty)
        {
            CheckMessage.Printf("Reject[%s]: OrderQty[%d] > Available Buy Qty[%d]", orderID.c_str(), orderQty, totalAvailableBuyQty);
            isPassed = false;                            
        }
        else
            FLogPtr->fprintf(" %s [%s] Passed: OrderQty[%d] <= Available Buy Qty[%d].", logHead.c_str(), orderID.c_str(), orderQty, totalAvailableBuyQty);
    }
    else
        FLogPtr->fprintf(" %s [%s] Passed: Do not check buy order.", logHead.c_str(), orderID.c_str());

    if (isPassed)
    {
        OrderPtr->SetPassed(isPassed, UFC::rwLockForWrite);
        
        bool isInsertOrderSuccess = false;
        if (msgType == TWSE::mtT010)
        {
            conditionPtr->IncreaseRegularOrderBuyQty(orderQty, UFC::rwNotLock);
            isInsertOrderSuccess = StockDataPtr->InsertRegularBuyOrder(orderID, OrderPtr, UFC::cstCheck, UFC::rwNotLock);                    
        }
        else if (msgType == TWSE::mtO110)
        {
            conditionPtr->IncreaseIntraDayOddOrderBuyQty(orderQty, UFC::rwNotLock);
            isInsertOrderSuccess = StockDataPtr->InsertIntraDayOddBuyOrder(orderID, OrderPtr, UFC::cstCheck, UFC::rwNotLock);                    
        }
        else
        {
            conditionPtr->IncreaseOrderBuyQty(orderQty, UFC::rwNotLock);
            isInsertOrderSuccess = StockDataPtr->InsertBuyOrder(orderID, OrderPtr, UFC::cstCheck, UFC::rwNotLock);
        }
            
        if (isInsertOrderSuccess) 
        {
            double origBuyOrderAmount = AccountPtr->GetBuyOrderAmount(UFC::rwNotLock);
            double newBuyOrderAmount = origBuyOrderAmount + orderAmount;
            AccountPtr->SetBuyOrderAmount(newBuyOrderAmount, UFC::rwNotLock);
            FLogPtr->fprintf(" %s Set Account[%s] BuyOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origBuyOrderAmount, newBuyOrderAmount);
            FLogPtr->fprintf(" %s %s.", logHead.c_str(), AccountPtr->ToString(UFC::rwNotLock).c_str());
        }
        
        if (isInsertOrderSuccess)
            FLogPtr->fprintf(" %s Stock: %s, Order[%s], MsgType[%d].", logHead.c_str(), StockDataPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), msgType);
        else
            FLogPtr->fprintf(" %s Insert Order[%s] into Stock[%s] Data failed.", logHead.c_str(), orderID.c_str(), stockNo.c_str());
        FLogPtr->fprintf(" %s Condition: %s, Order[%s], MsgType[%d].", logHead.c_str(), conditionPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), msgType);
    }  //if (isPassed)
    conditionPtr->UnLockForWrite();
    
    return isPassed;
}  //CMegaRiskManagerClass::CheckBuyOrder()
//---------------------------------------------------------------------------------------------------------------------
bool CMegaRiskManagerClass::CheckSellOrder(CRMOrderSummary *OrderPtr, CRMInvestorAccount *AccountPtr, CRMStockTradingData *StockDataPtr, UFC::AnsiString& CheckMessage)
{
    UFC::AnsiString logHead = "CMegaRiskManagerClass::CheckSellOrder()";
    bool isPassed = true;
    OrderPtr->LockForRead();
    TWSE::TMessageType msgType = OrderPtr->GetMsgType(UFC::rwNotLock);
    UFC::AnsiString orderID = OrderPtr->GetOrderID(UFC::rwNotLock);
    UFC::AnsiString stockNo = OrderPtr->GetStockNo(UFC::rwNotLock);
    UFC::AnsiString accountNo = OrderPtr->GetAccountNo(UFC::rwNotLock);
    UFC::AnsiString warrantNo = OrderPtr->GetWarrantNo(UFC::rwNotLock);
    UFC::AnsiString brokerID = OrderPtr->GetBrokerID(UFC::rwNotLock);
    char buySellCode = OrderPtr->GetSide(UFC::rwNotLock);
    char twsePriceType = OrderPtr->GetTWSEPriceType(UFC::rwNotLock);
    char twseOrderType = OrderPtr->GetTWSEOrderType(UFC::rwNotLock);
    int orderQty = OrderPtr->GetOrderQty(UFC::rwNotLock);
    int tradeUnit = OrderPtr->GetTradeUnit(UFC::rwNotLock);
    double orderPrice = OrderPtr->GetOrderPrice(UFC::rwNotLock);    
    double amountPrice = OrderPtr->GetAmountPrice(UFC::rwNotLock);
    double orderAmount = OrderPtr->GetOrderAmount(UFC::rwNotLock);
    nsOrderMessageDefine::TradingSessionIDEnum tradingSessionID = OrderPtr->GetSessionID(UFC::rwNotLock);
    OrderPtr->UnLockForRead();

    int availableSellQty = 0;  //可賣股數
    int availableBuyQty  = 0;  //可買股數
    bool isSellShort = false;  
    if ((msgType != TWSE::mtO010) && (msgType != TWSE::mtO110))  //零股沒有借券
        isSellShort = brokerID != CMegaRiskManagerDataPool::GetRMMainBrokerID();  //是否為借券
    CRMBrokerStockCondition *conditionPtr = StockDataPtr->FindCondition(warrantNo, brokerID, twseOrderType, UFC::rwNotLock);
    if (conditionPtr == 0)
    {
        if (!isSellShort)
        {
            FLogPtr->fprintf(" %s Sell Condition Account[%s] Stock[%s] WarrantNo[%s] BrokerID[%s] TWSEOrdType[%c] not Found.",
                             logHead.c_str(), accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerID.c_str(), twseOrderType);
            UFC::AnsiString todayStr;
            UFC::GetYYYYMMDD(todayStr, FALSE);
            conditionPtr = new CRMBrokerStockCondition(warrantNo, brokerID, twseOrderType, availableSellQty, availableBuyQty, todayStr, isSellShort);
            conditionPtr->SetAccountNo(accountNo, UFC::rwNotLock);
            conditionPtr->SetStockNo(stockNo, UFC::rwNotLock);
            if (!StockDataPtr->InsertCondition(warrantNo, brokerID, twseOrderType, conditionPtr, UFC::cstNotCheck, UFC::rwNotLock))
            {
                CheckMessage.Printf("Reject[%s]: Insert Account[%s] Stock[%s][%s][%s]%c condition failed", orderID.c_str(), accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerID.c_str(), twseOrderType);
                delete conditionPtr;
                conditionPtr = 0;
            }
        }
        else
            CheckMessage.Printf("Reject[%s]: Account[%s] Stock[%s][%s][%s]%c not found", orderID.c_str(), accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerID.c_str(), twseOrderType);
            
        if (conditionPtr == 0) 
        {
            FLogPtr->fprintf(" %s %s, MsgType[%d].", logHead.c_str(), CheckMessage.c_str(), msgType);
            FLogPtr->fprintf(" %s", logHead.c_str());
            FLogPtr->Flush();
            return false;
        }
    }  //if (conditionPtr == 0)
    
    conditionPtr->LockForWrite();
    availableSellQty = conditionPtr->GetAvailableSellQty(UFC::rwNotLock);                    //可賣股數
    availableBuyQty = conditionPtr->GetAvailableBuyQty(UFC::rwNotLock);                      //可買股數
//    isSellShort = conditionPtr->IsSellShort(UFC::rwNotLock);                                 //是否為借券
    int regularMatchBuyQty = conditionPtr->GetRegularMatchBuyQty(UFC::rwNotLock);            //今日日盤整股買進成交股數
    int regularMatchSellQty = conditionPtr->GetRegularMatchSellQty(UFC::rwNotLock);          //今日日盤整股賣出成交股數
    int intraDayOddMatchSellQty = conditionPtr->GetIntraDayOddMatchSellQty(UFC::rwNotLock);  //今日日盤整股賣出成交股數
    
    int matchBuyQty = conditionPtr->GetMatchBuyQty(UFC::rwNotLock);                          //今日成交買進股數
//    int matchSellQty = conditionPtr->GetMatchSellQty(UFC::rwNotLock);                      //今日成交賣出股數
    int regularOrderBuyQty = conditionPtr->GetRegularOrderBuyQty(UFC::rwNotLock);            //今日日盤整股委託買進股數
    int regularOrderSellQty = conditionPtr->GetRegularOrderSellQty(UFC::rwNotLock);          //今日日盤整股委託賣出股數
//    int intraDayOddOrderBuyQty = conditionPtr->GetIntraDayOddOrderBuyQty(UFC::rwNotLock);    //今日日盤零股委託買進股數
    int intraDayOddOrderSellQty = conditionPtr->GetIntraDayOddOrderSellQty(UFC::rwNotLock);  //今日日盤零股委託賣出股數
    int orderBuyQty = conditionPtr->GetOrderBuyQty(UFC::rwNotLock);                          //今日委託買進股數
    int orderSellQty = conditionPtr->GetOrderSellQty(UFC::rwNotLock);                        //今日委託賣出股數
    FLogPtr->fprintf(" %s Condition: %s, Order[%s], MsgType[%d].",
                     logHead.c_str(), conditionPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), msgType);
        
    UFC::PStringBuffer availableBuffer;
    if (!isSellShort && (twseOrderType == '6'))
    {
        CheckMessage.Printf("Reject[%s]: Wrong OrderType[%c]", orderID.c_str(), twseOrderType);
        isPassed = false;            
    }
    else
    {   //現行盤中零股不允許當沖（當日買進不可當日賣出），今賣成交股數要扣掉(當沖賣出)的部分
        //參考公式（可當沖股票）：
        //股數可賣 = (昨日股數 + 今買股數成交 - (今賣委託未成交股數 + 今賣成交股數)) - (今賣零股委託未成交 + 今賣成交零股)
        //零股可賣 = (昨日股數 - (今賣委託未成交股數 + 今賣成交股數)) - (今賣零股委託未成交 + 今賣成交零股)
        //參考公式（不可當沖股票）：
        //股數可賣 = (昨日股數 - (今賣委託未成交股數 + 今賣成交股數)) - (今賣零股委託未成交 + 今賣成交零股)
        //零股可賣 = (昨日股數 - (今賣委託未成交股數 + 今賣成交股數)) - (今賣零股委託未成交 + 今賣成交零股)
        if (AccountPtr->NeedCheckSellPosition(UFC::rwNotLock))  //Check Sell Position
        {
            int totalAvailableSellQty = availableSellQty;
            availableBuffer.Printf("Position[%d]", availableSellQty);
            if ((msgType != TWSE::mtT010) && (msgType != TWSE::mtO110))  //not Regular && not IntraDay Odd
            {
                totalAvailableSellQty -= (regularMatchSellQty + intraDayOddMatchSellQty);
                availableBuffer.AppendPrintf(" - RegularMatchSellQty[%d] - IntraDayOddMatchSellQty[%d]", regularMatchSellQty, intraDayOddMatchSellQty);
            }
            int dayTradeType = AccountPtr->GetDayTradeType(UFC::rwNotLock);
            if (dayTradeType == 1) 
            {
                if (CMegaRiskManagerDataPool::IsRMHedgeStock(stockNo, AccountPtr->GetAccountNo(UFC::rwNotLock)))
                    FLogPtr->fprintf(" %s [%s] is Hedge Stock, Account[%s] Buy/Sell Day Trade Enabled.", logHead.c_str(), stockNo.c_str(), accountNo.c_str());
                else
                {
                    FLogPtr->fprintf(" %s [%s] is not Hedge Stock, Account[%s] Buy/Sell Day Trade Disabled.", logHead.c_str(), stockNo.c_str(), accountNo.c_str());
                    dayTradeType = 0;
                }
            }  //if (dayTradeType == 1)
                    
            if (!isSellShort && (dayTradeType == 1) &&  //Can Sell Buy Today Qty
                (msgType != TWSE::mtO110) && (msgType != TWSE::mtO010))  //not IntraDay Odd && not Odd
            {
                totalAvailableSellQty += regularMatchBuyQty;
                availableBuffer.AppendPrintf(" + RegularMatchBuyQty[%d]", regularMatchBuyQty);
                if (msgType != TWSE::mtT010)  //not Regular
                {
                    totalAvailableSellQty += matchBuyQty;
                    availableBuffer.AppendPrintf(" + MatchBuyQty[%d]", matchBuyQty);
                }
            }
                
            if (msgType == TWSE::mtT010)  //Regular
            {
                totalAvailableSellQty -= (regularOrderSellQty + intraDayOddOrderSellQty);
                availableBuffer.AppendPrintf(" - RegularOrderSellQty[%d] - IntraDayOddOrderSellQty[%d]", regularOrderSellQty, intraDayOddOrderSellQty);
            }
            else if (msgType == TWSE::mtO110)  //IntraDay Odd
            {
//                if (!isSellShort && (dayTradeType == 1) && (regularMatchBuyQty > 0))
//                {
//                    int restRegularOrderSellQty = regularOrderSellQty - regularMatchBuyQty;
//                    if (restRegularOrderSellQty >= 0)
//                    {
//                        totalAvailableSellQty -= restRegularOrderSellQty;
//                        availableBuffer.AppendPrintf(" - (RegularOrderSellQty[%d] - RegularMatchBuyQty[%d])", regularOrderSellQty, regularMatchBuyQty);
//                    }
                    
//                    totalAvailableSellQty -= intraDayOddOrderSellQty;
//                    availableBuffer.AppendPrintf(" - IntraDayOddOrderSellQty[%d]", intraDayOddOrderSellQty);
//                }
//                else
//                {
                    totalAvailableSellQty -= (regularOrderSellQty + intraDayOddOrderSellQty);
                    availableBuffer.AppendPrintf(" - RegularOrderSellQty[%d] - IntraDayOddOrderSellQty[%d]", regularOrderSellQty, intraDayOddOrderSellQty);
//                }
            }
            else
            {
                totalAvailableSellQty -= orderSellQty;
                availableBuffer.AppendPrintf(" - OrderSellQty[%d]", orderSellQty);
            }
            
            FLogPtr->fprintf(" %s AvailableSellQty[%d] = %s.", logHead.c_str(), totalAvailableSellQty, availableBuffer.c_str());
            if (totalAvailableSellQty < 0) totalAvailableSellQty = 0;
            if (orderQty > totalAvailableSellQty)
            {
                if (isSellShort)
                    CheckMessage.Printf("Reject[%s]: OrderQty[%d] > Available SellShort Qty[%d]", orderID.c_str(), orderQty, totalAvailableSellQty);
                else
                    CheckMessage.Printf("Reject[%s]: OrderQty[%d] > Available Sell Qty[%d]", orderID.c_str(), orderQty, totalAvailableSellQty);
                isPassed = false;            
            }
            else
                FLogPtr->fprintf(" %s [%s] Passed: OrderQty[%d] <= Available Sell Qty[%d].", logHead.c_str(), orderID.c_str(), orderQty, totalAvailableSellQty);
        }
        else
            FLogPtr->fprintf(" %s Account[%s] need not Check Sell Position.", logHead.c_str(), accountNo.c_str());
    }  //if (!isSellShort && (orderType == '6'))
    
    if (isPassed)
    {
        OrderPtr->SetPassed(isPassed, UFC::rwLockForWrite);
        
        bool isInsertOrderSuccess = false;
        if (msgType == TWSE::mtT010)
        {
            conditionPtr->IncreaseRegularOrderSellQty(orderQty, UFC::rwNotLock);
            isInsertOrderSuccess = StockDataPtr->InsertRegularSellOrder(orderID, OrderPtr, UFC::cstCheck, UFC::rwNotLock);
        }
        else if (msgType == TWSE::mtO110)
        {
            conditionPtr->IncreaseIntraDayOddOrderSellQty(orderQty, UFC::rwNotLock);
            isInsertOrderSuccess = StockDataPtr->InsertIntraDayOddSellOrder(orderID, OrderPtr, UFC::cstCheck, UFC::rwNotLock);
        }
        else
        {
            conditionPtr->IncreaseOrderSellQty(orderQty, UFC::rwNotLock);
            isInsertOrderSuccess = StockDataPtr->InsertSellOrder(orderID, OrderPtr, UFC::cstCheck, UFC::rwNotLock);
        }
            
        if (isInsertOrderSuccess) 
        {
            double origSellOrderAmount = AccountPtr->GetSellOrderAmount(UFC::rwNotLock);
            double newSellOrderAmount = origSellOrderAmount + orderAmount;
            AccountPtr->SetSellOrderAmount(newSellOrderAmount, UFC::rwNotLock);
            FLogPtr->fprintf(" %s Set Account[%s] SellOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origSellOrderAmount, newSellOrderAmount);
            FLogPtr->fprintf(" %s %s.", logHead.c_str(), AccountPtr->ToString(UFC::rwNotLock).c_str());
        }
        
        if (isInsertOrderSuccess)
            FLogPtr->fprintf(" %s Stock: %s, Order[%s], MsgType[%d].", logHead.c_str(), StockDataPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), msgType);
        else
            FLogPtr->fprintf(" %s Insert Order[%s] into Stock[%s] Data failed.", logHead.c_str(), orderID.c_str(), stockNo.c_str());
        FLogPtr->fprintf(" %s Condition: %s, Order[%s], MsgType[%d].", logHead.c_str(), conditionPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), msgType);
    }
    else
        FLogPtr->fprintf(" %s %s", logHead.c_str(), CheckMessage.c_str());
    conditionPtr->UnLockForWrite();
    
    return isPassed;
}  //CMegaRiskManagerClass::CheckSellOrder()
//---------------------------------------------------------------------------------------------------------------------
bool CMegaRiskManagerClass::CheckReplacePriceOrder(CRMOrderSummary *OrderPtr, CRMInvestorAccount *AccountPtr, CRMStockTradingData *StockDataPtr, CRMOrderSummary *OrderRecPtr, UFC::AnsiString& CheckMessage)
{
    UFC::AnsiString logHead = "CMegaRiskManagerClass::CheckReplacePriceOrder()";
    bool isPassed = true;
    OrderPtr->LockForRead();
    TWSE::TMessageType msgType = OrderPtr->GetMsgType(UFC::rwNotLock);
    int functionCode = OrderPtr->GetFunctionCode(UFC::rwNotLock);
    UFC::AnsiString orderID = OrderPtr->GetOrderID(UFC::rwNotLock);
    UFC::AnsiString stockNo = OrderPtr->GetStockNo(UFC::rwNotLock);
    UFC::AnsiString accountNo = OrderPtr->GetAccountNo(UFC::rwNotLock);
    UFC::AnsiString warrantNo = OrderPtr->GetWarrantNo(UFC::rwNotLock);
    UFC::AnsiString brokerID = OrderPtr->GetBrokerID(UFC::rwNotLock);
    char buySellCode = OrderPtr->GetSide(UFC::rwNotLock);
    char twsePriceType = OrderPtr->GetTWSEPriceType(UFC::rwNotLock);
    char twseOrderType = OrderPtr->GetTWSEOrderType(UFC::rwNotLock);
    int orderQty = OrderPtr->GetOrderQty(UFC::rwNotLock);
    int tradeUnit = OrderPtr->GetTradeUnit(UFC::rwNotLock);
    double orderPrice = OrderPtr->GetOrderPrice(UFC::rwNotLock);    
    double amountPrice = OrderPtr->GetAmountPrice(UFC::rwNotLock);
    double orderAmount = OrderPtr->GetOrderAmount(UFC::rwNotLock);
    nsOrderMessageDefine::TradingSessionIDEnum tradingSessionID = OrderPtr->GetSessionID(UFC::rwNotLock);
    OrderPtr->UnLockForRead();
    
    OrderRecPtr->LockForRead();
    double origOrderPrice = OrderRecPtr->GetOrderPrice(UFC::rwNotLock);
    int origOrderOty = OrderRecPtr->GetOrderQty(UFC::rwNotLock);
    int origLeavesQty = OrderRecPtr->GetLeavesOrderQty(UFC::rwNotLock);
    double origOrderAmount = OrderRecPtr->GetOrderAmount(UFC::rwNotLock);
    OrderRecPtr->UnLockForRead();
    orderAmount = orderPrice * static_cast<double>(origLeavesQty);
    FLogPtr->fprintf(" %s Order[%s] Replace Price from [%10.4lf] to [%10.4lf], Amount from [%15.4lf] to [%15.4lf].", logHead.c_str(), orderID.c_str(), origOrderPrice, orderPrice, origOrderAmount, orderAmount);

    if (orderPrice > origOrderPrice)
    {
        double increaseAmount = (orderPrice - origOrderPrice) * static_cast<double>(orderQty);
        isPassed = AccountPtr->CheckOrderAmount(orderID, buySellCode, increaseAmount, CheckMessage, UFC::rwNotLock);
    }
    
    if (isPassed)
    {
        OrderRecPtr->LockForWrite();
        OrderRecPtr->SetOrigOrderPrice(origOrderPrice, UFC::rwNotLock);
        OrderRecPtr->SetOrigOrderAmount(origOrderAmount, UFC::rwNotLock);
        OrderRecPtr->SetOrderPrice(orderPrice, UFC::rwNotLock);
        OrderRecPtr->SetAmountPrice(orderPrice, UFC::rwNotLock);
        OrderRecPtr->SetOrderAmount(orderAmount, UFC::rwNotLock);
        OrderRecPtr->UnLockForWrite();
        
        if (buySellCode == 'B')
        {
            if (tradingSessionID == nsOrderMessageDefine::tsNormal)
                StockDataPtr->UpdateMaxRegularBuyOrderPrice(UFC::rwNotLock);
            else if (tradingSessionID == nsOrderMessageDefine::tsIntradayOdd)
                StockDataPtr->UpdateMaxIntraDayOddBuyOrderPrice(UFC::rwNotLock);
            else
                StockDataPtr->UpdateMaxBuyOrderPrice(UFC::rwNotLock);
                
            double origBuyOrderAmount = AccountPtr->GetBuyOrderAmount(UFC::rwNotLock);
            double newBuyOrderAmount = origBuyOrderAmount - origOrderAmount + orderAmount;
            if (newBuyOrderAmount <= 0.0) newBuyOrderAmount = 0.0;
            AccountPtr->SetBuyOrderAmount(newBuyOrderAmount, UFC::rwNotLock);
            FLogPtr->fprintf(" %s Set Account[%s] Buy OrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origBuyOrderAmount, newBuyOrderAmount);
        }
        else
        {
            if (tradingSessionID == nsOrderMessageDefine::tsNormal)
                StockDataPtr->UpdateMinRegularSellOrderPrice(UFC::rwNotLock);
            else if (tradingSessionID == nsOrderMessageDefine::tsIntradayOdd)
                StockDataPtr->UpdateMinIntraDayOddSellOrderPrice(UFC::rwNotLock);
            else
                StockDataPtr->UpdateMinSellOrderPrice(UFC::rwNotLock);
            
            double origSellOrderAmount = AccountPtr->GetSellOrderAmount(UFC::rwNotLock);
            double newSellOrderAmount = origSellOrderAmount - origOrderAmount + orderAmount;
            if (newSellOrderAmount <= 0.0) newSellOrderAmount = 0.0;
            AccountPtr->SetSellOrderAmount(newSellOrderAmount, UFC::rwNotLock);
            FLogPtr->fprintf(" %s Set Account[%s] Sell OrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origSellOrderAmount, newSellOrderAmount);
        }
        FLogPtr->fprintf(" %s %s.", logHead.c_str(), AccountPtr->ToString(UFC::rwNotLock).c_str());
        FLogPtr->fprintf(" %s Stock: %s, Order[%s], MsgType[%d].", logHead.c_str(), StockDataPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), msgType);
    }  //if (isPassed)
    
    return isPassed;
}  //CMegaRiskManagerClass::CheckReplacePriceOrder()
//---------------------------------------------------------------------------------------------------------------------
bool CMegaRiskManagerClass::CheckRiskCondition(TWSE::MessageHeader*& Msg, OrderRecord& OI, UFC::AnsiString& CheckMessage)
{
    int functionCode = OI.FuncCode;
    UFC::AnsiString orderKey = OI.Key;
    Int32 networkID = OI.NID;
    TWSE::TMessageType msgType = static_cast<TWSE::TMessageType>(OI.MsgType);
    if (((msgType != TWSE::mtT010) &&   //Regular      30, 93
         (msgType != TWSE::mtO010) &&   //Odd          40, 94
         (msgType != TWSE::mtO110) &&   //IntraDay Odd 33, 83
         (msgType != TWSE::mtP010)) ||  //Fix Price    32, 98
        ((functionCode != 1) && (functionCode != 2) && (functionCode != 6)))  //buy or sell or replace price 
        return true;

    UFC::AnsiString logHead = "CMegaRiskManagerClass::CheckRiskCondition()";
    FLogPtr->fprintf(" %s", logHead.c_str());
    
    CRMOrderSummary *orderPtr = new CRMOrderSummary(functionCode, msgType, orderKey, Msg);
    orderPtr->SetNetworkID(networkID, UFC::rwNotLock);
    
    UFC::AnsiString stockNo = orderPtr->GetStockNo(UFC::rwNotLock);
    UFC::AnsiString orderID = orderPtr->GetOrderID(UFC::rwNotLock);
    char twsePriceType = orderPtr->GetTWSEPriceType(UFC::rwNotLock);
    UFC::AnsiString accountNo = orderPtr->GetAccountNo(UFC::rwNotLock);
    
    UFC::AnsiString warrantNo = orderPtr->GetWarrantNo(UFC::rwNotLock);
    UFC::AnsiString brokerID = orderPtr->GetBrokerID(UFC::rwNotLock);
    char buySellCode = orderPtr->GetSide(UFC::rwNotLock);
    char twseOrderType = orderPtr->GetTWSEOrderType(UFC::rwNotLock);
    int orderQty = orderPtr->GetOrderQty(UFC::rwNotLock);
    int tradeUnit = orderPtr->GetTradeUnit(UFC::rwNotLock);
    double orderPrice = orderPtr->GetOrderPrice(UFC::rwNotLock);    
    double amountPrice = orderPtr->GetAmountPrice(UFC::rwNotLock);
    double orderAmount = orderPtr->GetOrderAmount(UFC::rwNotLock);
    nsOrderMessageDefine::TradingSessionIDEnum tradingSessionID = orderPtr->GetSessionID(UFC::rwNotLock);

    if (warrantNo.Length() <= 0)
    {
        CheckMessage.Printf("UserData[1] is not Valid.");
        return false;
    }
    else if (brokerID.Length() <= 0)
    {
        CheckMessage.Printf("UserData[2] is not Valid.");
        return false;
    }
        
    SecurityInfo* stockPtr = CRiskManagerDataPool::FindStock(stockNo);
    if (stockPtr != 0) 
    {
        int stkTradeUnit = stockPtr->GetTradeUnit();
        if (stkTradeUnit != tradeUnit)
        {
            orderPtr->SetTradeUnit(tradeUnit, UFC::rwNotLock);
            orderQty = orderPtr->GetOrderQty(UFC::rwNotLock);
            orderPtr->SetLeavesQty(orderQty, UFC::rwNotLock);
        }
        
        if (twsePriceType == '1')  //Market
        {
            double bullPrice = 0.0, bearPrice = 0.0;
            stockPtr->GetSessionPriceLimit(tradingSessionID, bullPrice, bearPrice);
            if (buySellCode == 'B')
                amountPrice = bullPrice;
            else
                amountPrice = bearPrice;
            FLogPtr->fprintf(" %s Set Market Order[%s] Stock[%s] Px[%15.4lf] AmtPx[%15.4lf].", logHead.c_str(), orderID.c_str(), stockNo.c_str(), orderPrice, amountPrice);
            orderPtr->SetAmountPrice(amountPrice, UFC::rwNotLock);
        }  //if (twsePriceType == '1')
        
        orderAmount = amountPrice * orderQty;
        orderPtr->SetOrderAmount(orderAmount, UFC::rwNotLock);
    }
    else
        FLogPtr->fprintf(" %s Order[%s] Stock[%s] not Found.", logHead.c_str(), orderID.c_str(), stockNo.c_str());    
    FLogPtr->fprintf(" %s Check %s", logHead.c_str(), orderPtr->ToString(UFC::rwNotLock).c_str());
    
    CheckMessage = "";
    bool isPassed = true;
    bool isOrderInserted = false;
    CRMOrderSummary *existOrderPtr = gMegaRMDataPoolPtr->FindOrderFromIDIndex(orderID, UFC::rwLockForRead);
    if ((functionCode == 1) || (functionCode == 2))  //buy or sell
    {
        if (existOrderPtr != 0)
        {
            if (networkID == existOrderPtr->GetNetworkID(UFC::rwLockForRead))
            {
                isPassed = existOrderPtr->IsPassed(UFC::rwLockForRead);
                if (isPassed)
                    FLogPtr->fprintf(" %s [%s] Passed: Order has been Checked.", logHead.c_str(), orderID.c_str());
                else
                    FLogPtr->fprintf(" %s [%s] Rejected: Order has been Checked.", logHead.c_str(), orderID.c_str());
                    
                delete orderPtr;
                return isPassed;
            }
            else
            {
                CheckMessage.Printf("Reject[%s] Duplicate OrderID", orderID.c_str());
                isPassed = false;
            }
        }
        else
        {
            if (gMegaRMDataPoolPtr->InsertOrderToIDIndex(orderID, orderPtr, UFC::cstNotCheck, UFC::rwLockForWrite))
                isOrderInserted = true;
            else
            {
                CheckMessage.Printf("Reject[%s] Duplicate OrderID", orderID.c_str());
                isPassed = false;
            }  //if (existOrderPtr != 0)
        }
    }
    else if (functionCode == 6)  //Replace Price
    {
        if (twsePriceType == '1')
        {
            CheckMessage.Printf("Can not Replace Market Order[%s] Price", orderID.c_str());
            isPassed = false;
        }
        else
        {
            if (existOrderPtr == 0)
            {
                CheckMessage.Printf("Replace Price Order[%s] not exist", orderID.c_str());
                isPassed = false;
            }
        }  //if (twsePriceType == '1')
    }  //if ((functionCode == 1) || (functionCode == 2))
    
    if (!isPassed)
    {
        FLogPtr->fprintf(" %s %s.", logHead.c_str(), CheckMessage.c_str());
        FLogPtr->Flush();
        if (!isOrderInserted) delete orderPtr;        
        return isPassed;
    }  //if (!isPassed)
        
    CRMInvestorAccount *accountPtr = gMegaRMDataPoolPtr->FindAccountFromNoIndex(accountNo, UFC::rwLockForRead);
    if (accountPtr != 0)
    {
        accountPtr->LockForWrite();
        FLogPtr->fprintf(" %s %s.", logHead.c_str(), accountPtr->ToString(UFC::rwNotLock).c_str());
        CRMStockTradingData *stockDataPtr = 0;
        if ((twsePriceType == '1') &&  //Market
            CMegaRiskManagerDataPool::NeedCheckSelfTrading() && accountPtr->NeedCheckSelfTrade(UFC::rwNotLock))
        {
            CheckMessage.Printf("Reject[%s]: Not Support Market Order.", orderID.c_str());
            isPassed = false;
        }
        else
        {
            if (accountPtr->NeedCheckStockWhiteList(UFC::rwNotLock) &&
                !accountPtr->IsStockInWhiteList(stockNo, UFC::rwNotLock))
            {
                CheckMessage.Printf("Reject[%s]: Stock[%s] not in WhiteList.", orderID.c_str(), stockNo.c_str());
                isPassed = false;
            }
            else
            {
                stockDataPtr = accountPtr->FindOrCreateStockTradingData(stockNo, UFC::rwNotLock);
                if (stockDataPtr == 0)
                {
                    CheckMessage.Printf("Reject[%s]: Wrong Account[%s] Stock[%s]", orderID.c_str(), accountNo.c_str(), stockNo.c_str());
                    isPassed = false;
                }
            }
        }  //if ((twsePriceType == '1') &&
        
        if (isPassed)
        {
            if (stockDataPtr != 0)
            {
                stockDataPtr->LockForWrite();
                FLogPtr->fprintf(" %s Stock: %s, Order[%s], MsgType[%d].", logHead.c_str(), stockDataPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), msgType);
                bool accountNeedCheckSelfTrade = accountPtr->NeedCheckSelfTrade(UFC::rwNotLock);
                if (CMegaRiskManagerDataPool::NeedCheckSelfTrading() && accountNeedCheckSelfTrade)
                {
                    if (buySellCode == 'B')
                    {
                        double minSellOrderPrice = 0.0;
                        if (msgType == TWSE::mtT010)  //Regular
                            minSellOrderPrice = stockDataPtr->GetMinRegularSellOrderPrice(UFC::rwNotLock);
                        else if (msgType == TWSE::mtO110)  //IntraDay Odd 
                            minSellOrderPrice = stockDataPtr->GetMinIntraDayOddSellOrderPrice(UFC::rwNotLock);
                        else
                            minSellOrderPrice = stockDataPtr->GetMinSellOrderPrice(UFC::rwNotLock);
                        
                        if (orderPrice < minSellOrderPrice)
                            FLogPtr->fprintf(" %s [%s] Passed: Px[%11.4lf] < MinSellOrderPx[%11.4lf].", logHead.c_str(), orderID.c_str(), orderPrice, minSellOrderPrice);
                        else
                        {
                            CheckMessage.Printf("Reject[%s]: Px[%11.4lf] >= MinSellOrderPx[%11.4lf]", orderID.c_str(), orderPrice, minSellOrderPrice);
                            isPassed = false;
                        }
                    }
                    else if (buySellCode == 'S')
                    {
                        double maxBuyOrderPrice = 0.0;
                        if (msgType == TWSE::mtT010)  //Regular
                            maxBuyOrderPrice = stockDataPtr->GetMaxRegularBuyOrderPrice(UFC::rwNotLock);
                        else if (msgType == TWSE::mtO110)  //IntraDay Odd 
                            maxBuyOrderPrice = stockDataPtr->GetMaxIntraDayOddBuyOrderPrice(UFC::rwNotLock);
                        else
                            maxBuyOrderPrice = stockDataPtr->GetMaxBuyOrderPrice(UFC::rwNotLock);
                
                        if (orderPrice > maxBuyOrderPrice)
                            FLogPtr->fprintf(" %s [%s] Passed: Px[%11.4lf] > MaxBuyOrderPx[%11.4lf].", logHead.c_str(), orderID.c_str(), orderPrice, maxBuyOrderPrice);
                        else
                        {
                            CheckMessage.Printf("Reject[%s]: Px[%11.4lf] <= MaxBuyOrderPx[%11.4lf]", orderID.c_str(), orderPrice, maxBuyOrderPrice);
                            isPassed = false;
                        }
                    }
                    else
                        FLogPtr->fprintf(" %s Wrong Buy Sell Code[%c].", logHead.c_str(), buySellCode);
                }
                else
                {
                    if (!CMegaRiskManagerDataPool::NeedCheckSelfTrading())
                        FLogPtr->fprintf(" %s Need not Check SelfTrading.", logHead.c_str());
                    else if (!accountNeedCheckSelfTrade)
                        FLogPtr->fprintf(" %s Account[%s] Need not Check SelfTrading.", logHead.c_str(), accountNo.c_str());
                }  //if (CMegaRiskManagerDataPool::NeedCheckSelfTrading() && accountNeedCheckSelfTrade)
                
                if (isPassed)
                {
                    if ((functionCode == 1) || (functionCode == 2))  //buy or sell
                    {
                        if (accountPtr->CheckOrderAmount(orderID, buySellCode, orderAmount, CheckMessage, UFC::rwNotLock))
                        {
                            if (functionCode == 1)
                                isPassed = CheckBuyOrder(orderPtr, accountPtr, stockDataPtr, CheckMessage);
                            else
                                isPassed = CheckSellOrder(orderPtr, accountPtr, stockDataPtr, CheckMessage);
                        }
                        else
                            isPassed = false;
                    }
                    else if (functionCode == 6)  //Replace Price
                        isPassed = CheckReplacePriceOrder(orderPtr, accountPtr, stockDataPtr, existOrderPtr, CheckMessage);
                }  //if (isPassed)
                stockDataPtr->UnLockForWrite();
            }
            else
            {
                CheckMessage.Printf("Reject[%s]: Wrong Account[%s] Stock[%s]", orderID.c_str(), accountNo.c_str(), stockNo.c_str());
                isPassed = false;
            }  //if (stockDataPtr != 0)
        }  //if (isPassed)
        accountPtr->UnLockForWrite();
    }
    else
    {
        CheckMessage.Printf("Reject[%s]: Account[%s] not found", orderID.c_str(), accountNo.c_str());
        isPassed = false;
    }
    
    if (!isOrderInserted) delete orderPtr;
    if (!isPassed) FLogPtr->fprintf(" %s %s.", logHead.c_str(), CheckMessage.c_str());
    FLogPtr->Flush();
    return isPassed;
}  //CMegaRiskManagerClass::CheckRiskCondition()
//---------------------------------------------------------------------------------------------------------------------
void CMegaRiskManagerClass::ProcessRiskRejectMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI)
{
    int functionCode = OI.FuncCode;
    if ((functionCode != 1) && (functionCode != 2) && (functionCode != 6)) return;

    UFC::AnsiString logHead = "CMegaRiskManagerClass::ProcessRiskRejectMessage()";
    UFC::AnsiString orderID( OI.GetOrderID() );
    TWSE::TMessageType msgType = static_cast<TWSE::TMessageType>(OI.MsgType);
    FLogPtr->fprintf(" %s Process Reject Order[%s].", logHead.c_str(), orderID.c_str());
    
    CRMOrderSummary *orderPtr = gMegaRMDataPoolPtr->FindOrderFromIDIndex(orderID, UFC::rwLockForRead);
    if (orderPtr != 0)
    {
        orderPtr->LockForWrite();
        UFC::AnsiString accountNo = orderPtr->GetAccountNo(UFC::rwNotLock);
        UFC::AnsiString stockNo = orderPtr->GetStockNo(UFC::rwNotLock);
        UFC::AnsiString warrantNo = orderPtr->GetWarrantNo(UFC::rwNotLock);
        UFC::AnsiString brokerID = orderPtr->GetBrokerID(UFC::rwNotLock);
        char twseOrderType = orderPtr->GetTWSEOrderType(UFC::rwNotLock);
        char buySellCode = orderPtr->GetSide(UFC::rwNotLock);
        //char exchangeCode = orderPtr->GetExchangeCode(UFC::rwNotLock);
        int orderOty = orderPtr->GetOrderQty(UFC::rwNotLock);
        double origOrderPrice = orderPtr->GetOrigOrderPrice(UFC::rwNotLock);
        double orderPrice = orderPtr->GetOrderPrice(UFC::rwNotLock);
        double orderAmount = orderPtr->GetOrderAmount(UFC::rwNotLock);
        double origOrderAmount = orderPtr->GetOrigOrderAmount(UFC::rwNotLock);
        if (functionCode == 6)  //Replace Price
        {
            orderPtr->SetOrderPrice(origOrderPrice, UFC::rwNotLock);
            orderPtr->SetAmountPrice(origOrderPrice, UFC::rwNotLock);
            orderPtr->SetOrderAmount(origOrderAmount, UFC::rwNotLock);
            FLogPtr->fprintf(" %s Set Order[%s] OrderPrice from[%10.4lf] to [%10.4lf], OrderAmount from[%15.4lf] to [%15.4lf].", logHead.c_str(), orderID.c_str(), orderPrice, origOrderPrice, orderAmount, origOrderAmount);
        }
        orderPtr->UnLockForWrite();
        
        CRMInvestorAccount *accountPtr = gMegaRMDataPoolPtr->FindAccountFromNoIndex(accountNo, UFC::rwLockForRead);
        if (accountPtr != 0)
        {
            accountPtr->LockForWrite();
            if (functionCode == 1)  //New Order Buy
            {
                double origBuyOrderAmount = accountPtr->GetBuyOrderAmount(UFC::rwNotLock); 
                double removeOrderAmount = orderAmount;
                double newBuyOrderAmount = origBuyOrderAmount - orderAmount;
                if (newBuyOrderAmount >= 0.0)
                    FLogPtr->fprintf(" %s Set AccountNo[%s] BuyOrderAmount[%15.4lf] = OrigBuyOrderAmount[%15.4lf] - OrderAmount[%15.4lf].", logHead.c_str(), accountNo.c_str(), newBuyOrderAmount, origBuyOrderAmount, orderAmount);
                else
                {
                    FLogPtr->fprintf(" %s AccountNo[%s] BuyOrderAmount[%15.4lf] < OrderAmount[%15.4lf], Set BuyOrderAmount to 0.0.", logHead.c_str(), accountNo.c_str(), origBuyOrderAmount, orderAmount);
                    newBuyOrderAmount = 0.0;
                }
                accountPtr->SetBuyOrderAmount(newBuyOrderAmount, UFC::rwNotLock);
            }
            else if (functionCode == 2)    //New Order Sell
            {
                double origSellOrderAmount = accountPtr->GetSellOrderAmount(UFC::rwNotLock); 
                double newSellOrderAmount = origSellOrderAmount - orderAmount;
                if (newSellOrderAmount >= 0.0)
                    FLogPtr->fprintf(" %s Set AccountNo[%s] SellOrderAmount[%15.4lf] = OrigSellOrderAmount[%15.4lf] - OrderAmount[%15.4lf].", logHead.c_str(), accountNo.c_str(), newSellOrderAmount, origSellOrderAmount, orderAmount);
                else
                {
                    FLogPtr->fprintf(" %s AccountNo[%s] SellOrderAmount[%15.4lf] < OrderAmount[%15.4lf], Set SellOrderAmount to 0.0.", logHead.c_str(), accountNo.c_str(), origSellOrderAmount, orderAmount);
                    newSellOrderAmount = 0.0;
                }
                accountPtr->SetSellOrderAmount(newSellOrderAmount, UFC::rwNotLock);
            }
            else  //functionCode[6] Replace Price
            {
                if (buySellCode == 'B')
                {
                    double origBuyOrderAmount = accountPtr->GetBuyOrderAmount(UFC::rwNotLock); 
                    double newBuyOrderAmount = origBuyOrderAmount - orderAmount + origOrderAmount;
                    FLogPtr->fprintf(" %s Set AccountNo[%s] BuyOrderAmount[%15.4lf] = OrigBuyOrderAmount[%15.4lf] - OrderAmount[%15.4lf] + OrigOrderAmount[%15.4lf].", logHead.c_str(), accountNo.c_str(), newBuyOrderAmount, origBuyOrderAmount, orderAmount, origOrderAmount);
                    if (newBuyOrderAmount <= 0.0) newBuyOrderAmount = 0.0;
                    accountPtr->SetBuyOrderAmount(newBuyOrderAmount, UFC::rwNotLock);
                }
                else
                {
                    double origSellOrderAmount = accountPtr->GetSellOrderAmount(UFC::rwNotLock); 
                    double newSellOrderAmount = origSellOrderAmount - orderAmount + origOrderAmount;
                    FLogPtr->fprintf(" %s Set AccountNo[%s] SellOrderAmount[%15.4lf] = OrigSellOrderAmount[%15.4lf] - OrderAmount[%15.4lf] + OrigOrderAmount[%15.4lf].", logHead.c_str(), accountNo.c_str(), newSellOrderAmount, origSellOrderAmount, orderAmount, origOrderAmount);
                    if (newSellOrderAmount <= 0.0) newSellOrderAmount = 0.0;
                    accountPtr->SetSellOrderAmount(newSellOrderAmount, UFC::rwNotLock);
                }
            }  //if (functionCode == 1)
            CRMStockTradingData *stockDataPtr = accountPtr->FindOrCreateStockTradingData(stockNo, UFC::rwNotLock);
            accountPtr->UnLockForWrite();
            
            if (stockDataPtr != 0)
            {
                stockDataPtr->LockForWrite();
                CRMOrderSummary *removeOrderPtr = 0;
                if (functionCode == 1)
                {
                    if (msgType == TWSE::mtT010)
                        removeOrderPtr = stockDataPtr->RemoveRegularBuyOrder(orderID, UFC::rwNotLock);
                    else if (msgType == TWSE::mtO110)
                        removeOrderPtr = stockDataPtr->RemoveIntraDayOddBuyOrder(orderID, UFC::rwNotLock);
                    else
                        removeOrderPtr = stockDataPtr->RemoveBuyOrder(orderID, UFC::rwNotLock);
                    
                    if (removeOrderPtr == 0)
                        FLogPtr->fprintf(" %s Remove Buy Order[%s] from Account[%s] Stock[%s] failed.", logHead.c_str(), orderID.c_str(), accountNo.c_str(), stockNo.c_str());
                }
                else if (functionCode == 2)
                {
                    if (msgType == TWSE::mtT010)
                        removeOrderPtr = stockDataPtr->RemoveRegularSellOrder(orderID, UFC::rwNotLock);
                    else if (msgType == TWSE::mtO110)
                        removeOrderPtr = stockDataPtr->RemoveIntraDayOddSellOrder(orderID, UFC::rwNotLock);
                    else
                        removeOrderPtr = stockDataPtr->RemoveSellOrder(orderID, UFC::rwNotLock);
                    
                    if (removeOrderPtr == 0)
                        FLogPtr->fprintf(" %s Remove Sell Order[%s] from Account[%s] Stock[%s] failed.", logHead.c_str(), orderID.c_str(), accountNo.c_str(), stockNo.c_str());
                }
                else
                {
                    if (buySellCode == 'B')
                    {
                        if (msgType == TWSE::mtT010)
                            stockDataPtr->UpdateMaxRegularBuyOrderPrice(UFC::rwNotLock);
                        else if (msgType == TWSE::mtO110)
                            stockDataPtr->UpdateMaxIntraDayOddBuyOrderPrice(UFC::rwNotLock);
                        else
                            stockDataPtr->UpdateMaxBuyOrderPrice(UFC::rwNotLock);
                    }
                    else
                    {
                        if (msgType == TWSE::mtT010)
                            stockDataPtr->UpdateMinRegularSellOrderPrice(UFC::rwNotLock);
                        else if (msgType == TWSE::mtO110)
                            stockDataPtr->UpdateMinIntraDayOddSellOrderPrice(UFC::rwNotLock);
                        else
                            stockDataPtr->UpdateMinSellOrderPrice(UFC::rwNotLock);
                    }
                }
                FLogPtr->fprintf(" %s StockData: %s, Order[%s], msgType[%d].", logHead.c_str(), stockDataPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), msgType);
                CRMBrokerStockCondition *conditionPtr = 0;
                if ((functionCode == 1) || (functionCode == 2))
                    conditionPtr = stockDataPtr->FindCondition(warrantNo, brokerID, twseOrderType, UFC::rwNotLock);
                stockDataPtr->UnLockForWrite();
                
                if (conditionPtr != 0)
                {
                    conditionPtr->LockForWrite();
                    if (functionCode == 1)
                    {
                        if (msgType == TWSE::mtT010)
                        {
                            int regularOrderBuyQty = conditionPtr->GetRegularOrderBuyQty(UFC::rwNotLock);
                            if (regularOrderBuyQty >= orderOty)
                                conditionPtr->IncreaseRegularOrderBuyQty(-orderOty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetRegularOrderBuyQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s RegularOrderBuyQty[%d] is Less then orderQty[%d], set RegularOrderBuyQty to 0.", logHead.c_str(), regularOrderBuyQty, orderOty);
                            }
                        }
                        else if (msgType == TWSE::mtO110)
                        {
                            int intraDayOddOrderBuyQty = conditionPtr->GetIntraDayOddOrderBuyQty(UFC::rwNotLock);
                            if (intraDayOddOrderBuyQty >= orderOty)
                                conditionPtr->IncreaseIntraDayOddOrderBuyQty(-orderOty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetIntraDayOddOrderBuyQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s RegularOrderBuyQty[%d] is Less then orderQty[%d], set IntraDayOddOrderBuyQty to 0.", logHead.c_str(), intraDayOddOrderBuyQty, orderOty);
                            }
                        }
                        else
                        {
                            int orderBuyQty = conditionPtr->GetOrderBuyQty(UFC::rwNotLock);
                            if (orderBuyQty >= orderOty)
                                conditionPtr->IncreaseOrderBuyQty(-orderOty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetOrderBuyQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s OrderBuyQty[%d] is Less then orderQty[%d], set OrderBuyQty to 0.", logHead.c_str(), orderBuyQty, orderOty);
                            }
                        }
                    }
                    else  //Sell
                    {
                        if (msgType == TWSE::mtT010)
                        {
                            int regularOrderSellQty = conditionPtr->GetRegularOrderSellQty(UFC::rwNotLock);
                            if (regularOrderSellQty >= orderOty)
                                conditionPtr->IncreaseRegularOrderSellQty(-orderOty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetRegularOrderSellQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s RegularOrderSellQty[%d] is Less then orderQty[%d], set RegularOrderSellQty to 0.", logHead.c_str(), regularOrderSellQty, orderOty);
                            }
                        }
                        else if (msgType == TWSE::mtO110)
                        {
                            int intraDayOddOrderSellQty = conditionPtr->GetIntraDayOddOrderSellQty(UFC::rwNotLock);
                            if (intraDayOddOrderSellQty >= orderOty)
                                conditionPtr->IncreaseIntraDayOddOrderSellQty(-orderOty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetRegularOrderSellQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s IntraDayOddOrderSellQty[%d] is Less then orderQty[%d], set IntraDayOddOrderSellQty to 0.", logHead.c_str(), intraDayOddOrderSellQty, orderOty);
                            }
                        }
                        else
                        {
                            int orderSellQty = conditionPtr->GetOrderSellQty(UFC::rwNotLock);
                            if (orderSellQty >= orderOty)
                                conditionPtr->IncreaseOrderSellQty(-orderOty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetOrderSellQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s OrderSellQty[%d] is Less then orderQty[%d], set OrderSellQty to 0.", logHead.c_str(), orderSellQty, orderOty);
                            }
                        }  //if (msgType == mtT010)
                    }
                    FLogPtr->fprintf(" %s Condition: %s, Order[%s].", logHead.c_str(), conditionPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str());
                    conditionPtr->UnLockForWrite();
                }
                else
                    FLogPtr->fprintf(" %s Account[%s] Stock[%s][%s][%s] %c not found, Order[%s].", logHead.c_str(), accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerID.c_str(), twseOrderType, orderID.c_str());
            }
            else
                FLogPtr->fprintf(" %s Account[%s] has no Stock[%s] data, Order[%s].", logHead.c_str(), accountNo.c_str(), stockNo.c_str(), orderID.c_str());
        }
        else
            FLogPtr->fprintf(" %s Account[%s] of Rejected Order[%s] not found.", logHead.c_str(), accountNo.c_str(), orderID.c_str());
    }
    else
        FLogPtr->fprintf(" %s Rejected Order[%s] not Found.", logHead.c_str(), orderID.c_str());
    FLogPtr->fprintf(" %s.", logHead.c_str());
    FLogPtr->Flush();
}  //CMegaRiskManagerClass::ProcessRiskRejectMessage()
//---------------------------------------------------------------------------------------------------------------------
void CMegaRiskManagerClass::ProcessRiskBuySellMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI)
{
    int msgStatusCode = ReplyMsg->GetStatusCode();
    if ((msgStatusCode != 31) && 
        (msgStatusCode != 48) && (msgStatusCode != 49) &&
        (msgStatusCode != 51) && (msgStatusCode != 52))
        return;
    
    UFC::AnsiString logHead = "CMegaRiskManagerClass::ProcessRiskBuySellMessage()";
    
    int functionCode = OI.FuncCode;
    TWSE::TMessageType msgType = ReplyMsg->MessageType();
    nsOrderMessageDefine::TradingSessionIDEnum tradingSessionID = nsOrderMessageDefine::tsAuto;
    UFC::AnsiString accountNo, stockNo, orderID, orderQtyStr, beforeQtyStr, afterQtyStr, orderPriceStr;
    char buySellCode = ' ', exchangeCode = ' ', twsePriceType = 0, twseTimeInForce = 0, twseOrderType = ' ';
    double orderPrice = 0.0;
    UFC::AnsiString sessionStr = "";
    if (msgType == TWSE::mtT020)  //Regular
    {
        tradingSessionID = nsOrderMessageDefine::tsNormal;
        sessionStr = "Regular";
        TWSE::MessageT020 *t020MsgPtr = static_cast<TWSE::MessageT020*>(ReplyMsg);
        accountNo = UFC::AnsiString(t020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(t020MsgPtr->StockID, 6);
        orderID = UFC::AnsiString(t020MsgPtr->OrderNo, 5);
        buySellCode = t020MsgPtr->BuySellCode;
        exchangeCode = t020MsgPtr->ExchangeCode;
        twseOrderType = t020MsgPtr->TWSEOrderType;
        twsePriceType = t020MsgPtr->OrderType;      //'1':Market '2':Limit
        twseTimeInForce = t020MsgPtr->TimeInForce;  //'0':ROD '3':IOC '4':FOK
        orderQtyStr = UFC::AnsiString(t020MsgPtr->OrderQty, 6);
        beforeQtyStr = UFC::AnsiString(t020MsgPtr->BeforeQty, 6);
        afterQtyStr = UFC::AnsiString(t020MsgPtr->AfterQty, 6);
        orderPriceStr = UFC::AnsiString(t020MsgPtr->OrderPrice, 9);
        orderPrice = Parse95v9999PriceStr(orderPriceStr);
    }
    else if (msgType == TWSE::mtO120)  //IntraDay Odd
    {
        tradingSessionID = nsOrderMessageDefine::tsIntradayOdd;
        sessionStr = "IntraDayOdd";
        TWSE::MessageO120 *o120MsgPtr = static_cast<TWSE::MessageO120*>(ReplyMsg);
        accountNo = UFC::AnsiString(o120MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(o120MsgPtr->StockID, 6);
        orderID = UFC::AnsiString(o120MsgPtr->OrderNo, 5);
        buySellCode = o120MsgPtr->BuySellCode;
        exchangeCode = '2';     //Odd
        twsePriceType = '2';    //Limit
        twseTimeInForce = '0';  //ROD
        orderQtyStr = UFC::AnsiString(o120MsgPtr->OrderQty, 6);
        beforeQtyStr = UFC::AnsiString(o120MsgPtr->BeforeQty, 6);
        afterQtyStr = UFC::AnsiString(o120MsgPtr->AfterQty, 6);
        orderPriceStr = UFC::AnsiString(o120MsgPtr->OrderPrice, 9);
        orderPrice = Parse95v9999PriceStr(orderPriceStr);
    }
    else if (msgType == TWSE::mtO020)  //Odd
    {
        tradingSessionID = nsOrderMessageDefine::tsOddLot;
        sessionStr = "Odd";
        TWSE::MessageO020 *o020MsgPtr = static_cast<TWSE::MessageO020*>(ReplyMsg);
        accountNo = UFC::AnsiString(o020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(o020MsgPtr->StockID, 6);
        orderID = UFC::AnsiString(o020MsgPtr->OrderNo, 5);
        buySellCode = o020MsgPtr->BuySellCode;
        exchangeCode = '2';     //Odd
        twsePriceType = '2';    //Limit
        twseTimeInForce = '0';  //ROD
        orderQtyStr = UFC::AnsiString(o020MsgPtr->OrderQty, 6);
        beforeQtyStr = UFC::AnsiString(o020MsgPtr->BeforeQty, 6);
        afterQtyStr = UFC::AnsiString(o020MsgPtr->AfterQty, 6);
        orderPriceStr = UFC::AnsiString(o020MsgPtr->OrderPrice, 9);
        orderPrice = Parse95v9999PriceStr(orderPriceStr);
    }
    else if (msgType == TWSE::mtP020)  //OffHour
    {
        tradingSessionID = nsOrderMessageDefine::tsOffHour;
        sessionStr = "OffHour";
        TWSE::MessageP020 *p020MsgPtr = static_cast<TWSE::MessageP020*>(ReplyMsg);
        accountNo = UFC::AnsiString(p020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(p020MsgPtr->StockID, 6);
        orderID = UFC::AnsiString(p020MsgPtr->OrderNo, 5);
        buySellCode = p020MsgPtr->BuySellCode;
        exchangeCode = p020MsgPtr->ExchangeCode;
        twsePriceType = '2';    //Limit
        twseTimeInForce = '0';  //ROD
        orderQtyStr = UFC::AnsiString(p020MsgPtr->OrderQty, 6);
        beforeQtyStr = UFC::AnsiString(p020MsgPtr->BeforeQty, 6);
        afterQtyStr = UFC::AnsiString(p020MsgPtr->AfterQty, 6);
        orderPriceStr = UFC::AnsiString(p020MsgPtr->OrderPrice, 9);
        orderPrice = Parse95v9999PriceStr(orderPriceStr);
    }
    else
    {
        FLogPtr->fprintf(" %s Order[%s] Wrong msgType[%d].", logHead.c_str(), OI.GetOrderID(), msgType);
        FLogPtr->fprintf(" %s", logHead.c_str());
        return;
    }  //if (msgType == mtT020)
    
    accountNo.TrimRight(' ');
    stockNo.TrimRight(' ');
    SecurityInfo* stockPtr = CRiskManagerDataPool::FindStock(stockNo);

    int tradeUnit = 1000;
    double amountPrice = orderPrice;
    if (stockPtr != 0) 
    {
        tradeUnit = stockPtr->GetTradeUnit();
        if (twsePriceType == '1')  //Market
        {
            double bullPrice = 0.0, bearPrice = 0.0;
            stockPtr->GetSessionPriceLimit(tradingSessionID, bullPrice, bearPrice);
            if (buySellCode == 'B')
                amountPrice = bullPrice;
            else
                amountPrice = bearPrice;
            FLogPtr->fprintf(" %s Set Market Order[%s] Stock[%s] Px[%15.4lf] AmtPx[%15.4lf].", logHead.c_str(), orderID.c_str(), stockNo.c_str(), orderPrice, amountPrice);
        }  //if (twsePriceType == '1')
    }
    else
        FLogPtr->fprintf(" %s Order[%s] Stock[%s] not Found.", logHead.c_str(), orderID.c_str(), stockNo.c_str());
    
    int orderQty = orderQtyStr.ToInt();
    int beforeQty = beforeQtyStr.ToInt();
    int afterQty = afterQtyStr.ToInt();
    if (exchangeCode != '2') 
    {
        orderQty *= tradeUnit;
        beforeQty *= tradeUnit;
        afterQty *= tradeUnit;
    }

    UFC::AnsiString sideStr = "";
    if (buySellCode == 'B')
        sideStr = "Buy";
    else
        sideStr = "Sell";
        
    UFC::AnsiString pxTypeStr = "";
    if (twsePriceType == '1')
        pxTypeStr = "Market";
    else if (twsePriceType == '2')
        pxTypeStr = "Limit";
    
    UFC::AnsiString tifStr = "";
    if (twseTimeInForce == '0')
        tifStr = "ROD";
    else if (twseTimeInForce == '3')
        tifStr = "IOC";
    else if (twseTimeInForce == '4')
        tifStr = "FOK";

    FLogPtr->fprintf(" %s %s Order[%s] func[%d] %s(%c) Px[%10.4lf] AmtPx[%10.4lf] Qty[Order=%d before=%d, after=%d] status[%d] msgType[%d] %s(%c) %s(%c) TradeUnit[%d].",
                     logHead.c_str(), sessionStr.c_str(), orderID.c_str(), functionCode, sideStr.c_str(), buySellCode, orderPrice, amountPrice, orderQty, beforeQty, afterQty, msgStatusCode, msgType, pxTypeStr.c_str(), twsePriceType, tifStr.c_str(), twseTimeInForce, tradeUnit);

    CRMOrderSummary *orderPtr = gMegaRMDataPoolPtr->FindOrderFromIDIndex(orderID, UFC::rwLockForRead);
    if (orderPtr != 0)
    {
        orderPtr->LockForWrite();
        UFC::AnsiString warrantNo = orderPtr->GetWarrantNo(UFC::rwNotLock);
        UFC::AnsiString brokerID = orderPtr->GetBrokerID(UFC::rwNotLock);
        int origOrderQty = orderPtr->GetOrderQty(UFC::rwNotLock);
        int origLeavesQty = orderPtr->GetLeavesOrderQty(UFC::rwNotLock);
        double origAmount = orderPtr->GetOrderAmount(UFC::rwNotLock);
        
        int newLeavesQty = afterQty;
        int differentQty = origLeavesQty - newLeavesQty;
        double newAmount = amountPrice * static_cast<double>(newLeavesQty);
        double differentAmount = origAmount - newAmount;
        orderPtr->SetOrderQty(orderQty, UFC::rwNotLock);
        orderPtr->SetLeavesQty(newLeavesQty, UFC::rwNotLock);
        orderPtr->SetOrderAmount(newAmount, UFC::rwNotLock);
        orderPtr->UnLockForWrite();
        FLogPtr->fprintf(" %s %s Order[%s] Set LeavesQty from %d to %d, OrdQty from %d to %d, Amount from %20.4lf to %20.4lf, account[%s], Stock[%s], func[%d], msgType[%d], bQty[%d], aQty[%d].",
                         logHead.c_str(), sessionStr.c_str(), orderID.c_str(),
                         origLeavesQty, newLeavesQty, 
                         origOrderQty, orderQty, 
                         origAmount, newAmount,
                         accountNo.c_str(), stockNo.c_str(), functionCode, msgType, beforeQty, afterQty);
        
        CRMInvestorAccount *accountPtr = gMegaRMDataPoolPtr->FindAccountFromNoIndex(accountNo, UFC::rwLockForRead);
        if (accountPtr != 0)
        {
            accountPtr->LockForWrite();
            if (buySellCode == 'B')
            {
                double origBuyOrderAmount = accountPtr->GetBuyOrderAmount(UFC::rwNotLock);
                double newBuyOrderAmount = 0.0;
                if (origBuyOrderAmount >= differentAmount)
                    newBuyOrderAmount = origBuyOrderAmount - differentAmount;
                else
                    FLogPtr->fprintf(" %s OrigBuyOrderAmount[%15.4lf] < DifferentAmount[%15.4lf], Set BuyOrderAmount to 0.0.", logHead.c_str(), origBuyOrderAmount, differentAmount);
                accountPtr->SetBuyOrderAmount(newBuyOrderAmount, UFC::rwNotLock);
                FLogPtr->fprintf(" %s Set Account[%s] BuyOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origBuyOrderAmount, newBuyOrderAmount);
            }
            else if (buySellCode == 'S')
            {
                double origSellOrderAmount = accountPtr->GetSellOrderAmount(UFC::rwNotLock);
                double newSellOrderAmount = 0.0;
                if (origSellOrderAmount >= differentAmount)
                    newSellOrderAmount = origSellOrderAmount - differentAmount;
                else
                    FLogPtr->fprintf(" %s OrigSellOrderAmount[%15.4lf] < DifferentAmount[%15.4lf], Set SellOrderAmount to 0.0.", logHead.c_str(), origSellOrderAmount, differentAmount);
                accountPtr->SetBuyOrderAmount(newSellOrderAmount, UFC::rwNotLock);
                FLogPtr->fprintf(" %s Set Account[%s] SellOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origSellOrderAmount, newSellOrderAmount);
            }  //if (buySellCode == 'B')
            FLogPtr->fprintf(" %s %s.", logHead.c_str(), accountPtr->ToString(UFC::rwNotLock).c_str());
            CRMStockTradingData *stockDataPtr = accountPtr->FindStockTradingData(stockNo, UFC::rwNotLock);
            accountPtr->UnLockForWrite();
            
            if (stockDataPtr != 0)
            {
                stockDataPtr->LockForWrite();
                CRMOrderSummary *targetOrderPtr = 0;
                if (buySellCode == 'B')
                {
                    if (newLeavesQty <= 0)
                    {
                        if (msgType == TWSE::mtT020)
                            targetOrderPtr = stockDataPtr->RemoveRegularBuyOrder(orderID, UFC::rwNotLock);
                        else if (msgType == TWSE::mtO120)
                            targetOrderPtr = stockDataPtr->RemoveIntraDayOddBuyOrder(orderID, UFC::rwNotLock);
                        else
                            targetOrderPtr = stockDataPtr->RemoveBuyOrder(orderID, UFC::rwNotLock);
                    }
                    else
                    {
                        if (msgType == TWSE::mtT020)
                            targetOrderPtr = stockDataPtr->FindRegularBuyOrder(orderID, UFC::rwNotLock);
                        else if (msgType == TWSE::mtO120)
                            targetOrderPtr = stockDataPtr->FindIntraDayOddBuyOrder(orderID, UFC::rwNotLock);
                        else
                            targetOrderPtr = stockDataPtr->FindBuyOrder(orderID, UFC::rwNotLock);
                    }
                    
                    if (targetOrderPtr == 0)
                        FLogPtr->fprintf(" %s Order[%s] not exist in StockData.", logHead.c_str(), orderID.c_str());
                }
                else if (buySellCode == 'S')
                {
                    if (newLeavesQty <= 0)
                    {
                        if (msgType == TWSE::mtT020)
                            targetOrderPtr = stockDataPtr->RemoveRegularSellOrder(orderID, UFC::rwNotLock);
                        else if (msgType == TWSE::mtO120)
                            targetOrderPtr = stockDataPtr->RemoveIntraDayOddSellOrder(orderID, UFC::rwNotLock);
                        else
                            targetOrderPtr = stockDataPtr->RemoveSellOrder(orderID, UFC::rwNotLock);
                    }
                    else
                    {
                        if (msgType == TWSE::mtT020)
                            targetOrderPtr = stockDataPtr->FindRegularSellOrder(orderID, UFC::rwNotLock);
                        else if (msgType == TWSE::mtO120)
                            targetOrderPtr = stockDataPtr->FindIntraDayOddSellOrder(orderID, UFC::rwNotLock);
                        else
                            targetOrderPtr = stockDataPtr->FindSellOrder(orderID, UFC::rwNotLock);
                    }
                    
                    if (targetOrderPtr == 0)
                        FLogPtr->fprintf(" %s Order[%s] not exist in StockData.", logHead.c_str(), orderID.c_str());
                }  //if (buySellCode == 'B')
                FLogPtr->fprintf(" %s StockData: %s, Order[%s], msgType[%d].", logHead.c_str(), stockDataPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), msgType);

                if (targetOrderPtr == 0)
                {
                    if (newLeavesQty <= 0)
                        FLogPtr->fprintf(" %s Remove Order[%s] from account[%s] Stock[%s] failed. BuySellCode'%c'.",
                                         logHead.c_str(), orderID.c_str(), accountNo.c_str(), stockNo.c_str(), buySellCode);
                    else
                        FLogPtr->fprintf(" %s Find Order[%s] from account[%s] Stock[%s] failed. BuySellCode'%c'.",
                                         logHead.c_str(), orderID.c_str(), accountNo.c_str(), stockNo.c_str(), buySellCode);
                }
                
                CRMBrokerStockCondition *conditionPtr = stockDataPtr->FindCondition(warrantNo, brokerID, twseOrderType, UFC::rwNotLock);
                stockDataPtr->UnLockForWrite();
                
                if (conditionPtr != 0)
                {
                    conditionPtr->LockForWrite();
                    if (buySellCode == 'B')
                    {
                        if (msgType == TWSE::mtT020)
                        {
                            int normalOrderBuyQty = conditionPtr->GetRegularOrderBuyQty(UFC::rwNotLock);
                            if (normalOrderBuyQty >= differentQty)
                                conditionPtr->IncreaseRegularOrderBuyQty(-differentQty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetRegularOrderBuyQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s RegularOrderBuyQty[%d] is Less then differentQty[%d], set NormalOrderBuyQty to 0.",
                                                 logHead.c_str(), normalOrderBuyQty, differentQty);
                            }
                        }
                        else if (msgType == TWSE::mtO120)
                        {
                            int intraDayOrderBuyQty = conditionPtr->GetIntraDayOddOrderBuyQty(UFC::rwNotLock);
                            if (intraDayOrderBuyQty >= differentQty)
                                conditionPtr->IncreaseIntraDayOddOrderBuyQty(-differentQty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetIntraDayOddOrderBuyQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s IntraDayOddOrderBuyQty[%d] is Less then differentQty[%d], set NormalOrderBuyQty to 0.",
                                                 logHead.c_str(), intraDayOrderBuyQty, differentQty);
                            }
                        }
                        else
                        {
                            int orderBuyQty = conditionPtr->GetOrderBuyQty(UFC::rwNotLock);
                            if (orderBuyQty >= differentQty)
                                conditionPtr->IncreaseOrderBuyQty(-differentQty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetOrderBuyQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s OrderBuyQty[%d] is Less then differentQty[%d], set OrderBuyQty to 0.",
                                                 logHead.c_str(), orderBuyQty, differentQty);
                            }
                        }
                    }
                    else
                    {
                        if (msgType == TWSE::mtT020)
                        {
                            int regularOrderSellQty = conditionPtr->GetRegularOrderSellQty(UFC::rwNotLock);
                            if (regularOrderSellQty >= differentQty)
                                conditionPtr->IncreaseRegularOrderSellQty(-differentQty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetRegularOrderSellQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s RegularOrderSellQty[%d] is Less then differentQty[%d], set NormalOrderSellQty to 0.",
                                                 logHead.c_str(), regularOrderSellQty, differentQty);
                            }
                        }
                        else if (msgType == TWSE::mtO120)
                        {
                            int intraDayOddOrderSellQty = conditionPtr->GetIntraDayOddOrderSellQty(UFC::rwNotLock);
                            if (intraDayOddOrderSellQty >= differentQty)
                                conditionPtr->IncreaseIntraDayOddOrderSellQty(-differentQty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetIntraDayOddOrderSellQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s IntraDayOddOrderSellQty[%d] is Less then differentQty[%d], set NormalOrderSellQty to 0.",
                                                 logHead.c_str(), intraDayOddOrderSellQty, differentQty);
                            }
                        }
                        else
                        {
                            int orderSellQty = conditionPtr->GetOrderSellQty(UFC::rwNotLock);
                            if (orderSellQty >= differentQty)
                                conditionPtr->IncreaseOrderSellQty(-differentQty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetOrderSellQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s OrderSellQty[%d] is Less then differentQty[%d], set OrderSellQty to 0.",
                                                 logHead.c_str(), orderSellQty, differentQty);
                            }
                        }  //if (msgType == mtT020)
                    }  //if (buySellCode == 'B')
                    FLogPtr->fprintf(" %s Condition: %s, order[%s], func[%d], msgType[%d], bQty[%d], aQty[%d].",
                                     logHead.c_str(), conditionPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), functionCode, msgType, beforeQty, afterQty);
                    conditionPtr->UnLockForWrite();
                }
                else
                    FLogPtr->fprintf(" %s Account[%s] Stock[%s][%s][%s] %c not found, Order[%s].",
                                     logHead.c_str(), accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerID.c_str(), twseOrderType, orderID.c_str());
            }
            else
                FLogPtr->fprintf(" %s Account[%s] has no Stock[%s] data, Order[%s].", logHead.c_str(), accountNo.c_str(), stockNo.c_str(), orderID.c_str());
        }
        else
            FLogPtr->fprintf(" %s Account[%s] of Cancel/Replace Order[%s] not found.", logHead.c_str(), accountNo.c_str(), orderID.c_str());
    }
    else
        FLogPtr->fprintf(" %s Order[%s] not found.", logHead.c_str(), orderID.c_str());
    FLogPtr->fprintf(" %s", logHead.c_str());
    FLogPtr->Flush();
}  //CMegaRiskManagerClass::ProcessRiskBuySellMessage()
//---------------------------------------------------------------------------------------------------------------------
void CMegaRiskManagerClass::ProcessRiskCancelReplaceMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI)
{
    int functionCode = OI.FuncCode;
    UFC::AnsiString logHead = "CMegaRiskManagerClass::ProcessRiskCancelReplaceMessage()";
    TWSE::TMessageType msgType = ReplyMsg->MessageType();
    nsOrderMessageDefine::TradingSessionIDEnum tradingSessionID = nsOrderMessageDefine::tsAuto;
    int msgStatusCode = ReplyMsg->GetStatusCode();
    UFC::AnsiString accountNo, stockNo, orderID, orderQtyStr, beforeQtyStr, afterQtyStr, orderPriceStr;
    char buySellCode = ' ', exchangeCode = ' ', twseOrderType = ' ', twsePriceType = ' ', twseTimeInForce = ' ';
    double orderPrice = 0.0;
    if (msgType == TWSE::mtT020)  //Regular
    {
        tradingSessionID = nsOrderMessageDefine::tsNormal;
        TWSE::MessageT020 *t020MsgPtr = static_cast<TWSE::MessageT020*>(ReplyMsg);
        accountNo = UFC::AnsiString(t020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(t020MsgPtr->StockID, 6);
        orderID = UFC::AnsiString(t020MsgPtr->OrderNo, 5);
        buySellCode = t020MsgPtr->BuySellCode;
        exchangeCode = t020MsgPtr->ExchangeCode;
        twseOrderType = t020MsgPtr->TWSEOrderType;
        twsePriceType = t020MsgPtr->OrderType;      //'1':Market '2':Limit
        twseTimeInForce = t020MsgPtr->TimeInForce;  //'0':ROD '3':IOC '4':FOK
        orderQtyStr = UFC::AnsiString(t020MsgPtr->OrderQty, 6);
        beforeQtyStr = UFC::AnsiString(t020MsgPtr->BeforeQty, 6);
        afterQtyStr = UFC::AnsiString(t020MsgPtr->AfterQty, 6);
        orderPriceStr = UFC::AnsiString(t020MsgPtr->OrderPrice, 9);
        orderPrice = Parse95v9999PriceStr(orderPriceStr);
    }
    else if (msgType == TWSE::mtO120)  //IntraDay Odd
    {
        tradingSessionID = nsOrderMessageDefine::tsIntradayOdd;
        TWSE::MessageO120 *o120MsgPtr = static_cast<TWSE::MessageO120*>(ReplyMsg);
        accountNo = UFC::AnsiString(o120MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(o120MsgPtr->StockID, 6);
        orderID = UFC::AnsiString(o120MsgPtr->OrderNo, 5);
        buySellCode = o120MsgPtr->BuySellCode;
        exchangeCode = '2';     //Odd
        twseOrderType = '0';
        twsePriceType = '2';    //Limit
        twseTimeInForce = '0';  //ROD
        orderQtyStr = UFC::AnsiString(o120MsgPtr->OrderQty, 6);
        beforeQtyStr = UFC::AnsiString(o120MsgPtr->BeforeQty, 6);
        afterQtyStr = UFC::AnsiString(o120MsgPtr->AfterQty, 6);
        orderPriceStr = UFC::AnsiString(o120MsgPtr->OrderPrice, 9);
        orderPrice = Parse95v9999PriceStr(orderPriceStr);
    }
    else if (msgType == TWSE::mtO020)  //Odd
    {
        tradingSessionID = nsOrderMessageDefine::tsOddLot;
        TWSE::MessageO020 *o020MsgPtr = static_cast<TWSE::MessageO020*>(ReplyMsg);
        accountNo = UFC::AnsiString(o020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(o020MsgPtr->StockID, 6);
        orderID = UFC::AnsiString(o020MsgPtr->OrderNo, 5);
        buySellCode = o020MsgPtr->BuySellCode;
        exchangeCode = '2';     //Odd
        twseOrderType = '0';
        twsePriceType = '2';    //Limit
        twseTimeInForce = '0';  //ROD
        orderQtyStr = UFC::AnsiString(o020MsgPtr->OrderQty, 6);
        beforeQtyStr = UFC::AnsiString(o020MsgPtr->BeforeQty, 6);
        afterQtyStr = UFC::AnsiString(o020MsgPtr->AfterQty, 6);
        orderPriceStr = UFC::AnsiString(o020MsgPtr->OrderPrice, 9);
        orderPrice = Parse95v9999PriceStr(orderPriceStr);
    }
    else if (msgType == TWSE::mtP020)  //OffHour
    {
        tradingSessionID = nsOrderMessageDefine::tsOffHour;
        TWSE::MessageP020 *p020MsgPtr = static_cast<TWSE::MessageP020*>(ReplyMsg);
        accountNo = UFC::AnsiString(p020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(p020MsgPtr->StockID, 6);
        orderID = UFC::AnsiString(p020MsgPtr->OrderNo, 5);
        buySellCode = p020MsgPtr->BuySellCode;
        exchangeCode = p020MsgPtr->ExchangeCode;
        twseOrderType = p020MsgPtr->TWSEOrderType;
        twsePriceType = '2';    //Limit
        twseTimeInForce = '0';  //ROD
        orderQtyStr = UFC::AnsiString(p020MsgPtr->OrderQty, 6);
        beforeQtyStr = UFC::AnsiString(p020MsgPtr->BeforeQty, 6);
        afterQtyStr = UFC::AnsiString(p020MsgPtr->AfterQty, 6);
        orderPriceStr = UFC::AnsiString(p020MsgPtr->OrderPrice, 9);
        orderPrice = Parse95v9999PriceStr(orderPriceStr);
    }
    else
    {
        FLogPtr->fprintf(" %s %s Wrong msgType[%d].", logHead.c_str(), OI.GetOrderID(), msgType);
        return;
    }
        
    accountNo.TrimRight(' ');
    stockNo.TrimRight(' ');
    SecurityInfo* stockPtr = CRiskManagerDataPool::FindStock(stockNo);
    int tradeUnit = 1000;
    double amountPrice = orderPrice;
    if (stockPtr != 0) 
    {
        tradeUnit = stockPtr->GetTradeUnit();
        if (twsePriceType == '1')  //Market
        {
            double bullPrice = 0.0, bearPrice = 0.0;
            stockPtr->GetSessionPriceLimit(tradingSessionID, bullPrice, bearPrice);
            if (buySellCode == 'B')
                amountPrice = bullPrice;
            else
                amountPrice = bearPrice;
            FLogPtr->fprintf(" %s Set Market Order[%s] Stock[%s] Px[%15.4lf] AmtPx[%15.4lf].", logHead.c_str(), orderID.c_str(), stockNo.c_str(), orderPrice, amountPrice);
        }  //if (twsePriceType == '1')
    }
    else
        FLogPtr->fprintf(" %s Order[%s] Stock[%s] not Found.", logHead.c_str(), orderID.c_str(), stockNo.c_str());
    
    int orderQty = orderQtyStr.ToInt();
    int beforeQty = beforeQtyStr.ToInt();
    int afterQty = afterQtyStr.ToInt();
    if (exchangeCode != '2') 
    {
        orderQty *= tradeUnit;
        beforeQty *= tradeUnit;
        afterQty *= tradeUnit;
    }
    int differentQty = beforeQty - afterQty;
    double differentAmount = amountPrice * differentQty;
    FLogPtr->fprintf(" %s Order[%s] func[%d] %c Px[%10.4lf] AmtPx[%10.4lf] Qty[Order=%d before=%d after=%d] status[%d] msgType[%d] PxType[%c] TIF[%c] TradeUnit[%d].",
                     logHead.c_str(), orderID.c_str(), functionCode, buySellCode, orderPrice, amountPrice, orderQty, beforeQty, afterQty, msgStatusCode, msgType, twsePriceType, twseTimeInForce, tradeUnit);

    CRMOrderSummary *orderPtr = gMegaRMDataPoolPtr->FindOrderFromIDIndex(orderID, UFC::rwLockForRead);
    if (orderPtr != 0)
    {
        orderPtr->LockForWrite();
        UFC::AnsiString warrantNo = orderPtr->GetWarrantNo(UFC::rwNotLock);
        UFC::AnsiString brokerID = orderPtr->GetBrokerID(UFC::rwNotLock);
        int origOrderQty = orderPtr->GetOrderQty(UFC::rwNotLock);
        int origLeavesQty = orderPtr->GetLeavesOrderQty(UFC::rwNotLock);
        double origOrderAmount = orderPtr->GetOrderAmount(UFC::rwNotLock);
        int newOrderQty = origOrderQty;
        int newLeavesQty = origLeavesQty - differentQty;
        if (newLeavesQty < 0)
        {
            FLogPtr->fprintf(" %s Order[%s] func[%d] OrigLeavesQty[%d] < DifferentQty[%d](beforeQty[%d] - afterQty[%d]).",
                             logHead.c_str(), orderID.c_str(), functionCode, origLeavesQty, differentQty, beforeQty, afterQty);            
            newLeavesQty = 0;
        }
        orderPtr->SetLeavesQty(newLeavesQty, UFC::rwNotLock);
        double newOrderAmount = amountPrice * static_cast<double>(newLeavesQty);
        orderPtr->SetOrderAmount(newOrderAmount, UFC::rwNotLock);
        if (functionCode == 3)  //Reduce Qty
        {
            newOrderQty = origOrderQty - differentQty;
            if (newOrderQty < 0) newOrderQty = 0;
            orderPtr->SetOrderQty(newOrderQty, UFC::rwNotLock);
            FLogPtr->fprintf(" %s Reduce Order[%s] Set Qty from [%d] to [%d], LeavesQty from [%d] to [%d], Amount from [%15.4lf] to [%15.4lf].",
                             logHead.c_str(), orderID.c_str(), origOrderQty, newOrderQty, origLeavesQty, newLeavesQty, origOrderAmount, newOrderAmount);
        }
        else
            FLogPtr->fprintf(" %s Cancel Order[%s] Set LeavesQty from [%d] to [%d], Amount from [%15.4lf] to [%15.4lf].",
                             logHead.c_str(), orderID.c_str(), origLeavesQty, newLeavesQty, origOrderAmount, newOrderAmount);
        orderPtr->UnLockForWrite();
        
        CRMInvestorAccount *accountPtr = gMegaRMDataPoolPtr->FindAccountFromNoIndex(accountNo, UFC::rwLockForRead);
        if (accountPtr != 0)
        {
            accountPtr->LockForWrite();
            if (buySellCode == 'B')
            {
                double origBuyOrderAmount = accountPtr->GetBuyOrderAmount(UFC::rwNotLock);
                double newBuyOrderAmount = 0.0;
                if (origBuyOrderAmount >= differentAmount)
                    newBuyOrderAmount = origBuyOrderAmount - differentAmount;
                else
                    FLogPtr->fprintf(" %s OrigBuyOrderAmount[%15.4lf] < DifferentAmount[%15.4lf], Set BuyOrderAmount to 0.0.", logHead.c_str(), origBuyOrderAmount, differentAmount);
                accountPtr->SetBuyOrderAmount(newBuyOrderAmount, UFC::rwNotLock);
                FLogPtr->fprintf(" %s Set Account[%s] BuyOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origBuyOrderAmount, newBuyOrderAmount);
            }
            else if (buySellCode == 'S')
            {
                double origSellOrderAmount = accountPtr->GetSellOrderAmount(UFC::rwNotLock);
                double newSellOrderAmount = 0.0;
                if (origSellOrderAmount >= differentAmount) 
                    newSellOrderAmount = origSellOrderAmount - differentAmount;
                else
                    FLogPtr->fprintf(" %s OrigSellOrderAmount[%15.4lf] < DifferentAmount[%15.4lf], Set SellOrderAmount to 0.0.", logHead.c_str(), origSellOrderAmount, differentAmount);
                accountPtr->SetSellOrderAmount(newSellOrderAmount, UFC::rwNotLock);
                FLogPtr->fprintf(" %s Set Account[%s] SellOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origSellOrderAmount, newSellOrderAmount);
            }
            else
                FLogPtr->fprintf(" %s Wrong BuySellCode[%c].", logHead.c_str(), buySellCode);
            FLogPtr->fprintf(" %s %s.", logHead.c_str(), accountPtr->ToString(UFC::rwNotLock).c_str());
            
            CRMStockTradingData *stockDataPtr = accountPtr->FindStockTradingData(stockNo, UFC::rwNotLock);
            accountPtr->UnLockForWrite();
            if (stockDataPtr != 0)
            {
                stockDataPtr->LockForWrite();
                if ((functionCode == 4) || (newLeavesQty <= 0))
                {
                    CRMOrderSummary *cancelOrderPtr = 0;
                    if (buySellCode == 'B')
                    {
                        if (msgType == TWSE::mtT020)
                            cancelOrderPtr = stockDataPtr->RemoveRegularBuyOrder(orderID, UFC::rwNotLock);
                        else if (msgType == TWSE::mtO120)
                            cancelOrderPtr = stockDataPtr->RemoveIntraDayOddBuyOrder(orderID, UFC::rwNotLock);
                        else
                            cancelOrderPtr = stockDataPtr->RemoveBuyOrder(orderID, UFC::rwNotLock);                        
                    }
                    else if (buySellCode == 'S')
                    {
                        if (msgType == TWSE::mtT020)
                            cancelOrderPtr = stockDataPtr->RemoveRegularSellOrder(orderID, UFC::rwNotLock);
                        else if (msgType == TWSE::mtO120)
                            cancelOrderPtr = stockDataPtr->RemoveIntraDayOddSellOrder(orderID, UFC::rwNotLock);
                        else
                            cancelOrderPtr = stockDataPtr->RemoveSellOrder(orderID, UFC::rwNotLock);
                    }
                    FLogPtr->fprintf(" %s StockData: %s, Order[%s], msgType[%d].", logHead.c_str(), stockDataPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), msgType);
            
                    if (cancelOrderPtr == 0)
                        FLogPtr->fprintf(" %s Remove Order[%s] from account[%s] Stock[%s] failed. BuySellCode'%c'.",
                                         logHead.c_str(), orderID.c_str(), accountNo.c_str(), stockNo.c_str(), buySellCode);
                }  //if ((functionCode == 4) || (newLeavesQty <= 0))
                FLogPtr->fprintf(" %s Set Order[%s] LeavesQty from %d to %d, account[%s], Stock[%s], func[%d], msgType[%d], bQty[%d], aQty[%d].",
                                 logHead.c_str(), orderID.c_str(), origLeavesQty, newLeavesQty, accountNo.c_str(), stockNo.c_str(), functionCode, msgType, beforeQty, afterQty);
                
                CRMBrokerStockCondition *conditionPtr = stockDataPtr->FindCondition(warrantNo, brokerID, twseOrderType, UFC::rwNotLock);
                stockDataPtr->UnLockForWrite();
                
                if (conditionPtr != 0)
                {
                    conditionPtr->LockForWrite();
                    if (buySellCode == 'B')
                    {
                        if (msgType == TWSE::mtT020)
                        {
                            int normalOrderBuyQty = conditionPtr->GetRegularOrderBuyQty(UFC::rwNotLock);
                            if (normalOrderBuyQty >= differentQty)
                                conditionPtr->IncreaseRegularOrderBuyQty(-differentQty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetRegularOrderBuyQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s RegularOrderBuyQty[%d] is Less then differentQty[%d], set RegularOrderBuyQty to 0.",
                                                 logHead.c_str(), normalOrderBuyQty, differentQty);
                            }
                        }
                        else if (msgType == TWSE::mtO120)
                        {
                            int intraDayOddOrderBuyQty = conditionPtr->GetIntraDayOddOrderBuyQty(UFC::rwNotLock);
                            if (intraDayOddOrderBuyQty >= differentQty)
                                conditionPtr->IncreaseIntraDayOddOrderBuyQty(-differentQty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetIntraDayOddOrderBuyQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s IntraDayOddOrderBuyQty[%d] is Less then differentQty[%d], set IntraDayOddOrderBuyQty to 0.",
                                                 logHead.c_str(), intraDayOddOrderBuyQty, differentQty);
                            }
                        }
                        else
                        {
                            int orderBuyQty = conditionPtr->GetOrderBuyQty(UFC::rwNotLock);
                            if (orderBuyQty >= differentQty)
                                conditionPtr->IncreaseOrderBuyQty(-differentQty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetOrderBuyQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s OrderBuyQty[%d] is Less then differentQty[%d], set OrderBuyQty to 0.",
                                                 logHead.c_str(), orderBuyQty, differentQty);
                            }
                        }
                    }
                    else
                    {
                        if (msgType == TWSE::mtT020)
                        {
                            int regularOrderSellQty = conditionPtr->GetRegularOrderSellQty(UFC::rwNotLock);
                            if (regularOrderSellQty >= differentQty)
                                conditionPtr->IncreaseRegularOrderSellQty(-differentQty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetRegularOrderSellQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s RegularOrderSellQty[%d] is Less then differentQty[%d], set RegularOrderSellQty to 0.",
                                                 logHead.c_str(), regularOrderSellQty, differentQty);
                            }
                        }
                        else if (msgType == TWSE::mtO120)
                        {
                            int intraDayOddOrderSellQty = conditionPtr->GetIntraDayOddOrderSellQty(UFC::rwNotLock);
                            if (intraDayOddOrderSellQty >= differentQty)
                                conditionPtr->IncreaseIntraDayOddOrderSellQty(-differentQty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetRegularOrderSellQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s IntraDayOddOrderSellQty[%d] is Less then differentQty[%d], set IntraDayOddOrderSellQty to 0.",
                                                 logHead.c_str(), intraDayOddOrderSellQty, differentQty);
                            }
                        }
                        else
                        {
                            int orderSellQty = conditionPtr->GetOrderSellQty(UFC::rwNotLock);
                            if (orderSellQty >= differentQty)
                                conditionPtr->IncreaseOrderSellQty(-differentQty, UFC::rwNotLock);
                            else
                            {
                                conditionPtr->SetOrderSellQty(0, UFC::rwNotLock);
                                FLogPtr->fprintf(" %s OrderSellQty[%d] is Less then differentQty[%d], set OrderSellQty to 0.",
                                                 logHead.c_str(), orderSellQty, differentQty);
                            }
                        }  //if (msgType == mtT020)
                    }  //if (buySellCode == 'B')
                    FLogPtr->fprintf(" %s Condition: %s, order[%s], func[%d], msgType[%d], bQty[%d], aQty[%d].",
                                     logHead.c_str(), conditionPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), functionCode, msgType, beforeQty, afterQty);
                    conditionPtr->UnLockForWrite();
                }
                else
                    FLogPtr->fprintf(" %s Account[%s] Stock[%s][%s][%s] %c not found, Order[%s].",
                                     logHead.c_str(), accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerID.c_str(), twseOrderType, orderID.c_str());
            }
            else
                FLogPtr->fprintf(" %s Account[%s] has no Stock[%s] data, Order[%s].",
                                 logHead.c_str(), accountNo.c_str(), stockNo.c_str(), orderID.c_str());
        }
        else
            FLogPtr->fprintf(" %s Account[%s] has no Stock[%s] data, Order[%s].",
                             logHead.c_str(), accountNo.c_str(), orderID.c_str());
    }
    else
        FLogPtr->fprintf(" %s Order[%s] not Found.", logHead.c_str(), orderID.c_str());
    FLogPtr->fprintf(" %s", logHead.c_str());
    FLogPtr->Flush();
}  //CMegaRiskManagerClass::ProcessRiskCancelReplaceMessage()
//----------------------------------------------------------------------------------------------------------------------
bool CMegaRiskManagerClass::IsFillRptExecExist(const UFC::AnsiString& ExecKey, UFC::RWLockTypeEnum LockType)
{
    bool isExist = false;
    FillRptExecLock.Lock(LockType);
    if (FillRptExecKey.Exists(ExecKey) == TRUE) isExist = true;
    FillRptExecLock.Unlock(LockType);
    return isExist;
}  //CMegaRiskManagerClass::IsFillRptExecExist()
//----------------------------------------------------------------------------------------------------------------------
void CMegaRiskManagerClass::AddFillRptExec(const UFC::AnsiString& ExecKey, UFC::RWLockTypeEnum LockType)
{
    if (ExecKey.Length() <= 0) return;
    FillRptExecLock.Lock(LockType);
    FillRptExecKey.Add(ExecKey);
    FillRptExecLock.Unlock(LockType);
}  //CMegaRiskManagerClass::AddFillRptExec()
//----------------------------------------------------------------------------------------------------------------------
void CMegaRiskManagerClass::AddRiskMatchQty(TWSE::R3Body *R3MsgBodyPtr, const UFC::AnsiString& ExecKey)
{
    UFC::AnsiString logHead = "CMegaRiskManagerClass::AddRiskMatchQty()";
    UFC::AnsiString orderID(R3MsgBodyPtr->FOrderNo, 5);
    
    UFC::AnsiString stockNo(R3MsgBodyPtr->FStockID, 6);
    stockNo.TrimRight(' ');
    SecurityInfo* stockPtr = CRiskManagerDataPool::FindStock(stockNo);
    
    char exchangeCode = R3MsgBodyPtr->FEXCD;
    char side = R3MsgBodyPtr->FBuySellCode;
    int matchQty = R3MsgBodyPtr->GetQty();
    double matchPrice = R3MsgBodyPtr->GetPrice();
    
    int tradeUnit = 1000;
    if (stockPtr != 0)
        tradeUnit = stockPtr->GetTradeUnit();
    else
        FLogPtr->fprintf(" %s Order[%s] Stock[%s] not Found.", logHead.c_str(), orderID.c_str(), stockNo.c_str());
    if (exchangeCode != '2') matchQty *= tradeUnit;
    double matchAmount = matchPrice * static_cast<double>(matchQty);
    FLogPtr->fprintf(" %s Match Order[%s] %c Stock[%s] Qty[%d] Price[%10.4lf] Amount[%15.4lf] TradeUnit[%d]", logHead.c_str(), orderID.c_str(), side, stockNo.c_str(), matchQty, matchPrice, matchAmount, tradeUnit);
    
    CRMOrderSummary *orderPtr = gMegaRMDataPoolPtr->FindOrderFromIDIndex(orderID, UFC::rwLockForRead);
    if (orderPtr != 0)
    {
        orderPtr->LockForWrite();
        UFC::AnsiString accountNo = orderPtr->GetAccountNo(UFC::rwNotLock);
        UFC::AnsiString warrantNo = orderPtr->GetWarrantNo(UFC::rwNotLock);
        UFC::AnsiString brokerID = orderPtr->GetBrokerID(UFC::rwNotLock);
        char twseOrderType = orderPtr->GetTWSEOrderType(UFC::rwNotLock);
        int orderQty = orderPtr->GetOrderQty(UFC::rwNotLock);
        int origLeavesQty = orderPtr->GetLeavesOrderQty(UFC::rwNotLock);
        int origCumQty = orderPtr->GetCumQty(UFC::rwNotLock);
        double origCumAmount = orderPtr->GetCumAmount(UFC::rwNotLock);
        double amountPrice = orderPtr->GetAmountPrice(UFC::rwNotLock);
        double origOrderAmount = orderPtr->GetOrderAmount(UFC::rwNotLock);
        TWSE::TMessageType msgType = orderPtr->GetMsgType(UFC::rwNotLock);
        FLogPtr->fprintf(" %s OrderRec[%s] Stock[%s] OrderQty[%d] CumQty[%d] LeavesQty[%d] .", logHead.c_str(), orderID.c_str(), stockNo.c_str(), orderQty, origCumQty, origLeavesQty);
        
        if (origLeavesQty > 0)
        {
            if (origLeavesQty >= matchQty)
            {
                int newLeavesQty = origLeavesQty - matchQty;
                if (newLeavesQty < 0)
                {
                    newLeavesQty = 0;
                    FLogPtr->fprintf(" %s Order[%s] Stock[%s] MatchQty[%d] > LeavesQty[%d].", logHead.c_str(), orderID.c_str(), stockNo.c_str(), matchQty, origLeavesQty);
                }
                orderPtr->SetLeavesQty(newLeavesQty, UFC::rwNotLock);
                int newCumQty = origCumQty + matchQty;
                orderPtr->SetCumQty(newCumQty, UFC::rwNotLock);
                double newCumAmount = origCumAmount + matchAmount;
                orderPtr->SetCumAmount(newCumAmount, UFC::rwNotLock);
                double newOrderAmount = amountPrice * newLeavesQty;
                orderPtr->SetOrderAmount(newOrderAmount, UFC::rwNotLock);
                orderPtr->UnLockForWrite();
                FLogPtr->fprintf(" %s Order[%s] Stock[%s] Set CumQty from [%d] to [%d], LeavesQty from [%d] to [%d], CumAmount from [%15.4lf] to [%15.4lf], OrderAmount from [%15.4lf] to [%15.4lf].",
                                 logHead.c_str(), orderID.c_str(), stockNo.c_str(), origCumQty, newCumQty, origLeavesQty, newLeavesQty, origCumAmount, newCumAmount, origOrderAmount, newOrderAmount);
                
                CRMInvestorAccount *accountPtr = gMegaRMDataPoolPtr->FindAccountFromNoIndex(accountNo, UFC::rwLockForRead);
                if (accountPtr != 0)
                {
                    accountPtr->LockForWrite();
                    if (side == 'B')
                    {
                        double origBuyMatchAmount = accountPtr->GetBuyMatchAmount(UFC::rwNotLock);
                        double newBuyMatchAmount = origBuyMatchAmount + matchAmount;
                        accountPtr->SetBuyMatchAmount(newBuyMatchAmount, UFC::rwNotLock);
                                                
                        double origBuyOrderAmount = accountPtr->GetBuyOrderAmount(UFC::rwNotLock);
                        double newBuyOrderAmount = origBuyOrderAmount - origOrderAmount + newOrderAmount;
                        if (newBuyOrderAmount < 0.0)
                        {
                            newBuyOrderAmount = 0;
                            FLogPtr->fprintf(" %s not enough BuyOrderAmount[%15.4lf], Set BuyOrderAmount to 0.0.", logHead.c_str(), origBuyOrderAmount);
                        }
                        accountPtr->SetBuyOrderAmount(newBuyOrderAmount, UFC::rwNotLock);
                        FLogPtr->fprintf(" %s Set Account[%s] BuyMatchAmount from [%15.4lf] to [%15.4lf], BuyOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origBuyMatchAmount, newBuyMatchAmount, origBuyOrderAmount, newBuyOrderAmount);                        
                    }
                    else
                    {
                        double origSellMatchAmount = accountPtr->GetSellMatchAmount(UFC::rwNotLock);
                        double newSellMatchAmount = origSellMatchAmount + matchAmount;
                        accountPtr->SetSellMatchAmount(newSellMatchAmount, UFC::rwNotLock);

                        double origSellOrderAmount = accountPtr->GetSellOrderAmount(UFC::rwNotLock);
                        double newSellOrderAmount = origSellOrderAmount - origOrderAmount + newOrderAmount;
                        if (newSellOrderAmount < 0.0)
                        {
                            newSellOrderAmount = 0;
                            FLogPtr->fprintf(" %s not enough SellOrderAmount[%15.4lf], Set SellOrderAmount to 0.0.", logHead.c_str(), origSellOrderAmount);
                        }
                        accountPtr->SetSellOrderAmount(newSellOrderAmount, UFC::rwNotLock);
                        FLogPtr->fprintf(" %s Set Account[%s] SellMatchAmount from [%15.4lf] to [%15.4lf], SellOrderAmount from [%15.4lf] to [%15.4lf].", logHead.c_str(), accountNo.c_str(), origSellMatchAmount, newSellMatchAmount, origSellOrderAmount, newSellOrderAmount);                        
                    }  //if (side == 'B')
                    FLogPtr->fprintf(" %s %s.", logHead.c_str(), accountPtr->ToString(UFC::rwNotLock).c_str());
                    CRMStockTradingData *stockDataPtr = accountPtr->FindStockTradingData(stockNo, UFC::rwNotLock);
                    accountPtr->UnLockForWrite();
                    
                    if (stockDataPtr != 0)
                    {
                        stockDataPtr->LockForWrite();
                        if (newLeavesQty <= 0)
                        {
                            CRMOrderSummary *filledOrderPtr = 0;
                            if (side == 'B')
                            {
                                if (msgType == TWSE::mtT010)
                                    filledOrderPtr = stockDataPtr->RemoveRegularBuyOrder(orderID, UFC::rwNotLock);
                                else if (msgType == TWSE::mtO110)
                                    filledOrderPtr = stockDataPtr->RemoveIntraDayOddBuyOrder(orderID, UFC::rwNotLock);
                                else
                                    filledOrderPtr = stockDataPtr->RemoveBuyOrder(orderID, UFC::rwNotLock);
                            }
                            else
                            {
                                if (msgType == TWSE::mtT010)
                                    filledOrderPtr = stockDataPtr->RemoveRegularSellOrder(orderID, UFC::rwNotLock);
                                else if (msgType == TWSE::mtO110)
                                    filledOrderPtr = stockDataPtr->RemoveIntraDayOddSellOrder(orderID, UFC::rwNotLock);
                                else
                                    filledOrderPtr = stockDataPtr->RemoveSellOrder(orderID, UFC::rwNotLock);
                            }
                    
                            if (filledOrderPtr != 0)
                                FLogPtr->fprintf(" %s StockData: %s, Match Order[%s] Qty[%d], ExecKey[%s]", logHead.c_str(), stockDataPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), matchQty, ExecKey.c_str());
                            else
                                FLogPtr->fprintf(" %s Remove Order[%s] from account[%s] Stock[%s] failed, Match Order[%s], ExecKey[%s]", logHead.c_str(), orderID.c_str(), accountNo.c_str(), stockNo.c_str(), orderID.c_str(), ExecKey.c_str());
                        }  //if (newLeavesQty <= 0)                    
                        FLogPtr->fprintf(" %s StockData: %s, Order[%s], msgType[%d].", logHead.c_str(), stockDataPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), msgType);
                        CRMBrokerStockCondition *conditionPtr = stockDataPtr->FindCondition(warrantNo, brokerID, twseOrderType, UFC::rwNotLock);
                        stockDataPtr->UnLockForWrite();
                        
                        if (conditionPtr != 0)
                        {
                            conditionPtr->LockForWrite();
                            if (side == 'B')
                            {
                                if (msgType == TWSE::mtT010)
                                    conditionPtr->IncreaseRegularMatchBuyQty(matchQty, UFC::rwNotLock);
                                else if (msgType == TWSE::mtO110)
                                    conditionPtr->IncreaseIntraDayOddMatchBuyQty(matchQty, UFC::rwNotLock);
                                else
                                    conditionPtr->IncreaseMatchBuyQty(matchQty, UFC::rwNotLock);
                            }
                            else
                            {
                                if (msgType == TWSE::mtT010)
                                    conditionPtr->IncreaseRegularMatchSellQty(matchQty, UFC::rwNotLock);
                                else if (msgType == TWSE::mtO110)
                                    conditionPtr->IncreaseIntraDayOddMatchSellQty(matchQty, UFC::rwNotLock);
                                else
                                    conditionPtr->IncreaseMatchSellQty(matchQty, UFC::rwNotLock);
                            }
                            FLogPtr->fprintf(" %s Condition: %s, Match Order[%s] Qty[%d], ExecKey[%s]", logHead.c_str(), conditionPtr->ToString(UFC::rwNotLock).c_str(), orderID.c_str(), matchQty, ExecKey.c_str());
                            conditionPtr->UnLockForWrite();
                        }
                        else
                            FLogPtr->fprintf(" %s Account[%s] Stock[%s][%s][%s]%c not found, Match Order[%s]", logHead.c_str(), accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerID.c_str(), twseOrderType, orderID.c_str());
                    }
                    else
                        FLogPtr->fprintf(" %s Account[%s] has no Stock[%s] data, Match Order[%s]", logHead.c_str(), accountNo.c_str(), stockNo.c_str(), orderID.c_str());
                }
                else
                    FLogPtr->fprintf(" %s Account[%s] of Match Order[%s] not found.", logHead.c_str(), accountNo.c_str(), orderID.c_str());
            }
            else
            {
                orderPtr->UnLockForWrite();
                FLogPtr->fprintf(" %s Wrong Order[%s]. LeavesQty[%d] < MatchQty[%d].", logHead.c_str(), orderID.c_str(), origLeavesQty, matchQty);
            }
        }
        else
        {
            orderPtr->UnLockForWrite();
            FLogPtr->fprintf(" %s Order[%s] is Filled. LeavesQty[%d] <= 0.", logHead.c_str(), orderID.c_str(), origLeavesQty);
        }
    }
    else
        FLogPtr->fprintf(" %s Order[%s] not Found.", logHead.c_str(), orderID.c_str());
    FLogPtr->fprintf(" %s", logHead.c_str());
    FLogPtr->Flush();
}  //CMegaRiskManagerClass::AddRiskMatchQty()
//------------------------------------------------------------------------------
void CMegaRiskManagerClass::Execute(void)
{
    UFC::AnsiString logHead = "CMegaRiskManagerClass::Execute()";
    int loopCount = 0;
    CRiskManagerDataPool::LoadStockDataFile();
//    CRiskManagerDataPool::DumpStockData();
    CMegaRiskManagerDataPool::LoadRiskManagerHedgeStock();
    
    int checkFileInterval = gMegaRMDataPoolPtr->GetCheckFileInterval();
    if (gMegaRMDataPoolPtr->LoadRiskManagerControlData(false))
    {
        gMegaRMDataPoolPtr->IncreaseRMCurrentControlFileSeqNo();
        while (gMegaRMDataPoolPtr->LoadRiskManagerControlData(true))
            gMegaRMDataPoolPtr->IncreaseRMCurrentControlFileSeqNo();
    }
    gMegaRMDataPoolPtr->DumpAccountNoIndex(true, UFC::rwLockForRead);  //DumpDetail
    
    if (FIsRecoverMode) RecoverRMData();
    
    UFC::AnsiString stockWhiteListFilePathName = CMegaRiskManagerDataPool::GetRMStockWhiteListFilePathName();
    if ((stockWhiteListFilePathName.Length() > 0) && (UFC::FileExists(stockWhiteListFilePathName) == TRUE))
    {
        if (UFC::GetFileModificationSecond(stockWhiteListFilePathName, FWhiteListFileSecond))
        {
            gMegaRMDataPoolPtr->LoadStockWhiteList();
            gMegaRMDataPoolPtr->DumpAccountStockWhiteList(UFC::rwLockForRead);
            FLogPtr->fprintf(" CMegaRiskManagerClass::%s() WhiteList Time[%lld]", __func__, FWhiteListFileSecond);
            FLogPtr->fprintf(" CMegaRiskManagerClass::%s()", __func__);
        }
    }

    while (gMegaRMDataPoolPtr->LoadOrderDeleteData()) gMegaRMDataPoolPtr->IncreaseRMCurrentDeleteFileSeqNo();    
    
    while (!IsTerminated())
    {
        loopCount++;
        FLogPtr->fprintf(" CMegaRiskManagerClass::%s() Loop %d", __func__, loopCount);
        
        bool isLoadControlFileSuccess = false;
        if (gMegaRMDataPoolPtr->GetRMCurrentControlFileSeqNo() <= 0)
            isLoadControlFileSuccess = gMegaRMDataPoolPtr->LoadRiskManagerControlData(false);
        else
            isLoadControlFileSuccess = gMegaRMDataPoolPtr->LoadRiskManagerControlData(true);
        if (isLoadControlFileSuccess)
        {
            gMegaRMDataPoolPtr->IncreaseRMCurrentControlFileSeqNo();
            gMegaRMDataPoolPtr->DumpAccountNoIndex(true, UFC::rwLockForRead);  //DumpDetail
        }
        
        if ((stockWhiteListFilePathName.Length() > 0) && (UFC::FileExists(stockWhiteListFilePathName) == TRUE))
        {
            long long newWhiteListFileSecond;
            if (UFC::GetFileModificationSecond(stockWhiteListFilePathName, newWhiteListFileSecond))
            {
                if (newWhiteListFileSecond > FWhiteListFileSecond)
                {
                    if (gMegaRMDataPoolPtr->LoadStockWhiteList())
                    {
                        FWhiteListFileSecond = newWhiteListFileSecond;
                        gMegaRMDataPoolPtr->DumpAccountStockWhiteList(UFC::rwLockForRead);
                        FLogPtr->fprintf(" CMegaRiskManagerClass::%s() WhiteList Time[%lld]", __func__, FWhiteListFileSecond);
                        FLogPtr->fprintf(" CMegaRiskManagerClass::%s()", __func__);
                    }
                }
            }
        }

        if (gMegaRMDataPoolPtr->LoadOrderDeleteData())
            gMegaRMDataPoolPtr->IncreaseRMCurrentDeleteFileSeqNo();

        FLogPtr->Flush();
        UFC::SleepMS(checkFileInterval);
    }  //while (!IsTerminated())
}  //CMegaRiskManagerClass::Execute()
}  //namespace twserm
