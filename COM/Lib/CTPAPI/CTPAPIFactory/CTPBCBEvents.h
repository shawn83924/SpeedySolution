//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#ifndef CTPBCBEventsH
#define CTPBCBEventsH
#include "CTPAPIDynamic.h"
//-------------------------------------------------------------------------------------------------------------------------
// 1. Define all ReqXXX, RegOnXXX function pointor and OnXXX callback function pointor
//    in XXXAPIDynamic.h. (export from XXXAPIFactory.dll)
//
// 2. Add CPP unit named XXXBCBEvents.h/cpp
//    add a class XXXTradeApiInit.In class XXXTradeApiInit
//    has member:
//	  lpXXXX <--- keep function pointor.
//    lpRegOnXXXX <--- keep regist calback function function pointor.
//    static void OnXXXX() <--- Callback founctions to regist to DLL.
//	           				    when Callback founctions called, call Win32 API PostMessage
//	  		        		    to send Windows message to XXXTraderAPI object's window.
//    InitFunctions( ) <--- Bind all functions from DLL.( bind to lpXXXX)
//	  InitRegCallbackFunctions( ) <--- Bind all callback regist functions from DLL( bind to lpRegOnXXXX)

//
// 3. Add CPP unit named XXXBCBTraderAPIs.h/cpp
//    implement XXXTraderSpi <--- callback from XXXAPI
//    class XXXTraderAPI <--- call XXXAPI functions
//
//-------------------------------------------------------------------------------------------------------------------------
class CTPTraderApiInit
{
private:
	bool FSupport;  ///< Support CTP API ?
	char FNotSupportReason[ 128 ]; ///< Reason of load Femas API failed.
	int  FFuncRtn;  ///< Bind which function failed ?
	int  FRegCBRtn; ///< Bind which callback regist function failed ?
private:
	int InitFunctions( void );  ///< Bind all functions from DLL
	int InitRegCallbackFunctions( void ); ///< Bind all callback regist functions from DLL
public:
	CTPTraderApiInit( UFC::AnsiString APIType ); ///< APIType : "CTP" "HSctp" "KNctp"
	~CTPTraderApiInit();
	bool SupportCTPAPI( void ) { return FSupport;}
	const char* Reason( void ) { return FNotSupportReason; }
public: ///< All Functions
	lpCreateCTPTraderApi                           pCreateCTPTraderApi;
	lpReleaseCTPTraderApi                          pReleaseCTPTraderApi;
	lpInit                                         pInit;
	lpGetTradingDay                                pGetTradingDay;
	lpRegisterFront                                pRegisterFront;
	lpRegisterNameServer                           pRegisterNameServer;
	lpSubscribePrivateTopic                        pSubscribePrivateTopic;
	lpSubscribePublicTopic                         pSubscribePublicTopic;
	lpReqAuthenticate                              pReqAuthenticate;
	lpReqUserLogin                                 pReqUserLogin;
	lpReqUserLogout                                pReqUserLogout;
	lpReqUserPasswordUpdate                        pReqUserPasswordUpdate;
	lpReqTradingAccountPasswordUpdate              pReqTradingAccountPasswordUpdate;
	lpReqOrderInsert                               pReqOrderInsert;
	lpReqParkedOrderInsert                         pReqParkedOrderInsert;
	lpReqParkedOrderAction                         pReqParkedOrderAction;
	lpReqOrderAction                               pReqOrderAction;
	lpReqQueryMaxOrderVolume                       pReqQueryMaxOrderVolume;
	lpReqSettlementInfoConfirm                     pReqSettlementInfoConfirm;
	lpReqRemoveParkedOrder                         pReqRemoveParkedOrder;
	lpReqRemoveParkedOrderAction                   pReqRemoveParkedOrderAction;
	lpReqQryOrder                                  pReqQryOrder;
	lpReqQryTrade                                  pReqQryTrade;
	lpReqQryInvestorPosition                       pReqQryInvestorPosition;
	lpReqQryTradingAccount                         pReqQryTradingAccount;
	lpReqQryInvestor                               pReqQryInvestor;
	lpReqQryTradingCode                            pReqQryTradingCode;
	lpReqQryInstrumentMarginRate                   pReqQryInstrumentMarginRate;
	lpReqQryInstrumentCommissionRate               pReqQryInstrumentCommissionRate;
	lpReqQryExchange                               pReqQryExchange;
	lpReqQryInstrument                             pReqQryInstrument;
	lpReqQryDepthMarketData                        pReqQryDepthMarketData;
	lpReqQrySettlementInfo                         pReqQrySettlementInfo;
	lpReqQryTransferBank                           pReqQryTransferBank;
	lpReqQryInvestorPositionDetail                 pReqQryInvestorPositionDetail;
	lpReqQryNotice                                 pReqQryNotice;
	lpReqQrySettlementInfoConfirm                  pReqQrySettlementInfoConfirm;
	lpReqQryInvestorPositionCombineDetail          pReqQryInvestorPositionCombineDetail;
	lpReqQryCFMMCTradingAccountKey                 pReqQryCFMMCTradingAccountKey;
	lpReqQryEWarrantOffset                         pReqQryEWarrantOffset;
	lpReqQryTransferSerial                         pReqQryTransferSerial;
	lpReqQryAccountregister                        pReqQryAccountregister;
	lpReqQryContractBank                           pReqQryContractBank;
	lpReqQryParkedOrder                            pReqQryParkedOrder;
	lpReqQryParkedOrderAction                      pReqQryParkedOrderAction;
	lpReqQryTradingNotice                          pReqQryTradingNotice;
	lpReqQryBrokerTradingParams                    pReqQryBrokerTradingParams;
	lpReqQryBrokerTradingAlgos                     pReqQryBrokerTradingAlgos;
	lpReqFromBankToFutureByFuture                  pReqFromBankToFutureByFuture;
	lpReqFromFutureToBankByFuture                  pReqFromFutureToBankByFuture;
	lpReqQueryBankAccountMoneyByFuture             pReqQueryBankAccountMoneyByFuture;
public:///< All callback function register
	lpRegOnFrontConnected                           pRegOnFrontConnected;
	lpRegOnFrontDisconnected                        pRegOnFrontDisconnected;
	lpRegOnRspUserLogin                             pRegOnRspUserLogin;
	lpRegOnRspQrySettlementInfo                     pRegOnRspQrySettlementInfo;
	lpRegOnRspSettlementInfoConfirm                 pRegOnRspSettlementInfoConfirm;
	lpRegOnRspOrderInsert                           pRegOnRspOrderInsert;
	lpRegOnErrRtnOrderInsert                        pRegOnErrRtnOrderInsert;
	lpRegOnRspOrderAction                           pRegOnRspOrderAction;
	lpRegOnErrRtnOrderAction                        pRegOnErrRtnOrderAction;
	lpRegOnRspQryOrder                              pRegOnRspQryOrder;
	lpRegOnRspQryExchange                           pRegOnRspQryExchange;
	lpRegOnRspQryInstrument                         pRegOnRspQryInstrument;
	lpRegOnRspQryInvestorPosition                   pRegOnRspQryInvestorPosition;
	lpRegOnRtnOrder                                 pRegOnRtnOrder;
	lpRegOnRtnTrade                                 pRegOnRtnTrade;
	lpRegOnRspError                                 pRegOnRspError;
	lpRegOnHeartBeatWarning                         pRegOnHeartBeatWarning;
	lpRegOnRspAuthenticate                          pRegOnRspAuthenticate;
	lpRegOnRspUserLogout                            pRegOnRspUserLogout;
	lpRegOnRspUserPasswordUpdate                    pRegOnRspUserPasswordUpdate;
	lpRegOnRspTradingAccountPasswordUpdate          pRegOnRspTradingAccountPasswordUpdate;
	lpRegOnRspParkedOrderInsert                     pRegOnRspParkedOrderInsert;
	lpRegOnRspParkedOrderAction                     pRegOnRspParkedOrderAction;
	lpRegOnRspQueryMaxOrderVolume                   pRegOnRspQueryMaxOrderVolume;
	lpRegOnRspRemoveParkedOrder                     pRegOnRspRemoveParkedOrder;
	lpRegOnRspRemoveParkedOrderAction               pRegOnRspRemoveParkedOrderAction;
	lpRegOnRspQryTrade                              pRegOnRspQryTrade;
	lpRegOnRspQryTradingAccount                     pRegOnRspQryTradingAccount;
	lpRegOnRspQryInvestor                           pRegOnRspQryInvestor;
	lpRegOnRspQryTradingCode                        pRegOnRspQryTradingCode;
	lpRegOnRspQryInstrumentMarginRate               pRegOnRspQryInstrumentMarginRate;
	lpRegOnRspQryInstrumentCommissionRate           pRegOnRspQryInstrumentCommissionRate;
	lpRegOnRspQryDepthMarketData                    pRegOnRspQryDepthMarketData;
	lpRegOnRspQryTransferBank                       pRegOnRspQryTransferBank;
	lpRegOnRspQryInvestorPositionDetail             pRegOnRspQryInvestorPositionDetail;
	lpRegOnRspQryNotice                             pRegOnRspQryNotice;
	lpRegOnRspQrySettlementInfoConfirm              pRegOnRspQrySettlementInfoConfirm;
	lpRegOnRspQryInvestorPositionCombineDetail      pRegOnRspQryInvestorPositionCombineDetail;
	lpRegOnRspQryCFMMCTradingAccountKey             pRegOnRspQryCFMMCTradingAccountKey;
	lpRegOnRspQryEWarrantOffset                     pRegOnRspQryEWarrantOffset;
	lpRegOnRspQryTransferSerial                     pRegOnRspQryTransferSerial;
	lpRegOnRspQryAccountregister                    pRegOnRspQryAccountregister;
	lpRegOnRtnInstrumentStatus                      pRegOnRtnInstrumentStatus;
	lpRegOnRtnTradingNotice                         pRegOnRtnTradingNotice;
	lpRegOnRtnErrorConditionalOrder                 pRegOnRtnErrorConditionalOrder;
	lpRegOnRspQryContractBank                       pRegOnRspQryContractBank;
	lpRegOnRspQryParkedOrder                        pRegOnRspQryParkedOrder;
	lpRegOnRspQryParkedOrderAction                  pRegOnRspQryParkedOrderAction;
	lpRegOnRspQryTradingNotice                      pRegOnRspQryTradingNotice;
	lpRegOnRspQryBrokerTradingParams                pRegOnRspQryBrokerTradingParams;
	lpRegOnRspQryBrokerTradingAlgos                 pRegOnRspQryBrokerTradingAlgos;
	lpRegOnRtnFromBankToFutureByBank                pRegOnRtnFromBankToFutureByBank;
	lpRegOnRtnFromFutureToBankByBank                pRegOnRtnFromFutureToBankByBank;
	lpRegOnRtnRepealFromBankToFutureByBank          pRegOnRtnRepealFromBankToFutureByBank;
	lpRegOnRtnRepealFromFutureToBankByBank          pRegOnRtnRepealFromFutureToBankByBank;
	lpRegOnRtnFromBankToFutureByFuture              pRegOnRtnFromBankToFutureByFuture;
	lpRegOnRtnFromFutureToBankByFuture              pRegOnRtnFromFutureToBankByFuture;
	lpRegOnRtnRepealFromBankToFutureByFutureManual  pRegOnRtnRepealFromBankToFutureByFutureManual;
	lpRegOnRtnRepealFromFutureToBankByFutureManual  pRegOnRtnRepealFromFutureToBankByFutureManual;
	lpRegOnRtnQueryBankBalanceByFuture              pRegOnRtnQueryBankBalanceByFuture;
	lpRegOnErrRtnBankToFutureByFuture               pRegOnErrRtnBankToFutureByFuture;
	lpRegOnErrRtnFutureToBankByFuture               pRegOnErrRtnFutureToBankByFuture;
	lpRegOnErrRtnRepealBankToFutureByFutureManual   pRegOnErrRtnRepealBankToFutureByFutureManual;
	lpRegOnErrRtnRepealFutureToBankByFutureManual   pRegOnErrRtnRepealFutureToBankByFutureManual;
	lpRegOnErrRtnQueryBankBalanceByFuture           pRegOnErrRtnQueryBankBalanceByFuture;
	lpRegOnRtnRepealFromBankToFutureByFuture        pRegOnRtnRepealFromBankToFutureByFuture;
	lpRegOnRtnRepealFromFutureToBankByFuture        pRegOnRtnRepealFromFutureToBankByFuture;
	lpRegOnRspFromBankToFutureByFuture              pRegOnRspFromBankToFutureByFuture;
	lpRegOnRspFromFutureToBankByFuture              pRegOnRspFromFutureToBankByFuture;
	lpRegOnRspQueryBankAccountMoneyByFuture         pRegOnRspQueryBankAccountMoneyByFuture;
	lpRegOnRtnOpenAccountByBank                     pRegOnRtnOpenAccountByBank;
	lpRegOnRtnCancelAccountByBank                   pRegOnRtnCancelAccountByBank;
	lpRegOnRtnChangeAccountByBank                   pRegOnRtnChangeAccountByBank;
public:
	static void OnFrontConnected( void* Owner );
	static void OnFrontDisconnected( void* Owner, int nReason);
	static void OnRspUserLogin(void* Owner,CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspUserLogout( void* Owner, CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQrySettlementInfo(void* Owner,CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspSettlementInfoConfirm(void* Owner,CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspOrderInsert(void* Owner,CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnErrRtnOrderInsert(void* Owner,CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
	static void OnRspOrderAction(void* Owner,CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnErrRtnOrderAction(void* Owner,CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);
	static void OnRspQryOrder(void* Owner,CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryExchange(void* Owner,CThostFtdcExchangeField *pExchange, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryInstrument(void* Owner,CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryInvestorPosition(void* Owner,CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspError(void* Owner,CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRtnOrder(void* Owner,CThostFtdcOrderField *pOrder);
	static void OnRtnTrade(void* Owner,CThostFtdcTradeField *pTrade);
	static void OnHeartBeatWarning(void* Owner,int nTimeLapse);
	static void OnRspAuthenticate(void* Owner,CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspUserPasswordUpdate(void* Owner,CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspTradingAccountPasswordUpdate(void* Owner,CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspParkedOrderInsert(void* Owner,CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspParkedOrderAction(void* Owner,CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQueryMaxOrderVolume(void* Owner,CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspRemoveParkedOrder(void* Owner,CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspRemoveParkedOrderAction(void* Owner,CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryTrade(void* Owner,CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryTradingAccount(void* Owner,CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryInvestor(void* Owner,CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryTradingCode(void* Owner,CThostFtdcTradingCodeField *pTradingCode, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryInstrumentMarginRate(void* Owner,CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryInstrumentCommissionRate(void* Owner,CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryDepthMarketData(void* Owner,CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryTransferBank(void* Owner,CThostFtdcTransferBankField *pTransferBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryInvestorPositionDetail(void* Owner,CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryNotice(void* Owner,CThostFtdcNoticeField *pNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQrySettlementInfoConfirm(void* Owner,CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryInvestorPositionCombineDetail(void* Owner,CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryCFMMCTradingAccountKey(void* Owner,CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryEWarrantOffset(void* Owner,CThostFtdcEWarrantOffsetField *pEWarrantOffset, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryTransferSerial(void* Owner,CThostFtdcTransferSerialField *pTransferSerial, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryAccountregister(void* Owner,CThostFtdcAccountregisterField *pAccountregister, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRtnInstrumentStatus(void* Owner,CThostFtdcInstrumentStatusField *pInstrumentStatus);
	static void OnRtnTradingNotice(void* Owner,CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo);
	static void OnRtnErrorConditionalOrder(void* Owner,CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder);
	static void OnRspQryContractBank(void* Owner,CThostFtdcContractBankField *pContractBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryParkedOrder(void* Owner,CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryParkedOrderAction(void* Owner,CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryTradingNotice(void* Owner,CThostFtdcTradingNoticeField *pTradingNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryBrokerTradingParams(void* Owner,CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryBrokerTradingAlgos(void* Owner,CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRtnFromBankToFutureByBank(void* Owner,CThostFtdcRspTransferField *pRspTransfer);
	static void OnRtnFromFutureToBankByBank(void* Owner,CThostFtdcRspTransferField *pRspTransfer);
	static void OnRtnRepealFromBankToFutureByBank(void* Owner,CThostFtdcRspRepealField *pRspRepeal);
	static void OnRtnRepealFromFutureToBankByBank(void* Owner,CThostFtdcRspRepealField *pRspRepeal);
	static void OnRtnFromBankToFutureByFuture(void* Owner,CThostFtdcRspTransferField *pRspTransfer);
	static void OnRtnFromFutureToBankByFuture(void* Owner,CThostFtdcRspTransferField *pRspTransfer);
	static void OnRtnRepealFromBankToFutureByFutureManual(void* Owner,CThostFtdcRspRepealField *pRspRepeal);
	static void OnRtnRepealFromFutureToBankByFutureManual(void* Owner,CThostFtdcRspRepealField *pRspRepeal);
	static void OnRtnQueryBankBalanceByFuture(void* Owner,CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount);
	static void OnErrRtnBankToFutureByFuture(void* Owner,CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo);
	static void OnErrRtnFutureToBankByFuture(void* Owner,CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo);
	static void OnErrRtnRepealBankToFutureByFutureManual(void* Owner,CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo);
	static void OnErrRtnRepealFutureToBankByFutureManual(void* Owner,CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo);
	static void OnErrRtnQueryBankBalanceByFuture(void* Owner,CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo);
	static void OnRtnRepealFromBankToFutureByFuture(void* Owner,CThostFtdcRspRepealField *pRspRepeal);
	static void OnRtnRepealFromFutureToBankByFuture(void* Owner,CThostFtdcRspRepealField *pRspRepeal);
	static void OnRspFromBankToFutureByFuture(void* Owner,CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspFromFutureToBankByFuture(void* Owner,CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQueryBankAccountMoneyByFuture(void* Owner,CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRtnOpenAccountByBank(void* Owner,CThostFtdcOpenAccountField *pOpenAccount);
	static void OnRtnCancelAccountByBank(void* Owner,CThostFtdcCancelAccountField *pCancelAccount);
	static void OnRtnChangeAccountByBank(void* Owner,CThostFtdcChangeAccountField *pChangeAccount);
};
//-------------------------------------------------------------------------------------------------------------------------
extern CTPTraderApiInit gCTPAPI;
extern CTPTraderApiInit gHSctpAPI;
extern CTPTraderApiInit gKNctpAPI;
//-------------------------------------------------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
