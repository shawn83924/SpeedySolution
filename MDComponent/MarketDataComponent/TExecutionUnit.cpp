//---------------------------------------------------------------------------
#pragma hdrstop
#pragma package(smart_init)
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "TExecutionUnit.h"
#include "TradingObjects.h"
#include "TickList.h"
#include "MDComponentStrings.hpp"
#include <WideStrUtils.hpp>
//---------------------------------------------------------------------------
TCMarketDataStore *TExecution::FMarketDataStorePtr = 0;
//---------------------------------------------------------------------------
nsOrderMessageDefine::SideEnum TranslateSide(const UFC::AnsiString& SideStr)
{
	if (SideStr == "B")
		return nsOrderMessageDefine::sBuy;
	else if (SideStr == "S")
		return nsOrderMessageDefine::sSell;
	else
		return nsOrderMessageDefine::sNone;
}  //TranslateSide()

//---------------------------------------------------------------------------
nsOrderMessageDefine::PositionEffectEnum TranslatePositionEffect(const UFC::AnsiString& PositionEffectStr)
{
	if (PositionEffectStr == "O")  //Open
		return nsOrderMessageDefine::peOpen;
	else if (PositionEffectStr == "C")  //Close
		return nsOrderMessageDefine::peClose;
	else if (PositionEffectStr == "D")  //Close Today
		return nsOrderMessageDefine::peDayTrade;
	else if (PositionEffectStr == "A")  //Auto
		return nsOrderMessageDefine::peAuto;
	else if (PositionEffectStr == "a")  //Auto Today
		return nsOrderMessageDefine::peAutoToday;
	else
		return nsOrderMessageDefine::peAuto;
}  //TranslatePositionEffect()

//---------------------------------------------------------------------------
nsOrderMessageDefine::OrderTypeEnum TranslateOrderType(const UFC::AnsiString& OrderTypeStr)
{
	if (OrderTypeStr == "LMT")
		return nsOrderMessageDefine::otLimit;
	else if (OrderTypeStr == "MKT")
		return nsOrderMessageDefine::otMarket;
	else
		return nsOrderMessageDefine::otNone;
}  //TranslateOrderType()

//---------------------------------------------------------------------------
nsOrderMessageDefine::TimeInForceEnum TranslateTimeInForce(const UFC::AnsiString& TimeInForceStr)
{
	if (TimeInForceStr == "R")
		return nsOrderMessageDefine::tifROD;
	else if (TimeInForceStr == "I")
		return nsOrderMessageDefine::tifIOC;
	else if (TimeInForceStr == "F")
		return nsOrderMessageDefine::tifFOK;
	else
		return nsOrderMessageDefine::tifNone;
}  //TranslateTimeInForce()

//---------------------------------------------------------------------------
UnicodeString TranslateOrderStatus(nsOrderMessageDefine::OrderStatusEnum OrderStatus)
{
	switch(OrderStatus)
	{
		case nsOrderMessageDefine::osPendingNew:      return L"A";
		case nsOrderMessageDefine::osNew:             return L"0";
		case nsOrderMessageDefine::osCanceled:        return L"4";
		case nsOrderMessageDefine::osPartiallyFilled: return L"1";
		case nsOrderMessageDefine::osFilled:          return L"2";
		case nsOrderMessageDefine::osRejected:        return L"8";
		default:                                      return L"";
	}
}  //TranslateOrderStatus()

//---------------------------------------------------------------------------
UnicodeString TranslateSide(nsOrderMessageDefine::SideEnum Side)
{
	switch(Side)
	{
		case nsOrderMessageDefine::sBuy:  return L"B";
		case nsOrderMessageDefine::sSell: return L"S";
		default:                          return L"";
	}
}  //TranslateSide()

//---------------------------------------------------------------------------
UnicodeString TranslatePositionEffect(nsOrderMessageDefine::PositionEffectEnum PositionEffect)
{
	switch(PositionEffect)
	{
		case nsOrderMessageDefine::peOpen:         return L"O";
		case nsOrderMessageDefine::peClose:        return L"C";
		case nsOrderMessageDefine::peDayTrade:       return L"D";
		case nsOrderMessageDefine::peAuto:         return L"A";
		case nsOrderMessageDefine::peAutoToday:    return L"a";
		default:                                   return L"";
	}
}  //TranslatePositionEffect()

//---------------------------------------------------------------------------
UnicodeString TranslateOrderType(nsOrderMessageDefine::OrderTypeEnum OrderType)
{
	switch(OrderType)
	{
		case nsOrderMessageDefine::otMarket: return L"MKT";
		case nsOrderMessageDefine::otLimit:  return L"LMT";
		default:                             return L"";
	}
}  //TranslateOrderType()

//---------------------------------------------------------------------------
UnicodeString TranslateTimeInForce(nsOrderMessageDefine::TimeInForceEnum TimeInForce)
{
	switch(TimeInForce)
	{
		case nsOrderMessageDefine::tifROD: return L"R";
		case nsOrderMessageDefine::tifIOC: return L"I";
		case nsOrderMessageDefine::tifFOK: return L"F";
		default:                           return L"";
	}
}  //TranslateTimeInForce()
//---------------------------------------------------------------------------
Char TExecution::FOrderLogDelimiter = L'^';
//---------------------------------------------------------------------------
//
// class TExecution
//
//---------------------------------------------------------------------------
//  default constructor
//---------------------------------------------------------------------------
TExecution::TExecution()
{
	FSymbol = 0;
	FPxDigit = 0;
	FTransactTime = L"";
	FLocalMessageTime = L"";
	FOrderID = L"";
	FExchangeCode = L"";
	FStopExchangeCode = L"";
	FStopSymbol = L"";
	FStopPrice = 0.0;
	FStopMarketPrice = 0.0;
	FRootNID = 0L;
	FOrderStatus = nsOrderMessageDefine::osNone;
	FExecType = nsOrderMessageDefine::etNone;
	FCxlRejResponseTo = nsOrderMessageDefine::crrNone;
	FStrategyName = L"";
	FAccount = L"";
	FTradeSymbol = L"";
	FIsRoot = false;
	FIsRootCreatedByReport = false;
	FHHMM = 0;
	FOrdKind = '0';
}  //TExecution::TExecution()
//---------------------------------------------------------------------------
//  Construct from ExecutionReport
//---------------------------------------------------------------------------
TExecution::TExecution( TExecutionReportMessage* Msg )
{
	FExchangeCode = Msg->GetExchangeCode();
	FMarket       = Msg->GetMarket();
	FSymbol       = new TExchangeSymbol(FMarket, FExchangeCode, Msg->GetSymbol());
	FTradeSymbol  = Msg->GetSymbol();
	FAccount      = Msg->GetAccount();
	FRootNID = 0L;
	FTransactTime = Msg->GetTransactTime();
	FLocalMessageTime = L"";
	FExecType = Msg->GetExecType();
	FTMPExecType = Msg->GetTMPExecType();
	FCxlRejResponseTo = Msg->GetCxlRejResponseTo();
	FStatusCode = Msg->GetTMPStstusCode();
	FOrderID = Msg->GetOrderID();
	FNID = Msg->GetNID();
	FPositionEffect = Msg->GetPositionEffect();
	FOrderType = Msg->GetOrderType();
	FTimeInForce = Msg->GetTimeInForce();
	FOrderStatus = Msg->GetOrderStatus();
	FOrderQty = Msg->GetOrderQty();
	FCumQty = Msg->GetCumQty();
	FLeavesQty = Msg->GetLeavesQty();
	FSide = Msg->GetSide();
	FLegSide1 = Msg->GetLegSide1();
	FLegSide2 = Msg->GetLegSide2();
	FLastQty = Msg->GetLastQty();
	FLegQty1 = Msg->GetLegQty1();
	FLegQty2 = Msg->GetLegQty2();
	FLegPx1 = Msg->GetLegPrice1();
	FLegPx2 = Msg->GetLegPrice2();
	FPxDigit = Msg->GetPxDigit();
	FExecID = Msg->GetExecID();
	FIsRoot = false;
	FIsRootCreatedByReport = false;
	FIsExpanded = false;
	FIsLastMessage = true;
	FStopExchangeCode = L"";
	FStopSymbol = L"";
	FStopPrice = 0.0;
	FStopMarketPrice = 0.0;
	FTickCount = 0;
	FText = UTF8String( Msg->GetText() );
	if ((FOrderStatus == nsOrderMessageDefine::osPartiallyFilled) || (FOrderStatus == nsOrderMessageDefine::osFilled))
	{
		FPrice = Msg->GetPrice();
		FLastPx = Msg->GetPrice();
	}
	else
	{
		FPrice = Msg->GetPrice();
		FLastPx = Msg->GetLastPx();
	}
	if( FCumQty > 0)
		FAvgPx = Msg->GetTMPPxSubTotal() / FCumQty / pow(10.0, FPxDigit);
	else
		FAvgPx = 0.0;
	GetFromUserData( FStrategyName,FHHMM, FOrdKind, Msg );
	UpdateDisplayName();
}
//---------------------------------------------------------------------------
//  Construct from other instance
//---------------------------------------------------------------------------
TExecution::TExecution( TExecution* ExecPtr )
{
	if( ExecPtr != NULL )
	{
		FTransactTime = ExecPtr->FTransactTime;
		FLocalMessageTime = ExecPtr->FLocalMessageTime;
		FExecType = ExecPtr->FExecType;
		FTMPExecType = ExecPtr->FTMPExecType;
		FCxlRejResponseTo = ExecPtr->FCxlRejResponseTo;
		FStatusCode = ExecPtr->FStatusCode;
		FText = ExecPtr->FText;
		FMarket = ExecPtr->FMarket;
		FExchangeCode = ExecPtr->FExchangeCode;
		FOrderID = ExecPtr->FOrderID;
		FNID = ExecPtr->FNID;
		FPositionEffect = ExecPtr->FPositionEffect;
		FOrderType = ExecPtr->FOrderType;
		FTimeInForce = ExecPtr->FTimeInForce;
		FOrderStatus = ExecPtr->FOrderStatus;
		FOrderQty = ExecPtr->FOrderQty;
		FCumQty = ExecPtr->FCumQty;
		FLeavesQty = ExecPtr->FLeavesQty;
		FPrice = ExecPtr->FPrice;
		String tempSymbol = ExecPtr->GetSymbol()->GetSymbol(0);
		FSymbol = new TExchangeSymbol( FMarket, FExchangeCode, tempSymbol, FSide );
		FDisplayName = ExecPtr->FDisplayName;
		FSide = ExecPtr->FSide;
		FLegSide1 = ExecPtr->FLegSide1;
		FLegSide2 = ExecPtr->FLegSide2;
		FLastQty = ExecPtr->FLastQty;
		FLegQty1 = ExecPtr->FLegQty1;
		FLegQty2 = ExecPtr->FLegQty2;
		FLastPx = ExecPtr->FLastPx;
		FLegPx1 = ExecPtr->FLegPx1;
		FLegPx2 = ExecPtr->FLegPx2;
		FPxDigit = ExecPtr->FPxDigit;
		FAvgPx = ExecPtr->FAvgPx;
		FExecID = ExecPtr->FExecID;
		FIsRoot = false;
		FIsRootCreatedByReport = false;
		FRootNID = 0L;
		FIsExpanded = false;
		FIsLastMessage = true;
		FStopExchangeCode = ExecPtr->FStopExchangeCode;
		FStopSymbol = ExecPtr->FStopSymbol;
		FStopPrice = ExecPtr->FStopPrice;
		FStopMarketPrice = ExecPtr->FStopMarketPrice;
		FTickCount = ExecPtr->FTickCount;
		FStrategyName = ExecPtr->FStrategyName;
		FHHMM  = ExecPtr->FHHMM;
		FAccount = ExecPtr->FAccount;
		FTradeSymbol = ExecPtr->FTradeSymbol;
		FOrdKind = ExecPtr->FOrdKind;
	}
}
//---------------------------------------------------------------------------
TExecution::TExecution( TNewOrderMessage& NewMsg )
{
	SetMarket( NewMsg.GetMarket() );
	SetExchangeCode( NewMsg.GetExchangeCode() );
	SetSymbol( NewMsg.GetSymbol() );
	SetSide(NewMsg.GetSide());
	SetPrice(NewMsg.GetPrice());
	SetOrderQty(NewMsg.GetOrderQty());
	SetLeavesQty(NewMsg.GetOrderQty());
	SetCumQty(0);
	SetOrderType(NewMsg.GetOrderType());
	SetTimeInForce(NewMsg.GetTimeInForce());
	SetPositionEffect(NewMsg.GetPositionEffect());
	SetAccount(NewMsg.GetAccount());
	SetStopPrice(NewMsg.GetStopPx());
	SetStopSymbol( NewMsg.GetSymbol() );
	SetTradeSymbol( NewMsg.GetSymbol() );
	SetOrderStatus( nsOrderMessageDefine::osNone );
}
//---------------------------------------------------------------------------
TExecution::TExecution( const String& OrderLogStr )
:FIsRoot( false )
,FIsRootCreatedByReport( false )
,FIsExpanded( false )
,FIsLastMessage( true )
{
	ParseOrderLogStr(OrderLogStr);
}
//---------------------------------------------------------------------------
String __fastcall TExecution::UTF8String( const String& CStr )
{
	AnsiString cstr( CStr );

	if( IsUTF8String( cstr.c_str() ) == true )
		return UTF8ToString( cstr.c_str() );
	return String( CStr );
}
//---------------------------------------------------------------------------
String __fastcall TExecution::UTF8String( const char* CStr )
{
	if( IsUTF8String( CStr ) == true )
		return UTF8ToString( CStr );
	return String( CStr );
}
//---------------------------------------------------------------------------
//  Names/Values in User Define
//
//  KT: kbar time HHMM.
//  T:  Send message time.
//  IP: Client IP.
//  ST: Strategy Name.
//  HexIP:            "FFFFFFFF"  length 8
//  DayTrade:         'Y'/'N'     length 1
//  KBar time HHMM:   "1259"      length 4
//  Strategy Name:    "ST00001"   length 7
//---------------------------------------------------------------------------
void TExecution::GetFromUserData( String& StrategyName, int& HHMM,  char& OrdKind, TExecutionReportMessage* Msg )
{
	UFC::AnsiString UDD( Msg->GetAllUserData() );

	if( UDD.Length() >= 20 )
	{
		UFC::AnsiString   AnsiStrategyName( Msg->GetAllUserData() + 13, 7);
		UFC::AnsiString   AnsiKHHMM( Msg->GetAllUserData() + 9, 4);
		UFC::AnsiString   Kind( Msg->GetAllUserData() + 8, 1 );

		AnsiStrategyName.TrimRight();
		StrategyName = AnsiStrategyName.c_str();
		HHMM         = AnsiKHHMM.ToInt();
		OrdKind      = Kind[ 0 ];
	}
	else
	{
		StrategyName = L"";
		HHMM         = 0;
		OrdKind      = '0';
	}
}
//---------------------------------------------------------------------------
bool TExecution::GetStrategyName( UFC::AnsiString& StrategyName, TExecutionReportMessage* Msg )
{
	UFC::AnsiString UDD( Msg->GetAllUserData() );

	if( UDD.Length() >= 20 )
	{
		UFC::AnsiString   AnsiStrategyName( Msg->GetAllUserData() + 13, 7);

		AnsiStrategyName.TrimRight();
		StrategyName = AnsiStrategyName.c_str();
		return true;
	}
	else
	{
		StrategyName = L"";
		return false;
	}
}
//---------------------------------------------------------------------------
void TExecution::UpdateDisplayName( void )
{
	UnicodeString tempSymbol( FSymbol->GetSymbol( 0 ).c_str() );
	BasicInformation* Info;

	if( FMarketDataStorePtr != NULL &&
		(Info = FMarketDataStorePtr->GetBasicInformation( FExchangeCode, tempSymbol.UpperCase(), false )) != NULL )
		FDisplayName = Info->GetDisplayName();
	else
		FDisplayName = FSymbol->GetSymbol( 0 ).c_str();
}
//---------------------------------------------------------------------------
void TExecution::SetFieldValue(UFC::NameValueMessage& FieldData)
{
	UFC::AnsiString AnsiFieldValueStr;
	UFC::AnsiString AnsiExchangeCode;
	UFC::AnsiString AnsiSymbolCode;
	UnicodeString   SymbolCode;
	BasicInformation* Info;

	if (FieldData.Get("exh", AnsiExchangeCode) == TRUE)
		FExchangeCode = UTF8ToUnicodeString(AnsiExchangeCode);
	if (FieldData.Get("sym", AnsiSymbolCode) == TRUE)
		SymbolCode = UTF8ToUnicodeString(AnsiSymbolCode);
	if (FieldData.Get("bs", AnsiFieldValueStr) == TRUE)
		FSide = TranslateSide(AnsiFieldValueStr);
	if(( Info = FMarketDataStorePtr->GetBasicInformation(AnsiExchangeCode, AnsiSymbolCode)) != NULL)
		FMarket = (nsOrderMessageDefine::MarketEnum)Info->GetMarket();
	FSymbol = new TExchangeSymbol(FMarket, FExchangeCode, SymbolCode);
	if (FieldData.Get("acc", AnsiFieldValueStr) == TRUE)
		FAccount = UTF8ToUnicodeString(AnsiFieldValueStr);
	if (FieldData.Get("qty", AnsiFieldValueStr) == TRUE)
		FOrderQty = AnsiFieldValueStr.ToInt();
	if (FieldData.Get("px", AnsiFieldValueStr) == TRUE)
		FPrice = AnsiFieldValueStr.ToDouble();
	if (FieldData.Get("oty", AnsiFieldValueStr) == TRUE)
		FOrderType = TranslateOrderType(AnsiFieldValueStr);
	if (FieldData.Get("pe", AnsiFieldValueStr) == TRUE)
		FPositionEffect = TranslatePositionEffect(AnsiFieldValueStr);
	if (FieldData.Get("tif", AnsiFieldValueStr) == TRUE)
		FTimeInForce = TranslateTimeInForce(AnsiFieldValueStr);
	if (FieldData.Get("KT", AnsiFieldValueStr) == TRUE)
		FHHMM = AnsiFieldValueStr.ToInt();
}
//---------------------------------------------------------------------------
void TExecution::ParseFieldNameValueString( const UFC::AnsiString& FieldNameValueStr )
{
	UFC::NameValueMessage FieldData( "^|\n");

	FieldData.FromString(FieldNameValueStr);
	SetFieldValue(FieldData);
}
//---------------------------------------------------------------------------
void TExecution::GenerateOrderLogStr( String& orderLogStr  )
{
	String fieldValue;

	if (FTransactTime.Length() > 0)
		fieldValue.printf(L"%s%c", FTransactTime, FOrderLogDelimiter);
	else
		fieldValue.printf(L"%c", FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  // 1
	if (FLocalMessageTime.Length() > 0)
		fieldValue.printf(L"%s%c", FLocalMessageTime, FOrderLogDelimiter);
	else
		fieldValue.printf(L"%c", FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  // 2
	fieldValue.printf(L"%d%c", FExecType, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  // 3
	fieldValue.printf(L"%d%c", FTMPExecType, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  // 4
	fieldValue.printf(L"%d%c", FCxlRejResponseTo, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  // 5
	fieldValue.printf(L"%d%c", FStatusCode, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  // 6
	if (FText.Length() > 0)
		fieldValue.printf(L"%s%c", FText, FOrderLogDelimiter);
	else
		fieldValue.printf(L"%c", FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  // 7
	fieldValue.printf(L"%d%c", FMarket, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  // 8
	if (FExchangeCode.Length() > 0)
		fieldValue.printf(L"%s%c", FExchangeCode, FOrderLogDelimiter);
	else
		fieldValue.printf(L"%c", FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  // 9
	if (FOrderID.Length() > 0)
		fieldValue.printf(L"%s%c", FOrderID, FOrderLogDelimiter);
	else
		fieldValue.printf(L"%c", FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //10
	fieldValue.printf(L"%lld%c", FNID, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //11
	fieldValue.printf(L"%d%c", FPositionEffect, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //12
	fieldValue.printf(L"%d%c", FOrderType, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //13
	fieldValue.printf(L"%d%c", FTimeInForce, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //14
	fieldValue.printf(L"%d%c", FOrderStatus, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //15
	fieldValue.printf(L"%d%c", FOrderQty, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //16
	fieldValue.printf(L"%d%c", FCumQty, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //17
	fieldValue.printf(L"%d%c", FLeavesQty, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //18
	fieldValue.printf(L"%17.6lf%c", FPrice, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //19
	if (GetSymbol() != 0)
		fieldValue.printf(L"%s%c", GetSymbol()->GetOriginalSymbol(), FOrderLogDelimiter);
	else
		fieldValue.printf(L"%c", FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //20
	if (FDisplayName.Length() > 0)
		fieldValue.printf(L"%s%c", FDisplayName, FOrderLogDelimiter);
	else
		fieldValue.printf(L"%c", FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //21
	fieldValue.printf(L"%d%c", FSide, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //22
	fieldValue.printf(L"%d%c", FLegSide1, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //23
	fieldValue.printf(L"%d%c", FLegSide2, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //24
	fieldValue.printf(L"%d%c", FLastQty, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //25
	fieldValue.printf(L"%d%c", FLegQty1, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //26
	fieldValue.printf(L"%d%c", FLegQty2, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //27
	fieldValue.printf(L"%17.6lf%c", FLastPx, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //28
	fieldValue.printf(L"%17.6lf%c", FLegPx1, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //29
	fieldValue.printf(L"%17.6lf%c", FLegPx2, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //30
	fieldValue.printf(L"%d%c", FPxDigit, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //31
	fieldValue.printf(L"%17.6lf%c", FAvgPx, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //32
	if (FExecID.Length() > 0)
		fieldValue.printf(L"%s%c", FExecID, FOrderLogDelimiter);
	else
		fieldValue.printf(L"%c", FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //33
	fieldValue.printf(L"%lld%c", FRootNID, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //34
	if (FStopExchangeCode.Length() > 0)
		fieldValue.printf(L"%s%c", FStopExchangeCode, FOrderLogDelimiter);
	else
		fieldValue.printf(L"%c", FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //35
	if (FStopSymbol.Length() > 0)
		fieldValue.printf(L"%s%c", FStopSymbol, FOrderLogDelimiter);
	else
		fieldValue.printf(L"%c", FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //36
	fieldValue.printf(L"%17.6lf%c", FStopPrice, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //37
	fieldValue.printf(L"%17.6lf%c", FStopMarketPrice, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //38
	if (FStrategyName.Length() > 0)
		fieldValue.printf(L"%s%c", FStrategyName, FOrderLogDelimiter);
	else
		fieldValue.printf(L"%c", FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //39
	if (FAccount.Length() > 0)
		fieldValue.printf(L"%s%c", FAccount, FOrderLogDelimiter);
	else
		fieldValue.printf(L"%c", FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //40
	if( FTradeSymbol.Length() > 0)
		fieldValue.printf(L"%s%c", FTradeSymbol, FOrderLogDelimiter);
	else
		fieldValue.printf(L"%c", FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //41
	fieldValue.printf(L"%04d%c", FHHMM, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //42
	fieldValue.printf(L"%c%c", FOrdKind, FOrderLogDelimiter);
	orderLogStr = orderLogStr + fieldValue;  //43
}
//---------------------------------------------------------------------------
int TExecution::WriteToOrderLogStream(TFileStream *OrderLogStmPtr)
{
	if( OrderLogStmPtr != NULL )
	{
		String orderLogStr;
		const int writeBufferSize = 2048;
		char writeBuffer[writeBufferSize];
		char LineFeed = 10;
		int  UTF8Length = 0;

		GenerateOrderLogStr( orderLogStr );
		if (orderLogStr.Length() > 0)
		{
			UTF8Length = UnicodeToUtf8(writeBuffer, writeBufferSize, orderLogStr.c_str(), orderLogStr.Length());
			writeBuffer[UTF8Length] = 0;
		}
		if( UTF8Length > 0 )
			OrderLogStmPtr->Write((void*)(writeBuffer), UTF8Length);
		OrderLogStmPtr->Write((void*)(&LineFeed), 1);
		return UTF8Length;
	}
	return 0;
}
//---------------------------------------------------------------------------
void TExecution::ParseOrderLogStr(const String& OrderLogStr)
{
	if( OrderLogStr.Length() >  0 )
	{
		TStringList *fieldListPtr = new TStringList();
		String fieldValueStr;

		fieldListPtr->StrictDelimiter = true;
		fieldListPtr->Delimiter = FOrderLogDelimiter;
		fieldListPtr->DelimitedText = OrderLogStr;
		FTransactTime = L"";
		if (fieldListPtr->Count > 1) FTransactTime = (*fieldListPtr)[0];
		FLocalMessageTime = L"";
		if (fieldListPtr->Count > 2) FLocalMessageTime = (*fieldListPtr)[1];
		FExecType = nsOrderMessageDefine::etNone;
		if (fieldListPtr->Count > 3)
		{
			fieldValueStr = (*fieldListPtr)[2];
			if (fieldValueStr.Length() > 0)
				FExecType = static_cast<nsOrderMessageDefine::ExecTypeEnum>(fieldValueStr.ToInt());
		}
		if (fieldListPtr->Count > 4)
		{
			fieldValueStr = (*fieldListPtr)[3];
			if (fieldValueStr.Length() > 0)
				FTMPExecType = static_cast<nsOrderMessageDefine::TMPExecTypeEnum>(fieldValueStr.ToInt());
		}
		FCxlRejResponseTo = nsOrderMessageDefine::crrNone;
		if (fieldListPtr->Count > 5)
		{
			fieldValueStr = (*fieldListPtr)[4];
			if (fieldValueStr.Length() > 0)
				FCxlRejResponseTo = static_cast<nsOrderMessageDefine::CxlRejResponseToEnum>(fieldValueStr.ToInt());
		}
		FStatusCode = 0;
		if (fieldListPtr->Count > 6)
		{
			fieldValueStr = (*fieldListPtr)[5];
			if (fieldValueStr.Length() > 0) FStatusCode = fieldValueStr.ToInt();
		}
		FText = L"";
		if (fieldListPtr->Count > 7) FText = UTF8String( (*fieldListPtr)[6] );
		if (fieldListPtr->Count > 8)
		{
			fieldValueStr = (*fieldListPtr)[7];
			if (fieldValueStr.Length() > 0)
				FMarket = static_cast<nsOrderMessageDefine::MarketEnum>(fieldValueStr.ToInt());
		}
		FExchangeCode = L"";
		if (fieldListPtr->Count > 9) FExchangeCode = (*fieldListPtr)[8];
		FOrderID = L"";
		if (fieldListPtr->Count > 10) FOrderID = (*fieldListPtr)[9];
		FNID = 0L;
		if (fieldListPtr->Count > 11)
		{
			fieldValueStr = (*fieldListPtr)[10];
			if (fieldValueStr.Length() > 0) FNID = _wtoi64(fieldValueStr.c_str());
		}
		FPositionEffect = nsOrderMessageDefine::peAuto;
		if (fieldListPtr->Count > 12)
		{
			fieldValueStr = (*fieldListPtr)[11];
			if (fieldValueStr.Length() > 0)
				FPositionEffect = static_cast<nsOrderMessageDefine::PositionEffectEnum>(fieldValueStr.ToInt());
		}
		FOrderType = nsOrderMessageDefine::otNone;
		if (fieldListPtr->Count > 13)
		{
			fieldValueStr = (*fieldListPtr)[12];
			if (fieldValueStr.Length() > 0)
				FOrderType = static_cast<nsOrderMessageDefine::OrderTypeEnum>(fieldValueStr.ToInt());
		}
		FTimeInForce = nsOrderMessageDefine::tifNone;
		if (fieldListPtr->Count > 14)
		{
			fieldValueStr = (*fieldListPtr)[13];
			if (fieldValueStr.Length() > 0)
				FTimeInForce = static_cast<nsOrderMessageDefine::TimeInForceEnum>(fieldValueStr.ToInt());
		}
		FOrderStatus = nsOrderMessageDefine::osNone;
		if (fieldListPtr->Count > 15)
		{
			fieldValueStr = (*fieldListPtr)[14];
			if (fieldValueStr.Length() > 0)
				FOrderStatus = static_cast<nsOrderMessageDefine::OrderStatusEnum>(fieldValueStr.ToInt());
		}
		FOrderQty = 0;
		if (fieldListPtr->Count > 16)
		{
			fieldValueStr = (*fieldListPtr)[15];
			if (fieldValueStr.Length() > 0) FOrderQty = fieldValueStr.ToInt();
		}
		FCumQty = 0;
		if (fieldListPtr->Count > 17)
		{
			fieldValueStr = (*fieldListPtr)[16];
			if (fieldValueStr.Length() > 0) FCumQty = fieldValueStr.ToInt();
		}
		FLeavesQty = 0;
		if (fieldListPtr->Count > 18)
		{
			fieldValueStr = (*fieldListPtr)[17];
			if (fieldValueStr.Length() > 0) FLeavesQty = fieldValueStr.ToInt();
		}
		FPrice = 0.0;
		if (fieldListPtr->Count > 19)
		{
			fieldValueStr = (*fieldListPtr)[18];
			if (fieldValueStr.Length() > 0) FPrice = fieldValueStr.ToDouble();
		}
		FSymbol = 0;
		if (fieldListPtr->Count > 20)
		{
			fieldValueStr = (*fieldListPtr)[19];
			FSymbol = new TExchangeSymbol(fieldValueStr);
		}
		FDisplayName = L"";
		if (fieldListPtr->Count > 21) FDisplayName = (*fieldListPtr)[20];
		FSide = nsOrderMessageDefine::sNone;
		if (fieldListPtr->Count > 22)
		{
			fieldValueStr = (*fieldListPtr)[21];
			if (fieldValueStr.Length() > 0)
				FSide = static_cast<nsOrderMessageDefine::SideEnum>(fieldValueStr.ToInt());
		}
		FLegSide1 = nsOrderMessageDefine::sNone;
		if (fieldListPtr->Count > 23)
		{
			fieldValueStr = (*fieldListPtr)[22];
			if (fieldValueStr.Length() > 0)
				FLegSide1 = static_cast<nsOrderMessageDefine::SideEnum>(fieldValueStr.ToInt());
		}
		FLegSide2 = nsOrderMessageDefine::sNone;
		if (fieldListPtr->Count > 24)
		{
			fieldValueStr = (*fieldListPtr)[23];
			if (fieldValueStr.Length() > 0)
				FLegSide2 = static_cast<nsOrderMessageDefine::SideEnum>(fieldValueStr.ToInt());
		}
		FLastQty = 0;
		if (fieldListPtr->Count > 25)
		{
			fieldValueStr = (*fieldListPtr)[24];
			if (fieldValueStr.Length() > 0) FLastQty = fieldValueStr.ToInt();
		}
		FLegQty1 = 0;
		if (fieldListPtr->Count > 26)
		{
			fieldValueStr = (*fieldListPtr)[25];
			if (fieldValueStr.Length() > 0) FLegQty1 = fieldValueStr.ToInt();
		}
		FLegQty2 = 0;
		if (fieldListPtr->Count > 27)
		{
			fieldValueStr = (*fieldListPtr)[26];
			if (fieldValueStr.Length() > 0) FLegQty2 = fieldValueStr.ToInt();
		}
		FLastPx = 0.0;
		if (fieldListPtr->Count > 28)
		{
			fieldValueStr = (*fieldListPtr)[27];
			if (fieldValueStr.Length() > 0) FLastPx = fieldValueStr.ToDouble();
		}
		FLegPx1 = 0.0;
		if (fieldListPtr->Count > 29)
		{
			fieldValueStr = (*fieldListPtr)[28];
			if (fieldValueStr.Length() > 0) FLegPx1 = fieldValueStr.ToDouble();
		}
		FLegPx2 = 0.0;
		if (fieldListPtr->Count > 30)
		{
			fieldValueStr = (*fieldListPtr)[29];
			if (fieldValueStr.Length() > 0) FLegPx2 = fieldValueStr.ToDouble();
		}
		FPxDigit = 0;
		if (fieldListPtr->Count > 31)
		{
			fieldValueStr = (*fieldListPtr)[30];
			if (fieldValueStr.Length() > 0) FPxDigit = fieldValueStr.ToInt();
		}
		FAvgPx = 0.0;
		if (fieldListPtr->Count > 32)
		{
			fieldValueStr = (*fieldListPtr)[31];
			if (fieldValueStr.Length() > 0) FAvgPx = fieldValueStr.ToDouble();
		}
		FExecID = L"";
		if (fieldListPtr->Count > 33) FExecID = (*fieldListPtr)[32];
		FRootNID = 0L;
		if (fieldListPtr->Count > 34)
		{
			fieldValueStr = (*fieldListPtr)[33];
			if (fieldValueStr.Length() > 0) FRootNID = _wtoi64(fieldValueStr.c_str());
		}
		FStopExchangeCode = L"";
		if (fieldListPtr->Count > 35) FStopExchangeCode = (*fieldListPtr)[34];
		FStopSymbol = L"";
		if (fieldListPtr->Count > 36) FStopSymbol = (*fieldListPtr)[35];
		FStopPrice = 0.0;
		if (fieldListPtr->Count > 37)
		{
			fieldValueStr = (*fieldListPtr)[36];
			if (fieldValueStr.Length() > 0) FStopPrice = fieldValueStr.ToDouble();
		}
		FStopMarketPrice = 0.0;
		if (fieldListPtr->Count > 38)
		{
			fieldValueStr = (*fieldListPtr)[37];
			if (fieldValueStr.Length() > 0) FStopMarketPrice = fieldValueStr.ToDouble();
		}
		FStrategyName = L"";
		if (fieldListPtr->Count > 39) FStrategyName = (*fieldListPtr)[38];
		FAccount = L"";
		if (fieldListPtr->Count > 40) FAccount = (*fieldListPtr)[39];
		FTradeSymbol = L"";
		if (fieldListPtr->Count > 41) FTradeSymbol = (*fieldListPtr)[40];
		FHHMM = 0;
		if (fieldListPtr->Count > 42)
		{
			fieldValueStr = (*fieldListPtr)[41];
			FHHMM = fieldValueStr.ToInt();
		}
		if (fieldListPtr->Count > 43 )
		{
			fieldValueStr = (*fieldListPtr)[42];
			FOrdKind = fieldValueStr[1];
		}
		else
			FOrdKind = STOCK_NORMAL;
		FIsRoot = false;
		FIsExpanded = false;
		FIsLastMessage = true;
	}
}
//---------------------------------------------------------------------------
void TExecution::SetSymbol(const String& SymbolCode)
{
	if (FSymbol != 0) delete FSymbol;
	FSymbol = new TExchangeSymbol(FMarket, FExchangeCode, SymbolCode, FSide);
}
//---------------------------------------------------------------------------
void TExecution::SetRoot(bool IsRoot)
{
	FIsRoot = IsRoot;
	if (!FIsRoot) FIsExpanded = false;
}
//---------------------------------------------------------------------------
void TExecution::SetExpanded(bool IsExpanded)
{
	if (FIsRoot)
		FIsExpanded = IsExpanded;
	else
		FIsExpanded = false;
}
//---------------------------------------------------------------------------
void TExecution::SetLastMessage(bool IsLastMessage)
{
	if (FIsRoot)
		FIsLastMessage = true;
	else
		FIsLastMessage = IsLastMessage;
}
//---------------------------------------------------------------------------
UnicodeString TExecution::GetExecTypeDescription(nsOrderMessageDefine::ExecTypeEnum TargetExecType, nsOrderMessageDefine::TMPExecTypeEnum TMPExecType, int LanguageID)
{
	UnicodeString ResultString;
	switch(TargetExecType)
	{
		case nsOrderMessageDefine::etPendingNew:
			if (LanguageID == 2)
				ResultString = L"PendingNew";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_PENDING;
			break;
		case nsOrderMessageDefine::etNew:
			if (LanguageID == 2)
				ResultString = L"New";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_NEW;
			break;
		case nsOrderMessageDefine::etPendingReplace:
			if (LanguageID == 2)
				ResultString = L"PendingReplace";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_PENDING;
			break;
		case nsOrderMessageDefine::etReplaced:
			if (TMPExecType == nsOrderMessageDefine::tetReplaced)
			{
				if (LanguageID == 2)
					ResultString = L"Replace Quantity";
				else
					ResultString = Mdcomponentstrings_MD_EXECUTION_REPLACE_QTY;
			}
			else if ((TMPExecType == nsOrderMessageDefine::tetPxReplaced) ||
					 (TMPExecType == nsOrderMessageDefine::tetPxReplaced2))
			{
				if (LanguageID == 2)
					ResultString = L"Replace Price";
				else
					ResultString = Mdcomponentstrings_MD_EXECUTION_REPLACE_PX;
			}
			break;
		case nsOrderMessageDefine::etPendingCancel:
			if (LanguageID == 2)
				ResultString = L"PendingCancel";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_PENDING;
			break;
		case nsOrderMessageDefine::etCanceled:
			if (LanguageID == 2)
				ResultString = L"Cancel";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_CANCEL;
			break;
		case nsOrderMessageDefine::etPartiallyFilled:
			if (LanguageID == 2)
				ResultString = L"PartiallyFilled";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_FILL;
			break;
		case nsOrderMessageDefine::etFilled:
			if (LanguageID == 2)
				ResultString = L"Filled";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_FILL;
			break;
		case nsOrderMessageDefine::etQuoteAccept:
			ResultString = L"QuoteAccept";
			break;
		case nsOrderMessageDefine::etExpired:
			ResultString = L"Expired";
			break;
		case nsOrderMessageDefine::etRejected:
			if (LanguageID == 2)
				ResultString = L"Rejected";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_REJECT;
			break;
		case nsOrderMessageDefine::etOrderStatus:
			if (LanguageID == 2)
				ResultString = L"OrderStatus";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_STATUS;
			break;
		case nsOrderMessageDefine::etQuoteRequestAccept:
			ResultString = L"QuoteRequestAccept";
			break;
		default:
			ResultString = L"N/A";
	}  //switch(ExecType)
	return ResultString;
}
//---------------------------------------------------------------------------
UnicodeString TExecution::GetMarketDescription(nsOrderMessageDefine::MarketEnum TargetMarket, int LanguageID)
{
	UnicodeString ResultString;
	switch(TargetMarket)
	{
		case nsOrderMessageDefine::mTWFutures:
			if (LanguageID == 2)
				ResultString = L"Futures";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_FUTURES;
			break;
		case nsOrderMessageDefine::mTWOptions:
			if (LanguageID == 2)
				ResultString = L"Options";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_OPTIONS;
			break;
		case nsOrderMessageDefine::mTSE:
			ResultString = L"TSE";
			break;
		case nsOrderMessageDefine::mOTC:
			ResultString = L"OTC";
			break;
		case nsOrderMessageDefine::mForeignFutures:
			ResultString = L"PATS Futures";
			break;
		case nsOrderMessageDefine::mForeignOptions:
			ResultString = L"PATS Options";
			break;
		case nsOrderMessageDefine::mCNFutures:
			if (LanguageID == 2)
				ResultString = L"GLQH China Futures";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_FUTURES;
			break;
		default:
			ResultString = L"N/A";
	}  //switch(TargetMarket)
	return ResultString;
}  //TExecution::GetMarketDescription()

//---------------------------------------------------------------------------
UnicodeString TExecution::GetPositionEffectDescription(nsOrderMessageDefine::PositionEffectEnum TargetPositionEffect, int LanguageID)
{
	UnicodeString ResultString;
	switch(TargetPositionEffect)
	{
		case nsOrderMessageDefine::peOpen:
			if (LanguageID == 2)
				ResultString = L"Open";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_POSITION_EFFECT_OPEN;
			break;
		case nsOrderMessageDefine::peClose:
			if (LanguageID == 2)
				ResultString = L"Close";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_POSITION_EFFECT_CLOSE;
			break;
		case nsOrderMessageDefine::peDayTrade:
			if (LanguageID == 2)
				ResultString = L"DayTrade";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_POSITION_EFFECT_ROLLED;
			break;
		case nsOrderMessageDefine::peDayTradeOpen:
			if (LanguageID == 2)
				ResultString = L"DayTradeOpen";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_POSITION_EFFECT_DAY_TRADE;
			break;
		case nsOrderMessageDefine::peAuto:
			ResultString = L"Auto";
			break;
		case nsOrderMessageDefine::peAutoToday:
			ResultString = L"Auto";
			break;
		case nsOrderMessageDefine::peTMPMarketMaker:
			if (LanguageID == 2)
				ResultString = L"TMPMarketMaker";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_POSITION_EFFECT_MARKET_MAKER;  //報價
			break;
		default:
			ResultString = L"N/A";
	}  //switch(TargetPositionEffect)
	return ResultString;
}
//---------------------------------------------------------------------------
UnicodeString TExecution::GetOrderTypeDescription(nsOrderMessageDefine::OrderTypeEnum TargetOrderType, int LanguageID)
{
	UnicodeString ResultString;
	switch(TargetOrderType)
	{
		case nsOrderMessageDefine::otMarket:
			if (LanguageID == 2)
				ResultString = L"Market";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_ORDER_TYPE_MARKET;
			break;
		case nsOrderMessageDefine::otLimit:
			if (LanguageID == 2)
				ResultString = L"Limit";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_ORDER_TYPE_LIMIT;
			break;
		case nsOrderMessageDefine::otStop:
			if (LanguageID == 2)
				ResultString = L"Stop";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_ORDER_TYPE_STOP;
			break;
		case nsOrderMessageDefine::otStopLimit:
			if (LanguageID == 2)
				ResultString = L"StopLimit";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_ORDER_TYPE_STOP_LIMIT;
			break;
		default:
			ResultString = L"N/A";
	}  //switch(TargetOrderType)
	return ResultString;
}
//---------------------------------------------------------------------------
UnicodeString TExecution::GetTimeInForceDescription(nsOrderMessageDefine::TimeInForceEnum TargetTimeInForce)
{
	UnicodeString ResultString;
	switch(TargetTimeInForce)
	{
		case nsOrderMessageDefine::tifROD:
			ResultString = L"ROD";
			break;
		case nsOrderMessageDefine::tifIOC:
			ResultString = L"IOC";
			break;
		case nsOrderMessageDefine::tifFOK:
			ResultString = L"FOK";
			break;
		case nsOrderMessageDefine::tifTFXQ:
			ResultString = L"TFXQ";
			break;
		default:
			ResultString = L"N/A";
	}  //switch(TargetOrderType)
	return ResultString;
}
//---------------------------------------------------------------------------
UnicodeString TExecution::GetOrderStatusDescription(nsOrderMessageDefine::OrderStatusEnum TargetOrderStatus, nsOrderMessageDefine::TMPExecTypeEnum TMPExecType, int LanguageID)
{
	UnicodeString ResultString;
	switch(TargetOrderStatus)
	{
		case nsOrderMessageDefine::osPendingNew:
			if (LanguageID == 2)
				ResultString = L"PendingNew";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_PENDING;
			break;
		case nsOrderMessageDefine::osNew:
			if (LanguageID == 2)
				ResultString = L"New";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_NEW;
			break;
		case nsOrderMessageDefine::osPendingReplace:
			if (LanguageID == 2)
				ResultString = L"PendingReplace";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_PENDING;
			break;
		case nsOrderMessageDefine::osReplaced:
			if (TMPExecType == nsOrderMessageDefine::tetReplaced)
			{
				if (LanguageID == 2)
					ResultString = L"Replace Quantity";
				else
					ResultString = Mdcomponentstrings_MD_EXECUTION_REPLACE_QTY;
			}
			else if ((TMPExecType == nsOrderMessageDefine::tetPxReplaced) ||
					 (TMPExecType == nsOrderMessageDefine::tetPxReplaced2))
			{
				if (LanguageID == 2)
					ResultString = L"Replace Price";
				else
					ResultString = Mdcomponentstrings_MD_EXECUTION_REPLACE_PX;
			}
			break;
		case nsOrderMessageDefine::osPendingCancel:
			if (LanguageID == 2)
				ResultString = L"PendingCancel";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_PENDING;
			break;
		case nsOrderMessageDefine::osCanceled:
			if (LanguageID == 2)
				ResultString = L"Cancel";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_CANCEL;
			break;
		case nsOrderMessageDefine::osPartiallyFilled:
			if (LanguageID == 2)
				ResultString = L"PartiallyFilled";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_FILL;
			break;
		case nsOrderMessageDefine::osFilled:
			if (LanguageID == 2)
				ResultString = L"Filled";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_FILL;
			break;
		case nsOrderMessageDefine::osQuoteAccept:
			ResultString = L"QuoteAccept";
			break;
		case nsOrderMessageDefine::osExpired:
			ResultString = L"Expired";
			break;
		case nsOrderMessageDefine::osRejected:
			if (LanguageID == 2)
				ResultString = L"Rejected";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_REJECT;
			break;
		case nsOrderMessageDefine::osQuoteRequestAccept:
			ResultString = L"QuoteRequestAccept";
			break;
		default:
			ResultString = L"N/A";
	}  //switch(TargetOrderStatus)
	return ResultString;
}
//---------------------------------------------------------------------------
UnicodeString TExecution::GetSideDescription(nsOrderMessageDefine::SideEnum TargetSide, int LanguageID)
{
	UnicodeString ResultString;
	switch(TargetSide)
	{
		case nsOrderMessageDefine::sBuy :
			if (LanguageID == 2)
				ResultString = L"Buy";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_BUY;
			break;
		case nsOrderMessageDefine::sSell:
			if (LanguageID == 2)
				ResultString = L"Sell";
			else
				ResultString = Mdcomponentstrings_MD_EXECUTION_SELL;
			break;
		default:
			ResultString = L"N/A";
	}  //switch(TargetSide)
	return ResultString;
}
//---------------------------------------------------------------------------
int TExecution::GetDecimalPartLength(const String& ExchangeCode, const String& SymbolCode, double RealValue)
{
	int DecimalPartLength = -1;

	if( (FMarketDataStorePtr != NULL) && (ExchangeCode.Length() > 0) && (SymbolCode.Length() > 0))
	{
		BasicInformation *InfoPtr = FMarketDataStorePtr->GetBasicInformation(ExchangeCode, SymbolCode, false);
		if( InfoPtr !=  NULL )
			DecimalPartLength = InfoPtr->GetDigit();
	}
	if( DecimalPartLength < 0 )
		DecimalPartLength = otCheckDecimalPartLength( RealValue );
	return DecimalPartLength;
}
//---------------------------------------------------------------------------
UnicodeString TExecution::GetStringField(int Index, int LanguageID)
{
	UnicodeString ResultString;
	UnicodeString DoubleFormat;
	nsOrderMessageDefine::EventTypeEnum CallOrPut;
	String timeStr;
	int TimeStrLength = 0;
	int DecimalPartLength;
	String Symbol0;
	TTickList *TickListPtr = 0;
	switch(Index)
	{
		case 1:
			timeStr = FTransactTime;
			if (timeStr.Length() <= 0) timeStr = FLocalMessageTime;
			TimeStrLength = timeStr.Length();
			if (TimeStrLength > 0)
			{
				if( timeStr.Pos( L":" ) > 0)
				{
					if( TimeStrLength == 8 )
						ResultString = timeStr + L".000";
					else
						ResultString = timeStr;

				}
				else
				{
					if (TimeStrLength > 6)
						ResultString = timeStr.SubString(1, 2) + L":" +
									   timeStr.SubString(3, 2) + L":" +
									   timeStr.SubString(5, 2) + L":" +
									   timeStr.SubString(7, 3);
					else
						ResultString = timeStr.SubString(1, 2) + L":" +
									   timeStr.SubString(3, 2) + L":" +
									   timeStr.SubString(5, 2) + L".000";
				}
			}
			else
			{
				ResultString = L"N/A";
			}
			break;
		case 2:
			ResultString = GetExecTypeDescription(FExecType, FTMPExecType, LanguageID);
			break;
		case 3:
			ResultString.printf(L"%d", FStatusCode, LanguageID);
			break;
		case 4:
			ResultString = FText;
			break;
		case 5:
			ResultString = GetMarketDescription(FMarket, LanguageID);
			break;
		case 6:
			ResultString = FOrderID;
			break;
		case 7:
			ResultString.printf(L"%lld", FNID);
			break;
		case 8:
			ResultString = GetPositionEffectDescription(FPositionEffect, LanguageID);
			break;
		case 9:
			ResultString = GetOrderTypeDescription(FOrderType, LanguageID);
			break;
		case 10:
			ResultString = GetTimeInForceDescription(FTimeInForce);
			break;
		case 11:
			ResultString = GetOrderStatusDescription(FOrderStatus, FTMPExecType, LanguageID);
			break;
		case 12:
			ResultString.printf(L"%d", FOrderQty);
			break;
		case 13:
			ResultString.printf(L"%d", FCumQty);
			break;
		case 14:
			ResultString.printf(L"%d", FLeavesQty);
			break;
		case 15:
			ResultString = L"";
			Symbol0 = FSymbol->GetSymbol(0);
			if (FMarketDataStorePtr != 0)
			{
				TickListPtr = 0;
				TickListPtr = (TTickList*)FMarketDataStorePtr->GetTickList(FExchangeCode, Symbol0);
				if (TickListPtr != 0)
					TickListPtr->ToTNFormat(FPrice, ResultString);
			}

			if (ResultString.Length() <= 0)
			{
				DecimalPartLength = GetDecimalPartLength(FExchangeCode, Symbol0, FPrice);
				ResultString.printf(L"%.*lf", DecimalPartLength, FPrice);
			}
			break;
		case 16:
			if (FDisplayName.Length() > 0)
				ResultString = FDisplayName;
			else
				ResultString = FSymbol->GetSymbol(0);
			break;
		case 17:
			ResultString = FSymbol->GetSymbol(1);
			break;
		case 18:
			ResultString = FSymbol->GetMaturityYearMonth(1);
			break;
		case 19:
			CallOrPut = FSymbol->GetCallPut(1);
			if (CallOrPut == nsOrderMessageDefine::evtCall)
			{
				ResultString = L"Call";
			}
			else if (CallOrPut == nsOrderMessageDefine::evtCall)
			{
				ResultString = L"Put";
			}
			else
			{
				ResultString = L"N/A";
			}
			break;
		case 20:
			ResultString = FSymbol->GetStrikePx(1);
			break;
		case 21:
			ResultString = FSymbol->GetSymbol(1);
			break;
		case 22:
			ResultString = FSymbol->GetSymbol(2);
			break;
		case 23:
			ResultString = FSymbol->GetMaturityYearMonth(1);
			break;
		case 24:
			ResultString = FSymbol->GetMaturityYearMonth(2);
			break;
		case 25:
			CallOrPut = FSymbol->GetCallPut(1);
			if (CallOrPut == nsOrderMessageDefine::evtCall)
			{
				ResultString = L"Call";
			}
			else if (CallOrPut == nsOrderMessageDefine::evtCall)
			{
				ResultString = L"Put";
			}
			else
			{
				ResultString = L"N/A";
			}
			break;
		case 26:
			CallOrPut = FSymbol->GetCallPut(2);
			if (CallOrPut == nsOrderMessageDefine::evtCall)
			{
				ResultString = L"Call";
			}
			else if (CallOrPut == nsOrderMessageDefine::evtCall)
			{
				ResultString = L"Put";
			}
			else
			{
				ResultString = L"N/A";
			}
			break;
		case 27:
			ResultString = FSymbol->GetStrikePx(1);
			break;
		case 28:
			ResultString = FSymbol->GetStrikePx(2);
			break;
		case 29:
			ResultString = GetSideDescription(FSide, LanguageID);
			break;
		case 30:
			ResultString = GetSideDescription(FLegSide1, LanguageID);
			break;
		case 31:
			ResultString = GetSideDescription(FLegSide2, LanguageID);
			break;
		case 32:
			ResultString = L"";
			Symbol0 = FSymbol->GetSymbol(0);
			if (FMarketDataStorePtr != 0)
			{
				TickListPtr = 0;
				TickListPtr = (TTickList*)FMarketDataStorePtr->GetTickList(FExchangeCode, Symbol0);
				if (TickListPtr != 0)
					TickListPtr->ToTNFormat(FLastPx, ResultString);
			}

			if (ResultString.Length() <= 0)
			{
				DecimalPartLength = GetDecimalPartLength(FExchangeCode, Symbol0, FLastPx);
				ResultString.printf(L"%.*lf", DecimalPartLength, FLastPx);
			}
			break;
		case 33:
			ResultString = L"";
			Symbol0 = FSymbol->GetSymbol(0);
			if (FMarketDataStorePtr != 0)
			{
				TickListPtr = 0;
				TickListPtr = (TTickList*)FMarketDataStorePtr->GetTickList(FExchangeCode, Symbol0);
				if (TickListPtr != 0)
					TickListPtr->ToTNFormat(FLegPx1, ResultString);
			}

			if (ResultString.Length() <= 0)
			{
				DecimalPartLength = GetDecimalPartLength(FExchangeCode, Symbol0, FLegPx1);
				ResultString.printf(L"%.*lf", DecimalPartLength, FLegPx1);
			}
			break;
		case 34:
			ResultString = L"";
			Symbol0 = FSymbol->GetSymbol(0);
			if (FMarketDataStorePtr != 0)
			{
				TickListPtr = 0;
				TickListPtr = (TTickList*)FMarketDataStorePtr->GetTickList(FExchangeCode, Symbol0);
				if (TickListPtr != 0)
					TickListPtr->ToTNFormat(FLegPx2, ResultString);
			}

			if (ResultString.Length() <= 0)
			{
				DecimalPartLength = GetDecimalPartLength(FExchangeCode, Symbol0, FLegPx2);
				ResultString.printf(L"%.*lf", DecimalPartLength, FLegPx2);
			}
			break;
		case 35:
			Symbol0 = FSymbol->GetSymbol(0);
			DecimalPartLength = GetDecimalPartLength(FExchangeCode, Symbol0, FAvgPx);
			ResultString.printf(L"%.*lf", DecimalPartLength, FAvgPx);
			break;
		case 36:
			ResultString.printf(L"%s", FExecID);
			break;
		case 37:
			ResultString.printf(L"%s", FStrategyName);
			break;
		case 38:
			Symbol0 = FSymbol->GetSymbol(0);
			DecimalPartLength = GetDecimalPartLength(FExchangeCode, Symbol0, FAvgPx);
			ResultString.printf(L"%.*lf", DecimalPartLength, FStopPrice);
			break;
		case 39:
			switch( FOrdKind )
			{
				case FUTURE_DAYTRADE:      ResultString = L"當沖";break;
				case STOCK_NORMAL:         ResultString = L"現股";break;
				case STOCK_DAYTRADE_NORMAL:ResultString = L"現沖";break;
				case STOCK_MARGIN_TRADE:   ResultString = L"資買";break;
				case STOCK_SELL_SHORT:     ResultString = L"券賣";break;
				case STOCK_DAYTRADE_LOAN:  ResultString = L"當沖";break;
				case FUTURE_NORMAL:
				default :                  ResultString = L"一般";break;
			}
			break;
		default:
			ResultString.printf(L"---");
			break;
	}
	return ResultString;
}
//---------------------------------------------------------------------------
bool TExecution::InsertOrderMessage(TExecution* OrderMessagePtr)
{
	const UnicodeString& IndexKey = OrderMessagePtr->GetTimeOrderIDKey();
	typedef CStringTExecutionMultiMap::iterator MI;
	TExecution* CurMessagePtr;
	bool IsLast = true;

	if( OrderMessagePtr == NULL || IndexKey.Length() <= 0)
		return false;

	for (MI MsgIt = FExecutions.begin(); MsgIt != FExecutions.end(); MsgIt++)
	{
		CurMessagePtr = MsgIt->second;

		const String& TimeKey = CurMessagePtr->GetTimeOrderIDKey();
		if( IndexKey.Compare( TimeKey ) >= 0 )
			CurMessagePtr->SetLastMessage(false);
		else
			IsLast = false;
	}
	OrderMessagePtr->SetLastMessage(IsLast);

	FExecutions.insert(CStringTExecutionMultiMap::value_type(IndexKey, OrderMessagePtr));
	return true;
}
//---------------------------------------------------------------------------
//
//  Generate Name Value Strings from Execution Report
//
//---------------------------------------------------------------------------
UnicodeString TExecution::EncodeNameValueString(TExecutionReportMessage *Msg)
{
	UnicodeString ResultStr;
	switch (FOrderStatus)
	{
		case nsOrderMessageDefine::osPendingNew:      //  1
		case nsOrderMessageDefine::osNew:             //  2
			ResultStr = EncodeStatusNameValueString(Msg);
			break;
		case nsOrderMessageDefine::osCanceled:		  //  6
			ResultStr = EncodeCancelNameValueString(Msg);
			break;
		case nsOrderMessageDefine::osPartiallyFilled: //  7
		case nsOrderMessageDefine::osFilled:          //  8
			ResultStr = EncodeFillNameValueString(Msg);
			break;
		case nsOrderMessageDefine::osRejected:        // 11
			ResultStr = EncodeRejectNameValueString(Msg);
			break;
		default:
			ResultStr = L"";;
	}
	return ResultStr;
}  //TExecution::EncodeNameValueString()
//---------------------------------------------------------------------------
UnicodeString TExecution::EncodeStatusNameValueString(TExecutionReportMessage *Msg)
{
	UnicodeString ResultStr = L"";

	ResultStr.printf(L"os=%s", TranslateOrderStatus(FOrderStatus).c_str());
	UnicodeString NameValueStr = L"";
	NameValueStr.printf(L"^apx=%.5lf", FAvgPx);
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^cqt=%d", FCumQty);
	ResultStr = ResultStr + NameValueStr;
	UnicodeString MsgExecId = L"";
	if (Msg != 0)
	{
		UFC::AnsiString AnsiExecId = Msg->GetExecID();
		MsgExecId = UTF8ToUnicodeString(AnsiExecId);
	}
	if (MsgExecId.Length() > 0)
		NameValueStr.printf(L"^eid=%s", MsgExecId.c_str());
	else
		NameValueStr.printf(L"^eid=%s", FExecID.c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^lqt=%d", FLeavesQty);
	ResultStr = ResultStr + NameValueStr;
	if (FOrderID.Length() > 0)
	{
		NameValueStr.printf(L"^oid=%s", FOrderID.c_str());
		ResultStr = ResultStr + NameValueStr;
	}
	NameValueStr.printf(L"^bs=%s", TranslateSide(FSide).c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^sym=%s", FSymbol->GetSymbol(0).c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^acc=%s", FAccount.c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^qty=%d", FOrderQty);
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^oty=%s", TranslateOrderType(FOrderType).c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^exh=%s", FExchangeCode.c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^px=%.5lf", FPrice);
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^pe=%s", TranslatePositionEffect(FPositionEffect).c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^rct=%s", FLocalMessageTime.c_str());
	ResultStr = ResultStr + NameValueStr;
	if (FText.Length() > 0)
	{
		NameValueStr.printf(L"%s^txt=%s", FText.c_str());
		ResultStr = ResultStr + NameValueStr;
	}
	return ResultStr;
}

//---------------------------------------------------------------------------
UnicodeString TExecution::EncodeCancelNameValueString(TExecutionReportMessage *Msg)
{
	UnicodeString ResultStr = L"";

	ResultStr.printf(L"os=%s", TranslateOrderStatus(FOrderStatus).c_str());
	UnicodeString NameValueStr = L"";
	NameValueStr.printf(L"^apx=%.5lf", FAvgPx);
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^cqt=%d", FCumQty);
	ResultStr = ResultStr + NameValueStr;
	UnicodeString MsgExecId = L"";
	if (Msg != 0)
	{
		UFC::AnsiString AnsiExecId = Msg->GetExecID();
		MsgExecId = UTF8ToUnicodeString(AnsiExecId);
	}
	if (MsgExecId.Length() > 0)
		NameValueStr.printf(L"^eid=%s", MsgExecId.c_str());
	else
		NameValueStr.printf(L"^eid=%s", FExecID.c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^lqt=%d", FLeavesQty);
	ResultStr = ResultStr + NameValueStr;
	if (FOrderID.Length() > 0)
	{
		NameValueStr.printf(L"^oid=%s", FOrderID.c_str());
		ResultStr = ResultStr + NameValueStr;
	}
	NameValueStr.printf(L"^bs=%s", TranslateSide(FSide).c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^sym=%s", FSymbol->GetSymbol(0).c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^acc=%s", FAccount.c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^qty=%d", FOrderQty);
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^oty=%s", TranslateOrderType(FOrderType).c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^exh=%s", FExchangeCode.c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^pe=%s", TranslatePositionEffect(FPositionEffect).c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^rct=%s", FLocalMessageTime.c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^spx=%.5lf", FStopPrice);
	ResultStr = ResultStr + NameValueStr;
	if (FText.Length() > 0)
	{
		NameValueStr.printf(L"^txt=%s", FText.c_str());
		ResultStr = ResultStr + NameValueStr;
	}
	return ResultStr;
}
//---------------------------------------------------------------------------
UnicodeString TExecution::EncodeFillNameValueString(TExecutionReportMessage *Msg)
{
	UnicodeString ResultStr = L"";

	ResultStr.printf(L"os=%s", TranslateOrderStatus(FOrderStatus).c_str());
	UnicodeString NameValueStr = L"";
	NameValueStr.printf(L"^apx=%.5lf", FAvgPx);
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^cqt=%d", FCumQty);
	ResultStr = ResultStr + NameValueStr;
	UnicodeString MsgExecId = L"";
	if (Msg != 0)
	{
		UFC::AnsiString AnsiExecId = Msg->GetExecID();
		MsgExecId = UTF8ToUnicodeString(AnsiExecId);
	}
	if (MsgExecId.Length() > 0)
		NameValueStr.printf(L"^eid=%s", MsgExecId.c_str());
	else
		NameValueStr.printf(L"^eid=%s", FExecID.c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^lqt=%d", FLeavesQty);
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^oid=%s", FOrderID.c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^bs=%s", TranslateSide(FSide).c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^sym=%s", FSymbol->GetSymbol(0).c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^acc=%s", FAccount.c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^qty=%d", FOrderQty);
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^eqt=%d", FLastQty);
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^epx=%.5lf", FLastPx);
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^oty=%s", TranslateOrderType(FOrderType).c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^exh=%s", FExchangeCode.c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^px=%.5lf", FPrice);
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^pe=%s", TranslatePositionEffect(FPositionEffect).c_str());
	ResultStr = ResultStr + NameValueStr;
	NameValueStr.printf(L"^rct=%s", FLocalMessageTime.c_str());
	ResultStr = ResultStr + NameValueStr;
	if (FText.Length() > 0)
	{
		NameValueStr.printf(L"^txt=%s", FText.c_str());
		ResultStr = ResultStr + NameValueStr;
	}
	return ResultStr;
}
//---------------------------------------------------------------------------
UnicodeString TExecution::EncodeRejectNameValueString(TExecutionReportMessage *Msg)
{
	UnicodeString ResultStr = L"";

	ResultStr.printf(L"os=%s", TranslateOrderStatus(FOrderStatus).c_str());
	UnicodeString NameValueStr = L"";
	NameValueStr.printf(L"^rjr=%d", FStatusCode);
	ResultStr = ResultStr + NameValueStr;
	UnicodeString MsgExecId = L"";
	if (Msg != 0)
	{
		UFC::AnsiString AnsiExecId = Msg->GetExecID();
		MsgExecId = UTF8ToUnicodeString(AnsiExecId);
	}
	if (MsgExecId.Length() > 0)
		NameValueStr.printf(L"^eid=%s", MsgExecId.c_str());
	else
		NameValueStr.printf(L"^eid=%s", FExecID.c_str());
	ResultStr = ResultStr + NameValueStr;
	if ((Msg != 0) && (Msg->GetCxlRejResponseTo() == nsOrderMessageDefine::crrCancel))
	{
		NameValueStr.printf(L"^rjt=1");
		ResultStr = ResultStr + NameValueStr;
		NameValueStr.printf(L"^cnid=%lld", FNID);
		ResultStr = ResultStr + NameValueStr;
	}
	else
	{
		NameValueStr.printf(L"^apx=%.5lf", FAvgPx);
		ResultStr = ResultStr + NameValueStr;
		NameValueStr.printf(L"^cqt=%d", FCumQty);
		ResultStr = ResultStr + NameValueStr;
		NameValueStr.printf(L"^lqt=%d", FLeavesQty);
		ResultStr = ResultStr + NameValueStr;
		if (FOrderID.Length() > 0)
		{
			NameValueStr.printf(L"^oid=%s", FOrderID.c_str());
			ResultStr = ResultStr + NameValueStr;
		}
		NameValueStr.printf(L"^bs=%s", TranslateSide(FSide).c_str());
		ResultStr = ResultStr + NameValueStr;
		NameValueStr.printf(L"^sym=%s", FSymbol->GetSymbol(0).c_str());
		ResultStr = ResultStr + NameValueStr;
		NameValueStr.printf(L"^acc=%s", FAccount.c_str());
		ResultStr = ResultStr + NameValueStr;
		NameValueStr.printf(L"^oty=%s", TranslateOrderType(FOrderType).c_str());
		ResultStr = ResultStr + NameValueStr;
		NameValueStr.printf(L"^exh=%s", FExchangeCode.c_str());
		ResultStr = ResultStr + NameValueStr;
		NameValueStr.printf(L"^pe=%s", TranslatePositionEffect(FPositionEffect).c_str());
		ResultStr = ResultStr + NameValueStr;
		NameValueStr.printf(L"^rct=%s", FLocalMessageTime.c_str());
		ResultStr = ResultStr + NameValueStr;
	}
	if (FText.Length() > 0)
	{
		NameValueStr.printf(L"^txt=%s", FText.c_str());
		ResultStr = ResultStr + NameValueStr;
	}
	return ResultStr;
}
//---------------------------------------------------------------------------
void TExecution::SetDisplayName( const UnicodeString& DisplayName)
{
	if( DisplayName.Length() >  0 )
		FDisplayName = DisplayName;
}
//---------------------------------------------------------------------------

