//---------------------------------------------------------------------------


#pragma hdrstop

#include "TTaifexConnection.h"
#include "CTPObjects.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
namespace s888
{
//---------------------------- CCTPBasicCommand -----------------------------
//---------------------------------------------------------------------------
CCTPBasicCommand::CCTPBasicCommand()
:CTradingBasicObject(constCreateRWLock)
,FCommandId(0)
,FBrokerId(L"")
,FInvestorId(L"")
,FRequestId(0)
,FCmdFields("^\n")
,FCommandText(L"")
{
}  //CCTPBasicCommand::CCTPBasicCommand()
//---------------------------------------------------------------------------
CCTPBasicCommand::CCTPBasicCommand(int CommandId, const String& BrokerId, const String& InvestorId)
:CTradingBasicObject(constCreateRWLock)
,FCommandId(CommandId)
,FBrokerId(BrokerId)
,FInvestorId(InvestorId)
,FRequestId(0)
,FCmdFields("^\n")
,FCommandText(L"")
{
}  //CCTPBasicCommand::CCTPBasicCommand()
//---------------------------------------------------------------------------
void CCTPBasicCommand::SetCommandFields(ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FCmdFields.Clear();
	if (FBrokerId.Length() > 0)  //經紀公司代碼
	{
		UFC::AnsiString ansiBrokerId = ConvertUnicodeToUFCAnsiStr(FBrokerId);
		FCmdFields.Append("BrokerId", ansiBrokerId.c_str());
	}

	if (FInvestorId.Length() > 0)  //投資者代碼
	{
		UFC::AnsiString ansiInvestorId = ConvertUnicodeToUFCAnsiStr(FInvestorId);
		FCmdFields.Append("InvestorId", ansiInvestorId.c_str());
	}
	Unlock(RWLockType);
}  //CCTPBasicCommand::SetCommandFields()
//---------------------------------------------------------------------------
UFC::AnsiString CCTPBasicCommand::GenerateAnsiCommandText(ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	UFC::AnsiString ansiCmdText = FCmdFields.ToString();
	Unlock(RWLockType);
	return ansiCmdText;
}  //CCTPBasicCommand::GenerateAnsiCommandText()

//-------------------- CCTPQueryInstrumentCommissionRate --------------------
//---------------------------------------------------------------------------
void CCTPQueryInstrumentCommissionRate::SetCommandFields(ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	CCTPBasicCommand::SetCommandFields(rwNotLock);
	if (FInstrumentId.Length() > 0)  //合約代碼
	{
		UFC::AnsiString ansiInstrumentId = ConvertUnicodeToUFCAnsiStr(FInstrumentId);
		FCmdFields.Append("InstrumentId", ansiInstrumentId.c_str());
	}
	Unlock(RWLockType);
}  //CCTPQueryInstrumentCommissionRate::SetCommandFields()
//---------------------------------------------------------------------------
CCTPQueryInstrumentCommissionRate::CCTPQueryInstrumentCommissionRate(const String& BrokerId, const String& InvestorId, const String& InstrumentId)
:CCTPBasicCommand(CTP_QueryInstrumentCommissionRate, BrokerId, InvestorId)
,FInstrumentId(InstrumentId)
{
	SetCommandFields(rwNotLock);
}  //CCTPQueryInstrumentCommissionRate::CCTPQueryInstrumentCommissionRate()

//------------------------ CCTPQueryInvestorPosition ------------------------
//---------------------------------------------------------------------------
void CCTPQueryInvestorPosition::SetCommandFields(ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	CCTPBasicCommand::SetCommandFields(rwNotLock);
	if (FInstrumentId.Length() > 0)  //合約代碼
	{
		UFC::AnsiString ansiInstrumentId = ConvertUnicodeToUFCAnsiStr(FInstrumentId);
		FCmdFields.Append("InstrumentId", ansiInstrumentId.c_str());
	}
	Unlock(RWLockType);
}  //CCTPQueryInvestorPosition::SetCommandFields()
//---------------------------------------------------------------------------
CCTPQueryInvestorPosition::CCTPQueryInvestorPosition(const String& BrokerId, const String& InvestorId, const String& InstrumentId)
:CCTPBasicCommand(CTP_QueryInvestorPosition, BrokerId, InvestorId),
 FInstrumentId(InstrumentId)
{
	SetCommandFields(rwNotLock);
}  //CCTPQueryInvestorPosition::CCTPQueryInvestorPosition()

//------------------------ CCTPQueryCTPTransferBank -------------------------
//---------------------------------------------------------------------------
void CCTPQueryCTPTransferBank::SetCommandFields(ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FCmdFields.Clear();
	if (FBankId.Length() > 0)  //銀行代碼
	{
		UFC::AnsiString ansiBankId = ConvertUnicodeToUFCAnsiStr(FBankId);
		FCmdFields.Append("BankId", ansiBankId.c_str());
	}
	if (FBankBranchId.Length() > 0)  //銀行分中心代碼
	{
		UFC::AnsiString ansiBankBranchId = ConvertUnicodeToUFCAnsiStr(FBankBranchId);
		FCmdFields.Append("BankBranchId", ansiBankBranchId.c_str());
	}
	Unlock(RWLockType);
}  //CCTPQueryCTPTransferBank::SetCommandFields()
//---------------------------------------------------------------------------
CCTPQueryCTPTransferBank::CCTPQueryCTPTransferBank(const String& BankId, const String& BankBranchId)
:CCTPBasicCommand(CTP_QueryTransferBank),
 FBankId(BankId),
 FBankBranchId(BankBranchId)
{
	SetCommandFields(rwNotLock);
}  //CCTPQueryCTPTransferBank::CCTPQueryCTPTransferBank()

//------------------------ CCTPQueryAccountRegister -------------------------
//---------------------------------------------------------------------------
void CCTPQueryAccountRegister::SetCommandFields(ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	CCTPBasicCommand::SetCommandFields(rwNotLock);
	if (FBankId.Length() > 0)  //銀行代碼
	{
		UFC::AnsiString ansiBankId = ConvertUnicodeToUFCAnsiStr(FBankId);
		FCmdFields.Append("BankId", ansiBankId.c_str());
	}
	Unlock(RWLockType);
}  //CCTPQueryAccountRegister::SetCommandFields()
//---------------------------------------------------------------------------
CCTPQueryAccountRegister::CCTPQueryAccountRegister(const String& BrokerId, const String& InvestorId, const String& BankId)
:CCTPBasicCommand(CTP_QueryAccountRegister, BrokerId, InvestorId),
 FBankId(BankId)
{
	SetCommandFields(rwNotLock);
}  //CCTPQueryAccountRegister::CCTPQueryAccountRegister()
};  //namespace s888
