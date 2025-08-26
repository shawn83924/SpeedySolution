//---------------------------------------------------------------------------

#ifndef TradingCommoditiesH
#define TradingCommoditiesH
#include <map>
#include <vector>
#include <hash_map>
#include <deque>
#include "System.hpp"
#include "UFC.h"
#include "OrderMessageDefine.h"
#include "TradingUtility.h"
#include "TradingBasicObjects.h"

//---------------------------------------------------------------------------
namespace s888
{
//---------------------------------------------------------------------------
class CTradingExchange : public CExchangeKey
{
public:
	static String ConvertExchangeCodeToBrokerCode(const String& ExchangeCode);
	static String ConvertBrokerExchangeCodeToCode(const String& BrokerExchangeCode);

protected:
	String FExchangeName;
	bool   FIsDayTradeOpenSupported;
	bool   FIsStopOrderSupported;

public:
	//Constructor
	CTradingExchange(const String& ExchangeCode, const String& ExchangeName = L"",
					 bool IsDayTradeOpenSupported = false,
					 bool IsStopOrderSupported = false,
					 const String& BrokerExchangeCode = L"");

	//Get/Set Field Function
	virtual String GetExchangeName(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FExchangeName, RWLockType);};
	virtual void SetExchangeName(const String& ExchangeName, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(ExchangeName, FExchangeName, RWLockType);};

	virtual bool IsDayTradeOpenSupported(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FIsDayTradeOpenSupported, RWLockType);};
	virtual void SetDayTradeOpenSupported(bool IsDayTradeOpenSupported, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(IsDayTradeOpenSupported, FIsDayTradeOpenSupported, RWLockType);};

	virtual bool IsStopOrderSupported(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FIsStopOrderSupported, RWLockType);};
	virtual void SetStopOrderSupported(bool IsStopOrderSupported, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(IsStopOrderSupported, FIsStopOrderSupported, RWLockType);};

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CTradingExchange

//---------------------------------------------------------------------------
class CTaxRateObject : public CExchangeContractKey
{
protected:
	double FTaxRate;

public:
	//Constructor
	CTaxRateObject(const String& ExchangeCode, const String& ContractCode,
				   double TaxRate = 0.0, const String& BrokerContractCode = L"",
				   nsOrderMessageDefine::MarketEnum Market = nsOrderMessageDefine::mOTC);

	//Get/Set Field Function
	virtual double GetTaxRate(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTaxRate, RWLockType);};
	virtual void SetTaxRate(double TaxRate, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(TaxRate, FTaxRate, RWLockType);};

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CTaxRateObject

//---------------------------------------------------------------------------
class CFeeObject : public CExchangeContractKey
{
protected:
	String FAccount;
	double FOpenRateByMoney;
	double FOpenRateByVolume;
	double FCloseRateByMoney;
	double FCloseRateByVolume;
	double FCloseTodayRateByMoney;
	double FCloseTodayRateByVolume;

public:
	//Constructor
	CFeeObject(const String& Account, const String& ExchangeCode, const String& ContractCode,
			   nsOrderMessageDefine::MarketEnum Market = nsOrderMessageDefine::mOTC);
	CFeeObject(const String& Account, const String& ExchangeCode, const String& ContractCode,
			   double OpenRateByMoney,       double OpenRateByVolume,
			   double CloseRateByMoney,      double CloseRateByVolume,
			   double CloseTodayRateByMoney, double CloseTodayRateByVolume,
			   nsOrderMessageDefine::MarketEnum Market = nsOrderMessageDefine::mOTC);

	//Get/Set Field Function
	virtual String GetAccount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FAccount, RWLockType);};
	virtual void SetAccount(const String& Account, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(Account, FAccount, RWLockType);};

	virtual double GetOpenRateByMoney(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOpenRateByMoney, RWLockType);};
	virtual void SetOpenRateByMoney(double OpenRateByMoney, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(OpenRateByMoney, FOpenRateByMoney, RWLockType);};

	virtual double GetOpenRateByVolume(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOpenRateByVolume, RWLockType);};
	virtual void SetOpenRateByVolume(double OpenRateByVolume, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(OpenRateByVolume, FOpenRateByVolume, RWLockType);};

	virtual double GetCloseRateByMoney(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCloseRateByMoney, RWLockType);};
	virtual void SetCloseRateByMoney(double CloseRateByMoney, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CloseRateByMoney, FCloseRateByMoney, RWLockType);};

	virtual double GetCloseRateByVolume(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCloseRateByVolume, RWLockType);};
	virtual void SetCloseRateByVolume(double CloseRateByVolume, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CloseRateByVolume, FCloseRateByVolume, RWLockType);};

	virtual double GetCloseTodayRateByMoney(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCloseTodayRateByMoney, RWLockType);};
	virtual void SetCloseTodayRateByMoney(double CloseTodayRateByMoney, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CloseTodayRateByMoney, FCloseTodayRateByMoney, RWLockType);};

	virtual double GetCloseTodayRateByVolume(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCloseTodayRateByVolume, RWLockType);};
	virtual void SetCloseTodayRateByVolume(double CloseTodayRateByVolume, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CloseTodayRateByVolume, FCloseTodayRateByVolume, RWLockType);};

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CFeeObject

//---------------------------------------------------------------------------
class CCommodityCancelStatistic : public CExchangeContractKey
{
protected:
	int FCancelReportCount;
	int FCancelQuantity;

public:
	//Constructor
	CCommodityCancelStatistic(const String& ExchangeCode, const String& ContractCode, const String& BrokerContractCode = L"");

	//Get/Set Field Function
	virtual int GetCancelReportCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCancelReportCount, RWLockType);};
	virtual void SetCancelReportCount(int CancelReportCount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CancelReportCount, FCancelReportCount, RWLockType);};

	virtual int GetCancelQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCancelQuantity, RWLockType);};
	virtual void SetCancelQuantity(int CancelQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CancelQuantity, FCancelQuantity, RWLockType);};

	int IncreaseCancelReportCount(int CancelReportCount, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int IncreaseCancelQuantity(int CancelQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CCommodityCancelStatistic

//---------------------------------------------------------------------------
class CContractCancelStatistic : public CExchangeSymbolKey
{
protected:
	int FCancelReportCount;
	int FCancelQuantity;

public:
	//Constructor
	CContractCancelStatistic(const String& ExchangeCode, const String& Symbol);

	//Get/Set Field Function
	virtual int GetCancelReportCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCancelReportCount, RWLockType);};
	virtual void SetCancelReportCount(int CancelReportCount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CancelReportCount, FCancelReportCount, RWLockType);};

	virtual int GetCancelQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCancelQuantity, RWLockType);};
	virtual void SetCancelQuantity(int CancelQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CancelQuantity, FCancelQuantity, RWLockType);};

	int IncreaseCancelReportCount(int CancelReportCount, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int IncreaseCancelQuantity(int CancelQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CContractCancelStatistic

//---------------------------------------------------------------------------
class CTradingContract;
//---------------------------------------------------------------------------
class CTradingCommodity : public CExchangeContractKey
{
protected:
	String                  FCommodityName;
	bool                    FIsDayTradeOpenSupported;
	bool                    FIsStopOrderSupported;
	double                  FTaxRate;
	String                  FCurrency;
	double                  FContractMultiplier;
	String                  FContractKind;  //契約種類：I-指數、R-利率、B-貨券、C-商品、S-股票
	CTradingExchange        *FExchangePtr;
	CWStrTradingObjPtrHMap  FContractSymbolIndex;
public:
	//Constructor
	CTradingCommodity(const String& ExchangeCode, const String& ContractCode,
					  nsOrderMessageDefine::MarketEnum Market,
					  bool IsDayTradeOpenSupported = false,
					  bool IsStopOrderSupported = false,
					  double TaxRate = 0.0,
					  CTradingExchange *ExchangePtr = 0);
	~CTradingCommodity();

	//Get/Set Field Function
	virtual String GetCommodityName(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCommodityName, RWLockType);};
	virtual void SetCommodityName(const String& CommodityName, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CommodityName, FCommodityName, RWLockType);};

	virtual bool IsDayTradeOpenSupported(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FIsDayTradeOpenSupported, RWLockType);};
	virtual void SetDayTradeOpenSupported(bool IsDayTradeOpenSupported, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(IsDayTradeOpenSupported, FIsDayTradeOpenSupported, RWLockType);};

	virtual bool IsStopOrderSupported(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FIsStopOrderSupported, RWLockType);};
	virtual void SetStopOrderSupported(bool IsStopOrderSupported, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(IsStopOrderSupported, FIsStopOrderSupported, RWLockType);};

	virtual double GetTaxRate(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTaxRate, RWLockType);};
	virtual void SetTaxRate(double TaxRate, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(TaxRate, FTaxRate, RWLockType);};

	virtual String GetCurrency(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCurrency, RWLockType);};
	virtual void SetCurrency(const String& Currency, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(Currency, FCurrency, RWLockType);};

	virtual String GetContractKind(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FContractKind, RWLockType);};
	virtual void SetContractKind(const String& ContractKind, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(ContractKind, FContractKind, RWLockType);};

	virtual double GetContractMultiplier(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FContractMultiplier, RWLockType);};
	virtual void SetContractMultiplier(double ContractMultiplier, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(ContractMultiplier, FContractMultiplier, RWLockType);};

	virtual CTradingExchange *GetExchange(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FExchangePtr, RWLockType);};
	virtual void SetExchange(CTradingExchange *ExchangePtr, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(ExchangePtr, FExchangePtr, RWLockType);};

	int GetContractSymbolIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FContractSymbolIndex, FCSLockPtr, RWLockType);};;
	CTradingContract *FindContractBySymbol(const String& Symbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertContractIntoSymbolIndex(const String& Symbol, CTradingContract *ContractPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTradingContract *RemoveContractFromSymbolIndex(const String& Symbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearContractSymbolIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTradingContract *GetFirstContract(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CTradingCommodity

//---------------------------------------------------------------------------
class CTradingContract : public CExchangeSymbolKey
{
public:
	static void ParseMaturityYearMonth(const String& MaturityYearMonthStr,
									   int& MaturityYear, int& MaturityMonth);
	static wchar_t GenerateYearCode(int MaturityYear);
	static wchar_t GenerateTaifexMonthCode(int MaturityMonth,
										   nsOrderMessageDefine::EventTypeEnum CallOrPut = nsOrderMessageDefine::evtNone);
	static wchar_t GenerateRfaMonthCode(int MaturityMonth,
										nsOrderMessageDefine::EventTypeEnum CallOrPut = nsOrderMessageDefine::evtNone);
	static String GenerateTaifexSymbol(const String& ContractCode, double StrikePrice, const String& MaturityYearMonthStr,
									   nsOrderMessageDefine::EventTypeEnum CallOrPut = nsOrderMessageDefine::evtNone);
	static String GenerateRfaSymbol(const String& ContractCode, double StrikePrice, const String& MaturityYearMonthStr,
									nsOrderMessageDefine::EventTypeEnum CallOrPut = nsOrderMessageDefine::evtNone);
	static String GenerateOverseasTradeSymbol(const String& TradeContractCode, const String& StrikePriceStr, const String& MaturityYearMonthStr,
											  nsOrderMessageDefine::EventTypeEnum CallOrPut = nsOrderMessageDefine::evtNone);
	static bool ParseTaifexFutureComposedSymbol(const String& Symbol, String& Leg1Symbol, String& Leg2Symbol);
	static bool ParseTaifexOptionComposedSymbol(const String& Symbol, String& Leg1Symbol, String& Leg2Symbol);
	static bool ParseTaifexComposedSymbol(const String& Symbol, String& Leg1Symbol, String& Leg2Symbol);
	static void ParseOverseasTradeSymbol(const String& TradeSymbol, String& TradeContractCode, String& StrikePriceStr, String& MaturityYearMonthStr,
										 nsOrderMessageDefine::EventTypeEnum& CallOrPut);

protected:
	double                              FStrikePrice;           //履約價
	nsOrderMessageDefine::EventTypeEnum FCallPut;               //買賣權
	String                              FMaturityYearMonthStr;  //到期年月
	String                              FContractName;          //合約名稱
	int                                 FFractionPartLength;    //價格小數部分長度
	double                              FContractMultiplier;    //合約乘數
	bool                                FIsComposed;
	String                              FLeg1Symbol;
	String                              FLeg2Symbol;
	CTradingCommodity                   *FCommodityPtr;
	CTradingExchange                    *FExchangePtr;

public:
	//Constructor
	CTradingContract(const String& ExchangeCode, const String& Symbol,
					 nsOrderMessageDefine::MarketEnum Market,
					 double StrikePrice = 0.0,
					 nsOrderMessageDefine::EventTypeEnum CallPut = nsOrderMessageDefine::evtNone,
					 CTradingCommodity *CommodityPtr = 0,
					 CTradingExchange *ExchangePtr = 0);

	//Get/Set Field Function
	virtual double GetStrikePrice(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FStrikePrice, RWLockType);};
	virtual void SetStrikePrice(double StrikePrice, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(StrikePrice, FStrikePrice, RWLockType);};

	virtual nsOrderMessageDefine::EventTypeEnum GetCallPut(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCallPut, RWLockType);};
	virtual void SetCallPut(nsOrderMessageDefine::EventTypeEnum CallPut, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CallPut, FCallPut, RWLockType);};

	virtual String GetMaturityYearMonthStr(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FMaturityYearMonthStr, RWLockType);};
	virtual void SetMaturityYearMonthStr(const String& MaturityYearMonthStr, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(MaturityYearMonthStr, FMaturityYearMonthStr, RWLockType);};

	virtual String GetContractName(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FContractName, RWLockType);};
	virtual void SetContractName(const String& ContractName, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(ContractName, FContractName, RWLockType);};

	virtual int GetFractionPartLength(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FFractionPartLength, RWLockType);};
	virtual void SetFractionPartLength(int FractionPartLength, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(FractionPartLength, FFractionPartLength, RWLockType);};

	virtual double GetContractMultiplier(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FContractMultiplier, RWLockType);};
	virtual void SetContractMultiplier(double ContractMultiplier, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(ContractMultiplier, FContractMultiplier, RWLockType);};

	virtual bool IsComposed(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FIsComposed, RWLockType);};
	virtual void SetComposed(bool IsComposed, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(IsComposed, FIsComposed, RWLockType);};

	virtual String GetLeg1Symbol(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FLeg1Symbol, RWLockType);};
	virtual void SetLeg1Symbol(const String& Leg1Symbol, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(Leg1Symbol, FLeg1Symbol, RWLockType);};

	virtual String GetLeg2Symbol(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FLeg2Symbol, RWLockType);};
	virtual void SetLeg2Symbol(const String& Leg2Symbol, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(Leg2Symbol, FLeg2Symbol, RWLockType);};

	virtual CTradingCommodity *GetCommodity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCommodityPtr, RWLockType);};
	virtual void SetCommodity(CTradingCommodity *CommodityPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CommodityPtr, FCommodityPtr, RWLockType);};

	virtual CTradingExchange *GetExchange(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FExchangePtr, RWLockType);};
	virtual void SetExchange(CTradingExchange *ExchangePtr, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(ExchangePtr, FExchangePtr, RWLockType);};

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CTradingContract

//---------------------------------------------------------------------------
class CHoldPositionData : public CExchangeSymbolKey
{  //前日留倉資料
protected:
	nsOrderMessageDefine::SideEnum FSide;            //買賣別
	int                            FHoldQuantity;    //持倉數量
	double                         FMatchPrice;      //成交價格
	String                         FOrderID;         //委託單號
	String                         FTradeDate;       //交易日期
	bool                           FIsDayTradeOver;  //當日平倉
	CTradingCommodity              *FCommodityPtr;
	CTradingExchange               *FExchangePtr;

public:
	//Constructor
	CHoldPositionData(const String& ExchangeCode, const String& Symbol,
					  nsOrderMessageDefine::SideEnum Side,
					  int HoldQuantity, double MatchPrice,
					  CTradingCommodity *CommodityPtr = 0,
					  CTradingExchange *ExchangePtr = 0);
	CHoldPositionData(const String& ExchangeCode, const String& Symbol,
					  nsOrderMessageDefine::MarketEnum Market,
					  nsOrderMessageDefine::SideEnum Side,
					  int HoldQuantity, double MatchPrice,
					  CTradingCommodity *CommodityPtr = 0,
					  CTradingExchange *ExchangePtr = 0);

	//Get/Set Field Function
	virtual nsOrderMessageDefine::SideEnum GetSide(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSide, RWLockType);};
	virtual void SetSide(nsOrderMessageDefine::SideEnum Side, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(Side, FSide, RWLockType);};

	virtual int GetHoldQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FHoldQuantity, RWLockType);};
	virtual void SetHoldQuantity(int HoldQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(HoldQuantity, FHoldQuantity, RWLockType);};

	virtual double GetMatchPrice(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FMatchPrice, RWLockType);};
	virtual void SetMatchPrice(double MatchPrice, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(MatchPrice, FMatchPrice, RWLockType);};

	virtual String GetOrderID(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOrderID, RWLockType);};
	virtual void SetOrderID(const String& OrderID, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(OrderID, FOrderID, RWLockType);};

	virtual String GetTradeDate(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTradeDate, RWLockType);};
	virtual void SetTradeDate(const String& TradeDate, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(TradeDate, FTradeDate, RWLockType);};

	virtual bool IsDayTradeOver(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FIsDayTradeOver, RWLockType);};
	virtual void SetDayTradeOver(bool IsDayTradeOver, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(IsDayTradeOver, FIsDayTradeOver, RWLockType);};

	virtual CTradingCommodity *GetCommodity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCommodityPtr, RWLockType);};
	virtual void SetCommodity(CTradingCommodity *CommodityPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CommodityPtr, FCommodityPtr, RWLockType);};

	virtual CTradingExchange *GetExchange(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FExchangePtr, RWLockType);};
	virtual void SetExchange(CTradingExchange *ExchangePtr, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(ExchangePtr, FExchangePtr, RWLockType);};

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CHoldPositionData

};  //namespace s888
#endif
