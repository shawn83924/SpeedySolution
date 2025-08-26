//---------------------------------------------------------------------------
// GraphButton Version 1.2
//---------------------------------------------------------------------------
// Coded by Li-Yuan Chang 1998/9/3
// Copyright WayTech Development Inc.
//---------------------------------------------------------------------------
#ifndef GraphButtonH
#define GraphButtonH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
//---------------------------------------------------------------------------
// Enum types
//---------------------------------------------------------------------------
typedef enum { bsNormalState   = 1 ,
               bsMouseOver     = 2 ,
               bsMouseDown     = 3 ,
               bsDisableState  = 4 ,
               bsSelectedState = 5  } ButtonState;
//---------------------------------------------------------------------------
typedef enum { bsHotTrack,
               bsWindows  }  ButtonStyle;
//---------------------------------------------------------------------------
typedef enum { soNone        = 0,
               soOnePixel    = 1,
               soTwoPixels   = 2,
               soThreePixels = 3,
               soFourPixels  = 4,
               soFivePixels  = 5 } TShadowOffset;
//---------------------------------------------------------------------------
typedef enum { caCenter,
               caLeft,
               caRight } CaptionAlign;
//---------------------------------------------------------------------------
class PACKAGE TFontAttrib : public TPersistent
{
 private:
        TColor HColor;
        TColor HSColor;
        TColor NSColor;
		TColor NColor;
		TColor SColor;
        TFont *FCaptionFont;
        TNotifyEvent FOnChange;
        TShadowOffset FOffset;
        void __fastcall FontChanged( TObject *Sender );
        void __fastcall SetFont( TFont *Value );
		void __fastcall SetFontColor( TColor Value );
		void __fastcall SetSelectedFontColor( TColor Value );
		void __fastcall SetFontShadowColor( TColor Value );
		void __fastcall SetFontHighlightColor( TColor Value );
        void __fastcall SetFontHighlightShadowColor( TColor Value );
        void __fastcall SetShadowOffset( TShadowOffset Value );
 public:
        __fastcall TFontAttrib( void );
        __fastcall virtual ~TFontAttrib( void );
 __published:
        __property TNotifyEvent  OnChange             = {read = FOnChange, write = FOnChange};
        __property TColor        FontColor            = {read = NColor, write = SetFontColor};
        __property TColor        FontShadowColor      = {read = NSColor, write = SetFontShadowColor};
        __property TColor        HighlightFontColor   = {read = HColor, write = SetFontHighlightColor};
		__property TColor        HighlightShadowColor = {read = HSColor, write = SetFontHighlightShadowColor};
		__property TColor        SelectedFontColor    = {read = SColor, write = SetSelectedFontColor};
        __property TFont*        CaptionFont          = {read = FCaptionFont, write = SetFont};
        __property TShadowOffset ShadowOffsetPixels   = {read = FOffset, write = SetShadowOffset};
};

//---------------------------------------------------------------------------
class PACKAGE TGraphButton : public TCustomControl
{
private:
        int             FID;
        int             FGraphCount;
        int             FImageIndex;
        bool            FTransparent;
        bool            FIsFocused;
        bool            CaptureFlag;
        bool            FFitFlag;
        bool            SelectedFlag;
        String          CaptionText;
        TImageList     *FImageList;
        CaptionAlign    AlignType;
        ButtonStyle     FBtnStyle;
		TFontAttrib    *FFontInfo;
        TNotifyEvent    FOnMouseMoveIn;
        TNotifyEvent    FOnMouseMoveOut;
        TWinControl    *ParentWin;
		Graphics::TBitmap *FButtonGraph;
        void __fastcall SetBtnStyle( ButtonStyle Vilue );
        void __fastcall SetButtonGraph(Graphics::TBitmap *Value);
        void __fastcall SetSelectFlag( bool Value);
        void __fastcall SetTransparent( bool Value);
        void __fastcall SetText( const String Value );
        void __fastcall SetAlignType(CaptionAlign Value);
        void __fastcall SetGraphCount(int Value);
        void __fastcall SetFontAttrib(TFontAttrib *Value);
        void __fastcall SetImageList(TImageList *Value);
        void __fastcall SetImageIndex( int Value );
        void __fastcall SetFitFlag( bool Value );
        void __fastcall WMLosingCapture(TMessage &Msg);
        void __fastcall WMEraseBkgnd( TMessage &Msg );
        void __fastcall PaintButton( TColor Front,TColor Back,ButtonState State );
        void __fastcall FontPropertyChange(TObject* Sender);
        void __fastcall SetFocused( bool Value );
        int  __fastcall GetGraphWidth( void );
        int  __fastcall GetGraphHeight( void );
        void __fastcall PaintParent( void );
        void __fastcall GraphChanged( TObject* Sender );
        void __fastcall Paint( void );
        void __fastcall Loaded( void );
		void __fastcall WndProc( TMessage &Msg );
        DYNAMIC void __fastcall MouseMove( TShiftState Shift, int X, int Y);
        DYNAMIC void __fastcall MouseDown(TMouseButton Button,TShiftState Shift, int X, int Y);
        DYNAMIC void __fastcall MouseUp(TMouseButton Button,TShiftState Shift, int X, int Y);
        DYNAMIC void __fastcall KeyDown(Word &Key,TShiftState Shift);
        DYNAMIC void __fastcall Click( void );
        __property int GraphWidth  = { read = GetGraphWidth  };
        __property int GraphHeight = { read = GetGraphHeight };
        __property bool IsFocused = { read = FIsFocused,
                                      write = SetFocused };
protected:
public:
        __fastcall TGraphButton(TComponent* Owner);
        __fastcall virtual ~TGraphButton( void );
__published:
        __property TabStop;
        __property TabOrder;
        __property Visible;
        __property ShowHint;
        __property DragMode;
        __property DragCursor;
        __property OnDragDrop;
        __property OnDragOver;
        __property OnMouseDown;
        __property OnMouseUp;
        __property OnKeyDown;
        __property OnKeyUp;
        __property OnClick;
        __property Enabled;
        __property Anchors;
        __property bool Transparent
                   ={read = FTransparent,write = SetTransparent,default = false};
        __property ButtonStyle BtnStyle
                   ={read = FBtnStyle, write = SetBtnStyle,default = bsHotTrack};
        __property int GraphCount
                   ={read = FGraphCount, write = SetGraphCount,default = 0};
        __property int ButtonID
				   ={read = FID, write = FID, index = -1};
		__property Graphics::TBitmap* ButtonGraph
				   ={read=FButtonGraph, write=SetButtonGraph};
		__property TFontAttrib* FontAttrib
				   ={read = FFontInfo, write = SetFontAttrib };
		__property bool Selected
				   ={read = SelectedFlag, write=SetSelectFlag,default = false };
        __property bool SizeFitBmp
                   ={read = FFitFlag, write = SetFitFlag, default = false };
        __property CaptionAlign TextAlign
                   ={read = AlignType, write = SetAlignType,default = caCenter };
        __property String ButtonText
                   ={read = CaptionText, write = SetText, nodefault };
        __property TNotifyEvent OnMouseMoveIn
                   = {read = FOnMouseMoveIn,write = FOnMouseMoveIn,default = NULL};
        __property TNotifyEvent OnMouseMoveOut
                   = {read = FOnMouseMoveOut,write = FOnMouseMoveOut,default = NULL};
        __property bool IsMouseOver
                   = {read = CaptureFlag };
        __property TImageList *ImageList
                   = {read = FImageList,write = SetImageList,default = NULL};
        __property int ImageIndex
                   = {read = FImageIndex,write = SetImageIndex,default = -1};
};
//---------------------------------------------------------------------------
#endif
