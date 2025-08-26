//---------------------------------------------------------------------------

#ifndef OrderConfirmH
#define OrderConfirmH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "GraphButton.h"
#include "RoundFormEx.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "MarketDataStore.h"
#include "OrderStore.h"
//---------------------------------------------------------------------------
class TOrdConfirmForm : public TForm
{
__published:	// IDE-managed Components
	TRoundFormEx *RoundFormEx;
	TPanel *pnlToolbar;
	TUpDown *LotsUpDown;
	TEdit *LotsPerOrderEdit;
	TGraphButton *NormalButton;
	TGraphButton *LoanButton;
	TGraphButton *LoanShareButton;
	TGraphButton *DayTradeButton;
	TLabel *AvgPxTextLabel;
	TLabel *Label1;
	TComboBox *PxComboBox;
	TButton *OKButton;
	TButton *CancelButton;
	TGraphButton *BuyGraphButton;
	TGraphButton *SellGraphButton;
	void __fastcall NormalButtonClick(TObject *Sender);
	void __fastcall DayTradeButtonClick(TObject *Sender);
	void __fastcall LotsUpDownChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction);
	void __fastcall PxComboBoxChange(TObject *Sender);
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall BuyGraphButtonClick(TObject *Sender);
	void __fastcall SellGraphButtonClick(TObject *Sender);
private:	// User declarations
	String FLastEx;
	String FLastSym;
	String FUnit;
	String FSideStr;
	TTickList* FTickList;
	nsOrderMessageDefine::SideEnum FSide;
	void __fastcall BuildBtnText( int Qty );
public:		// User declarations
	__fastcall TOrdConfirmForm(TComponent* Owner);
	void __fastcall Init( const String& Ex, const String& Sym, nsOrderMessageDefine::SideEnum side, double Px );
public:
	double                         __fastcall GetPrice( void );
	int                            __fastcall GetQty( void ) { return  LotsUpDown->Position; }
	nsOrderMessageDefine::SideEnum __fastcall GetSide( void ){ return  FSide; }
	PositionEffectEnum             __fastcall GetPositionEffect( void );
	char                           __fastcall GetTWSEOrderType( SideEnum side );
};
//---------------------------------------------------------------------------
extern PACKAGE TOrdConfirmForm *OrdConfirmForm;
//---------------------------------------------------------------------------
#endif
