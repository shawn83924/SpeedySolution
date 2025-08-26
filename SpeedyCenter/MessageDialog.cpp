//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MessageDialog.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMessageForm *MessageForm;
//---------------------------------------------------------------------------
__fastcall TMessageForm::TMessageForm(TComponent* Owner)
	: TForm(Owner)
{
	Users = new TStringList();
}
//---------------------------------------------------------------------------
void __fastcall TMessageForm::OkBtnClick(TObject *Sender)
{
	Users->Clear();
	if( HeaderEdit->Text.Length() == 0 )
	{
		MessageDlg( L"請填入標題", mtWarning, TMsgDlgButtons() << mbOK , 0 );
		return;
	}
	if( TextEdit->Text.Length() == 0 )
	{
		MessageDlg( L"請填入訊息", mtWarning, TMsgDlgButtons() << mbOK , 0 );
		return;
	}
	if( UserComboBox->ItemIndex == 0 )
	{
		for( int i = 1; i< UserComboBox->Items->Count; i ++ )
			 Users->Add( UserComboBox->Items->Strings[i] );
	}
	else
		Users->Add( UserComboBox->Text );
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
