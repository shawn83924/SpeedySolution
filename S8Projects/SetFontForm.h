//---------------------------------------------------------------------------

#ifndef SetFontFormH
#define SetFontFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "RoundFormEx.h"
#include "CandleStickChart.h"
//---------------------------------------------------------------------------
class TFontForm : public TForm
{
__published:	// IDE-managed Components
	TEdit *TextEdit;
	TComboBox *SizeComboBox;
	TLabel *Label1;
	TColorBox *TextColorBox;
	TSpeedButton *BoldButton;
	TSpeedButton *ItalicButton;
	TSpeedButton *UnderlineButton;
	TRoundFormEx *RoundFormEx;
	TButton *OKButton;
	TButton *CancelButton;
	TSpeedButton *SymbolButton;
	TSpeedButton *SpeedButton2;
	TSpeedButton *SpeedButton3;
	TSpeedButton *SpeedButton4;
	TSpeedButton *SpeedButton5;
	TSpeedButton *SpeedButton6;
	TSpeedButton *SpeedButton7;
	TSpeedButton *SpeedButton8;
	void __fastcall SizeComboBoxChange(TObject *Sender);
	void __fastcall TextColorBoxChange(TObject *Sender);
	void __fastcall BoldButtonClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall TextEditChange(TObject *Sender);
	void __fastcall SymbolButtonClick(TObject *Sender);
private:	// User declarations
	TText* FText;
public:		// User declarations
	__fastcall TFontForm(TComponent* Owner);
	void __fastcall SetTextObject( TText* txt );
};
//---------------------------------------------------------------------------
extern PACKAGE TFontForm *FontForm;
//---------------------------------------------------------------------------
#endif
