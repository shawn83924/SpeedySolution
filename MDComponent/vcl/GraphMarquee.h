//---------------------------------------------------------------------------

#ifndef GraphMarqueeH
#define GraphMarqueeH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
//---------------------------------------------------------------------------
class PACKAGE TGraphMarquee : public TCustomControl
{
private:
    int     FScrollDelay;
    int     FScrollPixel;
    int     FPos;
    bool    FEnable;
    TTimer *FTimer;
    Graphics::TBitmap *FGraph;
    Graphics::TBitmap *FBKImage;    
    void __fastcall SetEnabled( bool Value );
    void __fastcall SetGraph( Graphics::TBitmap *Value );
    void __fastcall SetScrollDelay( int Value );
    void __fastcall OnTimerUpdate( TObject *Sender );
    void __fastcall WndProc( TMessage &Msg );
	void __fastcall Paint( void );
public:
    __fastcall TGraphMarquee(TComponent* Owner);
    __fastcall ~TGraphMarquee( void );
__published:
	__property Visible;
    __property Color;
    __property Align;    
    __property bool Enabled = {read = FEnable,write = SetEnabled };
    __property Graphics::TBitmap *Graph = {read=FGraph, write=SetGraph};
    __property int ScrollDelay = {read=FScrollDelay,write=SetScrollDelay};
    __property int ScrollPixel = {read=FScrollPixel,write=FScrollPixel};

};
//---------------------------------------------------------------------------
#endif
