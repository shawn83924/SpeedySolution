//-------------------------------------------------------------------------------------------------------------------------
#ifndef CTPAPIdynamic_H
#define CTPAPIdynamic_H
//-------------------------------------------------------------------------------------------------------------------------
#if defined(__S8_CTP)
#include "include_CTP/ThostFtdcUserApiDataType.h"
#include "include_CTP/ThostFtdcUserApiStruct.h"
#elif defined(__S8_HS)
#include "include_HS/ThostFtdcUserApiDataType.h"
#include "include_HS/ThostFtdcUserApiStruct.h"
#elif defined(__S8_KN)
#include "include_KN/ThostFtdcUserApiDataType.h"
#include "include_KN/ThostFtdcUserApiStruct.h"
#else
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
#endif

#include "UFC.h"
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef void* HCTP;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern "C"
{
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Define all callback functions.
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef void (__cdecl *pfOnFrontConnected)( void* Owner );
typedef void (__cdecl *pfOnFrontDisconnected)( void* Owner, int nReason);
typedef void (__cdecl *pfOnRspUserLogin)(void* Owner,CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);///< Imp
typedef void (__cdecl *pfOnRspQrySettlementInfo)(void* Owner,CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspSettlementInfoConfirm)(void* Owner,CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspOrderInsert)(void* Owner,CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnErrRtnOrderInsert)(void* Owner,CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
typedef void (__cdecl *pfOnRspOrderAction)(void* Owner,CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnErrRtnOrderAction)(void* Owner,CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);
typedef void (__cdecl *pfOnRspQryOrder)(void* Owner,CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryExchange)(void* Owner,CThostFtdcExchangeField *pExchange, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryInstrument)(void* Owner,CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryInvestorPosition)(void* Owner,CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspError)(void* Owner,CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRtnOrder)(void* Owner,CThostFtdcOrderField *pOrder);
typedef void (__cdecl *pfOnRtnTrade)(void* Owner,CThostFtdcTradeField *pTrade);
typedef void (__cdecl *pfOnHeartBeatWarning)(void* Owner,int nTimeLapse);
typedef void (__cdecl *pfOnRspAuthenticate)(void* Owner,CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspUserLogout)(void* Owner,CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspUserPasswordUpdate)(void* Owner,CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspTradingAccountPasswordUpdate)(void* Owner,CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspParkedOrderInsert)(void* Owner,CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspParkedOrderAction)(void* Owner,CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQueryMaxOrderVolume)(void* Owner,CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspRemoveParkedOrder)(void* Owner,CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspRemoveParkedOrderAction)(void* Owner,CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryTrade)(void* Owner,CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryTradingAccount)(void* Owner,CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryInvestor)(void* Owner,CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryTradingCode)(void* Owner,CThostFtdcTradingCodeField *pTradingCode, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryInstrumentMarginRate)(void* Owner,CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryInstrumentCommissionRate)(void* Owner,CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryDepthMarketData)(void* Owner,CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryTransferBank)(void* Owner,CThostFtdcTransferBankField *pTransferBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryInvestorPositionDetail)(void* Owner,CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryNotice)(void* Owner,CThostFtdcNoticeField *pNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQrySettlementInfoConfirm)(void* Owner,CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryInvestorPositionCombineDetail)(void* Owner,CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryCFMMCTradingAccountKey)(void* Owner,CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryEWarrantOffset)(void* Owner,CThostFtdcEWarrantOffsetField *pEWarrantOffset, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryTransferSerial)(void* Owner,CThostFtdcTransferSerialField *pTransferSerial, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryAccountregister)(void* Owner,CThostFtdcAccountregisterField *pAccountregister, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRtnInstrumentStatus)(void* Owner,CThostFtdcInstrumentStatusField *pInstrumentStatus);
typedef void (__cdecl *pfOnRtnTradingNotice)(void* Owner,CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo);
typedef void (__cdecl *pfOnRtnErrorConditionalOrder)(void* Owner,CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder);
typedef void (__cdecl *pfOnRspQryContractBank)(void* Owner,CThostFtdcContractBankField *pContractBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryParkedOrder)(void* Owner,CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryParkedOrderAction)(void* Owner,CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryTradingNotice)(void* Owner,CThostFtdcTradingNoticeField *pTradingNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryBrokerTradingParams)(void* Owner,CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryBrokerTradingAlgos)(void* Owner,CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRtnFromBankToFutureByBank)(void* Owner,CThostFtdcRspTransferField *pRspTransfer);
typedef void (__cdecl *pfOnRtnFromFutureToBankByBank)(void* Owner,CThostFtdcRspTransferField *pRspTransfer);
typedef void (__cdecl *pfOnRtnRepealFromBankToFutureByBank)(void* Owner,CThostFtdcRspRepealField *pRspRepeal);
typedef void (__cdecl *pfOnRtnRepealFromFutureToBankByBank)(void* Owner,CThostFtdcRspRepealField *pRspRepeal);
typedef void (__cdecl *pfOnRtnFromBankToFutureByFuture)(void* Owner,CThostFtdcRspTransferField *pRspTransfer);
typedef void (__cdecl *pfOnRtnFromFutureToBankByFuture)(void* Owner,CThostFtdcRspTransferField *pRspTransfer);
typedef void (__cdecl *pfOnRtnRepealFromBankToFutureByFutureManual)(void* Owner,CThostFtdcRspRepealField *pRspRepeal);
typedef void (__cdecl *pfOnRtnRepealFromFutureToBankByFutureManual)(void* Owner,CThostFtdcRspRepealField *pRspRepeal);
typedef void (__cdecl *pfOnRtnQueryBankBalanceByFuture)(void* Owner,CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount);
typedef void (__cdecl *pfOnErrRtnBankToFutureByFuture)(void* Owner,CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo);
typedef void (__cdecl *pfOnErrRtnFutureToBankByFuture)(void* Owner,CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo);
typedef void (__cdecl *pfOnErrRtnRepealBankToFutureByFutureManual)(void* Owner,CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo);
typedef void (__cdecl *pfOnErrRtnRepealFutureToBankByFutureManual)(void* Owner,CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo);
typedef void (__cdecl *pfOnErrRtnQueryBankBalanceByFuture)(void* Owner,CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo);
typedef void (__cdecl *pfOnRtnRepealFromBankToFutureByFuture)(void* Owner,CThostFtdcRspRepealField *pRspRepeal);
typedef void (__cdecl *pfOnRtnRepealFromFutureToBankByFuture)(void* Owner,CThostFtdcRspRepealField *pRspRepeal);
typedef void (__cdecl *pfOnRspFromBankToFutureByFuture)(void* Owner,CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspFromFutureToBankByFuture)(void* Owner,CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQueryBankAccountMoneyByFuture)(void* Owner,CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRtnOpenAccountByBank)(void* Owner,CThostFtdcOpenAccountField *pOpenAccount);
typedef void (__cdecl *pfOnRtnCancelAccountByBank)(void* Owner,CThostFtdcCancelAccountField *pCancelAccount);
typedef void (__cdecl *pfOnRtnChangeAccountByBank)(void* Owner,CThostFtdcChangeAccountField *pChangeAccount);
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Define all export function pointors
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef HCTP (__cdecl *lpCreateCTPTraderApi)( void* Owner, char* strDllName );
typedef void (__cdecl *lpReleaseCTPTraderApi)( HCTP hCtp );
typedef void (__cdecl *lpInit)( HCTP hCtp );
typedef const char* (__cdecl *lpGetTradingDay)( HCTP hCtp );
typedef void (__cdecl *lpRegisterFront)( HCTP hCtp, char *pszFrontAddress);
typedef void (__cdecl *lpRegisterNameServer)( HCTP hCtp, char *pszNsAddress);
typedef void (__cdecl *lpSubscribePrivateTopic)( HCTP hCtp,THOST_TE_RESUME_TYPE nResumeType);
typedef void (__cdecl *lpSubscribePublicTopic)( HCTP hCtp, THOST_TE_RESUME_TYPE nResumeType);
typedef int  (__cdecl *lpReqAuthenticate)( HCTP hCtp, CThostFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID);
typedef int  (__cdecl *lpReqUserLogin)( HCTP hCtp, CThostFtdcReqUserLoginField *pReqUserLoginField, int nRequestID);
typedef int  (__cdecl *lpReqUserLogout)( HCTP hCtp, CThostFtdcUserLogoutField *pUserLogout, int nRequestID);
typedef int  (__cdecl *lpReqUserPasswordUpdate)(HCTP hCtp, CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID);
typedef int  (__cdecl *lpReqTradingAccountPasswordUpdate)(HCTP hCtp, CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, int nRequestID);
typedef int  (__cdecl *lpReqOrderInsert)(HCTP hCtp, CThostFtdcInputOrderField *pInputOrder, int nRequestID);
typedef int  (__cdecl *lpReqParkedOrderInsert)(HCTP hCtp, CThostFtdcParkedOrderField *pParkedOrder, int nRequestID);
typedef int  (__cdecl *lpReqParkedOrderAction)(HCTP hCtp, CThostFtdcParkedOrderActionField *pParkedOrderAction, int nRequestID);
typedef int  (__cdecl *lpReqOrderAction)(HCTP hCtp, CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID);
typedef int  (__cdecl *lpReqQueryMaxOrderVolume)(HCTP hCtp, CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, int nRequestID);
typedef int  (__cdecl *lpReqSettlementInfoConfirm)(HCTP hCtp, CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, int nRequestID);
typedef int  (__cdecl *lpReqRemoveParkedOrder)(HCTP hCtp, CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, int nRequestID);
typedef int  (__cdecl *lpReqRemoveParkedOrderAction)(HCTP hCtp, CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, int nRequestID);
typedef int  (__cdecl *lpReqQryOrder)(HCTP hCtp, CThostFtdcQryOrderField *pQryOrder, int nRequestID);
typedef int  (__cdecl *lpReqQryTrade)(HCTP hCtp, CThostFtdcQryTradeField *pQryTrade, int nRequestID);
typedef int  (__cdecl *lpReqQryInvestorPosition)(HCTP hCtp, CThostFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID);
typedef int  (__cdecl *lpReqQryTradingAccount)(HCTP hCtp, CThostFtdcQryTradingAccountField *pQryTradingAccount, int nRequestID);
typedef int  (__cdecl *lpReqQryInvestor)(HCTP hCtp, CThostFtdcQryInvestorField *pQryInvestor, int nRequestID);
typedef int  (__cdecl *lpReqQryTradingCode)(HCTP hCtp, CThostFtdcQryTradingCodeField *pQryTradingCode, int nRequestID);
typedef int  (__cdecl *lpReqQryInstrumentMarginRate)(HCTP hCtp, CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID);
typedef int  (__cdecl *lpReqQryInstrumentCommissionRate)(HCTP hCtp, CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate, int nRequestID);
typedef int  (__cdecl *lpReqQryExchange)(HCTP hCtp, CThostFtdcQryExchangeField *pQryExchange, int nRequestID);
typedef int  (__cdecl *lpReqQryInstrument)(HCTP hCtp, CThostFtdcQryInstrumentField *pQryInstrument, int nRequestID);
typedef int  (__cdecl *lpReqQryDepthMarketData)(HCTP hCtp, CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, int nRequestID);
typedef int  (__cdecl *lpReqQrySettlementInfo)(HCTP hCtp, CThostFtdcQrySettlementInfoField *pQrySettlementInfo, int nRequestID);
typedef int  (__cdecl *lpReqQryTransferBank)(HCTP hCtp, CThostFtdcQryTransferBankField *pQryTransferBank, int nRequestID);
typedef int  (__cdecl *lpReqQryInvestorPositionDetail)(HCTP hCtp, CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, int nRequestID);
typedef int  (__cdecl *lpReqQryNotice)(HCTP hCtp, CThostFtdcQryNoticeField *pQryNotice, int nRequestID);
typedef int  (__cdecl *lpReqQrySettlementInfoConfirm)(HCTP hCtp, CThostFtdcQrySettlementInfoConfirmField *pQrySettlementInfoConfirm, int nRequestID);
typedef int  (__cdecl *lpReqQryInvestorPositionCombineDetail)(HCTP hCtp, CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorPositionCombineDetail, int nRequestID);
typedef int  (__cdecl *lpReqQryCFMMCTradingAccountKey)(HCTP hCtp, CThostFtdcQryCFMMCTradingAccountKeyField *pQryCFMMCTradingAccountKey, int nRequestID);
typedef int  (__cdecl *lpReqQryEWarrantOffset)(HCTP hCtp, CThostFtdcQryEWarrantOffsetField *pQryEWarrantOffset, int nRequestID);
typedef int  (__cdecl *lpReqQryTransferSerial)(HCTP hCtp, CThostFtdcQryTransferSerialField *pQryTransferSerial, int nRequestID);
typedef int  (__cdecl *lpReqQryAccountregister)(HCTP hCtp, CThostFtdcQryAccountregisterField *pQryAccountregister, int nRequestID);
typedef int  (__cdecl *lpReqQryContractBank)(HCTP hCtp, CThostFtdcQryContractBankField *pQryContractBank, int nRequestID);
typedef int  (__cdecl *lpReqQryParkedOrder)(HCTP hCtp, CThostFtdcQryParkedOrderField *pQryParkedOrder, int nRequestID);
typedef int  (__cdecl *lpReqQryParkedOrderAction)(HCTP hCtp, CThostFtdcQryParkedOrderActionField *pQryParkedOrderAction, int nRequestID);
typedef int  (__cdecl *lpReqQryTradingNotice)(HCTP hCtp, CThostFtdcQryTradingNoticeField *pQryTradingNotice, int nRequestID);
typedef int  (__cdecl *lpReqQryBrokerTradingParams)(HCTP hCtp, CThostFtdcQryBrokerTradingParamsField *pQryBrokerTradingParams, int nRequestID);
typedef int  (__cdecl *lpReqQryBrokerTradingAlgos)(HCTP hCtp, CThostFtdcQryBrokerTradingAlgosField *pQryBrokerTradingAlgos, int nRequestID);
typedef int  (__cdecl *lpReqFromBankToFutureByFuture)(HCTP hCtp, CThostFtdcReqTransferField *pReqTransfer, int nRequestID);
typedef int  (__cdecl *lpReqFromFutureToBankByFuture)(HCTP hCtp, CThostFtdcReqTransferField *pReqTransfer, int nRequestID);
typedef int  (__cdecl *lpReqQueryBankAccountMoneyByFuture)(HCTP hCtp, CThostFtdcReqQueryAccountField *pReqQueryAccount, int nRequestID);
///< implement RegOnXXXXXX callback function
typedef void (__cdecl *lpRegOnFrontConnected)                         ( HCTP hCtp, pfOnFrontConnected callback );
typedef void (__cdecl *lpRegOnFrontDisconnected)                      ( HCTP hCtp, pfOnFrontDisconnected callback );
typedef void (__cdecl *lpRegOnRspUserLogin)                           ( HCTP hCtp, pfOnRspUserLogin callback );
typedef void (__cdecl *lpRegOnRspQrySettlementInfo)                   ( HCTP hCtp, pfOnRspQrySettlementInfo callback );
typedef void (__cdecl *lpRegOnRspSettlementInfoConfirm)               ( HCTP hCtp, pfOnRspSettlementInfoConfirm callback );
typedef void (__cdecl *lpRegOnRspOrderInsert)                         ( HCTP hCtp, pfOnRspOrderInsert callback );
typedef void (__cdecl *lpRegOnErrRtnOrderInsert)                      ( HCTP hCtp, pfOnErrRtnOrderInsert callback );
typedef void (__cdecl *lpRegOnRspOrderAction)                         ( HCTP hCtp, pfOnRspOrderAction callback );
typedef void (__cdecl *lpRegOnErrRtnOrderAction)                      ( HCTP hCtp, pfOnErrRtnOrderAction callback );
typedef void (__cdecl *lpRegOnRspQryOrder)                            ( HCTP hCtp, pfOnRspQryOrder callback );
typedef void (__cdecl *lpRegOnRspQryExchange)                         ( HCTP hCtp, pfOnRspQryExchange callback );
typedef void (__cdecl *lpRegOnRspQryInstrument)                       ( HCTP hCtp, pfOnRspQryInstrument callback );
typedef void (__cdecl *lpRegOnRspQryInvestorPosition)                 ( HCTP hCtp, pfOnRspQryInvestorPosition callback );
typedef void (__cdecl *lpRegOnRtnOrder)                               ( HCTP hCtp, pfOnRtnOrder callback );
typedef void (__cdecl *lpRegOnRtnTrade)                               ( HCTP hCtp, pfOnRtnTrade callback );
typedef void (__cdecl *lpRegOnRspError)                               ( HCTP hCtp, pfOnRspError callback );
///< Not implement RegXXXXXX
typedef void (__cdecl *lpRegOnHeartBeatWarning)                       ( HCTP hCtp, pfOnHeartBeatWarning callback );
typedef void (__cdecl *lpRegOnRspAuthenticate)                        ( HCTP hCtp, pfOnRspAuthenticate callback );
typedef void (__cdecl *lpRegOnRspUserLogout)                          ( HCTP hCtp, pfOnRspUserLogout callback );
typedef void (__cdecl *lpRegOnRspUserPasswordUpdate)                  ( HCTP hCtp, pfOnRspUserPasswordUpdate callback );
typedef void (__cdecl *lpRegOnRspTradingAccountPasswordUpdate)        ( HCTP hCtp, pfOnRspTradingAccountPasswordUpdate callback );
typedef void (__cdecl *lpRegOnRspParkedOrderInsert)                   ( HCTP hCtp, pfOnRspParkedOrderInsert callback );
typedef void (__cdecl *lpRegOnRspParkedOrderAction)                   ( HCTP hCtp, pfOnRspParkedOrderAction callback );
typedef void (__cdecl *lpRegOnRspQueryMaxOrderVolume)                 ( HCTP hCtp, pfOnRspQueryMaxOrderVolume callback );
typedef void (__cdecl *lpRegOnRspRemoveParkedOrder)                   ( HCTP hCtp, pfOnRspRemoveParkedOrder callback );
typedef void (__cdecl *lpRegOnRspRemoveParkedOrderAction)             ( HCTP hCtp, pfOnRspRemoveParkedOrderAction callback );
typedef void (__cdecl *lpRegOnRspQryTrade)                            ( HCTP hCtp, pfOnRspQryTrade callback );
typedef void (__cdecl *lpRegOnRspQryTradingAccount)                   ( HCTP hCtp, pfOnRspQryTradingAccount callback );
typedef void (__cdecl *lpRegOnRspQryInvestor)                         ( HCTP hCtp, pfOnRspQryInvestor callback );
typedef void (__cdecl *lpRegOnRspQryTradingCode)                      ( HCTP hCtp, pfOnRspQryTradingCode callback );
typedef void (__cdecl *lpRegOnRspQryInstrumentMarginRate)             ( HCTP hCtp, pfOnRspQryInstrumentMarginRate callback );
typedef void (__cdecl *lpRegOnRspQryInstrumentCommissionRate)         ( HCTP hCtp, pfOnRspQryInstrumentCommissionRate callback );
typedef void (__cdecl *lpRegOnRspQryDepthMarketData)                  ( HCTP hCtp, pfOnRspQryDepthMarketData callback );
typedef void (__cdecl *lpRegOnRspQryTransferBank)                     ( HCTP hCtp, pfOnRspQryTransferBank callback );
typedef void (__cdecl *lpRegOnRspQryInvestorPositionDetail)           ( HCTP hCtp, pfOnRspQryInvestorPositionDetail callback );
typedef void (__cdecl *lpRegOnRspQryNotice)                           ( HCTP hCtp, pfOnRspQryNotice callback );
typedef void (__cdecl *lpRegOnRspQrySettlementInfoConfirm)            ( HCTP hCtp, pfOnRspQrySettlementInfoConfirm callback );
typedef void (__cdecl *lpRegOnRspQryInvestorPositionCombineDetail)    ( HCTP hCtp, pfOnRspQryInvestorPositionCombineDetail callback );
typedef void (__cdecl *lpRegOnRspQryCFMMCTradingAccountKey)           ( HCTP hCtp, pfOnRspQryCFMMCTradingAccountKey callback );
typedef void (__cdecl *lpRegOnRspQryEWarrantOffset)                   ( HCTP hCtp, pfOnRspQryEWarrantOffset callback );
typedef void (__cdecl *lpRegOnRspQryTransferSerial)                   ( HCTP hCtp, pfOnRspQryTransferSerial callback );
typedef void (__cdecl *lpRegOnRspQryAccountregister)                  ( HCTP hCtp, pfOnRspQryAccountregister callback );
typedef void (__cdecl *lpRegOnRtnInstrumentStatus)                    ( HCTP hCtp, pfOnRtnInstrumentStatus callback );
typedef void (__cdecl *lpRegOnRtnTradingNotice)                       ( HCTP hCtp, pfOnRtnTradingNotice callback );
typedef void (__cdecl *lpRegOnRtnErrorConditionalOrder)               ( HCTP hCtp, pfOnRtnErrorConditionalOrder callback );
typedef void (__cdecl *lpRegOnRspQryContractBank)                     ( HCTP hCtp, pfOnRspQryContractBank callback );
typedef void (__cdecl *lpRegOnRspQryParkedOrder)                      ( HCTP hCtp, pfOnRspQryParkedOrder callback );
typedef void (__cdecl *lpRegOnRspQryParkedOrderAction)                ( HCTP hCtp, pfOnRspQryParkedOrderAction callback );
typedef void (__cdecl *lpRegOnRspQryTradingNotice)                    ( HCTP hCtp, pfOnRspQryTradingNotice callback );
typedef void (__cdecl *lpRegOnRspQryBrokerTradingParams)              ( HCTP hCtp, pfOnRspQryBrokerTradingParams callback );
typedef void (__cdecl *lpRegOnRspQryBrokerTradingAlgos)               ( HCTP hCtp, pfOnRspQryBrokerTradingAlgos callback );
typedef void (__cdecl *lpRegOnRtnFromBankToFutureByBank)              ( HCTP hCtp, pfOnRtnFromBankToFutureByBank callback );
typedef void (__cdecl *lpRegOnRtnFromFutureToBankByBank)              ( HCTP hCtp, pfOnRtnFromFutureToBankByBank callback );
typedef void (__cdecl *lpRegOnRtnRepealFromBankToFutureByBank)        ( HCTP hCtp, pfOnRtnRepealFromBankToFutureByBank callback );
typedef void (__cdecl *lpRegOnRtnRepealFromFutureToBankByBank)        ( HCTP hCtp, pfOnRtnRepealFromFutureToBankByBank callback );
typedef void (__cdecl *lpRegOnRtnFromBankToFutureByFuture)            ( HCTP hCtp, pfOnRtnFromBankToFutureByFuture callback );
typedef void (__cdecl *lpRegOnRtnFromFutureToBankByFuture)            ( HCTP hCtp, pfOnRtnFromFutureToBankByFuture callback );
typedef void (__cdecl *lpRegOnRtnRepealFromBankToFutureByFutureManual)( HCTP hCtp, pfOnRtnRepealFromBankToFutureByFutureManual callback );
typedef void (__cdecl *lpRegOnRtnRepealFromFutureToBankByFutureManual)( HCTP hCtp, pfOnRtnRepealFromFutureToBankByFutureManual callback );
typedef void (__cdecl *lpRegOnRtnQueryBankBalanceByFuture)            ( HCTP hCtp, pfOnRtnQueryBankBalanceByFuture callback );
typedef void (__cdecl *lpRegOnErrRtnBankToFutureByFuture)             ( HCTP hCtp, pfOnErrRtnBankToFutureByFuture callback );
typedef void (__cdecl *lpRegOnErrRtnFutureToBankByFuture)             ( HCTP hCtp, pfOnErrRtnFutureToBankByFuture callback );
typedef void (__cdecl *lpRegOnErrRtnRepealBankToFutureByFutureManual) ( HCTP hCtp, pfOnErrRtnRepealBankToFutureByFutureManual callback );
typedef void (__cdecl *lpRegOnErrRtnRepealFutureToBankByFutureManual) ( HCTP hCtp, pfOnErrRtnRepealFutureToBankByFutureManual callback );
typedef void (__cdecl *lpRegOnErrRtnQueryBankBalanceByFuture)         ( HCTP hCtp, pfOnErrRtnQueryBankBalanceByFuture callback );
typedef void (__cdecl *lpRegOnRtnRepealFromBankToFutureByFuture)      ( HCTP hCtp, pfOnRtnRepealFromBankToFutureByFuture callback );
typedef void (__cdecl *lpRegOnRtnRepealFromFutureToBankByFuture)      ( HCTP hCtp, pfOnRtnRepealFromFutureToBankByFuture callback );
typedef void (__cdecl *lpRegOnRspFromBankToFutureByFuture)            ( HCTP hCtp, pfOnRspFromBankToFutureByFuture callback );
typedef void (__cdecl *lpRegOnRspFromFutureToBankByFuture)            ( HCTP hCtp, pfOnRspFromFutureToBankByFuture callback );
typedef void (__cdecl *lpRegOnRspQueryBankAccountMoneyByFuture)       ( HCTP hCtp, pfOnRspQueryBankAccountMoneyByFuture callback );
typedef void (__cdecl *lpRegOnRtnOpenAccountByBank)                   ( HCTP hCtp, pfOnRtnOpenAccountByBank callback );
typedef void (__cdecl *lpRegOnRtnCancelAccountByBank)                 ( HCTP hCtp, pfOnRtnCancelAccountByBank callback );
typedef void (__cdecl *lpRegOnRtnChangeAccountByBank)                 ( HCTP hCtp, pfOnRtnChangeAccountByBank callback );
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
