//------------------------------------------------------------------------------------------------------------------------
#ifndef _KSFT
//------------------------------------------------------------------------------------------------------------------------
#ifndef KStrdBCBTraderAPIH
#define KStrdBCBTraderAPIH
//------------------------------------------------------------------------------------------------------------------------
#include "inc\KSTradeAPI.h"
#include <windows.h>
//------------------------------------------------------------------------------------------------------------------------
#ifndef Namespace__KingstarAPI_
#define Namespace__KingstarAPI_
#define KS_ KingstarAPI
#endif
//------------------------------------------------------------------------------------------------------------------------
//
// KingStar KStrd API events listener interface
//
//------------------------------------------------------------------------------------------------------------------------
class TKStrdTraderSpi
{
public:
	virtual void OnKStrdAPIConnected() {};
	virtual void OnKStrdAPIDisconnected( int nReason ) {};
	virtual void OnKStrdAPIRecoverFinished( int Count ) {};
public:
	// ---- CThostFtdcTraderSpi ----
	virtual void OnKStrdFrontConnected( ) {};
	virtual void OnKStrdFrontDisconnected( int nReason ) {};
	virtual void OnKStrdHeartBeatWarning( int nTimeLapse ) {}; ///< no implement
	virtual void OnKStrdRspAuthenticate( KS_::CThostFtdcRspAuthenticateField *pRspAuthenticateField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspUserLogin( KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspUserLogout( KS_::CThostFtdcUserLogoutField *pUserLogout, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspUserPasswordUpdate( KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspTradingAccountPasswordUpdate( KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspParkedOrderInsert( KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspParkedOrderAction( KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspOrderAction( KS_::CThostFtdcInputOrderActionField *pInputOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQueryMaxOrderVolume( KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspSettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspRemoveParkedOrder( KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspRemoveParkedOrderAction( KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspExecOrderInsert( KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspExecOrderAction( KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspForQuoteInsert( KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQuoteInsert( KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQuoteAction( KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryOrder( KS_::CThostFtdcOrderField *pOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryTrade( KS_::CThostFtdcTradeField *pTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryInvestorPosition( KS_::CThostFtdcInvestorPositionField *pInvestorPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspQryTradingAccount( KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspQryInvestor( KS_::CThostFtdcInvestorField *pInvestor, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryTradingCode( KS_::CThostFtdcTradingCodeField *pTradingCode, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspQryInstrumentMarginRate( KS_::CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryInstrumentCommissionRate( KS_::CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspQryExchange( KS_::CThostFtdcExchangeField *pExchange, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryProduct( KS_::CThostFtdcProductField *pProduct, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryInstrument( KS_::CThostFtdcInstrumentField *pInstrument, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryDepthMarketData( KS_::CThostFtdcDepthMarketDataField *pDepthMarketData, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQrySettlementInfo( KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspQryTransferBank( KS_::CThostFtdcTransferBankField *pTransferBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspQryInvestorPositionDetail( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspQryNotice( KS_::CThostFtdcNoticeField *pNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQrySettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspQryInvestorPositionCombineDetail( KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryCFMMCTradingAccountKey( KS_::CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryEWarrantOffset( KS_::CThostFtdcEWarrantOffsetField *pEWarrantOffset, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryInvestorProductGroupMargin( KS_::CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryExchangeMarginRate( KS_::CThostFtdcExchangeMarginRateField *pExchangeMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryExchangeMarginRateAdjust( KS_::CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryExchangeRate( KS_::CThostFtdcExchangeRateField *pExchangeRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQrySecAgentACIDMap( KS_::CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryOptionInstrTradeCost( KS_::CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryOptionInstrCommRate( KS_::CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryExecOrder( KS_::CThostFtdcExecOrderField *pExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryForQuote( KS_::CThostFtdcForQuoteField *pForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryQuote( KS_::CThostFtdcQuoteField *pQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryTransferSerial( KS_::CThostFtdcTransferSerialField *pTransferSerial, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspQryAccountregister( KS_::CThostFtdcAccountregisterField *pAccountregister, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspError( KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRtnOrder( KS_::CThostFtdcOrderField *pOrder ) {};
	virtual void OnKStrdRtnTrade( KS_::CThostFtdcTradeField *pTrade ) {};
	virtual void OnKStrdErrRtnOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo ) {};
	virtual void OnKStrdErrRtnOrderAction( KS_::CThostFtdcOrderActionField *pOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo ) {};
	virtual void OnKStrdRtnInstrumentStatus( KS_::CThostFtdcInstrumentStatusField *pInstrumentStatus ) {};
	virtual void OnKStrdRtnTradingNotice( KS_::CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo ) {}; ///< no implement
	virtual void OnKStrdRtnErrorConditionalOrder( KS_::CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder ) {}; ///< no implement
	virtual void OnKStrdRtnExecOrder( KS_::CThostFtdcExecOrderField *pExecOrder ) {}; ///< no implement
	virtual void OnKStrdErrRtnExecOrderInsert( KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKStrdErrRtnExecOrderAction( KS_::CThostFtdcExecOrderActionField *pExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKStrdErrRtnForQuoteInsert( KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKStrdRtnQuote( KS_::CThostFtdcQuoteField *pQuote ) {}; ///< no implement
	virtual void OnKStrdErrRtnQuoteInsert( KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKStrdErrRtnQuoteAction( KS_::CThostFtdcQuoteActionField *pQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKStrdRtnForQuoteRsp( KS_::CThostFtdcForQuoteRspField *pForQuoteRsp ) {}; ///< no implement
	virtual void OnKStrdRtnCFMMCTradingAccountToken( KS_::CThostFtdcCFMMCTradingAccountTokenField *pCFMMCTradingAccountToken ) {}; ///< no implement
	virtual void OnKStrdRspQryContractBank( KS_::CThostFtdcContractBankField *pContractBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspQryParkedOrder( KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryParkedOrderAction( KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryTradingNotice( KS_::CThostFtdcTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryBrokerTradingParams( KS_::CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryBrokerTradingAlgos( KS_::CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQueryCFMMCTradingAccountToken( KS_::CThostFtdcQueryCFMMCTradingAccountTokenField *pQueryCFMMCTradingAccountToken, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRtnFromBankToFutureByBank( KS_::CThostFtdcRspTransferField *pRspTransfer ) {}; ///< no implement
	virtual void OnKStrdRtnFromFutureToBankByBank( KS_::CThostFtdcRspTransferField *pRspTransfer ) {}; ///< no implement
	virtual void OnKStrdRtnRepealFromBankToFutureByBank( KS_::CThostFtdcRspRepealField *pRspRepeal ) {}; ///< no implement
	virtual void OnKStrdRtnRepealFromFutureToBankByBank( KS_::CThostFtdcRspRepealField *pRspRepeal ) {}; ///< no implement
	virtual void OnKStrdRtnFromBankToFutureByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer ) {};
	virtual void OnKStrdRtnFromFutureToBankByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer ) {};
	virtual void OnKStrdRtnRepealFromBankToFutureByFutureManual( KS_::CThostFtdcRspRepealField *pRspRepeal ) {}; ///< no implement
	virtual void OnKStrdRtnRepealFromFutureToBankByFutureManual( KS_::CThostFtdcRspRepealField *pRspRepeal ) {}; ///< no implement
	virtual void OnKStrdRtnQueryBankBalanceByFuture( KS_::CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount ) {};
	virtual void OnKStrdErrRtnBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKStrdErrRtnFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKStrdErrRtnRepealBankToFutureByFutureManual( KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKStrdErrRtnRepealFutureToBankByFutureManual( KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKStrdErrRtnQueryBankBalanceByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKStrdRtnRepealFromBankToFutureByFuture( KS_::CThostFtdcRspRepealField *pRspRepeal ) {}; ///< no implement
	virtual void OnKStrdRtnRepealFromFutureToBankByFuture( KS_::CThostFtdcRspRepealField *pRspRepeal ) {}; ///< no implement
	virtual void OnKStrdRspFromBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspFromFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspQueryBankAccountMoneyByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRtnOpenAccountByBank( KS_::CThostFtdcOpenAccountField *pOpenAccount ) {}; ///< no implement
	virtual void OnKStrdRtnCancelAccountByBank( KS_::CThostFtdcCancelAccountField *pCancelAccount ) {}; ///< no implement
	virtual void OnKStrdRtnChangeAccountByBank( KS_::CThostFtdcChangeAccountField *pChangeAccount ) {}; ///< no implement
	// v2.3.50212 // 20150410 by Tim Lin
	virtual void OnKStrdRspLockInsert( KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};///鎖定應答
	virtual void OnKStrdRspQryLock( KS_::CThostFtdcLockField *pLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};///請求查詢鎖定回應
	virtual void OnKStrdRspQryLockPosition( KS_::CThostFtdcLockPositionField *pLockPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};///請求查詢鎖定證券倉位回應
	virtual void OnKStrdRtnLock( KS_::CThostFtdcLockField *pLock ) {};///鎖定通知
	virtual void OnKStrdErrRtnLockInsert( KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo ) {};///鎖定錯誤通知
	// ---- CKSOptionSpi ----
	// v2.3.50212 // 20150410 by Tim Lin
	virtual void OnKStrdRspQryIndexPrice( KS_::CKSQryIndexPriceField *pQryIndexPrice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};///查詢股指現貨指數
	virtual void OnKStrdRspQryOptionInstrGuard( KS_::CKSQryOptionInstrGuardField *pQryOptionInstrGuard, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};///查詢期權合約保障系數
	virtual void OnKStrdRspQryExecOrderVolume( KS_::CKSQryExecOrderVolumeField *pQryExecOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};///查詢宣告數量請求
	virtual void OnKStrdRspQryStockOptionAssignment( KS_::CKSQryStockOptionAssignmentField *pQryStockOptionAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};///查詢個股行權指派信息
	virtual void OnKStrdRspQryInvestorTradeLevel( KS_::CKSQryInvestorTradeLevelField *pQryInvestorTradeLevel, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};///查詢客戶交易級別
	virtual void OnKStrdRspQryPurchaseLimitAmt( KS_::CKSQryPurchaseLimitAmtField *pQryPurchaseLimitAmt, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};///查詢個股限購額度
	virtual void OnKStrdRspQryPositionLimitVol( KS_::CKSQryPositionLimitVolField *pQryPositionLimitVol, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};///查詢個股限倉額度
	virtual void OnKStrdRspQryHistoryOrder( KS_::CKSQryHistoryOrderField *pQryHistoryOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};///請求查詢個股歷史報單
	virtual void OnKStrdRspQryHistoryTrade( KS_::CKSQryHistoryTradeField *pQryHistoryTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};///請求查詢個股歷史成交
	virtual void OnKStrdRspQryStockOptionHistoryAssignment( KS_::CKSQryHistoryAssignmentField *pQryHistoryAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};///請求查詢個股歷史行權指派明細
	virtual void OnKStrdRspQryStockOptionDelivDetail( KS_::CKSQrySODelivDetailField *pQrySODelivDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};///請求查詢個股行權交割明細
	virtual void OnKStrdRspAutoExecOrderAction( KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};///自動行權執行操作
	/*
	virtual void OnKStrdRspQryKSExecOrder( KS_::CThostFtdcKSExecOrderField *pKSExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspKSExecOrderInsert( KS_::CThostFtdcInputKSExecOrderField *pInputKSExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryExecOrderVolume( KS_::CKSExecOrderVolumeField *pExecOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQrySpotOptionParams( KS_::CKSSpotOptionParamsField *pExecOrderParams, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryOptionInsInfo( KS_::CKSOptionInsInfoField *pOptionInsInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryStockOptionInsCommRate( KS_::CKSStockOptionInsCommRateField *pStockOptionInsCommRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryStockOptionInsMarginRate( KS_::CKSStockOptionInsMarginRateField *pStockOptionInsMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryStockOptionAssignment( KS_::CKSStockOptionAssignmentField *pStockOptionAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryInvestorTradeLevel( KS_::CKSInvestorTradeLevelField *pInvestorTradeLevel, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryPurchaseLimitAmt( KS_::CKSPurchaseLimitAmtField *pPurchaseLimitAmt, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryPositionLimitVol( KS_::CKSPositionLimitVolField *pPositionLimitVol, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryHistoryOrder( KS_::CKSHistoryOrderField *pOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryHistoryTrade( KS_::CKSHistoryTradeField *pTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryStockOptionHistoryAssignment( KS_::CKSHistoryAssignmentField *pHistoryAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryStockOptionDelivDetail( KS_::CKSSODelivDetailField *pSODelivDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspAutoExecOrderAction( KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRtnKSExecOrder( KS_::CThostFtdcKSExecOrderRtnField *pExecOrder ) {}; ///< no implement
	*/
	// ---- CKSVocSpi ----
	// v2.3.50212 // 20150410 by Tim Lin
	virtual void OnKStrdRspQryInvestorOpenPosition( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKStrdRspQryInvestorOpenCombinePosition( KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspBulkCancelOrder( KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryCloseStrategy( KS_::CKSCloseStrategyResultField *pCloseStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryCombStrategy( KS_::CKSCombStrategyResultField *pCombStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryOptionCombStrategy( KS_::CKSOptionCombStrategyResultField *pOptionCombStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryTransferInfo( KS_::CKSTransferInfoResultField *pResultField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryKSTradingNotice( KS_::CKSTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryUserProductUrl( KS_::CKSUserProductUrlField *pUserProductUrl, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryMaxCombActionVolume( KS_::CKSMaxCombActionVolumeField *pMaxCombActionVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRtnKSTradingNotice( KS_::CKSTradingNoticeField *pTradingNoticeInfo ) {}; ///< no implement
	virtual void OnKStrdRspQryKSInstrumentMarginRate( KS_::CKSInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}; ///< no implement
	/*
	virtual void OnKStrdRspQryCrossRate( KS_::CKSCrossRateField *pCrossRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspMatchOrderInsert( KS_::CKSMatchOrderInsertField *pMatchOrderInsert, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryMatchOrder( KS_::CKSMatchOrderField *pMatchOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRspQryMaxMatchOrderVolume( KS_::CKSMaxMatchOrderVolumeField *pMaxMatchOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKStrdRtnKSTradingNotice( KS_::CKSTradingNoticeField *pTradingNoticeInfo ) {};
	virtual void OnKStrdRtnMatchOrder( KS_::CKSMatchOrderField *pMatchOrder ) {};
	*/
};
//------------------------------------------------------------------------------------------------------------------------
//
// KingStar KStrd Trader API class
//
//------------------------------------------------------------------------------------------------------------------------
class TKStrdTraderApi
{
private:
	void*				FKSTRDAPI;
	TKStrdTraderSpi*	FListener;
private:
	HWND				FHwnd;
	HINSTANCE			FInstance;
	DWORD				FThreadID;
	wchar_t MessageWindowsClassName[ 128 ];
private:
	BOOL InitMessageWindow( void );
	BOOL CreateMessageWindow( void );
	void RegAllCallbackFuncs( void );
public:
	static LRESULT CALLBACK MessageWndProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam );
	static bool SupportKStrdAPI( void );
	static const char* LoadKStrdAPIResult( void );
public:
	TKStrdTraderSpi*	GetSpi( void )	{ return FListener; }
	HWND				GetHWND( void )	{ return FHwnd; }
public:
	void RegisterSpi( TKStrdTraderSpi *pSpi );
	void ReqOnRecoverFilisned( int Count );
public:
	// ---- CThostFtdcTraderApi ----
	void Init();
	const char *GetTradingDay();
	void RegisterFront( char *pszFrontAddress );
	void RegisterNameServer( char *pszNsAddress );
	void RegisterFensUserInfo( KS_::CThostFtdcFensUserInfoField * pFensUserInfo );
	void SubscribePrivateTopic( KS_::THOST_TE_RESUME_TYPE nResumeType );
	void SubscribePublicTopic( KS_::THOST_TE_RESUME_TYPE nResumeType );
	int ReqAuthenticate( KS_::CThostFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID );
	int ReqUserLogin( KS_::CThostFtdcReqUserLoginField *pReqUserLoginField, int nRequestID );
	int ReqUserLogout( KS_::CThostFtdcUserLogoutField *pUserLogout, int nRequestID );
	int ReqUserPasswordUpdate( KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID );
	int ReqTradingAccountPasswordUpdate( KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, int nRequestID );
	int ReqOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, int nRequestID );
	int ReqParkedOrderInsert( KS_::CThostFtdcParkedOrderField *pParkedOrder, int nRequestID );
	int ReqParkedOrderAction( KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, int nRequestID );
	int ReqOrderAction( KS_::CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID );
	int ReqQueryMaxOrderVolume( KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, int nRequestID );
	int ReqSettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, int nRequestID );
	int ReqRemoveParkedOrder( KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, int nRequestID );
	int ReqRemoveParkedOrderAction( KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, int nRequestID );
	int ReqExecOrderInsert( KS_::CThostFtdcInputExecOrderField *pInputExecOrder, int nRequestID );
	int ReqExecOrderAction( KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, int nRequestID );
	int ReqForQuoteInsert( KS_::CThostFtdcInputForQuoteField *pInputForQuote, int nRequestID );
	int ReqQuoteInsert( KS_::CThostFtdcInputQuoteField *pInputQuote, int nRequestID );
	int ReqQuoteAction( KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, int nRequestID );
	int ReqQryOrder( KS_::CThostFtdcQryOrderField *pQryOrder, int nRequestID );
	int ReqQryTrade( KS_::CThostFtdcQryTradeField *pQryTrade, int nRequestID );
	int ReqQryInvestorPosition( KS_::CThostFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID );
	int ReqQryTradingAccount( KS_::CThostFtdcQryTradingAccountField *pQryTradingAccount, int nRequestID );
	int ReqQryInvestor( KS_::CThostFtdcQryInvestorField *pQryInvestor, int nRequestID );
	int ReqQryTradingCode( KS_::CThostFtdcQryTradingCodeField *pQryTradingCode, int nRequestID );
	int ReqQryInstrumentMarginRate( KS_::CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID );
	int ReqQryInstrumentCommissionRate( KS_::CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate, int nRequestID );
	int ReqQryExchange( KS_::CThostFtdcQryExchangeField *pQryExchange, int nRequestID );
	int ReqQryProduct( KS_::CThostFtdcQryProductField *pQryProduct, int nRequestID );
	int ReqQryInstrument( KS_::CThostFtdcQryInstrumentField *pQryInstrument, int nRequestID );
	int ReqQryDepthMarketData( KS_::CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, int nRequestID );
	int ReqQrySettlementInfo( KS_::CThostFtdcQrySettlementInfoField *pQrySettlementInfo, int nRequestID );
	int ReqQryTransferBank( KS_::CThostFtdcQryTransferBankField *pQryTransferBank, int nRequestID );
	int ReqQryInvestorPositionDetail( KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, int nRequestID );
	int ReqQryNotice( KS_::CThostFtdcQryNoticeField *pQryNotice, int nRequestID );
	int ReqQrySettlementInfoConfirm( KS_::CThostFtdcQrySettlementInfoConfirmField *pQrySettlementInfoConfirm, int nRequestID );
	int ReqQryInvestorPositionCombineDetail( KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorPositionCombineDetail, int nRequestID );
	int ReqQryCFMMCTradingAccountKey( KS_::CThostFtdcQryCFMMCTradingAccountKeyField *pQryCFMMCTradingAccountKey, int nRequestID );
	int ReqQryEWarrantOffset( KS_::CThostFtdcQryEWarrantOffsetField *pQryEWarrantOffset, int nRequestID );
	int ReqQryInvestorProductGroupMargin( KS_::CThostFtdcQryInvestorProductGroupMarginField *pQryInvestorProductGroupMargin, int nRequestID );
	int ReqQryExchangeMarginRate( KS_::CThostFtdcQryExchangeMarginRateField *pQryExchangeMarginRate, int nRequestID);
	int ReqQryExchangeMarginRateAdjust( KS_::CThostFtdcQryExchangeMarginRateAdjustField *pQryExchangeMarginRateAdjust, int nRequestID );
	int ReqQryExchangeRate( KS_::CThostFtdcQryExchangeRateField *pQryExchangeRate, int nRequestID );
	int ReqQrySecAgentACIDMap( KS_::CThostFtdcQrySecAgentACIDMapField *pQrySecAgentACIDMap, int nRequestID );
	int ReqQryOptionInstrTradeCost( KS_::CThostFtdcQryOptionInstrTradeCostField *pQryOptionInstrTradeCost, int nRequestID );
	int ReqQryOptionInstrCommRate( KS_::CThostFtdcQryOptionInstrCommRateField *pQryOptionInstrCommRate, int nRequestID );
	int ReqQryExecOrder( KS_::CThostFtdcQryExecOrderField *pQryExecOrder, int nRequestID );
	int ReqQryForQuote( KS_::CThostFtdcQryForQuoteField *pQryForQuote, int nRequestID );
	int ReqQryQuote( KS_::CThostFtdcQryQuoteField *pQryQuote, int nRequestID );
	int ReqQryTransferSerial( KS_::CThostFtdcQryTransferSerialField *pQryTransferSerial, int nRequestID );
	int ReqQryAccountregister( KS_::CThostFtdcQryAccountregisterField *pQryAccountregister, int nRequestID );
	int ReqQryContractBank( KS_::CThostFtdcQryContractBankField *pQryContractBank, int nRequestID );
	int ReqQryParkedOrder( KS_::CThostFtdcQryParkedOrderField *pQryParkedOrder, int nRequestID );
	int ReqQryParkedOrderAction( KS_::CThostFtdcQryParkedOrderActionField *pQryParkedOrderAction, int nRequestID );
	int ReqQryTradingNotice( KS_::CThostFtdcQryTradingNoticeField *pQryTradingNotice, int nRequestID );
	int ReqQryBrokerTradingParams( KS_::CThostFtdcQryBrokerTradingParamsField *pQryBrokerTradingParams, int nRequestID );
	int ReqQryBrokerTradingAlgos( KS_::CThostFtdcQryBrokerTradingAlgosField *pQryBrokerTradingAlgos, int nRequestID );
	int ReqQueryCFMMCTradingAccountToken( KS_::CThostFtdcQueryCFMMCTradingAccountTokenField *pQueryCFMMCTradingAccountToken, int nRequestID );
	int ReqFromBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID );
	int ReqFromFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID );
	int ReqQueryBankAccountMoneyByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, int nRequestID );
	// v2.3.50212 // 20150410 by Tim Lin
	int ReqLockInsert( KS_::CThostFtdcInputLockField *pInputLock, int nRequestID );///用戶端發出鎖定請求
	int ReqQryLock( KS_::CThostFtdcQryLockField *pQryLock, int nRequestID );///用戶端發出查詢鎖定請求
	int ReqQryLockPosition( KS_::CThostFtdcQryLockPositionField *pQryLockPosition, int nRequestID );///用戶端發出查詢鎖定證券倉位請求
	// ---- CKSOptionApi ----
	// v2.3.50212 // 20150410 by Tim Lin
	int ReqQryIndexPrice( KS_::CKSQryIndexPriceField *pQryIndexPrice, int nRequestID );///查詢股指現貨指數
	int ReqQryOptionInstrGuard( KS_::CKSQryOptionInstrGuardField *pQryOptionInstrGuard, int nRequestID );///查詢期權合約保障系數
	int ReqQryExecOrderVolume( KS_::CKSQryExecOrderVolumeField *pQryExecOrderVolume, int nRequestID );///查詢宣告數量請求
	int ReqQryStockOptionAssignment( KS_::CKSQryStockOptionAssignmentField *pQryStockOptionAssignment, int nRequestID );///查詢個股行權指派信息
	int ReqQryInvestorTradeLevel( KS_::CKSQryInvestorTradeLevelField *pQryInvestorTradeLevel, int nRequestID );///查詢客戶交易級別
	int ReqQryPurchaseLimitAmt( KS_::CKSQryPurchaseLimitAmtField *pQryPurchaseLimitAmt, int nRequestID );///查詢個股限購額度
	int ReqQryPositionLimitVol( KS_::CKSQryPositionLimitVolField *pQryPositionLimitVol, int nRequestID );///查詢個股限倉額度
	int ReqQryHistoryOrder( KS_::CKSQryHistoryOrderField *pQryHistoryOrder, int nRequestID );///請求查詢個股歷史報單
	int ReqQryHistoryTrade( KS_::CKSQryHistoryTradeField *pQryHistoryTrade, int nRequestID );///請求查詢個股歷史成交
	int ReqQryStockOptionHistoryAssignment( KS_::CKSQryHistoryAssignmentField *pQryHistoryAssignment, int nRequestID );///請求查詢個股歷史行權指派明細
	int ReqQryStockOptionDelivDetail( KS_::CKSQrySODelivDetailField *pQrySODelivDetail, int nRequestID );///請求查詢個股行權交割明細
	int ReqAutoExecOrderAction( KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, int nRequestID );///自動行權執行操作
	/*
	int ReqQryKSExecOrder( KS_::CThostFtdcQryKSExecOrderField *pQryKSExecOrder, int nRequestID );
	int ReqKSExecOrderInsert( KS_::CThostFtdcInputKSExecOrderField *pInputKSExecOrder, int nRequestID );
	int ReqQryExecOrderVolume( KS_::CKSQryExecOrderVolumeField *pQryExecOrderVolume, int nRequestID );
	int ReqQrySpotOptionParams( KS_::CKSQrySpotOptionParamsField *pQrySpotOptionParams, int nRequestID );
	int ReqQryOptionInsInfo( KS_::CKSQryOptionInsInfoField *pQryOptionInsInfo, int nRequestID );
	int ReqQryStockOptionInsCommRate( KS_::CKSQryStockOptionInsCommRateField *pQryStockOptionInsCommRate, int nRequestID );
	int ReqQryStockOptionInsMarginRate( KS_::CKSQryStockOptionInsMarginRateField *pQryStockOptionInsMarginRate, int nRequestID );
	int ReqQryStockOptionAssignment( KS_::CKSQryStockOptionAssignmentField *pQryStockOptionAssignment, int nRequestID );
	int ReqQryInvestorTradeLevel( KS_::CKSQryInvestorTradeLevelField *pQryInvestorTradeLevel, int nRequestID );
	int ReqQryPurchaseLimitAmt( KS_::CKSQryPurchaseLimitAmtField *pQryPurchaseLimitAmt, int nRequestID );
	int ReqQryPositionLimitVol( KS_::CKSQryPositionLimitVolField *pQryPositionLimitVol, int nRequestID );
	int ReqQryHistoryOrder( KS_::CKSQryHistoryOrderField *pQryHistoryOrder, int nRequestID );
	int ReqQryHistoryTrade( KS_::CKSQryHistoryTradeField *pQryHistoryTrade, int nRequestID );
	int ReqQryStockOptionHistoryAssignment( KS_::CKSQryHistoryAssignmentField *pQryHistoryAssignment, int nRequestID );
	int ReqQryStockOptionDelivDetail( KS_::CKSQrySODelivDetailField *pQrySODelivDetail, int nRequestID );
	int ReqAutoExecOrderAction( KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, int nRequestID );
	*/
	// ---- CKSVocApi ----
	// v2.3.50212 // 20150410 by Tim Lin
	int ReqQueryInvestorOpenPosition( KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorOpenPosition, int nRequestID );///查詢開盤前的持倉明細
	int ReqQueryInvestorOpenCombinePosition( KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorOpenCombinePosition, int nRequestID );///查詢開盤前的組合持倉明細
	int ReqBulkCancelOrder( KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, int nRequestID );///批量撤單
	int ReqQryCloseStrategy( KS_::CKSCloseStrategy *pCloseStrategy, int nRequestID );///平倉策略查詢請求
	int ReqQryCombStrategy( KS_::CKSCombStrategy *pCombStrategy, int nRequestID );///組合策略查詢請求
	int ReqQryOptionCombStrategy( KS_::CKSOptionCombStrategy *pOptionCombStrategy, int nRequestID );///期權組合策略查詢請求
	int ReqQryTransferInfo( KS_::CKSTransferInfo *pTransferInfo, int nRequestID );///請求查詢客戶轉帳資訊
	int ReqQryKSTradingNotice( KS_::CKSQryTradingNoticeField *pQryTradingNotice, int nRequestID );///請求查詢交易通知
	int ReqQryUserProductUrl( KS_::CKSQryUserProductUrlField *pQryUserProductUrl, int nRequestID );///使用者端產品資源查詢請求
	int ReqQryMaxCombActionVolume( KS_::CKSQryMaxCombActionVolumeField *pQryMaxCombActionVolume, int nRequestID );///最大組合拆分單量查詢請求回應
	int ReqQryKSInstrumentMarginRate( KS_::CKSQryInstrumentMarginRateField *pQryInstrumentMarginRate, int nRequestID);///請求查詢合約保證金率
	/*
	int ReqQryCrossRate( KS_::CKSQryCrossRateField *pQryCrossRate, int nRequestID );
	int ReqMatchOrderInsert( KS_::CKSMatchOrderInsertField *pMatchOrderInsert, int nRequestID );
	int ReqQryMatchOrder( KS_::CKSQryMatchOrderField *pQryMatchOrder, int nRequestID );
	int ReqQryMaxMatchOrderVolume( KS_::CKSQryMaxMatchOrderVolumeField *pQryMaxMatchOrderVolume, int nRequestID );
	*/
public:
	TKStrdTraderApi( TKStrdTraderSpi* pSpi );
	~TKStrdTraderApi();
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------

