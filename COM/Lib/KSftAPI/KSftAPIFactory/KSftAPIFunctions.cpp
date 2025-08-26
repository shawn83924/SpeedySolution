//------------------------------------------------------------------------------------------------------------------------
#include "Stdafx.h"
#include "KSftAPIComm.h"
//------------------------------------------------------------------------------------------------------------------------
HKSFT CreateKSftTraderApi( void* Owner )
{
	KS_::CThostFtdcTraderApi* KSftObject = KS_::CThostFtdcTraderApi::CreateFtdcTraderApi();
	if ( KSftObject != NULL )
	{
		KSftHandle* hKSFT = new KSftHandle();
		memset( hKSFT, 0, sizeof(KSftHandle) );
		hKSFT->KSftObj     = KSftObject;
		hKSFT->KSftHandler = new KSftAPIHandler( hKSFT );
		hKSFT->FOwner      = Owner;
		hKSFT->KSftObj->RegisterSpi( hKSFT->KSftHandler );
		return (HKSFT)hKSFT;
	}
	return NULL;
}
//------------------------------------------------------------------------------------------------------------------------
void ReleaseKSftTraderApi( HKSFT hKsft )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->KSftObj->RegisterSpi( NULL );
	hKSFT->KSftObj->Release( );
	delete hKSFT->KSftHandler;
	delete hKSFT;
}
//------------------------------------------------------------------------------------------------------------------------
void Init( HKSFT hKsft )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->KSftObj->Init( );
}
//------------------------------------------------------------------------------------------------------------------------
const char *GetTradingDay( HKSFT hKsft )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->GetTradingDay( );
}
//------------------------------------------------------------------------------------------------------------------------
void RegisterFront( HKSFT hKsft, char *pszFrontAddress )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->KSftObj->RegisterFront( pszFrontAddress );
}
//------------------------------------------------------------------------------------------------------------------------
void RegisterNameServer( HKSFT hKsft, char *pszNsAddress )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->KSftObj->RegisterNameServer( pszNsAddress );
}
//------------------------------------------------------------------------------------------------------------------------
void RegisterFensUserInfo( HKSFT hKsft, KS_::CThostFtdcFensUserInfoField * pFensUserInfo )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->KSftObj->RegisterFensUserInfo( pFensUserInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void SubscribePrivateTopic( HKSFT hKsft, KS_::THOST_TE_RESUME_TYPE nResumeType )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->KSftObj->SubscribePrivateTopic( nResumeType );
}
//------------------------------------------------------------------------------------------------------------------------
void SubscribePublicTopic( HKSFT hKsft, KS_::THOST_TE_RESUME_TYPE nResumeType )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->KSftObj->SubscribePublicTopic( nResumeType );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqAuthenticate( HKSFT hKsft, KS_::CThostFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqAuthenticate( pReqAuthenticateField, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqUserLogin( HKSFT hKsft, KS_::CThostFtdcReqUserLoginField *pReqUserLoginField, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqUserLogin( pReqUserLoginField, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqUserLogout( HKSFT hKsft, KS_::CThostFtdcUserLogoutField *pUserLogout, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqUserLogout( pUserLogout, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqUserPasswordUpdate( HKSFT hKsft, KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqUserPasswordUpdate( pUserPasswordUpdate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqTradingAccountPasswordUpdate( HKSFT hKsft, KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqTradingAccountPasswordUpdate( pTradingAccountPasswordUpdate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqOrderInsert( HKSFT hKsft, KS_::CThostFtdcInputOrderField *pInputOrder, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqOrderInsert( pInputOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqParkedOrderInsert( HKSFT hKsft, KS_::CThostFtdcParkedOrderField *pParkedOrder, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqParkedOrderInsert( pParkedOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqParkedOrderAction( HKSFT hKsft, KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqParkedOrderAction( pParkedOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqOrderAction( HKSFT hKsft, KS_::CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqOrderAction( pInputOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQueryMaxOrderVolume( HKSFT hKsft, KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQueryMaxOrderVolume( pQueryMaxOrderVolume, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqSettlementInfoConfirm( HKSFT hKsft, KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqSettlementInfoConfirm( pSettlementInfoConfirm, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqRemoveParkedOrder( HKSFT hKsft, KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqRemoveParkedOrder( pRemoveParkedOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqRemoveParkedOrderAction( HKSFT hKsft, KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqRemoveParkedOrderAction( pRemoveParkedOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqExecOrderInsert( HKSFT hKsft, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqExecOrderInsert( pInputExecOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqExecOrderAction( HKSFT hKsft, KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqExecOrderAction( pInputExecOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqForQuoteInsert( HKSFT hKsft, KS_::CThostFtdcInputForQuoteField *pInputForQuote, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqForQuoteInsert( pInputForQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQuoteInsert( HKSFT hKsft, KS_::CThostFtdcInputQuoteField *pInputQuote, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQuoteInsert( pInputQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQuoteAction( HKSFT hKsft, KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQuoteAction( pInputQuoteAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryOrder( HKSFT hKsft, KS_::CThostFtdcQryOrderField *pQryOrder, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryOrder( pQryOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryTrade( HKSFT hKsft, KS_::CThostFtdcQryTradeField *pQryTrade, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryTrade( pQryTrade, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInvestorPosition( HKSFT hKsft, KS_::CThostFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryInvestorPosition( pQryInvestorPosition, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryTradingAccount( HKSFT hKsft, KS_::CThostFtdcQryTradingAccountField *pQryTradingAccount, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryTradingAccount( pQryTradingAccount, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInvestor( HKSFT hKsft, KS_::CThostFtdcQryInvestorField *pQryInvestor, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryInvestor( pQryInvestor, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryTradingCode( HKSFT hKsft, KS_::CThostFtdcQryTradingCodeField *pQryTradingCode, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryTradingCode( pQryTradingCode, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInstrumentMarginRate( HKSFT hKsft, KS_::CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryInstrumentMarginRate( pQryInstrumentMarginRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInstrumentCommissionRate( HKSFT hKsft, KS_::CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryInstrumentCommissionRate( pQryInstrumentCommissionRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryExchange( HKSFT hKsft, KS_::CThostFtdcQryExchangeField *pQryExchange, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryExchange( pQryExchange, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryProduct( HKSFT hKsft, KS_::CThostFtdcQryProductField *pQryProduct, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryProduct( pQryProduct, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInstrument( HKSFT hKsft, KS_::CThostFtdcQryInstrumentField *pQryInstrument, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryInstrument( pQryInstrument, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryDepthMarketData( HKSFT hKsft, KS_::CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryDepthMarketData( pQryDepthMarketData, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQrySettlementInfo( HKSFT hKsft, KS_::CThostFtdcQrySettlementInfoField *pQrySettlementInfo, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQrySettlementInfo( pQrySettlementInfo, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryTransferBank( HKSFT hKsft, KS_::CThostFtdcQryTransferBankField *pQryTransferBank, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryTransferBank( pQryTransferBank, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInvestorPositionDetail( HKSFT hKsft, KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryInvestorPositionDetail( pQryInvestorPositionDetail, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryNotice( HKSFT hKsft, KS_::CThostFtdcQryNoticeField *pQryNotice, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryNotice( pQryNotice, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQrySettlementInfoConfirm( HKSFT hKsft, KS_::CThostFtdcQrySettlementInfoConfirmField *pQrySettlementInfoConfirm, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQrySettlementInfoConfirm( pQrySettlementInfoConfirm, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInvestorPositionCombineDetail( HKSFT hKsft, KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorPositionCombineDetail, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryInvestorPositionCombineDetail( pQryInvestorPositionCombineDetail, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryCFMMCTradingAccountKey( HKSFT hKsft, KS_::CThostFtdcQryCFMMCTradingAccountKeyField *pQryCFMMCTradingAccountKey, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryCFMMCTradingAccountKey( pQryCFMMCTradingAccountKey, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryEWarrantOffset( HKSFT hKsft, KS_::CThostFtdcQryEWarrantOffsetField *pQryEWarrantOffset, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryEWarrantOffset( pQryEWarrantOffset, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInvestorProductGroupMargin( HKSFT hKsft, KS_::CThostFtdcQryInvestorProductGroupMarginField *pQryInvestorProductGroupMargin, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryInvestorProductGroupMargin( pQryInvestorProductGroupMargin, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryExchangeMarginRate( HKSFT hKsft, KS_::CThostFtdcQryExchangeMarginRateField *pQryExchangeMarginRate, int nRequestID)
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryExchangeMarginRate( pQryExchangeMarginRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryExchangeMarginRateAdjust( HKSFT hKsft, KS_::CThostFtdcQryExchangeMarginRateAdjustField *pQryExchangeMarginRateAdjust, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryExchangeMarginRateAdjust( pQryExchangeMarginRateAdjust, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryExchangeRate( HKSFT hKsft, KS_::CThostFtdcQryExchangeRateField *pQryExchangeRate, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryExchangeRate( pQryExchangeRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQrySecAgentACIDMap( HKSFT hKsft, KS_::CThostFtdcQrySecAgentACIDMapField *pQrySecAgentACIDMap, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQrySecAgentACIDMap( pQrySecAgentACIDMap, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryOptionInstrTradeCost( HKSFT hKsft, KS_::CThostFtdcQryOptionInstrTradeCostField *pQryOptionInstrTradeCost, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryOptionInstrTradeCost( pQryOptionInstrTradeCost, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryOptionInstrCommRate( HKSFT hKsft, KS_::CThostFtdcQryOptionInstrCommRateField *pQryOptionInstrCommRate, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryOptionInstrCommRate( pQryOptionInstrCommRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryExecOrder( HKSFT hKsft, KS_::CThostFtdcQryExecOrderField *pQryExecOrder, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryExecOrder( pQryExecOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryForQuote( HKSFT hKsft, KS_::CThostFtdcQryForQuoteField *pQryForQuote, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryForQuote( pQryForQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryQuote( HKSFT hKsft, KS_::CThostFtdcQryQuoteField *pQryQuote, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryQuote( pQryQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryTransferSerial( HKSFT hKsft, KS_::CThostFtdcQryTransferSerialField *pQryTransferSerial, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryTransferSerial( pQryTransferSerial, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryAccountregister( HKSFT hKsft, KS_::CThostFtdcQryAccountregisterField *pQryAccountregister, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryAccountregister( pQryAccountregister, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryContractBank( HKSFT hKsft, KS_::CThostFtdcQryContractBankField *pQryContractBank, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryContractBank( pQryContractBank, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryParkedOrder( HKSFT hKsft, KS_::CThostFtdcQryParkedOrderField *pQryParkedOrder, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryParkedOrder( pQryParkedOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryParkedOrderAction( HKSFT hKsft, KS_::CThostFtdcQryParkedOrderActionField *pQryParkedOrderAction, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryParkedOrderAction( pQryParkedOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryTradingNotice( HKSFT hKsft, KS_::CThostFtdcQryTradingNoticeField *pQryTradingNotice, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryTradingNotice( pQryTradingNotice, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryBrokerTradingParams( HKSFT hKsft, KS_::CThostFtdcQryBrokerTradingParamsField *pQryBrokerTradingParams, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryBrokerTradingParams( pQryBrokerTradingParams, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryBrokerTradingAlgos( HKSFT hKsft, KS_::CThostFtdcQryBrokerTradingAlgosField *pQryBrokerTradingAlgos, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQryBrokerTradingAlgos( pQryBrokerTradingAlgos, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqFromBankToFutureByFuture( HKSFT hKsft, KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqFromBankToFutureByFuture( pReqTransfer, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqFromFutureToBankByFuture( HKSFT hKsft, KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqFromFutureToBankByFuture( pReqTransfer, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQueryBankAccountMoneyByFuture( HKSFT hKsft, KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQueryBankAccountMoneyByFuture( pReqQueryAccount, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQueryInvestorOpenPosition( HKSFT hKsft, KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorOpenPosition, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQueryInvestorOpenPosition( pQryInvestorOpenPosition, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQueryInvestorOpenCombinePosition( HKSFT hKsft, KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorOpenCombinePosition, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqQueryInvestorOpenCombinePosition( pQryInvestorOpenCombinePosition, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
void* LoadExtApi( HKSFT hKsft, void * spi, const char *ExtApiName )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->LoadExtApi( spi, ExtApiName );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqBulkCancelOrder( HKSFT hKsft, KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, int nRequestID )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	return hKSFT->KSftObj->ReqBulkCancelOrder( pBulkCancelOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnFrontConnected( HKSFT hKsft, pfOnFrontConnected callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnFrontConnected = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnFrontDisconnected( HKSFT hKsft, pfOnFrontDisconnected callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnFrontDisconnected = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnHeartBeatWarning( HKSFT hKsft, pfOnHeartBeatWarning callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnHeartBeatWarning = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspAuthenticate( HKSFT hKsft, pfOnRspAuthenticate callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspAuthenticate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspUserLogin( HKSFT hKsft, pfOnRspUserLogin callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspUserLogin = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspUserLogout( HKSFT hKsft, pfOnRspUserLogout callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspUserLogout = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspUserPasswordUpdate( HKSFT hKsft, pfOnRspUserPasswordUpdate callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspUserPasswordUpdate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspTradingAccountPasswordUpdate( HKSFT hKsft, pfOnRspTradingAccountPasswordUpdate callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspTradingAccountPasswordUpdate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspOrderInsert( HKSFT hKsft, pfOnRspOrderInsert callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspOrderInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspParkedOrderInsert( HKSFT hKsft, pfOnRspParkedOrderInsert callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspParkedOrderInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspParkedOrderAction( HKSFT hKsft, pfOnRspParkedOrderAction callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspParkedOrderAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspOrderAction( HKSFT hKsft, pfOnRspOrderAction callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspOrderAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQueryMaxOrderVolume( HKSFT hKsft, pfOnRspQueryMaxOrderVolume callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQueryMaxOrderVolume = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspSettlementInfoConfirm( HKSFT hKsft, pfOnRspSettlementInfoConfirm callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspSettlementInfoConfirm = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspRemoveParkedOrder( HKSFT hKsft, pfOnRspRemoveParkedOrder callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspRemoveParkedOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspRemoveParkedOrderAction( HKSFT hKsft, pfOnRspRemoveParkedOrderAction callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspRemoveParkedOrderAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspExecOrderInsert( HKSFT hKsft, pfOnRspExecOrderInsert callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspExecOrderInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspExecOrderAction( HKSFT hKsft, pfOnRspExecOrderAction callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspExecOrderAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspForQuoteInsert( HKSFT hKsft, pfOnRspForQuoteInsert callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspForQuoteInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQuoteInsert( HKSFT hKsft, pfOnRspQuoteInsert callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQuoteInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQuoteAction( HKSFT hKsft, pfOnRspQuoteAction callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQuoteAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryOrder( HKSFT hKsft, pfOnRspQryOrder callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryTrade( HKSFT hKsft, pfOnRspQryTrade callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryTrade = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInvestorPosition( HKSFT hKsft, pfOnRspQryInvestorPosition callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryInvestorPosition = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryTradingAccount( HKSFT hKsft, pfOnRspQryTradingAccount callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryTradingAccount = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInvestor( HKSFT hKsft, pfOnRspQryInvestor callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryInvestor = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryTradingCode( HKSFT hKsft, pfOnRspQryTradingCode callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryTradingCode = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInstrumentMarginRate( HKSFT hKsft, pfOnRspQryInstrumentMarginRate callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryInstrumentMarginRate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInstrumentCommissionRate( HKSFT hKsft, pfOnRspQryInstrumentCommissionRate callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryInstrumentCommissionRate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryExchange( HKSFT hKsft, pfOnRspQryExchange callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryExchange = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryProduct( HKSFT hKsft, pfOnRspQryProduct callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryProduct = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInstrument( HKSFT hKsft, pfOnRspQryInstrument callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryInstrument = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryDepthMarketData( HKSFT hKsft, pfOnRspQryDepthMarketData callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryDepthMarketData = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQrySettlementInfo( HKSFT hKsft, pfOnRspQrySettlementInfo callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQrySettlementInfo = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryTransferBank( HKSFT hKsft, pfOnRspQryTransferBank callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryTransferBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInvestorPositionDetail( HKSFT hKsft, pfOnRspQryInvestorPositionDetail callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryInvestorPositionDetail = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryNotice( HKSFT hKsft, pfOnRspQryNotice callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryNotice = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQrySettlementInfoConfirm( HKSFT hKsft, pfOnRspQrySettlementInfoConfirm callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQrySettlementInfoConfirm = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInvestorPositionCombineDetail( HKSFT hKsft, pfOnRspQryInvestorPositionCombineDetail callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryInvestorPositionCombineDetail = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryCFMMCTradingAccountKey( HKSFT hKsft, pfOnRspQryCFMMCTradingAccountKey callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryCFMMCTradingAccountKey = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryEWarrantOffset( HKSFT hKsft, pfOnRspQryEWarrantOffset callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryEWarrantOffset = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInvestorProductGroupMargin( HKSFT hKsft, pfOnRspQryInvestorProductGroupMargin callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryInvestorProductGroupMargin = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryExchangeMarginRate( HKSFT hKsft, pfOnRspQryExchangeMarginRate callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryExchangeMarginRate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryExchangeMarginRateAdjust( HKSFT hKsft, pfOnRspQryExchangeMarginRateAdjust callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryExchangeMarginRateAdjust = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryExchangeRate( HKSFT hKsft, pfOnRspQryExchangeRate callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryExchangeRate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQrySecAgentACIDMap( HKSFT hKsft, pfOnRspQrySecAgentACIDMap callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQrySecAgentACIDMap = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryOptionInstrTradeCost( HKSFT hKsft, pfOnRspQryOptionInstrTradeCost callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryOptionInstrTradeCost = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryOptionInstrCommRate( HKSFT hKsft, pfOnRspQryOptionInstrCommRate callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryOptionInstrCommRate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryExecOrder( HKSFT hKsft, pfOnRspQryExecOrder callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryExecOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryForQuote( HKSFT hKsft, pfOnRspQryForQuote callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryForQuote = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryQuote( HKSFT hKsft, pfOnRspQryQuote callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryQuote = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryTransferSerial( HKSFT hKsft, pfOnRspQryTransferSerial callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryTransferSerial = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryAccountregister( HKSFT hKsft, pfOnRspQryAccountregister callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryAccountregister = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspError( HKSFT hKsft, pfOnRspError callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspError = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnOrder( HKSFT hKsft, pfOnRtnOrder callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnTrade( HKSFT hKsft, pfOnRtnTrade callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnTrade = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnOrderInsert( HKSFT hKsft, pfOnErrRtnOrderInsert callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnErrRtnOrderInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnOrderAction( HKSFT hKsft, pfOnErrRtnOrderAction callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnErrRtnOrderAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnInstrumentStatus( HKSFT hKsft, pfOnRtnInstrumentStatus callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnInstrumentStatus = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnTradingNotice( HKSFT hKsft, pfOnRtnTradingNotice callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnTradingNotice = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnErrorConditionalOrder( HKSFT hKsft, pfOnRtnErrorConditionalOrder callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnErrorConditionalOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnExecOrder( HKSFT hKsft, pfOnRtnExecOrder callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnExecOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnExecOrderInsert( HKSFT hKsft, pfOnErrRtnExecOrderInsert callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnErrRtnExecOrderInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnExecOrderAction( HKSFT hKsft, pfOnErrRtnExecOrderAction callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnErrRtnExecOrderAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnForQuoteInsert( HKSFT hKsft, pfOnErrRtnForQuoteInsert callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnErrRtnForQuoteInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnQuote( HKSFT hKsft, pfOnRtnQuote callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnQuote = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnQuoteInsert( HKSFT hKsft, pfOnErrRtnQuoteInsert callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnErrRtnQuoteInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnQuoteAction( HKSFT hKsft, pfOnErrRtnQuoteAction callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnErrRtnQuoteAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryContractBank( HKSFT hKsft, pfOnRspQryContractBank callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryContractBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryParkedOrder( HKSFT hKsft, pfOnRspQryParkedOrder callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryParkedOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryParkedOrderAction( HKSFT hKsft, pfOnRspQryParkedOrderAction callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryParkedOrderAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryTradingNotice( HKSFT hKsft, pfOnRspQryTradingNotice callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryTradingNotice = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryBrokerTradingParams( HKSFT hKsft, pfOnRspQryBrokerTradingParams callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryBrokerTradingParams = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryBrokerTradingAlgos( HKSFT hKsft, pfOnRspQryBrokerTradingAlgos callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryBrokerTradingAlgos = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnFromBankToFutureByBank( HKSFT hKsft, pfOnRtnFromBankToFutureByBank callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnFromBankToFutureByBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnFromFutureToBankByBank( HKSFT hKsft, pfOnRtnFromFutureToBankByBank callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnFromFutureToBankByBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnRepealFromBankToFutureByBank( HKSFT hKsft, pfOnRtnRepealFromBankToFutureByBank callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnRepealFromBankToFutureByBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnRepealFromFutureToBankByBank( HKSFT hKsft, pfOnRtnRepealFromFutureToBankByBank callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnRepealFromFutureToBankByBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnFromBankToFutureByFuture( HKSFT hKsft, pfOnRtnFromBankToFutureByFuture callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnFromBankToFutureByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnFromFutureToBankByFuture( HKSFT hKsft, pfOnRtnFromFutureToBankByFuture callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnFromFutureToBankByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnRepealFromBankToFutureByFutureManual( HKSFT hKsft, pfOnRtnRepealFromBankToFutureByFutureManual callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnRepealFromBankToFutureByFutureManual = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnRepealFromFutureToBankByFutureManual( HKSFT hKsft, pfOnRtnRepealFromFutureToBankByFutureManual callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnRepealFromFutureToBankByFutureManual = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnQueryBankBalanceByFuture( HKSFT hKsft, pfOnRtnQueryBankBalanceByFuture callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnQueryBankBalanceByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnBankToFutureByFuture( HKSFT hKsft, pfOnErrRtnBankToFutureByFuture callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnErrRtnBankToFutureByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnFutureToBankByFuture( HKSFT hKsft, pfOnErrRtnFutureToBankByFuture callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnErrRtnFutureToBankByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnRepealBankToFutureByFutureManual( HKSFT hKsft, pfOnErrRtnRepealBankToFutureByFutureManual callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnErrRtnRepealBankToFutureByFutureManual = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnRepealFutureToBankByFutureManual( HKSFT hKsft, pfOnErrRtnRepealFutureToBankByFutureManual callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnErrRtnRepealFutureToBankByFutureManual = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnQueryBankBalanceByFuture( HKSFT hKsft, pfOnErrRtnQueryBankBalanceByFuture callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnErrRtnQueryBankBalanceByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnRepealFromBankToFutureByFuture( HKSFT hKsft, pfOnRtnRepealFromBankToFutureByFuture callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnRepealFromBankToFutureByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnRepealFromFutureToBankByFuture( HKSFT hKsft, pfOnRtnRepealFromFutureToBankByFuture callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnRepealFromFutureToBankByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspFromBankToFutureByFuture( HKSFT hKsft, pfOnRspFromBankToFutureByFuture callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspFromBankToFutureByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspFromFutureToBankByFuture( HKSFT hKsft, pfOnRspFromFutureToBankByFuture callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspFromFutureToBankByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQueryBankAccountMoneyByFuture( HKSFT hKsft, pfOnRspQueryBankAccountMoneyByFuture callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQueryBankAccountMoneyByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnOpenAccountByBank( HKSFT hKsft, pfOnRtnOpenAccountByBank callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnOpenAccountByBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnCancelAccountByBank( HKSFT hKsft, pfOnRtnCancelAccountByBank callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnCancelAccountByBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnChangeAccountByBank( HKSFT hKsft, pfOnRtnChangeAccountByBank callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRtnChangeAccountByBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInvestorOpenPosition( HKSFT hKsft, pfOnRspQryInvestorOpenPosition callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryInvestorOpenPosition = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInvestorOpenCombinePosition( HKSFT hKsft, pfOnRspQryInvestorOpenCombinePosition callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspQryInvestorOpenCombinePosition = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspBulkCancelOrder( HKSFT hKsft, pfOnRspBulkCancelOrder callback )
{
	KSftHandle* hKSFT = reinterpret_cast<KSftHandle*>(hKsft);
	hKSFT->OnRspBulkCancelOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
