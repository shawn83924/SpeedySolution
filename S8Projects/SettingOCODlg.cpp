//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SettingOCODlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma link "GraphButton"
#pragma resource "*.dfm"
TSettingOCODlgForm *SettingOCODlgForm;
//---------------------------------------------------------------------------
__fastcall TSettingOCODlgForm::TSettingOCODlgForm(TComponent* Owner,
									int orderType,
									int limitOrderTick,
									int rangeMarketOrderTick)
	: TForm(Owner)
	,FLimitOrderTitle		(L"以指定價格間距:")
	,FMarketOrderTitle		(L"以市價送出委託")
	,FRangeMarketOrderTitle	(L"以指定價格範圍:")
{
	FOrderType 					= orderType;
	OrderTypeComboBox->ItemIndex= orderType;
	FLimitOrderTick 			= limitOrderTick;
	FRangeMarketOrderTick 		= rangeMarketOrderTick;
	OrderTypeComboBoxChange(NULL);
}
//---------------------------------------------------------------------------
__fastcall TSettingOCODlgForm::~TSettingOCODlgForm( void )
{
}
//---------------------------------------------------------------------------
void __fastcall TSettingOCODlgForm::OrderTypeComboBoxChange(TObject *Sender)
{
	int type = OrderTypeComboBox->ItemIndex;
	SetOCOOrderType(type);
}
//---------------------------------------------------------------------------
void __fastcall TSettingOCODlgForm::SetOCOOrderType( int type )
{
	if(OrderTypeComboBox->ItemIndex == 0)
	{
		PriceTitleLabel->Caption= FLimitOrderTitle;
		TickEdit->Visible 		= true;
		TickUpDown->Visible 	= true;
		TickUpDown->Position 	= FLimitOrderTick;
		PriceEndLabel->Visible 	= true;
	}
	else if (OrderTypeComboBox->ItemIndex == 1)
	{
		PriceTitleLabel->Caption= FMarketOrderTitle;
		TickEdit->Visible 		= false;
		TickUpDown->Visible 	= false;
		PriceEndLabel->Visible 	= false;
	}
	else
	{
		PriceTitleLabel->Caption= FRangeMarketOrderTitle;
		TickEdit->Visible 		= true;
		TickUpDown->Visible 	= true;
		TickUpDown->Position 	= FRangeMarketOrderTick;
		PriceEndLabel->Visible 	= true;
	}
	FOrderType = OrderTypeComboBox->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TSettingOCODlgForm::TickUpDownClick(TObject *Sender, TUDBtnType Button)
{
	if(FOrderType == 0)
		FLimitOrderTick 		= TickUpDown->Position;
	if(FOrderType == 2)
		FRangeMarketOrderTick 	= TickUpDown->Position;
}
//---------------------------------------------------------------------------
