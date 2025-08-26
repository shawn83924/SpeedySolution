//---------------------------------------------------------------------------

#ifndef TradingObjectPoolH
#define TradingObjectPoolH
#include "TradingUtility.h"
#include "TradingBasicObjects.h"
#include "TradingCommodities.h"
#include "TradingInvestors.h"
//---------------------------------------------------------------------------
namespace s888
{
//---------------------------------------------------------------------------
class CTradingObjectsPool : public CTradingBasicObject
{
private:
	String          FPoolName;
	QueryStatusEnum FTaifexFuturesContractQueryStatus;
	String          FTaifexFuturesContractQueryMessage;
	QueryStatusEnum FTaifexOptionsContractQueryStatus;
	String          FTaifexOptionsContractQueryMessage;
	QueryStatusEnum FOverseasContractQueryStatus;
	String          FOverseasContractQueryMessage;
	QueryStatusEnum FTaifexTaxRateQueryStatus;
	String          FTaifexTaxRateQueryMessage;

	//Container for Exchange
	UFC::PCriticalSection  FExchangeCSLock;
	CWStrTradingObjPtrHMap FExchangeCodeIndex;
	CWStrTradingObjPtrHMap FExchangeBrokerCodeIndex;

	//Container for Commodity
	UFC::PCriticalSection  FCommodityCSLock;
	CWStrTradingObjPtrHMap FCommodityCodeIndex;
	CWStrTradingObjPtrHMap FCommodityTradeCodeIndex;
	CWStrTradingObjPtrHMap FCommodityBrokerCodeIndex;
	CWStrTradingObjPtrHMap FCommodityMarketCodeIndex;

	//Container for Tax
	UFC::PCriticalSection  FTaxRateCSLock;
	CWStrTradingObjPtrHMap FTaxRateCodeIndex;
	CWStrTradingObjPtrHMap FTaxRateBrokerCodeIndex;

	//Container for Contract
	UFC::PCriticalSection  FContractCSLock;
	CWStrTradingObjPtrHMap FContractSymbolIndex;
	CWStrTradingObjPtrHMap FContractTradeSymbolIndex;
	CWStrTradingObjPtrHMap FContractBrokerSymbolIndex;
	CWStrTradingObjPtrHMap FContractMarketSymbolIndex;

	//Container for Transfer Bank
	UFC::PCriticalSection  FTransferBankCSLock;
	CWStrTradingObjPtrHMap FTransferBankCodeIndex;

	//Container for Trading Account
	UFC::PCriticalSection   FAccountCSLock;
	CWStrTradingObjPtrHMap  FTradingAccountLogonIdIndex;

    CTaxRateObject* FStockTaxRate;
public:
	CTradingObjectsPool();
	CTradingObjectsPool(const String& PoolName);
	~CTradingObjectsPool();

	String GetPoolName() {return FPoolName;};
	void SetPoolName(const String& PoolName) {FPoolName = PoolName;};

	QueryStatusEnum GetTaifexFuturesContractQueryStatus() {return FTaifexFuturesContractQueryStatus;};
	void SetTaifexFuturesContractQueryStatus(QueryStatusEnum TaifexFuturesContractQueryStatus) {FTaifexFuturesContractQueryStatus = TaifexFuturesContractQueryStatus;};
	String GetTaifexFuturesContractQueryMessage() {return FTaifexFuturesContractQueryMessage;};
	void SetTaifexFuturesContractQueryMessage(String TaifexFuturesContractQueryMessage) {FTaifexFuturesContractQueryMessage = TaifexFuturesContractQueryMessage;};
	QueryStatusEnum GetTaifexOptionsContractQueryStatus() {return FTaifexOptionsContractQueryStatus;};
	void SetTaifexOptionsContractQueryStatus(QueryStatusEnum TaifexOptionsContractQueryStatus) {FTaifexOptionsContractQueryStatus = TaifexOptionsContractQueryStatus;};
	String GetTaifexOptionsContractQueryMessage() {return FTaifexOptionsContractQueryMessage;};
	void SetTaifexOptionsContractQueryMessage(String TaifexOptionsContractQueryMessage) {FTaifexOptionsContractQueryMessage = TaifexOptionsContractQueryMessage;};
	QueryStatusEnum GetOverseasContractQueryStatus() {return FOverseasContractQueryStatus;};
	void SetOverseasContractQueryStatus(QueryStatusEnum OverseasContractQueryStatus) {FOverseasContractQueryStatus = OverseasContractQueryStatus;};
	String GetOverseasContractQueryMessage() {return FOverseasContractQueryMessage;};
	void SetOverseasContractQueryMessage(String OverseasContractQueryMessage) {FOverseasContractQueryMessage = OverseasContractQueryMessage;};
	QueryStatusEnum GetTaifexTaxRateQueryStatus() {return FTaifexTaxRateQueryStatus;};
	void SetTaifexTaxRateQueryStatus(QueryStatusEnum TaifexTaxRateQueryStatus) {FTaifexTaxRateQueryStatus = TaifexTaxRateQueryStatus;};
	String GetTaifexTaxRateQueryMessage() {return FTaifexTaxRateQueryMessage;};
	void SetTaifexTaxRateQueryMessage(String TaifexTaxRateQueryMessage) {FTaifexTaxRateQueryMessage = TaifexTaxRateQueryMessage;};

	//Exchange Lock Function
	void LockExchange(ReadWrietLockTypeEnum RWLockType = rwNotLock) {DoLockObject(&FExchangeCSLock, RWLockType);};
	void UnlockExchange(ReadWrietLockTypeEnum RWLockType = rwNotLock) {DoUnlockObject(&FExchangeCSLock, RWLockType);};

	//Exchange Code Index Function
	int GetExchangeCodeIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FExchangeCodeIndex, &FExchangeCSLock, RWLockType);};
	CTradingExchange *FindExchangeFromCodeIndex(const String& ExchangeCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertExchangeIntoCodeIndex(const String& ExchangeCode, CTradingExchange *ExchangePtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTradingExchange *RemoveExchangeFromCodeIndex(const String& ExchangeCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearExchangeCodeIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpExchangeCodeIndex(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Exchange Broker Code Index Function
	int GetExchangeBrokerCodeIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FExchangeBrokerCodeIndex, &FExchangeCSLock, RWLockType);};
	CTradingExchange *FindExchangeFromBrokerCodeIndex(const String& BrokerExchangeCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertExchangeIntoBrokerCodeIndex(const String& BrokerExchangeCode, CTradingExchange *ExchangePtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTradingExchange *RemoveExchangeFromBrokerCodeIndex(const String& BrokerExchangeCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearExchangeBrokerCodeIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpExchangeBrokerCodeIndex(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	String ConvertIfBorkerExchangeCode(const String& OriginalExchangeCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AddExchange(CTradingExchange *ExchangePtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void ClearAllExchange(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool LoadExchangeInformation(const String& InformationFileName);

	//Commodity Lock Function
	void LockCommodity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {DoLockObject(&FCommodityCSLock, RWLockType);};
	void UnlockCommodity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {DoUnlockObject(&FCommodityCSLock, RWLockType);};

	//Commodity Code Index Function
	int GetCommodityCodeIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FCommodityCodeIndex, &FCommodityCSLock, RWLockType);};
	CTradingCommodity *FindCommodityFromCodeIndex(const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertCommodityIntoCodeIndex(const String& ExchangeCode, const String& ContractCode, CTradingCommodity *CommodityPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTradingCommodity *RemoveCommodityFromCodeIndex(const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearCommodityCodeIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpCommodityCodeIndex(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Commodity Trade Code Index Function
	int GetCommodityTradeCodeIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FCommodityTradeCodeIndex, &FCommodityCSLock, RWLockType);};
	CTradingCommodity *FindCommodityFromTradeCodeIndex(const String& ExchangeCode, const String& TradeContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertCommodityIntoTradeCodeIndex(const String& ExchangeCode, const String& TradeContractCode, CTradingCommodity *CommodityPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTradingCommodity *RemoveCommodityFromTradeCodeIndex(const String& ExchangeCode, const String& TradeContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearCommodityTradeCodeIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpCommodityTradeCodeIndex(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Commodity Broker Code Index Function
	int GetCommodityBrokerCodeIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FCommodityBrokerCodeIndex, &FCommodityCSLock, RWLockType);};
	CTradingCommodity *FindCommodityFromBrokerCodeIndex(const String& ExchangeCode, const String& BrokerContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertCommodityIntoBrokerCodeIndex(const String& ExchangeCode, const String& BrokerContractCode, CTradingCommodity *CommodityPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTradingCommodity *RemoveCommodityFromBrokerCodeIndex(const String& ExchangeCode, const String& BrokerContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearCommodityBrokerCodeIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpCommodityBrokerCodeIndex(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Commodity Market Code Index Function
	int GetCommodityMarketCodeIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FCommodityMarketCodeIndex, &FCommodityCSLock, RWLockType);};
	CTradingCommodity *FindCommodityFromMarketCodeIndex(nsOrderMessageDefine::MarketEnum Market, const String& ContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertCommodityIntoMarketCodeIndex(nsOrderMessageDefine::MarketEnum Market, const String& ContractCode, CTradingCommodity *CommodityPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTradingCommodity *RemoveCommodityFromMarketCodeIndex(nsOrderMessageDefine::MarketEnum Market, const String& ContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearCommodityMarketCodeIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpCommodityMarketCodeIndex(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	bool AddCommodity(CTradingCommodity *CommodityPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void ClearAllCommodity(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//TaxRate Lock Function
	void LockTaxRate(ReadWrietLockTypeEnum RWLockType = rwNotLock) {DoLockObject(&FTaxRateCSLock, RWLockType);};
	void UnlockTaxRate(ReadWrietLockTypeEnum RWLockType = rwNotLock) {DoUnlockObject(&FTaxRateCSLock, RWLockType);};

	//TaxRate Code Index Function
	int GetTaxRateCodeIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FTaxRateCodeIndex, &FTaxRateCSLock, RWLockType);};
	CTaxRateObject *FindTaxRateFromCodeIndex(const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertTaxRateIntoCodeIndex(const String& ExchangeCode, const String& ContractCode, CTaxRateObject *TaxRatePtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTaxRateObject *RemoveTaxRateFromCodeIndex(const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearTaxRateCodeIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpTaxRateCodeIndex(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//TaxRate Broker Code Index Function
	int GetTaxRateBrokerCodeIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FTaxRateBrokerCodeIndex, &FTaxRateCSLock, RWLockType);};
	CTaxRateObject *FindTaxRateFromBrokerCodeIndex(const String& ExchangeCode, const String& BrokerContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertTaxRateIntoBrokerCodeIndex(const String& ExchangeCode, const String& BrokerContractCode, CTaxRateObject *TaxRatePtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTaxRateObject *RemoveTaxRateFromBrokerCodeIndex(const String& ExchangeCode, const String& BrokerContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearTaxRateBrokerCodeIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpTaxRateBrokerCodeIndex(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	bool AddTaxRate(CTaxRateObject *TaxRatePtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void ClearAllTaxRate(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Contract Lock Function
	void LockContract(ReadWrietLockTypeEnum RWLockType = rwNotLock) {DoLockObject(&FContractCSLock, RWLockType);};
	void UnlockContract(ReadWrietLockTypeEnum RWLockType = rwNotLock) {DoUnlockObject(&FContractCSLock, RWLockType);};

	//Contract Symbol Index Function
	int GetContractSymbolIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FContractSymbolIndex, &FContractCSLock, RWLockType);};
	CTradingContract *FindContractFromSymbolIndex(const String& ExchangeCode, const String& Symbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertContractIntoSymbolIndex(const String& ExchangeCode, const String& Symbol, CTradingContract *ContractPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTradingContract *RemoveContractFromSymbolIndex(const String& ExchangeCode, const String& Symbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearContractSymbolIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpContractSymbolIndex(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Contract Trade Symbol Index Function
	int GetContractTradeSymbolIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FContractTradeSymbolIndex, &FContractCSLock, RWLockType);};
	CTradingContract *FindContractFromTradeSymbolIndex(const String& ExchangeCode, const String& TradeSymbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertContractIntoTradeSymbolIndex(const String& ExchangeCode, const String& TradeSymbol, CTradingContract *ContractPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTradingContract *RemoveContractFromTradeSymbolIndex(const String& ExchangeCode, const String& TradeSymbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearContractTradeSymbolIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpContractTradeSymbolIndex(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Contract Broker Symbol Index Function
	int GetContractBrokerSymbolIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FContractBrokerSymbolIndex, &FContractCSLock, RWLockType);};
	CTradingContract *FindContractFromBrokerSymbolIndex(const String& ExchangeCode, const String& BrokerSymbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertContractIntoBrokerSymbolIndex(const String& ExchangeCode, const String& BrokerSymbol, CTradingContract *ContractPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTradingContract *RemoveContractFromBrokerSymbolIndex(const String& ExchangeCode, const String& BrokerSymbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearContractBrokerSymbolIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpContractBrokerSymbolIndex(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Contract Market Symbol Index Function
	int GetContractMarketSymbolIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FContractMarketSymbolIndex, &FContractCSLock, RWLockType);};
	CTradingContract *FindContractFromMarketSymbolIndex(nsOrderMessageDefine::MarketEnum Market, const String& Symbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertContractIntoMarketSymbolIndex(nsOrderMessageDefine::MarketEnum Market, const String& Symbol, CTradingContract *ContractPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTradingContract *RemoveContractFromMarketSymbolIndex(nsOrderMessageDefine::MarketEnum Market, const String& Symbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearContractMarketSymbolIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpContractMarketSymbolIndex(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	CTradingContract *FindContractBySymbol(const String& ExchangeCode, const String& OrigSymbol, bool CheckSymbolIndexFirst, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AddContract(CTradingContract *ContractPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void ClearAllContract(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Transfer Bank Lock Function
	void LockTransferBank(ReadWrietLockTypeEnum RWLockType = rwNotLock) {DoLockObject(&FTransferBankCSLock, RWLockType);};
	void UnlockTransferBank(ReadWrietLockTypeEnum RWLockType = rwNotLock) {DoUnlockObject(&FTransferBankCSLock, RWLockType);};

	//Transfer Bank Code Index Function
	int GetTransferBankCodeIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FTransferBankCodeIndex, &FContractCSLock, RWLockType);};
	CTransferBank *FindTransferBankFromCodeIndex(const String& BankCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertTransferBankIntoCodeIndex(const String& BankCode, CTransferBank *BankPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTransferBank *RemoveTransferBankFromCodeIndex(const String& BankCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearTransferBankCodeIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpTransferBankCodeIndex(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Trading Account Lock Function
	void LockAccount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {DoLockObject(&FAccountCSLock, RWLockType);};
	void UnlockAccount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {DoUnlockObject(&FAccountCSLock, RWLockType);};

	//Trading Account Index Function
	int GetAccountLogonIdIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FTradingAccountLogonIdIndex, &FAccountCSLock, RWLockType);};
	CTradingAccount *FindAccountFromLogonIdIndex(const String& LogonId, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertAccountIntoLogonIdIndex(const String& LogonId, CTradingAccount *AccountPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CTradingAccount *RemoveAccountFromLogonIdIndex(const String& LogonId, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearAccountLogonIdIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpAccountLogonIDIndex(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	bool IsAllAccountDataQueried(ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CContractPosition *FindAccountContractPosition(const String& LogonId, const String& ExchangeCode, const String& Symbol, bool CreatePositionIfNotExist, int& ProcessCode, String& ProcessMsg, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearAccountContractHoldPosition(const String& LogonId, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	CFeeObject *FindFeeObjFromLogonIdContractCodeIndex(const String& LogonId, const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int SetTaifexCommodityFeeObject(CTradingAccount *AccountPtr);
	int SetTaifexCommodityFeeObject(const String& LogonId);

	CTradingObjPtrVector *GenerateCTPQueryFeeContractArray(ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CTradingObjectsPool

extern CTradingObjectsPool gTradingObjsPool;
extern CTradingObjectsPool gQueryObjsPool;
};  //namespace s888
#endif
