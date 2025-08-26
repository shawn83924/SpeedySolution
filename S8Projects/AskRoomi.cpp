//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AskRoomi.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TAskRoomiForm *AskRoomiForm;
//---------------------------------------------------------------------------
__fastcall TAskRoomiForm::TAskRoomiForm(TComponent* Owner)
	: TForm(Owner)
{
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
void __fastcall TAskRoomiForm::FormCreate(TObject *Sender)
{
   Application->NormalizeTopMosts();
   SetWindowPos( Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
}
//---------------------------------------------------------------------------

