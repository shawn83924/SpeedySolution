//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CancelWorkingTimeForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TCancelWorkingForm *CancelWorkingForm;
//---------------------------------------------------------------------------
__fastcall TCancelWorkingForm::TCancelWorkingForm(TComponent* Owner)
	: TForm(Owner)
{
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
void __fastcall TCancelWorkingForm::SetEnable( bool OnOff )
{
	FEnableTimer = OnOff;
}
//---------------------------------------------------------------------------
bool __fastcall TCancelWorkingForm::GetEnable( void )
{
	return FEnableTimer;
}
//---------------------------------------------------------------------------
void __fastcall TCancelWorkingForm::SetTime( int hhmmss )
{
	FCancelTime = hhmmss;
	HUpDown->Position       = FCancelTime/10000;
	MinutesUpDown->Position = (FCancelTime%10000)/100;
	SecUpDown->Position     = FCancelTime%100;
}
//---------------------------------------------------------------------------
int __fastcall TCancelWorkingForm::GetTime( void )
{
	return FCancelTime;
}
//---------------------------------------------------------------------------
const String&  __fastcall TCancelWorkingForm::GetTimeString( void )
{
	FTimeString.printf( L"%02d:%02d:%02d",HUpDown->Position,MinutesUpDown->Position,SecUpDown->Position );
	return FTimeString;
}
//---------------------------------------------------------------------------
void __fastcall TCancelWorkingForm::OKButtonClick(TObject *Sender)
{
	FEnableTimer = true;
	FCancelTime = HUpDown->Position*10000 + MinutesUpDown->Position*100 + SecUpDown->Position;
}
//---------------------------------------------------------------------------
void __fastcall TCancelWorkingForm::CancelButtonClick(TObject *Sender)
{
	FEnableTimer = false;
	FCancelTime = HUpDown->Position*10000 + MinutesUpDown->Position*100 + SecUpDown->Position;
}
//---------------------------------------------------------------------------
void __fastcall TCancelWorkingForm::FormCreate(TObject *Sender)
{
   Application->NormalizeTopMosts();
   SetWindowPos( Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
}
//---------------------------------------------------------------------------
