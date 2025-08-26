//-------------------------------------------------------------------------------------------------------------------------
#ifndef USTPAPIdynamic_H
#define USTPAPIdynamic_H
//-------------------------------------------------------------------------------------------------------------------------
#include "USTPFtdcUserApiDataType.h"
#include "USTPFtdcUserApiStruct.h"
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef void* HUSTP;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern "C"
{
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Define all callback functions.
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef void (__cdecl *pfOnFrontConnected)( HUSTP Owner );
typedef void (__cdecl *pfOnFrontDisconnected)( HUSTP Owner, int nReason);
typedef void (__cdecl *pfOnHeartBeatWarning)( HUSTP Owner, int nTimeLapse);
typedef void (__cdecl *pfOnPackageStart)( HUSTP Owner, int nTopicID, int nSequenceNo);
typedef void (__cdecl *pfOnPackageEnd)( HUSTP Owner, int nTopicID, int nSequenceNo);
typedef void (__cdecl *pfOnRspError)( HUSTP Owner, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspUserLogin)( HUSTP Owner, CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspUserLogout)( HUSTP Owner, CUstpFtdcRspUserLogoutField *pRspUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspUserPasswordUpdate)( HUSTP Owner, CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspOrderInsert)( HUSTP Owner, CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspOrderAction)( HUSTP Owner, CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRtnFlowMessageCancel)( HUSTP Owner, CUstpFtdcFlowMessageCancelField *pFlowMessageCancel);
typedef void (__cdecl *pfOnRtnTrade)( HUSTP Owner, CUstpFtdcTradeField *pTrade);
typedef void (__cdecl *pfOnRtnOrder)( HUSTP Owner, CUstpFtdcOrderField *pOrder);
typedef void (__cdecl *pfOnErrRtnOrderInsert)( HUSTP Owner, CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo);
typedef void (__cdecl *pfOnErrRtnOrderAction)( HUSTP Owner, CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo);
typedef void (__cdecl *pfOnRtnInstrumentStatus)( HUSTP Owner, CUstpFtdcInstrumentStatusField *pInstrumentStatus);
typedef void (__cdecl *pfOnRspQryOrder)( HUSTP Owner, CUstpFtdcOrderField *pOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryTrade)( HUSTP Owner, CUstpFtdcTradeField *pTrade, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryUserInvestor)( HUSTP Owner, CUstpFtdcRspUserInvestorField *pRspUserInvestor, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryTradingCode)( HUSTP Owner, CUstpFtdcRspTradingCodeField *pRspTradingCode, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryInvestorAccount)( HUSTP Owner, CUstpFtdcRspInvestorAccountField *pRspInvestorAccount, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryInstrument)( HUSTP Owner, CUstpFtdcRspInstrumentField *pRspInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryExchange)( HUSTP Owner, CUstpFtdcRspExchangeField *pRspExchange, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryInvestorPosition)( HUSTP Owner, CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspSubscribeTopic)( HUSTP Owner, CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryComplianceParam)( HUSTP Owner, CUstpFtdcRspComplianceParamField *pRspComplianceParam, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryTopic)( HUSTP Owner, CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryInvestorFee)( HUSTP Owner, CUstpFtdcInvestorFeeField *pInvestorFee, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
typedef void (__cdecl *pfOnRspQryInvestorMargin)( HUSTP Owner, CUstpFtdcInvestorMarginField *pInvestorMargin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Define all export function pointors
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef HUSTP (__cdecl *lpCreateUSTPTraderApi)( void* Owner );
typedef const char *(__cdecl *lpGetVersion)( HUSTP hUstp, int &nMajorVersion, int &nMinorVersion);
typedef void (__cdecl *lpReleaseUSTPTraderApi)( HUSTP hUstp );
typedef void (__cdecl *lpInit)( HUSTP hUstp );
typedef const char* (__cdecl *lpGetTradingDay)( HUSTP hUstp);
typedef void (__cdecl *lpRegisterFront)( HUSTP hUstp, char *pszFrontAddress);
typedef void (__cdecl *lpRegisterNameServer)( HUSTP hUstp, char *pszNsAddress);
typedef int  (__cdecl *lpRegisterCertificateFile)( HUSTP hUstp, const char *pszCertFileName, const char *pszKeyFileName, 	const char *pszCaFileName, const char *pszKeyFilePassword);
typedef void (__cdecl *lpSubscribePrivateTopic)( HUSTP hUstp, USTP_TE_RESUME_TYPE nResumeType);
typedef void (__cdecl *lpSubscribePublicTopic)( HUSTP hUstp, USTP_TE_RESUME_TYPE nResumeType);
typedef void (__cdecl *lpSubscribeUserTopic)( HUSTP hUstp, USTP_TE_RESUME_TYPE nResumeType);
typedef void (__cdecl *lpSetHeartbeatTimeout)( HUSTP hUstp, unsigned int timeout);
typedef int  (__cdecl *lpOpenRequestLog)( HUSTP hUstp, const char *pszReqLogFileName);
typedef int  (__cdecl *lpOpenResponseLog)( HUSTP hUstp, const char *pszRspLogFileName);
typedef int  (__cdecl *lpReqUserLogin)( HUSTP hUstp, CUstpFtdcReqUserLoginField *pReqUserLogin, int nRequestID);
typedef int  (__cdecl *lpReqUserLogout)( HUSTP hUstp, CUstpFtdcReqUserLogoutField *pReqUserLogout, int nRequestID);
typedef int  (__cdecl *lpReqUserPasswordUpdate)( HUSTP hUstp, CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID);
typedef int  (__cdecl *lpReqOrderInsert)( HUSTP hUstp, CUstpFtdcInputOrderField *pInputOrder, int nRequestID);
typedef int  (__cdecl *lpReqOrderAction)( HUSTP hUstp, CUstpFtdcOrderActionField *pOrderAction, int nRequestID);
typedef int  (__cdecl *lpReqQryOrder)( HUSTP hUstp, CUstpFtdcQryOrderField *pQryOrder, int nRequestID);
typedef int  (__cdecl *lpReqQryTrade)( HUSTP hUstp, CUstpFtdcQryTradeField *pQryTrade, int nRequestID);
typedef int  (__cdecl *lpReqQryUserInvestor)( HUSTP hUstp, CUstpFtdcQryUserInvestorField *pQryUserInvestor, int nRequestID);
typedef int  (__cdecl *lpReqQryTradingCode)( HUSTP hUstp, CUstpFtdcQryTradingCodeField *pQryTradingCode, int nRequestID);
typedef int  (__cdecl *lpReqQryInvestorAccount)( HUSTP hUstp, CUstpFtdcQryInvestorAccountField *pQryInvestorAccount, int nRequestID);
typedef int  (__cdecl *lpReqQryInstrument)( HUSTP hUstp, CUstpFtdcQryInstrumentField *pQryInstrument, int nRequestID);
typedef int  (__cdecl *lpReqQryExchange)( HUSTP hUstp, CUstpFtdcQryExchangeField *pQryExchange, int nRequestID);
typedef int  (__cdecl *lpReqQryInvestorPosition)( HUSTP hUstp, CUstpFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID);
typedef int  (__cdecl *lpReqSubscribeTopic)( HUSTP hUstp, CUstpFtdcDisseminationField *pDissemination, int nRequestID);
typedef int  (__cdecl *lpReqQryComplianceParam)( HUSTP hUstp, CUstpFtdcQryComplianceParamField *pQryComplianceParam, int nRequestID);
typedef int  (__cdecl *lpReqQryTopic)( HUSTP hUstp, CUstpFtdcDisseminationField *pDissemination, int nRequestID);
typedef int  (__cdecl *lpReqQryInvestorFee)( HUSTP hUstp, CUstpFtdcQryInvestorFeeField *pQryInvestorFee, int nRequestID);
typedef int  (__cdecl *lpReqQryInvestorMargin)( HUSTP hUstp, CUstpFtdcQryInvestorMarginField *pQryInvestorMargin, int nRequestID);
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Define all RegOnXXXXXX register callback function functions.
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef void (__cdecl *lpRegOnFrontConnected)            ( HUSTP hUstp, pfOnFrontConnected callback );
typedef void (__cdecl *lpRegOnFrontDisconnected)         ( HUSTP hUstp, pfOnFrontDisconnected callback );
typedef void (__cdecl *lpRegOnHeartBeatWarning)          ( HUSTP hUstp, pfOnHeartBeatWarning callback );
typedef void (__cdecl *lpRegOnPackageStart)              ( HUSTP hUstp, pfOnPackageStart callback );
typedef void (__cdecl *lpRegOnPackageEnd)                ( HUSTP hUstp, pfOnPackageEnd callback );
typedef void (__cdecl *lpRegOnRspError)                  ( HUSTP hUstp, pfOnRspError callback );
typedef void (__cdecl *lpRegOnRspUserLogin)              ( HUSTP hUstp, pfOnRspUserLogin callback );
typedef void (__cdecl *lpRegOnRspUserLogout)             ( HUSTP hUstp, pfOnRspUserLogout callback );
typedef void (__cdecl *lpRegOnRspUserPasswordUpdate)     ( HUSTP hUstp, pfOnRspUserPasswordUpdate callback );
typedef void (__cdecl *lpRegOnRspOrderInsert)            ( HUSTP hUstp, pfOnRspOrderInsert callback );
typedef void (__cdecl *lpRegOnRspOrderAction)            ( HUSTP hUstp, pfOnRspOrderAction callback );
typedef void (__cdecl *lpRegOnRtnFlowMessageCancel)      ( HUSTP hUstp, pfOnRtnFlowMessageCancel callback );
typedef void (__cdecl *lpRegOnRtnTrade)                  ( HUSTP hUstp, pfOnRtnTrade callback );
typedef void (__cdecl *lpRegOnRtnOrder)                  ( HUSTP hUstp, pfOnRtnOrder callback );
typedef void (__cdecl *lpRegOnErrRtnOrderInsert)         ( HUSTP hUstp, pfOnErrRtnOrderInsert callback );
typedef void (__cdecl *lpRegOnErrRtnOrderAction)         ( HUSTP hUstp, pfOnErrRtnOrderAction callback );
typedef void (__cdecl *lpRegOnRtnInstrumentStatus)       ( HUSTP hUstp, pfOnRtnInstrumentStatus callback );
typedef void (__cdecl *lpRegOnRspQryOrder)               ( HUSTP hUstp, pfOnRspQryOrder callback );
typedef void (__cdecl *lpRegOnRspQryTrade)               ( HUSTP hUstp, pfOnRspQryTrade callback );
typedef void (__cdecl *lpRegOnRspQryUserInvestor)        ( HUSTP hUstp, pfOnRspQryUserInvestor callback );
typedef void (__cdecl *lpRegOnRspQryTradingCode)         ( HUSTP hUstp, pfOnRspQryTradingCode callback );
typedef void (__cdecl *lpRegOnRspQryInvestorAccount)     ( HUSTP hUstp, pfOnRspQryInvestorAccount callback );
typedef void (__cdecl *lpRegOnRspQryInstrument)          ( HUSTP hUstp, pfOnRspQryInstrument callback );
typedef void (__cdecl *lpRegOnRspQryExchange)            ( HUSTP hUstp, pfOnRspQryExchange callback );
typedef void (__cdecl *lpRegOnRspQryInvestorPosition)    ( HUSTP hUstp, pfOnRspQryInvestorPosition callback );
typedef void (__cdecl *lpRegOnRspSubscribeTopic)         ( HUSTP hUstp, pfOnRspSubscribeTopic callback );
typedef void (__cdecl *lpRegOnRspQryComplianceParam)     ( HUSTP hUstp, pfOnRspQryComplianceParam callback );
typedef void (__cdecl *lpRegOnRspQryTopic)               ( HUSTP hUstp, pfOnRspQryTopic callback );
typedef void (__cdecl *lpRegOnRspQryInvestorFee)         ( HUSTP hUstp, pfOnRspQryInvestorFee callback );
typedef void (__cdecl *lpRegOnRspQryInvestorMargin)      ( HUSTP hUstp, pfOnRspQryInvestorMargin callback );
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif