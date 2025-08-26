/////////////////////////////////////////////////////////////////////////
///@system 新一代交易所系統
///@company 上海期貨資訊技術有限公司
///@file ThostFtdcUserApiStruct.h
///@brief 定義了用戶端介面使用的業務資料結構
///@history
///20060106	趙鴻昊		創建該文件
/////////////////////////////////////////////////////////////////////////

#if !defined(THOST_FTDCSTRUCT_H)
#define THOST_FTDCSTRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ThostFtdcUserApiDataType.h"

///資訊分發
struct CThostFtdcDisseminationField
{
	///序列系列號
	TThostFtdcSequenceSeriesType	SequenceSeries;
	///序號
	TThostFtdcSequenceNoType	SequenceNo;
};

///用戶登錄請求
struct CThostFtdcReqUserLoginField
{
	///交易日
	TThostFtdcDateType	TradingDay;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///密碼
	TThostFtdcPasswordType	Password;
	///使用者端產品資訊
	TThostFtdcProductInfoType	UserProductInfo;
	///介面端產品資訊
	TThostFtdcProductInfoType	InterfaceProductInfo;
	///協定資訊
	TThostFtdcProtocolInfoType	ProtocolInfo;
	///Mac地址
	TThostFtdcMacAddressType	MacAddress;
	///動態密碼
	TThostFtdcPasswordType	OneTimePassword;
	///終端IP位址
	TThostFtdcIPAddressType	ClientIPAddress;
};

///用戶登錄應答
struct CThostFtdcRspUserLoginField
{
	///交易日
	TThostFtdcDateType	TradingDay;
	///登錄成功時間
	TThostFtdcTimeType	LoginTime;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///交易系統名稱
	TThostFtdcSystemNameType	SystemName;
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///最大報單引用
	TThostFtdcOrderRefType	MaxOrderRef;
	///上期所時間
	TThostFtdcTimeType	SHFETime;
	///大商所時間
	TThostFtdcTimeType	DCETime;
	///鄭商所時間
	TThostFtdcTimeType	CZCETime;
	///中金所時間
	TThostFtdcTimeType	FFEXTime;
};

///用戶登出請求
struct CThostFtdcUserLogoutField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
};

///強制交易員退出
struct CThostFtdcForceUserLogoutField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
};

///用戶端認證請求
struct CThostFtdcReqAuthenticateField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///使用者端產品資訊
	TThostFtdcProductInfoType	UserProductInfo;
	///認證碼
	TThostFtdcAuthCodeType	AuthCode;
};

///用戶端認證回應
struct CThostFtdcRspAuthenticateField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///使用者端產品資訊
	TThostFtdcProductInfoType	UserProductInfo;
};

///用戶端認證資訊
struct CThostFtdcAuthenticationInfoField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///使用者端產品資訊
	TThostFtdcProductInfoType	UserProductInfo;
	///認證資訊
	TThostFtdcAuthInfoType	AuthInfo;
	///是否為認證結果
	TThostFtdcBoolType	IsResult;
};

///銀期轉帳報文頭
struct CThostFtdcTransferHeaderField
{
	///版本號，常量，1.0
	TThostFtdcVersionType	Version;
	///交易代碼，必填
	TThostFtdcTradeCodeType	TradeCode;
	///交易日期，必填，格式：yyyymmdd
	TThostFtdcTradeDateType	TradeDate;
	///交易時間，必填，格式：hhmmss
	TThostFtdcTradeTimeType	TradeTime;
	///發起方流水號，N/A
	TThostFtdcTradeSerialType	TradeSerial;
	///期貨公司代碼，必填
	TThostFtdcFutureIDType	FutureID;
	///銀行代碼，根據查詢銀行得到，必填
	TThostFtdcBankIDType	BankID;
	///銀行分中心代碼，根據查詢銀行得到，必填
	TThostFtdcBankBrchIDType	BankBrchID;
	///操作員，N/A
	TThostFtdcOperNoType	OperNo;
	///交易設備類型，N/A
	TThostFtdcDeviceIDType	DeviceID;
	///記錄數，N/A
	TThostFtdcRecordNumType	RecordNum;
	///會話編號，N/A
	TThostFtdcSessionIDType	SessionID;
	///請求編號，N/A
	TThostFtdcRequestIDType	RequestID;
};

///銀行資金轉期貨請求，TradeCode=202001
struct CThostFtdcTransferBankToFutureReqField
{
	///期貨資金帳戶
	TThostFtdcAccountIDType	FutureAccount;
	///密碼標誌
	TThostFtdcFuturePwdFlagType	FuturePwdFlag;
	///密碼
	TThostFtdcFutureAccPwdType	FutureAccPwd;
	///轉帳金額
	TThostFtdcMoneyType	TradeAmt;
	///客戶手續費
	TThostFtdcMoneyType	CustFee;
	///幣種：RMB-人民幣 USD-美圓 HKD-港元
	TThostFtdcCurrencyCodeType	CurrencyCode;
};

///銀行資金轉期貨請求回應
struct CThostFtdcTransferBankToFutureRspField
{
	///回應代碼
	TThostFtdcRetCodeType	RetCode;
	///回應信息
	TThostFtdcRetInfoType	RetInfo;
	///資金帳戶
	TThostFtdcAccountIDType	FutureAccount;
	///轉帳金額
	TThostFtdcMoneyType	TradeAmt;
	///應收客戶手續費
	TThostFtdcMoneyType	CustFee;
	///幣種
	TThostFtdcCurrencyCodeType	CurrencyCode;
};

///期貨資金轉銀行請求，TradeCode=202002
struct CThostFtdcTransferFutureToBankReqField
{
	///期貨資金帳戶
	TThostFtdcAccountIDType	FutureAccount;
	///密碼標誌
	TThostFtdcFuturePwdFlagType	FuturePwdFlag;
	///密碼
	TThostFtdcFutureAccPwdType	FutureAccPwd;
	///轉帳金額
	TThostFtdcMoneyType	TradeAmt;
	///客戶手續費
	TThostFtdcMoneyType	CustFee;
	///幣種：RMB-人民幣 USD-美圓 HKD-港元
	TThostFtdcCurrencyCodeType	CurrencyCode;
};

///期貨資金轉銀行請求回應
struct CThostFtdcTransferFutureToBankRspField
{
	///回應代碼
	TThostFtdcRetCodeType	RetCode;
	///回應信息
	TThostFtdcRetInfoType	RetInfo;
	///資金帳戶
	TThostFtdcAccountIDType	FutureAccount;
	///轉帳金額
	TThostFtdcMoneyType	TradeAmt;
	///應收客戶手續費
	TThostFtdcMoneyType	CustFee;
	///幣種
	TThostFtdcCurrencyCodeType	CurrencyCode;
};

///查詢銀行資金請求，TradeCode=204002
struct CThostFtdcTransferQryBankReqField
{
	///期貨資金帳戶
	TThostFtdcAccountIDType	FutureAccount;
	///密碼標誌
	TThostFtdcFuturePwdFlagType	FuturePwdFlag;
	///密碼
	TThostFtdcFutureAccPwdType	FutureAccPwd;
	///幣種：RMB-人民幣 USD-美圓 HKD-港元
	TThostFtdcCurrencyCodeType	CurrencyCode;
};

///查詢銀行資金請求回應
struct CThostFtdcTransferQryBankRspField
{
	///回應代碼
	TThostFtdcRetCodeType	RetCode;
	///回應信息
	TThostFtdcRetInfoType	RetInfo;
	///資金帳戶
	TThostFtdcAccountIDType	FutureAccount;
	///銀行餘額
	TThostFtdcMoneyType	TradeAmt;
	///銀行可用餘額
	TThostFtdcMoneyType	UseAmt;
	///銀行可取餘額
	TThostFtdcMoneyType	FetchAmt;
	///幣種
	TThostFtdcCurrencyCodeType	CurrencyCode;
};

///查詢銀行交易明細請求，TradeCode=204999
struct CThostFtdcTransferQryDetailReqField
{
	///期貨資金帳戶
	TThostFtdcAccountIDType	FutureAccount;
};

///查詢銀行交易明細請求回應
struct CThostFtdcTransferQryDetailRspField
{
	///交易日期
	TThostFtdcDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///交易代碼
	TThostFtdcTradeCodeType	TradeCode;
	///期貨流水號
	TThostFtdcTradeSerialNoType	FutureSerial;
	///期貨公司代碼
	TThostFtdcFutureIDType	FutureID;
	///資金帳號
	TThostFtdcFutureAccountType	FutureAccount;
	///銀行流水號
	TThostFtdcTradeSerialNoType	BankSerial;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分中心代碼
	TThostFtdcBankBrchIDType	BankBrchID;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///證件號碼
	TThostFtdcCertCodeType	CertCode;
	///貨幣代碼
	TThostFtdcCurrencyCodeType	CurrencyCode;
	///發生金額
	TThostFtdcMoneyType	TxAmount;
	///有效標誌
	TThostFtdcTransferValidFlagType	Flag;
};

///回應信息
struct CThostFtdcRspInfoField
{
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///交易所
struct CThostFtdcExchangeField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///交易所名稱
	TThostFtdcExchangeNameType	ExchangeName;
	///交易所屬性
	TThostFtdcExchangePropertyType	ExchangeProperty;
};

///產品
struct CThostFtdcProductField
{
	///產品代碼
	TThostFtdcInstrumentIDType	ProductID;
	///產品名稱
	TThostFtdcProductNameType	ProductName;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///產品類型
	TThostFtdcProductClassType	ProductClass;
	///合約數量乘數
	TThostFtdcVolumeMultipleType	VolumeMultiple;
	///最小變動價位
	TThostFtdcPriceType	PriceTick;
	///市價單最大下單量
	TThostFtdcVolumeType	MaxMarketOrderVolume;
	///市價單最小下單量
	TThostFtdcVolumeType	MinMarketOrderVolume;
	///限價單最大下單量
	TThostFtdcVolumeType	MaxLimitOrderVolume;
	///限價單最小下單量
	TThostFtdcVolumeType	MinLimitOrderVolume;
	///持倉類型
	TThostFtdcPositionTypeType	PositionType;
	///持倉日期類型
	TThostFtdcPositionDateTypeType	PositionDateType;
	///平倉處理類型
	TThostFtdcCloseDealTypeType	CloseDealType;
	///交易所產品代碼
	TThostFtdcInstrumentIDType	ExchangeProductID;
};

///合約
struct CThostFtdcInstrumentField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///合約名稱
	TThostFtdcInstrumentNameType	InstrumentName;
	///合約在交易所的代碼
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///產品代碼
	TThostFtdcInstrumentIDType	ProductID;
	///產品類型
	TThostFtdcProductClassType	ProductClass;
	///交割年份
	TThostFtdcYearType	DeliveryYear;
	///交割月
	TThostFtdcMonthType	DeliveryMonth;
	///市價單最大下單量
	TThostFtdcVolumeType	MaxMarketOrderVolume;
	///市價單最小下單量
	TThostFtdcVolumeType	MinMarketOrderVolume;
	///限價單最大下單量
	TThostFtdcVolumeType	MaxLimitOrderVolume;
	///限價單最小下單量
	TThostFtdcVolumeType	MinLimitOrderVolume;
	///合約數量乘數
	TThostFtdcVolumeMultipleType	VolumeMultiple;
	///最小變動價位
	TThostFtdcPriceType	PriceTick;
	///創建日
	TThostFtdcDateType	CreateDate;
	///上市日
	TThostFtdcDateType	OpenDate;
	///到期日
	TThostFtdcDateType	ExpireDate;
	///開始交割日
	TThostFtdcDateType	StartDelivDate;
	///結束交割日
	TThostFtdcDateType	EndDelivDate;
	///合約生命週期狀態
	TThostFtdcInstLifePhaseType	InstLifePhase;
	///當前是否交易
	TThostFtdcBoolType	IsTrading;
	///持倉類型
	TThostFtdcPositionTypeType	PositionType;
	///持倉日期類型
	TThostFtdcPositionDateTypeType	PositionDateType;
	///多頭保證金率
	TThostFtdcRatioType	LongMarginRatio;
	///空頭保證金率
	TThostFtdcRatioType	ShortMarginRatio;
	///基礎商品代碼
	TThostFtdcInstrumentIDType	UnderlyingInstrID;
	///執行價
	TThostFtdcPriceType	StrikePrice;
	///期權類型
	TThostFtdcOptionsTypeType	OptionsType;
	///合約系列
	TThostFtdcInstrumentIDType	InstrumentIdSerial;
};

///經紀公司
struct CThostFtdcBrokerField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///經紀公司簡稱
	TThostFtdcBrokerAbbrType	BrokerAbbr;
	///經紀公司名稱
	TThostFtdcBrokerNameType	BrokerName;
	///是否活躍
	TThostFtdcBoolType	IsActive;
};

///交易所交易員
struct CThostFtdcTraderField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///密碼
	TThostFtdcPasswordType	Password;
	///安裝數量
	TThostFtdcInstallCountType	InstallCount;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
};

///投資者
struct CThostFtdcInvestorField
{
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者分組代碼
	TThostFtdcInvestorIDType	InvestorGroupID;
	///投資者名稱
	TThostFtdcPartyNameType	InvestorName;
	///證件類型
	TThostFtdcIdCardTypeType	IdentifiedCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///是否活躍
	TThostFtdcBoolType	IsActive;
	///聯繫電話
	TThostFtdcTelephoneType	Telephone;
	///通訊位址
	TThostFtdcAddressType	Address;
	///開戶日期
	TThostFtdcDateType	OpenDate;
	///手機
	TThostFtdcMobileType	Mobile;
	///手續費率範本代碼
	TThostFtdcInvestorIDType	CommModelID;
	///保證金率範本代碼
	TThostFtdcInvestorIDType	MarginModelID;
};

///交易編碼
struct CThostFtdcTradingCodeField
{
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///是否活躍
	TThostFtdcBoolType	IsActive;
	///交易編碼類型
	TThostFtdcClientIDTypeType	ClientIDType;
};

///會員編碼和經紀公司編碼對照表
struct CThostFtdcPartBrokerField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///是否活躍
	TThostFtdcBoolType	IsActive;
};

///管理用戶
struct CThostFtdcSuperUserField
{
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///用戶名稱
	TThostFtdcUserNameType	UserName;
	///密碼
	TThostFtdcPasswordType	Password;
	///是否活躍
	TThostFtdcBoolType	IsActive;
};

///管理用戶功能許可權
struct CThostFtdcSuperUserFunctionField
{
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///功能代碼
	TThostFtdcFunctionCodeType	FunctionCode;
};

///投資者組
struct CThostFtdcInvestorGroupField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者分組代碼
	TThostFtdcInvestorIDType	InvestorGroupID;
	///投資者分組名稱
	TThostFtdcInvestorGroupNameType	InvestorGroupName;
};

///資金帳戶
struct CThostFtdcTradingAccountField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///上次質押金額
	TThostFtdcMoneyType	PreMortgage;
	///上次信用額度
	TThostFtdcMoneyType	PreCredit;
	///上次存款額
	TThostFtdcMoneyType	PreDeposit;
	///上次結算準備金
	TThostFtdcMoneyType	PreBalance;
	///上次佔用的保證金
	TThostFtdcMoneyType	PreMargin;
	///利息基數
	TThostFtdcMoneyType	InterestBase;
	///利息收入
	TThostFtdcMoneyType	Interest;
	///入金金額
	TThostFtdcMoneyType	Deposit;
	///出金金額
	TThostFtdcMoneyType	Withdraw;
	///凍結的保證金
	TThostFtdcMoneyType	FrozenMargin;
	///凍結的資金
	TThostFtdcMoneyType	FrozenCash;
	///凍結的手續費
	TThostFtdcMoneyType	FrozenCommission;
	///當前保證金總額
	TThostFtdcMoneyType	CurrMargin;
	///資金差額
	TThostFtdcMoneyType	CashIn;
	///手續費
	TThostFtdcMoneyType	Commission;
	///平倉盈虧
	TThostFtdcMoneyType	CloseProfit;
	///持倉盈虧
	TThostFtdcMoneyType	PositionProfit;
	///期貨結算準備金
	TThostFtdcMoneyType	Balance;
	///可用資金
	TThostFtdcMoneyType	Available;
	///可取資金
	TThostFtdcMoneyType	WithdrawQuota;
	///基本準備金
	TThostFtdcMoneyType	Reserve;
	///交易日
	TThostFtdcDateType	TradingDay;
	///結算編號
	TThostFtdcSettlementIDType	SettlementID;
	///信用額度
	TThostFtdcMoneyType	Credit;
	///質押金額
	TThostFtdcMoneyType	Mortgage;
	///交易所保證金
	TThostFtdcMoneyType	ExchangeMargin;
	///投資者交割保證金
	TThostFtdcMoneyType	DeliveryMargin;
	///交易所交割保證金
	TThostFtdcMoneyType	ExchangeDeliveryMargin;
	///期權平倉盈虧
	TThostFtdcMoneyType	OptionCloseProfit;
};

///投資者持倉
struct CThostFtdcInvestorPositionField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///持倉多空方向
	TThostFtdcPosiDirectionType	PosiDirection;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///持倉日期
	TThostFtdcPositionDateType	PositionDate;
	///上日持倉
	TThostFtdcVolumeType	YdPosition;
	///今日持倉
	TThostFtdcVolumeType	Position;
	///多頭凍結
	TThostFtdcVolumeType	LongFrozen;
	///空頭凍結
	TThostFtdcVolumeType	ShortFrozen;
	///開倉凍結金額
	TThostFtdcMoneyType	LongFrozenAmount;
	///開倉凍結金額
	TThostFtdcMoneyType	ShortFrozenAmount;
	///開倉量
	TThostFtdcVolumeType	OpenVolume;
	///平倉量
	TThostFtdcVolumeType	CloseVolume;
	///開倉金額
	TThostFtdcMoneyType	OpenAmount;
	///平倉金額
	TThostFtdcMoneyType	CloseAmount;
	///持倉成本
	TThostFtdcMoneyType	PositionCost;
	///上次佔用的保證金
	TThostFtdcMoneyType	PreMargin;
	///佔用的保證金
	TThostFtdcMoneyType	UseMargin;
	///凍結的保證金
	TThostFtdcMoneyType	FrozenMargin;
	///凍結的資金
	TThostFtdcMoneyType	FrozenCash;
	///凍結的手續費
	TThostFtdcMoneyType	FrozenCommission;
	///資金差額
	TThostFtdcMoneyType	CashIn;
	///手續費
	TThostFtdcMoneyType	Commission;
	///平倉盈虧
	TThostFtdcMoneyType	CloseProfit;
	///持倉盈虧
	TThostFtdcMoneyType	PositionProfit;
	///上次結算價
	TThostFtdcPriceType	PreSettlementPrice;
	///本次結算價
	TThostFtdcPriceType	SettlementPrice;
	///交易日
	TThostFtdcDateType	TradingDay;
	///結算編號
	TThostFtdcSettlementIDType	SettlementID;
	///開倉成本
	TThostFtdcMoneyType	OpenCost;
	///交易所保證金
	TThostFtdcMoneyType	ExchangeMargin;
	///組合成交形成的持倉
	TThostFtdcVolumeType	CombPosition;
	///組合多頭凍結
	TThostFtdcVolumeType	CombLongFrozen;
	///組合空頭凍結
	TThostFtdcVolumeType	CombShortFrozen;
	///逐日盯市平倉盈虧
	TThostFtdcMoneyType	CloseProfitByDate;
	///逐筆對沖平倉盈虧
	TThostFtdcMoneyType	CloseProfitByTrade;
	///今日持倉
	TThostFtdcVolumeType	TodayPosition;
	///保證金率
	TThostFtdcRatioType	MarginRateByMoney;
	///保證金率(按手數)
	TThostFtdcRatioType	MarginRateByVolume;
	///執行凍結
	TThostFtdcVolumeType	StrikeFrozen;
	///執行凍結金額
	TThostFtdcMoneyType	StrikeFrozenAmount;
};

///合約保證金率
struct CThostFtdcInstrumentMarginRateField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投資者範圍
	TThostFtdcInvestorRangeType	InvestorRange;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///多頭保證金率
	TThostFtdcRatioType	LongMarginRatioByMoney;
	///多頭保證金費
	TThostFtdcMoneyType	LongMarginRatioByVolume;
	///空頭保證金率
	TThostFtdcRatioType	ShortMarginRatioByMoney;
	///空頭保證金費
	TThostFtdcMoneyType	ShortMarginRatioByVolume;
	///是否相對交易所收取
	TThostFtdcBoolType	IsRelative;
};

///合約手續費率
struct CThostFtdcInstrumentCommissionRateField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投資者範圍
	TThostFtdcInvestorRangeType	InvestorRange;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///開倉手續費率
	TThostFtdcRatioType	OpenRatioByMoney;
	///開倉手續費
	TThostFtdcRatioType	OpenRatioByVolume;
	///平倉手續費率
	TThostFtdcRatioType	CloseRatioByMoney;
	///平倉手續費
	TThostFtdcRatioType	CloseRatioByVolume;
	///平今手續費率
	TThostFtdcRatioType	CloseTodayRatioByMoney;
	///平今手續費
	TThostFtdcRatioType	CloseTodayRatioByVolume;
};

///深度行情
struct CThostFtdcDepthMarketDataField
{
	///交易日
	TThostFtdcDateType	TradingDay;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///合約在交易所的代碼
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///最新價
	TThostFtdcPriceType	LastPrice;
	///上次結算價
	TThostFtdcPriceType	PreSettlementPrice;
	///昨收盤
	TThostFtdcPriceType	PreClosePrice;
	///昨持倉量
	TThostFtdcLargeVolumeType	PreOpenInterest;
	///今開盤
	TThostFtdcPriceType	OpenPrice;
	///最高價
	TThostFtdcPriceType	HighestPrice;
	///最低價
	TThostFtdcPriceType	LowestPrice;
	///數量
	TThostFtdcVolumeType	Volume;
	///成交金額
	TThostFtdcMoneyType	Turnover;
	///持倉量
	TThostFtdcLargeVolumeType	OpenInterest;
	///今收盤
	TThostFtdcPriceType	ClosePrice;
	///本次結算價
	TThostFtdcPriceType	SettlementPrice;
	///漲停板價
	TThostFtdcPriceType	UpperLimitPrice;
	///跌停板價
	TThostFtdcPriceType	LowerLimitPrice;
	///昨虛實度
	TThostFtdcRatioType	PreDelta;
	///今虛實度
	TThostFtdcRatioType	CurrDelta;
	///最後修改時間
	TThostFtdcTimeType	UpdateTime;
	///最後修改毫秒
	TThostFtdcMillisecType	UpdateMillisec;
	///申買價一
	TThostFtdcPriceType	BidPrice1;
	///申買量一
	TThostFtdcVolumeType	BidVolume1;
	///申賣價一
	TThostFtdcPriceType	AskPrice1;
	///申賣量一
	TThostFtdcVolumeType	AskVolume1;
	///申買價二
	TThostFtdcPriceType	BidPrice2;
	///申買量二
	TThostFtdcVolumeType	BidVolume2;
	///申賣價二
	TThostFtdcPriceType	AskPrice2;
	///申賣量二
	TThostFtdcVolumeType	AskVolume2;
	///申買價三
	TThostFtdcPriceType	BidPrice3;
	///申買量三
	TThostFtdcVolumeType	BidVolume3;
	///申賣價三
	TThostFtdcPriceType	AskPrice3;
	///申賣量三
	TThostFtdcVolumeType	AskVolume3;
	///申買價四
	TThostFtdcPriceType	BidPrice4;
	///申買量四
	TThostFtdcVolumeType	BidVolume4;
	///申賣價四
	TThostFtdcPriceType	AskPrice4;
	///申賣量四
	TThostFtdcVolumeType	AskVolume4;
	///申買價五
	TThostFtdcPriceType	BidPrice5;
	///申買量五
	TThostFtdcVolumeType	BidVolume5;
	///申賣價五
	TThostFtdcPriceType	AskPrice5;
	///申賣量五
	TThostFtdcVolumeType	AskVolume5;
	///當日均價
	TThostFtdcPriceType	AveragePrice;
};

///投資者合約交易許可權
struct CThostFtdcInstrumentTradingRightField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投資者範圍
	TThostFtdcInvestorRangeType	InvestorRange;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///交易許可權
	TThostFtdcTradingRightType	TradingRight;
};

///經紀公司用戶
struct CThostFtdcBrokerUserField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///用戶名稱
	TThostFtdcUserNameType	UserName;
	///用戶類型
	TThostFtdcUserTypeType	UserType;
	///是否活躍
	TThostFtdcBoolType	IsActive;
	///是否使用權杖
	TThostFtdcBoolType	IsUsingOTP;
};

///經紀公司用戶口令
struct CThostFtdcBrokerUserPasswordField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///密碼
	TThostFtdcPasswordType	Password;
};

///經紀公司用戶功能許可權
struct CThostFtdcBrokerUserFunctionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///經紀公司功能代碼
	TThostFtdcBrokerFunctionCodeType	BrokerFunctionCode;
};

///交易所交易員報盤機
struct CThostFtdcTraderOfferField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///密碼
	TThostFtdcPasswordType	Password;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///本地報單編號
	TThostFtdcOrderLocalIDType	OrderLocalID;
	///交易所交易員連接狀態
	TThostFtdcTraderConnectStatusType	TraderConnectStatus;
	///發出連接請求的日期
	TThostFtdcDateType	ConnectRequestDate;
	///發出連接請求的時間
	TThostFtdcTimeType	ConnectRequestTime;
	///上次報告日期
	TThostFtdcDateType	LastReportDate;
	///上次報告時間
	TThostFtdcTimeType	LastReportTime;
	///完成連接日期
	TThostFtdcDateType	ConnectDate;
	///完成連線時間
	TThostFtdcTimeType	ConnectTime;
	///啟動日期
	TThostFtdcDateType	StartDate;
	///啟動時間
	TThostFtdcTimeType	StartTime;
	///交易日
	TThostFtdcDateType	TradingDay;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///本席位最大成交編號
	TThostFtdcTradeIDType	MaxTradeID;
	///本席位最大報單備拷
	TThostFtdcReturnCodeType	MaxOrderMessageReference;
};

///投資者結算結果
struct CThostFtdcSettlementInfoField
{
	///交易日
	TThostFtdcDateType	TradingDay;
	///結算編號
	TThostFtdcSettlementIDType	SettlementID;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///序號
	TThostFtdcSequenceNoType	SequenceNo;
	///訊息文字
	TThostFtdcContentType	Content;
};

///合約保證金率調整
struct CThostFtdcInstrumentMarginRateAdjustField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投資者範圍
	TThostFtdcInvestorRangeType	InvestorRange;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///多頭保證金率
	TThostFtdcRatioType	LongMarginRatioByMoney;
	///多頭保證金費
	TThostFtdcMoneyType	LongMarginRatioByVolume;
	///空頭保證金率
	TThostFtdcRatioType	ShortMarginRatioByMoney;
	///空頭保證金費
	TThostFtdcMoneyType	ShortMarginRatioByVolume;
	///是否相對交易所收取
	TThostFtdcBoolType	IsRelative;
};

///交易所保證金率
struct CThostFtdcExchangeMarginRateField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///多頭保證金率
	TThostFtdcRatioType	LongMarginRatioByMoney;
	///多頭保證金費
	TThostFtdcMoneyType	LongMarginRatioByVolume;
	///空頭保證金率
	TThostFtdcRatioType	ShortMarginRatioByMoney;
	///空頭保證金費
	TThostFtdcMoneyType	ShortMarginRatioByVolume;
};

///交易所保證金率調整
struct CThostFtdcExchangeMarginRateAdjustField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///跟隨交易所投資者多頭保證金率
	TThostFtdcRatioType	LongMarginRatioByMoney;
	///跟隨交易所投資者多頭保證金費
	TThostFtdcMoneyType	LongMarginRatioByVolume;
	///跟隨交易所投資者空頭保證金率
	TThostFtdcRatioType	ShortMarginRatioByMoney;
	///跟隨交易所投資者空頭保證金費
	TThostFtdcMoneyType	ShortMarginRatioByVolume;
	///交易所多頭保證金率
	TThostFtdcRatioType	ExchLongMarginRatioByMoney;
	///交易所多頭保證金費
	TThostFtdcMoneyType	ExchLongMarginRatioByVolume;
	///交易所空頭保證金率
	TThostFtdcRatioType	ExchShortMarginRatioByMoney;
	///交易所空頭保證金費
	TThostFtdcMoneyType	ExchShortMarginRatioByVolume;
	///不跟隨交易所投資者多頭保證金率
	TThostFtdcRatioType	NoLongMarginRatioByMoney;
	///不跟隨交易所投資者多頭保證金費
	TThostFtdcMoneyType	NoLongMarginRatioByVolume;
	///不跟隨交易所投資者空頭保證金率
	TThostFtdcRatioType	NoShortMarginRatioByMoney;
	///不跟隨交易所投資者空頭保證金費
	TThostFtdcMoneyType	NoShortMarginRatioByVolume;
};

///結算引用
struct CThostFtdcSettlementRefField
{
	///交易日
	TThostFtdcDateType	TradingDay;
	///結算編號
	TThostFtdcSettlementIDType	SettlementID;
};

///當前時間
struct CThostFtdcCurrentTimeField
{
	///當前日期
	TThostFtdcDateType	CurrDate;
	///當前時間
	TThostFtdcTimeType	CurrTime;
	///當前時間（毫秒）
	TThostFtdcMillisecType	CurrMillisec;
};

///通訊階段
struct CThostFtdcCommPhaseField
{
	///交易日
	TThostFtdcDateType	TradingDay;
	///通訊時段編號
	TThostFtdcCommPhaseNoType	CommPhaseNo;
};

///登錄信息
struct CThostFtdcLoginInfoField
{
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///登錄日期
	TThostFtdcDateType	LoginDate;
	///登錄時間
	TThostFtdcTimeType	LoginTime;
	///IP地址
	TThostFtdcIPAddressType	IPAddress;
	///使用者端產品資訊
	TThostFtdcProductInfoType	UserProductInfo;
	///介面端產品資訊
	TThostFtdcProductInfoType	InterfaceProductInfo;
	///協定資訊
	TThostFtdcProtocolInfoType	ProtocolInfo;
	///系統名稱
	TThostFtdcSystemNameType	SystemName;
	///密碼
	TThostFtdcPasswordType	Password;
	///最大報單引用
	TThostFtdcOrderRefType	MaxOrderRef;
	///上期所時間
	TThostFtdcTimeType	SHFETime;
	///大商所時間
	TThostFtdcTimeType	DCETime;
	///鄭商所時間
	TThostFtdcTimeType	CZCETime;
	///中金所時間
	TThostFtdcTimeType	FFEXTime;
	///Mac地址
	TThostFtdcMacAddressType	MacAddress;
	///動態密碼
	TThostFtdcPasswordType	OneTimePassword;
};

///登錄信息
struct CThostFtdcLogoutAllField
{
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///系統名稱
	TThostFtdcSystemNameType	SystemName;
};

///前置狀態
struct CThostFtdcFrontStatusField
{
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///上次報告日期
	TThostFtdcDateType	LastReportDate;
	///上次報告時間
	TThostFtdcTimeType	LastReportTime;
	///是否活躍
	TThostFtdcBoolType	IsActive;
};

///用戶口令變更
struct CThostFtdcUserPasswordUpdateField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///原來的口令
	TThostFtdcPasswordType	OldPassword;
	///新的口令
	TThostFtdcPasswordType	NewPassword;
};

///輸入報單
struct CThostFtdcInputOrderField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///報單引用
	TThostFtdcOrderRefType	OrderRef;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///報單價格條件
	TThostFtdcOrderPriceTypeType	OrderPriceType;
	///買賣方向
	TThostFtdcDirectionType	Direction;
	///組合開平標誌
	TThostFtdcCombOffsetFlagType	CombOffsetFlag;
	///組合投機套保標誌
	TThostFtdcCombHedgeFlagType	CombHedgeFlag;
	///價格
	TThostFtdcPriceType	LimitPrice;
	///數量
	TThostFtdcVolumeType	VolumeTotalOriginal;
	///有效期類型
	TThostFtdcTimeConditionType	TimeCondition;
	///GTD日期
	TThostFtdcDateType	GTDDate;
	///成交量類型
	TThostFtdcVolumeConditionType	VolumeCondition;
	///最小成交量
	TThostFtdcVolumeType	MinVolume;
	///觸發條件
	TThostFtdcContingentConditionType	ContingentCondition;
	///止損價
	TThostFtdcPriceType	StopPrice;
	///強平原因
	TThostFtdcForceCloseReasonType	ForceCloseReason;
	///自動掛起標誌
	TThostFtdcBoolType	IsAutoSuspend;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///使用者強評標誌
	TThostFtdcBoolType	UserForceClose;
	///互換單標誌
	TThostFtdcBoolType	IsSwapOrder;
};

///報單
struct CThostFtdcOrderField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///報單引用
	TThostFtdcOrderRefType	OrderRef;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///報單價格條件
	TThostFtdcOrderPriceTypeType	OrderPriceType;
	///買賣方向
	TThostFtdcDirectionType	Direction;
	///組合開平標誌
	TThostFtdcCombOffsetFlagType	CombOffsetFlag;
	///組合投機套保標誌
	TThostFtdcCombHedgeFlagType	CombHedgeFlag;
	///價格
	TThostFtdcPriceType	LimitPrice;
	///數量
	TThostFtdcVolumeType	VolumeTotalOriginal;
	///有效期類型
	TThostFtdcTimeConditionType	TimeCondition;
	///GTD日期
	TThostFtdcDateType	GTDDate;
	///成交量類型
	TThostFtdcVolumeConditionType	VolumeCondition;
	///最小成交量
	TThostFtdcVolumeType	MinVolume;
	///觸發條件
	TThostFtdcContingentConditionType	ContingentCondition;
	///止損價
	TThostFtdcPriceType	StopPrice;
	///強平原因
	TThostFtdcForceCloseReasonType	ForceCloseReason;
	///自動掛起標誌
	TThostFtdcBoolType	IsAutoSuspend;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///本地報單編號
	TThostFtdcOrderLocalIDType	OrderLocalID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///合約在交易所的代碼
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///報單提交狀態
	TThostFtdcOrderSubmitStatusType	OrderSubmitStatus;
	///報單提示序號
	TThostFtdcSequenceNoType	NotifySequence;
	///交易日
	TThostFtdcDateType	TradingDay;
	///結算編號
	TThostFtdcSettlementIDType	SettlementID;
	///報單編號
	TThostFtdcOrderSysIDType	OrderSysID;
	///報單來源
	TThostFtdcOrderSourceType	OrderSource;
	///報單狀態
	TThostFtdcOrderStatusType	OrderStatus;
	///報單類型
	TThostFtdcOrderTypeType	OrderType;
	///今成交數量
	TThostFtdcVolumeType	VolumeTraded;
	///剩餘數量
	TThostFtdcVolumeType	VolumeTotal;
	///報單日期
	TThostFtdcDateType	InsertDate;
	///委託時間
	TThostFtdcTimeType	InsertTime;
	///啟動時間
	TThostFtdcTimeType	ActiveTime;
	///掛起時間
	TThostFtdcTimeType	SuspendTime;
	///最後修改時間
	TThostFtdcTimeType	UpdateTime;
	///撤銷時間
	TThostFtdcTimeType	CancelTime;
	///最後修改交易所交易員代碼
	TThostFtdcTraderIDType	ActiveTraderID;
	///結算會員編號
	TThostFtdcParticipantIDType	ClearingPartID;
	///序號
	TThostFtdcSequenceNoType	SequenceNo;
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///使用者端產品資訊
	TThostFtdcProductInfoType	UserProductInfo;
	///狀態資訊
	TThostFtdcErrorMsgType	StatusMsg;
	///使用者強評標誌
	TThostFtdcBoolType	UserForceClose;
	///操作用戶代碼
	TThostFtdcUserIDType	ActiveUserID;
	///經紀公司報單編號
	TThostFtdcSequenceNoType	BrokerOrderSeq;
	///相關報單
	TThostFtdcOrderSysIDType	RelativeOrderSysID;
	///鄭商所成交數量
	TThostFtdcVolumeType	ZCETotalTradedVolume;
	///互換單標誌
	TThostFtdcBoolType	IsSwapOrder;
};

///交易所報單
struct CThostFtdcExchangeOrderField
{
	///報單價格條件
	TThostFtdcOrderPriceTypeType	OrderPriceType;
	///買賣方向
	TThostFtdcDirectionType	Direction;
	///組合開平標誌
	TThostFtdcCombOffsetFlagType	CombOffsetFlag;
	///組合投機套保標誌
	TThostFtdcCombHedgeFlagType	CombHedgeFlag;
	///價格
	TThostFtdcPriceType	LimitPrice;
	///數量
	TThostFtdcVolumeType	VolumeTotalOriginal;
	///有效期類型
	TThostFtdcTimeConditionType	TimeCondition;
	///GTD日期
	TThostFtdcDateType	GTDDate;
	///成交量類型
	TThostFtdcVolumeConditionType	VolumeCondition;
	///最小成交量
	TThostFtdcVolumeType	MinVolume;
	///觸發條件
	TThostFtdcContingentConditionType	ContingentCondition;
	///止損價
	TThostFtdcPriceType	StopPrice;
	///強平原因
	TThostFtdcForceCloseReasonType	ForceCloseReason;
	///自動掛起標誌
	TThostFtdcBoolType	IsAutoSuspend;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///本地報單編號
	TThostFtdcOrderLocalIDType	OrderLocalID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///合約在交易所的代碼
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///報單提交狀態
	TThostFtdcOrderSubmitStatusType	OrderSubmitStatus;
	///報單提示序號
	TThostFtdcSequenceNoType	NotifySequence;
	///交易日
	TThostFtdcDateType	TradingDay;
	///結算編號
	TThostFtdcSettlementIDType	SettlementID;
	///報單編號
	TThostFtdcOrderSysIDType	OrderSysID;
	///報單來源
	TThostFtdcOrderSourceType	OrderSource;
	///報單狀態
	TThostFtdcOrderStatusType	OrderStatus;
	///報單類型
	TThostFtdcOrderTypeType	OrderType;
	///今成交數量
	TThostFtdcVolumeType	VolumeTraded;
	///剩餘數量
	TThostFtdcVolumeType	VolumeTotal;
	///報單日期
	TThostFtdcDateType	InsertDate;
	///委託時間
	TThostFtdcTimeType	InsertTime;
	///啟動時間
	TThostFtdcTimeType	ActiveTime;
	///掛起時間
	TThostFtdcTimeType	SuspendTime;
	///最後修改時間
	TThostFtdcTimeType	UpdateTime;
	///撤銷時間
	TThostFtdcTimeType	CancelTime;
	///最後修改交易所交易員代碼
	TThostFtdcTraderIDType	ActiveTraderID;
	///結算會員編號
	TThostFtdcParticipantIDType	ClearingPartID;
	///序號
	TThostFtdcSequenceNoType	SequenceNo;
};

///交易所報單插入失敗
struct CThostFtdcExchangeOrderInsertErrorField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///本地報單編號
	TThostFtdcOrderLocalIDType	OrderLocalID;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///輸入報單操作
struct CThostFtdcInputOrderActionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///報單操作引用
	TThostFtdcOrderActionRefType	OrderActionRef;
	///報單引用
	TThostFtdcOrderRefType	OrderRef;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///報單編號
	TThostFtdcOrderSysIDType	OrderSysID;
	///操作標誌
	TThostFtdcActionFlagType	ActionFlag;
	///價格
	TThostFtdcPriceType	LimitPrice;
	///數量變化
	TThostFtdcVolumeType	VolumeChange;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
};

///報單操作
struct CThostFtdcOrderActionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///報單操作引用
	TThostFtdcOrderActionRefType	OrderActionRef;
	///報單引用
	TThostFtdcOrderRefType	OrderRef;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///報單編號
	TThostFtdcOrderSysIDType	OrderSysID;
	///操作標誌
	TThostFtdcActionFlagType	ActionFlag;
	///價格
	TThostFtdcPriceType	LimitPrice;
	///數量變化
	TThostFtdcVolumeType	VolumeChange;
	///操作日期
	TThostFtdcDateType	ActionDate;
	///操作時間
	TThostFtdcTimeType	ActionTime;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///本地報單編號
	TThostFtdcOrderLocalIDType	OrderLocalID;
	///操作本地編號
	TThostFtdcOrderLocalIDType	ActionLocalID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///報單操作狀態
	TThostFtdcOrderActionStatusType	OrderActionStatus;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///狀態資訊
	TThostFtdcErrorMsgType	StatusMsg;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
};

///交易所報單操作
struct CThostFtdcExchangeOrderActionField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///報單編號
	TThostFtdcOrderSysIDType	OrderSysID;
	///操作標誌
	TThostFtdcActionFlagType	ActionFlag;
	///價格
	TThostFtdcPriceType	LimitPrice;
	///數量變化
	TThostFtdcVolumeType	VolumeChange;
	///操作日期
	TThostFtdcDateType	ActionDate;
	///操作時間
	TThostFtdcTimeType	ActionTime;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///本地報單編號
	TThostFtdcOrderLocalIDType	OrderLocalID;
	///操作本地編號
	TThostFtdcOrderLocalIDType	ActionLocalID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///報單操作狀態
	TThostFtdcOrderActionStatusType	OrderActionStatus;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
};

///交易所報單操作失敗
struct CThostFtdcExchangeOrderActionErrorField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///報單編號
	TThostFtdcOrderSysIDType	OrderSysID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///本地報單編號
	TThostFtdcOrderLocalIDType	OrderLocalID;
	///操作本地編號
	TThostFtdcOrderLocalIDType	ActionLocalID;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///交易所成交
struct CThostFtdcExchangeTradeField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///成交編號
	TThostFtdcTradeIDType	TradeID;
	///買賣方向
	TThostFtdcDirectionType	Direction;
	///報單編號
	TThostFtdcOrderSysIDType	OrderSysID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///交易角色
	TThostFtdcTradingRoleType	TradingRole;
	///合約在交易所的代碼
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///開平標誌
	TThostFtdcOffsetFlagType	OffsetFlag;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///價格
	TThostFtdcPriceType	Price;
	///數量
	TThostFtdcVolumeType	Volume;
	///成交時期
	TThostFtdcDateType	TradeDate;
	///成交時間
	TThostFtdcTimeType	TradeTime;
	///成交類型
	TThostFtdcTradeTypeType	TradeType;
	///成交價來源
	TThostFtdcPriceSourceType	PriceSource;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///本地報單編號
	TThostFtdcOrderLocalIDType	OrderLocalID;
	///結算會員編號
	TThostFtdcParticipantIDType	ClearingPartID;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///序號
	TThostFtdcSequenceNoType	SequenceNo;
	///成交來源
	TThostFtdcTradeSourceType	TradeSource;
};

///成交
struct CThostFtdcTradeField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///報單引用
	TThostFtdcOrderRefType	OrderRef;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///成交編號
	TThostFtdcTradeIDType	TradeID;
	///買賣方向
	TThostFtdcDirectionType	Direction;
	///報單編號
	TThostFtdcOrderSysIDType	OrderSysID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///交易角色
	TThostFtdcTradingRoleType	TradingRole;
	///合約在交易所的代碼
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///開平標誌
	TThostFtdcOffsetFlagType	OffsetFlag;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///價格
	TThostFtdcPriceType	Price;
	///數量
	TThostFtdcVolumeType	Volume;
	///成交時期
	TThostFtdcDateType	TradeDate;
	///成交時間
	TThostFtdcTimeType	TradeTime;
	///成交類型
	TThostFtdcTradeTypeType	TradeType;
	///成交價來源
	TThostFtdcPriceSourceType	PriceSource;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///本地報單編號
	TThostFtdcOrderLocalIDType	OrderLocalID;
	///結算會員編號
	TThostFtdcParticipantIDType	ClearingPartID;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///序號
	TThostFtdcSequenceNoType	SequenceNo;
	///交易日
	TThostFtdcDateType	TradingDay;
	///結算編號
	TThostFtdcSettlementIDType	SettlementID;
	///經紀公司報單編號
	TThostFtdcSequenceNoType	BrokerOrderSeq;
	///成交來源
	TThostFtdcTradeSourceType	TradeSource;
};

///用戶會話
struct CThostFtdcUserSessionField
{
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///登錄日期
	TThostFtdcDateType	LoginDate;
	///登錄時間
	TThostFtdcTimeType	LoginTime;
	///IP地址
	TThostFtdcIPAddressType	IPAddress;
	///使用者端產品資訊
	TThostFtdcProductInfoType	UserProductInfo;
	///介面端產品資訊
	TThostFtdcProductInfoType	InterfaceProductInfo;
	///協定資訊
	TThostFtdcProtocolInfoType	ProtocolInfo;
	///Mac地址
	TThostFtdcMacAddressType	MacAddress;
};

///查詢最大報單數量
struct CThostFtdcQueryMaxOrderVolumeField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///買賣方向
	TThostFtdcDirectionType	Direction;
	///開平標誌
	TThostFtdcOffsetFlagType	OffsetFlag;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///最大允許報單數量
	TThostFtdcVolumeType	MaxVolume;
};

///投資者結算結果確認資訊
struct CThostFtdcSettlementInfoConfirmField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///確認日期
	TThostFtdcDateType	ConfirmDate;
	///確認時間
	TThostFtdcTimeType	ConfirmTime;
};

///出入金同步
struct CThostFtdcSyncDepositField
{
	///出入金流水號
	TThostFtdcDepositSeqNoType	DepositSeqNo;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///入金金額
	TThostFtdcMoneyType	Deposit;
	///是否強制進行
	TThostFtdcBoolType	IsForce;
};

///經紀公司同步
struct CThostFtdcBrokerSyncField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
};

///正在同步中的投資者
struct CThostFtdcSyncingInvestorField
{
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者分組代碼
	TThostFtdcInvestorIDType	InvestorGroupID;
	///投資者名稱
	TThostFtdcPartyNameType	InvestorName;
	///證件類型
	TThostFtdcIdCardTypeType	IdentifiedCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///是否活躍
	TThostFtdcBoolType	IsActive;
	///聯繫電話
	TThostFtdcTelephoneType	Telephone;
	///通訊位址
	TThostFtdcAddressType	Address;
	///開戶日期
	TThostFtdcDateType	OpenDate;
	///手機
	TThostFtdcMobileType	Mobile;
	///手續費率範本代碼
	TThostFtdcInvestorIDType	CommModelID;
	///保證金率範本代碼
	TThostFtdcInvestorIDType	MarginModelID;
};

///正在同步中的交易代碼
struct CThostFtdcSyncingTradingCodeField
{
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///是否活躍
	TThostFtdcBoolType	IsActive;
	///交易編碼類型
	TThostFtdcClientIDTypeType	ClientIDType;
};

///正在同步中的投資者分組
struct CThostFtdcSyncingInvestorGroupField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者分組代碼
	TThostFtdcInvestorIDType	InvestorGroupID;
	///投資者分組名稱
	TThostFtdcInvestorGroupNameType	InvestorGroupName;
};

///正在同步中的交易帳號
struct CThostFtdcSyncingTradingAccountField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///上次質押金額
	TThostFtdcMoneyType	PreMortgage;
	///上次信用額度
	TThostFtdcMoneyType	PreCredit;
	///上次存款額
	TThostFtdcMoneyType	PreDeposit;
	///上次結算準備金
	TThostFtdcMoneyType	PreBalance;
	///上次佔用的保證金
	TThostFtdcMoneyType	PreMargin;
	///利息基數
	TThostFtdcMoneyType	InterestBase;
	///利息收入
	TThostFtdcMoneyType	Interest;
	///入金金額
	TThostFtdcMoneyType	Deposit;
	///出金金額
	TThostFtdcMoneyType	Withdraw;
	///凍結的保證金
	TThostFtdcMoneyType	FrozenMargin;
	///凍結的資金
	TThostFtdcMoneyType	FrozenCash;
	///凍結的手續費
	TThostFtdcMoneyType	FrozenCommission;
	///當前保證金總額
	TThostFtdcMoneyType	CurrMargin;
	///資金差額
	TThostFtdcMoneyType	CashIn;
	///手續費
	TThostFtdcMoneyType	Commission;
	///平倉盈虧
	TThostFtdcMoneyType	CloseProfit;
	///持倉盈虧
	TThostFtdcMoneyType	PositionProfit;
	///期貨結算準備金
	TThostFtdcMoneyType	Balance;
	///可用資金
	TThostFtdcMoneyType	Available;
	///可取資金
	TThostFtdcMoneyType	WithdrawQuota;
	///基本準備金
	TThostFtdcMoneyType	Reserve;
	///交易日
	TThostFtdcDateType	TradingDay;
	///結算編號
	TThostFtdcSettlementIDType	SettlementID;
	///信用額度
	TThostFtdcMoneyType	Credit;
	///質押金額
	TThostFtdcMoneyType	Mortgage;
	///交易所保證金
	TThostFtdcMoneyType	ExchangeMargin;
	///投資者交割保證金
	TThostFtdcMoneyType	DeliveryMargin;
	///交易所交割保證金
	TThostFtdcMoneyType	ExchangeDeliveryMargin;
	///期權平倉盈虧
	TThostFtdcMoneyType	OptionCloseProfit;
};

///正在同步中的投資者持倉
struct CThostFtdcSyncingInvestorPositionField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///持倉多空方向
	TThostFtdcPosiDirectionType	PosiDirection;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///持倉日期
	TThostFtdcPositionDateType	PositionDate;
	///上日持倉
	TThostFtdcVolumeType	YdPosition;
	///今日持倉
	TThostFtdcVolumeType	Position;
	///多頭凍結
	TThostFtdcVolumeType	LongFrozen;
	///空頭凍結
	TThostFtdcVolumeType	ShortFrozen;
	///開倉凍結金額
	TThostFtdcMoneyType	LongFrozenAmount;
	///開倉凍結金額
	TThostFtdcMoneyType	ShortFrozenAmount;
	///開倉量
	TThostFtdcVolumeType	OpenVolume;
	///平倉量
	TThostFtdcVolumeType	CloseVolume;
	///開倉金額
	TThostFtdcMoneyType	OpenAmount;
	///平倉金額
	TThostFtdcMoneyType	CloseAmount;
	///持倉成本
	TThostFtdcMoneyType	PositionCost;
	///上次佔用的保證金
	TThostFtdcMoneyType	PreMargin;
	///佔用的保證金
	TThostFtdcMoneyType	UseMargin;
	///凍結的保證金
	TThostFtdcMoneyType	FrozenMargin;
	///凍結的資金
	TThostFtdcMoneyType	FrozenCash;
	///凍結的手續費
	TThostFtdcMoneyType	FrozenCommission;
	///資金差額
	TThostFtdcMoneyType	CashIn;
	///手續費
	TThostFtdcMoneyType	Commission;
	///平倉盈虧
	TThostFtdcMoneyType	CloseProfit;
	///持倉盈虧
	TThostFtdcMoneyType	PositionProfit;
	///上次結算價
	TThostFtdcPriceType	PreSettlementPrice;
	///本次結算價
	TThostFtdcPriceType	SettlementPrice;
	///交易日
	TThostFtdcDateType	TradingDay;
	///結算編號
	TThostFtdcSettlementIDType	SettlementID;
	///開倉成本
	TThostFtdcMoneyType	OpenCost;
	///交易所保證金
	TThostFtdcMoneyType	ExchangeMargin;
	///組合成交形成的持倉
	TThostFtdcVolumeType	CombPosition;
	///組合多頭凍結
	TThostFtdcVolumeType	CombLongFrozen;
	///組合空頭凍結
	TThostFtdcVolumeType	CombShortFrozen;
	///逐日盯市平倉盈虧
	TThostFtdcMoneyType	CloseProfitByDate;
	///逐筆對沖平倉盈虧
	TThostFtdcMoneyType	CloseProfitByTrade;
	///今日持倉
	TThostFtdcVolumeType	TodayPosition;
	///保證金率
	TThostFtdcRatioType	MarginRateByMoney;
	///保證金率(按手數)
	TThostFtdcRatioType	MarginRateByVolume;
	///執行凍結
	TThostFtdcVolumeType	StrikeFrozen;
	///執行凍結金額
	TThostFtdcMoneyType	StrikeFrozenAmount;
};

///正在同步中的合約保證金率
struct CThostFtdcSyncingInstrumentMarginRateField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投資者範圍
	TThostFtdcInvestorRangeType	InvestorRange;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///多頭保證金率
	TThostFtdcRatioType	LongMarginRatioByMoney;
	///多頭保證金費
	TThostFtdcMoneyType	LongMarginRatioByVolume;
	///空頭保證金率
	TThostFtdcRatioType	ShortMarginRatioByMoney;
	///空頭保證金費
	TThostFtdcMoneyType	ShortMarginRatioByVolume;
	///是否相對交易所收取
	TThostFtdcBoolType	IsRelative;
};

///正在同步中的合約手續費率
struct CThostFtdcSyncingInstrumentCommissionRateField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投資者範圍
	TThostFtdcInvestorRangeType	InvestorRange;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///開倉手續費率
	TThostFtdcRatioType	OpenRatioByMoney;
	///開倉手續費
	TThostFtdcRatioType	OpenRatioByVolume;
	///平倉手續費率
	TThostFtdcRatioType	CloseRatioByMoney;
	///平倉手續費
	TThostFtdcRatioType	CloseRatioByVolume;
	///平今手續費率
	TThostFtdcRatioType	CloseTodayRatioByMoney;
	///平今手續費
	TThostFtdcRatioType	CloseTodayRatioByVolume;
};

///正在同步中的合約交易許可權
struct CThostFtdcSyncingInstrumentTradingRightField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投資者範圍
	TThostFtdcInvestorRangeType	InvestorRange;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///交易許可權
	TThostFtdcTradingRightType	TradingRight;
};

///查詢報單
struct CThostFtdcQryOrderField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///報單編號
	TThostFtdcOrderSysIDType	OrderSysID;
	///開始時間
	TThostFtdcTimeType	InsertTimeStart;
	///結束時間
	TThostFtdcTimeType	InsertTimeEnd;
};

///查詢成交
struct CThostFtdcQryTradeField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///成交編號
	TThostFtdcTradeIDType	TradeID;
	///開始時間
	TThostFtdcTimeType	TradeTimeStart;
	///結束時間
	TThostFtdcTimeType	TradeTimeEnd;
};

///查詢投資者持倉
struct CThostFtdcQryInvestorPositionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
};

///查詢資金帳戶
struct CThostFtdcQryTradingAccountField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
};

///查詢投資者
struct CThostFtdcQryInvestorField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
};

///查詢交易編碼
struct CThostFtdcQryTradingCodeField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///交易編碼類型
	TThostFtdcClientIDTypeType	ClientIDType;
};

///查詢交易編碼
struct CThostFtdcQryInvestorGroupField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
};

///查詢交易編碼
struct CThostFtdcQryInstrumentMarginRateField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
};

///查詢交易編碼
struct CThostFtdcQryInstrumentCommissionRateField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
};

///查詢交易編碼
struct CThostFtdcQryInstrumentTradingRightField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
};

///查詢經紀公司
struct CThostFtdcQryBrokerField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
};

///查詢交易員
struct CThostFtdcQryTraderField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
};

///查詢經紀公司會員代碼
struct CThostFtdcQryPartBrokerField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
};

///查詢管理用戶功能許可權
struct CThostFtdcQrySuperUserFunctionField
{
	///用戶代碼
	TThostFtdcUserIDType	UserID;
};

///查詢用戶會話
struct CThostFtdcQryUserSessionField
{
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
};

///查詢前置狀態
struct CThostFtdcQryFrontStatusField
{
	///前置編號
	TThostFtdcFrontIDType	FrontID;
};

///查詢交易所報單
struct CThostFtdcQryExchangeOrderField
{
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///合約在交易所的代碼
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
};

///查詢報單操作
struct CThostFtdcQryOrderActionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
};

///查詢交易所報單操作
struct CThostFtdcQryExchangeOrderActionField
{
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
};

///查詢管理用戶
struct CThostFtdcQrySuperUserField
{
	///用戶代碼
	TThostFtdcUserIDType	UserID;
};

///查詢交易所
struct CThostFtdcQryExchangeField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
};

///查詢產品
struct CThostFtdcQryProductField
{
	///產品代碼
	TThostFtdcInstrumentIDType	ProductID;
};

///查詢合約
struct CThostFtdcQryInstrumentField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///合約在交易所的代碼
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///產品代碼
	TThostFtdcInstrumentIDType	ProductID;
};

///查詢行情
struct CThostFtdcQryDepthMarketDataField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
};

///查詢經紀公司用戶
struct CThostFtdcQryBrokerUserField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
};

///查詢經紀公司用戶許可權
struct CThostFtdcQryBrokerUserFunctionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
};

///查詢交易員報盤機
struct CThostFtdcQryTraderOfferField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
};

///查詢出入金流水
struct CThostFtdcQrySyncDepositField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///出入金流水號
	TThostFtdcDepositSeqNoType	DepositSeqNo;
};

///查詢投資者結算結果
struct CThostFtdcQrySettlementInfoField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///交易日
	TThostFtdcDateType	TradingDay;
};

///查詢報單
struct CThostFtdcQryHisOrderField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///報單編號
	TThostFtdcOrderSysIDType	OrderSysID;
	///開始時間
	TThostFtdcTimeType	InsertTimeStart;
	///結束時間
	TThostFtdcTimeType	InsertTimeEnd;
	///交易日
	TThostFtdcDateType	TradingDay;
	///結算編號
	TThostFtdcSettlementIDType	SettlementID;
};

///當前期權合約最小保證金
struct CThostFtdcOptionInstrMiniMarginField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投資者範圍
	TThostFtdcInvestorRangeType	InvestorRange;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///單位（手）期權合約最小保證金
	TThostFtdcMoneyType	MinMargin;
};

///當前期權合約最小保障係數
struct CThostFtdcOptionInstrMiniGuaranteeField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投資者範圍
	TThostFtdcInvestorRangeType	InvestorRange;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///單位（手）期權合約最小保障係數
	TThostFtdcRatioType	MinGuarantee;
};

///當前期權合約保證金調整係數
struct CThostFtdcOptionInstrMarginAdjustField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投資者範圍
	TThostFtdcInvestorRangeType	InvestorRange;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///投機空頭保證金調整係數
	TThostFtdcRatioType	SShortMarginRatioByMoney;
	///投機空頭保證金調整係數
	TThostFtdcMoneyType	SShortMarginRatioByVolume;
	///保值空頭保證金調整係數
	TThostFtdcRatioType	HShortMarginRatioByMoney;
	///保值空頭保證金調整係數
	TThostFtdcMoneyType	HShortMarginRatioByVolume;
	///是否跟隨交易所收取
	TThostFtdcBoolType	IsRelative;
};

///當前期權合約手續費的詳細內容
struct CThostFtdcOptionInstrCommRateField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投資者範圍
	TThostFtdcInvestorRangeType	InvestorRange;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///開倉手續費率
	TThostFtdcRatioType	OpenRatioByMoney;
	///開倉手續費
	TThostFtdcRatioType	OpenRatioByVolume;
	///平倉手續費率
	TThostFtdcRatioType	CloseRatioByMoney;
	///平倉手續費
	TThostFtdcRatioType	CloseRatioByVolume;
	///平今手續費率
	TThostFtdcRatioType	CloseTodayRatioByMoney;
	///平今手續費
	TThostFtdcRatioType	CloseTodayRatioByVolume;
	///執行手續費率
	TThostFtdcRatioType	StrikeRatioByMoney;
	///執行手續費
	TThostFtdcRatioType	StrikeRatioByVolume;
};

///期權交易成本
struct CThostFtdcOptionInstrTradeCostField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///期權合約保證金
	TThostFtdcMoneyType	Margin;
	///期權合約權利金
	TThostFtdcMoneyType	Royalty;
};

///期權交易成本查詢
struct CThostFtdcQryOptionInstrTradeCostField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///期權合約報價
	TThostFtdcPriceType	InputPrice;
};

///期權手續費率查詢
struct CThostFtdcQryOptionInstrCommRateField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
};

///股指現貨指數
struct CThostFtdcIndexPriceField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///指數現貨收盤價
	TThostFtdcPriceType	ClosePrice;
};

///輸入的執行宣告
struct CThostFtdcInputExecOrderField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///執行宣告引用
	TThostFtdcOrderRefType	ExecOrderRef;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///數量
	TThostFtdcVolumeType	Volume;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///開平標誌
	TThostFtdcOffsetFlagType	OffsetFlag;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
};

///輸入執行宣告操作
struct CThostFtdcInputExecOrderActionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///執行宣告操作引用
	TThostFtdcOrderActionRefType	ExecOrderActionRef;
	///執行宣告引用
	TThostFtdcOrderRefType	ExecOrderRef;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///執行宣告操作編號
	TThostFtdcExecOrderSysIDType	ExecOrderSysID;
	///操作標誌
	TThostFtdcActionFlagType	ActionFlag;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
};

///執行宣告
struct CThostFtdcExecOrderField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///執行宣告引用
	TThostFtdcOrderRefType	ExecOrderRef;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///數量
	TThostFtdcVolumeType	Volume;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///開平標誌
	TThostFtdcOffsetFlagType	OffsetFlag;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///本地執行宣告編號
	TThostFtdcOrderLocalIDType	ExecOrderLocalID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///合約在交易所的代碼
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///執行宣告提交狀態
	TThostFtdcOrderSubmitStatusType	OrderSubmitStatus;
	///報單提示序號
	TThostFtdcSequenceNoType	NotifySequence;
	///交易日
	TThostFtdcDateType	TradingDay;
	///結算編號
	TThostFtdcSettlementIDType	SettlementID;
	///執行宣告編號
	TThostFtdcExecOrderSysIDType	ExecOrderSysID;
	///報單日期
	TThostFtdcDateType	InsertDate;
	///插入時間
	TThostFtdcTimeType	InsertTime;
	///撤銷時間
	TThostFtdcTimeType	CancelTime;
	///執行結果
	TThostFtdcExecResultType	ExecResult;
	///結算會員編號
	TThostFtdcParticipantIDType	ClearingPartID;
	///序號
	TThostFtdcSequenceNoType	SequenceNo;
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///使用者端產品資訊
	TThostFtdcProductInfoType	UserProductInfo;
	///狀態資訊
	TThostFtdcErrorMsgType	StatusMsg;
	///操作用戶代碼
	TThostFtdcUserIDType	ActiveUserID;
	///經紀公司報單編號
	TThostFtdcSequenceNoType	BrokerExecOrderSeq;
};

///執行宣告操作
struct CThostFtdcExecOrderActionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///執行宣告操作引用
	TThostFtdcOrderActionRefType	ExecOrderActionRef;
	///執行宣告引用
	TThostFtdcOrderRefType	ExecOrderRef;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///執行宣告操作編號
	TThostFtdcExecOrderSysIDType	ExecOrderSysID;
	///操作標誌
	TThostFtdcActionFlagType	ActionFlag;
	///操作日期
	TThostFtdcDateType	ActionDate;
	///操作時間
	TThostFtdcTimeType	ActionTime;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///本地執行宣告編號
	TThostFtdcOrderLocalIDType	ExecOrderLocalID;
	///操作本地編號
	TThostFtdcOrderLocalIDType	ActionLocalID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///報單操作狀態
	TThostFtdcOrderActionStatusType	OrderActionStatus;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///狀態資訊
	TThostFtdcErrorMsgType	StatusMsg;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
};

///執行宣告查詢
struct CThostFtdcQryExecOrderField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///執行宣告編號
	TThostFtdcExecOrderSysIDType	ExecOrderSysID;
	///開始時間
	TThostFtdcTimeType	InsertTimeStart;
	///結束時間
	TThostFtdcTimeType	InsertTimeEnd;
};

///交易所執行宣告資訊
struct CThostFtdcExchangeExecOrderField
{
	///數量
	TThostFtdcVolumeType	Volume;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///開平標誌
	TThostFtdcOffsetFlagType	OffsetFlag;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///本地執行宣告編號
	TThostFtdcOrderLocalIDType	ExecOrderLocalID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///合約在交易所的代碼
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///執行宣告提交狀態
	TThostFtdcOrderSubmitStatusType	OrderSubmitStatus;
	///報單提示序號
	TThostFtdcSequenceNoType	NotifySequence;
	///交易日
	TThostFtdcDateType	TradingDay;
	///結算編號
	TThostFtdcSettlementIDType	SettlementID;
	///執行宣告編號
	TThostFtdcExecOrderSysIDType	ExecOrderSysID;
	///報單日期
	TThostFtdcDateType	InsertDate;
	///插入時間
	TThostFtdcTimeType	InsertTime;
	///撤銷時間
	TThostFtdcTimeType	CancelTime;
	///執行結果
	TThostFtdcExecResultType	ExecResult;
	///結算會員編號
	TThostFtdcParticipantIDType	ClearingPartID;
	///序號
	TThostFtdcSequenceNoType	SequenceNo;
};

///交易所執行宣告查詢
struct CThostFtdcQryExchangeExecOrderField
{
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///合約在交易所的代碼
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
};

///執行宣告操作查詢
struct CThostFtdcQryExecOrderActionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
};

///交易所執行宣告操作
struct CThostFtdcExchangeExecOrderActionField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///執行宣告操作編號
	TThostFtdcExecOrderSysIDType	ExecOrderSysID;
	///操作標誌
	TThostFtdcActionFlagType	ActionFlag;
	///操作日期
	TThostFtdcDateType	ActionDate;
	///操作時間
	TThostFtdcTimeType	ActionTime;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///本地執行宣告編號
	TThostFtdcOrderLocalIDType	ExecOrderLocalID;
	///操作本地編號
	TThostFtdcOrderLocalIDType	ActionLocalID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///報單操作狀態
	TThostFtdcOrderActionStatusType	OrderActionStatus;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
};

///交易所執行宣告操作查詢
struct CThostFtdcQryExchangeExecOrderActionField
{
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
};

///錯誤執行宣告
struct CThostFtdcErrExecOrderField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///執行宣告引用
	TThostFtdcOrderRefType	ExecOrderRef;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///數量
	TThostFtdcVolumeType	Volume;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///開平標誌
	TThostFtdcOffsetFlagType	OffsetFlag;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///查詢錯誤執行宣告
struct CThostFtdcQryErrExecOrderField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
};

///錯誤執行宣告操作
struct CThostFtdcErrExecOrderActionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///執行宣告操作引用
	TThostFtdcOrderActionRefType	ExecOrderActionRef;
	///執行宣告引用
	TThostFtdcOrderRefType	ExecOrderRef;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///執行宣告操作編號
	TThostFtdcExecOrderSysIDType	ExecOrderSysID;
	///操作標誌
	TThostFtdcActionFlagType	ActionFlag;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///查詢錯誤執行宣告操作
struct CThostFtdcQryErrExecOrderActionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
};

///市場行情
struct CThostFtdcMarketDataField
{
	///交易日
	TThostFtdcDateType	TradingDay;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///合約在交易所的代碼
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///最新價
	TThostFtdcPriceType	LastPrice;
	///上次結算價
	TThostFtdcPriceType	PreSettlementPrice;
	///昨收盤
	TThostFtdcPriceType	PreClosePrice;
	///昨持倉量
	TThostFtdcLargeVolumeType	PreOpenInterest;
	///今開盤
	TThostFtdcPriceType	OpenPrice;
	///最高價
	TThostFtdcPriceType	HighestPrice;
	///最低價
	TThostFtdcPriceType	LowestPrice;
	///數量
	TThostFtdcVolumeType	Volume;
	///成交金額
	TThostFtdcMoneyType	Turnover;
	///持倉量
	TThostFtdcLargeVolumeType	OpenInterest;
	///今收盤
	TThostFtdcPriceType	ClosePrice;
	///本次結算價
	TThostFtdcPriceType	SettlementPrice;
	///漲停板價
	TThostFtdcPriceType	UpperLimitPrice;
	///跌停板價
	TThostFtdcPriceType	LowerLimitPrice;
	///昨虛實度
	TThostFtdcRatioType	PreDelta;
	///今虛實度
	TThostFtdcRatioType	CurrDelta;
	///最後修改時間
	TThostFtdcTimeType	UpdateTime;
	///最後修改毫秒
	TThostFtdcMillisecType	UpdateMillisec;
};

///行情基礎屬性
struct CThostFtdcMarketDataBaseField
{
	///交易日
	TThostFtdcDateType	TradingDay;
	///上次結算價
	TThostFtdcPriceType	PreSettlementPrice;
	///昨收盤
	TThostFtdcPriceType	PreClosePrice;
	///昨持倉量
	TThostFtdcLargeVolumeType	PreOpenInterest;
	///昨虛實度
	TThostFtdcRatioType	PreDelta;
};

///行情靜態屬性
struct CThostFtdcMarketDataStaticField
{
	///今開盤
	TThostFtdcPriceType	OpenPrice;
	///最高價
	TThostFtdcPriceType	HighestPrice;
	///最低價
	TThostFtdcPriceType	LowestPrice;
	///今收盤
	TThostFtdcPriceType	ClosePrice;
	///漲停板價
	TThostFtdcPriceType	UpperLimitPrice;
	///跌停板價
	TThostFtdcPriceType	LowerLimitPrice;
	///本次結算價
	TThostFtdcPriceType	SettlementPrice;
	///今虛實度
	TThostFtdcRatioType	CurrDelta;
};

///行情最新成交屬性
struct CThostFtdcMarketDataLastMatchField
{
	///最新價
	TThostFtdcPriceType	LastPrice;
	///數量
	TThostFtdcVolumeType	Volume;
	///成交金額
	TThostFtdcMoneyType	Turnover;
	///持倉量
	TThostFtdcLargeVolumeType	OpenInterest;
};

///行情最優價屬性
struct CThostFtdcMarketDataBestPriceField
{
	///申買價一
	TThostFtdcPriceType	BidPrice1;
	///申買量一
	TThostFtdcVolumeType	BidVolume1;
	///申賣價一
	TThostFtdcPriceType	AskPrice1;
	///申賣量一
	TThostFtdcVolumeType	AskVolume1;
};

///行情申買二、三屬性
struct CThostFtdcMarketDataBid23Field
{
	///申買價二
	TThostFtdcPriceType	BidPrice2;
	///申買量二
	TThostFtdcVolumeType	BidVolume2;
	///申買價三
	TThostFtdcPriceType	BidPrice3;
	///申買量三
	TThostFtdcVolumeType	BidVolume3;
};

///行情申賣二、三屬性
struct CThostFtdcMarketDataAsk23Field
{
	///申賣價二
	TThostFtdcPriceType	AskPrice2;
	///申賣量二
	TThostFtdcVolumeType	AskVolume2;
	///申賣價三
	TThostFtdcPriceType	AskPrice3;
	///申賣量三
	TThostFtdcVolumeType	AskVolume3;
};

///行情申買四、五屬性
struct CThostFtdcMarketDataBid45Field
{
	///申買價四
	TThostFtdcPriceType	BidPrice4;
	///申買量四
	TThostFtdcVolumeType	BidVolume4;
	///申買價五
	TThostFtdcPriceType	BidPrice5;
	///申買量五
	TThostFtdcVolumeType	BidVolume5;
};

///行情申賣四、五屬性
struct CThostFtdcMarketDataAsk45Field
{
	///申賣價四
	TThostFtdcPriceType	AskPrice4;
	///申賣量四
	TThostFtdcVolumeType	AskVolume4;
	///申賣價五
	TThostFtdcPriceType	AskPrice5;
	///申賣量五
	TThostFtdcVolumeType	AskVolume5;
};

///行情更新時間屬性
struct CThostFtdcMarketDataUpdateTimeField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///最後修改時間
	TThostFtdcTimeType	UpdateTime;
	///最後修改毫秒
	TThostFtdcMillisecType	UpdateMillisec;
};

///行情交易所代碼屬性
struct CThostFtdcMarketDataExchangeField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
};

///指定的合約
struct CThostFtdcSpecificInstrumentField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
};

///合約狀態
struct CThostFtdcInstrumentStatusField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///合約在交易所的代碼
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///結算組代碼
	TThostFtdcSettlementGroupIDType	SettlementGroupID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///合約交易狀態
	TThostFtdcInstrumentStatusType	InstrumentStatus;
	///交易階段編號
	TThostFtdcTradingSegmentSNType	TradingSegmentSN;
	///進入本狀態時間
	TThostFtdcTimeType	EnterTime;
	///進入本狀態原因
	TThostFtdcInstStatusEnterReasonType	EnterReason;
};

///查詢合約狀態
struct CThostFtdcQryInstrumentStatusField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///合約在交易所的代碼
	TThostFtdcExchangeInstIDType	ExchangeInstID;
};

///投資者帳戶
struct CThostFtdcInvestorAccountField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
};

///浮動盈虧演算法
struct CThostFtdcPositionProfitAlgorithmField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///盈虧演算法
	TThostFtdcAlgorithmType	Algorithm;
	///備註
	TThostFtdcMemoType	Memo;
};

///會員資金折扣
struct CThostFtdcDiscountField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者範圍
	TThostFtdcInvestorRangeType	InvestorRange;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///資金折扣比例
	TThostFtdcRatioType	Discount;
};

///查詢轉帳銀行
struct CThostFtdcQryTransferBankField
{
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分中心代碼
	TThostFtdcBankBrchIDType	BankBrchID;
};

///轉帳銀行
struct CThostFtdcTransferBankField
{
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分中心代碼
	TThostFtdcBankBrchIDType	BankBrchID;
	///銀行名稱
	TThostFtdcBankNameType	BankName;
	///是否活躍
	TThostFtdcBoolType	IsActive;
};

///查詢投資者持倉明細
struct CThostFtdcQryInvestorPositionDetailField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
};

///投資者持倉明細
struct CThostFtdcInvestorPositionDetailField
{
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///買賣
	TThostFtdcDirectionType	Direction;
	///開倉日期
	TThostFtdcDateType	OpenDate;
	///成交編號
	TThostFtdcTradeIDType	TradeID;
	///數量
	TThostFtdcVolumeType	Volume;
	///開倉價
	TThostFtdcPriceType	OpenPrice;
	///交易日
	TThostFtdcDateType	TradingDay;
	///結算編號
	TThostFtdcSettlementIDType	SettlementID;
	///成交類型
	TThostFtdcTradeTypeType	TradeType;
	///組合合約代碼
	TThostFtdcInstrumentIDType	CombInstrumentID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///逐日盯市平倉盈虧
	TThostFtdcMoneyType	CloseProfitByDate;
	///逐筆對沖平倉盈虧
	TThostFtdcMoneyType	CloseProfitByTrade;
	///逐日盯市持倉盈虧
	TThostFtdcMoneyType	PositionProfitByDate;
	///逐筆對沖持倉盈虧
	TThostFtdcMoneyType	PositionProfitByTrade;
	///投資者保證金
	TThostFtdcMoneyType	Margin;
	///交易所保證金
	TThostFtdcMoneyType	ExchMargin;
	///保證金率
	TThostFtdcRatioType	MarginRateByMoney;
	///保證金率(按手數)
	TThostFtdcRatioType	MarginRateByVolume;
	///昨結算價
	TThostFtdcPriceType	LastSettlementPrice;
	///結算價
	TThostFtdcPriceType	SettlementPrice;
	///平倉量
	TThostFtdcVolumeType	CloseVolume;
	///平倉金額
	TThostFtdcMoneyType	CloseAmount;
};

///資金帳戶口令域
struct CThostFtdcTradingAccountPasswordField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///密碼
	TThostFtdcPasswordType	Password;
};

///交易所行情報盤機
struct CThostFtdcMDTraderOfferField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///密碼
	TThostFtdcPasswordType	Password;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///本地報單編號
	TThostFtdcOrderLocalIDType	OrderLocalID;
	///交易所交易員連接狀態
	TThostFtdcTraderConnectStatusType	TraderConnectStatus;
	///發出連接請求的日期
	TThostFtdcDateType	ConnectRequestDate;
	///發出連接請求的時間
	TThostFtdcTimeType	ConnectRequestTime;
	///上次報告日期
	TThostFtdcDateType	LastReportDate;
	///上次報告時間
	TThostFtdcTimeType	LastReportTime;
	///完成連接日期
	TThostFtdcDateType	ConnectDate;
	///完成連線時間
	TThostFtdcTimeType	ConnectTime;
	///啟動日期
	TThostFtdcDateType	StartDate;
	///啟動時間
	TThostFtdcTimeType	StartTime;
	///交易日
	TThostFtdcDateType	TradingDay;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///本席位最大成交編號
	TThostFtdcTradeIDType	MaxTradeID;
	///本席位最大報單備拷
	TThostFtdcReturnCodeType	MaxOrderMessageReference;
};

///查詢行情報盤機
struct CThostFtdcQryMDTraderOfferField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
};

///查詢客戶通知
struct CThostFtdcQryNoticeField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
};

///客戶通知
struct CThostFtdcNoticeField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///訊息文字
	TThostFtdcContentType	Content;
	///經紀公司通知內容序號
	TThostFtdcSequenceLabelType	SequenceLabel;
};

///用戶許可權
struct CThostFtdcUserRightField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///客戶許可權類型
	TThostFtdcUserRightTypeType	UserRightType;
	///是否禁止
	TThostFtdcBoolType	IsForbidden;
};

///查詢結算資訊確認域
struct CThostFtdcQrySettlementInfoConfirmField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
};

///裝載結算資訊
struct CThostFtdcLoadSettlementInfoField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
};

///經紀公司可提資金演算法表
struct CThostFtdcBrokerWithdrawAlgorithmField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///可提資金演算法
	TThostFtdcAlgorithmType	WithdrawAlgorithm;
	///資金使用率
	TThostFtdcRatioType	UsingRatio;
	///可提是否包含平倉盈利
	TThostFtdcIncludeCloseProfitType	IncludeCloseProfit;
	///本日無倉且無成交客戶是否受可提比例限制
	TThostFtdcAllWithoutTradeType	AllWithoutTrade;
	///可用是否包含平倉盈利
	TThostFtdcIncludeCloseProfitType	AvailIncludeCloseProfit;
	///是否啟用用戶事件
	TThostFtdcBoolType	IsBrokerUserEvent;
};

///資金帳戶口令變更域
struct CThostFtdcTradingAccountPasswordUpdateV1Field
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///原來的口令
	TThostFtdcPasswordType	OldPassword;
	///新的口令
	TThostFtdcPasswordType	NewPassword;
};

///資金帳戶口令變更域
struct CThostFtdcTradingAccountPasswordUpdateField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///原來的口令
	TThostFtdcPasswordType	OldPassword;
	///新的口令
	TThostFtdcPasswordType	NewPassword;
};

///查詢組合合約分腿
struct CThostFtdcQryCombinationLegField
{
	///組合合約代碼
	TThostFtdcInstrumentIDType	CombInstrumentID;
	///單腿編號
	TThostFtdcLegIDType	LegID;
	///單腿合約代碼
	TThostFtdcInstrumentIDType	LegInstrumentID;
};

///查詢組合合約分腿
struct CThostFtdcQrySyncStatusField
{
	///交易日
	TThostFtdcDateType	TradingDay;
};

///組合交易合約的單腿
struct CThostFtdcCombinationLegField
{
	///組合合約代碼
	TThostFtdcInstrumentIDType	CombInstrumentID;
	///單腿編號
	TThostFtdcLegIDType	LegID;
	///單腿合約代碼
	TThostFtdcInstrumentIDType	LegInstrumentID;
	///買賣方向
	TThostFtdcDirectionType	Direction;
	///單腿乘數
	TThostFtdcLegMultipleType	LegMultiple;
	///派生層數
	TThostFtdcImplyLevelType	ImplyLevel;
};

///資料同步狀態
struct CThostFtdcSyncStatusField
{
	///交易日
	TThostFtdcDateType	TradingDay;
	///資料同步狀態
	TThostFtdcDataSyncStatusType	DataSyncStatus;
};

///查詢連絡人
struct CThostFtdcQryLinkManField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
};

///連絡人
struct CThostFtdcLinkManField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///連絡人類型
	TThostFtdcPersonTypeType	PersonType;
	///證件類型
	TThostFtdcIdCardTypeType	IdentifiedCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///名稱
	TThostFtdcPartyNameType	PersonName;
	///聯繫電話
	TThostFtdcTelephoneType	Telephone;
	///通訊位址
	TThostFtdcAddressType	Address;
	///郵遞區號
	TThostFtdcZipCodeType	ZipCode;
	///優先順序
	TThostFtdcPriorityType	Priority;
};

///查詢經紀公司用戶事件
struct CThostFtdcQryBrokerUserEventField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///用戶事件類型
	TThostFtdcUserEventTypeType	UserEventType;
};

///查詢經紀公司用戶事件
struct CThostFtdcBrokerUserEventField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///用戶事件類型
	TThostFtdcUserEventTypeType	UserEventType;
	///用戶事件序號
	TThostFtdcSequenceNoType	EventSequenceNo;
	///事件發生日期
	TThostFtdcDateType	EventDate;
	///事件發生時間
	TThostFtdcTimeType	EventTime;
	///使用者事件資訊
	TThostFtdcUserEventInfoType	UserEventInfo;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
};

///查詢簽約銀行請求
struct CThostFtdcQryContractBankField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分中心代碼
	TThostFtdcBankBrchIDType	BankBrchID;
};

///查詢簽約銀行回應
struct CThostFtdcContractBankField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分中心代碼
	TThostFtdcBankBrchIDType	BankBrchID;
	///銀行名稱
	TThostFtdcBankNameType	BankName;
};

///投資者組合持倉明細
struct CThostFtdcInvestorPositionCombineDetailField
{
	///交易日
	TThostFtdcDateType	TradingDay;
	///開倉日期
	TThostFtdcDateType	OpenDate;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///結算編號
	TThostFtdcSettlementIDType	SettlementID;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///組合編號
	TThostFtdcTradeIDType	ComTradeID;
	///撮合編號
	TThostFtdcTradeIDType	TradeID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///買賣
	TThostFtdcDirectionType	Direction;
	///持倉量
	TThostFtdcVolumeType	TotalAmt;
	///投資者保證金
	TThostFtdcMoneyType	Margin;
	///交易所保證金
	TThostFtdcMoneyType	ExchMargin;
	///保證金率
	TThostFtdcRatioType	MarginRateByMoney;
	///保證金率(按手數)
	TThostFtdcRatioType	MarginRateByVolume;
	///單腿編號
	TThostFtdcLegIDType	LegID;
	///單腿乘數
	TThostFtdcLegMultipleType	LegMultiple;
	///組合持倉合約編碼
	TThostFtdcInstrumentIDType	CombInstrumentID;
};

///預埋單
struct CThostFtdcParkedOrderField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///報單引用
	TThostFtdcOrderRefType	OrderRef;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///報單價格條件
	TThostFtdcOrderPriceTypeType	OrderPriceType;
	///買賣方向
	TThostFtdcDirectionType	Direction;
	///組合開平標誌
	TThostFtdcCombOffsetFlagType	CombOffsetFlag;
	///組合投機套保標誌
	TThostFtdcCombHedgeFlagType	CombHedgeFlag;
	///價格
	TThostFtdcPriceType	LimitPrice;
	///數量
	TThostFtdcVolumeType	VolumeTotalOriginal;
	///有效期類型
	TThostFtdcTimeConditionType	TimeCondition;
	///GTD日期
	TThostFtdcDateType	GTDDate;
	///成交量類型
	TThostFtdcVolumeConditionType	VolumeCondition;
	///最小成交量
	TThostFtdcVolumeType	MinVolume;
	///觸發條件
	TThostFtdcContingentConditionType	ContingentCondition;
	///止損價
	TThostFtdcPriceType	StopPrice;
	///強平原因
	TThostFtdcForceCloseReasonType	ForceCloseReason;
	///自動掛起標誌
	TThostFtdcBoolType	IsAutoSuspend;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///使用者強評標誌
	TThostFtdcBoolType	UserForceClose;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///預埋報單編號
	TThostFtdcParkedOrderIDType	ParkedOrderID;
	///用戶類型
	TThostFtdcUserTypeType	UserType;
	///預埋單狀態
	TThostFtdcParkedOrderStatusType	Status;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
	///互換單標誌
	TThostFtdcBoolType	IsSwapOrder;
};

///輸入預埋單操作
struct CThostFtdcParkedOrderActionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///報單操作引用
	TThostFtdcOrderActionRefType	OrderActionRef;
	///報單引用
	TThostFtdcOrderRefType	OrderRef;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///報單編號
	TThostFtdcOrderSysIDType	OrderSysID;
	///操作標誌
	TThostFtdcActionFlagType	ActionFlag;
	///價格
	TThostFtdcPriceType	LimitPrice;
	///數量變化
	TThostFtdcVolumeType	VolumeChange;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///預埋撤單單編號
	TThostFtdcParkedOrderActionIDType	ParkedOrderActionID;
	///用戶類型
	TThostFtdcUserTypeType	UserType;
	///預埋撤單狀態
	TThostFtdcParkedOrderStatusType	Status;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///查詢預埋單
struct CThostFtdcQryParkedOrderField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
};

///查詢預埋撤單
struct CThostFtdcQryParkedOrderActionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
};

///刪除預埋單
struct CThostFtdcRemoveParkedOrderField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///預埋報單編號
	TThostFtdcParkedOrderIDType	ParkedOrderID;
};

///刪除預埋撤單
struct CThostFtdcRemoveParkedOrderActionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///預埋撤單編號
	TThostFtdcParkedOrderActionIDType	ParkedOrderActionID;
};

///經紀公司可提資金演算法表
struct CThostFtdcInvestorWithdrawAlgorithmField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者範圍
	TThostFtdcInvestorRangeType	InvestorRange;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///可提資金比例
	TThostFtdcRatioType	UsingRatio;
};

///查詢組合持倉明細
struct CThostFtdcQryInvestorPositionCombineDetailField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///組合持倉合約編碼
	TThostFtdcInstrumentIDType	CombInstrumentID;
};

///成交均價
struct CThostFtdcMarketDataAveragePriceField
{
	///當日均價
	TThostFtdcPriceType	AveragePrice;
};

///校驗投資者密碼
struct CThostFtdcVerifyInvestorPasswordField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///密碼
	TThostFtdcPasswordType	Password;
};

///用戶IP
struct CThostFtdcUserIPField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///IP地址
	TThostFtdcIPAddressType	IPAddress;
	///IP位址遮罩
	TThostFtdcIPAddressType	IPMask;
	///Mac地址
	TThostFtdcMacAddressType	MacAddress;
};

///使用者事件通知資訊
struct CThostFtdcTradingNoticeInfoField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///發送時間
	TThostFtdcTimeType	SendTime;
	///訊息文字
	TThostFtdcContentType	FieldContent;
	///序列系列號
	TThostFtdcSequenceSeriesType	SequenceSeries;
	///序號
	TThostFtdcSequenceNoType	SequenceNo;
};

///用戶事件通知
struct CThostFtdcTradingNoticeField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者範圍
	TThostFtdcInvestorRangeType	InvestorRange;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///序列系列號
	TThostFtdcSequenceSeriesType	SequenceSeries;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///發送時間
	TThostFtdcTimeType	SendTime;
	///序號
	TThostFtdcSequenceNoType	SequenceNo;
	///訊息文字
	TThostFtdcContentType	FieldContent;
};

///查詢交易事件通知
struct CThostFtdcQryTradingNoticeField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
};

///查詢錯誤報單
struct CThostFtdcQryErrOrderField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
};

///錯誤報單
struct CThostFtdcErrOrderField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///報單引用
	TThostFtdcOrderRefType	OrderRef;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///報單價格條件
	TThostFtdcOrderPriceTypeType	OrderPriceType;
	///買賣方向
	TThostFtdcDirectionType	Direction;
	///組合開平標誌
	TThostFtdcCombOffsetFlagType	CombOffsetFlag;
	///組合投機套保標誌
	TThostFtdcCombHedgeFlagType	CombHedgeFlag;
	///價格
	TThostFtdcPriceType	LimitPrice;
	///數量
	TThostFtdcVolumeType	VolumeTotalOriginal;
	///有效期類型
	TThostFtdcTimeConditionType	TimeCondition;
	///GTD日期
	TThostFtdcDateType	GTDDate;
	///成交量類型
	TThostFtdcVolumeConditionType	VolumeCondition;
	///最小成交量
	TThostFtdcVolumeType	MinVolume;
	///觸發條件
	TThostFtdcContingentConditionType	ContingentCondition;
	///止損價
	TThostFtdcPriceType	StopPrice;
	///強平原因
	TThostFtdcForceCloseReasonType	ForceCloseReason;
	///自動掛起標誌
	TThostFtdcBoolType	IsAutoSuspend;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///使用者強評標誌
	TThostFtdcBoolType	UserForceClose;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
	///互換單標誌
	TThostFtdcBoolType	IsSwapOrder;
};

///查詢錯誤報單操作
struct CThostFtdcErrorConditionalOrderField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///報單引用
	TThostFtdcOrderRefType	OrderRef;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///報單價格條件
	TThostFtdcOrderPriceTypeType	OrderPriceType;
	///買賣方向
	TThostFtdcDirectionType	Direction;
	///組合開平標誌
	TThostFtdcCombOffsetFlagType	CombOffsetFlag;
	///組合投機套保標誌
	TThostFtdcCombHedgeFlagType	CombHedgeFlag;
	///價格
	TThostFtdcPriceType	LimitPrice;
	///數量
	TThostFtdcVolumeType	VolumeTotalOriginal;
	///有效期類型
	TThostFtdcTimeConditionType	TimeCondition;
	///GTD日期
	TThostFtdcDateType	GTDDate;
	///成交量類型
	TThostFtdcVolumeConditionType	VolumeCondition;
	///最小成交量
	TThostFtdcVolumeType	MinVolume;
	///觸發條件
	TThostFtdcContingentConditionType	ContingentCondition;
	///止損價
	TThostFtdcPriceType	StopPrice;
	///強平原因
	TThostFtdcForceCloseReasonType	ForceCloseReason;
	///自動掛起標誌
	TThostFtdcBoolType	IsAutoSuspend;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///本地報單編號
	TThostFtdcOrderLocalIDType	OrderLocalID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///合約在交易所的代碼
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///報單提交狀態
	TThostFtdcOrderSubmitStatusType	OrderSubmitStatus;
	///報單提示序號
	TThostFtdcSequenceNoType	NotifySequence;
	///交易日
	TThostFtdcDateType	TradingDay;
	///結算編號
	TThostFtdcSettlementIDType	SettlementID;
	///報單編號
	TThostFtdcOrderSysIDType	OrderSysID;
	///報單來源
	TThostFtdcOrderSourceType	OrderSource;
	///報單狀態
	TThostFtdcOrderStatusType	OrderStatus;
	///報單類型
	TThostFtdcOrderTypeType	OrderType;
	///今成交數量
	TThostFtdcVolumeType	VolumeTraded;
	///剩餘數量
	TThostFtdcVolumeType	VolumeTotal;
	///報單日期
	TThostFtdcDateType	InsertDate;
	///委託時間
	TThostFtdcTimeType	InsertTime;
	///啟動時間
	TThostFtdcTimeType	ActiveTime;
	///掛起時間
	TThostFtdcTimeType	SuspendTime;
	///最後修改時間
	TThostFtdcTimeType	UpdateTime;
	///撤銷時間
	TThostFtdcTimeType	CancelTime;
	///最後修改交易所交易員代碼
	TThostFtdcTraderIDType	ActiveTraderID;
	///結算會員編號
	TThostFtdcParticipantIDType	ClearingPartID;
	///序號
	TThostFtdcSequenceNoType	SequenceNo;
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///使用者端產品資訊
	TThostFtdcProductInfoType	UserProductInfo;
	///狀態資訊
	TThostFtdcErrorMsgType	StatusMsg;
	///使用者強評標誌
	TThostFtdcBoolType	UserForceClose;
	///操作用戶代碼
	TThostFtdcUserIDType	ActiveUserID;
	///經紀公司報單編號
	TThostFtdcSequenceNoType	BrokerOrderSeq;
	///相關報單
	TThostFtdcOrderSysIDType	RelativeOrderSysID;
	///鄭商所成交數量
	TThostFtdcVolumeType	ZCETotalTradedVolume;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
	///互換單標誌
	TThostFtdcBoolType	IsSwapOrder;
};

///查詢錯誤報單操作
struct CThostFtdcQryErrOrderActionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
};

///錯誤報單操作
struct CThostFtdcErrOrderActionField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///報單操作引用
	TThostFtdcOrderActionRefType	OrderActionRef;
	///報單引用
	TThostFtdcOrderRefType	OrderRef;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///前置編號
	TThostFtdcFrontIDType	FrontID;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///報單編號
	TThostFtdcOrderSysIDType	OrderSysID;
	///操作標誌
	TThostFtdcActionFlagType	ActionFlag;
	///價格
	TThostFtdcPriceType	LimitPrice;
	///數量變化
	TThostFtdcVolumeType	VolumeChange;
	///操作日期
	TThostFtdcDateType	ActionDate;
	///操作時間
	TThostFtdcTimeType	ActionTime;
	///交易所交易員代碼
	TThostFtdcTraderIDType	TraderID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///本地報單編號
	TThostFtdcOrderLocalIDType	OrderLocalID;
	///操作本地編號
	TThostFtdcOrderLocalIDType	ActionLocalID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///客戶代碼
	TThostFtdcClientIDType	ClientID;
	///業務單元
	TThostFtdcBusinessUnitType	BusinessUnit;
	///報單操作狀態
	TThostFtdcOrderActionStatusType	OrderActionStatus;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///狀態資訊
	TThostFtdcErrorMsgType	StatusMsg;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///查詢交易所狀態
struct CThostFtdcQryExchangeSequenceField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
};

///交易所狀態
struct CThostFtdcExchangeSequenceField
{
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///序號
	TThostFtdcSequenceNoType	SequenceNo;
	///合約交易狀態
	TThostFtdcInstrumentStatusType	MarketStatus;
};

///根據價格查詢最大報單數量
struct CThostFtdcQueryMaxOrderVolumeWithPriceField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///買賣方向
	TThostFtdcDirectionType	Direction;
	///開平標誌
	TThostFtdcOffsetFlagType	OffsetFlag;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///最大允許報單數量
	TThostFtdcVolumeType	MaxVolume;
	///報單價格
	TThostFtdcPriceType	Price;
};

///查詢經紀公司交易參數
struct CThostFtdcQryBrokerTradingParamsField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
};

///經紀公司交易參數
struct CThostFtdcBrokerTradingParamsField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///保證金價格類型
	TThostFtdcMarginPriceTypeType	MarginPriceType;
	///盈虧演算法
	TThostFtdcAlgorithmType	Algorithm;
	///可用是否包含平倉盈利
	TThostFtdcIncludeCloseProfitType	AvailIncludeCloseProfit;
};

///查詢經紀公司交易演算法
struct CThostFtdcQryBrokerTradingAlgosField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
};

///經紀公司交易演算法
struct CThostFtdcBrokerTradingAlgosField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///持倉處理演算法編號
	TThostFtdcHandlePositionAlgoIDType	HandlePositionAlgoID;
	///尋找保證金率演算法編號
	TThostFtdcFindMarginRateAlgoIDType	FindMarginRateAlgoID;
	///資金處理演算法編號
	TThostFtdcHandleTradingAccountAlgoIDType	HandleTradingAccountAlgoID;
};

///查詢經紀公司資金
struct CThostFtdcQueryBrokerDepositField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
};

///經紀公司資金
struct CThostFtdcBrokerDepositField
{
	///交易日期
	TThostFtdcTradeDateType	TradingDay;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///會員代碼
	TThostFtdcParticipantIDType	ParticipantID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///上次結算準備金
	TThostFtdcMoneyType	PreBalance;
	///當前保證金總額
	TThostFtdcMoneyType	CurrMargin;
	///平倉盈虧
	TThostFtdcMoneyType	CloseProfit;
	///期貨結算準備金
	TThostFtdcMoneyType	Balance;
	///入金金額
	TThostFtdcMoneyType	Deposit;
	///出金金額
	TThostFtdcMoneyType	Withdraw;
	///可提資金
	TThostFtdcMoneyType	Available;
	///基本準備金
	TThostFtdcMoneyType	Reserve;
	///凍結的保證金
	TThostFtdcMoneyType	FrozenMargin;
};

///查詢保證金監管系統經紀公司金鑰
struct CThostFtdcQryCFMMCBrokerKeyField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
};

///保證金監管系統經紀公司金鑰
struct CThostFtdcCFMMCBrokerKeyField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///經紀公司統一編碼
	TThostFtdcParticipantIDType	ParticipantID;
	///金鑰生成日期
	TThostFtdcDateType	CreateDate;
	///金鑰生成時間
	TThostFtdcTimeType	CreateTime;
	///金鑰編號
	TThostFtdcSequenceNoType	KeyID;
	///動態金鑰
	TThostFtdcCFMMCKeyType	CurrentKey;
	///動態金鑰類型
	TThostFtdcCFMMCKeyKindType	KeyKind;
};

///保證金監管系統經紀公司資金帳戶金鑰
struct CThostFtdcCFMMCTradingAccountKeyField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///經紀公司統一編碼
	TThostFtdcParticipantIDType	ParticipantID;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///金鑰編號
	TThostFtdcSequenceNoType	KeyID;
	///動態金鑰
	TThostFtdcCFMMCKeyType	CurrentKey;
};

///請求查詢保證金監管系統經紀公司資金帳戶金鑰
struct CThostFtdcQryCFMMCTradingAccountKeyField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
};

///使用者動態權杖參數
struct CThostFtdcBrokerUserOTPParamField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///動態權杖提供商
	TThostFtdcOTPVendorsIDType	OTPVendorsID;
	///動態權杖序號
	TThostFtdcSerialNumberType	SerialNumber;
	///權杖金鑰
	TThostFtdcAuthKeyType	AuthKey;
	///漂移值
	TThostFtdcLastDriftType	LastDrift;
	///成功值
	TThostFtdcLastSuccessType	LastSuccess;
	///動態權杖類型
	TThostFtdcOTPTypeType	OTPType;
};

///手工同步使用者動態權杖
struct CThostFtdcManualSyncBrokerUserOTPField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///動態權杖類型
	TThostFtdcOTPTypeType	OTPType;
	///第一個動態密碼
	TThostFtdcPasswordType	FirstOTP;
	///第二個動態密碼
	TThostFtdcPasswordType	SecondOTP;
};

///投資者手續費率範本
struct CThostFtdcCommRateModelField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///手續費率範本代碼
	TThostFtdcInvestorIDType	CommModelID;
	///範本名稱
	TThostFtdcCommModelNameType	CommModelName;
};

///請求查詢投資者手續費率範本
struct CThostFtdcQryCommRateModelField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///手續費率範本代碼
	TThostFtdcInvestorIDType	CommModelID;
};

///投資者保證金率範本
struct CThostFtdcMarginModelField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///保證金率範本代碼
	TThostFtdcInvestorIDType	MarginModelID;
	///範本名稱
	TThostFtdcCommModelNameType	MarginModelName;
};

///請求查詢投資者保證金率範本
struct CThostFtdcQryMarginModelField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///保證金率範本代碼
	TThostFtdcInvestorIDType	MarginModelID;
};

///倉單折抵信息
struct CThostFtdcEWarrantOffsetField
{
	///交易日期
	TThostFtdcTradeDateType	TradingDay;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
	///買賣方向
	TThostFtdcDirectionType	Direction;
	///投機套保標誌
	TThostFtdcHedgeFlagType	HedgeFlag;
	///數量
	TThostFtdcVolumeType	Volume;
};

///查詢倉單折抵資訊
struct CThostFtdcQryEWarrantOffsetField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///交易所代碼
	TThostFtdcExchangeIDType	ExchangeID;
	///合約代碼
	TThostFtdcInstrumentIDType	InstrumentID;
};

///轉帳開戶請求
struct CThostFtdcReqOpenAccountField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///性別
	TThostFtdcGenderType	Gender;
	///國家代碼
	TThostFtdcCountryCodeType	CountryCode;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///地址
	TThostFtdcAddressType	Address;
	///郵編
	TThostFtdcZipCodeType	ZipCode;
	///電話號碼
	TThostFtdcTelephoneType	Telephone;
	///手機
	TThostFtdcMobilePhoneType	MobilePhone;
	///傳真
	TThostFtdcFaxType	Fax;
	///電子郵件
	TThostFtdcEMailType	EMail;
	///資金帳戶狀態
	TThostFtdcMoneyAccountStatusType	MoneyAccountStatus;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///驗證客戶證件號碼標誌
	TThostFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///匯鈔標誌
	TThostFtdcCashExchangeCodeType	CashExchangeCode;
	///摘要
	TThostFtdcDigestType	Digest;
	///銀行帳號類型
	TThostFtdcBankAccTypeType	BankAccType;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨單位帳號類型
	TThostFtdcBankAccTypeType	BankSecuAccType;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///期貨單位帳號
	TThostFtdcBankAccountType	BankSecuAcc;
	///銀行密碼標誌
	TThostFtdcPwdFlagType	BankPwdFlag;
	///期貨資金密碼核對標誌
	TThostFtdcPwdFlagType	SecuPwdFlag;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///交易ID
	TThostFtdcTIDType	TID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
};

///轉帳銷戶請求
struct CThostFtdcReqCancelAccountField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///性別
	TThostFtdcGenderType	Gender;
	///國家代碼
	TThostFtdcCountryCodeType	CountryCode;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///地址
	TThostFtdcAddressType	Address;
	///郵編
	TThostFtdcZipCodeType	ZipCode;
	///電話號碼
	TThostFtdcTelephoneType	Telephone;
	///手機
	TThostFtdcMobilePhoneType	MobilePhone;
	///傳真
	TThostFtdcFaxType	Fax;
	///電子郵件
	TThostFtdcEMailType	EMail;
	///資金帳戶狀態
	TThostFtdcMoneyAccountStatusType	MoneyAccountStatus;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///驗證客戶證件號碼標誌
	TThostFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///匯鈔標誌
	TThostFtdcCashExchangeCodeType	CashExchangeCode;
	///摘要
	TThostFtdcDigestType	Digest;
	///銀行帳號類型
	TThostFtdcBankAccTypeType	BankAccType;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨單位帳號類型
	TThostFtdcBankAccTypeType	BankSecuAccType;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///期貨單位帳號
	TThostFtdcBankAccountType	BankSecuAcc;
	///銀行密碼標誌
	TThostFtdcPwdFlagType	BankPwdFlag;
	///期貨資金密碼核對標誌
	TThostFtdcPwdFlagType	SecuPwdFlag;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///交易ID
	TThostFtdcTIDType	TID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
};

///變更銀行帳戶請求
struct CThostFtdcReqChangeAccountField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///性別
	TThostFtdcGenderType	Gender;
	///國家代碼
	TThostFtdcCountryCodeType	CountryCode;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///地址
	TThostFtdcAddressType	Address;
	///郵編
	TThostFtdcZipCodeType	ZipCode;
	///電話號碼
	TThostFtdcTelephoneType	Telephone;
	///手機
	TThostFtdcMobilePhoneType	MobilePhone;
	///傳真
	TThostFtdcFaxType	Fax;
	///電子郵件
	TThostFtdcEMailType	EMail;
	///資金帳戶狀態
	TThostFtdcMoneyAccountStatusType	MoneyAccountStatus;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///新銀行帳號
	TThostFtdcBankAccountType	NewBankAccount;
	///新銀行密碼
	TThostFtdcPasswordType	NewBankPassWord;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///銀行帳號類型
	TThostFtdcBankAccTypeType	BankAccType;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///驗證客戶證件號碼標誌
	TThostFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///銀行密碼標誌
	TThostFtdcPwdFlagType	BankPwdFlag;
	///期貨資金密碼核對標誌
	TThostFtdcPwdFlagType	SecuPwdFlag;
	///交易ID
	TThostFtdcTIDType	TID;
	///摘要
	TThostFtdcDigestType	Digest;
};

///轉帳請求
struct CThostFtdcReqTransferField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///期貨公司流水號
	TThostFtdcFutureSerialType	FutureSerial;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///驗證客戶證件號碼標誌
	TThostFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///轉帳金額
	TThostFtdcTradeAmountType	TradeAmount;
	///期貨可取金額
	TThostFtdcTradeAmountType	FutureFetchAmount;
	///費用支付標誌
	TThostFtdcFeePayFlagType	FeePayFlag;
	///應收客戶費用
	TThostFtdcCustFeeType	CustFee;
	///應收期貨公司費用
	TThostFtdcFutureFeeType	BrokerFee;
	///發送方給接收方的消息
	TThostFtdcAddInfoType	Message;
	///摘要
	TThostFtdcDigestType	Digest;
	///銀行帳號類型
	TThostFtdcBankAccTypeType	BankAccType;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨單位帳號類型
	TThostFtdcBankAccTypeType	BankSecuAccType;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///期貨單位帳號
	TThostFtdcBankAccountType	BankSecuAcc;
	///銀行密碼標誌
	TThostFtdcPwdFlagType	BankPwdFlag;
	///期貨資金密碼核對標誌
	TThostFtdcPwdFlagType	SecuPwdFlag;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
	///轉帳交易狀態
	TThostFtdcTransferStatusType	TransferStatus;
};

///銀行發起銀行資金轉期貨回應
struct CThostFtdcRspTransferField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///期貨公司流水號
	TThostFtdcFutureSerialType	FutureSerial;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///驗證客戶證件號碼標誌
	TThostFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///轉帳金額
	TThostFtdcTradeAmountType	TradeAmount;
	///期貨可取金額
	TThostFtdcTradeAmountType	FutureFetchAmount;
	///費用支付標誌
	TThostFtdcFeePayFlagType	FeePayFlag;
	///應收客戶費用
	TThostFtdcCustFeeType	CustFee;
	///應收期貨公司費用
	TThostFtdcFutureFeeType	BrokerFee;
	///發送方給接收方的消息
	TThostFtdcAddInfoType	Message;
	///摘要
	TThostFtdcDigestType	Digest;
	///銀行帳號類型
	TThostFtdcBankAccTypeType	BankAccType;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨單位帳號類型
	TThostFtdcBankAccTypeType	BankSecuAccType;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///期貨單位帳號
	TThostFtdcBankAccountType	BankSecuAcc;
	///銀行密碼標誌
	TThostFtdcPwdFlagType	BankPwdFlag;
	///期貨資金密碼核對標誌
	TThostFtdcPwdFlagType	SecuPwdFlag;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
	///轉帳交易狀態
	TThostFtdcTransferStatusType	TransferStatus;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///沖正請求
struct CThostFtdcReqRepealField
{
	///沖正時間間隔
	TThostFtdcRepealTimeIntervalType	RepealTimeInterval;
	///已經沖正次數
	TThostFtdcRepealedTimesType	RepealedTimes;
	///銀行沖正標誌
	TThostFtdcBankRepealFlagType	BankRepealFlag;
	///期商沖正標誌
	TThostFtdcBrokerRepealFlagType	BrokerRepealFlag;
	///被沖正平臺流水號
	TThostFtdcPlateSerialType	PlateRepealSerial;
	///被沖正銀行流水號
	TThostFtdcBankSerialType	BankRepealSerial;
	///被沖正期貨流水號
	TThostFtdcFutureSerialType	FutureRepealSerial;
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///期貨公司流水號
	TThostFtdcFutureSerialType	FutureSerial;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///驗證客戶證件號碼標誌
	TThostFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///轉帳金額
	TThostFtdcTradeAmountType	TradeAmount;
	///期貨可取金額
	TThostFtdcTradeAmountType	FutureFetchAmount;
	///費用支付標誌
	TThostFtdcFeePayFlagType	FeePayFlag;
	///應收客戶費用
	TThostFtdcCustFeeType	CustFee;
	///應收期貨公司費用
	TThostFtdcFutureFeeType	BrokerFee;
	///發送方給接收方的消息
	TThostFtdcAddInfoType	Message;
	///摘要
	TThostFtdcDigestType	Digest;
	///銀行帳號類型
	TThostFtdcBankAccTypeType	BankAccType;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨單位帳號類型
	TThostFtdcBankAccTypeType	BankSecuAccType;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///期貨單位帳號
	TThostFtdcBankAccountType	BankSecuAcc;
	///銀行密碼標誌
	TThostFtdcPwdFlagType	BankPwdFlag;
	///期貨資金密碼核對標誌
	TThostFtdcPwdFlagType	SecuPwdFlag;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
	///轉帳交易狀態
	TThostFtdcTransferStatusType	TransferStatus;
};

///沖正回應
struct CThostFtdcRspRepealField
{
	///沖正時間間隔
	TThostFtdcRepealTimeIntervalType	RepealTimeInterval;
	///已經沖正次數
	TThostFtdcRepealedTimesType	RepealedTimes;
	///銀行沖正標誌
	TThostFtdcBankRepealFlagType	BankRepealFlag;
	///期商沖正標誌
	TThostFtdcBrokerRepealFlagType	BrokerRepealFlag;
	///被沖正平臺流水號
	TThostFtdcPlateSerialType	PlateRepealSerial;
	///被沖正銀行流水號
	TThostFtdcBankSerialType	BankRepealSerial;
	///被沖正期貨流水號
	TThostFtdcFutureSerialType	FutureRepealSerial;
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///期貨公司流水號
	TThostFtdcFutureSerialType	FutureSerial;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///驗證客戶證件號碼標誌
	TThostFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///轉帳金額
	TThostFtdcTradeAmountType	TradeAmount;
	///期貨可取金額
	TThostFtdcTradeAmountType	FutureFetchAmount;
	///費用支付標誌
	TThostFtdcFeePayFlagType	FeePayFlag;
	///應收客戶費用
	TThostFtdcCustFeeType	CustFee;
	///應收期貨公司費用
	TThostFtdcFutureFeeType	BrokerFee;
	///發送方給接收方的消息
	TThostFtdcAddInfoType	Message;
	///摘要
	TThostFtdcDigestType	Digest;
	///銀行帳號類型
	TThostFtdcBankAccTypeType	BankAccType;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨單位帳號類型
	TThostFtdcBankAccTypeType	BankSecuAccType;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///期貨單位帳號
	TThostFtdcBankAccountType	BankSecuAcc;
	///銀行密碼標誌
	TThostFtdcPwdFlagType	BankPwdFlag;
	///期貨資金密碼核對標誌
	TThostFtdcPwdFlagType	SecuPwdFlag;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
	///轉帳交易狀態
	TThostFtdcTransferStatusType	TransferStatus;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///查詢帳戶資訊請求
struct CThostFtdcReqQueryAccountField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///期貨公司流水號
	TThostFtdcFutureSerialType	FutureSerial;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///驗證客戶證件號碼標誌
	TThostFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///摘要
	TThostFtdcDigestType	Digest;
	///銀行帳號類型
	TThostFtdcBankAccTypeType	BankAccType;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨單位帳號類型
	TThostFtdcBankAccTypeType	BankSecuAccType;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///期貨單位帳號
	TThostFtdcBankAccountType	BankSecuAcc;
	///銀行密碼標誌
	TThostFtdcPwdFlagType	BankPwdFlag;
	///期貨資金密碼核對標誌
	TThostFtdcPwdFlagType	SecuPwdFlag;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
};

///查詢帳戶資訊回應
struct CThostFtdcRspQueryAccountField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///期貨公司流水號
	TThostFtdcFutureSerialType	FutureSerial;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///驗證客戶證件號碼標誌
	TThostFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///摘要
	TThostFtdcDigestType	Digest;
	///銀行帳號類型
	TThostFtdcBankAccTypeType	BankAccType;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨單位帳號類型
	TThostFtdcBankAccTypeType	BankSecuAccType;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///期貨單位帳號
	TThostFtdcBankAccountType	BankSecuAcc;
	///銀行密碼標誌
	TThostFtdcPwdFlagType	BankPwdFlag;
	///期貨資金密碼核對標誌
	TThostFtdcPwdFlagType	SecuPwdFlag;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
	///銀行可用金額
	TThostFtdcTradeAmountType	BankUseAmount;
	///銀行可取金額
	TThostFtdcTradeAmountType	BankFetchAmount;
};

///期商簽到簽退
struct CThostFtdcFutureSignIOField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///摘要
	TThostFtdcDigestType	Digest;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
};

///期商簽到回應
struct CThostFtdcRspFutureSignInField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///摘要
	TThostFtdcDigestType	Digest;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
	///PIN金鑰
	TThostFtdcPasswordKeyType	PinKey;
	///MAC金鑰
	TThostFtdcPasswordKeyType	MacKey;
};

///期商簽退請求
struct CThostFtdcReqFutureSignOutField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///摘要
	TThostFtdcDigestType	Digest;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
};

///期商簽退回應
struct CThostFtdcRspFutureSignOutField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///摘要
	TThostFtdcDigestType	Digest;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///查詢指定流水號的交易結果請求
struct CThostFtdcReqQueryTradeResultBySerialField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///流水號
	TThostFtdcSerialType	Reference;
	///本流水號發佈者的機構類型
	TThostFtdcInstitutionTypeType	RefrenceIssureType;
	///本流水號發佈者機構編碼
	TThostFtdcOrganCodeType	RefrenceIssure;
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///轉帳金額
	TThostFtdcTradeAmountType	TradeAmount;
	///摘要
	TThostFtdcDigestType	Digest;
};

///查詢指定流水號的交易結果回應
struct CThostFtdcRspQueryTradeResultBySerialField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
	///流水號
	TThostFtdcSerialType	Reference;
	///本流水號發佈者的機構類型
	TThostFtdcInstitutionTypeType	RefrenceIssureType;
	///本流水號發佈者機構編碼
	TThostFtdcOrganCodeType	RefrenceIssure;
	///原始返回代碼
	TThostFtdcReturnCodeType	OriginReturnCode;
	///原始返回碼描述
	TThostFtdcDescrInfoForReturnCodeType	OriginDescrInfoForReturnCode;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///轉帳金額
	TThostFtdcTradeAmountType	TradeAmount;
	///摘要
	TThostFtdcDigestType	Digest;
};

///日終檔就緒請求
struct CThostFtdcReqDayEndFileReadyField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///檔業務功能
	TThostFtdcFileBusinessCodeType	FileBusinessCode;
	///摘要
	TThostFtdcDigestType	Digest;
};

///返回結果
struct CThostFtdcReturnResultField
{
	///返回代碼
	TThostFtdcReturnCodeType	ReturnCode;
	///返回碼描述
	TThostFtdcDescrInfoForReturnCodeType	DescrInfoForReturnCode;
};

///驗證期貨資金密碼
struct CThostFtdcVerifyFuturePasswordField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///交易ID
	TThostFtdcTIDType	TID;
};

///驗證客戶資訊
struct CThostFtdcVerifyCustInfoField
{
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
};

///驗證期貨資金密碼和客戶資訊
struct CThostFtdcVerifyFuturePasswordAndCustInfoField
{
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
};

///驗證期貨資金密碼和客戶資訊
struct CThostFtdcDepositResultInformField
{
	///出入金流水號，該流水號為銀期報盤返回的流水號
	TThostFtdcDepositSeqNoType	DepositSeqNo;
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///入金金額
	TThostFtdcMoneyType	Deposit;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///返回代碼
	TThostFtdcReturnCodeType	ReturnCode;
	///返回碼描述
	TThostFtdcDescrInfoForReturnCodeType	DescrInfoForReturnCode;
};

///交易核心向銀期報盤發出金鑰同步請求
struct CThostFtdcReqSyncKeyField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///交易核心給銀期報盤的消息
	TThostFtdcAddInfoType	Message;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
};

///交易核心向銀期報盤發出金鑰同步響應
struct CThostFtdcRspSyncKeyField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///交易核心給銀期報盤的消息
	TThostFtdcAddInfoType	Message;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///查詢帳戶資訊通知
struct CThostFtdcNotifyQueryAccountField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///期貨公司流水號
	TThostFtdcFutureSerialType	FutureSerial;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///驗證客戶證件號碼標誌
	TThostFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///摘要
	TThostFtdcDigestType	Digest;
	///銀行帳號類型
	TThostFtdcBankAccTypeType	BankAccType;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨單位帳號類型
	TThostFtdcBankAccTypeType	BankSecuAccType;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///期貨單位帳號
	TThostFtdcBankAccountType	BankSecuAcc;
	///銀行密碼標誌
	TThostFtdcPwdFlagType	BankPwdFlag;
	///期貨資金密碼核對標誌
	TThostFtdcPwdFlagType	SecuPwdFlag;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
	///銀行可用金額
	TThostFtdcTradeAmountType	BankUseAmount;
	///銀行可取金額
	TThostFtdcTradeAmountType	BankFetchAmount;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///銀期轉帳交易流水錶
struct CThostFtdcTransferSerialField
{
	///平臺流水號
	TThostFtdcPlateSerialType	PlateSerial;
	///交易發起方日期
	TThostFtdcTradeDateType	TradeDate;
	///交易日期
	TThostFtdcDateType	TradingDay;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///交易代碼
	TThostFtdcTradeCodeType	TradeCode;
	///會話編號
	TThostFtdcSessionIDType	SessionID;
	///銀行編碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆編碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///銀行帳號類型
	TThostFtdcBankAccTypeType	BankAccType;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///期貨公司編碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///期貨公司帳號類型
	TThostFtdcFutureAccTypeType	FutureAccType;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///投資者代碼
	TThostFtdcInvestorIDType	InvestorID;
	///期貨公司流水號
	TThostFtdcFutureSerialType	FutureSerial;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///交易金額
	TThostFtdcTradeAmountType	TradeAmount;
	///應收客戶費用
	TThostFtdcCustFeeType	CustFee;
	///應收期貨公司費用
	TThostFtdcFutureFeeType	BrokerFee;
	///有效標誌
	TThostFtdcAvailabilityFlagType	AvailabilityFlag;
	///操作員
	TThostFtdcOperatorCodeType	OperatorCode;
	///新銀行帳號
	TThostFtdcBankAccountType	BankNewAccount;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///請求查詢轉帳流水
struct CThostFtdcQryTransferSerialField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///銀行編碼
	TThostFtdcBankIDType	BankID;
};

///期商簽到通知
struct CThostFtdcNotifyFutureSignInField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///摘要
	TThostFtdcDigestType	Digest;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
	///PIN金鑰
	TThostFtdcPasswordKeyType	PinKey;
	///MAC金鑰
	TThostFtdcPasswordKeyType	MacKey;
};

///期商簽退通知
struct CThostFtdcNotifyFutureSignOutField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///摘要
	TThostFtdcDigestType	Digest;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///交易核心向銀期報盤發出金鑰同步處理結果的通知
struct CThostFtdcNotifySyncKeyField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///交易核心給銀期報盤的消息
	TThostFtdcAddInfoType	Message;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///請求編號
	TThostFtdcRequestIDType	RequestID;
	///交易ID
	TThostFtdcTIDType	TID;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///請求查詢銀期簽約關係
struct CThostFtdcQryAccountregisterField
{
	///經紀公司代碼
	TThostFtdcBrokerIDType	BrokerID;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///銀行編碼
	TThostFtdcBankIDType	BankID;
};

///客戶開銷戶資訊表
struct CThostFtdcAccountregisterField
{
	///交易日期
	TThostFtdcTradeDateType	TradeDay;
	///銀行編碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆編碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///期貨公司編碼
	TThostFtdcBrokerIDType	BrokerID;
	///期貨公司分支機搆編碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///開銷戶類別
	TThostFtdcOpenOrDestroyType	OpenOrDestroy;
	///簽約日期
	TThostFtdcTradeDateType	RegDate;
	///解約日期
	TThostFtdcTradeDateType	OutDate;
	///交易ID
	TThostFtdcTIDType	TID;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///銀行帳號類型
	TThostFtdcBankAccTypeType	BankAccType;
};

///銀期開戶資訊
struct CThostFtdcOpenAccountField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///性別
	TThostFtdcGenderType	Gender;
	///國家代碼
	TThostFtdcCountryCodeType	CountryCode;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///地址
	TThostFtdcAddressType	Address;
	///郵編
	TThostFtdcZipCodeType	ZipCode;
	///電話號碼
	TThostFtdcTelephoneType	Telephone;
	///手機
	TThostFtdcMobilePhoneType	MobilePhone;
	///傳真
	TThostFtdcFaxType	Fax;
	///電子郵件
	TThostFtdcEMailType	EMail;
	///資金帳戶狀態
	TThostFtdcMoneyAccountStatusType	MoneyAccountStatus;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///驗證客戶證件號碼標誌
	TThostFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///匯鈔標誌
	TThostFtdcCashExchangeCodeType	CashExchangeCode;
	///摘要
	TThostFtdcDigestType	Digest;
	///銀行帳號類型
	TThostFtdcBankAccTypeType	BankAccType;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨單位帳號類型
	TThostFtdcBankAccTypeType	BankSecuAccType;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///期貨單位帳號
	TThostFtdcBankAccountType	BankSecuAcc;
	///銀行密碼標誌
	TThostFtdcPwdFlagType	BankPwdFlag;
	///期貨資金密碼核對標誌
	TThostFtdcPwdFlagType	SecuPwdFlag;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///交易ID
	TThostFtdcTIDType	TID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///銀期銷戶信息
struct CThostFtdcCancelAccountField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///性別
	TThostFtdcGenderType	Gender;
	///國家代碼
	TThostFtdcCountryCodeType	CountryCode;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///地址
	TThostFtdcAddressType	Address;
	///郵編
	TThostFtdcZipCodeType	ZipCode;
	///電話號碼
	TThostFtdcTelephoneType	Telephone;
	///手機
	TThostFtdcMobilePhoneType	MobilePhone;
	///傳真
	TThostFtdcFaxType	Fax;
	///電子郵件
	TThostFtdcEMailType	EMail;
	///資金帳戶狀態
	TThostFtdcMoneyAccountStatusType	MoneyAccountStatus;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///驗證客戶證件號碼標誌
	TThostFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///匯鈔標誌
	TThostFtdcCashExchangeCodeType	CashExchangeCode;
	///摘要
	TThostFtdcDigestType	Digest;
	///銀行帳號類型
	TThostFtdcBankAccTypeType	BankAccType;
	///管道標誌
	TThostFtdcDeviceIDType	DeviceID;
	///期貨單位帳號類型
	TThostFtdcBankAccTypeType	BankSecuAccType;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///期貨單位帳號
	TThostFtdcBankAccountType	BankSecuAcc;
	///銀行密碼標誌
	TThostFtdcPwdFlagType	BankPwdFlag;
	///期貨資金密碼核對標誌
	TThostFtdcPwdFlagType	SecuPwdFlag;
	///交易櫃員
	TThostFtdcOperNoType	OperNo;
	///交易ID
	TThostFtdcTIDType	TID;
	///使用者標識
	TThostFtdcUserIDType	UserID;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///銀期變更銀行帳號信息
struct CThostFtdcChangeAccountField
{
	///業務功能碼
	TThostFtdcTradeCodeType	TradeCode;
	///銀行代碼
	TThostFtdcBankIDType	BankID;
	///銀行分支機搆代碼
	TThostFtdcBankBrchIDType	BankBranchID;
	///期商代碼
	TThostFtdcBrokerIDType	BrokerID;
	///期商分支機搆代碼
	TThostFtdcFutureBranchIDType	BrokerBranchID;
	///交易日期
	TThostFtdcTradeDateType	TradeDate;
	///交易時間
	TThostFtdcTradeTimeType	TradeTime;
	///銀行流水號
	TThostFtdcBankSerialType	BankSerial;
	///交易系統日期
	TThostFtdcTradeDateType	TradingDay;
	///銀期平臺消息流水號
	TThostFtdcSerialType	PlateSerial;
	///最後分片標誌
	TThostFtdcLastFragmentType	LastFragment;
	///會話號
	TThostFtdcSessionIDType	SessionID;
	///客戶姓名
	TThostFtdcIndividualNameType	CustomerName;
	///證件類型
	TThostFtdcIdCardTypeType	IdCardType;
	///證件號碼
	TThostFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///性別
	TThostFtdcGenderType	Gender;
	///國家代碼
	TThostFtdcCountryCodeType	CountryCode;
	///客戶類型
	TThostFtdcCustTypeType	CustType;
	///地址
	TThostFtdcAddressType	Address;
	///郵編
	TThostFtdcZipCodeType	ZipCode;
	///電話號碼
	TThostFtdcTelephoneType	Telephone;
	///手機
	TThostFtdcMobilePhoneType	MobilePhone;
	///傳真
	TThostFtdcFaxType	Fax;
	///電子郵件
	TThostFtdcEMailType	EMail;
	///資金帳戶狀態
	TThostFtdcMoneyAccountStatusType	MoneyAccountStatus;
	///銀行帳號
	TThostFtdcBankAccountType	BankAccount;
	///銀行密碼
	TThostFtdcPasswordType	BankPassWord;
	///新銀行帳號
	TThostFtdcBankAccountType	NewBankAccount;
	///新銀行密碼
	TThostFtdcPasswordType	NewBankPassWord;
	///投資者帳號
	TThostFtdcAccountIDType	AccountID;
	///期貨密碼
	TThostFtdcPasswordType	Password;
	///銀行帳號類型
	TThostFtdcBankAccTypeType	BankAccType;
	///安裝編號
	TThostFtdcInstallIDType	InstallID;
	///驗證客戶證件號碼標誌
	TThostFtdcYesNoIndicatorType	VerifyCertNoFlag;
	///幣種代碼
	TThostFtdcCurrencyIDType	CurrencyID;
	///期貨公司銀行編碼
	TThostFtdcBankCodingForFutureType	BrokerIDByBank;
	///銀行密碼標誌
	TThostFtdcPwdFlagType	BankPwdFlag;
	///期貨資金密碼核對標誌
	TThostFtdcPwdFlagType	SecuPwdFlag;
	///交易ID
	TThostFtdcTIDType	TID;
	///摘要
	TThostFtdcDigestType	Digest;
	///錯誤代碼
	TThostFtdcErrorIDType	ErrorID;
	///錯誤資訊
	TThostFtdcErrorMsgType	ErrorMsg;
};

///災備中心交易許可權
struct CThostFtdcUserRightsAssignField
{
	///應用單元代碼
	TThostFtdcBrokerIDType	BrokerID;
	///用戶代碼
	TThostFtdcUserIDType	UserID;
	///交易中心代碼
	TThostFtdcDRIdentityIDType	DRIdentityID;
};

///經濟公司是否有在本標示的交易許可權
struct CThostFtdcBrokerUserRightAssignField
{
	///應用單元代碼
	TThostFtdcBrokerIDType	BrokerID;
	///交易中心代碼
	TThostFtdcDRIdentityIDType	DRIdentityID;
	///能否交易
	TThostFtdcBoolType	Tradeable;
};

///災備交易轉換報文
struct CThostFtdcDRTransferField
{
	///原交易中心代碼
	TThostFtdcDRIdentityIDType	OrigDRIdentityID;
	///目標交易中心代碼
	TThostFtdcDRIdentityIDType	DestDRIdentityID;
	///原應用單元代碼
	TThostFtdcBrokerIDType	OrigBrokerID;
	///目標易用單元代碼
	TThostFtdcBrokerIDType	DestBrokerID;
};



#endif

