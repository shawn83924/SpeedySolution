//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <XPMan.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TButton *FCreateNewKeyButton;
	TLabel *Label3;
	TEdit *FVendorEdit;
	TMemo *FMemo;
	TXPManifest *XPManifest1;
	TLabel *Label1;
	TSaveDialog *SaveDialog;
	void __fastcall FCreateNewKeyButtonClick(TObject *Sender);
private:	// User declarations
	AnsiString __fastcall CreateKey( AnsiString& Vender, AnsiString& Subject );
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
