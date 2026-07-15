//---------------------------------------------------------------------------
// GraphButton Version 1.5
//---------------------------------------------------------------------------
// Coded by Li-Yuan Chang 1999/9/3
// Add Transparent property
//---------------------------------------------------------------------------
// GraphButton Version 1.2
//---------------------------------------------------------------------------
// Coded by Li-Yuan Chang 1998/9/3
// Copyright WayTech Development Inc.
//---------------------------------------------------------------------------
//
//  The ButtonGraph bitmap should be layout as fellow...
//        -----------------------
//       |     Normal Graph      |
//        -----------------------
//       |   MouseOver Graph     |
//        -----------------------
//       |    MouseDown Graph    |
//        -----------------------
//       |  Button disable Graph |
//        -----------------------
//       | Button selected Graph |
//        -----------------------
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "GraphButton.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//---------------------------------------------------------------------------
static inline void ValidCtrCheck( TGraphButton *)
{
    new TGraphButton(NULL);
}
//---------------------------------------------------------------------------
//static inline void ValidCtrCheck( TFontAttrib *)
//{
//    new TFontAttrib( );
//}
//---------------------------------------------------------------------------
// Functions for TFontAttrib
//---------------------------------------------------------------------------
__fastcall TFontAttrib::TFontAttrib( void )
:TPersistent(  )
{
    FCaptionFont = new TFont();
    FCaptionFont->OnChange = FontChanged;
    FOnChange = NULL;
    DColor = clGrayText;
}
//---------------------------------------------------------------------------
__fastcall TFontAttrib::~TFontAttrib( void )
{
    delete FCaptionFont;
}
//---------------------------------------------------------------------------
void __fastcall TFontAttrib::FontChanged( TObject * )
{
    if( FOnChange != NULL ) FOnChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TFontAttrib::SetFont(TFont *Value)
{
    NColor = Value->Color;
    FCaptionFont->Assign( Value );
}
//---------------------------------------------------------------------------
void __fastcall TFontAttrib::SetShadowOffset( TShadowOffset Value )
{
    FOffset = Value;
    if( FOnChange != NULL ) FOnChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TFontAttrib::SetFontColor( TColor Value )
{
    NColor = Value;
    FCaptionFont->Color = NColor;
}
//---------------------------------------------------------------------------
void __fastcall TFontAttrib::SetSelectedFontColor( TColor Value )
{
	SColor = Value;
	if( FOnChange != NULL ) FOnChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TFontAttrib::SetFontShadowColor( TColor Value )
{
	NSColor = Value;
	if( FOnChange != NULL ) FOnChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TFontAttrib::SetFontHighlightColor( TColor Value )
{
    HColor = Value;
    if( FOnChange != NULL ) FOnChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TFontAttrib::SetFontHighlightShadowColor( TColor Value )
{
    HSColor = Value;
    if( FOnChange != NULL ) FOnChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TFontAttrib::SetDisabledFontColor( TColor Value )
{
	DColor = Value;
	if( FOnChange != NULL ) FOnChange(this);
}

//---------------------------------------------------------------------------
// Functions for TGraphButton
//---------------------------------------------------------------------------
// Constractor
//---------------------------------------------------------------------------
__fastcall TGraphButton::TGraphButton(TComponent* Owner)
    : TCustomControl(Owner),CaptureFlag( false ),
      FImageList( NULL ),FImageIndex( -1 )
{
    ParentWin = dynamic_cast<TWinControl*>(Owner);
    FFontInfo    = new TFontAttrib( );
    FFontInfo->OnChange = FontPropertyChange;
    FButtonGraph = new Graphics::TBitmap();
    FButtonGraph->OnChange = GraphChanged;
    Width  = 32;
    Height = 32;
}
//---------------------------------------------------------------------------
//  Destructor
//---------------------------------------------------------------------------
__fastcall TGraphButton::~TGraphButton( void )
{
    if( FButtonGraph != NULL )
    {
        delete FButtonGraph;
        FButtonGraph = NULL;
    }
    if( FFontInfo != NULL )
    {
        delete FFontInfo;
        FFontInfo = NULL;
    }
}
//---------------------------------------------------------------------------
//  Print parent window into this button as background
//---------------------------------------------------------------------------
void __fastcall TGraphButton::PaintParent( void )
{
     int   SaveIndex;
     HDC   DC;

     if( HandleAllocated( ) )
     {
         DC = GetDC( Handle );
         if( DC == NULL ) return;
         SaveIndex = SaveDC( DC );
           SetViewportOrgEx( DC, -Left, -Top, NULL );
           Parent->Perform ( WM_ERASEBKGND, (int)DC, 0 );
          // Parent->Perform ( WM_PAINT, (int)DC, 0 );
         RestoreDC( DC, SaveIndex );
         ReleaseDC( Handle, DC );
     }
}
//---------------------------------------------------------------------------
//  When the font setting changed repaint it
//---------------------------------------------------------------------------
void __fastcall TGraphButton::FontPropertyChange(TObject* )
{
    Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TGraphButton::SetButtonGraph(Graphics::TBitmap *Value)
{
    FButtonGraph->Assign( Value );
    FButtonGraph->Dormant();
    if( FGraphCount == 0 )FGraphCount = 1;
    FontPropertyChange( this );
}
//---------------------------------------------------------------------------
void __fastcall TGraphButton::SetTransparent( bool Value)
{
    FTransparent = Value;
    FontPropertyChange( this );
}
//---------------------------------------------------------------------------
void __fastcall TGraphButton::SetFontAttrib( TFontAttrib *Value )
{
    FFontInfo->Assign( Value );
    FontPropertyChange( this );
}
//---------------------------------------------------------------------------
void __fastcall TGraphButton::SetAlignType( CaptionAlign Value )
{
    AlignType = Value;
    FontPropertyChange( this );
}
//---------------------------------------------------------------------------
void __fastcall TGraphButton::SetSelectFlag( bool Value )
{
    SelectedFlag = Value;
    FontPropertyChange( this );
}
//---------------------------------------------------------------------------
void __fastcall TGraphButton::SetBtnStyle( ButtonStyle Value )
{
    FBtnStyle = Value;
    FontPropertyChange( this );
}
//---------------------------------------------------------------------------
void __fastcall TGraphButton::SetText( const String Value )
{
    CaptionText = Value;
    FontPropertyChange( this );
}
//---------------------------------------------------------------------------
void __fastcall TGraphButton::SetGraphCount( int Value )
{
    FGraphCount = Value;
    FontPropertyChange( this );
}
//---------------------------------------------------------------------------
void __fastcall TGraphButton::SetImageList( TImageList *Value )
{
    if( Value != NULL )
    {
        FImageList = Value;
        Paint();
    }
}
//---------------------------------------------------------------------------
void __fastcall TGraphButton::SetImageIndex( int Value )
{
    FImageIndex = Value;
    Paint();
}
//---------------------------------------------------------------------------
void __fastcall TGraphButton::SetFitFlag( bool Value )
{
    FFitFlag = Value;
    if( FFitFlag )
        SetBounds(Left,Top,GraphWidth,GraphHeight);
}
//---------------------------------------------------------------------------
int __fastcall TGraphButton::GetGraphWidth( void )
{
    if( FImageList != NULL && FImageIndex != -1 )
        return FImageList->Width;
    else
        return FButtonGraph->Width;
}
//---------------------------------------------------------------------------
int __fastcall TGraphButton::GetGraphHeight( void )
{
    if( FGraphCount != 0 )
    {
        if( FImageList != NULL && FImageIndex != -1 )
            return (int)( FImageList->Height/FGraphCount );
        else
            return (int)( FButtonGraph->Height/FGraphCount );
    }
    else
        return Height;
}
//---------------------------------------------------------------------------
void __fastcall TGraphButton::Click( void )
{
    if( CaptureFlag )
    {
        ReleaseCapture();
        CaptureFlag = false;
        Paint();
    }
    TCustomControl::Click();
}

//---------------------------------------------------------------------------
// Overridding the KeyDown virtual function to handle the KeyDown event
//---------------------------------------------------------------------------
void __fastcall TGraphButton::KeyDown( Word &Key,TShiftState Shift)
{
      if( Key == VK_RETURN ) Click();
      TCustomControl::KeyDown( Key, Shift );
}
//---------------------------------------------------------------------------
// Overridding the MouseMove virtual function to handle the mousemove event
//---------------------------------------------------------------------------
void __fastcall TGraphButton::MouseMove( TShiftState Shift, int X, int Y)
{
    if( !CaptureFlag )
    {
        if( HandleAllocated() )
            SetCapture( Handle );
	CaptureFlag = true;
        Paint();
        if( FOnMouseMoveIn != NULL )
            FOnMouseMoveIn( this );
    }
    if(( X < 0 )||( Y < 0 )||( X > Width )||( Y > Height ))
    {
        ReleaseCapture();
     	CaptureFlag = false;
        Paint();
    }
    else
    {
        TCustomControl::MouseMove( Shift, X, Y);
    }
}
//---------------------------------------------------------------------------
// Overridding the MouseDown virtual function to handle the mousedown event
//---------------------------------------------------------------------------
void __fastcall TGraphButton::MouseDown(TMouseButton Button,TShiftState Shift, int X, int Y)
{
    if( Button == mbLeft )
        PaintButton( FFontInfo->FontColor,FFontInfo->FontShadowColor,bsMouseDown );
    SetFocus();
    TCustomControl::MouseDown( Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
// Overridding the MouseUp virtual function to handle the mouseup event
//---------------------------------------------------------------------------
void __fastcall TGraphButton::MouseUp(TMouseButton Button,TShiftState Shift, int X, int Y)
{
    if(Button == mbLeft ) Paint();
    TCustomControl::MouseUp( Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TGraphButton::GraphChanged( TObject*  )
{
     // If FitFlag = true, Fit the button size to the bitmap size
     if( FFitFlag )
     {
         if( Width != GraphWidth || Height!= GraphHeight )
            SetBounds( Left, Top, GraphWidth, GraphHeight );
     }
     // if property Transparent = true set the bitmap transparent property to true
     if( FTransparent && FButtonGraph != NULL )
     {
        if( FButtonGraph->Transparent == false )
        {
            FButtonGraph->Transparent = true;
            FButtonGraph->TransparentMode = tmAuto;
        }
     }
}
//---------------------------------------------------------------------------
void __fastcall TGraphButton::Loaded( void )
{
     TCustomControl::Loaded();
     if( FFitFlag )
     {
         if( Width != GraphWidth || Height!= GraphHeight )
            SetBounds( Left, Top, GraphWidth, GraphHeight );
     }
}
//---------------------------------------------------------------------------
void __fastcall TGraphButton::PaintButton(  TColor Front, TColor Back,
                                            ButtonState State )
{
     register int OffsetX = 0,OffsetY;
     register int TextHPixels,TextVPixels;
     TCanvas *canvas;

     if( HandleAllocated() )
     {
         canvas = new TCanvas();
         canvas->Handle = GetDC( Handle );
         // Set the Brush & Font property of the current DC
         canvas->Brush->Style = bsClear;
		 canvas->Font->Assign( FFontInfo->CaptionFont );
         TextVPixels = canvas->TextHeight( CaptionText );
         if( TextVPixels > Height )
             OffsetY = 0;
         else
             OffsetY = (Height - TextVPixels)/2;
         TextHPixels = canvas->TextWidth( CaptionText );
         switch( AlignType )
         {
             case caCenter: if( TextHPixels > Width ) OffsetX = 0;
                            else OffsetX = (Width - TextHPixels)/2;
                            break;
             case caRight:  OffsetX = Width - TextHPixels;
                            break;
         }
         // Paint the background
         if( FImageList != NULL && FImageIndex > -1 && FImageIndex < FImageList->Count )
         {
			 // Paint the background using the image from ImageList
             FImageList->Draw( canvas ,0 ,-((int)(State-1)*GraphHeight), FImageIndex ,true );
         }
         else if( !FButtonGraph->Empty )
         {
             // Paint the background using the Bitmap
             canvas->Draw( 0,-((int)(State-1)*GraphHeight), FButtonGraph );
         }
         else if( !FTransparent )
         {
             // Paint the default button background
             canvas->Pen->Color = clSilver;
             canvas->Brush->Color = clSilver;
             canvas->Rectangle( 2,2,Width-2,Height-2 );
             TRect Bounds = Rect(0,0,Width,Height);
             if( State == bsMouseDown )
                 Frame3D( canvas, Bounds, clGray, clWhite, 2 );
             else
                 Frame3D( canvas, Bounds, clWhite, clGray, 2 );
         }
         // Paint the text
         if( FFontInfo->ShadowOffsetPixels != ::soNone )
         {
             canvas->Font->Color = Back;
             canvas->TextOut( OffsetX + (int)FFontInfo->ShadowOffsetPixels
                             ,OffsetY + (int)FFontInfo->ShadowOffsetPixels,CaptionText);
         }
         canvas->Font->Color = Front;
         canvas->TextOut( OffsetX, OffsetY, CaptionText );
         ReleaseDC( Handle, canvas->Handle );
         delete canvas;
     }
}
//---------------------------------------------------------------------------
//  Overridded the Paint virtual function to do the custom paint
//---------------------------------------------------------------------------
void __fastcall TGraphButton::Paint( void )
{
     if( FGraphCount == 0 ) // No graph
     {
         if( !Enabled )
			  PaintButton( FFontInfo->FontColor,FFontInfo->FontShadowColor,bsDisableState );
         else if( SelectedFlag || CaptureFlag )
              PaintButton( FFontInfo->SelectedFontColor,FFontInfo->HighlightShadowColor,bsSelectedState );
         else
              PaintButton( FFontInfo->FontColor,FFontInfo->FontShadowColor,bsNormalState );
         return;
     }
     if( BtnStyle == bsWindows ) //Is a Window style button (can get focus and tab can stop )
     {
         if( !Enabled && FGraphCount >= bsDisableState )
              PaintButton( FFontInfo->HighlightFontColor,FFontInfo->HighlightShadowColor,bsDisableState );
         else if( SelectedFlag && FGraphCount >= bsSelectedState )
			  PaintButton( FFontInfo->SelectedFontColor,FFontInfo->HighlightShadowColor,bsSelectedState );
         else if( IsFocused && FGraphCount >= bsMouseOver )
              PaintButton( FFontInfo->HighlightFontColor,FFontInfo->HighlightShadowColor,bsMouseOver );
         else if( FGraphCount >= bsNormalState )
              PaintButton( FFontInfo->FontColor,FFontInfo->FontShadowColor,bsNormalState );
     }
     else                       // A hotspot style button.
     {
         if( !Enabled && FGraphCount >= bsDisableState )
              PaintButton( FFontInfo->HighlightFontColor,FFontInfo->HighlightShadowColor,bsDisableState );
         else if( SelectedFlag && FGraphCount >= bsSelectedState )
			  PaintButton( FFontInfo->SelectedFontColor,FFontInfo->HighlightShadowColor,bsSelectedState );
         else if( CaptureFlag && FGraphCount >= bsMouseOver )
              PaintButton( FFontInfo->HighlightFontColor,FFontInfo->HighlightShadowColor,bsMouseOver );
         else if( FGraphCount >= bsNormalState )
              PaintButton( FFontInfo->FontColor,FFontInfo->FontShadowColor,bsNormalState );
     }
}
//---------------------------------------------------------------------------
//   When losing mouse capture, Trigger the MouseMoveOut event and repaint it
//---------------------------------------------------------------------------
void __fastcall TGraphButton::WMLosingCapture (TMessage &)
{
     CaptureFlag = false;
     Paint();
     if( FOnMouseMoveOut != NULL )
         FOnMouseMoveOut( this );
}
//---------------------------------------------------------------------------
//   Paint the button background when it received WM_ERASEBKGND
//---------------------------------------------------------------------------
void __fastcall TGraphButton::WMEraseBkgnd( TMessage &Msg )
{
     if( ( FTransparent || FGraphCount == 0 ) )
         PaintParent( );
     Paint();
     Msg.Result = 1;
}
//---------------------------------------------------------------------------
//   Let this button get the focus when it received WM_SETFOCUS
//---------------------------------------------------------------------------
void __fastcall TGraphButton::SetFocused( bool Value )
{
    if( Value != FIsFocused )
    {
        FIsFocused = Value;
        Paint();
    }
}
//---------------------------------------------------------------------------
// Overridden the WndProc
//---------------------------------------------------------------------------
void __fastcall TGraphButton::WndProc( TMessage &Msg )
{
    switch( Msg.Msg )
    {
        case WM_SETFOCUS:       Msg.Result = 0;
                                IsFocused = true;
                                TCustomControl::WndProc(Msg);
                                break;
        case WM_KILLFOCUS:      Msg.Result = 0;
                                IsFocused = false;
                                TCustomControl::WndProc(Msg);
                                break;
        case WM_CAPTURECHANGED: WMLosingCapture( Msg );
                                break;
        case WM_ERASEBKGND:     WMEraseBkgnd( Msg );
                                break;
        default:                TCustomControl::WndProc( Msg );
                                break;
    }
}
//---------------------------------------------------------------------------
//  register this component
//---------------------------------------------------------------------------
namespace Graphbutton
{
    void __fastcall PACKAGE Register()
    {
        TComponentClass classes[1] = {__classid(TGraphButton)};
        RegisterComponents("MD", classes, 0);
    }
}
//---------------------------------------------------------------------------
//   End of file
//---------------------------------------------------------------------------
