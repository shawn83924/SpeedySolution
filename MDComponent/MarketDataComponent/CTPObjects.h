//---------------------------------------------------------------------------

#ifndef CTPObjectsH
#define CTPObjectsH
//---------------------------------------------------------------------------
#include "System.hpp"
#include "UFC.h"
#include "NameValueMessage.h"
#include "TradingBasicObjects.h"
#include "TradingCommodities.h"
//---------------------------------------------------------------------------
namespace s888
{
//---------------------------------------------------------------------------
class CCTPBasicCommand : public CTradingBasicObject
{
protected:
	int    FCommandId;   //CTP指令代碼
	String FBrokerId;    //經紀公司代碼
	String FInvestorId;  //投資者代碼

	int    FRequestId;

	UFC::NameValueMessage FCmdFields;
	String FCommandText;

public:
	CCTPBasicCommand();
	CCTPBasicCommand(int CommandId, const String& BrokerId = L"", const String& InvestorId = L"");

	int GetCommandId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCommandId, RWLockType);};
	void SetCommandId(int CommandId, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CommandId, FCommandId, RWLockType);};

	String GetBrokerId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBrokerId, RWLockType);};
	void SetBrokerId(const String& BrokerId, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BrokerId, FBrokerId, RWLockType);};

	String GetInvestorId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FInvestorId, RWLockType);};
	void SetInvestorId(const String& InvestorId, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(InvestorId, FInvestorId, RWLockType);};

	int GetRequestId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FRequestId, RWLockType);};
	void SetRequestId(int RequestId, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(RequestId, FRequestId, RWLockType);};

	String GetCommandText(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCommandText, RWLockType);};
	void SetCommandText(const String& CommandText, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(CommandText, FCommandText, RWLockType);};

	virtual void SetCommandFields(ReadWrietLockTypeEnum RWLockType = rwNotLock);
	UFC::AnsiString GenerateAnsiCommandText(ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CCTPBasicCommand

//---------------------------------------------------------------------------
class CCTPQueryInstrumentCommissionRate : public CCTPBasicCommand
{
protected:
	String FInstrumentId;  //合約代碼

public:
	String GetInstrumentId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FInstrumentId, RWLockType);};
	void SetInstrumentId(const String& InstrumentId, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(InstrumentId, FInstrumentId, RWLockType);};

	virtual void SetCommandFields(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	CCTPQueryInstrumentCommissionRate(const String& BrokerId, const String& InvestorId, const String& InstrumentId);
};  //CCTPQryInstrumentCommissionRate

//---------------------------------------------------------------------------
class CCTPQueryInvestorPosition : public CCTPBasicCommand
{
protected:
	String FInstrumentId;  //合約代碼

public:
	String GetInstrumentId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FInstrumentId, RWLockType);};
	void SetInstrumentId(const String& InstrumentId, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(InstrumentId, FInstrumentId, RWLockType);};

	virtual void SetCommandFields(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	CCTPQueryInvestorPosition(const String& BrokerId, const String& InvestorId, const String& InstrumentId);
};  //CCTPQueryInvestorPosition

//---------------------------------------------------------------------------
class CCTPQueryCTPTransferBank : public CCTPBasicCommand
{
protected:
	String FBankId;        //銀行代碼
	String FBankBranchId;  //銀行分中心代碼

public:
	String GetBankId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBankId, RWLockType);};
	void SetBankId(const String& BankId, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BankId, FBankId, RWLockType);};

	String GetBankBranchId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBankBranchId, RWLockType);};
	void SetBankBranchId(const String& BankBranchId, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BankBranchId, FBankBranchId, RWLockType);};

	virtual void SetCommandFields(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	CCTPQueryCTPTransferBank(const String& BankId, const String& BankBranchId);
};  //CCTPQueryCTPTransferBank

//---------------------------------------------------------------------------
class CCTPQueryAccountRegister : public CCTPBasicCommand
{
protected:
	String FBankId;        //銀行代碼

public:
	String GetBankId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBankId, RWLockType);};
	void SetBankId(const String& BankId, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BankId, FBankId, RWLockType);};

	virtual void SetCommandFields(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	CCTPQueryAccountRegister(const String& BrokerId, const String& InvestorId, const String& BankId);
};  //CCTPQueryAccountRegister

};
#endif
