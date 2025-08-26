//---------------------------------------------------------------------------

#ifndef TradingPositionH
#define TradingPositionH
#include "OrderMessageDefine.h"
#include "TradingUtility.h"
#include "TradingBasicObjects.h"
#include "TradingCommodities.h"

#include "TradingCommodities.h"
//---------------------------------------------------------------------------
namespace s888
{
/*
//---------------------------------------------------------------------------
class CPositionStatisticSymbolKey : public CBasicTradingCommodity
{
protected:
	String                           FSymbol;       //合約代碼
	String                           FTradeSymbol;  //交易合約代碼
	nsOrderMessageDefine::MarketEnum FMarket;       //市場別

public:
	String GetSymbol(bool DoLock = true) {return GetStringMemberValue(FSymbol, DoLock);};
	void SetSymbol(const String& Symbol, bool DoLock = true) {SetStringMemberValue(Symbol, FSymbol, DoLock);};
	String GetTradeSymbol(bool DoLock = true) {return GetStringMemberValue(FTradeSymbol, DoLock);};
	void SetTradeSymbol(const String& TradeSymbol, bool DoLock = true) {SetStringMemberValue(TradeSymbol, FTradeSymbol, DoLock);};
	nsOrderMessageDefine::MarketEnum GetMarket(bool DoLock = true) {return GetMarketMemberValue(FMarket, DoLock);};
	void SetMarket(nsOrderMessageDefine::MarketEnum Market, bool DoLock = true);

	CPositionStatisticSymbolKey(const String& ExchangeCode, const String& Symbol, const String& TradeSymbol = L"");
};  //CPositionStatisticSymbolKey
*/
//---------------------------------------------------------------------------
class CMatchSummary : public CTradingBasicObject  //成交資料摘要
{
protected:
	String                                   FOrderId;
	String                                   FExecId;
	String                                   FExchangeCode;  //交易所代碼
	String                                   FSymbol;        //合約代碼
	String                                   FTradeSymbol;   //交易合約代碼
	nsOrderMessageDefine::MarketEnum         FMarket;        //市場別
	nsOrderMessageDefine::SideEnum           FSide;
	nsOrderMessageDefine::PositionEffectEnum FPositionEffect;
	double                                   FMatchPrice;
	int                                      FMatchQuantity;
	double                                   FMatchAmountPerLot;
	double                                   FMatchAmount;
	bool                                     FIsHoldPositionData;  //是否為昨日持倉資料

	double                                   FTaxPerLot;           //每口稅金
	double                                   FTax;                 //稅金
	double                                   FVolumeFeePerLot;     //每口手續費(依口數計算)
	double                                   FVolumeFee;		   //手續費(依口數計算)
	double                                   FPriceFeePerLot;      //每口手續費(依價格計算)
	double                                   FPriceFee;            //手續費(依價格計算)
	double                                   FFeePerLot;           //每口手續費
	double                                   FFee;				   //手續費

public:
	//Constructor
	CMatchSummary(const String& OrderId, const String& ExecId,
				  const String& ExchangeCode, const String& Symbol,
				  nsOrderMessageDefine::MarketEnum Market,
				  nsOrderMessageDefine::SideEnum Side,
				  nsOrderMessageDefine::PositionEffectEnum PositionEffect,
				  double MatchPrice, int MatchQuantity,
				  const String& TradeSymbol = L"");

	//Get/Set Field Function
	String GetOrderId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOrderId, RWLockType);};
	void SetOrderId(const String& OrderId, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(OrderId, FOrderId, RWLockType);};

	String GetExecId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FExecId, RWLockType);};
	void SetExecId(const String& ExecId, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	String GetExchangeCode(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FExchangeCode, RWLockType);};
	void SetExchangeCode(const String& ExchangeCode, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(ExchangeCode, FExchangeCode, RWLockType);};

	String GetSymbol(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSymbol, RWLockType);};
	void SetSymbol(const String& Symbol, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(Symbol, FSymbol, RWLockType);};

	String GetTradeSymbol(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTradeSymbol, RWLockType);};
	void SetTradeSymbol(const String& TradeSymbol, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(TradeSymbol, FTradeSymbol, RWLockType);};

	nsOrderMessageDefine::MarketEnum GetMarket(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FMarket, RWLockType);};
	void SetMarket(nsOrderMessageDefine::MarketEnum Market, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(Market, FMarket, RWLockType);};

	nsOrderMessageDefine::SideEnum GetSide(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSide, RWLockType);};
	void SetSide(nsOrderMessageDefine::SideEnum Side, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(Side, FSide, RWLockType);};

	nsOrderMessageDefine::PositionEffectEnum GetPositionEffect(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FPositionEffect, RWLockType);};
	void SetPositionEffect(nsOrderMessageDefine::PositionEffectEnum PositionEffect, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(PositionEffect, FPositionEffect, RWLockType);};

	double GetMatchPrice(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FMatchPrice, RWLockType);};
	void SetMatchPrice(double MatchPrice, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(MatchPrice, FMatchPrice, RWLockType);};

	int GetMatchQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FMatchQuantity, RWLockType);};
	void SetMatchQuantity(int MatchQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(MatchQuantity, FMatchQuantity, RWLockType);};

	double GetMatchAmountPerLot(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FMatchAmountPerLot, RWLockType);};
	void SetMatchAmountPreLot(double MatchAmountPerLot, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(MatchAmountPerLot, FMatchAmountPerLot, RWLockType);};

	double GetMatchAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FMatchAmount, RWLockType);};
	void SetMatchAmount(double MatchAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(MatchAmount, FMatchAmount, RWLockType);};

	bool IsHoldPositionData(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FIsHoldPositionData, RWLockType);};
	void SetHoldPositionData(bool IsHoldPositionData, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(IsHoldPositionData, FIsHoldPositionData, RWLockType);};

	double GetTaxPerLot(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTaxPerLot, RWLockType);};
	void SetTaxPerLot(double TaxPerLot, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(TaxPerLot, FTaxPerLot, RWLockType);};

	double GetTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTax, RWLockType);};
	void SetTax(double Tax, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(Tax, FTax, RWLockType);};

	double GetVolumeFeePerLot(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FVolumeFeePerLot, RWLockType);};
	void SetVolumeFeePerLot(double VolumeFeePerLot, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(VolumeFeePerLot, FVolumeFeePerLot, RWLockType);};

	double GetVolumeFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FVolumeFee, RWLockType);};
	void SetVolumeFee(double VolumeFee, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(VolumeFee, FVolumeFee, RWLockType);};

	double GetPriceFeePerLot(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FPriceFeePerLot, RWLockType);};
	void SetPriceFeePerLot(double PriceFeePerLot, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(PriceFeePerLot, FPriceFeePerLot, RWLockType);};

	double GetPriceFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FPriceFee, RWLockType);};
	void SetPriceFee(double PriceFee, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(PriceFee, FPriceFee, RWLockType);};

	double GetFeePerLot(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FFeePerLot, RWLockType);};
	void SetFeePerLot(double FeePerLot, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(FeePerLot, FFeePerLot, RWLockType);};

	double GetFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FFee, RWLockType);};
	void SetFee(double Fee, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(Fee, FFee, RWLockType);};

	void CalculateMatchAmount(double ContractMultiplier, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void CalculateTax(double ContractMultiplier, double TaxRate, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void CalculateFee(double ContractMultiplier, double UnitFee, double PriceFeeRate, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void CalculateTaxAndFee(double ContractMultiplier, double TaxRate, double UnitFee, double PriceFeeRate, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void CalculateTradeData(double ContractMultiplier, double TaxRate, double UnitFee, double PriceFeeRate, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CMatchSummary

//---------------------------------------------------------------------------
class COffsetUnitRecord : public CTradingBasicObject  //平倉單位記錄(每筆代表一口)
{
protected:
	double        FCloseProfit;            //已平倉損益
	double        FFloatingProfit;         //未實現損益
	CMatchSummary *FOpenMatchSummaryPtr;   //新倉成交資料摘要
	CMatchSummary *FCloseMatchSummaryPtr;  //平倉成交資料摘要
	double        FLastPrice;              //最新成交價

public:
	//Constructor
	COffsetUnitRecord(CMatchSummary *OpenMatchSummaryPtr = 0, CMatchSummary *CloseMatchSummaryPtr = 0);

	//Get/Set Field Function
	double GetCloseProfit(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCloseProfit, RWLockType);};

	double GetFloatingProfit(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FFloatingProfit, RWLockType);};

	CMatchSummary *GetOpenMatchSummary(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOpenMatchSummaryPtr, RWLockType);};
	void SetOpenMatchSummary(CMatchSummary *OpenMatchSummaryPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(OpenMatchSummaryPtr, FOpenMatchSummaryPtr, RWLockType);};

	CMatchSummary *GetCloseMatchSummary(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCloseMatchSummaryPtr, RWLockType);};
	void SetCloseMatchSummary(CMatchSummary *CloseMatchSummaryPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	double GetLastPrice(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FLastPrice, RWLockType);};

	double GetOpenMatchPrice(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	double GetCloseMatchPrice(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	void GetMatchPrice(double& OpenMatchPrice, double& CloseMatchPrice, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	double CalculateCloseProfit(ReadWrietLockTypeEnum RWLockType = rwNotLock);
	double CalculateFloatingProfit(double ContractMultiplier, double LastPrice, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	void SetLastPrice(double ContractMultiplier, double LastPrice, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //COffsetUnitRecord

//---------------------------------------------------------------------------
class CMatchPriceStatisticRecord : public CTradingBasicObject
{
public:
	static String GenerateMatchPriceKey(double MatchPrice);

protected:
	nsOrderMessageDefine::SideEnum FSide;
	double                         FMatchPrice;         //成交價格
	double                         FContractMultiplier; //合約乘數
	double                         FAmountPerLot;       //每單位(口)金額

	int                            FOpenInterestQuantity;  //未平倉數量
	double                         FOpenInterestAmount;    //未平倉金額
	int                            FDayTradeOpenQuantity;  //當日沖銷數量
	double                         FDayTradeOpenAmount;    //當日沖銷金額
	int                            FOpenQuantity;          //新倉數量
	double                         FOpenAmount;            //新倉金額
	int                            FRolledQuantity;        //平今數量
	double                         FRolledAmount;          //平今金額
	int                            FCloseQuantity;         //平倉數量
	double                         FCloseAmount;           //平倉金額
	int                            FTotalMatchQuantity;    //全部成交數量

private:
	void SetOpenInterestQuantity(int OpenInterestQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void SetDayTradeOpenQuantity(int DayTradeOpenQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int InceraseDayTradeOpenQuantity(int InceraseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void SetOpenQuantity(int OpenQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int InceraseOpenQuantity(int InceraseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void SetRolledQuantity(int RolledQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int InceraseRolledQuantity(int InceraseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void SetCloseQuantity(int CloseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int InceraseCloseQuantity(int InceraseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);

 public:
	//Constructor
	CMatchPriceStatisticRecord(nsOrderMessageDefine::SideEnum Side, double MatchPrice, double ContractMultiplier = 1.0);

	//Get/Set Field Function
	nsOrderMessageDefine::SideEnum GetSide(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSide, RWLockType);};

	double GetMatchPrice(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FMatchPrice, RWLockType);};

	double GetContractMultiplier(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FContractMultiplier, RWLockType);};
	void SetContractMultiplier(double ContractMultiplier, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	double GetAmountPerLot(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FAmountPerLot, RWLockType);};

	int GetOpenInterestQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOpenInterestQuantity, RWLockType);};
	int InceraseOpenInterestQuantity(int InceraseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	double GetOpenInterestAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOpenInterestAmount, RWLockType);};

	int GetDayTradeOpenQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FDayTradeOpenQuantity, RWLockType);};

	double GetDayTradeOpenAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FDayTradeOpenAmount, RWLockType);};

	int GetOpenQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOpenQuantity, RWLockType);};

	double GetOpenAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOpenAmount, RWLockType);};

	int GetRolledQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FRolledQuantity, RWLockType);};

	double GetRolledAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FRolledAmount, RWLockType);};

	int GetCloseQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCloseQuantity, RWLockType);};

	double GetCloseAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCloseAmount, RWLockType);};

	int GetTotalMatchQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTotalMatchQuantity, RWLockType);};

	int InceraseTotalMatchQuantity(int InceraseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	int GetMatchQuantity(nsOrderMessageDefine::PositionEffectEnum PositionEffect, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	int InceraseMatchQuantity(nsOrderMessageDefine::PositionEffectEnum PositionEffect, int InceraseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CMatchPriceStatisticRecord

//---------------------------------------------------------------------------
class CPositionStatisticRecord : public CExchangeSymbolKey
{
protected:
	double FTaxRate;			//交易稅率
	double FOpenFeeRate;		//新倉手續費率
	double FOpenUnitFee;		//新倉單位手續費
	double FCloseFeeRate;		//平倉手續費率
	double FCloseUnitFee;		//平倉單位手續費
	double FCloseTodayFeeRate;  //平今手續費率
	double FCloseTodayUnitFee;  //平今單位手續費
	double FContractMultiplier; //合約乘數

	//買邊統計資料
	int FBuyOpenInterestQuantity;   //買邊未平倉數量
	double FBuyOpenInterestAmount;  //買邊未平倉金額
	double FBuyOpenInterestTax;     //買邊未平倉稅金
	double FBuyOpenInterestFee;     //買邊未平倉手續費
	int FBuyHoldOpenQuantity;		//買邊昨日留倉數量
	double FBuyHoldOpenAmount;      //買邊昨日留倉金額
	double FBuyHoldOpenTax;         //買邊昨日留倉稅金
	double FBuyHoldOpenFee;         //買邊昨日留倉手續費
	int FBuyDayTradeOpenQuantity;   //買邊當日沖銷數量
	double FBuyDayTradeOpenAmount;  //買邊當日沖銷金額
	double FBuyDayTradeOpenTax;     //買邊當日沖銷稅金
	double FBuyDayTradeOpenFee;     //買邊當日沖銷手續費
	int FBuyOpenQuantity;           //買邊新倉數量
	double FBuyOpenAmount;          //買邊新倉金額
	double FBuyOpenTax;             //買邊新倉稅金
	double FBuyOpenFee;             //買邊新倉手續費
	int FBuyRolledQuantity;         //買邊平今數量
	double FBuyRolledAmount;        //買邊平今金額
	double FBuyRolledTax;           //買邊平今稅金
	double FBuyRolledFee;           //買邊平今手續費
	int FBuyCloseQuantity;          //買邊平倉數量
	double FBuyCloseAmount;         //買邊平倉金額
	double FBuyCloseTax;            //買邊平倉稅金
	double FBuyCloseFee;            //買邊平倉手續費
	int FBuyQuantity;               //買邊數量
	double FBuyAmount;              //買邊金額
	double FBuyTax;                 //買邊稅金
	double FBuyFee;                 //買邊手續費
	double FBuyCloseProfit;         //買邊平倉損益
	double FBuyFloatingProfit;      //買邊未實現損益

	//賣邊統計資料
	int FSellOpenInterestQuantity;   //賣邊未平倉數量
	double FSellOpenInterestAmount;  //賣邊未平倉金額
	double FSellOpenInterestTax;     //賣邊未平倉稅金
	double FSellOpenInterestFee;     //賣邊未平倉手續費
	int FSellHoldOpenQuantity;		 //賣邊昨日留倉數量
	double FSellHoldOpenAmount;      //賣邊昨日留倉金額
	double FSellHoldOpenTax;         //賣邊昨日留倉稅金
	double FSellHoldOpenFee;         //賣邊昨日留倉手續費
	int FSellDayTradeOpenQuantity;   //賣邊當日沖銷數量
	double FSellDayTradeOpenAmount;  //賣邊當日沖銷金額
	double FSellDayTradeOpenTax;     //賣邊當日沖銷稅金
	double FSellDayTradeOpenFee;     //賣邊當日沖銷手續費
	int FSellOpenQuantity;           //賣邊新倉數量
	double FSellOpenAmount;          //賣邊新倉金額
	double FSellOpenTax;             //賣邊新倉稅金
	double FSellOpenFee;             //賣邊新倉手續費
	int FSellRolledQuantity;         //賣邊平今數量
	double FSellRolledAmount;        //賣邊平今金額
	double FSellRolledTax;           //賣邊平今稅金
	double FSellRolledFee;           //賣邊平今手續費
	int FSellCloseQuantity;          //賣邊平倉數量
	double FSellCloseAmount;         //賣邊平倉金額
	double FSellCloseTax;            //賣邊平倉稅金
	double FSellCloseFee;            //賣邊平倉手續費
	int FSellQuantity;               //賣邊數量
	double FSellAmount;              //賣邊金額
	double FSellTax;                 //賣邊稅金
	double FSellFee;                 //賣邊手續費
	double FSellCloseProfit;         //賣邊平倉損益
	double FSellFloatingProfit;      //賣邊未實現損益

	//整體統計資料
	double FTax;              //稅金
	double FFee;              //手續費
	double FCloseProfit;      //平倉損益
	double FOpenInterestTax;  //未平倉稅金
	double FOpenInterestFee;  //未平倉手續費
	double FFloatingProfit;   //未實現損益

	//MarketDataStore 訂閱資料
	int  FSubscribeCount;        //訂閱次數
	bool FIsMarketDataReceived;  //是否收到定閱資料

	//OnMarketDataUpdate 事件資料
	double FLastPrice;               //最新成交價(OnMarketDataUpdate 事件)
	int    FBestBuyQuantity1;        //最佳五檔買邊第一檔數量(OnMarketDataUpdate 事件)
	double FBestBuyPrice1;           //最佳五檔買邊第一檔價格(OnMarketDataUpdate 事件)
	int    FBestSellQuantity1;       //最佳五檔賣邊第一檔數量(OnMarketDataUpdate 事件)
	double FBestSellPrice1;          //最佳五檔賣邊第一檔價格(OnMarketDataUpdate 事件)
	int    FMarketTradeQuantitySum;  //OnMarketDataUpdate 事件中 MarketDataMessage.GetTradeQty() 值的總和
	DWORD  FLastTickCount;           //The number of milliseconds that have elapsed since the system was started.
	bool   FIsWaitingForProcess;

	CTradingContract  *FContractPtr;    //合約物件
	CTradingCommodity *FCommodityPtr;   //商品物件
	CTaxRateObject    *FTaxRateObjPtr;  //稅率物件
	CFeeObject        *FFeeObjPtr;      //手續費率物件

	//昨日留倉摘要
	CTradingObjPtrVector FHoldMatchSummaryArray;

	//成交摘要
	CWStrTradingObjPtrHMap FMatchSummaryExecIdIndex;
	CTradingObjPtrVector   FMatchSummaryArray;

	//Open Interest Queue未平倉記錄
	CTradingObjQueue FBuyDayTradeOIQueue;
	CTradingObjQueue FBuyHoldOIQueue;
	CTradingObjQueue FBuyOIQueue;
	CTradingObjQueue FSellDayTradeOIQueue;
	CTradingObjQueue FSellHoldOIQueue;
	CTradingObjQueue FSellOIQueue;

	//平倉記錄
	CTradingObjPtrVector FBuyCloseTodayOffsetArray;
	CTradingObjPtrVector FBuyCloseOffsetArray;
	CTradingObjPtrVector FSellCloseTodayOffsetArray;
	CTradingObjPtrVector FSellCloseOffsetArray;

	//依價格統計成交量
	CWStrTradingObjPtrHMap FBuyPxQtyStatisticPriceIndex;
	CTradingObjPtrVector   FBuyPxQtyStatisticArray;
	CWStrTradingObjPtrHMap FSellPxQtyStatisticPriceIndex;
	CTradingObjPtrVector   FSellPxQtyStatisticArray;

 public:
	//Constructor
	CPositionStatisticRecord(const String& ExchangeCode, const String& Symbol, const String& TradeSymbol = L"");
	~CPositionStatisticRecord();

	//Get/Set Field Function
	double GetTaxRate(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTaxRate, RWLockType);};
	void SetTaxRate(double TaxRate, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(TaxRate, FTaxRate, RWLockType);};

	double GetOpenFeeRate(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOpenFeeRate, RWLockType);};
	void SetOpenFeeRate(double OpenFeeRate, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(OpenFeeRate, FOpenFeeRate, RWLockType);};

	double GetOpenUnitFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOpenUnitFee, RWLockType);};
	void SetOpenUnitFee(double OpenUnitFee, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(OpenUnitFee, FOpenUnitFee, RWLockType);};

	double GetCloseFeeRate(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCloseFeeRate, RWLockType);};
	void SetCloseFeeRate(double CloseFeeRate, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CloseFeeRate, FCloseFeeRate, RWLockType);};

	double GetCloseUnitFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCloseUnitFee, RWLockType);};
	void SetCloseUnitFee(double CloseUnitFee, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CloseUnitFee, FCloseUnitFee, RWLockType);};

	double GetCloseTodayFeeRate(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCloseTodayFeeRate, RWLockType);};
	void SetCloseTodayFeeRate(double CloseTodayFeeRate, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CloseTodayFeeRate, FCloseTodayFeeRate, RWLockType);};

	double GetCloseTodayUnitFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCloseTodayUnitFee, RWLockType);};
	void SetCloseTodayUnitFee(double CloseTodayUnitFee, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CloseTodayUnitFee, FCloseTodayUnitFee, RWLockType);};

	double GetContractMultiplier(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FContractMultiplier, RWLockType);};
	void SetContractMultiplier(double ContractMultiplier, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(ContractMultiplier, FContractMultiplier, RWLockType);};

	//買邊統計資料
	int GetBuyOpenInterestQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyOpenInterestQuantity, RWLockType);};
	int IncreaseBuyOpenInterestQuantity(int IncreaseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseQuantity, FBuyOpenInterestQuantity, constKeepPositive, RWLockType);};

	double GetBuyOpenInterestAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyOpenInterestAmount, RWLockType);};
	double IncreaseBuyOpenInterestAmount(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyOpenInterestAmount, constKeepPositive, RWLockType);};

	double GetBuyOpenInterestTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyOpenInterestTax, RWLockType);};
	double IncreaseBuyOpenInterestTax(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyOpenInterestTax, constKeepPositive, RWLockType);};

	double GetBuyOpenInterestFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyOpenInterestFee, RWLockType);};
	double IncreaseBuyOpenInterestFee(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyOpenInterestFee, constKeepPositive, RWLockType);};

	int GetBuyDayTradeOpenQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyDayTradeOpenQuantity, RWLockType);};
	int IncreaseBuyDayTradeOpenQuantity(int IncreaseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseQuantity, FBuyDayTradeOpenQuantity, constKeepPositive, RWLockType);};

	double GetBuyDayTradeOpenAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyDayTradeOpenAmount, RWLockType);};
	double IncreaseBuyDayTradeOpenAmount(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyDayTradeOpenAmount, constKeepPositive, RWLockType);};

	double GetBuyDayTradeOpenTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyDayTradeOpenTax, RWLockType);};
	double IncreaseBuyDayTradeOpenTax(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyDayTradeOpenTax, constKeepPositive, RWLockType);};

	double GetBuyDayTradeOpenFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyDayTradeOpenFee, RWLockType);};
	double IncreaseBuyDayTradeOpenFee(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyDayTradeOpenFee, constKeepPositive, RWLockType);};

	int GetBuyHoldOpenQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyHoldOpenQuantity, RWLockType);};
	int IncreaseBuyHoldOpenQuantity(int IncreaseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseQuantity, FBuyHoldOpenQuantity, constKeepPositive, RWLockType);};

	double GetBuyHoldOpenAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyHoldOpenAmount, RWLockType);};
	double IncreaseBuyHoldOpenAmount(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyHoldOpenAmount, constKeepPositive, RWLockType);};

	double GetBuyHoldOpenTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyHoldOpenTax, RWLockType);};
	double IncreaseBuyHoldOpenTax(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyHoldOpenTax, constKeepPositive, RWLockType);};

	double GetBuyHoldOpenFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyHoldOpenFee, RWLockType);};
	double IncreaseBuyHoldOpenFee(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyHoldOpenFee, constKeepPositive, RWLockType);};

	int GetBuyOpenQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyOpenQuantity, RWLockType);};
	int IncreaseBuyOpenQuantity(int IncreaseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseQuantity, FBuyOpenQuantity, constKeepPositive, RWLockType);};

	double GetBuyOpenAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyOpenAmount, RWLockType);};
	double IncreaseBuyOpenAmount(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyOpenAmount, constKeepPositive, RWLockType);};

	double GetBuyOpenTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyOpenTax, RWLockType);};
	double IncreaseBuyOpenTax(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyOpenTax, constKeepPositive, RWLockType);};

	double GetBuyOpenFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyOpenFee, RWLockType);};
	double IncreaseBuyOpenFee(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyOpenFee, constKeepPositive, RWLockType);};

	int GetBuyRolledQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyRolledQuantity, RWLockType);};
	int IncreaseBuyRolledQuantity(int IncreaseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseQuantity, FBuyRolledQuantity, constKeepPositive, RWLockType);};

	double GetBuyRolledAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyRolledAmount, RWLockType);};
	double IncreaseBuyRolledAmount(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyRolledAmount, constKeepPositive, RWLockType);};

	double GetBuyRolledTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyRolledTax, RWLockType);};
	double IncreaseBuyRolledTax(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyRolledTax, constKeepPositive, RWLockType);};

	double GetBuyRolledFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyRolledFee, RWLockType);};
	double IncreaseBuyRolledFee(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyRolledFee, constKeepPositive, RWLockType);};

	int GetBuyCloseQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyCloseQuantity, RWLockType);};
	int IncreaseBuyCloseQuantity(int IncreaseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseQuantity, FBuyCloseQuantity, constKeepPositive, RWLockType);};

	double GetBuyCloseAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyCloseAmount, RWLockType);};
	double IncreaseBuyCloseAmount(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyCloseAmount, constKeepPositive, RWLockType);};

	double GetBuyCloseTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyCloseTax, RWLockType);};
	double IncreaseBuyCloseTax(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyCloseTax, constKeepPositive, RWLockType);};

	double GetBuyCloseFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyCloseFee, RWLockType);};
	double IncreaseBuyCloseFee(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyCloseFee, constKeepPositive, RWLockType);};

	int GetBuyQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyQuantity, RWLockType);};
	int IncreaseBuyQuantity(int IncreaseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseQuantity, FBuyQuantity, constKeepPositive, RWLockType);};

	double GetBuyAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyAmount, RWLockType);};
	double IncreaseBuyAmount(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyAmount, constKeepPositive, RWLockType);};

	double GetBuyTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyTax, RWLockType);};
	double IncreaseBuyTax(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyTax, constKeepPositive, RWLockType);};

	double GetBuyFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyFee, RWLockType);};
	double IncreaseBuyFee(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyFee, constKeepPositive, RWLockType);};

	double GetBuyCloseProfit(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyCloseProfit, RWLockType);};
	double IncreaseBuyCloseProfit(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FBuyCloseProfit, constNotKeepPositive, RWLockType);};

	double GetOpenInterestTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOpenInterestTax, RWLockType);};
	double GetOpenInterestFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOpenInterestFee, RWLockType);};
	double GetBuyFloatingProfit(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyFloatingProfit, RWLockType);};

	//賣邊統計資料
	int GetSellOpenInterestQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellOpenInterestQuantity, RWLockType);};
	int IncreaseSellOpenInterestQuantity(int IncreaseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseQuantity, FSellOpenInterestQuantity, constKeepPositive, RWLockType);};

	double GetSellOpenInterestAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellOpenInterestAmount, RWLockType);};
	double IncreaseSellOpenInterestAmount(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellOpenInterestAmount, constKeepPositive, RWLockType);};

	double GetSellOpenInterestTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellOpenInterestTax, RWLockType);};
	double IncreaseSellOpenInterestTax(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellOpenInterestTax, constKeepPositive, RWLockType);};

	double GetSellOpenInterestFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellOpenInterestFee, RWLockType);};
	double IncreaseSellOpenInterestFee(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellOpenInterestFee, constKeepPositive, RWLockType);};

	int GetSellDayTradeOpenQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellDayTradeOpenQuantity, RWLockType);};
	int IncreaseSellDayTradeOpenQuantity(int IncreaseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseQuantity, FSellDayTradeOpenQuantity, constKeepPositive, RWLockType);};

	double GetSellDayTradeOpenAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellDayTradeOpenAmount, RWLockType);};
	double IncreaseSellDayTradeOpenAmount(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellDayTradeOpenAmount, constKeepPositive, RWLockType);};

	double GetSellDayTradeOpenTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellDayTradeOpenTax, RWLockType);};
	double IncreaseSellDayTradeOpenTax(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellDayTradeOpenTax, constKeepPositive, RWLockType);};

	double GetSellDayTradeOpenFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellDayTradeOpenFee, RWLockType);};
	double IncreaseSellDayTradeOpenFee(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellDayTradeOpenFee, constKeepPositive, RWLockType);};

	int GetSellHoldOpenQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellHoldOpenQuantity, RWLockType);};
	int IncreaseSellHoldOpenQuantity(int IncreaseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseQuantity, FSellHoldOpenQuantity, constKeepPositive, RWLockType);};

	double GetSellHoldOpenAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellHoldOpenAmount, RWLockType);};
	double IncreaseSellHoldOpenAmount(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellHoldOpenAmount, constKeepPositive, RWLockType);};

	double GetSellHoldOpenTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellHoldOpenTax, RWLockType);};
	double IncreaseSellHoldOpenTax(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellHoldOpenTax, constKeepPositive, RWLockType);};

	double GetSellHoldOpenFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellHoldOpenFee, RWLockType);};
	double IncreaseSellHoldOpenFee(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellHoldOpenFee, constKeepPositive, RWLockType);};

	int GetSellOpenQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellOpenQuantity, RWLockType);};
	int IncreaseSellOpenQuantity(int IncreaseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseQuantity, FSellOpenQuantity, constKeepPositive, RWLockType);};

	double GetSellOpenAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellOpenAmount, RWLockType);};
	double IncreaseSellOpenAmount(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellOpenAmount, constKeepPositive, RWLockType);};

	double GetSellOpenTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellOpenTax, RWLockType);};
	double IncreaseSellOpenTax(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellOpenTax, constKeepPositive, RWLockType);};

	double GetSellOpenFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellOpenFee, RWLockType);};
	double IncreaseSellOpenFee(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellOpenFee, constKeepPositive, RWLockType);};

	int GetSellRolledQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellRolledQuantity, RWLockType);};
	int IncreaseSellRolledQuantity(int IncreaseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseQuantity, FSellRolledQuantity, constKeepPositive, RWLockType);};

	double GetSellRolledAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellRolledAmount, RWLockType);};
	double IncreaseSellRolledAmount(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellRolledAmount, constKeepPositive, RWLockType);};

	double GetSellRolledTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellRolledTax, RWLockType);};
	double IncreaseSellRolledTax(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellRolledTax, constKeepPositive, RWLockType);};

	double GetSellRolledFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellRolledFee, RWLockType);};
	double IncreaseSellRolledFee(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellRolledFee, constKeepPositive, RWLockType);};

	int GetSellCloseQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellCloseQuantity, RWLockType);};
	int IncreaseSellCloseQuantity(int IncreaseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseQuantity, FSellCloseQuantity, constKeepPositive, RWLockType);};

	double GetSellCloseAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellCloseAmount, RWLockType);};
	double IncreaseSellCloseAmount(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellCloseAmount, constKeepPositive, RWLockType);};

	double GetSellCloseTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellCloseTax, RWLockType);};
	double IncreaseSellCloseTax(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellCloseTax, constKeepPositive, RWLockType);};

	double GetSellCloseFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellCloseFee, RWLockType);};
	double IncreaseSellCloseFee(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellCloseFee, constKeepPositive, RWLockType);};

	int GetSellQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellQuantity, RWLockType);};
	int IncreaseSellQuantity(int IncreaseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseQuantity, FSellQuantity, constKeepPositive, RWLockType);};

	double GetSellAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellAmount, RWLockType);};
	double IncreaseSellAmount(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellAmount, constKeepPositive, RWLockType);};

	double GetSellTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellTax, RWLockType);};
	double IncreaseSellTax(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellTax, constKeepPositive, RWLockType);};

	double GetSellFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellFee, RWLockType);};
	double IncreaseSellFee(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellFee, constKeepPositive, RWLockType);};

	double GetSellCloseProfit(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellCloseProfit, RWLockType);};
	double IncreaseSellCloseProfit(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellCloseProfit, constNotKeepPositive, RWLockType);};

	double GetSellFloatingProfit(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellFloatingProfit, RWLockType);};
	double IncreaseSellFloatingProfit(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FSellFloatingProfit, constNotKeepPositive, RWLockType);};

	//整體統計資料
	double GetTax(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTax, RWLockType);};
	double IncreaseTax(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FTax, constKeepPositive, RWLockType);};

	double GetFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FFee, RWLockType);};
	double IncreaseFee(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FFee, constKeepPositive, RWLockType);};

	double GetCloseProfit(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCloseProfit, RWLockType);};
	double IncreaseCloseProfit(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FCloseProfit, constNotKeepPositive, RWLockType);};

	double GetFloatingProfit(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FFloatingProfit, RWLockType);};
	double IncreaseFloatingProfit(double IncreaseAmount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseAmount, FFloatingProfit, constNotKeepPositive, RWLockType);};

	//訂閱資料
	int GetSubscribeCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSubscribeCount, RWLockType);};
	void SetSubscribeCount(int SubscribeCount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(SubscribeCount, FSubscribeCount, RWLockType);};
	int IncreaseSubscribeCount(int IncreaseCount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseCount, FSubscribeCount, constKeepPositive, RWLockType);};

	bool IsMarketDataReceived(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FIsMarketDataReceived, RWLockType);};
	void SetMarketDataReceived(bool IsMarketDataReceived, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(IsMarketDataReceived, FIsMarketDataReceived, RWLockType);};

	//OnMarketDataUpdate 事件資料
	double GetLastPrice(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FLastPrice, RWLockType);}
	void SetLastPrice(double LastPrice, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	int GetBestBuyQuantity1(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBestBuyQuantity1, RWLockType);}
	void SetBestBuyQuantity1(int BestBuyQuantity1, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BestBuyQuantity1, FBestBuyQuantity1, RWLockType);};

	double GetBestBuyPrice1(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBestBuyPrice1, RWLockType);}
	void SetBestBuyPrice1(double BestBuyPrice1, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BestBuyPrice1, FBestBuyPrice1, RWLockType);};

	int GetBestSellQuantity1(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBestSellQuantity1, RWLockType);}
	void SetBestSellQuantity1(int BestSellQuantity1, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BestSellQuantity1, FBestSellQuantity1, RWLockType);};

	double GetBestSellPrice1(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBestSellPrice1, RWLockType);}
	void SetBestSellPrice1(double BestSellPrice1, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BestSellPrice1, FBestSellPrice1, RWLockType);};

	int GetMarketTradeQuantitySum(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FMarketTradeQuantitySum, RWLockType);};
	void SetMarketTradeQuantitySum(int MarketTradeQuantitySum, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(MarketTradeQuantitySum, FMarketTradeQuantitySum, RWLockType);};
	int IncreaseMarketTradeQuantitySum(int IncreaseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return IncreaseFieldValue(IncreaseQuantity, FMarketTradeQuantitySum, constKeepPositive, RWLockType);};

	DWORD GetLastTickCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FLastTickCount, RWLockType);};
	void SetLastTickCount(DWORD LastTickCount, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(LastTickCount, FLastTickCount, RWLockType);};

	bool IsWaitingForProcess(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FIsWaitingForProcess, RWLockType);};
	void SetWaitingForProcess(bool IsWaitingForProcess, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(IsWaitingForProcess, FIsWaitingForProcess, RWLockType);};

	CTradingContract *GetContractObject(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FContractPtr, RWLockType);};
	void SetContractObject(CTradingContract *ContractPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(ContractPtr, FContractPtr, RWLockType);};

	CTradingCommodity *GetCommodityObject(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCommodityPtr, RWLockType);};
	void SetCommodityObject(CTradingCommodity *CommodityPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CommodityPtr, FCommodityPtr, RWLockType);};

	CTaxRateObject *GetTaxRateObject(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTaxRateObjPtr, RWLockType);};
	void SetTaxRateObject(CTaxRateObject *TaxRateObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	CFeeObject *GetFeeObject(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FFeeObjPtr, RWLockType);};
	void SetFeeObject(CFeeObject *FeeObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	double GetPriceFeeRate(nsOrderMessageDefine::PositionEffectEnum PositionEffect, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	double GetUnitFee(nsOrderMessageDefine::PositionEffectEnum PositionEffect, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void GetFee(nsOrderMessageDefine::PositionEffectEnum PositionEffect,
				double& FeeRate, double& UnitFee, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	int GetHoldMatchSummaryArrayCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FHoldMatchSummaryArray, FCSLockPtr, RWLockType);};
	CMatchSummary *GetHoldMatchSummaryFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AppendHoldMatchSummaryToArray(CMatchSummary *MatchSummaryPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearHoldMatchSummaryArray(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	int GetMatchSummaryExecIdIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FMatchSummaryExecIdIndex, FCSLockPtr, RWLockType);};
	CMatchSummary *FindMatchSummaryFromExecIdIndex(const String& ExecId, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertMatchSummaryIntoExecIdIndex(const String& ExecId, CMatchSummary *MatchSummaryPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CMatchSummary *RemoveMatchSummaryFromExecIdIndex(const String& ExecId, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearMatchSummaryExecIdIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	int GetMatchSummaryArrayCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FMatchSummaryArray, FCSLockPtr, RWLockType);};
	CMatchSummary *GetMatchSummaryFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AppendMatchSummaryToArray(CMatchSummary *MatchSummaryPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearMatchSummaryArray(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	int GetBuyPxQtyStatisticPriceIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FBuyPxQtyStatisticPriceIndex, FCSLockPtr, RWLockType);};
	CMatchPriceStatisticRecord *FindBuyPxQtyStatisticFromPriceIndex(double MatchPrice, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertBuyPxQtyStatisticIntoPriceIndex(double MatchPrice, CMatchPriceStatisticRecord *PxQtyStatisticPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CMatchPriceStatisticRecord *RemoveBuyPxQtyStatisticFromPriceIndex(double MatchPrice, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearBuyPxQtyStatisticPriceIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	int GetBuyPxQtyStatisticArrayCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FBuyPxQtyStatisticArray, FCSLockPtr, RWLockType);};
	CMatchPriceStatisticRecord *GetBuyPxQtyStatisticFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AppendBuyPxQtyStatisticToArray(CMatchPriceStatisticRecord *PxQtyStatisticPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearBuyPxQtyStatisticArray(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	int GetSellPxQtyStatisticPriceIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FSellPxQtyStatisticPriceIndex, FCSLockPtr, RWLockType);};
	CMatchPriceStatisticRecord *FindSellPxQtyStatisticFromPriceIndex(double MatchPrice, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertSellPxQtyStatisticIntoPriceIndex(double MatchPrice, CMatchPriceStatisticRecord *PxQtyStatisticPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CMatchPriceStatisticRecord *RemoveSellPxQtyStatisticFromPriceIndex(double MatchPrice, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearSellPxQtyStatisticPriceIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	int GetSellPxQtyStatisticArrayCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FSellPxQtyStatisticArray, FCSLockPtr, RWLockType);};
	CMatchPriceStatisticRecord *GetSellPxQtyStatisticFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AppendSellPxQtyStatisticToArray(CMatchPriceStatisticRecord *PxQtyStatisticPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearSellPxQtyStatisticArray(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	CMatchPriceStatisticRecord *FindOrCreatPxQtyStatistic(nsOrderMessageDefine::SideEnum Side,
														  double MatchPrice, ReadWrietLockTypeEnum RWLockType = rwNotLock);

protected:
	int GetOIQueueCount(CTradingObjQueue& OIQueue, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(OIQueue, FCSLockPtr, RWLockType);};
	COffsetUnitRecord *GetOffsetUnitRecFromOIQueue(CTradingObjQueue& OIQueue, int QueueIndex, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void PushOffsetUnitRecToOIQueue(CTradingObjQueue& OIQueue, COffsetUnitRecord *OffsetUnitRecPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	COffsetUnitRecord *GetFirstOffsetUnitRecOfOIQueue(CTradingObjQueue& OIQueue, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void PopOffsetUnitRecFromOIQueue(CTradingObjQueue& OIQueue, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearOIQueue(CTradingObjQueue& OIQueue, bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	int GetOffsetUnitRecArrayCount(CTradingObjPtrVector& OffsetArray, ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(OffsetArray, FCSLockPtr, RWLockType);};
	COffsetUnitRecord *GetOffsetUnitRecFromArray(int ArrayIndex, CTradingObjPtrVector& OffsetArray, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int AppendOffsetUnitRecToArray(CTradingObjPtrVector& OffsetArray, COffsetUnitRecord *OffsetUnitRecPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearOffsetUnitRecArray(CTradingObjPtrVector& OffsetArray, bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	int OffsetOpenInterestQueue(nsOrderMessageDefine::SideEnum CloseSide,
								nsOrderMessageDefine::PositionEffectEnum ClosePositionEffect,
								int CloseMatchQuantity, CMatchSummary *CloseMatchSummaryPtr, CTradingObjQueue& OpenOIQueue);
	int OffsetOpenInterest(nsOrderMessageDefine::SideEnum CloseSide,
						   nsOrderMessageDefine::PositionEffectEnum ClosePositionEffect,
						   int CloseMatchQuantity, double CloseMatchAmount,
						   double CloseTax, double CloseFee,
						   CMatchSummary *CloseMatchSummaryPtr);
	void AddOpenInterest(int MatchQuantity, CMatchSummary *MatchSummaryPtr, CTradingObjQueue& OIQueue);

	bool ProcessMatchSummary(CMatchSummary *MatchSummaryPtr);
	bool ProcessMatchSummaryBySide(CMatchSummary *MatchSummaryPtr);
	int CalculateOIQueueFloatingData(double ContractMultiplier, double LastPrice, CTradingObjQueue& OIQueue, double& Profit, double& Tax, double& Fee);

 public:
	double CalculateFloatingData(double ContractMultiplier, double LastPrice, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	double CalculateFloatingData(ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AddFillReportData(const String& OrderId, const String& ExecId,
						   nsOrderMessageDefine::MarketEnum Market,
						   nsOrderMessageDefine::SideEnum Side,
						   nsOrderMessageDefine::PositionEffectEnum PositionEffect,
						   double MatchPrice, int MatchQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AddHoldPosition(const String& OrderId, const String& ExecId,
						 nsOrderMessageDefine::MarketEnum Market,
						 nsOrderMessageDefine::SideEnum Side,
						 nsOrderMessageDefine::PositionEffectEnum PositionEffect,
						 double MatchPrice, int MatchQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void ReOffsetPosition(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	void DumpTitle(ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpFee(ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpQuantitySummary(ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);

};  //CPositionStatisticRecord
};  //namespace s888
#endif
