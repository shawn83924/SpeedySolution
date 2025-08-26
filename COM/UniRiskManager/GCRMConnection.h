// 
// File:   YuantaExecConnection.h
// Author: yuan
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _SinoPacExecConnection_H
#define	_SinoPacExecConnection_H
//------------------------------------------------------------------------------
#include <map>
#include "Sigo.h"
#include "UFC.h" 
#include "iniFile.h"
#include "TFixedFormat.h"
#include "TParseData.h"
#include "ContainerTemplate.h"  //UFC
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"

class CContractObject;
typedef std::map<UFC::AnsiString, CContractObject*> CStrContractMap;
int GetContractStrIndexMapCount(CStrContractMap& StrContractMap);
CContractObject *FindContractFromStrIndexMap(CStrContractMap& StrContractMap, const UFC::AnsiString& KeyValue);
bool InsertContractToStrIndexMap(CStrContractMap& StrContractMap, const UFC::AnsiString& KeyValue, CContractObject *ContractPtr);
int ClearContractStrIndexMap(CStrContractMap& StrContractMap, bool DoDelete);
CContractObject *GetFirstContractFromStrIndexMap(CStrContractMap& StrContractMap);
CContractObject *GetSecondContractFromStrIndexMap(CStrContractMap& StrContractMap);
void DumpContractStrIndexMap(CStrContractMap& StrContractMap);

UFC::AnsiString BuildDoubleStr(const UFC::AnsiString& DoubleStr, int DecimalLocator);
int TranslateDigitalChar(char DigitalChar);

//--------------------------------------------------------------------------
class CCurrencyObject
{
private:
    UFC::AnsiString FCurrencyCode;  //TWA USA RMB
    char            FCurrencyType;  //1:NTD 2:USD 3:EUR 4:JPY 5:GBP 6:AUD 7:HKD 8:CNY A:ZAR
    
public:
    CCurrencyObject(const UFC::AnsiString& CurrencyCode, char CurrencyType);
    
    UFC::AnsiString GetCurrencyCode() {return FCurrencyCode;};
    char GetCurrencyType() {return FCurrencyType;};
    
    UFC::AnsiString ToString();
};  //CCurrencyObject

//--------------------------------------------------------------------------
class CCommodityObject
{
public: 
    static UFC::AnsiString DescribeCurrency(char CurrencyType);
    
private:
    UFC::AnsiString FContractCode;
    UFC::AnsiString FStockID;                    //Stock Only
    char            FContractKind;               //I:Index, R:Rate, B:Bond, C:Commodity, S:Stock
    double          FContractMultiplier;
    char            FStatusCode;                 //N:Normal, P, U
    char            FCurrencyType;               //1:NTD 2:USD 3:EUR 4:JPY 5:GBP 6:AUD 7:HKD 8:CNY A:ZAR
    int             FStrikePriceDecimalLocator;
    int             FOrderDecimalLocator;
    char            FAcceptQuoteFlag;            //Y, N
    UFC::AnsiString FBeginDate;                  //YYYYMMDD, Stock Only
    char            FBlockTradeFlag;             //Y, N
    char            FExpiryType;                 //S:Standard, W:week
    bool            FIsFutures;
    bool            FIsEnabled;
    
    //P14 Field 
    char   FABType;             //A: Risk Margin, B:Lower Limit of Risk Margin, 0: not Options
    char   FMarginType;         //0: Amount 1:Percent
    double FClearingMargin;
    double FMaintenanceMargin;
    double FInitialMargin;

    CStrContractMap FContractSymbolIndex;

protected:
    void ParseP09Record(const char *P09RecBuffer);
        
public:
    CCommodityObject(const UFC::AnsiString& ContractCode, bool IsFutures = true);
    CCommodityObject(const char *P09RecBuffer, bool IsFutures = true);
    ~CCommodityObject();
    
    UFC::AnsiString GetContractCode() {return FContractCode;};
    
    UFC::AnsiString GetStockID() {return FStockID;};
    void SetStockID(const UFC::AnsiString& NewValue) {FStockID = NewValue;};

    char GetContractKind() {return FContractKind;};
    void SetContractKind(char NewValue) {FContractKind = NewValue;};
    
    double GetContractMultiplier() {return FContractMultiplier;};
    void SetContractMultiplier(double NewValue) {FContractMultiplier = NewValue;};
    
    char GetStatusCode() {return FStatusCode;};
    void SetStatusCode(char NewValue) {FStatusCode = NewValue;};

    char GetCurrencyType() {return FCurrencyType;};
    void SetCurrencyType(char NewValue) {FCurrencyType = NewValue;};

    int GetStrikePriceDecimalLocator() {return FStrikePriceDecimalLocator;};
    void SetStrikePriceDecimalLocator(int NewValue) {FStrikePriceDecimalLocator = NewValue;};
    
    int GetOrderDecimalLocator() {return FOrderDecimalLocator;};
    void SetOrderDecimalLocator(int NewValue) {FOrderDecimalLocator = NewValue;};

    char GetAcceptQuoteFlag() {return FAcceptQuoteFlag;};
    void SetAcceptQuoteFlag(char NewValue) {FAcceptQuoteFlag = NewValue;};

    UFC::AnsiString GetBeginDate() {return FBeginDate;};
    void SetBeginDate(const UFC::AnsiString& NewValue) {FBeginDate = NewValue;};

    char GetBlockTradeFlag() {return FBlockTradeFlag;};
    void SetBlockTradeFlag(char NewValue) {FBlockTradeFlag = NewValue;};
    
    char GetExpiryType() {return FExpiryType;};
    void SetExpiryType(char NewValue) {FExpiryType = NewValue;};
    
    bool IsFutures() {return FIsFutures;};
    void SetIsFutures(bool IsFutures) {FIsFutures = IsFutures;};
    
    bool IsEnabled() {return FIsEnabled;};
    void SetEnabled(bool Enabled) {FIsEnabled = Enabled;};

    char GetABType() {return FABType;};
    void SetABType(char NewValue) {FABType = NewValue;};
    
    char GetMarginType() {return FMarginType;};
    void SetMarginType(char NewValue) {FMarginType = NewValue;};
    
    double GetClearingMargin() {return FClearingMargin;};
    void SetClearingMargin(double NewValue) {FClearingMargin = NewValue;};
    
    double GetMaintenanceMargin() {return FMaintenanceMargin;};
    void SetMaintenanceMargin(double NewValue) {FMaintenanceMargin = NewValue;};
    
    double GetInitialMargin() {return FInitialMargin;};
    void SetInitialMargin(double NewValue) {FInitialMargin = NewValue;};
    
    int GetContractCount();
    CContractObject *FindContractFromSymbolIndex(const UFC::AnsiString& Symbol);
    bool InsertContractToSymbolIndex(const UFC::AnsiString& Symbol, CContractObject *ContractPtr);
    
    UFC::AnsiString ToString();
    void DumpData();
    void DumpContract();
};  //CCommodityObject

//--------------------------------------------------------------------------
class CContractYearMonthSet
{
public:
    static UFC::AnsiString GeterateContractSetKey(const UFC::AnsiString& ContractCode, char CallPut, const UFC::AnsiString& StrikePrice = "");
    
private:
    UFC::AnsiString FContractYearMonthKey;
    UFC::AnsiString FContractCode;
    UFC::AnsiString FStrikePriceStr;
    char            FCallPut;
    
    CStrContractMap FContractYMIndex;
    
public:
    CContractYearMonthSet(CContractObject *ContractPtr);
    CContractYearMonthSet(const UFC::AnsiString& ContractCode, char CallPut, const UFC::AnsiString& StrikePrice = "");
    ~CContractYearMonthSet();

    UFC::AnsiString GetContractYearMonthKey() {return FContractYearMonthKey;};
    
    UFC::AnsiString GetContractCode() {return FContractCode;};
    void SetContractCode(const UFC::AnsiString& NewValue) {FContractCode = NewValue;};
    
    UFC::AnsiString GetStrikePriceStr() {return FStrikePriceStr;};
    void SetStrikePriceStr(const UFC::AnsiString& NewValue) {FStrikePriceStr = NewValue;};
    
    char GetCallPut() {return FCallPut;};
    void SetCallPut(char NewValue) {FCallPut = NewValue;};
    
    int GetContractYearMonthCount();
    CContractObject *FindContractFromYearMonthIndex(const UFC::AnsiString& DeliveryYearMonth);
    bool InsertContractToYearMonthIndex(const UFC::AnsiString& DeliveryYearMonth, CContractObject *ContractPtr);
    CContractObject *GetFirstYearMonthContract();
    CContractObject *GetSecondYearMonthContract();
    
    UFC::AnsiString ToString();
};  //CContractYearMonthSet

//--------------------------------------------------------------------------
class CContractObject
{
private:
    UFC::AnsiString FSymbol;
    UFC::AnsiString FDeliveryYearMonth;  //yyyymm
    UFC::AnsiString FStrikePriceStr;     //99999.9999
    double          FStrikePrice;
    char            FCallPut;
    UFC::AnsiString FBeginDate;
    UFC::AnsiString FEndDate;
    UFC::AnsiString FRaisePrice1Str;
    double          FRaisePrice1;
    UFC::AnsiString FFallPrice1Str;
    double          FFallPrice1;
    UFC::AnsiString FPremiumStr;
    double          FPremium;
    UFC::AnsiString FRaisePrice2Str;
    double          FRaisePrice2;
    UFC::AnsiString FFallPrice2Str;
    double          FFallPrice2;
    UFC::AnsiString FRaisePrice3Str;
    double          FRaisePrice3;
    UFC::AnsiString FFallPrice3Str;
    double          FFallPrice3;
    char            FProductKind;      //I:Index, R:Rate, B:Bond, C:Commodity, S:Stock
    char            FAcceptQuoteFlag;  //Y, N
    int             FDecimalLocator;
    int             FProductSequence;
    UFC::AnsiString FContractCode;
    
protected:
    void ParseP08Record(const char *P08RecBuffer);
    void ParseP08Record(UFC::TRecord *P08RecPtr);
        
public:
    CContractObject();
    CContractObject(const char *P08RecBuffer);
    CContractObject(UFC::TRecord *P08RecPtr);
    
    UFC::AnsiString GetSymbol() {return FSymbol;};
    void SetSymbol(const UFC::AnsiString& NewValue) {FSymbol = NewValue;};
    
    UFC::AnsiString GetDeliveryYearMonth() {return FDeliveryYearMonth;};
    void SetDeliveryYearMonth(const UFC::AnsiString& NewValue) {FDeliveryYearMonth = NewValue;};
    
    UFC::AnsiString GetStrikePriceStr() {return FStrikePriceStr;};
    void SetStrikePriceStr(const UFC::AnsiString& NewValue) {FStrikePriceStr = NewValue;};
    
    double GetStrikePrice() {return FStrikePrice;};
    void SetStrikePrice(double NewValue) {FStrikePrice = NewValue;};
    
    char GetCallPut() {return FCallPut;};
    void SetCallPut(char NewValue) {FCallPut = NewValue;};
    
    UFC::AnsiString GetBeginDate() {return FBeginDate;};
    void SetBeginDate(const UFC::AnsiString& NewValue) {FBeginDate = NewValue;};
    
    UFC::AnsiString GetEndDate() {return FEndDate;};
    void SetEndDate(const UFC::AnsiString& NewValue) {FEndDate = NewValue;};
    
    UFC::AnsiString GetRaisePrice1Str() {return FRaisePrice1Str;};
    void SetRaisePrice1Str(const UFC::AnsiString& NewValue) {FRaisePrice1Str = NewValue;};
    
    double GetRaisePrice1() {return FRaisePrice1;};
    void SetRaisePrice1(double NewValue) {FRaisePrice1 = NewValue;};
    
    UFC::AnsiString GetFallPrice1Str() {return FFallPrice1Str;};
    void SetFallPrice1Str(const UFC::AnsiString& NewValue) {FFallPrice1Str = NewValue;};
    
    double GetFallPrice1() {return FFallPrice1;};
    void SetFallPrice1(double NewValue) {FFallPrice1 = NewValue;};
    
    UFC::AnsiString GetPremiumStr() {return FPremiumStr;};
    void SetPremiumStr(const UFC::AnsiString& NewValue) {FPremiumStr = NewValue;};
    
    double GetPremium() {return FPremium;};
    void SetPremium(double NewValue) {FPremium = NewValue;};
    
    UFC::AnsiString GetRaisePrice2Str() {return FRaisePrice2Str;};
    void SetRaisePrice2Str(const UFC::AnsiString& NewValue) {FRaisePrice2Str = NewValue;};
    
    double GetRaisePrice2() {return FRaisePrice2;};
    void SetRaisePrice2(double NewValue) {FRaisePrice2 = NewValue;};
    
    UFC::AnsiString GetFallPrice2Str() {return FFallPrice2Str;};
    void SetFallPrice2Str(const UFC::AnsiString& NewValue) {FFallPrice2Str = NewValue;};
    
    double GetFallPrice2() {return FFallPrice2;};
    void SetFallPrice2(double NewValue) {FFallPrice2 = NewValue;};
    
    UFC::AnsiString GetRaisePrice3Str() {return FRaisePrice3Str;};
    void SetRaisePrice3Str(const UFC::AnsiString& NewValue) {FRaisePrice3Str = NewValue;};
    
    double GetRaisePrice3() {return FRaisePrice3;};
    void SetRaisePrice3(double NewValue) {FRaisePrice3 = NewValue;};
    
    UFC::AnsiString GetFallPrice3Str() {return FFallPrice3Str;};
    void SetFallPrice3Str(const UFC::AnsiString& NewValue) {FFallPrice3Str = NewValue;};
    
    double GetFallPrice3() {return FFallPrice3;};
    void SetFallPrice3(double NewValue) {FFallPrice3 = NewValue;};
    
    char GetProductKind() {return FProductKind;};
    void SetProductKind(char NewValue) {FProductKind = NewValue;};
    
    char GetAcceptQuoteFlag() {return FAcceptQuoteFlag;};
    void SetAcceptQuoteFlag(char NewValue) {FAcceptQuoteFlag = NewValue;};
    
    int GetDecimalLocator() {return FDecimalLocator;};
    void SetDecimalLocator(int NewValue) {FDecimalLocator = NewValue;};
    
    int GetProductSequence() {return FProductSequence;};
    void SetProductSequence(int NewValue) {FProductSequence = NewValue;};
    
    UFC::AnsiString GetContractCode() {return FContractCode;};
    void SetContractCode(const UFC::AnsiString& NewValue) {FContractCode = NewValue;};
    
    UFC::AnsiString ToString();
};  //CContractObject

class GCRMConnection;
class CAccountExecutive;
//--------------------------------------------------------------------------
class AccountInfo
{
public:
    static Int32 FMaxQuoteOrderQty;
    static UFC::AnsiString BuildAccountKey( const UFC::AnsiString& Account, const UFC::AnsiString& AE, char CurrencyType );
    
private:
    UFC::AnsiString       FAE;
    UFC::AnsiString       FAccount;
    double                FOrderAmount;
    double                FQuoteOrderAmount;
    double                FMatchAmount;
    double                FQuoteMatchAmount;
    int                   FQuoteMatchQty;
    double                FCanUse;
    UFC::AnsiString       FCurrencyCode;  //TWA USA RMB
    char                  FCurrencyType;  //1:NTD 2:USD 3:EUR 4:JPY 5:GBP 6:AUD 7:HKD 8:CNY A:ZAR
    CAccountExecutive     *FAEPtr;
    UFC::PCriticalSection FCS;
    
public:
    AccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE );
    AccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE, const UFC::AnsiString& CurrencyCode );
    AccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE, char CurrencyType );
    
    UFC::AnsiString GetAE();
    
    double GetOrderAmount();
    void SetOrderAmount( double OrderAmount );

    double GetQuoteOrderAmount();
    void SetQuoteOrderAmount( double OrderAmount );
    
    double GetMatchAmount();
    void SetMatchAmount( double MatchAmount );

    double GetQuoteMatchAmount();
    void SetQuoteMatchAmount( double MatchAmount );

    double GetCanUse();
    void SetCanUse( double CanUse );

    UFC::AnsiString GetCurrencyCode();
    void SetCurrencyCode( const UFC::AnsiString& NewValue );

    char GetCurrencyType();
    void SetCurrencyType( char NewValue );

    CAccountExecutive *GetAEObj();
    void SetAEObj(CAccountExecutive *AEPtr);
    
    int GetQuoteMatchQty();
    void SetQuoteMatchQty(int QuoteMatchQty);
    void AddQuoteMatchQty(int MatchQty);
    
    BOOL CanPlaceOrder( double NewOrderAmount );
    bool CanPlaceFuturesOrder(int OrderQty, double CommodMargins, bool IsMultiLeg, const UFC::AnsiString& OrderID, const UFC::AnsiString& Symbol, char Side, double Price, const UFC::AnsiString& OrigAccount);
    bool CanPlaceFuturesQuoteOrder(int BidOrderQty, double BidCommodMargins, int AskOrderQty, double AskCommodMargin, const UFC::AnsiString& OrderID, const UFC::AnsiString& Symbol, double BidPrice, double AskPrice, const UFC::AnsiString& OrigAccount);
    bool CanPlaceOptionsOrder(int OrderQty, double CommodMargins, bool IsMultiLeg, const UFC::AnsiString& OrderID, const UFC::AnsiString& Symbol, char Side, double Price, const UFC::AnsiString& OrigAccount);
    bool CanPlaceOptionsQuoteOrder(int BidOrderQty, double BidCommodMargins, int AskOrderQty, double AskCommodMargins, const UFC::AnsiString& OrderID, const UFC::AnsiString& Symbol, double BidPrice, double AskPrice, const UFC::AnsiString& OrigAccount);
    void NewOrder( double NewOrderAmount );
    void NewQuoteOrder( double NewOrderAmount );
    void CancelOrder( double CancelAmount );
    void CancelQuoteOrder( double CancelAmount );
    void FilledOrder( double MatchAmount );
    void FilledQuoteOrder( int MatchQty );
    void FilledQuoteOrder( double MatchAmount );
    
    UFC::AnsiString ToString();
    void DumpData();
};

//--------------------------------------------------------------------------
class CAccountExecutive
{
public:
    static UFC::AnsiString BuildAccountExecutiveKey(const UFC::AnsiString& AE, char CurrencyType);
    
private:
    UFC::AnsiString FAE;
    char            FCurrencyType;
    UFC::AnsiString FCurrencyCode;
    bool            FIsOnlyNearMonth;
    double          FReserveDeposit;
    int             FQuoteMatchQtyLimit;
    
public:
    CAccountExecutive(const UFC::AnsiString& AE, char CurrencyType);
    
    UFC::AnsiString GetAE() {return FAE;};
    char GetCurrencyType() {return FCurrencyType;};
    
    UFC::AnsiString GetCurrencyCode() {return FCurrencyCode;};
    void SetCurrencyCode(const UFC::AnsiString& NewValue) {FCurrencyCode = NewValue;};

    bool IsOnlyNearMonth() {return FIsOnlyNearMonth;};
    void SetOnlyNearMonth(bool IsOnlyNearMonth) {FIsOnlyNearMonth = IsOnlyNearMonth;};
    
    double GetReserveDeposit() {return FReserveDeposit;};
    void SetReserveDeposit(double ReserveDeposit) {FReserveDeposit = ReserveDeposit;};
    
    int GetQuoteMatchQtyLimit() {return FQuoteMatchQtyLimit;};
    void SetQuoteMatchQtyLimit(int QuoteMatchQtyLimit) {FQuoteMatchQtyLimit = QuoteMatchQtyLimit;};
    
    UFC::AnsiString ToString();
    void DumpData();
};  //CAccountExecutive

//--------------------------------------------------------------------------
class CBasicRequest : public UFC::CMainKeyObject
{
protected:
    nsOrderMessageDefine::MarketEnum FMarket;
    Int32                            FNetworkID;
    UFC::AnsiString                  FTelegram;
    UFC::AnsiString                  FUserKey;
    
    //Header
    UFC::AnsiString FSubSystemName;  //9(02) "30"
    UFC::AnsiString FFunctionCode;   //9(02) "01":New "02":Reduce "03":Cancel "06":Replace Price
    UFC::AnsiString FMessageType;    //9(02) "00":Order "01":Confirm "08":Quote Order "09":Quote Confirm
    UFC::AnsiString FMessageTime;    //9(06) HHMMSS
    UFC::AnsiString FStatusCode;     //9(02) "00":Valid
    
    //Body
    UFC::AnsiString FFCMNo;           //X(07)  期貨商代號
    UFC::AnsiString FOrderNo;         //X(05)  委託書編號
    UFC::AnsiString FInvestorAcNo;    //X(07)  投資人帳號
    char            FInvestorFlag;    //X(01)  投資人身份碼
    UFC::AnsiString FProdID;          //X(20)  商品代號
    char            FOrderCondition;  //X(01)  委託條件 'F':FOK 'I':IOC 'R':ROD 'Q':Quote
    char            FOpenOffsetFlag;  //X(01)  開平倉碼 '0':Open '1':Close '9':Quote '2':DayTrade
    UFC::AnsiString FClearMeneberID;  //X(04)  結算會員代號
    
public:
    CBasicRequest();
    CBasicRequest(nsOrderMessageDefine::MarketEnum Market, Int32 NetworkID, const UFC::AnsiString& Telegram, const UFC::AnsiString& UserKey);
    virtual ~CBasicRequest() {};
    
    nsOrderMessageDefine::MarketEnum GetMarket() {return FMarket;};
    void SetMarket(nsOrderMessageDefine::MarketEnum NewValue) {FMarket = NewValue;};
    
    Int32 GetNetworkID() {return FNetworkID;};
    void SetNetworkID(Int32 NewValue) {FNetworkID = NewValue;};
    
    UFC::AnsiString GetTelegram() {return FTelegram;};
    void SetTelegram(const UFC::AnsiString& NewValue) {FTelegram = NewValue;};

    UFC::AnsiString GetUserKey() {return FUserKey;};
    void SetUserKey(const UFC::AnsiString& NewValue) {FUserKey = NewValue;};
    
    UFC::AnsiString GetSubSystemName() {return FSubSystemName;};
    void SetSubSystemName(const UFC::AnsiString& NewValue) {FSubSystemName = NewValue;};
    
    UFC::AnsiString GetFunctionCode() {return FFunctionCode;};
    void SetFunctionCode(const UFC::AnsiString& NewValue) {FFunctionCode = NewValue;};
    
    UFC::AnsiString GetMessageType() {return FMessageType;};
    void SetMessageType(const UFC::AnsiString& NewValue) {FMessageType = NewValue;};
    
    UFC::AnsiString GetMessageTime() {return FMessageTime;};
    void SetMessageTime(const UFC::AnsiString& NewValue) {FMessageTime = NewValue;};
    
    UFC::AnsiString GetStatusCode() {return FStatusCode;};
    void SetStatusCode(const UFC::AnsiString& NewValue) {FStatusCode = NewValue;};
    
    UFC::AnsiString GetFCMNo() {return FFCMNo;};
    void SetFCMNo(const UFC::AnsiString& NewValue) {FFCMNo = NewValue;};
    
    UFC::AnsiString GetOrderNo() {return FOrderNo;};
    void SetOrderNo(const UFC::AnsiString& NewValue) {FOrderNo = NewValue;};
    
    UFC::AnsiString GetInvestorAcNo() {return FInvestorAcNo;};
    void SetInvestorAcNo(const UFC::AnsiString& NewValue) {FInvestorAcNo = NewValue;};

    char GetInvestorFlag() {return FInvestorFlag;};
    void SetInvestorFlag(char NewValue) {FInvestorFlag = NewValue;};

    UFC::AnsiString GetProdID() {return FProdID;};
    void SetProdID(const UFC::AnsiString& NewValue) {FProdID = NewValue;};

    char GetOrderCondition() {return FOrderCondition;};
    void SetOrderCondition(char NewValue) {FOrderCondition = NewValue;};

    char GetOpenOffsetFlag() {return FOpenOffsetFlag;};
    void SetOpenOffsetFlag(char NewValue) {FOpenOffsetFlag = NewValue;};
    
    UFC::AnsiString GetClearMeneberID() {return FClearMeneberID;};
    void SetClearMeneberID(const UFC::AnsiString& NewValue) {FClearMeneberID = NewValue;};
    
    void ParseHeadTelegram(const UFC::AnsiString& Telegram);
    virtual void ParseBodyTelegram(const UFC::AnsiString& Telegram);
};  //CBasicRequest
        
//--------------------------------------------------------------------------
class COrderRequest : public CBasicRequest
{
protected:
    char            FBuySellCode;     //X(01)  買/賣別 'B':Buy 'S':Sell
    char            FOrderType;       //X(01)  委託方式 'M':Market 'L':Limit
    UFC::AnsiString FOrderPriceStr;   //S9(08) 委託價格/價差
    double          FOrderPrice;      //S9(08) 委託價格/價差
    int             FOrderQuantity;   //9(04)  委託數量

public:
    COrderRequest();
    COrderRequest(nsOrderMessageDefine::MarketEnum Market, Int32 NetworkID, const UFC::AnsiString& Telegram, const UFC::AnsiString& UserKey);
    ~COrderRequest() {};
    
    char GetBuySellCode() {return FBuySellCode;};
    void SetBuySellCode(char NewValue) {FBuySellCode = NewValue;};

    char GetOrderType() {return FOrderType;};
    void SetOrderType(char NewValue) {FOrderType = NewValue;};
    
    UFC::AnsiString GetOrderPriceStr() {return FOrderPriceStr;};
    void SetOrderPriceStr(const UFC::AnsiString& NewValue) {FOrderPriceStr = NewValue;};

    double GetOrderPrice() {return FOrderPrice;};
    void SetOrderPrice(double NewValue) {FOrderPrice = NewValue;};

    int GetOrderQuantity() {return FOrderQuantity;};
    void SetOrderQuantity(int NewValue) {FOrderQuantity = NewValue;};

    void ParseBodyTelegram(const UFC::AnsiString& Telegram);
};  //COrderRequest

//--------------------------------------------------------------------------
class CQuoteRequest : public CBasicRequest
{
protected:
    UFC::AnsiString FBidQuotePriceStr;   //S9(09) 買進報價價格
    double          FBidQuotePrice;      //S9(09) 買進報價價格
    UFC::AnsiString FAskQuotePriceStr;   //S9(09) 賣出報價價格
    double          FAskQuotePrice;      //S9(09) 賣出報價價格
    int             FBidQuoteQuantity;   //9(04)  買進委託數量
    int             FAskQuoteQuantity;   //9(04)  賣出委託數量
    
public:
    CQuoteRequest();
    CQuoteRequest(nsOrderMessageDefine::MarketEnum Market, Int32 NetworkID, const UFC::AnsiString& Telegram, const UFC::AnsiString& UserKey);
    ~CQuoteRequest() {};
    
    UFC::AnsiString GetBidQuotePriceStr() {return FBidQuotePriceStr;};
    void SetBidQuotePriceStr(const UFC::AnsiString& NewValue) {FBidQuotePriceStr = NewValue;};

    double GetBidQuotePrice() {return FBidQuotePrice;};
    void SetBidQuotePrice(double NewValue) {FBidQuotePrice = NewValue;};
    
    UFC::AnsiString GetAskQuotePriceStr() {return FAskQuotePriceStr;};
    void SetAskQuotePriceStr(const UFC::AnsiString& NewValue) {FAskQuotePriceStr = NewValue;};

    double GetAskQuotePrice() {return FAskQuotePrice;};
    void SetAskQuotePrice(double NewValue) {FAskQuotePrice = NewValue;};
    
    int GetBidQuoteQuantity() {return FBidQuoteQuantity;};
    void SetBidQuoteQuantity(int NewValue) {FBidQuoteQuantity = NewValue;};
    
    int GetAskQuoteQuantity() {return FAskQuoteQuantity;};
    void SetAskQuoteQuantity(int NewValue) {FAskQuoteQuantity = NewValue;};
    
    void ParseBodyTelegram(const UFC::AnsiString& Telegram);
};  //CQuoteRequest

//--------------------------------------------------------------------------
class CQuoteOrderRecord
{
private:
    nsOrderMessageDefine::MarketEnum FMarket;
    UFC::AnsiString FOrderID;
    UFC::AnsiString FSymbol;
    double FBidPrice;
    int FBidQty;
    int FBidMatchQty;
    int FBidCumQty;
    int FBidLeavesQty;
    double FAskPrice;
    int FAskQty;
    int FAskMatchQty;
    int FAskCumQty;
    int FAskLeavesQty;
    
public:
    CQuoteOrderRecord(nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& OrderID, const UFC::AnsiString& Symbol, double BidPrice, int BidQty, double AskPrice, int AskQty);

    nsOrderMessageDefine::MarketEnum GetMarket() {return FMarket;};
    
    UFC::AnsiString GetOrderID() {return FOrderID;};
    
    UFC::AnsiString GetSymbol() {return FSymbol;};
    
    double GetBidPrice() {return FBidPrice;};
    
    int GetBidQty() {return FBidQty;};
    
    int GetBidMatchQty() {return FBidMatchQty;};
    void SetBidMatchQty(int BidMatchQty) {FBidMatchQty = BidMatchQty;};
    
    int GetBidCumQty() {return FBidCumQty;};
    void SetBidCumQty(int BidCumQty) {FBidCumQty = BidCumQty;};
    
    int GetBidLeavesQty() {return FBidLeavesQty;};
    void SetBidLeavesQty(int BidLeavesQty) {FBidLeavesQty = BidLeavesQty;};
    
    double GetAskPrice() {return FAskPrice;};
    
    int GetAskQty() {return FAskQty;};
    
    int GetAskMatchQty() {return FAskMatchQty;};
    void SetAskMatchQty(int AskMatchQty) {FAskMatchQty = AskMatchQty;};
    
    int GetAskCumQty() {return FAskCumQty;};
    void SetAskCumQty(int AskCumQty) {FAskCumQty = AskCumQty;};
    
    int GetAskLeavesQty() {return FAskLeavesQty;};
    void SetAskLeavesQty(int AskLeavesQty) {FAskLeavesQty = AskLeavesQty;};
};  //CQuoteOrderRecord

//--------------------------------------------------------------------------
class COptionAValueInfo
{
private:
    UFC::AnsiString FContractCode;
    char            FMarginType;    //0:Amount 1:percent
    double          FMarginInitial;
    
public:
    COptionAValueInfo(const UFC::AnsiString& ContractCode, char MarginType, double MarginInitial);
    
    UFC::AnsiString GetContractCode() {return FContractCode;};
    
    char GetMarginType() {return FMarginType;};
    void SetMarginType(char NewValue) {FMarginType = NewValue;};
    
    double GetMarginInitial() {return FMarginInitial;};
    void SetMarginInitial(double NewValue) {FMarginInitial = NewValue;};

    UFC::AnsiString ToString();
    void DumpData();
};  //COptionAValueInfo
//--------------------------------------------------------------------------
class GCRMConnection : public IOrderConnectionEventListener, public MessageListener
{
public:
    static UFC::FileStreamEx *FCheckLogPtr;

    static bool FDoTrimAEZero;
    static UFC::AnsiString TrimBeginZero(const UFC::AnsiString& SourceStr);

    static UFC::AnsiString FClearAmountFileName;
    
    static UFC::PReadWriteLock FCurrencyObjLock;
    static UFC::PHashMap<UFC::AnsiString, CCurrencyObject*> FCurrencyObjCodeIndex;
    static UFC::PHashMap<char, CCurrencyObject*> FCurrencyObjTypeIndex;
    static void LockCurrencyObject(UFC::RWLockTypeEnum LockType) {FCurrencyObjLock.Lock(LockType);};
    static void UnlockCurrencyObject(UFC::RWLockTypeEnum LockType) {FCurrencyObjLock.Unlock(LockType);};
    static int GetCurrencyCodeIndexCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::GetStrIdxHMapElementPtrCount(FCurrencyObjCodeIndex, &FCurrencyObjLock, LockType);};
    static CCurrencyObject* FindCurrencyFromCodeIndex(const UFC::AnsiString& CurrencyCode, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::FindElementPtrFromStrIdxHMap(FCurrencyObjCodeIndex, CurrencyCode, &FCurrencyObjLock, LockType);};
    static bool InsertCurrencyToCodeIndex(const UFC::AnsiString& CurrencyCode, CCurrencyObject* CurrencyPtr, bool CheckExist = UFC::cstNotCheck, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::InsertElementPtrIntoStrIdxHMap(FCurrencyObjCodeIndex, CurrencyCode, CurrencyPtr, CheckExist, &FCurrencyObjLock, LockType);};
    static CCurrencyObject* RemoveCurrencyFromCodeIndex(const UFC::AnsiString& CurrencyCode, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::RemoveElementPtrFromStrIdxHMap(FCurrencyObjCodeIndex, CurrencyCode, &FCurrencyObjLock, LockType);};
    static int ClearCurrencyCodeIndex(bool DoDelete = UFC::cstNotDelete, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::ClearElementPtrStrIdxHMap(FCurrencyObjCodeIndex, DoDelete, &FCurrencyObjLock, LockType);};
    int GetCurrencyTypeIndexCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::GetHMapElementPtrCount(FCurrencyObjTypeIndex, &FCurrencyObjLock, LockType);};
    CCurrencyObject* FindCurrencyFromTypeIndex(char CurrencyType, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::FindElementPtrFromHMap(FCurrencyObjTypeIndex, CurrencyType, &FCurrencyObjLock, LockType);};
    bool InsertCurrencyToTypeIndex(char CurrencyType, CCurrencyObject* CurrencyPtr, bool CheckExist = UFC::cstNotCheck, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::InsertElementPtrIntoHMap(FCurrencyObjTypeIndex, CurrencyType, CurrencyPtr, CheckExist, &FCurrencyObjLock, LockType);};
    CCurrencyObject* RemoveCurrencyFromTypeIndex(char CurrencyType, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::RemoveElementPtrFromHMap(FCurrencyObjTypeIndex, CurrencyType, &FCurrencyObjLock, LockType);};
    int ClearCurrencyTypeIndex(bool DoDelete = UFC::cstNotDelete, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::ClearElementPtrHMap(FCurrencyObjTypeIndex, DoDelete, &FCurrencyObjLock, LockType);};
    static void DumpCurrencyObject();
    
    static std::map<UFC::AnsiString, CCommodityObject*> FCommodityContractCodeIndex;
    static int GetCommodityCount();
    static CCommodityObject *FindCommodity(const UFC::AnsiString& ContractCode);
    static bool InsertCommodity(const UFC::AnsiString& ContractCode, CCommodityObject *CommodityPtr);
    static int ClearCommodity(bool DoDelete = false);
    static void DumpCommodity(bool enabledOnly = false);
    static void LoadP09File(const UFC::AnsiString& P09FileName, bool IsFutures = true);

    static std::map<UFC::AnsiString, CContractObject*> FContractSymbolIndex;
    static int GetContractCount();
    static CContractObject *FindContract(const UFC::AnsiString& Symbol);
    static bool InsertContract(const UFC::AnsiString& Symbol, CContractObject *ContractPtr);
    static int ClearContract(bool DoDelete = false);
    static void DumpContract();

    static std::map<UFC::AnsiString, CContractYearMonthSet*> FContractYMSetIndex;
    static int GetContractYMSetCount();
    static CContractYearMonthSet *FindContractYMSet(const UFC::AnsiString& KeyValue);
    static bool InsertContractYMSet(const UFC::AnsiString& KeyValue, CContractYearMonthSet *ContractYMSetPtr);
    static int ClearContractYMSet(bool DoDelete = false);
    static void DumpContractYMSet();
    
    static std::map<UFC::AnsiString, CContractObject*> FNearMonthContractSymbolndex;
    static CContractObject *FindNearMonthContract(const UFC::AnsiString& Symbol);
    static bool InsertNearMonthContract(const UFC::AnsiString& Symbol, CContractObject *ContractPtr);
    static void DumpNearMonthContract();
    static void LoadP08File(const UFC::AnsiString& P08FormatFileName);
    static void BuildNearMonthIndex();

    static void LoadP14File(const UFC::AnsiString& P14FileName, bool NeedCopy = false, const UFC::AnsiString& P14CopyFileName = "");
    
    static std::map<UFC::AnsiString, CAccountExecutive*> FSubAccountAEIndex;
    static CAccountExecutive *FindSubAccount(const UFC::AnsiString& AE, char CurrencyType);
    static bool InsertSubAccount(const UFC::AnsiString& AE, char CurrencyType, CAccountExecutive* SubAccountPtr);
    static void DumpSubAccount();

    static std::map<UFC::AnsiString, CQuoteOrderRecord*> FQuoteOrderIdIndex;
    static CQuoteOrderRecord *FindQuoteOrder(const UFC::AnsiString& OrderID);
    static bool InsertQuoteOrder(const UFC::AnsiString& OrderID, CQuoteOrderRecord* QuoteOrderPtr);
    
    static bool ParseFuturesMultiLegSymbol(const UFC::AnsiString& Symbol, UFC::AnsiString& Leg1Symbol, UFC::AnsiString& Leg2Symbol);
    static bool ParseOptionsMultiLegSymbol(const UFC::AnsiString& Symbol, UFC::AnsiString& Leg1Symbol, UFC::AnsiString& Leg2Symbol);
    static bool ParseMultiLegSymbol(nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& Symbol, UFC::AnsiString& Leg1Symbol, UFC::AnsiString& Leg2Symbol);
    
private:    
    MessageObject*        FMessageObject;
    UFC::AnsiString       FIP;
    int                   FPort;
    UFC::UiniFile*        CommodityDepositTable;
    UFC::PCriticalSection FCommodCS;
    UFC::PCriticalSection FAccountLimitCS;
    UFC::PHashedList<UFC::AnsiString, AccountInfo*> AccountInfoTable;
    UFC::PHashedSet<UFC::AnsiString>                FExecIDTable;
    UFC::FileStreamEx*    FExecIDFile;
    ///< Speedy API
    UFC::AnsiString       FGatewayIP;
    int                   FGatewayPort;
    UFC::AnsiString       FID;
    UFC::AnsiString       FPassword;   
    UFC::AnsiString       FAccount;   
    UFC::AnsiString       FBrokerID;
    UFC::AnsiString       FClearMemberID;
    UFC::UiniFile*        FConfig;
    
private:
    int                   FRecoverBeginTime;    
    TTaifexConnection*    FConnection; 
    BOOL                  FIsLogon;
    BOOL                  FRejectMarketOrd;
private:
    BOOL                  FReleaseMarginOnFilled;
    BOOL                  FReleaseMarginOnRODCxl;
    BOOL                  FReleaseMarginOnFOKIOCCxl;
    int                   FMaxQuoteOrderQty;
private:
    UFC::AnsiString       FToSpeedySubject;
    UFC::AnsiString       FToSpeedyKey;
    UFC::AnsiString       FFutOrderSubject;
    UFC::AnsiString       FOptOrderSubject;
    
    UFC::PReadWriteLock FRequestLock;
    UFC::PHashMap<int, CBasicRequest*> FReqNIDIndex;
    
private:    
    void UseMargin( const UFC::AnsiString& Account, UFC::AnsiString& AE );
private:
    void LoadSetting( void );    
    void OnCancelReplace( TExecutionReportMessage* Msg );
    void RecoverExecID( void );
    void UpdateExecID( const UFC::AnsiString& ExecID );
    bool IsReplacePx( const UFC::AnsiString& Order );
    void GetAE( const UFC::AnsiString& Key, UFC::AnsiString& AE );
private: 
    bool GetOrderTreeNode(MTree *Data, UFC::AnsiString& OrderStr, Int32& NID, UFC::AnsiString& UserKey);
    void PassReplacePriceOrder(nsOrderMessageDefine::MarketEnum OrderMarket, MTree *Data);
    bool RejectMarketOrder(nsOrderMessageDefine::MarketEnum OrderMarket, char OrderType, MTree *Data);
    void RejectCommodityNotExist(nsOrderMessageDefine::MarketEnum OrderMarket, const UFC::AnsiString& ContractCode, MTree *Data);
    void RejectAccountInfoNotExist(nsOrderMessageDefine::MarketEnum OrderMarket, const UFC::AnsiString& Account, const UFC::AnsiString& AE, char currencyType, MTree *Data);
    void RejectNearMonthOrder(nsOrderMessageDefine::MarketEnum OrderMarket, MTree *Data);
    void RejectDepositNotExist(nsOrderMessageDefine::MarketEnum OrderMarket, const UFC::AnsiString& Symbol, MTree *Data);
    void CheckFuturesOrderRisk(nsOrderMessageDefine::MarketEnum OrderMarket, MTree *Data);
    void CheckOptionsOrderRisk(nsOrderMessageDefine::MarketEnum OrderMarket, MTree *Data);
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );
    
private: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count );
    virtual void OnCancelWorking( CancelWorkingResult Result, int Count ) {};
    virtual void OnNews( TNewsMessage* Msg ) {};
    
public:
    GCRMConnection( int RecoverBeginTime );
    virtual ~GCRMConnection( void );
    
    void LockRequest(UFC::RWLockTypeEnum LockType) {FRequestLock.Lock(LockType);};
    void UnlockRequest(UFC::RWLockTypeEnum LockType) {FRequestLock.Unlock(LockType);};
    int GetRequestNIDIndexCount(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) 
        {return UFC::GetHMapElementPtrCount(FReqNIDIndex, &FRequestLock, LockType);};
    CBasicRequest* FindRequestFromNIDIndex(int ReqNetworkID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::FindElementPtrFromHMap(FReqNIDIndex, ReqNetworkID, &FRequestLock, LockType);};
    bool InsertRequestToNIDIndex(int ReqNetworkID, CBasicRequest* ReqPtr, bool CheckExist = UFC::cstNotCheck, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::InsertElementPtrIntoHMap(FReqNIDIndex, ReqNetworkID, ReqPtr, CheckExist, &FRequestLock, LockType);};
    CBasicRequest* RemoveRequestFromNIDIndex(int ReqNetworkID, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::RemoveElementPtrFromHMap(FReqNIDIndex, ReqNetworkID, &FRequestLock, LockType);};
    int ClearRequestNIDIndex(bool DoDelete = UFC::cstNotDelete, UFC::RWLockTypeEnum LockType = UFC::rwNotLock)
        {return UFC::ClearElementPtrHMap(FReqNIDIndex, DoDelete, &FRequestLock, LockType);}; 
        
    bool ClearSubAccountOrderAmount(const UFC::AnsiString& AccountNo, const UFC::AnsiString& AENo, const UFC::AnsiString& Currency);
    bool ClearOrderAmount(const UFC::AnsiString& ClearFileName);
    
    double       GetCommodityDeposit(const UFC::AnsiString& Symbol, double Price, bool IsBuy, bool IsMultiLeg, const UFC::AnsiString& Leg1Symbol = "", const UFC::AnsiString& Leg2Symbol = "");
    void         SetAccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE, char CurrencyType, const UFC::AnsiString& CurrencyCode, double NewCanUse, AccountInfo *AccInfoPtr );
    AccountInfo* GetAccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE, char CurrencyType );
    void         CheckConnection( void );
    void         Recover( const UFC::AnsiString& BeginTime );
};
//------------------------------------------------------------------------------
#endif
