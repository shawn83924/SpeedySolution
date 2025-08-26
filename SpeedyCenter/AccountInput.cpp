//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "AccountInput.h"
#include "SCStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Publisher"
#pragma resource "*.dfm"
TAccountInputForm *AccountInputForm;
//---------------------------------------------------------------------------
__fastcall TAccountInputForm::TAccountInputForm(TComponent* Owner, bool InputFlag)
	: TForm(Owner)
{
	if( InputFlag == false )
	{
		Label2->Visible = false;
		AccountFlagEdit->Visible = false;
		AccountFlagEdit->Text = L"P";
		AccountEdit->MaxLength = 10;
	}
	else
		AccountEdit->MaxLength = 7;
}
//---------------------------------------------------------------------------
void __fastcall TAccountInputForm::BitBtn1Click(TObject *Sender)
{
/*	if( AccountEdit->Text.Length() != 7 )
	{
		ShowMessage( Scstrings_MAIN_ACCOUNT_INPUT_LENGTH_SHOULD_7 );
		return;
	}*/
	if( AccountFlagEdit->Text.Length() != 1 )
    {
		ShowMessage( Scstrings_MAIN_ACCOUNT_INPUT_FLAG_LEN_SHOULD_1 );
        return;
    }
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
