//------------------------------------------------------------------------------------------------------------------------
#ifndef __KStrdAPIFactory_H
#define __KStrdAPIFactory_H
//------------------------------------------------------------------------------------------------------------------------
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the KSTRDAPIFACTORY_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// KSTRDAPIFACTORY_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
//------------------------------------------------------------------------------------------------------------------------
#ifdef KSTRDAPIFACTORY_EXPORTS
#define KSTRDAPIFACTORY_API __declspec(dllexport)
#else
#define KSTRDAPIFACTORY_API __declspec(dllimport)
#endif
//------------------------------------------------------------------------------------------------------------------------
#include "KStrdAPIDynamic.h"
//------------------------------------------------------------------------------------------------------------------------
extern "C"
{
//------------------------------------------------------------------------------------------------------------------------
// Export CThostFtdcTraderApi, CKSVocApi, CKSOptionApi methods into C style.
//------------------------------------------------------------------------------------------------------------------------
// ---- CThostFtdcTraderApi ----
	KSTRDAPIFACTORY_API HKSTRD CreateKStrdTraderApi( void* Owner );
	KSTRDAPIFACTORY_API void ReleaseKStrdTraderApi( HKSTRD hKStrd );
	KSTRDAPIFACTORY_API void Init( HKSTRD hKStrd );
	KSTRDAPIFACTORY_API const char *GetTradingDay( HKSTRD hKStrd );
	KSTRDAPIFACTORY_API void RegisterFront( HKSTRD hKStrd, char *pszFrontAddress );
	KSTRDAPIFACTORY_API void RegisterNameServer( HKSTRD hKStrd, char *pszNsAddress );
	KSTRDAPIFACTORY_API void RegisterFensUserInfo( HKSTRD hKStrd, KS_::CThostFtdcFensUserInfoField * pFensUserInfo );
//	KSTRDAPIFACTORY_API void RegisterSpi( HKSTRD hKStrd, KS_::CThostFtdcTraderSpi *pSpi );
	KSTRDAPIFACTORY_API void SubscribePrivateTopic( HKSTRD hKStrd, KS_::THOST_TE_RESUME_TYPE nResumeType );
	KSTRDAPIFACTORY_API void SubscribePublicTopic( HKSTRD hKStrd, KS_::THOST_TE_RESUME_TYPE nResumeType );
	KSTRDAPIFACTORY_API int ReqAuthenticate( HKSTRD hKStrd, KS_::CThostFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID );
	KSTRDAPIFACTORY_API int ReqUserLogin( HKSTRD hKStrd, KS_::CThostFtdcReqUserLoginField *pReqUserLoginField, int nRequestID );
	KSTRDAPIFACTORY_API int ReqUserLogout( HKSTRD hKStrd, KS_::CThostFtdcUserLogoutField *pUserLogout, int nRequestID );
	KSTRDAPIFACTORY_API int ReqUserPasswordUpdate( HKSTRD hKStrd, KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID );
	KSTRDAPIFACTORY_API int ReqTradingAccountPasswordUpdate( HKSTRD hKStrd, KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, int nRequestID );
	KSTRDAPIFACTORY_API int ReqOrderInsert( HKSTRD hKStrd, KS_::CThostFtdcInputOrderField *pInputOrder, int nRequestID );
	KSTRDAPIFACTORY_API int ReqParkedOrderInsert( HKSTRD hKStrd, KS_::CThostFtdcParkedOrderField *pParkedOrder, int nRequestID );
	KSTRDAPIFACTORY_API int ReqParkedOrderAction( HKSTRD hKStrd, KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, int nRequestID );
	KSTRDAPIFACTORY_API int ReqOrderAction( HKSTRD hKStrd, KS_::CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQueryMaxOrderVolume( HKSTRD hKStrd, KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, int nRequestID );
	KSTRDAPIFACTORY_API int ReqSettlementInfoConfirm( HKSTRD hKStrd, KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, int nRequestID );
	KSTRDAPIFACTORY_API int ReqRemoveParkedOrder( HKSTRD hKStrd, KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, int nRequestID );
	KSTRDAPIFACTORY_API int ReqRemoveParkedOrderAction( HKSTRD hKStrd, KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, int nRequestID );
	KSTRDAPIFACTORY_API int ReqExecOrderInsert( HKSTRD hKStrd, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, int nRequestID );
	KSTRDAPIFACTORY_API int ReqExecOrderAction( HKSTRD hKStrd, KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, int nRequestID );
	KSTRDAPIFACTORY_API int ReqForQuoteInsert( HKSTRD hKStrd, KS_::CThostFtdcInputForQuoteField *pInputForQuote, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQuoteInsert( HKSTRD hKStrd, KS_::CThostFtdcInputQuoteField *pInputQuote, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQuoteAction( HKSTRD hKStrd, KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryOrder( HKSTRD hKStrd, KS_::CThostFtdcQryOrderField *pQryOrder, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryTrade( HKSTRD hKStrd, KS_::CThostFtdcQryTradeField *pQryTrade, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryInvestorPosition( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryTradingAccount( HKSTRD hKStrd, KS_::CThostFtdcQryTradingAccountField *pQryTradingAccount, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryInvestor( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorField *pQryInvestor, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryTradingCode( HKSTRD hKStrd, KS_::CThostFtdcQryTradingCodeField *pQryTradingCode, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryInstrumentMarginRate( HKSTRD hKStrd, KS_::CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryInstrumentCommissionRate( HKSTRD hKStrd, KS_::CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryExchange( HKSTRD hKStrd, KS_::CThostFtdcQryExchangeField *pQryExchange, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryProduct( HKSTRD hKStrd, KS_::CThostFtdcQryProductField *pQryProduct, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryInstrument( HKSTRD hKStrd, KS_::CThostFtdcQryInstrumentField *pQryInstrument, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryDepthMarketData( HKSTRD hKStrd, KS_::CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQrySettlementInfo( HKSTRD hKStrd, KS_::CThostFtdcQrySettlementInfoField *pQrySettlementInfo, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryTransferBank( HKSTRD hKStrd, KS_::CThostFtdcQryTransferBankField *pQryTransferBank, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryInvestorPositionDetail( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryNotice( HKSTRD hKStrd, KS_::CThostFtdcQryNoticeField *pQryNotice, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQrySettlementInfoConfirm( HKSTRD hKStrd, KS_::CThostFtdcQrySettlementInfoConfirmField *pQrySettlementInfoConfirm, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryInvestorPositionCombineDetail( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorPositionCombineDetail, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryCFMMCTradingAccountKey( HKSTRD hKStrd, KS_::CThostFtdcQryCFMMCTradingAccountKeyField *pQryCFMMCTradingAccountKey, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryEWarrantOffset( HKSTRD hKStrd, KS_::CThostFtdcQryEWarrantOffsetField *pQryEWarrantOffset, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryInvestorProductGroupMargin( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorProductGroupMarginField *pQryInvestorProductGroupMargin, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryExchangeMarginRate( HKSTRD hKStrd, KS_::CThostFtdcQryExchangeMarginRateField *pQryExchangeMarginRate, int nRequestID);
	KSTRDAPIFACTORY_API int ReqQryExchangeMarginRateAdjust( HKSTRD hKStrd, KS_::CThostFtdcQryExchangeMarginRateAdjustField *pQryExchangeMarginRateAdjust, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryExchangeRate( HKSTRD hKStrd, KS_::CThostFtdcQryExchangeRateField *pQryExchangeRate, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQrySecAgentACIDMap( HKSTRD hKStrd, KS_::CThostFtdcQrySecAgentACIDMapField *pQrySecAgentACIDMap, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryOptionInstrTradeCost( HKSTRD hKStrd, KS_::CThostFtdcQryOptionInstrTradeCostField *pQryOptionInstrTradeCost, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryOptionInstrCommRate( HKSTRD hKStrd, KS_::CThostFtdcQryOptionInstrCommRateField *pQryOptionInstrCommRate, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryExecOrder( HKSTRD hKStrd, KS_::CThostFtdcQryExecOrderField *pQryExecOrder, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryForQuote( HKSTRD hKStrd, KS_::CThostFtdcQryForQuoteField *pQryForQuote, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryQuote( HKSTRD hKStrd, KS_::CThostFtdcQryQuoteField *pQryQuote, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryTransferSerial( HKSTRD hKStrd, KS_::CThostFtdcQryTransferSerialField *pQryTransferSerial, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryAccountregister( HKSTRD hKStrd, KS_::CThostFtdcQryAccountregisterField *pQryAccountregister, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryContractBank( HKSTRD hKStrd, KS_::CThostFtdcQryContractBankField *pQryContractBank, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryParkedOrder( HKSTRD hKStrd, KS_::CThostFtdcQryParkedOrderField *pQryParkedOrder, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryParkedOrderAction( HKSTRD hKStrd, KS_::CThostFtdcQryParkedOrderActionField *pQryParkedOrderAction, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryTradingNotice( HKSTRD hKStrd, KS_::CThostFtdcQryTradingNoticeField *pQryTradingNotice, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryBrokerTradingParams( HKSTRD hKStrd, KS_::CThostFtdcQryBrokerTradingParamsField *pQryBrokerTradingParams, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQryBrokerTradingAlgos( HKSTRD hKStrd, KS_::CThostFtdcQryBrokerTradingAlgosField *pQryBrokerTradingAlgos, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQueryCFMMCTradingAccountToken( HKSTRD hKStrd, KS_::CThostFtdcQueryCFMMCTradingAccountTokenField *pQueryCFMMCTradingAccountToken, int nRequestID );
	KSTRDAPIFACTORY_API int ReqFromBankToFutureByFuture( HKSTRD hKStrd, KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID );
	KSTRDAPIFACTORY_API int ReqFromFutureToBankByFuture( HKSTRD hKStrd, KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID );
	KSTRDAPIFACTORY_API int ReqQueryBankAccountMoneyByFuture( HKSTRD hKStrd, KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, int nRequestID );
// v2.3.50212 // 20150410 by Tim Lin // <Step>.3
	KSTRDAPIFACTORY_API int ReqLockInsert( HKSTRD hKStrd, KS_::CThostFtdcInputLockField *pInputLock, int nRequestID);///用戶端發出鎖定請求		
	KSTRDAPIFACTORY_API int ReqQryLock( HKSTRD hKStrd, KS_::CThostFtdcQryLockField *pQryLock, int nRequestID);///用戶端發出查詢鎖定請求		
	KSTRDAPIFACTORY_API int ReqQryLockPosition( HKSTRD hKStrd, KS_::CThostFtdcQryLockPositionField *pQryLockPosition, int nRequestID);///用戶端發出查詢鎖定證券倉位請求
// ---- CKSOptionApi ----
// v2.3.50212 // 20150410 by Tim Lin // Step.3
	KSTRDAPIFACTORY_API int ReqQryIndexPrice( HKSTRD hKStrd, KS_::CKSQryIndexPriceField *pQryIndexPrice, int nRequestID );///查詢股指現貨指數
	KSTRDAPIFACTORY_API int ReqQryOptionInstrGuard( HKSTRD hKStrd, KS_::CKSQryOptionInstrGuardField *pQryOptionInstrGuard, int nRequestID );///查詢期權合約保障系數
	KSTRDAPIFACTORY_API int ReqQryExecOrderVolume( HKSTRD hKStrd, KS_::CKSQryExecOrderVolumeField *pQryExecOrderVolume, int nRequestID );///查詢宣告數量請求
	KSTRDAPIFACTORY_API int ReqQryStockOptionAssignment( HKSTRD hKStrd, KS_::CKSQryStockOptionAssignmentField *pQryStockOptionAssignment, int nRequestID );///查詢個股行權指派信息
	KSTRDAPIFACTORY_API int ReqQryInvestorTradeLevel( HKSTRD hKStrd, KS_::CKSQryInvestorTradeLevelField *pQryInvestorTradeLevel, int nRequestID );///查詢客戶交易級別
	KSTRDAPIFACTORY_API int ReqQryPurchaseLimitAmt( HKSTRD hKStrd, KS_::CKSQryPurchaseLimitAmtField *pQryPurchaseLimitAmt, int nRequestID );///查詢個股限購額度
	KSTRDAPIFACTORY_API int ReqQryPositionLimitVol( HKSTRD hKStrd, KS_::CKSQryPositionLimitVolField *pQryPositionLimitVol, int nRequestID );///查詢個股限倉額度
	KSTRDAPIFACTORY_API int ReqQryHistoryOrder( HKSTRD hKStrd, KS_::CKSQryHistoryOrderField *pQryHistoryOrder, int nRequestID );///請求查詢個股歷史報單
	KSTRDAPIFACTORY_API int ReqQryHistoryTrade( HKSTRD hKStrd, KS_::CKSQryHistoryTradeField *pQryHistoryTrade, int nRequestID );///請求查詢個股歷史成交
	KSTRDAPIFACTORY_API int ReqQryStockOptionHistoryAssignment( HKSTRD hKStrd, KS_::CKSQryHistoryAssignmentField *pQryHistoryAssignment, int nRequestID );///請求查詢個股歷史行權指派明細
	KSTRDAPIFACTORY_API int ReqQryStockOptionDelivDetail( HKSTRD hKStrd, KS_::CKSQrySODelivDetailField *pQrySODelivDetail, int nRequestID );///請求查詢個股行權交割明細
	KSTRDAPIFACTORY_API int ReqAutoExecOrderAction( HKSTRD hKStrd, KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, int nRequestID );///自動行權執行操作
// ---- CKSVocApi ----
// v2.3.50212 // 20150410 by Tim Lin // [Step].3
	KSTRDAPIFACTORY_API int ReqQueryInvestorOpenPosition( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorOpenPosition, int nRequestID );///查詢開盤前的持倉明細應答
	KSTRDAPIFACTORY_API int ReqQueryInvestorOpenCombinePosition( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorOpenCombinePosition, int nRequestID );///查詢開盤前的組合持倉明細應答
	KSTRDAPIFACTORY_API int ReqBulkCancelOrder( HKSTRD hKStrd, KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, int nRequestID );///批量報單撤除請求回應
	KSTRDAPIFACTORY_API int ReqQryCloseStrategy( HKSTRD hKStrd, KS_::CKSCloseStrategy *pCloseStrategy, int nRequestID );///平倉策略查詢回應
	KSTRDAPIFACTORY_API int ReqQryCombStrategy( HKSTRD hKStrd, KS_::CKSCombStrategy *pCombStrategy, int nRequestID );///組合策略查詢回應
	KSTRDAPIFACTORY_API int ReqQryOptionCombStrategy( HKSTRD hKStrd, KS_::CKSOptionCombStrategy *pOptionCombStrategy, int nRequestID );///期權組合策略查詢回應
	KSTRDAPIFACTORY_API int ReqQryTransferInfo( HKSTRD hKStrd, KS_::CKSTransferInfo *pTransferInfo, int nRequestID );///請求查詢客戶轉帳資訊回應
	KSTRDAPIFACTORY_API int ReqQryKSTradingNotice( HKSTRD hKStrd, KS_::CKSQryTradingNoticeField *pQryTradingNotice, int nRequestID );///請求查詢交易通知回應
	KSTRDAPIFACTORY_API int ReqQryUserProductUrl( HKSTRD hKStrd, KS_::CKSQryUserProductUrlField *pQryUserProductUrl, int nRequestID );///使用者端產品資源查詢應答
	KSTRDAPIFACTORY_API int ReqQryMaxCombActionVolume( HKSTRD hKStrd, KS_::CKSMaxCombActionVolumeField *pMaxCombActionVolume, int nRequestID );///最大組合拆分單量查詢請求回應
	KSTRDAPIFACTORY_API int ReqQryKSInstrumentMarginRate( HKSTRD hKStrd, KS_::CKSQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID );///請求查詢合約保證金率回應
//------------------------------------------------------------------------------------------------------------------------
// Regist callback function CThostFtdcTraderApi, CKSVocApi, CKSOptionApi
//------------------------------------------------------------------------------------------------------------------------
// ---- CThostFtdcTraderSpi ----
	KSTRDAPIFACTORY_API void RegOnFrontConnected( HKSTRD hKStrd, pfOnFrontConnected callback );
	KSTRDAPIFACTORY_API void RegOnFrontDisconnected( HKSTRD hKStrd, pfOnFrontDisconnected callback );
	KSTRDAPIFACTORY_API void RegOnHeartBeatWarning( HKSTRD hKStrd, pfOnHeartBeatWarning callback );
	KSTRDAPIFACTORY_API void RegOnRspAuthenticate( HKSTRD hKStrd, pfOnRspAuthenticate callback );
	KSTRDAPIFACTORY_API void RegOnRspUserLogin( HKSTRD hKStrd, pfOnRspUserLogin callback );
	KSTRDAPIFACTORY_API void RegOnRspUserLogout( HKSTRD hKStrd, pfOnRspUserLogout callback );
	KSTRDAPIFACTORY_API void RegOnRspUserPasswordUpdate( HKSTRD hKStrd, pfOnRspUserPasswordUpdate callback );
	KSTRDAPIFACTORY_API void RegOnRspTradingAccountPasswordUpdate( HKSTRD hKStrd, pfOnRspTradingAccountPasswordUpdate callback );
	KSTRDAPIFACTORY_API void RegOnRspOrderInsert( HKSTRD hKStrd, pfOnRspOrderInsert callback );
	KSTRDAPIFACTORY_API void RegOnRspParkedOrderInsert( HKSTRD hKStrd, pfOnRspParkedOrderInsert callback );
	KSTRDAPIFACTORY_API void RegOnRspParkedOrderAction( HKSTRD hKStrd, pfOnRspParkedOrderAction callback );
	KSTRDAPIFACTORY_API void RegOnRspOrderAction( HKSTRD hKStrd, pfOnRspOrderAction callback );
	KSTRDAPIFACTORY_API void RegOnRspQueryMaxOrderVolume( HKSTRD hKStrd, pfOnRspQueryMaxOrderVolume callback );
	KSTRDAPIFACTORY_API void RegOnRspSettlementInfoConfirm( HKSTRD hKStrd, pfOnRspSettlementInfoConfirm callback );
	KSTRDAPIFACTORY_API void RegOnRspRemoveParkedOrder( HKSTRD hKStrd, pfOnRspRemoveParkedOrder callback );
	KSTRDAPIFACTORY_API void RegOnRspRemoveParkedOrderAction( HKSTRD hKStrd, pfOnRspRemoveParkedOrderAction callback );
	KSTRDAPIFACTORY_API void RegOnRspExecOrderInsert( HKSTRD hKStrd, pfOnRspExecOrderInsert callback );
	KSTRDAPIFACTORY_API void RegOnRspExecOrderAction( HKSTRD hKStrd, pfOnRspExecOrderAction callback );
	KSTRDAPIFACTORY_API void RegOnRspForQuoteInsert( HKSTRD hKStrd, pfOnRspForQuoteInsert callback );
	KSTRDAPIFACTORY_API void RegOnRspQuoteInsert( HKSTRD hKStrd, pfOnRspQuoteInsert callback );
	KSTRDAPIFACTORY_API void RegOnRspQuoteAction( HKSTRD hKStrd, pfOnRspQuoteAction callback );
	KSTRDAPIFACTORY_API void RegOnRspQryOrder( HKSTRD hKStrd, pfOnRspQryOrder callback );
	KSTRDAPIFACTORY_API void RegOnRspQryTrade( HKSTRD hKStrd, pfOnRspQryTrade callback );
	KSTRDAPIFACTORY_API void RegOnRspQryInvestorPosition( HKSTRD hKStrd, pfOnRspQryInvestorPosition callback );
	KSTRDAPIFACTORY_API void RegOnRspQryTradingAccount( HKSTRD hKStrd, pfOnRspQryTradingAccount callback );
	KSTRDAPIFACTORY_API void RegOnRspQryInvestor( HKSTRD hKStrd, pfOnRspQryInvestor callback );
	KSTRDAPIFACTORY_API void RegOnRspQryTradingCode( HKSTRD hKStrd, pfOnRspQryTradingCode callback );
	KSTRDAPIFACTORY_API void RegOnRspQryInstrumentMarginRate( HKSTRD hKStrd, pfOnRspQryInstrumentMarginRate callback );
	KSTRDAPIFACTORY_API void RegOnRspQryInstrumentCommissionRate( HKSTRD hKStrd, pfOnRspQryInstrumentCommissionRate callback );
	KSTRDAPIFACTORY_API void RegOnRspQryExchange( HKSTRD hKStrd, pfOnRspQryExchange callback );
	KSTRDAPIFACTORY_API void RegOnRspQryProduct( HKSTRD hKStrd, pfOnRspQryProduct callback );
	KSTRDAPIFACTORY_API void RegOnRspQryInstrument( HKSTRD hKStrd, pfOnRspQryInstrument callback );
	KSTRDAPIFACTORY_API void RegOnRspQryDepthMarketData( HKSTRD hKStrd, pfOnRspQryDepthMarketData callback );
	KSTRDAPIFACTORY_API void RegOnRspQrySettlementInfo( HKSTRD hKStrd, pfOnRspQrySettlementInfo callback );
	KSTRDAPIFACTORY_API void RegOnRspQryTransferBank( HKSTRD hKStrd, pfOnRspQryTransferBank callback );
	KSTRDAPIFACTORY_API void RegOnRspQryInvestorPositionDetail( HKSTRD hKStrd, pfOnRspQryInvestorPositionDetail callback );
	KSTRDAPIFACTORY_API void RegOnRspQryNotice( HKSTRD hKStrd, pfOnRspQryNotice callback );
	KSTRDAPIFACTORY_API void RegOnRspQrySettlementInfoConfirm( HKSTRD hKStrd, pfOnRspQrySettlementInfoConfirm callback );
	KSTRDAPIFACTORY_API void RegOnRspQryInvestorPositionCombineDetail( HKSTRD hKStrd, pfOnRspQryInvestorPositionCombineDetail callback );
	KSTRDAPIFACTORY_API void RegOnRspQryCFMMCTradingAccountKey( HKSTRD hKStrd, pfOnRspQryCFMMCTradingAccountKey callback );
	KSTRDAPIFACTORY_API void RegOnRspQryEWarrantOffset( HKSTRD hKStrd, pfOnRspQryEWarrantOffset callback );
	KSTRDAPIFACTORY_API void RegOnRspQryInvestorProductGroupMargin( HKSTRD hKStrd, pfOnRspQryInvestorProductGroupMargin callback );
	KSTRDAPIFACTORY_API void RegOnRspQryExchangeMarginRate( HKSTRD hKStrd, pfOnRspQryExchangeMarginRate callback );
	KSTRDAPIFACTORY_API void RegOnRspQryExchangeMarginRateAdjust( HKSTRD hKStrd, pfOnRspQryExchangeMarginRateAdjust callback );
	KSTRDAPIFACTORY_API void RegOnRspQryExchangeRate( HKSTRD hKStrd, pfOnRspQryExchangeRate callback );
	KSTRDAPIFACTORY_API void RegOnRspQrySecAgentACIDMap( HKSTRD hKStrd, pfOnRspQrySecAgentACIDMap callback );
	KSTRDAPIFACTORY_API void RegOnRspQryOptionInstrTradeCost( HKSTRD hKStrd, pfOnRspQryOptionInstrTradeCost callback );
	KSTRDAPIFACTORY_API void RegOnRspQryOptionInstrCommRate( HKSTRD hKStrd, pfOnRspQryOptionInstrCommRate callback );
	KSTRDAPIFACTORY_API void RegOnRspQryExecOrder( HKSTRD hKStrd, pfOnRspQryExecOrder callback );
	KSTRDAPIFACTORY_API void RegOnRspQryForQuote( HKSTRD hKStrd, pfOnRspQryForQuote callback );
	KSTRDAPIFACTORY_API void RegOnRspQryQuote( HKSTRD hKStrd, pfOnRspQryQuote callback );
	KSTRDAPIFACTORY_API void RegOnRspQryTransferSerial( HKSTRD hKStrd, pfOnRspQryTransferSerial callback );
	KSTRDAPIFACTORY_API void RegOnRspQryAccountregister( HKSTRD hKStrd, pfOnRspQryAccountregister callback );
	KSTRDAPIFACTORY_API void RegOnRspError( HKSTRD hKStrd, pfOnRspError callback );
	KSTRDAPIFACTORY_API void RegOnRtnOrder( HKSTRD hKStrd, pfOnRtnOrder callback );
	KSTRDAPIFACTORY_API void RegOnRtnTrade( HKSTRD hKStrd, pfOnRtnTrade callback );
	KSTRDAPIFACTORY_API void RegOnErrRtnOrderInsert( HKSTRD hKStrd, pfOnErrRtnOrderInsert callback );
	KSTRDAPIFACTORY_API void RegOnErrRtnOrderAction( HKSTRD hKStrd, pfOnErrRtnOrderAction callback );
	KSTRDAPIFACTORY_API void RegOnRtnInstrumentStatus( HKSTRD hKStrd, pfOnRtnInstrumentStatus callback );
	KSTRDAPIFACTORY_API void RegOnRtnTradingNotice( HKSTRD hKStrd, pfOnRtnTradingNotice callback );
	KSTRDAPIFACTORY_API void RegOnRtnErrorConditionalOrder( HKSTRD hKStrd, pfOnRtnErrorConditionalOrder callback );
	KSTRDAPIFACTORY_API void RegOnRtnExecOrder( HKSTRD hKStrd, pfOnRtnExecOrder callback );
	KSTRDAPIFACTORY_API void RegOnErrRtnExecOrderInsert( HKSTRD hKStrd, pfOnErrRtnExecOrderInsert callback );
	KSTRDAPIFACTORY_API void RegOnErrRtnExecOrderAction( HKSTRD hKStrd, pfOnErrRtnExecOrderAction callback );
	KSTRDAPIFACTORY_API void RegOnErrRtnForQuoteInsert( HKSTRD hKStrd, pfOnErrRtnForQuoteInsert callback );
	KSTRDAPIFACTORY_API void RegOnRtnQuote( HKSTRD hKStrd, pfOnRtnQuote callback );
	KSTRDAPIFACTORY_API void RegOnErrRtnQuoteInsert( HKSTRD hKStrd, pfOnErrRtnQuoteInsert callback );
	KSTRDAPIFACTORY_API void RegOnErrRtnQuoteAction( HKSTRD hKStrd, pfOnErrRtnQuoteAction callback );
	KSTRDAPIFACTORY_API void RegOnRtnForQuoteRsp( HKSTRD hKStrd, pfOnRtnForQuoteRsp callback );
	KSTRDAPIFACTORY_API void RegOnRtnCFMMCTradingAccountToken( HKSTRD hKStrd, pfOnRtnCFMMCTradingAccountToken callback );
	KSTRDAPIFACTORY_API void RegOnRspQryContractBank( HKSTRD hKStrd, pfOnRspQryContractBank callback );
	KSTRDAPIFACTORY_API void RegOnRspQryParkedOrder( HKSTRD hKStrd, pfOnRspQryParkedOrder callback );
	KSTRDAPIFACTORY_API void RegOnRspQryParkedOrderAction( HKSTRD hKStrd, pfOnRspQryParkedOrderAction callback );
	KSTRDAPIFACTORY_API void RegOnRspQryTradingNotice( HKSTRD hKStrd, pfOnRspQryTradingNotice callback );
	KSTRDAPIFACTORY_API void RegOnRspQryBrokerTradingParams( HKSTRD hKStrd, pfOnRspQryBrokerTradingParams callback );
	KSTRDAPIFACTORY_API void RegOnRspQryBrokerTradingAlgos( HKSTRD hKStrd, pfOnRspQryBrokerTradingAlgos callback );
	KSTRDAPIFACTORY_API void RegOnRspQueryCFMMCTradingAccountToken( HKSTRD hKStrd, pfOnRspQueryCFMMCTradingAccountToken callback );
	KSTRDAPIFACTORY_API void RegOnRtnFromBankToFutureByBank( HKSTRD hKStrd, pfOnRtnFromBankToFutureByBank callback );
	KSTRDAPIFACTORY_API void RegOnRtnFromFutureToBankByBank( HKSTRD hKStrd, pfOnRtnFromFutureToBankByBank callback );
	KSTRDAPIFACTORY_API void RegOnRtnRepealFromBankToFutureByBank( HKSTRD hKStrd, pfOnRtnRepealFromBankToFutureByBank callback );
	KSTRDAPIFACTORY_API void RegOnRtnRepealFromFutureToBankByBank( HKSTRD hKStrd, pfOnRtnRepealFromFutureToBankByBank callback );
	KSTRDAPIFACTORY_API void RegOnRtnFromBankToFutureByFuture( HKSTRD hKStrd, pfOnRtnFromBankToFutureByFuture callback );
	KSTRDAPIFACTORY_API void RegOnRtnFromFutureToBankByFuture( HKSTRD hKStrd, pfOnRtnFromFutureToBankByFuture callback );
	KSTRDAPIFACTORY_API void RegOnRtnRepealFromBankToFutureByFutureManual( HKSTRD hKStrd, pfOnRtnRepealFromBankToFutureByFutureManual callback );
	KSTRDAPIFACTORY_API void RegOnRtnRepealFromFutureToBankByFutureManual( HKSTRD hKStrd, pfOnRtnRepealFromFutureToBankByFutureManual callback );
	KSTRDAPIFACTORY_API void RegOnRtnQueryBankBalanceByFuture( HKSTRD hKStrd, pfOnRtnQueryBankBalanceByFuture callback );
	KSTRDAPIFACTORY_API void RegOnErrRtnBankToFutureByFuture( HKSTRD hKStrd, pfOnErrRtnBankToFutureByFuture callback );
	KSTRDAPIFACTORY_API void RegOnErrRtnFutureToBankByFuture( HKSTRD hKStrd, pfOnErrRtnFutureToBankByFuture callback );
	KSTRDAPIFACTORY_API void RegOnErrRtnRepealBankToFutureByFutureManual( HKSTRD hKStrd, pfOnErrRtnRepealBankToFutureByFutureManual callback );
	KSTRDAPIFACTORY_API void RegOnErrRtnRepealFutureToBankByFutureManual( HKSTRD hKStrd, pfOnErrRtnRepealFutureToBankByFutureManual callback );
	KSTRDAPIFACTORY_API void RegOnErrRtnQueryBankBalanceByFuture( HKSTRD hKStrd, pfOnErrRtnQueryBankBalanceByFuture callback );
	KSTRDAPIFACTORY_API void RegOnRtnRepealFromBankToFutureByFuture( HKSTRD hKStrd, pfOnRtnRepealFromBankToFutureByFuture callback );
	KSTRDAPIFACTORY_API void RegOnRtnRepealFromFutureToBankByFuture( HKSTRD hKStrd, pfOnRtnRepealFromFutureToBankByFuture callback );
	KSTRDAPIFACTORY_API void RegOnRspFromBankToFutureByFuture( HKSTRD hKStrd, pfOnRspFromBankToFutureByFuture callback );
	KSTRDAPIFACTORY_API void RegOnRspFromFutureToBankByFuture( HKSTRD hKStrd, pfOnRspFromFutureToBankByFuture callback );
	KSTRDAPIFACTORY_API void RegOnRspQueryBankAccountMoneyByFuture( HKSTRD hKStrd, pfOnRspQueryBankAccountMoneyByFuture callback );
	KSTRDAPIFACTORY_API void RegOnRtnOpenAccountByBank( HKSTRD hKStrd, pfOnRtnOpenAccountByBank callback );
	KSTRDAPIFACTORY_API void RegOnRtnCancelAccountByBank( HKSTRD hKStrd, pfOnRtnCancelAccountByBank callback );
	KSTRDAPIFACTORY_API void RegOnRtnChangeAccountByBank( HKSTRD hKStrd, pfOnRtnChangeAccountByBank callback );
// v2.3.50212 // 20150410 by Tim Lin // <Step>.4
	KSTRDAPIFACTORY_API void RegOnRspLockInsert( HKSTRD hKStrd, pfOnRspLockInsert callback );///鎖定應答
	KSTRDAPIFACTORY_API void RegOnRspQryLock( HKSTRD hKStrd, pfOnRspQryLock callback );///請求查詢鎖定回應	
	KSTRDAPIFACTORY_API void RegOnRspQryLockPosition( HKSTRD hKStrd, pfOnRspQryLockPosition callback );///請求查詢鎖定證券倉位回應
	KSTRDAPIFACTORY_API void RegOnRtnLock( HKSTRD hKStrd, pfOnRtnLock callback );///鎖定通知
	KSTRDAPIFACTORY_API void RegOnErrRtnLockInsert( HKSTRD hKStrd, pfOnErrRtnLockInsert callback );///鎖定錯誤通知	
// ---- CKSOptionSpi ----
// v2.3.50212 // 20150410 by Tim Lin // Step.4
	KSTRDAPIFACTORY_API void RegOnRspQryIndexPrice( HKSTRD hKStrd, pfOnRspQryIndexPrice callback );///查詢股指現貨指數
	KSTRDAPIFACTORY_API void RegOnRspQryOptionInstrGuard( HKSTRD hKStrd, pfOnRspQryOptionInstrGuard callback );///查詢期權合約保障系數
	KSTRDAPIFACTORY_API void RegOnRspQryExecOrderVolume( HKSTRD hKStrd, pfOnRspQryExecOrderVolume callback );///查詢宣告數量請求
	KSTRDAPIFACTORY_API void RegOnRspQryStockOptionAssignment( HKSTRD hKStrd, pfOnRspQryStockOptionAssignment callback );///查詢個股行權指派信息
	KSTRDAPIFACTORY_API void RegOnRspQryInvestorTradeLevel( HKSTRD hKStrd, pfOnRspQryInvestorTradeLevel callback );///查詢客戶交易級別
	KSTRDAPIFACTORY_API void RegOnRspQryPurchaseLimitAmt( HKSTRD hKStrd, pfOnRspQryPurchaseLimitAmt callback );///查詢個股限購額度
	KSTRDAPIFACTORY_API void RegOnRspQryPositionLimitVol( HKSTRD hKStrd, pfOnRspQryPositionLimitVol callback );///查詢個股限倉額度
	KSTRDAPIFACTORY_API void RegOnRspQryHistoryOrder( HKSTRD hKStrd, pfOnRspQryHistoryOrder callback );///請求查詢個股歷史報單
	KSTRDAPIFACTORY_API void RegOnRspQryHistoryTrade( HKSTRD hKStrd, pfOnRspQryHistoryTrade callback );///請求查詢個股歷史成交
	KSTRDAPIFACTORY_API void RegOnRspQryStockOptionHistoryAssignment( HKSTRD hKStrd, pfOnRspQryStockOptionHistoryAssignment callback );///請求查詢個股歷史行權指派明細
	KSTRDAPIFACTORY_API void RegOnRspQryStockOptionDelivDetail( HKSTRD hKStrd, pfOnRspQryStockOptionDelivDetail callback );///請求查詢個股行權交割明細
	KSTRDAPIFACTORY_API void RegOnRspAutoExecOrderAction( HKSTRD hKStrd, pfOnRspAutoExecOrderAction callback );///自動行權執行操作
// ---- CKSVocSpi ----
// v2.3.50212 // 20150410 by Tim Lin // [Step].4
	KSTRDAPIFACTORY_API void RegOnRspQryInvestorOpenPosition( HKSTRD hKStrd, pfOnRspQryInvestorOpenPosition callback );///查詢開盤前的持倉明細應答
	KSTRDAPIFACTORY_API void RegOnRspQryInvestorOpenCombinePosition( HKSTRD hKStrd, pfOnRspQryInvestorOpenCombinePosition callback );///查詢開盤前的組合持倉明細應答
	KSTRDAPIFACTORY_API void RegOnRspBulkCancelOrder( HKSTRD hKStrd, pfOnRspBulkCancelOrder callback );///批量報單撤除請求回應
	KSTRDAPIFACTORY_API void RegOnRspQryCloseStrategy( HKSTRD hKStrd, pfOnRspQryCloseStrategy callback );///平倉策略查詢回應
	KSTRDAPIFACTORY_API void RegOnRspQryCombStrategy( HKSTRD hKStrd, pfOnRspQryCombStrategy callback );///組合策略查詢回應
	KSTRDAPIFACTORY_API void RegOnRspQryOptionCombStrategy( HKSTRD hKStrd, pfOnRspQryOptionCombStrategy callback );///期權組合策略查詢回應
	KSTRDAPIFACTORY_API void RegOnRspQryTransferInfo( HKSTRD hKStrd, pfOnRspQryTransferInfo callback );///請求查詢客戶轉帳資訊回應
	KSTRDAPIFACTORY_API void RegOnRspQryKSTradingNotice( HKSTRD hKStrd, pfOnRspQryKSTradingNotice callback );///請求查詢交易通知回應
	KSTRDAPIFACTORY_API void RegOnRspQryUserProductUrl( HKSTRD hKStrd, pfOnRspQryUserProductUrl callback );///使用者端產品資源查詢應答
	KSTRDAPIFACTORY_API void RegOnRspQryMaxCombActionVolume( HKSTRD hKStrd, pfOnRspQryMaxCombActionVolume callback );///最大組合拆分單量查詢請求回應
	KSTRDAPIFACTORY_API void RegOnRtnKSTradingNotice( HKSTRD hKStrd, pfOnRtnKSTradingNotice callback );///交易通知
	KSTRDAPIFACTORY_API void RegOnRspQryKSInstrumentMarginRate( HKSTRD hKStrd, pfOnRspQryKSInstrumentMarginRate callback );///請求查詢合約保證金率回應
//------------------------------------------------------------------------------------------------------------------------
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
