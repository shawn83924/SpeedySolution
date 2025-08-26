//------------------------------------------------------------------------------------------------------------------------
#ifndef __KStrdAPIDynamic_H
#define __KStrdAPIDynamic_H
//------------------------------------------------------------------------------------------------------------------------
#include "inc\KSTradeAPI.h"
//------------------------------------------------------------------------------------------------------------------------
typedef void* HKSTRD;
//------------------------------------------------------------------------------------------------------------------------
#ifndef Namespace__KingstarAPI_
#define Namespace__KingstarAPI_
#define KS_ KingstarAPI
#endif
//------------------------------------------------------------------------------------------------------------------------
extern "C"
{
//------------------------------------------------------------------------------------------------------------------------
// Define all callback functions.
//------------------------------------------------------------------------------------------------------------------------
// ---- CThostFtdcTraderSpi ----
typedef void (__cdecl *pfOnFrontConnected) ( HKSTRD Owner );
typedef void (__cdecl *pfOnFrontDisconnected) ( HKSTRD Owner, int nReason );
typedef void (__cdecl *pfOnHeartBeatWarning) ( HKSTRD Owner, int nTimeLapse );
typedef void (__cdecl *pfOnRspAuthenticate) ( HKSTRD Owner, KS_::CThostFtdcRspAuthenticateField *pRspAuthenticateField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspUserLogin) ( HKSTRD Owner, KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspUserLogout) ( HKSTRD Owner, KS_::CThostFtdcUserLogoutField *pUserLogout, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspUserPasswordUpdate) ( HKSTRD Owner, KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspTradingAccountPasswordUpdate) ( HKSTRD Owner, KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspOrderInsert) ( HKSTRD Owner, KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspParkedOrderInsert) ( HKSTRD Owner, KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspParkedOrderAction) ( HKSTRD Owner, KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspOrderAction) ( HKSTRD Owner, KS_::CThostFtdcInputOrderActionField *pInputOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQueryMaxOrderVolume) ( HKSTRD Owner, KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspSettlementInfoConfirm) ( HKSTRD Owner, KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspRemoveParkedOrder) ( HKSTRD Owner, KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspRemoveParkedOrderAction) ( HKSTRD Owner, KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspExecOrderInsert) ( HKSTRD Owner, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspExecOrderAction) ( HKSTRD Owner, KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspForQuoteInsert) ( HKSTRD Owner, KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQuoteInsert) ( HKSTRD Owner, KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQuoteAction) ( HKSTRD Owner, KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryOrder) ( HKSTRD Owner, KS_::CThostFtdcOrderField *pOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryTrade) ( HKSTRD Owner, KS_::CThostFtdcTradeField *pTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryInvestorPosition) ( HKSTRD Owner, KS_::CThostFtdcInvestorPositionField *pInvestorPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryTradingAccount) ( HKSTRD Owner, KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryInvestor) ( HKSTRD Owner, KS_::CThostFtdcInvestorField *pInvestor, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryTradingCode) ( HKSTRD Owner, KS_::CThostFtdcTradingCodeField *pTradingCode, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryInstrumentMarginRate) ( HKSTRD Owner, KS_::CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryInstrumentCommissionRate) ( HKSTRD Owner, KS_::CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryExchange) ( HKSTRD Owner, KS_::CThostFtdcExchangeField *pExchange, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryProduct) ( HKSTRD Owner, KS_::CThostFtdcProductField *pProduct, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryInstrument) ( HKSTRD Owner, KS_::CThostFtdcInstrumentField *pInstrument, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryDepthMarketData) ( HKSTRD Owner, KS_::CThostFtdcDepthMarketDataField *pDepthMarketData, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQrySettlementInfo) ( HKSTRD Owner, KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryTransferBank) ( HKSTRD Owner, KS_::CThostFtdcTransferBankField *pTransferBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryInvestorPositionDetail) ( HKSTRD Owner, KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryNotice) ( HKSTRD Owner, KS_::CThostFtdcNoticeField *pNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQrySettlementInfoConfirm) ( HKSTRD Owner, KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryInvestorPositionCombineDetail) ( HKSTRD Owner, KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryCFMMCTradingAccountKey) ( HKSTRD Owner, KS_::CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryEWarrantOffset) ( HKSTRD Owner, KS_::CThostFtdcEWarrantOffsetField *pEWarrantOffset, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryInvestorProductGroupMargin) ( HKSTRD Owner, KS_::CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryExchangeMarginRate) ( HKSTRD Owner, KS_::CThostFtdcExchangeMarginRateField *pExchangeMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryExchangeMarginRateAdjust) ( HKSTRD Owner, KS_::CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryExchangeRate) ( HKSTRD Owner, KS_::CThostFtdcExchangeRateField *pExchangeRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQrySecAgentACIDMap) ( HKSTRD Owner, KS_::CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryOptionInstrTradeCost) ( HKSTRD Owner, KS_::CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryOptionInstrCommRate) ( HKSTRD Owner, KS_::CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryExecOrder) ( HKSTRD Owner, KS_::CThostFtdcExecOrderField *pExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryForQuote) ( HKSTRD Owner, KS_::CThostFtdcForQuoteField *pForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryQuote) ( HKSTRD Owner, KS_::CThostFtdcQuoteField *pQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryTransferSerial) ( HKSTRD Owner, KS_::CThostFtdcTransferSerialField *pTransferSerial, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryAccountregister) ( HKSTRD Owner, KS_::CThostFtdcAccountregisterField *pAccountregister, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspError) ( HKSTRD Owner, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRtnOrder) ( HKSTRD Owner, KS_::CThostFtdcOrderField *pOrder );
typedef void (__cdecl *pfOnRtnTrade) ( HKSTRD Owner, KS_::CThostFtdcTradeField *pTrade );
typedef void (__cdecl *pfOnErrRtnOrderInsert) ( HKSTRD Owner, KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo );
typedef void (__cdecl *pfOnErrRtnOrderAction) ( HKSTRD Owner, KS_::CThostFtdcOrderActionField *pOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo );
typedef void (__cdecl *pfOnRtnInstrumentStatus) ( HKSTRD Owner, KS_::CThostFtdcInstrumentStatusField *pInstrumentStatus );
typedef void (__cdecl *pfOnRtnTradingNotice) ( HKSTRD Owner, KS_::CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo );
typedef void (__cdecl *pfOnRtnErrorConditionalOrder) ( HKSTRD Owner, KS_::CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder );
typedef void (__cdecl *pfOnRtnExecOrder) ( HKSTRD Owner, KS_::CThostFtdcExecOrderField *pExecOrder );
typedef void (__cdecl *pfOnErrRtnExecOrderInsert) ( HKSTRD Owner, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo );
typedef void (__cdecl *pfOnErrRtnExecOrderAction) ( HKSTRD Owner, KS_::CThostFtdcExecOrderActionField *pExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo );
typedef void (__cdecl *pfOnErrRtnForQuoteInsert) ( HKSTRD Owner, KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo );
typedef void (__cdecl *pfOnRtnQuote) ( HKSTRD Owner, KS_::CThostFtdcQuoteField *pQuote );
typedef void (__cdecl *pfOnErrRtnQuoteInsert) ( HKSTRD Owner, KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo );
typedef void (__cdecl *pfOnErrRtnQuoteAction) ( HKSTRD Owner, KS_::CThostFtdcQuoteActionField *pQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo );
typedef void (__cdecl *pfOnRtnForQuoteRsp) ( HKSTRD Owner, KS_::CThostFtdcForQuoteRspField *pForQuoteRsp );
typedef void (__cdecl *pfOnRtnCFMMCTradingAccountToken) ( HKSTRD Owner, KS_::CThostFtdcCFMMCTradingAccountTokenField *pCFMMCTradingAccountToken );
typedef void (__cdecl *pfOnRspQryContractBank) ( HKSTRD Owner, KS_::CThostFtdcContractBankField *pContractBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryParkedOrder) ( HKSTRD Owner, KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryParkedOrderAction) ( HKSTRD Owner, KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryTradingNotice) ( HKSTRD Owner, KS_::CThostFtdcTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryBrokerTradingParams) ( HKSTRD Owner, KS_::CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQryBrokerTradingAlgos) ( HKSTRD Owner, KS_::CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQueryCFMMCTradingAccountToken) ( HKSTRD Owner, KS_::CThostFtdcQueryCFMMCTradingAccountTokenField *pQueryCFMMCTradingAccountToken, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRtnFromBankToFutureByBank) ( HKSTRD Owner, KS_::CThostFtdcRspTransferField *pRspTransfer );
typedef void (__cdecl *pfOnRtnFromFutureToBankByBank) ( HKSTRD Owner, KS_::CThostFtdcRspTransferField *pRspTransfer );
typedef void (__cdecl *pfOnRtnRepealFromBankToFutureByBank) ( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
typedef void (__cdecl *pfOnRtnRepealFromFutureToBankByBank) ( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
typedef void (__cdecl *pfOnRtnFromBankToFutureByFuture) ( HKSTRD Owner, KS_::CThostFtdcRspTransferField *pRspTransfer );
typedef void (__cdecl *pfOnRtnFromFutureToBankByFuture) ( HKSTRD Owner, KS_::CThostFtdcRspTransferField *pRspTransfer );
typedef void (__cdecl *pfOnRtnRepealFromBankToFutureByFutureManual) ( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
typedef void (__cdecl *pfOnRtnRepealFromFutureToBankByFutureManual) ( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
typedef void (__cdecl *pfOnRtnQueryBankBalanceByFuture) ( HKSTRD Owner, KS_::CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount );
typedef void (__cdecl *pfOnErrRtnBankToFutureByFuture) ( HKSTRD Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo );
typedef void (__cdecl *pfOnErrRtnFutureToBankByFuture) ( HKSTRD Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo );
typedef void (__cdecl *pfOnErrRtnRepealBankToFutureByFutureManual) ( HKSTRD Owner, KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo );
typedef void (__cdecl *pfOnErrRtnRepealFutureToBankByFutureManual) ( HKSTRD Owner, KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo );
typedef void (__cdecl *pfOnErrRtnQueryBankBalanceByFuture) ( HKSTRD Owner, KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo );
typedef void (__cdecl *pfOnRtnRepealFromBankToFutureByFuture) ( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
typedef void (__cdecl *pfOnRtnRepealFromFutureToBankByFuture) ( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
typedef void (__cdecl *pfOnRspFromBankToFutureByFuture) ( HKSTRD Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspFromFutureToBankByFuture) ( HKSTRD Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRspQueryBankAccountMoneyByFuture) ( HKSTRD Owner, KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
typedef void (__cdecl *pfOnRtnOpenAccountByBank) ( HKSTRD Owner, KS_::CThostFtdcOpenAccountField *pOpenAccount );
typedef void (__cdecl *pfOnRtnCancelAccountByBank) ( HKSTRD Owner, KS_::CThostFtdcCancelAccountField *pCancelAccount );
typedef void (__cdecl *pfOnRtnChangeAccountByBank) ( HKSTRD Owner, KS_::CThostFtdcChangeAccountField *pChangeAccount );
// v2.3.50212 // 20150410 by Tim Lin // <Step>.1
typedef void (__cdecl *pfOnRspLockInsert) ( HKSTRD Owner, KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);///鎖定應答
typedef void (__cdecl *pfOnRspQryLock) ( HKSTRD Owner, KS_::CThostFtdcLockField *pLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);///請求查詢鎖定回應	
typedef void (__cdecl *pfOnRspQryLockPosition) ( HKSTRD Owner, KS_::CThostFtdcLockPositionField *pLockPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);///請求查詢鎖定證券倉位回應
typedef void (__cdecl *pfOnRtnLock) ( HKSTRD Owner, KS_::CThostFtdcLockField *pLock);///鎖定通知
typedef void (__cdecl *pfOnErrRtnLockInsert) ( HKSTRD Owner, KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo);///鎖定錯誤通知	
// ---- CKSOptionSpi ----
// v2.3.50212 // 20150410 by Tim Lin // Step.1
typedef void (__cdecl *pfOnRspQryIndexPrice) ( HKSTRD Owner, KS_::CKSQryIndexPriceField *pQryIndexPrice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢股指現貨指數
typedef void (__cdecl *pfOnRspQryOptionInstrGuard) ( HKSTRD Owner, KS_::CKSQryOptionInstrGuardField *pQryOptionInstrGuard, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢期權合約保障系數
typedef void (__cdecl *pfOnRspQryExecOrderVolume) ( HKSTRD Owner, KS_::CKSQryExecOrderVolumeField *pQryExecOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢宣告數量請求
typedef void (__cdecl *pfOnRspQryStockOptionAssignment) ( HKSTRD Owner, KS_::CKSQryStockOptionAssignmentField *pQryStockOptionAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢個股行權指派信息
typedef void (__cdecl *pfOnRspQryInvestorTradeLevel) ( HKSTRD Owner, KS_::CKSQryInvestorTradeLevelField *pQryInvestorTradeLevel, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢客戶交易級別
typedef void (__cdecl *pfOnRspQryPurchaseLimitAmt) ( HKSTRD Owner, KS_::CKSQryPurchaseLimitAmtField *pQryPurchaseLimitAmt, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢個股限購額度
typedef void (__cdecl *pfOnRspQryPositionLimitVol) ( HKSTRD Owner, KS_::CKSQryPositionLimitVolField *pQryPositionLimitVol, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢個股限倉額度
typedef void (__cdecl *pfOnRspQryHistoryOrder) ( HKSTRD Owner, KS_::CKSQryHistoryOrderField *pQryHistoryOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢個股歷史報單
typedef void (__cdecl *pfOnRspQryHistoryTrade) ( HKSTRD Owner, KS_::CKSQryHistoryTradeField *pQryHistoryTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢個股歷史成交
typedef void (__cdecl *pfOnRspQryStockOptionHistoryAssignment) ( HKSTRD Owner, KS_::CKSQryHistoryAssignmentField *pQryHistoryAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢個股歷史行權指派明細
typedef void (__cdecl *pfOnRspQryStockOptionDelivDetail) ( HKSTRD Owner, KS_::CKSQrySODelivDetailField *pQrySODelivDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢個股行權交割明細
typedef void (__cdecl *pfOnRspAutoExecOrderAction) ( HKSTRD Owner, KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///自動行權執行操作
// ---- CKSVocSpi ----
// v2.3.50212 // 20150410 by Tim Lin // [Step].1
typedef void (__cdecl *pfOnRspQryInvestorOpenPosition) ( HKSTRD Owner, KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢開盤前的持倉明細應答
typedef void (__cdecl *pfOnRspQryInvestorOpenCombinePosition) ( HKSTRD Owner, KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢開盤前的組合持倉明細應答
typedef void (__cdecl *pfOnRspBulkCancelOrder) ( HKSTRD Owner, KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///批量報單撤除請求回應
typedef void (__cdecl *pfOnRspQryCloseStrategy) ( HKSTRD Owner, KS_::CKSCloseStrategyResultField *pCloseStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///平倉策略查詢回應
typedef void (__cdecl *pfOnRspQryCombStrategy) ( HKSTRD Owner, KS_::CKSCombStrategyResultField *pCombStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///組合策略查詢回應
typedef void (__cdecl *pfOnRspQryOptionCombStrategy) ( HKSTRD Owner, KS_::CKSOptionCombStrategyResultField *pOptionCombStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///期權組合策略查詢回應
typedef void (__cdecl *pfOnRspQryTransferInfo) ( HKSTRD Owner, KS_::CKSTransferInfoResultField *pResultField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢客戶轉帳資訊回應
typedef void (__cdecl *pfOnRspQryKSTradingNotice) ( HKSTRD Owner, KS_::CKSTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢交易通知回應
typedef void (__cdecl *pfOnRspQryUserProductUrl) ( HKSTRD Owner, KS_::CKSUserProductUrlField *pUserProductUrl, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///使用者端產品資源查詢應答
typedef void (__cdecl *pfOnRspQryMaxCombActionVolume) ( HKSTRD Owner, KS_::CKSMaxCombActionVolumeField *pMaxCombActionVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);///最大組合拆分單量查詢請求回應
typedef void (__cdecl *pfOnRtnKSTradingNotice) ( HKSTRD Owner, KS_::CKSTradingNoticeField *pTradingNoticeInfo);///交易通知
typedef void (__cdecl *pfOnRspQryKSInstrumentMarginRate) ( HKSTRD Owner, KS_::CKSInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);///請求查詢合約保證金率
//------------------------------------------------------------------------------------------------------------------------
// Define all export function pointors
//------------------------------------------------------------------------------------------------------------------------
// ---- CThostFtdcTraderApi ----
typedef HKSTRD (__cdecl *lpCreateKStrdTraderApi) ( void* Owner );
typedef void (__cdecl *lpReleaseKStrdTraderApi) ( HKSTRD hKStrd );
typedef void (__cdecl *lpInit) ( HKSTRD hKStrd );
typedef const char* (__cdecl *lpGetTradingDay) ( HKSTRD hKStrd );
typedef void (__cdecl *lpRegisterFront) ( HKSTRD hKStrd, char *pszFrontAddress );
typedef void (__cdecl *lpRegisterNameServer) ( HKSTRD hKStrd, char *pszNsAddress );
typedef void (__cdecl *lpRegisterFensUserInfo) ( HKSTRD hKStrd, KS_::CThostFtdcFensUserInfoField * pFensUserInfo );
//typedef void (__cdecl *lpRegisterSpi) ( HKSTRD hKStrd, KS_::CThostFtdcTraderSpi *pSpi );
typedef void (__cdecl *lpSubscribePrivateTopic) ( HKSTRD hKStrd, KS_::THOST_TE_RESUME_TYPE nResumeType );
typedef void (__cdecl *lpSubscribePublicTopic) ( HKSTRD hKStrd, KS_::THOST_TE_RESUME_TYPE nResumeType );
typedef int  (__cdecl *lpReqAuthenticate) ( HKSTRD hKStrd, KS_::CThostFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID );
typedef int  (__cdecl *lpReqUserLogin) ( HKSTRD hKStrd, KS_::CThostFtdcReqUserLoginField *pReqUserLoginField, int nRequestID );
typedef int  (__cdecl *lpReqUserLogout) ( HKSTRD hKStrd, KS_::CThostFtdcUserLogoutField *pUserLogout, int nRequestID );
typedef int  (__cdecl *lpReqUserPasswordUpdate) ( HKSTRD hKStrd, KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID );
typedef int  (__cdecl *lpReqTradingAccountPasswordUpdate) ( HKSTRD hKStrd, KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, int nRequestID );
typedef int  (__cdecl *lpReqOrderInsert) ( HKSTRD hKStrd, KS_::CThostFtdcInputOrderField *pInputOrder, int nRequestID );
typedef int  (__cdecl *lpReqParkedOrderInsert) ( HKSTRD hKStrd, KS_::CThostFtdcParkedOrderField *pParkedOrder, int nRequestID );
typedef int  (__cdecl *lpReqParkedOrderAction) ( HKSTRD hKStrd, KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, int nRequestID );
typedef int  (__cdecl *lpReqOrderAction) ( HKSTRD hKStrd, KS_::CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID );
typedef int  (__cdecl *lpReqQueryMaxOrderVolume) ( HKSTRD hKStrd, KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, int nRequestID );
typedef int  (__cdecl *lpReqSettlementInfoConfirm) ( HKSTRD hKStrd, KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, int nRequestID );
typedef int  (__cdecl *lpReqRemoveParkedOrder) ( HKSTRD hKStrd, KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, int nRequestID );
typedef int  (__cdecl *lpReqRemoveParkedOrderAction) ( HKSTRD hKStrd, KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, int nRequestID );
typedef int  (__cdecl *lpReqExecOrderInsert) ( HKSTRD hKStrd, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, int nRequestID );
typedef int  (__cdecl *lpReqExecOrderAction) ( HKSTRD hKStrd, KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, int nRequestID );
typedef int  (__cdecl *lpReqForQuoteInsert) ( HKSTRD hKStrd, KS_::CThostFtdcInputForQuoteField *pInputForQuote, int nRequestID );
typedef int  (__cdecl *lpReqQuoteInsert) ( HKSTRD hKStrd, KS_::CThostFtdcInputQuoteField *pInputQuote, int nRequestID );
typedef int  (__cdecl *lpReqQuoteAction) ( HKSTRD hKStrd, KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, int nRequestID );
typedef int  (__cdecl *lpReqQryOrder) ( HKSTRD hKStrd, KS_::CThostFtdcQryOrderField *pQryOrder, int nRequestID );
typedef int  (__cdecl *lpReqQryTrade) ( HKSTRD hKStrd, KS_::CThostFtdcQryTradeField *pQryTrade, int nRequestID );
typedef int  (__cdecl *lpReqQryInvestorPosition) ( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID );
typedef int  (__cdecl *lpReqQryTradingAccount) ( HKSTRD hKStrd, KS_::CThostFtdcQryTradingAccountField *pQryTradingAccount, int nRequestID );
typedef int  (__cdecl *lpReqQryInvestor) ( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorField *pQryInvestor, int nRequestID );
typedef int  (__cdecl *lpReqQryTradingCode) ( HKSTRD hKStrd, KS_::CThostFtdcQryTradingCodeField *pQryTradingCode, int nRequestID );
typedef int  (__cdecl *lpReqQryInstrumentMarginRate) ( HKSTRD hKStrd, KS_::CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID );
typedef int  (__cdecl *lpReqQryInstrumentCommissionRate) ( HKSTRD hKStrd, KS_::CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate, int nRequestID );
typedef int  (__cdecl *lpReqQryExchange) ( HKSTRD hKStrd, KS_::CThostFtdcQryExchangeField *pQryExchange, int nRequestID );
typedef int  (__cdecl *lpReqQryProduct) ( HKSTRD hKStrd, KS_::CThostFtdcQryProductField *pQryProduct, int nRequestID );
typedef int  (__cdecl *lpReqQryInstrument) ( HKSTRD hKStrd, KS_::CThostFtdcQryInstrumentField *pQryInstrument, int nRequestID );
typedef int  (__cdecl *lpReqQryDepthMarketData) ( HKSTRD hKStrd, KS_::CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, int nRequestID );
typedef int  (__cdecl *lpReqQrySettlementInfo) ( HKSTRD hKStrd, KS_::CThostFtdcQrySettlementInfoField *pQrySettlementInfo, int nRequestID );
typedef int  (__cdecl *lpReqQryTransferBank) ( HKSTRD hKStrd, KS_::CThostFtdcQryTransferBankField *pQryTransferBank, int nRequestID );
typedef int  (__cdecl *lpReqQryInvestorPositionDetail) ( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, int nRequestID );
typedef int  (__cdecl *lpReqQryNotice) ( HKSTRD hKStrd, KS_::CThostFtdcQryNoticeField *pQryNotice, int nRequestID );
typedef int  (__cdecl *lpReqQrySettlementInfoConfirm) ( HKSTRD hKStrd, KS_::CThostFtdcQrySettlementInfoConfirmField *pQrySettlementInfoConfirm, int nRequestID );
typedef int  (__cdecl *lpReqQryInvestorPositionCombineDetail) ( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorPositionCombineDetail, int nRequestID );
typedef int  (__cdecl *lpReqQryCFMMCTradingAccountKey) ( HKSTRD hKStrd, KS_::CThostFtdcQryCFMMCTradingAccountKeyField *pQryCFMMCTradingAccountKey, int nRequestID );
typedef int  (__cdecl *lpReqQryEWarrantOffset) ( HKSTRD hKStrd, KS_::CThostFtdcQryEWarrantOffsetField *pQryEWarrantOffset, int nRequestID );
typedef int  (__cdecl *lpReqQryInvestorProductGroupMargin) ( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorProductGroupMarginField *pQryInvestorProductGroupMargin, int nRequestID );
typedef int  (__cdecl *lpReqQryExchangeMarginRate) ( HKSTRD hKStrd, KS_::CThostFtdcQryExchangeMarginRateField *pQryExchangeMarginRate, int nRequestID);
typedef int  (__cdecl *lpReqQryExchangeMarginRateAdjust) ( HKSTRD hKStrd, KS_::CThostFtdcQryExchangeMarginRateAdjustField *pQryExchangeMarginRateAdjust, int nRequestID );
typedef int  (__cdecl *lpReqQryExchangeRate) ( HKSTRD hKStrd, KS_::CThostFtdcQryExchangeRateField *pQryExchangeRate, int nRequestID );
typedef int  (__cdecl *lpReqQrySecAgentACIDMap) ( HKSTRD hKStrd, KS_::CThostFtdcQrySecAgentACIDMapField *pQrySecAgentACIDMap, int nRequestID );
typedef int  (__cdecl *lpReqQryOptionInstrTradeCost) ( HKSTRD hKStrd, KS_::CThostFtdcQryOptionInstrTradeCostField *pQryOptionInstrTradeCost, int nRequestID );
typedef int  (__cdecl *lpReqQryOptionInstrCommRate) ( HKSTRD hKStrd, KS_::CThostFtdcQryOptionInstrCommRateField *pQryOptionInstrCommRate, int nRequestID );
typedef int  (__cdecl *lpReqQryExecOrder) ( HKSTRD hKStrd, KS_::CThostFtdcQryExecOrderField *pQryExecOrder, int nRequestID );
typedef int  (__cdecl *lpReqQryForQuote) ( HKSTRD hKStrd, KS_::CThostFtdcQryForQuoteField *pQryForQuote, int nRequestID );
typedef int  (__cdecl *lpReqQryQuote) ( HKSTRD hKStrd, KS_::CThostFtdcQryQuoteField *pQryQuote, int nRequestID );
typedef int  (__cdecl *lpReqQryTransferSerial) ( HKSTRD hKStrd, KS_::CThostFtdcQryTransferSerialField *pQryTransferSerial, int nRequestID );
typedef int  (__cdecl *lpReqQryAccountregister) ( HKSTRD hKStrd, KS_::CThostFtdcQryAccountregisterField *pQryAccountregister, int nRequestID );
typedef int  (__cdecl *lpReqQryContractBank) ( HKSTRD hKStrd, KS_::CThostFtdcQryContractBankField *pQryContractBank, int nRequestID );
typedef int  (__cdecl *lpReqQryParkedOrder) ( HKSTRD hKStrd, KS_::CThostFtdcQryParkedOrderField *pQryParkedOrder, int nRequestID );
typedef int  (__cdecl *lpReqQryParkedOrderAction) ( HKSTRD hKStrd, KS_::CThostFtdcQryParkedOrderActionField *pQryParkedOrderAction, int nRequestID );
typedef int  (__cdecl *lpReqQryTradingNotice) ( HKSTRD hKStrd, KS_::CThostFtdcQryTradingNoticeField *pQryTradingNotice, int nRequestID );
typedef int  (__cdecl *lpReqQryBrokerTradingParams) ( HKSTRD hKStrd, KS_::CThostFtdcQryBrokerTradingParamsField *pQryBrokerTradingParams, int nRequestID );
typedef int  (__cdecl *lpReqQryBrokerTradingAlgos) ( HKSTRD hKStrd, KS_::CThostFtdcQryBrokerTradingAlgosField *pQryBrokerTradingAlgos, int nRequestID );
typedef int  (__cdecl *lpReqQueryCFMMCTradingAccountToken) ( HKSTRD hKStrd, KS_::CThostFtdcQueryCFMMCTradingAccountTokenField *pQueryCFMMCTradingAccountToken, int nRequestID );
typedef int  (__cdecl *lpReqFromBankToFutureByFuture) ( HKSTRD hKStrd, KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID );
typedef int  (__cdecl *lpReqFromFutureToBankByFuture) ( HKSTRD hKStrd, KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID );
typedef int  (__cdecl *lpReqQueryBankAccountMoneyByFuture) ( HKSTRD hKStrd, KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, int nRequestID );
// v2.3.50212 // 20150410 by Tim Lin
typedef int  (__cdecl *lpReqLockInsert) ( HKSTRD hKStrd, KS_::CThostFtdcInputLockField *pInputLock, int nRequestID );///用戶端發出鎖定請求
typedef int  (__cdecl *lpReqQryLock) ( HKSTRD hKStrd, KS_::CThostFtdcQryLockField *pQryLock, int nRequestID );///用戶端發出查詢鎖定請求
typedef int  (__cdecl *lpReqQryLockPosition) ( HKSTRD hKStrd, KS_::CThostFtdcQryLockPositionField *pQryLockPosition, int nRequestID );///用戶端發出查詢鎖定證券倉位請求
// ---- CKSOptionApi ----
// v2.3.50212 // 20150410 by Tim Lin
typedef int  (__cdecl *lpReqQryIndexPrice) ( HKSTRD hKStrd, KS_::CKSQryIndexPriceField *pQryIndexPrice, int nRequestID );///查詢股指現貨指數
typedef int  (__cdecl *lpReqQryOptionInstrGuard) ( HKSTRD hKStrd, KS_::CKSQryOptionInstrGuardField *pQryOptionInstrGuard, int nRequestID );///查詢期權合約保障系數
typedef int  (__cdecl *lpReqQryExecOrderVolume) ( HKSTRD hKStrd, KS_::CKSQryExecOrderVolumeField *pQryExecOrderVolume, int nRequestID );///查詢宣告數量請求
typedef int  (__cdecl *lpReqQryStockOptionAssignment) ( HKSTRD hKStrd, KS_::CKSQryStockOptionAssignmentField *pQryStockOptionAssignment, int nRequestID );///查詢個股行權指派信息
typedef int  (__cdecl *lpReqQryInvestorTradeLevel) ( HKSTRD hKStrd, KS_::CKSQryInvestorTradeLevelField *pQryInvestorTradeLevel, int nRequestID );///查詢客戶交易級別
typedef int  (__cdecl *lpReqQryPurchaseLimitAmt) ( HKSTRD hKStrd, KS_::CKSQryPurchaseLimitAmtField *pQryPurchaseLimitAmt, int nRequestID );///查詢個股限購額度
typedef int  (__cdecl *lpReqQryPositionLimitVol) ( HKSTRD hKStrd, KS_::CKSQryPositionLimitVolField *pQryPositionLimitVol, int nRequestID );///查詢個股限倉額度
typedef int  (__cdecl *lpReqQryHistoryOrder) ( HKSTRD hKStrd, KS_::CKSQryHistoryOrderField *pQryHistoryOrder, int nRequestID );///請求查詢個股歷史報單
typedef int  (__cdecl *lpReqQryHistoryTrade) ( HKSTRD hKStrd, KS_::CKSQryHistoryTradeField *pQryHistoryTrade, int nRequestID );///請求查詢個股歷史成交
typedef int  (__cdecl *lpReqQryStockOptionHistoryAssignment) ( HKSTRD hKStrd, KS_::CKSQryHistoryAssignmentField *pQryHistoryAssignment, int nRequestID );///請求查詢個股歷史行權指派明細
typedef int  (__cdecl *lpReqQryStockOptionDelivDetail) ( HKSTRD hKStrd, KS_::CKSQrySODelivDetailField *pQrySODelivDetail, int nRequestID );///請求查詢個股行權交割明細
typedef int  (__cdecl *lpReqAutoExecOrderAction) ( HKSTRD hKStrd, KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, int nRequestID );///自動行權執行操作
// ---- CKSVocApi ----
// v2.3.50212 // 20150410 by Tim Lin
typedef int  (__cdecl *lpReqQueryInvestorOpenPosition) ( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorOpenPosition, int nRequestID );///查詢開盤前的持倉明細
typedef int  (__cdecl *lpReqQueryInvestorOpenCombinePosition) ( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorOpenCombinePosition, int nRequestID );///查詢開盤前的組合持倉明細
typedef int  (__cdecl *lpReqBulkCancelOrder) ( HKSTRD hKStrd, KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, int nRequestID );///批量撤單
typedef int  (__cdecl *lpReqQryCloseStrategy) ( HKSTRD hKStrd, KS_::CKSCloseStrategy *pCloseStrategy, int nRequestID );///平倉策略查詢請求
typedef int  (__cdecl *lpReqQryCombStrategy) ( HKSTRD hKStrd, KS_::CKSCombStrategy *pCombStrategy, int nRequestID );///組合策略查詢請求
typedef int  (__cdecl *lpReqQryOptionCombStrategy) ( HKSTRD hKStrd, KS_::CKSOptionCombStrategy *pOptionCombStrategy, int nRequestID );///期權組合策略查詢請求
typedef int  (__cdecl *lpReqQryTransferInfo) ( HKSTRD hKStrd, KS_::CKSTransferInfo *pTransferInfo, int nRequestID );///請求查詢客戶轉帳資訊
typedef int  (__cdecl *lpReqQryKSTradingNotice) ( HKSTRD hKStrd, KS_::CKSQryTradingNoticeField *pQryTradingNotice, int nRequestID );///請求查詢交易通知
typedef int  (__cdecl *lpReqQryUserProductUrl) ( HKSTRD hKStrd, KS_::CKSQryUserProductUrlField *pQryUserProductUrl, int nRequestID );///使用者端產品資源查詢請求
typedef int  (__cdecl *lpReqQryMaxCombActionVolume) ( HKSTRD hKStrd, KS_::CKSQryMaxCombActionVolumeField *pQryMaxCombActionVolume, int nRequestID );///最大組合拆分單量查詢請求
typedef int  (__cdecl *lpReqQryKSInstrumentMarginRate) ( HKSTRD hKStrd, KS_::CKSQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID );///請求查詢合約保證金率
//------------------------------------------------------------------------------------------------------------------------
// Define all RegOnXXXXXX register callback function functions.
//------------------------------------------------------------------------------------------------------------------------
// ---- CThostFtdcTraderSpi ----
typedef void (__cdecl *lpRegOnFrontConnected)							( HKSTRD hKStrd, pfOnFrontConnected callback );
typedef void (__cdecl *lpRegOnFrontDisconnected)						( HKSTRD hKStrd, pfOnFrontDisconnected callback );
typedef void (__cdecl *lpRegOnHeartBeatWarning)							( HKSTRD hKStrd, pfOnHeartBeatWarning callback );
typedef void (__cdecl *lpRegOnRspAuthenticate)							( HKSTRD hKStrd, pfOnRspAuthenticate callback );
typedef void (__cdecl *lpRegOnRspUserLogin)								( HKSTRD hKStrd, pfOnRspUserLogin callback );
typedef void (__cdecl *lpRegOnRspUserLogout)							( HKSTRD hKStrd, pfOnRspUserLogout callback );
typedef void (__cdecl *lpRegOnRspUserPasswordUpdate)					( HKSTRD hKStrd, pfOnRspUserPasswordUpdate callback );
typedef void (__cdecl *lpRegOnRspTradingAccountPasswordUpdate)			( HKSTRD hKStrd, pfOnRspTradingAccountPasswordUpdate callback );
typedef void (__cdecl *lpRegOnRspOrderInsert)							( HKSTRD hKStrd, pfOnRspOrderInsert callback );
typedef void (__cdecl *lpRegOnRspParkedOrderInsert)						( HKSTRD hKStrd, pfOnRspParkedOrderInsert callback );
typedef void (__cdecl *lpRegOnRspParkedOrderAction)						( HKSTRD hKStrd, pfOnRspParkedOrderAction callback );
typedef void (__cdecl *lpRegOnRspOrderAction)							( HKSTRD hKStrd, pfOnRspOrderAction callback );
typedef void (__cdecl *lpRegOnRspQueryMaxOrderVolume)					( HKSTRD hKStrd, pfOnRspQueryMaxOrderVolume callback );
typedef void (__cdecl *lpRegOnRspSettlementInfoConfirm)					( HKSTRD hKStrd, pfOnRspSettlementInfoConfirm callback );
typedef void (__cdecl *lpRegOnRspRemoveParkedOrder)						( HKSTRD hKStrd, pfOnRspRemoveParkedOrder callback );
typedef void (__cdecl *lpRegOnRspRemoveParkedOrderAction)				( HKSTRD hKStrd, pfOnRspRemoveParkedOrderAction callback );
typedef void (__cdecl *lpRegOnRspExecOrderInsert)						( HKSTRD hKStrd, pfOnRspExecOrderInsert callback );
typedef void (__cdecl *lpRegOnRspExecOrderAction)						( HKSTRD hKStrd, pfOnRspExecOrderAction callback );
typedef void (__cdecl *lpRegOnRspForQuoteInsert)						( HKSTRD hKStrd, pfOnRspForQuoteInsert callback );
typedef void (__cdecl *lpRegOnRspQuoteInsert)							( HKSTRD hKStrd, pfOnRspQuoteInsert callback );
typedef void (__cdecl *lpRegOnRspQuoteAction)							( HKSTRD hKStrd, pfOnRspQuoteAction callback );
typedef void (__cdecl *lpRegOnRspQryOrder)								( HKSTRD hKStrd, pfOnRspQryOrder callback );
typedef void (__cdecl *lpRegOnRspQryTrade)								( HKSTRD hKStrd, pfOnRspQryTrade callback );
typedef void (__cdecl *lpRegOnRspQryInvestorPosition)					( HKSTRD hKStrd, pfOnRspQryInvestorPosition callback );
typedef void (__cdecl *lpRegOnRspQryTradingAccount)						( HKSTRD hKStrd, pfOnRspQryTradingAccount callback );
typedef void (__cdecl *lpRegOnRspQryInvestor)							( HKSTRD hKStrd, pfOnRspQryInvestor callback );
typedef void (__cdecl *lpRegOnRspQryTradingCode)						( HKSTRD hKStrd, pfOnRspQryTradingCode callback );
typedef void (__cdecl *lpRegOnRspQryInstrumentMarginRate)				( HKSTRD hKStrd, pfOnRspQryInstrumentMarginRate callback );
typedef void (__cdecl *lpRegOnRspQryInstrumentCommissionRate)			( HKSTRD hKStrd, pfOnRspQryInstrumentCommissionRate callback );
typedef void (__cdecl *lpRegOnRspQryExchange)							( HKSTRD hKStrd, pfOnRspQryExchange callback );
typedef void (__cdecl *lpRegOnRspQryProduct)							( HKSTRD hKStrd, pfOnRspQryProduct callback );
typedef void (__cdecl *lpRegOnRspQryInstrument)							( HKSTRD hKStrd, pfOnRspQryInstrument callback );
typedef void (__cdecl *lpRegOnRspQryDepthMarketData)					( HKSTRD hKStrd, pfOnRspQryDepthMarketData callback );
typedef void (__cdecl *lpRegOnRspQrySettlementInfo)						( HKSTRD hKStrd, pfOnRspQrySettlementInfo callback );
typedef void (__cdecl *lpRegOnRspQryTransferBank)						( HKSTRD hKStrd, pfOnRspQryTransferBank callback );
typedef void (__cdecl *lpRegOnRspQryInvestorPositionDetail)				( HKSTRD hKStrd, pfOnRspQryInvestorPositionDetail callback );
typedef void (__cdecl *lpRegOnRspQryNotice)								( HKSTRD hKStrd, pfOnRspQryNotice callback );
typedef void (__cdecl *lpRegOnRspQrySettlementInfoConfirm)				( HKSTRD hKStrd, pfOnRspQrySettlementInfoConfirm callback );
typedef void (__cdecl *lpRegOnRspQryInvestorPositionCombineDetail)		( HKSTRD hKStrd, pfOnRspQryInvestorPositionCombineDetail callback );
typedef void (__cdecl *lpRegOnRspQryCFMMCTradingAccountKey)				( HKSTRD hKStrd, pfOnRspQryCFMMCTradingAccountKey callback );
typedef void (__cdecl *lpRegOnRspQryEWarrantOffset)						( HKSTRD hKStrd, pfOnRspQryEWarrantOffset callback );
typedef void (__cdecl *lpRegOnRspQryInvestorProductGroupMargin)			( HKSTRD hKStrd, pfOnRspQryInvestorProductGroupMargin callback );
typedef void (__cdecl *lpRegOnRspQryExchangeMarginRate)					( HKSTRD hKStrd, pfOnRspQryExchangeMarginRate callback );
typedef void (__cdecl *lpRegOnRspQryExchangeMarginRateAdjust)			( HKSTRD hKStrd, pfOnRspQryExchangeMarginRateAdjust callback );
typedef void (__cdecl *lpRegOnRspQryExchangeRate)						( HKSTRD hKStrd, pfOnRspQryExchangeRate callback );
typedef void (__cdecl *lpRegOnRspQrySecAgentACIDMap)					( HKSTRD hKStrd, pfOnRspQrySecAgentACIDMap callback );
typedef void (__cdecl *lpRegOnRspQryOptionInstrTradeCost)				( HKSTRD hKStrd, pfOnRspQryOptionInstrTradeCost callback );
typedef void (__cdecl *lpRegOnRspQryOptionInstrCommRate)				( HKSTRD hKStrd, pfOnRspQryOptionInstrCommRate callback );
typedef void (__cdecl *lpRegOnRspQryExecOrder)							( HKSTRD hKStrd, pfOnRspQryExecOrder callback );
typedef void (__cdecl *lpRegOnRspQryForQuote)							( HKSTRD hKStrd, pfOnRspQryForQuote callback );
typedef void (__cdecl *lpRegOnRspQryQuote)								( HKSTRD hKStrd, pfOnRspQryQuote callback );
typedef void (__cdecl *lpRegOnRspQryTransferSerial)						( HKSTRD hKStrd, pfOnRspQryTransferSerial callback );
typedef void (__cdecl *lpRegOnRspQryAccountregister)					( HKSTRD hKStrd, pfOnRspQryAccountregister callback );
typedef void (__cdecl *lpRegOnRspError)									( HKSTRD hKStrd, pfOnRspError callback );
typedef void (__cdecl *lpRegOnRtnOrder)									( HKSTRD hKStrd, pfOnRtnOrder callback );
typedef void (__cdecl *lpRegOnRtnTrade)									( HKSTRD hKStrd, pfOnRtnTrade callback );
typedef void (__cdecl *lpRegOnErrRtnOrderInsert)						( HKSTRD hKStrd, pfOnErrRtnOrderInsert callback );
typedef void (__cdecl *lpRegOnErrRtnOrderAction)						( HKSTRD hKStrd, pfOnErrRtnOrderAction callback );
typedef void (__cdecl *lpRegOnRtnInstrumentStatus)						( HKSTRD hKStrd, pfOnRtnInstrumentStatus callback );
typedef void (__cdecl *lpRegOnRtnTradingNotice)							( HKSTRD hKStrd, pfOnRtnTradingNotice callback );
typedef void (__cdecl *lpRegOnRtnErrorConditionalOrder)					( HKSTRD hKStrd, pfOnRtnErrorConditionalOrder callback );
typedef void (__cdecl *lpRegOnRtnExecOrder)								( HKSTRD hKStrd, pfOnRtnExecOrder callback );
typedef void (__cdecl *lpRegOnErrRtnExecOrderInsert)					( HKSTRD hKStrd, pfOnErrRtnExecOrderInsert callback );
typedef void (__cdecl *lpRegOnErrRtnExecOrderAction)					( HKSTRD hKStrd, pfOnErrRtnExecOrderAction callback );
typedef void (__cdecl *lpRegOnErrRtnForQuoteInsert)						( HKSTRD hKStrd, pfOnErrRtnForQuoteInsert callback );
typedef void (__cdecl *lpRegOnRtnQuote)									( HKSTRD hKStrd, pfOnRtnQuote callback );
typedef void (__cdecl *lpRegOnErrRtnQuoteInsert)						( HKSTRD hKStrd, pfOnErrRtnQuoteInsert callback );
typedef void (__cdecl *lpRegOnErrRtnQuoteAction)						( HKSTRD hKStrd, pfOnErrRtnQuoteAction callback );
typedef void (__cdecl *lpRegOnRtnForQuoteRsp)							( HKSTRD hKStrd, pfOnRtnForQuoteRsp callback );
typedef void (__cdecl *lpRegOnRtnCFMMCTradingAccountToken)				( HKSTRD hKStrd, pfOnRtnCFMMCTradingAccountToken callback );
typedef void (__cdecl *lpRegOnRspQryContractBank)						( HKSTRD hKStrd, pfOnRspQryContractBank callback );
typedef void (__cdecl *lpRegOnRspQryParkedOrder)						( HKSTRD hKStrd, pfOnRspQryParkedOrder callback );
typedef void (__cdecl *lpRegOnRspQryParkedOrderAction)					( HKSTRD hKStrd, pfOnRspQryParkedOrderAction callback );
typedef void (__cdecl *lpRegOnRspQryTradingNotice)						( HKSTRD hKStrd, pfOnRspQryTradingNotice callback );
typedef void (__cdecl *lpRegOnRspQryBrokerTradingParams)				( HKSTRD hKStrd, pfOnRspQryBrokerTradingParams callback );
typedef void (__cdecl *lpRegOnRspQryBrokerTradingAlgos)					( HKSTRD hKStrd, pfOnRspQryBrokerTradingAlgos callback );
typedef void (__cdecl *lpRegOnRspQueryCFMMCTradingAccountToken)			( HKSTRD hKStrd, pfOnRspQueryCFMMCTradingAccountToken callback );
typedef void (__cdecl *lpRegOnRtnFromBankToFutureByBank)				( HKSTRD hKStrd, pfOnRtnFromBankToFutureByBank callback );
typedef void (__cdecl *lpRegOnRtnFromFutureToBankByBank)				( HKSTRD hKStrd, pfOnRtnFromFutureToBankByBank callback );
typedef void (__cdecl *lpRegOnRtnRepealFromBankToFutureByBank)			( HKSTRD hKStrd, pfOnRtnRepealFromBankToFutureByBank callback );
typedef void (__cdecl *lpRegOnRtnRepealFromFutureToBankByBank)			( HKSTRD hKStrd, pfOnRtnRepealFromFutureToBankByBank callback );
typedef void (__cdecl *lpRegOnRtnFromBankToFutureByFuture)				( HKSTRD hKStrd, pfOnRtnFromBankToFutureByFuture callback );
typedef void (__cdecl *lpRegOnRtnFromFutureToBankByFuture)				( HKSTRD hKStrd, pfOnRtnFromFutureToBankByFuture callback );
typedef void (__cdecl *lpRegOnRtnRepealFromBankToFutureByFutureManual)	( HKSTRD hKStrd, pfOnRtnRepealFromBankToFutureByFutureManual callback );
typedef void (__cdecl *lpRegOnRtnRepealFromFutureToBankByFutureManual)	( HKSTRD hKStrd, pfOnRtnRepealFromFutureToBankByFutureManual callback );
typedef void (__cdecl *lpRegOnRtnQueryBankBalanceByFuture)				( HKSTRD hKStrd, pfOnRtnQueryBankBalanceByFuture callback );
typedef void (__cdecl *lpRegOnErrRtnBankToFutureByFuture)				( HKSTRD hKStrd, pfOnErrRtnBankToFutureByFuture callback );
typedef void (__cdecl *lpRegOnErrRtnFutureToBankByFuture)				( HKSTRD hKStrd, pfOnErrRtnFutureToBankByFuture callback );
typedef void (__cdecl *lpRegOnErrRtnRepealBankToFutureByFutureManual)	( HKSTRD hKStrd, pfOnErrRtnRepealBankToFutureByFutureManual callback );
typedef void (__cdecl *lpRegOnErrRtnRepealFutureToBankByFutureManual)	( HKSTRD hKStrd, pfOnErrRtnRepealFutureToBankByFutureManual callback );
typedef void (__cdecl *lpRegOnErrRtnQueryBankBalanceByFuture)			( HKSTRD hKStrd, pfOnErrRtnQueryBankBalanceByFuture callback );
typedef void (__cdecl *lpRegOnRtnRepealFromBankToFutureByFuture)		( HKSTRD hKStrd, pfOnRtnRepealFromBankToFutureByFuture callback );
typedef void (__cdecl *lpRegOnRtnRepealFromFutureToBankByFuture)		( HKSTRD hKStrd, pfOnRtnRepealFromFutureToBankByFuture callback );
typedef void (__cdecl *lpRegOnRspFromBankToFutureByFuture)				( HKSTRD hKStrd, pfOnRspFromBankToFutureByFuture callback );
typedef void (__cdecl *lpRegOnRspFromFutureToBankByFuture)				( HKSTRD hKStrd, pfOnRspFromFutureToBankByFuture callback );
typedef void (__cdecl *lpRegOnRspQueryBankAccountMoneyByFuture)			( HKSTRD hKStrd, pfOnRspQueryBankAccountMoneyByFuture callback );
typedef void (__cdecl *lpRegOnRtnOpenAccountByBank)						( HKSTRD hKStrd, pfOnRtnOpenAccountByBank callback );
typedef void (__cdecl *lpRegOnRtnCancelAccountByBank)					( HKSTRD hKStrd, pfOnRtnCancelAccountByBank callback );
typedef void (__cdecl *lpRegOnRtnChangeAccountByBank)					( HKSTRD hKStrd, pfOnRtnChangeAccountByBank callback );
// v2.3.50212 // 20150410 by Tim Lin // <Step>.2
typedef void (__cdecl *lpRegOnRspLockInsert)							( HKSTRD hKStrd, pfOnRspLockInsert callback );///鎖定應答
typedef void (__cdecl *lpRegOnRspQryLock)								( HKSTRD hKStrd, pfOnRspQryLock callback );///請求查詢鎖定回應	
typedef void (__cdecl *lpRegOnRspQryLockPosition)						( HKSTRD hKStrd, pfOnRspQryLockPosition callback );///請求查詢鎖定證券倉位回應
typedef void (__cdecl *lpRegOnRtnLock)									( HKSTRD hKStrd, pfOnRtnLock callback );///鎖定通知
typedef void (__cdecl *lpRegOnErrRtnLockInsert)							( HKSTRD hKStrd, pfOnErrRtnLockInsert callback );///鎖定錯誤通知	
// ---- CKSOptionSpi ----
// v2.3.50212 // 20150410 by Tim Lin // Step.2
typedef void (__cdecl *lpRegOnRspQryIndexPrice)							( HKSTRD hKStrd, pfOnRspQryIndexPrice callback );///查詢股指現貨指數
typedef void (__cdecl *lpRegOnRspQryOptionInstrGuard)					( HKSTRD hKStrd, pfOnRspQryOptionInstrGuard callback );///查詢期權合約保障系數
typedef void (__cdecl *lpRegOnRspQryExecOrderVolume)					( HKSTRD hKStrd, pfOnRspQryExecOrderVolume callback );///查詢宣告數量請求
typedef void (__cdecl *lpRegOnRspQryStockOptionAssignment)				( HKSTRD hKStrd, pfOnRspQryStockOptionAssignment callback );///查詢個股行權指派信息
typedef void (__cdecl *lpRegOnRspQryInvestorTradeLevel)					( HKSTRD hKStrd, pfOnRspQryInvestorTradeLevel callback );///查詢客戶交易級別
typedef void (__cdecl *lpRegOnRspQryPurchaseLimitAmt)					( HKSTRD hKStrd, pfOnRspQryPurchaseLimitAmt callback );///查詢個股限購額度
typedef void (__cdecl *lpRegOnRspQryPositionLimitVol)					( HKSTRD hKStrd, pfOnRspQryPositionLimitVol callback );///查詢個股限倉額度
typedef void (__cdecl *lpRegOnRspQryHistoryOrder)						( HKSTRD hKStrd, pfOnRspQryHistoryOrder callback );///請求查詢個股歷史報單
typedef void (__cdecl *lpRegOnRspQryHistoryTrade)						( HKSTRD hKStrd, pfOnRspQryHistoryTrade callback );///請求查詢個股歷史成交
typedef void (__cdecl *lpRegOnRspQryStockOptionHistoryAssignment)		( HKSTRD hKStrd, pfOnRspQryStockOptionHistoryAssignment callback );///請求查詢個股歷史行權指派明細
typedef void (__cdecl *lpRegOnRspQryStockOptionDelivDetail)				( HKSTRD hKStrd, pfOnRspQryStockOptionDelivDetail callback );///請求查詢個股行權交割明細
typedef void (__cdecl *lpRegOnRspAutoExecOrderAction)					( HKSTRD hKStrd, pfOnRspAutoExecOrderAction callback );///自動行權執行操作
// ---- CKSVocSpi ----
// v2.3.50212 // 20150410 by Tim Lin // [Step].2
typedef void (__cdecl *lpRegOnRspQryInvestorOpenPosition)				( HKSTRD hKStrd, pfOnRspQryInvestorOpenPosition callback );///查詢開盤前的持倉明細應答
typedef void (__cdecl *lpRegOnRspQryInvestorOpenCombinePosition)		( HKSTRD hKStrd, pfOnRspQryInvestorOpenCombinePosition callback );///查詢開盤前的組合持倉明細應答
typedef void (__cdecl *lpRegOnRspBulkCancelOrder)						( HKSTRD hKStrd, pfOnRspBulkCancelOrder callback );///批量報單撤除請求回應
typedef void (__cdecl *lpRegOnRspQryCloseStrategy)						( HKSTRD hKStrd, pfOnRspQryCloseStrategy callback );///平倉策略查詢回應
typedef void (__cdecl *lpRegOnRspQryCombStrategy)						( HKSTRD hKStrd, pfOnRspQryCombStrategy callback );///組合策略查詢回應
typedef void (__cdecl *lpRegOnRspQryOptionCombStrategy)					( HKSTRD hKStrd, pfOnRspQryOptionCombStrategy callback );///期權組合策略查詢回應
typedef void (__cdecl *lpRegOnRspQryTransferInfo)						( HKSTRD hKStrd, pfOnRspQryTransferInfo callback );///請求查詢客戶轉帳資訊回應
typedef void (__cdecl *lpRegOnRspQryKSTradingNotice)					( HKSTRD hKStrd, pfOnRspQryKSTradingNotice callback );///請求查詢交易通知回應
typedef void (__cdecl *lpRegOnRspQryUserProductUrl)						( HKSTRD hKStrd, pfOnRspQryUserProductUrl callback );///使用者端產品資源查詢應答
typedef void (__cdecl *lpRegOnRspQryMaxCombActionVolume)				( HKSTRD hKStrd, pfOnRspQryMaxCombActionVolume callback );///最大組合拆分單量查詢請求回應
typedef void (__cdecl *lpRegOnRtnKSTradingNotice)						( HKSTRD hKStrd, pfOnRtnKSTradingNotice callback );///交易通知
typedef void (__cdecl *lpRegOnRspQryKSInstrumentMarginRate)				( HKSTRD hKStrd, pfOnRspQryKSInstrumentMarginRate callback );///請求查詢合約保證金率
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------

