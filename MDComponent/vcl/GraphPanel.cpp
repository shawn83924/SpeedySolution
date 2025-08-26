//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "GraphPanel.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TGraphPanel *)
{
	new TGraphPanel(NULL);
}
//---------------------------------------------------------------------------
__fastcall TGraphPanel::TGraphPanel(TComponent* Owner)
	: TCustomPanel(Owner),
	FAutoSize(false),
	FStretchGlyph(false),
	FTransparent(false)
{
	FGlyph = new Graphics::TBitmap();
	FGlyph->OnChange = DoGlyphChange;
}
//---------------------------------------------------------------------------
__fastcall TGraphPanel::~TGraphPanel()
{
	delete FGlyph;
}
//---------------------------------------------------------------------------
TCanvas* __fastcall TGraphPanel::GetCanvas( void )
{
	return Canvas;
}
//---------------------------------------------------------------------------
void __fastcall TGraphPanel::SetGlyph(Graphics::TBitmap *Value)
{
	FGlyph->Assign( Value );
	FGlyph->Dormant();
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TGraphPanel::SetAutoSize(bool Value)
{
	if(FAutoSize != Value)
	{
		FAutoSize = Value;

		if(FAutoSize && !FGlyph->Empty &&
			(Width!=FGlyph->Width || Height!=FGlyph->Height) )
		{
			inherited::SetBounds( Left, Top, FGlyph->Width, FGlyph->Height );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TGraphPanel::SetStretchGlyph(bool Value)
{
	if(FStretchGlyph != Value)
	{
		FStretchGlyph = Value;
		Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TGraphPanel::SetTransparent(bool Value)
{
	if(FTransparent != Value)
	{
		FTransparent = Value;
		FGlyph->Transparent = FTransparent;
		FGlyph->TransparentMode = tmAuto;
		Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TGraphPanel::SetBounds(int ALeft, int ATop, int AWidth, int AHeight)
{
	 if(!FAutoSize)
		  inherited::SetBounds(ALeft, ATop, AWidth, AHeight);
	 else
		  inherited::SetBounds(ALeft, ATop, Width, Height);
}
//---------------------------------------------------------------------------
void __fastcall TGraphPanel::SetClip( TCanvas* canvas )
{
	HRGN  CtrlRgn;

	FClipRgn = CreateRectRgn( 0,0, Width, Height );
	for( int i = 0; i < ControlCount; i++ )
	{
		TRect CtrlRect = Controls[i]->BoundsRect;
		CtrlRgn = CreateRectRgnIndirect( &CtrlRect );
		CombineRgn( FClipRgn, FClipRgn, CtrlRgn, RGN_DIFF );
		DeleteObject( CtrlRgn );
	}
	SelectClipRgn( canvas->Handle, FClipRgn );
}
//---------------------------------------------------------------------------
void __fastcall TGraphPanel::ResetClip( TCanvas* canvas )
{
	SelectClipRgn( canvas->Handle, NULL );
	DeleteObject( FClipRgn );
}
//---------------------------------------------------------------------------
void __fastcall TGraphPanel::Paint(void)
{
	if( !FGlyph->Empty ) ///< Contains image
	{
		if( FTransparent == true ) ///< FTransparent = true, draw the background image first.
			inherited::Paint();
		if( FStretchGlyph && !FAutoSize )
		{
			TRect PaintRect( 0,0,Width + 1,Height);
			Canvas->StretchDraw( PaintRect,FGlyph);
		}
		else
			Canvas->Draw( 0, 0, FGlyph );
		if( OnPaint != NULL )
			OnPaint( this );
	}
	else
	{
			if( OnPaint != NULL )
				OnPaint( this );
//			else
//				inherited::Paint();
	}
}
//---------------------------------------------------------------------------
void __fastcall TGraphPanel::DoGlyphChange(TObject* )
{
	// Update for property AutoSize
	SetAutoSize(FAutoSize);
	// Update for property Transparent
	SetTransparent(FTransparent);
}
//---------------------------------------------------------------------------
void __fastcall TGraphPanel::EraseBK( HDC DC )
{
	int SaveIndex = SaveDC( DC );
	TRect    PaintRect( 0, 0, Width, Height );
	TCanvas* pCanvas;

	pCanvas = new TCanvas( );
	pCanvas->Handle = DC;
//	SetClip( pCanvas );
	if( !FGlyph->Empty ) ///< With Background image
	{
		if( FStretchGlyph && !FAutoSize)
			pCanvas->StretchDraw( PaintRect, FGlyph );
		else
			pCanvas->Draw( 0, 0, FGlyph);
	}
	else
	{
		pCanvas->Brush->Color = Color;
		pCanvas->FillRect( PaintRect );
	}
//	ResetClip( pCanvas );
	delete pCanvas;
	RestoreDC( DC, SaveIndex );
}
//---------------------------------------------------------------------------
void __fastcall TGraphPanel::WndProc( TMessage &Msg )
{
	if( Msg.Msg == WM_ERASEBKGND )
	{
		EraseBK( (HDC) Msg.WParam );
		Msg.Result = 1;
		return;
	}
	inherited::WndProc( Msg );
}
//---------------------------------------------------------------------------
namespace Graphpanel
{
	void __fastcall PACKAGE Register()
    {
         TComponentClass classes[1] = {__classid(TGraphPanel)};
         RegisterComponents("MD", classes, 0);
    }
}
//---------------------------------------------------------------------------
