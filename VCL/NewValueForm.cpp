//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "NewValueForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAddValueForm *AddValueForm;
//---------------------------------------------------------------------------
__fastcall TAddValueForm::TAddValueForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TAddValueForm::OkBtnClick(TObject *Sender)
{
	if( NameEdit->Text.Length() > 0 &&
		ValueEdit->Text.Length() > 0 )
		this->ModalResult = mrOk;
}
//---------------------------------------------------------------------------
