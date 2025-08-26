//---------------------------------------------------------------------------

#ifndef UnitTestFutOptH
#define UnitTestFutOptH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
//---------------------------------------------------------------------------
#include "t2sdk_interface.h"
#include "HSufxBCBInit.h"
//#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
#define HSufx_SyncSend		0
#define HSufx_ASyncSend		1
//---------------------------------------------------------------------------
typedef AnsiStringT<936> GB2312String;
//---------------------------------------------------------------------------
namespace HSufxAPI
{
//---------------------------------------------------------------------------
struct dataHeartBeat
{
	REQ_DATA ReqData;
};
//---------------------------------------------------------------------------
struct dataHeartBeatMsg
{
	char* Buffer;
	int   BufferLen;
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
class HSufxListener : public CCallbackInterface
{
private:
	HWND				FHwnd;
	HINSTANCE			FInstance;
	wchar_t MessageWindowsClassName[ 128 ];
private:
	BOOL InitMessageWindow( void );
	BOOL CreateMessageWindow( void );
public:
	static LRESULT CALLBACK MessageWndProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam );
private:
	void RecvHeartBeatMsg( IBizMessage* lpMsg );
	void SendHeartBeatMsg( HSufxAPI::dataHeartBeatMsg* Data );
public:
	// 因為CCallbackInterface的最終純虛基類是IKnown，所以需要實現一下這3個方法
	unsigned long FUNCTION_CALL_MODE QueryInterface(const char *iid, IKnown **ppv);
	unsigned long FUNCTION_CALL_MODE AddRef();
	unsigned long FUNCTION_CALL_MODE Release();

	// 各種事件發生時的回調方法，實際使用時可以根據需要來選擇實現，對於不需要的事件回調方法，可直接return
	// Reserved?為保留方法，為以後擴展做準備，實現時可直接return或return 0。
	void FUNCTION_CALL_MODE OnConnect(CConnectionInterface *lpConnection);
	void FUNCTION_CALL_MODE OnSafeConnect(CConnectionInterface *lpConnection);
	void FUNCTION_CALL_MODE OnRegister(CConnectionInterface *lpConnection);
	void FUNCTION_CALL_MODE OnClose(CConnectionInterface *lpConnection);
	void FUNCTION_CALL_MODE OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData);
	void FUNCTION_CALL_MODE Reserved1(void *a, void *b, void *c, void *d);
	void FUNCTION_CALL_MODE Reserved2(void *a, void *b, void *c, void *d);
	int  FUNCTION_CALL_MODE Reserved3();
	void FUNCTION_CALL_MODE Reserved4();
	void FUNCTION_CALL_MODE Reserved5();
	void FUNCTION_CALL_MODE Reserved6();
	void FUNCTION_CALL_MODE Reserved7();
	void FUNCTION_CALL_MODE OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult);
	void FUNCTION_CALL_MODE OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult);
	void FUNCTION_CALL_MODE OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg);
public:
	HSufxListener( );
};
//---------------------------------------------------------------------------
class TTradingForm : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	TLabel *Label16;
	TButton *BtnConnStatus;
	TButton *BtnPosition;
	TButton *BtnAccount;
	TButton *BtnChangePwd;
	TButton *BtnNewOrder;
	TButton *BtnConn_Login;
	TButton *BtnDisconnect;
	TEdit *TxtBrokerID;
	TEdit *TxtUserID;
	TEdit *TxtPassword;
	TEdit *TxtServerAddress;
	TMemo *Memo1;
	TButton *BtnSubPush;
	TButton *BtnQrySymbol;
	TEdit *TxtSymbol;
	TEdit *TxtQuantity;
	TEdit *TxtPrice;
	TEdit *TxtBS;
	TEdit *TxtOC;
	TEdit *TxtOrderCondition;
	TButton *BtnCancelOrder;
	TEdit *TxtLastOrderNo;
	TButton *BtnQryOrder;
	TButton *BtnQryFill;
	TEdit *TxtUserToken;
	TEdit *TxtClientID;
	TCheckBox *ChkShowHB;
	TEdit *TxtLoginTimes;
	TButton *BtnQryCommission;
	TEdit *TxtVersionInfo;
	TButton *BtnLogoff;
	TEdit *TxtSysNodeID;
	TButton *BtnExchStatus;
	TButton *BtnQryExchStatus;
	TButton *BtnQryExchTime;
	TButton *BtnInitPosition;
	TLabel *Label17;
	TEdit *TxtRequestNum;
	void __fastcall BtnConn_LoginClick(TObject *Sender);
	void __fastcall BtnLogoffClick(TObject *Sender);
	void __fastcall BtnDisconnectClick(TObject *Sender);
	void __fastcall BtnConnStatusClick(TObject *Sender);
	void __fastcall BtnSubPushClick(TObject *Sender);
	void __fastcall BtnChangePwdClick(TObject *Sender);
	void __fastcall BtnPositionClick(TObject *Sender);
	void __fastcall BtnAccountClick(TObject *Sender);
	void __fastcall BtnQrySymbolClick(TObject *Sender);
	void __fastcall BtnNewOrderClick(TObject *Sender);
	void __fastcall BtnCancelOrderClick(TObject *Sender);
	void __fastcall BtnQryOrderClick(TObject *Sender);
	void __fastcall BtnQryFillClick(TObject *Sender);
	void __fastcall BtnQryCommissionClick(TObject *Sender);
	void __fastcall BtnExchStatusClick(TObject *Sender);
	void __fastcall BtnQryExchStatusClick(TObject *Sender);
	void __fastcall BtnQryExchTimeClick(TObject *Sender);
	void __fastcall BtnInitPositionClick(TObject *Sender);
private:
	void GetGlobalVariant( void );
	void SetGlobalVariant( void );
	void SetUfxConfig( void );
public:
	void ShowPacket( BOOL IsUnPackerData, IF2UnPacker* pUnPacker, AnsiString* PositionStr = NULL );
public:
	void SendNewOrderMsg( void );		///< 338202 Msg
	void SendCancelOrderMsg( void );	///< 338217 Msg
	void SendQryOrderMsg( AnsiString* PositionStr = NULL );		///< 338301 Msg
	void SendQryFillMsg( AnsiString* PositionStr = NULL );		///< 338302 Msg
	void SendLoginMsg( void );			///< 331100 Msg
	void SendLogoutMsg( void ); 		///< 331102 Msg
	void SendSubPushMsg( void );		///< 620001 Msg - 33101
	void SendSubExchStatus( void );		///< 620001 Msg - 33105 交易所狀態 (0:連接接斷開 1:開盤前 2:集合報價 3:競價平衡 4:競價撮合 5:連續交易 6:暫停交易 7:閉市 X:未知)
	void SendChangePwdMsg( void );		///< 331101 Msg
	void SendPositionMsg( void );		///< 338303 Msg
	void SendInitPositionMsg( void );	///< 338326 Msg
	void SendAccountMsg( void );		///< 338300 Msg
	void SendQrySymbolMsg( AnsiString* PositionStr = NULL );	///< 330850 Msg
	void SendQryCommissionMsg( void );	///< 338304 Msg
	void SendQryExchStatus( void );		///< 338313 Msg
	void SendQryExchTime( void );		///< 330852 Msg
public:
	void RecvBizMsg( String BizName, IBizMessage* lpMsg, bool IsRegMsg = false );
	void RecvBizMsgNewOrder( IBizMessage* lpMsg );
	void RecvBizMsgLogin( IBizMessage* lpMsg );
	void RecvBizMsgHeartBeat( IBizMessage* lpMsg );
public:
	void SetButtons( bool IsConnected );
	void CloseConnection( void );
	void ReleaseAPI( void );
public:
	CConnectionInterface*	FUfxConnection;
private:	// User declarations
	CConfigInterface*		FUfxConfig;
	HSufxListener*			FListener;
	char					FUserToken[40+1];
	char					FClientID[18+1];
public:		// User declarations
	TStringList*	FStrings;
	__fastcall TTradingForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TTradingForm *TradingForm;
//---------------------------------------------------------------------------
#endif
