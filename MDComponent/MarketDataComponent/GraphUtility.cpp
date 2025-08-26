//---------------------------------------------------------------------------
#pragma hdrstop
#include "GraphUtility.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
GDIClipRegion::GDIClipRegion( HDC hdc, int L,int T,int R, int B )
:FHDC( hdc )
{
	FHRgn = CreateRectRgn( L,T,R,B );
	SelectClipRgn( FHDC, FHRgn );
}
//------------------------------------------------------------------------------
GDIClipRegion::~GDIClipRegion()
{
	SelectClipRgn( FHDC, NULL );
	DeleteObject( FHRgn );
}
//------------------------------------------------------------------------------
void __fastcall GDIUtility::DrawGradientBar( TDirect2DCanvas* canvas, TRect& dRect,ID2D1LinearGradientBrush* GradientBrush )
{
	D2D1_RECT_F DrawRect = D2D1::RectF( 0,0, dRect.Width(), dRect.Height() );

	canvas->RenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(dRect.left, dRect.top));
	canvas->RenderTarget->FillRectangle(&DrawRect, GradientBrush );
	canvas->RenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}
//------------------------------------------------------------------------------
void __fastcall GDIUtility::DrawGradientBar( HDC GDIDC, TRect& dRect,TColor UpCol, TColor DownCol, bool IsVert )
{
								  ///< Vertex 1
	TRIVERTEX      Vertex[ 2 ] ={{dRect.Left,                  ///< X
								  dRect.Top,                   ///< Y
								  GetRValue( (DWORD)UpCol)<<8, ///< R
								  GetGValue( (DWORD)UpCol)<<8, ///< G
								  GetBValue( (DWORD)UpCol)<<8, ///< B
								  0x0000  },                     ///< Alpha
								  ///< Vertex 2
								 {dRect.Right,                  ///< X
								  dRect.Bottom,                 ///< Y
								  GetRValue( (DWORD)DownCol)<<8,///< R
								  GetGValue( (DWORD)DownCol)<<8,///< G
								  GetBValue( (DWORD)DownCol)<<8,///< B
								  0x0000  }};                     ///< Alpha
	GRADIENT_RECT  gRect    = { 0, 1 };
	ULONG          FillMode = ( IsVert == true )? GRADIENT_FILL_RECT_V:GRADIENT_FILL_RECT_H;

	GradientFill( GDIDC, Vertex, 2, &gRect, 1, FillMode );
}
//------------------------------------------------------------------------------
void __fastcall GDIUtility::DrawGradientBar( HDC GDIDC, TRect& dRect,TColor UpCol, TColor DownCol, int Alpha, bool IsVert )
{
								  ///< Vertex 1
	TRIVERTEX      Vertex[ 2 ] ={{dRect.Left,                  ///< X
								  dRect.Top,                   ///< Y
								  GetRValue( (DWORD)UpCol)<<8, ///< R
								  GetGValue( (DWORD)UpCol)<<8, ///< G
								  GetBValue( (DWORD)UpCol)<<8, ///< B
								  Alpha << 8 },                     ///< Alpha
								  ///< Vertex 2
								 {dRect.Right,                  ///< X
								  dRect.Bottom,                 ///< Y
								  GetRValue( (DWORD)DownCol)<<8,///< R
								  GetGValue( (DWORD)DownCol)<<8,///< G
								  GetBValue( (DWORD)DownCol)<<8,///< B
								  Alpha << 8 }};                     ///< Alpha
	GRADIENT_RECT  gRect    = { 0, 1 };
	ULONG          FillMode = ( IsVert == true )? GRADIENT_FILL_RECT_V:GRADIENT_FILL_RECT_H;

	GradientFill( GDIDC, Vertex, 2, &gRect, 1, FillMode );
}
//------------------------------------------------------------------------------
void __fastcall GDIUtility::DrawGradientBar( TCanvas* canvas, TRect& dRect, TColor UpCol, TColor DownCol, bool IsVert )
{
	DrawGradientBar( canvas->Handle, dRect, UpCol, DownCol,  IsVert );
}
//------------------------------------------------------------------------------
TColor __fastcall GDIUtility::BrighterColor( TColor Col, int Val )
{
	int R = GetRValue( (DWORD)Col) + Val;
	int G = GetGValue( (DWORD)Col) + Val;
	int B = GetBValue( (DWORD)Col) + Val;

	if( R > 255 ) R = 255;
	if( G > 255 ) G = 255;
	if( B > 255 ) B = 255;

	return (TColor) RGB(R,G,B);
}
//------------------------------------------------------------------------------
TColor __fastcall GDIUtility::ComplementaryColor( TColor Col )
{
	int R = 255 - GetRValue( (DWORD)Col);
	int G = 255 - GetGValue( (DWORD)Col);
	int B = 255 - GetBValue( (DWORD)Col);
	return (TColor) RGB(R,G,B);
}
//------------------------------------------------------------------------------
bool __fastcall GDIUtility::LineRectIntersect( int x1, int y1, int x2, int y2, const TRect& r )
{
   TPoint LT( x1,y1 );
   TPoint RB( x2,y2 );

   if( r.PtInRect( LT ) == true || r.PtInRect( RB ) == true )
	   return true;

   float rx = (float)r.left;
   float ry = (float)r.top;
   float rw = (float)r.Width();
   float rh = (float)r.Height();

   return LineRectIntersect( (float)x1, (float)y1, (float)x2, (float)y2, rx, ry,  rw, rh );
}
//------------------------------------------------------------------------------
bool __fastcall GDIUtility::LineRectIntersect( float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh)
{
  // check if the line has hit any of the rectangle's sides
  // uses the Line/Line function below
  bool left =   LineIntersect(x1,y1,x2,y2, rx,ry,rx, ry+rh);
  bool right =  LineIntersect(x1,y1,x2,y2, rx+rw,ry, rx+rw,ry+rh);
  bool top =    LineIntersect(x1,y1,x2,y2, rx,ry, rx+rw,ry);
  bool bottom = LineIntersect(x1,y1,x2,y2, rx,ry+rh, rx+rw,ry+rh);

  // if ANY of the above are true, the line
  // has hit the rectangle
  if( left || right || top || bottom )
	  return true;
  return false;
}
//------------------------------------------------------------------------------
bool __fastcall GDIUtility::LineIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
  // calculate the direction of the lines
  float uA = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
  float uB = ((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));

  // if uA and uB are between 0-1, lines are colliding
  if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
  {
	// optionally, draw a circle where the lines meet
	// float intersectionX = x1 + (uA * (x2-x1));
	// float intersectionY = y1 + (uA * (y2-y1));
	return true;
  }
  return false;
}
//------------------------------------------------------------------------------
