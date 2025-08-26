//------------------------------------------------------------------------------
#include <vcl.h>
#include <System.Math.hpp>
#pragma hdrstop
#include "StockChartEx.h"
#include <d2d1helper.h>
#include "TradingPosition.h"
#pragma package(smart_init)
//------------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//------------------------------------------------------------------------------
static inline void ValidCtrCheck(TStockChartEx *)
{
	new TStockChartEx(NULL);
}
//------------------------------------------------------------------------------
__fastcall TPositionPoint::TPositionPoint(int RefSec, int TenMs, int PxIndex, int Qty, bool IsBuy)
:TObject()
,FRefSec(RefSec)
,F10ms(TenMs)
,FFillTickIndex(PxIndex)
,FQty(Qty)
,FIsBuy(IsBuy)
{
}
//------------------------------------------------------------------------------
// Sort by time
//------------------------------------------------------------------------------
int TCompareTickObject::Compare( void* elem1, void* elem2 )
{
	TTickInfo* Item1 = (TTickInfo*)elem1;
	TTickInfo* Item2 = (TTickInfo*)elem2;
	int Time1 = Item1->RefSec * 100 + Item1->TenMS;
	int Time2 = Item2->RefSec * 100 + Item2->TenMS;

	///< Sort by Time
	if( Time1 < Time2 )
		return -1;
	else if( Time1 > Time2 )
		return 1;
	else ///< Equal time, Compare Unique ID
	{
		if( Item1->ID < Item2->ID )
			return -1;
		else
			return 1;
    }
}
//------------------------------------------------------------------------------
const int FONT_MIN_SIZE    = 6;
const int FONT_DEF_SIZE    = 11;
const int FONT_MAX_SIZE    = 38;
const int PAINT_TIMER_ID   = 100000;
const int THUMB_WIDTH_MAX  = 300;
//------------------------------------------------------------------------------
const int POINT_ARRAY_SIZE      = 1024*4; ///< Can draw up to POINT_ARRAY_SIZE points poly-line.
const int HALF_POINT_ARRAY_SIZE = POINT_ARRAY_SIZE / 2;
const int ENABLE_LOG2_VAL       = 256;
//------------------------------------------------------------------------------
//
// Class TStockChartEx
//
//------------------------------------------------------------------------------
__fastcall TStockChartEx::TStockChartEx(TComponent* Owner)
:TCustomControl(Owner)
,MARKET_MINUTES( 300 )
,BEFORE_OPEN_SEC( 30 )
,AFTER_CLOSED_SEC( 180*4 )
,FSecondsCount( 3600*5 + BEFORE_OPEN_SEC + AFTER_CLOSED_SEC ) ///< Trade time: 5 Hours + 210 sec
,FCloseRefSec(  3600*5 + BEFORE_OPEN_SEC )
,FSecondsPerGrid( 1 )  ///< Seconds per grid.(Min 1 )
,FPixelsPerGrid( 80 )
,FPixelsPerTick( 20 )
,FOpenTimeSec( 3600*8 + 45*60 - BEFORE_OPEN_SEC ) ///< Open at 8:44:30
,FCloseTimeSec( 3600*13 + 45*60 + AFTER_CLOSED_SEC )  ///< Close at 13:48:00
,FOpenHour( 8 )
,FOpenMinutes( 45 )
,FViewPortX( 0 )
,FViewPortY( 0 )
,FLastPaint( 0 )
,FReadOnly( false )
,FMaxFPS( 10 )
,FExchangeInfo( NULL )
,FPaintInterval( 100 )
,FInitSymbol( false )
,FThumbChart( true )
,FShowBubble( false )
,FShowTickLine( true )
,FShowMA( false )
,FBollingerBands( false )
,FEnableLog2( false )
,FBandsWidth( 2.0f )
,FBandsWidth2( 0.0f )
,FBandsWidth3( 0.0f )
,FBollingerSec( 0 )
,FMA1Sec( 0 )
,FMA2Sec( 0 )
,FMA3Sec( 0 )
,FCenterX( -1 )
,FCenterY( -1 )
,FSelectPP( -1 )
,FBuffer( new Graphics::TBitmap() )
,FMapBitmap( new Graphics::TBitmap() )
,FHeaderBitmap( new Graphics::TBitmap() )
,FLastTick( NULL  )
,FStore( NULL )
,FTicks( NULL )
,FTotalUserBuyQty( 0 )
,FTotalUserSellQty( 0 )
,FTotalUserFilledBuyQty(0)
,FTotalUserFilledSellQty(0)
,FTicksList( &FCmpObj, 65536 )
,FMouseDown( false )
,FCancelMouseDown( false )
,FCancelSide( nsOrderMessageDefine::sBuy )
,FCancelTickIndex(0)
,FQtyMax( 50 )
,FFillPxQtyMax(1)
,FCenterFillPx( true )
,FEnableAccelerate( true )
,FThunbDirty( true )
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
,FKeyCenterFillPx(0)
,FOnNewOrder( NULL )
,FOnNewOrders( NULL )
,FOnReduceQty( NULL )
,FOnReplacePx( NULL )
,FOnDeleteAllOrders( NULL )
,FOnNewMarketOrder( NULL )
,FOnProfitAndAvgPxUpdate( NULL )
,FOnNetPositionUpdate( NULL )
,FOnCenterPx( NULL )
,FOnStepChange( NULL )
,FBetterStep( 0 )
,FDebugMode( false )
,FShowFillQty( false )
,FShowSecQty( false )
,FShwStdDev( false )
,FCancelBeforeNew( false )
,FStepEnable( false )
,FStep( 0 )
,FStepCount( 0 )
,FMaxUseMS( 0 )
,FFrameCount( 0 )
,FCurrentFPS( 0 )
,FBeginFPSTick( UFC::GetTickCountMS() )
,FMouseInBtn( 0 )
,FEnablePingPong( false )
,FBuyPingPongTick( 0 )
,FSellPingPongTick( 0 )
,FClxAllDelayMS( 0 )
,FClipLayer( NULL )
,FRectangleGeometry( NULL )
,FCenterBtnClipLayer( NULL )
,FCenterBtnGeometry( NULL )
,FCancelBuyBtnClipLayer( NULL )
,FCancelBuyBtnGeometry( NULL )
,FCancelSellBtnClipLayer( NULL )
,FCancelSellBtnGeometry( NULL )
,FRedGradientBrush( NULL )
,FGreenGradientBrush( NULL )
,FBlueGradientBrush( NULL )
,FWhiteGradientBrush( NULL )
,FBullRadialGradientBrush( NULL )
,FBearRadialGradientBrush( NULL )
,FRefeRadialGradientBrush( NULL )
,FOldWidth( 0 )
,FOldHeight( 0 )
,FFixedRowColor( clWhite )
,FFixedRowBKColor( (TColor) RGB(0,97,171) )
,FGridColor( clGray )
,FTickUpperColor( clRed )
,FTickLowerColor( clLime )
,FTickBKColor( (TColor)0x00763A0A )
,FFillColor( clYellow )
,FFillBKColor( (TColor)RGB(128,128,0) )
,FBetterBuyColor( clRed )
,FBetterBuyBKColor( clMaroon )
,FBetterSellColor( clLime )
,FBetterSellBKColor( clGreen )
,FBidDepth5BKColor( clBlack )
,FBidDepth5Color( clWhite )
,FAskDepth5BKColor( clBlack )
,FAskDepth5Color( clWhite )
,FBuyBKColor( clNavy )
,FBuyColor( clWhite )
,FSellBKColor( clNavy )
,FSellColor( clWhite )
,FBuyFillColor( clWhite )
,FBuyFillBKColor( (TColor)0x00763A0A )
,FSellFillColor( clWhite )
,FSellFillBKColor( (TColor)0x00763A0A )
,FMA1Color( clLime )
,FMA2Color( clYellow )
,FMA3Color( clSkyBlue )
,FBBMAColor( clSilver )
{
	FBezierPoints = new TPoint[ POINT_ARRAY_SIZE ];
	FMeanPxs      = new double[ HALF_POINT_ARRAY_SIZE ];
	FOffsetPxs    = new double[ HALF_POINT_ARRAY_SIZE ];
	///< Init varbs for K line thumb image.
	FThumbHRatio       = (float)MARKET_MINUTES/(float)THUMB_WIDTH_MAX;///< THUMB_WIDTH_MAX != 0, Ok!
	FMapBitmap->Width  = MARKET_MINUTES;///< Default 300
	FMapBitmap->Height = MARKET_MINUTES*9/16;

	FQtys.Length       = MARKET_MINUTES;
	FMaxPx.Length      = MARKET_MINUTES;
	FMinPx.Length      = MARKET_MINUTES;
	FOpenPx.Length     = MARKET_MINUTES;
	FClosePx.Length    = MARKET_MINUTES;
	FHaveData.Length   = MARKET_MINUTES;
	FMapRect = TRect( 10,10, FMapBitmap->Width+10, FMapBitmap->Height +10 );
	///< Buffer image for drawing Bubble.
	FBubbleBmp = new Graphics::TBitmap();
	///< Images for bull bubble.
	FBullBmpL = new Graphics::TBitmap();
	FBullBmpM = new Graphics::TBitmap();
	FBullBmpS = new Graphics::TBitmap();
	///< Images for bear bubble.
	FBearBmpL = new Graphics::TBitmap();
	FBearBmpM = new Graphics::TBitmap();
	FBearBmpS = new Graphics::TBitmap();
	///< Images for reference px bubble
	FRefBmpL = new Graphics::TBitmap();
	FRefBmpM = new Graphics::TBitmap();
	FRefBmpS = new Graphics::TBitmap();
	///< Init button's images
	FCancelAllBuyBmp  = new Graphics::TBitmap();
	FCancelAllSellBmp = new Graphics::TBitmap();
	FCenterFilledBmp  = new Graphics::TBitmap();

	FPositionPoints = new TObjectList();
	FPositionPoints->OwnsObjects = true;
	FTimer = new TTimer( this );
	FTimer->Interval = FPaintInterval;
	FTimer->Enabled  = false;
	FTimer->OnTimer  = OnRepaintTimer;
}
//------------------------------------------------------------------------------
__fastcall TStockChartEx::~TStockChartEx( void )
{
	SafeRelease( &FBullRadialGradientBrush );
	SafeRelease( &FBearRadialGradientBrush );
	SafeRelease( &FRefeRadialGradientBrush );
	SafeRelease( &FClipLayer );
	SafeRelease( &FRectangleGeometry );
	if( FD2DCanvas != NULL )
		delete FD2DCanvas;
	ClearData( );
	delete [] FBezierPoints;
	delete [] FMeanPxs;
	delete [] FOffsetPxs;
	delete FMapBitmap;
	delete FBuffer;
	delete FBubbleBmp;
	delete FBullBmpL;
	delete FBullBmpM;
	delete FBullBmpS;
	delete FBearBmpL;
	delete FBearBmpM;
	delete FBearBmpS;
	delete FRefBmpL;
	delete FRefBmpM;
	delete FRefBmpS;
	delete FCancelAllBuyBmp;
	delete FCancelAllSellBmp;
	delete FCenterFilledBmp;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::CreateWnd ()
{
	TCustomControl::CreateWnd ();
	if( !ComponentState.Contains( csDesigning ) ) ///< Not design time.
	{
		if( TDirect2DCanvas::Supported() == true  )
		{
			FD2DCanvas = new Direct2d::TDirect2DCanvas( (HWND)Handle );
			FD2DCanvas->RenderTarget->GetFactory( &FD2DFactory );
			if( FD2DFactory == NULL )
			{
				delete FD2DCanvas;
				FD2DCanvas = NULL;
			}
		}
		else
			FD2DCanvas = NULL;
	}
	Resize();
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::RecreateD2D ()
{
	if( FD2DCanvas != NULL )
		delete FD2DCanvas;
	FD2DCanvas = new Direct2d::TDirect2DCanvas( (HWND)Handle );
	FD2DCanvas->RenderTarget->GetFactory( &FD2DFactory );
	if( FD2DFactory == NULL )
	{
		delete FD2DCanvas;
		FD2DCanvas = NULL;
		return;
	}
	Resize();
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetStore(TCMarketDataStore* Store)
{
	FStore = Store;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetCreatePositionPoint( TPositionPoint* NewPos )
{
	FPositionPoints->Add( NewPos );
	Paint();
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::CenterPositionPoint( int Index )
{
	if( Index >=0 && Index < FPositionPoints->Count )
	{
		TPoint          VartualPos;
		TPositionPoint* CenterPos = (TPositionPoint*)FPositionPoints->Items[Index];

		FSelectPP = Index;
		TimeTickToVirtualScreen( CenterPos->FRefSec, CenterPos->F10ms, CenterPos->FFillTickIndex, VartualPos );
		FViewPortX = VartualPos.x - Width/2;
		FViewPortY = VartualPos.y - Height/2;
		Paint();
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::ClearPosition( void )
{
	FPositionPoints->Clear();
	Paint();
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::OnRepaintTimer(TObject *Sender )
{
	FTimer->Enabled = false;
	Paint();
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::Loaded( void )
{
	TCustomControl::Loaded();
	///< Set font
	FBuffer->Canvas->Font->Assign( Font );
	FBuffer->Canvas->Font->Size = FONT_DEF_SIZE;
	FFontSize    = FONT_DEF_SIZE;
	FOldFontSize = FONT_DEF_SIZE;
	///< Set event handler.
	OnMouseWheel = MouseWheel;
	OnMouseEnter = MouseEnter;
	///< Init K line thumb image and data.
	int MARKET_MINUTES = FSecondsCount/60;
	if( MARKET_MINUTES > THUMB_WIDTH_MAX )
	{
		FThumbHRatio  = (float)MARKET_MINUTES/(float)THUMB_WIDTH_MAX; ///< THUMB_WIDTH_MAX !=0, Ok
		FMapBitmap->Width    = THUMB_WIDTH_MAX;
	}
	else
	{
		FThumbHRatio  = 1.0;
		FMapBitmap->Width    = MARKET_MINUTES;
	}
	FMapBitmap->Height   = (FMapBitmap->Width*9/16);
	FGridQtys.Length     = FSecondsCount;
	FGridSubTotal.Length = FSecondsCount;
	FSecTrueRange.Length = FSecondsCount;
	FQtys.Length         = MARKET_MINUTES;
	FMaxPx.Length        = MARKET_MINUTES;
	FMinPx.Length        = MARKET_MINUTES;
	FOpenPx.Length       = MARKET_MINUTES;
	FClosePx.Length      = MARKET_MINUTES;
	FHaveData.Length     = MARKET_MINUTES;
	FMapRect = TRect( 10,10, FMapBitmap->Width+10, FMapBitmap->Height +10 );
	///< Build bubble images.
	if( FBullBmpL->Empty == false )
		BuildImages( FBullBmpL, FBullBmpL, FBullBmpM, FBullBmpS );
	if( FBearBmpL->Empty == false )
		BuildImages( FBearBmpL, FBearBmpL, FBearBmpM, FBearBmpS );
	if( FRefBmpL->Empty == false )
		BuildImages( FRefBmpL, FRefBmpL, FRefBmpM, FRefBmpS );
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::Clear( void )
{
	for( register int i = 0; i < FTicks->ItemCount(); i++ )
	{
		 FUserBuyQty[ i ]        = 0;
		 FUserSellQty[ i ]       = 0;
		 FUserFilledBuyQty[ i ]  = 0;
		 FUserFilledSellQty[ i ] = 0;
	}
	FTotalUserBuyQty  = 0;
	FTotalUserSellQty = 0;
	FTotalUserFilledBuyQty = 0;
	FTotalUserFilledSellQty = 0;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::ClearData( void )
{
	FQtyMax       = 50;
	FFillPxQtyMax = 1;
	///< Clear K Line thumb image data
	for( int i =0; i<  FHaveData.Length ; i++ )
		 FHaveData[ i ] = false;
	///< Cliar Tick data
	for( int i = 0; i < FTicksList.ItemCount(); i++ )
		 delete ((TTickInfo*) FTicksList.GetItem( i ));
	///< Clear Depth data
	for( int i =0; i< DEPTH_COUNT; i ++ )
	{
		FBuyPxIndex[i] = -1;
		FSellPxIndex[i] = -1;
	}
	for( int i =0; i< FSecondsCount; i++ )
	{
		FGridQtys[i]     = 0;
		FGridSubTotal[i] = 0;
	}
	FTotalUserBuyQty  = 0;
	FTotalUserSellQty = 0;
	FTotalUserFilledBuyQty = 0;
	FTotalUserFilledSellQty = 0;
	FTicksList.Clear();
	FTickIDSet.Clear();
}
//------------------------------------------------------------------------------
//  Get the virtual screen position
//  from the given sec,ms and price index.(tick)
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::TimeTickToVirtualScreen( int RefTime, int TenMS, int Tick, TPoint& VsPos )
{
	VsPos.y = Tick*FPixelsPerTick + FPixelsPerTick/2;
	VsPos.x = (int)((double)RefTime*FPixelsPerSecond) + (int)(TenMS*FPixelPer10ms);
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::TimeTickToVirtualScreen( int RefTime, int TenMS, double Px, TPoint& VsPos )
{
	double TotalPxDiff = FBullPrice - FBearPrice;
	if( TotalPxDiff != 0.0 ) ///< Check TotalPxDiff != 0.
	{
		VsPos.y = FVirtualHeight - (int)((double)(FVirtualHeight)*(Px - FBearPrice)/ TotalPxDiff );
		VsPos.x = (int)((double)RefTime*FPixelsPerSecond) + (int)(TenMS*FPixelPer10ms);
	}
}
//------------------------------------------------------------------------------
//  Get the Window position
//  from the given sec,ms and price index.(tick)
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::TimeTickToWindow( int RefTime, int TenMS, int Tick, TPoint& WinPos )
{
	WinPos.y = Tick*FPixelsPerTick + FPixelsPerTick/2  - FViewPortY;
	WinPos.x = (int)((double)RefTime*FPixelsPerSecond) + (int)(TenMS*FPixelPer10ms) - FViewPortX;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::TimeTickToWindow( int RefTime, int TenMS, double Px, TPoint& WinPos )
{
	double TotalPxDiff = FBullPrice - FBearPrice;

	if( TotalPxDiff != 0.0 ) ///< Check TotalPxDiff != 0.
	{
		int PosY = (int)((double)(FVirtualHeight)*(Px - FBearPrice)/ TotalPxDiff );
		WinPos.y = FVirtualHeight - PosY - FViewPortY;
		WinPos.x = (int)((double)RefTime*FPixelsPerSecond) + (int)(TenMS*FPixelPer10ms) - FViewPortX;
	}
}
//------------------------------------------------------------------------------
//  Get the sec,ms and price index
//  from the given virtual screen X, Y position.
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::VirtualScreenToTimeTick( int& RefTime, int& TenMS, int& Tick, const TPoint& VsPos )
{
	if( FPixelsPerTick != 0 ) ///< Check FPixelsPerTick != 0.
	{
		VirtualScreenXToTime( RefTime, TenMS, VsPos.x );
		Tick    = VsPos.y/FPixelsPerTick;
	}
}
//------------------------------------------------------------------------------
//  Get the sec,ms
//  from the given virtual screen X position.
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::VirtualScreenXToTime( int& RefTime, int& TenMS, int X )
{
	if( FPixelPer10ms != 0 && FPixelsPerSecond != 0 ) ///< Check FPixelPer10ms,FPixelsPerSecond != 0.
	{
		double ms = fmod( (double)X, FPixelsPerSecond );

		RefTime = X/FPixelsPerSecond;
		TenMS   = (int)(ms/ FPixelPer10ms );
	}
}
//------------------------------------------------------------------------------
// Is the given virtual screen position in view?
//------------------------------------------------------------------------------
bool __fastcall TStockChartEx::PointInView( const TPoint& VSPos )
{
	if( VSPos.x < FViewPortX || VSPos.x > FViewPortX + Width )
		return false;
	return true;
}
//------------------------------------------------------------------------------
int  __fastcall TStockChartEx::CloseRefSec( void )
{
	return FCloseRefSec;
}
//------------------------------------------------------------------------------
int __fastcall TStockChartEx::ToRefSec( int HH, int MM, int SS )
{
	if( FExchangeInfo != NULL )
		return FExchangeInfo->TimeToIndexEx( HH, MM, SS );
	return 0;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::RefSecToHHMMSS( int Index ,int& HH, int& MM, int& SS )
{
	if( FExchangeInfo != NULL )
	{
		UFC::AnsiString Time;
		FExchangeInfo->IndexToTimeEx( Index, Time );
		HH = Time.SubString( 0, 2 ).ToInt();
		MM = Time.SubString( 3, 2 ).ToInt();
		SS = Time.SubString( 6, 2 ).ToInt();
	}
	else
	{
		HH = 8;
		MM = 45;
		SS = 0;
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::RefSecToTimeString( int Index , UFC::AnsiString& Time )
{
	if( FExchangeInfo != NULL )
		FExchangeInfo->IndexToTimeEx( Index, Time );
	else
		Time = "08:45:00";
}
//------------------------------------------------------------------------------
bool __fastcall TStockChartEx::ConvertData( TTickInfo& Tick )
{
	int HHMMSS   = Tick.Time/100;

	try
	{
		Tick.TenMS   = Tick.Time%100;
		Tick.RefSec  = ToRefSec( HHMMSS/10000, (HHMMSS%10000)/100, HHMMSS%100 );
		Tick.BuyPxIndex   = PxToTickIndex( Tick.BuyPx, false );
		Tick.SellPxIndex  = PxToTickIndex( Tick.SellPx,false );
		Tick.FillPxIndex  = PxToTickIndex( Tick.FillPx,FDebugMode );
		return true;
	}
	catch( ...)
	{
		return false;
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetData( TTickInfo& Tick, bool NeedPaint )
{
	if( FTickIDSet.Exists( Tick.ID ) == false )
	{
		FTickIDSet.Add( Tick.ID );
		if( ConvertData( Tick ) == true && Tick.RefSec < FSecondsCount )
		{
			int PxQty  = FFillQtyArray[ Tick.FillPxIndex ] + Tick.FillQty;

			if( PxQty > FFillPxQtyMax )
				FFillPxQtyMax = PxQty;
			Tick.FShowed = !NeedPaint;
			FGridQtys[ Tick.RefSec ]     += Tick.FillQty;
			FGridSubTotal[ Tick.RefSec ] += Tick.FillPxIndex * Tick.FillQty;//FillPx*Qty;
			FFillQtyArray[ Tick.FillPxIndex ]   = PxQty;
			FTicksList.Add( (void* )new TTickInfo( Tick ) );
			FLastTick = (TTickInfo*)FTicksList.GetItem( FTicksList.ItemCount() - 1 );
			if( FTicksList.ItemCount() == 2 )///< First real tick come in!
				CenterLastTick();///< Center both X,Y.
			else if( NeedPaint == true )
			{
				if( FCenterFillPx == true )
					CenterLastTick( true ); ///< Center X only.
				else
					Paint();///< redraw only
			}
		}
		else
		{
			String Msg;

			Msg.printf(L"Bid[%5.3f] Ask[%5.3f] Price[%5.3f] Qty[%d] Time[%08d]",Tick.BuyPx,Tick.SellPx,Tick.FillPx, Tick.FillQty, Tick.Time );
			if( FOnErrorMessage != NULL )
				FOnErrorMessage( this, L"成交價格或時間錯誤", Msg );
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetData( int HHMMSSmm, int ID, int Seq, double BuyPx, double SellPx, double FillPx, int Qty, bool NeedPaint )
{
	TTickInfo NewTick;

	NewTick.ID      = ID;
	NewTick.Seq     = Seq;
	NewTick.Time    = HHMMSSmm;
	NewTick.FillQty = Qty;
	NewTick.BuyPx   = BuyPx;
	NewTick.SellPx  = SellPx;
	NewTick.FillPx  = FillPx;
	SetData( NewTick, NeedPaint );
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetMinuteData( TMinutesTickInfo& Info, bool NeedPaint )
{
	if( FExchangeInfo != NULL )
	{
		int Index  = FExchangeInfo->TimeToIndex( Info.Hour, Info.Minutes );

		if( Index >= 0 && Index < MARKET_MINUTES )
		{
			FThunbDirty      = true;
			FQtys[Index]     = Info.Qty;
			FMaxPx[Index]    = Info.MaxPx;
			FMinPx[Index]    = Info.MinPx;
			FOpenPx[Index]   = Info.OpenPx;
			FClosePx[Index]  = Info.ClosePx;
			FHaveData[Index] = true;
			if( Info.Qty > FQtyMax )
				FQtyMax = Info.Qty;
			if( Info.MaxPx > FMaxPrice )
			{
				FMaxPrice   = Info.MaxPx;
				FMaxPxIndex = PxToTickIndex( FMaxPrice,FDebugMode );
			}
			if( Info.MinPx < FMinPrice )
			{
				FMinPrice   = Info.MinPx;
				FMinPxIndex = PxToTickIndex( FMinPrice,FDebugMode );
			}
			if( NeedPaint == true && FThumbChart == true )
				Paint();
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetFPS( int FPS )
{
	if( FPS < 4 )
		FPS = 4;
	if( FPS > 60 ) ///< upto 60 fps
		FPS = 60;
	FMaxFPS = FPS;
	FPaintInterval   = 1000/FMaxFPS;
	FTimer->Interval = FPaintInterval;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetThumbChart( bool Show )
{
	if( FThumbChart != Show )
	{
		FThumbChart = Show;
		Paint();
    }
}
//------------------------------------------------------------------------------
bool __fastcall TStockChartEx::SetExchangeTime( BasicInformation* SymbolInfo  )
{
	if( FStore != NULL &&
		(FExchangeInfo = FStore->GetExchangeInfo(SymbolInfo->GetExchange(), SymbolInfo->GetProductID() )) != NULL )
	{
		UFC::AnsiString OpenTime  = FExchangeInfo->GetSessionStartTime( 0 );
		UFC::AnsiString CloseTime = FExchangeInfo->GetSessionEndTime( FExchangeInfo->GetSessionCount()-1 );

		BEFORE_OPEN_SEC  = FExchangeInfo->GetExtraSecondBeforeOpen();
		AFTER_CLOSED_SEC = FExchangeInfo->GetExtraSecondAfterClose();
		FOpenHour     = OpenTime.SubString( 0, OpenTime.AnsiPos( ':' ) ).ToInt();
		FOpenMinutes  = OpenTime.SubString( OpenTime.AnsiPos( ':' ) + 1, OpenTime.Length() - OpenTime.AnsiPos( ':' ) - 1 ).ToInt();
		FSecondsCount = FExchangeInfo->GetTotalTicksEx();
		FCloseRefSec  = FSecondsCount - FExchangeInfo->GetExtraSecondAfterClose();
		FOpenTimeSec  = FOpenHour * 3600 + FOpenMinutes * 60 - FExchangeInfo->GetExtraSecondBeforeOpen();
		FCloseTimeSec = CloseTime.SubString( 0, CloseTime.AnsiPos( ':' ) ).ToInt() * 3600 +
						CloseTime.SubString( CloseTime.AnsiPos( ':' ) + 1, CloseTime.Length() - CloseTime.AnsiPos( ':' ) - 1 ).ToInt() * 60
						+ FExchangeInfo->GetExtraSecondAfterClose();
		MARKET_MINUTES = FExchangeInfo->TotalTicks();

		if( MARKET_MINUTES > THUMB_WIDTH_MAX )
		{
			FThumbHRatio  = (float)MARKET_MINUTES/(float)THUMB_WIDTH_MAX;///< THUMB_WIDTH_MAX != 0
			FMapBitmap->Width    = THUMB_WIDTH_MAX;
		}
		else
		{
			FThumbHRatio  = 1.0;
			FMapBitmap->Width    = MARKET_MINUTES;
		}
		FMapBitmap->Height   = (FMapBitmap->Width*9/16);
		FQtys.Length       = MARKET_MINUTES;
		FMaxPx.Length      = MARKET_MINUTES;
		FMinPx.Length      = MARKET_MINUTES;
		FOpenPx.Length     = MARKET_MINUTES;
		FClosePx.Length    = MARKET_MINUTES;
		FHaveData.Length   = MARKET_MINUTES;
		FMapRect = TRect( 10,10, FMapBitmap->Width+10, FMapBitmap->Height +10 );

		FGridQtys.Length     = FSecondsCount;
		FGridSubTotal.Length = FSecondsCount;
		FSecTrueRange.Length = FSecondsCount;
		Resize();
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetSymbol( BasicInformation* SymbolInfo, TTickList* Ticks )
{
	String WideExchange( SymbolInfo->GetExchange().c_str() );
	String WideSymbol( SymbolInfo->GetSymbol().c_str() );
	FInitSymbol = false;

	if( Ticks != NULL )
		FTicks = Ticks;
	else
		FTicks = (TTickList*)FStore->GetTickList( WideExchange, WideSymbol );
	if( FTicks != NULL && FStore != NULL && SetExchangeTime( SymbolInfo ) == true )
	{
		int TickCount   = FTicks->ItemCount();
		int IndexOffset = ( TickCount + 9 )/10;

		ClearData();///< Clear old ticks and map
		FEnableLog2     = false;
		FThunbDirty     = true;
		FStrikePxDigi   = SymbolInfo->GetDigit();
		FDecimalLocator = SymbolInfo->GetDecimalLocator();
		FExchange       = SymbolInfo->GetExchange().c_str();
		FSymbol         = SymbolInfo->GetSymbol().c_str();
		FFillQtyArray.Length       =  TickCount;
		FUserBuyQty.Length         =  TickCount;
		FUserSellQty.Length        =  TickCount;
		FUserFilledBuyQty.Length   =  TickCount;
		FUserFilledSellQty.Length  =  TickCount;
		FVirtualHeight             =  TickCount * FPixelsPerTick;
		for( register int i = 0; i < TickCount; i++ )
		{
			 FUserBuyQty[ i ]        = 0;
			 FUserSellQty[ i ]       = 0;
			 FUserFilledBuyQty[ i ]  = 0;
			 FUserFilledSellQty[ i ] = 0;
			 FFillQtyArray[ i ]      = 0;
		}
		///< init all price.
		FBullPrice  = SymbolInfo->GetBullPrice();
		FBearPrice  = SymbolInfo->GetBearPrice();
		FRefPrice   = SymbolInfo->GetRefPrice();
		FRefPxIndex = PxToTickIndex( FRefPrice, false );
		FMaxPxIndex = FRefPxIndex - IndexOffset;
		FMinPxIndex = FRefPxIndex + IndexOffset;
		if( FMaxPxIndex < 0 )
			FMaxPxIndex = 0;
		if( FMinPxIndex >= FTicks->ItemCount());
			FMinPxIndex = FTicks->ItemCount() -1;
		FMaxPrice   = FTicks->GetPxFromIndex( FMaxPxIndex, FDebugMode );
		FMinPrice   = FTicks->GetPxFromIndex( FMinPxIndex, FDebugMode );
		Resize( );

		UFC::PStringList TimeStrings;
		int Hour = 9;
		int Min  = 0;

		TimeStrings.SetStrings( FExchangeInfo->GetSessionStartTime( 0 ), ":.\n" );
		if( TimeStrings.ItemCount( ) >= 1 )
			Hour = TimeStrings[ 0 ].ToInt();
		if( TimeStrings.ItemCount( ) >= 2 )
			Min = TimeStrings[ 1 ].ToInt();
		UFC::UDateTime OpenTime( Hour, Min, 59, 999 );
		OpenTime -= 60;
		int OpenTimeInt = OpenTime.getHour()*1000000 + OpenTime.getMinute()*10000 + 5999;
		SetData( OpenTimeInt, -1, 0, FRefPrice, FRefPrice, FRefPrice, 0, false );
	}
	FInitSymbol = true;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::UpdateQty( nsOrderMessageDefine::SideEnum side, double Price, int Qty )
{
	int PxIndex = PxToTickIndex( Price, FDebugMode );

	if( side == nsOrderMessageDefine::sBuy )
	{
		FTotalUserBuyQty += Qty;
		if( FUserBuyQty[ PxIndex ] != 0 )
			FTotalUserBuyQty -= FUserBuyQty[ PxIndex ];
		FUserBuyQty[ PxIndex ] = Qty;
	}
	else
	{
		FTotalUserSellQty += Qty;
		if( FUserSellQty[ PxIndex ] != 0 )
			FTotalUserSellQty -= FUserSellQty[ PxIndex ];
		FUserSellQty[ PxIndex ] = Qty;
	}
	Paint();
}
//------------------------------------------------------------------------------
double __fastcall  TStockChartEx::IndexToPx( double DoubleIndex )
{
	if( FTicks != NULL )
	{
		int     IntPart   = (int)DoubleIndex;
		double  FloatPart = 1.0f - (DoubleIndex -((double)IntPart));
		double  BasePx    = FTicks->GetPxFromIndex( IntPart, FDebugMode );
		double  UpperPx   = FTicks->GetPxFromIndex( IntPart - 1, FDebugMode );

		return  BasePx + ((UpperPx-BasePx)*FloatPart);
	}
	else
		return (int)DoubleIndex;
}
//------------------------------------------------------------------------------
int __fastcall TStockChartEx::PxToTickIndex( double Price, bool ThrowException )
{
	if( FTicks == NULL )
		return -1;
	if( ThrowException == false )
	{
		int Index = FTicks->PxToTickIndex( Price, false );

		if( Index == -1 )
			return FTicks->FindTick( Price );
		return Index;
	}
	else
		return FTicks->PxToTickIndex( Price );
}
//------------------------------------------------------------------------------
bool __fastcall TStockChartEx::CanZooming( const String& PriceText, int NewSize )
{
	int PixelsPerXGrid,PixelsPerYGrid;

	if( FEnableAccelerate == true && FD2DCanvas != NULL   ) ///< Use D2D
	{
		FD2DCanvas->Font->Size = NewSize;
		PixelsPerXGrid   = FD2DCanvas->TextWidth( PriceText );
		PixelsPerYGrid   = FD2DCanvas->TextHeight( L"12345.67890") * 1.5;
	}
	else ///< Use GDI
	{
		FBuffer->Canvas->Font->Size = NewSize;
		PixelsPerXGrid   = FBuffer->Canvas->TextWidth( PriceText );
		PixelsPerYGrid   = FBuffer->Canvas->TextHeight( L"12345.67890") * 1.5;
	}
	double PixelsPerSecond = (double)PixelsPerXGrid/(double)FSecondsPerGrid; ///< FSecondsPerGrid 1~30
	int    VirtualWidth    = (int)( (double)FSecondsCount*PixelsPerSecond );
	int    VirtualHeight   = PixelsPerYGrid * 20;

	if( FTicks != NULL )
		VirtualHeight  = FTicks->ItemCount() * PixelsPerYGrid;
	if( VirtualWidth < Width  ||  VirtualHeight < Height )
		return false;
	return true;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::Resize( void )
{
	TPoint ViewPortCenter( FViewPortX + FOldWidth/2,FViewPortY + FOldHeight/2 );
	int    CenterRefTime,CenterTenMS,CenterTick;
	String PriceText;

	PriceText.printf( L"%.*f(999)", FDecimalLocator, FBullPrice );
	if( FOldFontSize != FFontSize ) ///< Zooming
	{
		if( CanZooming( PriceText, FFontSize ) == false )
			FFontSize = FOldFontSize;
		VirtualScreenToTimeTick( CenterRefTime, CenterTenMS, CenterTick, ViewPortCenter );
	}
	if( FEnableAccelerate == true && FD2DCanvas != NULL   )
	{
		ID2D1HwndRenderTarget* HWRenderTarget;

		HRESULT HR = FD2DCanvas->RenderTarget->QueryInterface( &HWRenderTarget );
		if( HR == S_OK )
		{
			HWRenderTarget->SetDpi( 96.0, 96.0 );
			HWRenderTarget->Resize( D2D1SizeU(Width, Height) );
			HWRenderTarget->Release();

			FD2DCanvas->Font->Size = FFontSize;
			FPixelsPerGrid   = FD2DCanvas->TextWidth( PriceText );
			FPixelsPerTick   = FD2DCanvas->TextHeight( L"12345.67890") * 1.5;
			FOldWidth  = Width;
			FOldHeight = Height;
			if( CreateD2DObjecsOnResize( ) == false )
				MessageDlg(	L"call CreateD2DObjecsOnResize failed.", mtError, TMsgDlgButtons() << mbOK, 0);
			else if( CreateD2DObjecsOnZoom() == false )
				MessageDlg(	L"call CreateD2DObjecsOnZoom failed.", mtError, TMsgDlgButtons() << mbOK, 0);
		}
	}
	else ///< Use GDI
	{
		if( FBuffer->Width  != Width )
		{
			FOldWidth = Width;
			FBuffer->Width  = Width;
		}
		if( FBuffer->Height != Height )
		{
			FOldHeight = Height;
			FBuffer->Height = Height;
		}
		FBuffer->Canvas->Font->Size = FFontSize;
		FPixelsPerGrid   = FBuffer->Canvas->TextWidth( PriceText );
		FPixelsPerTick   = FBuffer->Canvas->TextHeight( L"12345.67890") * 1.5;
	}
	FPixelsPerSecond = (double)FPixelsPerGrid / (double)FSecondsPerGrid; ///< FSecondsPerGrid 1~30
	FVirtualWidth    = (int)((double)FSecondsCount*FPixelsPerSecond);
	if( FTicks != NULL )
		FVirtualHeight   = FTicks->ItemCount() * FPixelsPerTick;
	else
		FVirtualHeight   = FPixelsPerTick;
	FViewPortWidth   = Width;  ///< Equals to Control Width
	FViewPortHeight  = Height; ///< Equals to Control Height
	FPixelPer10ms    = FPixelsPerSecond/100.0;
	if( FShowFilled == true )
		FHeaderBitmap->Width  = FPixelsPerGrid*7;
	else
		FHeaderBitmap->Width  = FPixelsPerGrid*5;
	FHeaderBitmap->Height = FPixelsPerTick;
	if( FOldFontSize != FFontSize )
	{
		TimeTickToVirtualScreen( CenterRefTime, CenterTenMS, CenterTick, ViewPortCenter );
		FViewPortX   = ViewPortCenter.x - Width/2;
		FViewPortY   = ViewPortCenter.y - Height/2;
		FOldFontSize = FFontSize;
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::CheckViewPortPosition( void )
{
	if( FViewPortX < 0 )
		FViewPortX = 0;
	if( FViewPortY < 0 )
		FViewPortY = 0;
	if( FViewPortX + Width > FVirtualWidth )
		FViewPortX = FVirtualWidth - Width;
	if( FViewPortY + Height > FVirtualHeight )
		FViewPortY = FVirtualHeight - Height;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawTimeGrid( TCanvas* canvas )
{
	int DeltaX       = FViewPortX%FPixelsPerGrid;
	int MaxY         = Height - FPixelsPerTick;
	int HalfStrWidth = canvas->TextWidth( L"13:59:59" )/2;
	int OffsetX,RefSec;
	UFC::AnsiString TimeStr;
	TRect           PaintRect;

	if( DeltaX == 0 )///< Zero point in second's line.
	{
		OffsetX = 0;
		RefSec  = FViewPortX/FPixelsPerGrid * FSecondsPerGrid;///< Check div by 0 OK
	}
	else
	{
		OffsetX = FPixelsPerGrid - DeltaX;
		RefSec  = (FViewPortX + OffsetX)/FPixelsPerGrid * FSecondsPerGrid;///< Check div by 0 OK
	}
	canvas->Pen->Color   = FGridColor;
	canvas->Font->Color  = FSellFillColor;
	canvas->Brush->Style = bsClear;
	canvas->MoveTo( 0, 0 );
	canvas->LineTo( 0, Height );
	canvas->MoveTo( 0, MaxY );
	canvas->LineTo( Width, MaxY );
	RefSecToTimeString( RefSec, TimeStr );
	PaintRect = TRect( OffsetX - FPixelsPerGrid - HalfStrWidth, MaxY, OffsetX - FPixelsPerGrid + HalfStrWidth, Height );
	DrawTextExA( canvas->Handle,(char*)TimeStr.c_str(),TimeStr.Length(),&PaintRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
	while( OffsetX < Width + FPixelsPerGrid )
	{   ///< Draw time V line.
		canvas->MoveTo( OffsetX, 0 );
		canvas->LineTo( OffsetX, MaxY );
		///< Draw time label
		PaintRect = TRect( OffsetX - HalfStrWidth, MaxY, OffsetX + HalfStrWidth, Height );
		RefSecToTimeString( RefSec, TimeStr );
		DrawTextExA( canvas->Handle,(char*)TimeStr.c_str(),TimeStr.Length(),&PaintRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
		OffsetX += FPixelsPerGrid;
		RefSec += FSecondsPerGrid;
	}
	canvas->Brush->Style = bsSolid;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawTimeGrid( TDirect2DCanvas* canvas )
{
	int         DeltaX       = FViewPortX%FPixelsPerGrid;
	int         MaxY         = Height - FPixelsPerTick;
	int         HalfStrWidth = canvas->TextWidth( L"13:59:59" )/2;
	int         OffsetX,RefSec;
	UFC::AnsiString  TimeStr;
	String      TimeString;
	TRect       PaintRect;
	TTextFormat Formats;

	Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
	if( DeltaX == 0 )///< Zero point in second's line.
	{
		OffsetX = 0;
		RefSec  = FViewPortX/FPixelsPerGrid * FSecondsPerGrid;///< Check div by 0 OK
	}
	else
	{
		OffsetX  = FPixelsPerGrid - DeltaX;
		RefSec  = (FViewPortX + OffsetX)/FPixelsPerGrid * FSecondsPerGrid; ///< Check div by 0 OK
	}
	canvas->Pen->Color   = FGridColor;
	canvas->Font->Color  = FSellFillColor;
	canvas->Brush->Style = bsClear;
	canvas->MoveTo( 0, 0 );
	canvas->LineTo( 0, Height );
	canvas->MoveTo( 0, MaxY );
	canvas->LineTo( Width, MaxY );
	RefSecToTimeString( RefSec, TimeStr );
	TimeString = TimeStr.c_str();
	PaintRect = TRect( OffsetX - FPixelsPerGrid - HalfStrWidth, MaxY, OffsetX - FPixelsPerGrid + HalfStrWidth, Height );
	canvas->TextRect(PaintRect,TimeString,Formats );
	while( OffsetX < Width + FPixelsPerGrid )
	{   ///< Draw time V line.
		canvas->MoveTo( OffsetX, 0 );
		canvas->LineTo( OffsetX, MaxY );
		///< Draw time label
		PaintRect = TRect( OffsetX - HalfStrWidth, MaxY, OffsetX + HalfStrWidth, Height );
		RefSecToTimeString( RefSec, TimeStr );
		TimeString = TimeStr.c_str();
		canvas->TextRect(PaintRect,TimeString,Formats );
		OffsetX += FPixelsPerGrid;
		RefSec += FSecondsPerGrid;
	}
	canvas->Brush->Style = bsSolid;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetBitmapAlpha( Graphics::TBitmap* RGBABmp, int Alpha  )
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
void __fastcall TStockChartEx::CreateCursor( int HotspotX, int HotspotY, int CancelQty, bool IsBuy )
{
	std::auto_ptr<Graphics::TBitmap> bmpMask( new Graphics::TBitmap );
	std::auto_ptr<Graphics::TBitmap> bmpColor( new Graphics::TBitmap );
	std::auto_ptr<TIconInfo>         iconInfo( new TIconInfo );
	TRect                            CurRect(0,0,FPixelsPerGrid,FPixelsPerTick);
	UFC::AnsiString                  QtyStr( CancelQty );

	///< Release old cursor.
	DestroyIcon( Screen->Cursors[5] );
	///< Create mask 1bit image.
	bmpMask->Width  = FPixelsPerGrid;
	bmpMask->Height = FPixelsPerTick;
	bmpMask->PixelFormat = pf1bit;
	bmpMask->Canvas->Brush->Color = clBlack;
	bmpMask->Canvas->FillRect( CurRect ); ///< All image pass mask.
	///< Create color image
	bmpColor->Width  = FPixelsPerGrid;
	bmpColor->Height = FPixelsPerTick;
	bmpColor->PixelFormat = pf32bit;   ///< RGBA format
	bmpColor->AlphaFormat = afDefined; ///< Use alpha value.
	if( IsBuy == true )
	{
		bmpColor->Canvas->Font->Color  = FBuyColor;
		bmpColor->Canvas->Brush->Color = FBuyBKColor;
	}
	else
	{
		bmpColor->Canvas->Font->Color  = FSellColor;
		bmpColor->Canvas->Brush->Color = FSellBKColor;
	}
	bmpColor->Canvas->Pen->Color   = FGridColor;
	bmpColor->Canvas->Rectangle( 0, 0, FPixelsPerGrid, FPixelsPerTick );
	bmpColor->Canvas->Brush->Style = bsClear;
	bmpColor->Canvas->Font->Size   = FFontSize;
	DrawTextExA( bmpColor->Canvas->Handle,(char*)QtyStr.c_str(),QtyStr.Length(),&CurRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
	SetBitmapAlpha( bmpColor.get(), 148  );
	///< Create cursor icon
	iconInfo->fIcon = false; ///< Cursor
	iconInfo->xHotspot = HotspotX;
	iconInfo->yHotspot = HotspotY;
	iconInfo->hbmMask  = bmpMask->Handle;
	iconInfo->hbmColor = bmpColor->Handle;
	Screen->Cursors[5] = CreateIconIndirect(iconInfo.get());
	Screen->Cursor = TCursor( 5 );
}
//------------------------------------------------------------------------------
int __fastcall TStockChartEx::GetLastTickLeft( void  )
{
	if( FLastTick != NULL )
		return FLastTickPos.x - (FLastTickPos.x % FPixelsPerGrid) - FViewPortX;
	return 0;
}
//------------------------------------------------------------------------------
bool __fastcall TStockChartEx::IsClickCursor( int X )
{
	if( FLastTick != NULL )
	{
		int FillLeft  = GetLastTickLeft();
		int FillRight = FillLeft  + FPixelsPerGrid;
		int LeftX     = FillLeft  - 2*FPixelsPerGrid;
		int RightX    = FillRight + 2*FPixelsPerGrid;

		if( X > LeftX && X < FillLeft )
			return true;
		if( X > FillRight && X < RightX )
			return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool __fastcall TStockChartEx::IsLastTickInView( void )
{
	if( FLastTick != NULL )
	{
		TPoint LastTickPos;

		TimeTickToVirtualScreen( FLastTick->RefSec, FLastTick->TenMS, FLastTick->FillPxIndex, LastTickPos );
		return PointInView( LastTickPos );
	}
	return false;
}
//------------------------------------------------------------------------------
bool __fastcall TStockChartEx::IsLastTickInView( TPoint& LastTickPos )
{
	if( FLastTick != NULL )
	{
		TimeTickToVirtualScreen( FLastTick->RefSec, FLastTick->TenMS, FLastTick->FillPxIndex, LastTickPos );
		return PointInView( LastTickPos );
	}
	return false;
}
//------------------------------------------------------------------------------
Graphics::TBitmap* __fastcall TStockChartEx::SelectBitmap( int Diameter, TBubbleType BubbleType )
{
	switch( BubbleType )
	{
	   case btBullBubble: if( Diameter >> 4 == 0 )
							  return FBullBmpS;
						  else if( Diameter >> 6 > 0 )
							  return FBullBmpL;
						  else
							  return FBullBmpM;
	   case	btBearBubble: if( Diameter >> 4 == 0 )
							  return FBearBmpS;
						  else if( Diameter >> 6 > 0 )
							  return FBearBmpL;
						  else
							  return FBearBmpM;
	   default:           if( Diameter >> 4 == 0 )
							  return FRefBmpS;
						  else if( Diameter >> 6 > 0 )
							  return FRefBmpL;
						  else
							  return FRefBmpM;
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::AlphaBubble( TCanvas* canvas , int X,int Y, int R, TBubbleType BubbleType )
{
	int   Diameter = R << 1;
	TRect BubbleRect( X - R, Y -R, X + R, Y + R );
	TRect BufferRect( 0, 0, Diameter, Diameter);
	HRGN  BubbleRgn;

	///< Prepare bubble image in buffer.(Scale to real bubble size)
	FBubbleBmp->SetSize( Diameter, Diameter );
	FBubbleBmp->Canvas->StretchDraw( BufferRect, SelectBitmap( Diameter, BubbleType ) );
	///< Create circle clip region.
	BubbleRgn = CreateRoundRectRgn(  BubbleRect.left,BubbleRect.top,BubbleRect.right,BubbleRect.bottom, Diameter, Diameter );
	SelectClipRgn( canvas->Handle, BubbleRgn );
	///< Draw bubble.
	canvas->Draw( BubbleRect.left, BubbleRect.top, FBubbleBmp, 64 );
	///< Reset clip region.
	SelectClipRgn( canvas->Handle, NULL );
	DeleteObject( BubbleRgn );
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::AlphaBubble( TDirect2DCanvas* canvas , int X,int Y, int R, TBubbleType BubbleType )
{
	const D2D1_ELLIPSE  BubbleCircle = D2D1::Ellipse( D2D1::Point2F(R,R),R,R);
	float ScaleRatio = (float)R/32.0f;

	canvas->RenderTarget->SetTransform( D2D1::Matrix3x2F::Translation( X - R, Y - R ));
	switch( BubbleType )
	{
	   case btBullBubble: FBullRadialGradientBrush->SetTransform( D2D1::Matrix3x2F::Scale( D2D1::Size(ScaleRatio,ScaleRatio), D2D1::Point2F(0,0)));
						  canvas->RenderTarget->FillEllipse( BubbleCircle, FBullRadialGradientBrush );
						  break;
	   case	btBearBubble: FBearRadialGradientBrush->SetTransform( D2D1::Matrix3x2F::Scale( D2D1::Size(ScaleRatio,ScaleRatio), D2D1::Point2F(0,0)));
						  canvas->RenderTarget->FillEllipse( BubbleCircle, FBearRadialGradientBrush );
						  break;
	   default:			  FRefeRadialGradientBrush->SetTransform( D2D1::Matrix3x2F::Scale( D2D1::Size(ScaleRatio,ScaleRatio), D2D1::Point2F(0,0)));
						  canvas->RenderTarget->FillEllipse( BubbleCircle, FRefeRadialGradientBrush );
						  break;
	}
	canvas->RenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SolidBubble( TCanvas* canvas , int X,int Y, int R, TBubbleType BubbleType )
{
	TRect BubbleRect( X - R, Y -R, X + R, Y + R );
	TColor      OrgPenColor = canvas->Pen->Color;
	TBrushStyle OrgBrushStyle = canvas->Brush->Style;


	switch( BubbleType )
	{
	   case btBullBubble: canvas->Pen->Color   = clMaroon;
						  canvas->Brush->Color = clMaroon;
						  break;
	   case	btBearBubble: canvas->Pen->Color   = clGreen;
						  canvas->Brush->Color = clGreen;
						  break;
	   default:           canvas->Pen->Color   = clWhite;
						  canvas->Brush->Color = clWhite;
						  break;
	}
	canvas->Ellipse( BubbleRect );
	canvas->Pen->Color = OrgPenColor;
	canvas->Brush->Style = OrgBrushStyle;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SolidBubble( TDirect2DCanvas* canvas , int X,int Y, int R, TBubbleType BubbleType )
{
	TRect BubbleRect( X - R, Y -R, X + R, Y + R );

	switch( BubbleType )
	{
	   case btBullBubble: canvas->Pen->Color   = clMaroon;
						  canvas->Brush->Color = clMaroon;
						  break;
	   case	btBearBubble: canvas->Pen->Color   = clGreen;
						  canvas->Brush->Color = clGreen;
						  break;
	   default:           canvas->Pen->Color   = clWhite;
						  canvas->Brush->Color = clWhite;
						  break;
	}
	canvas->Ellipse( BubbleRect );
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawBubble( TCanvas* canvas , int X,int Y, int R, TBubbleType BubbleType, int MinAlphaBubble )
{
	if( FEnableLog2 && R > 0 )
	{
		double logtwo = Log2((double)R);
		R = (int)(logtwo*0.5*logtwo);
	}
	switch( BubbleType )
	{
	   case btBullBubble:///< Red bubble for bull price.
						if( FBullBmpL->Empty == true || R <= MinAlphaBubble )///< Use brush
							SolidBubble( canvas , X, Y, R, btBullBubble );
						else
							AlphaBubble( canvas , X, Y, R, btBullBubble );
						break;
	   case	btBearBubble:///< Green bubble for bear price.
						if( FBearBmpL->Empty == true || R <= MinAlphaBubble )///< Use brush
							SolidBubble( canvas , X, Y, R, btBearBubble );
						else
							AlphaBubble( canvas , X, Y, R, btBearBubble );
						break;
	   default:         ///< white bubble for reference price.
						if( FRefBmpL->Empty == true || R <= MinAlphaBubble )///< Use brush
							SolidBubble( canvas , X, Y, R, btRefBubble );
						else
							AlphaBubble( canvas , X, Y, R, btRefBubble );
						break;
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawBubble( TDirect2DCanvas* canvas , int X,int Y, int R, TBubbleType BubbleType, int MinAlphaBubble )
{
	if( FEnableLog2 && R > 0)
	{
		double logtwo = Log2((double)R);
		R = (int)(logtwo*0.5*logtwo);
	}
	switch( BubbleType )
	{
	   case btBullBubble:///< Red bubble for bull price.
						if( FBullBmpL->Empty == true || R <= MinAlphaBubble )///< Use brush
							SolidBubble( canvas , X, Y, R, btBullBubble );
						else
							AlphaBubble( canvas , X, Y, R, btBullBubble );
						break;
	   case	btBearBubble:///< Green bubble for bear price.
						if( FBearBmpL->Empty == true || R <= MinAlphaBubble )///< Use brush
							SolidBubble( canvas , X, Y, R, btBearBubble );
						else
							AlphaBubble( canvas , X, Y, R, btBearBubble );
						break;
	   default:         ///< white bubble for reference price.
						if( FRefBmpL->Empty == true || R <= MinAlphaBubble )///< Use brush
							SolidBubble( canvas , X, Y, R, btRefBubble );
						else
							AlphaBubble( canvas , X, Y, R, btRefBubble );
						break;
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawDepth( TCanvas* canvas , int FillX, int UpperY, int UpperTick )
{
	TRect TextRect;
	UFC::AnsiString QtyStr;
	int PixelsHelfSecond = FPixelsPerGrid/2;
	int PixelsHelfTick   = FPixelsPerTick/2;
	int Cx,Cy,Radius,PosY;

	if( FShowBubble == true )
	{
		for( register int i =0; i< DEPTH_COUNT; i ++ )
		{
			if( FSellPxIndex[i] != -1 && FSellPxIndex[i] >= UpperTick )
			{
				Cy = UpperY + (FSellPxIndex[i] - UpperTick)* FPixelsPerTick + PixelsHelfTick;
				Cx = FillX + 3*PixelsHelfSecond;
				Radius = (FSellQty[i]+1)/2;
				DrawBubble( canvas , Cx, Cy, Radius, btBullBubble  );
			}
			if( FBuyPxIndex[i] != -1  && FBuyPxIndex[i] >= UpperTick )
			{
				Cy = UpperY + (FBuyPxIndex[i] - UpperTick)* FPixelsPerTick + PixelsHelfTick;
				Cx = FillX - PixelsHelfSecond;
				Radius = (FBuyQty[i]+1)/2;
				DrawBubble( canvas , Cx, Cy, Radius, btBearBubble  );
			}
		}
	}
	canvas->Brush->Style = bsClear;
	canvas->Font->Color  = FAskDepth5Color;
	for( register int i =0; i< DEPTH_COUNT; i ++ )
	{
		if( FSellPxIndex[i] != -1 && FSellPxIndex[i] >= UpperTick )
		{
			PosY     = UpperY + (FSellPxIndex[i] - UpperTick)* FPixelsPerTick;
			TextRect = TRect( FillX + FPixelsPerGrid, PosY, FillX + 2*FPixelsPerGrid, PosY + FPixelsPerTick );
			QtyStr.Printf( "%d", FSellQty[i] );
			DrawTextExA( canvas->Handle,(char*)QtyStr.c_str(),QtyStr.Length(),&TextRect, DT_SINGLELINE|DT_LEFT|DT_VCENTER, NULL);
		}
	}
	canvas->Font->Color  = FBidDepth5Color;
	for( register int i =0; i< DEPTH_COUNT; i ++ )
	{
		if( FBuyPxIndex[i] != -1  && FBuyPxIndex[i] >= UpperTick )
		{
			PosY     = UpperY + (FBuyPxIndex[i] - UpperTick)* FPixelsPerTick;
			TextRect = TRect( FillX - FPixelsPerGrid, PosY, FillX, PosY + FPixelsPerTick );
			QtyStr.Printf( "%d", FBuyQty[i] );
			DrawTextExA( canvas->Handle,(char*)QtyStr.c_str(),QtyStr.Length(),&TextRect, DT_SINGLELINE|DT_RIGHT|DT_VCENTER, NULL);
		}
	}
	canvas->Brush->Style = bsSolid;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawDepth( TDirect2DCanvas* canvas , int FillX, int UpperY, int UpperTick )
{
	TRect TextRect;
	String QtyStr;
	TTextFormat     Formats;
	int PixelsHelfSecond = FPixelsPerGrid/2;
	int PixelsHelfTick   = FPixelsPerTick/2;
	int Cx,Cy,Radius,PosY;

	if( FShowBubble == true )
	{
		for( register int i =0; i< DEPTH_COUNT; i ++ )
		{
			if( FSellPxIndex[i] != -1 && FSellPxIndex[i] >= UpperTick )
			{
				Cy = UpperY + (FSellPxIndex[i] - UpperTick)* FPixelsPerTick + PixelsHelfTick;
				Cx = FillX + 3*PixelsHelfSecond;
				Radius = (FSellQty[i]+1)/2;
				DrawBubble( canvas , Cx, Cy, Radius, btBullBubble  );
			}
			if( FBuyPxIndex[i] != -1  && FBuyPxIndex[i] >= UpperTick )
			{
				Cy = UpperY + (FBuyPxIndex[i] - UpperTick)* FPixelsPerTick + PixelsHelfTick;
				Cx = FillX - PixelsHelfSecond;
				Radius = (FBuyQty[i]+1)/2;
				DrawBubble( canvas , Cx, Cy, Radius, btBearBubble  );
			}
		}
	}
	canvas->Brush->Style = bsClear;
	Formats.Clear();
	Formats <<tfSingleLine<<tfLeft<<tfVerticalCenter;
	canvas->Font->Color  = FAskDepth5Color;
	for( register int i =0; i< DEPTH_COUNT; i ++ )
	{
		if( FSellPxIndex[i] != -1 && FSellPxIndex[i] >= UpperTick )
		{
			PosY     = UpperY + (FSellPxIndex[i] - UpperTick)* FPixelsPerTick;
			TextRect = TRect( FillX + FPixelsPerGrid, PosY, FillX + 2*FPixelsPerGrid, PosY + FPixelsPerTick );
			QtyStr.printf( L"%d", FSellQty[i] );
			canvas->TextRect(TextRect,QtyStr,Formats );
		}
	}
	Formats.Clear();
	Formats <<tfSingleLine<<tfRight<<tfVerticalCenter;
	canvas->Font->Color  = FBidDepth5Color;
	for( register int i =0; i< DEPTH_COUNT; i ++ )
	{
		if( FBuyPxIndex[i] != -1  && FBuyPxIndex[i] >= UpperTick )
		{
			PosY     = UpperY + (FBuyPxIndex[i] - UpperTick)* FPixelsPerTick;
			TextRect = TRect( FillX - FPixelsPerGrid, PosY, FillX, PosY + FPixelsPerTick );
			QtyStr.printf( L"%d", FBuyQty[i] );
			canvas->TextRect(TextRect,QtyStr,Formats );
		}
	}
	canvas->Brush->Style = bsSolid;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawColorRect( TCanvas* canvas, const TRect& PaintRect, TColor BKColor, TColor LightColor, TColor DarkColor )
{
	TColor      OrgBrushColor = canvas->Brush->Color;
	TBrushStyle OrgBrushStyle = canvas->Brush->Style;
	TRect        DrawRect = PaintRect;

	canvas->Brush->Color = BKColor;
	canvas->Brush->Style = bsSolid;
	canvas->FillRect( DrawRect );
	Frame3D( canvas, DrawRect, LightColor, DarkColor, 2 );
	///< Restore properties.
	canvas->Brush->Color = OrgBrushColor;
	canvas->Brush->Style = OrgBrushStyle;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawColorRect( TDirect2DCanvas* canvas, const TRect& PaintRect, TColor BKColor, TColor LightColor, TColor DarkColor )
{
	TColor      OrgBrushColor = canvas->Brush->Color;
	TColor      OrgPenColor   = canvas->Pen->Color;
	TBrushStyle OrgBrushStyle = canvas->Brush->Style;
	int         OrgPenWidth   = canvas->Pen->Width;

	canvas->Brush->Color = BKColor;
	canvas->Brush->Style = bsSolid;
	canvas->FillRect( PaintRect );

	canvas->Pen->Width = 2;
	canvas->Pen->Color = LightColor;
	canvas->MoveTo(PaintRect.left,PaintRect.bottom );
	canvas->LineTo(PaintRect.left,PaintRect.top );
	canvas->LineTo(PaintRect.right,PaintRect.top );
	canvas->Pen->Color = DarkColor;
	canvas->LineTo(PaintRect.right-1,PaintRect.bottom-1 );
	canvas->LineTo(PaintRect.left,PaintRect.bottom-1 );
	///< Restore properties.
	canvas->Pen->Width = OrgPenWidth;
	canvas->Pen->Color = OrgPenColor;
	canvas->Brush->Color = OrgBrushColor;
	canvas->Brush->Style = OrgBrushStyle;
}
//------------------------------------------------------------------------------
 void __fastcall TStockChartEx::DrawBmpButton( TCanvas* canvas, const TRect& BtnRect, Graphics::TBitmap* BtnBmp )
{
	HRGN  BtnRgn;

	BtnRgn = CreateRoundRectRgn(  BtnRect.left,BtnRect.top,BtnRect.right,BtnRect.bottom, 20, 20 );
	SelectClipRgn( canvas->Handle, BtnRgn );
	if( BtnRect.Contains( FMousePos ) == true )///< Mouse over
		canvas->Draw( BtnRect.left, BtnRect.top - BtnBmp->Height/2, BtnBmp );
	else
		canvas->Draw( BtnRect.left, BtnRect.top, BtnBmp, 200 );
	SelectClipRgn( canvas->Handle, NULL );
	DeleteObject( BtnRgn );
}
//------------------------------------------------------------------------------
 void __fastcall TStockChartEx::DrawBmpButton( TDirect2DCanvas* canvas, const TRect& BtnRect, Graphics::TBitmap* BtnBmp, ID2D1RoundedRectangleGeometry* RoundRectangleGeometry, ID2D1Layer* ClipLayer )
{
	canvas->RenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(BtnRect.left, BtnRect.top));
	canvas->RenderTarget->PushLayer( D2D1::LayerParameters(D2D1::InfiniteRect(), RoundRectangleGeometry ), ClipLayer );
	if( BtnRect.Contains( FMousePos ) == true )///< Mouse over
		canvas->Draw( 0, - BtnBmp->Height/2, BtnBmp );
	else
		canvas->Draw( 0, 0, BtnBmp, 200 );
	canvas->RenderTarget->PopLayer();
	canvas->RenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawButtons( TCanvas* canvas )
{
	if( FCenterFilledBmp->Empty == false )
	{
		FCenterFilledRect.top    = 5;
		FCenterFilledRect.Bottom = FCenterFilledBmp->Height/2 + 5;
		FCenterFilledRect.Right  = Width - 5;
		FCenterFilledRect.left   = FCenterFilledRect.Right - FCenterFilledBmp->Width;
		DrawBmpButton( canvas, FCenterFilledRect, FCenterFilledBmp );
	}
	if( FIsLastTickInView == true && FReadOnly == false )
	{
		int OffsetX = GetLastTickLeft();//FLastTickPos.x - FLastTickPos.x % FPixelsPerGrid - FViewPortX;
		if( FCancelAllBuyBmp->Empty == false )
		{
			FCancelBuyRect.Bottom = Height - FPixelsPerTick -5;//FCancelAllBuyBmp->Height/2 + 10;
			FCancelBuyRect.top    = FCancelBuyRect.bottom - FCancelAllBuyBmp->Height/2;;
			FCancelBuyRect.right  = OffsetX - 5;
			FCancelBuyRect.left   = FCancelBuyRect.right - FCancelAllBuyBmp->Width;
			DrawBmpButton( canvas, FCancelBuyRect, FCancelAllBuyBmp );
		}
		if( FCancelAllSellBmp->Empty == false )
		{
			FCancelSellRect.Bottom = Height - FPixelsPerTick -5;//FCancelAllSellBmp->Height/2 + 10;
			FCancelSellRect.top    = FCancelSellRect.bottom - FCancelAllSellBmp->Height/2;;;
			FCancelSellRect.left   = OffsetX + FPixelsPerGrid + 5;
			FCancelSellRect.right  = FCancelSellRect.left + FCancelAllSellBmp->Width;
			DrawBmpButton( canvas, FCancelSellRect, FCancelAllSellBmp );
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawButtons( TDirect2DCanvas* canvas )
{
	if( FCenterFilledBmp->Empty == false )
	{
		FCenterFilledRect.top    = 5;
		FCenterFilledRect.Bottom = FCenterFilledBmp->Height/2 + 5;
		FCenterFilledRect.Right  = Width - 5;
		FCenterFilledRect.left   = FCenterFilledRect.Right - FCenterFilledBmp->Width;
		DrawBmpButton( canvas, FCenterFilledRect, FCenterFilledBmp, FCenterBtnGeometry, FCenterBtnClipLayer );
	}
	if( FIsLastTickInView == true && FReadOnly == false )
	{
		int OffsetX = GetLastTickLeft();//FLastTickPos.x - FLastTickPos.x % FPixelsPerGrid - FViewPortX;
		if( FCancelAllBuyBmp->Empty == false )
		{
			FCancelBuyRect.Bottom = Height - FPixelsPerTick -5;//FCancelAllBuyBmp->Height/2 + 10;
			FCancelBuyRect.top    = FCancelBuyRect.bottom - FCancelAllBuyBmp->Height/2;;
			FCancelBuyRect.right  = OffsetX - 5;
			FCancelBuyRect.left   = FCancelBuyRect.right - FCancelAllBuyBmp->Width;
			DrawBmpButton( canvas, FCancelBuyRect, FCancelAllBuyBmp, FCancelBuyBtnGeometry, FCancelBuyBtnClipLayer );
		}
		if( FCancelAllSellBmp->Empty == false )
		{
			FCancelSellRect.Bottom = Height - FPixelsPerTick -5;//FCancelAllSellBmp->Height/2 + 10;
			FCancelSellRect.top    = FCancelSellRect.bottom - FCancelAllSellBmp->Height/2;;;
			FCancelSellRect.left   = OffsetX + FPixelsPerGrid + 5;
			FCancelSellRect.right  = FCancelSellRect.left + FCancelAllSellBmp->Width;
			DrawBmpButton( canvas, FCancelSellRect, FCancelAllSellBmp, FCancelSellBtnGeometry, FCancelSellBtnClipLayer );
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawGradientBar( TDirect2DCanvas* canvas, TRect& dRect,ID2D1LinearGradientBrush* GradientBrush )
{
	D2D1_RECT_F DrawRect = D2D1::RectF( 0,0, dRect.Width(), dRect.Height() );

	canvas->RenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(dRect.left, dRect.top));
	canvas->RenderTarget->FillRectangle(&DrawRect, GradientBrush );
	canvas->RenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawGradientBar( HDC GDIDC, TRect& dRect,TColor UpCol, TColor DownCol, bool IsVert )
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
	if( IsVert == true )
		GradientFill( GDIDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V );
	else
		GradientFill( GDIDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_H );
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawGradientBar( TCanvas* canvas, TRect& dRect,TColor UpCol, TColor DownCol, bool IsVert )
{
	DrawGradientBar( canvas->Handle, dRect,UpCol, DownCol, IsVert );
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::MoveCellRectX( const TRect& InRect, TRect& OutRect, int Pos  )
{
	OutRect.top    = InRect.top;
	OutRect.bottom = InRect.bottom;
	OutRect.left   = InRect.left + Pos*FPixelsPerGrid;
	OutRect.right  = OutRect.left + FPixelsPerGrid;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawTickGrid( TCanvas* canvas )
{
	int             NextDeltaY  = FViewPortY%FPixelsPerTick;
	int             OffsetY     = -NextDeltaY;
	int             OffsetX     = 1;
	int             Ticks       = (FViewPortY/FPixelsPerTick); ///< Check div by 0 OK
	int             MaxY        = Height - FPixelsPerTick;
	float           PixelPerQty = (float)FPixelsPerGrid/(float)FFillPxQtyMax;///< Check div by 0 OK
	TRect           DrawRect,BarRect,CenterRect,TextOutRect;
	UFC::AnsiString QtyStr;
	String          PxStr;

	canvas->Pen->Width   = 1;
	canvas->Brush->Style = bsClear;
	if( FIsLastTickInView == true  )
		OffsetX = GetLastTickLeft();//FLastTickPos.x - FLastTickPos.x%FPixelsPerGrid - FViewPortX;
	while( OffsetY < MaxY )
	{   ///< Draw time H line.
		canvas->Pen->Color   = FGridColor;
		canvas->MoveTo(     0, OffsetY );
		canvas->LineTo( Width, OffsetY );
		///< Draw Price label.
		CenterRect  = TRect( OffsetX, OffsetY, OffsetX + FPixelsPerGrid, OffsetY + FPixelsPerTick );
		TextOutRect = TRect( OffsetX+6, OffsetY, OffsetX + FPixelsPerGrid - 3, OffsetY + FPixelsPerTick );
		if( Ticks < 0 )
		{
			OffsetY += FPixelsPerTick;
			Ticks ++;
			continue;
        }
		if( FIsLastTickInView == false )
		{
			if( FTicks != NULL )
			{
				FTicks->ToTNFormat( FTicks->GetPxFromIndex( Ticks, FDebugMode ), PxStr );
				if( Ticks < FRefPxIndex )
					canvas->Font->Color  = FTickUpperColor;
				else if( Ticks > FRefPxIndex )
					canvas->Font->Color  = FTickLowerColor;
				else
					canvas->Font->Color  = FFillColor;
				DrawTextExW( canvas->Handle, PxStr.c_str(),PxStr.Length(),&CenterRect, DT_SINGLELINE|DT_LEFT|DT_VCENTER, NULL);
            }
		}
		else
		{
			///< Draw depth(Buy side)
			if( FUserBuyQty[ Ticks ] != 0 )
			{
				int Radius = (FUserBuyQty[ Ticks ]+1)/2;

				MoveCellRectX( CenterRect, DrawRect, -2 );
				canvas->Brush->Style = bsClear;
				DrawBubble( canvas , DrawRect.left + DrawRect.Width()/2, DrawRect.top + DrawRect.Height()/2, Radius, btRefBubble  );
				QtyStr.Printf( "%d", FUserBuyQty[ Ticks ] );
				canvas->Font->Color  = FBuyColor;

				DrawTextExA( canvas->Handle,(char*)QtyStr.c_str(),QtyStr.Length(),&DrawRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
			}
			///< Draw depth(Sell side)
			if( FUserSellQty[ Ticks ] != 0 )
			{
				int Radius = (FUserSellQty[ Ticks ]+1)/2;

				MoveCellRectX( CenterRect, DrawRect, 2 );
				canvas->Brush->Style = bsClear;
				DrawBubble( canvas , DrawRect.left + DrawRect.Width()/2, DrawRect.top + DrawRect.Height()/2, Radius, btRefBubble  );
				QtyStr.Printf( "%d", FUserSellQty[ Ticks ] );
				canvas->Font->Color  = FSellColor;
				DrawTextExA( canvas->Handle, (char*)QtyStr.c_str(),QtyStr.Length(),&DrawRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
			}
			///< Draw tick
			if( FBetterStep != 0 &&  Ticks == FLastTick->FillPxIndex - FBetterStep )
			{   ///< Is a better Sell tick.
				canvas->Font->Color  = FBetterSellColor;
				DrawColorRect( canvas, CenterRect, FBetterSellBKColor, LighterColor(FBetterSellBKColor),DarkerColor(FBetterSellBKColor));
			}
			else if( FBetterStep != 0&& Ticks == FLastTick->FillPxIndex + FBetterStep )
			{   ///< Is a better Buy  tick.
				canvas->Font->Color  = FBetterBuyColor;
				DrawColorRect( canvas, CenterRect, FBetterBuyBKColor, LighterColor(FBetterBuyBKColor),DarkerColor(FBetterBuyBKColor));
			}
			else if( Ticks == FLastTick->FillPxIndex )
			{   ///< Is a Filled tick.
				canvas->Font->Color  = FFillColor;
				if( FChangeFillColor == true && Ticks <= FSellPxIndex[0] )
					DrawColorRect( canvas, CenterRect, clRed, LighterColor(clRed),DarkerColor(clRed));
				else if( FChangeFillColor == true && Ticks >= FBuyPxIndex[0] )
					DrawColorRect( canvas, CenterRect, clGreen, LighterColor(clGreen),DarkerColor(clGreen));
				else
					DrawColorRect( canvas, CenterRect, FFillBKColor, LighterColor(FFillBKColor),DarkerColor(FFillBKColor));
			}
			else if( Ticks < FLastTick->FillPxIndex ) ///< Upper filled tick.
			{
				canvas->Font->Color  = FTickUpperColor;
			}
			else ///< Lower filled tick.
			{
				canvas->Font->Color  = FTickLowerColor;
			}
			if( FTicks != NULL )
				FTicks->ToTNFormat( FTicks->GetPxFromIndex( Ticks, FDebugMode ), PxStr );
			if( FShowFillQty == true )
			{
				DrawTextExW( canvas->Handle,PxStr.c_str(),PxStr.Length(),&TextOutRect, DT_SINGLELINE|DT_LEFT|DT_VCENTER, NULL);
				if( Ticks == FLastTick->FillPxIndex )///< Prepare filled text.
				{
					PxStr.printf( L"%d", FLastTick->FillQty );
					DrawTextExW( canvas->Handle,PxStr.c_str(),PxStr.Length(),&TextOutRect, DT_SINGLELINE|DT_RIGHT|DT_VCENTER, NULL);
				}
			}
			else
			{
				DrawTextExW( canvas->Handle,PxStr.c_str(),PxStr.Length(),&TextOutRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
			}
			if( FShowFilled == true )
			{
				///< Draw filled qty(Buy side)
				if( FUserFilledBuyQty[ Ticks ] != 0 )
				{
					MoveCellRectX( CenterRect, DrawRect, -3 );
					QtyStr.Printf( "%d", FUserFilledBuyQty[ Ticks ] );
					canvas->Font->Color  = FBuyFillColor;
					DrawTextExA( canvas->Handle,(char*)QtyStr.c_str(),QtyStr.Length(),&DrawRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
				}
				///< Draw filled qty(Sell side)
				if( FUserFilledSellQty[ Ticks ] != 0 )
				{
					MoveCellRectX( CenterRect, DrawRect, 3 );
					QtyStr.Printf( "%d", FUserFilledSellQty[ Ticks ] );
					canvas->Font->Color  = FSellFillColor;
					DrawTextExA( canvas->Handle,(char*)QtyStr.c_str(),QtyStr.Length(),&DrawRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
				}
			}
			///< Draw filled qty bar
			if( FShowFillQty == true && FFillQtyArray[ Ticks ] != 0 )
			{
				int YShift = FPixelsPerTick/10;

				canvas->Font->Color  = FSellFillColor;
				MoveCellRectX( CenterRect, DrawRect, 4 );
				if( Ticks == FLastTick->FillPxIndex && FLastTick->FShowed == false)
				{
					FLastTick->FShowed = true;
					BarRect   = TRect( DrawRect.left , DrawRect.Top + YShift, DrawRect.right /*- 2*YShift*/, DrawRect.bottom - YShift );

					switch( TickType( FLastTick ) )
					{
						case btBearBubble: DrawGradientBar( canvas , BarRect, (TColor)RGB(255,0,0),(TColor)RGB(64,0,0) );break;
						case btBullBubble: DrawGradientBar( canvas , BarRect, (TColor)RGB(0,255,0),(TColor)RGB(0,64,0) );break;
						case btRefBubble:  DrawGradientBar( canvas , BarRect, (TColor)RGB(128,128,128),(TColor)RGB(64,64,64));break;
					}
					QtyStr.Printf( "%d", FLastTick->FillQty );
					DrawTextExA( canvas->Handle, (char*)QtyStr.c_str(),QtyStr.Length(),&DrawRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
				}
				else
				{
					int BarWidthDiff = PixelPerQty * FFillQtyArray[ Ticks ];

					BarRect   = TRect( DrawRect.left , DrawRect.Top + YShift, DrawRect.left + BarWidthDiff, DrawRect.bottom - YShift );
					DrawGradientBar( canvas, BarRect, (TColor)RGB(0,0,255),(TColor)RGB(0,0,64) );
					QtyStr.Printf( "%d", FFillQtyArray[ Ticks ] );
					DrawTextExA( canvas->Handle, (char*)QtyStr.c_str(),QtyStr.Length(),&DrawRect, DT_SINGLELINE|DT_LEFT|DT_VCENTER, NULL);
				}
			}
		}
		OffsetY += FPixelsPerTick;
		Ticks ++;
	}
	canvas->Brush->Style = bsSolid;
	if( FIsLastTickInView == true  )
	{
		Ticks    = (FViewPortY/FPixelsPerTick); ///< Check Div by 0 Ok!.
		OffsetY  = -NextDeltaY;
		OffsetX  = GetLastTickLeft();//FLastTickPos.x - FLastTickPos.x%FPixelsPerGrid - FViewPortX;
		DrawDepth( canvas, OffsetX, OffsetY, Ticks );
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawTickGrid( TDirect2DCanvas* canvas )
{
	int             NextDeltaY  = FViewPortY%FPixelsPerTick;
	int             OffsetY     = -NextDeltaY;
	int             OffsetX     = 1;
	int             Ticks       = (FViewPortY/FPixelsPerTick); ///< Check Div by 0 Ok!.
	int             MaxY        = Height - FPixelsPerTick;
	float           PixelPerQty = (float)FPixelsPerGrid/(float)FFillPxQtyMax;///< Check div by 0 OK
	TRect           DrawRect,BarRect,CenterRect,TextOutRect;
	String          PxStr,QtyStr;
	TTextFormat     Formats;

	canvas->Pen->Width   = 1;
	canvas->Brush->Style = bsClear;
	if( FIsLastTickInView == true  )
		OffsetX = GetLastTickLeft();//FLastTickPos.x - FLastTickPos.x%FPixelsPerGrid - FViewPortX;
	while( OffsetY < MaxY )
	{   ///< Draw time H line.
		canvas->Pen->Color   = FGridColor;
		canvas->MoveTo(     0, OffsetY );
		canvas->LineTo( Width, OffsetY );
		///< Draw Price label.
		CenterRect  = TRect( OffsetX, OffsetY, OffsetX + FPixelsPerGrid, OffsetY + FPixelsPerTick );
		TextOutRect = TRect( OffsetX+6, OffsetY, OffsetX + FPixelsPerGrid-3, OffsetY + FPixelsPerTick );
		if( Ticks < 0 )
		{
			OffsetY += FPixelsPerTick;
			Ticks ++;
			continue;
        }
		if( FIsLastTickInView == false )
		{
			if( FTicks != NULL )
			{
				FTicks->ToTNFormat( FTicks->GetPxFromIndex( Ticks, FDebugMode ), PxStr );
				if( Ticks < FRefPxIndex )
					canvas->Font->Color  = FTickUpperColor;
				else if( Ticks > FRefPxIndex )
					canvas->Font->Color  = FTickLowerColor;
				else
					canvas->Font->Color  = FFillColor;
				Formats.Clear();
				Formats <<tfSingleLine<<tfLeft<<tfVerticalCenter;
				canvas->TextRect( CenterRect,PxStr,Formats );
            }
		}
		else
		{
			///< Draw depth(Buy side)
			if( FUserBuyQty[ Ticks ] != 0 )
			{
				int Radius = (FUserBuyQty[ Ticks ]+1)/2;

				MoveCellRectX( CenterRect, DrawRect, -2 );
				DrawBubble( canvas , DrawRect.left + DrawRect.Width()/2, DrawRect.top + DrawRect.Height()/2, Radius, btRefBubble  );
				QtyStr.printf( L"%d", FUserBuyQty[ Ticks ] );
				Formats.Clear();
				Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
				canvas->Font->Color  = FBuyColor;
				canvas->TextRect( DrawRect,QtyStr,Formats );
			}
			///< Draw depth(Sell side)
			if( FUserSellQty[ Ticks ] != 0 )
			{
				int Radius = (FUserSellQty[ Ticks ]+1)/2;

				MoveCellRectX( CenterRect, DrawRect, 2 );
				DrawBubble( canvas , DrawRect.left + DrawRect.Width()/2, DrawRect.top + DrawRect.Height()/2, Radius, btRefBubble  );
				QtyStr.printf( L"%d", FUserSellQty[ Ticks ] );
				Formats.Clear();
				Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
				canvas->Font->Color  = FSellColor;
				canvas->TextRect( DrawRect,QtyStr,Formats );
			}
			///< Draw tick
			if( FBetterStep != 0 &&  Ticks == FLastTick->FillPxIndex - FBetterStep )
			{   ///< Is a better Sell tick.
				canvas->Font->Color  = FBetterSellColor;
				DrawColorRect( canvas, CenterRect, FBetterSellBKColor, LighterColor(FBetterSellBKColor),DarkerColor(FBetterSellBKColor));
			}
			else if( FBetterStep != 0&& Ticks == FLastTick->FillPxIndex + FBetterStep )
			{   ///< Is a better Buy  tick.
				canvas->Font->Color  = FBetterBuyColor;
				DrawColorRect( canvas, CenterRect, FBetterBuyBKColor, LighterColor(FBetterBuyBKColor),DarkerColor(FBetterBuyBKColor));
			}
			else if( Ticks == FLastTick->FillPxIndex )
			{   ///< Is a Filled tick.
				canvas->Font->Color  = FFillColor;
				if( FChangeFillColor == true && Ticks <= FSellPxIndex[0] )
					DrawColorRect( canvas, CenterRect, clRed, LighterColor(clRed),DarkerColor(clRed));
				else if( FChangeFillColor == true && Ticks >= FBuyPxIndex[0] )
					DrawColorRect( canvas, CenterRect, clGreen, LighterColor(clGreen),DarkerColor(clGreen));
				else
					DrawColorRect( canvas, CenterRect, FFillBKColor, LighterColor(FFillBKColor),DarkerColor(FFillBKColor));
			}
			else if( Ticks < FLastTick->FillPxIndex ) ///< Upper filled tick.
			{
				canvas->Font->Color  = FTickUpperColor;
			}
			else ///< Lower filled tick.
			{
				canvas->Font->Color  = FTickLowerColor;
			}
			if( FTicks != NULL )
				FTicks->ToTNFormat( FTicks->GetPxFromIndex( Ticks, FDebugMode ), PxStr );
			Formats.Clear();
			if( FShowFillQty == true )
			{
				Formats <<tfSingleLine<<tfLeft<<tfVerticalCenter;
				canvas->TextRect( TextOutRect, PxStr,Formats );
				if( Ticks == FLastTick->FillPxIndex )///< Prepare filled text.
				{
					PxStr.printf( L"%d", FLastTick->FillQty );
					Formats.Clear();
					Formats <<tfSingleLine<<tfRight<<tfVerticalCenter;
					canvas->TextRect( TextOutRect, PxStr,Formats );
				}
			}
			else
			{
				Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
				canvas->TextRect( TextOutRect, PxStr,Formats );
			}
			///< Draw filled qty
			if( FShowFilled == true )
			{
				///< Draw filled qty(Buy side)
				if( FUserFilledBuyQty[ Ticks ] != 0 )
				{
					MoveCellRectX( CenterRect, DrawRect, -3 );
					QtyStr.printf( L"%d", FUserFilledBuyQty[ Ticks ] );
					Formats.Clear();
					Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
					canvas->Font->Color  = FBuyFillColor;
					canvas->TextRect(DrawRect,QtyStr,Formats );
				}
				///< Draw filled qty(Sell side)
				if( FUserFilledSellQty[ Ticks ] != 0 )
				{
					MoveCellRectX( CenterRect, DrawRect, 3 );
					QtyStr.printf( L"%d", FUserFilledSellQty[ Ticks ] );
					Formats.Clear();
					Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
					canvas->Font->Color  = FSellFillColor;
					canvas->TextRect(DrawRect,QtyStr,Formats );
				}
			}
			///< Draw filled qty
			if( FShowFillQty == true && FFillQtyArray[ Ticks ] != 0 )
			{
				int YShift = FPixelsPerTick/10;

				canvas->Font->Color  = FSellFillColor;
				MoveCellRectX( CenterRect, DrawRect, 4 );
				if( Ticks == FLastTick->FillPxIndex && FLastTick->FShowed == false)
				{
					FLastTick->FShowed = true;
					BarRect   = TRect( DrawRect.left , DrawRect.Top + YShift, DrawRect.right, DrawRect.bottom - YShift );

					if( FLastTick->FillPxIndex <= FLastTick->SellPxIndex )
						DrawGradientBar( canvas , BarRect, FRedGradientBrush );
					else if( FLastTick->FillPxIndex >= FLastTick->SellPxIndex )
						DrawGradientBar( canvas , BarRect, FGreenGradientBrush );
					else
						DrawGradientBar( canvas , BarRect, FWhiteGradientBrush );
					QtyStr.printf( L"%d", FLastTick->FillQty );
					Formats.Clear();
					Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
					canvas->TextRect(DrawRect,QtyStr,Formats );
				}
				else
				{
					int BarWidthDiff = PixelPerQty * FFillQtyArray[ Ticks ];

					BarRect   = TRect( DrawRect.left , DrawRect.Top + YShift, DrawRect.left + BarWidthDiff, DrawRect.bottom - YShift );
					DrawGradientBar( canvas, BarRect, FBlueGradientBrush );
					QtyStr.printf( L"%d", FFillQtyArray[ Ticks ] );
					Formats.Clear();
					Formats <<tfSingleLine<<tfLeft<<tfVerticalCenter;
					canvas->TextRect(DrawRect,QtyStr,Formats );
				}
			}
		}
		OffsetY += FPixelsPerTick;
		Ticks ++;
	}
	canvas->Brush->Style = bsSolid;
	if( FIsLastTickInView == true  )
	{
		Ticks    = (FViewPortY/FPixelsPerTick);///< Check div by 0 OK
		OffsetY  = -NextDeltaY;
		OffsetX  = GetLastTickLeft();//FLastTickPos.x - FLastTickPos.x%FPixelsPerGrid - FViewPortX;
		DrawDepth( canvas, OffsetX, OffsetY, Ticks );
	}
}
//------------------------------------------------------------------------------
double  __fastcall TStockChartEx::GetTickPrice( int Step )
{
	int PxIndex  = FLastTick->FillPxIndex + Step;
	int IndexMax = FTicks->ItemCount()-1;

	if( PxIndex < 0 )
		PxIndex = 0;
	if( PxIndex > IndexMax )
		PxIndex = IndexMax;
	return FTicks->GetPxFromIndex( PxIndex, false );
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawMap( void )
{
	double TotalPxDiff = FMaxPrice-FMinPrice;
	if( FThunbDirty == true && TotalPxDiff != 0.0 ) ///< Dirty flag on. need repaint
	{
		TRect  MapRect( 0, 0, FMapBitmap->Width,FMapBitmap->Height );
		TPoint ViewPortCenter( FViewPortX + Width/2, FViewPortY + Height/2 );
		double PixelsPerPrice = (float)FMapBitmap->Height / TotalPxDiff;
		int    UpperY,LowerY,CloseY,XOffset;
		int    RefTime, TenMS, Tick, ViewX, ViewY, TextH;
		int    TimeStep = 30* FThumbHRatio;
		String TimeStr;
		UFC::AnsiString Time;

		FThunbDirty = false; ///< Turn off dirty flag.
		///< Draw background with white frame
		FMapBitmap->Canvas->Brush->Color = clBlack;
		FMapBitmap->Canvas->Pen->Color   = clWhite;
		FMapBitmap->Canvas->Rectangle( MapRect );
		///< Draw time grid
		FMapBitmap->Canvas->Pen->Color  = clGray;
		FMapBitmap->Canvas->Pen->Style  = psSolid;
		FMapBitmap->Canvas->Font->Name  = L"Tahoma";
		FMapBitmap->Canvas->Font->Color = clWhite;
		FMapBitmap->Canvas->Font->Size  = 8;
		TextH = FMapBitmap->Canvas->TextHeight( L"09:12345678" ) + 2;

		for( register int i = 60 - FOpenMinutes; i < MARKET_MINUTES; i+= TimeStep )
		{
			int X = ((float)i / FThumbHRatio);
			FMapBitmap->Canvas->MoveTo( X, 1 );
			FMapBitmap->Canvas->LineTo( X, FMapBitmap->Height -1 );
			if(	FMapBitmap->Canvas->Pen->Style == psSolid )
			{
				FExchangeInfo->IndexToTime( i, Time );
				FMapBitmap->Canvas->TextOut( X + 2, FMapBitmap->Height - TextH, Time.c_str() );
				FMapBitmap->Canvas->Pen->Style = psDot;
			}
			else
				FMapBitmap->Canvas->Pen->Style = psSolid;
		}
		///< Draw Reference Px Line.
		FMapBitmap->Canvas->Pen->Style = psSolid;
		UpperY = (int)((FMaxPrice - FRefPrice )*PixelsPerPrice);
		FMapBitmap->Canvas->MoveTo( 1 , UpperY );
		FMapBitmap->Canvas->LineTo( FMapBitmap->Width - 1 , UpperY );
		///< Draw cross.
		VirtualScreenToTimeTick( RefTime, TenMS, Tick, ViewPortCenter );
		ViewX = RefTime/(60*FThumbHRatio);
		ViewY = (int)((FMaxPrice - FTicks->GetPxFromIndex( Tick, FDebugMode ) )*PixelsPerPrice);
		FMapBitmap->Canvas->Pen->Color = clYellow;
		FMapBitmap->Canvas->MoveTo( ViewX, 1 );
		FMapBitmap->Canvas->LineTo( ViewX, FMapBitmap->Height - 1 );
		FMapBitmap->Canvas->MoveTo( 1, ViewY );
		FMapBitmap->Canvas->LineTo( FMapBitmap->Width - 1, ViewY );
		///< Draw lines
		for( register int x = 0; x < FMapBitmap->Width; x++ )
		{
			 int index = ((float)x*FThumbHRatio);
			 if( FHaveData[ index ] == true )
			 {
				 if( FOpenPx[index] <= FClosePx[index] ) ///< Price increasing
					 FMapBitmap->Canvas->Pen->Color = clRed;
				 else
					 FMapBitmap->Canvas->Pen->Color = clLime;
				 UpperY = (int)((FMaxPrice - FMaxPx[index]   )*PixelsPerPrice);
				 LowerY = (int)((FMaxPrice - FMinPx[index]   )*PixelsPerPrice);
				 CloseY = (int)((FMaxPrice - FClosePx[index] )*PixelsPerPrice);
				 FMapBitmap->Canvas->MoveTo( x , LowerY );
				 FMapBitmap->Canvas->LineTo( x , UpperY );
				 FMapBitmap->Canvas->MoveTo( x , CloseY );
				 FMapBitmap->Canvas->LineTo( x + 1, CloseY );
			  }
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::CenterView( int X, int Y )
{
	double YRatio  = (double)((FMinPxIndex-FMaxPxIndex)*FPixelsPerTick)/(double)FMapBitmap->Height;
	double XRatio  = (double)FVirtualWidth / (float)FMapBitmap->Width;
	int    MinPxY  = (int)( (double)(FTicks->ItemCount() - FMinPxIndex) * FPixelsPerTick );

	FViewPortX = (int)((double)X*XRatio) - Width/2;
	FViewPortY = FVirtualHeight - ((int)((double)(FMapBitmap->Height-Y)*YRatio) + MinPxY) - Height/2;
	FThunbDirty = true;
	Paint();
}
//------------------------------------------------------------------------------
int __fastcall TStockChartEx::FindRefSecIndex( int RefSec )
{
	register int First = 0; ///< First Tick
	register int Last  = FTicksList.ItemCount()  - 1;///< Last tick point.
	register int Half  = (Last - First)>>1;
	register TTickInfo* HalfTick;

	///< Reference time Smaller than the first object.
	if( RefSec < ((TTickInfo*)FTicksList.GetItem( First ))->RefSec )
		return First;
	///< Reference time Larger than the Last object.
	if( RefSec > ((TTickInfo*)FTicksList.GetItem( Last ))->RefSec)
		return Last;
	for(;;)
	{
		HalfTick = (TTickInfo*)FTicksList.GetItem( Half );
		if( RefSec > HalfTick->RefSec )     ///< Larger than the middle object.
			First = Half;
		else if( RefSec < HalfTick->RefSec )///< Smaller than the middle object.
			Last  = Half;
		else                 ///< Bingo! Same as the middle object.
			return Half;
		Half  = First + ((Last - First)>>1);
		if( First == Half || Last == Half )
			return Last;
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::FindPointsRange( int BeginSec, int EndSec, int& BeginIndex, int& EndIndex )
{
	BeginIndex = 0;
	EndIndex   = FTicksList.ItemCount() - 1;///< Last tick point.

	if( EndIndex > HALF_POINT_ARRAY_SIZE ) ///< Don't waste time to find bounds if points less than HALF_POINT_ARRAY_SIZE.
	{
		BeginIndex = FindRefSecIndex( BeginSec );
		EndIndex   = FindRefSecIndex( EndSec );
	}
}
//------------------------------------------------------------------------------
TBubbleType __fastcall TStockChartEx::TickType( TTickInfo* pTick, int RefPxIndex )
{
	if( pTick->FillPxIndex <= FRefPxIndex )
		return btBullBubble;
	else if( pTick->FillPxIndex >= FRefPxIndex )
		return btBearBubble;
	else
		return btRefBubble;
}
//------------------------------------------------------------------------------
TBubbleType __fastcall TStockChartEx::TickType( TTickInfo* pTick )
{
	if( pTick->FillPxIndex <= pTick->SellPxIndex )
		return btBullBubble;
	else if( pTick->FillPxIndex >= pTick->BuyPxIndex )
		return btBearBubble;
	else
		return btRefBubble;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawFillBubble( TCanvas* canvas, TTickInfo* pTick, int Cx,int Cy )
{
	int Radius = ( pTick->FillQty + 1 )/2;

	if( Radius > 1 )
	{
		if( pTick->SellPxIndex == -1 || pTick->BuyPxIndex == -1 )
			DrawBubble( canvas , Cx, Cy, Radius, TickType( pTick, FRefPxIndex ), 3  );
		else
			DrawBubble( canvas , Cx, Cy, Radius, TickType( pTick ), 3  );
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawPoints( TCanvas* canvas )
{
	int    BeginSec,BeginMS,EndSec,EndMS;
	int    BeginIndex,EndIndex,TotalPoints;
	int    PointCount = 0;
	TPoint ScrPos;
	TTickInfo* pTick;

	VirtualScreenXToTime( BeginSec, BeginMS, FViewPortX );
	VirtualScreenXToTime( EndSec,   EndMS,   FViewPortX + Width );
	FindPointsRange( BeginSec, EndSec, BeginIndex, EndIndex );
	///< Draw Bollinger Bands
	if( FBollingerBands == true && FBollingerSec > 0  )
		DrawBollingerBands( canvas, BeginSec, EndSec, FBollingerSec );
	///< Draw Bubble and tick lines.
	if( FShowTickLine == true ||  FShowBubble == true )
	{
		if( (pTick = (TTickInfo*)FTicksList.GetItem( BeginIndex )) != NULL )
		{
			TTickInfo AvgTick( *pTick );
			register int i;
			register int TicksPerSec = 1;

			for( i = BeginIndex + 1; i <= EndIndex; i++ )
			{
				if( (pTick = (TTickInfo*)FTicksList.GetItem( i )) != NULL )
				{
					if( pTick->RefSec != AvgTick.RefSec )
					{
						AvgTick.Average( TicksPerSec );
						TimeTickToWindow( AvgTick.RefSec, 0, AvgTick.FillPxIndex, ScrPos );
						FBezierPoints[ PointCount ] = ScrPos;
						PointCount++;
						if( AvgTick.FillQty >= ENABLE_LOG2_VAL )
							FEnableLog2 = true;
						if( FShowBubble == true )
							DrawFillBubble( canvas, &AvgTick, ScrPos.x, ScrPos.y );
						if( PointCount >= POINT_ARRAY_SIZE )
							break;
						AvgTick = *pTick;
						TicksPerSec = 1;
					}
					else
					{
						AvgTick.Accumulate( *pTick );
						TicksPerSec++;
					}
				}
			}
		}
		if( FShowTickLine == true )
		{
			canvas->Pen->Color = FGridColor;
			canvas->Polyline( FBezierPoints, PointCount -1 );
		}
	}
	///< Draw MA lines
	if( FShowMA == true )
	{
		if(	FMA1Sec > 0 )
			DrawSecMA( canvas, BeginSec, EndSec, FMA1Sec, FMA1Color );
		if( FMA2Sec > 0 )
			DrawSecMA( canvas, BeginSec, EndSec, FMA2Sec, FMA2Color );
		if(	FMA3Sec > 0 )
			DrawSecMA( canvas, BeginSec, EndSec, FMA3Sec, FMA3Color );
	}
	///< Draw create position points.
	if( FPositionPoints->Count > 0 )
		DrawPositionPoints( canvas, BeginSec, EndSec );
	///< Draw Standard deviation curve.
	if( FShwStdDev == true )
		DrawStdDev( canvas, BeginSec, EndSec );
	///< Draw Second qty bar.
	if(	FShowSecQty == true )
		DrawSecQtyBars( canvas, BeginSec, EndSec );
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawFillBubble( TDirect2DCanvas* canvas, TTickInfo* pTick, int Cx,int Cy )
{
	int Radius = ( pTick->FillQty + 1 )/2;

	if( Radius > 1 )
	{
		if( pTick->SellPxIndex == -1 || pTick->BuyPxIndex == -1 )
			DrawBubble( canvas , Cx, Cy, Radius, TickType( pTick, FRefPxIndex ), 3 );
		else
			DrawBubble( canvas , Cx, Cy, Radius, TickType( pTick ), 3  );
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawPoints( TDirect2DCanvas* canvas )
{
	int    BeginSec,BeginMS,EndSec,EndMS;
	int    BeginIndex,EndIndex,TotalPoints;
	int    PointCount = 0;
	TPoint ScrPos;
	TTickInfo* pTick;

	VirtualScreenXToTime( BeginSec, BeginMS, FViewPortX );
	VirtualScreenXToTime( EndSec,   EndMS,   FViewPortX + Width );
	FindPointsRange( BeginSec, EndSec, BeginIndex, EndIndex );
	///< Draw Bollinger Bands
	if( FBollingerBands == true && FBollingerSec > 0  )
		DrawBollingerBands( canvas, BeginSec, EndSec, FBollingerSec );
	///< Draw Bubble and tick lines.
	if( FShowTickLine == true || FShowBubble == true  )
	{
		if( (pTick = (TTickInfo*)FTicksList.GetItem( BeginIndex )) != NULL )
		{
			TTickInfo AvgTick( *pTick );
			register int i;
			register int TicksPerSec = 1;

			for( i = BeginIndex + 1; i <= EndIndex; i++ )
			{
				if( (pTick = (TTickInfo*)FTicksList.GetItem( i )) != NULL )
				{
					if( pTick->RefSec != AvgTick.RefSec )
					{
						AvgTick.Average( TicksPerSec );
						TimeTickToWindow( AvgTick.RefSec, 0, AvgTick.FillPxIndex, ScrPos );
						FBezierPoints[ PointCount ] = ScrPos;
						PointCount++;
						if( AvgTick.FillQty >= ENABLE_LOG2_VAL )
							FEnableLog2 = true;
						if( FShowBubble == true )
							DrawFillBubble( canvas, &AvgTick, ScrPos.x, ScrPos.y );
						if( PointCount >= POINT_ARRAY_SIZE )
							break;
						AvgTick = *pTick;
						TicksPerSec = 1;
					}
					else
					{
						AvgTick.Accumulate( *pTick );
						TicksPerSec++;
					}
				}
			}
		}
		if( FShowTickLine == true )
		{
			canvas->Pen->Color = FGridColor;
			canvas->Polyline( FBezierPoints, PointCount -1 );
		}
	}
	///< Draw MA lines
	if( FShowMA == true )
	{
		if(	FMA1Sec > 0 )
			DrawSecMA( canvas, BeginSec, EndSec, FMA1Sec, FMA1Color );
		if( FMA2Sec > 0 )
			DrawSecMA( canvas, BeginSec, EndSec, FMA2Sec, FMA2Color );
		if(	FMA3Sec > 0 )
			DrawSecMA( canvas, BeginSec, EndSec, FMA3Sec, FMA3Color );
	}
	///< Draw create position points.
	if( FPositionPoints->Count > 0 )
		DrawPositionPoints( canvas, BeginSec, EndSec );
	///< Draw Standard deviation curve.
	if( FShwStdDev == true )
		DrawStdDev( canvas, BeginSec, EndSec );
	///< Draw Second qty bar.
	if(	FShowSecQty == true )
		DrawSecQtyBars( canvas, BeginSec, EndSec );
}
//------------------------------------------------------------------------------
int  __fastcall TStockChartEx::FindQtyMax( int BeginSec, int EndSec )
{
	register int QtyMax = 0;
	register int CurQty;

	for( register int i = BeginSec; i< EndSec; i ++ )
	{
		CurQty = FGridQtys[ i ];
		if( CurQty > QtyMax )
			QtyMax = CurQty;
	}
	return QtyMax;
}
//------------------------------------------------------------------------------
void  __fastcall TStockChartEx::DrawStdDev( TCanvas* canvas, int BeginSec, int EndSec )
{
	int RefSec;
	double StdDev,MAPx;
	double StdDevMax = 0.0001f;
	double StdDevs[ HALF_POINT_ARRAY_SIZE ];

	if( FBollingerSec == 0 )
		return;
	if( EndSec > FLastTick->RefSec )
		EndSec = FLastTick->RefSec;
	if( BeginSec - FBollingerSec < 0 )
		BeginSec = FBollingerSec;
	if( BeginSec < EndSec  )
	{
		int TotalPoints = EndSec - BeginSec + 1;

		if( TotalPoints > HALF_POINT_ARRAY_SIZE )
		{
			TotalPoints = HALF_POINT_ARRAY_SIZE;
			BeginSec    = EndSec + 1 - TotalPoints;
		}
		///< Get all Bollinger Bands points
		for( register int i = 0; i < TotalPoints; i++ )
		{
			RefSec = i + BeginSec;
			GetThreePxForBollingerBands( RefSec - FBollingerSec + 1, RefSec, MAPx, StdDev );
			StdDevs[ i ] = StdDev;
			if( StdDev > StdDevMax )
				StdDevMax = StdDev;
		}
		double QtyToPixel = (double)Height/ (StdDevMax*3.0);
		int    PointCount = 0;

		for( register int i = BeginSec; i< EndSec; i ++ )
		{
			FBezierPoints[ PointCount].x = (int)((double)i*FPixelsPerSecond) - FViewPortX;
			FBezierPoints[ PointCount].y = Height - (int)( StdDevs[ PointCount ]*QtyToPixel);
			PointCount++;
		}
		FBezierPoints[ PointCount ] = TPoint( FBezierPoints[ PointCount - 1 ].x, Height );
		PointCount ++;
		FBezierPoints[ PointCount ] = TPoint( 0, Height );
		PointCount ++;
		FBezierPoints[ PointCount ] = FBezierPoints[ 0 ];
		canvas->Pen->Color   = clYellow;
//		canvas->Brush->Style = bsClear;
		canvas->Brush->Style = bsSolid;
		canvas->Polygon( FBezierPoints, PointCount -1 );
	}
}
//------------------------------------------------------------------------------
void  __fastcall TStockChartEx::DrawStdDev( TDirect2DCanvas* canvas, int BeginSec, int EndSec )
{
	int RefSec;
	double StdDev,MAPx;
	double StdDevMax = 0.0001f;
	double StdDevs[ HALF_POINT_ARRAY_SIZE ];
	ID2D1SolidColorBrush* CurBrush;

	if( FBollingerSec == 0 )
		return;
	if( EndSec > FLastTick->RefSec )
		EndSec = FLastTick->RefSec;
	if( BeginSec - FBollingerSec < 0 )
		BeginSec = FBollingerSec;
	if( BeginSec < EndSec  )
	{
		int TotalPoints = EndSec - BeginSec + 1;

		if( TotalPoints > HALF_POINT_ARRAY_SIZE )
		{
			TotalPoints = HALF_POINT_ARRAY_SIZE;
			BeginSec    = EndSec + 1 - TotalPoints;
		}
		///< Get all Bollinger Bands points
		for( register int i = 0; i < TotalPoints; i++ )
		{
			RefSec = i + BeginSec;
			GetThreePxForBollingerBands( RefSec - FBollingerSec + 1, RefSec, MAPx, StdDev );
			StdDevs[ i ] = StdDev;
			if( StdDev > StdDevMax )
				StdDevMax = StdDev;
		}
		double QtyToPixel = (double)Height/ (StdDevMax*3.0);
		int    PointCount = 0;

		for( register int i = BeginSec; i< EndSec; i ++ )
		{
			FBezierPoints[ PointCount].x = (int)((double)i*FPixelsPerSecond) - FViewPortX;
			FBezierPoints[ PointCount].y = Height - (int)( StdDevs[ PointCount ]*QtyToPixel);
			PointCount++;
		}
		FBezierPoints[ PointCount ] = TPoint( FBezierPoints[ PointCount - 1 ].x, Height );
		PointCount ++;
		FBezierPoints[ PointCount ] = TPoint( 0, Height );
		PointCount ++;
		FBezierPoints[ PointCount ] = FBezierPoints[ 0 ];
		canvas->Pen->Color   = clYellow;
		canvas->Brush->Style = bsSolid;
		canvas->Brush->Handle->QueryInterface( &CurBrush );
		CurBrush->SetColor( D2D1::ColorF( 1.0f,1.0f,0.0f, 0.2f));
		canvas->Polygon( FBezierPoints, PointCount -1 );
		CurBrush->Release();
	}
}
//------------------------------------------------------------------------------
void  __fastcall TStockChartEx::DrawSecQtyBars( TDirect2DCanvas* canvas, int BeginSec, int EndSec )
{
	int QtyMax = FindQtyMax( BeginSec, EndSec );
	if( QtyMax > 0 )
	{
		double QtyToPixel = (double)Height/ ((double)QtyMax*3.0);
		int    BarX,BarY;
		int    BarWidth = FPixelsPerGrid / FSecondsPerGrid; ///< Check Div by 0 OK

		if( BarWidth < 1 )
			BarWidth = 1;
		canvas->Brush->Style = bsSolid;
		canvas->Brush->Color = clRed;
		canvas->Pen->Color   = clRed;
		for( register int i = BeginSec; i< EndSec; i ++ )
		{
			BarX = (int)((double)i*FPixelsPerSecond) - FViewPortX;
			BarY = Height - FGridQtys[i]*QtyToPixel;
			TRect BarRect( BarX, BarY ,BarX+BarWidth, Height );
			canvas->FillRect( BarRect );
		}

	}
}
//------------------------------------------------------------------------------
void  __fastcall TStockChartEx::DrawSecQtyBars( TCanvas* canvas, int BeginSec, int EndSec )
{
	int QtyMax = FindQtyMax( BeginSec, EndSec );
	if( QtyMax > 0 )
	{
		double QtyToPixel = (double)Height/ ((double)QtyMax*3.0);
		int    BarX,BarY;
		int    BarWidth = FPixelsPerGrid / FSecondsPerGrid;  ///< Check Div by 0 OK

		if( BarWidth < 1 )
			BarWidth = 1;
		canvas->Brush->Style = bsSolid;
		canvas->Brush->Color = clRed;
		canvas->Pen->Color   = clRed;
		for( register int i = BeginSec; i< EndSec; i ++ )
		{
			BarX = (int)((double)i*FPixelsPerSecond) - FViewPortX;
			BarY = Height - FGridQtys[i]*QtyToPixel;
			TRect BarRect( BarX, BarY ,BarX+BarWidth, Height );
			canvas->FillRect( BarRect );
		}
	}
}
//------------------------------------------------------------------------------
void  __fastcall TStockChartEx::DrawPositionPoints( TDirect2DCanvas* canvas, int BeginSec, int EndSec )
{
	String          OutStr;
	TPoint          ScrPos;
	TPositionPoint* PosPoint;

	canvas->Pen->Color   = clYellow;
	canvas->Font->Color  = clYellow;
	canvas->Brush->Style = bsClear;
	for( register int i = 0; i < FPositionPoints->Count; i ++ )
	{
		 if( (PosPoint = (TPositionPoint*)FPositionPoints->Items[i]) != NULL )
		 {
			 if( PosPoint->FRefSec >= BeginSec && PosPoint->FRefSec <= EndSec )
			 {
				if( FSelectPP == i )
				{
					canvas->Pen->Color   = clWhite;
					canvas->Font->Color  = clWhite;
				}
				TimeTickToWindow( PosPoint->FRefSec, PosPoint->F10ms, PosPoint->FFillTickIndex, ScrPos );
				canvas->MoveTo( ScrPos.x -3,ScrPos.y);
				canvas->LineTo( ScrPos.x +3,ScrPos.y);
				canvas->MoveTo( ScrPos.x ,ScrPos.y-3);
				canvas->LineTo( ScrPos.x ,ScrPos.y+3);
				OutStr.printf( L"%s [%d]lots@%0.*f", (PosPoint->FIsBuy?L"Buy":L"Sell"),PosPoint->FQty,FDecimalLocator,FTicks->GetPxFromIndex( PosPoint->FFillTickIndex, FDebugMode ) );
				canvas->TextOut( ScrPos.x +5, ScrPos.y -5, OutStr );
				if( FSelectPP == i )
				{
					canvas->Pen->Color   = clYellow;
					canvas->Font->Color  = clYellow;
				}
			 }
		 }
	}
	if( FCenterX !=  -1 ) ///< Draw center white cross.
	{
		int x = FCenterX - FViewPortX;
		int y = FCenterY - FViewPortY;

		canvas->Pen->Color   = clWhite;
		canvas->MoveTo( x -8,y);
		canvas->LineTo( x +8,y);
		canvas->MoveTo( x ,y-8);
		canvas->LineTo( x ,y+8);
	}
}
//------------------------------------------------------------------------------
void  __fastcall TStockChartEx::DrawPositionPoints( TCanvas* canvas, int BeginSec, int EndSec )
{
	String          OutStr;
	TPoint          ScrPos;
	TPositionPoint* PosPoint;

	canvas->Pen->Color   = clYellow;
	canvas->Font->Color  = clYellow;
	canvas->Brush->Style = bsClear;
	for( register int i = 0; i < FPositionPoints->Count; i ++ )
	{
		 if( (PosPoint = (TPositionPoint*)FPositionPoints->Items[i]) != NULL )
		 {
			 if( PosPoint->FRefSec >= BeginSec && PosPoint->FRefSec <= EndSec )
			 {
				if( FSelectPP == i )
				{
					canvas->Pen->Color   = clWhite;
					canvas->Font->Color  = clWhite;
				}
				TimeTickToWindow( PosPoint->FRefSec, PosPoint->F10ms, PosPoint->FFillTickIndex, ScrPos );
				canvas->MoveTo( ScrPos.x -3,ScrPos.y);
				canvas->LineTo( ScrPos.x +3,ScrPos.y);
				canvas->MoveTo( ScrPos.x ,ScrPos.y-3);
				canvas->LineTo( ScrPos.x ,ScrPos.y+3);
				OutStr.printf( L"%s [%d]lots@%0.*f", (PosPoint->FIsBuy?L"Buy":L"Sell"),PosPoint->FQty,FDecimalLocator,FTicks->GetPxFromIndex( PosPoint->FFillTickIndex, FDebugMode ) );
				canvas->TextOut( ScrPos.x +5, ScrPos.y -5, OutStr );
				if( FSelectPP == i )
				{
					canvas->Pen->Color   = clYellow;
					canvas->Font->Color  = clYellow;
				}
			 }
		 }
	}
	if( FCenterX !=  -1 ) ///< Draw center white cross.
	{
		int x = FCenterX - FViewPortX;
		int y = FCenterY - FViewPortY;

		canvas->Pen->Color   = clWhite;
		canvas->MoveTo( x -8,y);
		canvas->LineTo( x +8,y);
		canvas->MoveTo( x ,y-8);
		canvas->LineTo( x ,y+8);
	}
}
//------------------------------------------------------------------------------
void  __fastcall TStockChartEx::GetThreePxForBollingerBands( int BeginSec, int EndSec, double& Mean, double& StdDev )
{
	double PowerSum = 0;
	double Diff,SecPxSubTotal;
	int    QtyTotal   = 0;
	int    PxSubTotal = 0;
	int    PxCount = 0;
	register int SecQty;

	for( register int i = BeginSec; i <= EndSec; i++ )
	{
		if( (SecQty  = FGridQtys[i]) > 0 )
		{
			QtyTotal   += SecQty;
			PxSubTotal += FGridSubTotal[i];
			PxCount++;
		}
	}
	if( QtyTotal > 0 && PxCount > 0 )  ///< Check Div by 0 OK
	{
		double MeanIndex = (double)PxSubTotal/(double)QtyTotal;
		double StdDevIndex;

		Mean = IndexToPx( MeanIndex );
		for( register int i = BeginSec; i <= EndSec; i++ )
		{
			if( ( SecQty = FGridQtys[ i ] ) > 0 )///< Check Div by 0 OK
			{
				Diff = ((double)FGridSubTotal[i]/(double)SecQty) - MeanIndex;
				PowerSum += (Diff*Diff);
			}
		}
		StdDevIndex = sqrt( PowerSum/PxCount );
		StdDev      = fabs( IndexToPx( MeanIndex + StdDevIndex ) - Mean );
	}
}
//------------------------------------------------------------------------------
void  __fastcall TStockChartEx::DrawBollingerBands( TDirect2DCanvas* canvas, int BeginSec, int EndSec, int AvgSec )
{
	double StdDev, MAPx, LowerPx;
	int    TotalPoints,RefSec;
	bool   EnableBands1 = false;
	bool   EnableBands2 = false;
	bool   EnableBands3 = false;
	ID2D1SolidColorBrush* CurBrush;

	if( EndSec > FLastTick->RefSec )
		EndSec = FLastTick->RefSec;
	if( BeginSec - AvgSec < 0 )
		BeginSec = AvgSec;
	if( FBandsWidth  > 0.0f ) EnableBands1 = true;
	if( FBandsWidth2 > 0.0f ) EnableBands2 = true;
	if( FBandsWidth3 > 0.0f ) EnableBands3 = true;

	if( BeginSec < EndSec && (EnableBands1 == true || EnableBands2 == true ||EnableBands3 == true) )
	{
		TotalPoints = EndSec - BeginSec + 1;
		if( TotalPoints > HALF_POINT_ARRAY_SIZE )
		{
			TotalPoints = HALF_POINT_ARRAY_SIZE;
			BeginSec    = EndSec + 1 - TotalPoints;
		}
		///< Get all Bollinger Bands points
		for( register int i = 0; i < TotalPoints; i++ )
		{
			RefSec = i + BeginSec;
			GetThreePxForBollingerBands( RefSec - AvgSec + 1, RefSec, MAPx, StdDev );
			FMeanPxs[ i ]   = MAPx;
			FOffsetPxs[ i ] = StdDev;
			if( EnableBands3 == true )
			{
				TimeTickToWindow( RefSec, 50, (double)MAPx + FBandsWidth3*StdDev, FBezierPoints[ i ] );
				TimeTickToWindow( RefSec, 50, (double)MAPx - FBandsWidth3*StdDev, FBezierPoints[ 2*TotalPoints -i -1 ] );
			}
		}
		///< Draw Thired Bollinger Bands.
		if( EnableBands3 == true )
		{
			FBezierPoints[ TotalPoints + TotalPoints - 1 ] = FBezierPoints[0];
			canvas->Pen->Color = clBlue;
			canvas->Brush->Handle->QueryInterface( &CurBrush );
			CurBrush->SetColor( D2D1::ColorF( 0.5f,0.5f,1.0f, 0.3f));
			canvas->Polygon( FBezierPoints, 2*TotalPoints -1 );
			CurBrush->Release();
		}
		///< Draw Second Bollinger Bands.
		if( EnableBands2 == true )
		{
			for( register int i = 0; i < TotalPoints; i++ )
			{
				TimeTickToWindow( i + BeginSec, 50, (double)(FMeanPxs[ i ] + FBandsWidth2*FOffsetPxs[ i ]), FBezierPoints[ i ] );
				TimeTickToWindow( i + BeginSec, 50, (double)(FMeanPxs[ i ] - FBandsWidth2*FOffsetPxs[ i ]), FBezierPoints[ 2*TotalPoints -i -1 ] );
			}
			FBezierPoints[ TotalPoints + TotalPoints - 1 ] = FBezierPoints[0];
			canvas->Pen->Color = clBlue;
			canvas->Brush->Handle->QueryInterface( &CurBrush );
			CurBrush->SetColor( D2D1::ColorF( 0.5f,0.5f,1.0f, 0.3f));
			canvas->Polygon( FBezierPoints, 2*TotalPoints -1 );
			CurBrush->Release();
		}
		///< Draw First Bollinger Bands.
		if( EnableBands1 == true )
		{
			for( register int i = 0; i < TotalPoints; i++ )
			{
				TimeTickToWindow( i + BeginSec, 50, (double)(FMeanPxs[ i ] + FBandsWidth*FOffsetPxs[ i ]), FBezierPoints[ i ] );
				TimeTickToWindow( i + BeginSec, 50, (double)(FMeanPxs[ i ] - FBandsWidth*FOffsetPxs[ i ]), FBezierPoints[ 2*TotalPoints -i -1 ] );
			}
			FBezierPoints[ TotalPoints + TotalPoints - 1 ] = FBezierPoints[0];
			canvas->Pen->Color = clBlue;
			canvas->Brush->Handle->QueryInterface( &CurBrush );
			CurBrush->SetColor( D2D1::ColorF( 0.5f,0.5f,1.0f, 0.3f));
			canvas->Polygon( FBezierPoints, 2*TotalPoints -1 );
			CurBrush->Release();
		}
		///< Draw MA line
		if( AvgSec != FMA1Sec && AvgSec != FMA2Sec && AvgSec != FMA3Sec )
		{
			for( register int i = 0; i < TotalPoints; i++ )
				 TimeTickToWindow( i + BeginSec, 50, (double)FMeanPxs[ i ] , FBezierPoints[ i ] );
			canvas->Pen->Color = FBBMAColor;
			canvas->Polyline( FBezierPoints, TotalPoints -1 );
		}
	}
}
//------------------------------------------------------------------------------
void  __fastcall TStockChartEx::DrawBollingerBands( TCanvas* canvas, int BeginSec, int EndSec, int AvgSec )
{
	double StdDev, MAPx, LowerPx;
	int    TotalPoints,RefSec;
	bool   EnableBands1 = false;
	bool   EnableBands2 = false;
	bool   EnableBands3 = false;

	if( EndSec > FLastTick->RefSec )
		EndSec = FLastTick->RefSec;
	if( BeginSec - AvgSec < 0 )
		BeginSec = AvgSec;
	if( FBandsWidth  > 0.0f ) EnableBands1 = true;
	if( FBandsWidth2 > 0.0f ) EnableBands2 = true;
	if( FBandsWidth3 > 0.0f ) EnableBands3 = true;

	if( BeginSec < EndSec && (EnableBands1 == true || EnableBands2 == true ||EnableBands3 == true) )
	{
		TotalPoints = EndSec - BeginSec + 1;
		if( TotalPoints > HALF_POINT_ARRAY_SIZE )
		{
			TotalPoints = HALF_POINT_ARRAY_SIZE;
			BeginSec    = EndSec + 1 - TotalPoints;
		}
		///< Get all Bollinger Bands points
		for( register int i = 0; i < TotalPoints; i++ )
		{
			RefSec = i + BeginSec;
			GetThreePxForBollingerBands( RefSec - AvgSec + 1, RefSec, MAPx, StdDev );
			FMeanPxs[ i ]   = MAPx;
			FOffsetPxs[ i ] = StdDev;
			if( EnableBands3 == true )
			{
				TimeTickToWindow( RefSec, 50, (double)(MAPx + FBandsWidth3*StdDev), FBezierPoints[ i ] );
				TimeTickToWindow( RefSec, 50, (double)(MAPx - FBandsWidth3*StdDev), FBezierPoints[ 2*TotalPoints -i -1 ] );
			}
		}
		///< Draw Thired Bollinger Bands.
		if( EnableBands3 == true )
		{
			FBezierPoints[ TotalPoints + TotalPoints - 1 ] = FBezierPoints[0];
			canvas->Pen->Color = clAqua;
			canvas->Brush->Color = clBlue;
			canvas->Polygon( FBezierPoints, 2*TotalPoints -1 );
		}
		///< Draw Second Bollinger Bands.
		if( EnableBands2 == true )
		{
			for( register int i = 0; i < TotalPoints; i++ )
			{
				TimeTickToWindow( i + BeginSec, 50, (double)(FMeanPxs[ i ] + FBandsWidth2*FOffsetPxs[ i ]), FBezierPoints[ i ] );
				TimeTickToWindow( i + BeginSec, 50, (double)(FMeanPxs[ i ] - FBandsWidth2*FOffsetPxs[ i ]), FBezierPoints[ 2*TotalPoints -i -1 ] );
			}
			canvas->Pen->Color = clSkyBlue;
			canvas->Brush->Color = clBlue;
			canvas->Polygon( FBezierPoints, 2*TotalPoints -1 );
		}
		///< Draw First Bollinger Bands.
		if( EnableBands1 == true )
		{
			for( register int i = 0; i < TotalPoints; i++ )
			{
				TimeTickToWindow( i + BeginSec, 50, (double)(FMeanPxs[ i ] + FBandsWidth*FOffsetPxs[ i ]), FBezierPoints[ i ] );
				TimeTickToWindow( i + BeginSec, 50, (double)(FMeanPxs[ i ] - FBandsWidth*FOffsetPxs[ i ]), FBezierPoints[ 2*TotalPoints -i -1 ] );
			}
			FBezierPoints[ TotalPoints + TotalPoints - 1 ] = FBezierPoints[0];
			canvas->Pen->Color = clBlue;
			canvas->Brush->Color = clNavy;
			canvas->Polygon( FBezierPoints, 2*TotalPoints -1 );
		}
		///< Draw MA line
		if( AvgSec != FMA1Sec && AvgSec != FMA2Sec && AvgSec != FMA3Sec )
		{
			for( register int i = 0; i < TotalPoints; i++ )
				 TimeTickToWindow( i + BeginSec, 50, (double)FMeanPxs[ i ] , FBezierPoints[ i ] );
			canvas->Pen->Color = FBBMAColor;
			canvas->Polyline( FBezierPoints, TotalPoints -1 );
        }
	}
}
//------------------------------------------------------------------------------
void  __fastcall TStockChartEx::DrawSecMA( TDirect2DCanvas* canvas, int BeginSec, int EndSec, int AvgSec, TColor LineColor )
{
	int    SecQty;
	int    QtyTotal   = 0;
	int    PxSubTotal = 0;
	int    PointCount = 0;
	TPoint ScrPos;

	if( EndSec > FLastTick->RefSec )
		EndSec = FLastTick->RefSec;
	if( BeginSec - AvgSec < 0 )
		BeginSec = AvgSec;
	if( BeginSec < EndSec )
	{
		for( register int i = EndSec-AvgSec+1; i <= EndSec; i++ )
		{
			if( (SecQty = FGridQtys[ i ]) > 0 )
			{
				QtyTotal   += SecQty;
				PxSubTotal += FGridSubTotal[i];
			}
		}
		for( register int i = EndSec -1; i >= BeginSec; i-- )
		{
			if( i - AvgSec < 0 )
				break;
			if( (SecQty = FGridQtys[ i+1 ]) > 0 )///< Remove first data point.
			{
				QtyTotal   -= SecQty;
				PxSubTotal -= FGridSubTotal[i+1];
			}
			if( (SecQty = FGridQtys[ i-AvgSec+1 ]) > 0 )///< Add last data point.
			{
				QtyTotal   += SecQty;
				PxSubTotal += FGridSubTotal[i-AvgSec+1];
			}
			if( QtyTotal > 0 )
			{
				double MAPx = IndexToPx( (double) PxSubTotal/(double)QtyTotal );
				TimeTickToWindow( i, 50, (double)MAPx, FBezierPoints[ PointCount ] );
				PointCount++;
				if( PointCount >= POINT_ARRAY_SIZE )
					break;
			}
		}
		canvas->Pen->Color = LineColor;
		canvas->Polyline( FBezierPoints, PointCount -1 );
	}
}
//------------------------------------------------------------------------------
void  __fastcall TStockChartEx::DrawSecMA( TCanvas* canvas, int BeginSec, int EndSec, int AvgSec, TColor LineColor )
{
	int    SecQty;
	int    QtyTotal   = 0;
	int    PxSubTotal = 0;
	int    PointCount = 0;
	TPoint ScrPos;

	if( EndSec > FLastTick->RefSec )
		EndSec = FLastTick->RefSec;
	if( BeginSec - AvgSec < 0 )
		BeginSec = AvgSec;
	if( BeginSec < EndSec )
	{
		for( register int i = EndSec-AvgSec+1; i <= EndSec; i++ )
		{
			if( (SecQty = FGridQtys[ i ]) > 0 )
			{
				QtyTotal   += SecQty;
				PxSubTotal += FGridSubTotal[i];
			}
		}
		for( register int i = EndSec -1; i >= BeginSec; i-- )
		{
			if( i - AvgSec < 0 )
				break;
			if( (SecQty = FGridQtys[ i+1 ]) > 0 )///< Remove first data point.
			{
				QtyTotal   -= SecQty;
				PxSubTotal -= FGridSubTotal[i+1];
			}
			if( (SecQty = FGridQtys[ i-AvgSec+1 ]) > 0 )///< Add last data point.
			{
				QtyTotal   += SecQty;
				PxSubTotal += FGridSubTotal[i-AvgSec+1];
			}
			if( QtyTotal > 0 )
			{
				double MAPx = IndexToPx( (double)PxSubTotal/(double)QtyTotal );
				TimeTickToWindow( i, 50, (double)MAPx, FBezierPoints[ PointCount ] );
				PointCount++;
				if( PointCount >= POINT_ARRAY_SIZE )
					break;
			}
		}
		canvas->Pen->Color = LineColor;
		canvas->Polyline( FBezierPoints, PointCount -1 );
	}
}
//------------------------------------------------------------------------------
TColor  __fastcall TStockChartEx::DarkerColor( TColor OrigCol )
{
	int R = GetRValue( OrigCol )>> 1;
	int G = GetGValue( OrigCol )>> 1;
	int B = GetBValue( OrigCol )>> 1;
	return (TColor)RGB (R, G, B );
}
//------------------------------------------------------------------------------
TColor  __fastcall TStockChartEx::LighterColor( TColor OrigCol )
{
	int R = GetRValue( OrigCol )<< 1;
	int G = GetGValue( OrigCol )<< 1;
	int B = GetBValue( OrigCol )<< 1;
	if( R > 255 ) R = 255;
	if( G > 255 ) G = 255;
	if( B > 255 ) B = 255;
	return (TColor)RGB (R, G, B );
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawBackground(  TCanvas* canvas, TRect& BufferRect, bool IsFocus )
{
	if( FLastTick == NULL )
		return;
	TRect PaintRect;
	int   FillLeft  = GetLastTickLeft();

	if( FillLeft >= 0  && FillLeft < Width )
	{
		if( FReadOnly == true )
		{	///< Draw center price row
			if( IsFocus )
				canvas->Brush->Color = FTickBKColor;
			else
				canvas->Brush->Color = DarkerColor( FTickBKColor );
			PaintRect = TRect( FillLeft, 0, FillLeft + FPixelsPerGrid, Height );
			canvas->FillRect( PaintRect );
			///< Draw Filled row.
			if( IsFocus )
				canvas->Brush->Color = FBuyFillBKColor;
			else
				canvas->Brush->Color = DarkerColor( FBuyFillBKColor );
			PaintRect = TRect( 0, 0, FillLeft, Height );
			canvas->FillRect( PaintRect );
			if( FillLeft + FPixelsPerGrid < Width )
			{
				if( IsFocus )
					canvas->Brush->Color = FSellFillBKColor;
				else
					canvas->Brush->Color = DarkerColor( FSellFillBKColor );
				PaintRect = TRect( FillLeft + FPixelsPerGrid, 0, Width, Height );
				canvas->FillRect( PaintRect );
			}
		}
		else
		{	///< Draw center price row
			if( IsFocus )
				canvas->Brush->Color = FTickBKColor;
			else
				canvas->Brush->Color = DarkerColor( FTickBKColor );
			PaintRect = TRect( FillLeft, 0, FillLeft + FPixelsPerGrid, Height );
			canvas->FillRect( PaintRect );
			///< Draw "Place order" black row.
			if( IsFocus )
				canvas->Brush->Color = FBidDepth5BKColor;
			else
				canvas->Brush->Color = DarkerColor( FBidDepth5BKColor );
			PaintRect = TRect( FillLeft - FPixelsPerGrid, 0, FillLeft, Height );
			canvas->FillRect( PaintRect );
			if( IsFocus )
				canvas->Brush->Color = FAskDepth5BKColor;
			else
				canvas->Brush->Color = DarkerColor( FAskDepth5BKColor );
			PaintRect = TRect( FillLeft + FPixelsPerGrid, 0, FillLeft + 2*FPixelsPerGrid, Height );
			canvas->FillRect( PaintRect );
			///< Draw "on the way order" Navy row.
			if( IsFocus )
				canvas->Brush->Color = FBuyBKColor;
			else
				canvas->Brush->Color = DarkerColor( FBuyBKColor );
			PaintRect = TRect( FillLeft - 2*FPixelsPerGrid, 0, FillLeft - FPixelsPerGrid, Height );
			canvas->FillRect( PaintRect );
			if( IsFocus )
				canvas->Brush->Color = FSellBKColor;
			else
				canvas->Brush->Color = DarkerColor( FSellBKColor );
			PaintRect = TRect( FillLeft + 2*FPixelsPerGrid, 0, FillLeft + 3*FPixelsPerGrid, Height );
			canvas->FillRect( PaintRect );
			///< Draw Filled row.
			if( FillLeft - 2*FPixelsPerGrid > 0 )
			{
				if( IsFocus )
					canvas->Brush->Color = FBuyFillBKColor;
				else
					canvas->Brush->Color = DarkerColor( FBuyFillBKColor );
				PaintRect = TRect( 0, 0, FillLeft - 2*FPixelsPerGrid, Height );
				canvas->FillRect( PaintRect );
			}
			if( FillLeft + 3*FPixelsPerGrid < Width )
			{
				if( IsFocus )
					canvas->Brush->Color = FSellFillBKColor;
				else
					canvas->Brush->Color = DarkerColor( FSellFillBKColor );
				PaintRect = TRect( FillLeft + 3*FPixelsPerGrid, 0, Width, Height );
				canvas->FillRect( PaintRect );
			}
		}
	}
	else
	{
		if( FillLeft < 0 )
			canvas->Brush->Color = FSellFillBKColor;
		else
			canvas->Brush->Color = FBuyFillBKColor;
		if( !IsFocus )
			canvas->Brush->Color = DarkerColor( canvas->Brush->Color );
		canvas->FillRect( BufferRect );
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawBackground( TDirect2DCanvas* canvas, TRect& BufferRect, bool IsFocus )
{
	if( FLastTick == NULL )
		return;
	TRect PaintRect;
	int   FillLeft  = GetLastTickLeft();

	if( FillLeft >= 0  && FillLeft < Width )
	{
		if( FReadOnly == true )
		{	///< Draw center price row
			if( IsFocus )
				canvas->Brush->Color = FTickBKColor;
			else
				canvas->Brush->Color = DarkerColor( FTickBKColor );
			PaintRect = TRect( FillLeft, 0, FillLeft + FPixelsPerGrid, Height );
			canvas->FillRect( PaintRect );
			///< Draw Filled row.
			if( IsFocus )
				canvas->Brush->Color = FBuyFillBKColor;
			else
				canvas->Brush->Color = DarkerColor( FBuyFillBKColor );
			PaintRect = TRect( 0, 0, FillLeft, Height );
			canvas->FillRect( PaintRect );
			if( FillLeft + FPixelsPerGrid < Width )
			{
				if( IsFocus )
					canvas->Brush->Color = FSellFillBKColor;
				else
					canvas->Brush->Color = DarkerColor( FSellFillBKColor );
				PaintRect = TRect( FillLeft + FPixelsPerGrid, 0, Width, Height );
				canvas->FillRect( PaintRect );
			}
		}
		else
		{	///< Draw center price row
			if( IsFocus )
				canvas->Brush->Color = FTickBKColor;
			else
				canvas->Brush->Color = DarkerColor( FTickBKColor );
			PaintRect = TRect( FillLeft, 0, FillLeft + FPixelsPerGrid, Height );
			canvas->FillRect( PaintRect );
			///< Draw "Place order" black row.
			if( IsFocus )
				canvas->Brush->Color = FBidDepth5BKColor;
			else
				canvas->Brush->Color = DarkerColor( FBidDepth5BKColor );
			PaintRect = TRect( FillLeft - FPixelsPerGrid, 0, FillLeft, Height );
			canvas->FillRect( PaintRect );
			if( IsFocus )
				canvas->Brush->Color = FAskDepth5BKColor;
			else
				canvas->Brush->Color = DarkerColor( FAskDepth5BKColor );
			PaintRect = TRect( FillLeft + FPixelsPerGrid, 0, FillLeft + 2*FPixelsPerGrid, Height );
			canvas->FillRect( PaintRect );
			///< Draw "on the way order" Navy row.
			if( IsFocus )
				canvas->Brush->Color = FBuyBKColor;
			else
				canvas->Brush->Color = DarkerColor( FBuyBKColor );
			PaintRect = TRect( FillLeft - 2*FPixelsPerGrid, 0, FillLeft - FPixelsPerGrid, Height );
			canvas->FillRect( PaintRect );
			if( IsFocus )
				canvas->Brush->Color = FSellBKColor;
			else
				canvas->Brush->Color = DarkerColor( FSellBKColor );
			PaintRect = TRect( FillLeft + 2*FPixelsPerGrid, 0, FillLeft + 3*FPixelsPerGrid, Height );
			canvas->FillRect( PaintRect );
			///< Draw Filled row.
			if( FillLeft - 2*FPixelsPerGrid > 0 )
			{
				if( IsFocus )
					canvas->Brush->Color = FBuyFillBKColor;
				else
					canvas->Brush->Color = DarkerColor( FBuyFillBKColor );
				PaintRect = TRect( 0, 0, FillLeft - 2*FPixelsPerGrid, Height );
				canvas->FillRect( PaintRect );
			}
			if( FillLeft + 3*FPixelsPerGrid < Width )
			{
				if( IsFocus )
					canvas->Brush->Color = FSellFillBKColor;
				else
					canvas->Brush->Color = DarkerColor( FSellFillBKColor );
				PaintRect = TRect( FillLeft + 3*FPixelsPerGrid, 0, Width, Height );
				canvas->FillRect( PaintRect );
			}
		}
	}
	else
	{
		if( FillLeft < 0 )
			canvas->Brush->Color = FSellFillBKColor;
		else
			canvas->Brush->Color = FBuyFillBKColor;
		if( !IsFocus )
			canvas->Brush->Color = DarkerColor( canvas->Brush->Color );
		canvas->FillRect( BufferRect );
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DrawHeader( TCustomCanvas* canvas )
{
	if( FIsLastTickInView == true )
	{
		TRect HeaderRect( 0,0, FHeaderBitmap->Width, FHeaderBitmap->Height );
		TRect CellRect( 0,0,FPixelsPerGrid, FHeaderBitmap->Height );
		int   OffsetX;
		UFC::AnsiString QtyStr;

		if( FShowFilled == true )
			OffsetX = GetLastTickLeft() - 3*FPixelsPerGrid;
		else
			OffsetX = GetLastTickLeft() - 2*FPixelsPerGrid;
		DrawGradientBar( FHeaderBitmap->Canvas , HeaderRect, FFixedRowBKColor , clBlack );
		FHeaderBitmap->Canvas->Font->Size   = FFontSize;
		FHeaderBitmap->Canvas->Font->Color  = FFixedRowColor;
		FHeaderBitmap->Canvas->Pen->Color   = FGridColor;
		FHeaderBitmap->Canvas->Brush->Style = bsClear;
		///< Buy filled Qty
		if( FShowFilled == true )
		{
			QtyStr.Printf("%d",FTotalUserFilledBuyQty );
			DrawTextExA( FHeaderBitmap->Canvas->Handle, (char*)QtyStr.c_str(),QtyStr.Length(),&CellRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
			CellRect.Left = CellRect.right;
			CellRect.right +=FPixelsPerGrid;
		}
		///< Buy in orderbook
		QtyStr.Printf("%d",FTotalUserBuyQty );
		DrawTextExA( FHeaderBitmap->Canvas->Handle, (char*)QtyStr.c_str(),QtyStr.Length(),&CellRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
		CellRect.Left = CellRect.right;
		CellRect.right +=FPixelsPerGrid;
		///< Buy in market
		QtyStr.Printf("%d",FBuyTotalQty);
		DrawTextExA( FHeaderBitmap->Canvas->Handle, (char*)QtyStr.c_str(),QtyStr.Length(),&CellRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
		CellRect.Left = CellRect.right;
		CellRect.right +=FPixelsPerGrid;
		///< Price label
		int Total = FBuyTotalQty + FSellTotalQty;
		int BuyPercent;
		if( Total > 0 )
			BuyPercent = (int)(((float)FBuyTotalQty * 100) / ((float)Total));
		else
			BuyPercent = 50;
		int BuyWidth = FPixelsPerGrid*BuyPercent/100;
		TRect LeftRect( CellRect.left, CellRect.top, CellRect.left + BuyWidth, CellRect.bottom);
		TRect RightRect( CellRect.left + BuyWidth, CellRect.top, CellRect.right,  CellRect.bottom );

		DrawGradientBar( FHeaderBitmap->Canvas , LeftRect, clGreen, clBlack );
		DrawGradientBar( FHeaderBitmap->Canvas , RightRect, clRed, clBlack);
		QtyStr.Printf("%d:%d", BuyPercent,100 - BuyPercent );
		DrawTextExA( FHeaderBitmap->Canvas->Handle, (char*)QtyStr.c_str(),QtyStr.Length(),&CellRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
		CellRect.Left = CellRect.right;
		CellRect.right +=FPixelsPerGrid;
		///< Sell in market
		QtyStr.Printf("%d",FSellTotalQty);
		DrawTextExA( FHeaderBitmap->Canvas->Handle, (char*)QtyStr.c_str(),QtyStr.Length(),&CellRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
		CellRect.Left = CellRect.right;
		CellRect.right +=FPixelsPerGrid;
		///< Sell in orderbook
		QtyStr.Printf("%d",FTotalUserSellQty );
		DrawTextExA( FHeaderBitmap->Canvas->Handle, (char*)QtyStr.c_str(),QtyStr.Length(),&CellRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
		CellRect.Left = CellRect.right;
		CellRect.right +=FPixelsPerGrid;
		///< Sell filled Qty
		if( FShowFilled == true )
		{
			QtyStr.Printf("%d",FTotalUserFilledSellQty );
			DrawTextExA( FHeaderBitmap->Canvas->Handle, (char*)QtyStr.c_str(),QtyStr.Length(),&CellRect, DT_SINGLELINE|DT_CENTER|DT_VCENTER, NULL);
		}
		///< Draw grid line.
		FHeaderBitmap->Canvas->MoveTo( 0, 0 );
		FHeaderBitmap->Canvas->LineTo( FHeaderBitmap->Width, 0 );
		FHeaderBitmap->Canvas->MoveTo( 0, FHeaderBitmap->Height-1 );
		FHeaderBitmap->Canvas->LineTo( FHeaderBitmap->Width, FHeaderBitmap->Height-1  );
		int FieldCount = 5;
		if( FShowFilled == true )
			FieldCount = 7;
		for( int i = 0; i < FieldCount; i ++)
		{
			FHeaderBitmap->Canvas->MoveTo( FPixelsPerGrid*i, 0);
			FHeaderBitmap->Canvas->LineTo( FPixelsPerGrid*i, FHeaderBitmap->Height);
		}
		FHeaderBitmap->Canvas->MoveTo( FHeaderBitmap->Width-1, 0);
		FHeaderBitmap->Canvas->LineTo( FHeaderBitmap->Width-1, FHeaderBitmap->Height);
		///< Draw to canvas.
		canvas->Draw( OffsetX, 5, FHeaderBitmap );//, 200 );
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::PaintGDI( bool IsFocus )
{
	TRect BufferRect(0,0,Width,Height);
	TRect DrawRect( 0,0, Width, Height - FPixelsPerTick );
	HRGN  DrawRgn = CreateRectRgnIndirect( &DrawRect );

	DrawBackground( FBuffer->Canvas, BufferRect, IsFocus ); ///< Draw background.(setcolor OK!)
	SelectClipRgn( FBuffer->Canvas->Handle, DrawRgn );
		DrawPoints( FBuffer->Canvas );   ///< Draw tick points.(setcolor OK!)
		DrawTickGrid( FBuffer->Canvas ); ///< Draw grid lines.(setcolor OK!)
	SelectClipRgn( FBuffer->Canvas->Handle, NULL );
	DrawTimeGrid( FBuffer->Canvas );
	DrawHeader( FBuffer->Canvas ); ///< Draw row header.(setcolor OK!)
	DrawButtons( FBuffer->Canvas );
	DeleteObject( DrawRgn );
}
//------------------------------------------------------------------------------
bool __fastcall TStockChartEx::CreateD2DObjecsOnZoom( void )
{
	ID2D1GradientStopCollection *GradientStopCollection;
	D2D1_GRADIENT_STOP           GradientStops[2];

	SafeRelease( &FRedGradientBrush );
	SafeRelease( &FGreenGradientBrush );
	SafeRelease( &FBlueGradientBrush );
	SafeRelease( &FWhiteGradientBrush );
	SafeRelease( &FYellowBrush );
	///< Red gradient brush
	GradientStops[0].position = 0.0f;
	GradientStops[1].position = 1.0f;
	GradientStops[0].color = D2D1::ColorF(1,0,0, 1.0f );
	GradientStops[1].color = D2D1::ColorF(0.25f,0,0, 1.0f );
	if( FD2DCanvas->RenderTarget->CreateGradientStopCollection( GradientStops, 2,
				D2D1_GAMMA_2_2,	D2D1_EXTEND_MODE_CLAMP,	&GradientStopCollection ) != S_OK )
		return false;
	if( FD2DCanvas->RenderTarget->CreateLinearGradientBrush(
			D2D1::LinearGradientBrushProperties( D2D1::Point2F(0,0),D2D1::Point2F(0,0.8f*(float)FPixelsPerTick)),
			GradientStopCollection,&FRedGradientBrush ) != S_OK )
		return false;
	SafeRelease( &GradientStopCollection );
	///< Green gradient brush
	GradientStops[0].color = D2D1::ColorF(0,1,0, 1.0f );
	GradientStops[1].color = D2D1::ColorF(0,0.25f,0,1.0f );
	if( FD2DCanvas->RenderTarget->CreateGradientStopCollection( GradientStops, 2,
				D2D1_GAMMA_2_2,	D2D1_EXTEND_MODE_CLAMP,	&GradientStopCollection ) != S_OK )
		return false;
	if( FD2DCanvas->RenderTarget->CreateLinearGradientBrush(
			D2D1::LinearGradientBrushProperties( D2D1::Point2F(0,0),D2D1::Point2F(0,0.8f*(float)FPixelsPerTick)),
			GradientStopCollection,&FGreenGradientBrush ) != S_OK )
		return false;
	SafeRelease( &GradientStopCollection );
	///< Blue gradient brush
	GradientStops[0].color = D2D1::ColorF(0,0,1, 1.0f );
	GradientStops[1].color = D2D1::ColorF(0,0,0.25f,1.0f );
	if( FD2DCanvas->RenderTarget->CreateGradientStopCollection( GradientStops, 2,
				D2D1_GAMMA_2_2,	D2D1_EXTEND_MODE_CLAMP,	&GradientStopCollection ) != S_OK )
		return false;
	if( FD2DCanvas->RenderTarget->CreateLinearGradientBrush(
			D2D1::LinearGradientBrushProperties( D2D1::Point2F(0,0 ), D2D1::Point2F(0,0.8f*(float)FPixelsPerTick)),
			GradientStopCollection,&FBlueGradientBrush ) != S_OK )
		return false;
	SafeRelease( &GradientStopCollection );
	///< White gradient brush
	GradientStops[0].color = D2D1::ColorF(0.5f,0.5f,0.5f, 1.0f );
	GradientStops[1].color = D2D1::ColorF(0.25f,0.25f,0.25f,1.0f );
	if( FD2DCanvas->RenderTarget->CreateGradientStopCollection( GradientStops, 2,
				D2D1_GAMMA_2_2,	D2D1_EXTEND_MODE_CLAMP,	&GradientStopCollection ) != S_OK )
		return false;
	if( FD2DCanvas->RenderTarget->CreateLinearGradientBrush(
			D2D1::LinearGradientBrushProperties( D2D1::Point2F(0,0 ),D2D1::Point2F(0,0.8f*(float)FPixelsPerTick)),
			GradientStopCollection,&FWhiteGradientBrush ) != S_OK )
		return false;

	if( FD2DCanvas->RenderTarget->CreateSolidColorBrush(
		D2D1::ColorF( D2D1::ColorF::Yellow, 0.3f), &FYellowBrush) != S_OK )
		return false;

	SafeRelease( &GradientStopCollection );
	return true;
}
//------------------------------------------------------------------------------
bool __fastcall TStockChartEx::CreateD2DObjecsOnResize( void )
{
	const D2D1_RECT_F       ClipREECT = D2D1::RectF(0,0, Width, Height - FPixelsPerTick);
	const D2D1_ROUNDED_RECT CenterBtnRoundRECT  = D2D1::RoundedRect( D2D1::RectF(0,0,FCenterFilledBmp->Width,FCenterFilledBmp->Height/2),20.f,20.f);
	const D2D1_ROUNDED_RECT CancelBuyRoundRECT  = D2D1::RoundedRect( D2D1::RectF(0,0,FCancelAllBuyBmp->Width,FCancelAllBuyBmp->Height/2),20.f,20.f);
	const D2D1_ROUNDED_RECT CancelSellRoundRECT = D2D1::RoundedRect( D2D1::RectF(0,0,FCancelAllSellBmp->Width,FCancelAllSellBmp->Height/2),20.f,20.f);

	SafeRelease( &FClipLayer );
	SafeRelease( &FRectangleGeometry );
	SafeRelease( &FCenterBtnClipLayer );
	SafeRelease( &FCenterBtnGeometry );
	SafeRelease( &FCancelBuyBtnClipLayer );
	SafeRelease( &FCancelBuyBtnGeometry );
	SafeRelease( &FCancelSellBtnClipLayer );
	SafeRelease( &FCancelSellBtnGeometry );
	if( FD2DFactory->CreateRectangleGeometry( ClipREECT, &FRectangleGeometry ) != S_OK )
		return false;
	if( FD2DCanvas->RenderTarget->CreateLayer(NULL, &FClipLayer) != S_OK )
		return false;
	if( FD2DFactory->CreateRoundedRectangleGeometry( CenterBtnRoundRECT, &FCenterBtnGeometry ) != S_OK )
		return false;
	if( FD2DCanvas->RenderTarget->CreateLayer(NULL, &FCenterBtnClipLayer) != S_OK )
		return false;
	if( FD2DFactory->CreateRoundedRectangleGeometry( CancelBuyRoundRECT, &FCancelBuyBtnGeometry ) != S_OK )
		return false;
	if( FD2DCanvas->RenderTarget->CreateLayer(NULL, &FCancelBuyBtnClipLayer) != S_OK )
		return false;
	if( FD2DFactory->CreateRoundedRectangleGeometry( CancelSellRoundRECT, &FCancelSellBtnGeometry ) != S_OK )
		return false;
	if( FD2DCanvas->RenderTarget->CreateLayer(NULL, &FCancelSellBtnClipLayer) != S_OK )
		return false;

	ID2D1GradientStopCollection *GradientStopCollection = NULL;
	D2D1_GRADIENT_STOP           GradientStops[2];
	const D2D1_ELLIPSE 	         BubbleCircle = D2D1::Ellipse( D2D1::Point2F(32.f,32.f),32.f,32.f);

	SafeRelease( &FBullRadialGradientBrush );
	SafeRelease( &FBearRadialGradientBrush );
	SafeRelease( &FRefeRadialGradientBrush );
	///< Create bull bubble brush
	GradientStops[0].position = 0.0f;
	GradientStops[1].position = 1.0f;
	GradientStops[0].color = D2D1::ColorF(D2D1::ColorF::White, 0.25f );
	GradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Maroon, 0.4f );
	if( FD2DCanvas->RenderTarget->CreateGradientStopCollection( GradientStops,2,
																D2D1_GAMMA_2_2,	D2D1_EXTEND_MODE_CLAMP,
																&GradientStopCollection ) != S_OK )
		return false;
	if(	FD2DCanvas->RenderTarget->CreateRadialGradientBrush(
			D2D1::RadialGradientBrushProperties( D2D1::Point2F(32.f,32.f), D2D1::Point2F(5,-20.f),32,32),
			GradientStopCollection,&FBullRadialGradientBrush )!= S_OK )
		return false;
	SafeRelease( &GradientStopCollection );
	///< Create bear bubble brush
	GradientStops[0].color = D2D1::ColorF(D2D1::ColorF::White, 0.25f );
	GradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Green, 0.4f );
	if( FD2DCanvas->RenderTarget->CreateGradientStopCollection( GradientStops, 2,
																D2D1_GAMMA_2_2,	D2D1_EXTEND_MODE_CLAMP,
																&GradientStopCollection ) != S_OK )
		return false;
	if(	FD2DCanvas->RenderTarget->CreateRadialGradientBrush(
			D2D1::RadialGradientBrushProperties(  D2D1::Point2F(32.f,32.f), D2D1::Point2F(5,-20.f),32,32),
			GradientStopCollection,&FBearRadialGradientBrush )!= S_OK )
		return false;
	SafeRelease( &GradientStopCollection );
	///< Create ref px bubble brush
	GradientStops[0].color = D2D1::ColorF(D2D1::ColorF::White, 0.25f );
	GradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Gray, 0.4f );
	if( FD2DCanvas->RenderTarget->CreateGradientStopCollection( GradientStops, 2,
																D2D1_GAMMA_2_2,	D2D1_EXTEND_MODE_CLAMP,
																&GradientStopCollection ) != S_OK )
		return false;
	if(	FD2DCanvas->RenderTarget->CreateRadialGradientBrush(
			D2D1::RadialGradientBrushProperties(  D2D1::Point2F(32.f,32.f), D2D1::Point2F(5,-20.f),32,32),
			GradientStopCollection,&FRefeRadialGradientBrush )!= S_OK )
		return false;
	SafeRelease( &GradientStopCollection );
	return true;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::PaintD2D( bool IsFocus )
{
	TRect BufferRect(0,0,Width,Height);
	DWORD BeginMS,EndMS;
	int   UseMS;

	BeginMS = UFC::GetTickCountMS();
	DrawBackground( FD2DCanvas, BufferRect, IsFocus ); ///< Draw background
	if( FRectangleGeometry != NULL && FClipLayer != NULL )
	{
		FD2DCanvas->RenderTarget->PushLayer( D2D1::LayerParameters(D2D1::InfiniteRect(), FRectangleGeometry ), FClipLayer );
		DrawPoints( FD2DCanvas );   ///< Draw tick points.
		DrawTickGrid( FD2DCanvas ); ///< Draw grid lines.
		FD2DCanvas->RenderTarget->PopLayer();
	}
	DrawTimeGrid( FD2DCanvas );
	DrawHeader( FD2DCanvas );
	DrawButtons( FD2DCanvas );
	EndMS = UFC::GetTickCountMS();
	UseMS = EndMS - BeginMS;

	if( UseMS > FMaxUseMS )
	{
		int LimitFPS = 1000/UseMS;
		FMaxUseMS = UseMS;
		if( LimitFPS < FMaxFPS )
			SetFPS( LimitFPS );
	}
	if( FDebugMode == true ) ///< if DebugMode on, draw the FPS.
	{
		int    UseTime = EndMS - FBeginFPSTick;
		String Msg;

		FFrameCount ++;
		if( UseTime > 1000 )
		{
		   FCurrentFPS   = FFrameCount* (double)UseTime/1000.0;
		   FBeginFPSTick = EndMS;
		   FFrameCount   = 0;
		}
		Msg.printf( L"%3.1f FPS Max:%d ms Use:%d ms ", FCurrentFPS, FMaxUseMS, UseMS );
		int OldSize = FD2DCanvas->Font->Size;
		FD2DCanvas->Font->Color = clWhite;
		FD2DCanvas->Font->Size  = 11;
		FD2DCanvas->Brush->Style = bsClear;
		FD2DCanvas->TextOut( 10, 10, Msg);
		FD2DCanvas->Brush->Style = bsSolid;
		FD2DCanvas->Font->Size = OldSize;
	}
}
//------------------------------------------------------------------------------
/*void __fastcall TStockChartEx::WMPaint (TWMPaint & Message)
{
	TPaintStruct * PaintStruct = new TPaintStruct ();

	BeginPaint( Handle, PaintStruct);
	try
	{
		Paint ();
	}
	__finally
	{
		EndPaint (Handle, PaintStruct);
		delete (PaintStruct);
	}
} */
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::Paint( void )
{
	if(	FInitSymbol == true && FTicks != NULL && FTicks->ItemCount() > 0 )
	{
		DWORD TickNow = GetTickCount();
		bool  IsFocus = Focused();

		if( TickNow - FLastPaint < FPaintInterval )///< Up to MaxFPS frames per sec.
		{   ///< Skip this frame and enable time out timer.
			if( FTimer->Enabled == false )
				FTimer->Enabled = true;
			return;
		}
		else
		{   ///< Disable timeout timer.
			if( FTimer->Enabled == true )
				FTimer->Enabled = false;
			FLastPaint = TickNow;
		}
		FIsLastTickInView =  IsLastTickInView( FLastTickPos );
		CheckViewPortPosition();
		if( !ComponentState.Contains( csDesigning ) ) ///< Not design time.
		{
			int MapAlpha = 100;
			if( FThumbChart == true )
			{
				if( IsFocus )
					MapAlpha = 168;
				DrawMap( );
			}
			if( FEnableAccelerate == true && FD2DCanvas != NULL  ) ///< Use Direct 2D canvas.
			{
				FD2DCanvas->RenderTarget->BeginDraw();
					PaintD2D( IsFocus );
					if( FThumbChart == true )
						FD2DCanvas->Draw( FMapRect.Left, FMapRect.Top, FMapBitmap, MapAlpha );
				HRESULT hr = FD2DCanvas->RenderTarget->EndDraw();
				if( hr == D2DERR_RECREATE_TARGET )
					RecreateD2D( );
				else if( hr != S_OK )
				{
					String Msg;

					Msg.printf(L"Direct 2D Error Code:0x%x", hr );
					MessageDlg(	Msg, mtError, TMsgDlgButtons() << mbOK, 0);
				}
			}
			else ///< Use GDI canvas.
			{
				PaintGDI( IsFocus );
				if( FThumbChart == true )
					FBuffer->Canvas->Draw( FMapRect.Left, FMapRect.Top, FMapBitmap, MapAlpha );
				Canvas->Draw( 0,0, FBuffer );
			}
		}
		else ///< Use GDI canvas.
		{
			PaintGDI( IsFocus );
			Canvas->Draw( 0,0, FBuffer );
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::CenterLastTick( bool XOnly )
{
	if( FLastTick != NULL )
	{
		int LastTickX = (FLastTick->RefSec/FSecondsPerGrid)* FPixelsPerGrid; ///< Check Div by 0 OK

		if( Width > FPixelsPerGrid * 8 ) ///< Need 8 grid.( | Fill | Buy |Buy Market| B/S Ratio | Sell Market| Sell |Fill | Qty bar)
			FViewPortX = LastTickX - Width + 5*FPixelsPerGrid; ///< Shift 5 grid. ( B/S Ratio | Sell Market| Sell |Fill | Qty bar)
		else
			FViewPortX = LastTickX - Width + (Width -FPixelsPerGrid )/2;
		if( XOnly == false )
		{
			TPoint VsPos;
			TimeTickToVirtualScreen( FLastTick->RefSec, FLastTick->TenMS, FLastTick->FillPxIndex, VsPos );
			FViewPortY = VsPos.y - Height/2;
		}
		FThunbDirty = true;
		Paint();
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::CenterTick( int hh, int mm, int ss, int TenMS, double Px )
{
	int    RefTime = ToRefSec( hh, mm, ss );
	TPoint VartualPos;

	TimeTickToVirtualScreen( RefTime, TenMS, Px, VartualPos );
	FViewPortX = VartualPos.x - Width/2;
	FViewPortY = VartualPos.y - Height/2;
	FCenterX = VartualPos.x;
	FCenterY = VartualPos.y;
	Paint();
}
//------------------------------------------------------------------------------
Classes::TStringList* __fastcall TStockChartEx::GetSymbols( void )
{
	Classes::TStringList* List = new Classes::TStringList();
	List->Add( FSymbol );
	return List;
}
//------------------------------------------------------------------------------
void TStockChartEx::OnMarketDataUpdate(BasicData* Msg) {}
//------------------------------------------------------------------------------
void TStockChartEx::OnMarketDataUpdate(MatchInfo* Msg) {}
//------------------------------------------------------------------------------
void TStockChartEx::OnMarketDataUpdate(TotalMatch* Msg) {}
//------------------------------------------------------------------------------
void TStockChartEx::OnMarketDataUpdate(DayHighLowPrice* Msg) {}
//------------------------------------------------------------------------------
void TStockChartEx::OnMarketDataUpdate(OpeningInfo* Msg) {}
//------------------------------------------------------------------------------
void TStockChartEx::OnMarketDataUpdate(ClosingMarketData* Msg) {}
//------------------------------------------------------------------------------
void TStockChartEx::OnMarketDataUpdate(UnderlyingIndexInfo* Msg) {}
//------------------------------------------------------------------------------
void TStockChartEx::OnMarketDataUpdate(SumOfOrderInfo* Msg) {}
//------------------------------------------------------------------------------
void TStockChartEx::OnMarketDataUpdate( MarketDataMessage* Msg )
{
	UpdateDepth5( Msg );
}
//------------------------------------------------------------------------------
void TStockChartEx::OnMarketDataUpdate( OrderBookData* Msg )
{
	UpdateDepth5( Msg );
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::UpdateDepth5( OrderBookData* Msg )
{
	try
	{
		int BidQty,AskQty;
		double BidPx,AskPx;

		FBuyTotalQty = 0;
		FSellTotalQty = 0;
		for( register int i =0;i< DEPTH_COUNT; i++ )
		{
			if( Msg->GetBuyDepth( i + 1, BidPx, BidQty ) == true && BidPx != 0.0 )
			{
				FBuyQty[i]     = BidQty;
				FBuyTotalQty  += BidQty;
				FBuyPxIndex[i] = PxToTickIndex( BidPx,FDebugMode );
				if( BidQty >= ENABLE_LOG2_VAL  ) FEnableLog2 = true;
			}
			else
				FBuyPxIndex[i] = -1;

			if( Msg->GetSellDepth( i + 1, AskPx, AskQty ) == true && AskPx != 0.0)
			{
				FSellQty[i]     = AskQty;
				FSellTotalQty  += AskQty;
				FSellPxIndex[i] = PxToTickIndex( AskPx,FDebugMode );
				if( AskQty >= ENABLE_LOG2_VAL )	FEnableLog2 = true;
			}
			else
				FSellPxIndex[i] = -1;
		}
		if( IsLastTickInView() == true )
			Paint( );
	}
	catch( ... )
	{

	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::WndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case CN_KEYDOWN:    if( HandleKeyDown( Msg.WParam ) == true )
								return;
							break;
		case WM_ERASEBKGND:	Msg.Result = 1;
							return;
		case WM_KILLFOCUS:
		case WM_SETFOCUS:   Paint();
							break;
	}
	inherited::WndProc( Msg );
}
//------------------------------------------------------------------------------
//
//  Functions to handle mouse events
//
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::HandleMouseClick( int X, int Y )
{
	if( FLastTick != NULL  )
	{
		int FillLeft  = GetLastTickLeft();
		int LeftX     = FillLeft  - 2*FPixelsPerGrid;
		int FillRight = FillLeft  + FPixelsPerGrid;
		int RightX    = FillRight + 2*FPixelsPerGrid;
		int TickIndex = (Y+FViewPortY)/FPixelsPerTick; ///< Check Div By 0 Ok

		if( X > LeftX && X < FillLeft )///< Buy side
		{
			if( FReadOnly == true )
			{
				if( FOnUILock != NULL )
					FOnUILock( this );
				return;
			}
			if( X > LeftX + FPixelsPerGrid ) ///< New order
			{
				if( FOnNewOrder != NULL )
				{
					if( FCancelBeforeNew == true )
						DeleteAllOrders( true );
					if( FStepEnable == true  )
						PlaceStepNewOrder( nsOrderMessageDefine::sBuy, TickIndex, 1 );
					else
						FOnNewOrder( this, nsOrderMessageDefine::sBuy, FTicks->GetPxFromIndex( TickIndex, FDebugMode ),1 );
				}
			}
			else ///< Cancel
			{
				int CancelQty = FUserBuyQty[TickIndex];
				if( CancelQty > 0 )
				{
					FCancelMouseDown = true;
					FCancelSide      = nsOrderMessageDefine::sBuy;
					FCancelTickIndex = TickIndex;
					CreateCursor( X - LeftX, (Y+FViewPortY)%FPixelsPerTick, CancelQty, true );
				}
			}
		}
		else if( X > FillRight && X < RightX )///< Sell side
		{
			if( FReadOnly == true )
			{
				if( FOnUILock != NULL )
					FOnUILock( this );
				return;
			}
			if( X < FillRight + FPixelsPerGrid )///< New order
			{
				if( FOnNewOrder != NULL )
				{
					if( FCancelBeforeNew == true )
						DeleteAllOrders( true );
					if( FStepEnable == true  )
						PlaceStepNewOrder( nsOrderMessageDefine::sSell, TickIndex, 1 );
					else
					FOnNewOrder( this, nsOrderMessageDefine::sSell, FTicks->GetPxFromIndex( TickIndex, FDebugMode ),1 );
				}
			}
			else  ///< Cancel
			{
				int CancelQty = FUserSellQty[TickIndex];
				if( CancelQty > 0 )
				{
					FCancelMouseDown = true;
					FCancelSide      = nsOrderMessageDefine::sSell;
					FCancelTickIndex = TickIndex;
					CreateCursor( X - FPixelsPerGrid - FillRight, (Y+FViewPortY)%FPixelsPerTick, CancelQty, false );
				}
			}
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::MouseDown( Controls::TMouseButton Button, Classes::TShiftState Shift,int X,int Y)
{
	TPoint DownPoint = TPoint( X,Y );
	if( Button == mbLeft )
	{
		FCancelMouseDown = false;
		if( FCancelBuyRect.Contains( DownPoint ) &&	FReadOnly == false  )
		{
			TriggerCancelAllBuyOrders();
			return;
		}
		if( FCancelSellRect.Contains( DownPoint ) && FReadOnly == false )
		{
			TriggerCancelAllSellOrders();
			return;
		}
		if( FCenterFilledRect.Contains( DownPoint )  )
		{
			CenterLastTick( );
			if( FOnCenterPx != NULL )
				FOnCenterPx( this );
			return;
		}
		if( FThumbChart == true && FMapRect.Contains( DownPoint ) )
		{
			CenterView( X - FMapRect.Left, Y - FMapRect.Top );
		}
		else
		{
			if( IsClickCursor( X ) == false ) ///< Place/Cancel order
			{
				FMouseDown = true;
				FMouseDownPos.x = X;
				FMouseDownPos.y = Y;
				FMouseDownViewPortPos.x = FViewPortX;
				FMouseDownViewPortPos.y = FViewPortY;
				SetFocus();
			}
			else
				HandleMouseClick( X, Y );
		}
	}
	else if( Button == mbRight )
		HandleKeyDown( VK_RBUTTON );

}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::MouseMove( Classes::TShiftState Shift, int X, int Y)
{
	FMousePos.x = X;
	FMousePos.y = Y;
	if( FMouseDown == true )
	{
		int deltaX = X - FMouseDownPos.x;
		int deltaY = Y - FMouseDownPos.y;

		FViewPortX = FMouseDownViewPortPos.x - deltaX;
		FViewPortY = FMouseDownViewPortPos.y - deltaY;
		FThunbDirty = true;
		Paint( );
	}
	else
	{
		if( FCenterFilledRect.Contains( FMousePos )  == true )
		{
			if( FMouseInBtn != 1 )
			{
				FMouseInBtn = 1;
				Paint( );
			}
		}
		else if( FReadOnly == false && FCancelBuyRect.Contains( FMousePos ) == true )
		{
			if( FMouseInBtn != 2 )
			{
				FMouseInBtn = 2;
				Paint( );
			}
		}
		else if( FReadOnly == false && FCancelSellRect.Contains( FMousePos ) == true )
		{
			if( FMouseInBtn != 3 )
			{
				FMouseInBtn = 3;
				Paint( );
			}
		}
		else
		{
			if( FMouseInBtn != 0 )
			{
				FMouseInBtn = 0;
				Paint( );
			}
			if( FReadOnly == false && IsClickCursor( X ) == true )
			{
				if( FCancelMouseDown == false )
					Cursor = crHandPoint;
			}
			else
				Cursor = crDefault;
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift,int X,int Y)
{
	if( Button == mbLeft )
		FMouseDown = false;
	if( FCancelMouseDown == true )
	{
		Screen->Cursor = crDefault;
		FCancelMouseDown = false;
		if( FReadOnly == false )
		{
			TPoint MouseUpPt( X, Y );

			if( FCancelSide == nsOrderMessageDefine::sBuy && FCancelBuyRect.Contains( MouseUpPt ))
			{
				if( FOnReduceQty != NULL )
					FOnReduceQty( this, nsOrderMessageDefine::sBuy, FTicks->GetPxFromIndex( FCancelTickIndex, FDebugMode ),0 );
			}
			else  if( FCancelSide == nsOrderMessageDefine::sSell && FCancelSellRect.Contains( MouseUpPt ))
			{
				if( FOnReduceQty != NULL )
					FOnReduceQty( this, nsOrderMessageDefine::sSell, FTicks->GetPxFromIndex( FCancelTickIndex, FDebugMode ),0 );
			}
			else
			{
				int FillLeft        = GetLastTickLeft();
				int CancelBuyRight  = FillLeft - FPixelsPerGrid;
				int CancelBuyLeft   = CancelBuyRight  - FPixelsPerGrid;
				int CancelSellLeft  = FillLeft + 2*FPixelsPerGrid;
				int CancelSellRight = CancelSellLeft + FPixelsPerGrid;
				int TickIndex       = (Y+FViewPortY)/FPixelsPerTick;  ///< Check Div by 0 Ok

				if( X > CancelBuyLeft && X < CancelBuyRight )///< Buy side
				{
					 if( FCancelSide == nsOrderMessageDefine::sBuy )///< Same Side
					 {
						 if( FCancelTickIndex == TickIndex )      ///< Same tick
						 {
							if( FOnReduceQty != NULL )
								FOnReduceQty( this, nsOrderMessageDefine::sBuy, FTicks->GetPxFromIndex( FCancelTickIndex, FDebugMode ),0);
						 }
						 else
						 {
							if( FOnReplacePx != NULL )
								FOnReplacePx( this, nsOrderMessageDefine::sBuy, FTicks->GetPxFromIndex( FCancelTickIndex, FDebugMode ),FTicks->GetPxFromIndex( TickIndex, FDebugMode ) );
						 }
					}
				}
				else if( X > CancelSellLeft && X < CancelSellRight )///< Sell side
				{
					if( FCancelSide == nsOrderMessageDefine::sSell )///< Same Side
					{
						if( FCancelTickIndex == TickIndex )        ///< Same tick
						{
							if( FOnReduceQty != NULL )
								FOnReduceQty( this, nsOrderMessageDefine::sSell, FTicks->GetPxFromIndex( FCancelTickIndex, FDebugMode ),0 );
						}
						else
						{
							if( FOnReplacePx != NULL )
								FOnReplacePx( this, nsOrderMessageDefine::sSell, FTicks->GetPxFromIndex( FCancelTickIndex, FDebugMode ),FTicks->GetPxFromIndex( TickIndex, FDebugMode ) );
						}
					}
				}
			}
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::ZoomIn( void )
{
	if( FFontSize < FONT_MAX_SIZE )
	{
		FFontSize++;
		Resize();
		Paint();
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::ZoomOut( void )
{
	if( FFontSize > FONT_MIN_SIZE )
	{
		FFontSize--;
		Resize();
		Paint();
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::MouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta, const Types::TPoint &MousePos, bool &Handled)
{
	FOldFontSize = FFontSize;
	if( WheelDelta < 0 )
		ZoomOut();
	else
		ZoomIn();
	Handled = false;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::MouseEnter(  System::TObject* Sender )
{
	Application->BringToFront();
	SetFocus();
}
//------------------------------------------------------------------------------
//
//  Functions to handle key press
//
//------------------------------------------------------------------------------
bool __fastcall TStockChartEx::HandleKeyDown( System::Word Key )
{
	bool HandleMsg = true;

	if( FReadOnly == false )
	{
		if( Key == FKeyBuy )
			OnKeyBuyDown();
		else if( Key == FKeySell )
			OnKeySellDown();
		else if( Key == FKeyBetterIncrese )
			MoveBetter( 1 );
		else if( Key == FKeyBetterDecrese )
			MoveBetter( -1 );
		else if( Key == FKeyBuyMarket && FOnNewMarketOrder != NULL )
			FOnNewMarketOrder( this, nsOrderMessageDefine::sBuy, 1 );
		else if( Key == FKeySellMarket && FOnNewMarketOrder != NULL )
			FOnNewMarketOrder( this, nsOrderMessageDefine::sSell, 1 );
		else if( Key == FKeyCancelBuy  )
			TriggerCancelAllBuyOrders();
		else if( Key == FKeyCancelSell )
			TriggerCancelAllSellOrders();
		else if( Key == FKeyCancelAll )
			DeleteAllOrders( true);
		else if( Key == FKeyNewOrder )
			TriggerNewOrder();
		else
			HandleMsg = false;
	}
	else
	{
		if( Key == FKeyBuy || Key == FKeySell || Key == FKeyBuyMarket || Key == FKeySellMarket ||
			Key == FKeyCancelBuy || Key == FKeyCancelSell|| Key == FKeyCancelAll|| Key == FKeyNewOrder )
		{
			if( FOnUILock != NULL )
				FOnUILock( this );
		}
		HandleMsg = false;
	}
	if( Key == FKeyCenterFillPx )
	{
		CenterLastTick();
		if( FOnCenterPx != NULL )
			FOnCenterPx( this );
		HandleMsg = true;
	}
	return HandleMsg;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::OnKeyBuyDown( void )
{
	if( FOnNewOrder != NULL )
	{
		if( FLastTick != NULL )
		{
			int BuyIndex = FLastTick->FillPxIndex + FBetterStep;

			if( BuyIndex > 0 && BuyIndex < FTicks->ItemCount( ))
			{
				if( FCancelBeforeNew == true )
					DeleteAllOrders( true );
				if( FStepEnable )
					PlaceStepNewOrder( nsOrderMessageDefine::sBuy, BuyIndex, 1 );
				else
					FOnNewOrder( this, nsOrderMessageDefine::sBuy, FTicks->GetPxFromIndex( BuyIndex, FDebugMode ),1 );
			}
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::OnKeySellDown( void )
{
	if( FOnNewOrder != NULL )
	{
		if( FLastTick != NULL )
		{
			int SellIndex = FLastTick->FillPxIndex - FBetterStep;

			if( SellIndex > 0 && SellIndex < FTicks->ItemCount())
			{
				if( FCancelBeforeNew == true )
					DeleteAllOrders( true );
				if( FStepEnable )
					PlaceStepNewOrder( nsOrderMessageDefine::sSell, SellIndex, 1 );
				else
					FOnNewOrder( this, nsOrderMessageDefine::sSell, FTicks->GetPxFromIndex( SellIndex, FDebugMode ),1 );
			}
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::MoveBetter( int Dir )
{
	FBetterStep += Dir;
	Paint();
	if( FOnStepChange != NULL )
		FOnStepChange( this, FBetterStep );
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::DeleteAllOrders(  bool sync  )
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
void __fastcall TStockChartEx::CancelAll( void )
{
	int CxlCount = 0;
	int StopCancelCount = -1;
	int CancelCount;

	if( FOnAskCancelCount != NULL )
	{
		FOnAskCancelCount( this, StopCancelCount );
		CxlCount += TriggerCancelAllBuyOrders();
		CxlCount += TriggerCancelAllSellOrders();
		if( CxlCount > 0 )
		{
			StopCancelCount += CxlCount;
			for( int i = 0; i < CxlCount*20; i++ )
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
		CxlCount += TriggerCancelAllBuyOrders();
		CxlCount += TriggerCancelAllSellOrders();
		if( FClxAllDelayMS > 0 && CxlCount > 0  )
		{
			int WaitMS = (FClxAllDelayMS*CxlCount)/4;
			if( WaitMS < FClxAllDelayMS )
				WaitMS = FClxAllDelayMS;
			UFC::SleepMS( WaitMS );
		}
	}
}
//------------------------------------------------------------------------------
int __fastcall TStockChartEx::TriggerCancelAllBuyOrders( void )
{
	int CCount = 0;
	if( FOnDeleteAllOrders != NULL )
	{
		TDoubleDynArray PriceArray;
		PriceArray.Length = 0;

		for( register int i = 0; i < FUserBuyQty.Length; i++ )
		{

			if( FUserBuyQty[i] > 0 )
			{
				PriceArray.Length += 1;
				PriceArray[ PriceArray.Length - 1 ] = FTicks->GetPxFromIndex( i, FDebugMode );
			}
		}
		if( PriceArray.Length > 0 )
			FOnDeleteAllOrders( this, nsOrderMessageDefine::sBuy, PriceArray, CCount );
		PriceArray.Length = 0;  ///< Free dynamic double array's memory
	}
	return CCount;
}
//------------------------------------------------------------------------------
int __fastcall TStockChartEx::TriggerCancelAllSellOrders( void )
{
	int CCount = 0;
	if( FOnDeleteAllOrders != NULL )
	{
		TDoubleDynArray PriceArray;
		PriceArray.Length = 0;

		for( register int i = 0; i < FUserSellQty.Length; i++ )
		{

			if( FUserSellQty[i] > 0 )
			{
				PriceArray.Length += 1;
				PriceArray[ PriceArray.Length - 1 ] = FTicks->GetPxFromIndex( i, FDebugMode );
			}
		}
		if( PriceArray.Length > 0 )
			FOnDeleteAllOrders( this, nsOrderMessageDefine::sSell, PriceArray, CCount );
		PriceArray.Length = 0;  ///< Free dynamic double array's memory
	}
	return CCount;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::PlaceStepNewOrder( nsOrderMessageDefine::SideEnum Side, int BasePxIndex, int Qty )
{
	TDoubleDynArray PriceArray;
	int             BetterIndex = BasePxIndex;

	PriceArray.Length = 0;
	if( Side == nsOrderMessageDefine::sBuy )
	{
		for( register int i = 0; i <= FStepCount; i++ )
		{
			PriceArray.Length += 1;
			PriceArray[ PriceArray.Length - 1 ] = FTicks->GetPxFromIndex( BetterIndex, FDebugMode );
			BetterIndex += FStep;
		}
	}
	else
	{
		for( register int i = 0; i <= FStepCount; i++ )
		{
			PriceArray.Length += 1;
			PriceArray[ PriceArray.Length - 1 ] = FTicks->GetPxFromIndex( BetterIndex, FDebugMode );
			BetterIndex -= FStep;
		}
	}
	FOnNewOrders( this, Side, PriceArray, Qty );
	PriceArray.Length = 0;
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::TriggerNewOrder( void )
{
	if( FLastTick != NULL && FReadOnly == false && FOnNewOrder != NULL )
	{
		TPoint    Pos = ScreenToClient( Mouse->CursorPos );
		int FillLeft  = GetLastTickLeft();
		int LeftX     = FillLeft  - FPixelsPerGrid;
		int FillRight = FillLeft  + FPixelsPerGrid;
		int RightX    = FillRight + FPixelsPerGrid;
		int TickIndex = ( Pos.y + FViewPortY )/FPixelsPerTick;///< Check Div by 0 Ok

		if( Pos.x > LeftX && Pos.x < FillLeft )///< Buy side
		{
			if( FCancelBeforeNew == true )
				DeleteAllOrders( true );
			if( FStepEnable == true  )
				PlaceStepNewOrder( nsOrderMessageDefine::sBuy, TickIndex, 1 );
			else
				FOnNewOrder( this, nsOrderMessageDefine::sBuy, FTicks->GetPxFromIndex( TickIndex, FDebugMode ) ,1 );
		}
		else if( Pos.x > FillRight && Pos.x < RightX )///< Sell side
		{
			if( FCancelBeforeNew == true )
				DeleteAllOrders( true );
			if( FStepEnable == true  )
				PlaceStepNewOrder( nsOrderMessageDefine::sSell, TickIndex, 1 );
			else
				FOnNewOrder( this, nsOrderMessageDefine::sSell, FTicks->GetPxFromIndex( TickIndex, FDebugMode ),1 );
		}
	}
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::BuildImages( Graphics::TBitmap* Src,
											Graphics::TBitmap* Large,
											Graphics::TBitmap* Middle,
											Graphics::TBitmap* Small )
{
	if( Src != Large )
	{
		Large->SetSize( 64,64);
		Large->Canvas->StretchDraw(TRect(0,0,64,64),Src);
	}
	Middle->SetSize( 32,32);
	Middle->Canvas->StretchDraw(TRect(0,0,32,32),Src);
	Small->SetSize( 16,16);
	Small->Canvas->StretchDraw(TRect(0,0,16,16),Src);
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetBullBubble( Graphics::TBitmap* Bitmap )
{
	BuildImages( Bitmap,FBullBmpL,FBullBmpM,FBullBmpS );
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetBearBubble( Graphics::TBitmap* Bitmap )
{
	BuildImages( Bitmap,FBearBmpL,FBearBmpM,FBearBmpS );
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetRefBubble( Graphics::TBitmap* Bitmap )
{
	BuildImages( Bitmap,FRefBmpL,FRefBmpM,FRefBmpS );
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetCancelBuy( Graphics::TBitmap* Bitmap )
{
	FCancelAllBuyBmp->Assign(Bitmap);
	FCancelAllBuyBmp->Dormant();
	FCancelAllBuyBmp->FreeImage();
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetCancelSell( Graphics::TBitmap* Bitmap )
{
	FCancelAllSellBmp->Assign(Bitmap);
	FCancelAllSellBmp->Dormant();
	FCancelAllSellBmp->FreeImage();
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetCenterFilled( Graphics::TBitmap* Bitmap )
{
	FCenterFilledBmp->Assign(Bitmap);
	FCenterFilledBmp->Dormant();
	FCenterFilledBmp->FreeImage();
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetSecondsPerGrid( int Seconds )
{
	if( Seconds <= 0 )
		Seconds = 1;
	FSecondsPerGrid = Seconds;
	Resize();
}
//------------------------------------------------------------------------------
void __fastcall TStockChartEx::SetEnableD2D( bool EnableD2d )
{
	if( FEnableAccelerate != EnableD2d )
	{
		if( EnableD2d == true )
		{
			if( TDirect2DCanvas::Supported() == true )
			{
				FEnableAccelerate = true;
				Invalidate();
			}
			else
				FEnableAccelerate = false;///< Not Support Direct 2D
		}
		else
		{
			FEnableAccelerate = false;
			Invalidate();
		}
    }
}
//---------------------------------------------------------------------------
void __fastcall TStockChartEx::SetShowFilled( bool Show )
{
	FShowFilled = Show;
	if( FShowFilled == true )
		FHeaderBitmap->Width  = FPixelsPerGrid*7;
	else
		FHeaderBitmap->Width  = FPixelsPerGrid*5;
	FHeaderBitmap->Height = FPixelsPerTick;
	Invalidate();
}
//------------------------------------------------------------------------------
void TStockChartEx::OnFill(int Position, double BuyArvPx, double SellArvPx, int BuyQty, int SellQty,
						   int BuyOpenInterestQty, int SellOpenInterestQty,
						   DynamicArray<void*>& PosStatisticsArray, int NetPosition, double FloatingProfit )
{
	FTotalUserFilledBuyQty  = 0;
	FTotalUserFilledSellQty = 0;
	for( register int i = 0; i < PosStatisticsArray.Length; i++ )
	{
		s888::CMatchPriceStatisticRecord *curPxStatisticRecPtr = static_cast<s888::CMatchPriceStatisticRecord*>(PosStatisticsArray[i]);
		curPxStatisticRecPtr->LockForRead();
		double matchPx = curPxStatisticRecPtr->GetMatchPrice(s888::rwNotLock);
		nsOrderMessageDefine::SideEnum side = curPxStatisticRecPtr->GetSide(s888::rwNotLock);
		int matchQuantity = curPxStatisticRecPtr->GetTotalMatchQuantity(s888::rwNotLock);
		curPxStatisticRecPtr->UnlockForRead();
		int Index = PxToTickIndex(matchPx, FDebugMode);
		try
		{
			if( side == nsOrderMessageDefine::sBuy )
			{
				FUserFilledBuyQty[Index] = matchQuantity;
				FTotalUserFilledBuyQty += matchQuantity;
			}
			else
			{
				FUserFilledSellQty[Index] = matchQuantity;
				FTotalUserFilledSellQty += matchQuantity;
			}
		}
		catch( ... )
		{

		}
	}
	if( FOnNetPositionUpdate != NULL )
		FOnNetPositionUpdate( this, NetPosition, FloatingProfit, BuyOpenInterestQty, SellOpenInterestQty );
	Paint();
}
//------------------------------------------------------------------------------
void TStockChartEx::OnProfit(double AveragePrice, double ProfitAmount)
{
	if( FOnProfitAndAvgPxUpdate != NULL )
		FOnProfitAndAvgPxUpdate( this, ProfitAmount, AveragePrice );
}
//---------------------------------------------------------------------------
//	Handle PingPong Order
//---------------------------------------------------------------------------
void __fastcall TStockChartEx::EnablePingPong( bool Enable )
{
	FEnablePingPong = Enable;
}
//---------------------------------------------------------------------------
void __fastcall TStockChartEx::SetBuyPingPongTick( int Tick )
{
	FBuyPingPongTick = Tick;
}
//---------------------------------------------------------------------------
void __fastcall TStockChartEx::SetSellPingPongTick( int Tick )
{
	FSellPingPongTick = Tick;
}
//---------------------------------------------------------------------------
void TStockChartEx::OnFill(nsOrderMessageDefine::SideEnum Side, double Px, int Qty, int TickCount)
{
	if( FEnablePingPong && TickCount > 0 )
	{
		nsOrderMessageDefine::SideEnum OrderSide;
		int OrigTick, OrderTick;
		int TotalTick = FTicks->ItemCount() - 1;
		double OrderPx;

		OrigTick = PxToTickIndex( Px,FDebugMode );
		if( OrigTick == 0 || OrigTick ==  TotalTick )
		{
			FEnablePingPong = false;
			return;
		}

		if( Side == nsOrderMessageDefine::sBuy )
		{
			OrderSide = nsOrderMessageDefine::sSell;
			OrderTick = OrigTick - FBuyPingPongTick;
			if( OrderTick < 0 ) ///< Bull Px
				OrderTick = 0;
		}
		else
		{
			OrderSide = nsOrderMessageDefine::sBuy;
			OrderTick = OrigTick + FSellPingPongTick;
			if( OrderTick > TotalTick ) ///< Bear Px
				OrderTick = TotalTick;
		}
		OrderPx = FTicks->GetPxFromIndex( OrderTick, FDebugMode );
		if( FOnNewPingPongOrder != NULL )
			FOnNewPingPongOrder( this, OrderSide, OrderPx, Qty, TickCount );
	}
}
//------------------------------------------------------------------------------
int __fastcall TStockChartEx::TickCount( void )
{
	return FTicksList.ItemCount();
}
//------------------------------------------------------------------------------
TTickInfo* __fastcall TStockChartEx::GetTick( int Index )
{
	return ((TTickInfo*) FTicksList.GetItem( Index ));
}
//------------------------------------------------------------------------------
void  __fastcall TStockChartEx::ToTNFormat( double Px, String& PxStr )
{
	if( FTicks != NULL )
		FTicks->ToTNFormat( Px, PxStr );
	else
		PxStr.printf( L"%5.*f", FStrikePxDigi, Px );
}
//------------------------------------------------------------------------------
namespace Stockchartex
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TStockChartEx)};
		RegisterComponents( L"Speedy", classes, 0);
	}
}
//------------------------------------------------------------------------------
