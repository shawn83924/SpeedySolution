//---------------------------------------------------------------------------

#ifndef RoundFormExH
#define RoundFormExH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include "RoundFormExRes.h"
//---------------------------------------------------------------------------
const unsigned int HTLOCKBTN = 22;
const unsigned int HTWINICON = 23;
//---------------------------------------------------------------------------
class SuppressStyle
{
private:
	DWORD FStyle;
	DWORD FOldStyle;
	HWND  FhWnd;
public:
	SuppressStyle(HWND hWnd, DWORD style)
	:FhWnd(hWnd), FStyle(style), FOldStyle(0)
	{
		FOldStyle = ::GetWindowLong( FhWnd, GWL_STYLE);
		if( FOldStyle & FStyle)
			SetWindowLong( FhWnd, GWL_STYLE, FOldStyle & ~FStyle);
	}
	~SuppressStyle()
	{
		if( FOldStyle & FStyle)
			SetWindowLong( FhWnd, GWL_STYLE, FOldStyle);
	}
};
//---------------------------------------------------------------------------
class PACKAGE TRoundFormEx : public TComponent
{
private:
	TWinControl*    FParent;
	TForm*          FOwnerForm;
	TNotifyEvent    FOldFormResize;
	TNotifyEvent    FOldFormShow;
	TNotifyEvent    FOnIconClick;
	TNotifyEvent    FOnLockIconClick;
	TWndMethod      FOldWinProc;
    int             FRadius;
	bool            FIsActive;
    bool            FHasMinbox;
    bool            FHasMaxbox;
	bool            FHasSystemMenu;
	bool            FHasLockbox;
    bool            FSizable;
	bool            FLocked;
	bool            FTransparent;
	bool            FEnable;
private:
    TRect           FCloseBoxRect;
    TRect           FMaxBoxRect;
    TRect           FMinBoxRect;
	TRect           FLockBoxRect;
	TRect           FIconRect;
	int             FCaptionHeight;
	int             FBottomHight;
	int             FLeftFrameWidth;
	int             FRightFrameWidth;
	Graphics::TBitmap* FTitleBuffer;
    Graphics::TBitmap* FLeftBuffer;
    Graphics::TBitmap* FRightBuffer;
    Graphics::TBitmap* FBottomBuffer;
    Graphics::TBitmap* FButtonBuffer;
private:
	Graphics::TBitmap *FTopBmp;
	Graphics::TBitmap *FLeftBmp;
	Graphics::TBitmap *FRightBmp;
	Graphics::TBitmap *FBottomBmp;
	Graphics::TBitmap *FMinBtnBmp;
	Graphics::TBitmap *FMaxBtnBmp;
	Graphics::TBitmap *FRestoreBtnBmp;
	Graphics::TBitmap *FCloseBtnBmp;
	Graphics::TBitmap *FLockBtnBmp;
	Graphics::TBitmap *FLockedBtnBmp;
	Graphics::TBitmap *FIconBmp;
	TFont             *FCaptionFont;
	TRoundFormExRes   *FRes;
    TColor             FInactiveColor;
private:
	unsigned int FMoveHitTest;
	unsigned int FDownHitTest;
	unsigned int FMouseDown;
	unsigned int FOldHitTest;
private:
	TRect __fastcall CaptionTextRect( void );
	void __fastcall RedrawNCArea( void );
    void __fastcall ResetFormShape( void );
    void __fastcall SetRadius( int NewRadius );
	void __fastcall SetFormRound( int Radius );
	void __fastcall ShowForm( TObject *Sender );
    void __fastcall ResizeForm( TObject *Sender );
    virtual void __fastcall Loaded( void );
    void __fastcall ParentWinProc( TMessage &Message );
    void __fastcall OnSetText( TMessage &Message );
    void __fastcall OnNCCalcSize( TMessage &Message );
	void __fastcall OnNCActive( TMessage &Message );
	void __fastcall OnNCPaint( TMessage &Message );
	void __fastcall OnNCHitTest( TMessage &Message );
    void __fastcall OnNCMouseMove( TMessage &Message );
    void __fastcall OnNCLMouseDown( TMessage &Message );
    void __fastcall OnNCLMouseUp( TMessage &Message );
	void __fastcall OnNCLButtonDblClk( TMessage &Message );
	void __fastcall OnNCRButtonDown( TMessage &Message );
    void __fastcall OnSysCommand( TMessage &Message );
    void __fastcall CustomNCProc( TMessage &Message );
    void __fastcall DrawNCArea( HRGN Rgn );
    void __fastcall DrawCaptionBox(  TCanvas* canvas, TRect& BtnRect, Graphics::TBitmap* BtnImg, unsigned int  );
	void __fastcall StretchDrawFrame( TCanvas* canvas,Graphics::TBitmap* SrcImg,TRect& SrcRect,TRect& OutRect,bool IsVert );
    void __fastcall CalcBoxRects( int WinWidth );
    void __fastcall CalcCloseBoxRect( int WinWidth );
    void __fastcall CalcMinBoxRect( void );
    void __fastcall CalcMaxBoxRect( void );
	void __fastcall CalcLockBoxRect( void );
	void __fastcall CalcIconBoxRect( void );
private:
    void __fastcall DrawCaption( int WinWidth );
	void __fastcall DrawCaptionIcon( void );
	void __fastcall DrawCaptionText( void );
    void __fastcall DrawCaptionButtons( void );
	void __fastcall DrawLeftBorder( int WinHeight  );
	void __fastcall DrawRightBorder( int WinHeight  );
	void __fastcall DrawBottomBorder( int WinWidth );
private:
	void __fastcall SetTopBmp(Graphics::TBitmap *Value);
	void __fastcall SetLeftBmp(Graphics::TBitmap *Value);
	void __fastcall SetRightBmp(Graphics::TBitmap *Value);
	void __fastcall SetBottomBmp(Graphics::TBitmap *Value);
	void __fastcall SetCloseBoxBmp(Graphics::TBitmap *Value);
	void __fastcall SetMinimizeBmp(Graphics::TBitmap *Value);
	void __fastcall SetMaximizeBmp(Graphics::TBitmap *Value);
	void __fastcall SetRestoreBmp(Graphics::TBitmap *Value);
	void __fastcall SetLockBmp(Graphics::TBitmap *Value);
	void __fastcall SetLockedBmp(Graphics::TBitmap *Value);
	void __fastcall SetIconBmp(Graphics::TBitmap *Value);
	void __fastcall SetFont( TFont *Value );
	void __fastcall SetLock( bool Locked );
	void __fastcall SetLockBox( bool Exists );
	void __fastcall SetTransparent( bool Locked );
	void __fastcall SetActive( bool Locked );
	void __fastcall SetEnable( bool Enable );
	void __fastcall SetRes( TRoundFormExRes* res );
private:
	int __fastcall GetCaptionHeight(void);
	int __fastcall GetBottomHight(void);
	int __fastcall GetLeftFrameWidth(void);
	int __fastcall GetRightFrameWidth(void);
	Graphics::TBitmap* __fastcall TopBmp(void);
	Graphics::TBitmap* __fastcall LeftBmp(void);
	Graphics::TBitmap* __fastcall RightBmp(void);
	Graphics::TBitmap* __fastcall BottomBmp(void);
	Graphics::TBitmap* __fastcall MinBtnBmp(void);
	Graphics::TBitmap* __fastcall MaxBtnBmp(void);
	Graphics::TBitmap* __fastcall RestoreBtnBmp(void);
	Graphics::TBitmap* __fastcall CloseBtnBmp(void);
	Graphics::TBitmap* __fastcall LockBtnBmp(void);
	Graphics::TBitmap* __fastcall LockedBtnBmp(void);
	Graphics::TBitmap* __fastcall IconBmp(void);
public:
	__fastcall TRoundFormEx(TComponent* Owner);
	__fastcall ~TRoundFormEx( void );
	void __fastcall SetWindowIcons( TBorderIcons Icons );
__published:
	__property TRoundFormExRes*   Resource          ={read=FRes, write = SetRes };
	__property int                Radius            ={read=FRadius, write = SetRadius };
	__property TColor             InactiveFontColor ={read=FInactiveColor, write=FInactiveColor };
	__property bool               Lock              ={read=FLocked, write=SetLock };
	__property bool               ButtonTransparent ={read=FTransparent, write=SetTransparent };
	__property bool               Lockbox           ={read=FHasLockbox, write=SetLockBox };
	__property bool               Active            ={read=FIsActive, write=SetActive };
	__property bool               Enable            ={read=FEnable, write=SetEnable };
	__property Graphics::TBitmap* CaptionGraph      ={read=FTopBmp, write=SetTopBmp};
	__property Graphics::TBitmap* LeftGraph         ={read=FLeftBmp, write=SetLeftBmp};
	__property Graphics::TBitmap* RightGraph        ={read=FRightBmp, write=SetRightBmp};
	__property Graphics::TBitmap* BottomGraph       ={read=FBottomBmp, write=SetBottomBmp};
	__property TNotifyEvent       OnIconClick		={read=FOnIconClick,write=FOnIconClick, default=NULL};
	__property TNotifyEvent       OnLockIconClick   ={read=FOnLockIconClick,write=FOnLockIconClick, default=NULL};
	__property Graphics::TBitmap* CloseGraph        ={read=FCloseBtnBmp, write=SetCloseBoxBmp};
	__property Graphics::TBitmap* MinimizeGraph     ={read=FMinBtnBmp, write=SetMinimizeBmp};
	__property Graphics::TBitmap* MaximizeGraph     ={read=FMaxBtnBmp, write=SetMaximizeBmp};
	__property Graphics::TBitmap* RestoreGraph      ={read=FRestoreBtnBmp, write=SetRestoreBmp};
	__property Graphics::TBitmap* LockGraph         ={read=FLockBtnBmp, write=SetLockBmp};
	__property Graphics::TBitmap* LockedGraph       ={read=FLockedBtnBmp, write=SetLockedBmp};
	__property Graphics::TBitmap* IconGraph         ={read=FIconBmp, write=SetIconBmp};
	__property TFont*             CaptionFont       ={read= FCaptionFont, write=SetFont};
	__property TForm*             ParentForm        ={read= FOwnerForm };
	__property int                CaptionHeight     ={read= GetCaptionHeight };
	__property int                LeftFrameWidth    ={read= GetLeftFrameWidth };
	__property int                RightFrameWidth   ={read= GetRightFrameWidth };
	__property int                BottomHight       ={read= GetBottomHight};
};
//---------------------------------------------------------------------------
#endif
