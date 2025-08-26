//---------------------------------------------------------------------------
#ifndef USTPBCBEventsH
#define USTPBCBEventsH
#include "FemasAPIDynamic.h"
#include "PThread.h"
//-------------------------------------------------------------------------------------------------------------------------
//
//  Class to initialize Femas API
//
//-------------------------------------------------------------------------------------------------------------------------
class USTPTraderApiInit
{
private:
	bool FSupport;  ///< Support Femas API ?
	char FNotSupportReason[ 128 ]; ///< Reason of load Femas API failed.
	int  FFuncRtn;  ///< Bind which function failed ?
	int  FRegCBRtn; ///< Bind which callback regist function failed ?
	UFC::PEvent FEvent;
private:
	int  InitFunctions( void ); ///< Bind all functions from DLL
	int  InitRegCallbackFunctions( void );	///< Bind all callback regist functions from DLL
public:
	USTPTraderApiInit();
	~USTPTraderApiInit();
	bool SupportUSTPAPI( void ) { return FSupport;}
	const char* Reason( void )  { return FNotSupportReason; }
	void SetEvent( void );
	int  WiitFor( int sec );
public: ///< All Functions
	lpCreateUSTPTraderApi         pCreateUSTPTraderApi;
	lpGetVersion                  pGetVersion;
	lpReleaseUSTPTraderApi        pReleaseUSTPTraderApi;
	lpInit                        pInit;
	lpGetTradingDay               pGetTradingDay;
	lpRegisterFront               pRegisterFront;
	lpRegisterNameServer          pRegisterNameServer;
	lpRegisterCertificateFile     pRegisterCertificateFile;
	lpSubscribePrivateTopic       pSubscribePrivateTopic;
	lpSubscribePublicTopic        pSubscribePublicTopic;
	lpSubscribeUserTopic          pSubscribeUserTopic;
	lpSetHeartbeatTimeout         pSetHeartbeatTimeout;
	lpOpenRequestLog              pOpenRequestLog;
	lpOpenResponseLog             pOpenResponseLog;
	lpReqUserLogin                pReqUserLogin;
	lpReqUserLogout               pReqUserLogout;
	lpReqUserPasswordUpdate       pReqUserPasswordUpdate;
	lpReqOrderInsert              pReqOrderInsert;
	lpReqOrderAction              pReqOrderAction;
	lpReqQryOrder                 pReqQryOrder;
	lpReqQryTrade                 pReqQryTrade;
	lpReqQryUserInvestor          pReqQryUserInvestor;
	lpReqQryTradingCode           pReqQryTradingCode;
	lpReqQryInvestorAccount       pReqQryInvestorAccount;
	lpReqQryInstrument            pReqQryInstrument;
	lpReqQryExchange              pReqQryExchange;
	lpReqQryInvestorPosition      pReqQryInvestorPosition;
	lpReqSubscribeTopic           pReqSubscribeTopic;
	lpReqQryComplianceParam       pReqQryComplianceParam;
	lpReqQryTopic                 pReqQryTopic;
	lpReqQryInvestorFee           pReqQryInvestorFee;
	lpReqQryInvestorMargin        pReqQryInvestorMargin;
public:  ///< All callback regist functions.
	lpRegOnFrontConnected         pRegOnFrontConnected;
	lpRegOnFrontDisconnected      pRegOnFrontDisconnected;
	lpRegOnHeartBeatWarning       pRegOnHeartBeatWarning;
	lpRegOnPackageStart           pRegOnPackageStart;
	lpRegOnPackageEnd             pRegOnPackageEnd;
	lpRegOnRspError               pRegOnRspError;
	lpRegOnRspUserLogin           pRegOnRspUserLogin;
	lpRegOnRspUserLogout          pRegOnRspUserLogout;
	lpRegOnRspUserPasswordUpdate  pRegOnRspUserPasswordUpdate;
	lpRegOnRspOrderInsert         pRegOnRspOrderInsert;
	lpRegOnRspOrderAction         pRegOnRspOrderAction;
	lpRegOnRtnFlowMessageCancel   pRegOnRtnFlowMessageCancel;
	lpRegOnRtnTrade               pRegOnRtnTrade;
	lpRegOnRtnOrder               pRegOnRtnOrder;
	lpRegOnErrRtnOrderInsert      pRegOnErrRtnOrderInsert;
	lpRegOnErrRtnOrderAction      pRegOnErrRtnOrderAction;
	lpRegOnRtnInstrumentStatus    pRegOnRtnInstrumentStatus;
	lpRegOnRspQryOrder            pRegOnRspQryOrder;
	lpRegOnRspQryTrade            pRegOnRspQryTrade;
	lpRegOnRspQryUserInvestor     pRegOnRspQryUserInvestor;
	lpRegOnRspQryTradingCode      pRegOnRspQryTradingCode;
	lpRegOnRspQryInvestorAccount  pRegOnRspQryInvestorAccount;
	lpRegOnRspQryInstrument       pRegOnRspQryInstrument;
	lpRegOnRspQryExchange         pRegOnRspQryExchange;
	lpRegOnRspQryInvestorPosition pRegOnRspQryInvestorPosition;
	lpRegOnRspSubscribeTopic      pRegOnRspSubscribeTopic;
	lpRegOnRspQryComplianceParam  pRegOnRspQryComplianceParam;
	lpRegOnRspQryTopic            pRegOnRspQryTopic;
	lpRegOnRspQryInvestorFee      pRegOnRspQryInvestorFee;
	lpRegOnRspQryInvestorMargin   pRegOnRspQryInvestorMargin;
public:
	static void OnFrontConnected( HUSTP Owner );
	static void OnFrontDisconnected( HUSTP Owner, int nReason);
	static void OnHeartBeatWarning( HUSTP Owner, int nTimeLapse);
	static void OnPackageStart( HUSTP Owner, int nTopicID, int nSequenceNo);
	static void OnPackageEnd( HUSTP Owner, int nTopicID, int nSequenceNo);
	static void OnRspError( HUSTP Owner, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspUserLogin( HUSTP Owner, CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspUserLogout( HUSTP Owner, CUstpFtdcRspUserLogoutField *pRspUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspUserPasswordUpdate( HUSTP Owner, CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspOrderInsert( HUSTP Owner, CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspOrderAction( HUSTP Owner, CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRtnFlowMessageCancel( HUSTP Owner, CUstpFtdcFlowMessageCancelField *pFlowMessageCancel);
	static void OnRtnTrade( HUSTP Owner, CUstpFtdcTradeField *pTrade);
	static void OnRtnOrder( HUSTP Owner, CUstpFtdcOrderField *pOrder);
	static void OnErrRtnOrderInsert( HUSTP Owner, CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo);
	static void OnErrRtnOrderAction( HUSTP Owner, CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo);
	static void OnRtnInstrumentStatus( HUSTP Owner, CUstpFtdcInstrumentStatusField *pInstrumentStatus);
	static void OnRspQryOrder( HUSTP Owner, CUstpFtdcOrderField *pOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryTrade( HUSTP Owner, CUstpFtdcTradeField *pTrade, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryUserInvestor( HUSTP Owner, CUstpFtdcRspUserInvestorField *pRspUserInvestor, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryTradingCode( HUSTP Owner, CUstpFtdcRspTradingCodeField *pRspTradingCode, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryInvestorAccount( HUSTP Owner, CUstpFtdcRspInvestorAccountField *pRspInvestorAccount, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryInstrument( HUSTP Owner, CUstpFtdcRspInstrumentField *pRspInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryExchange( HUSTP Owner, CUstpFtdcRspExchangeField *pRspExchange, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryInvestorPosition( HUSTP Owner, CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspSubscribeTopic( HUSTP Owner, CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryComplianceParam( HUSTP Owner, CUstpFtdcRspComplianceParamField *pRspComplianceParam, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryTopic( HUSTP Owner, CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryInvestorFee( HUSTP Owner, CUstpFtdcInvestorFeeField *pInvestorFee, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	static void OnRspQryInvestorMargin( HUSTP Owner, CUstpFtdcInvestorMarginField *pInvestorMargin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
};
//-------------------------------------------------------------------------------------------------------------------------
extern USTPTraderApiInit gUSTPAPI;
//---------------------------------------------------------------------------
#endif
