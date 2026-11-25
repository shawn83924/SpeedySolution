//---------------------------------------------------------------------------

#ifndef UnifyDlgH
#define UnifyDlgH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <vector>
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
	struct TColorTextSegment	///< One run of text drawn in a single color.
	{
		String Text;
		TColor Color;
	};
	std::vector<TColorTextSegment> FSegments;
	TPaintBox* FColorTextBox;

	void __fastcall ParseColorText( const String& text, TColor DefColor );
	String __fastcall GetPlainText( void );
	void __fastcall ColorTextBoxPaint( TObject* Sender );
	static bool __fastcall TryParseColor( const String& Name, TColor& Color );
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
	///< Same as AskYesNoDialog, but text may contain color tags, e.g. L"<c=red>Buy</c> TXF".
	///< Color is a VCL color name without "cl" ( red, green, blue... ) or "$BBGGRR".
	static bool __fastcall AskYesNoColorDialog( const String& caption, const String& text );
	static void __fastcall OpenBrowser( const String& URL );
};
//---------------------------------------------------------------------------
extern PACKAGE TUnifyDlgs *UnifyDlgs;
//---------------------------------------------------------------------------
#endif
