//------------------------------------------------------------------------------------------------------------------------
#ifndef __KStrdAPIComm_H
#define __KStrdAPIComm_H
//------------------------------------------------------------------------------------------------------------------------
#include "Stdafx.h"
#include "KStrdAPIFactory.h"
#include <stdio.h>
//------------------------------------------------------------------------------------------------------------------------
typedef struct tagKStrdHandle
{
	KS_::CThostFtdcTraderApi*	KStrdObj;
	KS_::CKSOptionApi*			KSoptObj;
	KS_::CKSVocApi*				KSvocObj;
	void*						KStrdHandler;
	void*						FOwner;
	// ---- CThostFtdcTraderSpi ----
	pfOnFrontConnected							OnFrontConnected;
	pfOnFrontDisconnected						OnFrontDisconnected;
	pfOnHeartBeatWarning						OnHeartBeatWarning;
	pfOnRspAuthenticate							OnRspAuthenticate;
	pfOnRspUserLogin							OnRspUserLogin;
	pfOnRspUserLogout							OnRspUserLogout;
	pfOnRspUserPasswordUpdate					OnRspUserPasswordUpdate;
	pfOnRspTradingAccountPasswordUpdate			OnRspTradingAccountPasswordUpdate;
	pfOnRspOrderInsert							OnRspOrderInsert;
	pfOnRspParkedOrderInsert					OnRspParkedOrderInsert;
	pfOnRspParkedOrderAction					OnRspParkedOrderAction;
	pfOnRspOrderAction							OnRspOrderAction;
	pfOnRspQueryMaxOrderVolume					OnRspQueryMaxOrderVolume;
	pfOnRspSettlementInfoConfirm				OnRspSettlementInfoConfirm;
	pfOnRspRemoveParkedOrder					OnRspRemoveParkedOrder;
	pfOnRspRemoveParkedOrderAction				OnRspRemoveParkedOrderAction;
	pfOnRspExecOrderInsert						OnRspExecOrderInsert;
	pfOnRspExecOrderAction						OnRspExecOrderAction;
	pfOnRspForQuoteInsert						OnRspForQuoteInsert;
	pfOnRspQuoteInsert							OnRspQuoteInsert;
	pfOnRspQuoteAction							OnRspQuoteAction;
	pfOnRspQryOrder								OnRspQryOrder;
	pfOnRspQryTrade								OnRspQryTrade;
	pfOnRspQryInvestorPosition					OnRspQryInvestorPosition;
	pfOnRspQryTradingAccount					OnRspQryTradingAccount;
	pfOnRspQryInvestor							OnRspQryInvestor;
	pfOnRspQryTradingCode						OnRspQryTradingCode;
	pfOnRspQryInstrumentMarginRate				OnRspQryInstrumentMarginRate;
	pfOnRspQryInstrumentCommissionRate			OnRspQryInstrumentCommissionRate;
	pfOnRspQryExchange							OnRspQryExchange;
	pfOnRspQryProduct							OnRspQryProduct;
	pfOnRspQryInstrument						OnRspQryInstrument;
	pfOnRspQryDepthMarketData					OnRspQryDepthMarketData;
	pfOnRspQrySettlementInfo					OnRspQrySettlementInfo;
	pfOnRspQryTransferBank						OnRspQryTransferBank;
	pfOnRspQryInvestorPositionDetail			OnRspQryInvestorPositionDetail;
	pfOnRspQryNotice							OnRspQryNotice;
	pfOnRspQrySettlementInfoConfirm				OnRspQrySettlementInfoConfirm;
	pfOnRspQryInvestorPositionCombineDetail		OnRspQryInvestorPositionCombineDetail;
	pfOnRspQryCFMMCTradingAccountKey			OnRspQryCFMMCTradingAccountKey;
	pfOnRspQryEWarrantOffset					OnRspQryEWarrantOffset;
	pfOnRspQryInvestorProductGroupMargin		OnRspQryInvestorProductGroupMargin;
	pfOnRspQryExchangeMarginRate				OnRspQryExchangeMarginRate;
	pfOnRspQryExchangeMarginRateAdjust			OnRspQryExchangeMarginRateAdjust;
	pfOnRspQryExchangeRate						OnRspQryExchangeRate;
	pfOnRspQrySecAgentACIDMap					OnRspQrySecAgentACIDMap;
	pfOnRspQryOptionInstrTradeCost				OnRspQryOptionInstrTradeCost;
	pfOnRspQryOptionInstrCommRate				OnRspQryOptionInstrCommRate;
	pfOnRspQryExecOrder							OnRspQryExecOrder;
	pfOnRspQryForQuote							OnRspQryForQuote;
	pfOnRspQryQuote								OnRspQryQuote;
	pfOnRspQryTransferSerial					OnRspQryTransferSerial;
	pfOnRspQryAccountregister					OnRspQryAccountregister;
	pfOnRspError								OnRspError;
	pfOnRtnOrder								OnRtnOrder;
	pfOnRtnTrade								OnRtnTrade;
	pfOnErrRtnOrderInsert						OnErrRtnOrderInsert;
	pfOnErrRtnOrderAction						OnErrRtnOrderAction;
	pfOnRtnInstrumentStatus						OnRtnInstrumentStatus;
	pfOnRtnTradingNotice						OnRtnTradingNotice;
	pfOnRtnErrorConditionalOrder				OnRtnErrorConditionalOrder;
	pfOnRtnExecOrder							OnRtnExecOrder;
	pfOnErrRtnExecOrderInsert					OnErrRtnExecOrderInsert;
	pfOnErrRtnExecOrderAction					OnErrRtnExecOrderAction;
	pfOnErrRtnForQuoteInsert					OnErrRtnForQuoteInsert;
	pfOnRtnQuote								OnRtnQuote;
	pfOnErrRtnQuoteInsert						OnErrRtnQuoteInsert;
	pfOnErrRtnQuoteAction						OnErrRtnQuoteAction;
	pfOnRtnForQuoteRsp							OnRtnForQuoteRsp;
	pfOnRtnCFMMCTradingAccountToken				OnRtnCFMMCTradingAccountToken;
	pfOnRspQryContractBank						OnRspQryContractBank;
	pfOnRspQryParkedOrder						OnRspQryParkedOrder;
	pfOnRspQryParkedOrderAction					OnRspQryParkedOrderAction;
	pfOnRspQryTradingNotice						OnRspQryTradingNotice;
	pfOnRspQryBrokerTradingParams				OnRspQryBrokerTradingParams;
	pfOnRspQryBrokerTradingAlgos				OnRspQryBrokerTradingAlgos;
	pfOnRspQueryCFMMCTradingAccountToken		OnRspQueryCFMMCTradingAccountToken;
	pfOnRtnFromBankToFutureByBank				OnRtnFromBankToFutureByBank;
	pfOnRtnFromFutureToBankByBank				OnRtnFromFutureToBankByBank;
	pfOnRtnRepealFromBankToFutureByBank			OnRtnRepealFromBankToFutureByBank;
	pfOnRtnRepealFromFutureToBankByBank			OnRtnRepealFromFutureToBankByBank;
	pfOnRtnFromBankToFutureByFuture				OnRtnFromBankToFutureByFuture;
	pfOnRtnFromFutureToBankByFuture				OnRtnFromFutureToBankByFuture;
	pfOnRtnRepealFromBankToFutureByFutureManual	OnRtnRepealFromBankToFutureByFutureManual;
	pfOnRtnRepealFromFutureToBankByFutureManual	OnRtnRepealFromFutureToBankByFutureManual;
	pfOnRtnQueryBankBalanceByFuture				OnRtnQueryBankBalanceByFuture;
	pfOnErrRtnBankToFutureByFuture				OnErrRtnBankToFutureByFuture;
	pfOnErrRtnFutureToBankByFuture				OnErrRtnFutureToBankByFuture;
	pfOnErrRtnRepealBankToFutureByFutureManual	OnErrRtnRepealBankToFutureByFutureManual;
	pfOnErrRtnRepealFutureToBankByFutureManual	OnErrRtnRepealFutureToBankByFutureManual;
	pfOnErrRtnQueryBankBalanceByFuture			OnErrRtnQueryBankBalanceByFuture;
	pfOnRtnRepealFromBankToFutureByFuture		OnRtnRepealFromBankToFutureByFuture;
	pfOnRtnRepealFromFutureToBankByFuture		OnRtnRepealFromFutureToBankByFuture;
	pfOnRspFromBankToFutureByFuture				OnRspFromBankToFutureByFuture;
	pfOnRspFromFutureToBankByFuture				OnRspFromFutureToBankByFuture;
	pfOnRspQueryBankAccountMoneyByFuture		OnRspQueryBankAccountMoneyByFuture;
	pfOnRtnOpenAccountByBank					OnRtnOpenAccountByBank;
	pfOnRtnCancelAccountByBank					OnRtnCancelAccountByBank;
	pfOnRtnChangeAccountByBank					OnRtnChangeAccountByBank;
	// v2.3.50212 // 20150410 by Tim Lin // <Step>.5
	pfOnRspLockInsert							OnRspLockInsert;///鎖定應答
	pfOnRspQryLock								OnRspQryLock;///請求查詢鎖定回應	
	pfOnRspQryLockPosition						OnRspQryLockPosition;///請求查詢鎖定證券倉位回應
	pfOnRtnLock									OnRtnLock;///鎖定通知
	pfOnErrRtnLockInsert						OnErrRtnLockInsert;///鎖定錯誤通知	
	// ---- CKSOptionSpi ----
	// v2.3.50212 // 20150410 by Tim Lin // Step.5
	pfOnRspQryIndexPrice						OnRspQryIndexPrice;///查詢股指現貨指數
	pfOnRspQryOptionInstrGuard					OnRspQryOptionInstrGuard;///查詢期權合約保障系數
	pfOnRspQryExecOrderVolume					OnRspQryExecOrderVolume;///查詢宣告數量請求
	pfOnRspQryStockOptionAssignment				OnRspQryStockOptionAssignment;///查詢個股行權指派信息
	pfOnRspQryInvestorTradeLevel				OnRspQryInvestorTradeLevel;///查詢客戶交易級別
	pfOnRspQryPurchaseLimitAmt					OnRspQryPurchaseLimitAmt;///查詢個股限購額度
	pfOnRspQryPositionLimitVol					OnRspQryPositionLimitVol;///查詢個股限倉額度
	pfOnRspQryHistoryOrder						OnRspQryHistoryOrder;///請求查詢個股歷史報單
	pfOnRspQryHistoryTrade						OnRspQryHistoryTrade;///請求查詢個股歷史成交
	pfOnRspQryStockOptionHistoryAssignment		OnRspQryHistoryAssignment;///請求查詢個股歷史行權指派明細
	pfOnRspQryStockOptionDelivDetail			OnRspQrySODelivDetail;///請求查詢個股行權交割明細
	pfOnRspAutoExecOrderAction					OnRspAutoExecOrderAction;///自動行權執行操作
	// ---- CKSVocSpi ----
	// v2.3.50212 // 20150410 by Tim Lin // [Step].5
	pfOnRspQryInvestorOpenPosition				OnRspQryInvestorOpenPosition;///查詢開盤前的持倉明細應答
	pfOnRspQryInvestorOpenCombinePosition		OnRspQryInvestorOpenCombinePosition;///查詢開盤前的組合持倉明細應答
	pfOnRspBulkCancelOrder						OnRspBulkCancelOrder;///批量報單撤除請求回應
	pfOnRspQryCloseStrategy						OnRspQryCloseStrategy;///平倉策略查詢回應
	pfOnRspQryCombStrategy						OnRspQryCombStrategy;///組合策略查詢回應
	pfOnRspQryOptionCombStrategy				OnRspQryOptionCombStrategy;///期權組合策略查詢回應
	pfOnRspQryTransferInfo						OnRspQryTransferInfo;///請求查詢客戶轉帳資訊回應
	pfOnRspQryKSTradingNotice					OnRspQryKSTradingNotice;///請求查詢交易通知回應
	pfOnRspQryUserProductUrl					OnRspQryUserProductUrl;///使用者端產品資源查詢應答
	pfOnRspQryMaxCombActionVolume				OnRspQryMaxCombActionVolume;///最大組合拆分單量查詢請求回應
	pfOnRtnKSTradingNotice						OnRtnKSTradingNotice;///交易通知
	pfOnRspQryKSInstrumentMarginRate			OnRspQryKSInstrumentMarginRate;///請求查詢合約保證金率回應
} KStrdHandle;
//------------------------------------------------------------------------------------------------------------------------
// class KStrdAPIHandler
//------------------------------------------------------------------------------------------------------------------------
class KStrdAPIHandler : public KS_::CThostFtdcTraderSpi, public KS_::CKSOptionSpi, public KS_::CKSVocSpi
{
private:
	KStrdHandle*	FhKStrd;
public:
	// ---- CThostFtdcTraderSpi ----
	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected( int nReason );
	virtual void OnHeartBeatWarning( int nTimeLapse );
	virtual void OnRspAuthenticate( KS_::CThostFtdcRspAuthenticateField *pRspAuthenticateField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspUserLogin( KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspUserLogout( KS_::CThostFtdcUserLogoutField *pUserLogout, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspUserPasswordUpdate( KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspTradingAccountPasswordUpdate( KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspParkedOrderInsert( KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspParkedOrderAction( KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspOrderAction( KS_::CThostFtdcInputOrderActionField *pInputOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQueryMaxOrderVolume( KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspSettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspRemoveParkedOrder( KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspRemoveParkedOrderAction( KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspExecOrderInsert( KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspExecOrderAction( KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspForQuoteInsert( KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQuoteInsert( KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQuoteAction( KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryOrder( KS_::CThostFtdcOrderField *pOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryTrade( KS_::CThostFtdcTradeField *pTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryInvestorPosition( KS_::CThostFtdcInvestorPositionField *pInvestorPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryTradingAccount( KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryInvestor( KS_::CThostFtdcInvestorField *pInvestor, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryTradingCode( KS_::CThostFtdcTradingCodeField *pTradingCode, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryInstrumentMarginRate( KS_::CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryInstrumentCommissionRate( KS_::CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryExchange( KS_::CThostFtdcExchangeField *pExchange, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryProduct( KS_::CThostFtdcProductField *pProduct, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryInstrument( KS_::CThostFtdcInstrumentField *pInstrument, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryDepthMarketData( KS_::CThostFtdcDepthMarketDataField *pDepthMarketData, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQrySettlementInfo( KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryTransferBank( KS_::CThostFtdcTransferBankField *pTransferBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryInvestorPositionDetail( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryNotice( KS_::CThostFtdcNoticeField *pNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQrySettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryInvestorPositionCombineDetail( KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryCFMMCTradingAccountKey( KS_::CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryEWarrantOffset( KS_::CThostFtdcEWarrantOffsetField *pEWarrantOffset, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryInvestorProductGroupMargin( KS_::CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryExchangeMarginRate( KS_::CThostFtdcExchangeMarginRateField *pExchangeMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryExchangeMarginRateAdjust( KS_::CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryExchangeRate( KS_::CThostFtdcExchangeRateField *pExchangeRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQrySecAgentACIDMap( KS_::CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryOptionInstrTradeCost( KS_::CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryOptionInstrCommRate( KS_::CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryExecOrder( KS_::CThostFtdcExecOrderField *pExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryForQuote( KS_::CThostFtdcForQuoteField *pForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryQuote( KS_::CThostFtdcQuoteField *pQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryTransferSerial( KS_::CThostFtdcTransferSerialField *pTransferSerial, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryAccountregister( KS_::CThostFtdcAccountregisterField *pAccountregister, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspError( KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRtnOrder( KS_::CThostFtdcOrderField *pOrder );
	virtual void OnRtnTrade( KS_::CThostFtdcTradeField *pTrade );
	virtual void OnErrRtnOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo );
	virtual void OnErrRtnOrderAction( KS_::CThostFtdcOrderActionField *pOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo );
	virtual void OnRtnInstrumentStatus( KS_::CThostFtdcInstrumentStatusField *pInstrumentStatus );
	virtual void OnRtnTradingNotice( KS_::CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo );
	virtual void OnRtnErrorConditionalOrder( KS_::CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder );
	virtual void OnRtnExecOrder( KS_::CThostFtdcExecOrderField *pExecOrder );
	virtual void OnErrRtnExecOrderInsert( KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo );
	virtual void OnErrRtnExecOrderAction( KS_::CThostFtdcExecOrderActionField *pExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo );
	virtual void OnErrRtnForQuoteInsert( KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo );
	virtual void OnRtnQuote( KS_::CThostFtdcQuoteField *pQuote );
	virtual void OnErrRtnQuoteInsert( KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo );
	virtual void OnErrRtnQuoteAction( KS_::CThostFtdcQuoteActionField *pQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo );
	virtual void OnRtnForQuoteRsp( KS_::CThostFtdcForQuoteRspField *pForQuoteRsp );
	virtual void OnRtnCFMMCTradingAccountToken( KS_::CThostFtdcCFMMCTradingAccountTokenField *pCFMMCTradingAccountToken );
	virtual void OnRspQryContractBank( KS_::CThostFtdcContractBankField *pContractBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryParkedOrder( KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryParkedOrderAction( KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryTradingNotice( KS_::CThostFtdcTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryBrokerTradingParams( KS_::CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQryBrokerTradingAlgos( KS_::CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQueryCFMMCTradingAccountToken( KS_::CThostFtdcQueryCFMMCTradingAccountTokenField *pQueryCFMMCTradingAccountToken, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRtnFromBankToFutureByBank( KS_::CThostFtdcRspTransferField *pRspTransfer );
	virtual void OnRtnFromFutureToBankByBank( KS_::CThostFtdcRspTransferField *pRspTransfer );
	virtual void OnRtnRepealFromBankToFutureByBank( KS_::CThostFtdcRspRepealField *pRspRepeal );
	virtual void OnRtnRepealFromFutureToBankByBank( KS_::CThostFtdcRspRepealField *pRspRepeal );
	virtual void OnRtnFromBankToFutureByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer );
	virtual void OnRtnFromFutureToBankByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer );
	virtual void OnRtnRepealFromBankToFutureByFutureManual( KS_::CThostFtdcRspRepealField *pRspRepeal );
	virtual void OnRtnRepealFromFutureToBankByFutureManual( KS_::CThostFtdcRspRepealField *pRspRepeal );
	virtual void OnRtnQueryBankBalanceByFuture( KS_::CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount );
	virtual void OnErrRtnBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo );
	virtual void OnErrRtnFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo );
	virtual void OnErrRtnRepealBankToFutureByFutureManual( KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo );
	virtual void OnErrRtnRepealFutureToBankByFutureManual( KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo );
	virtual void OnErrRtnQueryBankBalanceByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo );
	virtual void OnRtnRepealFromBankToFutureByFuture( KS_::CThostFtdcRspRepealField *pRspRepeal );
	virtual void OnRtnRepealFromFutureToBankByFuture( KS_::CThostFtdcRspRepealField *pRspRepeal );
	virtual void OnRspFromBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspFromFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRspQueryBankAccountMoneyByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	virtual void OnRtnOpenAccountByBank( KS_::CThostFtdcOpenAccountField *pOpenAccount );
	virtual void OnRtnCancelAccountByBank( KS_::CThostFtdcCancelAccountField *pCancelAccount );
	virtual void OnRtnChangeAccountByBank( KS_::CThostFtdcChangeAccountField *pChangeAccount );
	// v2.3.50212 // 20150410 by Tim Lin // <Step>.6
	virtual void OnRspLockInsert( KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);///鎖定應答
	virtual void OnRspQryLock( KS_::CThostFtdcLockField *pLock, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);///請求查詢鎖定回應	
	virtual void OnRspQryLockPosition( KS_::CThostFtdcLockPositionField *pLockPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);///請求查詢鎖定證券倉位回應
	virtual void OnRtnLock( KS_::CThostFtdcLockField *pLock);///鎖定通知
	virtual void OnErrRtnLockInsert( KS_::CThostFtdcInputLockField *pInputLock, KS_::CThostFtdcRspInfoField *pRspInfo);///鎖定錯誤通知	
	// ---- CKSOptionSpi ----
	// v2.3.50212 // 20150410 by Tim Lin // Step.6
	virtual void OnRspQryIndexPrice( KS_::CKSQryIndexPriceField *pQryIndexPrice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢股指現貨指數
	virtual void OnRspQryOptionInstrGuard( KS_::CKSQryOptionInstrGuardField *pQryOptionInstrGuard, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢期權合約保障系數
	virtual void OnRspQryExecOrderVolume( KS_::CKSQryExecOrderVolumeField *pQryExecOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢宣告數量請求
	virtual void OnRspQryStockOptionAssignment( KS_::CKSQryStockOptionAssignmentField *pQryStockOptionAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢個股行權指派信息
	virtual void OnRspQryInvestorTradeLevel( KS_::CKSQryInvestorTradeLevelField *pQryInvestorTradeLevel, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢客戶交易級別
	virtual void OnRspQryPurchaseLimitAmt( KS_::CKSQryPurchaseLimitAmtField *pQryPurchaseLimitAmt, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢個股限購額度
	virtual void OnRspQryPositionLimitVol( KS_::CKSQryPositionLimitVolField *pQryPositionLimitVol, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢個股限倉額度
	virtual void OnRspQryHistoryOrder( KS_::CKSQryHistoryOrderField *pQryHistoryOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢個股歷史報單
	virtual void OnRspQryHistoryTrade( KS_::CKSQryHistoryTradeField *pQryHistoryTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢個股歷史成交
	virtual void OnRspQryStockOptionHistoryAssignment( KS_::CKSQryHistoryAssignmentField *pQryHistoryAssignment, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢個股歷史行權指派明細
	virtual void OnRspQryStockOptionDelivDetail( KS_::CKSQrySODelivDetailField *pQrySODelivDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢個股行權交割明細
	virtual void OnRspAutoExecOrderAction( KS_::CKSAutoExecOrderActionField *pAutoExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///自動行權執行操作	
	// ---- CKSVocSpi ----
	// v2.3.50212 // 20150410 by Tim Lin // [Step].6
	virtual void OnRspQryInvestorOpenPosition( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢開盤前的持倉明細應答
	virtual void OnRspQryInvestorOpenCombinePosition( KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///查詢開盤前的組合持倉明細應答
	virtual void OnRspBulkCancelOrder( KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///批量報單撤除請求回應
	virtual void OnRspQryCloseStrategy( KS_::CKSCloseStrategyResultField *pCloseStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///平倉策略查詢回應
	virtual void OnRspQryCombStrategy( KS_::CKSCombStrategyResultField *pCombStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///組合策略查詢回應
	virtual void OnRspQryOptionCombStrategy( KS_::CKSOptionCombStrategyResultField *pOptionCombStrategy, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///期權組合策略查詢回應
	virtual void OnRspQryTransferInfo( KS_::CKSTransferInfoResultField *pResultField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢客戶轉帳資訊回應
	virtual void OnRspQryKSTradingNotice( KS_::CKSTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢交易通知回應
	virtual void OnRspQryUserProductUrl( KS_::CKSUserProductUrlField *pUserProductUrl, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///使用者端產品資源查詢應答
	virtual void OnRspQryMaxCombActionVolume( KS_::CKSMaxCombActionVolumeField *pMaxCombActionVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///最大組合拆分單量查詢請求回應
	virtual void OnRtnKSTradingNotice( KS_::CKSTradingNoticeField *pTradingNoticeInfo);///交易通知
	virtual void OnRspQryKSInstrumentMarginRate( KS_::CKSInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );///請求查詢合約保證金率回應
public:
	KStrdAPIHandler( KStrdHandle* kstrdobj ) : FhKStrd( kstrdobj ) { }
	~KStrdAPIHandler() { }
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
