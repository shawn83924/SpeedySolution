#include "stdafx.h"
#include "FemasAPIFactory.h"
#include "FemasAPIDynamic.h"
#include "USTPFtdcTraderApi.h"
//---------------------------------------------------------------------------
typedef struct tagUSPHandle
{
	CUstpFtdcTraderApi*  USTPObj;
	CUstpFtdcTraderSpi*  USTPHandler;
	void*                FOwner;
	///< Call back functions
	pfOnFrontConnected            OnFrontConnected;
	pfOnFrontDisconnected         OnFrontDisconnected;
	pfOnHeartBeatWarning          OnHeartBeatWarning;
	pfOnPackageStart              OnPackageStart;
	pfOnPackageEnd                OnPackageEnd;
	pfOnRspError                  OnRspError;
	pfOnRspUserLogin              OnRspUserLogin;
	pfOnRspUserLogout             OnRspUserLogout;
	pfOnRspUserPasswordUpdate     OnRspUserPasswordUpdate;
	pfOnRspOrderInsert            OnRspOrderInsert;
	pfOnRspOrderAction            OnRspOrderAction;
	pfOnRtnFlowMessageCancel      OnRtnFlowMessageCancel;
	pfOnRtnTrade                  OnRtnTrade;
	pfOnRtnOrder                  OnRtnOrder;
	pfOnErrRtnOrderInsert         OnErrRtnOrderInsert;
	pfOnErrRtnOrderAction         OnErrRtnOrderAction;
	pfOnRtnInstrumentStatus       OnRtnInstrumentStatus;
	pfOnRspQryOrder               OnRspQryOrder;
	pfOnRspQryTrade               OnRspQryTrade;
	pfOnRspQryUserInvestor        OnRspQryUserInvestor;
	pfOnRspQryTradingCode         OnRspQryTradingCode;
	pfOnRspQryInvestorAccount     OnRspQryInvestorAccount;
	pfOnRspQryInstrument          OnRspQryInstrument;
	pfOnRspQryExchange            OnRspQryExchange;
	pfOnRspQryInvestorPosition    OnRspQryInvestorPosition;
	pfOnRspSubscribeTopic         OnRspSubscribeTopic;
	pfOnRspQryComplianceParam     OnRspQryComplianceParam;
	pfOnRspQryTopic               OnRspQryTopic;
	pfOnRspQryInvestorFee         OnRspQryInvestorFee;
	pfOnRspQryInvestorMargin      OnRspQryInvestorMargin;

} USTPHandle;
//---------------------------------------------------------------------------
class USTPAPIHandler : public CUstpFtdcTraderSpi
{
private:
	USTPHandle* FhUSTP;
public:
	USTPAPIHandler( USTPHandle* ustpobj ):FhUSTP( ustpobj ){}
	~USTPAPIHandler() {}
	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);
	virtual void OnHeartBeatWarning(int nTimeLapse);
	virtual void OnPackageStart(int nTopicID, int nSequenceNo);
	virtual void OnPackageEnd(int nTopicID, int nSequenceNo);
	virtual void OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUserLogout(CUstpFtdcRspUserLogoutField *pRspUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUserPasswordUpdate(CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnFlowMessageCancel(CUstpFtdcFlowMessageCancelField *pFlowMessageCancel);
	virtual void OnRtnTrade(CUstpFtdcTradeField *pTrade);
	virtual void OnRtnOrder(CUstpFtdcOrderField *pOrder);
	virtual void OnErrRtnOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo);
	virtual void OnErrRtnOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo);
	virtual void OnRtnInstrumentStatus(CUstpFtdcInstrumentStatusField *pInstrumentStatus);
	virtual void OnRspQryOrder(CUstpFtdcOrderField *pOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryTrade(CUstpFtdcTradeField *pTrade, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryUserInvestor(CUstpFtdcRspUserInvestorField *pRspUserInvestor, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryTradingCode(CUstpFtdcRspTradingCodeField *pRspTradingCode, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInvestorAccount(CUstpFtdcRspInvestorAccountField *pRspInvestorAccount, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInstrument(CUstpFtdcRspInstrumentField *pRspInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryExchange(CUstpFtdcRspExchangeField *pRspExchange, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspSubscribeTopic(CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryComplianceParam(CUstpFtdcRspComplianceParamField *pRspComplianceParam, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryTopic(CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInvestorFee(CUstpFtdcInvestorFeeField *pInvestorFee, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInvestorMargin(CUstpFtdcInvestorMarginField *pInvestorMargin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
};
//---------------------------------------------------------------------------

