//---------------------------------------------------------------------------


#pragma hdrstop

#include <WideStrUtils.hpp>
#include "TradingCommodities.h"

namespace s888
{
//---------------------------- CTradingExchange -----------------------------
String CTradingExchange::ConvertExchangeCodeToBrokerCode(const String& ExchangeCode)
{
	String brokerExchangeCode;
	if (ExchangeCode == L"CBOT")
		brokerExchangeCode = L"CBT";
	else if (ExchangeCode == L"NYMEX")
		brokerExchangeCode = L"NYM";
	else if (ExchangeCode == L"HKEx")
		brokerExchangeCode = L"HKF";
	else if (ExchangeCode == L"NBT")
		brokerExchangeCode = L"NYBOT";
	else if (ExchangeCode == L"TCE")
		brokerExchangeCode = L"TOCOM";
	else
		brokerExchangeCode = ExchangeCode;
	return brokerExchangeCode;
}  //CTradingExchange::ConvertExchangeCodeToBrokerCode()
//---------------------------------------------------------------------------
String CTradingExchange::ConvertBrokerExchangeCodeToCode(const String& BrokerExchangeCode)
{
	String exchangeCode;
	if (BrokerExchangeCode == L"CBT")
		exchangeCode = L"CBOT";
	else if (BrokerExchangeCode == L"NYM")
		exchangeCode = L"NYMEX";
	else if (BrokerExchangeCode == L"HKF")
		exchangeCode = L"HKEx";
	else if (BrokerExchangeCode == L"NYBOT")
		exchangeCode = L"NBT";
	else if (BrokerExchangeCode == L"TOCOM")
		exchangeCode = L"TCE";
	else
		exchangeCode = BrokerExchangeCode;
	return exchangeCode;
}  //CTradingExchange::ConvertBrokerExchangeCodeToCode()

//---------------------------------------------------------------------------
CTradingExchange::CTradingExchange(const String& ExchangeCode, const String& ExchangeName,
								   bool IsDayTradeOpenSupported,
								   bool IsStopOrderSupported,
								   const String& BrokerExchangeCode)
:CExchangeKey(constCreateRWLock, ExchangeCode, BrokerExchangeCode)
,FExchangeName(ExchangeName)
,FIsDayTradeOpenSupported(IsDayTradeOpenSupported)
,FIsStopOrderSupported(IsStopOrderSupported)
{
}  //CTradingExchange::CTradingExchange()
//---------------------------------------------------------------------------
void CTradingExchange::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	Lock(RWLockType);
	CExchangeKey::AssignFieldData(SrcObjPtr, rwNotLock);
	CTradingExchange *srcExchangePtr = static_cast<CTradingExchange*>(SrcObjPtr);
	FExchangeName = srcExchangePtr->GetExchangeName(rwLockForRead);
	Unlock(RWLockType);
}  //CTradingExchange::AssignFieldData()
//---------------------------------------------------------------------------
void CTradingExchange::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String dumpMessage;
	Lock(RWLockType);
	String dayTradeStr;
	if (FIsDayTradeOpenSupported)
		dayTradeStr = L"DayTrade[true],  ";
	else
		dayTradeStr = L"DayTrade[false], ";

	String stopOrderStr;
	if (FIsStopOrderSupported)
		stopOrderStr = L"StopOrder[true]";
	else
		stopOrderStr = L"StopOrder[false]";

	dumpMessage.printf(L"Exchange[%s]: %s%s Broker[%s].", FExchangeCode, dayTradeStr, stopOrderStr, FBrokerExchangeCode);
	Unlock(RWLockType);
	WriteUFCLog(dumpMessage, DoFlushToFile);
}  //CTradingExchange::DumpData()

//----------------------------- CTaxRateObject ------------------------------
//---------------------------------------------------------------------------
CTaxRateObject::CTaxRateObject(const String& ExchangeCode, const String& ContractCode,
							   double TaxRate, const String& BrokerContractCode,
							   nsOrderMessageDefine::MarketEnum Market)
:CExchangeContractKey(constCreateRWLock, ExchangeCode, ContractCode, Market, BrokerContractCode, ContractCode)
,FTaxRate(TaxRate)
{
}  //CTaxRateObject::CTaxRateObject()
//---------------------------------------------------------------------------
void CTaxRateObject::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	Lock(RWLockType);
	CExchangeContractKey::AssignFieldData(SrcObjPtr, rwNotLock);
	CTaxRateObject *srcTaxRatePtr = static_cast<CTaxRateObject*>(SrcObjPtr);
	FTaxRate = srcTaxRatePtr->GetTaxRate(rwLockForRead);
	Unlock(RWLockType);
}  //CTaxRateObject::AssignFieldData()
//---------------------------------------------------------------------------
void CTaxRateObject::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String dumpMessage;
	Lock(RWLockType);
	dumpMessage.printf(L"TaxRate[%s-%s] %15.6lf. Broker[%s].", FExchangeCode, FContractCode, FTaxRate, FBrokerContractCode);
	Unlock(RWLockType);
	WriteUFCLog(dumpMessage, DoFlushToFile);
}  //CTaxRateObject::DumpData()

//------------------------------- CFeeObject --------------------------------
//---------------------------------------------------------------------------
CFeeObject::CFeeObject(const String& Account, const String& ExchangeCode, const String& ContractCode, nsOrderMessageDefine::MarketEnum Market )
:CExchangeContractKey(constCreateRWLock, ExchangeCode, ContractCode, Market, ContractCode, ContractCode)
,FAccount(Account)
,FOpenRateByMoney(0.0)
,FOpenRateByVolume(0.0)
,FCloseRateByMoney(0.0)
,FCloseRateByVolume(0.0)
,FCloseTodayRateByMoney(0.0)
,FCloseTodayRateByVolume(0.0)
{
	if (FBrokerContractCode.Length() <= 0) FBrokerContractCode = ContractCode;
}  //CFeeObject::CFeeObject()
//---------------------------------------------------------------------------
CFeeObject::CFeeObject(const String& Account, const String& ExchangeCode, const String& ContractCode,
					   double OpenRateByMoney, double OpenRateByVolume,
					   double CloseRateByMoney, double CloseRateByVolume,
					   double CloseTodayRateByMoney, double CloseTodayRateByVolume,
					   nsOrderMessageDefine::MarketEnum Market )
:CExchangeContractKey(constCreateRWLock, ExchangeCode, ContractCode, Market, ContractCode, ContractCode )
,FAccount(Account)
,FOpenRateByMoney(OpenRateByMoney)
,FOpenRateByVolume(OpenRateByVolume)
,FCloseRateByMoney(CloseRateByMoney)
,FCloseRateByVolume(CloseRateByVolume)
,FCloseTodayRateByMoney(CloseTodayRateByMoney)
,FCloseTodayRateByVolume(CloseTodayRateByVolume)
{
}  //CFeeObject::CFeeObject()
//---------------------------------------------------------------------------
void CFeeObject::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	Lock(RWLockType);
	CExchangeContractKey::AssignFieldData(SrcObjPtr, rwNotLock);
	CFeeObject *srcFeePtr = static_cast<CFeeObject*>(SrcObjPtr);
	srcFeePtr->LockForRead();
	FAccount = srcFeePtr->GetAccount(rwNotLock);
	FOpenRateByMoney = srcFeePtr->GetOpenRateByMoney(rwNotLock);
	FOpenRateByVolume = srcFeePtr->GetOpenRateByVolume(rwNotLock);
	FCloseRateByMoney = srcFeePtr->GetCloseRateByMoney(rwNotLock);
	FCloseRateByVolume = srcFeePtr->GetCloseRateByVolume(rwNotLock);
	FCloseTodayRateByMoney = srcFeePtr->GetCloseTodayRateByMoney(rwNotLock);
	FCloseTodayRateByVolume = srcFeePtr->GetCloseTodayRateByVolume(rwNotLock);
	srcFeePtr->UnlockForRead();
	Unlock(RWLockType);
}  //CFeeObject::AssignFieldData()
//---------------------------------------------------------------------------
void CFeeObject::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String dumpMessage;
	Lock(RWLockType);
	dumpMessage.printf(L"Fee[%s][%s-%s] O[M=%10.6lf V=%10.6lf] C[M=%10.6lf V=%10.6lf] CT[M=%10.6lf V=%10.6lf], Trade[%s], Broker[%s].",
				FAccount, FExchangeCode, FContractCode,
				FOpenRateByMoney, FOpenRateByVolume, FCloseRateByMoney, FCloseRateByVolume, FCloseTodayRateByMoney, FCloseTodayRateByVolume,
				FTradeContractCode, FBrokerContractCode);
	Unlock(RWLockType);
	WriteUFCLog(dumpMessage, DoFlushToFile);
}  //CFeeObject::DumpData()

//------------------------ CCommodityCancelStatistic ------------------------
//---------------------------------------------------------------------------
CCommodityCancelStatistic::CCommodityCancelStatistic(const String& ExchangeCode, const String& ContractCode, const String& BrokerContractCode)
:CExchangeContractKey(constCreateRWLock, ExchangeCode, ContractCode, nsOrderMessageDefine::mOTC, BrokerContractCode, ContractCode)
,FCancelReportCount(0)
,FCancelQuantity(0)
{
}  //CCommodityCancelStatistic::CCommodityCancelStatistic()
//---------------------------------------------------------------------------
int CCommodityCancelStatistic::IncreaseCancelReportCount(int CancelReportCount, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	int newCancelReportCount = FCancelReportCount + CancelReportCount;
	FCancelReportCount = newCancelReportCount;
	Unlock(RWLockType);
	return newCancelReportCount;
}  //CCommodityCancelStatistic::IncreaseCancelReportCount()
//---------------------------------------------------------------------------
int CCommodityCancelStatistic::IncreaseCancelQuantity(int CancelQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	int newCancelQuantity = FCancelQuantity + CancelQuantity;
	FCancelQuantity = newCancelQuantity;
	Unlock(RWLockType);
	return newCancelQuantity;
}  //CCommodityCancelStatistic::IncreaseCancelQuantity()
//---------------------------------------------------------------------------
void CCommodityCancelStatistic::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	Lock(RWLockType);
	CExchangeContractKey::AssignFieldData(SrcObjPtr, rwNotLock);
	CCommodityCancelStatistic *srcCancelStatisticPtr = static_cast<CCommodityCancelStatistic*>(SrcObjPtr);
	srcCancelStatisticPtr->LockForRead();
	FCancelReportCount = srcCancelStatisticPtr->GetCancelReportCount(rwNotLock);
	FCancelQuantity = srcCancelStatisticPtr->GetCancelQuantity(rwNotLock);
	srcCancelStatisticPtr->UnlockForRead();
	Unlock(RWLockType);
}  //CCommodityCancelStatistic::AssignFieldData()
//---------------------------------------------------------------------------
void CCommodityCancelStatistic::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String dumpMessage;
	Lock(RWLockType);
	dumpMessage.printf(L"Commodity[%s-%s] Cancel Statistic: Qty=%d Times=%d.", FExchangeCode, FContractCode, FCancelQuantity, FCancelReportCount);
	Unlock(RWLockType);
	WriteUFCLog(dumpMessage, DoFlushToFile);
}  //CCommodityCancelStatistic::DumpData()

//------------------------ CContractCancelStatistic -------------------------
//---------------------------------------------------------------------------
CContractCancelStatistic::CContractCancelStatistic(const String& ExchangeCode, const String& Symbol)
:CExchangeSymbolKey(constCreateRWLock, ExchangeCode, Symbol, nsOrderMessageDefine::mOTC, Symbol, Symbol)
,FCancelReportCount(0)
,FCancelQuantity(0)
{
}  //CContractCancelStatistic::CContractCancelStatistic()
//---------------------------------------------------------------------------
int CContractCancelStatistic::IncreaseCancelReportCount(int CancelReportCount, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	int newCancelReportCount = FCancelReportCount + CancelReportCount;
	FCancelReportCount = newCancelReportCount;
	Unlock(RWLockType);
	return newCancelReportCount;
}  //CContractCancelStatistic::IncreaseCancelReportCount()
//---------------------------------------------------------------------------
int CContractCancelStatistic::IncreaseCancelQuantity(int CancelQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	int newCancelQuantity = FCancelQuantity + CancelQuantity;
	FCancelQuantity = newCancelQuantity;
	Unlock(RWLockType);
	return newCancelQuantity;
}  //CContractCancelStatistic::IncreaseCancelQuantity()
//---------------------------------------------------------------------------
void CContractCancelStatistic::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	Lock(RWLockType);
	CExchangeSymbolKey::AssignFieldData(SrcObjPtr, rwNotLock);
	CCommodityCancelStatistic *srcCancelStatisticPtr = static_cast<CCommodityCancelStatistic*>(SrcObjPtr);
	srcCancelStatisticPtr->LockForRead();
	FCancelReportCount = srcCancelStatisticPtr->GetCancelReportCount(rwNotLock);
	FCancelQuantity = srcCancelStatisticPtr->GetCancelQuantity(rwNotLock);
	srcCancelStatisticPtr->UnlockForRead();
	Unlock(RWLockType);
}  //CContractCancelStatistic::AssignFieldData()
//---------------------------------------------------------------------------
void CContractCancelStatistic::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String dumpMessage;
	Lock(RWLockType);
	dumpMessage.printf(L"Contract[%s-%s] Cancel Statistic: Qty=%d Times=%d.", FExchangeCode, FSymbol, FCancelQuantity, FCancelReportCount);
	Unlock(RWLockType);
	WriteUFCLog(dumpMessage, DoFlushToFile);
}  //CContractCancelStatistic::DumpData()

//---------------------------- CTradingCommodity ----------------------------
//---------------------------------------------------------------------------
CTradingCommodity::CTradingCommodity(const String& ExchangeCode, const String& ContractCode,
									 nsOrderMessageDefine::MarketEnum Market,
									 bool IsDayTradeOpenSupported, bool IsStopOrderSupported,
									 double TaxRate,
									 CTradingExchange *ExchangePtr)
:CExchangeContractKey(constCreateRWLock, ExchangeCode, ContractCode, Market, ContractCode, ContractCode)
,FCommodityName(ContractCode)
,FIsDayTradeOpenSupported(IsDayTradeOpenSupported)
,FIsStopOrderSupported(IsStopOrderSupported)
,FTaxRate(TaxRate)
,FCurrency(L"")
,FContractMultiplier(1.0)
,FContractKind(L"")
,FExchangePtr(ExchangePtr)
{
}  //CTradingCommodity::CTradingCommodity()
//---------------------------------------------------------------------------
CTradingCommodity::~CTradingCommodity()
{
	ClearContractSymbolIndex(constNotDelete, rwLockForWrite);
}  //CTradingCommodity::~CTradingCommodity()
//---------------------------------------------------------------------------
CTradingContract *CTradingCommodity::FindContractBySymbol(const String& Symbol, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingContract::BuildKey(FExchangeCode, Symbol);
	return static_cast<CTradingContract*>(FindTradingObjFromMap(FContractSymbolIndex, keyValue, FCSLockPtr, RWLockType));
}  //CTradingCommodity::FindContractBySymbol()
//---------------------------------------------------------------------------
bool CTradingCommodity::InsertContractIntoSymbolIndex(const String& Symbol, CTradingContract *ContractPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingContract::BuildKey(FExchangeCode, Symbol);
	return InsertTradingObjIntoMap(FContractSymbolIndex, keyValue, ContractPtr, DoCheckExist, FCSLockPtr, RWLockType);
}  //CTradingCommodity::InsertContractIntoSymbolIndex()
//---------------------------------------------------------------------------
CTradingContract *CTradingCommodity::RemoveContractFromSymbolIndex(const String& Symbol, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingContract::BuildKey(FExchangeCode, Symbol);
	return static_cast<CTradingContract*>(RemoveTradingObjFromMap(FContractSymbolIndex, keyValue, FCSLockPtr, RWLockType));
}  //CTradingCommodity::RemoveContractFromSymbolIndex()
//---------------------------------------------------------------------------
int CTradingCommodity::ClearContractSymbolIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FContractSymbolIndex, DoDelete, FCSLockPtr, RWLockType);
}  //CTradingCommodity::ClearContractSymbolIndex()
//---------------------------------------------------------------------------
CTradingContract *CTradingCommodity::GetFirstContract(ReadWrietLockTypeEnum RWLockType)
{
	CTradingContract *contractPtr = 0;
	Lock(RWLockType);
	if (GetContractSymbolIndexCount(s888::rwNotLock) > 0)
	{
		CWStrTradingObjPtrHMap::iterator it = FContractSymbolIndex.begin();
		if (it != FContractSymbolIndex.end())
		{
			CTradingBasicObject *tradingObjPtr = it->second;
			if (tradingObjPtr != 0) contractPtr = static_cast<CTradingContract*>(tradingObjPtr);
		}
	}  //if (GetContractSymbolIndexCount(s888::rwNotLock) > 0)
	Unlock(RWLockType);
	return contractPtr;
}  //CTradingCommodity::GetFirstContract()
//---------------------------------------------------------------------------
void CTradingCommodity::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	Lock(RWLockType);
	CExchangeContractKey::AssignFieldData(SrcObjPtr, rwNotLock);
	CTradingCommodity *srcCommodityPtr = static_cast<CTradingCommodity*>(SrcObjPtr);
	srcCommodityPtr->LockForRead();
	FCommodityName = srcCommodityPtr->GetCommodityName(rwNotLock);
	FIsDayTradeOpenSupported = srcCommodityPtr->IsDayTradeOpenSupported(rwNotLock);
	FIsStopOrderSupported = srcCommodityPtr->IsStopOrderSupported(rwNotLock);
	FTaxRate = srcCommodityPtr->GetTaxRate(rwNotLock);
	FCurrency = srcCommodityPtr->GetCurrency(rwNotLock);
	FContractMultiplier = srcCommodityPtr->GetContractMultiplier(rwNotLock);
	FContractKind = srcCommodityPtr->GetContractKind(rwNotLock);
	FExchangePtr = srcCommodityPtr->GetExchange(rwNotLock);
	srcCommodityPtr->UnlockForRead();
	Unlock(RWLockType);
}  //CTradingCommodity::AssignFieldData()
//---------------------------------------------------------------------------
void CTradingCommodity::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	String dayTradeStr;
	if (FIsDayTradeOpenSupported)
		dayTradeStr = L"DayTrade[true]";
	else
		dayTradeStr = L"DayTrade[false]";

	String stopOrderStr;
	if (FIsStopOrderSupported)
		stopOrderStr = L"StopOrder[true]";
	else
		stopOrderStr = L"StopOrder[false]";

	String taxRateStr;
	taxRateStr.printf(L"TaxRate=%12.8lf", FTaxRate);

	String dumpMessage;
	dumpMessage.printf(L"Commodity[%s-%s] %s %s Multiplier[%12.5lf] TaxRate[%12.5lf]. Market[%s],Broker[%s],Trade[%s],Name[%s]",
					   FExchangeCode, FContractCode, dayTradeStr, stopOrderStr, FContractMultiplier, FTaxRate, DescribeMarket(FMarket), FBrokerContractCode, FTradeContractCode, FCommodityName);
	Unlock(RWLockType);
	WriteUFCLog(dumpMessage, DoFlushToFile);
}  //CTradingCommodity::DumpData()

//---------------------------- CTradingContract -----------------------------
void CTradingContract::ParseMaturityYearMonth(const String& MaturityYearMonthStr,
											  int& MaturityYear, int& MaturityMonth)
{
	MaturityYear = 0;
	MaturityMonth = 0;
	int yearMonthStrLength = MaturityYearMonthStr.Length();
	if (yearMonthStrLength >= 4)
	{
		String yearStr = MaturityYearMonthStr.SubString(1, 4);
		if (IsDigitalString(yearStr)) MaturityYear = yearStr.ToInt();
	}

	if (yearMonthStrLength >= 6)
	{
		String monthStr = L"";
		int delimiterPosition = MaturityYearMonthStr.Pos(L"/");
		if (delimiterPosition <= 0)  //沒有分隔符號
			monthStr = MaturityYearMonthStr.SubString(5, 2);
		else  //有分隔符號
		{
			if ((yearMonthStrLength - delimiterPosition) >= 2)
				monthStr = MaturityYearMonthStr.SubString(delimiterPosition + 1, 2);
		}

		if ((monthStr.Length() > 0) && IsDigitalString(monthStr))
			MaturityMonth = monthStr.ToInt();
	}  //if (yearMonthStrLength >= 6)
}  //CTradingContract::ParseMaturityYearMonth()
//---------------------------------------------------------------------------
wchar_t CTradingContract::GenerateYearCode(int MaturityYear)
{
	String yearStr;
	yearStr.printf(L"%04d", MaturityYear);
	wchar_t yearCode = yearStr[4];
	return yearCode;
}  //CTradingContract::GenerateYearCode()
//---------------------------------------------------------------------------
wchar_t CTradingContract::GenerateTaifexMonthCode(int MaturityMonth,
												  nsOrderMessageDefine::EventTypeEnum CallOrPut)
{
	wchar_t monthCode;
	switch (MaturityMonth)
	{
		case  1:
			if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'M';
			else
				monthCode = L'A';
			break;
		case  2:
			if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'N';
			else
				monthCode = L'B';
			break;
		case  3:
			if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'O';
			else
				monthCode = L'C';
			break;
		case  4:
			if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'P';
			else
				monthCode = L'D';
			break;
		case  5:
			if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'Q';
			else
				monthCode = L'E';
			break;
		case  6:
			if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'R';
			else
				monthCode = L'F';
			break;
		case  7:
			if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'S';
			else
				monthCode = L'G';
			break;
		case  8:
			if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'T';
			else
				monthCode = L'H';
			break;
		case  9:
			if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'U';
			else
				monthCode = L'I';
			break;
		case 10:
			if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'V';
			else
				monthCode = L'J';
			break;
		case 11:
			if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'W';
			else
				monthCode = L'K';
			break;
		case 12:
			if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'X';
			else
				monthCode = L'L';
			break;
		default:
			monthCode = 0;
	}  //switch (MaturityMonth)
	return monthCode;
}  //CTradingContract::GenerateTaifexMonthCode()
//---------------------------------------------------------------------------
wchar_t CTradingContract::GenerateRfaMonthCode(int MaturityMonth,
											   nsOrderMessageDefine::EventTypeEnum CallOrPut)
{
	wchar_t monthCode;
	switch (MaturityMonth)
	{
		case  1:
			if (CallOrPut == nsOrderMessageDefine::evtCall)
				monthCode = L'A';
			else if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'M';
			else
				monthCode = L'F';
			break;
		case  2:
			if (CallOrPut == nsOrderMessageDefine::evtCall)
				monthCode = L'B';
			else if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'N';
			else
				monthCode = L'G';
			break;
		case  3:
			if (CallOrPut == nsOrderMessageDefine::evtCall)
				monthCode = L'C';
			else if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'O';
			else
				monthCode = L'H';
			break;
		case  4:
			if (CallOrPut == nsOrderMessageDefine::evtCall)
				monthCode = L'D';
			else if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'P';
			else
				monthCode = L'J';
			break;
		case  5:
			if (CallOrPut == nsOrderMessageDefine::evtCall)
				monthCode = L'E';
			else if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'Q';
			else
				monthCode = L'K';
			break;
		case  6:
			if (CallOrPut == nsOrderMessageDefine::evtCall)
				monthCode = L'F';
			else if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'R';
			else
				monthCode = L'M';
			break;
		case  7:
			if (CallOrPut == nsOrderMessageDefine::evtCall)
				monthCode = L'G';
			else if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'S';
			else
				monthCode = L'N';
			break;
		case  8:
			if (CallOrPut == nsOrderMessageDefine::evtCall)
				monthCode = L'H';
			else if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'T';
			else
				monthCode = L'Q';
			break;
		case  9:
			if (CallOrPut == nsOrderMessageDefine::evtCall)
				monthCode = L'I';
			else if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'U';
			else
				monthCode = L'U';
			break;
		case 10:
			if (CallOrPut == nsOrderMessageDefine::evtCall)
				monthCode = L'J';
			else if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'V';
			else
				monthCode = L'V';
			break;
		case 11:
			if (CallOrPut == nsOrderMessageDefine::evtCall)
				monthCode = L'K';
			else if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'W';
			else
				monthCode = L'X';
			break;
		case 12:
			if (CallOrPut == nsOrderMessageDefine::evtCall)
				monthCode = L'L';
			else if (CallOrPut == nsOrderMessageDefine::evtPut)
				monthCode = L'X';
			else
				monthCode = L'Z';
			break;
		default:
			monthCode = 0;
	}  //switch (MaturityMonth)
	return monthCode;
}  //CTradingContract::GenerateRfaMonthCode()
//---------------------------------------------------------------------------
String CTradingContract::GenerateTaifexSymbol(const String& ContractCode, double StrikePrice, const String& MaturityYearMonthStr,
											  nsOrderMessageDefine::EventTypeEnum CallOrPut)
{
	String symbol = L"";
	int maturityYear = 0;
	int maturityMonth = 0;
	ParseMaturityYearMonth(MaturityYearMonthStr, maturityYear, maturityMonth);
	if ((maturityYear != 0) && (maturityMonth != 0))
	{
		wchar_t yearCode = GenerateYearCode(maturityYear);
		wchar_t monthCode = GenerateTaifexMonthCode(maturityMonth, CallOrPut);
		if ((yearCode != 0) && (monthCode != 0))
		{
			if ((CallOrPut != nsOrderMessageDefine::evtCall) && (CallOrPut != nsOrderMessageDefine::evtPut))
				symbol.printf(L"%s%c%c", ContractCode, monthCode, yearCode);
			else
				symbol.printf(L"%s%05.0lf%c%c", ContractCode, StrikePrice, monthCode, yearCode);
		}
	}  //if ((MaturityYear != 0) && (MaturityMonth != 0))
	return symbol;
}  //CTradingContract::GenerateTaifexSymbol()
//---------------------------------------------------------------------------
String CTradingContract::GenerateRfaSymbol(const String& ContractCode, double StrikePrice, const String& MaturityYearMonthStr,
										   nsOrderMessageDefine::EventTypeEnum CallOrPut)
{
	String symbol = L"";
	int maturityYear = 0;
	int maturityMonth = 0;
	ParseMaturityYearMonth(MaturityYearMonthStr, maturityYear, maturityMonth);
	if ((maturityYear != 0) && (maturityMonth != 0))
	{
		wchar_t yearCode = GenerateYearCode(maturityYear);
		wchar_t monthCode = GenerateRfaMonthCode(maturityMonth, CallOrPut);
		if ((yearCode != 0) && (monthCode != 0))
		{
			if ((CallOrPut != nsOrderMessageDefine::evtCall) && (CallOrPut != nsOrderMessageDefine::evtPut))
				symbol.printf(L"%s%c%c", ContractCode, monthCode, yearCode);
			else
				symbol.printf(L"%s%.0lf%c%c", ContractCode, StrikePrice, monthCode, yearCode);
		}
	}  //if ((MaturityYear != 0) && (MaturityMonth != 0))
	return symbol;
}  //CTradingContract::GenerateRfaSymbol()
//---------------------------------------------------------------------------
String CTradingContract::GenerateOverseasTradeSymbol(const String& TradeContractCode, const String& StrikePriceStr, const String& MaturityYearMonthStr,
													 nsOrderMessageDefine::EventTypeEnum CallOrPut)
{
	String tradeSymbol = L"";
	int maturityYear = 0;
	int maturityMonth = 0;
	ParseMaturityYearMonth(MaturityYearMonthStr, maturityYear, maturityMonth);
	if ((maturityYear != 0) && (maturityMonth != 0))
	{
		wchar_t yearCode = GenerateYearCode(maturityYear);
		wchar_t monthCode = GenerateTaifexMonthCode(maturityMonth, CallOrPut);
		if ((yearCode != 0) && (monthCode != 0))
		{
			if ((CallOrPut == nsOrderMessageDefine::evtCall))      //KS200H4_C_242.5, 履約價去0
				tradeSymbol.printf(L"%s%c%c_C_%s", TradeContractCode, monthCode, yearCode, StrikePriceStr);
			else if ((CallOrPut == nsOrderMessageDefine::evtPut))  //KS200H4_P_247.5
				tradeSymbol.printf(L"%s%c%c_P_%s", TradeContractCode, monthCode, yearCode, StrikePriceStr);
			else  //期貨維持一樣KS200H4
				tradeSymbol.printf(L"%s%c%c", TradeContractCode, monthCode, yearCode);
		}
	}  //if ((MaturityYear != 0) && (MaturityMonth != 0))
	return tradeSymbol;
}  //CTradingContract::GenerateOverseasTradeSymbol()
//---------------------------------------------------------------------------
bool CTradingContract::ParseTaifexFutureComposedSymbol(const String& Symbol, String& Leg1Symbol, String& Leg2Symbol)
{   //期貨跨月價差商品代號
	bool isComposed = false;
	Leg1Symbol = Symbol;
	Leg2Symbol = L"";
	if (Symbol.Length() == 8)  //長度為 8 碼
	{
		wchar_t symbolDelimiter = Symbol[6];  //第六個字元
		if (symbolDelimiter == L'/')          //為分隔符號 '/'
		{
			Leg1Symbol = Symbol.SubString(1, 5);  //第一個契約商品代號
			String contractCode = Symbol.SubString(1, 3);
			String leg2MonthYearCode = Symbol.SubString(7, 2);  //第二個月份碼年分碼
			Leg2Symbol = contractCode + leg2MonthYearCode;  //第二個契約商品代號
			isComposed = true;
		}
	}  //if (Symbol.Length() == 8)
	return isComposed;
}  //CTradingContract::ParseTaifexFutureComposedSymbol()
//---------------------------------------------------------------------------
bool CTradingContract::ParseTaifexOptionComposedSymbol(const String& Symbol, String& Leg1Symbol, String& Leg2Symbol)
{
	bool isComposed = false;
	Leg1Symbol = Symbol;
	Leg2Symbol = L"";
	int symbolLength = Symbol.Length();
	if (symbolLength >= 3)
	{
		String contractCode = Symbol.SubString(1, 3);
		wchar_t symbolDelimiter;
		if (symbolLength == 13)
		{
			symbolDelimiter = Symbol[11];  //第十一個字元為分隔符號
			if ((symbolDelimiter == L'/') ||  //Time Spread
				(symbolDelimiter == L':') ||  //Straddle
				(symbolDelimiter == L'-'))    //Conversion & Reversals
			{
				Leg1Symbol = Symbol.SubString(1, 10);
				String strikePriceStr = Symbol.SubString(4, 5);
				String leg2MonthYearCode = Symbol.SubString(12, 2);
				Leg2Symbol = contractCode + strikePriceStr + leg2MonthYearCode;
				isComposed = true;
			}
		}
		else if (symbolLength == 16)
		{
			symbolDelimiter = Symbol[9];  //第十一個字元為分隔符號
			if (symbolDelimiter == L'/')  //Price Spread
			{
				String leg1StrikePriceStr = Symbol.SubString(4, 5);
				String leg2StrikePriceStr = Symbol.SubString(10, 5);
				String monthYearCode = Symbol.SubString(15, 2);
				Leg1Symbol = contractCode + leg1StrikePriceStr + monthYearCode;
				Leg2Symbol = contractCode + leg2StrikePriceStr + monthYearCode;
				isComposed = true;
			}
		}
		else if (symbolLength == 18)
		{
			symbolDelimiter = Symbol[11];  //第十一個字元為分隔符號
			if (symbolDelimiter == L':')  //Strangle
			{
				Leg1Symbol = Symbol.SubString(1, 10);
				String leg2StrikePriceStr = Symbol.SubString(12, 5);
				String leg2MonthYearCode = Symbol.SubString(17, 2);
				Leg2Symbol = contractCode + leg2StrikePriceStr + leg2MonthYearCode;
				isComposed = true;
			}
		}  //if (symbolLength == 13)
	}  //if (symbolLength >= 3)
	return isComposed;
}  //CTradingContract::ParseTaifexOptionComposedSymbol()
//---------------------------------------------------------------------------
bool CTradingContract::ParseTaifexComposedSymbol(const String& Symbol, String& Leg1Symbol, String& Leg2Symbol)
{
	bool isComposed = false;
	int symbolLength = Symbol.Length();
	switch (symbolLength)
	{
		case 8:
			isComposed = ParseTaifexFutureComposedSymbol(Symbol, Leg1Symbol, Leg2Symbol);
			break;
		case 13:
		case 16:
		case 18:
			isComposed = ParseTaifexOptionComposedSymbol(Symbol, Leg1Symbol, Leg2Symbol);
			break;
		default:
			Leg1Symbol = Symbol;
			Leg2Symbol = L"";
	}
	return isComposed;
}  //CTradingContract::ParseTaifexComposedSymbol()
//---------------------------------------------------------------------------
void CTradingContract::ParseOverseasTradeSymbol(const String& TradeSymbol, String& TradeContractCode, String& StrikePriceStr, String& MaturityYearMonthStr,
												nsOrderMessageDefine::EventTypeEnum& CallOrPut)
{   //Example Options:KS200H4_C_242.5, Futures:KS200H4
	TradeContractCode = L"";
	StrikePriceStr = L"";
	MaturityYearMonthStr = L"";
	CallOrPut = nsOrderMessageDefine::evtNone;
	TStringList *fieldValueList = 0;
	fieldValueList = new TStringList();
	fieldValueList->Delimiter = L'_';
	fieldValueList->DelimitedText = TradeSymbol;
	int fieldCount = fieldValueList->Count;
	if (fieldCount < 1) return;
	String contractMonthYearStr = (*fieldValueList)[0];
	if (contractMonthYearStr.Length() > 2)
	{
		TradeContractCode = contractMonthYearStr.SubString(1, contractMonthYearStr.Length() - 2);
		MaturityYearMonthStr = contractMonthYearStr.SubString(TradeContractCode.Length() + 1, 2);
	}

	if (fieldCount >= 2)
	{
		String callPutStr = (*fieldValueList)[1];
		if (callPutStr == L"C")
			CallOrPut = nsOrderMessageDefine::evtCall;
		else if (callPutStr == L"P")
			CallOrPut = nsOrderMessageDefine::evtPut;
	}

	if (fieldCount >= 3) StrikePriceStr = (*fieldValueList)[2];

	delete fieldValueList;
}  //ParseOverseasTradeSymbol()
//---------------------------------------------------------------------------
CTradingContract::CTradingContract(const String& ExchangeCode, const String& Symbol,
								   nsOrderMessageDefine::MarketEnum Market,
								   double StrikePrice,
								   nsOrderMessageDefine::EventTypeEnum CallPut,
								   CTradingCommodity *CommodityPtr,
								   CTradingExchange *ExchangePtr)
:CExchangeSymbolKey(constCreateRWLock, ExchangeCode, Symbol, Market, Symbol, Symbol)
,FStrikePrice(StrikePrice)
,FCallPut(CallPut)
,FMaturityYearMonthStr(L"")
,FContractName(L"")
,FFractionPartLength(3)
,FIsComposed(false)
,FLeg1Symbol(Symbol)
,FLeg2Symbol(L"")
,FCommodityPtr(CommodityPtr)
,FExchangePtr(ExchangePtr)
{
	if ((FMarket == nsOrderMessageDefine::mTWFutures) || (FMarket == nsOrderMessageDefine::mTWOptions))
	FIsComposed = ParseTaifexComposedSymbol(FSymbol, FLeg1Symbol, FLeg2Symbol);
}  //CTradingContract::CTradingContract()
//---------------------------------------------------------------------------
void CTradingContract::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	Lock(RWLockType);
	CExchangeSymbolKey::AssignFieldData(SrcObjPtr, rwNotLock);
	CTradingContract *srcContractPtr = static_cast<CTradingContract*>(SrcObjPtr);
	srcContractPtr->LockForRead();
	FStrikePrice = srcContractPtr->GetStrikePrice(rwNotLock);
	FCallPut = srcContractPtr->GetCallPut(rwNotLock);
	FMaturityYearMonthStr = srcContractPtr->GetMaturityYearMonthStr(rwNotLock);
	FContractName = srcContractPtr->GetContractName(rwNotLock);
	FFractionPartLength = srcContractPtr->GetFractionPartLength(rwNotLock);
	FContractMultiplier = srcContractPtr->GetContractMultiplier(rwNotLock);
	FCommodityPtr = srcContractPtr->GetCommodity(rwNotLock);
	FExchangePtr = srcContractPtr->GetExchange(rwNotLock);
	srcContractPtr->UnlockForRead();
	Unlock(RWLockType);
}  //CTradingContract::AssignFieldData()
//---------------------------------------------------------------------------
void CTradingContract::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String IsCheckedStr = L"IsChecked=false";
	String dumpMessage;
	Lock(RWLockType);
	dumpMessage.printf(L"Contract[%s-%s]%s: StrikePx=%15.6lf, %s, MaturityDate[%s], FractionLength[%d] Multiplier=%15.6lf. Trade[%s], Broker[%s], Leg1[%s], Leg2[%s]",
					   FExchangeCode, FSymbol, FContractName, FStrikePrice, DescribeCallPut(FCallPut), FMaturityYearMonthStr, FFractionPartLength, FContractMultiplier, FTradeSymbol, FBrokerSymbol, FLeg1Symbol, FLeg2Symbol);
	Unlock(RWLockType);
	WriteUFCLog(dumpMessage, DoFlushToFile);
}  //CTradingContract::DumpData()

//---------------------------- CHoldPositionData ----------------------------
//---------------------------------------------------------------------------
CHoldPositionData::CHoldPositionData(const String& ExchangeCode, const String& Symbol,
									 nsOrderMessageDefine::SideEnum Side,
									 int HoldQuantity, double MatchPrice,
									 CTradingCommodity *CommodityPtr,
									 CTradingExchange *ExchangePtr)
:CExchangeSymbolKey(constCreateRWLock, ExchangeCode, Symbol, nsOrderMessageDefine::mOTC, Symbol, Symbol)
,FSide(Side)
,FHoldQuantity(HoldQuantity)
,FMatchPrice(MatchPrice)
,FOrderID(L"")
,FTradeDate(L"")
,FIsDayTradeOver(false)
,FCommodityPtr(CommodityPtr)
,FExchangePtr(ExchangePtr)
{
}  //CHoldPositionData::CHoldPositionData()
//---------------------------------------------------------------------------
CHoldPositionData::CHoldPositionData(const String& ExchangeCode, const String& Symbol,
									 nsOrderMessageDefine::MarketEnum Market,
									 nsOrderMessageDefine::SideEnum Side,
									 int HoldQuantity, double MatchPrice,
									 CTradingCommodity *CommodityPtr,
									 CTradingExchange *ExchangePtr)
:CExchangeSymbolKey(constCreateRWLock, ExchangeCode, Symbol, Market, Symbol, Symbol)
,FSide(Side)
,FHoldQuantity(HoldQuantity)
,FMatchPrice(MatchPrice)
,FOrderID(L"")
,FTradeDate(L"")
,FIsDayTradeOver(false)
,FCommodityPtr(CommodityPtr)
,FExchangePtr(ExchangePtr)
{
}  //CHoldPositionData::CHoldPositionData()
//---------------------------------------------------------------------------
void CHoldPositionData::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	Lock(RWLockType);
	CExchangeSymbolKey::AssignFieldData(SrcObjPtr, rwNotLock);
	CHoldPositionData *srcHoldPositionPtr = static_cast<CHoldPositionData*>(SrcObjPtr);
	srcHoldPositionPtr->LockForRead();
	FSide = srcHoldPositionPtr->GetSide(rwNotLock);
	FHoldQuantity = srcHoldPositionPtr->GetHoldQuantity(rwNotLock);
	FMatchPrice = srcHoldPositionPtr->GetMatchPrice(rwNotLock);
	FOrderID = srcHoldPositionPtr->GetOrderID(rwNotLock);
	FTradeDate = srcHoldPositionPtr->GetTradeDate(rwNotLock);
	FIsDayTradeOver = srcHoldPositionPtr->IsDayTradeOver(rwNotLock);
	FCommodityPtr = srcHoldPositionPtr->GetCommodity(rwNotLock);
	FExchangePtr = srcHoldPositionPtr->GetExchange(rwNotLock);
	srcHoldPositionPtr->UnlockForRead();
	Unlock(RWLockType);
}  //CHoldPositionData::AssignFieldData()
//---------------------------------------------------------------------------
void CHoldPositionData::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String dumpMessage;
	Lock(RWLockType);
	String combineStr = L"";
	dumpMessage.printf(L"HoldPosition[%s-%s]: %s Qty[%5d] MatchPX[%15.6lf] TradeDate[%s]. Trade[%s]", FExchangeCode, FSymbol, DescribeBuySell(FSide), FHoldQuantity, FMatchPrice, FTradeDate, FTradeSymbol);
	Unlock(RWLockType);
	WriteUFCLog(dumpMessage, DoFlushToFile);
}  //CHoldPositionData::DumpData()
};  //namespace s888
#pragma package(smart_init)
