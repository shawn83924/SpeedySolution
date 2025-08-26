//---------------------------------------------------------------------------

#ifndef AskCloseAllH
#define AskCloseAllH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "RoundFormEx.h"
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
class TAskCloseAllForm : public TForm
{
__published:	// IDE-managed Components
	TComboBox *CloseAllStepComboBox;
	TRadioButton *MarketRadioButton;
	TRadioButton *RadioButton1;
	TButton *OKButton;
	TButton *CancelButton;
	TRoundFormEx *RoundFormEx;
	TEdit *LotsPerOrderEdit;
	TUpDown *LotsUpDown;
	TLabel *Label1;
	TLabel *Label2;
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TAskCloseAllForm(TComponent* Owner);
	bool __fastcall IsMarket( void );
	int __fastcall BetterTick( void );
	int __fastcall QtyPerOrder( void );
};
//---------------------------------------------------------------------------
extern PACKAGE TAskCloseAllForm *AskCloseAllForm;
//---------------------------------------------------------------------------
#endif
