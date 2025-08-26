//---------------------------------------------------------------------------

#ifndef PositionObjectsH
#define PositionObjectsH
#include <queue>
#include <map>
#include <vector>
#include <System.hpp>
#include "OrderMessageDefine.h"

class TPositionRecord;
//---------------------------------------------------------------------------
class TBasicPositionObject
{
protected:
	String FExchangeCode;
	String FSymbolCode;

public:
	TBasicPositionObject();
	TBasicPositionObject(const String& ExchangeCode, const String& SymbolCode);

	String GetExchangeCode() {return FExchangeCode;};
	String GetSymbolCode() {return FSymbolCode;};

	void SetExchangeCode(const String& ExchangeCode) {FExchangeCode = ExchangeCode;};
	void SetSymbolCode(const String& SymbolCode) {FSymbolCode = SymbolCode;};
};  //TBasicPositionObject

typedef std::queue<TBasicPositionObject*> TBasicPositionObjectQueue;
int GetBasicPositionObjQueueCount(TBasicPositionObjectQueue& BasicPositionObjQueue);
TBasicPositionObject *GetFirstBasicPositionObjFromQueue(TBasicPositionObjectQueue& BasicPositionObjQueue);
TBasicPositionObject *PopBasicPositionObjQueue(TBasicPositionObjectQueue& BasicPositionObjQueue);
bool PushBasicPositionObjQueue(TBasicPositionObjectQueue& BasicPositionObjQueue, TBasicPositionObject *PositionObjPtr);
void ClearBasicPositionObjQueue(TBasicPositionObjectQueue& BasicPositionObjQueue, bool DoDelete);

typedef std::vector<TBasicPositionObject*> TBasicPositionObjectVector;
int GetBasicPositionObjArrayCount(TBasicPositionObjectVector& BasicPositionObjArray);
TBasicPositionObject *GetBasicPositionObjFromArray(TBasicPositionObjectVector& BasicPositionObjArray, int ArrayIndex);
void AppendBasicPositionObjectArray(TBasicPositionObjectVector& BasicPositionObjArray, TBasicPositionObject *PositionObjPtr);
void ClearBasicPositionObjectArray(TBasicPositionObjectVector& BasicPositionObjArray, bool DoDelete);

typedef std::map<String, TBasicPositionObject*> TBasicPositionObjectMap;
int GetBasicPositionObjKeyIndexCount(TBasicPositionObjectMap& BasicPositionObjIndex);
TBasicPositionObject* FindBasicPositionObjByKey(TBasicPositionObjectMap& BasicPositionObjIndex, const String& KeyStr);
bool InsertBasicPositionObjKeyIndex(TBasicPositionObjectMap& BasicPositionObjIndex, const String& KeyStr, TBasicPositionObject *PositionObjPtr, bool DoCheck = true);
TBasicPositionObject *RemoveBasicPositionObjectKeyIndex(TBasicPositionObjectMap& BasicPositionObjIndex, const String& KeyStr);
void ClearBasicPositionObjectKeyIndex(TBasicPositionObjectMap& BasicPositionObjIndex, bool DoDelete);
int CopyBasicPositionObjFromIndexToArray(TBasicPositionObjectMap& BasicPositionObjIndex, TBasicPositionObjectVector& BasicPositionObjArray, bool DoClearArray = true);
//---------------------------------------------------------------------------
class TMatchRecord : public TBasicPositionObject
{
protected:
	String                                   FOrderID;
	String                                   FExecID;
	nsOrderMessageDefine::SideEnum           FSide;
	nsOrderMessageDefine::PositionEffectEnum FPositionEffect;
	double                                   FMatchPrice;
	int                                      FMatchQuantity;
	double                                   FMatchAmount;
	double									 FFeeRateByVolume;
	double									 FFeeRateByPrice;
	TPositionRecord                          *FPositionRecordPtr;

public:
	TMatchRecord();
	TMatchRecord(const String& ExchangeCode, const String& SymbolCode,
				 const String& OrderID, const String& ExecID,
				 nsOrderMessageDefine::SideEnum Side,
				 nsOrderMessageDefine::PositionEffectEnum PositionEffect,
				 double MatchPrice, int MatchQuantity,
				 double FeeRateByVolume, double FeeRateByPrice,
				 TPositionRecord *PositionRecordPtr = 0);

	String                                   GetOrderID() {return FOrderID;};
	String                                   GetExecID() {return FExecID;};
	nsOrderMessageDefine::SideEnum           GetSide() {return FSide;};
	nsOrderMessageDefine::PositionEffectEnum GetPositionEffect() {return FPositionEffect;};
	double                                   GetMatchPrice() {return FMatchPrice;};
	int                                      GetMatchQuantity() {return FMatchQuantity;};
	double                                   GetMatchAmount() {return FMatchAmount;};
	double									 GetFeeRateByVolume() {return FFeeRateByVolume;};
	double									 GetFeeRateByPrice() {return FFeeRateByPrice;};
	TPositionRecord                          *GetPositionRecord() {return  FPositionRecordPtr;};

	void SetOrderId(const String& OrderID) {FOrderID = OrderID;};
	void SetExecId(const String& ExecID) {FExecID = ExecID;};
	void SetSide(nsOrderMessageDefine::SideEnum Side) {FSide = Side;};
	void SetPositionEffect(nsOrderMessageDefine::PositionEffectEnum PositionEffect) {FPositionEffect = PositionEffect;};
	void SetMatchPrice(double MatchPrice) {FMatchPrice = MatchPrice;};
	void SetMatchQuantity(int MatchQuantity) {FMatchQuantity = MatchQuantity;};
	void SetMatchAmount(double MatchAmount) {FMatchAmount = MatchAmount;};
	void SetFeeRateByVolume(double FeeRateByVolume) {FFeeRateByVolume = FeeRateByVolume;};
	void SetFeeRateByPrice(double FeeRateByPrice) {FFeeRateByPrice = FeeRateByPrice;};
	void SetPositionRecord(TPositionRecord* PositionRecordPtr) {FPositionRecordPtr = PositionRecordPtr;};
};  //TMatchRecord

//---------------------------------------------------------------------------
class TOpenInterestRecord : public TBasicPositionObject
{
protected:
	nsOrderMessageDefine::SideEnum FSide;
	double                         FMatchPrice;
	int                            FMatchQuantity;
	int                            FOpenQuantity;
	int                            FCloseQuantity;
	TPositionRecord                *FPositionRecordPtr;

public:
	TOpenInterestRecord();
	TOpenInterestRecord(const String& ExchangeCode, const String& SymbolCode,
						nsOrderMessageDefine::SideEnum Side,
						double MatchPrice, int MatchQuantity,
						TPositionRecord *PositionRecordPtr = 0);

	nsOrderMessageDefine::SideEnum GetSide() {return FSide;};
	double                         GetMatchPrice() {return FMatchPrice;};
	int                            GetMatchQuantity() {return FMatchQuantity;};
	int                            GetOpenQuantity() {return FOpenQuantity;};
	int                            GetCloseQuantity() {return FCloseQuantity;};
	TPositionRecord                *GetPositionRecord() {return  FPositionRecordPtr;};

	void SetSide(nsOrderMessageDefine::SideEnum Side) {FSide = Side;};
	void SetMatchPrice(double MatchPrice) {FMatchPrice = MatchPrice;};
	void SetMatchQuantity(int MatchQuantity) {FMatchQuantity = MatchQuantity;};
	void SetOpenQuantity(int OpenQuantity) {FOpenQuantity = OpenQuantity;};
	void SetCloseQuantity(int CloseQuantity) {FCloseQuantity = CloseQuantity;};
	void SetPositionRecord(TPositionRecord *PositionRecordPtr) {FPositionRecordPtr = PositionRecordPtr;};

	int DoOffsetting(int CoverQuantity);
	void DumpData(const String& PrefixMsg = L"");
};  //TOpenInterestRecord

//---------------------------------------------------------------------------
class TMatchPriceSumRecord : public TBasicPositionObject
{
private:
	nsOrderMessageDefine::SideEnum FSide;
	double                         FMatchPrice;
	int                            FMatchQuantity;
	double                         FMatchAmount;
	int                            FCloseQuantity;
	double                         FCloseAmount;
	TPositionRecord                *FPositionRecordPtr;
	String                         FPriceKey;

public:
	static String GeneratePriceKey(double Price);

	TMatchPriceSumRecord();
	TMatchPriceSumRecord(const String& ExchangeCode, const String& SymbolCode,
						 nsOrderMessageDefine::SideEnum Side,
						 double MatchPrice, int MatchQuantity = 0,
						 TPositionRecord *PositionRecordPtr = 0);

	nsOrderMessageDefine::SideEnum GetSide() {return FSide;};
	double                         GetMatchPrice() {return FMatchPrice;};
	int                            GetMatchQuantity() {return FMatchQuantity;};
	double                         GetMatchAmount() {return FMatchAmount;};
	int                            GetCloseQuantity() {return FCloseQuantity;};
	double                         GetCloseAmount() {return FCloseAmount;};
	TPositionRecord                *GetPositionRecord() {return  FPositionRecordPtr;};
	String                         GetPriceKey() {return FPriceKey;};


	void SetSide(nsOrderMessageDefine::SideEnum Side) {FSide = Side;};
	void SetMatchPrice(double MatchPrice) {FMatchPrice = MatchPrice;};
	void SetMatchQuantity(int MatchQuantity);
	void SetMatchAmount(double MatchAmount) {FMatchAmount = MatchAmount;};
	void SetCloseQuantity(int CloseQuantity);
	void SetCloseAmount(double CloseAmount) {FCloseAmount = CloseAmount;};
	void SetPositionRecord(TPositionRecord *PositionRecordPtr) {FPositionRecordPtr = PositionRecordPtr;};
	void SetPriceKey(const String& PriceKey) {FPriceKey = PriceKey;};

	void CalculateMatchAmount() {FMatchAmount = FMatchPrice * FMatchQuantity;};
	void CalculateCloseAmount() {FCloseAmount = FMatchPrice * FCloseQuantity;};
	void CalculateAmount();
	void AddMatchQuantity(int MatchQuantity);
	void AddCloseQuantity(int CloseQuantity);

	void DumpData(const String& PrefixMsg = L"");
};  //TMatchPriceSumRecord

//---------------------------------------------------------------------------
class TPositionRecord : public TBasicPositionObject
{
protected:
	String FCommodityKey;
	double FPriceMultiplier;
	double FTaxRate;
	double FUnitFee;
	double FNewestPrice;
	int    FBuyQuantity1;    //最佳五檔買邊第一檔數量
	double FBuyPrice1;       //最佳五檔買邊第一檔價格
	int    FSellQuantity1;   //最佳五檔賣邊第一檔數量
	double FSellPrice1;      //最佳五檔賣邊第一檔價格

	//Hold Position Data(Buy Side)
	int    FBuyHoldQuantity;
	double FBuyHoldAmount;
	double FBuyHoldAveragePrice;
	TBasicPositionObjectVector FBuyHoldOIArray;

	//Hold Position Data(Sell Side)
	int    FSellHoldQuantity;
	double FSellHoldAmount;
	double FSellHoldAveragePrice;
	TBasicPositionObjectVector FSellHoldOIArray;

	//Fill Message
	TBasicPositionObjectMap    FMatchRecExecIDIndex;
	TBasicPositionObjectVector FMatchRecArray;

	//Fill Data Statistics
	int    FBuyQuantitySum;
	double FBuyAmountSum;
	int    FBuyCloseQuantitySum;
	double FBuyCloseAmountSum;
	int    FBuyPEOpenQuantitySum;
	int    FBuyPECloseQuantitySum;
	int    FSellQuantitySum;
	double FSellAmountSum;
	int    FSellCloseQuantitySum;
	double FSellCloseAmountSum;
	int    FSellPEOpenQuantitySum;
	int    FSellPECloseQuantitySum;
	TBasicPositionObjectMap    FBuyMatchPxSumRecPriceIndex;
	TBasicPositionObjectVector FBuyMatchPxSumRecArray;
	TBasicPositionObjectMap    FSellMatchPxSumRecPriceIndex;
	TBasicPositionObjectVector FSellMatchPxSumRecArray;

	//Open Interest Queue
	TBasicPositionObjectQueue FBuyDayTradeOIQueue;
	TBasicPositionObjectQueue FBuyHoldOIQueue;
	TBasicPositionObjectQueue FBuyOIQueue;
	TBasicPositionObjectQueue FSellDayTradeOIQueue;
	TBasicPositionObjectQueue FSellHoldOIQueue;
	TBasicPositionObjectQueue FSellOIQueue;

	double FTax;
	double FFee;
	double FBuyAveragePrice;
	double FSellAveragePrice;
	double FCloseProfit;
	int    FTradeQuantitySum;  //For OnMarketDataUpdate
	double FFloatingProfit;
	DWORD  FLastTickCount;
	bool   FIsWaiting;
	bool   FIsSubscribed;

public:
	TPositionRecord();
	TPositionRecord(const String& ExchangeCode, const String& SymbolCode);
	~TPositionRecord();

	String GetCommodityKey() {return FCommodityKey;};
	double GetPriceMultiplier() {return FPriceMultiplier;};
	double GetTaxRate() {return FTaxRate;};
	double GetUnitFee() {return FUnitFee;};
	double GetNewestPrice() {return FNewestPrice;};
	int    GetBuyQuantity1() {return FBuyQuantity1;};
	double GetBuyPrice1() {return FBuyPrice1;};
	int    GetSellQuantity1() {return FSellQuantity1;};
	double GetSellPrice1() {return FSellPrice1;};

	int    GetBuyHoldQuantity() {return FBuyHoldQuantity;};
	double GetBuyHoldAmount() {return FBuyHoldAmount;};
	double GetBuyHoldAveragePrice() {return FBuyHoldAveragePrice;};
	int    GetSellHoldQuantity() {return FSellHoldQuantity;};
	double GetSellHoldAmount() {return FSellHoldAmount;};
	double GetSellHoldAveragePrice() {return FSellHoldAveragePrice;};
	int    GetBuyQuantitySum() {return FBuyQuantitySum;};
	double GetBuyAmountSum() {return FBuyAmountSum;};
	int    GetBuyCloseQuantitySum() {return FBuyCloseQuantitySum;};
	double GetBuyCloseAmountSum() {return FBuyCloseAmountSum;};
	int    GetBuyPEOpenQuantitySum() {return FBuyPEOpenQuantitySum;};
	int    GetBuyPECloseQuantitySum() {return FBuyPECloseQuantitySum;};
	int    GetSellQuantitySum() {return FSellQuantitySum;};
	double GetSellAmountSum() {return FSellAmountSum;};
	int    GetSellCloseQuantitySum() {return FSellCloseQuantitySum;};
	double GetSellCloseAmountSum() {return FSellCloseAmountSum;};
	int    GetSellPEOpenQuantitySum() {return FSellPEOpenQuantitySum;};
	int    GetSellPECloseQuantitySum() {return FSellPECloseQuantitySum;};
	double GetTax() {return FTax;};
	double GetFee() {return FFee;};
	double GetBuyAveragePrice() {return FBuyAveragePrice;};
	double GetSellAveragePrice() {return FSellAveragePrice;};
	double GetCloseProfit() {return FCloseProfit;};
	int    GetTradeQuantitySum() {return FTradeQuantitySum;};
	double GetFloatingProfit() {return FFloatingProfit;};
	DWORD  GetLastTickCount() {return FLastTickCount;};
	bool   IsWaiting() {return FIsWaiting;};
	bool   IsSubscribed() {return FIsSubscribed;};

	void SetCommodityKey(const String& CommodityKey) {FCommodityKey = CommodityKey;};
	void SetPriceMultiplier(double PriceMultiplier) {FPriceMultiplier = PriceMultiplier;};
	void SetTaxRate(double TaxRate) {FTaxRate = TaxRate;};
	void SetUnitFee(double UnitFee) {FUnitFee = UnitFee;};
	void SetNewestPrice(double NewestPrice) {FNewestPrice = NewestPrice;};
	void SetBuyQuantity1(int BuyQuantity1) {FBuyQuantity1 = BuyQuantity1;};
	void SetBuyPrice1(double BuyPrice1) {FBuyPrice1 = BuyPrice1;};
	void SetSellQuantity1(int SellQuantity1) {FSellQuantity1 = SellQuantity1;};
	void SetSellPrice1(double SellPrice1) {FSellPrice1 = SellPrice1;};
	void SetTradeQuantitySum(int TradeQuantitySum) {FTradeQuantitySum = TradeQuantitySum;};
	void AddTradeQuantitySum(int TradeQuantitySum) {FTradeQuantitySum += TradeQuantitySum;};
	void SetFloatingProfit(double FloatingProfit) {FFloatingProfit = FloatingProfit;};
	void SetLastTickCount(DWORD LastTickCount) {FLastTickCount = LastTickCount;};
	void SetWaiting(bool IsWaiting) {FIsWaiting = IsWaiting;};
	void SetSubscribed(bool IsSubscribed) {FIsSubscribed = IsSubscribed;};

	//Fill Message
	TMatchRecord *FindMatchRecByExecID(const String& ExecID);
	bool InsertMatchRecExecIDIndex(const String& ExecID, TMatchRecord *MatchRecPtr, bool DoCheck = true);
	bool InsertMatchRecExecIDIndex(TMatchRecord *MatchRecPtr, bool DoCheck = true);
	int GetMatchRecCount() {return static_cast<int>(FMatchRecArray.size());};
	TMatchRecord *GetMatchRecFromArray(int ArrayIndex);
	void AppendMatchRecArray(TMatchRecord *MatchRecPtr);
	void ClearMatchRec();

	//TMatchPriceSumRecord Buy Side
	TMatchPriceSumRecord *FindBuyMatchPxSumRecByPrice(const String& PriceKey);
	TMatchPriceSumRecord *FindBuyMatchPxSumRecByPrice(double MatchPrice);
	bool InsertBuyMatchPxSumRecPriceIndex(const String& PriceKey,
										  TMatchPriceSumRecord *MatchPxSumPtr, bool DoCheck = true);
	bool InsertBuyMatchPxSumRecPriceIndex(double MatchPrice,
										  TMatchPriceSumRecord *MatchPxSumPtr, bool DoCheck = true);
	bool InsertBuyMatchPxSumRecPriceIndex(TMatchPriceSumRecord *MatchPxSumPtr, bool DoCheck = true);
	int GetBuyMatchPxSumRecCount() {return static_cast<int>(FBuyMatchPxSumRecArray.size());};
	TMatchPriceSumRecord *GetBuyMatchPxSumRecFromArray(int ArrayIndex);
	void ClearBuyMatchPxSumRec();

	//TMatchPriceSum Sell Side
	TMatchPriceSumRecord *FindSellMatchPxSumRecByPrice(const String& PriceKey);
	TMatchPriceSumRecord *FindSellMatchPxSumRecByPrice(double MatchPrice);
	bool InsertSellMatchPxSumRecPriceIndex(const String& PriceKey,
										   TMatchPriceSumRecord *MatchPxSumPtr, bool DoCheck = true);
	bool InsertSellMatchPxSumRecPriceIndex(double MatchPrice,
										   TMatchPriceSumRecord *MatchPxSumPtr, bool DoCheck = true);
	bool InsertSellMatchPxSumRecPriceIndex(TMatchPriceSumRecord *MatchPxSumPtr, bool DoCheck = true);
	int GetSellMatchPxSumRecCount() {return static_cast<int>(FSellMatchPxSumRecArray.size());};
	TMatchPriceSumRecord *GetSellMatchPxSumRecFromArray(int ArrayIndex);
	void ClearSellMatchPxSumRec();

	//Hold Position Data(Buy Side)
	void ClearHoldPosition();
	void AddHoldPosition(int BuyHoldQuantity, double BuyHoldAveragePrice,
						 int SellHoldQuantity, double SellHoldAveragePrice,
						 double PriceMultiplier = 1.0);

	bool SumCloseQuantity(nsOrderMessageDefine::SideEnum Side, double MatchPrice, int CloseQuantity);
	void OffsetOIQueuePosition(TOpenInterestRecord *NewOIRecPtr, TBasicPositionObjectQueue& OIQueue);
	void SumFillData(nsOrderMessageDefine::SideEnum Side, double MatchPrice, int MatchQuantity, int CloseQuantity);
	void OffsetPosition(nsOrderMessageDefine::SideEnum Side,
						nsOrderMessageDefine::PositionEffectEnum PositionEffect,
						double MatchPrice, int MatchQuantity, bool isHoldPosition = false);
	bool AddFillReportData(const String& OrderID, const String& ExecID,
						   nsOrderMessageDefine::SideEnum Side,
						   nsOrderMessageDefine::PositionEffectEnum PositionEffect,
						   double MatchPrice, int MatchQuantity,
						   double FeeRateByVolume, double FeeRateByPrice);
	void ReOffsetHoldPosition(TBasicPositionObjectVector& FirstHoldOIArray,
							  TBasicPositionObjectVector& SecondHoldOIArray);
	void ReOffsetPosition();
	void CalculateData();
	void DumpMatchPxSumRec();
	void DumpData();
};  //TPositionRecord
#endif
