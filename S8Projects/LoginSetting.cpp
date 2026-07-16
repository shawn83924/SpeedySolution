//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LoginSetting.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TLoginSettingForm *LoginSettingForm;
//---------------------------------------------------------------------------
__fastcall TLoginSettingForm::TLoginSettingForm(TComponent* Owner)
	: TForm(Owner)
{
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
void __fastcall TLoginSettingForm::FormShow(TObject *Sender)
{
	LoadConfigSetting("LiteService.ini");
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

	for (int i = 1; i < ValueListEditor->RowCount; i++)
	{
		String key = ValueListEditor->Keys[i];
		if(!config.GetValue( "Setting", key.c_str(), value))
			continue;

		ValueListEditor->Values[key] = value.c_str();
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
	for (int i = 1; i < ValueListEditor->RowCount; i++)
	{
		String key = ValueListEditor->Keys[i];
		String value = ValueListEditor->Values[key];
		if (value.IsEmpty() || value == "(null)")
			config.SetValue( "Setting", key.c_str(), " " );
		else
			config.SetValue( "Setting", key.c_str(), value.c_str() );
	}
	config.Save();
}
