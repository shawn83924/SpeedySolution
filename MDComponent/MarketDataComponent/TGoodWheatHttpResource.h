//------------------------------------------------------------------------------

#ifndef TGoodWheatHttpResourceH
#define TGoodWheatHttpResourceH
//------------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <vector>
#include <map>

#include "XMLDoc.hpp"
#include "IdBaseComponent.hpp"
#include "IdComponent.hpp"
#include "IdHTTP.hpp"
#include "IdTCPClient.hpp"
#include "IdTCPConnection.hpp"
#include "IdSSL.hpp"
#include "IdSSLOpenSSL.hpp"

//------------------------------------------------------------------------------
extern UnicodeString CheckGWCommonErrorMessage(UnicodeString UserKey, UnicodeString AccountNo, UnicodeString QryEnvFlag, UnicodeString ErrorField);
extern String CombineSymbolCode(String CommodityPrefix, double StrikePrice, String YearMonth, String CallPut);

//------------------------------------------------------------------------------
class TGoodWheatAccountMessage
{
private:
	UnicodeString FUserKey;
	UnicodeString FAccountNo;
	UnicodeString FAccountName;
	UnicodeString FBrokerNo;
	UnicodeString FActIp;
	UnicodeString FAeNo;
	UnicodeString FIbNo;
	UnicodeString FQueryEnvFlag;
	UnicodeString FErrorFlag;
public:
	TGoodWheatAccountMessage();
	TGoodWheatAccountMessage(TXMLDocument* DataXML);

	UnicodeString GetUserKey() {return FUserKey;};
	UnicodeString GetAccountNo() {return FAccountNo;};
	UnicodeString GetAccountName() {return FAccountName;};
	UnicodeString GetBrokerNo() {return FBrokerNo;};
	UnicodeString GetActIp() {return FActIp;};
	UnicodeString GetAeNo() {return FAeNo;};
	UnicodeString GetIbNo() {return FIbNo;};
	UnicodeString GetQueryEnvFlag() {return FQueryEnvFlag;};
	UnicodeString GetErrorFlag() {return FErrorFlag;};

	void SetUserKey(UnicodeString UserKey) {FUserKey = UserKey;};
	void SetAccountNo(UnicodeString AccountNo) {FAccountNo = AccountNo;};
	void SetAccountName(UnicodeString AccountName) {FAccountName = AccountName;};
	void SetBrokerNo(UnicodeString BrokerNo) {FBrokerNo = BrokerNo;};
	void SetActIp(UnicodeString ActIp) {FActIp = ActIp;};
	void SetAeNo(UnicodeString AeNo) {FAeNo = AeNo;};
	void SetIbNo(UnicodeString IbNo) {FIbNo = IbNo;};

	void GetDataFromXML(TXMLDocument* DataXML);
};  //TGoodWheatAccountMessage

//------------------------------------------------------------------------------
class TGoodWheatOrderMessage
{
private:
	bool          FCanCancel;
	bool          FCanRelpace;
	UnicodeString FOrderID;
	int           FLinesNo;
	UnicodeString FOrderType;
	UnicodeString FSide;
	UnicodeString FCommidityNo;
	UnicodeString FYearMonth;
	int           FQty;
	double        FPrice;
	UnicodeString FOrderTime;
	int           FTradeQty;
	UnicodeString FCallPut;  //Call, Put, None(Future)
	double        FStrikePrice;
	UnicodeString FOrderStatusCode;
	UnicodeString FCloseTrade;
	UnicodeString FOrderStatusDescription;
	UnicodeString FEntityType;
	bool          FIsSpread;
	UnicodeString FDayTradeOver;

public:
	TGoodWheatOrderMessage();
	TGoodWheatOrderMessage(_di_IXMLNode DataNode);

	bool          CanCancel() {return FCanCancel;};
	bool          CanRelpace() {return FCanRelpace;};
	UnicodeString GetOrderID() {return FOrderID;};
	int           GetLinesNo() {return FLinesNo;};
	UnicodeString GetOrderType() {return FOrderType;};
	UnicodeString GetSide() {return FSide;};
	UnicodeString GetCommidityNo() {return FCommidityNo;};
	UnicodeString GetYearMonth() {return FYearMonth;};
	int           GetQty() {return FQty;};
	double        GetPrice() {return FPrice;};
	UnicodeString GetOrderTime() {return FOrderTime;};
	int           GetTradeQty() {return FTradeQty;};
	UnicodeString GetCallPut() {return FCallPut;};
	double        GetStrikePrice() {return FStrikePrice;};
	UnicodeString GetOrderStatusCode() {return FOrderStatusCode;};
	UnicodeString GetCloseTrade() {return FCloseTrade;};
	UnicodeString GetOrderStatusDescription() {return FOrderStatusDescription;};
	UnicodeString GetEntityType() {return FEntityType;};
	bool          IsSpread() {return FIsSpread;};
	UnicodeString GetDayTradeOver() {return FDayTradeOver;};

	void SetCanCancel(bool CanCancel) {FCanCancel = CanCancel;};
	void SetCanRelpace(bool CanRelpace) {FCanRelpace = CanRelpace;};
	void SetOrderID(UnicodeString OrderID) {FOrderID = OrderID;};
	void SetLinesNo(int LinesNo) {FLinesNo = LinesNo;};
	void SetOrderType(UnicodeString OrderType) {FOrderType = OrderType;};
	void SetSide(UnicodeString Side) {FSide = Side;};
	void SetCommidityNo(UnicodeString CommidityNo) {FCommidityNo = CommidityNo;};
	void SetYearMonth(UnicodeString YearMonth) {FYearMonth = YearMonth;};
	void SetQty(int Qty) {FQty = Qty;};
	void SetPrice(double Price) {FPrice = Price;};
	void SetOrderTime(UnicodeString OrderTime) {FOrderTime = OrderTime;};
	void SetTradeQty(int TradeQty) {FTradeQty = TradeQty;};
	void SetCallPut(UnicodeString CallPut) {FCallPut = CallPut;};
	void SetStrikePrice(double StrikePrice) {FStrikePrice = StrikePrice;};
	void SetOrderStatusCode(UnicodeString OrderStatusCode) {FOrderStatusCode = OrderStatusCode;};
	void SetCloseTrade(UnicodeString CloseTrade) {FCloseTrade = CloseTrade;};
	void SetOrderStatusDescription(UnicodeString OrderStatusDescription) {FOrderStatusDescription = OrderStatusDescription;};
	void SetEntityType(UnicodeString EntityType) {FEntityType = EntityType;};
	void SetSpread(bool IsSpread) {FIsSpread = IsSpread;};
	void SetDayTradeOver(UnicodeString DayTradeOver) {FDayTradeOver = DayTradeOver;};

	void GetDataFromXML(_di_IXMLNode DataNode);
};  //TGoodWheatOrderMessage

//------------------------------------------------------------------------------
class TGoodWheatDailyOrderMessage
{
private:
	UnicodeString FMessageArriveTime;
	UnicodeString FUserKey;
	UnicodeString FBuyDescription;
	UnicodeString FSellDescription;
	UnicodeString FNewDescription;
	UnicodeString FOffsetDescription;
	UnicodeString FMarketDescription;
	UnicodeString FLimitDescription;
	UnicodeString FCancelDescription;
	UnicodeString FReplaceDescription;
	UnicodeString FInquireDescription;
	UnicodeString FAccountNo;
	UnicodeString FAccountName;
	UnicodeString FBrokerNo;
	UnicodeString FQueryEnvFlag;
	UnicodeString FErrorFlag;

	std::vector<TGoodWheatOrderMessage*> FOrderArray;
public:
	TGoodWheatDailyOrderMessage();
	TGoodWheatDailyOrderMessage(TXMLDocument* DataXML);
	~TGoodWheatDailyOrderMessage();
	void ClearData();

	UnicodeString GetMessageArriveTime() {return FMessageArriveTime;};
	UnicodeString GetUserKey() {return FUserKey;};
	UnicodeString GetBuyDescription() {return FBuyDescription;};
	UnicodeString GetSellDescription() {return FSellDescription;};
	UnicodeString GetNewDescription() {return FNewDescription;};
	UnicodeString GetOffsetDescription() {return FOffsetDescription;};
	UnicodeString GetMarketDescription() {return FMarketDescription;};
	UnicodeString GetLimitDescription() {return FLimitDescription;};
	UnicodeString GetCancelDescription() {return FCancelDescription;};
	UnicodeString GetReplaceDescription() {return FReplaceDescription;};
	UnicodeString GetInquireDescription() {return FInquireDescription;};
	UnicodeString GetAccountNo() {return FAccountNo;};
	UnicodeString GetAccountName() {return FAccountName;};
	UnicodeString GetBrokerNo() {return FBrokerNo;};
	UnicodeString GetQueryEnvFlag() {return FQueryEnvFlag;};
	UnicodeString GetErrorFlag() {return FErrorFlag;};

	void SetMessageArriveTime(UnicodeString MessageArriveTime) {FMessageArriveTime = MessageArriveTime;};
	void SetUserKey(UnicodeString UserKey) {FUserKey = UserKey;};
	void SetBuyDescription(UnicodeString BuyDescription) {FBuyDescription = BuyDescription;};
	void SetSellDescription(UnicodeString SellDescription) {FSellDescription = SellDescription;};
	void SetNewDescription(UnicodeString NewDescription) {FNewDescription = NewDescription;};
	void SetOffsetDescription(UnicodeString OffsetDescription) {FOffsetDescription = OffsetDescription;};
	void SetMarketDescription(UnicodeString MarketDescription) {FMarketDescription = MarketDescription;};
	void SetLimitDescription(UnicodeString LimitDescription) {FLimitDescription = LimitDescription;};
	void SetCancelDescription(UnicodeString CancelDescription) {FCancelDescription = CancelDescription;};
	void SetReplaceDescription(UnicodeString ReplaceDescription) {FReplaceDescription = ReplaceDescription;};
	void SetInquireDescription(UnicodeString InquireDescription) {FInquireDescription = InquireDescription;};
	void SetAccountNo(UnicodeString AccountNo) {FAccountNo = AccountNo;};
	void SetAccountName(UnicodeString AccountName) {FAccountName = AccountName;};
	void SetBrokerNo(UnicodeString BrokerNo) {FBrokerNo = BrokerNo;};

	void GetDataFromXML(TXMLDocument* DataXML);
	int GetMessageCount() {return FOrderArray.size();};
	TGoodWheatOrderMessage* GetMessageByIndexNo(int IndexNo) {return FOrderArray[IndexNo];};
};  //TGoodWheatDailyOrderMessage()

//------------------------------------------------------------------------------
class TGoodWheatFillMessage
{
private:
	int           FOrderSequenceNo;
	UnicodeString FAccountNo;
	UnicodeString FOrderID;
	UnicodeString FAeNo;
	UnicodeString FTradeDate;
	int           FMatchQty;
	double        FMatchPrice;
	UnicodeString FSide;
	UnicodeString FCommidityNo;
	UnicodeString FYearMonth;
	UnicodeString FTradeTime;
	UnicodeString FMarketType;  //0:Futures  1:Options
	double        FStrikePrice;
	UnicodeString FCallPut;  //Call, Put, None(Future)
	UnicodeString FDayTradeOver;

public:
	TGoodWheatFillMessage();
	TGoodWheatFillMessage(_di_IXMLNode DataNode);

	int           GetOrderSequenceNo() {return FOrderSequenceNo;};
	UnicodeString GetAccountNo() {return FAccountNo;};
	UnicodeString GetOrderID() {return FOrderID;};
	UnicodeString GetAeNo() {return FAeNo;};
	UnicodeString GetTradeDate() {return FTradeDate;};
	int           GetMatchQty() {return FMatchQty;};
	double        GetMatchPrice() {return FMatchPrice;};
	UnicodeString GetSide() {return FSide;};
	UnicodeString GetCommidityNo() {return FCommidityNo;};
	UnicodeString GetYearMonth() {return FYearMonth;};
	UnicodeString GetTradeTime() {return FTradeTime;};
	UnicodeString GetMarketType() {return FMarketType;};
	double        GetStrikePrice() {return FStrikePrice;};
	UnicodeString GetCallPut() {return FCallPut;};
	UnicodeString GetDayTradeOver() {return FDayTradeOver;};

	void SetOrderSequenceNo(int OrderSequenceNo) {FOrderSequenceNo = OrderSequenceNo;};
	void SetAccountNo(UnicodeString AccountNo) {FAccountNo = AccountNo;};
	void SetOrderID(UnicodeString OrderID) {FOrderID = OrderID;};
	void SetAeNo(UnicodeString AeNo) {FAeNo = AeNo;};
	void SetTradeDate(UnicodeString TradeDate) {FTradeDate = TradeDate;};
	void SetMatchQty(int MatchQty) {FMatchQty = MatchQty;};
	void SetMatchPrice(double MatchPrice) {FMatchPrice = MatchPrice;};
	void SetSide(UnicodeString Side) {FSide = Side;};
	void SetCommidityNo(UnicodeString CommidityNo) {FCommidityNo = CommidityNo;};
	void SetYearMonth(UnicodeString YearMonth) {FYearMonth = YearMonth;};
	void SetTradeTime(UnicodeString TradeTime) {FTradeTime = TradeTime;};
	void SetMarketType(UnicodeString MarketType) {FMarketType = MarketType;};
	void SetStrikePrice(double StrikePrice) {FStrikePrice = StrikePrice;};
	void SetCallPut(UnicodeString CallPut) {FCallPut = CallPut;};
	void SetDayTradeOver(UnicodeString DayTradeOver) {FDayTradeOver = DayTradeOver;};

	void GetDataFromXML(_di_IXMLNode DataNode);
};  //TGoodWheatFillMessage

//------------------------------------------------------------------------------
class TGoodWheatDailyFillMessage
{
private:
	UnicodeString FMessageArriveTime;
	UnicodeString FUserKey;
	UnicodeString FAccountNo;
	UnicodeString FQueryEnvFlag;
	UnicodeString FErrorFlag;

	std::vector<TGoodWheatFillMessage*> FFillArray;
public:
	TGoodWheatDailyFillMessage();
	TGoodWheatDailyFillMessage(TXMLDocument* DataXML);
	~TGoodWheatDailyFillMessage();
	void ClearData();

	UnicodeString GetMessageArriveTime() {return FMessageArriveTime;};
	UnicodeString GetUserKey() {return FUserKey;};
	UnicodeString GetAccountNo() {return FAccountNo;};
	UnicodeString GetQueryEnvFlag() {return FQueryEnvFlag;};
	UnicodeString GetErrorFlag() {return FErrorFlag;};

	void SetMessageArriveTime(UnicodeString MessageArriveTime) {FMessageArriveTime = MessageArriveTime;};
	void SetUserKey(UnicodeString UserKey) {FUserKey = UserKey;};
	void SetAccountNo(UnicodeString AccountNo) {FAccountNo = AccountNo;};

	void GetDataFromXML(TXMLDocument* DataXML);
	int GetMessageCount() {return FFillArray.size();};
	TGoodWheatFillMessage* GetMessageByIndexNo(int IndexNo) {return FFillArray[IndexNo];};
};  //TGoodWheatDailyFillMessage()

//------------------------------------------------------------------------------
class TGoodWheatDayTradeMessage
{
private:
	int           FOrderSequenceNo;
	UnicodeString FOrderID;
	UnicodeString FCommidityNo;
	UnicodeString FYearMonth;
	double        FStrikePrice;
	UnicodeString FCallPut;  //Call, Put, None(Future)
	UnicodeString FSide;
	UnicodeString FExchange;
	UnicodeString FTradeDate;
	double        FMatchPrice;
	int           FMatchQty;
	double        FPrtlos;
	UnicodeString FCurrency;

public:
	TGoodWheatDayTradeMessage();
	TGoodWheatDayTradeMessage(_di_IXMLNode DataNode);

	int           GetOrderSequenceNo() {return FOrderSequenceNo;};
	UnicodeString GetOrderID() {return FOrderID;};
	UnicodeString GetCommidityNo() {return FCommidityNo;};
	UnicodeString GetYearMonth() {return FYearMonth;};
	double        GetStrikePrice() {return FStrikePrice;};
	UnicodeString GetCallPut() {return FCallPut;};
	UnicodeString GetSide() {return FSide;};
	UnicodeString GetExchange() {return FExchange;};
	UnicodeString GetTradeDate() {return FTradeDate;};
	double        GetMatchPrice() {return FMatchPrice;};
	int           GetMatchQty() {return FMatchQty;};
	double        GetPrtlos() {return FPrtlos;};
	UnicodeString GetCurrency() {return FCurrency;};

	void SetOrderSequenceNo(int OrderSequenceNo) {FOrderSequenceNo = OrderSequenceNo;};
	void SetOrderID(UnicodeString OrderID) {FOrderID = OrderID;};
	void SetCommidityNo(UnicodeString CommidityNo) {FCommidityNo = CommidityNo;};
	void SetYearMonth(UnicodeString YearMonth) {FYearMonth = YearMonth;};
	void SetStrikePrice(double StrikePrice) {FStrikePrice = StrikePrice;};
	void SetCallPut(UnicodeString CallPut) {FCallPut = CallPut;};
	void SetSide(UnicodeString Side) {FSide = Side;};
	void SetExchange(UnicodeString Exchange) {FExchange = Exchange;};
	void SetTradeDate(UnicodeString TradeDate) {FTradeDate = TradeDate;};
	void SetMatchPrice(double MatchPrice) {FMatchPrice = MatchPrice;};
	void SetMatchQty(int MatchQty) {FMatchQty = MatchQty;};
	void SetPrtlos(double Prtlos) {FPrtlos = Prtlos;};
	void SetCurrency(UnicodeString Currency) {FCurrency = Currency;};

	void GetDataFromXML(_di_IXMLNode DataNode);
};  //TGoodWheatDayTradeMessage

//------------------------------------------------------------------------------
class TGoodWheatDailyDayTradeMessage
{
private:
	UnicodeString FMessageArriveTime;
	UnicodeString FUserKey;
	UnicodeString FBrokerNo;
	UnicodeString FAccountNo;
	UnicodeString FQueryEnvFlag;
	UnicodeString FErrorFlag;

	std::vector<TGoodWheatDayTradeMessage*> FDayTradeArray;
public:
	TGoodWheatDailyDayTradeMessage();
	TGoodWheatDailyDayTradeMessage(TXMLDocument* DataXML);
	~TGoodWheatDailyDayTradeMessage();
	void ClearData();

	UnicodeString GetMessageArriveTime() {return FMessageArriveTime;};
	UnicodeString GetUserKey() {return FUserKey;};
	UnicodeString GetBrokerNo() {return FBrokerNo;};
	UnicodeString GetAccountNo() {return FAccountNo;};
	UnicodeString GetQueryEnvFlag() {return FQueryEnvFlag;};
	UnicodeString GetErrorFlag() {return FErrorFlag;};

	void SetMessageArriveTime(UnicodeString MessageArriveTime) {FMessageArriveTime = MessageArriveTime;};
	void SetUserKey(UnicodeString UserKey) {FUserKey = UserKey;};
	void SetBrokerNo(UnicodeString BrokerNo) {FBrokerNo = BrokerNo;};
	void SetAccountNo(UnicodeString AccountNo) {FAccountNo = AccountNo;};

	void GetDataFromXML(TXMLDocument* DataXML);
	int GetMessageCount() {return FDayTradeArray.size();};
	TGoodWheatDayTradeMessage* GetMessageByIndexNo(int IndexNo) {return FDayTradeArray[IndexNo];};
};  //TGoodWheatDailyDayTradeMessage()

//------------------------------------------------------------------------------
class TGoodWheatOpenInterestMessage
{
private:
	int           FOrderSequenceNo;
	UnicodeString FOrderID;
	UnicodeString FAccountNo;
	UnicodeString FTradeDate;
	UnicodeString FSide;
	UnicodeString FCommidityNo;
	UnicodeString FYearMonth;
	double        FMatchPrice;
	int           FOpenInterest;
	double        FMarketPrice;
	double        FPrtlos;
	UnicodeString FMarketType;  //0:Futures  1:Options
	double        FStrikePrice;
	UnicodeString FCallPut;  //Call, Put, None(Future)
	bool          FIsSpread;
	UnicodeString FDayTradeOver;

public:
	TGoodWheatOpenInterestMessage();
	TGoodWheatOpenInterestMessage(_di_IXMLNode DataNode);

	int           GetOrderSequenceNo() {return FOrderSequenceNo;};
	UnicodeString GetOrderID() {return FOrderID;};
	UnicodeString GetAccountNo() {return FAccountNo;};
	UnicodeString GetTradeDate() {return FTradeDate;};
	UnicodeString GetSide() {return FSide;};
	UnicodeString GetCommidityNo() {return FCommidityNo;};
	UnicodeString GetYearMonth() {return FYearMonth;};
	double        GetMatchPrice() {return FMatchPrice;};
	int           GetOpenInterest() {return FOpenInterest;};
	double        GetMarketPrice() {return FMarketPrice;};
	double        GetPrtlos() {return FPrtlos;};
	UnicodeString GetMarketType() {return FMarketType;};
	double        GetStrikePrice() {return FStrikePrice;};
	UnicodeString GetCallPut() {return FCallPut;};
	bool          IsSpread() {return FIsSpread;};
	UnicodeString GetDayTradeOver() {return FDayTradeOver;};

	void SetOrderSequenceNo(int OrderSequenceNo) {FOrderSequenceNo = OrderSequenceNo;};
	void SetOrderID(UnicodeString OrderID) {FOrderID = OrderID;};
	void SetAccountNo(UnicodeString AccountNo) {FAccountNo = AccountNo;};
	void SetTradeDate(UnicodeString TradeDate) {FTradeDate = TradeDate;};
	void SetSide(UnicodeString Side) {FSide = Side;};
	void SetCommidityNo(UnicodeString CommidityNo) {FCommidityNo = CommidityNo;};
	void SetYearMonth(UnicodeString YearMonth) {FYearMonth = YearMonth;};
	void SetMatchPrice(double MatchPrice) {FMatchPrice = MatchPrice;};
	void SetOpenInterest(int OpenInterest) {FOpenInterest = OpenInterest;};
	void SetMarketPrice(double MarketPrice) {FMarketPrice = MarketPrice;};
	void SetPrtlos(double Prtlos) {FPrtlos = Prtlos;};
	void GetMarketType(UnicodeString MarketType) {FMarketType = MarketType;};
	void SetStrikePrice(double StrikePrice) {FStrikePrice = StrikePrice;};
	void SetCallPut(UnicodeString CallPut) {FCallPut = CallPut;};
	void SetSpread(bool Spread) {FIsSpread = Spread;};
	void GetDayTradeOver(UnicodeString DayTradeOver) {FDayTradeOver = DayTradeOver;};

	UnicodeString GetSymbolCode();
	void GetDataFromXML(_di_IXMLNode DataNode);
};  //TGoodWheatOpenInterestMessage

//------------------------------------------------------------------------------
class TGoodWheatDailyOpenInterestMessage
{
private:
	UnicodeString FMessageArriveTime;
	UnicodeString FUserKey;
	UnicodeString FCalTime;
	UnicodeString FAccountNo;
	UnicodeString FQueryEnvFlag;
	UnicodeString FErrorFlag;

	std::vector<TGoodWheatOpenInterestMessage*> FOpenInterestArray;
public:
	TGoodWheatDailyOpenInterestMessage();
	TGoodWheatDailyOpenInterestMessage(TXMLDocument* DataXML);
	~TGoodWheatDailyOpenInterestMessage();
	void ClearData();

	UnicodeString GetMessageArriveTime() {return FMessageArriveTime;};
	UnicodeString GetUserKey() {return FUserKey;};
	UnicodeString GetCalTime() {return FCalTime;};
	UnicodeString GetAccountNo() {return FAccountNo;};
	UnicodeString GetQueryEnvFlag() {return FQueryEnvFlag;};
	UnicodeString GetErrorFlag() {return FErrorFlag;};

	void SetMessageArriveTime(UnicodeString MessageArriveTime) {FMessageArriveTime = MessageArriveTime;};
	void SetUserKey(UnicodeString UserKey) {FUserKey = UserKey;};
	void SetCalTime(UnicodeString CalTime) {FCalTime = CalTime;};
	void SetAccountNo(UnicodeString AccountNo) {FAccountNo = AccountNo;};

	void GetDataFromXML(TXMLDocument* DataXML);
	int GetMessageCount() {return FOpenInterestArray.size();};
	TGoodWheatOpenInterestMessage* GetMessageByIndexNo(int IndexNo) {return FOpenInterestArray[IndexNo];};
};  //TGoodWheatDailyOpenInterestMessage()

//------------------------------------------------------------------------------
class TGoodWheatPositionMessage
{
private:
	UnicodeString FCommidityNo;
	UnicodeString FYearMonth;
	UnicodeString FCallPut;  //Call, Put, None(Future)
	double        FStrikePrice;
	double        FMarketPrice;
	int           FBSOptQty;  //琎ら痙计--禦
	int           FSSOptQty;  //琎ら痙计--芥
	int           FBORdQty;
	int           FSORdQty;
	int           FBTRdQty;
	double        FBTRdPrice;
	int           FSTRdQty;
	double        FSTRdPrice;
	int           FBOStQty;
	int           FSOStQty;
	int           FBCOptQty;
	int           FSCOptQty;
	double        FBCOptPrice;
	double        FSCOptPrice;
	bool          FIsSpread;
	UnicodeString FCT;
	UnicodeString FExchange;
	double        FOSPrtlos;
	UnicodeString FCurrency;
	double        FBSOptPrice;  //琎ら痙Θユ基璸--禦
	double        FSSOptPrice;  //琎ら痙Θユ基璸--芥

public:
	TGoodWheatPositionMessage();
	TGoodWheatPositionMessage(_di_IXMLNode DataNode);

	UnicodeString GetCommidityNo() {return FCommidityNo;};
	UnicodeString GetYearMonth() {return FYearMonth;};
	UnicodeString GetCallPut() {return FCallPut;};
	double        GetStrikePrice() {return FStrikePrice;};
	double        GetMarketPrice() {return FMarketPrice;};
	int           GetBSOptQty() {return FBSOptQty;};
	int           GetSSOptQty() {return FSSOptQty;};
	int           GetBORdQty() {return FBORdQty;};
	int           GetSORdQty() {return FSORdQty;};
	int           GetBTRdQty() {return FBTRdQty;};
	double        GetBTRdPrice() {return FBTRdPrice;};
	int           GetSTRdQty() {return FSTRdQty;};
	double        GetSTRdPrice() {return FSTRdPrice;};
	int           GetBOStQty() {return FBOStQty;};
	int           GetSOStQty() {return FSOStQty;};
	int           GetBCOptQty() {return FBCOptQty;};
	int           GetSCOptQty() {return FSCOptQty;};
	double        GetBCOptPrice() {return FBCOptPrice;};
	double        GetSCOptPrice() {return FSCOptPrice;};
	bool          IsSpread() {return FIsSpread;};
	UnicodeString GetCT() {return FCT;};
	UnicodeString GetExchange() {return FExchange;};
	double        GetOSPrtlos() {return FOSPrtlos;};
	UnicodeString GetCurrency() {return FCurrency;};
	double        GetBSOptPrice() {return FBSOptPrice;};
	double        GetSSOptPrice() {return FSSOptPrice;};
	UnicodeString GetSymbolCode();

	void SetCommidityNo(UnicodeString CommidityNo) {FCommidityNo = CommidityNo;};
	void SetYearMonth(UnicodeString YearMonth) {FYearMonth = YearMonth;};
	void SetCallPut(UnicodeString CallPut) {FCallPut = CallPut;};
	void SetStrikePrice(double StrikePrice) {FStrikePrice = StrikePrice;};
	void SetMarketPrice(double MarketPrice) {FMarketPrice = MarketPrice;};
	void SetBSOptQty(int BSOptQty) {FBSOptQty = BSOptQty;};
	void SetSSOptQty(int SSOptQty) {FSSOptQty = SSOptQty;};
	void SetBORdQty(int BORdQty) {FBORdQty = BORdQty;};
	void SetSORdQty(int SORdQty) {FSORdQty = SORdQty;};
	void SetBTRdQty(int BTRdQty) {FBTRdQty = BTRdQty;};
	void SetBTRdPrice(double BTRdPrice) {FBTRdPrice = BTRdPrice;};
	void SetSTRdQty(int STRdQty) {FSTRdQty = STRdQty;};
	void SetSTRdPrice(double STRdPrice) {FSTRdPrice = STRdPrice;};
	void SetBOStQty(int BOStQty) {FBOStQty = BOStQty;};
	void SetSOStQty(int SOStQty) {FSOStQty = SOStQty;};
	void SetBCOptQty(int BCOptQty) {FBCOptQty = BCOptQty;};
	void SetSCOptQty(int SCOptQty) {FSCOptQty = SCOptQty;};
	void SetBCOptPrice(double BCOptPrice) {FBCOptPrice = BCOptPrice;};
	void SetSCOptPrice(double SCOptPrice) {FSCOptPrice = SCOptPrice;};
	void SetSpread(bool IsSpread) {FIsSpread = IsSpread;};
	void SetCT(UnicodeString CT) {FCT = CT;};
	void SetExchange(UnicodeString Exchange) {FExchange = Exchange;};
	void SetOSPrtlos(double OSPrtlos) {FOSPrtlos = OSPrtlos;};
	void SetCurrency(UnicodeString Currency) {FCurrency = Currency;};

	void GetDataFromXML(_di_IXMLNode DataNode);
};  //TGoodWheatPositionMessage

//------------------------------------------------------------------------------
class TGoodWheatDailyPositionMessage
{
private:
	UnicodeString FMessageArriveTime;
	UnicodeString FUserKey;
	UnicodeString FAccountNo;
	UnicodeString FAccountName;
	UnicodeString FBrokerNo;
	UnicodeString FCalTime;
	UnicodeString FQueryEnvFlag;
	UnicodeString FErrorFlag;

	std::vector<TGoodWheatPositionMessage*> FPositionArray;
public:
	TGoodWheatDailyPositionMessage();
	TGoodWheatDailyPositionMessage(TXMLDocument* DataXML);
	~TGoodWheatDailyPositionMessage();
	void ClearData();

	UnicodeString GetMessageArriveTime() {return FMessageArriveTime;};
	UnicodeString GetUserKey() {return FUserKey;};
	UnicodeString GetAccountNo() {return FAccountNo;};
	UnicodeString GetAccountName() {return FAccountName;};
	UnicodeString GetBrokerNo() {return FBrokerNo;};
	UnicodeString GetCalTime() {return FCalTime;};
	UnicodeString GetQueryEnvFlag() {return FQueryEnvFlag;};
	UnicodeString GetErrorFlag() {return FErrorFlag;};

	void SetMessageArriveTime(UnicodeString MessageArriveTime) {FMessageArriveTime = MessageArriveTime;};
	void SetUserKey(UnicodeString UserKey) {FUserKey = UserKey;};
	void SetAccountNo(UnicodeString AccountNo) {FAccountNo = AccountNo;};
	void SetAccountName(UnicodeString AccountName) {FAccountName = AccountName;};
	void SetBrokerNo(UnicodeString BrokerNo) {FBrokerNo = BrokerNo;};
	void SetCalTime(UnicodeString CalTime) {FCalTime = CalTime;};

	void GetDataFromXML(TXMLDocument* DataXML);
	int GetMessageCount() {return FPositionArray.size();};
	TGoodWheatPositionMessage* GetMessageByIndexNo(int IndexNo) {return FPositionArray[IndexNo];};
};  //TGoodWheatDailyPositionMessage()

//------------------------------------------------------------------------------
class TGoodWheatMarginMessage
{
private:
	UnicodeString FAccountNo;
	UnicodeString FAccountName;
	UnicodeString FTactNo;
	UnicodeString FBroker;
	UnicodeString FCurrency;
	UnicodeString FCalDate;
	UnicodeString FCalTime;
	double        FCtdab;
	double        FDwamt;
	double        FOsprtlos;
	double        FOsprtlos_c;
	double        FFee;
	double        FAmt;
	double        FPremium;
	double        FExrate;
	double        FOrdmarg;
	double        FVal;
	double        FPrtlos;
	double        FPrtlos_c;
	double        FTmiamt;
	double        FMrgRate;
	double        FTmexcess1;
	double        FTmmamt;
	double        FMatnRate;
	double        FTmexcess;
	double        FOptequity;
	double        FOptRate;
	double        FTmexcess2;
	double        FMktval;
	double        FMktvalb;
	double        FMktvals;
	UnicodeString FMgnkind;
	UnicodeString FCofkind;
	double        FTmsciamt;
	double        FTmcofuse;
	double        FTmcofusefree;
	double        FPreintabf;
	double        FNtval;
	double        FNtmktval;
	double        FNtexcess1;
	double        FNtexcess2;
	double        FNtexcess3;
	double        FNtrate_a;
	double        FNtrate_b;

public:
	TGoodWheatMarginMessage();
	TGoodWheatMarginMessage(_di_IXMLNode DataNode);

	UnicodeString GetAccountNo() {return FAccountNo;};
	UnicodeString GetAccountName() {return FAccountName;};
	UnicodeString GetTactNo() {return FTactNo;};
	UnicodeString GetBroker() {return FBroker;};
	UnicodeString GetCurrency() {return FCurrency;};
	UnicodeString GetCalDate() {return FCalDate;};
	UnicodeString GetCalTime() {return FCalTime;};
	double GetCtdab() {return FCtdab;};
	double GetDwamt() {return FDwamt;};
	double GetOsprtlos() {return FOsprtlos;};
	double GetOsprtlos_c() {return FOsprtlos_c;};
	double GetFee() {return FFee;};
	double GetAmt() {return FAmt;};
	double GetPremium() {return FPremium;};
	double GetExrate() {return FExrate;};
	double GetOrdmarg() {return FOrdmarg;};
	double GetVal() {return FVal;};
	double GetPrtlos() {return FPrtlos;};
	double GetPrtlos_c() {return FPrtlos_c;};
	double GetTmiamt() {return FTmiamt;};
	double GetMrgRate() {return FMrgRate;};
	double GetTmexcess1() {return FTmexcess1;};
	double GetTmmamt() {return FTmmamt;};
	double GetMatnRate() {return FMatnRate;};
	double GetTmexcess() {return FTmexcess;};
	double GetOptequity() {return FOptequity;};
	double GetOptRate() {return FOptRate;};
	double GetTmexcess2() {return FTmexcess2;};
	double GetMktval() {return FMktval;};
	double GetMktvalb() {return FMktvalb;};
	double GetMktvals() {return FMktvals;};
	UnicodeString GetMgnkind() {return FMgnkind;};
	UnicodeString GetCofkind() {return FCofkind;};
	double GetTmsciamt() {return FTmsciamt;};
	double GetTmcofuse() {return FTmcofuse;};
	double GetTmcofusefree() {return FTmcofusefree;};
	double GetPreintabf() {return FPreintabf;};
	double GetNtval() {return FNtval;};
	double GetNtmktval() {return FNtmktval;};
	double GetNtexcess1() {return FNtexcess1;};
	double GetNtexcess2() {return FNtexcess2;};
	double GetNtexcess3() {return FNtexcess3;};
	double GetNtrate_a() {return FNtrate_a;};
	double GetNtrate_b() {return FNtrate_b;};

	void SetAccountNo(UnicodeString AccountNo) {FAccountNo = AccountNo;};
	void SetTactNo(UnicodeString TactNo) {FTactNo = TactNo;};
	void SetBroker(UnicodeString Broker) {FBroker = Broker;};
	void SetCurrency(UnicodeString Currency) {FCurrency = Currency;};
	void SetCalDate(UnicodeString CalDate) {FCalDate = CalDate;};
	void SetCalTime(UnicodeString CalTime) {FCalTime = CalTime;};
	void SetCtdab(double Ctdab) {FCtdab = Ctdab;};
	void SetDwamt(double Dwamt) {FDwamt = Dwamt;};
	void SetOsprtlos(double Osprtlos) {FOsprtlos = Osprtlos;};
	void SetOsprtlos_c(double Osprtlos_c) {FOsprtlos_c = Osprtlos_c;};
	void SetFee(double Fee) {FFee = Fee;};
	void SetAmt(double Amt) {FAmt = Amt;};
	void SetPremium(double Premium) {FPremium = Premium;};
	void SetExrate(double Exrate) {FExrate = Exrate;};
	void SetOrdmarg(double Ordmarg) {FOrdmarg = Ordmarg;};
	void SetVal(double Val) {FVal = Val;};
	void SetPrtlos(double Prtlos) {FPrtlos = Prtlos;};
	void SetPrtlos_c(double Prtlos_c) {FPrtlos_c = Prtlos_c;};
	void SetTmiamt(double Tmiamt) {FTmiamt = Tmiamt;};
	void SetMrgRate(double MrgRate) {FMrgRate = MrgRate;};
	void SetTmexcess1(double Tmexcess1) {FTmexcess1 = Tmexcess1;};
	void SetTmmamt(double Tmmamt) {FTmmamt = Tmmamt;};
	void SetMatnRate(double MatnRate) {FMatnRate = MatnRate;};
	void SetTmexcess(double Tmexcess) {FTmexcess = Tmexcess;};
	void SetOptequity(double Optequity) {FOptequity = Optequity;};
	void SetOptRate(double OptRate) {FOptRate = OptRate;};
	void SetTmexcess2(double Tmexcess2) {FTmexcess2 = Tmexcess2;};
	void SetMktval(double Mktval) {FMktval = Mktval;};
	void SetMktvalb(double Mktvalb) {FMktvalb = Mktvalb;};
	void SetMktvals(double Mktvals) {FMktvals = Mktvals;};
	void SetMgnkind(UnicodeString Mgnkind) {FMgnkind = Mgnkind;};
	void SetCofkind(UnicodeString Cofkind) {FCofkind = Cofkind;};
	void SetTmsciamt(double Tmsciamt) {FTmsciamt = Tmsciamt;};
	void SetTmcofuse(double Tmcofuse) {FTmcofuse = Tmcofuse;};
	void SetTmcofusefree(double Tmcofusefree) {FTmcofusefree = Tmcofusefree;};
	void SetPreintabf(double Preintabf) {FPreintabf = Preintabf;};
	void SetNtval(double Ntval) {FNtval = Ntval;};
	void SetNtmktval(double Ntmktval) {FNtmktval = Ntmktval;};
	void SetNtexcess1(double Ntexcess1) {FNtexcess1 = Ntexcess1;};
	void SetNtexcess2(double Ntexcess2) {FNtexcess2 = Ntexcess2;};
	void SetNtexcess3(double Ntexcess3) {FNtexcess3 = Ntexcess3;};
	void SetNtrate_a(double Ntrate_a) {FNtrate_a = Ntrate_a;};
	void SetNtrate_b(double Ntrate_b) {FNtrate_b = Ntrate_b;};

	void GetDataFromXML(_di_IXMLNode DataNode);
};  //TGoodWheatMarginMessage

//------------------------------------------------------------------------------
class TGoodWheatOpenInterestMessage2
{
private:
	UnicodeString FOrderID;
	UnicodeString FTradeDate;
	UnicodeString FSide;
	int           FQty;
	UnicodeString FCommidityNo;
	UnicodeString FYearMonth;
	UnicodeString FFirm;
	UnicodeString FDayTradeOver;
	UnicodeString FCallPut;  //Call, Put, None(Future)
	bool          FIsSpread;
	double        FMatchPrice;
	double        FMargin;
	double        FPrtlos;
	double        FMarketPrice1;
	double        FStrikePrice;

public:
	TGoodWheatOpenInterestMessage2();
	TGoodWheatOpenInterestMessage2(_di_IXMLNode DataNode);

	UnicodeString GetOrderID() {return FOrderID;};
	UnicodeString GetTradeDate() {return FTradeDate;};
	UnicodeString GetSide() {return FSide;};
	int           GetQty() {return FQty;};
	UnicodeString GetCommidityNo() {return FCommidityNo;};
	UnicodeString GetYearMonth() {return FYearMonth;};
	UnicodeString GetFirm() {return FFirm;};
	UnicodeString GetDayTradeOver() {return FDayTradeOver;};
	UnicodeString GetCallPut() {return FCallPut;};
	bool          IsSpread() {return FIsSpread;};
	double        GetMatchPrice() {return FMatchPrice;};
	double        GetMargin() {return FMargin;};
	double        GetPrtlos() {return FPrtlos;};
	double        GetMarketPrice1() {return FMarketPrice1;};
	double        GetStrikePrice() {return FStrikePrice;};

	void SetOrderID(UnicodeString OrderID) {FOrderID = OrderID;};
	void SetTradeDate(UnicodeString TradeDate) {FTradeDate = TradeDate;};
	void SetSide(UnicodeString Side) {FSide = Side;};
	void SetCommidityNo(UnicodeString CommidityNo) {FCommidityNo = CommidityNo;};
	void SetYearMonth(UnicodeString YearMonth) {FYearMonth = YearMonth;};
	void SetFirm(UnicodeString Firm) {FFirm = Firm;};
	void GetDayTradeOver(UnicodeString DayTradeOver) {FDayTradeOver = DayTradeOver;};
	void SetCallPut(UnicodeString CallPut) {FCallPut = CallPut;};
	void SetSpread(bool Spread) {FIsSpread = Spread;};
	void SetMatchPrice(double MatchPrice) {FMatchPrice = MatchPrice;};
	void SetMargin(double Margin) {FMargin = Margin;};
	void SetPrtlos(double Prtlos) {FPrtlos = Prtlos;};
	void SetMarketPrice1(double MarketPrice1) {FMarketPrice1 = MarketPrice1;};
	void SetStrikePrice(double StrikePrice) {FStrikePrice = StrikePrice;};

	void GetDataFromXML(_di_IXMLNode DataNode);
};  //TGoodWheatOpenInterestMessage2

//------------------------------------------------------------------------------
class TGoodWheatDailyMarginMessage
{
private:
	UnicodeString FMessageArriveTime;
	UnicodeString FUserKey;
	UnicodeString FMemberType;
	UnicodeString FAccountNo;
	UnicodeString FBrokerNo;
	UnicodeString FCurrency;
	UnicodeString FQueryEnvFlag;
	UnicodeString FErrorFlag;

	std::vector<TGoodWheatMarginMessage*> FMarginArray;
	std::vector<TGoodWheatOpenInterestMessage2*> FOpenInterestArray;
public:
	TGoodWheatDailyMarginMessage();
	TGoodWheatDailyMarginMessage(TXMLDocument* DataXML);
	~TGoodWheatDailyMarginMessage();
	void ClearData();

	UnicodeString GetMessageArriveTime() {return FMessageArriveTime;};
	UnicodeString GetUserKey() {return FUserKey;};
	UnicodeString GetMemberType() {return FMemberType;};
	UnicodeString GetAccountNo() {return FAccountNo;};
	UnicodeString GetBrokerNo() {return FBrokerNo;};
	UnicodeString GetCurrency() {return FCurrency;};
	UnicodeString GetQueryEnvFlag() {return FQueryEnvFlag;};
	UnicodeString GetErrorFlag() {return FErrorFlag;};

	void SetMessageArriveTime(UnicodeString MessageArriveTime) {FMessageArriveTime = MessageArriveTime;};
	void SetUserKey(UnicodeString UserKey) {FUserKey = UserKey;};
	void SetMemberType(UnicodeString MemberType) {FMemberType = MemberType;};
	void SetAccountNo(UnicodeString AccountNo) {FAccountNo = AccountNo;};
	void SetBrokerNo(UnicodeString BrokerNo) {FBrokerNo = BrokerNo;};
	void SetCurrency(UnicodeString Currency) {FCurrency = Currency;};

	void GetDataFromXML(TXMLDocument* DataXML);
	int GetMarginMessageCount() {return FMarginArray.size();};
	TGoodWheatMarginMessage* GetMarginMessageByIndexNo(int IndexNo) {return FMarginArray[IndexNo];};
	int GetOpenInterestMessageCount() {return FOpenInterestArray.size();};
	TGoodWheatOpenInterestMessage2* GetOpenInterestMessageByIndexNo(int IndexNo) {return FOpenInterestArray[IndexNo];};
};  //TGoodWheatDailyMarginMessage()

//------------------------------------------------------------------------------
class TGoodWheatReportDetail
{
private:
	UnicodeString FAccountNo;
	UnicodeString FAccountName;
	UnicodeString FAeNo;
	UnicodeString FAeName;
	UnicodeString FCurrency;
	UnicodeString FTradeDate;
	int           FCount;
	double        FTdab;
	double        FLtab;
	double        FExcess;
	double        FIamt;
	double        FMamt;
	double        FSciamt;
	double        FCoftot;
	double        FCofuse;
	double        FPpayamt;
	double        FEquity;
	double        FPrtlos;
	double        FExrate;
	double        FMtosamt;
	int           FMtrdqty;
	double        FMfee;
	double        FMfee1;
	double        FMpremium1;
	int           FMtrdqty1;
	double        FYtosamt;
	int           FYtrdqty;
	double        FYfee;
	double        FYfee1;
	double        FYpremium1;
	int           FYtrdqty1;

public:
	TGoodWheatReportDetail();
	TGoodWheatReportDetail(_di_IXMLNode DataNode);

	UnicodeString GetAccountNo() {return FAccountNo;};
	UnicodeString GetAccountName() {return FAccountName;};
	UnicodeString GetAeNo() {return FAeNo;};
	UnicodeString GetAeName() {return FAeName;};
	UnicodeString GetCurrency() {return FCurrency;};
	UnicodeString GetTradeDate() {return FTradeDate;};
	int           GetCount() {return FCount;};
	double        GetTdab() {return FTdab;};
	double        GetLtab() {return FLtab;};
	double        GetExcess() {return FExcess;};
	double        GetIamt() {return FIamt;};
	double        GetMamt() {return FMamt;};
	double        GetSciamt() {return FSciamt;};
	double        GetCoftot() {return FCoftot;};
	double        GetCofuse() {return FCofuse;};
	double        GetPpayamt() {return FPpayamt;};
	double        GetEquity() {return FEquity;};
	double        GetPrtlos() {return FPrtlos;};
	double        GetExrate() {return FExrate;};
	double        GetMtosamt() {return FMtosamt;};
	int           GetMtrdqty() {return FMtrdqty;};
	double        GetMfee() {return FMfee;};
	double        GetMfee1() {return FMfee1;};
	double        GetMpremium1() {return FMpremium1;};
	int           GetMtrdqty1() {return FMtrdqty1;};
	double        GetYtosamt() {return FYtosamt;};
	int           GetYtrdqty() {return FYtrdqty;};
	double        GetYfee() {return FYfee;};
	double        GetYfee1() {return FYfee1;};
	double        GetYpremium1() {return FYpremium1;};
	int           GetYtrdqty1() {return FYtrdqty1;};

	void SetAccountNo(UnicodeString AccountNo) {FAccountNo = AccountNo;};
	void SetAccountName(UnicodeString AccountName) {FAccountName = AccountName;};
	void SetAeNo(UnicodeString AeNo) {FAeNo = AeNo;};
	void SetAeName(UnicodeString AeName) {FAeName = AeName;};
	void SetCurrency(UnicodeString Currency) {FCurrency = Currency;};
	void SetTradeDate(UnicodeString TradeDate) {FTradeDate = TradeDate;};
	void SetCount(int Count) {FCount = Count;};
	void SetTdab(double Tdab) {FTdab = Tdab;};
	void SetLtab(double Ltab) {FLtab = Ltab;};
	void SetExcess(double Excess) {FExcess = Excess;};
	void SetIamt(double Iamt) {FIamt = Iamt;};
	void SetMamt(double Mamt) {FMamt = Mamt;};
	void SetSciamt(double Sciamt) {FSciamt = Sciamt;};
	void SetCoftot(double Coftot) {FCoftot = Coftot;};
	void SetCofuse(double Cofuse) {FCofuse = Cofuse;};
	void SetPpayamt(double Ppayamt) {FPpayamt = Ppayamt;};
	void SetEquity(double Equity) {FEquity = Equity;};
	void SetPrtlos(double Prtlos) {FPrtlos = Prtlos;};
	void SetExrate(double Exrate) {FExrate = Exrate;};
	void SetMtosamt(double Mtosamt) {FMtosamt = Mtosamt;};
	void SetMtrdqty(int Mtrdqty) {FMtrdqty = Mtrdqty;};
	void SetMfee(double Mfee) {FMfee = Mfee;};
	void SetMfee1(double Mfee1) {FMfee1 = Mfee1;};
	void SetMpremium1(double Mpremium1) {FMpremium1 = Mpremium1;};
	void SetMtrdqty1(int Mtrdqty1) {FMtrdqty1 = Mtrdqty1;};
	void SetYtosamt(double Ytosamt) {FYtosamt = Ytosamt;};
	void SetYtrdqty(int Ytrdqty) {FYtrdqty = Ytrdqty;};
	void SetYfee(double Yfee) {FYfee = Yfee;};
	void SetYfee1(double Yfee1) {FYfee1 = Yfee1;};
	void SetYpremium1(double Ypremium1) {FYpremium1 = Ypremium1;};
	void SetYtrdqty1(int Ytrdqty1) {FYtrdqty1 = Ytrdqty1;};

	void GetDataFromXML(_di_IXMLNode DataNode);
};  //TGoodWheatReportDetail

//------------------------------------------------------------------------------
class TGoodWheatBankDetail
{
private:
	UnicodeString FTxDate;
	UnicodeString FDescription;
	double        FNttxamt;
	UnicodeString FTxtp;
	UnicodeString FCdtype;

public:
	TGoodWheatBankDetail();
	TGoodWheatBankDetail(_di_IXMLNode DataNode);

	UnicodeString GetTxDate() {return FTxDate;};
	UnicodeString GetDescription() {return FDescription;};
	double        GetNttxamt() {return FNttxamt;};
	UnicodeString GetTxtp() {return FTxtp;};
	UnicodeString GetCdtype() {return FCdtype;};

	void SetTxDate(UnicodeString TxDate) {FTxDate = TxDate;};
	void SetDescription(UnicodeString Description) {FDescription = Description;};
	void SetNttxamt(double Nttxamt) {FNttxamt = Nttxamt;};
	void SetTxtp(UnicodeString Txtp) {FTxtp = Txtp;};
	void SetCdtype(UnicodeString Cdtype) {FCdtype = Cdtype;};

	void GetDataFromXML(_di_IXMLNode DataNode);
};  //TGoodWheatBankDetail

//------------------------------------------------------------------------------
class TGoodWheatFillDetail
{
private:
	UnicodeString FTradeDate;
	int           FBuyQty;
	int           FSellQty;
	UnicodeString FCommidityNo;
	UnicodeString FYearMonth;
	UnicodeString FFirm;
	double        FMatchPrice;
	double        FStrikePrice;
	UnicodeString FCallPut;  //Call, Put, None(Future)
	double        FPremium;

	double        FFeeSum;
	UnicodeString FProductKey;
public:
	TGoodWheatFillDetail();
	TGoodWheatFillDetail(_di_IXMLNode DataNode);

	UnicodeString GetTradeDate() {return FTradeDate;};
	int           GetBuyQty() {return FBuyQty;};
	int           GetSellQty() {return FSellQty;};
	UnicodeString GetCommidityNo() {return FCommidityNo;};
	UnicodeString GetYearMonth() {return FYearMonth;};
	UnicodeString GetFirm() {return FFirm;};
	double        GetMatchPrice() {return FMatchPrice;};
	double        GetStrikePrice() {return FStrikePrice;};
	UnicodeString GetCallPut() {return FCallPut;};
	double        GetPremium() {return FPremium;};
	double        GetFeeSum() {return FFeeSum;};
	UnicodeString GetProductKey() {return FProductKey;};

	void SetTradeDate(UnicodeString TradeDat) {FTradeDate = TradeDat;};
	void SetBuyQty(int BuyQty) {FBuyQty = BuyQty;};
	void SetSellQty(int SellQty) {FSellQty = SellQty;};
	void SetCommidityNo(UnicodeString CommidityNo) {FCommidityNo = CommidityNo;};
	void SetYearMonth(UnicodeString YearMonth) {FYearMonth = YearMonth;};
	void SetFirm(UnicodeString Firm) {FFirm = Firm;};
	void SetMatchPrice(double MatchPrice) {FMatchPrice = MatchPrice;};
	void SetStrikePrice(double StrikePrice) {FStrikePrice = StrikePrice;};
	void SetCallPut(UnicodeString CallPut) {FCallPut = CallPut;};
	void SetPremium(double Premium) {FPremium = Premium;};
	void SetFeeSum(double FeeSum) {FFeeSum = FeeSum;};
	void SetProductKey(UnicodeString ProductKey) {FProductKey = ProductKey;};

	void GetDataFromXML(_di_IXMLNode DataNode);
};  //TGoodWheatFillDetail

//------------------------------------------------------------------------------
class TGoodWheatTradeDetail
{
private:
	UnicodeString FTradeDate;
	int           FBuyQty;
	int           FSellQty;
	UnicodeString FCommidityNo;
	UnicodeString FYearMonth;
	double        FStrikePrice;
	UnicodeString FCallPut;  //Call, Put, None(Future)
	UnicodeString FFirm;
	double        FMatchPrice;
	double        FOrignps;

	UnicodeString FProductKey;
public:
	TGoodWheatTradeDetail();
	TGoodWheatTradeDetail(_di_IXMLNode DataNode);

	UnicodeString GetTradeDate() {return FTradeDate;};
	int           GetBuyQty() {return FBuyQty;};
	int           GetSellQty() {return FSellQty;};
	UnicodeString GetCommidityNo() {return FCommidityNo;};
	UnicodeString GetYearMonth() {return FYearMonth;};
	double        GetStrikePrice() {return FStrikePrice;};
	UnicodeString GetCallPut() {return FCallPut;};
	UnicodeString GetFirm() {return FFirm;};
	double        GetMatchPrice() {return FMatchPrice;};
	double        GetOrignps() {return FOrignps;};
	UnicodeString GetProductKey() {return FProductKey;};

	void SetTradeDate(UnicodeString TradeDat) {FTradeDate = TradeDat;};
	void SetBuyQty(int BuyQty) {FBuyQty = BuyQty;};
	void SetSellQty(int SellQty) {FSellQty = SellQty;};
	void SetCommidityNo(UnicodeString CommidityNo) {FCommidityNo = CommidityNo;};
	void SetYearMonth(UnicodeString YearMonth) {FYearMonth = YearMonth;};
	void SetStrikePrice(double StrikePrice) {FStrikePrice = StrikePrice;};
	void SetCallPut(UnicodeString CallPut) {FCallPut = CallPut;};
	void SetFirm(UnicodeString Firm) {FFirm = Firm;};
	void SetMatchPrice(double MatchPrice) {FMatchPrice = MatchPrice;};
	void SetOrignps(double Orignps) {FOrignps = Orignps;};
	void SetProductKey(UnicodeString ProductKey) {FProductKey = ProductKey;};

	void GetDataFromXML(_di_IXMLNode DataNode);
};  //TGoodWheatTradeDetail

//------------------------------------------------------------------------------
class TGoodWheatOpenInterestDetail
{
private:
	UnicodeString FTradeDate;
	int           FBuyQty;
	int           FSellQty;
	UnicodeString FCommidityNo;
	UnicodeString FYearMonth;
	double        FStrikePrice;
	UnicodeString FCallPut;  //Call, Put, None(Future)
	UnicodeString FFirm;
	double        FMatchPrice;
	double        FPrtlos;
	double        FAveragePrice;

	UnicodeString FProductKey;
public:
	TGoodWheatOpenInterestDetail();
	TGoodWheatOpenInterestDetail(_di_IXMLNode DataNode);

	UnicodeString GetTradeDate() {return FTradeDate;};
	int           GetBuyQty() {return FBuyQty;};
	int           GetSellQty() {return FSellQty;};
	UnicodeString GetCommidityNo() {return FCommidityNo;};
	UnicodeString GetYearMonth() {return FYearMonth;};
	double        GetStrikePrice() {return FStrikePrice;};
	UnicodeString GetCallPut() {return FCallPut;};
	UnicodeString GetFirm() {return FFirm;};
	double        GetMatchPrice() {return FMatchPrice;};
	double        GetPrtlos() {return FPrtlos;};
	double        GetAveragePrice() {return FAveragePrice;};
	UnicodeString GetProductKey() {return FProductKey;};

	void SetTradeDate(UnicodeString TradeDat) {FTradeDate = TradeDat;};
	void SetBuyQty(int BuyQty) {FBuyQty = BuyQty;};
	void SetSellQty(int SellQty) {FSellQty = SellQty;};
	void SetCommidityNo(UnicodeString CommidityNo) {FCommidityNo = CommidityNo;};
	void SetYearMonth(UnicodeString YearMonth) {FYearMonth = YearMonth;};
	void SetStrikePrice(double StrikePrice) {FStrikePrice = StrikePrice;};
	void SetCallPut(UnicodeString CallPut) {FCallPut = CallPut;};
	void SetFirm(UnicodeString Firm) {FFirm = Firm;};
	void SetMatchPrice(double MatchPrice) {FMatchPrice = MatchPrice;};
	void SetPrtlos(double Prtlos) {FPrtlos = Prtlos;};
	void SetAveragePrice(double AveragePrice) {FAveragePrice = AveragePrice;};
	void SetProductKey(UnicodeString ProductKey) {FProductKey = ProductKey;};

	void GetDataFromXML(_di_IXMLNode DataNode);
};  //TGoodWheatOpenInterestDetail

//------------------------------------------------------------------------------
class TGoodWheatValuePaperDetail
{
private:
	UnicodeString FOccDate;
	UnicodeString FStakeID;
	UnicodeString FStakeName;
	UnicodeString FCurrency;
	double        FStakePrice;
	int           FQty;
	double        FAmt;

public:
	TGoodWheatValuePaperDetail();
	TGoodWheatValuePaperDetail(_di_IXMLNode DataNode);

	UnicodeString GetOccDate() {return FOccDate;};
	UnicodeString GetStakeID() {return FStakeID;};
	UnicodeString GetStakeName() {return FStakeName;};
	UnicodeString GetCurrency() {return FCurrency;};
	double        GetStakePrice() {return FStakePrice;};
	int           GetQty() {return FQty;};
	double        GetAmt() {return FAmt;};

	void SetOccDate(UnicodeString OccDat) {FOccDate = OccDat;};
	void SetStakeID(UnicodeString StakeID) {FStakeID = StakeID;};
	void SetStakeName(UnicodeString StakeName) {FStakeName = StakeName;};
	void SetCurrency(UnicodeString Currency) {FCurrency = Currency;};
	void SetStakePrice(double StakePrice) {FStakePrice = StakePrice;};
	void SetQty(double Qty) {FQty = Qty;};
	void SetAmt(double Amt) {FAmt = Amt;};

	void GetDataFromXML(_di_IXMLNode DataNode);
};  //TGoodWheatValuePaperDetail

//------------------------------------------------------------------------------
class TGoodWheatDailyReportMessage
{
private:
	UnicodeString FMessageArriveTime;
	UnicodeString FUserKey;
	UnicodeString FTime;
	UnicodeString FAccountNo;
	UnicodeString FBrokerNo;
	UnicodeString FStatus;
	UnicodeString FQueryEnvFlag;
	UnicodeString FErrorFlag;

	std::vector<TGoodWheatReportDetail*> FReportArray;
	std::vector<TGoodWheatBankDetail*> FBankArray;
	std::vector<TGoodWheatFillDetail*> FFillArray;
	std::vector<TGoodWheatTradeDetail*> FTradeArray;
	std::vector<TGoodWheatOpenInterestDetail*> FOpenInterestArray;
	std::vector<TGoodWheatValuePaperDetail*> FValuePaperArray;

public:
	TGoodWheatDailyReportMessage();
	TGoodWheatDailyReportMessage(TXMLDocument* DataXML);
	~TGoodWheatDailyReportMessage();
	void ClearData();

	UnicodeString GetMessageArriveTime() {return FMessageArriveTime;};
	UnicodeString GetUserKey() {return FUserKey;};
	UnicodeString GetTime() {return FTime;};
	UnicodeString GetAccountNo() {return FAccountNo;};
	UnicodeString GetBrokerNo() {return FBrokerNo;};
	UnicodeString GetStatus() {return FStatus;};
	UnicodeString GetQueryEnvFlag() {return FQueryEnvFlag;};
	UnicodeString GetErrorFlag() {return FErrorFlag;};

	void SetMessageArriveTime(UnicodeString MessageArriveTime) {FMessageArriveTime = MessageArriveTime;};
	void SetUserKey(UnicodeString UserKey) {FUserKey = UserKey;};
	void SetTime(UnicodeString Time) {FTime = Time;};
	void SetAccountNo(UnicodeString AccountNo) {FAccountNo = AccountNo;};
	void SetBrokerNo(UnicodeString BrokerNo) {FBrokerNo = BrokerNo;};
	void SetStatus(UnicodeString Status) {FStatus = Status;};

	void GetDataFromXML(TXMLDocument* DataXML);
	int GetReportDetailCount() {return FReportArray.size();};
	TGoodWheatReportDetail* GetReportDetailByIndexNo(int IndexNo) {return FReportArray[IndexNo];};
	int GetBankDetailCount() {return FBankArray.size();};
	TGoodWheatBankDetail* GetBankDetailByIndexNo(int IndexNo) {return FBankArray[IndexNo];};
	int GetFillDetailCount() {return FFillArray.size();};
	TGoodWheatFillDetail* GetFillDetailByIndexNo(int IndexNo) {return FFillArray[IndexNo];};
	int GetTradeDetailCount() {return FTradeArray.size();};
	TGoodWheatTradeDetail* GetTradeDetailByIndexNo(int IndexNo) {return FTradeArray[IndexNo];};
	int GetOpenInterestDetailCount() {return FOpenInterestArray.size();};
	TGoodWheatOpenInterestDetail* GetOpenInterestDetailByIndexNo(int IndexNo) {return FOpenInterestArray[IndexNo];};
	int GetValuePaperDetailCount() {return FValuePaperArray.size();};
	TGoodWheatValuePaperDetail* GetValuePaperDetailByIndexNo(int IndexNo) {return FValuePaperArray[IndexNo];};
};  //TGoodWheatDailyReportMessage()

//------------------------------------------------------------------------------
class TGoodWheatDailyBankMessage
{
private:
	UnicodeString FMessageArriveTime;
	UnicodeString FUserKey;
	UnicodeString FTime;
	UnicodeString FAccountNo;
	UnicodeString FBrokerNo;
	UnicodeString FStatus;
	UnicodeString FQueryEnvFlag;
	UnicodeString FErrorFlag;

	std::vector<TGoodWheatBankDetail*> FBankArray;

public:
	TGoodWheatDailyBankMessage();
	TGoodWheatDailyBankMessage(TXMLDocument* DataXML);
	~TGoodWheatDailyBankMessage();
	void ClearData();

	UnicodeString GetMessageArriveTime() {return FMessageArriveTime;};
	UnicodeString GetUserKey() {return FUserKey;};
	UnicodeString GetTime() {return FTime;};
	UnicodeString GetAccountNo() {return FAccountNo;};
	UnicodeString GetBrokerNo() {return FBrokerNo;};
	UnicodeString GetStatus() {return FStatus;};
	UnicodeString GetQueryEnvFlag() {return FQueryEnvFlag;};
	UnicodeString GetErrorFlag() {return FErrorFlag;};

	void SetMessageArriveTime(UnicodeString MessageArriveTime) {FMessageArriveTime = MessageArriveTime;};
	void SetUserKey(UnicodeString UserKey) {FUserKey = UserKey;};
	void SetTime(UnicodeString Time) {FTime = Time;};
	void SetAccountNo(UnicodeString AccountNo) {FAccountNo = AccountNo;};
	void SetBrokerNo(UnicodeString BrokerNo) {FBrokerNo = BrokerNo;};
	void SetStatus(UnicodeString Status) {FStatus = Status;};

	void GetDataFromXML(TXMLDocument* DataXML);

	int GetMessageCount() {return FBankArray.size();};
	TGoodWheatBankDetail* GetMessageByIndexNo(int IndexNo) {return FBankArray[IndexNo];};
};  //TGoodWheatDailyBankMessage

//------------------------------------------------------------------------------
class PACKAGE TGoodWheatHttpResource : public TComponent
{
typedef void __fastcall (__closure *TOnSocketError)(System::TObject* Sender, UnicodeString ErrorMessage);
typedef void __fastcall (__closure *TOnResponseError)(System::TObject* Sender, int ResponseCode, UnicodeString ErrorMessage);
private:
	UnicodeString FUserKey;
	UnicodeString FAccountNo;
	UnicodeString FIdNo;
	UnicodeString FBrokerNo;
	UnicodeString FURLHost;
	UnicodeString FURLQryAccountPathName;
	UnicodeString FURLQryAccount;
	UnicodeString FURLQryOrderPathName;
	UnicodeString FURLQryOrder;
	UnicodeString FURLQryFillPathName;
	UnicodeString FURLQryFill;
	UnicodeString FURLQryDayTradePathName;
	UnicodeString FURLQryDayTrade;
	UnicodeString FURLQryOpenInterestPathName;
	UnicodeString FURLQryOpenInterest;
	UnicodeString FURLQryPositionPathName;
	UnicodeString FURLQryPosition;
	UnicodeString FURLQryMarginPathName;
	UnicodeString FURLQryMargin;
	UnicodeString FURLQryMarginOpenInterestPathName;
	UnicodeString FURLQryMarginOpenInterest;
	UnicodeString FURLQryDailyReportPathName;
	UnicodeString FURLQryDailyReport;
	UnicodeString FURLQryBankPathName;
	UnicodeString FURLQryBank;
	UnicodeString FURLQryHoldOpenInterest;

	TIdHTTP                      *FHttpAdapterPtr;
	TIdSSLIOHandlerSocketOpenSSL *FSSLIOHandlerPtr;

	//Event
	TWorkEvent       FOnWork;
	TWorkBeginEvent  FOnWorkBegin;
	TWorkEndEvent    FOnWorkEnd;
	TOnResponseError FOnResponseError;
	TOnSocketError   FOnSocketError;

protected:
	void __fastcall SetRULHost(UnicodeString URLHost);
	void __fastcall SetURLQryAccountPathName(UnicodeString URLQryAccountPathName);
	void __fastcall SetURLQryOrderPathName(UnicodeString URLQryOrderPathName);
	void __fastcall SetURLQryFillPathName(UnicodeString URLQryFillPathName);
	void __fastcall SetURLQryDayTradePathName(UnicodeString URLQryDayTradePathName);
	void __fastcall SetURLQryOpenInterestPathName(UnicodeString URLQryOpenInterestPathName);
	void __fastcall SetURLQryPositionPathName(UnicodeString URLQryPositionPathName);
	void __fastcall SetURLQryMarginPathName(UnicodeString URLQryMarginPathName);
	void __fastcall SetURLQryMarginOpenInterestPathName(UnicodeString URLQryMarginOpenInterestPathName);
	void __fastcall SetURLQryDailyReportPathName(UnicodeString URLQryDailyReportPathName);
	void __fastcall SetURLQryBankPathName(UnicodeString URLQryBankPathName);
	void __fastcall SetupSSL( const String& URL );
	//Event
	void __fastcall SetOnWork(TWorkEvent OnWork);
	void __fastcall SetOnWorkBegin(TWorkBeginEvent OnWorkBegin);
	void __fastcall SetOnWorkEnd(TWorkEndEvent OnWorkEnd);
public:
	__fastcall TGoodWheatHttpResource(TComponent* Owner);
	__fastcall ~TGoodWheatHttpResource();

	//QueryFunction
	void QueryAccount(TGoodWheatAccountMessage& AccountMsg);
	void QueryDailyOrder(UnicodeString Currency, TGoodWheatDailyOrderMessage& DailyOrderMsg);
	void QueryDailyFill(TGoodWheatDailyFillMessage& DailyFillMsg);
	void QueryDailyDayTrade(TGoodWheatDailyDayTradeMessage& DailyDayTradeMsg);
	void QueryDailyOpenInterest(UnicodeString Currency, TGoodWheatDailyOpenInterestMessage& DailyOpenInterestMsg);
	void QueryDailyPosition(TGoodWheatDailyPositionMessage& DailyPositionMsg);
	void QueryDailyMargin(UnicodeString Currency, TGoodWheatDailyMarginMessage& DailyPositionMsg);
	void QueryDailyReport(UnicodeString Currency, UnicodeString ReportDate, TGoodWheatDailyReportMessage& DailyReportMsg);
	void QueryDailyBank(UnicodeString Currency, UnicodeString StartDate, UnicodeString EndDate, TGoodWheatDailyBankMessage& DailyBankMsg);
__published:
	__property UnicodeString UserKey = {read = FUserKey, write = FUserKey};
	__property UnicodeString AccountNo = {read = FAccountNo, write = FAccountNo};
	__property UnicodeString IdNo = {read = FIdNo, write = FIdNo};
	__property UnicodeString BrokerNo = {read = FBrokerNo, write = FBrokerNo};
	__property UnicodeString URLHost = {read = FURLHost, write = FURLHost};
	__property UnicodeString URLQryAccountPathName = {read = FURLQryAccountPathName, write = SetURLQryAccountPathName};
	__property UnicodeString URLQryOrderPathName = {read = FURLQryOrderPathName, write = SetURLQryOrderPathName};
	__property UnicodeString URLQryFillPathName = {read = FURLQryFillPathName, write = SetURLQryFillPathName};
	__property UnicodeString URLQryDayTradePathName = {read = FURLQryDayTradePathName, write = SetURLQryDayTradePathName};
	__property UnicodeString URLQryOpenInterestPathName = {read = FURLQryOpenInterestPathName, write = SetURLQryOpenInterestPathName};
	__property UnicodeString URLQryPositionPathName = {read = FURLQryPositionPathName, write = SetURLQryPositionPathName};
	__property UnicodeString URLQryMarginPathName = {read = FURLQryMarginPathName, write = SetURLQryMarginPathName};
	__property UnicodeString URLQryMarginOpenInterestPathName = {read = FURLQryMarginOpenInterestPathName, write = SetURLQryMarginOpenInterestPathName};
	__property UnicodeString URLQryDailyReportPathName = {read = FURLQryDailyReportPathName, write = SetURLQryDailyReportPathName};
	__property UnicodeString URLQryBankPathName = {read = FURLQryBankPathName, write = SetURLQryBankPathName};
	__property UnicodeString URLQryHoldOpenInterest = {read = FURLQryHoldOpenInterest, write = FURLQryHoldOpenInterest};

	//Event
	__property TWorkEvent OnWork = {read=FOnWork, write=SetOnWork};
	__property TWorkBeginEvent OnWorkBegin = {read=FOnWorkBegin, write=FOnWorkBegin};
	__property TWorkEndEvent OnWorkEnd = {read=FOnWorkEnd, write=FOnWorkEnd};
	__property TOnSocketError OnSocketError = {read=FOnSocketError, write=FOnSocketError};
	__property TOnResponseError OnResponseError = {read=FOnResponseError, write=FOnResponseError};
};  //TGoodWheatHttpResource
//------------------------------------------------------------------------------
#endif
