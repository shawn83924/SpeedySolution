//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "GraphTrackBar.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TGraphTrackBar *)
{
    new TGraphTrackBar(NULL);
}
//---------------------------------------------------------------------------
__fastcall TGraphTrackBar::TGraphTrackBar(TComponent* Owner)
    : TCustomControl(Owner),FMin(0),FMax(100),FPos(0),FMouseDown(false)
{
    FFTImage    = new Graphics::TBitmap();
    FBKImage    = new Graphics::TBitmap();
    FThumbImage = new Graphics::TBitmap();
}
//---------------------------------------------------------------------------
__fastcall TGraphTrackBar::~TGraphTrackBar( )
{
    delete FFTImage;
    delete FBKImage;
    delete FThumbImage;
}
//---------------------------------------------------------------------------
void __fastcall TGraphTrackBar::SetPos( int Value )
{
    if( FPos != Value )
    {
        FPos = Value;
        if( FPos > FMax )
            FPos = FMax;
        else if( FPos < FMin )
            FPos = FMin;
        if( OnPositionChange != NULL )
            OnPositionChange( this );
        Paint( );
    }
}
//---------------------------------------------------------------------------
void __fastcall TGraphTrackBar::WndProc( TMessage &Msg )
{
    if( Msg.Msg == WM_ERASEBKGND )
    {
        Msg.Result = 1;
        return;
    }
    else
    {
        inherited::WndProc( Msg );
    }
}
//---------------------------------------------------------------------------
void __fastcall TGraphTrackBar::SetFront( Graphics::TBitmap *Value )
{
    TRect ControlRect = Rect(0,0,Width,Height);
    FFTImage->Width  = Width;
    FFTImage->Height = Height;
    FFTImage->Canvas->StretchDraw( ControlRect, Value );
}
//---------------------------------------------------------------------------
void __fastcall TGraphTrackBar::SetThumb( Graphics::TBitmap *Value )
{
    FThumbImage->Assign( Value );
    FThumbImage->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TGraphTrackBar::SetBackground( Graphics::TBitmap *Value )
{
    TRect ControlRect = Rect(0,0,Width,Height);
    FBKImage->Width  = Width;
    FBKImage->Height = Height;
    FBKImage->Canvas->StretchDraw( ControlRect, Value );
}
//---------------------------------------------------------------------------
int __fastcall TGraphTrackBar::ToPosition( int X )
{
    float Ratio = (float)(FMax-FMin) /(float) (Width - FThumbImage->Width );
    return (int)( (float)( X - (FThumbImage->Width/2) ) * Ratio );
}
//---------------------------------------------------------------------------
void __fastcall TGraphTrackBar::MouseDown(TMouseButton , Classes::TShiftState , int X, int )
{
    FMouseDown = true;
    SetPos( ToPosition(X) );
}
//---------------------------------------------------------------------------
void __fastcall TGraphTrackBar::MouseUp(TMouseButton , Classes::TShiftState , int , int )
{
    FMouseDown = false;
}
//---------------------------------------------------------------------------
void __fastcall TGraphTrackBar::MouseMove(Classes::TShiftState , int X, int )
{
    if( FMouseDown )
        SetPos( ToPosition(X) );
}
//---------------------------------------------------------------------------
void __fastcall TGraphTrackBar::Paint(void)
{
    int   Pixels   = Width - FThumbImage->Width;
    float Ratio    = (float) Pixels / (float)(FMax-FMin);
    int   ThumbPos = (int)((float)FPos * Ratio);
    TRect FTRect = Rect(0,0,ThumbPos,Height);
    TRect BKRect = Rect(ThumbPos,0,Width,Height);
    Graphics::TBitmap *FBuffer = new Graphics::TBitmap();

    FBuffer->Width  = Width;
    FBuffer->Height = Height;
    FBuffer->Canvas->CopyRect( FTRect,FFTImage->Canvas,FTRect);
    FBuffer->Canvas->CopyRect( BKRect,FBKImage->Canvas,BKRect);
    FBuffer->Canvas->Draw( ThumbPos,0, FThumbImage );
    Canvas->Draw( 0,0, FBuffer );
    delete FBuffer;
}
//---------------------------------------------------------------------------
namespace Graphtrackbar
{
    void __fastcall PACKAGE Register()
    {
         TComponentClass classes[1] = {__classid(TGraphTrackBar)};
         RegisterComponents("MD", classes, 0);
    }
}
//---------------------------------------------------------------------------
