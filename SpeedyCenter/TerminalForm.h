//---------------------------------------------------------------------------

#ifndef TerminalFormH
#define TerminalFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TTermForm : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *OKBtn;
	TBitBtn *CancelBtn;
	TMemo *TermMemo;
	TStaticText *StaticText;
	TCheckBox *Num62CheckBox;
	void __fastcall OKBtnClick(TObject *Sender);
	void __fastcall TermMemoChange(TObject *Sender);
	void __fastcall TermMemoKeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall Num62CheckBoxClick(TObject *Sender);

private:	// User declarations
	bool FIsTFX;
	bool FIsTermEx;
public:		// User declarations
	__fastcall TTermForm(TComponent* Owner, bool IsTFX, bool ThreeDigi, bool Use62Carry );
	bool __fastcall Is62Carry( void ) { return Num62CheckBox->Checked; }
};
//---------------------------------------------------------------------------
extern PACKAGE TTermForm *TermForm;
//---------------------------------------------------------------------------
#endif
