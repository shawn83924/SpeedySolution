//---------------------------------------------------------------------------
#ifndef SunGardObjectsH
#define SunGardObjectsH
#include <vector>
#include <map>
#include <vcl.h>
#include "UFC.h"

//---------------------------------------------------------------------------
typedef enum
{
	sgQueryClientFund            = 6012,
	sgQueryFillDetail            = 6013,
	sgQueryClientPosition        = 6014,
	sgQueryOrderRecord           = 6019,
	sgChangePassword             = 6023,
	sgQueryClientOpeningPosition = 6035,
	sgTransformMoney             = 6041,
	sgQueryUserBankAccount       = 6042,
	sgQueryUserTransformRecord   = 6043,
	sgQueryUserBankAvailableCash = 6052,
	sgQueryUserAvailableFund     = 6055,
	sgConfirmSettlement          = 6071,
	sgQuerySettlement            = 6072,
	sgAnotherLogonOn             = 6501,
	sgQueryClientHoldPosition    = 6502,
	sgNoneFunction               = 10000
} SunGardRequestFunctionNoEnum;
SunGardRequestFunctionNoEnum ParseSGRequestFunctionNo(const UnicodeString& SGRequestFunctionNoStr);

//---------------------------------------------------------------------------
typedef enum
{
	sgsdBuy  =  0,
	sgsdSell =  1,
	sgsdNone =  9
} SunGardSideEnum;
SunGardSideEnum ParseSGSide(const UnicodeString& SGSideStr);
UnicodeString DescribeSGSide(SunGardSideEnum SideValue);

//--------------------------------------------------------------------------
typedef enum
{
	sgpeOpen       =  0,
	sgpeClose      =  1,
	sgpeCloseToday =  2,
	sgpeNone       =  9,
} SunGardPositionEffectEnum;
SunGardPositionEffectEnum ParseSGPositionEffect(const UnicodeString& SGPositionEffectStr);
UnicodeString DescribeSGPositionEffect(SunGardPositionEffectEnum PositionEffectValue);

//--------------------------------------------------------------------------
typedef enum
{
	sgstSpeculate =  0,
	sgstHedge     =  1,
	sgstNone      =  9,
} SunGardStrategyEnum;
SunGardStrategyEnum ParseSGStrategy(const UnicodeString& SGStrategyStr);
UnicodeString DescribeSGStrategy(SunGardStrategyEnum StrategyValue);

//--------------------------------------------------------------------------
typedef enum
{
	sgosWaiting                    = L'n',
	sgosBaodaning                  = L's',
	sgosAlreadyBaodaned            = L'a',
	sgosPartialCompleted           = L'p',
	sgosCompleted                  = L'c',
	sgosDeleteing                  = L'f',
	sgosErrorOrder                 = L'e',
	sgosSystemDisable              = L'q',
	sgosTotalDeleted               = L'd',
	sgosPartialCompletedAndDeleted = L'b',
	sgosNone                       = L'~',
} SunGardOrderStatusEnum;
SunGardOrderStatusEnum ParseSGOrderStatus(wchar_t SGOrderStatusChar);
SunGardOrderStatusEnum ParseSGOrderStatus(const UnicodeString& SGOrderStatusStr);
UnicodeString DescribeSGOrderStatus(SunGardOrderStatusEnum OrderStatusValue);

//--------------------------------------------------------------------------
typedef enum
{
	sgotLimit           =  0,
	sgotMarket          =  1,
	sgotAtBest          =  2,
	sgotStopLoss        =  3,
	sgotTakeProfit      =  4,
	sgotStopLimitLoss   =  5,
	sgotTakeLimitProfit =  6,
	sgotNone            =  9
} SunGardOrderTypeEnum;
SunGardOrderTypeEnum ParseSGOrderType(const UnicodeString& SGOrderTypeStr);
UnicodeString DescribeSGOrderType(SunGardOrderTypeEnum OrderTypeValue);

//--------------------------------------------------------------------------
typedef enum
{
	sgtfGFD  =  0,
	sgtfFOK  =  1,
	sgtfFAK  =  2,
	sgtfNone =  9
} SunGardTimeInForceEnum;
SunGardTimeInForceEnum ParseSGTimeInForce(const UnicodeString& SGTimeInForceStr);
UnicodeString DescribeSGTimeInForce(SunGardTimeInForceEnum TimeInForceValue);

//---------------------------------------------------------------------------
class CSunGardBaseObject
{
protected:
	static wchar_t               FFieldDelimiter;
	SunGardRequestFunctionNoEnum FFunctionNo;

public:
	CSunGardBaseObject(SunGardRequestFunctionNoEnum FFunctionNo);

	SunGardRequestFunctionNoEnum GetFunctionNo() {return FFunctionNo;};
	virtual void GetFieldValue(TStringList *FieldValueList) {};
	virtual void GetFieldValue(const UnicodeString& DataStr) {};
};  //CSunGardBaseObject

//---------------------------------------------------------------------------
class CSGClientFund : public CSunGardBaseObject
{   //6012
private:
	UnicodeString FUserName;
	double        FLastBalance;
	double        FCallFreeze;
	double        FPutFreeze;
	double        FCallMargin;
	double        FPutMargin;
	double        FProfit;
	double        FLoss;
	double        FAvailableFund;
	double        FFloatingInterests;
	double        FFloatingRisk;
	int           FRiskLevel;
	double        FFee;
	double        FFloatingProfit;
	double        FCoverProfit;
	double        FInOutAmount;
	double        FTotalMargin;
	double        FPawningAmount;
	double        FExchangeRiskLevel;

public:
	CSGClientFund(const UnicodeString& DataStr);
	CSGClientFund(TStringList *FieldValueList);

	UnicodeString GetUserName() {return FUserName;};
	double        GetLastBalance() {return FLastBalance;};
	double        GetCallFreeze() {return FCallFreeze;};
	double        GetPutFreeze() {return FPutFreeze;};
	double        GetCallMargin() {return FCallMargin;};
	double        GetPutMargin() {return FPutMargin;};
	double        GetProfit() {return FProfit;};
	double        GetLoss() {return FLoss;};
	double        GetAvailableFund() {return FAvailableFund;};
	double        GetFloatingInterests() {return FFloatingInterests;};
	double        GetFloatingRisk() {return FFloatingRisk;};
	int           GetRiskLevel() {return FRiskLevel;};
	double        GetFee() {return FFee;};
	double        GetFloatingProfit() {return FFloatingProfit;};
	double        GetCoverProfit() {return FCoverProfit;};
	double        GetInOutAmount() {return FInOutAmount;};
	double        GetTotalMargin() {return FTotalMargin;};
	double        GetPawningAmount() {return FPawningAmount;};
	double        GetExchangeRiskLevel() {return FExchangeRiskLevel;};

	void GetFieldValue(TStringList *FieldValueList);
	void GetFieldValue(const UnicodeString& DataStr);
};  //CSGClientFund

//---------------------------------------------------------------------------
class CSGFillDetail : public CSunGardBaseObject
{   //6013
private:
	UnicodeString             FExchangeCode;
	UnicodeString             FExchangeName;
	UnicodeString             FOrderId;
	UnicodeString             FSymbol;
	UnicodeString             FName;
	SunGardSideEnum           FSide;
	UnicodeString             FSideDescription;
	SunGardPositionEffectEnum FPositionEffect;
	UnicodeString             FPositionEffectDescription;
	SunGardStrategyEnum       FStrategy;
	UnicodeString             FStrategyDescription;
	int                       FLastQuantity;
	double                    FLastPrice;
	UnicodeString             FFillDate;
	UnicodeString             FTradingCode;
	UnicodeString             FSystemCenterNo;
	UnicodeString             FFillId;
	UnicodeString             FDeliveryMonth;
	UnicodeString             FFillTime;
	UnicodeString             FSeatNo;
	UnicodeString             FCurrency;
	UnicodeString             FExchangeFillId;

public:
	CSGFillDetail(const UnicodeString& DataStr);
	CSGFillDetail(TStringList *FieldValueList);

	UnicodeString             GetExchangeCode() {return FExchangeCode;};
	UnicodeString             GetExchangeName() {return FExchangeName;};
	UnicodeString             GetOrderId() {return FOrderId;};
	UnicodeString             GetSymbol() {return FSymbol;};
	UnicodeString             GetName() {return FName;};
	SunGardSideEnum           GetSide() {return FSide;};
	UnicodeString             GetSideDescription() {return FSideDescription;};
	SunGardPositionEffectEnum GetPositionEffect() {return FPositionEffect;};
	UnicodeString             GetPositionEffectDescription() {return FPositionEffectDescription;};
	SunGardStrategyEnum       GetStrategy() {return FStrategy;};
	UnicodeString             GetStrategyDescription() {return FStrategyDescription;};
	int                       GetLastQuantity() {return FLastQuantity;};
	double                    GetLastPrice() {return FLastPrice;};
	UnicodeString             GetFillDate() {return FFillDate;};
	UnicodeString             GetTradingCode() {return FTradingCode;};
	UnicodeString             GetSystemCenterNo() {return FSystemCenterNo;};
	UnicodeString             GetFillId() {return FFillId;};
	UnicodeString             GetDeliveryMonth() {return FDeliveryMonth;};
	UnicodeString             GetFillTime() {return FFillTime;};
	UnicodeString             GetSeatNo() {return FSeatNo;};
	UnicodeString             GetCurrency() {return FCurrency;};
	UnicodeString             GetExchangeFillId() {return FExchangeFillId;};

	void GetFieldValue(TStringList *FieldValueList);
	void GetFieldValue(const UnicodeString& DataStr);
};  //CSGFillDetail

//---------------------------------------------------------------------------
class CSGFillStatistic : public CSunGardBaseObject
{   //6013
private:
	UnicodeString             FStatisticKey;
	UnicodeString             FExchangeCode;
	UnicodeString             FExchangeName;
	UnicodeString             FSymbol;
	UnicodeString             FName;
	SunGardSideEnum           FSide;
	UnicodeString             FSideDescription;
	SunGardPositionEffectEnum FPositionEffect;
	UnicodeString             FPositionEffectDescription;
	SunGardStrategyEnum       FStrategy;
	UnicodeString             FStrategyDescription;
	double                    FFillAmount;
	int                       FLastQuantity;
	double                    FLastPrice;
	UnicodeString             FFillDate;
	UnicodeString             FTradingCode;
	UnicodeString             FDeliveryMonth;
	UnicodeString             FCurrency;

public:
	CSGFillStatistic(const UnicodeString& DataStr);
	CSGFillStatistic(TStringList *FieldValueList);

	UnicodeString             GetStatisticKey() {return FStatisticKey;};
	UnicodeString             GetExchangeCode() {return FExchangeCode;};
	UnicodeString             GetExchangeName() {return FExchangeName;};
	UnicodeString             GetSymbol() {return FSymbol;};
	UnicodeString             GetName() {return FName;};
	SunGardSideEnum           GetSide() {return FSide;};
	UnicodeString             GetSideDescription() {return FSideDescription;};
	SunGardPositionEffectEnum GetPositionEffect() {return FPositionEffect;};
	UnicodeString             GetPositionEffectDescription() {return FPositionEffectDescription;};
	SunGardStrategyEnum       GetStrategy() {return FStrategy;};
	UnicodeString             GetStrategyDescription() {return FStrategyDescription;};
	double                    GetFillAmount() {return FFillAmount;};
	int                       GetLastQuantity() {return FLastQuantity;};
	double                    GetLastPrice() {return FLastPrice;};
	UnicodeString             GetFillDate() {return FFillDate;};
	UnicodeString             GetTradingCode() {return FTradingCode;};
	UnicodeString             GetDeliveryMonth() {return FDeliveryMonth;};
	UnicodeString             GetCurrency() {return FCurrency;};

	void GetFieldValue(TStringList *FieldValueList);
	void GetFieldValue(const UnicodeString& DataStr);
	void AddFillDetailData(int LastQuantity, double LastPrice);

	static UnicodeString GenerateStatisticKey(UnicodeString ExchangeCode, UnicodeString Symbol,
											  SunGardSideEnum Side, SunGardPositionEffectEnum PositionEffect);
};  //CSGFillStatistic

//---------------------------------------------------------------------------
class CSGClientPosition : public CSunGardBaseObject
{   //6014
private:
	UnicodeString       FExchangeCode;
	UnicodeString       FExchangeName;
	UnicodeString       FSymbol;
	SunGardStrategyEnum FStrategy;
	int                 FBuyPosition;
	double              FBuyAveragePrice;
	int                 FSellPosition;
	double              FSellAveragePrice;
	double              FBuyFloatingProfit;
	double              FSellFloatingProfit;
	double              FNewestPrice;
	UnicodeString       FStrategyDescription;
	double              FFloatingProfit;
	UnicodeString       FTradingCode;
	int                 FTodayBuyPosition;
	int                 FTodaySellPosition;
	double              FHoldAveragePrice;
	double              FOpenAveragePrice;
	SunGardSideEnum     FSide;

public:
	CSGClientPosition(const UnicodeString& DataStr);
	CSGClientPosition(TStringList *FieldValueList);

	UnicodeString       GetExchangeCode() {return FExchangeCode;};
	UnicodeString       GetExchangeName() {return FExchangeName;};
	UnicodeString       GetSymbol() {return FSymbol;};
	SunGardStrategyEnum GetStrategy() {return FStrategy;};
	int                 GetBuyPosition() {return FBuyPosition;};
	double              GetBuyAveragePrice() {return FBuyAveragePrice;};
	int                 GetSellPosition() {return FSellPosition;};
	double              GetSellAveragePrice() {return FSellAveragePrice;};
	double              GetBuyFloatingProfit() {return FBuyFloatingProfit;};
	double              GetSellFloatingProfit() {return FSellFloatingProfit;};
	double              GetNewestPrice() {return FNewestPrice;};
	UnicodeString       GetStrategyDescription() {return FStrategyDescription;};
	double              GetFloatingProfit() {return FFloatingProfit;};
	UnicodeString       GetTradingCode() {return FTradingCode;};
	int                 GetTodayBuyPosition() {return FTodayBuyPosition;};
	int                 GetTodaySellPosition() {return FTodaySellPosition;};
	double              GetHoldAveragePrice() {return FHoldAveragePrice;};
	double              GetOpenAveragePrice() {return FOpenAveragePrice;};
	SunGardSideEnum     GetSide() {return FSide;};

	void GetFieldValue(TStringList *FieldValueList);
	void GetFieldValue(const UnicodeString& DataStr);
};  //CSGClientPosition

//---------------------------------------------------------------------------
class CSGOrderRecord : public CSunGardBaseObject
{   //6019
private:
	UnicodeString             FExchangeCode;
	UnicodeString             FExchangeName;
	UnicodeString             FOrderId;
	UnicodeString             FSymbol;
	UnicodeString             FName;
	SunGardOrderStatusEnum    FOrderStatus;
	UnicodeString             FOrderStatusDescription;
	SunGardSideEnum           FSide;
	UnicodeString             FSideDescription;
	SunGardPositionEffectEnum FPositionEffect;
	UnicodeString             FPositionEffectDescription;
	SunGardStrategyEnum       FStrategy;
	UnicodeString             FStrategyDescription;
	int                       FOrderQuantity;
	double                    FOrderPrice;
	int                       FLastQuantity;
	double                    FLastPrice;
	int                       FLeavesQuantity;
	UnicodeString             FSystemCenterNo;
	UnicodeString             FTradingCode;
	UnicodeString             FOrderTime;
	UnicodeString             FDeclarationTime;
	UnicodeString             FCancelTime;
	UnicodeString             FSeatNo;
	UnicodeString             FOrderSeatNo;
	UnicodeString             FLocalFlag;
	SunGardOrderTypeEnum      FOrderType;
	SunGardTimeInForceEnum    FTimeInForce;
	double                    FStopPrice;
	UnicodeString             FTriggerTime;

public:
	CSGOrderRecord(const UnicodeString& DataStr);
	CSGOrderRecord(TStringList *FieldValueList);

	UnicodeString             GetExchangeCode() {return FExchangeCode;};
	UnicodeString             GetExchangeName() {return FExchangeName;};
	UnicodeString             GetOrderId() {return FOrderId;};
	UnicodeString             GetSymbol() {return FSymbol;};
	UnicodeString             GetName() {return FName;};
	SunGardOrderStatusEnum    GetOrderStatus() {return FOrderStatus;};
	UnicodeString             GetOrderStatusDescription() {return FOrderStatusDescription;};
	SunGardSideEnum           GetSide() {return FSide;};
	UnicodeString             GetSideDescription() {return FSideDescription;};
	SunGardPositionEffectEnum GetPositionEffect() {return FPositionEffect;};
	UnicodeString             GetPositionEffectDescription() {return FPositionEffectDescription;};
	SunGardStrategyEnum       GetStrategy() {return FStrategy;};
	UnicodeString             GetStrategyDescription() {return FStrategyDescription;};
	int                       GetOrderQuantity() {return FOrderQuantity;};
	double                    GetOrderPrice() {return FOrderPrice;};
	int                       GetLastQuantity() {return FLastQuantity;};
	double                    GetLastPrice() {return FLastPrice;};
	int                       GetLeavesQuantity() {return FLeavesQuantity;};
	UnicodeString             GetFSystemCenterNo() {return FSystemCenterNo;};
	UnicodeString             GetTradingCode() {return FTradingCode;};
	UnicodeString             GetOrderTime() {return FOrderTime;};
	UnicodeString             GetDeclarationTime() {return FDeclarationTime;};
	UnicodeString             GetCancelTime() {return FCancelTime;};
	UnicodeString             GetSeatNo() {return FSeatNo;};
	UnicodeString             GetOrderSeatNo() {return FOrderSeatNo;};
	UnicodeString             GetLocalFlag() {return FLocalFlag;};
	SunGardOrderTypeEnum      GetOrderType() {return FOrderType;};
	SunGardTimeInForceEnum    GetTimeInForce() {return FTimeInForce;};
	double                    GetStopPrice() {return FStopPrice;};
	UnicodeString             GetTriggerTime() {return FTriggerTime;};

	void GetFieldValue(TStringList *FieldValueList);
	void GetFieldValue(const UnicodeString& DataStr);
};  //CSGOrderRecord

//---------------------------------------------------------------------------
class CSGHoldPosition : public CSunGardBaseObject
{   //6502
private:
	UnicodeString FAccount;
	UnicodeString FExchangeCode;
	UnicodeString FSymbol;
	double        FPriceMultiplier;
	int           FBuyQuantity;
	double        FBuyAmount;
	double        FBuyAveragePrice;
	int           FSellQuantity;
	double        FSellAmount;
	double        FSellAveragePrice;

public:
	CSGHoldPosition(const UnicodeString& DataStr);
	CSGHoldPosition(TStringList *FieldValueList);

	UnicodeString GetAccount() {return FAccount;};
	UnicodeString GetExchangeCode() {return FExchangeCode;};
	UnicodeString GetSymbol() {return FSymbol;};
	double        GetPriceMultiplier() {return FPriceMultiplier;};
	int           GetBuyQuantity() {return FBuyQuantity;};
	double        GetBuyAmount() {return FBuyAmount;};
	double        GetBuyAveragePrice() {return FBuyAveragePrice;};
	int           GetSellQuantity() {return FSellQuantity;};
	double        GetSellAmount() {return FSellAmount;};
	double        GetSellAveragePrice() {return FSellAveragePrice;};

	void GetFieldValue(TStringList *FieldValueList);
	void GetFieldValue(const UnicodeString& DataStr);
};  //CSGHoldPosition
//---------------------------------------------------------------------------
class CSGAccount
{
private:
	UnicodeString                   FAccount;
	CSGClientFund                   *FFundPtr;
	std::vector<CSGFillDetail*>     FFillDetailArray;
	std::vector<CSGClientPosition*> FPositionArray;
	std::vector<CSGOrderRecord*>    FOrderRecordArray;
	std::vector<UnicodeString*>     FSettlementArray;

	std::map<UnicodeString, CSGFillStatistic*>  FFillStatisticIndex;
	std::vector<CSGFillStatistic*>              FFillStatisticArray;

	boolean                         FIsFundSuccess;
	UnicodeString                   FFundErrorMsg;
	boolean                         FIsFillDetailSuccess;
	UnicodeString                   FFillDetailErrorMsg;
	boolean                         FIsPositionSuccess;
	UnicodeString                   FPositionErrorMsg;
	boolean                         FIsOrderRecordSuccess;
	UnicodeString                   FOrderRecordErrorMsg;
	boolean                         FIsSettlementSuccess;
	UnicodeString                   FSettlementErrorMsg;

public:
	CSGAccount(const UnicodeString& Account);
	~CSGAccount();

	UnicodeString GetAccount() {return FAccount;};
	boolean IsFundSuccess() {return FIsFundSuccess;};
	UnicodeString GetFundErrorMsg() {return FFundErrorMsg;};
	boolean IsFillDetailSuccess() {return FIsFillDetailSuccess;};
	UnicodeString GetFillDetailErrorMsg() {return FFillDetailErrorMsg;};
	boolean IsPositionSuccess() {return FIsPositionSuccess;};
	UnicodeString GetPositionErrorMsg() {return FPositionErrorMsg;};
	boolean IsOrderRecordSuccess() {return FIsOrderRecordSuccess;};
	UnicodeString GetOrderRecordErrorMsg() {return FOrderRecordErrorMsg;};
	boolean IsSettlementSuccess() {return FIsSettlementSuccess;};
	UnicodeString GetSettlementErrorMsg() {return FSettlementErrorMsg;};

	void SetFundSuccess(boolean FundSuccess) {FIsFundSuccess = FundSuccess;};
	void SetFundErrorMsg(UnicodeString FundErrorMsg) {FFundErrorMsg = FundErrorMsg;};
	void SetFillDetailSuccess(boolean FillDetailSuccess) {FIsFillDetailSuccess = FillDetailSuccess;};
	void SetFillDetailErrorMsg(UnicodeString FillDetailErrorMsg) {FFillDetailErrorMsg = FillDetailErrorMsg;};
	void SetPositionSuccess(boolean PositionSuccess) {FIsPositionSuccess = PositionSuccess;};
	void SetPositionErrorMsg(UnicodeString PositionErrorMsg) {FPositionErrorMsg = PositionErrorMsg;};
	void SetOrderRecordSuccess(boolean OrderRecordSuccess) {FIsOrderRecordSuccess = OrderRecordSuccess;};
	void SetOrderRecordErrorMsg(UnicodeString OrderRecordErrorMsg) {FOrderRecordErrorMsg = OrderRecordErrorMsg;};
	void SetSettlementSuccess(boolean SettlementSuccess) {FIsSettlementSuccess = SettlementSuccess;};
	void SetSettlementErrorMsg(UnicodeString SettlementErrorMsg) {FSettlementErrorMsg = SettlementErrorMsg;};

	CSGClientFund *GetFund() {return FFundPtr;};
	void SetFund(CSGClientFund *FundPtr) {FFundPtr = FundPtr;};
	void ClearFund();

	int GetFillDetailCount() {return FFillDetailArray.size();};
	CSGFillDetail *GetFillDetailByIndex(int Index);
	void AddFillDetail(CSGFillDetail *FillDetailPtr);
	void ClearFillDetail();

	int GetPositionCount() {return FPositionArray.size();};
	CSGClientPosition *GetPositionByIndex(int Index);
	void AddPosition(CSGClientPosition *PositionPtr);
	void ClearPosition();

	int GetOrderRecordCount() {return FOrderRecordArray.size();};
	CSGOrderRecord *GetOrderRecordByIndex(int Index);
	void AddOrderRecord(CSGOrderRecord *OrderRecordPtr);
	void ClearOrderRecord();

	int GetSettlementCount() {return FSettlementArray.size();};
	UnicodeString *GetSettlementByIndex(int Index);
	void AddSettlement(UnicodeString *SettlementPtr);
	void ClearSettlement();

	CSGFillStatistic *FindFillStatistic(const UnicodeString& FillStatisticKey);
	bool InsertFillStatisticIndex(const UnicodeString& FillStatisticKey, CSGFillStatistic *FillStatisticPtr);
	int GetFillStatisticCount() {return FFillStatisticArray.size();};
	CSGFillStatistic *GetFillStatisticByIndex(int Index);
	void AddFillStatistic(CSGFillStatistic *FillStatisticPtr);
	void ClearFillStatistic();
};  //CSGAccount
#endif
