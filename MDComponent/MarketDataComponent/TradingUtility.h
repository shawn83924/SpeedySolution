//---------------------------------------------------------------------------

#ifndef TradingUtilityH
#define TradingUtilityH
#include <vector>
#include <map>
#include <hash_map>
#include <deque>
#include "System.hpp"
#include "UFC.h"
#include "OrderMessageDefine.h"
//---------------------------------------------------------------------------
namespace std
{
//---------------------------------------------------------------------------
size_t hash_value(const String& unicodeStr)
{
	long hashResult = 0;
	for (int i = 1; i <= unicodeStr.Length(); i++)
	{
		wchar_t curChar = unicodeStr[i];
		hashResult = (hashResult << 1) ^ curChar;
	}
	return static_cast<size_t>(hashResult);
}  //hash_value()
};  //namespace std

//---------------------------------------------------------------------------
namespace s888
{
//---------------------------------------------------------------------------
typedef enum
{
	qsNone            = 0,
	qsPrepareRequest  = 1,
	qsRequestSended   = 2,
	qsResponseArrived = 3,
	qsParsingResponse = 4,
	qsFinished        = 5,
	qsFailed          = 6
} QueryStatusEnum;

//---------------------------------------------------------------------------
typedef enum
{
	rwNotLock      = 0,
	rwLockForRead  = 1,
	rwLockForWrite = 2
} ReadWrietLockTypeEnum;

//---------------------------------------------------------------------------
const bool constCreateRWLock = true;
const bool constNotCreateRWLock = false;
//const bool constLock = true;
//const bool constNotLock = false;
const bool constDelete = true;
const bool constNotDelete = false;
const bool constCheckExist = true;
const bool constNotCheckExist = false;
const bool constSort = true;
const bool constNotSort = false;
const bool constFlush = true;
const bool constNotFlush = false;
const bool constCreate = true;
const bool constNotCreate = false;
const bool constKeepPositive = true;
const bool constNotKeepPositive = false;

int ConvertUnicodeToAnsi(const String& UnicodeStr, char *AnsiBuffer, int MaxAnsiLength);
AnsiString ConvertUnicodeToAnsiStr(const String& UnicodeStr);
UFC::AnsiString ConvertUnicodeToUFCAnsiStr(const String& UnicodeStr);
void WriteUFCLog(const String& LogStr, bool DoFlush = constNotFlush);
void WriteUFCLog(const String& HeadStr, const String& LogStr, bool DoFlush = constNotFlush);
void FlushUFCLog() {UFC::BufferedLog::FlushToFile();};

String DescribeMarket(nsOrderMessageDefine::MarketEnum Market);
String DescribeCallPut(nsOrderMessageDefine::EventTypeEnum CallPut);
String DescribeBuySell(nsOrderMessageDefine::SideEnum BuySell);
String DescribeOrderType(nsOrderMessageDefine::OrderTypeEnum OrderType);
String DescribePositionEffect(nsOrderMessageDefine::PositionEffectEnum PositionEffect);
String DescribeExecType(nsOrderMessageDefine::ExecTypeEnum ExecType);
String DescribeTMPExecType(nsOrderMessageDefine::TMPExecTypeEnum TMPExecType);
String DescribeTimeInForce(nsOrderMessageDefine::TimeInForceEnum TimeInForce);
String DescribeOrderStatus(nsOrderMessageDefine::OrderStatusEnum OrderStatus);
String DescribeCxlRejResponseTo(nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo);
String DescribeOrdRejReason(nsOrderMessageDefine::OrdRejReasonEnum OrdRejReason);
String DescribeQueryStatus(QueryStatusEnum QueryStatus);

bool IsDigitalCharacter(const wchar_t Data);
bool IsDigitalString(const String& TargetStr);

//Lock Function For LockObject
void DoLockObject(UFC::PCriticalSection *LockObjPtr, ReadWrietLockTypeEnum RWLockType);
void DoUnlockObject(UFC::PCriticalSection *LockObjPtr, ReadWrietLockTypeEnum RWLockType);
void DoLockObject(UFC::PReadWriteLock *LockObjPtr, ReadWrietLockTypeEnum RWLockType);
void DoUnlockObject(UFC::PReadWriteLock *LockObjPtr, ReadWrietLockTypeEnum RWLockType);

//---------------------------------------------------------------------------
template<typename fieldType, typename LockType> fieldType GetFieldValue(const fieldType& Field, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	DoLockObject(LockObjPtr, RWLockType);
	fieldType resultValue = MemberField;
	DoUnlockObject(LockObjPtr, RWLockType);
	return resultValue;
}  //GetFieldValueRWLock()

//---------------------------------------------------------------------------
template<typename fieldType, typename LockType> void SetFieldValue(const fieldType& NewValue, fieldType& Field, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	DoLockObject(LockObjPtr, RWLockType);
	MemberField = NewValue;
	DoUnlockObject(LockObjPtr, RWLockType);
}  //SetFieldValueRWLock()

//---------------------------------------------------------------------------
template<typename containerType, typename LockType> int GetObjectContainerCount(containerType& ObjectContainer, LockType *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	DoLockObject(LockObjPtr, RWLockType);
	int containerCount = static_cast<int>(ObjectContainer.size());
	DoUnlockObject(LockObjPtr, RWLockType);
	return containerCount;
}  //GetObjectContainerCount()
};  //namespace s888
#endif
