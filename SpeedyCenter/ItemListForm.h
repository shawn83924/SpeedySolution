//---------------------------------------------------------------------------

#ifndef ItemListFormH
#define ItemListFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class TItemsForm : public TForm
{
__published:	// IDE-managed Components
	TListBox *ItemListBox;
	TEdit *NewItemEdit;
	TBitBtn *OkBtn;
	TBitBtn *CancelBtn;
	TBitBtn *DelBtn;
	TBitBtn *AddBtn;
	void __fastcall DelBtnClick(TObject *Sender);
	void __fastcall AddBtnClick(TObject *Sender);
	void __fastcall ItemListBoxClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TItemsForm(TComponent* Owner);
	TModalResult ShowDialog( const String& txt, TStringList* Items );
};
//---------------------------------------------------------------------------
extern PACKAGE TItemsForm *ItemsForm;
//---------------------------------------------------------------------------
#endif
