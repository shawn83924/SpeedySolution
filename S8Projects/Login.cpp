//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Login.h"
#include "main.h"
#include "UnifyDlg.h"
#include "FMTConfig.h"
#include "UnifyUtility.h"
#include "WebBrowserForm.h"
#include "MD5.h"
#include <windowsx.h>
#include <System.JSON.hpp>
#include <System.IOUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "GraphButton"
#pragma resource "*.dfm"
TLoginForm *LoginForm;
//---------------------------------------------------------------------------
extern TUnifyUser gUser;
extern bool       GIsTestingMode;
extern bool       GSimMatch;
extern bool       gIsExpired;
extern bool       GVIPServer;
//---------------------------------------------------------------------------
const int FREE_DAYS = 3;
//---------------------------------------------------------------------------
__fastcall TLoginForm::TLoginForm(TComponent* Owner)
	: TForm(Owner)
{
	ActivityIndicator->Visible = false;
	ActivityIndicator->Animate = false;
	if( GSimMatch == false )
	{
		if( GIsTestingMode == true )
			EnvLabel->Caption = L"測試環境";
		else
			EnvLabel->Caption = L"正式環境";
		NetworkComboBox->Visible = false;
		CalendarButton->Visible  = false;
	}
	else
	{
		EnvLabel->Caption = L"練功模式";
		NetworkComboBox->Visible = true;
		CalendarButton->Visible  = true;
		AtAOELabel->Visible = false;
		AtAOECheckBox->Visible = false;
	}
	int     AppVer = g_Config.GetVersion();
	String  APVerStr;

	APVerStr.printf(L"版本: %d.%d.%d", AppVer/1000000,	(AppVer%1000000)/10000, (AppVer%10000)/100 );
	FVersion.printf(L"%d.%d.%d", AppVer/1000000,	(AppVer%1000000)/10000, (AppVer%10000)/100 );
	VersionLabel->Caption = APVerStr;
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::WndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case WM_ERASEBKGND: Msg.Result = 1; return;
		case WM_NCHITTEST:  OnNCHitTest( Msg );	return;
	}
	inherited::WndProc( Msg );
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::OnNCHitTest( TMessage &Message )
{
	POINT point;
	RECT  WinRect,SysRect,CaptionRect;

	GetWindowRect( Handle, &WinRect);
	point.x = GET_X_LPARAM( Message.LParam ) - WinRect.left;
	point.y = GET_Y_LPARAM( Message.LParam ) - WinRect.top;
	if( point.y < 240 )
	{
		Message.Result = HTCAPTION;
		return;
	}
	inherited::WndProc( Message );
}
//---------------------------------------------------------------------------
bool __fastcall TLoginForm::CheckFreeTry( void )
{
	if( gIsExpired == true ) ///< Needs check N Days free use.
	{
		UFC::AnsiString Today;
		UFC::GetYYYYMMDD( Today );
		String LastDate( Today.c_str() );
		String DayCount( L"1");
		String Msg, Val;

		///< Value not exists, means it's first time use. logon oK!
		if( g_Config.GetServerValue( "LastDate", LastDate, Today.c_str()) == false ||
			g_Config.GetServerValue( "DayCount", DayCount, "1" ) == false )
		{
			g_Config.SetServerValue( "LastDate", Today.c_str() );
			g_Config.SetServerValue( "DayCount", "1" );
			TUnifyDlgs::MessageDialog( "Speedy Unify", "謝謝您的試用!每個月您可以使用三天練功房." );
		}
		else
		{
			int DC = DayCount.ToInt();
			int DT = LastDate.ToInt();
			int DTN = Today.ToInt();
			int LeavesDay;

			if( DT == DTN )///< Same day, pass!
			{
				if( DC > FREE_DAYS )
				{
					StatusLabel->Caption = L"這個月的三天免費試用已用完!想無限制使用盤模擬功能,請訂閱正式版.";
					return false;
				}
				else
				{
					LeavesDay = FREE_DAYS - DC;
					if( LeavesDay > 0 )
						Msg.printf( L"謝謝您的試用!除了今天之外,這個月您還可以試用%d天.", LeavesDay ); ///< Pass!
					else
						Msg.printf( L"謝謝您的試用!今天是你這個月的最後一天試用,下個月您又可以試用三天.");
					TUnifyDlgs::MessageDialog( "Speedy Unify", Msg );
				}
			}
			else
			{
			   if( DTN/100 - DT/100 >= 1 ) ///< Next month, new N free days.
			   {
				  g_Config.SetServerValue( "LastDate", Today.c_str() );
				  g_Config.SetServerValue( "DayCount", "1" );
				  TUnifyDlgs::MessageDialog( "Speedy Unify", "謝謝您的試用!每個月您可以使用三天練功房." );
			   }
			   else
			   {
				  if( DC < FREE_DAYS )
				  {
					  DC++;
					  Val.printf( L"%d", DC );
					  g_Config.SetServerValue( "LastDate", Today.c_str() );
					  g_Config.SetServerValue( "DayCount", Val);
					  LeavesDay = FREE_DAYS - DC;
					  if( LeavesDay > 0 )
						  Msg.printf( L"謝謝您的試用!除了今天之外,這個月您還可以試用%d天.", LeavesDay ); ///< Pass!
					  else
						  Msg.printf( L"謝謝您的試用!今天是你這個月的最後一天試用,下個月您又可以試用三天.");
					  TUnifyDlgs::MessageDialog( "Speedy Unify", Msg );
				  }
				  else
				  {
					  StatusLabel->Caption = L"這個月的三天免費試用已用完!想無限制使用盤模擬功能,請訂閱正式版.";
					  return false;
				  }

			   }
			}
		}
	}
	return true;
}
//---------------------------------------------------------------------------
// 0 AOE-Internet
// 1 AOE-Office
// 2 Mega Real market data
// 3~N Tranning room
//---------------------------------------------------------------------------
void __fastcall TLoginForm::LoginButtonClick(TObject *Sender)
{
	String Msg;
	bool  NeedLicense = false;
	bool  Tranning = false;

    ExLabel->Visible = false;
	LoginButton->Enabled = false;
	Msg.printf( L"用戶[%s]嘗試登入系統",IDEdit->Text );
	StatusLabel->Caption = Msg;
	Application->ProcessMessages();

	GVIPServer = g_Config.IamVIP( IDEdit->Text );
	if( GSimMatch == true )
	{
		if( NetworkComboBox->ItemIndex == 2 ) ///< Production Market Data
			NeedLicense = true;
		else
			GVIPServer = false;
		if( NetworkComboBox->ItemIndex > 2 )
			Tranning = true;
		g_Config.SetInternetConfig( NetworkComboBox->ItemIndex );
		g_Config.SetIntegerProperty("Setting","InternetConfig",  NetworkComboBox->ItemIndex );
	}
	else
		g_Config.SetInternetConfig( 0 );


	g_Config.LoadStarWaveSettingIni();
	if( Logon() )
	{
		//g_Config.UserRightsInfo();
		if( Tranning == true ) ///< Traning room not production.
		{
			if( CheckFreeTry() == false )
			{
				LoginButton->Enabled = true;
				return;
            }
		}
		if( gIsExpired == true && NeedLicense == true) ///< Traning room production. but no license
		{
			LoginButton->Enabled = true;
			StatusLabel->Caption = L"謝謝您的試用!想開啟實盤模擬功能,請訂閱正式版.";
			return;
		}
		ActivityIndicator->Visible = true;
		ActivityIndicator->Animate = true;
		IDEdit->Visible = false;
		PasswordEdit->Visible = false;
		FWaitCount = 0;
		MainForm->Connect();
		WaitTimer->Enabled = true;
		///< Save ID/Password
		SaveIDPassword( );
	}
	else
		LoginButton->Enabled = true;
	g_Config.SetBoolProperty("Setting","SaveID",AccountCheckBox->Checked);
	g_Config.SetBoolProperty("Setting","SavePassword",SavePasswordCheckBox->Checked );
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::WaitTimerTimer(TObject *Sender)
{
	FWaitCount++;
	if( MainForm->Ready() == true )
	{
		WaitTimer->Enabled = false;
		MainForm->InitOrderStore();
		LoginButton->Enabled = true;
		IDEdit->Visible = true;
		PasswordEdit->Visible = true;
		ActivityIndicator->Visible = false;
		ActivityIndicator->Animate = false;
		this->ModalResult = mrOk;
	}
	else if( FWaitCount > 100 || MainForm->ConnectionFailed() )
	{
		LoginButton->Enabled = true;
		WaitTimer->Enabled = false;
		this->ModalResult = mrNone;//Cancel;
		IDEdit->Visible = true;
		PasswordEdit->Visible = true;
		ActivityIndicator->Visible = false;
		ActivityIndicator->Animate = false;
		StatusLabel->Caption = L"連線失敗,請檢查網路!";
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::ExitButtonClick(TObject *Sender)
{
	Application->Terminate();
}
//---------------------------------------------------------------------------

void __fastcall TLoginForm::LabelMouseEnter(TObject *Sender)
{
	TLabel* Label = dynamic_cast< TLabel* >( Sender);
	if( Label != NULL)
		Label->Font->Color = clWhite;
}
//---------------------------------------------------------------------------

void __fastcall TLoginForm::LabelMouseLeave(TObject *Sender)
{
	TLabel* Label = dynamic_cast< TLabel* >( Sender);
	if( Label != NULL)
		Label->Font->Color = clSilver;
}
//---------------------------------------------------------------------------

void __fastcall TLoginForm::SaveAccountLabelClick(TObject *Sender)
{
	AccountCheckBox->Checked = ! AccountCheckBox->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::SavePasswordLabelClick(TObject *Sender)
{
	SavePasswordCheckBox->Checked = ! SavePasswordCheckBox->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::AtAOELabelClick(TObject *Sender)
{
	 AtAOECheckBox->Checked = ! AtAOECheckBox->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::JoinMemberLabelClick(TObject *Sender)
{
	BrowserForm = new TBrowserForm( this );
	BrowserForm->Caption = L"成為新會員";
	BrowserForm->WebBrowser->Navigate( g_Config.GetRegisterURL( ) );
	BrowserForm->WaitLoading( );
	if( BrowserForm->ShowModal() == mrOk )
	{
		IDEdit->Text = BrowserForm->LoginID;
		PasswordEdit->Text = L"";
	}
	delete BrowserForm;
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::ResetPasswordLabelClick(TObject *Sender)
{
	BrowserForm = new TBrowserForm( this );
	BrowserForm->Caption = L"忘記密碼";
	BrowserForm->WebBrowser->Navigate( g_Config.GetForgetPasswordURL( ) );
	BrowserForm->WaitLoading( );
	if( BrowserForm->ShowModal() == mrOk )
	{
		IDEdit->Text = BrowserForm->LoginID;
		PasswordEdit->Text = L"";
	}
	delete BrowserForm;
}
//---------------------------------------------------------------------------
bool __fastcall TLoginForm::GetResponseJSON( TMemoryStream* Stream, String& ResponseJSON )
{
	UTF8String  JsonTxt( (char*)Stream->Memory, Stream->Size );
	TJSONValue *lpJson = TJSONObject::ParseJSONValue( JsonTxt );
	TJSONObject *lpRoot = dynamic_cast<TJSONObject *>(lpJson);
	if( lpRoot != NULL )
	{
		ResponseJSON = lpRoot->Values[ "response_data" ]->Value();
		delete lpJson;
		return true;
	}
	delete lpJson;
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TLoginForm::Logon( void )
{
	TMemoryStream* ResultStream = new TMemoryStream();
	if( RequestLogon( IDEdit->Text, PasswordEdit->Text, ResultStream ) )
	{
		bool IsOk = false;
		String ResponseJSON;
		if( GetResponseJSON( ResultStream, ResponseJSON ) )
		{
			TJSONValue *lpJson = TJSONObject::ParseJSONValue( ResponseJSON );
			TJSONObject *lpRoot = dynamic_cast<TJSONObject *>(lpJson);
			if( lpRoot != NULL )
			{
				String ResultStr  = lpRoot->Values[L"result"]->Value();
				UTF8String MessageStr( lpRoot->Values[L"message"]->Value() );
				if( ResultStr == L"0" )
				{
					gUser.UserID   = IDEdit->Text;
					//g_Config.LoadUserInfo( lpRoot );
					StatusLabel->Caption = L"登入成功!";
					IsOk = true;
				}
				else
				   StatusLabel->Caption = L"登入失敗:" + MessageStr;
			}
		}
		delete ResultStream;
		return IsOk;
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::GenData( const String& ID, const String& Password, String& Out )
{
	AnsiString IDpPasswd = ID + Password + L"1";
	UFC::MD5   ChkSum( (const UFC::UInt8*)IDpPasswd.c_str(), IDpPasswd.Length() );
	String     CheckSum( ChkSum.ToString().c_str() );
	Out.printf( L"{\"account\":\"%s\",\"pwd\":\"%s\",\"cp\":\"1\",\"checksum\":\"%s\"}",ID,Password,CheckSum );
}
//---------------------------------------------------------------------------
bool __fastcall TLoginForm::RequestLogon( const String& ID, const String& Password, TMemoryStream* OutStream )
{
	TIdSSLIOHandlerSocketOpenSSL* SSLIOHandler = new TIdSSLIOHandlerSocketOpenSSL( NULL );
	TIdHTTP*                      pHTTP        = new TIdHTTP( NULL );
	TMemoryStream*                SourceStream = new TMemoryStream();
	String                        URL,Msg,Data;
	bool                          Result = true;

	try
	{
		String URLPrefix = g_Config.GetLogonURL();

		GenData( ID, Password, Data );
		URL.printf( L"%s?data=%s&txid=IUN_O_01&vender=UnifyAP",URLPrefix, Data );
		if( URL.Pos( L"https" ) != 0 ) ///< Use https
		{
			SSLIOHandler->SSLOptions->Method = sslvSSLv23;
			pHTTP->IOHandler = SSLIOHandler;
		}
		pHTTP->ConnectTimeout = 90000; ///< 90 sec IIS default(Paul said)
		pHTTP->Post( pHTTP->URL->URLEncode( URL ), SourceStream, OutStream );
	}
	catch( Exception& ex )
	{
		Result = false;
		TUnifyDlgs::MessageDialog( "Speedy Unify", ex.ToString() );
	}
	delete SSLIOHandler;
	delete pHTTP;
	delete SourceStream;;
	return Result;
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::FormShow(TObject *Sender)
{
   NetworkComboBox->Items->Clear();
   NetworkComboBox->Items->AddStrings( g_Config.GetInternetConfigNames() );
   AccountCheckBox->Checked      = g_Config.GetBoolProperty("Setting","SaveID",true);
   SavePasswordCheckBox->Checked = g_Config.GetBoolProperty("Setting","SavePassword",true);
   NetworkComboBox->ItemIndex	 = g_Config.GetIntegerProperty("Setting","InternetConfig", 0 );
   ExLabel->Visible = true;
   LoadIDPassword();
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::IDEditEnter(TObject *Sender)
{
	if( IDEdit->Text == L"會員帳號" )
		IDEdit->Text = "";
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::PasswordEditEnter(TObject *Sender)
{
	if( PasswordEdit->Text == L"123456" )
		PasswordEdit->Text = "";
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::LoadIDPassword( void )
{
   ///< Account saved ?
   if( AccountCheckBox->Checked == true ) ///< Yes
	   IDEdit->Text	= g_Config.GetBase64StringProperty("Setting","ID","會員帳號" );
   else
	   IDEdit->Text	= L"";
   ///< Password saved ?
   if( SavePasswordCheckBox->Checked == true ) ///< Yes
	   PasswordEdit->Text = g_Config.GetBase64StringProperty("Setting","Password","123456" );
   else
	   PasswordEdit->Text	= "";
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::SaveIDPassword( void )
{
	///< Account saved ?
	if( AccountCheckBox->Checked == true )
		g_Config.SetBase64StringProperty("Setting","ID",IDEdit->Text );
	///< Password saved ?
	if( SavePasswordCheckBox->Checked == true )
		g_Config.SetBase64StringProperty("Setting","Password", PasswordEdit->Text );
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::CalendarButtonClick(TObject *Sender)
{
	BrowserForm = new TBrowserForm( this );
	BrowserForm->Caption = L"模擬盤行事曆";
	BrowserForm->WebBrowser->Navigate( L"https://unify.data-bee.com/download/calendar.html" );
	BrowserForm->WaitLoading( );
	if( BrowserForm->ShowModal() == mrOk )
	{
		IDEdit->Text = BrowserForm->LoginID;
		PasswordEdit->Text = L"";
	}
	delete BrowserForm;
}
//---------------------------------------------------------------------------

