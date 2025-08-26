//---------------------------------------------------------------------------

#ifndef PositionInstanceH
#define PositionInstanceH
#include <queue>
#include <map>
#include <vector>
#include <System.hpp>
#include "OrderMessageDefine.h"
#include "TExecutionUnit.h"

class TPositionInstance;
//---------------------------------------------------------------------------
class TMatchMessageData
{
private:
	TPositionInstance                        *FPositionInstancePtr;
	String                                   FOrderId;
	String                                   FExecId;
	nsOrderMessageDefine::SideEnum           FSide;
	nsOrderMessageDefine::PositionEffectEnum FPositionEffect;
	double                                   FMatchPrice;
	int                                      FMatchQuantity;
	double                                   FMatchAmount;

public:
	TMatchMessageData(const String& OrderId, const String& ExecId,
					  nsOrderMessageDefine::SideEnum Side,
					  nsOrderMessageDefine::PositionEffectEnum PositionEffect,
					  double MatchPrice, int MatchQuantity,
					  TPositionInstance *PositionInstancePtr = 0);

	TPositionInstance                        *GetPositionInstance() {return  FPositionInstancePtr;};
	String                                   GetOrderId() {return FOrderId;};
	String                                   GetExecId() {return FExecId;};
	nsOrderMessageDefine::SideEnum           GetSide() {return FSide;};
	nsOrderMessageDefine::PositionEffectEnum GetPositionEffect() {return FPositionEffect;};
	double                                   GetMatchPrice() {return FMatchPrice;};
	int                                      GetMatchQuantity() {return FMatchQuantity;};
	double                                   GetMatchAmount() {return FMatchAmount;};

	void SetPositionInstance(TPositionInstance* PositionInstancePtr) {FPositionInstancePtr = PositionInstancePtr;};
	void SetMatchPrice(double MatchPrice);
	void SetMatchQuantity(int MatchQuantity);
};  //TMatchMessageData

//---------------------------------------------------------------------------
class TOpenInterestObject
{
private:
	TPositionInstance              *FPositionInstancePtr;
	nsOrderMessageDefine::SideEnum FSide;
	double                         FMatchPrice;
	int                            FMatchQuantity;
	int                            FOpenQuantity;
	int                            FCloseQuantity;

public:
	TOpenInterestObject(nsOrderMessageDefine::SideEnum Side,
						double MatchPrice, int MatchQuantity,
						TPositionInstance *PositionInstancePtr = 0);

	nsOrderMessageDefine::SideEnum GetSide() {return FSide;};
	TPositionInstance *GetPositionInstance() {return  FPositionInstancePtr;};
	double            GetMatchPrice() {return FMatchPrice;};
	int               GetMatchQuantity() {return FMatchQuantity;};
	int               GetOpenQuantity() {return FOpenQuantity;};
	int               GetCloseQuantity() {return FCloseQuantity;};

	int DoOffsetting(int CoverQuantity);
	void DumpData( const String& Msg = L"");
};  //TOpenInterestObject

//---------------------------------------------------------------------------
class TMatchPriceSum
{
private:
	TPositionInstance              *FPositionInstancePtr;
	nsOrderMessageDefine::SideEnum FSide;
	double                         FMatchPrice;
	int                            FMatchQuantity;
	String                         FPriceKey;
	double                         FMatchAmount;
	int                            FCloseQuantity;
	double                         FCloseAmount;

public:
	static String GeneratePriceKey(double Price);

	TMatchPriceSum(nsOrderMessageDefine::SideEnum Side,
				   double MatchPrice, int MatchQuantity = 0,
				   TPositionInstance *PositionInstancePtr = 0);
	TPositionInstance *GetPositionInstance() {return  FPositionInstancePtr;};
	nsOrderMessageDefine::SideEnum GetSide() {return FSide;};
	double GetMatchPrice() {return FMatchPrice;};
	int GetMatchQuantity() {return FMatchQuantity;};
	String GetPriceKey() {return FPriceKey;};
	double GetMatchAmount() {return FMatchAmount;};
	int GetCloseQuantity() {return FCloseQuantity;};
	double GetCloseAmount() {return FCloseAmount;};

	void SetPositionInstance(TPositionInstance* PositionInstancePtr) {FPositionInstancePtr = PositionInstancePtr;};
	void CalculateMatchAmount() {FMatchAmount = FMatchPrice * FMatchQuantity;};
	void CalculateCloseAmount() {FCloseAmount = FMatchPrice * FCloseQuantity;};
	void CalculateAmount();
	void SetMatchQuantity(int MatchQuantity);
	void AddMatchQuantity(int MatchQuantity);
	void SetCloseQuantity(int CloseQuantity);
	void AddCloseQuantity(int CloseQuantity);
	void DumpData( const String& Msg = L"");
};  //TMatchPriceSum

//---------------------------------------------------------------------------
class TPositionInstance
{
private:
	typedef std::vector<TMatchMessageData*> CMatchMessageVector;
	typedef std::map<UnicodeString, TMatchMessageData*> CStringMatchMessageMap;

	typedef std::vector<TMatchPriceSum*> CMatchPriceSumVector;
	typedef std::map<String, TMatchPriceSum*> CStringMatchPriceSumMap;

	typedef std::vector<TOpenInterestObject*> CHoldOpenInterestVector;
	typedef std::queue<TOpenInterestObject*> COpenInterestQueue;

	String FExchangeCode;
	String FSymbolCode;
	String FCommodityKey;
	double FBuyHoldAveragePrice;
	int    FBuyHoldQuantity;
	double FBuyHoldAmount;
	double FSellHoldAveragePrice;
	int    FSellHoldQuantity;
	double FSellHoldAmount;
	double FPriceMultiplier;
	double FNewestPrice;
	int    FTradeQuantitySum;
	double FTaxRate;
	double FUnitFee;
	double FTax;
	double FFee;
	int    FBuyQuantitySum;
	double FBuyAmountSum;
	int    FBuyCloseQuantitySum;
	double FBuyCloseAmountSum;
	double FBuyAveragePrice;
	int    FSellQuantitySum;
	double FSellAmountSum;
	int    FSellCloseQuantitySum;
	double FSellCloseAmountSum;
	double FSellAveragePrice;
	double FCloseProfit;
	double FFloatingProfit;
	DWORD  FLastTickCount;
	bool   FIsWaiting;
	bool   FIsSubscribed;

	//Fill Message
	CStringMatchMessageMap FMatchMessageExecIdIndex;
	CMatchMessageVector    FMatchMessageArray;

	//Fill Data Statistics
	CStringMatchPriceSumMap FBuyMatchPriceSumPriceIndex;
	CMatchPriceSumVector    FBuyMatchPriceSumArray;
	CStringMatchPriceSumMap FSellMatchPriceSumPriceIndex;
	CMatchPriceSumVector    FSellMatchPriceSumArray;

	//Hold Open Interest Array
	CHoldOpenInterestVector FBuyHoldOpenInterestArray;
	CHoldOpenInterestVector FSellHoldOpenInterestArray;

	//Open Interest Queue
	COpenInterestQueue FBuyDayTradeOpenQueue;
	COpenInterestQueue FBuyOpenQueue;
	COpenInterestQueue FSellDayTradeOpenQueue;
	COpenInterestQueue FSellOpenQueue;

public:
	TPositionInstance(const UnicodeString& ExchangeCode, const UnicodeString& SymbolCode);
	~TPositionInstance();

	String GetExchangeCode() {return FExchangeCode;};
	String GetSymbolCode() {return FSymbolCode;};
	String GetCommodityKey() {return FCommodityKey;};
	double GetBuyHoldAveragePrice() {return FBuyHoldAveragePrice;};
	int    GetBuyHoldQuantity() {return FBuyHoldQuantity;};
	double GetBuyHoldAmount() {return FBuyHoldAmount;};
	double GetSellHoldAveragePrice() {return FSellHoldAveragePrice;};
	int    GetSellHoldQuantity() {return FSellHoldQuantity;};
	double GetSellHoldAmount() {return FSellHoldAmount;};
	double GetPriceMultiplier() {return FPriceMultiplier;};
	double GetNewestPrice() {return FNewestPrice;};
	int    GetTradeQuantitySum() {return FTradeQuantitySum;};
	double GetTaxRate() {return FTaxRate;};
	double GetUnitFee() {return FUnitFee;};
	double GetTax() {return FTax;};
	double GetFee() {return FFee;};
	int    GetBuyQuantitySum() {return FBuyQuantitySum;};
	double GetBuyAmountSum() {return FBuyAmountSum;};
	int    GetBuyCloseQuantitySum() {return FBuyCloseQuantitySum;};
	double GetBuyCloseAmountSum() {return FBuyCloseAmountSum;};
	double GetBuyAveragePrice() {return FBuyAveragePrice;};
	int    GetSellQuantitySum() {return FSellQuantitySum;};
	double GetSellAmountSum() {return FSellAmountSum;};
	int    GetSellCloseQuantitySum() {return FSellCloseQuantitySum;};
	double GetSellCloseAmountSum() {return FSellCloseAmountSum;};
	double GetSellAveragePrice() {return FSellAveragePrice;};
	double GetCloseProfit() {return FCloseProfit;};
	double GetFloatingProfit() {return FFloatingProfit;};
	DWORD  GetLastTickCount() {return FLastTickCount;};
	bool   IsWaiting() {return FIsWaiting;};
	bool   IsSubscribed() {return FIsSubscribed;};

	void SetBuyHoldAveragePrice(double BuyHoldAveragePrice) {FBuyHoldAveragePrice = BuyHoldAveragePrice;};
	void SetBuyHoldQuantity(int BuyHoldQuantity) {FBuyHoldQuantity = BuyHoldQuantity;};
	void SetBuyHoldAmount(double BuyHoldAmount) {FBuyHoldAmount = BuyHoldAmount;};
	void SetSellHoldAveragePrice(double SellHoldAveragePrice) {FSellHoldAveragePrice = SellHoldAveragePrice;};
	void SetSellHoldQuantity(int SellHoldQuantity) {FSellHoldQuantity = SellHoldQuantity;};
	void SetSellHoldAmount(double SellHoldAmount) {FSellHoldAmount = SellHoldAmount;};
	void SetPriceMultiplier(double PriceMultiplier) {FPriceMultiplier = PriceMultiplier;};
	void SetNewestPrice(double NewestPrice) {FNewestPrice = NewestPrice;};
	void SetTradeQuantitySum(int TradeQuantitySum) {FTradeQuantitySum = TradeQuantitySum;};
	void AddTradeQuantitySum(int TradeQuantitySum) {FTradeQuantitySum += TradeQuantitySum;};
	void SetTaxRate(double TaxRate) {FTaxRate = TaxRate;};
	void SetUnitFee(double UnitFee) {FUnitFee = UnitFee;};
	void SetFloatingProfit(double FloatingProfit) {FFloatingProfit = FloatingProfit;};
	void SetLastTickCount(DWORD LastTickCount) {FLastTickCount = LastTickCount;};
	void SetWaiting(bool IsWaiting) {FIsWaiting = IsWaiting;};
	void SetSubscribed(bool IsSubscribed) {FIsSubscribed = IsSubscribed;};

	//TMatchMessageData
	TMatchMessageData *FindMatchMessageByExecId(const String& ExecId);
	bool InsertMatchMessageExecIdIndex(const String& ExecId, TMatchMessageData *MatchMsgPtr, bool DoCheck = true);
	bool InsertMatchMessageExecIdIndex(TMatchMessageData *MatchMsgPtr, bool DoCheck = true);
	int GetMatchMessageCount() {return FMatchMessageArray.size();};
	TMatchMessageData *GetMatchMessageByIndex(int Index);
	void PushToMatchMessageArray(TMatchMessageData *MatchMsgPtr);
	void ClearMatchMessage();

	//TMatchPriceSum Buy Side
	TMatchPriceSum *FindBuyMatchPriceSumByPrice(const String& PriceKey);
	TMatchPriceSum *FindBuyMatchPriceSumByPrice(double Price);
	bool InsertBuyMatchPriceSumPriceIndex(const String& PriceKey,
										  TMatchPriceSum *MatchPriceSumPtr, bool DoCheck = true);
	bool InsertBuyMatchPriceSumPriceIndex(TMatchPriceSum *MatchPriceSumPtr, bool DoCheck = true);
	int GetBuyMatchPriceSumCount() {return FBuyMatchPriceSumArray.size();};
	TMatchPriceSum *GetBuyMatchPriceSumByIndex(int Index);
	void PushToBuyMatchPriceSumArray(TMatchPriceSum *MatchPriceSumPtr);
	void ClearBuyMatchPriceSum();

	//TMatchPriceSum Sell Side
	TMatchPriceSum *FindSellMatchPriceSumByPrice(const String& PriceKey);
	TMatchPriceSum *FindSellMatchPriceSumByPrice(double Price);
	bool InsertSellMatchPriceSumPriceIndex(const String& PriceKey,
										   TMatchPriceSum *MatchPriceSumPtr, bool DoCheck = true);
	bool InsertSellMatchPriceSumPriceIndex(TMatchPriceSum *MatchPriceSumPtr, bool DoCheck = true);
	int GetSellMatchPriceSumCount() {return FSellMatchPriceSumArray.size();};
	TMatchPriceSum *GetSellMatchPriceSumByIndex(int Index);
	void PushToSellMatchPriceSumArray(TMatchPriceSum *MatchPriceSumPtr);
	void ClearSellMatchPriceSum();

	//TOpenInterestObject
	void ClearHoldOpenInterestObject();
	void AppendHoldOpenInterestObjectToArray(nsOrderMessageDefine::SideEnum Side, double MatchPrice, int MatchQuantity);

	void PushOpenInterestObjectToQueue(TOpenInterestObject *OIPtr, COpenInterestQueue *OpenInterestQueuePtr);
	TOpenInterestObject *PopOpenInterestObjectFromQueue(COpenInterestQueue *OpenInterestQueuePtr);
	void ClearOpenInterestQueue(COpenInterestQueue *OpenInterestQueuePtr);

	bool SumCloseQuantity(nsOrderMessageDefine::SideEnum Side,
						  double MatchPrice, int CloseQuantity);
	void SumFillData(nsOrderMessageDefine::SideEnum Side,
					 double MatchPrice, int MatchQuantity, int CloseQuantity);
	void OffsetOpenQueuePosition(TOpenInterestObject *NewOIPtr,
								 COpenInterestQueue *OpenInterestQueuePtr);
	void OffsetPosition(nsOrderMessageDefine::SideEnum Side,
						nsOrderMessageDefine::PositionEffectEnum PositionEffect,
						double MatchPrice, int MatchQuantity);
	void SetHoldPosition(double BuyHoldAveragePrice, int BuyHoldQuantity,
						 double SellHoldAveragePrice, int SellHoldQuantity,
						 double PriceMultiplier = 1.0);
	void SetHoldPosition(int BuyHoldQuantity, double BuyHoldAmount, double BuyHoldAveragePrice,
						 int SellHoldQuantity, double SellHoldAmount, double SellHoldAveragePrice,
						 double PriceMultiplier = 1.0);
	void AddHoldPosition(int BuyHoldQuantity, double BuyHoldAveragePrice,
						 int SellHoldQuantity, double SellHoldAveragePrice,
						 double PriceMultiplier = 1.0);
	bool AddNewFillMessage(const String& OrderId, const String& ExecId,
						   nsOrderMessageDefine::SideEnum Side,
						   nsOrderMessageDefine::PositionEffectEnum PositionEffect,
						   double MatchPrice, int MatchQuantity);
	void ReOffsetPosition();
	void CalculateData();
};  //TPositionInstance
#endif
