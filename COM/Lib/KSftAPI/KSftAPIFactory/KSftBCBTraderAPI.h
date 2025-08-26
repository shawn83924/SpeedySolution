//------------------------------------------------------------------------------------------------------------------------
#ifdef _KSFT
//------------------------------------------------------------------------------------------------------------------------
#ifndef KSftBCBTraderAPIH
#define KSftBCBTraderAPIH
//------------------------------------------------------------------------------------------------------------------------
#include "KSTraderApiEx.h"
#include <windows.h>
//------------------------------------------------------------------------------------------------------------------------
#ifndef Namespace__KingstarAPI_
#define Namespace__KingstarAPI_
#define KS_ KingstarAPI
#endif
//------------------------------------------------------------------------------------------------------------------------
//
// KingStar KSFT API events listener interface
//
//------------------------------------------------------------------------------------------------------------------------
class TKSftTraderSpi
{
public:
	virtual void OnKSftAPIConnected() {};
	virtual void OnKSftAPIDisconnected( int nReason ) {};
	virtual void OnKSftAPIRecoverFinished( int Count ) {};
public:
	virtual void OnKSftFrontConnected( ) {};
	virtual void OnKSftFrontDisconnected( int nReason ) {};
	virtual void OnKSftHeartBeatWarning( int nTimeLapse ) {}; ///< no implement
	virtual void OnKSftRspAuthenticate( KS_::CThostFtdcRspAuthenticateField *pRspAuthenticateField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspUserLogin( KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspUserLogout( KS_::CThostFtdcUserLogoutField *pUserLogout, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspUserPasswordUpdate( KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspTradingAccountPasswordUpdate( KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspParkedOrderInsert( KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspParkedOrderAction( KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspOrderAction( KS_::CThostFtdcInputOrderActionField *pInputOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQueryMaxOrderVolume( KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspSettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspRemoveParkedOrder( KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspRemoveParkedOrderAction( KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspExecOrderInsert( KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspExecOrderAction( KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspForQuoteInsert( KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQuoteInsert( KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQuoteAction( KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryOrder( KS_::CThostFtdcOrderField *pOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryTrade( KS_::CThostFtdcTradeField *pTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryInvestorPosition( KS_::CThostFtdcInvestorPositionField *pInvestorPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspQryTradingAccount( KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspQryInvestor( KS_::CThostFtdcInvestorField *pInvestor, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryTradingCode( KS_::CThostFtdcTradingCodeField *pTradingCode, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspQryInstrumentMarginRate( KS_::CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryInstrumentCommissionRate( KS_::CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspQryExchange( KS_::CThostFtdcExchangeField *pExchange, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryProduct( KS_::CThostFtdcProductField *pProduct, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryInstrument( KS_::CThostFtdcInstrumentField *pInstrument, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryDepthMarketData( KS_::CThostFtdcDepthMarketDataField *pDepthMarketData, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQrySettlementInfo( KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspQryTransferBank( KS_::CThostFtdcTransferBankField *pTransferBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspQryInvestorPositionDetail( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspQryNotice( KS_::CThostFtdcNoticeField *pNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQrySettlementInfoConfirm( KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspQryInvestorPositionCombineDetail( KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryCFMMCTradingAccountKey( KS_::CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryEWarrantOffset( KS_::CThostFtdcEWarrantOffsetField *pEWarrantOffset, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryInvestorProductGroupMargin( KS_::CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryExchangeMarginRate( KS_::CThostFtdcExchangeMarginRateField *pExchangeMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryExchangeMarginRateAdjust( KS_::CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryExchangeRate( KS_::CThostFtdcExchangeRateField *pExchangeRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQrySecAgentACIDMap( KS_::CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryOptionInstrTradeCost( KS_::CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryOptionInstrCommRate( KS_::CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryExecOrder( KS_::CThostFtdcExecOrderField *pExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryForQuote( KS_::CThostFtdcForQuoteField *pForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryQuote( KS_::CThostFtdcQuoteField *pQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryTransferSerial( KS_::CThostFtdcTransferSerialField *pTransferSerial, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspQryAccountregister( KS_::CThostFtdcAccountregisterField *pAccountregister, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspError( KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRtnOrder( KS_::CThostFtdcOrderField *pOrder ) {};
	virtual void OnKSftRtnTrade( KS_::CThostFtdcTradeField *pTrade ) {};
	virtual void OnKSftErrRtnOrderInsert( KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo ) {};
	virtual void OnKSftErrRtnOrderAction( KS_::CThostFtdcOrderActionField *pOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo ) {};
	virtual void OnKSftRtnInstrumentStatus( KS_::CThostFtdcInstrumentStatusField *pInstrumentStatus ) {};
	virtual void OnKSftRtnTradingNotice( KS_::CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo ) {}; ///< no implement
	virtual void OnKSftRtnErrorConditionalOrder( KS_::CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder ) {}; ///< no implement
	virtual void OnKSftRtnExecOrder( KS_::CThostFtdcExecOrderField *pExecOrder ) {}; ///< no implement
	virtual void OnKSftErrRtnExecOrderInsert( KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKSftErrRtnExecOrderAction( KS_::CThostFtdcExecOrderActionField *pExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKSftErrRtnForQuoteInsert( KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKSftRtnQuote( KS_::CThostFtdcQuoteField *pQuote ) {}; ///< no implement
	virtual void OnKSftErrRtnQuoteInsert( KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKSftErrRtnQuoteAction( KS_::CThostFtdcQuoteActionField *pQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKSftRspQryContractBank( KS_::CThostFtdcContractBankField *pContractBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspQryParkedOrder( KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryParkedOrderAction( KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryTradingNotice( KS_::CThostFtdcTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryBrokerTradingParams( KS_::CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspQryBrokerTradingAlgos( KS_::CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRtnFromBankToFutureByBank( KS_::CThostFtdcRspTransferField *pRspTransfer ) {}; ///< no implement
	virtual void OnKSftRtnFromFutureToBankByBank( KS_::CThostFtdcRspTransferField *pRspTransfer ) {}; ///< no implement
	virtual void OnKSftRtnRepealFromBankToFutureByBank( KS_::CThostFtdcRspRepealField *pRspRepeal ) {}; ///< no implement
	virtual void OnKSftRtnRepealFromFutureToBankByBank( KS_::CThostFtdcRspRepealField *pRspRepeal ) {}; ///< no implement
	virtual void OnKSftRtnFromBankToFutureByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer ) {};
	virtual void OnKSftRtnFromFutureToBankByFuture( KS_::CThostFtdcRspTransferField *pRspTransfer ) {};
	virtual void OnKSftRtnRepealFromBankToFutureByFutureManual( KS_::CThostFtdcRspRepealField *pRspRepeal ) {}; ///< no implement
	virtual void OnKSftRtnRepealFromFutureToBankByFutureManual( KS_::CThostFtdcRspRepealField *pRspRepeal ) {}; ///< no implement
	virtual void OnKSftRtnQueryBankBalanceByFuture( KS_::CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount ) {};
	virtual void OnKSftErrRtnBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKSftErrRtnFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKSftErrRtnRepealBankToFutureByFutureManual( KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKSftErrRtnRepealFutureToBankByFutureManual( KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKSftErrRtnQueryBankBalanceByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo ) {}; ///< no implement
	virtual void OnKSftRtnRepealFromBankToFutureByFuture( KS_::CThostFtdcRspRepealField *pRspRepeal ) {}; ///< no implement
	virtual void OnKSftRtnRepealFromFutureToBankByFuture( KS_::CThostFtdcRspRepealField *pRspRepeal ) {}; ///< no implement
	virtual void OnKSftRspFromBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspFromFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspQueryBankAccountMoneyByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRtnOpenAccountByBank( KS_::CThostFtdcOpenAccountField *pOpenAccount ) {}; ///< no implement
	virtual void OnKSftRtnCancelAccountByBank( KS_::CThostFtdcCancelAccountField *pCancelAccount ) {}; ///< no implement
	virtual void OnKSftRtnChangeAccountByBank( KS_::CThostFtdcChangeAccountField *pChangeAccount ) {}; ///< no implement
	virtual void OnKSftRspQryInvestorOpenPosition( KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {};
	virtual void OnKSftRspQryInvestorOpenCombinePosition( KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
	virtual void OnKSftRspBulkCancelOrder( KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast ) {}; ///< no implement
};
//------------------------------------------------------------------------------------------------------------------------
//
// KingStar KSFT Trader API class
//
//------------------------------------------------------------------------------------------------------------------------
class TKSftTraderApi
{
private:
	void*				FKSFTAPI;
	TKSftTraderSpi*		FListener;
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
	static bool SupportKSftAPI( void );
	static const char* LoadKSftAPIResult( void );
public:
	TKSftTraderSpi*		GetSpi( void )	{ return FListener; }
	HWND				GetHWND( void )	{ return FHwnd; }
public:
	void RegisterSpi( TKSftTraderSpi *pSpi );
	void ReqOnRecoverFilisned( int Count );
public:
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
	int ReqFromBankToFutureByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID );
	int ReqFromFutureToBankByFuture( KS_::CThostFtdcReqTransferField *pReqTransfer, int nRequestID );
	int ReqQueryBankAccountMoneyByFuture( KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, int nRequestID );
	int ReqQueryInvestorOpenPosition( KS_::CThostFtdcQryInvestorPositionDetailField *pQryInvestorOpenPosition, int nRequestID );
	int ReqQueryInvestorOpenCombinePosition( KS_::CThostFtdcQryInvestorPositionCombineDetailField *pQryInvestorOpenCombinePosition, int nRequestID );
	void * LoadExtApi( void * spi, const char *ExtApiName = "KSCosApi" );
	int ReqBulkCancelOrder ( KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, int nRequestID );
public:
	TKSftTraderApi( TKSftTraderSpi* pSpi );
	~TKSftTraderApi();
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------

