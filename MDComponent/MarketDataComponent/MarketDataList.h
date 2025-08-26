//---------------------------------------------------------------------------
#ifndef MarketDataListH
#define MarketDataListH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include "OrderMessageDefine.h"
#include "MarketDataStore.h"
#include "SortListForm.h"
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TOnMouseClickSymbolEvent)(System::TObject* Sender,const String& Excahnge, const String& Symbol, nsOrderMessageDefine::SideEnum side, double Price );
typedef void __fastcall (__closure *TOnMouseClickItemEvent)(System::TObject* Sender,const String& Excahnge, const String& Symbol );
//---------------------------------------------------------------------------
class TMDListRecord
{
private:
	String FExchange;
	String FSymbol;
	String FDisplayName;
	bool   FTradeFlag;
	bool   FPriceRise;
	double FBidPx;
	double FAskPx;
	double FTradePx;
	double FOpeningPx;
	double FClosingPx;
	double FSettelmentPx;
	double FRefPx;
	double FBullPx;
	double FBearPx;
	double FLowPx;
	double FHiPx;
	int FBidQty;
	int FAskQty;
	int FBidTotalQty;
	int FAskTotalQty;
	int FTradeQty;
	int FTradeVolume;
	int FTotalBuy;
	int FTotalSell;
	double FDayHighPx;
	double FDayLowPx;
	int FOpenInterest;
	int FBase;
	int FDigit;
	UFC::AnsiString FTradeTime;
	TTickList*      FTicks;
public:
	TMDListRecord( const String& Exchange, const String& Symbol, bool TradeFlag = true );
	TTickList* GetTickList( void ) { return FTicks; }
	void       SetTickList( const TTickList* Ticklist );
	String GetExchange( void ) { return FExchange; }
	String GetSymbol( void ) { return FSymbol; }
	String GetDisplayName( void ) { return FDisplayName; }
	double GetBidPx( void ) { return FBidPx; }
	double GetAskPx( void ) { return FAskPx; }
	double GetTradePx( void ) { return FTradePx; }
	double GetOpeningPx( void ) { return FOpeningPx; }
	double GetClosingPx( void ) { return FClosingPx; }
	double GetSettlementPx( void ) { return FSettelmentPx; }
	double GetDayHighPx( void ) { return FDayHighPx; }
	double GetDayLowPx( void ){ return FDayLowPx; }
	double GetRefPx( void ){ return FRefPx; }
	double GetBullPx( void ) { return FBullPx; }
	double GetBearPx( void ) { return FBearPx; }
	int GetBidQty( void ) { return FBidQty; }
	int GetAskQty( void ) { return FAskQty; }
	int GetTotalBidQty( void ) { return FBidTotalQty; }
	int GetTotalAskQty( void ) { return FAskTotalQty; }
	int GetTradeVolume( void ) { return FTradeVolume; }
	int GetOpenInterest( void ) { return FOpenInterest; }
	int GetBase( void ) { return FBase; }
	int GetDigit( void ) { return FDigit; }
	int GetTradeQty( void ) { return FTradeQty; }
	UFC::AnsiString& GetTradeTime( void ) { return FTradeTime; }
	bool GetTradeFlag( void ) { return FTradeFlag; }
	bool IsMatched( void ) { return ( FTradeVolume > 0 && FTradePx > 0.0 ); }

	void SetBidPx( double Px ) { FBidPx = Px; }
	void SetAskPx( double Px ) { FAskPx = Px; }
	void SetBidQty( int Qty ) { FBidQty = Qty; }
	void SetAskQty( int Qty ) { FAskQty = Qty; }
	void SetTotalBidQty( int Qty ) { FBidTotalQty = Qty; }
	void SetTotalAskQty( int Qty ) { FAskTotalQty = Qty; }
	void SetTradeQty( int Qty );
	void SetTradePx( double Px );
	bool IsRise() { return FPriceRise; }
	void SetOpeningPx( double Px ) { FOpeningPx = Px; }
	void SetClosingPx( double Px ) { FClosingPx = Px; }
	void SetSettlementPx( double Px ) { FSettelmentPx = Px; }
	void SetTradeVolume( int Qty ) { FTradeVolume = Qty; }
	void SetDayHighPx( double Px ) { FDayHighPx = Px; }
	void SetDayLowPx( double Px ) { FDayLowPx = Px; }
	void SetOpenInterest( int Qty ) { FOpenInterest = Qty; }
	void SetBase( int Base ) { FBase = Base; }
	void SetDigit( int Digit ) { FDigit = Digit; }
	void SetRefPx( double Px ) { FRefPx = Px; }
	void SetBullPx( double Px ) { FBullPx = Px; }
	void SetBearPx( double Px ) { FBearPx = Px; }
	bool IsLimitUp( void );
	bool IsLimitDown( void );
	void SetDisplayName( String Name ) { FDisplayName = Name; }
	void SetTradeTime( const UFC::AnsiString& Time ) { FTradeTime = Time; }
};
//---------------------------------------------------------------------------
class PACKAGE TMarketDataList : public TCustomListView, public IMarketDataListener
{
private:
	static UFC::Int32 BaseYear;
	static UFC::Int32 Remainder;
	String FExchange;
	TCMarketDataStore* FStore;
	bool FIsLoaded;
	UFC::PHashedList<UFC::AnsiString, TMDListRecord*> FRecordTable;
	UFC::PHashedList<UFC::AnsiString, TListItem*> FListItemTable;
	TColor FOddRowColor;
	TColor FEvenRowColor;
	TColor FTextColor;
	TColor FRiseBKColor;
	TColor FFallBKColor;
	TColor FSymbolColor;
	TImageList* FImageList;
	TCanvas* FEarseBKCanvas;
	Graphics::TBitmap* FBufferBmp;
	TOnMouseClickSymbolEvent FOnMouseClickSymbolEvent;
	TOnMouseClickItemEvent   FOnMouseClickItemEvent;
	TNotifyEvent             FOnItemPosChanged;
	bool FCustomSort;
	bool FUpdating;
	UFC::PStringList FOrderList;
private:
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
	void __fastcall InitString( void );
	void __fastcall InitialCol( void );
//	void __fastcall MyDrawItem(TCustomListView* Sender, TListItem* Item, const Types::TRect &Rect, Windows::TOwnerDrawState State);
	void __fastcall SetStore( TCMarketDataStore* Store );
	void __fastcall Subscribe( const String& Exchange, const String& Symbol );
	void __fastcall Unsubscribe( const String& Exchange, const String& Symbol );
	void __fastcall MenuClicked( TObject* Sender );
	void __fastcall SetTextColor( TColor Color );
	void __fastcall SetEvenRowColor( TColor Color );
	void __fastcall SetOddRowColor( TColor Color );
	void __fastcall SetRiseBKColor( TColor Color );
	void __fastcall SetFallBKColor( TColor Color );
	void __fastcall SetSymbolColor( TColor Color );
	void __fastcall WndProc( TMessage &Msg );
	void __fastcall TMarketDataList::EraseBK( HDC DC );
	void __fastcall DrawLimitUp( UnicodeString& Text,  const Types::TRect &ARect );
	void __fastcall DrawLimitDown( UnicodeString& Text, const Types::TRect &ARect );
	void __fastcall DrawSelect( UnicodeString& Text,  const Types::TRect &ARect );
	void __fastcall DrawPriceUp( UnicodeString& Text,  const Types::TRect &ARect, int Row );
	void __fastcall DrawPriceDown( UnicodeString& Text,  const Types::TRect &ARect, int Row );
	void __fastcall DrawOrd( UnicodeString& Text,  const Types::TRect &ARect, int Row );
	void __fastcall DrawFillQty( UnicodeString& drawtext, const Types::TRect &ARect, int Row, bool Rise );
	void __fastcall DrawPrice( TMDListRecord* Record, double ComparePrice, UnicodeString& Text,  const Types::TRect &ARect, int Row, bool IsSelected );
	void __fastcall DrawPxDiff( int Row, double DiffPx, const Types::TRect &ARect, int Digit, bool IsSelected, TMDListRecord* Record );
	void __fastcall DrawPercentage( int Row, double DiffPx, const Types::TRect &ARect, double RefPx, bool IsSelected );
	void __fastcall DrawDoubleDash( int Row, const Types::TRect &ARect, bool IsSelected );
	int __fastcall GetMinWidth( int Col );
    int __fastcall GetMinFieldWidth( int FieldID );
	void __fastcall UserCompare(System::TObject* Sender, TListItem* Item1, TListItem* Item2, int Data, int &Compare);
	void __fastcall SetPopupMenuChecked( bool Checked );
	void __fastcall SetColWidth( TListColumn* NewCol, const String& ColName, int SetW = 0 );
	TListColumn* __fastcall AddColField( int Field, int SetW = 0  );
private:
	void __fastcall AddToRecordTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, TMDListRecord* Record );
	TMDListRecord* __fastcall SearchFromRecordTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol );
	void __fastcall RemoveFromRecordTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol );
	void __fastcall AddToListItemTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, TListItem* Record );
	TListItem* __fastcall SearchFromListItemTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol );
	void __fastcall RemoveFromListItemTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol );
protected:
	virtual void __fastcall Loaded(void);
	//DYNAMIC void __fastcall ColRightClick(TListColumn* Column, const Types::TPoint &Point);
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall DragDrop(System::TObject* Source, int X, int Y);
	DYNAMIC void __fastcall DragOver(System::TObject* Source, int X, int Y, System::Uitypes::TDragState State, bool &Accept);
	void __fastcall DrawItem( TListItem* Item, const Types::TRect &Rect, Windows::TOwnerDrawState State);
public:
	__fastcall TMarketDataList(TComponent* Owner);
	virtual __fastcall ~TMarketDataList();
	void __fastcall AddSymbol( const String& Exchange, const String& Symbol );
	void __fastcall RemoveSymbol( const String& Exchange, const String& Symbol );
	void __fastcall RemoveAllSymbol( void );
	void __fastcall BeginUpdate( void );
	void __fastcall EndUpdate( void );
	int  __fastcall ItemCount( void );
	BasicInformation* __fastcall GetSymbol( int index );
	void __fastcall GetColumns( UnicodeString& Str );
	void __fastcall SetColumns( const String& ColStr );
	void __fastcall AdjuestCol( void );
__published:
	__property BevelEdges;
	__property BevelInner;
	__property BevelOuter;
	__property BevelKind;
	__property BevelWidth;
	__property BorderStyle;

	__property Font;
	__property Align;
	__property OnMouseDown;
	__property OnMouseEnter;
	__property OnMouseLeave;
	__property OnMouseMove;
	__property OnMouseUp;
	__property OnDblClick;
	__property OnDragDrop;
	__property OnDragOver;
	__property Hint;
	__property ViewStyle;
	__property bool Sort = { read = FCustomSort, write  = FCustomSort };
	__property UnicodeString Exchange = { read = FExchange, write = FExchange };
	__property TCMarketDataStore* Store = { read = FStore, write = SetStore };
	__property TColor TextColor = { read = FTextColor, write = SetTextColor };
	__property TColor OddRowColor = { read = FOddRowColor, write = SetOddRowColor };
	__property TColor EvenRowColor = { read = FEvenRowColor, write = SetEvenRowColor };
	__property TColor RiseBKColor = { read = FRiseBKColor, write = SetRiseBKColor };
	__property TColor FallBKColor = { read = FFallBKColor, write = SetFallBKColor };
	__property TColor SymbolColor = { read = FSymbolColor, write = SetSymbolColor };
	__property TOnMouseClickSymbolEvent OnMouseClickSymbol = { read = FOnMouseClickSymbolEvent, write = FOnMouseClickSymbolEvent};
	__property TOnMouseClickItemEvent OnMouseClickItem = { read = FOnMouseClickItemEvent, write = FOnMouseClickItemEvent };
	__property TNotifyEvent OnItemPosChanged = { read = FOnItemPosChanged, write = FOnItemPosChanged };
};
//---------------------------------------------------------------------------
#endif
