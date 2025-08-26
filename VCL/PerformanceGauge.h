//---------------------------------------------------------------------------

#ifndef PerformanceGaugeH
#define PerformanceGaugeH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
//---------------------------------------------------------------------------
const int POINTS_MAX = 1024;
//---------------------------------------------------------------------------
class PACKAGE TPerformanceGauge : public TCustomControl
{
private:
	int    FMaxValue;
	int    FInitMaxValue;
	int    FWarningValue;
	int    FPixelSec;
	int    FTopBorder;
	int    FLeftBorder;
	int    FGrids;
	int    FScale;
	int    FDecimal;
	bool   FDrawVertical;
	bool   FDrawHorizontal;
	bool   FFill;
	TList* FDataList;
	TList* FData2List;
	TColor FBKColor;
	TColor FGridColor;
	TColor FLineColor;
	TColor FLine2Color;
	Graphics::TBitmap* FBufferBmp;
private:
	bool   FDrawSecondLine;
	TPoint FPoints[ POINTS_MAX ];
	void __fastcall PaintData( Graphics::TBitmap* BufferBmp );
	void __fastcall PaintGrid( Graphics::TBitmap* BufferBmp );
	void __fastcall PaintWarningLine( Graphics::TBitmap* BufferBmp );
	void __fastcall KeepInList( TList* DataList, int DataCount );
	void __fastcall CheckMax( TList* DataList );
	TColor  __fastcall DarkerColor( TColor OrigCol );
	void __fastcall CheckLeftBorder( void );
	void __fastcall Loaded( void );
protected:
	virtual void __fastcall WndProc( Messages::TMessage &Message );
	virtual void __fastcall Paint( void );
public:
	__fastcall TPerformanceGauge(TComponent* Owner);
	void __fastcall Add( int Value );
	void __fastcall Add2( int Value1, int Value2 );
	void __fastcall Clear( void );
	bool __fastcall DataExists( int TimeSec );
__published:
	__property Align;
	__property bool VerticalLine      = {read=FDrawVertical,write=FDrawVertical,default=true};
	__property bool HorizontalLine    = {read=FDrawHorizontal,write=FDrawHorizontal,default=true};
	__property bool DrawFill          = {read=FFill,write=FFill,default=false};
	__property int MaxValue           = {read=FInitMaxValue,write=FInitMaxValue};
	__property int WarningValue       = {read=FWarningValue,write=FWarningValue};
	__property int Decimal            = {read=FDecimal,write=FDecimal};
	__property TColor BackgroundColor = {read=FBKColor,write=FBKColor};
	__property TColor GridColor       = {read=FGridColor,write=FGridColor};
	__property TColor LineColor       = {read=FLineColor,write=FLineColor};
	__property TColor SecondLineColor = {read=FLine2Color,write=FLine2Color};
};
//---------------------------------------------------------------------------
#endif
