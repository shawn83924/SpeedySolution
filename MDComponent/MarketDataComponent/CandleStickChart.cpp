//---------------------------------------------------------------------------
#include <vcl.h>
#include <Gdiplus.h>
#include <Vcl.Clipbrd.hpp>
#include <utility>
#pragma hdrstop
#include "CandleStickChart.h"
#include "MDComponentStrings.hpp"
#include "GraphUtility.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
const int MAX_CANDLE_VIEW_PORT = 320;
const int MIN_CANDLE_VIEW_PORT = 10;
//---------------------------------------------------------------------------
const int FONT_DEF_SIZE  = 11;
const PAINT_TIMER_ID = 100078;
//---------------------------------------------------------------------------
const int HINT_W = 140;
//const int HINT_H = 105;
const int HINT_H = 95;
const POINT gRPoints[]={{HINT_W,0},{HINT_W-60,40},{HINT_W-25,40},{HINT_W,0}};
const POINT gLPoints[]={{0,0},{60,40},{25,40},{0,0}};
const int AFTER_CLOSE = 60;
const int OFFHOUR_MINUTES = 60*14; ///< 15:00~5:00
const int SHADOW_WIDTH = 2;
const int ALIGN_DIST = 30;
//---------------------------------------------------------------------------
const int AOETicksPerGrid = 10; ///< AOE Price per grid fix to 10 ticks
const int AOEBoxHLDiff = 46; ///< AOE Box H/L max ticks.
//---------------------------------------------------------------------------
//
// Last Price Qty Time Hint
//
//---------------------------------------------------------------------------
__fastcall TCandleHint::TCandleHint()
:TObject()
,FFillPx(0)
,FLastFillPx( 0 )
,FQty(0)
,FPxDigi(0)
,FIsEmpty( false )
,FColor( clRed )
{
}
//---------------------------------------------------------------------------
void __fastcall TCandleHint::Paint( int x, int y, int MaxX, TCanvas* canvas, TColor FontCol )
{
	System::Uitypes::TFontStyles FStyle;
	int YOffset = 5;
	int XOffset = 5;

	FStyle.Clear();
	canvas->Font->Size   = 10;
	canvas->Font->Color  = FontCol;
	canvas->Font->Style  = FStyle;
	canvas->Brush->Style = bsClear;

	int TextH = canvas->TextHeight( L"成交:漲跌量時間") + 2;
	int Textw = canvas->TextWidth( L"漲跌:") + 5;

	canvas->TextOut( x+XOffset,y+YOffset,  L"成交:");
	canvas->TextOut( x+XOffset,y+TextH+YOffset, L"漲跌:");
	canvas->TextOut( x+XOffset,y+2*TextH+YOffset, L"量:");
	canvas->TextOut( x+XOffset,y+3*TextH+YOffset, L"時間:");
	if( FIsEmpty == false )
	{
		String FillPxStr, DiffPx;

		FTickList->ToTNFormat( FFillPx, FillPxStr );
		FTickList->ToTNFormat( FFillPx - FLastFillPx, DiffPx );
		canvas->TextOut( x+XOffset+Textw,y+3*TextH+YOffset, FTime.c_str() );
		canvas->Font->Color  = FColor;
		canvas->TextOut( x+XOffset+Textw,y+YOffset, FillPxStr );
		canvas->TextOut( x+XOffset+Textw,y+TextH+YOffset, DiffPx);
		canvas->TextOut( x+XOffset+Textw,y+2*TextH+YOffset, String( FQty ));
	}
	else
	{
		canvas->TextOut( x+XOffset+Textw,y+YOffset,  L"-----");
		canvas->TextOut( x+XOffset+Textw,y+TextH+YOffset, L"-----");
		canvas->TextOut( x+XOffset+Textw,y+2*TextH+YOffset, L"-----");
		canvas->TextOut( x+XOffset+Textw,y+3*TextH+YOffset, L"--:--");
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleHint::SetData( float FillPx, float LastFillPx, int Qty, int Digi, TColor Col, const String& Time )
{
	FIsEmpty   = false;
	FFillPx    = FillPx;
	FLastFillPx= LastFillPx;
	FQty	   = Qty;
	FPxDigi    = Digi;
	FColor     = Col;
	FTime	   = Time;
}
//---------------------------------------------------------------------------
//  static Functions
//---------------------------------------------------------------------------
inline int AddMax255( int RGB, int Val )
{
	if( RGB + Val >= 255 )
		return 255;
	return  RGB + Val;
}
//---------------------------------------------------------------------------
inline int Min0( int RGB, int Val )
{
	if( RGB - Val <= 0 )
		return 0;
	return  RGB - Val;
}
//---------------------------------------------------------------------------
int FilledCompare::Compare( void* elem1, void* elem2 )
{
	TFilledData* Item1 = static_cast<TFilledData*>(elem1);
	TFilledData* Item2 = static_cast<TFilledData*>(elem2);
	int hhmm1 = Item1->GetCompareHHMM();
	int hhmm2 = Item2->GetCompareHHMM();

	if( hhmm1 > hhmm2 )
		return 1;
	else if ( hhmm1 < hhmm2 )
		return -1;
	else
		return 0;
}
//---------------------------------------------------------------------------
int HowManyCandles( int Minutes, int MinutesPerStick )
{
	int Candles = Minutes / MinutesPerStick;
	if( Minutes % MinutesPerStick )
		Candles += 1;
	return Candles;
}
//---------------------------------------------------------------------------
int TotalCandles( int Minutes, int MinutesPerStick, int Days, bool HasOffhour )
{
	int Total;
	int Candles = Minutes / MinutesPerStick;

	if( Minutes % MinutesPerStick )
		Candles += 1;
	Total = Candles*Days + (AFTER_CLOSE/MinutesPerStick);
	if( HasOffhour == true )
		Total += OFFHOUR_MINUTES/MinutesPerStick;
	return Total;
}
//---------------------------------------------------------------------------
//   |<---------------- FVirtualWidth ----------------->|
//   |---------------------------------------------------| ---
//   |                                                   |  ^
//   |(FViewPortX,FViewPortY)                            |  |
//   |      -----------------------------                |  |
//   |      |                        | ^                 |  |
//   |      |                        | |                 |  |
//   |      |                        | |                 |  | FVirtualHeight
//   |      |                        | | FViewPortHeight |  |
//   |      |                        | |                 |  |
//   |      |                        | |                 |  |
//   |      |                        | v                 |  |
//   |      -----------------------------                |  |
//   |      |<--- FViewPortWidth --->|                   |  |
//   |                                                   |  v
//   |---------------------------------------------------| ---
//
//   FViewPortWidthMax: View port width max.( up to see 300 KBars)
//   FViewPortWidthMin: View port width min.( at least to see 10 KBars)
//---------------------------------------------------------------------------
__fastcall TCandleStickChart::TCandleStickChart(TComponent* Owner)
:TCustomControl(Owner)
,FCandleWidth( 10 )
,FDispPixelsPerTick( 10 )
,FPixelsPerTick( 10 )
,FMinutesPerStick( 1 )
,FDayCandle( 300 )
,FTotalCandle( 300*5 + AFTER_CLOSE )
,FIsChangingSymbol( false )
,FOpenTimeMin( 60 * 8 + 45 )
,FCloseTimeMin( 60 * 13 + 45 )
,FDayMinutes( 300 )
,FViewPortX( 0 )
,FViewPortY( 0 )
,FFontSize( FONT_DEF_SIZE )
,FBuffer( new Graphics::TBitmap() )
,FQtyBuffer( new Graphics::TBitmap() )
,FTickList( NULL )
,FExInfo( NULL )
,FStore( NULL )
,FMouseDown( false )
,FViewPortToControlX(1)
,FViewPortToControlY(1)
,FYAxisOffset(10)
,FXAxisOffset(10)
,DAYS( 5 )
,FShowQty( 30 )
,FChartPercent( 0.7 )
,FLastMouseX(-1)
,FLastMouseY(-1)
,FCandleHintwin( NULL )
,FHintwin( NULL )
,FTimeHint( NULL )
,FPriceHint( NULL )
,FOrderHint( NULL )
,FMouseIndex( -1 )
,FLastIndex(0)
,FLastTick( -1 )
,FMaxQty( 100 )
,FLastPaint( 0 )
,FMaxFPS( 5 )
,FPaintInterval( 200 )
,FBullColor( clRed )
,FBearColor( clGreen )
,FCandleLineColor( clNone )
,FGridColor( clGray )
,FTextColor( clSilver )
,FShowMA1( true )
,FShowMA2( true )
,FShowMA3( true )
,FMACount1( 1 )
,FMACount2( 3 )
,FMACount3( 5 )
,FMAColor1( clMoneyGreen )
,FMAColor2( clYellow )
,FMAColor3( clOlive )
,FLineColor( clCream )
,FMAPrice1( maClose )
,FMAPrice2( maClose )
,FMAPrice3( maClose )
,FScaleType( stAuto )
,FGradientCandle( true )
,FPxGridType( pgtAuto )
,FTimeGridType( tgtAuto )
,FTicksPerGrid( 10 )
,FDebugMode( false )
,FMaxUseMS( 0 )
,FFrameCount( 0 )
,FCurrentFPS( 0 )
,FBeginFPSTick( UFC::GetTickCountMS() )
,FGDIPlus( true )
,FBuyStopOrderOrdType( nsOrderMessageDefine::otLimit )
,FSellStopOrderOrdType( nsOrderMessageDefine::otLimit )
,FBuyStopTick(0)
,FSellStopTick(0)
,FFilledList( &FFilledCompare )
,FEditMode( false )
,FEditType( etVLine )
,FAddingLine( false )
,FRightSpace( 5 )
,FShowBox( true )
,FHasBox( false )
,FBoxExists( false )
,FHLDiff( AOEBoxHLDiff )
,FVLeft( 0 )
,FVRight( 0 )
,FVTop( 0 )
,FVBottom( 0 )
,FDayHY( 0 )
,FDayLY( 0 )
,FDayHigh( 0.0 )
,FDayLow( 999999 )
,FHLSection( stNoSection )
,FSectionLineColor( clOlive )
,FExableSkyLand( false )
,FOnKBar( NULL )
,FPosition( this )
,FSectionHLine( this, 0, clRed )
,FSectionLLine( this, 200, clGreen )
,FExablePivot( false )
,FExtDepth( 12 )
,FExtDeviation( 0.002 )
,FExtBackstep( 3 )
,FFirstPivotExists( false )
,FShowAvgPx( true )
,FReplacePx( false )
,FCursorType( ctBoth )
,FCanUpdate( true )
{
	DoubleBuffered = false;

	FDayCandle   = HowManyCandles( FDayMinutes, FMinutesPerStick );
	FTotalCandle = TotalCandles( FDayMinutes, FMinutesPerStick, DAYS, FHasOffhour );
	FVTodayX     = FDayCandle * (DAYS-1) * FCandleWidth;
	Clear1MinKData();

	FVirtualWidth     = FTotalCandle * FCandleWidth;         ///< Virtual desktop width.
	FVirtualHeight    = 100 * FPixelsPerTick;               ///< Virtual desktop Height.
	FViewPortWidthMin = MIN_CANDLE_VIEW_PORT * FCandleWidth;///< View port width min.
	FViewPortWidthMax = MAX_CANDLE_VIEW_PORT * FCandleWidth;///< View port width max.
	if( FViewPortWidthMax > FVirtualWidth )
		FViewPortWidthMax = FVirtualWidth;
	FViewPortHeight   = FVirtualHeight;                     ///< View all virtual desktop.
	FViewPortWidth    = FViewPortWidthMax;                  ///< View all virtual desktop.

	FCandleHintwin = new TCandleHint( );
	OnMouseWheel = MouseWheel;
	OnMouseEnter = MouseEnter;
	OnMouseLeave = HandleMouseLeave;

	FTimer = new TTimer( this );
	FTimer->Interval = FPaintInterval;
	FTimer->Enabled  = false;
	FTimer->OnTimer  = OnRepaintTimer;

	FSectionHLine.SetShowPrice( true, true );
    FSectionLLine.SetShowPrice( true, false );
	this->TabStop = false;
	InitPopupMenu();
	Resize();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::InitPopupMenu( void )
{
	PopupMenu = new TPopupMenu(this);
	PopupMenu->AutoPopup   = false;
	PopupMenu->AutoHotkeys = maManual;
	TMenuItem* Item = new TMenuItem( PopupMenu );
	PopupMenu->Items->Add( Item );
	Item->Caption = L"觸價買";
	Item->OnClick = BuyStopClicked;

	TMenuItem* Item1 = new TMenuItem( PopupMenu );
	PopupMenu->Items->Add( Item1 );
	Item1->Caption = L"觸價賣";
	Item1->OnClick = SellStopClicked;

	TMenuItem* ItemS = new TMenuItem( PopupMenu );
	PopupMenu->Items->Add( ItemS );
	ItemS->Caption = L"-";

	TMenuItem* Item2 = new TMenuItem( PopupMenu );
	PopupMenu->Items->Add( Item2 );
	Item2->Caption = L"全部平倉";
	Item2->OnClick = CloseAllClicked;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::CheckViewPortSize( void )
{
	 ///< Check view port Height
	if( FViewPortHeight > FVirtualHeight )
		FViewPortHeight = FVirtualHeight;
	else if( FViewPortHeight < 100 )
		FViewPortHeight = 100;
	///< Check view port Y offset
	if( FViewPortY + FViewPortHeight > FVirtualHeight )
		FViewPortY = FVirtualHeight - FViewPortHeight;
	 ///< Check view port width
	if( FViewPortWidth > FViewPortWidthMax )
		FViewPortWidth = FViewPortWidthMax;
	else if( FViewPortWidth < FViewPortWidthMin )
		FViewPortWidth = FViewPortWidthMin;
	///< Check view port X offset
	if( FViewPortX + FViewPortWidth > FVirtualWidth )
		FViewPortX = FVirtualWidth - FViewPortWidth;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetDays( int days )
{
	FDayCandle   = HowManyCandles( FDayMinutes, FMinutesPerStick );
	FTotalCandle = TotalCandles( FDayMinutes, FMinutesPerStick, days, FHasOffhour );
	FVTodayX     = (FDayMinutes/FMinutesPerStick) * (days-1) * FCandleWidth;
	DAYS = days;
	if( FExInfo != NULL )
		FExInfo->SetKBarDays( DAYS );
//	ClearTodayData();
	FVirtualWidth     = FTotalCandle * FCandleWidth;
	FViewPortWidthMin = MIN_CANDLE_VIEW_PORT * FCandleWidth;
	FViewPortWidthMax = MAX_CANDLE_VIEW_PORT * FCandleWidth;
	if( FViewPortWidthMax > FVirtualWidth )
		FViewPortWidthMax = FVirtualWidth;
	FVirtualHeight    = 100 * FPixelsPerTick;
	CheckViewPortSize( );
}
//---------------------------------------------------------------------------
__fastcall TCandleStickChart::~TCandleStickChart( void )
{
	if( FHintwin != NULL )
		delete FHintwin;
	if( FTimeHint != NULL )
		delete FTimeHint;
	if( FPriceHint != NULL )
		delete FPriceHint;
	if(	FCandleHintwin != NULL )
		delete FCandleHintwin;
	delete FBuffer;
}
//------------------------------------------------------------------------------
void __fastcall TCandleStickChart::OnRepaintTimer(TObject *Sender )
{
	FTimer->Enabled = false;
	Paint();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::Loaded( void )
{
	TCustomControl::Loaded();
	FBuffer->Canvas->Font->Assign( Font );
	FBuffer->Canvas->Font->Size = FONT_DEF_SIZE;
	FFontSize = FONT_DEF_SIZE;
	Clear1MinKData();
	Resize();
	ChangeCursor();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::Resize( void )
{
	int RefTime, Tick;
	int RightX  = Width - FYAxisOffset;
	int BottomY = Height - FXAxisOffset;

	if( FBuffer->Width != Width )
		FBuffer->Width = Width;
	if( FBuffer->Height != Height )
		FBuffer->Height = Height;

	FDrawRect  = TRect( 0, 0, RightX, BottomY );
	FPriceRect = TRect( RightX, 0, Width, BottomY );
	FTimeRect  = TRect( 0, BottomY, Width, Height );

	FViewPortToControlX = (double)( Width - FYAxisOffset )  / (double)FViewPortWidth;
	if( FShowQty != 0 )
	{
		if( FQtyBuffer->Width != Width )
			FQtyBuffer->Width = Width;
		if( FQtyBuffer->Height != Height )
			FQtyBuffer->Height = Height * FChartPercent ;
		FViewPortToControlY = (double)( Height - FXAxisOffset ) * FChartPercent / (double)FViewPortHeight;
	}
	else
		FViewPortToControlY = (double)( Height - FXAxisOffset ) / (double)FViewPortHeight;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetStore( TCMarketDataStore* Store )
{
	FStore = Store;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetFPS( int FPS )
{
	if( FPS < 4 )
		FPS = 4;
	if( FPS > 60 ) ///< upto 60 fps
		FPS = 60;
	FMaxFPS = FPS;
	FPaintInterval   = 1000/FMaxFPS;
	FTimer->Interval = FPaintInterval;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetSymbol( BasicInformation* Info )
{
	FIsChangingSymbol = true;
	String BullPxStr;
	FInfo = Info;
	FDigit = Info->GetDigit();
	FExInfo = FStore->GetExchangeInfo( Info->GetExchange(), Info->GetProductID() );

	if( FExInfo != NULL )
	{
		if( Info->GetProductID() == "TXF" || Info->GetProductID() == "MXF" || Info->GetProductID() == "TMF" )
		{
			FHasBox = true;
			FHasOffhour = true;
		}
		else
		{
			FHasBox = false;
			FHasOffhour = false;
		}
		if( FInfo->GetMarket() == ::mTSE || FInfo->GetMarket() == ::mOTC )
			FIsStock = true;
		else
			FIsStock = false;
		FExInfo->SetKBarDays( DAYS );
		FExInfo->SetTAIFEXOffhour( FHasOffhour );
		FBoxExists  = false;
		FDayHigh = 0.0;
		FDayLow = 99999;
		FDayHY = 0;
		FDayLY = 0;
		FRefPx = Info->GetRefPrice();
		FDayMinutes = FExInfo->TotalMinutes();
		FDecimalLocator = Info->GetDecimalLocator();
		FMinutesPerStick = 1;
		Clear1MinKData();
		ClearOrderData();
		FTickList = (TTickList*)FStore->GetTickList( Info->GetExchange().c_str(), Info->GetSymbol().c_str() );
		FRefY = FTickList->PxToTickIndex( FRefPx,FDebugMode ) * FPixelsPerTick;
		FCandleHintwin->SetTickList( FTickList );
		if( FHintwin )
			FHintwin->SetTickList( FTickList );
		FDayCandle        = HowManyCandles( FDayMinutes, FMinutesPerStick );
		FTotalCandle      = TotalCandles( FDayMinutes, FMinutesPerStick, DAYS, FHasOffhour );
		FVirtualWidth     = FDayCandle* FCandleWidth;
		FViewPortWidthMax = MAX_CANDLE_VIEW_PORT * FCandleWidth; ///< upto MAX_CANDLE_VIEW_PORT K bars in view.(MAX_CANDLE_VIEW_PORT=300)
		if( FViewPortWidthMax > FVirtualWidth )
			FViewPortWidthMax = FVirtualWidth;
		FVirtualHeight    = FTickList->ItemCount() * FPixelsPerTick; ///< Ticks *10 = Virtual H
		CheckViewPortSize();
		FViewPortX        = FViewPortX = FLastIndex * FCandleWidth - FViewPortWidth / 2 ;
		FViewPortY        = 0;
		BullPxStr.printf( L"%.*f", FDigit, FInfo->GetBullPrice() );
		FYAxisOffset = FBuffer->Canvas->TextWidth( BullPxStr ) * 1.2 ;
		FXAxisOffset = FBuffer->Canvas->TextHeight( BullPxStr );
		if( FPriceHint != NULL )
			FPriceHint->SetMaxWidth( BullPxStr );
		if( FOrderHint != NULL )
			FOrderHint->SetMaxWidth( FYAxisOffset );
		Constraints->MinHeight = FXAxisOffset;
		Constraints->MinWidth = FYAxisOffset * 3;
		FCandleHintwin->Empty();
		FBearTick = FTickList->PxToTickIndex( FInfo->GetBearPrice(),FDebugMode );
	}
	FIsChangingSymbol = false;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetTickEx( double MaxPx, double MinPx )
{
	Clear1MinKData();
	String BullPxStr;

	FTickList = (TTickList*)FStore->GetTickListEx( FInfo->GetExchange().c_str(), FInfo->GetSymbol().c_str(), MaxPx, MinPx );
	FCandleHintwin->SetTickList( FTickList );
	if( FHintwin )
		FHintwin->SetTickList( FTickList );
	FVirtualHeight = FTickList->ItemCount() * FPixelsPerTick;
	CheckViewPortSize();
	BullPxStr.printf( L"%.*f", FDigit, MaxPx );
	FYAxisOffset = FBuffer->Canvas->TextWidth( BullPxStr ) * 1.2;
	FXAxisOffset = FBuffer->Canvas->TextHeight( BullPxStr );

	Constraints->MinHeight = FXAxisOffset;
	Constraints->MinWidth = FYAxisOffset * 3;
	FCandleHintwin->Empty();
	FBearTick = FTickList->PxToTickIndex( MinPx,FDebugMode );
	SetMinutesPerStick( FMinutesPerStick );
}
//---------------------------------------------------------------------------
double __fastcall TCandleStickChart::MaxPxInView( void )
{
	int Start = FViewPortX / FCandleWidth;
	if( Start < 0 )
		Start = 0;

	int End = ( FViewPortX + FViewPortWidth ) / FCandleWidth;
	if( End > FDrawMaxPx.Length )
		End = FDrawMaxPx.Length;

	double Max = -1;
	for( register int i = Start; i < End; i++ )
	{
		if( FDrawHaveData[i] && Max < FDrawMaxPx[i] )
			Max = FDrawMaxPx[i];
	}
	return Max;
}
//---------------------------------------------------------------------------
double __fastcall TCandleStickChart::MinPxInView( void )
{
	int Start = FViewPortX / FCandleWidth;
	if(Start < 0 )
		Start = 0;

	int End = ( FViewPortX + FViewPortWidth ) / FCandleWidth;
	if( End > FDrawMaxPx.Length )
		End = FDrawMaxPx.Length;

	double Min = FInfo->GetBullPrice() + 10;

	for( register int i = Start; i < End; i++ )
	{
		if( FDrawHaveData[i] && Min > FDrawMinPx[i]  )
			Min = FDrawMinPx[i];
	}

	if( Min > FInfo->GetBullPrice() )
		return -1;
	return Min;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::AdjustViewPortHeight( void )
{
	if( FScaleType == stAuto && !ComponentState.Contains( csDesigning ) && FTickList != NULL ) ///< Not design time.
	{
		double MaxPx = MaxPxInView();
		double MinPx = MinPxInView();
		if( MaxPx > 0 && MinPx > 0 )
		{
			int MaxY = FTickList->PxToTickIndex( MaxPx,FDebugMode ) * FPixelsPerTick;
			int MinY = FTickList->PxToTickIndex( MinPx,FDebugMode ) * FPixelsPerTick;

			if( MaxY - 25 < 0 )
				MaxY = 0;
			else
				MaxY -= 25;

			if( MinY + 25 > FVirtualHeight )
				MinY = FVirtualHeight;
			else
				MinY += 25;

			FViewPortHeight = ( MinY - MaxY );
			FViewPortY = MaxY;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetDrawPoint( int Index )
{
	if( FDrawHaveData[Index] )
	{
		int Max, Min, Top, Bottom, Left, Right;

		Max = FTickList->PxToTickIndex( FDrawMaxPx[Index],FDebugMode ) * FPixelsPerTick;
		Min = FTickList->PxToTickIndex( FDrawMinPx[Index],FDebugMode ) * FPixelsPerTick;
		Left = Index * FCandleWidth;
		Right = Left + FCandleWidth;
		if( FDrawOpenPx[Index] > FDrawClosePx[Index] )
		{
			Top = FTickList->PxToTickIndex( FDrawOpenPx[Index],FDebugMode ) * FPixelsPerTick;
			Bottom = FTickList->PxToTickIndex( FDrawClosePx[Index],FDebugMode ) * FPixelsPerTick;
		}
		else
		{
			Top = FTickList->PxToTickIndex( FDrawClosePx[Index],FDebugMode ) * FPixelsPerTick;
			Bottom = FTickList->PxToTickIndex( FDrawOpenPx[Index],FDebugMode ) * FPixelsPerTick;
		}
		FLastTick = FTickList->PxToTickIndex( FDrawClosePx[Index], false ) * FPixelsPerTick;
		FTopLeftArray[Index]     = TPoint( Left, Top );
		FBottomRightArray[Index] = TPoint( Right, Bottom );
		FMaxArray[Index]         = TPoint( Left + FCandleWidth/2, Max );
		FMinArray[Index]         = TPoint( Left + FCandleWidth/2, Min );
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetMinutesPerStick( int MinutesPerStick )
{
	FDayCandle       = HowManyCandles( FDayMinutes, MinutesPerStick );
	FTotalCandle     = TotalCandles( FDayMinutes, MinutesPerStick, DAYS, FHasOffhour );
	FVTodayX         = FDayCandle * (DAYS-1) * FCandleWidth;

	if( FMinutesPerStick != MinutesPerStick )
	{
		bool SetOpen = false;

		FMinutesPerStick = MinutesPerStick;
		ClearDrawData();
		FLastIndex = 0;
		for( register int i = 0, j = 0; i < FHaveData.Length - AFTER_CLOSE; i++ )
		{
			if( FHaveData[i] )
			{
				 if( SetOpen == false )
				 {
					SetOpen = true;
					FLastIndex = j;
					FDrawHaveData[j] = true;
					FDrawOpenPx[j]   = FOpenPx[i];
					FDrawClosePx[j]  = FClosePx[i];
					FDrawMaxPx[j]    = FMaxPx[i];
					FDrawMinPx[j]    = FMinPx[i];
					FDrawQty[j]      = FQtys[i];
				 }
				 else
				 {
					FDrawClosePx[j] = FClosePx[i];
					if( FDrawMaxPx[j] < FMaxPx[i] )
						FDrawMaxPx[j] = FMaxPx[i];
					if( FDrawMinPx[j] > FMinPx[i] )
						FDrawMinPx[j] = FMinPx[i];
					FDrawQty[j] += FQtys[i];
				 }
			}
			if( ( i % FDayMinutes ) % FMinutesPerStick == FMinutesPerStick - 1 || ///< Last Minutes Per Stick minutes
				( FDayMinutes % FMinutesPerStick > 0 && ( i % FDayMinutes ) >= FDayMinutes - 1 ) ) ///< Last minutes
			{
				SetOpen = false;
				SetDrawPoint( j );
				j++;
			}
		}
		FViewPortX = FLastIndex * FCandleWidth - FViewPortWidth / 2 ;;
		FViewPortY = 0;
		if( FLastTick == -10 ) ///< Last price not in tick list.
			FLastTick = FTickList->PxToTickIndex( FInfo->GetRefPrice(),FDebugMode ) * FPixelsPerTick;
		if( !ComponentState.Contains( csDesigning ) && !ComponentState.Contains( csLoading ) && FTickList != NULL ) ///< Not design time.
		{
			///< 1. Adjust virtual map
			FVirtualWidth     = FTotalCandle * FCandleWidth;
			FViewPortWidthMax = MAX_CANDLE_VIEW_PORT * FCandleWidth;
			if( FViewPortWidthMax > FVirtualWidth )
				FViewPortWidthMax = FVirtualWidth;
			///< 2.Adjust ViewPort size
			FVirtualHeight  = FTickList->ItemCount() * FPixelsPerTick;
			CheckViewPortSize();
			Pivot( FLastIndex  );
			CenterLastTick( );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawPxLine( TCanvas* canvas, TColor LineC, int StartX, int PxY, double Px, bool ShowPercent, bool TextUp )
{
	String PxStr,Dir;

	if( ShowPercent == true )
	{
		double Diff = Px - FRefPx;
		double Percent = fabs( Diff )*100.0 / FRefPx;

		if( Diff > 0 )
		{
			Dir = L"▲";
			canvas->Font->Color = clRed;
		}
		else
		{
			Dir = L"▼";
			canvas->Font->Color = clGreen;
		}
		PxStr.printf( L"%.*f %s%0.2f", FDigit, Px, Dir, Percent );
	}
	else
	{
		canvas->Font->Color = LineC;
		PxStr.printf( L"%.*f", FDigit, Px );
	}
	int TextW = canvas->TextWidth( PxStr );
	int TextH = canvas->TextHeight( PxStr );
	canvas->Pen->Color = LineC;
	canvas->MoveTo( StartX, PxY );
	canvas->LineTo( Width , PxY );
	if( TextUp == true && PxY < TextH + 6 )
		TextUp = false;
	if( TextUp == true )
		canvas->TextOutW( Width - TextW - FYAxisOffset - 5, PxY - TextH -5, PxStr );
	else
		canvas->TextOutW( Width - TextW - FYAxisOffset - 5, PxY + 5, PxStr );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawBox( TCanvas* canvas )
{
	int StartX = ( FVTodayX   - FViewPortX)* FViewPortToControlX;

	canvas->Brush->Style = bsClear;
	canvas->Font->Size   = FFontSize;
	///< Paint DayH/L
	if( FShowDayHL == true )
	{
		if( FDayLY != 0 )
		{
			int DHY = (FDayHY - FViewPortY)* FViewPortToControlY;
			int DLY = (FDayLY - FViewPortY)* FViewPortToControlY;

			canvas->Pen->Style   = psSolid;
			canvas->Pen->Width = 2;
			///< Day high line
			DrawPxLine( canvas, clRed, StartX, DHY, FDayHigh, true, true );
			///< Day low line
			DrawPxLine( canvas, clGreen, StartX, DLY, FDayLow, true, false );
			///< Draw RefPx line
			if( FIsStock == true )
			{
				int REF = (FRefY  - FViewPortY)* FViewPortToControlY;
				DrawPxLine( canvas, clSilver, StartX, REF, FRefPx, false, true );
			}

		}
	}
    ///< Paint AOE Box
	if( FShowBox == true && FHasBox == true && FBoxExists == true && FMinutesPerStick == 1 )
	{
		int    EndX    = (FVRight  - FViewPortX)* FViewPortToControlX;
		int    TopY    = (FVTop    - FViewPortY)* FViewPortToControlY;
		int    BottomY = (FVBottom - FViewPortY)* FViewPortToControlY;
		TRect  BoxRect( StartX, TopY,EndX, BottomY );
		int    TextTop,TextLeft;
		String RiskText;

		canvas->Pen->Style   = psSolid;
		RiskText.printf( L"%0.1f", FRiskRatio );
		if( FEmojis != NULL  && FEmojis->Count >= 3 )
		{
			TextLeft = StartX + 8 + FEmojis->Width;
			if( FBoxDirUp == true  )
			{
				TextTop = TopY + 5;
				FEmojis->Draw( canvas, StartX + 5, TextTop, FEmojiIndex );
			}
			else
			{
				TextTop = BottomY - canvas->TextHeight( RiskText ) - 5;
				FEmojis->Draw( canvas, StartX + 5, BottomY - FEmojis->Height - 5, FEmojiIndex );
			}
		}
		else ///< No Emoji Icon
		{
			TextLeft = StartX + 5;
			if( FBoxDirUp == true  )
				TextTop = TopY + 5;
			else
				TextTop = BottomY - canvas->TextHeight(RiskText) - 5;
		}
		canvas->Pen->Color = clYellow;
		canvas->Pen->Width = 3;
		canvas->Rectangle( BoxRect );
		canvas->Font->Color = clYellow;
		canvas->TextOutW( TextLeft, TextTop, RiskText );
		RiskText.printf( L"滿倉大台10口,風控調整後大台%d,小台%d口", (int)(10.0/FRiskRatio), (int)(40.0/FRiskRatio) );
		canvas->TextOutW( Width - 150 - FYAxisOffset - canvas->TextWidth( RiskText ), 5 , RiskText );
	}
	int DH = FSectionHLine.GetY();
	int DL = FSectionLLine.GetY();

	if( FHLSection == stNaturalSection || FHLSection ==  stGoldenSection )
	{
		if( DL > DH )
			DrawSection( canvas, DH, DL );
		else
			DrawSection( canvas, DL, DH );
		FSectionHLine.Paint( canvas );
		FSectionLLine.Paint( canvas );
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawSection(  TCanvas* canvas, int DHY, int DLY )
{
	int StartX = ( FVTodayX   - FViewPortX)* FViewPortToControlX;
	///< Day high/Low Section line
	if( FHLSection == stNaturalSection )
	{
		int Half     = (DLY-DHY)/2;
		int OneThird = (DLY-DHY)/3;

		canvas->Pen->Style = psDash;
		canvas->Pen->Width = 1;
		canvas->Pen->Color = FSectionLineColor;
		canvas->MoveTo( StartX, DLY - OneThird);
		canvas->LineTo( Width , DLY - OneThird);
		canvas->MoveTo( StartX, DLY - Half);
		canvas->LineTo( Width , DLY - Half);
		canvas->MoveTo( StartX, DHY + OneThird);
		canvas->LineTo( Width , DHY + OneThird);
		canvas->Pen->Style = psSolid;
	}
	else if( FHLSection ==  stGoldenSection )
	{
		int FirstY  = (DLY-DHY)*3820/10000;
		int SecondY = (DLY-DHY)*6180/10000;

		canvas->Pen->Style = psDash;
		canvas->Pen->Width = 1;
		canvas->Pen->Color = FSectionLineColor;
		canvas->MoveTo( StartX, DHY + FirstY);
		canvas->LineTo( Width , DHY + FirstY);
		canvas->MoveTo( StartX, DHY + SecondY);
		canvas->LineTo( Width , DHY + SecondY);
		canvas->Pen->Style = psSolid;
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::BoxGrade( int index, int Minutes, double HL )
{
	double TotalTick = 0;
	double diff;

	for( register int i = 1; i <= Minutes; i++ )
	{
		register int m = index - i;
		if( FHaveData[ m ] == true )
		{
			diff = (FMaxPx[m] - FMinPx[m]);
			TotalTick += diff;
		}
	}
	///< Risk ratio( H/L in box /30 ticks)
	FRiskRatio  = HL/FHLDiff;
	///< Waste ratio(K bars total ticks/30 ticks)
	FWasteRatio = TotalTick/HL;
	///< Select Emoji icon
	if( FWasteRatio >= 6.5 )
		FEmojiIndex = 3; ///< Worse case
	else if( FWasteRatio >= 4 && FWasteRatio < 6.5 )
		FEmojiIndex = 2; ///< bad
	else if( FWasteRatio >= 2.5 && FWasteRatio < 4 )
		FEmojiIndex = 1; ///< Okay
	else
		FEmojiIndex = 0; ///< Good!
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::FindBox( int index, int Minutes, TMinutesTickInfo& Info )
{
	if( FHasBox == true ) ///< TXF or MXF has box.
	{
		if( FBoxExists == false && Minutes >= 16 ) ///< Box not found and TWSE Opened.
		{
			double HL;
			double DayH = FDayHigh;
			double DayL = FDayLow;

			if( ( HL = DayH - DayL ) >= FHLDiff  )  ///< H/L large than 30, Check box exists.
			{
				if( Info.MaxPx >= DayH ) ///< Bingo! It's day high.
				{
					DayH = Info.MaxPx;
					FBoxDirUp  = true;
					FBoxExists = true;
				}
				if( Info.MinPx <= DayL )///<  Bingo! It's day low.
				{
					DayL = Info.MinPx;
					FBoxDirUp  = false;
					FBoxExists = true;
				}
				if( FBoxExists == true )
				{
					FVLeft   = FDayMinutes * (DAYS-1) * FCandleWidth; ///< Left = Open Time
					FVRight  = FVLeft + (Minutes+1) * FCandleWidth;   ///< Right = this minutes
					FVBottom = FTickList->PxToTickIndex( DayL, FDebugMode ) * FPixelsPerTick;
					FVTop    = FTickList->PxToTickIndex( DayH, FDebugMode ) * FPixelsPerTick;
					BoxGrade( index, Minutes, HL );
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::CheckDayHL( TMinutesTickInfo& Info )
{
	if(	Info.MaxPx > FDayHigh )
	{
		int YIndex = FTickList->PxToTickIndex( Info.MaxPx,FDebugMode );

		FDayHigh = Info.MaxPx;
		FDayHY   = YIndex * FPixelsPerTick;
		if( FUseHLSection == true )
			FSectionHLine.MoveTo( YIndex );
	}
	if(	Info.MinPx < FDayLow )
	{
		int YIndex = FTickList->PxToTickIndex( Info.MinPx,FDebugMode );

		FDayLow = Info.MinPx;
		FDayLY  = YIndex * FPixelsPerTick;
		if( FUseHLSection == true )
			FSectionLLine.MoveTo( YIndex );
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::CalculateKBarFistTime( int DrawIndex, TMinutesTickInfo& Info )
{
	int Top, Bottom, Max, Min;
	int	left  = DrawIndex * 10; ///< 10  virtaul KBar width
	int right = left + 10;

	FDrawHaveData[DrawIndex]  = true;
	FDrawQty[ DrawIndex ]     = Info.Qty;
	FDrawMaxPx[ DrawIndex ]   = Info.MaxPx;
	FDrawMinPx[ DrawIndex ]   = Info.MinPx;
	FDrawOpenPx[ DrawIndex ]  = Info.OpenPx;
	FDrawClosePx[ DrawIndex ] = Info.ClosePx;
	Max = FTickList->PxToTickIndex( FDrawMaxPx[ DrawIndex ],FDebugMode ) * FPixelsPerTick;
	Min = FTickList->PxToTickIndex( FDrawMinPx[ DrawIndex ],FDebugMode ) * FPixelsPerTick;
	if( FDrawOpenPx[ DrawIndex ] > FDrawClosePx[ DrawIndex ] )
	{
		Top    = FTickList->PxToTickIndex( FDrawOpenPx[ DrawIndex ],FDebugMode ) * FPixelsPerTick;
		Bottom = FTickList->PxToTickIndex( FDrawClosePx[ DrawIndex ],FDebugMode ) * FPixelsPerTick;
	}
	else
	{
		Top    = FTickList->PxToTickIndex( FDrawClosePx[ DrawIndex ],FDebugMode ) * FPixelsPerTick;
		Bottom = FTickList->PxToTickIndex( FDrawOpenPx[ DrawIndex ],FDebugMode ) * FPixelsPerTick;
	}
	/// Add a new point to array
	FTopLeftArray[ DrawIndex ]     = TPoint( left, Top );
	FBottomRightArray[ DrawIndex ] = TPoint( right, Bottom );
	FMaxArray[DrawIndex]           = TPoint( left + 5, Max );;
	FMinArray[DrawIndex]           = TPoint( left + 5, Min );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::CalculateKBarUpdate( int DrawIndex, int RefMin,  TMinutesTickInfo& Info )
{
	int Top, Bottom, Max, Min; ///< A KBar

	///< Calculate N-Min KBar Qty.
	FDrawQty[ DrawIndex ] = 0;
	if( FDayMinutes % FMinutesPerStick == 0 )
	{
		 for( register int i = 0; i < FMinutesPerStick; i++)
		{
			if( FHaveData[DrawIndex * FMinutesPerStick + i] )
				FDrawQty[ DrawIndex ] += FQtys[ DrawIndex * FMinutesPerStick + i ];
		}
	}
	else
	{
		int MinuteInCaldle;
		int Offset = RefMin - (RefMin % FMinutesPerStick );

		if( DrawIndex % FDayCandle == (int)( FDayMinutes / FMinutesPerStick ) )  // Last Candle
			MinuteInCaldle = FDayMinutes % FMinutesPerStick ;
		else
			MinuteInCaldle = FMinutesPerStick;
		for( register int i = 0; i < MinuteInCaldle ; i++)
		{
			if( FHaveData[Offset + i] )
				FDrawQty[DrawIndex] += FQtys[Offset + i];
		}
	}
	///< Calculate KBar LeftTop, RightBottom. Max, Min points.
	FDrawClosePx[ DrawIndex ] = Info.ClosePx;
	if( Info.MaxPx > FDrawMaxPx[ DrawIndex ] )
	{
		FDrawMaxPx[ DrawIndex ] = Info.MaxPx;
		Max = FTickList->PxToTickIndex( Info.MaxPx,FDebugMode ) * FPixelsPerTick;
		FMaxArray[DrawIndex].y = Max;
	}
	if( Info.MinPx < FDrawMinPx[ DrawIndex ] )
	{
		FDrawMinPx[ DrawIndex ] = Info.MinPx;
		Min = FTickList->PxToTickIndex( Info.MinPx,FDebugMode ) * FPixelsPerTick;
		FMinArray[DrawIndex].y = Min;
	}
	if( FDrawOpenPx[DrawIndex] > FDrawClosePx[DrawIndex] ) ///< Top: Open, Bottom: Close
	{
		Top    = FTickList->PxToTickIndex( FDrawOpenPx[ DrawIndex ],FDebugMode ) * FPixelsPerTick;
		Bottom = FTickList->PxToTickIndex( FDrawClosePx[ DrawIndex ],FDebugMode ) * FPixelsPerTick;
	}
	else
	{
		Top    = FTickList->PxToTickIndex( FDrawClosePx[ DrawIndex ],FDebugMode ) * FPixelsPerTick;
		Bottom = FTickList->PxToTickIndex( FDrawOpenPx[ DrawIndex ],FDebugMode ) * FPixelsPerTick;
	}
	FTopLeftArray[DrawIndex].y = Top;
	FBottomRightArray[DrawIndex].y = Bottom;
}
//---------------------------------------------------------------------------
bool __fastcall TCandleStickChart::SetData( int Day, TMinutesTickInfo& Info, bool NeedPaint )
{
	int RefMin = FExInfo->TimeToIndex( Info.Hour ,Info.Minutes, Day == 0  ); ///< Reference minutes
	int MinAfterOpen = RefMin;
	int Top, Bottom, Max, Min, DrawIndex; ///< A KBar

	if( RefMin == 0 )
	{
		if( FExInfo->IsTradingTime( Info.Hour ,Info.Minutes, Day == 0 ) == false ) ///< Not trading time
		{
			String Time;
			Time.printf( L"%02d:%02d", Info.Hour ,Info.Minutes );
			FCandleHintwin->SetData( Info.ClosePx, Info.ClosePx, Info.Qty, FDigit, FBearColor, Time );
			return false;
		}
	}
	if( Day == 0 ) ///< today data
	{
		DrawIndex = FDayCandle * ( DAYS - 1 ) + ( RefMin/FMinutesPerStick ); ///< Prev Days-1 candles  + Today's candles.
		RefMin += (DAYS - 1) * FDayMinutes;
		FindBox( DrawIndex, MinAfterOpen, Info );
		CheckDayHL( Info );
		Pivot( RefMin / FMinutesPerStick  );
	}
	else  ///< history data
	{
		DrawIndex = FDayCandle * ( Day - 1 ) + ( RefMin / FMinutesPerStick );
		RefMin += (Day - 1 ) * FDayMinutes;
	}
	///< Update KBar data.
	FLastIndex  = DrawIndex;
	FLastPxTick = FTickList->PxToTickIndex( Info.ClosePx, FDebugMode );
	FLastTick   = FLastPxTick * FPixelsPerTick;
	if( Day == 0 ) ///< Today
	{
		if( FHaveData[RefMin] == false ) ///< First data
		{
			FHaveData[RefMin] = true;
			FOpenPx[RefMin]   = Info.OpenPx;
			FMaxPx[RefMin]    = Info.MaxPx;
			FMinPx[RefMin]    = Info.MinPx;
			FQtys[RefMin]     = Info.Qty;
		}
		else
		{
			if( FQtys[RefMin]  < Info.Qty )
				FQtys[RefMin]  = Info.Qty;
			if( FMaxPx[RefMin] < Info.MaxPx )
				FMaxPx[RefMin] = Info.MaxPx;
			if( FMinPx[RefMin] > Info.MinPx )
				FMinPx[RefMin] = Info.MinPx;
		}
		FClosePx[ RefMin ] = Info.ClosePx;
	}
	else
	{
		FHaveData[RefMin] = true;
		FOpenPx[RefMin]   = Info.OpenPx;
		FMaxPx[RefMin]    = Info.MaxPx;
		FMinPx[RefMin]    = Info.MinPx;
		FQtys[RefMin]     = Info.Qty;
		FClosePx[ RefMin ] = Info.ClosePx;
	}
	///< Calculate KBar draw data
	if( FDrawHaveData[ DrawIndex ] == false )
	{
		if( Day == 0 ) ///< Today
		{
			FBaseIndex = DrawIndex;
			if( FOnKBar !=  NULL )
				FOnKBar( this, Info.Hour ,Info.Minutes );
		}
		CalculateKBarFistTime( DrawIndex, Info );
	}
	else
		CalculateKBarUpdate( DrawIndex, RefMin, Info );
	///< Repaint
	if( NeedPaint == true )
	{
		int BeginIndex = FViewPortX / FPixelsPerTick;
		int EndIndex = ( FViewPortX + FViewPortWidth ) / FPixelsPerTick;

		if( EndIndex >= FHaveData.Length / FMinutesPerStick )
			EndIndex = FHaveData.Length - 1;
		AdjustViewPortHeight();
		Resize();
		if( DrawIndex >= BeginIndex && DrawIndex <= EndIndex  )
			PaintFPS();
	}
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::ShowTickHint( int Index )
{
	if( FDrawHaveData.Length > Index )
	{
		if( FDrawHaveData[ Index ] )
		{
			int Hour, Minutes, LastIndex = 0;
			String Time;

			if( Index > 0 )
			{
				for( register int i = Index - 1; i > 0; i-- )
				{
					if( FDrawHaveData[i] )
					{
						LastIndex = i;
						break;
					}
				}
			}
			UFC::AnsiString TimeStr;

			int DayCandle = FDayMinutes / FMinutesPerStick;
			if( FDayMinutes % FMinutesPerStick > 0 )
				DayCandle += 1;
			int Day = Index / DayCandle;
			FExInfo->IndexToTime( Day * FDayMinutes + ( Index % DayCandle ) * FMinutesPerStick , TimeStr );
			Time = TimeStr.c_str();

			if( FDrawClosePx[Index] < FDrawOpenPx[Index] ) ///< Price increasing
				FCandleHintwin->SetData( FDrawClosePx[Index], FDrawClosePx[LastIndex], FDrawQty[Index], FDigit, FBearColor, Time );
			else
				FCandleHintwin->SetData( FDrawClosePx[Index], FDrawClosePx[LastIndex], FDrawQty[Index], FDigit, FBullColor, Time );
		}
		else
			FCandleHintwin->Empty();
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::ClearOrderData( void )
{
	TWorkingLine* WorkingOrder = FWorkingOrders.First();

	while( WorkingOrder != NULL )
	{
		delete WorkingOrder;
		WorkingOrder = FWorkingOrders.Next();
	}
	FWorkingOrders.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::Clear1MinKData( void )
{
	int TotalMinutes = FDayMinutes * DAYS + AFTER_CLOSE;

	if( FHasOffhour == true )
		TotalMinutes = FDayMinutes * DAYS + OFFHOUR_MINUTES + AFTER_CLOSE;
	FMaxPx.Length    = TotalMinutes;
	FMinPx.Length    = TotalMinutes;
	FOpenPx.Length   = TotalMinutes;
	FClosePx.Length  = TotalMinutes;
	FHaveData.Length = TotalMinutes;
	FQtys.Length     = TotalMinutes;
	for( register int i = 0; i < FHaveData.Length; i++ )
	{
		FHaveData[i] = false;
		FQtys[i] = 0;
		FMaxPx[i] = 0;
		FMinPx[i] = 0;
		FOpenPx[i] = 0;
		FClosePx[i] = 0;
	}
	ClearDrawData();
	ClearGraphs();
	ClearFilled();
	FDateList.Clear();
	FCountedBar = 0;
	FFirstPivotExists = false;
	FPivots.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::ClearDrawData( void )
{
	FTotalCandle = TotalCandles( FDayMinutes, FMinutesPerStick, DAYS, FHasOffhour );

	FDrawHaveData.Length     = FTotalCandle;
	FDrawQty.Length          = FTotalCandle;
	FDrawMaxPx.Length        = FTotalCandle;
	FDrawMinPx.Length        = FTotalCandle;
	FDrawOpenPx.Length       = FTotalCandle;
	FDrawClosePx.Length      = FTotalCandle;
	FTopLeftArray.Length     = FTotalCandle;
	FBottomRightArray.Length = FTotalCandle;
	FMaxArray.Length         = FTotalCandle;
	FMinArray.Length         = FTotalCandle;
	for( register int i = 0; i < FDrawHaveData.Length; i++ )
	{
		FDrawHaveData[i] = false;
		FDrawQty[i] = 0;
		FDrawMaxPx[i] = 0;
		FDrawMinPx[i] = 0;
		FDrawOpenPx[i] = 0;
		FDrawClosePx[i] = 0;
	}
	FCountedBar = 0;
	FFirstPivotExists = false;
	FPivots.Clear();
}
//---------------------------------------------------------------------------
int __fastcall TCandleStickChart::FirstAvgPt( int Index, int MACount, MAPrice maPrice, double& Sum )
{
	int DataCount = 0;

	for( register int i = Index; i > Index - MACount; i-- )
	{
		if( FDrawHaveData[i])
		{
			switch( maPrice )
			{
				case maOpen:	Sum += 100.0*FDrawOpenPx[i]; break;
				case maClose:	Sum += 100.0*FDrawClosePx[i]; break;
				case maMax:		Sum += 100.0*FDrawMaxPx[i]; break;
				case maMin:		Sum += 100.0*FDrawMinPx[i]; break;
				case maAvg:		Sum += 100.0*( FDrawMaxPx[i] + FDrawMinPx[i] )/2; break;
			}
			DataCount ++;
		}
	}
	return DataCount;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::NextAvgPt( int Index, int MACount, MAPrice maPrice, int& DataCount, double& Sum )
{
	int i = Index - MACount;

	if( FDrawHaveData[i])//< Remove first data point.
	{
		switch( maPrice )
		{
			case maOpen:	Sum -= 100.0*FDrawOpenPx[i]; break;
			case maClose:	Sum -= 100.0*FDrawClosePx[i]; break;
			case maMax:		Sum -= 100.0*FDrawMaxPx[i]; break;
			case maMin:		Sum -= 100.0*FDrawMinPx[i]; break;
			case maAvg:		Sum -= 100.0*( FDrawMaxPx[i] + FDrawMinPx[i] )/2; break;
		}
		DataCount --;
	}
	i = Index;
	if( FDrawHaveData[ i ] )////< Add last data point.
	{
		switch( maPrice )
		{
			case maOpen:	Sum += 100.0*FDrawOpenPx[i]; break;
			case maClose:	Sum += 100.0*FDrawClosePx[i]; break;
			case maMax:		Sum += 100.0*FDrawMaxPx[i]; break;
			case maMin:		Sum += 100.0*FDrawMinPx[i]; break;
			case maAvg:		Sum += 100.0*( FDrawMaxPx[i] + FDrawMinPx[i] )/2; break;
		}
		DataCount ++;
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SMAPoint( int Index, int DataCount, double Sum, TPoint& AvgPoint )
{
	if( DataCount > 0 )
	{
		double SMA           = Sum / (100.0 * DataCount);
		int    AvgTick       = FTickList->FindTick( SMA );
		double TickPx        = FTickList->GetPxFromIndex( AvgTick, FDebugMode );
		double TickStep      = FTickList->GetTickStepFromIndex( AvgTick );

		AvgPoint.x = ( (double)( FMaxArray[Index].x - FViewPortX ) )* FViewPortToControlX;
		AvgPoint.y = ( ((double)AvgTick - (SMA-TickPx)/ TickStep )* FPixelsPerTick - FViewPortY ) * FViewPortToControlY;
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawSMAGDIPlus( TCanvas* canvas, int Count, MAPrice maPrice, TColor color, int BeginIndex, int EndIndex )
{
	Gdiplus::Graphics GPPGraphics( canvas->Handle );
	Gdiplus::Pen      DrawPen( Gdiplus::Color(255, GetRValue(color), GetGValue(color), GetBValue(color)), 1 );
	TPoint            AvgPoint;
	int               DataCount  = 0;
	bool              FindFirstPoint = false;
	double            Sum;

	GPPGraphics.SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
	for( register int Index = BeginIndex; Index < EndIndex; Index++ )
	{
		if( FindFirstPoint == false )
		{
			if( FDrawHaveData[ Index ] == true  )
			{
				Sum = 0;
				FindFirstPoint = true;
				DataCount = FirstAvgPt( Index, Count, maPrice, Sum );
				SMAPoint( Index, DataCount, Sum, AvgPoint );
				FLastAvgPoint = AvgPoint;
			}
		}
		else
		{
			NextAvgPt( Index, Count, maPrice, DataCount, Sum );
			if( FDrawHaveData[ Index ] == true )
			{
				SMAPoint( Index, DataCount, Sum, AvgPoint );
				GPPGraphics.DrawLine( &DrawPen,(int)FLastAvgPoint.x,(int)FLastAvgPoint.y,(int)AvgPoint.x,(int)AvgPoint.y );
				FLastAvgPoint = AvgPoint;
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawSMAGDI( TCanvas* canvas, int Count, MAPrice maPrice, TColor color, int BeginIndex, int EndIndex )
{
	TPoint AvgPoint;
	int    DataCount  = 0;
	bool   FindFirstPoint = false;
	double Sum;

	canvas->Pen->Color = color;
	for( register int Index = BeginIndex; Index < EndIndex; Index++ )
	{
		if( FindFirstPoint == false )
		{
			if( FDrawHaveData[ Index ] == true  )
			{
				Sum = 0;
				FindFirstPoint = true;
				DataCount = FirstAvgPt( Index, Count, maPrice, Sum );
				SMAPoint( Index, DataCount, Sum, AvgPoint );
				FLastAvgPoint = AvgPoint;
			}
		}
		else
		{
			NextAvgPt( Index, Count, maPrice, DataCount, Sum );
			if( FDrawHaveData[ Index ] == true  )
			{
				SMAPoint( Index, DataCount, Sum, AvgPoint );
				canvas->MoveTo( FLastAvgPoint.x, FLastAvgPoint.y );
				canvas->LineTo( AvgPoint.x , AvgPoint.y );
				FLastAvgPoint = AvgPoint;
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawSMA( TCanvas* canvas, int Count, MAPrice maPrice, TColor color )
{
	int            BeginIndex = FViewPortX / FCandleWidth;
	int            EndIndex   = ( FViewPortX + FViewPortWidth ) / FCandleWidth;
	GDIClipRegion  Clip( canvas->Handle, 0,0, GetRightBottomX() , GetRightBottomY() );

	if( EndIndex >= FHaveData.Length / FMinutesPerStick )
		EndIndex = FHaveData.Length / FMinutesPerStick ;
	if( BeginIndex < Count )
		BeginIndex = Count;
	if( FGDIPlus == true )
	   DrawSMAGDIPlus( canvas, Count, maPrice, color, BeginIndex,  EndIndex );
	else
	   DrawSMAGDI( canvas, Count, maPrice, color, BeginIndex,  EndIndex );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetShowQtyBar( int ShowQtyBar )
{
	FShowQty = ShowQtyBar;
	FChartPercent = (double)(100-FShowQty) / 100.0;
	Resize();
}
//---------------------------------------------------------------------------
int __fastcall TCandleStickChart::YPosToTick( int Y )
{
	int CurrTick = (FViewPortY + Y/FViewPortToControlY )/FPixelsPerTick + 1;

	if( CurrTick < 0 )
		CurrTick = 0;
	if( CurrTick > FTickList->ItemCount() -1 )
		CurrTick = FTickList->ItemCount() -1;
	return  CurrTick;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::YPosToPrice( int Y, String& PxString )
{
	int CurrTick = YPosToTick( Y );

	FTickList->ToTNFormat( FTickList->GetPxFromIndex( CurrTick, false ), PxString );
}
//---------------------------------------------------------------------------
double __fastcall TCandleStickChart::YPosToPrice( int Y )
{
	int CurrTick = YPosToTick( Y );

	return FTickList->GetPxFromIndex( CurrTick, false );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::AlignmentCheck( int X, int& Y )
{
	int Index = XPosToIndex( X );

	if( Index != -1 && FDrawHaveData[ Index ] )
	{
		int MaxY = ( (double)( FMaxArray[Index].y - FViewPortY ) )* FViewPortToControlY;
		int MinY = ( (double)( FMinArray[Index].y - FViewPortY ) )* FViewPortToControlY;

		if( Y  < MaxY && MaxY - Y < ALIGN_DIST )
			Y = MaxY;
		else if( Y > MinY && Y - MinY < ALIGN_DIST )
			Y = MinY;
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::PaintGDI( void )
{
	TRect PaintRect( 0, 0, Width, Height );

	DrawBackground( FBuffer->Canvas, PaintRect, Focused() ); ///< Paint background
	DrawTimeLine( FBuffer->Canvas );                         ///< Draw Time line.
	DrawTickGrid( FBuffer->Canvas );                         ///< Draw price grid lines
	DrawSkyLand( FBuffer->Canvas );                          ///< Draw Sky and land line
	DrawViewPort( FBuffer->Canvas );                         ///< Paint Qty bars,K Bars, MA1~3
	DrawAxis( FBuffer->Canvas, PaintRect );                  ///< Draw X/Y Axis.
	Canvas->Draw( 0,0, FBuffer );
	if( FLastMouseX != -1 )
		DrawCross( FLastMouseX, FLastMouseY );               ///< Draw mouse cross.
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::PaintFPS( void )
{
	DWORD TickNow = UFC::GetTickCountMS();

	if( TickNow - FLastPaint < (DWORD)FPaintInterval )///< Up to MaxFPS frames per sec.
	{   ///< Skip this frame and enable time out timer.
		if( FTimer->Enabled == false )
			FTimer->Enabled = true;
	}
	else
		Paint();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::Paint( void )
{
	if( FIsChangingSymbol == false && FCanUpdate == true )
	{
		try
		{
			DWORD UseMS,EndMS;

			if( FTimer->Enabled == true )///< Disable timeout timer.
				FTimer->Enabled = false;
			FLastPaint = UFC::GetTickCountMS();
			CheckViewPortPosition();
			PaintGDI();
			UseMS = UFC::GetTickCountMS() - FLastPaint;
			if( UseMS > (DWORD)FMaxUseMS )
			{
				int LimitFPS = 800/UseMS;
				FMaxUseMS = UseMS;
				if( LimitFPS < FMaxFPS )
					SetFPS( LimitFPS );
			}
		}
		catch( Exception& ex )
		{
			AnsiString What( ex.Message );
			UFC::BufferedLog::Printf( " TCandleStickChart::Excaption(%s)", What.c_str() );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::EraseBackground( TMessage &Msg )
{
	Msg.Result = 1;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::CheckViewPortPosition( void )
{
	if( FViewPortX < 0 )
		FViewPortX = 0;
	if( FViewPortY < 0 )
		FViewPortY = 0;

	if( FViewPortX + FViewPortWidth > FVirtualWidth )
		FViewPortX = FVirtualWidth - FViewPortWidth;
	if( FViewPortY + FViewPortHeight > FVirtualHeight )
		FViewPortY = FVirtualHeight - FViewPortHeight;

}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::Zoom( bool IsZoomIn )
{
	switch ( FScaleType )
	{
		case stAuto: ZoomAuto( IsZoomIn ); break;
		case stXOnly: ZoomX( IsZoomIn ); break;
		case stYOnly: ZoomY( IsZoomIn ); break;
		case stXYBoth: ZoomBoth( IsZoomIn ); break;
		default: break;
	}
	Resize();
	Paint();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetPixelsPerTick( int Pixels )
{
	FDispPixelsPerTick = Pixels;
	if( FScaleType == stXOnly )
		SetScaleType( stXOnly );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetTicksPerGrid( int tick )
{
   FTicksPerGrid = tick;
   if( FPxGridType == pgtFix )
	   PaintFPS();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetScaleType( ScaleType Type )
{
	FScaleType = Type;
	PaintFPS();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::ZoomAuto( bool IsZoomIn )
{
	ZoomX( IsZoomIn );
	AdjustViewPortHeight();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::ZoomX( bool IsZoomIn )
{
	int DeltaX = FViewPortWidth * 0.05;   ///< 5% each time

	if( IsZoomIn && FViewPortWidth > FViewPortWidthMin ) ///< Zoom in
	{
		///< Reduce ViewPort size
		FViewPortWidth -= DeltaX;
		if( FViewPortWidth < FViewPortWidthMin )
			FViewPortWidth = FViewPortWidthMin;
		else///< Move viewport left
		{
			if( FViewPortX + FViewPortWidth < FVirtualWidth )
				FViewPortX += DeltaX / 2;
			else
				FViewPortX = FVirtualWidth - FViewPortWidth;
		}
	}
	else if( !IsZoomIn && FViewPortWidth < FVirtualWidth ) ///< Zoom Out
	{
		FViewPortWidth += DeltaX;
		if( FViewPortWidth > FViewPortWidthMax )
			FViewPortWidth = FViewPortWidthMax;
		if( FViewPortX > 0 )
		{
			FViewPortX -= DeltaX / 2;
			if( FViewPortX < 0 )
				FViewPortX = 0;
		}
		else
			FViewPortX = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::ZoomY( bool IsZoomIn )
{
	int DeltaY = FViewPortHeight * 0.05;
	if( IsZoomIn && FViewPortHeight > 100 ) ///< Zoom In
	{
		FViewPortHeight -= DeltaY;
		if( FViewPortHeight < 100 )
			FViewPortHeight = 100;
		else
		{
			if( FViewPortY + FViewPortHeight < FVirtualHeight )
				FViewPortY += DeltaY / 2;
			else
				FViewPortY = FVirtualHeight - FViewPortHeight;
		}
	}
	else if( !IsZoomIn && FViewPortHeight < FVirtualHeight )  ///< Zoom out
	{
		FViewPortHeight += DeltaY;
		if( FViewPortHeight > FVirtualHeight )
			FViewPortHeight = FVirtualHeight;
		if( FViewPortY > 0 )
		{
			FViewPortY -= DeltaY / 2;
			if( FViewPortY < 0 )
				FViewPortY = 0;
		}
		else
			FViewPortY = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::ZoomBoth( bool IsZoomIn )
{
	ZoomX( IsZoomIn );
	ZoomY( IsZoomIn );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::MouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta, const Types::TPoint &MousePos, bool &Handled)
{
	HandleMouseLeave(this);
	if( WheelDelta < 0 )
		Zoom( false );
	else
		Zoom( true );
	Handled = false;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::MouseEnter(  System::TObject* Sender )
{
	Application->BringToFront();
	if( ::GetFocus( ) != Handle )
		::SetFocus( Handle );
	SetMouseUpCursor();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::CenterLastTick( void )
{
	if( FEditMode == false )
	{
		if( FScaleType == stAuto )
		{
			FViewPortX = FLastIndex * FCandleWidth - FViewPortWidth + FRightSpace*FCandleWidth; //; - FViewPortWidth / 2 ;
			FViewPortY = FLastTick - FViewPortHeight / 2 ;
			CheckViewPortPosition();
			AdjustViewPortHeight();
		}
		else
		{
			int ViewPortH;

			if( FShowQty != 0 )
				ViewPortH = (double)( Height - FXAxisOffset ) * FChartPercent/FViewPortToControlY;
			else
				ViewPortH = (double)( Height - FXAxisOffset ) /FViewPortToControlY;
			FViewPortX = FLastIndex * FCandleWidth - FViewPortWidth + FRightSpace*FCandleWidth;// - FViewPortWidth / 2 ;
			FViewPortY = FLastTick - ViewPortH / 2 ;

			CheckViewPortPosition();
		}
		Resize();
		PaintFPS();
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::EraseCross( bool ClearFlag )
{
	if( FLastMouseX != -1 ) ///< Erase old cross line
		DrawCross( FLastMouseX, FLastMouseY );
	if( ClearFlag == true )
		FLastMouseX = -1;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetCursor( TCursor WinCursor, TCursor ScrCursor )
{
	Screen->Cursor = ScrCursor;
	Cursor = WinCursor;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::PenViewport( int X, int Y)
{
	int deltaX = X - FMouseDownPos.x;
	int deltaY = Y - FMouseDownPos.y;

	SetCursor( crSizeAll, crSizeAll );
	FViewPortX = FMouseDownViewPortPos.x - deltaX / FViewPortToControlX;
	FViewPortY = FMouseDownViewPortPos.y - deltaY / FViewPortToControlY;
	CheckViewPortPosition();
	AdjustViewPortHeight();
	Resize();
	Paint();
}
//---------------------------------------------------------------------------
bool __fastcall TCandleStickChart::HitHLLine(  int X, int Y)
{
	if( FUseHLSection == false )
	{
		if(	FSectionHLine.HitTest( X, Y ) == true ||
			FSectionLLine.HitTest( X, Y )== true  )
			return true;
    }
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::MouseMove( Classes::TShiftState Shift, int X, int Y)
{
	FMousePos.x = X;
	FMousePos.y = Y;

	if( FMouseDown == true ) ///< Mouse down
	{
		if( FReplacePx == true ) ///< Peplace price, change the cursoe
		{
			double OrderPx = YPosToPrice( Y );

			SetCursor( crSizeNS, crSizeNS );
			FHitWorkingOrder->SetMovingPrice( OrderPx );
			Paint();
			return;
		}
		else
		{
			TPoint NewPoint(X, Y);

			if( FEditMode == true  ) ///< Edit mode
			{
				if( FEditType == etDelete )
				{
					FCanUpdate = false;
					if( FLastMouseX != -1 ) ///< Erase old Rect
						DrawRect( FLastMouseX, FLastMouseY );
					DrawRect( X, Y);
				}
				else if( FEditType == etDefault ) ///< Pen the viewport.
				{
					if( FMoveGraph == NULL ) ///< No selected object.
						PenViewport( X, Y);
					else
					{
						FCanUpdate = false;
						if( FLastMouseX != -1 )
							FMoveGraph->Paint( FLastMouseX,FLastMouseY, FObjDeltaX, FObjDeltaY, Canvas );
						FMoveGraph->Paint( X, Y, FObjDeltaX, FObjDeltaY, Canvas );
						FLastMouseX = X;
						FLastMouseY = Y;
					}
				}
				else if( FEditType == et2PLine )
					return;
				else if( FEditType == et2PRect)
				{
                    FCanUpdate = false;
                    if( FLastMouseX != -1 ) ///< Erase old Rect
						DrawRect( FLastMouseX, FLastMouseY, 1);
					DrawRect( X, Y, 1);
				}
			}
			else if( FDrawRect.Contains( NewPoint ) ) ///< Pen the viewport.
			{
				if( FSelectSectionLine != NULL )
				{
					String PxStr;
					TPoint Pt = ClientToScreen( TPoint( 0, Y ) );

					YPosToPrice( Y, PxStr );
					SetCursor( crSizeNS, crSizeNS );
					FCanUpdate = false;
					if( FLastMouseX != -1 )
						FSelectSectionLine->Paint( FLastMouseX,FLastMouseY, FObjDeltaX, FObjDeltaY, Canvas );
					FSelectSectionLine->Paint( X, Y, FObjDeltaX, FObjDeltaY, Canvas );
					ShowPriceHint( Pt.x, Pt.y + 1, PxStr);
					FLastMouseX = X;
					FLastMouseY = Y;
				}
				else
					PenViewport( X, Y);
			}

		}
	}
	else
	{
		TPoint NewPoint( X, Y );

		if( FDrawRect.Contains( NewPoint ) ) ///< In K Bar area.( not include Time,Price area )
		{
			if( (FMouseIndex = XPosToIndex( X )) != -1 )
			{
				TPoint ClentPt( X, GetDataYPos( FMouseIndex, Y ) );
				TPoint ScreenPx = ClientToScreen( ClentPt );

				if( FEditMode == true ) ///< Drawing mode.
				{
					if( FEditType == et2PLine || FEditType == etText || FEditType == et2PRect)
						SetCursor( crCross, crCross );
					else if( FEditType == etDefault )
						SelectObjectCursor(  X, Y , crArrow );
					else if( FEditType == etDelete )
						SelectObjectCursor(  X, Y , crArrow );
					else
						SetCursor( TCursor( 6 ), crDefault );
					ShowInfoHint(  X, Y, ScreenPx );
					HideOrdereHint();
					EraseCross( false );///< Erase old cross line
					DrawCross( X, Y ); ///< Draw new cross line
				}
				else  ///< Pick time/Price Place order mode.
				{
					HitResult Result = olNone;

					if( FEnableTrade == true )
						Result = HitPositionLine( X, Y );
					if( Result != olNone  )  ///< Hit Working/Stop Order
					{
						HideDataHint();
						EraseCross( true );
						if( Result == olDelete )
							Screen->Cursor = crHandPoint;
						else
							Screen->Cursor = crSizeNS;
					}
					else if( HitHLLine( X, Y ) == true  )
					{
						HideDataHint();
						EraseCross( true );
						SetCursor( crSizeNS, crSizeNS );
					}
					else
					{
						SetCursor( TCursor( 6 ), crDefault );
						ShowInfoHint(  X, Y, ScreenPx );
						ShowOrdereHint( Y );
						EraseCross( false );
						FCursorType = ctCross;
						DrawCross( X, Y ); ///< Draw new cross line
					}
				}
			}
		}
		else if( FPriceRect.Contains( NewPoint ) ) ///< In Price area.
		{
			TPoint ScreenPx = ClientToScreen( NewPoint );

			if( FEditMode == false )
			{
				SetCursor( crHandPoint, crDefault );
				ShowOrdereHint( Y );
			}
			else
			{
				SetCursor( TCursor( 6 ), crDefault );
				HideOrdereHint();
			}
			HideTimeHint( );
			HideDataHint( );
			EraseCross( true );
			FCursorType = ctHLine;
			ShowInfoHint(  X, Y, ScreenPx );
			DrawCross( X, Y ); ///< Draw new cross line
		}
		else if( FTimeRect.Contains( NewPoint ) ) ///< In Timr area.
		{
			SetCursor( TCursor( 6 ), crDefault );
			HideInfoHint();
			HideOrdereHint();
			EraseCross( true );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::MouseDown( Controls::TMouseButton Button, Classes::TShiftState Shift,int X,int Y)
{
	TPoint DownPoint = TPoint( X,Y );

	if( Button == mbLeft )
	{
		FMouseDown = true;
		FMouseDownPos.x = X;
		FMouseDownPos.y = Y;
		FMouseDownViewPortPos.x = FViewPortX;
		FMouseDownViewPortPos.y = FViewPortY;

		if( FPriceRect.Contains( DownPoint ) && FEditMode == false  ) ///< Place Order
		{
			 nsOrderMessageDefine::SideEnum Side = sBuy;
			 double OrderPx = YPosToPrice( Y );

			if( X > Width - FYAxisOffset/2 )
				Side = sSell;
			if( FOnNewOrder != NULL && FEnableTrade == true  )
			{
				FOnNewOrder( this, Side, OrderPx );
				FMouseDown = false;
			}
			return;
		}
		HandleMouseLeave( this );
		if( FEditMode == true || FEnableTrade == false  )
		{
			SetMouseDownCursor();
			if( FEditMode == true )
				CreateGraph( X, Y );
			return;
		}
        HitResult Result = HitPositionLine( X, Y );
		switch (Result)
		{
			case olDelete:
                if( FHitWorkingOrder->IsStopOrder() == false)
				{
					if( FOnDelete != NULL )
						FOnDelete( this, FHitWorkingOrder->GetSide(), FHitWorkingOrder->GetPrice() );
					break;
				}
				if( FOnDeleteStopOrder != NULL )
					FOnDeleteStopOrder( this, FHitWorkingOrder->GetSide(), FHitWorkingOrder->GetPrice() );
				break;
			case olMove:
                FReplacePx = true;
				SetCursor( crSizeNS, crSizeNS );
				break;
			default:
				if( FUseHLSection == true )
				{
					FSelectSectionLine = NULL;
					return;
				}
				if(	FSectionHLine.HitTest( X, Y ) == true )
				{
					FSelectSectionLine = &FSectionHLine;
					SetCursor( crSizeNS, crSizeNS );
				}
				else if( FSectionLLine.HitTest( X, Y )== true  )
				{
					FSelectSectionLine = &FSectionLLine;
					SetCursor( crSizeNS, crSizeNS );
				}
				else
					FSelectSectionLine = NULL;
		}

		SetMouseDownCursor( );
		if( FEditMode == true )
			CreateGraph( X, Y );
	}
	else if( Button == mbRight )
	{
		if( FPriceRect.Contains( DownPoint ) && FEditMode == false  ) ///< Place Order
		{
			TPoint ScrPt = ClientToScreen( DownPoint );

			FStopPx = YPosToPrice( Y );
			PopupMenu->Popup( ScrPt.x, ScrPt.y );
		}
		else
			CenterLastTick( );
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift,int X,int Y)
{
	TPoint NewPoint( X, Y );

	if( Button == mbLeft )
		FMouseDown = false;
	if( FEditMode == true )
	{
		if( FEditType == etDelete )
		{
			int x1 = FMouseDownPos.x;
			int x2 = X;
			int y1 = FMouseDownPos.y;
			int y2 = Y;

			if( FMouseDownPos.x > X )
			{
				x1 = X;
				x2 = FMouseDownPos.x;
			}
			if( FMouseDownPos.y > Y )
			{
				y1 = Y;
				y2 = FMouseDownPos.y;
			}

			TRect DelRect( x1,y1, x2, y2 );

			if( FLastMouseX != -1 ) ///< Erase old Rect
			{
				DrawRect( FLastMouseX, FLastMouseY );
				FLastMouseX = -1;
			}
			FCanUpdate = true;
			if( DeleteGraphs( DelRect ) == true )
				PaintFPS();
		}
		else if(FEditType == et2PRect)
		{
			FCanUpdate = true;
			AddRect(X, FMouseDownPos.x, Y, FMouseDownPos.y);
			Paint();
		}
		else if( FEditType == etDefault && FMoveGraph != NULL )
		{
			int IndexX =  XPosToIndex( X - FObjDeltaX );
			int TickY  =  YPosToTick( Y - FObjDeltaY );

			FCanUpdate = true;
			FMoveGraph->MoveTo( IndexX, TickY );
			FLastMouseX = -1;
			FMoveGraph = NULL;
			PaintFPS();
		}
	}
	if( FReplacePx == true && FEnableTrade == true  )
	{
		FReplacePx = false;
		FHitWorkingOrder->StopMoving();

		nsOrderMessageDefine::SideEnum side = FHitWorkingOrder->GetSide();
		double Price = FHitWorkingOrder->GetPrice();
		double NewPrice  = YPosToPrice( Y );

		if( FHitWorkingOrder->IsStopOrder() == false )
		{
			if( FOnReplacePx != NULL )
				FOnReplacePx( this, side, Price, NewPrice );
		}
		else
		{
			if( FOnStopOrderReplacePx != NULL )
			{
				int    Qty = FHitWorkingOrder->GetQty();
				double ExecPx = 0; ///< Market
				nsOrderMessageDefine::OrderTypeEnum Type;


				int NewPxTick = FTickList->PxToTickIndex( NewPrice,false);
				int Tick;

				if( side == nsOrderMessageDefine::sBuy )
				{
					Tick = NewPxTick - FBuyStopTick;
					Type = FBuyStopOrderOrdType;
				}
				else
				{
					Tick = NewPxTick - FSellStopTick;
					Type = FSellStopOrderOrdType;
				}
				if( Type == nsOrderMessageDefine::otLimit )
				{
					if( Tick >= 0 || Tick < FTickList->ItemCount() )
						ExecPx = FTickList->GetPxFromIndex( Tick, false );
					else
						return;
				}
				FOnStopOrderReplacePx( this, side, Type, Qty, Price, NewPrice, ExecPx );
			}
		}
	}
	if( FDrawRect.Contains( NewPoint ) )
	{
		if( FSelectSectionLine != NULL )
		{
			int IndexX =  XPosToIndex( X - FObjDeltaX );
			int TickY  =  YPosToTick( Y - FObjDeltaY );

			FCanUpdate = true;
			FSelectSectionLine->MoveTo( IndexX, TickY );
			FLastMouseX = -1;
			FSelectSectionLine = NULL;
			PaintFPS();
		}
		else
		{
			if( FLastMouseX != -1 ) ///< Errase old cross line
				DrawCross( FLastMouseX, FLastMouseY );
			DrawCross( X, Y ); ///< Draw new cross line
			SetMouseUpCursor();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::ChangeCursor( void )
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
	Screen->Cursors[ 6 ] = CreateIconIndirect(iconInfo.get());
	Cursor = TCursor( 6 );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::HandleMouseLeave( TObject* )
{
	if( FLastMouseX != -1 )
	{
		DrawCross( FLastMouseX, FLastMouseY ); ///< Errase old cross line
		FLastMouseX = -1;
		FLastMouseY = -1;
	}
//	Screen->Cursor = crDefault;
	HideInfoHint();
	if(	FOrderHint != NULL )
		FOrderHint->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::WndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{

		case WM_ERASEBKGND:	EraseBackground( Msg );
							return;
		case WM_TIMER:
		case WM_KILLFOCUS:
		case WM_SETFOCUS:   Paint();
							break;
		case WM_GETDLGCODE: Msg.Result = DLGC_WANTARROWS;
                            return;
	}
	TCustomControl::WndProc(Msg);
}
//---------------------------------------------------------------------------
int __fastcall TCandleStickChart::GetMaxQty( void )
{
	int MaxQty = 0;
	for( register int i = 0; i < FDrawQty.Length; i++ )
	{
		if( FDrawHaveData[i] && FDrawQty[i] > MaxQty )
			MaxQty = FDrawQty[i];
	}

	if( MaxQty == 0 )
		return 100;
	if( MaxQty % 100 == 0 )
		MaxQty += 1;

	return MaxQty;
}
//---------------------------------------------------------------------------
int __fastcall TCandleStickChart::XPosToIndex( int X )
{
	 int Index = ( X / FViewPortToControlX + FViewPortX ) / FCandleWidth;
	 if( Index >= 0 && Index < FDrawQty.Length )
		return Index;
	 return -1;
}
//---------------------------------------------------------------------------
int  __fastcall TCandleStickChart::GetDataYPos( int Index, int Y )
{
	int   YOffset;

	if( FDrawHaveData[ Index ] == false )
		return Y;

	YOffset = ( (double)( FBottomRightArray[Index].y - FViewPortY ) )* FViewPortToControlY;
	return YOffset;
}
//---------------------------------------------------------------------------
int  __fastcall TCandleStickChart::GetDataXPos( int Index, int X )
{
	int   XOffset;

	if( FDrawHaveData[ Index ] == false )
		return X;

	XOffset = ( (double)( FMinArray[Index].x - FViewPortX ) )* FViewPortToControlX;
	return XOffset;
}
//---------------------------------------------------------------------------
TRect __fastcall TCandleStickChart::ToScreenRect( int l, int t, int r, int b )
{
	TPoint LeftPt = ClientToScreen( TPoint(l,t) );
	TPoint RightDownPt = ClientToScreen( TPoint(r,b) );
	TRect  ScreenRect( LeftPt.x, LeftPt.y, RightDownPt.x, RightDownPt.y );

	return ScreenRect;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::ShowTimeHint( int X, int Y, const UFC::AnsiString& Time )
{
	if( FTimeHint == NULL )
	{
		FTimeHint = new TTextHint( this, Font );
		FTimeHint->Init( Color, FTextColor, L"23:59" );
	}
	FTimeHint->SetString( Time.c_str() );
	FTimeHint->SetPosition( X, Y );
	if( FTimeHint->Visible == true )
		FTimeHint->Paint();
	else
		FTimeHint->SetVisible();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::ShowPriceHint( int X, int Y, const String& PxStr )
{
	if( FPriceHint == NULL )
	{
		String MaxStr;

		MaxStr.printf( L"%.*f", FDigit, FInfo->GetBullPrice() );
		FPriceHint = new TTextHint( this, Font );
		FPriceHint->Init( Color, FTextColor,  MaxStr  );
	}
	FPriceHint->SetString( PxStr  );
	FPriceHint->SetPosition( X, Y );
	if( FPriceHint->Visible == true )
		FPriceHint->Paint();
	else
		FPriceHint->SetVisible();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::ShowOrdereHint( int Y )
{
	int    X = Width - FYAxisOffset;
	TPoint Pt = ClientToScreen( TPoint(X,Y) );
	String PxStr, Str;

	if( FOrderHint == NULL )
	{
		FOrderHint = new TOrderHint( this, Font );
		FOrderHint->Init( FYAxisOffset, FXAxisOffset + FXAxisOffset/2 );
	}
	YPosToPrice( Y, PxStr );
	FOrderHint->SetString( PxStr );
	FOrderHint->SetPosition( Pt.x, Pt.y - FXAxisOffset / 2 );
	if( FOrderHint->Visible == true )
		FOrderHint->Paint();
	else
		FOrderHint->SetVisible();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::ShowDataHint(  TPoint& ScreenPx, TRect& ScreenRect, int Index, const UFC::AnsiString& Time )
{
	int    Hour,Minutes;

	if( FHintwin == NULL )
	{
		FHintwin = new TInfoHint( this );
		FHintwin->ChangeShape();
		FHintwin->SetTickList( FTickList );
	}
	if( FDrawHaveData[ Index ] )
	{
		Hour    = Time.SubString( 0, Time.AnsiPos( ':' )).ToInt();
		Minutes = Time.SubString( Time.AnsiPos( ':' ) + 1, Time.Length() - Time.AnsiPos( ':' ) ).ToInt();
		FHintwin->SetData( Hour,Minutes,
						   FDrawOpenPx[ Index ],
						   FDrawMaxPx[ Index ] ,
						   FDrawMinPx[ Index ] ,
						   FDrawClosePx[ Index ],
						   FDrawQty[ Index ], FDigit );
	}
	else
	{
		FHintwin->Empty();
		FHintwin->Hide();
		return;
	}
	FHintwin->SetPosition( ScreenPx.x, ScreenPx.y );
	if( ScreenRect.Contains( ScreenPx )==true )
	{
		if( FHintwin->Visible == true )
			FHintwin->Paint();
		else
			FHintwin->SetVisible();
	}
	else
		FHintwin->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::ShowInfoHint( int X, int Y, TPoint& ScreenPx )
{
	if( !ComponentState.Contains( csDesigning ) )
	{
		UFC::AnsiString Time;
		String          PxStr;
		int             DayCandle = FDayMinutes / FMinutesPerStick;
		int             Day;

		if( FDayMinutes % FMinutesPerStick )
			DayCandle += 1;
		Day = FMouseIndex / DayCandle;
		FExInfo->IndexToTime( Day * FDayMinutes + ( FMouseIndex % DayCandle ) * FMinutesPerStick , Time );
		AlignmentCheck( X, Y );
		YPosToPrice(  Y, PxStr );
		if( Visible == true )
		{
			TRect  ScreenRect = ToScreenRect( 0,0, Width,Height );
			TPoint Pt = ClientToScreen( TPoint(0,Y) );

			if( FEditMode == false )
			{
				if( FCursorType == ctVLine )
					ShowTimeHint( ScreenPx.x + 1, ScreenRect.Top, Time );
				if( FCursorType == ctHLine )
					ShowPriceHint( Pt.x, Pt.y + 1, PxStr);
				else
				{
					ShowPriceHint( Pt.x, Pt.y + 1, PxStr);
					ShowTimeHint( ScreenPx.x + 1, ScreenRect.Top, Time );
					ShowDataHint( ScreenPx, ScreenRect, FMouseIndex, Time );
				}
			}
			else
			{
				if( FEditType == etVLine )
					ShowTimeHint( ScreenPx.x + 1, ScreenRect.Top, Time );
				else if( FEditType == etHLine )
					ShowPriceHint( Pt.x, Pt.y + 1, PxStr);
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::HideOrdereHint( void )
{
	if( FOrderHint != NULL )
		FOrderHint->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::HideTimeHint( void )
{
	if( FTimeHint != NULL )
		FTimeHint->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::HidePriceHint( void )
{
	if( FPriceHint != NULL )
		FPriceHint->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::HideDataHint( void )
{
	if( FHintwin != NULL )
		FHintwin->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::HideInfoHint( void )
{
	HideDataHint();
	HideTimeHint();
	HidePriceHint();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetBullColor( TColor color )
{
	FBullColor = color;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetBearColor( TColor color )
{
	FBearColor = color;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetTextColor( TColor color )
{
	FTextColor = color;
	if( FTimeHint != NULL )
	{
		FTimeHint->SetColor( FTextColor );
		FTimeHint->SetBKColor( Color );
	}
	if( FPriceHint != NULL )
	{
		FPriceHint->SetColor( FTextColor );
		FPriceHint->SetBKColor( Color );
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetGridColor( TColor color )
{
	FGridColor = color;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetCandleLineColor( TColor color )
{
	FCandleLineColor = color;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetMAColor1( TColor color )
{
	FMAColor1 = color;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetMAColor2( TColor color )
{
	FMAColor2 = color;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetMAColor3( TColor color )
{
	FMAColor3 = color;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetShowMA1( bool ShowMA )
{
	FShowMA1 = ShowMA;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetShowMA2( bool ShowMA )
{
	FShowMA2 = ShowMA;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetShowMA3( bool ShowMA )
{
	FShowMA3 = ShowMA;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetMACount1( int Count )
{
	FMACount1 = Count;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetMACount2( int Count )
{
	FMACount2 = Count;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetMACount3( int Count )
{
	FMACount3 = Count;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetMAPrice1( MAPrice PxType )
{
	FMAPrice1 = PxType;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetMAPrice2( MAPrice PxType )
{
	FMAPrice2 = PxType;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetMAPrice3( MAPrice PxType )
{
	FMAPrice3 = PxType;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetMVType1( MVType type )
{
	FMVType1 = type;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetMVType2( MVType type )
{
	FMVType2 = type;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetMVType3( MVType type )
{
	FMVType3 = type;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetDate( int Index, const UFC::AnsiString& Date )
{
	FDateList.Add( Date );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetGradientCandle( bool GradientCandle )
{
	FGradientCandle = GradientCandle;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetPxGridType( PxGridType Type )
{
	FPxGridType = Type;
	Resize();
	PaintFPS();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetTimeGridType( TimeGridType Type )
{
	FTimeGridType = Type;
	Resize();
	PaintFPS();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetShowBox( bool Show)
{
	FShowBox = Show;
	PaintFPS();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetShowDayHL( bool Show)
{
	FShowDayHL = Show;
	PaintFPS();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetShowAvgPx( bool Show)
{
	FShowAvgPx = Show;
	PaintFPS();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetHLSection( SectionType Section )
{
	FHLSection = Section;
	PaintFPS();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetSectionLineColor( TColor SLColor )
{
	FSectionLineColor = SLColor;
	if( FHLSection != stNoSection )
		PaintFPS();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetXScale( double Ratio )
{
	if( Ratio > 1.0 )
		Ratio = 1.0;
	FViewPortWidth = FVirtualWidth*Ratio;
	if( FViewPortWidth < FViewPortWidthMin )
		FViewPortWidth = FViewPortWidthMin;
	if( FViewPortX + FViewPortWidth > FVirtualWidth )
		FViewPortX = FVirtualWidth - FViewPortWidth;
	PaintFPS();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetYScale( double Ratio )
{
	if( Ratio > 1.0 )
		Ratio = 1.0;
	FViewPortHeight = FVirtualHeight*Ratio;
	if( FViewPortHeight < 100 )
		FViewPortHeight = 100;
	if( FViewPortY + FViewPortHeight > FVirtualHeight )
		FViewPortY = FVirtualHeight - FViewPortHeight;
	PaintFPS();
}
//---------------------------------------------------------------------------
double __fastcall TCandleStickChart::GetXScale( void )
{
	return (double)FViewPortWidth/(double)FVirtualWidth;
}
//---------------------------------------------------------------------------
double __fastcall TCandleStickChart::GetYScale( void )
{
	return (double)FViewPortHeight/(double)FVirtualHeight;
}
//---------------------------------------------------------------------------
namespace Candlestickchart
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TCandleStickChart)};
		RegisterComponents(L"Speedy", classes, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawBackground(  TCanvas* canvas, TRect& BufferRect, bool IsFocus )
{
	int    Start = FViewPortX / FCandleWidth;
	int    End = ( FViewPortX + FViewPortWidth ) / FCandleWidth;
	int    DayCandle = FDayMinutes / FMinutesPerStick;
	int    StartOffhour = DayCandle* DAYS;
	TColor OddBKColor;
	TColor EvenBKColor;

	if( FDayMinutes % FMinutesPerStick )
		DayCandle += 1;
	if( IsFocus )
	{
		OddBKColor = (TColor)RGB( Min0( GetRValue( Color ), 10),
								  Min0(	GetGValue( Color ), 10),
								  Min0( GetBValue( Color ), 10) );
		EvenBKColor = Color;
	}
	else
	{
		OddBKColor = (TColor)RGB( Min0( GetRValue( Color ), 20),
								  Min0( GetGValue( Color ), 20),
								  Min0( GetBValue( Color ), 20) );
		EvenBKColor = (TColor)RGB( Min0( GetRValue( Color ),10),
								   Min0( GetGValue( Color ),10),
								   Min0( GetBValue( Color ),10) );
	}
	canvas->Brush->Color = Color;
	canvas->FillRect( BufferRect );

	int StartQuotient = Start / ( DayCandle );
	int EndQuotient = End / ( DayCandle );

	if( EndQuotient == DAYS )
		EndQuotient--;
	GDIClipRegion  Clip( canvas->Handle, 0,0, GetRightBottomX() , Height - FXAxisOffset );

	///< Draw Header
	TRect HeaderRect = Rect( ( Start * 10 - FViewPortX  ) * FViewPortToControlX,
							 0,
							 ( ( StartQuotient + 1 ) * ( DayCandle ) * 10 - FViewPortX )  * FViewPortToControlX,
							 Height - FXAxisOffset );
	if( StartQuotient < DAYS -1 )
	{
		if( StartQuotient % 2 == 0 )
			canvas->Brush->Color = EvenBKColor;
		else
			canvas->Brush->Color = OddBKColor;
	}
	else
		canvas->Brush->Color = EvenBKColor;
	canvas->FillRect( HeaderRect );
	for( int i = StartQuotient + 1; i < EndQuotient; i++ )
	{
		TRect BodyRect = Rect( (double)( i * ( DayCandle ) * 10 - FViewPortX ) * FViewPortToControlX ,
							  0,
							  ( (i + 1)* DayCandle * 10 - FViewPortX ) * FViewPortToControlX,
							  Height - FXAxisOffset );
		if( i < DAYS -1 )
		{
			if( i % 2 == 0 )
				canvas->Brush->Color = EvenBKColor;
			else
				canvas->Brush->Color = OddBKColor;
		}
		else
			canvas->Brush->Color = EvenBKColor;
		canvas->FillRect( BodyRect );
	}
	///< Draw tail
	TRect EndRect = Rect( ( EndQuotient * ( DayCandle ) * 10 - FViewPortX ) * FViewPortToControlX,
						  0,
						  Width - + FYAxisOffset,
						  Height - FXAxisOffset );
	if( EndQuotient < DAYS -1 )
	{
		if( EndQuotient % 2 == 0 )
			canvas->Brush->Color = EvenBKColor;
		else
			canvas->Brush->Color = OddBKColor;
	}
	else
		canvas->Brush->Color = EvenBKColor;
	canvas->FillRect( EndRect );
	canvas->Brush->Color = Color;
}
//---------------------------------------------------------------------------
int __fastcall TCandleStickChart::AutoIntervalPerGrid ( TCanvas* canvas, int HalfTimeStrWidth )
{
	float IntervalStepMin = HalfTimeStrWidth * 2 * 1.5;
	int   Step, TimeIntervalPerGrid;

	if( FMinutesPerStick < 10 )
		Step = 5;
	else
		Step = 2;
	for( TimeIntervalPerGrid = Step; ; TimeIntervalPerGrid += Step )
	{
		if( (float)TimeIntervalPerGrid * FCandleWidth * FViewPortToControlX > IntervalStepMin )
			break;
	}
	return TimeIntervalPerGrid;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawTimeLine( TCanvas* canvas )
{
	UFC::AnsiString TimeStr( "13:45" );
	int Start = FViewPortX / FCandleWidth;
	int End = ( FViewPortX + FViewPortWidth ) / FCandleWidth;
	int Step, TimeIntervalPerGrid, HalfTimeStrWidth;

	if( End > FDrawHaveData.Length )
		End = FDrawHaveData.Length;
	canvas->Font->Size  = FFontSize;
	canvas->Pen->Color  = FGridColor;//clGray;
	canvas->Font->Color = FTextColor;//clGray;
	HalfTimeStrWidth = canvas->TextWidth( "13:45" ) / 2;
	switch( FTimeGridType )
	{
		case tgt10min: if( FMinutesPerStick > 10 )
						   TimeIntervalPerGrid = 1;
					   else
						   TimeIntervalPerGrid = 10 / FMinutesPerStick;
					   break;
		case tgt15min: if( FMinutesPerStick > 15 )
						   TimeIntervalPerGrid = 1;
					   else
						   TimeIntervalPerGrid = 15 / FMinutesPerStick;
					   break;
		case tgt30min:if( FMinutesPerStick > 30 )
						   TimeIntervalPerGrid = 1;
					   else
						   TimeIntervalPerGrid = 30 / FMinutesPerStick;
					   break;
		case tgtAuto:
		default:       TimeIntervalPerGrid = AutoIntervalPerGrid( canvas, HalfTimeStrWidth );
					   break;
	}

	for( register int i = Start; i < End; i++ )
	{
		///< Draw Date
		int DayCandle = FDayMinutes / FMinutesPerStick;
		if( FDayMinutes % FMinutesPerStick )
			DayCandle += 1;
		int Index = i / DayCandle;

		{
			GDIClipRegion Clip( canvas->Handle, 0,0,  Width - FYAxisOffset, Height - FXAxisOffset );

			if( i % DayCandle == 0 && FDateList.ItemCount() > 0 &&
				FDateList.ItemCount() >= Index && Index < DAYS ) ///< Draw Date
			{
				UFC::AnsiString DateString = FDateList[ Index ];
				UFC::AnsiString DateStr;
				TTextFormat Formats;

				Formats<<tfCenter<<tfVerticalCenter;
				canvas->Font->Size = FFontSize;
				if( DateString.Length() >= 8 )
					DateStr.Printf( "%s/%s", DateString.SubString( 4, 2 ).c_str(), DateString.SubString( 6, 2 ).c_str() );
				else
					DateStr = DateString;

				int TextCenterPos = ( (double)( i * FCandleWidth - FViewPortX ) )* FViewPortToControlX;
				int DateWidth = canvas->TextWidth( DateStr.c_str() );
				int DateHeight = canvas->TextHeight( DateStr.c_str() );
				TRect TextRect = TRect( TextCenterPos, (Height - FXAxisOffset ) * FChartPercent - DateHeight, TextCenterPos + DateWidth, (Height - FXAxisOffset ) * FChartPercent );
				String DrawStr(  DateStr.c_str() );

				canvas->Brush->Style = bsClear;
				canvas->TextRect( TextRect, DrawStr, Formats );
			}
		}

		///< Draw Time string
		if( i % TimeIntervalPerGrid == 0 && i*FCandleWidth >= FViewPortX )
		{
			if( FExInfo != NULL )
			{
				int Day = i / DayCandle;

				FExInfo->IndexToTime( Day * FDayMinutes + ( i % DayCandle ) * FMinutesPerStick , TimeStr );
				canvas->Font->Size = FFontSize;
				if( i != 0 && (i * FMinutesPerStick) % FDayMinutes == 0 )
				{
					canvas->Pen->Style = psDashDotDot;
					canvas->Pen->Width = 2;
				}
				else
				{
					canvas->Pen->Style = psSolid;
					canvas->Pen->Width = 1;
				}
				int TextCenterPos = ( (double)( i * FCandleWidth - FViewPortX ) )* FViewPortToControlX;
				TRect TextRect = TRect( TextCenterPos - HalfTimeStrWidth, Height - FXAxisOffset, TextCenterPos + HalfTimeStrWidth, Height );
				String DrawStr(  TimeStr.c_str() );
				TTextFormat Formats;

				Formats<<tfCenter<<tfVerticalCenter;
				canvas->TextRect( TextRect, DrawStr, Formats );
				canvas->MoveTo( TextCenterPos, 0 );
				canvas->LineTo( TextCenterPos, Height - FXAxisOffset );
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawPositionLine( TCanvas* canvas )
{
	TWorkingLine* WorkingOrder = FWorkingOrders.First();

	while( WorkingOrder != NULL )
	{
		WorkingOrder->Paint( canvas );
		WorkingOrder = FWorkingOrders.Next();
	}
	if( FShowAvgPx == true )
		FPosition.Paint( canvas );
}
//---------------------------------------------------------------------------
HitResult __fastcall TCandleStickChart::HitPositionLine( int x,int y )
{
	HitResult Result;
	TWorkingLine* WorkingOrder = FWorkingOrders.First();

	while( WorkingOrder != NULL )
	{
		if( (Result = WorkingOrder->HitTest( x,y )) != olNone )
		{
			FHitWorkingOrder = WorkingOrder;
			return Result;
		}
		WorkingOrder = FWorkingOrders.Next();
	}
	return olNone;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawSkyLand( TCanvas* canvas )
{
	if( FExableSkyLand == true )
	{
		int StartX = ( FVTodayX  - FViewPortX)* FViewPortToControlX;
		int EndX   = FVirtualWidth * FViewPortToControlX;
		int BullY  = -1*FViewPortY* FViewPortToControlY;
		int BearY  = ( FTickList->ItemCount()* FPixelsPerTick  - FViewPortY)* FViewPortToControlY;
		int SkyY   = (FSkyY - FViewPortY)* FViewPortToControlY;
		int LandY  = (FLandY - FViewPortY)* FViewPortToControlY;
		if( StartX < Width - 20 )
		{
			GDIClipRegion                Clip( canvas->Handle, 0,0, GetRightBottomX() , GetRightBottomY() );
			Gdiplus::Graphics            GPPGraphics( canvas->Handle );
			Gdiplus::Rect                UpRect( StartX, 0, EndX, SkyY );
			Gdiplus::Rect                DownRect( StartX, LandY , EndX, BearY - LandY );
			Gdiplus::Pen                 DrawPen( Gdiplus::Color(255, GetRValue(clYellow), GetGValue(clYellow), GetBValue(clYellow)), 1 );
			Gdiplus::Font                myFont( canvas->Font->Name.c_str(), canvas->Font->Size  );
			Gdiplus::SolidBrush          fontBrush( Gdiplus::Color(255, GetRValue(clYellow), GetGValue(clYellow), GetBValue(clYellow)) );

			Gdiplus::SolidBrush          BullBrush( Gdiplus::Color( 32, 255,0,0 ) );
			Gdiplus::SolidBrush          BearBrush( Gdiplus::Color( 32, 0,255,0 ) );
			String                       PxStr;

			GPPGraphics.FillRectangle( &BullBrush , UpRect );
			GPPGraphics.FillRectangle( &BearBrush , DownRect );
			GPPGraphics.DrawLine( &DrawPen, StartX, SkyY, Width , SkyY );
			GPPGraphics.DrawLine( &DrawPen, StartX, LandY, Width , LandY );
			PxStr.printf( L"%.*f", FDigit, FSkyPx );
			Gdiplus::PointF PTsky( Width - canvas->TextWidth(PxStr) - FYAxisOffset - 5, SkyY - canvas->TextHeight(PxStr) -5 );
			GPPGraphics.DrawString(  PxStr.c_str(), PxStr.Length(), &myFont, PTsky, &fontBrush );
			PxStr.printf( L"%.*f", FDigit, FLandPx );
			Gdiplus::PointF PTland( Width - canvas->TextWidth(PxStr) -FYAxisOffset - 5, LandY + 5 );
			GPPGraphics.DrawString(  PxStr.c_str(), PxStr.Length(), &myFont, PTland, &fontBrush );
	   }

	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawTickGrid( TCanvas* canvas )
{
	if( !ComponentState.Contains( csDesigning ) && FTickList != NULL ) ///< Not design time.
	{
		canvas->Font->Color = FTextColor;
		canvas->Font->Size  = FFontSize;
		canvas->Pen->Width  = 1;

		UFC::AnsiString TickStr( "9999.999" );
		int MaxTick     = FViewPortY / FPixelsPerTick;                      ///< Highest price
		int TickStepMin = canvas->TextHeight( TickStr.c_str() ) * 1.5;      ///< Tick hight in pixels.
		int TickUnit    = UFC::DoubleToInt( FTickList->GetPxFromIndex( MaxTick, FDebugMode ) - FTickList->GetPxFromIndex( MaxTick + 1, FDebugMode ) ,  FInfo->GetDigit() );
		int IntPx       = UFC::DoubleToInt( FTickList->GetPxFromIndex( MaxTick, FDebugMode ), FInfo->GetDigit() );
		int QtyUpperLineY = (int)(Height - FXAxisOffset );
		int InitTick;
		int TicksPerGrid;

		if( FShowQty != 0 )
			QtyUpperLineY*= FChartPercent;
		GDIClipRegion Clip( canvas->Handle, 0,0,  Width, QtyUpperLineY );

		switch( FPxGridType )
		{
			case pgtFix:  TicksPerGrid = FTicksPerGrid;
						  InitTick = MaxTick + ( IntPx % (TickUnit * 5) )/TickUnit;
						  break;
			case pgtAuto: if( FPixelsPerTick * FViewPortToControlY > TickStepMin ) ///< 1 Tick height > Tick hight in pixels.
						  {
							   TicksPerGrid = 1;   ///< 1 tick per grid line
							   InitTick = MaxTick;
						  }
						  else
						  {
							   for( TicksPerGrid = 5; ; TicksPerGrid += 5 ) ///< find 5,10,15... ticks per grid line
							   {
									if( (float)TicksPerGrid * FPixelsPerTick * FViewPortToControlY > TickStepMin )
										break;
							   }
							   InitTick = MaxTick + ( IntPx % (TickUnit * 5) )/TickUnit;
						  }
						  break;
			case pgtAOE:  if( FBoxExists == true )
							  TicksPerGrid = FRiskRatio * AOETicksPerGrid;
						  else
							  TicksPerGrid = AOETicksPerGrid;
						  InitTick = MaxTick + ( IntPx % (TickUnit * 5) )/TickUnit;
						  break;
		}

		for( register int i = InitTick; /*i <= MinTick*/; i += TicksPerGrid )
		{
			String TickStr;

			FTickList->ToTNFormat( FTickList->GetPxFromIndex( i, false ), TickStr );
			int TickPos = ( (double)( i * FPixelsPerTick - FViewPortY ) )* FViewPortToControlY;

			if( TickPos > QtyUpperLineY )
				break;
			canvas->Pen->Color = FGridColor;
			canvas->Pen->Style = psDot;
			canvas->MoveTo( 0, TickPos );
			canvas->LineTo( Width - FYAxisOffset, TickPos );
			int HalfTextHeight = canvas->TextHeight( TickStr.c_str() ) / 2;
			TRect TextRect = TRect( Width - FYAxisOffset , TickPos - HalfTextHeight, Width, TickPos + HalfTextHeight );
			DrawTextExW( canvas->Handle, TickStr.c_str(), TickStr.Length(), &TextRect, DT_CENTER|DT_VCENTER, NULL );
		}
	}
	if( FShowQty != 0 )
	{
		String QtyStr;
		TTextFormat Formats;
		double HalfPercent = FChartPercent + (1.0-FChartPercent)/2.0;

		Formats <<tfCenter<<tfVerticalCenter;
		canvas->Font->Size -= 2;
		///< draw max qty
		QtyStr.printf( L"%d", GetMaxQty() );
		int HalfTextHeight = canvas->TextHeight( QtyStr.c_str() ) / 2;
		TRect TextRect = TRect( Width - FYAxisOffset , (Height - FXAxisOffset ) * FChartPercent, Width, (Height - FXAxisOffset ) * FChartPercent + 2*HalfTextHeight );

		canvas->TextRect( TextRect, QtyStr, Formats );
		///< Half Line
		QtyStr.printf( L"%d", GetMaxQty() / 2 );
		TextRect = TRect( Width - FYAxisOffset , (Height - FXAxisOffset ) * HalfPercent, Width, (Height - FXAxisOffset ) * HalfPercent + 2*HalfTextHeight );

		canvas->TextRect( TextRect, QtyStr, Formats );
		canvas->Font->Size += 2;
	}

}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawCandle( TCanvas* canvas, TPoint& MaxPoint, TPoint& MinPoint, TRect& CandleRect, TColor CandleCol )
{
	TColor PenCol;

	if( FCandleLineColor == clNone )
		PenCol = CandleCol;
	else
		PenCol = FCandleLineColor;
	PenCol = (TColor)RGB( AddMax255( GetRValue( PenCol ), 20 ),
						  AddMax255( GetGValue( PenCol ), 20 ),
						  AddMax255( GetBValue( PenCol ), 20 ));
	canvas->Pen->Color   = PenCol;
	canvas->Pen->Style   = psSolid;
	canvas->Brush->Color = CandleCol;
	FLastCandleColor     = CandleCol;
	canvas->MoveTo( MaxPoint.x, MaxPoint.y );
	canvas->LineTo( MinPoint.x, MinPoint.y );
	canvas->Pen->Width = 1;
	if( FGradientCandle )
		GDIUtility::DrawGradientBar( canvas, CandleRect, CandleCol, clBlack, false );
	else
		canvas->Rectangle( CandleRect );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawCandles( TCanvas* canvas, int BeginIndex, int EndIndex, int QtyUpperLineY )
{
	TPoint          MaxPoint, MinPoint;
	GDIClipRegion   Clip( canvas->Handle, 0,0, GetRightBottomX() , GetRightBottomY() );
	int             BarWidth = (int)(((double)FCandleWidth)*FViewPortToControlX + 0.5 ) -1;

	if( EndIndex > FDrawHaveData.Length -1 )
		EndIndex = FDrawHaveData.Length -1;
	for( register int Index = BeginIndex; Index < EndIndex; Index++ ) /// Draw Price Information
	{
		if( FDrawHaveData[Index] )
		{
			int left = ( (double)( FTopLeftArray[Index].x - FViewPortX + 1 ) )* FViewPortToControlX;
			int right = left + BarWidth;
			int top = ( (double)( FTopLeftArray[Index].y - FViewPortY ) )* FViewPortToControlY;
			int bottom = ( (double)( FBottomRightArray[Index].y - FViewPortY ) )* FViewPortToControlY;

			canvas->Pen->Width = (int )( SHADOW_WIDTH * FViewPortToControlX);
			if( canvas->Pen->Width == 0 )
				canvas->Pen->Width = 1;
			if( canvas->Pen->Width % 2 == 0 )
				canvas->Pen->Width -= 1;
			if( top == bottom )
				bottom += 1;
			if( ( right - left - canvas->Pen->Width ) % 2 == 1 && ( right - left ) > 0 )
				right -= 1;
			MaxPoint.x = left + ( right - left )/2;
			MinPoint.x = MaxPoint.x;
			MaxPoint.y = ( (double)( FMaxArray[Index].y - FViewPortY ) )* FViewPortToControlY;
			MinPoint.y = ( (double)( FMinArray[Index].y - FViewPortY ) )* FViewPortToControlY;

			TRect  PaintRect( left, top, right, bottom );

			if( FDrawOpenPx[Index] > FDrawClosePx[Index] ) ///< Bear candle
				DrawCandle( canvas, MaxPoint, MinPoint, PaintRect, FBearColor );
			else if( FDrawClosePx[Index] > FDrawOpenPx[Index] )///< Bull candle
				DrawCandle( canvas, MaxPoint, MinPoint, PaintRect, FBullColor );
			else  ///< OpenPx equals to ClosePx
				DrawCandle( canvas, MaxPoint, MinPoint, PaintRect, FLastCandleColor );
			canvas->Pen->Width = 1;
		}
	}
	DrawBox( canvas );
	DrawFilled( canvas );
	DrawGraphs( canvas );
	if( FEnableTrade == true )
		DrawPositionLine( canvas );
}
//---------------------------------------------------------------------------
int __fastcall TCandleStickChart::IndexToViewPortX( int index )
{
	int VX = index * 10 + 5;
	return ( (double)( VX - FViewPortX ) )* FViewPortToControlX;
}
//---------------------------------------------------------------------------
int __fastcall TCandleStickChart::TickToViewPortY( int tick )
{
	int	VY = tick * FPixelsPerTick;
	return ( (double)( VY - FViewPortY ) )* FViewPortToControlY;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawGraphs( TCanvas* canvas )
{
	if( FGDIPlus == true )
	{
		Gdiplus::Graphics GPPGraphics( canvas->Handle );

		GPPGraphics.SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
		for( register int i =0;i< FGraphs.ItemCount(); i ++ )
		{
		   TKBarGraph* Graph = FGraphs.GetItem( i );
		   if( dynamic_cast<TText*>(Graph) != NULL )
			   Graph->Paint( canvas );
		   else
			   Graph->Paint( &GPPGraphics );
		}
	}
	else
	{
		TColor oldcol = canvas->Pen->Color;
		int    penW = canvas->Pen->Width;

		canvas->Pen->Width = 1;
		for( register int i =0;i< FGraphs.ItemCount(); i ++ )
		{
		   TKBarGraph* Graph = FGraphs.GetItem( i );

		   Graph->Paint( canvas );
		}
		canvas->Pen->Color = oldcol;
		canvas->Pen->Width = penW;
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawFilled( TCanvas* canvas )
{
	int VY, VX, SX, SY, DrawIndex; ///< A KBar
	int NetPos = 0;
	int KBarHW = 5* FViewPortToControlX;
	int KBarQW = KBarHW/2;
	TFilledData* FilledPt;
	String out;
	TColor Col = GDIUtility::ComplementaryColor( Color );
	TColor OldCol = canvas->Font->Color;
	int OldSize = canvas->Font->Size;

	if( KBarHW == 0 )
		KBarHW = 1;
	if( KBarQW == 0 )
		KBarQW = 1;
	canvas->Font->Color = Col;
	canvas->Pen->Color  = Col;
	canvas->Font->Size  = OldSize + 1;
	canvas->Brush->Color  = Col;
	for( int i =0;i< FFilledList.ItemCount(); i ++ )
	{
		FilledPt = static_cast<TFilledData*>( FFilledList.GetItem(i));
		int Hour    = FilledPt->hhmm /100;
		int Minutes = FilledPt->hhmm %100;
		int RefMin  = FExInfo->TimeToIndex( Hour, Minutes, true ); ///< Reference minutes

		if( RefMin == 0 && FExInfo->IsTradingTime( Hour ,Minutes, true ) == false ) ///< Not trading time
			continue;
		DrawIndex = FDayCandle * ( DAYS - 1 ) + ( RefMin / FMinutesPerStick );
		RefMin += (DAYS - 1) * FDayMinutes;
		VY = FTickList->PxToTickIndex(  FilledPt->FilledPx , FDebugMode ) * FPixelsPerTick;
		VX = DrawIndex * 10 + 5;
		SX = ( (double)( VX - FViewPortX /*+ 1*/ ) )* FViewPortToControlX;
		SY = ( (double)( VY - FViewPortY ) )* FViewPortToControlY;
		if( FilledPt->Side == nsOrderMessageDefine::sBuy )
			NetPos += FilledPt->Qty;
		else
			NetPos -= FilledPt->Qty;
		canvas->Brush->Style = bsSolid;
		canvas->Ellipse( SX - KBarQW, SY - KBarQW,SX + KBarQW, SY + KBarQW );
		out.printf( L"%d", NetPos );
		canvas->Brush->Style = bsClear;
		canvas->TextOutW( SX + KBarHW, SY - 10, out );
	}
	canvas->Font->Color = OldCol;
	canvas->Font->Size = OldSize;

}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawBar( TCanvas* canvas, TRect& QtyRect, TColor BarCol )
{
	FLastCandleColor = BarCol;
	if( !FGradientCandle )
	{
		canvas->Brush->Color = BarCol;
		canvas->FillRect( QtyRect );
	}
	else
		GDIUtility::DrawGradientBar( canvas, QtyRect, BarCol, clBlack, false );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawQtyBars( TCanvas* canvas, int BeginIndex, int EndIndex, int QtyUpperLineY )
{
	DrawQtyGrid( canvas,  QtyUpperLineY );
	GDIClipRegion Clip( canvas->Handle, 0, QtyUpperLineY, Width - FYAxisOffset, Height - FXAxisOffset );

	for( register int Index = BeginIndex; Index < EndIndex; Index++ )
	{
		if( FDrawHaveData[Index] )
		{
			int    left = ( (double)( FTopLeftArray[Index].x - FViewPortX + 1) )* FViewPortToControlX;
			int    right = ( (double)( FBottomRightArray[Index].x - FViewPortX - 1 ) )* FViewPortToControlX;
			double Ratio = ((double)(FMaxQty - FDrawQty[Index])/FMaxQty);
			int    QtyTop = QtyUpperLineY + Ratio*( Height - QtyUpperLineY - FXAxisOffset );

			if( right <= left )
				right = left + 1;
			TRect QtyRect = TRect( left, QtyTop, right, Height - FXAxisOffset );

			if( FDrawOpenPx[Index] > FDrawClosePx[Index] )
				DrawBar( canvas, QtyRect, FBearColor );
			else if( FDrawClosePx[Index] > FDrawOpenPx[Index] )
				DrawBar( canvas, QtyRect, FBullColor );
			else  ///< OpenPx equals to ClosePx
				DrawBar( canvas, QtyRect, FLastCandleColor );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawQtyGrid( TCanvas* canvas,  int QtyUpperLineY )
{
	double HalfPercent = FChartPercent + (1.0-FChartPercent)/2.0;
	double Q1Percent = FChartPercent + (1.0-FChartPercent)/4.0;
	double Q3Percent = HalfPercent + (1.0-FChartPercent)/4.0;
	FMaxQty = GetMaxQty();
	canvas->Pen->Color = 0x00DD7C3C;// FGridColor;//clGray;
	canvas->Pen->Width = 3;
	canvas->MoveTo( 0, QtyUpperLineY );
	canvas->LineTo( Width - FYAxisOffset, QtyUpperLineY );
	canvas->Pen->Width = 1;
	canvas->MoveTo( 0, (int)(Height - FXAxisOffset ) * HalfPercent );
	canvas->LineTo( Width - FYAxisOffset, (int)(Height - FXAxisOffset ) * HalfPercent );
	canvas->Pen->Style = psDot;
	canvas->MoveTo( 0, (int)(Height - FXAxisOffset ) * Q1Percent );
	canvas->LineTo( Width - FYAxisOffset, (int)(Height - FXAxisOffset ) * Q1Percent );
	canvas->MoveTo( 0, (int)(Height - FXAxisOffset ) * Q3Percent );
	canvas->LineTo( Width - FYAxisOffset, (int)(Height - FXAxisOffset ) * Q3Percent );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawViewPort( TCanvas* canvas )
{
	int BeginIndex = FViewPortX / 10;///<FCandleWidth
	int EndIndex   = ( FViewPortX + FViewPortWidth ) / 10;
	int QtyUpperLineY = (int)(Height - FXAxisOffset ) * FChartPercent;

	if( FShowQty != 0 )
		DrawQtyBars( canvas, BeginIndex, EndIndex, QtyUpperLineY );
	DrawCandles( canvas, BeginIndex, EndIndex, QtyUpperLineY );
	if( FShowMA1 )
		DrawSMA( canvas, FMACount1, FMAPrice1, FMAColor1 );
	if( FShowMA2 )
		DrawSMA( canvas, FMACount2, FMAPrice2, FMAColor2 );
	if( FShowMA3 )
		DrawSMA( canvas, FMACount3, FMAPrice3, FMAColor3 );
	if( FExablePivot )
		DrawPivots( canvas );
	ShowTickHint( FLastIndex );
	if( FLastIndex <=  EndIndex )
		FCandleHintwin->Paint( 5, 5, 145 , FBuffer->Canvas, FTextColor );
	else
		FCandleHintwin->Empty();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawAxis( TCanvas* canvas, TRect& BufferRect )
{
	canvas->Pen->Color = FGridColor;//clGray;
	canvas->Pen->Style = psSolid;
	canvas->MoveTo( 0, Height - FXAxisOffset );
	canvas->LineTo( Width - FYAxisOffset, Height - FXAxisOffset );
	canvas->LineTo( Width - FYAxisOffset, 0 );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawNotLine(TCanvas *canvas,int x,int y,int x1,int y1)
{
	 canvas->MoveTo( x , y  );
	 canvas->LineTo( x1, y1 );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawRect( int X, int Y , int width)
{
	 TPenMode Mode = Canvas->Pen->Mode;

	 Canvas->Pen->Mode  = pmNot;
	 Canvas->Pen->Width = width;
	 Canvas->Brush->Style = bsClear;
	 Canvas->Rectangle( FMouseDownPos.x , FMouseDownPos.y , X,Y );
	 Canvas->Pen->Mode = Mode;
	 FLastMouseX = X;
	 FLastMouseY = Y;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawCross( int X, int Y )
{
	if( FEditMode == false )
	{
	   AlignmentCheck( X, Y );
	   TPenMode Mode = Canvas->Pen->Mode;

	   Canvas->Pen->Mode  = pmNot;
	   Canvas->Pen->Width  = 1;
	   Canvas->Pen->Style = psSolid;
	   switch( FCursorType )
	   {
		case ctCross:  DrawNotLine( Canvas, X, 0, X, Height - FXAxisOffset );
					   DrawNotLine( Canvas, 0, Y, Width - FYAxisOffset, Y );
					   break;
		case ctVLine:  DrawNotLine( Canvas, X, 0, X, Height - FXAxisOffset );
					   break;
		case ctHLine:  DrawNotLine( Canvas, 0, Y, Width - FYAxisOffset, Y );
					   break;
	   }
	   Canvas->Pen->Mode = Mode;
	   FLastMouseX = X;
	   FLastMouseY = Y;
	}
	else
	{
	   if( FEditType == etVLine || FEditType == etHLine )
	   {
		   AlignmentCheck( X, Y );
		   TPenMode Mode = Canvas->Pen->Mode;

		   Canvas->Pen->Mode  = pmNot;
		   Canvas->Pen->Width  = 1;
		   Canvas->Pen->Style = psSolid;
		   if( FEditType == etVLine )
			   DrawNotLine( Canvas, X, 0, X, Height - FXAxisOffset );
		   else
			   DrawNotLine( Canvas, 0, Y, Width - FYAxisOffset, Y );
		   Canvas->Pen->Mode = Mode;
		   FLastMouseX = X;
		   FLastMouseY = Y;
	   }
	   else if( FEditType == et2PLine &&  FAddingLine == true  )
	   {
		   TPenMode Mode = Canvas->Pen->Mode;

		   Canvas->Pen->Mode  = pmNot;
		   Canvas->Pen->Width  = 1;
		   Canvas->Pen->Style = psSolid;
		   DrawNotLine( Canvas, FBeginX, FBeginY, X, Y );
		   Canvas->Pen->Mode = Mode;
		   FLastMouseX = X;
		   FLastMouseY = Y;
	   }
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::KeyDown(System::Word &Key, Classes::TShiftState Shift)
{
	bool IsLRKey = false;
	if( Key == VK_LEFT )
	{
		if( FMouseIndex == -1 )
			FMouseIndex = FLastIndex;
		FMouseIndex--;
		if( FMouseIndex < 0 )
		{
			FMouseIndex = 0;
			return;
		}
		IsLRKey = true;
	}
	else if( Key == VK_RIGHT )
	{
		if( FMouseIndex == -1 )
			FMouseIndex = FLastIndex;
		FMouseIndex++;
		if( FMouseIndex > FLastIndex )
		{
			FMouseIndex = FLastIndex;
			return;
		}
		IsLRKey = true;
	}
	if( IsLRKey == true )
	{
		int    DataY    = GetDataYPos( FMouseIndex, FMousePos.y );
		int    DataX    = GetDataXPos( FMouseIndex, FMousePos.x );
		TPoint ScreenPx = ClientToScreen( TPoint( DataX, DataY ) );

		Mouse->CursorPos = ScreenPx;
		ShowInfoHint(  DataX, DataY, ScreenPx );
	}
	TCustomControl::KeyDown( Key, Shift );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::AddFilled( TFilledData& Filled )
{
	FFilledList.Add( new TFilledData(Filled) );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::ClearFilled( void )
{
	for( int i = 0; i < FFilledList.ItemCount(); i ++ )
		delete FFilledList.GetItem( i );
	FFilledList.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetEditMode( bool edit )
{
	if( edit == true )
	{
		if( FLastMouseX != -1 )
			DrawCross( FLastMouseX, FLastMouseY );               ///< Draw mouse cross.
		Screen->Cursor = crCross;
	}
	else
	{
		ChangeCursor( );
	}
	FEditMode = edit;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetEditType( EditType et )
{
	FEditType = et;
}
//---------------------------------------------------------------------------
TKBarGraph* __fastcall TCandleStickChart::SelectObjectCursor(  int X, int Y , TCursor NormalCursor )
{
	for( int i =0;i< FGraphs.ItemCount(); i ++ )
	{
	   TKBarGraph* Graph = FGraphs.GetItem( i );
	   if( Graph->HitTest( X,Y, FObjDeltaX, FObjDeltaY ) == true )
	   {
		   Screen->Cursor = crHandPoint;
		   return Graph;
	   }
	}
	Screen->Cursor = NormalCursor; //crArrow;
	return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetMouseDownCursor( void )
{
	if( FEditMode == false )
	{
		if( FScaleType == stAuto )
			Screen->Cursor = crSizeWE;
		else
			Screen->Cursor = crSizeAll;
	}
	else
	{
		switch( FEditType )
		{
			case etHLine:
			case etVLine:
			case etText:
			case et2PRect:
			case et2PLine: Screen->Cursor = crCross;break;
			case etDelete: /*Screen->Cursor = crHandPoint;*/ break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetMouseUpCursor( void )
{
	if( FEditMode == false )
	{
		Cursor = TCursor( 6 );
		Screen->Cursor = crDefault;
	}
	else
	{
		switch( FEditType )
		{
			case etHLine:
			case etVLine:
			case etText:
			case et2PRect:
			case et2PLine: Screen->Cursor = crCross;break;
			case etDelete: Screen->Cursor = crArrow;break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DeleteGraph(  int X, int Y  )
{
	TKBarGraph* DelGraph;
	for( int i =0;i < FGraphs.ItemCount(); i ++ )
	{
	   DelGraph = FGraphs.GetItem( i );
	   if( DelGraph->HitTest( X,Y ) == true )
	   {
		   FGraphs.Delete( i );
		   delete DelGraph;
		   return;
	   }
	}
}
//---------------------------------------------------------------------------
bool __fastcall TCandleStickChart::DeleteGraphs( const TRect& r )
{
	TKBarGraph* DelGraph;
	bool NeedUpdate = false;

	for( int i = FGraphs.ItemCount() -1 ; i >= 0 ; i-- )
	{
	   DelGraph = FGraphs.GetItem( i );
	   if( DelGraph->Intersects ( r ) == true )
	   {
		   FGraphs.Delete( i );
		   delete DelGraph;
		   NeedUpdate = true;
	   }
	}
	return NeedUpdate;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::ClearGraphs( void )
{
	TKBarGraph* DelGraph;
	for( int i =0;i < FGraphs.ItemCount(); i ++ )
	{
	   DelGraph = FGraphs.GetItem( i );
	   delete DelGraph;
	}
	FGraphs.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::AddLine( int x, int y )
{
	if( FAddingLine == false )
	{
		FAddingLine = true;
		FBeginTick = YPosToTick( y );
		FBeginIndex = XPosToIndex( x );
		FBeginX = x;
		FBeginY = y;
	}
	else
	{
		FAddingLine = false;
		int EndTick = YPosToTick( y );
		int EndIndex = XPosToIndex( x );
		FGraphs.Add( new TLine( this, FBeginIndex, FBeginTick, EndIndex, EndTick , FLineColor ) );
	}
}
//---------------------------------------------------------------------------
 void __fastcall TCandleStickChart::AddRect(int x1, int x2, int y1, int y2)
{
	if(x1>x2)
		std::swap(x1,x2);
	if(y1>y2)
		std::swap(y1,y2);

	int y1Tick = YPosToTick(y1);
	int y2Tick = YPosToTick(y2);
	int x1Index= XPosToIndex(x1);
	int x2Index= XPosToIndex(x2);
	FGraphs.Add( new TLine( this, x1Index, y2Tick, x2Index, y2Tick , FLineColor ) );
	FGraphs.Add( new TLine( this, x2Index, y2Tick, x2Index, y1Tick , FLineColor ) );
	FGraphs.Add( new TLine( this, x2Index, y1Tick, x1Index, y1Tick , FLineColor ) );
	FGraphs.Add( new TLine( this, x1Index, y1Tick, x1Index, y2Tick , FLineColor ) );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::AddText( int x, int y )
{
	FMouseDown = false;
	FLastMouseX = -1;
	if( FOnNewText != NULL )
	{
		bool   Accept = true;
		TText* NewText = new TText( this, XPosToIndex(x), YPosToTick(y), clWhite, L"Unify" );

		FGraphs.Add( NewText );
		FOnNewText( this, NewText, Accept );
		if( Accept == false )
		{
		   int Del =  FGraphs.IndexOf( NewText );
		   FGraphs.Delete( Del );
		   delete NewText;
		}

	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::CreateGraph( int x, int y )
{
	switch( FEditType )
	{
		case etHLine: FGraphs.Add( new THLine( this, YPosToTick(y), FLineColor ) );break;
		case etVLine: FGraphs.Add( new TVLine( this, XPosToIndex(x), FLineColor ) ); break;
		case et2PLine:AddLine( x, y ); break;
		case etText:  AddText( x, y ); break;
		case etDelete:DeleteGraph( x, y ); break;
		case etDefault: FMoveGraph = SelectObjectCursor( x, y, crArrow );
						if( FMoveGraph != NULL )
						{
							FLastMouseX = x;
							FLastMouseY = y;
							FMoveGraph->Paint( x,y, FObjDeltaX, FObjDeltaY, Canvas );
						}
						break;
	}
	Paint();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetSkyPx( double SkyPx )
{
	FSkyPx = SkyPx;
	FSkyY  = FTickList->PxToTickIndex( FSkyPx,FDebugMode ) * FPixelsPerTick;

}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetLandPx( double LandPx )
{
	FLandPx = LandPx;
	FLandY  = FTickList->PxToTickIndex( FLandPx,FDebugMode ) * FPixelsPerTick;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetExableSkyLand( bool IsEnable )
{
	FExableSkyLand = IsEnable;
	PaintFPS();
}
//---------------------------------------------------------------------------
bool __fastcall TCandleStickChart::FindHL( int index, int& HIndex, double& HPx, int& LIndex, double& LPx)
{
	int L1Index = FPivots.ItemCount() - 1;
	int L2Index = FPivots.ItemCount() - 2;

	if( (index - FPivots[ L2Index ].Index ) > FExtDepth )
	{
		HIndex = FPivots[ L1Index ].Index;
		HPx    = FPivots[ L1Index ].Price;
		LIndex = FPivots[ L1Index ].Index;
		LPx    = FPivots[ L1Index ].Price;
		if( FPivots[ L1Index ].Type == ptHigh )
			return false;///< Find low
		else
			return true;///< Find High
	}
	else
	{
		if( FPivots[ L1Index ].Type == ptHigh )
		{
			HIndex = FPivots[ L1Index ].Index;
			HPx    = FPivots[ L1Index ].Price;
			LIndex = FPivots[ L2Index ].Index;
			LPx    = FPivots[ L2Index ].Price;
			return false;///< Find low
		}
		else
		{
			HIndex = FPivots[ L2Index ].Index;
			HPx    = FPivots[ L2Index ].Price;
			LIndex = FPivots[ L1Index ].Index;
			LPx    = FPivots[ L1Index ].Price;
			return true;///< Find High
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::AddRootPivot( void )
{
	double CurrPx  = FDrawClosePx[0];
	int    PxIndex = FTickList->PxToTickIndex( CurrPx, FDebugMode );

	FPivots.Add( TPivot( 0, CurrPx, PxIndex, ptRoot )  );
	FCountedBar = 1;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::AddFirstPivot( int i, double CurrPx )
{
	double RefPx   = FPivots[ 0 ].Price;
	double UperPx  = RefPx* (1.0 + FExtDeviation);
	double LowerPx = RefPx* (1.0 - FExtDeviation);

	if( CurrPx > UperPx )
	{
		int PxIndex = FTickList->PxToTickIndex( CurrPx, FDebugMode );

		FPivots.Add( TPivot( i, CurrPx, PxIndex, ptHigh ) );
		FPivots[0].Type = ptLow;
		FFirstPivotExists = true;
	}
	else if( CurrPx < LowerPx )
	{
		int PxIndex = FTickList->PxToTickIndex( CurrPx,FDebugMode );

		FPivots.Add( TPivot( i, CurrPx, PxIndex, ptLow ) );
		FPivots[0].Type = ptHigh;
		FFirstPivotExists = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::Pivot( int LastI )
{
	if( FExablePivot == true )
	{
		if( FCountedBar == 0 && LastI > 0 )
			AddRootPivot( );

		if( LastI > FExtDepth )
		{
			for( int i = FCountedBar; i < LastI; i++ )
			{
				if( FDrawHaveData[i] == true )
				{
					double CurrPx  = FDrawClosePx[ i ];
					double HPx,LPx;
					int    HIndex,LIndex;

					if( FFirstPivotExists == true )
					{
						 bool   FindH = FindHL( LastI, HIndex, HPx, LIndex, LPx );
						 double UperPx  = HPx* (1.0 + FExtDeviation);
						 double LowerPx = LPx* (1.0 - FExtDeviation);

						 if( FindH == true  ) ///< Want to find a high pivot.
						 {
							 if( CurrPx < LPx ) ///< Want to find high...but it lower than the last low pivot
							 {
								 int L1Index = FPivots.ItemCount() - 1;

								 FPivots[L1Index].Index = i;
								 FPivots[L1Index].Price = CurrPx;
								 FPivots[L1Index].PxIndex =  FTickList->PxToTickIndex( CurrPx,FDebugMode );
							 }
							 else if( CurrPx > UperPx ) ///< Find a high point
							 {
								 int PxIndex = FTickList->PxToTickIndex( CurrPx, FDebugMode );
								 FPivots.Add(  TPivot( i, CurrPx, PxIndex, ptHigh ) );
							 }
						 }
						 else ///< Want to find a low pivot.
						 {
							 if( CurrPx > HPx ) ///< Want to find low point...but it higher than the last high pivot
							 {
								 int L1Index = FPivots.ItemCount() - 1;

								 FPivots[L1Index].Index = i;
								 FPivots[L1Index].Price = CurrPx;
								 FPivots[L1Index].PxIndex =  FTickList->PxToTickIndex( CurrPx,FDebugMode );
							 }
							 else if( CurrPx < LowerPx ) ///< Find a low point
							 {
								 int PxIndex = FTickList->PxToTickIndex( CurrPx, FDebugMode );
								 FPivots.Add( TPivot( i, CurrPx, PxIndex, ptLow ) );
							 }
						 }
					}
					else
						AddFirstPivot( i, CurrPx );
				}
				FCountedBar++;
			}
		}
	}
}
//---------------------------------------------------------------------------
int __fastcall TCandleStickChart::GetRightBottomX( void )
{
	return Width - FYAxisOffset;
}
//---------------------------------------------------------------------------
int __fastcall TCandleStickChart::GetRightBottomY( void )
{
	if( FShowQty != 0 )
		return (Height - FXAxisOffset)*FChartPercent;
	return Height - FXAxisOffset;
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::DrawPivots( TCanvas* canvas )
{
	int               StartIndex   = FPivots[0].Index;
	int               StartPxIndex = FPivots[0].PxIndex;
	int               ToX,ToY;
	int               FromX  = (int)((double)( StartIndex * FCandleWidth  - FViewPortX )* FViewPortToControlX);
	int               FromY  = (int)((double)( StartPxIndex * FPixelsPerTick - FViewPortY)* FViewPortToControlY);
	GDIClipRegion     Clip( canvas->Handle, 0,0, GetRightBottomX() , GetRightBottomY() );
	Gdiplus::Graphics GPPGraphics( canvas->Handle );
	Gdiplus::Pen      DrawPen( Gdiplus::Color(255, GetRValue(clSkyBlue), GetGValue(clSkyBlue), GetBValue(clSkyBlue)), 2 );

	GPPGraphics.SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
	for( register int i = 1; i < FPivots.ItemCount(); i ++ )
	{
		int Index   = FPivots[i].Index;
		int PxIndex = FPivots[i].PxIndex;
		int ToX  = (int)((double)( Index * FCandleWidth  - FViewPortX)* FViewPortToControlX);
		int ToY  = (int)((double)( PxIndex * FPixelsPerTick - FViewPortY)* FViewPortToControlY);

		GPPGraphics.DrawLine( &DrawPen,(int)FromX,(int)FromY,(int)ToX,(int)ToY );
		FromX = ToX;
		FromY = ToY;
	}
	ToX  = (int)((double)( FLastIndex * FCandleWidth  - FViewPortX )* FViewPortToControlX);
	ToY  = (int)((double)( FLastTick - FViewPortY)* FViewPortToControlY);
	GPPGraphics.DrawLine( &DrawPen,(int)FromX,(int)FromY,(int)ToX,(int)ToY );
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetPivotDeviation( double Deviation )
{
	FExtDeviation = Deviation;
	FCountedBar = 0;
	FFirstPivotExists = false;
	FPivots.Clear();
	Pivot( FLastIndex  );
	PaintFPS();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetExablePivot( bool NeedDraw )
{
	FExablePivot = NeedDraw;
	if( FExablePivot == true )
	{
		FCountedBar = 0;
		FFirstPivotExists = false;
		FPivots.Clear();
		Pivot( FLastIndex  );
	}
	PaintFPS();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetUseHLSection( bool Enable )
{
	int YIndex;
	FUseHLSection = Enable;

	if( FUseHLSection == true )
	{
		YIndex = FTickList->PxToTickIndex( FDayHigh,FDebugMode );
		FSectionHLine.MoveTo( YIndex );
		YIndex = FTickList->PxToTickIndex( FDayLow,FDebugMode );
		FSectionLLine.MoveTo( YIndex );
	}
	PaintFPS();
}
//---------------------------------------------------------------------------
double __fastcall TCandleStickChart::Open( int i )
{
	int Index = FBaseIndex - i;

	if( Index < 0 )
		return 0.0;
	return FDrawOpenPx[ Index ];
}
//---------------------------------------------------------------------------
double __fastcall TCandleStickChart::Close( int i )
{
	int Index = FBaseIndex - i;

	if( Index < 0 )
		return 0.0;
	return FDrawClosePx[ Index ];
}
//---------------------------------------------------------------------------
double __fastcall TCandleStickChart::High( int i )
{
	int Index = FBaseIndex - i;

	if( Index < 0 )
		return 0.0;
	return FDrawMaxPx[ Index ];
}
//---------------------------------------------------------------------------
double __fastcall TCandleStickChart::Low( int i )
{
	int Index = FBaseIndex - i;

	if( Index < 0 )
		return 0.0;
	return FDrawMinPx[ Index ];
}
//---------------------------------------------------------------------------
double __fastcall TCandleStickChart::OpenD( void )
{
	int OpenIndex = (DAYS - 1) * FDayMinutes;
	return FOpenPx[ OpenIndex ];
}
//---------------------------------------------------------------------------
double __fastcall TCandleStickChart::CloseD( void )
{
	return FClosePx[ FLastIndex ];
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SaveToClipboard( void )
{
	Graphics::TBitmap* ScreenBmpPtr = new Graphics::TBitmap();
	TClipboard* cb = Clipboard();
	unsigned int DataHandle;
	unsigned short MyFormat;
	HPALETTE APalette;
	Types::TRect SourceRect( 0,0, Width,Height);
	Types::TRect DestRect( 0,0, Width,Height);

	ScreenBmpPtr->Width  = Width;
	ScreenBmpPtr->Height = Height;
	ScreenBmpPtr->Canvas->CopyRect( DestRect, Canvas, SourceRect);
	ScreenBmpPtr->SaveToClipboardFormat( MyFormat, DataHandle, APalette );
	cb->SetAsHandle( MyFormat, DataHandle);
	delete cb;
	delete ScreenBmpPtr;
}
//---------------------------------------------------------------------------
void TCandleStickChart::OnProfit(double AveragePrice, double ProfitAmount)
{
	FPosition.SetAvgPrice( AveragePrice );
}
//---------------------------------------------------------------------------
int __fastcall TCandleStickChart::PriceToTick( double Price )
{
	return FTickList->PxToTickIndex( Price, false );
}
//---------------------------------------------------------------------------
void TCandleStickChart::OnFill(int Position, double BuyArvPx, double SellArvPx, int BuyQty, int SellQty,
							int BuyOpenInterestQty, int SellOpenInterestQty,
							DynamicArray<void*>& PosStatisticsArray, int NetPosition, double FloatingProfit )
{
	FPosition.SetPosition( NetPosition );
	if( FShowAvgPx == true )
		Paint();
}
//---------------------------------------------------------------------------
void TCandleStickChart::OrderQtyChanged( MarketEnum Market, const String& Symbol, SideEnum Side, double Px, int Qty)
{
	TWorkingLine* WorkingOrder;
	bool IsBuy;
	int key;

	if( Side == sSell )
	{
		key = 20000000 + PriceToTick( Px );
		IsBuy = false;
	}
	else
	{
		key = 10000000 + PriceToTick( Px );
		IsBuy = true;
	}
	if( FWorkingOrders.GetObjectByKey( key, WorkingOrder ) == false )
	{
		WorkingOrder = new TWorkingLine( this, Px, Qty, IsBuy, false );
		FWorkingOrders.Add( key, WorkingOrder );
	}
	else
	{
	   if( Qty == 0 )
	   {
		   FWorkingOrders.DeleteByKey( key );
		   delete WorkingOrder;
	   }
	   else
		   WorkingOrder->SetQty( Qty );
	}
	Paint();
}
//---------------------------------------------------------------------------
void TCandleStickChart::StopOrderQtyChanged( nsOrderMessageDefine::MarketEnum Market,
									  const String& Symbol, double StopPx,
									  nsOrderMessageDefine::SideEnum Side,
									  double Px, int Qty,
									  nsOrderMessageDefine::OrderTypeEnum OrderType )
{
	TWorkingLine* WorkingOrder;
	bool IsBuy;
	int key;

	if( Side == sSell )
	{
		key = 40000000 + PriceToTick( Px );
		IsBuy = false;
	}
	else
	{
		key = 30000000 + PriceToTick( Px );
		IsBuy = true;
	}
	if( FWorkingOrders.GetObjectByKey( key, WorkingOrder ) == false )
	{
		WorkingOrder = new TWorkingLine( this, StopPx, Qty, IsBuy, true );
		FWorkingOrders.Add( key, WorkingOrder );
	}
	else
	{
	   if( Qty == 0 )
	   {
		   FWorkingOrders.DeleteByKey( key );
		   delete WorkingOrder;
	   }
	   else
		   WorkingOrder->SetQty( Qty );
	}
	Paint();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SetEnableTrade( bool enable )
{
	FEnableTrade = enable;
	PaintFPS();
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::BuyStopClicked(TObject* Sender)
{
	if( FOnNewStopOrder != NULL )
	{
		int StopPxTick = FTickList->PxToTickIndex( FStopPx,false);
		int ExecTick;

		ExecTick = StopPxTick - FBuyStopTick;
		if( FBuyStopOrderOrdType == nsOrderMessageDefine::otLimit )
		{
			if( ExecTick >= 0 || ExecTick < FTickList->ItemCount() )
			{
				double ExecPx = FTickList->GetPxFromIndex( ExecTick, false );
				FOnNewStopOrder(this, nsOrderMessageDefine::sBuy, FStopPx, FBuyStopOrderOrdType, ExecPx );
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::SellStopClicked(TObject* Sender)
{
	if( FOnNewStopOrder != NULL )
	{
		int StopPxTick = FTickList->PxToTickIndex( FStopPx,false);
		int ExecTick;

		ExecTick = StopPxTick - FSellStopTick;
		if( FSellStopOrderOrdType == nsOrderMessageDefine::otLimit )
		{
			if( ExecTick >= 0 || ExecTick < FTickList->ItemCount() )
			{
				double ExecPx = FTickList->GetPxFromIndex( ExecTick, false );
				FOnNewStopOrder(this, nsOrderMessageDefine::sSell, FStopPx, FSellStopOrderOrdType, ExecPx );
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCandleStickChart::CloseAllClicked(TObject* Sender)
{
	if( FOnClosePosition != NULL )
		FOnClosePosition( this, FPosition.GetSide(), FPosition.GetQty() );
}
//------------------------------------------------------------------------------
double __fastcall TCandleStickChart::GetTickPrice( int Step )
{
	int ClosePxIndex  = FLastPxTick + Step;

	return FTickList->GetPxFromIndex( ClosePxIndex, false );
}
//---------------------------------------------------------------------------
void  __fastcall TCandleStickChart::GetPriceTickString( int PxIndex,String& PxStr )
{
	double Px = FTickList->GetPxFromIndex( PxIndex, false );
	PxStr.printf( L"%.*f", FDigit, Px );
}
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TCandleStickChart *)
{
	new TCandleStickChart(NULL);
}
//---------------------------------------------------------------------------
