//---------------------------------------------------------------------------


#pragma hdrstop

#include "iniFile.h"
#include "TradingInvestors.h"

//---------------------------------------------------------------------------
namespace s888
{
//----------------------------- CMarketAccount ------------------------------
String CMarketAccount::BuildKey(nsOrderMessageDefine::MarketEnum Market)
{
	String keyValue;
	keyValue.printf(L"%02d", static_cast<short>(Market));
	return keyValue;
}  //CMarketAccount::BuildKey()
//---------------------------------------------------------------------------
CMarketAccount::CMarketAccount(nsOrderMessageDefine::MarketEnum Market, const String& Account)
:CTradingBasicObject(constCreateRWLock)
,FMarket(Market)
,FAccount(Account)
{
	FMainKey = BuildKey(FMarket);
}  //CMarketAccount::CMarketAccount()
//---------------------------------------------------------------------------
void CMarketAccount::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	Lock(RWLockType);
	CTradingBasicObject::AssignFieldData(SrcObjPtr, rwNotLock);
	CMarketAccount *srcMarketAccountPtr = static_cast<CMarketAccount*>(SrcObjPtr);
	srcMarketAccountPtr->LockForRead();
	FMarket = srcMarketAccountPtr->GetMarket(rwNotLock);
	FAccount = srcMarketAccountPtr->GetAccount(rwNotLock);
	srcMarketAccountPtr->UnlockForRead();
	Unlock(RWLockType);
}  //CMarketAccount::AssignFieldData()
//---------------------------------------------------------------------------
void CMarketAccount::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String dumpMessage;
	Lock(RWLockType);
	dumpMessage.printf(L"MarketAccount[%d][%s]:[%s].", FMarket, DescribeMarket(FMarket), FAccount);
	Unlock(RWLockType);
	WriteUFCLog(dumpMessage, DoFlushToFile);
}  //CMarketAccount::DumpData()

//------------------------------ CTransferBank ------------------------------
//---------------------------------------------------------------------------
CTransferBank::CTransferBank(const String& BankCode, const String& BankName)
:CTradingBasicObject(constCreateRWLock)
,FBankCode(BankCode)
,FBankName(BankName)
{
	FMainKey = FBankCode;
}  //CTransferBank::CTransferBank()
//---------------------------------------------------------------------------
void CTransferBank::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	Lock(RWLockType);
	CTradingBasicObject::AssignFieldData(SrcObjPtr, rwNotLock);
	CTransferBank *srcTransferBankPtr = static_cast<CTransferBank*>(SrcObjPtr);
	srcTransferBankPtr->LockForRead();
	FBankCode = srcTransferBankPtr->GetBankCode(rwNotLock);
	FBankName = srcTransferBankPtr->GetBankName(rwNotLock);
	srcTransferBankPtr->UnlockForRead();
	Unlock(RWLockType);
}  //CTransferBank::AssignFieldData()
//---------------------------------------------------------------------------
void CTransferBank::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String dumpMessage;
	Lock(RWLockType);
	dumpMessage.printf(L"TransferBank[%s]:[%s].", FBankCode, FBankName);
	Unlock(RWLockType);
	WriteUFCLog(dumpMessage, DoFlushToFile);
}  //CTransferBank::DumpData()

//------------------------------ CBankAccount -------------------------------
//---------------------------------------------------------------------------
CBankAccount::CBankAccount(CTransferBank *TransferBankPtr,
						   const String& BankAccount, const String& Currency, const String& BankType)
:CTradingBasicObject(constCreateRWLock)
,FTransferBankPtr(TransferBankPtr)
,FBankAccount(BankAccount)
,FCurrency(Currency)
,FBankType(BankType)
{
	FMainKey = FBankAccount;
}  //CBankAccount::CBankAccount()
//---------------------------------------------------------------------------
void CBankAccount::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	Lock(RWLockType);
	CTradingBasicObject::AssignFieldData(SrcObjPtr, rwNotLock);
	CBankAccount *srcBankAccountPtr = static_cast<CBankAccount*>(SrcObjPtr);
	srcBankAccountPtr->LockForRead();
	FTransferBankPtr = srcBankAccountPtr->GetTransferBank(rwNotLock);
	FBankAccount = srcBankAccountPtr->GetBankAccount(rwNotLock);
	FCurrency = srcBankAccountPtr->GetCurrency(rwNotLock);
	FBankType = srcBankAccountPtr->GetBankType(rwNotLock);
	srcBankAccountPtr->UnlockForRead();
	Unlock(RWLockType);
}  //CBankAccount::AssignFieldData()
//---------------------------------------------------------------------------
void CBankAccount::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String dumpMessage;
	Lock(RWLockType);
	if (FTransferBankPtr != 0)
	{
		FTransferBankPtr->LockForRead();
		String bankCode = FTransferBankPtr->GetBankCode(rwNotLock);
		String bankName = FTransferBankPtr->GetBankName(rwNotLock);
		FTransferBankPtr->UnlockForRead();
		dumpMessage.printf(L"BankAccount[%s]: Bank[%s][%s] Currency[%s] Typr[%s].", FBankAccount, bankCode, bankName, FCurrency, FBankType);
	}
	else
		dumpMessage.printf(L"BankAccount[%s]: Bank[Null] Currency[%s] Typr[%s].", FBankAccount, FCurrency, FBankType);
	Unlock(RWLockType);
	WriteUFCLog(dumpMessage, DoFlushToFile);
}  //CBankAccount::DumpData()

//--------------------------- CCTPTransferRecord ----------------------------
//---------------------------------------------------------------------------
CCTPTransferRecord::CCTPTransferRecord(int ErrorId, const String& ErrorMessage, const String& TradeCode,
									   const String& BankId, const String& BankName,
									   const String& TradeDate, const String& TradeTime,
									   const String& BankAccount, const String& InvestorId, int FutureSerial,
									   double TradeAmount, const String& Currency)
:CTradingBasicObject(constCreateRWLock)
,FErrorId(ErrorId)
,FErrorMessage(ErrorMessage)
,FTradeCode(TradeCode)
,FBankId(BankId)
,FBankName(BankName)
,FTradeDate(TradeDate)
,FTradeTime(TradeTime)
,FBankAccount(BankAccount)
,FInvestorId(InvestorId)
,FFutureSerial(FutureSerial)
,FTradeAmount(TradeAmount)
,FCurrency(Currency)
{
}  //CCTPTransferRecord::CCTPTransferRecord()
//---------------------------------------------------------------------------
String CCTPTransferRecord::GenerateLog(ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	String logStr = L"";
	if (FTradeCode == L"202001")
	{
		if (FErrorId != 0)
			logStr.printf(L"%s %s 由[%s:%s]帳號[%s]轉金額[%13.2lf]到期貨錯誤:%d[%s].", FTradeDate, FTradeTime, FBankId, FBankName, FBankAccount, FTradeAmount, FErrorId, FErrorMessage);
		else
			logStr.printf(L"%s %s 由[%s:%s]帳號[%s]轉金額[%13.2lf]到期貨成功.", FTradeDate, FTradeTime, FBankId, FBankName, FBankAccount, FTradeAmount);
	}
	else if (FTradeCode == L"202002")
	{
		if (FErrorId != 0)
			logStr.printf(L"%s %s 由期貨轉金額[%13.2lf]到[%s:%s]帳號[%s]錯誤:%d[%s].", FTradeDate, FTradeTime, FTradeAmount, FBankId, FBankName, FBankAccount, FErrorId, FErrorMessage);
		else
			logStr.printf(L"%s %s 由期貨轉金額[%13.2lf]到[%s:%s]帳號[%s]成功.", FTradeDate, FTradeTime, FTradeAmount, FBankId, FBankName, FBankAccount);
	}
	Unlock(RWLockType);
	return logStr;
}  //CCTPTransferRecord::GenerateLog()

//---------------------------- CContractPosition ----------------------------
//---------------------------------------------------------------------------
CContractPosition::CContractPosition(const String& ExchangeCode, const String& Symbol)
:CExchangeSymbolKey(constCreateRWLock, ExchangeCode, Symbol, nsOrderMessageDefine::mOTC, Symbol, Symbol)
,FBuyHoldQuantity(0)
,FBuyOrderCloseQuantity(0)
,FBuyOrderCloseTodayQuantity(0)
,FBuyFillOpenQuantity(0)
,FBuyFillCloseQuantity(0)
,FBuyFillCloseTodayQuantity(0)
,FBuyDetailHoldQuantity(0)
,FSellHoldQuantity(0)
,FSellOrderCloseQuantity(0)
,FSellOrderCloseTodayQuantity(0)
,FSellFillOpenQuantity(0)
,FSellFillCloseQuantity(0)
,FSellFillCloseTodayQuantity(0)
,FSellDetailHoldQuantity(0)
{
}//CContractPosition::CContractPosition()
//---------------------------------------------------------------------------
void CContractPosition::AddBuyHoldQuantity(int HoldQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FBuyHoldQuantity += HoldQuantity;
	Unlock(RWLockType);
}//CContractPosition::AddBuyHoldQuantity()
//---------------------------------------------------------------------------
void CContractPosition::AddSellHoldQuantity(int HoldQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FSellHoldQuantity += HoldQuantity;
	Unlock(RWLockType);
}//CContractPosition::AddSellHoldQuantity()
//---------------------------------------------------------------------------
void CContractPosition::AddBuyDetailHoldQuantity(int HoldQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FBuyDetailHoldQuantity += HoldQuantity;
	Unlock(RWLockType);
}//CContractPosition::AddBuyDetailHoldQuantity()
//---------------------------------------------------------------------------
void CContractPosition::AddSellDetailHoldQuantity(int HoldQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FSellDetailHoldQuantity += HoldQuantity;
	Unlock(RWLockType);
}//CContractPosition::AddSellDetailHoldQuantity()
//---------------------------------------------------------------------------
bool CContractPosition::AddHoldQuantity(nsOrderMessageDefine::SideEnum Side, int HoldQuantity, ReadWrietLockTypeEnum RWLockType)
{
	bool isSuccess = true;
	Lock(RWLockType);
	if (Side == nsOrderMessageDefine::sBuy)
		FBuyHoldQuantity += HoldQuantity;
	else if (Side == nsOrderMessageDefine::sSell)
		FSellHoldQuantity += HoldQuantity;
	else
		isSuccess = false;
	Unlock(RWLockType);
	return isSuccess;
}  //CContractPosition::AddHoldQuantity()
//---------------------------------------------------------------------------
int CContractPosition::GetOrderQuantity(nsOrderMessageDefine::PositionEffectEnum PositionEffect,
										nsOrderMessageDefine::SideEnum Side, ReadWrietLockTypeEnum RWLockType)
{
	int orderQuantity = 0;
	Lock(RWLockType);
	if (PositionEffect == nsOrderMessageDefine::peClose)
	{
		if (Side == nsOrderMessageDefine::sBuy)
			orderQuantity = FBuyOrderCloseQuantity;
		else if (Side == nsOrderMessageDefine::sSell)
			orderQuantity = FSellOrderCloseQuantity;
	}
	else if (PositionEffect == nsOrderMessageDefine::peDayTrade)
	{
		if (Side == nsOrderMessageDefine::sBuy)
			orderQuantity = FBuyOrderCloseTodayQuantity;
		else if (Side == nsOrderMessageDefine::sSell)
			orderQuantity = FSellOrderCloseTodayQuantity;
	}
	Unlock(RWLockType);
	return orderQuantity;
}  //CContractPosition::GetOrderQuantity()
//---------------------------------------------------------------------------
void CContractPosition::SetOrderQuantity(nsOrderMessageDefine::PositionEffectEnum PositionEffect,
										 nsOrderMessageDefine::SideEnum Side,
										 int OrderQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	if (PositionEffect == nsOrderMessageDefine::peClose)
	{
		if (Side == nsOrderMessageDefine::sBuy)
			FBuyOrderCloseQuantity = OrderQuantity;
		else if (Side == nsOrderMessageDefine::sSell)
			FSellOrderCloseQuantity = OrderQuantity;
	}
	else if (PositionEffect == nsOrderMessageDefine::peDayTrade)
	{
		if (Side == nsOrderMessageDefine::sBuy)
			FBuyOrderCloseTodayQuantity = OrderQuantity;
		else if (Side == nsOrderMessageDefine::sSell)
			FSellOrderCloseTodayQuantity = OrderQuantity;
	}
	Unlock(RWLockType);
}  //CContractPosition::SetOrderQuantity()
//---------------------------------------------------------------------------
void CContractPosition::AddBuyOrderCloseQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FBuyOrderCloseQuantity += OrderQuantity;
	Unlock(RWLockType);
}  //CContractPosition::AddBuyOrderCloseQuantity()
//---------------------------------------------------------------------------
void CContractPosition::AddBuyOrderCloseTodayQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FBuyOrderCloseTodayQuantity += OrderQuantity;
	Unlock(RWLockType);
}  //CContractPosition::AddBuyOrderCloseTodayQuantity()
//---------------------------------------------------------------------------
void CContractPosition::AddSellOrderCloseQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FSellOrderCloseQuantity += OrderQuantity;
	Unlock(RWLockType);
}  //CContractPosition::AddSellOrderCloseQuantity()
//---------------------------------------------------------------------------
void CContractPosition::AddSellOrderCloseTodayQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FSellOrderCloseTodayQuantity += OrderQuantity;
	Unlock(RWLockType);
}  //CContractPosition::AddSellOrderCloseTodayQuantity()
//---------------------------------------------------------------------------
bool CContractPosition::AddOrderQuantity(nsOrderMessageDefine::PositionEffectEnum PositionEffect,
										 nsOrderMessageDefine::SideEnum Side,
										 int OrderQuantity, ReadWrietLockTypeEnum RWLockType)
{
	bool isSuccess = true;
	Lock(RWLockType);
	if (PositionEffect == nsOrderMessageDefine::peClose)
	{
		if (Side == nsOrderMessageDefine::sBuy)
			FBuyOrderCloseQuantity += OrderQuantity;
		else if (Side == nsOrderMessageDefine::sSell)
			FSellOrderCloseQuantity += OrderQuantity;
		else
			isSuccess = false;
	}
	else if (PositionEffect == nsOrderMessageDefine::peDayTrade)
	{
		if (Side == nsOrderMessageDefine::sBuy)
			FBuyOrderCloseTodayQuantity += OrderQuantity;
		else if (Side == nsOrderMessageDefine::sSell)
			FSellOrderCloseTodayQuantity += OrderQuantity;
		else
			isSuccess = false;
	}
	else
		isSuccess = false;
	Unlock(RWLockType);
	return isSuccess;
}  //CContractPosition::AddOrderQuantity()
//---------------------------------------------------------------------------
void CContractPosition::SubstractBuyOrderCloseQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FBuyOrderCloseQuantity -= OrderQuantity;
	Unlock(RWLockType);
}  //CContractPosition::SubstractBuyOrderCloseQuantity()
//---------------------------------------------------------------------------
void CContractPosition::SubstractBuyOrderCloseTodayQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FBuyOrderCloseTodayQuantity -= OrderQuantity;
	Unlock(RWLockType);
}  //CContractPosition::SubstractBuyOrderCloseTodayQuantity()
//---------------------------------------------------------------------------
void CContractPosition::SubstractSellOrderCloseQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FSellOrderCloseQuantity -= OrderQuantity;
	Unlock(RWLockType);
}  //CContractPosition::SubstractSellOrderCloseQuantity()
//---------------------------------------------------------------------------
void CContractPosition::SubstractSellOrderCloseTodayQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FSellOrderCloseTodayQuantity -= OrderQuantity;
	Unlock(RWLockType);
}  //CContractPosition::SubstractSellOrderCloseTodayQuantity()
//---------------------------------------------------------------------------
bool CContractPosition::SubstractOrderQuantity(nsOrderMessageDefine::PositionEffectEnum PositionEffect,
											   nsOrderMessageDefine::SideEnum Side,
											   int OrderQuantity, ReadWrietLockTypeEnum RWLockType)
{
	bool isSuccess = true;
	Lock(RWLockType);
	if (PositionEffect == nsOrderMessageDefine::peClose)
	{
		if (Side == nsOrderMessageDefine::sBuy)
			FBuyOrderCloseQuantity -= OrderQuantity;
		else if (Side == nsOrderMessageDefine::sSell)
			FSellOrderCloseQuantity -= OrderQuantity;
		else
			isSuccess = false;
	}
	else if (PositionEffect == nsOrderMessageDefine::peDayTrade)
	{
		if (Side == nsOrderMessageDefine::sBuy)
			FBuyOrderCloseTodayQuantity -= OrderQuantity;
		else if (Side == nsOrderMessageDefine::sSell)
			FSellOrderCloseTodayQuantity -= OrderQuantity;
		else
			isSuccess = false;
	}
	else
		isSuccess = false;
	Unlock(RWLockType);
	return isSuccess;
}  //CContractPosition::SubstractOrderQuantity()
//---------------------------------------------------------------------------
void CContractPosition::AddBuyFillOpenQuantity(int FillQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FBuyFillOpenQuantity += FillQuantity;
	Unlock(RWLockType);
}  //CContractPosition::AddBuyFillOpenQuantity()
//---------------------------------------------------------------------------
void CContractPosition::AddBuyFillCloseQuantity(int FillQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FBuyFillCloseQuantity += FillQuantity;
	Unlock(RWLockType);
}  //CContractPosition::AddBuyFillCloseQuantity()
//---------------------------------------------------------------------------
void CContractPosition::AddBuyFillCloseTodayQuantity(int FillQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FBuyFillCloseTodayQuantity += FillQuantity;
	Unlock(RWLockType);
}  //CContractPosition::AddBuyFillCloseTodayQuantity()
//---------------------------------------------------------------------------
void CContractPosition::AddSellFillOpenQuantity(int FillQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FSellFillOpenQuantity += FillQuantity;
	Unlock(RWLockType);
}  //CContractPosition::AddSellFillOpenQuantity()
//---------------------------------------------------------------------------
void CContractPosition::AddSellFillCloseQuantity(int FillQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FSellFillCloseQuantity += FillQuantity;
	Unlock(RWLockType);
}  //CContractPosition::AddSellFillCloseQuantity()
//---------------------------------------------------------------------------
void CContractPosition::AddSellFillCloseTodayQuantity(int FillQuantity, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	FSellFillCloseTodayQuantity += FillQuantity;
	Unlock(RWLockType);
}  //CContractPosition::AddSellFillCloseTodayQuantity()
//---------------------------------------------------------------------------
bool CContractPosition::AddFillQuantity(nsOrderMessageDefine::PositionEffectEnum PositionEffect,
										nsOrderMessageDefine::SideEnum Side,
										int FillQuantity, ReadWrietLockTypeEnum RWLockType)
{
	bool isSuccess = true;
	Lock(RWLockType);
	if (PositionEffect == nsOrderMessageDefine::peOpen)
	{
		if (Side == nsOrderMessageDefine::sBuy)
			FBuyFillOpenQuantity += FillQuantity;
		else if (Side == nsOrderMessageDefine::sSell)
			FSellFillOpenQuantity += FillQuantity;
		else
			isSuccess = false;
	}
	if (PositionEffect == nsOrderMessageDefine::peClose)
	{
		if (Side == nsOrderMessageDefine::sBuy)
			FBuyFillCloseQuantity += FillQuantity;
		else if (Side == nsOrderMessageDefine::sSell)
			FSellFillCloseQuantity += FillQuantity;
		else
			isSuccess = false;
	}
	else if (PositionEffect == nsOrderMessageDefine::peDayTrade)
	{
		if (Side == nsOrderMessageDefine::sBuy)
			FBuyFillCloseTodayQuantity += FillQuantity;
		else if (Side == nsOrderMessageDefine::sSell)
			FSellFillCloseTodayQuantity += FillQuantity;
		else
			isSuccess = false;
	}
	else
		isSuccess = false;
	Unlock(RWLockType);
	return isSuccess;
}  //CContractPosition::AddFillQuantity()
//---------------------------------------------------------------------------
String CContractPosition::GenerateLogStr(nsOrderMessageDefine::SideEnum Side, ReadWrietLockTypeEnum RWLockType)
{
	String logStr = L"";
	Lock(RWLockType);
	if (Side == nsOrderMessageDefine::sBuy)
		logStr.printf(L"[%s %s]B Hold[%04d] DH[%04d], Order: C[%04d] CT[%04d], Fill: O[%04d] C[%04d] CT[%04d]",
					  FExchangeCode, FSymbol, FBuyHoldQuantity, FBuyDetailHoldQuantity, FBuyOrderCloseQuantity, FBuyOrderCloseTodayQuantity, FBuyFillOpenQuantity, FBuyFillCloseQuantity, FBuyFillCloseTodayQuantity);
	else if (Side == nsOrderMessageDefine::sSell)
		logStr.printf(L"[%s %s]S Hold[%04d] DH[%04d], Order: C[%04d] CT[%04d], Fill: O[%04d] C[%04d] CT[%04d]",
					  FExchangeCode, FSymbol, FSellHoldQuantity, FSellDetailHoldQuantity, FSellOrderCloseQuantity, FSellOrderCloseTodayQuantity, FSellFillOpenQuantity, FSellFillCloseQuantity, FSellFillCloseTodayQuantity);
	Unlock(RWLockType);
	return logStr;
}  //CContractPosition::GenerateLogStr()
//---------------------------------------------------------------------------
void CContractPosition::AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (SrcObjPtr == 0) return;
	Lock(RWLockType);
	CContractPosition::AssignFieldData(SrcObjPtr, rwNotLock);
	CContractPosition *srcContractPositionPtr = static_cast<CContractPosition*>(SrcObjPtr);
	srcContractPositionPtr->LockForRead();
	FBuyHoldQuantity = srcContractPositionPtr->GetBuyHoldQuantity(rwNotLock);
	FBuyOrderCloseQuantity = srcContractPositionPtr->GetBuyOrderCloseQuantity(rwNotLock);
	FBuyOrderCloseTodayQuantity = srcContractPositionPtr->GetBuyOrderCloseTodayQuantity(rwNotLock);
	FBuyFillOpenQuantity = srcContractPositionPtr->GetBuyFillOpenQuantity(rwNotLock);
	FBuyFillCloseQuantity = srcContractPositionPtr->GetBuyFillCloseQuantity(rwNotLock);
	FBuyFillCloseTodayQuantity = srcContractPositionPtr->GetBuyFillCloseTodayQuantity(rwNotLock);
	FBuyDetailHoldQuantity = srcContractPositionPtr->GetBuyDetailHoldQuantity(rwNotLock);
	FSellHoldQuantity = srcContractPositionPtr->GetSellHoldQuantity(rwNotLock);
	FSellOrderCloseQuantity = srcContractPositionPtr->GetSellOrderCloseQuantity(rwNotLock);
	FSellOrderCloseTodayQuantity = srcContractPositionPtr->GetSellOrderCloseTodayQuantity(rwNotLock);
	FSellFillOpenQuantity = srcContractPositionPtr->GetSellFillOpenQuantity(rwNotLock);
	FSellFillCloseQuantity = srcContractPositionPtr->GetSellFillCloseQuantity(rwNotLock);
	FSellFillCloseTodayQuantity = srcContractPositionPtr->GetSellFillCloseTodayQuantity(rwNotLock);
	FSellDetailHoldQuantity = srcContractPositionPtr->GetSellDetailHoldQuantity(rwNotLock);
	srcContractPositionPtr->UnlockForRead();
	Unlock(RWLockType);
}  //CContractPosition::AssignFieldData()
//---------------------------------------------------------------------------
void CContractPosition::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String logPrefix = L"Pos";
	Lock(RWLockType);
	String buyLogStr = CContractPosition::GenerateLogStr(nsOrderMessageDefine::sBuy, rwNotLock);
	String sellLogStr = CContractPosition::GenerateLogStr(nsOrderMessageDefine::sSell, rwNotLock);
	Unlock(RWLockType);
	WriteUFCLog(logPrefix, buyLogStr, constNotFlush);
	WriteUFCLog(logPrefix, sellLogStr, DoFlushToFile);
}  //CContractPosition::DumpData()

//----------------------------- CTradingAccount -----------------------------
//---------------------------------------------------------------------------
CTradingAccount::CTradingAccount(const String& LogonId, int AccountIndex,
								 const String& AccountName, const String& BrokerId, const String& UserKey)
:CLogonIdKey(constCreateRWLock, LogonId)
,FAccountIndex(AccountIndex)
,FAccountName(AccountName)
,FBrokerId(BrokerId)
,FUserKey(UserKey)
,FStockFutFeePtr( NULL )
,FStockOptFeePtr( NULL )
,FStockFeePtr( NULL )
,FTaifexFeeQueryStatus(qsFinished)
,FTaifexFeeQueryMessage(L"")
,FOverseasFeeQueryStatus(qsFinished)
,FOverseasFeeQueryMessage(L"")
,FTaifexHoldPositionQueryStatus(qsFinished)
,FTaifexHoldPositionQueryMessage(L"")
,FOverseasHoldPositionQueryStatus(qsFinished)
,FOverseasHoldPositionQueryMessage(L"")
{
	if( FUserKey.Length() <= 0)
		FUserKey = FLogonId;
	InitialMarketAccountArray();
}  //CTradingAccount::CTradingAccount()
//---------------------------------------------------------------------------
CTradingAccount::~CTradingAccount()
{
	Lock(rwLockForWrite);
	ClearMarketAccount(rwNotLock);
	if (FStockFutFeePtr != 0) delete FStockFutFeePtr;
	if (FStockOptFeePtr != 0) delete FStockOptFeePtr;
	if (FStockFeePtr != 0) delete FStockFeePtr;
	ClearAllFee(constDelete, rwNotLock);
	ClearAllBankAccount(constDelete, rwNotLock);
	ClearTaifexHoldPositionArray(constDelete, rwNotLock);
	ClearTWSEHoldPositionArray(constDelete, rwNotLock);
	ClearOverseasHoldPositionArray(constDelete, rwNotLock);
	ClearChinaHoldPositionArray(constDelete, rwNotLock);
	ClearAllContractPosition(constDelete, rwNotLock);
	ClearCancelStatistic(constDelete, rwNotLock);
	ClearTransferRecordArray(constDelete, rwNotLock);
	Unlock(rwLockForWrite);
}  //CTradingAccount::~CTradingAccount()
//---------------------------------------------------------------------------
void CTradingAccount::InitialMarketAccountArray()
{
	for (int marketIndex = 0; marketIndex < FMarketCount; marketIndex++)
	{
		nsOrderMessageDefine::MarketEnum curMarket = static_cast<nsOrderMessageDefine::MarketEnum>(marketIndex);
		CMarketAccount *marketAccObjPtr = new CMarketAccount(curMarket, FLogonId);
		FMarketAccountArray[marketIndex] = marketAccObjPtr;
	}
}  //CTradingAccount::InitialMarketAccountArray()
//---------------------------------------------------------------------------
CMarketAccount *CTradingAccount::FindMarketAccountObj(nsOrderMessageDefine::MarketEnum Market, ReadWrietLockTypeEnum RWLockType)
{
	CMarketAccount *marketAccountPtr = 0;
	int marketIndex = static_cast<int>(Market);
	if ((marketIndex >= 0) && (marketIndex < FMarketCount))
	{
		Lock(RWLockType);
		marketAccountPtr = FMarketAccountArray[marketIndex];
		Unlock(RWLockType);
	}
	return marketAccountPtr;
}  //CTradingAccount::FindMarketAccountObj()
//---------------------------------------------------------------------------
String CTradingAccount::FindMarketAccount(nsOrderMessageDefine::MarketEnum Market, ReadWrietLockTypeEnum RWLockType)
{
	String marketAccount = FLogonId;
	CMarketAccount *marketAccountPtr = FindMarketAccountObj(Market, RWLockType);
	if (marketAccountPtr != 0) marketAccount = marketAccountPtr->GetAccount(rwLockForRead);
	return marketAccount;
}  //CTradingAccount::FindMarketAccount()
//---------------------------------------------------------------------------
bool CTradingAccount::SetMarketAccount(nsOrderMessageDefine::MarketEnum Market, const String& NewMarketAccount, ReadWrietLockTypeEnum RWLockType)
{
	CMarketAccount *marketAccountPtr = FindMarketAccountObj(Market, RWLockType);
	if (marketAccountPtr == 0) return false;
	marketAccountPtr->SetAccount(NewMarketAccount, rwLockForWrite);
	return true;
}  //CTradingAccount::SetMarketAccount()
//---------------------------------------------------------------------------
void CTradingAccount::ClearMarketAccount(ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	for (int marketIndex = 0; marketIndex < FMarketCount; marketIndex++)
	{
		CMarketAccount *marketAccObjPtr = FMarketAccountArray[marketIndex];
		if (marketAccObjPtr != 0)
		{
			delete marketAccObjPtr;
			FMarketAccountArray[marketIndex] = 0;
		}
	}
	Unlock(RWLockType);
}  //CTradingAccount::ClearMarketAccount()
//---------------------------------------------------------------------------
void CTradingAccount::SetAllMarketAccount(const String& NewMarketAccount, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	for (int marketIndex = 0; marketIndex < FMarketCount; marketIndex++)
	{
		CMarketAccount *marketAccObjPtr = FMarketAccountArray[marketIndex];
		if (marketAccObjPtr != 0) marketAccObjPtr->SetAccount(NewMarketAccount, rwLockForWrite);
	}
	Unlock(RWLockType);
}  //CTradingAccount::SetAllMarketAccount()
//---------------------------------------------------------------------------
CFeeObject *CTradingAccount::FindFeeFromCodeIndex(const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType)
{
	if( ExchangeCode == L"TWSE" || ExchangeCode == L"OTC" )
		return FStockFeePtr;
	String keyValue = CFeeObject::BuildKey(ExchangeCode, ContractCode);
	return static_cast<CFeeObject*>(FindTradingObjFromMap(FFeeCodeIndex, keyValue, FCSLockPtr, RWLockType));
}  //CTradingAccount::FindFeeFromCodeIndex()
//---------------------------------------------------------------------------
bool CTradingAccount::InsertFeeIntoCodeIndex(const String& ExchangeCode, const String& ContractCode, CFeeObject *FeePtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CFeeObject::BuildKey(ExchangeCode, ContractCode);
	return InsertTradingObjIntoMap(FFeeCodeIndex, keyValue, FeePtr, DoCheckExist, FCSLockPtr, RWLockType);
}  //CTradingAccount::InsertFeeIntoCodeIndex()
//---------------------------------------------------------------------------
CFeeObject *CTradingAccount::RemoveFeeFromCodeIndex(const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CFeeObject::BuildKey(ExchangeCode, ContractCode);
	return static_cast<CFeeObject*>(RemoveTradingObjFromMap(FFeeCodeIndex, keyValue, FCSLockPtr, RWLockType));
}  //CTradingAccount::RemoveFeeFromCodeIndex()
//---------------------------------------------------------------------------
int CTradingAccount::ClearFeeCodeIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FFeeCodeIndex, DoDelete, FCSLockPtr, RWLockType);
}  //CTradingAccount::ClearFeeCodeIndex()
//---------------------------------------------------------------------------
CFeeObject *CTradingAccount::FindFeeFromTradeCodeIndex(const String& ExchangeCode, const String& TradeContractCode, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CFeeObject::BuildKey(ExchangeCode, TradeContractCode);
	return static_cast<CFeeObject*>(FindTradingObjFromMap(FFeeTradeCodeIndex, keyValue, FCSLockPtr, RWLockType));
}  //CTradingAccount::FindFeeFromCodeIndex()
//---------------------------------------------------------------------------
bool CTradingAccount::InsertFeeIntoTradeCodeIndex(const String& ExchangeCode, const String& TradeContractCode, CFeeObject *FeePtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CFeeObject::BuildKey(ExchangeCode, TradeContractCode);
	return InsertTradingObjIntoMap(FFeeTradeCodeIndex, keyValue, FeePtr, DoCheckExist, FCSLockPtr, RWLockType);
}  //CTradingAccount::InsertFeeIntoTradeCodeIndex()
//---------------------------------------------------------------------------
CFeeObject *CTradingAccount::RemoveFeeFromTradeCodeIndex(const String& ExchangeCode, const String& TradeContractCode, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CFeeObject::BuildKey(ExchangeCode, TradeContractCode);
	return static_cast<CFeeObject*>(RemoveTradingObjFromMap(FFeeTradeCodeIndex, keyValue, FCSLockPtr, RWLockType));
}  //CTradingAccount::RemoveFeeFromTradeCodeIndex()
//---------------------------------------------------------------------------
int CTradingAccount::ClearFeeTradeCodeIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FFeeTradeCodeIndex, DoDelete, FCSLockPtr, RWLockType);
}  //CTradingAccount::ClearFeeTradeCodeIndex()
//---------------------------------------------------------------------------
bool CTradingAccount::AddFee(CFeeObject *FeePtr, ReadWrietLockTypeEnum RWLockType)
{
	bool isSuccess = false;
	if (FeePtr == 0) return isSuccess;
	FeePtr->LockForRead();
	String exchangeCode = FeePtr->GetExchangeCode(rwNotLock);
	String contractCode = FeePtr->GetContractCode(rwNotLock);
	String tradeContractCode = FeePtr->GetTradeContractCode(rwNotLock);
	FeePtr->UnlockForRead();

	String logHead = L"CTradingAccount::AddFee()";
	String logStr;
	Lock(RWLockType);
	if (InsertFeeIntoCodeIndex(exchangeCode, contractCode, FeePtr, constCheckExist, rwNotLock))
	{
		isSuccess = true;
		if (!InsertFeeIntoTradeCodeIndex(exchangeCode, tradeContractCode, FeePtr, constCheckExist, rwNotLock))
		{
			logStr.printf(L"Insert Fee[%s-%s] into Trade Contract Code Index falied. trade[%s]", exchangeCode, contractCode, tradeContractCode);
			WriteUFCLog(logHead, logStr, constFlush);
		}
	}
	else
	{
		logStr.printf(L"Insert Fee[%s-%s] into Contract Code Index falied. trade[%s]", exchangeCode, contractCode, tradeContractCode);
		WriteUFCLog(logHead, logStr, constFlush);
	}
	Unlock(RWLockType);
	return isSuccess;
}  //CTradingAccount::AddFee()
//---------------------------------------------------------------------------
void CTradingAccount::ClearAllFee(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	ClearFeeTradeCodeIndex(constNotDelete, rwNotLock);
	ClearFeeCodeIndex(DoDelete, rwNotLock);
	Unlock(RWLockType);
}  //CTradingAccount::ClearAllFee()
//---------------------------------------------------------------------------
int CTradingAccount::LoadTestFeeData()
{
	String logHead = L"CTradingObjectsPool::LoadTestFeeData()";
	String logMsg = L"";
	logMsg.printf(L"begin... ID=%s", FLogonId);
	WriteUFCLog(logHead, logMsg, constFlush);

	int commodityCount = 0;
	String testFeeFileName = L"";
	testFeeFileName.printf(L"testFee%s.ini", FLogonId);
	UFC::AnsiString ansiTestFeeFileName = ConvertUnicodeToUFCAnsiStr(testFeeFileName);
	if (UFC::FileExists(ansiTestFeeFileName) == FALSE)
	{
		logMsg.printf(L"File[%s] not Exist.", testFeeFileName);
		WriteUFCLog(logHead, logMsg, constFlush);
		return commodityCount;
	}

	try
	{
		UFC::UiniFile testFeeFile(ansiTestFeeFileName);
		for (int i = 0; i < testFeeFile.SectionCount(); i++)
		{
			UFC::Section *sectionPtr = testFeeFile.GetSection(i);
			if (sectionPtr == 0) continue;

			//讀取節區名稱，格式為 "交易所代碼.合約代碼"，例： "TAIFEX.TXF"
			UFC::AnsiString ansiSectionName = sectionPtr->GetSectionName();
			UFC::PStringList sectionNameList;
			sectionNameList.SetStrings(ansiSectionName, ".");
			UFC::AnsiString ansiExchangeCode = sectionNameList[0];
			UFC::AnsiString ansiContractCode = sectionNameList[1];
			String exchangeCode = UTF8ToUnicodeString(ansiExchangeCode);
			String contractCode = UTF8ToUnicodeString(ansiContractCode);

			//讀取手續費資料，格式為
			//單位手續費 volume.PositionEffect=FeeValue 例： "volume.open=3"
			//價格手續費率 price.PositionEffect=FeeValue 例： "price.rolled=0.005"
			//PositionEffect可為 open(新倉), close(平倉), rolled(平今)
			double openRateByMoney = 0.0;
			double openRateByVolume = 0.0;
			double closeRateByMoney = 0.0;
			double closeRateByVolume = 0.0;
			double closeTodayRateByMoney = 0.0;
			double closeTodayRateByVolume = 0.0;
			for (int j = 0; j < sectionPtr->ItemCount(); j++)
			{
				UFC::AnsiString ansiFieldName;
				UFC::AnsiString ansiFieldValue;
				if (sectionPtr->GetNameValue(j, ansiFieldName, ansiFieldValue) == TRUE)
				{
					if (ansiFieldValue.Length() <= 0) continue;
					double feeValue = ansiFieldValue.ToDouble();
					if (ansiFieldName == "volume.open")
						openRateByVolume = feeValue;
					else if (ansiFieldName == "volume.close")
						closeRateByVolume = feeValue;
					else if (ansiFieldName == "volume.rolled")
						closeTodayRateByVolume = feeValue;
					else if (ansiFieldName == "price.open")
						openRateByMoney = feeValue;
					else if (ansiFieldName == "price.close")
						closeRateByMoney = feeValue;
					else if (ansiFieldName == "price.rolled")
						closeTodayRateByMoney = feeValue;
					else
					{
						String fieldName = UTF8ToUnicodeString(ansiFieldName);
						String fieldValue = UTF8ToUnicodeString(ansiFieldValue);
						logMsg.printf(L"Wrong Setting valus%s=%s", fieldName, fieldValue);
						WriteUFCLog(logHead, logMsg, constFlush);
					}
				}  //if (sectionPtr->GetNameValue(j, ansiContractCode, ansiFeeStr) == TRUE)
			}  //for (int j = 0; j < sectionPtr->ItemCount(); j++)

			LockForWrite();
			CFeeObject *feeObjPtr = FindFeeFromCodeIndex(exchangeCode, contractCode, rwNotLock);
			if (feeObjPtr != 0)
			{
				feeObjPtr->LockForWrite();
				if (openRateByMoney > 0.0) feeObjPtr->SetOpenRateByMoney(openRateByMoney, rwNotLock);
				if (openRateByVolume > 0.0) feeObjPtr->SetOpenRateByVolume(openRateByVolume, rwNotLock);
				if (closeRateByMoney > 0.0) feeObjPtr->SetCloseRateByMoney(closeRateByMoney, rwNotLock);
				if (closeRateByVolume > 0.0) feeObjPtr->SetCloseRateByVolume(closeRateByVolume, rwNotLock);
				if (closeTodayRateByMoney > 0.0) feeObjPtr->SetCloseTodayRateByMoney(closeTodayRateByMoney, rwNotLock);
				if (closeTodayRateByVolume > 0.0) feeObjPtr->SetCloseTodayRateByVolume(closeTodayRateByVolume, rwNotLock);
				feeObjPtr->UnlockForWrite();
			}
			else
			{
				logMsg.printf(L"Fee Object[%s-%s] not Exist", exchangeCode, contractCode);
				WriteUFCLog(logHead, logMsg, constFlush);
			}  //if (feeObjPtr != 0)
			UnlockForWrite();
			commodityCount++;
		}  //for (int i = 0; i < testFeeFile.SectionCount(); i++)
	}
	catch(UFC::FileException e)
	{
		UFC::BufferedLog::Printf(" CTradingObjectsPool::LoadTestFeeData() FileName[%s]. Error:%s", ansiTestFeeFileName.c_str(), e.what());
	}
	logMsg.printf(L"Load %d Commodities Fee Data From File %s", commodityCount, testFeeFileName.c_str());
	WriteUFCLog(logHead, logMsg, constFlush);
	return commodityCount;
}  //CTradingAccount::LoadTestFeeData()
//---------------------------------------------------------------------------
CBankAccount *CTradingAccount::FindBankAccountFromCodeIndex(const String& BankAccount, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CBankAccount*>(FindTradingObjFromMap(FBankAccountCodeIndex, BankAccount, FCSLockPtr, RWLockType));
}  //CTradingAccount::FindBankAccountFromCodeIndex()
//---------------------------------------------------------------------------
bool CTradingAccount::InsertBankAccountIntoCodeIndex(const String& BankAccount, CBankAccount *BankAccountPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	return InsertTradingObjIntoMap(FBankAccountCodeIndex, BankAccount, BankAccountPtr, DoCheckExist, FCSLockPtr, RWLockType);
}  //CTradingAccount::InsertBankAccountIntoCodeIndex()
//---------------------------------------------------------------------------
CBankAccount *CTradingAccount::RemoveBankAccountFromCodeIndex(const String& BankAccount, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CBankAccount*>(RemoveTradingObjFromMap(FBankAccountCodeIndex, BankAccount, FCSLockPtr, RWLockType));
}  //CTradingAccount::RemoveBankAccountFromCodeIndex()
//---------------------------------------------------------------------------
int CTradingAccount::ClearBankAccountCodeIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FBankAccountCodeIndex, DoDelete, FCSLockPtr, RWLockType);
}  //CTradingAccount::ClearBankAccountCodeIndex()
//---------------------------------------------------------------------------
CBankAccount *CTradingAccount::GetBankAccountFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CBankAccount*>(GetTradingObjFromVector(FBankAccountArray, ArrayIndex, FCSLockPtr, RWLockType));
}  //CTradingAccount::GetBankAccountFromArray()
//---------------------------------------------------------------------------
bool CTradingAccount::AppendBankAccountToArray(CBankAccount *BankAccountPtr, ReadWrietLockTypeEnum RWLockType)
{
	return AppendTradingObjToVector(FBankAccountArray, BankAccountPtr, FCSLockPtr, RWLockType);
}  //CTradingAccount::AppendBankAccountToArray()
//---------------------------------------------------------------------------
int CTradingAccount::ClearBankAccountArray(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjVector(FBankAccountArray, DoDelete, FCSLockPtr, RWLockType);
}  //CTradingAccount::ClearBankAccountArray()
//---------------------------------------------------------------------------
bool CTradingAccount::AddBankAccount(CBankAccount *BankAccountPtr, ReadWrietLockTypeEnum RWLockType)
{
	bool isSuccess = false;
	if (BankAccountPtr == 0) return isSuccess;
	String bankAccount = BankAccountPtr->GetBankAccount(rwLockForRead);
	String logHead = L"CTradingAccount::AddBankAccount()";
	String logStr;
	Lock(RWLockType);
	if (InsertBankAccountIntoCodeIndex(bankAccount, BankAccountPtr, constCheckExist, rwNotLock))
	{
		isSuccess = true;
		if (!AppendBankAccountToArray(BankAccountPtr, rwNotLock))
		{
			logStr.printf(L"Append BankAccount[%s] into Array falied.", bankAccount);
			WriteUFCLog(logHead, logStr, constFlush);
		}
	}
	else
	{
		logStr.printf(L"Insert BankAccount[%s] into Code Index falied", bankAccount);
		WriteUFCLog(logHead, logStr, constFlush);
	}
	Unlock(RWLockType);
	return isSuccess;
}  //CTradingAccount::AddBankAccount()
//---------------------------------------------------------------------------
void CTradingAccount::ClearAllBankAccount(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	ClearBankAccountArray(constNotDelete, rwNotLock);
	ClearBankAccountCodeIndex(DoDelete, rwNotLock);
	Unlock(RWLockType);
}  //CTradingAccount::ClearAllBankAccount()
//---------------------------------------------------------------------------
void CTradingAccount::DumpBankAccount(ReadWrietLockTypeEnum RWLockType)
{
	DumpTradingObjMap(FBankAccountCodeIndex, constFlush, FCSLockPtr, rwNotLock);
}  //CTradingAccount::DumpBankAccount()
//---------------------------------------------------------------------------
CHoldPositionData *CTradingAccount::GetTaifexHoldPositionFromArray(int ArrayIndex,ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CHoldPositionData*>(GetTradingObjFromVector(FTaifexHoldPositionArray, ArrayIndex, FCSLockPtr, RWLockType));
}  //CTradingAccount::GetTaifexHoldPositionFromArray()
//---------------------------------------------------------------------------
bool CTradingAccount::AppendTaifexHoldPositionToArray(CHoldPositionData *HoldPositionPtr, ReadWrietLockTypeEnum RWLockType)
{
	return AppendTradingObjToVector(FTaifexHoldPositionArray, HoldPositionPtr, FCSLockPtr, RWLockType);
}  //CTradingAccount::AppendTaifexHoldPositionToArray()
//---------------------------------------------------------------------------
int CTradingAccount::ClearTaifexHoldPositionArray(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjVector(FTaifexHoldPositionArray, DoDelete, FCSLockPtr, RWLockType);
}  //CTradingAccount::ClearTaifexHoldPositionArray()

//---------------------------------------------------------------------------
CHoldPositionData *CTradingAccount::GetTWSEHoldPositionFromArray(int ArrayIndex,ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CHoldPositionData*>(GetTradingObjFromVector(FTWSEHoldPositionArray, ArrayIndex, FCSLockPtr, RWLockType));
}
//---------------------------------------------------------------------------
bool CTradingAccount::AppendTWSEHoldPositionToArray(CHoldPositionData *HoldPositionPtr, ReadWrietLockTypeEnum RWLockType)
{
	return AppendTradingObjToVector(FTWSEHoldPositionArray, HoldPositionPtr, FCSLockPtr, RWLockType);
}
//---------------------------------------------------------------------------
int CTradingAccount::ClearTWSEHoldPositionArray(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjVector(FTWSEHoldPositionArray, DoDelete, FCSLockPtr, RWLockType);
}
//---------------------------------------------------------------------------
CHoldPositionData *CTradingAccount::GetOverseasHoldPositionFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CHoldPositionData*>(GetTradingObjFromVector(FOverseasHoldPositionArray, ArrayIndex, FCSLockPtr, RWLockType));
}  //CTradingAccount::GetOverseasHoldPositionFromArray()
//---------------------------------------------------------------------------
bool CTradingAccount::AppendOverseasHoldPositionToArray(CHoldPositionData *HoldPositionPtr, ReadWrietLockTypeEnum RWLockType)
{
	return AppendTradingObjToVector(FOverseasHoldPositionArray, HoldPositionPtr, FCSLockPtr, RWLockType);
}  //CTradingAccount::AppendOverseasHoldPositionToArray()
//---------------------------------------------------------------------------
int CTradingAccount::ClearOverseasHoldPositionArray(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjVector(FOverseasHoldPositionArray, DoDelete, FCSLockPtr, RWLockType);
}  //CTradingAccount::ClearOverseasHoldPositionArray()
//---------------------------------------------------------------------------
CHoldPositionData *CTradingAccount::GetChinaHoldPositionFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CHoldPositionData*>(GetTradingObjFromVector(FChinaHoldPositionArray, ArrayIndex, FCSLockPtr, RWLockType));
}  //CTradingAccount::GetChinaHoldPositionFromArray()
//---------------------------------------------------------------------------
bool CTradingAccount::AppendChinaHoldPositionToArray(CHoldPositionData *HoldPositionPtr, ReadWrietLockTypeEnum RWLockType)
{
	return AppendTradingObjToVector(FChinaHoldPositionArray, HoldPositionPtr, FCSLockPtr, RWLockType);
}  //CTradingAccount::AppendChinaHoldPositionToArray()
//---------------------------------------------------------------------------
int CTradingAccount::ClearChinaHoldPositionArray(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjVector(FChinaHoldPositionArray, DoDelete, FCSLockPtr, RWLockType);
}  //CTradingAccount::ClearChinaHoldPositionArray()
//---------------------------------------------------------------------------
CContractPosition *CTradingAccount::FindContractPositionFromSymbolIndex(const String& ExchangeCode, const String& Symbol, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CContractPosition::BuildKey(ExchangeCode, Symbol);
	return static_cast<CContractPosition*>(FindTradingObjFromMap(FContractPositionSymbolIndex, keyValue, FCSLockPtr, RWLockType));
}  //CTradingAccount::FindContractPositionFromSymbolIndex()
//---------------------------------------------------------------------------
bool CTradingAccount::InsertContractPositionIntoSymbolIndex(const String& ExchangeCode, const String& Symbol, CContractPosition *ContractPositionPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CContractPosition::BuildKey(ExchangeCode, Symbol);
	return InsertTradingObjIntoMap(FContractPositionSymbolIndex, keyValue, ContractPositionPtr, DoCheckExist, FCSLockPtr, RWLockType);
}  //CTradingAccount::InsertContractPositionIntoSymbolIndex()
//---------------------------------------------------------------------------
int CTradingAccount::ClearContractPositionSymbolIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FContractPositionSymbolIndex, DoDelete, FCSLockPtr, RWLockType);
}  //CTradingAccount::ClearContractPositionSymbolIndex()
//---------------------------------------------------------------------------
CContractPosition *CTradingAccount::GetContractPositionFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CContractPosition*>(GetTradingObjFromVector(FContractPositionArray, ArrayIndex, FCSLockPtr, RWLockType));
}  //CTradingAccount::GetContractPositionFromArray()
//---------------------------------------------------------------------------
bool CTradingAccount::AppendContractPositionToArray(CContractPosition *ContractPositionPtr, ReadWrietLockTypeEnum RWLockType)
{
	return AppendTradingObjToVector(FContractPositionArray, ContractPositionPtr, FCSLockPtr, RWLockType);
}  //CTradingAccount::AppendContractPositionToArray()
//---------------------------------------------------------------------------
int CTradingAccount::ClearContractPositionArray(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjVector(FContractPositionArray, DoDelete, FCSLockPtr, RWLockType);
}  //CTradingAccount::ClearContractPositionArray()
//---------------------------------------------------------------------------
CContractPosition *CTradingAccount::FindOrCreateContractPositionSymbolIndex(const String& ExchangeCode, const String& Symbol, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	CContractPosition *positionPtr = FindContractPositionFromSymbolIndex(ExchangeCode, Symbol, rwNotLock);
	if (positionPtr == 0)
	{
		positionPtr = new s888::CContractPosition(ExchangeCode, Symbol);
		if (InsertContractPositionIntoSymbolIndex(ExchangeCode, Symbol, positionPtr, constNotCheckExist, rwNotLock))
			AppendContractPositionToArray(positionPtr, rwNotLock);
		else
		{
			String logHead = L"CTradingAccount::FindOrCreateContractPositionSymbolIndex()";
			String logStr;
			logStr.printf(L"Insert ContractPosition[%s-%s] into Symbol Index falied.", ExchangeCode, Symbol);
			WriteUFCLog(logHead, logStr, constFlush);
			delete positionPtr;
			positionPtr = 0;
		}
	}  //if (positionPtr == 0)
	Unlock(RWLockType);
	return positionPtr;
}  //CTradingAccount::FindOrCreateContractPositionSymbolIndex()
//---------------------------------------------------------------------------
void CTradingAccount::ClearAllContractPosition(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	ClearContractPositionArray(constNotDelete, rwNotLock);
	ClearContractPositionSymbolIndex(DoDelete, rwNotLock);
	Unlock(RWLockType);
}  //CTradingAccount::ClearAllContractPosition()
//---------------------------------------------------------------------------
CCommodityCancelStatistic *CTradingAccount::FindCommodityCancelStatisticFromCodeIndex(const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CCommodityCancelStatistic::BuildKey(ExchangeCode, ContractCode);
	return static_cast<CCommodityCancelStatistic*>(FindTradingObjFromMap(FCommodityCancelStatisticCodeIndex, keyValue, FCSLockPtr, RWLockType));
}  //CTradingAccount::FindCommodityCancelStatisticFromCodeIndex()
//---------------------------------------------------------------------------
bool CTradingAccount::InsertCommodityCancelStatisticIntoCodeIndex(const String& ExchangeCode, const String& ContractCode, CCommodityCancelStatistic *CancelStatisticPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CCommodityCancelStatistic::BuildKey(ExchangeCode, ContractCode);
	return InsertTradingObjIntoMap(FCommodityCancelStatisticCodeIndex, keyValue, CancelStatisticPtr, DoCheckExist, FCSLockPtr, RWLockType);
}  //CTradingAccount::InsertCommodityCancelStatisticIntoCodeIndex()
//---------------------------------------------------------------------------
int CTradingAccount::ClearCommodityCancelStatisticCodeIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FCommodityCancelStatisticCodeIndex, DoDelete, FCSLockPtr, RWLockType);
}  //CTradingAccount::ClearCommodityCancelStatisticCodeIndex()
//---------------------------------------------------------------------------
CCommodityCancelStatistic *CTradingAccount::GetCommodityCancelStatisticFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CCommodityCancelStatistic*>(GetTradingObjFromVector(FCommodityCancelStatisticArray, ArrayIndex, FCSLockPtr, RWLockType));
}  //CTradingAccount::GetCommodityCancelStatisticFromArray()
//---------------------------------------------------------------------------
bool CTradingAccount::AppendCommodityCancelStatisticToArray(CCommodityCancelStatistic *CancelStatisticPtr, ReadWrietLockTypeEnum RWLockType)
{
	return AppendTradingObjToVector(FCommodityCancelStatisticArray, CancelStatisticPtr, FCSLockPtr, RWLockType);
}  //CTradingAccount::AppendCommodityCancelStatisticToArray()
//---------------------------------------------------------------------------
int CTradingAccount::ClearCommodityCancelStatisticArray(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjVector(FCommodityCancelStatisticArray, DoDelete, FCSLockPtr, RWLockType);
}  //CTradingAccount::ClearCommodityCancelStatisticArray()
//---------------------------------------------------------------------------
bool CTradingAccount::AddCommodityCancelStatistic(const String& ExchangeCode, const String& ContractCode, int CancelQuantity, int CancelCount, ReadWrietLockTypeEnum RWLockType)
{
	bool isSuccess = false;
	Lock(RWLockType);
	CCommodityCancelStatistic *cancelStatisticPtr = FindCommodityCancelStatisticFromCodeIndex(ExchangeCode, ContractCode, rwNotLock);
	if (cancelStatisticPtr == 0)
	{
		cancelStatisticPtr = new CCommodityCancelStatistic(ExchangeCode, ContractCode);
		if (InsertCommodityCancelStatisticIntoCodeIndex(ExchangeCode, ContractCode, cancelStatisticPtr, constNotCheckExist, rwNotLock))
			AppendCommodityCancelStatisticToArray(cancelStatisticPtr, rwNotLock);
		else
		{
			String logHead = L"CTradingAccount::AddCommodityCancelStatistic()";
			String logStr;
			logStr.printf(L"Insert CommodityCancelStatistic[%s-%s] into Code Index falied. Quantity[%d], Count[%d]", ExchangeCode, ContractCode, CancelQuantity, CancelCount);
			WriteUFCLog(logHead, logStr, constFlush);
			delete cancelStatisticPtr;
			cancelStatisticPtr = 0;
		}
	}
	Unlock(RWLockType);

	if (cancelStatisticPtr != 0)
	{
		cancelStatisticPtr->LockForWrite();
		cancelStatisticPtr->IncreaseCancelQuantity(CancelQuantity, rwNotLock);
		cancelStatisticPtr->IncreaseCancelReportCount(CancelCount, rwNotLock);
		cancelStatisticPtr->UnlockForWrite();
		isSuccess = true;
	}
	return isSuccess;
}  //CTradingAccount::AddCommodityCancelStatistic()
//---------------------------------------------------------------------------
void CTradingAccount::DumpCommodityCancelStatistic(ReadWrietLockTypeEnum RWLockType)
{
	DumpTradingObjMap(FCommodityCancelStatisticCodeIndex, constFlush, FCSLockPtr, rwNotLock);
}  //CTradingAccount::DumpCommodityCancelStatistic()
//---------------------------------------------------------------------------
CContractCancelStatistic *CTradingAccount::FindContractCancelStatisticFromSymbolIndex(const String& ExchangeCode, const String& Symbol, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CContractCancelStatistic::BuildKey(ExchangeCode, Symbol);
	return static_cast<CContractCancelStatistic*>(FindTradingObjFromMap(FContractCancelStatisticSymbolIndex, keyValue, FCSLockPtr, RWLockType));
}  //CTradingAccount::FindContractCancelStatisticFromSymbolIndex()
//---------------------------------------------------------------------------
bool CTradingAccount::InsertContractCancelStatisticIntoSymbolIndex(const String& ExchangeCode, const String& Symbol, CContractCancelStatistic *CancelStatisticPtr, bool DoCheckExist, ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = CContractCancelStatistic::BuildKey(ExchangeCode, Symbol);
	return InsertTradingObjIntoMap(FContractCancelStatisticSymbolIndex, keyValue, CancelStatisticPtr, DoCheckExist, FCSLockPtr, RWLockType);
}  //CTradingAccount::InsertContractCancelStatisticIntoSymbolIndex()
//---------------------------------------------------------------------------
int CTradingAccount::ClearContractCancelStatisticSymbolIndex(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjMap(FContractCancelStatisticSymbolIndex, DoDelete, FCSLockPtr, RWLockType);
}  //CTradingAccount::ClearContractCancelStatisticSymbolIndex()
//---------------------------------------------------------------------------
CContractCancelStatistic *CTradingAccount::GetContractCancelStatisticFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CContractCancelStatistic*>(GetTradingObjFromVector(FContractCancelStatisticArray, ArrayIndex, FCSLockPtr, RWLockType));
}  //CTradingAccount::GetContractCancelStatisticFromArray()
//---------------------------------------------------------------------------
bool CTradingAccount::AppendContractCancelStatisticToArray(CContractCancelStatistic *CancelStatisticPtr, ReadWrietLockTypeEnum RWLockType)
{
	return AppendTradingObjToVector(FContractCancelStatisticArray, CancelStatisticPtr, FCSLockPtr, RWLockType);
}  //CTradingAccount::AppendContractCancelStatisticToArray()
//---------------------------------------------------------------------------
int CTradingAccount::ClearContractCancelStatisticArray(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjVector(FContractCancelStatisticArray, DoDelete, FCSLockPtr, RWLockType);
}  //CTradingAccount::ClearContractCancelStatisticArray()
//---------------------------------------------------------------------------
bool CTradingAccount::AddContractCancelStatistic(const String& ExchangeCode, const String& Symbol, int CancelQuantity, int CancelCount, ReadWrietLockTypeEnum RWLockType)
{
	bool isSuccess = false;
	Lock(RWLockType);
	CContractCancelStatistic *cancelStatisticPtr = FindContractCancelStatisticFromSymbolIndex(ExchangeCode, Symbol, rwNotLock);
	if (cancelStatisticPtr == 0)
	{
		cancelStatisticPtr = new CContractCancelStatistic(ExchangeCode, Symbol);
		if (InsertContractCancelStatisticIntoSymbolIndex(ExchangeCode, Symbol, cancelStatisticPtr, constNotCheckExist, rwNotLock))
			AppendContractCancelStatisticToArray(cancelStatisticPtr, rwNotLock);
		else
		{
			String logHead = L"CTradingAccount::AddContractCancelStatistic()";
			String logStr;
			logStr.printf(L"Insert AddContractCancelStatistic[%s-%s] into Symbol Index falied. Quantity[%d], Count[%d]", ExchangeCode, Symbol, CancelQuantity, CancelCount);
			WriteUFCLog(logHead, logStr, constFlush);
			delete cancelStatisticPtr;
			cancelStatisticPtr = 0;
		}
	}
	Unlock(RWLockType);

	if (cancelStatisticPtr != 0)
	{
		cancelStatisticPtr->LockForWrite();
		cancelStatisticPtr->IncreaseCancelQuantity(CancelQuantity, rwNotLock);
		cancelStatisticPtr->IncreaseCancelReportCount(CancelCount, rwNotLock);
		cancelStatisticPtr->UnlockForWrite();
		isSuccess = true;
	}
	return isSuccess;
}  //CTradingAccount::AddContractCancelStatistic()
//---------------------------------------------------------------------------
void CTradingAccount::ClearCancelStatistic(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	Lock(RWLockType);
	ClearCommodityCancelStatisticArray(constNotDelete, rwNotLock);
	ClearCommodityCancelStatisticCodeIndex(DoDelete, rwNotLock);
	ClearContractCancelStatisticArray(constNotDelete, rwNotLock);
	ClearContractCancelStatisticSymbolIndex(DoDelete, rwNotLock);
	Unlock(RWLockType);
}  //CTradingAccount::ClearCancelStatistic()
//---------------------------------------------------------------------------
void CTradingAccount::DumpContractCancelStatistic(ReadWrietLockTypeEnum RWLockType)
{
	DumpTradingObjMap(FContractCancelStatisticSymbolIndex, constFlush, FCSLockPtr, rwNotLock);
}  //CTradingAccount::DumpContractCancelStatistic()
//---------------------------------------------------------------------------
CCTPTransferRecord *CTradingAccount::GetTransferRecordFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<CCTPTransferRecord*>(GetTradingObjFromVector(FTransferRecordArray, ArrayIndex, FCSLockPtr, RWLockType));
}  //CTradingAccount::GetTransferRecordFromArray()
//---------------------------------------------------------------------------
bool CTradingAccount::AppendTransferRecordToArray(CCTPTransferRecord *TransferRecordPtr, ReadWrietLockTypeEnum RWLockType)
{
	return AppendTradingObjToVector(FTransferRecordArray, TransferRecordPtr, FCSLockPtr, RWLockType);
}  //CTradingAccount::AppendTransferRecordToArray()
//---------------------------------------------------------------------------
int CTradingAccount::ClearTransferRecordArray(bool DoDelete, ReadWrietLockTypeEnum RWLockType)
{
	return ClearTradingObjVector(FTransferRecordArray, DoDelete, FCSLockPtr, RWLockType);
}  //CTradingAccount::ClearTransferRecordArray()
//---------------------------------------------------------------------------
void CTradingAccount::DumpData(bool DoFlushToFile, ReadWrietLockTypeEnum RWLockType)
{
	String dumpMessage;
	Lock(RWLockType);
	dumpMessage.printf(L"TradingAccount: LogonID[%s] Name[%s] BrokerID[%s] UserKey[%s].", FLogonId, FAccountName, FBrokerId, FUserKey);
	WriteUFCLog(dumpMessage, constNotFlush);

	for (int marketAccountIndex = 0; marketAccountIndex < FMarketCount; marketAccountIndex++)
		FMarketAccountArray[marketAccountIndex]->DumpData(constNotFlush, rwLockForRead);

	dumpMessage.printf(L"[%s]: %d Taifex Hold Position Data.", FLogonId, FTaifexHoldPositionArray.size());
	WriteUFCLog(dumpMessage, constNotFlush);
	for (unsigned int i = 0; i < FTaifexHoldPositionArray.size(); i++)
	{
		CHoldPositionData *holdPosPtr = static_cast<CHoldPositionData*>(FTaifexHoldPositionArray[i]);
		holdPosPtr->DumpData(constNotFlush, rwLockForRead);
	}

	dumpMessage.printf(L"[%s]: %d Overseas Hold Position Data.", FLogonId, FOverseasHoldPositionArray.size());
	WriteUFCLog(dumpMessage, constNotFlush);
	for (unsigned int i = 0; i < FOverseasHoldPositionArray.size(); i++)
	{
		CHoldPositionData *holdPosPtr = static_cast<CHoldPositionData*>(FOverseasHoldPositionArray[i]);
		holdPosPtr->DumpData(constNotFlush, rwLockForRead);
	}

	dumpMessage.printf(L"[%s]: %d Fee Data.", FLogonId, FFeeCodeIndex.size());
	CWStrTradingObjPtrHMap::iterator feeIt;
	for (feeIt = FFeeCodeIndex.begin(); feeIt != FFeeCodeIndex.end(); feeIt++)
	{
		CTradingBasicObject *tradingObjPtr = feeIt->second;
		CFeeObject *curFeePtr = static_cast<CFeeObject*>(tradingObjPtr);
		curFeePtr->DumpData(constNotFlush, rwLockForRead);
	}
	Unlock(RWLockType);
	if (DoFlushToFile) FlushUFCLog();
}  //CTradingAccount::DumpData()
};  //namespace s888

#pragma package(smart_init)
