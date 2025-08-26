//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "GraphMarquee.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TGraphMarquee *)
{
    new TGraphMarquee(NULL);
}
//---------------------------------------------------------------------------
__fastcall TGraphMarquee::TGraphMarquee(TComponent* Owner)
    : TCustomControl(Owner),
    FScrollDelay( 100 ),
    FScrollPixel( 1 ),
    FPos( 0 ),
    FEnable( false )
{
    FTimer   = new TTimer(this);
    FGraph   = new Graphics::TBitmap();
    FBKImage = new Graphics::TBitmap();
    FTimer->Interval = FScrollDelay;
    FTimer->OnTimer  = OnTimerUpdate;
    FTimer->Enabled  = false;
}
//---------------------------------------------------------------------------
__fastcall TGraphMarquee::~TGraphMarquee( void )
{
    delete FTimer;
    delete FGraph;
    delete FBKImage;
}
//---------------------------------------------------------------------------
void __fastcall TGraphMarquee::Paint(void)
{
	int GraphWidth = FGraph->Width;
	if( GraphWidth!= 0 )
	{
		TRect PaintRect(0,0,Width,Height);

		FBKImage->Width  = Width;
		FBKImage->Height = Height;
		for( register int i = FPos; i < Width + GraphWidth; i+= GraphWidth )
			 FBKImage->Canvas->Draw( i, 1, FGraph );
		for( register int j = FPos; j > -GraphWidth; j-= GraphWidth )
			 FBKImage->Canvas->Draw( j, 1, FGraph );
		Canvas->Draw( 0, 0,FBKImage );
		Frame3D( Canvas, PaintRect ,clGray,clWhite,1);
    }
}
//---------------------------------------------------------------------------
void __fastcall TGraphMarquee::OnTimerUpdate(TObject *)
{
    FPos += FScrollPixel;
    if( FPos > Width )
        FPos %= Width;
    Paint();
}
//---------------------------------------------------------------------------
void __fastcall TGraphMarquee::SetScrollDelay( int Value )
{
    if( Value > 0 && Value != FScrollDelay )
    {
        FScrollDelay     = Value;
        FTimer->Interval = FScrollDelay;
    }
}
//---------------------------------------------------------------------------
void __fastcall TGraphMarquee::SetEnabled( bool Value )
{
    if( FEnable != Value )
    {
        FEnable = Value;
        FTimer->Enabled  = FEnable;
    }
}
//---------------------------------------------------------------------------
void __fastcall TGraphMarquee::SetGraph( Graphics::TBitmap *Value )
{
    FGraph->Assign( Value );
    FGraph->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TGraphMarquee::WndProc( TMessage &Msg )
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
namespace Graphmarquee
{
    void __fastcall PACKAGE Register()
    {
         TComponentClass classes[1] = {__classid(TGraphMarquee)};
         RegisterComponents("MD", classes, 0);
    }
}
//---------------------------------------------------------------------------
 