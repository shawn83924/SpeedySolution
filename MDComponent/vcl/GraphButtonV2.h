//---------------------------------------------------------------------------

#ifndef GraphButtonV2H
#define GraphButtonV2H
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include "GraphButton.h"
//---------------------------------------------------------------------------
class PACKAGE TGraphButtonV2 : public TCustomControl
{
private:
		int          FID;
		bool         FIsFocused;
		bool         CaptureFlag;   // mouse is over the button
		bool         FMouseDown;    // left button is pressed
		bool         SelectedFlag;
		String       CaptionText;
		CaptionAlign AlignType;
		TFontAttrib *FFontInfo;
		TNotifyEvent FOnMouseMoveIn;
		TNotifyEvent FOnMouseMoveOut;
		TPicture    *FNormalGraph;
		TPicture    *FHoverGraph;
		TPicture    *FDownGraph;
		TPicture    *FDisableGraph;
		TPicture    *FSelectedGraph;

		void __fastcall InitialGraph(void);
		void __fastcall SetGraph(TPicture* &target, TPicture* graph);
		void __fastcall SetNormalGraph (TPicture* graph);
		void __fastcall SetHoverGraph  (TPicture* graph);
		void __fastcall SetDownGraph   (TPicture* graph);
		void __fastcall SetDisableGraph(TPicture* graph);
		void __fastcall SetSelectedGraph(TPicture* graph);
		void __fastcall SetSelectFlag(bool Value);
		void __fastcall SetText(const String Value);
		void __fastcall SetAlignType(CaptionAlign Value);
		void __fastcall SetFontAttrib(TFontAttrib *Value);
		void __fastcall SetFocused(bool Value);
		void __fastcall GraphChanged(TObject* Sender);
		void __fastcall FontPropertyChange(TObject* Sender);
		void __fastcall PaintParent(void);
		void __fastcall DrawCaption(void);
		void __fastcall Paint(void);
		void __fastcall WMEraseBkgnd(TMessage &Msg);
		void __fastcall WMLosingCapture(TMessage &Msg);
		void __fastcall WndProc(TMessage &Msg);
		DYNAMIC void __fastcall MouseMove(TShiftState Shift, int X, int Y);
		DYNAMIC void __fastcall MouseDown(TMouseButton Button, TShiftState Shift, int X, int Y);
		DYNAMIC void __fastcall MouseUp  (TMouseButton Button, TShiftState Shift, int X, int Y);
		DYNAMIC void __fastcall KeyDown  (Word &Key, TShiftState Shift);
		DYNAMIC void __fastcall Click    (void);
		__property bool IsFocused = { read = FIsFocused, write = SetFocused };

protected:
public:
		__fastcall TGraphButtonV2(TComponent* Owner);
		__fastcall virtual ~TGraphButtonV2(void);

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
		__property int ButtonID = {read = FID, write = FID, index = -1};
		__property TPicture* NormalGraph    = { read = FNormalGraph,   write = SetNormalGraph  };
		__property TPicture* HoverGraph     = { read = FHoverGraph,    write = SetHoverGraph   };
		__property TPicture* DownGraph      = { read = FDownGraph,     write = SetDownGraph    };
		__property TPicture* DisableGraph   = { read = FDisableGraph,  write = SetDisableGraph };
		__property TPicture* SelectedGraph  = { read = FSelectedGraph, write = SetSelectedGraph};
		__property TFontAttrib* FontAttrib  = { read = FFontInfo,      write = SetFontAttrib   };
		__property bool Selected            = { read = SelectedFlag,   write = SetSelectFlag, default = false };
		__property CaptionAlign TextAlign   = { read = AlignType,      write = SetAlignType,  default = caCenter };
		__property String ButtonText        = { read = CaptionText,    write = SetText, nodefault };
		__property TNotifyEvent OnMouseMoveIn  = { read = FOnMouseMoveIn,  write = FOnMouseMoveIn,  default = NULL };
		__property TNotifyEvent OnMouseMoveOut = { read = FOnMouseMoveOut, write = FOnMouseMoveOut, default = NULL };
		__property bool IsMouseOver = { read = CaptureFlag };
};
//---------------------------------------------------------------------------
#endif
