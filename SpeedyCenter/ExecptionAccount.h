//---------------------------------------------------------------------------

#ifndef ExecptionAccountH
#define ExecptionAccountH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include "..\UFC\iniFile.h"
//---------------------------------------------------------------------------
class TExceptionAccForm : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *CancelBtn;
	TBitBtn *OkBtn;
	TListView *AccountListView;
	TBitBtn *DelBtn;
	TBitBtn *AddBtn;
	TEdit *NewItemEdit;
	TComboBox *RuleComboBox;
	void __fastcall AddBtnClick(TObject *Sender);
	void __fastcall DelBtnClick(TObject *Sender);
	void __fastcall AccountListViewClick(TObject *Sender);
private:	// User declarations
	String ToUnicodeString( const UFC::AnsiString& str );
	const char* ToCString( const String& str );
public:		// User declarations
	__fastcall TExceptionAccForm(TComponent* Owner);
	void Init( UFC::UiniFile* ini );
	int __fastcall ItemCount();
	void __fastcall GetItems( UFC::UiniFile* ini );
};
//---------------------------------------------------------------------------
extern PACKAGE TExceptionAccForm *ExceptionAccForm;
//---------------------------------------------------------------------------
#endif
