//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "GraphButtonV2.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TGraphButtonV2 *)
{
	new TGraphButtonV2(NULL);
}
//---------------------------------------------------------------------------
__fastcall TGraphButtonV2::TGraphButtonV2(TComponent* Owner)
	:	TCustomControl(Owner),
		CaptureFlag(false),
		FMouseDown(false),
		SelectedFlag(false),
		FIsFocused(false),
		AlignType(caCenter)
{
	FFontInfo = new TFontAttrib();
	FFontInfo->OnChange = FontPropertyChange;
	InitialGraph();
	DoubleBuffered = true;
	Width  = 32;
	Height = 32;
}
//---------------------------------------------------------------------------
__fastcall TGraphButtonV2::~TGraphButtonV2(void)
{
	delete FFontInfo;
	delete FNormalGraph;
	delete FHoverGraph;
	delete FDownGraph;
	delete FDisableGraph;
	delete FSelectedGraph;
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::InitialGraph(void)
{
	FNormalGraph   = new TPicture();
	FHoverGraph    = new TPicture();
	FDownGraph     = new TPicture();
	FDisableGraph  = new TPicture();
	FSelectedGraph = new TPicture();
	FNormalGraph  ->OnChange = GraphChanged;
	FHoverGraph   ->OnChange = GraphChanged;
	FDownGraph    ->OnChange = GraphChanged;
	FDisableGraph ->OnChange = GraphChanged;
	FSelectedGraph->OnChange = GraphChanged;
}
//---------------------------------------------------------------------------
// ── Property setters ───────────────────────────────────────────────────────
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::SetGraph(TPicture* &target, TPicture* graph)
{
	target->Assign(graph);
	Invalidate();
}

void __fastcall TGraphButtonV2::SetNormalGraph (TPicture* g) { SetGraph(FNormalGraph,   g); }
void __fastcall TGraphButtonV2::SetHoverGraph  (TPicture* g) { SetGraph(FHoverGraph,    g); }
void __fastcall TGraphButtonV2::SetDownGraph   (TPicture* g) { SetGraph(FDownGraph,     g); }
void __fastcall TGraphButtonV2::SetDisableGraph(TPicture* g) { SetGraph(FDisableGraph,  g); }
void __fastcall TGraphButtonV2::SetSelectedGraph(TPicture* g){ SetGraph(FSelectedGraph, g); }

void __fastcall TGraphButtonV2::SetSelectFlag(bool Value)      { SelectedFlag = Value;          Invalidate(); }
void __fastcall TGraphButtonV2::SetText(const String Value)    { CaptionText  = Value;          Invalidate(); }
void __fastcall TGraphButtonV2::SetAlignType(CaptionAlign Value){ AlignType   = Value;          Invalidate(); }
void __fastcall TGraphButtonV2::SetFontAttrib(TFontAttrib *Value){ FFontInfo->Assign(Value);    Invalidate(); }
void __fastcall TGraphButtonV2::GraphChanged(TObject*)         {                                Invalidate(); }
void __fastcall TGraphButtonV2::FontPropertyChange(TObject*)   {                                Invalidate(); }

void __fastcall TGraphButtonV2::SetFocused(bool Value)
{
	if (Value != FIsFocused) { FIsFocused = Value; Invalidate(); }
}
//---------------------------------------------------------------------------
// ── Paint parent background (for PNG transparency) ─────────────────────────
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::PaintParent(void)
{
	if (Parent == NULL)
		return;
	int SaveIndex = SaveDC(Canvas->Handle);
	SetViewportOrgEx(Canvas->Handle, -Left, -Top, NULL);
	Parent->Perform(WM_ERASEBKGND, (WPARAM)Canvas->Handle, 0);
	Parent->Perform(WM_PAINT, (WPARAM)Canvas->Handle, 0);
	RestoreDC(Canvas->Handle, SaveIndex);
}
//---------------------------------------------------------------------------
// ── Draw text with alignment and optional shadow ───────────────────────────
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::DrawCaption(void)
{
	if (CaptionText.IsEmpty()) return;

	Canvas->Font->Assign(FFontInfo->CaptionFont);
	Canvas->Brush->Style = bsClear;

	int textW = Canvas->TextWidth(CaptionText);
	int textH = Canvas->TextHeight(CaptionText);
	int offsetY = (textH < Height) ? (Height - textH) / 2 : 0;
	int offsetX;

	switch (AlignType)
	{
		case caLeft:   offsetX = 2; break;
		case caRight:  offsetX = (textW < Width) ? Width - textW - 2 : 0; break;
		case caCenter:
		default:       offsetX = (textW < Width) ? (Width - textW) / 2 : 0; break;
	}

	// Shadow
	if (FFontInfo->ShadowOffsetPixels != ::soNone)
	{
		int off = (int)FFontInfo->ShadowOffsetPixels;
		Canvas->Font->Color = FFontInfo->FontShadowColor;
		Canvas->TextOut(offsetX + off, offsetY + off, CaptionText);
	}

	// Main text
	Canvas->Font->Color = Enabled ? FFontInfo->FontColor : clGrayText;
	Canvas->TextOut(offsetX, offsetY, CaptionText);
}
//---------------------------------------------------------------------------
// ── Core paint ─────────────────────────────────────────────────────────────
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::Paint(void)
{
	// 1. Paint parent background so PNG alpha channels look correct
	PaintParent();

	// 2. Select picture by state priority:
	//    Disabled > MouseDown > Selected > Hover > Normal
	TPicture* picture;
	if (!Enabled)
		picture = FDisableGraph;
	else if (FMouseDown)
		picture = FDownGraph;
	else if (SelectedFlag)
		picture = FSelectedGraph;
	else if (CaptureFlag)
		picture = FHoverGraph;
	else
		picture = FNormalGraph;

	// 3. Draw background image (stretch to fill)
	if (picture->Graphic != NULL && !picture->Graphic->Empty)
		Canvas->StretchDraw(ClientRect, picture->Graphic);

	// 4. Draw caption on top
	DrawCaption();
}
//---------------------------------------------------------------------------
// ── Mouse / keyboard events ────────────────────────────────────────────────
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::MouseMove(TShiftState Shift, int X, int Y)
{
	const bool outside = (X < 0 || Y < 0 || X > Width || Y > Height);

	if (!CaptureFlag && !outside)
	{
		if (HandleAllocated()) SetCapture(Handle);
		CaptureFlag = true;
		Invalidate();
		if (FOnMouseMoveIn != NULL) FOnMouseMoveIn(this);
	}
	else if (CaptureFlag && outside)
	{
		ReleaseCapture();
		CaptureFlag = false;
		FMouseDown  = false;
		Invalidate();
	}
	else
	{
		TCustomControl::MouseMove(Shift, X, Y);
	}
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::MouseDown(TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button == mbLeft)
	{
		FMouseDown = true;
		Invalidate();
	}
	SetFocus();
	TCustomControl::MouseDown(Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::MouseUp(TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button == mbLeft)
	{
		FMouseDown = false;
		Invalidate();
	}
	TCustomControl::MouseUp(Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::Click(void)
{
	TCustomControl::Click();
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::KeyDown(Word &Key, TShiftState Shift)
{
	if (Key == VK_RETURN) Click();
	TCustomControl::KeyDown(Key, Shift);
}
//---------------------------------------------------------------------------
// ── Window message handling ────────────────────────────────────────────────
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::WMEraseBkgnd(TMessage &Msg)
{
	// We paint everything ourselves in Paint(); suppress default erase.
	Msg.Result = 1;
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::WMLosingCapture(TMessage &)
{
	CaptureFlag = false;
	Invalidate();
	if (FOnMouseMoveOut != NULL)
		FOnMouseMoveOut(this);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::WndProc(TMessage &Msg)
{
	switch (Msg.Msg)
	{
		case WM_SETFOCUS:       IsFocused = true;  TCustomControl::WndProc(Msg); break;
		case WM_KILLFOCUS:      IsFocused = false; TCustomControl::WndProc(Msg); break;
		case WM_CAPTURECHANGED: WMLosingCapture(Msg); break;
		case WM_ERASEBKGND:     WMEraseBkgnd(Msg);   break;
		default:                TCustomControl::WndProc(Msg); break;
	}
}
//---------------------------------------------------------------------------
namespace Graphbuttonv2
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TGraphButtonV2)};
		RegisterComponents(L"MD", classes, 0);
	}
}
//---------------------------------------------------------------------------
