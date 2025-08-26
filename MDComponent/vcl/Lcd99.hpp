// Borland C++ Builder
// Copyright (c) 1995, 2002 by Borland Software Corporation
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Lcd99.pas' rev: 6.00

#ifndef Lcd99HPP
#define Lcd99HPP

#pragma delphiheader begin
#pragma option push -w-
#pragma option push -Vx
#include <Menus.hpp>	// Pascal unit
#include <Dialogs.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <SysUtils.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <System.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Lcd99
{
//-- type declarations -------------------------------------------------------
typedef Shortint TLCDScale;

#pragma option push -b-
enum TLCDAnimation { anNone, anSpiral, anUp, anDown, anLeft, anRight, anRandom };
#pragma option pop

#pragma option push -b-
enum TLCDAbout { abNone, abAbout };
#pragma option pop

typedef void __fastcall (__closure *TLCDChangeProc)(System::TObject* Sender, AnsiString OldValue, AnsiString NewValue);

class DELPHICLASS TLCD99;
class PASCALIMPLEMENTATION TLCD99 : public Controls::TCustomControl 
{
	typedef Controls::TCustomControl inherited;
	
private:
	TLCDAbout fAbout;
	TLCDAnimation fAnimation;
	int fAnimationDelay;
	Graphics::TBitmap* fBufferBM;
	int fDigitNum;
	int fDigitSpacing;
	bool fDoBuffer;
	bool fDotDisplay;
	int fDotSpacing;
	bool fDoubleBuffer;
	int fGapX;
	int fGapY;
	bool fIsChanging;
	bool fIsPainting;
	Graphics::TColor fOffColor;
	AnsiString fOldValue;
	TLCDChangeProc fOnChange;
	Graphics::TColor fOnColor;
	unsigned fPaintDuration;
	bool fPreview;
	int fSegmentSize;
	AnsiString fValue;
	Graphics::TCanvas* fWorkCanvas;
	void __fastcall SetAnimation(TLCDAnimation Val);
	void __fastcall SetAnimationDelay(int Val);
	void __fastcall SetDigitNum(int Val);
	void __fastcall SetDigitSpacing(int Val);
	void __fastcall SetDotDisplay(bool Val);
	void __fastcall SetDotSpacing(int Val);
	void __fastcall SetDoubleBuffer(bool Val);
	void __fastcall SetGapX(int Val);
	void __fastcall SetGapY(int Val);
	void __fastcall SetOffColor(Graphics::TColor Val);
	void __fastcall SetOnColor(Graphics::TColor Val);
	void __fastcall SetSegmentSize(int Val);
	void __fastcall SetPreview(bool Val);
	void __fastcall SetValue(AnsiString Val);
	void __fastcall ShowAbout(TLCDAbout Val);
	HIDESBASE MESSAGE void __fastcall WMEraseBkgnd(Messages::TWMEraseBkgnd &Message);
	
protected:
	virtual void __fastcall Paint(void);
	
public:
	__fastcall virtual TLCD99(Classes::TComponent* AOwner);
	void __fastcall SetNumericValue(int NewValue);
	int __fastcall GetNumericValue(void);
	
__published:
	__property TLCDAbout About = {read=fAbout, write=ShowAbout, default=0};
	__property Align  = {default=0};
	__property TLCDAnimation Animation = {read=fAnimation, write=SetAnimation, default=0};
	__property int AnimationDelay = {read=fAnimationDelay, write=SetAnimationDelay, default=0};
	__property Color  = {default=-2147483643};
	__property int DigitNum = {read=fDigitNum, write=SetDigitNum, default=1};
	__property int DigitSpacing = {read=fDigitSpacing, write=SetDigitSpacing, default=5};
	__property bool DotDisplay = {read=fDotDisplay, write=SetDotDisplay, default=0};
	__property int DotSpacing = {read=fDotSpacing, write=SetDotSpacing, default=2};
	__property bool DoubleBuffer = {read=fDoubleBuffer, write=SetDoubleBuffer, default=1};
	__property DragCursor  = {default=-12};
	__property DragMode  = {default=0};
	__property Enabled  = {default=1};
	__property int GapX = {read=fGapX, write=SetGapX, default=2};
	__property int GapY = {read=fGapY, write=SetGapY, default=2};
	__property bool IsPainting = {read=fIsPainting, nodefault};
	__property Graphics::TColor OffColor = {read=fOffColor, write=SetOffColor, default=0};
	__property Graphics::TColor OnColor = {read=fOnColor, write=SetOnColor, default=65280};
	__property unsigned PaintDuration = {read=fPaintDuration, nodefault};
	__property ParentShowHint  = {default=1};
	__property PopupMenu ;
	__property bool Preview = {read=fPreview, write=SetPreview, default=0};
	__property int SegmentSize = {read=fSegmentSize, write=SetSegmentSize, default=2};
	__property ShowHint ;
	__property AnsiString Value = {read=fValue, write=SetValue};
	__property Visible  = {default=1};
	__property TLCDChangeProc OnChange = {read=fOnChange, write=fOnChange};
	__property OnClick ;
	__property OnDblClick ;
	__property OnDragDrop ;
	__property OnDragOver ;
	__property OnEndDrag ;
	__property OnMouseDown ;
	__property OnMouseMove ;
	__property OnMouseUp ;
public:
	#pragma option push -w-inl
	/* TCustomControl.Destroy */ inline __fastcall virtual ~TLCD99(void) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TWinControl.CreateParented */ inline __fastcall TLCD99(HWND ParentWindow) : Controls::TCustomControl(ParentWindow) { }
	#pragma option pop
	
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall Register(void);

}	/* namespace Lcd99 */
using namespace Lcd99;
#pragma option pop	// -w-
#pragma option pop	// -Vx

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Lcd99
