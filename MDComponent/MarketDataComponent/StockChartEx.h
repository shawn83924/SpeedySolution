// ---------------------------------------------------------------------------

#ifndef StockChartExH
#define StockChartExH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <SysUtils.hpp>
#include <StdCtrls.hpp>
#include <Direct2D.hpp>
#include <ExtCtrls.hpp>
// ---------------------------------------------------------------------------
#include <hash_map>
#include <vector>
#include "OrderMessageDefine.h"
#include "StockChart.h"
#include "OrderBookList.h"
#include "MarketDataStore.h"
// ---------------------------------------------------------------------------
const int DEPTH_COUNT = 10;
// ---------------------------------------------------------------------------
template<class T>
void SafeRelease(T * *ppT)
{
	if (*ppT)
	{
	   (*ppT)->Release();
	   *ppT = NULL;
	}
}
// ---------------------------------------------------------------------------
typedef enum
{
	btBearBubble = 0,
	btBullBubble = 1,
	btRefBubble = 2

} TBubbleType;
// ---------------------------------------------------------------------------
class PACKAGE TPositionPoint : public TObject
{
public:
	int  FRefSec;///< Time, format:HHMMSSmm
	int  F10ms;
	int  FFillTickIndex;
	int  FQty;
	bool FIsBuy;
public:
	__fastcall TPositionPoint(int RefSec, int TenMs, int PxIndex, int Qty, bool IsBuy);
};
// ------------------------------------------------------------------------------
class TCompareTickObject : public UFC::CompareInterface
{
public:
	virtual ~TCompareTickObject(void){ }
	virtual int Compare(void* elem1, void* elem2);
};
// ---------------------------------------------------------------------------
typedef void __fastcall(__closure * TOnErrorMessage)(System::TObject * Sender,const String& Title,const String& Msg );
// ---------------------------------------------------------------------------
class PACKAGE TStockChartEx : public TCustomControl, public IMarketDataListener, public TPositionChangeListener {
private:
	int MARKET_MINUTES;
	int BEFORE_OPEN_SEC;
	int AFTER_CLOSED_SEC;
	double FThumbHRatio;
	double FPixelsPerSecond;///< TextWidth("12345.00") depends on font size
	int FPixelsPerGrid;
	int FPixelsPerTick;	///< 1.2* TextHeight("12345.00") depends on font size
	int FSecondsCount; 	///< In Taifex it's 1800 sec.(5 hours)
	int FCloseRefSec;
	int FSecondsPerGrid;
	int FVirtualWidth; 	///<  FSecondCount * FPixelsPerSecond
	int FVirtualHeight;	///<  FTicksCount * FPixelsPerTick
	int FViewPortX;
	int FViewPortY;
	int FViewPortWidth;	///< Equals to Control Width
	int FViewPortHeight;///< Equals to Control Height
	int FOpenTimeSec;
	int FCloseTimeSec;
	int FOpenHour;
	int FOpenMinutes;
	int FStrikePxDigi;
	int FDecimalLocator;
	double FPixelPer10ms;
	DWORD FLastPaint;
	bool FReadOnly;
	int FMaxFPS;
	DWORD FPaintInterval;
	bool FInitSymbol;
	bool FThumbChart;
	bool FShowBubble;
	bool FShowTickLine;
	int FCenterX;
	int FCenterY;
	int FSelectPP;	///< Selected position point.
	TTimer* FTimer;
	double FBullPrice;
	double FBearPrice;
	double FRefPrice;
	int FRefPxIndex;
	int FFontSize;
	int FOldFontSize;
	int FOldWidth;
	int FOldHeight;
	ExchangeInfo* FExchangeInfo;	///< for Technical Indicator
	bool FBollingerBands;
	bool FEnableLog2;
	double FBandsWidth;
	double FBandsWidth2;
	double FBandsWidth3;
	int FBollingerSec;
	bool FShowMA;
	int FMA1Sec;
	int FMA2Sec;
	int FMA3Sec;
	bool FMouseDown;
	TPoint FMousePos;
	TPoint FMouseDownPos;
	TPoint FMouseDownViewPortPos;
	bool FCancelMouseDown;
	bool FChangeFillColor;

	nsOrderMessageDefine::SideEnum FCancelSide;

	int FCancelTickIndex;
	TRect FMapRect;
	///< Rect for thumbnail view.
	TRect FCancelBuyRect;
	///< Rect for buy side cancel all button.
	TRect FCancelSellRect;
	///< Rect for sell side cancel all button.
	TRect FCenterFilledRect;
	bool  FShowFilled;
	///< Rect for center filled px button.
	int FMouseInBtn;
	///< PingPong Order
	bool FEnablePingPong;
	int FBuyPingPongTick;
	int FSellPingPongTick;
	int FClxAllDelayMS;
	///< Mouse clicked in which button.
	Graphics::TBitmap* FCancelAllBuyBmp;	///< bitmap for buy side cancel all button.
	Graphics::TBitmap* FCancelAllSellBmp;	///< bitmap for sell side cancel all button.
	Graphics::TBitmap* FCenterFilledBmp; 	///< bitmap for center filled px button.
	Graphics::TBitmap* FBubbleBmp;
	Graphics::TBitmap* FBullBmpL;
	Graphics::TBitmap* FBullBmpM;
	Graphics::TBitmap* FBullBmpS;
	Graphics::TBitmap* FBearBmpL;
	Graphics::TBitmap* FBearBmpM;
	Graphics::TBitmap* FBearBmpS;
	Graphics::TBitmap* FRefBmpL;
	Graphics::TBitmap* FRefBmpM;
	Graphics::TBitmap* FRefBmpS;
	///< Buffer for GDI double buffered mechanism.( Direct2D don't need this )
	Graphics::TBitmap* FBuffer;
	///< Buffer fot thumbnail view.
	Graphics::TBitmap* FMapBitmap;
	///< Buffer for depth view header.
	Graphics::TBitmap* FHeaderBitmap;
	///< Data for drawing MA.
	TIntegerDynArray FGridQtys;
	TIntegerDynArray FGridSubTotal;
	TIntegerDynArray FSecTrueRange;
	///< To keep user's working orders.
	TIntegerDynArray FUserBuyQty;
	TIntegerDynArray FUserSellQty;
	int FTotalUserBuyQty;
	int FTotalUserSellQty;
	///< To keep user's positions.
	TIntegerDynArray FUserFilledBuyQty;
	TIntegerDynArray FUserFilledSellQty;
	int FTotalUserFilledBuyQty;
	int FTotalUserFilledSellQty;
	///< Filled qty for each Price.
	TIntegerDynArray FFillQtyArray;
	///< Data for Price to Index mechanism.
	TCMarketDataStore* FStore;
	TTickList* FTicks;
	UFC::PList FTicksList;
	///< List to keep all tick data.
	UFC::PHashedSet<int>FTickIDSet;
	///< Hashset to filter duplicate tick
	TCompareTickObject FCmpObj;
	///< Object contains compare function to sort ticks.
	TTickInfo* FLastTick;
	///< Pointor to the least tick data.
	TPoint FLastTickPos;
	bool FIsLastTickInView;
	TPoint* FBezierPoints;
	///< Points to draw polyline/polygon
	double* FMeanPxs;
	///< Points to keep mean data.( for MA line)
	double* FOffsetPxs;
	///< to keep bands width.( for Bollinger Bands )
	String FSymbol;
	String FExchange;
	TObjectList* FPositionPoints;
	///< Data to keep depth
	int FBuyQty[DEPTH_COUNT];
	///< Best N Bid px,qty
	int FBuyPxIndex[DEPTH_COUNT];
	int FSellQty[DEPTH_COUNT];
	///< Best N Ask px,qty
	int FSellPxIndex[DEPTH_COUNT];
	int FBuyTotalQty;
	///< Total depth N bid volume.
	int FSellTotalQty;
	///< Total depth N ask volume.
	double FMaxPrice;
	double FMinPrice;
	int FMaxPxIndex;
	int FMinPxIndex;
	int FQtyMax;
	int FFillPxQtyMax;
	bool FCenterFillPx;
	bool FEnableAccelerate;
	bool FThunbDirty;
	TIntegerDynArray FQtys;
	TDoubleDynArray FMaxPx;
	TDoubleDynArray FMinPx;
	TDoubleDynArray FOpenPx;
	TDoubleDynArray FClosePx;
	TBooleanDynArray FHaveData;
	///< Objects for Support Direct 2D
	TDirect2DCanvas* FD2DCanvas;
	ID2D1Factory* FD2DFactory;
	ID2D1Layer* FClipLayer;
	ID2D1RectangleGeometry* FRectangleGeometry;
	ID2D1Layer* FCenterBtnClipLayer;
	ID2D1RoundedRectangleGeometry* FCenterBtnGeometry;
	ID2D1Layer* FCancelBuyBtnClipLayer;
	ID2D1RoundedRectangleGeometry* FCancelBuyBtnGeometry;
	ID2D1Layer* FCancelSellBtnClipLayer;
	ID2D1RoundedRectangleGeometry* FCancelSellBtnGeometry;
	ID2D1LinearGradientBrush* FRedGradientBrush;
	ID2D1LinearGradientBrush* FGreenGradientBrush;
	ID2D1LinearGradientBrush* FBlueGradientBrush;
	ID2D1LinearGradientBrush* FWhiteGradientBrush;
	ID2D1RadialGradientBrush* FBullRadialGradientBrush;
	ID2D1RadialGradientBrush* FBearRadialGradientBrush;
	ID2D1RadialGradientBrush* FRefeRadialGradientBrush;
	ID2D1SolidColorBrush* FYellowBrush;
	bool __fastcall CreateD2DObjecsOnResize(void);
	bool __fastcall CreateD2DObjecsOnZoom(void);
	void __fastcall RecreateD2D( void );
	void __fastcall SetBitmapAlpha(Graphics::TBitmap* RGBABmp, int Alpha);
	void __fastcall CreateCursor(int HotspotX, int HotspotY, int CancelQty, bool IsBuy);
	///< Draw functions
	TColor __fastcall DarkerColor(TColor OrigCol);
	TColor __fastcall LighterColor(TColor OrigCol);

	void __fastcall DrawGradientBar(TCanvas* canvas, TRect& dRect, TColor UpCol, TColor DownCol, bool IsVert = true);
	void __fastcall DrawGradientBar(HDC DC, TRect& dRect, TColor UpCol, TColor DownCol, bool IsVert = true);
	void __fastcall DrawGradientBar(TDirect2DCanvas* canvas, TRect& dRect, ID2D1LinearGradientBrush*);

	void __fastcall SolidBubble(TCanvas* canvas, int X, int Y, int R, TBubbleType BubbleType);
	void __fastcall SolidBubble(TDirect2DCanvas* canvas, int X, int Y, int R, TBubbleType BubbleType);
	void __fastcall AlphaBubble(TCanvas* canvas, int X, int Y, int R, TBubbleType BubbleType);
	void __fastcall AlphaBubble(TDirect2DCanvas* canvas, int X, int Y, int R, TBubbleType BubbleType);
	void __fastcall DrawBubble(TCanvas* canvas, int X, int Y, int R, TBubbleType BubbleType, int MinAlphaBubble = 0);
	void __fastcall DrawBubble(TDirect2DCanvas* canvas, int X, int Y, int R, TBubbleType BubbleType, int MinAlphaBubble = 0);
	void __fastcall DrawBmpButton(TCanvas* canvas, const TRect& BtnRect, Graphics::TBitmap* BtnBmp);
	void __fastcall DrawBmpButton(TDirect2DCanvas* canvas, const TRect& BtnRect, Graphics::TBitmap* BtnBmp,	ID2D1RoundedRectangleGeometry* , ID2D1Layer*);
	void __fastcall DrawFillBubble(TCanvas* canvas, TTickInfo* pTick, int Cx, int Cy);
	void __fastcall DrawFillBubble(TDirect2DCanvas* canvas, TTickInfo* pTick, int Cx, int Cy);
	void __fastcall DrawColorRect(TCanvas* canvas, const TRect& PaintRect, TColor BKColor, TColor LightColor,TColor DarkColor);
	void __fastcall DrawColorRect(TDirect2DCanvas* canvas, const TRect& PaintRect, TColor BKColor, TColor LightColor,TColor DarkColor);
	void __fastcall DrawDepth(TCanvas* canvas, int FillX, int UpperY, int UpperTick);
	void __fastcall DrawDepth(TDirect2DCanvas* canvas, int FillX, int UpperY, int UpperTick);
	void __fastcall DrawBackground(TCanvas* canvas, TRect& BufferRect, bool IsFocus);
	void __fastcall DrawBackground(TDirect2DCanvas* canvas, TRect& BufferRect, bool IsFocus);
	void __fastcall DrawTickGrid(TCanvas* canvas);
	void __fastcall DrawTickGrid(TDirect2DCanvas* canvas);
	void __fastcall DrawTimeGrid(TCanvas* canvas);
	void __fastcall DrawTimeGrid(TDirect2DCanvas* canvas);
	void __fastcall DrawPoints(TCanvas* canvas);
	void __fastcall DrawPoints(TDirect2DCanvas* canvas);
	void __fastcall DrawSecMA(TDirect2DCanvas* canvas, int BeginSec, int EndSec, int AvgSec, TColor);
	void __fastcall DrawSecMA(TCanvas* canvas, int BeginSec, int EndSec, int AvgSec, TColor);
	void __fastcall DrawBollingerBands(TDirect2DCanvas* canvas, int BeginSec, int EndSec, int AvgSec);
	void __fastcall DrawBollingerBands(TCanvas* canvas, int BeginSec, int EndSec, int AvgSec);
	void __fastcall GetThreePxForBollingerBands(int BeginSec, int EndSec, double& MA, double& StdDev);
	void __fastcall DrawPositionPoints(TDirect2DCanvas* canvas, int BeginSec, int EndSec);
	void __fastcall DrawPositionPoints(TCanvas* canvas, int BeginSec, int EndSec);
	void __fastcall DrawStdDev(TDirect2DCanvas* canvas, int BeginSec, int EndSec);
	void __fastcall DrawStdDev(TCanvas* canvas, int BeginSec, int EndSec);
	void __fastcall DrawSecQtyBars(TDirect2DCanvas* canvas, int BeginSec, int EndSec);
	void __fastcall DrawSecQtyBars(TCanvas* canvas, int BeginSec, int EndSec);
	void __fastcall DrawButtons(TCanvas* canvas);
	void __fastcall DrawButtons(TDirect2DCanvas* canvas);

	int  __fastcall FindQtyMax(int BeginSec, int EndSec);
	void __fastcall DrawHeader(TCustomCanvas* canvas);
	void __fastcall DrawMap(void);
	void __fastcall PaintGDI(bool IsFocus);
	void __fastcall PaintD2D(bool IsFocus);
public:
	int  __fastcall CloseRefSec( void );
	int  __fastcall ToRefSec(int HH, int MM, int SS);
	void __fastcall RefSecToHHMMSS(int Index, int& HH, int& MM, int& SS);
	void __fastcall RefSecToTimeString(int Index, UFC::AnsiString& Time);
	void __fastcall TimeTickToVirtualScreen(int RefTime, int TenMS, double Px, TPoint& VsPos);
	void __fastcall TimeTickToVirtualScreen(int RefTime, int TenMS, int Tick, TPoint& VsPos);
	void __fastcall TimeTickToWindow(int RefTime, int TenMS, double Px, TPoint& WinPos);
	void __fastcall TimeTickToWindow(int RefTime, int TenMS, int Tick, TPoint& WinPos);
	void __fastcall VirtualScreenXToTime(int& RefTime, int& TenMS, int X);
	void __fastcall VirtualScreenToTimeTick(int& RefTime, int& TenMS, int& Tick, const TPoint& VsPos);
private:
	TBubbleType __fastcall TickType( TTickInfo* pTick, int RefPxIndex );
	TBubbleType __fastcall TickType( TTickInfo* pTick );
	bool __fastcall ConvertData( TTickInfo& Tick );
	void __fastcall UpdateDepth5(OrderBookData* Msg);
	int  __fastcall GetLastTickLeft(void);
	void __fastcall MoveCellRectX(const TRect& InRect, TRect& OutRect, int Pos);
	void __fastcall BuildImages(Graphics::TBitmap* Src, Graphics::TBitmap* Large, Graphics::TBitmap* Middle, Graphics::TBitmap* Small);
	double __fastcall IndexToPx(double DoubleIndex);
	int __fastcall PxToTickIndex(double Price, bool ThrowException = false);
	Graphics::TBitmap* __fastcall SelectBitmap(int Diameter, TBubbleType BubbleType);
	void __fastcall CheckViewPortPosition(void);
	void __fastcall Loaded(void);
	int  __fastcall FindRefSecIndex(int RefSec);
	void __fastcall FindPointsRange(int BeginSec, int EndSec, int& BeginIndex, int& EndIndex);
	bool __fastcall HandleKeyDown(System::Word Key);
	void __fastcall MouseLeave(System::TObject* Sender);
	void __fastcall MouseEnter(System::TObject* Sender);
	void __fastcall CenterView(int X, int Y);
	bool __fastcall PointInView(const TPoint& VSPos);
	bool __fastcall IsClickCursor(int X);
	bool __fastcall IsLastTickInView(void);
	bool __fastcall IsLastTickInView(TPoint& LastTickPos);
	void __fastcall HandleMouseClick(int X, int Y);
	void __fastcall MouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta, const Types::TPoint &MousePos, bool &Handled);
	void __fastcall OnRepaintTimer(TObject *Sender);
	void __fastcall CreateWnd();
//	void __fastcall WMPaint(TWMPaint & Message);
	virtual void __fastcall WndProc(TMessage &Msg);
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall Resize(void);
	bool __fastcall CanZooming( const String& PriceText, int NewSize );
//	BEGIN_MESSAGE_MAP MESSAGE_HANDLER(WM_PAINT, TWMPaint, WMPaint);
//	END_MESSAGE_MAP(TCustomControl)

	///< Hot key
	System::Word FKeyNewOrder;
	System::Word FKeyBuy;
	System::Word FKeySell;
	System::Word FKeyBetterIncrese;
	System::Word FKeyBetterDecrese;
	System::Word FKeyBuyMarket;
	System::Word FKeySellMarket;
	System::Word FKeyCancelAll;
	System::Word FKeyCancelBuy;
	System::Word FKeyCancelSell;
	System::Word FKeyCenterFillPx;
	Controls::TMouseButton FMouseButtonCenterFillPx;

	///<Event
	TNotifyEvent                 FOnUILock;
	TOnNewOrderEvent             FOnNewOrder;
	TOnNewOrdersEvent            FOnNewOrders;
	TOnReduceQtyEvent            FOnReduceQty;
	TOnReplacePxEvent            FOnReplacePx;
	TOnDeleteAllEvent            FOnDeleteAllOrders;
	TOnNewMarketOrderEvent       FOnNewMarketOrder;
	TOnProfitAndAvgPxUpdateEvent FOnProfitAndAvgPxUpdate;
	TOnNetPositionUpdateEvent    FOnNetPositionUpdate;
	TNotifyEvent                 FOnCenterPx;
	TOnStepChangeEvent           FOnStepChange;
	TOnErrorMessage              FOnErrorMessage;
	TOnNewPingPongOrderEvent     FOnNewPingPongOrder;
	TOnAskCancelCount            FOnAskCancelCount;
	int FBetterStep;
	bool FDebugMode;
	bool FShowFillQty;
	bool FShowSecQty;
	bool FShwStdDev;
	bool FCancelBeforeNew;
	bool FStepEnable;
	int FStep;
	int FStepCount;

	int    FMaxUseMS;
	int    FFrameCount;
	double FCurrentFPS;
	DWORD  FBeginFPSTick;
	///< Colors
	TColor FFixedRowColor;
	TColor FFixedRowBKColor;
	TColor FGridColor;
	TColor FTickUpperColor;
	TColor FTickLowerColor;
	TColor FTickBKColor;
	TColor FFillColor;
	TColor FFillBKColor;
	TColor FBetterBuyColor;
	TColor FBetterBuyBKColor;
	TColor FBetterSellColor;
	TColor FBetterSellBKColor;
	TColor FBidDepth5BKColor;
	TColor FBidDepth5Color;
	TColor FAskDepth5BKColor;
	TColor FAskDepth5Color;
	TColor FBuyBKColor;
	TColor FBuyColor;
	TColor FSellBKColor;
	TColor FSellColor;
	TColor FBuyFillColor;
	TColor FBuyFillBKColor;
	TColor FSellFillColor;
	TColor FSellFillBKColor;

	TColor FMA1Color;
	TColor FMA2Color;
	TColor FMA3Color;
	TColor FBBMAColor;

	///< Implement interface IMarketDataListener
	virtual Classes::TStringList* __fastcall GetSymbols(void);
	virtual void OnMarketDataUpdate(MarketDataMessage* Msg);
	virtual void OnMarketDataUpdate(OrderBookData* Msg);

	virtual void OnMarketDataUpdate(BasicData* Msg);
	virtual void OnMarketDataUpdate(MatchInfo* Msg);
	virtual void OnMarketDataUpdate(TotalMatch* Msg);
	virtual void OnMarketDataUpdate(DayHighLowPrice* Msg);
	virtual void OnMarketDataUpdate(OpeningInfo* Msg);
	virtual void OnMarketDataUpdate(ClosingMarketData* Msg);
	virtual void OnMarketDataUpdate(UnderlyingIndexInfo* Msg);
	virtual void OnMarketDataUpdate(SumOfOrderInfo* Msg);
	///< Implement interface TPositionChangeListener
	virtual void OnFill(int Position, double BuyArvPx, double SellArvPx, int BuyQty, int SellQty,
						int BuyOpenInterestQty, int SellOpenInterestQty,
						DynamicArray<void*>& PosStatisticsArray, int NetPosition, double FloatingProfit);
	virtual void OnProfit(double AveragePrice, double ProfitAmount);
	virtual void OnFill(nsOrderMessageDefine::SideEnum Side, double Px, int Qty, int TickCount);

private:
	void __fastcall CancelAll( void );
	void __fastcall OnKeyBuyDown(void);
	void __fastcall OnKeySellDown(void);
	int  __fastcall TriggerCancelAllBuyOrders(void);
	int  __fastcall TriggerCancelAllSellOrders(void);
	void __fastcall TriggerNewOrder(void);
	void __fastcall PlaceStepNewOrder(nsOrderMessageDefine::SideEnum Side, int BasePxIndex, int Qty);
	void __fastcall MoveBetter(int Dir);
	void __fastcall SetFPS(int FPS);
	void __fastcall SetThumbChart(bool Show);
	void __fastcall SetRefBubble(Graphics::TBitmap* Bitmap);
	void __fastcall SetBullBubble(Graphics::TBitmap* Bitmap);
	void __fastcall SetBearBubble(Graphics::TBitmap* Bitmap);
	void __fastcall SetCancelBuy(Graphics::TBitmap* Bitmap);
	void __fastcall SetCancelSell(Graphics::TBitmap* Bitmap);
	void __fastcall SetCenterFilled(Graphics::TBitmap* Bitmap);
	void __fastcall SetShowFilled( bool Show );
	void __fastcall SetSecondsPerGrid(int Seconds);
	void __fastcall SetEnableD2D(bool);
	bool __fastcall SetExchangeTime(BasicInformation* SymbolInfo);
	///< PingPong
	void __fastcall EnablePingPong( bool Enable );
	void __fastcall SetBuyPingPongTick( int Tick );
	void __fastcall SetSellPingPongTick( int Tick );
public:
	__fastcall TStockChartEx(TComponent* Owner);
	__fastcall ~TStockChartEx(void);
public:
	void __fastcall SetStore(TCMarketDataStore* Store);
	void __fastcall SetCreatePositionPoint(TPositionPoint* NewPos);
	void __fastcall CenterPositionPoint(int Index);
	void __fastcall ClearPosition(void);
	void __fastcall SetSymbol(BasicInformation* SymbolInfo, TTickList* Ticks = NULL);
	void __fastcall ClearData(void);
	void __fastcall Clear(void);
	void __fastcall SetData( TTickInfo& Tick, bool NeedPaint );
	void __fastcall SetData(int HHMMSSmm, int ID, int Seq, double BuyPx, double SellPx, double FillPx, int Qty,	bool NeedPaint);
	void __fastcall SetMinuteData(TMinutesTickInfo& Info, bool NeedPaint = true);
	void __fastcall CenterLastTick(bool XOnly = false);
	void __fastcall CenterTick(int h, int mm, int ss, int TenMS, double Px);
	void __fastcall UpdateQty(nsOrderMessageDefine::SideEnum side, double Price, int Qty);
	void __fastcall Paint(void);
	void __fastcall ZoomIn(void);
	void __fastcall ZoomOut(void);
	void __fastcall SaveToFile(String FileName);
	double  __fastcall GetTickPrice( int Step );
	void __fastcall DeleteAllOrders( bool Sync );
	int __fastcall TickCount( void );
	TTickInfo* __fastcall GetTick( int Index );
	void  __fastcall ToTNFormat( double Px, String& PxStr );
__published:
	__property Font;
	__property Constraints;
	__property OnResize;
	__property Align;
	__property Anchors;
	__property Color;
	__property Cursor;
	__property Touch;
	__property OnGesture;
	__property OnKeyDown;
	__property OnDragDrop;
	__property OnDragOver;
	__property TCMarketDataStore* Store = {
		read = FStore, write = SetStore
	};
	__property Graphics::TBitmap *CancelAllBuy = {
		read = FCancelAllBuyBmp, write = SetCancelBuy
	};
	__property Graphics::TBitmap *CancelAllSell = {
		read = FCancelAllSellBmp, write = SetCancelSell
	};
	__property Graphics::TBitmap *CenterFilled = {
		read = FCenterFilledBmp, write = SetCenterFilled
	};
	__property Graphics::TBitmap *ReferenceBubble = {
		read = FRefBmpL, write = SetRefBubble
	};
	__property Graphics::TBitmap *BullBubble = {
		read = FBullBmpL, write = SetBullBubble
	};
	__property Graphics::TBitmap *BearBubble = {
		read = FBearBmpL, write = SetBearBubble
	};
	__property bool EnableDirect2D = {
		read = FEnableAccelerate, write = SetEnableD2D
	};
	__property bool ShowBubble = {
		read = FShowBubble, write = FShowBubble
	};
	__property bool ShowTickLine = {
		read = FShowTickLine, write = FShowTickLine
	};
	__property bool CenterFillPx = {
		read = FCenterFillPx, write = FCenterFillPx
	};
	__property bool CancelBeforeNew = {
		read = FCancelBeforeNew, write = FCancelBeforeNew
	};
	__property bool StepEnable = {
		read = FStepEnable, write = FStepEnable
	};
	__property int Step = {
		read = FStep, write = FStep
	};
	__property int StepCount = {
		read = FStepCount, write = FStepCount
	};
	__property int StrikePxDigi = {
		read = FStrikePxDigi
	};
	__property int DecimalLocator = {
		read = FDecimalLocator
	};
	__property bool ThumbChart = {
		read = FThumbChart, write = SetThumbChart
	};
	__property bool ReadOnly = {
		read = FReadOnly, write = FReadOnly
	};
	__property int MaxFPS = {
		read = FMaxFPS, write = SetFPS
	};
	__property int SecondsPerGrid = {
		read = FSecondsPerGrid, write = SetSecondsPerGrid
	};
	__property int TradingSeconds = {
		read = FCloseRefSec
	};
	__property System::Word KeyBuy = {
		read = FKeyBuy, write = FKeyBuy
	};
	__property System::Word KeySell = {
		read = FKeySell, write = FKeySell
	};
	__property System::Word KeyBuyMarket = {
		read = FKeyBuyMarket, write = FKeyBuyMarket
	};
	__property System::Word KeySellMarket = {
		read = FKeySellMarket, write = FKeySellMarket
	};
	__property System::Word KeyCancelAll = {
		read = FKeyCancelAll, write = FKeyCancelAll
	};
	__property System::Word KeyCancelBuy = {
		read = FKeyCancelBuy, write = FKeyCancelBuy
	};
	__property System::Word KeyCancelSell = {
		read = FKeyCancelSell, write = FKeyCancelSell
	};
	__property System::Word KeyBetterIncrese = {
		read = FKeyBetterIncrese, write = FKeyBetterIncrese
	};
	__property System::Word KeyBetterDecrese = {
		read = FKeyBetterDecrese, write = FKeyBetterDecrese
	};
	__property System::Word KeyCenterFillPx = {
		read = FKeyCenterFillPx, write = FKeyCenterFillPx
	};
	__property System::Word KeyNewOrder = {
		read = FKeyNewOrder, write = FKeyNewOrder
	};
	///< Event
	__property TOnNewOrderEvent OnNewOrder = {
		read = FOnNewOrder, write = FOnNewOrder
	};
	__property TOnNewOrdersEvent OnNewOrders = {
		read = FOnNewOrders, write = FOnNewOrders
	};
	__property TOnReduceQtyEvent OnReduceQty = {
		read = FOnReduceQty, write = FOnReduceQty
	};
	__property TOnReplacePxEvent OnReplacePx = {
		read = FOnReplacePx, write = FOnReplacePx
	};
	__property TOnDeleteAllEvent OnDeleteAllOrders = {
		read = FOnDeleteAllOrders, write = FOnDeleteAllOrders
	};
	__property TOnNewMarketOrderEvent OnNewMarketOrder = {
		read = FOnNewMarketOrder, write = FOnNewMarketOrder
	};
	__property TOnProfitAndAvgPxUpdateEvent OnProfitAndAvgPxUpdate = {
		read = FOnProfitAndAvgPxUpdate, write = FOnProfitAndAvgPxUpdate
	};
	__property TOnNetPositionUpdateEvent OnNetPositionUpdate = {
		read = FOnNetPositionUpdate, write = FOnNetPositionUpdate
	};
	__property TOnNewPingPongOrderEvent OnNewPingPongOrder = { read = FOnNewPingPongOrder, write = FOnNewPingPongOrder };
	__property TNotifyEvent OnCenterPx = {
		read = FOnCenterPx, write = FOnCenterPx
	};
	__property TNotifyEvent OnUILock = { read = FOnUILock, write = FOnUILock };
	///< PingPong
	__property bool PingPong = { read = FEnablePingPong, write = EnablePingPong };
	__property int  BuyPingPongTick = { read = FBuyPingPongTick, write = SetBuyPingPongTick };
	__property int  SellPingPongTick = { read = FSellPingPongTick, write = SetSellPingPongTick };
	__property int  CancelAllDelay = { read = FClxAllDelayMS, write = FClxAllDelayMS };
	///< Color
	///< Top header
	__property TColor FixedRowColor = {
		read = FFixedRowColor, write = FFixedRowColor
	};
	__property TColor FixedRowBKColor = {
		read = FFixedRowBKColor, write = FFixedRowBKColor
	};
	///< All grid line
	__property TColor GridColor = {
		read = FGridColor, write = FGridColor
	};
	///< Centrer price row color
	__property TColor TickUpperColor = {
		read = FTickUpperColor, write = FTickUpperColor
	};
	__property TColor TickLowerColor = {
		read = FTickLowerColor, write = FTickLowerColor
	};
	__property TColor TickBKColor = {
		read = FTickBKColor, write = FTickBKColor
	};
	__property TColor FillColor = {
		read = FFillColor, write = FFillColor
	};
	__property TColor FillBKColor = {
		read = FFillBKColor, write = FFillBKColor
	};
	///< Better cell
	__property TColor BetterBuyColor = {
		read = FBetterBuyColor, write = FBetterBuyColor
	};
	__property TColor BetterBuyBKColor = {
		read = FBetterBuyBKColor, write = FBetterBuyBKColor
	};
	__property TColor BetterSellColor = {
		read = FBetterSellColor, write = FBetterSellColor
	};
	__property TColor BetterSellBKColor = {
		read = FBetterSellBKColor, write = FBetterSellBKColor
	};
	///< Colors for depth(Market), Place order(User), Filled rows
	__property TColor BidDepth5BKColor = {
		read = FBidDepth5BKColor, write = FBidDepth5BKColor
	};
	__property TColor BidDepth5Color = {
		read = FBidDepth5Color, write = FBidDepth5Color
	};
	__property TColor AskDepth5BKColor = {
		read = FAskDepth5BKColor, write = FAskDepth5BKColor
	};
	__property TColor AskDepth5Color = {
		read = FAskDepth5Color, write = FAskDepth5Color
	};
	__property TColor BuyBKColor = {
		read = FBuyBKColor, write = FBuyBKColor
	};
	__property TColor BuyColor = {
		read = FBuyColor, write = FBuyColor
	};
	__property TColor SellBKColor = {
		read = FSellBKColor, write = FSellBKColor
	};
	__property TColor SellColor = {
		read = FSellColor, write = FSellColor
	};
	__property TColor BuyFillColor = {
		read = FBuyFillColor, write = FBuyFillColor
	};
	__property TColor SellFillColor = {
		read = FSellFillColor, write = FSellFillColor
	};
	__property TColor BuyFillBKColor = {
		read = FBuyFillBKColor, write = FBuyFillBKColor
	};
	__property TColor SellFillBKColor = {
		read = FSellFillBKColor, write = FSellFillBKColor
	};
	__property TOnStepChangeEvent OnStepChange = { read = FOnStepChange, write = FOnStepChange };
	__property TOnErrorMessage OnErrorMessage = { read = FOnErrorMessage, write = FOnErrorMessage };
	__property TOnAskCancelCount  OnAskCancelCount = { read = FOnAskCancelCount, write = FOnAskCancelCount };
	__property bool ChangeFillColor = { read = FChangeFillColor, write = FChangeFillColor };
__published:
	__property bool DebugMode   = { read = FDebugMode, write = FDebugMode };
	__property bool ShowFilled  = { read = FShowFilled, write = SetShowFilled };
	__property bool ShowFillQty = {	read = FShowFillQty, write = FShowFillQty };
	__property bool ShowSecQty  = { read = FShowSecQty, write = FShowSecQty };
	///< Technical Indicators

	///<  Bollinger Bands
	__property bool   ShwStdDev = { read = FShwStdDev, write = FShwStdDev };
	__property bool   BollingerBands = { read = FBollingerBands, write = FBollingerBands };
	__property int    BollingerSec = { read = FBollingerSec, write = FBollingerSec };
	__property double BandsWidth1 = { read = FBandsWidth, write = FBandsWidth };
	__property double BandsWidth2 = { read = FBandsWidth2, write = FBandsWidth2	};
	__property double BandsWidth3 = { read = FBandsWidth3, write = FBandsWidth3	};
	__property TColor BollingerBandsMAColor = {	read = FBBMAColor, write = FBBMAColor };

	///<  Moving Averages
	__property bool   ShowMA   = { read = FShowMA, write = FShowMA };
	__property int    MA1Sec   = { read = FMA1Sec, write = FMA1Sec };
	__property int    MA2Sec   = { read = FMA2Sec, write = FMA2Sec };
	__property int    MA3Sec   = { read = FMA3Sec, write = FMA3Sec };
	__property TColor MA1Color = { read = FMA1Color, write = FMA1Color };
	__property TColor MA2Color = { read = FMA2Color, write = FMA2Color };
	__property TColor MA3Color = { read = FMA3Color, write = FMA3Color };

};
// ---------------------------------------------------------------------------
#endif
