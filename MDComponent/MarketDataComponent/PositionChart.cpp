//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "PositionChart.h"
#pragma package(smart_init)
const LEFT_BORDER = 10;
const TOP_BORDER = 10;
const RIGHT_BORDER = 10;
const BOTTOM_BORDER = 10;
//---------------------------------------------------------------------------
ChartRecord::ChartRecord( const UFC::AnsiString& Symbol, Market market, double StrikePx, CallPutCode CallPut, nsOrderMessageDefine::SideEnum Side, double ContracMultiplier )
:FSymbol( Symbol )
,FMarket( market )
,FPrice( 0 )
,FQty(0)
,FBestBuyPx(0)
,FBestSellPx(0)
,FSumPxQty(0)
,FSide( Side )
,FContractMultiplier( ContracMultiplier )
,FStrikePrice( StrikePx )
,FBEPPx( StrikePx )
,FCPCode( CallPut )
{}
//---------------------------------------------------------------------------
void __fastcall ChartRecord::AddPosition( double Px, int Qty )
{
	if( Qty > 0 )
	{
		FSumPxQty += Px * Qty;
		FQty += Qty;
		FPrice = FSumPxQty / Qty;

		if( FCPCode == cpCall )
			FBEPPx = FStrikePrice + FSumPxQty / Qty;
		else
			FBEPPx = FStrikePrice - FSumPxQty / Qty;
	}
}
//---------------------------------------------------------------------------
void __fastcall ChartRecord::SetPx( double Px )
{
	FPrice = Px;
	if( FCPCode == cpCall )
		FBEPPx = FStrikePrice + FPrice;
	else
		FBEPPx = FStrikePrice - FPrice;
}
//---------------------------------------------------------------------------
int  PxPointCompare::Compare( void* elem1, void* elem2 )
{
	return (*(PxPoint*)elem1).GetPx() - (*(PxPoint*)elem2).GetPx();
}
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TPositionChart *)
{
	new TPositionChart(NULL);
}
//---------------------------------------------------------------------------
__fastcall TPositionChart::TPositionChart(TComponent* Owner)
:TCustomControl(Owner)
,FBuffer( new Graphics::TBitmap() )
,FMarketDataStore( NULL )
,FMaxStrikePx( 9200 )
,FMinStrikePx( 6600 )
,FMaxPx( 10000 )
,FMinPx( -10000 )
,FPosList( new PxPointCompare() )
{
	DoubleBuffered = false;
	Constraints->MinHeight = 100;
	Constraints->MinWidth = 250;

	FPxWidth = FBuffer->Canvas->TextWidth( "-99999" );
	Clear();
	Resize();
}
//---------------------------------------------------------------------------
__fastcall TPositionChart::~TPositionChart( void )
{
	Clear();
	delete FBuffer;
}
//---------------------------------------------------------------------------
void __fastcall TPositionChart::Clear( void )
{
	for( register int i = 0; i < FTable.ItemCount(); i++ )
	{
		ChartRecord* Rec = FTable.GetItem(i);
		FMarketDataStore->Unsubscribe( "TAIFEX", Rec->GetSymbol(), this );
		if( Rec != NULL )
			delete Rec;
	}

	for( register int i = 0; i < FPosSummary.ItemCount(); i++ )
	{
		PxPoint* Point = FPosSummary.GetItem(i);
		if( Point != NULL )
			delete Point;
	}

	FTable.Clear();
	FPosSummary.Clear();
	FPosList.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TPositionChart::Paint( void )
{
	TRect PaintRect( 0, 0, Width, Height );
	bool  IsFocus = Focused();

	DrawBackground( FBuffer->Canvas, PaintRect, IsFocus );
	DrawCoordinate( FBuffer->Canvas );
	DrawPosition( FBuffer->Canvas );

	Canvas->Draw( 0, 0, FBuffer );
}
//---------------------------------------------------------------------------
void __fastcall TPositionChart::PxToCoordinate( double Px, double Profit, int& X, int & Y )
{
	X = LEFT_BORDER + FPxWidth + ( Px - FMinStrikePx )/( FMaxStrikePx - FMinStrikePx ) * ( Width - LEFT_BORDER - RIGHT_BORDER );
	Y = TOP_BORDER + ( FMaxPx - Profit )/( FMaxPx - FMinPx ) * ( Height - TOP_BORDER - BOTTOM_BORDER );
}
//---------------------------------------------------------------------------
void __fastcall TPositionChart::DrawPosition( TCanvas* canvas )
{
	canvas->Pen->Color = clWhite;

	FMaxPx = 1000;
	FMinPx = -1000;
	FBuffer->Canvas->Font->Size = 8;
	FBuffer->Canvas->Font->Color = clWhite;
	FBuffer->Canvas->Brush->Style = bsClear;
	for( register int i = 0; i < FPosList.ItemCount(); i++ )
	{
		PxPoint* Pos = (PxPoint*)FPosList.GetItem(i);
		if( Pos->GetProfit() > FMaxPx )
			FMaxPx = (( (int)( Pos->GetProfit() * 1.3 ) / 1000 ) + 1 ) * 1000;
		if( Pos->GetProfit() < FMinPx )
			FMinPx = (( (int)( Pos->GetProfit() * 1.3 ) / 1000 ) - 1 ) * 1000;
	}

	if( FPosSummary.ItemCount() )
	{
		HRGN DrawRgn = CreateRectRgn( LEFT_BORDER, TOP_BORDER, Width - RIGHT_BORDER, Height - BOTTOM_BORDER );
		SelectClipRgn( canvas->Handle, DrawRgn );

		int X, Y;
		int ZeroHeight = FMaxPx / ( FMaxPx - FMinPx ) * ( Height - TOP_BORDER - BOTTOM_BORDER ) + TOP_BORDER;
		String      Msg;
		TTextFormat Formats;

		Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		canvas->MoveTo( FPxWidth + LEFT_BORDER, ZeroHeight );
		canvas->LineTo( Width - RIGHT_BORDER - LEFT_BORDER, ZeroHeight );
		TPoint  HTriPoint[4] = { TPoint( Width - RIGHT_BORDER , ZeroHeight ),
								 TPoint( Width - RIGHT_BORDER - 10 , ZeroHeight + 5 ),
								 TPoint( Width - RIGHT_BORDER - 10 , ZeroHeight - 5 ),
								 TPoint( Width - RIGHT_BORDER , ZeroHeight ) };
		canvas->Polygon( HTriPoint, 3 );

		PxPoint* Pos = (PxPoint*)FPosList.GetItem(0);
		PxToCoordinate( Pos->GetPx(), Pos->GetProfit(), X, Y );
		canvas->MoveTo( X, Y );
		Msg.printf( L"(%.0f, %.0f)", Pos->GetPx(), Pos->GetProfit() );

		int MsgWidth  = canvas->TextWidth( Msg.c_str() );
		int MsgHeight = canvas->TextHeight( Msg.c_str() );
		int y;
		if( Y > 0 )
			y = Y - MsgHeight;
		else
			y = Y + MsgHeight;
		TRect TextRect = TRect( X, y, X + MsgWidth, y + MsgHeight );
		canvas->TextRect( TextRect, Msg, Formats );
		for( register int i = 1; i < FPosSummary.ItemCount(); i++ )
		{
			Pos = (PxPoint*)FPosList.GetItem(i);
			PxToCoordinate( Pos->GetPx(), Pos->GetProfit(), X, Y );
			canvas->LineTo( X, Y );
			if( (int)Pos->GetPx() % 100 == 0)
			{
				Msg.printf(L"(%.0f, %.0f)", Pos->GetPx(), Pos->GetProfit() );
				MsgWidth = canvas->TextWidth( Msg.c_str() );
				MsgHeight = canvas->TextHeight( Msg.c_str() );
				if( Y > 0 )
					y = Y - MsgHeight;
				else
					y = Y + MsgHeight;
				TRect TextRect = TRect( X, y, X + MsgWidth, y + MsgHeight );
				canvas->TextRect( TextRect, Msg, Formats );
			}
		}

		SelectClipRgn( canvas->Handle, NULL );
		DeleteObject( DrawRgn );
	}
}
//---------------------------------------------------------------------------
void __fastcall TPositionChart::DrawCoordinate( TCanvas* canvas )
{
	canvas->Pen->Color = clWhite;
	canvas->Brush->Color = clWhite;

	canvas->MoveTo( LEFT_BORDER + FPxWidth, TOP_BORDER );
	canvas->LineTo( LEFT_BORDER + FPxWidth, Height - BOTTOM_BORDER );
	TPoint  UVTriPoint[4] = { TPoint( LEFT_BORDER + FPxWidth , TOP_BORDER ),
							TPoint( LEFT_BORDER + FPxWidth - 5 , TOP_BORDER + 10 ),
							TPoint( LEFT_BORDER + FPxWidth + 5 , TOP_BORDER + 10 ),
							TPoint( LEFT_BORDER + FPxWidth , TOP_BORDER )
						  };
	canvas->Polygon( UVTriPoint, 3 );

	canvas->MoveTo( LEFT_BORDER + FPxWidth, BOTTOM_BORDER );
	TPoint  DVTriPoint[4] = { TPoint( LEFT_BORDER + FPxWidth , Height - BOTTOM_BORDER ),
							TPoint( LEFT_BORDER + FPxWidth - 5 , Height - BOTTOM_BORDER - 10 ),
							TPoint( LEFT_BORDER + FPxWidth + 5 , Height - BOTTOM_BORDER - 10 ),
							TPoint( LEFT_BORDER + FPxWidth , Height - BOTTOM_BORDER )
						  };
	canvas->Polygon( DVTriPoint, 3 );
}
//---------------------------------------------------------------------------
void __fastcall TPositionChart::DrawBackground( TCanvas* canvas, TRect& BufferRect, bool IsFocus )
{
	if( IsFocus )
		canvas->Brush->Color = Color;
	else
		canvas->Brush->Color = (TColor)RGB( GetRValue( Color )/2, GetGValue( Color )/2, GetBValue( Color )/2 );
	canvas->FillRect( BufferRect );
}
//---------------------------------------------------------------------------
void __fastcall TPositionChart::Resize( void )
{
	if( FBuffer->Width != Width )
		FBuffer->Width = Width;
	if( FBuffer->Height != Height )
		FBuffer->Height = Height;
}
//---------------------------------------------------------------------------
void __fastcall TPositionChart::SetMarketDataStore( TCMarketDataStore* Store )
{
	FMarketDataStore = Store;
}
//---------------------------------------------------------------------------
void __fastcall TPositionChart::AddSymbol( String Exchange, String Symbol, nsOrderMessageDefine::SideEnum Side, int Qty )
{
	if( FMarketDataStore != NULL )
	{
		ChartRecord*    Record;
		AnsiString      AnsiExchange = Exchange;
		AnsiString      AnsiSym = Symbol;
		UFC::AnsiString SymStr( AnsiSym.c_str() );

		if( FSymbol.Compare( SymStr.SubString(0, 2 ).c_str() ) )
		{
			Clear();
			FSymbol = SymStr.SubString( 0, 2 ).c_str();
		}

		if( ( Record = FTable.GetObjectByKey( SymStr ) ) == NULL )
		{
			BasicInformation* Info = FMarketDataStore->GetBasicInformation( Exchange, SymStr.c_str(), false );
			TTickList* List = (TTickList*)FMarketDataStore->GetTickList( Exchange, SymStr.c_str() );
			double CM = List->GetContractMultiplier();

			if( Info != NULL )
			{
				ChartRecord* Record = new ChartRecord( SymStr, Info->GetMarket(), Info->GetStrikePrice(), Info->GetCallPut(), Side, CM );
				Record->AddPosition( Info->GetRefPrice(), Qty );
				FTable.Add( SymStr, Record );
				if( FTable.ItemCount() == 1 && Info->GetMarket() == mTWOptions )
				{
					FMaxStrikePx = FMarketDataStore->GetMaxStrikePx( "TAIFEX", SymStr.SubString( 0, 3 ).c_str(), Info->GetMaturityDate().SubString( 0, 6 ).c_str() );
					FMinStrikePx = FMarketDataStore->GetMinStrikePx( "TAIFEX", SymStr.SubString( 0, 3 ).c_str(), Info->GetMaturityDate().SubString( 0, 6 ).c_str() );
				}
			}
		}
		else
		{
			Record->AddPosition( Record->GetPrice(), Qty );
		}

		CalculatePosition();
		Paint();
		FMarketDataStore->Subscribe( AnsiExchange.c_str(), SymStr, this );
	}
}
//---------------------------------------------------------------------------
void __fastcall TPositionChart::CalculatePosition( void )
{

	for( register int i = 0; i < FPosSummary.ItemCount(); i++ )
	{
		PxPoint* Point = FPosSummary.GetItem(i);
		if( Point != NULL )
			delete Point;
	}

	FPosSummary.Clear();
	FPosList.Clear();
	double MaxStkPx;
	double MinStkPx;
	for( register int i = 0; i < FTable.ItemCount(); i++ )
	{
		ChartRecord* Record = FTable.GetItem(i);

		if( Record != NULL )
		{
//			nsOrderMessageDefine::SideEnum Side = Record->GetSide();
//			CallPutCode CallPut = Record->GetCallPut();
			if( Record->GetMarket() == mTWOptions )
			{
				double BEPPx;// = Record->GetBEPPx();
				double StkPx = Record->GetStrikePx();
//				double cost = Record->GetPrice() * Record->GetQty();
//				double CM = Record->GetContractMultiplier();
				int INT_StkPx = UFC::DoubleToInt( StkPx, 3 );
				int INT_BEPPx =	UFC::DoubleToInt( BEPPx, 3 );
				PxPoint* CalPos;

				if( ( CalPos = FPosSummary.GetObjectByKey( INT_StkPx ) ) == NULL )
				{
					CalPos = new PxPoint( StkPx, 0 );
					FPosSummary.Add( INT_StkPx, CalPos );
					FPosList.Add( CalPos );
				}

				if( ( CalPos = FPosSummary.GetObjectByKey( INT_BEPPx ) ) == NULL )
				{
					CalPos = new PxPoint( BEPPx, 0 );
					FPosSummary.Add( INT_BEPPx, CalPos );
					FPosList.Add( CalPos );
				}

				if( i == 0 )
				{
					MaxStkPx = Record->GetStrikePx();
					MinStkPx = Record->GetStrikePx();
				}
				else
				{
					if( Record->GetStrikePx() > MaxStkPx )
						MaxStkPx = Record->GetStrikePx();
					if( Record->GetStrikePx() < MinStkPx )
						MinStkPx =  Record->GetStrikePx();
				}
			}
		}
	}

	if( FTable.ItemCount() > 0 )
	{
		FMaxStrikePx = MaxStkPx + 600;
		FMinStrikePx = MinStkPx - 600;
	}

	PxPoint* MaxPos = new PxPoint( FMaxStrikePx, 0 );
	PxPoint* MinPos = new PxPoint( FMinStrikePx, 0 );
	int INT_CO_MAX = UFC::DoubleToInt( FMaxStrikePx, 3 );
	int INT_CO_MIN = UFC::DoubleToInt( FMinStrikePx, 3 );
	FPosSummary.Add( INT_CO_MAX, MaxPos );
	FPosSummary.Add( INT_CO_MIN, MinPos );
	FPosList.Add( MaxPos );
	FPosList.Add( MinPos );

	for( register int i = 0; i < FPosList.ItemCount(); i++ )
	{
		PxPoint* CalPos = (PxPoint*)FPosList.GetItem(i);
		for( register int j = 0; j < FTable.ItemCount(); j++ )
		{
			ChartRecord* Record = FTable.GetItem(j);
			nsOrderMessageDefine::SideEnum Side = Record->GetSide();
			CallPutCode CallPut = Record->GetCallPut();
			if( Record->GetMarket() == mTWOptions )
			{
				double cost;
				//double BEPPx = Record->GetBEPPx();
				double StkPx = Record->GetStrikePx();
				if( Record->GetSide() == nsOrderMessageDefine::sBuy )
					cost = Record->GetBestSellPx();
				else
					cost = Record->GetBestBuyPx();
				double CM = Record->GetContractMultiplier();
				CalPointProfit( Side, CallPut, StkPx, cost, CM, CalPos );
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TPositionChart::CalPointProfit( nsOrderMessageDefine::SideEnum side, CallPutCode CallPut, double StkPx, double cost, double CM, PxPoint* point )
{
	double Index = point->GetPx();
	double Profit = 0;
	if( side == nsOrderMessageDefine::sBuy )
	{
		if( CallPut == cpCall )
		{
			if( Index <= StkPx )
				Profit = cost * CM * -1;
			else
				Profit = ( Index - StkPx ) * CM - cost * CM;
		}
		else if( CallPut == cpPut )
		{
			if( Index <= StkPx )
				Profit = ( StkPx - Index ) * CM - cost * CM;
			else
				Profit = cost * CM * -1;
		}
		else  ///< Futures
		{
			Profit = ( Index - cost ) * CM;
		}
	}
	else
	{
		if( CallPut == cpCall )
		{
			if( Index <= StkPx )
				Profit = cost * CM;
			else
				Profit = ( Index - StkPx ) * CM * -1 + cost * CM;
		}
		else if( CallPut == cpPut )
		{
			if( Index <= StkPx )
				Profit = ( StkPx - Index ) * CM * -1 + cost * CM;
			else
				Profit = cost * CM;
		}
		else  ///< Futures
		{
			Profit = ( cost - Index ) * CM;
		}
	}
	point->AddProfit( Profit );
}
//---------------------------------------------------------------------------
void __fastcall TPositionChart::WndProc( TMessage &Msg )
{
	if( Msg.Msg == WM_ERASEBKGND )
	{
		 Msg.Result = 1;
		 return;
	}
	else
	{
		inherited::WndProc( Msg );
	}
}
//---------------------------------------------------------------------------
void TPositionChart::OnMarketDataUpdate( MarketDataMessage* Msg )
{
	ChartRecord* Record;
	if( ( Record = FTable.GetObjectByKey( Msg->GetSymbol() ) ) != NULL )
	{
		Record->SetPx( Msg->GetTradePrice() );
		Record->SetBestBuyPx( Msg->GetBuyPrice1() );
		Record->SetBestSellPx( Msg->GetSellPrice1() );
	}
	CalculatePosition();
	Paint();
}
//---------------------------------------------------------------------------
void TPositionChart::OnMarketDataUpdate( MatchInfo* Msg )
{
	ChartRecord* Record;
	if( ( Record = FTable.GetObjectByKey( Msg->GetSymbol() ) ) != NULL )
	{
		Record->SetPx( Msg->GetMatchPrice() );
	}
	CalculatePosition();
	Paint();
}
//---------------------------------------------------------------------------
void TPositionChart::OnMarketDataUpdate( OrderBookData* Msg )
{
	ChartRecord* Record;
	if( ( Record = FTable.GetObjectByKey( Msg->GetSymbol() ) ) != NULL )
	{
		Record->SetBestBuyPx( Msg->GetBuyPrice1() );
		Record->SetBestSellPx( Msg->GetSellPrice1() );
	}
}
//---------------------------------------------------------------------------
void TPositionChart::OnMarketDataUpdate( BasicData* Msg ) {}
//---------------------------------------------------------------------------
void TPositionChart::OnMarketDataUpdate( TotalMatch* Msg ){}
//---------------------------------------------------------------------------
void TPositionChart::OnMarketDataUpdate( DayHighLowPrice* Msg ) {}
//---------------------------------------------------------------------------
void TPositionChart::OnMarketDataUpdate( OpeningInfo* Msg ) {}
//---------------------------------------------------------------------------
void TPositionChart::OnMarketDataUpdate( ClosingMarketData* Msg ) {}
//---------------------------------------------------------------------------
void TPositionChart::OnMarketDataUpdate( UnderlyingIndexInfo* Msg ) {}
//---------------------------------------------------------------------------
void TPositionChart::OnMarketDataUpdate( SumOfOrderInfo* Msg ) {}
//---------------------------------------------------------------------------
Classes::TStringList* __fastcall TPositionChart::GetSymbols( void ) { return NULL; }
//---------------------------------------------------------------------------
namespace Positionchart
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TPositionChart)};
		RegisterComponents(L"Speedy", classes, 0);
	}
}
//---------------------------------------------------------------------------
