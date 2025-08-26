//------------------------------------------------------------------------------------------------------------------------
#include "Stdafx.h"
#include "KStrdAPIComm.h"
//------------------------------------------------------------------------------------------------------------------------
//
// class KStrdAPIHandler
//
//------------------------------------------------------------------------------------------------------------------------
// ---- CThostFtdcTraderSpi ----
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnFrontConnected()
{
	if ( FhKStrd->OnFrontConnected != NULL )
		FhKStrd->OnFrontConnected( FhKStrd->FOwner );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnFrontDisconnected( int nReason )
{
	if ( FhKStrd->OnFrontDisconnected != NULL )
		FhKStrd->OnFrontDisconnected( FhKStrd->FOwner, nReason );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnHeartBeatWarning( int nTimeLapse )
{
	if ( FhKStrd->OnHeartBeatWarning != NULL )
		FhKStrd->OnHeartBeatWarning( FhKStrd->FOwner, nTimeLapse );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspAuthenticate( KS_::CThostFtdcRspAuthenticateField *pRspAuthenticateField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspAuthenticate != NULL )
		FhKStrd->OnRspAuthenticate( FhKStrd->FOwner, pRspAuthenticateField, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspUserLogin( KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspUserLogin != NULL )
		FhKStrd->OnRspUserLogin( FhKStrd->FOwner, pRspUserLogin, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspUserLogout( KS_::CThostFtdcUserLogoutField *pUserLogout, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspUserLogout != NULL )
		FhKStrd->OnRspUserLogout( FhKStrd->FOwner, pUserLogout, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspUserPasswordUpdate( KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspUserPasswordUpdate != NULL )
		FhKStrd->OnRspUserPasswordUpdate( FhKStrd->FOwner, pUserPasswordUpdate, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspTradingAccountPasswordUpdate( KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspTradingAccountPasswordUpdate != NULL )
		FhKStrd->OnRspTradingAccountPasswordUpdate( FhKStrd->FOwner, pTradingAccountPasswordUpdate, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspOrderInsert != NULL )
		FhKStrd->OnRspOrderInsert( FhKStrd->FOwner, pInputOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspParkedOrderInsert( KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspParkedOrderInsert != NULL )
		FhKStrd->OnRspParkedOrderInsert( FhKStrd->FOwner, pParkedOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspParkedOrderAction( KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspParkedOrderAction != NULL )
		FhKStrd->OnRspParkedOrderAction( FhKStrd->FOwner, pParkedOrderAction, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspOrderAction( KS_::CThostFtdcInputOrderActionField *pInputOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspOrderAction != NULL )
		FhKStrd->OnRspOrderAction( FhKStrd->FOwner, pInputOrderAction, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQueryMaxOrderVolume( KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQueryMaxOrderVolume != NULL )
		FhKStrd->OnRspQueryMaxOrderVolume( FhKStrd->FOwner, pQueryMaxOrderVolume, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspSettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspSettlementInfoConfirm != NULL )
		FhKStrd->OnRspSettlementInfoConfirm( FhKStrd->FOwner, pSettlementInfoConfirm, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspRemoveParkedOrder( KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspRemoveParkedOrder != NULL )
		FhKStrd->OnRspRemoveParkedOrder( FhKStrd->FOwner, pRemoveParkedOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspRemoveParkedOrderAction( KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspRemoveParkedOrderAction != NULL )
		FhKStrd->OnRspRemoveParkedOrderAction( FhKStrd->FOwner, pRemoveParkedOrderAction, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspExecOrderInsert( KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspExecOrderInsert != NULL )
		FhKStrd->OnRspExecOrderInsert( FhKStrd->FOwner, pInputExecOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspExecOrderAction( KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspExecOrderAction != NULL )
		FhKStrd->OnRspExecOrderAction( FhKStrd->FOwner, pInputExecOrderAction, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspForQuoteInsert( KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspForQuoteInsert != NULL )
		FhKStrd->OnRspForQuoteInsert( FhKStrd->FOwner, pInputForQuote, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQuoteInsert( KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQuoteInsert != NULL )
		FhKStrd->OnRspQuoteInsert( FhKStrd->FOwner, pInputQuote, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQuoteAction( KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQuoteAction != NULL )
		FhKStrd->OnRspQuoteAction( FhKStrd->FOwner, pInputQuoteAction, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryOrder( KS_::CThostFtdcOrderField *pOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryOrder != NULL )
		FhKStrd->OnRspQryOrder( FhKStrd->FOwner, pOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryTrade( KS_::CThostFtdcTradeField *pTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryTrade != NULL )
		FhKStrd->OnRspQryTrade( FhKStrd->FOwner, pTrade, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryInvestorPosition( KS_::CThostFtdcInvestorPositionField *pInvestorPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryInvestorPosition != NULL )
		FhKStrd->OnRspQryInvestorPosition( FhKStrd->FOwner, pInvestorPosition, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryTradingAccount( KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryTradingAccount != NULL )
		FhKStrd->OnRspQryTradingAccount( FhKStrd->FOwner, pTradingAccount, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryInvestor( KS_::CThostFtdcInvestorField *pInvestor, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryInvestor != NULL )
		FhKStrd->OnRspQryInvestor( FhKStrd->FOwner, pInvestor, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryTradingCode( KS_::CThostFtdcTradingCodeField *pTradingCode, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryTradingCode != NULL )
		FhKStrd->OnRspQryTradingCode( FhKStrd->FOwner, pTradingCode, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryInstrumentMarginRate( KS_::CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryInstrumentMarginRate != NULL )
		FhKStrd->OnRspQryInstrumentMarginRate( FhKStrd->FOwner, pInstrumentMarginRate, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryInstrumentCommissionRate( KS_::CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryInstrumentCommissionRate != NULL )
		FhKStrd->OnRspQryInstrumentCommissionRate( FhKStrd->FOwner, pInstrumentCommissionRate, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryExchange( KS_::CThostFtdcExchangeField *pExchange, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryExchange != NULL )
		FhKStrd->OnRspQryExchange( FhKStrd->FOwner, pExchange, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryProduct( KS_::CThostFtdcProductField *pProduct, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryProduct != NULL )
		FhKStrd->OnRspQryProduct( FhKStrd->FOwner, pProduct, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryInstrument( KS_::CThostFtdcInstrumentField *pInstrument, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryInstrument != NULL )
		FhKStrd->OnRspQryInstrument( FhKStrd->FOwner, pInstrument, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryDepthMarketData( KS_::CThostFtdcDepthMarketDataField *pDepthMarketData, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryDepthMarketData != NULL )
		FhKStrd->OnRspQryDepthMarketData( FhKStrd->FOwner, pDepthMarketData, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQrySettlementInfo( KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQrySettlementInfo != NULL )
		FhKStrd->OnRspQrySettlementInfo( FhKStrd->FOwner, pSettlementInfo, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryTransferBank( KS_::CThostFtdcTransferBankField *pTransferBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryTransferBank != NULL )
		FhKStrd->OnRspQryTransferBank( FhKStrd->FOwner, pTransferBank, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryInvestorPositionDetail( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryInvestorPositionDetail != NULL )
		FhKStrd->OnRspQryInvestorPositionDetail( FhKStrd->FOwner, pInvestorPositionDetail, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryNotice( KS_::CThostFtdcNoticeField *pNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryNotice != NULL )
		FhKStrd->OnRspQryNotice( FhKStrd->FOwner, pNotice, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQrySettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQrySettlementInfoConfirm != NULL )
		FhKStrd->OnRspQrySettlementInfoConfirm( FhKStrd->FOwner, pSettlementInfoConfirm, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryInvestorPositionCombineDetail( KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryInvestorPositionCombineDetail != NULL )
		FhKStrd->OnRspQryInvestorPositionCombineDetail( FhKStrd->FOwner, pInvestorPositionCombineDetail, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryCFMMCTradingAccountKey( KS_::CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryCFMMCTradingAccountKey != NULL )
		FhKStrd->OnRspQryCFMMCTradingAccountKey( FhKStrd->FOwner, pCFMMCTradingAccountKey, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryEWarrantOffset( KS_::CThostFtdcEWarrantOffsetField *pEWarrantOffset, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryEWarrantOffset != NULL )
		FhKStrd->OnRspQryEWarrantOffset( FhKStrd->FOwner, pEWarrantOffset, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryInvestorProductGroupMargin( KS_::CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryInvestorProductGroupMargin != NULL )
		FhKStrd->OnRspQryInvestorProductGroupMargin( FhKStrd->FOwner, pInvestorProductGroupMargin, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryExchangeMarginRate( KS_::CThostFtdcExchangeMarginRateField *pExchangeMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryExchangeMarginRate != NULL )
		FhKStrd->OnRspQryExchangeMarginRate( FhKStrd->FOwner, pExchangeMarginRate, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryExchangeMarginRateAdjust( KS_::CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryExchangeMarginRateAdjust != NULL )
		FhKStrd->OnRspQryExchangeMarginRateAdjust( FhKStrd->FOwner, pExchangeMarginRateAdjust, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryExchangeRate( KS_::CThostFtdcExchangeRateField *pExchangeRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryExchangeRate != NULL )
		FhKStrd->OnRspQryExchangeRate( FhKStrd->FOwner, pExchangeRate, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQrySecAgentACIDMap( KS_::CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQrySecAgentACIDMap != NULL )
		FhKStrd->OnRspQrySecAgentACIDMap( FhKStrd->FOwner, pSecAgentACIDMap, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryOptionInstrTradeCost( KS_::CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryOptionInstrTradeCost != NULL )
		FhKStrd->OnRspQryOptionInstrTradeCost( FhKStrd->FOwner, pOptionInstrTradeCost, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryOptionInstrCommRate( KS_::CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryOptionInstrCommRate != NULL )
		FhKStrd->OnRspQryOptionInstrCommRate( FhKStrd->FOwner, pOptionInstrCommRate, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryExecOrder( KS_::CThostFtdcExecOrderField *pExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryExecOrder != NULL )
		FhKStrd->OnRspQryExecOrder( FhKStrd->FOwner, pExecOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryForQuote( KS_::CThostFtdcForQuoteField *pForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryForQuote != NULL )
		FhKStrd->OnRspQryForQuote( FhKStrd->FOwner, pForQuote, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryQuote( KS_::CThostFtdcQuoteField *pQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryQuote != NULL )
		FhKStrd->OnRspQryQuote( FhKStrd->FOwner, pQuote, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryTransferSerial( KS_::CThostFtdcTransferSerialField *pTransferSerial, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryTransferSerial != NULL )
		FhKStrd->OnRspQryTransferSerial( FhKStrd->FOwner, pTransferSerial, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryAccountregister( KS_::CThostFtdcAccountregisterField *pAccountregister, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryAccountregister != NULL )
		FhKStrd->OnRspQryAccountregister( FhKStrd->FOwner, pAccountregister, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspError( KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspError != NULL )
		FhKStrd->OnRspError( FhKStrd->FOwner, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnOrder( KS_::CThostFtdcOrderField *pOrder )
{
	if ( FhKStrd->OnRtnOrder != NULL )
		FhKStrd->OnRtnOrder( FhKStrd->FOwner, pOrder );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnTrade( KS_::CThostFtdcTradeField *pTrade )
{
	if ( FhKStrd->OnRtnTrade != NULL )
		FhKStrd->OnRtnTrade( FhKStrd->FOwner, pTrade );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnErrRtnOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKStrd->OnErrRtnOrderInsert != NULL )
		FhKStrd->OnErrRtnOrderInsert( FhKStrd->FOwner, pInputOrder, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnErrRtnOrderAction( KS_::CThostFtdcOrderActionField *pOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKStrd->OnErrRtnOrderAction != NULL )
		FhKStrd->OnErrRtnOrderAction( FhKStrd->FOwner, pOrderAction, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnInstrumentStatus( KS_::CThostFtdcInstrumentStatusField *pInstrumentStatus )
{
	if ( FhKStrd->OnRtnInstrumentStatus != NULL )
		FhKStrd->OnRtnInstrumentStatus( FhKStrd->FOwner, pInstrumentStatus );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnTradingNotice( KS_::CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo )
{
	if ( FhKStrd->OnRtnTradingNotice != NULL )
		FhKStrd->OnRtnTradingNotice( FhKStrd->FOwner, pTradingNoticeInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnErrorConditionalOrder( KS_::CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder )
{
	if ( FhKStrd->OnRtnErrorConditionalOrder != NULL )
		FhKStrd->OnRtnErrorConditionalOrder( FhKStrd->FOwner, pErrorConditionalOrder );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnExecOrder( KS_::CThostFtdcExecOrderField *pExecOrder )
{
	if ( FhKStrd->OnRtnExecOrder != NULL )
		FhKStrd->OnRtnExecOrder( FhKStrd->FOwner, pExecOrder );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnErrRtnExecOrderInsert( KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKStrd->OnErrRtnExecOrderInsert != NULL )
		FhKStrd->OnErrRtnExecOrderInsert( FhKStrd->FOwner, pInputExecOrder, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnErrRtnExecOrderAction( KS_::CThostFtdcExecOrderActionField *pExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKStrd->OnErrRtnExecOrderAction != NULL )
		FhKStrd->OnErrRtnExecOrderAction( FhKStrd->FOwner, pExecOrderAction, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnErrRtnForQuoteInsert( KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKStrd->OnErrRtnForQuoteInsert != NULL )
		FhKStrd->OnErrRtnForQuoteInsert( FhKStrd->FOwner, pInputForQuote, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnQuote( KS_::CThostFtdcQuoteField *pQuote )
{
	if ( FhKStrd->OnRtnQuote != NULL )
		FhKStrd->OnRtnQuote( FhKStrd->FOwner, pQuote );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnErrRtnQuoteInsert( KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKStrd->OnErrRtnQuoteInsert != NULL )
		FhKStrd->OnErrRtnQuoteInsert( FhKStrd->FOwner, pInputQuote, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnErrRtnQuoteAction( KS_::CThostFtdcQuoteActionField *pQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKStrd->OnErrRtnQuoteAction != NULL )
		FhKStrd->OnErrRtnQuoteAction( FhKStrd->FOwner, pQuoteAction, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnForQuoteRsp( KS_::CThostFtdcForQuoteRspField *pForQuoteRsp )
{
	if ( FhKStrd->OnRtnForQuoteRsp != NULL )
		FhKStrd->OnRtnForQuoteRsp( FhKStrd->FOwner, pForQuoteRsp );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnCFMMCTradingAccountToken( KS_::CThostFtdcCFMMCTradingAccountTokenField *pCFMMCTradingAccountToken )
{
	if ( FhKStrd->OnRtnCFMMCTradingAccountToken != NULL )
		FhKStrd->OnRtnCFMMCTradingAccountToken( FhKStrd->FOwner, pCFMMCTradingAccountToken );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryContractBank( KS_::CThostFtdcContractBankField *pContractBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryContractBank != NULL )
		FhKStrd->OnRspQryContractBank( FhKStrd->FOwner, pContractBank, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryParkedOrder( KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryParkedOrder != NULL )
		FhKStrd->OnRspQryParkedOrder( FhKStrd->FOwner, pParkedOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryParkedOrderAction( KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryParkedOrderAction != NULL )
		FhKStrd->OnRspQryParkedOrderAction( FhKStrd->FOwner, pParkedOrderAction, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryTradingNotice( KS_::CThostFtdcTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryTradingNotice != NULL )
		FhKStrd->OnRspQryTradingNotice( FhKStrd->FOwner, pTradingNotice, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryBrokerTradingParams( KS_::CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryBrokerTradingParams != NULL )
		FhKStrd->OnRspQryBrokerTradingParams( FhKStrd->FOwner, pBrokerTradingParams, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryBrokerTradingAlgos( KS_::CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryBrokerTradingAlgos != NULL )
		FhKStrd->OnRspQryBrokerTradingAlgos( FhKStrd->FOwner, pBrokerTradingAlgos, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQueryCFMMCTradingAccountToken( KS_::CThostFtdcQueryCFMMCTradingAccountTokenField *pQueryCFMMCTradingAccountToken, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQueryCFMMCTradingAccountToken != NULL )
		FhKStrd->OnRspQueryCFMMCTradingAccountToken( FhKStrd->FOwner, pQueryCFMMCTradingAccountToken, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnFromBankToFutureByBank( KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	if ( FhKStrd->OnRtnFromBankToFutureByBank != NULL )
		FhKStrd->OnRtnFromBankToFutureByBank( FhKStrd->FOwner, pRspTransfer );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnFromFutureToBankByBank( KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	if ( FhKStrd->OnRtnFromFutureToBankByBank != NULL )
		FhKStrd->OnRtnFromFutureToBankByBank( FhKStrd->FOwner, pRspTransfer );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnRepealFromBankToFutureByBank( KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	if ( FhKStrd->OnRtnRepealFromBankToFutureByBank != NULL )
		FhKStrd->OnRtnRepealFromBankToFutureByBank( FhKStrd->FOwner, pRspRepeal );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnRepealFromFutureToBankByBank( KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	if ( FhKStrd->OnRtnRepealFromFutureToBankByBank != NULL )
		FhKStrd->OnRtnRepealFromFutureToBankByBank( FhKStrd->FOwner, pRspRepeal );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnFromBankToFutureByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	if ( FhKStrd->OnRtnFromBankToFutureByFuture != NULL )
		FhKStrd->OnRtnFromBankToFutureByFuture( FhKStrd->FOwner, pRspTransfer );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnFromFutureToBankByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	if ( FhKStrd->OnRtnFromFutureToBankByFuture != NULL )
		FhKStrd->OnRtnFromFutureToBankByFuture( FhKStrd->FOwner, pRspTransfer );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnRepealFromBankToFutureByFutureManual( KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	if ( FhKStrd->OnRtnRepealFromBankToFutureByFutureManual != NULL )
		FhKStrd->OnRtnRepealFromBankToFutureByFutureManual( FhKStrd->FOwner, pRspRepeal );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnRepealFromFutureToBankByFutureManual( KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	if ( FhKStrd->OnRtnRepealFromFutureToBankByFutureManual != NULL )
		FhKStrd->OnRtnRepealFromFutureToBankByFutureManual( FhKStrd->FOwner, pRspRepeal );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnQueryBankBalanceByFuture( KS_::CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount )
{
	if ( FhKStrd->OnRtnQueryBankBalanceByFuture != NULL )
		FhKStrd->OnRtnQueryBankBalanceByFuture( FhKStrd->FOwner, pNotifyQueryAccount );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnErrRtnBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKStrd->OnErrRtnBankToFutureByFuture != NULL )
		FhKStrd->OnErrRtnBankToFutureByFuture( FhKStrd->FOwner, pReqTransfer, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnErrRtnFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKStrd->OnErrRtnFutureToBankByFuture != NULL )
		FhKStrd->OnErrRtnFutureToBankByFuture( FhKStrd->FOwner, pReqTransfer, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnErrRtnRepealBankToFutureByFutureManual( KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKStrd->OnErrRtnRepealBankToFutureByFutureManual != NULL )
		FhKStrd->OnErrRtnRepealBankToFutureByFutureManual( FhKStrd->FOwner, pReqRepeal, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnErrRtnRepealFutureToBankByFutureManual( KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKStrd->OnErrRtnRepealFutureToBankByFutureManual != NULL )
		FhKStrd->OnErrRtnRepealFutureToBankByFutureManual( FhKStrd->FOwner, pReqRepeal, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnErrRtnQueryBankBalanceByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKStrd->OnErrRtnQueryBankBalanceByFuture != NULL )
		FhKStrd->OnErrRtnQueryBankBalanceByFuture( FhKStrd->FOwner, pReqQueryAccount, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnRepealFromBankToFutureByFuture( KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	if ( FhKStrd->OnRtnRepealFromBankToFutureByFuture != NULL )
		FhKStrd->OnRtnRepealFromBankToFutureByFuture( FhKStrd->FOwner, pRspRepeal );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnRepealFromFutureToBankByFuture( KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	if ( FhKStrd->OnRtnRepealFromFutureToBankByFuture != NULL )
		FhKStrd->OnRtnRepealFromFutureToBankByFuture( FhKStrd->FOwner, pRspRepeal );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspFromBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspFromBankToFutureByFuture != NULL )
		FhKStrd->OnRspFromBankToFutureByFuture( FhKStrd->FOwner, pReqTransfer, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspFromFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspFromFutureToBankByFuture != NULL )
		FhKStrd->OnRspFromFutureToBankByFuture( FhKStrd->FOwner, pReqTransfer, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQueryBankAccountMoneyByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQueryBankAccountMoneyByFuture != NULL )
		FhKStrd->OnRspQueryBankAccountMoneyByFuture( FhKStrd->FOwner, pReqQueryAccount, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnOpenAccountByBank( KS_::CThostFtdcOpenAccountField *pOpenAccount )
{
	if ( FhKStrd->OnRtnOpenAccountByBank != NULL )
		FhKStrd->OnRtnOpenAccountByBank( FhKStrd->FOwner, pOpenAccount );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnCancelAccountByBank( KS_::CThostFtdcCancelAccountField *pCancelAccount )
{
	if ( FhKStrd->OnRtnCancelAccountByBank != NULL )
		FhKStrd->OnRtnCancelAccountByBank( FhKStrd->FOwner, pCancelAccount );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnChangeAccountByBank( KS_::CThostFtdcChangeAccountField *pChangeAccount )
{
	if ( FhKStrd->OnRtnChangeAccountByBank != NULL )
		FhKStrd->OnRtnChangeAccountByBank( FhKStrd->FOwner, pChangeAccount );
}
//------------------------------------------------------------------------------------------------------------------------
// ---- CKSOptionSpi ----
// v2.3.50212 // 20150410 by Tim Lin // Step.8
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryIndexPrice ( KS_::CKSQryIndexPriceField *pQryIndexPrice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///查詢股指現貨指數
{
	if ( FhKStrd->OnRspQryIndexPrice != NULL )
		FhKStrd->OnRspQryIndexPrice( FhKStrd->FOwner, pQryIndexPrice, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryOptionInstrGuard ( KS_::CKSQryOptionInstrGuardField *pQryOptionInstrGuard, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///查詢期權合約保障系數
{
	if ( FhKStrd->OnRspQryOptionInstrGuard != NULL )
		FhKStrd->OnRspQryOptionInstrGuard( FhKStrd->FOwner, pQryOptionInstrGuard, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryExecOrderVolume ( KS_::CKSQryExecOrderVolumeField *pQryExecOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )//查詢宣告數量請求
{
	if ( FhKStrd->OnRspQryExecOrderVolume != NULL )
		FhKStrd->OnRspQryExecOrderVolume( FhKStrd->FOwner, pQryExecOrderVolume, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryStockOptionAssignment ( KS_::CKSQryStockOptionAssignmentField *pQryStockOptionAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///查詢個股行權指派信息
{
	if ( FhKStrd->OnRspQryStockOptionAssignment != NULL )
		FhKStrd->OnRspQryStockOptionAssignment( FhKStrd->FOwner, pQryStockOptionAssignment, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryInvestorTradeLevel ( KS_::CKSQryInvestorTradeLevelField *pQryInvestorTradeLevel, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///查詢客戶交易級別
{
	if ( FhKStrd->OnRspQryInvestorTradeLevel != NULL )
		FhKStrd->OnRspQryInvestorTradeLevel( FhKStrd->FOwner, pQryInvestorTradeLevel, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryPurchaseLimitAmt ( KS_::CKSQryPurchaseLimitAmtField *pQryPurchaseLimitAmt, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///查詢個股限購額度
{
	if ( FhKStrd->OnRspQryPurchaseLimitAmt != NULL )
		FhKStrd->OnRspQryPurchaseLimitAmt( FhKStrd->FOwner, pQryPurchaseLimitAmt, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryPositionLimitVol ( KS_::CKSQryPositionLimitVolField *pQryPositionLimitVol, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///查詢個股限倉額度
{
	if ( FhKStrd->OnRspQryPositionLimitVol != NULL )
		FhKStrd->OnRspQryPositionLimitVol( FhKStrd->FOwner, pQryPositionLimitVol, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryHistoryOrder ( KS_::CKSQryHistoryOrderField *pQryHistoryOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///請求查詢個股歷史報單
{
	if ( FhKStrd->OnRspQryHistoryOrder != NULL )
		FhKStrd->OnRspQryHistoryOrder( FhKStrd->FOwner, pQryHistoryOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryHistoryTrade ( KS_::CKSQryHistoryTradeField *pQryHistoryTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///請求查詢個股歷史成交
{
	if ( FhKStrd->OnRspQryHistoryTrade != NULL )
		FhKStrd->OnRspQryHistoryTrade( FhKStrd->FOwner, pQryHistoryTrade, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryStockOptionHistoryAssignment ( KS_::CKSQryHistoryAssignmentField *pQryHistoryAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///請求查詢個股歷史行權指派明細
{
	if ( FhKStrd->OnRspQryHistoryAssignment != NULL )
		FhKStrd->OnRspQryHistoryAssignment( FhKStrd->FOwner, pQryHistoryAssignment, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryStockOptionDelivDetail ( KS_::CKSQrySODelivDetailField *pQrySODelivDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///請求查詢個股行權交割明細
{
	if ( FhKStrd->OnRspQrySODelivDetail != NULL )
		FhKStrd->OnRspQrySODelivDetail( FhKStrd->FOwner, pQrySODelivDetail, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspAutoExecOrderAction ( KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )///自動行權執行操作
{
	if ( FhKStrd->OnRspAutoExecOrderAction != NULL )
		FhKStrd->OnRspAutoExecOrderAction( FhKStrd->FOwner, pAutoExecOrderAction, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspLockInsert( KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)///鎖定應答
{
	if ( FhKStrd->OnRspLockInsert != NULL )
		FhKStrd->OnRspLockInsert( FhKStrd->FOwner, pInputLock, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryLock( KS_::CThostFtdcLockField *pLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)///請求查詢鎖定回應	
{
	if ( FhKStrd->OnRspQryLock != NULL )
		FhKStrd->OnRspQryLock( FhKStrd->FOwner, pLock, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryLockPosition( KS_::CThostFtdcLockPositionField *pLockPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)///請求查詢鎖定證券倉位回應
{
	if ( FhKStrd->OnRspQryLockPosition != NULL )
		FhKStrd->OnRspQryLockPosition( FhKStrd->FOwner, pLockPosition, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnLock( KS_::CThostFtdcLockField *pLock)///鎖定通知
{
	if ( FhKStrd->OnRtnLock != NULL )
		FhKStrd->OnRtnLock( FhKStrd->FOwner, pLock );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnErrRtnLockInsert( KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo)///鎖定錯誤通知	
{
	if ( FhKStrd->OnErrRtnLockInsert != NULL )
		FhKStrd->OnErrRtnLockInsert( FhKStrd->FOwner, pInputLock, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
// ---- CKSVocSpi ----
// v2.3.50212 // 20150410 by Tim Lin // [Step].8
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryInvestorOpenPosition( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryInvestorOpenPosition != NULL )
		FhKStrd->OnRspQryInvestorOpenPosition( FhKStrd->FOwner, pInvestorPositionDetail, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryInvestorOpenCombinePosition( KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryInvestorOpenCombinePosition != NULL )
		FhKStrd->OnRspQryInvestorOpenCombinePosition( FhKStrd->FOwner, pInvestorPositionCombineDetail, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspBulkCancelOrder( KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspBulkCancelOrder != NULL )
		FhKStrd->OnRspBulkCancelOrder( FhKStrd->FOwner, pBulkCancelOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryCloseStrategy( KS_::CKSCloseStrategyResultField *pCloseStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryCloseStrategy != NULL )
		FhKStrd->OnRspQryCloseStrategy( FhKStrd->FOwner, pCloseStrategy, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryCombStrategy( KS_::CKSCombStrategyResultField *pCombStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryCombStrategy != NULL )
		FhKStrd->OnRspQryCombStrategy( FhKStrd->FOwner, pCombStrategy, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryOptionCombStrategy( KS_::CKSOptionCombStrategyResultField *pOptionCombStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryOptionCombStrategy != NULL )
		FhKStrd->OnRspQryOptionCombStrategy( FhKStrd->FOwner, pOptionCombStrategy, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryTransferInfo( KS_::CKSTransferInfoResultField *pResultField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryTransferInfo != NULL )
		FhKStrd->OnRspQryTransferInfo( FhKStrd->FOwner, pResultField, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryKSTradingNotice( KS_::CKSTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryKSTradingNotice != NULL )
		FhKStrd->OnRspQryKSTradingNotice( FhKStrd->FOwner, pTradingNotice, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryUserProductUrl( KS_::CKSUserProductUrlField *pUserProductUrl, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryUserProductUrl != NULL )
		FhKStrd->OnRspQryUserProductUrl( FhKStrd->FOwner, pUserProductUrl, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryMaxCombActionVolume( KS_::CKSMaxCombActionVolumeField *pMaxCombActionVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryMaxCombActionVolume != NULL )
		FhKStrd->OnRspQryMaxCombActionVolume( FhKStrd->FOwner, pMaxCombActionVolume, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRtnKSTradingNotice ( KS_::CKSTradingNoticeField *pTradingNoticeInfo )
{
	if ( FhKStrd->OnRtnKSTradingNotice != NULL )
		FhKStrd->OnRtnKSTradingNotice( FhKStrd->FOwner, pTradingNoticeInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KStrdAPIHandler::OnRspQryKSInstrumentMarginRate( KS_::CKSInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKStrd->OnRspQryKSInstrumentMarginRate != NULL )
		FhKStrd->OnRspQryKSInstrumentMarginRate( FhKStrd->FOwner, pInstrumentMarginRate, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
