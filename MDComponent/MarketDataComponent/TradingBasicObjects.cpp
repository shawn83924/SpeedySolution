//---------------------------------------------------------------------------


#pragma hdrstop

#include "TradingBasicObjects.h"

//---------------------------------------------------------------------------
namespace s888
{
//--------------------------- CTradingBasicObject ---------------------------
//---------------------------------------------------------------------------
CTradingBasicObject::CTradingBasicObject(bool NeedCreatReadWriiteLock)
:FCSLockPtr(0)
,FMainKey(L"")
{
	if (NeedCreatReadWriiteLock) FCSLockPtr = new UFC::PCriticalSection();
}  //CTradingBasicObject::CTradingBasicObject()
//---------------------------------------------------------------------------
CTradingBasicObject::~CTradingBasicObject()
{
	if (FCSLockPtr != 0) delete FCSLockPtr;
}  //CTradingBasicObject::~CTradingBasicObject()
//---------------------------------------------------------------------------
void CTradingBasicObject::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	Lock(RWLockType);
	FMainKey = SrcObjPtr->GetMainKey(rwLockForRead);
	Unlock(RWLockType);
}  //CTradingBasicObject::AssignFieldData()

//------------------------------ CExchangeKey -------------------------------
//---------------------------------------------------------------------------
CExchangeKey::CExchangeKey(bool NeedCreatReadWriiteLock, const String& ExchangeCode, const String& BrokerExchangeCode)
:CTradingBasicObject(NeedCreatReadWriiteLock)
,FExchangeCode(ExchangeCode)
,FBrokerExchangeCode(BrokerExchangeCode)
{
	FMainKey = FExchangeCode;
	if (FBrokerExchangeCode.Length() <= 0) FBrokerExchangeCode = FExchangeCode;
}  //CExchangeKey::CExchangeKey()
//---------------------------------------------------------------------------
void CExchangeKey::SetExchangeCode(const String& ExchangeCode, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FExchangeCode = ExchangeCode;
	FMainKey = FExchangeCode;
	Unlock(RWLockType);
}  //CExchangeKey::SetExchangeCode()
//---------------------------------------------------------------------------
void CExchangeKey::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	Lock(RWLockType);
	CTradingBasicObject::AssignFieldData(SrcObjPtr, rwNotLock);
	CExchangeKey *srcExchangeKeyPtr = static_cast<CExchangeKey*>(SrcObjPtr);
	srcExchangeKeyPtr->LockForRead();
	FExchangeCode = srcExchangeKeyPtr->GetExchangeCode(rwNotLock);
	FBrokerExchangeCode = srcExchangeKeyPtr->GetBrokerExchangeCode(rwNotLock);
	srcExchangeKeyPtr->UnlockForRead();
	Unlock(RWLockType);
}  //CExchangeKey::AssignFieldData()

//-------------------------- CExchangeContractKey ---------------------------
//---------------------------------------------------------------------------
String CExchangeContractKey::BuildKey(const String& ExchangeCode, const String& ContractCode)
{
	return ExchangeCode + ContractCode;
}  //CExchangeContractKey::BuildKey()
//---------------------------------------------------------------------------
String CExchangeContractKey::BuildKey(nsOrderMessageDefine::MarketEnum Market, const String& ContractCode)
{
	String keyValue;
	keyValue.printf(L"%d%s", Market, ContractCode);
	return keyValue;
}  //CExchangeContractKey::BuildKey()
//---------------------------------------------------------------------------
CExchangeContractKey::CExchangeContractKey(bool NeedCreatReadWriiteLock,
										   const String& ExchangeCode, const String& ContractCode,
										   nsOrderMessageDefine::MarketEnum Market,
										   const String& BrokerContractCode, const String& TradeContractCode)
:CExchangeKey(NeedCreatReadWriiteLock, ExchangeCode, ExchangeCode)
,FContractCode(ContractCode)
,FMarket(Market)
,FBrokerContractCode(BrokerContractCode)
,FTradeContractCode(TradeContractCode)
,FBrokerKey(L"")
,FTradeKey(L"")
,FMarketKey(L"")
{
	if (FBrokerContractCode.Length() <= 0) FBrokerContractCode = FContractCode;
	if (FTradeContractCode.Length() <= 0) FTradeContractCode = FContractCode;
	FMainKey = BuildKey(FExchangeCode, FContractCode);
	FBrokerKey = BuildKey(FExchangeCode, FBrokerContractCode);
	FTradeKey = BuildKey(FExchangeCode, FTradeContractCode);
	FMarketKey = BuildKey(FMarket, FContractCode);
};  //CExchangeContractKey::CExchangeContractKey()
//---------------------------------------------------------------------------
void CExchangeContractKey::SetExchangeCode(const String& ExchangeCode, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FExchangeCode = ExchangeCode;
	FMainKey = BuildKey(FExchangeCode, FContractCode);
	FBrokerKey = BuildKey(FExchangeCode, FBrokerContractCode);
	Unlock(RWLockType);
}  //CExchangeContractKey::SetExchangeCode()
//---------------------------------------------------------------------------
void CExchangeContractKey::SetContractCode(const String& ContractCode, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FContractCode = ContractCode;
	FMainKey = BuildKey(FExchangeCode, FContractCode);
	FMarketKey = BuildKey(FMarket, FContractCode);
	Unlock(RWLockType);
}  //CExchangeContractKey::SetContractCode()
//---------------------------------------------------------------------------
void CExchangeContractKey::SetBrokerContractCode(const String& BrokerContractCode, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FBrokerContractCode = BrokerContractCode;
	FBrokerKey = BuildKey(FExchangeCode, FBrokerContractCode);
	Unlock(RWLockType);
}  //CExchangeContractKey::SetBrokerContractCode()
//---------------------------------------------------------------------------
void CExchangeContractKey::SetTradeContractCode(const String& TradeContractCode, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FTradeContractCode = TradeContractCode;
	FTradeKey = BuildKey(FExchangeCode, FTradeContractCode);
	Unlock(RWLockType);
}  //CExchangeContractKey::SetTradeContractCode()
//---------------------------------------------------------------------------
void CExchangeContractKey::SetMarket(nsOrderMessageDefine::MarketEnum Market, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FMarket = Market;
	FMarketKey = BuildKey(FMarket, FContractCode);
	Unlock(RWLockType);
}  //CExchangeContractKey::SetMarket()
//---------------------------------------------------------------------------
void CExchangeContractKey::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	CExchangeContractKey *srcExchangeContractKeyPtr = static_cast<CExchangeContractKey*>(SrcObjPtr);
	Lock(RWLockType);
	srcExchangeContractKeyPtr->LockForRead();
	FExchangeCode = srcExchangeContractKeyPtr->GetExchangeCode(rwNotLock);
	FBrokerExchangeCode = srcExchangeContractKeyPtr->GetBrokerExchangeCode(rwNotLock);
	FContractCode = srcExchangeContractKeyPtr->GetContractCode(rwNotLock);
	FBrokerContractCode = srcExchangeContractKeyPtr->GetBrokerContractCode(rwNotLock);
	FTradeContractCode = srcExchangeContractKeyPtr->GetTradeContractCode(rwNotLock);
	FMarket = srcExchangeContractKeyPtr->GetMarket(rwNotLock);
	FMainKey = srcExchangeContractKeyPtr->GetMainKey(rwNotLock);
	FBrokerKey = srcExchangeContractKeyPtr->GetBrokerKey(rwNotLock);
	FTradeKey = srcExchangeContractKeyPtr->GetTradeKey(rwNotLock);
	FMarketKey = srcExchangeContractKeyPtr->GetMarketKey(rwNotLock);
	srcExchangeContractKeyPtr->UnlockForRead();
	Unlock(RWLockType);
}  //CExchangeContractKey::AssignFieldData()

//--------------------------- CExchangeSymbolKey ----------------------------
//---------------------------------------------------------------------------
String CExchangeSymbolKey::BuildKey(const String& ExchangeCode, const String& Symbol)
{
	return ExchangeCode + Symbol;
}  //CExchangeSymbolKey::BuildKey()
//---------------------------------------------------------------------------
String CExchangeSymbolKey::BuildKey(nsOrderMessageDefine::MarketEnum Market, const String& Symbol)
{
	String keyValue;
	keyValue.printf(L"%d%s", Market, Symbol);
	return keyValue;
}  //CExchangeSymbolKey::BuildKey()
//---------------------------------------------------------------------------
CExchangeSymbolKey::CExchangeSymbolKey(bool NeedCreatReadWriiteLock,
									   const String& ExchangeCode, const String& Symbol,
									   nsOrderMessageDefine::MarketEnum Market,
									   const String& BrokerSymbol, const String& TradeSymbol)
:CExchangeKey(NeedCreatReadWriiteLock, ExchangeCode, ExchangeCode)
,FSymbol(Symbol)
,FBrokerSymbol(BrokerSymbol)
,FTradeSymbol(TradeSymbol)
,FMarket(Market)
,FBrokerKey(L"")
,FTradeKey(L"")
,FMarketKey(L"")
{
	if (FBrokerSymbol.Length() <= 0) FBrokerSymbol = FSymbol;
	if (FTradeSymbol.Length() <= 0) FTradeSymbol = FSymbol;
	FMainKey = BuildKey(FExchangeCode, FSymbol);
	FBrokerKey = BuildKey(FExchangeCode, FBrokerSymbol);
	FTradeKey = BuildKey(FExchangeCode, FTradeSymbol);
	FMarketKey = BuildKey(FMarket, FTradeSymbol);
}  //CExchangeSymbolKey::CExchangeSymbolKey()
//---------------------------------------------------------------------------
void CExchangeSymbolKey::SetExchangeCode(const String& ExchangeCode, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FExchangeCode = ExchangeCode;
	FMainKey = BuildKey(FExchangeCode, FSymbol);
	FBrokerKey = BuildKey(FExchangeCode, FBrokerSymbol);
	FTradeKey = BuildKey(FExchangeCode, FTradeSymbol);
	Unlock(RWLockType);
}  //CExchangeSymbolKey::SetExchangeCode()
//---------------------------------------------------------------------------
void CExchangeSymbolKey::SetSymbol(const String& Symbol, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FSymbol = Symbol;
	FMainKey = BuildKey(FExchangeCode, FSymbol);
	Unlock(RWLockType);
}  //CExchangeSymbolKey::SetSymbol()
//---------------------------------------------------------------------------
void CExchangeSymbolKey::SetBrokerSymbol(const String& BrokerSymbol, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FBrokerSymbol = BrokerSymbol;
	FBrokerKey = BuildKey(FExchangeCode, FBrokerSymbol);
	Unlock(RWLockType);
}  //CExchangeSymbolKey::SetBrokerSymbol()
//---------------------------------------------------------------------------
void CExchangeSymbolKey::SetTradeSymbol(const String& TradeSymbol, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FTradeSymbol = TradeSymbol;
	FTradeKey = BuildKey(FExchangeCode, FTradeSymbol);
	Unlock(RWLockType);
}  //CExchangeSymbolKey::SetTradeSymbol()
//---------------------------------------------------------------------------
void CExchangeSymbolKey::SetMarket(nsOrderMessageDefine::MarketEnum Market, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FMarket = Market;
	FMarketKey = BuildKey(FExchangeCode, FMarket);
	Unlock(RWLockType);
}  //CExchangeSymbolKey::SetMarket()
//---------------------------------------------------------------------------
void CExchangeSymbolKey::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	CExchangeSymbolKey *srcExchangeSymbolKeyPtr = static_cast<CExchangeSymbolKey*>(SrcObjPtr);
	Lock(RWLockType);
	srcExchangeSymbolKeyPtr->LockForRead();
	FExchangeCode = srcExchangeSymbolKeyPtr->GetExchangeCode(rwNotLock);
	FBrokerExchangeCode = srcExchangeSymbolKeyPtr->GetBrokerExchangeCode(rwNotLock);
	FSymbol = srcExchangeSymbolKeyPtr->GetSymbol(rwNotLock);
	FBrokerSymbol = srcExchangeSymbolKeyPtr->GetBrokerSymbol(rwNotLock);
	FTradeSymbol = srcExchangeSymbolKeyPtr->GetTradeSymbol(rwNotLock);
	FMarket = srcExchangeSymbolKeyPtr->GetMarket(rwNotLock);
	FMainKey = srcExchangeSymbolKeyPtr->GetMainKey(rwNotLock);
	FBrokerKey = srcExchangeSymbolKeyPtr->GetBrokerKey(rwNotLock);
	FTradeKey = srcExchangeSymbolKeyPtr->GetTradeKey(rwNotLock);
	FMarketKey = srcExchangeSymbolKeyPtr->GetMarketKey(rwNotLock);
	srcExchangeSymbolKeyPtr->UnlockForRead();
	Unlock(RWLockType);
}  //CExchangeSymbolKey::AssignFieldData()

//------------------------------- CLogonIdKey -------------------------------
//---------------------------------------------------------------------------
CLogonIdKey::CLogonIdKey(bool NeedCreatReadWriiteLock, const String& LogonId)
:CTradingBasicObject(NeedCreatReadWriiteLock)
,FLogonId(LogonId)
{
	FMainKey = FLogonId;
}  //CLogonIdKey::CLogonIdKey()
//---------------------------------------------------------------------------
void CLogonIdKey::SetLogonId(const String& LogonId, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FLogonId = LogonId;
	FMainKey = FLogonId;
	Unlock(RWLockType);
}  //CLogonIdKey::SetLogonId()
//---------------------------------------------------------------------------
void CLogonIdKey::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	Lock(RWLockType);
	CTradingBasicObject::AssignFieldData(SrcObjPtr, rwNotLock);
	CLogonIdKey *srcLogonIdKeyPtr = static_cast<CLogonIdKey*>(SrcObjPtr);
	FLogonId = srcLogonIdKeyPtr->GetLogonId(rwLockForRead);
	Unlock(RWLockType);
}  //CLogonIdKey::AssignFieldData()
};  //namespace s888
#pragma package(smart_init)
