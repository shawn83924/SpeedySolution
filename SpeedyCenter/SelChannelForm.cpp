//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SelChannelForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TChannelForm *ChannelForm;
//---------------------------------------------------------------------------
__fastcall TChannelForm::TChannelForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TChannelForm::Init( const String& Ex, const String& Mkt,  const String& ch,TStringList* chs)
{
	ExLabel->Caption  = Ex;
	MktLabel->Caption = Mkt;
	ChComboBox->Clear();
	ChComboBox->Items->Add( "---" );
	ChComboBox->Items->AddStrings( chs );
	int index = ChComboBox->Items->IndexOf( ch );
	if( index == -1 )
		index = 0;
	ChComboBox->ItemIndex = index;
}
//---------------------------------------------------------------------------

