//---------------------------------------------------------------------------


#pragma hdrstop

#include "PositionInstance.h"
#include "UFC.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
//---------------------------- TMatchMessageData ----------------------------
//---------------------------------------------------------------------------
TMatchMessageData::TMatchMessageData(const String& OrderId, const String& ExecId,
									 nsOrderMessageDefine::SideEnum Side,
									 nsOrderMessageDefine::PositionEffectEnum PositionEffect,
									 double MatchPrice, int MatchQuantity,
									 TPositionInstance *PositionInstancePtr)
:FPositionInstancePtr(PositionInstancePtr),
 FOrderId(OrderId),
 FExecId(ExecId),
 FSide(Side),
 FPositionEffect(PositionEffect),
 FMatchPrice(MatchPrice),
 FMatchQuantity(MatchQuantity)
{
	FMatchAmount = MatchPrice * MatchQuantity;
}  //TMatchMessageData::TMatchMessageData()

//---------------------------------------------------------------------------
void TMatchMessageData::SetMatchPrice(double MatchPrice)
{
	FMatchPrice = MatchPrice;
	FMatchAmount = FMatchPrice * FMatchQuantity;
}  //TMatchMessageData::SetMatchPrice()

//---------------------------------------------------------------------------
void TMatchMessageData::SetMatchQuantity(int MatchQuantity)
{
	FMatchQuantity = MatchQuantity;
	FMatchAmount = FMatchPrice * FMatchQuantity;
}  //TMatchMessageData::SetMatchQuantity()

//--------------------------- TOpenInterestObject ----------------------------
//---------------------------------------------------------------------------
TOpenInterestObject::TOpenInterestObject(nsOrderMessageDefine::SideEnum Side,
										 double MatchPrice, int MatchQuantity,
										 TPositionInstance *PositionInstancePtr)
:FPositionInstancePtr(PositionInstancePtr),
 FSide(Side),
 FMatchPrice(MatchPrice),
 FMatchQuantity(MatchQuantity)
{
	FOpenQuantity = MatchQuantity;
	FCloseQuantity = 0;
}  //TOpenInterestObject::TOpenInterestObject()

//---------------------------------------------------------------------------
int TOpenInterestObject::DoOffsetting(int CoverQuantity)
{
	if (CoverQuantity > FOpenQuantity)
	{
		FCloseQuantity += FOpenQuantity;
		FOpenQuantity = 0;
		return CoverQuantity - FOpenQuantity;
	}
	else
	{
		FOpenQuantity -= CoverQuantity;
		FCloseQuantity += CoverQuantity;
		return 0;
	}  //if (CoverQuantity >= FOpenQuantity)
}  //TOpenInterestObject::DoOffsetting()

//---------------------------------------------------------------------------
void TOpenInterestObject::DumpData( const String& Msg )
{
	AnsiString AnsiMsg      = Msg;
	AnsiString ExchangeCode = " ";
	AnsiString SymbolCode   = " ";
	AnsiString SideName;
	if (FPositionInstancePtr != 0)
	{
		ExchangeCode = FPositionInstancePtr->GetExchangeCode();
		SymbolCode = FPositionInstancePtr->GetSymbolCode();
	}
	if (FSide == nsOrderMessageDefine::sBuy)
		SideName = "B";
	else if (FSide == nsOrderMessageDefine::sSell)
		SideName = "S";
	else
		SideName = "N";
	if (AnsiMsg.Length() > 0)
		UFC::BufferedLog::Printf(" %s[%s %s] %s Px=%12.3lf  Qty=%d  O=%d  C=%d",
								 AnsiMsg.c_str(), ExchangeCode.c_str(), SymbolCode.c_str(), SideName.c_str(),
								 FMatchPrice, FMatchQuantity, FOpenQuantity, FCloseQuantity );
	else
		UFC::BufferedLog::Printf(" [%s %s] %s Px=%12.3lf  Qty=%d  O=%d  C=%d",
								 ExchangeCode.c_str(), SymbolCode.c_str(), SideName.c_str(),
								 FMatchPrice, FMatchQuantity, FOpenQuantity, FCloseQuantity );
	UFC::BufferedLog::FlushToFile();
}  //TOpenInterestObject::DumpData()

//----------------------------- TMatchPriceSum ------------------------------
//---------------------------------------------------------------------------
String TMatchPriceSum::GeneratePriceKey(double Price)
{
	String PriceKey;
	PriceKey.printf(L"%12.5lf", Price);
	return PriceKey;
}  //TMatchPriceSum::GeneratePriceKey()

//---------------------------------------------------------------------------
TMatchPriceSum::TMatchPriceSum(nsOrderMessageDefine::SideEnum Side,
							   double MatchPrice, int MatchQuantity,
							   TPositionInstance *PositionInstancePtr)
:FSide(Side),
 FMatchPrice(MatchPrice),
 FMatchQuantity(MatchQuantity),
 FPositionInstancePtr(PositionInstancePtr),
 FCloseQuantity(0),
 FCloseAmount(0.0)
{
	FPriceKey = GeneratePriceKey(MatchPrice);
	CalculateMatchAmount();
}  //TMatchPriceSum::TMatchPriceSum()

//---------------------------------------------------------------------------
void TMatchPriceSum::CalculateAmount()
{
	CalculateMatchAmount();
	CalculateCloseAmount();
}  //TMatchPriceSum::CalculateAmount()
//---------------------------------------------------------------------------
void TMatchPriceSum::SetMatchQuantity(int MatchQuantity)
{
	FMatchQuantity = MatchQuantity;
	CalculateMatchAmount();
}  //TMatchPriceSum::SetMatchQuantity()

//---------------------------------------------------------------------------
void TMatchPriceSum::AddMatchQuantity(int MatchQuantity)
{
	FMatchQuantity += MatchQuantity;
	CalculateMatchAmount();
}

//---------------------------------------------------------------------------
void TMatchPriceSum::SetCloseQuantity(int CloseQuantity)
{
	FCloseQuantity = CloseQuantity;
	CalculateCloseAmount();
}  //TMatchMessageData::SetCloseQuantity()

//---------------------------------------------------------------------------
void TMatchPriceSum::AddCloseQuantity(int CloseQuantity)
{
	FCloseQuantity += CloseQuantity;
	CalculateCloseAmount();
}  //TMatchMessageData::AddCloseQuantity()

//---------------------------------------------------------------------------
void TMatchPriceSum::DumpData(const String& Msg)
{
	AnsiString AnsiMsg      = Msg;
	AnsiString ExchangeCode = " ";
	AnsiString SymbolCode   = " ";
	AnsiString SideName;

	if (FPositionInstancePtr != 0)
	{
		ExchangeCode = FPositionInstancePtr->GetExchangeCode();
		SymbolCode = FPositionInstancePtr->GetSymbolCode();
	}
	if (FSide == nsOrderMessageDefine::sBuy)
		SideName = L"B";
	else if (FSide == nsOrderMessageDefine::sSell)
		SideName = L"S";
	else
		SideName = L"N";

	if( AnsiMsg.Length() > 0)
		UFC::BufferedLog::Printf(" %s[%s %s] %s Px=%12.3lf  Qty=%d  O=%d  C=%d",
								 AnsiMsg.c_str(), ExchangeCode.c_str(), SymbolCode.c_str(), SideName.c_str(),
								 FMatchPrice, FMatchQuantity, FMatchQuantity - FCloseQuantity, FCloseQuantity);
	else
		UFC::BufferedLog::Printf(" [%s %s] %s Px=%12.3lf  Qty=%d  O=%d  C=%d",
								 ExchangeCode.c_str(), SymbolCode.c_str(), SideName.c_str(),
								 FMatchPrice, FMatchQuantity, FMatchQuantity - FCloseQuantity, FCloseQuantity);
	UFC::BufferedLog::FlushToFile();
}  //TMatchPriceSum::DumpData()

//---------------------------- TPositionInstance ----------------------------
//---------------------------------------------------------------------------
TPositionInstance::TPositionInstance(const UnicodeString& ExchangeCode, const UnicodeString& SymbolCode)
:FExchangeCode(ExchangeCode),
 FSymbolCode(SymbolCode),
 FBuyHoldAveragePrice(0.0),
 FBuyHoldQuantity(0),
 FBuyHoldAmount(0.0),
 FSellHoldAveragePrice(0.0),
 FSellHoldQuantity(0),
 FSellHoldAmount(0.0),
 FPriceMultiplier(1.0),
 FNewestPrice(0.0),
 FTradeQuantitySum(0),
 FTaxRate(0.0),
 FUnitFee(0.0),
 FTax(0.0),
 FFee(0.0),
 FBuyQuantitySum(0),
 FBuyAmountSum(0.0),
 FBuyCloseQuantitySum(0),
 FBuyCloseAmountSum(0.0),
 FBuyAveragePrice(0.0),
 FSellQuantitySum(0),
 FSellAmountSum(0.0),
 FSellCloseQuantitySum(0),
 FSellCloseAmountSum(0.0),
 FSellAveragePrice(0.0),
 FCloseProfit(0.0),
 FFloatingProfit(0.0),
 FLastTickCount(0),
 FIsWaiting(false),
 FIsSubscribed(false)
{
	FCommodityKey = ExchangeCode + SymbolCode;
}  //TPositionInstance::TPositionInstance()

//---------------------------------------------------------------------------
TPositionInstance::~TPositionInstance()
{
	ClearMatchMessage();
	ClearHoldOpenInterestObject();
}  //TPositionInstance::~TPositionInstance()

//---------------------------------------------------------------------------
TMatchMessageData* TPositionInstance::FindMatchMessageByExecId(const String& ExecId)
{
	TMatchMessageData *MatchMessagePtr = 0;
	if ((FMatchMessageExecIdIndex.size() <= 0) || (ExecId.Length() <= 0)) return MatchMessagePtr;
	CStringMatchMessageMap::iterator it = FMatchMessageExecIdIndex.find(ExecId);
	if (it != FMatchMessageExecIdIndex.end()) MatchMessagePtr = it->second;
	return MatchMessagePtr;
}  //TPositionInstance::FindMatchMessageByExecId()

//---------------------------------------------------------------------------
bool TPositionInstance::InsertMatchMessageExecIdIndex(const String& ExecId, TMatchMessageData *MatchMsgPtr, bool DoCheck)
{
	bool IsSuccess = false;
	if ((ExecId.Length() <= 0) || (MatchMsgPtr == 0)) return IsSuccess;
	if (DoCheck && (FindMatchMessageByExecId(ExecId) != 0)) return IsSuccess;
	std::pair<CStringMatchMessageMap::iterator, bool> InsertPair;
	InsertPair = FMatchMessageExecIdIndex.insert(CStringMatchMessageMap::value_type(ExecId, MatchMsgPtr));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //TPositionInstance::InsertMatchMessageExecIdIndex()

//---------------------------------------------------------------------------
bool TPositionInstance::InsertMatchMessageExecIdIndex(TMatchMessageData *MatchMsgPtr, bool DoCheck)
{
	bool IsSuccess = false;
	if (MatchMsgPtr == 0) return IsSuccess;
	String ExecId = MatchMsgPtr->GetExecId();
	IsSuccess = InsertMatchMessageExecIdIndex(ExecId, MatchMsgPtr, DoCheck);
	return IsSuccess;
}  //TPositionInstance::InsertMatchMessageExecIdIndex()

//---------------------------------------------------------------------------
TMatchMessageData *TPositionInstance::GetMatchMessageByIndex(int Index)
{
	TMatchMessageData *MatchMessagePtr = 0;
	if ((Index < 0) || (Index >= static_cast<int>(FMatchMessageArray.size()))) return MatchMessagePtr;
	MatchMessagePtr = FMatchMessageArray[Index];
	return MatchMessagePtr;
}  //TPositionInstance::GetMatchMessageByIndex()

//---------------------------------------------------------------------------
void TPositionInstance::PushToMatchMessageArray(TMatchMessageData *MatchMessagePtr)
{
	if (MatchMessagePtr == 0) return;
	FMatchMessageArray.push_back(MatchMessagePtr);
}  //TPositionInstance::PushToMatchMessageArray()

//---------------------------------------------------------------------------
void TPositionInstance::ClearMatchMessage()
{
	CStringMatchMessageMap::iterator it;
	for (it = FMatchMessageExecIdIndex.begin(); it != FMatchMessageExecIdIndex.end(); it++)
	{
		TMatchMessageData *MatchMessagePtr = it->second;
		delete MatchMessagePtr;
	}
	FMatchMessageExecIdIndex.clear();
	FMatchMessageArray.clear();
}  //TPositionInstance::ClearMatchMessage()

//---------------------------------------------------------------------------
TMatchPriceSum *TPositionInstance::FindBuyMatchPriceSumByPrice(const String& PriceKey)
{
	TMatchPriceSum *MatchPriceSumPtr = 0;
	if ((FBuyMatchPriceSumPriceIndex.size() <= 0) || (PriceKey.Length() <= 0)) return MatchPriceSumPtr;
	CStringMatchPriceSumMap::iterator it = FBuyMatchPriceSumPriceIndex.find(PriceKey);
	if (it != FBuyMatchPriceSumPriceIndex.end()) MatchPriceSumPtr = it->second;
	return MatchPriceSumPtr;
}  //TPositionInstance::FindBuyMatchPriceSumByPrice()

//---------------------------------------------------------------------------
TMatchPriceSum *TPositionInstance::FindBuyMatchPriceSumByPrice(double Price)
{
	String PriceKey = TMatchPriceSum::GeneratePriceKey(Price);
	return FindBuyMatchPriceSumByPrice(PriceKey);
}  //TPositionInstance::FindBuyMatchPriceSumByPrice()

//---------------------------------------------------------------------------
bool TPositionInstance::InsertBuyMatchPriceSumPriceIndex(const String& PriceKey,
														 TMatchPriceSum *MatchPriceSumPtr,
														 bool DoCheck)
{
	bool IsSuccess = false;
	if ((PriceKey.Length() <= 0) || (MatchPriceSumPtr == 0)) return IsSuccess;
	if (DoCheck && (FindBuyMatchPriceSumByPrice(PriceKey) != 0)) return IsSuccess;
	std::pair<CStringMatchPriceSumMap::iterator, bool> InsertPair;
	InsertPair = FBuyMatchPriceSumPriceIndex.insert(CStringMatchPriceSumMap::value_type(PriceKey, MatchPriceSumPtr));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //TPositionInstance::InsertBuyMatchPriceSumPriceIndex()

//---------------------------------------------------------------------------
bool TPositionInstance::InsertBuyMatchPriceSumPriceIndex(TMatchPriceSum *MatchPriceSumPtr,
														 bool DoCheck)
{
	bool IsSuccess = false;
	if (MatchPriceSumPtr == 0) return IsSuccess;
	String PriceKey = TMatchPriceSum::GeneratePriceKey(MatchPriceSumPtr->GetMatchPrice());
	IsSuccess = InsertBuyMatchPriceSumPriceIndex(PriceKey, MatchPriceSumPtr, DoCheck);
	return IsSuccess;
}  //TPositionInstance::InsertBuyMatchPriceSumPriceIndex()

//---------------------------------------------------------------------------
TMatchPriceSum *TPositionInstance::GetBuyMatchPriceSumByIndex(int Index)
{
	TMatchPriceSum *MatchPriceSumPtr = 0;
	if ((Index < 0) || (Index >= static_cast<int>(FBuyMatchPriceSumArray.size()))) return MatchPriceSumPtr;
	MatchPriceSumPtr = FBuyMatchPriceSumArray[Index];
	return MatchPriceSumPtr;
}  //TPositionInstance::GetBuyMatchPriceSumByIndex()

//---------------------------------------------------------------------------
void TPositionInstance::PushToBuyMatchPriceSumArray(TMatchPriceSum *MatchPriceSumPtr)
{
	if (MatchPriceSumPtr == 0) return;
	FBuyMatchPriceSumArray.push_back(MatchPriceSumPtr);
}  //TPositionInstance::PushToBuyMatchPriceSumArray()

//---------------------------------------------------------------------------
void TPositionInstance::ClearBuyMatchPriceSum()
{
	CStringMatchPriceSumMap::iterator it;
	for (it = FBuyMatchPriceSumPriceIndex.begin(); it != FBuyMatchPriceSumPriceIndex.end(); it++)
	{
		TMatchPriceSum *MatchPriceSumPtr = it->second;
		delete MatchPriceSumPtr;
	}
	FBuyMatchPriceSumPriceIndex.clear();
	FBuyMatchPriceSumArray.clear();
}  //TPositionInstance::ClearBuyMatchPriceSum()

//---------------------------------------------------------------------------
TMatchPriceSum *TPositionInstance::FindSellMatchPriceSumByPrice(const String& PriceKey)
{
	TMatchPriceSum *MatchPriceSumPtr = 0;
	if ((FSellMatchPriceSumPriceIndex.size() <= 0) || (PriceKey.Length() <= 0)) return MatchPriceSumPtr;
	CStringMatchPriceSumMap::iterator it = FSellMatchPriceSumPriceIndex.find(PriceKey);
	if (it != FSellMatchPriceSumPriceIndex.end()) MatchPriceSumPtr = it->second;
	return MatchPriceSumPtr;
}  //TPositionInstance::FindSellMatchPriceSumByPrice()

//---------------------------------------------------------------------------
TMatchPriceSum *TPositionInstance::FindSellMatchPriceSumByPrice(double Price)
{
	String PriceKey = TMatchPriceSum::GeneratePriceKey(Price);
	return FindSellMatchPriceSumByPrice(PriceKey);
}  //TPositionInstance::FindSellMatchPriceSumByPrice()

//---------------------------------------------------------------------------
bool TPositionInstance::InsertSellMatchPriceSumPriceIndex(const String& PriceKey,
														  TMatchPriceSum *MatchPriceSumPtr, bool DoCheck)
{
	bool IsSuccess = false;
	if ((PriceKey.Length() <= 0) || (MatchPriceSumPtr == 0)) return IsSuccess;
	if (DoCheck && (FindSellMatchPriceSumByPrice(PriceKey) != 0)) return IsSuccess;
	std::pair<CStringMatchPriceSumMap::iterator, bool> InsertPair;
	InsertPair = FSellMatchPriceSumPriceIndex.insert(CStringMatchPriceSumMap::value_type(PriceKey, MatchPriceSumPtr));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //TPositionInstance::InsertSellMatchPriceSumPriceIndex()

//---------------------------------------------------------------------------
bool TPositionInstance::InsertSellMatchPriceSumPriceIndex(TMatchPriceSum *MatchPriceSumPtr, bool DoCheck)
{
	bool IsSuccess = false;
	if (MatchPriceSumPtr == 0) return IsSuccess;
	String PriceKey = TMatchPriceSum::GeneratePriceKey(MatchPriceSumPtr->GetMatchPrice());
	IsSuccess = InsertSellMatchPriceSumPriceIndex(PriceKey, MatchPriceSumPtr, DoCheck);
	return IsSuccess;
}  //TPositionInstance::InsertSellMatchPriceSumPriceIndex()

//---------------------------------------------------------------------------
TMatchPriceSum *TPositionInstance::GetSellMatchPriceSumByIndex(int Index)
{
	TMatchPriceSum *MatchPriceSumPtr = 0;
	if ((Index < 0) || (Index >= static_cast<int>(FSellMatchPriceSumArray.size()))) return MatchPriceSumPtr;
	MatchPriceSumPtr = FSellMatchPriceSumArray[Index];
	return MatchPriceSumPtr;
}  //TPositionInstance::GetSellMatchPriceSumByIndex()

//---------------------------------------------------------------------------
void TPositionInstance::PushToSellMatchPriceSumArray(TMatchPriceSum *MatchPriceSumPtr)
{
	if (MatchPriceSumPtr == 0) return;
	FSellMatchPriceSumArray.push_back(MatchPriceSumPtr);
}  //TPositionInstance::PushToSellMatchPriceSumArray()

//---------------------------------------------------------------------------
void TPositionInstance::ClearSellMatchPriceSum()
{
	CStringMatchPriceSumMap::iterator it;
	for (it = FSellMatchPriceSumPriceIndex.begin(); it != FSellMatchPriceSumPriceIndex.end(); it++)
	{
		TMatchPriceSum *MatchPriceSumPtr = it->second;
		delete MatchPriceSumPtr;
	}
	FSellMatchPriceSumPriceIndex.clear();
	FSellMatchPriceSumArray.clear();
}  //TPositionInstance::ClearSellMatchPriceSum()

//---------------------------------------------------------------------------
void TPositionInstance::ClearHoldOpenInterestObject()
{
	TOpenInterestObject *curOIObjPtr;
	for (unsigned int i = 0; i < FBuyHoldOpenInterestArray.size(); i++)
	{
		curOIObjPtr = FBuyHoldOpenInterestArray[i];
		delete curOIObjPtr;
	}
	FBuyHoldOpenInterestArray.clear();

	for (unsigned int i = 0; i < FSellHoldOpenInterestArray.size(); i++)
	{
		curOIObjPtr = FSellHoldOpenInterestArray[i];
		delete curOIObjPtr;
	}
	FSellHoldOpenInterestArray.clear();
}  //TPositionInstance::ClearHoldOpenInterestObject()

//---------------------------------------------------------------------------
void TPositionInstance::AppendHoldOpenInterestObjectToArray(nsOrderMessageDefine::SideEnum Side, double MatchPrice, int MatchQuantity)
{
	TOpenInterestObject *newOIPtr = new TOpenInterestObject(Side, MatchPrice, MatchQuantity, this);
	if (Side == nsOrderMessageDefine::sBuy)
		FBuyHoldOpenInterestArray.push_back(newOIPtr);
	else if (Side == nsOrderMessageDefine::sSell)
		FSellHoldOpenInterestArray.push_back(newOIPtr);
	else
		delete newOIPtr;
}  //TPositionInstance::AppendHoldOpenInterestObjectToArray()

//---------------------------------------------------------------------------
void TPositionInstance::PushOpenInterestObjectToQueue(TOpenInterestObject *OIPtr,
													  COpenInterestQueue *OpenInterestQueuePtr)
{
	if ((OIPtr == 0) || (OpenInterestQueuePtr == 0)) return;
	OpenInterestQueuePtr->push(OIPtr);
}  //TPositionInstance::PushOpenInterestObjectToQueue()

//---------------------------------------------------------------------------
TOpenInterestObject *TPositionInstance::PopOpenInterestObjectFromQueue(COpenInterestQueue *OpenInterestQueuePtr)
{
	TOpenInterestObject *OIPtr = 0;
	if ((OpenInterestQueuePtr == 0) || OpenInterestQueuePtr->empty()) return OIPtr;
	OIPtr = OpenInterestQueuePtr->front();
	OpenInterestQueuePtr->pop();
	return OIPtr;
}  //TPositionInstance::PopOpenInterestObjectFromQueue()

//---------------------------------------------------------------------------
void TPositionInstance::ClearOpenInterestQueue(COpenInterestQueue *OpenInterestQueuePtr)
{
	if ((OpenInterestQueuePtr == 0) || OpenInterestQueuePtr->empty()) return;
	while (!OpenInterestQueuePtr->empty())
	{
		TOpenInterestObject *OIPtr = PopOpenInterestObjectFromQueue(OpenInterestQueuePtr);
		if (OIPtr != 0) delete OIPtr;
	}  //while (OpenInterestQueuePtr->empty())
}  //TPositionInstance::ClearOpenInterestQueue()

//---------------------------------------------------------------------------
bool TPositionInstance::SumCloseQuantity(nsOrderMessageDefine::SideEnum Side,
										 double MatchPrice, int CloseQuantity)
{
	if (CloseQuantity <= 0) return false;
	TMatchPriceSum *MatchPriceSumPtr = 0;
	String PriceKey = TMatchPriceSum::GeneratePriceKey(MatchPrice);
	if (Side == nsOrderMessageDefine::sBuy)
	{
		MatchPriceSumPtr = FindBuyMatchPriceSumByPrice(PriceKey);
		FBuyCloseQuantitySum += CloseQuantity;
		FBuyCloseAmountSum += MatchPrice * CloseQuantity;
	}
	else if (Side == nsOrderMessageDefine::sSell)
	{
		MatchPriceSumPtr = FindSellMatchPriceSumByPrice(PriceKey);
		FSellCloseQuantitySum += CloseQuantity;
		FSellCloseAmountSum += MatchPrice * CloseQuantity;
	}

	if (MatchPriceSumPtr == 0) return false;
	MatchPriceSumPtr->AddCloseQuantity(CloseQuantity);
	return true;
}  //TPositionInstance::SumCloseQuantity()

//---------------------------------------------------------------------------
void TPositionInstance::SumFillData(nsOrderMessageDefine::SideEnum Side,
									double MatchPrice, int MatchQuantity, int CloseQuantity)
{
	String PriceKey = TMatchPriceSum::GeneratePriceKey(MatchPrice);
	TMatchPriceSum *MatchPriceSumPtr = 0;
	if (Side == nsOrderMessageDefine::sBuy)
	{
		MatchPriceSumPtr = FindBuyMatchPriceSumByPrice(PriceKey);
		FBuyQuantitySum += MatchQuantity;
		FBuyAmountSum += MatchPrice * MatchQuantity;
	}
	else if (Side == nsOrderMessageDefine::sSell)
	{
		MatchPriceSumPtr = FindSellMatchPriceSumByPrice(PriceKey);
		FSellQuantitySum += MatchQuantity;
		FSellAmountSum += MatchPrice * MatchQuantity;
	}
	else
		return;

	if (MatchPriceSumPtr == 0)
	{
		MatchPriceSumPtr = new TMatchPriceSum(Side, MatchPrice, MatchQuantity, this);
		bool IsInsertSuccess = false;
		if (Side == nsOrderMessageDefine::sBuy)
		{
			IsInsertSuccess = InsertBuyMatchPriceSumPriceIndex(PriceKey, MatchPriceSumPtr, false);
			if (IsInsertSuccess) PushToBuyMatchPriceSumArray(MatchPriceSumPtr);
		}
		else
		{
			IsInsertSuccess = InsertSellMatchPriceSumPriceIndex(PriceKey, MatchPriceSumPtr, false);
			if (IsInsertSuccess) PushToSellMatchPriceSumArray(MatchPriceSumPtr);
		}

		if (IsInsertSuccess)
			MatchPriceSumPtr->SetCloseQuantity(CloseQuantity);
		else
			delete MatchPriceSumPtr;
	}
	else
	{
		MatchPriceSumPtr->AddMatchQuantity(MatchQuantity);
		MatchPriceSumPtr->AddCloseQuantity(CloseQuantity);
	}
}  //TPositionInstance::SumFillData()
//---------------------------------------------------------------------------
void TPositionInstance::OffsetOpenQueuePosition(TOpenInterestObject *NewOIPtr,
												COpenInterestQueue *OpenInterestQueuePtr)
{
	if (((NewOIPtr == 0) || (NewOIPtr->GetOpenQuantity() <= 0)) ||
		((OpenInterestQueuePtr == 0) || OpenInterestQueuePtr->empty()))
		return;

	while (!OpenInterestQueuePtr->empty() && (NewOIPtr->GetOpenQuantity() > 0))
	{
		TOpenInterestObject *CurOIPtr = OpenInterestQueuePtr->front();
		int OrigCloseQuantity = CurOIPtr->GetCloseQuantity();
		int NewOpenQuantity = NewOIPtr->GetOpenQuantity();
		NewOIPtr->DoOffsetting(CurOIPtr->GetOpenQuantity());
		CurOIPtr->DoOffsetting(NewOpenQuantity);
		SumCloseQuantity(CurOIPtr->GetSide(), CurOIPtr->GetMatchPrice(), CurOIPtr->GetCloseQuantity() - OrigCloseQuantity);
		if (CurOIPtr->GetOpenQuantity() <= 0)
		{
			OpenInterestQueuePtr->pop();
			delete CurOIPtr;
		}  //if (CurOpenQuantity <= 0)
	}  //while (!OpenInterestQueuePtr->empty() && (NewOpenQuantity > 0))
}  //TPositionInstance::OffsetOpenQueuePosition()

//---------------------------------------------------------------------------
void TPositionInstance::OffsetPosition(nsOrderMessageDefine::SideEnum Side,
									   nsOrderMessageDefine::PositionEffectEnum PositionEffect,
									   double MatchPrice, int MatchQuantity)
{
	TOpenInterestObject *NewOIPtr = new TOpenInterestObject(Side, MatchPrice, MatchQuantity, this);
	COpenInterestQueue *OppositeSideDayTradeOpenQueuePtr = 0;
	COpenInterestQueue *OppositeSideOpenQueuePtr = 0;
	COpenInterestQueue *SameSideDayTradeOpenQueuePtr = 0;
	COpenInterestQueue *SameSideOpenQueuePtr = 0;
	if (Side == nsOrderMessageDefine::sBuy)
	{
		OppositeSideDayTradeOpenQueuePtr = &FSellDayTradeOpenQueue;
		OppositeSideOpenQueuePtr = &FSellOpenQueue;
		SameSideDayTradeOpenQueuePtr = &FBuyDayTradeOpenQueue;
		SameSideOpenQueuePtr = &FBuyOpenQueue;
	}
	else if (Side == nsOrderMessageDefine::sSell)
	{
		OppositeSideDayTradeOpenQueuePtr = &FBuyDayTradeOpenQueue;
		OppositeSideOpenQueuePtr = &FBuyOpenQueue;
		SameSideDayTradeOpenQueuePtr = &FSellDayTradeOpenQueue;
		SameSideOpenQueuePtr = &FSellOpenQueue;
	}
	else
	{
		AnsiString tempExchangeCode = FExchangeCode;
		AnsiString tempSymbolCode   = FSymbolCode;
		UFC::BufferedLog::Printf(" TPositionInstance::OffsetPosition() [%s %s] Side Error.",
								 tempExchangeCode.c_str(), tempSymbolCode.c_str());
		UFC::BufferedLog::FlushToFile();
		return;
	}  //if (Side == nsOrderMessageDefine::sBuy)

	OffsetOpenQueuePosition(NewOIPtr, OppositeSideDayTradeOpenQueuePtr);  //Offset Day Trade Open First
	if (NewOIPtr->GetOpenQuantity() > 0) OffsetOpenQueuePosition(NewOIPtr, OppositeSideOpenQueuePtr);
	SumFillData(Side, MatchPrice, MatchQuantity, NewOIPtr->GetCloseQuantity());
	if (NewOIPtr->GetOpenQuantity() > 0)
	{
		if (PositionEffect == nsOrderMessageDefine::peDayTradeOpen)
			PushOpenInterestObjectToQueue(NewOIPtr, SameSideDayTradeOpenQueuePtr);
		else
			PushOpenInterestObjectToQueue(NewOIPtr, SameSideOpenQueuePtr);
	}
	else
		delete NewOIPtr;
}  //TPositionInstance::OffsetPosition()

//---------------------------------------------------------------------------
void TPositionInstance::SetHoldPosition(double BuyHoldAveragePrice, int BuyHoldQuantity,
										double SellHoldAveragePrice, int SellHoldQuantity,
										double PriceMultiplier)
{
	ClearHoldOpenInterestObject();
	FBuyHoldAveragePrice = BuyHoldAveragePrice;
	FBuyHoldQuantity = BuyHoldQuantity;
	FBuyHoldAmount = BuyHoldAveragePrice * BuyHoldQuantity;
	if (BuyHoldQuantity > 0)
		AppendHoldOpenInterestObjectToArray(nsOrderMessageDefine::sBuy, BuyHoldAveragePrice, BuyHoldQuantity);

	FSellHoldAveragePrice = SellHoldAveragePrice;
	FSellHoldQuantity = SellHoldQuantity;
	FSellHoldAmount = SellHoldAveragePrice * SellHoldQuantity;
	if (SellHoldQuantity > 0)
		AppendHoldOpenInterestObjectToArray(nsOrderMessageDefine::sSell, SellHoldAveragePrice, SellHoldQuantity);

	FPriceMultiplier = PriceMultiplier;
}  //TPositionInstance::SetHoldPosition()

//---------------------------------------------------------------------------
void TPositionInstance::SetHoldPosition(int BuyHoldQuantity, double BuyHoldAmount, double BuyHoldAveragePrice,
										int SellHoldQuantity, double SellHoldAmount, double SellHoldAveragePrice,
										double PriceMultiplier)
{
	ClearHoldOpenInterestObject();
	FBuyHoldAveragePrice = BuyHoldAveragePrice;
	FBuyHoldQuantity = BuyHoldQuantity;
	FBuyHoldAmount = BuyHoldAmount;
	if (BuyHoldQuantity > 0)
		AppendHoldOpenInterestObjectToArray(nsOrderMessageDefine::sBuy, BuyHoldAveragePrice, BuyHoldQuantity);

	FSellHoldAveragePrice = SellHoldAveragePrice;
	FSellHoldQuantity = SellHoldQuantity;
	FSellHoldAmount = SellHoldAmount;
	if (SellHoldQuantity > 0)
		AppendHoldOpenInterestObjectToArray(nsOrderMessageDefine::sSell, SellHoldAveragePrice, SellHoldQuantity);

	FPriceMultiplier = PriceMultiplier;
}  //TPositionInstance::SetHoldPosition()

//---------------------------------------------------------------------------
void TPositionInstance::AddHoldPosition(int BuyHoldQuantity, double BuyHoldAveragePrice,
										int SellHoldQuantity, double SellHoldAveragePrice,
										double PriceMultiplier)
{
	if (BuyHoldQuantity > 0)
	{
		FBuyHoldQuantity = FBuyHoldQuantity + BuyHoldQuantity;
		FBuyHoldAmount = FBuyHoldAmount + (static_cast<double>(BuyHoldQuantity) * BuyHoldAveragePrice);
		if (FBuyHoldQuantity > 0)
			FBuyHoldAveragePrice = FBuyHoldAmount / static_cast<double>(FBuyHoldQuantity);
		else
			FBuyHoldAveragePrice = 0.0;
		AppendHoldOpenInterestObjectToArray(nsOrderMessageDefine::sBuy, BuyHoldAveragePrice, BuyHoldQuantity);
	}

	if (SellHoldQuantity > 0)
	{
		FSellHoldQuantity = FSellHoldQuantity + SellHoldQuantity;
		FSellHoldAmount = FSellHoldAmount + (static_cast<double>(SellHoldQuantity) * SellHoldAveragePrice);
		if (FSellHoldQuantity > 0)
			FSellHoldAveragePrice = FSellHoldAmount / static_cast<double>(FSellHoldQuantity);
		else
			FSellHoldAveragePrice = 0.0;
		AppendHoldOpenInterestObjectToArray(nsOrderMessageDefine::sSell, SellHoldAveragePrice, SellHoldQuantity);
	}

	FPriceMultiplier = PriceMultiplier;
}  //TPositionInstance::AddHoldPosition()

//---------------------------------------------------------------------------
bool TPositionInstance::AddNewFillMessage(const String& OrderId, const String& ExecId,
										  nsOrderMessageDefine::SideEnum Side,
										  nsOrderMessageDefine::PositionEffectEnum PositionEffect,
										  double MatchPrice, int MatchQuantity)
{
	bool IsSuccess = false;
	if ((ExecId.Length() <= 0) || (OrderId.Length() <= 0)) return IsSuccess;

	//Add New Match Message
	TMatchMessageData *ExistMatchMsgPtr = FindMatchMessageByExecId(ExecId);
	if (ExistMatchMsgPtr != 0) return IsSuccess;
	TMatchMessageData *NewMatchMsgPtr =
		new TMatchMessageData(OrderId, ExecId, Side, PositionEffect, MatchPrice, MatchQuantity, this);
	if (InsertMatchMessageExecIdIndex(ExecId, NewMatchMsgPtr, false))
	{
		PushToMatchMessageArray(NewMatchMsgPtr);
		FNewestPrice = MatchPrice;
		OffsetPosition(Side, PositionEffect, MatchPrice, MatchQuantity);
		IsSuccess = true;
	}
	else
	{
		AnsiString tempOrderId = OrderId;
		AnsiString tempExecId  = ExecId;
		UFC::BufferedLog::Printf(" TPositionInstance::AddNewFillMessage() Insert OrderId[%s] ExecId[%s] Match Message Error.",
								 tempOrderId.c_str(), tempExecId.c_str());
		UFC::BufferedLog::FlushToFile();
		delete NewMatchMsgPtr;
	}

	return IsSuccess;
}  //TPositionInstance::AddNewFillMessage()

//---------------------------------------------------------------------------
void TPositionInstance::CalculateData()
{
	FTax = 0.0;
	FFee = 0.0;
	FBuyQuantitySum = 0;
	FBuyAmountSum = 0.0;
	FBuyCloseQuantitySum = 0;
	FBuyCloseAmountSum = 0.0;
	FBuyAveragePrice = 0.0;
	FSellQuantitySum = 0;
	FSellAmountSum = 0.0;
	FSellCloseQuantitySum = 0;
	FSellCloseAmountSum = 0.0;
	FSellAveragePrice = 0.0;
	FCloseProfit = 0.0;
	CStringMatchPriceSumMap::iterator mpSumIt;

	//Process Buy Side
	for (mpSumIt = FBuyMatchPriceSumPriceIndex.begin(); mpSumIt != FBuyMatchPriceSumPriceIndex.end(); mpSumIt++)
	{
		TMatchPriceSum *MatchPriceSumPtr = mpSumIt->second;
		FBuyQuantitySum += MatchPriceSumPtr->GetMatchQuantity();
		FBuyAmountSum += MatchPriceSumPtr->GetMatchAmount();
		FBuyCloseQuantitySum += MatchPriceSumPtr->GetCloseQuantity();
		FBuyCloseAmountSum += MatchPriceSumPtr->GetCloseAmount();
		int TempTaxValue = (MatchPriceSumPtr->GetMatchPrice() * FPriceMultiplier * FTaxRate) + 0.5;
		FTax += TempTaxValue * MatchPriceSumPtr->GetMatchQuantity();
	}
	if (FBuyQuantitySum > 0) FBuyAveragePrice = FBuyAmountSum / FBuyQuantitySum;

	//Process Sell Side
	for (mpSumIt = FSellMatchPriceSumPriceIndex.begin(); mpSumIt != FSellMatchPriceSumPriceIndex.end(); mpSumIt++)
	{
		TMatchPriceSum *MatchPriceSumPtr = mpSumIt->second;
		FSellQuantitySum += MatchPriceSumPtr->GetMatchQuantity();
		FSellAmountSum += MatchPriceSumPtr->GetMatchAmount();
		FSellCloseQuantitySum += MatchPriceSumPtr->GetCloseQuantity();
		FSellCloseAmountSum += MatchPriceSumPtr->GetCloseAmount();
		int TempTaxValue = (MatchPriceSumPtr->GetMatchPrice() * FPriceMultiplier * FTaxRate) + 0.5;
		FTax += TempTaxValue * MatchPriceSumPtr->GetMatchQuantity();
	}
	if (FSellQuantitySum > 0) FSellAveragePrice = FSellAmountSum / FSellQuantitySum;

	FFee = FUnitFee * (FBuyQuantitySum + FSellQuantitySum);
	FCloseProfit = (FSellCloseAmountSum - FBuyCloseAmountSum) * FPriceMultiplier;
}  //TPositionInstance::CalculateData()

//---------------------------------------------------------------------------
void TPositionInstance::ReOffsetPosition()
{
	ClearOpenInterestQueue(&FBuyDayTradeOpenQueue);
	ClearOpenInterestQueue(&FBuyOpenQueue);
	ClearBuyMatchPriceSum();
	ClearOpenInterestQueue(&FSellDayTradeOpenQueue);
	ClearOpenInterestQueue(&FSellOpenQueue);
	ClearSellMatchPriceSum();

	//Add Buy Side Hold Position To Open Queue
	for (unsigned int i = 0; i < FBuyHoldOpenInterestArray.size(); i++)
	{
		TOpenInterestObject *curOIPtr = FBuyHoldOpenInterestArray[i];
		OffsetPosition(curOIPtr->GetSide(), nsOrderMessageDefine::peOpen,
					   curOIPtr->GetMatchPrice(), curOIPtr->GetMatchQuantity());
	}

	for (unsigned int i = 0; i < FSellHoldOpenInterestArray.size(); i++)
	{
		TOpenInterestObject *curOIPtr = FSellHoldOpenInterestArray[i];
		OffsetPosition(curOIPtr->GetSide(), nsOrderMessageDefine::peOpen,
					   curOIPtr->GetMatchPrice(), curOIPtr->GetMatchQuantity());
	}

	for (int MatchMsgIndex = 0; MatchMsgIndex < GetMatchMessageCount(); MatchMsgIndex++)
	{
		TMatchMessageData *MatchMsgPtr = GetMatchMessageByIndex(MatchMsgIndex);
		OffsetPosition(MatchMsgPtr->GetSide(), MatchMsgPtr->GetPositionEffect(),
					   MatchMsgPtr->GetMatchPrice(), MatchMsgPtr->GetMatchQuantity());
	}  //for (int MatchMsgIndex = 0; MatchMsgIndex < GetMatchMessageCount(); MatchMsgIndex++)
}  //TPositionInstance::ReOffsetPosition()

