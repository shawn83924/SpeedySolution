// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FEMASAPIFACTORY_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FEMASAPIFACTORY_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef FEMASAPIFACTORY_EXPORTS
#define FEMASAPIFACTORY_API __declspec(dllexport)
#else
#define FEMASAPIFACTORY_API __declspec(dllimport)
#endif

#include "USTPFtdcUserApiDataType.h"
#include "USTPFtdcTraderApi.h"
#include "FemasAPIDynamic.h"


extern "C"
{
	FEMASAPIFACTORY_API HUSTP CreateUSTPTraderApi( void* Owner );
	FEMASAPIFACTORY_API void ReleaseUSTPTraderApi( HUSTP hUstp );
	FEMASAPIFACTORY_API void Init( HUSTP hUstp );
	FEMASAPIFACTORY_API const char *GetAPIVersion( HUSTP hUstp, int &nMajorVersion, int &nMinorVersion);
	FEMASAPIFACTORY_API const char* GetTradingDay( HUSTP hUstp);
	FEMASAPIFACTORY_API void RegisterFront( HUSTP hUstp, char *pszFrontAddress);
	FEMASAPIFACTORY_API void RegisterNameServer( HUSTP hUstp, char *pszNsAddress);	
	FEMASAPIFACTORY_API int  RegisterCertificateFile( HUSTP hUstp, const char *pszCertFileName, const char *pszKeyFileName, 	const char *pszCaFileName, const char *pszKeyFilePassword);
	FEMASAPIFACTORY_API void SubscribePrivateTopic( HUSTP hUstp, USTP_TE_RESUME_TYPE nResumeType);
	FEMASAPIFACTORY_API void SubscribePublicTopic( HUSTP hUstp, USTP_TE_RESUME_TYPE nResumeType);
	FEMASAPIFACTORY_API void SubscribeUserTopic( HUSTP hUstp, USTP_TE_RESUME_TYPE nResumeType);
	FEMASAPIFACTORY_API void SetHeartbeatTimeout( HUSTP hUstp, unsigned int timeout);
	FEMASAPIFACTORY_API int  OpenRequestLog( HUSTP hUstp, const char *pszReqLogFileName);
	FEMASAPIFACTORY_API int  OpenResponseLog( HUSTP hUstp, const char *pszRspLogFileName);
	FEMASAPIFACTORY_API int  ReqUserLogin( HUSTP hUstp, CUstpFtdcReqUserLoginField *pReqUserLogin, int nRequestID);
	FEMASAPIFACTORY_API int  ReqUserLogout( HUSTP hUstp, CUstpFtdcReqUserLogoutField *pReqUserLogout, int nRequestID);
	FEMASAPIFACTORY_API int  ReqUserPasswordUpdate( HUSTP hUstp, CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID);
	FEMASAPIFACTORY_API int  ReqOrderInsert( HUSTP hUstp, CUstpFtdcInputOrderField *pInputOrder, int nRequestID);
	FEMASAPIFACTORY_API int  ReqOrderAction( HUSTP hUstp, CUstpFtdcOrderActionField *pOrderAction, int nRequestID);
	FEMASAPIFACTORY_API int  ReqQryOrder( HUSTP hUstp, CUstpFtdcQryOrderField *pQryOrder, int nRequestID);
	FEMASAPIFACTORY_API int  ReqQryTrade( HUSTP hUstp, CUstpFtdcQryTradeField *pQryTrade, int nRequestID);
	FEMASAPIFACTORY_API int  ReqQryUserInvestor( HUSTP hUstp, CUstpFtdcQryUserInvestorField *pQryUserInvestor, int nRequestID);
	FEMASAPIFACTORY_API int  ReqQryTradingCode( HUSTP hUstp, CUstpFtdcQryTradingCodeField *pQryTradingCode, int nRequestID);
	FEMASAPIFACTORY_API int  ReqQryInvestorAccount( HUSTP hUstp, CUstpFtdcQryInvestorAccountField *pQryInvestorAccount, int nRequestID);
	FEMASAPIFACTORY_API int  ReqQryInstrument( HUSTP hUstp, CUstpFtdcQryInstrumentField *pQryInstrument, int nRequestID);
	FEMASAPIFACTORY_API int  ReqQryExchange( HUSTP hUstp, CUstpFtdcQryExchangeField *pQryExchange, int nRequestID);
	FEMASAPIFACTORY_API int  ReqQryInvestorPosition( HUSTP hUstp, CUstpFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID);
	FEMASAPIFACTORY_API int  ReqSubscribeTopic( HUSTP hUstp, CUstpFtdcDisseminationField *pDissemination, int nRequestID);
	FEMASAPIFACTORY_API int  ReqQryComplianceParam( HUSTP hUstp, CUstpFtdcQryComplianceParamField *pQryComplianceParam, int nRequestID);
	FEMASAPIFACTORY_API int  ReqQryTopic( HUSTP hUstp, CUstpFtdcDisseminationField *pDissemination, int nRequestID);
	FEMASAPIFACTORY_API int  ReqQryInvestorFee( HUSTP hUstp, CUstpFtdcQryInvestorFeeField *pQryInvestorFee, int nRequestID);
	FEMASAPIFACTORY_API int  ReqQryInvestorMargin( HUSTP hUstp, CUstpFtdcQryInvestorMarginField *pQryInvestorMargin, int nRequestID);
	FEMASAPIFACTORY_API void RegOnFrontConnected( HUSTP hUstp, pfOnFrontConnected callback );
	FEMASAPIFACTORY_API void RegOnFrontDisconnected( HUSTP hUstp, pfOnFrontDisconnected callback );
	FEMASAPIFACTORY_API void RegOnHeartBeatWarning( HUSTP hUstp, pfOnHeartBeatWarning callback );
	FEMASAPIFACTORY_API void RegOnPackageStart( HUSTP hUstp, pfOnPackageStart callback );
	FEMASAPIFACTORY_API void RegOnPackageEnd( HUSTP hUstp, pfOnPackageEnd callback );
	FEMASAPIFACTORY_API void RegOnRspError( HUSTP hUstp, pfOnRspError callback );
	FEMASAPIFACTORY_API void RegOnRspUserLogin( HUSTP hUstp, pfOnRspUserLogin callback );
	FEMASAPIFACTORY_API void RegOnRspUserLogout( HUSTP hUstp, pfOnRspUserLogout callback );
	FEMASAPIFACTORY_API void RegOnRspUserPasswordUpdate( HUSTP hUstp, pfOnRspUserPasswordUpdate callback );
	FEMASAPIFACTORY_API void RegOnRspOrderInsert( HUSTP hUstp, pfOnRspOrderInsert callback );
	FEMASAPIFACTORY_API void RegOnRspOrderAction( HUSTP hUstp, pfOnRspOrderAction callback );
	FEMASAPIFACTORY_API void RegOnRtnFlowMessageCancel( HUSTP hUstp, pfOnRtnFlowMessageCancel callback );
	FEMASAPIFACTORY_API void RegOnRtnTrade( HUSTP hUstp, pfOnRtnTrade callback );
	FEMASAPIFACTORY_API void RegOnRtnOrder( HUSTP hUstp, pfOnRtnOrder callback );
	FEMASAPIFACTORY_API void RegOnErrRtnOrderInsert( HUSTP hUstp, pfOnErrRtnOrderInsert callback );
	FEMASAPIFACTORY_API void RegOnErrRtnOrderAction( HUSTP hUstp, pfOnErrRtnOrderAction callback );
	FEMASAPIFACTORY_API void RegOnRtnInstrumentStatus( HUSTP hUstp, pfOnRtnInstrumentStatus callback );
	FEMASAPIFACTORY_API void RegOnRspQryOrder( HUSTP hUstp, pfOnRspQryOrder callback );
	FEMASAPIFACTORY_API void RegOnRspQryTrade( HUSTP hUstp, pfOnRspQryTrade callback );
	FEMASAPIFACTORY_API void RegOnRspQryUserInvestor( HUSTP hUstp, pfOnRspQryUserInvestor callback );
	FEMASAPIFACTORY_API void RegOnRspQryTradingCode( HUSTP hUstp, pfOnRspQryTradingCode callback );
	FEMASAPIFACTORY_API void RegOnRspQryInvestorAccount( HUSTP hUstp, pfOnRspQryInvestorAccount callback );
	FEMASAPIFACTORY_API void RegOnRspQryInstrument( HUSTP hUstp, pfOnRspQryInstrument callback );
	FEMASAPIFACTORY_API void RegOnRspQryExchange( HUSTP hUstp, pfOnRspQryExchange callback );
	FEMASAPIFACTORY_API void RegOnRspQryInvestorPosition( HUSTP hUstp, pfOnRspQryInvestorPosition callback );
	FEMASAPIFACTORY_API void RegOnRspSubscribeTopic( HUSTP hUstp, pfOnRspSubscribeTopic callback );
	FEMASAPIFACTORY_API void RegOnRspQryComplianceParam( HUSTP hUstp, pfOnRspQryComplianceParam callback );
	FEMASAPIFACTORY_API void RegOnRspQryTopic( HUSTP hUstp, pfOnRspQryTopic callback );
	FEMASAPIFACTORY_API void RegOnRspQryInvestorFee( HUSTP hUstp, pfOnRspQryInvestorFee callback );
	FEMASAPIFACTORY_API void RegOnRspQryInvestorMargin( HUSTP hUstp, pfOnRspQryInvestorMargin callback );
};