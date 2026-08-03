//---------------------------------------------------------------------------

#ifndef UnifyDlgH
#define UnifyDlgH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "RoundFormEx.h"
//---------------------------------------------------------------------------
class TUnifyDlgs : public TForm
{
__published:	// IDE-managed Components
	TRoundFormEx *RoundFormEx;
	TEdit *NameEdit;
	TButton *OKButton;
	TButton *CancelButton;
	TLabel *MsgLabel;
	void __fastcall NameEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TUnifyDlgs(TComponent* Owner);
	static bool __fastcall AskStringDialog( const String& caption, String& Name );
	static void __fastcall MessageDialog( const String& caption, const String& text );
	static bool __fastcall AskYesNoDialog( const String& caption, const String& text );
	static bool __fastcall AskYesNoDialog(
		const String& caption,
		const String& text,
		const String& YesStr,
		const String& CancelStr
		);
	static void __fastcall OpenBrowser( const String& URL );
};
//---------------------------------------------------------------------------
extern PACKAGE TUnifyDlgs *UnifyDlgs;
//---------------------------------------------------------------------------
#endif
