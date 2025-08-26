//---------------------------------------------------------------------------

#ifndef GraphUtilityH
#define GraphUtilityH
#include <Direct2D.hpp>
//---------------------------------------------------------------------------
class GDIClipRegion
{
private:
	HRGN FHRgn;
	HDC  FHDC;
public:
	GDIClipRegion( HDC hdc, int L,int T,int R, int B );
	~GDIClipRegion();
};
//---------------------------------------------------------------------------
class GDIUtility
{
public:
	static void __fastcall DrawGradientBar( TDirect2DCanvas* canvas, TRect& dRect,ID2D1LinearGradientBrush* GradientBrush );
	static void __fastcall DrawGradientBar( TCanvas* canvas, TRect& dRect, TColor UpCol, TColor DownCol, bool IsVert = true);
	static void __fastcall DrawGradientBar( HDC GDIDC, TRect& dRect,TColor UpCol, TColor DownCol, bool IsVert = true );
	static void __fastcall DrawGradientBar( HDC GDIDC, TRect& dRect,TColor UpCol, TColor DownCol, int Alpha, bool IsVert = true );
	static TColor __fastcall BrighterColor( TColor Col, int Val );
	static TColor __fastcall ComplementaryColor( TColor Col );
	static bool __fastcall LineIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
	static bool __fastcall LineRectIntersect( float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh);
	static bool __fastcall LineRectIntersect( int x1, int y1, int x2, int y2, const TRect& r );
};
//---------------------------------------------------------------------------
#endif
