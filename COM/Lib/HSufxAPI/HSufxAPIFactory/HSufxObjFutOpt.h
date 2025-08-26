//---------------------------------------------------------------------------
#ifndef HSufxObjFutOptH
#define HSufxObjFutOptH
//---------------------------------------------------------------------------
#include <windows.h>
#include <vector>
//---------------------------------------------------------------------------
#include "HSufxObjBase.h"
#include "HSufxStructFutOpt.h"
//---------------------------------------------------------------------------
#include "COrderInfo.h"
//---------------------------------------------------------------------------
struct HSufxRemainReport
{
	UFC::UInt32 TickMs;
	char        Type;       ///< 'C'-Confirm  'F'-Fill
	void*       ReportData; ///< dataReportConfirm or dataReportFill
public:
	~HSufxRemainReport()
	{
		if ( Type == 'C' )
			delete ( (HSufxFutOpt::dataReportConfirm*)ReportData );
		else if ( Type == 'F' )
			delete ( (HSufxFutOpt::dataReportFill*)ReportData );
	}
};
//---------------------------------------------------------------------------
// class HSufxObjFutOpt
//---------------------------------------------------------------------------
class HSufxObjFutOpt : public CCallbackInterface, public HSufxObjBase
{
private:
	TTaifexConnection*	FSpeedyConnPtr;
	HWND				FHwnd;
	HINSTANCE			FInstance;
	wchar_t				MessageWindowsClassName[ 128 ];
private:
	std::vector<HSufxFutOpt::dataQryOrderResult*>	FHSufxQryOrderVector;
	std::vector<HSufxFutOpt::dataQryFillResult*>	FHSufxQryFillVector;
	UFC::List<HSufxRemainReport*>					FHSufxRemainReportList;
	UFC::List<TExecutionReportMessage*>				FHSufxCancelAfterFillList;
	UFC::PHashMap<UFC::AnsiString, HSufxFutOpt::dataRefToOrderKey* >	FHSufxCancelRefToOrigOrderKey;
	CConfigInterface*					FHSufxConfig;
	CConnectionInterface*				FHSufxConnection;
	char								FHSufxUserToken[512+1];
	char								FHSufxClientID[18+1];
	int									FHSufxSysNodeID;
	int									FHSufxBranchNo;
	int									FHSufxSessionNo;
	int									FHSufxRequestNum;
//---------------------------------------------------------------------------
// code at HSufxObjFutOptListener.cpp
//---------------------------------------------------------------------------
private:
	BOOL InitMessageWindow( void );
	BOOL CreateMessageWindow( void );
public:
	static LRESULT CALLBACK MessageWndProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam );
private:
	void PassOnConnect( void );
	void PassOnClose( void );
	void PassLoginResult( bool IsLoginOK, const char* ErrMsg,
						  int branch_no = 0, const char* fund_account ="",
						  const char* user_token = "", const char* client_id = "",
						  int sysnode_id = 0, int session_no = 0, int init_date = 0 );
	void PassLogoutResult( bool IsLogoutOK );
	void PassReplyHeartBeat( IBizMessage* lpMsg );
	void PassChangePwdResult( bool IsChangePwdOK, const char* ErrMsg );
	void PassSubscribeResult( bool IsSubscribeOK, int IssueType, const char* ErrMsg );
	void PassQryOrderResult( IF2UnPacker* Packet );
	void PassQryFillResult( IF2UnPacker* Packet );
	void PassQryAssetResult( IF2UnPacker* Packet );
	void PassQryPositionResult( IF2UnPacker* Packet );
	void PassQryExchStatusResult( IF2UnPacker* Packet );
	void PassQryExchTimeResult( IF2UnPacker* Packet );
	void PassPendingExecution( int hSend, IF2UnPacker* Packet );
	void PassPushReport( IF2UnPacker* Packet );
	void PassPushExchStatus( IF2UnPacker* Packet );
	void PassRejectExecution( int hSend, const char* ErrMsg, nsOrderMessageDefine::CxlRejResponseToEnum ResponseTo );
private:
	void RecvLogin( IF2UnPacker* Packet );
	void RecvChangePwd( IF2UnPacker* Packet );
private:
	void MainBizMsgOK( int hSend, IBizMessage* lpMsg );
	void MainBizMsgErr( int hSend, IBizMessage* lpMsg );
public: ///< implement IKnown interface
	unsigned long FUNCTION_CALL_MODE QueryInterface(const char *iid, IKnown **ppv) { return 0; }
	unsigned long FUNCTION_CALL_MODE AddRef()  { return 0; }
	unsigned long FUNCTION_CALL_MODE Release() { return 0; }
public: ///< implement CCallbackInterface interface
	void FUNCTION_CALL_MODE OnConnect(CConnectionInterface *lpConnection);
	void FUNCTION_CALL_MODE OnSafeConnect(CConnectionInterface *lpConnection);
	void FUNCTION_CALL_MODE OnRegister(CConnectionInterface *lpConnection);
	void FUNCTION_CALL_MODE OnClose(CConnectionInterface *lpConnection);
	void FUNCTION_CALL_MODE OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData) { /*no implementation*/ }
	void FUNCTION_CALL_MODE Reserved1(void *a, void *b, void *c, void *d) { /*no implementation*/ }
	void FUNCTION_CALL_MODE Reserved2(void *a, void *b, void *c, void *d) { /*no implementation*/ }
	int  FUNCTION_CALL_MODE Reserved3() { /*no implementation*/ return 0; }
	void FUNCTION_CALL_MODE Reserved4() { /*no implementation*/ }
	void FUNCTION_CALL_MODE Reserved5() { /*no implementation*/ }
	void FUNCTION_CALL_MODE Reserved6() { /*no implementation*/ }
	void FUNCTION_CALL_MODE Reserved7() { /*no implementation*/ }
	void FUNCTION_CALL_MODE OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult) { /*no implementation*/ }
	void FUNCTION_CALL_MODE OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult) { /*no implementation*/ }
	void FUNCTION_CALL_MODE OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg);
//---------------------------------------------------------------------------
// code at HSufxObjFutOpt.cpp
//---------------------------------------------------------------------------
private: ///< HSufx Receiving functions (<-)
	void HSufxOnConnect( void );
	void HSufxOnClose( void );
	void HSufxReQueryCmd( HSufxFutOpt::dataReQueryCmd* Data );
	void HSufxLoginResult( HSufxFutOpt::dataLoginResult* Data );
	void HSufxLogoutResult( HSufxFutOpt::dataLogoutResult* Data );
	void HSufxChangePwdResult( HSufxFutOpt::dataChangePwdResult* Data );
	void HSufxSubscribeResult( HSufxFutOpt::dataSubscribeResult* Data );
	void HSufxQryOrderResult( HSufxFutOpt::dataQryOrderResult* Data );
	void HSufxQryFillResult( HSufxFutOpt::dataQryFillResult* Data );
	void HSufxQryAssetResult( HSufxFutOpt::dataQryAssetResult* Data );
	void HSufxQryPositionResult( HSufxFutOpt::dataQryPositionResult* Data );
	void HSufxExchStatusResult( HSufxFutOpt::dataExchStatusResult* Data );
	void HSufxExchTimeResult( HSufxFutOpt::dataExchTimeResult* Data );
	void HSufxPendingExecution( HSufxFutOpt::dataPendingExecution* Data );
	void HSufxRejectExecution( HSufxFutOpt::dataRejectExecution* Data );
	char* GetNextPushToken( char*& strPos );
	void HSufxPushReport( HSufxFutOpt::dataPushReport* Data );
	BOOL HSufxPushReportReDo( HSufxRemainReport* Data );
	BOOL HSufxPushConfirm( HSufxFutOpt::dataReportConfirm& Data, BOOL IsCallByReDo = FALSE );
	BOOL HSufxPushFill( HSufxFutOpt::dataReportFill& Data, BOOL IsCallByReDo = FALSE );
	BOOL HSufxConfirmExecution( HSufxFutOpt::dataReportConfirm& Data );
	BOOL HSufxRejectByExh( HSufxFutOpt::dataReportConfirm& Data, nsOrderMessageDefine::CxlRejResponseToEnum ResponseTo, BOOL IsCallByReDo = FALSE );
	BOOL HSufxCanceledExecution( HSufxFutOpt::dataReportConfirm& Data );
	BOOL HSufxFillExecution( HSufxFutOpt::dataReportFill& Data, BOOL IsCallByReDo = FALSE );
private: ///< HSufx Convert function
	const char* HSufxTo_ExchangeCode( const UFC::AnsiString& exchange_type );
	const char* HSufxTo_exchange_type( const UFC::AnsiString& ExchangeCode );
	void        HSufxTo_CTPExchTime( UFC::AnsiString& StrTime, int curr_time /*HHMMSSmmm*/ );
private: ///< Container operation functions
	void ClearQryOrderVector( void );
	void ClearQryFillVector( void );
	void ClearRemainReportList(void );
	void ClearCancelList( void );
	void DoQryOrderVector( void );
	void DoQryFillVector( void );
	void DoCancelList( void );
	void QryOrderFinishProcess( void );
	void QryFillFinishProcess( void );
	void AddRemainReport( HSufxFutOpt::dataReportConfirm& Data );
	void AddRemainReport( HSufxFutOpt::dataReportFill& Data );
	void CheckRemainReport( void );
private: ///< HSufx Sending functions (->)
	void HSufxReplyHeartBeatMsg( HSufxFutOpt::dataReplyHeartBeatMsg* Data );
	void HSufxSendSubscribePush( void );
	void HSufxSendQryOrderConfirm( const char* position_str = NULL );
	void HSufxSendQryOrderFill( const char* position_str = NULL );
	void HSufxSendQryExchStatus( void );
	void HSufxSendQryExchTime( void );
	void HSufxSendSubscribeExchStatus( void );
	int  HSufxSendChangePwd( TNewsMessage* Msg );
	int  HSufxSendQryPosition( const char* position_str = NULL );
	int  HSufxSendQryAssert( TNewsMessage* Msg );
	COrderInfo* HSufxAddPendingOrder( int OrdRef, TNewOrderMessage* Ord, const UFC::AnsiString& UserDefine );
public : ///< implement HSufxObjBase interface
	void HSufxInitial( void );
	void HSufxRelease( void );
	BOOL HSufxConnect( const char* strT2Servers, int TimeoutSec );
	void HSufxSendLogin( void );
	void HSufxSendLogout( void );
	void HSufxNewOrder( TNewOrderMessage* Msg, const UFC::AnsiString& UserDefine );
	void HSufxCancelOrder( TCancelOrderMessage* Msg, const UFC::AnsiString& UserDefine );
	int  HandleHSufxMessage( TNewsMessage* Msg );
public:
	HSufxObjFutOpt( TTaifexConnection* SpeedyConnPtr );
	virtual ~HSufxObjFutOpt();
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
