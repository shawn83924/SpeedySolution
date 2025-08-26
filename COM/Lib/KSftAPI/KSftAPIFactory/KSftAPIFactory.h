//------------------------------------------------------------------------------------------------------------------------
#ifndef __KSftAPIFactory_H
#define __KSftAPIFactory_H
//------------------------------------------------------------------------------------------------------------------------
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the KSFTAPIFACTORY_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// KSFTAPIFACTORY_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
//------------------------------------------------------------------------------------------------------------------------
#ifdef KSFTAPIFACTORY_EXPORTS
#define KSFTAPIFACTORY_API __declspec(dllexport)
#else
#define KSFTAPIFACTORY_API __declspec(dllimport)
#endif
//------------------------------------------------------------------------------------------------------------------------
#include "KSUserApiDataTypeEx.h"
#include "KSUserApiStructEx.h"
#include "KSftAPIDynamic.h"
//------------------------------------------------------------------------------------------------------------------------
extern "C"
{
//------------------------------------------------------------------------------------------------------------------------
// Export CThostFtdcTraderApi methods into C style.
//------------------------------------------------------------------------------------------------------------------------
	KSFTAPIFACTORY_API HKSFT CreateKSftTraderApi( void* Owner );
	KSFTAPIFACTORY_API void ReleaseKSftTraderApi( HKSFT hKsft );
	KSFTAPIFACTORY_API void Init( HKSFT hKsft );
	KSFTAPIFACTORY_API const char *GetTradingDay( HKSFT hKsft );
	KSFTAPIFACTORY_API void RegisterFront( HKSFT hKsft, char *pszFrontAddress );
	KSFTAPIFACTORY_API void RegisterNameServer( HKSFT hKsft, char *pszNsAddress );
	KSFTAPIFACTORY_API void RegisterFensUserInfo( HKSFT hKsft, KS_::CThostFtdcFensUserInfoField * pFensUserInfo );
//	KSFTAPIFACTORY_API void RegisterSpi( HKSFT hKsft, KS_::CThostFtdcTraderSpi *pSpi );
	KSFTAPIFACTORY_API void SubscribePrivateTopic( HKSFT hKsft, KS_::THOST_TE_RESUME_TYPE nResumeType );
	KSFTAPIFACTORY_API void SubscribePublicTopic( HKSFT hKsft, KS_::THOST_TE_RESUME_TYPE nResumeType );
	KSFTAPIFACTORY_API int ReqAuthenticate( HKSFT hKsft, KS_::CThostFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID );
	KSFTAPIFACTORY_API int ReqUserLogin( HKSFT hKsft, KS_::CThostFtdcReqUserLoginField *pReqUserLoginField, int nRequestID );
	KSFTAPIFACTORY_API int ReqUserLogout( HKSFT hKsft, KS_::CThostFtdcUserLogoutField *pUserLogout, int nRequestID );
	KSFTAPIFACTORY_API int ReqUserPasswordUpdate( HKSFT hKsft, KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID );
	KSFTAPIFACTORY_API int ReqTradingAccountPasswordUpdate( HKSFT hKsft, KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, int nRequestID );
	KSFTAPIFACTORY_API int ReqOrderInsert( HKSFT hKsft, KS_::CThostFtdcInputOrderField *pInputOrder, int nRequestID );
	KSFTAPIFACTORY_API int ReqParkedOrderInsert( HKSFT hKsft, KS_::CThostFtdcParkedOrderField *pParkedOrder, int nRequestID );
	KSFTAPIFACTORY_API int ReqParkedOrderAction( HKSFT hKsft, KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, int nRequestID );
	KSFTAPIFACTORY_API int ReqOrderAction( HKSFT hKsft, KS_::CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID );
	KSFTAPIFACTORY_API int ReqQueryMaxOrderVolume( HKSFT hKsft, KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, int nRequestID );
	KSFTAPIFACTORY_API int ReqSettlementInfoConfirm( HKSFT hKsft, KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, int nRequestID );
	KSFTAPIFACTORY_API int ReqRemoveParkedOrder( HKSFT hKsft, KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, int nRequestID );
	KSFTAPIFACTORY_API int ReqRemoveParkedOrderAction( HKSFT hKsft, KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, int nRequestID );
	KSFTAPIFACTORY_API int ReqExecOrderInsert( HKSFT hKsft, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, int nRequestID );
	KSFTAPIFACTORY_API int ReqExecOrderAction( HKSFT hKsft, KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, int nRequestID );
	KSFTAPIFACTORY_API int ReqForQuoteInsert( HKSFT hKsft, KS_::CThostFtdcInputForQuoteField *pInputForQuote, int nRequestID );
	KSFTAPIFACTORY_API int ReqQuoteInsert( HKSFT hKsft, KS_::CThostFtdcInputQuoteField *pInputQuote, int nRequestID );
	KSFTAPIFACTORY_API int ReqQuoteAction( HKSFT hKsft, KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryOrder( HKSFT hKsft, KS_::CThostFtdcQryOrderField *pQryOrder, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryTrade( HKSFT hKsft, KS_::CThostFtdcQryTradeField *pQryTrade, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryInvestorPosition( HKSFT hKsft, KS_::CThostFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryTradingAccount( HKSFT hKsft, KS_::CThostFtdcQryTradingAccountField *pQryTradingAccount, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryInvestor( HKSFT hKsft, KS_::CThostFtdcQryInvestorField *pQryInvestor, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryTradingCode( HKSFT hKsft, KS_::CThostFtdcQryTradingCodeField *pQryTradingCode, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryInstrumentMarginRate( HKSFT hKsft, KS_::CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryInstrumentCommissionRate( HKSFT hKsft, KS_::CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryExchange( HKSFT hKsft, KS_::CThostFtdcQryExchangeField *pQryExchange, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryProduct( HKSFT hKsft, KS_::CThostFtdcQryProductField *pQryProduct, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryInstrument( HKSFT hKsft, KS_::CThostFtdcQryInstrumentField *pQryInstrument, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryDepthMarketData( HKSFT hKsft, KS_::CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, int nRequestID );
	KSFTAPIFACTORY_API int ReqQrySettlementInfo( HKSFT hKsft, KS_::CThostFtdcQrySettlementInfoField *pQrySettlementInfo, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryTransferBank( HKSFT hKsft, KS_::CThostFtdcQryTransferBankField *pQryTransferBank, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryInvestorPositionDetail( HKSFT hKsft, KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryNotice( HKSFT hKsft, KS_::CThostFtdcQryNoticeField *pQryNotice, int nRequestID );
	KSFTAPIFACTORY_API int ReqQrySettlementInfoConfirm( HKSFT hKsft, KS_::CThostFtdcQrySettlementInfoConfirmField *pQrySettlementInfoConfirm, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryInvestorPositionCombineDetail( HKSFT hKsft, KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorPositionCombineDetail, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryCFMMCTradingAccountKey( HKSFT hKsft, KS_::CThostFtdcQryCFMMCTradingAccountKeyField *pQryCFMMCTradingAccountKey, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryEWarrantOffset( HKSFT hKsft, KS_::CThostFtdcQryEWarrantOffsetField *pQryEWarrantOffset, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryInvestorProductGroupMargin( HKSFT hKsft, KS_::CThostFtdcQryInvestorProductGroupMarginField *pQryInvestorProductGroupMargin, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryExchangeMarginRate( HKSFT hKsft, KS_::CThostFtdcQryExchangeMarginRateField *pQryExchangeMarginRate, int nRequestID);
	KSFTAPIFACTORY_API int ReqQryExchangeMarginRateAdjust( HKSFT hKsft, KS_::CThostFtdcQryExchangeMarginRateAdjustField *pQryExchangeMarginRateAdjust, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryExchangeRate( HKSFT hKsft, KS_::CThostFtdcQryExchangeRateField *pQryExchangeRate, int nRequestID );
	KSFTAPIFACTORY_API int ReqQrySecAgentACIDMap( HKSFT hKsft, KS_::CThostFtdcQrySecAgentACIDMapField *pQrySecAgentACIDMap, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryOptionInstrTradeCost( HKSFT hKsft, KS_::CThostFtdcQryOptionInstrTradeCostField *pQryOptionInstrTradeCost, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryOptionInstrCommRate( HKSFT hKsft, KS_::CThostFtdcQryOptionInstrCommRateField *pQryOptionInstrCommRate, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryExecOrder( HKSFT hKsft, KS_::CThostFtdcQryExecOrderField *pQryExecOrder, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryForQuote( HKSFT hKsft, KS_::CThostFtdcQryForQuoteField *pQryForQuote, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryQuote( HKSFT hKsft, KS_::CThostFtdcQryQuoteField *pQryQuote, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryTransferSerial( HKSFT hKsft, KS_::CThostFtdcQryTransferSerialField *pQryTransferSerial, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryAccountregister( HKSFT hKsft, KS_::CThostFtdcQryAccountregisterField *pQryAccountregister, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryContractBank( HKSFT hKsft, KS_::CThostFtdcQryContractBankField *pQryContractBank, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryParkedOrder( HKSFT hKsft, KS_::CThostFtdcQryParkedOrderField *pQryParkedOrder, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryParkedOrderAction( HKSFT hKsft, KS_::CThostFtdcQryParkedOrderActionField *pQryParkedOrderAction, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryTradingNotice( HKSFT hKsft, KS_::CThostFtdcQryTradingNoticeField *pQryTradingNotice, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryBrokerTradingParams( HKSFT hKsft, KS_::CThostFtdcQryBrokerTradingParamsField *pQryBrokerTradingParams, int nRequestID );
	KSFTAPIFACTORY_API int ReqQryBrokerTradingAlgos( HKSFT hKsft, KS_::CThostFtdcQryBrokerTradingAlgosField *pQryBrokerTradingAlgos, int nRequestID );
	KSFTAPIFACTORY_API int ReqFromBankToFutureByFuture( HKSFT hKsft, KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID );
	KSFTAPIFACTORY_API int ReqFromFutureToBankByFuture( HKSFT hKsft, KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID );
	KSFTAPIFACTORY_API int ReqQueryBankAccountMoneyByFuture( HKSFT hKsft, KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, int nRequestID );
	KSFTAPIFACTORY_API int ReqQueryInvestorOpenPosition( HKSFT hKsft, KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorOpenPosition, int nRequestID );
	KSFTAPIFACTORY_API int ReqQueryInvestorOpenCombinePosition( HKSFT hKsft, KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorOpenCombinePosition, int nRequestID );
	KSFTAPIFACTORY_API void* LoadExtApi( HKSFT hKsft, void * spi, const char *ExtApiName = "KSCosApi" );
	KSFTAPIFACTORY_API int ReqBulkCancelOrder( HKSFT hKsft, KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, int nRequestID );
//------------------------------------------------------------------------------------------------------------------------
// Functions to regist callback function
//------------------------------------------------------------------------------------------------------------------------
	KSFTAPIFACTORY_API void RegOnFrontConnected( HKSFT hKsft, pfOnFrontConnected callback );
	KSFTAPIFACTORY_API void RegOnFrontDisconnected( HKSFT hKsft, pfOnFrontDisconnected callback );
	KSFTAPIFACTORY_API void RegOnHeartBeatWarning( HKSFT hKsft, pfOnHeartBeatWarning callback );
	KSFTAPIFACTORY_API void RegOnRspAuthenticate( HKSFT hKsft, pfOnRspAuthenticate callback );
	KSFTAPIFACTORY_API void RegOnRspUserLogin( HKSFT hKsft, pfOnRspUserLogin callback );
	KSFTAPIFACTORY_API void RegOnRspUserLogout( HKSFT hKsft, pfOnRspUserLogout callback );
	KSFTAPIFACTORY_API void RegOnRspUserPasswordUpdate( HKSFT hKsft, pfOnRspUserPasswordUpdate callback );
	KSFTAPIFACTORY_API void RegOnRspTradingAccountPasswordUpdate( HKSFT hKsft, pfOnRspTradingAccountPasswordUpdate callback );
	KSFTAPIFACTORY_API void RegOnRspOrderInsert( HKSFT hKsft, pfOnRspOrderInsert callback );
	KSFTAPIFACTORY_API void RegOnRspParkedOrderInsert( HKSFT hKsft, pfOnRspParkedOrderInsert callback );
	KSFTAPIFACTORY_API void RegOnRspParkedOrderAction( HKSFT hKsft, pfOnRspParkedOrderAction callback );
	KSFTAPIFACTORY_API void RegOnRspOrderAction( HKSFT hKsft, pfOnRspOrderAction callback );
	KSFTAPIFACTORY_API void RegOnRspQueryMaxOrderVolume( HKSFT hKsft, pfOnRspQueryMaxOrderVolume callback );
	KSFTAPIFACTORY_API void RegOnRspSettlementInfoConfirm( HKSFT hKsft, pfOnRspSettlementInfoConfirm callback );
	KSFTAPIFACTORY_API void RegOnRspRemoveParkedOrder( HKSFT hKsft, pfOnRspRemoveParkedOrder callback );
	KSFTAPIFACTORY_API void RegOnRspRemoveParkedOrderAction( HKSFT hKsft, pfOnRspRemoveParkedOrderAction callback );
	KSFTAPIFACTORY_API void RegOnRspExecOrderInsert( HKSFT hKsft, pfOnRspExecOrderInsert callback );
	KSFTAPIFACTORY_API void RegOnRspExecOrderAction( HKSFT hKsft, pfOnRspExecOrderAction callback );
	KSFTAPIFACTORY_API void RegOnRspForQuoteInsert( HKSFT hKsft, pfOnRspForQuoteInsert callback );
	KSFTAPIFACTORY_API void RegOnRspQuoteInsert( HKSFT hKsft, pfOnRspQuoteInsert callback );
	KSFTAPIFACTORY_API void RegOnRspQuoteAction( HKSFT hKsft, pfOnRspQuoteAction callback );
	KSFTAPIFACTORY_API void RegOnRspQryOrder( HKSFT hKsft, pfOnRspQryOrder callback );
	KSFTAPIFACTORY_API void RegOnRspQryTrade( HKSFT hKsft, pfOnRspQryTrade callback );
	KSFTAPIFACTORY_API void RegOnRspQryInvestorPosition( HKSFT hKsft, pfOnRspQryInvestorPosition callback );
	KSFTAPIFACTORY_API void RegOnRspQryTradingAccount( HKSFT hKsft, pfOnRspQryTradingAccount callback );
	KSFTAPIFACTORY_API void RegOnRspQryInvestor( HKSFT hKsft, pfOnRspQryInvestor callback );
	KSFTAPIFACTORY_API void RegOnRspQryTradingCode( HKSFT hKsft, pfOnRspQryTradingCode callback );
	KSFTAPIFACTORY_API void RegOnRspQryInstrumentMarginRate( HKSFT hKsft, pfOnRspQryInstrumentMarginRate callback );
	KSFTAPIFACTORY_API void RegOnRspQryInstrumentCommissionRate( HKSFT hKsft, pfOnRspQryInstrumentCommissionRate callback );
	KSFTAPIFACTORY_API void RegOnRspQryExchange( HKSFT hKsft, pfOnRspQryExchange callback );
	KSFTAPIFACTORY_API void RegOnRspQryProduct( HKSFT hKsft, pfOnRspQryProduct callback );
	KSFTAPIFACTORY_API void RegOnRspQryInstrument( HKSFT hKsft, pfOnRspQryInstrument callback );
	KSFTAPIFACTORY_API void RegOnRspQryDepthMarketData( HKSFT hKsft, pfOnRspQryDepthMarketData callback );
	KSFTAPIFACTORY_API void RegOnRspQrySettlementInfo( HKSFT hKsft, pfOnRspQrySettlementInfo callback );
	KSFTAPIFACTORY_API void RegOnRspQryTransferBank( HKSFT hKsft, pfOnRspQryTransferBank callback );
	KSFTAPIFACTORY_API void RegOnRspQryInvestorPositionDetail( HKSFT hKsft, pfOnRspQryInvestorPositionDetail callback );
	KSFTAPIFACTORY_API void RegOnRspQryNotice( HKSFT hKsft, pfOnRspQryNotice callback );
	KSFTAPIFACTORY_API void RegOnRspQrySettlementInfoConfirm( HKSFT hKsft, pfOnRspQrySettlementInfoConfirm callback );
	KSFTAPIFACTORY_API void RegOnRspQryInvestorPositionCombineDetail( HKSFT hKsft, pfOnRspQryInvestorPositionCombineDetail callback );
	KSFTAPIFACTORY_API void RegOnRspQryCFMMCTradingAccountKey( HKSFT hKsft, pfOnRspQryCFMMCTradingAccountKey callback );
	KSFTAPIFACTORY_API void RegOnRspQryEWarrantOffset( HKSFT hKsft, pfOnRspQryEWarrantOffset callback );
	KSFTAPIFACTORY_API void RegOnRspQryInvestorProductGroupMargin( HKSFT hKsft, pfOnRspQryInvestorProductGroupMargin callback );
	KSFTAPIFACTORY_API void RegOnRspQryExchangeMarginRate( HKSFT hKsft, pfOnRspQryExchangeMarginRate callback );
	KSFTAPIFACTORY_API void RegOnRspQryExchangeMarginRateAdjust( HKSFT hKsft, pfOnRspQryExchangeMarginRateAdjust callback );
	KSFTAPIFACTORY_API void RegOnRspQryExchangeRate( HKSFT hKsft, pfOnRspQryExchangeRate callback );
	KSFTAPIFACTORY_API void RegOnRspQrySecAgentACIDMap( HKSFT hKsft, pfOnRspQrySecAgentACIDMap callback );
	KSFTAPIFACTORY_API void RegOnRspQryOptionInstrTradeCost( HKSFT hKsft, pfOnRspQryOptionInstrTradeCost callback );
	KSFTAPIFACTORY_API void RegOnRspQryOptionInstrCommRate( HKSFT hKsft, pfOnRspQryOptionInstrCommRate callback );
	KSFTAPIFACTORY_API void RegOnRspQryExecOrder( HKSFT hKsft, pfOnRspQryExecOrder callback );
	KSFTAPIFACTORY_API void RegOnRspQryForQuote( HKSFT hKsft, pfOnRspQryForQuote callback );
	KSFTAPIFACTORY_API void RegOnRspQryQuote( HKSFT hKsft, pfOnRspQryQuote callback );
	KSFTAPIFACTORY_API void RegOnRspQryTransferSerial( HKSFT hKsft, pfOnRspQryTransferSerial callback );
	KSFTAPIFACTORY_API void RegOnRspQryAccountregister( HKSFT hKsft, pfOnRspQryAccountregister callback );
	KSFTAPIFACTORY_API void RegOnRspError( HKSFT hKsft, pfOnRspError callback );
	KSFTAPIFACTORY_API void RegOnRtnOrder( HKSFT hKsft, pfOnRtnOrder callback );
	KSFTAPIFACTORY_API void RegOnRtnTrade( HKSFT hKsft, pfOnRtnTrade callback );
	KSFTAPIFACTORY_API void RegOnErrRtnOrderInsert( HKSFT hKsft, pfOnErrRtnOrderInsert callback );
	KSFTAPIFACTORY_API void RegOnErrRtnOrderAction( HKSFT hKsft, pfOnErrRtnOrderAction callback );
	KSFTAPIFACTORY_API void RegOnRtnInstrumentStatus( HKSFT hKsft, pfOnRtnInstrumentStatus callback );
	KSFTAPIFACTORY_API void RegOnRtnTradingNotice( HKSFT hKsft, pfOnRtnTradingNotice callback );
	KSFTAPIFACTORY_API void RegOnRtnErrorConditionalOrder( HKSFT hKsft, pfOnRtnErrorConditionalOrder callback );
	KSFTAPIFACTORY_API void RegOnRtnExecOrder( HKSFT hKsft, pfOnRtnExecOrder callback );
	KSFTAPIFACTORY_API void RegOnErrRtnExecOrderInsert( HKSFT hKsft, pfOnErrRtnExecOrderInsert callback );
	KSFTAPIFACTORY_API void RegOnErrRtnExecOrderAction( HKSFT hKsft, pfOnErrRtnExecOrderAction callback );
	KSFTAPIFACTORY_API void RegOnErrRtnForQuoteInsert( HKSFT hKsft, pfOnErrRtnForQuoteInsert callback );
	KSFTAPIFACTORY_API void RegOnRtnQuote( HKSFT hKsft, pfOnRtnQuote callback );
	KSFTAPIFACTORY_API void RegOnErrRtnQuoteInsert( HKSFT hKsft, pfOnErrRtnQuoteInsert callback );
	KSFTAPIFACTORY_API void RegOnErrRtnQuoteAction( HKSFT hKsft, pfOnErrRtnQuoteAction callback );
	KSFTAPIFACTORY_API void RegOnRspQryContractBank( HKSFT hKsft, pfOnRspQryContractBank callback );
	KSFTAPIFACTORY_API void RegOnRspQryParkedOrder( HKSFT hKsft, pfOnRspQryParkedOrder callback );
	KSFTAPIFACTORY_API void RegOnRspQryParkedOrderAction( HKSFT hKsft, pfOnRspQryParkedOrderAction callback );
	KSFTAPIFACTORY_API void RegOnRspQryTradingNotice( HKSFT hKsft, pfOnRspQryTradingNotice callback );
	KSFTAPIFACTORY_API void RegOnRspQryBrokerTradingParams( HKSFT hKsft, pfOnRspQryBrokerTradingParams callback );
	KSFTAPIFACTORY_API void RegOnRspQryBrokerTradingAlgos( HKSFT hKsft, pfOnRspQryBrokerTradingAlgos callback );
	KSFTAPIFACTORY_API void RegOnRtnFromBankToFutureByBank( HKSFT hKsft, pfOnRtnFromBankToFutureByBank callback );
	KSFTAPIFACTORY_API void RegOnRtnFromFutureToBankByBank( HKSFT hKsft, pfOnRtnFromFutureToBankByBank callback );
	KSFTAPIFACTORY_API void RegOnRtnRepealFromBankToFutureByBank( HKSFT hKsft, pfOnRtnRepealFromBankToFutureByBank callback );
	KSFTAPIFACTORY_API void RegOnRtnRepealFromFutureToBankByBank( HKSFT hKsft, pfOnRtnRepealFromFutureToBankByBank callback );
	KSFTAPIFACTORY_API void RegOnRtnFromBankToFutureByFuture( HKSFT hKsft, pfOnRtnFromBankToFutureByFuture callback );
	KSFTAPIFACTORY_API void RegOnRtnFromFutureToBankByFuture( HKSFT hKsft, pfOnRtnFromFutureToBankByFuture callback );
	KSFTAPIFACTORY_API void RegOnRtnRepealFromBankToFutureByFutureManual( HKSFT hKsft, pfOnRtnRepealFromBankToFutureByFutureManual callback );
	KSFTAPIFACTORY_API void RegOnRtnRepealFromFutureToBankByFutureManual( HKSFT hKsft, pfOnRtnRepealFromFutureToBankByFutureManual callback );
	KSFTAPIFACTORY_API void RegOnRtnQueryBankBalanceByFuture( HKSFT hKsft, pfOnRtnQueryBankBalanceByFuture callback );
	KSFTAPIFACTORY_API void RegOnErrRtnBankToFutureByFuture( HKSFT hKsft, pfOnErrRtnBankToFutureByFuture callback );
	KSFTAPIFACTORY_API void RegOnErrRtnFutureToBankByFuture( HKSFT hKsft, pfOnErrRtnFutureToBankByFuture callback );
	KSFTAPIFACTORY_API void RegOnErrRtnRepealBankToFutureByFutureManual( HKSFT hKsft, pfOnErrRtnRepealBankToFutureByFutureManual callback );
	KSFTAPIFACTORY_API void RegOnErrRtnRepealFutureToBankByFutureManual( HKSFT hKsft, pfOnErrRtnRepealFutureToBankByFutureManual callback );
	KSFTAPIFACTORY_API void RegOnErrRtnQueryBankBalanceByFuture( HKSFT hKsft, pfOnErrRtnQueryBankBalanceByFuture callback );
	KSFTAPIFACTORY_API void RegOnRtnRepealFromBankToFutureByFuture( HKSFT hKsft, pfOnRtnRepealFromBankToFutureByFuture callback );
	KSFTAPIFACTORY_API void RegOnRtnRepealFromFutureToBankByFuture( HKSFT hKsft, pfOnRtnRepealFromFutureToBankByFuture callback );
	KSFTAPIFACTORY_API void RegOnRspFromBankToFutureByFuture( HKSFT hKsft, pfOnRspFromBankToFutureByFuture callback );
	KSFTAPIFACTORY_API void RegOnRspFromFutureToBankByFuture( HKSFT hKsft, pfOnRspFromFutureToBankByFuture callback );
	KSFTAPIFACTORY_API void RegOnRspQueryBankAccountMoneyByFuture( HKSFT hKsft, pfOnRspQueryBankAccountMoneyByFuture callback );
	KSFTAPIFACTORY_API void RegOnRtnOpenAccountByBank( HKSFT hKsft, pfOnRtnOpenAccountByBank callback );
	KSFTAPIFACTORY_API void RegOnRtnCancelAccountByBank( HKSFT hKsft, pfOnRtnCancelAccountByBank callback );
	KSFTAPIFACTORY_API void RegOnRtnChangeAccountByBank( HKSFT hKsft, pfOnRtnChangeAccountByBank callback );
	KSFTAPIFACTORY_API void RegOnRspQryInvestorOpenPosition( HKSFT hKsft, pfOnRspQryInvestorOpenPosition callback );
	KSFTAPIFACTORY_API void RegOnRspQryInvestorOpenCombinePosition( HKSFT hKsft, pfOnRspQryInvestorOpenCombinePosition callback );
	KSFTAPIFACTORY_API void RegOnRspBulkCancelOrder( HKSFT hKsft, pfOnRspBulkCancelOrder callback );
//------------------------------------------------------------------------------------------------------------------------
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
