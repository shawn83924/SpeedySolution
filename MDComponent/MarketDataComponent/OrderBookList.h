//---------------------------------------------------------------------------

#ifndef OrderBookListH
#define OrderBookListH
//---------------------------------------------------------------------------
#include "OrderMessageDefine.h"
#include "MarketDataStore.h"
#include "OrderStore.h"
#include "StopOrderSetting.h"
#include "OrderStore_OCO.h"
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
typedef enum
{
	satSymbol = 0,
	satDepth  = 1,
	satKChart = 2

} TSelectActionType;
//---------------------------------------------------------------------------
typedef enum
{
	bptLastPx = 0,
	bptAsk1Px = 1,
	bptBid1Px = 2

} TBetterPxType;
//---------------------------------------------------------------------------
typedef enum
{
	csBoth = 0,
	csBuy  = 1,
	csSell = 2

} TCancelSide;
//---------------------------------------------------------------------------
typedef enum colName
{
	BUY_FILL_COL		= 0,
	BUY_CONDITION_COL	= 1,
	BUY_OCODEL_COL      = 2,
	BUY_OCO_COL         = 3,
	BUY_DEL_BTN_COL		= 4,
	BUY_ORDER_COL		= 5,
	BUY_MK_COL			= 6,
	BUY_ORDER_BTN_COL	= 7,
	PRICE_COL 			= 8,
	SELL_ORDER_BTN_COL 	= 9,
	SELL_MK_COL 		= 10,
	SELL_ORDER_COL 		= 11,
	SELL_DEL_BTN_COL 	= 12,
	SELL_OCO_COL        = 13,
	SELL_OCODEL_COL     = 14,
	SELL_CONDITION_COL	= 15,
	SELL_FILL_COL		= 16
}ColName;
//---------------------------------------------------------------------------
/*class IOrderInfoListener
{
public:
	virtual Classes::TStringList* __fastcall GetSymbols( void ) = 0;
	virtual void __fastcall UpdateOrder( AnsiString Symbol, double Price, nsOrderMessageDefine::SideEnum side, int Qty, nsOrderMessageDefine::OrderTypeEnum Type ) = 0;
	virtual void __fastcall UpdateQty( nsOrderMessageDefine::SideEnum side, double Price, int Qty ) = 0;
	virtual void __fastcall UpdateStopOrderQty( nsOrderMessageDefine::SideEnum side, double Price, int Qty ) = 0;
};*/
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TOnStepChangeEvent)(System::TObject* Sender, int Step );
typedef void __fastcall (__closure *TOnNewOrderEvent)(System::TObject* Sender, nsOrderMessageDefine::SideEnum side, double Price, int Qty );
typedef void __fastcall (__closure *TOnNewOrdersEvent)(System::TObject* Sender, nsOrderMessageDefine::SideEnum side, TDoubleDynArray& PriceArray, int Qty );
typedef void __fastcall (__closure *TOnReduceQtyEvent)(System::TObject* Sender, nsOrderMessageDefine::SideEnum side, double Price, int Qty );
typedef void __fastcall (__closure *TOnDeleteAllEvent)(System::TObject* Sender, nsOrderMessageDefine::SideEnum side, const TDoubleDynArray& PriceArray, int& DelCount );
typedef void __fastcall (__closure *TOnNewMarketOrderEvent)(System::TObject* Sender, nsOrderMessageDefine::SideEnum side, int Qty );
typedef void __fastcall (__closure *TOnProfitAndAvgPxUpdateEvent)(System::TObject* Sender, double NetPositionProfit, double AvgPx );
typedef void __fastcall (__closure *TOnNetPositionUpdateEvent)(System::TObject* Sender, int NetPosition, double Profitint, int BuyOpenQty, int SellOpenQty );
typedef void __fastcall (__closure *TOnNewConditionOrderEvent)(System::TObject* Sender, nsOrderMessageDefine::SideEnum side, double Px, nsOrderMessageDefine::OrderTypeEnum Type, double OrderPrice );
typedef void __fastcall (__closure *TOnNewOCOFailEvent)(System::TObject* Sender, const AnsiString& ErrorMessage);
typedef void __fastcall (__closure *TOnNewOCOEvent)(System::TObject* Sender, nsOrderMessageDefine::SideEnum side, double Price);
typedef void __fastcall (__closure *TOnNewPingPongOrderEvent)(System::TObject* Sender, nsOrderMessageDefine::SideEnum side, double Price, int Qty, int Tick );
typedef void __fastcall (__closure *TOnDeleteStopOrderEvent)(System::TObject* Sender, nsOrderMessageDefine::SideEnum side, double StopPx );
typedef void __fastcall (__closure *TOnCenterEvent)(System::TObject* Sender );
typedef void __fastcall (__closure *TOnAskCancelCount)(System::TObject* Sender, int& Count );
typedef void __fastcall(__closure * TOnReplacePxEvent)(System::TObject * Sender, nsOrderMessageDefine::SideEnum side,double Price, double NewPrice);
typedef void __fastcall (__closure *TOnTick)(System::TObject* Sender, const AnsiString& time,  double MatchPx, int MatchQty );


typedef void __fastcall (__closure *TOnAutoStopEvent)(System::TObject* Sender, nsOrderMessageDefine::SideEnum side, double Price, int Qty );
//---------------------------------------------------------------------------
class PACKAGE TOrderBookList : 	public TCustomGrid, /*public IOrderInfoListener,*/
								public IMarketDataListener,
								public TPositionChangeListener,
                                public IOCOOrderStoreListener
{
private:
	double 			FBullPx;
	double 			FBearPx;
	double 			FFillPx;
	double 			FLastFillPx;
	double 			FMarkPx;
	double 			FMarkPx2;
	double          FDayH;
	double          FDayL;
	int				FFillQty;
	double 			FAvgFillPx;
	int             FAvgFillPxIndex;
	int             FMarkPxIndex;
	bool            FMarkPxAlarm;
	bool            FMarkPxHigher;
	int             FMarkPx2Index;
	bool            FMarkPx2Alarm;
	bool            FMarkPx2Higher;
	bool			FShowFillQty;
	bool            FOrderByOneClick;
    bool            FCancelByRightClick;
	TColor 			FBuyBKColor;
	TColor 			FBuyColor;
	TColor 			FSellBKColor;
	TColor 			FSellColor;
	TColor          FBuyDelBKColor;
	TColor          FSellDelBKColor;
	TColor 			FBidDepth5BKColor;
	TColor 			FBidDepth5Color;
	TColor 			FAskDepth5BKColor;
	TColor 			FAskDepth5Color;
	TColor 			FGridColor;
	TColor			FTickUpperColor;
	TColor			FTickLowerColor;
	TColor			FTickBKColor;
	TColor			FFillColor;
	TColor			FFillBKColor;
	TColor			FFixedRowColor;
	TColor			FFixedRowBKColor;
	TColor			FFixedRowBKEndColor;
	TColor			FFixedRowBidColor;
	TColor			FFixedRowBidEndColor;
	TColor			FFixedRowAskColor;
	TColor			FFixedRowAskEndColor;
	TColor			FBetterBuyColor;
	TColor			FBetterBuyBKColor;
	TColor			FBetterSellColor;
	TColor			FBetterSellBKColor;
	TColor			FBuyFillColor;
	TColor			FBuyFillBKColor;
	TColor			FSellFillColor;
	TColor			FSellFillBKColor;
	TColor 			FBuyConditionColColor;
	TColor 			FBuyConditionColBKColor;
	TColor 			FSellConditionColColor;
	TColor 			FSellConditionColBKColor;
	TColor          FBuyOCOColColor;
	TColor          FBuyOCOColBKColor;
	TColor          FSellOCOColColor;
	TColor          FSellOCOColBKColor;
	TColor          FBuyOCODelColColor;
	TColor          FBuyOCODelColBKColor;
	TColor          FSellOCODelColColor;
    TColor          FSellOCODelColBKColor;
	TColor          FDayHFrameColor;
	TColor          FDayLFrameColor;
	TColor          FAvgPxColor;
	UnicodeString	FExchange;
	UnicodeString 	FSymbol;
	UnicodeString   FChineseName;
	int				FDigit;
	TFont*			FFont;
	TTickList*		FTickList;
	TIntegerDynArray	FBuyDepth5Qty;
	TIntegerDynArray	FSellDepth5Qty;
	TIntegerDynArray	FUserBuyQty;
	TIntegerDynArray	FUserSellQty;
	TIntegerDynArray	FBuyFillQty;
	TIntegerDynArray	FSellFillQty;
	TIntegerDynArray	FBuyConditionQty;
	TIntegerDynArray	FSellConditionQty;
	TIntegerDynArray	FBuyOCOQty;
	TIntegerDynArray	FSellOCOQty;
	bool			FIsCompact;
	bool			FShowFilled;
	bool			FCenterFillPrice;
	bool            FFirstMatchPx;
	bool            FSync;
	bool            FInvertBS;
	bool            FUpdateEveryFill;
	int				FFillRowIndex;
	int				FBuyPxIndex[10];
	int				FSellPxIndex[10];
	double			FBuyPx[10];
	double			FSellPx[10];
	int				FBuyQty[10];
	int				FSellQty[10];
	int				FClickQty;
	int				FBestFitWidth;
	TGridCoord		FMouseCoord;
	TGridCoord		FMouseDownCoord;
	nsOrderMessageDefine::SideEnum				FCancelSide;
	Graphics::TBitmap* FPlusBmp;
	Graphics::TBitmap* FMinusBmp;
	Graphics::TBitmap* FPlusDarkBmp;
	Graphics::TBitmap* FPlusLightBmp;
	Graphics::TBitmap* FMinusDarkBmp;
	Graphics::TBitmap* FMinusLightBmp;
	Graphics::TBitmap* FAlarmBmp;
	Graphics::TBitmap* FAlarmedBmp;
	TCMarketDataStore* FStore;
	TOrderStore_OCO*   FOCOStore;
	UnicodeString FSubscribeExchange;
	UnicodeString FSubscribeSymbol;
	bool IsLoaded;
	Graphics::TBitmap* FBufferBmp;
	int FTotalBuyQty;
	int FTotalSellQty;
	int FTotalStopBuyQty;
	int FTotalStopSellQty;
	int FTotalOCOBuyQty;
	int FTotalOCOSellQty;
	bool FBetterEnable;
	int FBetterStep;
	bool FStepEnable;
	int FStep;
	int FStepCount;
	bool FCancelBeforeNew;
    int FNetPosition;
	double FBuyAvgFillPx;
	double FSellAvgFillPx;
	bool FShowDerivedPx;
	int FDerivedBuyIndex;
	int FDerivedSellIndex;
	int FDerivedBuyQty;
	int FDerivedSellQty;
	double FDerivedBuyPx;
	double FDerivedSellPx;
	int FBuySideFillQty;
	int FSellSideFillQty;
	DRAWTEXTPARAMS FDrawTextParam;
	bool FSettingMode;
	int FMarketTotalBuyQty;
	int FMarketTotalSellQty;
	bool FNeedCenterFill;
	bool FEnableHotTracks;
	bool FChangeFillColor;
	int  FClxAllDelayMS;
	TColor FHotTracksColor;
	TColor FHotTracksBKColor;
	///< Stop Order Setting
	bool FEnableCoditionlOrder;
	nsOrderMessageDefine::OrderTypeEnum FBuyStopOrderOrdType;
	nsOrderMessageDefine::OrderTypeEnum FSellStopOrderOrdType;
	int FBuyStopTick;
	int FSellStopTick;
	//< OCO Order Setting
	bool FShowOCO;
	bool FIsPairingOCO;
	bool FDeleteOCOByRightClick;
	TOCOPair* FCurrentPairOCO;
	std::set<TOCOPair*> FOCOPairs;
	///< Auto Stop-loss Take-Profit
	bool   FAutoStopLoss;
	bool   FAutoTakeProfit;
	bool   FStopLossUseTick;
	bool   FTakeProfitUseTick;
	int    FStopLossTick;
	int    FTakeProfitTick;
	double FStopLossPercent;
	double FTakeProfitPercent;
	///< Hot key
	bool FEnableHotKey;
	int  FMaxFillQty;
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
	TNotifyEvent      FOnUILock;
	TNotifyEvent      FOnFocus;
	TNotifyEvent      FOnHigherPxAlarm;
	TNotifyEvent      FOnLowerPxAlarm;
	TOnNewOrderEvent  FOnNewOrder;
	TOnNewOrdersEvent FOnNewOrders;
	TOnReduceQtyEvent FOnReduceQty;
	TOnReplacePxEvent FOnReplacePx;
	TOnDeleteAllEvent FOnDeleteAllOrders;
	TOnNewMarketOrderEvent FOnNewMarketOrder;
	TOnProfitAndAvgPxUpdateEvent FOnProfitAndAvgPxUpdate;
	TOnNetPositionUpdateEvent FOnNetPositionUpdate;
	TOnNewPingPongOrderEvent FOnNewPingPongOrder;
	TOnNewConditionOrderEvent FOnNewConditionOrder;
	TOnNewOCOFailEvent FOnNewOCOFail;
	TOnNewOCOEvent FOnNewOCO;

	TOnDeleteStopOrderEvent FOnDeleteStopOrder;
	TOnCenterEvent FOnCenterPx;
	TOnStepChangeEvent FOnStepChange;
	TOnAskCancelCount  FOnAskCancelCount;
	TOnTick            FOnTick;
	TOnAutoStopEvent   FOnAutoStopLoss;
	TOnAutoStopEvent   FOnAutoTakeProfit;

private:
	int				FMaxFPS;
	int             FUpdateMinMS;
	unsigned int    FLastUpdateFill;
	unsigned int    FLastUpdateDepth;
	bool            FDirtyFill;
	bool            FDirtyDepth;
	TTimer*         FTimer;
	void __fastcall OnUpdateTimer( System::TObject* Sender );
private:
	int FHH;
	int FMM;
	int FSS;
	void __fastcall UpdateTimer( const UFC::AnsiString& Time );
private:
	static UFC::List<TOrderBookList*> FSyncDepth;
private:
	static String HeaderString[];
	virtual void OnMarketDataUpdate( MarketDataMessage* Msg );///< Full Snapshot
	virtual void OnMarketDataUpdate( BasicData* Msg );
	virtual void OnMarketDataUpdate( MatchInfo* Msg );        ///< Filled
	virtual void OnMarketDataUpdate( TotalMatch* Msg );
	virtual void OnMarketDataUpdate( DayHighLowPrice* Msg );
	virtual void OnMarketDataUpdate( OpeningInfo* Msg );
	virtual void OnMarketDataUpdate( OrderBookData* Msg );    ///< Orderbook
	virtual void OnMarketDataUpdate( ClosingMarketData* Msg );
	virtual void OnMarketDataUpdate( UnderlyingIndexInfo* Msg );
	virtual void OnMarketDataUpdate( SumOfOrderInfo* Msg );
	virtual AnsiString GetEx( void );
	virtual AnsiString GetSymbol( void );
	virtual void OnOCOOrderUpdate(TOCOPair* Pair, OCOUpdateType Type);
private:
	void __fastcall InitString( void );
	void __fastcall SetCompact( bool IsCompact );
	void __fastcall SetDigit( int Digit );
	void __fastcall DrawGridLine( Graphics::TBitmap* Bmp, const Types::TRect &ARect );
	void __fastcall DrawHeader( int Col, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall DrawSumRow( int Col, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall DrawTick( int Row, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall DrawButtonCol( int Col, int Row, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall DrawBuyOrderCol( int Row, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall DrawSellOrderCol( int Row, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall DrawDelCol( int ACol, int ARow, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall DrawBuyFillCol( int Row, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall DrawSellFillCol( int Row, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall DrawGradientBar( TCanvas* canvas, TRect& dRect,TColor UpCol, TColor DownCol );
	TColor  __fastcall DarkerColor( TColor OrigCol );
	TColor  __fastcall LighterColor( TColor OrigCol );
	void __fastcall SetBullPrice( double Price );
	void __fastcall SetBearPrice( double Price );
	void __fastcall SetFillPrice( double Price );
	void __fastcall SetAvgFillPx( double AvgPx );
	void __fastcall SetFont( TFont* Font );
	void __fastcall SetBuyBKColor( TColor Color );
	void __fastcall SetSellBKColor( TColor Color );
	void __fastcall SetBuyDelBKColor( TColor Color );
    void __fastcall SetSellDelBKColor( TColor Color );
	void __fastcall SetFillColor( TColor Color );
	void __fastcall SetFillBKColor( TColor Color );
	void __fastcall SetFixedRowColor( TColor Color );
	void __fastcall SetBidDepth5Color( TColor Color );
	void __fastcall SetBidDepth5BKColor( TColor Color );
	void __fastcall SetAskDepth5Color( TColor Color );
	void __fastcall SetAskDepth5BKColor( TColor Color );
	void __fastcall SetGridColor( TColor Color );
	void __fastcall SetUpdateEveryFill( bool UpdateEveryFill );
	void __fastcall DrawBuyMKCol( int Row, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall DrawSellMKCol( int Row, const Types::TRect &ARect, TGridDrawState AState );
	int __fastcall GetRowIndex( double Price, int* PxIndex = NULL );
	double __fastcall GetPxFromIndex( int Index );
	void __fastcall CalSize( void );
    void __fastcall SetSettingModeCol( void );
	void __fastcall SetCompactCol( int ColWidth );
	void __fastcall SetFillCol( int FillWidth );
	void __fastcall SetConditionCol( int OrderWidth );
	void __fastcall SetSmartOrderCol( int OrderWidth );
	void __fastcall CalFilledColSize( TCanvas* canvas );
	void __fastcall DrawQty( Graphics::TBitmap* Bmp, Types::TRect *ARect, TTextFormats Align, int Qty, TColor TextColor, int DerivedQty = 0 );
	void __fastcall DrawOCOQty( Graphics::TBitmap* Bmp,	Types::TRect* ARect, int ACol, int ARow, TColor TextColor);
	void __fastcall MouseOverCell( const TGridCoord& Coord );
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall KeyDown(System::Word &Key, Classes::TShiftState Shift);
	void __fastcall MouseLeave( System::TObject* Sender );
	void __fastcall MouseEnter(  System::TObject* Sender );
	void __fastcall SetPlusBmp( Graphics::TBitmap* Bitmap );
	void __fastcall SetAlarmBmp( Graphics::TBitmap* Bitmap );
	void __fastcall SetAlarmedBmp( Graphics::TBitmap* Bitmap );
	void __fastcall SetMinusBmp( Graphics::TBitmap* Bitmap );
	void __fastcall UpdateDepth5( OrderBookData* Msg );
	void __fastcall SetCenterFillPrice( bool CenterFill );
	void __fastcall SetFixedRowBKColor( TColor Color );
	void __fastcall SetFixedRowBKEndColor( TColor Color );
	void __fastcall SetFixedRowBidColor( TColor Color );
	void __fastcall SetFixedRowBidEndColor( TColor Color );
	void __fastcall SetFixedRowAskColor( TColor Color );
	void __fastcall SetFixedRowAskEndColor( TColor Color );
	int __fastcall TriggerCancelAllBuyOrders( void );
	int __fastcall TriggerCancelAllSellOrders( void );
	void __fastcall TriggerNewOrder( void );
	void __fastcall OnKeyBuyDown( void );
	void __fastcall OnKeySellDown( void );
	void __fastcall SetShowFilled( bool Show );
	bool __fastcall UpdateDepth( OrderBookData* Msg );
	void __fastcall SetSettingMode( bool Mode );
	void __fastcall WndProc( TMessage &Msg );
	void __fastcall EnableHotkey( bool Enable );
	void __fastcall SetFPS( int FPS );
	///< Ping Pong functions
	void __fastcall EnablePingPong( bool Enable );
	void __fastcall SetBuyPingPongTick( int Tick );
	void __fastcall SetSellPingPongTick( int Tick );
	void __fastcall SetBuyPingPongPercent( double Percent );
	void __fastcall SetSellPingPongPercent( double Percent );
	double __fastcall GetPingPongPrice( double Px, int OrigTick, nsOrderMessageDefine::SideEnum OrderSide );

	///< Stop-loss Take-Profit functions
	void __fastcall SetStopLossTick( int Tick );
	void __fastcall SetTakeProfitTick( int Tick );
	void __fastcall SetStopLossPercent( double Percent );
	void __fastcall SetTakeProfitPercent( double Percent );
	double __fastcall GetStopLossPrice( double Px, int OrigTick, nsOrderMessageDefine::SideEnum OrderSide );
	double __fastcall GetTakeProfitPrice( double Px, int OrigTick, nsOrderMessageDefine::SideEnum OrderSide );
	bool __fastcall CheckStopLoss( int PxTick );
	bool __fastcall CheckTakeProfit( int PxTick );
	///< Condition Order
	void __fastcall EnableConditionOrder( bool Enable );
	void __fastcall EnableOCO( bool Enable );
	void __fastcall EnableSmartOrder( bool OCO );
	void __fastcall DrawConditionCol( int ACol, int ARow, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall DrawOCOCol( int ACol, int ARow, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall DrawOCODelCol( int ACol, int ARow, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall ConditionOrderColMouseDown( Classes::TShiftState Shift, int X, int Y );
	void __fastcall OCOColLeftMouseDown( Classes::TShiftState Shift, int X, int Y );
	void __fastcall OCOColRightMouseDown( Classes::TShiftState Shift, int X, int Y );
	void __fastcall OCODelColLeftMouseDown( Classes::TShiftState Shift, int X, int Y );
    void __fastcall SetOCOQtyArrayByPair(TOCOPair* pair, bool isAdd);
	void __fastcall SetOCOQtyArray(nsOrderMessageDefine::SideEnum side, int index, int Qty, bool isAdd );
	void __fastcall MarkPrice( int Tick, bool IsMark1 );
	void __fastcall SetBuyStopTick( int Tick );
	void __fastcall SetSellStopTick( int Tick );
	void __fastcall SetBuyStopOrderType( nsOrderMessageDefine::OrderTypeEnum OrderType );
	void __fastcall SetSellStopOrderType( nsOrderMessageDefine::OrderTypeEnum OrderType );
	void __fastcall DeleteAllStopOrder( nsOrderMessageDefine::SideEnum Side );
	void __fastcall PlaceStepNewOrder( nsOrderMessageDefine::SideEnum Side, int BasePxIndex, int Qty );
	void __fastcall SetSync( bool IsSync );
	void __fastcall ChangeBetterValue( int NewBetter );
	void __fastcall InvalidateCellRect( int COL, int ROW );
	void __fastcall InvalidateColumn( int COL, int TopROW, int DownROW );
	void __fastcall InvalidateBoxRect( int LeftCOL, int TopROW, int RightCOL, int DownROW );
	void __fastcall SetBitmapAlpha( Graphics::TBitmap* RGBABmp, int Alpha  );
	void __fastcall CreateCursor( int HotspotX, int HotspotY, int CancelQty, bool IsBuy );
	int __fastcall GetBidQty( int depth );
	int __fastcall GetAskQty( int depth );
	double __fastcall GetBidPrice( int depth );
	double __fastcall GetAskPrice( int depth );
	void __fastcall SubscribeOCOStore(void);
	void __fastcall UnsubscribeOCOStore(void);
protected:
	void _fastcall DrawCell(int ACol, int ARow, const Types::TRect &ARect, TGridDrawState AState );
	virtual void __fastcall Loaded(void);
	void __fastcall MyMouseWheelDown( System::TObject* Sender, Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled );
	void __fastcall MyMouseWheelUp( System::TObject* Sender, Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled );
	virtual void OnFill(int Position, double BuyArvPx, double SellArvPx, int BuyQty, int SellQty,
						int BuyOpenInterestQty, int SellOpenInterestQty,
						DynamicArray<void*>& PosStatisticsArray, int NetPosition, double FloatingProfit );
	virtual void OnProfit(double AveragePrice, double ProfitAmount);
	virtual void OnFill(nsOrderMessageDefine::SideEnum Side, double Px, int Qty, int TickCount);  //For Ping Pong
public:
	__fastcall TOrderBookList(TComponent* Owner);
	void __fastcall InitialGrid( double BullPrice, double BearPrice, double FillPrice, int Digit );
	virtual __fastcall ~TOrderBookList();
	void __fastcall CenterPx( double Px, bool NeedFocus );
	void __fastcall CenterPx( bool NeedFocus );
	void __fastcall TopBullPx( void );
	void __fastcall BottomBearPx( void );
	void __fastcall CenterFillPx( bool force = false );
	void __fastcall SetTestString( UnicodeString NewString );
	void __fastcall ScrollUp( int Tick );
	void __fastcall ScrollDown( int Tick );
	void __fastcall Subscribe( TCMarketDataStore* Store = NULL );
	void __fastcall SetOCOStore( TOrderStore_OCO* Store = NULL );
	void __fastcall SetBetterEnable( bool Enable );
	void __fastcall Unsubscribe( void );
	void __fastcall OpenStopOrderSettingForm( void );
	void __fastcall DeleteBuyStopOrders( void );
	void __fastcall DeleteSellStopOrders( void );
	void __fastcall DeleteAllStopOrders( void );
	void __fastcall DeleteAllOrders( bool Sync );
	void __fastcall DeleteBuyOrders( bool Sync );
	void __fastcall DeleteSellOrders( bool Sync );
	void __fastcall Clear( bool IncludeDepth = true );
	double __fastcall GetTickPrice( int Step );
	bool __fastcall UpdateFill( void );
	void __fastcall ForceUpdateFill( void );
	double __fastcall GetBetterPrice( nsOrderMessageDefine::SideEnum side, TBetterPxType PxType, int BetterTick );
	double __fastcall GetBetterPrice( double Px, int BetterTick );
	void __fastcall CancelAll( TCancelSide Side = csBoth );
	bool __fastcall SetSymbol( const String& Ex, const String& Sym );
	void __fastcall GetPrices( bool IsUp, TDoubleDynArray& PriceArray, int Times, int Step, int Offset = 0 );
	double __fastcall GetStopExecutePrice( nsOrderMessageDefine::SideEnum side, int PxIndex );
	double __fastcall GetStopExecutePrice( nsOrderMessageDefine::SideEnum side, double Px );
	bool __fastcall HasStopOrder( void );
	bool __fastcall HasWorkingOrder( void );
	double __fastcall GetBullPrice( int BetterSellTick );
	void PriceAlarm( int MarkPxIndex, bool& MarkPxAlarm, bool MarkPxHigher, double MarkPx );
public:
	static int    __fastcall SyncCount( void );
	static TForm* __fastcall GetSyncForm( int i );
	static void   __fastcall GetSyncFormInfo(  int i, String& FormText, bool& Invert );
public:
	virtual Classes::TStringList* __fastcall GetSymbols( void );
	virtual void __fastcall UpdateOrder( AnsiString Symbol, double Price, nsOrderMessageDefine::SideEnum side, int Qty, nsOrderMessageDefine::OrderTypeEnum Type );
	virtual void __fastcall UpdateQty( nsOrderMessageDefine::SideEnum side, double Price, int Qty );
	virtual void __fastcall UpdateStopOrderQty( nsOrderMessageDefine::SideEnum side, double Price, int Qty );
__published:
	__property UnicodeString Symbol = { read = FSymbol, write = FSymbol };
	__property UnicodeString Exchange = { read = FExchange, write = FExchange };
	__property UnicodeString ChineseName = { read = FChineseName };
	__property bool IsCompact = { read = FIsCompact, write = SetCompact };
	__property bool CenterFillPrice = { read = FCenterFillPrice, write = SetCenterFillPrice, default = false };
	__property double BullPrice = { read = FBullPx, write = SetBullPrice };
	__property double BearPrice = { read = FBearPx, write = SetBearPrice };
	__property double FillPrice = { read = FFillPx, write = SetFillPrice };
	__property int Digit = { read = FDigit, write = SetDigit, default = 0 };
	__property int ClickQty = { read = FClickQty, write = FClickQty };
	__property int BestFitWidth = { read = FBestFitWidth, write = FBestFitWidth };
	__property bool ShowFillQty = { read = FShowFillQty, write = FShowFillQty };
	__property bool OrderByOneClick = { read = FOrderByOneClick, write = FOrderByOneClick };
	__property bool CancelByRightClick = { read = FCancelByRightClick, write = FCancelByRightClick };
	__property bool ShowDerivedPx = { read = FShowDerivedPx, write = FShowDerivedPx };
	__property bool EnableHotTracks = { read = FEnableHotTracks, write = FEnableHotTracks };
	__property int BidQty[ int depth ]  = { read = GetBidQty };
	__property int AskQty[ int depth ]  = { read = GetAskQty };
	__property double BidPrice[ int depth ]  = { read = GetBidPrice };
	__property double AskPrice[ int depth ]  = { read = GetAskPrice };
	__property int WorkingBuyQty = { read = FTotalBuyQty };
	__property int WorkingSellQty = { read = FTotalSellQty };
	///< Stop loss, Take Profit
	__property bool AutoStopLoss = { read = FAutoStopLoss, write = FAutoStopLoss };
	__property bool AutoTakeProfit = { read = FAutoTakeProfit, write = FAutoTakeProfit };
	__property bool StopLossUseTick = { read = FStopLossUseTick, write = FStopLossUseTick };
	__property bool TakeProfitUseTick = { read = FTakeProfitUseTick, write = FTakeProfitUseTick };
	__property int  StopLossTick = { read = FStopLossTick, write = SetStopLossTick };
	__property int  TakeProfitTick = { read = FTakeProfitTick, write = SetTakeProfitTick };
	__property double StopLossPercent = { read = FStopLossPercent, write = SetStopLossPercent };
	__property double TakeProfitPercent = { read = FTakeProfitPercent, write = SetTakeProfitPercent };
	///< Color
	__property TColor BuyBKColor = { read = FBuyBKColor, write = SetBuyBKColor };
	__property TColor BuyColor  = { read = FBuyColor, write = FBuyColor };
	__property TColor BuyDelBKColor = { read = FBuyDelBKColor, write = SetBuyDelBKColor };
	__property TColor SellBKColor = { read = FSellBKColor, write = SetSellBKColor };
	__property TColor SellColor = { read = FSellColor, write = FSellColor };
	__property TColor SellDelBKColor = { read = FSellDelBKColor, write = SetSellDelBKColor };
	__property TColor TickUpperColor = { read = FTickUpperColor, write = FTickUpperColor };
	__property TColor TickLowerColor = { read = FTickLowerColor, write = FTickLowerColor };
	__property TColor TickBKColor = { read = FTickBKColor, write = FTickBKColor };
	__property TColor FillColor = { read = FFillColor, write = SetFillColor };
	__property TColor FillBKColor = { read = FFillBKColor, write = SetFillBKColor };
	__property TColor BidDepth5BKColor = { read = FBidDepth5BKColor, write = SetBidDepth5BKColor };
	__property TColor BidDepth5Color = { read = FBidDepth5Color, write = SetBidDepth5Color };
	__property TColor AskDepth5BKColor = { read = FAskDepth5BKColor, write = SetAskDepth5BKColor, default = clMenu };
	__property TColor AskDepth5Color = { read = FAskDepth5Color, write = SetAskDepth5Color, default = clRed };
	__property TColor GridColor = { read = FGridColor, write = SetGridColor };
	__property TColor FixedRowColor = { read = FFixedRowColor, write = SetFixedRowColor };
	__property TColor FixedRowBKColor = { read = FFixedRowBKColor, write = SetFixedRowBKColor };
	__property TColor FixedRowBKEndColor = { read = FFixedRowBKEndColor, write = SetFixedRowBKEndColor };

	__property TColor FixedRowBidColor = { read = FFixedRowBidColor, write = SetFixedRowBidColor };
	__property TColor FixedRowBidEndColor = { read = FFixedRowBidEndColor, write = SetFixedRowBidEndColor };
	__property TColor FixedRowAskColor = { read = FFixedRowAskColor, write = SetFixedRowAskColor };
	__property TColor FixedRowAskEndColor = { read = FFixedRowAskEndColor, write = SetFixedRowAskEndColor };

	__property TColor BetterBuyColor = { read = FBetterBuyColor, write = FBetterBuyColor };
	__property TColor BetterBuyBKColor = { read = FBetterBuyBKColor, write = FBetterBuyBKColor };
	__property TColor BetterSellColor = {read = FBetterSellColor, write = FBetterSellColor };
	__property TColor BetterSellBKColor = {read = FBetterSellBKColor, write = FBetterSellBKColor };
	__property TColor BuyConditionColColor = { read = FBuyConditionColColor, write = FBuyConditionColColor };
	__property TColor BuyConditionColBKColor = { read = FBuyConditionColBKColor, write = FBuyConditionColBKColor };
	__property TColor SellConditionColColor = { read = FSellConditionColColor, write = FSellConditionColColor };
	__property TColor SellConditionColBKColor = { read = FSellConditionColBKColor, write = FSellConditionColBKColor };
	__property TColor HotTracksColor = { read = FHotTracksColor, write = FHotTracksColor };
	__property TColor HotTracksBKColor = { read = FHotTracksBKColor, write = FHotTracksBKColor };
	__property TColor BuyFillColor = { read = FBuyFillColor, write = FBuyFillColor };
	__property TColor SellFillColor = { read = FSellFillColor, write = FSellFillColor };
	__property TColor BuyFillBKColor = { read = FBuyFillBKColor, write = FBuyFillBKColor };
	__property TColor SellFillBKColor = { read = FSellFillBKColor, write = FSellFillBKColor };
	__property TColor BuyOCOColColor = { read = FBuyOCOColColor, write = FBuyOCOColColor };
	__property TColor BuyOCOColBKColor = { read = FBuyOCOColBKColor, write = FBuyOCOColBKColor };
	__property TColor SellOCOColColor = { read = FSellOCOColColor, write = FSellOCOColColor };
	__property TColor SellOCOColBKColor = { read = FSellOCOColBKColor, write = FSellOCOColBKColor };
	__property TColor BuyOCODelColColor = { read = FBuyOCODelColColor, write = FBuyOCODelColColor };
	__property TColor BuyOCODelColBKColor = { read = FBuyOCODelColBKColor, write = FBuyOCODelColBKColor };
	__property TColor SellOCODelColColor = { read = FSellOCODelColColor, write = FSellOCODelColColor };
	__property TColor SellOCODelColBKColor = { read = FSellOCODelColBKColor, write = FSellOCODelColBKColor };

	__property TColor DayHColor  = {read = FDayHFrameColor, write = FDayHFrameColor };
	__property TColor DayLColor  = {read = FDayLFrameColor, write = FDayLFrameColor };
	__property TColor AvgPxColor = {read = FAvgPxColor, write = FAvgPxColor };

	__property TFont* Font = { read = FFont, write = SetFont };
	__property Graphics::TBitmap *AlarmBmp = { read = FAlarmBmp, write = SetAlarmBmp };
	__property Graphics::TBitmap *AlarmedBmp = { read = FAlarmedBmp, write = SetAlarmedBmp };
	__property Graphics::TBitmap *PlusBmp = { read = FPlusBmp, write = SetPlusBmp };
	__property Graphics::TBitmap *MinusBmp = { read = FMinusBmp, write = SetMinusBmp };
	__property TCMarketDataStore* Store = { read = FStore, write = Subscribe };
	__property TOrderStore_OCO*   OCOStore = { read = FOCOStore, write = SetOCOStore };
	__property Align;
	__property OnMouseDown;
	__property OnKeyDown;
	__property Color;
	__property DrawingStyle;
	__property OnDragDrop;
	__property OnDragOver;
    __property Visible;
	__property int CancelAllDelay = { read = FClxAllDelayMS, write = FClxAllDelayMS };
	__property int BetterStep = { read = FBetterStep, write = FBetterStep };
	__property int Step = { read = FStep, write = FStep };
	__property int StepCount = { read = FStepCount, write = FStepCount };
	__property bool SetEnable = { read = FStepEnable, write = FStepEnable };
	__property bool CancelBeforeNew = { read = FCancelBeforeNew, write = FCancelBeforeNew };
	__property bool ShowFilled = { read = FShowFilled, write = SetShowFilled };
	__property bool UpdateEveryFill = { read = FUpdateEveryFill, write = SetUpdateEveryFill };
	__property bool SettingMode = { read = FSettingMode, write = SetSettingMode };
	__property bool ChangeFillColor = { read = FChangeFillColor, write = FChangeFillColor };
	__property bool Sync = { read = FSync, write = SetSync };
	__property bool InvertBuySell = { read = FInvertBS, write = FInvertBS };
	__property int MaxFPS = { read = FMaxFPS, write = SetFPS };
	///< Stop Order setting
	__property bool ConditionOrder = { read = FEnableCoditionlOrder, write = EnableConditionOrder };
	__property bool ShowOCO = { read = FShowOCO, write = EnableOCO };
	__property int  BuyStopTick = { read = FBuyStopTick, write = SetBuyStopTick };
	__property int  SellStopTick = { read = FSellStopTick, write = SetSellStopTick };
	__property nsOrderMessageDefine::OrderTypeEnum BuyStopOrderType = { read = FBuyStopOrderOrdType, write = SetBuyStopOrderType };
	__property nsOrderMessageDefine::OrderTypeEnum SellStopOrderType = { read = FSellStopOrderOrdType, write = SetSellStopOrderType };
	///< Hot Key
	__property bool Hotkey = { read = FEnableHotKey, write = EnableHotkey };
	__property bool BetterEnable = { read = FBetterEnable, write = SetBetterEnable };
	__property System::Word KeyBuy           = { read = FKeyBuy, write = FKeyBuy };
	__property System::Word KeySell          = { read = FKeySell, write = FKeySell };
	__property System::Word KeyBuyMarket     = { read = FKeyBuyMarket, write = FKeyBuyMarket };
	__property System::Word KeySellMarket    = { read = FKeySellMarket, write = FKeySellMarket };
	__property System::Word KeyCancelAll     = { read = FKeyCancelAll, write = FKeyCancelAll };
	__property System::Word KeyCancelBuy     = { read = FKeyCancelBuy, write = FKeyCancelBuy };
	__property System::Word KeyCancelSell    = { read = FKeyCancelSell, write = FKeyCancelSell };
	__property System::Word KeyBetterIncrese = { read = FKeyBetterIncrese, write = FKeyBetterIncrese };
	__property System::Word KeyBetterDecrese = { read = FKeyBetterDecrese, write = FKeyBetterDecrese };
	__property System::Word KeyCenterFillPx  = { read = FKeyCenterFillPx, write = FKeyCenterFillPx };
	__property System::Word KeyNewOrder      = { read = FKeyNewOrder, write = FKeyNewOrder };
	__property Controls::TMouseButton MouseButtonCenterFillPx = { read = FMouseButtonCenterFillPx, write = FMouseButtonCenterFillPx };
	///< Event
	__property TNotifyEvent     OnPxAlarm = { read = FOnHigherPxAlarm, write = FOnHigherPxAlarm };
	__property TNotifyEvent     OnLowerPxAlarm = { read = FOnLowerPxAlarm, write = FOnLowerPxAlarm };
	__property TNotifyEvent     OnFocus = { read = FOnFocus, write = FOnFocus };
	__property TNotifyEvent     OnUILock = { read = FOnUILock, write = FOnUILock };
	__property TOnNewOrderEvent OnNewOrder = { read = FOnNewOrder, write = FOnNewOrder };
	__property TOnNewOrdersEvent OnNewOrders = { read = FOnNewOrders, write = FOnNewOrders };
	__property TOnReduceQtyEvent OnReduceQty = { read = FOnReduceQty, write = FOnReduceQty };
	__property TOnReplacePxEvent OnReplacePx = { read = FOnReplacePx, write = FOnReplacePx };
	__property TOnDeleteAllEvent OnDeleteAllOrders = { read = FOnDeleteAllOrders, write = FOnDeleteAllOrders };
	__property TOnNewMarketOrderEvent OnNewMarketOrder = { read = FOnNewMarketOrder, write = FOnNewMarketOrder };
	__property TOnProfitAndAvgPxUpdateEvent OnProfitAndAvgPxUpdate = { read = FOnProfitAndAvgPxUpdate, write = FOnProfitAndAvgPxUpdate };
	__property TOnNetPositionUpdateEvent OnNetPositionUpdate = { read = FOnNetPositionUpdate, write = FOnNetPositionUpdate };
	__property TOnNewPingPongOrderEvent OnNewPingPongOrder = { read = FOnNewPingPongOrder, write = FOnNewPingPongOrder };
	__property TOnNewConditionOrderEvent OnNewConditionOrder = { read = FOnNewConditionOrder, write = FOnNewConditionOrder };
	__property TOnNewOCOFailEvent OnNewOCOFail = { read = FOnNewOCOFail, write = FOnNewOCOFail };
	__property TOnNewOCOEvent OnNewOCO = { read = FOnNewOCO, write = FOnNewOCO };
	__property TOnDeleteStopOrderEvent OnDeleteStopOrder = { read = FOnDeleteStopOrder, write = FOnDeleteStopOrder };
	__property TOnCenterEvent OnCenterPx = { read = FOnCenterPx, write = FOnCenterPx };
	__property TOnStepChangeEvent OnStepChange = { read = FOnStepChange, write = FOnStepChange };
	__property TOnAskCancelCount  OnAskCancelCount = { read = FOnAskCancelCount, write = FOnAskCancelCount };

	__property TOnAutoStopEvent  OnAutoStopLoss = { read = FOnAutoStopLoss, write = FOnAutoStopLoss };
	__property TOnAutoStopEvent  OnAutoTakeProfit = { read = FOnAutoTakeProfit, write = FOnAutoTakeProfit };

	__property double AvgFillPx = { read = FAvgFillPx, write = SetAvgFillPx };
	__property double LastPx = { read = FLastFillPx };
	__property TOnTick  OnTick = { read = FOnTick, write = FOnTick };
};
//---------------------------------------------------------------------------
#endif
