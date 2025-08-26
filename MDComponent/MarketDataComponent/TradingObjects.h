//---------------------------------------------------------------------------
#ifndef TradingObjectsH
#define TradingObjectsH
#include <vcl.h>
#include <map>
#include <vector>
#include "XMLDoc.hpp"
#include "UFC.h"
#include "OrderMessageDefine.h"

//---------------------------------------------------------------------------
extern String toDescribeTMPExecType( nsOrderMessageDefine::TMPExecTypeEnum TMPExecType );
extern String toDescribeMarket( nsOrderMessageDefine::MarketEnum Market );
extern String toDescribeMessageType( nsOrderMessageDefine::MessageTypeEnum MessageType );
extern String toDescribeOrderType( nsOrderMessageDefine::OrderTypeEnum OrderType );
extern String toDescribeSide( nsOrderMessageDefine::SideEnum Side );
extern String toDescribeTimeInForce( nsOrderMessageDefine::TimeInForceEnum TimeInForce );
extern String toDescribeEventType( nsOrderMessageDefine::EventTypeEnum EventType );
extern String toDescribeExecTransType( nsOrderMessageDefine::ExecTransTypeEnum ExecTransType );
extern String toDescribeExecType( nsOrderMessageDefine::ExecTypeEnum ExecType );
extern String toDescribeOrderStatus( nsOrderMessageDefine::OrderStatusEnum OrderStatus,
									 nsOrderMessageDefine::TMPExecTypeEnum TMPExecType = nsOrderMessageDefine::tetNew );
extern String toDescribeCxlRejResponseTo( nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo );
extern String toDescribeOrdRejReason( nsOrderMessageDefine::OrdRejReasonEnum OrdRejReason );
extern String toDescribePositionEffect( nsOrderMessageDefine::PositionEffectEnum PositionEffect );

//---------------------------------------------------------------------------
extern String toFindTaifexContractCodeByOldCode(const String& OldTaifexContractCode);
extern bool toInsertOldTaifexContractCodeIndex(const String& OldTaifexContractCode,
											   const String& NewTaifexContractCode,
											   bool DoCheck = true);

//--------------------------------------------------------------------------
typedef enum
{
	toTFXNone           = 0,
	toTFXSingle         = 1,
	toTFXPriceSpread    = 2,
	toTFXTimeSpread	    = 3,
	toTFXStraddle       = 4,
	toTFXStrangle       = 5,
	toTFXConversion     = 6,
	toTFXReversals      = 7,
	toTFXCalendarSpread = 8
} TaifexSymbolKindEnum;

//---------------------------------------------------------------------------
extern int otCheckDecimalPartLength(double RealNumber, int MaxDecimalPartLength = 8);
extern void otCheckRealNumberLength(double RealNumber, int& IntegerPartLength, int& DecimalPartLength, int MaxDecimalPartLength = 8);

//---------------------------------------------------------------------------
class TMixedFraction      //帶分數物件
{
private:
	bool   FIsPositive;   //正負號
	int    FWholeNumber;  //整數部分
	double FNumerator;    //分子
	int    FDenominator;  //分母

	double FRealNumber;

public:
	static double CalculateRealNumber(int WholeNumber, double Numerator, int Denominator, bool IsPositive);
	static void ConvertRealNumber(double RealNumber, int Denominator,
								  int& WholeNumber, double& Numerator, bool& IsPositive);
	static int gcd(int a, int b);
	static int lcm(int a, int b);

	TMixedFraction(int WholeNumber = 0, double Numerator = 0.0, int Denominator = 1, bool IsPositive = true);
	TMixedFraction(double RealNumber, int Denominator = 1);

	bool   IsPositive() {return FIsPositive;};
	int    GetWholeNumber() {return FWholeNumber;};
	double GetNumerator() {return FNumerator;};
	int    GetDenominator() {return FDenominator;};

	double GetRealNumber() {return FRealNumber;};
	String GetMixedFractionFormatString();

	double GetImproperNumerator();
	double ConvertImproperNumerator(double ImproperNumerator);
	double Add(int WholeNumber, double Numerator = 0.0, int Denominator = 1, bool IsPositive = true);
	double Add(TMixedFraction& Addend);
	double Add(TMixedFraction *AddendPtr);
	double Add(double RealNumber);
	double Multiply(double Multiplier);
	double Divide(double Divisor);
};  //TMixedFraction
//----------------------------------------------------------------------------
class TTransferBank
{
private:
	String FBankCode;
	String FBankName;

public:
	TTransferBank(const String& BankCode, const String& BankName = L"");
	String GetBankCode() {return FBankCode;};
	String GetBankName() {return FBankName;};
};  //TTransferBank

extern TTransferBank *toFindTransferBankByCode(const String& BankCode);
extern bool toInsertTransferBankCodeIndex(const String& BankCode,
										  TTransferBank *TransferBankPtr,
										  bool DoCheck);
extern TTransferBank *toFindOrCreateTransferBank(const String& BankCode, const String& BankName = L"", bool DoCreate = true);

//----------------------------------------------------------------------------
class TTradingAccount;
class TBankAccount
{
private:
	TTransferBank   *FTransferBankPtr;
	TTradingAccount *FTradingAccountPtr;
	String          FBankAccount;
	String          FCurrency;
	String          FBankType;  //Z, T, Q

public:
	TBankAccount(TTransferBank *TransferBankPtr, TTradingAccount *TradingAccountPtr,
				 const String& BankAccount, const String& Currency, const String& BankType);
	TTransferBank *GetTransferBank() {return FTransferBankPtr;};
	TTradingAccount *GetTradingAccount() {return FTradingAccountPtr;};
	String GetBankAccount() {return FBankAccount;};
	String GetCurrency() {return FCurrency;};
	String GetBankType() {return FBankType;};  //Z, T, Q
};  //TBankAccount

//----------------------------------------------------------------------------
class TTransferMoneyCommand
{
private:
	int    FUID;
	String FTradingAccount;
	String FBankCode;
	String FBankAccount;
	String FTransferFlag;
	int    FAmount;
	String FCreateTime;
	int    FTransferStatus;  //0: Waiting Response  1: Success  -1: Failed
	int    FSequenceNo;
	String FErrorCode;
	String FErrorMessage;

public:
	TTransferMoneyCommand(int UID, const String& TradingAccount, const String& BankCode,
						  const String& BankAccount, const String& TransferFlag,
						  int Amount);

	int    GetUID() {return FUID;};
	String GetTradingAccount() {return FTradingAccount;};
	String GetBankCode() {return FBankCode;};
	String GetBankAccount() {return FBankAccount;};
	String GetTransferFlag() {return FTransferFlag;};
	int    GetAmount() {return FAmount;};
	String GetCreateTime() {return FCreateTime;};
	int    GetTransferStatus() {return FTransferStatus;};
	int    GetSequenceNo() {return FSequenceNo;};
	String GetErrorCode() {return FErrorCode;};
	String GetErrorMessage() {return FErrorMessage;};

	void SetTransferStatus(int TransferStatus) {FTransferStatus = TransferStatus;};
	void SetSequenceNo(int SequenceNo) {FSequenceNo = SequenceNo;};
	void SetErrorCode(const String& ErrorCode) {FErrorCode = ErrorCode;};
	void SetErrorMessage(const String& ErrorMessage) {FErrorMessage = ErrorMessage;};
};  //TTransferMoneyCommand
//----------------------------------------------------------------------------
class TTransferRecord
{
private:
	int    FSequenceNo;
	String FTradingAccount;
	String FBankName;
	String FBankAccount;
	String FOperator;
	String FOperatingDate;
	String FOperatingTime;
	String FOperatingType;
	int    FAmount;
	String FProcessStatus;
	String FProcessResult;
	String FBankCode;
	String FBankType;  //Z, T, Q

public:
	TTransferRecord(int SequenceNo, const String& TradingAccount,
					const String& BankName, const String& BankAccount,
					const String& Operator, const String& OperatingDate,
					const String& OperatingTime, const String& OperatingType,
					int Amount, const String& ProcessStatus,
					const String& ProcessResult, const String& BankCode,
					const String& BankType);

	int    GetSequenceNo() {return FSequenceNo;};
	String GetTradingAccount() {return FTradingAccount;};
	String GetBankName() {return FBankName;};
	String GetBankAccount() {return FBankAccount;};
	String GetOperator() {return FOperator;};
	String GetOperatingDate() {return FOperatingDate;};
	String GetOperatingTime() {return FOperatingTime;};
	String GetOperatingType() {return FOperatingType;};
	int    GetAmount() {return FAmount;};
	String GetProcessStatus() {return FProcessStatus;};
	String GetProcessResult() {return FProcessResult;};
	String GetBankCode() {return FBankCode;};
	String GetBankType() {return FBankType;};  //Z, T, Q

	void SetSequenceNo(int SequenceNo) {FSequenceNo = SequenceNo;};
	void SetTradingAccount(const String& TradingAccount) {FTradingAccount = TradingAccount;};
	void SetBankName(const String& BankName) {FBankName = BankName;};
	void SetBankAccount(const String& BankAccount) {FBankAccount = BankAccount;};
	void SetOperator(const String& Operator) {FOperator = Operator;};
	void SetOperatingDate(const String& OperatingDate) {FOperatingDate = OperatingDate;};
	void SetOperatingTime(const String& OperatingTime) {FOperatingTime = OperatingTime;};
	void SetOperatingType(const String& OperatingType) {FOperatingType = OperatingType;};
	void SetAmount(int Amount) {FAmount = Amount;};
	void SetProcessStatus(const String& ProcessStatus) {FProcessStatus = ProcessStatus;};
	void SetProcessResult(const String& ProcessResult) {FProcessResult = ProcessResult;};
	void SetBankCode(const String& BankCode) {FBankCode = BankCode;};
	void SetBankType(const String& BankType) {FBankType = BankType;};  //Z, T, Q
};  //TTransferRecord
//----------------------------------------------------------------------------
class TQueryBankAvailableCashCommand
{
private:
	int    FUID;
	String FTradingAccount;
	String FBankCode;
	String FCurrency;
	String FCreateTime;
	int    FAvailableCash;
	String FErrorCode;
	String FErrorMessage;

public:
	TQueryBankAvailableCashCommand(int UID, const String& TradingAccount, const String& BankCode,
								   const String& Currency);

	int    GetUID() {return FUID;};
	String GetTradingAccount() {return FTradingAccount;};
	String GetBankCode() {return FBankCode;};
	String GetCurrency() {return FCurrency;};
	String GetCreateTime() {return FCreateTime;};

	String GetErrorCode() {return FErrorCode;};
	String GetErrorMessage() {return FErrorMessage;};
	int    GetAvailableCash() {return FAvailableCash;};
	void SetErrorCode(const String& ErrorCode) {FErrorCode = ErrorCode;};
	void SetErrorMessage(const String& ErrorMessage) {FErrorMessage = ErrorMessage;};
};  //TQueryBankAvailableCashCommand
//----------------------------------------------------------------------------
class TAvailableBankCashRecord
{
private:
	String FBankCode;
	String FCurrency;
	int    FAvailableCash;
	String FUpdateTime;

public:
	TAvailableBankCashRecord(const String& BankCode, const String& Currency, int AvailableCash);
	String GetBankCode() {return FBankCode;};
	String GetCurrency() {return FCurrency;};
	int    GetAvailableCash() {return FAvailableCash;};
	String GetUpdateTime() {return FUpdateTime;};

	void SetAvailableCash(int AvailableCash) {FAvailableCash = AvailableCash;};
	void SetUpdateTime(const String& UpdateTime) {FUpdateTime = UpdateTime;};
};  //TAvailableBankCashRecord
//----------------------------------------------------------------------------
class TAvailableFundRecord
{
private:
	int    FAvailableFund;
	int    FAvailableCash;
	int    FAvailableTradingFund;
	int    FAvailableTradingCash;
	int    FCheckCredit;
	String FErrorCode;
	String FErrorMessage;
	String FUpdateTime;

public:
	TAvailableFundRecord(int AvailableFund, int AvailableCash, int AvailableTradingFund, int AvailableTradingCash, int CheckCredit);
	int    GetAvailableFund() {return FAvailableFund;};
	int    GetAvailableCash() {return FAvailableCash;};
	int    GetAvailableTradingFund() {return FAvailableTradingFund;};
	int    GetAvailableTradingCash() {return FAvailableTradingCash;};
	int    GetCheckCredit() {return FCheckCredit;};
	String GetErrorCode() {return FErrorCode;};
	String GetErrorMessage() {return FErrorMessage;};
	String GetUpdateTime() {return FUpdateTime;};

	void SetAvailableFund(int AvailableFund) {FAvailableFund = AvailableFund;};
	void SetAvailableCash(int AvailableCash) {FAvailableCash = AvailableCash;};
	void SetAvailableTradingFund(int AvailableTradingFund) {FAvailableTradingFund = AvailableTradingFund;};
	void SetAvailableTradingCash(int AvailableTradingCash) {FAvailableTradingCash = AvailableTradingCash;};
	void SetErrorCode(const String& ErrorCode) {FErrorCode = ErrorCode;};
	void SetErrorMessage(const String& ErrorMessage) {FErrorMessage = ErrorMessage;};
	void SetUpdateTime(const String& UpdateTime) {FUpdateTime = UpdateTime;};
	void UpdateField(int AvailableFund, int AvailableCash, int AvailableTradingFund, int AvailableTradingCash, int CheckCredit);
	void UpdateField(const String& ErrorCode, const String& ErrorMessage);
};  //TAvailableFundRecord
//----------------------------------------------------------------------------
class TTradingAccount
{
private:
	typedef std::map<String, TBankAccount*> toStringBankAccountMap;
	typedef std::vector<TBankAccount*> toBankAccountVector;
	typedef std::vector<TTransferMoneyCommand*> toTransferMoneyCmdVector;
	typedef std::vector<TTransferRecord*> toTransferRecordVector;
	typedef std::map<int, TQueryBankAvailableCashCommand*> toIntQryAvailableCashCmdMap;
	typedef std::vector<TAvailableBankCashRecord*> toAvailableCashRecordVector;

	String FAccount;
	String FName;
	String FQryTaifexHoldOpenInterestURL;
	String FQryTaifexHoldOpenInterestErrMsg;
	String FQryOverseasHoldOpenInterestURL;
	String FQryOverseasHoldOpenInterestErrMsg;
	String FQryTransRecErrCode;
	String FQryTransRecErrMsg;
	String FRecvTransRecTime;
	String FQryAvailableCashErrCode;
	String FQryAvailableCashErrMsg;
	String FQryAvailableCashTime;

	toStringBankAccountMap      FBankAccountAccountIndex;
	toBankAccountVector         FBankAccountArray;
	toTransferMoneyCmdVector    FTransferMoneyCmdArray;
	toTransferRecordVector      FTransferRecordArray;
	toIntQryAvailableCashCmdMap FQryAvailableCashCmdUIDIndex;
	toAvailableCashRecordVector FAvailableCashRecordArray;
	TAvailableFundRecord        *FAvailableFundRecordPtr;

public:
	TTradingAccount(const String& Account);
	TTradingAccount(const String& Account, const String& Name);
	~TTradingAccount()
	{
		ClearBankAccount();
		ClearTransferMoneyCmd();
	};

	String GetAccount() {return FAccount;};
	String GetName() {return FName;};
	String GetQryTransRecErrCode() {return FQryTransRecErrCode;};
	String GetQryTransRecErrMsg() {return FQryTransRecErrMsg;};
	String GetRecvTransRecTime() {return FRecvTransRecTime;};
	String GetQryAvailableCashErrCode() {return FQryAvailableCashErrCode;};
	String GetQryAvailableCashErrMsg() {return FQryAvailableCashErrMsg;};
	String GetQryAvailableCashTime() {return FQryAvailableCashTime;};

	void SetName(const String& Name) {FName = Name;};
	void SetQryTransRecErrCode(const String& QryTransRecErrCode) {FQryTransRecErrCode = QryTransRecErrCode;};
	void SetQryTransRecErrMsg(const String& QryTransRecErrMsg) {FQryTransRecErrMsg = QryTransRecErrMsg;};
	void SetRecvTransRecTime(const String& RecvTransRecTime) {FRecvTransRecTime = RecvTransRecTime;};
	void SetQryAvailableCashErrCode(const String& AvailableCashErrCode) {FQryAvailableCashErrCode = AvailableCashErrCode;};
	void SetQryAvailableCashErrMsg(const String& QryAvailableCashErrMsg) {FQryAvailableCashErrMsg = QryAvailableCashErrMsg;};
	void SetQryAvailableCashTime(const String& QryAvailableCashTime) {FQryAvailableCashTime = QryAvailableCashTime;};

	void ClearBankAccount();
	TBankAccount *FindBankAccountByIndexKey(const String& IndexKey);
	TBankAccount *FindBankAccountByAccount(const String& BankCode, const String& BankAccount);
	bool InsertBankAccountAccountIndex(const String& IndexKey,
									   TBankAccount *BankAccountPtr,
									   bool DoCheck = true);
	bool InsertBankAccountAccountIndex(const String& BankCode, const String& BankAccount,
									   TBankAccount *BankAccountPtr,
									   bool DoCheck = true);
	int GetBankAccountCount() {return FBankAccountArray.size();};
	void AddBankAccount(TBankAccount *BankAccountPtr);
	TBankAccount *GetBankAccountByIndex(int Index);

	void ClearTransferMoneyCmd();
	int GetTransferMoneyCmdCount() {return FTransferMoneyCmdArray.size();};
	void AddTransferMoneyCmd(TTransferMoneyCommand *TransferMoneyCmdPtr);
	TTransferMoneyCommand *GetTransferMoneyCmdByIndex(int Index);
	TTransferMoneyCommand *GetTransferMoneyCmdByID(int ID);

	void ClearTransferRecord();
	int GetTransferRecordCount() {return FTransferRecordArray.size();};
	void AddTransferRecord(TTransferRecord *TransferRecordPtr);
	TTransferRecord *GetTransferRecordByIndex(int Index);

	void ClearQryAvailableCashCmd();
	TQueryBankAvailableCashCommand *FindQryAvailableCashCmdByUID(int UID);
	bool InsertQryAvailableCashCmdUIDIndex(int UID,
									   TQueryBankAvailableCashCommand *QryAvailableCashCmdPtr,
									   bool DoCheck = true);
	int GetQryAvailableCashCmdCount() {return FQryAvailableCashCmdUIDIndex.size();};

	void ClearAvailableCashRec();
	int GetAvailableCashRecCount() {return FAvailableCashRecordArray.size();};
	void AddAvailableCashRec(TAvailableBankCashRecord *AvailableCashRecPtr);
	TAvailableBankCashRecord *GetAvailableCashRecByIndex(int Index);
	TAvailableBankCashRecord *GetAvailableCashRecByBankCode(const String& BankCode, const String& Currency);

	void GetAvailableFund(int& AvailableFund, int& AvailableCash, int& AvailableTradingFund, int& AvailableTradingCash, int& CheckCredit, String& UpdateTime);
	void GetAvailableFundError(String& ErrorCode, String& ErrorMessage, String& UpdateTime);
	void UpdateAvailableFund(int AvailableFund, int AvailableCash, int AvailableTradingFund, int AvailableTradingCash, int CheckCredit);
	void UpdateAvailableFundError(const String& ErrorCode, const String& ErrorMessage);
};  //TTradingAccount

extern TTradingAccount *toFindTradingAccountByAccount(const String& Account);
extern bool toInsertTradingAccountIndex(const String& Account,
										TTradingAccount *AccountPtr,
										bool DoCheck = true);
extern TTradingAccount *toFindOrCreateTradingAccount(const String& Account, bool DoCreate = true);

#endif
