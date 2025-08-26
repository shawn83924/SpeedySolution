//---------------------------------------------------------------------------

#ifndef TradingBasicObjectsH
#define TradingBasicObjectsH
#include "TradingUtility.h"
//---------------------------------------------------------------------------
namespace s888
{
//---------------------------------------------------------------------------
class CTradingBasicObject
{
protected:
	UFC::PCriticalSection *FCSLockPtr;
	String                FMainKey;

	template<typename fieldType> fieldType GetFieldValue(const fieldType& MemberField, ReadWrietLockTypeEnum RWLockType)
	{
		DoLockObject(FCSLockPtr, RWLockType);
		fieldType resultValue = MemberField;
		DoUnlockObject(FCSLockPtr, RWLockType);
		return resultValue;
	}  //GetFieldValue()

	template<typename fieldType> void SetFieldValue(const fieldType& NewValue, fieldType& MemberField, ReadWrietLockTypeEnum RWLockType)
	{
		DoLockObject(FCSLockPtr, RWLockType);
		MemberField = NewValue;
		DoUnlockObject(FCSLockPtr, RWLockType);
	}  //SetFieldValue()

	template<typename fieldType> fieldType IncreaseFieldValue(const fieldType& IncreaseValue, fieldType& MemberField, bool KeepPositive, ReadWrietLockTypeEnum RWLockType)
	{
		fieldType sumValue = 0;
		fieldType zeroValue = static_cast<fieldType>(0);
		DoLockObject(FCSLockPtr, RWLockType);
		sumValue = MemberField + IncreaseValue;
		if (KeepPositive && (sumValue < zeroValue)) sumValue = zeroValue;
		MemberField = sumValue;
		DoUnlockObject(FCSLockPtr, RWLockType);
		return sumValue;
	}  //SetFieldValue()

public:
	//Constructor
	CTradingBasicObject(bool NeedCreatReadWriiteLock = constNotCreateRWLock);
	~CTradingBasicObject();

	//Lock Function
	void Lock(ReadWrietLockTypeEnum RWLockType) { DoLockObject(FCSLockPtr, RWLockType); };
	void LockForRead() { DoLockObject(FCSLockPtr, rwLockForRead); };
	void LockForWrite() { DoLockObject(FCSLockPtr, rwLockForWrite); };
	void Unlock(ReadWrietLockTypeEnum RWLockType) {DoUnlockObject(FCSLockPtr, RWLockType); };
	void UnlockForRead() { DoUnlockObject(FCSLockPtr, rwLockForRead); };
	void UnlockForWrite() { DoUnlockObject(FCSLockPtr, rwLockForWrite); };

	//Get/Set Field Function
	virtual String GetMainKey(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FMainKey, RWLockType);};
	virtual void SetMainKey(const String& MainKey, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(MainKey, FMainKey, RWLockType);};

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock) {};
};  //CTradingBasicObject

//---------------------------------------------------------------------------
static bool lessMainKey(CTradingBasicObject *TradingObjPtr1, CTradingBasicObject *TradingObjPtr2)
{
	return (TradingObjPtr1->GetMainKey(rwLockForRead) < TradingObjPtr2->GetMainKey(rwLockForRead));
};  //lessMainKey()

//declare Vector Function template
typedef std::vector<CTradingBasicObject*> CTradingObjPtrVector;
//---------------------------------------------------------------------------
template<typename VectorType, typename LockType> CTradingBasicObject *GetTradingObjFromVector(VectorType& ObjectVector, int ElementIndex, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	CTradingBasicObject *objectPtr = 0;
	DoLockObject(LockObjPtr, RWLockType);
	if ((ElementIndex >= 0) && (ElementIndex < static_cast<int>(ObjectVector.size())))
		objectPtr = ObjectVector[ElementIndex];
	DoUnlockObject(LockObjPtr, RWLockType);
	return objectPtr;
}  //GetTradingObjFromVector()
//---------------------------------------------------------------------------
template<typename VectorType, typename LockType> bool AppendTradingObjToVector(VectorType& ObjectVector, CTradingBasicObject *ObjectPtr, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (ObjectPtr == 0) return false;
	DoLockObject(LockObjPtr, RWLockType);
	ObjectVector.push_back(ObjectPtr);
	DoUnlockObject(LockObjPtr, RWLockType);
	return true;
}  //AppendTradingObjToVector()
//---------------------------------------------------------------------------
template<typename VectorType, typename LockType> int ClearTradingObjVector(VectorType& ObjectVector, bool DoDelete, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	int deleteCount = 0;
	DoLockObject(LockObjPtr, RWLockType);
	if (DoDelete)
	{
		for (unsigned int i = 0; i < ObjectVector.size(); i++)
		{
			CTradingBasicObject *elementPtr = ObjectVector[i];
			if (elementPtr != 0)
			{
				delete elementPtr;
				deleteCount++;
			}
		}  //for (unsigned int i = 0; i < ObjectVector.size(); i++)
	}  //if (DoDelete)
	ObjectVector.clear();
	DoUnlockObject(LockObjPtr, RWLockType);
	return deleteCount;
}  //ClearTradingObjVector()
//---------------------------------------------------------------------------
template<typename VectorType, typename LockType> void SortTradingObjVector(VectorType& ObjectVector, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	DoLockObject(LockObjPtr, RWLockType);
	if (ObjectVector.size() > 0)
		std::sort(ObjectVector.begin(), ObjectVector.end(), lessMainKey);
	DoUnlockObject(LockObjPtr, RWLockType);
}  //SortTradingObjVector()
//---------------------------------------------------------------------------
template<typename VectorType, typename LockType> void DumpTradingObjVector(VectorType& ObjectVector, bool DoFlush, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	DoLockObject(LockObjPtr, RWLockType);
	for (unsigned int i = 0; i < ObjectVector.size(); i++)
	{
		CTradingBasicObject *elementPtr = ObjectVector[i];
		if (elementPtr == 0) continue;
		elementPtr->DumpData(constNotFlush, rwLockForRead);
	}  //for (unsigned int i = 0; i < ObjectVector.size(); i++)
	DoUnlockObject(LockObjPtr, RWLockType);
	if (DoFlush) FlushUFCLog();
}  //ClearTradingObjVector()


//Define Hash Map type
typedef stdext::hash_map<String, CTradingBasicObject*> CWStrTradingObjPtrHMap;  //wide Character
typedef stdext::hash_map<AnsiString, CTradingBasicObject*> CNStrObjectPtrHMap;  //byte Character
typedef stdext::hash_map<long, CTradingBasicObject*> CLongObjectPtrHMap;
typedef stdext::hash_map<short, CTradingBasicObject*> CShortObjectPtrHMap;
typedef stdext::hash_map<short, String> CShortStringHMap;

//Define Map type
typedef std::map<String, CTradingBasicObject*> CWStrTradingObjPtrMap;  //wide Character
typedef std::map<AnsiString, CTradingBasicObject*> CNStrObjectPtrMap;  //byte Character
typedef std::map<long, CTradingBasicObject*> CLongObjectPtrMap;
typedef std::map<short, CTradingBasicObject*> CShortObjectPtrMap;
typedef std::map<short, String> CShortStringMap;

//declare Map Function template
//---------------------------------------------------------------------------
template<typename MapType, typename KeyType, typename LockType>
CTradingBasicObject *FindTradingObjFromMap(MapType& ObjectMap, const KeyType& KeyValue, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	CTradingBasicObject *objectPtr = 0;
	DoLockObject(LockObjPtr, RWLockType);
	MapType::iterator it = ObjectMap.find(KeyValue);
	if (it != ObjectMap.end()) objectPtr = it->second;
	DoUnlockObject(LockObjPtr, RWLockType);
	return objectPtr;
}  //FindTradingObjFromMap()
//---------------------------------------------------------------------------
template<typename MapType, typename KeyType, typename LockType>
bool InsertTradingObjIntoMap(MapType& ObjectMap, const KeyType& KeyValue, CTradingBasicObject *ObjectPtr, bool DoCheckExist, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (ObjectPtr == 0) return false;
	DoLockObject(LockObjPtr, RWLockType);
	if (DoCheckExist && (FindTradingObjFromMap(ObjectMap, KeyValue, LockObjPtr, rwNotLock) != 0)) return false;
	std::pair<MapType::iterator, bool> insertPair = ObjectMap.insert(MapType::value_type(KeyValue, ObjectPtr));
	DoUnlockObject(LockObjPtr, RWLockType);
	return insertPair.second;
}  //InsertTradingObjIntoMap()
//---------------------------------------------------------------------------
template<typename MapType, typename KeyType, typename LockType>
CTradingBasicObject *RemoveTradingObjFromMap(MapType& ObjectMap, const KeyType& KeyValue, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	CTradingBasicObject *objectPtr = 0;
	DoLockObject(LockObjPtr, RWLockType);
	MapType::iterator it = ObjectMap.find(KeyValue);
	if (it != ObjectMap.end())
	{
		objectPtr = it->second;
		ObjectMap.erase(it);
	}
	DoUnlockObject(LockObjPtr, RWLockType);
	return objectPtr;
}  //RemoveTradingObjFromMap()
//---------------------------------------------------------------------------
template<typename MapType, typename LockType>
int ClearTradingObjMap(MapType& ObjectMap, bool DoDelete, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	int deleteCount = 0;
	DoLockObject(LockObjPtr, RWLockType);
	if (DoDelete)
	{
		MapType::iterator it;
		for (it = ObjectMap.begin(); it != ObjectMap.end(); it++)
		{
			CTradingBasicObject *tradingObjPtr = it->second;
			if (tradingObjPtr != 0)
			{
				deleteCount++;
				delete tradingObjPtr;
			}
		}  //for (it = ObjectMap.begin(); it != ObjectMap.end(); it++)
	}  //if (DoDelete)
	ObjectMap.clear();
	DoUnlockObject(LockObjPtr, RWLockType);
	return deleteCount;
}  //ClearTradingObjMap()
//---------------------------------------------------------------------------
template<typename MapType, typename LockType>
void DumpTradingObjMap(MapType& TradingObjMap, bool DoFlush, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	MapType::iterator it;
	DoLockObject(LockObjPtr, RWLockType);
	for (it = TradingObjMap.begin(); it != TradingObjMap.end(); it++)
	{
		CTradingBasicObject *tradingObjPtr = static_cast<CTradingBasicObject*>(it->second);
		tradingObjPtr->DumpData(constNotFlush, rwLockForRead);
	}  //for (it = TradingObjMap.begin(); it != TradingObjMap.end(); it++)
	DoUnlockObject(LockObjPtr, RWLockType);
	if (DoFlush) FlushUFCLog();
}  //DumpTradingObjMap()

//---------------------------------------------------------------------------
typedef std::deque<CTradingBasicObject*> CTradingObjQueue;
//---------------------------------------------------------------------------
template<typename QueueType, typename LockType>
CTradingBasicObject *IsQueueEmpty(QueueType& TradingObjQueue, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	DoLockObject(LockObjPtr, RWLockType);
	bool isEmpty = TradingObjQueue.empty();
	UnlockObject(LockObjPtr, RWLockType);
	return isEmpty;
}  //GetTradingObjFromQueue()
//---------------------------------------------------------------------------
template<typename QueueType, typename LockType>
CTradingBasicObject *GetTradingObjFromQueue(QueueType& TradingObjQueue, int QueueIndex, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	CTradingBasicObject *tradingObjPtr = 0;
	DoLockObject(LockObjPtr, RWLockType);
	if (TradingObjQueue.empty()) return tradingObjPtr;
	if ((QueueIndex >= 0) && (QueueIndex < static_cast<int>(TradingObjQueue.size())))
		tradingObjPtr = TradingObjQueue[QueueIndex];
	DoUnlockObject(LockObjPtr, RWLockType);
	return tradingObjPtr;
}  //GetTradingObjFromQueue()
//---------------------------------------------------------------------------
template<typename QueueType, typename LockType>
bool PushTradingObjToQueueFront(QueueType& TradingObjQueue, CTradingBasicObject *TradingObjPtr, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (TradingObjPtr == 0) return false;
	DoLockObject(LockObjPtr, RWLockType);
	TradingObjQueue.push_front(TradingObjPtr);
	DoUnlockObject(LockObjPtr, RWLockType);
	return true;
}  //PushTradingObjToQueueFront()
//---------------------------------------------------------------------------
template<typename QueueType, typename LockType>
bool PushTradingObjToQueueBack(QueueType& TradingObjQueue, CTradingBasicObject *TradingObjPtr, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (TradingObjPtr == 0) return false;
	DoLockObject(LockObjPtr, RWLockType);
	TradingObjQueue.push_back(TradingObjPtr);
	DoUnlockObject(LockObjPtr, RWLockType);
	return true;
}  //PushTradingObjToQueueBack()
//---------------------------------------------------------------------------
template<typename QueueType, typename LockType>
CTradingBasicObject *GetFirstTradingObjOfQueue(QueueType& TradingObjQueue, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	CTradingBasicObject *tradingObjPtr = 0;
	DoLockObject(LockObjPtr, RWLockType);
	if (!TradingObjQueue.empty()) tradingObjPtr = TradingObjQueue.front();
	DoUnlockObject(LockObjPtr, RWLockType);
	return tradingObjPtr;
}  //GetFirstTradingObjOfQueue()
//---------------------------------------------------------------------------
template<typename QueueType, typename LockType>
void PopFirstTradingObjFromQueue(QueueType& TradingObjQueue, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	DoLockObject(LockObjPtr, RWLockType);
	if (!TradingObjQueue.empty()) TradingObjQueue.pop_front();
	DoUnlockObject(LockObjPtr, RWLockType);
}  //PopFirstTradingObjFromQueue()
//---------------------------------------------------------------------------
template<typename QueueType, typename LockType>
CTradingBasicObject *GetLastTradingObjOfQueue(QueueType& TradingObjQueue, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	CTradingBasicObject *tradingObjPtr = 0;
	DoLockObject(LockObjPtr, RWLockType);
	if (!TradingObjQueue.empty()) tradingObjPtr = TradingObjQueue.back();
	DoUnlockObject(LockObjPtr, RWLockType);
	return tradingObjPtr;
}  //GetLastTradingObjOfQueue()
//---------------------------------------------------------------------------
template<typename QueueType, typename LockType>
void PopLastTradingObjFromQueue(QueueType& TradingObjQueue, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	DoLockObject(LockObjPtr, RWLockType);
	if (!TradingObjQueue.empty()) TradingObjQueue.pop_back();
	DoUnlockObject(LockObjPtr, RWLockType);
}  //PopLastTradingObjFromQueue()
//---------------------------------------------------------------------------
template<typename QueueType, typename LockType>
int ClearTradingObjQueue(QueueType& TradingObjQueue, bool DoDelete, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	int deleteCount = 0;
	DoLockObject(LockObjPtr, RWLockType);
	while (!TradingObjQueue.empty())
	{
		CTradingBasicObject *tradingObjPtr = 0;
		if (DoDelete) tradingObjPtr = TradingObjQueue.front();
		TradingObjQueue.pop_front();
		if (tradingObjPtr != 0)
		{
			delete tradingObjPtr;
			deleteCount++;
		}
	}  //while (!BasicTradingObjQueue.empty())
	DoUnlockObject(LockObjPtr, RWLockType);
	return deleteCount;
}  //ClearTradingObjQueue()
//---------------------------------------------------------------------------
template<typename QueueType, typename LockType>
void DumpTradingObjQueue(QueueType& TradingObjQueue, bool DoFlush, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	DoLockObject(LockObjPtr, RWLockType);
	if (TradingObjQueue.empty())
	{
		UnlockForReadWrite(RWLockPtr, RWLockType);
		return;
	}

	for (unsigned int i = 0; i < TradingObjQueue.size(); i++)
		TradingObjQueue[i]->DumpData(rwLockForRead);
	DoUnlockObject(LockObjPtr, RWLockType);
	if (DoFlush) FlushUFCLog();
}  //DumpTradingObjQueue()
//---------------------------------------------------------------------------
class CExchangeKey : public CTradingBasicObject
{
protected:
	String FExchangeCode;
	String FBrokerExchangeCode;

public:
	//Constructor
	CExchangeKey(bool NeedCreatReadWriiteLock, const String& ExchangeCode, const String& BrokerExchangeCode = L"");

	//Get/Set Field Function
	virtual String GetExchangeCode(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FExchangeCode, RWLockType);};
	virtual void SetExchangeCode(const String& ExchangeCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	virtual String GetBrokerExchangeCode(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBrokerExchangeCode, RWLockType);};
	virtual void SetBrokerExchangeCode(const String& BrokerExchangeCode, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BrokerExchangeCode, FBrokerExchangeCode, RWLockType);};

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CExchangeKey

//---------------------------------------------------------------------------
class CExchangeContractKey : public CExchangeKey
{
public:
	static String BuildKey(const String& ExchangeCode, const String& ContractCode);
	static String BuildKey(nsOrderMessageDefine::MarketEnum Market, const String& ContractCode);

protected:
	String                           FContractCode;        //合約代碼
	String                           FBrokerContractCode;  //Taifex 舊有 ContractCode
	String                           FTradeContractCode;   //下單到 TaifexConnection 用的 ContractCode
	nsOrderMessageDefine::MarketEnum FMarket;
	String                           FBrokerKey;
	String                           FTradeKey;
	String                           FMarketKey;

public:
	//Constructor
	CExchangeContractKey(bool NeedCreatReadWriiteLock,
						 const String& ExchangeCode, const String& ContractCode,
						 nsOrderMessageDefine::MarketEnum Market,
						 const String& BrokerContractCode = L"", const String& TradeContractCode = L"");

	//Get/Set Field Function
	virtual void SetExchangeCode(const String& ExchangeCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	virtual String GetContractCode(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FContractCode, RWLockType);};
	virtual void SetContractCode(const String& ContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	virtual String GetBrokerContractCode(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBrokerContractCode, RWLockType);};
	virtual void SetBrokerContractCode(const String& BrokerContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	virtual String GetTradeContractCode(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTradeContractCode, RWLockType);};
	virtual void SetTradeContractCode(const String& TradeContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	virtual nsOrderMessageDefine::MarketEnum GetMarket(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FMarket, RWLockType);};
	virtual void SetMarket(nsOrderMessageDefine::MarketEnum Market, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	virtual String GetBrokerKey(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBrokerKey, RWLockType);};
	virtual void SetBrokerKey(const String& BrokerKey, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BrokerKey, FBrokerKey, RWLockType);};

	virtual String GetTradeKey(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTradeKey, RWLockType);};
	virtual void SetTradeKey(const String& TradeKey, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(TradeKey, FTradeKey, RWLockType);};

	virtual String GetMarketKey(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FMarketKey, RWLockType);};
	virtual void SetMarketKey(const String& MarketKey, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(MarketKey, FMarketKey, RWLockType);};

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CExchangeContractKey

//---------------------------------------------------------------------------
class CExchangeSymbolKey : public CExchangeKey
{
public:
	static String BuildKey(const String& ExchangeCode, const String& Symbol);
	static String BuildKey(nsOrderMessageDefine::MarketEnum Market, const String& Symbol);

protected:
	String                           FSymbol;
	String                           FBrokerSymbol;          //Taifex 舊有 Symbol
	String                           FTradeSymbol;           //下單到 TaifexConnection 用的 Symbol
	nsOrderMessageDefine::MarketEnum FMarket;
	String                           FBrokerKey;
	String                           FTradeKey;
	String                           FMarketKey;

public:
	//Constructor
	CExchangeSymbolKey(bool NeedCreatReadWriiteLock,
					   const String& ExchangeCode, const String& Symbol,
					   nsOrderMessageDefine::MarketEnum Market,
					   const String& BrokerSymbol = L"", const String& TradeSymbol = L"");

	//Get/Set Field Function
	virtual void SetExchangeCode(const String& ExchangeCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	String GetSymbol(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSymbol, RWLockType);};
	void SetSymbol(const String& Symbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	String GetBrokerSymbol(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBrokerSymbol, RWLockType);};
	void SetBrokerSymbol(const String& BrokerSymbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	String GetTradeSymbol(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTradeSymbol, RWLockType);};
	void SetTradeSymbol(const String& TradeSymbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	nsOrderMessageDefine::MarketEnum GetMarket(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FMarket, RWLockType);};
	void SetMarket(nsOrderMessageDefine::MarketEnum Market, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	virtual String GetBrokerKey(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBrokerKey, RWLockType);};
	virtual void SetBrokerKey(const String& BrokerKey, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BrokerKey, FBrokerKey, RWLockType);};

	virtual String GetTradeKey(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTradeKey, RWLockType);};
	virtual void SetTradeKey(const String& TradeKey, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(TradeKey, FTradeKey, RWLockType);};

	virtual String GetMarketKey(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FMarketKey, RWLockType);};
	virtual void SetMarketKey(const String& MarketKey, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(MarketKey, FMarketKey, RWLockType);};

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CExchangeSymbolKey

//---------------------------------------------------------------------------
class CLogonIdKey : public CTradingBasicObject
{
protected:
	String FLogonId;

public:
	//Constructor
	CLogonIdKey(bool NeedCreatReadWriiteLock, const String& LogonId);

	//Get/Set Field Function
	virtual String GetLogonId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FLogonId, RWLockType);};
	virtual void SetLogonId(const String& LogonId, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CExchangeKey


};  //namespace s888
#endif
