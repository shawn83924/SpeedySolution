//---------------------------------------------------------------------------

#ifndef CandleStickChartH
#define CandleStickChartH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include "MarketDataStore.h"
#include "OrderStore.h"
#include "StockChart.h"
#include <Gdiplus.h>
#include "CandleStickGraphs.h"
//---------------------------------------------------------------------------
class TCandleStickChart;
//---------------------------------------------------------------------------
typedef enum maPrice
{
	maClose = 0,
	maOpen  = 1,
	maMax   = 2,
	maMin   = 3,
	maAvg   = 4

}MAPrice;
//---------------------------------------------------------------------------
typedef enum mvType
{
	mvSMA = 0,
	mvWMA = 1,
	mvEMA = 2

}MVType;
//---------------------------------------------------------------------------
typedef enum scaleType
{
	stAuto = 0,
	stXOnly = 1,
	stYOnly = 2,
	stXYBoth = 3

}ScaleType;
//---------------------------------------------------------------------------
typedef enum pxgridType
{
	pgtFix  = 0,
	pgtAuto = 1,
	pgtAOE  = 2

}PxGridType;
//---------------------------------------------------------------------------
typedef enum timegridType
{
	tgt10min  = 0,
	tgt15min  = 1,
	tgt30min  = 2,
	tgtAuto   = 3

}TimeGridType;
//---------------------------------------------------------------------------
typedef enum sectionType
{
	stNoSection = 0,
	stGoldenSection = 1,
	stNaturalSection = 2

}SectionType;
//---------------------------------------------------------------------------
typedef enum editType
{
    etDefault = 0,
	etVLine   = 1,
	etHLine   = 2,
	et2PLine  = 3,
	etDelete  = 4,
	etText    = 5,
    et2PRect  = 6

} EditType;
//---------------------------------------------------------------------------
typedef enum curType
{
	ctCross   = 1,
	ctVLine   = 2,
	ctHLine   = 3,

} CursorType;
//---------------------------------------------------------------------------
typedef enum pivotType
{
	ptLow      = 0,
	ptHigh     = 1,
	ptRoot     = 2,
	ptUnknown  = 3

} PivotType;

//---------------------------------------------------------------------------
class TPivot
{
public:
	int        Index;
	int        PxIndex;
	double     Price;
	PivotType  Type;
public:
	TPivot( int Idx, double Px, int PxIdx, PivotType tp = ptUnknown ):Index( Idx ),PxIndex( PxIdx ),Price( Px ),Type( tp ){}
	int TPivot::Distance( const TPivot& PreHL )
	{
		int dist = Index - PreHL.Index;
		if( dist < 0 )
			return -1* dist;
		return dist;
	}
	void SetIsHeigh( PivotType tp ){ Type = tp; }
};
//---------------------------------------------------------------------------
class FilledCompare : public UFC::CompareInterface
{
public:
	int Compare( void* elem1, void* elem2 );
};
//---------------------------------------------------------------------------
class TCandleHint : public TObject
{
private:
	float  FFillPx;
	float  FLastFillPx;
	int    FPxDigi;
	int	   FQty;
	bool   FIsEmpty;
	String FTime;
	TColor FColor;
	TTickList* FTickList;
public:
	__fastcall TCandleHint();
	void __fastcall Paint( int x, int y, int MaxX, TCanvas* ,TColor FontCol );
	void __fastcall SetData( float Fill, float LastFill, int Qty, int Digi, TColor Col, const String& Time );
	void __fastcall Empty( void ) { FIsEmpty = true; }
	void __fastcall SetTickList( TTickList* List ) { FTickList = List; }
};
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TOnDeleteEvent)(System::TObject* Sender, nsOrderMessageDefine::SideEnum side, double Price );
typedef void __fastcall (__closure *TOnKBar) (System::TObject* Sender, int Hour, int Minute );
typedef void __fastcall(__closure * TOnReplacePx)(System::TObject * Sender, nsOrderMessageDefine::SideEnum side,double Price, double NewPrice);
typedef void __fastcall(__closure * TOnReplaceStopPx)(System::TObject * Sender, nsOrderMessageDefine::SideEnum side, nsOrderMessageDefine::OrderTypeEnum Type, int Qty, double Price, double NewPrice,double ExecPx);
typedef void __fastcall (__closure *TOnNewOrder)(System::TObject* Sender, nsOrderMessageDefine::SideEnum side, double Price );
typedef void __fastcall (__closure *TOnNewStopOrder)(System::TObject* Sender, nsOrderMessageDefine::SideEnum side, double Px, nsOrderMessageDefine::OrderTypeEnum Type, double ExecPrice );
typedef void __fastcall (__closure *TOnClosePosition)(System::TObject* Sender, nsOrderMessageDefine::SideEnum side, int Pos );
typedef void __fastcall (__closure *TOnNewText)(System::TObject* Sender, TText* NewText, bool& Accept );
//---------------------------------------------------------------------------
class PACKAGE TCandleStickChart : public TCustomControl, public TPositionChangeListener, public TOrderQtyListener
{
private:
	int    FDispPixelsPerTick;
	int    FCandleWidth;       ///<  KBar width in virtual desktop
	int    FPixelsPerTick;     ///<  Pixel per tick in virtual desktop, fix value 10
	int    FMinutesPerStick;   ///<  Minute Per candle stick 1, 5, 10, 30
	int    FDayCandle;         ///<  How many K Bars a trading day has. (FDayCandle = FDayMinutes / FMinutesPerStick)
	int    FTotalCandle;       ///<  Total K Bars DAYS have.
	int    FVirtualWidth;      ///<  FSecondCount * FPixelsPerMinute
	int    FVirtualHeight;     ///<  FTicksCount * FPixelsPerTick
	int    FViewPortX;         ///<  View port offset X
	int    FViewPortY;         ///<  View port offset Y
	int    FViewPortWidth;     ///<  View port width
	int    FViewPortHeight;    ///<  View port Height
	int    FViewPortWidthMax;  ///<  View port width max.( up to see 300 KBars) MAX_CANDLE_VIEW_PORT =300
	int    FViewPortWidthMin;  ///<  View port width min.( at least to see 10 KBars) MIN_CANDLE_VIEW_PORT=10
	double FViewPortToControlX; ///< Viewport X map to screen X ratio
	double FViewPortToControlY; ///< Viewport Y map to screen Y ratio
	int    FYAxisOffset;        ///< Y axis Right Side offset.( for display price text)
	int    FXAxisOffset;        ///< X axis bottom offset.( for display Time text)
	int    FVTodayX;
private:
	int                FFontSize;
	int                FOpenTimeMin;
	int                FCloseTimeMin;
	int                FDayMinutes;        ///< How many minutes a trading day has.
	int                FDigit;
	int                DAYS;
	int                FDecimalLocator;
	UFC::PStringList   FDateList;
	bool               FIsChangingSymbol;
	bool               FHasOffhour;
	int                FEndIndex;
	///< Px for tick
	double             FTickMaxPx;
	double             FTickMinPx;
	/// Qty info
	int                FShowQty;
    double             FChartPercent;
	int                FMaxQty;
	TTickList*         FTickList;
	BasicInformation*  FInfo;
	ExchangeInfo*      FExInfo;
	TCMarketDataStore* FStore;
	TImageList*        FEmojis;
	TColor             FLastCandleColor;
	Graphics::TBitmap* FBuffer;
	Graphics::TBitmap* FQtyBuffer;
	TInfoHint*         FHintwin;
	TCandleHint*       FCandleHintwin;
	TTextHint*         FTimeHint;
	TTextHint*         FPriceHint;
	TOrderHint*        FOrderHint;
	int 			   FMouseIndex;
	int 			   FLastIndex;
	int				   FLastTick;
	int				   FLastPxTick;
	DWORD  			   FLastPaint;
	int    			   FMaxFPS;
	int    			   FPaintInterval;
	TColor			   FBullColor;
	TColor			   FBearColor;
	TColor             FCandleLineColor;
	TColor             FGridColor;
	TColor             FTextColor;
	TPoint			   FLastAvgPoint;
	///< Moving Average
	bool			   FShowMA1;
	bool			   FShowMA2;
	bool			   FShowMA3;
	int 			   FMACount1;
	int                FMACount2;
	int				   FMACount3;
	TColor			   FMAColor1;
	TColor			   FMAColor2;
	TColor			   FMAColor3;
	TColor             FLineColor;
	MAPrice			   FMAPrice1;
	MAPrice			   FMAPrice2;
	MAPrice			   FMAPrice3;
	MVType			   FMVType1;
	MVType			   FMVType2;
	MVType			   FMVType3;
	int				   FBearTick;
	ScaleType		   FScaleType;
	bool			   FGradientCandle;
	PxGridType         FPxGridType;
    TimeGridType       FTimeGridType;
	int                FTicksPerGrid;
	int                FRightSpace;
	CursorType         FCursorType;
	///< Performance debug
	bool               FDebugMode;
	int                FMaxUseMS;
	int                FFrameCount;
	double             FCurrentFPS;
	TTimer*            FTimer;
	DWORD              FBeginFPSTick;
private: ///< AOE Box
	bool   FShowBox;
	bool   FHasBox;
	bool   FBoxExists;
	int    FVLeft;
	int    FVRight;
	int    FVTop;
	int    FVBottom;
	int    FDayHY;
	int    FDayLY;
	int    FRefY;
	bool   FBoxDirUp;
	bool   FShowDayHL;
	bool   FUseHLSection;
	double FHLDiff;
	double FDayHigh;
	double FDayLow;
	double FRefPx;
	double FRiskRatio;
	double FWasteRatio;
	int    FEmojiIndex;
	SectionType     FHLSection;
	TColor          FSectionLineColor;
	double FSkyPx;
	double FLandPx;
	int    FSkyY;
	int    FLandY;
	bool   FExableSkyLand;
	bool   FIsStock;
	bool   FCanUpdate;
	TPositionLine    FPosition;
	THLine           FSectionHLine;
	THLine           FSectionLLine;
	THLine*          FSelectSectionLine;
	TOnKBar FOnKBar;
	TOnDeleteEvent   FOnDelete;
	TOnDeleteEvent   FOnDeleteStopOrder;
	TOnReplacePx     FOnReplacePx;
	TOnReplaceStopPx FOnStopOrderReplacePx;
	TOnNewOrder      FOnNewOrder;
	TOnNewStopOrder  FOnNewStopOrder;
	TOnClosePosition FOnClosePosition;
	TOnNewText       FOnNewText;
	void __fastcall SetSectionLineColor( TColor SLColor );
	void __fastcall SetUseHLSection( bool Enable );
	void __fastcall SetHLSection( SectionType Section );
	void __fastcall SetShowBox( bool Show );
	void __fastcall SetShowDayHL( bool Show );
	void __fastcall SetShowAvgPx( bool Show );
	void __fastcall CheckDayHL( TMinutesTickInfo& Info );
	void __fastcall FindBox( int index, int Minutes, TMinutesTickInfo& Info );
	void __fastcall DrawBox( TCanvas* canvas );
	void __fastcall DrawPxLine( TCanvas* canvas, TColor LineC, int StartX, int PxY, double Px, bool ShowPercent, bool TextUp );
	void __fastcall BoxGrade( int index, int Minutes, double HL );
	void __fastcall DrawSection(  TCanvas* canvas, int DHY, int DLY );
	void __fastcall SetSkyPx( double );
	void __fastcall SetLandPx( double );
	void __fastcall SetExableSkyLand( bool );
private:
	double FStopPx;
	void __fastcall InitPopupMenu( void );
	void __fastcall BuyStopClicked(TObject* Sender);
	void __fastcall SellStopClicked(TObject* Sender);
	void __fastcall CloseAllClicked(TObject* Sender);
private: ///< Show Trade position.
	UFC::PList              FFilledList;
	FilledCompare           FFilledCompare;
	void __fastcall DrawFilled( TCanvas* canvas );
private: ///< Graph tools.
	bool                    FEditMode;
	bool                    FAddingLine;
	int                     FBeginTick;
	int                     FBeginIndex;
	int                     FBeginX;
	int                     FBeginY;
	EditType                FEditType;
	UFC::PtrList<TKBarGraph> FGraphs;
	TKBarGraph*              FMoveGraph;
	int                      FObjDeltaX;
	int                      FObjDeltaY;
	void __fastcall AddLine( int x, int y );
    void __fastcall AddRect( int x1, int x2, int y1, int y2 );
	void __fastcall AddText( int x, int y );
	void __fastcall DrawGraphs( TCanvas* canvas );
	void __fastcall SetMouseDownCursor( void );
	void __fastcall SetMouseUpCursor( void );
	TKBarGraph* __fastcall SelectObjectCursor( int X, int Y, TCursor NormalCursor  );
	void __fastcall DeleteGraph(  int X, int Y  );
	bool __fastcall DeleteGraphs(  const TRect& r  );
	void __fastcall CreateGraph( int x, int y );
	void __fastcall ClearGraphs( void );
	bool __fastcall HitHLLine(  int X, int Y );
private:
	TIntegerDynArray FQtys;
	TDoubleDynArray  FMaxPx;
	TDoubleDynArray  FMinPx;
	TDoubleDynArray  FOpenPx;
	TDoubleDynArray  FClosePx;
	TBooleanDynArray FHaveData;
	///< DrawArray
	TIntegerDynArray FDrawQty;
	TDoubleDynArray  FDrawMaxPx;
	TDoubleDynArray  FDrawMinPx;
	TDoubleDynArray  FDrawOpenPx;
	TDoubleDynArray  FDrawClosePx;
	TBooleanDynArray FDrawHaveData;
	System::DynamicArray<TPoint> FTopLeftArray;
	System::DynamicArray<TPoint> FBottomRightArray;
	System::DynamicArray<TPoint> FMaxArray;
	System::DynamicArray<TPoint> FMinArray;
	double FViewMaxPx;
	double FViewMinPx;
private:
	bool FExablePivot;
	int FCountedBar;
	int FExtDepth;
	double FExtDeviation;
	int FExtBackstep;
	bool FFirstPivotExists;
	UFC::List<TPivot> FPivots;
	void __fastcall AddRootPivot( void );
	void __fastcall AddFirstPivot( int i, double CurrPx );
	void __fastcall Pivot( int LastI );
	bool __fastcall FindHL( int index, int& HIndex, double& HPx, int& LIndex, double& LPx);
	void __fastcall DrawPivots( TCanvas* canvas );
	void __fastcall SetExablePivot( bool Draw );
	void __fastcall SetPivotDeviation( double Deviation );
private:
	TRect  FViewPrtRect;
	bool   FMouseDown;
	TPoint FMousePos;
	TPoint FMouseDownPos;
	TPoint FMouseDownViewPortPos;
	int    FLastMouseX;
	int    FLastMouseY;
	TRect  FDrawRect;
	TRect  FPriceRect;
	TRect  FTimeRect;
	bool   FGDIPlus;
	///< Stop Order Setting
	nsOrderMessageDefine::OrderTypeEnum FBuyStopOrderOrdType;
	nsOrderMessageDefine::OrderTypeEnum FSellStopOrderOrdType;
	int FBuyStopTick;
	int FSellStopTick;
	bool FEnableTrade;
	void __fastcall SetEnableTrade( bool enable );
private:
	void __fastcall Paint( void ); ///< call CheckViewPortSize and PaintGDI
	void __fastcall PaintFPS( void ); ///< call paint or set the OnRepaintTimer
	void __fastcall OnRepaintTimer(TObject *Sender ); ///< call Paint
	void __fastcall CheckViewPortSize( void );
private:
	///< Paint function
	void __fastcall PaintGDI( void );
	void __fastcall DrawBackground(  TCanvas* canvas, TRect& BufferRect, bool IsFocus );
	int __fastcall AutoIntervalPerGrid ( TCanvas* canvas, int HalfTimeStrWidth );
	void __fastcall DrawTimeLine( TCanvas* canvas );
	void __fastcall DrawTickGrid( TCanvas* canvas );
	void __fastcall DrawSkyLand( TCanvas* canvas );
	void __fastcall DrawPositionLine( TCanvas* canvas );
	void __fastcall DrawViewPort( TCanvas* canvas );
	void __fastcall DrawAxis( TCanvas* canvas, TRect& BufferRect );
	void __fastcall DrawCross( int X, int Y );
	void __fastcall DrawRect( int X, int Y , int width = 2);
	void __fastcall EraseCross( bool ClearFlag );
private: ///< Called in DrawCross
	void __fastcall DrawNotLine(TCanvas *canvas,int x,int y,int x1,int y1);
private: ///< Called in DrawViewPort
	///< Draw Qty Bars.
	void __fastcall DrawQtyGrid( TCanvas* canvas,  int QtyUpperLineY );
	void __fastcall DrawBar( TCanvas* canvas, TRect& QtyRect, TColor BarCol );
	void __fastcall DrawQtyBar( TCanvas* canvas );
	void __fastcall DrawQtyBars( TCanvas* canvas, int BeginIndex, int EndIndex,int QtyUpperLineY );
	///< Draw Candle sticks.
	void __fastcall DrawCandle( TCanvas* canvas, TPoint& MaxPoint, TPoint& MinPoint, TRect& CandleRect, TColor CandleCol );
	void __fastcall DrawCandles( TCanvas* canvas, int BeginIndex, int EndIndex, int QtyUpperLineY );
	///< Draw MA funtions
	void __fastcall DrawSMA( TCanvas* canvas, int Count, MAPrice maPrice, TColor color );
	void __fastcall DrawSMAGDIPlus( TCanvas* canvas, int Count, MAPrice maPrice, TColor color, int BeginIndex, int EndIndex );
	void __fastcall DrawSMAGDI( TCanvas* canvas, int Count, MAPrice maPrice, TColor color, int BeginIndex, int EndIndex );
	void __fastcall SMAPoint( int Index, int DataCount, double Sum, TPoint& AvgPoint );
	int __fastcall  FirstAvgPt( int Index, int MACount, MAPrice maPrice, double& Sum );
	void __fastcall NextAvgPt( int Index, int MACount, MAPrice maPrice, int& DataCount, double& Sum );
private:
	void __fastcall Loaded( void );
	void __fastcall Clear1MinKData( void );
	void __fastcall ClearOrderData( void );
	void __fastcall CheckViewPortPosition( void );
	void __fastcall EraseBackground( TMessage &Msg );
	void __fastcall SetDrawPoint( int Index );
	void __fastcall ClearDrawData( void );
	double __fastcall MaxPxInView( void );
	double __fastcall MinPxInView( void );
	void __fastcall AdjustViewPortHeight( void );
	int __fastcall GetMaxQty( void );
	void __fastcall ChangeCursor( void );
	void __fastcall HandleMouseLeave( TObject* );
	TRect __fastcall ToScreenRect( int l, int t, int r, int b );
	int  __fastcall GetDataYPos( int Index, int Y );
	int  __fastcall GetDataXPos( int Index, int X );
	void __fastcall ShowTimeHint( int X, int Y, const UFC::AnsiString& Time );
	void __fastcall ShowPriceHint( int X, int Y, const String& PxStr );
	void __fastcall ShowOrdereHint( int Y );
	void __fastcall ShowDataHint( TPoint& ScreenPx, TRect& ScreenRect, int Index, const UFC::AnsiString& Time );
	void __fastcall ShowInfoHint( int X, int Y, TPoint& ScreenPx );
	void __fastcall HideInfoHint( void );
	void __fastcall HideOrdereHint( void );
	void __fastcall HideTimeHint( void );
	void __fastcall HidePriceHint( void );
	void __fastcall HideDataHint( void );
	void __fastcall AlignmentCheck( int X, int& Y );
	void __fastcall ShowTickHint( int Index );
	void __fastcall SetCursor( TCursor WinCursor, TCursor ScrCursor );
public:
	inline int __fastcall GetRightBottomX( void );
	inline int __fastcall GetRightBottomY( void );
private: ///< Set/Get properties.
	void __fastcall SetMinutesPerStick( int Minute );
	void __fastcall SetStore( TCMarketDataStore* Store );
	void __fastcall SetTicksPerGrid( int tick );
	void __fastcall SetPixelsPerTick( int Pixels );
	void __fastcall SetShowQtyBar( int ShowQtyBar );
	void __fastcall SetFPS( int FPS );
	void __fastcall SetBullColor( TColor color );
	void __fastcall SetBearColor( TColor color );
	void __fastcall SetCandleLineColor( TColor color );
	void __fastcall SetTextColor( TColor color );
	void __fastcall SetGridColor( TColor color );
	void __fastcall SetMAColor1( TColor color );
	void __fastcall SetMAColor2( TColor color );
	void __fastcall SetMAColor3( TColor color );
	void __fastcall SetShowMA1( bool ShowMA );
	void __fastcall SetShowMA2( bool ShowMA );
	void __fastcall SetShowMA3( bool ShowMA );
	void __fastcall SetMAPrice1( MAPrice PxType );
	void __fastcall SetMAPrice2( MAPrice PxType );
	void __fastcall SetMAPrice3( MAPrice PxType );
	void __fastcall SetMACount1( int Count );
	void __fastcall SetMACount2( int Count );
	void __fastcall SetMACount3( int Count );
	void __fastcall SetMVType1( MVType type );
	void __fastcall SetMVType2( MVType type );
	void __fastcall SetMVType3( MVType type );
	void __fastcall SetPxGridType( PxGridType Type );
	void __fastcall SetTimeGridType( TimeGridType Type );
	void __fastcall SetScaleType( ScaleType Type );
	void __fastcall SetGradientCandle( bool GradientCandle );
	void __fastcall SetDays( int days );
	void __fastcall SetXScale( double Ratio );
	void __fastcall SetYScale( double Ratio );
	double __fastcall GetXScale( void );
	double __fastcall GetYScale( void );
	void __fastcall SetEditMode( bool edit );
	void __fastcall SetEditType( EditType et );
private:
	void __fastcall Zoom( bool IsZoomIn );
	void __fastcall ZoomAuto( bool IsZoomIn );
	void __fastcall ZoomX( bool IsZoomIn );
	void __fastcall ZoomY( bool IsZoomIn );
	void __fastcall ZoomBoth( bool IsZoomIn );
private:
	void __fastcall MouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta, const Types::TPoint &MousePos, bool &Handled);
	void __fastcall MouseEnter( TObject *Sender );
private:
	void __fastcall CalculateKBarFistTime( int DrawIndex, TMinutesTickInfo& Info );
	void __fastcall CalculateKBarUpdate( int DrawIndex, int RefMin, TMinutesTickInfo& Info );
	void __fastcall PenViewport( int X, int Y);
protected:
	virtual void __fastcall WndProc( TMessage &Msg );
	DYNAMIC void __fastcall Resize( void );
	DYNAMIC void __fastcall MouseDown( Controls::TMouseButton Button, Classes::TShiftState Shift,int X,int Y);
	DYNAMIC void __fastcall MouseMove( Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift,int X,int Y);
	DYNAMIC void __fastcall KeyDown(System::Word &Key, Classes::TShiftState Shift);
private:
	bool   FShowAvgPx;
	bool   FReplacePx;
	TWorkingLine* FHitWorkingOrder;
	UFC::PHashMap<int,TWorkingLine*> FWorkingOrders;
	HitResult __fastcall HitPositionLine( int x,int y );
private: ///< Implement interface TPositionChangeListener
	virtual void OnFill(int Position, double BuyArvPx, double SellArvPx, int BuyQty, int SellQty,
						int BuyOpenInterestQty, int SellOpenInterestQty,
						DynamicArray<void*>& PosStatisticsArray, int NetPosition, double FloatingProfit );
	virtual void OnProfit(double AveragePrice, double ProfitAmount);
	virtual void OnFill(nsOrderMessageDefine::SideEnum Side, double Px, int Qty, int TickCount){}  //For Ping Pong
private: ///< Implement interface TOrderQtyListener
	virtual void OrderQtyChanged( nsOrderMessageDefine::MarketEnum Market,
								  const String& Symbol,
								  nsOrderMessageDefine::SideEnum Side,
								  double Px, int Qty);
	virtual void StopOrderQtyChanged( nsOrderMessageDefine::MarketEnum Market,
									  const String& Symbol, double StopPx,
									  nsOrderMessageDefine::SideEnum Side,
									  double Px, int Qty,
									  nsOrderMessageDefine::OrderTypeEnum OrderType);
public:
	__fastcall TCandleStickChart(TComponent* Owner);
	__fastcall ~TCandleStickChart( void );
	bool __fastcall SetData( int Day, TMinutesTickInfo& Info, bool NeedPaint );
	void __fastcall SetSymbol( BasicInformation* Info );
	void __fastcall SetTickEx( double MaxPx, double MinPx );
	void __fastcall SetDate( int Index, const UFC::AnsiString& Date );
	void __fastcall CenterLastTick( void );
public: ///< Show trade points functions.
	void __fastcall AddFilled( TFilledData& Filled );
	void __fastcall ClearFilled( void );
public:
	int __fastcall  XPosToIndex( int X );
	int __fastcall  YPosToTick( int Y );
	void __fastcall YPosToPrice( int Y, String& Str );
	double __fastcall YPosToPrice( int Y );
	int __fastcall  IndexToViewPortX( int index );
	int __fastcall  TickToViewPortY( int index );
	int __fastcall  PriceToTick( double Price );
	void __fastcall SaveToClipboard( void );
	double __fastcall GetTickPrice( int Step );
	void  __fastcall GetPriceTickString( int PxTick,String& Str );
private:
	int FBaseIndex;
public:
	double __fastcall Open( int i );
	double __fastcall Close( int i );
	double __fastcall High( int i );
	double __fastcall Low( int i );

	double __fastcall OpenD( void );
	double __fastcall CloseD( void  );
__published:
	__property TOnKBar OnKBar = { read = FOnKBar, write = FOnKBar };
	__property TOnDeleteEvent OnDelete = { read = FOnDelete, write = FOnDelete };
	__property TOnDeleteEvent OnDeleteStopOrder = { read = FOnDeleteStopOrder, write = FOnDeleteStopOrder };
	__property TOnReplacePx   OnReplacePx = { read = FOnReplacePx, write = FOnReplacePx };
	__property TOnReplaceStopPx   OnStopOrderReplacePx = { read = FOnStopOrderReplacePx, write = FOnStopOrderReplacePx };
	__property TOnNewOrder        OnNewOrder = { read = FOnNewOrder, write = FOnNewOrder };
	__property TOnNewStopOrder    OnNewStopOrder = { read = FOnNewStopOrder, write = FOnNewStopOrder };
	__property TOnClosePosition   OnClosePosition = { read = FOnClosePosition, write = FOnClosePosition };
	__property TOnNewText         OnNewText = { read = FOnNewText, write = FOnNewText };
	__property Align;
	__property Color;
	__property OnDragDrop;
	__property OnDragOver;
	__property Font;
	__property TCMarketDataStore* Store = { read = FStore, write = SetStore };
	__property int MinutesPerStick = { read = FMinutesPerStick, write = SetMinutesPerStick };
	__property int PixelsPerTick = { read = FDispPixelsPerTick, write = SetPixelsPerTick };
	__property int KBarDays = { read = DAYS, write = SetDays };
	__property int ShowQtyBar = { read = FShowQty, write = SetShowQtyBar };
	__property int  TicksPerGrid = { read = FTicksPerGrid, write = SetTicksPerGrid };
	__property PxGridType PxGrid = { read = FPxGridType, write = SetPxGridType };
	__property TimeGridType TimeGrid = { read = FTimeGridType, write = SetTimeGridType };
	__property bool ShowBox = { read = FShowBox, write = SetShowBox };
	__property bool ShowDayHL = { read = FShowDayHL, write = SetShowDayHL };
	__property int MaxFPS = { read = FMaxFPS, write = SetFPS };
	__property TColor BullColor = { read = FBullColor, write = SetBullColor };
	__property TColor BearColor = { read = FBearColor, write = SetBearColor };
	__property TColor CandleLineColor = { read = FCandleLineColor, write = SetCandleLineColor };
	__property TColor GridColor = { read = FGridColor , write  = FGridColor };
	__property TColor TextColor = { read = FTextColor , write = SetTextColor };
	__property TColor MAColor1 = { read = FMAColor1, write = SetMAColor1 };
	__property TColor MAColor2 = { read = FMAColor2, write = SetMAColor2 };
	__property TColor MAColor3 = { read = FMAColor3, write = SetMAColor3 };
	__property TColor LineColor = { read = FLineColor, write = FLineColor };
	__property bool ShowMA1 = { read = FShowMA1, write = SetShowMA1 };
	__property bool ShowMA2 = { read = FShowMA2, write = SetShowMA2 };
	__property bool ShowMA3 = { read = FShowMA3, write = SetShowMA3 };
	__property int MACount1 = { read = FMACount1, write = SetMACount1 };
	__property int MACount2 = { read = FMACount2, write = SetMACount2 };
	__property int MACount3 = { read = FMACount3, write = SetMACount3 };
	__property MAPrice MAPrice1 = { read = FMAPrice1, write = SetMAPrice1 };
	__property MAPrice MAPrice2 = { read = FMAPrice2, write = SetMAPrice2 };
	__property MAPrice MAPrice3 = { read = FMAPrice3, write = SetMAPrice3 };
	__property MVType MVType1 = { read = FMVType1, write = SetMVType1 };
	__property MVType MVType2 = { read = FMVType2, write = SetMVType2 };
	__property MVType MVType3 = { read = FMVType3, write = SetMVType3 };
	__property int          DecimalLocator  = { read = FDecimalLocator };
	__property ScaleType Scale = { read = FScaleType, write = SetScaleType };
	__property EditType Edit = { read = FEditType, write = SetEditType };
	__property bool GradientCandle = { read = FGradientCandle, write = SetGradientCandle };
	__property bool DebugMode   = { read = FDebugMode, write = FDebugMode };
	__property bool GDIPlus     = { read = FGDIPlus, write = FGDIPlus };
	__property TImageList* EmojiIcons = { read = FEmojis, write = FEmojis };
	__property double XScale = { read = GetXScale, write = SetXScale };
	__property double YScale = { read = GetYScale, write = SetYScale };
	__property SectionType HLSection = { read = FHLSection, write = SetHLSection };
	__property TColor  SectionLineColor = {	read = FSectionLineColor, write = SetSectionLineColor };
	__property bool EditMode = { read = FEditMode, write = SetEditMode };
	__property int RightSpace = { read = FRightSpace, write = FRightSpace };

	__property bool   ExableSkyLand = { read = FExableSkyLand, write = SetExableSkyLand };
	__property double SkyPx    = { read = FSkyPx, write = SetSkyPx };
	__property double LandPx   = { read = FLandPx, write = SetLandPx };

	__property bool   DayHLSection   = { read = FUseHLSection, write = SetUseHLSection };
	__property bool   ExablePivot    = { read = FExablePivot, write = SetExablePivot };
	__property bool   ShowAvgPx      = { read = FShowAvgPx, write = SetShowAvgPx };
	__property double PivotDeviation = { read = FExtDeviation, write = SetPivotDeviation };
	__property int    Digit          = { read = FDigit };
	__property int    FontSize       = { read = FFontSize };

	///< Stop Order setting
	__property int  BuyStopTick = { read = FBuyStopTick, write = FBuyStopTick };
	__property int  SellStopTick = { read = FSellStopTick, write = FSellStopTick };
	__property nsOrderMessageDefine::OrderTypeEnum BuyStopOrderType = { read = FBuyStopOrderOrdType, write = FBuyStopOrderOrdType };
	__property nsOrderMessageDefine::OrderTypeEnum SellStopOrderType = { read = FSellStopOrderOrdType, write = FSellStopOrderOrdType };
	__property bool EnableTrade = { read = FEnableTrade, write = SetEnableTrade };

};
//---------------------------------------------------------------------------
#endif
