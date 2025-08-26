//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "USTPBCBTraderAPI.h"
#include "FemasAPIDynamic.h"
#include "USTPBCBEvents.h"
#include "APIParams.h"
#include "../../../../UFC/UFC.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
USTPTraderApiInit gUSTPAPI;
HINSTANCE         gUSTPDLLInstance = NULL;
//------------------------------------------------------------------------------
//
// Class TUstpFtdcTraderApi
// Wrapper for Femas APV visual C++ version.
//
//------------------------------------------------------------------------------
TUstpFtdcTraderApi::TUstpFtdcTraderApi( TUstpFtdcTraderSpi *pSpi )
:FHUSTPAPI( NULL )
,FListener( pSpi )
{
	///< Check Femas API exists.
	if( TUstpFtdcTraderApi::SupportUSTPAPI() == false )
		throw Exception( " Not support Femas API.");
	///< Create Femas Trader API Instance.
	if( (FHUSTPAPI = gUSTPAPI.pCreateUSTPTraderApi( this )) == NULL )
		throw Exception( " Create CUstpFtdcTraderApi failed!");
	///< Register all call back functions.
	RegAllCallbackFuncs( );
	///< Create message window
	if( InitMessageWindow() == TRUE )
	{
		if( CreateMessageWindow()== FALSE )
		{
			UFC::BufferedLog::DebugPrintf( " Can't create the Femas message window." );
			throw UFC::Exception( " Can't create the Femas message window." );
		}
		else
			UFC::BufferedLog::DebugPrintf( " Create Femas message window:%p Thread:%d", (void*)FHwnd,GetCurrentThreadId() );
	}
	else
	{
		UFC::BufferedLog::DebugPrintf( " Register Femas window class failed" );
		throw UFC::Exception( " Register Femas window class failed." );
	}
}
//------------------------------------------------------------------------------
TUstpFtdcTraderApi::~TUstpFtdcTraderApi()
{
	RegisterSpi( NULL );
	FListener = NULL;
	DestroyWindow( FHwnd );
	UnregisterClassW( MessageWindowsClassName, FInstance );
	gUSTPAPI.pReleaseUSTPTraderApi( FHUSTPAPI );
}
//------------------------------------------------------------------------------
void TUstpFtdcTraderApi::RegAllCallbackFuncs( void )
{
	gUSTPAPI.pRegOnFrontConnected( FHUSTPAPI         ,USTPTraderApiInit::OnFrontConnected);
	gUSTPAPI.pRegOnFrontDisconnected( FHUSTPAPI      ,USTPTraderApiInit::OnFrontDisconnected);
	gUSTPAPI.pRegOnHeartBeatWarning( FHUSTPAPI       ,USTPTraderApiInit::OnHeartBeatWarning);
	gUSTPAPI.pRegOnPackageStart( FHUSTPAPI           ,USTPTraderApiInit::OnPackageStart);
	gUSTPAPI.pRegOnPackageEnd( FHUSTPAPI             ,USTPTraderApiInit::OnPackageEnd);
	gUSTPAPI.pRegOnRspError( FHUSTPAPI               ,USTPTraderApiInit::OnRspError);
	gUSTPAPI.pRegOnRspUserLogin( FHUSTPAPI           ,USTPTraderApiInit::OnRspUserLogin);
	gUSTPAPI.pRegOnRspUserLogout( FHUSTPAPI          ,USTPTraderApiInit::OnRspUserLogout);
	gUSTPAPI.pRegOnRspUserPasswordUpdate( FHUSTPAPI  ,USTPTraderApiInit::OnRspUserPasswordUpdate);
	gUSTPAPI.pRegOnRspOrderInsert( FHUSTPAPI         ,USTPTraderApiInit::OnRspOrderInsert);
	gUSTPAPI.pRegOnRspOrderAction( FHUSTPAPI         ,USTPTraderApiInit::OnRspOrderAction);
	gUSTPAPI.pRegOnRtnFlowMessageCancel( FHUSTPAPI   ,USTPTraderApiInit::OnRtnFlowMessageCancel);
	gUSTPAPI.pRegOnRtnTrade( FHUSTPAPI               ,USTPTraderApiInit::OnRtnTrade);
	gUSTPAPI.pRegOnRtnOrder( FHUSTPAPI               ,USTPTraderApiInit::OnRtnOrder);
	gUSTPAPI.pRegOnErrRtnOrderInsert( FHUSTPAPI      ,USTPTraderApiInit::OnErrRtnOrderInsert);
	gUSTPAPI.pRegOnErrRtnOrderAction( FHUSTPAPI      ,USTPTraderApiInit::OnErrRtnOrderAction);
	gUSTPAPI.pRegOnRtnInstrumentStatus( FHUSTPAPI    ,USTPTraderApiInit::OnRtnInstrumentStatus);
	gUSTPAPI.pRegOnRspQryOrder( FHUSTPAPI            ,USTPTraderApiInit::OnRspQryOrder);
	gUSTPAPI.pRegOnRspQryTrade( FHUSTPAPI            ,USTPTraderApiInit::OnRspQryTrade);
	gUSTPAPI.pRegOnRspQryUserInvestor( FHUSTPAPI     ,USTPTraderApiInit::OnRspQryUserInvestor);
	gUSTPAPI.pRegOnRspQryTradingCode( FHUSTPAPI      ,USTPTraderApiInit::OnRspQryTradingCode);
	gUSTPAPI.pRegOnRspQryInvestorAccount( FHUSTPAPI  ,USTPTraderApiInit::OnRspQryInvestorAccount);
	gUSTPAPI.pRegOnRspQryInstrument( FHUSTPAPI       ,USTPTraderApiInit::OnRspQryInstrument);
	gUSTPAPI.pRegOnRspQryExchange( FHUSTPAPI         ,USTPTraderApiInit::OnRspQryExchange);
	gUSTPAPI.pRegOnRspQryInvestorPosition( FHUSTPAPI ,USTPTraderApiInit::OnRspQryInvestorPosition);
	gUSTPAPI.pRegOnRspSubscribeTopic( FHUSTPAPI      ,USTPTraderApiInit::OnRspSubscribeTopic);
	gUSTPAPI.pRegOnRspQryComplianceParam( FHUSTPAPI  ,USTPTraderApiInit::OnRspQryComplianceParam);
	gUSTPAPI.pRegOnRspQryTopic( FHUSTPAPI            ,USTPTraderApiInit::OnRspQryTopic);
	gUSTPAPI.pRegOnRspQryInvestorFee( FHUSTPAPI      ,USTPTraderApiInit::OnRspQryInvestorFee);
	gUSTPAPI.pRegOnRspQryInvestorMargin( FHUSTPAPI   ,USTPTraderApiInit::OnRspQryInvestorMargin);
}
//---------------------------------------------------------------------------
LRESULT CALLBACK TUstpFtdcTraderApi::MessageWndProc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	TCTPParams*          Params   = (TCTPParams*)lParam;
	TUstpFtdcTraderSpi*  Listener = (TUstpFtdcTraderSpi*)wParam;
	switch( Msg )
	{

		case WM_FEMAS_EVENT:    Listener->OnFemasFrontConnected();break;
		case WM_FEMAS_EVENT+1:  Listener->OnFemasFrontDisconnected(Params->Param1.AsInt());
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+2:  Listener->OnFemasHeartBeatWarning(Params->Param1.AsInt());
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+3:  Listener->OnFemasPackageStart(Params->Param1.AsInt(), Params->Param2.AsInt());
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+4:  Listener->OnFemasPackageEnd(Params->Param1.AsInt(), Params->Param2.AsInt());
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+5:  Listener->OnFemasRspError((CUstpFtdcRspInfoField*)Params->Param1.AsPtr(), Params->Param2.AsInt(), Params->Param3.AsBool() ) ;
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+6:  Listener->OnFemasRspUserLogin((CUstpFtdcRspUserLoginField*)Params->Param1.AsPtr(), (CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool()) ;
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+7:  Listener->OnFemasRspUserLogout((CUstpFtdcRspUserLogoutField*)Params->Param1.AsPtr(),(CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+8:  Listener->OnFemasRspUserPasswordUpdate((CUstpFtdcUserPasswordUpdateField*)Params->Param1.AsPtr(), (CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+9:  Listener->OnFemasRspOrderInsert((CUstpFtdcInputOrderField*)Params->Param1.AsPtr(),(CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+10: Listener->OnFemasRspOrderAction((CUstpFtdcOrderActionField*)Params->Param1.AsPtr(),(CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+11: Listener->OnFemasRtnFlowMessageCancel((CUstpFtdcFlowMessageCancelField*)Params->Param1.AsPtr()) ;
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+12: Listener->OnFemasRtnTrade((CUstpFtdcTradeField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+13: Listener->OnFemasRtnOrder((CUstpFtdcOrderField*)Params->Param1.AsPtr()) ;
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+14: Listener->OnFemasErrRtnOrderInsert((CUstpFtdcInputOrderField*)Params->Param1.AsPtr(), (CUstpFtdcRspInfoField*)Params->Param2.AsPtr()) ;
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+15: Listener->OnFemasErrRtnOrderAction((CUstpFtdcOrderActionField*)Params->Param1.AsPtr(), (CUstpFtdcRspInfoField*)Params->Param2.AsPtr()) ;
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+16: Listener->OnFemasRtnInstrumentStatus((CUstpFtdcInstrumentStatusField*)Params->Param1.AsPtr() );
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+17: Listener->OnFemasRspQryOrder((CUstpFtdcOrderField *)Params->Param1.AsPtr(), (CUstpFtdcRspInfoField *)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() ) ;
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+18: Listener->OnFemasRspQryTrade((CUstpFtdcTradeField*)Params->Param1.AsPtr(), (CUstpFtdcRspInfoField *)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() ) ;
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+19: Listener->OnFemasRspQryUserInvestor((CUstpFtdcRspUserInvestorField*)Params->Param1.AsPtr(), (CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+20: Listener->OnFemasRspQryTradingCode((CUstpFtdcRspTradingCodeField*)Params->Param1.AsPtr(), (CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+21: Listener->OnFemasRspQryInvestorAccount((CUstpFtdcRspInvestorAccountField*)Params->Param1.AsPtr(),(CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+22: Listener->OnFemasRspQryInstrument((CUstpFtdcRspInstrumentField*)Params->Param1.AsPtr(),(CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+23: Listener->OnFemasRspQryExchange((CUstpFtdcRspExchangeField*)Params->Param1.AsPtr(), (CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+24: Listener->OnFemasRspQryInvestorPosition((CUstpFtdcRspInvestorPositionField*)Params->Param1.AsPtr(), (CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+25: Listener->OnFemasRspSubscribeTopic((CUstpFtdcDisseminationField*)Params->Param1.AsPtr(), (CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+26: Listener->OnFemasRspQryComplianceParam((CUstpFtdcRspComplianceParamField*)Params->Param1.AsPtr(),(CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+27: Listener->OnFemasRspQryTopic((CUstpFtdcDisseminationField*)Params->Param1.AsPtr(),(CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+28: Listener->OnFemasRspQryInvestorFee((CUstpFtdcInvestorFeeField*)Params->Param1.AsPtr(),(CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+29: Listener->OnFemasRspQryInvestorMargin((CUstpFtdcInvestorMarginField*)Params->Param1.AsPtr(),(CUstpFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_FEMAS_EVENT+30: Listener->OnFemasRecoverFinished( (int)lParam );
							  return 0;
		default:              return DefWindowProc( hwnd, Msg, wParam, lParam);
	}
	return DefWindowProc( hwnd, Msg, wParam, lParam);
}
//---------------------------------------------------------------------------
BOOL TUstpFtdcTraderApi::InitMessageWindow( void )
{
	GUID        Guid;
	WNDCLASSEXW wcx;

	if( CoCreateGuid( &Guid ) == S_OK )
		swprintf( MessageWindowsClassName, L"%0.8X-%0.4X-%0.4X-%0.2X%0.2X-%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X",
										 Guid.Data1, Guid.Data2, Guid.Data3,
										 Guid.Data4[0], Guid.Data4[1], Guid.Data4[2], Guid.Data4[3],
										 Guid.Data4[4], Guid.Data4[5], Guid.Data4[6], Guid.Data4[7]);
	else
		swprintf( MessageWindowsClassName, L"USTP_API_MsgWin_%10d", UFC::GetTickCountMS());
	// Fill in the window class structure with parameters that describe the main window.
	wcx.cbSize = sizeof( wcx );                 // size of structure
	wcx.style = CS_HREDRAW |CS_VREDRAW;         // redraw if size changes
	wcx.lpfnWndProc   = TUstpFtdcTraderApi::MessageWndProc;// points to window procedure
	wcx.cbClsExtra    = 0;                      // no extra class memory
	wcx.cbWndExtra    = 0;                      // no extra window memory
	wcx.hInstance     = FInstance;              // handle to instance
	wcx.hIcon         = NULL;                   // predefined app. icon
	wcx.hCursor       = NULL;                   // predefined arrow
	wcx.hbrBackground = NULL;                   // background brush
	wcx.lpszMenuName  = NULL;                   // name of menu resource
	wcx.lpszClassName = MessageWindowsClassName;// name of window class
	wcx.hIconSm = NULL;
	if( RegisterClassExW( &wcx ) == 0 )          // Register the window class.
		return FALSE;
	return TRUE;
}
//---------------------------------------------------------------------------
BOOL TUstpFtdcTraderApi::CreateMessageWindow( void ) // Create the message window.
{
	FHwnd = CreateWindowW(  MessageWindowsClassName,// name of window class
						   L"USTP_API_App",              // title-bar string
						   WS_OVERLAPPEDWINDOW ,// top-level window
						   10,                  // horizontal position
						   10,                  // vertical position
						   100,                 // width
						   100,                 // height
						   HWND_MESSAGE,        // Message only window
						   NULL,                // no class menu
						   NULL,                // handle to application instance
						   NULL );              // no window-creation data
	if( !FHwnd )
		return FALSE;
	return TRUE;
}
//---------------------------------------------------------------------------
bool TUstpFtdcTraderApi::SupportUSTPAPI( void )
{
	return gUSTPAPI.SupportUSTPAPI();
}
//---------------------------------------------------------------------------
const char* TUstpFtdcTraderApi::LoadUSTPAPIResult( void )
{
	return gUSTPAPI.Reason();
}
//------------------------------------------------------------------------------
void  TUstpFtdcTraderApi::RegisterSpi( TUstpFtdcTraderSpi *pSpi )
{
	FListener = pSpi;
}
//------------------------------------------------------------------------------
void TUstpFtdcTraderApi::Init()
{
	gUSTPAPI.pInit( FHUSTPAPI );
}
//---------------------------------------------------------------------------
const char* TUstpFtdcTraderApi::GetVersion(int &nMajorVersion, int &nMinorVersion)
{
	return gUSTPAPI.pGetVersion( FHUSTPAPI, nMajorVersion, nMinorVersion );
}
//---------------------------------------------------------------------------
const char* TUstpFtdcTraderApi::GetTradingDay()
{
	return gUSTPAPI.pGetTradingDay( FHUSTPAPI );
}
//---------------------------------------------------------------------------
void TUstpFtdcTraderApi::RegisterFront(char *pszFrontAddress)
{
	 gUSTPAPI.pRegisterFront( FHUSTPAPI, pszFrontAddress );
}
//---------------------------------------------------------------------------
void TUstpFtdcTraderApi::RegisterNameServer(char *pszNsAddress)
{
	 gUSTPAPI.pRegisterNameServer( FHUSTPAPI, pszNsAddress );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::RegisterCertificateFile(const char *pszCertFileName, const char *pszKeyFileName,const char *pszCaFileName, const char *pszKeyFilePassword)
{
	return gUSTPAPI.pRegisterCertificateFile( FHUSTPAPI, pszCertFileName, pszKeyFileName, pszCaFileName, pszKeyFilePassword );
}
//---------------------------------------------------------------------------
void TUstpFtdcTraderApi::SubscribePrivateTopic(USTP_TE_RESUME_TYPE nResumeType)
{
	 gUSTPAPI.pSubscribePrivateTopic( FHUSTPAPI, nResumeType );
}
//---------------------------------------------------------------------------
void TUstpFtdcTraderApi::SubscribePublicTopic(USTP_TE_RESUME_TYPE nResumeType)
{
	 gUSTPAPI.pSubscribePublicTopic( FHUSTPAPI, nResumeType );
}
//---------------------------------------------------------------------------
void TUstpFtdcTraderApi::SubscribeUserTopic(USTP_TE_RESUME_TYPE nResumeType)
{
	 gUSTPAPI.pSubscribeUserTopic( FHUSTPAPI, nResumeType );
}
//---------------------------------------------------------------------------
void TUstpFtdcTraderApi::SetHeartbeatTimeout(unsigned int timeout)
{
	 gUSTPAPI.pSetHeartbeatTimeout( FHUSTPAPI, timeout );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::OpenRequestLog(const char *pszReqLogFileName)
{
	return gUSTPAPI.pOpenRequestLog( FHUSTPAPI, pszReqLogFileName );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::OpenResponseLog(const char *pszRspLogFileName)
{
	return gUSTPAPI.pOpenResponseLog( FHUSTPAPI, pszRspLogFileName );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqUserLogin(CUstpFtdcReqUserLoginField *pReqUserLogin, int nRequestID)
{
	return gUSTPAPI.pReqUserLogin( FHUSTPAPI, pReqUserLogin, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqUserLogout(CUstpFtdcReqUserLogoutField *pReqUserLogout, int nRequestID)
{
	return gUSTPAPI.pReqUserLogout( FHUSTPAPI, pReqUserLogout, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqUserPasswordUpdate(CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID)
{
	return gUSTPAPI.pReqUserPasswordUpdate( FHUSTPAPI, pUserPasswordUpdate, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqOrderInsert(CUstpFtdcInputOrderField *pInputOrder, int nRequestID)
{
	return gUSTPAPI.pReqOrderInsert( FHUSTPAPI, pInputOrder, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqOrderAction(CUstpFtdcOrderActionField *pOrderAction, int nRequestID)
{
	return gUSTPAPI.pReqOrderAction( FHUSTPAPI, pOrderAction, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqQryOrder(CUstpFtdcQryOrderField *pQryOrder, int nRequestID)
{
	return gUSTPAPI.pReqQryOrder( FHUSTPAPI, pQryOrder, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqQryTrade(CUstpFtdcQryTradeField *pQryTrade, int nRequestID)
{
	return gUSTPAPI.pReqQryTrade( FHUSTPAPI, pQryTrade, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqQryUserInvestor(CUstpFtdcQryUserInvestorField *pQryUserInvestor, int nRequestID)
{
	return gUSTPAPI.pReqQryUserInvestor( FHUSTPAPI, pQryUserInvestor, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqQryTradingCode(CUstpFtdcQryTradingCodeField *pQryTradingCode, int nRequestID)
{
	return gUSTPAPI.pReqQryTradingCode( FHUSTPAPI, pQryTradingCode, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqQryInvestorAccount(CUstpFtdcQryInvestorAccountField *pQryInvestorAccount, int nRequestID)
{
	return gUSTPAPI.pReqQryInvestorAccount( FHUSTPAPI, pQryInvestorAccount, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqQryInstrument(CUstpFtdcQryInstrumentField *pQryInstrument, int nRequestID)
{
	return gUSTPAPI.pReqQryInstrument( FHUSTPAPI, pQryInstrument, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqQryExchange(CUstpFtdcQryExchangeField *pQryExchange, int nRequestID)
{
	return gUSTPAPI.pReqQryExchange( FHUSTPAPI, pQryExchange, nRequestID);
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqQryInvestorPosition(CUstpFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID)
{
	return gUSTPAPI.pReqQryInvestorPosition( FHUSTPAPI, pQryInvestorPosition, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqSubscribeTopic(CUstpFtdcDisseminationField *pDissemination, int nRequestID)
{
	return gUSTPAPI.pReqSubscribeTopic( FHUSTPAPI, pDissemination, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqQryComplianceParam(CUstpFtdcQryComplianceParamField *pQryComplianceParam, int nRequestID)
{
	return gUSTPAPI.pReqQryComplianceParam( FHUSTPAPI, pQryComplianceParam, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqQryTopic(CUstpFtdcDisseminationField *pDissemination, int nRequestID)
{
	return gUSTPAPI.pReqQryTopic( FHUSTPAPI, pDissemination, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqQryInvestorFee(CUstpFtdcQryInvestorFeeField *pQryInvestorFee, int nRequestID)
{
	return gUSTPAPI.pReqQryInvestorFee( FHUSTPAPI , pQryInvestorFee, nRequestID );
}
//---------------------------------------------------------------------------
int  TUstpFtdcTraderApi::ReqQryInvestorMargin(CUstpFtdcQryInvestorMarginField *pQryInvestorMargin, int nRequestID)
{
	return gUSTPAPI.pReqQryInvestorMargin( FHUSTPAPI, pQryInvestorMargin, nRequestID );
}
//------------------------------------------------------------------------------
void  TUstpFtdcTraderApi::ReqOnRecoverFilisned( int Count )
{
	TUstpFtdcTraderSpi*  Spi  = GetSpi( );
	HWND                 Hwnd = GetHWND( );

	PostMessage( Hwnd, WM_FEMAS_EVENT+30, (WPARAM)Spi,(LPARAM)Count );
}
//------------------------------------------------------------------------------
void TUstpFtdcTraderApi::SetEvent( void )
{
	gUSTPAPI.SetEvent();
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
