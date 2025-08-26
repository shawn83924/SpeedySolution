//------------------------------------------------------------------------------------------------------------------------
#include "Stdafx.h"
#include "KStrdAPIComm.h"
//------------------------------------------------------------------------------------------------------------------------
//
// Export CThostFtdcTraderApi, CKSVocApi, CKSOptionApi methods into C style.
//
//------------------------------------------------------------------------------------------------------------------------
// ---- CThostFtdcTraderApi ----
//------------------------------------------------------------------------------------------------------------------------
HKSTRD CreateKStrdTraderApi( void* Owner )
{
	KS_::CThostFtdcTraderApi* KStrdObject = KS_::CThostFtdcTraderApi::CreateFtdcTraderApi();
	if ( KStrdObject == NULL )
		return NULL;
	KStrdHandle* hKSTRD = new KStrdHandle();
	memset( hKSTRD, 0, sizeof(KStrdHandle) );
	hKSTRD->KStrdObj          = KStrdObject;
	KStrdAPIHandler* KStrdHnd = new KStrdAPIHandler( hKSTRD );
	hKSTRD->KStrdHandler      = KStrdHnd;
	hKSTRD->FOwner            = Owner;
	hKSTRD->KStrdObj->RegisterSpi( (KS_::CThostFtdcTraderSpi*)hKSTRD->KStrdHandler );
	// LoadExtApi KS_OPT_API & KS_VOC_API
	hKSTRD->KSoptObj = (KS_::CKSOptionApi*)hKSTRD->KStrdObj->LoadExtApi( (void*)dynamic_cast<KS_::CKSOptionSpi*>(KStrdHnd), KS_::KS_OPT_API );
	hKSTRD->KSvocObj = (KS_::CKSVocApi*)hKSTRD->KStrdObj->LoadExtApi( (void*)dynamic_cast<KS_::CKSVocSpi*>(KStrdHnd), KS_::KS_VOC_API );
	if ( hKSTRD->KSoptObj == NULL || hKSTRD->KSvocObj == NULL )
		return NULL;
	return (HKSTRD)hKSTRD;
}
//------------------------------------------------------------------------------------------------------------------------
void ReleaseKStrdTraderApi( HKSTRD hKStrd )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->KStrdObj->RegisterSpi( NULL );
	hKSTRD->KStrdObj->Release( );
	delete hKSTRD->KStrdHandler;
	delete hKSTRD;
}
//------------------------------------------------------------------------------------------------------------------------
void Init( HKSTRD hKStrd )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->KStrdObj->Init( );
}
//------------------------------------------------------------------------------------------------------------------------
const char *GetTradingDay( HKSTRD hKStrd )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->GetTradingDay( );
}
//------------------------------------------------------------------------------------------------------------------------
void RegisterFront( HKSTRD hKStrd, char *pszFrontAddress )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->KStrdObj->RegisterFront( pszFrontAddress );
}
//------------------------------------------------------------------------------------------------------------------------
void RegisterNameServer( HKSTRD hKStrd, char *pszNsAddress )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->KStrdObj->RegisterNameServer( pszNsAddress );
}
//------------------------------------------------------------------------------------------------------------------------
void RegisterFensUserInfo( HKSTRD hKStrd, KS_::CThostFtdcFensUserInfoField * pFensUserInfo )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->KStrdObj->RegisterFensUserInfo( pFensUserInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void SubscribePrivateTopic( HKSTRD hKStrd, KS_::THOST_TE_RESUME_TYPE nResumeType )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->KStrdObj->SubscribePrivateTopic( nResumeType );
}
//------------------------------------------------------------------------------------------------------------------------
void SubscribePublicTopic( HKSTRD hKStrd, KS_::THOST_TE_RESUME_TYPE nResumeType )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->KStrdObj->SubscribePublicTopic( nResumeType );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqAuthenticate( HKSTRD hKStrd, KS_::CThostFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqAuthenticate( pReqAuthenticateField, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqUserLogin( HKSTRD hKStrd, KS_::CThostFtdcReqUserLoginField *pReqUserLoginField, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqUserLogin( pReqUserLoginField, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqUserLogout( HKSTRD hKStrd, KS_::CThostFtdcUserLogoutField *pUserLogout, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqUserLogout( pUserLogout, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqUserPasswordUpdate( HKSTRD hKStrd, KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqUserPasswordUpdate( pUserPasswordUpdate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqTradingAccountPasswordUpdate( HKSTRD hKStrd, KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqTradingAccountPasswordUpdate( pTradingAccountPasswordUpdate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqOrderInsert( HKSTRD hKStrd, KS_::CThostFtdcInputOrderField *pInputOrder, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqOrderInsert( pInputOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqParkedOrderInsert( HKSTRD hKStrd, KS_::CThostFtdcParkedOrderField *pParkedOrder, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqParkedOrderInsert( pParkedOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqParkedOrderAction( HKSTRD hKStrd, KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqParkedOrderAction( pParkedOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqOrderAction( HKSTRD hKStrd, KS_::CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqOrderAction( pInputOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQueryMaxOrderVolume( HKSTRD hKStrd, KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQueryMaxOrderVolume( pQueryMaxOrderVolume, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqSettlementInfoConfirm( HKSTRD hKStrd, KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqSettlementInfoConfirm( pSettlementInfoConfirm, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqRemoveParkedOrder( HKSTRD hKStrd, KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqRemoveParkedOrder( pRemoveParkedOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqRemoveParkedOrderAction( HKSTRD hKStrd, KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqRemoveParkedOrderAction( pRemoveParkedOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqExecOrderInsert( HKSTRD hKStrd, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqExecOrderInsert( pInputExecOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqExecOrderAction( HKSTRD hKStrd, KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqExecOrderAction( pInputExecOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqForQuoteInsert( HKSTRD hKStrd, KS_::CThostFtdcInputForQuoteField *pInputForQuote, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqForQuoteInsert( pInputForQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQuoteInsert( HKSTRD hKStrd, KS_::CThostFtdcInputQuoteField *pInputQuote, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQuoteInsert( pInputQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQuoteAction( HKSTRD hKStrd, KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQuoteAction( pInputQuoteAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryOrder( HKSTRD hKStrd, KS_::CThostFtdcQryOrderField *pQryOrder, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryOrder( pQryOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryTrade( HKSTRD hKStrd, KS_::CThostFtdcQryTradeField *pQryTrade, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryTrade( pQryTrade, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInvestorPosition( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryInvestorPosition( pQryInvestorPosition, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryTradingAccount( HKSTRD hKStrd, KS_::CThostFtdcQryTradingAccountField *pQryTradingAccount, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryTradingAccount( pQryTradingAccount, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInvestor( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorField *pQryInvestor, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryInvestor( pQryInvestor, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryTradingCode( HKSTRD hKStrd, KS_::CThostFtdcQryTradingCodeField *pQryTradingCode, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryTradingCode( pQryTradingCode, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInstrumentMarginRate( HKSTRD hKStrd, KS_::CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryInstrumentMarginRate( pQryInstrumentMarginRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInstrumentCommissionRate( HKSTRD hKStrd, KS_::CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryInstrumentCommissionRate( pQryInstrumentCommissionRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryExchange( HKSTRD hKStrd, KS_::CThostFtdcQryExchangeField *pQryExchange, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryExchange( pQryExchange, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryProduct( HKSTRD hKStrd, KS_::CThostFtdcQryProductField *pQryProduct, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryProduct( pQryProduct, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInstrument( HKSTRD hKStrd, KS_::CThostFtdcQryInstrumentField *pQryInstrument, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryInstrument( pQryInstrument, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryDepthMarketData( HKSTRD hKStrd, KS_::CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryDepthMarketData( pQryDepthMarketData, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQrySettlementInfo( HKSTRD hKStrd, KS_::CThostFtdcQrySettlementInfoField *pQrySettlementInfo, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQrySettlementInfo( pQrySettlementInfo, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryTransferBank( HKSTRD hKStrd, KS_::CThostFtdcQryTransferBankField *pQryTransferBank, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryTransferBank( pQryTransferBank, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInvestorPositionDetail( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryInvestorPositionDetail( pQryInvestorPositionDetail, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryNotice( HKSTRD hKStrd, KS_::CThostFtdcQryNoticeField *pQryNotice, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryNotice( pQryNotice, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQrySettlementInfoConfirm( HKSTRD hKStrd, KS_::CThostFtdcQrySettlementInfoConfirmField *pQrySettlementInfoConfirm, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQrySettlementInfoConfirm( pQrySettlementInfoConfirm, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInvestorPositionCombineDetail( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorPositionCombineDetail, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryInvestorPositionCombineDetail( pQryInvestorPositionCombineDetail, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryCFMMCTradingAccountKey( HKSTRD hKStrd, KS_::CThostFtdcQryCFMMCTradingAccountKeyField *pQryCFMMCTradingAccountKey, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryCFMMCTradingAccountKey( pQryCFMMCTradingAccountKey, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryEWarrantOffset( HKSTRD hKStrd, KS_::CThostFtdcQryEWarrantOffsetField *pQryEWarrantOffset, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryEWarrantOffset( pQryEWarrantOffset, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryInvestorProductGroupMargin( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorProductGroupMarginField *pQryInvestorProductGroupMargin, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryInvestorProductGroupMargin( pQryInvestorProductGroupMargin, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryExchangeMarginRate( HKSTRD hKStrd, KS_::CThostFtdcQryExchangeMarginRateField *pQryExchangeMarginRate, int nRequestID)
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryExchangeMarginRate( pQryExchangeMarginRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryExchangeMarginRateAdjust( HKSTRD hKStrd, KS_::CThostFtdcQryExchangeMarginRateAdjustField *pQryExchangeMarginRateAdjust, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryExchangeMarginRateAdjust( pQryExchangeMarginRateAdjust, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryExchangeRate( HKSTRD hKStrd, KS_::CThostFtdcQryExchangeRateField *pQryExchangeRate, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryExchangeRate( pQryExchangeRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQrySecAgentACIDMap( HKSTRD hKStrd, KS_::CThostFtdcQrySecAgentACIDMapField *pQrySecAgentACIDMap, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQrySecAgentACIDMap( pQrySecAgentACIDMap, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryOptionInstrTradeCost( HKSTRD hKStrd, KS_::CThostFtdcQryOptionInstrTradeCostField *pQryOptionInstrTradeCost, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryOptionInstrTradeCost( pQryOptionInstrTradeCost, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryOptionInstrCommRate( HKSTRD hKStrd, KS_::CThostFtdcQryOptionInstrCommRateField *pQryOptionInstrCommRate, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryOptionInstrCommRate( pQryOptionInstrCommRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryExecOrder( HKSTRD hKStrd, KS_::CThostFtdcQryExecOrderField *pQryExecOrder, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryExecOrder( pQryExecOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryForQuote( HKSTRD hKStrd, KS_::CThostFtdcQryForQuoteField *pQryForQuote, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryForQuote( pQryForQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryQuote( HKSTRD hKStrd, KS_::CThostFtdcQryQuoteField *pQryQuote, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryQuote( pQryQuote, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryTransferSerial( HKSTRD hKStrd, KS_::CThostFtdcQryTransferSerialField *pQryTransferSerial, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryTransferSerial( pQryTransferSerial, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryAccountregister( HKSTRD hKStrd, KS_::CThostFtdcQryAccountregisterField *pQryAccountregister, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryAccountregister( pQryAccountregister, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryContractBank( HKSTRD hKStrd, KS_::CThostFtdcQryContractBankField *pQryContractBank, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryContractBank( pQryContractBank, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryParkedOrder( HKSTRD hKStrd, KS_::CThostFtdcQryParkedOrderField *pQryParkedOrder, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryParkedOrder( pQryParkedOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryParkedOrderAction( HKSTRD hKStrd, KS_::CThostFtdcQryParkedOrderActionField *pQryParkedOrderAction, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryParkedOrderAction( pQryParkedOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryTradingNotice( HKSTRD hKStrd, KS_::CThostFtdcQryTradingNoticeField *pQryTradingNotice, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryTradingNotice( pQryTradingNotice, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryBrokerTradingParams( HKSTRD hKStrd, KS_::CThostFtdcQryBrokerTradingParamsField *pQryBrokerTradingParams, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryBrokerTradingParams( pQryBrokerTradingParams, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQryBrokerTradingAlgos( HKSTRD hKStrd, KS_::CThostFtdcQryBrokerTradingAlgosField *pQryBrokerTradingAlgos, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryBrokerTradingAlgos( pQryBrokerTradingAlgos, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQueryCFMMCTradingAccountToken( HKSTRD hKStrd, KS_::CThostFtdcQueryCFMMCTradingAccountTokenField *pQueryCFMMCTradingAccountToken, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQueryCFMMCTradingAccountToken( pQueryCFMMCTradingAccountToken, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqFromBankToFutureByFuture( HKSTRD hKStrd, KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqFromBankToFutureByFuture( pReqTransfer, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqFromFutureToBankByFuture( HKSTRD hKStrd, KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqFromFutureToBankByFuture( pReqTransfer, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
int ReqQueryBankAccountMoneyByFuture( HKSTRD hKStrd, KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQueryBankAccountMoneyByFuture( pReqQueryAccount, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
// v2.3.50212 // 20150410 by Tim Lin
//------------------------------------------------------------------------------------------------------------------------
///用戶端發出鎖定請求
int ReqLockInsert( HKSTRD hKStrd, KS_::CThostFtdcInputLockField *pInputLock, int nRequestID)
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqLockInsert( pInputLock, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///用戶端發出查詢鎖定請求
int ReqQryLock( HKSTRD hKStrd, KS_::CThostFtdcQryLockField *pQryLock, int nRequestID)
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryLock( pQryLock, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///用戶端發出查詢鎖定證券倉位請求
int ReqQryLockPosition( HKSTRD hKStrd, KS_::CThostFtdcQryLockPositionField *pQryLockPosition, int nRequestID)
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KStrdObj->ReqQryLockPosition( pQryLockPosition, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
// ---- CKSOptionApi ----
//------------------------------------------------------------------------------------------------------------------------
// v2.3.50212 // 20150410 by Tim Lin
//------------------------------------------------------------------------------------------------------------------------
///查詢股指現貨指數
int ReqQryIndexPrice( HKSTRD hKStrd, KS_::CKSQryIndexPriceField *pQryIndexPrice, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSoptObj->ReqQryIndexPrice( pQryIndexPrice, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///查詢期權合約保障系數
int ReqQryOptionInstrGuard( HKSTRD hKStrd, KS_::CKSQryOptionInstrGuardField *pQryOptionInstrGuard, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSoptObj->ReqQryOptionInstrGuard( pQryOptionInstrGuard, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///查詢宣告數量請求
int ReqQryExecOrderVolume( HKSTRD hKStrd, KS_::CKSQryExecOrderVolumeField *pQryExecOrderVolume, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSoptObj->ReqQryExecOrderVolume( pQryExecOrderVolume, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///查詢個股行權指派信息
int ReqQryStockOptionAssignment( HKSTRD hKStrd, KS_::CKSQryStockOptionAssignmentField *pQryStockOptionAssignment, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSoptObj->ReqQryStockOptionAssignment( pQryStockOptionAssignment, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///查詢客戶交易級別
int ReqQryInvestorTradeLevel( HKSTRD hKStrd, KS_::CKSQryInvestorTradeLevelField *pQryInvestorTradeLevel, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSoptObj->ReqQryInvestorTradeLevel( pQryInvestorTradeLevel, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///查詢個股限購額度
int ReqQryPurchaseLimitAmt( HKSTRD hKStrd, KS_::CKSQryPurchaseLimitAmtField *pQryPurchaseLimitAmt, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSoptObj->ReqQryPurchaseLimitAmt( pQryPurchaseLimitAmt, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///查詢個股限倉額度
int ReqQryPositionLimitVol( HKSTRD hKStrd, KS_::CKSQryPositionLimitVolField *pQryPositionLimitVol, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSoptObj->ReqQryPositionLimitVol( pQryPositionLimitVol, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///請求查詢個股歷史報單
int ReqQryHistoryOrder( HKSTRD hKStrd, KS_::CKSQryHistoryOrderField *pQryHistoryOrder, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSoptObj->ReqQryHistoryOrder( pQryHistoryOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///請求查詢個股歷史成交
int ReqQryHistoryTrade( HKSTRD hKStrd, KS_::CKSQryHistoryTradeField *pQryHistoryTrade, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSoptObj->ReqQryHistoryTrade( pQryHistoryTrade, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///請求查詢個股歷史行權指派明細
int ReqQryStockOptionHistoryAssignment( HKSTRD hKStrd, KS_::CKSQryHistoryAssignmentField *pQryHistoryAssignment, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSoptObj->ReqQryStockOptionHistoryAssignment( pQryHistoryAssignment, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///請求查詢個股行權交割明細
int ReqQryStockOptionDelivDetail( HKSTRD hKStrd, KS_::CKSQrySODelivDetailField *pQrySODelivDetail, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSoptObj->ReqQryStockOptionDelivDetail( pQrySODelivDetail, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///自動行權執行操作
int ReqAutoExecOrderAction( HKSTRD hKStrd, KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSoptObj->ReqAutoExecOrderAction( pAutoExecOrderAction, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
// ---- CKSVocApi ----
//------------------------------------------------------------------------------------------------------------------------
// v2.3.50212 // 20150410 by Tim Lin
//------------------------------------------------------------------------------------------------------------------------
///查詢開盤前的持倉明細
int ReqQueryInvestorOpenPosition( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorOpenPosition, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSvocObj->ReqQueryInvestorOpenPosition( pQryInvestorOpenPosition, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///查詢開盤前的組合持倉明細
int ReqQueryInvestorOpenCombinePosition( HKSTRD hKStrd, KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorOpenCombinePosition, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSvocObj->ReqQueryInvestorOpenCombinePosition( pQryInvestorOpenCombinePosition, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///批量撤單
int ReqBulkCancelOrder( HKSTRD hKStrd, KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSvocObj->ReqBulkCancelOrder( pBulkCancelOrder, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///平倉策略查詢請求
int ReqQryCloseStrategy( HKSTRD hKStrd, KS_::CKSCloseStrategy *pCloseStrategy, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSvocObj->ReqQryCloseStrategy( pCloseStrategy, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///組合策略查詢請求
int ReqQryCombStrategy( HKSTRD hKStrd, KS_::CKSCombStrategy *pCombStrategy, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSvocObj->ReqQryCombStrategy( pCombStrategy, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///期權組合策略查詢請求
int ReqQryOptionCombStrategy( HKSTRD hKStrd, KS_::CKSOptionCombStrategy *pOptionCombStrategy, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSvocObj->ReqQryOptionCombStrategy( pOptionCombStrategy, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///請求查詢客戶轉帳資訊
int ReqQryTransferInfo( HKSTRD hKStrd, KS_::CKSTransferInfo *pTransferInfo, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSvocObj->ReqQryTransferInfo( pTransferInfo, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///請求查詢交易通知
int ReqQryKSTradingNotice( HKSTRD hKStrd, KS_::CKSQryTradingNoticeField *pQryTradingNotice, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSvocObj->ReqQryKSTradingNotice( pQryTradingNotice, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///使用者端產品資源查詢請求
int ReqQryUserProductUrl( HKSTRD hKStrd, KS_::CKSQryUserProductUrlField *pQryUserProductUrl, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSvocObj->ReqQryUserProductUrl( pQryUserProductUrl, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///最大組合拆分單量查詢請求
int ReqQryMaxCombActionVolume( HKSTRD hKStrd, KS_::CKSQryMaxCombActionVolumeField *pQryMaxCombActionVolume, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSvocObj->ReqQryMaxCombActionVolume( pQryMaxCombActionVolume, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
///請求查詢合約保證金率
int ReqQryKSInstrumentMarginRate( HKSTRD hKStrd, KS_::CKSQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	return hKSTRD->KSvocObj->ReqQryKSInstrumentMarginRate( pQryInstrumentMarginRate, nRequestID );
}
//------------------------------------------------------------------------------------------------------------------------
//
// Regist callback function CThostFtdcTraderApi, CKSVocApi, CKSOptionApi
//
//------------------------------------------------------------------------------------------------------------------------
// ---- CThostFtdcTraderSpi ----
//------------------------------------------------------------------------------------------------------------------------
void RegOnFrontConnected( HKSTRD hKStrd, pfOnFrontConnected callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnFrontConnected = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnFrontDisconnected( HKSTRD hKStrd, pfOnFrontDisconnected callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnFrontDisconnected = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnHeartBeatWarning( HKSTRD hKStrd, pfOnHeartBeatWarning callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnHeartBeatWarning = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspAuthenticate( HKSTRD hKStrd, pfOnRspAuthenticate callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspAuthenticate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspUserLogin( HKSTRD hKStrd, pfOnRspUserLogin callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspUserLogin = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspUserLogout( HKSTRD hKStrd, pfOnRspUserLogout callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspUserLogout = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspUserPasswordUpdate( HKSTRD hKStrd, pfOnRspUserPasswordUpdate callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspUserPasswordUpdate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspTradingAccountPasswordUpdate( HKSTRD hKStrd, pfOnRspTradingAccountPasswordUpdate callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspTradingAccountPasswordUpdate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspOrderInsert( HKSTRD hKStrd, pfOnRspOrderInsert callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspOrderInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspParkedOrderInsert( HKSTRD hKStrd, pfOnRspParkedOrderInsert callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspParkedOrderInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspParkedOrderAction( HKSTRD hKStrd, pfOnRspParkedOrderAction callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspParkedOrderAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspOrderAction( HKSTRD hKStrd, pfOnRspOrderAction callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspOrderAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQueryMaxOrderVolume( HKSTRD hKStrd, pfOnRspQueryMaxOrderVolume callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQueryMaxOrderVolume = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspSettlementInfoConfirm( HKSTRD hKStrd, pfOnRspSettlementInfoConfirm callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspSettlementInfoConfirm = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspRemoveParkedOrder( HKSTRD hKStrd, pfOnRspRemoveParkedOrder callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspRemoveParkedOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspRemoveParkedOrderAction( HKSTRD hKStrd, pfOnRspRemoveParkedOrderAction callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspRemoveParkedOrderAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspExecOrderInsert( HKSTRD hKStrd, pfOnRspExecOrderInsert callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspExecOrderInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspExecOrderAction( HKSTRD hKStrd, pfOnRspExecOrderAction callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspExecOrderAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspForQuoteInsert( HKSTRD hKStrd, pfOnRspForQuoteInsert callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspForQuoteInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQuoteInsert( HKSTRD hKStrd, pfOnRspQuoteInsert callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQuoteInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQuoteAction( HKSTRD hKStrd, pfOnRspQuoteAction callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQuoteAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryOrder( HKSTRD hKStrd, pfOnRspQryOrder callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryTrade( HKSTRD hKStrd, pfOnRspQryTrade callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryTrade = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInvestorPosition( HKSTRD hKStrd, pfOnRspQryInvestorPosition callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryInvestorPosition = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryTradingAccount( HKSTRD hKStrd, pfOnRspQryTradingAccount callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryTradingAccount = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInvestor( HKSTRD hKStrd, pfOnRspQryInvestor callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryInvestor = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryTradingCode( HKSTRD hKStrd, pfOnRspQryTradingCode callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryTradingCode = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInstrumentMarginRate( HKSTRD hKStrd, pfOnRspQryInstrumentMarginRate callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryInstrumentMarginRate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInstrumentCommissionRate( HKSTRD hKStrd, pfOnRspQryInstrumentCommissionRate callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryInstrumentCommissionRate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryExchange( HKSTRD hKStrd, pfOnRspQryExchange callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryExchange = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryProduct( HKSTRD hKStrd, pfOnRspQryProduct callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryProduct = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInstrument( HKSTRD hKStrd, pfOnRspQryInstrument callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryInstrument = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryDepthMarketData( HKSTRD hKStrd, pfOnRspQryDepthMarketData callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryDepthMarketData = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQrySettlementInfo( HKSTRD hKStrd, pfOnRspQrySettlementInfo callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQrySettlementInfo = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryTransferBank( HKSTRD hKStrd, pfOnRspQryTransferBank callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryTransferBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInvestorPositionDetail( HKSTRD hKStrd, pfOnRspQryInvestorPositionDetail callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryInvestorPositionDetail = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryNotice( HKSTRD hKStrd, pfOnRspQryNotice callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryNotice = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQrySettlementInfoConfirm( HKSTRD hKStrd, pfOnRspQrySettlementInfoConfirm callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQrySettlementInfoConfirm = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInvestorPositionCombineDetail( HKSTRD hKStrd, pfOnRspQryInvestorPositionCombineDetail callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryInvestorPositionCombineDetail = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryCFMMCTradingAccountKey( HKSTRD hKStrd, pfOnRspQryCFMMCTradingAccountKey callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryCFMMCTradingAccountKey = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryEWarrantOffset( HKSTRD hKStrd, pfOnRspQryEWarrantOffset callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryEWarrantOffset = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInvestorProductGroupMargin( HKSTRD hKStrd, pfOnRspQryInvestorProductGroupMargin callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryInvestorProductGroupMargin = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryExchangeMarginRate( HKSTRD hKStrd, pfOnRspQryExchangeMarginRate callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryExchangeMarginRate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryExchangeMarginRateAdjust( HKSTRD hKStrd, pfOnRspQryExchangeMarginRateAdjust callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryExchangeMarginRateAdjust = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryExchangeRate( HKSTRD hKStrd, pfOnRspQryExchangeRate callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryExchangeRate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQrySecAgentACIDMap( HKSTRD hKStrd, pfOnRspQrySecAgentACIDMap callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQrySecAgentACIDMap = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryOptionInstrTradeCost( HKSTRD hKStrd, pfOnRspQryOptionInstrTradeCost callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryOptionInstrTradeCost = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryOptionInstrCommRate( HKSTRD hKStrd, pfOnRspQryOptionInstrCommRate callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryOptionInstrCommRate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryExecOrder( HKSTRD hKStrd, pfOnRspQryExecOrder callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryExecOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryForQuote( HKSTRD hKStrd, pfOnRspQryForQuote callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryForQuote = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryQuote( HKSTRD hKStrd, pfOnRspQryQuote callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryQuote = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryTransferSerial( HKSTRD hKStrd, pfOnRspQryTransferSerial callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryTransferSerial = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryAccountregister( HKSTRD hKStrd, pfOnRspQryAccountregister callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryAccountregister = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspError( HKSTRD hKStrd, pfOnRspError callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspError = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnOrder( HKSTRD hKStrd, pfOnRtnOrder callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnTrade( HKSTRD hKStrd, pfOnRtnTrade callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnTrade = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnOrderInsert( HKSTRD hKStrd, pfOnErrRtnOrderInsert callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnErrRtnOrderInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnOrderAction( HKSTRD hKStrd, pfOnErrRtnOrderAction callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnErrRtnOrderAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnInstrumentStatus( HKSTRD hKStrd, pfOnRtnInstrumentStatus callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnInstrumentStatus = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnTradingNotice( HKSTRD hKStrd, pfOnRtnTradingNotice callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnTradingNotice = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnErrorConditionalOrder( HKSTRD hKStrd, pfOnRtnErrorConditionalOrder callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnErrorConditionalOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnExecOrder( HKSTRD hKStrd, pfOnRtnExecOrder callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnExecOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnExecOrderInsert( HKSTRD hKStrd, pfOnErrRtnExecOrderInsert callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnErrRtnExecOrderInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnExecOrderAction( HKSTRD hKStrd, pfOnErrRtnExecOrderAction callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnErrRtnExecOrderAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnForQuoteInsert( HKSTRD hKStrd, pfOnErrRtnForQuoteInsert callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnErrRtnForQuoteInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnQuote( HKSTRD hKStrd, pfOnRtnQuote callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnQuote = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnQuoteInsert( HKSTRD hKStrd, pfOnErrRtnQuoteInsert callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnErrRtnQuoteInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnQuoteAction( HKSTRD hKStrd, pfOnErrRtnQuoteAction callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnErrRtnQuoteAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnForQuoteRsp( HKSTRD hKStrd, pfOnRtnForQuoteRsp callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnForQuoteRsp = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnCFMMCTradingAccountToken( HKSTRD hKStrd, pfOnRtnCFMMCTradingAccountToken callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnCFMMCTradingAccountToken = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryContractBank( HKSTRD hKStrd, pfOnRspQryContractBank callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryContractBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryParkedOrder( HKSTRD hKStrd, pfOnRspQryParkedOrder callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryParkedOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryParkedOrderAction( HKSTRD hKStrd, pfOnRspQryParkedOrderAction callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryParkedOrderAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryTradingNotice( HKSTRD hKStrd, pfOnRspQryTradingNotice callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryTradingNotice = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryBrokerTradingParams( HKSTRD hKStrd, pfOnRspQryBrokerTradingParams callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryBrokerTradingParams = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryBrokerTradingAlgos( HKSTRD hKStrd, pfOnRspQryBrokerTradingAlgos callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryBrokerTradingAlgos = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQueryCFMMCTradingAccountToken( HKSTRD hKStrd, pfOnRspQueryCFMMCTradingAccountToken callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQueryCFMMCTradingAccountToken = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnFromBankToFutureByBank( HKSTRD hKStrd, pfOnRtnFromBankToFutureByBank callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnFromBankToFutureByBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnFromFutureToBankByBank( HKSTRD hKStrd, pfOnRtnFromFutureToBankByBank callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnFromFutureToBankByBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnRepealFromBankToFutureByBank( HKSTRD hKStrd, pfOnRtnRepealFromBankToFutureByBank callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnRepealFromBankToFutureByBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnRepealFromFutureToBankByBank( HKSTRD hKStrd, pfOnRtnRepealFromFutureToBankByBank callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnRepealFromFutureToBankByBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnFromBankToFutureByFuture( HKSTRD hKStrd, pfOnRtnFromBankToFutureByFuture callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnFromBankToFutureByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnFromFutureToBankByFuture( HKSTRD hKStrd, pfOnRtnFromFutureToBankByFuture callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnFromFutureToBankByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnRepealFromBankToFutureByFutureManual( HKSTRD hKStrd, pfOnRtnRepealFromBankToFutureByFutureManual callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnRepealFromBankToFutureByFutureManual = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnRepealFromFutureToBankByFutureManual( HKSTRD hKStrd, pfOnRtnRepealFromFutureToBankByFutureManual callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnRepealFromFutureToBankByFutureManual = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnQueryBankBalanceByFuture( HKSTRD hKStrd, pfOnRtnQueryBankBalanceByFuture callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnQueryBankBalanceByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnBankToFutureByFuture( HKSTRD hKStrd, pfOnErrRtnBankToFutureByFuture callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnErrRtnBankToFutureByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnFutureToBankByFuture( HKSTRD hKStrd, pfOnErrRtnFutureToBankByFuture callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnErrRtnFutureToBankByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnRepealBankToFutureByFutureManual( HKSTRD hKStrd, pfOnErrRtnRepealBankToFutureByFutureManual callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnErrRtnRepealBankToFutureByFutureManual = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnRepealFutureToBankByFutureManual( HKSTRD hKStrd, pfOnErrRtnRepealFutureToBankByFutureManual callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnErrRtnRepealFutureToBankByFutureManual = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnQueryBankBalanceByFuture( HKSTRD hKStrd, pfOnErrRtnQueryBankBalanceByFuture callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnErrRtnQueryBankBalanceByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnRepealFromBankToFutureByFuture( HKSTRD hKStrd, pfOnRtnRepealFromBankToFutureByFuture callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnRepealFromBankToFutureByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnRepealFromFutureToBankByFuture( HKSTRD hKStrd, pfOnRtnRepealFromFutureToBankByFuture callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnRepealFromFutureToBankByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspFromBankToFutureByFuture( HKSTRD hKStrd, pfOnRspFromBankToFutureByFuture callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspFromBankToFutureByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspFromFutureToBankByFuture( HKSTRD hKStrd, pfOnRspFromFutureToBankByFuture callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspFromFutureToBankByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQueryBankAccountMoneyByFuture( HKSTRD hKStrd, pfOnRspQueryBankAccountMoneyByFuture callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQueryBankAccountMoneyByFuture = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnOpenAccountByBank( HKSTRD hKStrd, pfOnRtnOpenAccountByBank callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnOpenAccountByBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnCancelAccountByBank( HKSTRD hKStrd, pfOnRtnCancelAccountByBank callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnCancelAccountByBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnChangeAccountByBank( HKSTRD hKStrd, pfOnRtnChangeAccountByBank callback )
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnChangeAccountByBank = callback;
}
//------------------------------------------------------------------------------------------------------------------------
// v2.3.50212 // 20150410 by Tim Lin // <Step>.7
void RegOnRspLockInsert( HKSTRD hKStrd, pfOnRspLockInsert callback )///鎖定應答
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspLockInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryLock( HKSTRD hKStrd, pfOnRspQryLock callback )///請求查詢鎖定回應	
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryLock = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryLockPosition( HKSTRD hKStrd, pfOnRspQryLockPosition callback )///請求查詢鎖定證券倉位回應
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryLockPosition = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnLock( HKSTRD hKStrd, pfOnRtnLock callback )///鎖定通知
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnLock = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnErrRtnLockInsert( HKSTRD hKStrd, pfOnErrRtnLockInsert callback )///鎖定錯誤通知	
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnErrRtnLockInsert = callback;
}
//------------------------------------------------------------------------------------------------------------------------
// ---- CKSOptionSpi ----
//------------------------------------------------------------------------------------------------------------------------
// v2.3.50212 // 20150410 by Tim Lin // Step.7
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryIndexPrice( HKSTRD hKStrd, pfOnRspQryIndexPrice callback )///查詢股指現貨指數
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryIndexPrice = callback;	
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryOptionInstrGuard( HKSTRD hKStrd, pfOnRspQryOptionInstrGuard callback )///查詢期權合約保障系數
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryOptionInstrGuard = callback;	
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryExecOrderVolume( HKSTRD hKStrd, pfOnRspQryExecOrderVolume callback )///查詢宣告數量請求
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryExecOrderVolume = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryStockOptionAssignment( HKSTRD hKStrd, pfOnRspQryStockOptionAssignment callback )///查詢個股行權指派信息
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryStockOptionAssignment = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInvestorTradeLevel( HKSTRD hKStrd, pfOnRspQryInvestorTradeLevel callback )///查詢客戶交易級別
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryInvestorTradeLevel = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryPurchaseLimitAmt( HKSTRD hKStrd, pfOnRspQryPurchaseLimitAmt callback )///查詢個股限購額度
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryPurchaseLimitAmt = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryPositionLimitVol( HKSTRD hKStrd, pfOnRspQryPositionLimitVol callback )///查詢個股限倉額度
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryPositionLimitVol = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryHistoryOrder( HKSTRD hKStrd, pfOnRspQryHistoryOrder callback )///請求查詢個股歷史報單
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryHistoryOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryHistoryTrade( HKSTRD hKStrd, pfOnRspQryHistoryTrade callback )///請求查詢個股歷史成交
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryHistoryTrade = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryStockOptionHistoryAssignment( HKSTRD hKStrd, pfOnRspQryStockOptionHistoryAssignment callback )///請求查詢個股歷史行權指派明細
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryHistoryAssignment = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryStockOptionDelivDetail( HKSTRD hKStrd, pfOnRspQryStockOptionDelivDetail callback )///請求查詢個股行權交割明細
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQrySODelivDetail = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspAutoExecOrderAction( HKSTRD hKStrd, pfOnRspAutoExecOrderAction callback )///自動行權執行操作
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspAutoExecOrderAction = callback;
}
//------------------------------------------------------------------------------------------------------------------------
// ---- CKSVocSpi ----
// v2.3.50212 // 20150410 by Tim Lin // [Step].7
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInvestorOpenPosition( HKSTRD hKStrd, pfOnRspQryInvestorOpenPosition callback )///查詢開盤前的持倉明細應答
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryInvestorOpenPosition = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryInvestorOpenCombinePosition( HKSTRD hKStrd, pfOnRspQryInvestorOpenCombinePosition callback )///查詢開盤前的組合持倉明細應答
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryInvestorOpenCombinePosition = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspBulkCancelOrder( HKSTRD hKStrd, pfOnRspBulkCancelOrder callback )///批量報單撤除請求回應
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspBulkCancelOrder = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryCloseStrategy( HKSTRD hKStrd, pfOnRspQryCloseStrategy callback )///平倉策略查詢回應
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryCloseStrategy = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryCombStrategy( HKSTRD hKStrd, pfOnRspQryCombStrategy callback )///組合策略查詢回應
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryCombStrategy = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryOptionCombStrategy( HKSTRD hKStrd, pfOnRspQryOptionCombStrategy callback )///期權組合策略查詢回應
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryOptionCombStrategy = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryTransferInfo( HKSTRD hKStrd, pfOnRspQryTransferInfo callback )///請求查詢客戶轉帳資訊回應
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryTransferInfo = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryKSTradingNotice( HKSTRD hKStrd, pfOnRspQryKSTradingNotice callback )///請求查詢交易通知回應
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryKSTradingNotice = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryUserProductUrl( HKSTRD hKStrd, pfOnRspQryUserProductUrl callback )///使用者端產品資源查詢應答
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryUserProductUrl = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryMaxCombActionVolume ( HKSTRD hKStrd, pfOnRspQryMaxCombActionVolume callback )///最大組合拆分單量查詢請求回應
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryMaxCombActionVolume = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRtnKSTradingNotice ( HKSTRD hKStrd, pfOnRtnKSTradingNotice callback )///交易通知
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRtnKSTradingNotice = callback;
}
//------------------------------------------------------------------------------------------------------------------------
void RegOnRspQryKSInstrumentMarginRate ( HKSTRD hKStrd, pfOnRspQryKSInstrumentMarginRate callback )///請求查詢合約保證金率回應
{
	KStrdHandle* hKSTRD = reinterpret_cast<KStrdHandle*>(hKStrd);
	hKSTRD->OnRspQryKSInstrumentMarginRate = callback;
}
//------------------------------------------------------------------------------------------------------------------------
