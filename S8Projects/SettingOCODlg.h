//---------------------------------------------------------------------------

#ifndef SettingOCODlgH
#define SettingOCODlgH
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
class TSettingOCODlgForm : public TForm
{
__published:	// IDE-managed Components
	TRoundFormEx *RoundFormEx;
	TPanel *BottomPanel;
	TComboBox *OrderTypeComboBox;
	TEdit *TickEdit;
	TButton *CancelButton;
	TButton *OKButton;
	TPanel *ContentPanel;
	TLabel *PriceTitleLabel;
	TLabel *PriceEndLabel;
	TUpDown *TickUpDown;
	void __fastcall OrderTypeComboBoxChange(TObject *Sender);
	void __fastcall TickUpDownClick(TObject *Sender, TUDBtnType Button);
public:		// User declarations
	__fastcall TSettingOCODlgForm(	TComponent* Owner,
									int orderType,
									int limitOrderTick);
	__fastcall ~TSettingOCODlgForm( void );
	void __fastcall SetOCOOrderType( int type );
	int __fastcall GetOrderType( void ){ return FOrderType;}
	int __fastcall GetLimitOrderTick( void ){ return FLimitOrderTick; }
private:	// User declarations
	int FOrderType;
	int FLimitOrderTick;
	const String FLimitOrderTitle;
	const String FMarketOrderTitle;
	const String FRangeMarketOrderTitle;
};
//---------------------------------------------------------------------------
extern PACKAGE TSettingOCODlgForm *SettingOCODlgForm;
//---------------------------------------------------------------------------
#endif
