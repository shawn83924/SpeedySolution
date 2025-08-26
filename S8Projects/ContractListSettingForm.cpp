//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "ContractListSettingForm.h"
#include "config.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma link "GraphButton"
#pragma link "GraphPanel"
#pragma resource "*.dfm"
TContractListColorSettingForm *ContractListColorSettingForm;
//---------------------------------------------------------------------------
__fastcall TContractListColorSettingForm::TContractListColorSettingForm(TComponent* Owner)
	: TForm(Owner)
{
	GraphPanel->Glyph->LoadFromResourceName( g_Config.GetResourceInstance(), L"IDR_DARKBLUE_BK" );
}
//---------------------------------------------------------------------------
void __fastcall TContractListColorSettingForm::LoadColor( void )
{
	FTextColor    = (TColor)g_Config.GetIntegerProperty( "Setting","CLTextColor",    clWhite );
	FOddBKColor   = (TColor)g_Config.GetIntegerProperty( "Setting","CLOddRowColor",  clBlack );
	FEvenBKColor  = (TColor)g_Config.GetIntegerProperty( "Setting","CLEvenRowColor", 0x00763A0A );
	FRiseBKColor  = (TColor)g_Config.GetIntegerProperty( "Setting","CLRiseBKColor",  clRed );
	FFallBKColor  = (TColor)g_Config.GetIntegerProperty( "Setting","CLFallBKColor", clGreen );
}
//---------------------------------------------------------------------------
void __fastcall TContractListColorSettingForm::SaveColor( void )
{
	g_Config.SetIntegerProperty( "Setting","CLTextColor",    TextColorBox->Selected );
	g_Config.SetIntegerProperty( "Setting","CLOddRowColor",  OddBKColorBox->Selected );
	g_Config.SetIntegerProperty( "Setting","CLEvenRowColor", EvenBKColorBox->Selected );
	g_Config.SetIntegerProperty( "Setting","CLRiseBKColor",  RiseBKColorBox->Selected );
	g_Config.SetIntegerProperty( "Setting","CLFallBKColor",  FallBKColorBox->Selected );
}
//---------------------------------------------------------------------------
void __fastcall TContractListColorSettingForm::DefaultColor( void )
{
	FTextColor      = clWhite;
	FOddBKColor     = clBlack;
	FEvenBKColor    = (TColor)0x00763A0A;
	FRiseBKColor    = clRed;
	FFallBKColor    = clGreen;
}
//---------------------------------------------------------------------------
void __fastcall TContractListColorSettingForm::SetColor( void )
{
	TextColorBox->Selected   =  FTextColor;
	OddBKColorBox->Selected  =  FOddBKColor;
	EvenBKColorBox->Selected =  FEvenBKColor;
	RiseBKColorBox->Selected =  FRiseBKColor;
	FallBKColorBox->Selected =  FFallBKColor;
}
//---------------------------------------------------------------------------
void __fastcall TContractListColorSettingForm::FormShow(TObject *Sender)
{
	LoadColor( );
	SetColor();
}
//---------------------------------------------------------------------------
void __fastcall TContractListColorSettingForm::OKButtonClick(TObject *Sender)
{
	SaveColor( );
	ModalResult = mrOk;
}
//------------------------------------------------------------------------------
void __fastcall TContractListColorSettingForm::CancelButtonClick(TObject *Sender)
{
	ModalResult = mrCancel;
}
//------------------------------------------------------------------------------


