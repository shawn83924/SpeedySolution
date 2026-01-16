//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "NewOCODlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma link "GraphButton"
#pragma resource "*.dfm"
TNewOCODlgForm *NewOCODlgForm;
//---------------------------------------------------------------------------
__fastcall TNewOCODlgForm::TNewOCODlgForm(TComponent* Owner, DlgType Type)
	: TForm(Owner)
{
	FOrderbookList = dynamic_cast<TDepthForm*>(Owner)->OrderBookList;
	FFillPrice = FOrderbookList->FillPrice;
	FTakeProfitContent = new PageContent();
	FStopLossContent = new PageContent();

	if(Type == DlgType::NewOCO)
	{
		SetOCOContent( FTakeProfitContent	, L"Θユ基单:", TriggerType::TakeProfit);
		SetOCOContent( FStopLossContent	 	, L"Θユ基单:", TriggerType::StopLoss);
	}
	else
	{
		//SetOCOContent();
	}

	TakeProfitButton->Tag = reinterpret_cast<intptr_t>(FTakeProfitContent);
	StopLossButton->Tag = reinterpret_cast<intptr_t>(FStopLossContent);
	OnPageChange(TakeProfitButton);
	OrderTypeComboBoxChange(NULL);
}
//---------------------------------------------------------------------------
__fastcall TNewOCODlgForm::~TNewOCODlgForm( void )
{
	delete FTakeProfitContent;
	FTakeProfitContent = NULL;
	delete FStopLossContent;
	FStopLossContent = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TNewOCODlgForm::SetOCOContent(	PageContent *content,
												const String& triggerPriceLabel,
												TriggerType type,
												int triggerPriceTick,
												BuySellAction actionType,
												int orderType,
												int limitOrderPriceStep,
												int marketOrderStep,
												int lot )
{
	content->TriggerPriceLabel 		= triggerPriceLabel;
	content->Type					= type;
	content->TriggerPriceTick 		= triggerPriceTick;
	content->ActionType				= actionType;
	content->MarketOrderBuy         = L"蔼Θユ基:";
	content->MarketOrderSell        = L"Θユ基:";
	content->OrderType 				= orderType;
	content->LimitOrderPriceStep 	= limitOrderPriceStep;
	content->MarketOrderStep		= marketOrderStep;
	content->LotEdit  				= lot;
}
//---------------------------------------------------------------------------
void __fastcall TNewOCODlgForm::BuyGraphButtonClick(TObject *Sender)
{
	if(BuyGraphButton->Selected == true)
		return;

	BuyGraphButton->Selected = true;
	SellGraphButton->Selected = false;
	MarketOrderLabelStart->Caption = FCurrentContent->MarketOrderBuy;
	EnablePriceContent(BuySellAction::Buy);
	FCurrentContent->ActionType = BuySellAction::Buy;
}
//---------------------------------------------------------------------------

void __fastcall TNewOCODlgForm::SellGraphButtonClick(TObject *Sender)
{
	if(SellGraphButton->Selected == true)
		return;

	BuyGraphButton->Selected = false;
	SellGraphButton->Selected = true;
	MarketOrderLabelStart->Caption = FCurrentContent->MarketOrderSell;
	EnablePriceContent(BuySellAction::Sell);
	FCurrentContent->ActionType = BuySellAction::Sell;
}
//---------------------------------------------------------------------------
void __fastcall TNewOCODlgForm::OrderTypeComboBoxChange(TObject *Sender)
{
	bool isLimitOrder = (OrderTypeComboBox->ItemIndex == 0);

	LimitOrderPriceEdit->Visible   = isLimitOrder;
	LimitOrderPriceUpDown->Visible = isLimitOrder;
	MarketOrderLabelStart->Visible = !isLimitOrder;
	MarketOrderLabelEnd->Visible   = !isLimitOrder;
	MarketOrderEdit->Visible       = !isLimitOrder;
	MarketOrderUpDown->Visible     = !isLimitOrder;

	FCurrentContent->OrderType = OrderTypeComboBox->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TNewOCODlgForm::OnPageChange(TObject *Sender)
{
	TGraphButton *activeButton = dynamic_cast<TGraphButton*>(Sender);
	DisableTabButtons();
    activeButton->Selected = true;
	SetContentValue(activeButton);

}
//---------------------------------------------------------------------------
void __fastcall TNewOCODlgForm::DisableTabButtons( void )
{
	for (int i = 0; i < TopPanel->ControlCount; i++)
	{
		TGraphButton *btn = dynamic_cast<TGraphButton*>(TopPanel->Controls[i]);
		btn->Selected = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TNewOCODlgForm::SetContentValue( TGraphButton *activeButton )
{
	FCurrentContent = reinterpret_cast<PageContent*>(activeButton->Tag);
	TriggerPriceLabel->Caption = FCurrentContent->TriggerPriceLabel;

	double triggerPrice = FOrderbookList->GetBetterPrice(FFillPrice, FCurrentContent->TriggerPriceTick);
	TriggerPriceEdit->Text = FloatToStr(triggerPrice);
	BuyGraphButton->Selected = FCurrentContent->ActionType == BuySellAction::Buy? true:false;
	SellGraphButton->Selected = FCurrentContent->ActionType == BuySellAction::Sell? true:false;
	OrderTypeComboBox->ItemIndex = FCurrentContent->OrderType;
	OrderTypeComboBoxChange(NULL);

	if(FCurrentContent->ActionType == BuySellAction::Buy)
		MarketOrderLabelStart->Caption = FCurrentContent->MarketOrderBuy;
	else
		MarketOrderLabelStart->Caption = FCurrentContent->MarketOrderSell;
		
	EnablePriceContent(FCurrentContent->ActionType);

	double limitOrderPrice = FOrderbookList->GetTickPrice(FCurrentContent->LimitOrderPriceStep);
	LimitOrderPriceEdit->Text = FloatToStr(limitOrderPrice);
	MarketOrderEdit->Text = IntToStr(FCurrentContent->MarketOrderStep);
	LotEdit->Text = IntToStr(FCurrentContent->LotEdit);
}
//---------------------------------------------------------------------------
void __fastcall TNewOCODlgForm::EnablePriceContent( BuySellAction actionType )
{
	bool enable = actionType != BuySellAction::None;

	LimitOrderPriceEdit->Enabled 	= enable;
	LimitOrderPriceUpDown->Enabled 	= enable;
	MarketOrderEdit->Enabled 		= enable;
	MarketOrderLabelStart->Enabled 	= enable;
	MarketOrderLabelEnd->Enabled 	= enable;
	MarketOrderUpDown->Enabled 		= enable;
	OrderTypeComboBox->Enabled 		= enable;
	LotEdit->Enabled 				= enable;
	LotLabel->Enabled 				= enable;
	LotUpDown->Enabled 				= enable;
}
//---------------------------------------------------------------------------
void __fastcall TNewOCODlgForm::TriggerPriceUpDownChangingEx(TObject *Sender, bool &AllowChange,
		  int NewValue, TUpDownDirection Direction)
{
	int tick = FCurrentContent->TriggerPriceTick;
	if(Direction == TUpDownDirection::updUp)
		tick+=1;
	else if(Direction == TUpDownDirection::updDown)
		tick-=1;

	if(tick < 0 && FCurrentContent->Type == TriggerType::TakeProfit)
		return;
	if(tick > 0 && FCurrentContent->Type == TriggerType::StopLoss)
		return;
	
	double price = FOrderbookList->GetBetterPrice(FFillPrice, tick);
	String priceStr = FloatToStr(price);
	if(TriggerPriceEdit->Text == priceStr)
		return;

	TriggerPriceEdit->Text = priceStr;
	FCurrentContent->TriggerPriceTick = tick;
}
//---------------------------------------------------------------------------

void __fastcall TNewOCODlgForm::LimitOrderPriceUpDownChangingEx(TObject *Sender, bool &AllowChange,
          int NewValue, TUpDownDirection Direction)
{
	int step = FCurrentContent->LimitOrderPriceStep;
    if(Direction == TUpDownDirection::updUp)
		step-=1;
	else if(Direction == TUpDownDirection::updDown)
		step+=1;

	double price = FOrderbookList->GetTickPrice(step);
	String priceStr = FloatToStr(price);
	if(LimitOrderPriceEdit->Text == priceStr)
		return;

	LimitOrderPriceEdit->Text = priceStr;
	FCurrentContent->LimitOrderPriceStep = step;
}
//---------------------------------------------------------------------------

void __fastcall TNewOCODlgForm::LotUpDownChangingEx(TObject *Sender, bool &AllowChange,
          int NewValue, TUpDownDirection Direction)
{
	int step = FCurrentContent->LotEdit;
	if(Direction == TUpDownDirection::updUp)
		step-=1;
	else if(Direction == TUpDownDirection::updDown)
		step+=1;

	if(step<=1)
		return;

	LotEdit->Text = IntToStr(step);
	FCurrentContent->LotEdit = step;
}
//---------------------------------------------------------------------------
