//---------------------------------------------------------------------------

#ifndef NewValueFormH
#define NewValueFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class TAddValueForm : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *OkBtn;
	TBitBtn *CancelBtn;
	TComboBox *SectionComboBox;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *NameEdit;
	TEdit *ValueEdit;
	void __fastcall OkBtnClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TAddValueForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAddValueForm *AddValueForm;
//---------------------------------------------------------------------------
#endif
