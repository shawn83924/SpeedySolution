//---------------------------------------------------------------------------

#ifndef NewOCODlgH
#define NewOCODlgH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "RoundFormEx.h"
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include "GraphButton.h"
#include "DepthTrade.h"
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
//---------------------------------------------------------------------------
class TNewOCODlgForm : public TForm
{
__published:	// IDE-managed Components
	TRoundFormEx *RoundFormEx;
	TPanel *BottomPanel;
	TLabel *TriggerPriceLabel;
	TEdit *TriggerPriceEdit;
	TGraphButton *BuyGraphButton;
	TGraphButton *SellGraphButton;
	TComboBox *OrderTypeComboBox;
	TEdit *LimitOrderPriceEdit;
	TLabel *MarketOrderLabelStart;
	TEdit *MarketOrderEdit;
	TUpDown *MarketOrderUpDown;
	TLabel *MarketOrderLabelEnd;
	TLabel *LotLabel;
	TEdit *LotEdit;
	TUpDown *LotUpDown;
	TButton *CancelButton;
	TButton *OKButton;
	TPanel *ContentPanel;
	TPanel *TopPanel;
	TGraphButton *TakeProfitButton;
	TGraphButton *StopLossButton;
	TUpDown *LimitOrderPriceUpDown;
	TUpDown *TriggerPriceUpDown;
	void __fastcall BuyGraphButtonClick(TObject *Sender);
	void __fastcall SellGraphButtonClick(TObject *Sender);
	void __fastcall OrderTypeComboBoxChange(TObject *Sender);
	void __fastcall OnPageChange(TObject *Sender);
	void __fastcall TriggerPriceUpDownChangingEx(TObject *Sender, bool &AllowChange,
          int NewValue, TUpDownDirection Direction);
	void __fastcall LimitOrderPriceUpDownChangingEx(TObject *Sender, bool &AllowChange,
          int NewValue, TUpDownDirection Direction);
	void __fastcall LotUpDownChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);


public:		// User declarations
	enum class DlgType
	{
		NewOCO,
		EditOCO
	};
	enum class TriggerType
	{
		TakeProfit,
		StopLoss
	};
	enum class BuySellAction
	{
		None,
		Buy,
		Sell
	};
	__fastcall TNewOCODlgForm(TComponent* Owner, DlgType Type);
    __fastcall ~TNewOCODlgForm( void );
	DlgType __fastcall GetDlgType( void ){ return FDlgType; };
private:	// User declarations
	DlgType FDlgType;
	TOrderBookList *FOrderbookList;
	double FFillPrice;
	class PageContent
	{
		public:
			String TriggerPriceLabel;
			int TriggerPriceTick;
			TriggerType Type;
			BuySellAction ActionType;
			String MarketOrderBuy;
			String MarketOrderSell;
			int OrderType;
			int LimitOrderPriceStep;
			int MarketOrderStep;
			int LotEdit;
	};
	PageContent *FTakeProfitContent;
	PageContent *FStopLossContent;
	PageContent *FCurrentContent;
	void __fastcall SetOCOContent(	PageContent *content,
									const String& triggerPriceLabel,
									TriggerType type,
									int triggerPriceTick = 0,
									BuySellAction actionType = BuySellAction::None,
									int orderType = 0,
									int limitOrderPriceStep = 0,
									int marketOrderStep = 1,
									int lot = 1);
	void __fastcall DisableTabButtons( void );
	void __fastcall SetContentValue( TGraphButton *activeButton );
    void __fastcall EnablePriceContent( BuySellAction actionType );
};
//---------------------------------------------------------------------------
extern PACKAGE TNewOCODlgForm *NewOCODlgForm;
//---------------------------------------------------------------------------
#endif
