//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>

#pragma hdrstop

#include "CTPBCBTraderAPI.h"
#include "CTPBCBEvents.h"
#include "APIParams.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
extern HINSTANCE  gCTPDLLInstance;
extern HINSTANCE  gHSctpDLLInstance;
extern HINSTANCE  gKNctpDLLInstance;
//---------------------------------------------------------------------------
int CTPTraderApiInit::InitFunctions( void )
{
	if((pCreateCTPTraderApi = (lpCreateCTPTraderApi)::GetProcAddress(gCTPDLLInstance, "CreateCTPTraderApi"))== NULL )
		return 1;
	if((pReleaseCTPTraderApi = (lpReleaseCTPTraderApi)::GetProcAddress(gCTPDLLInstance, "ReleaseCTPTraderApi"))== NULL )
		return 2;
	if((pInit = (lpInit)::GetProcAddress(gCTPDLLInstance, "Init"))== NULL )
		return 3;
	if((pGetTradingDay = (lpGetTradingDay)::GetProcAddress(gCTPDLLInstance, "GetTradingDay"))== NULL )
		return 4;
	if((pRegisterFront = (lpRegisterFront)::GetProcAddress(gCTPDLLInstance, "RegisterFront"))== NULL )
		return 5;
	if((pRegisterNameServer = (lpRegisterNameServer)::GetProcAddress(gCTPDLLInstance, "RegisterNameServer"))== NULL )
		return 6;
	if((pSubscribePrivateTopic = (lpSubscribePrivateTopic)::GetProcAddress(gCTPDLLInstance, "SubscribePrivateTopic"))== NULL )
		return 7;
	if((pSubscribePublicTopic = (lpSubscribePublicTopic)::GetProcAddress(gCTPDLLInstance, "SubscribePublicTopic"))== NULL )
		return 8;
	if((pReqAuthenticate = (lpReqAuthenticate)::GetProcAddress(gCTPDLLInstance, "ReqAuthenticate"))== NULL )
		return 9;
	if((pReqUserLogin = (lpReqUserLogin)::GetProcAddress(gCTPDLLInstance, "ReqUserLogin"))== NULL )
		return 10;
	if((pReqUserLogout = (lpReqUserLogout)::GetProcAddress(gCTPDLLInstance, "ReqUserLogout"))== NULL )
		return 11;
	if((pReqUserPasswordUpdate = (lpReqUserPasswordUpdate)::GetProcAddress(gCTPDLLInstance, "ReqUserPasswordUpdate"))== NULL )
		return 12;
	if((pReqTradingAccountPasswordUpdate = (lpReqTradingAccountPasswordUpdate)::GetProcAddress(gCTPDLLInstance, "ReqTradingAccountPasswordUpdate"))== NULL )
		return 13;
	if((pReqOrderInsert = (lpReqOrderInsert)::GetProcAddress(gCTPDLLInstance, "ReqOrderInsert"))== NULL )
		return 14;
	if((pReqParkedOrderInsert = (lpReqParkedOrderInsert)::GetProcAddress(gCTPDLLInstance, "ReqParkedOrderInsert"))== NULL )
		return 15;
	if((pReqParkedOrderAction = (lpReqParkedOrderAction)::GetProcAddress(gCTPDLLInstance, "ReqParkedOrderAction"))== NULL )
		return 16;
	if((pReqOrderAction = (lpReqOrderAction)::GetProcAddress(gCTPDLLInstance, "ReqOrderAction"))== NULL )
		return 17;
	if((pReqQueryMaxOrderVolume = (lpReqQueryMaxOrderVolume)::GetProcAddress(gCTPDLLInstance, "ReqQueryMaxOrderVolume"))== NULL )
		return 18;
	if((pReqSettlementInfoConfirm = (lpReqSettlementInfoConfirm)::GetProcAddress(gCTPDLLInstance, "ReqSettlementInfoConfirm"))== NULL )
		return 19;
	if((pReqRemoveParkedOrder = (lpReqRemoveParkedOrder)::GetProcAddress(gCTPDLLInstance, "ReqRemoveParkedOrder"))== NULL )
		return 20;
	if((pReqRemoveParkedOrderAction = (lpReqRemoveParkedOrderAction)::GetProcAddress(gCTPDLLInstance, "ReqRemoveParkedOrderAction"))== NULL )
		return 21;
	if((pReqQryOrder = (lpReqQryOrder)::GetProcAddress(gCTPDLLInstance, "ReqQryOrder"))== NULL )
		return 22;
	if((pReqQryTrade = (lpReqQryTrade)::GetProcAddress(gCTPDLLInstance, "ReqQryTrade"))== NULL )
		return 23;
	if((pReqQryInvestorPosition = (lpReqQryInvestorPosition)::GetProcAddress(gCTPDLLInstance, "ReqQryInvestorPosition"))== NULL )
		return 24;
	if((pReqQryTradingAccount = (lpReqQryTradingAccount)::GetProcAddress(gCTPDLLInstance, "ReqQryTradingAccount"))== NULL )
		return 25;
	if((pReqQryInvestor = (lpReqQryInvestor)::GetProcAddress(gCTPDLLInstance, "ReqQryInvestor"))== NULL )
		return 26;
	if((pReqQryTradingCode = (lpReqQryTradingCode)::GetProcAddress(gCTPDLLInstance, "ReqQryTradingCode"))== NULL )
		return 27;
	if((pReqQryInstrumentMarginRate = (lpReqQryInstrumentMarginRate)::GetProcAddress(gCTPDLLInstance, "ReqQryInstrumentMarginRate"))== NULL )
		return 28;
	if((pReqQryInstrumentCommissionRate = (lpReqQryInstrumentCommissionRate)::GetProcAddress(gCTPDLLInstance, "ReqQryInstrumentCommissionRate"))== NULL )
		return 29;
	if((pReqQryExchange = (lpReqQryExchange)::GetProcAddress(gCTPDLLInstance, "ReqQryExchange"))== NULL )
		return 30;
	if((pReqQryInstrument = (lpReqQryInstrument)::GetProcAddress(gCTPDLLInstance, "ReqQryInstrument"))== NULL )
		return 31;
	if((pReqQryDepthMarketData = (lpReqQryDepthMarketData)::GetProcAddress(gCTPDLLInstance, "ReqQryDepthMarketData"))== NULL )
		return 32;
	if((pReqQrySettlementInfo = (lpReqQrySettlementInfo)::GetProcAddress(gCTPDLLInstance, "ReqQrySettlementInfo"))== NULL )
		return 33;
	if((pReqQryTransferBank = (lpReqQryTransferBank)::GetProcAddress(gCTPDLLInstance, "ReqQryTransferBank"))== NULL )
		return 34;
	if((pReqQryInvestorPositionDetail = (lpReqQryInvestorPositionDetail)::GetProcAddress(gCTPDLLInstance, "ReqQryInvestorPositionDetail"))== NULL )
		return 35;
	if((pReqQryNotice = (lpReqQryNotice)::GetProcAddress(gCTPDLLInstance, "ReqQryNotice"))== NULL )
		return 36;
	if((pReqQrySettlementInfoConfirm = (lpReqQrySettlementInfoConfirm)::GetProcAddress(gCTPDLLInstance, "ReqQrySettlementInfoConfirm"))== NULL )
		return 37;
	if((pReqQryInvestorPositionCombineDetail = (lpReqQryInvestorPositionCombineDetail)::GetProcAddress(gCTPDLLInstance, "ReqQryInvestorPositionCombineDetail"))== NULL )
		return 38;
	if((pReqQryCFMMCTradingAccountKey = (lpReqQryCFMMCTradingAccountKey)::GetProcAddress(gCTPDLLInstance, "ReqQryCFMMCTradingAccountKey"))== NULL )
		return 39;
	if((pReqQryEWarrantOffset = (lpReqQryEWarrantOffset)::GetProcAddress(gCTPDLLInstance, "ReqQryEWarrantOffset"))== NULL )
		return 40;
	if((pReqQryTransferSerial = (lpReqQryTransferSerial)::GetProcAddress(gCTPDLLInstance, "ReqQryTransferSerial"))== NULL )
		return 41;
	if((pReqQryAccountregister = (lpReqQryAccountregister)::GetProcAddress(gCTPDLLInstance, "ReqQryAccountregister"))== NULL )
		return 42;
	if((pReqQryContractBank = (lpReqQryContractBank)::GetProcAddress(gCTPDLLInstance, "ReqQryContractBank"))== NULL )
		return 43;
	if((pReqQryParkedOrder = (lpReqQryParkedOrder)::GetProcAddress(gCTPDLLInstance, "ReqQryParkedOrder"))== NULL )
		return 44;
	if((pReqQryParkedOrderAction = (lpReqQryParkedOrderAction)::GetProcAddress(gCTPDLLInstance, "ReqQryParkedOrderAction"))== NULL )
		return 45;
	if((pReqQryTradingNotice = (lpReqQryTradingNotice)::GetProcAddress(gCTPDLLInstance, "ReqQryTradingNotice"))== NULL )
		return 46;
	if((pReqQryBrokerTradingParams = (lpReqQryBrokerTradingParams)::GetProcAddress(gCTPDLLInstance, "ReqQryBrokerTradingParams"))== NULL )
		return 47;
	if((pReqQryBrokerTradingAlgos = (lpReqQryBrokerTradingAlgos)::GetProcAddress(gCTPDLLInstance, "ReqQryBrokerTradingAlgos"))== NULL )
		return 48;
	if((pReqFromBankToFutureByFuture = (lpReqFromBankToFutureByFuture)::GetProcAddress(gCTPDLLInstance, "ReqFromBankToFutureByFuture"))== NULL )
		return 49;
	if((pReqFromFutureToBankByFuture = (lpReqFromFutureToBankByFuture)::GetProcAddress(gCTPDLLInstance, "ReqFromFutureToBankByFuture"))== NULL )
		return 50;
	if((pReqQueryBankAccountMoneyByFuture = (lpReqQueryBankAccountMoneyByFuture)::GetProcAddress(gCTPDLLInstance, "ReqQueryBankAccountMoneyByFuture"))== NULL )
		return 51;
	return 0;
}
//---------------------------------------------------------------------------
CTPTraderApiInit::CTPTraderApiInit( UFC::AnsiString APIType )
:FSupport( false )
{
	// Dynamic load dll by APIType
	char strDllName[32];
	if ( APIType == "CTP" )
	{
		gCTPDLLInstance = ::LoadLibraryW( L"CTPAPIFactory.dll" );
		strcpy( strDllName, "CTPAPIFactory.dll" );
	}
	else if ( APIType == "HSctp" )
	{
		gCTPDLLInstance = ::LoadLibraryW( L"HSctpAPIFactory.dll" );
		strcpy( strDllName, "HSctpAPIFactory.dll" );
	}
	else if ( APIType == "KNctp" )
	{
		gCTPDLLInstance = ::LoadLibraryW( L"KNctpAPIFactory.dll" );
		strcpy( strDllName, "KNctpAPIFactory.dll" );
	}

	if ( gCTPDLLInstance != NULL )
	{
		if( (FFuncRtn  = InitFunctions()) != 0 )
		{
			sprintf( FNotSupportReason, "Dynamic bind function %d failed.", FFuncRtn  );
			return;
		}
		if( (FRegCBRtn = InitRegCallbackFunctions()) != 0 )
		{
			sprintf( FNotSupportReason, "Dynamic bind regist callback function %d failed.", FRegCBRtn );
			return;
		}
		sprintf( FNotSupportReason, "Load %s OK!", strDllName );
		FSupport = true; ///< Load CTP dll OK!
	}
	else
		sprintf( FNotSupportReason, "Can't dynamic load %s", strDllName );
}
//------------------------------------------------------------------------------
CTPTraderApiInit::~CTPTraderApiInit()
{

}
//---------------------------------------------------------------------------
// Static call back functions
//---------------------------------------------------------------------------
bool TriggerEvent( void* Owner, int MSG, TCTPParams* Params )
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;

	if( CTPObj != NULL )
	{
		TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
		if( Listener != NULL )
		{
			PostMessage( CTPObj->GetHWND(), MSG, (WPARAM)Listener,(LPARAM)Params );
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnFrontConnected( void* Owner )
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;

	if( CTPObj != NULL )
	{
		TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
		if( Listener != NULL )
		{
			Listener->OnAPIConnected();
			PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT, (WPARAM)Listener,(LPARAM)NULL );
		}
	}
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnFrontDisconnected( void* Owner, int nReason)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;

	if( CTPObj != NULL )
	{
		TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
		if( Listener != NULL )
		{
			TCTPParams* Params = new TCTPParams( nReason );

			Listener->OnAPIDisconnected( nReason );
			PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+1, (WPARAM)Listener,(LPARAM)Params );
		}
	}
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspUserLogin(void* Owner,CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pRspUserLogin,sizeof(CThostFtdcRspUserLoginField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+2, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspUserLogout( void* Owner, CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pUserLogout,sizeof(CThostFtdcUserLogoutField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+3, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQrySettlementInfo(void* Owner,CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pSettlementInfo,sizeof(CThostFtdcSettlementInfoField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+4, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspSettlementInfoConfirm(void* Owner,CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pSettlementInfoConfirm,sizeof(CThostFtdcSettlementInfoConfirmField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+5, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspOrderInsert(void* Owner,CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pInputOrder,sizeof(CThostFtdcInputOrderField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+6, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnErrRtnOrderInsert(void* Owner,CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	TCTPParams* Params = new TCTPParams( (void*)pInputOrder,sizeof(CThostFtdcInputOrderField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField) );

	TriggerEvent( Owner, WM_CTP_EVENT+7, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspOrderAction(void* Owner,CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pInputOrderAction,sizeof(CThostFtdcInputOrderActionField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+8, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnErrRtnOrderAction(void* Owner,CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
	TCTPParams* Params = new TCTPParams( (void*)pOrderAction,sizeof(CThostFtdcOrderActionField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField) );

	TriggerEvent( Owner, WM_CTP_EVENT+9, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryOrder(void* Owner,CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pOrder,sizeof(CThostFtdcOrderField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+10, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryExchange(void* Owner,CThostFtdcExchangeField *pExchange, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pExchange,sizeof(CThostFtdcExchangeField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+11, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryInstrument(void* Owner,CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pInstrument,sizeof(CThostFtdcInstrumentField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+12, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryInvestorPosition(void* Owner,CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorPosition,sizeof(CThostFtdcInvestorPositionField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+13, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspError(void* Owner,CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pRspInfo,sizeof(CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+14, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnOrder(void* Owner,CThostFtdcOrderField *pOrder)
{
	TCTPParams*          Params = new TCTPParams( (void*)pOrder,sizeof(CThostFtdcOrderField) );

	TriggerEvent( Owner, WM_CTP_EVENT+15, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnTrade(void* Owner,CThostFtdcTradeField *pTrade)
{
	TCTPParams*          Params = new TCTPParams( (void*)pTrade,sizeof(CThostFtdcTradeField) );

	TriggerEvent( Owner, WM_CTP_EVENT+16, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnHeartBeatWarning(void* Owner,int nTimeLapse)
{
	TCTPParams*          Params = new TCTPParams( nTimeLapse );

	TriggerEvent( Owner, WM_CTP_EVENT+17, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspAuthenticate(void* Owner,CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pRspAuthenticateField,sizeof(CThostFtdcRspAuthenticateField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+18, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspUserPasswordUpdate(void* Owner,CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pUserPasswordUpdate,sizeof(CThostFtdcUserPasswordUpdateField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+19, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspTradingAccountPasswordUpdate(void* Owner,CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pTradingAccountPasswordUpdate,sizeof(CThostFtdcTradingAccountPasswordUpdateField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+20, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspParkedOrderInsert(void* Owner,CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pParkedOrder,sizeof(CThostFtdcParkedOrderField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+21, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspParkedOrderAction(void* Owner,CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pParkedOrderAction,sizeof(CThostFtdcParkedOrderActionField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+22, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQueryMaxOrderVolume(void* Owner,CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pQueryMaxOrderVolume,sizeof(CThostFtdcQueryMaxOrderVolumeField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+23, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspRemoveParkedOrder(void* Owner,CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pRemoveParkedOrder,sizeof(CThostFtdcRemoveParkedOrderField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+24, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspRemoveParkedOrderAction(void* Owner,CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pRemoveParkedOrderAction,sizeof(CThostFtdcRemoveParkedOrderActionField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+25, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryTrade(void* Owner,CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pTrade,sizeof(CThostFtdcTradeField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+26, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryTradingAccount(void* Owner,CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pTradingAccount,sizeof(CThostFtdcTradingAccountField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+27, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryInvestor(void* Owner,CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pInvestor,sizeof(CThostFtdcInvestorField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+28, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryTradingCode(void* Owner,CThostFtdcTradingCodeField *pTradingCode, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pTradingCode,sizeof(CThostFtdcTradingCodeField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+29, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryInstrumentMarginRate(void* Owner,CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pInstrumentMarginRate,sizeof(CThostFtdcInstrumentMarginRateField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+30, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryInstrumentCommissionRate(void* Owner,CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pInstrumentCommissionRate,sizeof(CThostFtdcInstrumentCommissionRateField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+31, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryDepthMarketData(void* Owner,CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pDepthMarketData,sizeof(CThostFtdcDepthMarketDataField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+32, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryTransferBank(void* Owner,CThostFtdcTransferBankField *pTransferBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pTransferBank,sizeof(CThostFtdcTransferBankField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+33, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryInvestorPositionDetail(void* Owner,CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pInvestorPositionDetail,sizeof(CThostFtdcInvestorPositionDetailField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+34, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryNotice(void* Owner,CThostFtdcNoticeField *pNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pNotice,sizeof(CThostFtdcNoticeField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+35, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQrySettlementInfoConfirm(void* Owner,CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pSettlementInfoConfirm,sizeof(CThostFtdcSettlementInfoConfirmField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+36, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryInvestorPositionCombineDetail(void* Owner,CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pInvestorPositionCombineDetail,sizeof(CThostFtdcInvestorPositionCombineDetailField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+37, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryCFMMCTradingAccountKey(void* Owner,CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pCFMMCTradingAccountKey,sizeof(CThostFtdcCFMMCTradingAccountKeyField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+38, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryEWarrantOffset(void* Owner,CThostFtdcEWarrantOffsetField *pEWarrantOffset, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pEWarrantOffset,sizeof(CThostFtdcEWarrantOffsetField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+39, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryTransferSerial(void* Owner,CThostFtdcTransferSerialField *pTransferSerial, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pTransferSerial,sizeof(CThostFtdcTransferSerialField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+40, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryAccountregister(void* Owner,CThostFtdcAccountregisterField *pAccountregister, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pAccountregister,sizeof(CThostFtdcAccountregisterField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast );

	TriggerEvent( Owner, WM_CTP_EVENT+41, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnInstrumentStatus(void* Owner,CThostFtdcInstrumentStatusField *pInstrumentStatus)
{
	TCTPParams*          Params = new TCTPParams( (void*)pInstrumentStatus,sizeof(CThostFtdcInstrumentStatusField) );

	TriggerEvent( Owner, WM_CTP_EVENT+42, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnTradingNotice(void* Owner,CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo)
{
	TCTPParams*          Params = new TCTPParams( (void*)pTradingNoticeInfo,sizeof(CThostFtdcTradingNoticeInfoField) );

	TriggerEvent( Owner, WM_CTP_EVENT+43, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnErrorConditionalOrder(void* Owner,CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder)
{
	TCTPParams*          Params = new TCTPParams( (void*)pErrorConditionalOrder,sizeof(CThostFtdcErrorConditionalOrderField) );

	TriggerEvent( Owner, WM_CTP_EVENT+44, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryContractBank(void* Owner,CThostFtdcContractBankField *pContractBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pContractBank,sizeof(CThostFtdcContractBankField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast  );

	TriggerEvent( Owner, WM_CTP_EVENT+45, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryParkedOrder(void* Owner,CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pParkedOrder,sizeof(CThostFtdcParkedOrderField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast  );

	TriggerEvent( Owner, WM_CTP_EVENT+46, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryParkedOrderAction(void* Owner,CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pParkedOrderAction,sizeof(CThostFtdcParkedOrderActionField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast  );

	TriggerEvent( Owner, WM_CTP_EVENT+47, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryTradingNotice(void* Owner,CThostFtdcTradingNoticeField *pTradingNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams*          Params = new TCTPParams( (void*)pTradingNotice,sizeof(CThostFtdcTradingNoticeField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast  );

	TriggerEvent( Owner, WM_CTP_EVENT+48, Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryBrokerTradingParams(void* Owner,CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pBrokerTradingParams,sizeof(CThostFtdcBrokerTradingParamsField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast  );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+49, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQryBrokerTradingAlgos(void* Owner,CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pBrokerTradingAlgos,sizeof(CThostFtdcBrokerTradingAlgosField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast  );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+50, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnFromBankToFutureByBank(void* Owner,CThostFtdcRspTransferField *pRspTransfer)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pRspTransfer,sizeof(CThostFtdcRspTransferField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+51, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnFromFutureToBankByBank(void* Owner,CThostFtdcRspTransferField *pRspTransfer)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pRspTransfer,sizeof(CThostFtdcRspTransferField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+52, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnRepealFromBankToFutureByBank(void* Owner,CThostFtdcRspRepealField *pRspRepeal)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pRspRepeal,sizeof(CThostFtdcRspRepealField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+53, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnRepealFromFutureToBankByBank(void* Owner,CThostFtdcRspRepealField *pRspRepeal)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pRspRepeal,sizeof(CThostFtdcRspRepealField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+54, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnFromBankToFutureByFuture(void* Owner,CThostFtdcRspTransferField *pRspTransfer)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pRspTransfer,sizeof(CThostFtdcRspTransferField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+55, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnFromFutureToBankByFuture(void* Owner,CThostFtdcRspTransferField *pRspTransfer)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pRspTransfer,sizeof(CThostFtdcRspTransferField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+56, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnRepealFromBankToFutureByFutureManual(void* Owner,CThostFtdcRspRepealField *pRspRepeal)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pRspRepeal,sizeof(CThostFtdcRspRepealField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+57, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnRepealFromFutureToBankByFutureManual(void* Owner,CThostFtdcRspRepealField *pRspRepeal)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pRspRepeal,sizeof(CThostFtdcRspRepealField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+58, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnQueryBankBalanceByFuture(void* Owner,CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pNotifyQueryAccount,sizeof(CThostFtdcNotifyQueryAccountField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+59, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnErrRtnBankToFutureByFuture(void* Owner,CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pReqTransfer,sizeof(CThostFtdcReqTransferField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+60, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnErrRtnFutureToBankByFuture(void* Owner,CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pReqTransfer,sizeof(CThostFtdcReqTransferField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+61, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnErrRtnRepealBankToFutureByFutureManual(void* Owner,CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pReqRepeal,sizeof(CThostFtdcReqRepealField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+62, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnErrRtnRepealFutureToBankByFutureManual(void* Owner,CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pReqRepeal,sizeof(CThostFtdcReqRepealField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+63, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnErrRtnQueryBankBalanceByFuture(void* Owner,CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pReqQueryAccount,sizeof(CThostFtdcReqQueryAccountField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+64, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnRepealFromBankToFutureByFuture(void* Owner,CThostFtdcRspRepealField *pRspRepeal)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pRspRepeal,sizeof(CThostFtdcRspRepealField));

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+65, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnRepealFromFutureToBankByFuture(void* Owner,CThostFtdcRspRepealField *pRspRepeal)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pRspRepeal,sizeof(CThostFtdcRspRepealField));

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+66, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspFromBankToFutureByFuture(void* Owner,CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pReqTransfer,sizeof(CThostFtdcReqTransferField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast  );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+67, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspFromFutureToBankByFuture(void* Owner,CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pReqTransfer,sizeof(CThostFtdcReqTransferField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast  );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+68, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRspQueryBankAccountMoneyByFuture(void* Owner,CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pReqQueryAccount,sizeof(CThostFtdcReqQueryAccountField),(void*)pRspInfo,sizeof(CThostFtdcRspInfoField),nRequestID, bIsLast  );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+69, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnOpenAccountByBank(void* Owner,CThostFtdcOpenAccountField *pOpenAccount)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pOpenAccount,sizeof(CThostFtdcOpenAccountField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+70, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnCancelAccountByBank(void* Owner,CThostFtdcCancelAccountField *pCancelAccount)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pCancelAccount,sizeof(CThostFtdcCancelAccountField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+71, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
void CTPTraderApiInit::OnRtnChangeAccountByBank(void* Owner,CThostFtdcChangeAccountField *pChangeAccount)
{
	TThostFtdcTraderApi* CTPObj = (TThostFtdcTraderApi*)Owner;
	TThostFtdcTraderSpi* Listener = CTPObj->GetSpi( );
	TCTPParams*          Params = new TCTPParams( (void*)pChangeAccount,sizeof(CThostFtdcChangeAccountField) );

	PostMessage( CTPObj->GetHWND(), WM_CTP_EVENT+72, (WPARAM)Listener,(LPARAM)Params );
}
//---------------------------------------------------------------------------
int CTPTraderApiInit::InitRegCallbackFunctions( void )
{
	if( (pRegOnFrontConnected = (lpRegOnFrontConnected)::GetProcAddress(gCTPDLLInstance, "RegOnFrontConnected"))== NULL )
		return 1;
	if( (pRegOnFrontDisconnected = (lpRegOnFrontDisconnected)::GetProcAddress(gCTPDLLInstance, "RegOnFrontDisconnected"))== NULL )
		return 2;
	if( (pRegOnRspUserLogin = (lpRegOnRspUserLogin)::GetProcAddress(gCTPDLLInstance, "RegOnRspUserLogin"))== NULL )
		return 3;
	if( (pRegOnRspUserLogout = (lpRegOnRspUserLogout)::GetProcAddress(gCTPDLLInstance, "RegOnRspUserLogout"))== NULL )
		return 4;
	if( (pRegOnRspQrySettlementInfo = (lpRegOnRspQrySettlementInfo)::GetProcAddress(gCTPDLLInstance, "RegOnRspQrySettlementInfo"))== NULL )
		return 5;
	if( (pRegOnRspSettlementInfoConfirm = (lpRegOnRspSettlementInfoConfirm)::GetProcAddress(gCTPDLLInstance, "RegOnRspSettlementInfoConfirm"))== NULL )
		return 6;
	if( (pRegOnRspOrderInsert = (lpRegOnRspOrderInsert)::GetProcAddress(gCTPDLLInstance, "RegOnRspOrderInsert"))== NULL )
		return 7;
	if( (pRegOnErrRtnOrderInsert = (lpRegOnErrRtnOrderInsert)::GetProcAddress(gCTPDLLInstance, "RegOnErrRtnOrderInsert"))== NULL )
		return 8;
	if( (pRegOnRspOrderAction = (lpRegOnRspOrderAction)::GetProcAddress(gCTPDLLInstance, "RegOnRspOrderAction"))== NULL )
		return 9;
	if( (pRegOnErrRtnOrderAction = (lpRegOnErrRtnOrderAction)::GetProcAddress(gCTPDLLInstance, "RegOnErrRtnOrderAction"))== NULL )
		return 10;
	if( (pRegOnRspQryOrder = (lpRegOnRspQryOrder)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryOrder"))== NULL )
		return 11;
	if( (pRegOnRspQryExchange = (lpRegOnRspQryExchange)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryExchange"))== NULL )
		return 12;
	if( (pRegOnRspQryInstrument = (lpRegOnRspQryInstrument)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryInstrument"))== NULL )
		return 13;
	if( (pRegOnRspQryInvestorPosition = (lpRegOnRspQryInvestorPosition)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryInvestorPosition"))== NULL )
		return 14;
	if( (pRegOnRtnOrder = (lpRegOnRtnOrder)::GetProcAddress(gCTPDLLInstance, "RegOnRtnOrder"))== NULL )
		return 15;
	if( (pRegOnRtnTrade = (lpRegOnRtnTrade)::GetProcAddress(gCTPDLLInstance, "RegOnRtnTrade"))== NULL )
		return 16;
	if( (pRegOnRspError = (lpRegOnRspError)::GetProcAddress(gCTPDLLInstance, "RegOnRspError"))== NULL )
		return 17;
	if( (pRegOnHeartBeatWarning = (lpRegOnHeartBeatWarning)::GetProcAddress(gCTPDLLInstance, "RegOnHeartBeatWarning"))== NULL )
		return 18;
	if( (pRegOnRspAuthenticate = (lpRegOnRspAuthenticate)::GetProcAddress(gCTPDLLInstance, "RegOnRspAuthenticate"))== NULL )
		return 19;
	if( (pRegOnRspUserPasswordUpdate = (lpRegOnRspUserPasswordUpdate)::GetProcAddress(gCTPDLLInstance, "RegOnRspUserPasswordUpdate"))== NULL )
		return 20;
	if( (pRegOnRspTradingAccountPasswordUpdate = (lpRegOnRspTradingAccountPasswordUpdate)::GetProcAddress(gCTPDLLInstance, "RegOnRspTradingAccountPasswordUpdate"))== NULL )
		return 21;
	if( (pRegOnRspParkedOrderInsert = (lpRegOnRspParkedOrderInsert)::GetProcAddress(gCTPDLLInstance, "RegOnRspParkedOrderInsert"))== NULL )
		return 22;
	if( (pRegOnRspParkedOrderAction = (lpRegOnRspParkedOrderAction)::GetProcAddress(gCTPDLLInstance, "RegOnRspParkedOrderAction"))== NULL )
		return 23;
	if( (pRegOnRspQueryMaxOrderVolume = (lpRegOnRspQueryMaxOrderVolume)::GetProcAddress(gCTPDLLInstance, "RegOnRspQueryMaxOrderVolume"))== NULL )
		return 24;
	if( (pRegOnRspRemoveParkedOrder = (lpRegOnRspRemoveParkedOrder)::GetProcAddress(gCTPDLLInstance, "RegOnRspRemoveParkedOrder"))== NULL )
		return 25;
	if( (pRegOnRspRemoveParkedOrderAction = (lpRegOnRspRemoveParkedOrderAction)::GetProcAddress(gCTPDLLInstance, "RegOnRspRemoveParkedOrderAction"))== NULL )
		return 26;
	if( (pRegOnRspQryTrade = (lpRegOnRspQryTrade)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryTrade"))== NULL )
		return 27;
	if( (pRegOnRspQryTradingAccount = (lpRegOnRspQryTradingAccount)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryTradingAccount"))== NULL )
		return 28;
	if( (pRegOnRspQryInvestor = (lpRegOnRspQryInvestor)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryInvestor"))== NULL )
		return 29;
	if( (pRegOnRspQryTradingCode = (lpRegOnRspQryTradingCode)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryTradingCode"))== NULL )
		return 30;
	if( (pRegOnRspQryInstrumentMarginRate = (lpRegOnRspQryInstrumentMarginRate)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryInstrumentMarginRate"))== NULL )
		return 31;
	if( (pRegOnRspQryInstrumentCommissionRate = (lpRegOnRspQryInstrumentCommissionRate)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryInstrumentCommissionRate"))== NULL )
		return 32;
	if( (pRegOnRspQryDepthMarketData = (lpRegOnRspQryDepthMarketData)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryDepthMarketData"))== NULL )
		return 33;
	if( (pRegOnRspQryTransferBank = (lpRegOnRspQryTransferBank)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryTransferBank"))== NULL )
		return 34;
	if( (pRegOnRspQryInvestorPositionDetail = (lpRegOnRspQryInvestorPositionDetail)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryInvestorPositionDetail"))== NULL )
		return 35;
	if( (pRegOnRspQryNotice = (lpRegOnRspQryNotice)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryNotice"))== NULL )
		return 36;
	if( (pRegOnRspQrySettlementInfoConfirm = (lpRegOnRspQrySettlementInfoConfirm)::GetProcAddress(gCTPDLLInstance, "RegOnRspQrySettlementInfoConfirm"))== NULL )
		return 37;
	if( (pRegOnRspQryInvestorPositionCombineDetail = (lpRegOnRspQryInvestorPositionCombineDetail)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryInvestorPositionCombineDetail"))== NULL )
		return 38;
	if( (pRegOnRspQryCFMMCTradingAccountKey = (lpRegOnRspQryCFMMCTradingAccountKey)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryCFMMCTradingAccountKey"))== NULL )
		return 39;
	if( (pRegOnRspQryEWarrantOffset = (lpRegOnRspQryEWarrantOffset)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryEWarrantOffset"))== NULL )
		return 40;
	if( (pRegOnRspQryTransferSerial = (lpRegOnRspQryTransferSerial)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryTransferSerial"))== NULL )
		return 41;
	if( (pRegOnRspQryAccountregister = (lpRegOnRspQryAccountregister)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryAccountregister"))== NULL )
		return 42;
	if( (pRegOnRtnInstrumentStatus = (lpRegOnRtnInstrumentStatus)::GetProcAddress(gCTPDLLInstance, "RegOnRtnInstrumentStatus"))== NULL )
		return 43;
	if( (pRegOnRtnTradingNotice = (lpRegOnRtnTradingNotice)::GetProcAddress(gCTPDLLInstance, "RegOnRtnTradingNotice"))== NULL )
		return 44;
	if( (pRegOnRtnErrorConditionalOrder = (lpRegOnRtnErrorConditionalOrder)::GetProcAddress(gCTPDLLInstance, "RegOnRtnErrorConditionalOrder"))== NULL )
		return 45;
	if( (pRegOnRspQryContractBank = (lpRegOnRspQryContractBank)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryContractBank"))== NULL )
		return 46;
	if( (pRegOnRspQryParkedOrder = (lpRegOnRspQryParkedOrder)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryParkedOrder"))== NULL )
		return 47;
	if( (pRegOnRspQryParkedOrderAction = (lpRegOnRspQryParkedOrderAction)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryParkedOrderAction"))== NULL )
		return 48;
	if( (pRegOnRspQryTradingNotice = (lpRegOnRspQryTradingNotice)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryTradingNotice"))== NULL )
		return 49;
	if( (pRegOnRspQryBrokerTradingParams = (lpRegOnRspQryBrokerTradingParams)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryBrokerTradingParams"))== NULL )
		return 50;
	if( (pRegOnRspQryBrokerTradingAlgos = (lpRegOnRspQryBrokerTradingAlgos)::GetProcAddress(gCTPDLLInstance, "RegOnRspQryBrokerTradingAlgos"))== NULL )
		return 51;
	if( (pRegOnRtnFromBankToFutureByBank = (lpRegOnRtnFromBankToFutureByBank)::GetProcAddress(gCTPDLLInstance, "RegOnRtnFromBankToFutureByBank"))== NULL )
		return 52;
	if( (pRegOnRtnFromFutureToBankByBank = (lpRegOnRtnFromFutureToBankByBank)::GetProcAddress(gCTPDLLInstance, "RegOnRtnFromFutureToBankByBank"))== NULL )
		return 53;
	if( (pRegOnRtnRepealFromBankToFutureByBank = (lpRegOnRtnRepealFromBankToFutureByBank)::GetProcAddress(gCTPDLLInstance, "RegOnRtnRepealFromBankToFutureByBank"))== NULL )
		return 54;
	if( (pRegOnRtnRepealFromFutureToBankByBank = (lpRegOnRtnRepealFromFutureToBankByBank)::GetProcAddress(gCTPDLLInstance, "RegOnRtnRepealFromFutureToBankByBank"))== NULL )
		return 55;
	if( (pRegOnRtnFromBankToFutureByFuture = (lpRegOnRtnFromBankToFutureByFuture)::GetProcAddress(gCTPDLLInstance, "RegOnRtnFromBankToFutureByFuture"))== NULL )
		return 56;
	if( (pRegOnRtnFromFutureToBankByFuture = (lpRegOnRtnFromFutureToBankByFuture)::GetProcAddress(gCTPDLLInstance, "RegOnRtnFromFutureToBankByFuture"))== NULL )
		return 57;
	if( (pRegOnRtnRepealFromBankToFutureByFutureManual = (lpRegOnRtnRepealFromBankToFutureByFutureManual)::GetProcAddress(gCTPDLLInstance, "RegOnRtnRepealFromBankToFutureByFutureManual"))== NULL )
		return 58;
	if( (pRegOnRtnRepealFromFutureToBankByFutureManual = (lpRegOnRtnRepealFromFutureToBankByFutureManual)::GetProcAddress(gCTPDLLInstance, "RegOnRtnRepealFromFutureToBankByFutureManual"))== NULL )
		return 59;
	if( (pRegOnRtnQueryBankBalanceByFuture = (lpRegOnRtnQueryBankBalanceByFuture)::GetProcAddress(gCTPDLLInstance, "RegOnRtnQueryBankBalanceByFuture"))== NULL )
		return 60;
	if( (pRegOnErrRtnBankToFutureByFuture = (lpRegOnErrRtnBankToFutureByFuture)::GetProcAddress(gCTPDLLInstance, "RegOnErrRtnBankToFutureByFuture"))== NULL )
		return 61;
	if( (pRegOnErrRtnFutureToBankByFuture = (lpRegOnErrRtnFutureToBankByFuture)::GetProcAddress(gCTPDLLInstance, "RegOnErrRtnFutureToBankByFuture"))== NULL )
		return 62;
	if( (pRegOnErrRtnRepealBankToFutureByFutureManual = (lpRegOnErrRtnRepealBankToFutureByFutureManual)::GetProcAddress(gCTPDLLInstance, "RegOnErrRtnRepealBankToFutureByFutureManual"))== NULL )
		return 63;
	if( (pRegOnErrRtnRepealFutureToBankByFutureManual = (lpRegOnErrRtnRepealFutureToBankByFutureManual)::GetProcAddress(gCTPDLLInstance, "RegOnErrRtnRepealFutureToBankByFutureManual"))== NULL )
		return 64;
	if( (pRegOnErrRtnQueryBankBalanceByFuture = (lpRegOnErrRtnQueryBankBalanceByFuture)::GetProcAddress(gCTPDLLInstance, "RegOnErrRtnQueryBankBalanceByFuture"))== NULL )
		return 65;
	if( (pRegOnRtnRepealFromBankToFutureByFuture = (lpRegOnRtnRepealFromBankToFutureByFuture)::GetProcAddress(gCTPDLLInstance, "RegOnRtnRepealFromBankToFutureByFuture"))== NULL )
		return 66;
	if( (pRegOnRtnRepealFromFutureToBankByFuture = (lpRegOnRtnRepealFromFutureToBankByFuture)::GetProcAddress(gCTPDLLInstance, "RegOnRtnRepealFromFutureToBankByFuture"))== NULL )
		return 67;
	if( (pRegOnRspFromBankToFutureByFuture = (lpRegOnRspFromBankToFutureByFuture)::GetProcAddress(gCTPDLLInstance, "RegOnRspFromBankToFutureByFuture"))== NULL )
		return 68;
	if( (pRegOnRspFromFutureToBankByFuture = (lpRegOnRspFromFutureToBankByFuture)::GetProcAddress(gCTPDLLInstance, "RegOnRspFromFutureToBankByFuture"))== NULL )
		return 69;
	if( (pRegOnRspQueryBankAccountMoneyByFuture = (lpRegOnRspQueryBankAccountMoneyByFuture)::GetProcAddress(gCTPDLLInstance, "RegOnRspQueryBankAccountMoneyByFuture"))== NULL )
		return 70;
	if( (pRegOnRtnOpenAccountByBank = (lpRegOnRtnOpenAccountByBank)::GetProcAddress(gCTPDLLInstance, "RegOnRtnOpenAccountByBank"))== NULL )
		return 71;
	if( (pRegOnRtnCancelAccountByBank = (lpRegOnRtnCancelAccountByBank)::GetProcAddress(gCTPDLLInstance, "RegOnRtnCancelAccountByBank"))== NULL )
		return 72;
	if( (pRegOnRtnChangeAccountByBank = (lpRegOnRtnChangeAccountByBank)::GetProcAddress(gCTPDLLInstance, "RegOnRtnChangeAccountByBank"))== NULL )
		return 73;
	return 0;
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
