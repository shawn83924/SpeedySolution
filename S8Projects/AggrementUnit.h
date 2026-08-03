//---------------------------------------------------------------------------

#ifndef AggrementUnitH
#define AggrementUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "RoundFormEx.h"
#include "UFC.h"
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TAggrementForm : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TButton *OKButton;
	TButton *CancelButton;
	TScrollBox *ScrollBox1;
	TCheckBox *CheckBox1;
	TCheckBox *CheckBox2;
	TCheckBox *CheckBox3;
	TCheckBox *CheckBox4;
	TCheckBox *CheckBox5;
	TCheckBox *CheckBox6;
	TCheckBox *CheckBox7;
	TRoundFormEx *RoundFormEx;
	void __fastcall CheckBox1Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
	UFC::List<TCheckBox*> FCheckBoxs;
	bool __fastcall AllCjecked( void );
public:		// User declarations
	__fastcall TAggrementForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAggrementForm *AggrementForm;
//---------------------------------------------------------------------------
#endif
