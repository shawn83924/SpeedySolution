//---------------------------------------------------------------------------
#ifdef _CTP
//------------------------------------------------------------------------------------------------------------------------
#ifdef _KSFT
//------------------------------------------------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include "KSftBCBTraderAPI.h"
#include "KSftAPIDynamic.h"
#include "KSftBCBEvents.h"
#include "APIParams.h"
#include "UFC.h"
//------------------------------------------------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------------------------------------------------
KSftTraderApiInit	gKSFTAPI;
HINSTANCE			gKSftDLLInstance = NULL;
//------------------------------------------------------------------------------------------------------------------------
//
// Class TKSFTTraderApi
// Wrapper for KingStar KSFT API visual C++ version.
//
//------------------------------------------------------------------------------------------------------------------------
TKSftTraderApi::TKSftTraderApi( TKSftTraderSpi* pSpi )
: FKSFTAPI( NULL )
, FListener( pSpi )
{
	// 1. Check KSFT API exist
	//    < KSftAPIFactory.dll >
	//    < KSftAPIFactory.dll  KSTraderAPI.dll  ksPortalAPI.dll  lkcdll.dll  SSPXEncode.dll >
	if ( TKSftTraderApi::SupportKSftAPI() == false )
		throw UFC::Exception( " Not support KSFT API." );
	// 2. Create KSFT Trader  API Instance.
	if ( ( FKSFTAPI = gKSFTAPI.pCreateKSftTraderApi( this ) ) == NULL )
		throw UFC::Exception( " Create CKSFTTraderApi failed!" );
	// 3. Register all call back functions.
	RegAllCallbackFuncs( );
	// 4. Create message window
	if ( InitMessageWindow() == FALSE )
	{
		UFC::BufferedLog::DebugPrintf( " Register KSFT window class failed." );
		throw UFC::Exception( " Register KSFT window class failed." );
	}
	if ( CreateMessageWindow() == FALSE )
	{
		UFC::BufferedLog::DebugPrintf( " Can't create the KSFT message window." );
		throw UFC::Exception( " Can't create the KSFT message window." );
	}
	UFC::BufferedLog::DebugPrintf( " Create KSFT message window:%p Thread:%d", (void*)FHwnd, GetCurrentThreadId() );
	// 5. Check KingStar license key file
	UFC::BufferedLog::Printf( " Create KKFT message window:%p Thread:%d", (void*)FHwnd, GetCurrentThreadId() );
	if ( UFC::FileExists( "KSInterB2C.lkc" ) == FALSE )
	{
		UFC::BufferedLog::DebugPrintf( "KingStar license key file not exist." );
		throw UFC::Exception( "KingStar license key file not exist." );
	}
}
//------------------------------------------------------------------------------------------------------------------------
TKSftTraderApi::~TKSftTraderApi()
{
	RegisterSpi( NULL );
	FListener = NULL;
	DestroyWindow( FHwnd );
	UnregisterClassW( MessageWindowsClassName, FInstance );
	gKSFTAPI.pReleaseKSftTraderApi( FKSFTAPI );
}
//------------------------------------------------------------------------------------------------------------------------
BOOL TKSftTraderApi::InitMessageWindow( void )
{
	GUID        Guid;
	WNDCLASSEXW wcx;
	if ( CoCreateGuid( &Guid ) == S_OK )
	{
		swprintf( MessageWindowsClassName, L"%0.8X-%0.4X-%0.4X-%0.2X%0.2X-%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X"
										, Guid.Data1, Guid.Data2, Guid.Data3
										, Guid.Data4[0], Guid.Data4[1], Guid.Data4[2], Guid.Data4[3]
										, Guid.Data4[4], Guid.Data4[5], Guid.Data4[6], Guid.Data4[7] );
	}
	else
		swprintf( MessageWindowsClassName, L"KSFT_API_MsgWin_%10d", UFC::GetTickCountMS() );
	// Fill in the window class structure with parameters that describe the main window.
	wcx.cbSize = sizeof( wcx );                 // size of structure
	wcx.style = CS_HREDRAW |CS_VREDRAW;         // redraw if size changes
	wcx.lpfnWndProc   = TKSftTraderApi::MessageWndProc;// points to window procedure
	wcx.cbClsExtra    = 0;                      // no extra class memory
	wcx.cbWndExtra    = 0;                      // no extra window memory
	wcx.hInstance     = FInstance;              // handle to instance
	wcx.hIcon         = NULL;                   // predefined app. icon
	wcx.hCursor       = NULL;                   // predefined arrow
	wcx.hbrBackground = NULL;                   // background brush
	wcx.lpszMenuName  = NULL;                   // name of menu resource
	wcx.lpszClassName = MessageWindowsClassName;// name of window class
	wcx.hIconSm = NULL;
	if ( RegisterClassExW( &wcx ) == 0 )			// Register the window class.
		return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL TKSftTraderApi::CreateMessageWindow( void )
{
	FHwnd = CreateWindowW( MessageWindowsClassName,	// name of window class
						   L"KSFT_API_App",			// title-bar string
						   WS_OVERLAPPEDWINDOW ,	// top-level window
						   10,						// horizontal position
						   10,						// vertical position
						   100,						// width
						   100,						// height
						   HWND_MESSAGE,			// Message only window
						   NULL,					// no class menu
						   NULL,					// handle to application instance
						   NULL );					// no window-creation data
	if ( !FHwnd )
		return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void TKSftTraderApi::RegAllCallbackFuncs( void )
{
	gKSFTAPI.pRegOnFrontConnected( FKSFTAPI,                          KSftTraderApiInit::OnFrontConnected );
	gKSFTAPI.pRegOnFrontDisconnected( FKSFTAPI,                       KSftTraderApiInit::OnFrontDisconnected );
	gKSFTAPI.pRegOnHeartBeatWarning( FKSFTAPI,                        KSftTraderApiInit::OnHeartBeatWarning );
	gKSFTAPI.pRegOnRspAuthenticate( FKSFTAPI,                         KSftTraderApiInit::OnRspAuthenticate );
	gKSFTAPI.pRegOnRspUserLogin( FKSFTAPI,                            KSftTraderApiInit::OnRspUserLogin );
	gKSFTAPI.pRegOnRspUserLogout( FKSFTAPI,                           KSftTraderApiInit::OnRspUserLogout );
	gKSFTAPI.pRegOnRspUserPasswordUpdate( FKSFTAPI,                   KSftTraderApiInit::OnRspUserPasswordUpdate );
	gKSFTAPI.pRegOnRspTradingAccountPasswordUpdate( FKSFTAPI,         KSftTraderApiInit::OnRspTradingAccountPasswordUpdate );
	gKSFTAPI.pRegOnRspOrderInsert( FKSFTAPI,                          KSftTraderApiInit::OnRspOrderInsert );
	gKSFTAPI.pRegOnRspParkedOrderInsert( FKSFTAPI,                    KSftTraderApiInit::OnRspParkedOrderInsert );
	gKSFTAPI.pRegOnRspParkedOrderAction( FKSFTAPI,                    KSftTraderApiInit::OnRspParkedOrderAction );
	gKSFTAPI.pRegOnRspOrderAction( FKSFTAPI,                          KSftTraderApiInit::OnRspOrderAction );
	gKSFTAPI.pRegOnRspQueryMaxOrderVolume( FKSFTAPI,                  KSftTraderApiInit::OnRspQueryMaxOrderVolume );
	gKSFTAPI.pRegOnRspSettlementInfoConfirm( FKSFTAPI,                KSftTraderApiInit::OnRspSettlementInfoConfirm );
	gKSFTAPI.pRegOnRspRemoveParkedOrder( FKSFTAPI,                    KSftTraderApiInit::OnRspRemoveParkedOrder );
	gKSFTAPI.pRegOnRspRemoveParkedOrderAction( FKSFTAPI,              KSftTraderApiInit::OnRspRemoveParkedOrderAction );
	gKSFTAPI.pRegOnRspExecOrderInsert( FKSFTAPI,                      KSftTraderApiInit::OnRspExecOrderInsert );
	gKSFTAPI.pRegOnRspExecOrderAction( FKSFTAPI,                      KSftTraderApiInit::OnRspExecOrderAction );
	gKSFTAPI.pRegOnRspForQuoteInsert( FKSFTAPI,                       KSftTraderApiInit::OnRspForQuoteInsert );
	gKSFTAPI.pRegOnRspQuoteInsert( FKSFTAPI,                          KSftTraderApiInit::OnRspQuoteInsert );
	gKSFTAPI.pRegOnRspQuoteAction( FKSFTAPI,                          KSftTraderApiInit::OnRspQuoteAction );
	gKSFTAPI.pRegOnRspQryOrder( FKSFTAPI,                             KSftTraderApiInit::OnRspQryOrder );
	gKSFTAPI.pRegOnRspQryTrade( FKSFTAPI,                             KSftTraderApiInit::OnRspQryTrade );
	gKSFTAPI.pRegOnRspQryInvestorPosition( FKSFTAPI,                  KSftTraderApiInit::OnRspQryInvestorPosition );
	gKSFTAPI.pRegOnRspQryTradingAccount( FKSFTAPI,                    KSftTraderApiInit::OnRspQryTradingAccount );
	gKSFTAPI.pRegOnRspQryInvestor( FKSFTAPI,                          KSftTraderApiInit::OnRspQryInvestor );
	gKSFTAPI.pRegOnRspQryTradingCode( FKSFTAPI,                       KSftTraderApiInit::OnRspQryTradingCode );
	gKSFTAPI.pRegOnRspQryInstrumentMarginRate( FKSFTAPI,              KSftTraderApiInit::OnRspQryInstrumentMarginRate );
	gKSFTAPI.pRegOnRspQryInstrumentCommissionRate( FKSFTAPI,          KSftTraderApiInit::OnRspQryInstrumentCommissionRate );
	gKSFTAPI.pRegOnRspQryExchange( FKSFTAPI,                          KSftTraderApiInit::OnRspQryExchange );
	gKSFTAPI.pRegOnRspQryProduct( FKSFTAPI,                           KSftTraderApiInit::OnRspQryProduct );
	gKSFTAPI.pRegOnRspQryInstrument( FKSFTAPI,                        KSftTraderApiInit::OnRspQryInstrument );
	gKSFTAPI.pRegOnRspQryDepthMarketData( FKSFTAPI,                   KSftTraderApiInit::OnRspQryDepthMarketData );
	gKSFTAPI.pRegOnRspQrySettlementInfo( FKSFTAPI,                    KSftTraderApiInit::OnRspQrySettlementInfo );
	gKSFTAPI.pRegOnRspQryTransferBank( FKSFTAPI,                      KSftTraderApiInit::OnRspQryTransferBank );
	gKSFTAPI.pRegOnRspQryInvestorPositionDetail( FKSFTAPI,            KSftTraderApiInit::OnRspQryInvestorPositionDetail );
	gKSFTAPI.pRegOnRspQryNotice( FKSFTAPI,                            KSftTraderApiInit::OnRspQryNotice );
	gKSFTAPI.pRegOnRspQrySettlementInfoConfirm( FKSFTAPI,             KSftTraderApiInit::OnRspQrySettlementInfoConfirm );
	gKSFTAPI.pRegOnRspQryInvestorPositionCombineDetail( FKSFTAPI,     KSftTraderApiInit::OnRspQryInvestorPositionCombineDetail );
	gKSFTAPI.pRegOnRspQryCFMMCTradingAccountKey( FKSFTAPI,            KSftTraderApiInit::OnRspQryCFMMCTradingAccountKey );
	gKSFTAPI.pRegOnRspQryEWarrantOffset( FKSFTAPI,                    KSftTraderApiInit::OnRspQryEWarrantOffset );
	gKSFTAPI.pRegOnRspQryInvestorProductGroupMargin( FKSFTAPI,        KSftTraderApiInit::OnRspQryInvestorProductGroupMargin );
	gKSFTAPI.pRegOnRspQryExchangeMarginRate( FKSFTAPI,                KSftTraderApiInit::OnRspQryExchangeMarginRate );
	gKSFTAPI.pRegOnRspQryExchangeMarginRateAdjust( FKSFTAPI,          KSftTraderApiInit::OnRspQryExchangeMarginRateAdjust );
	gKSFTAPI.pRegOnRspQryExchangeRate( FKSFTAPI,                      KSftTraderApiInit::OnRspQryExchangeRate );
	gKSFTAPI.pRegOnRspQrySecAgentACIDMap( FKSFTAPI,                   KSftTraderApiInit::OnRspQrySecAgentACIDMap );
	gKSFTAPI.pRegOnRspQryOptionInstrTradeCost( FKSFTAPI,              KSftTraderApiInit::OnRspQryOptionInstrTradeCost );
	gKSFTAPI.pRegOnRspQryOptionInstrCommRate( FKSFTAPI,               KSftTraderApiInit::OnRspQryOptionInstrCommRate );
	gKSFTAPI.pRegOnRspQryExecOrder( FKSFTAPI,                         KSftTraderApiInit::OnRspQryExecOrder );
	gKSFTAPI.pRegOnRspQryForQuote( FKSFTAPI,                          KSftTraderApiInit::OnRspQryForQuote );
	gKSFTAPI.pRegOnRspQryQuote( FKSFTAPI,                             KSftTraderApiInit::OnRspQryQuote );
	gKSFTAPI.pRegOnRspQryTransferSerial( FKSFTAPI,                    KSftTraderApiInit::OnRspQryTransferSerial );
	gKSFTAPI.pRegOnRspQryAccountregister( FKSFTAPI,                   KSftTraderApiInit::OnRspQryAccountregister );
	gKSFTAPI.pRegOnRspError( FKSFTAPI,                                KSftTraderApiInit::OnRspError );
	gKSFTAPI.pRegOnRtnOrder( FKSFTAPI,                                KSftTraderApiInit::OnRtnOrder );
	gKSFTAPI.pRegOnRtnTrade( FKSFTAPI,                                KSftTraderApiInit::OnRtnTrade );
	gKSFTAPI.pRegOnErrRtnOrderInsert( FKSFTAPI,                       KSftTraderApiInit::OnErrRtnOrderInsert );
	gKSFTAPI.pRegOnErrRtnOrderAction( FKSFTAPI,                       KSftTraderApiInit::OnErrRtnOrderAction );
	gKSFTAPI.pRegOnRtnInstrumentStatus( FKSFTAPI,                     KSftTraderApiInit::OnRtnInstrumentStatus );
	gKSFTAPI.pRegOnRtnTradingNotice( FKSFTAPI,                        KSftTraderApiInit::OnRtnTradingNotice );
	gKSFTAPI.pRegOnRtnErrorConditionalOrder( FKSFTAPI,                KSftTraderApiInit::OnRtnErrorConditionalOrder );
	gKSFTAPI.pRegOnRtnExecOrder( FKSFTAPI,                            KSftTraderApiInit::OnRtnExecOrder );
	gKSFTAPI.pRegOnErrRtnExecOrderInsert( FKSFTAPI,                   KSftTraderApiInit::OnErrRtnExecOrderInsert );
	gKSFTAPI.pRegOnErrRtnExecOrderAction( FKSFTAPI,                   KSftTraderApiInit::OnErrRtnExecOrderAction );
	gKSFTAPI.pRegOnErrRtnForQuoteInsert( FKSFTAPI,                    KSftTraderApiInit::OnErrRtnForQuoteInsert );
	gKSFTAPI.pRegOnRtnQuote( FKSFTAPI,                                KSftTraderApiInit::OnRtnQuote );
	gKSFTAPI.pRegOnErrRtnQuoteInsert( FKSFTAPI,                       KSftTraderApiInit::OnErrRtnQuoteInsert );
	gKSFTAPI.pRegOnErrRtnQuoteAction( FKSFTAPI,                       KSftTraderApiInit::OnErrRtnQuoteAction );
	gKSFTAPI.pRegOnRspQryContractBank( FKSFTAPI,                      KSftTraderApiInit::OnRspQryContractBank );
	gKSFTAPI.pRegOnRspQryParkedOrder( FKSFTAPI,                       KSftTraderApiInit::OnRspQryParkedOrder );
	gKSFTAPI.pRegOnRspQryParkedOrderAction( FKSFTAPI,                 KSftTraderApiInit::OnRspQryParkedOrderAction );
	gKSFTAPI.pRegOnRspQryTradingNotice( FKSFTAPI,                     KSftTraderApiInit::OnRspQryTradingNotice );
	gKSFTAPI.pRegOnRspQryBrokerTradingParams( FKSFTAPI,               KSftTraderApiInit::OnRspQryBrokerTradingParams );
	gKSFTAPI.pRegOnRspQryBrokerTradingAlgos( FKSFTAPI,                KSftTraderApiInit::OnRspQryBrokerTradingAlgos );
	gKSFTAPI.pRegOnRtnFromBankToFutureByBank( FKSFTAPI,               KSftTraderApiInit::OnRtnFromBankToFutureByBank );
	gKSFTAPI.pRegOnRtnFromFutureToBankByBank( FKSFTAPI,               KSftTraderApiInit::OnRtnFromFutureToBankByBank );
	gKSFTAPI.pRegOnRtnRepealFromBankToFutureByBank( FKSFTAPI,         KSftTraderApiInit::OnRtnRepealFromBankToFutureByBank );
	gKSFTAPI.pRegOnRtnRepealFromFutureToBankByBank( FKSFTAPI,         KSftTraderApiInit::OnRtnRepealFromFutureToBankByBank );
	gKSFTAPI.pRegOnRtnFromBankToFutureByFuture( FKSFTAPI,             KSftTraderApiInit::OnRtnFromBankToFutureByFuture );
	gKSFTAPI.pRegOnRtnFromFutureToBankByFuture( FKSFTAPI,             KSftTraderApiInit::OnRtnFromFutureToBankByFuture );
	gKSFTAPI.pRegOnRtnRepealFromBankToFutureByFutureManual( FKSFTAPI, KSftTraderApiInit::OnRtnRepealFromBankToFutureByFutureManual );
	gKSFTAPI.pRegOnRtnRepealFromFutureToBankByFutureManual( FKSFTAPI, KSftTraderApiInit::OnRtnRepealFromFutureToBankByFutureManual );
	gKSFTAPI.pRegOnRtnQueryBankBalanceByFuture( FKSFTAPI,             KSftTraderApiInit::OnRtnQueryBankBalanceByFuture );
	gKSFTAPI.pRegOnErrRtnBankToFutureByFuture( FKSFTAPI,              KSftTraderApiInit::OnErrRtnBankToFutureByFuture );
	gKSFTAPI.pRegOnErrRtnFutureToBankByFuture( FKSFTAPI,              KSftTraderApiInit::OnErrRtnFutureToBankByFuture );
	gKSFTAPI.pRegOnErrRtnRepealBankToFutureByFutureManual( FKSFTAPI,  KSftTraderApiInit::OnErrRtnRepealBankToFutureByFutureManual );
	gKSFTAPI.pRegOnErrRtnRepealFutureToBankByFutureManual( FKSFTAPI,  KSftTraderApiInit::OnErrRtnRepealFutureToBankByFutureManual );
	gKSFTAPI.pRegOnErrRtnQueryBankBalanceByFuture( FKSFTAPI,          KSftTraderApiInit::OnErrRtnQueryBankBalanceByFuture );
	gKSFTAPI.pRegOnRtnRepealFromBankToFutureByFuture( FKSFTAPI,       KSftTraderApiInit::OnRtnRepealFromBankToFutureByFuture );
	gKSFTAPI.pRegOnRtnRepealFromFutureToBankByFuture( FKSFTAPI,       KSftTraderApiInit::OnRtnRepealFromFutureToBankByFuture );
	gKSFTAPI.pRegOnRspFromBankToFutureByFuture( FKSFTAPI,             KSftTraderApiInit::OnRspFromBankToFutureByFuture );
	gKSFTAPI.pRegOnRspFromFutureToBankByFuture( FKSFTAPI,             KSftTraderApiInit::OnRspFromFutureToBankByFuture );
	gKSFTAPI.pRegOnRspQueryBankAccountMoneyByFuture( FKSFTAPI,        KSftTraderApiInit::OnRspQueryBankAccountMoneyByFuture );
	gKSFTAPI.pRegOnRtnOpenAccountByBank( FKSFTAPI,                    KSftTraderApiInit::OnRtnOpenAccountByBank );
	gKSFTAPI.pRegOnRtnCancelAccountByBank( FKSFTAPI,                  KSftTraderApiInit::OnRtnCancelAccountByBank );
	gKSFTAPI.pRegOnRtnChangeAccountByBank( FKSFTAPI,                  KSftTraderApiInit::OnRtnChangeAccountByBank );
	gKSFTAPI.pRegOnRspQryInvestorOpenPosition( FKSFTAPI,              KSftTraderApiInit::OnRspQryInvestorOpenPosition );
	gKSFTAPI.pRegOnRspQryInvestorOpenCombinePosition( FKSFTAPI,       KSftTraderApiInit::OnRspQryInvestorOpenCombinePosition );
	gKSFTAPI.pRegOnRspBulkCancelOrder( FKSFTAPI,                      KSftTraderApiInit::OnRspBulkCancelOrder );
}
//------------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK TKSftTraderApi::MessageWndProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	TCTPParams*			Params = (TCTPParams*)lParam;
	TKSftTraderSpi*		Listener = (TKSftTraderSpi*)wParam;
	switch ( Message )
	{
	case WM_KSFT_EVENT    : Listener->OnKSftFrontConnected( ); break;
	case WM_KSFT_EVENT+1  : Listener->OnKSftFrontDisconnected( Params->Param1.AsInt() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+2  : Listener->OnKSftHeartBeatWarning( Params->Param1.AsInt() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+3  : Listener->OnKSftRspAuthenticate( (KS_::CThostFtdcRspAuthenticateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+4  : Listener->OnKSftRspUserLogin( (KS_::CThostFtdcRspUserLoginField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+5  : Listener->OnKSftRspUserLogout( (KS_::CThostFtdcUserLogoutField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+6  : Listener->OnKSftRspUserPasswordUpdate( (KS_::CThostFtdcUserPasswordUpdateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+7  : Listener->OnKSftRspTradingAccountPasswordUpdate( (KS_::CThostFtdcTradingAccountPasswordUpdateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+8  : Listener->OnKSftRspOrderInsert( (KS_::CThostFtdcInputOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+9  : Listener->OnKSftRspParkedOrderInsert( (KS_::CThostFtdcParkedOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+10 : Listener->OnKSftRspParkedOrderAction( (KS_::CThostFtdcParkedOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+11 : Listener->OnKSftRspOrderAction( (KS_::CThostFtdcInputOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+12 : Listener->OnKSftRspQueryMaxOrderVolume( (KS_::CThostFtdcQueryMaxOrderVolumeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+13 : Listener->OnKSftRspSettlementInfoConfirm( (KS_::CThostFtdcSettlementInfoConfirmField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+14 : Listener->OnKSftRspRemoveParkedOrder( (KS_::CThostFtdcRemoveParkedOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+15 : Listener->OnKSftRspRemoveParkedOrderAction( (KS_::CThostFtdcRemoveParkedOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+16 : Listener->OnKSftRspExecOrderInsert( (KS_::CThostFtdcInputExecOrderField*) Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+17 : Listener->OnKSftRspExecOrderAction( (KS_::CThostFtdcInputExecOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+18 : Listener->OnKSftRspForQuoteInsert( (KS_::CThostFtdcInputForQuoteField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+19 : Listener->OnKSftRspQuoteInsert( (KS_::CThostFtdcInputQuoteField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+20 : Listener->OnKSftRspQuoteAction( (KS_::CThostFtdcInputQuoteActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+21 : Listener->OnKSftRspQryOrder( (KS_::CThostFtdcOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+22 : Listener->OnKSftRspQryTrade( (KS_::CThostFtdcTradeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+23 : Listener->OnKSftRspQryInvestorPosition( (KS_::CThostFtdcInvestorPositionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+24 : Listener->OnKSftRspQryTradingAccount( (KS_::CThostFtdcTradingAccountField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+25 : Listener->OnKSftRspQryInvestor( (KS_::CThostFtdcInvestorField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+26 : Listener->OnKSftRspQryTradingCode( (KS_::CThostFtdcTradingCodeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+27 : Listener->OnKSftRspQryInstrumentMarginRate( (KS_::CThostFtdcInstrumentMarginRateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+28 : Listener->OnKSftRspQryInstrumentCommissionRate( (KS_::CThostFtdcInstrumentCommissionRateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+29 : Listener->OnKSftRspQryExchange( (KS_::CThostFtdcExchangeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+30 : Listener->OnKSftRspQryProduct( (KS_::CThostFtdcProductField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+31 : Listener->OnKSftRspQryInstrument( (KS_::CThostFtdcInstrumentField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+32 : Listener->OnKSftRspQryDepthMarketData( (KS_::CThostFtdcDepthMarketDataField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+33 : Listener->OnKSftRspQrySettlementInfo( (KS_::CThostFtdcSettlementInfoField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+34 : Listener->OnKSftRspQryTransferBank( (KS_::CThostFtdcTransferBankField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+35 : Listener->OnKSftRspQryInvestorPositionDetail( (KS_::CThostFtdcInvestorPositionDetailField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+36 : Listener->OnKSftRspQryNotice( (KS_::CThostFtdcNoticeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+37 : Listener->OnKSftRspQrySettlementInfoConfirm( (KS_::CThostFtdcSettlementInfoConfirmField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+38 : Listener->OnKSftRspQryInvestorPositionCombineDetail( (KS_::CThostFtdcInvestorPositionCombineDetailField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+39 : Listener->OnKSftRspQryCFMMCTradingAccountKey( (KS_::CThostFtdcCFMMCTradingAccountKeyField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+40 : Listener->OnKSftRspQryEWarrantOffset( (KS_::CThostFtdcEWarrantOffsetField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+41 : Listener->OnKSftRspQryInvestorProductGroupMargin( (KS_::CThostFtdcInvestorProductGroupMarginField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+42 : Listener->OnKSftRspQryExchangeMarginRate( (KS_::CThostFtdcExchangeMarginRateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+43 : Listener->OnKSftRspQryExchangeMarginRateAdjust( (KS_::CThostFtdcExchangeMarginRateAdjustField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+44 : Listener->OnKSftRspQryExchangeRate( (KS_::CThostFtdcExchangeRateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+45 : Listener->OnKSftRspQrySecAgentACIDMap( (KS_::CThostFtdcSecAgentACIDMapField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+46 : Listener->OnKSftRspQryOptionInstrTradeCost( (KS_::CThostFtdcOptionInstrTradeCostField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+47 : Listener->OnKSftRspQryOptionInstrCommRate( (KS_::CThostFtdcOptionInstrCommRateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+48 : Listener->OnKSftRspQryExecOrder( (KS_::CThostFtdcExecOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+49 : Listener->OnKSftRspQryForQuote( (KS_::CThostFtdcForQuoteField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+50 : Listener->OnKSftRspQryQuote( (KS_::CThostFtdcQuoteField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+51 : Listener->OnKSftRspQryTransferSerial( (KS_::CThostFtdcTransferSerialField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+52 : Listener->OnKSftRspQryAccountregister( (KS_::CThostFtdcAccountregisterField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+53 : Listener->OnKSftRspError( (KS_::CThostFtdcRspInfoField*)Params->Param1.AsPtr(), Params->Param2.AsInt(), Params->Param3.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+54 : Listener->OnKSftRtnOrder( (KS_::CThostFtdcOrderField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+55 : Listener->OnKSftRtnTrade( (KS_::CThostFtdcTradeField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+56 : Listener->OnKSftErrRtnOrderInsert( (KS_::CThostFtdcInputOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+57 : Listener->OnKSftErrRtnOrderAction( (KS_::CThostFtdcOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+58 : Listener->OnKSftRtnInstrumentStatus( (KS_::CThostFtdcInstrumentStatusField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+59 : Listener->OnKSftRtnTradingNotice( (KS_::CThostFtdcTradingNoticeInfoField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+60 : Listener->OnKSftRtnErrorConditionalOrder( (KS_::CThostFtdcErrorConditionalOrderField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+61 : Listener->OnKSftRtnExecOrder( (KS_::CThostFtdcExecOrderField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+62 : Listener->OnKSftErrRtnExecOrderInsert( (KS_::CThostFtdcInputExecOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+63 : Listener->OnKSftErrRtnExecOrderAction( (KS_::CThostFtdcExecOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+64 : Listener->OnKSftErrRtnForQuoteInsert( (KS_::CThostFtdcInputExecOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+65 : Listener->OnKSftRtnQuote( (KS_::CThostFtdcQuoteField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+66 : Listener->OnKSftErrRtnQuoteInsert( (KS_::CThostFtdcInputQuoteField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+67 : Listener->OnKSftErrRtnQuoteAction( (KS_::CThostFtdcQuoteActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+68 : Listener->OnKSftRspQryContractBank( (KS_::CThostFtdcContractBankField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+69 : Listener->OnKSftRspQryParkedOrder( (KS_::CThostFtdcParkedOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+70 : Listener->OnKSftRspQryParkedOrderAction( (KS_::CThostFtdcParkedOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+71 : Listener->OnKSftRspQryTradingNotice( (KS_::CThostFtdcTradingNoticeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+72 : Listener->OnKSftRspQryBrokerTradingParams( (KS_::CThostFtdcBrokerTradingParamsField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+73 : Listener->OnKSftRspQryBrokerTradingAlgos( (KS_::CThostFtdcBrokerTradingAlgosField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+74 : Listener->OnKSftRtnFromBankToFutureByBank( (KS_::CThostFtdcRspTransferField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+75 : Listener->OnKSftRtnFromFutureToBankByBank( (KS_::CThostFtdcRspTransferField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+76 : Listener->OnKSftRtnRepealFromBankToFutureByBank( (KS_::CThostFtdcRspRepealField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+77 : Listener->OnKSftRtnRepealFromFutureToBankByBank( (KS_::CThostFtdcRspRepealField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+78 : Listener->OnKSftRtnFromBankToFutureByFuture( (KS_::CThostFtdcRspTransferField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+79 : Listener->OnKSftRtnFromFutureToBankByFuture( (KS_::CThostFtdcRspTransferField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+80 : Listener->OnKSftRtnRepealFromBankToFutureByFutureManual( (KS_::CThostFtdcRspRepealField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+81 : Listener->OnKSftRtnRepealFromFutureToBankByFutureManual( (KS_::CThostFtdcRspRepealField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+82 : Listener->OnKSftRtnQueryBankBalanceByFuture( (KS_::CThostFtdcNotifyQueryAccountField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+83 : Listener->OnKSftErrRtnBankToFutureByFuture( (KS_::CThostFtdcReqTransferField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+84 : Listener->OnKSftErrRtnFutureToBankByFuture( (KS_::CThostFtdcReqTransferField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+85 : Listener->OnKSftErrRtnRepealBankToFutureByFutureManual( (KS_::CThostFtdcReqRepealField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+86 : Listener->OnKSftErrRtnRepealFutureToBankByFutureManual( (KS_::CThostFtdcReqRepealField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+87 : Listener->OnKSftErrRtnQueryBankBalanceByFuture( (KS_::CThostFtdcReqQueryAccountField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+88 : Listener->OnKSftRtnRepealFromBankToFutureByFuture( (KS_::CThostFtdcRspRepealField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+89 : Listener->OnKSftRtnRepealFromFutureToBankByFuture( (KS_::CThostFtdcRspRepealField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+90 : Listener->OnKSftRspFromBankToFutureByFuture( (KS_::CThostFtdcReqTransferField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+91 : Listener->OnKSftRspFromFutureToBankByFuture( (KS_::CThostFtdcReqTransferField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+92 : Listener->OnKSftRspQueryBankAccountMoneyByFuture( (KS_::CThostFtdcReqQueryAccountField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+93 : Listener->OnKSftRtnOpenAccountByBank( (KS_::CThostFtdcOpenAccountField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+94 : Listener->OnKSftRtnCancelAccountByBank( (KS_::CThostFtdcCancelAccountField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+95 : Listener->OnKSftRtnChangeAccountByBank( (KS_::CThostFtdcChangeAccountField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+96 : Listener->OnKSftRspQryInvestorOpenPosition( (KS_::CThostFtdcInvestorPositionDetailField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+97 : Listener->OnKSftRspQryInvestorOpenCombinePosition( (KS_::CThostFtdcInvestorPositionCombineDetailField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+98 : Listener->OnKSftRspBulkCancelOrder( (KS_::CThostFtdcBulkCancelOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSFT_EVENT+99 : Listener->OnKSftAPIRecoverFinished( (int)lParam );
							return 0;
	default               : return DefWindowProc( hwnd, Message, wParam, lParam );
	}
	return DefWindowProc( hwnd, Message, wParam, lParam );
}
//------------------------------------------------------------------------------------------------------------------------
bool TKSftTraderApi::SupportKSftAPI( void )
{
	return gKSFTAPI.SupportKSftAPI( );
}
//------------------------------------------------------------------------------------------------------------------------
const char* TKSftTraderApi::LoadKSftAPIResult( void )
{
	return gKSFTAPI.Reason( );
}
//------------------------------------------------------------------------------------------------------------------------
void TKSftTraderApi::RegisterSpi( TKSftTraderSpi *pSpi )
{
	FListener = pSpi;
}
//------------------------------------------------------------------------------------------------------------------------
void TKSftTraderApi::ReqOnRecoverFilisned( int Count )
{
	TKSftTraderSpi*		Spi  = GetSpi( );
	HWND				Hwnd = GetHWND( );

	PostMessage( Hwnd, WM_KSFT_EVENT+99, (WPARAM)Spi, (LPARAM)Count );
}
//------------------------------------------------------------------------------------------------------------------------
void TKSftTraderApi::Init()
{
	gKSFTAPI.pInit( FKSFTAPI );
}
//------------------------------------------------------------------------------------------------------------------------
const char* TKSftTraderApi::GetTradingDay()
{
	return gKSFTAPI.pGetTradingDay( FKSFTAPI );
}
//------------------------------------------------------------------------------------------------------------------------
void TKSftTraderApi::RegisterFront( char *pszFrontAddress )
{
	gKSFTAPI.pRegisterFront( FKSFTAPI, pszFrontAddress );
}
//------------------------------------------------------------------------------------------------------------------------
void TKSftTraderApi::RegisterNameServer( char *pszNsAddress )
{
	gKSFTAPI.pRegisterNameServer( FKSFTAPI, pszNsAddress );
}
//------------------------------------------------------------------------------------------------------------------------
void TKSftTraderApi::RegisterFensUserInfo( KS_::CThostFtdcFensUserInfoField * pFensUserInfo )
{
	gKSFTAPI.pRegisterFensUserInfo( FKSFTAPI, pFensUserInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void TKSftTraderApi::SubscribePrivateTopic( KS_::THOST_TE_RESUME_TYPE nResumeType )
{
	gKSFTAPI.pSubscribePrivateTopic( FKSFTAPI, nResumeType );
}
//------------------------------------------------------------------------------------------------------------------------
void TKSftTraderApi::SubscribePublicTopic( KS_::THOST_TE_RESUME_TYPE nResumeType )
{
	gKSFTAPI.pSubscribePublicTopic( FKSFTAPI, nResumeType );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqAuthenticate( KS_::CThostFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID )
{
	return gKSFTAPI.pReqAuthenticate( FKSFTAPI, pReqAuthenticateField, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqUserLogin( KS_::CThostFtdcReqUserLoginField *pReqUserLoginField, int nRequestID )
{
	return gKSFTAPI.pReqUserLogin( FKSFTAPI, pReqUserLoginField, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqUserLogout( KS_::CThostFtdcUserLogoutField *pUserLogout, int nRequestID )
{
	return gKSFTAPI.pReqUserLogout( FKSFTAPI, pUserLogout, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqUserPasswordUpdate( KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID )
{
	return gKSFTAPI.pReqUserPasswordUpdate( FKSFTAPI, pUserPasswordUpdate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqTradingAccountPasswordUpdate( KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, int nRequestID )
{
	return gKSFTAPI.pReqTradingAccountPasswordUpdate( FKSFTAPI, pTradingAccountPasswordUpdate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, int nRequestID )
{
	return gKSFTAPI.pReqOrderInsert( FKSFTAPI, pInputOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqParkedOrderInsert( KS_::CThostFtdcParkedOrderField *pParkedOrder, int nRequestID )
{
	return gKSFTAPI.pReqParkedOrderInsert( FKSFTAPI, pParkedOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqParkedOrderAction( KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, int nRequestID )
{
	return gKSFTAPI.pReqParkedOrderAction( FKSFTAPI, pParkedOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqOrderAction( KS_::CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID )
{
	return gKSFTAPI.pReqOrderAction( FKSFTAPI, pInputOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQueryMaxOrderVolume( KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, int nRequestID )
{
	return gKSFTAPI.pReqQueryMaxOrderVolume( FKSFTAPI, pQueryMaxOrderVolume, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqSettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, int nRequestID )
{
	return gKSFTAPI.pReqSettlementInfoConfirm( FKSFTAPI, pSettlementInfoConfirm, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqRemoveParkedOrder( KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, int nRequestID )
{
	return gKSFTAPI.pReqRemoveParkedOrder( FKSFTAPI, pRemoveParkedOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqRemoveParkedOrderAction( KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, int nRequestID )
{
	return gKSFTAPI.pReqRemoveParkedOrderAction( FKSFTAPI, pRemoveParkedOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqExecOrderInsert( KS_::CThostFtdcInputExecOrderField *pInputExecOrder, int nRequestID )
{
	return gKSFTAPI.pReqExecOrderInsert( FKSFTAPI, pInputExecOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqExecOrderAction( KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, int nRequestID )
{
	return gKSFTAPI.pReqExecOrderAction( FKSFTAPI, pInputExecOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqForQuoteInsert( KS_::CThostFtdcInputForQuoteField *pInputForQuote, int nRequestID )
{
	return gKSFTAPI.pReqForQuoteInsert( FKSFTAPI, pInputForQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQuoteInsert( KS_::CThostFtdcInputQuoteField *pInputQuote, int nRequestID )
{
	return gKSFTAPI.pReqQuoteInsert( FKSFTAPI, pInputQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQuoteAction( KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, int nRequestID )
{
	return gKSFTAPI.pReqQuoteAction( FKSFTAPI, pInputQuoteAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryOrder( KS_::CThostFtdcQryOrderField *pQryOrder, int nRequestID )
{
	return gKSFTAPI.pReqQryOrder( FKSFTAPI, pQryOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryTrade( KS_::CThostFtdcQryTradeField *pQryTrade, int nRequestID )
{
	return gKSFTAPI.pReqQryTrade( FKSFTAPI, pQryTrade, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryInvestorPosition( KS_::CThostFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID )
{
	return gKSFTAPI.pReqQryInvestorPosition( FKSFTAPI, pQryInvestorPosition, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryTradingAccount( KS_::CThostFtdcQryTradingAccountField *pQryTradingAccount, int nRequestID )
{
	return gKSFTAPI.pReqQryTradingAccount( FKSFTAPI, pQryTradingAccount, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryInvestor( KS_::CThostFtdcQryInvestorField *pQryInvestor, int nRequestID )
{
	return gKSFTAPI.pReqQryInvestor( FKSFTAPI, pQryInvestor, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryTradingCode( KS_::CThostFtdcQryTradingCodeField *pQryTradingCode, int nRequestID )
{
	return gKSFTAPI.pReqQryTradingCode( FKSFTAPI, pQryTradingCode, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryInstrumentMarginRate( KS_::CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID )
{
	return gKSFTAPI.pReqQryInstrumentMarginRate( FKSFTAPI, pQryInstrumentMarginRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryInstrumentCommissionRate( KS_::CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate, int nRequestID )
{
	return gKSFTAPI.pReqQryInstrumentCommissionRate( FKSFTAPI, pQryInstrumentCommissionRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryExchange( KS_::CThostFtdcQryExchangeField *pQryExchange, int nRequestID )
{
	return gKSFTAPI.pReqQryExchange( FKSFTAPI, pQryExchange, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryProduct( KS_::CThostFtdcQryProductField *pQryProduct, int nRequestID )
{
	return gKSFTAPI.pReqQryProduct( FKSFTAPI, pQryProduct, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryInstrument( KS_::CThostFtdcQryInstrumentField *pQryInstrument, int nRequestID )
{
	return gKSFTAPI.pReqQryInstrument( FKSFTAPI, pQryInstrument, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryDepthMarketData( KS_::CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, int nRequestID )
{
	return gKSFTAPI.pReqQryDepthMarketData( FKSFTAPI, pQryDepthMarketData, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQrySettlementInfo( KS_::CThostFtdcQrySettlementInfoField *pQrySettlementInfo, int nRequestID )
{
	return gKSFTAPI.pReqQrySettlementInfo( FKSFTAPI, pQrySettlementInfo, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryTransferBank( KS_::CThostFtdcQryTransferBankField *pQryTransferBank, int nRequestID )
{
	return gKSFTAPI.pReqQryTransferBank( FKSFTAPI, pQryTransferBank, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryInvestorPositionDetail( KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, int nRequestID )
{
	return gKSFTAPI.pReqQryInvestorPositionDetail( FKSFTAPI, pQryInvestorPositionDetail, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryNotice( KS_::CThostFtdcQryNoticeField *pQryNotice, int nRequestID )
{
	return gKSFTAPI.pReqQryNotice( FKSFTAPI, pQryNotice, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQrySettlementInfoConfirm( KS_::CThostFtdcQrySettlementInfoConfirmField *pQrySettlementInfoConfirm, int nRequestID )
{
	return gKSFTAPI.pReqQrySettlementInfoConfirm( FKSFTAPI, pQrySettlementInfoConfirm, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryInvestorPositionCombineDetail( KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorPositionCombineDetail, int nRequestID )
{
	return gKSFTAPI.pReqQryInvestorPositionCombineDetail( FKSFTAPI, pQryInvestorPositionCombineDetail, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryCFMMCTradingAccountKey( KS_::CThostFtdcQryCFMMCTradingAccountKeyField *pQryCFMMCTradingAccountKey, int nRequestID )
{
	return gKSFTAPI.pReqQryCFMMCTradingAccountKey( FKSFTAPI, pQryCFMMCTradingAccountKey, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryEWarrantOffset( KS_::CThostFtdcQryEWarrantOffsetField *pQryEWarrantOffset, int nRequestID )
{
	return gKSFTAPI.pReqQryEWarrantOffset( FKSFTAPI, pQryEWarrantOffset, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryInvestorProductGroupMargin( KS_::CThostFtdcQryInvestorProductGroupMarginField *pQryInvestorProductGroupMargin, int nRequestID )
{
	return gKSFTAPI.pReqQryInvestorProductGroupMargin( FKSFTAPI, pQryInvestorProductGroupMargin, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryExchangeMarginRate( KS_::CThostFtdcQryExchangeMarginRateField *pQryExchangeMarginRate, int nRequestID)
{
	return gKSFTAPI.pReqQryExchangeMarginRate( FKSFTAPI, pQryExchangeMarginRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryExchangeMarginRateAdjust( KS_::CThostFtdcQryExchangeMarginRateAdjustField *pQryExchangeMarginRateAdjust, int nRequestID )
{
	return gKSFTAPI.pReqQryExchangeMarginRateAdjust( FKSFTAPI, pQryExchangeMarginRateAdjust, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryExchangeRate( KS_::CThostFtdcQryExchangeRateField *pQryExchangeRate, int nRequestID )
{
	return gKSFTAPI.pReqQryExchangeRate( FKSFTAPI, pQryExchangeRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQrySecAgentACIDMap( KS_::CThostFtdcQrySecAgentACIDMapField *pQrySecAgentACIDMap, int nRequestID )
{
	return gKSFTAPI.pReqQrySecAgentACIDMap( FKSFTAPI, pQrySecAgentACIDMap, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryOptionInstrTradeCost( KS_::CThostFtdcQryOptionInstrTradeCostField *pQryOptionInstrTradeCost, int nRequestID )
{
	return gKSFTAPI.pReqQryOptionInstrTradeCost( FKSFTAPI, pQryOptionInstrTradeCost, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryOptionInstrCommRate( KS_::CThostFtdcQryOptionInstrCommRateField *pQryOptionInstrCommRate, int nRequestID )
{
	return gKSFTAPI.pReqQryOptionInstrCommRate( FKSFTAPI, pQryOptionInstrCommRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryExecOrder( KS_::CThostFtdcQryExecOrderField *pQryExecOrder, int nRequestID )
{
	return gKSFTAPI.pReqQryExecOrder( FKSFTAPI, pQryExecOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryForQuote( KS_::CThostFtdcQryForQuoteField *pQryForQuote, int nRequestID )
{
	return gKSFTAPI.pReqQryForQuote( FKSFTAPI, pQryForQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryQuote( KS_::CThostFtdcQryQuoteField *pQryQuote, int nRequestID )
{
	return gKSFTAPI.pReqQryQuote( FKSFTAPI, pQryQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryTransferSerial( KS_::CThostFtdcQryTransferSerialField *pQryTransferSerial, int nRequestID )
{
	return gKSFTAPI.pReqQryTransferSerial( FKSFTAPI, pQryTransferSerial, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryAccountregister( KS_::CThostFtdcQryAccountregisterField *pQryAccountregister, int nRequestID )
{
	return gKSFTAPI.pReqQryAccountregister( FKSFTAPI, pQryAccountregister, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryContractBank( KS_::CThostFtdcQryContractBankField *pQryContractBank, int nRequestID )
{
	return gKSFTAPI.pReqQryContractBank( FKSFTAPI, pQryContractBank, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryParkedOrder( KS_::CThostFtdcQryParkedOrderField *pQryParkedOrder, int nRequestID )
{
	return gKSFTAPI.pReqQryParkedOrder( FKSFTAPI, pQryParkedOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryParkedOrderAction( KS_::CThostFtdcQryParkedOrderActionField *pQryParkedOrderAction, int nRequestID )
{
	return gKSFTAPI.pReqQryParkedOrderAction( FKSFTAPI, pQryParkedOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryTradingNotice( KS_::CThostFtdcQryTradingNoticeField *pQryTradingNotice, int nRequestID )
{
	return gKSFTAPI.pReqQryTradingNotice( FKSFTAPI, pQryTradingNotice, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryBrokerTradingParams( KS_::CThostFtdcQryBrokerTradingParamsField *pQryBrokerTradingParams, int nRequestID )
{
	return gKSFTAPI.pReqQryBrokerTradingParams( FKSFTAPI, pQryBrokerTradingParams, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQryBrokerTradingAlgos( KS_::CThostFtdcQryBrokerTradingAlgosField *pQryBrokerTradingAlgos, int nRequestID )
{
	return gKSFTAPI.pReqQryBrokerTradingAlgos( FKSFTAPI, pQryBrokerTradingAlgos, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqFromBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID )
{
	return gKSFTAPI.pReqFromBankToFutureByFuture( FKSFTAPI, pReqTransfer, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqFromFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID )
{
	return gKSFTAPI.pReqFromFutureToBankByFuture( FKSFTAPI, pReqTransfer, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQueryBankAccountMoneyByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, int nRequestID )
{
	return gKSFTAPI.pReqQueryBankAccountMoneyByFuture( FKSFTAPI, pReqQueryAccount, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQueryInvestorOpenPosition( KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorOpenPosition, int nRequestID )
{
	return gKSFTAPI.pReqQueryInvestorOpenPosition( FKSFTAPI, pQryInvestorOpenPosition, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqQueryInvestorOpenCombinePosition( KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorOpenCombinePosition, int nRequestID )
{
	return gKSFTAPI.pReqQueryInvestorOpenCombinePosition( FKSFTAPI, pQryInvestorOpenCombinePosition, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
void* TKSftTraderApi::LoadExtApi( void * spi, const char *ExtApiName )
{
	return gKSFTAPI.pLoadExtApi( FKSFTAPI, spi, ExtApiName );
}
//------------------------------------------------------------------------------------------------------------------------
int TKSftTraderApi::ReqBulkCancelOrder ( KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, int nRequestID )
{
	return gKSFTAPI.pReqBulkCancelOrder( FKSFTAPI, pBulkCancelOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
