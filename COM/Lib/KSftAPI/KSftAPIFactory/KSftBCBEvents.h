//------------------------------------------------------------------------------------------------------------------------
#ifdef _KSFT
//------------------------------------------------------------------------------------------------------------------------
#ifndef KSftBCBEventsH
#define KSftBCBEventsH
//------------------------------------------------------------------------------------------------------------------------
#include "KSftAPIDynamic.h"
#include "Pthread.h"
//------------------------------------------------------------------------------------------------------------------------
//
// Class to initialize KingStar KSFT API
//
//------------------------------------------------------------------------------------------------------------------------
class KSftTraderApiInit
{
private:
	bool	FSupport;  ///< Support KSFT API ?
	char	FNotSupportReason[ 128 ]; ///< Reason of load KSFT API failed.
	int		FFuncRtn;  ///< Bind which function failed ?
	int		FRegCBRtn; ///< Bind which callback regist function failed ?
private:
	int		InitFunctions( void ); ///< Bind all functions from DLL
	int		InitRegCallbackFunctions( void ); ///< Bind all callback regist functions from DLL
public:
	bool	SupportKSftAPI( void )	{ return FSupport; }
	const char* Reason( void )		{ return FNotSupportReason; }
public: ///< All Functions
	lpCreateKSftTraderApi					pCreateKSftTraderApi;
	lpReleaseKSftTraderApi					pReleaseKSftTraderApi;
	lpInit									pInit;
	lpGetTradingDay							pGetTradingDay;
	lpRegisterFront							pRegisterFront;
	lpRegisterNameServer					pRegisterNameServer;
	lpRegisterFensUserInfo					pRegisterFensUserInfo;
	lpSubscribePrivateTopic					pSubscribePrivateTopic;
	lpSubscribePublicTopic					pSubscribePublicTopic;
	lpReqAuthenticate						pReqAuthenticate;
	lpReqUserLogin							pReqUserLogin;
	lpReqUserLogout							pReqUserLogout;
	lpReqUserPasswordUpdate					pReqUserPasswordUpdate;
	lpReqTradingAccountPasswordUpdate		pReqTradingAccountPasswordUpdate;
	lpReqOrderInsert						pReqOrderInsert;
	lpReqParkedOrderInsert					pReqParkedOrderInsert;
	lpReqParkedOrderAction					pReqParkedOrderAction;
	lpReqOrderAction						pReqOrderAction;
	lpReqQueryMaxOrderVolume				pReqQueryMaxOrderVolume;
	lpReqSettlementInfoConfirm				pReqSettlementInfoConfirm;
	lpReqRemoveParkedOrder					pReqRemoveParkedOrder;
	lpReqRemoveParkedOrderAction			pReqRemoveParkedOrderAction;
	lpReqExecOrderInsert					pReqExecOrderInsert;
	lpReqExecOrderAction					pReqExecOrderAction;
	lpReqForQuoteInsert						pReqForQuoteInsert;
	lpReqQuoteInsert						pReqQuoteInsert;
	lpReqQuoteAction						pReqQuoteAction;
	lpReqQryOrder							pReqQryOrder;
	lpReqQryTrade							pReqQryTrade;
	lpReqQryInvestorPosition				pReqQryInvestorPosition;
	lpReqQryTradingAccount					pReqQryTradingAccount;
	lpReqQryInvestor						pReqQryInvestor;
	lpReqQryTradingCode						pReqQryTradingCode;
	lpReqQryInstrumentMarginRate			pReqQryInstrumentMarginRate;
	lpReqQryInstrumentCommissionRate		pReqQryInstrumentCommissionRate;
	lpReqQryExchange						pReqQryExchange;
	lpReqQryProduct							pReqQryProduct;
	lpReqQryInstrument						pReqQryInstrument;
	lpReqQryDepthMarketData					pReqQryDepthMarketData;
	lpReqQrySettlementInfo					pReqQrySettlementInfo;
	lpReqQryTransferBank					pReqQryTransferBank;
	lpReqQryInvestorPositionDetail			pReqQryInvestorPositionDetail;
	lpReqQryNotice							pReqQryNotice;
	lpReqQrySettlementInfoConfirm			pReqQrySettlementInfoConfirm;
	lpReqQryInvestorPositionCombineDetail	pReqQryInvestorPositionCombineDetail;
	lpReqQryCFMMCTradingAccountKey			pReqQryCFMMCTradingAccountKey;
	lpReqQryEWarrantOffset					pReqQryEWarrantOffset;
	lpReqQryInvestorProductGroupMargin		pReqQryInvestorProductGroupMargin;
	lpReqQryExchangeMarginRate				pReqQryExchangeMarginRate;
	lpReqQryExchangeMarginRateAdjust		pReqQryExchangeMarginRateAdjust;
	lpReqQryExchangeRate					pReqQryExchangeRate;
	lpReqQrySecAgentACIDMap					pReqQrySecAgentACIDMap;
	lpReqQryOptionInstrTradeCost			pReqQryOptionInstrTradeCost;
	lpReqQryOptionInstrCommRate				pReqQryOptionInstrCommRate;
	lpReqQryExecOrder						pReqQryExecOrder;
	lpReqQryForQuote						pReqQryForQuote;
	lpReqQryQuote							pReqQryQuote;
	lpReqQryTransferSerial					pReqQryTransferSerial;
	lpReqQryAccountregister					pReqQryAccountregister;
	lpReqQryContractBank					pReqQryContractBank;
	lpReqQryParkedOrder						pReqQryParkedOrder;
	lpReqQryParkedOrderAction				pReqQryParkedOrderAction;
	lpReqQryTradingNotice					pReqQryTradingNotice;
	lpReqQryBrokerTradingParams				pReqQryBrokerTradingParams;
	lpReqQryBrokerTradingAlgos				pReqQryBrokerTradingAlgos;
	lpReqFromBankToFutureByFuture			pReqFromBankToFutureByFuture;
	lpReqFromFutureToBankByFuture			pReqFromFutureToBankByFuture;
	lpReqQueryBankAccountMoneyByFuture		pReqQueryBankAccountMoneyByFuture;
	lpReqQueryInvestorOpenPosition			pReqQueryInvestorOpenPosition;
	lpReqQueryInvestorOpenCombinePosition	pReqQueryInvestorOpenCombinePosition;
	lpLoadExtApi							pLoadExtApi;
	lpReqBulkCancelOrder					pReqBulkCancelOrder;
public: ///< All callback regist functions.
	lpRegOnFrontConnected							pRegOnFrontConnected;
	lpRegOnFrontDisconnected						pRegOnFrontDisconnected;
	lpRegOnHeartBeatWarning							pRegOnHeartBeatWarning;
	lpRegOnRspAuthenticate							pRegOnRspAuthenticate;
	lpRegOnRspUserLogin								pRegOnRspUserLogin;
	lpRegOnRspUserLogout							pRegOnRspUserLogout;
	lpRegOnRspUserPasswordUpdate					pRegOnRspUserPasswordUpdate;
	lpRegOnRspTradingAccountPasswordUpdate			pRegOnRspTradingAccountPasswordUpdate;
	lpRegOnRspOrderInsert							pRegOnRspOrderInsert;
	lpRegOnRspParkedOrderInsert						pRegOnRspParkedOrderInsert;
	lpRegOnRspParkedOrderAction						pRegOnRspParkedOrderAction;
	lpRegOnRspOrderAction							pRegOnRspOrderAction;
	lpRegOnRspQueryMaxOrderVolume					pRegOnRspQueryMaxOrderVolume;
	lpRegOnRspSettlementInfoConfirm					pRegOnRspSettlementInfoConfirm;
	lpRegOnRspRemoveParkedOrder						pRegOnRspRemoveParkedOrder;
	lpRegOnRspRemoveParkedOrderAction				pRegOnRspRemoveParkedOrderAction;
	lpRegOnRspExecOrderInsert						pRegOnRspExecOrderInsert;
	lpRegOnRspExecOrderAction						pRegOnRspExecOrderAction;
	lpRegOnRspForQuoteInsert						pRegOnRspForQuoteInsert;
	lpRegOnRspQuoteInsert							pRegOnRspQuoteInsert;
	lpRegOnRspQuoteAction							pRegOnRspQuoteAction;
	lpRegOnRspQryOrder								pRegOnRspQryOrder;
	lpRegOnRspQryTrade								pRegOnRspQryTrade;
	lpRegOnRspQryInvestorPosition					pRegOnRspQryInvestorPosition;
	lpRegOnRspQryTradingAccount						pRegOnRspQryTradingAccount;
	lpRegOnRspQryInvestor							pRegOnRspQryInvestor;
	lpRegOnRspQryTradingCode						pRegOnRspQryTradingCode;
	lpRegOnRspQryInstrumentMarginRate				pRegOnRspQryInstrumentMarginRate;
	lpRegOnRspQryInstrumentCommissionRate			pRegOnRspQryInstrumentCommissionRate;
	lpRegOnRspQryExchange							pRegOnRspQryExchange;
	lpRegOnRspQryProduct							pRegOnRspQryProduct;
	lpRegOnRspQryInstrument							pRegOnRspQryInstrument;
	lpRegOnRspQryDepthMarketData					pRegOnRspQryDepthMarketData;
	lpRegOnRspQrySettlementInfo						pRegOnRspQrySettlementInfo;
	lpRegOnRspQryTransferBank						pRegOnRspQryTransferBank;
	lpRegOnRspQryInvestorPositionDetail				pRegOnRspQryInvestorPositionDetail;
	lpRegOnRspQryNotice								pRegOnRspQryNotice;
	lpRegOnRspQrySettlementInfoConfirm				pRegOnRspQrySettlementInfoConfirm;
	lpRegOnRspQryInvestorPositionCombineDetail		pRegOnRspQryInvestorPositionCombineDetail;
	lpRegOnRspQryCFMMCTradingAccountKey				pRegOnRspQryCFMMCTradingAccountKey;
	lpRegOnRspQryEWarrantOffset						pRegOnRspQryEWarrantOffset;
	lpRegOnRspQryInvestorProductGroupMargin			pRegOnRspQryInvestorProductGroupMargin;
	lpRegOnRspQryExchangeMarginRate					pRegOnRspQryExchangeMarginRate;
	lpRegOnRspQryExchangeMarginRateAdjust			pRegOnRspQryExchangeMarginRateAdjust;
	lpRegOnRspQryExchangeRate						pRegOnRspQryExchangeRate;
	lpRegOnRspQrySecAgentACIDMap					pRegOnRspQrySecAgentACIDMap;
	lpRegOnRspQryOptionInstrTradeCost				pRegOnRspQryOptionInstrTradeCost;
	lpRegOnRspQryOptionInstrCommRate				pRegOnRspQryOptionInstrCommRate;
	lpRegOnRspQryExecOrder							pRegOnRspQryExecOrder;
	lpRegOnRspQryForQuote							pRegOnRspQryForQuote;
	lpRegOnRspQryQuote								pRegOnRspQryQuote;
	lpRegOnRspQryTransferSerial						pRegOnRspQryTransferSerial;
	lpRegOnRspQryAccountregister					pRegOnRspQryAccountregister;
	lpRegOnRspError									pRegOnRspError;
	lpRegOnRtnOrder									pRegOnRtnOrder;
	lpRegOnRtnTrade									pRegOnRtnTrade;
	lpRegOnErrRtnOrderInsert						pRegOnErrRtnOrderInsert;
	lpRegOnErrRtnOrderAction						pRegOnErrRtnOrderAction;
	lpRegOnRtnInstrumentStatus						pRegOnRtnInstrumentStatus;
	lpRegOnRtnTradingNotice							pRegOnRtnTradingNotice;
	lpRegOnRtnErrorConditionalOrder					pRegOnRtnErrorConditionalOrder;
	lpRegOnRtnExecOrder								pRegOnRtnExecOrder;
	lpRegOnErrRtnExecOrderInsert					pRegOnErrRtnExecOrderInsert;
	lpRegOnErrRtnExecOrderAction					pRegOnErrRtnExecOrderAction;
	lpRegOnErrRtnForQuoteInsert						pRegOnErrRtnForQuoteInsert;
	lpRegOnRtnQuote									pRegOnRtnQuote;
	lpRegOnErrRtnQuoteInsert						pRegOnErrRtnQuoteInsert;
	lpRegOnErrRtnQuoteAction						pRegOnErrRtnQuoteAction;
	lpRegOnRspQryContractBank						pRegOnRspQryContractBank;
	lpRegOnRspQryParkedOrder						pRegOnRspQryParkedOrder;
	lpRegOnRspQryParkedOrderAction					pRegOnRspQryParkedOrderAction;
	lpRegOnRspQryTradingNotice						pRegOnRspQryTradingNotice;
	lpRegOnRspQryBrokerTradingParams				pRegOnRspQryBrokerTradingParams;
	lpRegOnRspQryBrokerTradingAlgos					pRegOnRspQryBrokerTradingAlgos;
	lpRegOnRtnFromBankToFutureByBank				pRegOnRtnFromBankToFutureByBank;
	lpRegOnRtnFromFutureToBankByBank				pRegOnRtnFromFutureToBankByBank;
	lpRegOnRtnRepealFromBankToFutureByBank			pRegOnRtnRepealFromBankToFutureByBank;
	lpRegOnRtnRepealFromFutureToBankByBank			pRegOnRtnRepealFromFutureToBankByBank;
	lpRegOnRtnFromBankToFutureByFuture				pRegOnRtnFromBankToFutureByFuture;
	lpRegOnRtnFromFutureToBankByFuture				pRegOnRtnFromFutureToBankByFuture;
	lpRegOnRtnRepealFromBankToFutureByFutureManual	pRegOnRtnRepealFromBankToFutureByFutureManual;
	lpRegOnRtnRepealFromFutureToBankByFutureManual	pRegOnRtnRepealFromFutureToBankByFutureManual;
	lpRegOnRtnQueryBankBalanceByFuture				pRegOnRtnQueryBankBalanceByFuture;
	lpRegOnErrRtnBankToFutureByFuture				pRegOnErrRtnBankToFutureByFuture;
	lpRegOnErrRtnFutureToBankByFuture				pRegOnErrRtnFutureToBankByFuture;
	lpRegOnErrRtnRepealBankToFutureByFutureManual	pRegOnErrRtnRepealBankToFutureByFutureManual;
	lpRegOnErrRtnRepealFutureToBankByFutureManual	pRegOnErrRtnRepealFutureToBankByFutureManual;
	lpRegOnErrRtnQueryBankBalanceByFuture			pRegOnErrRtnQueryBankBalanceByFuture;
	lpRegOnRtnRepealFromBankToFutureByFuture		pRegOnRtnRepealFromBankToFutureByFuture;
	lpRegOnRtnRepealFromFutureToBankByFuture		pRegOnRtnRepealFromFutureToBankByFuture;
	lpRegOnRspFromBankToFutureByFuture				pRegOnRspFromBankToFutureByFuture;
	lpRegOnRspFromFutureToBankByFuture				pRegOnRspFromFutureToBankByFuture;
	lpRegOnRspQueryBankAccountMoneyByFuture			pRegOnRspQueryBankAccountMoneyByFuture;
	lpRegOnRtnOpenAccountByBank						pRegOnRtnOpenAccountByBank;
	lpRegOnRtnCancelAccountByBank					pRegOnRtnCancelAccountByBank;
	lpRegOnRtnChangeAccountByBank					pRegOnRtnChangeAccountByBank;
	lpRegOnRspQryInvestorOpenPosition				pRegOnRspQryInvestorOpenPosition;
	lpRegOnRspQryInvestorOpenCombinePosition		pRegOnRspQryInvestorOpenCombinePosition;
	lpRegOnRspBulkCancelOrder						pRegOnRspBulkCancelOrder;
public:
	static void OnFrontConnected( HKSFT Owner );
	static void OnFrontDisconnected( HKSFT Owner, int nReason );
	static void OnHeartBeatWarning( HKSFT Owner, int nTimeLapse );
	static void OnRspAuthenticate( HKSFT Owner, KS_::CThostFtdcRspAuthenticateField *pRspAuthenticateField, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspUserLogin( HKSFT Owner, KS_::CThostFtdcRspUserLoginField *pRspUserLogin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspUserLogout( HKSFT Owner, KS_::CThostFtdcUserLogoutField *pUserLogout, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspUserPasswordUpdate( HKSFT Owner, KS_::CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspTradingAccountPasswordUpdate( HKSFT Owner, KS_::CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspOrderInsert( HKSFT Owner, KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspParkedOrderInsert( HKSFT Owner, KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspParkedOrderAction( HKSFT Owner, KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspOrderAction( HKSFT Owner, KS_::CThostFtdcInputOrderActionField *pInputOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQueryMaxOrderVolume( HKSFT Owner, KS_::CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspSettlementInfoConfirm( HKSFT Owner, KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspRemoveParkedOrder( HKSFT Owner, KS_::CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspRemoveParkedOrderAction( HKSFT Owner, KS_::CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspExecOrderInsert( HKSFT Owner, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspExecOrderAction( HKSFT Owner, KS_::CThostFtdcInputExecOrderActionField *pInputExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspForQuoteInsert( HKSFT Owner, KS_::CThostFtdcInputForQuoteField *pInputForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQuoteInsert( HKSFT Owner, KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQuoteAction( HKSFT Owner, KS_::CThostFtdcInputQuoteActionField *pInputQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryOrder( HKSFT Owner, KS_::CThostFtdcOrderField *pOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryTrade( HKSFT Owner, KS_::CThostFtdcTradeField *pTrade, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInvestorPosition( HKSFT Owner, KS_::CThostFtdcInvestorPositionField *pInvestorPosition, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryTradingAccount( HKSFT Owner, KS_::CThostFtdcTradingAccountField *pTradingAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInvestor( HKSFT Owner, KS_::CThostFtdcInvestorField *pInvestor, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryTradingCode( HKSFT Owner, KS_::CThostFtdcTradingCodeField *pTradingCode, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInstrumentMarginRate( HKSFT Owner, KS_::CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInstrumentCommissionRate( HKSFT Owner, KS_::CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryExchange( HKSFT Owner, KS_::CThostFtdcExchangeField *pExchange, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryProduct( HKSFT Owner, KS_::CThostFtdcProductField *pProduct, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInstrument( HKSFT Owner, KS_::CThostFtdcInstrumentField *pInstrument, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryDepthMarketData( HKSFT Owner, KS_::CThostFtdcDepthMarketDataField *pDepthMarketData, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQrySettlementInfo( HKSFT Owner, KS_::CThostFtdcSettlementInfoField *pSettlementInfo, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryTransferBank( HKSFT Owner, KS_::CThostFtdcTransferBankField *pTransferBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInvestorPositionDetail( HKSFT Owner, KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryNotice( HKSFT Owner, KS_::CThostFtdcNoticeField *pNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQrySettlementInfoConfirm( HKSFT Owner, KS_::CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInvestorPositionCombineDetail( HKSFT Owner, KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryCFMMCTradingAccountKey( HKSFT Owner, KS_::CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryEWarrantOffset( HKSFT Owner, KS_::CThostFtdcEWarrantOffsetField *pEWarrantOffset, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInvestorProductGroupMargin( HKSFT Owner, KS_::CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryExchangeMarginRate( HKSFT Owner, KS_::CThostFtdcExchangeMarginRateField *pExchangeMarginRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryExchangeMarginRateAdjust( HKSFT Owner, KS_::CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryExchangeRate( HKSFT Owner, KS_::CThostFtdcExchangeRateField *pExchangeRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQrySecAgentACIDMap( HKSFT Owner, KS_::CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryOptionInstrTradeCost( HKSFT Owner, KS_::CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryOptionInstrCommRate( HKSFT Owner, KS_::CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryExecOrder( HKSFT Owner, KS_::CThostFtdcExecOrderField *pExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryForQuote( HKSFT Owner, KS_::CThostFtdcForQuoteField *pForQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryQuote( HKSFT Owner, KS_::CThostFtdcQuoteField *pQuote, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryTransferSerial( HKSFT Owner, KS_::CThostFtdcTransferSerialField *pTransferSerial, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryAccountregister( HKSFT Owner, KS_::CThostFtdcAccountregisterField *pAccountregister, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspError( HKSFT Owner, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRtnOrder( HKSFT Owner, KS_::CThostFtdcOrderField *pOrder );
	static void OnRtnTrade( HKSFT Owner, KS_::CThostFtdcTradeField *pTrade );
	static void OnErrRtnOrderInsert( HKSFT Owner, KS_::CThostFtdcInputOrderField *pInputOrder, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnOrderAction( HKSFT Owner, KS_::CThostFtdcOrderActionField *pOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnRtnInstrumentStatus( HKSFT Owner, KS_::CThostFtdcInstrumentStatusField *pInstrumentStatus );
	static void OnRtnTradingNotice( HKSFT Owner, KS_::CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo );
	static void OnRtnErrorConditionalOrder( HKSFT Owner, KS_::CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder );
	static void OnRtnExecOrder( HKSFT Owner, KS_::CThostFtdcExecOrderField *pExecOrder );
	static void OnErrRtnExecOrderInsert( HKSFT Owner, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnExecOrderAction( HKSFT Owner, KS_::CThostFtdcExecOrderActionField *pExecOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnForQuoteInsert( HKSFT Owner, KS_::CThostFtdcInputExecOrderField *pInputExecOrder, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnRtnQuote( HKSFT Owner, KS_::CThostFtdcQuoteField *pQuote );
	static void OnErrRtnQuoteInsert( HKSFT Owner, KS_::CThostFtdcInputQuoteField *pInputQuote, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnQuoteAction( HKSFT Owner, KS_::CThostFtdcQuoteActionField *pQuoteAction, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnRspQryContractBank( HKSFT Owner, KS_::CThostFtdcContractBankField *pContractBank, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryParkedOrder( HKSFT Owner, KS_::CThostFtdcParkedOrderField *pParkedOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryParkedOrderAction( HKSFT Owner, KS_::CThostFtdcParkedOrderActionField *pParkedOrderAction, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryTradingNotice( HKSFT Owner, KS_::CThostFtdcTradingNoticeField *pTradingNotice, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryBrokerTradingParams( HKSFT Owner, KS_::CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryBrokerTradingAlgos( HKSFT Owner, KS_::CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRtnFromBankToFutureByBank( HKSFT Owner, KS_::CThostFtdcRspTransferField *pRspTransfer );
	static void OnRtnFromFutureToBankByBank( HKSFT Owner, KS_::CThostFtdcRspTransferField *pRspTransfer );
	static void OnRtnRepealFromBankToFutureByBank( HKSFT Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
	static void OnRtnRepealFromFutureToBankByBank( HKSFT Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
	static void OnRtnFromBankToFutureByFuture( HKSFT Owner, KS_::CThostFtdcRspTransferField *pRspTransfer );
	static void OnRtnFromFutureToBankByFuture( HKSFT Owner, KS_::CThostFtdcRspTransferField *pRspTransfer );
	static void OnRtnRepealFromBankToFutureByFutureManual( HKSFT Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
	static void OnRtnRepealFromFutureToBankByFutureManual( HKSFT Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
	static void OnRtnQueryBankBalanceByFuture( HKSFT Owner, KS_::CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount );
	static void OnErrRtnBankToFutureByFuture( HKSFT Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnFutureToBankByFuture( HKSFT Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnRepealBankToFutureByFutureManual( HKSFT Owner, KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnRepealFutureToBankByFutureManual( HKSFT Owner, KS_::CThostFtdcReqRepealField *pReqRepeal, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnErrRtnQueryBankBalanceByFuture( HKSFT Owner, KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo );
	static void OnRtnRepealFromBankToFutureByFuture( HKSFT Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
	static void OnRtnRepealFromFutureToBankByFuture( HKSFT Owner, KS_::CThostFtdcRspRepealField *pRspRepeal );
	static void OnRspFromBankToFutureByFuture( HKSFT Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspFromFutureToBankByFuture( HKSFT Owner, KS_::CThostFtdcReqTransferField *pReqTransfer, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQueryBankAccountMoneyByFuture( HKSFT Owner, KS_::CThostFtdcReqQueryAccountField *pReqQueryAccount, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRtnOpenAccountByBank( HKSFT Owner, KS_::CThostFtdcOpenAccountField *pOpenAccount );
	static void OnRtnCancelAccountByBank( HKSFT Owner, KS_::CThostFtdcCancelAccountField *pCancelAccount );
	static void OnRtnChangeAccountByBank( HKSFT Owner, KS_::CThostFtdcChangeAccountField *pChangeAccount );
	static void OnRspQryInvestorOpenPosition( HKSFT Owner, KS_::CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspQryInvestorOpenCombinePosition( HKSFT Owner, KS_::CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
	static void OnRspBulkCancelOrder( HKSFT Owner, KS_::CThostFtdcBulkCancelOrderField *pBulkCancelOrder, KS_::CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast );
public:
	KSftTraderApiInit();
	~KSftTraderApiInit();
};
//------------------------------------------------------------------------------------------------------------------------
extern KSftTraderApiInit gKSftAPI;
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------

