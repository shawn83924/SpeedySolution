//---------------------------------------------------------------------------
#include <vcl.h>
#include "main.h"
#pragma hdrstop
#include "FMTConfig.h"
#include "AskForUpdateForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "GraphButton"
#pragma link "GraphPanel"
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TAskUpdateForm *AskUpdateForm;
//---------------------------------------------------------------------------
__fastcall TAskUpdateForm::TAskUpdateForm( TComponent* Owner )
	: TForm(Owner)
{
	String Msg;
	int AppVer = g_Config.GetUpdateVersion();


	FForce     = g_Config.NeedsForceUpdate();
	FReInstall = g_Config.NeedsReInstall();
	Msg.printf( L"版本 %d.%d.%d.%d 有以下修改:", AppVer/1000000,
												(AppVer%1000000)/10000,
												(AppVer%10000)/100,
												 AppVer%100 );///"版本 %d.%d.%d.%d 有以下修改:"
	VersionLabel->Caption = Msg;
	RoundFormEx->Lockbox = false;
	g_Config.LoadUpdateDescription( NoteMemo->Lines );
	if( FReInstall == true )
	{
		int BL = (Width - OKButton->Width )/2;
		OKButton->Left = BL;
		CancelButton->Visible = false;
		UpdateLabel->Caption = L"必須重新安裝才能使用.";
	}
	else
	{
		if( FForce == true )
		{
			int BL = (Width - OKButton->Width )/2;
			OKButton->Left = BL;
			CancelButton->Visible = false;
			UpdateLabel->Caption = L"更新版本後才能使用.";
		}
		else
		{
			int BL = (Width - OKButton->Width -CancelButton->Width -20)/2;

			OKButton->Left = BL;
			CancelButton->Left = BL + OKButton->Width + 20;
			UpdateLabel->Caption = L"是否要更新版本?";
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TAskUpdateForm::OKButtonClick(TObject *Sender)
{
	if( FReInstall == true )
		ModalResult = mrClose;
	else
		ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TAskUpdateForm::CancelButtonClick(TObject *Sender)
{
	ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
TModalResult __fastcall TAskUpdateForm::AskForUpdate()
{
	TModalResult Result;

	AskUpdateForm = new  TAskUpdateForm( NULL );
	AskUpdateForm->ShowModal();
	if( AskUpdateForm->FForce == true )
	{
		if( AskUpdateForm->FReInstall == true )
			Result = mrClose;
		else
			Result = mrOk;
	}
	else
		Result = AskUpdateForm->ModalResult;
	delete AskUpdateForm;
	return Result;
}
//---------------------------------------------------------------------------
