//---------------------------------------------------------------------------
#include "PositionObjects.h"
#pragma hdrstop
#include "UFC.h"
#include "TradingObjects.h"
#include "TradingCommodities.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

//-------------------------- TBasicPositionObject ---------------------------
//---------------------------------------------------------------------------
TBasicPositionObject::TBasicPositionObject()
:FExchangeCode(L""),
 FSymbolCode(L"")
{
}  //TBasicPositionObject::TBasicPositionObject()
//---------------------------------------------------------------------------
TBasicPositionObject::TBasicPositionObject(const String& ExchangeCode, const String& SymbolCode)
:FExchangeCode(ExchangeCode),
 FSymbolCode(SymbolCode)
{

}  //TBasicPositionObject::TBasicPositionObject()

//---------------------------------------------------------------------------
int GetBasicPositionObjQueueCount(TBasicPositionObjectQueue& BasicPositionObjQueue)
{
	return static_cast<int>(BasicPositionObjQueue.size());
}  //GetBasicPositionObjQueueCount()
//---------------------------------------------------------------------------
TBasicPositionObject *GetFirstBasicPositionObjFromQueue(TBasicPositionObjectQueue& BasicPositionObjQueue)
{
	TBasicPositionObject *positionObjPtr = 0;
	if (!BasicPositionObjQueue.empty()) positionObjPtr = BasicPositionObjQueue.front();
	return positionObjPtr;
}  //GetFirstBasicPositionObjFromQueue()
//---------------------------------------------------------------------------
TBasicPositionObject *PopBasicPositionObjQueue(TBasicPositionObjectQueue& BasicPositionObjQueue)
{
	TBasicPositionObject *positionObjPtr = 0;
	if (!BasicPositionObjQueue.empty())
	{
		positionObjPtr = BasicPositionObjQueue.front();
		BasicPositionObjQueue.pop();
	}
	return positionObjPtr;
}  //PopBasicPositionObjQueue()
//---------------------------------------------------------------------------
bool PushBasicPositionObjQueue(TBasicPositionObjectQueue& BasicPositionObjQueue, TBasicPositionObject *PositionObjPtr)
{
	if (PositionObjPtr == 0) return false;
	BasicPositionObjQueue.push(PositionObjPtr);
	return true;
}  //PushBasicPositionObjQueue()
//---------------------------------------------------------------------------
void ClearBasicPositionObjQueue(TBasicPositionObjectQueue& BasicPositionObjQueue, bool DoDelete)
{
	while(!BasicPositionObjQueue.empty())
	{
		TBasicPositionObject *positionObjPtr = BasicPositionObjQueue.front();
		if (DoDelete && (positionObjPtr != 0)) delete positionObjPtr;
		BasicPositionObjQueue.pop();
	}
}  //ClearBasicPositionObjQueue()

//---------------------------------------------------------------------------
int GetBasicPositionObjArrayCount(TBasicPositionObjectVector& BasicPositionObjArray)
{
	return static_cast<int>(BasicPositionObjArray.size());
}  //GetBasicPositionObjKeyIndexCount()
//---------------------------------------------------------------------------
TBasicPositionObject *GetBasicPositionObjFromArray(TBasicPositionObjectVector& BasicPositionObjArray, int ArrayIndex)
{
	TBasicPositionObject *positionObjPtr = 0;
	if ((ArrayIndex >= 0) && (ArrayIndex <= static_cast<int>(BasicPositionObjArray.size())))
		positionObjPtr = BasicPositionObjArray[ArrayIndex];
	return positionObjPtr;
}  //GetBasicPositionObjFromArray()
//---------------------------------------------------------------------------
void AppendBasicPositionObjectArray(TBasicPositionObjectVector& BasicPositionObjArray, TBasicPositionObject *PositionObjPtr)
{
	if (PositionObjPtr == 0) return;
	BasicPositionObjArray.push_back(PositionObjPtr);
}  //AppendBasicPositionObjectArray()
//---------------------------------------------------------------------------
void ClearBasicPositionObjectArray(TBasicPositionObjectVector& BasicPositionObjArray, bool DoDelete)
{
	if (DoDelete)
	{
		for (unsigned int i = 0; i < BasicPositionObjArray.size(); i++)
		{
			TBasicPositionObject *positionObjPtr = BasicPositionObjArray[i];
			if (positionObjPtr != 0) delete positionObjPtr;
		}
	}
	BasicPositionObjArray.clear();
}  //AppendBasicPositionObjectToArray()

//---------------------------------------------------------------------------
int GetBasicPositionObjKeyIndexCount(TBasicPositionObjectMap& BasicPositionObjIndex)
{
	return static_cast<int>(BasicPositionObjIndex.size());
}  //GetBasicPositionObjKeyIndexCount()
//---------------------------------------------------------------------------
TBasicPositionObject* FindBasicPositionObjByKey(TBasicPositionObjectMap& BasicPositionObjIndex, const String& KeyStr)
{
	TBasicPositionObject *positionObjPtr = 0;
	if ((BasicPositionObjIndex.size() <= 0) || (KeyStr.Length() <= 0)) return positionObjPtr;
	TBasicPositionObjectMap::iterator it = BasicPositionObjIndex.find(KeyStr);
	if (it != BasicPositionObjIndex.end()) positionObjPtr = it->second;
	return positionObjPtr;
}  //FindBasicPositionObjByKey()
//---------------------------------------------------------------------------
bool InsertBasicPositionObjKeyIndex(TBasicPositionObjectMap& BasicPositionObjIndex, const String& KeyStr, TBasicPositionObject *PositionObjPtr, bool DoCheck)
{
	bool isSuccess = false;
	if ((KeyStr.Length() <= 0) || (PositionObjPtr == 0)) return isSuccess;
	if (DoCheck && (FindBasicPositionObjByKey(BasicPositionObjIndex, KeyStr) != 0)) return isSuccess;
	std::pair<TBasicPositionObjectMap::iterator, bool> insertPair =
		BasicPositionObjIndex.insert(TBasicPositionObjectMap::value_type(KeyStr, PositionObjPtr));
	isSuccess = insertPair.second;
	return isSuccess;
}  //InsertBasicPositionObjKeyIndex()
//---------------------------------------------------------------------------
TBasicPositionObject *RemoveBasicPositionObjectKeyIndex(TBasicPositionObjectMap& BasicPositionObjIndex, const String& KeyStr)
{
	TBasicPositionObject *positionObjPtr = 0;
	if (KeyStr.Length() <= 0) return positionObjPtr;
	TBasicPositionObjectMap::iterator it = BasicPositionObjIndex.find(KeyStr);
	if (it != BasicPositionObjIndex.end())
	{
		positionObjPtr = it->second;
		BasicPositionObjIndex.erase(it);
	}
	return positionObjPtr;
}  //RemoveBasicPositionObjectKeyIndex()
//---------------------------------------------------------------------------
void ClearBasicPositionObjectKeyIndex(TBasicPositionObjectMap& BasicPositionObjIndex, bool DoDelete)
{
	if (DoDelete)
	{
		for (TBasicPositionObjectMap::iterator it = BasicPositionObjIndex.begin(); it != BasicPositionObjIndex.end(); it++)
		{
			TBasicPositionObject *positionObjPtr = it->second;
			if (positionObjPtr != 0) delete positionObjPtr;
		}
	}
	BasicPositionObjIndex.clear();
}  //ClearBasicPositionObjectKeyIndex()
//---------------------------------------------------------------------------
int CopyBasicPositionObjFromIndexToArray(TBasicPositionObjectMap& BasicPositionObjIndex, TBasicPositionObjectVector& BasicPositionObjArray, bool DoClearArray)
{
	if (DoClearArray) ClearBasicPositionObjectArray(BasicPositionObjArray, false);  //Do not Delete Object
	int copyCount = 0;
	for (TBasicPositionObjectMap::iterator it = BasicPositionObjIndex.begin(); it != BasicPositionObjIndex.end(); it++)
	{
		TBasicPositionObject *positionObjPtr = it->second;
		AppendBasicPositionObjectArray(BasicPositionObjArray, positionObjPtr);
		copyCount++;
	}
	return copyCount;
}  //CopyBasicPositionObjFromIndexToArray()

//------------------------------ TMatchRecord -------------------------------
//---------------------------------------------------------------------------
TMatchRecord::TMatchRecord()
:TBasicPositionObject(),
 FOrderID(L""),
 FExecID(L""),
 FSide(nsOrderMessageDefine::sNone),
 FPositionEffect(nsOrderMessageDefine::peAuto),
 FMatchPrice(0.0),
 FMatchQuantity(0),
 FMatchAmount(0.0),
 FFeeRateByVolume(0.0),
 FFeeRateByPrice(0.0),
 FPositionRecordPtr(0)
{
}  //TMatchRecord::TMatchRecord()
//---------------------------------------------------------------------------
TMatchRecord::TMatchRecord(const String& ExchangeCode, const String& SymbolCode,
						   const String& OrderID, const String& ExecID,
						   nsOrderMessageDefine::SideEnum Side,
						   nsOrderMessageDefine::PositionEffectEnum PositionEffect,
						   double MatchPrice, int MatchQuantity,
						   double FeeRateByVolume, double FeeRateByPrice,
						   TPositionRecord *PositionRecordPtr)
:TBasicPositionObject(ExchangeCode, SymbolCode),
 FOrderID(OrderID),
 FExecID(ExecID),
 FSide(Side),
 FPositionEffect(PositionEffect),
 FMatchPrice(MatchPrice),
 FMatchQuantity(MatchQuantity),
 FMatchAmount(0.0),
 FFeeRateByVolume(FeeRateByVolume),
 FFeeRateByPrice(FeeRateByPrice),
 FPositionRecordPtr(PositionRecordPtr)
{
	FMatchAmount = FMatchPrice * FMatchQuantity;
}  //TMatchRecord::TMatchRecord()

//--------------------------- TOpenInterestRecord ---------------------------
//---------------------------------------------------------------------------
TOpenInterestRecord::TOpenInterestRecord()
:TBasicPositionObject(),
 FSide(nsOrderMessageDefine::sNone),
 FMatchPrice(0.0),
 FMatchQuantity(0),
 FOpenQuantity(0),
 FCloseQuantity(0),
 FPositionRecordPtr(0)
{
}  //TOpenInterestRecord::TOpenInterestRecord()
//---------------------------------------------------------------------------
TOpenInterestRecord::TOpenInterestRecord(const String& ExchangeCode, const String& SymbolCode,
										 nsOrderMessageDefine::SideEnum Side,
										 double MatchPrice, int MatchQuantity,
										 TPositionRecord *PositionRecordPtr)
:TBasicPositionObject(ExchangeCode, SymbolCode),
 FSide(Side),
 FMatchPrice(MatchPrice),
 FMatchQuantity(MatchQuantity),
 FOpenQuantity(MatchQuantity),
 FCloseQuantity(0),
 FPositionRecordPtr(PositionRecordPtr)
{
}  //TOpenInterestRecord::TOpenInterestRecord()
//---------------------------------------------------------------------------
int TOpenInterestRecord::DoOffsetting(int CoverQuantity)
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
}  //TOpenInterestRecord::DoOffsetting()
//---------------------------------------------------------------------------
void TOpenInterestRecord::DumpData(const String& PrefixMsg)
{
	String sideName = L"";
	if (FSide == nsOrderMessageDefine::sBuy)
		sideName = L"B";
	else if (FSide == nsOrderMessageDefine::sSell)
		sideName = L"S";
	else
		sideName = L"N/A";

	String dumpMsg = L"";
	if (PrefixMsg.Length() > 0)
		dumpMsg.printf(L"%s[%s %s] %s Px=%12.5lf  MQty=%d  OI=%d  C=%d",
					   PrefixMsg, FExchangeCode, FSymbolCode, sideName,
					   FMatchPrice, FMatchQuantity, FOpenQuantity, FCloseQuantity);
	else
		dumpMsg.printf(L"[%s %s] %s Px=%12.5lf  MQty=%d  OI=%d  C=%d",
					   FExchangeCode, FSymbolCode, sideName,
					   FMatchPrice, FMatchQuantity, FOpenQuantity, FCloseQuantity);
	AnsiString ansiDumpMsg = s888::ConvertUnicodeToAnsiStr(dumpMsg);
	UFC::BufferedLog::Printf(" %s", ansiDumpMsg.c_str());
	UFC::BufferedLog::FlushToFile();
}  //TOpenInterestRecord::DumpData()

//--------------------------- TMatchPriceSumRecord --------------------------
//---------------------------------------------------------------------------
String TMatchPriceSumRecord::GeneratePriceKey(double Price)
{
	String priceKey;
	priceKey.printf(L"%13.6lf", Price);
	return priceKey;
}  //TMatchPriceSumRecord::GeneratePriceKey()

//---------------------------------------------------------------------------
TMatchPriceSumRecord::TMatchPriceSumRecord()
:TBasicPositionObject(),
 FSide(nsOrderMessageDefine::sNone),
 FMatchPrice(0.0),
 FMatchQuantity(0),
 FMatchAmount(0.0),
 FCloseQuantity(0),
 FCloseAmount(0.0),
 FPositionRecordPtr(0),
 FPriceKey(L"")
{
}  //TMatchPriceSumRecord::TMatchPriceSumRecord()
//---------------------------------------------------------------------------
TMatchPriceSumRecord::TMatchPriceSumRecord(const String& ExchangeCode, const String& SymbolCode,
										   nsOrderMessageDefine::SideEnum Side,
										   double MatchPrice, int MatchQuantity,
										   TPositionRecord *PositionRecordPtr)
:TBasicPositionObject(ExchangeCode, SymbolCode),
 FSide(Side),
 FMatchPrice(MatchPrice),
 FMatchQuantity(MatchQuantity),
 FMatchAmount(0.0),
 FCloseQuantity(0),
 FCloseAmount(0.0),
 FPositionRecordPtr(PositionRecordPtr),
 FPriceKey(L"")
{
	FPriceKey = GeneratePriceKey(MatchPrice);
	CalculateMatchAmount();
}  //TMatchPriceSumRecord::TMatchPriceSumRecord()
//---------------------------------------------------------------------------
void TMatchPriceSumRecord::CalculateAmount()
{
	CalculateMatchAmount();
	CalculateCloseAmount();
}  //TMatchPriceSumRecord::CalculateAmount()
//---------------------------------------------------------------------------
void TMatchPriceSumRecord::SetMatchQuantity(int MatchQuantity)
{
	FMatchQuantity = MatchQuantity;
	CalculateMatchAmount();
}  //TMatchPriceSumRecord::SetMatchQuantity()
//---------------------------------------------------------------------------
void TMatchPriceSumRecord::AddMatchQuantity(int MatchQuantity)
{
	FMatchQuantity += MatchQuantity;
	CalculateMatchAmount();
}  //TMatchPriceSumRecord::AddMatchQuantity()
//---------------------------------------------------------------------------
void TMatchPriceSumRecord::SetCloseQuantity(int CloseQuantity)
{
	FCloseQuantity = CloseQuantity;
	CalculateCloseAmount();
}  //TMatchPriceSumRecord::SetCloseQuantity()
//---------------------------------------------------------------------------
void TMatchPriceSumRecord::AddCloseQuantity(int CloseQuantity)
{
	FCloseQuantity += CloseQuantity;
	CalculateCloseAmount();
}  //TMatchPriceSumRecord::AddCloseQuantity()
//---------------------------------------------------------------------------
void TMatchPriceSumRecord::DumpData(const String& PrefixMsg)
{
	String sideName = L"";
	if (FSide == nsOrderMessageDefine::sBuy)
		sideName = L"B";
	else if (FSide == nsOrderMessageDefine::sSell)
		sideName = L"S";
	else
		sideName = L"N/A";

	String dumpMsg = L"";
	if (PrefixMsg.Length() > 0)
		dumpMsg.printf(L"%s[%s %s] %s Px=%13.6lf  Qty=%d  O=%d  C=%d",
					   PrefixMsg, FExchangeCode, FSymbolCode, sideName,
					   FMatchPrice, FMatchQuantity, FMatchQuantity - FCloseQuantity, FCloseQuantity);
	else
		dumpMsg.printf(L"[%s %s] %s Px=%13.6lf  Qty=%d  O=%d  C=%d",
					   FExchangeCode, FSymbolCode, sideName,
					   FMatchPrice, FMatchQuantity, FMatchQuantity - FCloseQuantity, FCloseQuantity);
	AnsiString ansiDumpMsg = s888::ConvertUnicodeToAnsiStr(dumpMsg);
	UFC::BufferedLog::Printf(" %s", ansiDumpMsg.c_str());
	UFC::BufferedLog::FlushToFile();
}  //TMatchPriceSumRecord::DumpData()

//----------------------------- TPositionRecord -----------------------------
//---------------------------------------------------------------------------
TPositionRecord::TPositionRecord()
:TBasicPositionObject(),
 FCommodityKey(L""),
 FPriceMultiplier(1.0),
 FTaxRate(0.0),
 FUnitFee(0.0),
 FNewestPrice(0.0),
 FBuyQuantity1(0),
 FBuyPrice1(0.0),
 FSellQuantity1(0),
 FSellPrice1(0.0),
 FBuyHoldQuantity(0),
 FBuyHoldAmount(0.0),
 FBuyHoldAveragePrice(0.0),
 FSellHoldQuantity(0),
 FSellHoldAmount(0.0),
 FSellHoldAveragePrice(0.0),
 FBuyQuantitySum(0),
 FBuyAmountSum(0.0),
 FBuyCloseQuantitySum(0),
 FBuyCloseAmountSum(0.0),
 FSellQuantitySum(0),
 FSellAmountSum(0.0),
 FSellCloseQuantitySum(0),
 FSellCloseAmountSum(0.0),
 FTax(0.0),
 FFee(0.0),
 FBuyAveragePrice(0.0),
 FSellAveragePrice(0.0),
 FCloseProfit(0.0),
 FTradeQuantitySum(0),
 FFloatingProfit(0.0),
 FLastTickCount(0),
 FIsWaiting(false),
 FIsSubscribed(false)
{
}  //TPositionRecord::TPositionRecord()
//---------------------------------------------------------------------------
TPositionRecord::TPositionRecord(const String& ExchangeCode, const String& SymbolCode)
:TBasicPositionObject(ExchangeCode, SymbolCode),
 FCommodityKey(L""),
 FPriceMultiplier(1.0),
 FTaxRate(0.0),
 FUnitFee(0.0),
 FNewestPrice(0.0),
 FBuyHoldQuantity(0),
 FBuyHoldAmount(0.0),
 FBuyHoldAveragePrice(0.0),
 FSellHoldQuantity(0),
 FSellHoldAmount(0.0),
 FSellHoldAveragePrice(0.0),
 FBuyQuantitySum(0),
 FBuyAmountSum(0.0),
 FBuyCloseQuantitySum(0),
 FBuyCloseAmountSum(0.0),
 FBuyPEOpenQuantitySum(0),
 FBuyPECloseQuantitySum(0),
 FSellQuantitySum(0),
 FSellAmountSum(0.0),
 FSellCloseQuantitySum(0),
 FSellCloseAmountSum(0.0),
 FSellPEOpenQuantitySum(0),
 FSellPECloseQuantitySum(0),
 FTax(0.0),
 FFee(0.0),
 FBuyAveragePrice(0.0),
 FSellAveragePrice(0.0),
 FCloseProfit(0.0),
 FTradeQuantitySum(0),
 FFloatingProfit(0.0),
 FLastTickCount(0),
 FIsWaiting(false),
 FIsSubscribed(false)
{
	FCommodityKey = FExchangeCode + FSymbolCode;
}  //TPositionRecord::TPositionRecord()
//---------------------------------------------------------------------------
TPositionRecord::~TPositionRecord()
{
	ClearHoldPosition();
	ClearMatchRec();
	ClearBasicPositionObjQueue(FBuyDayTradeOIQueue, true);
	ClearBasicPositionObjQueue(FBuyHoldOIQueue, true);
	ClearBasicPositionObjQueue(FBuyOIQueue, true);
	ClearBasicPositionObjQueue(FSellDayTradeOIQueue, true);
	ClearBasicPositionObjQueue(FSellHoldOIQueue, true);
	ClearBasicPositionObjQueue(FSellOIQueue, true);
	ClearBuyMatchPxSumRec();
	ClearSellMatchPxSumRec();
}  //TPositionRecord::~TPositionRecord()
//---------------------------------------------------------------------------
TMatchRecord *TPositionRecord::FindMatchRecByExecID(const String& ExecID)
{
	TBasicPositionObject *positionObjectPtr = FindBasicPositionObjByKey(FMatchRecExecIDIndex, ExecID);
	return static_cast<TMatchRecord*>(positionObjectPtr);
}  //TPositionRecord::FindMatchRecByExecID()
//---------------------------------------------------------------------------
bool TPositionRecord::InsertMatchRecExecIDIndex(const String& ExecID, TMatchRecord *MatchRecPtr, bool DoCheck)
{
	return InsertBasicPositionObjKeyIndex(FMatchRecExecIDIndex, ExecID, MatchRecPtr, DoCheck);
}  //TPositionRecord::InsertMatchRecExecIDIndex()
//---------------------------------------------------------------------------
bool TPositionRecord::InsertMatchRecExecIDIndex(TMatchRecord *MatchRecPtr, bool DoCheck)
{
	if (MatchRecPtr == 0) return false;
	String keyStr = MatchRecPtr->GetExecID();
	return InsertBasicPositionObjKeyIndex(FMatchRecExecIDIndex, keyStr, MatchRecPtr, DoCheck);
}  //TPositionRecord::InsertMatchRecExecIDIndex()
//---------------------------------------------------------------------------
TMatchRecord *TPositionRecord::GetMatchRecFromArray(int ArrayIndex)
{
	TBasicPositionObject *positionObjectPtr = GetBasicPositionObjFromArray(FMatchRecArray, ArrayIndex);
	return static_cast<TMatchRecord*>(positionObjectPtr);
}  //TPositionRecord::GetMatchRecFromArray()
//---------------------------------------------------------------------------
void TPositionRecord::AppendMatchRecArray(TMatchRecord *MatchRecPtr)
{
	AppendBasicPositionObjectArray(FMatchRecArray, MatchRecPtr);
}  //TPositionRecord::AppendMatchRecArray()
//---------------------------------------------------------------------------
void TPositionRecord::ClearMatchRec()
{
	ClearBasicPositionObjectKeyIndex(FMatchRecExecIDIndex, true);
	ClearBasicPositionObjectArray(FMatchRecArray, false);
}  //TPositionRecord::ClearMatchRec()
//---------------------------------------------------------------------------
TMatchPriceSumRecord *TPositionRecord::FindBuyMatchPxSumRecByPrice(const String& PriceKey)
{
	TBasicPositionObject *positionObjectPtr = FindBasicPositionObjByKey(FBuyMatchPxSumRecPriceIndex, PriceKey);
	return static_cast<TMatchPriceSumRecord*>(positionObjectPtr);
}  //TPositionRecord::FindBuyMatchPxSumRecByPrice()
//---------------------------------------------------------------------------
TMatchPriceSumRecord *TPositionRecord::FindBuyMatchPxSumRecByPrice(double MatchPrice)
{
	String priceKey = TMatchPriceSumRecord::GeneratePriceKey(MatchPrice);
	return FindBuyMatchPxSumRecByPrice(priceKey);
}  //TPositionRecord::FindBuyMatchPxSumRecByPrice()
//---------------------------------------------------------------------------
bool TPositionRecord::InsertBuyMatchPxSumRecPriceIndex(const String& PriceKey,
													   TMatchPriceSumRecord *MatchPxSumPtr, bool DoCheck)
{
	bool isInsertSuccess = InsertBasicPositionObjKeyIndex(FBuyMatchPxSumRecPriceIndex, PriceKey, MatchPxSumPtr, DoCheck);
	if (isInsertSuccess) CopyBasicPositionObjFromIndexToArray(FBuyMatchPxSumRecPriceIndex, FBuyMatchPxSumRecArray, true);  //Clear Array
	return isInsertSuccess;
}  //TPositionRecord::InsertBuyMatchPxSumRecPriceIndex()
//---------------------------------------------------------------------------
bool TPositionRecord::InsertBuyMatchPxSumRecPriceIndex(double MatchPrice,
													   TMatchPriceSumRecord *MatchPxSumPtr, bool DoCheck)
{
	String priceKey = TMatchPriceSumRecord::GeneratePriceKey(MatchPrice);
	return InsertBuyMatchPxSumRecPriceIndex(priceKey, MatchPxSumPtr, DoCheck);
}  //TPositionRecord::InsertBuyMatchPxSumRecPriceIndex()
//---------------------------------------------------------------------------
bool TPositionRecord::InsertBuyMatchPxSumRecPriceIndex(TMatchPriceSumRecord *MatchPxSumPtr, bool DoCheck)
{
	if (MatchPxSumPtr == 0) return false;
	String priceKey = MatchPxSumPtr->GetPriceKey();
	return InsertBuyMatchPxSumRecPriceIndex(priceKey, MatchPxSumPtr, DoCheck);
}  //TPositionRecord::InsertBuyMatchPxSumRecPriceIndex()
//---------------------------------------------------------------------------
TMatchPriceSumRecord *TPositionRecord::GetBuyMatchPxSumRecFromArray(int ArrayIndex)
{
	TBasicPositionObject *positionObjectPtr = GetBasicPositionObjFromArray(FBuyMatchPxSumRecArray, ArrayIndex);
	return static_cast<TMatchPriceSumRecord*>(positionObjectPtr);
}  //TPositionRecord::GetBuyMatchPxSumRecFromArray()
//---------------------------------------------------------------------------
void TPositionRecord::ClearBuyMatchPxSumRec()
{
	FBuyQuantitySum = 0;
	FBuyAmountSum = 0.0;
	FBuyCloseQuantitySum = 0;
	FBuyCloseAmountSum = 0.0;
	ClearBasicPositionObjectArray(FBuyMatchPxSumRecArray, false);  //Do not Delete Object
	ClearBasicPositionObjectKeyIndex(FBuyMatchPxSumRecPriceIndex, true);  //Delete Object
}  //TPositionRecord::ClearBuyMatchPxSumRec()
//---------------------------------------------------------------------------
TMatchPriceSumRecord *TPositionRecord::FindSellMatchPxSumRecByPrice(const String& PriceKey)
{
	TBasicPositionObject *positionObjectPtr = FindBasicPositionObjByKey(FSellMatchPxSumRecPriceIndex, PriceKey);
	return static_cast<TMatchPriceSumRecord*>(positionObjectPtr);
}  //TPositionRecord::FindSellMatchPxSumRecByPrice()
//---------------------------------------------------------------------------
TMatchPriceSumRecord *TPositionRecord::FindSellMatchPxSumRecByPrice(double MatchPrice)
{
	String priceKey = TMatchPriceSumRecord::GeneratePriceKey(MatchPrice);
	return FindSellMatchPxSumRecByPrice(priceKey);
}  //TPositionRecord::FindSellMatchPxSumRecByPrice()
//---------------------------------------------------------------------------
bool TPositionRecord::InsertSellMatchPxSumRecPriceIndex(const String& PriceKey,
														TMatchPriceSumRecord *MatchPxSumPtr, bool DoCheck)
{
	bool isInsertSuccess = InsertBasicPositionObjKeyIndex(FSellMatchPxSumRecPriceIndex, PriceKey, MatchPxSumPtr, DoCheck);
	if (isInsertSuccess) CopyBasicPositionObjFromIndexToArray(FSellMatchPxSumRecPriceIndex, FSellMatchPxSumRecArray, true);  //Clear Array
	return isInsertSuccess;
}  //TPositionRecord::InsertSellMatchPxSumRecPriceIndex()
//---------------------------------------------------------------------------
bool TPositionRecord::InsertSellMatchPxSumRecPriceIndex(double MatchPrice,
														TMatchPriceSumRecord *MatchPxSumPtr, bool DoCheck)
{
	String priceKey = TMatchPriceSumRecord::GeneratePriceKey(MatchPrice);
	return InsertSellMatchPxSumRecPriceIndex(priceKey, MatchPxSumPtr, DoCheck);
}  //TPositionRecord::InsertSellMatchPxSumRecPriceIndex()
//---------------------------------------------------------------------------
bool TPositionRecord::InsertSellMatchPxSumRecPriceIndex(TMatchPriceSumRecord *MatchPxSumPtr, bool DoCheck)
{
	if (MatchPxSumPtr == 0) return false;
	String priceKey = MatchPxSumPtr->GetPriceKey();
	return InsertSellMatchPxSumRecPriceIndex(priceKey, MatchPxSumPtr, DoCheck);
}  //TPositionRecord::InsertSellMatchPxSumRecPriceIndex()
//---------------------------------------------------------------------------
TMatchPriceSumRecord *TPositionRecord::GetSellMatchPxSumRecFromArray(int ArrayIndex)
{
	TBasicPositionObject *positionObjectPtr = GetBasicPositionObjFromArray(FSellMatchPxSumRecArray, ArrayIndex);
	return static_cast<TMatchPriceSumRecord*>(positionObjectPtr);
}  //TPositionRecord::GetSellMatchPxSumRecFromArray()
//---------------------------------------------------------------------------
void TPositionRecord::ClearSellMatchPxSumRec()
{
	FSellQuantitySum = 0;
	FSellAmountSum = 0.0;
	FSellCloseQuantitySum = 0;
	FSellCloseAmountSum = 0.0;
	ClearBasicPositionObjectArray(FSellMatchPxSumRecArray, false);  //Do not Delete Object
	ClearBasicPositionObjectKeyIndex(FSellMatchPxSumRecPriceIndex, true);  //Delete Object
}  //TPositionRecord::ClearSellMatchPxSumRec()
//---------------------------------------------------------------------------
void TPositionRecord::ClearHoldPosition()
{
	FBuyHoldQuantity = 0;
	FBuyHoldAmount = 0.0;
	FBuyHoldAveragePrice = 0.0;
	ClearBasicPositionObjectArray(FBuyHoldOIArray, true);  //Do Delete
	FSellHoldQuantity = 0;
	FSellHoldAmount = 0.0;
	FSellHoldAveragePrice = 0.0;
	ClearBasicPositionObjectArray(FSellHoldOIArray, true);  //Do Delete
}  //TPositionRecord::ClearHoldPosition()
//---------------------------------------------------------------------------
void TPositionRecord::AddHoldPosition(int BuyHoldQuantity, double BuyHoldAveragePrice,
									  int SellHoldQuantity, double SellHoldAveragePrice,
									  double PriceMultiplier)
{
	if (BuyHoldQuantity > 0)
	{
		FBuyHoldQuantity += BuyHoldQuantity;
		FBuyHoldAmount += static_cast<double>(BuyHoldQuantity) * BuyHoldAveragePrice;
		if (FBuyHoldQuantity > 0)
			FBuyHoldAveragePrice = FBuyHoldAmount / static_cast<double>(FBuyHoldQuantity);
		else
			FBuyHoldAveragePrice = 0.0;
		TOpenInterestRecord *newBuyOIRec = new TOpenInterestRecord(FExchangeCode, FSymbolCode, nsOrderMessageDefine::sBuy,
																   BuyHoldAveragePrice, BuyHoldQuantity, this);
		AppendBasicPositionObjectArray(FBuyHoldOIArray, newBuyOIRec);
	}  //if (BuyHoldQuantity > 0)

	if (SellHoldQuantity > 0)
	{
		FSellHoldQuantity += SellHoldQuantity;
		FSellHoldAmount += static_cast<double>(SellHoldQuantity) * SellHoldAveragePrice;
		if (FSellHoldQuantity > 0)
			FSellHoldAveragePrice = FSellHoldAmount / static_cast<double>(FSellHoldQuantity);
		else
			FSellHoldAveragePrice = 0.0;
		TOpenInterestRecord *newBuyOIRec = new TOpenInterestRecord(FExchangeCode, FSymbolCode, nsOrderMessageDefine::sSell,
																   SellHoldAveragePrice, SellHoldQuantity, this);
		AppendBasicPositionObjectArray(FSellHoldOIArray, newBuyOIRec);
	}  //if (SellHoldQuantity > 0)

	FPriceMultiplier = PriceMultiplier;
}  //TPositionRecord::AddHoldPosition()
//---------------------------------------------------------------------------
bool TPositionRecord::SumCloseQuantity(nsOrderMessageDefine::SideEnum Side, double MatchPrice, int CloseQuantity)
{
	if (CloseQuantity <= 0) return false;
	TMatchPriceSumRecord *matchPxSumRecPtr = 0;
	String priceKey = TMatchPriceSumRecord::GeneratePriceKey(MatchPrice);
	if (Side == nsOrderMessageDefine::sBuy)
	{
		matchPxSumRecPtr = FindBuyMatchPxSumRecByPrice(priceKey);
		FBuyCloseQuantitySum += CloseQuantity;
		FBuyCloseAmountSum += MatchPrice * CloseQuantity;
	}
	else if (Side == nsOrderMessageDefine::sSell)
	{
		matchPxSumRecPtr = FindSellMatchPxSumRecByPrice(priceKey);
		FSellCloseQuantitySum += CloseQuantity;
		FSellCloseAmountSum += MatchPrice * CloseQuantity;
	}
	else
	{
		AnsiString ansiExchangeCode = s888::ConvertUnicodeToAnsiStr(FExchangeCode);
		AnsiString ansiSymbolCode = s888::ConvertUnicodeToAnsiStr(FSymbolCode);
		UFC::BufferedLog::Printf(" TPositionRecord::SumCloseQuantity[%s-%s] Side=%d error.", ansiExchangeCode.c_str(), ansiSymbolCode.c_str(), Side);
		return false;
	}

	if (matchPxSumRecPtr == 0)
	{
		AnsiString ansiExchangeCode = s888::ConvertUnicodeToAnsiStr(FExchangeCode);
		AnsiString ansiSymbolCode = s888::ConvertUnicodeToAnsiStr(FSymbolCode);
		UFC::BufferedLog::Printf(" TPositionRecord::SumCloseQuantity[%s-%s] Side=%d, Px[%s] not found.", ansiExchangeCode.c_str(), ansiSymbolCode.c_str(), Side, priceKey.c_str());
		return false;
	}
	matchPxSumRecPtr->AddCloseQuantity(CloseQuantity);
	return true;
}  //TPositionRecord::SumCloseQuantity()
//---------------------------------------------------------------------------
void TPositionRecord::OffsetOIQueuePosition(TOpenInterestRecord *NewOIRecPtr, TBasicPositionObjectQueue& OIQueue)
{
	if (NewOIRecPtr == 0) return;
	while(!OIQueue.empty() && (NewOIRecPtr->GetOpenQuantity() > 0))
	{
		TOpenInterestRecord *curOIRecPtr = static_cast<TOpenInterestRecord*>(OIQueue.front());
		int origCloseQuantity = curOIRecPtr->GetCloseQuantity();
		int newOpenQuantity = NewOIRecPtr->GetOpenQuantity();
		NewOIRecPtr->DoOffsetting(curOIRecPtr->GetOpenQuantity());
		curOIRecPtr->DoOffsetting(newOpenQuantity);
		if (!SumCloseQuantity(curOIRecPtr->GetSide(), curOIRecPtr->GetMatchPrice(), curOIRecPtr->GetCloseQuantity() - origCloseQuantity))
		{
			AnsiString ansiExchangeCode = s888::ConvertUnicodeToAnsiStr(curOIRecPtr->GetExchangeCode());
			AnsiString ansiSymbolCode = s888::ConvertUnicodeToAnsiStr(curOIRecPtr->GetSymbolCode());
			UFC::BufferedLog::Printf(" TPositionRecord::OffsetOIQueuePosition() SumCloseQuantity[%s-%s] Failed. Side=%d", ansiExchangeCode.c_str(), ansiSymbolCode.c_str(), curOIRecPtr->GetSide());
		}

		if (curOIRecPtr->GetOpenQuantity() <= 0)
		{
			OIQueue.pop();
			delete curOIRecPtr;
		}
		else
			break;
	}  //while(!OIQueue.empty() && (NewOIRecPtr->GetOpenQuantity() > 0))
}  //TPositionRecord::OffsetOIQueuePosition()
//---------------------------------------------------------------------------
void TPositionRecord::SumFillData(nsOrderMessageDefine::SideEnum Side, double MatchPrice, int MatchQuantity, int CloseQuantity)
{
	String priceKey = TMatchPriceSumRecord::GeneratePriceKey(MatchPrice);
	TMatchPriceSumRecord *matchPxSumRecPtr = 0;
	if (Side == nsOrderMessageDefine::sBuy)
	{
		matchPxSumRecPtr = FindBuyMatchPxSumRecByPrice(priceKey);
		FBuyQuantitySum += MatchQuantity;
		FBuyAmountSum += MatchPrice * MatchQuantity;
	}
	else if (Side == nsOrderMessageDefine::sSell)
	{
		matchPxSumRecPtr = FindSellMatchPxSumRecByPrice(priceKey);
		FSellQuantitySum += MatchQuantity;
		FSellAmountSum += MatchPrice * MatchQuantity;
	}
	else
		return;

	bool isInsertSuccess = true;
	if (matchPxSumRecPtr == 0)
	{
		matchPxSumRecPtr = new TMatchPriceSumRecord(FExchangeCode, FSymbolCode, Side, MatchPrice, MatchQuantity, this);
		if (Side == nsOrderMessageDefine::sBuy)
			isInsertSuccess = InsertBuyMatchPxSumRecPriceIndex(priceKey, matchPxSumRecPtr, false);  //Do not Check
		else
			isInsertSuccess = InsertSellMatchPxSumRecPriceIndex(priceKey, matchPxSumRecPtr, false);  //Do not Check

		if (isInsertSuccess)
			matchPxSumRecPtr->SetCloseQuantity(CloseQuantity);
		else
			delete matchPxSumRecPtr;
	}
	else
	{
		matchPxSumRecPtr->AddMatchQuantity(MatchQuantity);
		matchPxSumRecPtr->AddCloseQuantity(CloseQuantity);
	}  //if (matchPxSumRecPtr == 0)

//	if (isInsertSuccess) matchPxSumRecPtr->DumpData(L"");
}  //TPositionRecord::SumFillData()
//---------------------------------------------------------------------------
void TPositionRecord::OffsetPosition(nsOrderMessageDefine::SideEnum Side,
									 nsOrderMessageDefine::PositionEffectEnum PositionEffect,
									 double MatchPrice, int MatchQuantity, bool isHoldPosition)
{
	String errorLog = L"";
	if ((Side != nsOrderMessageDefine::sBuy) && (Side != nsOrderMessageDefine::sSell))
	{
		errorLog.printf(L"TPositionRecord::OffsetPosition() [%s-%s] Wrong Side[%d]. MatchPx=%13.6lf, MatchQty[%d]", FExchangeCode, FSymbolCode, Side, MatchPrice, MatchQuantity);
		UFC::BufferedLog::Printf(" %s", s888::ConvertUnicodeToAnsiStr(errorLog).c_str());
		UFC::BufferedLog::FlushToFile();
		return;
	}

	TOpenInterestRecord *newOIRecPtr = new TOpenInterestRecord(FExchangeCode, FSymbolCode, Side, MatchPrice, MatchQuantity, this);
	if ((PositionEffect == nsOrderMessageDefine::peOpen) ||
		(PositionEffect == nsOrderMessageDefine::peDayTradeOpen))
	{
		if (Side == nsOrderMessageDefine::sBuy)
		{
			FBuyPEOpenQuantitySum += MatchQuantity;
			if (isHoldPosition)
				PushBasicPositionObjQueue(FBuyHoldOIQueue, newOIRecPtr);
			else
			{
				if (PositionEffect == nsOrderMessageDefine::peDayTradeOpen)
					PushBasicPositionObjQueue(FBuyDayTradeOIQueue, newOIRecPtr);
				else
					PushBasicPositionObjQueue(FBuyOIQueue, newOIRecPtr);
			}
		}
		else if (Side == nsOrderMessageDefine::sSell)
		{
			FSellPEOpenQuantitySum += MatchQuantity;
			if (isHoldPosition)
				PushBasicPositionObjQueue(FSellHoldOIQueue, newOIRecPtr);
			else
			{
				if (PositionEffect == nsOrderMessageDefine::peDayTradeOpen)
					PushBasicPositionObjQueue(FSellDayTradeOIQueue, newOIRecPtr);
				else
					PushBasicPositionObjQueue(FSellOIQueue, newOIRecPtr);
			}
		}
		SumFillData(Side, MatchPrice, MatchQuantity, 0);
	}
	else if ((PositionEffect == nsOrderMessageDefine::peClose) ||
			 (PositionEffect == nsOrderMessageDefine::peDayTrade))  //Close Today
	{
		if (Side == nsOrderMessageDefine::sBuy)
		{
			FBuyPECloseQuantitySum += MatchQuantity;
			//Offset Day Trade Open First
			OffsetOIQueuePosition(newOIRecPtr, FSellDayTradeOIQueue);

			//Offset Hold Position
			if ((PositionEffect != nsOrderMessageDefine::peDayTrade) && (newOIRecPtr->GetOpenQuantity() > 0))
				OffsetOIQueuePosition(newOIRecPtr, FSellHoldOIQueue);

			//Offset Open Position
			if (newOIRecPtr->GetOpenQuantity() > 0)
				OffsetOIQueuePosition(newOIRecPtr, FSellOIQueue);
		}
		else if (Side == nsOrderMessageDefine::sSell)
		{
			FSellPECloseQuantitySum += MatchQuantity;
			//Offset Day Trade Open First
			OffsetOIQueuePosition(newOIRecPtr, FBuyDayTradeOIQueue);

			//Offset Hold Position
			if ((PositionEffect != nsOrderMessageDefine::peDayTrade) && (newOIRecPtr->GetOpenQuantity() > 0))
				OffsetOIQueuePosition(newOIRecPtr, FBuyHoldOIQueue);

			//Offset Open Position
			if (newOIRecPtr->GetOpenQuantity() > 0)
				OffsetOIQueuePosition(newOIRecPtr, FBuyOIQueue);
		}
		SumFillData(Side, MatchPrice, MatchQuantity, newOIRecPtr->GetCloseQuantity());
		if (newOIRecPtr->GetOpenQuantity() > 0)
		{
			errorLog.printf(L"TPositionRecord::OffsetPosition() [%s-%s] PositionEffect[%d], MatchPx=%13.6lf, not engogh OpenQty, MatchQty[%d] - OpenQty = %d.", FExchangeCode, FSymbolCode, PositionEffect, MatchPrice, MatchQuantity, newOIRecPtr->GetOpenQuantity());
			UFC::BufferedLog::Printf(" %s", s888::ConvertUnicodeToAnsiStr(errorLog).c_str());
		}
		delete newOIRecPtr;
	}
	else
	{
		delete newOIRecPtr;
		errorLog.printf(L"TPositionRecord::OffsetPosition() [%s-%s] Wrong PositionEffect[%d]. MatchPx=%13.6lf, MatchQty[%d]", FExchangeCode, FSymbolCode, PositionEffect, MatchPrice, MatchQuantity);
		UFC::BufferedLog::Printf(" %s", s888::ConvertUnicodeToAnsiStr(errorLog).c_str());
	}
	UFC::BufferedLog::FlushToFile();
//	DumpMatchPxSumRec();
}  //TPositionRecord::OffsetPosition()
//---------------------------------------------------------------------------
bool TPositionRecord::AddFillReportData(const String& OrderID, const String& ExecID,
										nsOrderMessageDefine::SideEnum Side,
										nsOrderMessageDefine::PositionEffectEnum PositionEffect,
										double MatchPrice, int MatchQuantity,
										double FeeRateByVolume, double FeeRateByPrice)
{
	bool isSuccess = false;
	if ((OrderID.Length() <= 0) || (ExecID.Length() <= 0)) return isSuccess;
	if (FindMatchRecByExecID(ExecID) != 0) return isSuccess;

	//Add New Match Record
	TMatchRecord *newMatchRecPtr = new TMatchRecord(FExchangeCode, FSymbolCode,
													OrderID, ExecID, Side, PositionEffect,
													MatchPrice, MatchQuantity, FeeRateByVolume, FeeRateByPrice, this);
	if (InsertMatchRecExecIDIndex(ExecID, newMatchRecPtr, false))  //Do not Check
	{
		AppendMatchRecArray(newMatchRecPtr);
		FNewestPrice = MatchPrice;
		OffsetPosition(Side, PositionEffect, MatchPrice, MatchQuantity, false);  //is not Hold Position
		isSuccess = true;
	}
	else
	{
		String errorLog = L"";
		errorLog.printf(L"TPositionRecord::AddFillReportData() Insert OrderId[%s] ExecId[%s] Symbol[%s-%s] Match Record ExecID index failed.", OrderID, ExecID, FExchangeCode, FSymbolCode);
		UFC::BufferedLog::Printf(" %s", s888::ConvertUnicodeToAnsiStr(errorLog).c_str());
		UFC::BufferedLog::FlushToFile();
		delete newMatchRecPtr;
	}

	return isSuccess;
}  //TPositionRecord::AddFillReportData()
//---------------------------------------------------------------------------
void TPositionRecord::ReOffsetHoldPosition(TBasicPositionObjectVector& FirstHoldOIArray,
										   TBasicPositionObjectVector& SecondHoldOIArray)
{
	for (unsigned int i = 0; i < FirstHoldOIArray.size(); i++)
	{
		TOpenInterestRecord *curOIRecPtr = static_cast<TOpenInterestRecord*>(FirstHoldOIArray[i]);
		OffsetPosition(curOIRecPtr->GetSide(), nsOrderMessageDefine::peOpen,
					   curOIRecPtr->GetMatchPrice(), curOIRecPtr->GetMatchQuantity(), true);
	}

	for (unsigned int i = 0; i < SecondHoldOIArray.size(); i++)
	{
		TOpenInterestRecord *curOIRecPtr = static_cast<TOpenInterestRecord*>(SecondHoldOIArray[i]);
		OffsetPosition(curOIRecPtr->GetSide(), nsOrderMessageDefine::peOpen,
					   curOIRecPtr->GetMatchPrice(), curOIRecPtr->GetMatchQuantity(), true);
	}
}  //TPositionRecord::ReOffsetHoldPosition()
//---------------------------------------------------------------------------
void TPositionRecord::ReOffsetPosition()
{
	FBuyPEOpenQuantitySum = 0;
	FBuyPECloseQuantitySum = 0;
	FSellPEOpenQuantitySum = 0;
	FSellPECloseQuantitySum = 0;
	ClearBasicPositionObjQueue(FBuyDayTradeOIQueue, true);
	ClearBasicPositionObjQueue(FBuyHoldOIQueue, true);
	ClearBasicPositionObjQueue(FBuyOIQueue, true);
	ClearBasicPositionObjQueue(FSellDayTradeOIQueue, true);
	ClearBasicPositionObjQueue(FSellHoldOIQueue, true);
	ClearBasicPositionObjQueue(FSellOIQueue, true);
	ClearBuyMatchPxSumRec();
	ClearSellMatchPxSumRec();

	if ((FBuyHoldQuantity > 0) || (FSellHoldQuantity > 0))
	{
		if (FBuyHoldQuantity >= FSellHoldQuantity)
			ReOffsetHoldPosition(FBuyHoldOIArray, FSellHoldOIArray);  //Add Buy Side Hold Position To Open Queue First.
		else
			ReOffsetHoldPosition(FSellHoldOIArray, FBuyHoldOIArray);  //Add Sell Side Hold Position To Open Queue First.
	}  //if ((FBuyHoldQuantity > 0) || (FSellHoldQuantity > 0))

	for (int matchRecIndex = 0; matchRecIndex < GetMatchRecCount(); matchRecIndex++)
	{
		TMatchRecord *matchRecPtr = GetMatchRecFromArray(matchRecIndex);
		OffsetPosition(matchRecPtr->GetSide(), matchRecPtr->GetPositionEffect(),
					   matchRecPtr->GetMatchPrice(), matchRecPtr->GetMatchQuantity(), false);    //is not Hold Position
	}  //for (int MatchMsgIndex = 0; MatchMsgIndex < GetMatchMessageCount(); MatchMsgIndex++)
}  //TPositionRecord::ReOffsetPosition()
//---------------------------------------------------------------------------
void TPositionRecord::CalculateData()
{   //Clear Sum Data
AnsiString ansiExchangeCode = s888::ConvertUnicodeToAnsiStr(FExchangeCode);
AnsiString ansiSymbolCode = s888::ConvertUnicodeToAnsiStr(FSymbolCode);
	FTax = 0.0;
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
	FFee = 0.0;
	FCloseProfit = 0.0;

	for (int recIndex = 0; recIndex < GetBuyMatchPxSumRecCount(); recIndex++)
	{
		TMatchPriceSumRecord *matchPxSumRecPtr = GetBuyMatchPxSumRecFromArray(recIndex);
		if (matchPxSumRecPtr == 0) continue;
		FBuyQuantitySum += matchPxSumRecPtr->GetMatchQuantity();
		FBuyAmountSum += matchPxSumRecPtr->GetMatchAmount();
		FBuyCloseQuantitySum += matchPxSumRecPtr->GetCloseQuantity();
		FBuyCloseAmountSum += matchPxSumRecPtr->GetCloseAmount();
		int TempTaxValue = (matchPxSumRecPtr->GetMatchPrice() * FPriceMultiplier * FTaxRate) + 0.5;
		FTax += TempTaxValue * matchPxSumRecPtr->GetMatchQuantity();
	}  //for (int recIndex = 0; recIndex < GetBuyMatchPxSumRecCount(); recIndex++)
	if (FBuyQuantitySum > 0) FBuyAveragePrice = FBuyAmountSum / FBuyQuantitySum;

	for (int recIndex = 0; recIndex < GetSellMatchPxSumRecCount(); recIndex++)
	{
		TMatchPriceSumRecord *matchPxSumRecPtr = GetSellMatchPxSumRecFromArray(recIndex);
		if (matchPxSumRecPtr == 0) continue;
		FSellQuantitySum += matchPxSumRecPtr->GetMatchQuantity();
		FSellAmountSum += matchPxSumRecPtr->GetMatchAmount();
		FSellCloseQuantitySum += matchPxSumRecPtr->GetCloseQuantity();
		FSellCloseAmountSum += matchPxSumRecPtr->GetCloseAmount();
		int TempTaxValue = (matchPxSumRecPtr->GetMatchPrice() * FPriceMultiplier * FTaxRate) + 0.5;
		FTax += TempTaxValue * matchPxSumRecPtr->GetMatchQuantity();
	}  //for (int recIndex = 0; recIndex < GetBuyMatchPxSumRecCount(); recIndex++)
	if (FSellQuantitySum > 0) FSellAveragePrice = FSellAmountSum / FSellQuantitySum;

	FFee = FUnitFee * (FBuyQuantitySum + FSellQuantitySum);
	FCloseProfit = (FSellCloseAmountSum - FBuyCloseAmountSum) * FPriceMultiplier;
//UFC::BufferedLog::Printf(" TPositionRecord::CalculateData [%s-%s] CloseProfit[%15.6lf] = (SellCloseAmountSum[%15.6lf] - BuyCloseAmountSum[%15.6lf]) * PriceMultiplier[%15.6lf].", ansiExchangeCode.c_str(), ansiSymbolCode.c_str(),
//FCloseProfit, FSellCloseAmountSum, FBuyCloseAmountSum, FPriceMultiplier);
}  //TPositionRecord::CalculateData()
//---------------------------------------------------------------------------
void TPositionRecord::DumpMatchPxSumRec()
{
	String prefixStr = L"";
	for (int i = 0; i < GetBuyMatchPxSumRecCount(); i++)
	{
		prefixStr.printf(L"%d/%d", i + 1, GetBuyMatchPxSumRecCount());
		TMatchPriceSumRecord *curMatchPxSumRecPtr = GetBuyMatchPxSumRecFromArray(i);
		curMatchPxSumRecPtr->DumpData(prefixStr);
	}

	for (int i = 0; i < GetSellMatchPxSumRecCount(); i++)
	{
		prefixStr.printf(L"%d/%d", i + 1, GetSellMatchPxSumRecCount());
		TMatchPriceSumRecord *curMatchPxSumRecPtr = GetSellMatchPxSumRecFromArray(i);
		curMatchPxSumRecPtr->DumpData(prefixStr);
	}
	UFC::BufferedLog::Printf(" ");
}  //TPositionRecord::DumpMatchPxSumRec()
//---------------------------------------------------------------------------
void TPositionRecord::DumpData()
{
	AnsiString ansiExchangeCode = s888::ConvertUnicodeToAnsiStr(FExchangeCode);
	AnsiString ansiSymbolCode = s888::ConvertUnicodeToAnsiStr(FSymbolCode);
	UFC::BufferedLog::Printf(" PosRec[%s-%s] B H[%4d] T[%4d]=O[%4d]+C[%4d] M[%12.6lf]  L[%12.6lf].", ansiExchangeCode.c_str(), ansiSymbolCode.c_str(),
							 FBuyHoldQuantity, FBuyQuantitySum, FBuyQuantitySum - FBuyCloseQuantitySum, FBuyCloseQuantitySum, FPriceMultiplier, FNewestPrice);
	UFC::BufferedLog::Printf(" PosRec[%s-%s] S H[%4d] T[%4d]=O[%4d]+C[%4d] M[%12.6lf]  L[%12.6lf].", ansiExchangeCode.c_str(), ansiSymbolCode.c_str(),
							 FSellHoldQuantity, FSellQuantitySum, FSellQuantitySum - FSellCloseQuantitySum, FSellCloseQuantitySum, FPriceMultiplier, FNewestPrice);
}  //TPositionRecord::DumpData()


