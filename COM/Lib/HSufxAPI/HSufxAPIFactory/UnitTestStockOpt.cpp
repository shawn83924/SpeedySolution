//---------------------------------------------------------------------------

#include <vcl.h>
#include <WideStrUtils.hpp>
#pragma hdrstop

#include "UnitTestStockOpt.h"

#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//#pragma link "t2sdk.lib"
//extern HSufxApiInit gHSufxAPI;
//---------------------------------------------------------------------------
TTradingForm *TradingForm;
//---------------------------------------------------------------------------
int gBrokerID  = 0;
// 長江
//int gSysNodeID = 0;
//AnsiString gServerAddress( "202.103.39.205:19004" );
////AnsiString gServerAddress( "202.103.39.205:9117" );
////AnsiString gUserID   = "601073212";
////AnsiString gPassword    = "216435";
//AnsiString gUserID   = "601073238";
//AnsiString gPassword    = "123456";
// 恒生
int gSysNodeID = 5;
AnsiString gServerAddress( "117.78.3.150:6006" );
AnsiString gUserID   = "1010000078";
AnsiString gPassword    = "111111";
AnsiString gNewPassword = "123456";
////
int gCurRequestId = 0;
int gOrderRef = 1;
//---------------------------------------------------------------------------
HSufxListener::HSufxListener( )
{
/*
	// Create message window
	if ( InitMessageWindow() == FALSE )
	{
		throw Exception( " Register HSUFX window class failed." );
	}
	if ( CreateMessageWindow() == FALSE )
	{
		throw Exception( " Can't create the HSUFX message window." );
	}
*/
}
//---------------------------------------------------------------------------
BOOL HSufxListener::InitMessageWindow( void )
{
	GUID        Guid;
	WNDCLASSEXW wcx;
	if ( CoCreateGuid( &Guid ) == S_OK )
	{
		swprintf( MessageWindowsClassName, L"%0.8X-%0.4X-%0.4X-%0.2X%0.2X-%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X"
										 , Guid.Data1, Guid.Data2, Guid.Data3
										 , Guid.Data4[0], Guid.Data4[1], Guid.Data4[2], Guid.Data4[3]
										 , Guid.Data4[4], Guid.Data4[5], Guid.Data4[6], Guid.Data4[7] );
	}
	else
		swprintf( MessageWindowsClassName, L"HSUFX_TestStockOpt_MsgWin" );
	// Fill in the window class structure with parameters that describe the main window.
	wcx.cbSize = sizeof( wcx );                 // size of structure
	wcx.style = CS_HREDRAW |CS_VREDRAW;         // redraw if size changes
	wcx.lpfnWndProc   = HSufxListener::MessageWndProc;// points to window procedure
	wcx.cbClsExtra    = 0;                      // no extra class memory
	wcx.cbWndExtra    = 0;                      // no extra window memory
	wcx.hInstance     = FInstance;              // handle to instance
	wcx.hIcon         = NULL;                   // predefined app. icon
	wcx.hCursor       = NULL;                   // predefined arrow
	wcx.hbrBackground = NULL;                   // background brush
	wcx.lpszMenuName  = NULL;                   // name of menu resource
	wcx.lpszClassName = MessageWindowsClassName;// name of window class
	wcx.hIconSm = NULL;
	if ( RegisterClassExW( &wcx ) == 0 ) // Register the window class.
		return FALSE;
	return TRUE;
}
//---------------------------------------------------------------------------
BOOL HSufxListener::CreateMessageWindow( void )
{
	FHwnd = CreateWindowW( MessageWindowsClassName,	// name of window class
						   L"HSUFX_TestStockOpt_App",// title-bar string
						   WS_OVERLAPPEDWINDOW ,	// top-level window
						   10,						// horizontal position
						   10,						// vertical position
						   100,						// width
						   100,						// height
						   HWND_MESSAGE,			// Message only window
						   NULL,					// no class menu
						   NULL,					// handle to application instance
						   NULL );					// no window-creation data
	if ( !FHwnd )
		return FALSE;
	return TRUE;
}
//---------------------------------------------------------------------------
LRESULT CALLBACK HSufxListener::MessageWndProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	if ( Message == WM_USER+901 )
	{
		HSufxAPI::dataHeartBeat* Data = (HSufxAPI::dataHeartBeat*)lParam;
		HSufxListener* ThisInst = (HSufxListener*)wParam;

		ThisInst->SendHeartBeat( Data );
		delete Data;
	}
	else if ( Message == WM_USER+902 )
	{
		HSufxAPI::dataHeartBeatMsg* Data = (HSufxAPI::dataHeartBeatMsg*)lParam;
		HSufxListener* ThisInst = (HSufxListener*)wParam;

		ThisInst->SendHeartBeatMsg( Data );
		delete [] Data->Buffer;
		delete Data;
	}
	return 0;
}
//---------------------------------------------------------------------------
void HSufxListener::RecvHeartBeat( LPRET_DATA lpRetData )
{
	if ( TradingForm->ChkShowHB->Checked == true )
	{
		String Msg;
		Msg.printf( L"[Receive HeartBeat] [funcID:%d] [rtnCode:%d] [errNo:%d] [errInfo:%s] [issue_type:%d] [key:%s(%d)]"
					, lpRetData->functionID, lpRetData->returnCode, lpRetData->errorNo, lpRetData->errorInfo
					, lpRetData->issueType, String((const char*)lpRetData->lpKeyInfo), lpRetData->keyInfoLen );
		TradingForm->Memo1->Lines->Add( Msg );
	}

	HSufxAPI::dataHeartBeat* Data = new HSufxAPI::dataHeartBeat();
	memset( &Data->ReqData, 0, sizeof(REQ_DATA) );
	Data->ReqData.issueType  = 0;	///<此處需要確認，心跳應答是否區分訂閱類型
	Data->ReqData.packetType = 1;	///<轉換為應答包
	memcpy( &Data->ReqData.routeInfo, &lpRetData->sendInfo, sizeof(Route_Info) );
	PostMessage( FHwnd, WM_USER+901, (WPARAM)this, (LPARAM)Data );
}
//---------------------------------------------------------------------------
void HSufxListener::SendHeartBeat( HSufxAPI::dataHeartBeat* Data )
{
	String Msg;
	int    RtnVal;

	RtnVal = TradingForm->FUfxConnection->SendBizEx( 620000, NULL, NULL, HSufx_ASyncSend, gSysNodeID, 0, gBrokerID, &Data->ReqData );

	if ( RtnVal < 0 )
	{
		GB2312String GBStr( TradingForm->FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBizEx HeartBeat(%d) packet failed!! ErrorCode[%d] ErrorMsg[%s]"
					, Data->ReqData.issueType, RtnVal, String(GBStr) );
		TradingForm->Memo1->Lines->Add( Msg );
	}
	else
	{
		if ( TradingForm->ChkShowHB->Checked == true )
		{
			Msg.printf( L"SendBizEx HeartBeat(%d) packet OK", Data->ReqData.issueType );
			TradingForm->Memo1->Lines->Add( Msg );
		}
	}
}
//---------------------------------------------------------------------------
void HSufxListener::RecvHeartBeatMsg( IBizMessage* lpMsg )
{
	if ( TradingForm->ChkShowHB->Checked == true )
	{
		String Msg;
		Msg.printf( L"[Receive HeartBeatMsg] [funcID:%d] [rtnCode:%d] [errNo:%d] [errInfo:%s] [issue_type:%d]"
					, lpMsg->GetFunction(), lpMsg->GetReturnCode(), lpMsg->GetErrorNo(), lpMsg->GetErrorInfo(), lpMsg->GetIssueType() );
		TradingForm->Memo1->Lines->Add( Msg );
	}

	lpMsg->ChangeReq2AnsMessage();
	HSufxAPI::dataHeartBeatMsg* Data = new HSufxAPI::dataHeartBeatMsg();
	void* lpMsgBuffer = lpMsg->GetBuff( Data->BufferLen );
	Data->Buffer = new char[ Data->BufferLen ];
	memcpy( Data->Buffer, lpMsgBuffer, Data->BufferLen );
	PostMessage( FHwnd, WM_USER+902, (WPARAM)this, (LPARAM)Data );
}
//---------------------------------------------------------------------------
void HSufxListener::SendHeartBeatMsg( HSufxAPI::dataHeartBeatMsg* Data )
{
	String Msg;
	int    RtnVal;

	IBizMessage* lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		TradingForm->Memo1->Lines->Add( "When SendHeartBeatMsg() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetBuff( Data->Buffer, Data->BufferLen );
	RtnVal = TradingForm->FUfxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( TradingForm->FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"[Receive HeartBeatMsg] [issue_type:%d] SendBizMsg Reply HeartBeat packet failed!! ErrorCode[%d] ErrorMsg[%s]"
					 , lpBizMessage->GetIssueType(), RtnVal, String(GBStr) );
		TradingForm->Memo1->Lines->Add( Msg );
	}
	else
	{
		if ( TradingForm->ChkShowHB->Checked == true )
		{
			Msg.printf( L"[Receive HeartBeatMsg] [issue_type:%d] SendBizMsgMsg Reply HeartBeat packet OK", lpBizMessage->GetIssueType() );
			TradingForm->Memo1->Lines->Add( Msg );
		}
	}
	lpBizMessage->Release();
}
//---------------------------------------------------------------------------
unsigned long FUNCTION_CALL_MODE HSufxListener::QueryInterface(const char *iid, IKnown **ppv)
{
	return 0;
}
//---------------------------------------------------------------------------
unsigned long FUNCTION_CALL_MODE HSufxListener::AddRef()
{
	return 0;
}
//---------------------------------------------------------------------------
unsigned long FUNCTION_CALL_MODE HSufxListener::Release()
{
	return 0;
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxListener::OnConnect(CConnectionInterface *lpConnection)
{
	String Msg;
	Msg.printf( L"******< OnConnect >****** [thdid:%d]", ::GetCurrentThreadId() );
	TradingForm->Memo1->Lines->Add( Msg );
	TradingForm->BtnConnStatusClick( NULL );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxListener::OnSafeConnect(CConnectionInterface *lpConnection)
{
	String Msg;
	Msg.printf( L"******< OnSafeConnect >****** [thdid:%d]", ::GetCurrentThreadId() );
	TradingForm->Memo1->Lines->Add( Msg );
	TradingForm->BtnConnStatusClick( NULL );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxListener::OnRegister(CConnectionInterface *lpConnection)
{
	String Msg;
	Msg.printf( L"******< OnRegister >****** [thdid:%d]", ::GetCurrentThreadId() );
	TradingForm->Memo1->Lines->Add( Msg );
	TradingForm->BtnConnStatusClick( NULL );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxListener::OnClose(CConnectionInterface *lpConnection)
{
	String Msg;
	Msg.printf( L"******< OnClose >****** [thdid:%d]", ::GetCurrentThreadId() );
	TradingForm->Memo1->Lines->Add( Msg );
	TradingForm->BtnConnStatusClick( NULL );
	TradingForm->Memo1->Lines->Add( "<<<<< Client Disconnected >>>>>" );
	// Disable Buttons
	TradingForm->SetButtons( false );
	// !! Can not release connection in OnClose
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxListener::OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData)
{
	String Msg;
	Msg.printf( L"******< OnSent >****** [thdid:%d]", ::GetCurrentThreadId() );
	TradingForm->Memo1->Lines->Add( Msg );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxListener::Reserved1(void *a, void *b, void *c, void *d)
{
	TradingForm->Memo1->Lines->Add( "******< Reserved1 >******" );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxListener::Reserved2(void *a, void *b, void *c, void *d)
{
	TradingForm->Memo1->Lines->Add( "******< Reserved2 >******" );
}
//---------------------------------------------------------------------------
int  FUNCTION_CALL_MODE HSufxListener::Reserved3()
{
	TradingForm->Memo1->Lines->Add( "******< Reserved3 >******" );
	return 0;
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxListener::Reserved4()
{
	TradingForm->Memo1->Lines->Add( "******< Reserved4 >******" );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxListener::Reserved5()
{
	TradingForm->Memo1->Lines->Add( "******< Reserved5 >******" );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxListener::Reserved6()
{
	TradingForm->Memo1->Lines->Add( "******< Reserved6 >******" );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxListener::Reserved7()
{
	TradingForm->Memo1->Lines->Add( "******< Reserved7 >******" );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxListener::OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult)
{
	TradingForm->Memo1->Lines->Add( "******< OnReceivedBiz >******" );
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxListener::OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult)
{
	if ( lpRetData == NULL )
	{
		TradingForm->Memo1->Lines->Add( "OnReceivedBizEx Error!! lpRetData is NULL" );
		return;
	}

	String Msg;
	int iFuncID     = lpRetData->functionID;
	int iIssueType  = lpRetData->issueType;
	int iErrorNo    = lpRetData->errorNo;
	int iReturnCode = lpRetData->returnCode;


	if ( TradingForm->ChkShowHB->Checked == true || iFuncID != 620000 )
	{
		DWORD thdid = ::GetCurrentThreadId();
		Msg.printf( L"******< OnReceivedBizEx >****** [functionID:%d][hSend:%d][nResult:%d][errorNo:%d][returnCode:%d][errorInfo:%s][thdid:%d]"
					, iFuncID, hSend, nResult, iErrorNo, iReturnCode, String(GB2312String(lpRetData->errorInfo)), thdid );
		TradingForm->Memo1->Lines->Add( Msg );
	}

	// 1. Check Result
	switch ( nResult )
	{
	case 0 : ///< 表示業務數據接收成功，並且業務操作成功，lpUnpackerOrStr指向一個解包器
	case 4 : ///< 表示業務包為空，lpUnpackerOrStr指向 NULL。lpRetData這部分結果還會存在
		break;
	case 1 : ///< 表示業務數據接收成功，但業務操作失敗了，lpUnpackerOrStr指向一個解包器
		TradingForm->RecvBizEx( "RecvBizEx", lpRetData, lpUnpackerOrStr );
		return;
	case 2 : ///< 表示收到非業務錯誤信息，lpUnpackerOrStr指向一個可讀的字符串錯誤信息
		TradingForm->Memo1->Lines->Add( String(GB2312String((char*)lpUnpackerOrStr)) );
		return;
	case 3 : ///< 表示業務包解包失敗，lpUnpackerOrStr指向 NULL
		TradingForm->Memo1->Lines->Add( "Business Unpack Error!!" ); ///< 業務解包失敗
		return;
	default :
		if ( iFuncID == 620000 )
			TradingForm->Memo1->Lines->Add( L"[Receive HeartBeat]" );
		else
			TradingForm->Memo1->Lines->Add( L"!! Unhandle nResult !!" + String(nResult) );
		return;
	}

	// 2. Check errorNo
	//if ( lpRetData->errorNo != 0 || lpRetData->returnCode != 0 )
	if ( lpRetData->errorNo != 0 )
	{
		Msg.printf( L"OnReceivedBizEx return Error!! [errorNo:%d] GetErrorMsg[%s] [returnCode:%d] [errorInfo:%s]"
					, iErrorNo, String(GB2312String(lpConnection->GetErrorMsg(lpRetData->returnCode)))
					, iReturnCode, String(GB2312String(lpRetData->errorInfo)) );
		TradingForm->Memo1->Lines->Add( Msg );
		return;
	}

	// 3. Check Business functionID
	switch ( iFuncID )
	{
	///< Stock Options Biz functions
	case 338011 : ///< [func_opt_entrust] 		///< New Order (single row)
		TradingForm->RecvNewOrder( lpRetData, lpUnpackerOrStr );
		break;
	case 338012 : ///< [func_opt_withdraw] 		///< Cancel Order (single row)
		TradingForm->RecvBizEx( "RecvCancelOrder", lpRetData, lpUnpackerOrStr );
		break;
	case 620001 : ///< [subscribe]				///< Subscribe Push Report (single row)
		TradingForm->RecvBizEx( "RecvRegPush", lpRetData, lpUnpackerOrStr );
		Msg.printf( L"[Receive Subscribe] [issue_type:%d] [%s]", iIssueType, iIssueType==33011 ? String("ReportFill") : String("ReportConfirm") );
		TradingForm->Memo1->Lines->Add( Msg );
		break;
	case 620003 : ///< [subscribe data report]	///< 33011-Fill , 33012-OrderConfirm (single row)
		Msg.printf( L"[Recevie %s] [issue_type:%d]", iIssueType==33011 ? String("ReportFill") : String("ReportConfirm"), iIssueType );
		TradingForm->Memo1->Lines->Add( Msg );
		TradingForm->RecvBizEx( "RecvReport", lpRetData, lpUnpackerOrStr );
		break;
	case 338020 : ///< [func_opt_entrust_qry]	///< Query Order (multi row)
		TradingForm->RecvBizEx( "RecvQryOrder", lpRetData, lpUnpackerOrStr );
		break;
	case 338021 : ///< [func_opt_realtime_qry]	///< Query Fill (multi row)
		TradingForm->RecvBizEx( "RecvQryFill", lpRetData, lpUnpackerOrStr );
		break;
	case 338023 : ///< [func_opt_hold_qry]		///< Query Hold Postion (multi row)
		TradingForm->RecvBizEx( "RecvQryPosition", lpRetData, lpUnpackerOrStr );
		break;
	case 338022 : ///< [func_opt_asset_qry]		///< Query Asset (single row)
		TradingForm->RecvBizEx( "RecvQryAccount", lpRetData, lpUnpackerOrStr );
		break;
	case 338000 : ///< [func_opt_optcode_qry]	///< Query Symbol base data (multi row)
		TradingForm->RecvBizEx( "RecvQrySymbol", lpRetData, lpUnpackerOrStr );
		break;
	case 338001 : ///< [func_opt_optcode_qry]	///< Query Underling data (multi row)
		TradingForm->RecvBizEx( "RecvQryUnderling", lpRetData, lpUnpackerOrStr );
		break;
	///< General Biz functions
	case 620000 : ///< []						///< HeartBeat
		//RecvHeartBeat( lpRetData );
		TradingForm->RecvHeartBeat( lpRetData, lpUnpackerOrStr );
		break;
	case 331100 : ///< [func_client_login]		///< Login (single row)
		TradingForm->RecvLogin( lpRetData, lpUnpackerOrStr );
		break;
	case 331101 : ///< [func_client_pwd_mod]	///< Change Password (single row)
		TradingForm->RecvBizEx( "RecvChangePwd", lpRetData, lpUnpackerOrStr );
		break;
	case 331102 : ///< [func_client_exit]		///< Logout (single row)
		TradingForm->RecvBizEx( "RecvLogout", lpRetData, lpUnpackerOrStr );
		break;
	case 331156 : ///< func_client_farerate_qry (Query Commissition) (multi row)
		TradingForm->RecvBizEx( "RecvQryCommission", lpRetData, lpUnpackerOrStr );
		break;
	/// Unhandle(or Unknow) Biz functions
	default :
		TradingForm->Memo1->Lines->Add( L"!! Unhandle functionID !! " + String(iFuncID) );
	}
}
//---------------------------------------------------------------------------
void FUNCTION_CALL_MODE HSufxListener::OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg)
{
	if ( lpMsg == NULL )
	{
		TradingForm->Memo1->Lines->Add( "OnReceivedBizMsg Error!! lpMsg is NULL" );
		return;
	}

	String Msg;
	int    iFuncID     = lpMsg->GetFunction();
	int    iIssueType  = lpMsg->GetIssueType();
	int    iErrorNo    = lpMsg->GetErrorNo();
	int    iReturnCode = lpMsg->GetReturnCode();

	if ( TradingForm->ChkShowHB->Checked == true || iFuncID != 620000 )
	{
		DWORD thdid = ::GetCurrentThreadId();
		Msg.printf( L"******< OnReceivedBizMsg >****** [functionID:%d][hSend:%d][errorNo:%d][returnCode:%d][errorInfo:%s][thdid:%d]"
					, iFuncID, hSend, iErrorNo, iReturnCode, String(GB2312String(lpMsg->GetErrorInfo())), thdid );
		TradingForm->Memo1->Lines->Add( Msg );
	}

	// 1. Check ErrorNo
	if ( iErrorNo != 0 )
	{
		Msg.printf( L"OnReceivedBizMsg return Error!! [ErrorNo:%d] GetErrorInfo[%s] [ReturnCode:%d]"
					, iErrorNo, String(GB2312String(lpMsg->GetErrorInfo())), iReturnCode );
		TradingForm->Memo1->Lines->Add( Msg );
		return;
	}

	// 3. Check Business functionID
	switch ( iFuncID )
	{
	///< Stock Options Biz functions
	case 338011 : ///< [func_opt_entrust] 		///< New Order (single row)
		TradingForm->RecvBizMsgNewOrder( lpMsg );
		break;
	case 338012 : ///< [func_opt_withdraw] 		///< Cancel Order (single row)
		TradingForm->RecvBizMsg( "RecvCancelOrder", lpMsg );
		break;
	case 620001 : ///< [subscribe]				///< Subscribe Push Report (single row)
		TradingForm->RecvBizMsg( "RecvRegPush", lpMsg, true );
		Msg.printf( L"[Receive Subscribe] [issue_type:%d] [%s]", iIssueType, iIssueType==33011 ? String("ReportFill") : String("ReportConfirm") );
		TradingForm->Memo1->Lines->Add( Msg );
		break;
	case 620003 : ///< [subscribe data report]	///< 33011-Fill , 33012-OrderConfirm (single row)
		Msg.printf( L"[Recevie %s] [issue_type:%d]", iIssueType==33011 ? String("ReportFill") : String("ReportConfirm"), iIssueType );
		TradingForm->Memo1->Lines->Add( Msg );
		TradingForm->RecvBizMsg( "RecvReport", lpMsg, true );
		break;
	case 338020 : ///< [func_opt_entrust_qry]	///< Query Order (multi row)
		TradingForm->RecvBizMsg( "RecvQryOrder", lpMsg );
		break;
	case 338021 : ///< [func_opt_realtime_qry]	///< Query Fill (multi row)
		TradingForm->RecvBizMsg( "RecvQryFill", lpMsg );
		break;
	case 338023 : ///< [func_opt_hold_qry]		///< Query Hold Postion (multi row)
		TradingForm->RecvBizMsg( "RecvQryPosition", lpMsg );
		break;
	case 338022 : ///< [func_opt_asset_qry]		///< Query Asset (single row)
		TradingForm->RecvBizMsg( "RecvQryAccount", lpMsg );
		break;
	case 338000 : ///< [func_opt_optcode_qry]	///< Query Symbol base data (multi row)
		TradingForm->RecvBizMsg( "RecvQrySymbol", lpMsg );
		break;
	case 338001 : ///< [func_opt_optcode_qry]	///< Query Underling data (multi row)
		TradingForm->RecvBizMsg( "RecvQryUnderling", lpMsg );
		break;
	///< General Biz functions
	case 620000 : ///< []						///< HeartBeat
		//RecvHeartBeatMsg( lpMsg );
		TradingForm->RecvBizMsgHeartBeat( lpMsg );
		break;
	case 331100 : ///< [func_client_login]		///< Login (single row)
		TradingForm->RecvBizMsgLogin( lpMsg );
		break;
	case 331101 : ///< [func_client_pwd_mod]	///< Change Password (single row)
		TradingForm->RecvBizMsg( "RecvChangePwd", lpMsg );
		break;
	case 331102 : ///< [func_client_exit]		///< Logout (single row)
		TradingForm->RecvBizMsg( "RecvLogout", lpMsg );
		break;
	case 331156 : ///< func_client_farerate_qry (Query Commissition) (multi row)
		TradingForm->RecvBizMsg( "RecvQryCommission", lpMsg );
		break;
	/// Unhandle(or Unknow) Biz functions
	default :
		TradingForm->Memo1->Lines->Add( L"!! Unhandle functionID !! " + String(iFuncID) );
	}
}
//---------------------------------------------------------------------------
__fastcall TTradingForm::TTradingForm(TComponent* Owner)
	: TForm(Owner)
	, FUfxConfig( NULL )
	, FUfxConnection( NULL )
	, FListener( NULL )
{
	String Msg;
	DWORD thdid = ::GetCurrentThreadId();
	Msg.printf( L"TTradingForm::TTradingForm [thdid:%d]", thdid );
	TradingForm->Memo1->Lines->Add( Msg );

	FStrings = new TStringList();

	// Set Screen
	GetGlobalVariant( );
	SetButtons( false );

	String Version;
	//Version.printf( L"%x", GetVersionInfo() );
	Version.printf( L"%x", gHSufxAPI.pGetVersionInfo() );
	TxtVersionInfo->Text = Version;

	// Create UFX CConfigInterface
	//通過T2SDK的引出函數，來獲取一個新的CConfig對象
	//此對象在創建連接對象時被傳遞，用於配置所創建的連接對象的各種屬性（比如服務器IP地址、安全模式等）
	//值得注意的是，在向配置對象設置配置信息時，配置信息既可以從ini文件中載入，
	//也可以在程序代碼中設定，或者是2者的混合，如果對同一個配置項設不同的值，則以最近一次設置為準
	//FUfxConfig = NewConfig();
	FUfxConfig = gHSufxAPI.pNewConfig();

	//通過T2SDK的引出函數NewXXXX返回的對象，需要調用對象的Release方法釋放，而不能直接用delete
	//因為t2sdk.dll和調用程序可能是由不同的編譯器、編譯模式生成，delete可能會導致異常
	//為了適合Delphi等使用（Delphi對接口自動調用AddRef方法），用C/C++開發的代碼，需要在NewXXXX之後調用一下AddRef
	//以保證引用計數正確
	FUfxConfig->AddRef();
	//FUfxConfig->Load( "t2sdk.ini" );
}
//---------------------------------------------------------------------------
void TTradingForm::GetGlobalVariant( void )
{
	TxtBrokerID->Text      = String(gBrokerID);
	TxtUserID->Text        = gUserID.c_str();
	TxtPassword->Text      = gPassword.c_str();
	TxtServerAddress->Text = gServerAddress.c_str();
}
//---------------------------------------------------------------------------
void TTradingForm::SetGlobalVariant( void )
{
	gBrokerID      = TxtBrokerID->Text.ToInt();
	gUserID        = TxtUserID->Text.Trim();
	gPassword      = TxtPassword->Text.Trim();
	gServerAddress = TxtServerAddress->Text.Trim();
}
//---------------------------------------------------------------------------
void TTradingForm::SetUfxConfig( void )
{
	///可以在代碼中對lpConfig對像中的具體值進行設置
	//設置以最後一次為準

	//[t2sdk] servers指定需要連接的IP地址及端口，可配置多個，中間以“;”間隔//111.207.179.6:8066  18.8.1.171:8066
	//lpConfig->SetString("t2sdk", "servers", "192.168.51.162:3335");

	//[t2sdk] license_file指定許可證文件路徑 HS-HUNDSUN001-FBASE2-0000_3rd.dat
	//lpConfig->SetString("t2sdk", "license_file", "(20130306)tyjrnb-TYJRTYJR-ALL-0001_3rd.dat");

	//[t2sdk] lang指定錯誤信息的語言號（缺省為簡體中文2052），1033為英文
	//lpConfig->SetString("t2sdk", "lang", "1033");

	//[t2sdk] send_queue_size指定T2_SDK的發送隊列大小
	//lpConfig->SetString("t2sdk", "send_queue_size", "100");

	//[t2sdk] send_queue_size指定T2_SDK的發送隊列大小
	//lpConfig->SetString("t2sdk", "license_pwd", "888888");

	//[safe] safe_level指定連接的安全模式，需要和T2通道的安全模式一致，否則連接失敗
	//lpConfig->SetString("safe", "safe_level", "ssl");

	//[safe]
	//lpConfig->SetString("safe", "cert_file", "c20121011.pfx");

	//[safe]
	//lpConfig->SetString("safe", "cert_pwd", "111111");
	IsMultiThread = True;
	FUfxConfig->SetString( "t2sdk", "servers", gServerAddress.c_str() );
	FUfxConfig->SetString( "t2sdk", "send_queue_size", "100" );
	//FUfxConfig->SetString( "t2sdk", "support_multi", "1" ); ///< support multi-thread
	FUfxConfig->SetString( "t2sdk", "enable_nagle", "1" ); ///< disable nagle algorithm
	//FUfxConfig->SetString( "t2sdk", "heartbeat_time", "5" ); ///< enable client side send heartbeat second
	//FUfxConfig->SetString( "t2sdk", "event_count", "10" );
	// 長江 UFX
	//FUfxConfig->SetString( "t2sdk", "license_file", "HSt2sdkLicense.dat" );
	//FUfxConfig->SetString( "t2sdk", "license_pwd", "123456" );
	//FUfxConfig->SetString( "safe",  "safe_level", "none" ); ///< "none" "ssl"

	// 恒生 UFX
	FUfxConfig->SetString( "t2sdk", "login_name", "192.168.17.1-super8" );
	FUfxConfig->SetString( "t2sdk", "license_file", "(20130306)tyjrnb-TYJRTYJR-ALL-0001_3rd.dat" );
	//FUfxConfig->SetString( "t2sdk", "license_pwd", "888888" );
	FUfxConfig->SetString( "safe",  "safe_level", "ssl" ); ///< "none" "ssl"
	FUfxConfig->SetString( "safe",  "cert_file", "c20121011.pfx" );
	FUfxConfig->SetString( "safe",  "cert_pwd", "111111" );
}
//---------------------------------------------------------------------------
void TTradingForm::ShowPacket( BOOL IsUnPackerData, IF2UnPacker* pUnPacker )
{
	int i, t, j, k;
	String Line, Msg;
	for ( i=0; i < pUnPacker->GetDatasetCount(); i++ )
	{
		pUnPacker->SetCurrentDatasetByIndex(i);

		// print column name
		Line = "";
		for ( t=0; t < pUnPacker->GetColCount(); t++ )
		{
			Msg.printf( L"%20s ", String(pUnPacker->GetColName(t)) );
			Line += Msg;
		}
		Memo1->Lines->Add( Line );
		Line = "";
		for ( t=0; t < pUnPacker->GetColCount(); t++ )
		{
			if ( IsUnPackerData )
				Msg = " ____________________";
			else
				Msg = " ====================";
			Line += Msg;
		}
		Memo1->Lines->Add( Line );

		// print row
		if ( pUnPacker->GetRowCount() == 0 )
		{
			Memo1->Lines->Add( "0 row, no data." );
			continue;
		}
		for ( j=0; j < pUnPacker->GetRowCount(); j++ )
		{
			Line = "";
			// print column value
			for ( k=0; k < pUnPacker->GetColCount(); k++ )
			{
				char ColType = pUnPacker->GetColType(k);
				if ( ColType == 'I' )
					Msg.printf( L"%20d ", pUnPacker->GetIntByIndex(k) );
				else if ( ColType == 'C' )
					Msg.printf( L"%20c ", pUnPacker->GetCharByIndex(k) );
				else if ( ColType == 'S' )
				{
					// AnsiToUtf8Ex() IsUTF8String() UTF8ToString() UTF8ToUnicodeString
					// work
					UTF8String UTF8Str( AnsiToUtf8Ex(pUnPacker->GetStrByIndex(k), 936) );
					// work
					//GB2312String GBStr( pUnPacker->GetStrByIndex(k) );
					//UTF8String   UTF8Str( GBStr );
					// work
					//UTF8String UTF8Str( GB2312String(pUnPacker->GetStrByIndex(k)) );
					// no work
					//UTF8String UTF8Str( GB2312String(pUnPacker->GetStrByIndex(k)).c_str() );

					Msg.printf( L"%20s ", String(UTF8ToString(UTF8Str.c_str())) );
					//Msg.printf( L"%20s ", String(GB2312String(pUnPacker->GetStrByIndex(k))) );
				}
				else if ( ColType == 'F' )
					Msg.printf( L"%20.4f ", pUnPacker->GetDoubleByIndex(k) );
				else if ( ColType == 'R' )
					Msg = "<<<<RawData>>>>";
				else
					Msg = "<<Unknow Type>>";
/*
				switch ( pUnPacker->GetColType(k) )
				{
				case 'I': Msg.printf( L"%20d ", pUnPacker->GetIntByIndex(k) ); break;
				case 'C': Msg.printf( L"%20c ", pUnPacker->GetCharByIndex(k) ); break;
				case 'S': Msg.printf( L"%20s ", String(GB2312String(pUnPacker->GetStrByIndex(k))) ); break;
				//case 'S': Msg.printf( L"%20s ", UnicodeString(GB2312String(pUnPacker->GetStrByIndex(k))).c_str() ); break;
				case 'F': Msg.printf( L"%20.2f ", pUnPacker->GetDoubleByIndex(k) ); break;
				case 'R': Msg = "<<<<RawData>>>>"; break;
				default:  Msg = "<<Unknow Type>>"; break;
				}
*/
				Line += Msg;
			}
			Memo1->Lines->Add( Line );
			pUnPacker->Next();
		}
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendNewOrder( void )
{
	int		RtnVal;
	String	Msg;

	// 1. New Packer
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendNewOrder() NewPacker() failed!!" );
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 10 );
	pPacker->AddField( "password_type", 'C', 1 );
	pPacker->AddField( "user_token", 'S', 40 );
	pPacker->AddField( "asset_prop", 'C', 1 );
	pPacker->AddField( "exchange_type", 'S', 4 );
	pPacker->AddField( "option_code", 'S', 8 );
	pPacker->AddField( "entrust_amount", 'F', 16, 2 );
	pPacker->AddField( "opt_entrust_price", 'F', 11, 4 );
	pPacker->AddField( "entrust_bs", 'C', 1 );
	pPacker->AddField( "entrust_oc", 'C', 1 );
	pPacker->AddField( "covered_flag", 'C', 1 );
	pPacker->AddField( "entrust_prop", 'S', 3 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );				///< op_branch_no
	pPacker->AddChar( '7' );					///< op_entrust_way
	pPacker->AddStr( "1" );						///< op_station
	pPacker->AddInt( gBrokerID );				///< branch_no
	pPacker->AddStr( FClientID );				///< client_id
	pPacker->AddStr( gUserID.c_str() );			///< fund_account
	pPacker->AddStr( gPassword.c_str() );		///< password
	pPacker->AddChar( '2' );					///< password_type
	pPacker->AddStr( FUserToken );				///< user_token
	pPacker->AddChar( 'B' );					///< asset_prop
	pPacker->AddStr( "1" );						///< exchange_type
	pPacker->AddStr( AnsiString(TxtSymbol->Text).c_str() );	///< option_code
	pPacker->AddDouble( TxtQuantity->Text.ToDouble() );		///< entrust_amount
	pPacker->AddDouble( TxtPrice->Text.ToDouble() );		///< opt_entrust_price
	// entrust_bs
	if ( TxtBS->Text == "B" )
		pPacker->AddChar( '1' );
	else
		pPacker->AddChar( '2' );
	pPacker->AddChar( AnsiString(TxtOC->Text).c_str()[0] );			///< entrust_oc
	pPacker->AddChar( ' ' );					///< covered_flag
	pPacker->AddStr( AnsiString(TxtOrderCondition->Text).c_str() );	///< entrust_prop
	pPacker->EndPack();

	// 4. Send Packet
	RtnVal = FUfxConnection->SendBiz( 338011, pPacker, HSufx_ASyncSend, gSysNodeID );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBiz SendNewOrder packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
		return;
	}
	else
	{
		Msg.printf( L">>>>>> SendNewOrder SendBiz function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendNewOrderMsg( void )
{
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendNewOrderMsg() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338011 );
	lpBizMessage->SetSystemNo( gSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 1. New Packer
	pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendNewOrderMsg() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 10 );
	pPacker->AddField( "password_type", 'C', 1 );
	pPacker->AddField( "user_token", 'S', 40 );
	pPacker->AddField( "asset_prop", 'C', 1 );
	pPacker->AddField( "exchange_type", 'S', 4 );
	pPacker->AddField( "option_code", 'S', 8 );
	pPacker->AddField( "entrust_amount", 'F', 16, 2 );
	pPacker->AddField( "opt_entrust_price", 'F', 11, 4 );
	pPacker->AddField( "entrust_bs", 'C', 1 );
	pPacker->AddField( "entrust_oc", 'C', 1 );
	pPacker->AddField( "covered_flag", 'C', 1 );
	pPacker->AddField( "entrust_prop", 'S', 3 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );				///< op_branch_no
	pPacker->AddChar( '7' );					///< op_entrust_way
	pPacker->AddStr( "1" );						///< op_station
	pPacker->AddInt( gBrokerID );				///< branch_no
	pPacker->AddStr( FClientID );				///< client_id
	pPacker->AddStr( gUserID.c_str() );			///< fund_account
	pPacker->AddStr( gPassword.c_str() );		///< password
	pPacker->AddChar( '2' );					///< password_type
	pPacker->AddStr( FUserToken );				///< user_token
	pPacker->AddChar( 'B' );					///< asset_prop
	pPacker->AddStr( "1" );						///< exchange_type
	pPacker->AddStr( AnsiString(TxtSymbol->Text).c_str() );	///< option_code
	pPacker->AddDouble( TxtQuantity->Text.ToDouble() );		///< entrust_amount
	pPacker->AddDouble( TxtPrice->Text.ToDouble() );		///< opt_entrust_price
	// entrust_bs
	if ( TxtBS->Text == "B" )
		pPacker->AddChar( '1' );
	else
		pPacker->AddChar( '2' );
	pPacker->AddChar( AnsiString(TxtOC->Text).c_str()[0] );			///< entrust_oc
	pPacker->AddChar( ' ' );					///< covered_flag
	pPacker->AddStr( AnsiString(TxtOrderCondition->Text).c_str() );	///< entrust_prop
	pPacker->EndPack();

	// 4. Send Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FUfxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBizMsg SendNewOrderMsg packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> SendNewOrderMsg SendBizMsg function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendCancelOrder( void )
{
	int		RtnVal;
	String	Msg;

	// 1. New Packer
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendCancelOrder() NewPacker() failed!!" );
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 10 );
	pPacker->AddField( "password_type", 'C', 1 );
	pPacker->AddField( "user_token", 'S', 40 );
	pPacker->AddField( "asset_prop", 'C', 1 );
	pPacker->AddField( "exchange_type", 'S', 4 );
	pPacker->AddField( "entrust_no", 'I', 8 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );				///< op_branch_no
	pPacker->AddChar( '7' );					///< op_entrust_way
	pPacker->AddStr( "1" );						///< op_station
	pPacker->AddInt( gBrokerID );				///< branch_no
	pPacker->AddStr( FClientID );				///< client_id
	pPacker->AddStr( gUserID.c_str() );			///< fund_account
	pPacker->AddStr( gPassword.c_str() );		///< password
	pPacker->AddChar( '2' );					///< password_type
	pPacker->AddStr( FUserToken );				///< user_token
	pPacker->AddChar( 'B' );					///< asset_prop
	pPacker->AddStr( "1" );						///< exchange_type
	pPacker->AddInt( TxtLastOrderNo->Text.ToInt() );	///< entrust_no
	pPacker->EndPack();

	// 4. Send Packet
	RtnVal = FUfxConnection->SendBiz( 338012, pPacker, HSufx_ASyncSend, gSysNodeID );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBiz SendCancelOrder packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
		return;
	}
	else
	{
		Msg.printf( L">>>>>> SendCancelOrder SendBiz function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendCancelOrderMsg( void )
{
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendCancelOrderMsg() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338012 );
	lpBizMessage->SetSystemNo( gSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 1. New Packer
	pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendCancelOrderMsg() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 10 );
	pPacker->AddField( "password_type", 'C', 1 );
	pPacker->AddField( "user_token", 'S', 40 );
	pPacker->AddField( "asset_prop", 'C', 1 );
	pPacker->AddField( "exchange_type", 'S', 4 );
	pPacker->AddField( "entrust_no", 'I', 8 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );				///< op_branch_no
	pPacker->AddChar( '7' );					///< op_entrust_way
	pPacker->AddStr( "1" );						///< op_station
	pPacker->AddInt( gBrokerID );				///< branch_no
	pPacker->AddStr( FClientID );				///< client_id
	pPacker->AddStr( gUserID.c_str() );			///< fund_account
	pPacker->AddStr( gPassword.c_str() );		///< password
	pPacker->AddChar( '2' );					///< password_type
	pPacker->AddStr( FUserToken );				///< user_token
	pPacker->AddChar( 'B' );					///< asset_prop
	pPacker->AddStr( "1" );						///< exchange_type
	pPacker->AddInt( TxtLastOrderNo->Text.ToInt() );	///< entrust_no
	pPacker->EndPack();

	// 4. Send Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FUfxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBizMsg SendCancelOrderMsg packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> SendCancelOrderMsg SendBizMsg function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendQryOrder( void )
{
	int		RtnVal;
	String	Msg;

	// 1. New Packer
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendQryOrder() NewPacker() failed!!" );
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< Require
	pPacker->AddField( "client_id", 'S', 18 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	pPacker->AddField( "password", 'S', 10 );		///< Require
	pPacker->AddField( "password_type", 'C', 1 );	///< NonRequire
	pPacker->AddField( "user_token", 'S', 40 );		///< NonRequire
	pPacker->AddField( "asset_prop", 'C', 1 );		///< Require
	pPacker->AddField( "query_type", 'C', 1 );		///< NonRequire
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );				///< op_branch_no
	pPacker->AddChar( '7' );					///< op_entrust_way
	pPacker->AddStr( "1" );						///< op_station
	pPacker->AddInt( gBrokerID );				///< branch_no
	pPacker->AddStr( FClientID );				///< client_id
	pPacker->AddStr( gUserID.c_str() );			///< fund_account
	pPacker->AddStr( gPassword.c_str() );		///< password
	pPacker->AddChar( '2' );					///< password_type
	pPacker->AddStr( FUserToken );				///< user_token
	pPacker->AddChar( 'B' );					///< asset_prop
	pPacker->AddChar( '1' );					///< query_type
	pPacker->EndPack();

	// 4. Send Packet
	RtnVal = FUfxConnection->SendBiz( 338020, pPacker, HSufx_ASyncSend, gSysNodeID );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBiz SendQryOrder packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
		return;
	}
	else
	{
		Msg.printf( L">>>>>> SendQryOrder SendBiz function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendQryOrderMsg( void )
{
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendQryOrderMsg() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338020 );
	lpBizMessage->SetSystemNo( gSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 1. New Packer
	pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendQryOrderMsg() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< Require
	pPacker->AddField( "client_id", 'S', 18 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	pPacker->AddField( "password", 'S', 10 );		///< Require
	pPacker->AddField( "password_type", 'C', 1 );	///< NonRequire
	pPacker->AddField( "user_token", 'S', 40 );		///< NonRequire
	pPacker->AddField( "asset_prop", 'C', 1 );		///< Require
	pPacker->AddField( "query_type", 'C', 1 );		///< NonRequire
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );				///< op_branch_no
	pPacker->AddChar( '7' );					///< op_entrust_way
	pPacker->AddStr( "1" );						///< op_station
	pPacker->AddInt( gBrokerID );				///< branch_no
	pPacker->AddStr( FClientID );				///< client_id
	pPacker->AddStr( gUserID.c_str() );			///< fund_account
	pPacker->AddStr( gPassword.c_str() );		///< password
	pPacker->AddChar( '2' );					///< password_type
	pPacker->AddStr( FUserToken );				///< user_token
	pPacker->AddChar( 'B' );					///< asset_prop
	pPacker->AddChar( '1' );					///< query_type
	pPacker->EndPack();

	// 4. Send Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FUfxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBizMsg SendQryOrderMsg packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> SendQryOrderMsg SendBizMsg function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendQryFill( void )
{
	int		RtnVal;
	String	Msg;

	// 1. New Packer
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendQryFill() NewPacker() failed!!" );
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 10 );
	pPacker->AddField( "password_type", 'C', 1 );
	pPacker->AddField( "user_token", 'S', 40 );
	pPacker->AddField( "asset_prop", 'C', 1 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );				///< op_branch_no
	pPacker->AddChar( '7' );					///< op_entrust_way
	pPacker->AddStr( "1" );						///< op_station
	pPacker->AddInt( gBrokerID );				///< branch_no
	pPacker->AddStr( FClientID );				///< client_id
	pPacker->AddStr( gUserID.c_str() );			///< fund_account
	pPacker->AddStr( gPassword.c_str() );		///< password
	pPacker->AddChar( '2' );					///< password_type
	pPacker->AddStr( FUserToken );				///< user_token
	pPacker->AddChar( 'B' );					///< asset_prop
	pPacker->EndPack();

	// 4. Send Packet
	RtnVal = FUfxConnection->SendBiz( 338021, pPacker, HSufx_ASyncSend, gSysNodeID );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBiz SendQryFill packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
		return;
	}
	else
	{
		Msg.printf( L">>>>>> SendQryFill SendBiz function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendQryFillMsg( void )
{
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendQryFillMsg() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338021 );
	lpBizMessage->SetSystemNo( gSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 1. New Packer
	pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendQryFillMsg() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 10 );
	pPacker->AddField( "password_type", 'C', 1 );
	pPacker->AddField( "user_token", 'S', 40 );
	pPacker->AddField( "asset_prop", 'C', 1 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );				///< op_branch_no
	pPacker->AddChar( '7' );					///< op_entrust_way
	pPacker->AddStr( "1" );						///< op_station
	pPacker->AddInt( gBrokerID );				///< branch_no
	pPacker->AddStr( FClientID );				///< client_id
	pPacker->AddStr( gUserID.c_str() );			///< fund_account
	pPacker->AddStr( gPassword.c_str() );		///< password
	pPacker->AddChar( '2' );					///< password_type
	pPacker->AddStr( FUserToken );				///< user_token
	pPacker->AddChar( 'B' );					///< asset_prop
	pPacker->EndPack();

	// 4. Send Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FUfxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBizMsg SendQryFillMsg packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> SendQryFillMsg SendBizMsg function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendLogin( void )
{
	int		RtnVal;
	String	Msg;

	// 1. New Packer
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendLogin() NewPacker() failed!!" );
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< NonRequire
	pPacker->AddField( "password", 'S', 10 );		///< Require
	pPacker->AddField( "password_type", 'C', 1 );	///< NonRequire
	pPacker->AddField( "input_content", 'C', 1 );	///< Require
	pPacker->AddField( "account_content", 'S', 30 );///< Require
	pPacker->AddField( "content_type", 'S', 6 );	///< Require
	// 3. Add Filed Value
	pPacker->AddInt( 0 );					///< op_branch_no
	//pPacker->AddChar( 'L' );				///< op_entrust_way
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( 0 );					///< branch_no
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddChar( '2'/*'\0'*/ );		///< password_type
	pPacker->AddChar( '1' );				///< input_content
	pPacker->AddStr( gUserID.c_str() );		///< account_content
	pPacker->AddStr( "0" );					///< content_type
	pPacker->EndPack();
	Msg.printf( L"Login...Account[%s]", String(gUserID.c_str()) );
	Memo1->Lines->Add( Msg );

	// 4. Send Packet
	RtnVal = FUfxConnection->SendBiz( 331100, pPacker, HSufx_ASyncSend, 0 );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBiz Login packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
		return;
	}
	else
	{
		Msg.printf( L">>>>>> Login SendBiz function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendLoginMsg( void )
{
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendLoginMsg() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 331100 );
	//lpBizMessage->SetSubSystemNo( 0 );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 1. New Packer
	pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendLoginMsg() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< NonRequire
	pPacker->AddField( "password", 'S', 10 );		///< Require
	pPacker->AddField( "password_type", 'C', 1 );	///< NonRequire
	pPacker->AddField( "input_content", 'C', 1 );	///< Require
	pPacker->AddField( "account_content", 'S', 30 );///< Require
	pPacker->AddField( "content_type", 'S', 6 );	///< Require
	// 3. Add Filed Value
	pPacker->AddInt( 0 );					///< op_branch_no
	//pPacker->AddChar( 'L' );				///< op_entrust_way
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( 0 );					///< branch_no
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddChar( '2'/*'\0'*/ );		///< password_type
	pPacker->AddChar( '1' );				///< input_content
	pPacker->AddStr( gUserID.c_str() );		///< account_content
	pPacker->AddStr( "0" );					///< content_type
	pPacker->EndPack();
	Msg.printf( L"Login...Account[%s]", String(gUserID.c_str()) );
	Memo1->Lines->Add( Msg );

	// 4. Send Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FUfxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBizMsg Login packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> Login SendBizMsg function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendLogout( void )
{
	int		RtnVal;
	String	Msg;

	// 1. New Packer
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendLogout() NewPacker() failed!!" );
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< NonRequire
	pPacker->AddField( "client_id", 'S',18 );		///< Require
	pPacker->AddField( "fund_account", 'S',18 );	///< Require
	pPacker->AddField( "password", 'S', 10 );		///< Require
	pPacker->AddField( "password_type", 'C', 1 );	///< NonRequire
	pPacker->AddField( "user_token", 'S', 40 );		///< NonRequire
	// 3. Add Filed Value
	pPacker->AddInt( 0 );					///< op_branch_no
	//pPacker->AddChar( 'L' );				///< op_entrust_way
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( 0 );					///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddChar( '2'/*'\0'*/ );		///< password_type
	pPacker->AddStr( FUserToken );			///< user_token
	pPacker->EndPack();
	Msg.printf( L"Logout...Account[%s]", String(gUserID.c_str()) );
	Memo1->Lines->Add( Msg );

	// 4. Send Packet
	RtnVal = FUfxConnection->SendBiz( 331102, pPacker, HSufx_ASyncSend, gSysNodeID );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBiz Logout packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
		return;
	}
	else
	{
		Msg.printf( L">>>>>> Logout SendBiz function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendLogoutMsg( void )
{
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendLogoutMsg() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 331102 );
	lpBizMessage->SetSystemNo( gSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 1. New Packer
	pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendLogout() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< NonRequire
	pPacker->AddField( "client_id", 'S',18 );		///< Require
	pPacker->AddField( "fund_account", 'S',18 );	///< Require
	pPacker->AddField( "password", 'S', 10 );		///< Require
	pPacker->AddField( "password_type", 'C', 1 );	///< NonRequire
	pPacker->AddField( "user_token", 'S', 40 );		///< NonRequire
	// 3. Add Filed Value
	pPacker->AddInt( 0 );					///< op_branch_no
	//pPacker->AddChar( 'L' );				///< op_entrust_way
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( 0 );					///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddChar( '2'/*'\0'*/ );		///< password_type
	pPacker->AddStr( FUserToken );			///< user_token
	pPacker->EndPack();
	Msg.printf( L"Logout...Account[%s]", String(gUserID.c_str()) );
	Memo1->Lines->Add( Msg );

	// 4. Send Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FUfxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBizMsg LogoutMsg packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> LogoutMsg SendBizMsg function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendSubPush( BOOL IsFill )
{
	int		RtnVal;
	int		iIssueType;
	String	Msg;

	if ( IsFill )
		iIssueType = 33011;
	else
		iIssueType = 33012;

	// 1. New Packer
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendSubConfirm() NewPacker() failed!!" );
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< NonRequire
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< NonRequire
	pPacker->AddField( "op_station", 'S', 255 );	///< NonRequire
	pPacker->AddField( "branch_no", 'I', 5 );		///< Require
	//pPacker->AddField( "client_id", 'S', 18 );	///< NonRequire
	pPacker->AddField( "password", 'S', 10 );		///< NonRequire
	pPacker->AddField( "user_token", 'S', 40 );		///< NonRequire
	pPacker->AddField( "issue_type", 'I', 8 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	//pPacker->AddStr( gUserID.c_str() );	///< client_id
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddStr( FUserToken );			///< user_token
	pPacker->AddInt( iIssueType );			///< issue_type
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->EndPack();

	// 4. Set REQ_DATA
	REQ_DATA szReqData;
	memset( &szReqData, 0, sizeof(REQ_DATA) );
	szReqData.issueType  = iIssueType;
	szReqData.lpKeyInfo  = pPacker->GetPackBuf();
	szReqData.keyInfoLen = pPacker->GetPackLen();

	// 5. Send Packet
	RtnVal = FUfxConnection->SendBizEx( 620001, pPacker, NULL, HSufx_ASyncSend, gSysNodeID, 0, gBrokerID, &szReqData );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBizEx SendSubPush(%d) packet failed!! ErrorCode[%d] ErrorMsg[%s]", iIssueType, RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> SendSubPush(%d) SendBizEx function call return. RtnVal[%d]", iIssueType, RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendSubPushMsg( BOOL IsFill )
{
	int          iIssueType;
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	if ( IsFill )
		iIssueType = 33011;
	else
		iIssueType = 33012;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendSubPushMsg() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 620001 );
	lpBizMessage->SetIssueType( iIssueType );
	lpBizMessage->SetSystemNo( gSysNodeID );
	//lpBizMessage->SetSubSystemNo( 0 );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 1. New Packer
	pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendSubConfirm() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< NonRequire
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< NonRequire
	pPacker->AddField( "op_station", 'S', 255 );	///< NonRequire
	pPacker->AddField( "branch_no", 'I', 5 );		///< Require
	//pPacker->AddField( "client_id", 'S', 18 );	///< NonRequire
	pPacker->AddField( "password", 'S', 10 );		///< NonRequire
	pPacker->AddField( "user_token", 'S', 40 );		///< NonRequire
	pPacker->AddField( "issue_type", 'I', 8 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	//pPacker->AddStr( gUserID.c_str() );	///< client_id
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddStr( FUserToken );			///< user_token
	pPacker->AddInt( iIssueType );			///< issue_type
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->EndPack();

	// 4. Send Packet
	//lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	lpBizMessage->SetKeyInfo( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FUfxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBizEx SendSubPushMsg(%d) packet failed!! ErrorCode[%d] ErrorMsg[%s]", iIssueType, RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> SendSubPushMsg(%d) SendBizMsg function call return. RtnVal[%d]", iIssueType, RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendChangePwd( void )
{
	int		RtnVal;
	String	Msg;

	// 1. New Packer
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendChangePwd() NewPacker() failed!!" );
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 10 );
	pPacker->AddField( "password_type", 'C', 1 );
	pPacker->AddField( "user_token", 'S', 512 );
	pPacker->AddField( "new_password", 'S', 10 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddChar( '2' );				///< password_type
	pPacker->AddStr( FUserToken );			///< user_token
	pPacker->AddStr( gNewPassword.c_str() );///< new_password
	pPacker->EndPack();

	// 4. Send Packet
	RtnVal = FUfxConnection->SendBiz( 331101, pPacker, HSufx_ASyncSend, gSysNodeID );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBiz ChangePwd packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
		return;
	}
	else
	{
		Msg.printf( L">>>>>> ChangePwd SendBiz function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendChangePwdMsg( void )
{
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendChangePwdMsg() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 331101 );
	lpBizMessage->SetSystemNo( gSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 1. New Packer
	pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendChangePwdMsg() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 10 );
	pPacker->AddField( "password_type", 'C', 1 );
	pPacker->AddField( "user_token", 'S', 512 );
	pPacker->AddField( "new_password", 'S', 10 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddChar( '2' );				///< password_type
	pPacker->AddStr( FUserToken );			///< user_token
	pPacker->AddStr( gNewPassword.c_str() );///< new_password
	pPacker->EndPack();

	// 4. Send Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FUfxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBizMsg ChangePwdMsg packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> ChangePwdMsg SendBizMsg function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendPosition( void )
{
	int		RtnVal;
	String	Msg;

	// 1. New Packer
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendPosition() NewPacker() failed!!" );
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 10 );
	pPacker->AddField( "password_type", 'C', 1 );
	pPacker->AddField( "user_token", 'S', 40 );
	pPacker->AddField( "asset_prop", 'C', 1 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddChar( '2' );				///< password_type
	pPacker->AddStr( FUserToken );			///< user_token
	pPacker->AddChar( 'B' );				///< asset_prop
	pPacker->EndPack();

	// 4. Send Packet
	RtnVal = FUfxConnection->SendBiz( 338023, pPacker, HSufx_ASyncSend, gSysNodeID );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBiz SendPosition packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
		return;
	}
	else
	{
		Msg.printf( L">>>>>> SendPosition SendBiz function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendPositionMsg( void )
{
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendPositionMsg() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338023 );
	lpBizMessage->SetSystemNo( gSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 1. New Packer
	pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendPositionMsg() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 10 );
	pPacker->AddField( "password_type", 'C', 1 );
	pPacker->AddField( "user_token", 'S', 40 );
	pPacker->AddField( "asset_prop", 'C', 1 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddChar( '2' );				///< password_type
	pPacker->AddStr( FUserToken );			///< user_token
	pPacker->AddChar( 'B' );				///< asset_prop
	pPacker->EndPack();

	// 4. Send Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FUfxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBizMsg SendPositionMsg packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> SendPositionMSg SendBizMsg function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendAccount( void )
{
	int		RtnVal;
	String	Msg;

	// 1. New Packer
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendAccount() NewPacker() failed!!" );
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );//
	pPacker->AddField( "op_entrust_way", 'C', 1 );//
	pPacker->AddField( "op_station", 'S', 255 );//
	pPacker->AddField( "branch_no", 'I', 5 );//
	pPacker->AddField( "client_id", 'S', 18 );//
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 10 );
	pPacker->AddField( "password_type", 'C', 1 );//
	pPacker->AddField( "user_token", 'S', 40 );//
	pPacker->AddField( "asset_prop", 'C', 1 );
	// "money_type" ///< ??
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddChar( '2' );				///< password_type
	pPacker->AddStr( FUserToken );			///< user_token
	pPacker->AddChar( 'B' );				///< asset_prop
	pPacker->EndPack();

	// 4. Send Packet
	RtnVal = FUfxConnection->SendBiz( 338022, pPacker, HSufx_ASyncSend, gSysNodeID );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBiz SendAccount packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
		return;
	}
	else
	{
		Msg.printf( L">>>>>> SendAccount SendBiz function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendAccountMsg( void )
{
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendAccountMsg() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338022 );
	lpBizMessage->SetSystemNo( gSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 1. New Packer
	pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendAccountMsg() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );//
	pPacker->AddField( "op_entrust_way", 'C', 1 );//
	pPacker->AddField( "op_station", 'S', 255 );//
	pPacker->AddField( "branch_no", 'I', 5 );//
	pPacker->AddField( "client_id", 'S', 18 );//
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 10 );
	pPacker->AddField( "password_type", 'C', 1 );//
	pPacker->AddField( "user_token", 'S', 40 );//
	pPacker->AddField( "asset_prop", 'C', 1 );
	// "money_type" ///< ??
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddChar( '2' );				///< password_type
	pPacker->AddStr( FUserToken );			///< user_token
	pPacker->AddChar( 'B' );				///< asset_prop
	pPacker->EndPack();

	// 4. Send Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FUfxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBizMsg SendAccountMSg packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> SendAccountMsg SendBizMSg function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendQrySymbol( void )
{
	int		RtnVal;
	String	Msg;

	// 1. New Packer
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendQrySymbol() NewPacker() failed!!" );
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->EndPack();

	// 4. Send Packet
	RtnVal = FUfxConnection->SendBiz( 338000, pPacker, HSufx_ASyncSend, gSysNodeID );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBiz SendQrySymbol packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
		return;
	}
	else
	{
		Msg.printf( L">>>>>> SendQrySymbol SendBiz function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendQrySymbolMsg( void )
{
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendQrySymbolMsg() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338000 );
	lpBizMessage->SetSystemNo( gSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 1. New Packer
	pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendQrySymbolMsg() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->EndPack();

	// 4. Send Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FUfxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBizMsg SendQrySymbolMsg packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> SendQrySymbolMsg SendBizMsg function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendQryUnderling( void )
{
	int		RtnVal;
	String	Msg;

	// 1. New Packer
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendQryUnderling() NewPacker() failed!!" );
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->EndPack();

	// 4. Send Packet
	RtnVal = FUfxConnection->SendBiz( 338001, pPacker, HSufx_ASyncSend, gSysNodeID );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBiz SendQryUnderling packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
		return;
	}
	else
	{
		Msg.printf( L">>>>>> SendQryUnderling SendBiz function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendQryUnderlingMsg( void )
{
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendQryUnderlingMsg() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338001 );
	lpBizMessage->SetSystemNo( gSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 1. New Packer
	pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendQryUnderlingMsg() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->EndPack();

	// 4. Send Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FUfxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBizMsg SendQryUnderlingMsg packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> SendQryUnderlingMsg SendBizMsg function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendQryCommission( void )
{
	int		RtnVal;
	String	Msg;

	// 1. New Packer
	IF2Packer* pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendQryCommission() NewPacker() failed!!" );
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 10 );
	pPacker->AddField( "password_type", 'C', 1 );
	pPacker->AddField( "user_token", 'S', 40 );
	//pPacker->AddField( "entrust_bs", 'C', 1 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddChar( '2' );				///< password_type
	pPacker->AddStr( FUserToken );			///< user_token
	//pPacker->AddChar( '1' );				///< entrust_bs
	pPacker->EndPack();

	// 4. Send Packet
	RtnVal = FUfxConnection->SendBiz( 331156, pPacker, HSufx_ASyncSend, gSysNodeID );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBiz SendQryCommission packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
		return;
	}
	else
	{
		Msg.printf( L">>>>>> SendQryCommission SendBiz function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendQryCommissionMsg( void )
{
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendQryCommissionMsg() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 331156 );
	lpBizMessage->SetSystemNo( gSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 1. New Packer
	pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendQryCommissionMsg() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 10 );
	pPacker->AddField( "password_type", 'C', 1 );
	pPacker->AddField( "user_token", 'S', 40 );
	//pPacker->AddField( "entrust_bs", 'C', 1 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddChar( '2' );				///< password_type
	pPacker->AddStr( FUserToken );			///< user_token
	//pPacker->AddChar( '1' );				///< entrust_bs
	pPacker->EndPack();

	// 4. Send Packet
	lpBizMessage->SetContent( pPacker->GetPackBuf(), pPacker->GetPackLen() );
	RtnVal = FUfxConnection->SendBizMsg( lpBizMessage, HSufx_ASyncSend );
	pPacker->FreeMem( pPacker->GetPackBuf() );
	pPacker->Release();
	lpBizMessage->Release();
	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"SendBizMsg SendQryCommissionMsg packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> SendQryCommissionMsg SendBizMsg function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::RecvBizEx( String BizName, LPRET_DATA lpRetData, const void *lpUnpackerOrStr )
{
	String Msg;

	IF2UnPacker* Packet = (IF2UnPacker*)lpUnpackerOrStr;
	if ( Packet )
	{
		ShowPacket( TRUE, Packet );
	}
	else
	{
		Msg.printf( L"TTradingForm::RecvBizEx [BizName:%s] lpUnpackerOrStr is NULL !!", BizName.c_str() );
		Memo1->Lines->Add( Msg );
	}

	IF2UnPacker* Packet1 = gHSufxAPI.pNewUnPacker( lpRetData->lpKeyInfo, lpRetData->keyInfoLen );
	if ( Packet1 )
	{
		Packet1->AddRef();
		ShowPacket( FALSE, Packet1 );
	}
	else
	{
		Msg.printf( L"TTradingForm::RecvBizEx [BizName:%s] NewUnPacker return NULL !!", BizName.c_str() );
		Memo1->Lines->Add( Msg );
	}

	if ( Packet == NULL && Packet1 == NULL )
	{
		Msg.printf( L"TTradingForm::RecvBizEx [BizName:%s] Both lpUnpackerOrStr and NewUnPacker by lpRetData->lpKeyInfo is NULL !!", BizName.c_str() );
		Memo1->Lines->Add( Msg );
		goto EXIT;
	}
	Msg.printf( L"<<<<<< %s finish!! >>>>>>", BizName.c_str() );
	TradingForm->Memo1->Lines->Add( Msg );
	TradingForm->Memo1->Lines->Add( "" );

EXIT:
	if ( Packet1 )
		Packet1->Release();
}
//---------------------------------------------------------------------------
void TTradingForm::RecvNewOrder( LPRET_DATA lpRetData, const void *lpUnpackerOrStr )
{
	IF2UnPacker* Packet = (IF2UnPacker*)lpUnpackerOrStr;
	if ( Packet )
	{
		ShowPacket( TRUE, Packet );
		TxtLastOrderNo->Text = String( Packet->GetInt( "entrust_no" ) );
	}
	else
		Memo1->Lines->Add( "TTradingForm::RecvNewOrder lpUnpackerOrStr is NULL !!" );

	IF2UnPacker* Packet1 = gHSufxAPI.pNewUnPacker( lpRetData->lpKeyInfo, lpRetData->keyInfoLen );
	if ( Packet1 )
	{
		Packet1->AddRef();
		ShowPacket( FALSE, Packet1 );
		TxtLastOrderNo->Text = String( Packet1->GetInt( "entrust_no" ) );
	}
	else
		Memo1->Lines->Add( "TTradingForm::RecvNewOrder NewUnPacker return NULL !!" );

	if ( Packet == NULL && Packet1 == NULL )
	{
		Memo1->Lines->Add( "TTradingForm::RecvNewOrder Both lpUnpackerOrStr and NewUnPacker by lpRetData->lpKeyInfo is NULL !!" );
		goto EXIT;
	}
	TradingForm->Memo1->Lines->Add( "<<<<<< NewOrder finish !! >>>>>>" );
	TradingForm->Memo1->Lines->Add( "" );

EXIT:
	if ( Packet1 )
		Packet1->Release();
}
//---------------------------------------------------------------------------
void TTradingForm::RecvLogin( LPRET_DATA lpRetData, const void *lpUnpackerOrStr )
{
	const char* pUserToken = NULL;
	const char* pClientID  = NULL;
	int iLoginTimes;
	String	Msg;

	IF2UnPacker* Packet = (IF2UnPacker*)lpUnpackerOrStr;
	if ( Packet )
	{
		ShowPacket( TRUE, Packet );
		gBrokerID   = Packet->GetInt( "branch_no" );
		TxtBrokerID->Text = String( gBrokerID );
		gSysNodeID  = Packet->GetInt( "sysnode_id" );
		pUserToken  = Packet->GetStr( "user_token" );
		pClientID   = Packet->GetStr( "client_id" );
		iLoginTimes = Packet->GetInt( "login_times" );
	}
	else
		Memo1->Lines->Add( "TTradingForm::RecvLogin lpUnpackerOrStr is NULL !!" );

	IF2UnPacker* Packet1 = gHSufxAPI.pNewUnPacker( lpRetData->lpKeyInfo, lpRetData->keyInfoLen );
	if ( Packet1 )
	{
		Packet1->AddRef();
		ShowPacket( FALSE, Packet1 );
		gBrokerID  = Packet1->GetInt( "branch_no" );
		TxtBrokerID->Text = String( gBrokerID );
		gSysNodeID = Packet1->GetInt( "sysnode_id" );
		pUserToken = Packet1->GetStr( "user_token" );
		pClientID  = Packet1->GetStr( "client_id" );
		iLoginTimes = Packet->GetInt( "login_times" );
	}
	else
		Memo1->Lines->Add( "TTradingForm::RecvLogin NewUnPacker return NULL !!" );

	if ( Packet == NULL && Packet1 == NULL )
	{
		Memo1->Lines->Add( "TTradingForm::RecvLogin Both lpUnpackerOrStr and NewUnPacker by lpRetData->lpKeyInfo is NULL !!" );
		goto EXIT;
	}
	if ( pUserToken == NULL )
	{
		Memo1->Lines->Add( "TTradingForm::RecvLogin lpUnpack->GetStr <user_token> return NULL !!" );
		goto EXIT;
	}
	if ( pClientID == NULL )
	{
		Memo1->Lines->Add( "TTradingForm::RecvLogin lpUnpack->GetStr <client_id> return NULL !!" );
		goto EXIT;
	}
	strcpy( FUserToken, pUserToken );
	strcpy( FClientID,  pClientID );
	TxtUserToken->Text  = "[" + String( FUserToken ) + "]";
	TxtClientID->Text   = String( FClientID );
	TxtLoginTimes->Text = String( iLoginTimes );
	TxtSysNodeID->Text = String( gSysNodeID );
	TradingForm->Memo1->Lines->Add( "<<<<<< Login finish >>>>>>" );
	TradingForm->Memo1->Lines->Add( "" );

	// Set Buttons
	TradingForm->SetButtons( true );

EXIT:
	if ( Packet1 )
		Packet1->Release();
}
//---------------------------------------------------------------------------
void TTradingForm::RecvHeartBeat( LPRET_DATA lpRetData, const void *lpUnpackerOrStr )
{
	String	Msg;
	int RtnVal;
	REQ_DATA reqData;

	memset( &reqData,0,sizeof(REQ_DATA) );
	reqData.issueType  = 0;		///<此處需要確認，心跳應答是否區分訂閱類型
	reqData.packetType = 1;		///<轉換為應答包
	memcpy(&reqData.routeInfo, &lpRetData->sendInfo, sizeof(reqData.routeInfo));
	reqData.lpKeyInfo = new char[lpRetData->keyInfoLen+1];
	memcpy( reqData.lpKeyInfo, lpRetData->lpKeyInfo, lpRetData->keyInfoLen );
	((char*)reqData.lpKeyInfo)[lpRetData->keyInfoLen] = 0;

	RtnVal = FUfxConnection->SendBizEx( 620000, NULL, NULL, HSufx_ASyncSend, gSysNodeID, 0, gBrokerID, &reqData);

	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"[Receive HeartBeat] [issue_type:%d] [key:%s(%d)] SendBizEx HeartBeat(%d) packet failed!! ErrorCode[%d] ErrorMsg[%s]"
					 , lpRetData->issueType, String((const char*)lpRetData->lpKeyInfo), lpRetData->keyInfoLen
					 , reqData.issueType, RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		if ( ChkShowHB->Checked == true )
		{
			Msg.printf( L"[Receive HeartBeat] [issue_type:%d] [key:%s(%d)] SendBizEx HeartBeat(%d) packet OK"
						, lpRetData->issueType, String((const char*)(lpRetData->lpKeyInfo)), lpRetData->keyInfoLen
						, reqData.issueType );
			TradingForm->Memo1->Lines->Add( Msg );
		}
	}

	delete [] (char*)reqData.lpKeyInfo;
}
//---------------------------------------------------------------------------
void TTradingForm::RecvBizMsg( String BizName, IBizMessage* lpMsg, bool IsRegMsg )
{
	String Msg;
	int iLen = 0;
	const void* lpBuffer;

	if ( IsRegMsg )
		lpBuffer = lpMsg->GetKeyInfo( iLen );
	else
		lpBuffer = lpMsg->GetContent( iLen );

	IF2UnPacker* Packet = gHSufxAPI.pNewUnPacker( (void*)lpBuffer, iLen );
	if ( Packet == NULL )
	{
		Msg.printf( L"TTradingForm::RecvBizMsg [BizName:%s] NewUnPacker return NULL !!", BizName.c_str() );
		Memo1->Lines->Add( Msg );
		return;
	}
	Packet->AddRef();
	ShowPacket( TRUE, Packet );
	Msg.printf( L"<<<<<< %s finish!! >>>>>>", BizName.c_str() );
	TradingForm->Memo1->Lines->Add( Msg );
	TradingForm->Memo1->Lines->Add( "" );
	Packet->Release();
}
//---------------------------------------------------------------------------
void TTradingForm::RecvBizMsgNewOrder( IBizMessage* lpMsg )
{
	int iLen = 0;
	const void* lpBuffer = lpMsg->GetContent( iLen );
	IF2UnPacker* Packet = gHSufxAPI.pNewUnPacker( (void*)lpBuffer, iLen );
	if ( Packet == NULL )
	{
		Memo1->Lines->Add( "TTradingForm::RecvNewOrder lpUnpackerOrStr is NULL !!" );
		return;
	}

	Packet->AddRef();
	ShowPacket( TRUE, Packet );
	TxtLastOrderNo->Text = String( Packet->GetInt( "entrust_no" ) );
	TradingForm->Memo1->Lines->Add( "<<<<<< NewOrder finish !! >>>>>>" );
	TradingForm->Memo1->Lines->Add( "" );
	Packet->Release();
}
//---------------------------------------------------------------------------
void TTradingForm::RecvBizMsgLogin( IBizMessage* lpMsg )
{
	const char* pUserToken = NULL;
	const char* pClientID  = NULL;
	int iLoginTimes;
	String	Msg;

	int iLen = 0;
	const void* lpBuffer = lpMsg->GetContent( iLen );
	IF2UnPacker* Packet = gHSufxAPI.pNewUnPacker( (void*)lpBuffer, iLen );
	if ( Packet == NULL )
	{
		Memo1->Lines->Add( "TTradingForm::RecvBizMsgLogin NewUnPacker is NULL !!" );
		return;
	}
	ShowPacket( TRUE, Packet );
	gBrokerID   = Packet->GetInt( "branch_no" );
	TxtBrokerID->Text = String( gBrokerID );
	gSysNodeID  = Packet->GetInt( "sysnode_id" );
	pUserToken  = Packet->GetStr( "user_token" );
	pClientID   = Packet->GetStr( "client_id" );
	iLoginTimes = Packet->GetInt( "login_times" );

	if ( pUserToken == NULL )
	{
		Memo1->Lines->Add( "TTradingForm::RecvBizMsgLogin Packet->GetStr <user_token> return NULL !!" );
		goto EXIT;
	}
	if ( pClientID == NULL )
	{
		Memo1->Lines->Add( "TTradingForm::RecvBizMsgLogin Packet->GetStr <client_id> return NULL !!" );
		goto EXIT;
	}
	strcpy( FUserToken, pUserToken );
	strcpy( FClientID,  pClientID );
	TxtUserToken->Text  = "[" + String( FUserToken ) + "]";
	TxtClientID->Text   = String( FClientID );
	TxtLoginTimes->Text = String( iLoginTimes );
	TxtSysNodeID->Text = String( gSysNodeID );
	TradingForm->Memo1->Lines->Add( "<<<<<< Login finish >>>>>>" );
	TradingForm->Memo1->Lines->Add( "" );

	// Set Buttons
	TradingForm->SetButtons( true );

EXIT:
	if ( Packet )
		Packet->Release();
}
//---------------------------------------------------------------------------
void TTradingForm::RecvBizMsgHeartBeat( IBizMessage* lpMsg )
{
	String	Msg;
	int RtnVal;

	lpMsg->ChangeReq2AnsMessage();
	RtnVal = FUfxConnection->SendBizMsg( lpMsg, HSufx_ASyncSend );

	if ( RtnVal < 0 )
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"[Receive HeartBeat] [issue_type:%d] SendBizMsg Reply HeartBeat packet failed!! ErrorCode[%d] ErrorMsg[%s]"
					 , lpMsg->GetIssueType(), RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		if ( ChkShowHB->Checked == true )
		{
			Msg.printf( L"[Receive HeartBeat] [issue_type:%d] SendBizMsg Reply HeartBeat packet OK", lpMsg->GetIssueType() );
			TradingForm->Memo1->Lines->Add( Msg );
		}
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SetButtons( bool IsConnected )
{
	if ( IsConnected )
	{
		BtnNewOrder->Enabled      = true;
		BtnCancelOrder->Enabled   = true;
		BtnQryOrder->Enabled      = true;
		BtnQryFill->Enabled       = true;
		BtnConnStatus->Enabled    = true;
		BtnSubPush->Enabled       = true;
		BtnChangePwd->Enabled     = false;
		BtnPosition->Enabled      = true;
		BtnAccount->Enabled       = true;
		BtnQrySymbol->Enabled     = true;
		BtnQryUnderlying->Enabled = true;
		BtnLogoff->Enabled        = true;
		BtnDisconnect->Enabled    = true;
		BtnQryCommission->Enabled = true;
		BtnConn_Login->Enabled    = false;
	}
	else
	{
		BtnNewOrder->Enabled      = false;
		BtnCancelOrder->Enabled   = false;
		BtnQryOrder->Enabled      = false;
		BtnQryFill->Enabled       = false;
		BtnConnStatus->Enabled    = false;
		BtnSubPush->Enabled       = true;
		BtnChangePwd->Enabled     = false;
		BtnPosition->Enabled      = false;
		BtnAccount->Enabled       = false;
		BtnQrySymbol->Enabled     = false;
		BtnQryUnderlying->Enabled = false;
		BtnLogoff->Enabled        = false;
		BtnDisconnect->Enabled    = false;
		BtnQryCommission->Enabled = false;
		BtnConn_Login->Enabled    = true;
	}
}
//---------------------------------------------------------------------------
void TTradingForm::CloseConnection( void )
{
	if ( FUfxConnection != NULL )
	{
		// Check connection status then close connection
		if ( FUfxConnection->GetStatus() != CConnectionInterface::Disconnected ) ///< Disconnected = 0x0000, /**< 未連接 */
			FUfxConnection->Close();
	}
}
//---------------------------------------------------------------------------
void TTradingForm::ReleaseAPI( void )
{
	// 1. Connection object
	if ( FUfxConnection != NULL )
	{
		// Release connection object
		FUfxConnection->Release();
		FUfxConnection = NULL;
	}
	Memo1->Lines->Add( "<< Client Release API !! >>" );
	// 2. Listener object
	/*
	Sleep( 1000 );
	if ( FListener != NULL )
	{
		delete FListener;
		FListener = NULL;
	}
	*/
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnConn_LoginClick(TObject *Sender)
{
	int		RtnVal;
	String	Msg;

	SetGlobalVariant( );
	SetUfxConfig( );

	// 1. Create Object
	if ( FListener == NULL )
		FListener = new HSufxListener( );
	if ( FUfxConnection == NULL )
	{
		//FUfxConnection = NewConnection( FUfxConfig );
		FUfxConnection = gHSufxAPI.pNewConnection( FUfxConfig );
		FUfxConnection->AddRef();
		//if ( ( RtnVal = FUfxConnection->CreateEx( FListener ) ) != 0 )
		if ( ( RtnVal = FUfxConnection->Create2BizMsg( FListener ) ) != 0 )
		{
			GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
			//Msg.printf( L"FUfxConnection->CreateEx() failed!!, ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
			Msg.printf( L"FUfxConnection->Create2BizMsg() failed!!, ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
			Memo1->Lines->Add( Msg );
			return;
		}
	}

	// 2. Connect
	Msg.printf( L"Connect...[%s]", String(gServerAddress.c_str()) );
	Memo1->Lines->Add( Msg );
	if ( ( RtnVal = FUfxConnection->Connect( 5000 ) ) != 0 ) ///< 10 sec.
	{
		GB2312String GBStr( FUfxConnection->GetErrorMsg( RtnVal ) );
		Msg.printf( L"Connect failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
		return;
	}
	Memo1->Lines->Add( "<<<<<< Client Connected !! >>>>>>" );

	// 3. Login
	//SendLogin( );
	SendLoginMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnLogoffClick(TObject *Sender)
{
	//SendLogout( );
	SendLogoutMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnDisconnectClick(TObject *Sender)
{
	// Disable Buttons
	SetButtons( false );
	// Close connection
	CloseConnection( );
	// Release API
	ReleaseAPI( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnConnStatusClick(TObject *Sender)
{
	String Msg = "Connection->GetStatus() : ";
	int status = FUfxConnection->GetStatus();
	if ( status & CConnectionInterface::Disconnected )		Msg += "[Disconnected]";
	if ( status & CConnectionInterface::Connecting )		Msg += "[Connecting]";
	if ( status & CConnectionInterface::Connected )			Msg += "[Connected]";
	if ( status & CConnectionInterface::SafeConnecting )	Msg += "[SafeConnecting]";
	if ( status & CConnectionInterface::SafeConnected )		Msg += "[SafeConnected]";
	if ( status & CConnectionInterface::Registering )		Msg += "[Registering]";
	if ( status & CConnectionInterface::Registered )		Msg += "[Registered]";
	if ( status & CConnectionInterface::Rejected )			Msg += "[Rejected]";
	Memo1->Lines->Add( Msg );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnSubPushClick(TObject *Sender)
{
	// 1. Subscribe Order Confirm push ( 620001-33012 )
	//SendSubPush( FALSE );
	SendSubPushMsg( FALSE );

	// 2. Subscribe Order Fill push ( 620001-33011 )
	//SendSubPush( TRUE );
	SendSubPushMsg( TRUE );

}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnChangePwdClick(TObject *Sender)
{
	//SendChangePwd( );
	SendChangePwdMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnPositionClick(TObject *Sender)
{
	//SendPosition( );
	SendPositionMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnAccountClick(TObject *Sender)
{
	//SendAccount( );
	SendAccountMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnQrySymbolClick(TObject *Sender)
{
	//SendQrySymbol( );
	SendQrySymbolMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnQryUnderlyingClick(TObject *Sender)
{
	//SendQryUnderling( );
	SendQryUnderlingMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnNewOrderClick(TObject *Sender)
{
	//SendNewOrder( );
	SendNewOrderMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnCancelOrderClick(TObject *Sender)
{
	//SendCancelOrder( );
	SendCancelOrderMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnQryOrderClick(TObject *Sender)
{
	//SendQryOrder( );
	SendQryOrderMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnQryFillClick(TObject *Sender)
{
	//SendQryFill( );
	SendQryFillMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnQryCommissionClick(TObject *Sender)
{
	//SendQryCommission( );
	SendQryCommissionMsg( );
}
//---------------------------------------------------------------------------

