//---------------------------------------------------------------------------
#ifdef _CTP
//------------------------------------------------------------------------------------------------------------------------
#ifdef _KSFT
//------------------------------------------------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "KSftBCBTraderAPI.h"
#include "KSftBCBEvents.h"
#include "KSftAPIDynamic.h"
#include "APIParams.h"
//------------------------------------------------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------------------------------------------------
extern HINSTANCE gKSftDLLInstance;
//------------------------------------------------------------------------------------------------------------------------
bool TriggerKSftEvent( HKSFT Owner, int MSG, TCTPParams* Params )
{
	TKSftTraderApi* KSftObj = (TKSftTraderApi*)Owner;
	if ( KSftObj == NULL )
		return false;
	TKSftTraderSpi* Listener = KSftObj->GetSpi( );
	if ( Listener == NULL )
		return false;

	PostMessage( KSftObj->GetHWND(), MSG, (WPARAM)Listener, (LPARAM)Params );
	return true;
}
//------------------------------------------------------------------------------------------------------------------------
KSftTraderApiInit::KSftTraderApiInit()
{
	// 1. Load KSftAPIFactory.dll
	gKSftDLLInstance = ::LoadLibraryW( L"KSftAPIFactory.dll" );
	if ( gKSftDLLInstance == NULL )
	{
		sprintf( FNotSupportReason, "Can't dynamic load KSftAPIFactory.dll" );
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
	sprintf( FNotSupportReason, "Load KSftAPIFactory.dll OK!" );
	FSupport = true;
}
//------------------------------------------------------------------------------------------------------------------------
KSftTraderApiInit::~KSftTraderApiInit()
{
}
//------------------------------------------------------------------------------------------------------------------------
int KSftTraderApiInit::InitFunctions( void )
{
	if ( ( pCreateKSftTraderApi                 = (lpCreateKSftTraderApi                 )::GetProcAddress( gKSftDLLInstance, "CreateKSftTraderApi" ) ) == NULL )
		return 1;
	if ( ( pReleaseKSftTraderApi                = (lpReleaseKSftTraderApi                )::GetProcAddress( gKSftDLLInstance, "ReleaseKSftTraderApi" ) ) == NULL )
		return 2;
	if ( ( pInit                                = (lpInit                                )::GetProcAddress( gKSftDLLInstance, "Init" ) ) == NULL )
		return 3;
	if ( ( pGetTradingDay                       = (lpGetTradingDay                       )::GetProcAddress( gKSftDLLInstance, "GetTradingDay" ) ) == NULL )
		return 4;
	if ( ( pRegisterFront                       = (lpRegisterFront                       )::GetProcAddress( gKSftDLLInstance, "RegisterFront" ) ) == NULL )
		return 5;
	if ( ( pRegisterNameServer                  = (lpRegisterNameServer                  )::GetProcAddress( gKSftDLLInstance, "RegisterNameServer" ) ) == NULL )
		return 6;
	if ( ( pRegisterFensUserInfo                = (lpRegisterFensUserInfo                )::GetProcAddress( gKSftDLLInstance, "RegisterFensUserInfo" ) ) == NULL )
		return 7;
	if ( ( pSubscribePrivateTopic               = (lpSubscribePrivateTopic               )::GetProcAddress( gKSftDLLInstance, "SubscribePrivateTopic" ) ) == NULL )
		return 8;
	if ( ( pSubscribePublicTopic                = (lpSubscribePublicTopic                )::GetProcAddress( gKSftDLLInstance, "SubscribePublicTopic" ) ) == NULL )
		return 9;
	if ( ( pReqAuthenticate                     = (lpReqAuthenticate                     )::GetProcAddress( gKSftDLLInstance, "ReqAuthenticate" ) ) == NULL )
		return 10;
	if ( ( pReqUserLogin                        = (lpReqUserLogin                        )::GetProcAddress( gKSftDLLInstance, "ReqUserLogin" ) ) == NULL )
		return 11;
	if ( ( pReqUserLogout                       = (lpReqUserLogout                       )::GetProcAddress( gKSftDLLInstance, "ReqUserLogout" ) ) == NULL )
		return 12;
	if ( ( pReqUserPasswordUpdate               = (lpReqUserPasswordUpdate               )::GetProcAddress( gKSftDLLInstance, "ReqUserPasswordUpdate" ) ) == NULL )
		return 13;
	if ( ( pReqTradingAccountPasswordUpdate     = (lpReqTradingAccountPasswordUpdate     )::GetProcAddress( gKSftDLLInstance, "ReqTradingAccountPasswordUpdate" ) ) == NULL )
		return 14;
	if ( ( pReqOrderInsert                      = (lpReqOrderInsert                      )::GetProcAddress( gKSftDLLInstance, "ReqOrderInsert" ) ) == NULL )
		return 15;
	if ( ( pReqParkedOrderInsert                = (lpReqParkedOrderInsert                )::GetProcAddress( gKSftDLLInstance, "ReqParkedOrderInsert" ) ) == NULL )
		return 16;
	if ( ( pReqParkedOrderAction                = (lpReqParkedOrderAction                )::GetProcAddress( gKSftDLLInstance, "ReqParkedOrderAction" ) ) == NULL )
		return 17;
	if ( ( pReqOrderAction                      = (lpReqOrderAction                      )::GetProcAddress( gKSftDLLInstance, "ReqOrderAction" ) ) == NULL )
		return 18;
	if ( ( pReqQueryMaxOrderVolume              = (lpReqQueryMaxOrderVolume              )::GetProcAddress( gKSftDLLInstance, "ReqQueryMaxOrderVolume" ) ) == NULL )
		return 19;
	if ( ( pReqSettlementInfoConfirm            = (lpReqSettlementInfoConfirm            )::GetProcAddress( gKSftDLLInstance, "ReqSettlementInfoConfirm" ) ) == NULL )
		return 20;
	if ( ( pReqRemoveParkedOrder                = (lpReqRemoveParkedOrder                )::GetProcAddress( gKSftDLLInstance, "ReqRemoveParkedOrder" ) ) == NULL )
		return 21;
	if ( ( pReqRemoveParkedOrderAction          = (lpReqRemoveParkedOrderAction          )::GetProcAddress( gKSftDLLInstance, "ReqRemoveParkedOrderAction" ) ) == NULL )
		return 22;
	if ( ( pReqExecOrderInsert                  = (lpReqExecOrderInsert                  )::GetProcAddress( gKSftDLLInstance, "ReqExecOrderInsert" ) ) == NULL )
		return 23;
	if ( ( pReqExecOrderAction                  = (lpReqExecOrderAction                  )::GetProcAddress( gKSftDLLInstance, "ReqExecOrderAction" ) ) == NULL )
		return 24;
	if ( ( pReqForQuoteInsert                   = (lpReqForQuoteInsert                   )::GetProcAddress( gKSftDLLInstance, "ReqForQuoteInsert" ) ) == NULL )
		return 25;
	if ( ( pReqQuoteInsert                      = (lpReqQuoteInsert                      )::GetProcAddress( gKSftDLLInstance, "ReqQuoteInsert" ) ) == NULL )
		return 26;
	if ( ( pReqQuoteAction                      = (lpReqQuoteAction                      )::GetProcAddress( gKSftDLLInstance, "ReqQuoteAction" ) ) == NULL )
		return 27;
	if ( ( pReqQryOrder                         = (lpReqQryOrder                         )::GetProcAddress( gKSftDLLInstance, "ReqQryOrder" ) ) == NULL )
		return 28;
	if ( ( pReqQryTrade                         = (lpReqQryTrade                         )::GetProcAddress( gKSftDLLInstance, "ReqQryTrade" ) ) == NULL )
		return 29;
	if ( ( pReqQryInvestorPosition              = (lpReqQryInvestorPosition              )::GetProcAddress( gKSftDLLInstance, "ReqQryInvestorPosition" ) ) == NULL )
		return 30;
	if ( ( pReqQryTradingAccount                = (lpReqQryTradingAccount                )::GetProcAddress( gKSftDLLInstance, "ReqQryTradingAccount" ) ) == NULL )
		return 31;
	if ( ( pReqQryInvestor                      = (lpReqQryInvestor                      )::GetProcAddress( gKSftDLLInstance, "ReqQryInvestor" ) ) == NULL )
		return 32;
	if ( ( pReqQryTradingCode                   = (lpReqQryTradingCode                   )::GetProcAddress( gKSftDLLInstance, "ReqQryTradingCode" ) ) == NULL )
		return 33;
	if ( ( pReqQryInstrumentMarginRate          = (lpReqQryInstrumentMarginRate          )::GetProcAddress( gKSftDLLInstance, "ReqQryInstrumentMarginRate" ) ) == NULL )
		return 34;
	if ( ( pReqQryInstrumentCommissionRate      = (lpReqQryInstrumentCommissionRate      )::GetProcAddress( gKSftDLLInstance, "ReqQryInstrumentCommissionRate" ) ) == NULL )
		return 35;
	if ( ( pReqQryExchange                      = (lpReqQryExchange                      )::GetProcAddress( gKSftDLLInstance, "ReqQryExchange" ) ) == NULL )
		return 36;
	if ( ( pReqQryProduct                       = (lpReqQryProduct                       )::GetProcAddress( gKSftDLLInstance, "ReqQryProduct" ) ) == NULL )
		return 37;
	if ( ( pReqQryInstrument                    = (lpReqQryInstrument                    )::GetProcAddress( gKSftDLLInstance, "ReqQryInstrument" ) ) == NULL )
		return 38;
	if ( ( pReqQryDepthMarketData               = (lpReqQryDepthMarketData               )::GetProcAddress( gKSftDLLInstance, "ReqQryDepthMarketData" ) ) == NULL )
		return 39;
	if ( ( pReqQrySettlementInfo                = (lpReqQrySettlementInfo                )::GetProcAddress( gKSftDLLInstance, "ReqQrySettlementInfo" ) ) == NULL )
		return 40;
	if ( ( pReqQryTransferBank                  = (lpReqQryTransferBank                  )::GetProcAddress( gKSftDLLInstance, "ReqQryTransferBank" ) ) == NULL )
		return 41;
	if ( ( pReqQryInvestorPositionDetail        = (lpReqQryInvestorPositionDetail        )::GetProcAddress( gKSftDLLInstance, "ReqQryInvestorPositionDetail" ) ) == NULL )
		return 42;
	if ( ( pReqQryNotice                        = (lpReqQryNotice                        )::GetProcAddress( gKSftDLLInstance, "ReqQryNotice" ) ) == NULL )
		return 43;
	if ( ( pReqQrySettlementInfoConfirm         = (lpReqQrySettlementInfoConfirm         )::GetProcAddress( gKSftDLLInstance, "ReqQrySettlementInfoConfirm" ) ) == NULL )
		return 44;
	if ( ( pReqQryInvestorPositionCombineDetail = (lpReqQryInvestorPositionCombineDetail )::GetProcAddress( gKSftDLLInstance, "ReqQryInvestorPositionCombineDetail" ) ) == NULL )
		return 45;
	if ( ( pReqQryCFMMCTradingAccountKey        = (lpReqQryCFMMCTradingAccountKey        )::GetProcAddress( gKSftDLLInstance, "ReqQryCFMMCTradingAccountKey" ) ) == NULL )
		return 46;
	if ( ( pReqQryEWarrantOffset                = (lpReqQryEWarrantOffset                )::GetProcAddress( gKSftDLLInstance, "ReqQryEWarrantOffset" ) ) == NULL )
		return 47;
	if ( ( pReqQryInvestorProductGroupMargin    = (lpReqQryInvestorProductGroupMargin    )::GetProcAddress( gKSftDLLInstance, "ReqQryInvestorProductGroupMargin" ) ) == NULL )
		return 48;
	if ( ( pReqQryExchangeMarginRate            = (lpReqQryExchangeMarginRate            )::GetProcAddress( gKSftDLLInstance, "ReqQryExchangeMarginRate" ) ) == NULL )
		return 49;
	if ( ( pReqQryExchangeMarginRateAdjust      = (lpReqQryExchangeMarginRateAdjust      )::GetProcAddress( gKSftDLLInstance, "ReqQryExchangeMarginRateAdjust" ) ) == NULL )
		return 50;
	if ( ( pReqQryExchangeRate                  = (lpReqQryExchangeRate                  )::GetProcAddress( gKSftDLLInstance, "ReqQryExchangeRate" ) ) == NULL )
		return 51;
	if ( ( pReqQrySecAgentACIDMap               = (lpReqQrySecAgentACIDMap               )::GetProcAddress( gKSftDLLInstance, "ReqQrySecAgentACIDMap" ) ) == NULL )
		return 52;
	if ( ( pReqQryOptionInstrTradeCost          = (lpReqQryOptionInstrTradeCost          )::GetProcAddress( gKSftDLLInstance, "ReqQryOptionInstrTradeCost" ) ) == NULL )
		return 53;
	if ( ( pReqQryOptionInstrCommRate           = (lpReqQryOptionInstrCommRate           )::GetProcAddress( gKSftDLLInstance, "ReqQryOptionInstrCommRate" ) ) == NULL )
		return 54;
	if ( ( pReqQryExecOrder                     = (lpReqQryExecOrder                     )::GetProcAddress( gKSftDLLInstance, "ReqQryExecOrder" ) ) == NULL )
		return 55;
	if ( ( pReqQryForQuote                      = (lpReqQryForQuote                      )::GetProcAddress( gKSftDLLInstance, "ReqQryForQuote" ) ) == NULL )
		return 56;
	if ( ( pReqQryQuote                         = (lpReqQryQuote                         )::GetProcAddress( gKSftDLLInstance, "ReqQryQuote" ) ) == NULL )
		return 57;
	if ( ( pReqQryTransferSerial                = (lpReqQryTransferSerial                )::GetProcAddress( gKSftDLLInstance, "ReqQryTransferSerial" ) ) == NULL )
		return 58;
	if ( ( pReqQryAccountregister               = (lpReqQryAccountregister               )::GetProcAddress( gKSftDLLInstance, "ReqQryAccountregister" ) ) == NULL )
		return 59;
	if ( ( pReqQryContractBank                  = (lpReqQryContractBank                  )::GetProcAddress( gKSftDLLInstance, "ReqQryContractBank" ) ) == NULL )
		return 60;
	if ( ( pReqQryParkedOrder                   = (lpReqQryParkedOrder                   )::GetProcAddress( gKSftDLLInstance, "ReqQryParkedOrder" ) ) == NULL )
		return 61;
	if ( ( pReqQryParkedOrderAction             = (lpReqQryParkedOrderAction             )::GetProcAddress( gKSftDLLInstance, "ReqQryParkedOrderAction" ) ) == NULL )
		return 62;
	if ( ( pReqQryTradingNotice                 = (lpReqQryTradingNotice                 )::GetProcAddress( gKSftDLLInstance, "ReqQryTradingNotice" ) ) == NULL )
		return 63;
	if ( ( pReqQryBrokerTradingParams           = (lpReqQryBrokerTradingParams           )::GetProcAddress( gKSftDLLInstance, "ReqQryBrokerTradingParams" ) ) == NULL )
		return 64;
	if ( ( pReqQryBrokerTradingAlgos            = (lpReqQryBrokerTradingAlgos            )::GetProcAddress( gKSftDLLInstance, "ReqQryBrokerTradingAlgos" ) ) == NULL )
		return 65;
	if ( ( pReqFromBankToFutureByFuture         = (lpReqFromBankToFutureByFuture         )::GetProcAddress( gKSftDLLInstance, "ReqFromBankToFutureByFuture" ) ) == NULL )
		return 66;
	if ( ( pReqFromFutureToBankByFuture         = (lpReqFromFutureToBankByFuture         )::GetProcAddress( gKSftDLLInstance, "ReqFromFutureToBankByFuture" ) ) == NULL )
		return 67;
	if ( ( pReqQueryBankAccountMoneyByFuture    = (lpReqQueryBankAccountMoneyByFuture    )::GetProcAddress( gKSftDLLInstance, "ReqQueryBankAccountMoneyByFuture" ) ) == NULL )
		return 68;
	if ( ( pReqQueryInvestorOpenPosition        = (lpReqQueryInvestorOpenPosition        )::GetProcAddress( gKSftDLLInstance, "ReqQueryInvestorOpenPosition" ) ) == NULL )
		return 69;
	if ( ( pReqQueryInvestorOpenCombinePosition = (lpReqQueryInvestorOpenCombinePosition )::GetProcAddress( gKSftDLLInstance, "ReqQueryInvestorOpenCombinePosition" ) ) == NULL )
		return 70;
	if ( ( pLoadExtApi                          = (lpLoadExtApi                          )::GetProcAddress( gKSftDLLInstance, "LoadExtApi" ) ) == NULL )
		return 71;
	if ( ( pReqBulkCancelOrder                  = (lpReqBulkCancelOrder                  )::GetProcAddress( gKSftDLLInstance, "ReqBulkCancelOrder" ) ) == NULL )
		return 72;

	return 0;
}
//------------------------------------------------------------------------------------------------------------------------
int KSftTraderApiInit::InitRegCallbackFunctions( void )
{
	if ( ( pRegOnFrontConnected                          = (lpRegOnFrontConnected                         )::GetProcAddress( gKSftDLLInstance, "RegOnFrontConnected" ) ) == NULL )
		return 1;
	if ( ( pRegOnFrontDisconnected                       = (lpRegOnFrontDisconnected                      )::GetProcAddress( gKSftDLLInstance, "RegOnFrontDisconnected" ) ) == NULL )
		return 2;
	if ( ( pRegOnHeartBeatWarning                        = (lpRegOnHeartBeatWarning                       )::GetProcAddress( gKSftDLLInstance, "RegOnHeartBeatWarning" ) ) == NULL )
		return 3;
	if ( ( pRegOnRspAuthenticate                         = (lpRegOnRspAuthenticate                        )::GetProcAddress( gKSftDLLInstance, "RegOnRspAuthenticate" ) ) == NULL )
		return 4;
	if ( ( pRegOnRspUserLogin                            = (lpRegOnRspUserLogin                           )::GetProcAddress( gKSftDLLInstance, "RegOnRspUserLogin" ) ) == NULL )
		return 5;
	if ( ( pRegOnRspUserLogout                           = (lpRegOnRspUserLogout                          )::GetProcAddress( gKSftDLLInstance, "RegOnRspUserLogout" ) ) == NULL )
		return 6;
	if ( ( pRegOnRspUserPasswordUpdate                   = (lpRegOnRspUserPasswordUpdate                  )::GetProcAddress( gKSftDLLInstance, "RegOnRspUserPasswordUpdate" ) ) == NULL )
		return 7;
	if ( ( pRegOnRspTradingAccountPasswordUpdate         = (lpRegOnRspTradingAccountPasswordUpdate        )::GetProcAddress( gKSftDLLInstance, "RegOnRspTradingAccountPasswordUpdate" ) ) == NULL )
		return 8;
	if ( ( pRegOnRspOrderInsert                          = (lpRegOnRspOrderInsert                         )::GetProcAddress( gKSftDLLInstance, "RegOnRspOrderInsert" ) ) == NULL )
		return 9;
	if ( ( pRegOnRspParkedOrderInsert                    = (lpRegOnRspParkedOrderInsert                   )::GetProcAddress( gKSftDLLInstance, "RegOnRspParkedOrderInsert" ) ) == NULL )
		return 10;
	if ( ( pRegOnRspParkedOrderAction                    = (lpRegOnRspParkedOrderAction                   )::GetProcAddress( gKSftDLLInstance, "RegOnRspParkedOrderAction" ) ) == NULL )
		return 11;
	if ( ( pRegOnRspOrderAction                          = (lpRegOnRspOrderAction                         )::GetProcAddress( gKSftDLLInstance, "RegOnRspOrderAction" ) ) == NULL )
		return 12;
	if ( ( pRegOnRspQueryMaxOrderVolume                  = (lpRegOnRspQueryMaxOrderVolume                 )::GetProcAddress( gKSftDLLInstance, "RegOnRspQueryMaxOrderVolume" ) ) == NULL )
		return 13;
	if ( ( pRegOnRspSettlementInfoConfirm                = (lpRegOnRspSettlementInfoConfirm               )::GetProcAddress( gKSftDLLInstance, "RegOnRspSettlementInfoConfirm" ) ) == NULL )
		return 14;
	if ( ( pRegOnRspRemoveParkedOrder                    = (lpRegOnRspRemoveParkedOrder                   )::GetProcAddress( gKSftDLLInstance, "RegOnRspRemoveParkedOrder" ) ) == NULL )
		return 15;
	if ( ( pRegOnRspRemoveParkedOrderAction              = (lpRegOnRspRemoveParkedOrderAction             )::GetProcAddress( gKSftDLLInstance, "RegOnRspRemoveParkedOrderAction" ) ) == NULL )
		return 16;
	if ( ( pRegOnRspExecOrderInsert                      = (lpRegOnRspExecOrderInsert                     )::GetProcAddress( gKSftDLLInstance, "RegOnRspExecOrderInsert" ) ) == NULL )
		return 17;
	if ( ( pRegOnRspExecOrderAction                      = (lpRegOnRspExecOrderAction                     )::GetProcAddress( gKSftDLLInstance, "RegOnRspExecOrderAction" ) ) == NULL )
		return 18;
	if ( ( pRegOnRspForQuoteInsert                       = (lpRegOnRspForQuoteInsert                      )::GetProcAddress( gKSftDLLInstance, "RegOnRspForQuoteInsert" ) ) == NULL )
		return 19;
	if ( ( pRegOnRspQuoteInsert                          = (lpRegOnRspQuoteInsert                         )::GetProcAddress( gKSftDLLInstance, "RegOnRspQuoteInsert" ) ) == NULL )
		return 20;
	if ( ( pRegOnRspQuoteAction                          = (lpRegOnRspQuoteAction                         )::GetProcAddress( gKSftDLLInstance, "RegOnRspQuoteAction" ) ) == NULL )
		return 21;
	if ( ( pRegOnRspQryOrder                             = (lpRegOnRspQryOrder                            )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryOrder" ) ) == NULL )
		return 22;
	if ( ( pRegOnRspQryTrade                             = (lpRegOnRspQryTrade                            )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryTrade" ) ) == NULL )
		return 23;
	if ( ( pRegOnRspQryInvestorPosition                  = (lpRegOnRspQryInvestorPosition                 )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryInvestorPosition" ) ) == NULL )
		return 24;
	if ( ( pRegOnRspQryTradingAccount                    = (lpRegOnRspQryTradingAccount                   )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryTradingAccount" ) ) == NULL )
		return 25;
	if ( ( pRegOnRspQryInvestor                          = (lpRegOnRspQryInvestor                         )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryInvestor" ) ) == NULL )
		return 26;
	if ( ( pRegOnRspQryTradingCode                       = (lpRegOnRspQryTradingCode                      )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryTradingCode" ) ) == NULL )
		return 27;
	if ( ( pRegOnRspQryInstrumentMarginRate              = (lpRegOnRspQryInstrumentMarginRate             )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryInstrumentMarginRate" ) ) == NULL )
		return 28;
	if ( ( pRegOnRspQryInstrumentCommissionRate          = (lpRegOnRspQryInstrumentCommissionRate         )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryInstrumentCommissionRate" ) ) == NULL )
		return 29;
	if ( ( pRegOnRspQryExchange                          = (lpRegOnRspQryExchange                         )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryExchange" ) ) == NULL )
		return 30;
	if ( ( pRegOnRspQryProduct                           = (lpRegOnRspQryProduct                          )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryProduct" ) ) == NULL )
		return 31;
	if ( ( pRegOnRspQryInstrument                        = (lpRegOnRspQryInstrument                       )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryInstrument" ) ) == NULL )
		return 32;
	if ( ( pRegOnRspQryDepthMarketData                   = (lpRegOnRspQryDepthMarketData                  )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryDepthMarketData" ) ) == NULL )
		return 33;
	if ( ( pRegOnRspQrySettlementInfo                    = (lpRegOnRspQrySettlementInfo                   )::GetProcAddress( gKSftDLLInstance, "RegOnRspQrySettlementInfo" ) ) == NULL )
		return 34;
	if ( ( pRegOnRspQryTransferBank                      = (lpRegOnRspQryTransferBank                     )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryTransferBank" ) ) == NULL )
		return 35;
	if ( ( pRegOnRspQryInvestorPositionDetail            = (lpRegOnRspQryInvestorPositionDetail           )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryInvestorPositionDetail" ) ) == NULL )
		return 36;
	if ( ( pRegOnRspQryNotice                            = (lpRegOnRspQryNotice                           )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryNotice" ) ) == NULL )
		return 37;
	if ( ( pRegOnRspQrySettlementInfoConfirm             = (lpRegOnRspQrySettlementInfoConfirm            )::GetProcAddress( gKSftDLLInstance, "RegOnRspQrySettlementInfoConfirm" ) ) == NULL )
		return 38;
	if ( ( pRegOnRspQryInvestorPositionCombineDetail     = (lpRegOnRspQryInvestorPositionCombineDetail    )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryInvestorPositionCombineDetail" ) ) == NULL )
		return 39;
	if ( ( pRegOnRspQryCFMMCTradingAccountKey            = (lpRegOnRspQryCFMMCTradingAccountKey           )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryCFMMCTradingAccountKey" ) ) == NULL )
		return 40;
	if ( ( pRegOnRspQryEWarrantOffset                    = (lpRegOnRspQryEWarrantOffset                   )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryEWarrantOffset" ) ) == NULL )
		return 41;
	if ( ( pRegOnRspQryInvestorProductGroupMargin        = (lpRegOnRspQryInvestorProductGroupMargin       )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryInvestorProductGroupMargin" ) ) == NULL )
		return 42;
	if ( ( pRegOnRspQryExchangeMarginRate                = (lpRegOnRspQryExchangeMarginRate               )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryExchangeMarginRate" ) ) == NULL )
		return 43;
	if ( ( pRegOnRspQryExchangeMarginRateAdjust          = (lpRegOnRspQryExchangeMarginRateAdjust         )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryExchangeMarginRateAdjust" ) ) == NULL )
		return 44;
	if ( ( pRegOnRspQryExchangeRate                      = (lpRegOnRspQryExchangeRate                     )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryExchangeRate" ) ) == NULL )
		return 45;
	if ( ( pRegOnRspQrySecAgentACIDMap                   = (lpRegOnRspQrySecAgentACIDMap                  )::GetProcAddress( gKSftDLLInstance, "RegOnRspQrySecAgentACIDMap" ) ) == NULL )
		return 46;
	if ( ( pRegOnRspQryOptionInstrTradeCost              = (lpRegOnRspQryOptionInstrTradeCost             )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryOptionInstrTradeCost" ) ) == NULL )
		return 47;
	if ( ( pRegOnRspQryOptionInstrCommRate               = (lpRegOnRspQryOptionInstrCommRate              )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryOptionInstrCommRate" ) ) == NULL )
		return 48;
	if ( ( pRegOnRspQryExecOrder                         = (lpRegOnRspQryExecOrder                        )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryExecOrder" ) ) == NULL )
		return 49;
	if ( ( pRegOnRspQryForQuote                          = (lpRegOnRspQryForQuote                         )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryForQuote" ) ) == NULL )
		return 50;
	if ( ( pRegOnRspQryQuote                             = (lpRegOnRspQryQuote                            )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryQuote" ) ) == NULL )
		return 51;
	if ( ( pRegOnRspQryTransferSerial                    = (lpRegOnRspQryTransferSerial                   )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryTransferSerial" ) ) == NULL )
		return 52;
	if ( ( pRegOnRspQryAccountregister                   = (lpRegOnRspQryAccountregister                  )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryAccountregister" ) ) == NULL )
		return 53;
	if ( ( pRegOnRspError                                = (lpRegOnRspError                               )::GetProcAddress( gKSftDLLInstance, "RegOnRspError" ) ) == NULL )
		return 54;
	if ( ( pRegOnRtnOrder                                = (lpRegOnRtnOrder                               )::GetProcAddress( gKSftDLLInstance, "RegOnRtnOrder" ) ) == NULL )
		return 55;
	if ( ( pRegOnRtnTrade                                = (lpRegOnRtnTrade                               )::GetProcAddress( gKSftDLLInstance, "RegOnRtnTrade" ) ) == NULL )
		return 56;
	if ( ( pRegOnErrRtnOrderInsert                       = (lpRegOnErrRtnOrderInsert                      )::GetProcAddress( gKSftDLLInstance, "RegOnErrRtnOrderInsert" ) ) == NULL )
		return 57;
	if ( ( pRegOnErrRtnOrderAction                       = (lpRegOnErrRtnOrderAction                      )::GetProcAddress( gKSftDLLInstance, "RegOnErrRtnOrderAction" ) ) == NULL )
		return 58;
	if ( ( pRegOnRtnInstrumentStatus                     = (lpRegOnRtnInstrumentStatus                    )::GetProcAddress( gKSftDLLInstance, "RegOnRtnInstrumentStatus" ) ) == NULL )
		return 59;
	if ( ( pRegOnRtnTradingNotice                        = (lpRegOnRtnTradingNotice                       )::GetProcAddress( gKSftDLLInstance, "RegOnRtnTradingNotice" ) ) == NULL )
		return 60;
	if ( ( pRegOnRtnErrorConditionalOrder                = (lpRegOnRtnErrorConditionalOrder               )::GetProcAddress( gKSftDLLInstance, "RegOnRtnErrorConditionalOrder" ) ) == NULL )
		return 61;
	if ( ( pRegOnRtnExecOrder                            = (lpRegOnRtnExecOrder                           )::GetProcAddress( gKSftDLLInstance, "RegOnRtnExecOrder" ) ) == NULL )
		return 62;
	if ( ( pRegOnErrRtnExecOrderInsert                   = (lpRegOnErrRtnExecOrderInsert                  )::GetProcAddress( gKSftDLLInstance, "RegOnErrRtnExecOrderInsert" ) ) == NULL )
		return 63;
	if ( ( pRegOnErrRtnExecOrderAction                   = (lpRegOnErrRtnExecOrderAction                  )::GetProcAddress( gKSftDLLInstance, "RegOnErrRtnExecOrderAction" ) ) == NULL )
		return 64;
	if ( ( pRegOnErrRtnForQuoteInsert                    = (lpRegOnErrRtnForQuoteInsert                   )::GetProcAddress( gKSftDLLInstance, "RegOnErrRtnForQuoteInsert" ) ) == NULL )
		return 65;
	if ( ( pRegOnRtnQuote                                = (lpRegOnRtnQuote                               )::GetProcAddress( gKSftDLLInstance, "RegOnRtnQuote" ) ) == NULL )
		return 66;
	if ( ( pRegOnErrRtnQuoteInsert                       = (lpRegOnErrRtnQuoteInsert                      )::GetProcAddress( gKSftDLLInstance, "RegOnErrRtnQuoteInsert" ) ) == NULL )
		return 67;
	if ( ( pRegOnErrRtnQuoteAction                       = (lpRegOnErrRtnQuoteAction                      )::GetProcAddress( gKSftDLLInstance, "RegOnErrRtnQuoteAction" ) ) == NULL )
		return 68;
	if ( ( pRegOnRspQryContractBank                      = (lpRegOnRspQryContractBank                     )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryContractBank" ) ) == NULL )
		return 69;
	if ( ( pRegOnRspQryParkedOrder                       = (lpRegOnRspQryParkedOrder                      )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryParkedOrder" ) ) == NULL )
		return 70;
	if ( ( pRegOnRspQryParkedOrderAction                 = (lpRegOnRspQryParkedOrderAction                )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryParkedOrderAction" ) ) == NULL )
		return 71;
	if ( ( pRegOnRspQryTradingNotice                     = (lpRegOnRspQryTradingNotice                    )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryTradingNotice" ) ) == NULL )
		return 72;
	if ( ( pRegOnRspQryBrokerTradingParams               = (lpRegOnRspQryBrokerTradingParams              )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryBrokerTradingParams" ) ) == NULL )
		return 73;
	if ( ( pRegOnRspQryBrokerTradingAlgos                = (lpRegOnRspQryBrokerTradingAlgos               )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryBrokerTradingAlgos" ) ) == NULL )
		return 74;
	if ( ( pRegOnRtnFromBankToFutureByBank               = (lpRegOnRtnFromBankToFutureByBank              )::GetProcAddress( gKSftDLLInstance, "RegOnRtnFromBankToFutureByBank" ) ) == NULL )
		return 75;
	if ( ( pRegOnRtnFromFutureToBankByBank               = (lpRegOnRtnFromFutureToBankByBank              )::GetProcAddress( gKSftDLLInstance, "RegOnRtnFromFutureToBankByBank" ) ) == NULL )
		return 76;
	if ( ( pRegOnRtnRepealFromBankToFutureByBank         = (lpRegOnRtnRepealFromBankToFutureByBank        )::GetProcAddress( gKSftDLLInstance, "RegOnRtnRepealFromBankToFutureByBank" ) ) == NULL )
		return 77;
	if ( ( pRegOnRtnRepealFromFutureToBankByBank         = (lpRegOnRtnRepealFromFutureToBankByBank        )::GetProcAddress( gKSftDLLInstance, "RegOnRtnRepealFromFutureToBankByBank" ) ) == NULL )
		return 78;
	if ( ( pRegOnRtnFromBankToFutureByFuture             = (lpRegOnRtnFromBankToFutureByFuture            )::GetProcAddress( gKSftDLLInstance, "RegOnRtnFromBankToFutureByFuture" ) ) == NULL )
		return 79;
	if ( ( pRegOnRtnFromFutureToBankByFuture             = (lpRegOnRtnFromFutureToBankByFuture            )::GetProcAddress( gKSftDLLInstance, "RegOnRtnFromFutureToBankByFuture" ) ) == NULL )
		return 80;
	if ( ( pRegOnRtnRepealFromBankToFutureByFutureManual = (lpRegOnRtnRepealFromBankToFutureByFutureManual)::GetProcAddress( gKSftDLLInstance, "RegOnRtnRepealFromBankToFutureByFutureManual" ) ) == NULL )
		return 81;
	if ( ( pRegOnRtnRepealFromFutureToBankByFutureManual = (lpRegOnRtnRepealFromFutureToBankByFutureManual)::GetProcAddress( gKSftDLLInstance, "RegOnRtnRepealFromFutureToBankByFutureManual" ) ) == NULL )
		return 82;
	if ( ( pRegOnRtnQueryBankBalanceByFuture             = (lpRegOnRtnQueryBankBalanceByFuture            )::GetProcAddress( gKSftDLLInstance, "RegOnRtnQueryBankBalanceByFuture" ) ) == NULL )
		return 83;
	if ( ( pRegOnErrRtnBankToFutureByFuture              = (lpRegOnErrRtnBankToFutureByFuture             )::GetProcAddress( gKSftDLLInstance, "RegOnErrRtnBankToFutureByFuture" ) ) == NULL )
		return 84;
	if ( ( pRegOnErrRtnFutureToBankByFuture              = (lpRegOnErrRtnFutureToBankByFuture             )::GetProcAddress( gKSftDLLInstance, "RegOnErrRtnFutureToBankByFuture" ) ) == NULL )
		return 85;
	if ( ( pRegOnErrRtnRepealBankToFutureByFutureManual  = (lpRegOnErrRtnRepealBankToFutureByFutureManual )::GetProcAddress( gKSftDLLInstance, "RegOnErrRtnRepealBankToFutureByFutureManual" ) ) == NULL )
		return 86;
	if ( ( pRegOnErrRtnRepealFutureToBankByFutureManual  = (lpRegOnErrRtnRepealFutureToBankByFutureManual )::GetProcAddress( gKSftDLLInstance, "RegOnErrRtnRepealFutureToBankByFutureManual" ) ) == NULL )
		return 87;
	if ( ( pRegOnErrRtnQueryBankBalanceByFuture          = (lpRegOnErrRtnQueryBankBalanceByFuture         )::GetProcAddress( gKSftDLLInstance, "RegOnErrRtnQueryBankBalanceByFuture" ) ) == NULL )
		return 88;
	if ( ( pRegOnRtnRepealFromBankToFutureByFuture       = (lpRegOnRtnRepealFromBankToFutureByFuture      )::GetProcAddress( gKSftDLLInstance, "RegOnRtnRepealFromBankToFutureByFuture" ) ) == NULL )
		return 89;
	if ( ( pRegOnRtnRepealFromFutureToBankByFuture       = (lpRegOnRtnRepealFromFutureToBankByFuture      )::GetProcAddress( gKSftDLLInstance, "RegOnRtnRepealFromFutureToBankByFuture" ) ) == NULL )
		return 90;
	if ( ( pRegOnRspFromBankToFutureByFuture             = (lpRegOnRspFromBankToFutureByFuture            )::GetProcAddress( gKSftDLLInstance, "RegOnRspFromBankToFutureByFuture" ) ) == NULL )
		return 91;
	if ( ( pRegOnRspFromFutureToBankByFuture             = (lpRegOnRspFromFutureToBankByFuture            )::GetProcAddress( gKSftDLLInstance, "RegOnRspFromFutureToBankByFuture" ) ) == NULL )
		return 92;
	if ( ( pRegOnRspQueryBankAccountMoneyByFuture        = (lpRegOnRspQueryBankAccountMoneyByFuture       )::GetProcAddress( gKSftDLLInstance, "RegOnRspQueryBankAccountMoneyByFuture" ) ) == NULL )
		return 93;
	if ( ( pRegOnRtnOpenAccountByBank                    = (lpRegOnRtnOpenAccountByBank                   )::GetProcAddress( gKSftDLLInstance, "RegOnRtnOpenAccountByBank" ) ) == NULL )
		return 94;
	if ( ( pRegOnRtnCancelAccountByBank                  = (lpRegOnRtnCancelAccountByBank                 )::GetProcAddress( gKSftDLLInstance, "RegOnRtnCancelAccountByBank" ) ) == NULL )
		return 95;
	if ( ( pRegOnRtnChangeAccountByBank                  = (lpRegOnRtnChangeAccountByBank                 )::GetProcAddress( gKSftDLLInstance, "RegOnRtnChangeAccountByBank" ) ) == NULL )
		return 96;
	if ( ( pRegOnRspQryInvestorOpenPosition              = (lpRegOnRspQryInvestorOpenPosition             )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryInvestorOpenPosition" ) ) == NULL )
		return 97;
	if ( ( pRegOnRspQryInvestorOpenCombinePosition       = (lpRegOnRspQryInvestorOpenCombinePosition      )::GetProcAddress( gKSftDLLInstance, "RegOnRspQryInvestorOpenCombinePosition" ) ) == NULL )
		return 98;
	if ( ( pRegOnRspBulkCancelOrder                      = (lpRegOnRspBulkCancelOrder                     )::GetProcAddress( gKSftDLLInstance, "RegOnRspBulkCancelOrder" ) ) == NULL )
		return 99;

	return 0;
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnFrontConnected( HKSFT Owner )
{
	TKSftTraderApi* KSftObj = (TKSftTraderApi*)Owner;

	if ( KSftObj != NULL )
	{
		TKSftTraderSpi* Listener = KSftObj->GetSpi( );
		if ( Listener != NULL )
		{
			Listener->OnKSftAPIConnected( );
			PostMessage( KSftObj->GetHWND(), WM_KSFT_EVENT, (WPARAM)Listener, (LPARAM)NULL );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnFrontDisconnected( HKSFT Owner, int nReason )
{
	TKSftTraderApi* KSftObj = (TKSftTraderApi*)Owner;

	if ( KSftObj != NULL )
	{
		TKSftTraderSpi* Listener = KSftObj->GetSpi( );
		if ( Listener != NULL )
		{
			TCTPParams* Params = new TCTPParams( nReason );
			Listener->OnKSftAPIDisconnected( nReason );
			PostMessage( KSftObj->GetHWND(), WM_KSFT_EVENT+1, (WPARAM)Listener, (LPARAM)Params );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnHeartBeatWarning( HKSFT Owner, int nTimeLapse )
{
	TCTPParams* Params = new TCTPParams( nTimeLapse );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+2, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspAuthenticate( HKSFT Owner, KS_::CThostFtdcRspAuthenticateField *pRspAuthenticateField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspAuthenticateField, sizeof(KS_::CThostFtdcRspAuthenticateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+3, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspUserLogin( HKSFT Owner, KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspUserLogin, sizeof(KS_::CThostFtdcRspUserLoginField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+4, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspUserLogout( HKSFT Owner, KS_::CThostFtdcUserLogoutField *pUserLogout, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pUserLogout, sizeof(KS_::CThostFtdcUserLogoutField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+5, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspUserPasswordUpdate( HKSFT Owner, KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pUserPasswordUpdate, sizeof(KS_::CThostFtdcUserPasswordUpdateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+6, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspTradingAccountPasswordUpdate( HKSFT Owner, KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTradingAccountPasswordUpdate, sizeof(KS_::CThostFtdcTradingAccountPasswordUpdateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+7, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspOrderInsert( HKSFT Owner, KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputOrder, sizeof(KS_::CThostFtdcInputOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+8, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspParkedOrderInsert( HKSFT Owner, KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pParkedOrder, sizeof(KS_::CThostFtdcParkedOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+9, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspParkedOrderAction( HKSFT Owner, KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pParkedOrderAction, sizeof(KS_::CThostFtdcParkedOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+10, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspOrderAction( HKSFT Owner, KS_::CThostFtdcInputOrderActionField *pInputOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputOrderAction, sizeof(KS_::CThostFtdcInputOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+11, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQueryMaxOrderVolume( HKSFT Owner, KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pQueryMaxOrderVolume, sizeof(KS_::CThostFtdcQueryMaxOrderVolumeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+12, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspSettlementInfoConfirm( HKSFT Owner, KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pSettlementInfoConfirm, sizeof(KS_::CThostFtdcSettlementInfoConfirmField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+13, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspRemoveParkedOrder( HKSFT Owner, KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pRemoveParkedOrder, sizeof(KS_::CThostFtdcRemoveParkedOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+14, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspRemoveParkedOrderAction( HKSFT Owner, KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pRemoveParkedOrderAction, sizeof(KS_::CThostFtdcRemoveParkedOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+15, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspExecOrderInsert( HKSFT Owner, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputExecOrder, sizeof(KS_::CThostFtdcInputExecOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+16, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspExecOrderAction( HKSFT Owner, KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputExecOrderAction, sizeof(KS_::CThostFtdcInputExecOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+17, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspForQuoteInsert( HKSFT Owner, KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputForQuote, sizeof(KS_::CThostFtdcInputForQuoteField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+18, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQuoteInsert( HKSFT Owner, KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputQuote, sizeof(KS_::CThostFtdcInputQuoteField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+19, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQuoteAction( HKSFT Owner, KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputQuoteAction, sizeof(KS_::CThostFtdcInputQuoteActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+20, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryOrder( HKSFT Owner, KS_::CThostFtdcOrderField *pOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pOrder, sizeof(KS_::CThostFtdcOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+21, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryTrade( HKSFT Owner, KS_::CThostFtdcTradeField *pTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTrade, sizeof(KS_::CThostFtdcTradeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+22, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryInvestorPosition( HKSFT Owner, KS_::CThostFtdcInvestorPositionField *pInvestorPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorPosition, sizeof(KS_::CThostFtdcInvestorPositionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+23, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryTradingAccount( HKSFT Owner, KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTradingAccount, sizeof(KS_::CThostFtdcTradingAccountField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+24, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryInvestor( HKSFT Owner, KS_::CThostFtdcInvestorField *pInvestor, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestor, sizeof(KS_::CThostFtdcInvestorField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+25, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryTradingCode( HKSFT Owner, KS_::CThostFtdcTradingCodeField *pTradingCode, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTradingCode, sizeof(KS_::CThostFtdcTradingCodeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+26, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryInstrumentMarginRate( HKSFT Owner, KS_::CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInstrumentMarginRate, sizeof(KS_::CThostFtdcInstrumentMarginRateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+27, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryInstrumentCommissionRate( HKSFT Owner, KS_::CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInstrumentCommissionRate, sizeof(KS_::CThostFtdcInstrumentCommissionRateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+28, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryExchange( HKSFT Owner, KS_::CThostFtdcExchangeField *pExchange, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pExchange, sizeof(KS_::CThostFtdcExchangeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+29, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryProduct( HKSFT Owner, KS_::CThostFtdcProductField *pProduct, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pProduct, sizeof(KS_::CThostFtdcProductField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+30, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryInstrument( HKSFT Owner, KS_::CThostFtdcInstrumentField *pInstrument, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInstrument, sizeof(KS_::CThostFtdcInstrumentField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+31, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryDepthMarketData( HKSFT Owner, KS_::CThostFtdcDepthMarketDataField *pDepthMarketData, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pDepthMarketData, sizeof(KS_::CThostFtdcDepthMarketDataField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+32, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQrySettlementInfo( HKSFT Owner, KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pSettlementInfo, sizeof(KS_::CThostFtdcSettlementInfoField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+33, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryTransferBank( HKSFT Owner, KS_::CThostFtdcTransferBankField *pTransferBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTransferBank, sizeof(KS_::CThostFtdcTransferBankField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+34, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryInvestorPositionDetail( HKSFT Owner, KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorPositionDetail, sizeof(KS_::CThostFtdcInvestorPositionDetailField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+35, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryNotice( HKSFT Owner, KS_::CThostFtdcNoticeField *pNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pNotice, sizeof(KS_::CThostFtdcNoticeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+36, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQrySettlementInfoConfirm( HKSFT Owner, KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pSettlementInfoConfirm, sizeof(KS_::CThostFtdcSettlementInfoConfirmField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+37, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryInvestorPositionCombineDetail( HKSFT Owner, KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorPositionCombineDetail, sizeof(KS_::CThostFtdcInvestorPositionCombineDetailField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+38, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryCFMMCTradingAccountKey( HKSFT Owner, KS_::CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pCFMMCTradingAccountKey, sizeof(KS_::CThostFtdcCFMMCTradingAccountKeyField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+39, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryEWarrantOffset( HKSFT Owner, KS_::CThostFtdcEWarrantOffsetField *pEWarrantOffset, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pEWarrantOffset, sizeof(KS_::CThostFtdcEWarrantOffsetField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+40, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryInvestorProductGroupMargin( HKSFT Owner, KS_::CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorProductGroupMargin, sizeof(KS_::CThostFtdcInvestorProductGroupMarginField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+41, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryExchangeMarginRate( HKSFT Owner, KS_::CThostFtdcExchangeMarginRateField *pExchangeMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pExchangeMarginRate, sizeof(KS_::CThostFtdcExchangeMarginRateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+42, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryExchangeMarginRateAdjust( HKSFT Owner, KS_::CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pExchangeMarginRateAdjust, sizeof(KS_::CThostFtdcExchangeMarginRateAdjustField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+43, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryExchangeRate( HKSFT Owner, KS_::CThostFtdcExchangeRateField *pExchangeRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pExchangeRate, sizeof(KS_::CThostFtdcExchangeRateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+44, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQrySecAgentACIDMap( HKSFT Owner, KS_::CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pSecAgentACIDMap, sizeof(KS_::CThostFtdcSecAgentACIDMapField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+45, Params );
}
//------------------------------------------------------------------------------------------------------------------------

void KSftTraderApiInit::OnRspQryOptionInstrTradeCost( HKSFT Owner, KS_::CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pOptionInstrTradeCost, sizeof(KS_::CThostFtdcOptionInstrTradeCostField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+46, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryOptionInstrCommRate( HKSFT Owner, KS_::CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pOptionInstrCommRate, sizeof(KS_::CThostFtdcOptionInstrCommRateField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+47, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryExecOrder( HKSFT Owner, KS_::CThostFtdcExecOrderField *pExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pExecOrder, sizeof(KS_::CThostFtdcExecOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+48, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryForQuote( HKSFT Owner, KS_::CThostFtdcForQuoteField *pForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pForQuote, sizeof(KS_::CThostFtdcForQuoteField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+49, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryQuote( HKSFT Owner, KS_::CThostFtdcQuoteField *pQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pQuote, sizeof(KS_::CThostFtdcQuoteField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+50, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryTransferSerial( HKSFT Owner, KS_::CThostFtdcTransferSerialField *pTransferSerial, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTransferSerial, sizeof(KS_::CThostFtdcTransferSerialField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+51, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryAccountregister( HKSFT Owner, KS_::CThostFtdcAccountregisterField *pAccountregister, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pAccountregister, sizeof(KS_::CThostFtdcAccountregisterField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+52, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspError( HKSFT Owner, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+53, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnOrder( HKSFT Owner, KS_::CThostFtdcOrderField *pOrder )
{
	TCTPParams* Params = new TCTPParams( (void*)pOrder, sizeof(KS_::CThostFtdcOrderField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+54, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnTrade( HKSFT Owner, KS_::CThostFtdcTradeField *pTrade )
{
	TCTPParams* Params = new TCTPParams( (void*)pTrade, sizeof(KS_::CThostFtdcTradeField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+55, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnErrRtnOrderInsert( HKSFT Owner, KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputOrder, sizeof(KS_::CThostFtdcInputOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+56, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnErrRtnOrderAction( HKSFT Owner, KS_::CThostFtdcOrderActionField *pOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pOrderAction, sizeof(KS_::CThostFtdcOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+57, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnInstrumentStatus( HKSFT Owner, KS_::CThostFtdcInstrumentStatusField *pInstrumentStatus )
{
	TCTPParams* Params = new TCTPParams( (void*)pInstrumentStatus, sizeof(KS_::CThostFtdcInstrumentStatusField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+58, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnTradingNotice( HKSFT Owner, KS_::CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pTradingNoticeInfo, sizeof(KS_::CThostFtdcTradingNoticeInfoField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+59, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnErrorConditionalOrder( HKSFT Owner, KS_::CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder )
{
	TCTPParams* Params = new TCTPParams( (void*)pErrorConditionalOrder, sizeof(KS_::CThostFtdcErrorConditionalOrderField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+60, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnExecOrder( HKSFT Owner, KS_::CThostFtdcExecOrderField *pExecOrder )
{
	TCTPParams* Params = new TCTPParams( (void*)pExecOrder, sizeof(KS_::CThostFtdcExecOrderField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+61, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnErrRtnExecOrderInsert( HKSFT Owner, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputExecOrder, sizeof(KS_::CThostFtdcInputExecOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+62, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnErrRtnExecOrderAction( HKSFT Owner, KS_::CThostFtdcExecOrderActionField *pExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pExecOrderAction, sizeof(KS_::CThostFtdcExecOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+63, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnErrRtnForQuoteInsert( HKSFT Owner, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputExecOrder, sizeof(KS_::CThostFtdcInputExecOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+64, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnQuote( HKSFT Owner, KS_::CThostFtdcQuoteField *pQuote )
{
	TCTPParams* Params = new TCTPParams( (void*)pQuote, sizeof(KS_::CThostFtdcQuoteField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+65, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnErrRtnQuoteInsert( HKSFT Owner, KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pInputQuote, sizeof(KS_::CThostFtdcInputQuoteField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+66, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnErrRtnQuoteAction( HKSFT Owner, KS_::CThostFtdcQuoteActionField *pQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pQuoteAction, sizeof(KS_::CThostFtdcQuoteActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+67, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryContractBank( HKSFT Owner, KS_::CThostFtdcContractBankField *pContractBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pContractBank, sizeof(KS_::CThostFtdcContractBankField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+68, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryParkedOrder( HKSFT Owner, KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pParkedOrder, sizeof(KS_::CThostFtdcParkedOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+69, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryParkedOrderAction( HKSFT Owner, KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pParkedOrderAction, sizeof(KS_::CThostFtdcParkedOrderActionField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+70, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryTradingNotice( HKSFT Owner, KS_::CThostFtdcTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pTradingNotice, sizeof(KS_::CThostFtdcTradingNoticeField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+71, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryBrokerTradingParams( HKSFT Owner, KS_::CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pBrokerTradingParams, sizeof(KS_::CThostFtdcBrokerTradingParamsField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+72, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryBrokerTradingAlgos( HKSFT Owner, KS_::CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pBrokerTradingAlgos, sizeof(KS_::CThostFtdcBrokerTradingAlgosField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+73, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnFromBankToFutureByBank( HKSFT Owner, KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspTransfer, sizeof(KS_::CThostFtdcRspTransferField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+74, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnFromFutureToBankByBank( HKSFT Owner, KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspTransfer, sizeof(KS_::CThostFtdcRspTransferField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+75, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnRepealFromBankToFutureByBank( HKSFT Owner, KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspRepeal, sizeof(KS_::CThostFtdcRspRepealField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+76, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnRepealFromFutureToBankByBank( HKSFT Owner, KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspRepeal, sizeof(KS_::CThostFtdcRspRepealField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+77, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnFromBankToFutureByFuture( HKSFT Owner, KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspTransfer, sizeof(KS_::CThostFtdcRspTransferField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+78, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnFromFutureToBankByFuture( HKSFT Owner, KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspTransfer, sizeof(KS_::CThostFtdcRspTransferField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+79, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnRepealFromBankToFutureByFutureManual( HKSFT Owner, KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspRepeal, sizeof(KS_::CThostFtdcRspRepealField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+80, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnRepealFromFutureToBankByFutureManual( HKSFT Owner, KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspRepeal, sizeof(KS_::CThostFtdcRspRepealField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+81, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnQueryBankBalanceByFuture( HKSFT Owner, KS_::CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount )
{
	TCTPParams* Params = new TCTPParams( (void*)pNotifyQueryAccount, sizeof(KS_::CThostFtdcNotifyQueryAccountField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+82, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnErrRtnBankToFutureByFuture( HKSFT Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqTransfer, sizeof(KS_::CThostFtdcReqTransferField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+83, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnErrRtnFutureToBankByFuture( HKSFT Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqTransfer, sizeof(KS_::CThostFtdcReqTransferField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+84, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnErrRtnRepealBankToFutureByFutureManual( HKSFT Owner, KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqRepeal, sizeof(KS_::CThostFtdcReqRepealField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+85, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnErrRtnRepealFutureToBankByFutureManual( HKSFT Owner, KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqRepeal, sizeof(KS_::CThostFtdcReqRepealField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+86, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnErrRtnQueryBankBalanceByFuture( HKSFT Owner, KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqQueryAccount, sizeof(KS_::CThostFtdcReqQueryAccountField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+87, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnRepealFromBankToFutureByFuture( HKSFT Owner, KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspRepeal, sizeof(KS_::CThostFtdcRspRepealField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+88, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnRepealFromFutureToBankByFuture( HKSFT Owner, KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	TCTPParams* Params = new TCTPParams( (void*)pRspRepeal, sizeof(KS_::CThostFtdcRspRepealField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+89, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspFromBankToFutureByFuture( HKSFT Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqTransfer, sizeof(KS_::CThostFtdcReqTransferField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+90, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspFromFutureToBankByFuture( HKSFT Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqTransfer, sizeof(KS_::CThostFtdcReqTransferField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+91, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQueryBankAccountMoneyByFuture( HKSFT Owner, KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pReqQueryAccount, sizeof(KS_::CThostFtdcReqQueryAccountField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+92, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnOpenAccountByBank( HKSFT Owner, KS_::CThostFtdcOpenAccountField *pOpenAccount )
{
	TCTPParams* Params = new TCTPParams( (void*)pOpenAccount, sizeof(KS_::CThostFtdcOpenAccountField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+93, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnCancelAccountByBank( HKSFT Owner, KS_::CThostFtdcCancelAccountField *pCancelAccount )
{
	TCTPParams* Params = new TCTPParams( (void*)pCancelAccount, sizeof(KS_::CThostFtdcCancelAccountField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+94, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRtnChangeAccountByBank( HKSFT Owner, KS_::CThostFtdcChangeAccountField *pChangeAccount )
{
	TCTPParams* Params = new TCTPParams( (void*)pChangeAccount, sizeof(KS_::CThostFtdcChangeAccountField) );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+95, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryInvestorOpenPosition( HKSFT Owner, KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorPositionDetail, sizeof(KS_::CThostFtdcInvestorPositionDetailField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+96, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspQryInvestorOpenCombinePosition( HKSFT Owner, KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pInvestorPositionCombineDetail, sizeof(KS_::CThostFtdcInvestorPositionCombineDetailField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+97, Params );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftTraderApiInit::OnRspBulkCancelOrder( HKSFT Owner, KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	TCTPParams* Params = new TCTPParams( (void*)pBulkCancelOrder, sizeof(KS_::CThostFtdcBulkCancelOrderField), (void*)pRspInfo, sizeof(KS_::CThostFtdcRspInfoField), nRequestID, bIsLast );

	TriggerKSftEvent( Owner, WM_KSFT_EVENT+98, Params );
}
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
