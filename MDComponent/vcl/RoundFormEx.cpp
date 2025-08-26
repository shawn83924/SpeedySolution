//---------------------------------------------------------------------------
#include <basepch.h>
#include <windowsx.h>
#pragma hdrstop
#include "RoundFormEx.h"
#include <Forms.hpp>
#include <Dialogs.hpp>
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TRoundFormEx *)
{
    new TRoundFormEx(NULL);
}
//---------------------------------------------------------------------------
const float DEGToRAD =  3.141592654 / 180.0;
//---------------------------------------------------------------------------
__fastcall TRoundFormEx::TRoundFormEx(TComponent* Owner)
: TComponent( Owner ),
  FOwnerForm( dynamic_cast< TForm* >( Owner ) ),
  FOldFormResize( NULL ),
  FOldFormShow( NULL ),
  FOldWinProc( NULL ),
  FIsActive( false ),
  FHasSystemMenu( true ),
  FHasMinbox( true ),
  FHasMaxbox( true ),
  FHasLockbox( true ),
  FSizable( true ),
  FLocked( false ),
  FTransparent( false ),
  FEnable( false ),
  FCaptionHeight( GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXSIZEFRAME) ),
  FBottomHight( GetSystemMetrics(SM_CYSIZEFRAME) ),
  FLeftFrameWidth( GetSystemMetrics(SM_CXSIZEFRAME) ),
  FRightFrameWidth( GetSystemMetrics(SM_CXSIZEFRAME) ),
  FMoveHitTest( 0 ),
  FDownHitTest( 0 ),
  FOldHitTest( 0 ),
  FMouseDown( 0 ),
  FInactiveColor( (TColor)COLOR_INACTIVECAPTIONTEXT ),
  FRes( NULL )
{
	if( !FOwnerForm )
		throw Exception( "The owner of a TRoundFormEx must be a form" );
	FParent = FOwnerForm->Parent;
    FCaptionFont = new TFont();
	FCaptionFont->Assign( FOwnerForm->Font );
	///< Bitmaps for frame
	FTopBmp        = new Graphics::TBitmap();
	FLeftBmp       = new Graphics::TBitmap();
	FRightBmp      = new Graphics::TBitmap();
	FBottomBmp     = new Graphics::TBitmap();
	///< Bitmaps for system box
	FIconBmp       = new Graphics::TBitmap();
	FMinBtnBmp     = new Graphics::TBitmap();
	FMaxBtnBmp     = new Graphics::TBitmap();
	FRestoreBtnBmp = new Graphics::TBitmap();
	FCloseBtnBmp   = new Graphics::TBitmap();
	FLockBtnBmp    = new Graphics::TBitmap();
	FLockedBtnBmp  = new Graphics::TBitmap();
	///< Draw buffers
	FLeftBuffer    = new Graphics::TBitmap();
	FRightBuffer   = new Graphics::TBitmap();
	FTitleBuffer   = new Graphics::TBitmap();
	FButtonBuffer  = new Graphics::TBitmap();
	FBottomBuffer  = new Graphics::TBitmap();

	DWORD Style    = GetWindowLong( FOwnerForm->Handle, GWL_STYLE );
	if( !(Style & WS_THICKFRAME) )
	{
		FCaptionHeight   = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFIXEDFRAME);
		FBottomHight     = GetSystemMetrics(SM_CYFIXEDFRAME);
		FLeftFrameWidth  = GetSystemMetrics(SM_CXFIXEDFRAME);
		FRightFrameWidth = GetSystemMetrics(SM_CXFIXEDFRAME);
	}
}
//---------------------------------------------------------------------------
__fastcall TRoundFormEx::~TRoundFormEx( void )
{
	if( !ComponentState.Contains( csDesigning ) )
		SetEnable( false );
	delete FCaptionFont;
	delete FTopBmp;
	delete FLeftBmp;
	delete FRightBmp;
	delete FBottomBmp;
	delete FMinBtnBmp;
	delete FMaxBtnBmp;
	delete FIconBmp;
	delete FRestoreBtnBmp;
	delete FCloseBtnBmp;
	delete FLockBtnBmp;
	delete FLockedBtnBmp;
	delete FTitleBuffer;
	delete FLeftBuffer;
	delete FRightBuffer;
	delete FBottomBuffer;
	delete FButtonBuffer;
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetEnable( bool Enable )
{
	if( FEnable != Enable )
	{
		FEnable = Enable;
		if( FEnable == true )
		{
			if( FOwnerForm->OnResize != ResizeForm )
			{
				FOldFormResize       = FOwnerForm->OnResize;
				FOwnerForm->OnResize = ResizeForm;
			}
			if( FOwnerForm->OnShow != ShowForm )
			{
				FOldFormShow        = FOwnerForm->OnShow;
				FOwnerForm->OnShow  = ShowForm;
			}
			if( FOwnerForm->WindowProc != CustomNCProc )
			{
				FOldWinProc            = FOwnerForm->WindowProc;
				FOwnerForm->WindowProc = CustomNCProc;
			}
			SetFormRound( FRadius );
		}
		else
		{
			if( FOldFormResize != NULL )
				FOwnerForm->OnResize = FOldFormResize;
			if( FOldFormShow != NULL )
				FOwnerForm->OnShow   = FOldFormShow;
			if( FOldWinProc != NULL )
				FOwnerForm->WindowProc   = FOldWinProc;
			if(!FOwnerForm->ComponentState.Contains(csDestroying))
				ResetFormShape( );
        }
	}
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::Loaded( void )
{
	if( !ComponentState.Contains( csDesigning ) )
	{
		if( (FOwnerForm = dynamic_cast< TForm* >( GetOwner()) ) != NULL )
		{
			SetEnable( true );
			FCaptionHeight   = TopBmp()->Height/2;
			FLeftFrameWidth  = LeftBmp()->Width/2;
			FRightFrameWidth = RightBmp()->Width/2;
			FBottomHight     = BottomBmp()->Height/2;
			SetWindowIcons( FOwnerForm->BorderIcons );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetWindowIcons( TBorderIcons Icons )
{
	DWORD Style    = GetWindowLong( FOwnerForm->Handle, GWL_STYLE );

	FSizable       = Style & WS_SIZEBOX;
	FHasMinbox     = Icons.Contains( biMinimize );
	FHasMaxbox     = Icons.Contains( biMaximize );
	FHasSystemMenu = Icons.Contains( biSystemMenu );
	Style &= ~(WS_MINIMIZEBOX); ///< Remove MINIMIZEBOX Icon
	Style &= ~WS_MAXIMIZEBOX;   ///< Remove MAXIMIZEBOX Icon
	Style &= ~WS_SYSMENU;       ///< Remove SYSMENU Icon
	SetWindowLong( FOwnerForm->Handle, GWL_STYLE, Style );
	CalcBoxRects( FOwnerForm->Width );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetFont(TFont *Value)
{
    FCaptionFont->Assign( Value );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::CalcBoxRects( int WinWidth )
{
	CalcCloseBoxRect( WinWidth );
    CalcMaxBoxRect( );
    CalcMinBoxRect( );
	CalcLockBoxRect( );
	CalcIconBoxRect( );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::CalcIconBoxRect( void )
{
	int      IconHeight = IconBmp()->Height/2;
	int      YOffset    = (GetCaptionHeight()-IconHeight)/2;
	FIconRect = Rect( GetLeftFrameWidth(),
					  YOffset,
					  IconBmp()->Width + GetLeftFrameWidth(),
					  YOffset + IconHeight );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::CalcCloseBoxRect( int WinWidth )
{
	int Btnheight = CloseBtnBmp()->Height/3;
	int BtnWidth  = CloseBtnBmp()->Width/2;

	if( FHasSystemMenu == true )
    {
		FCloseBoxRect.left   = WinWidth - 2*GetLeftFrameWidth() - BtnWidth;
		FCloseBoxRect.top    = (GetCaptionHeight() - Btnheight )/2;
		FCloseBoxRect.right  = FCloseBoxRect.left + BtnWidth;
		FCloseBoxRect.bottom = FCloseBoxRect.top  + Btnheight;
	}
	else
	{
		FCloseBoxRect.left   = WinWidth - 2*GetLeftFrameWidth();
		FCloseBoxRect.top    = (GetCaptionHeight() - Btnheight )/2;
		FCloseBoxRect.right  = WinWidth - 2*GetLeftFrameWidth();
		FCloseBoxRect.bottom = (GetCaptionHeight() - Btnheight )/2;
	}
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::CalcMaxBoxRect( void )
{
	int Btnheight = MaxBtnBmp()->Height/3;
	int BtnWidth  = MaxBtnBmp()->Width/2;

	if( FHasMaxbox == true )
	{
		FMaxBoxRect.left   = FCloseBoxRect.left - GetLeftFrameWidth() - BtnWidth;
		FMaxBoxRect.top    = (GetCaptionHeight() - Btnheight )/2;
		FMaxBoxRect.right  = FMaxBoxRect.left + BtnWidth;
		FMaxBoxRect.bottom = FMaxBoxRect.top  + Btnheight;
	}
	else
	{
		FMaxBoxRect.left   = FCloseBoxRect.left;
		FMaxBoxRect.top    = (GetCaptionHeight() - Btnheight )/2;
		FMaxBoxRect.right  = FCloseBoxRect.left;
		FMaxBoxRect.bottom = (GetCaptionHeight() - Btnheight )/2;
	}
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::CalcMinBoxRect( void )
{
	int Btnheight = MinBtnBmp()->Height/3;
	int BtnWidth  = MinBtnBmp()->Width/2;

	if( FHasMinbox == true )
	{
		FMinBoxRect.left   = FMaxBoxRect.left - GetLeftFrameWidth() - BtnWidth;
		FMinBoxRect.top    = (GetCaptionHeight() - Btnheight )/2;
        FMinBoxRect.right  = FMinBoxRect.left + BtnWidth;
        FMinBoxRect.bottom = FMinBoxRect.top  + Btnheight;
    }
    else
    {
		FMinBoxRect.left   = FMaxBoxRect.left;
		FMinBoxRect.top    = (GetCaptionHeight() - Btnheight )/2;
		FMinBoxRect.right  = FMaxBoxRect.left;
		FMinBoxRect.bottom = (GetCaptionHeight() - Btnheight )/2;
    }
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::CalcLockBoxRect( void )
{
	int Btnheight = LockBtnBmp()->Height/3;
	int BtnWidth  = LockBtnBmp()->Width/2;

	if( FHasLockbox == true  )
    {
		FLockBoxRect.left   = FMinBoxRect.left - GetLeftFrameWidth() - BtnWidth;
		FLockBoxRect.top    = (GetCaptionHeight() - Btnheight )/2;
        FLockBoxRect.right  = FLockBoxRect.left + BtnWidth;
        FLockBoxRect.bottom = FLockBoxRect.top  + Btnheight;
    }
    else
    {
        FLockBoxRect.left   = FMinBoxRect.left;
		FLockBoxRect.top    = (GetCaptionHeight() - Btnheight )/2;
		FLockBoxRect.right  = FMinBoxRect.left;
		FLockBoxRect.bottom = (GetCaptionHeight() - Btnheight )/2;
	}
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::ShowForm( TObject *Sender )
{
	if( FOldFormShow != NULL )
		FOldFormShow( Sender );
	SetFormRound( FRadius );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::ResizeForm( TObject *Sender )
{
	if( FOldFormResize != NULL )
		FOldFormResize( Sender );
	CalcBoxRects( FOwnerForm->Width );
	SetFormRound( FRadius );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetRadius( int NewRadius )
{
	if( FRadius != NewRadius )
	{
		FRadius = NewRadius;
		if( !ComponentState.Contains( csDesigning ) )
			SetFormRound( FRadius );
	}
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetFormRound( int Radius )
{
	 HRGN   WinRgn = CreateRoundRectRgn( 0, 0, FOwnerForm->Width+1, FOwnerForm->Height+1, 2*Radius, 2*Radius );

	 SetWindowRgn( FOwnerForm->Handle , WinRgn , FOwnerForm->Visible );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::ResetFormShape( void )
{
	 SetWindowRgn( FOwnerForm->Handle , NULL , FOwnerForm->Visible );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::ParentWinProc( TMessage &Message )
{
	 if( FOldWinProc != NULL )
		 FOldWinProc( Message );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::OnNCCalcSize( TMessage &Message )
{
	RECT*               pRect;
	LPNCCALCSIZE_PARAMS pNCSP;
	if( Message.WParam == TRUE )
	{
    	pNCSP = (LPNCCALCSIZE_PARAMS)Message.LParam;
        pRect = &pNCSP->rgrc[0];
		pRect->left    = pRect->left   + GetLeftFrameWidth();
		pRect->top     = pRect->top    + GetCaptionHeight();
		pRect->right   = pRect->right  - GetRightFrameWidth();
		pRect->bottom  = pRect->bottom - GetBottomHight();
		pNCSP->rgrc[1] = pNCSP->rgrc[0];
		Message.Result = WVR_REDRAW;
	}
	else
		ParentWinProc( Message );
}
//---------------------------------------------------------------------------
TRect __fastcall TRoundFormEx::CaptionTextRect( void )
{
	return 	TRect( GetLeftFrameWidth() + IconBmp()->Width + 5 ,0, FLockBoxRect.left, GetCaptionHeight() );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::RedrawNCArea( void  )
{
	RedrawWindow( FOwnerForm->Handle, NULL, NULL, RDW_INVALIDATE|RDW_FRAME );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::OnNCActive( TMessage &Message )
{
	{
		SuppressStyle ss( FOwnerForm->Handle, WS_VISIBLE );
		ParentWinProc( Message );
	}
	FIsActive = (bool)Message.WParam;
	RedrawNCArea( );
	Message.Result = 1;
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::OnNCPaint( TMessage &Message )
{
	if( FOwnerForm->WindowState == wsMinimized )
	{
		ParentWinProc( Message );
		return;
	}
	///< wParam
	///< Handle to the update region of the window.
	///< The update region is clipped to the window frame.
	///< When wParam is 1, the entire window frame needs to be updated.
	if( Message.WParam == 1 )
	{
		HRGN     WinRgn;
		RECT     WinRect;

		GetWindowRect( FOwnerForm->Handle, &WinRect );
		WinRgn = CreateRectRgnIndirect( &WinRect );
		DrawNCArea( WinRgn );
		DeleteObject((HGDIOBJ)WinRgn );
	}
	else
		DrawNCArea( (HRGN) Message.WParam );
	Message.Result = 1;
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::OnNCHitTest( TMessage &Message )
{
	POINT point;
	RECT  WinRect,SysRect,CaptionRect;

	GetWindowRect( FOwnerForm->Handle, &WinRect);
	point.x = GET_X_LPARAM( Message.LParam ) - WinRect.left;
	point.y = GET_Y_LPARAM( Message.LParam ) - WinRect.top;
	if( PtInRect(&FCloseBoxRect, point))
	{
		Message.Result = HTCLOSE;
        return;
    }
	if(PtInRect(&FMinBoxRect, point) && FHasMinbox )
	{
		Message.Result = HTMINBUTTON;
        return;
	}
	if(PtInRect(&FMaxBoxRect, point) && FHasMaxbox )
    {
		Message.Result = HTMAXBUTTON;
        return;
	}
	if(PtInRect(&FLockBoxRect, point) )
	{
		Message.Result = HTLOCKBTN;
		return;
	}
	if(PtInRect(&FIconRect, point) )
	{
		Message.Result = HTWINICON;
		return;
	}
	SetRect( &CaptionRect,0,3,WinRect.right-WinRect.left, GetCaptionHeight()-3 );

	int cx = GetSystemMetrics(SM_CXSMICON);
	int cy = GetSystemMetrics(SM_CYSMICON);
	SetRect(&SysRect, GetLeftFrameWidth(), 5, GetLeftFrameWidth() + cx, cy + 5);
	if( PtInRect( &SysRect, point ))
	{
		Message.Result = HTSYSMENU;
		return;
	}
	if( PtInRect( &CaptionRect, point ))
	{
		Message.Result = HTCAPTION;
		return;
	}
	ParentWinProc( Message );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::OnNCMouseMove( TMessage &Message )
{
	if( Message.WParam >= HTLEFT && Message.WParam <= HTBOTTOMRIGHT ||
		Message.WParam == HTCAPTION && FOwnerForm->WindowState != wsMaximized )
	    ParentWinProc( Message );

	FMoveHitTest = (UINT)Message.WParam;
	FDownHitTest = 0;
	if( FOldHitTest != (UINT)Message.WParam )
	{
		RECT     WinRect;

		GetWindowRect( FOwnerForm->Handle, &WinRect );
		RedrawNCArea( );
		FOldHitTest = (UINT)Message.WParam;
	}
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::OnNCLMouseDown( TMessage &Message )
{
	POINT point;

	point.x = GET_X_LPARAM( Message.LParam );
	point.y = GET_Y_LPARAM( Message.LParam );
	FDownHitTest = (UINT)Message.WParam;
	FMoveHitTest = FDownHitTest;

	RedrawNCArea( );

	if( Message.WParam >= HTLEFT && Message.WParam <= HTBOTTOMRIGHT ||
		Message.WParam == HTCAPTION && FOwnerForm->WindowState != wsMaximized)
	{
		ParentWinProc( Message );
		return;
	}
	else
    {
		if( (UINT)Message.WParam == HTCLOSE || (UINT)Message.WParam == HTMINBUTTON ||
			(UINT)Message.WParam == HTMAXBUTTON || (UINT)Message.WParam == HTLOCKBTN )
	    {
			RECT WindowRect;

			GetWindowRect(FOwnerForm->Handle, &WindowRect );
			point.x -= WindowRect.left;
			point.y -= WindowRect.top;
			if( Message.WParam == HTCLOSE)
	        {
				if( PtInRect(&FCloseBoxRect, point))
					FMouseDown = HTCLOSE;
			}
			else if( Message.WParam == HTMINBUTTON)
        	{
				if( PtInRect(&FMinBoxRect, point))
					FMouseDown = HTMINBUTTON;
			}
			else if( Message.WParam == HTMAXBUTTON)
        	{
				if( PtInRect(&FMaxBoxRect, point) )
					FMouseDown = HTMAXBUTTON;
			}
			else if( (UINT)Message.WParam == HTLOCKBTN )
			{
				if( PtInRect(&FLockBoxRect, point) )
					FMouseDown = HTLOCKBTN;
			}
			else if( (UINT)Message.WParam == HTWINICON )
			{
				if( PtInRect(&FLockBoxRect, point) )
					FMouseDown = HTWINICON;
            }
		}
		else FMouseDown = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::OnNCLMouseUp( TMessage &Message )
{
	if( Message.WParam == HTCLOSE )
    {
		SendMessage( FOwnerForm->Handle, WM_CLOSE, 0, 0);
		return;
	}
	else if( Message.WParam == HTMAXBUTTON)
    {
		if( FOwnerForm->WindowState == wsNormal )
			FOwnerForm->Perform( WM_SYSCOMMAND,SC_MAXIMIZE, 0 );
		else
	        FOwnerForm->Perform( WM_SYSCOMMAND,SC_RESTORE, 0 );
	}
   	else if( Message.WParam == HTMINBUTTON)
	{
		//if( FLocked == false )
			FOwnerForm->Perform( WM_SYSCOMMAND,SC_MINIMIZE,0 );
	}
	else if( (UINT)Message.WParam == HTLOCKBTN )
	{
		if( FHasLockbox == true  )
		{
			if( FOnLockIconClick != NULL )
				FOnLockIconClick( this );
			SetLock( !FLocked );
		}
	}
	else if( (UINT)Message.WParam == HTWINICON )
	{
		if( FOnIconClick != NULL)
			FOnIconClick( this );
	}
	else
		return;
	FDownHitTest = 0;
	FMoveHitTest = 0;
	FMouseDown   = 0;

	RedrawNCArea( );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::OnNCLButtonDblClk( TMessage &Message )
{
	if( Message.WParam == HTCAPTION && FSizable )
	{
		if ( FOwnerForm->WindowState == wsMaximized )
			FOwnerForm->Perform( WM_SYSCOMMAND,SC_RESTORE, 0 );
		else
			FOwnerForm->Perform( WM_SYSCOMMAND,SC_MAXIMIZE, 0 );

		FDownHitTest = 0;
		FMoveHitTest = 0;
		RedrawNCArea( );
	}
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::OnNCRButtonDown( TMessage &Message )
{
	if( Message.WParam == HTCAPTION)
    {
        HMENU hMenu = GetSystemMenu( FOwnerForm->Handle, FALSE);
		POINT point;

		point.x = GET_X_LPARAM( Message.LParam );
		point.y = GET_Y_LPARAM( Message.LParam );
		if( hMenu )
        {
        	int RestoreID  =  GetMenuItemID( hMenu, 0 );
        	int MovingID   =  GetMenuItemID( hMenu, 1 );
        	int SizingID   =  GetMenuItemID( hMenu, 2 );
            int MinimizeID =  GetMenuItemID( hMenu, 3 );
            int MaximizeID =  GetMenuItemID( hMenu, 4 );
            int CloseID    =  GetMenuItemID( hMenu, 6 );
            if( FOwnerForm->WindowState == wsMaximized )
            {
                 EnableMenuItem( hMenu, RestoreID, MF_ENABLED);
                 EnableMenuItem( hMenu, MovingID,  MF_GRAYED);
                 EnableMenuItem( hMenu, SizingID,  MF_GRAYED);
                 EnableMenuItem( hMenu, MinimizeID,MF_ENABLED);
                 EnableMenuItem( hMenu, MaximizeID,MF_GRAYED);
                 EnableMenuItem( hMenu, CloseID,   MF_ENABLED);
            }
            else if( FOwnerForm->WindowState == wsNormal )
            {
                 EnableMenuItem( hMenu, RestoreID, MF_GRAYED);
                 EnableMenuItem( hMenu, MovingID,  MF_ENABLED);
                 EnableMenuItem( hMenu, SizingID,  MF_ENABLED);
                 EnableMenuItem( hMenu, MinimizeID,MF_ENABLED);
                 EnableMenuItem( hMenu, MaximizeID,MF_ENABLED);
                 EnableMenuItem( hMenu, CloseID,   MF_ENABLED);
            }
			else if( FOwnerForm->WindowState == wsMinimized )
            {
                 EnableMenuItem( hMenu, RestoreID, MF_ENABLED);
                 EnableMenuItem( hMenu, MovingID,  MF_GRAYED);
                 EnableMenuItem( hMenu, SizingID,  MF_GRAYED);
                 EnableMenuItem( hMenu, MinimizeID,MF_GRAYED);
                 EnableMenuItem( hMenu, MaximizeID,MF_ENABLED);
                 EnableMenuItem( hMenu, CloseID,   MF_ENABLED);
            }
			int Result = TrackPopupMenu( hMenu, TPM_LEFTALIGN|TPM_RETURNCMD, point.x, point.y, 0, FOwnerForm->Handle, NULL);
            if( Result == RestoreID )
		        FOwnerForm->Perform( WM_SYSCOMMAND,SC_RESTORE, 0 );
        	else if( Result == MovingID)
				FOwnerForm->Perform( WM_SYSCOMMAND,SC_MOVE,0 );
        	else if( Result == SizingID)
	            FOwnerForm->Perform( WM_SYSCOMMAND,SC_SIZE,0 );
			else if( Result == MinimizeID /*&& FLocked == false*/ )
				FOwnerForm->Perform( WM_SYSCOMMAND,SC_MINIMIZE,0 );
            else if( Result == MaximizeID)
			    FOwnerForm->Perform( WM_SYSCOMMAND,SC_MAXIMIZE, 0 );
            else if( Result == CloseID)
				FOwnerForm->Perform( WM_SYSCOMMAND,SC_CLOSE, 0 );
        }
	}
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::OnSysCommand( TMessage &Message )
{
	if( Message.WParam == SC_MAXIMIZE )
        FOwnerForm->Perform( WM_SYSCOMMAND,SC_MAXIMIZE, 0 );
	else if ( Message.WParam == SC_RESTORE)
		FOwnerForm->Perform( WM_SYSCOMMAND,SC_RESTORE, 0 );
	else if ( Message.WParam == SC_MINIMIZE /*&& FLocked == false*/  )
		FOwnerForm->Perform( WM_SYSCOMMAND,SC_MINIMIZE,0 );
	else if ( Message.WParam == SC_CLOSE )
		SendMessage( FOwnerForm->Handle, WM_CLOSE, 0, 0);
    else
		ParentWinProc( Message );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::OnSetText( TMessage &Message )
{
	{
      	SuppressStyle ss( FOwnerForm->Handle, WS_VISIBLE );
    	ParentWinProc( Message );
	}
	RedrawNCArea( );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::CustomNCProc( TMessage &Message )
{
	switch( Message.Msg )
    {
		case WM_NCCALCSIZE:      OnNCCalcSize( Message );return;
		case WM_NCACTIVATE:      OnNCActive( Message );return;
		case WM_NCPAINT:         OnNCPaint( Message );return;
		case WM_NCHITTEST:       OnNCHitTest( Message );return;
        case WM_NCLBUTTONUP:     OnNCLMouseUp( Message );return;
        case WM_NCLBUTTONDOWN:   OnNCLMouseDown( Message );return;
		case WM_NCMOUSEMOVE:     OnNCMouseMove( Message );return;
        case WM_NCLBUTTONDBLCLK: OnNCLButtonDblClk( Message );return;
        case WM_NCRBUTTONDOWN:   OnNCRButtonDown( Message );return;
        case WM_SETTEXT:         OnSetText( Message );return;
		default:                 ParentWinProc( Message );break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::StretchDrawFrame( TCanvas* DestCanvas,
												Graphics::TBitmap* SrcImg,
                                                TRect& SrcRect,
                                                TRect& OutRect,
                                                bool IsVert )
{
    int	SrcX = SrcRect.Left;
	int SrcY = SrcRect.Top;

    if( OutRect.Width() <= 0 ||	OutRect.Height()<= 0 ||
	    SrcRect.Width() <= 0 ||	SrcRect.Height()<= 0 )
		return;
	if( FIsActive == false )///< Set inactive image offset
    {
	    if( IsVert == true )
	        SrcY += SrcImg->Height/2;
        else
	        SrcX += SrcImg->Width/2;
    }
	StretchBlt( DestCanvas->Handle,      // handle to destination DC
			  	OutRect.left, // x-coord of destination upper-left corner
				OutRect.Top, // y-coord of destination upper-left corner
				OutRect.Width(),   // width of destination rectangle
				OutRect.Height(),  // height of destination rectangle
				SrcImg->Canvas->Handle,       // handle to source DC
				SrcX,  // x-coord of source upper-left corner
				SrcY,  // y-coord of source upper-left corner
				SrcRect.Width(),    // width of source rectangle
				SrcRect.Height(),   // height of source rectangle
				SRCCOPY);       // raster operation code
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::DrawCaptionBox( TCanvas* canvas, TRect& BtnRect, Graphics::TBitmap* BtnImg, unsigned int HitTest )
{
	int YOffset;
    int XOffset = 0;

    if( FDownHitTest == HitTest )
	  	YOffset = 2*BtnRect.Height();  ///< Mouse down
	else if(FMoveHitTest == HitTest )
    {
		if( FMouseDown == HitTest )
	  		YOffset = 2*BtnRect.Height();///< Mouse down
		else
	  		YOffset = BtnRect.Height(); ///< Mouse over
	}
	else
		YOffset = 0; ///< Normal stat
    FButtonBuffer->Width  = BtnRect.Width();
	FButtonBuffer->Height = BtnRect.Height();
	if( FIsActive == false )
        XOffset = FButtonBuffer->Width;
    FButtonBuffer->Canvas->Draw( -XOffset, -YOffset, BtnImg );
    if( FTransparent == true )
    {
	    FButtonBuffer->Transparent = true;
	    FButtonBuffer->TransparentColor = FButtonBuffer->Canvas->Pixels[0][0];
    }
    else
	    FButtonBuffer->Transparent = false;
    canvas->Draw( BtnRect.left, BtnRect.top, FButtonBuffer );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::DrawCaptionButtons( void )
{
  	DrawCaptionBox( FTitleBuffer->Canvas, FCloseBoxRect, CloseBtnBmp(), HTCLOSE );
    if( FOwnerForm->WindowState == wsMaximized )
		DrawCaptionBox( FTitleBuffer->Canvas, FMaxBoxRect, RestoreBtnBmp(), HTMAXBUTTON );
	else
		DrawCaptionBox( FTitleBuffer->Canvas, FMaxBoxRect, MaxBtnBmp(), HTMAXBUTTON );
	if( FHasLockbox == true )
	{
		if( FLocked == true )
			DrawCaptionBox( FTitleBuffer->Canvas, FLockBoxRect, LockedBtnBmp(), HTLOCKBTN );
		else
		{
			DrawCaptionBox( FTitleBuffer->Canvas, FMinBoxRect, MinBtnBmp(), HTMINBUTTON );
			DrawCaptionBox( FTitleBuffer->Canvas, FLockBoxRect, LockBtnBmp(), HTLOCKBTN );
		}
	}
	else
		DrawCaptionBox( FTitleBuffer->Canvas, FMinBoxRect, MinBtnBmp(), HTMINBUTTON );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::DrawCaption( int WinWidth  )
{
	TRect CaptionSrcRect( 0, 0, TopBmp()->Width, GetCaptionHeight() );
	TRect CaptionRect( GetLeftFrameWidth(), 0, WinWidth - GetRightFrameWidth(), GetCaptionHeight() );

	if( FTitleBuffer->Width  != WinWidth|| FTitleBuffer->Height != GetCaptionHeight() )
    {
	    FTitleBuffer->Width  = WinWidth;
		FTitleBuffer->Height = GetCaptionHeight();
    }
	StretchDrawFrame( FTitleBuffer->Canvas, TopBmp(), CaptionSrcRect, CaptionRect, true );
	if( FIsActive == true )///< Draw Left-Top and Right-Top corner.
	{
		FTitleBuffer->Canvas->CopyRect( TRect(0,0,GetLeftFrameWidth(),GetCaptionHeight()), LeftBmp()->Canvas, TRect(0,0,GetLeftFrameWidth(),GetCaptionHeight()) );
		FTitleBuffer->Canvas->CopyRect( TRect(WinWidth-GetRightFrameWidth(),0,WinWidth ,GetCaptionHeight()), RightBmp()->Canvas, TRect(0,0,GetRightFrameWidth(),GetCaptionHeight()) );
	}
	else
	{
		FTitleBuffer->Canvas->CopyRect( TRect(0,0,GetLeftFrameWidth(),GetCaptionHeight()), LeftBmp()->Canvas, TRect(GetLeftFrameWidth(),0,GetLeftFrameWidth()*2,GetCaptionHeight()) );
		FTitleBuffer->Canvas->CopyRect( TRect(WinWidth-GetRightFrameWidth(),0,WinWidth,GetCaptionHeight()), RightBmp()->Canvas, TRect(GetRightFrameWidth(),0,GetRightFrameWidth()*2,GetCaptionHeight()) );
    }
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::DrawCaptionIcon( void )
{
	if( IconBmp()->Width > 0 )
	{
		int      IconHeight = IconBmp()->Height/2;
		int      YOffset    = (GetCaptionHeight()-IconHeight)/2;
		HRGN     IconRgn;
		TRect    IconRect( GetLeftFrameWidth(),YOffset,IconBmp()->Width + GetLeftFrameWidth(), YOffset + IconHeight );

		IconRgn = CreateRectRgnIndirect( &IconRect );
		SelectClipRgn( FTitleBuffer->Canvas->Handle, IconRgn );
		if( FIsActive == true )
			FTitleBuffer->Canvas->Draw( GetLeftFrameWidth(), YOffset, IconBmp());
		else
			FTitleBuffer->Canvas->Draw( GetLeftFrameWidth(), YOffset - IconHeight, IconBmp());
		SelectClipRgn( FTitleBuffer->Canvas->Handle, NULL );
        DeleteObject((HGDIOBJ)IconRgn );
    }
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::DrawCaptionText(  )
{
	TRect       TextRect = CaptionTextRect();
	String      WinCaption = FOwnerForm->Caption;
	TTextFormat Formats;

	FTitleBuffer->Canvas->Brush->Style = bsClear;
	FTitleBuffer->Canvas->Font         = FCaptionFont;
	if( FIsActive == false )
		FTitleBuffer->Canvas->Font->Color = FInactiveColor;
	Formats<<tfSingleLine<<tfLeft<<tfVerticalCenter<<tfEndEllipsis;
	FTitleBuffer->Canvas->TextRect( TextRect, WinCaption, Formats );
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::DrawLeftBorder( int WinHeight )
{
	int   RepeatHeight = LeftBmp()->Height - GetCaptionHeight() - GetBottomHight();
	int   LeftHeight   = WinHeight - GetBottomHight() - GetCaptionHeight();
	TRect LeftFrameRect( 0, 0, GetLeftFrameWidth(), LeftHeight );

	if( FLeftBuffer->Width != GetLeftFrameWidth() || FTitleBuffer->Height != LeftHeight)
	{
		FLeftBuffer->Width  = GetLeftFrameWidth();
		FLeftBuffer->Height = LeftHeight;
	}
    if( RepeatHeight > 0 )
    {
		TRect SrcRect( 0, GetCaptionHeight(), GetLeftFrameWidth(), LeftBmp()->Height - GetBottomHight() );
		StretchDrawFrame( FLeftBuffer->Canvas, LeftBmp(), SrcRect, LeftFrameRect, false );
	}
	else
	{
		TRect SrcRect( 0, 0, GetLeftFrameWidth(), LeftBmp()->Height );
		StretchDrawFrame( FLeftBuffer->Canvas, LeftBmp(), SrcRect, LeftFrameRect, false );
    }
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::DrawRightBorder( int WinHeight )
{
	int   RepeatHeight = RightBmp()->Height - GetCaptionHeight() - GetBottomHight();
	int   RightHeight  = WinHeight - GetBottomHight() - GetCaptionHeight();
	TRect RightFrameRect( 0,0, GetRightFrameWidth(),RightHeight );

	if( FRightBuffer->Width != GetRightFrameWidth() || FRightBuffer->Height != RightHeight )
	{
		FRightBuffer->Width  = GetRightFrameWidth();
		FRightBuffer->Height = RightHeight;
	}
	if( RepeatHeight > 0 )
    {
		TRect SrcRect( 0, GetCaptionHeight(), GetRightFrameWidth(), RightBmp()->Height - GetBottomHight() );
		StretchDrawFrame( FRightBuffer->Canvas, RightBmp(), SrcRect, RightFrameRect, false );
	}
	else
	{
		TRect SrcRect( 0, 0, GetRightFrameWidth(), RightBmp()->Height );
		StretchDrawFrame( FRightBuffer->Canvas, RightBmp(), SrcRect, RightFrameRect, false );
    }
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::DrawBottomBorder( int WinWidth )
{
    TRect SrcRect( 0, 0, BottomBmp()->Width, GetBottomHight() );
	TRect BottomRect( GetLeftFrameWidth(), 0, WinWidth - GetRightFrameWidth(), GetBottomHight());

	if( FBottomBuffer->Width  != WinWidth ||  FBottomBuffer->Height != GetBottomHight() )
	{
		FBottomBuffer->Width  = WinWidth;
		FBottomBuffer->Height = GetBottomHight();
	}
	StretchDrawFrame( FBottomBuffer->Canvas, BottomBmp(), SrcRect, BottomRect, true );
	if( FIsActive == true )///< Draw Left-Top and Right-Top corner.
	{
		FBottomBuffer->Canvas->CopyRect( TRect(0,0,GetLeftFrameWidth(),GetBottomHight()), LeftBmp()->Canvas, TRect(0,LeftBmp()->Height- GetBottomHight(), GetLeftFrameWidth(), LeftBmp()->Height) );
		FBottomBuffer->Canvas->CopyRect( TRect(WinWidth-GetRightFrameWidth(),0,WinWidth ,GetBottomHight()), RightBmp()->Canvas, TRect(0,RightBmp()->Height - GetBottomHight(),GetRightFrameWidth(),RightBmp()->Height) );
	}
	else
	{
		FBottomBuffer->Canvas->CopyRect( TRect(0,0,GetLeftFrameWidth(),GetBottomHight()), LeftBmp()->Canvas, TRect(GetLeftFrameWidth(),LeftBmp()->Height- GetBottomHight(), GetLeftFrameWidth()*2, LeftBmp()->Height) );
		FBottomBuffer->Canvas->CopyRect( TRect(WinWidth-GetRightFrameWidth(),0,WinWidth ,GetBottomHight()), RightBmp()->Canvas, TRect(GetRightFrameWidth(),RightBmp()->Height-GetBottomHight(),GetRightFrameWidth()*2,RightBmp()->Height) );
    }
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::DrawNCArea( HRGN Rgn  )
{
	TCanvas* FormCanvas;
	TRect    WinRect;
	int      WinWidth,WinHeight;

	FormCanvas = new TCanvas( );
	FormCanvas->Handle =  GetDCEx( FOwnerForm->Handle,(HRGN)Rgn,
								   DCX_WINDOW|DCX_CACHE|
								   DCX_CLIPSIBLINGS|DCX_INTERSECTRGN );
	///< Get Windows current position.
	GetWindowRect( FOwnerForm->Handle, &WinRect );
	WinWidth  = WinRect.Width();
	WinHeight = WinRect.Height();
	///< Deaw Caption
	DrawCaption( WinWidth );
	DrawCaptionButtons();  ///< Deaw Caption system buttons
	DrawCaptionIcon();     ///< Draw icon
	DrawCaptionText();     ///< Draw text
	FormCanvas->Draw( 0, 0, FTitleBuffer ); ///< Draw whole caption bitmap into windows DC.
	///< Left border
	DrawLeftBorder( WinHeight );
	FormCanvas->Draw( 0, GetCaptionHeight(), FLeftBuffer );
	///< Right border
	DrawRightBorder( WinHeight );
	FormCanvas->Draw( WinWidth - GetRightFrameWidth(), GetCaptionHeight(), FRightBuffer );
	///< Bottom border
	DrawBottomBorder( WinWidth );
	FormCanvas->Draw( 0, WinHeight - GetBottomHight(), FBottomBuffer );
	///< Paint finished, Release resource.
	ReleaseDC( FOwnerForm->Handle, FormCanvas->Handle );
	delete FormCanvas;
	RedrawWindow( FOwnerForm->Handle, &WinRect,(HRGN)Rgn, RDW_UPDATENOW);
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetTopBmp(Graphics::TBitmap *Value)
{
	FTopBmp->Assign( Value );
	FTopBmp->Dormant();
	FCaptionHeight = TopBmp()->Height / 2;
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetLeftBmp(Graphics::TBitmap *Value)
{
    FLeftBmp->Assign( Value );
    FLeftBmp->Dormant();
	FLeftFrameWidth = LeftBmp()->Width / 2;
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetRightBmp(Graphics::TBitmap *Value)
{
    FRightBmp->Assign( Value );
    FRightBmp->Dormant();
	FRightFrameWidth = RightBmp()->Width / 2;
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetBottomBmp(Graphics::TBitmap *Value)
{
    FBottomBmp->Assign( Value );
    FBottomBmp->Dormant();
	FBottomHight = BottomBmp()->Height / 2;
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetCloseBoxBmp(Graphics::TBitmap *Value)
{
    FCloseBtnBmp->Assign( Value );
	FCloseBtnBmp->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetMinimizeBmp(Graphics::TBitmap *Value)
{
    FMinBtnBmp->Assign( Value );
	FMinBtnBmp->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetMaximizeBmp(Graphics::TBitmap *Value)
{
    FMaxBtnBmp->Assign( Value );
	FMaxBtnBmp->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetRestoreBmp(Graphics::TBitmap *Value)
{
    RestoreGraph->Assign( Value );
    RestoreGraph->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetLockBmp(Graphics::TBitmap *Value)
{
    FLockBtnBmp->Assign( Value );
    FLockBtnBmp->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetLockedBmp(Graphics::TBitmap *Value)
{
    FLockedBtnBmp->Assign( Value );
	FLockedBtnBmp->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetIconBmp(Graphics::TBitmap *Value)
{
	FIconBmp->Assign( Value );
	FIconBmp->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetLockBox( bool Exists )
{
   if( FHasLockbox != Exists )
   {
	   FHasLockbox = Exists;
       CalcLockBoxRect( );
   }
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetLock( bool Locked )
{
   bool IsVisible = FOwnerForm->Visible;
   if( FHasLockbox == true && FLocked != Locked )
   {
	   FLocked = Locked;
	   if( FLocked == TRUE )
	   {
		   TPoint ClPt( FOwnerForm->Left, FOwnerForm->Top );
		   TPoint ScrPt = FOwnerForm->Parent->ClientToScreen( ClPt );

		   if( IsVisible == true )
			   FOwnerForm->Visible = false;
		   FParent = FOwnerForm->Parent;
		   FOwnerForm->Parent = NULL;
		   Application->NormalizeTopMosts();
		   SetWindowPos( FOwnerForm->Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
		   if( IsVisible == true )
		   {
			   FOwnerForm->Left = ScrPt.X;
			   FOwnerForm->Top = ScrPt.Y;
			   FOwnerForm->Visible = true;
		   }
	   }
	   else
	   {
		   TPoint ScrPt( FOwnerForm->Left, FOwnerForm->Top );
		   TPoint ClPt = FParent->ScreenToClient( ScrPt );

		   if( IsVisible == true )
			   FOwnerForm->Visible = false;
		   FOwnerForm->Parent = FParent;
		   Application->NormalizeTopMosts();
		   SetWindowPos( FOwnerForm->Handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
		   if( IsVisible == true )
		   {
			   if( ClPt.Y < 0 )
				   ClPt.Y = 0;
			   if( ClPt.X < 0 )
				   ClPt.X = 0;
			   if( ClPt.Y > FParent->Height - 30 )
				   ClPt.Y = FParent->Height - 30;
			   if( ClPt.X > FParent->Width - 100 )
				   ClPt.X = FParent->Width - 100;
			   FOwnerForm->Left = ClPt.X;
			   FOwnerForm->Top = ClPt.Y;
			   FOwnerForm->Visible = true;
		   }
	   }
   }
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetTransparent( bool Transparent )
{
	FTransparent = Transparent;
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetRes( TRoundFormExRes* res )
{
	FRes = res;
	if( FRes != NULL )
	{
		FTopBmp->SetSize(0,0);
		FLeftBmp->SetSize(0,0);
		FRightBmp->SetSize(0,0);
		FBottomBmp->SetSize(0,0);
		FIconBmp->SetSize(0,0);
		FMinBtnBmp->SetSize(0,0);
		FMaxBtnBmp->SetSize(0,0);
		FRestoreBtnBmp->SetSize(0,0);
		FCloseBtnBmp->SetSize(0,0);
		FLockBtnBmp->SetSize(0,0);
		FLockedBtnBmp->SetSize(0,0);

		FCaptionHeight   = TopBmp()->Height/2;
		FLeftFrameWidth  = LeftBmp()->Width/2;
		FRightFrameWidth = RightBmp()->Width/2;
		FBottomHight     = BottomBmp()->Height/2;
	}
}
//------------------------------------------------------------------------------
void __fastcall TRoundFormEx::SetActive( bool Active )
{
	if( Active != FIsActive )
	{
		FIsActive = Active;
		ShowForm( FOwnerForm );
	}
}
//------------------------------------------------------------------------------
int __fastcall TRoundFormEx::GetCaptionHeight(void)   { return (Resource == NULL)? FCaptionHeight: Resource->CaptionHeight; }
//------------------------------------------------------------------------------
int __fastcall TRoundFormEx::GetBottomHight(void)     { return (Resource == NULL)? FBottomHight: Resource->BottomHight; }
//------------------------------------------------------------------------------
int __fastcall TRoundFormEx::GetLeftFrameWidth(void)  { return (Resource == NULL)? FLeftFrameWidth: Resource->LeftFrameWidth; }
//------------------------------------------------------------------------------
int __fastcall TRoundFormEx::GetRightFrameWidth(void) { return (Resource == NULL)? FRightFrameWidth: Resource->RightFrameWidth; }
//------------------------------------------------------------------------------
Graphics::TBitmap* __fastcall TRoundFormEx::TopBmp(void)        { return (Resource == NULL)? FTopBmp: Resource->CaptionGraph; }
//------------------------------------------------------------------------------
Graphics::TBitmap* __fastcall TRoundFormEx::LeftBmp(void)       { return (Resource == NULL)? FLeftBmp: Resource->LeftGraph; }
//------------------------------------------------------------------------------
Graphics::TBitmap* __fastcall TRoundFormEx::RightBmp(void)      { return (Resource == NULL)? FRightBmp: Resource->RightGraph; }
//------------------------------------------------------------------------------
Graphics::TBitmap* __fastcall TRoundFormEx::BottomBmp(void)     { return (Resource == NULL)? FBottomBmp: Resource->BottomGraph; }
//------------------------------------------------------------------------------
Graphics::TBitmap* __fastcall TRoundFormEx::MinBtnBmp(void)     { return (Resource == NULL)? FMinBtnBmp: Resource->MinimizeGraph; }
//------------------------------------------------------------------------------
Graphics::TBitmap* __fastcall TRoundFormEx::MaxBtnBmp(void)     { return (Resource == NULL)? FMaxBtnBmp: Resource->MaximizeGraph; }
//------------------------------------------------------------------------------
Graphics::TBitmap* __fastcall TRoundFormEx::RestoreBtnBmp(void) { return (Resource == NULL)? FRestoreBtnBmp: Resource->RestoreGraph; }
//------------------------------------------------------------------------------
Graphics::TBitmap* __fastcall TRoundFormEx::CloseBtnBmp(void)   { return (Resource == NULL)? FCloseBtnBmp: Resource->CloseGraph; }
//------------------------------------------------------------------------------
Graphics::TBitmap* __fastcall TRoundFormEx::LockBtnBmp(void)    { return (Resource == NULL)? FLockBtnBmp: Resource->LockGraph; }
//------------------------------------------------------------------------------
Graphics::TBitmap* __fastcall TRoundFormEx::LockedBtnBmp(void)  { return (Resource == NULL)? FLockedBtnBmp: Resource->LockedGraph; }
//------------------------------------------------------------------------------
Graphics::TBitmap* __fastcall TRoundFormEx::IconBmp(void)       { return (Resource == NULL)? FIconBmp: Resource->IconGraph; }
//------------------------------------------------------------------------------
namespace Roundformex
{
	void __fastcall PACKAGE Register()
	{
		 TComponentClass classes[1] = {__classid(TRoundFormEx)};
		 RegisterComponents(L"MD", classes, 0);
	}
}
//---------------------------------------------------------------------------
