//---------------------------------------------------------------------------


#pragma hdrstop

#include "TradingPosition.h"
namespace s888
{
/*
//----------------------- CPositionStatisticSymbolKey -----------------------
//---------------------------------------------------------------------------
void CPositionStatisticSymbolKey::SetMarket(nsOrderMessageDefine::MarketEnum Market, bool DoLock)
{
	if (DoLock) Lock();
	FMarket = Market;
	if (DoLock) Unlock();
}  //CPositionStatisticSymbolKey::SetMarket()
//---------------------------------------------------------------------------
CPositionStatisticSymbolKey::CPositionStatisticSymbolKey(const String& ExchangeCode, const String& Symbol, const String& TradeSymbol)
:CBasicTradingCommodity(ExchangeCode),
 FSymbol(Symbol),
 FTradeSymbol(TradeSymbol),
 FMarket(nsOrderMessageDefine::mOTC)
{
	if (FTradeSymbol.Length() <= 0) FTradeSymbol = FSymbol;
}  //CPositionStatisticSymbolKey::CPositionStatisticSymbolKey()
*/
//------------------------------ CMatchSummary ------------------------------
//---------------------------------------------------------------------------
CMatchSummary::CMatchSummary(const String& OrderId, const String& ExecId,
							 const String& ExchangeCode, const String& Symbol,
							 nsOrderMessageDefine::MarketEnum Market,
							 nsOrderMessageDefine::SideEnum Side,
							 nsOrderMessageDefine::PositionEffectEnum PositionEffect,
							 double MatchPrice, int MatchQuantity,
							 const String& TradeSymbol)
:CTradingBasicObject(constCreateRWLock)
,FOrderId(OrderId)
,FExecId(ExecId)
,FExchangeCode(ExchangeCode)
,FSymbol(Symbol)
,FTradeSymbol(TradeSymbol)
,FMarket(Market)
,FSide(Side)
,FPositionEffect(PositionEffect)
,FMatchPrice(MatchPrice)
,FMatchQuantity(MatchQuantity)
,FMatchAmountPerLot(0.0)
,FMatchAmount(0.0)
,FIsHoldPositionData(false)
,FTaxPerLot(0.0)
,FTax(0.0)
,FVolumeFeePerLot(0.0)
,FVolumeFee(0.0)
,FPriceFeePerLot(0.0)
,FPriceFee(0.0)
,FFeePerLot(0.0)
,FFee(0.0)
{
	FMainKey = FExecId;
}  //CMatchSummary::CMatchSummary()
//---------------------------------------------------------------------------
void CMatchSummary::SetExecId(const String& ExecId, ReadWrietLockTypeEnum RWLockType)
{
	SetFieldValue(ExecId, FExecId, RWLockType);
	FMainKey = FExecId;
}  //CMatchSummary::SetExecId()
//---------------------------------------------------------------------------
void CMatchSummary::CalculateMatchAmount(double ContractMultiplier, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FMatchAmountPerLot = FMatchPrice * ContractMultiplier;
	FMatchAmount = FMatchAmountPerLot * FMatchQuantity;
	Unlock(RWLockType);
}  //CMatchSummary::CalculateMatchAmount()
//---------------------------------------------------------------------------
void CMatchSummary::CalculateTax(double ContractMultiplier, double TaxRate, ReadWrietLockTypeEnum RWLockType)
{   //計算方式：tickvalue = 每點價值(期貨 FITX=200, FITF=1000, FITE=4000, TXO=50)
	//(成交價 * tickvalue * 稅額 ) -> 四捨五入後 * 口數
	Lock(RWLockType);
	int tempTaxPerLot = (FMatchPrice * ContractMultiplier * TaxRate) + 0.5;
	FTaxPerLot = tempTaxPerLot;
	FTax = FTaxPerLot * FMatchQuantity;
	Unlock(RWLockType);
}  //CMatchSummary::CalculateTax()
//---------------------------------------------------------------------------
void CMatchSummary::CalculateFee(double ContractMultiplier, double UnitFee, double PriceFeeRate, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FVolumeFeePerLot = UnitFee;
	FVolumeFee = FVolumeFeePerLot * FMatchQuantity;
	FPriceFeePerLot = FMatchPrice * ContractMultiplier * PriceFeeRate;
	FPriceFee = FPriceFeePerLot * FMatchQuantity;
	FFeePerLot = FVolumeFeePerLot + FPriceFeePerLot;
	FFee = FFeePerLot * FMatchQuantity;
	Unlock(RWLockType);
}  //CMatchSummary::CalculateFee()
//---------------------------------------------------------------------------
void CMatchSummary::CalculateTaxAndFee(double ContractMultiplier, double TaxRate, double UnitFee, double PriceFeeRate, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	CalculateTax(ContractMultiplier, TaxRate, rwNotLock);
	CalculateFee(ContractMultiplier, UnitFee, PriceFeeRate, rwNotLock);
	Unlock(RWLockType);
}  //CMatchSummary::CalculateTaxAndFee()
//---------------------------------------------------------------------------
void CMatchSummary::CalculateTradeData(double ContractMultiplier, double TaxRate, double UnitFee, double PriceFeeRate, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FMatchAmountPerLot = FMatchPrice * ContractMultiplier;
	FMatchAmount = FMatchAmountPerLot * FMatchQuantity;
	int tempTaxPerLot = (FMatchAmountPerLot * TaxRate) + 0.5;
	FTaxPerLot = tempTaxPerLot;
	FTax = FTaxPerLot * FMatchQuantity;
	FVolumeFeePerLot = UnitFee;
	FVolumeFee = FVolumeFeePerLot * FMatchQuantity;
	FPriceFeePerLot = FMatchAmountPerLot * PriceFeeRate;
	FPriceFee = FPriceFeePerLot * FMatchQuantity;
	FFeePerLot = FVolumeFeePerLot + FPriceFeePerLot;
	FFee = FFeePerLot * FMatchQuantity;
	Unlock(RWLockType);
}  //CMatchSummary::CalculateTradeData()
//---------------------------------------------------------------------------
void CMatchSummary::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String dumpMsg;
	Lock(RWLockType);
	dumpMsg.printf(L"Order[%s] [%s-%s] %s %s qty=%d px=%15.6lf amount=%15.6lf tax=%15.6lf fee=%15.6lf Exec[%s] ",
				   FOrderId, FExchangeCode, FSymbol, DescribeBuySell(FSide), DescribePositionEffect(FPositionEffect),
				   FMatchQuantity, FMatchPrice, FMatchAmount, FTax, FFee, FExecId);
	Unlock(RWLockType);
	WriteUFCLog(L"MatchSummary:", dumpMsg, DoFlushToFile);
}  //CMatchSummary::DumpData()

//---------------------------- COffsetUnitRecord ----------------------------
//---------------------------------------------------------------------------
COffsetUnitRecord::COffsetUnitRecord(CMatchSummary *OpenMatchSummaryPtr, CMatchSummary *CloseMatchSummaryPtr)
:CTradingBasicObject(constCreateRWLock)
,FCloseProfit(0.0)
,FFloatingProfit(0.0)
,FOpenMatchSummaryPtr(OpenMatchSummaryPtr)
,FCloseMatchSummaryPtr(CloseMatchSummaryPtr)
,FLastPrice(0.0)
{
}  //COffsetUnitRecord::COffsetUnitRecord()
//---------------------------------------------------------------------------
void COffsetUnitRecord::SetCloseMatchSummary(CMatchSummary *CloseMatchSummaryPtr, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FCloseMatchSummaryPtr = CloseMatchSummaryPtr;
	CalculateCloseProfit(rwNotLock);
	Unlock(RWLockType);
}  //COffsetUnitRecord::SetCloseMatchSummary()
//---------------------------------------------------------------------------
double COffsetUnitRecord::GetOpenMatchPrice(ReadWrietLockTypeEnum RWLockType)
{
	double openMatchPrice = 0;
	Lock(RWLockType);
	if (FOpenMatchSummaryPtr != 0) openMatchPrice = FOpenMatchSummaryPtr->GetMatchPrice(rwLockForRead);
	Unlock(RWLockType);
	return openMatchPrice;
}  //COffsetUnitRecord::GetOpenMatchPrice()
//---------------------------------------------------------------------------
double COffsetUnitRecord::GetCloseMatchPrice(ReadWrietLockTypeEnum RWLockType)
{
	double closeMatchPrice = 0;
	Lock(RWLockType);
	if (FCloseMatchSummaryPtr != 0) closeMatchPrice = FCloseMatchSummaryPtr->GetMatchPrice(rwLockForRead);
	Unlock(RWLockType);
	return closeMatchPrice;
}  //COffsetUnitRecord::GetCloseMatchPrice()
//---------------------------------------------------------------------------
void COffsetUnitRecord::GetMatchPrice(double& OpenMatchPrice, double& CloseMatchPrice, ReadWrietLockTypeEnum RWLockType)
{
	OpenMatchPrice = 0;
	CloseMatchPrice = 0;
	Lock(RWLockType);
	if (FOpenMatchSummaryPtr != 0) OpenMatchPrice = FOpenMatchSummaryPtr->GetMatchPrice(rwLockForRead);
	if (FCloseMatchSummaryPtr != 0) CloseMatchPrice = FCloseMatchSummaryPtr->GetMatchPrice(rwLockForRead);
	Unlock(RWLockType);
}  //COffsetUnitRecord::GetMatchPrice()
//---------------------------------------------------------------------------
double COffsetUnitRecord::CalculateCloseProfit(ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	double closeProfit = 0.0;
	if ((FOpenMatchSummaryPtr != 0) && (FCloseMatchSummaryPtr != 0))
	{
		FOpenMatchSummaryPtr->LockForRead();
		nsOrderMessageDefine::SideEnum openSide = FOpenMatchSummaryPtr->GetSide(rwNotLock);
		double openAmount = FOpenMatchSummaryPtr->GetMatchAmountPerLot(rwNotLock);
		FOpenMatchSummaryPtr->UnlockForRead();
		FCloseMatchSummaryPtr->LockForRead();
		nsOrderMessageDefine::SideEnum closeSide = FCloseMatchSummaryPtr->GetSide(rwNotLock);
		double closeAmount = FCloseMatchSummaryPtr->GetMatchAmountPerLot(rwNotLock);
		FCloseMatchSummaryPtr->UnlockForRead();
		if ((openSide == nsOrderMessageDefine::sBuy) && (closeSide == nsOrderMessageDefine::sSell))
			closeProfit = closeAmount - openAmount;
		else if ((openSide == nsOrderMessageDefine::sSell) && (closeSide == nsOrderMessageDefine::sBuy))
			closeProfit = openAmount - closeAmount;
	}
	FCloseProfit = closeProfit;
	Unlock(RWLockType);
	return closeProfit;
}  //COffsetUnitRecord::CalculateCloseProfit()
//---------------------------------------------------------------------------
double COffsetUnitRecord::CalculateFloatingProfit(double ContractMultiplier, double LastPrice, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FLastPrice = LastPrice;
	double floatingProfit = 0.0;
	if (FOpenMatchSummaryPtr != 0)
	{
		FOpenMatchSummaryPtr->LockForRead();
		nsOrderMessageDefine::SideEnum openSide = FOpenMatchSummaryPtr->GetSide(rwNotLock);
		double openMatchPrice = FOpenMatchSummaryPtr->GetMatchPrice(rwNotLock);
		FOpenMatchSummaryPtr->UnlockForRead();
		if (openSide == nsOrderMessageDefine::sBuy)
			floatingProfit = (LastPrice - openMatchPrice) * ContractMultiplier;
		else if (openSide == nsOrderMessageDefine::sSell)
			floatingProfit = (openMatchPrice - LastPrice) * ContractMultiplier;
	}  //if (FOpenMatchSummaryPtr != 0)
	FFloatingProfit = floatingProfit;
	Unlock(RWLockType);
	return floatingProfit;
}  //COffsetUnitRecord::CalculateFloatingProfit()
//---------------------------------------------------------------------------
void COffsetUnitRecord::SetLastPrice(double ContractMultiplier, double LastPrice, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	if (FCloseMatchSummaryPtr == 0)
	{
		CalculateFloatingProfit(ContractMultiplier, LastPrice, rwNotLock);
		FCloseProfit = 0.0;
	}
	else
	{
		FLastPrice = LastPrice;
		FFloatingProfit = 0.0;
	}
	Unlock(RWLockType);
}  //COffsetUnitRecord::SetLastPrice()
//---------------------------------------------------------------------------
void COffsetUnitRecord::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String openMsg = L"";
	String closeMsg = L"";
	Lock(RWLockType);
	if (FOpenMatchSummaryPtr == 0)
	{
		Unlock(RWLockType);
		return;
	}

	FOpenMatchSummaryPtr->LockForRead();
	String exchangeCode = FOpenMatchSummaryPtr->GetExchangeCode(rwNotLock);
	String symbol = FOpenMatchSummaryPtr->GetSymbol(rwNotLock);
	String openOrderId = FOpenMatchSummaryPtr->GetOrderId(rwNotLock);
	String openExecId = FOpenMatchSummaryPtr->GetExecId(rwNotLock);
	nsOrderMessageDefine::SideEnum openSide = FOpenMatchSummaryPtr->GetSide(rwNotLock);
	nsOrderMessageDefine::PositionEffectEnum openPositionEffect = FOpenMatchSummaryPtr->GetPositionEffect(rwNotLock);
	double openMatchPrice = FOpenMatchSummaryPtr->GetMatchPrice(rwNotLock);
	FOpenMatchSummaryPtr->UnlockForRead();
	openMsg.printf(L"[%s %s %s Px=%15.6lf %s]", openOrderId, DescribeBuySell(openSide), DescribePositionEffect(openPositionEffect), openMatchPrice, openExecId);

	if (FCloseMatchSummaryPtr != 0)
	{
		FCloseMatchSummaryPtr->LockForRead();
		String closeOrderId = FCloseMatchSummaryPtr->GetOrderId(rwNotLock);
		String closeExecId = FCloseMatchSummaryPtr->GetExecId(rwNotLock);
		nsOrderMessageDefine::SideEnum closeSide = FCloseMatchSummaryPtr->GetSide(rwNotLock);
		nsOrderMessageDefine::PositionEffectEnum closePositionEffect = FCloseMatchSummaryPtr->GetPositionEffect(rwNotLock);
		double closeMatchPrice = FCloseMatchSummaryPtr->GetMatchPrice(rwNotLock);
		FCloseMatchSummaryPtr->UnlockForRead();
		closeMsg.printf(L"[%s %s %s Px=%15.6lf %s]", closeOrderId, DescribeBuySell(closeSide), DescribePositionEffect(closePositionEffect), closeMatchPrice, closeExecId);
	}
	Unlock(RWLockType);
	String dumpMsg = L"";
	if (closeMsg.Length() <= 0)
		dumpMsg.printf(L"open interest unit[%s-%s] %s lastPx=%15.6lf floatingProfit=%15.6lf", exchangeCode, symbol, openMsg, FLastPrice, FFloatingProfit);
	else
		dumpMsg.printf(L"offset unit[%s-%s] %s %s closeProfit=%15.6lf", exchangeCode, symbol, openMsg, closeMsg, FCloseProfit);
	WriteUFCLog(dumpMsg, DoFlushToFile);
}  //COffsetUnitRecord::DumpData()

//----------------------- CMatchPriceStatisticRecord ------------------------
String CMatchPriceStatisticRecord::GenerateMatchPriceKey(double MatchPrice)
{
	String keyStr;
	keyStr.printf(L"%-15.6lf", MatchPrice);
	return keyStr;
}  //CMatchPriceStatisticRecord::GenerateMatchPriceKey()
//---------------------------------------------------------------------------
CMatchPriceStatisticRecord::CMatchPriceStatisticRecord(nsOrderMessageDefine::SideEnum Side, double MatchPrice, double ContractMultiplier)
:CTradingBasicObject(constCreateRWLock)
,FSide(Side)
,FMatchPrice(MatchPrice)
,FContractMultiplier(ContractMultiplier)
,FOpenInterestQuantity(0)
,FOpenInterestAmount(0.0)
,FDayTradeOpenQuantity(0)
,FDayTradeOpenAmount(0.0)
,FOpenQuantity(0)
,FOpenAmount(0.0)
,FRolledQuantity(0)
,FRolledAmount(0.0)
,FCloseQuantity(0)
,FCloseAmount(0.0)
,FTotalMatchQuantity(0)
{
	FMainKey = GenerateMatchPriceKey(MatchPrice);
	FAmountPerLot = FMatchPrice * FContractMultiplier;
}  //CMatchPriceStatisticRecord::CMatchPriceStatisticRecord()
//---------------------------------------------------------------------------
void CMatchPriceStatisticRecord::SetDayTradeOpenQuantity(int DayTradeOpenQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FDayTradeOpenQuantity = DayTradeOpenQuantity;
	FDayTradeOpenAmount = FAmountPerLot * FDayTradeOpenQuantity;
	Unlock(RWLockType);
}  //CMatchPriceStatisticRecord::SetDayTradeOpenQuantity()
//---------------------------------------------------------------------------
int CMatchPriceStatisticRecord::InceraseDayTradeOpenQuantity(int InceraseQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	int sumQuantity = FDayTradeOpenQuantity + InceraseQuantity;
	if (sumQuantity < 0) sumQuantity = 0;
	FDayTradeOpenQuantity = sumQuantity;
	FDayTradeOpenAmount = FAmountPerLot * static_cast<double>(FDayTradeOpenQuantity);
	Unlock(RWLockType);
	return sumQuantity;
}  //CMatchPriceStatisticRecord::InceraseDayTradeOpenQuantity()
//---------------------------------------------------------------------------
void CMatchPriceStatisticRecord::SetOpenQuantity(int OpenQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FOpenQuantity = OpenQuantity;
	FOpenAmount = FAmountPerLot * FOpenQuantity;
	Unlock(RWLockType);
}  //CMatchPriceStatisticRecord::SetOpenQuantity()
//---------------------------------------------------------------------------
int CMatchPriceStatisticRecord::InceraseOpenQuantity(int InceraseQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	int sumQuantity = FOpenQuantity + InceraseQuantity;
	if (sumQuantity < 0) sumQuantity = 0;
	FOpenQuantity = sumQuantity;
	FOpenAmount = FAmountPerLot * static_cast<double>(FOpenQuantity);
	Unlock(RWLockType);
	return sumQuantity;
}  //CMatchPriceStatisticRecord::InceraseOpenQuantity()
//---------------------------------------------------------------------------
void CMatchPriceStatisticRecord::SetRolledQuantity(int RolledQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FRolledQuantity = RolledQuantity;
	FRolledAmount = FAmountPerLot * FRolledQuantity;
	Unlock(RWLockType);
}  //CMatchPriceStatisticRecord::SetRolledQuantity()
//---------------------------------------------------------------------------
int CMatchPriceStatisticRecord::InceraseRolledQuantity(int InceraseQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	int sumQuantity = FRolledQuantity + InceraseQuantity;
	if (sumQuantity < 0) sumQuantity = 0;
	FRolledQuantity = sumQuantity;
	FRolledAmount = FAmountPerLot * static_cast<double>(FRolledQuantity);
	Unlock(RWLockType);
	return sumQuantity;
}  //CMatchPriceStatisticRecord::InceraseRolledQuantity()
//---------------------------------------------------------------------------
void CMatchPriceStatisticRecord::SetCloseQuantity(int CloseQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FCloseQuantity = CloseQuantity;
	FCloseAmount = FAmountPerLot * FCloseQuantity;
	Unlock(RWLockType);
}  //CMatchPriceStatisticRecord::SetCloseQuantity()
//---------------------------------------------------------------------------
int CMatchPriceStatisticRecord::InceraseCloseQuantity(int InceraseQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	int sumQuantity = FCloseQuantity + InceraseQuantity;
	if (sumQuantity < 0) sumQuantity = 0;
	FCloseQuantity = sumQuantity;
	FCloseAmount = FAmountPerLot * static_cast<double>(FCloseQuantity);
	Unlock(RWLockType);
	return sumQuantity;
}  //CMatchPriceStatisticRecord::InceraseCloseQuantity()
//---------------------------------------------------------------------------
void CMatchPriceStatisticRecord::SetContractMultiplier(double ContractMultiplier, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	if (FContractMultiplier != ContractMultiplier)
	{
		FContractMultiplier = ContractMultiplier;
		FAmountPerLot = FMatchPrice * FContractMultiplier;
		FOpenInterestAmount = FAmountPerLot * static_cast<double>(FOpenInterestQuantity);
		FDayTradeOpenAmount = FAmountPerLot * static_cast<double>(FDayTradeOpenQuantity);
		FOpenAmount         = FAmountPerLot * static_cast<double>(FOpenQuantity);
		FRolledAmount       = FAmountPerLot * static_cast<double>(FRolledQuantity);
		FCloseAmount        = FAmountPerLot * static_cast<double>(FCloseQuantity);
	}
	Unlock(RWLockType);
}  //CMatchPriceStatisticRecord::SetContractMultiplier()
//---------------------------------------------------------------------------
void CMatchPriceStatisticRecord::SetOpenInterestQuantity(int OpenInterestQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FOpenInterestQuantity = OpenInterestQuantity;
	FOpenInterestAmount = FAmountPerLot * FOpenInterestQuantity;
	Unlock(RWLockType);
}  //CMatchPriceStatisticRecord::SetOpenInterestQuantity()
//---------------------------------------------------------------------------
int CMatchPriceStatisticRecord::InceraseOpenInterestQuantity(int InceraseQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	int sumQuantity = FOpenInterestQuantity + InceraseQuantity;
	if (sumQuantity < 0) sumQuantity = 0;
	FOpenInterestQuantity = sumQuantity;
	FOpenInterestAmount = FAmountPerLot * static_cast<double>(FOpenInterestQuantity);
	Unlock(RWLockType);
	return sumQuantity;
}  //CMatchPriceStatisticRecord::InceraseOpenInterestQuantity()
//---------------------------------------------------------------------------
int CMatchPriceStatisticRecord::InceraseTotalMatchQuantity(int InceraseQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	int sumQuantity = FTotalMatchQuantity + InceraseQuantity;
	if (sumQuantity < 0) sumQuantity = 0;
	FTotalMatchQuantity = sumQuantity;
	Unlock(RWLockType);
	return sumQuantity;
}  //CMatchPriceStatisticRecord::InceraseTotalMatchQuantity()
//---------------------------------------------------------------------------
int CMatchPriceStatisticRecord::GetMatchQuantity(nsOrderMessageDefine::PositionEffectEnum PositionEffect, ReadWrietLockTypeEnum RWLockType)
{
	int matchQuantity;
	Lock(RWLockType);
	switch (PositionEffect)
	{
		case nsOrderMessageDefine::peOpen:         matchQuantity = FOpenQuantity;         break;  //0
		case nsOrderMessageDefine::peClose:        matchQuantity = FCloseQuantity;        break;  //1
		case nsOrderMessageDefine::peDayTrade:     matchQuantity = FRolledQuantity;       break;  //2
		case nsOrderMessageDefine::peDayTradeOpen: matchQuantity = FDayTradeOpenQuantity; break;  //3
		default:                                   matchQuantity = 0;
	}
	Unlock(RWLockType);
	return matchQuantity;
}  //CMatchPriceStatisticRecord::GetMatchQuantity()
//---------------------------------------------------------------------------
int CMatchPriceStatisticRecord::InceraseMatchQuantity(nsOrderMessageDefine::PositionEffectEnum PositionEffect, int InceraseQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	int newQuantity = 0;
	switch (PositionEffect)
	{
		case nsOrderMessageDefine::peOpen:  //0
			InceraseOpenInterestQuantity(InceraseQuantity, rwNotLock);
			newQuantity = InceraseOpenQuantity(InceraseQuantity, rwNotLock);
			break;
		case nsOrderMessageDefine::peClose:  //1
			newQuantity = InceraseCloseQuantity(InceraseQuantity, rwNotLock);
			break;
		case nsOrderMessageDefine::peDayTrade:  //2
			newQuantity = InceraseRolledQuantity(InceraseQuantity, rwNotLock);
			break;
		case nsOrderMessageDefine::peDayTradeOpen:  //3
			InceraseOpenInterestQuantity(InceraseQuantity, rwNotLock);
			newQuantity = InceraseDayTradeOpenQuantity(InceraseQuantity, rwNotLock);
			break;
	}  //switch (PositionEffect)
	InceraseTotalMatchQuantity(InceraseQuantity, rwNotLock);
	Unlock(RWLockType);
	return newQuantity;
}  //CMatchPriceStatisticRecord::InceraseMatchQuantity()
//---------------------------------------------------------------------------
void CMatchPriceStatisticRecord::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String dumpMsg = L"";
	Lock(RWLockType);
	dumpMsg.printf(L"%s Px=%15.6lf Qty[OpenInterest=%d DayTradeOpen=%d Open=%d Rolled=%d Close=%d]", DescribeBuySell(FSide), FMatchPrice, FOpenInterestQuantity, FDayTradeOpenQuantity, FOpenQuantity, FRolledQuantity, FCloseQuantity);
	Unlock(RWLockType);
	WriteUFCLog(L"MatchPriceStatistic:", dumpMsg, DoFlushToFile);
}  //CMatchPriceStatisticRecord::DumpData()

//------------------------ CPositionStatisticRecord -------------------------
//---------------------------------------------------------------------------
CPositionStatisticRecord::CPositionStatisticRecord(const String& ExchangeCode, const String& Symbol, const String& TradeSymbol)
:CExchangeSymbolKey(constCreateRWLock, ExchangeCode, Symbol, nsOrderMessageDefine::mOTC, Symbol, TradeSymbol)
,FTaxRate(0.0)
,FOpenFeeRate(0.0)
,FOpenUnitFee(0.0)
,FCloseFeeRate(0.0)
,FCloseUnitFee(0.0)
,FCloseTodayFeeRate(0.0)
,FCloseTodayUnitFee(0.0)
,FContractMultiplier(1.0)
,FBuyOpenInterestQuantity(0)
,FBuyOpenInterestAmount(0.0)
,FBuyOpenInterestTax(0.0)
,FBuyOpenInterestFee(0.0)
,FBuyHoldOpenQuantity(0)
,FBuyHoldOpenAmount(0.0)
,FBuyHoldOpenTax(0.0)
,FBuyHoldOpenFee(0.0)
,FBuyDayTradeOpenQuantity(0)
,FBuyDayTradeOpenAmount(0.0)
,FBuyDayTradeOpenTax(0.0)
,FBuyDayTradeOpenFee(0.0)
,FBuyOpenQuantity(0)
,FBuyOpenAmount(0.0)
,FBuyOpenTax(0.0)
,FBuyOpenFee(0.0)
,FBuyRolledQuantity(0)
,FBuyRolledAmount(0.0)
,FBuyRolledTax(0.0)
,FBuyRolledFee(0.0)
,FBuyCloseQuantity(0)
,FBuyCloseAmount(0.0)
,FBuyCloseTax(0.0)
,FBuyCloseFee(0.0)
,FBuyQuantity(0)
,FBuyAmount(0.0)
,FBuyTax(0.0)
,FBuyFee(0.0)
,FBuyCloseProfit(0.0)
,FBuyFloatingProfit(0.0)
,FSellOpenInterestQuantity(0)
,FSellOpenInterestAmount(0.0)
,FSellOpenInterestTax(0.0)
,FSellOpenInterestFee(0.0)
,FSellHoldOpenQuantity(0)
,FSellHoldOpenAmount(0.0)
,FSellHoldOpenTax(0.0)
,FSellHoldOpenFee(0.0)
,FSellDayTradeOpenQuantity(0)
,FSellDayTradeOpenAmount(0.0)
,FSellDayTradeOpenTax(0.0)
,FSellDayTradeOpenFee(0.0)
,FSellOpenQuantity(0)
,FSellOpenAmount(0.0)
,FSellOpenTax(0.0)
,FSellOpenFee(0.0)
,FSellRolledQuantity(0)
,FSellRolledAmount(0.0)
,FSellRolledTax(0.0)
,FSellRolledFee(0.0)
,FSellCloseQuantity(0)
,FSellCloseAmount(0.0)
,FSellCloseTax(0.0)
,FSellCloseFee(0.0)
,FSellQuantity(0)
,FSellAmount(0.0)
,FSellTax(0.0)
,FSellFee(0.0)
,FSellCloseProfit(0.0)
,FSellFloatingProfit(0.0)
,FTax(0.0)
,FFee(0.0)
,FCloseProfit(0.0)
,FOpenInterestTax(0.0)
,FOpenInterestFee(0.0)
,FFloatingProfit(0.0)
,FLastPrice(0.0)
,FBestBuyQuantity1(0)
,FBestBuyPrice1(0.0)
,FBestSellQuantity1(0)
,FBestSellPrice1(0.0)
,FSubscribeCount(0)
,FIsMarketDataReceived(false)
,FLastTickCount(0)
,FIsWaitingForProcess(false)
,FMarketTradeQuantitySum(0)
,FContractPtr(0)
,FCommodityPtr(0)
,FTaxRateObjPtr(0)
,FFeeObjPtr(0)
{
	if( ExchangeCode == L"TWSE" || ExchangeCode == L"OTC" )
	{
		FTaxRate = 0.0015;
		FContractMultiplier = 1000;
    }
}  //CPositionStatisticRecord::CPositionStatisticRecord()
//---------------------------------------------------------------------------
CPositionStatisticRecord::~CPositionStatisticRecord()
{
	LockForWrite();
	ClearOIQueue(FBuyDayTradeOIQueue, constDelete, rwNotLock);
	ClearOIQueue(FBuyHoldOIQueue, constDelete, rwNotLock);
	ClearOIQueue(FBuyOIQueue, constDelete, rwNotLock);
	ClearOIQueue(FSellDayTradeOIQueue, constDelete, rwNotLock);
	ClearOIQueue(FSellHoldOIQueue, constDelete, rwNotLock);
	ClearOIQueue(FSellOIQueue, constDelete, rwNotLock);
	ClearOffsetUnitRecArray(FBuyCloseTodayOffsetArray, constDelete, rwNotLock);
	ClearOffsetUnitRecArray(FBuyCloseOffsetArray, constDelete, rwNotLock);
	ClearOffsetUnitRecArray(FSellCloseTodayOffsetArray, constDelete, rwNotLock);
	ClearOffsetUnitRecArray(FSellCloseOffsetArray, constDelete, rwNotLock);

	ClearBuyPxQtyStatisticArray(constNotDelete, rwNotLock);
	ClearBuyPxQtyStatisticPriceIndex(constDelete, rwNotLock);
	ClearSellPxQtyStatisticArray(constNotDelete, rwNotLock);
	ClearSellPxQtyStatisticPriceIndex(constDelete, rwNotLock);

	ClearHoldMatchSummaryArray(constDelete, rwNotLock);
	ClearMatchSummaryArray(constNotDelete, rwNotLock);
	ClearMatchSummaryExecIdIndex(constDelete, rwNotLock);
	UnlockForWrite();
}  //CPositionStatisticRecord::~CPositionStatisticRecord()
//---------------------------------------------------------------------------
void CPositionStatisticRecord::SetLastPrice(double LastPrice, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FLastPrice = LastPrice;
	CalculateFloatingData(rwNotLock);
	Unlock(RWLockType);
}  //CPositionStatisticRecord::SetLastPrice()
//---------------------------------------------------------------------------
void CPositionStatisticRecord::SetTaxRateObject(CTaxRateObject *TaxRateObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FTaxRateObjPtr = TaxRateObjPtr;
	if (FTaxRateObjPtr != 0) FTaxRate = FTaxRateObjPtr->GetTaxRate(rwLockForRead);
	Unlock(RWLockType);
}  //CPositionStatisticRecord::SetTaxRateObject()
//---------------------------------------------------------------------------
void CPositionStatisticRecord::SetFeeObject(CFeeObject *FeeObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FFeeObjPtr = FeeObjPtr;
	if (FFeeObjPtr != 0)
	{
		FFeeObjPtr->LockForRead();
		FOpenFeeRate       = FFeeObjPtr->GetOpenRateByMoney(rwNotLock);
		FOpenUnitFee       = FFeeObjPtr->GetOpenRateByVolume(rwNotLock);
		FCloseFeeRate      = FFeeObjPtr->GetCloseRateByMoney(rwNotLock);
		FCloseUnitFee      = FFeeObjPtr->GetCloseRateByVolume(rwNotLock);
		FCloseTodayFeeRate = FFeeObjPtr->GetCloseTodayRateByMoney(rwNotLock);
		FCloseTodayUnitFee = FFeeObjPtr->GetCloseTodayRateByVolume(rwNotLock);
		FFeeObjPtr->UnlockForRead();
	}
	Unlock(RWLockType);
}  //CPositionStatisticRecord::SetFeeObject()
//---------------------------------------------------------------------------
double CPositionStatisticRecord::GetPriceFeeRate(nsOrderMessageDefine::PositionEffectEnum PositionEffect, ReadWrietLockTypeEnum RWLockType)
{
	double feeRate;
	Lock(RWLockType);
	switch (PositionEffect)
	{
		case nsOrderMessageDefine::peOpen:         feeRate = FOpenFeeRate;       break;  //0
		case nsOrderMessageDefine::peClose:        feeRate = FCloseFeeRate;      break;  //1
		case nsOrderMessageDefine::peDayTrade:     feeRate = FCloseTodayFeeRate; break;  //2
		case nsOrderMessageDefine::peDayTradeOpen: feeRate = FOpenFeeRate;       break;  //3
		default:                                   feeRate = 0.0;
	}
	Unlock(RWLockType);
	return feeRate;
}  //CPositionStatisticRecord::GetPriceFeeRate()
//---------------------------------------------------------------------------
double CPositionStatisticRecord::GetUnitFee(nsOrderMessageDefine::PositionEffectEnum PositionEffect, ReadWrietLockTypeEnum RWLockType)
{
	double unitFee;
	Lock(RWLockType);
	switch (PositionEffect)
	{
		case nsOrderMessageDefine::peOpen:         unitFee = FOpenUnitFee;       break;  //0
		case nsOrderMessageDefine::peClose:        unitFee = FCloseUnitFee;      break;  //1
		case nsOrderMessageDefine::peDayTrade:     unitFee = FCloseTodayUnitFee; break;  //2
		case nsOrderMessageDefine::peDayTradeOpen: unitFee = FOpenUnitFee;       break;  //3
		default:                                   unitFee = 0.0;
	}
	Unlock(RWLockType);
	return unitFee;
}  //CPositionStatisticRecord::GetUnitFee()
//---------------------------------------------------------------------------
void CPositionStatisticRecord::GetFee(nsOrderMessageDefine::PositionEffectEnum PositionEffect,
									  double& FeeRate, double& UnitFee, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	switch (PositionEffect)
	{
		case nsOrderMessageDefine::peOpen:  //0
			FeeRate = FOpenFeeRate;
			UnitFee = FOpenUnitFee;
			break;
		case nsOrderMessageDefine::peClose:  //1
			FeeRate = FCloseFeeRate;
			UnitFee = FCloseUnitFee;
			break;
		case nsOrderMessageDefine::peDayTrade:  //2
			FeeRate = FCloseTodayFeeRate;
			UnitFee = FCloseTodayUnitFee;
			break;
		case nsOrderMessageDefine::peDayTradeOpen:  //3
			FeeRate = FOpenFeeRate;
			UnitFee = FOpenUnitFee;
			break;
		default:
			FeeRate = 0.0;
			UnitFee = 0.0;
	}
	Unlock(RWLockType);
}  //CPositionStatisticRecord::GetFee()
//---------------------------------------------------------------------------
CMatchSummary *CPositionStatisticRecord::GetHoldMatchSummaryFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CMatchSummary*>(GetTradingObjFromVector(FHoldMatchSummaryArray, ArrayIndex, FCSLockPtr, RWLockType));
}  //CPositionStatisticRecord::GetHoldMatchSummaryFromArray()
//---------------------------------------------------------------------------
bool CPositionStatisticRecord::AppendHoldMatchSummaryToArray(CMatchSummary *MatchSummaryPtr, ReadWrietLockTypeEnum RWLockType)
{
	return AppendTradingObjToVector(FHoldMatchSummaryArray, MatchSummaryPtr, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord::AppendHoldMatchSummaryToArray()
//---------------------------------------------------------------------------
int CPositionStatisticRecord::ClearHoldMatchSummaryArray(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjVector(FHoldMatchSummaryArray, DoDelete, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord::ClearHoldMatchSummaryArray()
//---------------------------------------------------------------------------
CMatchSummary *CPositionStatisticRecord::FindMatchSummaryFromExecIdIndex(const String& ExecId, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CMatchSummary*>(FindTradingObjFromMap(FMatchSummaryExecIdIndex, ExecId, FCSLockPtr, RWLockType));
}  //CPositionStatisticRecord::FindMatchSummaryFromExecIdIndex()
//---------------------------------------------------------------------------
bool CPositionStatisticRecord::InsertMatchSummaryIntoExecIdIndex(const String& ExecId, CMatchSummary *MatchSummaryPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	return InsertTradingObjIntoMap(FMatchSummaryExecIdIndex, ExecId, MatchSummaryPtr, DoCheckExist, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord::InsertMatchSummaryIntoExecIdIndex()
//---------------------------------------------------------------------------
CMatchSummary *CPositionStatisticRecord::RemoveMatchSummaryFromExecIdIndex(const String& ExecId, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CMatchSummary*>(RemoveTradingObjFromMap(FMatchSummaryExecIdIndex, ExecId, FCSLockPtr, RWLockType));
}  //CPositionStatisticRecord::RemoveMatchSummaryFromExecIdIndex()
//---------------------------------------------------------------------------
int CPositionStatisticRecord::ClearMatchSummaryExecIdIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FMatchSummaryExecIdIndex, DoDelete, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord::ClearMatchSummaryExecIdIndex()
//---------------------------------------------------------------------------
CMatchSummary *CPositionStatisticRecord::GetMatchSummaryFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CMatchSummary*>(GetTradingObjFromVector(FMatchSummaryArray, ArrayIndex, FCSLockPtr, RWLockType));
}  //CPositionStatisticRecord::GetHoldMatchSummaryFromArray()
//---------------------------------------------------------------------------
bool CPositionStatisticRecord::AppendMatchSummaryToArray(CMatchSummary *MatchSummaryPtr, ReadWrietLockTypeEnum RWLockType)
{
	return AppendTradingObjToVector(FMatchSummaryArray, MatchSummaryPtr, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord::AppendMatchSummaryToArray()
//---------------------------------------------------------------------------
int CPositionStatisticRecord::ClearMatchSummaryArray(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjVector(FMatchSummaryArray, DoDelete, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord::ClearMatchSummaryArray()
//---------------------------------------------------------------------------
CMatchPriceStatisticRecord *CPositionStatisticRecord::FindBuyPxQtyStatisticFromPriceIndex(double MatchPrice, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CMatchPriceStatisticRecord::GenerateMatchPriceKey(MatchPrice);
	return static_cast<CMatchPriceStatisticRecord*>(FindTradingObjFromMap(FBuyPxQtyStatisticPriceIndex, keyValue, FCSLockPtr, RWLockType));
}  //CPositionStatisticRecord:FindBuyPxQtyStatisticFromPriceIndex()
//---------------------------------------------------------------------------
bool CPositionStatisticRecord::InsertBuyPxQtyStatisticIntoPriceIndex(double MatchPrice, CMatchPriceStatisticRecord *PxQtyStatisticPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CMatchPriceStatisticRecord::GenerateMatchPriceKey(MatchPrice);
	return InsertTradingObjIntoMap(FBuyPxQtyStatisticPriceIndex, keyValue, PxQtyStatisticPtr, DoCheckExist, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord:InsertBuyPxQtyStatisticIntoPriceIndex()
//---------------------------------------------------------------------------
CMatchPriceStatisticRecord *CPositionStatisticRecord::RemoveBuyPxQtyStatisticFromPriceIndex(double MatchPrice, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CMatchPriceStatisticRecord::GenerateMatchPriceKey(MatchPrice);
	return static_cast<CMatchPriceStatisticRecord*>(RemoveTradingObjFromMap(FBuyPxQtyStatisticPriceIndex, keyValue, FCSLockPtr, RWLockType));
}  //CPositionStatisticRecord:RemoveBuyPxQtyStatisticFromPriceIndex()
//---------------------------------------------------------------------------
int CPositionStatisticRecord::ClearBuyPxQtyStatisticPriceIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FBuyPxQtyStatisticPriceIndex, DoDelete, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord:ClearBuyPxQtyStatisticPriceIndex()
//---------------------------------------------------------------------------
CMatchPriceStatisticRecord *CPositionStatisticRecord::GetBuyPxQtyStatisticFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CMatchPriceStatisticRecord*>(GetTradingObjFromVector(FBuyPxQtyStatisticArray, ArrayIndex, FCSLockPtr, RWLockType));
}  //CPositionStatisticRecord:GetBuyPxQtyStatisticFromArray()
//---------------------------------------------------------------------------
bool CPositionStatisticRecord::AppendBuyPxQtyStatisticToArray(CMatchPriceStatisticRecord *PxQtyStatisticPtr, ReadWrietLockTypeEnum RWLockType)
{
	return AppendTradingObjToVector(FBuyPxQtyStatisticArray, PxQtyStatisticPtr, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord:AppendBuyPxQtyStatisticToArray()
//---------------------------------------------------------------------------
int CPositionStatisticRecord::ClearBuyPxQtyStatisticArray(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjVector(FBuyPxQtyStatisticArray, DoDelete, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord:ClearBuyPxQtyStatisticArray()
//---------------------------------------------------------------------------
CMatchPriceStatisticRecord *CPositionStatisticRecord::FindSellPxQtyStatisticFromPriceIndex(double MatchPrice, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CMatchPriceStatisticRecord::GenerateMatchPriceKey(MatchPrice);
	return static_cast<CMatchPriceStatisticRecord*>(FindTradingObjFromMap(FSellPxQtyStatisticPriceIndex, keyValue, FCSLockPtr, RWLockType));
}  //CPositionStatisticRecord:FindSellPxQtyStatisticFromPriceIndex()
//---------------------------------------------------------------------------
bool CPositionStatisticRecord::InsertSellPxQtyStatisticIntoPriceIndex(double MatchPrice, CMatchPriceStatisticRecord *PxQtyStatisticPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CMatchPriceStatisticRecord::GenerateMatchPriceKey(MatchPrice);
	return InsertTradingObjIntoMap(FSellPxQtyStatisticPriceIndex, keyValue, PxQtyStatisticPtr, DoCheckExist, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord:InsertSellPxQtyStatisticIntoPriceIndex()
//---------------------------------------------------------------------------
CMatchPriceStatisticRecord *CPositionStatisticRecord::RemoveSellPxQtyStatisticFromPriceIndex(double MatchPrice, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CMatchPriceStatisticRecord::GenerateMatchPriceKey(MatchPrice);
	return static_cast<CMatchPriceStatisticRecord*>(RemoveTradingObjFromMap(FSellPxQtyStatisticPriceIndex, keyValue, FCSLockPtr, RWLockType));
}  //CPositionStatisticRecord:RemoveSellPxQtyStatisticFromPriceIndex()
//---------------------------------------------------------------------------
int CPositionStatisticRecord::ClearSellPxQtyStatisticPriceIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FSellPxQtyStatisticPriceIndex, DoDelete, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord:ClearSellPxQtyStatisticPriceIndex()
//---------------------------------------------------------------------------
CMatchPriceStatisticRecord *CPositionStatisticRecord::GetSellPxQtyStatisticFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CMatchPriceStatisticRecord*>(GetTradingObjFromVector(FSellPxQtyStatisticArray, ArrayIndex, FCSLockPtr, RWLockType));
}  //CPositionStatisticRecord:GetSellPxQtyStatisticFromArray()
//---------------------------------------------------------------------------
bool CPositionStatisticRecord::AppendSellPxQtyStatisticToArray(CMatchPriceStatisticRecord *PxQtyStatisticPtr, ReadWrietLockTypeEnum RWLockType)
{
	return AppendTradingObjToVector(FSellPxQtyStatisticArray, PxQtyStatisticPtr, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord:AppendSellPxQtyStatisticToArray()
//---------------------------------------------------------------------------
int CPositionStatisticRecord::ClearSellPxQtyStatisticArray(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjVector(FSellPxQtyStatisticArray, DoDelete, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord:ClearSellPxQtyStatisticArray()
//---------------------------------------------------------------------------
CMatchPriceStatisticRecord *CPositionStatisticRecord::FindOrCreatPxQtyStatistic(nsOrderMessageDefine::SideEnum Side,
																				double MatchPrice, ReadWrietLockTypeEnum RWLockType)
{
	CMatchPriceStatisticRecord *pxQtyStatisticPtr = 0;
	Lock(RWLockType);
	if (Side == nsOrderMessageDefine::sBuy)
		pxQtyStatisticPtr = FindBuyPxQtyStatisticFromPriceIndex(MatchPrice, rwNotLock);
	else if (Side == nsOrderMessageDefine::sSell)
		pxQtyStatisticPtr = FindSellPxQtyStatisticFromPriceIndex(MatchPrice, rwNotLock);

	if (pxQtyStatisticPtr == 0)
	{
		bool isInsertSuccess = false;
		pxQtyStatisticPtr = new CMatchPriceStatisticRecord(Side, MatchPrice, FContractMultiplier);
		if (Side == nsOrderMessageDefine::sBuy)
		{
			isInsertSuccess = InsertBuyPxQtyStatisticIntoPriceIndex(MatchPrice, pxQtyStatisticPtr, constNotCheckExist, rwNotLock);
			if (isInsertSuccess) AppendBuyPxQtyStatisticToArray(pxQtyStatisticPtr, rwNotLock);
		}
		else if (Side == nsOrderMessageDefine::sSell)
		{
			isInsertSuccess = InsertSellPxQtyStatisticIntoPriceIndex(MatchPrice, pxQtyStatisticPtr, constNotCheckExist, rwNotLock);
			if (isInsertSuccess) AppendSellPxQtyStatisticToArray(pxQtyStatisticPtr, rwNotLock);
		}
		if (!isInsertSuccess)
		{
			delete pxQtyStatisticPtr;
			pxQtyStatisticPtr = 0;
			String logMsg;
			logMsg.printf(L"CPositionStatisticRecord::FindOrCreatPxQtyStatistic() Insert %s Price[%15.6lf] into Price Index Failed.", DescribeBuySell(Side), MatchPrice);
			WriteUFCLog(logMsg, constFlush);
		}
	}
	else
	{
		pxQtyStatisticPtr->LockForWrite();
		double origContractMultiplier = pxQtyStatisticPtr->GetContractMultiplier(rwNotLock);
		if (FContractMultiplier != origContractMultiplier) pxQtyStatisticPtr->SetContractMultiplier(FContractMultiplier, rwNotLock);
		pxQtyStatisticPtr->UnlockForWrite();
	}  //if (pxQtyStatisticPtr == 0)
	Unlock(RWLockType);
	return pxQtyStatisticPtr;
}  //CPositionStatisticRecord::FindOrCreatPxQtyStatistic()
//---------------------------------------------------------------------------
COffsetUnitRecord *CPositionStatisticRecord::GetOffsetUnitRecFromOIQueue(CTradingObjQueue& OIQueue, int QueueIndex, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<COffsetUnitRecord*>(GetTradingObjFromQueue(OIQueue, QueueIndex, FCSLockPtr, RWLockType));
}  //CPositionStatisticRecord::GetOffsetUnitRecFromOIQueue()
//---------------------------------------------------------------------------
void CPositionStatisticRecord::PushOffsetUnitRecToOIQueue(CTradingObjQueue& OIQueue, COffsetUnitRecord *OffsetUnitRecPtr, ReadWrietLockTypeEnum RWLockType)
{
	PushTradingObjToQueueBack(OIQueue, OffsetUnitRecPtr, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord::PushOffsetUnitRecToOIQueue()
//---------------------------------------------------------------------------
COffsetUnitRecord *CPositionStatisticRecord::GetFirstOffsetUnitRecOfOIQueue(CTradingObjQueue& OIQueue, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<COffsetUnitRecord*>(GetFirstTradingObjOfQueue(OIQueue, FCSLockPtr, RWLockType));
}  //CPositionStatisticRecord::GetFirstOffsetUnitRecOfOIQueue()
//---------------------------------------------------------------------------
void CPositionStatisticRecord::PopOffsetUnitRecFromOIQueue(CTradingObjQueue& OIQueue, ReadWrietLockTypeEnum RWLockType)
{
	PopFirstTradingObjFromQueue(OIQueue, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord::PopOffsetUnitRecFromOIQueue()
//---------------------------------------------------------------------------
int CPositionStatisticRecord::ClearOIQueue(CTradingObjQueue& OIQueue, bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjQueue(OIQueue, DoDelete, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord::ClearOIQueue()
//---------------------------------------------------------------------------
COffsetUnitRecord *CPositionStatisticRecord::GetOffsetUnitRecFromArray(int ArrayIndex, CTradingObjPtrVector& OffsetArray, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<COffsetUnitRecord*>(GetTradingObjFromVector(OffsetArray, ArrayIndex, FCSLockPtr, RWLockType));
}  //CPositionStatisticRecord::GetOffsetUnitRecFromArray()
//---------------------------------------------------------------------------
int CPositionStatisticRecord::AppendOffsetUnitRecToArray(CTradingObjPtrVector& OffsetArray, COffsetUnitRecord *OffsetUnitRecPtr, ReadWrietLockTypeEnum RWLockType)
{
	return AppendTradingObjToVector(OffsetArray, OffsetUnitRecPtr, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord::AppendOffsetUnitRecToArray()
//---------------------------------------------------------------------------
int CPositionStatisticRecord::ClearOffsetUnitRecArray(CTradingObjPtrVector& OffsetArray, bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjVector(OffsetArray, DoDelete, FCSLockPtr, RWLockType);
}  //CPositionStatisticRecord::ClearOffsetUnitRecArray()
//---------------------------------------------------------------------------
int CPositionStatisticRecord::OffsetOpenInterestQueue(nsOrderMessageDefine::SideEnum CloseSide,
													  nsOrderMessageDefine::PositionEffectEnum ClosePositionEffect,
													  int CloseMatchQuantity, CMatchSummary *CloseMatchSummaryPtr, CTradingObjQueue& OpenOIQueue)
{
	int offsetQuantity = 0;
	for (int i = 0; i < CloseMatchQuantity; i++)
	{
		if (OpenOIQueue.empty()) break;
		COffsetUnitRecord *offsetUnitRecPtr = GetFirstOffsetUnitRecOfOIQueue(OpenOIQueue, rwNotLock);
		PopOffsetUnitRecFromOIQueue(OpenOIQueue, rwNotLock);
		offsetUnitRecPtr->LockForWrite();
		offsetUnitRecPtr->SetCloseMatchSummary(CloseMatchSummaryPtr, rwNotLock);
		CMatchSummary *openMatchSummaryPtr = offsetUnitRecPtr->GetOpenMatchSummary(rwNotLock);
		double closeProfit = offsetUnitRecPtr->GetCloseProfit(rwNotLock);
		offsetUnitRecPtr->UnlockForWrite();

		openMatchSummaryPtr->LockForRead();
		nsOrderMessageDefine::SideEnum openSide = openMatchSummaryPtr->GetSide(rwNotLock);
		double openMatchPrice = openMatchSummaryPtr->GetMatchPrice(rwNotLock);
		double openMatchAmount = openMatchSummaryPtr->GetMatchAmountPerLot(rwNotLock);
		openMatchSummaryPtr->UnlockForRead();
		CMatchPriceStatisticRecord *pxQtyStatisticPtr = FindOrCreatPxQtyStatistic(openSide, openMatchPrice, rwNotLock);
		if (pxQtyStatisticPtr != 0) pxQtyStatisticPtr->InceraseOpenInterestQuantity(-1, rwLockForWrite);

		if (openSide == nsOrderMessageDefine::sBuy)
		{
			IncreaseBuyOpenInterestQuantity(-1, rwNotLock);
			IncreaseBuyOpenInterestAmount(-openMatchAmount, rwNotLock);
			IncreaseBuyCloseProfit(closeProfit, rwNotLock);
		}
		else if (openSide == nsOrderMessageDefine::sSell)
		{
			IncreaseSellOpenInterestQuantity(-1, rwNotLock);
			IncreaseSellOpenInterestAmount(-openMatchAmount, rwNotLock);
			IncreaseSellCloseProfit(closeProfit, rwNotLock);
		}
		IncreaseCloseProfit(closeProfit, rwNotLock);

		if ((CloseSide == nsOrderMessageDefine::sBuy) && (ClosePositionEffect == nsOrderMessageDefine::peClose))
			AppendOffsetUnitRecToArray(FBuyCloseOffsetArray, offsetUnitRecPtr, rwNotLock);
		else if ((CloseSide == nsOrderMessageDefine::sBuy) && (ClosePositionEffect == nsOrderMessageDefine::peDayTrade))
			AppendOffsetUnitRecToArray(FBuyCloseTodayOffsetArray, offsetUnitRecPtr, rwNotLock);
		else if ((CloseSide == nsOrderMessageDefine::sSell) && (ClosePositionEffect == nsOrderMessageDefine::peClose))
			AppendOffsetUnitRecToArray(FSellCloseOffsetArray, offsetUnitRecPtr, rwNotLock);
		else if ((CloseSide == nsOrderMessageDefine::sSell) && (ClosePositionEffect == nsOrderMessageDefine::peDayTrade))
			AppendOffsetUnitRecToArray(FSellCloseTodayOffsetArray, offsetUnitRecPtr, rwNotLock);
		offsetQuantity++;
	}  //for (int i = 0; i < CloseMatchQuantity; i++)
	return offsetQuantity;
}  //CPositionStatisticRecord::OffsetOpenInterestQueue()
//---------------------------------------------------------------------------
int CPositionStatisticRecord::OffsetOpenInterest(nsOrderMessageDefine::SideEnum CloseSide,
												 nsOrderMessageDefine::PositionEffectEnum ClosePositionEffect,
												 int CloseMatchQuantity, double CloseMatchAmount,
												 double CloseTax, double CloseFee,
												 CMatchSummary *CloseMatchSummaryPtr)

{
	int restMatchQuantity = CloseMatchQuantity;  //剩餘未平倉量
	int closeDayTradeOpenQuantity = 0;
	int closeHoldQuantity = 0;
	int closeOpenQuantity = 0;
	if (CloseSide == nsOrderMessageDefine::sBuy)
	{
		if (ClosePositionEffect == nsOrderMessageDefine::peClose)
		{
			IncreaseBuyCloseQuantity(CloseMatchQuantity, rwNotLock);
			IncreaseBuyCloseAmount(CloseMatchAmount, rwNotLock);
			IncreaseBuyCloseTax(CloseTax, rwNotLock);
			IncreaseBuyCloseFee(CloseFee, rwNotLock);
		}
		else if (ClosePositionEffect == nsOrderMessageDefine::peDayTrade)
		{
			IncreaseBuyRolledQuantity(CloseMatchQuantity, rwNotLock);
			IncreaseBuyRolledAmount(CloseMatchAmount, rwNotLock);
			IncreaseBuyRolledTax(CloseTax, rwNotLock);
			IncreaseBuyRolledFee(CloseFee, rwNotLock);
		}

		//平當日沖銷
		closeDayTradeOpenQuantity = OffsetOpenInterestQueue(CloseSide, ClosePositionEffect, restMatchQuantity, CloseMatchSummaryPtr, FSellDayTradeOIQueue);
		restMatchQuantity -= closeDayTradeOpenQuantity;
		if (restMatchQuantity > 0)
		{
			if (ClosePositionEffect == nsOrderMessageDefine::peClose)
			{   //平昨日留倉
				closeHoldQuantity = OffsetOpenInterestQueue(CloseSide, ClosePositionEffect, restMatchQuantity, CloseMatchSummaryPtr, FSellHoldOIQueue);
				restMatchQuantity -= closeHoldQuantity;
			}

			if (restMatchQuantity > 0)
			{   //平其他交易
				closeOpenQuantity = OffsetOpenInterestQueue(CloseSide, ClosePositionEffect, restMatchQuantity, CloseMatchSummaryPtr, FSellOIQueue);
				restMatchQuantity -= closeOpenQuantity;
			}
		}  //if (restMatchQuantity > 0)
	}
	else if (CloseSide == nsOrderMessageDefine::sSell)
	{
		if (ClosePositionEffect == nsOrderMessageDefine::peClose)
		{
			IncreaseSellCloseQuantity(CloseMatchQuantity, rwNotLock);
			IncreaseSellCloseAmount(CloseMatchAmount, rwNotLock);
			IncreaseSellCloseTax(CloseTax, rwNotLock);
			IncreaseSellCloseFee(CloseFee, rwNotLock);
		}
		else if (ClosePositionEffect == nsOrderMessageDefine::peDayTrade)
		{
			IncreaseSellRolledQuantity(CloseMatchQuantity, rwNotLock);
			IncreaseSellRolledAmount(CloseMatchAmount, rwNotLock);
			IncreaseSellRolledTax(CloseTax, rwNotLock);
			IncreaseSellRolledFee(CloseFee, rwNotLock);
		}

		//平當日沖銷
		closeDayTradeOpenQuantity = OffsetOpenInterestQueue(CloseSide, ClosePositionEffect, restMatchQuantity, CloseMatchSummaryPtr, FBuyDayTradeOIQueue);
		restMatchQuantity -= closeDayTradeOpenQuantity;
		if (restMatchQuantity > 0)
		{
			if (ClosePositionEffect == nsOrderMessageDefine::peClose)
			{   //平昨日留倉
				closeHoldQuantity = OffsetOpenInterestQueue(CloseSide, ClosePositionEffect, restMatchQuantity, CloseMatchSummaryPtr, FBuyHoldOIQueue);
				restMatchQuantity -= closeHoldQuantity;
			}

			if (restMatchQuantity > 0)
			{   //平倉其他交易
				closeOpenQuantity = OffsetOpenInterestQueue(CloseSide, ClosePositionEffect, restMatchQuantity, CloseMatchSummaryPtr, FBuyOIQueue);
				restMatchQuantity -= closeOpenQuantity;
			}
		}
	}  //if (Side == nsOrderMessageDefine::sBuy)
	int offsetQuantity = closeDayTradeOpenQuantity + closeHoldQuantity + closeOpenQuantity;
	return offsetQuantity;
}  //CPositionStatisticRecord::OffsetOpenInterest()
//---------------------------------------------------------------------------
void CPositionStatisticRecord::AddOpenInterest(int MatchQuantity, CMatchSummary *MatchSummaryPtr, CTradingObjQueue& OIQueue)
{
	for (int i = 0; i < MatchQuantity; i++)
	{
		COffsetUnitRecord *offsetUnitRecPtr = new COffsetUnitRecord(MatchSummaryPtr, 0);
		offsetUnitRecPtr->SetLastPrice(FContractMultiplier, FLastPrice, rwNotLock);
		PushOffsetUnitRecToOIQueue(OIQueue, offsetUnitRecPtr, rwNotLock);
	}  //for (int i = 0; i < MatchQuantity; i++)
}  //CPositionStatisticRecord::AddOpenInterest()
//---------------------------------------------------------------------------
bool CPositionStatisticRecord::ProcessMatchSummary(CMatchSummary *MatchSummaryPtr)
{
	bool isSuccess = true;
	String logMsg = L"";
	double unitFee = 0.0;
	double priceFeeRate = 0.0;
	MatchSummaryPtr->LockForWrite();
	String orderId = MatchSummaryPtr->GetOrderId(rwNotLock);
	String execId = MatchSummaryPtr->GetExecId(rwNotLock);
	nsOrderMessageDefine::SideEnum side = MatchSummaryPtr->GetSide(rwNotLock);
	nsOrderMessageDefine::PositionEffectEnum positionEffect = MatchSummaryPtr->GetPositionEffect(rwNotLock);
	double matchPrice = MatchSummaryPtr->GetMatchPrice(rwNotLock);
	int matchQuantity = MatchSummaryPtr->GetMatchQuantity(rwNotLock);
	GetFee(positionEffect, priceFeeRate, unitFee, rwNotLock);  //取得稅率資料
	MatchSummaryPtr->CalculateTradeData(FContractMultiplier, FTaxRate, unitFee, priceFeeRate, rwNotLock);
	double matchAmount = MatchSummaryPtr->GetMatchAmount(rwNotLock);
	double matchTax = MatchSummaryPtr->GetTax(rwNotLock);
	double matchFee = MatchSummaryPtr->GetFee(rwNotLock);
	MatchSummaryPtr->UnlockForWrite();

	if ((side == nsOrderMessageDefine::sBuy) || (side == nsOrderMessageDefine::sSell))
	{   //統計成交價資料
		CMatchPriceStatisticRecord *pxQtyStatisticPtr = FindOrCreatPxQtyStatistic(side, matchPrice, rwNotLock);
		if (pxQtyStatisticPtr != 0) pxQtyStatisticPtr->InceraseMatchQuantity(positionEffect, matchQuantity, rwLockForWrite);

		if (positionEffect == nsOrderMessageDefine::peDayTradeOpen)
		{
			if (side == nsOrderMessageDefine::sBuy)
			{
				IncreaseBuyOpenInterestQuantity(matchQuantity, rwNotLock);
				IncreaseBuyOpenInterestAmount(matchAmount, rwNotLock);
				IncreaseBuyDayTradeOpenQuantity(matchQuantity, rwNotLock);
				IncreaseBuyDayTradeOpenAmount(matchAmount, rwNotLock);
				IncreaseBuyDayTradeOpenTax(matchTax, rwNotLock);
				IncreaseBuyDayTradeOpenFee(matchFee, rwNotLock);
				AddOpenInterest(matchQuantity, MatchSummaryPtr, FBuyDayTradeOIQueue);
			}
			else if (side == nsOrderMessageDefine::sSell)
			{
				IncreaseSellOpenInterestQuantity(matchQuantity, rwNotLock);
				IncreaseSellOpenInterestAmount(matchAmount, rwNotLock);
				IncreaseSellDayTradeOpenQuantity(matchQuantity, rwNotLock);
				IncreaseSellDayTradeOpenAmount(matchAmount, rwNotLock);
				IncreaseSellDayTradeOpenTax(matchTax, rwNotLock);
				IncreaseSellDayTradeOpenFee(matchFee, rwNotLock);
				AddOpenInterest(matchQuantity, MatchSummaryPtr, FSellDayTradeOIQueue);
			}
		}
		else if (positionEffect == nsOrderMessageDefine::peOpen)
		{
			if (side == nsOrderMessageDefine::sBuy)
			{
				IncreaseBuyOpenInterestQuantity(matchQuantity, rwNotLock);
				IncreaseBuyOpenInterestAmount(matchAmount, rwNotLock);
				IncreaseBuyOpenQuantity(matchQuantity, rwNotLock);
				IncreaseBuyOpenAmount(matchAmount, rwNotLock);
				IncreaseBuyOpenTax(matchTax, rwNotLock);
				IncreaseBuyOpenFee(matchFee, rwNotLock);
				AddOpenInterest(matchQuantity, MatchSummaryPtr, FBuyOIQueue);
			}
			else if (side == nsOrderMessageDefine::sSell)
			{
				IncreaseSellOpenInterestQuantity(matchQuantity, rwNotLock);
				IncreaseSellOpenInterestAmount(matchAmount, rwNotLock);
				IncreaseSellOpenQuantity(matchQuantity, rwNotLock);
				IncreaseSellOpenAmount(matchAmount, rwNotLock);
				IncreaseSellOpenTax(matchTax, rwNotLock);
				IncreaseSellOpenFee(matchFee, rwNotLock);
				AddOpenInterest(matchQuantity, MatchSummaryPtr, FSellOIQueue);
			}
		}
		else if ((positionEffect == nsOrderMessageDefine::peClose) || (positionEffect == nsOrderMessageDefine::peDayTrade))
		{
			int closeQuantity = OffsetOpenInterest(side, positionEffect, matchQuantity, matchAmount, matchTax, matchFee, MatchSummaryPtr);
			if (closeQuantity < matchQuantity)
			{
				isSuccess = false;
				logMsg.printf(L"OrderId[%s] ExecId[%s] [%s-%s] has no enough open interest.", orderId, execId, FExchangeCode, FSymbol);
			}
		}
		else
		{
			isSuccess = false;
			logMsg.printf(L"OrderId[%s] ExecId[%s] Wrong PositionEffect %d[%s]", orderId, execId, positionEffect, DescribePositionEffect(positionEffect));
		}

		if (side == nsOrderMessageDefine::sBuy)
		{
			IncreaseBuyQuantity(matchQuantity, rwNotLock);
			IncreaseBuyAmount(matchAmount, rwNotLock);
			IncreaseBuyTax(matchTax, rwNotLock);
			IncreaseBuyFee(matchFee, rwNotLock);
		}
		else if (side == nsOrderMessageDefine::sSell)
		{
			IncreaseSellQuantity(matchQuantity, rwNotLock);
			IncreaseSellAmount(matchAmount, rwNotLock);
			IncreaseSellTax(matchTax, rwNotLock);
			IncreaseSellFee(matchFee, rwNotLock);
		}

		IncreaseTax(matchTax, rwNotLock);
		IncreaseFee(matchFee, rwNotLock);
	}
	else
	{
		isSuccess = false;
		logMsg.printf(L"OrderId[%s] ExecId[%s] has Wrong Side %d[%s]", orderId, execId, side, DescribeBuySell(side));
	}  //if ((side == nsOrderMessageDefine::sBuy) || (side == nsOrderMessageDefine::sSell))

	if (logMsg.Length() > 0) WriteUFCLog(L"CPositionStatisticRecord::ProcessMatchSummary()", logMsg, constFlush);
	return isSuccess;
}  //CPositionStatisticRecord::ProcessMatchSummary()
//---------------------------------------------------------------------------
bool CPositionStatisticRecord::ProcessMatchSummaryBySide(CMatchSummary *MatchSummaryPtr)
{   //非大陸商品使用自動倉
	String logHaed = L"CPositionStatisticRecord::ProcessMatchSummaryBySide()";
	String logMsg = L"";
	if ((FMarket == nsOrderMessageDefine::mCNFutures) || (FMarket == nsOrderMessageDefine::mCNOptions))
	{
		logMsg.printf(L"Wrong Market.");
		WriteUFCLog(logHaed, logMsg, constFlush);
		return false;
	}

	double unitFee = 0.0;
	double priceFeeRate = 0.0;
	GetFee(nsOrderMessageDefine::peOpen, priceFeeRate, unitFee, rwNotLock);  //取得稅率資料
	MatchSummaryPtr->LockForWrite();
	String orderId = MatchSummaryPtr->GetOrderId(rwNotLock);
	String execId = MatchSummaryPtr->GetExecId(rwNotLock);
	nsOrderMessageDefine::SideEnum side = MatchSummaryPtr->GetSide(rwNotLock);
	double matchPrice = MatchSummaryPtr->GetMatchPrice(rwNotLock);
	int matchQuantity = MatchSummaryPtr->GetMatchQuantity(rwNotLock);
	MatchSummaryPtr->CalculateTradeData(FContractMultiplier, FTaxRate, unitFee, priceFeeRate, rwNotLock);
	double matchAmount = MatchSummaryPtr->GetMatchAmount(rwNotLock);
	double matchTax = MatchSummaryPtr->GetTax(rwNotLock);
	double matchFee = MatchSummaryPtr->GetFee(rwNotLock);
	MatchSummaryPtr->UnlockForWrite();
	if ((side != nsOrderMessageDefine::sBuy) && (side != nsOrderMessageDefine::sSell))
	{
		logMsg.printf(L"OrderId[%s] ExecId[%s] [%s-%s] has wrong Side[%d].", orderId, execId, FExchangeCode, FSymbol, side);
		WriteUFCLog(logHaed, logMsg, constFlush);
		return false;
	}

	int openMatchQuantity = 0;
	int closeMatchQuantity = 0;
	if (side == nsOrderMessageDefine::sBuy)
	{   //成交在買邊
		if (FSellOpenInterestQuantity > 0)
		{   //賣邊有未平倉數量
			if (FSellOpenInterestQuantity >= matchQuantity)  //買邊成交量未超過賣邊未平倉數量
				closeMatchQuantity = matchQuantity;  //全部拿來平倉
			else
			{   //買邊成交量超過賣邊未平倉數量
				closeMatchQuantity = FSellOpenInterestQuantity;  //將賣邊完全平倉
				openMatchQuantity = matchQuantity - FSellOpenInterestQuantity;  //增加買邊新倉
			}
		}
		else  //賣邊無未平倉數量
			openMatchQuantity = matchQuantity;  //全部是買邊新倉
	}
	else if (side == nsOrderMessageDefine::sSell)
	{   //成交在賣邊
		if (FBuyOpenInterestQuantity > 0)
		{   //買邊有未平倉數量
			if (FBuyOpenInterestQuantity >= matchQuantity)  //賣邊成交量未超過買邊未平倉數量
				closeMatchQuantity = matchQuantity;  //全部拿來平倉
			else
			{   //賣邊成交量超過買邊未平倉數量
				closeMatchQuantity = FBuyOpenInterestQuantity;  //將買邊完全平倉
				openMatchQuantity = matchQuantity - FBuyOpenInterestQuantity;  //增加賣邊新倉
			}
		}
		else  //買邊無未平倉數量
			openMatchQuantity = matchQuantity;  //全部是賣邊新倉
	}  //if (side == nsOrderMessageDefine::sBuy)

	//計算手續費，交易稅
	double matchAmountPerLot = matchPrice * FContractMultiplier;
	int tempTaxPerLot = (matchAmountPerLot * FTaxRate) + 0.5;
	double taxPerLot = tempTaxPerLot;
	double priceFeePerLot = matchAmountPerLot * priceFeeRate;
	double feePerLot = unitFee + priceFeePerLot;

	//找出統計成交價資料
	CMatchPriceStatisticRecord *pxQtyStatisticPtr = FindOrCreatPxQtyStatistic(side, matchPrice, rwNotLock);

	bool isSuccess = true;
	//處理平倉
	if (closeMatchQuantity > 0)
	{   //統計成交價資料
		if (pxQtyStatisticPtr != 0) pxQtyStatisticPtr->InceraseMatchQuantity(nsOrderMessageDefine::peClose, closeMatchQuantity, rwLockForWrite);

		double closeMatchAmount = matchAmountPerLot * closeMatchQuantity;
		double closeTax = taxPerLot * closeMatchQuantity;
		double closeFee = feePerLot * closeMatchQuantity;
		int closeQuantity = OffsetOpenInterest(side, nsOrderMessageDefine::peClose, closeMatchQuantity, closeMatchAmount, closeTax, closeFee, MatchSummaryPtr);
		if (closeQuantity < matchQuantity)
		{
			isSuccess = false;
			logMsg.printf(L"OrderId[%s] ExecId[%s] [%s-%s] has no enough open interest.", orderId, execId, FExchangeCode, FSymbol);
			WriteUFCLog(logHaed, logMsg, constFlush);
		}
	}  //if (closeMatchQuantity > 0)

	//處理新倉
	if (openMatchQuantity > 0)
	{   //統計成交價資料
		if (pxQtyStatisticPtr != 0) pxQtyStatisticPtr->InceraseMatchQuantity(nsOrderMessageDefine::peOpen, openMatchQuantity, rwLockForWrite);

		double openMatchAmount = matchAmountPerLot * openMatchQuantity;
		double openTax = taxPerLot * openMatchQuantity;
		double openFee = feePerLot * openMatchQuantity;
		if (side == nsOrderMessageDefine::sBuy)
		{
			IncreaseBuyOpenInterestQuantity(openMatchQuantity, rwNotLock);
			IncreaseBuyOpenInterestAmount(openMatchAmount, rwNotLock);
			IncreaseBuyOpenQuantity(openMatchQuantity, rwNotLock);
			IncreaseBuyOpenAmount(openMatchAmount, rwNotLock);
			IncreaseBuyOpenTax(openTax, rwNotLock);
			IncreaseBuyOpenFee(openFee, rwNotLock);
			AddOpenInterest(openMatchQuantity, MatchSummaryPtr, FBuyOIQueue);
		}
		else if (side == nsOrderMessageDefine::sSell)
		{
			IncreaseSellOpenInterestQuantity(openMatchQuantity, rwNotLock);
			IncreaseSellOpenInterestAmount(openMatchAmount, rwNotLock);
			IncreaseSellOpenQuantity(openMatchQuantity, rwNotLock);
			IncreaseSellOpenAmount(openMatchAmount, rwNotLock);
			IncreaseSellOpenTax(openTax, rwNotLock);
			IncreaseSellOpenFee(openFee, rwNotLock);
			AddOpenInterest(openMatchQuantity, MatchSummaryPtr, FSellOIQueue);
		}
	}  //if (openMatchQuantity > 0)

	if (side == nsOrderMessageDefine::sBuy)
	{
		IncreaseBuyQuantity(matchQuantity, rwNotLock);
		IncreaseBuyAmount(matchAmount, rwNotLock);
		IncreaseBuyTax(matchTax, rwNotLock);
		IncreaseBuyFee(matchFee, rwNotLock);
	}
	else if (side == nsOrderMessageDefine::sSell)
	{
		IncreaseSellQuantity(matchQuantity, rwNotLock);
		IncreaseSellAmount(matchAmount, rwNotLock);
		IncreaseSellTax(matchTax, rwNotLock);
		IncreaseSellFee(matchFee, rwNotLock);
	}

	IncreaseTax(matchTax, rwNotLock);
	IncreaseFee(matchFee, rwNotLock);
	return isSuccess;
}  //CPositionStatisticRecord::ProcessMatchSummaryBySide()
//---------------------------------------------------------------------------
int CPositionStatisticRecord::CalculateOIQueueFloatingData(double ContractMultiplier, double LastPrice, CTradingObjQueue& OIQueue, double& FloatingProfit, double& FloatingTax, double& FloatingFee)
{
	FloatingProfit = 0.0;
	FloatingTax = 0.0;
	FloatingFee = 0.0;
	int openOffsetUnitCount = GetOIQueueCount(OIQueue, rwNotLock);
	for (int i = 0; i < openOffsetUnitCount; i++)
	{
		COffsetUnitRecord *curOffsetUnitRecPtr = GetOffsetUnitRecFromOIQueue(OIQueue, i, rwNotLock);
		curOffsetUnitRecPtr->LockForWrite();
		double curFloatingProfit = curOffsetUnitRecPtr->CalculateFloatingProfit(ContractMultiplier, LastPrice, rwNotLock);
		FloatingProfit += curFloatingProfit;
		CMatchSummary *openMatchSummaryPtr = curOffsetUnitRecPtr->GetOpenMatchSummary(rwNotLock);
		curOffsetUnitRecPtr->UnlockForWrite();
		if (openMatchSummaryPtr != 0)
		{
			openMatchSummaryPtr->LockForRead();
			nsOrderMessageDefine::MarketEnum market = openMatchSummaryPtr->GetMarket(rwNotLock);
			if (!openMatchSummaryPtr->IsHoldPositionData(rwNotLock) ||
				((market != nsOrderMessageDefine::mCNFutures) && (market != nsOrderMessageDefine::mCNOptions)))
			{
				FloatingTax += openMatchSummaryPtr->GetTaxPerLot(rwNotLock);
				FloatingFee += openMatchSummaryPtr->GetFeePerLot(rwNotLock);
			}
			openMatchSummaryPtr->UnlockForRead();
		}  //if (openMatchSummaryPtr != 0)
	}  //for (int i = 0; i < openOffsetUnitCount; i++)
	return openOffsetUnitCount;
}  //CPositionStatisticRecord::CalculateOIQueueFloatingData()
//---------------------------------------------------------------------------
double CPositionStatisticRecord::CalculateFloatingData(double ContractMultiplier, double LastPrice, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FBuyFloatingProfit = 0.0;
	FBuyOpenInterestTax = 0.0;
	FBuyOpenInterestFee = 0.0;
	double queueProfit, queueTax, queueFee;
	CalculateOIQueueFloatingData(ContractMultiplier, LastPrice, FBuyDayTradeOIQueue, queueProfit, queueTax, queueFee);
	FBuyFloatingProfit += queueProfit;
	FBuyOpenInterestTax += queueTax;
	FBuyOpenInterestFee += queueFee;
	CalculateOIQueueFloatingData(ContractMultiplier, LastPrice, FBuyHoldOIQueue, queueProfit, queueTax, queueFee);
	FBuyFloatingProfit += queueProfit;
	FBuyOpenInterestTax += queueTax;
	FBuyOpenInterestFee += queueFee;
	CalculateOIQueueFloatingData(ContractMultiplier, LastPrice, FBuyOIQueue, queueProfit, queueTax, queueFee);
	FBuyFloatingProfit += queueProfit;
	FBuyOpenInterestTax += queueTax;
	FBuyOpenInterestFee += queueFee;

	FSellFloatingProfit = 0.0;
	FSellOpenInterestTax = 0.0;
	FSellOpenInterestFee = 0.0;
	CalculateOIQueueFloatingData(ContractMultiplier, LastPrice, FSellDayTradeOIQueue, queueProfit, queueTax, queueFee);
	FSellFloatingProfit += queueProfit;
	FSellOpenInterestTax += queueTax;
	FSellOpenInterestFee += queueFee;
	CalculateOIQueueFloatingData(ContractMultiplier, LastPrice, FSellHoldOIQueue, queueProfit, queueTax, queueFee);
	FSellFloatingProfit += queueProfit;
	FSellOpenInterestTax += queueTax;
	FSellOpenInterestFee += queueFee;
	CalculateOIQueueFloatingData(ContractMultiplier, LastPrice, FSellOIQueue, queueProfit, queueTax, queueFee);
	FSellFloatingProfit += queueProfit;
	FSellOpenInterestTax += queueTax;
	FSellOpenInterestFee += queueFee;
	FOpenInterestTax = FBuyOpenInterestTax + FSellOpenInterestTax;
	FOpenInterestFee = FBuyOpenInterestFee + FSellOpenInterestFee;
	FFloatingProfit = FBuyFloatingProfit + FSellFloatingProfit;
	Unlock(RWLockType);
	return FFloatingProfit;
}  //CPositionStatisticRecord::CalculateFloatingData()
//---------------------------------------------------------------------------
double CPositionStatisticRecord::CalculateFloatingData(ReadWrietLockTypeEnum RWLockType)
{
	return CalculateFloatingData(FContractMultiplier, FLastPrice, RWLockType);
}  //CPositionStatisticRecord::CalculateFloatingData()
//---------------------------------------------------------------------------
bool CPositionStatisticRecord::AddFillReportData(const String& OrderId, const String& ExecId,
												 nsOrderMessageDefine::MarketEnum Market,
												 nsOrderMessageDefine::SideEnum Side,
												 nsOrderMessageDefine::PositionEffectEnum PositionEffect,
												 double MatchPrice, int MatchQuantity, ReadWrietLockTypeEnum RWLockType)
{
	bool isSuccess = false;
	String logHead = L"CPositionStatisticRecord::AddFillReportData()";
	String errorLog = L"";
	if ((OrderId.Length() <= 0) || (ExecId.Length() <= 0)) return isSuccess;
	Lock(RWLockType);
	if (FindMatchSummaryFromExecIdIndex(ExecId) == 0)
	{   //Add New Match Summary
		CMatchSummary *newMatchSummaryPtr = new CMatchSummary(OrderId, ExecId, FExchangeCode, FSymbol,
															  Market, Side, PositionEffect, MatchPrice, MatchQuantity, FTradeSymbol);
		if (InsertMatchSummaryIntoExecIdIndex(ExecId, newMatchSummaryPtr, constNotCheckExist, rwNotLock))
		{
			AppendMatchSummaryToArray(newMatchSummaryPtr, rwNotLock);
			FLastPrice = MatchPrice;
			if ((Market == nsOrderMessageDefine::mCNFutures) || (Market == nsOrderMessageDefine::mCNOptions))
				ProcessMatchSummary(newMatchSummaryPtr);
			else  //非大陸商品使用自動倉
				ProcessMatchSummaryBySide(newMatchSummaryPtr);
			CalculateFloatingData(rwNotLock);
			isSuccess = true;
		}
		else
		{
			errorLog.printf(L"Insert Match Summary(OrderId[%s] ExecId[%s] Symbol[%s-%s]) into ExecID index failed.", OrderId, ExecId, FExchangeCode, FSymbol);
			delete newMatchSummaryPtr;
		}
	}
	else
		errorLog.printf(L"Insert Match Summary(OrderId[%s] ExecId[%s] Symbol[%s-%s]) Exist in ExecID index failed.", OrderId, ExecId, FExchangeCode, FSymbol);
	Unlock(RWLockType);
	if (errorLog.Length() > 0) WriteUFCLog(logHead, errorLog, constFlush);
	return isSuccess;
}  //CPositionStatisticRecord::AddFillReportData()
//---------------------------------------------------------------------------
bool CPositionStatisticRecord::AddHoldPosition(const String& OrderId, const String& ExecId,
											   nsOrderMessageDefine::MarketEnum Market,
											   nsOrderMessageDefine::SideEnum Side,
											   nsOrderMessageDefine::PositionEffectEnum PositionEffect,
											   double MatchPrice, int MatchQuantity, ReadWrietLockTypeEnum RWLockType)
{
	bool isSuccess = false;
	String holdOrderId, holdExecId;
	Lock(RWLockType);
	int holdArrayCount = GetHoldMatchSummaryArrayCount(rwNotLock) + 1;
	if (OrderId.Length() <= 0)
		holdOrderId.printf(L"HoldOrder%04d", holdArrayCount);
	else
		holdOrderId = OrderId;
	if (ExecId.Length() <= 0)
		holdExecId.printf(L"HoldExec%04d", holdArrayCount);
	else
		holdExecId = ExecId;
	CMatchSummary *newMatchSummaryPtr = new CMatchSummary(holdOrderId, holdExecId, FExchangeCode, FSymbol,
														  Market, Side, PositionEffect, MatchPrice, MatchQuantity, FTradeSymbol);
	newMatchSummaryPtr->SetHoldPositionData(true, rwNotLock);
	isSuccess = AppendHoldMatchSummaryToArray(newMatchSummaryPtr, rwNotLock);
	Unlock(RWLockType);
	if (!isSuccess && (newMatchSummaryPtr != 0)) delete newMatchSummaryPtr;
	return isSuccess;
}  //CPositionStatisticRecord::AddHoldPosition()
//---------------------------------------------------------------------------
void CPositionStatisticRecord::ReOffsetPosition(ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	//將買邊統計資料歸零
	FBuyOpenInterestQuantity = 0;
	FBuyOpenInterestAmount = 0.0;
	FBuyOpenInterestTax = 0.0;
	FBuyOpenInterestFee = 0.0;
	FBuyHoldOpenQuantity = 0;
	FBuyHoldOpenAmount = 0.0;
	FBuyHoldOpenTax = 0.0;
	FBuyHoldOpenFee = 0.0;
	FBuyDayTradeOpenQuantity = 0;
	FBuyDayTradeOpenAmount = 0.0;
	FBuyDayTradeOpenTax = 0.0;
	FBuyDayTradeOpenFee = 0.0;
	FBuyOpenQuantity = 0;
	FBuyOpenAmount = 0.0;
	FBuyOpenTax = 0.0;
	FBuyOpenFee = 0.0;
	FBuyRolledQuantity = 0;
	FBuyRolledAmount = 0.0;
	FBuyRolledTax = 0.0;
	FBuyRolledFee = 0.0;
	FBuyCloseQuantity = 0;
	FBuyCloseAmount = 0.0;
	FBuyCloseTax = 0.0;
	FBuyCloseFee = 0.0;
	FBuyQuantity = 0;
	FBuyAmount = 0.0;
	FBuyTax = 0.0;
	FBuyFee = 0.0;
	FBuyCloseProfit = 0.0;
	FBuyFloatingProfit = 0.0;

	//將賣邊統計資料歸零
	FSellOpenInterestQuantity = 0;
	FSellOpenInterestAmount = 0.0;
	FSellOpenInterestTax = 0.0;
	FSellOpenInterestFee = 0.0;
	FSellHoldOpenQuantity = 0;
	FSellHoldOpenAmount = 0.0;
	FSellHoldOpenTax = 0.0;
	FSellHoldOpenFee = 0.0;
	FSellDayTradeOpenQuantity = 0;
	FSellDayTradeOpenAmount = 0.0;
	FSellDayTradeOpenTax = 0.0;
	FSellDayTradeOpenFee = 0.0;
	FSellOpenQuantity = 0;
	FSellOpenAmount = 0.0;
	FSellOpenTax = 0.0;
	FSellOpenFee = 0.0;
	FSellRolledQuantity = 0;
	FSellRolledAmount = 0.0;
	FSellRolledTax = 0.0;
	FSellRolledFee = 0.0;
	FSellCloseQuantity = 0;
	FSellCloseAmount = 0.0;
	FSellCloseTax = 0.0;
	FSellCloseFee = 0.0;
	FSellQuantity = 0;
	FSellAmount = 0.0;
	FSellTax = 0.0;
	FSellFee = 0.0;
	FSellCloseProfit = 0.0;
	FSellFloatingProfit = 0.0;

	//將整體統計資料歸零
	FTax = 0.0;
	FFee = 0.0;
	FCloseProfit = 0.0;
	FOpenInterestTax = 0.0;
	FOpenInterestFee = 0.0;
	FFloatingProfit = 0.0;

	//清空平倉資料
	ClearOffsetUnitRecArray(FBuyCloseTodayOffsetArray, constDelete, rwNotLock);
	ClearOffsetUnitRecArray(FBuyCloseOffsetArray, constDelete, rwNotLock);
	ClearOffsetUnitRecArray(FSellCloseTodayOffsetArray, constDelete, rwNotLock);
	ClearOffsetUnitRecArray(FSellCloseOffsetArray, constDelete, rwNotLock);

	//清空未平倉資料
	ClearOIQueue(FBuyDayTradeOIQueue, constDelete, rwNotLock);
	ClearOIQueue(FBuyHoldOIQueue, constDelete, rwNotLock);
	ClearOIQueue(FBuyOIQueue, constDelete, rwNotLock);
	ClearOIQueue(FSellDayTradeOIQueue, constDelete, rwNotLock);
	ClearOIQueue(FSellHoldOIQueue, constDelete, rwNotLock);
	ClearOIQueue(FSellOIQueue, constDelete, rwNotLock);

	//清空成交價統計資料
	ClearBuyPxQtyStatisticArray(constNotDelete, rwNotLock);
	ClearBuyPxQtyStatisticPriceIndex(constDelete, rwNotLock);
	ClearSellPxQtyStatisticArray(constNotDelete, rwNotLock);
	ClearSellPxQtyStatisticPriceIndex(constDelete, rwNotLock);

	for (int i = 0; i < GetObjectContainerCount(FHoldMatchSummaryArray, FCSLockPtr, rwNotLock); i++)
	{   //處理昨日持倉資料
		CMatchSummary *matchSummaryPtr = GetHoldMatchSummaryFromArray(i, rwNotLock);
		matchSummaryPtr->LockForWrite();
		matchSummaryPtr->CalculateTradeData(FContractMultiplier, FTaxRate, FOpenUnitFee, FOpenFeeRate, rwNotLock);
		nsOrderMessageDefine::SideEnum side = matchSummaryPtr->GetSide(rwNotLock);
		int matchQuantity = matchSummaryPtr->GetMatchQuantity(rwNotLock); //成交量
		double matchPrice = matchSummaryPtr->GetMatchPrice(rwNotLock);    //成交價
		double matchAmount = matchSummaryPtr->GetMatchAmount(rwNotLock);  //成交金額
		double matchTax = matchSummaryPtr->GetTax(rwNotLock);             //交易稅
		double matchFee = matchSummaryPtr->GetFee(rwNotLock);             //手續費
		matchSummaryPtr->UnlockForWrite();

		//依照成交價統計資料
		CMatchPriceStatisticRecord *PxQtyStatisticPtr = FindOrCreatPxQtyStatistic(side, matchPrice, rwNotLock);
		if (PxQtyStatisticPtr != 0)
			PxQtyStatisticPtr->InceraseMatchQuantity(nsOrderMessageDefine::peOpen, matchQuantity, rwLockForWrite);

		if (side == nsOrderMessageDefine::sBuy)
		{
			IncreaseBuyQuantity(matchQuantity, rwNotLock);
			IncreaseBuyAmount(matchAmount, rwNotLock);
			IncreaseBuyHoldOpenQuantity(matchQuantity, rwNotLock);
			IncreaseBuyHoldOpenAmount(matchAmount, rwNotLock);
			IncreaseBuyHoldOpenTax(matchTax, rwNotLock);
			IncreaseBuyHoldOpenFee(matchFee, rwNotLock);
			IncreaseBuyOpenInterestQuantity(matchQuantity, rwNotLock);
			IncreaseBuyOpenInterestAmount(matchAmount, rwNotLock);
			AddOpenInterest(matchQuantity, matchSummaryPtr, FBuyHoldOIQueue);
		}
		else if (side == nsOrderMessageDefine::sSell)
		{
			IncreaseSellQuantity(matchQuantity, rwNotLock);
			IncreaseSellAmount(matchAmount, rwNotLock);
			IncreaseSellHoldOpenQuantity(matchQuantity, rwNotLock);
			IncreaseSellHoldOpenAmount(matchAmount, rwNotLock);
			IncreaseSellHoldOpenTax(matchTax, rwNotLock);
			IncreaseSellHoldOpenFee(matchFee, rwNotLock);
			IncreaseSellOpenInterestQuantity(matchQuantity, rwNotLock);
			IncreaseSellOpenInterestAmount(matchAmount, rwNotLock);
			AddOpenInterest(matchQuantity, matchSummaryPtr, FSellHoldOIQueue);
		}
		IncreaseTax(matchTax, rwNotLock);
		IncreaseFee(matchFee, rwNotLock);
	}  //for (int i = 0; i < GetObjectContainerCount(FHoldMatchSummaryArray, FRWLockPtr, rwNotLock); i++)

	for (int i = 0; i < GetObjectContainerCount(FMatchSummaryArray, FCSLockPtr, rwNotLock); i++)
	{   //處理今日成交資料
		CMatchSummary *matchSummaryPtr = GetMatchSummaryFromArray(i, rwNotLock);
		nsOrderMessageDefine::MarketEnum market = matchSummaryPtr->GetMarket(rwLockForRead);
		if ((market == nsOrderMessageDefine::mCNFutures) || (market == nsOrderMessageDefine::mCNOptions))
			ProcessMatchSummary(matchSummaryPtr);
		else  //非大陸商品使用自動倉
			ProcessMatchSummaryBySide(matchSummaryPtr);
	}  //for (int i = 0; i < GetObjectContainerCount(FMatchSummaryArray, FRWLockPtr, rwNotLock); i++)
	CalculateFloatingData(rwNotLock);
	Unlock(RWLockType);
}  //CPositionStatisticRecord::ReOffsetPosition()
//--------------------------------------------------------------------------
void CPositionStatisticRecord::DumpTitle(ReadWrietLockTypeEnum RWLockType)
{
	String dumpMsg = L"";
	String isMarketDataReceivedStr = L"No Market Data Received";
	Lock(RWLockType);
	if (FIsMarketDataReceived) isMarketDataReceivedStr = L"Market Data Received";
	dumpMsg.printf(L"Position Statistic[%s-%s(%s)] TaxRate=%-15.6lf LastPx=%-15.6lf Multiplier=%-15.6lf SubscribeCount=%d(%s)", FExchangeCode, FSymbol, FTradeSymbol, FTaxRate, FLastPrice, FContractMultiplier, FSubscribeCount, isMarketDataReceivedStr);
	WriteUFCLog(dumpMsg, constNotFlush);
	Unlock(RWLockType);
}  //CPositionStatisticRecord::DumpTitle()
//--------------------------------------------------------------------------
void CPositionStatisticRecord::DumpFee(ReadWrietLockTypeEnum RWLockType)
{
	String dumpMsg = L"";
	Lock(RWLockType);
	dumpMsg.printf(L"[%s-%s] Price  Fee Rate[Open=%-15.6lf Close=%-15.6lf CloseToday=%-15.6lf]", FExchangeCode, FSymbol, FOpenFeeRate, FCloseFeeRate, FCloseTodayFeeRate);
	WriteUFCLog(dumpMsg, constNotFlush);
	dumpMsg.printf(L"[%s-%s] Volume Fee Unit[Open=%-15.6lf Close=%-15.6lf CloseToday=%-15.6lf]", FExchangeCode, FSymbol, FOpenUnitFee, FCloseUnitFee, FCloseTodayUnitFee);
	WriteUFCLog(dumpMsg, constNotFlush);
	Unlock(RWLockType);
}  //CPositionStatisticRecord::DumpFee()
//--------------------------------------------------------------------------
void CPositionStatisticRecord::DumpQuantitySummary(ReadWrietLockTypeEnum RWLockType)
{
	String dumpMsg = L"";
	Lock(RWLockType);
	dumpMsg.printf(L"[%s-%s] Buy  Qty[OI=%4d HO=%4d DT=%4d O=%4d CT=%4d C=%4d Sum=%4d] Amt[%15.6lf]", FExchangeCode, FSymbol, FBuyOpenInterestQuantity, FBuyHoldOpenQuantity, FBuyDayTradeOpenQuantity, FBuyOpenQuantity, FBuyRolledQuantity, FBuyCloseQuantity, FBuyQuantity, FBuyAmount);
	WriteUFCLog(dumpMsg, constNotFlush);
	dumpMsg.printf(L"[%s-%s] Sell Qty[OI=%4d HO=%4d DT=%4d O=%4d CT=%4d C=%4d Sum=%4d] Amt[%15.6lf]", FExchangeCode, FSymbol, FSellOpenInterestQuantity, FSellHoldOpenQuantity, FSellDayTradeOpenQuantity, FSellOpenQuantity, FSellRolledQuantity, FSellCloseQuantity, FSellQuantity, FSellAmount);
	WriteUFCLog(dumpMsg, constNotFlush);
	dumpMsg.printf(L"[%s-%s] CloseProfit[%15.6lf] Tax[%15.6lf] Fee[%15.6lf]", FExchangeCode, FSymbol, FCloseProfit, FTax, FFee);
	WriteUFCLog(dumpMsg, constNotFlush);
	dumpMsg.printf(L"[%s-%s] FloatingProfit[%15.6lf] OITax[%15.6lf] OIFee[%15.6lf]", FExchangeCode, FSymbol, FFloatingProfit, FOpenInterestTax, FOpenInterestFee);
	WriteUFCLog(dumpMsg, constNotFlush);
	Unlock(RWLockType);
}  //CPositionStatisticRecord::DumpQuantitySummary()
//--------------------------------------------------------------------------
void CPositionStatisticRecord::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String dumpMsg = L"";
	Lock(RWLockType);
	DumpTitle(rwNotLock);
	DumpFee(rwNotLock);
	DumpQuantitySummary(rwNotLock);
/*
	UFC::BufferedLog::Printf(" === HoldMatchSummary %d ===", FHoldMatchSummaryArray.size());
	DumpBasicTradingObjArray(FHoldMatchSummaryArray, constNotSort);
	UFC::BufferedLog::Printf(" === MatchSummary %d ===", FMatchSummaryArray.size());
	DumpBasicTradingObjArray(FMatchSummaryArray, constNotSort);
	UFC::BufferedLog::Printf(" === Buy DayTrade OI Queue %d ===", FBuyDayTradeOIQueue.size());
	DumpBasicTradingObjQueue(FBuyDayTradeOIQueue);
	UFC::BufferedLog::Printf(" === Buy Hold OI Queue %d ===", FBuyHoldOIQueue.size());
	DumpBasicTradingObjQueue(FBuyHoldOIQueue);
	UFC::BufferedLog::Printf(" === Buy OI Queue %d ===", FBuyOIQueue.size());
	DumpBasicTradingObjQueue(FBuyOIQueue);
	UFC::BufferedLog::Printf(" === Sell DayTrade OI Queue %d ===", FSellDayTradeOIQueue.size());
	DumpBasicTradingObjQueue(FSellDayTradeOIQueue);
	UFC::BufferedLog::Printf(" === Sell Hold OI Queue %d ===", FSellHoldOIQueue.size());
	DumpBasicTradingObjQueue(FSellHoldOIQueue);
	UFC::BufferedLog::Printf(" === Sell OI Queue %d ===", FSellOIQueue.size());
	DumpBasicTradingObjQueue(FSellOIQueue);
	UFC::BufferedLog::Printf(" === Buy Close Today Offset %d ===", FBuyCloseTodayOffsetArray.size());
	DumpBasicTradingObjArray(FBuyCloseTodayOffsetArray, constNotSort);
	UFC::BufferedLog::Printf(" === Buy Close Offset %d ===", FBuyCloseOffsetArray.size());
	DumpBasicTradingObjArray(FBuyCloseOffsetArray, constNotSort);
	UFC::BufferedLog::Printf(" === Sell Close Today Offset %d ===", FSellCloseTodayOffsetArray.size());
	DumpBasicTradingObjArray(FSellCloseTodayOffsetArray, constNotSort);
	UFC::BufferedLog::Printf(" === Sell Close Offset %d ===", FSellCloseOffsetArray.size());
	DumpBasicTradingObjArray(FSellCloseOffsetArray, constNotSort);
	UFC::BufferedLog::Printf(" === Buy Price Statistic %d ===", FBuyPxQtyStatisticArray.size());
	DumpBasicTradingObjArray(FBuyPxQtyStatisticArray, constNotSort);
	UFC::BufferedLog::Printf(" === Sell Price Statistic %d ===", FSellPxQtyStatisticArray.size());
	DumpBasicTradingObjArray(FSellPxQtyStatisticArray, constNotSort);
*/
	Unlock(RWLockType);
	if (DoFlushToFile) FlushUFCLog();
}  //CPositionStatisticRecord::DumpData()
};
#pragma package(smart_init)
