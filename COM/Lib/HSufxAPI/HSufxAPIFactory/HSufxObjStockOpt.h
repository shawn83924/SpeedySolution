//---------------------------------------------------------------------------
#ifndef HSufxObjStockOptH
#define HSufxObjStockOptH
//---------------------------------------------------------------------------
#include <windows.h>
//---------------------------------------------------------------------------
#include "HSufxObjBase.h"
#include "HSufxStructStockOpt.h"
//---------------------------------------------------------------------------
#include "COrderInfo.h"
//---------------------------------------------------------------------------
// class HSufxObjStockOpt
//---------------------------------------------------------------------------
class HSufxObjStockOpt : public CCallbackInterface, public HSufxObjBase
{
private:
	TTaifexConnection*	FSpeedyConnPtr;
	HWND				FHwnd;
	HINSTANCE			FInstance;
	wchar_t				MessageWindowsClassName[ 128 ];
private:
	UFC::List<TExecutionReportMessage*>	FHSufxCancelAfterFillList;
	UFC::PHashMap<UFC::AnsiString, int>	FHSufxCancelRefToOrdLocalID;
	CConfigInterface*					FHSufxConfig;
	CConnectionInterface*				FHSufxConnection;
	char								FHSufxUserToken[40+1];
	char								FHSufxClientID[18+1];
	int									FHSufxSysNodeID;
	int									FHSufxBranchNo;
	int									FHSufxLoginTimes;
	int									FHSufxSubscribeMask; ///< 0x01:Confirm 0x02 Fill
//---------------------------------------------------------------------------
// code at HSufxObjStockOptListener.cpp
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
						  int sysnode_id = 0, int login_times = 0, int init_date = 0 );
	void PassLogoutResult( bool IsLogoutOK );
	void PassReplyHeartBeat( LPRET_DATA lpRetData );
	void PassReplyHeartBeat( IBizMessage* lpMsg );
	void PassChangePwdResult( bool IsChangePwdOK, const char* ErrMsg );
	void PassSubscribeResult( bool IsSubscribeOK, int IssueType, const char* ErrMsg );
	void PassQryOrderResult( IF2UnPacker* Packet );
	void PassQryFillResult( IF2UnPacker* Packet );
	void PassQryAssetResult( IF2UnPacker* Packet );
	void PassQryPositionResult( IF2UnPacker* Packet );
	void PassPendingExecution( int hSend, IF2UnPacker* Packet );
	void PassConfirmExecution( IF2UnPacker* Packet );
	void PassRejectExecution( int hSend, const char* ErrMsg, nsOrderMessageDefine::CxlRejResponseToEnum ResponseTo );
	void PassRejectByExh( IF2UnPacker* Packet, nsOrderMessageDefine::CxlRejResponseToEnum ResponseTo );
	void PassCanceledExecution( IF2UnPacker* Packet );
	void PassFillExecution( IF2UnPacker* Packet );
private:
	void RecvReportConfirm( IF2UnPacker* Packet );
	void RecvReportFill( IF2UnPacker* Packet );
	void RecvLogin( IF2UnPacker* Packet );
	void RecvChangePwd( IF2UnPacker* Packet );
private:
	void MainBizEx04( int hSend, LPRET_DATA lpRetData, const void *lpUnPackerOrStr, int nResult );
	void MainBizEx12( int hSend, LPRET_DATA lpRetData, const void *lpUnPackerOrStr, int nResult );
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
	void FUNCTION_CALL_MODE OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult);
	void FUNCTION_CALL_MODE OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg);
//---------------------------------------------------------------------------
// code at HSufxObjStockOpt.cpp
//---------------------------------------------------------------------------
private: ///< HSufx Receiving functions (<-)
	void HSufxOnConnect( void );
	void HSufxOnClose( void );
	void HSufxLoginResult( HSufxStockOpt::dataLoginResult* Data );
	void HSufxLogoutResult( HSufxStockOpt::dataLogoutResult* Data );
	void HSufxChangePwdResult( HSufxStockOpt::dataChangePwdResult* Data );
	void HSufxSubscribeResult( HSufxStockOpt::dataSubscribeResult* Data );
	void HSufxQryOrderResult( HSufxStockOpt::dataQryOrderResult* Data );
	void HSufxQryFillResult( HSufxStockOpt::dataQryFillResult* Data );
	void HSufxQryAssetResult( HSufxStockOpt::dataQryAssetResult* Data );
	void HSufxQryPositionResult( HSufxStockOpt::dataQryPositionResult* Data );
	void HSufxPendingExecution( HSufxStockOpt::dataPendingExecution* Data );
	void HSufxConfirmExecution( HSufxStockOpt::dataConfirmExecution* Data );
	void HSufxRejectExecution( HSufxStockOpt::dataRejectExecution* Data );
	void HSufxRejectByExh( HSufxStockOpt::dataRejectByExh* Data );
	void HSufxCanceledExecution( HSufxStockOpt::dataCanceledExecution* Data );
	void HSufxFillExecution( HSufxStockOpt::dataFillExecution* Data );
private: ///< HSufx Sending functions (->)
	const char* HSufxTo_ExchangeCode( const UFC::AnsiString& exchange_type );
	const char* HSufxTo_exchange_type( const UFC::AnsiString& ExchangeCode );
	void HSufxClearCancelList( void );
	void HSufxDoCancelList( void );
	void HSufxReplyHeartBeat( HSufxStockOpt::dataReplyHeartBeat* Data );
	void HSufxReplyHeartBeatMsg( HSufxStockOpt::dataReplyHeartBeatMsg* Data );
	void HSufxSendSubscribePush( int IssueType );
	void HSufxSendQryOrderConfirm( void );
	void HSufxSendQryOrderFill( void );
	int  HSufxSendChangePwd( TNewsMessage* Msg );
	int  HSufxSendQryPosition( TNewsMessage* Msg );
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
	HSufxObjStockOpt( TTaifexConnection* SpeedyConnPtr );
	virtual ~HSufxObjStockOpt();
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

