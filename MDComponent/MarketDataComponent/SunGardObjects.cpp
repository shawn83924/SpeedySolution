//---------------------------------------------------------------------------


#pragma hdrstop

#include "SunGardObjects.h"
#include "MDComponentStrings.hpp"

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
SunGardRequestFunctionNoEnum ParseSGRequestFunctionNo(const UnicodeString& SGRequestFunctionNoStr)
{
	int IntValue = SGRequestFunctionNoStr.ToInt();
	switch(IntValue)
	{
		case 6012: return sgQueryClientFund;
		case 6013: return sgQueryFillDetail;
		case 6014: return sgQueryClientPosition;
		case 6019: return sgQueryOrderRecord;
		case 6023: return sgChangePassword;
		case 6035: return sgQueryClientOpeningPosition;
		case 6041: return sgTransformMoney;
		case 6042: return sgQueryUserBankAccount;
		case 6043: return sgQueryUserTransformRecord;
		case 6052: return sgQueryUserBankAvailableCash;
		case 6055: return sgQueryUserAvailableFund;
		case 6071: return sgConfirmSettlement;
		case 6072: return sgQuerySettlement;
		case 6501: return sgAnotherLogonOn;
		case 6502: return sgQueryClientHoldPosition;
		default:   return sgNoneFunction;
	}
}  //ParseSGRequestFunctionNo()
//---------------------------------------------------------------------------
SunGardSideEnum ParseSGSide(const UnicodeString& SGSideStr)
{
	int IntValue = SGSideStr.ToInt();
	switch(IntValue)
	{
		case 0:  return sgsdBuy;
		case 1:  return sgsdSell;
		default: return sgsdNone;
	}
}  //ParseSGSide()

//---------------------------------------------------------------------------
UnicodeString DescribeSGSide(SunGardSideEnum SideValue)
{
	switch(SideValue)
	{
		case sgsdBuy:   return Mdcomponentstrings_MD_SG_BUY;   //L"禦";
		case sgsdSell:  return Mdcomponentstrings_MD_SG_SELL;  //L"芥";
		default:        return L"None";
	}
}  //DescribeSGSide()

//---------------------------------------------------------------------------
SunGardPositionEffectEnum ParseSGPositionEffect(const UnicodeString& SGPositionEffectStr)
{
	int IntValue = SGPositionEffectStr.ToInt();
	switch(IntValue)
	{
		case 0:  return sgpeOpen;
		case 1:  return sgpeClose;
		case 2:  return sgpeCloseToday;
		default: return sgpeNone;
	}
}  //PParseSGPositionEffect()
//--------------------------------------------------------------------------
UnicodeString DescribeSGPositionEffect(SunGardPositionEffectEnum PositionEffectValue)
{
	switch(PositionEffectValue)
	{
		case sgpeOpen:       return Mdcomponentstrings_MD_SG_OPEN;         //L"秨";
		case sgpeClose:      return Mdcomponentstrings_MD_SG_CLOSE;        //L"キ";
		case sgpeCloseToday: return Mdcomponentstrings_MD_SG_CLOSE_TODAY;  //L"キさ";
		default:             return L"None";
	}
}  //DescribeSGPositionEffect()
//--------------------------------------------------------------------------
SunGardStrategyEnum ParseSGStrategy(const UnicodeString& SGStrategyStr)
{
	int IntValue = SGStrategyStr.ToInt();
	switch(IntValue)
	{
		case 0:  return sgstSpeculate;
		case 1:  return sgstHedge;
		default: return sgstNone;
	}
}  //ParseSGStrategy()
//--------------------------------------------------------------------------
UnicodeString DescribeSGStrategy(SunGardStrategyEnum StrategyValue)
{
	switch(StrategyValue)
	{
		case sgstSpeculate:  return Mdcomponentstrings_MD_SG_SPECULATE;  //L"щ诀";
		case sgstHedge:      return Mdcomponentstrings_MD_SG_HEDGE;      //L"玂";
		default:             return L"None";
	}
}  //DescribeSGStrategy()
//--------------------------------------------------------------------------
SunGardOrderStatusEnum ParseSGOrderStatus(wchar_t SGOrderStatusChar)
{
	switch(SGOrderStatusChar)
	{
		case L'n': return sgosWaiting;
		case L's': return sgosBaodaning;
		case L'a': return sgosAlreadyBaodaned;
		case L'p': return sgosPartialCompleted;
		case L'c': return sgosCompleted;
		case L'f': return sgosDeleteing;
		case L'e': return sgosErrorOrder;
		case L'q': return sgosSystemDisable;
		case L'd': return sgosTotalDeleted;
		case L'b': return sgosPartialCompletedAndDeleted;
		default:   return sgosNone;
	}  //switch(SGOrderStatusChar)
}  //ParseSGOrderStatus()
//--------------------------------------------------------------------------
SunGardOrderStatusEnum ParseSGOrderStatus(const UnicodeString& SGOrderStatusStr)
{
	SunGardOrderStatusEnum SgOrderStatus = sgosNone;
	if (SGOrderStatusStr.Length() > 0)
	{
		wchar_t OrderStatusChar = SGOrderStatusStr[1];
		SgOrderStatus = ParseSGOrderStatus(OrderStatusChar);
	}
	return SgOrderStatus;
}  //ParseSGOrderStatus()
//--------------------------------------------------------------------------
UnicodeString DescribeSGOrderStatus(SunGardOrderStatusEnum OrderStatusValue)
{
	switch(OrderStatusValue)
	{
		case sgosWaiting:                    return Mdcomponentstrings_MD_SG_WAITING;                         //L"单祇";
		case sgosBaodaning:                  return Mdcomponentstrings_MD_SG_BAODANING;                       //L"タビ厨";
		case sgosAlreadyBaodaned:            return Mdcomponentstrings_MD_SG_ALREADY_BAODANED;                //L"竒厨";
		case sgosPartialCompleted:           return Mdcomponentstrings_MD_SG_PARTIAL_COMPLETED;               //L"场だΘユ";
		case sgosCompleted:                  return Mdcomponentstrings_MD_SG_COMPLETED;                       //L"场Θユ";
		case sgosDeleteing:                  return Mdcomponentstrings_MD_SG_DELETEING;                       //L"单篗埃";
		case sgosErrorOrder:                 return Mdcomponentstrings_MD_SG_ERROR_ORDER;                     //L"岿粇〆癠";
		case sgosSystemDisable:              return Mdcomponentstrings_MD_SG_SYSTEM_DISABLE;                  //L"初ず┶荡";
		case sgosTotalDeleted:               return Mdcomponentstrings_MD_SG_TOTAL_DELETED;                   //L"竒篗綪";
		case sgosPartialCompletedAndDeleted: return Mdcomponentstrings_MD_SG_PARTIAL_COMPLETED_AND_DELETED ;  //L"场Θ场篗";
		default:                             return L"None";
	}  //switch(SGOrderStatusChar)
}  //DescribeSGOrderStatus()
//--------------------------------------------------------------------------
SunGardOrderTypeEnum ParseSGOrderType(const UnicodeString& SGOrderTypeStr)
{
	int IntValue = SGOrderTypeStr.ToInt();
	switch(IntValue)
	{
		case 0:  return sgotLimit;
		case 1:  return sgotMarket;
		case 2:  return sgotAtBest;
		case 3:  return sgotStopLoss;
		case 4:  return sgotTakeProfit;
		case 5:  return sgotStopLimitLoss;
		case 6:  return sgotTakeLimitProfit;
		default: return sgotNone;
	}
}  //ParseSGOrderType()
//--------------------------------------------------------------------------
UnicodeString DescribeSGOrderType(SunGardOrderTypeEnum OrderTypeValue)
{
	switch(OrderTypeValue)
	{
		case sgotLimit:           return Mdcomponentstrings_MD_SG_LIMIT;              //L"基";
		case sgotMarket:          return Mdcomponentstrings_MD_SG_MARKET;             //L"カ基";
		case sgotAtBest:          return Mdcomponentstrings_MD_SG_AT_BEST;            //L"程纔基";
		case sgotStopLoss:        return Mdcomponentstrings_MD_SG_STOP_LOSS;          //L"カ基ゎ穕";
		case sgotTakeProfit:      return Mdcomponentstrings_MD_SG_TAKE_PROFIT;        //L"カ基ゎ";
		case sgotStopLimitLoss:   return Mdcomponentstrings_MD_SG_STOP_LIMIT_LOSS;    //L"基ゎ穕";
		case sgotTakeLimitProfit: return Mdcomponentstrings_MD_SG_TAKE_LIMIT_PROFIT;  //L"基ゎ";
		default:                  return L"None";
	}
}  //DescribeSGOrderType()
//--------------------------------------------------------------------------
SunGardTimeInForceEnum ParseSGTimeInForce(const UnicodeString& SGTimeInForceStr)
{
	int IntValue = SGTimeInForceStr.ToInt();
	switch(IntValue)
	{
		case 0:  return sgtfGFD;
		case 1:  return sgtfFOK;
		case 2:  return sgtfFAK;
		default: return sgtfNone;
	}
}  //ParseSGTimeInForce()
//--------------------------------------------------------------------------
UnicodeString DescribeSGTimeInForce(SunGardTimeInForceEnum TimeInForceValue)
{
	switch(TimeInForceValue)
	{
		case sgtfGFD: return Mdcomponentstrings_MD_SG_GFD;  //L"讽らΤ(GFD)";
		case sgtfFOK: return Mdcomponentstrings_MD_SG_FOK;  //L"Θ┪篗(FOK)";
		case sgtfFAK: return Mdcomponentstrings_MD_SG_FAK;  //L"逞緇篗虫(FAK)";
		default:      return L"";
	}
}  //DescribeSGTimeInForce()
//--------------------------- CSunGardBaseObject ----------------------------
wchar_t CSunGardBaseObject::FFieldDelimiter = L'|';
//---------------------------------------------------------------------------
CSunGardBaseObject::CSunGardBaseObject(SunGardRequestFunctionNoEnum FunctionNo)
:FFunctionNo(FFunctionNo)
{
}  //CSunGardBaseObject::CSunGardBaseObject()

//--------------------------- CSGClientFund 6012 ----------------------------
//---------------------------------------------------------------------------
CSGClientFund::CSGClientFund(const UnicodeString& DataStr)
:CSunGardBaseObject(sgQueryClientFund)
{
	GetFieldValue(DataStr);
}  //CSGClientFund::CSGClientFund()

//---------------------------------------------------------------------------
CSGClientFund::CSGClientFund(TStringList *FieldValueList)
:CSunGardBaseObject(sgQueryClientFund)
{
	GetFieldValue(FieldValueList);
}  //CSGClientFund::CSGClientFund()

//---------------------------------------------------------------------------
void CSGClientFund::GetFieldValue(TStringList *FieldValueList)
{
	int FieldCount = FieldValueList->Count;
	if ((FieldCount >= 5) && ((*FieldValueList)[4].Length() > 0))
		FUserName = (*FieldValueList)[4];

	if ((FieldCount >= 6) && ((*FieldValueList)[5].Length() > 0))
		FLastBalance = (*FieldValueList)[5].ToDouble();

	if ((FieldCount >= 7) && ((*FieldValueList)[6].Length() > 0))
		FCallFreeze = (*FieldValueList)[6].ToDouble();

	if ((FieldCount >= 8) && ((*FieldValueList)[7].Length() > 0))
		FPutFreeze = (*FieldValueList)[7].ToDouble();

	if ((FieldCount >= 9) && ((*FieldValueList)[8].Length() > 0))
		FCallMargin = (*FieldValueList)[8].ToDouble();

	if ((FieldCount >= 10) && ((*FieldValueList)[9].Length() > 0))
		FPutMargin = (*FieldValueList)[9].ToDouble();

	if ((FieldCount >= 11) && ((*FieldValueList)[10].Length() > 0))
		FProfit = (*FieldValueList)[10].ToDouble();

	if ((FieldCount >= 12) && ((*FieldValueList)[11].Length() > 0))
		FLoss = (*FieldValueList)[11].ToDouble();

	if ((FieldCount >= 13) && ((*FieldValueList)[12].Length() > 0))
		FAvailableFund = (*FieldValueList)[12].ToDouble();

	if ((FieldCount >= 14) && ((*FieldValueList)[13].Length() > 0))
		FFloatingInterests = (*FieldValueList)[13].ToDouble();

	if ((FieldCount >= 15) && ((*FieldValueList)[14].Length() > 0))
		FFloatingRisk = (*FieldValueList)[14].ToDouble();

	if ((FieldCount >= 16) && ((*FieldValueList)[15].Length() > 0))
		FRiskLevel = (*FieldValueList)[15].ToInt();

	if ((FieldCount >= 17) && ((*FieldValueList)[16].Length() > 0))
		FFee = (*FieldValueList)[16].ToDouble();

	if ((FieldCount >= 18) && ((*FieldValueList)[17].Length() > 0))
		FFloatingProfit = (*FieldValueList)[17].ToDouble();

	if ((FieldCount >= 19) && ((*FieldValueList)[18].Length() > 0))
		FCoverProfit = (*FieldValueList)[18].ToDouble();

	if ((FieldCount >= 20) && ((*FieldValueList)[19].Length() > 0))
		FInOutAmount = (*FieldValueList)[19].ToDouble();

	if ((FieldCount >= 21) && ((*FieldValueList)[20].Length() > 0))
		FTotalMargin = (*FieldValueList)[20].ToDouble();

	if ((FieldCount >= 22) && ((*FieldValueList)[21].Length() > 0))
		FPawningAmount = (*FieldValueList)[21].ToDouble();

	if ((FieldCount >= 23) && ((*FieldValueList)[22].Length() > 0))
		FExchangeRiskLevel = (*FieldValueList)[22].ToDouble();
}  //CSGClientFund::GetFieldValue()

//---------------------------------------------------------------------------
void CSGClientFund::GetFieldValue(const UnicodeString& DataStr)
{
	TStringList *FieldValueList = new TStringList();
	FieldValueList->Delimiter = FFieldDelimiter;
	FieldValueList->DelimitedText = DataStr;
	GetFieldValue(FieldValueList);
	delete FieldValueList;
}  //CSGClientFund::GetFieldValue()

//--------------------------- CSGFillDetail 6013 ----------------------------
//---------------------------------------------------------------------------
CSGFillDetail::CSGFillDetail(const UnicodeString& DataStr)
:CSunGardBaseObject(sgQueryFillDetail)
{
	GetFieldValue(DataStr);
}  //CSGFillDetail::CSGFillDetail()

//---------------------------------------------------------------------------
CSGFillDetail::CSGFillDetail(TStringList *FieldValueList)
:CSunGardBaseObject(sgQueryFillDetail)
{
	GetFieldValue(FieldValueList);
}  //CSGFillDetail::CSGFillDetail()

//---------------------------------------------------------------------------
void CSGFillDetail::GetFieldValue(TStringList *FieldValueList)
{
	int FieldCount = FieldValueList->Count;
	if (FieldCount >= 5) FExchangeCode = (*FieldValueList)[4];
	if (FieldCount >= 6) FExchangeName = (*FieldValueList)[5];
	if (FieldCount >= 7) FOrderId = (*FieldValueList)[6];
	if (FieldCount >= 8) FSymbol = (*FieldValueList)[7];
	if (FieldCount >= 9) FName = (*FieldValueList)[8];
	if (FieldCount >= 10) FSide = ParseSGSide((*FieldValueList)[9]);
	if (FieldCount >= 11) FSideDescription = (*FieldValueList)[10];
	if (FieldCount >= 12) FPositionEffect = ParseSGPositionEffect((*FieldValueList)[11]);
	if (FieldCount >= 13) FPositionEffectDescription = (*FieldValueList)[12];
	if (FieldCount >= 14) FStrategy = ParseSGStrategy((*FieldValueList)[13]);
	if (FieldCount >= 15) FStrategyDescription = (*FieldValueList)[14];
	if (FieldCount >= 16) FLastQuantity = (*FieldValueList)[15].ToInt();
	if (FieldCount >= 17) FLastPrice = (*FieldValueList)[16].ToDouble();
	if (FieldCount >= 18) FFillDate = (*FieldValueList)[17];
	if (FieldCount >= 19) FTradingCode = (*FieldValueList)[18];
	if (FieldCount >= 20) FSystemCenterNo = (*FieldValueList)[19];
	if (FieldCount >= 21) FFillId = (*FieldValueList)[20];
	if (FieldCount >= 22) FDeliveryMonth = (*FieldValueList)[21];
	if (FieldCount >= 23) FFillTime = (*FieldValueList)[22];
	if (FieldCount >= 24) FSeatNo = (*FieldValueList)[23];
	if (FieldCount >= 25) FCurrency = (*FieldValueList)[24];
	if (FieldCount >= 26) FExchangeFillId = (*FieldValueList)[25];
}  //CSGFillDetail::GetFieldValue()

//---------------------------------------------------------------------------
void CSGFillDetail::GetFieldValue(const UnicodeString& DataStr)
{
	TStringList *FieldValueList = new TStringList();
	FieldValueList->Delimiter = FFieldDelimiter;
	FieldValueList->DelimitedText = DataStr;
	GetFieldValue(FieldValueList);
	delete FieldValueList;
}  //CSGFillDetail::GetFieldValue()

//---------------------------- CSGFillStatistic -----------------------------
//---------------------------------------------------------------------------
CSGFillStatistic::CSGFillStatistic(const UnicodeString& DataStr)
:CSunGardBaseObject(sgQueryFillDetail)
{
	GetFieldValue(DataStr);
}  //CSGFillStatistic::CSGFillStatistic()

//---------------------------------------------------------------------------
CSGFillStatistic::CSGFillStatistic(TStringList *FieldValueList)
:CSunGardBaseObject(sgQueryFillDetail)
{
	GetFieldValue(FieldValueList);
}  //CSGFillStatistic::CSGFillStatistic()

//---------------------------------------------------------------------------
void CSGFillStatistic::GetFieldValue(TStringList *FieldValueList)
{
	int FieldCount = FieldValueList->Count;
	if (FieldCount >= 5) FExchangeCode = (*FieldValueList)[4];
	if (FieldCount >= 6) FExchangeName = (*FieldValueList)[5];
	if (FieldCount >= 8) FSymbol = (*FieldValueList)[7];
	if (FieldCount >= 9) FName = (*FieldValueList)[8];
	if (FieldCount >= 10) FSide = ParseSGSide((*FieldValueList)[9]);
	if (FieldCount >= 11) FSideDescription = (*FieldValueList)[10];
	if (FieldCount >= 12) FPositionEffect = ParseSGPositionEffect((*FieldValueList)[11]);
	if (FieldCount >= 13) FPositionEffectDescription = (*FieldValueList)[12];
	if (FieldCount >= 14) FStrategy = ParseSGStrategy((*FieldValueList)[13]);
	if (FieldCount >= 15) FStrategyDescription = (*FieldValueList)[14];
	if (FieldCount >= 16) FLastQuantity = (*FieldValueList)[15].ToInt();
	if (FieldCount >= 17) FLastPrice = (*FieldValueList)[16].ToDouble();
	if (FieldCount >= 18) FFillDate = (*FieldValueList)[17];
	if (FieldCount >= 19) FTradingCode = (*FieldValueList)[18];
	if (FieldCount >= 22) FDeliveryMonth = (*FieldValueList)[21];
	if (FieldCount >= 25) FCurrency = (*FieldValueList)[24];
	FStatisticKey = GenerateStatisticKey(FExchangeCode, FSymbol, FSide, FPositionEffect);
	FFillAmount = FLastPrice * FLastQuantity;
}  //CSGFillStatistic::GetFieldValue()

//---------------------------------------------------------------------------
void CSGFillStatistic::GetFieldValue(const UnicodeString& DataStr)
{
	TStringList *FieldValueList = new TStringList();
	FieldValueList->Delimiter = FFieldDelimiter;
	FieldValueList->DelimitedText = DataStr;
	GetFieldValue(FieldValueList);
	delete FieldValueList;
}  //CSGFillStatistic::GetFieldValue()

//---------------------------------------------------------------------------
UnicodeString CSGFillStatistic::GenerateStatisticKey(UnicodeString ExchangeCode, UnicodeString Symbol,
													 SunGardSideEnum Side, SunGardPositionEffectEnum PositionEffect)
{
	UnicodeString StatisticKey = L"";
	if (ExchangeCode.Length() > 0) StatisticKey = ExchangeCode;
	if (Symbol.Length() > 0) StatisticKey = StatisticKey + Symbol;
	UnicodeString SideStr;
	SideStr.printf(L"%02d", Side);
	StatisticKey = StatisticKey + SideStr;
	UnicodeString PositionEffectStr;
	PositionEffectStr.printf(L"%02d", PositionEffect);
	StatisticKey = StatisticKey + PositionEffectStr;
	return StatisticKey;
}  //CSGFillStatistic::GenerateStatisticKey()

//---------------------------------------------------------------------------
void CSGFillStatistic::AddFillDetailData(int LastQuantity, double LastPrice)
{
	FFillAmount = FFillAmount + (LastPrice * LastQuantity);
	FLastQuantity = FLastQuantity + LastQuantity;
	if (FLastQuantity > 0)
		FLastPrice = FFillAmount / FLastQuantity;
	else
		FLastPrice = 0.0;
}  //CSGFillStatistic::AddFillDetailData()

//------------------------- CSGClientPosition 6014 --------------------------
//---------------------------------------------------------------------------
CSGClientPosition::CSGClientPosition(const UnicodeString& DataStr)
:CSunGardBaseObject(sgQueryClientPosition)
{
	GetFieldValue(DataStr);
}  //CSGClientPosition::CSGClientPosition()

//---------------------------------------------------------------------------
CSGClientPosition::CSGClientPosition(TStringList *FieldValueList)
:CSunGardBaseObject(sgQueryClientPosition)
{
	GetFieldValue(FieldValueList);
}  //CSGClientPosition::CSGClientPosition()

//---------------------------------------------------------------------------
void CSGClientPosition::GetFieldValue(TStringList *FieldValueList)
{
	int FieldCount = FieldValueList->Count;
	if (FieldCount >= 5) FExchangeCode = (*FieldValueList)[4];
	if (FieldCount >= 6) FExchangeName = (*FieldValueList)[5];
	if (FieldCount >= 7) FSymbol = (*FieldValueList)[6];
	if (FieldCount >= 8) FStrategy = ParseSGStrategy((*FieldValueList)[7]);
	if (FieldCount >= 9) FBuyPosition = (*FieldValueList)[8].ToInt();
	if (FieldCount >= 10) FBuyAveragePrice = (*FieldValueList)[9].ToDouble();
	if (FieldCount >= 11) FSellPosition = (*FieldValueList)[10].ToInt();
	if (FieldCount >= 12) FSellAveragePrice = (*FieldValueList)[11].ToDouble();
	if (FieldCount >= 13) FBuyFloatingProfit = (*FieldValueList)[12].ToDouble();
	if (FieldCount >= 14) FSellFloatingProfit = (*FieldValueList)[13].ToDouble();
	if (FieldCount >= 15) FNewestPrice = (*FieldValueList)[14].ToDouble();
	if (FieldCount >= 16) FStrategyDescription = (*FieldValueList)[15];
	if (FieldCount >= 17) FFloatingProfit = (*FieldValueList)[16].ToDouble();
	if (FieldCount >= 18) FTradingCode = (*FieldValueList)[17];
	if (FieldCount >= 19) FTodayBuyPosition = (*FieldValueList)[18].ToInt();
	if (FieldCount >= 20) FTodaySellPosition = (*FieldValueList)[19].ToInt();
	if (FieldCount >= 21) FHoldAveragePrice = (*FieldValueList)[20].ToDouble();
	if (FieldCount >= 22) FOpenAveragePrice = (*FieldValueList)[21].ToDouble();
	if (FieldCount >= 23) FSide = ParseSGSide((*FieldValueList)[22]);
}  //CSGClientPosition::GetFieldValue()

//---------------------------------------------------------------------------
void CSGClientPosition::GetFieldValue(const UnicodeString& DataStr)
{
	TStringList *FieldValueList = new TStringList();
	FieldValueList->Delimiter = FFieldDelimiter;
	FieldValueList->DelimitedText = DataStr;
	GetFieldValue(FieldValueList);
	delete FieldValueList;
};  //CSGClientPosition::GetFieldValue()

//--------------------------- CSGOrderRecord 6019 ---------------------------
//---------------------------------------------------------------------------
CSGOrderRecord::CSGOrderRecord(const UnicodeString& DataStr)
:CSunGardBaseObject(sgQueryOrderRecord)
{
	GetFieldValue(DataStr);
}  //CSGOrderRecord::CSGOrderRecord()

//---------------------------------------------------------------------------
CSGOrderRecord::CSGOrderRecord(TStringList *FieldValueList)
:CSunGardBaseObject(sgQueryOrderRecord)
{
	GetFieldValue(FieldValueList);
}  //CSGOrderRecord::CSGOrderRecord()

//---------------------------------------------------------------------------
void CSGOrderRecord::GetFieldValue(TStringList *FieldValueList)
{
	int FieldCount = FieldValueList->Count;
	if (FieldCount >= 5) FExchangeCode = (*FieldValueList)[4];
	if (FieldCount >= 6) FExchangeName = (*FieldValueList)[5];
	if (FieldCount >= 7) FOrderId = (*FieldValueList)[6];
	if (FieldCount >= 8) FSymbol = (*FieldValueList)[7];
	if (FieldCount >= 9) FName = (*FieldValueList)[8];
	if (FieldCount >= 10) FOrderStatus = ParseSGOrderStatus((*FieldValueList)[9]);
	if (FieldCount >= 11) FOrderStatusDescription = (*FieldValueList)[10];
	if (FieldCount >= 12) FSide = ParseSGSide((*FieldValueList)[11]);
	if (FieldCount >= 13) FSideDescription = (*FieldValueList)[12];
	if (FieldCount >= 14) FPositionEffect = ParseSGPositionEffect((*FieldValueList)[13]);
	if (FieldCount >= 15) FPositionEffectDescription = (*FieldValueList)[14];
	if (FieldCount >= 16) FStrategy = ParseSGStrategy((*FieldValueList)[15]);
	if (FieldCount >= 17) FStrategyDescription = (*FieldValueList)[16];
	if (FieldCount >= 18) FOrderQuantity = (*FieldValueList)[17].ToInt();
	if (FieldCount >= 19) FOrderPrice = (*FieldValueList)[18].ToDouble();
	if (FieldCount >= 20) FLastQuantity = (*FieldValueList)[19].ToInt();
	if (FieldCount >= 21) FLastPrice = (*FieldValueList)[20].ToDouble();
	if (FieldCount >= 22) FLeavesQuantity = (*FieldValueList)[21].ToInt();
	if (FieldCount >= 23) FSystemCenterNo = (*FieldValueList)[22];
	if (FieldCount >= 24) FTradingCode = (*FieldValueList)[23];
	if (FieldCount >= 25) FOrderTime = (*FieldValueList)[24];
	if (FieldCount >= 26) FDeclarationTime = (*FieldValueList)[25];
	if (FieldCount >= 27) FCancelTime = (*FieldValueList)[26];
	if (FieldCount >= 28) FSeatNo = (*FieldValueList)[27];
	if (FieldCount >= 29) FOrderSeatNo = (*FieldValueList)[28];
	if (FieldCount >= 30) FLocalFlag = (*FieldValueList)[29];
	if (FieldCount >= 31) FOrderType = ParseSGOrderType((*FieldValueList)[30]);
	if (FieldCount >= 32) FTimeInForce = ParseSGTimeInForce((*FieldValueList)[31]);
	if (FieldCount >= 33) FStopPrice = (*FieldValueList)[32].ToDouble();
	if (FieldCount >= 34) FTriggerTime = (*FieldValueList)[33];
}  //CSGOrderRecord::GetFieldValue()

//---------------------------------------------------------------------------
void CSGOrderRecord::GetFieldValue(const UnicodeString& DataStr)
{
	TStringList *FieldValueList = new TStringList();
	FieldValueList->Delimiter = FFieldDelimiter;
	FieldValueList->DelimitedText = DataStr;
	GetFieldValue(FieldValueList);
	delete FieldValueList;
}  //CSGOrderRecord::GetFieldValue()

//----------------------------- CSGHoldPosition -----------------------------
//---------------------------------------------------------------------------
CSGHoldPosition::CSGHoldPosition(const UnicodeString& DataStr)
:CSunGardBaseObject(sgQueryClientHoldPosition)
{
	GetFieldValue(DataStr);
}  //CSGHoldPosition::CSGHoldPosition()

//---------------------------------------------------------------------------
CSGHoldPosition::CSGHoldPosition(TStringList *FieldValueList)
:CSunGardBaseObject(sgQueryClientHoldPosition)
{
	GetFieldValue(FieldValueList);
}  //CSGHoldPosition::CSGHoldPosition()

//---------------------------------------------------------------------------
void CSGHoldPosition::GetFieldValue(TStringList *FieldValueList)
{   //  0|1|00800187|1|CFFEX|IF1203|1.0|0|0.0|0.0|8|20281.6|2535.2|
	FAccount = L"";
	FExchangeCode = L"";
	FSymbol = L"";
	FPriceMultiplier = 1.0;
	FBuyQuantity = 0;
	FBuyAmount = 0.0;
	FBuyAveragePrice = 0.0;
	FSellQuantity = 0;
	FSellAmount = 0.0;
	FSellAveragePrice = 0.0;
	int FieldCount = FieldValueList->Count;
	if (FieldCount >= 3) FAccount = (*FieldValueList)[2];
	if (FieldCount >= 5) FExchangeCode = (*FieldValueList)[4];
	if (FieldCount >= 6) FSymbol = (*FieldValueList)[5];
	if (FieldCount >= 7) FPriceMultiplier = (*FieldValueList)[6].ToDouble();
	if (FieldCount >= 8) FBuyQuantity = (*FieldValueList)[7].ToInt();
	if (FBuyQuantity > 0)
	{
		if (FieldCount >= 9) FBuyAmount = (*FieldValueList)[8].ToDouble();
		if (FieldCount >= 10) FBuyAveragePrice = (*FieldValueList)[9].ToDouble();
	}

	if (FieldCount >= 11) FSellQuantity = (*FieldValueList)[10].ToInt();
	if (FSellQuantity > 0)
	{
		if (FieldCount >= 12) FSellAmount = (*FieldValueList)[11].ToDouble();
		if (FieldCount >= 13) FSellAveragePrice = (*FieldValueList)[12].ToDouble();
	}
}  //CSGHoldPosition::GetFieldValue()

//---------------------------------------------------------------------------
void CSGHoldPosition::GetFieldValue(const UnicodeString& DataStr)
{
	TStringList *FieldValueList = new TStringList();
	FieldValueList->Delimiter = FFieldDelimiter;
	FieldValueList->DelimitedText = DataStr;
	GetFieldValue(FieldValueList);
	delete FieldValueList;
}  //CSGHoldPosition::GetFieldValue()

//------------------------------- CSGAccount --------------------------------
//---------------------------------------------------------------------------
CSGAccount::CSGAccount(const UnicodeString& Account)
:FAccount(Account),
 FFundPtr(0),
 FIsFundSuccess(false),
 FFundErrorMsg(L""),
 FIsFillDetailSuccess(false),
 FFillDetailErrorMsg(L""),
 FIsPositionSuccess(false),
 FPositionErrorMsg(L""),
 FIsOrderRecordSuccess(false),
 FOrderRecordErrorMsg(L""),
 FIsSettlementSuccess(false),
 FSettlementErrorMsg(L"")
{
}  //CSGAccount::CSGAccount()
//---------------------------------------------------------------------------
CSGAccount::~CSGAccount()
{
	ClearFund();
	ClearFillDetail();
	ClearPosition();
	ClearOrderRecord();
}  //CSGAccount::~CSGAccount()
//---------------------------------------------------------------------------
void CSGAccount::ClearFund()
{
	if (FFundPtr != 0) delete FFundPtr;
	FFundPtr = 0;
}  //CSGAccount::ClearFund()
//---------------------------------------------------------------------------
CSGFillDetail *CSGAccount::GetFillDetailByIndex(int Index)
{
	CSGFillDetail *FillDetailPtr = 0;
	int FillDetailCount = FFillDetailArray.size();
	if ((Index < 0) || (Index >= FillDetailCount)) return FillDetailPtr;
	FillDetailPtr = FFillDetailArray[Index];
	return FillDetailPtr;
}  //CSGAccount::GetFillDetailByIndex()
//---------------------------------------------------------------------------
void CSGAccount::AddFillDetail(CSGFillDetail *FillDetailPtr)
{
	if (FillDetailPtr == 0) return;
	FFillDetailArray.push_back(FillDetailPtr);
}  //CSGAccount::AddFillDetail()

//---------------------------------------------------------------------------
void CSGAccount::ClearFillDetail()
{
	for (unsigned int i = 0; i < FFillDetailArray.size(); i++)
	{
		CSGFillDetail *FillDetailPtr = FFillDetailArray[i];
		delete FillDetailPtr;
	}
	FFillDetailArray.clear();
}  //CSGAccount::ClearFillDetail()
//---------------------------------------------------------------------------
CSGClientPosition *CSGAccount::GetPositionByIndex(int Index)
{
	CSGClientPosition *PositionPtr = 0;
	int PositionCount = FPositionArray.size();
	if ((Index < 0) || (Index >= PositionCount)) return PositionPtr;
	PositionPtr = FPositionArray[Index];
	return PositionPtr;
}  //CSGAccount::GetPositionByIndex()
//---------------------------------------------------------------------------
void CSGAccount::AddPosition(CSGClientPosition *PositionPtr)
{
	if (PositionPtr == 0) return;
	FPositionArray.push_back(PositionPtr);
}  //CSGAccount::AddPosition()
//---------------------------------------------------------------------------
void CSGAccount::ClearPosition()
{
	for (unsigned int i = 0; i < FPositionArray.size(); i++)
	{
		CSGClientPosition *PositionPtr = FPositionArray[i];
		delete PositionPtr;
	}
	FPositionArray.clear();
}  //CSGAccount::ClearPosition()
//---------------------------------------------------------------------------
CSGOrderRecord *CSGAccount::GetOrderRecordByIndex(int Index)
{
	CSGOrderRecord *OrderRecordPtr = 0;
	int OrderRecordCount = FOrderRecordArray.size();
	if ((Index < 0) || (Index >= OrderRecordCount)) return OrderRecordPtr;
	OrderRecordPtr = FOrderRecordArray[Index];
	return OrderRecordPtr;
}  //CSGAccount::GetOrderRecordByIndex()
//---------------------------------------------------------------------------
void CSGAccount::AddOrderRecord(CSGOrderRecord *OrderRecordPtr)
{
	if (OrderRecordPtr == 0) return;
	FOrderRecordArray.push_back(OrderRecordPtr);
}  //CSGAccount::AddOrderRecord()
//---------------------------------------------------------------------------
void CSGAccount::ClearOrderRecord()
{
	for (unsigned int i = 0; i < FOrderRecordArray.size(); i++)
	{
		CSGOrderRecord *OrderRecordPtr = FOrderRecordArray[i];
		delete OrderRecordPtr;
	}
	FOrderRecordArray.clear();
}  //CSGAccount::ClearOrderRecord()
//---------------------------------------------------------------------------
UnicodeString *CSGAccount::GetSettlementByIndex(int Index)
{
	UnicodeString *SettlementPtr = 0;
	int SettlementCount = FSettlementArray.size();
	if ((Index < 0) || (Index >= SettlementCount)) return SettlementPtr;
	SettlementPtr = FSettlementArray[Index];
	return SettlementPtr;
}  //CSGAccount::GetSettlementByIndex()
//---------------------------------------------------------------------------
void CSGAccount::AddSettlement(UnicodeString *SettlementPtr)
{
	if (SettlementPtr == 0) return;
	FSettlementArray.push_back(SettlementPtr);
}  //CSGAccount::AddSettlement()
//---------------------------------------------------------------------------
void CSGAccount::ClearSettlement()
{
	for (unsigned int i = 0; i < FSettlementArray.size(); i++)
	{
		UnicodeString *SettlementPtr = FSettlementArray[i];
		delete SettlementPtr;
	}
	FSettlementArray.clear();
}  //CSGAccount::ClearSettlement()
//---------------------------------------------------------------------------
CSGFillStatistic *CSGAccount::FindFillStatistic(const UnicodeString& FillStatisticKey)
{
	CSGFillStatistic *FillStatisticPtr = 0;
	if ((FillStatisticKey.Length()  <= 0) || (FFillStatisticIndex.size() <= 0))
		return FillStatisticPtr;

	std::map<UnicodeString, CSGFillStatistic*>::iterator it = FFillStatisticIndex.find(FillStatisticKey);
	if (it != FFillStatisticIndex.end()) FillStatisticPtr = it->second;

	return FillStatisticPtr;
}  //CSGAccount::FindFillStatistic()
//---------------------------------------------------------------------------
bool CSGAccount::InsertFillStatisticIndex(const UnicodeString& FillStatisticKey, CSGFillStatistic *FillStatisticPtr)
{
	bool IsSuccess = false;
	if ((FillStatisticKey.Length() <= 0) || (FillStatisticPtr == 0)) return IsSuccess;

	std::pair<std::map<UnicodeString, CSGFillStatistic*>::iterator, bool> InsertPair;
	InsertPair = FFillStatisticIndex.insert(std::map<UnicodeString, CSGFillStatistic*>::value_type(FillStatisticKey, FillStatisticPtr));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //CSGAccount::InsertFillStatisticIndex()
//---------------------------------------------------------------------------
CSGFillStatistic *CSGAccount::GetFillStatisticByIndex(int Index)
{
	CSGFillStatistic *FillStatisticPtr = 0;
	int FillStatisticCount = FFillStatisticArray.size();
	if ((Index < 0) || (Index >= FillStatisticCount)) return FillStatisticPtr;
	FillStatisticPtr = FFillStatisticArray[Index];
	return FillStatisticPtr;
}  //CSGAccount::GetFillStatisticByIndex()
//---------------------------------------------------------------------------
void CSGAccount::AddFillStatistic(CSGFillStatistic *FillStatisticPtr)
{
	if (FillStatisticPtr == 0) return;
	FFillStatisticArray.push_back(FillStatisticPtr);
}  //CSGAccount::AddFillStatistic()
//---------------------------------------------------------------------------
void CSGAccount::ClearFillStatistic()
{
	for (unsigned int i = 0; i < FFillStatisticArray.size(); i++)
	{
		CSGFillStatistic *FillStatisticPtr = FFillStatisticArray[i];
		delete FillStatisticPtr;
	}
	FFillStatisticArray.clear();
	FFillStatisticIndex.clear();
}  //CSGAccount::ClearFillStatistic()

