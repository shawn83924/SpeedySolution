//---------------------------------------------------------------------------

#ifndef MarqueeH
#define MarqueeH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class PACKAGE TMarquee : public TCustomControl
{
public:

    enum TScrollType { stHorizontal, stVertical };
    enum TTextAlign { taTopLeft, taCenter, taBottomRight };

private:

    typedef TCustomControl inherited;

    bool FTransparent;
    int FScrollDelay;
    int FScrollPixel;
    TScrollType FScrollType;
    int FRepeatCount;
    AnsiString FText;
    TTextAlign FTextAlign;
    TTimer *FTimer;
    int CurrentPos;
    int CurrentRepeat;
    TSize TextExtent;
    Graphics::TBitmap *FBKImg;
    void __fastcall SetTransparent(bool Value);
    void __fastcall SetScrollDelay(int Value);
    void __fastcall SetScrollType(TScrollType Value);
    void __fastcall SetRepeatCount(int Value);
    void __fastcall SetText(AnsiString Value);
    void __fastcall SetTextAlign(TTextAlign Value);
    void __fastcall SetEnabled(bool Value);
    void __fastcall SetFont(TFont *Value);
    TFont * __fastcall GetFont();
    bool __fastcall GetEnabled();
    void __fastcall OnTimerUpdate(TObject *Sender);
	void __fastcall Paint(void);
    void __fastcall EraseBK( HDC hdc );
    void __fastcall WndProc( TMessage &Msg );
    void __fastcall Loaded();

public:

    __fastcall TMarquee(TComponent* Owner);
    void __fastcall Restart();

__published:

    __property Color;
	__property Visible;
	__property OnClick ;
	__property OnDblClick ;
	__property OnDragDrop ;
	__property OnDragOver ;
	__property OnEnter ;
	__property OnExit ;
	__property OnMouseDown ;
	__property OnMouseMove ;
	__property OnMouseUp ;
	__property OnResize ;

    __property TFont *Font = {read=GetFont,write=SetFont};
    __property bool Transparent = {read=FTransparent,write=SetTransparent};
    __property int ScrollDelay = {read=FScrollDelay,write=SetScrollDelay};
    __property int ScrollPixel = {read=FScrollPixel,write=FScrollPixel};
    __property TScrollType ScrollType = {read=FScrollType,write=SetScrollType};
    __property int RepeatCount = {read=FRepeatCount,write=SetRepeatCount};
    __property AnsiString Text = {read=FText,write=SetText};
    __property TTextAlign TextAlign = {read=FTextAlign,write=SetTextAlign};
    __property bool Enabled = {read=GetEnabled,write=SetEnabled};

};
//---------------------------------------------------------------------------
#endif
