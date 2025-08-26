//---------------------------------------------------------------------------
#ifdef _CTP
//------------------------------------------------------------------------------------------------------------------------
#ifndef _KSFT
//------------------------------------------------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "KStrdBCBTraderAPI.h"
#include "KStrdBCBEvents.h"
#include "KStrdAPIDynamic.h"
#include "APIParams.h"
//------------------------------------------------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------------------------------------------------
extern HINSTANCE gKStrdDLLInstance;
//------------------------------------------------------------------------------------------------------------------------
bool TriggerKStrdEvent( HKSTRD Owner, int MSG, TCTPParams* Params )
{
	TKStrdTraderApi* KStrdObj = (TKStrdTraderApi*)Owner;
	if ( KStrdObj == NULL )
		return false;
	TKStrdTraderSpi* Listener = KStrdObj->GetSpi( );
	if ( Listener == NULL )
		return false;

	PostMessage( KStrdObj->GetHWND(), MSG, (WPARAM)Listener, (LPARAM)Params );
	return true;
}
//------------------------------------------------------------------------------------------------------------------------
KStrdTraderApiInit::KStrdTraderApiInit()
{
	// 1. Load KStrdAPIFactory.dll
	gKStrdDLLInstance = ::LoadLibraryW( L"KStrdAPIFactory.dll" );
	if ( gKStrdDLLInstance == NULL )
	{
		sprintf( FNotSupportReason, "Can't dynamic load KStrdAPIFactory.dll" );
		return;
	}
	// 2. Bind functions
	if ( ( FFuncRtn = InitFunctions() ) != 0 )
	{
		sprintf( FNotSupportReason, "Dynamic bind function %d failed.", FFuncRtn );
		return;
	}
	// 3. Bind regist callback functions
	if ( ( FRegCBRtn = InitRegCallbackFunctions() ) != 0 )
	{
		sprintf( FNotSupportReason, "Dynamic bind regist callback function %d failed.", FRegCBRtn );
		return;
	}
	sprintf( FNotSupportReason, "Load KStrdAPIFactory.dll OK!" );
	FSupport = true;
}
//------------------------------------------------------------------------------------------------------------------------
KStrdTraderApiInit::~KStrdTraderApiInit()
{
}
//------------------------------------------------------------------------------------------------------------------------
int KStrdTraderApiInit::InitFunctions( void )
{
	// ---- CThostFtdcTraderApi ----
	if ( ( pCreateKStrdTraderApi                = (lpCreateKStrdTraderApi                )::GetProcAddress( gKStrdDLLInstance, "CreateKStrdTraderApi" ) ) == NULL )
		return 1;
	if ( ( pReleaseKStrdTraderApi               = (lpReleaseKStrdTraderApi               )::GetProcAddress( gKStrdDLLInstance, "ReleaseKStrdTraderApi" ) ) == NULL )
		return 2;
	if ( ( pInit                                = (lpInit                                )::GetProcAddress( gKStrdDLLInstance, "Init" ) ) == NULL )
		return 3;
	if ( ( pGetTradingDay                       = (lpGetTradingDay                       )::GetProcAddress( gKStrdDLLInstance, "GetTradingDay" ) ) == NULL )
		return 4;
	if ( ( pRegisterFront                       = (lpRegisterFront                       )::GetProcAddress( gKStrdDLLInstance, "RegisterFront" ) ) == NULL )
		return 5;
	if ( ( pRegisterNameServer                  = (lpRegisterNameServer                  )::GetProcAddress( gKStrdDLLInstance, "RegisterNameServer" ) ) == NULL )
		return 6;
	if ( ( pRegisterFensUserInfo                = (lpRegisterFensUserInfo                )::GetProcAddress( gKStrdDLLInstance, "RegisterFensUserInfo" ) ) == NULL )
		return 7;
	if ( ( pSubscribePrivateTopic               = (lpSubscribePrivateTopic               )::GetProcAddress( gKStrdDLLInstance, "SubscribePrivateTopic" ) ) == NULL )
		return 8;
	if ( ( pSubscribePublicTopic                = (lpSubscribePublicTopic                )::GetProcAddress( gKStrdDLLInstance, "SubscribePublicTopic" ) ) == NULL )
		return 9;
	if ( ( pReqAuthenticate                     = (lpReqAuthenticate                     )::GetProcAddress( gKStrdDLLInstance, "ReqAuthenticate" ) ) == NULL )
		return 10;
	if ( ( pReqUserLogin                        = (lpReqUserLogin                        )::GetProcAddress( gKStrdDLLInstance, "ReqUserLogin" ) ) == NULL )
		return 11;
	if ( ( pReqUserLogout                       = (lpReqUserLogout                       )::GetProcAddress( gKStrdDLLInstance, "ReqUserLogout" ) ) == NULL )
		return 12;
	if ( ( pReqUserPasswordUpdate               = (lpReqUserPasswordUpdate               )::GetProcAddress( gKStrdDLLInstance, "ReqUserPasswordUpdate" ) ) == NULL )
		return 13;
	if ( ( pReqTradingAccountPasswordUpdate     = (lpReqTradingAccountPasswordUpdate     )::GetProcAddress( gKStrdDLLInstance, "ReqTradingAccountPasswordUpdate" ) ) == NULL )
		return 14;
	if ( ( pReqOrderInsert                      = (lpReqOrderInsert                      )::GetProcAddress( gKStrdDLLInstance, "ReqOrderInsert" ) ) == NULL )
		return 15;
	if ( ( pReqParkedOrderInsert                = (lpReqParkedOrderInsert                )::GetProcAddress( gKStrdDLLInstance, "ReqParkedOrderInsert" ) ) == NULL )
		return 16;
	if ( ( pReqParkedOrderAction                = (lpReqParkedOrderAction                )::GetProcAddress( gKStrdDLLInstance, "ReqParkedOrderAction" ) ) == NULL )
		return 17;
	if ( ( pReqOrderAction                      = (lpReqOrderAction                      )::GetProcAddress( gKStrdDLLInstance, "ReqOrderAction" ) ) == NULL )
		return 18;
	if ( ( pReqQueryMaxOrderVolume              = (lpReqQueryMaxOrderVolume              )::GetProcAddress( gKStrdDLLInstance, "ReqQueryMaxOrderVolume" ) ) == NULL )
		return 19;
	if ( ( pReqSettlementInfoConfirm            = (lpReqSettlementInfoConfirm            )::GetProcAddress( gKStrdDLLInstance, "ReqSettlementInfoConfirm" ) ) == NULL )
		return 20;
	if ( ( pReqRemoveParkedOrder                = (lpReqRemoveParkedOrder                )::GetProcAddress( gKStrdDLLInstance, "ReqRemoveParkedOrder" ) ) == NULL )
		return 21;
	if ( ( pReqRemoveParkedOrderAction          = (lpReqRemoveParkedOrderAction          )::GetProcAddress( gKStrdDLLInstance, "ReqRemoveParkedOrderAction" ) ) == NULL )
		return 22;
	if ( ( pReqExecOrderInsert                  = (lpReqExecOrderInsert                  )::GetProcAddress( gKStrdDLLInstance, "ReqExecOrderInsert" ) ) == NULL )
		return 23;
	if ( ( pReqExecOrderAction                  = (lpReqExecOrderAction                  )::GetProcAddress( gKStrdDLLInstance, "ReqExecOrderAction" ) ) == NULL )
		return 24;
	if ( ( pReqForQuoteInsert                   = (lpReqForQuoteInsert                   )::GetProcAddress( gKStrdDLLInstance, "ReqForQuoteInsert" ) ) == NULL )
		return 25;
	if ( ( pReqQuoteInsert                      = (lpReqQuoteInsert                      )::GetProcAddress( gKStrdDLLInstance, "ReqQuoteInsert" ) ) == NULL )
		return 26;
	if ( ( pReqQuoteAction                      = (lpReqQuoteAction                      )::GetProcAddress( gKStrdDLLInstance, "ReqQuoteAction" ) ) == NULL )
		return 27;
	if ( ( pReqQryOrder                         = (lpReqQryOrder                         )::GetProcAddress( gKStrdDLLInstance, "ReqQryOrder" ) ) == NULL )
		return 28;
	if ( ( pReqQryTrade                         = (lpReqQryTrade                         )::GetProcAddress( gKStrdDLLInstance, "ReqQryTrade" ) ) == NULL )
		return 29;
	if ( ( pReqQryInvestorPosition              = (lpReqQryInvestorPosition              )::GetProcAddress( gKStrdDLLInstance, "ReqQryInvestorPosition" ) ) == NULL )
		return 30;
	if ( ( pReqQryTradingAccount                = (lpReqQryTradingAccount                )::GetProcAddress( gKStrdDLLInstance, "ReqQryTradingAccount" ) ) == NULL )
		return 31;
	if ( ( pReqQryInvestor                      = (lpReqQryInvestor                      )::GetProcAddress( gKStrdDLLInstance, "ReqQryInvestor" ) ) == NULL )
		return 32;
	if ( ( pReqQryTradingCode                   = (lpReqQryTradingCode                   )::GetProcAddress( gKStrdDLLInstance, "ReqQryTradingCode" ) ) == NULL )
		return 33;
	if ( ( pReqQryInstrumentMarginRate          = (lpReqQryInstrumentMarginRate          )::GetProcAddress( gKStrdDLLInstance, "ReqQryInstrumentMarginRate" ) ) == NULL )
		return 34;
	if ( ( pReqQryInstrumentCommissionRate      = (lpReqQryInstrumentCommissionRate      )::GetProcAddress( gKStrdDLLInstance, "ReqQryInstrumentCommissionRate" ) ) == NULL )
		return 35;
	if ( ( pReqQryExchange                      = (lpReqQryExchange                      )::GetProcAddress( gKStrdDLLInstance, "ReqQryExchange" ) ) == NULL )
		return 36;
	if ( ( pReqQryProduct                       = (lpReqQryProduct                       )::GetProcAddress( gKStrdDLLInstance, "ReqQryProduct" ) ) == NULL )
		return 37;
	if ( ( pReqQryInstrument                    = (lpReqQryInstrument                    )::GetProcAddress( gKStrdDLLInstance, "ReqQryInstrument" ) ) == NULL )
		return 38;
	if ( ( pReqQryDepthMarketData               = (lpReqQryDepthMarketData               )::GetProcAddress( gKStrdDLLInstance, "ReqQryDepthMarketData" ) ) == NULL )
		return 39;
	if ( ( pReqQrySettlementInfo                = (lpReqQrySettlementInfo                )::GetProcAddress( gKStrdDLLInstance, "ReqQrySettlementInfo" ) ) == NULL )
		return 40;
	if ( ( pReqQryTransferBank                  = (lpReqQryTransferBank                  )::GetProcAddress( gKStrdDLLInstance, "ReqQryTransferBank" ) ) == NULL )
		return 41;
	if ( ( pReqQryInvestorPositionDetail        = (lpReqQryInvestorPositionDetail        )::GetProcAddress( gKStrdDLLInstance, "ReqQryInvestorPositionDetail" ) ) == NULL )
		return 42;
	if ( ( pReqQryNotice                        = (lpReqQryNotice                        )::GetProcAddress( gKStrdDLLInstance, "ReqQryNotice" ) ) == NULL )
		return 43;
	if ( ( pReqQrySettlementInfoConfirm         = (lpReqQrySettlementInfoConfirm         )::GetProcAddress( gKStrdDLLInstance, "ReqQrySettlementInfoConfirm" ) ) == NULL )
		return 44;
	if ( ( pReqQryInvestorPositionCombineDetail = (lpReqQryInvestorPositionCombineDetail )::GetProcAddress( gKStrdDLLInstance, "ReqQryInvestorPositionCombineDetail" ) ) == NULL )
		return 45;
	if ( ( pReqQryCFMMCTradingAccountKey        = (lpReqQryCFMMCTradingAccountKey        )::GetProcAddress( gKStrdDLLInstance, "ReqQryCFMMCTradingAccountKey" ) ) == NULL )
		return 46;
	if ( ( pReqQryEWarrantOffset                = (lpReqQryEWarrantOffset                )::GetProcAddress( gKStrdDLLInstance, "ReqQryEWarrantOffset" ) ) == NULL )
		return 47;
	if ( ( pReqQryInvestorProductGroupMargin    = (lpReqQryInvestorProductGroupMargin    )::GetProcAddress( gKStrdDLLInstance, "ReqQryInvestorProductGroupMargin" ) ) == NULL )
		return 48;
	if ( ( pReqQryExchangeMarginRate            = (lpReqQryExchangeMarginRate            )::GetProcAddress( gKStrdDLLInstance, "ReqQryExchangeMarginRate" ) ) == NULL )
		return 49;
	if ( ( pReqQryExchangeMarginRateAdjust      = (lpReqQryExchangeMarginRateAdjust      )::GetProcAddress( gKStrdDLLInstance, "ReqQryExchangeMarginRateAdjust" ) ) == NULL )
		return 50;
	if ( ( pReqQryExchangeRate                  = (lpReqQryExchangeRate                  )::GetProcAddress( gKStrdDLLInstance, "ReqQryExchangeRate" ) ) == NULL )
		return 51;
	if ( ( pReqQrySecAgentACIDMap               = (lpReqQrySecAgentACIDMap               )::GetProcAddress( gKStrdDLLInstance, "ReqQrySecAgentACIDMap" ) ) == NULL )
		return 52;
	if ( ( pReqQryOptionInstrTradeCost          = (lpReqQryOptionInstrTradeCost          )::GetProcAddress( gKStrdDLLInstance, "ReqQryOptionInstrTradeCost" ) ) == NULL )
		return 53;
	if ( ( pReqQryOptionInstrCommRate           = (lpReqQryOptionInstrCommRate           )::GetProcAddress( gKStrdDLLInstance, "ReqQryOptionInstrCommRate" ) ) == NULL )
		return 54;
	if ( ( pReqQryExecOrder                     = (lpReqQryExecOrder                     )::GetProcAddress( gKStrdDLLInstance, "ReqQryExecOrder" ) ) == NULL )
		return 55;
	if ( ( pReqQryForQuote                      = (lpReqQryForQuote                      )::GetProcAddress( gKStrdDLLInstance, "ReqQryForQuote" ) ) == NULL )
		return 56;
	if ( ( pReqQryQuote                         = (lpReqQryQuote                         )::GetProcAddress( gKStrdDLLInstance, "ReqQryQuote" ) ) == NULL )
		return 57;
	if ( ( pReqQryTransferSerial                = (lpReqQryTransferSerial                )::GetProcAddress( gKStrdDLLInstance, "ReqQryTransferSerial" ) ) == NULL )
		return 58;
	if ( ( pReqQryAccountregister               = (lpReqQryAccountregister               )::GetProcAddress( gKStrdDLLInstance, "ReqQryAccountregister" ) ) == NULL )
		return 59;
	if ( ( pReqQryContractBank                  = (lpReqQryContractBank                  )::GetProcAddress( gKStrdDLLInstance, "ReqQryContractBank" ) ) == NULL )
		return 60;
	if ( ( pReqQryParkedOrder                   = (lpReqQryParkedOrder                   )::GetProcAddress( gKStrdDLLInstance, "ReqQryParkedOrder" ) ) == NULL )
		return 61;
	if ( ( pReqQryParkedOrderAction             = (lpReqQryParkedOrderAction             )::GetProcAddress( gKStrdDLLInstance, "ReqQryParkedOrderAction" ) ) == NULL )
		return 62;
	if ( ( pReqQryTradingNotice                 = (lpReqQryTradingNotice                 )::GetProcAddress( gKStrdDLLInstance, "ReqQryTradingNotice" ) ) == NULL )
		return 63;
	if ( ( pReqQryBrokerTradingParams           = (lpReqQryBrokerTradingParams           )::GetProcAddress( gKStrdDLLInstance, "ReqQryBrokerTradingParams" ) ) == NULL )
		return 64;
	if ( ( pReqQryBrokerTradingAlgos            = (lpReqQryBrokerTradingAlgos            )::GetProcAddress( gKStrdDLLInstance, "ReqQryBrokerTradingAlgos" ) ) == NULL )
		return 65;
	if ( ( pReqQueryCFMMCTradingAccountToken    = (lpReqQueryCFMMCTradingAccountToken    )::GetProcAddress( gKStrdDLLInstance, "ReqQueryCFMMCTradingAccountToken" ) ) == NULL )
		return 66;
	if ( ( pReqFromBankToFutureByFuture         = (lpReqFromBankToFutureByFuture         )::GetProcAddress( gKStrdDLLInstance, "ReqFromBankToFutureByFuture" ) ) == NULL )
		return 67;
	if ( ( pReqFromFutureToBankByFuture         = (lpReqFromFutureToBankByFuture         )::GetProcAddress( gKStrdDLLInstance, "ReqFromFutureToBankByFuture" ) ) == NULL )
		return 68;
	if ( ( pReqQueryBankAccountMoneyByFuture    = (lpReqQueryBankAccountMoneyByFuture    )::GetProcAddress( gKStrdDLLInstance, "ReqQueryBankAccountMoneyByFuture" ) ) == NULL )
		return 69;
	// v2.3.50212 // 20150410 by Tim Lin
	if ( ( pReqLockInsert                       = (lpReqLockInsert                       )::GetProcAddress( gKStrdDLLInstance, "ReqLockInsert" ) ) == NULL )
		return 70;///用戶端發出鎖定請求
	if ( ( pReqQryLock                          = (lpReqQryLock                          )::GetProcAddress( gKStrdDLLInstance, "ReqQryLock" ) ) == NULL )
		return 71;///用戶端發出查詢鎖定請求
	if ( ( pReqQryLockPosition                  = (lpReqQryLockPosition                  )::GetProcAddress( gKStrdDLLInstance, "ReqQryLockPosition" ) ) == NULL )
		return 72;///用戶端發出查詢鎖定證券倉位請求
	// ---- CKSOptionApi ----
	// v2.3.50212 // 20150410 by Tim Lin
	if ( ( pReqQryIndexPrice					= (lpReqQryIndexPrice                   )::GetProcAddress( gKStrdDLLInstance, "ReqQryIndexPrice" ) ) == NULL )
		return 121;
	if ( ( pReqQryOptionInstrGuard				= (lpReqQryOptionInstrGuard             )::GetProcAddress( gKStrdDLLInstance, "ReqQryOptionInstrGuard" ) ) == NULL )
		return 122;
	if ( ( pReqQryExecOrderVolume	   			= (lpReqQryExecOrderVolume              )::GetProcAddress( gKStrdDLLInstance, "ReqQryExecOrderVolume" ) ) == NULL )
		return 123;
	if ( ( pReqQryStockOptionAssignment			= (lpReqQryStockOptionAssignment        )::GetProcAddress( gKStrdDLLInstance, "ReqQryStockOptionAssignment" ) ) == NULL )
		return 124;
	if ( ( pReqQryInvestorTradeLevel   			= (lpReqQryInvestorTradeLevel           )::GetProcAddress( gKStrdDLLInstance, "ReqQryInvestorTradeLevel" ) ) == NULL )
		return 125;
	if ( ( pReqQryPurchaseLimitAmt	   			= (lpReqQryPurchaseLimitAmt             )::GetProcAddress( gKStrdDLLInstance, "ReqQryPurchaseLimitAmt" ) ) == NULL )
		return 126;
	if ( ( pReqQryPositionLimitVol	   			= (lpReqQryPositionLimitVol             )::GetProcAddress( gKStrdDLLInstance, "ReqQryPositionLimitVol" ) ) == NULL )
		return 127;
	if ( ( pReqQryHistoryOrder					= (lpReqQryHistoryOrder                 )::GetProcAddress( gKStrdDLLInstance, "ReqQryHistoryOrder" ) ) == NULL )
		return 128;
	if ( ( pReqQryHistoryTrade					= (lpReqQryHistoryTrade                 )::GetProcAddress( gKStrdDLLInstance, "ReqQryHistoryTrade" ) ) == NULL )
		return 129;
	if ( ( pReqQryHistoryAssignment	   			= (lpReqQryStockOptionHistoryAssignment )::GetProcAddress( gKStrdDLLInstance, "ReqQryStockOptionHistoryAssignment" ) ) == NULL )
		return 130;
	if ( ( pReqQrySODelivDetail					= (lpReqQryStockOptionDelivDetail       )::GetProcAddress( gKStrdDLLInstance, "ReqQryStockOptionDelivDetail" ) ) == NULL )
		return 131;
	if ( ( pReqAutoExecOrderAction	   			= (lpReqAutoExecOrderAction             )::GetProcAddress( gKStrdDLLInstance, "ReqAutoExecOrderAction" ) ) == NULL )
		return 132;
	/*
	if ( ( pReqQryKSExecOrder                   = (lpReqQryKSExecOrder                   )::GetProcAddress( gKStrdDLLInstance, "ReqQryKSExecOrder" ) ) == NULL )
		return 121;
	if ( ( pReqKSExecOrderInsert                = (lpReqKSExecOrderInsert                )::GetProcAddress( gKStrdDLLInstance, "ReqKSExecOrderInsert" ) ) == NULL )
		return 122;
	if ( ( pReqQryExecOrderVolume               = (lpReqQryExecOrderVolume               )::GetProcAddress( gKStrdDLLInstance, "ReqQryExecOrderVolume" ) ) == NULL )
		return 123;
	if ( ( pReqQrySpotOptionParams              = (lpReqQrySpotOptionParams              )::GetProcAddress( gKStrdDLLInstance, "ReqQrySpotOptionParams" ) ) == NULL )
		return 124;
	if ( ( pReqQryOptionInsInfo                 = (lpReqQryOptionInsInfo                 )::GetProcAddress( gKStrdDLLInstance, "ReqQryOptionInsInfo" ) ) == NULL )
		return 125;
	if ( ( pReqQryStockOptionInsCommRate        = (lpReqQryStockOptionInsCommRate        )::GetProcAddress( gKStrdDLLInstance, "ReqQryStockOptionInsCommRate" ) ) == NULL )
		return 126;
	if ( ( pReqQryStockOptionInsMarginRate      = (lpReqQryStockOptionInsMarginRate      )::GetProcAddress( gKStrdDLLInstance, "ReqQryStockOptionInsMarginRate" ) ) == NULL )
		return 127;
	if ( ( pReqQryStockOptionAssignment         = (lpReqQryStockOptionAssignment         )::GetProcAddress( gKStrdDLLInstance, "ReqQryStockOptionAssignment" ) ) == NULL )
		return 128;
	if ( ( pReqQryInvestorTradeLevel            = (lpReqQryInvestorTradeLevel            )::GetProcAddress( gKStrdDLLInstance, "ReqQryInvestorTradeLevel" ) ) == NULL )
		return 129;
	if ( ( pReqQryPurchaseLimitAmt              = (lpReqQryPurchaseLimitAmt              )::GetProcAddress( gKStrdDLLInstance, "ReqQryPurchaseLimitAmt" ) ) == NULL )
		return 130;
	if ( ( pReqQryPositionLimitVol              = (lpReqQryPositionLimitVol              )::GetProcAddress( gKStrdDLLInstance, "ReqQryPositionLimitVol" ) ) == NULL )
		return 131;
	if ( ( pReqQryHistoryOrder                  = (lpReqQryHistoryOrder                  )::GetProcAddress( gKStrdDLLInstance, "ReqQryHistoryOrder" ) ) == NULL )
		return 132;
	if ( ( pReqQryHistoryTrade                  = (lpReqQryHistoryTrade                  )::GetProcAddress( gKStrdDLLInstance, "ReqQryHistoryTrade" ) ) == NULL )
		return 133;
	if ( ( pReqQryStockOptionHistoryAssignment  = (lpReqQryStockOptionHistoryAssignment  )::GetProcAddress( gKStrdDLLInstance, "ReqQryStockOptionHistoryAssignment" ) ) == NULL )
		return 134;
	if ( ( pReqQryStockOptionDelivDetail        = (lpReqQryStockOptionDelivDetail        )::GetProcAddress( gKStrdDLLInstance, "ReqQryStockOptionDelivDetail" ) ) == NULL )
		return 135;
	if ( ( pReqAutoExecOrderAction              = (lpReqAutoExecOrderAction              )::GetProcAddress( gKStrdDLLInstance, "ReqAutoExecOrderAction" ) ) == NULL )
		return 136;
	*/
	// ---- CKSVocApi ----
	// v2.3.50212 // 20150410 by Tim Lin
	if ( ( pReqQueryInvestorOpenPosition        = (lpReqQueryInvestorOpenPosition        )::GetProcAddress( gKStrdDLLInstance, "ReqQueryInvestorOpenPosition" ) ) == NULL )
		return 161;
	if ( ( pReqQueryInvestorOpenCombinePosition = (lpReqQueryInvestorOpenCombinePosition )::GetProcAddress( gKStrdDLLInstance, "ReqQueryInvestorOpenCombinePosition" ) ) == NULL )
		return 162;
	if ( ( pReqBulkCancelOrder                  = (lpReqBulkCancelOrder                  )::GetProcAddress( gKStrdDLLInstance, "ReqBulkCancelOrder" ) ) == NULL )
		return 163;
	if ( ( pReqQryCloseStrategy                 = (lpReqQryCloseStrategy                 )::GetProcAddress( gKStrdDLLInstance, "ReqQryCloseStrategy" ) ) == NULL )
		return 164;
	if ( ( pReqQryCombStrategy                  = (lpReqQryCombStrategy                  )::GetProcAddress( gKStrdDLLInstance, "ReqQryCombStrategy" ) ) == NULL )
		return 165;
	if ( ( pReqQryOptionCombStrategy            = (lpReqQryOptionCombStrategy            )::GetProcAddress( gKStrdDLLInstance, "ReqQryOptionCombStrategy" ) ) == NULL )
		return 166;
	if ( ( pReqQryTransferInfo                  = (lpReqQryTransferInfo                  )::GetProcAddress( gKStrdDLLInstance, "ReqQryTransferInfo" ) ) == NULL )
		return 167;
	if ( ( pReqQryKSTradingNotice               = (lpReqQryKSTradingNotice               )::GetProcAddress( gKStrdDLLInstance, "ReqQryKSTradingNotice" ) ) == NULL )
		return 168;
	if ( ( pReqQryUserProductUrl                = (lpReqQryUserProductUrl                )::GetProcAddress( gKStrdDLLInstance, "ReqQryUserProductUrl" ) ) == NULL )
		return 169;

	// ???????? // 20150410 by Tim Lin
	//if ( ( pReqQryMaxCombActionVolume           = (lpReqQryMaxCombActionVolume        	 )::GetProcAddress( gKStrdDLLInstance, "ReqQryMaxCombActionVolume" ) ) == NULL )
	//	return 170;

	if ( ( pReqQryKSInstrumentMarginRate        = (lpReqQryKSInstrumentMarginRate        )::GetProcAddress( gKStrdDLLInstance, "ReqQryKSInstrumentMarginRate" ) ) == NULL )
		return 171;

	/*
	if ( ( pReqQryCrossRate                     = (lpReqQryCrossRate                     )::GetProcAddress( gKStrdDLLInstance, "ReqQryCrossRate" ) ) == NULL )
		return 170;
	if ( ( pReqMatchOrderInsert                 = (lpReqMatchOrderInsert                 )::GetProcAddress( gKStrdDLLInstance, "ReqMatchOrderInsert" ) ) == NULL )
		return 171;
	if ( ( pReqQryMatchOrder                    = (lpReqQryMatchOrder                    )::GetProcAddress( gKStrdDLLInstance, "ReqQryMatchOrder" ) ) == NULL )
		return 172;
	if ( ( pReqQryMaxMatchOrderVolume           = (lpReqQryMaxMatchOrderVolume           )::GetProcAddress( gKStrdDLLInstance, "ReqQryMaxMatchOrderVolume" ) ) == NULL )
		return 173;
	*/

	return 0;
}
//------------------------------------------------------------------------------------------------------------------------
int KStrdTraderApiInit::InitRegCallbackFunctions( void )
{
	// ---- CThostFtdcTraderSpi ----
	if ( ( pRegOnFrontConnected                          = (lpRegOnFrontConnected                         )::GetProcAddress( gKStrdDLLInstance, "RegOnFrontConnected" ) ) == NULL )
		return 1;
	if ( ( pRegOnFrontDisconnected                       = (lpRegOnFrontDisconnected                      )::GetProcAddress( gKStrdDLLInstance, "RegOnFrontDisconnected" ) ) == NULL )
		return 2;
	if ( ( pRegOnHeartBeatWarning                        = (lpRegOnHeartBeatWarning                       )::GetProcAddress( gKStrdDLLInstance, "RegOnHeartBeatWarning" ) ) == NULL )
		return 3;
	if ( ( pRegOnRspAuthenticate                         = (lpRegOnRspAuthenticate                        )::GetProcAddress( gKStrdDLLInstance, "RegOnRspAuthenticate" ) ) == NULL )
		return 4;
	if ( ( pRegOnRspUserLogin                            = (lpRegOnRspUserLogin                           )::GetProcAddress( gKStrdDLLInstance, "RegOnRspUserLogin" ) ) == NULL )
		return 5;
	if ( ( pRegOnRspUserLogout                           = (lpRegOnRspUserLogout                          )::GetProcAddress( gKStrdDLLInstance, "RegOnRspUserLogout" ) ) == NULL )
		return 6;
	if ( ( pRegOnRspUserPasswordUpdate                   = (lpRegOnRspUserPasswordUpdate                  )::GetProcAddress( gKStrdDLLInstance, "RegOnRspUserPasswordUpdate" ) ) == NULL )
		return 7;
	if ( ( pRegOnRspTradingAccountPasswordUpdate         = (lpRegOnRspTradingAccountPasswordUpdate        )::GetProcAddress( gKStrdDLLInstance, "RegOnRspTradingAccountPasswordUpdate" ) ) == NULL )
		return 8;
	if ( ( pRegOnRspOrderInsert                          = (lpRegOnRspOrderInsert                         )::GetProcAddress( gKStrdDLLInstance, "RegOnRspOrderInsert" ) ) == NULL )
		return 9;
	if ( ( pRegOnRspParkedOrderInsert                    = (lpRegOnRspParkedOrderInsert                   )::GetProcAddress( gKStrdDLLInstance, "RegOnRspParkedOrderInsert" ) ) == NULL )
		return 10;
	if ( ( pRegOnRspParkedOrderAction                    = (lpRegOnRspParkedOrderAction                   )::GetProcAddress( gKStrdDLLInstance, "RegOnRspParkedOrderAction" ) ) == NULL )
		return 11;
	if ( ( pRegOnRspOrderAction                          = (lpRegOnRspOrderAction                         )::GetProcAddress( gKStrdDLLInstance, "RegOnRspOrderAction" ) ) == NULL )
		return 12;
	if ( ( pRegOnRspQueryMaxOrderVolume                  = (lpRegOnRspQueryMaxOrderVolume                 )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQueryMaxOrderVolume" ) ) == NULL )
		return 13;
	if ( ( pRegOnRspSettlementInfoConfirm                = (lpRegOnRspSettlementInfoConfirm               )::GetProcAddress( gKStrdDLLInstance, "RegOnRspSettlementInfoConfirm" ) ) == NULL )
		return 14;
	if ( ( pRegOnRspRemoveParkedOrder                    = (lpRegOnRspRemoveParkedOrder                   )::GetProcAddress( gKStrdDLLInstance, "RegOnRspRemoveParkedOrder" ) ) == NULL )
		return 15;
	if ( ( pRegOnRspRemoveParkedOrderAction              = (lpRegOnRspRemoveParkedOrderAction             )::GetProcAddress( gKStrdDLLInstance, "RegOnRspRemoveParkedOrderAction" ) ) == NULL )
		return 16;
	if ( ( pRegOnRspExecOrderInsert                      = (lpRegOnRspExecOrderInsert                     )::GetProcAddress( gKStrdDLLInstance, "RegOnRspExecOrderInsert" ) ) == NULL )
		return 17;
	if ( ( pRegOnRspExecOrderAction                      = (lpRegOnRspExecOrderAction                     )::GetProcAddress( gKStrdDLLInstance, "RegOnRspExecOrderAction" ) ) == NULL )
		return 18;
	if ( ( pRegOnRspForQuoteInsert                       = (lpRegOnRspForQuoteInsert                      )::GetProcAddress( gKStrdDLLInstance, "RegOnRspForQuoteInsert" ) ) == NULL )
		return 19;
	if ( ( pRegOnRspQuoteInsert                          = (lpRegOnRspQuoteInsert                         )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQuoteInsert" ) ) == NULL )
		return 20;
	if ( ( pRegOnRspQuoteAction                          = (lpRegOnRspQuoteAction                         )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQuoteAction" ) ) == NULL )
		return 21;
	if ( ( pRegOnRspQryOrder                             = (lpRegOnRspQryOrder                            )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryOrder" ) ) == NULL )
		return 22;
	if ( ( pRegOnRspQryTrade                             = (lpRegOnRspQryTrade                            )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryTrade" ) ) == NULL )
		return 23;
	if ( ( pRegOnRspQryInvestorPosition                  = (lpRegOnRspQryInvestorPosition                 )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryInvestorPosition" ) ) == NULL )
		return 24;
	if ( ( pRegOnRspQryTradingAccount                    = (lpRegOnRspQryTradingAccount                   )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryTradingAccount" ) ) == NULL )
		return 25;
	if ( ( pRegOnRspQryInvestor                          = (lpRegOnRspQryInvestor                         )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryInvestor" ) ) == NULL )
		return 26;
	if ( ( pRegOnRspQryTradingCode                       = (lpRegOnRspQryTradingCode                      )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryTradingCode" ) ) == NULL )
		return 27;
	if ( ( pRegOnRspQryInstrumentMarginRate              = (lpRegOnRspQryInstrumentMarginRate             )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryInstrumentMarginRate" ) ) == NULL )
		return 28;
	if ( ( pRegOnRspQryInstrumentCommissionRate          = (lpRegOnRspQryInstrumentCommissionRate         )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryInstrumentCommissionRate" ) ) == NULL )
		return 29;
	if ( ( pRegOnRspQryExchange                          = (lpRegOnRspQryExchange                         )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryExchange" ) ) == NULL )
		return 30;
	if ( ( pRegOnRspQryProduct                           = (lpRegOnRspQryProduct                          )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryProduct" ) ) == NULL )
		return 31;
	if ( ( pRegOnRspQryInstrument                        = (lpRegOnRspQryInstrument                       )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryInstrument" ) ) == NULL )
		return 32;
	if ( ( pRegOnRspQryDepthMarketData                   = (lpRegOnRspQryDepthMarketData                  )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryDepthMarketData" ) ) == NULL )
		return 33;
	if ( ( pRegOnRspQrySettlementInfo                    = (lpRegOnRspQrySettlementInfo                   )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQrySettlementInfo" ) ) == NULL )
		return 34;
	if ( ( pRegOnRspQryTransferBank                      = (lpRegOnRspQryTransferBank                     )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryTransferBank" ) ) == NULL )
		return 35;
	if ( ( pRegOnRspQryInvestorPositionDetail            = (lpRegOnRspQryInvestorPositionDetail           )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryInvestorPositionDetail" ) ) == NULL )
		return 36;
	if ( ( pRegOnRspQryNotice                            = (lpRegOnRspQryNotice                           )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryNotice" ) ) == NULL )
		return 37;
	if ( ( pRegOnRspQrySettlementInfoConfirm             = (lpRegOnRspQrySettlementInfoConfirm            )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQrySettlementInfoConfirm" ) ) == NULL )
		return 38;
	if ( ( pRegOnRspQryInvestorPositionCombineDetail     = (lpRegOnRspQryInvestorPositionCombineDetail    )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryInvestorPositionCombineDetail" ) ) == NULL )
		return 39;
	if ( ( pRegOnRspQryCFMMCTradingAccountKey            = (lpRegOnRspQryCFMMCTradingAccountKey           )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryCFMMCTradingAccountKey" ) ) == NULL )
		return 40;
	if ( ( pRegOnRspQryEWarrantOffset                    = (lpRegOnRspQryEWarrantOffset                   )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryEWarrantOffset" ) ) == NULL )
		return 41;
	if ( ( pRegOnRspQryInvestorProductGroupMargin        = (lpRegOnRspQryInvestorProductGroupMargin       )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryInvestorProductGroupMargin" ) ) == NULL )
		return 42;
	if ( ( pRegOnRspQryExchangeMarginRate                = (lpRegOnRspQryExchangeMarginRate               )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryExchangeMarginRate" ) ) == NULL )
		return 43;
	if ( ( pRegOnRspQryExchangeMarginRateAdjust          = (lpRegOnRspQryExchangeMarginRateAdjust         )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryExchangeMarginRateAdjust" ) ) == NULL )
		return 44;
	if ( ( pRegOnRspQryExchangeRate                      = (lpRegOnRspQryExchangeRate                     )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryExchangeRate" ) ) == NULL )
		return 45;
	if ( ( pRegOnRspQrySecAgentACIDMap                   = (lpRegOnRspQrySecAgentACIDMap                  )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQrySecAgentACIDMap" ) ) == NULL )
		return 46;
	if ( ( pRegOnRspQryOptionInstrTradeCost              = (lpRegOnRspQryOptionInstrTradeCost             )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryOptionInstrTradeCost" ) ) == NULL )
		return 47;
	if ( ( pRegOnRspQryOptionInstrCommRate               = (lpRegOnRspQryOptionInstrCommRate              )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryOptionInstrCommRate" ) ) == NULL )
		return 48;
	if ( ( pRegOnRspQryExecOrder                         = (lpRegOnRspQryExecOrder                        )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryExecOrder" ) ) == NULL )
		return 49;
	if ( ( pRegOnRspQryForQuote                          = (lpRegOnRspQryForQuote                         )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryForQuote" ) ) == NULL )
		return 50;
	if ( ( pRegOnRspQryQuote                             = (lpRegOnRspQryQuote                            )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryQuote" ) ) == NULL )
		return 51;
	if ( ( pRegOnRspQryTransferSerial                    = (lpRegOnRspQryTransferSerial                   )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryTransferSerial" ) ) == NULL )
		return 52;
	if ( ( pRegOnRspQryAccountregister                   = (lpRegOnRspQryAccountregister                  )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryAccountregister" ) ) == NULL )
		return 53;
	if ( ( pRegOnRspError                                = (lpRegOnRspError                               )::GetProcAddress( gKStrdDLLInstance, "RegOnRspError" ) ) == NULL )
		return 54;
	if ( ( pRegOnRtnOrder                                = (lpRegOnRtnOrder                               )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnOrder" ) ) == NULL )
		return 55;
	if ( ( pRegOnRtnTrade                                = (lpRegOnRtnTrade                               )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnTrade" ) ) == NULL )
		return 56;
	if ( ( pRegOnErrRtnOrderInsert                       = (lpRegOnErrRtnOrderInsert                      )::GetProcAddress( gKStrdDLLInstance, "RegOnErrRtnOrderInsert" ) ) == NULL )
		return 57;
	if ( ( pRegOnErrRtnOrderAction                       = (lpRegOnErrRtnOrderAction                      )::GetProcAddress( gKStrdDLLInstance, "RegOnErrRtnOrderAction" ) ) == NULL )
		return 58;
	if ( ( pRegOnRtnInstrumentStatus                     = (lpRegOnRtnInstrumentStatus                    )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnInstrumentStatus" ) ) == NULL )
		return 59;
	if ( ( pRegOnRtnTradingNotice                        = (lpRegOnRtnTradingNotice                       )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnTradingNotice" ) ) == NULL )
		return 60;
	if ( ( pRegOnRtnErrorConditionalOrder                = (lpRegOnRtnErrorConditionalOrder               )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnErrorConditionalOrder" ) ) == NULL )
		return 61;
	if ( ( pRegOnRtnExecOrder                            = (lpRegOnRtnExecOrder                           )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnExecOrder" ) ) == NULL )
		return 62;
	if ( ( pRegOnErrRtnExecOrderInsert                   = (lpRegOnErrRtnExecOrderInsert                  )::GetProcAddress( gKStrdDLLInstance, "RegOnErrRtnExecOrderInsert" ) ) == NULL )
		return 63;
	if ( ( pRegOnErrRtnExecOrderAction                   = (lpRegOnErrRtnExecOrderAction                  )::GetProcAddress( gKStrdDLLInstance, "RegOnErrRtnExecOrderAction" ) ) == NULL )
		return 64;
	if ( ( pRegOnErrRtnForQuoteInsert                    = (lpRegOnErrRtnForQuoteInsert                   )::GetProcAddress( gKStrdDLLInstance, "RegOnErrRtnForQuoteInsert" ) ) == NULL )
		return 65;
	if ( ( pRegOnRtnQuote                                = (lpRegOnRtnQuote                               )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnQuote" ) ) == NULL )
		return 66;
	if ( ( pRegOnErrRtnQuoteInsert                       = (lpRegOnErrRtnQuoteInsert                      )::GetProcAddress( gKStrdDLLInstance, "RegOnErrRtnQuoteInsert" ) ) == NULL )
		return 67;
	if ( ( pRegOnErrRtnQuoteAction                       = (lpRegOnErrRtnQuoteAction                      )::GetProcAddress( gKStrdDLLInstance, "RegOnErrRtnQuoteAction" ) ) == NULL )
		return 68;
	if ( ( pRegOnRtnForQuoteRsp                          = (lpRegOnRtnForQuoteRsp                         )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnForQuoteRsp" ) ) == NULL )
		return 69;
	if ( ( pRegOnRtnCFMMCTradingAccountToken             = (lpRegOnRtnCFMMCTradingAccountToken            )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnCFMMCTradingAccountToken" ) ) == NULL )
		return 70;
	if ( ( pRegOnRspQryContractBank                      = (lpRegOnRspQryContractBank                     )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryContractBank" ) ) == NULL )
		return 71;
	if ( ( pRegOnRspQryParkedOrder                       = (lpRegOnRspQryParkedOrder                      )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryParkedOrder" ) ) == NULL )
		return 72;
	if ( ( pRegOnRspQryParkedOrderAction                 = (lpRegOnRspQryParkedOrderAction                )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryParkedOrderAction" ) ) == NULL )
		return 73;
	if ( ( pRegOnRspQryTradingNotice                     = (lpRegOnRspQryTradingNotice                    )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryTradingNotice" ) ) == NULL )
		return 74;
	if ( ( pRegOnRspQryBrokerTradingParams               = (lpRegOnRspQryBrokerTradingParams              )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryBrokerTradingParams" ) ) == NULL )
		return 75;
	if ( ( pRegOnRspQryBrokerTradingAlgos                = (lpRegOnRspQryBrokerTradingAlgos               )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryBrokerTradingAlgos" ) ) == NULL )
		return 76;
	if ( ( pRegOnRspQueryCFMMCTradingAccountToken        = (lpRegOnRspQueryCFMMCTradingAccountToken       )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQueryCFMMCTradingAccountToken" ) ) == NULL )
		return 77;
	if ( ( pRegOnRtnFromBankToFutureByBank               = (lpRegOnRtnFromBankToFutureByBank              )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnFromBankToFutureByBank" ) ) == NULL )
		return 78;
	if ( ( pRegOnRtnFromFutureToBankByBank               = (lpRegOnRtnFromFutureToBankByBank              )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnFromFutureToBankByBank" ) ) == NULL )
		return 79;
	if ( ( pRegOnRtnRepealFromBankToFutureByBank         = (lpRegOnRtnRepealFromBankToFutureByBank        )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnRepealFromBankToFutureByBank" ) ) == NULL )
		return 80;
	if ( ( pRegOnRtnRepealFromFutureToBankByBank         = (lpRegOnRtnRepealFromFutureToBankByBank        )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnRepealFromFutureToBankByBank" ) ) == NULL )
		return 81;
	if ( ( pRegOnRtnFromBankToFutureByFuture             = (lpRegOnRtnFromBankToFutureByFuture            )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnFromBankToFutureByFuture" ) ) == NULL )
		return 82;
	if ( ( pRegOnRtnFromFutureToBankByFuture             = (lpRegOnRtnFromFutureToBankByFuture            )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnFromFutureToBankByFuture" ) ) == NULL )
		return 83;
	if ( ( pRegOnRtnRepealFromBankToFutureByFutureManual = (lpRegOnRtnRepealFromBankToFutureByFutureManual)::GetProcAddress( gKStrdDLLInstance, "RegOnRtnRepealFromBankToFutureByFutureManual" ) ) == NULL )
		return 84;
	if ( ( pRegOnRtnRepealFromFutureToBankByFutureManual = (lpRegOnRtnRepealFromFutureToBankByFutureManual)::GetProcAddress( gKStrdDLLInstance, "RegOnRtnRepealFromFutureToBankByFutureManual" ) ) == NULL )
		return 85;
	if ( ( pRegOnRtnQueryBankBalanceByFuture             = (lpRegOnRtnQueryBankBalanceByFuture            )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnQueryBankBalanceByFuture" ) ) == NULL )
		return 86;
	if ( ( pRegOnErrRtnBankToFutureByFuture              = (lpRegOnErrRtnBankToFutureByFuture             )::GetProcAddress( gKStrdDLLInstance, "RegOnErrRtnBankToFutureByFuture" ) ) == NULL )
		return 87;
	if ( ( pRegOnErrRtnFutureToBankByFuture              = (lpRegOnErrRtnFutureToBankByFuture             )::GetProcAddress( gKStrdDLLInstance, "RegOnErrRtnFutureToBankByFuture" ) ) == NULL )
		return 88;
	if ( ( pRegOnErrRtnRepealBankToFutureByFutureManual  = (lpRegOnErrRtnRepealBankToFutureByFutureManual )::GetProcAddress( gKStrdDLLInstance, "RegOnErrRtnRepealBankToFutureByFutureManual" ) ) == NULL )
		return 89;
	if ( ( pRegOnErrRtnRepealFutureToBankByFutureManual  = (lpRegOnErrRtnRepealFutureToBankByFutureManual )::GetProcAddress( gKStrdDLLInstance, "RegOnErrRtnRepealFutureToBankByFutureManual" ) ) == NULL )
		return 90;
	if ( ( pRegOnErrRtnQueryBankBalanceByFuture          = (lpRegOnErrRtnQueryBankBalanceByFuture         )::GetProcAddress( gKStrdDLLInstance, "RegOnErrRtnQueryBankBalanceByFuture" ) ) == NULL )
		return 91;
	if ( ( pRegOnRtnRepealFromBankToFutureByFuture       = (lpRegOnRtnRepealFromBankToFutureByFuture      )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnRepealFromBankToFutureByFuture" ) ) == NULL )
		return 92;
	if ( ( pRegOnRtnRepealFromFutureToBankByFuture       = (lpRegOnRtnRepealFromFutureToBankByFuture      )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnRepealFromFutureToBankByFuture" ) ) == NULL )
		return 93;
	if ( ( pRegOnRspFromBankToFutureByFuture             = (lpRegOnRspFromBankToFutureByFuture            )::GetProcAddress( gKStrdDLLInstance, "RegOnRspFromBankToFutureByFuture" ) ) == NULL )
		return 94;
	if ( ( pRegOnRspFromFutureToBankByFuture             = (lpRegOnRspFromFutureToBankByFuture            )::GetProcAddress( gKStrdDLLInstance, "RegOnRspFromFutureToBankByFuture" ) ) == NULL )
		return 95;
	if ( ( pRegOnRspQueryBankAccountMoneyByFuture        = (lpRegOnRspQueryBankAccountMoneyByFuture       )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQueryBankAccountMoneyByFuture" ) ) == NULL )
		return 96;
	if ( ( pRegOnRtnOpenAccountByBank                    = (lpRegOnRtnOpenAccountByBank                   )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnOpenAccountByBank" ) ) == NULL )
		return 97;
	if ( ( pRegOnRtnCancelAccountByBank                  = (lpRegOnRtnCancelAccountByBank                 )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnCancelAccountByBank" ) ) == NULL )
		return 98;
	if ( ( pRegOnRtnChangeAccountByBank                  = (lpRegOnRtnChangeAccountByBank                 )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnChangeAccountByBank" ) ) == NULL )
		return 99;
	// v2.3.50212 // 20150410 by Tim Lin
	if ( ( pRegOnRspLockInsert							 = (lpRegOnRspLockInsert						  )::GetProcAddress( gKStrdDLLInstance, "RegOnRspLockInsert" ) ) == NULL )
		return 100;///鎖定應答
	if ( ( pRegOnRspQryLock							     = (lpRegOnRspQryLock   						  )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryLock" ) ) == NULL )
		return 101;///請求查詢鎖定回應
	if ( ( pRegOnRspQryLockPosition						 = (lpRegOnRspQryLockPosition					  )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryLockPosition" ) ) == NULL )
		return 102;///請求查詢鎖定證券倉位回應
	if ( ( pRegOnRtnLock							     = (lpRegOnRtnLock						          )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnLock" ) ) == NULL )
		return 103;///鎖定通知
	if ( ( pRegOnErrRtnLockInsert						 = (lpRegOnErrRtnLockInsert						  )::GetProcAddress( gKStrdDLLInstance, "RegOnErrRtnLockInsert" ) ) == NULL )
		return 104;///鎖定錯誤通知
	// ---- CKSOptionSpi ----
	// v2.3.50212 // 20150410 by Tim Lin
	if ( ( pRegOnRspQryIndexPrice                        = (lpRegOnRspQryIndexPrice                       )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryIndexPrice" ) ) == NULL )
		return 121;
	if ( ( pRegOnRspQryOptionInstrGuard                  = (lpRegOnRspQryOptionInstrGuard                 )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryOptionInstrGuard" ) ) == NULL )
		return 122;
	if ( ( pRegOnRspQryExecOrderVolume                   = (lpRegOnRspQryExecOrderVolume                  )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryExecOrderVolume" ) ) == NULL )
		return 123;
	if ( ( pRegOnRspQryStockOptionAssignment             = (lpRegOnRspQryStockOptionAssignment            )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryStockOptionAssignment" ) ) == NULL )
		return 124;
	if ( ( pRegOnRspQryInvestorTradeLevel                = (lpRegOnRspQryInvestorTradeLevel               )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryInvestorTradeLevel" ) ) == NULL )
		return 125;
	if ( ( pRegOnRspQryPurchaseLimitAmt                  = (lpRegOnRspQryPurchaseLimitAmt                 )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryPurchaseLimitAmt" ) ) == NULL )
		return 126;
	if ( ( pRegOnRspQryPositionLimitVol                  = (lpRegOnRspQryPositionLimitVol                 )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryPositionLimitVol" ) ) == NULL )
		return 127;
	if ( ( pRegOnRspQryHistoryOrder                      = (lpRegOnRspQryHistoryOrder                     )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryHistoryOrder" ) ) == NULL )
		return 128;
	if ( ( pRegOnRspQryHistoryTrade                      = (lpRegOnRspQryHistoryTrade                     )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryHistoryTrade" ) ) == NULL )
		return 129;
	if ( ( pRegOnRspQryHistoryAssignment      			 = (lpRegOnRspQryStockOptionHistoryAssignment     )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryStockOptionHistoryAssignment" ) ) == NULL )
		return 130;
	if ( ( pRegOnRspQrySODelivDetail            		 = (lpRegOnRspQryStockOptionDelivDetail           )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryStockOptionDelivDetail" ) ) == NULL )
		return 131;
	if ( ( pRegOnRspQryAutoExecOrderAction               = (lpRegOnRspAutoExecOrderAction                 )::GetProcAddress( gKStrdDLLInstance, "RegOnRspAutoExecOrderAction" ) ) == NULL )
		return 132;
	/*
	if ( ( pRegOnRspQryKSExecOrder                       = (lpRegOnRspQryKSExecOrder                      )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryKSExecOrder" ) ) == NULL )
		return 121;
	if ( ( pRegOnRspKSExecOrderInsert                    = (lpRegOnRspKSExecOrderInsert                   )::GetProcAddress( gKStrdDLLInstance, "RegOnRspKSExecOrderInsert" ) ) == NULL )
		return 122;
	if ( ( pRegOnRspQryExecOrderVolume                   = (lpRegOnRspQryExecOrderVolume                  )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryExecOrderVolume" ) ) == NULL )
		return 123;
	if ( ( pRegOnRspQrySpotOptionParams                  = (lpRegOnRspQrySpotOptionParams                 )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQrySpotOptionParams" ) ) == NULL )
		return 124;
	if ( ( pRegOnRspQryOptionInsInfo                     = (lpRegOnRspQryOptionInsInfo                    )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryOptionInsInfo" ) ) == NULL )
		return 125;
	if ( ( pRegOnRspQryStockOptionInsCommRate            = (lpRegOnRspQryStockOptionInsCommRate           )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryStockOptionInsCommRate" ) ) == NULL )
		return 126;
	if ( ( pRegOnRspQryStockOptionInsMarginRate          = (lpRegOnRspQryStockOptionInsMarginRate         )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryStockOptionInsMarginRate" ) ) == NULL )
		return 127;
	if ( ( pRegOnRspQryStockOptionAssignment             = (lpRegOnRspQryStockOptionAssignment            )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryStockOptionAssignment" ) ) == NULL )
		return 128;
	if ( ( pRegOnRspQryInvestorTradeLevel                = (lpRegOnRspQryInvestorTradeLevel               )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryInvestorTradeLevel" ) ) == NULL )
		return 129;
	if ( ( pRegOnRspQryPurchaseLimitAmt                  = (lpRegOnRspQryPurchaseLimitAmt                 )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryPurchaseLimitAmt" ) ) == NULL )
		return 130;
	if ( ( pRegOnRspQryPositionLimitVol                  = (lpRegOnRspQryPositionLimitVol                 )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryPositionLimitVol" ) ) == NULL )
		return 131;
	if ( ( pRegOnRspQryHistoryOrder                      = (lpRegOnRspQryHistoryOrder                     )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryHistoryOrder" ) ) == NULL )
		return 132;
	if ( ( pRegOnRspQryHistoryTrade                      = (lpRegOnRspQryHistoryTrade                     )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryHistoryTrade" ) ) == NULL )
		return 133;
	if ( ( pRegOnRspQryStockOptionHistoryAssignment      = (lpRegOnRspQryStockOptionHistoryAssignment     )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryStockOptionHistoryAssignment" ) ) == NULL )
		return 134;
	if ( ( pRegOnRspQryStockOptionDelivDetail            = (lpRegOnRspQryStockOptionDelivDetail           )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryStockOptionDelivDetail" ) ) == NULL )
		return 135;
	if ( ( pRegOnRspAutoExecOrderAction                  = (lpRegOnRspAutoExecOrderAction                 )::GetProcAddress( gKStrdDLLInstance, "RegOnRspAutoExecOrderAction" ) ) == NULL )
		return 136;
	if ( ( pRegOnRtnKSExecOrder                          = (lpRegOnRtnKSExecOrder                         )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnKSExecOrder" ) ) == NULL )
		return 137;
	*/
	// ---- CKSVocSpi ----
	// v2.3.50212 // 20150410 by Tim Lin
	if ( ( pRegOnRspQryInvestorOpenPosition              = (lpRegOnRspQryInvestorOpenPosition             )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryInvestorOpenPosition" ) ) == NULL )
		return 161;
	if ( ( pRegOnRspQryInvestorOpenCombinePosition       = (lpRegOnRspQryInvestorOpenCombinePosition      )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryInvestorOpenCombinePosition" ) ) == NULL )
		return 162;
	if ( ( pRegOnRspBulkCancelOrder                      = (lpRegOnRspBulkCancelOrder                     )::GetProcAddress( gKStrdDLLInstance, "RegOnRspBulkCancelOrder" ) ) == NULL )
		return 163;
	if ( ( pRegOnRspQryCloseStrategy                     = (lpRegOnRspQryCloseStrategy                    )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryCloseStrategy" ) ) == NULL )
		return 164;
	if ( ( pRegOnRspQryCombStrategy                      = (lpRegOnRspQryCombStrategy                     )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryCombStrategy" ) ) == NULL )
		return 165;
	if ( ( pRegOnRspQryOptionCombStrategy                = (lpRegOnRspQryOptionCombStrategy               )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryOptionCombStrategy" ) ) == NULL )
		return 166;
	if ( ( pRegOnRspQryTransferInfo                      = (lpRegOnRspQryTransferInfo                     )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryTransferInfo" ) ) == NULL )
		return 167;
	if ( ( pRegOnRspQryKSTradingNotice                   = (lpRegOnRspQryKSTradingNotice                  )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryKSTradingNotice" ) ) == NULL )
		return 168;
	if ( ( pRegOnRspQryUserProductUrl                    = (lpRegOnRspQryUserProductUrl                   )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryUserProductUrl" ) ) == NULL )
		return 169;
	if ( ( pRegOnRspQryMaxCombActionVolume               = (lpRegOnRspQryMaxCombActionVolume              )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryMaxCombActionVolume" ) ) == NULL )
		return 170;
	if ( ( pRegOnRtnKSTradingNotice                    	 = (lpRegOnRtnKSTradingNotice                     )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnKSTradingNotice" ) ) == NULL )
		return 171;
	if ( ( pRegOnRspQryKSInstrumentMarginRate          	 = (lpRegOnRspQryKSInstrumentMarginRate           )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryKSInstrumentMarginRate" ) ) == NULL )
		return 172;
	/*
	if ( ( pRegOnRspQryCrossRate                         = (lpRegOnRspQryCrossRate                        )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryCrossRate" ) ) == NULL )
		return 170;
	if ( ( pRegOnRspMatchOrderInsert                     = (lpRegOnRspMatchOrderInsert                    )::GetProcAddress( gKStrdDLLInstance, "RegOnRspMatchOrderInsert" ) ) == NULL )
		return 171;
	if ( ( pRegOnRspQryMatchOrder                        = (lpRegOnRspQryMatchOrder                       )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryMatchOrder" ) ) == NULL )
		return 172;
	if ( ( pRegOnRspQryMaxMatchOrderVolume               = (lpRegOnRspQryMaxMatchOrderVolume              )::GetProcAddress( gKStrdDLLInstance, "RegOnRspQryMaxMatchOrderVolume" ) ) == NULL )
		return 173;
	if ( ( pRegOnRtnKSTradingNotice                      = (lpRegOnRtnKSTradingNotice                     )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnKSTradingNotice" ) ) == NULL )
		return 174;
	if ( ( pRegOnRtnMatchOrder                           = (lpRegOnRtnMatchOrder                          )::GetProcAddress( gKStrdDLLInstance, "RegOnRtnMatchOrder" ) ) == NULL )
		return 175;
	*/

	return 0;
}
//------------------------------------------------------------------------------------------------------------------------
// ---- CThostFtdcTraderSpi ----
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnFrontConnected( HKSTRD Owner )
{
	TKStrdTraderApi* KStrdObj = (TKStrdTraderApi*)Owner;

	if ( KStrdObj != NULL )
	{
		TKStrdTraderSpi* Listener = KStrdObj->GetSpi( );
		if ( Listener != NULL )
		{
			Listener->OnKStrdAPIConnected( );
			PostMessage( KStrdObj->GetHWND(), WM_KSTRD_EVENT, (WPARAM)Listener, (LPARAM)NULL );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnFrontDisconnected( HKSTRD Owner, int nReason )
{
	TKStrdTraderApi* KStrdObj = (TKStrdTraderApi*)Owner;

	if ( KStrdObj != NULL )
	{
		TKStrdTraderSpi* Listener = KStrdObj->GetSpi( );
		if ( Listener != NULL )
		{
			TCTPParams* Params = new TCTPParams( nReason );
			Listener->OnKStrdAPIDisconnected( nReason );
			PostMessage( KStrdObj->GetHWND(), WM_KSTRD_EVENT+1, (WPARAM)Listener, (LPARAM)Params );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnHeartBeatWarning( HKSTRD Owner, int nTimeLapse )
{
	TCTPParams* Params = new TCTPParams( nTimeLapse );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+2, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspAuthenticate( HKSTRD Owner, KS_::CThostFtdcRspAuthenticateField *pRspAuthenticateField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspAuthenticateField, sizeof(KS_::CThostFtdcRspAuthenticateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+3, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspUserLogin( HKSTRD Owner, KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspUserLogin, sizeof(KS_::CThostFtdcRspUserLoginField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+4, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspUserLogout( HKSTRD Owner, KS_::CThostFtdcUserLogoutField *pUserLogout, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pUserLogout, sizeof(KS_::CThostFtdcUserLogoutField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+5, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspUserPasswordUpdate( HKSTRD Owner, KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pUserPasswordUpdate, sizeof(KS_::CThostFtdcUserPasswordUpdateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+6, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspTradingAccountPasswordUpdate( HKSTRD Owner, KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTradingAccountPasswordUpdate, sizeof(KS_::CThostFtdcTradingAccountPasswordUpdateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+7, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspOrderInsert( HKSTRD Owner, KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputOrder, sizeof(KS_::CThostFtdcInputOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+8, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspParkedOrderInsert( HKSTRD Owner, KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pParkedOrder, sizeof(KS_::CThostFtdcParkedOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+9, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspParkedOrderAction( HKSTRD Owner, KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pParkedOrderAction, sizeof(KS_::CThostFtdcParkedOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+10, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspOrderAction( HKSTRD Owner, KS_::CThostFtdcInputOrderActionField *pInputOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputOrderAction, sizeof(KS_::CThostFtdcInputOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+11, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQueryMaxOrderVolume( HKSTRD Owner, KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pQueryMaxOrderVolume, sizeof(KS_::CThostFtdcQueryMaxOrderVolumeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+12, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspSettlementInfoConfirm( HKSTRD Owner, KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pSettlementInfoConfirm, sizeof(KS_::CThostFtdcSettlementInfoConfirmField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+13, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspRemoveParkedOrder( HKSTRD Owner, KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pRemoveParkedOrder, sizeof(KS_::CThostFtdcRemoveParkedOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+14, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspRemoveParkedOrderAction( HKSTRD Owner, KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pRemoveParkedOrderAction, sizeof(KS_::CThostFtdcRemoveParkedOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+15, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspExecOrderInsert( HKSTRD Owner, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	// ????????
	//TCTPParams* Params = new TCTPParams( (void*)pInputExecOrder, sizeof(KS_::CThostFtdcInputExecOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );
	TCTPParams* Params = new TCTPParams( (void*)pInputExecOrder, sizeof(KS_::CThostFtdcInputExecOrderField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+16, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspExecOrderAction( HKSTRD Owner, KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputExecOrderAction, sizeof(KS_::CThostFtdcInputExecOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+17, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspForQuoteInsert( HKSTRD Owner, KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputForQuote, sizeof(KS_::CThostFtdcInputForQuoteField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+18, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQuoteInsert( HKSTRD Owner, KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputQuote, sizeof(KS_::CThostFtdcInputQuoteField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+19, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQuoteAction( HKSTRD Owner, KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputQuoteAction, sizeof(KS_::CThostFtdcInputQuoteActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+20, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryOrder( HKSTRD Owner, KS_::CThostFtdcOrderField *pOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pOrder, sizeof(KS_::CThostFtdcOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+21, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryTrade( HKSTRD Owner, KS_::CThostFtdcTradeField *pTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTrade, sizeof(KS_::CThostFtdcTradeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+22, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryInvestorPosition( HKSTRD Owner, KS_::CThostFtdcInvestorPositionField *pInvestorPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorPosition, sizeof(KS_::CThostFtdcInvestorPositionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+23, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryTradingAccount( HKSTRD Owner, KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTradingAccount, sizeof(KS_::CThostFtdcTradingAccountField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+24, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryInvestor( HKSTRD Owner, KS_::CThostFtdcInvestorField *pInvestor, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestor, sizeof(KS_::CThostFtdcInvestorField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+25, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryTradingCode( HKSTRD Owner, KS_::CThostFtdcTradingCodeField *pTradingCode, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTradingCode, sizeof(KS_::CThostFtdcTradingCodeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+26, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryInstrumentMarginRate( HKSTRD Owner, KS_::CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInstrumentMarginRate, sizeof(KS_::CThostFtdcInstrumentMarginRateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+27, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryInstrumentCommissionRate( HKSTRD Owner, KS_::CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInstrumentCommissionRate, sizeof(KS_::CThostFtdcInstrumentCommissionRateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+28, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryExchange( HKSTRD Owner, KS_::CThostFtdcExchangeField *pExchange, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pExchange, sizeof(KS_::CThostFtdcExchangeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+29, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryProduct( HKSTRD Owner, KS_::CThostFtdcProductField *pProduct, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pProduct, sizeof(KS_::CThostFtdcProductField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+30, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryInstrument( HKSTRD Owner, KS_::CThostFtdcInstrumentField *pInstrument, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInstrument, sizeof(KS_::CThostFtdcInstrumentField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+31, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryDepthMarketData( HKSTRD Owner, KS_::CThostFtdcDepthMarketDataField *pDepthMarketData, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pDepthMarketData, sizeof(KS_::CThostFtdcDepthMarketDataField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+32, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQrySettlementInfo( HKSTRD Owner, KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pSettlementInfo, sizeof(KS_::CThostFtdcSettlementInfoField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+33, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryTransferBank( HKSTRD Owner, KS_::CThostFtdcTransferBankField *pTransferBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTransferBank, sizeof(KS_::CThostFtdcTransferBankField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+34, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryInvestorPositionDetail( HKSTRD Owner, KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorPositionDetail, sizeof(KS_::CThostFtdcInvestorPositionDetailField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+35, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryNotice( HKSTRD Owner, KS_::CThostFtdcNoticeField *pNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pNotice, sizeof(KS_::CThostFtdcNoticeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+36, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQrySettlementInfoConfirm( HKSTRD Owner, KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pSettlementInfoConfirm, sizeof(KS_::CThostFtdcSettlementInfoConfirmField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+37, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryInvestorPositionCombineDetail( HKSTRD Owner, KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorPositionCombineDetail, sizeof(KS_::CThostFtdcInvestorPositionCombineDetailField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+38, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryCFMMCTradingAccountKey( HKSTRD Owner, KS_::CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pCFMMCTradingAccountKey, sizeof(KS_::CThostFtdcCFMMCTradingAccountKeyField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+39, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryEWarrantOffset( HKSTRD Owner, KS_::CThostFtdcEWarrantOffsetField *pEWarrantOffset, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pEWarrantOffset, sizeof(KS_::CThostFtdcEWarrantOffsetField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+40, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryInvestorProductGroupMargin( HKSTRD Owner, KS_::CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorProductGroupMargin, sizeof(KS_::CThostFtdcInvestorProductGroupMarginField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+41, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryExchangeMarginRate( HKSTRD Owner, KS_::CThostFtdcExchangeMarginRateField *pExchangeMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pExchangeMarginRate, sizeof(KS_::CThostFtdcExchangeMarginRateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+42, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryExchangeMarginRateAdjust( HKSTRD Owner, KS_::CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pExchangeMarginRateAdjust, sizeof(KS_::CThostFtdcExchangeMarginRateAdjustField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+43, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryExchangeRate( HKSTRD Owner, KS_::CThostFtdcExchangeRateField *pExchangeRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pExchangeRate, sizeof(KS_::CThostFtdcExchangeRateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+44, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQrySecAgentACIDMap( HKSTRD Owner, KS_::CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pSecAgentACIDMap, sizeof(KS_::CThostFtdcSecAgentACIDMapField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+45, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryOptionInstrTradeCost( HKSTRD Owner, KS_::CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pOptionInstrTradeCost, sizeof(KS_::CThostFtdcOptionInstrTradeCostField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+46, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryOptionInstrCommRate( HKSTRD Owner, KS_::CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pOptionInstrCommRate, sizeof(KS_::CThostFtdcOptionInstrCommRateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+47, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryExecOrder( HKSTRD Owner, KS_::CThostFtdcExecOrderField *pExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pExecOrder, sizeof(KS_::CThostFtdcExecOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+48, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryForQuote( HKSTRD Owner, KS_::CThostFtdcForQuoteField *pForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pForQuote, sizeof(KS_::CThostFtdcForQuoteField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+49, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryQuote( HKSTRD Owner, KS_::CThostFtdcQuoteField *pQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pQuote, sizeof(KS_::CThostFtdcQuoteField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+50, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryTransferSerial( HKSTRD Owner, KS_::CThostFtdcTransferSerialField *pTransferSerial, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTransferSerial, sizeof(KS_::CThostFtdcTransferSerialField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+51, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryAccountregister( HKSTRD Owner, KS_::CThostFtdcAccountregisterField *pAccountregister, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pAccountregister, sizeof(KS_::CThostFtdcAccountregisterField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+52, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspError( HKSTRD Owner, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+53, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnOrder( HKSTRD Owner, KS_::CThostFtdcOrderField *pOrder )
{
	TCTPParams* Params = new TCTPParams( (void*)pOrder, sizeof(KS_::CThostFtdcOrderField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+54, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnTrade( HKSTRD Owner, KS_::CThostFtdcTradeField *pTrade )
{
	TCTPParams* Params = new TCTPParams( (void*)pTrade, sizeof(KS_::CThostFtdcTradeField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+55, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnErrRtnOrderInsert( HKSTRD Owner, KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputOrder, sizeof(KS_::CThostFtdcInputOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+56, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnErrRtnOrderAction( HKSTRD Owner, KS_::CThostFtdcOrderActionField *pOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pOrderAction, sizeof(KS_::CThostFtdcOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+57, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnInstrumentStatus( HKSTRD Owner, KS_::CThostFtdcInstrumentStatusField *pInstrumentStatus )
{
	TCTPParams* Params = new TCTPParams( (void*)pInstrumentStatus, sizeof(KS_::CThostFtdcInstrumentStatusField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+58, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnTradingNotice( HKSTRD Owner, KS_::CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pTradingNoticeInfo, sizeof(KS_::CThostFtdcTradingNoticeInfoField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+59, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnErrorConditionalOrder( HKSTRD Owner, KS_::CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder )
{
	TCTPParams* Params = new TCTPParams( (void*)pErrorConditionalOrder, sizeof(KS_::CThostFtdcErrorConditionalOrderField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+60, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnExecOrder( HKSTRD Owner, KS_::CThostFtdcExecOrderField *pExecOrder )
{
	TCTPParams* Params = new TCTPParams( (void*)pExecOrder, sizeof(KS_::CThostFtdcExecOrderField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+61, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnErrRtnExecOrderInsert( HKSTRD Owner, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputExecOrder, sizeof(KS_::CThostFtdcInputExecOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+62, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnErrRtnExecOrderAction( HKSTRD Owner, KS_::CThostFtdcExecOrderActionField *pExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pExecOrderAction, sizeof(KS_::CThostFtdcExecOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+63, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnErrRtnForQuoteInsert( HKSTRD Owner, KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputForQuote, sizeof(KS_::CThostFtdcInputForQuoteField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+64, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnQuote( HKSTRD Owner, KS_::CThostFtdcQuoteField *pQuote )
{
	TCTPParams* Params = new TCTPParams( (void*)pQuote, sizeof(KS_::CThostFtdcQuoteField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+65, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnErrRtnQuoteInsert( HKSTRD Owner, KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputQuote, sizeof(KS_::CThostFtdcInputQuoteField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+66, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnErrRtnQuoteAction( HKSTRD Owner, KS_::CThostFtdcQuoteActionField *pQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pQuoteAction, sizeof(KS_::CThostFtdcQuoteActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+67, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnForQuoteRsp( HKSTRD Owner, KS_::CThostFtdcForQuoteRspField *pForQuoteRsp )
{
	TCTPParams* Params = new TCTPParams( (void*)pForQuoteRsp, sizeof(KS_::CThostFtdcForQuoteRspField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+68, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnCFMMCTradingAccountToken( HKSTRD Owner, KS_::CThostFtdcCFMMCTradingAccountTokenField *pCFMMCTradingAccountToken )
{
	TCTPParams* Params = new TCTPParams( (void*)pCFMMCTradingAccountToken, sizeof(KS_::CThostFtdcCFMMCTradingAccountTokenField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+69, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryContractBank( HKSTRD Owner, KS_::CThostFtdcContractBankField *pContractBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pContractBank, sizeof(KS_::CThostFtdcContractBankField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+70, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryParkedOrder( HKSTRD Owner, KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pParkedOrder, sizeof(KS_::CThostFtdcParkedOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+71, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryParkedOrderAction( HKSTRD Owner, KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pParkedOrderAction, sizeof(KS_::CThostFtdcParkedOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+72, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryTradingNotice( HKSTRD Owner, KS_::CThostFtdcTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTradingNotice, sizeof(KS_::CThostFtdcTradingNoticeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+73, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryBrokerTradingParams( HKSTRD Owner, KS_::CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pBrokerTradingParams, sizeof(KS_::CThostFtdcBrokerTradingParamsField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+74, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryBrokerTradingAlgos( HKSTRD Owner, KS_::CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pBrokerTradingAlgos, sizeof(KS_::CThostFtdcBrokerTradingAlgosField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+75, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQueryCFMMCTradingAccountToken( HKSTRD Owner, KS_::CThostFtdcQueryCFMMCTradingAccountTokenField *pQueryCFMMCTradingAccountToken, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pQueryCFMMCTradingAccountToken, sizeof(KS_::CThostFtdcQueryCFMMCTradingAccountTokenField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+76, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnFromBankToFutureByBank( HKSTRD Owner, KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspTransfer, sizeof(KS_::CThostFtdcRspTransferField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+77, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnFromFutureToBankByBank( HKSTRD Owner, KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspTransfer, sizeof(KS_::CThostFtdcRspTransferField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+78, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnRepealFromBankToFutureByBank( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspRepeal, sizeof(KS_::CThostFtdcRspRepealField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+79, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnRepealFromFutureToBankByBank( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspRepeal, sizeof(KS_::CThostFtdcRspRepealField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+80, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnFromBankToFutureByFuture( HKSTRD Owner, KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspTransfer, sizeof(KS_::CThostFtdcRspTransferField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+81, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnFromFutureToBankByFuture( HKSTRD Owner, KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspTransfer, sizeof(KS_::CThostFtdcRspTransferField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+82, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnRepealFromBankToFutureByFutureManual( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspRepeal, sizeof(KS_::CThostFtdcRspRepealField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+83, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnRepealFromFutureToBankByFutureManual( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspRepeal, sizeof(KS_::CThostFtdcRspRepealField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+84, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnQueryBankBalanceByFuture( HKSTRD Owner, KS_::CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount )
{
	TCTPParams* Params = new TCTPParams( (void*)pNotifyQueryAccount, sizeof(KS_::CThostFtdcNotifyQueryAccountField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+85, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnErrRtnBankToFutureByFuture( HKSTRD Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqTransfer, sizeof(KS_::CThostFtdcReqTransferField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+86, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnErrRtnFutureToBankByFuture( HKSTRD Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqTransfer, sizeof(KS_::CThostFtdcReqTransferField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+87, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnErrRtnRepealBankToFutureByFutureManual( HKSTRD Owner, KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqRepeal, sizeof(KS_::CThostFtdcReqRepealField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+88, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnErrRtnRepealFutureToBankByFutureManual( HKSTRD Owner, KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqRepeal, sizeof(KS_::CThostFtdcReqRepealField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+89, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnErrRtnQueryBankBalanceByFuture( HKSTRD Owner, KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqQueryAccount, sizeof(KS_::CThostFtdcReqQueryAccountField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+90, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnRepealFromBankToFutureByFuture( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspRepeal, sizeof(KS_::CThostFtdcRspRepealField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+91, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnRepealFromFutureToBankByFuture( HKSTRD Owner, KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspRepeal, sizeof(KS_::CThostFtdcRspRepealField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+92, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspFromBankToFutureByFuture( HKSTRD Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqTransfer, sizeof(KS_::CThostFtdcReqTransferField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+93, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspFromFutureToBankByFuture( HKSTRD Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqTransfer, sizeof(KS_::CThostFtdcReqTransferField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+94, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQueryBankAccountMoneyByFuture( HKSTRD Owner, KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqQueryAccount, sizeof(KS_::CThostFtdcReqQueryAccountField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+95, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnOpenAccountByBank( HKSTRD Owner, KS_::CThostFtdcOpenAccountField *pOpenAccount )
{
	TCTPParams* Params = new TCTPParams( (void*)pOpenAccount, sizeof(KS_::CThostFtdcOpenAccountField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+96, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnCancelAccountByBank( HKSTRD Owner, KS_::CThostFtdcCancelAccountField *pCancelAccount )
{
	TCTPParams* Params = new TCTPParams( (void*)pCancelAccount, sizeof(KS_::CThostFtdcCancelAccountField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+97, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnChangeAccountByBank( HKSTRD Owner, KS_::CThostFtdcChangeAccountField *pChangeAccount )
{
	TCTPParams* Params = new TCTPParams( (void*)pChangeAccount, sizeof(KS_::CThostFtdcChangeAccountField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+98, Params );
}
//------------------------------------------------------------------------------------------------------------------------
// v2.3.50212 // 20150410 by Tim Lin
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspLockInsert( HKSTRD Owner, KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///鎖定應答
{
	TCTPParams* Params = new TCTPParams( (void*)pInputLock, sizeof(KS_::CThostFtdcInputLockField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+100, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryLock( HKSTRD Owner, KS_::CThostFtdcLockField *pLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///請求查詢鎖定回應
{
	TCTPParams* Params = new TCTPParams( (void*)pLock, sizeof(KS_::CThostFtdcLockField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+101, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryLockPosition( HKSTRD Owner, KS_::CThostFtdcLockPositionField *pLockPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///請求查詢鎖定證券倉位回應
{
	TCTPParams* Params = new TCTPParams( (void*)pLockPosition, sizeof(KS_::CThostFtdcLockPositionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+102, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnLock( HKSTRD Owner, KS_::CThostFtdcLockField *pLock )///鎖定通知
{
	TCTPParams* Params = new TCTPParams( (void*)pLock, sizeof(KS_::CThostFtdcLockField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+103, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnErrRtnLockInsert( HKSTRD Owner, KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo )///鎖定錯誤通知
{
	TCTPParams* Params = new TCTPParams( (void*)pInputLock, sizeof(KS_::CThostFtdcInputLockField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+104, Params );
}
//------------------------------------------------------------------------------------------------------------------------
// ---- CKSOptionSpi ----
// v2.3.50212 // 20150410 by Tim Lin
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryIndexPrice( HKSTRD Owner, KS_::CKSQryIndexPriceField *pQryIndexPrice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///查詢股指現貨指數
{
	TCTPParams* Params = new TCTPParams( (void*)pQryIndexPrice, sizeof(KS_::CKSQryIndexPriceField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+121, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryOptionInstrGuard( HKSTRD Owner, KS_::CKSQryOptionInstrGuardField *pQryOptionInstrGuard, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///查詢期權合約保障系數
{
	TCTPParams* Params = new TCTPParams( (void*)pQryOptionInstrGuard, sizeof(KS_::CKSQryOptionInstrGuardField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+122, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryExecOrderVolume( HKSTRD Owner, KS_::CKSQryExecOrderVolumeField *pQryExecOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///查詢宣告數量請求
{
	TCTPParams* Params = new TCTPParams( (void*)pQryExecOrderVolume, sizeof(KS_::CKSQryExecOrderVolumeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+123, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryStockOptionAssignment( HKSTRD Owner, KS_::CKSQryStockOptionAssignmentField *pQryStockOptionAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///查詢個股行權指派信息
{
	TCTPParams* Params = new TCTPParams( (void*)pQryStockOptionAssignment, sizeof(KS_::CKSQryStockOptionAssignmentField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+124, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryInvestorTradeLevel( HKSTRD Owner, KS_::CKSQryInvestorTradeLevelField *pQryInvestorTradeLevel, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///查詢客戶交易級別
{
	TCTPParams* Params = new TCTPParams( (void*)pQryInvestorTradeLevel, sizeof(KS_::CKSQryInvestorTradeLevelField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+125, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryPurchaseLimitAmt( HKSTRD Owner, KS_::CKSQryPurchaseLimitAmtField *pQryPurchaseLimitAmt, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///查詢個股限購額度
{
	TCTPParams* Params = new TCTPParams( (void*)pQryPurchaseLimitAmt, sizeof(KS_::CKSQryPurchaseLimitAmtField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+126, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryPositionLimitVol( HKSTRD Owner, KS_::CKSQryPositionLimitVolField *pQryPositionLimitVol, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///查詢個股限倉額度
{
	TCTPParams* Params = new TCTPParams( (void*)pQryPositionLimitVol, sizeof(KS_::CKSQryPositionLimitVolField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+127, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryHistoryOrder( HKSTRD Owner, KS_::CKSQryHistoryOrderField *pQryHistoryOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///請求查詢個股歷史報單
{
	TCTPParams* Params = new TCTPParams( (void*)pQryHistoryOrder, sizeof(KS_::CKSQryHistoryOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+128, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryHistoryTrade( HKSTRD Owner, KS_::CKSQryHistoryTradeField *pQryHistoryTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///請求查詢個股歷史成交
{
	TCTPParams* Params = new TCTPParams( (void*)pQryHistoryTrade, sizeof(KS_::CKSQryHistoryTradeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+129, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryStockOptionHistoryAssignment( HKSTRD Owner, KS_::CKSQryHistoryAssignmentField *pQryHistoryAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///請求查詢個股歷史行權指派明細
{
	TCTPParams* Params = new TCTPParams( (void*)pQryHistoryAssignment, sizeof(KS_::CKSQryHistoryAssignmentField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+130, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryStockOptionDelivDetail( HKSTRD Owner, KS_::CKSQrySODelivDetailField *pQrySODelivDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///請求查詢個股行權交割明細
{
	TCTPParams* Params = new TCTPParams( (void*)pQrySODelivDetail, sizeof(KS_::CKSQrySODelivDetailField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+131, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspAutoExecOrderAction( HKSTRD Owner, KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///自動行權執行操作
{
	TCTPParams* Params = new TCTPParams( (void*)pAutoExecOrderAction, sizeof(KS_::CKSAutoExecOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+132, Params );
}
//------------------------------------------------------------------------------------------------------------------------
/*
void KStrdTraderApiInit::OnRspQryKSExecOrder( HKSTRD Owner, KS_::CThostFtdcKSExecOrderField *pKSExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pKSExecOrder, sizeof(KS_::CThostFtdcKSExecOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+121, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspKSExecOrderInsert( HKSTRD Owner, KS_::CThostFtdcInputKSExecOrderField *pInputKSExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputKSExecOrder, sizeof(KS_::CThostFtdcInputKSExecOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+122, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryExecOrderVolume( HKSTRD Owner, KS_::CKSExecOrderVolumeField *pExecOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pExecOrderVolume, sizeof(KS_::CKSExecOrderVolumeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+123, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQrySpotOptionParams( HKSTRD Owner, KS_::CKSSpotOptionParamsField *pExecOrderParams, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pExecOrderParams, sizeof(KS_::CKSSpotOptionParamsField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+124, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryOptionInsInfo( HKSTRD Owner, KS_::CKSOptionInsInfoField *pOptionInsInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pOptionInsInfo, sizeof(KS_::CKSOptionInsInfoField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+125, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryStockOptionInsCommRate( HKSTRD Owner, KS_::CKSStockOptionInsCommRateField *pStockOptionInsCommRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pStockOptionInsCommRate, sizeof(KS_::CKSStockOptionInsCommRateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+126, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryStockOptionInsMarginRate( HKSTRD Owner, KS_::CKSStockOptionInsMarginRateField *pStockOptionInsMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pStockOptionInsMarginRate, sizeof(KS_::CKSStockOptionInsMarginRateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+127, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryStockOptionAssignment( HKSTRD Owner, KS_::CKSStockOptionAssignmentField *pStockOptionAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pStockOptionAssignment, sizeof(KS_::CKSStockOptionAssignmentField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+128, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryInvestorTradeLevel( HKSTRD Owner, KS_::CKSInvestorTradeLevelField *pInvestorTradeLevel, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorTradeLevel, sizeof(KS_::CKSInvestorTradeLevelField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+129, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryPurchaseLimitAmt( HKSTRD Owner, KS_::CKSPurchaseLimitAmtField *pPurchaseLimitAmt, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pPurchaseLimitAmt, sizeof(KS_::CKSPurchaseLimitAmtField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+130, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryPositionLimitVol( HKSTRD Owner, KS_::CKSPositionLimitVolField *pPositionLimitVol, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pPositionLimitVol, sizeof(KS_::CKSPositionLimitVolField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+131, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryHistoryOrder( HKSTRD Owner, KS_::CKSHistoryOrderField *pOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pOrder, sizeof(KS_::CKSHistoryOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+132, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryHistoryTrade( HKSTRD Owner, KS_::CKSHistoryTradeField *pTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTrade, sizeof(KS_::CKSHistoryTradeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+133, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryStockOptionHistoryAssignment( HKSTRD Owner, KS_::CKSHistoryAssignmentField *pHistoryAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pHistoryAssignment, sizeof(KS_::CKSHistoryAssignmentField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+134, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryStockOptionDelivDetail( HKSTRD Owner, KS_::CKSSODelivDetailField *pSODelivDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pSODelivDetail, sizeof(KS_::CKSSODelivDetailField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+135, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspAutoExecOrderAction( HKSTRD Owner, KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pAutoExecOrderAction, sizeof(KS_::CKSAutoExecOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+136, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnKSExecOrder( HKSTRD Owner, KS_::CThostFtdcKSExecOrderRtnField *pExecOrder )
{
	TCTPParams* Params = new TCTPParams( (void*)pExecOrder, sizeof(KS_::CThostFtdcKSExecOrderRtnField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+137, Params );
}
*/
//------------------------------------------------------------------------------------------------------------------------
// ---- CKSVocSpi ----
// v2.3.50212 // 20150410 by Tim Lin
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryInvestorOpenPosition( HKSTRD Owner, KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorPositionDetail, sizeof(KS_::CThostFtdcInvestorPositionDetailField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+161, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryInvestorOpenCombinePosition( HKSTRD Owner, KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorPositionCombineDetail, sizeof(KS_::CThostFtdcInvestorPositionCombineDetailField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+162, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspBulkCancelOrder( HKSTRD Owner, KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pBulkCancelOrder, sizeof(KS_::CThostFtdcBulkCancelOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+163, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryCloseStrategy( HKSTRD Owner, KS_::CKSCloseStrategyResultField *pCloseStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pCloseStrategy, sizeof(KS_::CKSCloseStrategyResultField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+164, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryCombStrategy( HKSTRD Owner, KS_::CKSCombStrategyResultField *pCombStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pCombStrategy, sizeof(KS_::CKSCombStrategyResultField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+165, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryOptionCombStrategy( HKSTRD Owner, KS_::CKSOptionCombStrategyResultField *pOptionCombStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pOptionCombStrategy, sizeof(KS_::CKSOptionCombStrategyResultField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+166, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryTransferInfo( HKSTRD Owner, KS_::CKSTransferInfoResultField *pResultField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pResultField, sizeof(KS_::CKSTransferInfoResultField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+167, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryKSTradingNotice( HKSTRD Owner, KS_::CKSTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTradingNotice, sizeof(KS_::CKSTradingNoticeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+168, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryUserProductUrl( HKSTRD Owner, KS_::CKSUserProductUrlField *pUserProductUrl, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pUserProductUrl, sizeof(KS_::CKSUserProductUrlField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+169, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryMaxCombActionVolume( HKSTRD Owner, KS_::CKSMaxCombActionVolumeField *pMaxCombActionVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pMaxCombActionVolume, sizeof(KS_::CKSMaxCombActionVolumeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+170, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnKSTradingNotice( HKSTRD Owner, KS_::CKSTradingNoticeField *pTradingNoticeInfo)
{
	TCTPParams* Params = new TCTPParams( (void*)pTradingNoticeInfo, sizeof(KS_::CKSTradingNoticeField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+171, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryKSInstrumentMarginRate( HKSTRD Owner, KS_::CKSInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TCTPParams* Params = new TCTPParams( (void*)pInstrumentMarginRate, sizeof(KS_::CKSInstrumentMarginRateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+172, Params );
}
//------------------------------------------------------------------------------------------------------------------------
/*
void KStrdTraderApiInit::OnRspQryCrossRate( HKSTRD Owner, KS_::CKSCrossRateField *pCrossRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pCrossRate, sizeof(KS_::CKSCrossRateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+170, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspMatchOrderInsert( HKSTRD Owner, KS_::CKSMatchOrderInsertField *pMatchOrderInsert, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pMatchOrderInsert, sizeof(KS_::CKSMatchOrderInsertField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+171, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryMatchOrder( HKSTRD Owner, KS_::CKSMatchOrderField *pMatchOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pMatchOrder, sizeof(KS_::CKSMatchOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+172, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRspQryMaxMatchOrderVolume( HKSTRD Owner, KS_::CKSMaxMatchOrderVolumeField *pMaxMatchOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pMaxMatchOrderVolume, sizeof(KS_::CKSMaxMatchOrderVolumeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+173, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnKSTradingNotice( HKSTRD Owner, KS_::CKSTradingNoticeField *pTradingNoticeInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pTradingNoticeInfo, sizeof(KS_::CKSTradingNoticeField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+174, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdTraderApiInit::OnRtnMatchOrder( HKSTRD Owner, KS_::CKSMatchOrderField *pMatchOrder )
{
	TCTPParams* Params = new TCTPParams( (void*)pMatchOrder, sizeof(KS_::CKSMatchOrderField) );

	TriggerKStrdEvent( Owner, WM_KSTRD_EVENT+175, Params );
}
*/
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
