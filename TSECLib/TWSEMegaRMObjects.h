/* 
 * File:   TWSEMegaRMObjects.h
 * Author: Zhen Fan
 *
 * Created on 2021/01/15 11:40
 */

#ifndef TWSEMEGARMOBJECTS_H
#define TWSEMEGARMOBJECTS_H
#include "UFC.h"                //UFC
#include "ContainerTemplate.h"  //UFC

#include "MessageHeader.h"  //TSECLib
#include "TMessages.h"      //TSECLib
#include "PMessages.h"      //TSECLib
#include "OMessages.h"      //TSECLib
#include "O1Messages.h"     //TSECLib

namespace twserm
{
//------------------------------------------------------------------------------
class CRMHedgeStock
{
protected:
    UFC::AnsiString FStockNo;
    
    UFC::PHashedSet<UFC::AnsiString> FAccountSet;
    
public:
    CRMHedgeStock(const UFC::AnsiString& StockNo);
    
    int GetAccountCount() {return FAccountSet.ItemCount();};
    bool IsAccountExists(const UFC::AnsiString& AccountNo);
    bool AddAccountToSet(const UFC::AnsiString& AccountNo);
    UFC::AnsiString ToAccountCSV();
    UFC::AnsiString ToString();
};  //CRMHedgeStock

//------------------------------------------------------------------------------
class CRMOrderSummary : public UFC::CMainKeyObject
{
public:
    static void ParseUserData(const UFC::AnsiString& OrderKey, UFC::AnsiString& WarrantNo, UFC::AnsiString& BrokerID);
    static double Parse95v9999PriceStr(const UFC::AnsiString& PriceStr);

protected:
    Int32              FNetworkID;
    int                FFunctionCode;
    UFC::AnsiString    FOrderKey;
    TWSE::TMessageType FMsgType;
    
    UFC::AnsiString    FBrokerID;         //券商代號
    UFC::AnsiString    FOrderID;          //委託書編號
    UFC::AnsiString    FAccountNo;        //集保帳號
    UFC::AnsiString    FStockNo;          //股票代號
    double             FOrderPrice;       //委託價
    int                FOrigOrderQty;     //原始委託量
    int                FOrderQty;         //委託量
    char               FSide;             //'B':Bye 'S':Sell
    char               FExchangeCode;     //交易碼   '0':一般 '1'：鉅額 '2'：零股 '3'：外國股票價確認碼
    char               FTWSEOrderType;    //委託類別 '0':一般 '1':證金融資 '2':證金融券 '3':自辦融資 '4':自辦融券 '5''6':借券賣出
    char               FTWSEPriceType;    //委託價格種類 '1':市價 '2':限價
    char               FTWSETimeInForce;  //委託時效 '0':當日有效 '3':IOC '4':FOK
    
    nsOrderMessageDefine::TradingSessionIDEnum FTradingSessionID;
    UFC::AnsiString                            FSessionName;
    
    UFC::AnsiString    FWarrantNo;        //權證別
    int                FLeavesQty;        //剩餘量
    int                FCumQty;           //成交量
    double             FAmountPrice;
    double             FOrderAmount;      //委託額度
    double             FCumAmount;        //成交額度
    int                FTradeUnit;        //每一交易單位所含股數
    double             FOrigOrderPrice;
    double             FOrigOrderAmount;
    
    bool FIsPassed;  //For Try Order
    
protected:
    void SetRegularOrderFields(TWSE::MessageT010* T010MsgPtr);
    void SetInterDayOddOrderFields(TWSE::MessageO110* O110MsgPtr);
    void SetOddOrderFields(TWSE::MessageO010* O010MsgPtr);
    void SetFixPriceOrderFields(TWSE::MessageP010* P010MsgPtr);
    
public:
    CRMOrderSummary(int FunctionCode, TWSE::TMessageType MsgType, const UFC::AnsiString& OrderKey, TWSE::MessageHeader*& OrderMsg);
    CRMOrderSummary(const UFC::AnsiString& OrderID, char Side, const UFC::AnsiString& StockNo,
                    const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, char TWSEOrderType, 
                    double FOrderPrice, int OrderQty, char ExchangeCode, TWSE::TMessageType MsgType, const UFC::AnsiString& AccountNo = "");
        
    Int32 GetNetworkID(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FNetworkID, LockType);};
    void SetNetworkID(Int32 NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FNetworkID, LockType);};
    
    int GetFunctionCode(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FFunctionCode, LockType);};
    void SetFunctionCode(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FFunctionCode, LockType);};

    UFC::AnsiString GetOrderKey(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FOrderKey, LockType);};
    void SetOrderKey(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FOrderKey, LockType);};

    TWSE::TMessageType GetMsgType(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FMsgType, LockType);};
    void SetMsgType(TWSE::TMessageType NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FMsgType, LockType);};
    
    UFC::AnsiString GetBrokerID(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FBrokerID, LockType);};
    void SetBrokerID(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FBrokerID, LockType);};
    
    UFC::AnsiString GetOrderID(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FOrderID, LockType);};
    void SetOrderID(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FOrderID, LockType);};

    UFC::AnsiString GetAccountNo(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FAccountNo, LockType);};
    void SetAccountNo(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FAccountNo, LockType);};

    UFC::AnsiString GetStockNo(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FStockNo, LockType);};
    void SetStockNo(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FStockNo, LockType);};
    
    double GetOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FOrderPrice, LockType);};
    void SetOrderPrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FOrderPrice, LockType);};
    
    int GetOrigOrderQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FOrigOrderQty, LockType);};
    void SetOrigOrderQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FOrigOrderQty, LockType);};
    
    int GetOrderQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FOrderQty, LockType);};
    void SetOrderQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FOrderQty, LockType);};
    int IncreaseOrderQty(int IncreaseQty, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    
    char GetSide(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FSide, LockType);};
    void SetSide(char NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FSide, LockType);};

    char GetExchangeCode(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FExchangeCode, LockType);};
    void SetExchangeCode(char NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FExchangeCode, LockType);};

    char GetTWSEOrderType(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FTWSEOrderType, LockType);};
    void SetTWSEOrderType(char NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FTWSEOrderType, LockType);};

    char GetTWSEPriceType(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FTWSEPriceType, LockType);};
    void SetTWSEPriceType(char NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FTWSEPriceType, LockType);};

    char GetTWSETimeInForce(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FTWSETimeInForce, LockType);};
    void SetTWSETimeInForce(char NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FTWSETimeInForce, LockType);};

    nsOrderMessageDefine::TradingSessionIDEnum GetSessionID(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FTradingSessionID, LockType);};
    void SetSessionID(nsOrderMessageDefine::TradingSessionIDEnum NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FTradingSessionID, LockType);};
    
    UFC::AnsiString GetSessionName(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FSessionName, LockType);};
    void SetSessionName(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FSessionName, LockType);};
    
    UFC::AnsiString GetWarrantNo(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FWarrantNo, LockType);};
    void SetWarrantNo(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FWarrantNo, LockType);};
    
    int GetLeavesOrderQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FLeavesQty, LockType);};
    void SetLeavesQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FLeavesQty, LockType);};
    int IncreaseLeavesQty(int IncreaseQty, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);

    int GetCumQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FCumQty, LockType);};
    void SetCumQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FCumQty, LockType);};
    int IncreaseCumQty(int IncreaseQty, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    
    double GetAmountPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FAmountPrice, LockType);};
    void SetAmountPrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FAmountPrice, LockType);};
    
    double GetOrderAmount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FOrderAmount, LockType);};
    void SetOrderAmount(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FOrderAmount, LockType);};
    
    double GetCumAmount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FCumAmount, LockType);};
    void SetCumAmount(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FCumAmount, LockType);};
    
    int GetTradeUnit(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FTradeUnit, LockType);};
    void SetTradeUnit(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);

    double GetOrigOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FOrigOrderPrice, LockType);};
    void SetOrigOrderPrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FOrigOrderPrice, LockType);};
    
    double GetOrigOrderAmount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FOrigOrderAmount, LockType);};
    void SetOrigOrderAmount(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FOrigOrderAmount, LockType);};
    
    bool IsPassed(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FIsPassed, LockType);};
    void SetPassed(bool NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FIsPassed, LockType);};
    
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
};  //CRMOrderSummary

//------------------------------------------------------------------------------
class CRMBrokerStockCondition : public UFC::CMainKeyObject
{
public:
    static UFC::AnsiString BuildIndexKey(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, char TWSEOrderType);
    
protected:
    UFC::AnsiString FWarrantNo;         //權證別
    UFC::AnsiString FBrokerID;          //券商代號
    char            FTWSEOrderType;     //'0':一般 '1':融資(證金) '2':融券(證金) '3':融資(自辦) '4':融券(自辦) '5':借券賣出(一般策略) '6':借券賣出(權證策略)    
    int             FAvailableSellQty;  //可賣股數
    int             FAvailableBuyQty;   //可買股數
    UFC::AnsiString FDataDate;          //資料日期
    bool            FIsSellShort;       //是否為借券
    UFC::AnsiString FAccountNo;         //集保帳號
    UFC::AnsiString FStockNo;           //股票代號
    
    int FRegularMatchBuyQty;       //今日日盤整股買進成交股數
    int FRegularMatchSellQty;      //今日日盤整股賣出成交股數
    int FIntraDayOddMatchBuyQty;   //今日日盤零股買進成交股數
    int FIntraDayOddMatchSellQty;  //今日日盤零股賣出成交股數
    int FMatchBuyQty;              //今日買進成交股數
    int FMatchSellQty;             //今日賣出成交股數
    
    int FRegularOrderBuyQty;       //今日日盤整股委託買進股數
    int FRegularOrderSellQty;      //今日日盤整股委託賣出股數
    int FIntraDayOddOrderBuyQty;   //今日日盤零股委託買進股數
    int FIntraDayOddOrderSellQty;  //今日日盤零股委託賣出股數
    int FOrderBuyQty;              //今日委託買進股數
    int FOrderSellQty;             //今日委託賣出股數
    
public:
    CRMBrokerStockCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, char TWSEOrderType,
                            int AvailableSellQty, int AvailableBuyQty,
                            const UFC::AnsiString& DataDate = "", bool IsSellShort = false);
    
    UFC::AnsiString GetWarrantNo(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FWarrantNo, LockType);};
    UFC::AnsiString GetBrokerID(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FBrokerID, LockType);};
    char GetTWSEOrderType(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FTWSEOrderType, LockType);};

    int GetAvailableSellQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FAvailableSellQty, LockType);};
    void SetAvailableSellQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FAvailableSellQty, LockType);};

    int GetAvailableBuyQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FAvailableBuyQty, LockType);};
    void SetAvailableBuyQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FAvailableBuyQty, LockType);};
    
    UFC::AnsiString GetDataDate(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FDataDate, LockType);};
    void SetDataDate(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FDataDate, LockType);};

    bool IsSellShort(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FIsSellShort, LockType);};
    void SetSellShort(bool IsSellShort, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(IsSellShort, FIsSellShort, LockType);};
    
    UFC::AnsiString GetAccountNo(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FAccountNo, LockType);};
    void SetAccountNo(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FAccountNo, LockType);};

    UFC::AnsiString GetStockNo(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FStockNo, LockType);};
    void SetStockNo(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FStockNo, LockType);};
    
    int GetRegularMatchBuyQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FRegularMatchBuyQty, LockType);};
    void SetRegularMatchBuyQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FRegularMatchBuyQty, LockType);};
    int IncreaseRegularMatchBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    
    int GetRegularMatchSellQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FRegularMatchSellQty, LockType);};
    void SetRegularMatchSellQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FRegularMatchSellQty, LockType);};
    int IncreaseRegularMatchSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    
    int GetIntraDayOddMatchBuyQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FIntraDayOddMatchBuyQty, LockType);};
    void SetIntraDayOddMatchBuyQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FIntraDayOddMatchBuyQty, LockType);};
    int IncreaseIntraDayOddMatchBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);

    int GetIntraDayOddMatchSellQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FIntraDayOddMatchSellQty, LockType);};
    void SetIntraDayOddMatchSellQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FIntraDayOddMatchSellQty, LockType);};
    int IncreaseIntraDayOddMatchSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    
    int GetMatchBuyQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FMatchBuyQty, LockType);};
    void SetMatchBuyQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FMatchBuyQty, LockType);};
    int IncreaseMatchBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);

    int GetMatchSellQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FMatchSellQty, LockType);};
    void SetMatchSellQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FMatchSellQty, LockType);};
    int IncreaseMatchSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    
    int GetRegularOrderBuyQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FRegularOrderBuyQty, LockType);};
    void SetRegularOrderBuyQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FRegularOrderBuyQty, LockType);};
    int IncreaseRegularOrderBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    
    int GetRegularOrderSellQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FRegularOrderSellQty, LockType);};
    void SetRegularOrderSellQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FRegularOrderSellQty, LockType);};
    int IncreaseRegularOrderSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    
    int GetIntraDayOddOrderBuyQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FIntraDayOddOrderBuyQty, LockType);};
    void SetIntraDayOddOrderBuyQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FIntraDayOddOrderBuyQty, LockType);};
    int IncreaseIntraDayOddOrderBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    
    int GetIntraDayOddOrderSellQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FIntraDayOddOrderSellQty, LockType);};
    void SetIntraDayOddOrderSellQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FIntraDayOddOrderSellQty, LockType);};
    int IncreaseIntraDayOddOrderSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    
    int GetOrderBuyQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FOrderBuyQty, LockType);};
    void SetOrderBuyQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FOrderBuyQty, LockType);};
    int IncreaseOrderBuyQty(int IncreaseQty, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    
    int GetOrderSellQty(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FOrderSellQty, LockType);};
    void SetOrderSellQty(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FOrderSellQty, LockType);};
    int IncreaseOrderSellQty(int IncreaseQty, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);

    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
};  //CRMBrokerStockCondition

//------------------------------------------------------------------------------
class CRMStockTradingData : public UFC::CMainKeyObject
{
protected:
    UFC::AnsiString FStockNo;                        //股票代號
    
    CRMOrderSummary* FMaxRegularBuyOrderPtr;         //最高日盤整股買進委託單    
    double           FMaxRegularBuyOrderPrice;       //最高日盤整股買進委託價
    CRMOrderSummary* FMinRegularSellOrderPtr;        //最低日盤整股賣出委託單
    double           FMinRegularSellOrderPrice;      //最低日盤整股賣出委託價

    CRMOrderSummary* FMaxIntraDayOddBuyOrderPtr;     //最高日盤零股買進委託單
    double           FMaxIntraDayOddBuyOrderPrice;   //最高日盤零股買進委託價
    CRMOrderSummary* FMinIntraDayOddSellOrderPtr;    //最低日盤零股賣出委託單
    double           FMinIntraDayOddSellOrderPrice;  //最低日盤零股賣出委託價
    
    CRMOrderSummary* FMaxBuyOrderPtr;                //最高買進委託單
    double           FMaxBuyOrderPrice;              //最高買進委託價
    CRMOrderSummary* FMinSellOrderPtr;               //最低賣出委託單
    double           FMinSellOrderPrice;             //最低賣出委託價
    
    double FBuyAmount;
    double FSellAmount;

    UFC::AnsiString FAccountNo;
    
    UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*> FRegularBuyOrderIDIndex;
    UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*> FRegularSellOrderIDIndex;
    UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*> FIntraDayOddBuyOrderIDIndex;
    UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*> FIntraDayOddSellOrderIDIndex;
    UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*> FBuyOrderIDIndex;
    UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*> FSellOrderIDIndex;
    
    UFC::PHashMap<UFC::AnsiString, CRMBrokerStockCondition*> FConditionWarrantBrokerOrdTpeIndex;
    
    CRMOrderSummary *FindMaxOrderPriceOrder(UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*>& OrderIDIndex, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    double FindMaxRegularBuyOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    double FindMaxIntraDayOddBuyOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    double FindMaxBuyOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    CRMOrderSummary *FindMinOrderPriceOrder(UFC::PHashMap<UFC::AnsiString, CRMOrderSummary*>& OrderIDIndex, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    double FindMinRegularSellOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    double FindMinIntraDayOddSellOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    double FindMinSellOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
        
public:
    CRMStockTradingData(const UFC::AnsiString& StockNo);
    ~CRMStockTradingData();
    
    virtual CRMOrderSummary *GetMaxRegularBuyOrder(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FMaxRegularBuyOrderPtr, LockType);};
    virtual double GetMaxRegularBuyOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FMaxRegularBuyOrderPrice, LockType);};
    virtual void SetMaxRegularBuyOrderPrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FMaxRegularBuyOrderPrice, LockType);};

    virtual CRMOrderSummary *GetMinRegularSellOrder(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FMinRegularSellOrderPtr, LockType);};
    virtual double GetMinRegularSellOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FMinRegularSellOrderPrice, LockType);};
    virtual void SetMinRegularSellOrderPrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FMinRegularSellOrderPrice, LockType);};

    virtual CRMOrderSummary *GetMaxIntraDayOddBuyOrder(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FMaxIntraDayOddBuyOrderPtr, LockType);};
    virtual double GetMaxIntraDayOddBuyOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FMaxIntraDayOddBuyOrderPrice, LockType);};
    virtual void SetMaxIntraDayOddBuyOrderPrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FMaxIntraDayOddBuyOrderPrice, LockType);};

    virtual CRMOrderSummary *GetMinIntraDayOddSellOrder(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FMinIntraDayOddSellOrderPtr, LockType);};
    virtual double GetMinIntraDayOddSellOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FMinIntraDayOddSellOrderPrice, LockType);};
    virtual void SetMinIntraDayOddSellOrderPrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FMinIntraDayOddSellOrderPrice, LockType);};
    
    virtual CRMOrderSummary *GetMaxBuyOrder(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FMaxBuyOrderPtr, LockType);};
    virtual double GetMaxBuyOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FMaxBuyOrderPrice, LockType);};
    virtual void SetMaxBuyOrderPrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FMaxBuyOrderPrice, LockType);};
    
    virtual CRMOrderSummary *GetMinSellOrder(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FMinSellOrderPtr, LockType);};
    virtual double GetMinSellOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FMinSellOrderPrice, LockType);};
    virtual void SetMinSellOrderPrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FMinSellOrderPrice, LockType);};
        
    virtual double GetBuyAmount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FBuyAmount, LockType);};
    virtual void SetBuyAmount(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FBuyAmount, LockType);};
    
    virtual double GetSellAmount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FSellAmount, LockType);};
    virtual void SetSellAmount(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FSellAmount, LockType);};
    
    virtual UFC::AnsiString GetAccountNo(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FAccountNo, LockType);};
    virtual void SetAccountNo(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FAccountNo, LockType);};
    
    int GetRegularBuyOrderCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) 
        {return UFC::GetStrIdxHMapElementPtrCount(FRegularBuyOrderIDIndex, FRWLockPtr, LockType);};
    CRMOrderSummary* FindRegularBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::FindElementPtrFromStrIdxHMap(FRegularBuyOrderIDIndex, OrderID, FRWLockPtr, LockType);};
    bool InsertRegularBuyOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    CRMOrderSummary* RemoveRegularBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    void UpdateMaxRegularBuyOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    int ClearRegularBuyOrder(bool DoDelete, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::ClearElementPtrStrIdxHMap(FRegularBuyOrderIDIndex, DoDelete, FRWLockPtr, LockType);};

    int GetRegularSellOrderCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) 
        {return UFC::GetStrIdxHMapElementPtrCount(FRegularSellOrderIDIndex, FRWLockPtr, LockType);};
    CRMOrderSummary* FindRegularSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::FindElementPtrFromStrIdxHMap(FRegularSellOrderIDIndex, OrderID, FRWLockPtr, LockType);};
    bool InsertRegularSellOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    CRMOrderSummary* RemoveRegularSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    void UpdateMinRegularSellOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    int ClearRegularSellOrder(bool DoDelete, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::ClearElementPtrStrIdxHMap(FRegularSellOrderIDIndex, DoDelete, FRWLockPtr, LockType);};
    
    int GetIntraDayOddBuyOrderCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) 
        {return UFC::GetStrIdxHMapElementPtrCount(FIntraDayOddBuyOrderIDIndex, FRWLockPtr, LockType);};
    CRMOrderSummary* FindIntraDayOddBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::FindElementPtrFromStrIdxHMap(FIntraDayOddBuyOrderIDIndex, OrderID, FRWLockPtr, LockType);};
    bool InsertIntraDayOddBuyOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    CRMOrderSummary* RemoveIntraDayOddBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    void UpdateMaxIntraDayOddBuyOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    int ClearIntraDayOddBuyOrder(bool DoDelete, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::ClearElementPtrStrIdxHMap(FIntraDayOddBuyOrderIDIndex, DoDelete, FRWLockPtr, LockType);};
    
    int GetIntraDayOddSellOrderCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) 
        {return UFC::GetStrIdxHMapElementPtrCount(FIntraDayOddSellOrderIDIndex, FRWLockPtr, LockType);};
    CRMOrderSummary* FindIntraDayOddSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::FindElementPtrFromStrIdxHMap(FIntraDayOddSellOrderIDIndex, OrderID, FRWLockPtr, LockType);};
    bool InsertIntraDayOddSellOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    CRMOrderSummary* RemoveIntraDayOddSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    void UpdateMinIntraDayOddSellOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    int ClearIntraDayOddSellOrder(bool DoDelete, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::ClearElementPtrStrIdxHMap(FIntraDayOddSellOrderIDIndex, DoDelete, FRWLockPtr, LockType);};
    
    int GetBuyOrderCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) 
        {return UFC::GetStrIdxHMapElementPtrCount(FBuyOrderIDIndex, FRWLockPtr, LockType);};
    CRMOrderSummary* FindBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::FindElementPtrFromStrIdxHMap(FBuyOrderIDIndex, OrderID, FRWLockPtr, LockType);};
    bool InsertBuyOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    CRMOrderSummary* RemoveBuyOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    void UpdateMaxBuyOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    int ClearBuyOrder(bool DoDelete, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::ClearElementPtrStrIdxHMap(FBuyOrderIDIndex, DoDelete, FRWLockPtr, LockType);};
    
    int GetSellOrderCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) 
        {return UFC::GetStrIdxHMapElementPtrCount(FSellOrderIDIndex, FRWLockPtr, LockType);};
    CRMOrderSummary* FindSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::FindElementPtrFromStrIdxHMap(FSellOrderIDIndex, OrderID, FRWLockPtr, LockType);};
    bool InsertSellOrder(const UFC::AnsiString& OrderID, CRMOrderSummary *OrderPtr, bool CheckExist, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    CRMOrderSummary* RemoveSellOrder(const UFC::AnsiString& OrderID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    void UpdateMinSellOrderPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    int ClearSellOrder(bool DoDelete, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::ClearElementPtrStrIdxHMap(FSellOrderIDIndex, DoDelete, FRWLockPtr, LockType);};
    
    int GetConditionCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) 
        {return UFC::GetStrIdxHMapElementPtrCount(FConditionWarrantBrokerOrdTpeIndex, FRWLockPtr, LockType);};
    CRMBrokerStockCondition* FindCondition(const UFC::AnsiString& KeyValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::FindElementPtrFromStrIdxHMap(FConditionWarrantBrokerOrdTpeIndex, KeyValue, FRWLockPtr, LockType);};
    bool InsertCondition(const UFC::AnsiString& KeyValue, CRMBrokerStockCondition *ConditionPtr, bool CheckExist, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::InsertElementPtrIntoStrIdxHMap(FConditionWarrantBrokerOrdTpeIndex, KeyValue, ConditionPtr, CheckExist, FRWLockPtr, LockType);};        
    CRMBrokerStockCondition* RemoveCondition(const UFC::AnsiString& KeyValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::RemoveElementPtrFromStrIdxHMap(FConditionWarrantBrokerOrdTpeIndex, KeyValue, FRWLockPtr, LockType);};
    int ClearCondition(bool DoDelete, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::ClearElementPtrStrIdxHMap(FConditionWarrantBrokerOrdTpeIndex, DoDelete, FRWLockPtr, LockType);};
    CRMBrokerStockCondition* FindCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, char TWSEOrderType, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    bool InsertCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, char TWSEOrderType, CRMBrokerStockCondition *ConditionPtr, bool CheckExist, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    CRMBrokerStockCondition* RemoveCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, char TWSEOrderType, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    void DumpCondition(UFC::BufferedLog* LogPtr, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
        
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
};  //CRMStockTradingData

//------------------------------------------------------------------------------
class CRMInvestorAccount : public UFC::CMainKeyObject
{
protected:
    UFC::AnsiString FAccountNo;                //集保帳號
    int             FDayTradeType;             //0：不可賣當日買股票，1：可賣當日買股票
    bool            FNeedCheckSelfTrade;       //false:不檢查自行成交，true：檢查自行成交
    bool            FNeedCheckSellPosition;    //false:不檢查賣出部位，true：檢查賣出部位
    double          FStockAmountLimit;         //Need Check if StockAmountLimit > 0.0
    bool            FNeedCheckStockWhiteList;  //false:不檢查白名單 true:檢查白名單

    double FPositionAmount;
    double FBuyOrderAmount;
    double FSellOrderAmount;
    double FBuyMatchAmount;
    double FSellMatchAmount;
    
    UFC::PHashMap<UFC::AnsiString, CRMStockTradingData*> FStockNoIndex;
    UFC::PHashedSet<UFC::AnsiString> FStockWhiteListSet;
    
public:
    CRMInvestorAccount(const UFC::AnsiString& AccountNo);
    ~CRMInvestorAccount();
    
    virtual UFC::AnsiString GetAccountNo(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<UFC::AnsiString>(FAccountNo, LockType);};

    virtual int GetDayTradeType(UFC::RWLockTypeEnum LockType = UFC::rwNotLock)  {return GetFieldValue(FDayTradeType, LockType);};
    virtual void SetDayTradeType(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FDayTradeType, LockType);};

    virtual bool NeedCheckSelfTrade(UFC::RWLockTypeEnum LockType = UFC::rwNotLock)  {return GetFieldValue(FNeedCheckSelfTrade, LockType);};
    virtual void SetNeedCheckSelfTrade(bool NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FNeedCheckSelfTrade, LockType);};
    
    virtual bool NeedCheckSellPosition(UFC::RWLockTypeEnum LockType = UFC::rwNotLock)  {return GetFieldValue(FNeedCheckSellPosition, LockType);};
    virtual void SetNeedCheckSellPosition(bool NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FNeedCheckSellPosition, LockType);};
    
    virtual double GetStockAmountLimit(UFC::RWLockTypeEnum LockType = UFC::rwNotLock)  {return GetFieldValue(FStockAmountLimit, LockType);};
    virtual void SetStockAmountLimit(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FStockAmountLimit, LockType);};

    virtual bool NeedCheckStockWhiteList(UFC::RWLockTypeEnum LockType = UFC::rwNotLock)  {return GetFieldValue(FNeedCheckStockWhiteList, LockType);};
    virtual void SetNeedCheckStockWhiteList(bool NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FNeedCheckStockWhiteList, LockType);};
    
    virtual double GetPositionAmount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FPositionAmount, LockType);};
    virtual void SetPositionAmount(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FPositionAmount, LockType);};
    
    virtual double GetBuyOrderAmount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FBuyOrderAmount, LockType);};
    virtual void SetBuyOrderAmount(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FBuyOrderAmount, LockType);};
    
    virtual double GetSellOrderAmount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FSellOrderAmount, LockType);};
    virtual void SetSellOrderAmount(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FSellOrderAmount, LockType);};
    
    virtual double GetBuyMatchAmount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FBuyMatchAmount, LockType);};
    virtual void SetBuyMatchAmount(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FBuyMatchAmount, LockType);};
    
    virtual double GetSellMatchAmount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue(FSellMatchAmount, LockType);};
    virtual void SetSellMatchAmount(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue(NewValue, FSellMatchAmount, LockType);};
    
    int GetStockTradingDataCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) 
        {return UFC::GetStrIdxHMapElementPtrCount(FStockNoIndex, FRWLockPtr, LockType);};
    CRMStockTradingData *FindStockTradingData(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::FindElementPtrFromStrIdxHMap(FStockNoIndex, StockNo, FRWLockPtr, LockType);};
    bool InsertStockTradingData(const UFC::AnsiString& StockNo, CRMStockTradingData *StockDataPtr, bool CheckExist, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::InsertElementPtrIntoStrIdxHMap(FStockNoIndex, StockNo, StockDataPtr, CheckExist, FRWLockPtr, LockType);};        
    CRMStockTradingData* RemoveStockTradingData(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::RemoveElementPtrFromStrIdxHMap(FStockNoIndex, StockNo, FRWLockPtr, LockType);};
    int ClearStockTradingData(bool DoDelete, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::ClearElementPtrStrIdxHMap(FStockNoIndex, DoDelete, FRWLockPtr, LockType);};
    CRMStockTradingData *FindOrCreateStockTradingData(const UFC::AnsiString& StockNo, bool& IsNewStockData, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    CRMStockTradingData *FindOrCreateStockTradingData(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    void DumpStockTradingData(UFC::BufferedLog* LogPtr, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
        
    int GetStockWhiteListCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    bool IsStockInWhiteList(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    void AddStockToWhiteList(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    void ClearStockWhiteList(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    UFC::AnsiString GetStockWhiteListCSV(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);

    bool CheckOrderAmount(const UFC::AnsiString& OrderID, char BuySellCode, double OrderAmount, UFC::AnsiString& CheckMessage, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);

    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
};  //CRMInvestorAccount
}  //namespace twserm
#endif /* TWSEMEGARMOBJECTS_H */
