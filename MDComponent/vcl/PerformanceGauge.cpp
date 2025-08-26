//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "PerformanceGauge.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TPerformanceGauge *)
{
	new TPerformanceGauge(NULL);
}
//---------------------------------------------------------------------------
__fastcall TPerformanceGauge::TPerformanceGauge(TComponent* Owner)
: TCustomControl(Owner),
  FMaxValue( 1000 ),
  FInitMaxValue( 1000 ),
  FWarningValue( 0 ),
  FPixelSec( 2 ),  ///< Move 2 pixels per second.
  FTopBorder( 5 ),
  FLeftBorder( 25 ),
  FGrids( 5 ),
  FScale( 100 ),
  FDecimal( 0 ),
  FDrawVertical( true ),
  FDrawHorizontal( true ),
  FFill( false ),
  FDrawSecondLine( false ),
  FBKColor( clBlack ),
  FGridColor( clGreen ),
  FLineColor( clYellow )
{
	FDataList = new TList();
	FData2List = new TList();
	FBufferBmp = new Graphics::TBitmap();
}
//---------------------------------------------------------------------------
void __fastcall TPerformanceGauge::PaintGrid( Graphics::TBitmap* BufferBmp )
{
	int   Step = (Height - FTopBorder)/FGrids;
	int   YPos,XPos = FLeftBorder;
	float ValueStep = (float)(FMaxValue/FScale)/(float)FGrids;
	AnsiString ValueStr;

	BufferBmp->Canvas->Pen->Color  = FGridColor;//(TColor)RGB( 0, 64, 0 );
	if( FDrawVertical == true )
	{
		while( true )
		{
			XPos += (FPixelSec*10);
			if( XPos >= Width )
				break;
			BufferBmp->Canvas->MoveTo( XPos, 0 );
			BufferBmp->Canvas->LineTo( XPos, Height );
		};
	}
	BufferBmp->Canvas->Pen->Color  = FGridColor;
	BufferBmp->Canvas->Font->Color = FGridColor;
	BufferBmp->Canvas->MoveTo( FLeftBorder, 0 );
	BufferBmp->Canvas->LineTo( FLeftBorder, Height );
	BufferBmp->Canvas->Brush->Style = bsClear;
	if( FDrawHorizontal == false )
	{
		BufferBmp->Canvas->MoveTo( FLeftBorder, FTopBorder );
		BufferBmp->Canvas->LineTo( Width, FTopBorder );
	}
	for( int i = 0; i < FGrids; i ++ )
	{
		YPos = FTopBorder + Step* i;
		if( FDecimal == 0 )
			ValueStr.printf( "%d", (int)(ValueStep*(float)(FGrids-i)));
		else
			ValueStr.printf("%3.*f", FDecimal, ValueStep*(float)(FGrids-i));
		if( FDrawHorizontal == true )
		{
			BufferBmp->Canvas->MoveTo( FLeftBorder, YPos );
			BufferBmp->Canvas->LineTo( Width, YPos );
		}
		BufferBmp->Canvas->TextOut( 0, YPos, ValueStr );
	}
}
//---------------------------------------------------------------------------
void __fastcall TPerformanceGauge::PaintWarningLine( Graphics::TBitmap* BufferBmp )
{
	if( FWarningValue > 0 )
	{
		float Scale = (float)( Height - FTopBorder ) / (float) FMaxValue;
		int    YPos = Height - (((int)FWarningValue )*Scale);

		BufferBmp->Canvas->Pen->Color = clRed;
		BufferBmp->Canvas->MoveTo( FLeftBorder, YPos );
		BufferBmp->Canvas->LineTo( Width, YPos);
	}
}
//------------------------------------------------------------------------------
TColor  __fastcall TPerformanceGauge::DarkerColor( TColor OrigCol )
{
	int R = GetRValue( OrigCol )>> 1;
	int G = GetGValue( OrigCol )>> 1;
	int B = GetBValue( OrigCol )>> 1;
	return (TColor)RGB (R, G, B );
}
//---------------------------------------------------------------------------
void __fastcall TPerformanceGauge::PaintData( Graphics::TBitmap* BufferBmp )
{
	float Scale = (float)( Height - FTopBorder ) / (float) FMaxValue;
	register int YPos;

	///< Draw line 1
	BufferBmp->Canvas->Pen->Color = FLineColor;
	for( register int i = 0; i < FDataList->Count; i++ )
	{
		if( i >= POINTS_MAX - 3 )
			break;
		YPos = ((int)FDataList->Items[ i ])*Scale;
		FPoints[i] = TPoint( FLeftBorder + i*FPixelSec, Height - YPos );
	}
	if( FFill == true )
	{
		FPoints[ FDataList->Count ] = TPoint( Width + 100 , Height  );
		FPoints[ FDataList->Count+1 ] = TPoint( FPoints[0].x, Height  );
		BufferBmp->Canvas->Brush->Style = bsSolid;
		BufferBmp->Canvas->Brush->Color = DarkerColor( FLineColor );
		BufferBmp->Canvas->Polygon( FPoints, FDataList->Count + 1 );
	}
	else
		BufferBmp->Canvas->Polyline( FPoints, FDataList->Count - 1 );
	///< Draw line 2
	if( FDrawSecondLine == true )
	{
		BufferBmp->Canvas->Pen->Color = FLine2Color;
		for( register int i = 0; i < FData2List->Count; i++ )
		{
			if( i >= POINTS_MAX - 3 )
				break;
			YPos = ((int)FData2List->Items[ i ])*Scale;
			FPoints[i] = TPoint( FLeftBorder + i*FPixelSec, Height - YPos );
		}
		if( FFill == true )
		{
			FPoints[ FDataList->Count     ] = TPoint( Width + 100, Height );
			FPoints[ FDataList->Count+1 ] = TPoint( FPoints[0].x, Height  );
			BufferBmp->Canvas->Brush->Style = bsSolid;
			BufferBmp->Canvas->Brush->Color = DarkerColor( FLine2Color );
			BufferBmp->Canvas->Polygon( FPoints, FDataList->Count + 1 );
		}
		else
			BufferBmp->Canvas->Polyline( FPoints, FData2List->Count - 1 );
	}
}
//---------------------------------------------------------------------------
void __fastcall TPerformanceGauge::CheckLeftBorder( void )
{
	String MaxValueStr;
	int NewLeftBorder;

	if( FDecimal == 0 )
		MaxValueStr.printf( L"%d", (int)(FMaxValue/FScale));
	else
		MaxValueStr.printf( L"%3.*f", FDecimal, (int)(FMaxValue/FScale));
	NewLeftBorder = Canvas->TextWidth( MaxValueStr ) + 2;
	if( NewLeftBorder > 25 )
		FLeftBorder = NewLeftBorder;
}
//---------------------------------------------------------------------------
void __fastcall TPerformanceGauge::Loaded( void )
{
	int DataCount = ( Width - FLeftBorder ) / FPixelSec;

	FMaxValue = FInitMaxValue;
	FDrawSecondLine = false;
	for( register int i = 0; i < DataCount; i++ )
	{
		 FDataList->Add( (void*) 0 ); ///< feed data 0 to filled the data list.
		 FData2List->Add( (void*) 0 ); ///< feed data 0 to filled the data list.
	}
	CheckLeftBorder();
}
//---------------------------------------------------------------------------
void __fastcall TPerformanceGauge::Paint( void )
{
	TRect PaintRect = Rect( 0, 0, Width, Height );

	if( FBufferBmp->Width != Width || FBufferBmp->Height != Height )
	{
		FBufferBmp->Width = Width;
		FBufferBmp->Height = Height;
	}
	FBufferBmp->Canvas->Brush->Color = FBKColor;
	FBufferBmp->Canvas->FillRect( PaintRect );
	PaintData( FBufferBmp );
	PaintGrid( FBufferBmp );
	PaintWarningLine( FBufferBmp );
	Canvas->Draw( 0, 0, FBufferBmp );
}
//---------------------------------------------------------------------------
void __fastcall TPerformanceGauge::Clear( void )
{
	int DataCount = ( Width - FLeftBorder ) / FPixelSec;

	FMaxValue = FInitMaxValue;
	FDataList->Clear();
	FData2List->Clear();
	FDrawSecondLine = false;
	for( register int i = 0; i < DataCount; i++ )
	{
		 FDataList->Add( (void*) 0 );
		 FData2List->Add( (void*) 0 );
	}
	Paint();
}
//---------------------------------------------------------------------------
void __fastcall TPerformanceGauge::KeepInList( TList* DataList, int DataCount )
{
	while( true )
	{
		if( DataList->Count <=  DataCount )
			break;
		DataList->Delete( 0 );
	}
}
//---------------------------------------------------------------------------
void __fastcall TPerformanceGauge::CheckMax( TList* DataList )
{
	register int CheckVal;
	bool Modify = false;
	for( register int i = 0;i < DataList->Count; i++ )
	{
		 if( ( CheckVal = (int)DataList->Items[ i ] ) > FMaxValue )
		 {
			 FMaxValue = CheckVal;
			 Modify = true;
		 }
	}
	if( Modify == true )
		CheckLeftBorder();
}
//---------------------------------------------------------------------------
void __fastcall TPerformanceGauge::Add( int Value )
{
	int DataCount = ( Width - FLeftBorder ) / FPixelSec;
	int CheckVal;

	KeepInList( FDataList, DataCount );
	FDataList->Add( (void*) Value );
	///< Find the Max value.
	FMaxValue = FInitMaxValue;
	CheckMax( FDataList );
	Paint();
}
//---------------------------------------------------------------------------
void __fastcall TPerformanceGauge::Add2( int Value1,  int Value2 )
{
	int DataCount = ( Width - FLeftBorder ) / FPixelSec;
	int CheckVal;

	FDrawSecondLine = true;
	KeepInList( FDataList, DataCount );
	FDataList->Add( (void*) Value1 );

	KeepInList( FData2List, DataCount );
	FData2List->Add( (void*) Value2 );
	///< Find the Max value.
	FMaxValue = FInitMaxValue;
	CheckMax( FDataList );
	CheckMax( FData2List );
	Paint();
}
//---------------------------------------------------------------------------
bool __fastcall TPerformanceGauge::DataExists( int TimeSec )
{
	int SecCount = 0;

	for( int i = FDataList->Count-1; i >= 0 ; i-- )
	{
		 if( (int)FDataList->Items[ i ] > 0 )
			 return true;
		 SecCount++;
		 if( SecCount >= TimeSec )
			 break;
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TPerformanceGauge::WndProc( Messages::TMessage &Message )
{
	if( Message.Msg == WM_ERASEBKGND )
	{
		Message.Result = 1;
		return;
	}
	TCustomControl::WndProc( Message );
}
//---------------------------------------------------------------------------
namespace Performancegauge
{
    void __fastcall PACKAGE Register()
    {
         TComponentClass classes[1] = {__classid(TPerformanceGauge)};
         RegisterComponents("MD", classes, 0);
    }
}
//---------------------------------------------------------------------------
