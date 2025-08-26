//---------------------------------------------------------------------------

#pragma hdrstop

#include "CandleStickGraphs.h"
#include "CandleStickChart.h"
#include "GraphUtility.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
//
// Graphs
//
//---------------------------------------------------------------------------
TVLine::TVLine( TCandleStickChart* own, int xindex, TColor col )
:TKBarGraph( col )
,FXidx( xindex )
,FOwn( own )
{
}
//---------------------------------------------------------------------------
void TVLine::Paint( TCanvas* canvas )
{
	int x = FOwn->IndexToViewPortX( FXidx );

	canvas->Pen->Color = GetColor();
	canvas->MoveTo( x, 0 );
	canvas->LineTo( x, FOwn->Height );
}
//---------------------------------------------------------------------------
void TVLine::Paint( int X,int Y, int Dx, int Dy, TCanvas* canvas )
{
	 TPenMode Mode = canvas->Pen->Mode;

	 canvas->Pen->Mode  = pmNot;
	 canvas->Pen->Width = 1;
	 canvas->Pen->Style = psSolid;
	 canvas->MoveTo( X, 0 );
	 canvas->LineTo( X, FOwn->Height );
	 canvas->Pen->Mode  = Mode;
}
//---------------------------------------------------------------------------
void TVLine::Paint( Gdiplus::Graphics* GPPGraphics )
{
	int           x = FOwn->IndexToViewPortX( FXidx );
	Gdiplus::Pen  DrawPen( Gdiplus::Color(255, GetRValue(GetColor()), GetGValue(GetColor()), GetBValue(GetColor())), 1 );

	GPPGraphics->DrawLine( &DrawPen,x,0,x, FOwn->Height );
}
//---------------------------------------------------------------------------
bool TVLine::HitTest( int x, int y )
{
	int IdxX = FOwn->XPosToIndex( x );

	if( abs( FXidx - IdxX ) < 3 )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool TVLine::HitTest( int vx, int vy, int& DeltaX, int& DeltaY  )
{
	DeltaX = 0;
	DeltaY = 0;
	return  HitTest( vx, vy );
}
//---------------------------------------------------------------------------
bool TVLine::Intersects(const TRect &r)
{
	int x = FOwn->IndexToViewPortX( FXidx );
	if( x >= r.left && x <= r.right )
		return true;
	return false;
}
//---------------------------------------------------------------------------
void TVLine::MoveTo( int IndexX, int TickY )
{
	FXidx = IndexX;
}
//---------------------------------------------------------------------------
THLine::THLine( TCandleStickChart* own, int tick, TColor col )
:TKBarGraph( col )
,FTick( tick )
,FShowPxText( false )
,FIsAbove( true )
,FOwn( own )
{
}
//---------------------------------------------------------------------------
void THLine::SetShowPrice( bool Show, bool Above )
{
	FShowPxText = Show;
	FIsAbove    = Above;
}
//---------------------------------------------------------------------------
void THLine::Paint( TCanvas* canvas )
{
	int y = FOwn->TickToViewPortY( FTick );

	canvas->Pen->Color = GetColor();
	canvas->MoveTo( 0, y );
	canvas->LineTo( FOwn->Width, y );
	if( FShowPxText == true )
	{
		TTextFormat Formats;
		TRect TxtRect;
		String PxStr;

		Formats<<tfCenter<<tfVerticalCenter;
		canvas->Font->Color = GetColor();
		canvas->Font->Size = 10;
		canvas->Brush->Style = bsClear;
		FOwn->GetPriceTickString( FTick, PxStr );
		TxtRect.left   = 0;
		TxtRect.right  = canvas->TextWidth( PxStr ) + 10;
		if( FIsAbove == true )
		{
			TxtRect.top   = y - canvas->TextHeight( PxStr ) - 2;
			TxtRect.bottom = y - 2;
		}
		else
		{
			TxtRect.top   = y + 2;
			TxtRect.bottom = y + canvas->TextHeight( PxStr ) + 2;
		}
		canvas->TextRect( TxtRect, PxStr, Formats );
    }

}
//---------------------------------------------------------------------------
void THLine::Paint( int X,int Y, int Dx, int Dy, TCanvas* canvas )
{
	 TPenMode Mode = canvas->Pen->Mode;

	 canvas->Pen->Mode  = pmNot;
	 canvas->Pen->Width = 1;
	 canvas->Pen->Style = psSolid;
	 canvas->MoveTo( 0, Y );
	 canvas->LineTo( FOwn->Width, Y );
	 canvas->Pen->Mode  = Mode;
}
//---------------------------------------------------------------------------
void THLine::Paint( Gdiplus::Graphics* GPPGraphics )
{
	int          y = FOwn->TickToViewPortY( FTick );
	Gdiplus::Pen DrawPen( Gdiplus::Color(255, GetRValue(GetColor()), GetGValue(GetColor()), GetBValue(GetColor())), 1 );

	GPPGraphics->DrawLine( &DrawPen,0,y,FOwn->Width, y );
}
//---------------------------------------------------------------------------
bool THLine::HitTest( int x, int y )
{
	int Tick = FOwn->YPosToTick( y );

	if( abs( FTick - Tick ) < 3 )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool THLine::HitTest( int vx, int vy, int& DeltaX, int& DeltaY  )
{
	DeltaX = 0;
	DeltaY = 0;
	return  HitTest( vx, vy );
}
//---------------------------------------------------------------------------
bool THLine::Intersects(const TRect &r)
{
	int y = FOwn->TickToViewPortY( FTick );
	if( y >= r.top && y <= r.bottom )
		return true;
	return false;
}
//---------------------------------------------------------------------------
int THLine::GetY( void )
{
    return FOwn->TickToViewPortY( FTick );
}
//---------------------------------------------------------------------------
void THLine::MoveTo( int TickY )
{
	FTick = TickY;
}
//---------------------------------------------------------------------------
void THLine::MoveTo( int IndexX, int TickY )
{
	FTick = TickY;
}
//---------------------------------------------------------------------------
TLine::TLine( TCandleStickChart* own, int index1, int tick1,  int index2, int tick2, TColor col )
:TKBarGraph( col )
,FIndex1( index1 )
,FTick1( tick1 )
,FIndex2( index2 )
,FTick2( tick2 )
,FOwn( own )
{
	if( FIndex2 == FIndex1 )
		FSlop = 0;
	else
		FSlop = 10*fabs((float)(FTick2-FTick1)) / fabs( (float)(FIndex2 - FIndex1));
}
//---------------------------------------------------------------------------
void TLine::Paint( TCanvas* canvas )
{
	int x1 = FOwn->IndexToViewPortX( FIndex1 );
	int y1 = FOwn->TickToViewPortY( FTick1 );
	int x2 = FOwn->IndexToViewPortX( FIndex2 );
	int y2 = FOwn->TickToViewPortY( FTick2 );

	canvas->Pen->Color = GetColor();
	canvas->MoveTo( x1, y1 );
	canvas->LineTo( x2, y2 );
}
//---------------------------------------------------------------------------
void TLine::Paint( int X,int Y, int Dx, int Dy, TCanvas* canvas )
{
	int NewX = X-Dx;
	int NewY = Y-Dy;
	int x1 = FOwn->IndexToViewPortX( FIndex1 );
	int y1 = FOwn->TickToViewPortY( FTick1 );
	int x2 = FOwn->IndexToViewPortX( FIndex2 ) + (NewX -x1);
	int y2 = FOwn->TickToViewPortY( FTick2 ) + (NewY -y1);

	TPenMode Mode = canvas->Pen->Mode;

	canvas->Pen->Mode  = pmNot;
	canvas->Pen->Width = 1;
	canvas->Pen->Style = psSolid;
	canvas->MoveTo( NewX, NewY );
	canvas->LineTo( x2, y2 );
	canvas->Pen->Mode  = Mode;
}
//---------------------------------------------------------------------------
void TLine::Paint( Gdiplus::Graphics* GPPGraphics )
{
	int x1 = FOwn->IndexToViewPortX( FIndex1 );
	int y1 = FOwn->TickToViewPortY( FTick1 );
	int x2 = FOwn->IndexToViewPortX( FIndex2 );
	int y2 = FOwn->TickToViewPortY( FTick2 );
	Gdiplus::Pen   DrawPen( Gdiplus::Color(255, GetRValue(GetColor()), GetGValue(GetColor()), GetBValue(GetColor())), 1 );

	GPPGraphics->DrawLine( &DrawPen, x1, y1, x2, y2 );
}
//---------------------------------------------------------------------------
bool TLine::InRange( int v1,int v2, int v )
{
	int Max,Min;
	if( v1 > v2 )
	{
		Max = v1;
		Min =v2;
	}
	else
	{
		Max = v2;
		Min = v1;
	}
	if( v >= Min && v <= Max )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool TLine::HitTest( int x, int y )
{
	int Tick  = FOwn->YPosToTick( y );
	int Index = FOwn->XPosToIndex( x );
	int Slop;

	if( InRange( FTick1,  FTick2, Tick ) == true  &&
		InRange( FIndex1,  FIndex2, Index ) == true )
	{
		float DeltaX = fabs( (float)(Index - FIndex1) );

		if( DeltaX < 5  )
			return true;
		if( Index == FIndex1 )
			Slop = 0;
		else
			Slop = 10*fabs( (float)(Tick - FTick1)) /DeltaX;
		if( Slop <= FSlop + 5 && Slop >= FSlop -5 )
			return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool TLine::HitTest( int vx, int vy, int& DeltaX, int& DeltaY  )
{
	bool Hit = HitTest( vx, vy );

	if( Hit == true )
	{
		int x1 = FOwn->IndexToViewPortX( FIndex1 );
		int y1 = FOwn->TickToViewPortY( FTick1 );
		DeltaX = vx - x1;
		DeltaY = vy - y1;
	}
	else
	{
		DeltaX = 0;
		DeltaY = 0;
	}
	return Hit;
}
//---------------------------------------------------------------------------
bool TLine::Intersects(const TRect &r)
{
	int x1 = FOwn->IndexToViewPortX( FIndex1 );
	int y1 = FOwn->TickToViewPortY( FTick1 );
	int x2 = FOwn->IndexToViewPortX( FIndex2 );
	int y2 = FOwn->TickToViewPortY( FTick2 );

	return GDIUtility::LineRectIntersect(  x1,  y1,  x2,  y2, r );
}
//---------------------------------------------------------------------------
void TLine::MoveTo( int IndexX, int TickY )
{
	int DeltaX = IndexX - FIndex1;
	int DeltaY = TickY - FTick1;

	FIndex1 += DeltaX;
	FTick1  += DeltaY;
	FIndex2 += DeltaX;
	FTick2  += DeltaY;
}
//---------------------------------------------------------------------------
TText::TText( TCandleStickChart* own, int index, int tick, TColor col, const String& text )
:TKBarGraph( col )
,FIndex( index )
,FTick( tick )
,FOwn( own )
,FSize( 10 )
,FCaption( text )
{
	FStyle.Clear();
}
//---------------------------------------------------------------------------
void TText::Paint( TCanvas* canvas )
{
	TFontStyles OrgStyle = canvas->Font->Style;
	TTextFormat Formats;

	Formats<<tfCenter<<tfVerticalCenter;
	canvas->Font->Color = GetColor();
	canvas->Font->Size = FSize;
	canvas->Font->Style = FStyle;
	canvas->Brush->Style = bsClear;
	FBounds.top    = FOwn->TickToViewPortY( FTick );
	FBounds.left   = FOwn->IndexToViewPortX( FIndex );
	FBounds.bottom = FBounds.top + canvas->TextHeight( FCaption ) + 6;
	FBounds.right  = FBounds.left + canvas->TextWidth( FCaption ) + 10;
	canvas->TextRect( FBounds, FCaption, Formats );
	canvas->Font->Style = OrgStyle;
}
//---------------------------------------------------------------------------
void TText::Paint( int X,int Y, int Dx, int Dy, TCanvas* canvas )
{
	TRect Bounds;
	TPenMode Mode = canvas->Pen->Mode;

	Bounds.top    = Y-Dy;
	Bounds.left   = X-Dx;
	Bounds.bottom = Bounds.top  + FBounds.Height();
	Bounds.right  = Bounds.left + FBounds.Width();
	canvas->Pen->Mode  = pmNot;
	canvas->Pen->Width = 1;
	canvas->Pen->Style = psSolid;
	canvas->Brush->Style = bsClear;
	canvas->Rectangle( Bounds );
	canvas->Pen->Mode  = Mode;
}
//---------------------------------------------------------------------------
void TText::Paint( Gdiplus::Graphics* gCanvas )
{
	TCanvas* canvas = new TCanvas();
	canvas->Handle = gCanvas->GetHDC();
	Paint( canvas );
	delete canvas;
}
//---------------------------------------------------------------------------
void  TText::RePaint( )
{
	 FOwn->Repaint();
}
//---------------------------------------------------------------------------
bool TText::HitTest( int vx, int vy )
{
	 TPoint pt( vx,vy );
	 return FBounds.PtInRect( pt );
}
//---------------------------------------------------------------------------
bool TText::HitTest( int vx, int vy, int& DeltaX, int& DeltaY  )
{
	bool Hit = HitTest( vx, vy );

	if( Hit == true )
	{
		int x1 = FOwn->IndexToViewPortX( FIndex );
		int y1 = FOwn->TickToViewPortY( FTick );
		DeltaX = vx - x1;
		DeltaY = vy - y1;
	}
	else
	{
		DeltaX = 0;
		DeltaY = 0;
	}
	return Hit;
}
//---------------------------------------------------------------------------
bool TText::Intersects(const TRect &r)
{
	return FBounds.IntersectsWith( r );
}
//---------------------------------------------------------------------------
void TText::MoveTo( int IndexX, int TickY )
{
	FIndex = IndexX;
	FTick =  TickY;
}
//---------------------------------------------------------------------------
void TText::SetSize( int Size )
{
	FSize = Size;
	FOwn->Repaint();
}
//---------------------------------------------------------------------------
void TText::SetText( const String& txt )
{
	FCaption = txt;
	FOwn->Repaint();
}
//---------------------------------------------------------------------------
void TText::SetStyle( System::Uitypes::TFontStyles Style )
{
	FStyle = Style;
	FOwn->Repaint();
}
//---------------------------------------------------------------------------
//
// Position Line
//
//---------------------------------------------------------------------------
int TOrderLine::TXT_W_SPACE = 10;
int TOrderLine::TXT_H_SPACE = 5;
//---------------------------------------------------------------------------
TPositionLine::TPositionLine(  TCandleStickChart* Owner )
:TOrderLine( )
,FOwn( Owner )
{

}
//---------------------------------------------------------------------------
void TPositionLine::SetAvgPrice( double Px )
{
	FPrice = Px;
	FPriceTick = FOwn->PriceToTick( Px );
	UpdateText( );
}
//---------------------------------------------------------------------------
void TPositionLine::UpdateText( void )
{
	if( FIsLong == true )
		FPxString.printf( L" ¡¶%d@%.*f", FQty, FOwn->Digit, FPrice );
	else
		FPxString.printf( L" ¡¿%d@%.*f", FQty, FOwn->Digit, FPrice );
}
//---------------------------------------------------------------------------
void TPositionLine::SetPosition( int Pos )
{
	if( Pos >  0 ) ///< long position
	{
		FQty = Pos;
		FIsLong = true;
		FTextColor = clRed;
	}
	else ///< Short Position
	{
		FQty = -1 * Pos;
		FIsLong = false;
		FTextColor = clGreen;
	}
	UpdateText( );
}
//---------------------------------------------------------------------------
void TPositionLine::Paint( TCanvas* canvas )
{
	if( FQty > 0 )
	{
		canvas->Font->Size  = FOwn->FontSize;

		int EndX   = FOwn->GetRightBottomX();
		int AvgPxY = FOwn->TickToViewPortY( FPriceTick );
		int BoxHalfH  = (canvas->TextHeight( FPxString ) + TXT_H_SPACE )/2;
		int LeftX  = EndX - canvas->TextWidth( FPxString ) - TXT_W_SPACE;
		int LeftY  = AvgPxY - BoxHalfH;
		TRect        TextRect = TRect( LeftX, LeftY, EndX, AvgPxY + BoxHalfH );
		TTextFormat  Formats;

		Formats<<tfCenter<<tfVerticalCenter;
		canvas->Pen->Width = 2;
		canvas->Pen->Color = (TColor)RGB( 60, 124, 221 );
		canvas->MoveTo( 0, AvgPxY );
		canvas->LineTo( LeftX , AvgPxY );
		canvas->RoundRect( TextRect, 6, 6 );
		canvas->Brush->Color = (TColor)RGB( 30, 62, 120 );
		canvas->Brush->Style = bsSolid;
		canvas->FloodFill( LeftX + 5, LeftY + 5, canvas->Pen->Color, fsBorder );
		canvas->Font->Color = FTextColor;
		canvas->TextRect( TextRect, FPxString, Formats );
	}
}
//---------------------------------------------------------------------------
TWorkingLine::TWorkingLine(  TCandleStickChart* Owner, double Px, int Qty, bool IsBuy, bool IsStop )
:TOrderLine( )
,FBkColor( (TColor)RGB( 30, 62, 120 ) )
,FOwn( Owner )
,FInit( false )
,FIsStop( IsStop )
,FCxlString( L" ¢® " )
,FOrigPx( 0.0 )
{
	FFormats<<tfCenter<<tfVerticalCenter;
	FIsLong = IsBuy;
	if( FIsLong == true )
	{
		if( FIsStop == true )
			FColor = (TColor)RGB( 200, 0, 0 );
		else
			FColor = clRed;
		FSideString = L" ¶R ";
	}
	else
	{
		if( FIsStop == true )
			FColor = clGreen;
		else
			FColor = (TColor)RGB( 0, 168, 0 );
		FSideString = L" ½æ ";
	}
	SetQty( Qty );
	SetPrice( Px );
}
//---------------------------------------------------------------------------
void TWorkingLine::SetPrice( double Px )
{
	FPrice = Px;
	FPriceTick = FOwn->PriceToTick( Px );
	FPxString.printf( L"%.*f  ", FOwn->Digit, FPrice );
	FDirty = true;
}
//---------------------------------------------------------------------------
void TWorkingLine::SetMovingPrice( double Px )
{
	if( FOrigPx == 0.0 )
		FOrigPx = FPrice;
	SetPrice( Px );
}
//---------------------------------------------------------------------------
void TWorkingLine::StopMoving( void )
{
	SetPrice( FOrigPx );
	FOrigPx = 0.0;
}
//---------------------------------------------------------------------------
void TWorkingLine::SetQty( int Qty )
{
	FQty = Qty;
	FQtyString.printf( L"%d ", FQty );
	FDirty = true;
}
//---------------------------------------------------------------------------
void TWorkingLine::UpdateSize( TCanvas* canvas )
{
	canvas->Font->Size  = 10;
	if( FInit == false )
	{
		FInit = true;
		FBSWidth = canvas->TextWidth( FSideString );
		FCxWidth = canvas->TextWidth( FCxlString );
		FHalfHeight = (canvas->TextHeight( FCxWidth + FBSWidth ) + TXT_H_SPACE )/2;
	}
	if( FDirty == true )
	{
		FDirty = false;
		FQtyWidth = canvas->TextWidth( FQtyString );
		FPxWidth = canvas->TextWidth( FPxString );
	}
}
//---------------------------------------------------------------------------
int TWorkingLine::TextWidth()
{
	return  FQtyWidth +  FBSWidth +  FPxWidth +  FCxWidth;
}
//---------------------------------------------------------------------------
void TWorkingLine::DrawQty( TCanvas* canvas, TRect& QtyRect )
{
	canvas->Font->Color = clWhite;
	canvas->TextRect( QtyRect, FQtyString, FFormats );
}
//---------------------------------------------------------------------------
void TWorkingLine::DrawSide( TCanvas* canvas, TRect& BSRect  )
{
	canvas->Brush->Color = FBkColor;
	canvas->FillRect( BSRect );
	canvas->Font->Color = FColor;
	canvas->TextRect( BSRect, FSideString, FFormats );
}
//---------------------------------------------------------------------------
void TWorkingLine::DrawPrice( TCanvas* canvas, TRect& PxRect, String& PxStr )
{
	canvas->Brush->Color = FBkColor;
	canvas->FillRect( PxRect );
	canvas->Font->Color = FColor;
	canvas->TextRect( PxRect, PxStr, FFormats );
}
//---------------------------------------------------------------------------
void TWorkingLine::DrawCancel( TCanvas* canvas, TRect& CxRect )
{
	canvas->Brush->Style = bsClear;
	canvas->Font->Color = clWhite;
	canvas->TextRect( CxRect, FCxlString, FFormats );
}
//---------------------------------------------------------------------------
void TWorkingLine::DrawBK( TCanvas* canvas, TRect& BoundsRect, int AvgPxY )
{
	canvas->Pen->Width   = 1;
	canvas->Pen->Color   = FColor;
	canvas->Pen->Mode    = pmCopy;
	if( AvgPxY != 0 )
	{
		canvas->Brush->Style = bsClear;
		if( FIsStop == true )
			canvas->Pen->Style = psDash;
		else
			canvas->Pen->Style = psSolid;
		canvas->MoveTo( 0, AvgPxY );
		canvas->LineTo( BoundsRect.Left , AvgPxY );
	}
	canvas->Brush->Color = FColor;
	canvas->Brush->Style = bsSolid;
	canvas->RoundRect( BoundsRect, 6, 6 );
}
//---------------------------------------------------------------------------
void TWorkingLine::Paint( TCanvas* canvas )
{
	if( FQty > 0 )
	{
		UpdateSize( canvas );

		int AvgPxY    = FOwn->TickToViewPortY( FPriceTick );
		int RightX    = FOwn->GetRightBottomX();
		int LeftX     = RightX - TextWidth() - TXT_W_SPACE;
		int RightY    = AvgPxY + FHalfHeight;
		int LeftY     = AvgPxY - FHalfHeight;
		TRect DrawRect( LeftX, LeftY, LeftX + FQtyWidth + TXT_W_SPACE, RightY );

		FBounds    = TRect( LeftX, LeftY, RightX, RightY );
		DrawBK( canvas, FBounds, AvgPxY );
		DrawQty( canvas, DrawRect );

		DrawRect.Top+=2;
		DrawRect.Bottom -=2;
		DrawRect.Left  = DrawRect.Right;
		DrawRect.Right = DrawRect.Left + FBSWidth;
		DrawSide( canvas, DrawRect );

		DrawRect.Left  = DrawRect.Right;
		DrawRect.Right = DrawRect.Left + FPxWidth;
		DrawPrice( canvas, DrawRect, FPxString );

		FCxlBounds = TRect( DrawRect.Right, LeftY, DrawRect.Right + FCxWidth, RightY );
		DrawCancel( canvas, FCxlBounds );
	}
}
//---------------------------------------------------------------------------
HitResult TWorkingLine::HitTest( int vx, int vy )
{
	TPoint P( vx,vy );

	if( FBounds.PtInRect( P ) == true )
	{
		if( FCxlBounds.PtInRect( P ) == true )
			return olDelete;
		return olMove;
	}
	return olNone;
}
//---------------------------------------------------------------------------

