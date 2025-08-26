//---------------------------------------------------------------------------
#ifndef USTPBCBTraderAPIH
#define USTPBCBTraderAPIH
//-------------------------------------------------------------------------------------------------------------------------
#include "USTPFtdcTraderApi.h"
#include "USTPFtdcUserApiDataType.h"
#include <windows.h>
//-------------------------------------------------------------------------------------------------------------------------
//
//  Femas API events listener interface
//
//-------------------------------------------------------------------------------------------------------------------------
class TUstpFtdcTraderSpi
{
public:
	virtual void OnFemasAPIConnected(){};
	virtual void OnFemasAPIDisconnected(int nReason){};
	virtual void OnFemasRecoverFinished(int Count ){};
public:
	virtual void OnFemasFrontConnected(){};
	virtual void OnFemasFrontDisconnected(int nReason){};
	virtual void OnFemasHeartBeatWarning(int nTimeLapse){};
	virtual void OnFemasPackageStart(int nTopicID, int nSequenceNo){};
	virtual void OnFemasPackageEnd(int nTopicID, int nSequenceNo){};
	virtual void OnFemasRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRspUserLogout(CUstpFtdcRspUserLogoutField *pRspUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRspUserPasswordUpdate(CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};
	virtual void OnFemasRspOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRspOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRtnFlowMessageCancel(CUstpFtdcFlowMessageCancelField *pFlowMessageCancel) {};
	virtual void OnFemasRtnTrade(CUstpFtdcTradeField *pTrade) {};
	virtual void OnFemasRtnOrder(CUstpFtdcOrderField *pOrder) {};
	virtual void OnFemasErrRtnOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo) {};
	virtual void OnFemasErrRtnOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo) {};
	virtual void OnFemasRtnInstrumentStatus(CUstpFtdcInstrumentStatusField *pInstrumentStatus) {};
	virtual void OnFemasRspQryOrder(CUstpFtdcOrderField *pOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRspQryTrade(CUstpFtdcTradeField *pTrade, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRspQryUserInvestor(CUstpFtdcRspUserInvestorField *pRspUserInvestor, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRspQryTradingCode(CUstpFtdcRspTradingCodeField *pRspTradingCode, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRspQryInvestorAccount(CUstpFtdcRspInvestorAccountField *pRspInvestorAccount, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){};
	virtual void OnFemasRspQryInstrument(CUstpFtdcRspInstrumentField *pRspInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRspQryExchange(CUstpFtdcRspExchangeField *pRspExchange, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRspSubscribeTopic(CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRspQryComplianceParam(CUstpFtdcRspComplianceParamField *pRspComplianceParam, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRspQryTopic(CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRspQryInvestorFee(CUstpFtdcInvestorFeeField *pInvestorFee, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	virtual void OnFemasRspQryInvestorMargin(CUstpFtdcInvestorMarginField *pInvestorMargin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
};
//-------------------------------------------------------------------------------------------------------------------------
//
//  Femas Trader API class.
//
//-------------------------------------------------------------------------------------------------------------------------
class TUstpFtdcTraderApi
{
private:
	void*               FHUSTPAPI;
	TUstpFtdcTraderSpi* FListener;
private:
	HWND            FHwnd;
	HINSTANCE       FInstance;
	DWORD           FThreadID;
	wchar_t MessageWindowsClassName[ 128 ];
private:
	BOOL CreateMessageWindow( void );
	BOOL InitMessageWindow( void );
	void RegAllCallbackFuncs( void );
public:
	static LRESULT CALLBACK MessageWndProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam );
	static bool SupportUSTPAPI( void );
	static const char* LoadUSTPAPIResult( void );
public:
	TUstpFtdcTraderApi( TUstpFtdcTraderSpi *pSpi );
	~TUstpFtdcTraderApi();
	TUstpFtdcTraderSpi*  GetSpi( void ) { return FListener; }
	HWND                 GetHWND( void ) { return FHwnd; }
public:
	void Init();
	const char *GetVersion(int &nMajorVersion, int &nMinorVersion);
	const char *GetTradingDay();
	void RegisterFront(char *pszFrontAddress);
	void RegisterNameServer(char *pszNsAddress);
	void RegisterSpi( TUstpFtdcTraderSpi *pSpi);
	int  RegisterCertificateFile(const char *pszCertFileName, const char *pszKeyFileName,const char *pszCaFileName, const char *pszKeyFilePassword);
	void SubscribePrivateTopic(USTP_TE_RESUME_TYPE nResumeType);
	void SubscribePublicTopic(USTP_TE_RESUME_TYPE nResumeType);
	void SubscribeUserTopic(USTP_TE_RESUME_TYPE nResumeType);
	void SetHeartbeatTimeout(unsigned int timeout);
	int  OpenRequestLog(const char *pszReqLogFileName);
	int  OpenResponseLog(const char *pszRspLogFileName);
	int  ReqUserLogin(CUstpFtdcReqUserLoginField *pReqUserLogin, int nRequestID);
	int  ReqUserLogout(CUstpFtdcReqUserLogoutField *pReqUserLogout, int nRequestID);
	int  ReqUserPasswordUpdate(CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID);
	int  ReqOrderInsert(CUstpFtdcInputOrderField *pInputOrder, int nRequestID);
	int  ReqOrderAction(CUstpFtdcOrderActionField *pOrderAction, int nRequestID);
	int  ReqQryOrder(CUstpFtdcQryOrderField *pQryOrder, int nRequestID);
	int  ReqQryTrade(CUstpFtdcQryTradeField *pQryTrade, int nRequestID);
	int  ReqQryUserInvestor(CUstpFtdcQryUserInvestorField *pQryUserInvestor, int nRequestID);
	int  ReqQryTradingCode(CUstpFtdcQryTradingCodeField *pQryTradingCode, int nRequestID);
	int  ReqQryInvestorAccount(CUstpFtdcQryInvestorAccountField *pQryInvestorAccount, int nRequestID);
	int  ReqQryInstrument(CUstpFtdcQryInstrumentField *pQryInstrument, int nRequestID);
	int  ReqQryExchange(CUstpFtdcQryExchangeField *pQryExchange, int nRequestID);
	int  ReqQryInvestorPosition(CUstpFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID);
	int  ReqSubscribeTopic(CUstpFtdcDisseminationField *pDissemination, int nRequestID);
	int  ReqQryComplianceParam(CUstpFtdcQryComplianceParamField *pQryComplianceParam, int nRequestID);
	int  ReqQryTopic(CUstpFtdcDisseminationField *pDissemination, int nRequestID);
	int  ReqQryInvestorFee(CUstpFtdcQryInvestorFeeField *pQryInvestorFee, int nRequestID);
	int  ReqQryInvestorMargin(CUstpFtdcQryInvestorMarginField *pQryInvestorMargin, int nRequestID);
	void ReqOnRecoverFilisned( int Count );
	void SetEvent( void );
};
//---------------------------------------------------------------------------
#endif
