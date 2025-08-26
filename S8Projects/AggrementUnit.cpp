//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AggrementUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TAggrementForm *AggrementForm;
//---------------------------------------------------------------------------
__fastcall TAggrementForm::TAggrementForm(TComponent* Owner)
	: TForm(Owner)
{
	RoundFormEx->Lockbox = false;
	FCheckBoxs.Add(CheckBox1);
	FCheckBoxs.Add(CheckBox2);
	FCheckBoxs.Add(CheckBox3);
	FCheckBoxs.Add(CheckBox4);
	FCheckBoxs.Add(CheckBox5);
	FCheckBoxs.Add(CheckBox6);
	FCheckBoxs.Add(CheckBox7);
	FCheckBoxs.Add(CheckBox8);
	FCheckBoxs.Add(CheckBox9);
	FCheckBoxs.Add(CheckBox10);
}
//---------------------------------------------------------------------------
bool __fastcall TAggrementForm::AllCjecked( void )
{
	for( int i = 0 ;i < FCheckBoxs.ItemCount(); i ++ )
		 if( FCheckBoxs[i]->Checked == false )
			 return false;
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TAggrementForm::CheckBox1Click(TObject *Sender)
{
	OKButton->Enabled = AllCjecked( );
}
//---------------------------------------------------------------------------
void __fastcall TAggrementForm::FormCreate(TObject *Sender)
{
   Application->NormalizeTopMosts();
   SetWindowPos( Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
}
//---------------------------------------------------------------------------

