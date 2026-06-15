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
#pragma link "AlignEdit"
#pragma resource "*.dfm"
TLoginForm *LoginForm;
//---------------------------------------------------------------------------
extern TUnifyUser gUser;
extern bool       GIsTestingMode;
extern bool       GSimMatch;
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
	MainForm->InitOrderStore();
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
	if( !Logon() )
	{
		LoginButton->Enabled = true;
		g_Config.SetBoolProperty("Setting","SaveLoginInfo",AccountCheckBox->Checked);
		return;
	}
	if (Tranning == true) ///< Traning room not production.
	{
		if (CheckFreeTry() == false)
		{
			LoginButton->Enabled = true;
			return;
		}
	}
	ActivityIndicator->Visible = true;
	ActivityIndicator->Animate = true;
	IDEdit->Enabled = false;
	AccountEdit->Enabled = false;
	PasswordEdit->Enabled = false;
	FWaitCount = 0;
	MainForm->Connect();
	WaitTimer->Enabled = true;
	///< Save ID/Password
	SaveIDPassword();
	g_Config.SetBoolProperty("Setting","SaveLoginInfo",AccountCheckBox->Checked);;
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::WaitTimerTimer(TObject *Sender)
{
	FWaitCount++;
	if( MainForm->Ready() == true )
	{
		WaitTimer->Enabled = false;
		LoginButton->Enabled = true;
		IDEdit->Enabled = true;
		AccountEdit->Enabled = true;
		PasswordEdit->Enabled = true;
		ActivityIndicator->Visible = false;
		ActivityIndicator->Animate = false;
		this->ModalResult = mrOk;
	}
	else if( FWaitCount > 100 || MainForm->ConnectionFailed() )
	{
		LoginButton->Enabled = true;
		WaitTimer->Enabled = false;
		this->ModalResult = mrNone;//Cancel;
		IDEdit->Enabled = true;
		AccountEdit->Enabled = true;
		PasswordEdit->Enabled = true;
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
void __fastcall TLoginForm::AtAOELabelClick(TObject *Sender)
{
	 AtAOECheckBox->Checked = ! AtAOECheckBox->Checked;
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
	if( !RequestLogon(IDEdit->Text, AccountEdit->Text, PasswordEdit->Text) )
	{
		StatusLabel->Caption = L"登入失敗:";
		return false;
	}

	gUser.UserID		= IDEdit->Text;
	gUser.LoginUserID 	= IDEdit->Text;
	gUser.AccountType   = hatTWSE;
	StatusLabel->Caption = L"登入成功!";
	return true;
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
bool __fastcall TLoginForm::RequestLogon(
	const String& ID,
	const String& Account,
	const String& Password)
{
	TBrokerConfig* loginBroker = g_Config.BrokerConfig( 0 );
	String errMsg;
	if (loginBroker->GetService()->LoginBroker(ID, Account, Password, errMsg) != true)
	{
		TUnifyDlgs::MessageDialog(Mdcomponentstrings_MD_SpeedyUnify_AppName, errMsg);
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::FormShow(TObject *Sender)
{
   NetworkComboBox->Items->Clear();
   NetworkComboBox->Items->AddStrings( g_Config.GetInternetConfigNames() );
   AccountCheckBox->Checked      = g_Config.GetBoolProperty("Setting","SaveLoginInfo",true);
   NetworkComboBox->ItemIndex	 = g_Config.GetIntegerProperty("Setting","InternetConfig", 0 );
   ExLabel->Visible = true;
   LoadIDPassword();
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::LoadIDPassword( void )
{
   if( AccountCheckBox->Checked == true )
   {
	   IDEdit->Text	= g_Config.GetBase64StringProperty("Setting", "ID", "" );
	   AccountEdit->Text = g_Config.GetBase64StringProperty("Setting", "Account", "" );
	   PasswordEdit->Text = g_Config.GetBase64StringProperty("Setting", "Password", "" );
   }
   else
   {
	   IDEdit->Text	= L"";
	   AccountEdit->Text = "";
	   PasswordEdit->Text = "";
   }
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::SaveIDPassword( void )
{
	///< Account saved ?
	if( AccountCheckBox->Checked == false )
		return;

	g_Config.SetBase64StringProperty("Setting","ID",IDEdit->Text );
	g_Config.SetBase64StringProperty("Setting","Account",AccountEdit->Text );
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

void __fastcall TLoginForm::LoginSettingButtonClick(TObject *Sender)
{
	ShellExecute(
		Handle,
		L"open",
		L"LiteService.ini",
		NULL,
		NULL,
		SW_SHOWNORMAL
	);
}
//---------------------------------------------------------------------------

