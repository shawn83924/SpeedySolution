//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LoginSetting.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TLoginSettingForm *LoginSettingForm;
const int SettingListCount = 9;
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
	"ProxyLogon"
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
	true
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
		String value = SettingListEditor->Values[key];
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
