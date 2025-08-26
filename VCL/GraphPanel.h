//---------------------------------------------------------------------------

#ifndef GraphPanelH
#define GraphPanelH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class PACKAGE TGraphPanel : public TCustomPanel
{
private:

    typedef TCustomPanel inherited;

    Graphics::TBitmap *FGlyph;
    bool FAutoSize;
    bool FStretchGlyph;
    bool FTransparent;
	TNotifyEvent       FOnPaint;
	HRGN FClipRgn;
    void __fastcall SetGlyph(Graphics::TBitmap *Value);
    void __fastcall SetAutoSize(bool Value);
    void __fastcall SetStretchGlyph(bool Value);
    void __fastcall SetTransparent(bool Value);
	void __fastcall SetClip( TCanvas* canvas );
	void __fastcall ResetClip( TCanvas* canvas );
protected:

  	virtual void __fastcall SetBounds(int ALeft, int ATop, int AWidth, int AHeight);
	virtual void __fastcall Paint(void);
    virtual void __fastcall DoGlyphChange(TObject* Sender);
    void __fastcall EraseBK( HDC hdc );
    virtual void __fastcall WndProc( TMessage &Msg );
public:

    __fastcall TGraphPanel(TComponent* Owner);
    __fastcall ~TGraphPanel();
    TCanvas* __fastcall GetCanvas( void );
__published:

__property Align ;
	__property Alignment ;
	__property Anchors ;
	__property BevelInner ;
	__property BevelOuter ;
	__property BevelWidth ;
	__property BiDiMode ;
	__property BorderWidth ;
	__property BorderStyle ;
	__property Caption ;
	__property Color ;
	__property Constraints ;
	__property Ctl3D ;
	__property UseDockManager ;
	__property DockSite ;
	__property DragCursor ;
	__property DragKind ;
	__property DragMode ;
	__property Enabled ;
	__property FullRepaint ;
	__property Font ;
	__property Locked ;
	__property ParentBiDiMode ;
	__property ParentColor ;
	__property ParentCtl3D ;
	__property ParentFont ;
	__property ParentShowHint ;
	__property PopupMenu ;
	__property ShowHint ;
	__property TabOrder ;
	__property TabStop ;
	__property Visible ;
	__property OnCanResize ;
	__property OnClick ;
	__property OnConstrainedResize ;
	__property OnContextPopup ;
	__property OnDockDrop ;
	__property OnDockOver ;
	__property OnDblClick ;
	__property OnDragDrop ;
	__property OnDragOver ;
	__property OnEndDock ;
	__property OnEndDrag ;
	__property OnEnter ;
	__property OnExit ;
	__property OnGetSiteInfo ;
	__property OnMouseDown ;
	__property OnMouseMove ;
	__property OnMouseUp ;
	__property OnResize ;
	__property OnStartDock ;
	__property OnStartDrag ;
	__property OnUnDock ;

    __property TNotifyEvent OnPaint = {read=FOnPaint,write=FOnPaint, default=NULL};
	__property Graphics::TBitmap* Glyph ={read=FGlyph, write=SetGlyph};
	__property bool AutoSize ={read = FAutoSize,write = SetAutoSize};
	__property bool StretchGlyph ={read = FStretchGlyph,write = SetStretchGlyph};
	__property bool Transparent ={read = FTransparent,write = SetTransparent};

};
//---------------------------------------------------------------------------
#endif
