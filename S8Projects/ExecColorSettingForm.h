//---------------------------------------------------------------------------

#ifndef ExecColorSettingFormH
#define ExecColorSettingFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "RoundFormEx.h"
#include <Buttons.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
class TExecSettingForm : public TForm
{
__published:	// IDE-managed Components
	TImage *Image;
	TDrawGrid *DrawGrid;
	TGroupBox *ColorGroupBox;
	TLabel *BKColorLabel;
	TLabel *FontColorLabel;
	TSpeedButton *DefaultColorButton;
	TColorBox *BKColorBox;
	TColorBox *FontColorBox;
	TRoundFormEx *RoundFormEx;
	TButton *OptNewOrderButton;
	TButton *Button1;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	void __fastcall DefaultColorButtonClick(TObject *Sender);
	void __fastcall DrawGridDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TGridDrawState State);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall DrawGridClick(TObject *Sender);
	void __fastcall BKColorBoxChange(TObject *Sender);
	void __fastcall OptNewOrderButtonClick(TObject *Sender);
private:	// User declarations
	TColor  FNewBuyColor;
	TColor  FNewBuyBKColor;
	TColor  FNewSellColor;
	TColor  FNewSellBKColor;

	TColor  FFillBuyColor;
	TColor  FFillBuyBKColor;
	TColor  FFillSellColor;
	TColor  FFillSellBKColor;

	TColor  FPartiallyBuyColor;
	TColor  FPartiallyBuyBKColor;
	TColor  FPartiallySellColor;
	TColor  FPartiallySellBKColor;

	TColor  FPendingColor;
	TColor  FPendingBKColor;

	TColor  FRejectColor;
	TColor  FRejectBKColor;

	TColor  FCancelColor;
	TColor  FCancelBKColor;
	String  FStatusStrings[ 9 ];
	void __fastcall DrawGradientBar( TCanvas* canvas, TRect& dRect,TColor DownCol );
public:		// User declarations
	__fastcall TExecSettingForm(TComponent* Owner);
	void __fastcall LoadColor( void );
	void __fastcall SaveColor( void );
	void __fastcall DefaultColor( void );
};
//---------------------------------------------------------------------------
extern PACKAGE TExecSettingForm *ExecSettingForm;
//---------------------------------------------------------------------------
#endif
