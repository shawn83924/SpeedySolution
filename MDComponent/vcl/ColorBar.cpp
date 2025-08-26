//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "ColorBar.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TColorBar *)
{
    new TColorBar(NULL);
}
//---------------------------------------------------------------------------
__fastcall TColorBar::TColorBar(TComponent* Owner)
    : TCustomControl(Owner)
{
    FbMouseIn = false ;
    MouseDowned = false;
    MyOnwer = Owner;
    FPosition = 0;
    FMin = 0;
    FMax = 100;
    FStep = 1;
    ShiftX = 0;
    FAutoSize = false;
    Height = 30;
    Width = 30;
    FColor = clBtnFace;
    FImage = new Graphics::TBitmap() ;
    FPosImage = new  Graphics::TBitmap() ;
    FImagePlus = new Graphics::TBitmap() ;

    WindowProc = WndProc ;
}
//---------------------------------------------------------------------------
__fastcall TColorBar::~TColorBar()
{
    delete FPosImage;
    delete FImage;
    delete FImagePlus;
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::PaintWindow(HDC DC)
{
    Paint();
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::KeyDown(Word &Key, Classes::TShiftState Shift)
{
    switch( Key )
    {
    case VK_HOME :
        FPosition = FMin ;
        break ;
    case VK_END :
        FPosition = FMax ;
        break ;
    case VK_RETURN :
        FPosition = ( FMax + FMin ) / 2 ;
        break ;
    case VK_LEFT :
    case VK_UP :
        FPosition -= FStep ;
        break ;
    case VK_PRIOR :
        FPosition -= FStep * 5 ;
        break ;
    case VK_RIGHT :
    case VK_DOWN :
        FPosition += FStep ;
        break ;
    case VK_NEXT :
        FPosition += FStep * 5 ;
        break ;
    default :
        return ;
    }

    Key = 0 ;

    if( FPosition > FMax )
        FPosition = FMax ;
    if( FPosition < FMin )
        FPosition = FMin ;

    Paint() ;

    if(OnChange != NULL)
        OnChange(this) ;
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::Paint()
{
    ShiftX = FPosImage->Width / 2 ;
    NowRate= (float)(FImage->Width) / (float)(FMax - FMin) ;
    if (!FAutoSize)
        Canvas->StretchDraw(ClientRect, FImage);
    else
    {
        NowRate=(float)(FImage->Width)/(float)(FMax - FMin);

        PaintPointX = ((float)NowRate * (FPosition - FMin));
        Height = FPosImage->Height + FImage->Height ;
        Width = FImage->Width + FPosImage->Width ;
        if( FImagePlus->Empty )
            Canvas->Draw( ShiftX, 0, FImage ) ;
        else
        {
            TRect rect ;
            rect.Left = ShiftX ;
            rect.Top = 0 ;
            rect.Right = PaintPointX + ShiftX ;
            rect.Bottom = FImage->Height ;
            Canvas->StretchDraw(rect, FImage) ;
            rect.Left = PaintPointX + ShiftX ;
            rect.Right = ShiftX + FImage->Width ;
            Canvas->StretchDraw(rect, FImagePlus);
        }

        DrawPosPoint(PaintPointX, FImage->Height);
    }
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::DrawPosPoint(int TmpX,int TmpY)
{
    TColor TmpColor=Canvas->Brush->Color;

    TRect NowRect;
    Canvas->Brush->Color=Color;
    NowRect.Top=Image->Height;
    NowRect.Bottom=Height;
    NowRect.Left=0;
    NowRect.Right=Width;
    Canvas->FillRect(NowRect);
    Canvas->Brush->Color=TmpColor;

    Canvas->Draw(TmpX,TmpY,ImagePos);
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::FImageWrite(Graphics::TBitmap *tmp)
{
    FImage->Assign(tmp);
    Paint() ;
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::ChangeImagePlus(Graphics::TBitmap *tmp)
{
    FImagePlus->Assign(tmp);
    Paint() ;
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::PositionChange(int TmpInt)
{
    try
    {
        FPosition = TmpInt;
        if (TmpInt > FMax) FPosition = FMax;
        if (TmpInt < FMin) FPosition = FMin;
    }
    catch(...)
    {
        FPosition=0;
    }
    Paint();
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::ChangePosImage(Graphics::TBitmap *TmpPos)
{
    FPosImage->Assign(TmpPos);
    Height=Height+ImagePos->Height;
    Width=Width+ImagePos->Width;
    ShiftX=ImagePos->Width/2;
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::ChangeMax(int TmpMax)
{
    if (TmpMax < Min) FMax = Min ;
    else FMax = TmpMax ;
    if (FPosition > FMax) FPosition = FMax ;
    Paint() ;
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::ChangeMin(int TmpMin)
{
    if (TmpMin > Max) FMin = Max ;
    else  FMin = TmpMin ;
    if (FPosition < FMin) FPosition = FMin;
    Paint() ;
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::ChangeStep(int TmpStep)
{
    FStep = TmpStep;
    if (FStep<1) FStep = 1 ;
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::ChangeAutoSize(bool TmpStep)
{
    FAutoSize=TmpStep;
    Paint();
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::ChangeColor(TColor TmpColor)
{
    FColor=TmpColor;
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::WndProc(Messages::TMessage &Message)
{
    switch( Message.Msg )
    {
    case WM_CREATE :
        ::SetWindowLong( Handle, GWL_STYLE, 0x54010061 ) ;
        break ;
    case 0xBD00 :
        return ;
    default :
        break ;
    }
    TCustomControl::WndProc( Message ) ;
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::WMCaptureChanged(TMessage &msg)
{
    FbMouseIn = false ;
    MouseDowned = false ;
    Cursor = crArrow ;
    UpdateControlState() ;
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::MouseDown(TMouseButton Button,TShiftState Shift, int X, int Y)
{
    int iStX;
    int iEdX;
    if ( !Enabled )
    {
        MouseDowned = false;
        return;
    }

    SetFocus() ;

    if (MouseDowned)
        return;
    else
    {
        if( FPosImage->Empty )
        {

          Cursor = crHandPoint ;
            MouseDowned = true ;
        }
        else
        {
            iStX = PaintPointX ;
            iEdX = iStX + ImagePos->Width ;
            if ( (X<=iEdX) && (X>=iStX) && (Y<Height) && (Y> Height-ImagePos->Height) )
            {
                Cursor = crHandPoint ;
                MouseDowned = true ;
            }
            else
                MouseDowned = false ;
        }
    }
    UpdateControlState() ;
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::MouseUp(TMouseButton Button,TShiftState Shift, int X, int Y)
{
    MouseDowned = false ;
    ::ReleaseCapture() ;
    FbMouseIn = false ;
}
//---------------------------------------------------------------------------
void __fastcall TColorBar::MouseMove(TShiftState Shift, int X, int Y)
{
    POINT pt = { X, Y } ;
    TRect rect = BoundsRect ;
    rect.Right -= rect.Left ;
    rect.Left = 0 ;
    if( FbMouseIn && !::PtInRect( &rect, pt ) )
    {
        FbMouseIn = false ;
        if( !MouseDowned )
            ::ReleaseCapture() ;
    }
    if( !FbMouseIn && ::PtInRect( &rect, pt ) )
    {
        FbMouseIn = true ;
        ::SetCapture( Handle ) ;
    }
    if ( !Enabled )
    {
        MouseDowned = false;
        return;
    }
    if (!MouseDowned) return ;

    if( X < 0 )
        Position = FMin ;
    else if( X > Width )
        Position = FMax ;
    else
        Position = (X - FPosImage->Width / 2) / NowRate + FMin ;

    if(FOnChange != NULL)
        FOnChange( (TObject*)this ) ;
}
//---------------------------------------------------------------------------

namespace Colorbar
{
    void __fastcall PACKAGE Register()
    {
        TComponentClass classes[1] = {__classid(TColorBar)};
        RegisterComponents("Simon", classes, 0);
    }
}
//---------------------------------------------------------------------------
