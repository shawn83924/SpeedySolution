// CodeGear C++Builder
// Copyright (c) 1995, 2008 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Btchkbox.pas' rev: 20.00

#ifndef BtchkboxHPP
#define BtchkboxHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <Sysinit.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <Sysutils.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit
#include <Dialogs.hpp>	// Pascal unit
#include <Stdctrls.hpp>	// Pascal unit
#include <Menus.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Btchkbox
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TBitCheckBox;
class PASCALIMPLEMENTATION TBitCheckBox : public Controls::TCustomControl
{
	typedef Controls::TCustomControl inherited;
	
private:
	Graphics::TBitmap* fCheckedBitmap;
	Graphics::TBitmap* fUncheckedBitmap;
	Graphics::TBitmap* fGrayedBitmap;
	Classes::TAlignment fAlignment;
	Stdctrls::TCheckBoxState fState;
	bool fAllowGrayed;
	int fCheckedNumGlyphs;
	int fGrayedNumGlyphs;
	int fUncheckedNumGlyphs;
	bool __fastcall GetChecked(void);
	void __fastcall SetCheckedNumGlyphs(int Value);
	void __fastcall SetGrayedNumGlyphs(int Value);
	void __fastcall SetUncheckedNumGlyphs(int Value);
	void __fastcall SetCheckedBitmap(Graphics::TBitmap* Value);
	void __fastcall SetGrayedBitmap(Graphics::TBitmap* Value);
	void __fastcall SetUncheckedBitmap(Graphics::TBitmap* Value);
	void __fastcall SetState(Stdctrls::TCheckBoxState Value);
	void __fastcall SetAlignment(Classes::TLeftRight Value);
	void __fastcall SetChecked(bool Value);
	HIDESBASE MESSAGE void __fastcall CMEnabledChanged(Messages::TMessage &Msg);
	HIDESBASE MESSAGE void __fastcall CMDialogChar(Messages::TWMKey &Msg);
	HIDESBASE MESSAGE void __fastcall CMFontChanged(Messages::TMessage &Msg);
	HIDESBASE MESSAGE void __fastcall CMTextChanged(Messages::TMessage &Msg);
	HIDESBASE MESSAGE void __fastcall CMSysColorChange(Messages::TMessage &Msg);
	HIDESBASE MESSAGE void __fastcall CMCtl3DChanged(Messages::TMessage &Msg);
	HIDESBASE MESSAGE void __fastcall WMSetFocus(Messages::TWMSetFocus &Message);
	HIDESBASE MESSAGE void __fastcall WMKillFocus(Messages::TWMKillFocus &Message);
	
protected:
	virtual void __fastcall Paint(void);
	DYNAMIC void __fastcall Click(void);
	DYNAMIC void __fastcall KeyPress(System::WideChar &Key);
	
public:
	__fastcall virtual TBitCheckBox(Classes::TComponent* AOwner);
	__fastcall virtual ~TBitCheckBox(void);
	virtual void __fastcall Toggle(void);
	
__published:
	__property Classes::TLeftRight Alignment = {read=fAlignment, write=SetAlignment, default=1};
	__property bool AllowGrayed = {read=fAllowGrayed, write=fAllowGrayed, default=0};
	__property Caption;
	__property bool Checked = {read=GetChecked, write=SetChecked, default=0};
	__property Graphics::TBitmap* CheckedGlyph = {read=fCheckedBitmap, write=SetCheckedBitmap};
	__property int CheckedNumGlyphs = {read=fCheckedNumGlyphs, write=SetCheckedNumGlyphs, default=1};
	__property Color = {default=-16777211};
	__property Ctl3D;
	__property DragCursor = {default=-12};
	__property DragMode = {default=0};
	__property Enabled = {default=1};
	__property Font;
	__property Graphics::TBitmap* GrayedGlyph = {read=fGrayedBitmap, write=SetGrayedBitmap};
	__property int GrayedNumGlyphs = {read=fGrayedNumGlyphs, write=SetGrayedNumGlyphs, default=1};
	__property ParentColor = {default=1};
	__property ParentCtl3D = {default=1};
	__property ParentFont = {default=1};
	__property ParentShowHint = {default=1};
	__property PopupMenu;
	__property ShowHint;
	__property Stdctrls::TCheckBoxState State = {read=fState, write=SetState, default=0};
	__property TabOrder = {default=-1};
	__property TabStop = {default=0};
	__property Graphics::TBitmap* UncheckedGlyph = {read=fUncheckedBitmap, write=SetUncheckedBitmap};
	__property int UncheckedNumGlyphs = {read=fUncheckedNumGlyphs, write=SetUncheckedNumGlyphs, default=1};
	__property Visible = {default=1};
	__property OnClick;
	__property OnDragDrop;
	__property OnDragOver;
	__property OnEndDrag;
	__property OnEnter;
	__property OnExit;
	__property OnKeyDown;
	__property OnKeyPress;
	__property OnKeyUp;
	__property OnMouseDown;
	__property OnMouseMove;
	__property OnMouseUp;
	__property OnStartDrag;
public:
	/* TWinControl.CreateParented */ inline __fastcall TBitCheckBox(HWND ParentWindow) : Controls::TCustomControl(ParentWindow) { }
	
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall Register(void);

}	/* namespace Btchkbox */
using namespace Btchkbox;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// BtchkboxHPP
