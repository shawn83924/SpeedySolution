//---------------------------------------------------------------------------

#ifndef ChangeSessionStatusH
#define ChangeSessionStatusH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TChangeSessionStatusForm : public TForm
{
__published:	// IDE-managed Components
	TRadioGroup *StatusGroupBox;
	TBitBtn *OkBtn;
	TBitBtn *CancelBtn;
private:	// User declarations
public:		// User declarations
	__fastcall TChangeSessionStatusForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TChangeSessionStatusForm *ChangeSessionStatusForm;
//---------------------------------------------------------------------------
#endif
