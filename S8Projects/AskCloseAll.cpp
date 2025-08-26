//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AskCloseAll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TAskCloseAllForm *AskCloseAllForm;
//---------------------------------------------------------------------------
__fastcall TAskCloseAllForm::TAskCloseAllForm(TComponent* Owner)
	: TForm(Owner)
{
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
bool __fastcall TAskCloseAllForm::IsMarket( void )
{
	if( MarketRadioButton->Checked == true )
		return true;
	return false;
}
//---------------------------------------------------------------------------
int __fastcall TAskCloseAllForm::BetterTick( void )
{
	if( CloseAllStepComboBox->ItemIndex >= 0 )
		return CloseAllStepComboBox->ItemIndex;
	return 0;
}
//---------------------------------------------------------------------------
int __fastcall TAskCloseAllForm::QtyPerOrder( void )
{
   return LotsUpDown->Position;
}
//---------------------------------------------------------------------------
void __fastcall TAskCloseAllForm::FormCreate(TObject *Sender)
{
   Application->NormalizeTopMosts();
   SetWindowPos( Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
}
//---------------------------------------------------------------------------
