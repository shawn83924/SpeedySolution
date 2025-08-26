//---------------------------------------------------------------------------

#ifndef GraphTrackBarH
#define GraphTrackBarH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
//---------------------------------------------------------------------------
class PACKAGE TGraphTrackBar : public TCustomControl
{
private:
    int FMin;
    int FMax;
    int FPos;
    bool FMouseDown;
    Graphics::TBitmap *FFTImage;
    Graphics::TBitmap *FBKImage;
    Graphics::TBitmap *FThumbImage;
    TNotifyEvent       FOnPosChange;
    int __fastcall ToPosition( int X );
private:
    void __fastcall SetPos( int Value );
    void __fastcall WndProc( TMessage &Msg );
	void __fastcall Paint( void );
    void __fastcall SetFront( Graphics::TBitmap *Value );
    void __fastcall SetThumb( Graphics::TBitmap *Value );
    void __fastcall SetBackground( Graphics::TBitmap *Value );
    DYNAMIC void __fastcall MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
    DYNAMIC void __fastcall MouseUp(TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
    DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
public:
    __fastcall TGraphTrackBar(TComponent* Owner);
    __fastcall ~TGraphTrackBar( );
__published:
   __property Graphics::TBitmap *Thumb      = {read=FThumbImage, write=SetThumb};
   __property Graphics::TBitmap *Front      = {read=FFTImage,    write=SetFront};
   __property Graphics::TBitmap *Beakground = {read=FBKImage,    write=SetBackground};
   __property int Min = {read=FMin,write=FMin};
   __property int Max = {read=FMax,write=FMax};
   __property int Position = {read=FPos,write=SetPos};
    __property TNotifyEvent OnPositionChange = {read=FOnPosChange,write=FOnPosChange, default=NULL};
};
//---------------------------------------------------------------------------
#endif
