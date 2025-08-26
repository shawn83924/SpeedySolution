//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CTPAPIFACTORY_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CTPAPIFACTORY_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef CTPAPIFACTORY_EXPORTS
#define CTPAPIFACTORY_API __declspec(dllexport)
#else
#define CTPAPIFACTORY_API __declspec(dllimport)
#endif
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if defined(__S8_CTP)
#include "include_CTP/ThostFtdcUserApiDataType.h"
#include "include_CTP/ThostFtdcUserApiStruct.h"
#elif defined(__S8_HS)
#include "include_HS/ThostFtdcUserApiDataType.h"
#include "include_HS/ThostFtdcUserApiStruct.h"
#elif defined(__S8_KN)
#include "include_KN/ThostFtdcUserApiDataType.h"
#include "include_KN/ThostFtdcUserApiStruct.h"
#endif
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "CTPAPIDynamic.h"
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern "C"
{
	///< Export CThostFtdcTraderApi 50 method into C style.
	CTPAPIFACTORY_API HCTP  CreateCTPTraderApi( void* Owner, char* strDllName );
	CTPAPIFACTORY_API void  ReleaseCTPTraderApi( HCTP hCtp );
	CTPAPIFACTORY_API void  Init( HCTP hCtp );
	CTPAPIFACTORY_API const char*  GetTradingDay( HCTP hCtp );
	CTPAPIFACTORY_API void  RegisterFront( HCTP hCtp, char *pszFrontAddress);
	CTPAPIFACTORY_API void  RegisterNameServer( HCTP hCtp, char *pszNsAddress);
	CTPAPIFACTORY_API void  SubscribePrivateTopic( HCTP hCtp,THOST_TE_RESUME_TYPE nResumeType);
	CTPAPIFACTORY_API void  SubscribePublicTopic( HCTP hCtp, THOST_TE_RESUME_TYPE nResumeType);
	CTPAPIFACTORY_API int   ReqAuthenticate( HCTP hCtp, CThostFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID);
	CTPAPIFACTORY_API int   ReqUserLogin( HCTP hCtp, CThostFtdcReqUserLoginField *pReqUserLoginField, int nRequestID);
	CTPAPIFACTORY_API int   ReqUserLogout( HCTP hCtp, CThostFtdcUserLogoutField *pUserLogout, int nRequestID);
	CTPAPIFACTORY_API int  ReqUserPasswordUpdate(HCTP hCtp, CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID);
	CTPAPIFACTORY_API int  ReqTradingAccountPasswordUpdate(HCTP hCtp, CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, int nRequestID);
	CTPAPIFACTORY_API int  ReqOrderInsert(HCTP hCtp, CThostFtdcInputOrderField *pInputOrder, int nRequestID);
	CTPAPIFACTORY_API int  ReqParkedOrderInsert(HCTP hCtp, CThostFtdcParkedOrderField *pParkedOrder, int nRequestID);
	CTPAPIFACTORY_API int  ReqParkedOrderAction(HCTP hCtp, CThostFtdcParkedOrderActionField *pParkedOrderAction, int nRequestID);
	CTPAPIFACTORY_API int  ReqOrderAction(HCTP hCtp, CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID);
	CTPAPIFACTORY_API int  ReqQueryMaxOrderVolume(HCTP hCtp, CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, int nRequestID);
	CTPAPIFACTORY_API int  ReqSettlementInfoConfirm(HCTP hCtp, CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, int nRequestID);
	CTPAPIFACTORY_API int  ReqRemoveParkedOrder(HCTP hCtp, CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, int nRequestID);
	CTPAPIFACTORY_API int  ReqRemoveParkedOrderAction(HCTP hCtp, CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryOrder(HCTP hCtp, CThostFtdcQryOrderField *pQryOrder, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryTrade(HCTP hCtp, CThostFtdcQryTradeField *pQryTrade, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryInvestorPosition(HCTP hCtp, CThostFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryTradingAccount(HCTP hCtp, CThostFtdcQryTradingAccountField *pQryTradingAccount, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryInvestor(HCTP hCtp, CThostFtdcQryInvestorField *pQryInvestor, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryTradingCode(HCTP hCtp, CThostFtdcQryTradingCodeField *pQryTradingCode, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryInstrumentMarginRate(HCTP hCtp, CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryInstrumentCommissionRate(HCTP hCtp, CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryExchange(HCTP hCtp, CThostFtdcQryExchangeField *pQryExchange, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryInstrument(HCTP hCtp, CThostFtdcQryInstrumentField *pQryInstrument, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryDepthMarketData(HCTP hCtp, CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, int nRequestID);
	CTPAPIFACTORY_API int  ReqQrySettlementInfo(HCTP hCtp, CThostFtdcQrySettlementInfoField *pQrySettlementInfo, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryTransferBank(HCTP hCtp, CThostFtdcQryTransferBankField *pQryTransferBank, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryInvestorPositionDetail(HCTP hCtp, CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryNotice(HCTP hCtp, CThostFtdcQryNoticeField *pQryNotice, int nRequestID);
	CTPAPIFACTORY_API int  ReqQrySettlementInfoConfirm(HCTP hCtp, CThostFtdcQrySettlementInfoConfirmField *pQrySettlementInfoConfirm, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryInvestorPositionCombineDetail(HCTP hCtp, CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorPositionCombineDetail, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryCFMMCTradingAccountKey(HCTP hCtp, CThostFtdcQryCFMMCTradingAccountKeyField *pQryCFMMCTradingAccountKey, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryEWarrantOffset(HCTP hCtp, CThostFtdcQryEWarrantOffsetField *pQryEWarrantOffset, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryTransferSerial(HCTP hCtp, CThostFtdcQryTransferSerialField *pQryTransferSerial, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryAccountregister(HCTP hCtp, CThostFtdcQryAccountregisterField *pQryAccountregister, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryContractBank(HCTP hCtp, CThostFtdcQryContractBankField *pQryContractBank, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryParkedOrder(HCTP hCtp, CThostFtdcQryParkedOrderField *pQryParkedOrder, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryParkedOrderAction(HCTP hCtp, CThostFtdcQryParkedOrderActionField *pQryParkedOrderAction, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryTradingNotice(HCTP hCtp, CThostFtdcQryTradingNoticeField *pQryTradingNotice, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryBrokerTradingParams(HCTP hCtp, CThostFtdcQryBrokerTradingParamsField *pQryBrokerTradingParams, int nRequestID);
	CTPAPIFACTORY_API int  ReqQryBrokerTradingAlgos(HCTP hCtp, CThostFtdcQryBrokerTradingAlgosField *pQryBrokerTradingAlgos, int nRequestID);
	CTPAPIFACTORY_API int  ReqFromBankToFutureByFuture(HCTP hCtp, CThostFtdcReqTransferField *pReqTransfer, int nRequestID);
	CTPAPIFACTORY_API int  ReqFromFutureToBankByFuture(HCTP hCtp, CThostFtdcReqTransferField *pReqTransfer, int nRequestID);
	CTPAPIFACTORY_API int  ReqQueryBankAccountMoneyByFuture(HCTP hCtp, CThostFtdcReqQueryAccountField *pReqQueryAccount, int nRequestID);
	///< Functions to regist callback function 
	CTPAPIFACTORY_API void  RegOnFrontConnected( HCTP hCtp, pfOnFrontConnected callback );
	CTPAPIFACTORY_API void  RegOnFrontDisconnected( HCTP hCtp, pfOnFrontDisconnected callback );
	CTPAPIFACTORY_API void  RegOnRspUserLogin( HCTP hCtp, pfOnRspUserLogin callback );
	CTPAPIFACTORY_API void  RegOnRspQrySettlementInfo( HCTP hCtp, pfOnRspQrySettlementInfo callback );
	CTPAPIFACTORY_API void  RegOnRspSettlementInfoConfirm( HCTP hCtp, pfOnRspSettlementInfoConfirm callback );
	CTPAPIFACTORY_API void  RegOnRspOrderInsert( HCTP hCtp, pfOnRspOrderInsert callback );
	CTPAPIFACTORY_API void  RegOnErrRtnOrderInsert( HCTP hCtp, pfOnErrRtnOrderInsert callback );
	CTPAPIFACTORY_API void  RegOnRspOrderAction( HCTP hCtp, pfOnRspOrderAction callback );
	CTPAPIFACTORY_API void  RegOnErrRtnOrderAction( HCTP hCtp, pfOnErrRtnOrderAction callback );
	CTPAPIFACTORY_API void  RegOnRspQryOrder( HCTP hCtp, pfOnRspQryOrder callback );
	CTPAPIFACTORY_API void  RegOnRspQryExchange( HCTP hCtp, pfOnRspQryExchange callback );
	CTPAPIFACTORY_API void  RegOnRspQryInstrument( HCTP hCtp, pfOnRspQryInstrument callback );
	CTPAPIFACTORY_API void  RegOnRspQryInvestorPosition( HCTP hCtp, pfOnRspQryInvestorPosition callback );
	CTPAPIFACTORY_API void  RegOnRtnOrder( HCTP hCtp, pfOnRtnOrder callback );
	CTPAPIFACTORY_API void  RegOnRtnTrade( HCTP hCtp, pfOnRtnTrade callback );
	CTPAPIFACTORY_API void  RegOnRspError( HCTP hCtp, pfOnRspError callback );
	///< Functions to regist callback function (Not imp )
	CTPAPIFACTORY_API void RegOnHeartBeatWarning( HCTP hCtp, pfOnHeartBeatWarning callback );
	CTPAPIFACTORY_API void RegOnRspAuthenticate( HCTP hCtp, pfOnRspAuthenticate callback );
	CTPAPIFACTORY_API void RegOnRspUserLogout( HCTP hCtp, pfOnRspUserLogout callback );
	CTPAPIFACTORY_API void RegOnRspUserPasswordUpdate( HCTP hCtp, pfOnRspUserPasswordUpdate callback );
	CTPAPIFACTORY_API void RegOnRspTradingAccountPasswordUpdate( HCTP hCtp, pfOnRspTradingAccountPasswordUpdate callback );
	CTPAPIFACTORY_API void RegOnRspParkedOrderInsert( HCTP hCtp, pfOnRspParkedOrderInsert callback );
	CTPAPIFACTORY_API void RegOnRspParkedOrderAction( HCTP hCtp, pfOnRspParkedOrderAction callback );
	CTPAPIFACTORY_API void RegOnRspQueryMaxOrderVolume( HCTP hCtp, pfOnRspQueryMaxOrderVolume callback );
	CTPAPIFACTORY_API void RegOnRspRemoveParkedOrder( HCTP hCtp, pfOnRspRemoveParkedOrder callback );
	CTPAPIFACTORY_API void RegOnRspRemoveParkedOrderAction( HCTP hCtp, pfOnRspRemoveParkedOrderAction callback );
	CTPAPIFACTORY_API void RegOnRspQryTrade( HCTP hCtp, pfOnRspQryTrade callback );
	CTPAPIFACTORY_API void RegOnRspQryTradingAccount( HCTP hCtp, pfOnRspQryTradingAccount callback );
	CTPAPIFACTORY_API void RegOnRspQryInvestor( HCTP hCtp, pfOnRspQryInvestor callback );
	CTPAPIFACTORY_API void RegOnRspQryTradingCode( HCTP hCtp, pfOnRspQryTradingCode callback );
	CTPAPIFACTORY_API void RegOnRspQryInstrumentMarginRate( HCTP hCtp, pfOnRspQryInstrumentMarginRate callback );
	CTPAPIFACTORY_API void RegOnRspQryInstrumentCommissionRate( HCTP hCtp, pfOnRspQryInstrumentCommissionRate callback );
	CTPAPIFACTORY_API void RegOnRspQryDepthMarketData( HCTP hCtp, pfOnRspQryDepthMarketData callback );
	CTPAPIFACTORY_API void RegOnRspQryTransferBank( HCTP hCtp, pfOnRspQryTransferBank callback );
	CTPAPIFACTORY_API void RegOnRspQryInvestorPositionDetail( HCTP hCtp, pfOnRspQryInvestorPositionDetail callback );
	CTPAPIFACTORY_API void RegOnRspQryNotice( HCTP hCtp, pfOnRspQryNotice callback );
	CTPAPIFACTORY_API void RegOnRspQrySettlementInfoConfirm( HCTP hCtp, pfOnRspQrySettlementInfoConfirm callback );
	CTPAPIFACTORY_API void RegOnRspQryInvestorPositionCombineDetail( HCTP hCtp, pfOnRspQryInvestorPositionCombineDetail callback );
	CTPAPIFACTORY_API void RegOnRspQryCFMMCTradingAccountKey( HCTP hCtp, pfOnRspQryCFMMCTradingAccountKey callback );
	CTPAPIFACTORY_API void RegOnRspQryEWarrantOffset( HCTP hCtp, pfOnRspQryEWarrantOffset callback );
	CTPAPIFACTORY_API void RegOnRspQryTransferSerial( HCTP hCtp, pfOnRspQryTransferSerial callback );
	CTPAPIFACTORY_API void RegOnRspQryAccountregister( HCTP hCtp, pfOnRspQryAccountregister callback );
	CTPAPIFACTORY_API void RegOnRtnInstrumentStatus( HCTP hCtp, pfOnRtnInstrumentStatus callback );
	CTPAPIFACTORY_API void RegOnRtnTradingNotice( HCTP hCtp, pfOnRtnTradingNotice callback );
	CTPAPIFACTORY_API void RegOnRtnErrorConditionalOrder( HCTP hCtp, pfOnRtnErrorConditionalOrder callback );
	CTPAPIFACTORY_API void RegOnRspQryContractBank( HCTP hCtp, pfOnRspQryContractBank callback );
	CTPAPIFACTORY_API void RegOnRspQryParkedOrder( HCTP hCtp, pfOnRspQryParkedOrder callback );
	CTPAPIFACTORY_API void RegOnRspQryParkedOrderAction( HCTP hCtp, pfOnRspQryParkedOrderAction callback );
	CTPAPIFACTORY_API void RegOnRspQryTradingNotice( HCTP hCtp, pfOnRspQryTradingNotice callback );
	CTPAPIFACTORY_API void RegOnRspQryBrokerTradingParams( HCTP hCtp, pfOnRspQryBrokerTradingParams callback );
	CTPAPIFACTORY_API void RegOnRspQryBrokerTradingAlgos( HCTP hCtp, pfOnRspQryBrokerTradingAlgos callback );
	CTPAPIFACTORY_API void RegOnRtnFromBankToFutureByBank( HCTP hCtp, pfOnRtnFromBankToFutureByBank callback );
	CTPAPIFACTORY_API void RegOnRtnFromFutureToBankByBank( HCTP hCtp, pfOnRtnFromFutureToBankByBank callback );
	CTPAPIFACTORY_API void RegOnRtnRepealFromBankToFutureByBank( HCTP hCtp, pfOnRtnRepealFromBankToFutureByBank callback );
	CTPAPIFACTORY_API void RegOnRtnRepealFromFutureToBankByBank( HCTP hCtp, pfOnRtnRepealFromFutureToBankByBank callback );
	CTPAPIFACTORY_API void RegOnRtnFromBankToFutureByFuture( HCTP hCtp, pfOnRtnFromBankToFutureByFuture callback );
	CTPAPIFACTORY_API void RegOnRtnFromFutureToBankByFuture( HCTP hCtp, pfOnRtnFromFutureToBankByFuture callback );
	CTPAPIFACTORY_API void RegOnRtnRepealFromBankToFutureByFutureManual( HCTP hCtp, pfOnRtnRepealFromBankToFutureByFutureManual callback );
	CTPAPIFACTORY_API void RegOnRtnRepealFromFutureToBankByFutureManual( HCTP hCtp, pfOnRtnRepealFromFutureToBankByFutureManual callback );
	CTPAPIFACTORY_API void RegOnRtnQueryBankBalanceByFuture( HCTP hCtp, pfOnRtnQueryBankBalanceByFuture callback );
	CTPAPIFACTORY_API void RegOnErrRtnBankToFutureByFuture( HCTP hCtp, pfOnErrRtnBankToFutureByFuture callback );
	CTPAPIFACTORY_API void RegOnErrRtnFutureToBankByFuture( HCTP hCtp, pfOnErrRtnFutureToBankByFuture callback );
	CTPAPIFACTORY_API void RegOnErrRtnRepealBankToFutureByFutureManual( HCTP hCtp, pfOnErrRtnRepealBankToFutureByFutureManual callback );
	CTPAPIFACTORY_API void RegOnErrRtnRepealFutureToBankByFutureManual( HCTP hCtp, pfOnErrRtnRepealFutureToBankByFutureManual callback );
	CTPAPIFACTORY_API void RegOnErrRtnQueryBankBalanceByFuture( HCTP hCtp, pfOnErrRtnQueryBankBalanceByFuture callback );
	CTPAPIFACTORY_API void RegOnRtnRepealFromBankToFutureByFuture( HCTP hCtp, pfOnRtnRepealFromBankToFutureByFuture callback );
	CTPAPIFACTORY_API void RegOnRtnRepealFromFutureToBankByFuture( HCTP hCtp, pfOnRtnRepealFromFutureToBankByFuture callback );
	CTPAPIFACTORY_API void RegOnRspFromBankToFutureByFuture( HCTP hCtp, pfOnRspFromBankToFutureByFuture callback );
	CTPAPIFACTORY_API void RegOnRspFromFutureToBankByFuture( HCTP hCtp, pfOnRspFromFutureToBankByFuture callback );
	CTPAPIFACTORY_API void RegOnRspQueryBankAccountMoneyByFuture( HCTP hCtp, pfOnRspQueryBankAccountMoneyByFuture callback );
	CTPAPIFACTORY_API void RegOnRtnOpenAccountByBank( HCTP hCtp, pfOnRtnOpenAccountByBank callback );
	CTPAPIFACTORY_API void RegOnRtnCancelAccountByBank( HCTP hCtp, pfOnRtnCancelAccountByBank callback );
	CTPAPIFACTORY_API void RegOnRtnChangeAccountByBank( HCTP hCtp, pfOnRtnChangeAccountByBank callback );
};
