//---------------------------------------------------------------------------

#ifndef StockChartH
#define StockChartH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include "OrderMessageDefine.h"
#include "MarketDataStore.h"
#include "OrderStore.h"
#include "OrderBookList.h"
#include "ChartsStore.h"
//---------------------------------------------------------------------------
class TMinutesTickInfo
{
public:
	 int   Hour;
	 int   Minutes;
	 int   Qty;

	 // 20140717 by Tim Lin
	 double MaxPx;
	 double MinPx;
	 double OpenPx;
	 double ClosePx;
public:
	__fastcall TMinutesTickInfo( int* IntArray, int DecimalLocator  );
	__fastcall TMinutesTickInfo( TMinuteKInfo& KInfo );
	__fastcall TMinutesTickInfo( void )
	:Hour(0),Minutes(0),Qty(0),MaxPx(0),MinPx(0),OpenPx(0),ClosePx(0){}
	__fastcall TMinutesTickInfo( int H,int M, int qty, double max,double min,double open, double close )
	:Hour(H),Minutes(M),Qty(qty),MaxPx(max),MinPx(min),OpenPx(open),ClosePx(close){}
	void __fastcall Reset( void  );
};
//---------------------------------------------------------------------------
class TTextHint : public TCustomHintWindow
{
private:
	String  FText;
	int     FWidth;
	int     FHeight;
	Graphics::TTextFormat FFormats;
	TColor      FColor;
	TColor      FBKColor;
private:
	void __fastcall NCPaint( HDC DC ){} ///< Must overwrite.
	void __fastcall ChangeStyle( void );
public:
	void __fastcall SetVisible( void );
	__fastcall TTextHint( TWinControl* Owner, TFont* font );
	void __fastcall Init( TColor BK, TColor FNT, const String& MaxStr );
	void __fastcall SetPosition( int x, int y );
	void __fastcall SetString( const String& Msg );
	void __fastcall Paint( void );
	void __fastcall SetMaxWidth( const String& MaxStr);
	void __fastcall SetBKColor( TColor BK ) { FBKColor = BK; }
	void __fastcall SetColor( TColor Fnt ) { FColor = Fnt; }
};
//---------------------------------------------------------------------------
class TOrderHint : public TCustomHintWindow
{
private:
	String  FText;
	int     FWidth;
	int     FHeight;
	int     FHalfWidth;
	Graphics::TTextFormat FFormats;
private:
	void __fastcall NCPaint( HDC DC ){} ///< Must overwrite.
	void __fastcall ChangeStyle( void );
public:
	void __fastcall SetVisible( void );
	__fastcall TOrderHint( TWinControl* Owner, TFont* font);
	void __fastcall Init(  int W, int H  );
	void __fastcall SetPosition( int x, int y );
	void __fastcall SetString( const String& Msg );
	void __fastcall SetMaxWidth(  int MaxW  );
	void __fastcall Paint( void );
};
//---------------------------------------------------------------------------
class TInfoHint : public TCustomHintWindow
{
private:
	int    FHour;
	int    FMinutes;
	float  FOpenPx;
	float  FBullPx;
	float  FBearPx;
	float  FClosePx;
	int    FQty;
	int    FPxDigi;
	bool   FIsEmpty;
	TTickList* FList;
private:
	void __fastcall WndProc( TMessage &Msg );
	void __fastcall EraseBK( TMessage &Msg );
	void __fastcall NCPaint( HDC DC ){}
public:
	__fastcall TInfoHint( TWinControl* Owner);
	void __fastcall SetTickList( TTickList* List ) { FList = List; }
	void __fastcall ChangeShape( void );
	void __fastcall SetVisible( void );
	void __fastcall SetPosition( int x, int y );
	void __fastcall SetStrings( TStringList* Msgs );
	void __fastcall Paint( void );
	void __fastcall SetData( int h,int m, float Open,float Bull,float Bear,float Close,int Qty, int Digi );
	void __fastcall Empty( void ) { FIsEmpty = true; }
};
//---------------------------------------------------------------------------
class TTickHint : public TObject
{
private:
	float  FFillPx;
	int    FQty;
	int    FPxDigi;
	bool   FIsEmpty;
	TColor FColor;
private:
	HRGN   FWinRgn;
	Graphics::TBitmap* FBuffer;
	TRect  FBoundsRect;
private:
	void __fastcall SetClip( TCanvas* canvas, int X,int Y, bool IsRight );
	void __fastcall RestetClip( TCanvas* canvas );
public:
	__fastcall TTickHint( void );
	void __fastcall Paint( int x, int y, int MaxX, TCanvas* );
	void __fastcall SetData( float Fill, int Qty, int Digi, TColor Col );
	void __fastcall Empty( void ) { FIsEmpty = true; }
	const TRect& __fastcall GetRect( void ) {return FBoundsRect; }
};
//---------------------------------------------------------------------------
class PACKAGE TStockChart : public TCustomControl
{
private:
		///< Time
	   int   FBeginHour;
	   int   FBeginMinute;
	   int   FEndHour;
	   int   FEndMinute;
	   int   FMaxMinutesIndex; ///< Minutes Max
	   ///<  Strike price digit and decimal locator for price.
	   int   FStrikePxDigi;
	   int   FDecimalLocator;
	   ///<  Variables about Price
	   float FRefPrice;
	   int   FRefPxIndex;
	   int   FMaxPxIndex;
	   int   FMinPxIndex;
	   int   FQtyMax;
	   int   FIndexMax;
	   int   FLastFillQty;
	   ///<
	   float FPixelPerMinute; ///<
	   float FPixelPerTick;
	   int   FRefPxYOffset;
	   int   FFirstTickYOffset;
	   int   FTicksPerGrid;
	   int   FPixelsPerTickGrid;
	   float FFirstTickPx;
	   ///< Variables about ticks.
	   int   				  FTickCount;        ///< How many ticks.(equals to FTickArray.size())
	   std::vector<double>    FTickArray;        ///< Ticks array.(keep all tick price)
	   std::hash_map<int,int> FPxToTickIndexMap; ///< price to tick index map.(map price to tick index)
private:
	   int                FLastMouseX;
	   int                FLastMouseY;
	   int                FLastIndex;
	   TRect              FLineChartRect;
	   TRect              FBarChartRect;
	   Graphics::TBitmap* FBuffer;
	   TInfoHint*         FHintwin;
	   TTickHint*         FTickHintwin;
	   bool               FShowFillHint;
private: ///< for colors
	   TColor           FFrontColor;
	   TColor           FBullColor;
	   TColor           FBearColor;
	   TColor           FGridColor;
private: ///< Keep minutes data.(OpenPx,ClosePx,MaxPx,MinPx,Qty and a hava data flag)
	   TIntegerDynArray FQtys;
	   TIntegerDynArray FMaxPx;
	   TIntegerDynArray FMinPx;
	   TIntegerDynArray FOpenPx;
	   TIntegerDynArray FClosePx;
	   TBooleanDynArray FHaveData;
private:
	   void __fastcall CalculateDimension( void );
	   int  __fastcall PxToTickIndex( double Price );
	   void __fastcall PaintGrid(  TCanvas *canvas  );
	   void __fastcall DrawChartBorder( TCustomCanvas* );
	   void __fastcall DrawChartTimeGrid( TCustomCanvas* );
	   void __fastcall DrawChartTickGrid( TCustomCanvas* );
	   void __fastcall DrawText( TCustomCanvas* );
	   void __fastcall DrawPriceLine( TCustomCanvas *canvas );
	   void __fastcall ErasePriceLine( int Index, TCustomCanvas *canvas, bool AdjPenWidth = true );
	   void __fastcall DrawPriceLine( int Index, TCustomCanvas *canvas,  bool AdjPenWidth = true );
	   void __fastcall DrawVolumeBar( TCustomCanvas *canvas );
	   void __fastcall DrawVolumeBar( int Index, TCustomCanvas *canvas );
	   int  __fastcall XPosToIndex( int X );
	   void __fastcall EraseBackground( TMessage &Msg );
	   void __fastcall DrawNotLine(TCanvas *canvas,int x,int y,int x1,int y1);
	   void __fastcall HandleMouseLeave( TObject* );
	   void __fastcall HandleMouseEnter( TObject* );
	   void __fastcall ShowInfoHint( int Index, TPoint& ScreenPx );
	   void __fastcall ShowTickHint( int Index, bool Paint );
	   void __fastcall MoveHint( int Index, int X, int Y);
	   DYNAMIC void __fastcall MouseMove( Classes::TShiftState Shift, int X, int Y);
	   DYNAMIC void __fastcall Resize( void );
	   virtual void __fastcall WndProc( TMessage &Msg );
	   virtual void __fastcall Loaded( void );
	   void __fastcall DrawTickHintRect( int );
private:
	   void __fastcall SetTick( float Tick );
	   void __fastcall SetPriceDigi( int Digi );
	   void __fastcall SetTime( void );
	   void __fastcall IndexToTime( int i, int& h, int& m );
	   void __fastcall DrawCross( TCanvas* aCanvas, int X = -1 , int Y = -1, bool NeedErase = true );
	   int  __fastcall GetDataYPos( int Index, int Y );
	   void __fastcall ChangeCursor( void );
private:
	   void __fastcall SetCanvasFontColor( TCustomCanvas* aCanvas, TColor Color );
	   void __fastcall SetPenStyle( TCustomCanvas* aCanvas, TPenStyle PenStyle );
	   void __fastcall SetPenColor( TCustomCanvas* aCanvas, TColor Color );
	   void __fastcall SetPenWidth( TCustomCanvas* aCanvas, int Width );
	   void __fastcall SetFontColor( TColor Color );
	   void __fastcall SetBullColor( TColor Color );
	   void __fastcall SetBearColor( TColor Color );
	   void __fastcall SetGridColor( TColor Color );
public:
	   __fastcall TStockChart(TComponent* Owner);
	   __fastcall ~TStockChart( void );
	   void __fastcall SetTime( int OpenHours, int OpenMinutes, int CloseHours, int CloseMinutes );
	   void __fastcall SetSymbol( BasicInformation* SymbolInfo );
	   void __fastcall ClearData( void );
	   void __fastcall SetData( TMinutesTickInfo& TickInfo, bool IsTick, bool NeedPaint = true );
	   void __fastcall Paint( void );
__published:
	   __property Constraints;
	   __property OnResize;
	   __property Align;
	   __property Anchors;
	   __property Color;
	   __property Cursor;
	   __property TColor TextColor ={read = FFrontColor, write = SetFontColor };
	   __property TColor BullColor ={read = FBullColor, write = SetBullColor };
	   __property TColor BearColor ={read = FBearColor, write = SetBearColor };
	   __property TColor GridColor ={read = FGridColor, write = SetGridColor };
	   __property bool  ShowFillHoint  ={read = FShowFillHint, write = FShowFillHint };
	   __property int   DecimalLocator ={read = FDecimalLocator };
	   __property int   StrikePxDigi   ={read = FStrikePxDigi };
};
//---------------------------------------------------------------------------
#endif
