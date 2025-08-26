//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#pragma hdrstop
#include <stdio.h>
#include <string.h>
#include "USTPBCBTraderAPI.h"
#include "USTPBCBEvents.h"
#include "FemasAPIDynamic.h"
#include "APIParams.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
extern HINSTANCE  gUSTPDLLInstance;
//---------------------------------------------------------------------------
int USTPTraderApiInit::InitFunctions( void )
{
	if((pCreateUSTPTraderApi=    (lpCreateUSTPTraderApi     )::GetProcAddress(gUSTPDLLInstance, "CreateUSTPTraderApi"))== NULL )
		return 1;
	if((pGetVersion=             (lpGetVersion              )::GetProcAddress(gUSTPDLLInstance, "GetAPIVersion"))== NULL )
		return 2;
	if((pReleaseUSTPTraderApi=   (lpReleaseUSTPTraderApi    )::GetProcAddress(gUSTPDLLInstance, "ReleaseUSTPTraderApi"))== NULL )
		return 3;
	if((pInit=                   (lpInit                    )::GetProcAddress(gUSTPDLLInstance, "Init"))== NULL )
		return 4;
	if((pGetTradingDay=          (lpGetTradingDay           )::GetProcAddress(gUSTPDLLInstance, "GetTradingDay"))== NULL )
		return 5;
	if((pRegisterFront=          (lpRegisterFront           )::GetProcAddress(gUSTPDLLInstance, "RegisterFront"))== NULL )
		return 6;
	if((pRegisterNameServer=     (lpRegisterNameServer      )::GetProcAddress(gUSTPDLLInstance, "RegisterNameServer"))== NULL )
		return 7;
	if((pRegisterCertificateFile=(lpRegisterCertificateFile )::GetProcAddress(gUSTPDLLInstance, "RegisterCertificateFile"))== NULL )
		return 8;
	if((pSubscribePrivateTopic=  (lpSubscribePrivateTopic   )::GetProcAddress(gUSTPDLLInstance, "SubscribePrivateTopic"))== NULL )
		return 9;
	if((pSubscribePublicTopic=   (lpSubscribePublicTopic    )::GetProcAddress(gUSTPDLLInstance, "SubscribePublicTopic"))== NULL )
		return 10;
	if((pSubscribeUserTopic=     (lpSubscribeUserTopic      )::GetProcAddress(gUSTPDLLInstance, "SubscribeUserTopic"))== NULL )
		return 11;
	if((pSetHeartbeatTimeout=    (lpSetHeartbeatTimeout     )::GetProcAddress(gUSTPDLLInstance, "SetHeartbeatTimeout"))== NULL )
		return 12;
	if((pOpenRequestLog=         (lpOpenRequestLog          )::GetProcAddress(gUSTPDLLInstance, "OpenRequestLog"))== NULL )
		return 13;
	if((pOpenResponseLog=        (lpOpenResponseLog         )::GetProcAddress(gUSTPDLLInstance, "OpenResponseLog"))== NULL )
		return 14;
	if((pReqUserLogin=           (lpReqUserLogin            )::GetProcAddress(gUSTPDLLInstance, "ReqUserLogin"))== NULL )
		return 15;
	if((pReqUserLogout=          (lpReqUserLogout           )::GetProcAddress(gUSTPDLLInstance, "ReqUserLogout"))== NULL )
		return 16;
	if((pReqUserPasswordUpdate=  (lpReqUserPasswordUpdate   )::GetProcAddress(gUSTPDLLInstance, "ReqUserPasswordUpdate"))== NULL )
		return 17;
	if((pReqOrderInsert=         (lpReqOrderInsert          )::GetProcAddress(gUSTPDLLInstance, "ReqOrderInsert"))== NULL )
		return 18;
	if((pReqOrderAction=         (lpReqOrderAction          )::GetProcAddress(gUSTPDLLInstance, "ReqOrderAction"))== NULL )
		return 19;
	if((pReqQryOrder=            (lpReqQryOrder             )::GetProcAddress(gUSTPDLLInstance, "ReqQryOrder"))== NULL )
		return 20;
	if((pReqQryTrade=            (lpReqQryTrade             )::GetProcAddress(gUSTPDLLInstance, "ReqQryTrade"))== NULL )
		return 21;
	if((pReqQryUserInvestor=     (lpReqQryUserInvestor      )::GetProcAddress(gUSTPDLLInstance, "ReqQryUserInvestor"))== NULL )
		return 22;
	if((pReqQryTradingCode=      (lpReqQryTradingCode       )::GetProcAddress(gUSTPDLLInstance, "ReqQryTradingCode"))== NULL )
		return 23;
	if((pReqQryInvestorAccount=  (lpReqQryInvestorAccount   )::GetProcAddress(gUSTPDLLInstance, "ReqQryInvestorAccount"))== NULL )
		return 24;
	if((pReqQryInstrument=       (lpReqQryInstrument        )::GetProcAddress(gUSTPDLLInstance, "ReqQryInstrument"))== NULL )
		return 25;
	if((pReqQryExchange=         (lpReqQryExchange          )::GetProcAddress(gUSTPDLLInstance, "ReqQryExchange"))== NULL )
		return 26;
	if((pReqQryInvestorPosition= (lpReqQryInvestorPosition  )::GetProcAddress(gUSTPDLLInstance, "ReqQryInvestorPosition"))== NULL )
		return 27;
	if((pReqSubscribeTopic=      (lpReqSubscribeTopic       )::GetProcAddress(gUSTPDLLInstance, "ReqSubscribeTopic"))== NULL )
		return 28;
	if((pReqQryComplianceParam=  (lpReqQryComplianceParam   )::GetProcAddress(gUSTPDLLInstance, "ReqQryComplianceParam"))== NULL )
		return 29;
	if((pReqQryTopic=            (lpReqQryTopic             )::GetProcAddress(gUSTPDLLInstance, "ReqQryTopic"))== NULL )
		return 30;
	if((pReqQryInvestorFee=      (lpReqQryInvestorFee       )::GetProcAddress(gUSTPDLLInstance, "ReqQryInvestorFee"))== NULL )
		return 31;
	if((pReqQryInvestorMargin=   (lpReqQryInvestorMargin    )::GetProcAddress(gUSTPDLLInstance, "ReqQryInvestorMargin"))== NULL )
		return 32;
	return 0;
}
//---------------------------------------------------------------------------
USTPTraderApiInit::USTPTraderApiInit()
:FSupport( false )
,FEvent( )
{
	gUSTPDLLInstance = ::LoadLibraryW( L"FemasAPIFactory.dll" );
	if( gUSTPDLLInstance != NULL )
	{
		if( (FFuncRtn  = InitFunctions()) != 0 )
		{
			sprintf( FNotSupportReason, "Dynamic bind function %d failed.", FFuncRtn  );
			return;
		}
		if(	(FRegCBRtn = InitRegCallbackFunctions()) != 0 )
		{
			sprintf( FNotSupportReason, "Dynamic bind regist callback function %d failed.", FRegCBRtn );
			return;
		}
		sprintf( FNotSupportReason, "Load FemasAPIFactory.dll OK!" );
		FSupport = true; ///< Load CTP dll OK!
	}
	else
		sprintf( FNotSupportReason, "Can't dynamic load FemasAPIFactory.dll" );
}
//------------------------------------------------------------------------------
USTPTraderApiInit::~USTPTraderApiInit()
{

}
//---------------------------------------------------------------------------
bool TriggerFemasEvent( HUSTP Owner, int MSG, TCTPParams* Params )
{
	TUstpFtdcTraderApi* USTPObj = (TUstpFtdcTraderApi*)Owner;

	if( USTPObj != NULL )
	{
		TUstpFtdcTraderSpi* Listener = USTPObj->GetSpi( );
		if( Listener != NULL )
		{
			PostMessage( USTPObj->GetHWND(), MSG, (WPARAM)Listener,(LPARAM)Params );
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
void USTPTraderApiInit::SetEvent( void )
{
	FEvent.SetEvent();
}
//---------------------------------------------------------------------------
int  USTPTraderApiInit::WiitFor( int sec )
{
	return FEvent.WaitFor(sec);
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnFrontConnected( HUSTP Owner )
{
	TUstpFtdcTraderApi* USTPObj = (TUstpFtdcTraderApi*)Owner;

	if( USTPObj != NULL )
	{
		TUstpFtdcTraderSpi* Listener = USTPObj->GetSpi( );
		if( Listener != NULL )
		{
			Listener->OnFemasAPIConnected();
			PostMessage( USTPObj->GetHWND(), WM_FEMAS_EVENT, (WPARAM)Listener,(LPARAM)NULL );
		}
	}
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnFrontDisconnected( HUSTP Owner, int nReason)
{
	TUstpFtdcTraderApi* USTPObj = (TUstpFtdcTraderApi*)Owner;

	if( USTPObj != NULL )
	{
		TUstpFtdcTraderSpi* Listener = USTPObj->GetSpi( );
		if( Listener != NULL )
		{
			TCTPParams* Params = new TCTPParams( nReason );

			Listener->OnFemasAPIDisconnected( nReason );
			PostMessage( USTPObj->GetHWND(), WM_FEMAS_EVENT+1, (WPARAM)Listener,(LPARAM)Params );
		}
	}
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnHeartBeatWarning( HUSTP Owner, int nTimeLapse)
{
	TCTPParams* Params = new TCTPParams( nTimeLapse);

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+2, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnPackageStart( HUSTP Owner, int nTopicID, int nSequenceNo)
{
	TCTPParams* Params = new TCTPParams( nTopicID, nSequenceNo);

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+3, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnPackageEnd( HUSTP Owner, int nTopicID, int nSequenceNo)
{
	TCTPParams* Params = new TCTPParams( nTopicID, nSequenceNo );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+4, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspError( HUSTP Owner, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pRspInfo,sizeof(CUstpFtdcRspInfoField), nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+5, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspUserLogin( HUSTP Owner, CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pRspUserLogin,sizeof(CUstpFtdcRspUserLoginField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+6, Params );
	gUSTPAPI.WiitFor( 10 );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspUserLogout( HUSTP Owner, CUstpFtdcRspUserLogoutField *pRspUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pRspUserLogout,sizeof(CUstpFtdcRspUserLogoutField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+7, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspUserPasswordUpdate( HUSTP Owner, CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pUserPasswordUpdate,sizeof(CUstpFtdcUserPasswordUpdateField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+8, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspOrderInsert( HUSTP Owner, CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pInputOrder,sizeof(CUstpFtdcInputOrderField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+9, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspOrderAction( HUSTP Owner, CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pOrderAction,sizeof(CUstpFtdcOrderActionField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+10, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnRtnFlowMessageCancel( HUSTP Owner, CUstpFtdcFlowMessageCancelField *pFlowMessageCancel)
{
	TCTPParams* Params = new TCTPParams( (void*)pFlowMessageCancel,sizeof(CUstpFtdcFlowMessageCancelField) );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+11, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnRtnTrade( HUSTP Owner, CUstpFtdcTradeField *pTrade)
{
	TCTPParams* Params = new TCTPParams( (void*)pTrade,sizeof(CUstpFtdcTradeField) );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+12, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnRtnOrder( HUSTP Owner, CUstpFtdcOrderField *pOrder)
{
	TCTPParams* Params = new TCTPParams( (void*)pOrder,sizeof(CUstpFtdcOrderField) );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+13, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnErrRtnOrderInsert( HUSTP Owner, CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo)
{
	TCTPParams* Params = new TCTPParams( (void*)pInputOrder,sizeof(CUstpFtdcInputOrderField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField));

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+14, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnErrRtnOrderAction( HUSTP Owner, CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo)
{
	TCTPParams* Params = new TCTPParams( (void*)pOrderAction,sizeof(CUstpFtdcOrderActionField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField));

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+15, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnRtnInstrumentStatus( HUSTP Owner, CUstpFtdcInstrumentStatusField *pInstrumentStatus)
{
	TCTPParams* Params = new TCTPParams( (void*)pInstrumentStatus,sizeof(CUstpFtdcInstrumentStatusField) );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+16, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspQryOrder( HUSTP Owner, CUstpFtdcOrderField *pOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pOrder,sizeof(CUstpFtdcOrderField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+17, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspQryTrade( HUSTP Owner, CUstpFtdcTradeField *pTrade, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pTrade,sizeof(CUstpFtdcTradeField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+18, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspQryUserInvestor( HUSTP Owner, CUstpFtdcRspUserInvestorField *pRspUserInvestor, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pRspUserInvestor,sizeof(CUstpFtdcRspUserInvestorField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+19, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspQryTradingCode( HUSTP Owner, CUstpFtdcRspTradingCodeField *pRspTradingCode, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pRspTradingCode,sizeof(CUstpFtdcRspTradingCodeField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+20, Params );
}
//---------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspQryInvestorAccount( HUSTP Owner, CUstpFtdcRspInvestorAccountField *pRspInvestorAccount, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pRspInvestorAccount,sizeof(CUstpFtdcRspInvestorAccountField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+21, Params );
}
//------------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspQryInstrument( HUSTP Owner, CUstpFtdcRspInstrumentField *pRspInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pRspInstrument,sizeof(CUstpFtdcRspInstrumentField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+22, Params );
}
//------------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspQryExchange( HUSTP Owner, CUstpFtdcRspExchangeField *pRspExchange, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pRspExchange,sizeof(CUstpFtdcRspExchangeField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+23, Params );
}
//------------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspQryInvestorPosition( HUSTP Owner, CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pRspInvestorPosition,sizeof(CUstpFtdcRspInvestorPositionField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+24, Params );
}
//------------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspSubscribeTopic( HUSTP Owner, CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pDissemination,sizeof(CUstpFtdcDisseminationField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+25, Params );
}
//------------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspQryComplianceParam( HUSTP Owner, CUstpFtdcRspComplianceParamField *pRspComplianceParam, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pRspComplianceParam,sizeof(CUstpFtdcRspComplianceParamField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+26, Params );
}
//------------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspQryTopic( HUSTP Owner, CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pDissemination,sizeof(CUstpFtdcDisseminationField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+27, Params );
}
//------------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspQryInvestorFee( HUSTP Owner, CUstpFtdcInvestorFeeField *pInvestorFee, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorFee,sizeof(CUstpFtdcInvestorFeeField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+28, Params );
}
//------------------------------------------------------------------------------
void  USTPTraderApiInit::OnRspQryInvestorMargin( HUSTP Owner, CUstpFtdcInvestorMarginField *pInvestorMargin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorMargin,sizeof(CUstpFtdcInvestorMarginField),(void*)pRspInfo,sizeof(CUstpFtdcRspInfoField),nRequestID, bIsLast );

	TriggerFemasEvent( Owner, WM_FEMAS_EVENT+29, Params );
}
//------------------------------------------------------------------------------
int USTPTraderApiInit::InitRegCallbackFunctions( void )
{
	if((pRegOnFrontConnected=        (lpRegOnFrontConnected         )::GetProcAddress(gUSTPDLLInstance, "RegOnFrontConnected"))== NULL )
		return 1;
	if((pRegOnFrontDisconnected=     (lpRegOnFrontDisconnected      )::GetProcAddress(gUSTPDLLInstance, "RegOnFrontDisconnected"))== NULL )
		return 2;
	if((pRegOnHeartBeatWarning=      (lpRegOnHeartBeatWarning       )::GetProcAddress(gUSTPDLLInstance, "RegOnHeartBeatWarning"))== NULL )
		return 3;
	if((pRegOnPackageStart=          (lpRegOnPackageStart           )::GetProcAddress(gUSTPDLLInstance, "RegOnPackageStart"))== NULL )
		return 4;
	if((pRegOnPackageEnd=            (lpRegOnPackageEnd             )::GetProcAddress(gUSTPDLLInstance, "RegOnPackageEnd"))== NULL )
		return 5;
	if((pRegOnRspError=              (lpRegOnRspError               )::GetProcAddress(gUSTPDLLInstance, "RegOnRspError"))== NULL )
		return 6;
	if((pRegOnRspUserLogin=          (lpRegOnRspUserLogin           )::GetProcAddress(gUSTPDLLInstance, "RegOnRspUserLogin"))== NULL )
		return 7;
	if((pRegOnRspUserLogout=         (lpRegOnRspUserLogout          )::GetProcAddress(gUSTPDLLInstance, "RegOnRspUserLogout"))== NULL )
		return 8;
	if((pRegOnRspUserPasswordUpdate= (lpRegOnRspUserPasswordUpdate  )::GetProcAddress(gUSTPDLLInstance, "RegOnRspUserPasswordUpdate"))== NULL )
		return 9;
	if((pRegOnRspOrderInsert=        (lpRegOnRspOrderInsert         )::GetProcAddress(gUSTPDLLInstance, "RegOnRspOrderInsert"))== NULL )
		return 10;
	if((pRegOnRspOrderAction=        (lpRegOnRspOrderAction         )::GetProcAddress(gUSTPDLLInstance, "RegOnRspOrderAction"))== NULL )
		return 11;
	if((pRegOnRtnFlowMessageCancel=  (lpRegOnRtnFlowMessageCancel   )::GetProcAddress(gUSTPDLLInstance, "RegOnRtnFlowMessageCancel"))== NULL )
		return 12;
	if((pRegOnRtnTrade=              (lpRegOnRtnTrade               )::GetProcAddress(gUSTPDLLInstance, "RegOnRtnTrade"))== NULL )
		return 13;
	if((pRegOnRtnOrder=              (lpRegOnRtnOrder               )::GetProcAddress(gUSTPDLLInstance, "RegOnRtnOrder"))== NULL )
		return 14;
	if((pRegOnErrRtnOrderInsert=     (lpRegOnErrRtnOrderInsert      )::GetProcAddress(gUSTPDLLInstance, "RegOnErrRtnOrderInsert"))== NULL )
		return 15;
	if((pRegOnErrRtnOrderAction=     (lpRegOnErrRtnOrderAction      )::GetProcAddress(gUSTPDLLInstance, "RegOnErrRtnOrderAction"))== NULL )
		return 16;
	if((pRegOnRtnInstrumentStatus=   (lpRegOnRtnInstrumentStatus    )::GetProcAddress(gUSTPDLLInstance, "RegOnRtnInstrumentStatus"))== NULL )
		return 17;
	if((pRegOnRspQryOrder=           (lpRegOnRspQryOrder            )::GetProcAddress(gUSTPDLLInstance, "RegOnRspQryOrder"))== NULL )
		return 18;
	if((pRegOnRspQryTrade=           (lpRegOnRspQryTrade            )::GetProcAddress(gUSTPDLLInstance, "RegOnRspQryTrade"))== NULL )
		return 19;
	if((pRegOnRspQryUserInvestor=    (lpRegOnRspQryUserInvestor     )::GetProcAddress(gUSTPDLLInstance, "RegOnRspQryUserInvestor"))== NULL )
		return 20;
	if((pRegOnRspQryTradingCode=     (lpRegOnRspQryTradingCode      )::GetProcAddress(gUSTPDLLInstance, "RegOnRspQryTradingCode"))== NULL )
		return 21;
	if((pRegOnRspQryInvestorAccount= (lpRegOnRspQryInvestorAccount  )::GetProcAddress(gUSTPDLLInstance, "RegOnRspQryInvestorAccount"))== NULL )
		return 22;
	if((pRegOnRspQryInstrument=      (lpRegOnRspQryInstrument       )::GetProcAddress(gUSTPDLLInstance, "RegOnRspQryInstrument"))== NULL )
		return 23;
	if((pRegOnRspQryExchange=        (lpRegOnRspQryExchange         )::GetProcAddress(gUSTPDLLInstance, "RegOnRspQryExchange"))== NULL )
		return 24;
	if((pRegOnRspQryInvestorPosition=(lpRegOnRspQryInvestorPosition )::GetProcAddress(gUSTPDLLInstance, "RegOnRspQryInvestorPosition"))== NULL )
		return 25;
	if((pRegOnRspSubscribeTopic=     (lpRegOnRspSubscribeTopic      )::GetProcAddress(gUSTPDLLInstance, "RegOnRspSubscribeTopic"))== NULL )
		return 26;
	if((pRegOnRspQryComplianceParam= (lpRegOnRspQryComplianceParam  )::GetProcAddress(gUSTPDLLInstance, "RegOnRspQryComplianceParam"))== NULL )
		return 27;
	if((pRegOnRspQryTopic=           (lpRegOnRspQryTopic            )::GetProcAddress(gUSTPDLLInstance, "RegOnRspQryTopic"))== NULL )
		return 28;
	if((pRegOnRspQryInvestorFee=     (lpRegOnRspQryInvestorFee      )::GetProcAddress(gUSTPDLLInstance, "RegOnRspQryInvestorFee"))== NULL )
		return 29;
	if((pRegOnRspQryInvestorMargin=  (lpRegOnRspQryInvestorMargin   )::GetProcAddress(gUSTPDLLInstance, "RegOnRspQryInvestorMargin"))== NULL )
		return 30;
	return 0;
}
//------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
