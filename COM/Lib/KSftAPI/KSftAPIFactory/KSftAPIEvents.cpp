//------------------------------------------------------------------------------------------------------------------------
#include "Stdafx.h"
#include "KSftAPIComm.h"
//------------------------------------------------------------------------------------------------------------------------
// class KSftAPIHandler
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnFrontConnected()
{
	if ( FhKSft->OnFrontConnected != NULL )
		FhKSft->OnFrontConnected( FhKSft->FOwner );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnFrontDisconnected( int nReason )
{
	if ( FhKSft->OnFrontDisconnected != NULL )
		FhKSft->OnFrontDisconnected( FhKSft->FOwner, nReason );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnHeartBeatWarning( int nTimeLapse )
{
	if ( FhKSft->OnHeartBeatWarning != NULL )
		FhKSft->OnHeartBeatWarning( FhKSft->FOwner, nTimeLapse );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspAuthenticate( KS_::CThostFtdcRspAuthenticateField *pRspAuthenticateField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspAuthenticate != NULL )
		FhKSft->OnRspAuthenticate( FhKSft->FOwner, pRspAuthenticateField, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspUserLogin( KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspUserLogin != NULL )
		FhKSft->OnRspUserLogin( FhKSft->FOwner, pRspUserLogin, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspUserLogout( KS_::CThostFtdcUserLogoutField *pUserLogout, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspUserLogout != NULL )
		FhKSft->OnRspUserLogout( FhKSft->FOwner, pUserLogout, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspUserPasswordUpdate( KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspUserPasswordUpdate != NULL )
		FhKSft->OnRspUserPasswordUpdate( FhKSft->FOwner, pUserPasswordUpdate, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspTradingAccountPasswordUpdate( KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspTradingAccountPasswordUpdate != NULL )
		FhKSft->OnRspTradingAccountPasswordUpdate( FhKSft->FOwner, pTradingAccountPasswordUpdate, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspOrderInsert != NULL )
		FhKSft->OnRspOrderInsert( FhKSft->FOwner, pInputOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspParkedOrderInsert( KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspParkedOrderInsert != NULL )
		FhKSft->OnRspParkedOrderInsert( FhKSft->FOwner, pParkedOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspParkedOrderAction( KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspParkedOrderAction != NULL )
		FhKSft->OnRspParkedOrderAction( FhKSft->FOwner, pParkedOrderAction, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspOrderAction( KS_::CThostFtdcInputOrderActionField *pInputOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspOrderAction != NULL )
		FhKSft->OnRspOrderAction( FhKSft->FOwner, pInputOrderAction, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQueryMaxOrderVolume( KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQueryMaxOrderVolume != NULL )
		FhKSft->OnRspQueryMaxOrderVolume( FhKSft->FOwner, pQueryMaxOrderVolume, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspSettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspSettlementInfoConfirm != NULL )
		FhKSft->OnRspSettlementInfoConfirm( FhKSft->FOwner, pSettlementInfoConfirm, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspRemoveParkedOrder( KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspRemoveParkedOrder != NULL )
		FhKSft->OnRspRemoveParkedOrder( FhKSft->FOwner, pRemoveParkedOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspRemoveParkedOrderAction( KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspRemoveParkedOrderAction != NULL )
		FhKSft->OnRspRemoveParkedOrderAction( FhKSft->FOwner, pRemoveParkedOrderAction, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspExecOrderInsert( KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspExecOrderInsert != NULL )
		FhKSft->OnRspExecOrderInsert( FhKSft->FOwner, pInputExecOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspExecOrderAction( KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspExecOrderAction != NULL )
		FhKSft->OnRspExecOrderAction( FhKSft->FOwner, pInputExecOrderAction, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspForQuoteInsert( KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspForQuoteInsert != NULL )
		FhKSft->OnRspForQuoteInsert( FhKSft->FOwner, pInputForQuote, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQuoteInsert( KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQuoteInsert != NULL )
		FhKSft->OnRspQuoteInsert( FhKSft->FOwner, pInputQuote, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQuoteAction( KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQuoteAction != NULL )
		FhKSft->OnRspQuoteAction( FhKSft->FOwner, pInputQuoteAction, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryOrder( KS_::CThostFtdcOrderField *pOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryOrder != NULL )
		FhKSft->OnRspQryOrder( FhKSft->FOwner, pOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryTrade( KS_::CThostFtdcTradeField *pTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryTrade != NULL )
		FhKSft->OnRspQryTrade( FhKSft->FOwner, pTrade, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryInvestorPosition( KS_::CThostFtdcInvestorPositionField *pInvestorPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryInvestorPosition != NULL )
		FhKSft->OnRspQryInvestorPosition( FhKSft->FOwner, pInvestorPosition, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryTradingAccount( KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryTradingAccount != NULL )
		FhKSft->OnRspQryTradingAccount( FhKSft->FOwner, pTradingAccount, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryInvestor( KS_::CThostFtdcInvestorField *pInvestor, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryInvestor != NULL )
		FhKSft->OnRspQryInvestor( FhKSft->FOwner, pInvestor, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryTradingCode( KS_::CThostFtdcTradingCodeField *pTradingCode, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryTradingCode != NULL )
		FhKSft->OnRspQryTradingCode( FhKSft->FOwner, pTradingCode, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryInstrumentMarginRate( KS_::CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryInstrumentMarginRate != NULL )
		FhKSft->OnRspQryInstrumentMarginRate( FhKSft->FOwner, pInstrumentMarginRate, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryInstrumentCommissionRate( KS_::CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryInstrumentCommissionRate != NULL )
		FhKSft->OnRspQryInstrumentCommissionRate( FhKSft->FOwner, pInstrumentCommissionRate, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryExchange( KS_::CThostFtdcExchangeField *pExchange, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryExchange != NULL )
		FhKSft->OnRspQryExchange( FhKSft->FOwner, pExchange, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryProduct( KS_::CThostFtdcProductField *pProduct, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryProduct != NULL )
		FhKSft->OnRspQryProduct( FhKSft->FOwner, pProduct, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryInstrument( KS_::CThostFtdcInstrumentField *pInstrument, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryInstrument != NULL )
		FhKSft->OnRspQryInstrument( FhKSft->FOwner, pInstrument, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryDepthMarketData( KS_::CThostFtdcDepthMarketDataField *pDepthMarketData, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryDepthMarketData != NULL )
		FhKSft->OnRspQryDepthMarketData( FhKSft->FOwner, pDepthMarketData, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQrySettlementInfo( KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQrySettlementInfo != NULL )
		FhKSft->OnRspQrySettlementInfo( FhKSft->FOwner, pSettlementInfo, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryTransferBank( KS_::CThostFtdcTransferBankField *pTransferBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryTransferBank != NULL )
		FhKSft->OnRspQryTransferBank( FhKSft->FOwner, pTransferBank, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryInvestorPositionDetail( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryInvestorPositionDetail != NULL )
		FhKSft->OnRspQryInvestorPositionDetail( FhKSft->FOwner, pInvestorPositionDetail, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryNotice( KS_::CThostFtdcNoticeField *pNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryNotice != NULL )
		FhKSft->OnRspQryNotice( FhKSft->FOwner, pNotice, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQrySettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQrySettlementInfoConfirm != NULL )
		FhKSft->OnRspQrySettlementInfoConfirm( FhKSft->FOwner, pSettlementInfoConfirm, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryInvestorPositionCombineDetail( KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryInvestorPositionCombineDetail != NULL )
		FhKSft->OnRspQryInvestorPositionCombineDetail( FhKSft->FOwner, pInvestorPositionCombineDetail, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryCFMMCTradingAccountKey( KS_::CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryCFMMCTradingAccountKey != NULL )
		FhKSft->OnRspQryCFMMCTradingAccountKey( FhKSft->FOwner, pCFMMCTradingAccountKey, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryEWarrantOffset( KS_::CThostFtdcEWarrantOffsetField *pEWarrantOffset, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryEWarrantOffset != NULL )
		FhKSft->OnRspQryEWarrantOffset( FhKSft->FOwner, pEWarrantOffset, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryInvestorProductGroupMargin( KS_::CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryInvestorProductGroupMargin != NULL )
		FhKSft->OnRspQryInvestorProductGroupMargin( FhKSft->FOwner, pInvestorProductGroupMargin, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryExchangeMarginRate( KS_::CThostFtdcExchangeMarginRateField *pExchangeMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryExchangeMarginRate != NULL )
		FhKSft->OnRspQryExchangeMarginRate( FhKSft->FOwner, pExchangeMarginRate, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryExchangeMarginRateAdjust( KS_::CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryExchangeMarginRateAdjust != NULL )
		FhKSft->OnRspQryExchangeMarginRateAdjust( FhKSft->FOwner, pExchangeMarginRateAdjust, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryExchangeRate( KS_::CThostFtdcExchangeRateField *pExchangeRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryExchangeRate != NULL )
		FhKSft->OnRspQryExchangeRate( FhKSft->FOwner, pExchangeRate, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQrySecAgentACIDMap( KS_::CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQrySecAgentACIDMap != NULL )
		FhKSft->OnRspQrySecAgentACIDMap( FhKSft->FOwner, pSecAgentACIDMap, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryOptionInstrTradeCost( KS_::CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryOptionInstrTradeCost != NULL )
		FhKSft->OnRspQryOptionInstrTradeCost( FhKSft->FOwner, pOptionInstrTradeCost, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryOptionInstrCommRate( KS_::CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryOptionInstrCommRate != NULL )
		FhKSft->OnRspQryOptionInstrCommRate( FhKSft->FOwner, pOptionInstrCommRate, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryExecOrder( KS_::CThostFtdcExecOrderField *pExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryExecOrder != NULL )
		FhKSft->OnRspQryExecOrder( FhKSft->FOwner, pExecOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryForQuote( KS_::CThostFtdcForQuoteField *pForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryForQuote != NULL )
		FhKSft->OnRspQryForQuote( FhKSft->FOwner, pForQuote, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryQuote( KS_::CThostFtdcQuoteField *pQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryQuote != NULL )
		FhKSft->OnRspQryQuote( FhKSft->FOwner, pQuote, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryTransferSerial( KS_::CThostFtdcTransferSerialField *pTransferSerial, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryTransferSerial != NULL )
		FhKSft->OnRspQryTransferSerial( FhKSft->FOwner, pTransferSerial, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryAccountregister( KS_::CThostFtdcAccountregisterField *pAccountregister, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryAccountregister != NULL )
		FhKSft->OnRspQryAccountregister( FhKSft->FOwner, pAccountregister, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspError( KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspError != NULL )
		FhKSft->OnRspError( FhKSft->FOwner, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnOrder( KS_::CThostFtdcOrderField *pOrder )
{
	if ( FhKSft->OnRtnOrder != NULL )
		FhKSft->OnRtnOrder( FhKSft->FOwner, pOrder );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnTrade( KS_::CThostFtdcTradeField *pTrade )
{
	if ( FhKSft->OnRtnTrade != NULL )
		FhKSft->OnRtnTrade( FhKSft->FOwner, pTrade );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnErrRtnOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKSft->OnErrRtnOrderInsert != NULL )
		FhKSft->OnErrRtnOrderInsert( FhKSft->FOwner, pInputOrder, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnErrRtnOrderAction( KS_::CThostFtdcOrderActionField *pOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKSft->OnErrRtnOrderAction != NULL )
		FhKSft->OnErrRtnOrderAction( FhKSft->FOwner, pOrderAction, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnInstrumentStatus( KS_::CThostFtdcInstrumentStatusField *pInstrumentStatus )
{
	if ( FhKSft->OnRtnInstrumentStatus != NULL )
		FhKSft->OnRtnInstrumentStatus( FhKSft->FOwner, pInstrumentStatus );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnTradingNotice( KS_::CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo )
{
	if ( FhKSft->OnRtnTradingNotice != NULL )
		FhKSft->OnRtnTradingNotice( FhKSft->FOwner, pTradingNoticeInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnErrorConditionalOrder( KS_::CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder )
{
	if ( FhKSft->OnRtnErrorConditionalOrder != NULL )
		FhKSft->OnRtnErrorConditionalOrder( FhKSft->FOwner, pErrorConditionalOrder );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnExecOrder( KS_::CThostFtdcExecOrderField *pExecOrder )
{
	if ( FhKSft->OnRtnExecOrder != NULL )
		FhKSft->OnRtnExecOrder( FhKSft->FOwner, pExecOrder );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnErrRtnExecOrderInsert( KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKSft->OnErrRtnExecOrderInsert != NULL )
		FhKSft->OnErrRtnExecOrderInsert( FhKSft->FOwner, pInputExecOrder, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnErrRtnExecOrderAction( KS_::CThostFtdcExecOrderActionField *pExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKSft->OnErrRtnExecOrderAction != NULL )
		FhKSft->OnErrRtnExecOrderAction( FhKSft->FOwner, pExecOrderAction, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnErrRtnForQuoteInsert( KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKSft->OnErrRtnForQuoteInsert != NULL )
		FhKSft->OnErrRtnForQuoteInsert( FhKSft->FOwner, pInputExecOrder, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnQuote( KS_::CThostFtdcQuoteField *pQuote )
{
	if ( FhKSft->OnRtnQuote != NULL )
		FhKSft->OnRtnQuote( FhKSft->FOwner, pQuote );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnErrRtnQuoteInsert( KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKSft->OnErrRtnQuoteInsert != NULL )
		FhKSft->OnErrRtnQuoteInsert( FhKSft->FOwner, pInputQuote, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnErrRtnQuoteAction( KS_::CThostFtdcQuoteActionField *pQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKSft->OnErrRtnQuoteAction != NULL )
		FhKSft->OnErrRtnQuoteAction( FhKSft->FOwner, pQuoteAction, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryContractBank( KS_::CThostFtdcContractBankField *pContractBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryContractBank != NULL )
		FhKSft->OnRspQryContractBank( FhKSft->FOwner, pContractBank, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryParkedOrder( KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryParkedOrder != NULL )
		FhKSft->OnRspQryParkedOrder( FhKSft->FOwner, pParkedOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryParkedOrderAction( KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryParkedOrderAction != NULL )
		FhKSft->OnRspQryParkedOrderAction( FhKSft->FOwner, pParkedOrderAction, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryTradingNotice( KS_::CThostFtdcTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryTradingNotice != NULL )
		FhKSft->OnRspQryTradingNotice( FhKSft->FOwner, pTradingNotice, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryBrokerTradingParams( KS_::CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryBrokerTradingParams != NULL )
		FhKSft->OnRspQryBrokerTradingParams( FhKSft->FOwner, pBrokerTradingParams, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryBrokerTradingAlgos( KS_::CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryBrokerTradingAlgos != NULL )
		FhKSft->OnRspQryBrokerTradingAlgos( FhKSft->FOwner, pBrokerTradingAlgos, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnFromBankToFutureByBank( KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	if ( FhKSft->OnRtnFromBankToFutureByBank != NULL )
		FhKSft->OnRtnFromBankToFutureByBank( FhKSft->FOwner, pRspTransfer );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnFromFutureToBankByBank( KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	if ( FhKSft->OnRtnFromFutureToBankByBank != NULL )
		FhKSft->OnRtnFromFutureToBankByBank( FhKSft->FOwner, pRspTransfer );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnRepealFromBankToFutureByBank( KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	if ( FhKSft->OnRtnRepealFromBankToFutureByBank != NULL )
		FhKSft->OnRtnRepealFromBankToFutureByBank( FhKSft->FOwner, pRspRepeal );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnRepealFromFutureToBankByBank( KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	if ( FhKSft->OnRtnRepealFromFutureToBankByBank != NULL )
		FhKSft->OnRtnRepealFromFutureToBankByBank( FhKSft->FOwner, pRspRepeal );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnFromBankToFutureByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	if ( FhKSft->OnRtnFromBankToFutureByFuture != NULL )
		FhKSft->OnRtnFromBankToFutureByFuture( FhKSft->FOwner, pRspTransfer );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnFromFutureToBankByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer )
{
	if ( FhKSft->OnRtnFromFutureToBankByFuture != NULL )
		FhKSft->OnRtnFromFutureToBankByFuture( FhKSft->FOwner, pRspTransfer );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnRepealFromBankToFutureByFutureManual( KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	if ( FhKSft->OnRtnRepealFromBankToFutureByFutureManual != NULL )
		FhKSft->OnRtnRepealFromBankToFutureByFutureManual( FhKSft->FOwner, pRspRepeal );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnRepealFromFutureToBankByFutureManual( KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	if ( FhKSft->OnRtnRepealFromFutureToBankByFutureManual != NULL )
		FhKSft->OnRtnRepealFromFutureToBankByFutureManual( FhKSft->FOwner, pRspRepeal );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnQueryBankBalanceByFuture( KS_::CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount )
{
	if ( FhKSft->OnRtnQueryBankBalanceByFuture != NULL )
		FhKSft->OnRtnQueryBankBalanceByFuture( FhKSft->FOwner, pNotifyQueryAccount );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnErrRtnBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKSft->OnErrRtnBankToFutureByFuture != NULL )
		FhKSft->OnErrRtnBankToFutureByFuture( FhKSft->FOwner, pReqTransfer, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnErrRtnFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKSft->OnErrRtnFutureToBankByFuture != NULL )
		FhKSft->OnErrRtnFutureToBankByFuture( FhKSft->FOwner, pReqTransfer, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnErrRtnRepealBankToFutureByFutureManual( KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKSft->OnErrRtnRepealBankToFutureByFutureManual != NULL )
		FhKSft->OnErrRtnRepealBankToFutureByFutureManual( FhKSft->FOwner, pReqRepeal, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnErrRtnRepealFutureToBankByFutureManual( KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKSft->OnErrRtnRepealFutureToBankByFutureManual != NULL )
		FhKSft->OnErrRtnRepealFutureToBankByFutureManual( FhKSft->FOwner, pReqRepeal, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnErrRtnQueryBankBalanceByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo )
{
	if ( FhKSft->OnErrRtnQueryBankBalanceByFuture != NULL )
		FhKSft->OnErrRtnQueryBankBalanceByFuture( FhKSft->FOwner, pReqQueryAccount, pRspInfo );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnRepealFromBankToFutureByFuture( KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	if ( FhKSft->OnRtnRepealFromBankToFutureByFuture != NULL )
		FhKSft->OnRtnRepealFromBankToFutureByFuture( FhKSft->FOwner, pRspRepeal );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnRepealFromFutureToBankByFuture( KS_::CThostFtdcRspRepealField *pRspRepeal )
{
	if ( FhKSft->OnRtnRepealFromFutureToBankByFuture != NULL )
		FhKSft->OnRtnRepealFromFutureToBankByFuture( FhKSft->FOwner, pRspRepeal );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspFromBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspFromBankToFutureByFuture != NULL )
		FhKSft->OnRspFromBankToFutureByFuture( FhKSft->FOwner, pReqTransfer, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspFromFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspFromFutureToBankByFuture != NULL )
		FhKSft->OnRspFromFutureToBankByFuture( FhKSft->FOwner, pReqTransfer, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQueryBankAccountMoneyByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQueryBankAccountMoneyByFuture != NULL )
		FhKSft->OnRspQueryBankAccountMoneyByFuture( FhKSft->FOwner, pReqQueryAccount, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnOpenAccountByBank( KS_::CThostFtdcOpenAccountField *pOpenAccount )
{
	if ( FhKSft->OnRtnOpenAccountByBank != NULL )
		FhKSft->OnRtnOpenAccountByBank( FhKSft->FOwner, pOpenAccount );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnCancelAccountByBank( KS_::CThostFtdcCancelAccountField *pCancelAccount )
{
	if ( FhKSft->OnRtnCancelAccountByBank != NULL )
		FhKSft->OnRtnCancelAccountByBank( FhKSft->FOwner, pCancelAccount );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRtnChangeAccountByBank( KS_::CThostFtdcChangeAccountField *pChangeAccount )
{
	if ( FhKSft->OnRtnChangeAccountByBank != NULL )
		FhKSft->OnRtnChangeAccountByBank( FhKSft->FOwner, pChangeAccount );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryInvestorOpenPosition( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryInvestorOpenPosition != NULL )
		FhKSft->OnRspQryInvestorOpenPosition( FhKSft->FOwner, pInvestorPositionDetail, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspQryInvestorOpenCombinePosition( KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspQryInvestorOpenCombinePosition != NULL )
		FhKSft->OnRspQryInvestorOpenCombinePosition( FhKSft->FOwner, pInvestorPositionCombineDetail, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
void KSftAPIHandler::OnRspBulkCancelOrder( KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{
	if ( FhKSft->OnRspBulkCancelOrder != NULL )
		FhKSft->OnRspBulkCancelOrder( FhKSft->FOwner, pBulkCancelOrder, pRspInfo, nRequestID, bIsLast );
}
//------------------------------------------------------------------------------------------------------------------------
