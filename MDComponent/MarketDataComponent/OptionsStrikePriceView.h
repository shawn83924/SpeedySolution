//---------------------------------------------------------------------------
#ifndef OptionsStrikePriceViewH
#define OptionsStrikePriceViewH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <Grids.hpp>
#include "OrderMessageDefine.h"
#include "MarketDataStore.h"
#include "OptionsGreeks.h"
//---------------------------------------------------------------------------
const int STRIKE_PX_LINE_LEN = 8;
//---------------------------------------------------------------------------
typedef enum optStkColName
{
	BUY_PX		   = 0,  //<OK
	SELL_PX		   = 1,  //<OK
	TRADE_PX	   = 2,  //<OK
	DIFF_PX		   = 3,  //<OK
	TRADE_QTY	   = 4,  //<OK
	TOTAL_QTY	   = 5,  //<OK*
	HIGH_PX        = 6,  //<OK
	LOW_PX 	       = 7,  //<OK
	SETTLEMENT_PX  = 8,  //<OK

	OPEN_INTEREST  = 9,  //<OK*
	DELTA          = 10, //<OK
	GAMMA          = 11, //<OK
	THETA          = 12, //<OK
	VEGA           = 13, //<OK
	RHO            = 14, //<OK
	STRIKE_PRICE   = 15, //<OK

	DIFF_OI        = 16,
	BUY_QTY        = 17, //<OK
	SELL_QTY       = 18, //<OK
	IMP_VAR        = 19, //<OK*
	THEORY_PX      = 20  //<OK

} OptStkColName;
//---------------------------------------------------------------------------
typedef enum optbkColName
{
	obNone           =  0,
	obTOTAL_QTY	     =  5,
	obOPEN_INTEREST  =  9,
	obIMP_VAR        = 19

} TOptBKColName;
//---------------------------------------------------------------------------
class TOptRecord
{
private:
	String      FExchange;
	String      FSymbol;
	int         FBullPx;
	int         FBearPx;
	int         FRefInt;
	double      FBidPx;
	double      FAskPx;
	int         FBidQty;
	int         FAskQty;
	double      FTradePx;
	double      FRefPx;
	int         FTradeQty;
	int         FTotalQty;
	double      FHighPx;
	double      FLowPx;
	CallPutCode FCallPut;
	int         FDigit;
	double      FStrikePx;
	double      FSettlementPx;
	String      FYearMonth;
	int         FOpenInterest;
	int         FOpenInterestDiff;
	bool        FIsUnderlying;
	int  		FValueWidth;
	bool        FNeedGreeks;   ///< 批次處理旗標: 此筆行情變動後需在 timer 重算希臘值
private:
	String      FProdID;
	int         FMaxHDays;
	double      FExpiryDays;
	double      FHistoryVol;
	TGreeks     FGreeks;
private: ///< for underlying
	String             FDisplayName;
	Graphics::TBitmap* FDspBuf;
	UFC::PDoubleList   FStrikePrices;
	double             FStrikePxRate;
	int                FRowIndex;         ///< Row index in the grid.
	int                FStrikePxAt;       ///< Strike px index in FStrikePrices.
	int                FStrikePxIndex;    ///< Strike px index in grid.
	int                FPrevStrikePxIndex;///< Prev Strike px index in grid.
private:
	int  UnderlyingTradePxAt( double Px );
	int  FindNearistStrikePx( int Last, double Px, double StrikePxStep );
	void StrikePxPosition( double Pxdif, double StrikePxStep );
	int  CalExpiryDays( const String& sLastTradeDate );
public:
	TOptRecord*     Next;
	TOptRecord*     Underlying;
public:
	void AddStrikePx( double Spx ) { FStrikePrices.Add( Spx ); }
	void SetRowIndex( int Index  ) { FRowIndex = Index; }
	int  GetRowIndex( void  )      { return FRowIndex; }
	void SetNeedGreeks( bool b )   { FNeedGreeks = b; }
	bool NeedGreeks( void  )       { return FNeedGreeks; }
public:
	TOptRecord( BasicInformation* Info, bool Underlying = false );
	~TOptRecord( void );
	void UpdateGreeks( double TimeToClose );
	///< Get Functions
	Graphics::TBitmap* GetBuffer( void )      { return FDspBuf; }
	const String&      GetExchange( void )    { return FExchange; }
	const String&      GetSymbol( void )      { return FSymbol; }
	const String&      GetYearMonth( void )   { return FYearMonth; }
	const String&      GetDisplayName( void ) { return FDisplayName; }
	const TGreeks&     GetGreeks( void )      { return FGreeks; }
	double      GetBidPx( void )           { return FBidPx; }
	double      GetAskPx( void )           { return FAskPx; }
	int         GetBidQty( void )          { return FBidQty; }
	int         GetAskQty( void )          { return FAskQty; }
	double      GetTradePx( void )         { return FTradePx; }
	double      GetRefPx( void )           { return FRefPx; }
	int         GetTradeQty( void )        { return FTradeQty; }
	int         GetTotalQty( void )        { return FTotalQty; }
	double      GetHighPx( void )          { return FHighPx; }
	double      GetLowPx( void )           { return FLowPx; }
	CallPutCode GetCallPut( void )         { return FCallPut; }
	int         GetOpenInterest( void )    { return FOpenInterest; }
	int         GetOpenInterestDiff( void ){ return FOpenInterestDiff; }
	int         GetDigit( void )           { return FDigit; }
	double 		GetStrikePx( void )        { return FStrikePx; }
	double 		GetSettlementPx( void )    { return FSettlementPx; }
	int 		GetUnderlyingStrikePxIndex( void )     { return FStrikePxIndex; }
	int 		GetPrevUnderlyingStrikePxIndex( void ) { return FPrevStrikePxIndex; }
	double      GetStrikePxPosRate( void )             { return FStrikePxRate; }
	///< Set functions
	void SetYearMonth( const String& YM ){ FYearMonth = YM; }
	void SetBidPx( double Px )  { FBidPx = Px; }
	void SetAskPx( double Px )  { FAskPx = Px; }
	void SetBidQty( int Qty )   { FBidQty = Qty; }
	void SetAskQty( int Qty )   { FAskQty = Qty; }
	void SetTradeQty( int Qty ) { FTradeQty = Qty; }
	void SetTotalQty( int Qty ) { FTotalQty = Qty; }
	void SetHighPx( double Px ) { FHighPx = Px; }
	void SetLowPx( double Px )  { FLowPx = Px; }
	void SetTradePx( double Px );
	void SetOpenInterest( int Qty );
	bool IsUpeerLimit( double Px );
	bool IsLowerLimit( double Px );
	bool IsFlat( double Px );
	bool IsClimbed( double Px );
	bool IsFell( double Px );
	bool IsUnderlyingStrikePx( int RowIndex );
	bool IsUnderlying( void ) { return FIsUnderlying; }
public:
	void    UpdateValueWidth( TOptBKColName Field, double MaxVal, int TotalWidth );
	double  GetBKBarValue( TOptBKColName Field );
	int     GetValueWidth( void ) { return FValueWidth; }
};
//---------------------------------------------------------------------------
int __fastcall OptSymbolCompare(void *Item1, void *Item2);
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TOnTBarMouseDownEvent)(System::TObject* Sender, UnicodeString Symbol, UnicodeString MaturityDate, nsOrderMessageDefine::SideEnum side, double Price, CallPutCode CallPut, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y );
typedef void __fastcall (__closure *TOnTBarMouseClickEvent)(System::TObject* Sender, UnicodeString Symbol, CallPutCode CallPut, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y );
typedef void __fastcall (__closure *TOnTBarStrikePxMouseDownEvent)(System::TObject* Sender, UnicodeString Symbol, CallPutCode CallPut);
//---------------------------------------------------------------------------
class PACKAGE TOptionsStrikePriceView : public TCustomGrid, public IMarketDataListener
{
private:
	String FSymbol;
	String FExchange;
	String FYearMonth;
	String FDisplayName;
	bool   FIsLoaded;
	bool   FIsReverse;
	bool   FShowBKBar;
	bool   FLocked;        ///< true 時忽略使用者輸入(滑鼠/鍵盤)，但不變灰、不重繪
	TColor FHeaderCallColor;
	TColor FHeaderCallBKColor;
	TColor FHeaderPutColor;
	TColor FHeaderPutBKColor;
	TColor FHeaderCommodColor;
	TColor FHeaderCommodBKColor;
	TColor FFieldNameCallColor;
	TColor FFieldNameCallBKColor;
	TColor FFieldNamePutColor;
	TColor FFieldNamePutBKColor;
	TColor FFieldNameCommodColor;
	TColor FFieldNameCommodBKColor;
	TColor FCallBKColor;
	TColor FPutBKColor;
	TColor FStkPxColor;
	TColor FStkPxBKColor;
	TColor FRisePxColor;
	TColor FFallPxColor;
	TColor FEqualPxColor;
	TColor FHGridColor;
	TColor FVGridColor;
	TColor FBKBarColor;
	TCMarketDataStore* FStore;
	TOnTBarMouseDownEvent         FOnTBarMouseDown;
	TOnTBarMouseClickEvent        FOnTBarMouseClick;
	TOnTBarStrikePxMouseDownEvent FOnTBarStrikePxMouseDown;
	TNotifyEvent                  FOnTBarStrikePxClick;
	TNotifyEvent                  FOnTBarFieldsChanged;
	TGridCoord		FMouseCoord;
	TOptBKColName   FBKCol;
private:
	Graphics::TBitmap* FBufferBmp;
	Graphics::TBitmap* FAlphaBmp;
	UFC::PHashMap<UFC::AnsiString, OptStkColName>  FColNameColTypeMap;
	UFC::PHashMap<OptStkColName, int >             FCallFieldTypeMap;
	UFC::PHashMap<OptStkColName, int >              FPutFieldTypeMap;
	UFC::PHashedList<UFC::AnsiString, TOptRecord*> FTable;
	TList*           FCallList;
	TList*           FPutList;
	TStringList*     FStrikePriceList;
	UFC::AnsiString  FSubsceibeExchange;
	UFC::PStringList FSubsceibeSymbolList;
	int              FResponseCount;
	int              FBestFitWidth;
	int              FStrikePxColIndex;
	int              FLastColColIndex;
	int              FCallTextCol;
	int              FPutTextCol;
	OptStkColName    FFieldTypes[ 64 ];
	UFC::AnsiString  FFieldsString;
	double           FTimeToClose;
	TTimer*          FTimer;
	bool             FBatchDirty;   ///< 需在批次呼叫時重算希臘值 / BK Bar
	bool             FPaintDirty;   ///< 需在批次呼叫時重繪
private:
	int     FTotalWidth;
	int     FStrikePxWidth;
	double  FBKValueMax;
private:
	TStringList* FHeaderRowText;
	TStringList* HeaderString;
	virtual Classes::TStringList* __fastcall GetSymbols( void );
	virtual void OnMarketDataUpdate( MarketDataMessage* Msg );
	virtual void OnMarketDataUpdate( BasicData* Msg );
	virtual void OnMarketDataUpdate( MatchInfo* Msg );
	virtual void OnMarketDataUpdate( TotalMatch* Msg );
	virtual void OnMarketDataUpdate( DayHighLowPrice* Msg );
	virtual void OnMarketDataUpdate( OpeningInfo* Msg );
	virtual void OnMarketDataUpdate( OrderBookData* Msg );
	virtual void OnMarketDataUpdate( ClosingMarketData* Msg );
	virtual void OnMarketDataUpdate( UnderlyingIndexInfo* Msg );
	virtual void OnMarketDataUpdate( SumOfOrderInfo* Msg );
private:
	void __fastcall DrawHeader( int Col, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall DrawFieldName( int Col, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall DrawStrikePrice( int Row, const Types::TRect &ARect, TGridDrawState AState );
	void __fastcall DrawPxField( int Row, const Types::TRect &ARect, TGridDrawState AState, CallPutCode CallPut, OptStkColName FieldType, bool CheckTotol = false );
	void __fastcall DrawPrice( const Types::TRect &ARect, TGridDrawState AState, TOptRecord* Record, double Px );
	void __fastcall DrawDoubleDash( const Types::TRect &ARect, TGridDrawState AState, TOptRecord* Record );
	void __fastcall DrawQtyField( int Row, const Types::TRect &ARect, TGridDrawState AState, CallPutCode CallPut, OptStkColName FieldType );
	void __fastcall DrawQty( const Types::TRect &ARect, TGridDrawState AState, TOptRecord* Record, int Qty );
	void __fastcall DrawGeeksField( int Row, const Types::TRect &ARect, TGridDrawState AState, CallPutCode CallPut, OptStkColName FieldType );
	void __fastcall DrawValue( const Types::TRect &ARect, TGridDrawState AState, TOptRecord* Record, double Value );
	void __fastcall DrawDiff( const Types::TRect &ARect, TGridDrawState AState, TOptRecord* Record );
	void __fastcall DrawDiffPx( int Row, const Types::TRect &ARect, TGridDrawState AState, CallPutCode CallPut);
	void __fastcall DrawImpVar( int Row, const Types::TRect &ARect, TGridDrawState AState, CallPutCode CallPut );
	void __fastcall DrawPercentage( const Types::TRect &ARect, TGridDrawState AState, TOptRecord* Record, double Value );
	void __fastcall DrawGrid( TOptRecord* Record );
	void __fastcall DrawBackground( const Types::TRect &ARect, Types::TRect& PaintRect, TOptRecord* Record, bool IsMouseOver );
	void __fastcall UpdateBKBar( bool FindMax );
	void __fastcall RecalcDirtyGreeks( void );
	void __fastcall UpdateCell( int ColIndex, int RowIndex );
	void __fastcall UpdateRow( int RowIndex );
	void __fastcall UpdateCallRow( int RowIndex );
	void __fastcall UpdatePutRow( int RowIndex );

	void __fastcall SetReverse( bool IsReverse );
	void __fastcall SetSymbol( UnicodeString Symbol );
	void __fastcall SetExchange( UnicodeString Exchange );
	void __fastcall SetYearMonth( UnicodeString YearMonth );
	void __fastcall SetDisplayName( UnicodeString Name );
	void __fastcall Subscribe( void );
	int  __fastcall GetMaxWidth( optStkColName Type );
	void __fastcall BuildStrikePriceList( void );
	TOptRecord* __fastcall GetRecord( CallPutCode CallPut, int RowIndex );
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	void __fastcall MouseLeave( System::TObject* Sender );
	void __fastcall MouseEnter(  System::TObject* Sender );
	void __fastcall WndProc( TMessage &Msg );
	int  __fastcall StrikePxFieldWidth( void );
	bool __fastcall StrikePxExists( double StkPx, TList* CPList );
	void __fastcall InitGrid();
	void __fastcall InitFieldNameValue();
	void __fastcall SetMarketDataStore( TCMarketDataStore* Store );
	void __fastcall AddUnderlyingRecord( TOptRecord* UnderlyingRec, UFC::PHashedList<UFC::AnsiString, TOptRecord*>& PinsTable );
	void __fastcall InAButNotInB( TList* AList, TList* BList, UFC::PHashedList<UFC::AnsiString, TOptRecord*>& AllPinsTable );
	void __fastcall CheckCallPutPair(  TList* CallList, TList* PutList, UFC::PHashedList<UFC::AnsiString, TOptRecord*>& PinsTable );
	void __fastcall AddOptionsByMonth( const String& YearMonth, TList* CallList, TList* PutList,UFC::PHashedList<UFC::AnsiString, TOptRecord*>& PinsTable,UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*>& ExcludeStkPxSet );
	void __fastcall UpdateUnderlyingBuffer( TOptRecord* UnderlyingRec );
	void __fastcall ScrollUp( int Tick );
	void __fastcall ScrollDown( int Tick );
	BasicInformation* __fastcall GetUnderlying( BasicInformation* Info, const String& YearMonth );
	UFC::AnsiString   __fastcall GetUnderlyingProductID( BasicInformation* Info );
	void __fastcall HandleMouseDown( Controls::TMouseButton Button, Classes::TShiftState Shift, int X,int Y, TOptRecord* SelRec, int CoordX, CallPutCode CallPut );
	void __fastcall BuidPinList( TList* CallPins, TList* PutPins, UFC::PHashedList<UFC::AnsiString, TOptRecord*>& PinsTable, UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*>& ExcludeStkPxSet );
	void __fastcall ClearList( TList* call, TList* put );
	void __fastcall MenuClicked( TObject* Sender );
    void __fastcall SetPopupMenuChecked( bool Checked );
private:
	void __fastcall SetHeaderCommodColor( TColor Col );
	void __fastcall SetHeaderCommodBKColor( TColor Col );
	void __fastcall SetHeaderCallColor( TColor Col );
	void __fastcall SetHeaderCallBKColor( TColor Col );
	void __fastcall SetHeaderPutColor( TColor Col );
	void __fastcall SetHeaderPutBKColor( TColor Col );
	void __fastcall SetFieldNameCallColor( TColor Col );
	void __fastcall SetFieldNameCallBKColor( TColor Col );
	void __fastcall SetFieldNamePutColor( TColor Col );
	void __fastcall SetFieldNamePutBKColor( TColor Col );
	void __fastcall SetFieldNameCommodColor( TColor Col );
	void __fastcall SetFieldNameCommodBKColor( TColor Col );
	void __fastcall SetCallBKColor( TColor Col );
	void __fastcall SetPutBKColor( TColor Col );
	void __fastcall SetRisePxColor( TColor Col );
	void __fastcall SetFallPxColor( TColor Col );
	void __fastcall SetEqualPxColor( TColor Col );
	void __fastcall SetHGridColor( TColor Col );
	void __fastcall SetVGridColor( TColor Col );
	void __fastcall SetBKBarColor( TColor Col );
	void __fastcall SetStkPxColor( TColor Col );
	void __fastcall SetStkPxBKColor( TColor Col );
	void __fastcall SetBKcol( TOptBKColName Col );
protected:
	void _fastcall DrawCell(int ACol, int ARow, const Types::TRect &ARect, TGridDrawState AState );
	virtual void __fastcall Loaded( void );
	void __fastcall MyMouseWheelDown( System::TObject* Sender, Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled );
	void __fastcall MyMouseWheelUp( System::TObject* Sender, Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled );
	void __fastcall TradetimeTimer( System::TObject* Sender );
public:
	__fastcall TOptionsStrikePriceView(TComponent* Owner);
	__fastcall ~TOptionsStrikePriceView();
	void __fastcall UpdateTable( void ); ///< Update all Symbols in TBar
	void __fastcall UpdateTable( UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*>& ExcludeStkPxSet );
	void __fastcall BatchTimer( System::TObject* Sender ); ///< 由外部(TBarForm)的 TTimer 週期呼叫, 批次重算希臘值並重繪
	void __fastcall Clear( void );
	void __fastcall CalSize( void );
	void __fastcall SetFieldsString( const String& Fields );
	String  __fastcall GetFieldsString( void );
	void __fastcall GetStrikePxInfo( int& Digit, UFC::PStringList& YM, UFC::PDoubleList& StrikePx, UFC::PDoubleList& SpotPx );
__published:
	__property Color;
	__property Align;
	__property Font;
	__property DrawingStyle;
	__property OnMouseDown;
__published:
	__property String             Exchange      = { read = FExchange, write = SetExchange };
	__property String             Symbol        = { read = FSymbol, write = SetSymbol };
	__property String             YearMonth     = { read = FYearMonth, write = SetYearMonth };
	__property String             DisplayName   = { read = FDisplayName, write = SetDisplayName };
	__property TCMarketDataStore* Store         = { read = FStore, write = SetMarketDataStore };
	__property TOptBKColName      BackgroundCol = { read = FBKCol, write = SetBKcol };
	__property bool               IsReverse     = { read = FIsReverse, write = SetReverse };
	__property bool               Locked        = { read = FLocked, write = FLocked };
	__property int                BestFitWidth  = { read = FBestFitWidth };
	/// All Colors
	///< Color for commodity grid cell.
	__property TColor HeaderCommodColor      = { read = FHeaderCommodColor,   write = SetHeaderCommodColor };
	__property TColor HeaderCommodBKColor    = { read = FHeaderCommodBKColor, write = SetHeaderCommodBKColor };
	///< Color for Call/put header color.
	__property TColor HeaderCallColor        = { read = FHeaderCallColor,   write = SetHeaderCallColor };
	__property TColor HeaderCallBKColor      = { read = FHeaderCallBKColor, write = SetHeaderCallBKColor };
	__property TColor HeaderPutColor         = { read = FHeaderPutColor,    write = SetHeaderPutColor };
	__property TColor HeaderPutBKColor       = { read = FHeaderPutBKColor,  write = SetHeaderPutBKColor };
	///< Color for Call/put header field name cells.
	__property TColor FieldNameCallColor     = { read = FFieldNameCallColor,   write = SetFieldNameCallColor };
	__property TColor FieldNameCallBKColor   = { read = FFieldNameCallBKColor, write = SetFieldNameCallBKColor };
	__property TColor FieldNamePutColor      = { read = FFieldNamePutColor,    write = SetFieldNamePutColor };
	__property TColor FieldNamePutBKColor    = { read = FFieldNamePutBKColor,  write = SetFieldNamePutBKColor };
	///< Color for header Strike Price field cell.
	__property TColor FieldNameCommodColor   = { read = FFieldNameCommodColor,   write = SetFieldNameCommodColor };
	__property TColor FieldNameCommodBKColor = { read = FFieldNameCommodBKColor, write = SetFieldNameCommodBKColor };
	///< Colors
	__property TColor CallBKColor            = { read = FCallBKColor,  write = SetCallBKColor };
	__property TColor PutBKColor             = { read = FPutBKColor,   write = SetPutBKColor };
	__property TColor RisePxColor            = { read = FRisePxColor,  write = SetRisePxColor };
	__property TColor FallPxColor            = { read = FFallPxColor,  write = SetFallPxColor };
	__property TColor EqualPxColor           = { read = FEqualPxColor, write = SetEqualPxColor };
	__property TColor HGridColor             = { read = FHGridColor,   write = SetHGridColor };
	__property TColor VGridColor             = { read = FVGridColor,   write = SetVGridColor };
	__property TColor BKBarColor             = { read = FBKBarColor,   write = SetBKBarColor };
	///< Color for Strike Price col.
	__property TColor StkPxColor 			 = { read = FStkPxColor,   write = SetStkPxColor };
	__property TColor StkPxBKColor			 = { read = FStkPxBKColor, write = SetStkPxBKColor };
	///< Events
	__property TNotifyEvent                  OnTBarFieldsChanged      = { read = FOnTBarFieldsChanged, write = FOnTBarFieldsChanged };
	__property TNotifyEvent                  OnTBarStrikePxClick      = { read = FOnTBarStrikePxClick, write = FOnTBarStrikePxClick };
	__property TOnTBarMouseDownEvent         OnTBarMouseDown          = { read = FOnTBarMouseDown, write = FOnTBarMouseDown };
	__property TOnTBarMouseClickEvent        OnTBarMouseClick         = { read = FOnTBarMouseClick, write = FOnTBarMouseClick };
	__property TOnTBarStrikePxMouseDownEvent OnTBarStrikePxMouseClick = { read = FOnTBarStrikePxMouseDown, write = FOnTBarStrikePxMouseDown };
};
//---------------------------------------------------------------------------
extern UFC::AnsiString gOptFieldsTypeName[];
extern TStringList*    gOptFieldsDisplayName;
const  int             gOptFieldsCount = 21;
//---------------------------------------------------------------------------
#endif
