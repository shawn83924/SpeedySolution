//------------------------------------------------------------------------------------------------------------------------
#ifndef _KSFT
//------------------------------------------------------------------------------------------------------------------------
#ifndef KStrdBCBEventsH
#define KStrdBCBEventsH
//------------------------------------------------------------------------------------------------------------------------
#include "KStrdAPIDynamic.h"
#include "Pthread.h"
//------------------------------------------------------------------------------------------------------------------------
//
// Class to initialize KingStar KStrd API
//
//------------------------------------------------------------------------------------------------------------------------
class KStrdTraderApiInit
{
private:
	bool	FSupport;  ///< Support KStrd API ?
	char	FNotSupportReason[ 128 ]; ///< Reason of load KStrd API failed.
	int		FFuncRtn;  ///< Bind which function failed ?
	int		FRegCBRtn; ///< Bind which callback regist function failed ?
private:
	int		InitFunctions( void ); ///< Bind all functions from DLL
	int		InitRegCallbackFunctions( void ); ///< Bind all callback regist functions from DLL
public:
	bool	SupportKStrdAPI( void )	{ return FSupport; }
	const char* Reason( void )		{ return FNotSupportReason; }
public: ///< All Functions
	// ---- CThostFtdcTraderApi ----
	lpCreateKStrdTraderApi					pCreateKStrdTraderApi;
	lpReleaseKStrdTraderApi					pReleaseKStrdTraderApi;
	lpInit									pInit;
	lpGetTradingDay							pGetTradingDay;
	lpRegisterFront							pRegisterFront;
	lpRegisterNameServer					pRegisterNameServer;
	lpRegisterFensUserInfo					pRegisterFensUserInfo;
	lpSubscribePrivateTopic					pSubscribePrivateTopic;
	lpSubscribePublicTopic					pSubscribePublicTopic;
	lpReqAuthenticate						pReqAuthenticate;
	lpReqUserLogin							pReqUserLogin;
	lpReqUserLogout							pReqUserLogout;
	lpReqUserPasswordUpdate					pReqUserPasswordUpdate;
	lpReqTradingAccountPasswordUpdate		pReqTradingAccountPasswordUpdate;
	lpReqOrderInsert						pReqOrderInsert;
	lpReqParkedOrderInsert					pReqParkedOrderInsert;
	lpReqParkedOrderAction					pReqParkedOrderAction;
	lpReqOrderAction						pReqOrderAction;
	lpReqQueryMaxOrderVolume				pReqQueryMaxOrderVolume;
	lpReqSettlementInfoConfirm				pReqSettlementInfoConfirm;
	lpReqRemoveParkedOrder					pReqRemoveParkedOrder;
	lpReqRemoveParkedOrderAction			pReqRemoveParkedOrderAction;
	lpReqExecOrderInsert					pReqExecOrderInsert;
	lpReqExecOrderAction					pReqExecOrderAction;
	lpReqForQuoteInsert						pReqForQuoteInsert;
	lpReqQuoteInsert						pReqQuoteInsert;
	lpReqQuoteAction						pReqQuoteAction;
	lpReqQryOrder							pReqQryOrder;
	lpReqQryTrade							pReqQryTrade;
	lpReqQryInvestorPosition				pReqQryInvestorPosition;
	lpReqQryTradingAccount					pReqQryTradingAccount;
	lpReqQryInvestor						pReqQryInvestor;
	lpReqQryTradingCode						pReqQryTradingCode;
	lpReqQryInstrumentMarginRate			pReqQryInstrumentMarginRate;
	lpReqQryInstrumentCommissionRate		pReqQryInstrumentCommissionRate;
	lpReqQryExchange						pReqQryExchange;
	lpReqQryProduct							pReqQryProduct;
	lpReqQryInstrument						pReqQryInstrument;
	lpReqQryDepthMarketData					pReqQryDepthMarketData;
	lpReqQrySettlementInfo					pReqQrySettlementInfo;
	lpReqQryTransferBank					pReqQryTransferBank;
	lpReqQryInvestorPositionDetail			pReqQryInvestorPositionDetail;
	lpReqQryNotice							pReqQryNotice;
	lpReqQrySettlementInfoConfirm			pReqQrySettlementInfoConfirm;
	lpReqQryInvestorPositionCombineDetail	pReqQryInvestorPositionCombineDetail;
	lpReqQryCFMMCTradingAccountKey			pReqQryCFMMCTradingAccountKey;
	lpReqQryEWarrantOffset					pReqQryEWarrantOffset;
	lpReqQryInvestorProductGroupMargin		pReqQryInvestorProductGroupMargin;
	lpReqQryExchangeMarginRate				pReqQryExchangeMarginRate;
	lpReqQryExchangeMarginRateAdjust		pReqQryExchangeMarginRateAdjust;
	lpReqQryExchangeRate					pReqQryExchangeRate;
	lpReqQrySecAgentACIDMap					pReqQrySecAgentACIDMap;
	lpReqQryOptionInstrTradeCost			pReqQryOptionInstrTradeCost;
	lpReqQryOptionInstrCommRate				pReqQryOptionInstrCommRate;
	lpReqQryExecOrder						pReqQryExecOrder;
	lpReqQryForQuote						pReqQryForQuote;
	lpReqQryQuote							pReqQryQuote;
	lpReqQryTransferSerial					pReqQryTransferSerial;
	lpReqQryAccountregister					pReqQryAccountregister;
	lpReqQryContractBank					pReqQryContractBank;
	lpReqQryParkedOrder						pReqQryParkedOrder;
	lpReqQryParkedOrderAction				pReqQryParkedOrderAction;
	lpReqQryTradingNotice					pReqQryTradingNotice;
	lpReqQryBrokerTradingParams				pReqQryBrokerTradingParams;
	lpReqQryBrokerTradingAlgos				pReqQryBrokerTradingAlgos;
	lpReqQueryCFMMCTradingAccountToken		pReqQueryCFMMCTradingAccountToken;
	lpReqFromBankToFutureByFuture			pReqFromBankToFutureByFuture;
	lpReqFromFutureToBankByFuture			pReqFromFutureToBankByFuture;
	lpReqQueryBankAccountMoneyByFuture		pReqQueryBankAccountMoneyByFuture;
	// v2.3.50212 // 20150410 by Tim Lin
	lpReqLockInsert							pReqLockInsert;///用戶端發出鎖定請求
	lpReqQryLock							pReqQryLock;///用戶端發出查詢鎖定請求
	lpReqQryLockPosition					pReqQryLockPosition;///用戶端發出查詢鎖定證券倉位請求
	// ---- CKSOptionApi ----
	// v2.3.50212 // 20150410 by Tim Lin
	lpReqQryIndexPrice						pReqQryIndexPrice;///查詢股指現貨指數
	lpReqQryOptionInstrGuard				pReqQryOptionInstrGuard;///查詢期權合約保障系數
	lpReqQryExecOrderVolume					pReqQryExecOrderVolume;///查詢宣告數量請求
	lpReqQryStockOptionAssignment			pReqQryStockOptionAssignment;///查詢個股行權指派信息
	lpReqQryInvestorTradeLevel				pReqQryInvestorTradeLevel;///查詢客戶交易級別
	lpReqQryPurchaseLimitAmt				pReqQryPurchaseLimitAmt;///查詢個股限購額度
	lpReqQryPositionLimitVol				pReqQryPositionLimitVol;///查詢個股限倉額度
	lpReqQryHistoryOrder					pReqQryHistoryOrder;///請求查詢個股歷史報單
	lpReqQryHistoryTrade					pReqQryHistoryTrade;///請求查詢個股歷史成交
	lpReqQryStockOptionHistoryAssignment	pReqQryHistoryAssignment;///請求查詢個股歷史行權指派明細
	lpReqQryStockOptionDelivDetail			pReqQrySODelivDetail;///請求查詢個股行權交割明細
	lpReqAutoExecOrderAction				pReqAutoExecOrderAction;///自動行權執行操作
	/*
	lpReqQryKSExecOrder						pReqQryKSExecOrder;
	lpReqKSExecOrderInsert					pReqKSExecOrderInsert;
	lpReqQryExecOrderVolume					pReqQryExecOrderVolume;
	lpReqQrySpotOptionParams				pReqQrySpotOptionParams;
	lpReqQryOptionInsInfo					pReqQryOptionInsInfo;
	lpReqQryStockOptionInsCommRate			pReqQryStockOptionInsCommRate;
	lpReqQryStockOptionInsMarginRate		pReqQryStockOptionInsMarginRate;
	lpReqQryStockOptionAssignment			pReqQryStockOptionAssignment;
	lpReqQryInvestorTradeLevel				pReqQryInvestorTradeLevel;
	lpReqQryPurchaseLimitAmt				pReqQryPurchaseLimitAmt;
	lpReqQryPositionLimitVol				pReqQryPositionLimitVol;
	lpReqQryHistoryOrder					pReqQryHistoryOrder;
	lpReqQryHistoryTrade					pReqQryHistoryTrade;
	lpReqQryStockOptionHistoryAssignment	pReqQryStockOptionHistoryAssignment;
	lpReqQryStockOptionDelivDetail			pReqQryStockOptionDelivDetail;
	lpReqAutoExecOrderAction				pReqAutoExecOrderAction;
	*/
	// ---- CKSVocApi ----
	// v2.3.50212 // 20150410 by Tim Lin
	lpReqQueryInvestorOpenPosition			pReqQueryInvestorOpenPosition;
	lpReqQueryInvestorOpenCombinePosition	pReqQueryInvestorOpenCombinePosition;
	lpReqBulkCancelOrder					pReqBulkCancelOrder;
	lpReqQryCloseStrategy					pReqQryCloseStrategy;
	lpReqQryCombStrategy					pReqQryCombStrategy;
	lpReqQryOptionCombStrategy				pReqQryOptionCombStrategy;
	lpReqQryTransferInfo					pReqQryTransferInfo;
	lpReqQryKSTradingNotice					pReqQryKSTradingNotice;
	lpReqQryUserProductUrl					pReqQryUserProductUrl;
	lpReqQryMaxCombActionVolume				pReqQryMaxCombActionVolume;
	lpReqQryKSInstrumentMarginRate          pReqQryKSInstrumentMarginRate;
	/*
	lpReqQryCrossRate						pReqQryCrossRate;
	lpReqMatchOrderInsert					pReqMatchOrderInsert;
	lpReqQryMatchOrder						pReqQryMatchOrder;
	lpReqQryMaxMatchOrderVolume				pReqQryMaxMatchOrderVolume;
	*/
public: ///< All callback regist functions.
	// ---- CThostFtdcTraderSpi ----
	lpRegOnFrontConnected							pRegOnFrontConnected;
	lpRegOnFrontDisconnected						pRegOnFrontDisconnected;
	lpRegOnHeartBeatWarning							pRegOnHeartBeatWarning;
	lpRegOnRspAuthenticate							pRegOnRspAuthenticate;
	lpRegOnRspUserLogin								pRegOnRspUserLogin;
	lpRegOnRspUserLogout							pRegOnRspUserLogout;
	lpRegOnRspUserPasswordUpdate					pRegOnRspUserPasswordUpdate;
	lpRegOnRspTradingAccountPasswordUpdate			pRegOnRspTradingAccountPasswordUpdate;
	lpRegOnRspOrderInsert							pRegOnRspOrderInsert;
	lpRegOnRspParkedOrderInsert						pRegOnRspParkedOrderInsert;
	lpRegOnRspParkedOrderAction						pRegOnRspParkedOrderAction;
	lpRegOnRspOrderAction							pRegOnRspOrderAction;
	lpRegOnRspQueryMaxOrderVolume					pRegOnRspQueryMaxOrderVolume;
	lpRegOnRspSettlementInfoConfirm					pRegOnRspSettlementInfoConfirm;
	lpRegOnRspRemoveParkedOrder						pRegOnRspRemoveParkedOrder;
	lpRegOnRspRemoveParkedOrderAction				pRegOnRspRemoveParkedOrderAction;
	lpRegOnRspExecOrderInsert						pRegOnRspExecOrderInsert;
	lpRegOnRspExecOrderAction						pRegOnRspExecOrderAction;
	lpRegOnRspForQuoteInsert						pRegOnRspForQuoteInsert;
	lpRegOnRspQuoteInsert							pRegOnRspQuoteInsert;
	lpRegOnRspQuoteAction							pRegOnRspQuoteAction;
	lpRegOnRspQryOrder								pRegOnRspQryOrder;
	lpRegOnRspQryTrade								pRegOnRspQryTrade;
	lpRegOnRspQryInvestorPosition					pRegOnRspQryInvestorPosition;
	lpRegOnRspQryTradingAccount						pRegOnRspQryTradingAccount;
	lpRegOnRspQryInvestor							pRegOnRspQryInvestor;
	lpRegOnRspQryTradingCode						pRegOnRspQryTradingCode;
	lpRegOnRspQryInstrumentMarginRate				pRegOnRspQryInstrumentMarginRate;
	lpRegOnRspQryInstrumentCommissionRate			pRegOnRspQryInstrumentCommissionRate;
	lpRegOnRspQryExchange							pRegOnRspQryExchange;
	lpRegOnRspQryProduct							pRegOnRspQryProduct;
	lpRegOnRspQryInstrument							pRegOnRspQryInstrument;
	lpRegOnRspQryDepthMarketData					pRegOnRspQryDepthMarketData;
	lpRegOnRspQrySettlementInfo						pRegOnRspQrySettlementInfo;
	lpRegOnRspQryTransferBank						pRegOnRspQryTransferBank;
	lpRegOnRspQryInvestorPositionDetail				pRegOnRspQryInvestorPositionDetail;
	lpRegOnRspQryNotice								pRegOnRspQryNotice;
	lpRegOnRspQrySettlementInfoConfirm				pRegOnRspQrySettlementInfoConfirm;
	lpRegOnRspQryInvestorPositionCombineDetail		pRegOnRspQryInvestorPositionCombineDetail;
	lpRegOnRspQryCFMMCTradingAccountKey				pRegOnRspQryCFMMCTradingAccountKey;
	lpRegOnRspQryEWarrantOffset						pRegOnRspQryEWarrantOffset;
	lpRegOnRspQryInvestorProductGroupMargin			pRegOnRspQryInvestorProductGroupMargin;
	lpRegOnRspQryExchangeMarginRate					pRegOnRspQryExchangeMarginRate;
	lpRegOnRspQryExchangeMarginRateAdjust			pRegOnRspQryExchangeMarginRateAdjust;
	lpRegOnRspQryExchangeRate						pRegOnRspQryExchangeRate;
	lpRegOnRspQrySecAgentACIDMap					pRegOnRspQrySecAgentACIDMap;
	lpRegOnRspQryOptionInstrTradeCost				pRegOnRspQryOptionInstrTradeCost;
	lpRegOnRspQryOptionInstrCommRate				pRegOnRspQryOptionInstrCommRate;
	lpRegOnRspQryExecOrder							pRegOnRspQryExecOrder;
	lpRegOnRspQryForQuote							pRegOnRspQryForQuote;
	lpRegOnRspQryQuote								pRegOnRspQryQuote;
	lpRegOnRspQryTransferSerial						pRegOnRspQryTransferSerial;
	lpRegOnRspQryAccountregister					pRegOnRspQryAccountregister;
	lpRegOnRspError									pRegOnRspError;
	lpRegOnRtnOrder									pRegOnRtnOrder;
	lpRegOnRtnTrade									pRegOnRtnTrade;
	lpRegOnErrRtnOrderInsert						pRegOnErrRtnOrderInsert;
	lpRegOnErrRtnOrderAction						pRegOnErrRtnOrderAction;
	lpRegOnRtnInstrumentStatus						pRegOnRtnInstrumentStatus;
	lpRegOnRtnTradingNotice							pRegOnRtnTradingNotice;
	lpRegOnRtnErrorConditionalOrder					pRegOnRtnErrorConditionalOrder;
	lpRegOnRtnExecOrder								pRegOnRtnExecOrder;
	lpRegOnErrRtnExecOrderInsert					pRegOnErrRtnExecOrderInsert;
	lpRegOnErrRtnExecOrderAction					pRegOnErrRtnExecOrderAction;
	lpRegOnErrRtnForQuoteInsert						pRegOnErrRtnForQuoteInsert;
	lpRegOnRtnQuote									pRegOnRtnQuote;
	lpRegOnErrRtnQuoteInsert						pRegOnErrRtnQuoteInsert;
	lpRegOnErrRtnQuoteAction						pRegOnErrRtnQuoteAction;
	lpRegOnRtnForQuoteRsp							pRegOnRtnForQuoteRsp;
	lpRegOnRtnCFMMCTradingAccountToken				pRegOnRtnCFMMCTradingAccountToken;
	lpRegOnRspQryContractBank						pRegOnRspQryContractBank;
	lpRegOnRspQryParkedOrder						pRegOnRspQryParkedOrder;
	lpRegOnRspQryParkedOrderAction					pRegOnRspQryParkedOrderAction;
	lpRegOnRspQryTradingNotice						pRegOnRspQryTradingNotice;
	lpRegOnRspQryBrokerTradingParams				pRegOnRspQryBrokerTradingParams;
	lpRegOnRspQryBrokerTradingAlgos					pRegOnRspQryBrokerTradingAlgos;
	lpRegOnRspQueryCFMMCTradingAccountToken			pRegOnRspQueryCFMMCTradingAccountToken;
	lpRegOnRtnFromBankToFutureByBank				pRegOnRtnFromBankToFutureByBank;
	lpRegOnRtnFromFutureToBankByBank				pRegOnRtnFromFutureToBankByBank;
	lpRegOnRtnRepealFromBankToFutureByBank			pRegOnRtnRepealFromBankToFutureByBank;
	lpRegOnRtnRepealFromFutureToBankByBank			pRegOnRtnRepealFromFutureToBankByBank;
	lpRegOnRtnFromBankToFutureByFuture				pRegOnRtnFromBankToFutureByFuture;
	lpRegOnRtnFromFutureToBankByFuture				pRegOnRtnFromFutureToBankByFuture;
	lpRegOnRtnRepealFromBankToFutureByFutureManual	pRegOnRtnRepealFromBankToFutureByFutureManual;
	lpRegOnRtnRepealFromFutureToBankByFutureManual	pRegOnRtnRepealFromFutureToBankByFutureManual;
	lpRegOnRtnQueryBankBalanceByFuture				pRegOnRtnQueryBankBalanceByFuture;
	lpRegOnErrRtnBankToFutureByFuture				pRegOnErrRtnBankToFutureByFuture;
	lpRegOnErrRtnFutureToBankByFuture				pRegOnErrRtnFutureToBankByFuture;
	lpRegOnErrRtnRepealBankToFutureByFutureManual	pRegOnErrRtnRepealBankToFutureByFutureManual;
	lpRegOnErrRtnRepealFutureToBankByFutureManual	pRegOnErrRtnRepealFutureToBankByFutureManual;
	lpRegOnErrRtnQueryBankBalanceByFuture			pRegOnErrRtnQueryBankBalanceByFuture;
	lpRegOnRtnRepealFromBankToFutureByFuture		pRegOnRtnRepealFromBankToFutureByFuture;
	lpRegOnRtnRepealFromFutureToBankByFuture		pRegOnRtnRepealFromFutureToBankByFuture;
	lpRegOnRspFromBankToFutureByFuture				pRegOnRspFromBankToFutureByFuture;
	lpRegOnRspFromFutureToBankByFuture				pRegOnRspFromFutureToBankByFuture;
	lpRegOnRspQueryBankAccountMoneyByFuture			pRegOnRspQueryBankAccountMoneyByFuture;
	lpRegOnRtnOpenAccountByBank						pRegOnRtnOpenAccountByBank;
	lpRegOnRtnCancelAccountByBank					pRegOnRtnCancelAccountByBank;
	lpRegOnRtnChangeAccountByBank					pRegOnRtnChangeAccountByBank;
	// v2.3.50212 // 20150410 by Tim Lin
	lpRegOnRspLockInsert							pRegOnRspLockInsert;///鎖定應答
	lpRegOnRspQryLock								pRegOnRspQryLock;///請求查詢鎖定回應
	lpRegOnRspQryLockPosition						pRegOnRspQryLockPosition;///請求查詢鎖定證券倉位回應
	lpRegOnRtnLock									pRegOnRtnLock;///鎖定通知
	lpRegOnErrRtnLockInsert							pRegOnErrRtnLockInsert;///鎖定錯誤通知
	// ---- CKSOptionSpi ----
	// v2.3.50212 // 20150410 by Tim Lin
	lpRegOnRspQryIndexPrice							pRegOnRspQryIndexPrice;///查詢股指現貨指數
	lpRegOnRspQryOptionInstrGuard					pRegOnRspQryOptionInstrGuard;///查詢期權合約保障系數
	lpRegOnRspQryExecOrderVolume					pRegOnRspQryExecOrderVolume;///查詢宣告數量請求
	lpRegOnRspQryStockOptionAssignment				pRegOnRspQryStockOptionAssignment;///查詢個股行權指派信息
	lpRegOnRspQryInvestorTradeLevel					pRegOnRspQryInvestorTradeLevel;///查詢客戶交易級別
	lpRegOnRspQryPurchaseLimitAmt					pRegOnRspQryPurchaseLimitAmt;///查詢個股限購額度
	lpRegOnRspQryPositionLimitVol					pRegOnRspQryPositionLimitVol;///查詢個股限倉額度
	lpRegOnRspQryHistoryOrder						pRegOnRspQryHistoryOrder;///請求查詢個股歷史報單
	lpRegOnRspQryHistoryTrade						pRegOnRspQryHistoryTrade;///請求查詢個股歷史成交
	lpRegOnRspQryStockOptionHistoryAssignment		pRegOnRspQryHistoryAssignment;///請求查詢個股歷史行權指派明細
	lpRegOnRspQryStockOptionDelivDetail				pRegOnRspQrySODelivDetail;///請求查詢個股行權交割明細
	lpRegOnRspAutoExecOrderAction					pRegOnRspQryAutoExecOrderAction;///自動行權執行操作
	/*
	lpRegOnRspQryKSExecOrder						pRegOnRspQryKSExecOrder;
	lpRegOnRspKSExecOrderInsert						pRegOnRspKSExecOrderInsert;
	lpRegOnRspQryExecOrderVolume					pRegOnRspQryExecOrderVolume;
	lpRegOnRspQrySpotOptionParams					pRegOnRspQrySpotOptionParams;
	lpRegOnRspQryOptionInsInfo						pRegOnRspQryOptionInsInfo;
	lpRegOnRspQryStockOptionInsCommRate				pRegOnRspQryStockOptionInsCommRate;
	lpRegOnRspQryStockOptionInsMarginRate			pRegOnRspQryStockOptionInsMarginRate;
	lpRegOnRspQryStockOptionAssignment				pRegOnRspQryStockOptionAssignment;
	lpRegOnRspQryInvestorTradeLevel					pRegOnRspQryInvestorTradeLevel;
	lpRegOnRspQryPurchaseLimitAmt					pRegOnRspQryPurchaseLimitAmt;
	lpRegOnRspQryPositionLimitVol					pRegOnRspQryPositionLimitVol;
	lpRegOnRspQryHistoryOrder						pRegOnRspQryHistoryOrder;
	lpRegOnRspQryHistoryTrade						pRegOnRspQryHistoryTrade;
	lpRegOnRspQryStockOptionHistoryAssignment		pRegOnRspQryStockOptionHistoryAssignment;
	lpRegOnRspQryStockOptionDelivDetail				pRegOnRspQryStockOptionDelivDetail;
	lpRegOnRspAutoExecOrderAction					pRegOnRspAutoExecOrderAction;
	lpRegOnRtnKSExecOrder							pRegOnRtnKSExecOrder;
	*/
	// ---- CKSVocSpi ----
	// v2.3.50212 // 20150410 by Tim Lin
	lpRegOnRspQryInvestorOpenPosition				pRegOnRspQryInvestorOpenPosition;
	lpRegOnRspQryInvestorOpenCombinePosition		pRegOnRspQryInvestorOpenCombinePosition;
	lpRegOnRspBulkCancelOrder						pRegOnRspBulkCancelOrder;
	lpRegOnRspQryCloseStrategy						pRegOnRspQryCloseStrategy;
	lpRegOnRspQryCombStrategy						pRegOnRspQryCombStrategy;
	lpRegOnRspQryOptionCombStrategy					pRegOnRspQryOptionCombStrategy;
	lpRegOnRspQryTransferInfo						pRegOnRspQryTransferInfo;
	lpRegOnRspQryKSTradingNotice					pRegOnRspQryKSTradingNotice;
	lpRegOnRspQryUserProductUrl						pRegOnRspQryUserProductUrl;
	lpRegOnRspQryMaxCombActionVolume				pRegOnRspQryMaxCombActionVolume;
	lpRegOnRtnKSTradingNotice						pRegOnRtnKSTradingNotice;
	lpRegOnRspQryKSInstrumentMarginRate				pRegOnRspQryKSInstrumentMarginRate;
	/*
	lpRegOnRspQryCrossRate							pRegOnRspQryCrossRate;
	lpRegOnRspMatchOrderInsert						pRegOnRspMatchOrderInsert;
	lpRegOnRspQryMatchOrder							pRegOnRspQryMatchOrder;
	lpRegOnRspQryMaxMatchOrderVolume				pRegOnRspQryMaxMatchOrderVolume;
	lpRegOnRtnKSTradingNotice						pRegOnRtnKSTradingNotice;
	lpRegOnRtnMatchOrder							pRegOnRtnMatchOrder;
	*/
public:
	// ---- CThostFtdcTraderSpi ----
	static void OnFrontConnected( HKSTRD Owner );
	static void OnFrontDisconnected( HKSTRD Owner, int nReason );
	static void OnHeartBeatWarning( HKSTRD Owner, int nTimeLapse );
	static void OnRspAuthenticate( HKSTRD Owner, KS_::CThostFtdcRspAuthenticateField *pRspAuthenticateField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspUserLogin( HKSTRD Owner, KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspUserLogout( HKSTRD Owner, KS_::CThostFtdcUserLogoutField *pUserLogout, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspUserPasswordUpdate( HKSTRD Owner, KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspTradingAccountPasswordUpdate( HKSTRD Owner, KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspOrderInsert( HKSTRD Owner, KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspParkedOrderInsert( HKSTRD Owner, KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspParkedOrderAction( HKSTRD Owner, KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspOrderAction( HKSTRD Owner, KS_::CThostFtdcInputOrderActionField *pInputOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQueryMaxOrderVolume( HKSTRD Owner, KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspSettlementInfoConfirm( HKSTRD Owner, KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspRemoveParkedOrder( HKSTRD Owner, KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspRemoveParkedOrderAction( HKSTRD Owner, KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspExecOrderInsert( HKSTRD Owner, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspExecOrderAction( HKSTRD Owner, KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspForQuoteInsert( HKSTRD Owner, KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQuoteInsert( HKSTRD Owner, KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQuoteAction( HKSTRD Owner, KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryOrder( HKSTRD Owner, KS_::CThostFtdcOrderField *pOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryTrade( HKSTRD Owner, KS_::CThostFtdcTradeField *pTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInvestorPosition( HKSTRD Owner, KS_::CThostFtdcInvestorPositionField *pInvestorPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryTradingAccount( HKSTRD Owner, KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInvestor( HKSTRD Owner, KS_::CThostFtdcInvestorField *pInvestor, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryTradingCode( HKSTRD Owner, KS_::CThostFtdcTradingCodeField *pTradingCode, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInstrumentMarginRate( HKSTRD Owner, KS_::CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInstrumentCommissionRate( HKSTRD Owner, KS_::CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryExchange( HKSTRD Owner, KS_::CThostFtdcExchangeField *pExchange, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryProduct( HKSTRD Owner, KS_::CThostFtdcProductField *pProduct, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInstrument( HKSTRD Owner, KS_::CThostFtdcInstrumentField *pInstrument, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryDepthMarketData( HKSTRD Owner, KS_::CThostFtdcDepthMarketDataField *pDepthMarketData, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQrySettlementInfo( HKSTRD Owner, KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryTransferBank( HKSTRD Owner, KS_::CThostFtdcTransferBankField *pTransferBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInvestorPositionDetail( HKSTRD Owner, KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryNotice( HKSTRD Owner, KS_::CThostFtdcNoticeField *pNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQrySettlementInfoConfirm( HKSTRD Owner, KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInvestorPositionCombineDetail( HKSTRD Owner, KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryCFMMCTradingAccountKey( HKSTRD Owner, KS_::CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryEWarrantOffset( HKSTRD Owner, KS_::CThostFtdcEWarrantOffsetField *pEWarrantOffset, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInvestorProductGroupMargin( HKSTRD Owner, KS_::CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryExchangeMarginRate( HKSTRD Owner, KS_::CThostFtdcExchangeMarginRateField *pExchangeMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryExchangeMarginRateAdjust( HKSTRD Owner, KS_::CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryExchangeRate( HKSTRD Owner, KS_::CThostFtdcExchangeRateField *pExchangeRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQrySecAgentACIDMap( HKSTRD Owner, KS_::CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryOptionInstrTradeCost( HKSTRD Owner, KS_::CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryOptionInstrCommRate( HKSTRD Owner, KS_::CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryExecOrder( HKSTRD Owner, KS_::CThostFtdcExecOrderField *pExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryForQuote( HKSTRD Owner, KS_::CThostFtdcForQuoteField *pForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryQuote( HKSTRD Owner, KS_::CThostFtdcQuoteField *pQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryTransferSerial( HKSTRD Owner, KS_::CThostFtdcTransferSerialField *pTransferSerial, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryAccountregister( HKSTRD Owner, KS_::CThostFtdcAccountregisterField *pAccountregister, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspError( HKSTRD Owner, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRtnOrder( HKSTRD Owner, KS_::CThostFtdcOrderField *pOrder );
	static void OnRtnTrade( HKSTRD Owner, KS_::CThostFtdcTradeField *pTrade );
	static void OnErrRtnOrderInsert( HKSTRD Owner, KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnOrderAction( HKSTRD Owner, KS_::CThostFtdcOrderActionField *pOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnRtnInstrumentStatus( HKSTRD Owner, KS_::CThostFtdcInstrumentStatusField *pInstrumentStatus );
	static void OnRtnTradingNotice( HKSTRD Owner, KS_::CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo );
	static void OnRtnErrorConditionalOrder( HKSTRD Owner, KS_::CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder );
	static void OnRtnExecOrder( HKSTRD Owner, KS_::CThostFtdcExecOrderField *pExecOrder );
	static void OnErrRtnExecOrderInsert( HKSTRD Owner, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnExecOrderAction( HKSTRD Owner, KS_::CThostFtdcExecOrderActionField *pExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnForQuoteInsert( HKSTRD Owner, KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnRtnQuote( HKSTRD Owner, KS_::CThostFtdcQuoteField *pQuote );
	static void OnErrRtnQuoteInsert( HKSTRD Owner, KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnQuoteAction( HKSTRD Owner, KS_::CThostFtdcQuoteActionField *pQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnRtnForQuoteRsp( HKSTRD Owner, KS_::CThostFtdcForQuoteRspField *pForQuoteRsp );
	static void OnRtnCFMMCTradingAccountToken( HKSTRD Owner, KS_::CThostFtdcCFMMCTradingAccountTokenField *pCFMMCTradingAccountToken );
	static void OnRspQryContractBank( HKSTRD Owner, KS_::CThostFtdcContractBankField *pContractBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryParkedOrder( HKSTRD Owner, KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryParkedOrderAction( HKSTRD Owner, KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryTradingNotice( HKSTRD Owner, KS_::CThostFtdcTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryBrokerTradingParams( HKSTRD Owner, KS_::CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryBrokerTradingAlgos( HKSTRD Owner, KS_::CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQueryCFMMCTradingAccountToken( HKSTRD Owner, KS_::CThostFtdcQueryCFMMCTradingAccountTokenField *pQueryCFMMCTradingAccountToken, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRtnFromBankToFutureByBank( HKSTRD Owner, KS_::CThostFtdcRspTransferField *pRspTransfer );
	static void OnRtnFromFutureToBankByBank( HKSTRD Owner, KS_::CThostFtdcRspTransferField *pRspTransfer );
	static void OnRtnRepealFromBankToFutureByBank( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
	static void OnRtnRepealFromFutureToBankByBank( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
	static void OnRtnFromBankToFutureByFuture( HKSTRD Owner, KS_::CThostFtdcRspTransferField *pRspTransfer );
	static void OnRtnFromFutureToBankByFuture( HKSTRD Owner, KS_::CThostFtdcRspTransferField *pRspTransfer );
	static void OnRtnRepealFromBankToFutureByFutureManual( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
	static void OnRtnRepealFromFutureToBankByFutureManual( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
	static void OnRtnQueryBankBalanceByFuture( HKSTRD Owner, KS_::CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount );
	static void OnErrRtnBankToFutureByFuture( HKSTRD Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnFutureToBankByFuture( HKSTRD Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnRepealBankToFutureByFutureManual( HKSTRD Owner, KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnRepealFutureToBankByFutureManual( HKSTRD Owner, KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnQueryBankBalanceByFuture( HKSTRD Owner, KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnRtnRepealFromBankToFutureByFuture( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
	static void OnRtnRepealFromFutureToBankByFuture( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
	static void OnRspFromBankToFutureByFuture( HKSTRD Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspFromFutureToBankByFuture( HKSTRD Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQueryBankAccountMoneyByFuture( HKSTRD Owner, KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRtnOpenAccountByBank( HKSTRD Owner, KS_::CThostFtdcOpenAccountField *pOpenAccount );
	static void OnRtnCancelAccountByBank( HKSTRD Owner, KS_::CThostFtdcCancelAccountField *pCancelAccount );
	static void OnRtnChangeAccountByBank( HKSTRD Owner, KS_::CThostFtdcChangeAccountField *pChangeAccount );
	// v2.3.50212 // 20150410 by Tim Lin
	static void OnRspLockInsert( HKSTRD Owner, KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///鎖定應答
	static void OnRspQryLock( HKSTRD Owner, KS_::CThostFtdcLockField *pLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢鎖
	static void OnRspQryLockPosition( HKSTRD Owner, KS_::CThostFtdcLockPositionField *pLockPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢鎖
	static void OnRtnLock( HKSTRD Owner, KS_::CThostFtdcLockField *pLock );///鎖定通知
	static void OnErrRtnLockInsert( HKSTRD Owner, KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo );///鎖定錯誤通
	// ---- CKSOptionSpi ----
	// v2.3.50212 // 20150410 by Tim Lin
	static void OnRspQryIndexPrice( HKSTRD Owner, KS_::CKSQryIndexPriceField *pQryIndexPrice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢股指現貨指數
	static void OnRspQryOptionInstrGuard( HKSTRD Owner, KS_::CKSQryOptionInstrGuardField *pQryOptionInstrGuard, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢期權合約保障系數
	static void OnRspQryExecOrderVolume( HKSTRD Owner, KS_::CKSQryExecOrderVolumeField *pQryExecOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢宣告數量請求
	static void OnRspQryStockOptionAssignment( HKSTRD Owner, KS_::CKSQryStockOptionAssignmentField *pQryStockOptionAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢個股行權指派信息
	static void OnRspQryInvestorTradeLevel( HKSTRD Owner, KS_::CKSQryInvestorTradeLevelField *pQryInvestorTradeLevel, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢客戶交易級別
	static void OnRspQryPurchaseLimitAmt( HKSTRD Owner, KS_::CKSQryPurchaseLimitAmtField *pQryPurchaseLimitAmt, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢個股限購額度
	static void OnRspQryPositionLimitVol( HKSTRD Owner, KS_::CKSQryPositionLimitVolField *pQryPositionLimitVol, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢個股限倉額度
	static void OnRspQryHistoryOrder( HKSTRD Owner, KS_::CKSQryHistoryOrderField *pQryHistoryOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢個股歷史報單
	static void OnRspQryHistoryTrade( HKSTRD Owner, KS_::CKSQryHistoryTradeField *pQryHistoryTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢個股歷史成交
	static void OnRspQryStockOptionHistoryAssignment( HKSTRD Owner, KS_::CKSQryHistoryAssignmentField *pQryHistoryAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢個股歷史行權指派明細
	static void OnRspQryStockOptionDelivDetail( HKSTRD Owner, KS_::CKSQrySODelivDetailField *pQrySODelivDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢個股行權交割明細
	static void OnRspAutoExecOrderAction( HKSTRD Owner, KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///自動行權執行操作
	/*
	static void OnRspQryKSExecOrder( HKSTRD Owner, KS_::CThostFtdcKSExecOrderField *pKSExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspKSExecOrderInsert( HKSTRD Owner, KS_::CThostFtdcInputKSExecOrderField *pInputKSExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryExecOrderVolume( HKSTRD Owner, KS_::CKSExecOrderVolumeField *pExecOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQrySpotOptionParams( HKSTRD Owner, KS_::CKSSpotOptionParamsField *pExecOrderParams, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryOptionInsInfo( HKSTRD Owner, KS_::CKSOptionInsInfoField *pOptionInsInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryStockOptionInsCommRate( HKSTRD Owner, KS_::CKSStockOptionInsCommRateField *pStockOptionInsCommRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryStockOptionInsMarginRate( HKSTRD Owner, KS_::CKSStockOptionInsMarginRateField *pStockOptionInsMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryStockOptionAssignment( HKSTRD Owner, KS_::CKSStockOptionAssignmentField *pStockOptionAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInvestorTradeLevel( HKSTRD Owner, KS_::CKSInvestorTradeLevelField *pInvestorTradeLevel, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryPurchaseLimitAmt( HKSTRD Owner, KS_::CKSPurchaseLimitAmtField *pPurchaseLimitAmt, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryPositionLimitVol( HKSTRD Owner, KS_::CKSPositionLimitVolField *pPositionLimitVol, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryHistoryOrder( HKSTRD Owner, KS_::CKSHistoryOrderField *pOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryHistoryTrade( HKSTRD Owner, KS_::CKSHistoryTradeField *pTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryStockOptionHistoryAssignment( HKSTRD Owner, KS_::CKSHistoryAssignmentField *pHistoryAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryStockOptionDelivDetail( HKSTRD Owner, KS_::CKSSODelivDetailField *pSODelivDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspAutoExecOrderAction( HKSTRD Owner, KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRtnKSExecOrder( HKSTRD Owner, KS_::CThostFtdcKSExecOrderRtnField *pExecOrder );
	*/
	// ---- CKSVocSpi ----
	// v2.3.50212 // 20150410 by Tim Lin
	static void OnRspQryInvestorOpenPosition( HKSTRD Owner, KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInvestorOpenCombinePosition( HKSTRD Owner, KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspBulkCancelOrder( HKSTRD Owner, KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryCloseStrategy( HKSTRD Owner, KS_::CKSCloseStrategyResultField *pCloseStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryCombStrategy( HKSTRD Owner, KS_::CKSCombStrategyResultField *pCombStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryOptionCombStrategy( HKSTRD Owner, KS_::CKSOptionCombStrategyResultField *pOptionCombStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryTransferInfo( HKSTRD Owner, KS_::CKSTransferInfoResultField *pResultField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryKSTradingNotice( HKSTRD Owner, KS_::CKSTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryUserProductUrl( HKSTRD Owner, KS_::CKSUserProductUrlField *pUserProductUrl, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryMaxCombActionVolume( HKSTRD Owner, KS_::CKSMaxCombActionVolumeField *pMaxCombActionVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRtnKSTradingNotice( HKSTRD Owner, KS_::CKSTradingNoticeField *pTradingNoticeInfo);
	static void OnRspQryKSInstrumentMarginRate( HKSTRD Owner, KS_::CKSInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	/*
	static void OnRspQryCrossRate( HKSTRD Owner, KS_::CKSCrossRateField *pCrossRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspMatchOrderInsert( HKSTRD Owner, KS_::CKSMatchOrderInsertField *pMatchOrderInsert, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryMatchOrder( HKSTRD Owner, KS_::CKSMatchOrderField *pMatchOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryMaxMatchOrderVolume( HKSTRD Owner, KS_::CKSMaxMatchOrderVolumeField *pMaxMatchOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRtnKSTradingNotice( HKSTRD Owner, KS_::CKSTradingNoticeField *pTradingNoticeInfo );
	static void OnRtnMatchOrder( HKSTRD Owner, KS_::CKSMatchOrderField *pMatchOrder );
	*/
public:
	KStrdTraderApiInit();
	~KStrdTraderApiInit();
};
//------------------------------------------------------------------------------------------------------------------------
extern KStrdTraderApiInit gKStrdAPI;
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------

