//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <stdio.h>
#include "CTPBCBTraderAPI.h"
#include "CTPBCBEvents.h"
#include "APIParams.h"
#include "../../../../UFC/UFC.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
CTPTraderApiInit gCTPAPI( "CTP" );
CTPTraderApiInit gHSctpAPI( "HSctp" );
CTPTraderApiInit gKNctpAPI( "KNctp" );
HINSTANCE        gCTPDLLInstance   = NULL;
HINSTANCE        gHSctpDLLInstance = NULL;
HINSTANCE        gKNctpDLLInstance = NULL;
//------------------------------------------------------------------------------
//
// Class TThostFtdcTraderApi
// Wrapper for CTP APV visual C++ version.
//
//------------------------------------------------------------------------------
TThostFtdcTraderApi::TThostFtdcTraderApi( TThostFtdcTraderSpi *pSpi, UFC::AnsiString APIType )
: FCTPLikeAPI( NULL )
, FHCTPAPI( NULL )
, FListener( pSpi )
{
	char strDllName[64];

	///< Set FCTPLikeAPI by APIType
	if ( APIType == "CTP" )
		FCTPLikeAPI = &gCTPAPI;
	else if ( APIType == "HSctp" )
		FCTPLikeAPI = &gHSctpAPI;
	else if ( APIType == "KNctp" )
		FCTPLikeAPI = &gKNctpAPI;
	else
		throw Exception( " Unknow APIType" );
	///< Check CTP API exists.
	if( TThostFtdcTraderApi::SupportCTPAPI(APIType) == false )
		throw Exception( String(" Not support ") + String(APIType.c_str()) + String(" API."));
	///< Create CTP Trader API Instance.
	if( (FHCTPAPI = FCTPLikeAPI->pCreateCTPTraderApi( this, strDllName )) == NULL )
		throw Exception( String(" Load ") + String(strDllName) + String(" failed!") );
	///< Register all call back functions.
	RegAllCallbackFuncs( );
	///< Create message window
	if( InitMessageWindow() == TRUE )
	{
		if( CreateMessageWindow()== FALSE )
		{
			UFC::BufferedLog::DebugPrintf( " Can't create the message window." );
			throw UFC::Exception( " Can't create the message window." );
		}
		else
			UFC::BufferedLog::DebugPrintf( " Create message window:%p Thread:%d", (void*)FHwnd,GetCurrentThreadId() );
	}
	else
	{
		UFC::BufferedLog::DebugPrintf( " Register window class failed" );
		throw UFC::Exception( " Register window class failed." );
	}
}
//------------------------------------------------------------------------------
TThostFtdcTraderApi::~TThostFtdcTraderApi()
{
	RegisterSpi( NULL );
	FListener = NULL;
	DestroyWindow( FHwnd );
	UnregisterClassW( MessageWindowsClassName, FInstance );
	FCTPLikeAPI->pReleaseCTPTraderApi( FHCTPAPI );
}
//---------------------------------------------------------------------------
void TThostFtdcTraderApi::RegAllCallbackFuncs( void )
{
	FCTPLikeAPI->pRegOnFrontConnected( FHCTPAPI                          ,CTPTraderApiInit::OnFrontConnected);
	FCTPLikeAPI->pRegOnFrontDisconnected( FHCTPAPI                       ,CTPTraderApiInit::OnFrontDisconnected);
	FCTPLikeAPI->pRegOnRspUserLogin( FHCTPAPI                            ,CTPTraderApiInit::OnRspUserLogin);
	FCTPLikeAPI->pRegOnRspUserLogout( FHCTPAPI                           ,CTPTraderApiInit::OnRspUserLogout);
	FCTPLikeAPI->pRegOnRspQrySettlementInfo( FHCTPAPI                    ,CTPTraderApiInit::OnRspQrySettlementInfo);
	FCTPLikeAPI->pRegOnRspSettlementInfoConfirm( FHCTPAPI                ,CTPTraderApiInit::OnRspSettlementInfoConfirm);
	FCTPLikeAPI->pRegOnRspOrderInsert( FHCTPAPI                          ,CTPTraderApiInit::OnRspOrderInsert);
	FCTPLikeAPI->pRegOnErrRtnOrderInsert( FHCTPAPI                       ,CTPTraderApiInit::OnErrRtnOrderInsert);
	FCTPLikeAPI->pRegOnRspOrderAction( FHCTPAPI                          ,CTPTraderApiInit::OnRspOrderAction);
	FCTPLikeAPI->pRegOnErrRtnOrderAction( FHCTPAPI                       ,CTPTraderApiInit::OnErrRtnOrderAction);
	FCTPLikeAPI->pRegOnRspQryOrder( FHCTPAPI                             ,CTPTraderApiInit::OnRspQryOrder);
	FCTPLikeAPI->pRegOnRspQryExchange( FHCTPAPI                          ,CTPTraderApiInit::OnRspQryExchange);
	FCTPLikeAPI->pRegOnRspQryInstrument( FHCTPAPI                        ,CTPTraderApiInit::OnRspQryInstrument);
	FCTPLikeAPI->pRegOnRspQryInvestorPosition( FHCTPAPI                  ,CTPTraderApiInit::OnRspQryInvestorPosition);
	FCTPLikeAPI->pRegOnRtnOrder( FHCTPAPI                                ,CTPTraderApiInit::OnRtnOrder);
	FCTPLikeAPI->pRegOnRtnTrade( FHCTPAPI                                ,CTPTraderApiInit::OnRtnTrade);
	FCTPLikeAPI->pRegOnRspError( FHCTPAPI                                ,CTPTraderApiInit::OnRspError);
	FCTPLikeAPI->pRegOnHeartBeatWarning( FHCTPAPI                        ,CTPTraderApiInit::OnHeartBeatWarning);
	FCTPLikeAPI->pRegOnRspAuthenticate( FHCTPAPI                         ,CTPTraderApiInit::OnRspAuthenticate);
	FCTPLikeAPI->pRegOnRspUserPasswordUpdate( FHCTPAPI                   ,CTPTraderApiInit::OnRspUserPasswordUpdate);
	FCTPLikeAPI->pRegOnRspTradingAccountPasswordUpdate( FHCTPAPI         ,CTPTraderApiInit::OnRspTradingAccountPasswordUpdate);
	FCTPLikeAPI->pRegOnRspParkedOrderInsert( FHCTPAPI                    ,CTPTraderApiInit::OnRspParkedOrderInsert);
	FCTPLikeAPI->pRegOnRspParkedOrderAction( FHCTPAPI                    ,CTPTraderApiInit::OnRspParkedOrderAction);
	FCTPLikeAPI->pRegOnRspQueryMaxOrderVolume( FHCTPAPI                  ,CTPTraderApiInit::OnRspQueryMaxOrderVolume);
	FCTPLikeAPI->pRegOnRspRemoveParkedOrder( FHCTPAPI                    ,CTPTraderApiInit::OnRspRemoveParkedOrder);
	FCTPLikeAPI->pRegOnRspRemoveParkedOrderAction( FHCTPAPI              ,CTPTraderApiInit::OnRspRemoveParkedOrderAction);
	FCTPLikeAPI->pRegOnRspQryTrade( FHCTPAPI                             ,CTPTraderApiInit::OnRspQryTrade);
	FCTPLikeAPI->pRegOnRspQryTradingAccount( FHCTPAPI                    ,CTPTraderApiInit::OnRspQryTradingAccount);
	FCTPLikeAPI->pRegOnRspQryInvestor( FHCTPAPI                          ,CTPTraderApiInit::OnRspQryInvestor);
	FCTPLikeAPI->pRegOnRspQryTradingCode( FHCTPAPI                       ,CTPTraderApiInit::OnRspQryTradingCode);
	FCTPLikeAPI->pRegOnRspQryInstrumentMarginRate( FHCTPAPI              ,CTPTraderApiInit::OnRspQryInstrumentMarginRate);
	FCTPLikeAPI->pRegOnRspQryInstrumentCommissionRate( FHCTPAPI          ,CTPTraderApiInit::OnRspQryInstrumentCommissionRate);
	FCTPLikeAPI->pRegOnRspQryDepthMarketData( FHCTPAPI                   ,CTPTraderApiInit::OnRspQryDepthMarketData);
	FCTPLikeAPI->pRegOnRspQryTransferBank( FHCTPAPI                      ,CTPTraderApiInit::OnRspQryTransferBank);
	FCTPLikeAPI->pRegOnRspQryInvestorPositionDetail( FHCTPAPI            ,CTPTraderApiInit::OnRspQryInvestorPositionDetail);
	FCTPLikeAPI->pRegOnRspQryNotice( FHCTPAPI                            ,CTPTraderApiInit::OnRspQryNotice);
	FCTPLikeAPI->pRegOnRspQrySettlementInfoConfirm( FHCTPAPI             ,CTPTraderApiInit::OnRspQrySettlementInfoConfirm);
	FCTPLikeAPI->pRegOnRspQryInvestorPositionCombineDetail( FHCTPAPI     ,CTPTraderApiInit::OnRspQryInvestorPositionCombineDetail);
	FCTPLikeAPI->pRegOnRspQryCFMMCTradingAccountKey( FHCTPAPI            ,CTPTraderApiInit::OnRspQryCFMMCTradingAccountKey);
	FCTPLikeAPI->pRegOnRspQryEWarrantOffset( FHCTPAPI                    ,CTPTraderApiInit::OnRspQryEWarrantOffset);
	FCTPLikeAPI->pRegOnRspQryTransferSerial( FHCTPAPI                    ,CTPTraderApiInit::OnRspQryTransferSerial);
	FCTPLikeAPI->pRegOnRspQryAccountregister( FHCTPAPI                   ,CTPTraderApiInit::OnRspQryAccountregister);
	FCTPLikeAPI->pRegOnRtnInstrumentStatus( FHCTPAPI                     ,CTPTraderApiInit::OnRtnInstrumentStatus);
	FCTPLikeAPI->pRegOnRtnTradingNotice( FHCTPAPI                        ,CTPTraderApiInit::OnRtnTradingNotice);
	FCTPLikeAPI->pRegOnRtnErrorConditionalOrder( FHCTPAPI                ,CTPTraderApiInit::OnRtnErrorConditionalOrder);
	FCTPLikeAPI->pRegOnRspQryContractBank( FHCTPAPI                      ,CTPTraderApiInit::OnRspQryContractBank);
	FCTPLikeAPI->pRegOnRspQryParkedOrder( FHCTPAPI                       ,CTPTraderApiInit::OnRspQryParkedOrder);
	FCTPLikeAPI->pRegOnRspQryParkedOrderAction( FHCTPAPI                 ,CTPTraderApiInit::OnRspQryParkedOrderAction);
	FCTPLikeAPI->pRegOnRspQryTradingNotice( FHCTPAPI                     ,CTPTraderApiInit::OnRspQryTradingNotice);
	FCTPLikeAPI->pRegOnRspQryBrokerTradingParams( FHCTPAPI               ,CTPTraderApiInit::OnRspQryBrokerTradingParams);
	FCTPLikeAPI->pRegOnRspQryBrokerTradingAlgos( FHCTPAPI                ,CTPTraderApiInit::OnRspQryBrokerTradingAlgos);
	FCTPLikeAPI->pRegOnRtnFromBankToFutureByBank( FHCTPAPI               ,CTPTraderApiInit::OnRtnFromBankToFutureByBank);
	FCTPLikeAPI->pRegOnRtnFromFutureToBankByBank( FHCTPAPI               ,CTPTraderApiInit::OnRtnFromFutureToBankByBank);
	FCTPLikeAPI->pRegOnRtnRepealFromBankToFutureByBank( FHCTPAPI         ,CTPTraderApiInit::OnRtnRepealFromBankToFutureByBank);
	FCTPLikeAPI->pRegOnRtnRepealFromFutureToBankByBank( FHCTPAPI         ,CTPTraderApiInit::OnRtnRepealFromFutureToBankByBank);
	FCTPLikeAPI->pRegOnRtnFromBankToFutureByFuture( FHCTPAPI             ,CTPTraderApiInit::OnRtnFromBankToFutureByFuture);
	FCTPLikeAPI->pRegOnRtnFromFutureToBankByFuture( FHCTPAPI             ,CTPTraderApiInit::OnRtnFromFutureToBankByFuture);
	FCTPLikeAPI->pRegOnRtnRepealFromBankToFutureByFutureManual( FHCTPAPI ,CTPTraderApiInit::OnRtnRepealFromBankToFutureByFutureManual);
	FCTPLikeAPI->pRegOnRtnRepealFromFutureToBankByFutureManual( FHCTPAPI ,CTPTraderApiInit::OnRtnRepealFromFutureToBankByFutureManual);
	FCTPLikeAPI->pRegOnRtnQueryBankBalanceByFuture( FHCTPAPI             ,CTPTraderApiInit::OnRtnQueryBankBalanceByFuture);
	FCTPLikeAPI->pRegOnErrRtnBankToFutureByFuture( FHCTPAPI              ,CTPTraderApiInit::OnErrRtnBankToFutureByFuture);
	FCTPLikeAPI->pRegOnErrRtnFutureToBankByFuture( FHCTPAPI              ,CTPTraderApiInit::OnErrRtnFutureToBankByFuture);
	FCTPLikeAPI->pRegOnErrRtnRepealBankToFutureByFutureManual( FHCTPAPI  ,CTPTraderApiInit::OnErrRtnRepealBankToFutureByFutureManual);
	FCTPLikeAPI->pRegOnErrRtnRepealFutureToBankByFutureManual( FHCTPAPI  ,CTPTraderApiInit::OnErrRtnRepealFutureToBankByFutureManual);
	FCTPLikeAPI->pRegOnErrRtnQueryBankBalanceByFuture( FHCTPAPI          ,CTPTraderApiInit::OnErrRtnQueryBankBalanceByFuture);
	FCTPLikeAPI->pRegOnRtnRepealFromBankToFutureByFuture( FHCTPAPI       ,CTPTraderApiInit::OnRtnRepealFromBankToFutureByFuture);
	FCTPLikeAPI->pRegOnRtnRepealFromFutureToBankByFuture( FHCTPAPI       ,CTPTraderApiInit::OnRtnRepealFromFutureToBankByFuture);
	FCTPLikeAPI->pRegOnRspFromBankToFutureByFuture( FHCTPAPI             ,CTPTraderApiInit::OnRspFromBankToFutureByFuture);
	FCTPLikeAPI->pRegOnRspFromFutureToBankByFuture( FHCTPAPI             ,CTPTraderApiInit::OnRspFromFutureToBankByFuture);
	FCTPLikeAPI->pRegOnRspQueryBankAccountMoneyByFuture( FHCTPAPI        ,CTPTraderApiInit::OnRspQueryBankAccountMoneyByFuture);
	FCTPLikeAPI->pRegOnRtnOpenAccountByBank( FHCTPAPI                    ,CTPTraderApiInit::OnRtnOpenAccountByBank);
	FCTPLikeAPI->pRegOnRtnCancelAccountByBank( FHCTPAPI                  ,CTPTraderApiInit::OnRtnCancelAccountByBank);
	FCTPLikeAPI->pRegOnRtnChangeAccountByBank( FHCTPAPI                  ,CTPTraderApiInit::OnRtnChangeAccountByBank);
}
//---------------------------------------------------------------------------
LRESULT CALLBACK TThostFtdcTraderApi::MessageWndProc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	TCTPParams*          Params   = (TCTPParams*)lParam;
	TThostFtdcTraderSpi* Listener = (TThostFtdcTraderSpi*)wParam;
	switch( Msg )
	{
		case WM_CTP_EVENT:    Listener->OnFrontConnected();break;
		case WM_CTP_EVENT+1:  Listener->OnFrontDisconnected( Params->Param1.AsInt() );
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+2:  Listener->OnRspUserLogin( (CThostFtdcRspUserLoginField*)Params->Param1.AsPtr(),
														(CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
														 Params->Param3.AsInt(),
														 Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+3:  Listener->OnRspUserLogout((CThostFtdcUserLogoutField*)Params->Param1.AsPtr(),
														(CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
														 Params->Param3.AsInt(),
														 Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+4:  Listener->OnRspQrySettlementInfo((CThostFtdcSettlementInfoField*)Params->Param1.AsPtr(),
															   (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
															   Params->Param3.AsInt(),
															   Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+5:  Listener->OnRspSettlementInfoConfirm((CThostFtdcSettlementInfoConfirmField*)Params->Param1.AsPtr(),
																   (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																   Params->Param3.AsInt(),
																   Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+6:  Listener->OnRspOrderInsert((CThostFtdcInputOrderField*)Params->Param1.AsPtr(),
														 (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
														 Params->Param3.AsInt(),
														 Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+7:  Listener->OnErrRtnOrderInsert((CThostFtdcInputOrderField*)Params->Param1.AsPtr(),
															(CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+8:  Listener->OnRspOrderAction((CThostFtdcInputOrderActionField*)Params->Param1.AsPtr(),
														 (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
														 Params->Param3.AsInt(),
														 Params->Param4.AsBool() );
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+9:  Listener->OnErrRtnOrderAction((CThostFtdcOrderActionField*)Params->Param1.AsPtr(),
															(CThostFtdcRspInfoField*)Params->Param2.AsPtr() );
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+10: Listener->OnRspQryOrder((CThostFtdcOrderField*)Params->Param1.AsPtr(),
													  (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
													  Params->Param3.AsInt(),
													  Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+11: Listener->OnRspQryExchange((CThostFtdcExchangeField*)Params->Param1.AsPtr(),
														 (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
														 Params->Param3.AsInt(),
														 Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+12: Listener->OnRspQryInstrument((CThostFtdcInstrumentField*)Params->Param1.AsPtr(),
														   (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
															Params->Param3.AsInt(),
															Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+13: Listener->OnRspQryInvestorPosition((CThostFtdcInvestorPositionField*)Params->Param1.AsPtr(),
																 (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																 Params->Param3.AsInt(),
																 Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+14: Listener->OnRspError((CThostFtdcRspInfoField*)Params->Param1.AsPtr(),
												   Params->Param2.AsInt(),
												   Params->Param3.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+15: Listener->OnRtnOrder((CThostFtdcOrderField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+16: Listener->OnRtnTrade((CThostFtdcTradeField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+17: Listener->OnHeartBeatWarning( Params->Param1.AsInt());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+18: Listener->OnRspAuthenticate((CThostFtdcRspAuthenticateField*)Params->Param1.AsPtr(),
														  (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
														  Params->Param3.AsInt(),
														  Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+19: Listener->OnRspUserPasswordUpdate((CThostFtdcUserPasswordUpdateField*)Params->Param1.AsPtr(),
																(CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																Params->Param3.AsInt(),
																Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+20: Listener->OnRspTradingAccountPasswordUpdate((CThostFtdcTradingAccountPasswordUpdateField*)Params->Param1.AsPtr(),
																		  (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																		  Params->Param3.AsInt(),
																		  Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+21: Listener->OnRspParkedOrderInsert((CThostFtdcParkedOrderField*)Params->Param1.AsPtr(),
															   (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
															   Params->Param3.AsInt(),
															   Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+22: Listener->OnRspParkedOrderAction((CThostFtdcParkedOrderActionField*)Params->Param1.AsPtr(),
															   (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
															   Params->Param3.AsInt(),
															   Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+23: Listener->OnRspQueryMaxOrderVolume((CThostFtdcQueryMaxOrderVolumeField*)Params->Param1.AsPtr(),
																 (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																 Params->Param3.AsInt(),
																 Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+24: Listener->OnRspRemoveParkedOrder((CThostFtdcRemoveParkedOrderField*)Params->Param1.AsPtr(),
															   (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
															   Params->Param3.AsInt(),
															   Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+25: Listener->OnRspRemoveParkedOrderAction((CThostFtdcRemoveParkedOrderActionField*)Params->Param1.AsPtr(),
																	 (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																	 Params->Param3.AsInt(),
																	 Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+26: Listener->OnRspQryTrade((CThostFtdcTradeField*)Params->Param1.AsPtr(),
													  (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
													  Params->Param3.AsInt(),
													  Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+27: Listener->OnRspQryTradingAccount((CThostFtdcTradingAccountField*)Params->Param1.AsPtr(),
															   (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
															   Params->Param3.AsInt(),
															   Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+28: Listener->OnRspQryInvestor((CThostFtdcInvestorField*)Params->Param1.AsPtr(),
														 (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
														 Params->Param3.AsInt(),
														 Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+29: Listener->OnRspQryTradingCode((CThostFtdcTradingCodeField*)Params->Param1.AsPtr(),
															(CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
															Params->Param3.AsInt(),
															Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+30: Listener->OnRspQryInstrumentMarginRate((CThostFtdcInstrumentMarginRateField*)Params->Param1.AsPtr(),
																	 (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																	 Params->Param3.AsInt(),
																	 Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+31: Listener->OnRspQryInstrumentCommissionRate((CThostFtdcInstrumentCommissionRateField*)Params->Param1.AsPtr(),
																		 (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																		 Params->Param3.AsInt(),
																		 Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+32: Listener->OnRspQryDepthMarketData((CThostFtdcDepthMarketDataField*)Params->Param1.AsPtr(),
																(CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																Params->Param3.AsInt(),
																Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+33: Listener->OnRspQryTransferBank((CThostFtdcTransferBankField*)Params->Param1.AsPtr(),
															 (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
															 Params->Param3.AsInt(),
															 Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+34: Listener->OnRspQryInvestorPositionDetail((CThostFtdcInvestorPositionDetailField*)Params->Param1.AsPtr(),
																	   (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																	   Params->Param3.AsInt(),
																	   Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+35: Listener->OnRspQryNotice((CThostFtdcNoticeField*)Params->Param1.AsPtr(),
													   (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
													   Params->Param3.AsInt(),
													   Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+36: Listener->OnRspQrySettlementInfoConfirm((CThostFtdcSettlementInfoConfirmField*)Params->Param1.AsPtr(),
																	   (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																	   Params->Param3.AsInt(),
																	   Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+37: Listener->OnRspQryInvestorPositionCombineDetail((CThostFtdcInvestorPositionCombineDetailField*)Params->Param1.AsPtr(),
																			  (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																			  Params->Param3.AsInt(),
																			  Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+38: Listener->OnRspQryCFMMCTradingAccountKey((CThostFtdcCFMMCTradingAccountKeyField*)Params->Param1.AsPtr(),
																	   (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																	   Params->Param3.AsInt(),
																	   Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+39: Listener->OnRspQryEWarrantOffset((CThostFtdcEWarrantOffsetField*)Params->Param1.AsPtr(),
															   (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
															   Params->Param3.AsInt(),
															   Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+40: Listener->OnRspQryTransferSerial((CThostFtdcTransferSerialField*)Params->Param1.AsPtr(),
															   (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
															   Params->Param3.AsInt(),
															   Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+41: Listener->OnRspQryAccountregister((CThostFtdcAccountregisterField*)Params->Param1.AsPtr(),
																(CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																Params->Param3.AsInt(),
																Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+42: Listener->OnRtnInstrumentStatus((CThostFtdcInstrumentStatusField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+43: Listener->OnRtnTradingNotice((CThostFtdcTradingNoticeInfoField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+44: Listener->OnRtnErrorConditionalOrder((CThostFtdcErrorConditionalOrderField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+45: Listener->OnRspQryContractBank((CThostFtdcContractBankField*)Params->Param1.AsPtr(),
															 (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
															 Params->Param3.AsInt(),
															 Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+46: Listener->OnRspQryParkedOrder((CThostFtdcParkedOrderField*)Params->Param1.AsPtr(),
															(CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
															Params->Param3.AsInt(),
															Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+47: Listener->OnRspQryParkedOrderAction((CThostFtdcParkedOrderActionField*)Params->Param1.AsPtr(),
																  (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																  Params->Param3.AsInt(),
																  Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+48: Listener->OnRspQryTradingNotice((CThostFtdcTradingNoticeField*)Params->Param1.AsPtr(),
															  (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
															  Params->Param3.AsInt(),
															  Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+49: Listener->OnRspQryBrokerTradingParams((CThostFtdcBrokerTradingParamsField*)Params->Param1.AsPtr(),
															  (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
															  Params->Param3.AsInt(),
															  Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+50: Listener->OnRspQryBrokerTradingAlgos((CThostFtdcBrokerTradingAlgosField*)Params->Param1.AsPtr(),
															  (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
															  Params->Param3.AsInt(),
															  Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+51: Listener->OnRtnFromBankToFutureByBank((CThostFtdcRspTransferField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+52: Listener->OnRtnFromFutureToBankByBank((CThostFtdcRspTransferField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+53: Listener->OnRtnRepealFromBankToFutureByBank((CThostFtdcRspRepealField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+54: Listener->OnRtnRepealFromFutureToBankByBank((CThostFtdcRspRepealField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+55: Listener->OnRtnFromBankToFutureByFuture((CThostFtdcRspTransferField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+56: Listener->OnRtnFromFutureToBankByFuture((CThostFtdcRspTransferField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+57: Listener->OnRtnRepealFromBankToFutureByFutureManual((CThostFtdcRspRepealField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+58: Listener->OnRtnRepealFromFutureToBankByFutureManual((CThostFtdcRspRepealField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+59: Listener->OnRtnQueryBankBalanceByFuture((CThostFtdcNotifyQueryAccountField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+60: Listener->OnErrRtnBankToFutureByFuture((CThostFtdcReqTransferField*)Params->Param1.AsPtr(),
																	 (CThostFtdcRspInfoField*)Params->Param2.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+61: Listener->OnErrRtnFutureToBankByFuture((CThostFtdcReqTransferField*)Params->Param1.AsPtr(),
																	 (CThostFtdcRspInfoField*)Params->Param2.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+62: Listener->OnErrRtnRepealBankToFutureByFutureManual((CThostFtdcReqRepealField*)Params->Param1.AsPtr(),
																				 (CThostFtdcRspInfoField*)Params->Param2.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+63: Listener->OnErrRtnRepealFutureToBankByFutureManual((CThostFtdcReqRepealField*)Params->Param1.AsPtr(),
																				 (CThostFtdcRspInfoField*)Params->Param2.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+64: Listener->OnErrRtnQueryBankBalanceByFuture((CThostFtdcReqQueryAccountField*)Params->Param1.AsPtr(),
																		 (CThostFtdcRspInfoField*)Params->Param2.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+65: Listener->OnRtnRepealFromBankToFutureByFuture((CThostFtdcRspRepealField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+66: Listener->OnRtnRepealFromFutureToBankByFuture((CThostFtdcRspRepealField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+67: Listener->OnRspFromBankToFutureByFuture((CThostFtdcReqTransferField*)Params->Param1.AsPtr(),
																	  (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																	  Params->Param3.AsInt(),
																	  Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+68: Listener->OnRspFromFutureToBankByFuture((CThostFtdcReqTransferField*)Params->Param1.AsPtr(),
																	  (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																	  Params->Param3.AsInt(),
																	  Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+69: Listener->OnRspQueryBankAccountMoneyByFuture((CThostFtdcReqQueryAccountField*)Params->Param1.AsPtr(),
																		   (CThostFtdcRspInfoField*)Params->Param2.AsPtr(),
																		   Params->Param3.AsInt(),
																		   Params->Param4.AsBool());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+70: Listener->OnRtnOpenAccountByBank((CThostFtdcOpenAccountField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+71: Listener->OnRtnCancelAccountByBank((CThostFtdcCancelAccountField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+72: Listener->OnRtnChangeAccountByBank((CThostFtdcChangeAccountField*)Params->Param1.AsPtr());
							  delete Params;
							  return 0;
		case WM_CTP_EVENT+73: Listener->OnRecoverFinished( (int)lParam );
							  return 0;
		default:              return DefWindowProc( hwnd, Msg, wParam, lParam);
	}
	return DefWindowProc( hwnd, Msg, wParam, lParam);
}
//---------------------------------------------------------------------------
BOOL TThostFtdcTraderApi::InitMessageWindow( void )
{
	GUID        Guid;
	WNDCLASSEXW wcx;

	if( CoCreateGuid( &Guid ) == S_OK )
		swprintf( MessageWindowsClassName, L"%0.8X-%0.4X-%0.4X-%0.2X%0.2X-%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X",
										 Guid.Data1, Guid.Data2, Guid.Data3,
										 Guid.Data4[0], Guid.Data4[1], Guid.Data4[2], Guid.Data4[3],
										 Guid.Data4[4], Guid.Data4[5], Guid.Data4[6], Guid.Data4[7]);
	else
		swprintf( MessageWindowsClassName, L"CTP_API_MsgWin_%10d", UFC::GetTickCountMS());
	// Fill in the window class structure with parameters that describe the main window.
	wcx.cbSize = sizeof( wcx );                 // size of structure
	wcx.style = CS_HREDRAW |CS_VREDRAW;         // redraw if size changes
	wcx.lpfnWndProc   = TThostFtdcTraderApi::MessageWndProc;// points to window procedure
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
BOOL TThostFtdcTraderApi::CreateMessageWindow( void ) // Create the message window.
{
	FHwnd = CreateWindowW( MessageWindowsClassName,// name of window class
						   L"CTP_API_App",              // title-bar string
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
bool TThostFtdcTraderApi::SupportCTPAPI( UFC::AnsiString APIType )
{
	if ( APIType == "CTP" )
		return gCTPAPI.SupportCTPAPI();
	else if ( APIType == "HSctp" )
		return gHSctpAPI.SupportCTPAPI();
	else if ( APIType == "KNctp" )
		return gKNctpAPI.SupportCTPAPI();
	else
		return false;
}
//---------------------------------------------------------------------------
const char* TThostFtdcTraderApi::LoadCTPAPIResult( void )
{
	return FCTPLikeAPI->Reason();
}
//------------------------------------------------------------------------------
void  TThostFtdcTraderApi::RegisterSpi( TThostFtdcTraderSpi *pSpi )
{
	FListener = pSpi;
}
//------------------------------------------------------------------------------
void  TThostFtdcTraderApi::Init()
{
	FCTPLikeAPI->pInit( FHCTPAPI );
}
//------------------------------------------------------------------------------
const char*  TThostFtdcTraderApi::GetTradingDay()
{
	return FCTPLikeAPI->pGetTradingDay( FHCTPAPI );
}
//------------------------------------------------------------------------------
void  TThostFtdcTraderApi::RegisterFront(char *pszFrontAddress )
{
	FCTPLikeAPI->pRegisterFront( FHCTPAPI, pszFrontAddress );
}
//------------------------------------------------------------------------------
void  TThostFtdcTraderApi::RegisterNameServer(char *pszNsAddress)
{
	FCTPLikeAPI->pRegisterNameServer( FHCTPAPI, pszNsAddress);
}
//------------------------------------------------------------------------------
void  TThostFtdcTraderApi::SubscribePrivateTopic(THOST_TE_RESUME_TYPE nResumeType)
{
	FCTPLikeAPI->pSubscribePrivateTopic( FHCTPAPI, nResumeType );
}
//------------------------------------------------------------------------------
void  TThostFtdcTraderApi::SubscribePublicTopic(THOST_TE_RESUME_TYPE nResumeType)
{
	FCTPLikeAPI->pSubscribePublicTopic( FHCTPAPI, nResumeType );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqAuthenticate(CThostFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID)
{
	return FCTPLikeAPI->pReqAuthenticate( FHCTPAPI ,pReqAuthenticateField, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqUserLogin(CThostFtdcReqUserLoginField *pReqUserLoginField, int nRequestID)
{
	return FCTPLikeAPI->pReqUserLogin( FHCTPAPI, pReqUserLoginField,  nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqUserLogout(CThostFtdcUserLogoutField *pUserLogout, int nRequestID)
{
	return FCTPLikeAPI->pReqUserLogout( FHCTPAPI, pUserLogout, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID)
{
	return FCTPLikeAPI->pReqUserPasswordUpdate( FHCTPAPI, pUserPasswordUpdate, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqTradingAccountPasswordUpdate(CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, int nRequestID)
{
	return FCTPLikeAPI->pReqTradingAccountPasswordUpdate( FHCTPAPI, pTradingAccountPasswordUpdate, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqOrderInsert(CThostFtdcInputOrderField *pInputOrder, int nRequestID)
{
	return FCTPLikeAPI->pReqOrderInsert( FHCTPAPI, pInputOrder, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder, int nRequestID)
{
	return FCTPLikeAPI->pReqParkedOrderInsert( FHCTPAPI, pParkedOrder, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, int nRequestID)
{
	return FCTPLikeAPI->pReqParkedOrderAction( FHCTPAPI, pParkedOrderAction, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID)
{
	return FCTPLikeAPI->pReqOrderAction(FHCTPAPI, pInputOrderAction, nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQueryMaxOrderVolume(CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, int nRequestID)
{
	return FCTPLikeAPI->pReqQueryMaxOrderVolume( FHCTPAPI, pQueryMaxOrderVolume, nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, int nRequestID)
{
	return FCTPLikeAPI->pReqSettlementInfoConfirm( FHCTPAPI, pSettlementInfoConfirm, nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqRemoveParkedOrder(CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, int nRequestID)
{
	return FCTPLikeAPI->pReqRemoveParkedOrder( FHCTPAPI, pRemoveParkedOrder, nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqRemoveParkedOrderAction(CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, int nRequestID)
{
	return FCTPLikeAPI->pReqRemoveParkedOrderAction( FHCTPAPI, pRemoveParkedOrderAction, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryOrder(CThostFtdcQryOrderField *pQryOrder, int nRequestID)
{
	return FCTPLikeAPI->pReqQryOrder( FHCTPAPI, pQryOrder, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryTrade(CThostFtdcQryTradeField *pQryTrade, int nRequestID)
{
	return FCTPLikeAPI->pReqQryTrade( FHCTPAPI, pQryTrade, nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryInvestorPosition(CThostFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID)
{
	return FCTPLikeAPI->pReqQryInvestorPosition( FHCTPAPI, pQryInvestorPosition, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryTradingAccount(CThostFtdcQryTradingAccountField *pQryTradingAccount, int nRequestID)
{
	return FCTPLikeAPI->pReqQryTradingAccount( FHCTPAPI, pQryTradingAccount, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryInvestor(CThostFtdcQryInvestorField *pQryInvestor, int nRequestID)
{
	return FCTPLikeAPI->pReqQryInvestor( FHCTPAPI, pQryInvestor, nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryTradingCode(CThostFtdcQryTradingCodeField *pQryTradingCode, int nRequestID)
{
	return FCTPLikeAPI->pReqQryTradingCode( FHCTPAPI, pQryTradingCode, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryInstrumentMarginRate(CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID)
{
	return FCTPLikeAPI->pReqQryInstrumentMarginRate( FHCTPAPI, pQryInstrumentMarginRate, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryInstrumentCommissionRate(CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate, int nRequestID)
{
	return FCTPLikeAPI->pReqQryInstrumentCommissionRate( FHCTPAPI, pQryInstrumentCommissionRate, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryExchange(CThostFtdcQryExchangeField *pQryExchange, int nRequestID)
{
	return FCTPLikeAPI->pReqQryExchange( FHCTPAPI, pQryExchange, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryInstrument(CThostFtdcQryInstrumentField *pQryInstrument, int nRequestID)
{
	return FCTPLikeAPI->pReqQryInstrument( FHCTPAPI, pQryInstrument, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryDepthMarketData(CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, int nRequestID)
{
	return FCTPLikeAPI->pReqQryDepthMarketData( FHCTPAPI, pQryDepthMarketData, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQrySettlementInfo(CThostFtdcQrySettlementInfoField *pQrySettlementInfo, int nRequestID)
{
	return FCTPLikeAPI->pReqQrySettlementInfo( FHCTPAPI, pQrySettlementInfo, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryTransferBank(CThostFtdcQryTransferBankField *pQryTransferBank, int nRequestID)
{
	return FCTPLikeAPI->pReqQryTransferBank( FHCTPAPI, pQryTransferBank, nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryInvestorPositionDetail(CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, int nRequestID)
{
	return FCTPLikeAPI->pReqQryInvestorPositionDetail( FHCTPAPI, pQryInvestorPositionDetail, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryNotice(CThostFtdcQryNoticeField *pQryNotice, int nRequestID)
{
	return FCTPLikeAPI->pReqQryNotice( FHCTPAPI, pQryNotice, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQrySettlementInfoConfirm(CThostFtdcQrySettlementInfoConfirmField *pQrySettlementInfoConfirm, int nRequestID)
{
	return FCTPLikeAPI->pReqQrySettlementInfoConfirm( FHCTPAPI, pQrySettlementInfoConfirm, nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryInvestorPositionCombineDetail(CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorPositionCombineDetail, int nRequestID)
{
	return FCTPLikeAPI->pReqQryInvestorPositionCombineDetail(FHCTPAPI, pQryInvestorPositionCombineDetail,  nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryCFMMCTradingAccountKey(CThostFtdcQryCFMMCTradingAccountKeyField *pQryCFMMCTradingAccountKey, int nRequestID)
{
	return FCTPLikeAPI->pReqQryCFMMCTradingAccountKey( FHCTPAPI, pQryCFMMCTradingAccountKey,  nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryEWarrantOffset(CThostFtdcQryEWarrantOffsetField *pQryEWarrantOffset, int nRequestID)
{
	return FCTPLikeAPI->pReqQryEWarrantOffset( FHCTPAPI, pQryEWarrantOffset, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryTransferSerial(CThostFtdcQryTransferSerialField *pQryTransferSerial, int nRequestID)
{
	return FCTPLikeAPI->pReqQryTransferSerial( FHCTPAPI, pQryTransferSerial, nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryAccountregister(CThostFtdcQryAccountregisterField *pQryAccountregister, int nRequestID)
{
	return FCTPLikeAPI->pReqQryAccountregister( FHCTPAPI, pQryAccountregister, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryContractBank(CThostFtdcQryContractBankField *pQryContractBank, int nRequestID)
{
	return FCTPLikeAPI->pReqQryContractBank( FHCTPAPI, pQryContractBank, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryParkedOrder(CThostFtdcQryParkedOrderField *pQryParkedOrder, int nRequestID)
{
	return FCTPLikeAPI->pReqQryParkedOrder( FHCTPAPI, pQryParkedOrder,  nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryParkedOrderAction(CThostFtdcQryParkedOrderActionField *pQryParkedOrderAction, int nRequestID)
{
	return FCTPLikeAPI->pReqQryParkedOrderAction( FHCTPAPI, pQryParkedOrderAction, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryTradingNotice(CThostFtdcQryTradingNoticeField *pQryTradingNotice, int nRequestID)
{
	return FCTPLikeAPI->pReqQryTradingNotice( FHCTPAPI, pQryTradingNotice, nRequestID );
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryBrokerTradingParams(CThostFtdcQryBrokerTradingParamsField *pQryBrokerTradingParams, int nRequestID)
{
	return FCTPLikeAPI->pReqQryBrokerTradingParams( FHCTPAPI, pQryBrokerTradingParams, nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQryBrokerTradingAlgos(CThostFtdcQryBrokerTradingAlgosField *pQryBrokerTradingAlgos, int nRequestID)
{
	return FCTPLikeAPI->pReqQryBrokerTradingAlgos( FHCTPAPI, pQryBrokerTradingAlgos, nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqFromBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer, int nRequestID)
{
	return FCTPLikeAPI->pReqFromBankToFutureByFuture( FHCTPAPI, pReqTransfer, nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqFromFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer, int nRequestID)
{
	return FCTPLikeAPI->pReqFromFutureToBankByFuture( FHCTPAPI, pReqTransfer, nRequestID);
}
//------------------------------------------------------------------------------
int  TThostFtdcTraderApi::ReqQueryBankAccountMoneyByFuture(CThostFtdcReqQueryAccountField *pReqQueryAccount, int nRequestID)
{
	return FCTPLikeAPI->pReqQueryBankAccountMoneyByFuture( FHCTPAPI, pReqQueryAccount, nRequestID);
}
//------------------------------------------------------------------------------
void  TThostFtdcTraderApi::ReqOnRecoverFilisned( int Count )
{
	TThostFtdcTraderSpi*  Spi  = GetSpi( );
	HWND                  Hwnd = GetHWND( );

	PostMessage( Hwnd, WM_CTP_EVENT + 73, (WPARAM)Spi,(LPARAM)Count );
}
//------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
