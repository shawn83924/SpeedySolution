//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <winuser.h>
#include <imm.h>
#include "OrderBookList.h"
#include "TradingPosition.h"
#include "MDComponentStrings.hpp"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
const int COL_COUNT = 17;
const int ShortWidth = 30;
const int DEPTH_COUNT = 10;
const PAINT_TIMER_ID = 100079;
//---------------------------------------------------------------------------
UFC::List<TOrderBookList*>  TOrderBookList::FSyncDepth;
//---------------------------------------------------------------------------
//{ "買成", "觸價","OCO刪", "OCO", "刪", "買進", "委買", "買", "Price", "賣", "委賣", "賣出", "刪", "OCO", "OCO刪", "觸價","賣成" };
String TOrderBookList::HeaderString[COL_COUNT];
//---------------------------------------------------------------------------
__fastcall TOrderBookList::TOrderBookList(TComponent* Owner)
:TCustomGrid(Owner)
,FIsCompact( false )
,FBullPx( 7293 )
,FBearPx( 6339 )
,FDigit(0)
,FFillPx( FBullPx )
,FLastFillPx( FBullPx )
,FFillRowIndex( 2 )
,FAvgFillPxIndex( -1 )
,FMarkPxIndex( -1 )
,FMarkPx2Index( -1)
,FBidDepth5BKColor( clSkyBlue )
,FBidDepth5Color( clBlack )
,FAskDepth5BKColor( clSkyBlue )
,FAskDepth5Color( clBlack )
,FBuyBKColor( clNavy )
,FSellBKColor( clNavy )
,FBuyDelBKColor( clNavy )
,FSellDelBKColor( clNavy )
,FFixedRowColor( clBlack )
,FFixedRowBKColor( clWhite )
,FFixedRowBKEndColor( clWhite )
,FFixedRowBidColor( clWhite )
,FFixedRowBidEndColor( clWhite )
,FFixedRowAskColor( clWhite )
,FFixedRowAskEndColor( clWhite )
,FBetterBuyBKColor( clRed )
,FBetterSellBKColor( clLime )
,FTickUpperColor( clRed )
,FTickLowerColor( clGreen )
,FTickBKColor( clSilver )
,FFillColor( clAqua )
,FFillBKColor( clYellow )
,FGridColor( clBlack )
,FClickQty(1)
,FStore( NULL )
,FExchange(L"TAIFEX")
,FSymbol(L"TXFH9")
,IsLoaded( false )
,FCenterFillPrice( true )
,FFirstMatchPx( true )
,FInvertBS( false )
,FUpdateEveryFill( false )
,FTotalBuyQty( 0 )
,FTotalSellQty( 0 )
,FCancelBeforeNew( false )
,FStep( 0 )
,FStepCount( 0 )
,KeyNewOrder(0)
,FKeyBuy(0)
,FKeySell(0)
,FKeyBetterIncrese(0)
,FKeyBetterDecrese(0)
,FKeyBuyMarket(0)
,FKeySellMarket(0)
,FKeyCancelAll(0)
,FKeyCancelBuy(0)
,FKeyCancelSell(0)
,FBetterEnable( true )
,FKeyCenterFillPx( 0 )
,FMouseButtonCenterFillPx( mbRight )
,FStepEnable( false )
,FShowFilled( true )
,FBuyFillColor( clBlack )
,FBuyFillBKColor( (TColor)0x00763A0A )
,FSellFillColor( clBlack )
,FSellFillBKColor( (TColor)0x00763A0A )
,FBuyConditionColColor( clWhite )
,FBuyConditionColBKColor( (TColor)0x00763A0A )
,FSellConditionColColor( clWhite )
,FSellConditionColBKColor( (TColor)0x00763A0A )
,FBuyOCOColColor( clBlack )
,FBuyOCOColBKColor( clWhite )
,FBuyOCODelColColor( clBlack )
,FBuyOCODelColBKColor( clWhite )
,FSellOCOColColor( clBlack )
,FSellOCOColBKColor( clWhite )
,FDayHFrameColor( clRed )
,FDayLFrameColor( clGreen )
,FAvgPxColor( clWhite )
,FBuyAvgFillPx(0.0)
,FSellAvgFillPx(0.0)
,FDerivedBuyIndex(-1)
,FDerivedSellIndex(-1)
,FDerivedBuyQty(-1)
,FDerivedSellQty(-1)
,FDerivedBuyPx( 0 )
,FDerivedSellPx( 0 )
,FShowFillQty(true)
,FShowDerivedPx(true)
,FFillQty(0)
,FSettingMode(false)
,FBestFitWidth(0)
,FEnableHotKey( false )
,FMarketTotalBuyQty(0)
,FMarketTotalSellQty(0)
,FMaxFPS( 8 )
,FNeedCenterFill( true )
,FHH(0)
,FMM(0)
,FSS(0)
,FEnableCoditionlOrder( true )
,FShowOCO( false )
,FBuyStopOrderOrdType( nsOrderMessageDefine::otLimit )
,FSellStopOrderOrdType( nsOrderMessageDefine::otLimit )
,FBuyStopTick(0)
,FSellStopTick(0)
,FEnableHotTracks( true )
,FMaxFillQty( 99 )
,FClxAllDelayMS( 0 )
,FHotTracksColor( clBlack )
,FHotTracksBKColor( clSilver )
,FTotalStopBuyQty( 0 )
,FTotalStopSellQty( 0 )
,FTotalOCOBuyQty( 0 )
,FTotalOCOSellQty( 0 )
,FIsPairingOCO( false )
,FDeleteOCOByRightClick( false )
,FCurrentPairOCO( NULL )
,FOnStepChange( NULL )
,FOnReplacePx( NULL )
{
	InitString( );
	TGridOptions OptionSet;

	OptionSet.Clear();
	Options = OptionSet;
	FMouseCoord.X = -1;
	FMouseCoord.Y = -1;
	FMouseDownCoord.X = -1;
	FMouseDownCoord.Y = -1;
	FFont = new TFont();
	FFont->Size = 16;
	ScrollBars = ssNone;
	EditorMode = false;
	DoubleBuffered = false;
	DefaultDrawing = false;
	ColCount = COL_COUNT;
	FixedCols = 0;
	FixedRows = 2;
	OnMouseLeave = MouseLeave;
	OnMouseEnter = MouseEnter;
	OnMouseWheelDown = MyMouseWheelDown;
	OnMouseWheelUp = MyMouseWheelUp;
	FPlusDarkBmp = new Graphics::TBitmap();
	FPlusLightBmp = new Graphics::TBitmap();
	FMinusDarkBmp = new Graphics::TBitmap();
	FMinusLightBmp = new Graphics::TBitmap();
	FPlusBmp = new Graphics::TBitmap();
	FMinusBmp = new Graphics::TBitmap();
	FAlarmBmp = new Graphics::TBitmap();
	FAlarmedBmp = new Graphics::TBitmap();
	FBufferBmp = new Graphics::TBitmap();

	FPlusDarkBmp->Width = 20;
	FPlusDarkBmp->Height = 20;
	FPlusLightBmp->Width = 20;
	FPlusLightBmp->Height = 20;
	FMinusDarkBmp->Width = 20;
	FMinusDarkBmp->Height = 20;
	FMinusLightBmp->Width = 20;
	FMinusLightBmp->Height = 20;

	FDrawTextParam.cbSize = 20;
	FDrawTextParam.iLeftMargin = 5;
	FDrawTextParam.iRightMargin = 5;

	FUpdateMinMS = 1000/FMaxFPS;
	FLastUpdateFill = UFC::GetTickCountMS();
	FLastUpdateDepth = UFC::GetTickCountMS();

	FTimer = new TTimer( this );
	FTimer->Interval = FUpdateMinMS;
	FTimer->OnTimer = OnUpdateTimer;
	if( ComponentState.Contains( csDesigning ) )
		InitialGrid( FBullPx, FBearPx, FFillPx, FDigit );
}
//---------------------------------------------------------------------------
__fastcall TOrderBookList::~TOrderBookList()
{
	SetSync( false );
	FTimer->Enabled = false;
	delete FTimer;
	if( FStore != NULL && ComponentState.Contains( csDesigning ) == false)
	{
		UFC::AnsiString Ex( FExchange.c_str() );
		UFC::AnsiString Sym( FSymbol.c_str() );

		FStore->Unsubscribe( Ex, Sym, this );
	}
	Clear();
	delete FAlarmBmp;
	delete FAlarmedBmp;
	delete FPlusBmp;
	delete FMinusBmp;
	delete FPlusDarkBmp;
	delete FPlusLightBmp;
	delete FMinusDarkBmp;
	delete FMinusLightBmp;
	delete FBufferBmp;
	delete FFont;
	FBuyDepth5Qty.Length = 0;
	FSellDepth5Qty.Length = 0;
	FUserBuyQty.Length = 0;
	FUserSellQty.Length = 0;
	FBuyFillQty.Length = 0;
	FSellFillQty.Length = 0;
	FBuyConditionQty.Length = 0;
	FSellConditionQty.Length = 0;
	FBuyOCOQty.Length  = 0;
	FSellOCOQty.Length = 0;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::InitString( void )
{
	HeaderString[ 0] = Mdcomponentstrings_MD_ORDERBOOK_FILL_BUY;///'買成'
	HeaderString[ 1] = Mdcomponentstrings_MD_ORDERBOOK_STOPPX;///'觸價'
	HeaderString[ 2] = Mdcomponentstrings_MD_ORDERBOOK_CANCELOCO;///'刪單(OCO)'
	HeaderString[ 3] = Mdcomponentstrings_MD_ORDERBOOK_OCO;///'OCO'
	HeaderString[ 4] = Mdcomponentstrings_MD_ORDERBOOK_BUYCANCEL;///'刪(買進)'
	HeaderString[ 5] = Mdcomponentstrings_MD_ORDERBOOK_BUY_IN;////'買進'
	HeaderString[ 6] = Mdcomponentstrings_MD_ORDERBOOK_BUY_ORDER;///'委買'
	HeaderString[ 7] = Mdcomponentstrings_MD_ORDERBOOK_BUY;///'買'
	HeaderString[ 8] = Mdcomponentstrings_MD_ORDERLIST_PRICE;///'價格'
	HeaderString[ 9] = Mdcomponentstrings_MD_ORDERBOOK_SELL;///'賣'
	HeaderString[10] = Mdcomponentstrings_MD_ORDERBOOK_SELL_ORDER;///'委賣'
	HeaderString[11] = Mdcomponentstrings_MD_ORDERBOOK_SELL_OFF;///'賣出'
	HeaderString[12] = Mdcomponentstrings_MD_ORDERBOOK_SELLCANCEL;///'刪(賣出)'
	HeaderString[13] = Mdcomponentstrings_MD_ORDERBOOK_OCO;///'OCO'
	HeaderString[14] = Mdcomponentstrings_MD_ORDERBOOK_CANCELOCO;///'刪單(OCO)'
	HeaderString[15] = Mdcomponentstrings_MD_ORDERBOOK_STOPPX;///'觸價'
	HeaderString[16] = Mdcomponentstrings_MD_ORDERBOOK_FILL_SELL;///'賣成'
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetAlarmBmp( Graphics::TBitmap* Bitmap )
{
	FAlarmBmp->Assign( Bitmap );
	FAlarmBmp->Dormant();
	FAlarmBmp->FreeImage();
	FAlarmBmp->TransparentMode = tmAuto;
	FAlarmBmp->Transparent = true;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetAlarmedBmp( Graphics::TBitmap* Bitmap )
{
	FAlarmedBmp->Assign( Bitmap );
	FAlarmedBmp->Dormant();
	FAlarmedBmp->FreeImage();
	FAlarmedBmp->TransparentMode = tmAuto;
	FAlarmedBmp->Transparent = true;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetPlusBmp( Graphics::TBitmap* Bitmap )
{
	FPlusBmp->Assign( Bitmap );
	FPlusBmp->Dormant();
	FPlusDarkBmp->Canvas->Draw( 0, 0, FPlusBmp );
	FPlusLightBmp->Canvas->Draw( 0, -40, FPlusBmp );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetMinusBmp( Graphics::TBitmap* Bitmap )
{
	FMinusBmp->Assign( Bitmap );
	FMinusBmp->Dormant();
	FMinusDarkBmp->Canvas->Draw( 0, 0, FMinusBmp );
	FMinusLightBmp->Canvas->Draw( 0, -40, FMinusBmp );
}
//---------------------------------------------------------------------------
Classes::TStringList* __fastcall TOrderBookList::GetSymbols( void )
{
	Classes::TStringList* List = new Classes::TStringList();
	List->Add( FSymbol );
	return List;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::Loaded(void)
{
	FAlarmBmp->TransparentMode = tmAuto;
	FAlarmBmp->Transparent = true;
	FAlarmedBmp->TransparentMode = tmAuto;
	FAlarmedBmp->Transparent = true;
	FPlusDarkBmp->Canvas->Draw( 0, 0, FPlusBmp );
	FPlusLightBmp->Canvas->Draw( 0, -40, FPlusBmp );
	FMinusDarkBmp->Canvas->Draw( 0, 0, FMinusBmp );
	FMinusLightBmp->Canvas->Draw( 0, -40, FMinusBmp );
	SetFont( FFont );
	CalSize();

	if( ComponentState.Contains( csDesigning ) )
		Invalidate();
	IsLoaded = true;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::CalFilledColSize( TCanvas* canvas )
{
	String PriceText;
	int PriceWidth,PriceColWidth,RateColWidth,TimeColWidth;

	if( FTickList == NULL || ComponentState.Contains( csDesigning ) )
		PriceText.printf( L"%.*f", FDigit, FBullPx );
	else
		FTickList->ToTNFormat( GetPxFromIndex( FBullPx ), PriceText );
	PriceText += L"()" ;
	if( FShowFillQty == true  )
		PriceText += IntToStr( FMaxFillQty ); ///< Check "Fill Qty hint"
	PriceColWidth = canvas->TextWidth( PriceText  ) + 4;
	PriceWidth    = canvas->TextWidth( L"_Price_" ) + 4;
	RateColWidth  = canvas->TextWidth( L"_50:50_" ) + 4;
	TimeColWidth  = canvas->TextWidth( L"_23:59:59" ) + 4;
	if( PriceColWidth < PriceWidth )
		PriceColWidth = PriceWidth;
	if( PriceColWidth < RateColWidth )
		PriceColWidth = RateColWidth;
	if( PriceColWidth < TimeColWidth )
		PriceColWidth = TimeColWidth;
	ColWidths[ PRICE_COL ] = PriceColWidth;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::CalSize( void )
{
	UnicodeString HeaderText,FillPxText;
	int FillWidth, LongColWidth, ShortColWidth, _9999Width, QtyWidth, OrderWidth;

	Graphics::TBitmap* BufferBmp = new Graphics::TBitmap();
	BufferBmp->Canvas->Font->Assign( FFont );
	LeftCol = 0;//BUY_FILL_COL;
	if( SettingMode )
	{
		FIsCompact = true;
		FShowFilled = false;
		FShowFillQty = false;
		SetSettingModeCol();
		delete BufferBmp;
		return;
	}

	HeaderText.printf( L"%s(999)", HeaderString[BUY_FILL_COL].c_str() );
	//FillPxText.printf( L" %.*f ", FDigit+1, FBullPx );
	//_9999Width    = BufferBmp->Canvas->TextWidth( FillPxText ) + 4;    ///< width: 123.456(BullPx)
	ShortColWidth = BufferBmp->Canvas->TextWidth( HeaderString[BUY_CONDITION_COL] ) + 4; ///< width: 觸價
	LongColWidth  = BufferBmp->Canvas->TextWidth( HeaderText ) + 4;      ///< width: 買成(999)
	QtyWidth      = BufferBmp->Canvas->TextWidth( L"9999(99)" ) + 4;     ///< width: 9999(99)
	OrderWidth    = BufferBmp->Canvas->TextWidth( L"9999999" ) + 4;         ///< width: 9999999
	DefaultRowHeight = BufferBmp->Canvas->TextHeight( HeaderText ) + 2;  ///< Height of
	if( LongColWidth >= OrderWidth ) ///< FillQtyWidth
		FillWidth = LongColWidth;
	else
		FillWidth = OrderWidth;

	SetCompactCol(ShortColWidth);
	SetFillCol(FillWidth);
	SetConditionCol(OrderWidth);
	SetSmartOrderCol(OrderWidth);
	CalFilledColSize( BufferBmp->Canvas );    ///< Filled price col
	ColWidths[ BUY_ORDER_COL ]  = OrderWidth;
	ColWidths[ SELL_ORDER_COL ] = OrderWidth;
	ColWidths[ BUY_DEL_BTN_COL ] = OrderWidth;
	ColWidths[ SELL_DEL_BTN_COL ] = OrderWidth;
	ColWidths[ BUY_MK_COL ]     = QtyWidth;
	ColWidths[ SELL_MK_COL ]    = QtyWidth;
	FBestFitWidth = COL_COUNT;// + 2;
	for( register int i = 0; i < COL_COUNT; i++ )
		 FBestFitWidth += ColWidths[ i ];

	delete BufferBmp;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetSettingModeCol( void )
{
	ColWidths[ BUY_DEL_BTN_COL ] = 0;
	ColWidths[ BUY_ORDER_BTN_COL ] = 0;
	ColWidths[ SELL_ORDER_BTN_COL ] = 0;
	ColWidths[ SELL_DEL_BTN_COL ] = 0;
	ColWidths[ BUY_CONDITION_COL ] = 0;
	ColWidths[ SELL_CONDITION_COL ] = 0;

	ColWidths[ BUY_FILL_COL ] = 48;
	ColWidths[ SELL_FILL_COL ] = 48;
	ColWidths[ BUY_ORDER_COL ] = 35;
	ColWidths[ BUY_MK_COL ] = 39;
	ColWidths[ SELL_MK_COL ] = 39;
	ColWidths[ SELL_ORDER_COL ] = 35;
	ColWidths[ PRICE_COL ] = 49;

	FBestFitWidth = 5;
	for( register int i = 0; i < COL_COUNT; i++ )
		FBestFitWidth += ColWidths[i];
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetCompactCol( int ColWidth )
{
	int width = FIsCompact ? 0 : ColWidth;

	ColWidths[BUY_DEL_BTN_COL]    = width;
	ColWidths[BUY_ORDER_BTN_COL]  = width;
	ColWidths[SELL_ORDER_BTN_COL] = width;
    ColWidths[SELL_DEL_BTN_COL]   = width;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetFillCol( int FillWidth )
{
	int width = FShowFilled ? FillWidth : 0;
	ColWidths[BUY_FILL_COL]	 = width;
	ColWidths[SELL_FILL_COL] = width;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetConditionCol( int OrderWidth )
{
	int width = FEnableCoditionlOrder ? OrderWidth : 0;
	ColWidths[BUY_CONDITION_COL] = width;
	ColWidths[SELL_CONDITION_COL] = width;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetSmartOrderCol( int OrderWidth )
{
	int OCOWidth = FShowOCO ? OrderWidth : 0;
	ColWidths[BUY_OCO_COL] = OCOWidth;
	ColWidths[SELL_OCO_COL] = OCOWidth;
	ColWidths[BUY_OCODEL_COL] = OCOWidth;
	ColWidths[SELL_OCODEL_COL] = OCOWidth;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetSettingMode( bool Mode )
{
	FSettingMode = Mode;

	if( ComponentState.Contains( csDesigning ) )
	{
		InitialGrid( FBullPx, FBearPx, FFillPx, FDigit );
		CalSize();
		Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetDigit( int Digit )
{
	FDigit = Digit;
	if( ComponentState.Contains( csDesigning ) )
	{
		InitialGrid( FBullPx, FBearPx, FFillPx, FDigit );
		CalSize();
		Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::EnableConditionOrder( bool Enable )
{
	FEnableCoditionlOrder = Enable;
	//if( ComponentState.Contains( csDesigning ) )
	{
		CalSize();
		Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::EnableOCO( bool Enable )
{
	EnableSmartOrder(Enable);
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::EnableSmartOrder( bool OCO )
{
	FShowOCO = OCO;
    CalSize();
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::InitialGrid( double BullPrice, double BearPrice, double FillPrice, int Digit )
{
	FBullPx = BullPrice;
	FBearPx = BearPrice;
	FDigit  = Digit;

	Clear();
	if( FSettingMode == true )
	{
		FBullPx = 8240;
		FBearPx = 8230;
		FillPrice = 8235;
		FBetterEnable = true;
		FBetterStep = 2;
	}
	if( ComponentState.Contains( csDesigning ) || FSettingMode )
	{
		RowCount = FBullPx - FBearPx + 3;
	}
	else
	{
		if ( FStore != 0 )
		{
			FTickList = (TTickList*)FStore->GetTickList( FExchange, FSymbol );
			if( FTickList != NULL )
				RowCount = FTickList->ItemCount() + 2;
		}
	}

	FBuyDepth5Qty.Length = RowCount;
	FSellDepth5Qty.Length = RowCount;
	FUserBuyQty.Length = RowCount;
	FUserSellQty.Length = RowCount;
	FBuyFillQty.Length = RowCount;
	FSellFillQty.Length = RowCount;
	FBuyConditionQty.Length = RowCount;
	FSellConditionQty.Length = RowCount;
	FBuyOCOQty.Length = RowCount;
	FSellOCOQty.Length = RowCount;

	for( register int i = 0; i < DEPTH_COUNT; i++ )
	{
		FBuyPxIndex[i] = -1;
		FSellPxIndex[i] = -1;
		FBuyQty[i]  = 0;
		FSellQty[i] = 0;
	}
	for( register int i = 0; i < RowCount; i++ )
	{
		FBuyDepth5Qty[i] = -1;
		FSellDepth5Qty[i] = -1;
		FUserBuyQty[i] = -1;
		FUserSellQty[i] = -1;
		FBuyFillQty[i] = -1;
		FSellFillQty[i] = -1;
		FBuyConditionQty[i] = -1;
		FSellConditionQty[i] = -1;
		FBuyOCOQty[i] = 0;
		FSellOCOQty[i] = 0;
	}
	if( FSettingMode == true )
	{
		FBuyPxIndex[0] = GetRowIndex( 8235 );
		FBuyPxIndex[1] = GetRowIndex( 8234 );
		FBuyPxIndex[2] = GetRowIndex( 8233 );
		FBuyDepth5Qty[FBuyPxIndex[0]] = 5;
		FBuyDepth5Qty[FBuyPxIndex[1]] = 8;
		FBuyDepth5Qty[FBuyPxIndex[2]] = 10;
		FUserBuyQty[FBuyPxIndex[1]] = 3;

		FSellPxIndex[0] = GetRowIndex( 8236 );
		FSellPxIndex[1] = GetRowIndex( 8237 );
		FSellPxIndex[2] = GetRowIndex( 8238 );
		FSellDepth5Qty[FSellPxIndex[0]] = 3;
		FSellDepth5Qty[FSellPxIndex[1]] = 6;
		FSellDepth5Qty[FSellPxIndex[2]] = 9;
		FUserSellQty[FSellPxIndex[2]] = 5;
		CenterPx( 8235.0, true );
		FTotalBuyQty = 3;
		FTotalSellQty = 5;
		FBuyAvgFillPx = 8234;
		FSellAvgFillPx = 8236;
		FBuyFillQty[GetRowIndex( 8234 )] = 3;
		FSellFillQty[GetRowIndex( 8236 )] = 5;
	}
	CalSize();

	if( FillPrice > BullPrice || FillPrice < BearPrice )
		FFillPx = BullPrice;
	else
		FFillPx = FillPrice;
	FLastFillPx = FFillPx;
	FFillRowIndex = GetRowIndex( FFillPx );
	FAvgFillPxIndex = -1;
	FMarkPxIndex = -1;
	FMarkPx2Index = -1;
	Subscribe( FStore );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetBullPrice( double Price )
{
	FBullPx = Price;

	if( ComponentState.Contains( csDesigning ) )
	{
		InitialGrid( FBullPx, FBearPx, FFillPx, FDigit );
		CalSize();
		Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetBearPrice( double Price )
{
	FBearPx = Price;

	if( ComponentState.Contains( csDesigning ) )
	{
		InitialGrid( FBullPx, FBearPx, FFillPx, FDigit );
		CalSize();
		Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetFillPrice( double Price )
{
	if( IsLoaded == false )
		return;

	if( Price <= FBullPx && Price >= FBearPx )
	{
		FFillPx = Price;
		int OrigIndex = FFillRowIndex;
		FFillRowIndex = GetRowIndex( Price );
		int Diff = abs( OrigIndex - FFillRowIndex );

		if( FBetterEnable )
		{
			InvalidateBoxRect( PRICE_COL,  TopRow ,PRICE_COL,  TopRow + VisibleRowCount );///< Price col
			return;
		}
		for( register int i = 0; i <= Diff; i++ )
		{
			if( OrigIndex < FFillRowIndex )
				InvalidateCellRect( PRICE_COL, FFillRowIndex - i );
			else
				InvalidateCellRect( PRICE_COL, FFillRowIndex + i );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetAvgFillPx( double AvgPx )
{
	int OrigIndex = FAvgFillPxIndex;

	if( AvgPx <= FBullPx && AvgPx >= FBearPx )
	{
		FAvgFillPx = AvgPx;
		FAvgFillPxIndex = GetRowIndex( AvgPx );
		if( OrigIndex != -1 )
		{
			int Diff = abs( OrigIndex - FFillRowIndex );

			for( register int i = 0; i <= Diff; i++ )
			{
				if( OrigIndex < FAvgFillPxIndex )
					InvalidateCellRect( PRICE_COL, FAvgFillPxIndex - i );
				else
					InvalidateCellRect( PRICE_COL, FAvgFillPxIndex + i );
			}
		}
		InvalidateCellRect( PRICE_COL, FAvgFillPxIndex  );
	}
	else
	{
		FAvgFillPxIndex = -1;
		if( OrigIndex != -1 )
			InvalidateCellRect( PRICE_COL, OrigIndex );
	}
}
//---------------------------------------------------------------------------
double __fastcall TOrderBookList::GetPxFromIndex( int Index )
{
	if( ComponentState.Contains( csDesigning ) || FSettingMode )
	{
		return FBullPx - Index + 2;
	}
	return FTickList->GetPxFromIndex( Index - 2, false );
}
//---------------------------------------------------------------------------
int __fastcall TOrderBookList::GetRowIndex( double Price, int* PxIndex )
{
	if( ComponentState.Contains( csDesigning ) || FSettingMode )
	{
		int Index = ( (int)FBullPx - (int)Price ) + 2;
		if( PxIndex != NULL )
			*PxIndex = 0;
		return Index;
	}
	try
	{
		int Index = FTickList->FindTick( Price );

		if( PxIndex != NULL )
			*PxIndex = Index;
		if( Index >= 0 )
			return Index + 2 ;
		return Index;   ///< Index == -1;
	}
	catch( Exception& )
	{
		if( PxIndex != NULL )
			*PxIndex = 0;
        return 2;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::UpdateOrder( AnsiString Symbol, double Price, nsOrderMessageDefine::SideEnum side, int Qty, nsOrderMessageDefine::OrderTypeEnum Type ) {}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::UpdateQty( nsOrderMessageDefine::SideEnum side, double Price, int Qty )
{
	if( IsLoaded == false )
		return;
	if( side == nsOrderMessageDefine::sBuy )
	{
		int Index = GetRowIndex( Price );
		if( Index >= 0 && Index < FUserBuyQty.Length )
		{
			if( FUserBuyQty[Index] > 0 )
				FTotalBuyQty = FTotalBuyQty - FUserBuyQty[Index] + Qty;
			else
				FTotalBuyQty += Qty;
			FUserBuyQty[Index] = Qty;
			InvalidateCellRect( BUY_ORDER_COL, Index );
			InvalidateCellRect( BUY_ORDER_COL, 1 );
		}
	}
	else
	{
		int Index = GetRowIndex( Price );
		if( Index >= 0 && Index < FUserSellQty.Length )
		{
			if( FUserSellQty[Index] > 0 )
				FTotalSellQty = FTotalSellQty - FUserSellQty[Index] + Qty;
			else
				FTotalSellQty += Qty;
			FUserSellQty[Index] = Qty;
			InvalidateCellRect( SELL_ORDER_COL, Index );
			InvalidateCellRect( SELL_ORDER_COL, 1 );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::UpdateStopOrderQty( nsOrderMessageDefine::SideEnum side, double Price, int Qty )
{
	if( IsLoaded == false )
		return;
	if( side == nsOrderMessageDefine::sBuy )
	{
		int Index = GetRowIndex( Price );
		if( Index >= 0 && Index < FBuyConditionQty.Length )
		{
			if( FBuyConditionQty[Index] > 0 )
				FTotalStopBuyQty = FTotalStopBuyQty - FBuyConditionQty[Index] + Qty;
			else
				FTotalStopBuyQty += Qty;
			FBuyConditionQty[Index] = Qty;
			InvalidateCellRect( BUY_CONDITION_COL, Index );
			InvalidateCellRect( BUY_CONDITION_COL, 1 );
		}
	}
	else
	{
		int Index = GetRowIndex( Price );
		if( Index >= 0 && Index < FSellConditionQty.Length )
		{
			if( FSellConditionQty[Index] > 0 )
				FTotalStopSellQty = FTotalStopSellQty - FSellConditionQty[Index] + Qty;
			else
				FTotalStopSellQty += Qty;
			FSellConditionQty[Index] = Qty;
			InvalidateCellRect( SELL_CONDITION_COL, Index );
			InvalidateCellRect( SELL_CONDITION_COL, 1 );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::InvalidateCellRect( int COL, int ROW )
{
	TRect URect = CellRect( COL,  ROW );

	DrawCell( COL, ROW, URect, (TGridDrawState)0 );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::InvalidateColumn( int COL, int TopROW, int DownROW )
{
	TGridDrawState AState;

	AState.Clear();
	::SelectClipRgn( Canvas->Handle, NULL );
	for( register int i = TopROW; i <= DownROW; i++ )
	{
		TRect URect = CellRect( COL,  i );
		DrawCell( COL, i, URect, AState );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::InvalidateBoxRect( int LeftCOL, int TopROW, int RightCOL, int DownROW )
{
	TGridDrawState AState;

	::SelectClipRgn( Canvas->Handle, NULL );
	for( register int i= LeftCOL; i <= RightCOL; i ++ )
	{
		for( register int j= TopROW; j <= DownROW; j ++ )
		{
			TRect URect = CellRect( i,  j );
			DrawCell( i, j, URect, AState );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::CenterPx( double Px, bool NeedFoucs )
{
	int CenterRow        = GetRowIndex( Px );
	int ShowRowCount     = Height / DefaultRowHeight - 2;
	int HalfShowRowCount = ShowRowCount / 2;
	int OrigTopRow       = TopRow;
	int NewTopRow;

	if( CenterRow <= HalfShowRowCount )
		NewTopRow = 2;
	else if( RowCount - CenterRow <= HalfShowRowCount )
		NewTopRow = RowCount - ShowRowCount;
	else
		NewTopRow = CenterRow - HalfShowRowCount;
	if( OrigTopRow != NewTopRow )
	{
		TopRow = NewTopRow;
		FocusCell( PRICE_COL, CenterRow, true );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::TopBullPx( void )
{
	TopRow = 2;
	FocusCell( PRICE_COL, TopRow, true );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::BottomBearPx( void )
{
	TopRow = RowCount - DefaultRowHeight + 2;
	FocusCell( PRICE_COL, TopRow, true );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::CenterPx( bool NeedFoucs )
{
	int ShowRowCount     = Height / DefaultRowHeight - 2;
	int HalfShowRowCount = ShowRowCount / 2;
	int OrigTopRow       = TopRow;
	int NewTopRow;

	if( FFillRowIndex <= HalfShowRowCount ) ///< Near bull Px
		NewTopRow = 2;
	else if( RowCount - FFillRowIndex <= HalfShowRowCount ) ///< Near Bear Px
		NewTopRow = RowCount - ShowRowCount;
	else
		NewTopRow = FFillRowIndex - HalfShowRowCount;
	if( OrigTopRow != NewTopRow ) ///< Fill price changed.
	{
		TopRow = NewTopRow;
		if( NeedFoucs == true )
			FocusCell( PRICE_COL, FFillRowIndex, true );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::CenterFillPx( bool force )
{
	if( force == false && FNeedCenterFill == false )
		return;
	CenterPx( true );
}
//---------------------------------------------------------------------------
void _fastcall TOrderBookList::DrawCell(int ACol, int ARow, const Types::TRect &ARect, TGridDrawState AState )
{
	if( IsLoaded == false )
		return;
	if( ARow > RowCount )
		return;
	if( ARect.Width() ==0 || ARect.Height() == 0 )
		return;
	if( ARow == 0 )
	{
		DrawHeader( ACol, ARect, AState );
		return;
	}
	else if( ARow == 1 )
	{
		DrawSumRow( ACol, ARect, AState );
		return;
	}
	switch( ACol )
	{
		case BUY_FILL_COL		: DrawBuyFillCol( ARow, ARect, AState ); break;
		case BUY_ORDER_COL		: DrawBuyOrderCol( ARow, ARect, AState ); break;
		case BUY_MK_COL			: DrawBuyMKCol( ARow, ARect, AState ); break;
		case PRICE_COL			: DrawTick( ARow, ARect, AState ); break;
		case SELL_MK_COL		: DrawSellMKCol( ARow, ARect, AState ); break;
		case SELL_ORDER_COL		: DrawSellOrderCol( ARow, ARect, AState ); break;
		case BUY_DEL_BTN_COL	:
		case SELL_DEL_BTN_COL	: DrawDelCol( ACol, ARow, ARect, AState ); break;
		case BUY_ORDER_BTN_COL	:
		case SELL_ORDER_BTN_COL	: DrawButtonCol( ACol, ARow, ARect, AState ); break;
		case SELL_FILL_COL		: DrawSellFillCol( ARow, ARect, AState ); break;
		case BUY_CONDITION_COL	:
		case SELL_CONDITION_COL	: DrawConditionCol( ACol, ARow, ARect, AState ); break;
		case BUY_OCO_COL        :
		case SELL_OCO_COL       : DrawOCOCol( ACol, ARow, ARect, AState ); break;
		case BUY_OCODEL_COL     :
		case SELL_OCODEL_COL    : DrawOCODelCol( ACol, ARow, ARect, AState ); break;
		default : break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawGridLine( Graphics::TBitmap* Bmp, const Types::TRect &ARect )
{
	FBufferBmp->Canvas->Pen->Color = FGridColor;
	Bmp->Canvas->MoveTo( ARect.Left , ARect.Bottom - 1 );
	Bmp->Canvas->LineTo( ARect.Right - 1, ARect.Bottom - 1 );
	Bmp->Canvas->MoveTo( ARect.Right - 1, ARect.Top );
	Bmp->Canvas->LineTo( ARect.Right - 1, ARect.Bottom );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawGradientBar( TCanvas* canvas, TRect& dRect,TColor UpCol, TColor DownCol )
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
	GradientFill( canvas->Handle, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawHeader( int Col, const Types::TRect &ARect, TGridDrawState AState )
{
	TRect       PaintRect = Rect( 0, 0, ARect.Width(), DefaultRowHeight );
	TTextFormat Formats;
	String      Text;

	FBufferBmp->Canvas->Font->Assign( FFont );
	FBufferBmp->Canvas->Brush->Style = bsClear;
	Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
	if( FBufferBmp->Width != ARect.Width() || FBufferBmp->Height != DefaultRowHeight )
	{
		FBufferBmp->Width = ARect.Width();
		FBufferBmp->Height = DefaultRowHeight;
	}
	if( Col > PRICE_COL ) ///< Sell
	{
		DrawGradientBar( FBufferBmp->Canvas, PaintRect, FFixedRowBidColor, FFixedRowBidColor);//FFixedRowBidEndColor );
	}
	else if( Col < PRICE_COL ) ///< Buy
	{
		DrawGradientBar( FBufferBmp->Canvas, PaintRect, FFixedRowAskColor, FFixedRowAskColor);//FFixedRowAskEndColor );
	}
	else ///< Price cell
	{
		DrawGradientBar( FBufferBmp->Canvas, PaintRect, FFixedRowBKColor, FFixedRowBKColor);//FFixedRowBKEndColor );
	}
	FBufferBmp->Canvas->Font->Color = FFixedRowColor;

	if( Col == BUY_FILL_COL )
	{
		if( SettingMode )
			Text.printf( L"%s", HeaderString[Col].c_str() );
		else
			Text.printf( L"%s(%d)", HeaderString[Col].c_str(), FBuySideFillQty );
	}
	else if( Col == SELL_FILL_COL )
	{
		if( SettingMode )
			Text.printf( L"%s", HeaderString[Col].c_str() );
		else
			Text.printf( L"%s(%d)", HeaderString[Col].c_str(), FSellSideFillQty );
	}
	else if( Col == PRICE_COL )
	{
		if( FHH == 0 && FMM == 0 && FSS == 0)
			Text.printf( L"%s", HeaderString[Col].c_str() );
		else
			Text.printf( L"%02d:%02d:%02d", FHH, FMM, FSS );
	}
	else
		Text = HeaderString[ Col ];
	FBufferBmp->Canvas->TextRect( PaintRect, Text, Formats );
	DrawGridLine( FBufferBmp, PaintRect );
	Canvas->Draw( ARect.Left, ARect.Top , FBufferBmp );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawSumRow( int Col, const Types::TRect &ARect, TGridDrawState AState )
{
	TRect PaintRect = Rect( 0, 0, ARect.Width(),  DefaultRowHeight );
	int Qty = 0;
	double Px = 0;
	TColor FrontColor, BKColor,BKEndColor;
	if( FBufferBmp->Width != ARect.Width() || FBufferBmp->Height != DefaultRowHeight )
	{
		FBufferBmp->Width = ARect.Width();
		FBufferBmp->Height = DefaultRowHeight;
	}

	FBufferBmp->Canvas->Brush->Style = bsClear;
	FBufferBmp->Canvas->Font->Assign( FFont );
	if( FMouseCoord.Y == 1 && Col == FMouseCoord.X &&
		( FEnableHotTracks && ( Col == BUY_CONDITION_COL || Col == SELL_CONDITION_COL ) ||
		( FIsCompact && ( Col == BUY_ORDER_COL || Col == SELL_ORDER_COL ) ) ) )
	{
		FrontColor = FHotTracksColor;
		BKColor = FHotTracksBKColor;
		BKEndColor = clBlack;
	}
	else
	{
		if( Col > PRICE_COL ) ///< Sell
		{
			BKColor = FFixedRowBidColor;
			BKEndColor = FFixedRowBidEndColor;
			FBufferBmp->Canvas->Font->Color = FAskDepth5Color;//(TColor)RGB(17,71,144);
		}
		else if( Col < PRICE_COL ) ///< Buy
		{
			BKColor = FFixedRowAskColor;
			BKEndColor = FFixedRowAskEndColor;
			FBufferBmp->Canvas->Font->Color = FBidDepth5Color;//(TColor)RGB(250,101,138);
		}
		else
		{
			BKColor = FFixedRowBKColor;
			BKEndColor = FFixedRowBKEndColor;
			FBufferBmp->Canvas->Font->Color = FrontColor;
		}
		FrontColor = FFixedRowColor;
	}
	DrawGradientBar( FBufferBmp->Canvas, PaintRect, BKColor, BKColor );

	if( Col == BUY_MK_COL )
		Qty = FMarketTotalBuyQty;
	else if ( Col == SELL_MK_COL )
		Qty = FMarketTotalSellQty;
	else if( Col == BUY_ORDER_COL )
		Qty = FTotalBuyQty;
	else if( Col == SELL_ORDER_COL )
		Qty = FTotalSellQty;
	else if( Col == BUY_CONDITION_COL )
		Qty = FTotalStopBuyQty;
	else if( Col == SELL_CONDITION_COL )
		Qty = FTotalStopSellQty;
	else if( Col == BUY_OCO_COL )
		Qty = FTotalOCOBuyQty;
	else if( Col == SELL_OCO_COL )
		Qty = FTotalOCOSellQty;
	else if( Col == BUY_FILL_COL )
	{
		Px = FBuyAvgFillPx;
	}
	else if( Col == SELL_FILL_COL )
	{
		Px = FSellAvgFillPx;
	}
	else if( Col == PRICE_COL )
	{
		int Total = FMarketTotalBuyQty + FMarketTotalSellQty;
		int BuyPercent;
		String      QtyStr;
		TTextFormat Formats;

		if( Total > 0 )
			BuyPercent = (int)(((float)FMarketTotalBuyQty* 100) / ((float)Total));
		else
			BuyPercent = 50;
		int BuyWidth = PaintRect.Width()*BuyPercent/100;
		TRect LeftRect( PaintRect.left, PaintRect.top, PaintRect.left + BuyWidth, PaintRect.bottom);
		TRect RightRect( PaintRect.left + BuyWidth, PaintRect.top, PaintRect.right,  PaintRect.bottom );

		Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		DrawGradientBar( FBufferBmp->Canvas , LeftRect,  (TColor)RGB(230,0,0),  (TColor)RGB(230,0,0) );//, false );
		DrawGradientBar( FBufferBmp->Canvas , RightRect, (TColor)RGB(33,140,4), (TColor)RGB(33,140,4));//, false );
		QtyStr.printf( L"%d:%d", BuyPercent, 100 - BuyPercent );
		FBufferBmp->Canvas->Font->Color = FrontColor;
		FBufferBmp->Canvas->TextRect( PaintRect, QtyStr, Formats );
	}
	else
	{
		DrawGridLine( FBufferBmp, PaintRect );
		Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
		return;
	}

	if( Px < 0 )
		Px = 0;

	if( Qty < 0 )
		Qty = 0;

	String Text;

	if( Col == BUY_FILL_COL || Col == SELL_FILL_COL)
	{
		if( !SettingMode )
			Text.printf( L"%.*f", FDigit+1, Px );
		else
			Text.printf( L"%.0f", Px );
	}
	else
		Text.printf( L"%d", Qty );
	if( Col == BUY_MK_COL || Col == BUY_ORDER_COL )
	{
		TTextFormat Formats;

		Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		FBufferBmp->Canvas->TextRect( PaintRect, Text, Formats );
	}
	else if( Col == SELL_MK_COL || Col == SELL_ORDER_COL )
	{
		TTextFormat Formats;

		Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		FBufferBmp->Canvas->TextRect( PaintRect, Text, Formats );
	}
	else if( Col == BUY_FILL_COL || Col == SELL_FILL_COL )
	{
		TTextFormat Formats;

		Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		FBufferBmp->Canvas->TextRect( PaintRect, Text, Formats );
	}
	else if( Col == BUY_DEL_BTN_COL || Col == SELL_DEL_BTN_COL )
	{
        TTextFormat Formats;

		Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		FBufferBmp->Canvas->TextRect( PaintRect, Text, Formats );
	}
	else if( Col == BUY_CONDITION_COL || Col == SELL_CONDITION_COL )
	{
		TTextFormat Formats;

		Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		FBufferBmp->Canvas->TextRect( PaintRect, Text, Formats );
	}
	else if( Col == BUY_OCO_COL || Col == SELL_OCO_COL )
	{
        TTextFormat Formats;

		Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		FBufferBmp->Canvas->TextRect( PaintRect, Text, Formats );
	}
	else if( Col == BUY_OCODEL_COL || Col == SELL_OCODEL_COL )
	{
        TTextFormat Formats;

		Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		FBufferBmp->Canvas->TextRect( PaintRect, Text, Formats );
	}
	DrawGridLine( FBufferBmp, PaintRect );
	Canvas->Draw( ARect.Left , ARect.Top, FBufferBmp );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawTick( int Row, const Types::TRect &ARect, TGridDrawState AState )
{
	UnicodeString TickString;
	TColor        RectColor;
	TRect         PaintRect   = Rect( 0, 0, ARect.Width(), DefaultRowHeight );
	TRect         TextOutRect = Rect( 3, 0, ARect.Width()- 3, DefaultRowHeight );
	double        CellPx = GetPxFromIndex( Row );
	TTextFormat   Formats;

	if( FTickList == NULL || ComponentState.Contains( csDesigning ) )
		TickString.printf( L"%.*f", FDigit, CellPx );
	else
		FTickList->ToTNFormat( CellPx, TickString );
	if( FBufferBmp->Width != PaintRect.Width() || FBufferBmp->Height != DefaultRowHeight )
	{
		FBufferBmp->Width = PaintRect.Width();
		FBufferBmp->Height = DefaultRowHeight;
	}
	FBufferBmp->Canvas->Font->Assign( FFont );
	if( Row == 2 ) ///< Bull Price
	{
		FBufferBmp->Canvas->Brush->Color = clWebHotPink;
		FBufferBmp->Canvas->Font->Color = clBlack;
	}
	else if( Row == RowCount - 1 ) ///< Bear Price
	{
		FBufferBmp->Canvas->Brush->Color = clWebLawnGreen;
		FBufferBmp->Canvas->Font->Color = clBlack;
	}
	else if( ( Row == FFillRowIndex + FBetterStep ) && FBetterStep != 0 )  ///< Butter Buy
	{
		FBufferBmp->Canvas->Brush->Color = FBetterBuyBKColor;
		FBufferBmp->Canvas->Font->Color = FBetterBuyColor;
	}
	else if( Row == FFillRowIndex - FBetterStep && FBetterStep != 0 )  ///< Butter Sell
	{
		FBufferBmp->Canvas->Brush->Color = FBetterSellBKColor;
		FBufferBmp->Canvas->Font->Color = FBetterSellColor;
	}
	else if( Row < FFillRowIndex ) ///< Price > Filled Px
	{
		FBufferBmp->Canvas->Brush->Color = FTickBKColor;
		FBufferBmp->Canvas->Font->Color = FTickUpperColor;
	}
	else if( Row > FFillRowIndex ) ///> Price < Filled Px
	{
		FBufferBmp->Canvas->Brush->Color = FTickBKColor;
		FBufferBmp->Canvas->Font->Color = FTickLowerColor;
	}
	else ///< Filled Px
	{
		FBufferBmp->Canvas->Font->Color = FFillColor;
		if( FChangeFillColor == true && FFillRowIndex <= FSellPxIndex[0] )
		{
			FBufferBmp->Canvas->Brush->Color = clRed;
			RectColor = clRed;
		}
		else if( FChangeFillColor == true && FFillRowIndex >= FBuyPxIndex[0]  )
		{
			FBufferBmp->Canvas->Brush->Color = clGreen;
			RectColor = clGreen;
		}
		else
		{
			FBufferBmp->Canvas->Brush->Color = FFillBKColor;
			RectColor = FFillBKColor;
		}
	}
	FBufferBmp->Canvas->FillRect( PaintRect );
	///< Draw price text
	if( FShowFillQty == true  )
	{
		Formats <<tfSingleLine<<tfLeft<<tfVerticalCenter<<tfEndEllipsis;
		FBufferBmp->Canvas->TextRect( TextOutRect, TickString, Formats );
		if( Row == FFillRowIndex )
		{
			TTextFormat RFormats;

			RFormats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
			TickString.printf( L"%d", FFillQty );
			FBufferBmp->Canvas->TextRect( TextOutRect, TickString, RFormats );
			Frame3D( FBufferBmp->Canvas, PaintRect, LighterColor( RectColor ), DarkerColor( RectColor ), 2 );
		}
		else
			DrawGridLine( FBufferBmp, PaintRect );
	}
	else
	{
		Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		FBufferBmp->Canvas->TextRect( TextOutRect, TickString, Formats );
		if( Row == FFillRowIndex )
			Frame3D( FBufferBmp->Canvas, PaintRect, LighterColor( RectColor ), DarkerColor( RectColor ), 2 );
		else
			DrawGridLine( FBufferBmp, PaintRect );
	}
    ///< Day H/L avg px cell.
	if( CellPx == FDayH )  ///< Day Height Px
		Frame3D( FBufferBmp->Canvas, PaintRect, LighterColor( FDayHFrameColor ), DarkerColor( FDayHFrameColor ), 2 );
	else if( CellPx == FDayL ) ///< Day low Px
		Frame3D( FBufferBmp->Canvas, PaintRect, LighterColor( FDayLFrameColor ), DarkerColor( FDayLFrameColor ), 2 );
	if(	FMarkPxIndex == Row ) ///< Marketd Px
	{
		Frame3D( FBufferBmp->Canvas, PaintRect, clSilver, clGray, 2 );
		if( FMarkPxAlarm == true )
			FBufferBmp->Canvas->Draw( 2,2, FAlarmBmp );
		else
			FBufferBmp->Canvas->Draw( 2,2, FAlarmedBmp );
	}
	if(	FMarkPx2Index == Row ) ///< Marketd Px
	{
		Frame3D( FBufferBmp->Canvas, PaintRect, clSilver, clGray, 2 );
		if( FMarkPx2Alarm == true )
			FBufferBmp->Canvas->Draw( ARect.Width() - FAlarmBmp->Width - 2, 2, FAlarmBmp );
		else
			FBufferBmp->Canvas->Draw( ARect.Width() - FAlarmBmp->Width - 2, 2, FAlarmedBmp );
	}
	if(	FAvgFillPxIndex == Row ) ///< Avg filled Px
	{
		FBufferBmp->Canvas->Pen->Color = FAvgPxColor;
		FBufferBmp->Canvas->Pen->Width = 2;
		FBufferBmp->Canvas->MoveTo( PaintRect.Left, PaintRect.Bottom - 1 );
		FBufferBmp->Canvas->LineTo( PaintRect.Right, PaintRect.Bottom - 1 );
		FBufferBmp->Canvas->Pen->Width = 1;
	}
	Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
}
//------------------------------------------------------------------------------
double __fastcall TOrderBookList::GetTickPrice( int Step )
{
	int PxIndex  = FFillRowIndex + Step;
	int IndexMax = FTickList->ItemCount() +1;

	if( PxIndex < 2 )
		PxIndex = 2;
	if( PxIndex > IndexMax )
		PxIndex = IndexMax;
	return GetPxFromIndex( PxIndex );
}
//------------------------------------------------------------------------------
double __fastcall TOrderBookList::GetBetterPrice( nsOrderMessageDefine::SideEnum side, TBetterPxType PxType, int BetterTick )
{
	int BasePxIndex = -1;
	int PxIndex;

	switch( PxType )
	{
		case bptLastPx: BasePxIndex = FFillRowIndex;
						break;
		case bptAsk1Px: BasePxIndex = FSellPxIndex[ 0 ];
						break;
		case bptBid1Px: BasePxIndex = FBuyPxIndex[ 0 ];
						break;
	}
	if( BasePxIndex == -1 )
		return 0.0;
	if( side == nsOrderMessageDefine::sBuy )
		PxIndex = BasePxIndex - BetterTick;
	else
		PxIndex = BasePxIndex + BetterTick;
	return	GetPxFromIndex( PxIndex );
}
//------------------------------------------------------------------------------
double __fastcall TOrderBookList::GetBetterPrice( double Px, int BetterTick )
{
	int Index = FTickList->FindTick( Px );

	Index -= BetterTick;
	return FTickList->GetPxFromIndex( Index , false );
}
//------------------------------------------------------------------------------
TColor  __fastcall TOrderBookList::DarkerColor( TColor OrigCol )
{
	int R = GetRValue( OrigCol )>> 1;
	int G = GetGValue( OrigCol )>> 1;
	int B = GetBValue( OrigCol )>> 1;
	return (TColor)RGB (R, G, B );
}
//------------------------------------------------------------------------------
TColor  __fastcall TOrderBookList::LighterColor( TColor OrigCol )
{
	int R = GetRValue( OrigCol )<< 1;
	int G = GetGValue( OrigCol )<< 1;
	int B = GetBValue( OrigCol )<< 1;
	if( R > 255 ) R = 255;
	if( G > 255 ) G = 255;
	if( B > 255 ) B = 255;
	return (TColor)RGB (R, G, B );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawBuyFillCol( int Row, const Types::TRect &ARect, TGridDrawState AState )
{
	if( Row > 0 )
	{
		TRect PaintRect = Rect( 0, 0, ARect.Width(), DefaultRowHeight );
		if( FBufferBmp->Width != ARect.Width() || FBufferBmp->Height != DefaultRowHeight )
		{
			FBufferBmp->Width = ARect.Width();
			FBufferBmp->Height = DefaultRowHeight;
		}
		FBufferBmp->Canvas->Brush->Color = FBuyFillBKColor;
		FBufferBmp->Canvas->FillRect( PaintRect );

		DrawQty( FBufferBmp, &PaintRect, tfRight, FBuyFillQty[Row], FBuyFillColor );
		DrawGridLine( FBufferBmp, PaintRect );
		Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawSellFillCol( int Row, const Types::TRect &ARect, TGridDrawState AState )
{
	if( Row > 0 )
	{
		TRect PaintRect = Rect( 0, 0, ColWidths[SELL_FILL_COL], DefaultRowHeight );
		if( FBufferBmp->Width != ARect.Width() || FBufferBmp->Height != DefaultRowHeight )
		{
			//FBufferBmp->Width = ARect.Width();
			FBufferBmp->Width = ColWidths[SELL_FILL_COL];
			FBufferBmp->Height = DefaultRowHeight;
		}
		FBufferBmp->Canvas->Brush->Color = FSellFillBKColor;
		FBufferBmp->Canvas->FillRect( PaintRect );

		DrawQty( FBufferBmp, &PaintRect, tfRight, FSellFillQty[Row], FSellFillColor );
		DrawGridLine( FBufferBmp, PaintRect );
		Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawConditionCol( int ACol, int ARow, const Types::TRect &ARect, TGridDrawState AState )
{
	if( ARow > 0 )
	{
		TRect PaintRect = Rect( 0, 0, ARect.Width(), DefaultRowHeight );
		TColor FrontColor, BKColor;
		if( FBufferBmp->Width != ARect.Width() || FBufferBmp->Height != DefaultRowHeight )
		{
			FBufferBmp->Width = ARect.Width();
			FBufferBmp->Height = DefaultRowHeight;
		}

		if( FEnableHotTracks && ARow == FMouseCoord.Y && ACol == FMouseCoord.X )
		{
			FrontColor = FHotTracksColor;
			BKColor = FHotTracksBKColor;
		}
		else
		{
			if( ACol == BUY_CONDITION_COL )
			{
				FrontColor = FBuyConditionColColor;
				BKColor = FBuyConditionColBKColor;
			}
			else
			{
				FrontColor = FSellConditionColColor;
				BKColor = FSellConditionColBKColor;
			}
		}
		FBufferBmp->Canvas->Brush->Color = BKColor;
		FBufferBmp->Canvas->FillRect( PaintRect );

		if( ACol == BUY_CONDITION_COL )
			DrawQty( FBufferBmp, &PaintRect, tfRight, FBuyConditionQty[ARow], FrontColor );
		else
			DrawQty( FBufferBmp, &PaintRect, tfRight, FSellConditionQty[ARow], FrontColor );

		DrawGridLine( FBufferBmp, PaintRect );
		Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawOCOCol( int ACol, int ARow, const Types::TRect &ARect, TGridDrawState AState )
{
	if( ARow <= 0 )
		return;

	TRect PaintRect = Rect( 0, 0, ARect.Width(), DefaultRowHeight );
	if( FBufferBmp->Width != ARect.Width() || FBufferBmp->Height != DefaultRowHeight )
	{
		FBufferBmp->Width = ARect.Width();
		FBufferBmp->Height = DefaultRowHeight;
	}

	TColor FrontColor, BKColor;
	bool isHotTrack = 	FEnableHotTracks &&
						ARow == FMouseCoord.Y &&
						ACol == FMouseCoord.X;
	if (isHotTrack)
	{
		FrontColor = FHotTracksColor;
		BKColor    = FHotTracksBKColor;
	}
	else
	{
		bool isBuyCol = (ACol == BUY_OCO_COL);
		FrontColor = isBuyCol ? FBuyOCOColColor : FSellOCOColColor;
		BKColor    = isBuyCol ? FBuyOCOColBKColor : FSellOCOColBKColor;
	}

	FBufferBmp->Canvas->Brush->Color = BKColor;
	FBufferBmp->Canvas->FillRect( PaintRect );
	DrawOCOQty( FBufferBmp, &PaintRect, ACol, ARow, FrontColor );

	DrawGridLine( FBufferBmp, PaintRect );
	Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawOCODelCol( int ACol, int ARow, const Types::TRect &ARect, TGridDrawState AState )
{
	if( ARow <= 0 )
		return;

	TRect PaintRect = Rect( 0, 0, ARect.Width(), DefaultRowHeight );
	if( FBufferBmp->Width != ARect.Width() || FBufferBmp->Height != DefaultRowHeight )
	{
		FBufferBmp->Width = ARect.Width();
		FBufferBmp->Height = DefaultRowHeight;
	}

	TColor BKColor;
	bool isHotTrack = 	FEnableHotTracks &&
						ARow == FMouseCoord.Y &&
						ACol == FMouseCoord.X;
	if (isHotTrack)
	{
		BKColor = FHotTracksBKColor;
	}
	else
	{
		bool isBuyCol = (ACol == BUY_OCODEL_COL);
		BKColor    = isBuyCol ? FBuyOCODelColBKColor : FSellOCODelColBKColor;
	}
	FBufferBmp->Canvas->Brush->Color = BKColor;
	FBufferBmp->Canvas->FillRect( PaintRect );

	DrawGridLine( FBufferBmp, PaintRect );
	Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawQty( Graphics::TBitmap* Bmp, Types::TRect* ARect, TTextFormats Align, int Qty, TColor TextColor, int DerivedQty )
{
	TTextFormat Formats;
	TRect       PaintRect( *ARect );

	PaintRect.Left  += 5;
	PaintRect.Right -= 5;
	Formats <<tfSingleLine<<tfVerticalCenter<<tfEndEllipsis<<Align;
	if( Qty > 0 )
	{
		String Text;

		if( DerivedQty > 0 && FShowDerivedPx == true )
			Text.printf( L"%d(%d)", Qty, DerivedQty );
		else
			Text.printf( L"%d", Qty );
		Bmp->Canvas->Font->Assign( FFont );
		Bmp->Canvas->Font->Color = TextColor;
		Bmp->Canvas->TextRect( PaintRect, Text,Formats );
	}
	else if( DerivedQty > 0 && FShowDerivedPx == true )
	{
		UnicodeString Text;

		Text.printf( L"(%d)", DerivedQty );
		Bmp->Canvas->Font->Assign( FFont );
		Bmp->Canvas->Font->Color = TextColor;
		Bmp->Canvas->TextRect( PaintRect, Text, Formats );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawOCOQty(
	Graphics::TBitmap* Bmp,
	Types::TRect* ARect,
	int ACol,
	int ARow,
	TColor TextColor)
{
	int Qty = 0;
	if( ACol == BUY_OCO_COL )
		Qty = FBuyOCOQty[ARow];
	else
		Qty = FSellOCOQty[ARow];

	if( Qty > 0 )
	{
		TRect PaintRectRight( *ARect );

		PaintRectRight.Left += 5;
		PaintRectRight.Right-= 5;

		TTextFormat FormatsRight;
		FormatsRight<<tfSingleLine<<tfVerticalCenter<<tfEndEllipsis<<TTextFormats::tfRight;

		String TextRight;
		TextRight.printf(L"%d", Qty );
		Bmp->Canvas->Font->Assign( FFont );
		Bmp->Canvas->Font->Color = TextColor;
		Bmp->Canvas->TextRect( PaintRectRight, TextRight, FormatsRight );
	}

	if(!FIsPairingOCO)
		return;

	if( ACol == BUY_OCO_COL )
	{
		if(FCurrentPairOCO->OrderSide1 == nsOrderMessageDefine::sSell)
			return;
	}
	else
	{
		if(FCurrentPairOCO->OrderSide1 == nsOrderMessageDefine::sBuy)
			return;
	}

	if(ARow != GetRowIndex(FCurrentPairOCO->ConditionPrice1))
		return;

	TRect PaintRectLeft( *ARect );
	PaintRectLeft.Left	+= 5;
	PaintRectLeft.Right -= 5;
	PaintRectLeft.Right -= PaintRectLeft.Size.Width/2;

	TTextFormat FormatsLeft;
	FormatsLeft	<<tfSingleLine<<tfVerticalCenter<<tfEndEllipsis<<TTextFormats::tfLeft;

	String TextLeft;

	TextLeft.printf(L"%d", FCurrentPairOCO->OrderQty1);
	Bmp->Canvas->Font->Color = clWebLightGreen;
	Bmp->Canvas->TextRect( PaintRectLeft, TextLeft, FormatsLeft );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawBuyMKCol( int Row, const Types::TRect &ARect, TGridDrawState AState )
{
	if( Row > 0 )
	{
		int DerivedQty = 0;
		TColor FrontColor, BKColor;
		TRect PaintRect = Rect( 0, 0, ARect.Width(), DefaultRowHeight );
		if( FBufferBmp->Width != ARect.Width() || FBufferBmp->Height != DefaultRowHeight )
		{
			FBufferBmp->Width = ARect.Width();
			FBufferBmp->Height = DefaultRowHeight;
		}

		if( FEnableHotTracks && Row == FMouseCoord.Y && FMouseCoord.X == BUY_MK_COL )
		{
			FrontColor = FHotTracksColor;
			BKColor = FHotTracksBKColor;
		}
		else
		{
			FrontColor = FBidDepth5Color;
			BKColor = FBidDepth5BKColor;
		}

		FBufferBmp->Canvas->Brush->Color = BKColor;
		FBufferBmp->Canvas->FillRect( PaintRect );
		DrawGridLine( FBufferBmp, PaintRect );
		if( Row == FDerivedBuyIndex && FDerivedBuyQty > 0 )
			DerivedQty = FDerivedBuyQty;

		DrawQty( FBufferBmp, &PaintRect, tfRight, FBuyDepth5Qty[Row], FrontColor, DerivedQty );
		Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawSellMKCol( int Row, const Types::TRect &ARect, TGridDrawState AState )
{
	if( Row > 0 )
	{
		int DerivedQty = 0;
		TRect PaintRect = Rect( 0, 0, ARect.Width(), DefaultRowHeight );
		TColor FrontColor, BKColor;
		if( FBufferBmp->Width != ARect.Width() || FBufferBmp->Height != DefaultRowHeight )
		{
			FBufferBmp->Width = ARect.Width();
			FBufferBmp->Height = DefaultRowHeight;
		}

		if( FEnableHotTracks && Row == FMouseCoord.Y && SELL_MK_COL == FMouseCoord.X )
		{
			FrontColor = FHotTracksColor;
			BKColor = FHotTracksBKColor;
		}
		else
		{
			FrontColor = FAskDepth5Color;
			BKColor = FAskDepth5BKColor;
		}
		FBufferBmp->Canvas->Brush->Color = BKColor;
		FBufferBmp->Canvas->FillRect( PaintRect );
		DrawGridLine( FBufferBmp, PaintRect );
		if( Row == FDerivedSellIndex && FDerivedSellQty > 0 )
			DerivedQty = FDerivedSellQty;
		DrawQty( FBufferBmp, &PaintRect, tfLeft, FSellDepth5Qty[Row], FrontColor, DerivedQty );
		Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawButtonCol( int Col, int Row, const Types::TRect &ARect, TGridDrawState AState )
{
	if( Row > 0 )
	{
		if( FBufferBmp->Width != ARect.Width() || FBufferBmp->Height != DefaultRowHeight )
		{
			FBufferBmp->Width = ARect.Width();
			FBufferBmp->Height = DefaultRowHeight;
		}

		if( Col == BUY_ORDER_BTN_COL || Col == SELL_ORDER_BTN_COL )
		{

			if( Row == FMouseCoord.Y && Col == FMouseCoord.X )
			{
				TRect PaintRect = Rect( 0, 0, ARect.Width(), DefaultRowHeight );
				FBufferBmp->Canvas->StretchDraw( PaintRect, FPlusLightBmp );
				DrawGridLine( FBufferBmp, PaintRect );
			}
			else
			{
				TRect PaintRect = Rect( 0, 0, ARect.Width(), DefaultRowHeight );
				FBufferBmp->Canvas->StretchDraw( PaintRect, FPlusDarkBmp );
				DrawGridLine( FBufferBmp, PaintRect );
			}
			Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
		}
		else
		{
			if( Row == FMouseCoord.Y && Col == FMouseCoord.X )
			{
				TRect PaintRect = Rect( 0, 0, ARect.Width(), DefaultRowHeight );
				FBufferBmp->Canvas->StretchDraw( PaintRect, FMinusLightBmp );
				DrawGridLine( FBufferBmp, PaintRect );
			}
			else
			{
				TRect PaintRect = Rect( 0, 0, ARect.Width(), DefaultRowHeight );
				FBufferBmp->Canvas->StretchDraw( PaintRect, FMinusDarkBmp );
				DrawGridLine( FBufferBmp, PaintRect );
			}
			Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawBuyOrderCol( int Row, const Types::TRect &ARect, TGridDrawState AState )
{
	if( Row > 0 )
	{
		TRect PaintRect = Rect( 0, 0, ARect.Width(), DefaultRowHeight );
		TColor FrontColor, BKColor;
		if( FBufferBmp->Width != ARect.Width() || FBufferBmp->Height != DefaultRowHeight )
		{
			FBufferBmp->Width = ARect.Width();
			FBufferBmp->Height = DefaultRowHeight;
		}

		if( FEnableHotTracks && Row == FMouseCoord.Y && BUY_ORDER_COL == FMouseCoord.X )
		{
			FrontColor = FHotTracksColor;
			BKColor = FHotTracksBKColor;
		}
		else
		{
			FrontColor = FBuyColor;
			BKColor = FBuyBKColor;
		}

		FBufferBmp->Canvas->Brush->Color = BKColor;
		FBufferBmp->Canvas->FillRect( PaintRect );
		DrawGridLine( FBufferBmp, PaintRect );
		DrawQty( FBufferBmp, &PaintRect, tfRight, FUserBuyQty[Row], FrontColor );
		Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawSellOrderCol( int Row, const Types::TRect &ARect, TGridDrawState AState )
{
	if( Row > 0 )
	{
		TRect PaintRect = Rect( 0, 0, ARect.Width(), DefaultRowHeight );
		TColor FrontColor, BKColor;
		if( FBufferBmp->Width != ARect.Width() || FBufferBmp->Height != DefaultRowHeight )
		{
			FBufferBmp->Width = ARect.Width();
			FBufferBmp->Height = DefaultRowHeight;
		}

		if( FEnableHotTracks && Row == FMouseCoord.Y && SELL_ORDER_COL == FMouseCoord.X )
		{
			FrontColor = FHotTracksColor;
			BKColor = FHotTracksBKColor;
		}
		else
		{
			FrontColor = FSellColor;
			BKColor = FSellBKColor;
		}

		FBufferBmp->Canvas->Brush->Color = BKColor;
		FBufferBmp->Canvas->FillRect( PaintRect );
		DrawGridLine( FBufferBmp, PaintRect );
		DrawQty( FBufferBmp, &PaintRect, tfRight, FUserSellQty[Row], FrontColor );
		Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DrawDelCol( int ACol, int ARow, const Types::TRect &ARect, TGridDrawState AState )
{
	if( ARow <= 0 )
		return;

    TRect PaintRect = Rect( 0, 0, ARect.Width(), DefaultRowHeight );
	if( FBufferBmp->Width != ARect.Width() || FBufferBmp->Height != DefaultRowHeight )
	{
		FBufferBmp->Width = ARect.Width();
		FBufferBmp->Height = DefaultRowHeight;
	}

	TColor BKColor;
	bool isHotTrack = 	FEnableHotTracks &&
						ARow == FMouseCoord.Y &&
						ACol == FMouseCoord.X;
	if (isHotTrack)
	{
		BKColor = FHotTracksBKColor;
	}
	else
	{
		bool isBuyCol = (ACol == BUY_DEL_BTN_COL);
		BKColor = isBuyCol ? FBuyDelBKColor : FSellDelBKColor;
	}
	FBufferBmp->Canvas->Brush->Color = BKColor;
	FBufferBmp->Canvas->FillRect( PaintRect );

	DrawGridLine( FBufferBmp, PaintRect );
	Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetFont( TFont* Font )
{
	FFont->Assign( Font );
	CalSize();
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetCompact( bool IsCompact )
{
	FIsCompact = IsCompact;
	CalSize();
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetShowFilled( bool Show )
{
	FShowFilled = Show;
	CalSize();
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetBuyBKColor( TColor Color )
{
	FBuyBKColor = Color;
	InvalidateCol( BUY_ORDER_COL );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetSellBKColor( TColor Color )
{
	FSellBKColor = Color;
	InvalidateCol( SELL_ORDER_COL );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetBuyDelBKColor( TColor Color )
{
	FBuyDelBKColor = Color;
	InvalidateCol( BUY_DEL_BTN_COL );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetSellDelBKColor( TColor Color )
{
	FSellDelBKColor = Color;
	InvalidateCol( SELL_DEL_BTN_COL );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetFillColor( TColor Color )
{
	FFillColor = Color;
	InvalidateCellRect( PRICE_COL, FFillRowIndex );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetFillBKColor( TColor Color )
{
	FFillBKColor = Color;
	InvalidateCellRect( PRICE_COL, FFillRowIndex );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetFixedRowBKColor( TColor Color )
{
	FFixedRowBKColor = Color;
	InvalidateRow( 0 );
	InvalidateRow( 1 );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetFixedRowColor( TColor Color )
{
	FFixedRowColor = Color;
	InvalidateRow( 0 );
	InvalidateRow( 1 );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetFixedRowBKEndColor( TColor Color )
{
	FFixedRowBKEndColor = Color;
	InvalidateRow( 0 );
	InvalidateRow( 1 );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetFixedRowBidColor( TColor Color )
{
	FFixedRowBidColor = Color;
	InvalidateRow( 0 );
	InvalidateRow( 1 );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetFixedRowBidEndColor( TColor Color )
{
	FFixedRowBidEndColor = Color;
	InvalidateRow( 0 );
	InvalidateRow( 1 );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetFixedRowAskColor( TColor Color )
{
	FFixedRowAskColor = Color;
	InvalidateRow( 0 );
	InvalidateRow( 1 );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetFixedRowAskEndColor( TColor Color )
{
	FFixedRowAskEndColor = Color;
	InvalidateRow( 0 );
	InvalidateRow( 1 );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetBidDepth5Color( TColor Color )
{
	FBidDepth5Color = Color;
	InvalidateCol( BUY_MK_COL );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetBidDepth5BKColor( TColor Color )
{
	FBidDepth5BKColor = Color;
	InvalidateCol( BUY_MK_COL );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetAskDepth5Color( TColor Color )
{
	FAskDepth5Color = Color;
	InvalidateCol( SELL_MK_COL );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetAskDepth5BKColor( TColor Color )
{
	FAskDepth5BKColor = Color;
	InvalidateCol( SELL_MK_COL );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetGridColor( TColor Color )
{
	FGridColor = Color;
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::ChangeBetterValue( int NewBetter )
{
	int MaxStep = ( VisibleRowCount - 3 )/2 + 1;

	if( abs(NewBetter) <= MaxStep )
	{
		int PxIndex;
		int FillIndex = GetRowIndex( FFillPx, &PxIndex );
		int OldBetter = FBetterStep;
		if( NewBetter >= 0 )
		{
			if( PxIndex + abs(NewBetter) >= FTickList->ItemCount() )
				NewBetter = FTickList->ItemCount() - PxIndex -1;
			if( PxIndex - abs(NewBetter) < 0 )
				NewBetter = PxIndex;
		}
		else
		{
			if( PxIndex - NewBetter >= FTickList->ItemCount() )
				NewBetter = -(FTickList->ItemCount() - PxIndex -1);
			if( PxIndex + NewBetter < 0 )
				NewBetter = -PxIndex;
		}
		FBetterStep = NewBetter;
		InvalidateCellRect( PRICE_COL, FillIndex + OldBetter );
		InvalidateCellRect( PRICE_COL, FillIndex + NewBetter );
		InvalidateCellRect( PRICE_COL, FillIndex - OldBetter );
		InvalidateCellRect( PRICE_COL, FillIndex - NewBetter );
		::UpdateWindow( Handle );
		if( FCenterFillPrice == true )
			CenterFillPx();
		if( FOnStepChange != NULL )
			FOnStepChange( this, FBetterStep );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::KeyDown(System::Word &Key, Classes::TShiftState Shift)
{
//	if( Key == VK_PROCESSKEY )
//		ImmDisableIME( 0 );
//				Msg.WParam = ImmGetVirtualKey( Handle );
	if( FEnableHotKey == false )
	{
		if( Key == FKeyCenterFillPx )
		{
			CenterFillPx( true );
			if( FOnCenterPx )
				OnCenterPx( this );
		}
		else if( Key == FKeyBuy || Key == FKeySell || Key == FKeyBuyMarket || FKeySellMarket ||
				 Key == FKeyCancelBuy || Key == FKeyCancelSell || Key == FKeyNewOrder || Key == FKeyCancelAll )
		{
			if( FOnUILock != NULL )
				FOnUILock( this );
		}
		TCustomGrid::KeyDown( Key, Shift );
		return;
	}
	if( Key == FKeyBuy && FBetterEnable ) ///< Place order @better Px
	{
		if( FSync == true )
		{
			register TOrderBookList* Dapth;

			if( FInvertBS == true )
				OnKeySellDown( );
			else
				OnKeyBuyDown();
			for( register i = 0; i< FSyncDepth.ItemCount();i++)
			{
				if( (Dapth = FSyncDepth[i]) != this )
				{
					if( Dapth->FInvertBS == FInvertBS )
						Dapth->OnKeyBuyDown( );
					else
						Dapth->OnKeySellDown( );
				}
			}
		}
		else
			OnKeyBuyDown();
	}
	else if( Key == FKeySell && FBetterEnable ) ///< Place order @better Px
	{
		if( FSync == true )
		{
			register TOrderBookList* Dapth;

			if( FInvertBS == true )
				OnKeyBuyDown();
			else
				OnKeySellDown();
			for( register i = 0; i< FSyncDepth.ItemCount();i++)
			{
				if( (Dapth = FSyncDepth[i]) != this )
				{
					if( Dapth->FInvertBS == FInvertBS )
						Dapth->OnKeySellDown( );
					else
						Dapth->OnKeyBuyDown( );
				}
			}
		}
		else
			OnKeySellDown();
	}
	else if( Key == FKeyBetterIncrese && FBetterEnable )  ///< Increase better price
	{
		int NewBetter = FBetterStep + 1;

		if( FSync == true )
		{
			for( register i = 0; i< FSyncDepth.ItemCount();i++)
				 FSyncDepth[i]->ChangeBetterValue( NewBetter );
		}
		else
			ChangeBetterValue( NewBetter );

	}
	else if( Key == FKeyBetterDecrese && FBetterEnable ) ///< Decrease better price
	{
		int NewBetter = FBetterStep - 1;

		if( FSync == true )
		{
			for( register i = 0; i< FSyncDepth.ItemCount();i++)
				 FSyncDepth[i]->ChangeBetterValue( NewBetter );
		}
		else
			ChangeBetterValue( NewBetter );
	}
	else if( Key == FKeyBuyMarket && FOnNewMarketOrder != NULL ) ///< Place market buy order
	{
		if( FSync == true )
		{
			register TOrderBookList* Dapth;

			if( FInvertBS == true )
				FOnNewMarketOrder( this, nsOrderMessageDefine::sSell, FClickQty );
			else
				FOnNewMarketOrder( this, nsOrderMessageDefine::sBuy, FClickQty );
			for( register i = 0; i< FSyncDepth.ItemCount();i++)
			{
				if( (Dapth = FSyncDepth[i]) != this )
				{
					if( Dapth->FInvertBS == FInvertBS )
						Dapth->FOnNewMarketOrder( Dapth, nsOrderMessageDefine::sBuy, Dapth->FClickQty );
					else
						Dapth->FOnNewMarketOrder( Dapth, nsOrderMessageDefine::sSell, Dapth->FClickQty );
				}
			}
		}
		else
			FOnNewMarketOrder( this, nsOrderMessageDefine::sBuy, FClickQty );
	}
	else if( Key == FKeySellMarket && FOnNewMarketOrder != NULL ) ///< Place market Sell order
	{
		if( FSync == true )
		{
			register TOrderBookList* Dapth;

			if( FInvertBS == true )
				FOnNewMarketOrder( this, nsOrderMessageDefine::sBuy, FClickQty );
			else
				FOnNewMarketOrder( this, nsOrderMessageDefine::sSell, FClickQty );
			for( register i = 0; i< FSyncDepth.ItemCount();i++)
			{
				if( (Dapth = FSyncDepth[i]) != this )
				{
					if( Dapth->FInvertBS == FInvertBS )
						Dapth->FOnNewMarketOrder( Dapth, nsOrderMessageDefine::sSell, Dapth->FClickQty );
					else
						Dapth->FOnNewMarketOrder( Dapth, nsOrderMessageDefine::sBuy, Dapth->FClickQty );
				}
			}
		}
		else
			FOnNewMarketOrder( this, nsOrderMessageDefine::sSell, FClickQty );
	}
	else if( Key == FKeyCancelBuy  )  ///< Cancel all buy orders
	{
		if( FSync == true )
		{
			for( register i = 0; i< FSyncDepth.ItemCount();i++)
				 FSyncDepth[i]->TriggerCancelAllBuyOrders( );
		}
		else
			TriggerCancelAllBuyOrders();
	}
	else if( Key == FKeyCancelSell )  ///< Cancel all sell orders
	{
		if( FSync == true )
		{
			for( register i = 0; i< FSyncDepth.ItemCount();i++)
				 FSyncDepth[i]->TriggerCancelAllSellOrders( );
		}
		else
			TriggerCancelAllSellOrders();
	}
	else if( Key == FKeyNewOrder )   ///< Place new order
		TriggerNewOrder();
	else if( Key == FKeyCancelAll ) ///< Cancel all orders
	{
		if( FSync == true )
		{
			for( register i = 0; i< FSyncDepth.ItemCount();i++)
			{
				FSyncDepth[i]->TriggerCancelAllBuyOrders();
				FSyncDepth[i]->TriggerCancelAllSellOrders( );
			}
		}
		else
		{
			TriggerCancelAllBuyOrders();
			TriggerCancelAllSellOrders();
		}
	}
	else if( Key == FKeyCenterFillPx ) ///< Center fill Price
	{
		CenterFillPx( true );
		if( FOnCenterPx )
			OnCenterPx( this );
	}
	else
		 TCustomGrid::KeyDown( Key, Shift );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DeleteBuyOrders( bool sync )
{
	if( sync  == false )
		TriggerCancelAllBuyOrders();
	else
		CancelAll( csBuy );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DeleteSellOrders( bool sync )
{
	if( sync  == false )
		TriggerCancelAllSellOrders();
	else
		CancelAll( csSell );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DeleteAllOrders( bool sync )
{
	if( sync  == false )
	{
		TriggerCancelAllBuyOrders();
		TriggerCancelAllSellOrders();
	}
	else
		CancelAll( );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::CancelAll( TCancelSide Side )
{
	int CxlCount = 0;
	int StopCancelCount = -1;
	int CancelCount;

	if( FOnAskCancelCount != NULL )
	{
		FOnAskCancelCount( this, StopCancelCount );
		switch( Side )
		{
			case csBoth:CxlCount += TriggerCancelAllBuyOrders();
						CxlCount += TriggerCancelAllSellOrders();
						break;
			case csBuy: CxlCount += TriggerCancelAllBuyOrders();
						break;
			case csSell:CxlCount += TriggerCancelAllSellOrders();
						break;
		}
		if( CxlCount > 0 )
		{
			StopCancelCount += CxlCount;
			for( int i = 0; i < CxlCount*50; i++ )
			{
				UFC::SleepMS( 10 );
				Application->ProcessMessages();
				FOnAskCancelCount( this, CancelCount );
				if( CancelCount >= StopCancelCount )
					return;
			}
		}
	}
	else
	{
		switch( Side )
		{
			case csBoth:CxlCount += TriggerCancelAllBuyOrders();
						CxlCount += TriggerCancelAllSellOrders();
						break;
			case csBuy: CxlCount += TriggerCancelAllBuyOrders();
						break;
			case csSell:CxlCount += TriggerCancelAllSellOrders();
						break;
		}
		if( FClxAllDelayMS > 0 && CxlCount > 0  )
		{
			int WaitMS = (FClxAllDelayMS*CxlCount)/4;
			if( WaitMS < FClxAllDelayMS )
				WaitMS = FClxAllDelayMS;
			UFC::SleepMS( WaitMS );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::OnKeyBuyDown( void )
{
	if( FOnNewOrder != NULL )
	{
		int BuyIndex = FFillRowIndex + FBetterStep;

		if( FCancelBeforeNew == true )
			CancelAll();

		double Px = GetPxFromIndex( BuyIndex );
		if( FStepEnable )
			PlaceStepNewOrder( nsOrderMessageDefine::sBuy, BuyIndex, FClickQty );
		else
			FOnNewOrder( this, nsOrderMessageDefine::sBuy, Px, FClickQty );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::OnKeySellDown( void )
{
	if( FOnNewOrder != NULL )
	{
		int SellIndex = FFillRowIndex - FBetterStep;

		if( FCancelBeforeNew == true )
			CancelAll();
		double Px = GetPxFromIndex( SellIndex );
		if( FStepEnable )
			PlaceStepNewOrder( nsOrderMessageDefine::sSell, SellIndex, FClickQty );
		else
			FOnNewOrder( this, nsOrderMessageDefine::sSell, Px, FClickQty );
	}
}
//---------------------------------------------------------------------------
int __fastcall TOrderBookList::TriggerCancelAllBuyOrders( void )
{
	int CCount = 0;
	if( FOnDeleteAllOrders != NULL )
	{
		TDoubleDynArray PriceArray;
		PriceArray.Length = 0;

		for( register int i = 2; i < FUserBuyQty.Length; i++ )
		{

			if( FUserBuyQty[i] > 0 )
			{
				PriceArray.Length += 1;

				double Price = GetPxFromIndex( i  );
				PriceArray[ PriceArray.Length - 1 ] = Price;
			}
		}
		if( PriceArray.Length > 0 )
			FOnDeleteAllOrders( this, nsOrderMessageDefine::sBuy, PriceArray, CCount );
		PriceArray.Length = 0;  ///< Free dynamic double array's memory
	}
	return CCount;
}
//---------------------------------------------------------------------------
int __fastcall TOrderBookList::TriggerCancelAllSellOrders( void )
{
	int CCount = 0;
	if( FOnDeleteAllOrders != NULL )
	{
		TDoubleDynArray PriceArray;
		PriceArray.Length = 0;

		for( register int i = 2; i < FUserSellQty.Length; i++ )
		{
			if( FUserSellQty[i] > 0 )
			{
				PriceArray.Length += 1;
				double Price = GetPxFromIndex(i);
				PriceArray[ PriceArray.Length - 1 ] = Price;
			}
		}
		if( PriceArray.Length > 0 )
			FOnDeleteAllOrders( this, nsOrderMessageDefine::sSell, PriceArray, CCount );
		PriceArray.Length = 0;  ///< Free dynamic double array's memory
	}
	return CCount;
}
//------------------------------------------------------------------------------
void __fastcall TOrderBookList::PlaceStepNewOrder( nsOrderMessageDefine::SideEnum Side, int BasePxIndex, int Qty )
{
	TDoubleDynArray PriceArray;
	int             BetterIndex = BasePxIndex;

	PriceArray.Length = 0;
	if( Side == nsOrderMessageDefine::sBuy )
	{
		for( register int i = 0; i <= FStepCount; i++ )
		{
			PriceArray.Length += 1;
			PriceArray[ PriceArray.Length - 1 ] = GetPxFromIndex( BetterIndex );
			BetterIndex += FStep;
		}
	}
	else
	{
		for( register int i = 0; i <= FStepCount; i++ )
		{
			PriceArray.Length += 1;
			PriceArray[ PriceArray.Length - 1 ] = GetPxFromIndex( BetterIndex );
			BetterIndex -= FStep;
		}
	}
	FOnNewOrders( this, Side, PriceArray, Qty );
	PriceArray.Length = 0;
}
//------------------------------------------------------------------------------
void __fastcall TOrderBookList::TriggerNewOrder( void )
{
	TPoint     Pos   = ScreenToClient( Mouse->CursorPos );
	TGridCoord Coord = MouseCoord( Pos.x, Pos.y );

	if( Coord.Y  > 1 && FOnNewOrder != NULL  )///< Click header
	{
		switch( Coord.X )
		{
			case BUY_MK_COL:
			case BUY_ORDER_BTN_COL:
				if( ( IsCompact == false && Coord.X == BUY_ORDER_BTN_COL ) ||
					( IsCompact == true && Coord.X == BUY_MK_COL ) )
				{
					if( FCancelBeforeNew == true )
						CancelAll();
					if( FStepEnable )
						PlaceStepNewOrder( nsOrderMessageDefine::sBuy, Coord.Y, FClickQty );
					else
						FOnNewOrder( this, nsOrderMessageDefine::sBuy, GetPxFromIndex( Coord.Y ), FClickQty );
				}
				break;
			case SELL_MK_COL:
			case SELL_ORDER_BTN_COL:
				if( ( IsCompact == false && Coord.X == SELL_ORDER_BTN_COL ) ||
					( IsCompact == true && Coord.X == SELL_MK_COL ) )
				{
					if( FCancelBeforeNew == true )
						CancelAll();
					if( FStepEnable )
						PlaceStepNewOrder( nsOrderMessageDefine::sSell,  Coord.Y, FClickQty );
					else
						FOnNewOrder( this, nsOrderMessageDefine::sSell, GetPxFromIndex( Coord.Y ), FClickQty );
				}
				break;
			case BUY_DEL_BTN_COL:
			case BUY_ORDER_COL:
			case SELL_DEL_BTN_COL:
			case SELL_ORDER_COL:
			case BUY_CONDITION_COL:
			case SELL_CONDITION_COL:
			default: break;
		}
	}

}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DeleteAllStopOrder( nsOrderMessageDefine::SideEnum Side )
{
	if( Side == nsOrderMessageDefine::sBuy )
	{
		if( FOnDeleteStopOrder != NULL )
		{
			for( register int i = 0; i < FBuyConditionQty.Length; i++ )
			{
				if( FBuyConditionQty[i] > 0 )
					FOnDeleteStopOrder( this, nsOrderMessageDefine::sBuy, GetPxFromIndex(i) );
			}
		}

	}
	else
	{
		if( FOnDeleteStopOrder != NULL )
		{
			for( register int i = 0; i < FSellConditionQty.Length; i++ )
			{
				if( FSellConditionQty[i] > 0 )
					FOnDeleteStopOrder( this, nsOrderMessageDefine::sSell, GetPxFromIndex(i) );
			}

		}
	}
}
//---------------------------------------------------------------------------
double __fastcall TOrderBookList::GetStopExecutePrice( nsOrderMessageDefine::SideEnum side, int PxIndex )
{
	double OrderPx;

	if( side == sBuy )
	{
		if( FBuyStopOrderOrdType != nsOrderMessageDefine::otMarket )
		{
			int Tick = PxIndex - FBuyStopTick;
			if( Tick >= 0 || Tick < FTickList->ItemCount() )
				OrderPx = GetPxFromIndex( Tick );
			else
				OrderPx = GetPxFromIndex( PxIndex );
		}
		else
			OrderPx = 0;
	}
	else
	{
		if( FSellStopOrderOrdType != nsOrderMessageDefine::otMarket )
		{
			int Tick = PxIndex - FSellStopTick;
			if( Tick >= 0 || Tick < FTickList->ItemCount() )
				OrderPx = GetPxFromIndex( Tick );
			else
				OrderPx = GetPxFromIndex( PxIndex );
		}
		else
			OrderPx = 0;
	}
	return OrderPx;
}
//---------------------------------------------------------------------------
double __fastcall TOrderBookList::GetStopExecutePrice( nsOrderMessageDefine::SideEnum side, double Px )
{
	int	PxIndex = GetRowIndex( Px );
	return GetStopExecutePrice( side, PxIndex );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::ConditionOrderColMouseDown( Classes::TShiftState Shift, int X, int Y )
{
	nsOrderMessageDefine::SideEnum Side;

	if( X == BUY_CONDITION_COL && Y >= 1 )
	{
		Side = nsOrderMessageDefine::sBuy;
		if( FOnDeleteStopOrder != NULL && Shift.Contains( ssRight ) )
		{
			if( FCancelByRightClick == false)
				return;
			if( Y == 1 )
				DeleteAllStopOrder( nsOrderMessageDefine::sBuy );
			else
				FOnDeleteStopOrder( this, Side, GetPxFromIndex( Y ) );
			return;
		}
		else
		{
			double Px = GetPxFromIndex( Y );
			double OrderPx;
			if( FBuyStopOrderOrdType == nsOrderMessageDefine::otMarket )
				OrderPx = 0;
			else
			{
				int Tick = Y - FBuyStopTick;
				if( Tick >= 0 || Tick < FTickList->ItemCount() )
					OrderPx = GetPxFromIndex( Tick );
				else
					return;
			}
			if( FOnNewConditionOrder != NULL && Shift.Contains( ssLeft ) )
				FOnNewConditionOrder( this, Side, Px, FBuyStopOrderOrdType, OrderPx );
		}
	}
	else if( X == SELL_CONDITION_COL && Y >= 1 )
	{
		Side = nsOrderMessageDefine::sSell;
		if( FOnDeleteStopOrder != NULL && Shift.Contains( ssRight ) )
		{
            if( FCancelByRightClick == false)
				return;
			if( Y == 1 )
				DeleteAllStopOrder( nsOrderMessageDefine::sSell );
			else
				FOnDeleteStopOrder( this, Side, GetPxFromIndex( Y ) );
			return;
		}
		else
		{
			double Px = GetPxFromIndex( Y );
			double OrderPx;
			if( FSellStopOrderOrdType == nsOrderMessageDefine::otMarket )
				OrderPx = 0;
			else
			{
				int Tick = Y - FSellStopTick;
				if( Tick >= 0 || Tick < FTickList->ItemCount() )
					OrderPx = GetPxFromIndex( Tick );
				else
					return;
			}
			if( FOnNewConditionOrder != NULL )
				FOnNewConditionOrder( this, Side, Px, FSellStopOrderOrdType, OrderPx );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::OCOColLeftMouseDown( Classes::TShiftState Shift, int X, int Y )
{
	if(Y <= 2)
		return;
	if(Y == FFillRowIndex)
		return;

	if(X == BUY_OCO_COL)
		FOnNewOCO(this, nsOrderMessageDefine::sBuy,	GetPxFromIndex( Y ));
	if(X == SELL_OCO_COL)
		FOnNewOCO(this, nsOrderMessageDefine::sSell,GetPxFromIndex( Y ));
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::OCODelColLeftMouseDown( Classes::TShiftState Shift, int X, int Y )
{
	double price = GetPxFromIndex(Y);
	SideEnum side = (X == BUY_OCODEL_COL)? sBuy : sSell;

	if (FIsPairingOCO && FCurrentPairOCO->ConditionPrice1 == price )
	{
		FOCOStore->CancelPairingOCO(
			FExchange.c_str(),
			FSymbol.c_str(),
			side,
			price);
		return;
	}

	FOCOStore->DeleteOCO(
		FExchange.c_str(),
		FSymbol.c_str(),
		side,
		price);
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::OCOColRightMouseDown( Classes::TShiftState Shift, int X, int Y )
{
	if( FCancelByRightClick == false)
		return;
	if(Y < 0)
		return;

	double price = GetPxFromIndex(Y);
	SideEnum side = (X == BUY_OCO_COL)? sBuy : sSell;
	if (FIsPairingOCO && FCurrentPairOCO->ConditionPrice1 == price )
	{
		FOCOStore->CancelPairingOCO(
			FExchange.c_str(),
			FSymbol.c_str(),
			side,
			price);
		return;
	}

	FOCOStore->DeleteOCO(
		FExchange.c_str(),
		FSymbol.c_str(),
		side,
		price);
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DelAllBuyOCO(void)
{
	if(FOCOStore == NULL)
		return;

	for( register int i = 0; i < FBuyOCOQty.Length; i++ )
	{
		if( FBuyOCOQty[i] <= 0 )
			continue;

		double price = GetPxFromIndex(i);
		if( FIsPairingOCO && FCurrentPairOCO->OrderSide1 == nsOrderMessageDefine::sBuy && FCurrentPairOCO->ConditionPrice1 == price )
		{
			FOCOStore->CancelPairingOCO(
				FExchange.c_str(),
				FSymbol.c_str(),
				nsOrderMessageDefine::sBuy,
				price);
			continue;
		}

		FOCOStore->DeleteOCO(
			FExchange.c_str(),
			FSymbol.c_str(),
			nsOrderMessageDefine::sBuy,
			price);
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DelAllSellOCO(void)
{
	if(FOCOStore == NULL)
		return;

	for( register int i = 0; i < FSellOCOQty.Length; i++ )
	{
		if( FSellOCOQty[i] <= 0 )
			continue;

		double price = GetPxFromIndex(i);
		if( FIsPairingOCO && FCurrentPairOCO->OrderSide1 == nsOrderMessageDefine::sSell && FCurrentPairOCO->ConditionPrice1 == price )
		{
			FOCOStore->CancelPairingOCO(
				FExchange.c_str(),
				FSymbol.c_str(),
				nsOrderMessageDefine::sSell,
				price);
			continue;
		}

		FOCOStore->DeleteOCO(
			FExchange.c_str(),
			FSymbol.c_str(),
			nsOrderMessageDefine::sSell,
			price);
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetBuyStopTick( int Tick )
{
	FBuyStopTick = Tick;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetSellStopTick( int Tick )
{
	FSellStopTick = Tick;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetBuyStopOrderType( nsOrderMessageDefine::OrderTypeEnum OrderType )
{
	FBuyStopOrderOrdType = OrderType;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetSellStopOrderType( nsOrderMessageDefine::OrderTypeEnum OrderType )
{
	FSellStopOrderOrdType = OrderType;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DeleteBuyStopOrders( void )
{
	DeleteAllStopOrder( nsOrderMessageDefine::sBuy );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DeleteSellStopOrders( void )
{
	DeleteAllStopOrder( nsOrderMessageDefine::sSell );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::DeleteAllStopOrders( void )
{
	DeleteAllStopOrder( nsOrderMessageDefine::sBuy );
	DeleteAllStopOrder( nsOrderMessageDefine::sSell );
}
//------------------------------------------------------------------------------
void __fastcall TOrderBookList::SetBitmapAlpha( Graphics::TBitmap* RGBABmp, int Alpha  )
{
	register TRGBQuad* CurrentScanLine;

	for( register int i = 0; i < RGBABmp->Height; i ++ ) ///< Apply alpha value to each DIB pixel.
	{
		CurrentScanLine = (TRGBQuad*)RGBABmp->ScanLine[i];
		for( register int j = 0; j < RGBABmp->Width; j ++ )
			 CurrentScanLine[ j ].rgbReserved = Alpha;
	}
}
//------------------------------------------------------------------------------
void __fastcall TOrderBookList::CreateCursor( int HotspotX, int HotspotY, int CancelQty, bool IsBuy )
{
	std::auto_ptr<Graphics::TBitmap> bmpMask( new Graphics::TBitmap );
	std::auto_ptr<Graphics::TBitmap> bmpColor( new Graphics::TBitmap );
	std::auto_ptr<TIconInfo>         iconInfo( new TIconInfo );
	TRect                            CurRect( 0, 0, ColWidths[ BUY_ORDER_COL ], DefaultRowHeight);
	String                           QtyStr( CancelQty );
	TTextFormat                      Formats;

	///< Release old cursor.
	DestroyIcon( Screen->Cursors[5] );
	///< Create mask 1bit image.
	bmpMask->Width  = CurRect.Width();
	bmpMask->Height = CurRect.Height();
	bmpMask->PixelFormat = pf1bit;
	bmpMask->Canvas->Brush->Color = clBlack;
	bmpMask->Canvas->FillRect( CurRect ); ///< All image pass mask.
	///< Create color image
	bmpColor->Width  = CurRect.Width();
	bmpColor->Height = CurRect.Height();
	bmpColor->PixelFormat = pf32bit;   ///< RGBA format
	bmpColor->AlphaFormat = afDefined; ///< Use alpha value.
	bmpColor->Canvas->Pen->Color   = FGridColor;
	bmpColor->Canvas->Font->Assign( FFont );
	if( IsBuy == true )
	{
		bmpColor->Canvas->Font->Color  = FBuyColor;
		bmpColor->Canvas->Brush->Color = FBuyBKColor;
		bmpColor->Canvas->Rectangle( 0, 0, CurRect.Width(), CurRect.Height() );
		bmpColor->Canvas->Brush->Style = bsClear;
	}
	else
	{
		bmpColor->Canvas->Font->Color  = FSellColor;
		bmpColor->Canvas->Brush->Color = FSellBKColor;
		bmpColor->Canvas->Rectangle( 0, 0, CurRect.Width(), CurRect.Height() );
		bmpColor->Canvas->Brush->Style = bsClear;
	}
	Formats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
	bmpColor->Canvas->TextRect( CurRect, QtyStr, Formats );
	SetBitmapAlpha( bmpColor.get(), 148  );
	///< Create cursor icon
	iconInfo->fIcon = false; ///< Cursor
	iconInfo->xHotspot = HotspotX;
	iconInfo->yHotspot = HotspotY;
	iconInfo->hbmMask  = bmpMask->Handle;
	iconInfo->hbmColor = bmpColor->Handle;
	Screen->Cursors[5] = CreateIconIndirect(iconInfo.get());
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::GetPrices( bool IsUp, TDoubleDynArray& PriceArray, int Times, int Step, int Offset  )
{
	int PxIndex = FFillRowIndex;

	if( IsUp == true )
		PxIndex = FFillRowIndex - Offset;
	else
		PxIndex = FFillRowIndex + Offset;

	PriceArray.Length = 0;
	for( register int i = 0; i < Times; i++ )
	{
		PriceArray.Length += 1;
		if( IsUp == true )
			PxIndex -= Step;
		else
			PxIndex += Step;
		PriceArray[ PriceArray.Length - 1 ] = GetPxFromIndex( PxIndex );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::MarkPrice( int PxIndex, bool IsMark1  )
{
	if( IsMark1 == true )
	{
		if( FMarkPxIndex == -1 ) ///< New mark
		{
			FMarkPxIndex = PxIndex;
			FMarkPx      = GetPxFromIndex( PxIndex );
			FMarkPxAlarm = true;
			if( FMarkPx > FLastFillPx )
				FMarkPxHigher = true;  ///< Marked price higher than last price.
			else
				FMarkPxHigher = false;
			InvalidateCellRect( PRICE_COL, FMarkPxIndex );
		}
		else if(FMarkPxIndex == PxIndex ) ///< Un Mark
		{
			FMarkPxIndex = -1;
			FMarkPxAlarm = false;
			InvalidateCellRect( PRICE_COL, PxIndex );
		}
		else ///< Mark to other price.
		{
			FMarkPxIndex = PxIndex;
			FMarkPx      = GetPxFromIndex( PxIndex );
			FMarkPxAlarm = true;
			if( FMarkPx > FLastFillPx )
				FMarkPxHigher = true;  ///< Marked price higher than last price.
			else
				FMarkPxHigher = false;
			InvalidateColumn( PRICE_COL,  TopRow , TopRow + VisibleRowCount );///< Buy col
		}
	}
	else
	{
		if( FMarkPx2Index == -1 ) ///< New mark
		{
			FMarkPx2Index = PxIndex;
			FMarkPx2      = GetPxFromIndex( PxIndex );
			FMarkPx2Alarm = true;
			if( FMarkPx2 > FLastFillPx )
				FMarkPx2Higher = true;  ///< Marked price higher than last price.
			else
				FMarkPx2Higher = false;
			InvalidateCellRect( PRICE_COL, FMarkPx2Index );
		}
		else if(FMarkPx2Index == PxIndex ) ///< Un Mark
		{
			FMarkPx2Index = -1;
			FMarkPx2Alarm = false;
			InvalidateCellRect( PRICE_COL, PxIndex );
		}
		else ///< Mark to other price.
		{
			FMarkPx2Index = PxIndex;
			FMarkPx2      = GetPxFromIndex( PxIndex );
			FMarkPx2Alarm = true;
			if( FMarkPx2 > FLastFillPx )
				FMarkPx2Higher = true;  ///< Marked price higher than last price.
			else
				FMarkPx2Higher = false;
			InvalidateColumn( PRICE_COL,  TopRow , TopRow + VisibleRowCount );///< Buy col
		}
    }
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
{
	TCustomGrid::MouseDown( Button, Shift, X, Y );
	TGridCoord Coord = MouseCoord( X, Y );

	if( Button == mbLeft ) ///< Mouse left button down
	{
		if( Coord.Y <= 0 ) ///< Click Caption
			return;
		if( FEnableHotKey == false ) ///< HotKey disable
		{
			if( FOnUILock != NULL )
				FOnUILock( this );
			return;
		}
		if( Coord.Y == 1 ) ///< Click header, trigger delete all event
		{
			if( ( ( Coord.X == BUY_DEL_BTN_COL && FIsCompact == false ) ||  ( Coord.X == BUY_ORDER_COL && FIsCompact == true ) ) && FOnDeleteAllOrders != NULL )
				TriggerCancelAllBuyOrders();  ///< Delete all buy order
			else if( ( ( Coord.X == SELL_DEL_BTN_COL && FIsCompact == false ) ||  ( Coord.X == SELL_ORDER_COL && FIsCompact == true ) )  && FOnDeleteAllOrders != NULL )
				TriggerCancelAllSellOrders(); ///< Delete all sell order
			else if( Coord.X == BUY_CONDITION_COL && ConditionOrder == TRUE )
				DeleteAllStopOrder( nsOrderMessageDefine::sBuy ); ///< Delete all buy Stop order
			else if( Coord.X == SELL_CONDITION_COL && ConditionOrder == TRUE )
				DeleteAllStopOrder( nsOrderMessageDefine::sSell ); ///< Delete all Sell Stop order
			return;
		}

		int    Qty,CancelQty;
		TRect  CellR;
		double SelPx = GetPxFromIndex( Coord.Y );
		nsOrderMessageDefine::SideEnum side;

		switch( Coord.X )
		{
			case BUY_DEL_BTN_COL:
				Qty = FUserBuyQty[Coord.Y];
				if( Qty > 0 && FOnReduceQty != NULL)
					FOnReduceQty( this, nsOrderMessageDefine::sBuy, SelPx, Qty );
				break;
			case SELL_DEL_BTN_COL:
				Qty = FUserSellQty[Coord.Y];
				if( Qty > 0 && FOnReduceQty != NULL)
					FOnReduceQty( this, nsOrderMessageDefine::sSell, SelPx, Qty );
				break;
			case BUY_MK_COL:
			case BUY_ORDER_BTN_COL:
				side = nsOrderMessageDefine::sBuy;
				if( FOnNewOrder != NULL )
				{
					if( ( IsCompact == false && Coord.X == BUY_ORDER_BTN_COL ) ||
						( IsCompact == true && Coord.X == BUY_MK_COL ) )
					{
						if( FCancelBeforeNew == true )
							CancelAll();
						if( FStepEnable && FOnNewOrders != NULL )
						{
							int BuyBetterIndex = Coord.Y;
							TDoubleDynArray PriceArray;
							PriceArray.Length = 0;
							for( register int i = 0; i <= FStepCount; i++ )
							{
								PriceArray.Length += 1;
								PriceArray[ PriceArray.Length - 1 ] = GetPxFromIndex( BuyBetterIndex );
								BuyBetterIndex += FStep;
							}
							FOnNewOrders( this, nsOrderMessageDefine::sBuy, PriceArray, FClickQty );
							PriceArray.Length = 0;
						}
						else
							FOnNewOrder( this, side, SelPx, FClickQty );
					}
				}
				break;
			case SELL_MK_COL:
			case SELL_ORDER_BTN_COL:
				side = nsOrderMessageDefine::sSell;
				if( FOnNewOrder != NULL )
				{
					if( ( IsCompact == false && Coord.X == SELL_ORDER_BTN_COL ) ||
						( IsCompact == true && Coord.X == SELL_MK_COL ) )
					{
						if( FCancelBeforeNew == true )
							CancelAll();
						if( FStepEnable && FOnNewOrders != NULL )
						{
							int SellBetterIndex = Coord.Y;
							TDoubleDynArray PriceArray;
							PriceArray.Length = 0;
							for( register int i = 0; i <= FStepCount; i++ )
							{
								PriceArray.Length += 1;
								PriceArray[ PriceArray.Length - 1 ] = GetPxFromIndex( SellBetterIndex );
								SellBetterIndex -= FStep;
							}
							FOnNewOrders( this, nsOrderMessageDefine::sSell, PriceArray, FClickQty );
							PriceArray.Length = 0;
						}
						else
							FOnNewOrder( this, side, SelPx, FClickQty );
					}
				}
				break;
			case BUY_CONDITION_COL:
			case SELL_CONDITION_COL: ConditionOrderColMouseDown( Shift, Coord.X, Coord.Y );
				break;
			case BUY_OCO_COL:
			case SELL_OCO_COL: OCOColLeftMouseDown( Shift, Coord.X, Coord.Y );
				break;
			case BUY_OCODEL_COL:
			case SELL_OCODEL_COL: OCODelColLeftMouseDown( Shift, Coord.X, Coord.Y );
				break;
			case PRICE_COL:
							CellR = CellRect( Coord.X, Coord.Y );
							if( X < CellR.Left + CellR.Width()/2 )
								MarkPrice( Coord.Y, true  );
							else
								MarkPrice( Coord.Y, false );
							break;
			default: break;
		}
	}
	else if( Button == mbRight )  ///< Mouse right button down
	{
		if( Coord.X == BUY_ORDER_COL )
		{
			if( FCancelByRightClick == false)
				return;
			int Qty = FUserBuyQty[Coord.Y];
			if( Qty <= 0 || FOnReduceQty == NULL )
				return;

            double SelPx = GetPxFromIndex( Coord.Y );
			FOnReduceQty( this, nsOrderMessageDefine::sBuy, SelPx, Qty );
		}
		else if( Coord.X == SELL_ORDER_COL )
		{
        	if( FCancelByRightClick == false)
				return;
			int Qty = FUserSellQty[Coord.Y];
			if( Qty <= 0 || FOnReduceQty == NULL )
				return;

            double SelPx = GetPxFromIndex( Coord.Y );
			FOnReduceQty( this, nsOrderMessageDefine::sSell, SelPx, Qty );
		}
		else if( Coord.X == BUY_CONDITION_COL || Coord.X == SELL_CONDITION_COL )
			ConditionOrderColMouseDown( Shift, Coord.X, Coord.Y );
		else if( Coord.X == BUY_OCO_COL || Coord.X == SELL_OCO_COL)
			OCOColRightMouseDown( Shift, Coord.X, Coord.Y );
		else
		{
			TShiftState Shift;
			WORD        Key = VK_RBUTTON;

			KeyDown( Key, Shift );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::MouseMove(Classes::TShiftState Shift, int X, int Y)
{
	TGridCoord Coord = MouseCoord( X, Y );

	if( Coord.Y  == 0 )
		return;
	if( Coord.X > 0 && Coord.X < 12 && Coord.X != 6 )
		FNeedCenterFill = false;
	else
		FNeedCenterFill = true;

	TGridCoord OrigCoord = FMouseCoord;
	FMouseCoord = Coord;
	InvalidateCellRect( OrigCoord.X, OrigCoord.Y );
	switch( Coord.X )
	{
		case BUY_ORDER_BTN_COL:
		case SELL_ORDER_BTN_COL:
		case BUY_DEL_BTN_COL:
		case SELL_DEL_BTN_COL:
			MouseOverCell( Coord );
			break;
		default:break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::MouseOverCell( const TGridCoord& Coord )
{
	DrawCell( Coord.X, Coord.Y, CellRect( Coord.X, Coord.Y ), (TGridDrawState)0);
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::Clear( bool IncludeDepth )
{
	if( IncludeDepth == true )
	{
		FDerivedBuyIndex = -1;
		FDerivedSellIndex = -1;
		for( register int i = 0; i < FBuyDepth5Qty.Length; i++ )
			FBuyDepth5Qty[i] = -1;
		for( register int i = 0; i < FSellDepth5Qty.Length; i++ )
			FSellDepth5Qty[i] = -1;
	}
	FTotalBuyQty = 0;
	FTotalSellQty = 0;
	FMarketTotalBuyQty = 0;
	FMarketTotalSellQty = 0;
	FTotalStopBuyQty = 0;
	FTotalStopSellQty = 0;
	for( register int i = 0; i < FUserBuyQty.Length; i++ )
		FUserBuyQty[i] = -1;
	for( register int i = 0; i < FUserSellQty.Length; i++ )
		FUserSellQty[i] = -1;
	for( register int i = 0; i < FBuyFillQty.Length; i++ )
		FBuyFillQty[i] = -1;
	for( register int i = 0; i < FSellFillQty.Length; i++ )
		FSellFillQty[i] = -1;
	for( register int i = 0; i < FBuyConditionQty.Length; i++ )
		FBuyConditionQty[i] = -1;
	for( register int i = 0; i < FSellConditionQty.Length; i++ )
		FSellConditionQty[i] = -1;
	for( register int i = 0; i < MAX_DEPTH; i++ )
	{
		FBuyPxIndex[i] = -1;
		FSellPxIndex[i] = -1;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::ScrollUp( int Tick )
{
	if( TopRow > 2 )
		TopRow -= 1;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::ScrollDown( int Tick )
{
	if( TopRow < RowCount - 1 )
		TopRow += 1;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::MyMouseWheelDown( System::TObject* Sender, Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled )
{
	ScrollDown( 1 );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::MyMouseWheelUp( System::TObject* Sender, Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled )
{
	ScrollUp( 1 );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::MouseLeave( System::TObject* Sender )
{
	TGridCoord OrigCoord = FMouseCoord;
	FMouseCoord.X = -1;
	FMouseCoord.Y = -1;
	FNeedCenterFill = true;
	InvalidateCellRect( OrigCoord.X, OrigCoord.Y );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::MouseEnter(  System::TObject* Sender )
{
	if( ::GetFocus( ) != Handle )
	{
		::SetFocus( Handle );
		if( FOnFocus != NULL )
            FOnFocus( this );
	}
}
//---------------------------------------------------------------------------
bool __fastcall TOrderBookList::UpdateDepth( OrderBookData* Msg )
{
	unsigned int Now  = UFC::GetTickCountMS();
	unsigned int Diff = Now - FLastUpdateFill;
	bool NeedPaint;

	if( IsLoaded == false )
		return false;
	///< Needs update ?
	if( Diff >= FUpdateMinMS )
	{
		FLastUpdateDepth = Now;
		NeedPaint = true;
		FDirtyDepth = false;
	}
	else
	{
		NeedPaint = false;
		FDirtyDepth = true;
	}
	///< Clear old depth data
	for( register int i = 0; i < DEPTH_COUNT; i++ )
	{
		if( FBuyPxIndex[i] >= 0 && FBuyPxIndex[i] < FBuyDepth5Qty.Length )
			FBuyDepth5Qty[FBuyPxIndex[i]] = -1;
		if( FSellPxIndex[i] >= 0 && FSellPxIndex[i] < FSellDepth5Qty.Length )
			FSellDepth5Qty[FSellPxIndex[i]] = -1;
	}
	if( FDerivedBuyQty > 0 )
		FDerivedBuyQty = -1;
	if( FDerivedSellQty > 0 )
		FDerivedSellQty = -1;
	///< Update new depth data.
	for( register int i = 0; i < DEPTH_COUNT; i++ )
	{
		if( Msg->GetBuyDepth( i + 1, FBuyPx[i], FBuyQty[i] ) )
		{
			if( FBuyQty[i] > 0 )
			{
				FBuyPxIndex[i] = GetRowIndex( FBuyPx[i] );
				if( FBuyPxIndex[i] > 0 && FBuyPxIndex[i] < FBuyDepth5Qty.Length )
					FBuyDepth5Qty[FBuyPxIndex[i]] = FBuyQty[i];
			}
		}
		else
			FBuyPxIndex[i] = -1;
		if( Msg->GetSellDepth( i + 1 , FSellPx[i], FSellQty[i] ) )
		{
			if( FSellQty[i] > 0 )
			{
				FSellPxIndex[i] = GetRowIndex( FSellPx[i] );
				if( FSellPxIndex[i] > 0  && FSellPxIndex[i] < FSellDepth5Qty.Length )
					FSellDepth5Qty[FSellPxIndex[i]] = FSellQty[i];
			}
		}
		else
			FSellPxIndex[i ] = -1;
	}
	///< Derived price.
	if( Msg->GetDerivedFlag() )
	{
		FDerivedBuyPx  = Msg->GetDerivedBuyPrice();
		FDerivedSellPx = Msg->GetDerivedSellPrice();
		FDerivedBuyQty  = Msg->GetDerivedBuyQty();
		FDerivedSellQty = Msg->GetDerivedSellQty();
		FDerivedBuyIndex  = GetRowIndex( FDerivedBuyPx );
		FDerivedSellIndex = GetRowIndex( FDerivedSellPx );
	}
	else
	{
		FDerivedBuyQty = -1;
		FDerivedSellQty = -1;
	}
	///< Buy/Sell depth total qty.
	FMarketTotalSellQty = 0;
	FMarketTotalBuyQty  = 0;
	for( register int i = 0; i < DEPTH_COUNT; i++ )
	{
		if( FBuyPxIndex[i] >= 0 && FBuyPxIndex[i] < FBuyDepth5Qty.Length )
			FMarketTotalBuyQty += FBuyDepth5Qty[FBuyPxIndex[i]];
		if( FSellPxIndex[i] >= 0 && FSellPxIndex[i] < FSellDepth5Qty.Length )
			FMarketTotalSellQty += FSellDepth5Qty[FSellPxIndex[i]];
	}
	if( FShowDerivedPx == true )
	{
		if( FDerivedBuyQty >= 0 && FShowDerivedPx == true )
			FMarketTotalBuyQty += FDerivedBuyQty;
		if( FDerivedSellQty > 0 && FShowDerivedPx == true )
			FMarketTotalSellQty += FDerivedSellQty;
	}
	///< update UI
	if( NeedPaint == true )
	{
		InvalidateColumn( BUY_MK_COL,  TopRow , TopRow + VisibleRowCount );///< Buy col
		InvalidateColumn( SELL_MK_COL, TopRow , TopRow + VisibleRowCount );///< Sell col
		InvalidateCellRect( BUY_MK_COL,  1 ); ///< Total Buy cell
		InvalidateCellRect( SELL_MK_COL, 1 ); ///< Total Sell cell
		InvalidateCellRect( PRICE_COL,   1 ); ///< Buy/Sell Rate cell
	}
	else
	{
		if( FTimer->Enabled == false )
			FTimer->Enabled = true;
    }
	return NeedPaint;
}
//---------------------------------------------------------------------------
bool __fastcall TOrderBookList::UpdateFill( void )
{
	unsigned int Now = UFC::GetTickCountMS();
	unsigned int Diff = Now - FLastUpdateFill;
	bool NeedPaint;

	if( Diff >= FUpdateMinMS )
	{
		FLastUpdateFill = Now;
		NeedPaint = true;
		FDirtyFill = false;
	}
	else
	{
		NeedPaint = false;
		FDirtyFill = true;
		if( FTimer->Enabled == false )
			FTimer->Enabled = true;
	}
	if( FLastFillPx != FFillPx ) ///< Fill Price changed.
	{
		FFillPx       = FLastFillPx;
		FFillRowIndex = GetRowIndex( FLastFillPx );
		if( FCenterFillPrice == true )
			CenterFillPx();
		///< Update the price column.
		if( NeedPaint == true )
			InvalidateColumn( PRICE_COL, TopRow , TopRow + VisibleRowCount );
	}
	else ///< Fill Price not chinged. update only one cell
	{
		if( NeedPaint == true )
			InvalidateCell(  PRICE_COL,  FFillRowIndex );
	}
	return NeedPaint;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::OnUpdateTimer( System::TObject* Sender )
{
	bool Update = false;

	FTimer->Enabled = false;
	if( FDirtyFill == true )
	{
		FDirtyFill = false;
		FLastUpdateFill = UFC::GetTickCountMS();
		InvalidateColumn( PRICE_COL, TopRow , TopRow + VisibleRowCount ); ///< Price col
		Update = true;
	}
	if( FDirtyDepth == true )
	{
		FDirtyDepth = false;
		FLastUpdateDepth = UFC::GetTickCountMS();
		InvalidateColumn( BUY_MK_COL,  TopRow , TopRow + VisibleRowCount );///< Buy col
		InvalidateColumn( SELL_MK_COL, TopRow , TopRow + VisibleRowCount );///< Sell col
		InvalidateCellRect( BUY_MK_COL,  1 ); ///< Total Buy cell
		InvalidateCellRect( SELL_MK_COL, 1 ); ///< Total Sell cell
		InvalidateCellRect( PRICE_COL,   1 ); ///< Buy/Sell Rate cell
		Update = true;
	}
	if( Update == true )
		::UpdateWindow( Handle );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::ForceUpdateFill( void )
{
	FFillPx       = FLastFillPx;
	FFillRowIndex = GetRowIndex( FLastFillPx );
	///< Force center price
	CenterFillPx( true );
	///< Update the price column.
	InvalidateColumn( PRICE_COL, TopRow , TopRow + VisibleRowCount );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::UpdateTimer( const UFC::AnsiString& Time )
{
	int hh,mm,ss;

	if( Time.Length() >= 6 )
	{
		hh = UFC::AnsiString::StrToInt32( Time.c_str()    , 2 );
		if( Time[2] == ':' )
		{
			mm = UFC::AnsiString::StrToInt32( Time.c_str() + 3, 2 );
			ss = UFC::AnsiString::StrToInt32( Time.c_str() + 6, 2 );
		}
		else
		{
			mm = UFC::AnsiString::StrToInt32( Time.c_str() + 2, 2 );
			ss = UFC::AnsiString::StrToInt32( Time.c_str() + 4, 2 );
		}
		if( hh > 23 || mm > 59 || ss > 59 )
		{
			return;
		}
		if( mm != FMM || ss != FSS )
		{
			FHH = hh;
			FMM = mm;
			FSS = ss;
			InvalidateCellRect( PRICE_COL, 0 );
		}
	}
}
//---------------------------------------------------------------------------
// Update Fill snapshot
//---------------------------------------------------------------------------
void TOrderBookList::OnMarketDataUpdate( MarketDataMessage* Msg )
{
	double NewPrice    = Msg->GetTradePrice();
	Msg->GetSymbol();
	Msg->GetExchange();
	if( NewPrice <= FBullPx && NewPrice >= FBearPx )
	{
		///< Update Filled
		FFillQty      = Msg->GetTradeQty();
		FLastFillPx	  = NewPrice;
		FDayH         = Msg->GetDayHighPrice();
		FDayL         = Msg->GetDayLowPrice();
		if( FFillQty > FMaxFillQty )
		{
			FMaxFillQty = FFillQty;
			CalSize();
		}
		UpdateTimer( Msg->GetTradeTime() );
		///< Update Depth
		if(  UpdateFill() == true || UpdateDepth( Msg ) == true )
			::UpdateWindow( Handle );
		CenterPx( true );
	}
}
//---------------------------------------------------------------------------
// Update Fill ticks
//---------------------------------------------------------------------------
void TOrderBookList::OnMarketDataUpdate( MatchInfo* Msg )
{
	double NewPrice = Msg->GetMatchPrice();

	if( NewPrice <= FBullPx && NewPrice >= FBearPx )
	{
		FFillQty    = Msg->GetMatchQty();
		FLastFillPx = NewPrice;
		if( FFillQty > FMaxFillQty )
		{
			FMaxFillQty = FFillQty;
			CalSize();
		}
		if( FUpdateEveryFill == true )
		{
			if( UpdateFill() == true )
				::UpdateWindow( Handle );
		}
		if( FOnTick != NULL )
		{
			AnsiString Time( Msg->GetMatchTime().c_str() );
			FOnTick( this, Time, Msg->GetMatchPrice(), Msg->GetMatchQty() );
		}
		PriceAlarm( FMarkPxIndex, FMarkPxAlarm, FMarkPxHigher, FMarkPx );
		PriceAlarm( FMarkPx2Index, FMarkPx2Alarm, FMarkPx2Higher, FMarkPx2 );
	}
}
//---------------------------------------------------------------------------
void TOrderBookList::PriceAlarm( int MarkPxIndex, bool& MarkPxAlarm, bool MarkPxHigher, double MarkPx )
{
		if( MarkPxIndex != -1 && MarkPxAlarm == true )
		{
			if( MarkPxHigher == true )
			{
				if( FLastFillPx >= MarkPx )
				{
					MarkPxAlarm = false;
					if( FOnHigherPxAlarm != NULL )
						FOnHigherPxAlarm( this );
				}
			}
			else
			{
				if( FLastFillPx <= MarkPx )
				{
					MarkPxAlarm = false;
					if( FOnLowerPxAlarm != NULL )
						FOnLowerPxAlarm( this );
				}
			}
		}
}
//---------------------------------------------------------------------------
// Update Depth info
//---------------------------------------------------------------------------
void TOrderBookList::OnMarketDataUpdate( OrderBookData* Msg )
{
	UpdateFill( );
	UpdateTimer( Msg->GetMessageTime() );
	if( UpdateDepth( Msg ) == true )
		::UpdateWindow( Handle );
}
//---------------------------------------------------------------------------
void TOrderBookList::OnMarketDataUpdate( DayHighLowPrice* Msg )
{
	FDayH = Msg->GetDayHighPrice();
	FDayL = Msg->GetDayLowPrice();
}
//---------------------------------------------------------------------------
void TOrderBookList::OnMarketDataUpdate( BasicData* Msg ) {}
//---------------------------------------------------------------------------
void TOrderBookList::OnMarketDataUpdate( TotalMatch* Msg ){}
//---------------------------------------------------------------------------
void TOrderBookList::OnMarketDataUpdate( OpeningInfo* Msg ) {}
//---------------------------------------------------------------------------
void TOrderBookList::OnMarketDataUpdate( ClosingMarketData* Msg ) {}
//---------------------------------------------------------------------------
void TOrderBookList::OnMarketDataUpdate( UnderlyingIndexInfo* Msg ) {}
//---------------------------------------------------------------------------
void TOrderBookList::OnMarketDataUpdate( SumOfOrderInfo* Msg ) {}
//---------------------------------------------------------------------------
AnsiString TOrderBookList::GetEx( void )
{
	return FExchange.c_str();
}
//---------------------------------------------------------------------------
AnsiString TOrderBookList::GetSymbol( void )
{
	return FSymbol.c_str();
}
//---------------------------------------------------------------------------
void TOrderBookList::OnOCOOrderUpdate(TOCOPair* pair, OCOUpdateType Type)
{
	if(pair == NULL)
		return;

	if(Type == OCOUpdateType::New)
	{
		//Is pairing OCO
		if(pair->State == OCOState::ocoNone)
		{
			FIsPairingOCO = true;
			FCurrentPairOCO = pair;
			int col = (pair->OrderSide1 == SideEnum::sBuy)? BUY_OCO_COL : SELL_OCO_COL ;
			InvalidateCellRect( col, GetRowIndex(pair->ConditionPrice1));
			return;
		}

		if(pair->State != OCOState::Pending)
			return;

		// Set OCO data to qty array
		SetOCOQtyArrayByPair(pair, true);
	}
	else if(Type == OCOUpdateType::Edit)
	{
		// Is pairing OCO
		if(	pair == FCurrentPairOCO)
		{
			FIsPairingOCO = false;
			FCurrentPairOCO = NULL;
			//From pairing OCO to pending OCO
			if(pair->State == OCOState::Pending)
			{
				SetOCOQtyArrayByPair(pair, true);
			}
			return;
		}
		// If OCO is form pending to triggered or canceled
		if(	pair->State == OCOState::Triggered 	||
			pair->State == OCOState::Canceled	||
			pair->State == OCOState::Failed)
		{
			SetOCOQtyArrayByPair(pair, false);
		}
	}
	else if(Type == OCOUpdateType::Delete)
	{
		if(pair == FCurrentPairOCO)
		{
            FIsPairingOCO = false;
			FCurrentPairOCO = NULL;
			int col = (pair->OrderSide1 == SideEnum::sBuy)? BUY_OCO_COL : SELL_OCO_COL ;
			InvalidateCellRect( col, GetRowIndex(pair->ConditionPrice1));
			return;
		}
		if(pair->State != OCOState::Pending)
			return;

		SetOCOQtyArrayByPair(pair, false);
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::Subscribe( TCMarketDataStore* Store )
{
	if( Store != NULL && FStore != Store  )
		FStore = Store;

	if( !ComponentState.Contains( csDesigning ) && IsLoaded && FStore != NULL  )
	{
		UFC::AnsiString UnsubEx  = FSubscribeExchange.c_str();
		UFC::AnsiString UnsubSym = FSubscribeSymbol.c_str();
		UFC::AnsiString SubEx  = FExchange.c_str();
		UFC::AnsiString SubSym = FSymbol.c_str();

		FStore->Unsubscribe( UnsubEx, UnsubSym, this );
		FStore->Subscribe( SubEx, SubSym, this );
		FSubscribeExchange = FExchange;
		FSubscribeSymbol   = FSymbol;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SubscribeOCOStore(void)
{
	if(FOCOStore == NULL)
		return;

	for( register int i = 0; i < RowCount; i++ )
	{
		FBuyOCOQty[i] = 0;
		FSellOCOQty[i] = 0;
	}

	FTotalOCOBuyQty = 0;
	FTotalOCOSellQty= 0;
	FIsPairingOCO = false;
    FCurrentPairOCO = NULL;
	FOCOStore->Subscribe(this);
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::UnsubscribeOCOStore(void)
{
	if(FOCOStore == NULL)
		return;

	FOCOStore->UnSubscribe(this);
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetOCOStore( TOrderStore_OCO* Store )
{
	if(Store == NULL)
		return;

	FOCOStore = Store;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetCenterFillPrice( bool CenterFill )
{
	FCenterFillPrice = CenterFill;
	if( FCenterFillPrice == true && IsLoaded == true )
		CenterFillPx();
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetBetterEnable( bool Enable )
{
	FBetterEnable = Enable;
	if( FBetterEnable == false )
	{
		FBetterStep = 0;
		InvalidateCol( PRICE_COL );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::Unsubscribe( void )
{
	UFC::AnsiString TempExchange( FExchange.c_str());
	UFC::AnsiString TempSymbol( FSymbol.c_str());

	FStore->Unsubscribe( TempExchange, TempSymbol, this );
	UnsubscribeOCOStore();
}
//---------------------------------------------------------------------------
void TOrderBookList::OnFill(int Position, double BuyArvPx, double SellArvPx, int BuyQty, int SellQty,
							int BuyOpenInterestQty, int SellOpenInterestQty,
							DynamicArray<void*>& PosStatisticsArray, int NetPosition, double FloatingProfit )
{
	FBuyAvgFillPx = BuyArvPx;
	FSellAvgFillPx = SellArvPx;
	FBuySideFillQty = BuyQty;
	FSellSideFillQty = SellQty;
	for( register int i = 0; i < PosStatisticsArray.Length; i++ )
	{
		s888::CMatchPriceStatisticRecord *curPxStatisticRecPtr = static_cast<s888::CMatchPriceStatisticRecord*>(PosStatisticsArray[i]);
		curPxStatisticRecPtr->LockForRead();
		double matchPx = curPxStatisticRecPtr->GetMatchPrice(s888::rwNotLock);
		nsOrderMessageDefine::SideEnum side = curPxStatisticRecPtr->GetSide(s888::rwNotLock);
		int matchQuantity = curPxStatisticRecPtr->GetTotalMatchQuantity(s888::rwNotLock);
		curPxStatisticRecPtr->UnlockForRead();
		int Index = GetRowIndex(matchPx);
		if (Index < 0) continue;
		if( side == nsOrderMessageDefine::sBuy )
		{
			FBuyFillQty[Index] = matchQuantity;
			if( FShowFilled == true )
			{
				TRect DrawRect = CellRect( BUY_FILL_COL, Index );
				if( DrawRect.Width() > 0)
					InvalidateCellRect( BUY_FILL_COL, Index );
			}
		}
		else
		{
			FSellFillQty[Index] = matchQuantity;
			if( FShowFilled == true )
			{
				TRect DrawRect = CellRect( SELL_FILL_COL, Index );
				if( DrawRect.Width() > 0)
					InvalidateCellRect( SELL_FILL_COL, Index );
			}
		}
	}
	if( FShowFilled == true )
	{
		InvalidateCellRect( BUY_FILL_COL, 0 );
		InvalidateCellRect( BUY_FILL_COL, 1 );

		TRect FillRect = CellRect( SELL_FILL_COL, 0 );
		if( FillRect.Width() > 0)
			InvalidateCellRect( SELL_FILL_COL, 0 );
		TRect AvgPxRect = CellRect( SELL_FILL_COL, 1 );
		if( AvgPxRect.Width() > 0)
			InvalidateCellRect( SELL_FILL_COL, 1 );
	}
	if( FOnNetPositionUpdate != NULL && FStore != NULL )
	{
		FOnNetPositionUpdate( this, NetPosition, FloatingProfit, BuyOpenInterestQty, SellOpenInterestQty );
		FNetPosition = NetPosition;
	}
}
//---------------------------------------------------------------------------
void TOrderBookList::OnProfit(double AveragePrice, double ProfitAmount)
{
	if( FOnProfitAndAvgPxUpdate != NULL )
		FOnProfitAndAvgPxUpdate( this, ProfitAmount, AveragePrice );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::WndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case WM_ERASEBKGND:
			Msg.Result = 1;
			return;
		case WM_MOUSEWHEEL:
		case WM_KEYDOWN:
			if( SettingMode )
				return;
			break;
		case WM_MDIACTIVATE:
             Msg.Result = 1;
             return;
		case WM_LBUTTONDOWN:
			if( SettingMode )
			{
				POINTS ClickPt = MAKEPOINTS( Msg.LParam );
				TGridCoord Coord = MouseCoord( ClickPt.x, ClickPt.y );
				if( Coord.Y > 9 )
					return;
			}
			if(FOrderByOneClick == false)
				return;
			break;
		case WM_LBUTTONUP:
            if(FOrderByOneClick == false)
				return;
			break;
		case WM_LBUTTONDBLCLK:
			if(FOrderByOneClick == true)
				return;
            break;
		case WM_RBUTTONDOWN:
			if(FOrderByOneClick == false)
				return;
			break;
		case WM_RBUTTONUP:
			if(FOrderByOneClick == false)
				return;
			break;
		case WM_RBUTTONDBLCLK:
            if(FOrderByOneClick == true)
				return;
			break;
	}
	inherited::WndProc( Msg );
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::EnableHotkey( bool Enable )
{
	FEnableHotKey = Enable;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetFPS( int FPS )
{
	if( FPS > 8 && FPS < 100 )
		FMaxFPS = FPS;
	else
		FMaxFPS = 8;
	FUpdateMinMS = 1000/FMaxFPS;
	FTimer->Interval = FUpdateMinMS;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetStopLossTick( int Tick )
{
	if( Tick >= 1 )
		FStopLossTick = Tick;
	else
		FStopLossTick = 1;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetTakeProfitTick( int Tick )
{
	if( Tick >= 1 )
		FTakeProfitTick = Tick;
	else
		FTakeProfitTick = 1;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetStopLossPercent( double Percent )
{
	if( Percent >= 0.01 )
		FStopLossPercent = Percent;
	else
		FStopLossPercent = 0.0;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetTakeProfitPercent( double Percent )
{
	if( Percent >= 0.01 )
		FTakeProfitPercent = Percent;
	else
		FTakeProfitPercent = 0.0;
}
//---------------------------------------------------------------------------
double __fastcall TOrderBookList::GetStopLossPrice( double Px, int OrigTick, nsOrderMessageDefine::SideEnum OrderSide )
{
	int OrderTick;

	if( OrderSide == nsOrderMessageDefine::sBuy )
	{
		if( FStopLossUseTick == true ) ///< Sell price
		{
			int  BearPxTick = FTickList->ItemCount() - 1;

			OrderTick = OrigTick + FStopLossTick;
			if( OrderTick > BearPxTick ) ///< Use Bear Px index
				OrderTick = BearPxTick;
		}
		else
		{
			double OrderPx  = Px - (Px*FStopLossPercent)/100.0;
			OrderTick = FTickList->PxToTickIndex( OrderPx , false );
		}
	}
	else
	{
		if( FStopLossUseTick == true ) ///< Buy price
		{
			OrderTick = OrigTick - FStopLossTick;
			if( OrderTick < 0 ) ///< Use Bull Px index
				OrderTick = 0;
		}
		else
		{
			double OrderPx  = Px + (Px*FStopLossPercent)/100.0;
			OrderTick = FTickList->PxToTickIndex( OrderPx , false );
		}
	}
	return FTickList->GetPxFromIndex( OrderTick );
}
//---------------------------------------------------------------------------
double __fastcall TOrderBookList::GetTakeProfitPrice( double Px, int OrigTick, nsOrderMessageDefine::SideEnum OrderSide )
{
	int OrderTick;

	if( OrderSide == nsOrderMessageDefine::sBuy )
	{
		if( FTakeProfitUseTick == true ) ///< Sell price
		{
			OrderTick = OrigTick - FTakeProfitTick;
			if( OrderTick < 0 ) ///< Use Bull Px index
				OrderTick = 0;
		}
		else
		{
			double OrderPx  = Px + (Px*FTakeProfitPercent)/100.0;
			OrderTick = FTickList->PxToTickIndex( OrderPx , false );
		}
	}
	else
	{
		if( FTakeProfitUseTick == true ) ///< Buy price
		{
			int  BearPxTick = FTickList->ItemCount() - 1;

			OrderTick = OrigTick + FTakeProfitTick;
			if( OrderTick > BearPxTick ) ///< Use Bear Px index
				OrderTick = BearPxTick;
		}
		else
		{
			double OrderPx  = Px - (Px*FTakeProfitPercent)/100.0;
			OrderTick = FTickList->PxToTickIndex( OrderPx , false );
		}
	}
	return FTickList->GetPxFromIndex( OrderTick );
}
//---------------------------------------------------------------------------
bool __fastcall TOrderBookList::CheckStopLoss( int OrigTick )
{
	int BearPxTick = FTickList->ItemCount() - 1;

	if( OrigTick == 0 ||  ///< Bull Price
		OrigTick == BearPxTick || ///< Bear Price
		( FStopLossUseTick == true && FStopLossTick < 1 ) ||      ///< Stop tick < 1
		( FStopLossUseTick == false && FStopLossPercent == 0.0 )) ///< Stop Percent = 0.0
		return false;
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall TOrderBookList::CheckTakeProfit( int OrigTick )
{
	int BearPxTick = FTickList->ItemCount() - 1;

	if( OrigTick == 0 ||  ///< Bull Price
		OrigTick == BearPxTick || ///< Bear Price
		( FTakeProfitUseTick == true && FTakeProfitTick < 1 ) ||      ///< Stop tick < 1
		( FTakeProfitUseTick == false && FTakeProfitPercent == 0.0 )) ///< Stop Percent = 0.0
		return false;
	return true;
}
//---------------------------------------------------------------------------
void TOrderBookList::OnFill(nsOrderMessageDefine::SideEnum Side, double Px, int Qty, int TickCount)
{
	int OrigTick = FTickList->PxToTickIndex( Px, false );

	///< Trigger Stop loss event.
	if( FAutoStopLoss == true && CheckStopLoss( OrigTick ) == true  )
	{
		if( FOnAutoStopLoss != NULL )
		{
			double OrderPx = GetStopLossPrice( Px, OrigTick, Side );
			if( Side == nsOrderMessageDefine::sBuy )
				FOnAutoStopLoss( this, nsOrderMessageDefine::sSell, OrderPx, Qty );
			else
				FOnAutoStopLoss( this, nsOrderMessageDefine::sBuy,  OrderPx, Qty );
		}
	}
	///< Trigger Take Profit event.
	if( FAutoTakeProfit == true && CheckTakeProfit( OrigTick ) == true  )
	{
		if( FOnAutoTakeProfit != NULL )
		{
			double OrderPx = GetTakeProfitPrice( Px, OrigTick, Side );
			if( Side == nsOrderMessageDefine::sBuy )
				FOnAutoTakeProfit( this, nsOrderMessageDefine::sSell, OrderPx, Qty );
			else
				FOnAutoTakeProfit( this, nsOrderMessageDefine::sBuy,  OrderPx, Qty );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetSync( bool IsSync )
{
	if( IsSync != FSync )
	{
		int Index = FSyncDepth.IndexOf( this );

		if( IsSync == true )
		{
			if( Index == -1 )
				FSyncDepth.Add( this );
		}
		else
		{
			if( Index != -1 )
				FSyncDepth.Delete( Index );
		}
		FSync = IsSync;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetUpdateEveryFill( bool UpdateEveryFill )
{
	FUpdateEveryFill = UpdateEveryFill;
}
//---------------------------------------------------------------------------
int __fastcall TOrderBookList::SyncCount( void )
{
	return FSyncDepth.ItemCount();
}
//---------------------------------------------------------------------------
TForm* __fastcall TOrderBookList::GetSyncForm( int i )
{
	TOrderBookList* Depth = FSyncDepth[ i ];
	return dynamic_cast<TForm*>(Depth->Parent);
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::GetSyncFormInfo( int i, String& FormText, bool& Invert )
{
	TOrderBookList* Depth = FSyncDepth[ i ];
	TForm* DepthForm =  dynamic_cast<TForm*>(Depth->Parent);
	FormText = DepthForm->Caption;
	Invert   = Depth->FInvertBS;
}
//---------------------------------------------------------------------------
bool __fastcall TOrderBookList::SetSymbol( const String& Ex, const String& Sym )
{
	if( Store != NULL )
	{
		BasicInformation* SymInfo = Store->GetBasicInformation( Ex, Sym, false );
		if( SymInfo != NULL)
		{
			Unsubscribe();
			Exchange = SymInfo->GetExchange().c_str();
			Symbol   = SymInfo->GetSymbol().c_str();
			FChineseName = SymInfo->GetDisplayName();
			///< Init the depth grid and subscribe market data.
			InitialGrid( SymInfo->GetBullPrice(), SymInfo->GetBearPrice(), SymInfo->GetRefPrice(), SymInfo->GetDigit() );
			CenterFillPx();
			Subscribe();
			SubscribeOCOStore();
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TOrderBookList::HasStopOrder( void )
{
	if( FTotalStopBuyQty > 0 ||  FTotalStopSellQty > 0 )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TOrderBookList::HasWorkingOrder( void )
{
	if( FTotalBuyQty > 0 ||  FTotalSellQty > 0 )
		return true;
	return false;
}
//---------------------------------------------------------------------------
int __fastcall TOrderBookList::GetBidQty( int depth )
{
	if( depth > 0 && depth < DEPTH_COUNT )
		return FBuyQty[ depth ];
	return 0;
}
//---------------------------------------------------------------------------
int __fastcall TOrderBookList::GetAskQty( int depth )
{
	if( depth > 0 && depth < DEPTH_COUNT )
		return FSellQty[ depth ];
	return 0;
}
//---------------------------------------------------------------------------
double __fastcall TOrderBookList::GetBidPrice( int depth )
{
	if( depth >= 0 && depth < DEPTH_COUNT )
		return FBuyPx[ depth ];
	return 0.0;
}
//---------------------------------------------------------------------------
double __fastcall TOrderBookList::GetAskPrice( int depth )
{
	if( depth >= 0 && depth < DEPTH_COUNT )
		return FSellPx[ depth ];
	return 0.0;
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetOCOQtyArrayByPair(TOCOPair* pair, bool isAdd)
{
	const bool havePair = (FOCOPairs.find(pair) != FOCOPairs.end());

	// No-op cases:
	// 1) adding an existing pair
	// 2) removing a non-existing pair
	if (isAdd == havePair)
		return;

	if (isAdd)
		FOCOPairs.insert(pair);
	else
		FOCOPairs.erase(pair);

	SetOCOQtyArray(pair->OrderSide1, GetRowIndex(pair->ConditionPrice1), pair->OrderQty1, isAdd);
	SetOCOQtyArray(pair->OrderSide2, GetRowIndex(pair->ConditionPrice2), pair->OrderQty2, isAdd);
}
//---------------------------------------------------------------------------
void __fastcall TOrderBookList::SetOCOQtyArray(
	nsOrderMessageDefine::SideEnum side,
	int index,
	int Qty,
	bool isAdd)
{
	int sign = isAdd ? 1 : -1;

	if (side == nsOrderMessageDefine::sBuy)
	{
		FBuyOCOQty[index] += sign * Qty;
		FTotalOCOBuyQty   += sign * Qty;
		InvalidateCellRect(BUY_OCO_COL, index);
		InvalidateCellRect(BUY_OCO_COL, 1);
	}
	else if (side == nsOrderMessageDefine::sSell)
	{
        FSellOCOQty[index] += sign * Qty;
        FTotalOCOSellQty   += sign * Qty;
        InvalidateCellRect(SELL_OCO_COL, index);
		InvalidateCellRect(SELL_OCO_COL, 1);
    }
}
//---------------------------------------------------------------------------
double __fastcall TOrderBookList::GetBullPrice( int BetterSellTick )
{
	if( BetterSellTick < FTickList->ItemCount() && BetterSellTick >= 0 )
		return FTickList->GetPxFromIndex( BetterSellTick, false );
	return 0.0;
}
//---------------------------------------------------------------------------
namespace Orderbooklist
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TOrderBookList)};
		RegisterComponents(L"Speedy", classes, 0);
	}
}
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TOrderBookList *)
{
	new TOrderBookList(NULL);
}
//---------------------------------------------------------------------------
