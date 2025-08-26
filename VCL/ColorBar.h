//---------------------------------------------------------------------------
#ifndef ColorBarH
#define ColorBarH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
typedef struct{
    Cardinal Msg ;
    int MouseKey ;
    short X ;
    short Y ;
    int Result ;
} TMouseMessage ;

class PACKAGE TColorBar : public TCustomControl
{
private:
  TRect         RECTTmp;
  TComponent    *MyOnwer;
  bool          MouseDowned;
  bool          FbMouseIn ;

  Graphics::TBitmap *FImage;
  Graphics::TBitmap *FImagePlus;
  Graphics::TBitmap *FPosImage;

  int ShiftX;
  int PaintPointX;
  int FPosition;
  int FMax;
  int FMin;
  int FStep;
  bool FAutoSize;
  float NowRate;
  TColor FColor;
  TNotifyEvent FOnChange;
  void __fastcall FImageWrite(Graphics::TBitmap *);
  void __fastcall ChangePosImage(Graphics::TBitmap *);
  void __fastcall ChangeImagePlus(Graphics::TBitmap *);
  void __fastcall PositionChange(int TmpInt);
  void __fastcall ChangeMax(int TmpMax);
  void __fastcall ChangeMin(int TmpMin);
  void __fastcall ChangeStep(int TmpStep);
  void __fastcall ChangeAutoSize(bool TmpStep);
  void __fastcall ChangeColor(TColor);
  void __fastcall DrawPosPoint(int TmpX,int TmpY);
  void __fastcall Paint();
  void __fastcall PaintWindow(HDC);
  DYNAMIC void __fastcall MouseDown(TMouseButton Button,TShiftState Shift, int X, int Y);
  DYNAMIC void __fastcall MouseUp(TMouseButton Button,TShiftState Shift, int X, int Y);
  DYNAMIC void __fastcall MouseMove(TShiftState Shift, int X, int Y);
  DYNAMIC void __fastcall KeyDown(Word &Key, Classes::TShiftState Shift) ;

protected:
  void __fastcall WndProc(Messages::TMessage &Message);
  void __fastcall WMCaptureChanged(TMessage &msg) ;

public:
    __fastcall TColorBar(TComponent* Owner);
    __fastcall ~TColorBar();

__published:
    __property Graphics::TBitmap *Image   = {read = FImage,write = FImageWrite};
    __property Graphics::TBitmap *Image2  = {read = FImagePlus,write = ChangeImagePlus};
    __property Graphics::TBitmap *ImagePos= {read = FPosImage,write = ChangePosImage};
    __property int  Position ={read = FPosition,write =PositionChange};
    __property int  Max      ={read = FMax  ,write =ChangeMax};
    __property int  Min      ={read = FMin  ,write =ChangeMin};
    __property int  Step     ={read = FStep ,write =ChangeStep, default = 1};
    __property bool AutoSize ={read = FAutoSize, write=ChangeAutoSize};
    __property TNotifyEvent OnChange = {read =FOnChange,write=FOnChange};
    __property Enabled;
    __property ShowHint;
    __property Visible;
    __property Color;
    __property Cursor ;
    __property TabOrder;
    __property TabStop ;

BEGIN_MESSAGE_MAP
    MESSAGE_HANDLER( WM_CAPTURECHANGED,   TMessage, WMCaptureChanged )
END_MESSAGE_MAP( TCustomControl )

};
//---------------------------------------------------------------------------
#endif
