//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TrainingDlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "GraphButton"
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TTrainingDlgForm *TrainingDlgForm;
//---------------------------------------------------------------------------
__fastcall TTrainingDlgForm::TTrainingDlgForm(TComponent* Owner)
	: TForm(Owner)
{
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
void __fastcall TTrainingDlgForm::CAButtonClick(TObject *Sender)
{
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TTrainingDlgForm::MessageDlg( const String& caption, const String& caption2 )
{
	TTrainingDlgForm* MsgDlg = new TTrainingDlgForm( NULL );

	MsgDlg->Label1->Caption = caption;
	MsgDlg->Label2->Caption = caption2;
	MsgDlg->ShowModal();
	delete MsgDlg;
}
//---------------------------------------------------------------------------
void __fastcall TTrainingDlgForm::FormCreate(TObject *Sender)
{
   Application->NormalizeTopMosts();
   SetWindowPos( Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
}
//---------------------------------------------------------------------------

