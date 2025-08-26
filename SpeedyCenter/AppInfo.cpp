//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "AppInfo.h"
#include "LinkFrame.h"
#include "..\UFC\iniFile.h"
#include "SimTFXMain.h"
#include "SCStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "IniEditBox"
#pragma resource "*.dfm"
TAppInfoForm *AppInfoForm;
//---------------------------------------------------------------------------
extern AnsiString CurrentDir;
//---------------------------------------------------------------------------
__fastcall TAppInfoForm::TAppInfoForm(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------
TModalResult __fastcall TAppInfoForm::Show( TSimTFXForm* OF, AnsiString File )
{
	AnsiString  FileName = CurrentDir +"\\Config.ini";

	FOwnerForm = OF;
    Caption = "Setting " + File;
    IniEditBox->Load( FileName );
    PageControl->ActivePage = SettingSheet;
    if( ShowModal() == mrOk )
    {
        if( IniEditBox->Modified() == true )
        {
            IniEditBox->Save();
            return mrOk;
        }
    }
    return mrCancel;
}
//---------------------------------------------------------------------------
TModalResult __fastcall TAppInfoForm::ShowSpeedy( TSimTFXForm* OF, TMarket Market )
{
	bool KSEnable = false;

	Caption    = L"About MDBS Speedy";
	FOwnerForm = OF;
	FMarket    = Market;
	PageControl->ActivePage = SpeedySheet;
	switch( Market )
	{
		case mtFutures:	SpeedyLabel->Caption = L"Speedy for TAIFEX Futures";
						KSEnable = FOwnerForm->IsProcessRunning( "SpeedyFUT" );
						CancelAllGroupBox->Visible = true;
						KSGroupBox->Visible = true;
						UpdateFCMList();
						break;
		case mtOption:  SpeedyLabel->Caption = L"Speedy for TAIFEX Options";
						KSEnable = FOwnerForm->IsProcessRunning( "SpeedyOPT" );
						CancelAllGroupBox->Visible = true;
						KSGroupBox->Visible = true;
						UpdateFCMList();
						break;
		case mtTAIFEX:
		case mtTW:
						KSEnable = FOwnerForm->IsProcessRunning( "SpeedyTW" );
						CancelAllGroupBox->Visible = true;
						KSGroupBox->Visible = true;
						if( MessageDlg( L"顯示期貨KillSwitch畫面?選No顯示選擇權KillSwitch畫面.)", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0) == mrYes )
						{
							FMarket = mtFutures;
							SpeedyLabel->Caption = L"Speedy TW Futures";
						}
						else
						{
							FMarket = mtOption;
							SpeedyLabel->Caption = L"Speedy TW Options";
						}
						UpdateFCMList();
						break;
		case mtStock:   SpeedyLabel->Caption = L"Speedy for TSEC";
						CancelAllGroupBox->Visible = false;
						KSGroupBox->Visible = false;
						break;
		case mtOTC:     SpeedyLabel->Caption = L"Speedy for OTC";
						CancelAllGroupBox->Visible = false;
						KSGroupBox->Visible = false;
						break;
	}
	CancelAllGroupBox->Enabled = KSEnable;
	KSGroupBox->Enabled = KSEnable;
	return ShowModal();
}
//---------------------------------------------------------------------------
TModalResult __fastcall TAppInfoForm::ShowSpiderNet( TSimTFXForm* OF, int Index, const AnsiString& Host )
{
	AnsiString       FileName = CurrentDir +"\\"+Host+"-Speedy.ini";
    UFC::UiniFile    ini( FileName.c_str() );
    UFC::Section*    iniSection = ini.GetSection( Index );
    UFC::AnsiString  Param;
    TModalResult     Result;

	FOwnerForm = OF;
	Caption = L"SpiderNet Setting";
    FItemIndex = Index;
    PageControl->ActivePage = SpiderNetSheet;
    ///< Default settings
    MTCheckBox->Checked = false;
    HACheckBox->Checked = true;
    FIXNameRadioGroup->ItemIndex = 0;
    NameEdit->Enabled = false;
    NameEdit->Text = "";
    if( iniSection && iniSection->GetValue( "Param",  Param ) == true )
    {
        TStringList* Params = new TStringList();
        Params->Delimiter = ' ';
        Params->DelimitedText = Param.c_str();
        for( int i = 0; i < Params->Count; i ++ )
        {
            AnsiString Opt = Params->Strings[i];
            if( Opt.AnsiPos("-MT") != 0 || Opt.AnsiPos("-mt") != 0 )
                MTCheckBox->Checked = true;
            if( Opt.AnsiPos("-S") != 0 || Opt.AnsiPos("-s") != 0 )
                HACheckBox->Checked = false;
            if( Opt.AnsiPos( "-N" ) != 0  || Opt.AnsiPos( "-n" ) != 0 )
            {
                FIXNameRadioGroup->ItemIndex = 1;
                NameEdit->Enabled = true;
                NameEdit->Text = Opt.c_str() + 2;///< Skip "-N"
            }
        }
        delete  Params;
    }
    if( (Result = ShowModal())== mrOk  && iniSection )
    {
		AnsiString  Param;
		AnsiString  Name = NameEdit->Text;

		if( MTCheckBox->Checked == true )
			Param = Param + " -MT";
		if( HACheckBox->Checked == false )
			Param = Param + " -S";
		if( FIXNameRadioGroup->ItemIndex == 1 )
			Param = Param + " -N" + Name;
		if( Param.Length() == 0 )
            iniSection->DeleteValue( "Param" );
        else
			iniSection->SetValue( "Param", Param.c_str() );
        ini.Save();
    }
    return Result;
}
//---------------------------------------------------------------------------
void __fastcall TAppInfoForm::FIXNameRadioGroupClick(TObject *Sender)
{
    if( FIXNameRadioGroup->ItemIndex == 0 )
    {
        NameEdit->Enabled = false;
		NameEdit->Text = L"";
	}
    else
    {
        NameEdit->Enabled = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TAppInfoForm::OKBitBtnClick(TObject *Sender)
{
    if( PageControl->ActivePage == SpiderNetSheet )
    {
        if( FIXNameRadioGroup->ItemIndex == 1  && NameEdit->Text.Length() < 5 )
        {
			ShowMessage( Scstrings_MAIN_APP_INFO_FIXENGINE_NAME_NEED_LEN5 );
			ModalResult = mrNone;
        }
	}
	PasswordEdit->Text = "";
	KSPasswordEdit->Text = "";
}
//---------------------------------------------------------------------------
void __fastcall TAppInfoForm::CancelAllBtnClick(TObject *Sender)
{
	if( PasswordEdit->Text.Length() == 0 )
	{
		ShowMessage( Scstrings_MAIN_APP_INFO_INPUT_SC_PASSWORD );
		return;
	}
	if( AccountEdit->Text.Length() != 0 && AccountEdit->Text.Length() != 7 )
	{
		ShowMessage( Scstrings_MAIN_APP_INFO_TFX_ACCOUNT_LEN_SHOULD_7 );
		return;
	}
	if( FOwnerForm != NULL )
	{
		bool IsOption = true;
		bool IsOffHour = false;

		if( TSComboBox->ItemIndex == 1 )
			IsOffHour = true;
		if( FMarket == mtFutures )
			IsOption = false;
		if( FOwnerForm->RequestCancelAll( IsOption, IsOffHour, PasswordEdit->Text, AccountEdit->Text ) == false )
			ShowMessage( Scstrings_MAIN_APP_INFO_PWD_ERR_INPUT_SC_PASSWORD );
	}
	PasswordEdit->Text = "";
	KSPasswordEdit->Text = "";
}
//---------------------------------------------------------------------------
void __fastcall TAppInfoForm::OnBtnClick(TObject *Sender)
{
	if( KSPasswordEdit->Text.Length() == 0 )
	{
		ShowMessage( Scstrings_MAIN_APP_INFO_INPUT_SC_PASSWORD );
		return;
	}
	if( FOwnerForm != NULL )
	{
		bool IsOption = true;
		bool IsOffHour = false;

		if( KSTSComboBox->ItemIndex == 1 )
			IsOffHour = true;
		if( FMarket == mtFutures )
			IsOption = false;
		if( ByFCMRadioButton->Checked == true )
		{
			if( FOwnerForm->RequestKillSwitch( IsOption, IsOffHour, true,
											  KSPasswordEdit->Text,
											  FCMComboBox->Text ) == false )
				ShowMessage( Scstrings_MAIN_APP_INFO_PWD_ERR_INPUT_SC_PASSWORD );
		}
		else
		{
			if( FOwnerForm->RequestKillSwitch( IsOption, IsOffHour, true,
											  KSPasswordEdit->Text,
											  FCM2ComboBox->Text,
											  SFCMComboBox->Text,
											  SIDComboBox->Text ) == false )
				ShowMessage( Scstrings_MAIN_APP_INFO_PWD_ERR_INPUT_SC_PASSWORD );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TAppInfoForm::OffBtnClick(TObject *Sender)
{
	if( KSPasswordEdit->Text.Length() == 0 )
	{
		ShowMessage( Scstrings_MAIN_APP_INFO_INPUT_SC_PASSWORD );
		return;
	}
	if( FOwnerForm != NULL )
	{
		bool IsOption = true;
		bool IsOffHour = false;

		if( KSTSComboBox->ItemIndex == 1 )
			IsOffHour = true;
		if( FMarket == mtFutures )
			IsOption = false;
		if( ByFCMRadioButton->Checked == true )
		{
			if( FOwnerForm->RequestKillSwitch( IsOption, IsOffHour, false,
											  KSPasswordEdit->Text,
											  FCMComboBox->Text ) == false )
				ShowMessage( Scstrings_MAIN_APP_INFO_PWD_ERR_INPUT_SC_PASSWORD );
		}
		else
		{
			if( FOwnerForm->RequestKillSwitch( IsOption, IsOffHour, false,
											  KSPasswordEdit->Text,
											  FCM2ComboBox->Text,
											  SFCMComboBox->Text,
											  SIDComboBox->Text ) == false )
				ShowMessage( Scstrings_MAIN_APP_INFO_PWD_ERR_INPUT_SC_PASSWORD );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TAppInfoForm::UpdateFCMList( void)
{
	TLineInfo* Info;
	TListBox* Links = FOwnerForm->LineListBox;
	bool IsOffhour = false;

	FCMComboBox->Items->Clear();
	FCM2ComboBox->Items->Clear();
	SFCMComboBox->Items->Clear();
	SIDComboBox->Items->Clear();
	if( KSTSComboBox->ItemIndex == 1 )
		IsOffhour = true;
	for( int i = 0; i < Links->Count; i++ )
	{
		if( (Info = (TLineInfo*)Links->Items->Objects[ i ] ) != NULL )
		{
			if(  FMarket ==  Info->GetSystem() && Info->GetOffHour() == IsOffhour )
			{
				String FCM( Info->GetBrokerID().c_str() );
				String SID( Info->GetPVC() );

				if( FCMComboBox->Items->IndexOf( FCM ) == -1 )
				{
					FCMComboBox->Items->Add( FCM );
					FCM2ComboBox->Items->Add( FCM );
					SFCMComboBox->Items->Add( FCM );
				}
				if( SIDComboBox->Items->IndexOf( SID ) == -1 )
					SIDComboBox->Items->Add( SID );
			}
		}
	}
	SIDComboBox->Items->Add( "All" );
	FCMComboBox->ItemIndex = 0;
	FCM2ComboBox->ItemIndex = 0;
	SFCMComboBox->ItemIndex = 0;
	SIDComboBox->ItemIndex = 0;

	if( FMarket == mtFutures )
		FOwnerForm->RequestKillSwitchList( false, IsOffhour );
	else
		FOwnerForm->RequestKillSwitchList( true, IsOffhour );
}
//---------------------------------------------------------------------------
void __fastcall  TAppInfoForm::UpdateKillSwitch( TSimTFXForm* OF, MTree* Tree )
{
	UFC::AnsiString  Name,Value;
	int iCount;

	FOwnerForm = OF;
	if( Tree->get( "COUNT", iCount ) && iCount > 0 )
	{
		KSListView->Clear();
		for( int i =0; i < iCount; i++ )
		{
			Name.Printf( "Item%d", i+1 );
			if( Tree->get( Name, Value ))
			{
				UFC::PStringList Strs;
				TListItem*       Item;

				Strs.SetStrings( Value, "^\n" );
				if( Strs.ItemCount() >= 2)
				{
					Item = KSListView->Items->Add();
					Item->Caption = Strs[0].c_str();
					Item->SubItems->Add( Strs[1].c_str());
					if( Strs.ItemCount() >= 4 )
					{
						Item->SubItems->Add( Strs[2].c_str());
						Item->SubItems->Add( Strs[3].c_str());
					}
					else
					{
						Item->SubItems->Add( "---");
						Item->SubItems->Add( "---");
					}
				}
			}
		}
		KSListView->Visible =  true;
	}
	else
		KSListView->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TAppInfoForm::KSTSComboBoxChange(TObject *Sender)
{
	UpdateFCMList();
}
//---------------------------------------------------------------------------

