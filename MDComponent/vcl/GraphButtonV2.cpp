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
		CaptureFlag(false)
{
	ParentWin = dynamic_cast<TWinControl*>(Owner);
	FFontInfo = new TFontAttrib();
	FFontInfo->OnChange = FontPropertyChange;
    InitialGraph();
	Width = 32;
	Height = 32;
}
//---------------------------------------------------------------------------
__fastcall TGraphButtonV2::~TGraphButtonV2(void)
{
	if (FFontInfo != NULL)
	{
		delete FFontInfo;
		FFontInfo = NULL;
	}
	delete FNormalGraph;
	delete FHoverGraph;
	delete FDownGraph;
	delete FDisableGraph;
	delete FSelectedGraph;
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::InitialGraph(void)
{
	FNormalGraph	= new TPicture();
	FHoverGraph		= new TPicture();
	FDownGraph		= new TPicture();
	FDisableGraph	= new TPicture();
	FSelectedGraph	= new TPicture();
	FNormalGraph	->OnChange = GraphChanged;
	FHoverGraph		->OnChange = GraphChanged;
	FDownGraph		->OnChange = GraphChanged;
	FSelectedGraph	->OnChange = GraphChanged;
	FSelectedGraph	->OnChange = GraphChanged;
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::SetNormalGraph(TPicture* graph)
{
	FNormalGraph->Assign(graph);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::SetHoverGraph(TPicture* graph)
{
	FHoverGraph->Assign(graph);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::SetDownGraph(TPicture* graph)
{
	FDownGraph->Assign(graph);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::SetDisableGraph(TPicture* graph)
{
	FDisableGraph->Assign(graph);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::SetSelectedGraph(TPicture* graph)
{
	FSelectedGraph->Assign(graph);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::PaintParent(void)
{
	 int SaveIndex;
	 HDC DC;

	 if (HandleAllocated())
	 {
		 DC = GetDC(Handle);
		 if (DC == NULL) return;
		 SaveIndex = SaveDC(DC);
		 SetViewportOrgEx(DC, -Left, -Top, NULL);
		 Parent->Perform(WM_ERASEBKGND, (int)DC, 0);
		 RestoreDC(DC, SaveIndex);
		 ReleaseDC(Handle, DC);
	 }
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::FontPropertyChange(TObject*)
{
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::GraphChanged(TObject* Sender)
{
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::SetFontAttrib(TFontAttrib *Value)
{
	FFontInfo->Assign(Value);
	FontPropertyChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::SetAlignType(CaptionAlign Value)
{
	AlignType = Value;
	FontPropertyChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::SetSelectFlag(bool Value)
{
	SelectedFlag = Value;
	FontPropertyChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::SetBtnStyle(ButtonStyle Value)
{
	FBtnStyle = Value;
	FontPropertyChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::SetText(const String Value)
{
	CaptionText = Value;
	FontPropertyChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::Click(void)
{
	if (CaptureFlag)
	{
		ReleaseCapture();
		CaptureFlag = false;
		Paint();
	}
	TCustomControl::Click();
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::KeyDown(Word &Key, TShiftState Shift)
{
	if (Key == VK_RETURN)
		Click();
	TCustomControl::KeyDown(Key, Shift);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::MouseMove(TShiftState Shift, int X, int Y)
{
	if (!CaptureFlag)
	{
		if (HandleAllocated())
			SetCapture(Handle);
		CaptureFlag = true;
		Paint();
		if (FOnMouseMoveIn != NULL)
			FOnMouseMoveIn(this);
	}
	if ((X < 0) || (Y < 0) || (X > Width) || (Y > Height))
	{
		ReleaseCapture();
		CaptureFlag = false;
		Paint();
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
		PaintButton(FFontInfo->FontColor, FFontInfo->FontShadowColor, bsMouseDown);
	SetFocus();
	TCustomControl::MouseDown(Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::MouseUp(TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button == mbLeft)
		Paint();
	TCustomControl::MouseUp(Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::PaintButton(TColor Front, TColor Back, ButtonState State)
{
	register int OffsetX = 0, OffsetY;
	register int TextHPixels, TextVPixels;
	TCanvas *canvas;

	if (!HandleAllocated())
		return;
	canvas = new TCanvas();
	canvas->Handle = GetDC(Handle);
	canvas->Brush->Style = bsClear;
	canvas->Font->Assign(FFontInfo->CaptionFont);
	TextVPixels = canvas->TextHeight(CaptionText);
	if (TextVPixels > Height)
		OffsetY = 0;
	else
		OffsetY = (Height - TextVPixels) / 2;

	switch (AlignType)
	{
		case caCenter:
			if (TextHPixels > Width)
				OffsetX = 0;
			else
				OffsetX = (Width - TextHPixels) / 2;
			break;
		case caRight:
			OffsetX = Width - TextHPixels;
			break;
	}
	// paint graph
	/*
	...
	*/
	if (FFontInfo->ShadowOffsetPixels != ::soNone)
	{
		canvas->Font->Color = Back;
		canvas->TextOut(OffsetX + (int)FFontInfo->ShadowOffsetPixels,
		                OffsetY + (int)FFontInfo->ShadowOffsetPixels,
		                CaptionText);
	}
	canvas->Font->Color = Front;
	canvas->TextOut(OffsetX, OffsetY, CaptionText);
	ReleaseDC(Handle, canvas->Handle);
	delete canvas;
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::Paint(void)
{
	const bool isWindowsStyle = (BtnStyle == bsWindows);
	const bool isHoverState = isWindowsStyle ? IsFocused : CaptureFlag;

	if (!Enabled)
	{
		PaintButton(FFontInfo->HighlightFontColor, FFontInfo->HighlightShadowColor, bsDisableState);
	}
	else if (SelectedFlag)
	{
		PaintButton(FFontInfo->SelectedFontColor, FFontInfo->HighlightShadowColor, bsSelectedState);
	}
	else if (isHoverState)
	{
		PaintButton(FFontInfo->HighlightFontColor, FFontInfo->HighlightShadowColor, bsMouseOver);
	}
	else
	{
		PaintButton(FFontInfo->FontColor, FFontInfo->FontShadowColor, bsNormalState);
	}
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::WMLosingCapture(TMessage &)
{
	CaptureFlag = false;
	Paint();
	if (FOnMouseMoveOut != NULL)
		FOnMouseMoveOut(this);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::WMEraseBkgnd(TMessage &Msg)
{
	Paint();
	Msg.Result = 1;
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::SetFocused(bool Value)
{
	if (Value != FIsFocused)
	{
		FIsFocused = Value;
		Paint();
	}
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonV2::WndProc(TMessage &Msg)
{
	switch (Msg.Msg)
	{
		case WM_SETFOCUS:
			Msg.Result = 0;
			IsFocused = true;
			TCustomControl::WndProc(Msg);
			break;
		case WM_KILLFOCUS:
			Msg.Result = 0;
			IsFocused = false;
			TCustomControl::WndProc(Msg);
			break;
		case WM_CAPTURECHANGED:
			WMLosingCapture(Msg);
			break;
		case WM_ERASEBKGND:
			WMEraseBkgnd(Msg);
			break;
		default:
			TCustomControl::WndProc(Msg);
			break;
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
