//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "StopOrderSetting.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
int TStopOrderSettingForm::TickSteps = 50;
//---------------------------------------------------------------------------
__fastcall TStopOrderSettingForm::TStopOrderSettingForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::OrderTypeEnum __fastcall TStopOrderSettingForm::GetBuyOrderType( void )
{
	if( BuyRadioGroup->ItemIndex == 0 )
		return nsOrderMessageDefine::otMarket;
	else
		return nsOrderMessageDefine::otLimit;
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::OrderTypeEnum __fastcall TStopOrderSettingForm::GetSellOrderType( void )
{
	if( SellRadioGroup->ItemIndex == 0 )
		return nsOrderMessageDefine::otMarket;
	else
		return nsOrderMessageDefine::otLimit;
}
//---------------------------------------------------------------------------
int __fastcall TStopOrderSettingForm::GetBuyTick( void )
{
	return TickSteps - ComboBox2->ItemIndex;
}
//---------------------------------------------------------------------------
int __fastcall TStopOrderSettingForm::GetSellTick( void )
{
    return TickSteps - ComboBox4->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TStopOrderSettingForm::Button1Click(TObject *Sender)
{
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TStopOrderSettingForm::Button2Click(TObject *Sender)
{
	ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

