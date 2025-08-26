//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <stdlib.h>
#include <memory>
#include <math.h>
#include "StockChart.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//---------------------------------------------------------------------------
const int BORDER_PIXEL     = 5;
const int MIN_MAX_PX_BASE  = 20;///< 1/20 equals 5%
const int MAX_QTY_INIT     = 50;
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TStockChart *)
{
		new TStockChart(NULL);
}
//---------------------------------------------------------------------------
__fastcall TMinutesTickInfo::TMinutesTickInfo( int* Array, int DecimalLocator )
:Hour( Array[0]/1000000)
,Minutes( (Array[0]%1000000)/10000 )
,Qty( Array[1] )
{
	double divided = pow( 10.0, (double)DecimalLocator );

	MaxPx   = ((double)Array[2])/divided;
	MinPx   = ((double)Array[3])/divided;
	OpenPx  = ((double)Array[4])/divided;
	ClosePx = ((double)Array[5])/divided;
}
//---------------------------------------------------------------------------
__fastcall TMinutesTickInfo::TMinutesTickInfo( TMinuteKInfo& KInfo )
:Hour( KInfo.Hour )
,Minutes( KInfo.Minutes )
,Qty( KInfo.Qty )
,MaxPx( KInfo.MaxPx )
,MinPx( KInfo.MinPx )
,OpenPx( KInfo.OpenPx )
,ClosePx( KInfo.ClosePx )
{

}
//---------------------------------------------------------------------------
void __fastcall TMinutesTickInfo::Reset( void  )
{
	MaxPx   = -1;
	MinPx   = -1;
	OpenPx  = -1;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
__fastcall TTextHint::TTextHint( TWinControl* Owner, TFont* font )
:TCustomHintWindow( Owner )
,FText( "---" )
,FWidth( 80 )
,FHeight( 20 )
{
	FFormats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
	Parent     = Owner;
	Font->Assign( font );
	Font->Size = font->Size;
	Canvas->Font->Size = font->Size;
}
//---------------------------------------------------------------------------
void __fastcall TTextHint::Init( TColor BK, TColor FNT, const String& MaxStr )
{
	FColor   = FNT;
	FBKColor = BK;
	Color    = BK;
	FWidth   = Canvas->TextWidth( MaxStr ) + 6;
	FHeight  = Canvas->TextHeight( MaxStr ) + 4;
	ChangeStyle(  );
}
//---------------------------------------------------------------------------
void __fastcall TTextHint::SetMaxWidth( const String& MaxStr)
{
	FWidth   = Canvas->TextWidth( MaxStr ) + 6;
}
//---------------------------------------------------------------------------
void __fastcall TTextHint::ChangeStyle( void )
{
	long   ExStyle,Style;

	Style = ::GetWindowLong( Handle, GWL_STYLE);
	SetWindowLong( Handle, GWL_STYLE, Style & ~WS_VISIBLE );
	///< Set alpha value.
	ExStyle = GetWindowLong( Handle, GWL_EXSTYLE);
	SetWindowLong( Handle, GWL_EXSTYLE, ExStyle|WS_EX_LAYERED );
	::SetLayeredWindowAttributes( Handle, RGB(0,0,0), 200, LWA_ALPHA );
}
//---------------------------------------------------------------------------
void __fastcall TTextHint::SetVisible( void )
{
	long   ExStyle,Style;

	Style = ::GetWindowLong( Handle, GWL_STYLE);
	SetWindowLong( Handle, GWL_STYLE, Style & ~WS_VISIBLE );
	///< Set alpha value.
	ExStyle = GetWindowLong( Handle, GWL_EXSTYLE);
	SetWindowLong( Handle, GWL_EXSTYLE, ExStyle|WS_EX_LAYERED );
	::SetLayeredWindowAttributes( Handle, RGB(0,0,0), 200, LWA_ALPHA );
	Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TTextHint::SetPosition( int x, int y )
{
	SetBounds( x , y, FWidth, FHeight );
}
//---------------------------------------------------------------------------
void __fastcall TTextHint::SetString( const String& Msg )
{
	FText = Msg;
}
//---------------------------------------------------------------------------
void __fastcall TTextHint::Paint( void )
{
	TRect MsgRect( 3,0,FWidth-3, FHeight );

	Canvas->Pen->Color   = FColor;
	Canvas->Font->Color  = FColor;
	Canvas->Brush->Color = FBKColor;
	Canvas->Rectangle( 0,0,FWidth, FHeight  );
	Canvas->TextRect( MsgRect, FText, FFormats );
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
__fastcall TOrderHint::TOrderHint( TWinControl* Owner, TFont* font )
:TCustomHintWindow( Owner )
,FText( "---" )
,FWidth( 80 )
,FHeight( 20 )
{
	FFormats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
	Parent     = Owner;
	Font->Size = font->Size;
	Canvas->Font->Size = font->Size;
}
//---------------------------------------------------------------------------
void __fastcall TOrderHint::Init( int W, int H )
{
	FWidth   = W;
	FHeight  = H;
	FHalfWidth = W/2;
	ChangeStyle(  );
}
//---------------------------------------------------------------------------
void __fastcall TOrderHint::ChangeStyle( void )
{
	long   ExStyle,Style;

	Style = ::GetWindowLong( Handle, GWL_STYLE);
	SetWindowLong( Handle, GWL_STYLE, Style & ~WS_VISIBLE );
	///< Set alpha value.
	ExStyle = GetWindowLong( Handle, GWL_EXSTYLE);
	SetWindowLong( Handle, GWL_EXSTYLE, ExStyle|WS_EX_LAYERED );
	::SetLayeredWindowAttributes( Handle, RGB(0,0,0), 250, LWA_ALPHA );
}
//---------------------------------------------------------------------------
void __fastcall TOrderHint::SetVisible( void )
{
	long   ExStyle,Style;

	Style = ::GetWindowLong( Handle, GWL_STYLE);
	SetWindowLong( Handle, GWL_STYLE, Style & ~WS_VISIBLE );
	///< Set alpha value.
	ExStyle = GetWindowLong( Handle, GWL_EXSTYLE);
	SetWindowLong( Handle, GWL_EXSTYLE, ExStyle|WS_EX_LAYERED );
	::SetLayeredWindowAttributes( Handle, RGB(0,0,0), 250, LWA_ALPHA );
	Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TOrderHint::SetPosition( int x, int y )
{
	SetBounds( x , y, FWidth, FHeight );
}
//---------------------------------------------------------------------------
void __fastcall TOrderHint::SetString( const String& Msg )
{
	FText = Msg;
}
//---------------------------------------------------------------------------
void __fastcall TOrderHint::SetMaxWidth( int MaxW )
{
	FWidth   = MaxW;
}
//---------------------------------------------------------------------------
void __fastcall TOrderHint::Paint( void )
{
	TRect MsgRect( 3,0,FWidth-3, FHeight );

	Canvas->Pen->Color   = clRed;
	Canvas->Brush->Color = clRed;
	Canvas->Rectangle( 0,0,FHalfWidth, FHeight  );
	Canvas->Pen->Color   = clGreen;
	Canvas->Brush->Color = clGreen;
	Canvas->Rectangle( FHalfWidth,0,FWidth, FHeight  );
	Canvas->Font->Color  = clWhite;
	Canvas->Brush->Style = bsClear;
	Canvas->TextRect( MsgRect, FText, FFormats );
}
//---------------------------------------------------------------------------
const int HINT_H     = 65;

int HINT_W     = 110;
const int TEXT_TOP  = 23;
int INFOHINT_H = 155;
int TEXT_LEFT = 24;
int TEXT_H    = 20;
const POINT gRPoints[]={{HINT_W,0},{HINT_W-50,40},{HINT_W-25,40},{HINT_W,0}};
const POINT gLPoints[]={{0,0},{50,40},{25,40},{0,0}};
//---------------------------------------------------------------------------
__fastcall TInfoHint::TInfoHint( TWinControl* TheOwner)
:TCustomHintWindow( TheOwner )
,FHour(8)
,FMinutes(45)
,FOpenPx(0)
,FBullPx(0)
,FBearPx(0)
,FClosePx(0)
,FQty(0)
,FPxDigi(0)
,FIsEmpty( false)
,FList( NULL )
{
	Parent     = TheOwner;
	Color      = (TColor)RGB(200,200,200);
	Font->Size = 10;
	Canvas->Font->Size = 10;
	TEXT_H = Canvas->TextHeight( L"時間:開高低收量" ) + 2;
	HINT_W = Canvas->TextWidth( L"開:1234567.89" ) + 10;
	INFOHINT_H = 6* TEXT_H + TEXT_TOP;
}
//---------------------------------------------------------------------------
void __fastcall TInfoHint::ChangeShape( void )
{
	const POINT LPoints[]={{0,0},{50,40},{25,40},{0,0}};
	HRGN   TriRgn,WinRgn;
	long   ExStyle,Style;

	Style = ::GetWindowLong( Handle, GWL_STYLE);
	SetWindowLong( Handle, GWL_STYLE, Style & ~WS_VISIBLE );
	///< Set alpha value.
	ExStyle = GetWindowLong( Handle, GWL_EXSTYLE);
	SetWindowLong( Handle, GWL_EXSTYLE, ExStyle|WS_EX_LAYERED );
	::SetLayeredWindowAttributes( Handle, RGB(0,0,0), 180, LWA_ALPHA );
	///< Set window rgn.
	TriRgn = CreatePolygonRgn( (tagPOINT*)LPoints, 4, ALTERNATE );
	WinRgn = CreateRectRgn( 20, 20, HINT_W, INFOHINT_H );
	CombineRgn( WinRgn, WinRgn, TriRgn, RGN_OR);
	SetWindowRgn( Handle , WinRgn , true );
	DeleteObject( TriRgn );
}
//---------------------------------------------------------------------------
void __fastcall TInfoHint::SetVisible( void )
{
	long   ExStyle,Style;

	Style = ::GetWindowLong( Handle, GWL_STYLE);
	SetWindowLong( Handle, GWL_STYLE, Style & ~WS_VISIBLE );
	///< Set alpha value.
	ExStyle = GetWindowLong( Handle, GWL_EXSTYLE);
	SetWindowLong( Handle, GWL_EXSTYLE, ExStyle|WS_EX_LAYERED );
	::SetLayeredWindowAttributes( Handle, RGB(0,0,0), 180, LWA_ALPHA );
	Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TInfoHint::EraseBK( TMessage &Msg )
{
	HDC    DC = (HDC)Msg.WParam;
	HRGN   TriRgn,WinRgn;
	HBRUSH Brush,BrushFrame;
	const POINT LPoints[]={{0,0},{50,40},{25,40},{0,0}};

	Brush       = CreateSolidBrush( Color );
	BrushFrame  = CreateSolidBrush( clNavy );
	TriRgn      = CreatePolygonRgn( (tagPOINT*)LPoints, 4, ALTERNATE );
	WinRgn      = CreateRectRgn( 20, 20, HINT_W, INFOHINT_H );

	CombineRgn( WinRgn, WinRgn, TriRgn, RGN_OR); // polygon-filling mode
	FillRgn( DC, WinRgn, Brush ); 	///< Paint background.
	FrameRgn( DC, WinRgn, BrushFrame, 5, 5 );
	DeleteObject( Brush );
	DeleteObject( BrushFrame );
	DeleteObject( TriRgn );
	DeleteObject( WinRgn );
}
//---------------------------------------------------------------------------
void __fastcall TInfoHint::WndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case WM_ERASEBKGND:	EraseBK( Msg );
							break;
		case WM_CREATE:     ChangeShape( );
							break;
	}
	inherited::WndProc( Msg );
}
//---------------------------------------------------------------------------
void __fastcall TInfoHint::SetPosition( int x, int y )
{
	SetBounds( x , y, HINT_W, INFOHINT_H );
}
//---------------------------------------------------------------------------
void __fastcall TInfoHint::Paint( void )
{
	String Msg;
	TRect  Box( TEXT_LEFT, TEXT_TOP, HINT_W, INFOHINT_H );
	int T_LEFT= Canvas->TextWidth( L"時間:" ) + 2;
	int V_LEFT= Canvas->TextWidth( L"開:" ) + 2;

	Canvas->Font->Color  = clBlack;
	Canvas->Brush->Color = Color;
	Canvas->FillRect( Box );
	Canvas->TextOut( TEXT_LEFT,TEXT_TOP ,L"時間:");
	Canvas->TextOut( TEXT_LEFT,TEXT_TOP+1*TEXT_H ,L"開:");
	Canvas->TextOut( TEXT_LEFT,TEXT_TOP+2*TEXT_H ,L"高:");
	Canvas->TextOut( TEXT_LEFT,TEXT_TOP+3*TEXT_H ,L"低:");
	Canvas->TextOut( TEXT_LEFT,TEXT_TOP+4*TEXT_H ,L"收:");
	Canvas->TextOut( TEXT_LEFT,TEXT_TOP+5*TEXT_H ,L"量:");
	if( FIsEmpty == false )
	{
		Msg.printf( L"%02d:%02d", FHour,FMinutes);
		Canvas->TextOut( TEXT_LEFT+ T_LEFT,TEXT_TOP ,Msg);
		FList->ToTNFormat( FOpenPx, Msg );
		Canvas->TextOut( TEXT_LEFT+ V_LEFT,TEXT_TOP+1*TEXT_H ,Msg);
		FList->ToTNFormat( FBullPx, Msg );
		Canvas->TextOut( TEXT_LEFT+ V_LEFT,TEXT_TOP+2*TEXT_H ,Msg);
		FList->ToTNFormat( FBearPx, Msg );
		Canvas->TextOut( TEXT_LEFT+ V_LEFT,TEXT_TOP+3*TEXT_H ,Msg);
		FList->ToTNFormat( FClosePx, Msg );
		if( FClosePx > FOpenPx )
			Canvas->Font->Color  = clRed;
		else if( FClosePx < FOpenPx )
			Canvas->Font->Color  = clGreen;
		else
			Canvas->Font->Color  = clBlack;
		Canvas->TextOut( TEXT_LEFT+ V_LEFT,TEXT_TOP+4*TEXT_H ,Msg);
		Msg.printf( L"%d", FQty );
		Canvas->TextOut( TEXT_LEFT+ V_LEFT,TEXT_TOP+5*TEXT_H ,Msg);
	}
	else
	{
		Canvas->TextOut( TEXT_LEFT+ T_LEFT,TEXT_TOP ,L"--:--");
		Canvas->TextOut( TEXT_LEFT+ V_LEFT,TEXT_TOP+1*TEXT_H ,L"-----");
		Canvas->TextOut( TEXT_LEFT+ V_LEFT,TEXT_TOP+2*TEXT_H ,L"-----");
		Canvas->TextOut( TEXT_LEFT+ V_LEFT,TEXT_TOP+3*TEXT_H ,L"-----");
		Canvas->TextOut( TEXT_LEFT+ V_LEFT,TEXT_TOP+4*TEXT_H ,L"-----");
		Canvas->TextOut( TEXT_LEFT+ V_LEFT,TEXT_TOP+5*TEXT_H ,L"-----");
	}
}
//---------------------------------------------------------------------------
void __fastcall TInfoHint::SetData( int h,int m, float Open,float Bull,float Bear,float Close,int Qty, int Digi )
{
	FIsEmpty = false;
	FHour    = h;
	FMinutes = m;
	FOpenPx  = Open;
	FBullPx  = Bull;
	FBearPx  = Bear;
	FClosePx = Close;
	FQty     = Qty;
	FPxDigi  = Digi;
}
//---------------------------------------------------------------------------
void __fastcall DrawGradientBar( HDC DC , TRect& dRect,TColor UpCol, TColor DownCol )
{
	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;

	vert [0] .x      = dRect.Left;
	vert [0] .y      = dRect.Top;
	vert [0] .Red    = GetRValue( (DWORD)UpCol)<<8;
	vert [0] .Green  = GetGValue( (DWORD)UpCol)<<8;
	vert [0] .Blue   = GetBValue( (DWORD)UpCol)<<8;
	vert [0] .Alpha  = 0x0000;

	vert [1] .x      = dRect.Right;
	vert [1] .y      = dRect.Bottom;
	vert [1] .Red    = GetRValue( (DWORD)DownCol)<<8;;
	vert [1] .Green  = GetGValue( (DWORD)DownCol)<<8;;
	vert [1] .Blue   = GetBValue( (DWORD)DownCol)<<8;;
	vert [1] .Alpha  = 0x0000;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill( DC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V );
}
//---------------------------------------------------------------------------
//
//
//---------------------------------------------------------------------------
__fastcall TTickHint::TTickHint( void )
:TObject()
,FFillPx(0)
,FQty(0)
,FPxDigi(0)
,FIsEmpty( false )
,FColor( clRed )
{
	FBuffer = new Graphics::TBitmap();
	FBuffer->Width  = HINT_W;
	FBuffer->Height = HINT_H;
	FBoundsRect = Rect(0,0,HINT_W,HINT_H);
}
//---------------------------------------------------------------------------
void __fastcall TTickHint::SetClip( TCanvas* canvas, int x, int y, bool IsRight )
{
	HRGN  TriRgn;

	if( IsRight == true )
	{
		TriRgn  = CreatePolygonRgn( (tagPOINT*)gRPoints, 4, ALTERNATE );  // polygon-filling mode
		FWinRgn = CreateRoundRectRgn( 0, 10, HINT_W - 10, HINT_H , 20, 20 );
		CombineRgn( FWinRgn, FWinRgn, TriRgn, RGN_OR);
		OffsetRgn( FWinRgn, x-HINT_W, y );
	}
	else
	{
		TriRgn  = CreatePolygonRgn( (tagPOINT*)gLPoints, 4, ALTERNATE );  // polygon-filling mode
		FWinRgn = CreateRoundRectRgn( 10, 10, HINT_W, HINT_H, 20, 20 );
		CombineRgn( FWinRgn, FWinRgn, TriRgn, RGN_OR);
		OffsetRgn( FWinRgn, x, y );
	}
	SelectClipRgn( canvas->Handle, FWinRgn );
	DeleteObject( TriRgn );
}
//---------------------------------------------------------------------------
void __fastcall TTickHint::RestetClip( TCanvas* canvas )
{
	SelectClipRgn( canvas->Handle, NULL );
	DeleteObject( FWinRgn );
}
//---------------------------------------------------------------------------
void __fastcall TTickHint::Paint(  int x, int y, int MaxX, TCanvas* canvas )
{
	String Msg;
	TRect  BKRect(0,0,HINT_W,65);
	bool   IsRight = false;

	if( FIsEmpty == true )
		return;
	if( MaxX - x < HINT_W )
		IsRight = true;
	DrawGradientBar( FBuffer->Canvas->Handle , BKRect, FColor, clBlack );
	FBuffer->Canvas->Font->Size   = 10;
	FBuffer->Canvas->Font->Color  = clWhite;//FTextColor;
	FBuffer->Canvas->Brush->Style = bsClear;
	if( FIsEmpty == false )
	{
		Msg.printf( L"%s:%5.*f", Mdcomponentstrings_MD_ORDERLIST_FILL_PX, FPxDigi, FFillPx );///<成交價
		FBuffer->Canvas->TextRect(TRect(14,16,HINT_W,36),14,16 ,Msg);
		Msg.printf( L"%s%d",Mdcomponentstrings_MD_CHART_FILL_QTY, FQty);//成交量:
		FBuffer->Canvas->TextRect(TRect(14,36,HINT_W,56),14,36,Msg);
	}
	else
	{
		Msg.printf( L"%s:-----", Mdcomponentstrings_MD_ORDERLIST_FILL_PX );///<成交價
		FBuffer->Canvas->TextRect(TRect(14,36,HINT_W,36),14,16 ,Msg);
		Msg.printf( L"%s-----", Mdcomponentstrings_MD_CHART_FILL_QTY ); //成交量:
		FBuffer->Canvas->TextRect(TRect(14,36,HINT_W,56),14,36,Msg);
	}
	SetClip( canvas, x,y, IsRight );
	if( IsRight )
	{
		canvas->Draw( x - HINT_W, y, FBuffer, 200 );
		FBoundsRect = Rect( x - HINT_W, y, x, y + HINT_H);
	}
	else
	{
		canvas->Draw( x, y, FBuffer, 200 );
		FBoundsRect = Rect( x, y, x + HINT_W, y + HINT_H);
	}
	RestetClip( canvas );

}
//---------------------------------------------------------------------------
void __fastcall TTickHint::SetData( float FillPx, int Qty, int Digi, TColor Col )
{
	FIsEmpty   = false;
	FFillPx    = FillPx;
	FQty       = Qty;
	FPxDigi    = Digi;
	FColor     = Col;
}
//---------------------------------------------------------------------------
//
// Class TStockChart
// componect to draw 1 minutes American chart
//
//---------------------------------------------------------------------------
__fastcall TStockChart::TStockChart(TComponent* Owner)
: TCustomControl(Owner)
,FStrikePxDigi( 0 )
,FRefPrice( 7300 )
,FTickCount( 1022 )
,FLastMouseX( -1 )
,FLastMouseY(-1)
,FLastIndex( 0 )
,FQtyMax( MAX_QTY_INIT )
,FIndexMax( 0 )
,FLastFillQty( 0 )
,FHintwin( NULL )
,FTickHintwin( NULL )
,FShowFillHint( false )
,FDecimalLocator( 2 )
,FFrontColor( clBlack )
,FBullColor( clRed )
,FBearColor( clGreen )
,FGridColor( clGray )
{
	BasicInformation Info( "Symbol=TXFX9|Exchange=TAIFEX|EndDate=20091221|MKT=0|RefPx=7689|BullPx=8227|BearPx=7151|DecimalLocator=2" );

	this->DoubleBuffered = false;
	Width  = 200;
	Height = 150;
	FBuffer = new Graphics::TBitmap();
	OnMouseEnter = HandleMouseEnter;
	OnMouseLeave = HandleMouseLeave;
	SetTime( 8, 45, 13, 45 ); ///< From 8:45 To 13:45
	///< Generate new ticks
	FTickCount  = GenerateTicks( "TAIFEX","TXFX9",8227,7151,FTickArray );
	///< Map reference price to tick index.
	for( register int i = 0; i < FTickCount; i++ )
	{
		 int IntTickPx = UFC::DoubleToInt( FTickArray[ i ], 3 );
		 FPxToTickIndexMap[ IntTickPx ] = i;
	}
	int  IndexOffset = ( FTickCount + MIN_MAX_PX_BASE - 1 )/MIN_MAX_PX_BASE; ///< 5% of total ticks
	FRefPrice     = 7689;
	FRefPxIndex   = PxToTickIndex( 7689 );
	FMaxPxIndex   = FRefPxIndex - IndexOffset;
	FMinPxIndex   = FRefPxIndex + IndexOffset;
	if( FMaxPxIndex < 0 )
		FMaxPxIndex = 0;
	if( FMinPxIndex >= FTickCount )
		FMinPxIndex = FTickCount - 1;
	FTickHintwin = new TTickHint( );
	ClearData();
}
//---------------------------------------------------------------------------
__fastcall TStockChart::~TStockChart( void )
{
	if( FHintwin != NULL )
		delete FHintwin;
	if(	FTickHintwin != NULL )
		delete FTickHintwin;
	delete FBuffer;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::SetPenStyle( TCustomCanvas* aCanvas, TPenStyle PenStyle )
{
	((TCanvas*)aCanvas)->Pen->Style = PenStyle;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::SetPenColor( TCustomCanvas* aCanvas, TColor PenColor )
{
	((TCanvas*)aCanvas)->Pen->Color = PenColor;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::SetPenWidth( TCustomCanvas* aCanvas, int PenWidth )
{
	((TCanvas*)aCanvas)->Pen->Width = PenWidth;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::SetCanvasFontColor( TCustomCanvas* aCanvas, TColor Color )
{
	((TCanvas*)aCanvas)->Font->Color = Color;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::ClearData( void )
{
	for( int i = 0; i< FMaxMinutesIndex; i++ )
	{
		 FHaveData[ i ] = false;
		 FQtys[ i ] = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::SetData( TMinutesTickInfo& TickInfo, bool IsTick, bool NeedPaint )
{
	int  HourDiff      = TickInfo.Hour - FBeginHour;
	int  Index         = HourDiff * 60 - FBeginMinute + TickInfo.Minutes;
	bool BoundsChange  = false;

	if( Index >= 0 && Index < FMaxMinutesIndex )
	{
		if( FHaveData[Index] == true && NeedPaint == true  )
			ErasePriceLine( Index, Canvas );
		if( IsTick == true )
		{
			int OrigQty = FQtys[Index];

			FLastFillQty = TickInfo.Qty;
			FQtys[Index] = OrigQty + FLastFillQty;
		}
		else
			FQtys[Index] = TickInfo.Qty;
		FMaxPx[Index]    = PxToTickIndex( TickInfo.MaxPx );
		FMinPx[Index]    = PxToTickIndex( TickInfo.MinPx );
		FOpenPx[Index]   = PxToTickIndex( TickInfo.OpenPx );
		FClosePx[Index]  = PxToTickIndex( TickInfo.ClosePx );
		FHaveData[Index] = true;
		if( Index > FIndexMax )
			FIndexMax = Index;
		FLastIndex = Index;
		if( TickInfo.Qty > FQtyMax ) ///< Check the max Qty
		{
			FQtyMax = TickInfo.Qty;
			BoundsChange = true;
		}
		if( FMaxPx[ Index ]  < FMaxPxIndex )
		{
			FMaxPxIndex  = FMaxPx[Index];
			BoundsChange = true;
		}
		if( FMinPx[ Index ] > FMinPxIndex )
		{
			FMinPxIndex  = FMinPx[Index];
			BoundsChange = true;
		}
		if( BoundsChange == true ) ///< Bounds changed, redraw all
		{
			CalculateDimension( );
			if( NeedPaint == true )
			{
				ShowTickHint( Index, false );
				Paint();
			}
		}
		else ///< Only update this tick
		{
			if( NeedPaint == true )
			{
				DrawPriceLine( Index, Canvas ); ///< Draw the price line of this tick.
				DrawVolumeBar( Index, Canvas ); ///< Draw the volume line of this tick.
				DrawTickHintRect( Index );            ///< Erase the old hint window.
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::ChangeCursor( void )
{
	std::auto_ptr<Graphics::TBitmap> bmpMask( new Graphics::TBitmap );
	std::auto_ptr<TIconInfo>         iconInfo( new TIconInfo );

	bmpMask->Width  = 1;
	bmpMask->Height = 2;
	bmpMask->PixelFormat = pf1bit;
	iconInfo->fIcon = false;
	iconInfo->xHotspot = 0;
	iconInfo->yHotspot = 0;
    iconInfo->hbmMask  = bmpMask->Handle;
    iconInfo->hbmColor = NULL;//bmpColor->Handle;
    Screen->Cursors[5] = CreateIconIndirect(iconInfo.get());
    Cursor = TCursor( 5 );
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::Loaded( void )
{
	TCustomControl::Loaded();
	ChangeCursor();
	CalculateDimension( );
}
//---------------------------------------------------------------------------
int __fastcall TStockChart::PxToTickIndex( double Price )
{
	int IntTickPx =  UFC::DoubleToInt( Price, 3 );

	if( IntTickPx == 0 )
		return -1;
	std::hash_map<int,int>::iterator Iterator = FPxToTickIndexMap.find( IntTickPx );
	if( Iterator == FPxToTickIndexMap.end() )///< Not found
	{
		String Msg;
		Msg.printf( L"Price %5.*f not in tick price list", FStrikePxDigi, Price );
		throw( Exception( Msg ) );
	}
	else
		return Iterator->second;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::SetSymbol( BasicInformation* SymbolInfo )
{
	///< Clear old ticks and map
	ClearData();
	FTickArray.clear();
	FPxToTickIndexMap.clear();
	///< set StrikePxDigi and DecimalLocator.
	FDecimalLocator = SymbolInfo->GetDecimalLocator();
	FStrikePxDigi   = SymbolInfo->GetDigit( );
	///< Generate new ticks
	FTickCount  = GenerateTicks( SymbolInfo->GetExchange(),
								 SymbolInfo->GetSymbol(),
								 SymbolInfo->GetBullPrice(),
								 SymbolInfo->GetBearPrice(),
								 FTickArray );
	///< Map reference price to tick index.
	for( register int i = 0; i < FTickCount; i++ )
	{
		 int IntTickPx = UFC::DoubleToInt( FTickArray[ i ], 3 );
		 FPxToTickIndexMap[ IntTickPx ] = i;
	}
	int  IndexOffset = ( FTickCount + MIN_MAX_PX_BASE - 1 )/MIN_MAX_PX_BASE; ///< 5% of total ticks

	FRefPrice     = SymbolInfo->GetRefPrice();
	FRefPxIndex   = PxToTickIndex( FRefPrice );
	FMaxPxIndex   = FRefPxIndex - IndexOffset;
	FMinPxIndex   = FRefPxIndex + IndexOffset;
	if( FMaxPxIndex < 0 )
		FMaxPxIndex = 0;
	if( FMinPxIndex >= FTickCount )
		FMinPxIndex = FTickCount - 1;
	FQtyMax     = MAX_QTY_INIT;
	FLastMouseX = -1;
	FLastMouseY = -1;
	FLastIndex  = 0;
	FTickHintwin->Empty();
	CalculateDimension( );
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::SetTime( int OpenHours, int OpenMinutes, int CloseHours, int CloseMinutes )
{
	int HourDiff = CloseHours - OpenHours;

	if( OpenHours >= CloseHours )
		throw(Exception("Open market time should less than close time"));
	FBeginHour       = OpenHours;
	FBeginMinute     = OpenMinutes;
	FEndHour         = CloseHours;
	FEndMinute       = CloseMinutes;
	FMaxMinutesIndex = HourDiff * 60 - FBeginMinute + FEndMinute; ///< How many minutes.
	FQtys.Length     = FMaxMinutesIndex;
	FMaxPx.Length    = FMaxMinutesIndex;
	FMinPx.Length    = FMaxMinutesIndex;
	FOpenPx.Length   = FMaxMinutesIndex;
	FClosePx.Length  = FMaxMinutesIndex;
	FHaveData.Length = FMaxMinutesIndex;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::Resize( void )
{
	TCustomControl::Resize();

	if( FBuffer->Width  != Width )
		FBuffer->Width  = Width;
	if( FBuffer->Height != Height )
		FBuffer->Height = Height;
	CalculateDimension();
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::CalculateDimension( void )
{
	int   TextHeight     = Canvas->TextHeight( "123456789.0" );
	int   LeftX          = Canvas->TextWidth("10000.00") + BORDER_PIXEL * 2;///< Border Px Border
	int   TopBorderSpec  = TextHeight / 2;
	int   BarChartHeight = ( Height - 5 * TopBorderSpec) / 4;
	int   BarChartWidth  = Width - LeftX - BORDER_PIXEL;
	float TickStepMin    = (float)TextHeight*1.5;
	float PixelsPerTickGrid;

	//< Calculate the basic chart dimensions.( No ref to Px )
	//<  3/4 Line chart.
	//<  1/4 Bar chart.
	FLineChartRect     = TRect( LeftX,TopBorderSpec,Width-BORDER_PIXEL,3*BarChartHeight+TopBorderSpec );
	FBarChartRect      = TRect( LeftX,3*BarChartHeight+2*TopBorderSpec,Width-BORDER_PIXEL,Height-3*TopBorderSpec);
	//< Calculate parems ref to px.
	FPixelPerMinute    = (float)BarChartWidth/(float)FMaxMinutesIndex;
	FPixelPerTick      = (float)FLineChartRect.Height()/(float)(FMinPxIndex - FMaxPxIndex);
	FRefPxYOffset      = (int)((float)(FRefPxIndex-FMaxPxIndex) * FPixelPerTick);
	///< FTicksPerGrid, A grid equals N ticks, at least 5 tick,
	///< FPixelsPerTickGrid, A grid's height.(in pixel)
	for( FTicksPerGrid = 5; ; FTicksPerGrid += 5 )
	{
		PixelsPerTickGrid = (float)FTicksPerGrid * FPixelPerTick;
		if( PixelsPerTickGrid > TickStepMin )
			break;
	}
	FPixelsPerTickGrid = (int)PixelsPerTickGrid;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::DrawChartBorder(  TCustomCanvas *canvas  )
{
	 // Draw Bar&Line chart rectangle
	 SetPenStyle( canvas, psSolid );
	 SetPenWidth( canvas, 1 );
	 SetPenColor( canvas, FFrontColor );
	 canvas->Rectangle( FLineChartRect );
	 canvas->Rectangle( FBarChartRect );
	 // Draw Ref Price Line ( Y = 0 )
	 canvas->MoveTo( FLineChartRect.Left ,FLineChartRect.Top + FRefPxYOffset );
	 canvas->LineTo( FLineChartRect.Right,FLineChartRect.Top + FRefPxYOffset );
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::DrawChartTimeGrid(  TCustomCanvas *canvas  )
{
	 int PixelPerHour  = FPixelPerMinute * 60;
	 int PixelHalfHour = FPixelPerMinute * 30;
	 int LineHour      = FBeginHour + 1;
	 int XOffset;
	 String TimeStr;

     if( FBeginMinute == 0 )
        XOffset = FLineChartRect.Left + PixelPerHour;
     else
        XOffset = FLineChartRect.Left + FPixelPerMinute * (60-FBeginMinute);
	 SetPenStyle( canvas, psSolid );
	 SetPenWidth( canvas, 1 );
	 SetPenColor( canvas, FGridColor );
	 while( XOffset < FLineChartRect.Right )
	 {
///		  SetPenColor( canvas, FGridColor );
		  canvas->MoveTo( XOffset ,FLineChartRect.Top);
		  canvas->LineTo( XOffset ,FLineChartRect.Bottom );
		  canvas->MoveTo( XOffset ,FBarChartRect.Top);
		  canvas->LineTo( XOffset ,FBarChartRect.Bottom );
		  canvas->MoveTo( XOffset + PixelHalfHour, FLineChartRect.Top);
		  canvas->LineTo( XOffset + PixelHalfHour, FLineChartRect.Bottom );
		  canvas->MoveTo( XOffset + PixelHalfHour, FBarChartRect.Top);
		  canvas->LineTo( XOffset + PixelHalfHour, FBarChartRect.Bottom );
	      // Draw time label
          TimeStr.printf( L"%02d:00",LineHour );
		  canvas->TextOut( XOffset, FBarChartRect.Bottom + BORDER_PIXEL, TimeStr );
          XOffset += PixelPerHour;
          LineHour ++;
     }
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::DrawChartTickGrid(  TCustomCanvas *canvas  )
{
	 int    YOffset = FLineChartRect.Top;//FFirstTickYOffset + FLineChartRect.Top;
	 int    TickPxIndex  = FMaxPxIndex;//FFirstTickPx;
	 int    TextHeight = canvas->TextHeight("123456789");
	 int    HalfTextHeight = TextHeight/2;
	 int    LineCount = 0;
	 String PxStr;

	 SetPenStyle( canvas, psDot );
	 SetPenWidth( canvas, 1 );
	 SetPenColor( canvas, FGridColor );
	 ///<  Draw Tick lines
	 while( YOffset < FLineChartRect.Bottom )
     {
          canvas->MoveTo( FLineChartRect.Left ,YOffset);
          canvas->LineTo( FLineChartRect.Right,YOffset);
		  // Draw price label
		  PxStr.printf( L"%5.*f", FStrikePxDigi, FTickArray[ TickPxIndex ] );
		  if( YOffset > FLineChartRect.Top + FRefPxYOffset + 2*HalfTextHeight || YOffset < FLineChartRect.Top + FRefPxYOffset - 2*HalfTextHeight )
		  {
			  if( TickPxIndex < FRefPxIndex )
			  {
				  SetCanvasFontColor( canvas, FBullColor);
				  canvas->TextOut( FLineChartRect.Left - canvas->TextWidth( PxStr ) -BORDER_PIXEL, YOffset -HalfTextHeight , PxStr );
			  }
			  else if( TickPxIndex > FRefPxIndex )
			  {
				  SetCanvasFontColor( canvas, FBearColor );
				  canvas->TextOut( FLineChartRect.Left - canvas->TextWidth( PxStr ) -BORDER_PIXEL, YOffset -HalfTextHeight , PxStr );
			  }
		  }
		  YOffset += FPixelsPerTickGrid;
		  LineCount ++;
		  TickPxIndex +=  FTicksPerGrid;
	 }
	 // Draw bar chart H lines
	 float QtyPerPixels  = ((float)FQtyMax*1.1)/(float)FBarChartRect.Height();
	 int   QtyPerGrid    = QtyPerPixels*(float)FPixelsPerTickGrid;
	 int   DrawQty       = QtyPerGrid;

	 SetPenColor( canvas, FFrontColor );
	 YOffset = FBarChartRect.Bottom - FPixelsPerTickGrid;
	 while( YOffset > FBarChartRect.Top )
	 {
		  canvas->MoveTo( FBarChartRect.Left ,YOffset );
		  canvas->LineTo( FBarChartRect.Right,YOffset );
		  // Draw time label
		  PxStr.printf( L"%d", DrawQty );
		  canvas->TextOut( FBarChartRect.Left - canvas->TextWidth( PxStr ) -BORDER_PIXEL, YOffset -HalfTextHeight , PxStr );
		  DrawQty += QtyPerGrid;
		  YOffset -= FPixelsPerTickGrid;
	 }
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::DrawText( TCustomCanvas *canvas )
{
     int HalfTextHeight = canvas->TextHeight("123456")/2;
     AnsiString Price;

     // Draw Reference price label
	 SetCanvasFontColor( canvas, FFrontColor );
	 Price.sprintf( "%5.*f" , FStrikePxDigi, FRefPrice );
	 canvas->TextOut( FLineChartRect.Left - canvas->TextWidth( Price ) -BORDER_PIXEL, FLineChartRect.Top + FRefPxYOffset - HalfTextHeight,Price);
	 // Draw Bull price label
	 SetCanvasFontColor( canvas, FBullColor );
	 Price.sprintf( "%5.*f", FStrikePxDigi, FTickArray[ FMaxPxIndex ] );
	 canvas->TextOut( FLineChartRect.Left - canvas->TextWidth( Price ) -BORDER_PIXEL, 1, Price);
	 // Draw Bear price label
	 SetCanvasFontColor( canvas, FBearColor );
	 Price.sprintf( "%5.*f", FStrikePxDigi, FTickArray[ FMinPxIndex ] );
	 canvas->TextOut( FLineChartRect.Left - canvas->TextWidth( Price ) -BORDER_PIXEL, FLineChartRect.Bottom - HalfTextHeight,Price );
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::DrawPriceLine( TCustomCanvas *canvas )
{
	 SetPenStyle( canvas, psSolid );
	 if( FPixelPerMinute < 2.0 )
		 SetPenWidth( canvas, 1 );
	 else
		 SetPenWidth( canvas, (int)FPixelPerMinute - 1 );
	 for( register int i = 0; i < FMaxMinutesIndex; i++ )
		  DrawPriceLine( i, canvas, false  );
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::DrawPriceLine( int Index, TCustomCanvas *canvas, bool AdjPenWidth )
{
	 if( FHaveData[ Index ] == true )
	 {
		 int XOffset = FPixelPerMinute*Index + FLineChartRect.left;
		 int UpperY  = FLineChartRect.Top + ((FMaxPx[ Index ]   - FMaxPxIndex)*FPixelPerTick);
		 int LowerY  = FLineChartRect.Top + ((FMinPx[ Index ]   - FMaxPxIndex)*FPixelPerTick);
		 int CloseY  = FLineChartRect.Top + ((FClosePx[ Index ] - FMaxPxIndex)*FPixelPerTick);

		 if( AdjPenWidth == true )
		 {
			 SetPenStyle( canvas, psSolid );
			 if( FPixelPerMinute < 2.0 )
				 SetPenWidth( canvas, 1 );
			 else
				 SetPenWidth( canvas, (int)FPixelPerMinute-1 );
		 }
		 if( FOpenPx[Index] >= FClosePx[Index] ) ///< Price increasing
			 SetPenColor( canvas, FBullColor );
		 else
			 SetPenColor( canvas, FBearColor );
		 canvas->MoveTo( XOffset , LowerY );
		 canvas->LineTo( XOffset , UpperY );
		 canvas->MoveTo( XOffset , CloseY );
		 canvas->LineTo( XOffset+(int)FPixelPerMinute, CloseY );
	 }
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::ErasePriceLine( int Index, TCustomCanvas *canvas, bool AdjPenWidth )
{
	 if( FHaveData[ Index ] == true )
	 {
		 int XOffset = FPixelPerMinute*Index + FLineChartRect.left;
		 int UpperY  = FLineChartRect.Top + ((FMaxPx[ Index ]   - FMaxPxIndex)*FPixelPerTick);
		 int LowerY  = FLineChartRect.Top + ((FMinPx[ Index ]   - FMaxPxIndex)*FPixelPerTick);
		 int CloseY  = FLineChartRect.Top + ((FClosePx[ Index ] - FMaxPxIndex)*FPixelPerTick);

		 if( AdjPenWidth == true )
		 {
			 SetPenStyle( canvas, psSolid );
			 if( FPixelPerMinute < 2.0 )
				 SetPenWidth( canvas, 1 );
			 else
				 SetPenWidth( canvas, (int)FPixelPerMinute-1 );
		 }
		 if( FOpenPx[Index] >= FClosePx[Index] ) ///< Price increasing
			 SetPenColor( canvas, Color );
		 else
			 SetPenColor( canvas, Color );
		 canvas->MoveTo( XOffset , LowerY );
		 canvas->LineTo( XOffset , UpperY );
		 canvas->MoveTo( XOffset , CloseY );
		 canvas->LineTo( XOffset+(int)FPixelPerMinute, CloseY );
	 }
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::DrawVolumeBar( TCustomCanvas *canvas )
{
	 for( register int i = 0; i < FMaxMinutesIndex; i++ )
		  DrawVolumeBar( i, canvas );
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::DrawVolumeBar( int Index, TCustomCanvas *canvas )
{
	if( FHaveData[Index] == true )
	{
		float PixelsPerQty   = (float)FBarChartRect.Height()/((float)FQtyMax*1.1);
		int   UpperY,XOffset;
		int   ZeroY = FBarChartRect.Bottom-1;

		SetPenStyle( canvas, psSolid );
		if( FPixelPerMinute < 2.0 )
			SetPenWidth( canvas, 1 );
		else
			SetPenWidth( canvas, (int)FPixelPerMinute-1 );
		if( FOpenPx[Index] >= FClosePx[Index] ) ///< Price increasing
			SetPenColor( canvas, FBullColor );
		else
			SetPenColor( canvas, FBearColor );
		XOffset = FPixelPerMinute*Index + FBarChartRect.left;
		UpperY  = FBarChartRect.Bottom - FQtys[Index]*PixelsPerQty;
		canvas->MoveTo( XOffset , ZeroY );
		canvas->LineTo( XOffset , UpperY );
	 }
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::PaintGrid( TCanvas *canvas )
{
	 TRect BufferRect(0,0,Width,Height);

	 if( Focused() == false )
		 FBuffer->Canvas->Brush->Color = (TColor)RGB( GetRValue( Color)/2, GetGValue( Color)/2, GetBValue( Color)/2 );
	 else
		 FBuffer->Canvas->Brush->Color = Color;
	 FBuffer->Canvas->Font->Color  = FFrontColor;
	 FBuffer->Canvas->FillRect( BufferRect );
	 FBuffer->Canvas->Brush->Style = bsClear;
	 DrawChartBorder( FBuffer->Canvas );
	 DrawChartTimeGrid( FBuffer->Canvas );
	 DrawChartTickGrid( FBuffer->Canvas );
	 DrawText( FBuffer->Canvas );
	 DrawPriceLine( FBuffer->Canvas );
	 DrawVolumeBar( FBuffer->Canvas );
	 FTickHintwin->Paint(  FPixelPerMinute*FLastIndex + FLineChartRect.left, GetDataYPos( FLastIndex, 0 ), Width, FBuffer->Canvas );
/*	 if( !ComponentState.Contains( csDesigning ) && TDirect2DCanvas::Supported() )
	 {
		 TDirect2DCanvas* FD2DBufferCanvas = new TDirect2DCanvas( canvas, BufferRect );
		 FD2DBufferCanvas->BeginDraw();
		 FD2DBufferCanvas->Draw( 0, 0, FBuffer );
		 FD2DBufferCanvas->EndDraw();
		 delete FD2DBufferCanvas;
	 }
	 else*/
	 canvas->Draw( 0, 0, FBuffer );
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::DrawTickHintRect( int Index )
{
	 TRect BufferRect(0,0,Width,Height);
	 TRect PaintRect = FTickHintwin->GetRect();
	 HRGN  ClipRgn;

	 ClipRgn = CreateRectRgnIndirect( &PaintRect );
	 SelectClipRgn( FBuffer->Canvas->Handle, ClipRgn );
		 if( Focused() == false )
			 FBuffer->Canvas->Brush->Color = (TColor)RGB( GetRValue( Color)/2, GetGValue( Color)/2, GetBValue( Color)/2 );
		 else
			 FBuffer->Canvas->Brush->Color = Color;
		 FBuffer->Canvas->Font->Color  = FFrontColor;
		 FBuffer->Canvas->FillRect( BufferRect );
		 FBuffer->Canvas->Brush->Style = bsClear;
		 DrawChartBorder( FBuffer->Canvas );
		 DrawChartTimeGrid( FBuffer->Canvas );
		 DrawChartTickGrid( FBuffer->Canvas );
		 DrawText( FBuffer->Canvas );
		 DrawPriceLine( FBuffer->Canvas );
		 DrawVolumeBar( FBuffer->Canvas );
		 if( FHaveData[ Index ] )
		 {
			if( FOpenPx[Index] >= FClosePx[Index] ) ///< Price increasing
				FTickHintwin->SetData( FTickArray[ FClosePx[ Index ]], FLastFillQty, FStrikePxDigi, FBullColor );
			else
				FTickHintwin->SetData( FTickArray[ FClosePx[ Index ]], FLastFillQty, FStrikePxDigi, FBearColor );
			FTickHintwin->Paint(  FPixelPerMinute*Index + FLineChartRect.left, GetDataYPos( Index, 0 ), Width, FBuffer->Canvas );
		 }
		 if( FLastMouseX != -1 && FLastMouseY != -1 )
			 DrawCross( FBuffer->Canvas, FLastMouseX, FLastMouseY, false );
	 SelectClipRgn( FBuffer->Canvas->Handle, NULL );
	 DeleteObject( ClipRgn );
	 Canvas->CopyRect( PaintRect, FBuffer->Canvas, PaintRect);
}
//---------------------------------------------------------------------------
int __fastcall TStockChart::XPosToIndex( int X )
{
	 int   ChartWidth = FBarChartRect.Width();
	 float XRatio     = ( float )ChartWidth/(float)FMaxMinutesIndex;
	 int   Offset     = X - FBarChartRect.Left;

	 if( Offset < ChartWidth && Offset >= 0 )
	 {
		 int Index = (int)((float)(X - FBarChartRect.Left)/XRatio);
		 if( Index >= 0 && Index < FMaxMinutesIndex )
			 return Index;
	 }
	 return -1;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::DrawNotLine(TCanvas *canvas,int x,int y,int x1,int y1)
{
     canvas->MoveTo( x , y  );
	 canvas->LineTo( x1, y1 );
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::DrawCross( TCanvas* aCanvas, int X, int Y, bool NeedErase )
{
   if( aCanvas->TryLock() == true )
   {
	   TPenMode Mode = aCanvas->Pen->Mode;

	   aCanvas->Pen->Color = clWhite;
	   aCanvas->Pen->Mode  = pmNot;
	   aCanvas->Pen->Width = 2;
	   aCanvas->Pen->Style = psSolid;
	   if( NeedErase == true && FLastMouseX != -1 && FLastMouseY != -1 ) ///< Erease old cross
	   {
		  DrawNotLine( aCanvas, FLineChartRect.left,FLastMouseY, FLineChartRect.right,FLastMouseY );
		  DrawNotLine( aCanvas,FLastMouseX,FBarChartRect.Top,FLastMouseX,FBarChartRect.Bottom);
		  DrawNotLine( aCanvas,FLastMouseX,FLineChartRect.Top,FLastMouseX,FLineChartRect.Bottom);
	   }
	   if( X != -1 && Y != -1 )
	   {
		  DrawNotLine( aCanvas, FLineChartRect.left,  Y, FLineChartRect.right, Y );
		  DrawNotLine( aCanvas, X,FBarChartRect.Top,  X, FBarChartRect.Bottom);
		  DrawNotLine( aCanvas, X,FLineChartRect.Top, X, FLineChartRect.Bottom);
	   }
	   aCanvas->Pen->Mode = Mode;
	   FLastMouseX = X;
	   FLastMouseY = Y;
	   aCanvas->Unlock();
   }
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::IndexToTime( int i, int& h, int& m )
{
	if( i <= 0 )
	{
		h = FBeginHour;
		m = FBeginMinute;
		return;
	}
	if( i >= FMaxMinutesIndex )
	{
		h = FEndHour;
		m = FEndMinute;
		return;
	}
	int HDiff = i/60;
	int MDiff = i%60;
	h = FBeginHour + HDiff;
	m = FBeginMinute + MDiff;
	if( m > 60 )
	{
		h += m/60;
        m = m%60;
    }
}
//---------------------------------------------------------------------------
int __fastcall TStockChart::GetDataYPos( int Index, int Y )
{
	int   YOffset;

	if( FHaveData[ Index ] == false )
		return Y;
	if( Y >= FBarChartRect.top ) ///< in qty bar chart.
	{
		 float PixelsPerQty   = (float)FBarChartRect.Height()/((float)FQtyMax*1.1);

		 YOffset  = FBarChartRect.Bottom - FQtys[Index]*PixelsPerQty;
	}
	else ///< in line chart
		 YOffset = FLineChartRect.Bottom - (int)((float)(FMinPxIndex-FClosePx[Index])*FPixelPerTick );
	return YOffset;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::ShowTickHint( int Index, bool Paint )
{
	if( FHaveData[ Index ] )
	{
		if( FOpenPx[Index] >= FClosePx[Index] ) ///< Price increasing
			FTickHintwin->SetData( FTickArray[ FClosePx[ Index ]], FLastFillQty, FStrikePxDigi, FBullColor );
		else
			FTickHintwin->SetData( FTickArray[ FClosePx[ Index ]], FLastFillQty, FStrikePxDigi, FBearColor );
		if( Paint == true )
			FTickHintwin->Paint(  FPixelPerMinute*Index + FLineChartRect.left, GetDataYPos( Index, 0 ), Width, Canvas );
	}
	else
		FTickHintwin->Empty();
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::ShowInfoHint( int Index, TPoint& ScreenPx )
{
	if( !ComponentState.Contains( csDesigning ) )
	{
		int Hour,Minutes;

		if( FHintwin == NULL )
		{
			FHintwin = new TInfoHint( this );
			FHintwin->ChangeShape();
			FHintwin->SetPosition( ScreenPx.x , ScreenPx.y );
		}
		if( Visible == true )
		{
			if( FHaveData[ Index ] )
			{
				IndexToTime( Index, Hour, Minutes );
				FHintwin->SetData( Hour,Minutes,
								   FTickArray[ FOpenPx[ Index ] ],
								   FTickArray[ FMaxPx[ Index ] ],
								   FTickArray[ FMinPx[ Index ] ],
								   FTickArray[ FClosePx[ Index ]],
								   FQtys[ Index ],FStrikePxDigi );
			}
			else
				FHintwin->Empty();
			FHintwin->SetPosition( ScreenPx.x , ScreenPx.y );
			if( FHintwin->Visible == false )
				FHintwin->Show();
			else
				FHintwin->Paint();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::MouseMove( Classes::TShiftState Shift, int X, int Y)
{
	if( FLastMouseX == X && FLastMouseY == Y )
		return;

	int Index = XPosToIndex( X );

	if( Index != -1 && Y > FLineChartRect.top && Y < FBarChartRect.Bottom )
	{
		DrawCross( Canvas, X, Y ); ///< Draw new cross line
	 //	MoveHint( Index, X, Y );
	}
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::MoveHint( int Index, int X, int Y)
{
	int    DataY = GetDataYPos( Index, Y );
	TPoint ScreenPx( X, DataY );

	ScreenPx = ClientToScreen( ScreenPx );
	ShowInfoHint( Index, ScreenPx );
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::HandleMouseEnter( TObject*  )
{
	SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::HandleMouseLeave( TObject* )
{
	if( FLastMouseX != -1 )
		DrawCross( Canvas ); ///< Errase old cross line
	if( FHintwin != NULL )
		FHintwin->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::Paint( void )
{
	PaintGrid( Canvas );
	FLastMouseX = -1;
	FLastMouseY = -1;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::EraseBackground( TMessage &Msg )
{
	TCanvas *canvas = new TCanvas();
	canvas->Handle = (HDC) Msg.WParam ;
	PaintGrid( canvas );
	FLastMouseX = -1;
	FLastMouseY = -1;
	Msg.Result = 1;
	delete canvas;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::WndProc( TMessage &Msg )
{
	switch(  Msg.Msg )
	{
		case WM_ERASEBKGND: EraseBackground( Msg );
							return;
		case WM_KILLFOCUS:  if( FHintwin != NULL )
								FHintwin->Visible = false;
							Paint();
							break;
		case WM_SETFOCUS:   Paint();
							break;
	}
	inherited::WndProc( Msg );
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::SetFontColor( TColor Color )
{
	FFrontColor = Color;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::SetBullColor( TColor Color )
{
	FBullColor = Color;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::SetBearColor( TColor Color )
{
	FBearColor = Color;
}
//---------------------------------------------------------------------------
void __fastcall TStockChart::SetGridColor( TColor Color )
{
	FGridColor = Color;
}
//---------------------------------------------------------------------------
namespace Stockchart
{
	void __fastcall PACKAGE Register()
	{
	   TComponentClass classes[1] = {__classid(TStockChart)};
	   RegisterComponents(L"Speedy", classes, 0);
	}
}
//---------------------------------------------------------------------------
