//---------------------------------------------------------------------------

#ifndef MessageDialogH
#define MessageDialogH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class TMessageForm : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TComboBox *UserComboBox;
	TLabel *Label3;
	TEdit *HeaderEdit;
	TBitBtn *OkBtn;
	TBitBtn *CancelBtn;
	TLabel *Label2;
	TEdit *TextEdit;
	void __fastcall OkBtnClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TMessageForm(TComponent* Owner);
	TStringList* Users;
};
//---------------------------------------------------------------------------
extern PACKAGE TMessageForm *MessageForm;
//---------------------------------------------------------------------------
#endif
