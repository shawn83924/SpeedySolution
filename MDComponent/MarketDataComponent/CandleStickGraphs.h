//---------------------------------------------------------------------------

#ifndef CandleStickGraphsH
#define CandleStickGraphsH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <Gdiplus.h>
#include "OrderMessageDefine.h"
//---------------------------------------------------------------------------
class TCandleStickChart;
//---------------------------------------------------------------------------
class TKBarGraph
{
private:
	TColor FColor;
public:
	void   SetColor( TColor cl )  { FColor = cl; }
	TColor GetColor( void )       { return FColor; }
public:
	TKBarGraph( TColor cl ):FColor( cl ) {}
	virtual void Paint( TCanvas* ) = 0;
	virtual void Paint( Gdiplus::Graphics* ) = 0;
	virtual bool HitTest( int vx, int vy ) = 0;
	virtual bool HitTest(  int vx, int vy, int& DeltaX, int& DeltaY ) = 0;
	virtual bool Intersects(const TRect &r) = 0;
	virtual void MoveTo( int IndexX, int TickY ) = 0;
	virtual void Paint( int X,int Y, int Dx, int Dy, TCanvas* ) = 0;

};
//---------------------------------------------------------------------------
class TVLine : public TKBarGraph
{
private:
	int FXidx;
	TCandleStickChart* FOwn;
public:
	TVLine( TCandleStickChart* own, int xidx, TColor col );
	virtual void Paint( TCanvas* );
	virtual void Paint( Gdiplus::Graphics* );
	virtual bool HitTest( int vx, int vy );
	virtual bool HitTest( int vx, int vy, int& DeltaX, int& DeltaY  );
	virtual bool Intersects(const TRect &r);
	virtual void MoveTo( int IndexX, int TickY );
	virtual void Paint( int X,int Y, int Dx, int Dy, TCanvas* );
};
//---------------------------------------------------------------------------
class THLine : public TKBarGraph
{
private:
	int FTick;
	bool FShowPxText;
	bool FIsAbove;
	TCandleStickChart* FOwn;
public:
	THLine( TCandleStickChart* own, int tick, TColor col );
	void MoveTo( int TickY );
	int  GetY( void );
	void SetShowPrice( bool Show, bool Above );
	virtual void Paint( TCanvas* );
	virtual void Paint( Gdiplus::Graphics* );
	virtual bool HitTest( int vx, int vy );
	virtual bool HitTest( int vx, int vy, int& DeltaX, int& DeltaY  );
	virtual bool Intersects(const TRect &r);
	virtual void MoveTo( int IndexX, int TickY );
	virtual void Paint( int X,int Y, int Dx, int Dy, TCanvas* );
};
//---------------------------------------------------------------------------
class TLine : public TKBarGraph
{
private:
	int FIndex1;
	int FTick1;
	int FIndex2;
	int FTick2;
	int FSlop;
	TCandleStickChart* FOwn;
	bool InRange( int p1,int p2, int p );
public:
	TLine( TCandleStickChart* own, int index1, int tick1,  int index2, int tick2, TColor col );
	virtual void Paint( TCanvas* );
	virtual void Paint( Gdiplus::Graphics* );
	virtual bool HitTest( int vx, int vy );
	virtual bool HitTest( int vx, int vy, int& DeltaX, int& DeltaY  );
	virtual bool Intersects(const TRect &r);
	virtual void MoveTo( int IndexX, int TickY );
	virtual void Paint( int X,int Y, int Dx, int Dy, TCanvas* );
};
//---------------------------------------------------------------------------
class TText : public TKBarGraph
{
private:
	int FIndex;
	int FTick;
	int FSize;
	String FCaption;
	System::Uitypes::TFontStyles FStyle;
	TCandleStickChart* FOwn;
	TRect FBounds;
public:
	TText( TCandleStickChart* own, int index, int tick, TColor col, const String& text );
	int   GetSize( void ) { return FSize; }
	const String& GetText( void )      { return  FCaption; }
	void  SetSize( int Size );
	void  SetText( const String& txt );
	void  SetStyle( System::Uitypes::TFontStyles Style );
	void  RePaint( );
	virtual void Paint( TCanvas* );
	virtual void Paint( Gdiplus::Graphics* );
	virtual bool HitTest( int vx, int vy );
	virtual bool HitTest( int vx, int vy, int& DeltaX, int& DeltaY  );
	virtual bool Intersects(const TRect &r);
	virtual void MoveTo( int IndexX, int TickY );
	virtual void Paint( int X,int Y, int Dx, int Dy, TCanvas* );
};
//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
typedef enum olHitResult
{
	olNone   = 0,
	olDelete = 1,
	olMove   = 2

}HitResult;
//---------------------------------------------------------------------------
class TOrderLine
{
protected:
	static int TXT_W_SPACE;
	static int TXT_H_SPACE;
protected:
	double FPrice;
	int    FQty;
	bool   FIsLong;
public:
	TOrderLine( void ):FPrice(0.0),FQty( 0 ),FIsLong(true){}
	double   GetPrice( void ) { return FPrice; }
	int      GetQty( void )   { return FQty; }
	nsOrderMessageDefine::SideEnum GetSide( void )  { return FIsLong? nsOrderMessageDefine::sBuy:nsOrderMessageDefine::sSell; }
public:
	virtual void Paint( TCanvas* ) = 0;
	virtual HitResult HitTest( int vx, int vy ) = 0;
};
//---------------------------------------------------------------------------
class TPositionLine : public TOrderLine
{
private:
	int    FPriceTick;
	TColor FTextColor;
	String FPxString;
	TCandleStickChart* FOwn;
	void UpdateText( void );
public:
	TPositionLine( TCandleStickChart* Owner );
	void SetAvgPrice( double Px );
	void SetPosition( int Pos );
	virtual void Paint( TCanvas* );
	virtual HitResult HitTest( int vx, int vy ) { return olNone; }
};
//---------------------------------------------------------------------------
class TWorkingLine : public TOrderLine
{
private:
	int    FPriceTick;
	TColor FColor;
	TColor FBkColor;
	TRect  FBounds;
	TRect  FCxlBounds;
	TCandleStickChart* FOwn;
	void UpdateSize( TCanvas* canvas );
private:
	bool FInit;
	bool FDirty;
	bool FIsStop;
	int FQtyWidth;
	int FBSWidth;
	int FPxWidth;
	int FCxWidth;
	int FHalfHeight;
	int FxHotspot;
	int FyHotspot;
	String FPxString;
	String FQtyString;
	String FSideString;
	String FCxlString;
	TTextFormat  FFormats;
	inline int TextWidth();
	void DrawBK( TCanvas*, TRect&, int AvgPxY = 0 );
	void DrawQty( TCanvas*, TRect& );
	void DrawSide( TCanvas*, TRect& );
	void DrawPrice( TCanvas*, TRect& , String& PxStr);
	void DrawCancel( TCanvas*, TRect& );
private:
	double FOrigPx;
public:
	TWorkingLine( TCandleStickChart* Owner, double Px, int Qty, bool IsBuy, bool IsStop );
	bool IsStopOrder( void ) { return FIsStop; }
	void SetPrice( double Px );
	void SetMovingPrice( double Px );
	void StopMoving( void );
	void SetQty( int Pos );
	int  GetPriceTick( void ) { return FPriceTick; }
	virtual void Paint( TCanvas* );
	virtual HitResult HitTest( int vx, int vy );

};
//---------------------------------------------------------------------------
#endif
