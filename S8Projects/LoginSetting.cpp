//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <Vcl.Dialogs.hpp>
#include "LoginSetting.h"
#include "UnifyUtility.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TLoginSettingForm *LoginSettingForm;
const String MD_PASS_KEY  = L"MD_Pass";
const String MD_PASS_MASK = L"********";
const int SettingListCount = 14;
String TitleListStr[SettingListCount] =
{
	"IP",
	"Port",
	"FutBrokerID",
	"StockBrokerID",
	"ClearMemberID",
	"StockAccount",
	"FutAccount",
	"TryVersion",
	"ProxyLogon",
    "PositionPort",
	"MD",
	"MD_ID",
	"MD_Pass",
	"Chart"
};
bool TitleIsPickList[SettingListCount] =
{
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	true,
	true,
    false,
	false,
	false,
	false,
	false
};
//---------------------------------------------------------------------------
__fastcall TLoginSettingForm::TLoginSettingForm(TComponent* Owner)
	: TForm(Owner)
{
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
void __fastcall TLoginSettingForm::FormShow(TObject *Sender)
{
	InitSettingListEditor();
	LoadConfigSetting("LiteService.ini");
}
//---------------------------------------------------------------------------
void __fastcall TLoginSettingForm::InitSettingListEditor( void )
{
	for(int i=0; i < SettingListCount; i++)
	{
		String title = TitleListStr[i];
        SettingListEditor->InsertRow(title, "", true);
	}
	// 只改顯示名稱，ini 的 Key 仍為 MD 與 Chart
	SettingListEditor->ItemProps[String( L"MD" )]->KeyDesc    = L"MD_IP";
	SettingListEditor->ItemProps[String( L"Chart" )]->KeyDesc = L"Chart_IP";
	// 密碼不放進表格（InplaceEditor 為多行輸入框，無法可靠遮罩），改由按鈕開啟密碼輸入框
	Vcl::Valedit::TItemProp *passProp = SettingListEditor->ItemProps[MD_PASS_KEY];
	passProp->EditStyle = esEllipsis;
	passProp->ReadOnly  = true;
}
//---------------------------------------------------------------------------
void __fastcall TLoginSettingForm::LoadConfigSetting(const char* FileName)
{
	if( UFC::FileExists( FileName ) == false )
	{
		UFC::BufferedLog::Printf( "找不到設定檔: %s", FileName );
		return;
	}

	UFC::UiniFile   config( FileName, true );
	UFC::AnsiString value;
	for(int i=0; i < SettingListCount; i++)
	{
		String key = TitleListStr[i];
		if(!config.GetValue( "Setting", key.c_str(), value))
			continue;

		if(TitleIsPickList[i] == true)
		{
			Vcl::Valedit::TItemProp *prop = SettingListEditor->ItemProps[key];
			prop->EditStyle = esPickList;
			prop->PickList->Add("True");
			prop->PickList->Add("False");
            SettingListEditor->Values[key] = (value == "True")? "True":"False";
		}
		else if(key == MD_PASS_KEY)
		{
			FMDPass = Base64Decode( value.c_str() );
			SettingListEditor->Values[key] = FMDPass.IsEmpty() ? String() : MD_PASS_MASK;
		}
		else
		{
			SettingListEditor->Values[key] = value.c_str();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoginSettingForm::OKButtonClick(TObject *Sender)
{
	SaveConfigSetting("LiteService.ini");
}
//---------------------------------------------------------------------------
void __fastcall TLoginSettingForm::SaveConfigSetting(const char* FileName)
{
	if (UFC::FileExists( FileName ) == false )
	{
		UFC::BufferedLog::Printf( "找不到設定檔: %s", FileName );
		return;
	}

	UFC::UiniFile   config( FileName, true );
	for (int i = 1; i < SettingListEditor->RowCount; i++)
	{
		String key = SettingListEditor->Keys[i];
		String value = (key == MD_PASS_KEY) ? Base64Encode( FMDPass ) : SettingListEditor->Values[key];
		if (value.IsEmpty() || value == "(null)")
			config.SetValue( "Setting", key.c_str(), " " );
		else
			config.SetValue( "Setting", key.c_str(), value.c_str() );
	}
	config.Save();
}
//---------------------------------------------------------------------------
void __fastcall TLoginSettingForm::SettingListEditorKeyPress(TObject *Sender, System::WideChar &Key)
{
	int row = SettingListEditor->Row;

	if (TitleIsPickList[row - 1] != true)
		return;

	// 允許控制鍵
	if (Key != VK_BACK)
	{
		Key = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLoginSettingForm::SettingListEditorEditButtonClick(TObject *Sender)
{
	if( SettingListEditor->Keys[ SettingListEditor->Row ] != MD_PASS_KEY )
		return;

	// 提示字串第一個字元小於 #32 時，InputQuery 的輸入框會以 * 遮罩
	String pass = FMDPass;
	if( InputQuery( L"MD_Pass", String( L'\x1F' ) + L"請輸入 MD 密碼:", pass ) == false )
		return;

	FMDPass = pass;
	SettingListEditor->Values[MD_PASS_KEY] = FMDPass.IsEmpty() ? String() : MD_PASS_MASK;
}
//---------------------------------------------------------------------------
