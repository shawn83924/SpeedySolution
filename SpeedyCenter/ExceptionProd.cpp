//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ExceptionProd.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TExceptionProdForm *ExceptionProdForm;
//---------------------------------------------------------------------------
__fastcall TExceptionProdForm::TExceptionProdForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TExceptionProdForm::Init( TStringList* Exchanges, TStringList* Channels )
{
	ExComboBox->Clear();
	ExComboBox->Items->AddStrings( Exchanges );
	ExComboBox->ItemIndex = 0;
	MarketComboBox->ItemIndex = 0;
	ProdIDEdit->Text = "";
	ChComboBox->Clear();
	ChComboBox->Items->AddStrings( Channels );
	ChComboBox->ItemIndex = 0;
	ExComboBox->Enabled = true;
	MarketComboBox->Enabled = true;
	ProdIDEdit->Enabled = true;
	ChComboBox->Enabled = true;
	ApplyAllChCheckBox->Visible = false;
	ApplyAllChCheckBox->Checked = false;
	Caption = L"新增例外商品";
}
//---------------------------------------------------------------------------
void TExceptionProdForm::Init( TStringList* Exchanges, TStringList* Channels, TStrings* Settings )
{
	Init( Exchanges, Channels );
	ExComboBox->ItemIndex     = ExComboBox->Items->IndexOf( Settings->Strings[0]);
	MarketComboBox->ItemIndex = MarketComboBox->Items->IndexOf( Settings->Strings[1] );
	ProdIDEdit->Text          = Settings->Strings[2];
	ChComboBox->ItemIndex     = ChComboBox->Items->IndexOf( Settings->Strings[3] );
	ExComboBox->Enabled = false;
	MarketComboBox->Enabled = false;
	ProdIDEdit->Enabled = false;
	ChComboBox->Enabled = true;
	ApplyAllChCheckBox->Visible = true;
	Caption = L"修改例外商品";
}
//---------------------------------------------------------------------------

