//---------------------------------------------------------------------------

#ifndef SelAccountH
#define SelAccountH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "RoundFormEx.h"
//---------------------------------------------------------------------------
class TDefAccForm : public TForm
{
__published:	// IDE-managed Components
	TButton *CancelButton;
	TButton *OKButton;
	TRoundFormEx *RoundFormEx;
	TLabel *Label1;
	TLabel *Label2;
	TComboBox *FUTComboBox;
	TComboBox *TSEComboBox;
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TDefAccForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDefAccForm *DefAccForm;
//---------------------------------------------------------------------------
#endif
