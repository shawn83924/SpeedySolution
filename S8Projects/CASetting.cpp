//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CASetting.h"
#include "FMTConfig.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma link "GraphButton"
#pragma resource "*.dfm"
TCASettingForm *CASettingForm;
//---------------------------------------------------------------------------
__fastcall TCASettingForm::TCASettingForm(TComponent* Owner)
	: TForm(Owner)
	,FCloseMainForm(false)
{
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
void __fastcall TCASettingForm::BrowseButtonClick(TObject *Sender)
{
	String Dir =  ExtractFilePath( CAFileEdit->Text );
	String File = ExtractFileName( CAFileEdit->Text );

	if( FileExists( CAFileEdit->Text ) == true )
	{
		DirectoryListBox->Directory = Dir;
		FileListBox->FileName = File;
	}
	else
	{
		if( DirectoryExists( Dir ) == true )
			DirectoryListBox->Directory = Dir;
		else
			DirectoryListBox->Directory = GetCurrentDir();
		FileListBox->FileName = "";
	}
	PageControl->ActivePage = FileTabSheet;
}
//---------------------------------------------------------------------------
void __fastcall TCASettingForm::CheckCAButtonClick(TObject *Sender)
{
   String Result;

   MainForm->FCAChecker->PFXFileName = CAFileEdit->Text;
   MainForm->FCAChecker->PFXPassword = CAPasswordEdit->Text;
   if( MainForm->FCAChecker->InitialCheckerAndTest( Result ) == true )
	   SNLabel->Caption = L"憑證序號:" + MainForm->FCAChecker->CASerial;
   ResultLabel->Caption = L"檢查結果:" + Result;
}
//---------------------------------------------------------------------------
void __fastcall TCASettingForm::QuiteButtonClick(TObject *Sender)
{
   MainForm->FCAChecker->PFXFileName = CAFileEdit->Text;
   MainForm->FCAChecker->PFXPassword = CAPasswordEdit->Text;
   this->ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TCASettingForm::CancelButtonClick(TObject *Sender)
{
	PageControl->ActivePage = CATabSheet;
}
//---------------------------------------------------------------------------

void __fastcall TCASettingForm::OKButtonClick(TObject *Sender)
{
	PageControl->ActivePage = CATabSheet;

	CAFileEdit->Text = FileListBox->FileName;
}
//---------------------------------------------------------------------------

void __fastcall TCASettingForm::FormShow(TObject *Sender)
{
	PageControl->ActivePage = CATabSheet;
}
//---------------------------------------------------------------------------
void __fastcall TCASettingForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	if(MainForm->FCAChecker->CheckResult == true)
	{
		CanClose = true;
		return;
	}

	if( TUnifyDlgs::AskYesNoDialog( Mdcomponentstrings_MD_SpeedyUnify_AppName, L"確定要登出並離開程式?") == false)
	{
		CanClose = false;
        return;
	}

	CanClose = true;
    FCloseMainForm = true;
}
//---------------------------------------------------------------------------

