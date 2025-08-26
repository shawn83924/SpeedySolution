//---------------------------------------------------------------------------


#pragma hdrstop

#include "iniFile.h"
#include "TradingObjectPool.h"

//---------------------------------------------------------------------------
namespace s888
{
//--------------------------- CTradingObjectsPool ---------------------------
CTradingObjectsPool gTradingObjsPool(L"TradingPool");
CTradingObjectsPool gQueryObjsPool(L"QueryPool");

//---------------------------------------------------------------------------
CTradingObjectsPool::CTradingObjectsPool()
:CTradingBasicObject(constCreateRWLock)
,FTaifexFuturesContractQueryStatus(qsNone)
,FTaifexFuturesContractQueryMessage(L"")
,FTaifexOptionsContractQueryStatus(qsNone)
,FTaifexOptionsContractQueryMessage(L"")
,FOverseasContractQueryStatus(qsNone)
,FOverseasContractQueryMessage(L"")
,FTaifexTaxRateQueryStatus(qsNone)
,FTaifexTaxRateQueryMessage(L"")
{
	FStockTaxRate = new CTaxRateObject( L"TWSE", L"2330", 0.0015 );
}  //CTradingObjectsPool::CTradingObjectsPool()
//---------------------------------------------------------------------------
CTradingObjectsPool::CTradingObjectsPool(const String& PoolName)
:CTradingBasicObject(constCreateRWLock)
,FPoolName(PoolName)
,FTaifexFuturesContractQueryStatus(qsNone)
,FTaifexFuturesContractQueryMessage(L"")
,FTaifexOptionsContractQueryStatus(qsNone)
,FTaifexOptionsContractQueryMessage(L"")
,FOverseasContractQueryStatus(qsNone)
,FOverseasContractQueryMessage(L"")
,FTaifexTaxRateQueryStatus(qsNone)
,FTaifexTaxRateQueryMessage(L"")
{
	FStockTaxRate = new CTaxRateObject( L"TWSE", L"2330", 0.0015 );
}  //CTradingObjectsPool::CTradingObjectsPool()
//---------------------------------------------------------------------------
CTradingObjectsPool::~CTradingObjectsPool()
{
	ClearAllTaxRate(constDelete, rwLockForWrite);
//	ClearAllContract(constDelete, rwLockForWrite);
	ClearAllCommodity(constDelete, rwLockForWrite);
	ClearAllExchange(constDelete, rwLockForWrite);
	ClearTransferBankCodeIndex(constDelete, rwLockForWrite);
	ClearAccountLogonIdIndex(constDelete, rwLockForWrite);
}  //CTradingObjectsPool::~CTradingObjectsPool()
//---------------------------------------------------------------------------
CTradingExchange *CTradingObjectsPool::FindExchangeFromCodeIndex(const String& ExchangeCode, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CTradingExchange*>(FindTradingObjFromMap(FExchangeCodeIndex, ExchangeCode, &FExchangeCSLock, RWLockType));
}  //CTradingObjectsPool::FindExchangeFromCodeIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::InsertExchangeIntoCodeIndex(const String& ExchangeCode, CTradingExchange *ExchangePtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	return InsertTradingObjIntoMap(FExchangeCodeIndex, ExchangeCode, ExchangePtr, DoCheckExist, &FExchangeCSLock, RWLockType);
}  //CTradingObjectsPool::InsertExchangeIntoCodeIndex()
//---------------------------------------------------------------------------
CTradingExchange *CTradingObjectsPool::RemoveExchangeFromCodeIndex(const String& ExchangeCode, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CTradingExchange*>(RemoveTradingObjFromMap(FExchangeCodeIndex, ExchangeCode, &FExchangeCSLock, RWLockType));
}  //CTradingObjectsPool::RemoveExchangeFromCodeIndex()
//---------------------------------------------------------------------------
int CTradingObjectsPool::ClearExchangeCodeIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FExchangeCodeIndex, DoDelete, &FExchangeCSLock, RWLockType);
}  //CTradingObjectsPool::ClearExchangeCodeIndex()
//---------------------------------------------------------------------------
void CTradingObjectsPool::DumpExchangeCodeIndex(ReadWrietLockTypeEnum RWLockType)
{
	String logStr;
	LockExchange(RWLockType);
	logStr.printf(L"===== Dump %s %d Exchanges Code Index =====", FPoolName, FExchangeCodeIndex.size());
	WriteUFCLog(logStr, constNotFlush);
	DumpTradingObjMap(FExchangeCodeIndex, constNotFlush, &FExchangeCSLock, rwNotLock);
	UnlockExchange(RWLockType);
	logStr = L" ";
	WriteUFCLog(logStr, constFlush);
}  //CTradingObjectsPool::DumpExchangeCodeIndex()
//---------------------------------------------------------------------------
CTradingExchange *CTradingObjectsPool::FindExchangeFromBrokerCodeIndex(const String& BrokerExchangeCode, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CTradingExchange*>(FindTradingObjFromMap(FExchangeBrokerCodeIndex, BrokerExchangeCode, &FExchangeCSLock, RWLockType));
}  //CTradingObjectsPool::FindExchangeFromBrokerCodeIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::InsertExchangeIntoBrokerCodeIndex(const String& BrokerExchangeCode, CTradingExchange *ExchangePtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	return InsertTradingObjIntoMap(FExchangeBrokerCodeIndex, BrokerExchangeCode, ExchangePtr, DoCheckExist, &FExchangeCSLock, RWLockType);
}  //CTradingObjectsPool::InsertExchangeIntoBrokerCodeIndex()
//---------------------------------------------------------------------------
CTradingExchange *CTradingObjectsPool::RemoveExchangeFromBrokerCodeIndex(const String& BrokerExchangeCode, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CTradingExchange*>(RemoveTradingObjFromMap(FExchangeBrokerCodeIndex, BrokerExchangeCode, &FExchangeCSLock, RWLockType));
}  //CTradingObjectsPool::RemoveExchangeFromBrokerCodeIndex()
//---------------------------------------------------------------------------
int CTradingObjectsPool::ClearExchangeBrokerCodeIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FExchangeBrokerCodeIndex, DoDelete, &FExchangeCSLock, RWLockType);
}  //CTradingObjectsPool::ClearExchangeBrokerCodeIndex()
//---------------------------------------------------------------------------
void CTradingObjectsPool::DumpExchangeBrokerCodeIndex(ReadWrietLockTypeEnum RWLockType)
{
	String logStr;
	LockExchange(RWLockType);
	logStr.printf(L"===== Dump %s %d Exchanges Broker Code Index =====", FPoolName, FExchangeBrokerCodeIndex.size());
	WriteUFCLog(logStr, constNotFlush);
	DumpTradingObjMap(FExchangeBrokerCodeIndex, constNotFlush, &FExchangeCSLock, rwNotLock);
	UnlockExchange(RWLockType);
	logStr = L" ";
	WriteUFCLog(logStr, constFlush);
}  //CTradingObjectsPool::DumpExchangeBrokerCodeIndex()
//---------------------------------------------------------------------------
String CTradingObjectsPool::ConvertIfBorkerExchangeCode(const String& OriginalExchangeCode, ReadWrietLockTypeEnum RWLockType)
{
	String resultExchangeCode = OriginalExchangeCode;
	LockExchange(RWLockType);
	CTradingExchange *exchangePtr = FindExchangeFromCodeIndex(OriginalExchangeCode, rwNotLock);
	if (exchangePtr == 0)
	{
		exchangePtr = FindExchangeFromBrokerCodeIndex(OriginalExchangeCode, rwNotLock);
		if (exchangePtr != 0)
			resultExchangeCode = exchangePtr->GetExchangeCode(rwLockForRead);
	}
	UnlockExchange(RWLockType);
	return resultExchangeCode;
}  //CTradingObjectsPool::ConvertIfBorkerExchangeCode()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::AddExchange(CTradingExchange *ExchangePtr, ReadWrietLockTypeEnum RWLockType)
{
	bool isSuccess = false;
	if (ExchangePtr == 0) return isSuccess;
	ExchangePtr->LockForRead();
	String exchangeCode = ExchangePtr->GetExchangeCode(rwNotLock);
	String brokerExchangeCode = ExchangePtr->GetBrokerExchangeCode(rwNotLock);
	ExchangePtr->UnlockForRead();

	String logHead = L"CTradingObjectsPool::AddExchange()";
	String logStr = L"";
	LockExchange(RWLockType);
	if (InsertExchangeIntoCodeIndex(exchangeCode, ExchangePtr, constCheckExist, rwNotLock))
	{
		isSuccess = true;
		if (!InsertExchangeIntoBrokerCodeIndex(brokerExchangeCode, ExchangePtr, constCheckExist, rwNotLock))
			logStr.printf(L"Insert Exchange[%s] into %s Broker Code Index falied. broker[%s]", exchangeCode, FPoolName, brokerExchangeCode);
	}
	else
		logStr.printf(L"Insert Exchange[%s] into %s Code Index falied. broker[%s]", exchangeCode, FPoolName, brokerExchangeCode);
	UnlockExchange(RWLockType);
	if (logStr.Length() > 0) WriteUFCLog(logHead, logStr, constFlush);
	return isSuccess;
}  //CTradingObjectsPool::AddExchange()
//---------------------------------------------------------------------------
void CTradingObjectsPool::ClearAllExchange(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	LockExchange(RWLockType);
	ClearExchangeBrokerCodeIndex(constNotDelete, rwNotLock);
	ClearExchangeCodeIndex(DoDelete, rwNotLock);
	UnlockExchange(RWLockType);
}  //CTradingObjectsPool::ClearAllExchange()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::LoadExchangeInformation(const String& InformationFileName)
{
	bool isLoadSuccess = false;
	if (InformationFileName.Length() <= 0) return isLoadSuccess;
	UFC::AnsiString ansiInformationFileName = ConvertUnicodeToUFCAnsiStr(InformationFileName);
	String logHead = FPoolName + L"::LoadExchangeInformation()";
	String logMsg = L"Begin...";
	WriteUFCLog(logHead, logMsg, constNotFlush);
	try
	{
		UFC::UiniFile exchangeInfo(ansiInformationFileName);
		for (int i = 0; i < exchangeInfo.SectionCount(); i++)
		{
			UFC::Section *sectionPtr = exchangeInfo.GetSection(i);
			if (sectionPtr != 0)
			{
				UFC::PStringList sectionNameList;
				sectionNameList.SetStrings(sectionPtr->GetSectionName(), "."); ///< Example: SGX.STW
				UFC::AnsiString ansiExchangeCode = sectionNameList[0];
				String exchangeCode = UTF8ToUnicodeString(ansiExchangeCode);

				UFC::AnsiString ansiFieldValue;
				bool isStopOrderSupported = false;
				if ((sectionPtr->GetValue("StopOrd", ansiFieldValue) == TRUE) &&
					((ansiFieldValue == "Y") || (ansiFieldValue == "y")))
					 isStopOrderSupported = true;

				bool isDayTradeOpenSupported = false;
				if ((sectionPtr->GetValue("DayTrade", ansiFieldValue) == TRUE) &&
					((ansiFieldValue == "Y") || (ansiFieldValue == "y")))
					isDayTradeOpenSupported = true;

				UFC::AnsiString ansiBrokerExchangeCode = sectionNameList[0];
				String brokerExchangeCode = L"";
				if (sectionPtr->GetValue("BrokerExchange", ansiBrokerExchangeCode) == TRUE)
					brokerExchangeCode = UTF8ToUnicodeString(ansiBrokerExchangeCode);
				if (brokerExchangeCode.Length() <= 0)
					brokerExchangeCode = s888::CTradingExchange::ConvertExchangeCodeToBrokerCode(exchangeCode);
				String exchangeName = L"";

				LockExchange(rwLockForWrite);
				CTradingExchange *exchangePtr = FindExchangeFromCodeIndex(exchangeCode, rwNotLock);
				if (exchangePtr == 0)
				{
					exchangePtr = new CTradingExchange(exchangeCode, exchangeName, isDayTradeOpenSupported, isStopOrderSupported, brokerExchangeCode);
					if (!AddExchange(exchangePtr, rwNotLock))
					{
						logMsg.printf(L"Add Exchange[%s] falied. Broker[%s]", exchangeCode, brokerExchangeCode);
						WriteUFCLog(logHead, logMsg, constNotFlush);
						delete exchangePtr;
						exchangePtr = 0;
					}
				}
				UnlockExchange(rwLockForWrite);
			}  //if (sectionPtr != 0)
		}  //for (int i = 0; i < exchangeInfo.SectionCount(); i++)
		isLoadSuccess = true;
	} catch(UFC::FileException e) {
		String exceptionStr = UTF8ToUnicodeString(e.what());
		logMsg.printf(L"FileException:[%s]", exceptionStr);
		WriteUFCLog(logHead, logMsg, constNotFlush);
	}
	logMsg = L"End...";
	WriteUFCLog(logHead, logMsg, constNotFlush);
	FlushUFCLog();
	return isLoadSuccess;
}  //CTradingObjectsPool::LoadExchangeInformation()
//---------------------------------------------------------------------------
CTradingCommodity *CTradingObjectsPool::FindCommodityFromCodeIndex(const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingCommodity::BuildKey(ExchangeCode, ContractCode);
	return static_cast<CTradingCommodity*>(FindTradingObjFromMap(FCommodityCodeIndex, keyValue, &FCommodityCSLock, RWLockType));
}  //CTradingObjectsPool::FindCommodityFromCodeIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::InsertCommodityIntoCodeIndex(const String& ExchangeCode, const String& ContractCode, CTradingCommodity *CommodityPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingCommodity::BuildKey(ExchangeCode, ContractCode);
	return InsertTradingObjIntoMap(FCommodityCodeIndex, keyValue, CommodityPtr, DoCheckExist, &FCommodityCSLock, RWLockType);
}  //CTradingObjectsPool::InsertCommodityIntoCodeIndex()
//---------------------------------------------------------------------------
CTradingCommodity *CTradingObjectsPool::RemoveCommodityFromCodeIndex(const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingCommodity::BuildKey(ExchangeCode, ContractCode);
	return static_cast<CTradingCommodity*>(RemoveTradingObjFromMap(FCommodityCodeIndex, keyValue, &FCommodityCSLock, RWLockType));
}  //CTradingObjectsPool::RemoveCommodityFromCodeIndex()
//---------------------------------------------------------------------------
int CTradingObjectsPool::ClearCommodityCodeIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FCommodityCodeIndex, DoDelete, &FCommodityCSLock, RWLockType);
}  //CTradingObjectsPool::ClearCommodityCodeIndex()
//---------------------------------------------------------------------------
void CTradingObjectsPool::DumpCommodityCodeIndex(ReadWrietLockTypeEnum RWLockType)
{
	String logStr;
	LockCommodity(RWLockType);
	logStr.printf(L"===== Dump %s %d Commodity Code Index =====", FPoolName, FCommodityCodeIndex.size());
	WriteUFCLog(logStr, constNotFlush);
	DumpTradingObjMap(FCommodityCodeIndex, constNotFlush, &FCommodityCSLock, rwNotLock);
	UnlockCommodity(RWLockType);
	logStr = L" ";
	WriteUFCLog(logStr, constFlush);
}  //CTradingObjectsPool::DumpCommodityCodeIndex()
//---------------------------------------------------------------------------
CTradingCommodity *CTradingObjectsPool::FindCommodityFromTradeCodeIndex(const String& ExchangeCode, const String& TradeContractCode, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingCommodity::BuildKey(ExchangeCode, TradeContractCode);
	return static_cast<CTradingCommodity*>(FindTradingObjFromMap(FCommodityTradeCodeIndex, keyValue, &FCommodityCSLock, RWLockType));
}  //CTradingObjectsPool::FindCommodityFromTradeCodeIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::InsertCommodityIntoTradeCodeIndex(const String& ExchangeCode, const String& TradeContractCode, CTradingCommodity *CommodityPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingCommodity::BuildKey(ExchangeCode, TradeContractCode);
	return InsertTradingObjIntoMap(FCommodityTradeCodeIndex, keyValue, CommodityPtr, DoCheckExist, &FCommodityCSLock, RWLockType);
}  //CTradingObjectsPool::InsertCommodityIntoTradeCodeIndex()
//---------------------------------------------------------------------------
CTradingCommodity *CTradingObjectsPool::RemoveCommodityFromTradeCodeIndex(const String& ExchangeCode, const String& TradeContractCode, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingCommodity::BuildKey(ExchangeCode, TradeContractCode);
	return static_cast<CTradingCommodity*>(RemoveTradingObjFromMap(FCommodityTradeCodeIndex, keyValue, &FCommodityCSLock, RWLockType));
}  //CTradingObjectsPool::RemoveCommodityFromTradeCodeIndex()
//---------------------------------------------------------------------------
int CTradingObjectsPool::ClearCommodityTradeCodeIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FCommodityTradeCodeIndex, DoDelete, &FCommodityCSLock, RWLockType);
}  //CTradingObjectsPool::ClearCommodityTradeCodeIndex()
//---------------------------------------------------------------------------
void CTradingObjectsPool::DumpCommodityTradeCodeIndex(ReadWrietLockTypeEnum RWLockType)
{
	String logStr;
	LockCommodity(RWLockType);
	logStr.printf(L"===== Dump %s %d Commodity Trade Code Index =====", FPoolName, FCommodityTradeCodeIndex.size());
	WriteUFCLog(logStr, constNotFlush);
	DumpTradingObjMap(FCommodityTradeCodeIndex, constNotFlush, &FCommodityCSLock, rwNotLock);
	UnlockCommodity(RWLockType);
	logStr = L" ";
	WriteUFCLog(logStr, constFlush);
}  //CTradingObjectsPool::DumpCommodityTradeCodeIndex()
//---------------------------------------------------------------------------
CTradingCommodity *CTradingObjectsPool::FindCommodityFromBrokerCodeIndex(const String& ExchangeCode, const String& BrokerContractCode, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingCommodity::BuildKey(ExchangeCode, BrokerContractCode);
	return static_cast<CTradingCommodity*>(FindTradingObjFromMap(FCommodityBrokerCodeIndex, keyValue, &FCommodityCSLock, RWLockType));
}  //CTradingObjectsPool::FindCommodityFromBrokerCodeIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::InsertCommodityIntoBrokerCodeIndex(const String& ExchangeCode, const String& BrokerContractCode, CTradingCommodity *CommodityPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingCommodity::BuildKey(ExchangeCode, BrokerContractCode);
	return InsertTradingObjIntoMap(FCommodityBrokerCodeIndex, keyValue, CommodityPtr, DoCheckExist, &FCommodityCSLock, RWLockType);
}  //CTradingObjectsPool::InsertCommodityIntoBrokerCodeIndex()
//---------------------------------------------------------------------------
CTradingCommodity *CTradingObjectsPool::RemoveCommodityFromBrokerCodeIndex(const String& ExchangeCode, const String& BrokerContractCode, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingCommodity::BuildKey(ExchangeCode, BrokerContractCode);
	return static_cast<CTradingCommodity*>(RemoveTradingObjFromMap(FCommodityBrokerCodeIndex, keyValue, &FCommodityCSLock, RWLockType));
}  //CTradingObjectsPool::RemoveCommodityFromBrokerCodeIndex()
//---------------------------------------------------------------------------
int CTradingObjectsPool::ClearCommodityBrokerCodeIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FCommodityBrokerCodeIndex, DoDelete, &FCommodityCSLock, RWLockType);
}  //CTradingObjectsPool::ClearCommodityBrokerCodeIndex()
//---------------------------------------------------------------------------
void CTradingObjectsPool::DumpCommodityBrokerCodeIndex(ReadWrietLockTypeEnum RWLockType)
{
	String logStr;
	LockCommodity(RWLockType);
	logStr.printf(L"===== Dump %s %d Commodity Broker Code Index =====", FPoolName, FCommodityBrokerCodeIndex.size());
	WriteUFCLog(logStr, constNotFlush);
	DumpTradingObjMap(FCommodityBrokerCodeIndex, constNotFlush, &FCommodityCSLock, rwNotLock);
	UnlockCommodity(RWLockType);
	logStr = L" ";
	WriteUFCLog(logStr, constFlush);
}  //CTradingObjectsPool::DumpCommodityBrokerCodeIndex()
//---------------------------------------------------------------------------
CTradingCommodity *CTradingObjectsPool::FindCommodityFromMarketCodeIndex(nsOrderMessageDefine::MarketEnum Market, const String& ContractCode, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingCommodity::BuildKey(Market, ContractCode);
	return static_cast<CTradingCommodity*>(FindTradingObjFromMap(FCommodityMarketCodeIndex, keyValue, &FCommodityCSLock, RWLockType));
}  //CTradingObjectsPool::FindCommodityFromMarketCodeIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::InsertCommodityIntoMarketCodeIndex(nsOrderMessageDefine::MarketEnum Market, const String& ContractCode, CTradingCommodity *CommodityPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingCommodity::BuildKey(Market, ContractCode);
	return InsertTradingObjIntoMap(FCommodityMarketCodeIndex, keyValue, CommodityPtr, DoCheckExist, &FCommodityCSLock, RWLockType);
}  //CTradingObjectsPool::InsertCommodityIntoMarketCodeIndex()
//---------------------------------------------------------------------------
CTradingCommodity *CTradingObjectsPool::RemoveCommodityFromMarketCodeIndex(nsOrderMessageDefine::MarketEnum Market, const String& ContractCode, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingCommodity::BuildKey(Market, ContractCode);
	return static_cast<CTradingCommodity*>(RemoveTradingObjFromMap(FCommodityMarketCodeIndex, keyValue, &FCommodityCSLock, RWLockType));
}  //CTradingObjectsPool::RemoveCommodityFromMarketCodeIndex()
//---------------------------------------------------------------------------
int CTradingObjectsPool::ClearCommodityMarketCodeIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FCommodityMarketCodeIndex, DoDelete, &FCommodityCSLock, RWLockType);
}  //CTradingObjectsPool::ClearCommodityMarketCodeIndex()
//---------------------------------------------------------------------------
void CTradingObjectsPool::DumpCommodityMarketCodeIndex(ReadWrietLockTypeEnum RWLockType)
{
	String logStr;
	LockCommodity(RWLockType);
	logStr.printf(L"===== Dump %s %d Commodity Market Code Index =====", FPoolName, FCommodityMarketCodeIndex.size());
	WriteUFCLog(logStr, constNotFlush);
	DumpTradingObjMap(FCommodityMarketCodeIndex, constNotFlush, &FCommodityCSLock, rwNotLock);
	UnlockCommodity(RWLockType);
	logStr = L" ";
	WriteUFCLog(logStr, constFlush);
}  //CTradingObjectsPool::DumpCommodityMarketCodeIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::AddCommodity(CTradingCommodity *CommodityPtr, ReadWrietLockTypeEnum RWLockType)
{
	bool isSuccess = false;
	if (CommodityPtr == 0) return isSuccess;
	CommodityPtr->LockForRead();
	String exchangeCode = CommodityPtr->GetExchangeCode(rwNotLock);
	String contractCode = CommodityPtr->GetContractCode(rwNotLock);
	String tradeContractCode = CommodityPtr->GetTradeContractCode(rwNotLock);
	String brokerContractCode = CommodityPtr->GetBrokerContractCode(rwNotLock);
	nsOrderMessageDefine::MarketEnum market = CommodityPtr->GetMarket(rwNotLock);
	CommodityPtr->UnlockForRead();

	String logHead = L"CTradingObjectsPool::AddCommodity()";
	String logStr;
	LockCommodity(RWLockType);
	if (InsertCommodityIntoCodeIndex(exchangeCode, contractCode, CommodityPtr, constCheckExist, rwNotLock))
	{
		isSuccess = true;
		if (!InsertCommodityIntoTradeCodeIndex(exchangeCode, tradeContractCode, CommodityPtr, constCheckExist, rwNotLock))
		{
			logStr.printf(L"Insert Commodity[%s-%s] into %s Trade Contract Code Index falied. trade[%s] broker[%s]", exchangeCode, contractCode, FPoolName, tradeContractCode, brokerContractCode);
			WriteUFCLog(logHead, logStr, constFlush);
		}

		if (!InsertCommodityIntoBrokerCodeIndex(exchangeCode, brokerContractCode, CommodityPtr, constCheckExist, rwNotLock))
		{
			logStr.printf(L"Insert Commodity[%s-%s] into %s Broker Contract Code Index falied. trade[%s] broker[%s]", exchangeCode, contractCode, FPoolName, tradeContractCode, brokerContractCode);
			WriteUFCLog(logHead, logStr, constFlush);
		}

		if (!InsertCommodityIntoMarketCodeIndex(market, contractCode, CommodityPtr, constCheckExist, rwNotLock))
		{
			logStr.printf(L"Insert Commodity[%s-%s] into %s Market Contract Code Index falied. trade[%s] broker[%s] Market[%s]", exchangeCode, contractCode, FPoolName, tradeContractCode, brokerContractCode, DescribeMarket(market));
			WriteUFCLog(logHead, logStr, constFlush);
		}
	}
	else
	{
		logStr.printf(L"Insert Commodity[%s-%s] into %s Contract Code Index falied. trade[%s] broker[%s]", exchangeCode, contractCode, FPoolName, tradeContractCode, brokerContractCode);
		WriteUFCLog(logHead, logStr, constFlush);
	}
	UnlockCommodity(RWLockType);
	return isSuccess;
}  //CTradingObjectsPool::AddCommodity()
//---------------------------------------------------------------------------
void CTradingObjectsPool::ClearAllCommodity(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	LockCommodity(RWLockType);
	ClearCommodityMarketCodeIndex(constNotDelete, rwNotLock);
	ClearCommodityBrokerCodeIndex(constNotDelete, rwNotLock);
	ClearCommodityTradeCodeIndex(constNotDelete, rwNotLock);
	ClearCommodityCodeIndex(DoDelete, rwNotLock);
	UnlockCommodity(RWLockType);
}  //CTradingObjectsPool::ClearAllCommodity()
//---------------------------------------------------------------------------
CTaxRateObject *CTradingObjectsPool::FindTaxRateFromCodeIndex(const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType)
{
	if( ExchangeCode == L"TWSE" || ExchangeCode == L"OTC" )
	   return FStockTaxRate;
	String keyValue = CTaxRateObject::BuildKey(ExchangeCode, ContractCode);
	return static_cast<CTaxRateObject*>(FindTradingObjFromMap(FTaxRateCodeIndex, keyValue, &FTaxRateCSLock, RWLockType));
}  //CTradingObjectsPool::FindTaxRateFromCodeIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::InsertTaxRateIntoCodeIndex(const String& ExchangeCode, const String& ContractCode, CTaxRateObject *TaxRatePtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTaxRateObject::BuildKey(ExchangeCode, ContractCode);
	return InsertTradingObjIntoMap(FTaxRateCodeIndex, keyValue, TaxRatePtr, DoCheckExist, &FTaxRateCSLock, RWLockType);
}  //CTradingObjectsPool::InsertTaxRateIntoCodeIndex()
//---------------------------------------------------------------------------
CTaxRateObject *CTradingObjectsPool::RemoveTaxRateFromCodeIndex(const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTaxRateObject::BuildKey(ExchangeCode, ContractCode);
	return static_cast<CTaxRateObject*>(RemoveTradingObjFromMap(FTaxRateCodeIndex, keyValue, &FTaxRateCSLock, RWLockType));
}  //CTradingObjectsPool::RemoveTaxRateFromCodeIndex()
//---------------------------------------------------------------------------
int CTradingObjectsPool::ClearTaxRateCodeIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FTaxRateCodeIndex, DoDelete, &FTaxRateCSLock, RWLockType);
}  //CTradingObjectsPool::ClearTaxRateCodeIndex()
//---------------------------------------------------------------------------
void CTradingObjectsPool::DumpTaxRateCodeIndex(ReadWrietLockTypeEnum RWLockType)
{
	String logStr;
	LockTaxRate(RWLockType);
	logStr.printf(L"===== Dump %s %d Tax Rate Code Index =====", FPoolName, FTaxRateCodeIndex.size());
	WriteUFCLog(logStr, constNotFlush);
	DumpTradingObjMap(FTaxRateCodeIndex, constNotFlush, &FTaxRateCSLock, rwNotLock);
	UnlockTaxRate(RWLockType);
	logStr = L" ";
	WriteUFCLog(logStr, constFlush);
}  //CTradingObjectsPool::DumpTaxRateCodeIndex()
//---------------------------------------------------------------------------
CTaxRateObject *CTradingObjectsPool::FindTaxRateFromBrokerCodeIndex(const String& ExchangeCode, const String& BrokerContractCode, ReadWrietLockTypeEnum RWLockType)
{
	if( ExchangeCode == L"TWSE" || ExchangeCode == L"OTC" )
	   return FStockTaxRate;
	String keyValue = CTaxRateObject::BuildKey(ExchangeCode, BrokerContractCode);
	return static_cast<CTaxRateObject*>(FindTradingObjFromMap(FTaxRateBrokerCodeIndex, keyValue, &FTaxRateCSLock, RWLockType));
}  //CTradingObjectsPool::FindTaxRateFromBrokerCodeIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::InsertTaxRateIntoBrokerCodeIndex(const String& ExchangeCode, const String& BrokerContractCode, CTaxRateObject *TaxRatePtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTaxRateObject::BuildKey(ExchangeCode, BrokerContractCode);
	return InsertTradingObjIntoMap(FTaxRateBrokerCodeIndex, keyValue, TaxRatePtr, DoCheckExist, &FTaxRateCSLock, RWLockType);
}  //CTradingObjectsPool::InsertTaxRateIntoBrokerCodeIndex()
//---------------------------------------------------------------------------
CTaxRateObject *CTradingObjectsPool::RemoveTaxRateFromBrokerCodeIndex(const String& ExchangeCode, const String& BrokerContractCode, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTaxRateObject::BuildKey(ExchangeCode, BrokerContractCode);
	return static_cast<CTaxRateObject*>(RemoveTradingObjFromMap(FTaxRateBrokerCodeIndex, keyValue, &FTaxRateCSLock, RWLockType));
}  //CTradingObjectsPool::RemoveTaxRateFromBrokerCodeIndex()
//---------------------------------------------------------------------------
int CTradingObjectsPool::ClearTaxRateBrokerCodeIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FTaxRateBrokerCodeIndex, DoDelete, &FTaxRateCSLock, RWLockType);
}  //CTradingObjectsPool::ClearTaxRateBrokerCodeIndex()
//---------------------------------------------------------------------------
void CTradingObjectsPool::DumpTaxRateBrokerCodeIndex(ReadWrietLockTypeEnum RWLockType)
{
	String logStr;
	LockTaxRate(RWLockType);
	logStr.printf(L"===== Dump %s %d Tax Rate Broker Code Index =====", FPoolName, FTaxRateBrokerCodeIndex.size());
	WriteUFCLog(logStr, constNotFlush);
	DumpTradingObjMap(FTaxRateBrokerCodeIndex, constNotFlush, &FTaxRateCSLock, rwNotLock);
	UnlockTaxRate(RWLockType);
	logStr = L" ";
	WriteUFCLog(logStr, constFlush);
}  //CTradingObjectsPool::DumpTaxRateBrokerCodeIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::AddTaxRate(CTaxRateObject *TaxRatePtr, ReadWrietLockTypeEnum RWLockType)
{
	bool isSuccess = false;
	if (TaxRatePtr == 0) return isSuccess;
	TaxRatePtr->LockForRead();
	String exchangeCode = TaxRatePtr->GetExchangeCode(rwNotLock);
	String contractCode = TaxRatePtr->GetContractCode(rwNotLock);
	String brokerContractCode = TaxRatePtr->GetBrokerContractCode(rwNotLock);
	TaxRatePtr->UnlockForRead();

	String logHead = L"CTradingObjectsPool::AddTaxRate()";
	String logStr;
	LockTaxRate(RWLockType);
	if (InsertTaxRateIntoCodeIndex(exchangeCode, contractCode, TaxRatePtr, constCheckExist, rwNotLock))
	{
		isSuccess = true;
		if (!InsertTaxRateIntoBrokerCodeIndex(exchangeCode, brokerContractCode, TaxRatePtr, constCheckExist, rwNotLock))
		{
			logStr.printf(L"Insert TaxRate[%s-%s] into %s Broker Contract Code Index falied. broker[%s]", exchangeCode, contractCode, FPoolName, brokerContractCode);
			WriteUFCLog(logHead, logStr, constFlush);
		}
	}
	else
	{
		logStr.printf(L"Insert TaxRate[%s-%s] into %s Contract Code Index falied. broker[%s]", exchangeCode, contractCode, FPoolName, brokerContractCode);
		WriteUFCLog(logHead, logStr, constFlush);
	}
	UnlockTaxRate(RWLockType);
	return isSuccess;
}  //CTradingObjectsPool::AddTaxRate()
//---------------------------------------------------------------------------
void CTradingObjectsPool::ClearAllTaxRate(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	LockTaxRate(RWLockType);
	ClearTaxRateBrokerCodeIndex(constNotDelete, rwNotLock);
	ClearTaxRateCodeIndex(DoDelete, rwNotLock);
	UnlockTaxRate(RWLockType);
}  //CTradingObjectsPool::ClearAllTaxRate()
//---------------------------------------------------------------------------
CTradingContract *CTradingObjectsPool::FindContractFromSymbolIndex(const String& ExchangeCode, const String& Symbol, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingContract::BuildKey(ExchangeCode, Symbol);
	return static_cast<CTradingContract*>(FindTradingObjFromMap(FContractSymbolIndex, keyValue, &FContractCSLock, RWLockType));
}  //CTradingObjectsPool::FindContractFromSymbolIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::InsertContractIntoSymbolIndex(const String& ExchangeCode, const String& Symbol, CTradingContract *ContractPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingContract::BuildKey(ExchangeCode, Symbol);
	return InsertTradingObjIntoMap(FContractSymbolIndex, keyValue, ContractPtr, DoCheckExist, &FContractCSLock, RWLockType);
}  //CTradingObjectsPool::InsertContractIntoSymbolIndex()
//---------------------------------------------------------------------------
CTradingContract *CTradingObjectsPool::RemoveContractFromSymbolIndex(const String& ExchangeCode, const String& Symbol, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingContract::BuildKey(ExchangeCode, Symbol);
	return static_cast<CTradingContract*>(RemoveTradingObjFromMap(FContractSymbolIndex, keyValue, &FContractCSLock, RWLockType));
}  //CTradingObjectsPool::RemoveContractFromSymbolIndex()
//---------------------------------------------------------------------------
int CTradingObjectsPool::ClearContractSymbolIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FContractSymbolIndex, DoDelete, &FContractCSLock, RWLockType);
}  //CTradingObjectsPool::ClearContractSymbolIndex()
//---------------------------------------------------------------------------
void CTradingObjectsPool::DumpContractSymbolIndex(ReadWrietLockTypeEnum RWLockType)
{
	String logStr;
	LockContract(RWLockType);
	logStr.printf(L"===== Dump %s %d Contract Symbol Index =====", FPoolName, FContractSymbolIndex.size());
	WriteUFCLog(logStr, constNotFlush);
	DumpTradingObjMap(FContractSymbolIndex, constNotFlush, &FContractCSLock, rwNotLock);
	UnlockContract(RWLockType);
	logStr = L" ";
	WriteUFCLog(logStr, constFlush);
}  //CTradingObjectsPool::DumpContractSymbolIndex()
//---------------------------------------------------------------------------
CTradingContract *CTradingObjectsPool::FindContractFromTradeSymbolIndex(const String& ExchangeCode, const String& TradeSymbol, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingContract::BuildKey(ExchangeCode, TradeSymbol);
	return static_cast<CTradingContract*>(FindTradingObjFromMap(FContractTradeSymbolIndex, keyValue, &FContractCSLock, RWLockType));
}  //CTradingObjectsPool::FindContractFromTradeSymbolIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::InsertContractIntoTradeSymbolIndex(const String& ExchangeCode, const String& TradeSymbol, CTradingContract *ContractPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingContract::BuildKey(ExchangeCode, TradeSymbol);
	return InsertTradingObjIntoMap(FContractTradeSymbolIndex, keyValue, ContractPtr, DoCheckExist, &FContractCSLock, RWLockType);
}  //CTradingObjectsPool::InsertContractIntoTradeSymbolIndex()
//---------------------------------------------------------------------------
CTradingContract *CTradingObjectsPool::RemoveContractFromTradeSymbolIndex(const String& ExchangeCode, const String& TradeSymbol, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingContract::BuildKey(ExchangeCode, TradeSymbol);
	return static_cast<CTradingContract*>(RemoveTradingObjFromMap(FContractTradeSymbolIndex, keyValue, &FContractCSLock, RWLockType));
}  //CTradingObjectsPool::RemoveContractFromTradeSymbolIndex()
//---------------------------------------------------------------------------
int CTradingObjectsPool::ClearContractTradeSymbolIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FContractTradeSymbolIndex, DoDelete, &FContractCSLock, RWLockType);
}  //CTradingObjectsPool::ClearContractTradeSymbolIndex()
//---------------------------------------------------------------------------
void CTradingObjectsPool::DumpContractTradeSymbolIndex(ReadWrietLockTypeEnum RWLockType)
{
	String logStr;
	LockContract(RWLockType);
	logStr.printf(L"===== Dump %s %d Contract Trade Symbol Index =====", FPoolName, FContractTradeSymbolIndex.size());
	WriteUFCLog(logStr, constNotFlush);
	DumpTradingObjMap(FContractTradeSymbolIndex, constNotFlush, &FContractCSLock, rwNotLock);
	UnlockContract(RWLockType);
	logStr = L" ";
	WriteUFCLog(logStr, constFlush);
}  //CTradingObjectsPool::DumpContractTradeSymbolIndex()
//---------------------------------------------------------------------------
CTradingContract *CTradingObjectsPool::FindContractFromBrokerSymbolIndex(const String& ExchangeCode, const String& BrokerSymbol, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingContract::BuildKey(ExchangeCode, BrokerSymbol);
	return static_cast<CTradingContract*>(FindTradingObjFromMap(FContractBrokerSymbolIndex, keyValue, &FContractCSLock, RWLockType));
}  //CTradingObjectsPool::FindContractFromBrokerSymbolIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::InsertContractIntoBrokerSymbolIndex(const String& ExchangeCode, const String& BrokerSymbol, CTradingContract *ContractPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingContract::BuildKey(ExchangeCode, BrokerSymbol);
	return InsertTradingObjIntoMap(FContractBrokerSymbolIndex, keyValue, ContractPtr, DoCheckExist, &FContractCSLock, RWLockType);
}  //CTradingObjectsPool::InsertContractIntoBrokerSymbolIndex()
//---------------------------------------------------------------------------
CTradingContract *CTradingObjectsPool::RemoveContractFromBrokerSymbolIndex(const String& ExchangeCode, const String& BrokerSymbol, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingContract::BuildKey(ExchangeCode, BrokerSymbol);
	return static_cast<CTradingContract*>(RemoveTradingObjFromMap(FContractBrokerSymbolIndex, keyValue, &FContractCSLock, RWLockType));
}  //CTradingObjectsPool::RemoveContractFromBrokerSymbolIndex()
//---------------------------------------------------------------------------
int CTradingObjectsPool::ClearContractBrokerSymbolIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FContractBrokerSymbolIndex, DoDelete, &FContractCSLock, RWLockType);
}  //CTradingObjectsPool::ClearContractBrokerSymbolIndex()
//---------------------------------------------------------------------------
void CTradingObjectsPool::DumpContractBrokerSymbolIndex(ReadWrietLockTypeEnum RWLockType)
{
	String logStr;
	LockContract(RWLockType);
	logStr.printf(L"===== Dump %s %d Contract Broker Symbol Index =====", FPoolName, FContractBrokerSymbolIndex.size());
	WriteUFCLog(logStr, constNotFlush);
	DumpTradingObjMap(FContractBrokerSymbolIndex, constNotFlush, &FContractCSLock, rwNotLock);
	UnlockContract(RWLockType);
	logStr = L" ";
	WriteUFCLog(logStr, constFlush);
}  //CTradingObjectsPool::DumpContractBrokerSymbolIndex()
//---------------------------------------------------------------------------
CTradingContract *CTradingObjectsPool::FindContractFromMarketSymbolIndex(nsOrderMessageDefine::MarketEnum Market, const String& Symbol, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingContract::BuildKey(Market, Symbol);
	return static_cast<CTradingContract*>(FindTradingObjFromMap(FContractMarketSymbolIndex, keyValue, &FContractCSLock, RWLockType));
}  //CTradingObjectsPool::FindContractFromMarketSymbolIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::InsertContractIntoMarketSymbolIndex(nsOrderMessageDefine::MarketEnum Market, const String& Symbol, CTradingContract *ContractPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingContract::BuildKey(Market, Symbol);
	return InsertTradingObjIntoMap(FContractMarketSymbolIndex, keyValue, ContractPtr, DoCheckExist, &FContractCSLock, RWLockType);
}  //CTradingObjectsPool::InsertContractIntoMarketSymbolIndex()
//---------------------------------------------------------------------------
CTradingContract *CTradingObjectsPool::RemoveContractFromMarketSymbolIndex(nsOrderMessageDefine::MarketEnum Market, const String& Symbol, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CTradingContract::BuildKey(Market, Symbol);
	return static_cast<CTradingContract*>(RemoveTradingObjFromMap(FContractMarketSymbolIndex, keyValue, &FContractCSLock, RWLockType));
}  //CTradingObjectsPool::RemoveContractFromMarketSymbolIndex()
//---------------------------------------------------------------------------
int CTradingObjectsPool::ClearContractMarketSymbolIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FContractMarketSymbolIndex, DoDelete, &FContractCSLock, RWLockType);
}  //CTradingObjectsPool::ClearContractMarketSymbolIndex()
//---------------------------------------------------------------------------
void CTradingObjectsPool::DumpContractMarketSymbolIndex(ReadWrietLockTypeEnum RWLockType)
{
	String logStr;
	LockContract(RWLockType);
	logStr.printf(L"===== Dump %s %d Contract Market Symbol Index =====", FPoolName, FContractMarketSymbolIndex.size());
	WriteUFCLog(logStr, constNotFlush);
	DumpTradingObjMap(FContractMarketSymbolIndex, constNotFlush, &FContractCSLock, rwNotLock);
	UnlockContract(RWLockType);
	logStr = L" ";
	WriteUFCLog(logStr, constFlush);
}  //CTradingObjectsPool::DumpContractMarketSymbolIndex()
//---------------------------------------------------------------------------
CTradingContract *CTradingObjectsPool::FindContractBySymbol(const String& ExchangeCode, const String& OrigSymbol, bool CheckSymbolIndexFirst, ReadWrietLockTypeEnum RWLockType)
{
	CTradingContract *contractPtr = 0;
	LockContract(RWLockType);
	if (CheckSymbolIndexFirst)
	{
		contractPtr = FindContractFromSymbolIndex(ExchangeCode, OrigSymbol, rwNotLock);
		if (contractPtr == 0) contractPtr = FindContractFromTradeSymbolIndex(ExchangeCode, OrigSymbol, rwNotLock);
	}
	else
	{
		contractPtr = FindContractFromTradeSymbolIndex(ExchangeCode, OrigSymbol, rwNotLock);
		if (contractPtr == 0) contractPtr = FindContractFromSymbolIndex(ExchangeCode, OrigSymbol, rwNotLock);
	}
	if (contractPtr == 0)
		contractPtr = FindContractFromBrokerSymbolIndex(ExchangeCode, OrigSymbol, rwNotLock);
	UnlockContract(RWLockType);
	return contractPtr;
}  //CTradingObjectsPool::FindContractBySymbol()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::AddContract(CTradingContract *ContractPtr, ReadWrietLockTypeEnum RWLockType)
{
	bool isSuccess = false;
	if (ContractPtr == 0) return isSuccess;
	ContractPtr->LockForRead();
	String exchangeCode = ContractPtr->GetExchangeCode(rwNotLock);
	String symbol = ContractPtr->GetSymbol(rwNotLock);
	String tradeSymbol = ContractPtr->GetTradeSymbol(rwNotLock);
	String brokerSymbol = ContractPtr->GetBrokerSymbol(rwNotLock);
	nsOrderMessageDefine::MarketEnum market = ContractPtr->GetMarket(rwNotLock);
	ContractPtr->UnlockForRead();

	String logHead = L"CTradingObjectsPool::AddContract()";
	String logStr;
	LockContract(RWLockType);
	if (InsertContractIntoSymbolIndex(exchangeCode, symbol, ContractPtr, constCheckExist, rwNotLock))
	{
		isSuccess = true;
		if (!InsertContractIntoTradeSymbolIndex(exchangeCode, tradeSymbol, ContractPtr, constCheckExist, rwNotLock))
		{
			logStr.printf(L"Insert Contract[%s-%s] into %s Trade Symbol Index falied. trade[%s] broker[%s]", exchangeCode, symbol, FPoolName, tradeSymbol, brokerSymbol);
			WriteUFCLog(logHead, logStr, constFlush);
		}

		if (!InsertContractIntoBrokerSymbolIndex(exchangeCode, brokerSymbol, ContractPtr, constCheckExist, rwNotLock))
		{
			logStr.printf(L"Insert Contract[%s-%s] into %s Broker Symbol Index falied. trade[%s] broker[%s]", exchangeCode, symbol, FPoolName, tradeSymbol, brokerSymbol);
			WriteUFCLog(logHead, logStr, constFlush);
		}

		if (!InsertContractIntoMarketSymbolIndex(market, symbol, ContractPtr, constCheckExist, rwNotLock))
		{
			logStr.printf(L"Insert Contract[%s-%s] into %s Market Symbol Index falied. trade[%s] broker[%s] Market[%s]", exchangeCode, symbol, FPoolName, tradeSymbol, brokerSymbol, DescribeMarket(market));
			WriteUFCLog(logHead, logStr, constFlush);
		}
	}
	else
	{
		logStr.printf(L"Insert Contract[%s-%s] into %s Symbol Index falied. trade[%s] broker[%s]", exchangeCode, symbol, FPoolName, tradeSymbol, brokerSymbol);
		WriteUFCLog(logHead, logStr, constFlush);
	}
	UnlockContract(RWLockType);
	return isSuccess;
}  //CTradingObjectsPool::AddContract()
//---------------------------------------------------------------------------
void CTradingObjectsPool::ClearAllContract(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	LockContract(RWLockType);
	ClearContractMarketSymbolIndex(constNotDelete, rwNotLock);
	ClearContractBrokerSymbolIndex(constNotDelete, rwNotLock);
	ClearContractTradeSymbolIndex(constNotDelete, rwNotLock);
	ClearContractSymbolIndex(DoDelete, rwNotLock);
	UnlockContract(RWLockType);
}  //CTradingObjectsPool::ClearAllContract()
//---------------------------------------------------------------------------
CTransferBank *CTradingObjectsPool::FindTransferBankFromCodeIndex(const String& BankCode, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CTransferBank*>(FindTradingObjFromMap(FTransferBankCodeIndex, BankCode, &FTransferBankCSLock, RWLockType));
}  //CTradingObjectsPool::FindTransferBankFromCodeIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::InsertTransferBankIntoCodeIndex(const String& BankCode, CTransferBank *BankPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	return InsertTradingObjIntoMap(FTransferBankCodeIndex, BankCode, BankPtr, DoCheckExist, &FTransferBankCSLock, RWLockType);
}  //CTradingObjectsPool::InsertTransferBankIntoCodeIndex()
//---------------------------------------------------------------------------
CTransferBank *CTradingObjectsPool::RemoveTransferBankFromCodeIndex(const String& BankCode, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CTransferBank*>(RemoveTradingObjFromMap(FTransferBankCodeIndex, BankCode, &FTransferBankCSLock, RWLockType));
}  //CTradingObjectsPool::RemoveTransferBankFromCodeIndex()
//---------------------------------------------------------------------------
int CTradingObjectsPool::ClearTransferBankCodeIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FTransferBankCodeIndex, DoDelete, &FTransferBankCSLock, RWLockType);
}  //CTradingObjectsPool::ClearTransferBankCodeIndex()
//---------------------------------------------------------------------------
void CTradingObjectsPool::DumpTransferBankCodeIndex(ReadWrietLockTypeEnum RWLockType)
{
	String logStr;
	LockContract(RWLockType);
	logStr.printf(L"===== Dump %s %d Transfer Bank Code Index =====", FPoolName, FTransferBankCodeIndex.size());
	WriteUFCLog(logStr, constNotFlush);
	DumpTradingObjMap(FTransferBankCodeIndex, constNotFlush, &FTransferBankCSLock, rwNotLock);
	UnlockContract(RWLockType);
	logStr = L" ";
	WriteUFCLog(logStr, constFlush);
}  //CTradingObjectsPool::DumpTransferBankCodeIndex()
//---------------------------------------------------------------------------
CTradingAccount *CTradingObjectsPool::FindAccountFromLogonIdIndex(const String& LogonId, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CTradingAccount*>(FindTradingObjFromMap(FTradingAccountLogonIdIndex, LogonId, &FAccountCSLock, RWLockType));
}  //CTradingObjectsPool::FindAccountFromLogonIDIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::InsertAccountIntoLogonIdIndex(const String& LogonId, CTradingAccount *AccountPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	return InsertTradingObjIntoMap(FTradingAccountLogonIdIndex, LogonId, AccountPtr, DoCheckExist, &FAccountCSLock, RWLockType);
}  //CTradingObjectsPool::InsertAccountIntoIndex()
//---------------------------------------------------------------------------
CTradingAccount *CTradingObjectsPool::RemoveAccountFromLogonIdIndex(const String& LogonId, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CTradingAccount*>(RemoveTradingObjFromMap(FTradingAccountLogonIdIndex, LogonId, &FAccountCSLock, RWLockType));
}  //CTradingObjectsPool::RemoveAccountFromLogonIDIndex()
//---------------------------------------------------------------------------
int CTradingObjectsPool::ClearAccountLogonIdIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FTradingAccountLogonIdIndex, DoDelete, &FAccountCSLock, RWLockType);
}  //CTradingObjectsPool::ClearAccountLogonIDIndex()
//---------------------------------------------------------------------------
void CTradingObjectsPool::DumpAccountLogonIDIndex(ReadWrietLockTypeEnum RWLockType)
{
	String logStr;
	LockAccount(RWLockType);
	logStr.printf(L"===== Dump %s %d Trading Account LogonId Index =====", FPoolName, FTradingAccountLogonIdIndex.size());
	WriteUFCLog(logStr, constNotFlush);
	DumpTradingObjMap(FTradingAccountLogonIdIndex, constNotFlush, &FAccountCSLock, rwNotLock);
	UnlockAccount(RWLockType);
	logStr = L" ";
	WriteUFCLog(logStr, constFlush);
}  //CTradingObjectsPool::DumpAccountIndex()
//---------------------------------------------------------------------------
bool CTradingObjectsPool::IsAllAccountDataQueried(ReadWrietLockTypeEnum RWLockType)
{
	LockAccount(RWLockType);
	CWStrTradingObjPtrHMap::iterator it;
	bool isTaifexFeeQueried = false;
	bool isOverseasFeeQueried = false;
	bool isTaifexHoldPositionQueried = false;
	bool isOverseasHoldPositionQueried = false;
	for (it = FTradingAccountLogonIdIndex.begin(); it != FTradingAccountLogonIdIndex.end(); it++)
	{
		CTradingBasicObject *tradingObjPtr = it->second;
		CTradingAccount *accountPtr = 0;
		if (tradingObjPtr != 0)
		{
			accountPtr = static_cast<CTradingAccount*>(tradingObjPtr);
			accountPtr->LockForRead();
			isTaifexFeeQueried = accountPtr->GetTaifexFeeQueryStatus(rwNotLock) == qsFinished;
			isOverseasFeeQueried = accountPtr->GetOverseasFeeQueryStatus(rwNotLock) == qsFinished;
			isTaifexHoldPositionQueried = accountPtr->GetTaifexHoldPositionQueryStatus(rwNotLock) == qsFinished;
			isOverseasHoldPositionQueried = accountPtr->GetOverseasHoldPositionQueryStatus(rwNotLock) == qsFinished;
			accountPtr->UnlockForRead();
			if (!isTaifexFeeQueried || !isOverseasFeeQueried || !isTaifexHoldPositionQueried || !isOverseasHoldPositionQueried) break;
		}
	}  //for (it = FTradingAccountLogonIdIndex.begin(); it != FTradingAccountLogonIdIndex.end(); it++)
	UnlockAccount(RWLockType);
	if (!isTaifexFeeQueried || !isOverseasFeeQueried || !isTaifexHoldPositionQueried || !isOverseasHoldPositionQueried)
		return false;
	else
		return true;
}  //CTradingObjectsPool::IsAllAccountDataQueried()
//---------------------------------------------------------------------------
CContractPosition *CTradingObjectsPool::FindAccountContractPosition(const String& LogonId, const String& ExchangeCode, const String& Symbol, bool CreatePositionIfNotExist, int& ProcessCode, String& ProcessMsg, ReadWrietLockTypeEnum RWLockType)
{
	CTradingAccount *accountPtr = FindAccountFromLogonIdIndex(LogonId, rwLockForRead);
	if (accountPtr == 0)
	{
		ProcessCode = -1;
		ProcessMsg.printf(L"Account[%s] not found.", LogonId);
		return 0;
	}

	ProcessMsg = L"";
	ProcessCode = 0;
	accountPtr->Lock(RWLockType);
	CContractPosition *positionPtr = accountPtr->FindContractPositionFromSymbolIndex(ExchangeCode, Symbol, rwNotLock);
	if (positionPtr == 0)
	{
		if (CreatePositionIfNotExist)
		{
			positionPtr = new s888::CContractPosition(ExchangeCode, Symbol);
			if (accountPtr->InsertContractPositionIntoSymbolIndex(ExchangeCode, Symbol, positionPtr, constNotCheckExist, rwNotLock))
			{
				accountPtr->AppendContractPositionToArray(positionPtr, rwNotLock);
				ProcessCode = 1;
			}
			else
			{
				delete positionPtr;
				positionPtr = 0;
				ProcessCode = -3;
				ProcessMsg.printf(L"Insert Account[%s] ContractPosition[%s-%s] Failed.", LogonId, ExchangeCode, Symbol);
			}
		}
		else
		{
			ProcessCode = -2;
			ProcessMsg.printf(L"Account[%s] ContractPosition[%s-%s] not Found.", LogonId, ExchangeCode, Symbol);
		}
	}
	accountPtr->Unlock(RWLockType);
	return positionPtr;
}  //CTradingObjectsPool::FindAccountContractPosition()
//---------------------------------------------------------------------------
int CTradingObjectsPool::ClearAccountContractHoldPosition(const String& LogonId, ReadWrietLockTypeEnum RWLockType)
{
	int clearCount = 0;
	CTradingAccount *accountPtr = FindAccountFromLogonIdIndex(LogonId, RWLockType);
	if (accountPtr == 0) return clearCount;

	accountPtr->LockForRead();
	for (int i = 0; i < accountPtr->GetContractPositionArrayCount(rwNotLock); i++)
	{
		CContractPosition *curPositionPtr = accountPtr->GetContractPositionFromArray(i, rwNotLock);
		curPositionPtr->LockForWrite();
		curPositionPtr->SetBuyHoldQuantity(0, rwNotLock);
		curPositionPtr->SetSellHoldQuantity(0, rwNotLock);
		curPositionPtr->UnlockForWrite();
		clearCount++;
	}
	accountPtr->UnlockForRead();
	return clearCount;
}  //CTradingObjectsPool::ClearAccountContractHoldPosition()
//---------------------------------------------------------------------------
CFeeObject *CTradingObjectsPool::FindFeeObjFromLogonIdContractCodeIndex(const String& LogonId, const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType)
{
	CFeeObject *feePtr = 0;
	CTradingAccount *accountPtr = FindAccountFromLogonIdIndex( LogonId, RWLockType);
	if( accountPtr == 0 )
		return feePtr;
	feePtr = accountPtr->FindFeeFromCodeIndex(ExchangeCode, ContractCode, RWLockType);
	if( feePtr == 0 )
		feePtr = accountPtr->FindFeeFromTradeCodeIndex(ExchangeCode, ContractCode, RWLockType);
	return feePtr;
}  //CTradingObjectsPool::FindFeeObjFromLogonIdContractCodeIndex()
//---------------------------------------------------------------------------
int CTradingObjectsPool::SetTaifexCommodityFeeObject(CTradingAccount *AccountPtr)
{
	int processCount = 0;
	int createCount = 0;
	if (AccountPtr == 0) return createCount;
	String taifexExchangeCode = L"TAIFEX";
	String logonId = AccountPtr->GetLogonId(rwLockForRead);
	String logHead = FPoolName + L"::SetTaifexCommodityUnitFee";
	String logMsg;
	LockCommodity(rwLockForRead);
	for (CWStrTradingObjPtrHMap::iterator it = FCommodityCodeIndex.begin(); it != FCommodityCodeIndex.end(); it++)
	{
		CTradingCommodity *curCommodityPtr = static_cast<CTradingCommodity*>(it->second);
		curCommodityPtr->LockForRead();
		String curExchangeCode = curCommodityPtr->GetExchangeCode(rwNotLock);
		String curContractCode = curCommodityPtr->GetContractCode(rwNotLock);
		String curTradeContractCode = curCommodityPtr->GetTradeContractCode(rwNotLock);
		String curBrokerContractCode = curCommodityPtr->GetBrokerContractCode(rwNotLock);
		nsOrderMessageDefine::MarketEnum curMarket = curCommodityPtr->GetMarket(rwNotLock);
		String curContractKind = curCommodityPtr->GetContractKind(rwNotLock);
		curCommodityPtr->UnlockForRead();
		if (curExchangeCode != taifexExchangeCode) continue;
		processCount++;

		CFeeObject *curFeeObjPtr = AccountPtr->FindFeeFromCodeIndex(curExchangeCode, curContractCode, rwLockForRead);
		if ((curFeeObjPtr == 0) && (curContractKind == L"S"))
		{
			curFeeObjPtr = new CFeeObject(logonId, curExchangeCode, curContractCode, curMarket);
			curFeeObjPtr->SetTradeContractCode(curTradeContractCode);
			curFeeObjPtr->SetBrokerContractCode(curBrokerContractCode);

			double openRateByMoney = 0.0;
			double openRateByVolume = 0.0;
			double closeRateByMoney = 0.0;
			double closeRateByVolume = 0.0;
			double closeTodayRateByMoney = 0.0;
			double closeTodayRateByVolume = 0.0;
			if (curMarket == nsOrderMessageDefine::mTWOptions)
			{
				CFeeObject *stockFutFeeObjPtr = AccountPtr->GetStockFutFee(rwLockForRead);
				if (stockFutFeeObjPtr != 0)
				{
					stockFutFeeObjPtr->LockForRead();
					openRateByMoney = stockFutFeeObjPtr->GetOpenRateByMoney(rwNotLock);
					openRateByVolume = stockFutFeeObjPtr->GetOpenRateByVolume(rwNotLock);
					closeRateByMoney = stockFutFeeObjPtr->GetCloseRateByMoney(rwNotLock);
					closeRateByVolume = stockFutFeeObjPtr->GetCloseRateByVolume(rwNotLock);
					closeTodayRateByMoney = stockFutFeeObjPtr->GetCloseTodayRateByMoney(rwNotLock);
					closeTodayRateByVolume = stockFutFeeObjPtr->GetCloseTodayRateByVolume(rwNotLock);
					stockFutFeeObjPtr->UnlockForRead();
				}
			}
			else if (curMarket == nsOrderMessageDefine::mTWFutures)
			{
				CFeeObject *stockOptFeeObjPtr = AccountPtr->GetStockOptFee(rwLockForRead);
				if (stockOptFeeObjPtr != 0)
				{
					stockOptFeeObjPtr->LockForRead();
					openRateByMoney = stockOptFeeObjPtr->GetOpenRateByMoney(rwNotLock);
					openRateByVolume = stockOptFeeObjPtr->GetOpenRateByVolume(rwNotLock);
					closeRateByMoney = stockOptFeeObjPtr->GetCloseRateByMoney(rwNotLock);
					closeRateByVolume = stockOptFeeObjPtr->GetCloseRateByVolume(rwNotLock);
					closeTodayRateByMoney = stockOptFeeObjPtr->GetCloseTodayRateByMoney(rwNotLock);
					closeTodayRateByVolume = stockOptFeeObjPtr->GetCloseTodayRateByVolume(rwNotLock);
					stockOptFeeObjPtr->UnlockForRead();
				}
			}  //if (curMarket == nsOrderMessageDefine::mTWOptions)
			curFeeObjPtr->SetOpenRateByMoney(openRateByMoney, rwNotLock);
			curFeeObjPtr->SetOpenRateByVolume(openRateByVolume, rwNotLock);
			curFeeObjPtr->SetCloseRateByMoney(closeRateByMoney, rwNotLock);
			curFeeObjPtr->SetCloseRateByVolume(closeRateByVolume, rwNotLock);
			curFeeObjPtr->SetCloseTodayRateByMoney(closeTodayRateByMoney, rwNotLock);
			curFeeObjPtr->SetCloseTodayRateByVolume(closeTodayRateByVolume, rwNotLock);

			if (!AccountPtr->AddFee(curFeeObjPtr, rwLockForWrite))
				createCount++;
			else
			{
				logMsg.printf(L"Add Fee[%s-%s] into Account[%s] Falied.", curExchangeCode, curContractCode, logonId);
				WriteUFCLog(logHead, logMsg, constFlush);
				delete curFeeObjPtr;
			}
		}  //if ((curFeeObjPtr == 0) && (curContractKind == L"S"))
	}  //for (CWStrTradingObjPtrHMap::iterator it = FCommodityCodeIndex.begin(); it != FCommodityCodeIndex.end(); it++)
	UnlockCommodity(rwLockForRead);
	logMsg.printf(L"Process %d Taifex Commodities, create %d Fee Object.", processCount, createCount);
	WriteUFCLog(logHead, logMsg, constFlush);
	return createCount;
}  //CTradingObjectsPool::SetTaifexCommodityFeeObject()
//---------------------------------------------------------------------------
int CTradingObjectsPool::SetTaifexCommodityFeeObject(const String& LogonId)
{
	CTradingAccount *accountPtr = FindAccountFromLogonIdIndex(LogonId, rwLockForRead);
	return SetTaifexCommodityFeeObject(accountPtr);
}  //CTradingObjectsPool::SetTaifexCommodityFeeObject()
//---------------------------------------------------------------------------
CTradingObjPtrVector *CTradingObjectsPool::GenerateCTPQueryFeeContractArray(ReadWrietLockTypeEnum RWLockType)
{
	String logHead = FPoolName + L"::GenerateCTPQueryFeeContractArray";
	String logMsg;
	CTradingObjPtrVector *contractArrayPtr = new CTradingObjPtrVector();
	LockCommodity(RWLockType);
	CWStrTradingObjPtrHMap::iterator it;
	int forLoopCount = 0;
	for (it = FCommodityCodeIndex.begin(); it != FCommodityCodeIndex.end(); it++)
	{
		forLoopCount++;
		CTradingBasicObject *curObjPtr = it->second;
		if (curObjPtr == 0)
		{
			logMsg.printf(L"%d/%d null Commodity pointer.", forLoopCount, FCommodityCodeIndex.size());
			WriteUFCLog(logHead, logMsg, constFlush);
			continue;
		}

		CTradingCommodity *curCommodityPtr = static_cast<CTradingCommodity*>(curObjPtr);
		CTradingContract *firstContractPtr = curCommodityPtr->GetFirstContract(rwLockForRead);
		if (firstContractPtr != 0)
			contractArrayPtr->push_back(firstContractPtr);
		else
		{
			logMsg.printf(L"%d/%d Commodity[%s] has no contract.", forLoopCount, FCommodityCodeIndex.size());
			WriteUFCLog(logHead, logMsg, constFlush);
		}
	}  //for (it = FCommodityCodeIndex.begin(); it != FCommodityCodeIndex.end(); it++)
	UnlockCommodity(RWLockType);
	return contractArrayPtr;
}  //CTradingObjectsPool::GenerateCTPQueryFeeContractArray()
};  //namespace s888
#pragma package(smart_init)
