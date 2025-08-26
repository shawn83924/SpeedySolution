//---------------------------------------------------------------------------

#include <vcl.h>
#include <WideStrUtils.hpp>
#pragma hdrstop

#include "UnitTestFutOpt.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTradingForm *TradingForm;
//---------------------------------------------------------------------------
int gBrokerID  = 0;
// 恒生
/*
int gSysNodeID = 5;
AnsiString gServerAddress( "117.78.3.150:6006" );
AnsiString gUserID   = "1010000078";
AnsiString gPassword    = "111111";
AnsiString gNewPassword = "123456";
*/
// 中糧期貨
/*
int gSysNodeID = 0; ///< SystemNo
AnsiString gServerAddress( "116.228.196.249:18001" );
AnsiString gUserID   = "10000296";
AnsiString gPassword = "123789";
AnsiString gNewPassword = "123456";
*/
/*
// 永安期貨
int gSysNodeID = 0; ///< SystemNo
AnsiString gServerAddress( "115.238.53.140:9006" );
AnsiString gUserID   = "101300930";
AnsiString gPassword = "888888";
AnsiString gNewPassword = "123456";
*/
// 魯證期貨
int gSysNodeID = 0; ///< SystemNo
AnsiString gServerAddress( "124.133.0.117:9025" );
AnsiString gUserID   = "20105037";
AnsiString gPassword = "123456";
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
		swprintf( MessageWindowsClassName, L"HSUFX_TestFutOpt_MsgWin" );
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
						   L"HSUFX_TestFutOpt_App",// title-bar string
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
	if ( Message == WM_USER+902 )
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
	TradingForm->Memo1->Lines->Add( "******< OnReceivedBizEx >******" );
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
	case 338202 : ///< New Order (single row)
		TradingForm->RecvBizMsgNewOrder( lpMsg );
		break;
	case 338217 : ///< Cancel Order (single row)
		TradingForm->RecvBizMsg( "RecvCancelOrder", lpMsg );
		break;
	case 620001 : ///< Subscribe Ack
		if ( iIssueType == 33101 )
		{
			TradingForm->RecvBizMsg( "RecvRegReport", lpMsg, true );
			Msg.printf( L"[Receive Subscribe Ack] [issue_type:%d] [Report Confirm & Fill]", iIssueType );
		}
		else if ( iIssueType == 33105 )
		{
			TradingForm->RecvBizMsg( "RecvRegExchStatus", lpMsg, true );
			Msg.printf( L"[Receive Subscribe Ack] [issue_type:%d] [ExchStatus]", iIssueType );
		}
		TradingForm->Memo1->Lines->Add( Msg );
		break;
	case 620003 : ///< Subscribe Result  33101-Report , 33105-ExchStatus
		if ( iIssueType == 33101 )
			Msg.printf( L"[Recevie Report] [issue_type:%d]", iIssueType );
		else if ( iIssueType == 33105 )
			Msg.printf( L"[Recevie ExchStatus] [issue_type:%d]", iIssueType );
		TradingForm->Memo1->Lines->Add( Msg );
		TradingForm->RecvBizMsg( "RecvSubResult", lpMsg, true );
		TradingForm->RecvBizMsg( "RecvSubResult", lpMsg, false );
		break;
	case 338301 : ///< Query Order (multi row)
		TradingForm->RecvBizMsg( "RecvQryOrder", lpMsg );
		break;
	case 338302 : ///< Query Fill (multi row)
		TradingForm->RecvBizMsg( "RecvQryFill", lpMsg );
		break;
	case 338303 : ///< Query Hold Postion (multi row)
		TradingForm->RecvBizMsg( "RecvQryPosition", lpMsg );
		break;
	case 338326 : ///< Query Init Hold Postion (multi row)
		TradingForm->RecvBizMsg( "RecvQryInitPosition", lpMsg );
		break;
	case 338300 : ///< Query Asset (single row)
		TradingForm->RecvBizMsg( "RecvQryAccount", lpMsg );
		break;
	case 330850 : ///< Query Symbol base data (multi row)
		TradingForm->RecvBizMsg( "RecvQrySymbol", lpMsg );
		break;
	///< General Biz functions
	case 620000 : ///< HeartBeat
		//RecvHeartBeatMsg( lpMsg );
		TradingForm->RecvBizMsgHeartBeat( lpMsg );
		break;
	case 331100 : ///< Login (single row)
		TradingForm->RecvBizMsgLogin( lpMsg );
		break;
	case 331101 : ///< Change Password (single row)
		TradingForm->RecvBizMsg( "RecvChangePwd", lpMsg );
		break;
	case 331102 : ///< Logout (single row)
		TradingForm->RecvBizMsg( "RecvLogout", lpMsg );
		break;
	case 338304 : ///< (Query Commissition) (multi row)
		TradingForm->RecvBizMsg( "RecvQryCommission", lpMsg );
		break;
	case 338313 : ///< (Query ExchStatus) (multi row)
		TradingForm->RecvBizMsg( "RecvQryExchStatus", lpMsg );
		break;
	case 330852 : ///< (Query ExchTime) (multi row)
		TradingForm->RecvBizMsg( "RecvQryExchTime", lpMsg );
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
	FUfxConfig->SetString( "t2sdk", "login_name", "192.168.17.1-super8" );
	// 中糧期貨
	//FUfxConfig->SetString( "t2sdk", "license_file", "(20130729)ZLQH-ZLQHSHKY-0000_3rd.dat" );
	////FUfxConfig->SetString( "t2sdk", "license_pwd", "123456" );
	//FUfxConfig->SetString( "safe",  "safe_level", "none" ); ///< "none" "ssl"
	// 永安期貨
	FUfxConfig->SetString( "t2sdk", "license_file", "HSt2sdkFutLicense.dat" );
	FUfxConfig->SetString( "safe",  "safe_level", "none" ); ///< "none" "ssl"
	// 恒生 UFX
	/*
	FUfxConfig->SetString( "t2sdk", "license_file", "(20130306)tyjrnb-TYJRTYJR-ALL-0001_3rd.dat" );
	//FUfxConfig->SetString( "t2sdk", "license_pwd", "888888" );
	FUfxConfig->SetString( "safe",  "safe_level", "ssl" ); ///< "none" "ssl"
	FUfxConfig->SetString( "safe",  "cert_file", "c20121011.pfx" );
	FUfxConfig->SetString( "safe",  "cert_pwd", "111111" );
	*/
}
//---------------------------------------------------------------------------
void TTradingForm::ShowPacket( BOOL IsUnPackerData, IF2UnPacker* pUnPacker, AnsiString* PositionStr )
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

		if ( PositionStr )
			*PositionStr = "";
		//for ( j=0; j < pUnPacker->GetRowCount(); j++ )
		pUnPacker->First();
		while ( !pUnPacker->IsEOF() )
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
			// save position_str
			if ( PositionStr )
			{
				const char* strPos = pUnPacker->GetStr("position_str");
				if( strPos == 0)
					*PositionStr = "";
				else
					*PositionStr = strPos;
			}
			// Move next
			pUnPacker->Next();
		}
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
	lpBizMessage->SetFunction( 338202 );
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
	pPacker->AddField( "user_token", 'S', 512 );
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 50 );
	pPacker->AddField( "futu_exch_type", 'S', 4 );
	pPacker->AddField( "futures_account", 'S', 12 );
	pPacker->AddField( "futu_code", 'S', 30 );
	pPacker->AddField( "entrust_bs", 'C', 1 );
	pPacker->AddField( "futures_direction", 'C', 1 );
	pPacker->AddField( "hedge_type", 'C', 1 );
	pPacker->AddField( "entrust_amount", 'I' );
	pPacker->AddField( "futu_entrust_price", 'F', 12, 6 );
	pPacker->AddField( "entrust_prop", 'S', 3 );
	pPacker->AddField( "entrust_occasion", 'S', 32 );
	pPacker->AddField( "entrust_reference", 'S', 32 );
	// 3. Add Filed Value
	pPacker->AddStr( FUserToken );				///< user_token
	pPacker->AddInt( gBrokerID );				///< op_branch_no
	pPacker->AddChar( '7' );					///< op_entrust_way
	pPacker->AddStr( "1" );						///< op_station
	pPacker->AddInt( gBrokerID );				///< branch_no
	pPacker->AddStr( FClientID );				///< client_id
	pPacker->AddStr( gUserID.c_str() );			///< fund_account
	pPacker->AddStr( gPassword.c_str() );		///< password
	pPacker->AddStr( "F4" );					///< futu_exch_type F1-CZCE F2-DCE F3-SHFE F4-CFFEX
	pPacker->AddStr( "" );						///< futures_account
	pPacker->AddStr( AnsiString(TxtSymbol->Text).c_str() );	///< futu_code
	// entrust_bs
	if ( TxtBS->Text == "B" )
		pPacker->AddChar( '1' );
	else
		pPacker->AddChar( '2' );
	// futures_direction
	if ( TxtOC->Text == "O" )
		pPacker->AddChar( '1' );
	else
		pPacker->AddChar( '2' );
	pPacker->AddChar( '0' );					///< hedge_type
	pPacker->AddInt( TxtQuantity->Text.ToInt() );///< entrust_amount
	pPacker->AddDouble( TxtPrice->Text.ToDouble() );///< futu_entrust_price
	pPacker->AddStr( AnsiString(TxtOrderCondition->Text).c_str() );	///< entrust_prop
	AnsiString hsRef;
	hsRef.printf( "S8-entrust_occasion_New_%d", gOrderRef );
	pPacker->AddStr( hsRef.c_str() );	///< entrust_occasion
	hsRef.printf( "S8-entrust_reference_New_%d", gOrderRef );
	pPacker->AddStr( hsRef.c_str() );	///< entrust_reference
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
	lpBizMessage->SetFunction( 338217 );
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
	pPacker->AddField( "user_token", 'S', 512 );
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 50 );
	pPacker->AddField( "futu_exch_type", 'S', 4 );
	pPacker->AddField( "entrust_no", 'I', 8 );
	//pPacker->AddField( "confirm_id", 'S', 20 );
	//pPacker->AddField( "session_no", 'I', 8 );
	pPacker->AddField( "entrust_occasion", 'S', 32 );
	pPacker->AddField( "entrust_reference", 'S', 32 );
	// 3. Add Filed Value
	pPacker->AddStr( FUserToken );				///< user_token
	pPacker->AddInt( gBrokerID );				///< op_branch_no
	pPacker->AddChar( '7' );					///< op_entrust_way
	pPacker->AddStr( "1" );						///< op_station
	pPacker->AddInt( gBrokerID );				///< branch_no
	pPacker->AddStr( FClientID );				///< client_id
	pPacker->AddStr( gUserID.c_str() );			///< fund_account
	pPacker->AddStr( gPassword.c_str() );		///< password
	pPacker->AddStr( "F4" );					///< futu_exch_type F1-CZCE F2-DCE F3-SHFE F4-CFFEX
	pPacker->AddInt( TxtLastOrderNo->Text.ToInt() );	///< entrust_no
	AnsiString hsRef;
	hsRef.printf( "S8-entrust_occasion_New_%d", gOrderRef );
	pPacker->AddStr( hsRef.c_str() );	///< entrust_occasion
	hsRef.printf( "S8-entrust_reference_New_%d", gOrderRef );
	pPacker->AddStr( hsRef.c_str() );	///< entrust_reference
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
void TTradingForm::SendQryOrderMsg( AnsiString* PositionStr )
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
	lpBizMessage->SetFunction( 338301 );
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
	pPacker->AddField( "user_token", 'S', 512 );
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	pPacker->AddField( "branch_no", 'I', 5 );		///< Require
	pPacker->AddField( "client_id", 'S', 18 );		///< Require
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	pPacker->AddField( "password", 'S', 50 );		///< Require
	//pPacker->AddField( "futu_exch_type", 'S', 4 );	///< NonRequire
	pPacker->AddField( "query_direction", 'C', 1 );	///< NonRequire
	pPacker->AddField( "sort_direction", 'C', 1 );	///< NonRequire
	pPacker->AddField( "request_num", 'I', 10 );	///< NonRequire
	pPacker->AddField( "position_str", 'S', 100 );	///< NonRequire
	// 3. Add Filed Value
	pPacker->AddStr( FUserToken );				///< user_token
	pPacker->AddInt( gBrokerID );				///< op_branch_no
	pPacker->AddChar( '7' );					///< op_entrust_way
	pPacker->AddStr( "1" );						///< op_station
	pPacker->AddInt( gBrokerID );				///< branch_no
	pPacker->AddStr( FClientID );				///< client_id
	pPacker->AddStr( gUserID.c_str() );			///< fund_account
	pPacker->AddStr( gPassword.c_str() );		///< password
	//pPacker->AddStr( "F4" );					///< futu_exch_type F1-CZCE F2-DCE F3-SHFE F4-CFFEX
	pPacker->AddChar( '0' );					///< query_direction
	pPacker->AddChar( '0' );					///< sort_direction
	pPacker->AddInt( TxtRequestNum->Text.ToInt() );///< request_num
	///< position_str
	if ( PositionStr )
		pPacker->AddStr( PositionStr->c_str() );
	else
		pPacker->AddStr( "" );
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
void TTradingForm::SendQryFillMsg( AnsiString* PositionStr )
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
	lpBizMessage->SetFunction( 338302 );
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
	pPacker->AddField( "password", 'S', 50 );
	//pPacker->AddField( "futu_exch_type", 'S', 4 );	///< NonRequire
	pPacker->AddField( "query_mode", 'C', 1 );		///< NonRequire
	pPacker->AddField( "query_direction", 'C', 1 );	///< NonRequire
	pPacker->AddField( "sort_direction", 'C', 1 );	///< NonRequire
	pPacker->AddField( "request_num", 'I', 10 );	///< NonRequire
	pPacker->AddField( "position_str", 'S', 100 );	///< NonRequire
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );				///< op_branch_no
	pPacker->AddChar( '7' );					///< op_entrust_way
	pPacker->AddStr( "1" );						///< op_station
	pPacker->AddInt( gBrokerID );				///< branch_no
	pPacker->AddStr( FClientID );				///< client_id
	pPacker->AddStr( gUserID.c_str() );			///< fund_account
	pPacker->AddStr( gPassword.c_str() );		///< password
	//pPacker->AddStr( "F4" );					///< futu_exch_type F1-CZCE F2-DCE F3-SHFE F4-CFFEX
	pPacker->AddChar( '0' );					///< query_mode //明細
	pPacker->AddChar( '0' );					///< query_direction
	pPacker->AddChar( '1' );					///< sort_direction
	pPacker->AddInt( TxtRequestNum->Text.ToInt() );///< request_num
	///< position_str
	if ( PositionStr )
		pPacker->AddStr( PositionStr->c_str() );
	else
		pPacker->AddStr( "" );
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
	pPacker->AddField( "password", 'S', 50 );		///< Require
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
	pPacker->AddField( "password", 'S', 50 );		///< Require
	// 3. Add Filed Value
	pPacker->AddInt( 0 );					///< op_branch_no
	//pPacker->AddChar( 'L' );				///< op_entrust_way
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( 0 );					///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
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
void TTradingForm::SendSubPushMsg( void )
{
	int          iIssueType;
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	iIssueType = 33101;

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
		Memo1->Lines->Add( "When SendSubPush() NewPacker() failed!!" );
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
	pPacker->AddField( "password", 'S', 50 );		///< NonRequire
	pPacker->AddField( "user_token", 'S', 512 );	///< NonRequire
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
void TTradingForm::SendSubExchStatus( void )
{
	int          iIssueType;
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	iIssueType = 33105;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendSubExchStatus() NewBizMessage() failed!!" );
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
		Memo1->Lines->Add( "When SendSubExchStatus() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	//pPacker->AddField( "op_branch_no", 'I', 5 );	///< NonRequire
	//pPacker->AddField( "op_entrust_way", 'C', 1 );	///< NonRequire
	//pPacker->AddField( "op_station", 'S', 255 );	///< NonRequire
	//pPacker->AddField( "branch_no", 'I', 5 );		///< NonRequire
	//pPacker->AddField( "client_id", 'S', 18 );	///< NonRequire
	pPacker->AddField( "fund_account", 'S', 18 );	///< Require
	//pPacker->AddField( "password", 'S', 50 );		///< NonRequire
	//pPacker->AddField( "user_token", 'S', 512 );	///< NonRequire
	pPacker->AddField( "issue_type", 'I', 8 );		///< Require
	pPacker->AddField( "futu_exch_type", 'S', 4 );	///< Require
	// 3. Add Filed Value
	//pPacker->AddInt( gBrokerID );			///< op_branch_no
	//pPacker->AddChar( '7' );				///< op_entrust_way
	//pPacker->AddStr( "1" );					///< op_station
	//pPacker->AddInt( gBrokerID );			///< branch_no
	//pPacker->AddStr( gUserID.c_str() );	///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	//pPacker->AddStr( gPassword.c_str() );	///< password
	//pPacker->AddStr( FUserToken );			///< user_token
	pPacker->AddInt( iIssueType );			///< issue_type
	pPacker->AddStr( "F1" );				///< futu_exch_type
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddInt( iIssueType );			///< issue_type
	pPacker->AddStr( "F2" );				///< futu_exch_type
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddInt( iIssueType );			///< issue_type
	pPacker->AddStr( "F3" );				///< futu_exch_type
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddInt( iIssueType );			///< issue_type
	pPacker->AddStr( "F4" );				///< futu_exch_type
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
		Msg.printf( L"SendBizEx SendSubExchStatus(%d) packet failed!! ErrorCode[%d] ErrorMsg[%s]", iIssueType, RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> SendSubExchStatus(%d) SendBizMsg function call return. RtnVal[%d]", iIssueType, RtnVal );
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
	pPacker->AddField( "password", 'S', 50 );
	pPacker->AddField( "password_type", 'C', 1 );
	pPacker->AddField( "user_token", 'S', 512 );
	pPacker->AddField( "new_password", 'S', 50 );
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
	lpBizMessage->SetFunction( 338303 );
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
	pPacker->AddField( "password", 'S', 50 );
	//pPacker->AddField( "futu_exch_type", 'S', 4 );	///< NonRequire
	pPacker->AddField( "query_mode", 'C', 1 );		///< NonRequire
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
	//pPacker->AddStr( "F4" );					///< futu_exch_type F1-CZCE F2-DCE F3-SHFE F4-CFFEX
	pPacker->AddChar( '1' );				///< query_mode //不過濾持倉為0的數據
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
void TTradingForm::SendInitPositionMsg( void )
{
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendInitPositionMsg() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338326 );
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
	pPacker->AddField( "password", 'S', 50 );
	pPacker->AddField( "fund_account", 'S', 18 );
	//pPacker->AddField( "futu_exch_type", 'S', 4 );	///< NonRequire
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	//pPacker->AddStr( "F4" );					///< futu_exch_type F1-CZCE F2-DCE F3-SHFE F4-CFFEX
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
	lpBizMessage->SetFunction( 338300 );
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
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 50 );
	pPacker->AddField( "user_token", 'S', 512 );
	//pPacker->AddField( "money_type", 'S', 3 );	///< NonRequire
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddStr( FUserToken );			///< user_token
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
void TTradingForm::SendQrySymbolMsg( AnsiString* PositionStr )
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
	lpBizMessage->SetFunction( 330850 );
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
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 50 );
	pPacker->AddField( "futu_exch_type", 'S', 4 );	///< Require
	//pPacker->AddField( "futu_code", 'S', 30 );		///< Require
	pPacker->AddField( "request_num", 'I', 10 );	///< NonRequire
	pPacker->AddField( "position_str", 'S', 100 );	///< NonRequire
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddStr( "F4" );				///< futu_exch_type F1-CZCE F2-DCE F3-SHFE F4-CFFEX
	pPacker->AddInt( TxtRequestNum->Text.ToInt() );///< request_num
	///< position_str
	if ( PositionStr )
		pPacker->AddStr( PositionStr->c_str() );
	else
		pPacker->AddStr( "" );
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
	lpBizMessage->SetFunction( 338304 );
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
	pPacker->AddField( "password", 'S', 50 );
	pPacker->AddField( "user_token", 'S', 40 );
	pPacker->AddField( "futufare_type", 'C', 1 );
	pPacker->AddField( "futucode_type", 'S', 4 );
	pPacker->AddField( "futu_code", 'S', 30 );
	pPacker->AddField( "entrust_bs", 'C', 1 );
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
	pPacker->AddStr( FUserToken );			///< user_token
	pPacker->AddChar( '1' ); 				///< futufare_type
	pPacker->AddStr( "h" );					///< futucode_type
	pPacker->AddStr( "IF1505" );			///< futu_code
	pPacker->AddChar( '1' );				///< entrust_bs
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
void TTradingForm::SendQryExchStatus( void )
{
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendQryExchStatus() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 338313 );
	lpBizMessage->SetSystemNo( gSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 1. New Packer
	pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendQryExchStatus() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "op_branch_no", 'I', 5 );	///< Require
	pPacker->AddField( "op_entrust_way", 'C', 1 );	///< Require
	pPacker->AddField( "op_station", 'S', 255 );	///< Require
	//pPacker->AddField( "futu_exch_type", 'S', 4 );	///< NonRequire
	// 3. Add Filed Value
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	//pPacker->AddStr( "F4" );				///< futu_exch_type F1-CZCE F2-DCE F3-SHFE F4-CFFEX
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
		Msg.printf( L"SendBizMsg SendQryExchStatus packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> SendQryExchStatus SendBizMsg function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
}
//---------------------------------------------------------------------------
void TTradingForm::SendQryExchTime( void )
{
	int          RtnVal;
	String       Msg;
	IBizMessage* lpBizMessage = NULL;
	IF2Packer*   pPacker      = NULL;

	lpBizMessage = gHSufxAPI.pNewBizMessage();
	if ( lpBizMessage == NULL )
	{
		Memo1->Lines->Add( "When SendQryExchTime() NewBizMessage() failed!!" );
		return;
	}
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction( 330852 );
	lpBizMessage->SetSystemNo( gSysNodeID );
	lpBizMessage->SetPacketType( REQUEST_PACKET );

	// 1. New Packer
	pPacker = gHSufxAPI.pNewPacker(2); ///< 業務包格式版本(取值:1 字串版,其他值 0x20版), 推薦使用0x20版
	if ( pPacker == NULL )
	{
		Memo1->Lines->Add( "When SendQryExchTime() NewPacker() failed!!" );
		lpBizMessage->Release();
		return;
	}
	pPacker->AddRef();
	pPacker->BeginPack();
	// 2. Add Field Name
	pPacker->AddField( "user_token", 'S', 40 );
	pPacker->AddField( "op_branch_no", 'I', 5 );
	pPacker->AddField( "op_entrust_way", 'C', 1 );
	pPacker->AddField( "op_station", 'S', 255 );
	pPacker->AddField( "branch_no", 'I', 5 );
	pPacker->AddField( "client_id", 'S', 18 );
	pPacker->AddField( "fund_account", 'S', 18 );
	pPacker->AddField( "password", 'S', 50 );
	// 3. Add Filed Value
	pPacker->AddStr( FUserToken );			///< user_token
	pPacker->AddInt( gBrokerID );			///< op_branch_no
	pPacker->AddChar( '7' );				///< op_entrust_way
	pPacker->AddStr( "1" );					///< op_station
	pPacker->AddInt( gBrokerID );			///< branch_no
	pPacker->AddStr( FClientID );			///< client_id
	pPacker->AddStr( gUserID.c_str() );		///< fund_account
	pPacker->AddStr( gPassword.c_str() );	///< password
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
		Msg.printf( L"SendBizMsg SendQryExchTime packet failed!! ErrorCode[%d] ErrorMsg[%s]", RtnVal, String(GBStr) );
		Memo1->Lines->Add( Msg );
	}
	else
	{
		Msg.printf( L">>>>>> SendQryExchTime SendBizMsg function call return. RtnVal[%d]", RtnVal );
		Memo1->Lines->Add( Msg );
	}
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
	AnsiString PositionStr;
	ShowPacket( TRUE, Packet, &PositionStr );
	if ( Packet->GetRowCount() == TxtRequestNum->Text.ToInt() &&
		 PositionStr.Length() != 0 )
	{
		if ( BizName == "RecvQryOrder" )
			SendQryOrderMsg( &PositionStr );
		else if ( BizName == "RecvQryFill" )
			SendQryFillMsg( &PositionStr );
		else if ( BizName == "RecvQrySymbol" )
			SendQrySymbolMsg( &PositionStr );
	}
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
		BtnExchStatus->Enabled    = true;
		BtnChangePwd->Enabled     = true;
		BtnPosition->Enabled      = true;
		BtnInitPosition->Enabled  = true;
		BtnAccount->Enabled       = true;
		BtnQrySymbol->Enabled     = true;
		BtnLogoff->Enabled        = true;
		BtnDisconnect->Enabled    = true;
		BtnQryCommission->Enabled = true;
		BtnQryExchStatus->Enabled = true;
		BtnQryExchTime->Enabled   = true;
		BtnConn_Login->Enabled    = false;
	}
	else
	{
		BtnNewOrder->Enabled      = false;
		BtnCancelOrder->Enabled   = false;
		BtnQryOrder->Enabled      = false;
		BtnQryFill->Enabled       = false;
		BtnConnStatus->Enabled    = false;
		BtnSubPush->Enabled       = false;
		BtnExchStatus->Enabled    = false;
		BtnChangePwd->Enabled     = false;
		BtnPosition->Enabled      = false;
		BtnInitPosition->Enabled  = false;
		BtnAccount->Enabled       = false;
		BtnQrySymbol->Enabled     = false;
		BtnLogoff->Enabled        = false;
		BtnDisconnect->Enabled    = false;
		BtnQryCommission->Enabled = false;
		BtnQryExchStatus->Enabled = false;
		BtnQryExchTime->Enabled   = false;
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
	// 1. Subscribe Order Confirm & Fill push ( 620001-33101 )
	SendSubPushMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnExchStatusClick(TObject *Sender)
{
	// 1. Subscribe Order Confirm & Fill push ( 620001-33105 )
	SendSubExchStatus( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnChangePwdClick(TObject *Sender)
{
	SendChangePwdMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnPositionClick(TObject *Sender)
{
	SendPositionMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnInitPositionClick(TObject *Sender)
{
	SendInitPositionMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnAccountClick(TObject *Sender)
{
	SendAccountMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnQrySymbolClick(TObject *Sender)
{
	SendQrySymbolMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnNewOrderClick(TObject *Sender)
{
	SendNewOrderMsg( );
	gOrderRef++;
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnCancelOrderClick(TObject *Sender)
{
	SendCancelOrderMsg( );
	gOrderRef++;
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnQryOrderClick(TObject *Sender)
{
	SendQryOrderMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnQryFillClick(TObject *Sender)
{
	SendQryFillMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnQryCommissionClick(TObject *Sender)
{
	SendQryCommissionMsg( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnQryExchStatusClick(TObject *Sender)
{
	SendQryExchStatus( );
}
//---------------------------------------------------------------------------
void __fastcall TTradingForm::BtnQryExchTimeClick(TObject *Sender)
{
	SendQryExchTime( );
}
//---------------------------------------------------------------------------

