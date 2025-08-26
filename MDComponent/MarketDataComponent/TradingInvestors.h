//---------------------------------------------------------------------------

#ifndef TradingInvestorsH
#define TradingInvestorsH
#include "OrderMessageDefine.h"
#include "TradingUtility.h"
#include "TradingBasicObjects.h"
#include "TradingCommodities.h"
//---------------------------------------------------------------------------
namespace s888
{
//---------------------------------------------------------------------------
class CMarketAccount : public CTradingBasicObject
{
public:
	static String BuildKey(nsOrderMessageDefine::MarketEnum Market);

protected:
	nsOrderMessageDefine::MarketEnum FMarket;
	String                           FAccount;

public:
	//Constructor
	CMarketAccount(nsOrderMessageDefine::MarketEnum Market, const String& Account);

	//Get/Set Field Function
	nsOrderMessageDefine::MarketEnum GetMarket(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FMarket, RWLockType);};

	String GetAccount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FAccount, RWLockType);};
	virtual void SetAccount(const String& Account, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(Account, FAccount, RWLockType);};

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CMarketAccount

//----------------------------------------------------------------------------
class CTransferBank : public CTradingBasicObject
{
protected:
	String FBankCode;
	String FBankName;

public:
	//Constructor
	CTransferBank(const String& BankCode, const String& BankName = L"");

	//Get/Set Field Function
	String GetBankCode(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBankCode, RWLockType);};

	String GetBankName(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBankName, RWLockType);};
	virtual void SetBankName(const String& BankName, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BankName, FBankName, RWLockType);};

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CTransferBank

//----------------------------------------------------------------------------
class CBankAccount : public CTradingBasicObject
{
protected:
	CTransferBank *FTransferBankPtr;
	String        FBankAccount;
	String        FCurrency;
	String        FBankType;  //Z, T, Q

public:
	//Constructor
	CBankAccount(CTransferBank *TransferBankPtr,
				 const String& BankAccount, const String& Currency, const String& BankType);

	//Get/Set Field Function
	CTransferBank *GetTransferBank(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTransferBankPtr, RWLockType);};

	String GetBankAccount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBankAccount, RWLockType);};

	String GetCurrency(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCurrency, RWLockType);};

	String GetBankType(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBankType, RWLockType);};

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CBankAccount

//----------------------------------------------------------------------------
class CCTPTransferRecord : public CTradingBasicObject
{
protected:
	int FErrorId;               //錯誤代碼
	String FErrorMessage;       //錯誤訊息
	String FTradeCode;          //業務功能碼
	String FBankId;             //銀行代碼
	String FBankName;           //銀行名稱
	String FTradeDate;          //交易日期
	String FTradeTime;          //交易時間
	String FBankAccount;        //銀行帳號
	String FInvestorId;         //投資者帳號
	int    FFutureSerial;       //期貨公司流水號
	double FTradeAmount;	    //轉帳金額
	String FCurrency;           //幣種代碼

public:
	//Constructor
	CCTPTransferRecord(int ErrorId, const String& ErrorMessage, const String& TradeCode,
					   const String& BankId, const String& BankName,
					   const String& TradeDate, const String& TradeTime,
					   const String& BankAccount, const String& InvestorId, int FutureSerial,
					   double TradeAmount, const String& Currency);

	//Get/Set Field Function
	int GetErrorId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FErrorId, RWLockType);};

	String GetErrorMessage(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FErrorMessage, RWLockType);};

	String GetTradeCode(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTradeCode, RWLockType);};

	String GetBankId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBankId, RWLockType);};

	String GetBankName(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBankName, RWLockType);};
	virtual void SetBankName(const String& BankName, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BankName, FBankName, RWLockType);};

	String GetTradeDate(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTradeDate, RWLockType);};

	String GetTradeTime(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTradeTime, RWLockType);};

	String GetBankAccount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBankAccount, RWLockType);};

	String GetInvestorId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FInvestorId, RWLockType);};

	int GetFutureSerial(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FFutureSerial, RWLockType);};

	double GetTradeAmount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTradeAmount, RWLockType);};

	String GetCurrency(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FCurrency, RWLockType);};

	String GenerateLog(ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CCTPTransferRecord

//----------------------------------------------------------------------------
class CContractPosition : public CExchangeSymbolKey
{
protected:
	//Buy Side Position
	int FBuyHoldQuantity;
	int FBuyOrderCloseQuantity;
	int FBuyOrderCloseTodayQuantity;
	int FBuyFillOpenQuantity;
	int FBuyFillCloseQuantity;
	int FBuyFillCloseTodayQuantity;
	int FBuyDetailHoldQuantity;

	//Sell Side Position
	int FSellHoldQuantity;
	int FSellOrderCloseQuantity;
	int FSellOrderCloseTodayQuantity;
	int FSellFillOpenQuantity;
	int FSellFillCloseQuantity;
	int FSellFillCloseTodayQuantity;
	int FSellDetailHoldQuantity;

public:
	//Constructor
	CContractPosition(const String& ExchangeCode, const String& Symbol);

	//Get/Set Field Function
	int GetBuyHoldQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyHoldQuantity, RWLockType);};
	virtual void SetBuyHoldQuantity(int BuyHoldQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BuyHoldQuantity, FBuyHoldQuantity, RWLockType);};

	int GetBuyOrderCloseQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyOrderCloseQuantity, RWLockType);};
	virtual void SetBuyOrderCloseQuantity(int BuyOrderCloseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BuyOrderCloseQuantity, FBuyOrderCloseQuantity, RWLockType);};

	int GetBuyOrderCloseTodayQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyOrderCloseTodayQuantity, RWLockType);};
	virtual void SetBuyOrderCloseTodayQuantity(int BuyOrderCloseTodayQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BuyOrderCloseTodayQuantity, FBuyOrderCloseTodayQuantity, RWLockType);};

	int GetBuyFillOpenQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyFillOpenQuantity, RWLockType);};
	virtual void SetBuyFillOpenQuantity(int BuyFillOpenQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BuyFillOpenQuantity, FBuyFillOpenQuantity, RWLockType);};

	int GetBuyFillCloseQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyFillCloseQuantity, RWLockType);};
	virtual void SetBuyFillCloseQuantity(int BuyFillCloseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BuyFillCloseQuantity, FBuyFillCloseQuantity, RWLockType);};

	int GetBuyFillCloseTodayQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyFillCloseTodayQuantity, RWLockType);};
	virtual void SetBuyFillCloseTodayQuantity(int BuyFillCloseTodayQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BuyFillCloseTodayQuantity, FBuyFillCloseTodayQuantity, RWLockType);};

	int GetBuyDetailHoldQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBuyDetailHoldQuantity, RWLockType);};
	virtual void SetBuyDetailHoldQuantity(int BuyDetailHoldQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BuyDetailHoldQuantity, FBuyDetailHoldQuantity, RWLockType);};

	int GetSellHoldQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellHoldQuantity, RWLockType);};
	virtual void SetSellHoldQuantity(int SellHoldQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(SellHoldQuantity, FSellHoldQuantity, RWLockType);};

	int GetSellOrderCloseQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellOrderCloseQuantity, RWLockType);};
	virtual void SetSellOrderCloseQuantity(int SellOrderCloseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(SellOrderCloseQuantity, FSellOrderCloseQuantity, RWLockType);};

	int GetSellOrderCloseTodayQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellOrderCloseTodayQuantity, RWLockType);};
	virtual void SetSellOrderCloseTodayQuantity(int SellOrderCloseTodayQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(SellOrderCloseTodayQuantity, FSellOrderCloseTodayQuantity, RWLockType);};

	int GetSellFillOpenQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellFillOpenQuantity, RWLockType);};
	virtual void SetSellFillOpenQuantity(int SellFillOpenQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(SellFillOpenQuantity, FSellFillOpenQuantity, RWLockType);};

	int GetSellFillCloseQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellFillCloseQuantity, RWLockType);};
	virtual void SetSellFillCloseQuantity(int SellFillCloseQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(SellFillCloseQuantity, FSellFillCloseQuantity, RWLockType);};

	int GetSellFillCloseTodayQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellFillCloseTodayQuantity, RWLockType);};
	virtual void SetSellFillCloseTodayQuantity(int SellFillCloseTodayQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(SellFillCloseTodayQuantity, FSellFillCloseTodayQuantity, RWLockType);};

	int GetSellDetailHoldQuantity(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FSellDetailHoldQuantity, RWLockType);};
	virtual void SetSellDetailHoldQuantity(int SellDetailHoldQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(SellDetailHoldQuantity, FSellDetailHoldQuantity, RWLockType);};

	void AddBuyHoldQuantity(int HoldQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void AddSellHoldQuantity(int HoldQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void AddBuyDetailHoldQuantity(int HoldQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void AddSellDetailHoldQuantity(int HoldQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AddHoldQuantity(nsOrderMessageDefine::SideEnum Side, int HoldQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	int GetOrderQuantity(nsOrderMessageDefine::PositionEffectEnum PositionEffect,
						 nsOrderMessageDefine::SideEnum Side, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void SetOrderQuantity(nsOrderMessageDefine::PositionEffectEnum PositionEffect,
						  nsOrderMessageDefine::SideEnum Side,
						  int OrderQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	void AddBuyOrderCloseQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void AddBuyOrderCloseTodayQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void AddSellOrderCloseQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void AddSellOrderCloseTodayQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AddOrderQuantity(nsOrderMessageDefine::PositionEffectEnum PositionEffect,
						  nsOrderMessageDefine::SideEnum Side,
						  int OrderQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	void SubstractBuyOrderCloseQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void SubstractBuyOrderCloseTodayQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void SubstractSellOrderCloseQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void SubstractSellOrderCloseTodayQuantity(int OrderQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool SubstractOrderQuantity(nsOrderMessageDefine::PositionEffectEnum PositionEffect,
								nsOrderMessageDefine::SideEnum Side,
								int OrderQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	void AddBuyFillOpenQuantity(int FillQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void AddBuyFillCloseQuantity(int FillQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void AddBuyFillCloseTodayQuantity(int FillQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void AddSellFillOpenQuantity(int FillQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void AddSellFillCloseQuantity(int FillQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void AddSellFillCloseTodayQuantity(int FillQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AddFillQuantity(nsOrderMessageDefine::PositionEffectEnum PositionEffect,
						 nsOrderMessageDefine::SideEnum Side,
						 int FillQuantity, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	String GenerateLogStr(nsOrderMessageDefine::SideEnum Side, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	virtual void AssignFieldData(CTradingBasicObject *SrcObjPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //ContractPosition

//----------------------------------------------------------------------------
class CTradingAccount : public CLogonIdKey
{
private:
	static const int FMarketCount = 8;

protected:
	int    FAccountIndex;  //帳號在g_config中的索引號
	String FAccountName;
	String FBrokerId;
	String FUserKey;

	CMarketAccount *FMarketAccountArray[FMarketCount];

	QueryStatusEnum        FTaifexFeeQueryStatus;
	String                 FTaifexFeeQueryMessage;
	QueryStatusEnum        FOverseasFeeQueryStatus;
	String                 FOverseasFeeQueryMessage;
	CFeeObject             *FStockFutFeePtr;
	CFeeObject             *FStockOptFeePtr;
	CFeeObject             *FStockFeePtr;
	CWStrTradingObjPtrHMap FFeeCodeIndex;
	CWStrTradingObjPtrHMap FFeeTradeCodeIndex;

	CWStrTradingObjPtrHMap FBankAccountCodeIndex;
	CTradingObjPtrVector   FBankAccountArray;

	QueryStatusEnum      FTaifexHoldPositionQueryStatus;
	String               FTaifexHoldPositionQueryMessage;
	CTradingObjPtrVector FTaifexHoldPositionArray;

	QueryStatusEnum      FTWSEHoldPositionQueryStatus;
	String               FTWSEHoldPositionQueryMessage;
	CTradingObjPtrVector FTWSEHoldPositionArray;

	QueryStatusEnum      FOverseasHoldPositionQueryStatus;
	String               FOverseasHoldPositionQueryMessage;
	CTradingObjPtrVector FOverseasHoldPositionArray;

	CTradingObjPtrVector FChinaHoldPositionArray;

	CWStrTradingObjPtrHMap FContractPositionSymbolIndex;
	CTradingObjPtrVector   FContractPositionArray;

	CWStrTradingObjPtrHMap FCommodityCancelStatisticCodeIndex;
	CTradingObjPtrVector   FCommodityCancelStatisticArray;

	CWStrTradingObjPtrHMap FContractCancelStatisticSymbolIndex;
	CTradingObjPtrVector   FContractCancelStatisticArray;

	CTradingObjPtrVector FTransferRecordArray;

public:
	static int GetMarketCount() {return FMarketCount;};

	//Constructor
	CTradingAccount(const String& LogonId, int AccountIndex,
					const String& AccountName = L"", const String& BrokerId = L"", const String& UserKey = L"");
	~CTradingAccount();

public:
	//Get/Set Field Function
	virtual int GetAccountIndex(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FAccountIndex, RWLockType);};
	virtual void SetAccountIndex(int AccountIndex, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(AccountIndex, FAccountIndex, RWLockType);};

	virtual String GetAccountName(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FAccountName, RWLockType);};
	virtual void SetAccountName(const String& AccountName, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(AccountName, FAccountName, RWLockType);};

	virtual String GetBrokerId(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FBrokerId, RWLockType);};
	virtual void SetBrokerId(const String& BrokerId, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(BrokerId, FBrokerId, RWLockType);};

	virtual String GetUserKey(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FUserKey, RWLockType);};
	virtual void SetUserKey(const String& UserKey, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(UserKey, FUserKey, RWLockType);};

	//MarketAccount Function
protected:
	void InitialMarketAccountArray();
public:
	CMarketAccount *FindMarketAccountObj(nsOrderMessageDefine::MarketEnum Market, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	String FindMarketAccount(nsOrderMessageDefine::MarketEnum Market, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool SetMarketAccount(nsOrderMessageDefine::MarketEnum Market, const String& NewMarketAccount, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void ClearMarketAccount(ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void SetAllMarketAccount(const String& NewMarketAccount, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Fee Query Status Function
	virtual QueryStatusEnum GetTaifexFeeQueryStatus(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTaifexFeeQueryStatus, RWLockType);};
	virtual void SetTaifexFeeQueryStatus(QueryStatusEnum TaifexFeeQueryStatus, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(TaifexFeeQueryStatus, FTaifexFeeQueryStatus, RWLockType);};

	virtual String GetTaifexFeeQueryMessage(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTaifexFeeQueryMessage, RWLockType);};
	virtual void SetTaifexFeeQueryMessage(const String& TaifexFeeQueryMessage, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(TaifexFeeQueryMessage, FTaifexFeeQueryMessage, RWLockType);};

	virtual QueryStatusEnum GetOverseasFeeQueryStatus(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOverseasFeeQueryStatus, RWLockType);};
	virtual void SetOverseasFeeQueryStatus(QueryStatusEnum OverseasFeeQueryStatus, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(OverseasFeeQueryStatus, FOverseasFeeQueryStatus, RWLockType);};

	virtual String GetOverseasFeeQueryMessage(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOverseasFeeQueryMessage, RWLockType);};
	virtual void SetOverseasFeeQueryMessage(const String& OverseasFeeQueryMessage, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(OverseasFeeQueryMessage, FOverseasFeeQueryMessage, RWLockType);};

	virtual CFeeObject *GetStockFutFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FStockFutFeePtr, RWLockType);};
	virtual void SetStockFutFee(CFeeObject *StockFutFeePtr, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(StockFutFeePtr, FStockFutFeePtr, RWLockType);};

	virtual CFeeObject *GetStockOptFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FStockOptFeePtr, RWLockType);};
	virtual void SetStockOptFee(CFeeObject *StockOptFeePtr, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(StockOptFeePtr, FStockOptFeePtr, RWLockType);};

	virtual CFeeObject *GetStockFee(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FStockFeePtr, RWLockType);};
	virtual void SetStockFee(CFeeObject *StockOptFeePtr, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(StockOptFeePtr, FStockFeePtr, RWLockType);};

	//Fee Code Index Function
	int GetFeeCodeIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FFeeCodeIndex, FCSLockPtr, RWLockType);};
	CFeeObject *FindFeeFromCodeIndex(const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertFeeIntoCodeIndex(const String& ExchangeCode, const String& ContractCode, CFeeObject *FeePtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CFeeObject *RemoveFeeFromCodeIndex(const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearFeeCodeIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Fee Trade Code Index Function
	int GetFeeTradeCodeIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FFeeTradeCodeIndex, FCSLockPtr, RWLockType);};
	CFeeObject *FindFeeFromTradeCodeIndex(const String& ExchangeCode, const String& TradeContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertFeeIntoTradeCodeIndex(const String& ExchangeCode, const String& TradeContractCode, CFeeObject *FeePtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CFeeObject *RemoveFeeFromTradeCodeIndex(const String& ExchangeCode, const String& TradeContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearFeeTradeCodeIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	bool AddFee(CFeeObject *FeePtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void ClearAllFee(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int LoadTestFeeData();

	//BankAccount Code Index Function
	int GetBankAccountCodeIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FBankAccountCodeIndex, FCSLockPtr, RWLockType);};
	CBankAccount *FindBankAccountFromCodeIndex(const String& BankAccount, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertBankAccountIntoCodeIndex(const String& BankAccount, CBankAccount *BankAccountPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	CBankAccount *RemoveBankAccountFromCodeIndex(const String& BankAccount, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearBankAccountCodeIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//BankAccount Array Function
	int GetBankAccountArrayCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FBankAccountArray, FCSLockPtr, RWLockType);};
	CBankAccount *GetBankAccountFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AppendBankAccountToArray(CBankAccount *BankAccountPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearBankAccountArray(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	bool AddBankAccount(CBankAccount *BankAccountPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void ClearAllBankAccount(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpBankAccount(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Taifex Hold Position Query Status Function
	virtual QueryStatusEnum GetTaifexHoldPositionQueryStatus(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTaifexHoldPositionQueryStatus, RWLockType);};
	virtual void SetTaifexHoldPositionQueryStatus(QueryStatusEnum TaifexHoldPositionQueryStatus, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(TaifexHoldPositionQueryStatus, FTaifexHoldPositionQueryStatus, RWLockType);};
	virtual String GetTaifexHoldPositionQueryMessage(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTaifexHoldPositionQueryMessage, RWLockType);};
	virtual void SetTaifexHoldPositionQueryMessage(const String& TaifexHoldPositionQueryMessage, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(TaifexHoldPositionQueryMessage, FTaifexHoldPositionQueryMessage, RWLockType);};

	//Taifex Hold Position Array Function
	int GetTaifexHoldPositionCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FTaifexHoldPositionArray, FCSLockPtr, RWLockType);};
	CHoldPositionData *GetTaifexHoldPositionFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AppendTaifexHoldPositionToArray(CHoldPositionData *HoldPositionPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearTaifexHoldPositionArray(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	///< TWSE
	virtual QueryStatusEnum GetTWSEHoldPositionQueryStatus(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTWSEHoldPositionQueryStatus, RWLockType);};
	virtual void SetTWSEHoldPositionQueryStatus( QueryStatusEnum TWSEHoldPositionQueryStatus, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(TWSEHoldPositionQueryStatus, FTWSEHoldPositionQueryStatus, RWLockType);};
	virtual String GetTWSEHoldPositionQueryMessage(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTWSEHoldPositionQueryMessage, RWLockType);};
	virtual void SetTWSEHoldPositionQueryMessage(const String& TWSEHoldPositionQueryMessage, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(TWSEHoldPositionQueryMessage, FTWSEHoldPositionQueryMessage, RWLockType);};

	//TWSE Hold Position Array Function
	int GetTWSEHoldPositionCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FTWSEHoldPositionArray, FCSLockPtr, RWLockType);};
	CHoldPositionData *GetTWSEHoldPositionFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AppendTWSEHoldPositionToArray(CHoldPositionData *HoldPositionPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearTWSEHoldPositionArray(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);


	//Overseas Hold Position Query Status Function
	virtual QueryStatusEnum GetOverseasHoldPositionQueryStatus(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FOverseasHoldPositionQueryStatus, RWLockType);};
	virtual void SetOverseasHoldPositionQueryStatus(QueryStatusEnum OverseasHoldPositionQueryStatus, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(OverseasHoldPositionQueryStatus, FOverseasHoldPositionQueryStatus, RWLockType);};
	virtual String GetOverseasHoldPositionQueryMessage(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetFieldValue(FTaifexHoldPositionQueryMessage, RWLockType);};
	virtual void SetOverseasHoldPositionQueryMessage(const String& OverseasHoldPositionQueryMessage, ReadWrietLockTypeEnum RWLockType = rwNotLock) {SetFieldValue(OverseasHoldPositionQueryMessage, FOverseasHoldPositionQueryMessage, RWLockType);};

	//Overseas Hold Position Array Function
	int GetOverseasHoldPositionCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FOverseasHoldPositionArray, FCSLockPtr, RWLockType);};
	CHoldPositionData *GetOverseasHoldPositionFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AppendOverseasHoldPositionToArray(CHoldPositionData *HoldPositionPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearOverseasHoldPositionArray(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);



	int GetChinaHoldPositionCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FChinaHoldPositionArray, FCSLockPtr, RWLockType);};
	CHoldPositionData *GetChinaHoldPositionFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AppendChinaHoldPositionToArray(CHoldPositionData *HoldPositionPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearChinaHoldPositionArray(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//ContractPosition Symbol Index Function
	int GetContractPositionSymbolIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FContractPositionSymbolIndex, FCSLockPtr, RWLockType);};
	CContractPosition *FindContractPositionFromSymbolIndex(const String& ExchangeCode, const String& Symbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertContractPositionIntoSymbolIndex(const String& ExchangeCode, const String& Symbol, CContractPosition *ContractPositionPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearContractPositionSymbolIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//ContractPosition Array Function
	int GetContractPositionArrayCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FContractPositionArray, FCSLockPtr, RWLockType);};
	CContractPosition *GetContractPositionFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AppendContractPositionToArray(CContractPosition *ContractPositionPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearContractPositionArray(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	CContractPosition *FindOrCreateContractPositionSymbolIndex(const String& ExchangeCode, const String& Symbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void ClearAllContractPosition(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Commodity Cancel Statistic Code Index Function
	int GetCommodityCancelStatisticCodeIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FCommodityCancelStatisticCodeIndex, FCSLockPtr, RWLockType);};
	CCommodityCancelStatistic *FindCommodityCancelStatisticFromCodeIndex(const String& ExchangeCode, const String& ContractCode, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertCommodityCancelStatisticIntoCodeIndex(const String& ExchangeCode, const String& ContractCode, CCommodityCancelStatistic *CancelStatisticPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearCommodityCancelStatisticCodeIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Commodity Cancel Statistic Array Function
	int GetCommodityCancelStatisticArrayCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FCommodityCancelStatisticArray, FCSLockPtr, RWLockType);};
	CCommodityCancelStatistic *GetCommodityCancelStatisticFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AppendCommodityCancelStatisticToArray(CCommodityCancelStatistic *CancelStatisticPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearCommodityCancelStatisticArray(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	bool AddCommodityCancelStatistic(const String& ExchangeCode, const String& ContractCode, int CancelQuantity, int CancelCount = 1, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpCommodityCancelStatistic(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Contract Cancel Statistic Symbol Index Function
	int GetContractCancelStatisticSymbolIndexCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FContractCancelStatisticSymbolIndex, FCSLockPtr, RWLockType);};
	CContractCancelStatistic *FindContractCancelStatisticFromSymbolIndex(const String& ExchangeCode, const String& Symbol, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool InsertContractCancelStatisticIntoSymbolIndex(const String& ExchangeCode, const String& Symbol, CContractCancelStatistic *CancelStatisticPtr, bool DoCheckExist = constNotCheckExist, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearContractCancelStatisticSymbolIndex(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	//Contract Cancel Statistic Array Function
	int GetContractCancelStatisticArrayCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FContractCancelStatisticArray, FCSLockPtr, RWLockType);};
	CContractCancelStatistic *GetContractCancelStatisticFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AppendContractCancelStatisticToArray(CContractCancelStatistic *CancelStatisticPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearContractCancelStatisticArray(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	bool AddContractCancelStatistic(const String& ExchangeCode, const String& Symbol, int CancelQuantity, int CancelCount = 1, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void ClearCancelStatistic(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	void DumpContractCancelStatistic(ReadWrietLockTypeEnum RWLockType = rwNotLock);

	int GetTransferRecordArrayCount(ReadWrietLockTypeEnum RWLockType = rwNotLock) {return GetObjectContainerCount(FTransferRecordArray, FCSLockPtr, RWLockType);};
	CCTPTransferRecord *GetTransferRecordFromArray(int ArrayIndex, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	bool AppendTransferRecordToArray(CCTPTransferRecord *TransferRecordPtr, ReadWrietLockTypeEnum RWLockType = rwNotLock);
	int ClearTransferRecordArray(bool DoDelete = constNotDelete, ReadWrietLockTypeEnum RWLockType = rwNotLock);

	virtual void DumpData(bool DoFlushToFile = constNotFlush, ReadWrietLockTypeEnum RWLockType = rwNotLock);
};  //CTradingAccount

};  //namespace s888
#endif
