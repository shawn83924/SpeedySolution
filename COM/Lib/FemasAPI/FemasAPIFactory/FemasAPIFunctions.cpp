#include "stdafx.h"
#include "FemasAPIComm.h"
//---------------------------------------------------------------------
HUSTP  CreateUSTPTraderApi( void* Owner )
{
	CUstpFtdcTraderApi* USTPObject = CUstpFtdcTraderApi::CreateFtdcTraderApi();
	if( USTPObject != NULL )
	{
		USTPHandle* hUSTP = new USTPHandle(); 
		memset( hUSTP, 0, sizeof( USTPHandle ) );
		hUSTP->USTPObj = USTPObject;
		hUSTP->USTPHandler = new USTPAPIHandler( hUSTP );
		hUSTP->FOwner = Owner; 
		hUSTP->USTPObj->RegisterSpi( hUSTP->USTPHandler );
		return (HUSTP)hUSTP; 
	}
	return NULL;
}
//---------------------------------------------------------------------
void  ReleaseUSTPTraderApi( HUSTP hUstp )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->USTPObj->RegisterSpi( NULL );
	hUSTP->USTPObj->Release();
	delete hUSTP->USTPHandler;
	delete hUSTP;
}
//---------------------------------------------------------------------
void Init( HUSTP hUstp )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->Init();
}
//---------------------------------------------------------------------
const char *GetAPIVersion( HUSTP hUstp, int &nMajorVersion, int &nMinorVersion)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->GetVersion( nMajorVersion, nMinorVersion );
}
//---------------------------------------------------------------------
const char* GetTradingDay( HUSTP hUstp)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->GetTradingDay();
}
//---------------------------------------------------------------------
void RegisterFront( HUSTP hUstp, char *pszFrontAddress)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->RegisterFront( pszFrontAddress );
}
//---------------------------------------------------------------------
void RegisterNameServer( HUSTP hUstp, char *pszNsAddress)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->RegisterNameServer( pszNsAddress );
}
//---------------------------------------------------------------------
int  RegisterCertificateFile( HUSTP hUstp, const char *pszCertFileName, const char *pszKeyFileName, 	const char *pszCaFileName, const char *pszKeyFilePassword)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->RegisterCertificateFile( pszCertFileName, pszKeyFileName, pszCaFileName, pszKeyFilePassword );
}
//---------------------------------------------------------------------
void SubscribePrivateTopic( HUSTP hUstp, USTP_TE_RESUME_TYPE nResumeType)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->SubscribePrivateTopic( nResumeType );
}
//---------------------------------------------------------------------
void SubscribePublicTopic( HUSTP hUstp, USTP_TE_RESUME_TYPE nResumeType)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->SubscribePublicTopic( nResumeType );
}
//---------------------------------------------------------------------
void SubscribeUserTopic( HUSTP hUstp, USTP_TE_RESUME_TYPE nResumeType)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->SubscribeUserTopic( nResumeType );
}
//---------------------------------------------------------------------
void SetHeartbeatTimeout( HUSTP hUstp, unsigned int timeout)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->SetHeartbeatTimeout( timeout );
}
//---------------------------------------------------------------------
int  OpenRequestLog( HUSTP hUstp, const char *pszReqLogFileName)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->OpenRequestLog( pszReqLogFileName );
}
//---------------------------------------------------------------------
int  OpenResponseLog( HUSTP hUstp, const char *pszRspLogFileName)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->OpenResponseLog( pszRspLogFileName );
}
//---------------------------------------------------------------------
int  ReqUserLogin( HUSTP hUstp, CUstpFtdcReqUserLoginField *pReqUserLogin, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqUserLogin( pReqUserLogin, nRequestID );
}
//---------------------------------------------------------------------
int  ReqUserLogout( HUSTP hUstp, CUstpFtdcReqUserLogoutField *pReqUserLogout, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqUserLogout( pReqUserLogout, nRequestID );
}
//---------------------------------------------------------------------
int  ReqUserPasswordUpdate( HUSTP hUstp, CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqUserPasswordUpdate( pUserPasswordUpdate, nRequestID );
}
//---------------------------------------------------------------------
int  ReqOrderInsert( HUSTP hUstp, CUstpFtdcInputOrderField *pInputOrder, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqOrderInsert( pInputOrder, nRequestID);
}
//---------------------------------------------------------------------
int  ReqOrderAction( HUSTP hUstp, CUstpFtdcOrderActionField *pOrderAction, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqOrderAction( pOrderAction, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryOrder( HUSTP hUstp, CUstpFtdcQryOrderField *pQryOrder, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqQryOrder( pQryOrder, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryTrade( HUSTP hUstp, CUstpFtdcQryTradeField *pQryTrade, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqQryTrade( pQryTrade, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryUserInvestor( HUSTP hUstp, CUstpFtdcQryUserInvestorField *pQryUserInvestor, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqQryUserInvestor( pQryUserInvestor, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryTradingCode( HUSTP hUstp, CUstpFtdcQryTradingCodeField *pQryTradingCode, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqQryTradingCode( pQryTradingCode, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryInvestorAccount( HUSTP hUstp, CUstpFtdcQryInvestorAccountField *pQryInvestorAccount, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqQryInvestorAccount( pQryInvestorAccount, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryInstrument( HUSTP hUstp, CUstpFtdcQryInstrumentField *pQryInstrument, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqQryInstrument( pQryInstrument, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryExchange( HUSTP hUstp, CUstpFtdcQryExchangeField *pQryExchange, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqQryExchange( pQryExchange, nRequestID);
}
//---------------------------------------------------------------------
int  ReqQryInvestorPosition( HUSTP hUstp, CUstpFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqQryInvestorPosition( pQryInvestorPosition, nRequestID);
}
//---------------------------------------------------------------------
int  ReqSubscribeTopic( HUSTP hUstp, CUstpFtdcDisseminationField *pDissemination, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqSubscribeTopic( pDissemination, nRequestID);
}
//---------------------------------------------------------------------
int  ReqQryComplianceParam( HUSTP hUstp, CUstpFtdcQryComplianceParamField *pQryComplianceParam, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqQryComplianceParam( pQryComplianceParam, nRequestID);
}
//---------------------------------------------------------------------
int  ReqQryTopic( HUSTP hUstp, CUstpFtdcDisseminationField *pDissemination, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqQryTopic( pDissemination, nRequestID);
}
//---------------------------------------------------------------------
int  ReqQryInvestorFee( HUSTP hUstp, CUstpFtdcQryInvestorFeeField *pQryInvestorFee, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqQryInvestorFee( pQryInvestorFee, nRequestID);
}
//---------------------------------------------------------------------
int  ReqQryInvestorMargin( HUSTP hUstp, CUstpFtdcQryInvestorMarginField *pQryInvestorMargin, int nRequestID)
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	return hUSTP->USTPObj->ReqQryInvestorMargin( pQryInvestorMargin, nRequestID);
}
//---------------------------------------------------------------------
//   Implement register call back function functions.
//---------------------------------------------------------------------
void RegOnFrontConnected( HUSTP hUstp, pfOnFrontConnected callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnFrontConnected = callback;
}
//---------------------------------------------------------------------
void RegOnFrontDisconnected( HUSTP hUstp, pfOnFrontDisconnected callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnFrontDisconnected = callback;
}
//---------------------------------------------------------------------
void RegOnHeartBeatWarning( HUSTP hUstp, pfOnHeartBeatWarning callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnHeartBeatWarning = callback;
}
//---------------------------------------------------------------------
void RegOnPackageStart( HUSTP hUstp, pfOnPackageStart callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnPackageStart = callback;
}
//---------------------------------------------------------------------
void RegOnPackageEnd( HUSTP hUstp, pfOnPackageEnd callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnPackageEnd = callback;
}
//---------------------------------------------------------------------
void RegOnRspError( HUSTP hUstp, pfOnRspError callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspError = callback;
}
//---------------------------------------------------------------------
void RegOnRspUserLogin( HUSTP hUstp, pfOnRspUserLogin callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspUserLogin = callback;
}
//---------------------------------------------------------------------
void RegOnRspUserLogout( HUSTP hUstp, pfOnRspUserLogout callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspUserLogout = callback;
}
//---------------------------------------------------------------------
void RegOnRspUserPasswordUpdate( HUSTP hUstp, pfOnRspUserPasswordUpdate callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspUserPasswordUpdate = callback;
}
//---------------------------------------------------------------------
void RegOnRspOrderInsert( HUSTP hUstp, pfOnRspOrderInsert callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspOrderInsert = callback;
}
//---------------------------------------------------------------------
void RegOnRspOrderAction( HUSTP hUstp, pfOnRspOrderAction callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspOrderAction = callback;
}
//---------------------------------------------------------------------
void RegOnRtnFlowMessageCancel( HUSTP hUstp, pfOnRtnFlowMessageCancel callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRtnFlowMessageCancel = callback;
}
//---------------------------------------------------------------------
void RegOnRtnTrade( HUSTP hUstp, pfOnRtnTrade callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRtnTrade = callback;
}
//---------------------------------------------------------------------
void RegOnRtnOrder( HUSTP hUstp, pfOnRtnOrder callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRtnOrder = callback;
}
//---------------------------------------------------------------------
void RegOnErrRtnOrderInsert( HUSTP hUstp, pfOnErrRtnOrderInsert callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnErrRtnOrderInsert = callback;
}
//---------------------------------------------------------------------
void RegOnErrRtnOrderAction( HUSTP hUstp, pfOnErrRtnOrderAction callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnErrRtnOrderAction = callback;
}
//---------------------------------------------------------------------
void RegOnRtnInstrumentStatus( HUSTP hUstp, pfOnRtnInstrumentStatus callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRtnInstrumentStatus = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryOrder( HUSTP hUstp, pfOnRspQryOrder callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspQryOrder = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryTrade( HUSTP hUstp, pfOnRspQryTrade callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspQryTrade = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryUserInvestor( HUSTP hUstp, pfOnRspQryUserInvestor callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspQryUserInvestor = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryTradingCode( HUSTP hUstp, pfOnRspQryTradingCode callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspQryTradingCode = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryInvestorAccount( HUSTP hUstp, pfOnRspQryInvestorAccount callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspQryInvestorAccount = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryInstrument( HUSTP hUstp, pfOnRspQryInstrument callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspQryInstrument = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryExchange( HUSTP hUstp, pfOnRspQryExchange callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspQryExchange = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryInvestorPosition( HUSTP hUstp, pfOnRspQryInvestorPosition callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspQryInvestorPosition = callback;
}
//---------------------------------------------------------------------
void RegOnRspSubscribeTopic( HUSTP hUstp, pfOnRspSubscribeTopic callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspSubscribeTopic = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryComplianceParam( HUSTP hUstp, pfOnRspQryComplianceParam callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspQryComplianceParam = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryTopic( HUSTP hUstp, pfOnRspQryTopic callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspQryTopic = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryInvestorFee( HUSTP hUstp, pfOnRspQryInvestorFee callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspQryInvestorFee = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryInvestorMargin( HUSTP hUstp, pfOnRspQryInvestorMargin callback )
{
	USTPHandle* hUSTP = reinterpret_cast<USTPHandle*>(hUstp);
	hUSTP->OnRspQryInvestorMargin = callback;
}
//---------------------------------------------------------------------