//---------------------------------------------------------------------------

#ifndef TExecutionUnitH
#define TExecutionUnitH
#include <map>
#include <vcl.h>
#include "UFC.h"
#include "TExecutionReportMessage.h"
#include "ExchangeSymbolUnit.h"
#include "MarketDataStore.h"
#include "TTaifexConnection.h"
#include "OrderMessageDefine.h"
#include "stl_config.h"
//--------------------------------------------------------------------------
class TExecution;
//--------------------------------------------------------------------------
typedef std::map<UnicodeString, TExecution*> CStringTExecutionMap;
typedef std::multimap<UnicodeString, TExecution*> CStringTExecutionMultiMap;
//---------------------------------------------------------------------------
const char FUTURE_NORMAL         = 'N';
const char FUTURE_DAYTRADE       = 'Y';
const char STOCK_NORMAL          = '0';
const char STOCK_DAYTRADE_NORMAL = 'A';
const char STOCK_MARGIN_TRADE    = '3';
const char STOCK_SELL_SHORT      = '4';
const char STOCK_DAYTRADE_LOAN   = '9'; ///< daytrade sell short/margin trade
//--------------------------------------------------------------------------
nsOrderMessageDefine::SideEnum           TranslateSide(const UFC::AnsiString& SideStr);
nsOrderMessageDefine::PositionEffectEnum TranslatePositionEffect(const UFC::AnsiString& PositionEffectStr);
nsOrderMessageDefine::OrderTypeEnum      TranslateOrderType(const UFC::AnsiString& OrderTypeStr);
nsOrderMessageDefine::TimeInForceEnum    TranslateTimeInForce(const UFC::AnsiString& TimeInForceStr);

UnicodeString TranslateOrderStatus(nsOrderMessageDefine::OrderStatusEnum OrderStatus);
UnicodeString TranslateSide(nsOrderMessageDefine::SideEnum Side);
UnicodeString TranslatePositionEffect(nsOrderMessageDefine::PositionEffectEnum PositionEffect);
UnicodeString TranslateOrderType(nsOrderMessageDefine::OrderTypeEnum OrderType);
UnicodeString TranslateTimeInForce(nsOrderMessageDefine::TimeInForceEnum TimeInForce);
//--------------------------------------------------------------------------
class TExecution : public TObject
{
private:
	static Char FOrderLogDelimiter;

public:
	static Char GetOrderLogDelimiter() {return FOrderLogDelimiter;};
	static void SetOrderLogDelimiter(Char OrderLogDelimiter) {FOrderLogDelimiter = OrderLogDelimiter;};

private:
	nsOrderMessageDefine::ExecTypeEnum         FExecType;
	nsOrderMessageDefine::TMPExecTypeEnum      FTMPExecType;
	nsOrderMessageDefine::CxlRejResponseToEnum FCxlRejResponseTo;
	nsOrderMessageDefine::MarketEnum           FMarket;
	nsOrderMessageDefine::PositionEffectEnum   FPositionEffect;
	nsOrderMessageDefine::OrderTypeEnum        FOrderType;
	nsOrderMessageDefine::TimeInForceEnum      FTimeInForce;
	nsOrderMessageDefine::OrderStatusEnum      FOrderStatus;
	nsOrderMessageDefine::SideEnum             FSide;
	nsOrderMessageDefine::SideEnum             FLegSide1;
	nsOrderMessageDefine::SideEnum             FLegSide2;
	TExchangeSymbol*                           FSymbol;
	UFCType::Int64                             FRootNID;
	UFCType::Int64                             FNID;
	double                                     FPrice;
	double                                     FLastPx;
	double                                     FLegPx1;
	double                                     FLegPx2;
	double                                     FAvgPx;
	double                                     FStopPrice;
	double                                     FStopMarketPrice;
	bool                                       FIsRoot;
	bool                                       FIsRootCreatedByReport;
	bool                                       FIsExpanded;
	bool                                       FIsLastMessage;
	int                                        FStatusCode;
	int                                        FOrderQty;
	int                                        FCumQty;
	int                                        FLeavesQty;
	int                                        FLastQty;
	int                                        FLegQty1;
	int                                        FLegQty2;
	int                                        FPxDigit;
	int                                        FTickCount;
	int                                        FHHMM;
	char                                       FOrdKind;
	UnicodeString                              FTransactTime;      //hh24:mm:ss.fff 12
	UnicodeString                              FLocalMessageTime;  //hh24:mm:ss.fff 12
	UnicodeString                              FText;
	UnicodeString                              FExchangeCode;
	UnicodeString                              FOrderID;
	UnicodeString                              FDisplayName;
	UnicodeString                              FExecID;
	UnicodeString                              FTimeOrderIDKey;
	UnicodeString                              FStopExchangeCode;
	UnicodeString                              FStopSymbol;
	UnicodeString                              FStrategyName;
	UnicodeString                              FAccount;
	UnicodeString                              FTradeSymbol;
	CStringTExecutionMultiMap                  FExecutions;
private:
	int  GetDecimalPartLength(const String& ExchangeCode, const String& SymbolCode, double RealValue);
	void GenerateOrderLogStr( String& orderLogStr );
	void SetFieldValue(UFC::NameValueMessage& FieldData);
	void ParseFieldNameValueString(const UFC::AnsiString& FieldNameValueStr);
	void ParseOrderLogStr(const String& OrderLogStr);
   	void GetFromUserData(  String& StrategyName, int& hhmm, char& OrdKind, TExecutionReportMessage* Msg );
public:
	static TCMarketDataStore *FMarketDataStorePtr;
	static UnicodeString GetExecTypeDescription(nsOrderMessageDefine::ExecTypeEnum TargetExecType, nsOrderMessageDefine::TMPExecTypeEnum TMPExecType, int LanguageID = 1);
	static UnicodeString GetMarketDescription(nsOrderMessageDefine::MarketEnum TargetMarket, int LanguageID = 1);
	static UnicodeString GetPositionEffectDescription(nsOrderMessageDefine::PositionEffectEnum TargetPositionEffect, int LanguageID = 1);
	static UnicodeString GetOrderTypeDescription(nsOrderMessageDefine::OrderTypeEnum TargetOrderType, int LanguageID = 1);
	static UnicodeString GetTimeInForceDescription(nsOrderMessageDefine::TimeInForceEnum TargetTimeInForce);
	static UnicodeString GetOrderStatusDescription(nsOrderMessageDefine::OrderStatusEnum TargetOrderStatus, nsOrderMessageDefine::TMPExecTypeEnum TMPExecType, int LanguageID = 1);
	static UnicodeString GetSideDescription(nsOrderMessageDefine::SideEnum TargetSide, int LanguageID = 1);
public:
	TExecution();
	TExecution( TExecutionReportMessage* Msg );
	TExecution( TExecution* ExecPtr );
	TExecution( const String& OrderLogStr );
	TExecution( TNewOrderMessage& NewMsg );
public:
	void UpdateDisplayName( void ); ///< Update display name from BasicInfo
	int  WriteToOrderLogStream(TFileStream *OrderLogStmPtr);
	bool InsertOrderMessage(TExecution* OrderMessagePtr);
	int  GetMessageCount() {return FExecutions.size();};
	CStringTExecutionMultiMap* GetExecutions() {return &FExecutions;};
public: ///< Get properties
	const UnicodeString&                       GetTransactTime() {return FTransactTime;};
	const UnicodeString&                       GetLocalMessageTime() {return FLocalMessageTime;};
	const UnicodeString&                       GetText() {return FText;};
	const UnicodeString&                       GetExchangeCode() {return FExchangeCode;};
	const UnicodeString&                       GetOrderID() {return FOrderID;};
	const UnicodeString&                       GetDisplayName() {return FDisplayName;};
	const UnicodeString&                       GetExecID() {return FExecID;};
	const UnicodeString&                       GetTimeOrderIDKey() {return FTimeOrderIDKey;};
	const UnicodeString&                       GetStopExchangeCode() {return FStopExchangeCode;};
	const UnicodeString&                       GetStopSymbol() {return FStopSymbol;};
	const UnicodeString&                       GetStrategyName() {return FStrategyName;};
	const UnicodeString&                       GetAccount() {return FAccount;};
	const UnicodeString&                       GetTradeSymbol() {return FTradeSymbol;};
	nsOrderMessageDefine::ExecTypeEnum         GetExecType() {return FExecType;};
	nsOrderMessageDefine::TMPExecTypeEnum      GetTMPExecType() {return FTMPExecType;};
	nsOrderMessageDefine::CxlRejResponseToEnum GetCxlRejResponseTo() {return FCxlRejResponseTo;};
	nsOrderMessageDefine::MarketEnum           GetMarket() {return FMarket;};
	nsOrderMessageDefine::PositionEffectEnum   GetPositionEffect() {return FPositionEffect;};
	nsOrderMessageDefine::OrderTypeEnum        GetOrderType() {return FOrderType;};
	nsOrderMessageDefine::TimeInForceEnum      GetTimeInForce() {return FTimeInForce;};
	nsOrderMessageDefine::OrderStatusEnum      GetOrderStatus() {return FOrderStatus;};
	nsOrderMessageDefine::SideEnum             GetSide() {return FSide;};
	nsOrderMessageDefine::SideEnum             GetLegSide1() {return FLegSide1;};
	nsOrderMessageDefine::SideEnum             GetLegSide2() {return FLegSide2;};
	TExchangeSymbol*                           GetSymbol() {return FSymbol;};
	UFCType::Int64                             GetNID() {return FNID;};
	UFCType::Int64                             GetRootNID() {return FRootNID;};
	int                                        GetStatusCode() {return FStatusCode;};
	int                                        GetOrderQty() {return FOrderQty;};
	int                                        GetCumQty() {return FCumQty;};
	int                                        GetLeavesQty() {return FLeavesQty;};
	int                                        GetPxDigit() {return FPxDigit;};
	int                                        GetLastQty() {return FLastQty;};
	int                                        GetLegQty1() {return FLegQty1;};
	int                                        GetLegQty2() {return FLegQty2;};
	int                                        GetTickCount() {return FTickCount;};
	int                                        GetHHMM() { return FHHMM; };
	double                                     GetPrice() {return FPrice;};
	double                                     GetLastPx() {return FLastPx;};
	double                                     GetLegPx1() {return FLegPx1;};
	double                                     GetLegPx2() {return FLegPx2;};
	double                                     GetAvgPx() {return FAvgPx;};
	double                                     GetStopPrice() {return FStopPrice;};
	double                                     GetStopMarketPrice() {return FStopMarketPrice;};
	char                                       GetOrderKind() { return FOrdKind; }
	bool                                       IsRoot() {return FIsRoot;};
	bool                                       IsRootCreatedByReport() {return FIsRootCreatedByReport;};
	bool                                       IsExpanded() {return FIsExpanded;};
	bool                                       IsLastMessage() {return FIsLastMessage;};
public: ///< Set properties
	void SetTransactTime( const UnicodeString& TransactTime) {FTransactTime = TransactTime;};
	void SetLocalMessageTime( const UnicodeString& LocalMessageTime) {FLocalMessageTime = LocalMessageTime;};
	void SetText( const UnicodeString& Text) {FText = Text;};
	void SetExchangeCode( const UnicodeString& ExchangeCode) {FExchangeCode = ExchangeCode;};
	void SetOrderID(UnicodeString OrderID) {FOrderID = OrderID;};
	void SetSymbol(const String& SymbolCode);
	void SetDisplayName( const UnicodeString& DisplayName);
	void SetExecID( const UnicodeString& ExecID) {FExecID = ExecID;};
	void SetTimeOrderIDKey( const UnicodeString& TimeOrderIDKey) {FTimeOrderIDKey = TimeOrderIDKey;};
	void SetStopExchangeCode( const UnicodeString& StopExchangeCode) {FStopExchangeCode = StopExchangeCode;};
	void SetStopSymbol( const UnicodeString& StopSymbol) {FStopSymbol = StopSymbol;};
	void SetStrategyName(const UnicodeString& StrategyName) {FStrategyName = StrategyName;};
	void SetAccount(const UnicodeString& Account) {FAccount = Account;};
	void SetTradeSymbol(const String& TradeSymbol) {FTradeSymbol = TradeSymbol;};
	void SetExecType(nsOrderMessageDefine::ExecTypeEnum ExecType) {FExecType = ExecType;};
	void SetTMPExecType(nsOrderMessageDefine::TMPExecTypeEnum TMPExecType) {FTMPExecType = TMPExecType;};
	void SetCxlRejResponseTo(nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo) {FCxlRejResponseTo = CxlRejResponseTo;};
	void SetStatusCode(int StatusCode) {FStatusCode = StatusCode;};
	void SetMarket(nsOrderMessageDefine::MarketEnum Market) {FMarket = Market;};
	void SetNID(UFCType::Int64 NID) {FNID = NID;};
	void SetPositionEffect(nsOrderMessageDefine::PositionEffectEnum PositionEffect) {FPositionEffect = PositionEffect;};
	void SetOrderType(nsOrderMessageDefine::OrderTypeEnum OrderType) {FOrderType = OrderType;};
	void SetTimeInForce(nsOrderMessageDefine::TimeInForceEnum TimeInForce) {FTimeInForce = TimeInForce;};
	void SetOrderStatus(nsOrderMessageDefine::OrderStatusEnum OrderStatus) {FOrderStatus = OrderStatus;};
	void SetOrderQty(int OrderQty) {FOrderQty = OrderQty;};
	void SetCumQty(int CumQty) {FCumQty = CumQty;};
	void SetLeavesQty(int LeavesQty) {FLeavesQty = LeavesQty;};
	void SetPrice(double Price) {FPrice = Price;};
	void SetSymbol(TExchangeSymbol* Symbol) {FSymbol = Symbol;};
	void SetSide(nsOrderMessageDefine::SideEnum Side) {FSide = Side;};
	void SetLegSide1(nsOrderMessageDefine::SideEnum LegSide1) {FLegSide1 = LegSide1;};
	void SetLegSide2(nsOrderMessageDefine::SideEnum LegSide2) {FLegSide2 = LegSide2;};
	void SetLastQty(int LastQty) {FLastQty = LastQty;};
	void SetLegQty1(int LegQty1) {FLegQty1 = LegQty1;};
	void SetLegQty2(int LegQty2) {FLegQty2 = LegQty2;};
	void SetLastPx(double LastPx) {FLastPx = LastPx;};
	void SetLegPx1(double LegPx1) {FLegPx1 = LegPx1;};
	void SetLegPx2(double LegPx2) {FLegPx2 = LegPx2;};
	void SetPxDigit(int PxDigit) {FPxDigit = PxDigit;};
	void SetAvgPx(double AvgPx) {FAvgPx = AvgPx;};
	void SetRoot(bool IsRoot);
	void SetRootCreatedByReport(bool IsRootCreatedByReport) {FIsRootCreatedByReport = IsRootCreatedByReport;};
	void SetRootNID( UFCType::Int64 RootNID) {FRootNID = RootNID;};
	void SetExpanded(bool IsExpanded);
	void SetLastMessage(bool IsLastMessage);
	void SetStopPrice(double StopPrice) {FStopPrice = StopPrice;};
	void SetStopMarketPrice(double MarketPrice) {FStopMarketPrice = MarketPrice;};
	void SetTickCount(int TickCount) {FTickCount = TickCount;};
	void SetHHMM( int hhmm ) { FHHMM = hhmm; };
    void SetOrderKind( char OrdKind ) { FOrdKind = OrdKind; }
public:
	UnicodeString GetStringField(int Index, int LanguageID = 1);
	UnicodeString EncodeNameValueString(TExecutionReportMessage *Msg = 0);
	bool GetStrategyName( UFC::AnsiString& StrategyName, TExecutionReportMessage* Msg );
private:
	UnicodeString EncodeStatusNameValueString(TExecutionReportMessage *Msg = 0);
	UnicodeString EncodeCancelNameValueString(TExecutionReportMessage *Msg = 0);
	UnicodeString EncodeFillNameValueString(TExecutionReportMessage *Msg = 0);
	UnicodeString EncodeRejectNameValueString(TExecutionReportMessage *Msg = 0);
	String __fastcall UTF8String( const String& CStr );
	String __fastcall UTF8String( const char* CStr );
};  //TExecution
//---------------------------------------------------------------------------
#endif
