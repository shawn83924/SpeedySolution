//---------------------------------------------------------------------------

#ifndef UnitTestStockOptH
#define UnitTestStockOptH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "t2sdk_interface.h"
#include "HSufxBCBInit.h"
#include <ComCtrls.hpp>
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
	void RecvHeartBeat( LPRET_DATA lpRetData );
	void SendHeartBeat( HSufxAPI::dataHeartBeat* Data );
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
	TButton *BtnQryUnderlying;
	TLabel *Label5;
	TEdit *TxtSymbol;
	TLabel *Label6;
	TEdit *TxtQuantity;
	TLabel *Label7;
	TEdit *TxtPrice;
	TLabel *Label8;
	TEdit *TxtBS;
	TLabel *Label9;
	TEdit *TxtOC;
	TLabel *Label10;
	TEdit *TxtOrderCondition;
	TButton *BtnCancelOrder;
	TLabel *Label11;
	TEdit *TxtLastOrderNo;
	TButton *BtnQryOrder;
	TButton *BtnQryFill;
	TLabel *Label12;
	TEdit *TxtUserToken;
	TLabel *Label13;
	TEdit *TxtClientID;
	TCheckBox *ChkShowHB;
	TLabel *Label14;
	TEdit *TxtLoginTimes;
	TButton *BtnQryCommission;
	TLabel *Label15;
	TEdit *TxtVersionInfo;
	TButton *BtnLogoff;
	TLabel *Label16;
	TEdit *TxtSysNodeID;
	void __fastcall BtnConn_LoginClick(TObject *Sender);
	void __fastcall BtnDisconnectClick(TObject *Sender);
	void __fastcall BtnConnStatusClick(TObject *Sender);
	void __fastcall BtnSubPushClick(TObject *Sender);
	void __fastcall BtnChangePwdClick(TObject *Sender);
	void __fastcall BtnPositionClick(TObject *Sender);
	void __fastcall BtnAccountClick(TObject *Sender);
	void __fastcall BtnQrySymbolClick(TObject *Sender);
	void __fastcall BtnQryUnderlyingClick(TObject *Sender);
	void __fastcall BtnNewOrderClick(TObject *Sender);
	void __fastcall BtnCancelOrderClick(TObject *Sender);
	void __fastcall BtnQryOrderClick(TObject *Sender);
	void __fastcall BtnQryFillClick(TObject *Sender);
	void __fastcall BtnQryCommissionClick(TObject *Sender);
	void __fastcall BtnLogoffClick(TObject *Sender);
private:
	void GetGlobalVariant( void );
	void SetGlobalVariant( void );
	void SetUfxConfig( void );
public:
	void ShowPacket( BOOL IsUnPackerData, IF2UnPacker* pUnPacker );
public:
	void SendNewOrder( void );			///< 338011
	void SendNewOrderMsg( void );		///< 338011 Msg
	void SendCancelOrder( void );		///< 338012
	void SendCancelOrderMsg( void );	///< 338012 Msg
	void SendQryOrder( void );			///< 338020
	void SendQryOrderMsg( void );		///< 338020 Msg
	void SendQryFill( void );			///< 338021
	void SendQryFillMsg( void );		///< 338021 Msg
	void SendLogin( void );				///< 331100
	void SendLoginMsg( void );			///< 331100 Msg
	void SendLogout( void ); 			///< 331102
	void SendLogoutMsg( void ); 		///< 331102 Msg
	void SendSubPush( BOOL IsFill );	///< 620001
	void SendSubPushMsg( BOOL IsFill );	///< 620001 Msg
	void SendChangePwd( void );			///< 331101
	void SendChangePwdMsg( void );		///< 331101 Msg
	void SendPosition( void );			///< 338023
	void SendPositionMsg( void );		///< 338023 Msg
	void SendAccount( void );			///< 338022
	void SendAccountMsg( void );		///< 338022 Msg
	void SendQrySymbol( void );			///< 338000
	void SendQrySymbolMsg( void );	  	///< 338000 Msg
	void SendQryUnderling( void );		///< 338001
	void SendQryUnderlingMsg( void ); 	///< 338001 Msg
	void SendQryCommission( void );		///< 331156
	void SendQryCommissionMsg( void );	///< 331156 Msg
public:
	void RecvBizEx( String BizName, LPRET_DATA lpRetData, const void *lpUnpackerOrStr );
	void RecvNewOrder( LPRET_DATA lpRetData, const void *lpUnpackerOrStr );
	void RecvLogin( LPRET_DATA lpRetData, const void *lpUnpackerOrStr );
	void RecvHeartBeat( LPRET_DATA lpRetData, const void *lpUnpackerOrStr );
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
