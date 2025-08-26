//---------------------------------------------------------------------------

#ifndef AskRoomiH
#define AskRoomiH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "RoundFormEx.h"
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>
//---------------------------------------------------------------------------
class TAskRoomiForm : public TForm
{
__published:	// IDE-managed Components
	TRoundFormEx *RoundFormEx;
	TImage *Image1;
	TButton *CancelButton;
	TButton *OKButton;
	TLabel *Label1;
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TAskRoomiForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAskRoomiForm *AskRoomiForm;
//---------------------------------------------------------------------------
#endif
