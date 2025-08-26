//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SelAccount.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TDefAccForm *DefAccForm;
//---------------------------------------------------------------------------
__fastcall TDefAccForm::TDefAccForm(TComponent* Owner)
	: TForm(Owner)
{
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
void __fastcall TDefAccForm::FormCreate(TObject *Sender)
{
   Application->NormalizeTopMosts();
   SetWindowPos( Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
}
//---------------------------------------------------------------------------

