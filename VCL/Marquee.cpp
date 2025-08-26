//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Marquee.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TMarquee *)
{
    new TMarquee(NULL);
}
//---------------------------------------------------------------------------
__fastcall TMarquee::TMarquee(TComponent* Owner)
    : TCustomControl(Owner),
    FTransparent(false),
    FScrollDelay(100),
    FScrollPixel(1),
    FScrollType(stHorizontal),
    FRepeatCount(0),
    CurrentPos(0),
    CurrentRepeat(0)
{
    FTimer = new TTimer(this);
    FBKImg = new Graphics::TBitmap();
    FTimer->Interval = FScrollDelay;
    FTimer->OnTimer = OnTimerUpdate;
    FTimer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TMarquee::Restart()
{
    CurrentRepeat = 0;
    CurrentPos = 0;
}
//---------------------------------------------------------------------------
void __fastcall TMarquee::Loaded(void)
{
    Restart();
}
//---------------------------------------------------------------------------

void __fastcall TMarquee::Paint(void)
{
    FBKImg->Width  = Width;
    FBKImg->Height = Height;
    if(!FTransparent)
    {
        FBKImg->Canvas->Brush->Color = Color;
        FBKImg->Canvas->Brush->Style = bsSolid;
        FBKImg->Canvas->FillRect(TRect(0,0,Width,Height));
    }
    else
    {
        FBKImg->Canvas->Brush->Style = bsClear;
        EraseBK( FBKImg->Canvas->Handle );
    }

    int x, y;

    // Determine the position of text
    if(FScrollType==stHorizontal)
    {
        x = CurrentPos;

        if(FTextAlign==taTopLeft) y = 0;
        else if(FTextAlign==taCenter) y = (Height-TextExtent.cy)/2;
        else y = Height - TextExtent.cy;
    }
    else
    {
        y = CurrentPos;

        if(FTextAlign==taTopLeft) x = 0;
        else if(FTextAlign==taCenter) x = (Width-TextExtent.cx)/2;
        else x = Width - TextExtent.cx;
    }

    FBKImg->Canvas->Font = Canvas->Font;
    FBKImg->Canvas->TextOut(x, y, FText);
    Canvas->Draw( 0, 0, FBKImg );
}
//---------------------------------------------------------------------------
void __fastcall TMarquee::SetTransparent(bool Value)
{
    FTransparent = Value;
    Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TMarquee::SetScrollDelay(int Value)
{
    if(Value > 0)
    {
        FScrollDelay = Value;
        FTimer->Interval = FScrollDelay;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMarquee::SetScrollType(TScrollType Value)
{
    FScrollType = Value;
    Paint();
}
//---------------------------------------------------------------------------
void __fastcall TMarquee::SetRepeatCount(int Value)
{
    if(Value >= 0)
    {
        FRepeatCount = Value;
        Restart();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMarquee::SetText(AnsiString Value)
{
    FText = Value;
    TextExtent = Canvas->TextExtent(FText);
    Paint();
}
//---------------------------------------------------------------------------
void __fastcall TMarquee::SetTextAlign(TTextAlign Value)
{
    FTextAlign = Value;
    Paint();
}
//---------------------------------------------------------------------------
void __fastcall TMarquee::SetEnabled(bool Value)
{
    inherited::Enabled = Value;
    FTimer->Enabled = Value;
}
//---------------------------------------------------------------------------
void __fastcall TMarquee::SetFont(TFont *Value)
{
    Canvas->Font = Value;
    TextExtent = Canvas->TextExtent(FText);
}
//---------------------------------------------------------------------------
TFont * __fastcall TMarquee::GetFont()
{
    return Canvas->Font;
}
//---------------------------------------------------------------------------
bool __fastcall TMarquee::GetEnabled()
{
    return inherited::Enabled;
}
//---------------------------------------------------------------------------
void __fastcall TMarquee::OnTimerUpdate(TObject *)
{
    // no update if repeat count is reached
    if(FRepeatCount>0 && CurrentRepeat>=FRepeatCount) return;

    CurrentPos += FScrollPixel;

    // Determine if text has reached to the end
    if(FScrollType==stHorizontal)
    {
        if(FScrollPixel>0 && CurrentPos>Width)
        {
            CurrentRepeat ++;
            CurrentPos = 0;
        }
        else if(FScrollPixel<0 && CurrentPos+TextExtent.cx<0)
        {
            CurrentRepeat++;
            CurrentPos = Width;
        }
    }
    else
    {
        if(FScrollPixel>0 && CurrentPos>Height)
        {
            CurrentRepeat ++;
            CurrentPos = 0;
        }
        else if(FScrollPixel<0 && CurrentPos+TextExtent.cy<0)
        {
            CurrentRepeat ++;
            CurrentPos = Height;
        }
    }

    Paint();
}
//---------------------------------------------------------------------------
void __fastcall TMarquee::EraseBK( HDC DC )
{
    int SaveIndex = SaveDC( DC );
    SetViewportOrgEx( DC, -Left, -Top, NULL );
    Parent->Perform ( WM_PAINT, (int)DC, 0 );
    RestoreDC( DC, SaveIndex );
}
//---------------------------------------------------------------------------
void __fastcall TMarquee::WndProc( TMessage &Msg )
{
    if( Msg.Msg == WM_ERASEBKGND )
    {
		EraseBK( (HDC) Msg.WParam );
    }
    else
    {
         inherited::WndProc( Msg );
    }
}
//---------------------------------------------------------------------------
namespace Marquee
{
    void __fastcall PACKAGE Register()
    {
         TComponentClass classes[1] = {__classid(TMarquee)};
         RegisterComponents("MD", classes, 0);
    }
}
//---------------------------------------------------------------------------
