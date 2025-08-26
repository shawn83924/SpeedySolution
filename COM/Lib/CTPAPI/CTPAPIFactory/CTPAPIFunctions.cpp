#include "stdafx.h"
#include "CTPAPIComm.h"
//---------------------------------------------------------------------
HINSTANCE gCTPDLLInstance = NULL;
typedef CThostFtdcTraderApi* (__cdecl *lpCreateFtdcTraderApi)( const char *pszFlowPath, const bool bIsUsingUdp );
//---------------------------------------------------------------------
HCTP  CreateCTPTraderApi( void* Owner, char* strDllName )
{
	if ( gCTPDLLInstance == NULL )
	{
		#if defined(__S8_CTP)
		gCTPDLLInstance = ::LoadLibrary( L"CTPtraderapi_120530.dll" );
		strcpy( strDllName, "CTPtraderapi_120530.dll" );
		#elif defined(__S8_HS)
		gCTPDLLInstance = ::LoadLibrary( L"HStraderapi_120530.dll" );
		strcpy( strDllName, "HStraderapi_120530.dll" );
		#elif defined(__S8_KN)
		gCTPDLLInstance = ::LoadLibrary( L"KNtraderapi_120530.dll" );
		strcpy( strDllName, "KNtraderapi_120530.dll" );
		#else
		return NULL;
		#endif
	}
	if ( gCTPDLLInstance != NULL )
	{
		lpCreateFtdcTraderApi fCreateFtdcTraderApi = (lpCreateFtdcTraderApi)::GetProcAddress( gCTPDLLInstance, "?CreateFtdcTraderApi@CThostFtdcTraderApi@@SAPAV1@PBD_N@Z" );
		if ( fCreateFtdcTraderApi != NULL )
		{
			CThostFtdcTraderApi* CTPObject = fCreateFtdcTraderApi( "", false );
			//CThostFtdcTraderApi* CTPObject = CThostFtdcTraderApi::CreateFtdcTraderApi();
			if ( CTPObject != NULL )
			{
				CTPHandle* hCTP = new CTPHandle(); 
				memset( hCTP, 0, sizeof( CTPHandle ) );
				hCTP->CTPObj = CTPObject;
				hCTP->CTPHandler = new CTPAPIHandler( hCTP );
				hCTP->FOwner = Owner; 
				hCTP->CTPObj->RegisterSpi( hCTP->CTPHandler );
				return (HCTP)hCTP; 
			}
		}
	}
	return NULL;
}
//---------------------------------------------------------------------
void  ReleaseCTPTraderApi( HCTP hCtp )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->CTPObj->RegisterSpi( NULL );
	hCTP->CTPObj->Release();
	delete hCTP->CTPHandler;
	delete hCTP;
}
//---------------------------------------------------------------------
void  Init( HCTP hCtp )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->CTPObj->Init( );
}
//---------------------------------------------------------------------
const char*  GetTradingDay( HCTP hCtp )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->GetTradingDay();
}
//---------------------------------------------------------------------
void  RegisterFront( HCTP hCtp, char *pszFrontAddress)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->CTPObj->RegisterFront( pszFrontAddress );
}
//---------------------------------------------------------------------
void  RegisterNameServer( HCTP hCtp, char *pszNsAddress)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->CTPObj->RegisterNameServer( pszNsAddress );
}
//---------------------------------------------------------------------
void  SubscribePrivateTopic( HCTP hCtp,THOST_TE_RESUME_TYPE nResumeType)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->CTPObj->SubscribePrivateTopic( nResumeType );
}
//---------------------------------------------------------------------
void  SubscribePublicTopic( HCTP hCtp, THOST_TE_RESUME_TYPE nResumeType)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->CTPObj->SubscribePublicTopic( nResumeType );
}
//---------------------------------------------------------------------
int  ReqAuthenticate( HCTP hCtp, CThostFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqAuthenticate( pReqAuthenticateField, nRequestID );
}
//---------------------------------------------------------------------
int  ReqUserLogin( HCTP hCtp, CThostFtdcReqUserLoginField *pReqUserLoginField, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqUserLogin( pReqUserLoginField, nRequestID );
}
//---------------------------------------------------------------------
int  ReqUserLogout( HCTP hCtp, CThostFtdcUserLogoutField *pUserLogout, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqUserLogout( pUserLogout, nRequestID );
}
//---------------------------------------------------------------------
int  ReqUserPasswordUpdate(HCTP hCtp, CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqUserPasswordUpdate( pUserPasswordUpdate, nRequestID );
}
//---------------------------------------------------------------------
int  ReqTradingAccountPasswordUpdate(HCTP hCtp, CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqTradingAccountPasswordUpdate( pTradingAccountPasswordUpdate, nRequestID );
}
//---------------------------------------------------------------------
int  ReqOrderInsert(HCTP hCtp, CThostFtdcInputOrderField *pInputOrder, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqOrderInsert( pInputOrder, nRequestID );
}
//---------------------------------------------------------------------
int  ReqParkedOrderInsert(HCTP hCtp, CThostFtdcParkedOrderField *pParkedOrder, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqParkedOrderInsert( pParkedOrder, nRequestID );
}
//---------------------------------------------------------------------
int  ReqParkedOrderAction(HCTP hCtp, CThostFtdcParkedOrderActionField *pParkedOrderAction, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqParkedOrderAction( pParkedOrderAction, nRequestID );
}
//---------------------------------------------------------------------
int  ReqOrderAction(HCTP hCtp, CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqOrderAction( pInputOrderAction, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQueryMaxOrderVolume(HCTP hCtp, CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQueryMaxOrderVolume( pQueryMaxOrderVolume, nRequestID );
}
//---------------------------------------------------------------------
int  ReqSettlementInfoConfirm(HCTP hCtp, CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqSettlementInfoConfirm( pSettlementInfoConfirm, nRequestID );
}
//---------------------------------------------------------------------
int  ReqRemoveParkedOrder(HCTP hCtp, CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqRemoveParkedOrder( pRemoveParkedOrder, nRequestID );
}
//---------------------------------------------------------------------
int  ReqRemoveParkedOrderAction(HCTP hCtp, CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqRemoveParkedOrderAction( pRemoveParkedOrderAction, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryOrder(HCTP hCtp, CThostFtdcQryOrderField *pQryOrder, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryOrder( pQryOrder, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryTrade(HCTP hCtp, CThostFtdcQryTradeField *pQryTrade, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryTrade( pQryTrade, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryInvestorPosition(HCTP hCtp, CThostFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryInvestorPosition( pQryInvestorPosition, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryTradingAccount(HCTP hCtp, CThostFtdcQryTradingAccountField *pQryTradingAccount, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryTradingAccount( pQryTradingAccount, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryInvestor(HCTP hCtp, CThostFtdcQryInvestorField *pQryInvestor, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryInvestor( pQryInvestor, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryTradingCode(HCTP hCtp, CThostFtdcQryTradingCodeField *pQryTradingCode, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryTradingCode( pQryTradingCode, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryInstrumentMarginRate(HCTP hCtp, CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryInstrumentMarginRate( pQryInstrumentMarginRate, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryInstrumentCommissionRate(HCTP hCtp, CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryInstrumentCommissionRate( pQryInstrumentCommissionRate, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryExchange(HCTP hCtp, CThostFtdcQryExchangeField *pQryExchange, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryExchange( pQryExchange, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryInstrument(HCTP hCtp, CThostFtdcQryInstrumentField *pQryInstrument, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryInstrument( pQryInstrument, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryDepthMarketData(HCTP hCtp, CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryDepthMarketData( pQryDepthMarketData, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQrySettlementInfo(HCTP hCtp, CThostFtdcQrySettlementInfoField *pQrySettlementInfo, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQrySettlementInfo( pQrySettlementInfo, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryTransferBank(HCTP hCtp, CThostFtdcQryTransferBankField *pQryTransferBank, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryTransferBank( pQryTransferBank, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryInvestorPositionDetail(HCTP hCtp, CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryInvestorPositionDetail( pQryInvestorPositionDetail, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryNotice(HCTP hCtp, CThostFtdcQryNoticeField *pQryNotice, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryNotice( pQryNotice, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQrySettlementInfoConfirm(HCTP hCtp, CThostFtdcQrySettlementInfoConfirmField *pQrySettlementInfoConfirm, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQrySettlementInfoConfirm( pQrySettlementInfoConfirm, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryInvestorPositionCombineDetail(HCTP hCtp, CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorPositionCombineDetail, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryInvestorPositionCombineDetail( pQryInvestorPositionCombineDetail, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryCFMMCTradingAccountKey(HCTP hCtp, CThostFtdcQryCFMMCTradingAccountKeyField *pQryCFMMCTradingAccountKey, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryCFMMCTradingAccountKey( pQryCFMMCTradingAccountKey, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryEWarrantOffset(HCTP hCtp, CThostFtdcQryEWarrantOffsetField *pQryEWarrantOffset, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryEWarrantOffset( pQryEWarrantOffset, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryTransferSerial(HCTP hCtp, CThostFtdcQryTransferSerialField *pQryTransferSerial, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryTransferSerial( pQryTransferSerial, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryAccountregister(HCTP hCtp, CThostFtdcQryAccountregisterField *pQryAccountregister, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryAccountregister( pQryAccountregister, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryContractBank(HCTP hCtp, CThostFtdcQryContractBankField *pQryContractBank, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryContractBank( pQryContractBank, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryParkedOrder(HCTP hCtp, CThostFtdcQryParkedOrderField *pQryParkedOrder, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryParkedOrder( pQryParkedOrder, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryParkedOrderAction(HCTP hCtp, CThostFtdcQryParkedOrderActionField *pQryParkedOrderAction, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryParkedOrderAction( pQryParkedOrderAction, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryTradingNotice(HCTP hCtp, CThostFtdcQryTradingNoticeField *pQryTradingNotice, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryTradingNotice( pQryTradingNotice, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryBrokerTradingParams(HCTP hCtp, CThostFtdcQryBrokerTradingParamsField *pQryBrokerTradingParams, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryBrokerTradingParams( pQryBrokerTradingParams, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQryBrokerTradingAlgos(HCTP hCtp, CThostFtdcQryBrokerTradingAlgosField *pQryBrokerTradingAlgos, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQryBrokerTradingAlgos( pQryBrokerTradingAlgos, nRequestID );
}
//---------------------------------------------------------------------
int  ReqFromBankToFutureByFuture(HCTP hCtp, CThostFtdcReqTransferField *pReqTransfer, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqFromBankToFutureByFuture( pReqTransfer, nRequestID );
}
//---------------------------------------------------------------------
int  ReqFromFutureToBankByFuture(HCTP hCtp, CThostFtdcReqTransferField *pReqTransfer, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqFromFutureToBankByFuture( pReqTransfer, nRequestID );
}
//---------------------------------------------------------------------
int  ReqQueryBankAccountMoneyByFuture(HCTP hCtp, CThostFtdcReqQueryAccountField *pReqQueryAccount, int nRequestID)
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	return hCTP->CTPObj->ReqQueryBankAccountMoneyByFuture( pReqQueryAccount, nRequestID );
}
//---------------------------------------------------------------------
void  RegOnFrontConnected( HCTP hCtp, pfOnFrontConnected callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnFrontConnected = callback;
}
//---------------------------------------------------------------------
void  RegOnFrontDisconnected( HCTP hCtp, pfOnFrontDisconnected callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnFrontDisconnected = callback;
}
//---------------------------------------------------------------------
void  RegOnRspUserLogin( HCTP hCtp, pfOnRspUserLogin callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspUserLogin = callback;
}
//---------------------------------------------------------------------
void  RegOnRspQrySettlementInfo( HCTP hCtp, pfOnRspQrySettlementInfo callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQrySettlementInfo = callback;
}
//---------------------------------------------------------------------
void  RegOnRspSettlementInfoConfirm( HCTP hCtp, pfOnRspSettlementInfoConfirm callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspSettlementInfoConfirm = callback;
}
//---------------------------------------------------------------------
void  RegOnRspOrderInsert( HCTP hCtp, pfOnRspOrderInsert callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspOrderInsert = callback;
}
//---------------------------------------------------------------------
void  RegOnErrRtnOrderInsert( HCTP hCtp, pfOnErrRtnOrderInsert callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnErrRtnOrderInsert = callback;
}
//---------------------------------------------------------------------
void  RegOnRspOrderAction( HCTP hCtp, pfOnRspOrderAction callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspOrderAction = callback;
}
//---------------------------------------------------------------------
void  RegOnErrRtnOrderAction( HCTP hCtp, pfOnErrRtnOrderAction callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnErrRtnOrderAction = callback;
}
//---------------------------------------------------------------------
void  RegOnRspQryOrder( HCTP hCtp, pfOnRspQryOrder callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryOrder = callback;
}
//---------------------------------------------------------------------
void  RegOnRspQryExchange( HCTP hCtp, pfOnRspQryExchange callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryExchange = callback;
}
//---------------------------------------------------------------------
void  RegOnRspQryInstrument( HCTP hCtp, pfOnRspQryInstrument callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryInstrument = callback;
}
//---------------------------------------------------------------------
void  RegOnRspQryInvestorPosition( HCTP hCtp, pfOnRspQryInvestorPosition callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryInvestorPosition = callback;
}
//---------------------------------------------------------------------
void  RegOnRtnOrder( HCTP hCtp, pfOnRtnOrder callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnOrder = callback;
}
//---------------------------------------------------------------------
void  RegOnRtnTrade( HCTP hCtp, pfOnRtnTrade callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnTrade = callback;
}
//---------------------------------------------------------------------
void  RegOnRspError( HCTP hCtp, pfOnRspError callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspError = callback;
}
//---------------------------------------------------------------------
//
//
//---------------------------------------------------------------------
void RegOnHeartBeatWarning( HCTP hCtp, pfOnHeartBeatWarning callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnHeartBeatWarning = callback;
}
//---------------------------------------------------------------------
void RegOnRspAuthenticate( HCTP hCtp, pfOnRspAuthenticate callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspAuthenticate = callback;
}
//---------------------------------------------------------------------
void RegOnRspUserLogout( HCTP hCtp, pfOnRspUserLogout callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspUserLogout = callback;
}
//---------------------------------------------------------------------
void RegOnRspUserPasswordUpdate( HCTP hCtp, pfOnRspUserPasswordUpdate callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspUserPasswordUpdate = callback;
}
//---------------------------------------------------------------------
void RegOnRspTradingAccountPasswordUpdate( HCTP hCtp, pfOnRspTradingAccountPasswordUpdate callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspTradingAccountPasswordUpdate = callback;
}
//---------------------------------------------------------------------
void RegOnRspParkedOrderInsert( HCTP hCtp, pfOnRspParkedOrderInsert callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspParkedOrderInsert = callback;
}
//---------------------------------------------------------------------
void RegOnRspParkedOrderAction( HCTP hCtp, pfOnRspParkedOrderAction callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspParkedOrderAction = callback;
}
//---------------------------------------------------------------------
void RegOnRspQueryMaxOrderVolume( HCTP hCtp, pfOnRspQueryMaxOrderVolume callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQueryMaxOrderVolume = callback;
}
//---------------------------------------------------------------------
void RegOnRspRemoveParkedOrder( HCTP hCtp, pfOnRspRemoveParkedOrder callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspRemoveParkedOrder = callback;
}
//---------------------------------------------------------------------
void RegOnRspRemoveParkedOrderAction( HCTP hCtp, pfOnRspRemoveParkedOrderAction callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspRemoveParkedOrderAction = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryTrade( HCTP hCtp, pfOnRspQryTrade callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryTrade = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryTradingAccount( HCTP hCtp, pfOnRspQryTradingAccount callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryTradingAccount = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryInvestor( HCTP hCtp, pfOnRspQryInvestor callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryInvestor = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryTradingCode( HCTP hCtp, pfOnRspQryTradingCode callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryTradingCode = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryInstrumentMarginRate( HCTP hCtp, pfOnRspQryInstrumentMarginRate callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryInstrumentMarginRate = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryInstrumentCommissionRate( HCTP hCtp, pfOnRspQryInstrumentCommissionRate callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryInstrumentCommissionRate = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryDepthMarketData( HCTP hCtp, pfOnRspQryDepthMarketData callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryDepthMarketData = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryTransferBank( HCTP hCtp, pfOnRspQryTransferBank callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryTransferBank = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryInvestorPositionDetail( HCTP hCtp, pfOnRspQryInvestorPositionDetail callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryInvestorPositionDetail = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryNotice( HCTP hCtp, pfOnRspQryNotice callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryNotice = callback;
}
//---------------------------------------------------------------------
void RegOnRspQrySettlementInfoConfirm( HCTP hCtp, pfOnRspQrySettlementInfoConfirm callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQrySettlementInfoConfirm = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryInvestorPositionCombineDetail( HCTP hCtp, pfOnRspQryInvestorPositionCombineDetail callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryInvestorPositionCombineDetail = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryCFMMCTradingAccountKey( HCTP hCtp, pfOnRspQryCFMMCTradingAccountKey callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryCFMMCTradingAccountKey = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryEWarrantOffset( HCTP hCtp, pfOnRspQryEWarrantOffset callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryEWarrantOffset = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryTransferSerial( HCTP hCtp, pfOnRspQryTransferSerial callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryTransferSerial = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryAccountregister( HCTP hCtp, pfOnRspQryAccountregister callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryAccountregister = callback;
}
//---------------------------------------------------------------------
void RegOnRtnInstrumentStatus( HCTP hCtp, pfOnRtnInstrumentStatus callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnInstrumentStatus = callback;
}
//---------------------------------------------------------------------
void RegOnRtnTradingNotice( HCTP hCtp, pfOnRtnTradingNotice callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnTradingNotice = callback;
}
//---------------------------------------------------------------------
void RegOnRtnErrorConditionalOrder( HCTP hCtp, pfOnRtnErrorConditionalOrder callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnErrorConditionalOrder = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryContractBank( HCTP hCtp, pfOnRspQryContractBank callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryContractBank = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryParkedOrder( HCTP hCtp, pfOnRspQryParkedOrder callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryParkedOrder = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryParkedOrderAction( HCTP hCtp, pfOnRspQryParkedOrderAction callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryParkedOrderAction = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryTradingNotice( HCTP hCtp, pfOnRspQryTradingNotice callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryTradingNotice = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryBrokerTradingParams( HCTP hCtp, pfOnRspQryBrokerTradingParams callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryBrokerTradingParams = callback;
}
//---------------------------------------------------------------------
void RegOnRspQryBrokerTradingAlgos( HCTP hCtp, pfOnRspQryBrokerTradingAlgos callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQryBrokerTradingAlgos = callback;
}
//---------------------------------------------------------------------
void RegOnRtnFromBankToFutureByBank( HCTP hCtp, pfOnRtnFromBankToFutureByBank callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnFromBankToFutureByBank = callback;
}
//---------------------------------------------------------------------
void RegOnRtnFromFutureToBankByBank( HCTP hCtp, pfOnRtnFromFutureToBankByBank callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnFromFutureToBankByBank = callback;
}
//---------------------------------------------------------------------
void RegOnRtnRepealFromBankToFutureByBank( HCTP hCtp, pfOnRtnRepealFromBankToFutureByBank callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnRepealFromBankToFutureByBank = callback;
}
//---------------------------------------------------------------------
void RegOnRtnRepealFromFutureToBankByBank( HCTP hCtp, pfOnRtnRepealFromFutureToBankByBank callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnRepealFromFutureToBankByBank = callback;
}
//---------------------------------------------------------------------
void RegOnRtnFromBankToFutureByFuture( HCTP hCtp, pfOnRtnFromBankToFutureByFuture callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnFromBankToFutureByFuture = callback;
}
//---------------------------------------------------------------------
void RegOnRtnFromFutureToBankByFuture( HCTP hCtp, pfOnRtnFromFutureToBankByFuture callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnFromFutureToBankByFuture = callback;
}
//---------------------------------------------------------------------
void RegOnRtnRepealFromBankToFutureByFutureManual( HCTP hCtp, pfOnRtnRepealFromBankToFutureByFutureManual callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnRepealFromBankToFutureByFutureManual = callback;
}
//---------------------------------------------------------------------
void RegOnRtnRepealFromFutureToBankByFutureManual( HCTP hCtp, pfOnRtnRepealFromFutureToBankByFutureManual callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnRepealFromFutureToBankByFutureManual = callback;
}
//---------------------------------------------------------------------
void RegOnRtnQueryBankBalanceByFuture( HCTP hCtp, pfOnRtnQueryBankBalanceByFuture callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnQueryBankBalanceByFuture = callback;
}
//---------------------------------------------------------------------
void RegOnErrRtnBankToFutureByFuture( HCTP hCtp, pfOnErrRtnBankToFutureByFuture callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnErrRtnBankToFutureByFuture = callback;
}
//---------------------------------------------------------------------
void RegOnErrRtnFutureToBankByFuture( HCTP hCtp, pfOnErrRtnFutureToBankByFuture callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnErrRtnFutureToBankByFuture = callback;
}
//---------------------------------------------------------------------
void RegOnErrRtnRepealBankToFutureByFutureManual( HCTP hCtp, pfOnErrRtnRepealBankToFutureByFutureManual callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnErrRtnRepealBankToFutureByFutureManual = callback;
}
//---------------------------------------------------------------------
void RegOnErrRtnRepealFutureToBankByFutureManual( HCTP hCtp, pfOnErrRtnRepealFutureToBankByFutureManual callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnErrRtnRepealFutureToBankByFutureManual = callback;
}
//---------------------------------------------------------------------
void RegOnErrRtnQueryBankBalanceByFuture( HCTP hCtp, pfOnErrRtnQueryBankBalanceByFuture callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnErrRtnQueryBankBalanceByFuture = callback;
}
//---------------------------------------------------------------------
void RegOnRtnRepealFromBankToFutureByFuture( HCTP hCtp, pfOnRtnRepealFromBankToFutureByFuture callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnRepealFromBankToFutureByFuture = callback;
}
//---------------------------------------------------------------------
void RegOnRtnRepealFromFutureToBankByFuture( HCTP hCtp, pfOnRtnRepealFromFutureToBankByFuture callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnRepealFromFutureToBankByFuture = callback;
}
//---------------------------------------------------------------------
void RegOnRspFromBankToFutureByFuture( HCTP hCtp, pfOnRspFromBankToFutureByFuture callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspFromBankToFutureByFuture = callback;
}
//---------------------------------------------------------------------
void RegOnRspFromFutureToBankByFuture( HCTP hCtp, pfOnRspFromFutureToBankByFuture callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspFromFutureToBankByFuture = callback;
}
//---------------------------------------------------------------------
void RegOnRspQueryBankAccountMoneyByFuture( HCTP hCtp, pfOnRspQueryBankAccountMoneyByFuture callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRspQueryBankAccountMoneyByFuture = callback;
}
//---------------------------------------------------------------------
void RegOnRtnOpenAccountByBank( HCTP hCtp, pfOnRtnOpenAccountByBank callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnOpenAccountByBank = callback;
}
//---------------------------------------------------------------------
void RegOnRtnCancelAccountByBank( HCTP hCtp, pfOnRtnCancelAccountByBank callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnCancelAccountByBank = callback;
}
//---------------------------------------------------------------------
void RegOnRtnChangeAccountByBank( HCTP hCtp, pfOnRtnChangeAccountByBank callback )
{
	CTPHandle* hCTP = reinterpret_cast<CTPHandle*>(hCtp);
	hCTP->OnRtnChangeAccountByBank = callback;
}
//---------------------------------------------------------------------