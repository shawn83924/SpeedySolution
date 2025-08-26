//---------------------------------------------------------------------------
#ifndef ProfileGraphH
#define ProfileGraphH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "PerformanceGauge.h"
#include <Grids.hpp>
#include <ValEdit.hpp>
//---------------------------------------------------------------------------
class TProfileFrame : public TFrame
{
__published:	// IDE-managed Components
	TPerformanceGauge *PerformanceGauge;
	TStaticText *TitleStaticText;
	TValueListEditor *ValueListEditor;
	void __fastcall ValueListEditorDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
private:	// User declarations
	int     FPrevCount;
	int     FPrevTick;
	float   FMaxThroughput;
	Graphics::TBitmap* FBuffer;
public:		// User declarations
	__fastcall TProfileFrame(TComponent* Owner);
	__fastcall TProfileFrame(TWinControl* Owner, int ID, const AnsiString& Title );
	void __fastcall SetBackgroundColor( TColor Color );
	void __fastcall SetGridColor( TColor Color );
	void __fastcall SetLineColor( TColor Color );
	void __fastcall SetData( int Tick, const char* DataStr );
};
//---------------------------------------------------------------------------
extern PACKAGE TProfileFrame *ProfileFrame;
//---------------------------------------------------------------------------
#endif
