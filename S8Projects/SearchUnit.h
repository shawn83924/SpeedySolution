//---------------------------------------------------------------------------

#ifndef SearchUnitH
#define SearchUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "GraphButton.h"
//---------------------------------------------------------------------------
class TSearchSymbolForm : public TForm
{
__published:	// IDE-managed Components
	TEdit *SearchEditBox;
	TGraphButton *SearchButton;
	void __fastcall SearchEditBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall SearchEditBoxMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall SearchButtonClick(TObject *Sender);
	void __fastcall FormDeactivate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);

private:	// User declarations
	void __fastcall WndProc( TMessage &Msg );
	void __fastcall PaintBk( TMessage &Msg );
public:		// User declarations
	__fastcall TSearchSymbolForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSearchSymbolForm *SearchSymbolForm;
//---------------------------------------------------------------------------
#endif
