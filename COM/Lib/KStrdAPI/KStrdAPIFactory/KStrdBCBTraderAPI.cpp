//---------------------------------------------------------------------------
#ifdef _CTP
//------------------------------------------------------------------------------------------------------------------------
#ifndef _KSFT
//------------------------------------------------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include "KStrdBCBTraderAPI.h"
#include "KStrdAPIDynamic.h"
#include "KStrdBCBEvents.h"
#include "APIParams.h"
#include "UFC.h"
//------------------------------------------------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------------------------------------------------
KStrdTraderApiInit	gKSTRDAPI;
HINSTANCE			gKStrdDLLInstance = NULL;
//------------------------------------------------------------------------------------------------------------------------
//
// Class TKStrdTraderApi
// Wrapper for KingStar KStrd API visual C++ version.
//
//------------------------------------------------------------------------------------------------------------------------
TKStrdTraderApi::TKStrdTraderApi( TKStrdTraderSpi* pSpi )
: FKSTRDAPI( NULL )
, FListener( pSpi )
{
	// 1. Check KStrd API exist
	//    < KStrdAPIFactory.dll >
	//    < KStrdAPIFactory.dll  KSTraderAPI.dll  ksPortalAPI.dll  lkcdll.dll  SSPXEncode.dll >
	if ( TKStrdTraderApi::SupportKStrdAPI() == false )
		throw UFC::Exception( " Not support KStrd API." );
	// 2. Create KStrd Trader  API Instance.
	if ( ( FKSTRDAPI = gKSTRDAPI.pCreateKStrdTraderApi( this ) ) == NULL )
		throw UFC::Exception( " Create TKStrdTraderApi failed!" );
	// 3. Register all call back functions.
	RegAllCallbackFuncs( );
	// 4. Create message window
	if ( InitMessageWindow() == FALSE )
	{
		UFC::BufferedLog::DebugPrintf( " Register KStrd window class failed." );
		throw UFC::Exception( " Register KStrd window class failed." );
	}
	if ( CreateMessageWindow() == FALSE )
	{
		UFC::BufferedLog::DebugPrintf( " Can't create the KStrd message window." );
		throw UFC::Exception( " Can't create the KStrd message window." );
	}
	UFC::BufferedLog::DebugPrintf( " Create KStrd message window:%p Thread:%d", (void*)FHwnd, GetCurrentThreadId() );
	// 5. Check KingStar license key file
	UFC::BufferedLog::Printf( " Create KKFT message window:%p Thread:%d", (void*)FHwnd, GetCurrentThreadId() );
	if ( UFC::FileExists( "KSInterB2C.lkc" ) == FALSE )
	{
		UFC::BufferedLog::DebugPrintf( "KingStar license key file not exist." );
		throw UFC::Exception( "KingStar license key file not exist." );
	}
}
//------------------------------------------------------------------------------------------------------------------------
TKStrdTraderApi::~TKStrdTraderApi()
{
	RegisterSpi( NULL );
	FListener = NULL;
	DestroyWindow( FHwnd );
	UnregisterClassW( MessageWindowsClassName, FInstance );
	gKSTRDAPI.pReleaseKStrdTraderApi( FKSTRDAPI );
}
//------------------------------------------------------------------------------------------------------------------------
BOOL TKStrdTraderApi::InitMessageWindow( void )
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
		swprintf( MessageWindowsClassName, L"KStrd_API_MsgWin_%10d", UFC::GetTickCountMS() );
	// Fill in the window class structure with parameters that describe the main window.
	wcx.cbSize = sizeof( wcx );                 // size of structure
	wcx.style = CS_HREDRAW |CS_VREDRAW;         // redraw if size changes
	wcx.lpfnWndProc   = TKStrdTraderApi::MessageWndProc;// points to window procedure
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
BOOL TKStrdTraderApi::CreateMessageWindow( void )
{
	FHwnd = CreateWindowW( MessageWindowsClassName,	// name of window class
						   L"KStrd_API_App",			// title-bar string
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
void TKStrdTraderApi::RegAllCallbackFuncs( void )
{
	// ---- CThostFtdcTraderSpi ----
	gKSTRDAPI.pRegOnFrontConnected( FKSTRDAPI,                          KStrdTraderApiInit::OnFrontConnected );
	gKSTRDAPI.pRegOnFrontDisconnected( FKSTRDAPI,                       KStrdTraderApiInit::OnFrontDisconnected );
	gKSTRDAPI.pRegOnHeartBeatWarning( FKSTRDAPI,                        KStrdTraderApiInit::OnHeartBeatWarning );
	gKSTRDAPI.pRegOnRspAuthenticate( FKSTRDAPI,                         KStrdTraderApiInit::OnRspAuthenticate );
	gKSTRDAPI.pRegOnRspUserLogin( FKSTRDAPI,                            KStrdTraderApiInit::OnRspUserLogin );
	gKSTRDAPI.pRegOnRspUserLogout( FKSTRDAPI,                           KStrdTraderApiInit::OnRspUserLogout );
	gKSTRDAPI.pRegOnRspUserPasswordUpdate( FKSTRDAPI,                   KStrdTraderApiInit::OnRspUserPasswordUpdate );
	gKSTRDAPI.pRegOnRspTradingAccountPasswordUpdate( FKSTRDAPI,         KStrdTraderApiInit::OnRspTradingAccountPasswordUpdate );
	gKSTRDAPI.pRegOnRspOrderInsert( FKSTRDAPI,                          KStrdTraderApiInit::OnRspOrderInsert );
	gKSTRDAPI.pRegOnRspParkedOrderInsert( FKSTRDAPI,                    KStrdTraderApiInit::OnRspParkedOrderInsert );
	gKSTRDAPI.pRegOnRspParkedOrderAction( FKSTRDAPI,                    KStrdTraderApiInit::OnRspParkedOrderAction );
	gKSTRDAPI.pRegOnRspOrderAction( FKSTRDAPI,                          KStrdTraderApiInit::OnRspOrderAction );
	gKSTRDAPI.pRegOnRspQueryMaxOrderVolume( FKSTRDAPI,                  KStrdTraderApiInit::OnRspQueryMaxOrderVolume );
	gKSTRDAPI.pRegOnRspSettlementInfoConfirm( FKSTRDAPI,                KStrdTraderApiInit::OnRspSettlementInfoConfirm );
	gKSTRDAPI.pRegOnRspRemoveParkedOrder( FKSTRDAPI,                    KStrdTraderApiInit::OnRspRemoveParkedOrder );
	gKSTRDAPI.pRegOnRspRemoveParkedOrderAction( FKSTRDAPI,              KStrdTraderApiInit::OnRspRemoveParkedOrderAction );
	gKSTRDAPI.pRegOnRspExecOrderInsert( FKSTRDAPI,                      KStrdTraderApiInit::OnRspExecOrderInsert );
	gKSTRDAPI.pRegOnRspExecOrderAction( FKSTRDAPI,                      KStrdTraderApiInit::OnRspExecOrderAction );
	gKSTRDAPI.pRegOnRspForQuoteInsert( FKSTRDAPI,                       KStrdTraderApiInit::OnRspForQuoteInsert );
	gKSTRDAPI.pRegOnRspQuoteInsert( FKSTRDAPI,                          KStrdTraderApiInit::OnRspQuoteInsert );
	gKSTRDAPI.pRegOnRspQuoteAction( FKSTRDAPI,                          KStrdTraderApiInit::OnRspQuoteAction );
	gKSTRDAPI.pRegOnRspQryOrder( FKSTRDAPI,                             KStrdTraderApiInit::OnRspQryOrder );
	gKSTRDAPI.pRegOnRspQryTrade( FKSTRDAPI,                             KStrdTraderApiInit::OnRspQryTrade );
	gKSTRDAPI.pRegOnRspQryInvestorPosition( FKSTRDAPI,                  KStrdTraderApiInit::OnRspQryInvestorPosition );
	gKSTRDAPI.pRegOnRspQryTradingAccount( FKSTRDAPI,                    KStrdTraderApiInit::OnRspQryTradingAccount );
	gKSTRDAPI.pRegOnRspQryInvestor( FKSTRDAPI,                          KStrdTraderApiInit::OnRspQryInvestor );
	gKSTRDAPI.pRegOnRspQryTradingCode( FKSTRDAPI,                       KStrdTraderApiInit::OnRspQryTradingCode );
	gKSTRDAPI.pRegOnRspQryInstrumentMarginRate( FKSTRDAPI,              KStrdTraderApiInit::OnRspQryInstrumentMarginRate );
	gKSTRDAPI.pRegOnRspQryInstrumentCommissionRate( FKSTRDAPI,          KStrdTraderApiInit::OnRspQryInstrumentCommissionRate );
	gKSTRDAPI.pRegOnRspQryExchange( FKSTRDAPI,                          KStrdTraderApiInit::OnRspQryExchange );
	gKSTRDAPI.pRegOnRspQryProduct( FKSTRDAPI,                           KStrdTraderApiInit::OnRspQryProduct );
	gKSTRDAPI.pRegOnRspQryInstrument( FKSTRDAPI,                        KStrdTraderApiInit::OnRspQryInstrument );
	gKSTRDAPI.pRegOnRspQryDepthMarketData( FKSTRDAPI,                   KStrdTraderApiInit::OnRspQryDepthMarketData );
	gKSTRDAPI.pRegOnRspQrySettlementInfo( FKSTRDAPI,                    KStrdTraderApiInit::OnRspQrySettlementInfo );
	gKSTRDAPI.pRegOnRspQryTransferBank( FKSTRDAPI,                      KStrdTraderApiInit::OnRspQryTransferBank );
	gKSTRDAPI.pRegOnRspQryInvestorPositionDetail( FKSTRDAPI,            KStrdTraderApiInit::OnRspQryInvestorPositionDetail );
	gKSTRDAPI.pRegOnRspQryNotice( FKSTRDAPI,                            KStrdTraderApiInit::OnRspQryNotice );
	gKSTRDAPI.pRegOnRspQrySettlementInfoConfirm( FKSTRDAPI,             KStrdTraderApiInit::OnRspQrySettlementInfoConfirm );
	gKSTRDAPI.pRegOnRspQryInvestorPositionCombineDetail( FKSTRDAPI,     KStrdTraderApiInit::OnRspQryInvestorPositionCombineDetail );
	gKSTRDAPI.pRegOnRspQryCFMMCTradingAccountKey( FKSTRDAPI,            KStrdTraderApiInit::OnRspQryCFMMCTradingAccountKey );
	gKSTRDAPI.pRegOnRspQryEWarrantOffset( FKSTRDAPI,                    KStrdTraderApiInit::OnRspQryEWarrantOffset );
	gKSTRDAPI.pRegOnRspQryInvestorProductGroupMargin( FKSTRDAPI,        KStrdTraderApiInit::OnRspQryInvestorProductGroupMargin );
	gKSTRDAPI.pRegOnRspQryExchangeMarginRate( FKSTRDAPI,                KStrdTraderApiInit::OnRspQryExchangeMarginRate );
	gKSTRDAPI.pRegOnRspQryExchangeMarginRateAdjust( FKSTRDAPI,          KStrdTraderApiInit::OnRspQryExchangeMarginRateAdjust );
	gKSTRDAPI.pRegOnRspQryExchangeRate( FKSTRDAPI,                      KStrdTraderApiInit::OnRspQryExchangeRate );
	gKSTRDAPI.pRegOnRspQrySecAgentACIDMap( FKSTRDAPI,                   KStrdTraderApiInit::OnRspQrySecAgentACIDMap );
	gKSTRDAPI.pRegOnRspQryOptionInstrTradeCost( FKSTRDAPI,              KStrdTraderApiInit::OnRspQryOptionInstrTradeCost );
	gKSTRDAPI.pRegOnRspQryOptionInstrCommRate( FKSTRDAPI,               KStrdTraderApiInit::OnRspQryOptionInstrCommRate );
	gKSTRDAPI.pRegOnRspQryExecOrder( FKSTRDAPI,                         KStrdTraderApiInit::OnRspQryExecOrder );
	gKSTRDAPI.pRegOnRspQryForQuote( FKSTRDAPI,                          KStrdTraderApiInit::OnRspQryForQuote );
	gKSTRDAPI.pRegOnRspQryQuote( FKSTRDAPI,                             KStrdTraderApiInit::OnRspQryQuote );
	gKSTRDAPI.pRegOnRspQryTransferSerial( FKSTRDAPI,                    KStrdTraderApiInit::OnRspQryTransferSerial );
	gKSTRDAPI.pRegOnRspQryAccountregister( FKSTRDAPI,                   KStrdTraderApiInit::OnRspQryAccountregister );
	gKSTRDAPI.pRegOnRspError( FKSTRDAPI,                                KStrdTraderApiInit::OnRspError );
	gKSTRDAPI.pRegOnRtnOrder( FKSTRDAPI,                                KStrdTraderApiInit::OnRtnOrder );
	gKSTRDAPI.pRegOnRtnTrade( FKSTRDAPI,                                KStrdTraderApiInit::OnRtnTrade );
	gKSTRDAPI.pRegOnErrRtnOrderInsert( FKSTRDAPI,                       KStrdTraderApiInit::OnErrRtnOrderInsert );
	gKSTRDAPI.pRegOnErrRtnOrderAction( FKSTRDAPI,                       KStrdTraderApiInit::OnErrRtnOrderAction );
	gKSTRDAPI.pRegOnRtnInstrumentStatus( FKSTRDAPI,                     KStrdTraderApiInit::OnRtnInstrumentStatus );
	gKSTRDAPI.pRegOnRtnTradingNotice( FKSTRDAPI,                        KStrdTraderApiInit::OnRtnTradingNotice );
	gKSTRDAPI.pRegOnRtnErrorConditionalOrder( FKSTRDAPI,                KStrdTraderApiInit::OnRtnErrorConditionalOrder );
	gKSTRDAPI.pRegOnRtnExecOrder( FKSTRDAPI,                            KStrdTraderApiInit::OnRtnExecOrder );
	gKSTRDAPI.pRegOnErrRtnExecOrderInsert( FKSTRDAPI,                   KStrdTraderApiInit::OnErrRtnExecOrderInsert );
	gKSTRDAPI.pRegOnErrRtnExecOrderAction( FKSTRDAPI,                   KStrdTraderApiInit::OnErrRtnExecOrderAction );
	gKSTRDAPI.pRegOnErrRtnForQuoteInsert( FKSTRDAPI,                    KStrdTraderApiInit::OnErrRtnForQuoteInsert );
	gKSTRDAPI.pRegOnRtnQuote( FKSTRDAPI,                                KStrdTraderApiInit::OnRtnQuote );
	gKSTRDAPI.pRegOnErrRtnQuoteInsert( FKSTRDAPI,                       KStrdTraderApiInit::OnErrRtnQuoteInsert );
	gKSTRDAPI.pRegOnErrRtnQuoteAction( FKSTRDAPI,                       KStrdTraderApiInit::OnErrRtnQuoteAction );
	gKSTRDAPI.pRegOnRtnForQuoteRsp( FKSTRDAPI,                          KStrdTraderApiInit::OnRtnForQuoteRsp );
	gKSTRDAPI.pRegOnRtnCFMMCTradingAccountToken( FKSTRDAPI,             KStrdTraderApiInit::OnRtnCFMMCTradingAccountToken );
	gKSTRDAPI.pRegOnRspQryContractBank( FKSTRDAPI,                      KStrdTraderApiInit::OnRspQryContractBank );
	gKSTRDAPI.pRegOnRspQryParkedOrder( FKSTRDAPI,                       KStrdTraderApiInit::OnRspQryParkedOrder );
	gKSTRDAPI.pRegOnRspQryParkedOrderAction( FKSTRDAPI,                 KStrdTraderApiInit::OnRspQryParkedOrderAction );
	gKSTRDAPI.pRegOnRspQryTradingNotice( FKSTRDAPI,                     KStrdTraderApiInit::OnRspQryTradingNotice );
	gKSTRDAPI.pRegOnRspQryBrokerTradingParams( FKSTRDAPI,               KStrdTraderApiInit::OnRspQryBrokerTradingParams );
	gKSTRDAPI.pRegOnRspQryBrokerTradingAlgos( FKSTRDAPI,                KStrdTraderApiInit::OnRspQryBrokerTradingAlgos );
	gKSTRDAPI.pRegOnRspQueryCFMMCTradingAccountToken( FKSTRDAPI,        KStrdTraderApiInit::OnRspQueryCFMMCTradingAccountToken );
	gKSTRDAPI.pRegOnRtnFromBankToFutureByBank( FKSTRDAPI,               KStrdTraderApiInit::OnRtnFromBankToFutureByBank );
	gKSTRDAPI.pRegOnRtnFromFutureToBankByBank( FKSTRDAPI,               KStrdTraderApiInit::OnRtnFromFutureToBankByBank );
	gKSTRDAPI.pRegOnRtnRepealFromBankToFutureByBank( FKSTRDAPI,         KStrdTraderApiInit::OnRtnRepealFromBankToFutureByBank );
	gKSTRDAPI.pRegOnRtnRepealFromFutureToBankByBank( FKSTRDAPI,         KStrdTraderApiInit::OnRtnRepealFromFutureToBankByBank );
	gKSTRDAPI.pRegOnRtnFromBankToFutureByFuture( FKSTRDAPI,             KStrdTraderApiInit::OnRtnFromBankToFutureByFuture );
	gKSTRDAPI.pRegOnRtnFromFutureToBankByFuture( FKSTRDAPI,             KStrdTraderApiInit::OnRtnFromFutureToBankByFuture );
	gKSTRDAPI.pRegOnRtnRepealFromBankToFutureByFutureManual( FKSTRDAPI, KStrdTraderApiInit::OnRtnRepealFromBankToFutureByFutureManual );
	gKSTRDAPI.pRegOnRtnRepealFromFutureToBankByFutureManual( FKSTRDAPI, KStrdTraderApiInit::OnRtnRepealFromFutureToBankByFutureManual );
	gKSTRDAPI.pRegOnRtnQueryBankBalanceByFuture( FKSTRDAPI,             KStrdTraderApiInit::OnRtnQueryBankBalanceByFuture );
	gKSTRDAPI.pRegOnErrRtnBankToFutureByFuture( FKSTRDAPI,              KStrdTraderApiInit::OnErrRtnBankToFutureByFuture );
	gKSTRDAPI.pRegOnErrRtnFutureToBankByFuture( FKSTRDAPI,              KStrdTraderApiInit::OnErrRtnFutureToBankByFuture );
	gKSTRDAPI.pRegOnErrRtnRepealBankToFutureByFutureManual( FKSTRDAPI,  KStrdTraderApiInit::OnErrRtnRepealBankToFutureByFutureManual );
	gKSTRDAPI.pRegOnErrRtnRepealFutureToBankByFutureManual( FKSTRDAPI,  KStrdTraderApiInit::OnErrRtnRepealFutureToBankByFutureManual );
	gKSTRDAPI.pRegOnErrRtnQueryBankBalanceByFuture( FKSTRDAPI,          KStrdTraderApiInit::OnErrRtnQueryBankBalanceByFuture );
	gKSTRDAPI.pRegOnRtnRepealFromBankToFutureByFuture( FKSTRDAPI,       KStrdTraderApiInit::OnRtnRepealFromBankToFutureByFuture );
	gKSTRDAPI.pRegOnRtnRepealFromFutureToBankByFuture( FKSTRDAPI,       KStrdTraderApiInit::OnRtnRepealFromFutureToBankByFuture );
	gKSTRDAPI.pRegOnRspFromBankToFutureByFuture( FKSTRDAPI,             KStrdTraderApiInit::OnRspFromBankToFutureByFuture );
	gKSTRDAPI.pRegOnRspFromFutureToBankByFuture( FKSTRDAPI,             KStrdTraderApiInit::OnRspFromFutureToBankByFuture );
	gKSTRDAPI.pRegOnRspQueryBankAccountMoneyByFuture( FKSTRDAPI,        KStrdTraderApiInit::OnRspQueryBankAccountMoneyByFuture );
	gKSTRDAPI.pRegOnRtnOpenAccountByBank( FKSTRDAPI,                    KStrdTraderApiInit::OnRtnOpenAccountByBank );
	gKSTRDAPI.pRegOnRtnCancelAccountByBank( FKSTRDAPI,                  KStrdTraderApiInit::OnRtnCancelAccountByBank );
	gKSTRDAPI.pRegOnRtnChangeAccountByBank( FKSTRDAPI,                  KStrdTraderApiInit::OnRtnChangeAccountByBank );
	// v2.3.50212 // 20150410 by Tim Lin
	gKSTRDAPI.pRegOnRspLockInsert( FKSTRDAPI,                  			KStrdTraderApiInit::OnRspLockInsert );///鎖定應答
	gKSTRDAPI.pRegOnRspQryLock( FKSTRDAPI,                  			KStrdTraderApiInit::OnRspQryLock );///請求查詢鎖定回應
	gKSTRDAPI.pRegOnRspQryLockPosition( FKSTRDAPI,                 		KStrdTraderApiInit::OnRspQryLockPosition );///請求查詢鎖定證券倉位回應
	gKSTRDAPI.pRegOnRtnLock( FKSTRDAPI,                  				KStrdTraderApiInit::OnRtnLock );///鎖定通知
	gKSTRDAPI.pRegOnErrRtnLockInsert( FKSTRDAPI,                  		KStrdTraderApiInit::OnErrRtnLockInsert );///鎖定錯誤通知
	// ---- CKSOptionSpi ----
	// v2.3.50212 // 20150410 by Tim Lin
	gKSTRDAPI.pRegOnRspQryIndexPrice( FKSTRDAPI,                       	KStrdTraderApiInit::OnRspQryIndexPrice );
	gKSTRDAPI.pRegOnRspQryOptionInstrGuard( FKSTRDAPI,                  KStrdTraderApiInit::OnRspQryOptionInstrGuard );
	gKSTRDAPI.pRegOnRspQryExecOrderVolume( FKSTRDAPI,                   KStrdTraderApiInit::OnRspQryExecOrderVolume );
	gKSTRDAPI.pRegOnRspQryStockOptionAssignment( FKSTRDAPI,             KStrdTraderApiInit::OnRspQryStockOptionAssignment );
	gKSTRDAPI.pRegOnRspQryInvestorTradeLevel( FKSTRDAPI,                KStrdTraderApiInit::OnRspQryInvestorTradeLevel );
	gKSTRDAPI.pRegOnRspQryPurchaseLimitAmt( FKSTRDAPI,                  KStrdTraderApiInit::OnRspQryPurchaseLimitAmt );
	gKSTRDAPI.pRegOnRspQryPositionLimitVol( FKSTRDAPI,                  KStrdTraderApiInit::OnRspQryPositionLimitVol );
	gKSTRDAPI.pRegOnRspQryHistoryOrder( FKSTRDAPI,                      KStrdTraderApiInit::OnRspQryHistoryOrder );
	gKSTRDAPI.pRegOnRspQryHistoryTrade( FKSTRDAPI,                      KStrdTraderApiInit::OnRspQryHistoryTrade );
	gKSTRDAPI.pRegOnRspQryHistoryAssignment( FKSTRDAPI,      			KStrdTraderApiInit::OnRspQryStockOptionHistoryAssignment );
	gKSTRDAPI.pRegOnRspQrySODelivDetail( FKSTRDAPI,            			KStrdTraderApiInit::OnRspQryStockOptionDelivDetail );
	gKSTRDAPI.pRegOnRspQryAutoExecOrderAction( FKSTRDAPI,               KStrdTraderApiInit::OnRspAutoExecOrderAction );
	/*
	gKSTRDAPI.pRegOnRspQryKSExecOrder( FKSTRDAPI,                       KStrdTraderApiInit::OnRspQryKSExecOrder );
	gKSTRDAPI.pRegOnRspKSExecOrderInsert( FKSTRDAPI,                    KStrdTraderApiInit::OnRspKSExecOrderInsert );
	gKSTRDAPI.pRegOnRspQryExecOrderVolume( FKSTRDAPI,                   KStrdTraderApiInit::OnRspQryExecOrderVolume );
	gKSTRDAPI.pRegOnRspQrySpotOptionParams( FKSTRDAPI,                  KStrdTraderApiInit::OnRspQrySpotOptionParams );
	gKSTRDAPI.pRegOnRspQryOptionInsInfo( FKSTRDAPI,                     KStrdTraderApiInit::OnRspQryOptionInsInfo );
	gKSTRDAPI.pRegOnRspQryStockOptionInsCommRate( FKSTRDAPI,            KStrdTraderApiInit::OnRspQryStockOptionInsCommRate );
	gKSTRDAPI.pRegOnRspQryStockOptionInsMarginRate( FKSTRDAPI,          KStrdTraderApiInit::OnRspQryStockOptionInsMarginRate );
	gKSTRDAPI.pRegOnRspQryStockOptionAssignment( FKSTRDAPI,             KStrdTraderApiInit::OnRspQryStockOptionAssignment );
	gKSTRDAPI.pRegOnRspQryInvestorTradeLevel( FKSTRDAPI,                KStrdTraderApiInit::OnRspQryInvestorTradeLevel );
	gKSTRDAPI.pRegOnRspQryPurchaseLimitAmt( FKSTRDAPI,                  KStrdTraderApiInit::OnRspQryPurchaseLimitAmt );
	gKSTRDAPI.pRegOnRspQryPositionLimitVol( FKSTRDAPI,                  KStrdTraderApiInit::OnRspQryPositionLimitVol );
	gKSTRDAPI.pRegOnRspQryHistoryOrder( FKSTRDAPI,                      KStrdTraderApiInit::OnRspQryHistoryOrder );
	gKSTRDAPI.pRegOnRspQryHistoryTrade( FKSTRDAPI,                      KStrdTraderApiInit::OnRspQryHistoryTrade );
	gKSTRDAPI.pRegOnRspQryStockOptionHistoryAssignment( FKSTRDAPI,      KStrdTraderApiInit::OnRspQryStockOptionHistoryAssignment );
	gKSTRDAPI.pRegOnRspQryStockOptionDelivDetail( FKSTRDAPI,            KStrdTraderApiInit::OnRspQryStockOptionDelivDetail );
	gKSTRDAPI.pRegOnRspAutoExecOrderAction( FKSTRDAPI,                  KStrdTraderApiInit::OnRspAutoExecOrderAction );
	gKSTRDAPI.pRegOnRtnKSExecOrder( FKSTRDAPI,                          KStrdTraderApiInit::OnRtnKSExecOrder );
	*/
	// ---- CKSVocSpi ----
	// v2.3.50212 // 20150410 by Tim Lin
	gKSTRDAPI.pRegOnRspQryInvestorOpenPosition( FKSTRDAPI,              KStrdTraderApiInit::OnRspQryInvestorOpenPosition );
	gKSTRDAPI.pRegOnRspQryInvestorOpenCombinePosition( FKSTRDAPI,       KStrdTraderApiInit::OnRspQryInvestorOpenCombinePosition );
	gKSTRDAPI.pRegOnRspBulkCancelOrder( FKSTRDAPI,                      KStrdTraderApiInit::OnRspBulkCancelOrder );
	gKSTRDAPI.pRegOnRspQryCloseStrategy( FKSTRDAPI,                     KStrdTraderApiInit::OnRspQryCloseStrategy );
	gKSTRDAPI.pRegOnRspQryCombStrategy( FKSTRDAPI,                      KStrdTraderApiInit::OnRspQryCombStrategy );
	gKSTRDAPI.pRegOnRspQryOptionCombStrategy( FKSTRDAPI,                KStrdTraderApiInit::OnRspQryOptionCombStrategy );
	gKSTRDAPI.pRegOnRspQryTransferInfo( FKSTRDAPI,                      KStrdTraderApiInit::OnRspQryTransferInfo );
	gKSTRDAPI.pRegOnRspQryKSTradingNotice( FKSTRDAPI,                   KStrdTraderApiInit::OnRspQryKSTradingNotice );
	gKSTRDAPI.pRegOnRspQryUserProductUrl( FKSTRDAPI,                    KStrdTraderApiInit::OnRspQryUserProductUrl );
	gKSTRDAPI.pRegOnRspQryMaxCombActionVolume ( FKSTRDAPI,              KStrdTraderApiInit::OnRspQryMaxCombActionVolume );
	gKSTRDAPI.pRegOnRtnKSTradingNotice ( FKSTRDAPI,                    	KStrdTraderApiInit::OnRtnKSTradingNotice );
	gKSTRDAPI.pRegOnRspQryKSInstrumentMarginRate ( FKSTRDAPI,           KStrdTraderApiInit::OnRspQryKSInstrumentMarginRate);
	/*
	gKSTRDAPI.pRegOnRspQryCrossRate( FKSTRDAPI,                         KStrdTraderApiInit::OnRspQryCrossRate );
	gKSTRDAPI.pRegOnRspMatchOrderInsert( FKSTRDAPI,                     KStrdTraderApiInit::OnRspMatchOrderInsert );
	gKSTRDAPI.pRegOnRspQryMatchOrder( FKSTRDAPI,                        KStrdTraderApiInit::OnRspQryMatchOrder );
	gKSTRDAPI.pRegOnRspQryMaxMatchOrderVolume( FKSTRDAPI,               KStrdTraderApiInit::OnRspQryMaxMatchOrderVolume );
	gKSTRDAPI.pRegOnRtnKSTradingNotice( FKSTRDAPI,                      KStrdTraderApiInit::OnRtnKSTradingNotice );
	gKSTRDAPI.pRegOnRtnMatchOrder( FKSTRDAPI,                           KStrdTraderApiInit::OnRtnMatchOrder );
	*/
}
//------------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK TKStrdTraderApi::MessageWndProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	TCTPParams*			Params = (TCTPParams*)lParam;
	TKStrdTraderSpi*	Listener = (TKStrdTraderSpi*)wParam;
	switch ( Message )
	{
	// ---- CThostFtdcTraderSpi ----
	case WM_KSTRD_EVENT    : Listener->OnKStrdFrontConnected( ); break;
	case WM_KSTRD_EVENT+1  : Listener->OnKStrdFrontDisconnected( Params->Param1.AsInt() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+2  : Listener->OnKStrdHeartBeatWarning( Params->Param1.AsInt() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+3  : Listener->OnKStrdRspAuthenticate( (KS_::CThostFtdcRspAuthenticateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+4  : Listener->OnKStrdRspUserLogin( (KS_::CThostFtdcRspUserLoginField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+5  : Listener->OnKStrdRspUserLogout( (KS_::CThostFtdcUserLogoutField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+6  : Listener->OnKStrdRspUserPasswordUpdate( (KS_::CThostFtdcUserPasswordUpdateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+7  : Listener->OnKStrdRspTradingAccountPasswordUpdate( (KS_::CThostFtdcTradingAccountPasswordUpdateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+8  : Listener->OnKStrdRspOrderInsert( (KS_::CThostFtdcInputOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+9  : Listener->OnKStrdRspParkedOrderInsert( (KS_::CThostFtdcParkedOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+10 : Listener->OnKStrdRspParkedOrderAction( (KS_::CThostFtdcParkedOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+11 : Listener->OnKStrdRspOrderAction( (KS_::CThostFtdcInputOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+12 : Listener->OnKStrdRspQueryMaxOrderVolume( (KS_::CThostFtdcQueryMaxOrderVolumeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+13 : Listener->OnKStrdRspSettlementInfoConfirm( (KS_::CThostFtdcSettlementInfoConfirmField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+14 : Listener->OnKStrdRspRemoveParkedOrder( (KS_::CThostFtdcRemoveParkedOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+15 : Listener->OnKStrdRspRemoveParkedOrderAction( (KS_::CThostFtdcRemoveParkedOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+16 : Listener->OnKStrdRspExecOrderInsert( (KS_::CThostFtdcInputExecOrderField*) Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+17 : Listener->OnKStrdRspExecOrderAction( (KS_::CThostFtdcInputExecOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+18 : Listener->OnKStrdRspForQuoteInsert( (KS_::CThostFtdcInputForQuoteField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+19 : Listener->OnKStrdRspQuoteInsert( (KS_::CThostFtdcInputQuoteField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+20 : Listener->OnKStrdRspQuoteAction( (KS_::CThostFtdcInputQuoteActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+21 : Listener->OnKStrdRspQryOrder( (KS_::CThostFtdcOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+22 : Listener->OnKStrdRspQryTrade( (KS_::CThostFtdcTradeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+23 : Listener->OnKStrdRspQryInvestorPosition( (KS_::CThostFtdcInvestorPositionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+24 : Listener->OnKStrdRspQryTradingAccount( (KS_::CThostFtdcTradingAccountField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+25 : Listener->OnKStrdRspQryInvestor( (KS_::CThostFtdcInvestorField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+26 : Listener->OnKStrdRspQryTradingCode( (KS_::CThostFtdcTradingCodeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+27 : Listener->OnKStrdRspQryInstrumentMarginRate( (KS_::CThostFtdcInstrumentMarginRateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+28 : Listener->OnKStrdRspQryInstrumentCommissionRate( (KS_::CThostFtdcInstrumentCommissionRateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+29 : Listener->OnKStrdRspQryExchange( (KS_::CThostFtdcExchangeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+30 : Listener->OnKStrdRspQryProduct( (KS_::CThostFtdcProductField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+31 : Listener->OnKStrdRspQryInstrument( (KS_::CThostFtdcInstrumentField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+32 : Listener->OnKStrdRspQryDepthMarketData( (KS_::CThostFtdcDepthMarketDataField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+33 : Listener->OnKStrdRspQrySettlementInfo( (KS_::CThostFtdcSettlementInfoField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+34 : Listener->OnKStrdRspQryTransferBank( (KS_::CThostFtdcTransferBankField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+35 : Listener->OnKStrdRspQryInvestorPositionDetail( (KS_::CThostFtdcInvestorPositionDetailField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+36 : Listener->OnKStrdRspQryNotice( (KS_::CThostFtdcNoticeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+37 : Listener->OnKStrdRspQrySettlementInfoConfirm( (KS_::CThostFtdcSettlementInfoConfirmField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+38 : Listener->OnKStrdRspQryInvestorPositionCombineDetail( (KS_::CThostFtdcInvestorPositionCombineDetailField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+39 : Listener->OnKStrdRspQryCFMMCTradingAccountKey( (KS_::CThostFtdcCFMMCTradingAccountKeyField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+40 : Listener->OnKStrdRspQryEWarrantOffset( (KS_::CThostFtdcEWarrantOffsetField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+41 : Listener->OnKStrdRspQryInvestorProductGroupMargin( (KS_::CThostFtdcInvestorProductGroupMarginField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+42 : Listener->OnKStrdRspQryExchangeMarginRate( (KS_::CThostFtdcExchangeMarginRateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+43 : Listener->OnKStrdRspQryExchangeMarginRateAdjust( (KS_::CThostFtdcExchangeMarginRateAdjustField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+44 : Listener->OnKStrdRspQryExchangeRate( (KS_::CThostFtdcExchangeRateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+45 : Listener->OnKStrdRspQrySecAgentACIDMap( (KS_::CThostFtdcSecAgentACIDMapField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+46 : Listener->OnKStrdRspQryOptionInstrTradeCost( (KS_::CThostFtdcOptionInstrTradeCostField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+47 : Listener->OnKStrdRspQryOptionInstrCommRate( (KS_::CThostFtdcOptionInstrCommRateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+48 : Listener->OnKStrdRspQryExecOrder( (KS_::CThostFtdcExecOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+49 : Listener->OnKStrdRspQryForQuote( (KS_::CThostFtdcForQuoteField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+50 : Listener->OnKStrdRspQryQuote( (KS_::CThostFtdcQuoteField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+51 : Listener->OnKStrdRspQryTransferSerial( (KS_::CThostFtdcTransferSerialField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+52 : Listener->OnKStrdRspQryAccountregister( (KS_::CThostFtdcAccountregisterField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+53 : Listener->OnKStrdRspError( (KS_::CThostFtdcRspInfoField*)Params->Param1.AsPtr(), Params->Param2.AsInt(), Params->Param3.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+54 : Listener->OnKStrdRtnOrder( (KS_::CThostFtdcOrderField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+55 : Listener->OnKStrdRtnTrade( (KS_::CThostFtdcTradeField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+56 : Listener->OnKStrdErrRtnOrderInsert( (KS_::CThostFtdcInputOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+57 : Listener->OnKStrdErrRtnOrderAction( (KS_::CThostFtdcOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+58 : Listener->OnKStrdRtnInstrumentStatus( (KS_::CThostFtdcInstrumentStatusField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+59 : Listener->OnKStrdRtnTradingNotice( (KS_::CThostFtdcTradingNoticeInfoField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+60 : Listener->OnKStrdRtnErrorConditionalOrder( (KS_::CThostFtdcErrorConditionalOrderField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+61 : Listener->OnKStrdRtnExecOrder( (KS_::CThostFtdcExecOrderField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+62 : Listener->OnKStrdErrRtnExecOrderInsert( (KS_::CThostFtdcInputExecOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+63 : Listener->OnKStrdErrRtnExecOrderAction( (KS_::CThostFtdcExecOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+64 : Listener->OnKStrdErrRtnForQuoteInsert( (KS_::CThostFtdcInputForQuoteField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+65 : Listener->OnKStrdRtnQuote( (KS_::CThostFtdcQuoteField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+66 : Listener->OnKStrdErrRtnQuoteInsert( (KS_::CThostFtdcInputQuoteField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+67 : Listener->OnKStrdErrRtnQuoteAction( (KS_::CThostFtdcQuoteActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+68 : Listener->OnKStrdRtnForQuoteRsp( (KS_::CThostFtdcForQuoteRspField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+69 : Listener->OnKStrdRtnCFMMCTradingAccountToken( (KS_::CThostFtdcCFMMCTradingAccountTokenField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+70 : Listener->OnKStrdRspQryContractBank( (KS_::CThostFtdcContractBankField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+71 : Listener->OnKStrdRspQryParkedOrder( (KS_::CThostFtdcParkedOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+72 : Listener->OnKStrdRspQryParkedOrderAction( (KS_::CThostFtdcParkedOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+73 : Listener->OnKStrdRspQryTradingNotice( (KS_::CThostFtdcTradingNoticeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+74 : Listener->OnKStrdRspQryBrokerTradingParams( (KS_::CThostFtdcBrokerTradingParamsField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+75 : Listener->OnKStrdRspQryBrokerTradingAlgos( (KS_::CThostFtdcBrokerTradingAlgosField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+76 : Listener->OnKStrdRspQueryCFMMCTradingAccountToken( (KS_::CThostFtdcQueryCFMMCTradingAccountTokenField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+77 : Listener->OnKStrdRtnFromBankToFutureByBank( (KS_::CThostFtdcRspTransferField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+78 : Listener->OnKStrdRtnFromFutureToBankByBank( (KS_::CThostFtdcRspTransferField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+79 : Listener->OnKStrdRtnRepealFromBankToFutureByBank( (KS_::CThostFtdcRspRepealField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+80 : Listener->OnKStrdRtnRepealFromFutureToBankByBank( (KS_::CThostFtdcRspRepealField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+81 : Listener->OnKStrdRtnFromBankToFutureByFuture( (KS_::CThostFtdcRspTransferField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+82 : Listener->OnKStrdRtnFromFutureToBankByFuture( (KS_::CThostFtdcRspTransferField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+83 : Listener->OnKStrdRtnRepealFromBankToFutureByFutureManual( (KS_::CThostFtdcRspRepealField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+84 : Listener->OnKStrdRtnRepealFromFutureToBankByFutureManual( (KS_::CThostFtdcRspRepealField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+85 : Listener->OnKStrdRtnQueryBankBalanceByFuture( (KS_::CThostFtdcNotifyQueryAccountField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+86 : Listener->OnKStrdErrRtnBankToFutureByFuture( (KS_::CThostFtdcReqTransferField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+87 : Listener->OnKStrdErrRtnFutureToBankByFuture( (KS_::CThostFtdcReqTransferField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+88 : Listener->OnKStrdErrRtnRepealBankToFutureByFutureManual( (KS_::CThostFtdcReqRepealField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+89 : Listener->OnKStrdErrRtnRepealFutureToBankByFutureManual( (KS_::CThostFtdcReqRepealField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+90 : Listener->OnKStrdErrRtnQueryBankBalanceByFuture( (KS_::CThostFtdcReqQueryAccountField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+91 : Listener->OnKStrdRtnRepealFromBankToFutureByFuture( (KS_::CThostFtdcRspRepealField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+92 : Listener->OnKStrdRtnRepealFromFutureToBankByFuture( (KS_::CThostFtdcRspRepealField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+93 : Listener->OnKStrdRspFromBankToFutureByFuture( (KS_::CThostFtdcReqTransferField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+94 : Listener->OnKStrdRspFromFutureToBankByFuture( (KS_::CThostFtdcReqTransferField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+95 : Listener->OnKStrdRspQueryBankAccountMoneyByFuture( (KS_::CThostFtdcReqQueryAccountField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+96 : Listener->OnKStrdRtnOpenAccountByBank( (KS_::CThostFtdcOpenAccountField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+97 : Listener->OnKStrdRtnCancelAccountByBank( (KS_::CThostFtdcCancelAccountField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+98 : Listener->OnKStrdRtnChangeAccountByBank( (KS_::CThostFtdcChangeAccountField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+99 : Listener->OnKStrdAPIRecoverFinished( (int)lParam );
							return 0;
	// v2.3.50212 // 20150410 by Tim Lin
	case WM_KSTRD_EVENT+100 : Listener->OnKStrdRspLockInsert( (KS_::CThostFtdcInputLockField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );///鎖定應答
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+101 : Listener->OnKStrdRspQryLock( (KS_::CThostFtdcLockField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );///請求查詢鎖定回應
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+102 : Listener->OnKStrdRspQryLockPosition( (KS_::CThostFtdcLockPositionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );///請求查詢鎖定證券倉位回應
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+103 : Listener->OnKStrdRtnLock( (KS_::CThostFtdcLockField*)Params->Param1.AsPtr() );///鎖定通知
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+104 : Listener->OnKStrdErrRtnLockInsert( (KS_::CThostFtdcInputLockField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr() );///鎖定錯誤通知
							delete Params;
							return 0;
	// ---- CKSOptionSpi ----
	// v2.3.50212 // 20150410 by Tim Lin
	case WM_KSTRD_EVENT+121: Listener->OnKStrdRspQryIndexPrice( (KS_::CKSQryIndexPriceField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+122: Listener->OnKStrdRspQryOptionInstrGuard( (KS_::CKSQryOptionInstrGuardField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+123: Listener->OnKStrdRspQryExecOrderVolume( (KS_::CKSQryExecOrderVolumeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+124: Listener->OnKStrdRspQryStockOptionAssignment( (KS_::CKSQryStockOptionAssignmentField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+125: Listener->OnKStrdRspQryInvestorTradeLevel( (KS_::CKSQryInvestorTradeLevelField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+126: Listener->OnKStrdRspQryPurchaseLimitAmt( (KS_::CKSQryPurchaseLimitAmtField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+127: Listener->OnKStrdRspQryPositionLimitVol( (KS_::CKSQryPositionLimitVolField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+128: Listener->OnKStrdRspQryHistoryOrder( (KS_::CKSQryHistoryOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+129: Listener->OnKStrdRspQryHistoryTrade( (KS_::CKSQryHistoryTradeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+130: Listener->OnKStrdRspQryStockOptionHistoryAssignment( (KS_::CKSQryHistoryAssignmentField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+131: Listener->OnKStrdRspQryStockOptionDelivDetail( (KS_::CKSQrySODelivDetailField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+132: Listener->OnKStrdRspAutoExecOrderAction( (KS_::CKSAutoExecOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	/*
	case WM_KSTRD_EVENT+121: Listener->OnKStrdRspQryKSExecOrder( (KS_::CThostFtdcKSExecOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+122: Listener->OnKStrdRspKSExecOrderInsert( (KS_::CThostFtdcInputKSExecOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+123: Listener->OnKStrdRspQryExecOrderVolume( (KS_::CKSExecOrderVolumeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+124: Listener->OnKStrdRspQrySpotOptionParams( (KS_::CKSSpotOptionParamsField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+125: Listener->OnKStrdRspQryOptionInsInfo( (KS_::CKSOptionInsInfoField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+126: Listener->OnKStrdRspQryStockOptionInsCommRate( (KS_::CKSStockOptionInsCommRateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+127: Listener->OnKStrdRspQryStockOptionInsMarginRate( (KS_::CKSStockOptionInsMarginRateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+128: Listener->OnKStrdRspQryStockOptionAssignment( (KS_::CKSStockOptionAssignmentField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+129: Listener->OnKStrdRspQryInvestorTradeLevel( (KS_::CKSInvestorTradeLevelField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+130: Listener->OnKStrdRspQryPurchaseLimitAmt( (KS_::CKSPurchaseLimitAmtField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+131: Listener->OnKStrdRspQryPositionLimitVol( (KS_::CKSPositionLimitVolField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+132: Listener->OnKStrdRspQryHistoryOrder( (KS_::CKSHistoryOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+133: Listener->OnKStrdRspQryHistoryTrade( (KS_::CKSHistoryTradeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+134: Listener->OnKStrdRspQryStockOptionHistoryAssignment( (KS_::CKSHistoryAssignmentField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+135: Listener->OnKStrdRspQryStockOptionDelivDetail( (KS_::CKSSODelivDetailField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+136: Listener->OnKStrdRspAutoExecOrderAction( (KS_::CKSAutoExecOrderActionField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+137: Listener->OnKStrdRtnKSExecOrder( (KS_::CThostFtdcKSExecOrderRtnField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	*/
	// ---- CKSVocSpi ----
	// v2.3.50212 // 20150410 by Tim Lin
	case WM_KSTRD_EVENT+161: Listener->OnKStrdRspQryInvestorOpenPosition( (KS_::CThostFtdcInvestorPositionDetailField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+162: Listener->OnKStrdRspQryInvestorOpenCombinePosition( (KS_::CThostFtdcInvestorPositionCombineDetailField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+163: Listener->OnKStrdRspBulkCancelOrder( (KS_::CThostFtdcBulkCancelOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+164: Listener->OnKStrdRspQryCloseStrategy( (KS_::CKSCloseStrategyResultField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+165: Listener->OnKStrdRspQryCombStrategy( (KS_::CKSCombStrategyResultField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+166: Listener->OnKStrdRspQryOptionCombStrategy( (KS_::CKSOptionCombStrategyResultField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+167: Listener->OnKStrdRspQryTransferInfo( (KS_::CKSTransferInfoResultField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+168: Listener->OnKStrdRspQryKSTradingNotice( (KS_::CKSTradingNoticeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+169: Listener->OnKStrdRspQryUserProductUrl( (KS_::CKSUserProductUrlField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+170: Listener->OnKStrdRspQryMaxCombActionVolume( (KS_::CKSMaxCombActionVolumeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+171: Listener->OnKStrdRtnKSTradingNotice( (KS_::CKSTradingNoticeField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+172: Listener->OnKStrdRspQryKSInstrumentMarginRate( (KS_::CKSInstrumentMarginRateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	/*
	case WM_KSTRD_EVENT+170: Listener->OnKStrdRspQryCrossRate( (KS_::CKSCrossRateField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+171: Listener->OnKStrdRspMatchOrderInsert( (KS_::CKSMatchOrderInsertField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+172: Listener->OnKStrdRspQryMatchOrder( (KS_::CKSMatchOrderField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+173: Listener->OnKStrdRspQryMaxMatchOrderVolume( (KS_::CKSMaxMatchOrderVolumeField*)Params->Param1.AsPtr(), (KS_::CThostFtdcRspInfoField*)Params->Param2.AsPtr(), Params->Param3.AsInt(), Params->Param4.AsBool() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+174: Listener->OnKStrdRtnKSTradingNotice( (KS_::CKSTradingNoticeField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	case WM_KSTRD_EVENT+175: Listener->OnKStrdRtnMatchOrder( (KS_::CKSMatchOrderField*)Params->Param1.AsPtr() );
							delete Params;
							return 0;
	*/
	// -----------------------
	default               : return DefWindowProc( hwnd, Message, wParam, lParam );
	}
	return DefWindowProc( hwnd, Message, wParam, lParam );
}
//------------------------------------------------------------------------------------------------------------------------
bool TKStrdTraderApi::SupportKStrdAPI( void )
{
	return gKSTRDAPI.SupportKStrdAPI( );
}
//------------------------------------------------------------------------------------------------------------------------
const char* TKStrdTraderApi::LoadKStrdAPIResult( void )
{
	return gKSTRDAPI.Reason( );
}
//------------------------------------------------------------------------------------------------------------------------
void TKStrdTraderApi::RegisterSpi( TKStrdTraderSpi *pSpi )
{
	FListener = pSpi;
}
//------------------------------------------------------------------------------------------------------------------------
void TKStrdTraderApi::ReqOnRecoverFilisned( int Count )
{
	TKStrdTraderSpi*	Spi  = GetSpi( );
	HWND				Hwnd = GetHWND( );

	PostMessage( Hwnd, WM_KSTRD_EVENT+99, (WPARAM)Spi, (LPARAM)Count );
}
//------------------------------------------------------------------------------------------------------------------------
// ---- CThostFtdcTraderApi ----
//------------------------------------------------------------------------------------------------------------------------
void TKStrdTraderApi::Init()
{
	gKSTRDAPI.pInit( FKSTRDAPI );
}
//------------------------------------------------------------------------------------------------------------------------
const char* TKStrdTraderApi::GetTradingDay()
{
	return gKSTRDAPI.pGetTradingDay( FKSTRDAPI );
}
//------------------------------------------------------------------------------------------------------------------------
void TKStrdTraderApi::RegisterFront( char *pszFrontAddress )
{
	gKSTRDAPI.pRegisterFront( FKSTRDAPI, pszFrontAddress );
}
//------------------------------------------------------------------------------------------------------------------------
void TKStrdTraderApi::RegisterNameServer( char *pszNsAddress )
{
	gKSTRDAPI.pRegisterNameServer( FKSTRDAPI, pszNsAddress );
}
//------------------------------------------------------------------------------------------------------------------------
void TKStrdTraderApi::RegisterFensUserInfo( KS_::CThostFtdcFensUserInfoField * pFensUserInfo )
{
	gKSTRDAPI.pRegisterFensUserInfo( FKSTRDAPI, pFensUserInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void TKStrdTraderApi::SubscribePrivateTopic( KS_::THOST_TE_RESUME_TYPE nResumeType )
{
	gKSTRDAPI.pSubscribePrivateTopic( FKSTRDAPI, nResumeType );
}
//------------------------------------------------------------------------------------------------------------------------
void TKStrdTraderApi::SubscribePublicTopic( KS_::THOST_TE_RESUME_TYPE nResumeType )
{
	gKSTRDAPI.pSubscribePublicTopic( FKSTRDAPI, nResumeType );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqAuthenticate( KS_::CThostFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID )
{
	return gKSTRDAPI.pReqAuthenticate( FKSTRDAPI, pReqAuthenticateField, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqUserLogin( KS_::CThostFtdcReqUserLoginField *pReqUserLoginField, int nRequestID )
{
	return gKSTRDAPI.pReqUserLogin( FKSTRDAPI, pReqUserLoginField, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqUserLogout( KS_::CThostFtdcUserLogoutField *pUserLogout, int nRequestID )
{
	return gKSTRDAPI.pReqUserLogout( FKSTRDAPI, pUserLogout, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqUserPasswordUpdate( KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID )
{
	return gKSTRDAPI.pReqUserPasswordUpdate( FKSTRDAPI, pUserPasswordUpdate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqTradingAccountPasswordUpdate( KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, int nRequestID )
{
	return gKSTRDAPI.pReqTradingAccountPasswordUpdate( FKSTRDAPI, pTradingAccountPasswordUpdate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, int nRequestID )
{
	return gKSTRDAPI.pReqOrderInsert( FKSTRDAPI, pInputOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqParkedOrderInsert( KS_::CThostFtdcParkedOrderField *pParkedOrder, int nRequestID )
{
	return gKSTRDAPI.pReqParkedOrderInsert( FKSTRDAPI, pParkedOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqParkedOrderAction( KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, int nRequestID )
{
	return gKSTRDAPI.pReqParkedOrderAction( FKSTRDAPI, pParkedOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqOrderAction( KS_::CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID )
{
	return gKSTRDAPI.pReqOrderAction( FKSTRDAPI, pInputOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQueryMaxOrderVolume( KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, int nRequestID )
{
	return gKSTRDAPI.pReqQueryMaxOrderVolume( FKSTRDAPI, pQueryMaxOrderVolume, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqSettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, int nRequestID )
{
	return gKSTRDAPI.pReqSettlementInfoConfirm( FKSTRDAPI, pSettlementInfoConfirm, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqRemoveParkedOrder( KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, int nRequestID )
{
	return gKSTRDAPI.pReqRemoveParkedOrder( FKSTRDAPI, pRemoveParkedOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqRemoveParkedOrderAction( KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, int nRequestID )
{
	return gKSTRDAPI.pReqRemoveParkedOrderAction( FKSTRDAPI, pRemoveParkedOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqExecOrderInsert( KS_::CThostFtdcInputExecOrderField *pInputExecOrder, int nRequestID )
{
	return gKSTRDAPI.pReqExecOrderInsert( FKSTRDAPI, pInputExecOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqExecOrderAction( KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, int nRequestID )
{
	return gKSTRDAPI.pReqExecOrderAction( FKSTRDAPI, pInputExecOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqForQuoteInsert( KS_::CThostFtdcInputForQuoteField *pInputForQuote, int nRequestID )
{
	return gKSTRDAPI.pReqForQuoteInsert( FKSTRDAPI, pInputForQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQuoteInsert( KS_::CThostFtdcInputQuoteField *pInputQuote, int nRequestID )
{
	return gKSTRDAPI.pReqQuoteInsert( FKSTRDAPI, pInputQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQuoteAction( KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, int nRequestID )
{
	return gKSTRDAPI.pReqQuoteAction( FKSTRDAPI, pInputQuoteAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryOrder( KS_::CThostFtdcQryOrderField *pQryOrder, int nRequestID )
{
	return gKSTRDAPI.pReqQryOrder( FKSTRDAPI, pQryOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryTrade( KS_::CThostFtdcQryTradeField *pQryTrade, int nRequestID )
{
	return gKSTRDAPI.pReqQryTrade( FKSTRDAPI, pQryTrade, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryInvestorPosition( KS_::CThostFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID )
{
	return gKSTRDAPI.pReqQryInvestorPosition( FKSTRDAPI, pQryInvestorPosition, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryTradingAccount( KS_::CThostFtdcQryTradingAccountField *pQryTradingAccount, int nRequestID )
{
	return gKSTRDAPI.pReqQryTradingAccount( FKSTRDAPI, pQryTradingAccount, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryInvestor( KS_::CThostFtdcQryInvestorField *pQryInvestor, int nRequestID )
{
	return gKSTRDAPI.pReqQryInvestor( FKSTRDAPI, pQryInvestor, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryTradingCode( KS_::CThostFtdcQryTradingCodeField *pQryTradingCode, int nRequestID )
{
	return gKSTRDAPI.pReqQryTradingCode( FKSTRDAPI, pQryTradingCode, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryInstrumentMarginRate( KS_::CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID )
{
	return gKSTRDAPI.pReqQryInstrumentMarginRate( FKSTRDAPI, pQryInstrumentMarginRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryInstrumentCommissionRate( KS_::CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate, int nRequestID )
{
	return gKSTRDAPI.pReqQryInstrumentCommissionRate( FKSTRDAPI, pQryInstrumentCommissionRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryExchange( KS_::CThostFtdcQryExchangeField *pQryExchange, int nRequestID )
{
	return gKSTRDAPI.pReqQryExchange( FKSTRDAPI, pQryExchange, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryProduct( KS_::CThostFtdcQryProductField *pQryProduct, int nRequestID )
{
	return gKSTRDAPI.pReqQryProduct( FKSTRDAPI, pQryProduct, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryInstrument( KS_::CThostFtdcQryInstrumentField *pQryInstrument, int nRequestID )
{
	return gKSTRDAPI.pReqQryInstrument( FKSTRDAPI, pQryInstrument, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryDepthMarketData( KS_::CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, int nRequestID )
{
	return gKSTRDAPI.pReqQryDepthMarketData( FKSTRDAPI, pQryDepthMarketData, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQrySettlementInfo( KS_::CThostFtdcQrySettlementInfoField *pQrySettlementInfo, int nRequestID )
{
	return gKSTRDAPI.pReqQrySettlementInfo( FKSTRDAPI, pQrySettlementInfo, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryTransferBank( KS_::CThostFtdcQryTransferBankField *pQryTransferBank, int nRequestID )
{
	return gKSTRDAPI.pReqQryTransferBank( FKSTRDAPI, pQryTransferBank, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryInvestorPositionDetail( KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, int nRequestID )
{
	return gKSTRDAPI.pReqQryInvestorPositionDetail( FKSTRDAPI, pQryInvestorPositionDetail, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryNotice( KS_::CThostFtdcQryNoticeField *pQryNotice, int nRequestID )
{
	return gKSTRDAPI.pReqQryNotice( FKSTRDAPI, pQryNotice, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQrySettlementInfoConfirm( KS_::CThostFtdcQrySettlementInfoConfirmField *pQrySettlementInfoConfirm, int nRequestID )
{
	return gKSTRDAPI.pReqQrySettlementInfoConfirm( FKSTRDAPI, pQrySettlementInfoConfirm, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryInvestorPositionCombineDetail( KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorPositionCombineDetail, int nRequestID )
{
	return gKSTRDAPI.pReqQryInvestorPositionCombineDetail( FKSTRDAPI, pQryInvestorPositionCombineDetail, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryCFMMCTradingAccountKey( KS_::CThostFtdcQryCFMMCTradingAccountKeyField *pQryCFMMCTradingAccountKey, int nRequestID )
{
	return gKSTRDAPI.pReqQryCFMMCTradingAccountKey( FKSTRDAPI, pQryCFMMCTradingAccountKey, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryEWarrantOffset( KS_::CThostFtdcQryEWarrantOffsetField *pQryEWarrantOffset, int nRequestID )
{
	return gKSTRDAPI.pReqQryEWarrantOffset( FKSTRDAPI, pQryEWarrantOffset, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryInvestorProductGroupMargin( KS_::CThostFtdcQryInvestorProductGroupMarginField *pQryInvestorProductGroupMargin, int nRequestID )
{
	return gKSTRDAPI.pReqQryInvestorProductGroupMargin( FKSTRDAPI, pQryInvestorProductGroupMargin, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryExchangeMarginRate( KS_::CThostFtdcQryExchangeMarginRateField *pQryExchangeMarginRate, int nRequestID)
{
	return gKSTRDAPI.pReqQryExchangeMarginRate( FKSTRDAPI, pQryExchangeMarginRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryExchangeMarginRateAdjust( KS_::CThostFtdcQryExchangeMarginRateAdjustField *pQryExchangeMarginRateAdjust, int nRequestID )
{
	return gKSTRDAPI.pReqQryExchangeMarginRateAdjust( FKSTRDAPI, pQryExchangeMarginRateAdjust, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryExchangeRate( KS_::CThostFtdcQryExchangeRateField *pQryExchangeRate, int nRequestID )
{
	return gKSTRDAPI.pReqQryExchangeRate( FKSTRDAPI, pQryExchangeRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQrySecAgentACIDMap( KS_::CThostFtdcQrySecAgentACIDMapField *pQrySecAgentACIDMap, int nRequestID )
{
	return gKSTRDAPI.pReqQrySecAgentACIDMap( FKSTRDAPI, pQrySecAgentACIDMap, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryOptionInstrTradeCost( KS_::CThostFtdcQryOptionInstrTradeCostField *pQryOptionInstrTradeCost, int nRequestID )
{
	return gKSTRDAPI.pReqQryOptionInstrTradeCost( FKSTRDAPI, pQryOptionInstrTradeCost, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryOptionInstrCommRate( KS_::CThostFtdcQryOptionInstrCommRateField *pQryOptionInstrCommRate, int nRequestID )
{
	return gKSTRDAPI.pReqQryOptionInstrCommRate( FKSTRDAPI, pQryOptionInstrCommRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryExecOrder( KS_::CThostFtdcQryExecOrderField *pQryExecOrder, int nRequestID )
{
	return gKSTRDAPI.pReqQryExecOrder( FKSTRDAPI, pQryExecOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryForQuote( KS_::CThostFtdcQryForQuoteField *pQryForQuote, int nRequestID )
{
	return gKSTRDAPI.pReqQryForQuote( FKSTRDAPI, pQryForQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryQuote( KS_::CThostFtdcQryQuoteField *pQryQuote, int nRequestID )
{
	return gKSTRDAPI.pReqQryQuote( FKSTRDAPI, pQryQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryTransferSerial( KS_::CThostFtdcQryTransferSerialField *pQryTransferSerial, int nRequestID )
{
	return gKSTRDAPI.pReqQryTransferSerial( FKSTRDAPI, pQryTransferSerial, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryAccountregister( KS_::CThostFtdcQryAccountregisterField *pQryAccountregister, int nRequestID )
{
	return gKSTRDAPI.pReqQryAccountregister( FKSTRDAPI, pQryAccountregister, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryContractBank( KS_::CThostFtdcQryContractBankField *pQryContractBank, int nRequestID )
{
	return gKSTRDAPI.pReqQryContractBank( FKSTRDAPI, pQryContractBank, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryParkedOrder( KS_::CThostFtdcQryParkedOrderField *pQryParkedOrder, int nRequestID )
{
	return gKSTRDAPI.pReqQryParkedOrder( FKSTRDAPI, pQryParkedOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryParkedOrderAction( KS_::CThostFtdcQryParkedOrderActionField *pQryParkedOrderAction, int nRequestID )
{
	return gKSTRDAPI.pReqQryParkedOrderAction( FKSTRDAPI, pQryParkedOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryTradingNotice( KS_::CThostFtdcQryTradingNoticeField *pQryTradingNotice, int nRequestID )
{
	return gKSTRDAPI.pReqQryTradingNotice( FKSTRDAPI, pQryTradingNotice, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryBrokerTradingParams( KS_::CThostFtdcQryBrokerTradingParamsField *pQryBrokerTradingParams, int nRequestID )
{
	return gKSTRDAPI.pReqQryBrokerTradingParams( FKSTRDAPI, pQryBrokerTradingParams, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryBrokerTradingAlgos( KS_::CThostFtdcQryBrokerTradingAlgosField *pQryBrokerTradingAlgos, int nRequestID )
{
	return gKSTRDAPI.pReqQryBrokerTradingAlgos( FKSTRDAPI, pQryBrokerTradingAlgos, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQueryCFMMCTradingAccountToken( KS_::CThostFtdcQueryCFMMCTradingAccountTokenField *pQueryCFMMCTradingAccountToken, int nRequestID )
{
	return gKSTRDAPI.pReqQueryCFMMCTradingAccountToken( FKSTRDAPI, pQueryCFMMCTradingAccountToken, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqFromBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID )
{
	return gKSTRDAPI.pReqFromBankToFutureByFuture( FKSTRDAPI, pReqTransfer, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqFromFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID )
{
	return gKSTRDAPI.pReqFromFutureToBankByFuture( FKSTRDAPI, pReqTransfer, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQueryBankAccountMoneyByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, int nRequestID )
{
	return gKSTRDAPI.pReqQueryBankAccountMoneyByFuture( FKSTRDAPI, pReqQueryAccount, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
// v2.3.50212 // 20150410 by Tim Lin
int TKStrdTraderApi::ReqLockInsert( KS_::CThostFtdcInputLockField *pInputLock, int nRequestID)///用戶端發出鎖定請求
{
	return gKSTRDAPI.pReqLockInsert( FKSTRDAPI, pInputLock, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryLock( KS_::CThostFtdcQryLockField *pQryLock, int nRequestID)///用戶端發出查詢鎖定請求
{
	return gKSTRDAPI.pReqQryLock( FKSTRDAPI, pQryLock, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryLockPosition( KS_::CThostFtdcQryLockPositionField *pQryLockPosition, int nRequestID)///用戶端發出查詢鎖定證券倉位請求
{
	return gKSTRDAPI.pReqQryLockPosition( FKSTRDAPI, pQryLockPosition, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
// ---- CKSOptionApi ----
// v2.3.50212 // 20150410 by Tim Lin
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryIndexPrice( KS_::CKSQryIndexPriceField *pQryIndexPrice, int nRequestID )///查詢股指現貨指數
{
	return gKSTRDAPI.pReqQryIndexPrice( FKSTRDAPI, pQryIndexPrice, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryOptionInstrGuard( KS_::CKSQryOptionInstrGuardField *pQryOptionInstrGuard, int nRequestID )///查詢期權合約保障系數
{
	return gKSTRDAPI.pReqQryOptionInstrGuard( FKSTRDAPI, pQryOptionInstrGuard, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryExecOrderVolume( KS_::CKSQryExecOrderVolumeField *pQryExecOrderVolume, int nRequestID )///查詢宣告數量請求
{
	return gKSTRDAPI.pReqQryExecOrderVolume( FKSTRDAPI, pQryExecOrderVolume, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryStockOptionAssignment( KS_::CKSQryStockOptionAssignmentField *pQryStockOptionAssignment, int nRequestID )///查詢個股行權指派信息
{
	return gKSTRDAPI.pReqQryStockOptionAssignment( FKSTRDAPI, pQryStockOptionAssignment, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryInvestorTradeLevel( KS_::CKSQryInvestorTradeLevelField *pQryInvestorTradeLevel, int nRequestID )///查詢客戶交易級別
{
	return gKSTRDAPI.pReqQryInvestorTradeLevel( FKSTRDAPI, pQryInvestorTradeLevel, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryPurchaseLimitAmt( KS_::CKSQryPurchaseLimitAmtField *pQryPurchaseLimitAmt, int nRequestID )///查詢個股限購額度
{
	return gKSTRDAPI.pReqQryPurchaseLimitAmt( FKSTRDAPI, pQryPurchaseLimitAmt, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryPositionLimitVol( KS_::CKSQryPositionLimitVolField *pQryPositionLimitVol, int nRequestID )///查詢個股限倉額度
{
	return gKSTRDAPI.pReqQryPositionLimitVol( FKSTRDAPI, pQryPositionLimitVol, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryHistoryOrder( KS_::CKSQryHistoryOrderField *pQryHistoryOrder, int nRequestID )///請求查詢個股歷史報單
{
	return gKSTRDAPI.pReqQryHistoryOrder( FKSTRDAPI, pQryHistoryOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryHistoryTrade( KS_::CKSQryHistoryTradeField *pQryHistoryTrade, int nRequestID )///請求查詢個股歷史成交
{
	return gKSTRDAPI.pReqQryHistoryTrade( FKSTRDAPI, pQryHistoryTrade, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryStockOptionHistoryAssignment( KS_::CKSQryHistoryAssignmentField *pQryHistoryAssignment, int nRequestID )///請求查詢個股歷史行權指派明細
{
	return gKSTRDAPI.pReqQryHistoryAssignment( FKSTRDAPI, pQryHistoryAssignment, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryStockOptionDelivDetail( KS_::CKSQrySODelivDetailField *pQrySODelivDetail, int nRequestID )///請求查詢個股行權交割明細
{
	return gKSTRDAPI.pReqQrySODelivDetail( FKSTRDAPI, pQrySODelivDetail, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqAutoExecOrderAction( KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, int nRequestID )///自動行權執行操作
{
	return gKSTRDAPI.pReqAutoExecOrderAction( FKSTRDAPI, pAutoExecOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
/*
int TKStrdTraderApi::ReqQryKSExecOrder( KS_::CThostFtdcQryKSExecOrderField *pQryKSExecOrder, int nRequestID )
{
	return gKSTRDAPI.pReqQryKSExecOrder( FKSTRDAPI, pQryKSExecOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqKSExecOrderInsert( KS_::CThostFtdcInputKSExecOrderField *pInputKSExecOrder, int nRequestID )
{
	return gKSTRDAPI.pReqKSExecOrderInsert( FKSTRDAPI, pInputKSExecOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryExecOrderVolume( KS_::CKSQryExecOrderVolumeField *pQryExecOrderVolume, int nRequestID )
{
	return gKSTRDAPI.pReqQryExecOrderVolume( FKSTRDAPI, pQryExecOrderVolume, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQrySpotOptionParams( KS_::CKSQrySpotOptionParamsField *pQrySpotOptionParams, int nRequestID )
{
	return gKSTRDAPI.pReqQrySpotOptionParams( FKSTRDAPI, pQrySpotOptionParams, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryOptionInsInfo( KS_::CKSQryOptionInsInfoField *pQryOptionInsInfo, int nRequestID )
{
	return gKSTRDAPI.pReqQryOptionInsInfo( FKSTRDAPI, pQryOptionInsInfo, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryStockOptionInsCommRate( KS_::CKSQryStockOptionInsCommRateField *pQryStockOptionInsCommRate, int nRequestID )
{
	return gKSTRDAPI.pReqQryStockOptionInsCommRate( FKSTRDAPI, pQryStockOptionInsCommRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryStockOptionInsMarginRate( KS_::CKSQryStockOptionInsMarginRateField *pQryStockOptionInsMarginRate, int nRequestID )
{
	return gKSTRDAPI.pReqQryStockOptionInsMarginRate( FKSTRDAPI, pQryStockOptionInsMarginRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryStockOptionAssignment( KS_::CKSQryStockOptionAssignmentField *pQryStockOptionAssignment, int nRequestID )
{
	return gKSTRDAPI.pReqQryStockOptionAssignment( FKSTRDAPI, pQryStockOptionAssignment, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryInvestorTradeLevel( KS_::CKSQryInvestorTradeLevelField *pQryInvestorTradeLevel, int nRequestID )
{
	return gKSTRDAPI.pReqQryInvestorTradeLevel( FKSTRDAPI, pQryInvestorTradeLevel, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryPurchaseLimitAmt( KS_::CKSQryPurchaseLimitAmtField *pQryPurchaseLimitAmt, int nRequestID )
{
	return gKSTRDAPI.pReqQryPurchaseLimitAmt( FKSTRDAPI, pQryPurchaseLimitAmt, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryPositionLimitVol( KS_::CKSQryPositionLimitVolField *pQryPositionLimitVol, int nRequestID )
{
	return gKSTRDAPI.pReqQryPositionLimitVol( FKSTRDAPI, pQryPositionLimitVol, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryHistoryOrder( KS_::CKSQryHistoryOrderField *pQryHistoryOrder, int nRequestID )
{
	return gKSTRDAPI.pReqQryHistoryOrder( FKSTRDAPI, pQryHistoryOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryHistoryTrade( KS_::CKSQryHistoryTradeField *pQryHistoryTrade, int nRequestID )
{
	return gKSTRDAPI.pReqQryHistoryTrade( FKSTRDAPI, pQryHistoryTrade, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryStockOptionHistoryAssignment( KS_::CKSQryHistoryAssignmentField *pQryHistoryAssignment, int nRequestID )
{
	return gKSTRDAPI.pReqQryStockOptionHistoryAssignment( FKSTRDAPI, pQryHistoryAssignment, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryStockOptionDelivDetail( KS_::CKSQrySODelivDetailField *pQrySODelivDetail, int nRequestID )
{
	return gKSTRDAPI.pReqQryStockOptionDelivDetail( FKSTRDAPI, pQrySODelivDetail, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqAutoExecOrderAction( KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, int nRequestID )
{
	return gKSTRDAPI.pReqAutoExecOrderAction( FKSTRDAPI, pAutoExecOrderAction, nRequestID );
}
*/
//------------------------------------------------------------------------------------------------------------------------
// ---- CKSVocApi ----
// v2.3.50212 // 20150410 by Tim Lin
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQueryInvestorOpenPosition( KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorOpenPosition, int nRequestID )///查詢開盤前的持倉明細
{
	return gKSTRDAPI.pReqQueryInvestorOpenPosition( FKSTRDAPI, pQryInvestorOpenPosition, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQueryInvestorOpenCombinePosition( KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorOpenCombinePosition, int nRequestID )///查詢開盤前的組合持倉明細
{
	return gKSTRDAPI.pReqQueryInvestorOpenCombinePosition( FKSTRDAPI, pQryInvestorOpenCombinePosition, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqBulkCancelOrder( KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, int nRequestID )///批量撤單
{
	return gKSTRDAPI.pReqBulkCancelOrder( FKSTRDAPI, pBulkCancelOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryCloseStrategy( KS_::CKSCloseStrategy *pCloseStrategy, int nRequestID )///平倉策略查詢請求
{
	return gKSTRDAPI.pReqQryCloseStrategy( FKSTRDAPI, pCloseStrategy, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryCombStrategy( KS_::CKSCombStrategy *pCombStrategy, int nRequestID )///組合策略查詢請求
{
	return gKSTRDAPI.pReqQryCombStrategy( FKSTRDAPI, pCombStrategy, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryOptionCombStrategy( KS_::CKSOptionCombStrategy *pOptionCombStrategy, int nRequestID )///期權組合策略查詢請求
{
	return gKSTRDAPI.pReqQryOptionCombStrategy( FKSTRDAPI, pOptionCombStrategy, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryTransferInfo( KS_::CKSTransferInfo *pTransferInfo, int nRequestID )///請求查詢客戶轉帳資訊
{
	return gKSTRDAPI.pReqQryTransferInfo( FKSTRDAPI, pTransferInfo, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryKSTradingNotice( KS_::CKSQryTradingNoticeField *pQryTradingNotice, int nRequestID )///請求查詢交易通知
{
	return gKSTRDAPI.pReqQryKSTradingNotice( FKSTRDAPI, pQryTradingNotice, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryUserProductUrl( KS_::CKSQryUserProductUrlField *pQryUserProductUrl, int nRequestID )///使用者端產品資源查詢請求
{
	return gKSTRDAPI.pReqQryUserProductUrl( FKSTRDAPI, pQryUserProductUrl, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryMaxCombActionVolume( KS_::CKSQryMaxCombActionVolumeField *pQryMaxCombActionVolume, int nRequestID )///最大組合拆分單量查詢請求
{
	return gKSTRDAPI.pReqQryMaxCombActionVolume( FKSTRDAPI, pQryMaxCombActionVolume, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryKSInstrumentMarginRate( KS_::CKSQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID)///請求查詢合約保證金率
{
	return gKSTRDAPI.pReqQryKSInstrumentMarginRate( FKSTRDAPI, pQryInstrumentMarginRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
/*
int TKStrdTraderApi::ReqQryCrossRate( KS_::CKSQryCrossRateField *pQryCrossRate, int nRequestID )
{
	return gKSTRDAPI.pReqQryCrossRate( FKSTRDAPI, pQryCrossRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqMatchOrderInsert( KS_::CKSMatchOrderInsertField *pMatchOrderInsert, int nRequestID )
{
	return gKSTRDAPI.pReqMatchOrderInsert( FKSTRDAPI, pMatchOrderInsert, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryMatchOrder( KS_::CKSQryMatchOrderField *pQryMatchOrder, int nRequestID )
{
	return gKSTRDAPI.pReqQryMatchOrder( FKSTRDAPI, pQryMatchOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int TKStrdTraderApi::ReqQryMaxMatchOrderVolume( KS_::CKSQryMaxMatchOrderVolumeField *pQryMaxMatchOrderVolume, int nRequestID )
{
	return gKSTRDAPI.pReqQryMaxMatchOrderVolume( FKSTRDAPI, pQryMaxMatchOrderVolume, nRequestID );
}
*/
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
