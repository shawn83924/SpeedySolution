//---------------------------------------------------------------------------

#ifndef StarWaveFrameH
#define StarWaveFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "PerformanceGauge.h"
#include <Grids.hpp>
#include <ValEdit.hpp>
//---------------------------------------------------------------------------
#include "MessageObject.h"
#include "Publisher.h"
#include "Subscriber.h"
//---------------------------------------------------------------------------
class TSWFrame : public TFrame
{
__published:	// IDE-managed Components
	TValueListEditor *ValueListEditor;
	TPerformanceGauge *PerformanceGauge;
	TStaticText *TitleStaticText;
	void __fastcall ValueListEditorDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TGridDrawState State);
private:	// User declarations
	float   FMaxThroughput;
	Graphics::TBitmap* FBuffer;
public:		// User declarations
	__fastcall TSWFrame(TComponent* Owner);
	__fastcall TSWFrame(TWinControl* Owner, const String& Hostname );
	void __fastcall UpdateData( MTree *Tree );
};
//---------------------------------------------------------------------------
extern PACKAGE TSWFrame *SWFrame;
//---------------------------------------------------------------------------
#endif
