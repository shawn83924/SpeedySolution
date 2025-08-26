//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "StopSetting.h"
#include "OrderBookList.h"
#include "CandleStickChart.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
int TStopSettingForm::TickSteps = 50;
//---------------------------------------------------------------------------
__fastcall TStopSettingForm::TStopSettingForm(TComponent* Owner )
	: TForm(Owner)
{
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
void __fastcall TStopSettingForm::ApplySetting( TOrderBookList* Depth )
{
	if( Depth->BuyStopOrderType == nsOrderMessageDefine::otMarket)
		BuyRadioGroup->ItemIndex = 0;
	else if( Depth->BuyStopOrderType == nsOrderMessageDefine::otLimit)
		BuyRadioGroup->ItemIndex = 1;
	else
		BuyRadioGroup->ItemIndex = -1;

	if((Depth->BuyStopTick >= -1 * TickSteps ) && (Depth->BuyStopTick <= TickSteps ))
		BuyStopTickComboBox->ItemIndex = TickSteps - Depth->BuyStopTick;
	else
		BuyStopTickComboBox->ItemIndex = TickSteps - 5;

	if( Depth->SellStopOrderType == nsOrderMessageDefine::otMarket )
		SellRadioGroup->ItemIndex = 0;
	else if( Depth->SellStopOrderType == nsOrderMessageDefine::otLimit)
		SellRadioGroup->ItemIndex = 1;
	else
		SellRadioGroup->ItemIndex = -1;
	if ((Depth->SellStopTick >= -1*TickSteps ) && (Depth->SellStopTick <= TickSteps))
		SellStopTickComboBox->ItemIndex = TickSteps - Depth->SellStopTick;
	else
		SellStopTickComboBox->ItemIndex = TickSteps + 5;
}
//---------------------------------------------------------------------------
void __fastcall TStopSettingForm::ApplySetting( TCandleStickChart* Depth )
{
	if( Depth->BuyStopOrderType == nsOrderMessageDefine::otMarket)
		BuyRadioGroup->ItemIndex = 0;
	else if( Depth->BuyStopOrderType == nsOrderMessageDefine::otLimit)
		BuyRadioGroup->ItemIndex = 1;
	else
		BuyRadioGroup->ItemIndex = -1;

	if((Depth->BuyStopTick >= -1 * TickSteps ) && (Depth->BuyStopTick <= TickSteps ))
		BuyStopTickComboBox->ItemIndex = TickSteps - Depth->BuyStopTick;
	else
		BuyStopTickComboBox->ItemIndex = TickSteps - 5;

	if( Depth->SellStopOrderType == nsOrderMessageDefine::otMarket )
		SellRadioGroup->ItemIndex = 0;
	else if( Depth->SellStopOrderType == nsOrderMessageDefine::otLimit)
		SellRadioGroup->ItemIndex = 1;
	else
		SellRadioGroup->ItemIndex = -1;
	if ((Depth->SellStopTick >= -1*TickSteps ) && (Depth->SellStopTick <= TickSteps))
		SellStopTickComboBox->ItemIndex = TickSteps - Depth->SellStopTick;
	else
		SellStopTickComboBox->ItemIndex = TickSteps + 5;
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::OrderTypeEnum __fastcall TStopSettingForm::GetBuyOrderType( void )
{
	if( BuyRadioGroup->ItemIndex == 0 )
		return nsOrderMessageDefine::otMarket;
	else
		return nsOrderMessageDefine::otLimit;
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::OrderTypeEnum __fastcall TStopSettingForm::GetSellOrderType( void )
{
	if( SellRadioGroup->ItemIndex == 0 )
		return nsOrderMessageDefine::otMarket;
	else
		return nsOrderMessageDefine::otLimit;
}
//---------------------------------------------------------------------------
int __fastcall TStopSettingForm::GetBuyTick( void )
{
	return TickSteps - BuyStopTickComboBox->ItemIndex;
}
//---------------------------------------------------------------------------
int __fastcall TStopSettingForm::GetSellTick( void )
{
	return TickSteps - SellStopTickComboBox->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TStopSettingForm::OKButtonClick(TObject *Sender)
{
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TStopSettingForm::CancelButtonClick(TObject *Sender)
{
	ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TStopSettingForm::FormCreate(TObject *Sender)
{
   Application->NormalizeTopMosts();
   SetWindowPos( Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
}
//---------------------------------------------------------------------------

