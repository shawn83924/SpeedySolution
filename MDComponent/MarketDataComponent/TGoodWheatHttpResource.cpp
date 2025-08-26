//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include <DateUtils.hpp>
#include "TGoodWheatHttpResource.h"
#include "TradingObjects.h"
#include "TradingObjectPool.h"

#pragma package(smart_init)

//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TGoodWheatHttpResource *)
{
	new TGoodWheatHttpResource(NULL);
}

//------------------------------------------------------------------------------
UnicodeString CheckGWCommonErrorMessage(UnicodeString UserKey, UnicodeString AccountNo, UnicodeString QryEnvFlag, UnicodeString ErrorField)
{
	UnicodeString ErrorMessage = L"";
	if (UserKey.Length() == 1)
	{
		if (UserKey == L"0")
		{
			if ((ErrorField.Length() == 1) && (ErrorField == L"Y"))
				ErrorMessage = L"5.	查詢中發生錯誤";
			else
				ErrorMessage = L"User Key 不正確";
		}
		else if (UserKey == L"2")
			ErrorMessage = L"未帶公司別參數，查無客戶基本資料";
	}
	else if (UserKey.Length() > 1)
	{
		if ((QryEnvFlag.Length() == 1) && (QryEnvFlag == L"N"))
			ErrorMessage = L"交易查詢暫時關閉";
//		else if (AccountNo.Length() <= 0)
//			ErrorMessage = L"未帶帳號參數或非可查詢的帳號";
	}
	return ErrorMessage;
}  //CheckCommonErrorMessage()

//------------------------------------------------------------------------------
String CombineSymbolCode(String CommodityPrefix, double StrikePrice, String YearMonth, String CallPut)
{
	String result = L"";
	if ((CommodityPrefix.Length() < 3) || (YearMonth.Length() != 6)) return result;

	String priceString = L"";
	String convertCommodityPrefix = CommodityPrefix;
	if ((CallPut == L"C") || (CallPut == L"P"))
	{
		if (StrikePrice <= 0.0) return result;
		priceString.printf(L"%05.0lf", StrikePrice);
	}
	else
	{
		String exchangeCode = L"TAIFEX";
		s888::CTradingCommodity *commodiytPtr = s888::gTradingObjsPool.FindCommodityFromBrokerCodeIndex(exchangeCode, CommodityPrefix, s888::rwLockForRead);
		if (commodiytPtr != 0)
			convertCommodityPrefix = commodiytPtr->GetBrokerContractCode(s888::rwLockForRead);
	}

	String yearCode = YearMonth.SubString(4, 1);
	String monthCode = YearMonth.SubString(5, 2);
	int month = monthCode.ToInt();
	if (CallPut == L"P")
	{
		switch(month)
		{
			case  1: monthCode = L"M"; break;
			case  2: monthCode = L"N"; break;
			case  3: monthCode = L"O"; break;
			case  4: monthCode = L"P"; break;
			case  5: monthCode = L"Q"; break;
			case  6: monthCode = L"R"; break;
			case  7: monthCode = L"S"; break;
			case  8: monthCode = L"T"; break;
			case  9: monthCode = L"U"; break;
			case 10: monthCode = L"V"; break;
			case 11: monthCode = L"W"; break;
			case 12: monthCode = L"X"; break;
			default: return result;
		}  //switch(Month)
	}
	else
	{
		switch(month)
		{
			case  1: monthCode = L"A"; break;
			case  2: monthCode = L"B"; break;
			case  3: monthCode = L"C"; break;
			case  4: monthCode = L"D"; break;
			case  5: monthCode = L"E"; break;
			case  6: monthCode = L"F"; break;
			case  7: monthCode = L"G"; break;
			case  8: monthCode = L"H"; break;
			case  9: monthCode = L"I"; break;
			case 10: monthCode = L"J"; break;
			case 11: monthCode = L"K"; break;
			case 12: monthCode = L"L"; break;
			default: return result;
		}  //switch(month)
	}  //if (CallPut == L"P")
	result = convertCommodityPrefix + priceString + monthCode + yearCode;
	return result;
}  //CombineSymbolCode()

//------------------------  TGoodWheatAccountMessage  --------------------------
//------------------------------------------------------------------------------
TGoodWheatAccountMessage::TGoodWheatAccountMessage()
:FUserKey(L""),
 FAccountNo(L""),
 FAccountName(L""),
 FBrokerNo(L""),
 FActIp(L""),
 FAeNo(L""),
 FIbNo(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
}  //TGoodWheatAccountMessage::TGoodWheatAccountMessage()

//------------------------------------------------------------------------------
TGoodWheatAccountMessage::TGoodWheatAccountMessage(TXMLDocument* DataXML)
:FUserKey(L""),
 FAccountNo(L""),
 FAccountName(L""),
 FBrokerNo(L""),
 FActIp(L""),
 FAeNo(L""),
 FIbNo(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
	GetDataFromXML(DataXML);
}  //TGoodWheatAccountMessage::TGoodWheatAccountMessage()

//------------------------------------------------------------------------------
void TGoodWheatAccountMessage::GetDataFromXML(TXMLDocument* DataXML)
{
	if (DataXML == 0) return;

	_di_IXMLNode RootNode = DataXML->DocumentElement;
	_di_IXMLNode DataNode = RootNode->ChildNodes->First();
	UnicodeString NodeName;
	while (DataNode != 0)
	{
		if (DataNode->HasChildNodes && DataNode->IsTextElement)
		{
			NodeName = DataNode->GetNodeName();
			if (NodeName == L"UK")
				FUserKey = DataNode->GetText();
			else if (NodeName == L"actno")
				FAccountNo = DataNode->GetText();
			else if (NodeName == L"actname")
				FAccountName = DataNode->GetText();
			else if (NodeName == L"company")
				FBrokerNo = DataNode->GetText();
			else if (NodeName == L"actip")
				FActIp = DataNode->GetText();
			else if (NodeName == L"aeno")
				FAeNo = DataNode->GetText();
			else if (NodeName == L"ibno")
				FIbNo = DataNode->GetText();
			else if (NodeName == L"QryEnvFlag")
				FQueryEnvFlag = DataNode->GetText();
			else if (NodeName == L"ERROR")
				FErrorFlag = DataNode->GetText();
		}  //if (!DataNode->HasChildNodes || !DataNode->IsTextElement)
		DataNode = DataNode->NextSibling();
	}  //while (DataNode != 0)
}  //TGoodWheatAccountMessage::GetDataFromXML()

//-------------------------  TGoodWheatOrderMessage  ---------------------------
//------------------------------------------------------------------------------
TGoodWheatOrderMessage::TGoodWheatOrderMessage()
:FCanCancel(false),
 FCanRelpace(false),
 FOrderID(L""),
 FLinesNo(1),
 FOrderType(L""),
 FSide(L""),
 FCommidityNo(L""),
 FYearMonth(L""),
 FQty(0),
 FPrice(0.0),
 FOrderTime(L""),
 FTradeQty(0),
 FCallPut(L"N"),
 FStrikePrice(0),
 FOrderStatusCode(L""),
 FCloseTrade(L"N"),
 FOrderStatusDescription(L""),
 FEntityType(L""),
 FIsSpread(false),
 FDayTradeOver(L"")
{
}  //TGoodWheatOrderMessage::TGoodWheatOrderMessage()

//------------------------------------------------------------------------------
TGoodWheatOrderMessage::TGoodWheatOrderMessage(_di_IXMLNode DataNode)
:FCanCancel(false),
 FCanRelpace(false),
 FOrderID(L""),
 FLinesNo(1),
 FOrderType(L""),
 FSide(L""),
 FCommidityNo(L""),
 FYearMonth(L""),
 FQty(0),
 FPrice(0.0),
 FOrderTime(L""),
 FTradeQty(0),
 FCallPut(L"N"),
 FStrikePrice(0),
 FOrderStatusCode(L""),
 FCloseTrade(L"N"),
 FOrderStatusDescription(L""),
 FEntityType(L""),
 FIsSpread(false),
 FDayTradeOver(L"")
{
	GetDataFromXML(DataNode);
}  //TGoodWheatOrderMessage::TGoodWheatOrderMessage()

//------------------------------------------------------------------------------
void TGoodWheatOrderMessage::GetDataFromXML(_di_IXMLNode DataNode)
{
	if ((DataNode == 0) || (!DataNode->HasChildNodes)) return;

	_di_IXMLNode FieldNode = DataNode->ChildNodes->First();
	UnicodeString NodeName;
	while (FieldNode != 0)
	{
		if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
		{
			NodeName = FieldNode->GetNodeName();
			UnicodeString TempText;
			if (NodeName == L"cancel")
			{
				TempText = FieldNode->GetText();
				if ((TempText.Length() > 0) && (TempText == L"刪單"))
					FCanCancel = true;
				else
					FCanCancel = false;
			}
			else if (NodeName == L"modify")
			{
				TempText = FieldNode->GetText();
				if ((TempText.Length() > 0) && (TempText == L"改量"))
					FCanRelpace = true;
				else
					FCanRelpace = false;
			}
			else if (NodeName == L"ordno")
				FOrderID = FieldNode->GetText();
			else if (NodeName == L"linesno")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FLinesNo = TempText.ToInt();
				else
					FLinesNo = 0;
			}
			else if (NodeName == L"ordtype")
				FOrderType = FieldNode->GetText();
			else if (NodeName == L"ps")
				FSide = FieldNode->GetText();
			else if (NodeName == L"comno")
				FCommidityNo = FieldNode->GetText();
			else if (NodeName == L"comym")
				FYearMonth = FieldNode->GetText();
			else if (NodeName == L"qty")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FQty = TempText.ToInt();
				else
					FQty = 0;
			}
			else if (NodeName == L"utprice")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FPrice = TempText.ToDouble();
				else
					FPrice = 0.0;
			}
			else if (NodeName == L"ordtime")
				FOrderTime = FieldNode->GetText();
			else if (NodeName == L"trdqty")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FTradeQty = TempText.ToInt();
				else
					FTradeQty = 0;
			}
			else if (NodeName == L"cp")
				FCallPut = FieldNode->GetText();
			else if (NodeName == L"stkprice")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FStrikePrice = TempText.ToDouble();
				else
					FStrikePrice = 0.0;
			}
			else if (NodeName == L"code")
				FOrderStatusCode = FieldNode->GetText();
			else if (NodeName == L"closetrd")
				FCloseTrade = FieldNode->GetText();
			else if (NodeName == L"status")
				FOrderStatusDescription = FieldNode->GetText();
			else if (NodeName == L"enttype")
				FEntityType = FieldNode->GetText();
			else if (NodeName == L"spread")
			{
				TempText = FieldNode->GetText();
				if ((TempText.Length() > 0) && (TempText == L"Y"))
					FIsSpread = true;
				else
					FIsSpread = false;
			}
			else if (NodeName == L"dtover")
				FDayTradeOver = FieldNode->GetText();
		}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)

		FieldNode = FieldNode->NextSibling();
	}  //while (FieldNode != 0)
}  //TGoodWheatOrderMessage::GetDataFromXML()

//----------------------  TGoodWheatDailyOrderMessage  -------------------------
//------------------------------------------------------------------------------
TGoodWheatDailyOrderMessage::TGoodWheatDailyOrderMessage()
:FMessageArriveTime(L""),
 FUserKey(L""),
 FBuyDescription(L""),
 FSellDescription(L""),
 FNewDescription(L""),
 FOffsetDescription(L""),
 FMarketDescription(L""),
 FLimitDescription(L""),
 FCancelDescription(L""),
 FReplaceDescription(L""),
 FInquireDescription(L""),
 FAccountNo(L""),
 FAccountName(L""),
 FBrokerNo(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
}  //TGoodWheatDailyOrderMessage::TGoodWheatDailyOrderMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyOrderMessage::TGoodWheatDailyOrderMessage(TXMLDocument* DataXML)
:FMessageArriveTime(L""),
 FUserKey(L""),
 FBuyDescription(L""),
 FSellDescription(L""),
 FNewDescription(L""),
 FOffsetDescription(L""),
 FMarketDescription(L""),
 FLimitDescription(L""),
 FCancelDescription(L""),
 FReplaceDescription(L""),
 FInquireDescription(L""),
 FAccountNo(L""),
 FAccountName(L""),
 FBrokerNo(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
	GetDataFromXML(DataXML);
}  //TGoodWheatDailyOrderMessage::TGoodWheatDailyOrderMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyOrderMessage::~TGoodWheatDailyOrderMessage()
{
	ClearData();
}  //TGoodWheatDailyOrderMessage::~TGoodWheatDailyOrderMessage()

//------------------------------------------------------------------------------
void TGoodWheatDailyOrderMessage::ClearData()
{
	FMessageArriveTime = L"";
	FUserKey = L"";
	FBuyDescription = L"";
	FSellDescription = L"";
	FNewDescription = L"";
	FOffsetDescription = L"";
	FMarketDescription = L"";
	FLimitDescription = L"";
	FCancelDescription = L"";
	FReplaceDescription = L"";
	FInquireDescription = L"";
	FAccountNo = L"";
	FAccountName = L"";
	FBrokerNo = L"";
	FQueryEnvFlag = L"";
	FErrorFlag = L"";
	TGoodWheatOrderMessage* OrderPtr;
	for (unsigned int i = 0; i < FOrderArray.size(); i++)
	{
		OrderPtr = FOrderArray[i];
		delete OrderPtr;
	}  //for (int i = 0; i < FOrderArray.size(); i++)

	FOrderArray.clear();
}  //TGoodWheatDailyOrderMessage::ClearData()

//------------------------------------------------------------------------------
void TGoodWheatDailyOrderMessage::GetDataFromXML(TXMLDocument* DataXML)
{
	if (DataXML == 0) return;

	ClearData();
	TDateTime MsgTime = Now();
	FMessageArriveTime.printf(L"%02d:%02d:%02d.%03d", HourOf(MsgTime), MinuteOf(MsgTime), SecondOf(MsgTime), MilliSecondOf(MsgTime));
	_di_IXMLNode RootNode = DataXML->DocumentElement;
	_di_IXMLNode DataNode = RootNode->ChildNodes->First();
	UnicodeString NodeName;
	while (DataNode != 0)
	{
		if (DataNode->HasChildNodes)
		{
			NodeName = DataNode->GetNodeName();
			if (DataNode->IsTextElement)
			{
				if (NodeName == L"UK")
					FUserKey = DataNode->GetText();
				else if (NodeName == L"buyField")
					FBuyDescription = DataNode->GetText();
				else if (NodeName == L"sellField")
					FSellDescription = DataNode->GetText();
				else if (NodeName == L"newField")
					FNewDescription = DataNode->GetText();
				else if (NodeName == L"offsetField")
					FOffsetDescription = DataNode->GetText();
				else if (NodeName == L"MKTField")
					FMarketDescription = DataNode->GetText();
				else if (NodeName == L"LMTField")
					FLimitDescription = DataNode->GetText();
				else if (NodeName == L"CXLField")
					FCancelDescription = DataNode->GetText();
				else if (NodeName == L"UPDField")
					FReplaceDescription = DataNode->GetText();
				else if (NodeName == L"INQField")
					FInquireDescription = DataNode->GetText();
				else if (NodeName == L"actno")
					FAccountNo = DataNode->GetText();
				else if (NodeName == L"actname")
					FAccountName = DataNode->GetText();
				else if (NodeName == L"company")
					FBrokerNo = DataNode->GetText();
				else if (NodeName == L"QryEnvFlag")
					FQueryEnvFlag = DataNode->GetText();
				else if (NodeName == L"ERROR")
					FErrorFlag = DataNode->GetText();
			}
			else
			{
				if ((NodeName == L"fotord") && DataNode->HasChildNodes)
				{
					TGoodWheatOrderMessage* OrderPtr = new TGoodWheatOrderMessage(DataNode);
					FOrderArray.push_back(OrderPtr);
				}
			}  //if (DataNode->IsTextElement)
		}  //if (!DataNode->HasChildNodes)

		DataNode = DataNode->NextSibling();
	}  //while (DataNode != 0)
}  //TGoodWheatDailyOrderMessage::GetDataFromXML()

//-------------------------  TGoodWheatFillMessage  ----------------------------
//------------------------------------------------------------------------------
TGoodWheatFillMessage::TGoodWheatFillMessage()
:FOrderSequenceNo(0),
 FAccountNo(L""),
 FOrderID(L""),
 FAeNo(L""),
 FTradeDate(L""),
 FMatchQty(0),
 FMatchPrice(0.0),
 FSide(L""),
 FCommidityNo(L""),
 FYearMonth(L""),
 FTradeTime(L""),
 FMarketType(L""),
 FStrikePrice(0.0),
 FCallPut(L"N"),
 FDayTradeOver(L"")
{
}  //TGoodWheatFillMessage::TGoodWheatFillMessage()

//------------------------------------------------------------------------------
TGoodWheatFillMessage::TGoodWheatFillMessage(_di_IXMLNode DataNode)
:FOrderSequenceNo(0),
 FAccountNo(L""),
 FOrderID(L""),
 FAeNo(L""),
 FTradeDate(L""),
 FMatchQty(0),
 FMatchPrice(0.0),
 FSide(L""),
 FCommidityNo(L""),
 FYearMonth(L""),
 FTradeTime(L""),
 FMarketType(L""),
 FStrikePrice(0.0),
 FCallPut(L"N"),
 FDayTradeOver(L"")
{
	GetDataFromXML(DataNode);
}  //TGoodWheatFillMessage::TGoodWheatFillMessage()

//------------------------------------------------------------------------------
void TGoodWheatFillMessage::GetDataFromXML(_di_IXMLNode DataNode)
{
	if ((DataNode == 0) || (!DataNode->HasChildNodes)) return;

	_di_IXMLNode FieldNode = DataNode->ChildNodes->First();
	UnicodeString NodeName;
	UnicodeString TempText;
	while (FieldNode != 0)
	{
		if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
		{
			NodeName = FieldNode->GetNodeName();
			if (NodeName == L"order")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FOrderSequenceNo = TempText.ToInt();
				else
					FOrderSequenceNo = 0;
			}
			else if (NodeName == L"actno")
				FAccountNo = FieldNode->GetText();
			else if (NodeName == L"ordno")
				FOrderID = FieldNode->GetText();
			else if (NodeName == L"aeno")
				FAeNo = FieldNode->GetText();
			else if (NodeName == L"trddt")
				FTradeDate = FieldNode->GetText();
			else if (NodeName == L"qty")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMatchQty = TempText.ToInt();
				else
					FMatchQty = 0;
			}
			else if (NodeName == L"trdprc1")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMatchPrice = TempText.ToDouble();
				else
					FMatchPrice = 0.0;
			}
			else if (NodeName == L"ps")
				FSide = FieldNode->GetText();
			else if (NodeName == L"comno")
				FCommidityNo = FieldNode->GetText();
			else if (NodeName == L"comym")
				FYearMonth = FieldNode->GetText();
			else if (NodeName == L"trdtm")
				FTradeTime = FieldNode->GetText();
			else if (NodeName == L"comtype")
				FMarketType = FieldNode->GetText();
			else if (NodeName == L"stkprc")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FStrikePrice = TempText.ToDouble();
				else
					FStrikePrice = 0.0;
			}
			else if (NodeName == L"cp")
				FCallPut = FieldNode->GetText();
			else if (NodeName == L"dtover")
				FDayTradeOver = FieldNode->GetText();
		}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)

		FieldNode = FieldNode->NextSibling();
	}  //while (FieldNode != 0)
}  //TGoodWheatFillMessage::GetDataFromXML()

//----------------------  TGoodWheatDailyFillMessage  -------------------------
//------------------------------------------------------------------------------
TGoodWheatDailyFillMessage::TGoodWheatDailyFillMessage()
:FMessageArriveTime(L""),
 FUserKey(L""),
 FAccountNo(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
}  //TGoodWheatDailyFillMessage::TGoodWheatDailyFillMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyFillMessage::TGoodWheatDailyFillMessage(TXMLDocument* DataXML)
:FMessageArriveTime(L""),
 FUserKey(L""),
 FAccountNo(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
	GetDataFromXML(DataXML);
}  //TGoodWheatDailyFillMessage::TGoodWheatDailyFillMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyFillMessage::~TGoodWheatDailyFillMessage()
{
	ClearData();
}  //TGoodWheatDailyFillMessage::~TGoodWheatDailyFillMessage()

//------------------------------------------------------------------------------
void TGoodWheatDailyFillMessage::ClearData()
{
	FMessageArriveTime = L"";
	FUserKey = L"";
	FAccountNo = L"";
	FErrorFlag = L"";
	TGoodWheatFillMessage* FillPtr;
	for (unsigned int i = 0; i < FFillArray.size(); i++)
	{
		FillPtr = FFillArray[i];
		delete FillPtr;
	}  //for (int i = 0; i < FFillArray.size(); i++)

	FFillArray.clear();
}  //TGoodWheatDailyFillMessage::ClearData()

//------------------------------------------------------------------------------
void TGoodWheatDailyFillMessage::GetDataFromXML(TXMLDocument* DataXML)
{
	if (DataXML == 0) return;

	ClearData();
	TDateTime MsgTime = Now();
	FMessageArriveTime.printf(L"%02d:%02d:%02d.%03d", HourOf(MsgTime), MinuteOf(MsgTime), SecondOf(MsgTime), MilliSecondOf(MsgTime));
	_di_IXMLNode RootNode = DataXML->DocumentElement;
	_di_IXMLNode DataNode = RootNode->ChildNodes->First();
	UnicodeString NodeName;
	while (DataNode != 0)
	{
		if (DataNode->HasChildNodes)
		{
			NodeName = DataNode->GetNodeName();
			if (DataNode->IsTextElement)
			{
				if (NodeName == L"UK")
					FUserKey = DataNode->GetText();
				else if (NodeName == L"actno")
					FAccountNo = DataNode->GetText();
				else if (NodeName == L"QryEnvFlag")
					FQueryEnvFlag = DataNode->GetText();
				else if (NodeName == L"ERROR")
					FErrorFlag = DataNode->GetText();
			}
			else
			{
				if ((NodeName == L"fbtdtr") && DataNode->HasChildNodes)
				{
					TGoodWheatFillMessage* FillPtr = new TGoodWheatFillMessage(DataNode);
					FFillArray.push_back(FillPtr);
				}
			}  //if (DataNode->IsTextElement)
		}  //if (!DataNode->HasChildNodes)

		DataNode = DataNode->NextSibling();
	}  //while (DataNode != 0)
}  //TGoodWheatDailyFillMessage::GetDataFromXML()

//------------------------  TGoodWheatDayTradeMessage  -------------------------
//------------------------------------------------------------------------------
TGoodWheatDayTradeMessage::TGoodWheatDayTradeMessage()
:FOrderSequenceNo(0),
 FOrderID(L""),
 FCommidityNo(L""),
 FYearMonth(L""),
 FStrikePrice(0.0),
 FCallPut(L""),
 FSide(L""),
 FExchange(L""),
 FTradeDate(L""),
 FMatchPrice(0.0),
 FMatchQty(0),
 FPrtlos(0.0),
 FCurrency(L"")

{
}  //TGoodWheatDayTradeMessage::TGoodWheatDayTradeMessage()

//------------------------------------------------------------------------------
TGoodWheatDayTradeMessage::TGoodWheatDayTradeMessage(_di_IXMLNode DataNode)
:FOrderSequenceNo(0),
 FOrderID(L""),
 FCommidityNo(L""),
 FYearMonth(L""),
 FStrikePrice(0.0),
 FCallPut(L""),
 FSide(L""),
 FExchange(L""),
 FTradeDate(L""),
 FMatchPrice(0.0),
 FMatchQty(0),
 FPrtlos(0.0),
 FCurrency(L"")
{
	GetDataFromXML(DataNode);
}  //TGoodWheatDayTradeMessage::TGoodWheatDayTradeMessage()

//------------------------------------------------------------------------------
void TGoodWheatDayTradeMessage::GetDataFromXML(_di_IXMLNode DataNode)
{
	if ((DataNode == 0) || (!DataNode->HasChildNodes)) return;

	_di_IXMLNode FieldNode = DataNode->ChildNodes->First();
	UnicodeString NodeName;
	UnicodeString TempText;
	while (FieldNode != 0)
	{
		NodeName = FieldNode->GetNodeName();
		if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
		{
			if (NodeName == L"sn")
			{
				TempText = FieldNode->GetText();
				FOrderSequenceNo = TempText.ToInt();
			}
			else if (NodeName == L"ordno")
				FOrderID = FieldNode->GetText();
			else if (NodeName == L"comno")
				FCommidityNo = FieldNode->GetText();
			else if (NodeName == L"comym")
				FYearMonth = FieldNode->GetText();
			else if (NodeName == L"stkprc")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FStrikePrice = TempText.ToDouble();
				else
					FStrikePrice = 0.0;
			}
			else if (NodeName == L"callput")
				FCallPut = FieldNode->GetText();
			else if (NodeName == L"ps")
				FSide = FieldNode->GetText();
			else if (NodeName == L"exh")
				FExchange = FieldNode->GetText();
			else if (NodeName == L"fndt")
				FTradeDate = FieldNode->GetText();
			else if (NodeName == L"trdprc1")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMatchPrice = TempText.ToDouble();
				else
					FMatchPrice = 0.0;
			}
			else if (NodeName == L"qty")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMatchQty = TempText.ToInt();
				else
					FMatchQty = 0;
			}
			else if (NodeName == L"prtlos")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FPrtlos = TempText.ToDouble();
				else
					FPrtlos = 0.0;
			}
			else if (NodeName == L"curr")
				FCurrency = FieldNode->GetText();
		}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)

		FieldNode = FieldNode->NextSibling();
	}  //while (FieldNode != 0)
}  //TGoodWheatDayTradeMessage::GetDataFromXML()

//---------------------  TGoodWheatDailyDayTradeMessage  -----------------------
//------------------------------------------------------------------------------
TGoodWheatDailyDayTradeMessage::TGoodWheatDailyDayTradeMessage()
:FMessageArriveTime(L""),
 FUserKey(L""),
 FBrokerNo(L""),
 FAccountNo(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
}  //TGoodWheatDailyDayTradeMessage::TGoodWheatDailyDayTradeMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyDayTradeMessage::TGoodWheatDailyDayTradeMessage(TXMLDocument* DataXML)
:FMessageArriveTime(L""),
 FUserKey(L""),
 FBrokerNo(L""),
 FAccountNo(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
	GetDataFromXML(DataXML);
}  //TGoodWheatDailyDayTradeMessage::TGoodWheatDailyDayTradeMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyDayTradeMessage::~TGoodWheatDailyDayTradeMessage()
{
	ClearData();
}  //TGoodWheatDailyDayTradeMessage::~TGoodWheatDailyDayTradeMessage()

//------------------------------------------------------------------------------
void TGoodWheatDailyDayTradeMessage::ClearData()
{
	FMessageArriveTime = L"";
	FUserKey = L"";
	FBrokerNo = L"";
	FAccountNo = L"";
	FQueryEnvFlag = L"";
	FErrorFlag = L"";
	TGoodWheatDayTradeMessage* DayTradePtr;
	for (unsigned int i = 0; i < FDayTradeArray.size(); i++)
	{
		DayTradePtr = FDayTradeArray[i];
		delete DayTradePtr;
	}  //for (int i = 0; i < FDayTradeArray.size(); i++)

	FDayTradeArray.clear();
}  //TGoodWheatDailyDayTradeMessage::ClearData()

//------------------------------------------------------------------------------
void TGoodWheatDailyDayTradeMessage::GetDataFromXML(TXMLDocument* DataXML)
{
	if (DataXML == 0) return;

	ClearData();
	TDateTime MsgTime = Now();
	FMessageArriveTime.printf(L"%02d:%02d:%02d.%03d", HourOf(MsgTime), MinuteOf(MsgTime), SecondOf(MsgTime), MilliSecondOf(MsgTime));
	_di_IXMLNode RootNode = DataXML->DocumentElement;
	_di_IXMLNode DataNode = RootNode->ChildNodes->First();
	UnicodeString NodeName;
	while (DataNode != 0)
	{
		if (DataNode->HasChildNodes)
		{
			NodeName = DataNode->GetNodeName();
			if (DataNode->IsTextElement)
			{
				if (NodeName == L"UK")
					FUserKey = DataNode->GetText();
				else if (NodeName == L"company")
					FBrokerNo = DataNode->GetText();
				else if (NodeName == L"actno")
					FAccountNo = DataNode->GetText();
				else if (NodeName == L"QryEnvFlag")
					FQueryEnvFlag = DataNode->GetText();
				else if (NodeName == L"ERROR")
					FErrorFlag = DataNode->GetText();
			}
			else
			{
				if ((NodeName == L"fbwost") && DataNode->HasChildNodes)
				{
					TGoodWheatDayTradeMessage* DayTradePtr = new TGoodWheatDayTradeMessage(DataNode);
					FDayTradeArray.push_back(DayTradePtr);
				}
			}  //if (DataNode->IsTextElement)
		}  //if (!DataNode->HasChildNodes)

		DataNode = DataNode->NextSibling();
	}  //while (DataNode != 0)
}  //TGoodWheatDailyDayTradeMessage::GetDataFromXML()

//----------------------  TGoodWheatOpenInterestMessage  -----------------------
//------------------------------------------------------------------------------
TGoodWheatOpenInterestMessage::TGoodWheatOpenInterestMessage()
:FOrderSequenceNo(0),
 FOrderID(L""),
 FAccountNo(L""),
 FTradeDate(L""),
 FSide(L""),
 FCommidityNo(L""),
 FYearMonth(L""),
 FMatchPrice(0.0),
 FOpenInterest(0),
 FMarketPrice(0.0),
 FPrtlos(0.0),
 FMarketType(L""),
 FStrikePrice(0.0),
 FCallPut(L"N"),
 FIsSpread(false),
 FDayTradeOver(L"")
{
}  //TGoodWheatOpenInterestMessage::TGoodWheatOpenInterestMessage()

//------------------------------------------------------------------------------
TGoodWheatOpenInterestMessage::TGoodWheatOpenInterestMessage(_di_IXMLNode DataNode)
:FOrderSequenceNo(0),
 FOrderID(L""),
 FAccountNo(L""),
 FTradeDate(L""),
 FSide(L""),
 FCommidityNo(L""),
 FYearMonth(L""),
 FMatchPrice(0.0),
 FOpenInterest(0),
 FMarketPrice(0.0),
 FPrtlos(0.0),
 FMarketType(L""),
 FStrikePrice(0.0),
 FCallPut(L"N"),
 FIsSpread(false),
 FDayTradeOver(L"")
{
	GetDataFromXML(DataNode);
}  //TGoodWheatOpenInterestMessage::TGoodWheatOpenInterestMessage()

//------------------------------------------------------------------------------
void TGoodWheatOpenInterestMessage::GetDataFromXML(_di_IXMLNode DataNode)
{
	if ((DataNode == 0) || (!DataNode->HasChildNodes)) return;

	_di_IXMLNode FieldNode = DataNode->ChildNodes->First();
	UnicodeString NodeName;
	UnicodeString TempText;
	while (FieldNode != 0)
	{
		NodeName = FieldNode->GetNodeName();
		if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
		{
			if (NodeName == L"order")
			{
				TempText = FieldNode->GetText();
				FOrderSequenceNo = TempText.ToInt();
			}
			else if (NodeName == L"ordno")
				FOrderID = FieldNode->GetText();
			else if (NodeName == L"actno")
				FAccountNo = FieldNode->GetText();
			else if (NodeName == L"trddt")
				FTradeDate = FieldNode->GetText();
			else if (NodeName == L"ps")
				FSide = FieldNode->GetText();
			else if (NodeName == L"comno")
				FCommidityNo = FieldNode->GetText();
			else if (NodeName == L"comym")
				FYearMonth = FieldNode->GetText();
			else if (NodeName == L"trdprc1")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMatchPrice = TempText.ToDouble();
				else
					FMatchPrice = 0.0;
			}
			else if (NodeName == L"qty")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FOpenInterest = TempText.ToInt();
				else
					FOpenInterest = 0;
			}
			else if (NodeName == L"mktpre1")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMarketPrice = TempText.ToDouble();
				else
					FMarketPrice = 0.0;
			}
			else if (NodeName == L"prtlos")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FPrtlos = TempText.ToDouble();
				else
					FPrtlos = 0.0;
			}
			else if (NodeName == L"comtype")
				FMarketType = FieldNode->GetText();
			else if (NodeName == L"stkprc")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FStrikePrice = TempText.ToDouble();
				else
					FStrikePrice = 0.0;
			}
			else if (NodeName == L"callput")
				FCallPut = FieldNode->GetText();
			else if (NodeName == L"spread")
			{
				TempText = FieldNode->GetText();
				if ((TempText.Length() > 0) && (TempText == L"Y"))
					FIsSpread = true;
				else
					FIsSpread = false;
			}
			else if (NodeName == L"dtover")
				FDayTradeOver = FieldNode->GetText();
		}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)

		FieldNode = FieldNode->NextSibling();
	}  //while (FieldNode != 0)
}  //TGoodWheatOpenInterestMessage::GetDataFromXML()

//------------------------------------------------------------------------------
UnicodeString TGoodWheatOpenInterestMessage::GetSymbolCode()
{
	return CombineSymbolCode(FCommidityNo, FStrikePrice, FYearMonth, FCallPut);
}  //TGoodWheatOpenInterestMessage::GetSymbolCode()

//-------------------  TGoodWheatDailyOpenInterestMessage  ---------------------
//------------------------------------------------------------------------------
TGoodWheatDailyOpenInterestMessage::TGoodWheatDailyOpenInterestMessage()
:FMessageArriveTime(L""),
 FUserKey(L""),
 FAccountNo(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
}  //TGoodWheatDailyOpenInterestMessage::TGoodWheatDailyOpenInterestMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyOpenInterestMessage::TGoodWheatDailyOpenInterestMessage(TXMLDocument* DataXML)
:FMessageArriveTime(L""),
 FUserKey(L""),
 FAccountNo(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
	GetDataFromXML(DataXML);
}  //TGoodWheatDailyOpenInterestMessage::TGoodWheatDailyOpenInterestMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyOpenInterestMessage::~TGoodWheatDailyOpenInterestMessage()
{
	ClearData();
}  //TGoodWheatDailyOpenInterestMessage::~TGoodWheatDailyOpenInterestMessage()

//------------------------------------------------------------------------------
void TGoodWheatDailyOpenInterestMessage::ClearData()
{
	FMessageArriveTime = L"";
	FUserKey = L"";
	FAccountNo = L"";
	FQueryEnvFlag = L"";
	FErrorFlag = L"";
	TGoodWheatOpenInterestMessage* OpenInterestPtr;
	for (unsigned int i = 0; i < FOpenInterestArray.size(); i++)
	{
		OpenInterestPtr = FOpenInterestArray[i];
		delete OpenInterestPtr;
	}  //for (int i = 0; i < FOpenInterestArray.size(); i++)

	FOpenInterestArray.clear();
}  //TGoodWheatDailyOpenInterestMessage::ClearData()

//------------------------------------------------------------------------------
void TGoodWheatDailyOpenInterestMessage::GetDataFromXML(TXMLDocument* DataXML)
{
	if (DataXML == 0) return;

	ClearData();
	TDateTime MsgTime = Now();
	FMessageArriveTime.printf(L"%02d:%02d:%02d.%03d", HourOf(MsgTime), MinuteOf(MsgTime), SecondOf(MsgTime), MilliSecondOf(MsgTime));
	_di_IXMLNode RootNode = DataXML->DocumentElement;
	_di_IXMLNode DataNode = RootNode->ChildNodes->First();
	UnicodeString NodeName;
	while (DataNode != 0)
	{
		if (DataNode->HasChildNodes)
		{
			NodeName = DataNode->GetNodeName();
			if (DataNode->IsTextElement)
			{
				if (NodeName == L"UK")
					FUserKey = DataNode->GetText();
				else if (NodeName == L"caltime")
					FCalTime = DataNode->GetText();
				else if (NodeName == L"actno")
					FAccountNo = DataNode->GetText();
				else if (NodeName == L"QryEnvFlag")
					FQueryEnvFlag = DataNode->GetText();
				else if (NodeName == L"ERROR")
					FErrorFlag = DataNode->GetText();
			}
			else
			{
				if ((NodeName == L"fbtopd") && DataNode->HasChildNodes)
				{
					TGoodWheatOpenInterestMessage* OpenInterestPtr = new TGoodWheatOpenInterestMessage(DataNode);
					FOpenInterestArray.push_back(OpenInterestPtr);
				}
			}  //if (DataNode->IsTextElement)
		}  //if (!DataNode->HasChildNodes)

		DataNode = DataNode->NextSibling();
	}  //while (DataNode != 0)
}  //TGoodWheatDailyOpenInterestMessage::GetDataFromXML()

//------------------------  TGoodWheatPositionMessage  -------------------------
//------------------------------------------------------------------------------
TGoodWheatPositionMessage::TGoodWheatPositionMessage()
:FCommidityNo(L""),
 FYearMonth(L""),
 FCallPut(L""),
 FStrikePrice(0.0),
 FMarketPrice(0.0),
 FBSOptQty(0),
 FSSOptQty(0),
 FBORdQty(0),
 FSORdQty(0),
 FBTRdQty(0),
 FBTRdPrice(0.0),
 FSTRdQty(0),
 FSTRdPrice(0.0),
 FBOStQty(0),
 FSOStQty(0),
 FBCOptQty(0),
 FSCOptQty(0),
 FBCOptPrice(0.0),
 FSCOptPrice(0.0),
 FIsSpread(false),
 FCT(L""),
 FExchange(L""),
 FOSPrtlos(0.0),
 FCurrency(L"")
{
}  //TGoodWheatPositionMessage::TGoodWheatPositionMessage()

//------------------------------------------------------------------------------
TGoodWheatPositionMessage::TGoodWheatPositionMessage(_di_IXMLNode DataNode)
:FCommidityNo(L""),
 FYearMonth(L""),
 FCallPut(L""),
 FStrikePrice(0.0),
 FMarketPrice(0.0),
 FBSOptQty(0),
 FSSOptQty(0),
 FBORdQty(0),
 FSORdQty(0),
 FBTRdQty(0),
 FBTRdPrice(0.0),
 FSTRdQty(0),
 FSTRdPrice(0.0),
 FBOStQty(0),
 FSOStQty(0),
 FBCOptQty(0),
 FSCOptQty(0),
 FBCOptPrice(0.0),
 FSCOptPrice(0.0),
 FIsSpread(false),
 FCT(L""),
 FExchange(L""),
 FOSPrtlos(0.0),
 FCurrency(L"")
{
	GetDataFromXML(DataNode);
}  //TGoodWheatPositionMessage::TGoodWheatPositionMessage()

//------------------------------------------------------------------------------
void TGoodWheatPositionMessage::GetDataFromXML(_di_IXMLNode DataNode)
{
	if ((DataNode == 0) || (!DataNode->HasChildNodes)) return;

	_di_IXMLNode FieldNode = DataNode->ChildNodes->First();
	UnicodeString NodeName;
	UnicodeString TempText;
	while (FieldNode != 0)
	{
		NodeName = FieldNode->GetNodeName();
		if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
		{
			if (NodeName == L"COMNO")  //商品代號
				FCommidityNo = FieldNode->GetText();
			else if (NodeName == L"COMYM")  //商品年月
				FYearMonth = FieldNode->GetText();
			else if (NodeName == L"CALLPUT")  //CALL / PUT
				FCallPut = FieldNode->GetText();
			else if (NodeName == L"STKPRC")  //履約價
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FStrikePrice = TempText.ToDouble();
				else
					FStrikePrice = 0.0;
			}
			else if (NodeName == L"MKTPRC")  //計算市價
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMarketPrice = TempText.ToDouble();
				else
					FMarketPrice = 0.0;
			}
			else if (NodeName == L"BSOPTQTY")  //昨日留倉-買
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FBSOptQty = TempText.ToInt();
				else
					FBSOptQty = 0;
			}
			else if (NodeName == L"SSOPTQTY")  //昨日留倉-賣
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FSSOptQty = TempText.ToInt();
				else
					FSSOptQty = 0;
			}
			else if (NodeName == L"BORDQTY")  //本日委託-買
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FBORdQty = TempText.ToInt();
				else
					FBORdQty = 0;
			}
			else if (NodeName == L"SORDQTY")  //本日委託-賣
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FSORdQty = TempText.ToInt();
				else
					FSORdQty = 0;
			}
			else if (NodeName == L"BTRDQTY")  //目前成交-買
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FBTRdQty = TempText.ToInt();
				else
					FBTRdQty = 0;
			}
			else if (NodeName == L"BTRDPRC")  //成交均價-買
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FBTRdPrice = TempText.ToDouble();
				else
					FBTRdPrice = 0.0;
			}
			else if (NodeName == L"STRDQTY")  //目前成交-賣
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FSTRdQty = TempText.ToInt();
				else
					FSTRdQty = 0;
			}
			else if (NodeName == L"STRDPRC")  //成交均價-賣
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FSTRdPrice = TempText.ToDouble();
				else
					FSTRdPrice = 0.0;
			}
			else if (NodeName == L"BOSTQTY")  //本日了結-買
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FBOStQty = TempText.ToInt();
				else
					FBOStQty = 0;
			}
			else if (NodeName == L"SOSTQTY")  //本日了結-賣
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FSOStQty = TempText.ToInt();
				else
					FSOStQty = 0;
			}
			else if (NodeName == L"BCOPTQTY")  //目前留倉-買
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FBCOptQty = TempText.ToInt();
				else
					FBCOptQty = 0;
			}
			else if (NodeName == L"SCOPTQTY")  //目前留倉-賣
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FSCOptQty = TempText.ToInt();
				else
					FSCOptQty = 0;
			}
			else if (NodeName == L"BCOPTPRC")  //目前留倉之成交均價-買
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FBCOptPrice = TempText.ToDouble();
				else
					FBCOptPrice = 0.0;
			}
			else if (NodeName == L"SCOPTPRC")  //目前留倉之成交均價-賣
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FSCOptPrice = TempText.ToDouble();
				else
					FSCOptPrice = 0.0;
			}
			else if (NodeName == L"SPREAD")
			{
				TempText = FieldNode->GetText();
				if ((TempText.Length() > 0) && (TempText == L"Y"))
					FIsSpread = true;
				else
					FIsSpread = false;
			}
			else if (NodeName == L"CT")
				FCT = FieldNode->GetText();
			else if (NodeName == L"EXH")
				FExchange = FieldNode->GetText();
			else if (NodeName == L"OSPRTLOS")  //平倉損益
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FOSPrtlos = TempText.ToDouble();
				else
					FOSPrtlos = 0.0;
			}
			else if (NodeName == L"currency")
				FCurrency = FieldNode->GetText();
			else if (NodeName == L"BSOPTPRC")  //昨日留倉成交價合計--買
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FBSOptPrice = TempText.ToDouble();
				else
					FBSOptPrice = 0.0;
			}
			else if (NodeName == L"SSOPTPRC")  //昨日留倉成交價合計--賣
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FSSOptPrice = TempText.ToDouble();
				else
					FSSOptPrice = 0.0;
			}
		}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)

		FieldNode = FieldNode->NextSibling();
	}  //while (FieldNode != 0)
}  //TGoodWheatPositionMessage::GetDataFromXML()

//------------------------------------------------------------------------------
UnicodeString TGoodWheatPositionMessage::GetSymbolCode()
{
	return CombineSymbolCode(FCommidityNo, FStrikePrice, FYearMonth, FCallPut);
}  //TGoodWheatPositionMessage::GetSymbolCode()

//-------------------  TGoodWheatDailyPositionMessage  ---------------------
//------------------------------------------------------------------------------
TGoodWheatDailyPositionMessage::TGoodWheatDailyPositionMessage()
:FMessageArriveTime(L""),
 FUserKey(L""),
 FAccountNo(L""),
 FAccountName(L""),
 FBrokerNo(L""),
 FCalTime(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
}  //TGoodWheatDailyPositionMessage::TGoodWheatDailyPositionMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyPositionMessage::TGoodWheatDailyPositionMessage(TXMLDocument* DataXML)
:FMessageArriveTime(L""),
 FUserKey(L""),
 FAccountNo(L""),
 FAccountName(L""),
 FBrokerNo(L""),
 FCalTime(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
	GetDataFromXML(DataXML);
}  //TGoodWheatDailyPositionMessage::TGoodWheatDailyPositionMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyPositionMessage::~TGoodWheatDailyPositionMessage()
{
	ClearData();
}  //TGoodWheatDailyPositionMessage::~TGoodWheatDailyPositionMessage()

//------------------------------------------------------------------------------
void TGoodWheatDailyPositionMessage::ClearData()
{
	FMessageArriveTime = L"";
	FUserKey = L"";
	FAccountNo = L"";
	FAccountName = L"";
	FBrokerNo = L"";
	FCalTime = L"";
	FQueryEnvFlag = L"";
	FErrorFlag = L"";
	TGoodWheatPositionMessage* PositionPtr;
	for (unsigned int i = 0; i < FPositionArray.size(); i++)
	{
		PositionPtr = FPositionArray[i];
		delete PositionPtr;
	}  //for (int i = 0; i < FPositionArray.size(); i++)
	FPositionArray.clear();
}  //TGoodWheatDailyPositionMessage::ClearData()

//------------------------------------------------------------------------------
void TGoodWheatDailyPositionMessage::GetDataFromXML(TXMLDocument* DataXML)
{
	if (DataXML == 0) return;

	ClearData();
	TDateTime MsgTime = Now();
	FMessageArriveTime.printf(L"%02d:%02d:%02d.%03d", HourOf(MsgTime), MinuteOf(MsgTime), SecondOf(MsgTime), MilliSecondOf(MsgTime));
	_di_IXMLNode RootNode = DataXML->DocumentElement;
	_di_IXMLNode DataNode = RootNode->ChildNodes->First();
	UnicodeString NodeName;
	NodeName = DataNode->GetNodeName();
	if (NodeName == L"foi068s")
		DataNode = DataNode->ChildNodes->First();

	while (DataNode != 0)
	{
		if (DataNode->HasChildNodes)
		{
			NodeName = DataNode->GetNodeName();
			if (DataNode->IsTextElement)
			{
				if (NodeName == L"UK")
					FUserKey = DataNode->GetText();
				else if (NodeName == L"actno")
					FAccountNo = DataNode->GetText();
				else if (NodeName == L"actname")
					FAccountName = DataNode->GetText();
				else if (NodeName == L"company")
					FBrokerNo = DataNode->GetText();
				else if (NodeName == L"caltime")
					FCalTime = DataNode->GetText();
				else if (NodeName == L"QryEnvFlag")
					FQueryEnvFlag = DataNode->GetText();
				else if (NodeName == L"ERROR")
					FErrorFlag = DataNode->GetText();
			}
			else
			{
				if ((NodeName == L"foi068") && DataNode->HasChildNodes)
				{
					TGoodWheatPositionMessage* PositionPtr = new TGoodWheatPositionMessage(DataNode);
					FPositionArray.push_back(PositionPtr);
				}
			}  //if (DataNode->IsTextElement)
		}  //if (!DataNode->HasChildNodes)

		DataNode = DataNode->NextSibling();
	}  //while (DataNode != 0)
}  //TGoodWheatDailyPositionMessage::GetDataFromXML()

//-------------------------  TGoodWheatMarginMessage  --------------------------
//------------------------------------------------------------------------------
TGoodWheatMarginMessage::TGoodWheatMarginMessage()
:FAccountNo(L""),
 FAccountName(L""),
 FTactNo(L""),
 FBroker(L""),
 FCurrency(L""),
 FCalTime(L""),
 FCtdab(0.0),
 FDwamt(0.0),
 FOsprtlos(0.0),
 FOsprtlos_c(0.0),
 FFee(0.0),
 FAmt(0.0),
 FPremium(0.0),
 FExrate(0.0),
 FOrdmarg(0.0),
 FVal(0.0),
 FPrtlos(0.0),
 FPrtlos_c(0.0),
 FTmiamt(0.0),
 FMrgRate(0.0),
 FTmexcess1(0.0),
 FTmmamt(0.0),
 FMatnRate(0.0),
 FTmexcess(0.0),
 FOptequity(0.0),
 FOptRate(0.0),
 FTmexcess2(0.0),
 FMktval(0.0),
 FMktvalb(0.0),
 FMktvals(0.0),
 FMgnkind(L""),
 FCofkind(L""),
 FTmsciamt(0.0),
 FTmcofuse(0.0),
 FTmcofusefree(0.0),
 FPreintabf(0.0),
 FNtval(0.0),
 FNtmktval(0.0),
 FNtexcess1(0.0),
 FNtexcess2(0.0),
 FNtexcess3(0.0),
 FNtrate_a(0.0),
 FNtrate_b(0.0)
{
}  //TGoodWheatMarginMessage::TGoodWheatMarginMessage()

//------------------------------------------------------------------------------
TGoodWheatMarginMessage::TGoodWheatMarginMessage(_di_IXMLNode DataNode)
:FAccountNo(L""),
 FAccountName(L""),
 FTactNo(L""),
 FBroker(L""),
 FCurrency(L""),
 FCalDate(L""),
 FCalTime(L""),
 FCtdab(0.0),
 FDwamt(0.0),
 FOsprtlos(0.0),
 FOsprtlos_c(0.0),
 FFee(0.0),
 FAmt(0.0),
 FPremium(0.0),
 FExrate(0.0),
 FOrdmarg(0.0),
 FVal(0.0),
 FPrtlos(0.0),
 FPrtlos_c(0.0),
 FTmiamt(0.0),
 FMrgRate(0.0),
 FTmexcess1(0.0),
 FTmmamt(0.0),
 FMatnRate(0.0),
 FTmexcess(0.0),
 FOptequity(0.0),
 FOptRate(0.0),
 FTmexcess2(0.0),
 FMktval(0.0),
 FMktvalb(0.0),
 FMktvals(0.0),
 FMgnkind(L""),
 FCofkind(L""),
 FTmsciamt(0.0),
 FTmcofuse(0.0),
 FTmcofusefree(0.0),
 FPreintabf(0.0),
 FNtval(0.0),
 FNtmktval(0.0),
 FNtexcess1(0.0),
 FNtexcess2(0.0),
 FNtexcess3(0.0),
 FNtrate_a(0.0),
 FNtrate_b(0.0)
{
	GetDataFromXML(DataNode);
}  //TGoodWheatMarginMessage::TGoodWheatMarginMessage()

//------------------------------------------------------------------------------
void TGoodWheatMarginMessage::GetDataFromXML(_di_IXMLNode DataNode)
{
	if ((DataNode == 0) || (!DataNode->HasChildNodes)) return;

	_di_IXMLNode FieldNode = DataNode->ChildNodes->First();
	UnicodeString NodeName;
	UnicodeString TempText;
	while (FieldNode != 0)
	{
		NodeName = FieldNode->GetNodeName();
		if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
		{
			if (NodeName == L"actno")  //客戶帳號
				FAccountNo = FieldNode->GetText();
			else if (NodeName == L"actname")  //客戶姓名
				FAccountName = FieldNode->GetText();
			else if (NodeName == L"tactno")  //證券帳號
				FTactNo = FieldNode->GetText();
			else if (NodeName == L"broker")  //IB代碼
				FBroker = FieldNode->GetText();
			else if (NodeName == L"currency")  //幣別
				FCurrency = FieldNode->GetText();
			else if (NodeName == L"caldt")  //試算日期
				FCalDate = FieldNode->GetText();
			else if (NodeName == L"caltime")  //試算時間
				FCalTime = FieldNode->GetText();
			else if (NodeName == L"ctdab")  //本日帳戶餘額(前盤淨值)
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FCtdab = TempText.ToDouble();
				else
					FCtdab = 0.0;
			}
			else if (NodeName == L"dwamt")  //帳款異動金額
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FDwamt = TempText.ToDouble();
				else
					FDwamt = 0.0;
			}
			else if (NodeName == L"osprtlos")  //試算平倉損益(日結法)
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FOsprtlos = TempText.ToDouble();
				else
					FOsprtlos = 0.0;
			}
			else if (NodeName == L"osprtlos_c")  //試算平倉損益(償還法)
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FOsprtlos_c = TempText.ToDouble();
				else
					FOsprtlos_c = 0.0;
			}
			else if (NodeName == L"fee")  //費用
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FFee = TempText.ToDouble();
				else
					FFee = 0.0;
			}
			else if (NodeName == L"amt")  //稅款
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FAmt = TempText.ToDouble();
				else
					FAmt = 0.0;
			}
			else if (NodeName == L"premium")  //權利金
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FPremium = TempText.ToDouble();
				else
					FPremium = 0.0;
			}
			else if (NodeName == L"exrate")  //匯率
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FExrate = TempText.ToDouble();
				else
					FExrate = 0.0;
			}
			else if (NodeName == L"ordmarg")  //下單保證金
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FOrdmarg = TempText.ToDouble();
				else
					FOrdmarg = 0.0;
			}
			else if (NodeName == L"val")  //淨值
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FVal = TempText.ToDouble();
				else
					FVal = 0.0;
			}
			else if (NodeName == L"prtlos")  //未平倉損益合計(日結法)
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FPrtlos = TempText.ToDouble();
				else
					FPrtlos = 0.0;
			}
			else if (NodeName == L"prtlos_c")  //未平倉損益合計(償還法)
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FPrtlos_c = TempText.ToDouble();
				else
					FPrtlos_c = 0.0;
			}
			else if (NodeName == L"tmiamt")  //留倉保證金
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FTmiamt = TempText.ToDouble();
				else
					FTmiamt = 0.0;
			}
			else if (NodeName == L"mrgRate")  //原始比率
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMrgRate = TempText.ToDouble();
				else
					FMrgRate = 0.0;
			}
			else if (NodeName == L"tmexcess1")  //下單可用保證金
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FTmexcess1 = TempText.ToDouble();
				else
					FTmexcess1 = 0.0;
			}
			else if (NodeName == L"tmmamt")  //維持保證金
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FTmmamt = TempText.ToDouble();
				else
					FTmmamt = 0.0;
			}
			else if (NodeName == L"matnRate")  //維持比率
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMatnRate = TempText.ToDouble();
				else
					FMatnRate = 0.0;
			}
			else if (NodeName == L"tmexcess")  //目前可用保證金
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FTmexcess = TempText.ToDouble();
				else
					FTmexcess = 0.0;
			}
			else if (NodeName == L"optequity")  //期權清算值
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FOptequity = TempText.ToDouble();
				else
					FOptequity = 0.0;
			}
			else if (NodeName == L"optRate")  //清算比率
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FOptRate = TempText.ToDouble();
				else
					FOptRate = 0.0;
			}
			else if (NodeName == L"tmexcess2")  //清算可用保證金
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FTmexcess2 = TempText.ToDouble();
				else
					FTmexcess2 = 0.0;
			}
			else if (NodeName == L"mktval")  //期權市值
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMktval = TempText.ToDouble();
				else
					FMktval = 0.0;
			}
			else if (NodeName == L"mktvalb")  //期權市值_買
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMktvalb = TempText.ToDouble();
				else
					FMktvalb = 0.0;
			}
			else if (NodeName == L"mktvals")  //期權市值_賣
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMktvals = TempText.ToDouble();
				else
					FMktvals = 0.0;
			}
			else if (NodeName == L"mgnkind")  //保證金類別 (S-SPAN,其它值則為策略)
				FMgnkind = FieldNode->GetText();
			else if (NodeName == L"cofkind")  //抵繳方式 (N-無抵繳,1-方式一,2-方式二)
				FCofkind = FieldNode->GetText();
			else if (NodeName == L"tmsciamt")  //SPAN結算保證金
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FTmsciamt = TempText.ToDouble();
				else
					FTmsciamt = 0.0;
			}
			else if (NodeName == L"tmcofuse")  //已抵繳金額
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FTmcofuse = TempText.ToDouble();
				else
					FTmcofuse = 0.0;
			}
			else if (NodeName == L"tmcofusefree")  //下單可用抵繳金額
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FTmcofusefree = TempText.ToDouble();
				else
					FTmcofusefree = 0.0;
			}
			else if (NodeName == L"preintabf")  //預約出金金額
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FPreintabf = TempText.ToDouble();
				else
					FPreintabf = 0.0;
			}
			else if (NodeName == L"ntval")  //臺幣淨值
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FNtval = TempText.ToDouble();
				else
					FNtval = 0.0;
			}
			else if (NodeName == L"ntmktval")  //臺幣清算值
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FNtmktval = TempText.ToDouble();
				else
					FNtmktval = 0.0;
			}
			else if (NodeName == L"ntexcess1")  //臺幣下單可用保證金
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FNtexcess1 = TempText.ToDouble();
				else
					FNtexcess1 = 0.0;
			}
			else if (NodeName == L"ntexcess2")  //臺幣清算保證金
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FNtexcess2 = TempText.ToDouble();
				else
					FNtexcess2 = 0.0;
			}
			else if (NodeName == L"ntexcess3")  //臺幣目前可用保證金
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FNtexcess3 = TempText.ToDouble();
				else
					FNtexcess3 = 0.0;
			}
			else if (NodeName == L"ntrate_a")  //NT-比率A
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FNtrate_a = TempText.ToDouble();
				else
					FNtrate_a = 0.0;
			}
			else if (NodeName == L"ntrate_b")  //NT-比率B
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FNtrate_b = TempText.ToDouble();
				else
					FNtrate_b = 0.0;
			}
		}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)

		FieldNode = FieldNode->NextSibling();
	}  //while (FieldNode != 0)
}  //TGoodWheatMarginMessage::GetDataFromXML()

//----------------------  TGoodWheatOpenInterestMessage2  -----------------------
//------------------------------------------------------------------------------
TGoodWheatOpenInterestMessage2::TGoodWheatOpenInterestMessage2()
:FOrderID(L""),
 FTradeDate(L""),
 FSide(L""),
 FQty(0),
 FCommidityNo(L""),
 FYearMonth(L""),
 FFirm(L""),
 FDayTradeOver(L""),
 FCallPut(L""),
 FIsSpread(false),
 FMatchPrice(0.0),
 FMargin(0.0),
 FPrtlos(0.0),
 FMarketPrice1(0.0),
 FStrikePrice(0.0)
{
}  //TGoodWheatOpenInterestMessage2::TGoodWheatOpenInterestMessage2()

//------------------------------------------------------------------------------
TGoodWheatOpenInterestMessage2::TGoodWheatOpenInterestMessage2(_di_IXMLNode DataNode)
:FOrderID(L""),
 FTradeDate(L""),
 FSide(L""),
 FQty(0),
 FCommidityNo(L""),
 FYearMonth(L""),
 FFirm(L""),
 FDayTradeOver(L""),
 FCallPut(L""),
 FIsSpread(false),
 FMatchPrice(0.0),
 FMargin(0.0),
 FPrtlos(0.0),
 FMarketPrice1(0.0),
 FStrikePrice(0.0)
{
	GetDataFromXML(DataNode);
}  //TGoodWheatOpenInterestMessage::TGoodWheatOpenInterestMessage2()

//------------------------------------------------------------------------------
void TGoodWheatOpenInterestMessage2::GetDataFromXML(_di_IXMLNode DataNode)
{
	if ((DataNode == 0) || (!DataNode->HasChildNodes)) return;

	_di_IXMLNode FieldNode = DataNode->ChildNodes->First();
	UnicodeString NodeName;
	UnicodeString TempText;
	while (FieldNode != 0)
	{
		NodeName = FieldNode->GetNodeName();
		if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
		{
			if (NodeName == L"ordno")
				FOrderID = FieldNode->GetText();
			else if (NodeName == L"trddt")
				FTradeDate = FieldNode->GetText();
			else if (NodeName == L"ps")
				FSide = FieldNode->GetText();
			else if (NodeName == L"qty")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FQty = TempText.ToInt();
				else
					FQty = 0;
			}
			else if (NodeName == L"comno")
				FCommidityNo = FieldNode->GetText();
			else if (NodeName == L"comym")
				FYearMonth = FieldNode->GetText();
			else if (NodeName == L"firm")
				FFirm = FieldNode->GetText();
			else if (NodeName == L"DT")
				FDayTradeOver = FieldNode->GetText();
			else if (NodeName == L"cp")
				FCallPut = FieldNode->GetText();
			else if (NodeName == L"spread")
			{
				TempText = FieldNode->GetText();
				if ((TempText.Length() > 0) && (TempText == L"Y"))
					FIsSpread = true;
				else
					FIsSpread = false;
			}
			else if (NodeName == L"price")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMatchPrice = TempText.ToDouble();
				else
					FMatchPrice = 0.0;
			}
			else if (NodeName == L"margin")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMargin = TempText.ToDouble();
				else
					FMargin = 0.0;
			}
			else if (NodeName == L"prtlos")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FPrtlos = TempText.ToDouble();
				else
					FPrtlos = 0.0;
			}
			else if (NodeName == L"mktpre1")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMarketPrice1 = TempText.ToDouble();
				else
					FMarketPrice1 = 0.0;
			}
			else if (NodeName == L"stkprc")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FStrikePrice = TempText.ToDouble();
				else
					FStrikePrice = 0.0;
			}
		}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)

		FieldNode = FieldNode->NextSibling();
	}  //while (FieldNode != 0)
}  //TGoodWheatOpenInterestMessage2::GetDataFromXML()

//----------------------  TGoodWheatDailyMarginMessage  ------------------------
//------------------------------------------------------------------------------
TGoodWheatDailyMarginMessage::TGoodWheatDailyMarginMessage()
:FMessageArriveTime(L""),
 FUserKey(L""),
 FMemberType(L""),
 FAccountNo(L""),
 FBrokerNo(L""),
 FCurrency(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
}  //TGoodWheatDailyMarginMessage::TGoodWheatDailyMarginMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyMarginMessage::TGoodWheatDailyMarginMessage(TXMLDocument* DataXML)
:FMessageArriveTime(L""),
 FUserKey(L""),
 FMemberType(L""),
 FAccountNo(L""),
 FBrokerNo(L""),
 FCurrency(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
	GetDataFromXML(DataXML);
}  //TGoodWheatDailyMarginMessage::TGoodWheatDailyMarginMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyMarginMessage::~TGoodWheatDailyMarginMessage()
{
	ClearData();
}  //TGoodWheatDailyMarginMessage::~TGoodWheatDailyMarginMessage()

//------------------------------------------------------------------------------
void TGoodWheatDailyMarginMessage::ClearData()
{
	FMessageArriveTime = L"";
	FUserKey = L"";
	FMemberType = L"";
	FAccountNo = L"";
	FBrokerNo = L"";
	FCurrency = L"";
	FQueryEnvFlag = L"";
	FErrorFlag = L"";
	TGoodWheatMarginMessage* MarginPtr;
	for (unsigned int i = 0; i < FMarginArray.size(); i++)
	{
		MarginPtr = FMarginArray[i];
		delete MarginPtr;
	}  //for (int i = 0; i < FMarginArray.size(); i++)
	FMarginArray.clear();

	TGoodWheatOpenInterestMessage2* OpenInterestPtr;
	for (unsigned int i = 0; i < FOpenInterestArray.size(); i++)
	{
		OpenInterestPtr = FOpenInterestArray[i];
		delete OpenInterestPtr;
	}  //for (int i = 0; i < FOpenInterestArray.size(); i++)
	FOpenInterestArray.clear();
}  //TGoodWheatDailyMarginMessage::ClearData()

//------------------------------------------------------------------------------
void TGoodWheatDailyMarginMessage::GetDataFromXML(TXMLDocument* DataXML)
{
	if (DataXML == 0) return;

	ClearData();
	TDateTime MsgTime = Now();
	FMessageArriveTime.printf(L"%02d:%02d:%02d.%03d", HourOf(MsgTime), MinuteOf(MsgTime), SecondOf(MsgTime), MilliSecondOf(MsgTime));
	_di_IXMLNode RootNode = DataXML->DocumentElement;
	_di_IXMLNode DataNode = RootNode->ChildNodes->First();
	UnicodeString NodeName;
	while (DataNode != 0)
	{
		if (DataNode->HasChildNodes)
		{
			NodeName = DataNode->GetNodeName();
			if (DataNode->IsTextElement)
			{
				if (NodeName == L"UK")
					FUserKey = DataNode->GetText();
				else if (NodeName == L"MemberType")
					FMemberType = DataNode->GetText();
				else if (NodeName == L"actno")
					FAccountNo = DataNode->GetText();
				else if (NodeName == L"company")
					FBrokerNo = DataNode->GetText();
				else if (NodeName == L"currency")
					FCurrency = DataNode->GetText();
				else if (NodeName == L"QryEnvFlag")
					FQueryEnvFlag = DataNode->GetText();
				else if (NodeName == L"ERROR")
					FErrorFlag = DataNode->GetText();
			}
			else
			{
				if ((NodeName == L"fbscus") && DataNode->HasChildNodes)
				{
					TGoodWheatMarginMessage* MarginPtr = new TGoodWheatMarginMessage(DataNode);
					FMarginArray.push_back(MarginPtr);
				}
				else if ((NodeName == L"fowopt") && DataNode->HasChildNodes)
				{
					TGoodWheatOpenInterestMessage2* OpenInterestPtr = new TGoodWheatOpenInterestMessage2(DataNode);
					FOpenInterestArray.push_back(OpenInterestPtr);
				}
			}  //if (DataNode->IsTextElement)
		}  //if (!DataNode->HasChildNodes)

		DataNode = DataNode->NextSibling();
	}  //while (DataNode != 0)
}  //TGoodWheatDailyMarginMessage::GetDataFromXML()

//-------------------------  TGoodWheatReportDetail  ---------------------------
//------------------------------------------------------------------------------
TGoodWheatReportDetail::TGoodWheatReportDetail()
:FAccountNo(L""),
 FAccountName(L""),
 FAeNo(L""),
 FAeName(L""),
 FCurrency(L""),
 FTradeDate(L""),
 FCount(0),
 FTdab(0.0),
 FLtab(0.0),
 FExcess(0.0),
 FIamt(0.0),
 FMamt(0.0),
 FSciamt(0.0),
 FCoftot(0),
 FCofuse(0),
 FPpayamt(0),
 FEquity(0.0),
 FPrtlos(0.0),
 FExrate(0.0),
 FMtosamt(0.0),
 FMtrdqty(0),
 FMfee(0.0),
 FMfee1(0.0),
 FMpremium1(0.0),
 FMtrdqty1(0),
 FYtosamt(0.0),
 FYtrdqty(0),
 FYfee(0.0),
 FYfee1(0.0),
 FYpremium1(0.0),
 FYtrdqty1(0)
{
}  //TGoodWheatReportDetail::TGoodWheatReportDetail()

//------------------------------------------------------------------------------
TGoodWheatReportDetail::TGoodWheatReportDetail(_di_IXMLNode DataNode)
:FAccountNo(L""),
 FAccountName(L""),
 FAeNo(L""),
 FAeName(L""),
 FCurrency(L""),
 FTradeDate(L""),
 FCount(0),
 FTdab(0.0),
 FLtab(0.0),
 FExcess(0.0),
 FIamt(0.0),
 FMamt(0.0),
 FSciamt(0.0),
 FCoftot(0),
 FCofuse(0),
 FPpayamt(0),
 FEquity(0.0),
 FPrtlos(0.0),
 FExrate(0.0),
 FMtosamt(0.0),
 FMtrdqty(0),
 FMfee(0.0),
 FMfee1(0.0),
 FMpremium1(0.0),
 FMtrdqty1(0),
 FYtosamt(0.0),
 FYtrdqty(0),
 FYfee(0.0),
 FYfee1(0.0),
 FYpremium1(0.0),
 FYtrdqty1(0)
{
	GetDataFromXML(DataNode);
}  //TGoodWheatReportDetail::TGoodWheatReportDetail()

//------------------------------------------------------------------------------
void TGoodWheatReportDetail::GetDataFromXML(_di_IXMLNode DataNode)
{
	if ((DataNode == 0) || (!DataNode->HasChildNodes)) return;

	_di_IXMLNode FieldNode = DataNode->ChildNodes->First();
	UnicodeString NodeName;
	UnicodeString TempText;
	while (FieldNode != 0)
	{
		NodeName = FieldNode->GetNodeName();
		if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
		{
			if (NodeName == L"actno")
				FAccountNo = FieldNode->GetText();
			else if (NodeName == L"actname")
				FAccountName = FieldNode->GetText();
			else if (NodeName == L"aeno")
				FAeNo = FieldNode->GetText();
			else if (NodeName == L"aename")
				FAeName = FieldNode->GetText();
			else if (NodeName == L"currency")
				FCurrency = FieldNode->GetText();
			else if (NodeName == L"tddt")
				FTradeDate = FieldNode->GetText();
			else if (NodeName == L"count")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FCount = TempText.ToInt();
				else
					FCount = 0;
			}
			else if (NodeName == L"tdab")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FTdab = TempText.ToDouble();
				else
					FTdab = 0.0;
			}
			else if (NodeName == L"ltab")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FLtab = TempText.ToDouble();
				else
					FLtab = 0.0;
			}
			else if (NodeName == L"excess")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FExcess = TempText.ToDouble();
				else
					FExcess = 0.0;
			}
			else if (NodeName == L"iamt")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FIamt = TempText.ToDouble();
				else
					FIamt = 0.0;
			}
			else if (NodeName == L"mamt")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMamt = TempText.ToDouble();
				else
					FMamt = 0.0;
			}
			else if (NodeName == L"sciamt")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FSciamt = TempText.ToDouble();
				else
					FSciamt = 0.0;
			}
			else if (NodeName == L"coftot")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FCoftot = TempText.ToDouble();
				else
					FCoftot = 0.0;
			}
			else if (NodeName == L"cofuse")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FCofuse = TempText.ToDouble();
				else
					FCofuse = 0.0;
			}
			else if (NodeName == L"ppayamt")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FPpayamt = TempText.ToDouble();
				else
					FPpayamt = 0.0;
			}
			else if (NodeName == L"equity")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FEquity = TempText.ToDouble();
				else
					FEquity = 0.0;
			}
			else if (NodeName == L"prtlos")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FPrtlos = TempText.ToDouble();
				else
					FPrtlos = 0.0;
			}
			else if (NodeName == L"exrate")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FExrate = TempText.ToDouble();
				else
					FExrate = 0.0;
			}
			else if (NodeName == L"Mtosamt")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMtosamt = TempText.ToDouble();
				else
					FMtosamt = 0.0;
			}
			else if (NodeName == L"Mtrdqty")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMtrdqty = TempText.ToInt();
				else
					FMtrdqty = 0;
			}
			else if (NodeName == L"Mfee")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMfee = TempText.ToDouble();
				else
					FMfee = 0.0;
			}
			else if (NodeName == L"Mfee1")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMfee1 = TempText.ToDouble();
				else
					FMfee1 = 0.0;
			}
			else if (NodeName == L"Mpremium1")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMpremium1 = TempText.ToDouble();
				else
					FMpremium1 = 0.0;
			}
			else if (NodeName == L"Mtrdqty1")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMtrdqty1 = TempText.ToInt();
				else
					FMtrdqty1 = 0;
			}
			else if (NodeName == L"Ytosamt")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FYtosamt = TempText.ToDouble();
				else
					FYtosamt = 0.0;
			}
			else if (NodeName == L"Ytrdqty")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FYtrdqty = TempText.ToInt();
				else
					FYtrdqty = 0;
			}
			else if (NodeName == L"Yfee")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FYfee = TempText.ToDouble();
				else
					FYfee = 0.0;
			}
			else if (NodeName == L"Yfee1")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FYfee1 = TempText.ToDouble();
				else
					FYfee1 = 0.0;
			}
			else if (NodeName == L"Ypremium1")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FYpremium1 = TempText.ToDouble();
				else
					FYpremium1 = 0.0;
			}
			else if (NodeName == L"Ytrdqty1")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FYtrdqty1 = TempText.ToInt();
				else
					FYtrdqty1 = 0;
			}
		}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)

		FieldNode = FieldNode->NextSibling();
	}  //while (FieldNode != 0)
}  //TGoodWheatReportDetail::GetDataFromXML()

//--------------------------  TGoodWheatBankDetail  ----------------------------
//------------------------------------------------------------------------------
TGoodWheatBankDetail::TGoodWheatBankDetail()
:FTxDate(L""),
 FDescription(L""),
 FNttxamt(0.0),
 FTxtp(L""),
 FCdtype(L"")
{
}  //TGoodWheatBankDetail::TGoodWheatBankDetail()

//------------------------------------------------------------------------------
TGoodWheatBankDetail::TGoodWheatBankDetail(_di_IXMLNode DataNode)
:FTxDate(L""),
 FDescription(L""),
 FNttxamt(0.0),
 FTxtp(L""),
 FCdtype(L"")
{
	GetDataFromXML(DataNode);
}  //TGoodWheatBankDetail::TGoodWheatBankDetail()

//------------------------------------------------------------------------------
void TGoodWheatBankDetail::GetDataFromXML(_di_IXMLNode DataNode)
{
	if ((DataNode == 0) || (!DataNode->HasChildNodes)) return;

	_di_IXMLNode FieldNode = DataNode->ChildNodes->First();
	UnicodeString NodeName;
	UnicodeString TempText;
	while (FieldNode != 0)
	{
		NodeName = FieldNode->GetNodeName();
		if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
		{
			if (NodeName == L"txdate")
				FTxDate = FieldNode->GetText();
			else if (NodeName == L"desp")
				FDescription = FieldNode->GetText();
			else if (NodeName == L"nttxamt")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FNttxamt = TempText.ToDouble();
				else
					FNttxamt = 0.0;
			}
			else if (NodeName == L"txtp")
				FTxtp = FieldNode->GetText();
			else if (NodeName == L"cdtype")
				FCdtype = FieldNode->GetText();
		}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)

		FieldNode = FieldNode->NextSibling();
	}  //while (FieldNode != 0)
}  //TGoodWheatBankDetail::GetDataFromXML()

//--------------------------  TGoodWheatFillDetail  ----------------------------
//------------------------------------------------------------------------------
TGoodWheatFillDetail::TGoodWheatFillDetail()
:FTradeDate(L""),
 FBuyQty(0),
 FSellQty(0),
 FCommidityNo(L""),
 FYearMonth(L""),
 FFirm(L""),
 FMatchPrice(0.0),
 FStrikePrice(0.0),
 FCallPut(L""),
 FPremium(0.0),
 FFeeSum(0.0),
 FProductKey(L"")
{
}  //TGoodWheatFillDetail::TGoodWheatFillDetail()

//------------------------------------------------------------------------------
TGoodWheatFillDetail::TGoodWheatFillDetail(_di_IXMLNode DataNode)
:FTradeDate(L""),
 FBuyQty(0),
 FSellQty(0),
 FCommidityNo(L""),
 FYearMonth(L""),
 FFirm(L""),
 FMatchPrice(0.0),
 FStrikePrice(0.0),
 FCallPut(L""),
 FPremium(0.0),
 FFeeSum(0.0),
 FProductKey(L"")
{
	GetDataFromXML(DataNode);
}  //TGoodWheatFillDetail::TGoodWheatFillDetail()

//------------------------------------------------------------------------------
void TGoodWheatFillDetail::GetDataFromXML(_di_IXMLNode DataNode)
{
	if ((DataNode == 0) || (!DataNode->HasChildNodes)) return;

	_di_IXMLNode FieldNode = DataNode->ChildNodes->First();
	UnicodeString NodeName;
	UnicodeString TempText;
	while (FieldNode != 0)
	{
		NodeName = FieldNode->GetNodeName();
		if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
		{
			if (NodeName == L"trddt")
				FTradeDate = FieldNode->GetText();
			else if (NodeName == L"BQty")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FBuyQty = TempText.ToInt();
				else
					FBuyQty = 0;
			}
			else if (NodeName == L"SQty")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FSellQty = TempText.ToInt();
				else
					FSellQty = 0;
			}
			else if (NodeName == L"comno")
				FCommidityNo = FieldNode->GetText();
			else if (NodeName == L"comym")
				FYearMonth = FieldNode->GetText();
			else if (NodeName == L"firm")
				FFirm = FieldNode->GetText();
			else if (NodeName == L"trdprc")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMatchPrice = TempText.ToDouble();
				else
					FMatchPrice = 0.0;
			}
			else if (NodeName == L"stkprc")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FStrikePrice = TempText.ToDouble();
				else
					FStrikePrice = 0.0;
			}
			else if (NodeName == L"callput")
				FCallPut = FieldNode->GetText();
			else if (NodeName == L"premium")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FPremium = TempText.ToDouble();
				else
					FPremium = 0.0;
			}
		}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)

		FieldNode = FieldNode->NextSibling();
	}  //while (FieldNode != 0)

	if ((FTradeDate == L"合計") || (FTradeDate == L"小計"))
		FFeeSum = FYearMonth.ToDouble();
	else
	{
		UnicodeString PriceString;
		PriceString.printf(L"%05.0lf", FStrikePrice);
		FProductKey = FCommidityNo + PriceString + FYearMonth + FCallPut;
	}
}  //TGoodWheatFillDetail::GetDataFromXML()

//--------------------------  TGoodWheatTradeDetail  ---------------------------
//------------------------------------------------------------------------------
TGoodWheatTradeDetail::TGoodWheatTradeDetail()
:FTradeDate(L""),
 FBuyQty(0),
 FSellQty(0),
 FCommidityNo(L""),
 FYearMonth(L""),
 FStrikePrice(0.0),
 FCallPut(L""),
 FFirm(L""),
 FMatchPrice(0.0),
 FOrignps(0.0),
 FProductKey(L"")
{
}  //TGoodWheatTradeDetail::TGoodWheatTradeDetail()

//------------------------------------------------------------------------------
TGoodWheatTradeDetail::TGoodWheatTradeDetail(_di_IXMLNode DataNode)
:FTradeDate(L""),
 FBuyQty(0),
 FSellQty(0),
 FCommidityNo(L""),
 FYearMonth(L""),
 FStrikePrice(0.0),
 FCallPut(L""),
 FFirm(L""),
 FMatchPrice(0.0),
 FOrignps(0.0),
 FProductKey(L"")
{
	GetDataFromXML(DataNode);
}  //TGoodWheatTradeDetail::TGoodWheatTradeDetail()

//------------------------------------------------------------------------------
void TGoodWheatTradeDetail::GetDataFromXML(_di_IXMLNode DataNode)
{
	if ((DataNode == 0) || (!DataNode->HasChildNodes)) return;

	_di_IXMLNode FieldNode = DataNode->ChildNodes->First();
	UnicodeString NodeName;
	UnicodeString TempText;
	while (FieldNode != 0)
	{
		NodeName = FieldNode->GetNodeName();
		if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
		{
			if (NodeName == L"trddt")
				FTradeDate = FieldNode->GetText();
			else if (NodeName == L"BQty")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FBuyQty = TempText.ToInt();
				else
					FBuyQty = 0;
			}
			else if (NodeName == L"SQty")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FSellQty = TempText.ToInt();
				else
					FSellQty = 0;
			}
			else if (NodeName == L"comno")
				FCommidityNo = FieldNode->GetText();
			else if (NodeName == L"comym")
				FYearMonth = FieldNode->GetText();
			else if (NodeName == L"stkprc")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FStrikePrice = TempText.ToDouble();
				else
					FStrikePrice = 0.0;
			}
			else if (NodeName == L"callput")
				FCallPut = FieldNode->GetText();
			else if (NodeName == L"firm")
				FFirm = FieldNode->GetText();
			else if (NodeName == L"trdprc")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMatchPrice = TempText.ToDouble();
				else
					FMatchPrice = 0.0;
			}
			else if (NodeName == L"orignps")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FOrignps = TempText.ToDouble();
				else
					FOrignps = 0.0;
			}
		}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)

		FieldNode = FieldNode->NextSibling();
	}  //while (FieldNode != 0)

	if ((FTradeDate != L"合計") && (FTradeDate != L"小計"))
	{
		UnicodeString PriceString;
		PriceString.printf(L"%05.0lf", FStrikePrice);
		FProductKey = FCommidityNo + PriceString + FYearMonth + FCallPut;
	}
}  //TGoodWheatTradeDetail::GetDataFromXML()

//-----------------------  TGoodWheatOpenInterestDetail  -----------------------
//------------------------------------------------------------------------------
TGoodWheatOpenInterestDetail::TGoodWheatOpenInterestDetail()
:FTradeDate(L""),
 FBuyQty(0),
 FSellQty(0),
 FCommidityNo(L""),
 FYearMonth(L""),
 FStrikePrice(0.0),
 FCallPut(L""),
 FFirm(L""),
 FMatchPrice(0.0),
 FPrtlos(0.0),
 FAveragePrice(0.0),
 FProductKey(L"")
{
}  //TGoodWheatOpenInterestDetail::TGoodWheatOpenInterestDetail()

//------------------------------------------------------------------------------
TGoodWheatOpenInterestDetail::TGoodWheatOpenInterestDetail(_di_IXMLNode DataNode)
:FTradeDate(L""),
 FBuyQty(0),
 FSellQty(0),
 FCommidityNo(L""),
 FYearMonth(L""),
 FStrikePrice(0.0),
 FCallPut(L""),
 FFirm(L""),
 FMatchPrice(0.0),
 FPrtlos(0.0),
 FAveragePrice(0.0),
 FProductKey(L"")
{
	GetDataFromXML(DataNode);
}  //TGoodWheatOpenInterestDetail::TGoodWheatOpenInterestDetail()

//------------------------------------------------------------------------------
void TGoodWheatOpenInterestDetail::GetDataFromXML(_di_IXMLNode DataNode)
{
	if ((DataNode == 0) || (!DataNode->HasChildNodes)) return;

	_di_IXMLNode FieldNode = DataNode->ChildNodes->First();
	UnicodeString NodeName;
	UnicodeString TempText;
	while (FieldNode != 0)
	{
		NodeName = FieldNode->GetNodeName();
		if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
		{
			if (NodeName == L"trddt")
				FTradeDate = FieldNode->GetText();
			else if (NodeName == L"BQty")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FBuyQty = TempText.ToInt();
				else
					FBuyQty = 0;
			}
			else if (NodeName == L"SQty")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FSellQty = TempText.ToInt();
				else
					FSellQty = 0;
			}
			else if (NodeName == L"comno")
				FCommidityNo = FieldNode->GetText();
			else if (NodeName == L"comym")
				FYearMonth = FieldNode->GetText();
			else if (NodeName == L"stkprc")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FStrikePrice = TempText.ToDouble();
				else
					FStrikePrice = 0.0;
			}
			else if (NodeName == L"callput")
				FCallPut = FieldNode->GetText();
			else if (NodeName == L"firm")
				FFirm = FieldNode->GetText();
			else if (NodeName == L"trdprc")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FMatchPrice = TempText.ToDouble();
				else
					FMatchPrice = 0.0;
			}
			else if (NodeName == L"prtlos")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FPrtlos = TempText.ToDouble();
				else
					FPrtlos = 0.0;
			}
		}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)

		FieldNode = FieldNode->NextSibling();
	}  //while (FieldNode != 0)

	if ((FTradeDate == L"合計") || (FTradeDate == L"小計"))
		FAveragePrice = FYearMonth.ToDouble();
	else
	{
		UnicodeString PriceString;
		PriceString.printf(L"%05.0lf", FStrikePrice);
		FProductKey = FCommidityNo + PriceString + FYearMonth + FCallPut;
	}
}  //TGoodWheatOpenInterestDetail::GetDataFromXML()

//-----------------------  TGoodWheatValuePaperDetail  -------------------------
//------------------------------------------------------------------------------
TGoodWheatValuePaperDetail::TGoodWheatValuePaperDetail()
:FOccDate(L""),
 FStakeID(L""),
 FStakeName(L""),
 FCurrency(L""),
 FStakePrice(0.0),
 FQty(0),
 FAmt(0.0)
{
}  //TGoodWheatValuePaperDetail::TGoodWheatValuePaperDetail()

//------------------------------------------------------------------------------
TGoodWheatValuePaperDetail::TGoodWheatValuePaperDetail(_di_IXMLNode DataNode)
:FOccDate(L""),
 FStakeID(L""),
 FStakeName(L""),
 FCurrency(L""),
 FStakePrice(0.0),
 FQty(0),
 FAmt(0.0)
{
	GetDataFromXML(DataNode);
}  //TGoodWheatValuePaperDetail::TGoodWheatValuePaperDetail()

//------------------------------------------------------------------------------
void TGoodWheatValuePaperDetail::GetDataFromXML(_di_IXMLNode DataNode)
{
	if ((DataNode == 0) || (!DataNode->HasChildNodes)) return;

	_di_IXMLNode FieldNode = DataNode->ChildNodes->First();
	UnicodeString NodeName;
	UnicodeString TempText;
	while (FieldNode != 0)
	{
		NodeName = FieldNode->GetNodeName();
		if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
		{
			if (NodeName == L"occdt")
				FOccDate = FieldNode->GetText();
			else if (NodeName == L"stkid")
				FStakeID = FieldNode->GetText();
			else if (NodeName == L"stkname")
				FStakeName = FieldNode->GetText();
			else if (NodeName == L"currency")
				FCurrency = FieldNode->GetText();
			else if (NodeName == L"sttprc")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FStakePrice = TempText.ToDouble();
				else
					FStakePrice = 0.0;
			}
			else if (NodeName == L"qty")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FQty = TempText.ToInt();
				else
					FQty = 0;
			}
			else if (NodeName == L"amt")
			{
				TempText = FieldNode->GetText();
				if (TempText.Length() > 0)
					FAmt = TempText.ToDouble();
				else
					FAmt = 0.0;
			}
		}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)

		FieldNode = FieldNode->NextSibling();
	}  //while (FieldNode != 0)
}  //TGoodWheatValuePaperDetail::GetDataFromXML()

//----------------------  TGoodWheatDailyReportMessage  ------------------------
//------------------------------------------------------------------------------
TGoodWheatDailyReportMessage::TGoodWheatDailyReportMessage()
:FMessageArriveTime(L""),
 FUserKey(L""),
 FTime(L""),
 FAccountNo(L""),
 FBrokerNo(L""),
 FStatus(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
}  //TGoodWheatDailyReportMessage::TGoodWheatDailyReportMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyReportMessage::TGoodWheatDailyReportMessage(TXMLDocument* DataXML)
:FMessageArriveTime(L""),
 FUserKey(L""),
 FTime(L""),
 FAccountNo(L""),
 FBrokerNo(L""),
 FStatus(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
	GetDataFromXML(DataXML);
}  //TGoodWheatDailyReportMessage::TGoodWheatDailyReportMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyReportMessage::~TGoodWheatDailyReportMessage()
{
	ClearData();
}  //TGoodWheatDailyReportMessage::~TGoodWheatDailyReportMessage()

//------------------------------------------------------------------------------
void TGoodWheatDailyReportMessage::ClearData()
{
	FMessageArriveTime = L"";
	FUserKey = L"";
	FTime = L"";
	FAccountNo = L"";
	FBrokerNo = L"";
	FStatus = L"";
	FQueryEnvFlag = L"";
	FErrorFlag = L"";

	TGoodWheatReportDetail* ReportPtr;
	for (unsigned int i = 0; i < FReportArray.size(); i++)
	{
		ReportPtr = FReportArray[i];
		delete ReportPtr;
	}  //for (int i = 0; i < FReportArray.size(); i++)
	FReportArray.clear();

	TGoodWheatBankDetail* BankPtr;
	for (unsigned int i = 0; i < FBankArray.size(); i++)
	{
		BankPtr = FBankArray[i];
		delete BankPtr;
	}  //for (int i = 0; i < FBankArray.size(); i++)
	FBankArray.clear();

	TGoodWheatFillDetail* FillPtr;
	for (unsigned int i = 0; i < FFillArray.size(); i++)
	{
		FillPtr = FFillArray[i];
		delete FillPtr;
	}  //for (int i = 0; i < FFillArray.size(); i++)
	FFillArray.clear();

	TGoodWheatTradeDetail* TradePtr;
	for (unsigned int i = 0; i < FTradeArray.size(); i++)
	{
		TradePtr = FTradeArray[i];
		delete TradePtr;
	}  //for (int i = 0; i < FTradeArray.size(); i++)
	FTradeArray.clear();

	TGoodWheatOpenInterestDetail* OpenInterestPtr;
	for (unsigned int i = 0; i < FOpenInterestArray.size(); i++)
	{
		OpenInterestPtr = FOpenInterestArray[i];
		delete OpenInterestPtr;
	}  //for (int i = 0; i < FOpenInterestArray.size(); i++)
	FOpenInterestArray.clear();

	TGoodWheatValuePaperDetail* ValuePaperPtr;
	for (unsigned int i = 0; i < FValuePaperArray.size(); i++)
	{
		ValuePaperPtr = FValuePaperArray[i];
		delete ValuePaperPtr;
	}  //for (int i = 0; i < FValuePaperArray.size(); i++)
	FValuePaperArray.clear();
}  //TGoodWheatDailyReportMessage::ClearData()

//------------------------------------------------------------------------------
void TGoodWheatDailyReportMessage::GetDataFromXML(TXMLDocument* DataXML)
{
	if (DataXML == 0) return;

	ClearData();
	TDateTime MsgTime = Now();
	FMessageArriveTime.printf(L"%02d:%02d:%02d.%03d", HourOf(MsgTime), MinuteOf(MsgTime), SecondOf(MsgTime), MilliSecondOf(MsgTime));
	_di_IXMLNode RootNode = DataXML->DocumentElement;
	_di_IXMLNode DataNode = RootNode->ChildNodes->First();
	UnicodeString NodeName;
	UnicodeString CurProductKey = L"";
	UnicodeString CurCommidityNo = L"";
	UnicodeString CurYearMonth = L"";
	UnicodeString CurCallPut = L"";
	while (DataNode != 0)
	{
		if (DataNode->HasChildNodes)
		{
			NodeName = DataNode->GetNodeName();
			if (DataNode->IsTextElement)
			{
				if (NodeName == L"UK")
					FUserKey = DataNode->GetText();
				else if (NodeName == L"Time")
					FTime = DataNode->GetText();
				else if (NodeName == L"actno")
					FAccountNo = DataNode->GetText();
				else if (NodeName == L"company")
					FBrokerNo = DataNode->GetText();
				else if (NodeName == L"status")
					FStatus = DataNode->GetText();
				else if (NodeName == L"QryEnvFlag")
					FQueryEnvFlag = DataNode->GetText();
				else if (NodeName == L"ERROR")
					FErrorFlag = DataNode->GetText();
			}
			else
			{
				if ((NodeName == L"dailyReport") && DataNode->HasChildNodes)
				{
					TGoodWheatReportDetail* ReportPtr = new TGoodWheatReportDetail(DataNode);
					FReportArray.push_back(ReportPtr);
				}
				else if ((NodeName == L"fbtbnk") && DataNode->HasChildNodes)
				{
					TGoodWheatBankDetail* BankPtr = new TGoodWheatBankDetail(DataNode);
					FBankArray.push_back(BankPtr);
				}
				else if ((NodeName == L"fbttrd") && DataNode->HasChildNodes)
				{
					TGoodWheatFillDetail* FillPtr = new TGoodWheatFillDetail(DataNode);
/*
					if (FillPtr->GetProductKey().Length() > 0)  //Is Detail
					{
						CurProductKey = FillPtr->GetProductKey();
						CurCommidityNo = FillPtr->GetCommidityNo();
						CurStrikePrice = FillPtr->GetStrikePrice();
						CurYearMonth = FillPtr->GetYearMonth();
						CurCallPut = FillPtr->GetCallPut();
					}
					else
					{
						FillPtr->SetProductKey(CurProductKey);
						FillPtr->SetCommidityNo(CurCommidityNo);
						FillPtr->SetStrikePrice(CurStrikePrice);
						FillPtr->SetYearMonth(CurYearMonth);
						FillPtr->SetCallPut(CurCallPut);
					}
*/
					FFillArray.push_back(FillPtr);
				}
				else if ((NodeName == L"fbtosd") && DataNode->HasChildNodes)
				{
					TGoodWheatTradeDetail* TradePtr = new TGoodWheatTradeDetail(DataNode);
					FTradeArray.push_back(TradePtr);
				}
				else if ((NodeName == L"fbtopd") && DataNode->HasChildNodes)
				{
					TGoodWheatOpenInterestDetail* OpenInterestPtr = new TGoodWheatOpenInterestDetail(DataNode);
					FOpenInterestArray.push_back(OpenInterestPtr);
				}
				else if ((NodeName == L"fbtcvf") && DataNode->HasChildNodes)
				{
					TGoodWheatValuePaperDetail* ValuePaperPtr = new TGoodWheatValuePaperDetail(DataNode);
					FValuePaperArray.push_back(ValuePaperPtr);
				}
			}  //if (DataNode->IsTextElement)
		}  //if (!DataNode->HasChildNodes)

		DataNode = DataNode->NextSibling();
	}  //while (DataNode != 0)
}  //TGoodWheatDailyReportMessage::GetDataFromXML()

//-----------------------  TGoodWheatDailyBankMessage  -------------------------
//------------------------------------------------------------------------------
TGoodWheatDailyBankMessage::TGoodWheatDailyBankMessage()
:FMessageArriveTime(L""),
 FUserKey(L""),
 FTime(L""),
 FAccountNo(L""),
 FBrokerNo(L""),
 FStatus(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
}  //TGoodWheatDailyBankMessage::TGoodWheatDailyBankMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyBankMessage::TGoodWheatDailyBankMessage(TXMLDocument* DataXML)
:FMessageArriveTime(L""),
 FUserKey(L""),
 FTime(L""),
 FAccountNo(L""),
 FBrokerNo(L""),
 FStatus(L""),
 FQueryEnvFlag(L""),
 FErrorFlag(L"")
{
	GetDataFromXML(DataXML);
}  //TGoodWheatDailyBankMessage::TGoodWheatDailyBankMessage()

//------------------------------------------------------------------------------
TGoodWheatDailyBankMessage::~TGoodWheatDailyBankMessage()
{
	ClearData();
}  //TGoodWheatDailyBankMessage::~TGoodWheatDailyBankMessage()

//------------------------------------------------------------------------------
void TGoodWheatDailyBankMessage::ClearData()
{
	FMessageArriveTime = L"";
	FUserKey = L"";
	FTime = L"";
	FAccountNo = L"";
	FBrokerNo = L"";
	FStatus = L"";
	FQueryEnvFlag = L"";
	FErrorFlag = L"";
	TGoodWheatBankDetail* BankPtr;
	for (unsigned int i = 0; i < FBankArray.size(); i++)
	{
		BankPtr = FBankArray[i];
		delete BankPtr;
	}  //for (int i = 0; i < FBankArray.size(); i++)

	FBankArray.clear();
}  //TGoodWheatDailyBankMessage::ClearData()

//------------------------------------------------------------------------------
void TGoodWheatDailyBankMessage::GetDataFromXML(TXMLDocument* DataXML)
{
	if (DataXML == 0) return;

	ClearData();
	TDateTime MsgTime = Now();
	FMessageArriveTime.printf(L"%02d:%02d:%02d.%03d", HourOf(MsgTime), MinuteOf(MsgTime), SecondOf(MsgTime), MilliSecondOf(MsgTime));
	_di_IXMLNode RootNode = DataXML->DocumentElement;
	_di_IXMLNode DataNode = RootNode->ChildNodes->First();
	UnicodeString NodeName;
	UnicodeString CurProductKey = L"";
	UnicodeString CurCommidityNo = L"";
	UnicodeString CurYearMonth = L"";
	UnicodeString CurCallPut = L"";
	while (DataNode != 0)
	{
		if (DataNode->HasChildNodes)
		{
			NodeName = DataNode->GetNodeName();
			if (DataNode->IsTextElement)
			{
				if (NodeName == L"UK")
					FUserKey = DataNode->GetText();
				else if (NodeName == L"Time")
					FTime = DataNode->GetText();
				else if (NodeName == L"actno")
					FAccountNo = DataNode->GetText();
				else if (NodeName == L"company")
					FBrokerNo = DataNode->GetText();
				else if (NodeName == L"status")
					FStatus = DataNode->GetText();
				else if (NodeName == L"QryEnvFlag")
					FQueryEnvFlag = DataNode->GetText();
				else if (NodeName == L"ERROR")
					FErrorFlag = DataNode->GetText();
			}
			else
			{
				if ((NodeName == L"fbtbnk") && DataNode->HasChildNodes)
				{
					TGoodWheatBankDetail* BankPtr = new TGoodWheatBankDetail(DataNode);
					FBankArray.push_back(BankPtr);
				}
			}  //if (DataNode->IsTextElement)
		}  //if (!DataNode->HasChildNodes)

		DataNode = DataNode->NextSibling();
	}  //while (DataNode != 0)
}  //TGoodWheatDailyBankMessage::GetDataFromXML()

//-------------------------  TGoodWheatHttpResource  ---------------------------
//------------------------------------------------------------------------------
__fastcall TGoodWheatHttpResource::TGoodWheatHttpResource(TComponent* Owner)
:TComponent(Owner),
 FUserKey(L""),
 FAccountNo(L""),
 FIdNo(L""),
 FBrokerNo(L""),
 FURLHost(L"http://etradetest.money888.com.tw"),
 FURLQryAccountPathName(L"/refcoTW/fbmcus.aspx"),
 FURLQryOrderPathName(L"/refcoTW/fotord1.aspx"),
 FURLQryFillPathName(L"/refcoTW/fbtdtr.aspx"),
 FURLQryDayTradePathName(L"/refcoTW/fbwost.aspx"),
 FURLQryOpenInterestPathName(L"/refcoTW/fbtopd.aspx"),
 FURLQryPositionPathName(L"/refcoTW/foi068.aspx"),
 FURLQryMarginPathName(L"/refcoTW/fbscus.aspx"),
 FURLQryMarginOpenInterestPathName(L"/refcoTW/fbscus2.aspx"),
 FURLQryDailyReportPathName(L"/refcoTW/dailyReport.aspx"),
 FURLQryBankPathName(L"/refcoTW/fbi100.aspx"),
 FURLQryAccount(L""),
 FURLQryOrder(L""),
 FURLQryFill(L""),
 FURLQryDayTrade(L""),
 FURLQryOpenInterest(L""),
 FURLQryPosition(L""),
 FURLQryMargin(L""),
 FURLQryMarginOpenInterest(L""),
 FURLQryDailyReport(L""),
 FURLQryBank(L""),
 FHttpAdapterPtr(0)
{
	FURLQryAccount            = FURLHost + FURLQryAccountPathName;
	FURLQryOrder              = FURLHost + FURLQryOrderPathName;
	FURLQryFill               = FURLHost + FURLQryFillPathName;
	FURLQryDayTrade           = FURLHost + FURLQryDayTradePathName;
	FURLQryOpenInterest       = FURLHost + FURLQryOpenInterestPathName;
	FURLQryPosition           = FURLHost + FURLQryPositionPathName;
	FURLQryMargin             = FURLHost + FURLQryMarginPathName;
	FURLQryMarginOpenInterest = FURLHost + FURLQryMarginOpenInterestPathName;
	FURLQryDailyReport        = FURLHost + FURLQryDailyReportPathName;
	FURLQryBank               = FURLHost + FURLQryBankPathName;

	FSSLIOHandlerPtr = new TIdSSLIOHandlerSocketOpenSSL(this);
	FSSLIOHandlerPtr->SSLOptions->Method = sslvSSLv23;
	FHttpAdapterPtr = new TIdHTTP(this);
	FHttpAdapterPtr->ConnectTimeout = 10000; ///< 10 sec
	FHttpAdapterPtr->IOHandler = FSSLIOHandlerPtr;
}
//------------------------------------------------------------------------------
__fastcall TGoodWheatHttpResource::~TGoodWheatHttpResource()
{
	delete FHttpAdapterPtr;
	delete FSSLIOHandlerPtr;
}  //TGoodWheatHttpResource::~TGoodWheatHttpResource()
//------------------------------------------------------------------------------
void __fastcall TGoodWheatHttpResource::SetupSSL( const String& URL )
{
	if( URL.Pos( L"https" ) != 0 ) ///< Use https
		FHttpAdapterPtr->IOHandler = FSSLIOHandlerPtr;
	else
		FHttpAdapterPtr->IOHandler = NULL;
}
//------------------------------------------------------------------------------
void __fastcall TGoodWheatHttpResource::SetRULHost(UnicodeString URLHost)
{
	FURLHost                  = URLHost;
	FURLQryAccount            = FURLHost + FURLQryAccountPathName;
	FURLQryOrder              = FURLHost + FURLQryOrderPathName;
	FURLQryFill               = FURLHost + FURLQryFillPathName;
	FURLQryDayTrade           = FURLHost + FURLQryDayTradePathName;
	FURLQryOpenInterest       = FURLHost + FURLQryOpenInterestPathName;
	FURLQryPosition           = FURLHost + FURLQryPositionPathName;
	FURLQryMargin             = FURLHost + FURLQryMarginPathName;
	FURLQryMarginOpenInterest = FURLHost + FURLQryMarginOpenInterestPathName;
	FURLQryDailyReport        = FURLHost + FURLQryDailyReportPathName;
	FURLQryBank               = FURLHost + FURLQryBankPathName;
}  //TGoodWheatHttpResource::SetURLHost()

//------------------------------------------------------------------------------
void __fastcall TGoodWheatHttpResource::SetURLQryAccountPathName(UnicodeString URLQryAccountPathName)
{
	FURLQryAccountPathName = URLQryAccountPathName;
	FURLQryAccount         = FURLHost + FURLQryAccountPathName;
}  //TGoodWheatHttpResource::SetURLHost()

//------------------------------------------------------------------------------
void __fastcall TGoodWheatHttpResource::SetURLQryOrderPathName(UnicodeString URLQryOrderPathName)
{
	FURLQryOrderPathName = URLQryOrderPathName;
	FURLQryOrder         = FURLHost + FURLQryOrderPathName;
}  //TGoodWheatHttpResource::SetURLQryOrderPathName()

//------------------------------------------------------------------------------
void __fastcall TGoodWheatHttpResource::SetURLQryFillPathName(UnicodeString URLQryFillPathName)
{
	FURLQryFillPathName = URLQryFillPathName;
	FURLQryFill         = FURLHost + FURLQryFillPathName;
}  //TGoodWheatHttpResource::SetURLQryFillPathName()

//------------------------------------------------------------------------------
void __fastcall TGoodWheatHttpResource::SetURLQryDayTradePathName(UnicodeString URLQryDayTradePathName)
{
	FURLQryDayTradePathName = URLQryDayTradePathName;
	FURLQryDayTrade         = FURLHost + FURLQryDayTradePathName;
}  //TGoodWheatHttpResource::SetURLQryDayTradePathName()

//------------------------------------------------------------------------------
void __fastcall TGoodWheatHttpResource::SetURLQryOpenInterestPathName(UnicodeString URLQryOpenInterestPathName)
{
	FURLQryOpenInterestPathName = URLQryOpenInterestPathName;
	FURLQryOpenInterest         = FURLHost + FURLQryOpenInterestPathName;
}  //TGoodWheatHttpResource::SetURLQryOpenInterestPathName()

//------------------------------------------------------------------------------
void __fastcall TGoodWheatHttpResource::SetURLQryPositionPathName(UnicodeString URLQryPositionPathName)
{
	FURLQryPositionPathName = URLQryPositionPathName;
	FURLQryPosition         = FURLHost + FURLQryPositionPathName;
}  //TGoodWheatHttpResource::SetURLQryPositionPathName()

//------------------------------------------------------------------------------
void __fastcall TGoodWheatHttpResource::SetURLQryMarginPathName(UnicodeString URLQryMarginPathName)
{
	FURLQryMarginPathName = URLQryMarginPathName;
	FURLQryMargin         = FURLHost + FURLQryMarginPathName;
}  //TGoodWheatHttpResource::SetURLQryMarginPathName()

//------------------------------------------------------------------------------
void __fastcall TGoodWheatHttpResource::SetURLQryMarginOpenInterestPathName(UnicodeString URLQryMarginOpenInterestPathName)
{
	FURLQryMarginOpenInterestPathName = URLQryMarginOpenInterestPathName;
	FURLQryMarginOpenInterest         = FURLHost + FURLQryMarginOpenInterestPathName;
}  //TGoodWheatHttpResource::SetURLQryMarginOpenInterestPathName()

//------------------------------------------------------------------------------
void __fastcall TGoodWheatHttpResource::SetURLQryDailyReportPathName(UnicodeString URLQryDailyReportPathName)
{
	FURLQryDailyReportPathName = URLQryDailyReportPathName;
	FURLQryDailyReport         = FURLHost + FURLQryDailyReportPathName;
}  //TGoodWheatHttpResource::SetURLQryDailyReportPathName()

//------------------------------------------------------------------------------
void __fastcall TGoodWheatHttpResource::SetURLQryBankPathName(UnicodeString URLQryBankPathName)
{
	FURLQryBankPathName = URLQryBankPathName;
	FURLQryBank         = FURLHost + FURLQryBankPathName;
}  //TGoodWheatHttpResource::SetURLQryBankPathName()

//------------------------------------------------------------------------------
void __fastcall TGoodWheatHttpResource::SetOnWork(TWorkEvent OnWork)
{
	FOnWork = OnWork;
	FHttpAdapterPtr->OnWork = FOnWork;
}  //TGoodWheatHttpResource::SetOnWork()

//------------------------------------------------------------------------------
void __fastcall TGoodWheatHttpResource::SetOnWorkBegin(TWorkBeginEvent OnWorkBegin)
{
	FOnWorkBegin = OnWorkBegin;
	FHttpAdapterPtr->OnWorkBegin = FOnWorkBegin;
}  //TGoodWheatHttpResource::SetOnWorkBegin()

//------------------------------------------------------------------------------
void __fastcall TGoodWheatHttpResource::SetOnWorkEnd(TWorkEndEvent OnWorkEnd)
{
	FOnWorkEnd = OnWorkEnd;
	FHttpAdapterPtr->OnWorkEnd = FOnWorkEnd;
}  //TGoodWheatHttpResource::SetOnWorkEnd()

//------------------------------------------------------------------------------
void TGoodWheatHttpResource::QueryAccount(TGoodWheatAccountMessage& AccountMsg)
{
	String QueryString = FURLQryAccount + L"?UK=" + FUserKey + L"&idno=" + FIdNo + L"&actno=" + FAccountNo + L"&company=" + FBrokerNo;
	TXMLDocument* DataXML;

	try
	{
		TStringStream *Response = new TStringStream();
		SetupSSL( QueryString );
		FHttpAdapterPtr->Get(QueryString, Response);
		DataXML = new TXMLDocument(this);
		DataXML->LoadFromStream(Response);
		DataXML->Active = true;
		AccountMsg.GetDataFromXML(DataXML);
		delete DataXML;
		delete Response;
	}
	catch (EIdSocketError& e)
	{
		if (OnSocketError != 0) OnSocketError(this, e.Message);
	}
	catch (EIdHTTPProtocolException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (EIdException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (...)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}  //try
}  //TGoodWheatHttpResource::QueryAccount()

//------------------------------------------------------------------------------
void TGoodWheatHttpResource::QueryDailyOrder(UnicodeString Currency, TGoodWheatDailyOrderMessage& DailyOrderMsg)
{
	String TempCurrency,QueryString;
	TXMLDocument* DataXML;

	TempCurrency = Currency;
	if (TempCurrency.Length() <= 0) TempCurrency = L"ALL";
	QueryString = FURLQryOrder + L"?UK=" + FUserKey + L"&actno=" + FAccountNo + L"&company=" + FBrokerNo + L"&currency=" + TempCurrency;
	try
	{
		TStringStream *Response = new TStringStream();
		SetupSSL( QueryString );
		FHttpAdapterPtr->Get(QueryString, Response);
		DataXML = new TXMLDocument(this);
		DataXML->LoadFromStream(Response);
		DataXML->Active = true;
		DailyOrderMsg.GetDataFromXML(DataXML);
		delete DataXML;
		delete Response;
	}
	catch (EIdSocketError& e)
	{
		if (OnSocketError != 0) OnSocketError(this, e.Message);
	}
	catch (EIdHTTPProtocolException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (EIdException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (...)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}  //try
}  //TGoodWheatHttpResource::QueryDailyOrder()

//------------------------------------------------------------------------------
void TGoodWheatHttpResource::QueryDailyFill(TGoodWheatDailyFillMessage& DailyFillMsg)
{
	String QueryString = FURLQryFill + L"?UK=" + FUserKey + L"&actno=" + FAccountNo + L"&company=" + FBrokerNo;
	TXMLDocument* DataXML;

	try
	{
		TStringStream *Response = new TStringStream();
		SetupSSL( QueryString );
		FHttpAdapterPtr->Get(QueryString, Response);
		DataXML = new TXMLDocument(this);
		DataXML->LoadFromStream(Response);
		DataXML->Active = true;
		DailyFillMsg.GetDataFromXML(DataXML);
		delete DataXML;
		delete Response;
	}
	catch (EIdSocketError& e)
	{
		if (OnSocketError != 0) OnSocketError(this, e.Message);
	}
	catch (EIdHTTPProtocolException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (EIdException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (...)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
}  //TGoodWheatHttpResource::QueryDailyFill()

//------------------------------------------------------------------------------
void TGoodWheatHttpResource::QueryDailyDayTrade(TGoodWheatDailyDayTradeMessage& DailyDayTradeMsg)
{
	String QueryString = FURLQryDayTrade + L"?UK=" + FUserKey + L"&actno=" + FAccountNo + L"&company=" + FBrokerNo;
	TXMLDocument* DataXML;

	try
	{
		TStringStream *Response = new TStringStream();
		SetupSSL( QueryString );
		FHttpAdapterPtr->Get(QueryString, Response);
		DataXML = new TXMLDocument(this);
		DataXML->LoadFromStream(Response);
		DataXML->Active = true;
		DailyDayTradeMsg.GetDataFromXML(DataXML);
		delete DataXML;
		delete Response;
	}
	catch (EIdSocketError& e)
	{
		if (OnSocketError != 0) OnSocketError(this, e.Message);
	}
	catch (EIdHTTPProtocolException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (EIdException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (...)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}  //try
}  //TGoodWheatHttpResource::QueryDailyDayTrade()

//------------------------------------------------------------------------------
void TGoodWheatHttpResource::QueryDailyOpenInterest(UnicodeString Currency, TGoodWheatDailyOpenInterestMessage& DailyOpenInterestMsg)
{
	String TempCurrency,QueryString;
	TXMLDocument* DataXML;

	TempCurrency = Currency;
	if( TempCurrency.Length() <= 0)
		TempCurrency = L"ALL";
	QueryString = FURLQryHoldOpenInterest + L"?UK=" + FUserKey + L"&currency=" + TempCurrency;
	try
	{
		TStringStream *Response = new TStringStream( );
		SetupSSL( QueryString );
		FHttpAdapterPtr->Get( QueryString, Response );
		DataXML = new TXMLDocument( this );
		DataXML->LoadFromStream( Response );
		DataXML->Active = true;
		DailyOpenInterestMsg.GetDataFromXML( DataXML );
		delete DataXML;
		delete Response;
	}
	catch (EIdSocketError& e)
	{
		if (OnSocketError != 0) OnSocketError(this, e.Message);
	}
	catch (EIdHTTPProtocolException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (EIdException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (...)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
}  //TGoodWheatHttpResource::QueryDailyOpenInterest()

//------------------------------------------------------------------------------
void TGoodWheatHttpResource::QueryDailyPosition(TGoodWheatDailyPositionMessage& DailyPositionMsg)
{
	String QueryString = FURLHost + FURLQryPositionPathName + L"?UK=" + FUserKey + L"&sprc=Y";
	TXMLDocument* DataXML;

	try
	{
		TStringStream *Response = new TStringStream( );
		SetupSSL( QueryString );
		FHttpAdapterPtr->Get(QueryString, Response);
		DataXML = new TXMLDocument(this);
		DataXML->LoadFromStream(Response);
		DataXML->Active = true;
		DailyPositionMsg.GetDataFromXML(DataXML);
		delete DataXML;
		delete Response;
	}
	catch (EIdSocketError& e)
	{
		if (OnSocketError != 0) OnSocketError(this, e.Message);
	}
	catch (EIdHTTPProtocolException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (EIdException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (...)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
}  //TGoodWheatHttpResource::QueryDailyPosition()

//------------------------------------------------------------------------------
void TGoodWheatHttpResource::QueryDailyMargin(UnicodeString Currency, TGoodWheatDailyMarginMessage& DailyMarginMsg)
{
	String QueryString = FURLQryMarginOpenInterest + L"?UK=" + FUserKey + L"&actno=" + FAccountNo + L"&company=" + FBrokerNo + L"&currency=" + Currency;
	TXMLDocument* DataXML;

	try
	{
		TStringStream *Response = new TStringStream();
		SetupSSL( QueryString );
		FHttpAdapterPtr->Get(QueryString, Response);
		DataXML = new TXMLDocument(this);
		DataXML->LoadFromStream(Response);
		DataXML->Active = true;
		DailyMarginMsg.GetDataFromXML(DataXML);
		delete DataXML;
		delete Response;
	}
	catch (EIdSocketError& e)
	{
		if (OnSocketError != 0) OnSocketError(this, e.Message);
	}
	catch (EIdHTTPProtocolException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (EIdException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (...)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}  //try
}  //TGoodWheatHttpResource::QueryDailyMargin()

//------------------------------------------------------------------------------
void TGoodWheatHttpResource::QueryDailyReport(UnicodeString Currency, UnicodeString ReportDate, TGoodWheatDailyReportMessage& DailyReportMsg)
{
	String QueryString = FURLQryDailyReport + L"?UK=" + FUserKey + L"&actno=" + FAccountNo + L"&company=" + FBrokerNo + L"&currency=" + Currency + L"&occdt=" + ReportDate;
	TXMLDocument* DataXML;

	try
	{
		TStringStream *Response = new TStringStream();
		SetupSSL( QueryString );
		FHttpAdapterPtr->Get(QueryString, Response);
		DataXML = new TXMLDocument(this);
		DataXML->LoadFromStream(Response);
		DataXML->Active = true;
		DailyReportMsg.GetDataFromXML(DataXML);
		delete DataXML;
		delete Response;
	}
	catch (EIdSocketError& e)
	{
		if (OnSocketError != 0) OnSocketError(this, e.Message);
	}
	catch (EIdHTTPProtocolException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (EIdException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (...)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}  //try
}  //TGoodWheatHttpResource::QueryDailyReport()

//------------------------------------------------------------------------------
void TGoodWheatHttpResource::QueryDailyBank(UnicodeString Currency, UnicodeString StartDate, UnicodeString EndDate, TGoodWheatDailyBankMessage& DailyBankMsg)
{
	String QueryString = FURLQryBank + L"?UK=" + FUserKey + L"&actno=" + FAccountNo + L"&company=" + FBrokerNo + L"&currency=" + Currency + L"&occdt1=" + StartDate + L"&occdt2=" + EndDate;
	TXMLDocument* DataXML;

	try
	{
		TStringStream *Response = new TStringStream();
		SetupSSL( QueryString );
		FHttpAdapterPtr->Get(QueryString, Response);
		DataXML = new TXMLDocument(this);
		DataXML->LoadFromStream(Response);
		DataXML->Active = true;
		DailyBankMsg.GetDataFromXML(DataXML);
		delete DataXML;
		delete Response;
	}
	catch (EIdSocketError& e)
	{
		if (OnSocketError != 0) OnSocketError(this, e.Message);
	}
	catch (EIdHTTPProtocolException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (EIdException& e)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}
	catch (...)
	{
		if (OnResponseError != 0) OnResponseError(this, FHttpAdapterPtr->ResponseCode, FHttpAdapterPtr->ResponseText);
	}  //try
}  //TGoodWheatHttpResource::QueryDailyReport()

//------------------------------------------------------------------------------
namespace Tgoodwheathttpresource
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TGoodWheatHttpResource)};
		RegisterComponents(L"Speedy", classes, 0);
	}  //Register()
}
//---------------------------------------------------------------------------
