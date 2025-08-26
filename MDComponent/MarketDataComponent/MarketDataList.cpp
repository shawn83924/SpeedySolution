//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "MarketDataList.h"
#include "MDComponentStrings.hpp"
#include "math.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
String ListViewHeaderString[20];/* = { "商品名稱", "委買價", "委賣價", "成交價", "單量", "委買量", "委賣量", "總量", "總委買量", "總委賣量",
									"開盤價", "最高", "最低", "未平倉", "結算價", "昨收", "漲跌", "漲跌幅", "成交時間" };*/
//---------------------------------------------------------------------------
int __stdcall SymbolSort(long lParam1, long lParam2, long ParamSort);
UFC::Int32 TMarketDataList::BaseYear;
UFC::Int32 TMarketDataList::Remainder;
//---------------------------------------------------------------------------
TMDListRecord::TMDListRecord( const String& Exchange, const String& Symbol, bool TradeFlag )
:FExchange( Exchange )
,FSymbol( Symbol )
,FTradeFlag( TradeFlag )
,FBidPx(0)
,FAskPx(0)
,FBidQty(0)
,FAskQty(0)
,FTradePx(0)
,FTradeQty(0)
,FOpeningPx(0)
,FClosingPx(0)
,FSettelmentPx(0)
,FTradeVolume(0)
,FTotalBuy(0)
,FTotalSell(0)
,FDayHighPx(0)
,FDayLowPx(0)
,FOpenInterest(0)
,FBase(0)
,FDigit(0)
,FRefPx(0)
,FBullPx(0)
,FBearPx(0)
,FTradeTime( "--" )
{}
//---------------------------------------------------------------------------
bool TMDListRecord::IsLimitUp( void )
{
	if( FBidPx == FBullPx )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool TMDListRecord::IsLimitDown( void )
{
	if( FAskPx == FBearPx )
		return true;
	return false;
}
//---------------------------------------------------------------------------
void TMDListRecord::SetTickList( const TTickList* Ticklist )
{
	FTicks = (TTickList*)Ticklist;

	FLowPx = FTicks->GetTickStepFromIndex( FTicks->ItemCount() -1 );
	FHiPx  = FTicks->GetBullPx() * 10.0;
}
//---------------------------------------------------------------------------
void TMDListRecord::SetTradePx( double Px )
{
	if( (Px > FLowPx && Px < FHiPx ) || FTradeFlag == false )
	{
		FTradePx = Px;
		if( FTradePx >= FAskPx )
			FPriceRise = true;
		else
			FPriceRise = false;
	}
	else
		FTradePx = 0;
}
//---------------------------------------------------------------------------
void TMDListRecord::SetTradeQty( int Qty )
{
	if( (Qty > 0 && Qty < FTradeVolume) || FTradeFlag == false  )
		FTradeQty = Qty;
	else
		FTradeQty = 0;
}
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TMarketDataList *)
{
	new TMarketDataList(NULL);
}
//---------------------------------------------------------------------------
//
// Class TMarketDataList
//
//---------------------------------------------------------------------------
__fastcall TMarketDataList::TMarketDataList(TComponent* Owner)
:TCustomListView(Owner)
,FExchange( L"TAIFEX" )
,FIsLoaded( false )
,FStore( NULL )
,FEvenRowColor( clSilver )
,FOddRowColor( clBlack )
,FTextColor( clNavy )
,FCustomSort( false )
,FUpdating( false )
{
	InitString();
	if( dynamic_cast< TWinControl*>( Owner ) )
        Parent = dynamic_cast< TWinControl*>( Owner );
	RowSelect = true;
	DoubleBuffered = false;
	OwnerDraw   = true;
	FullDrag    = true;
	ReadOnly    = true;
	Checkboxes  = false;
	MultiSelect = true;
	//OnDrawItem  = MyDrawItem;
	ViewStyle   = vsReport;
	DragMode    = dmManual;

	SmallImages = new TImageList(this);
	LargeImages = new TImageList(this);
	LargeImages->Width = 128;
	LargeImages->Height = 128;

	FEarseBKCanvas = new TCanvas();
	FEarseBKCanvas->Brush->Color = FOddRowColor;
	FBufferBmp = new Graphics::TBitmap();
	PopupMenu = new TPopupMenu(this);
	PopupMenu->AutoPopup = false;
	PopupMenu->AutoHotkeys = maManual;
	for( register int i = 1; i < 19; i++ )
	{
		TMenuItem* Item = new TMenuItem(PopupMenu);
		PopupMenu->Items->Add( Item );
		Item->Caption = ListViewHeaderString[i];
		Item->Tag = i;
		Item->Checked = true;
		Item->OnClick = MenuClicked;
	}
	UFC::UDate Date;
	Date.setCurrent();
	BaseYear = ( Date.getYear() / 10 ) * 10;
	Remainder = Date.getYear() % 10;
	InitialCol();
	OnCompare = UserCompare;
}
//---------------------------------------------------------------------------
__fastcall TMarketDataList::~TMarketDataList()
{
	RemoveAllSymbol();
	Items->Clear();
	Columns->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::InitString( void )
{
	ListViewHeaderString[ 0] = Mdcomponentstrings_MD_MDLIST_SYMBOL;//"商品名稱";
	ListViewHeaderString[ 1] = Mdcomponentstrings_MD_OPTIONS_TBAR_BIDPX;//"委買價";
	ListViewHeaderString[ 2] = Mdcomponentstrings_MD_OPTIONS_TBAR_ASKPX;//"委賣價";
	ListViewHeaderString[ 3] = Mdcomponentstrings_MD_OPTIONS_TBAR_FILLPX;//"成交價";
	ListViewHeaderString[ 4] = Mdcomponentstrings_MD_OPTIONS_TBAR_QTY; //"單量";
	ListViewHeaderString[ 5] = Mdcomponentstrings_MD_MDLIST_BID_QTY;//'委買量';
	ListViewHeaderString[ 6] = Mdcomponentstrings_MD_MDLIST_ASK_QTY;//'委賣量';
	ListViewHeaderString[ 7] = Mdcomponentstrings_MD_OPTIONS_TBAR_TOTALQTY;//"總量";
	ListViewHeaderString[ 8] = Mdcomponentstrings_MD_MDLIST_BID_TOTAL_QTY;//'總委買量';
	ListViewHeaderString[ 9] = Mdcomponentstrings_MD_MDLIST_ASK_TOTAL_QTY;//'總委賣量';
	ListViewHeaderString[10] = Mdcomponentstrings_MD_MDLIST_OPEN_PX;//'開盤價';
	ListViewHeaderString[11] = Mdcomponentstrings_MD_OPTIONS_TBAR_HIGH;//"最高";
	ListViewHeaderString[12] = Mdcomponentstrings_MD_OPTIONS_TBAR_LOW;//"最低";
	ListViewHeaderString[13] = Mdcomponentstrings_MD_MDLIST_OPEN_INTEREST;//'未平倉';
	ListViewHeaderString[14] = Mdcomponentstrings_MD_MDLIST_SETTLEMENT_PX;//'結算價';
	ListViewHeaderString[15] = Mdcomponentstrings_MD_MDLIST_PREV_CLOSE_PX;//'昨收';
	ListViewHeaderString[16] = Mdcomponentstrings_MD_MDLIST_UP_DOWN;//'漲跌';
	ListViewHeaderString[17] = Mdcomponentstrings_MD_MDLIST_UP_DOWN_RATE;//'漲跌幅';
	ListViewHeaderString[18] = Mdcomponentstrings_MD_MDLIST_FILL_TIME;//'成交時間';
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::SetColWidth( TListColumn* NewCol, const String& ColName, int SetW )
{
	int minWidth = FBufferBmp->Canvas->TextWidth( ColName )* 1.5;
	int maxWidth = FBufferBmp->Canvas->TextWidth( ColName )* 2.5;
	int curWidth = minWidth;

	if( curWidth < SetW )
		curWidth = SetW;
	if( maxWidth < curWidth )
		maxWidth = curWidth * 1.66;
	NewCol->MaxWidth = maxWidth;
	NewCol->Width    = curWidth;
	NewCol->MinWidth = minWidth;
}
//---------------------------------------------------------------------------
TListColumn* __fastcall TMarketDataList::AddColField( int Field, int SetW )
{
	TListColumn* NewCol;

	FBufferBmp->Canvas->Font->Assign( Font );
	NewCol = (TListColumn*)Columns->Add();
	NewCol->Caption = ListViewHeaderString[ Field ];
	NewCol->Tag = Field;
	NewCol->AutoSize = false;
	SetColWidth(  NewCol, ListViewHeaderString[Field], SetW );
	return NewCol;
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::InitialCol( void )
{
	TListColumn* NewCol;

	Columns->BeginUpdate();
	Columns->Clear();
	for( register int i = 0; i < 19; i++ )
		 AddColField( i );
	Columns->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::AdjuestCol( void )
{
	SmallImages->Height = Canvas->TextHeight( L"A" ) + 5;
	Columns->BeginUpdate();
	for( register int i = 0; i < Columns->Count; i++ )
	{
		TListColumn* Col = Columns->Items[i];
		SetColWidth( Col, ListViewHeaderString[Col->Tag], GetMinFieldWidth( Col->Tag ) );
	}
	Columns->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::Loaded(void)
{
	FIsLoaded = true;
	FBufferBmp->Canvas->Font->Assign( Font );
	SmallImages->Height = FBufferBmp->Canvas->TextHeight( L"A" ) + 5;
	SetStore( FStore );
}
//---------------------------------------------------------------------------
//
// MenuClicked, add or remove field cloums.
//
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::MenuClicked( TObject* Sender )
{
	TMenuItem* ClickedItem = dynamic_cast<TMenuItem*>(Sender);
	if( ClickedItem != NULL )
	{
		int FieldIndex = ClickedItem->Tag;

		Columns->BeginUpdate();
		if( ClickedItem->Checked == true ) ///< Remove column
		{
			ClickedItem->Checked = false;
			UFC::PInt32List Fields,FieldsWidth;
			///< Keep original fields info
			for( register int i = 0; i < Columns->Count; i++ )
			{
				if( FieldIndex != Columns->Items[i]->Tag ) ///< Is the remove one ?
				{
					Fields.Add( Columns->Items[i]->Tag );
					FieldsWidth.Add( Columns->Items[i]->Width );
				}
			}
			///< Clear all
			Columns->Clear();
			///< Restore fields
			for( register int i = 0; i < Fields.ItemCount(); i++ )
				 AddColField( Fields[i], FieldsWidth[i] );
		}
		else ///< Add column
		{
			ClickedItem->Checked = true;
			AddColField( FieldIndex , GetMinFieldWidth( FieldIndex ) );
		}
		Columns->EndUpdate();
	}
}
//---------------------------------------------------------------------------
int  __fastcall TMarketDataList::ItemCount( void )
{
	return FRecordTable.ItemCount();
}
//---------------------------------------------------------------------------
BasicInformation* __fastcall TMarketDataList::GetSymbol( int index )
{
	for( int i = 0; i < Items->Count; i++ )
		 if( Items->Item[ i ]->Index == index )
			 return (BasicInformation*)Items->Item[ i ]->Data;
	return FStore->GetBasicInformation( "TAIFEX", "TXFFront1", false );
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::SetStore( TCMarketDataStore* Store )
{
	if( FStore != Store )
		FStore = Store;
	if( FIsLoaded == true && FStore != NULL )
	{
		TMDListRecord* Record;
		for( register int i = 0; i < FRecordTable.ItemCount(); i++ )
		{
			Record = FRecordTable.GetItem(i);
			if( Record != NULL )
			{
				AnsiString exchange = Record->GetExchange();
				AnsiString symbol   = Record->GetSymbol();

				FStore->Subscribe( exchange.c_str(), symbol.c_str(), this );
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::Subscribe( const String& Exchange, const UnicodeString& Symbol )
{
	if( FIsLoaded == true && FStore != NULL )
	{
		AnsiString exchange = Exchange;
		AnsiString symbol   = Symbol;

		FStore->Subscribe( exchange.c_str(), symbol.c_str(), this );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::AddSymbol( const String& Exchange, const String& Symbol )
{
	TMDListRecord* Record;
	AnsiString     ExStr = Exchange;
	AnsiString     SymStr = Symbol;

	if( ( Record = SearchFromRecordTable( ExStr.c_str(), SymStr.c_str() ) ) == NULL )
	{
		BasicInformation* Info = FStore->GetBasicInformation( Exchange, Symbol, false );
		if( Info != NULL )
		{
			Record = new TMDListRecord( Exchange, Symbol, Info->GetTradeFlag() );

			AddToRecordTable( ExStr.c_str(), SymStr.c_str(), Record );
				TListItem* NewItem = Items->Add();
				NewItem->Caption = Symbol;
			AddToListItemTable( ExStr.c_str(), SymStr.c_str(), NewItem );

			if( FIsLoaded && FStore != NULL )
			{
				NewItem->Data = Info;
				Record->SetBullPx( Info->GetBullPrice() );
				Record->SetBearPx( Info->GetBearPrice() );
				Record->SetRefPx( Info->GetRefPrice() );
				Record->SetDigit( Info->GetDigit() );
				Record->SetDisplayName( Info->GetDisplayName() );
				Record->SetTickList( FStore->GetTickList( Exchange, Symbol) );
				Subscribe( Exchange, Symbol );
			}
		}
	}
	if( FCustomSort )
		CustomSort( SymbolSort, 0 );
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::RemoveAllSymbol( void )
{
	int Count = FRecordTable.ItemCount();
	for( register int i = 0; i < Count; i++ )
	{
		TMDListRecord* DelRec = FRecordTable.GetItem( 0 );
		if( DelRec )
		{
			String Exchange = DelRec->GetExchange();
			String Symbol = DelRec->GetSymbol();
			RemoveSymbol( Exchange, Symbol );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::RemoveSymbol( const String& Exchange, const String& Symbol )
{
	AnsiString AnsiExchange = Exchange;
	AnsiString AnsiSymbol   = Symbol;

	TListItem* DelItem = SearchFromListItemTable( AnsiExchange.c_str(), AnsiSymbol.c_str() );
	TMDListRecord* DelRec = SearchFromRecordTable( AnsiExchange.c_str(), AnsiSymbol.c_str() );

	if( DelItem != NULL )
	{
		RemoveFromListItemTable( AnsiExchange.c_str(), AnsiSymbol.c_str() );
		Items->BeginUpdate();
		Unsubscribe( Exchange, Symbol );
		Items->Delete( Items->IndexOf( DelItem ) );
		Items->EndUpdate();
	}

	if( DelRec != NULL )
	{
		RemoveFromRecordTable( AnsiExchange.c_str(), AnsiSymbol.c_str() );
		delete DelRec;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::Unsubscribe( const String& Exchange, const String& Symbol )
{
	TMDListRecord* Record;
	AnsiString     AnsiExchange = Exchange;
	AnsiString     AnsiSymbol = Symbol;

	FStore->Unsubscribe( AnsiExchange.c_str(), AnsiSymbol.c_str(), this );
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::DrawItem( TListItem* Item, const Types::TRect &Rect, Windows::TOwnerDrawState State)
{
	if( FUpdating == true )
		return;
	TMDListRecord*    Record;
	BasicInformation* Info = (BasicInformation*)Item->Data;
	UFC::AnsiString   Symbol( Info->GetSymbol() );
	UFC::AnsiString   Exchange( Info->GetExchange() );

	if( FBufferBmp->Height != Rect.Height() )
		FBufferBmp->Height = Rect.Height();
	if( FBufferBmp->Width != Rect.Width() )
		FBufferBmp->Width = Rect.Width();

	TRect PaintRect = TRect( 2, 0, Rect.Width() - 2, Rect.Height() );
	FBufferBmp->Canvas->Font->Assign( Font );
	if( ( Record = SearchFromRecordTable( Exchange, Symbol ) ) != NULL )
	{
		int Digit = Record->GetDigit();

		if( Item->Selected )
		{
			FBufferBmp->Canvas->Brush->Color = clNavy;
			FBufferBmp->Canvas->Font->Color  = clWhite;
		}
		else
		{
			if( Item->Index % 2 )
				FBufferBmp->Canvas->Brush->Color = FEvenRowColor;
			else
				FBufferBmp->Canvas->Brush->Color = FOddRowColor;
			FBufferBmp->Canvas->Font->Color  = FSymbolColor;
		}
		FBufferBmp->Canvas->FillRect( PaintRect );

		///< Draw Display name
		TTextFormat Formats;
		String      DispName = Record->GetDisplayName();

		Formats <<tfSingleLine<<tfLeft<<tfVerticalCenter;
		FBufferBmp->Canvas->TextRect( PaintRect, DispName, Formats );

		///< Draw SubItems
		for( int i = 1; i < Columns->Count ; i++ )
		{
			UnicodeString DrawText;
			TTickList*    Ticks;
			TListColumn*  Col = Columns->Items[i];
			int           width;// = 0;

			PaintRect.Left += Columns->Items[i-1]->Width;
			width = Col->Width;
			PaintRect.Right = PaintRect.Left + width;
			switch( Col->Tag )
			{
				case 1://"委買價"
					if( Record->GetBidQty() > 0 && Record->GetBidPx() > 0.0001 )
					{
						if((Ticks = Record->GetTickList( )) != NULL )
							Ticks->ToTNFormat( Record->GetBidPx(), DrawText );
						else
							DrawText.printf( L"%.*f", Record->GetDigit(), Record->GetBidPx() );
						DrawPrice( Record, Record->GetBidPx(), DrawText,  PaintRect, Item->Index, Item->Selected );
					}
					else
						DrawDoubleDash( Item->Index, PaintRect, Item->Selected );
					break;
				case 2: //"委賣價"
					if( Record->GetAskQty() > 0 && Record->GetAskPx() > 0.0001 )
					{
						if((Ticks = Record->GetTickList( )) != NULL )
							Ticks->ToTNFormat( Record->GetAskPx(), DrawText );
						else
							DrawText.printf( L"%.*f", Record->GetDigit(), Record->GetAskPx() );
						DrawPrice( Record, Record->GetAskPx(), DrawText,  PaintRect, Item->Index, Item->Selected );
					}
					else
						DrawDoubleDash( Item->Index, PaintRect, Item->Selected );
					break;
				case 3:// "成交價"
					if( Record->GetTradePx() > 0 &&
						( Record->IsMatched() == true || Record->GetTradeFlag() == false)  )
					{
						double TradePx = Record->GetTradePx();

						if((Ticks = Record->GetTickList( )) != NULL )
							Ticks->ToTNFormat( TradePx, DrawText );
						else
							DrawText.printf( L"%.*f", Record->GetDigit(), TradePx );
						DrawPrice( Record, TradePx, DrawText, PaintRect, Item->Index, Item->Selected );
					}
					else
						DrawDoubleDash( Item->Index, PaintRect, Item->Selected );
					break;
				case 4: // "單量"
					DrawText.printf( L"%d", Record->GetTradeQty() );
					if( Item->Selected )
						DrawSelect( DrawText, PaintRect );
					else
						DrawFillQty( DrawText, PaintRect, Item->Index, Record->IsRise() );
					break;
				case 5: // "委買量"
					DrawText.printf( L"%d", Record->GetBidQty() );
					if( Item->Selected )
						DrawSelect( DrawText, PaintRect );
					else
						DrawOrd( DrawText, PaintRect, Item->Index );
					break;
				case 6: // "委賣量"
					DrawText.printf( L"%d", Record->GetAskQty() );
					if( Item->Selected )
						DrawSelect( DrawText, PaintRect );
					else
						DrawOrd( DrawText, PaintRect, Item->Index );
					break;
				case 7: // "總量"
					if( Record->GetTradeVolume() > 1 )
					{
						DrawText.printf( L"%d", Record->GetTradeVolume() );
						if( Item->Selected )
							DrawSelect( DrawText, PaintRect );
						else
							DrawOrd( DrawText, PaintRect, Item->Index );
					}
					else
						DrawDoubleDash( Item->Index, PaintRect, Item->Selected );
					break;
				case 8: // "總委買量"
					DrawText.printf( L"%d", Record->GetTotalBidQty() );
					if( Item->Selected )
						DrawSelect( DrawText, PaintRect );
					else
						DrawOrd( DrawText, PaintRect, Item->Index );
					break;
				case 9: // "總委賣量"
					DrawText.printf( L"%d", Record->GetTotalAskQty() );
					if( Item->Selected )
						DrawSelect( DrawText, PaintRect );
					else
						DrawOrd( DrawText, PaintRect, Item->Index );
					break;
				case 10: // "開盤價"
					if( Record->GetOpeningPx() > 0 &&
						( Record->IsMatched() == true || Record->GetTradeFlag() == false) )
					{
						if((Ticks = Record->GetTickList( )) != NULL )
							Ticks->ToTNFormat( Record->GetOpeningPx(), DrawText );
						else
							DrawText.printf( L"%.*f", Record->GetDigit(), Record->GetOpeningPx() );
						DrawPrice( Record, Record->GetOpeningPx(), DrawText,  PaintRect, Item->Index, Item->Selected );
					}
					else
						DrawDoubleDash( Item->Index, PaintRect, Item->Selected );
					break;
				case 11: // "最高"
					if(	Record->GetDayHighPx() > 0 &&
						( Record->IsMatched() == true || Record->GetTradeFlag() == false ) )
					{
						if((Ticks = Record->GetTickList( )) != NULL )
							Ticks->ToTNFormat( Record->GetDayHighPx(), DrawText );
						else
							DrawText.printf( L"%.*f", Record->GetDigit(), Record->GetDayHighPx() );
						DrawPrice( Record, Record->GetDayHighPx(), DrawText,  PaintRect, Item->Index, Item->Selected );
					}
					else
						DrawDoubleDash( Item->Index, PaintRect, Item->Selected );
					break;
				case 12: // "最低"
					if( Record->GetDayLowPx() > 0 &&
						( Record->IsMatched() == true || Record->GetTradeFlag() == false) )
					{
						if((Ticks = Record->GetTickList( )) != NULL )
							Ticks->ToTNFormat( Record->GetDayLowPx(), DrawText );
						else
							DrawText.printf( L"%.*f", Record->GetDigit(), Record->GetDayLowPx() );
						DrawPrice( Record, Record->GetDayLowPx(), DrawText,  PaintRect, Item->Index, Item->Selected );
					}
					else
						DrawDoubleDash( Item->Index, PaintRect, Item->Selected );
					break;
				case 13:  // "未平倉"
					DrawText.printf( L"%d", Record->GetOpenInterest() );
					if( Item->Selected )
						DrawSelect( DrawText, PaintRect );
					else
						DrawOrd( DrawText, PaintRect, Item->Index );
					break;
				case 14: // "結算價"
					if((Ticks = Record->GetTickList( )) != NULL )
						Ticks->ToTNFormat( Record->GetSettlementPx(), DrawText );
					else
						DrawText.printf( L"%.*f", Record->GetDigit(), Record->GetSettlementPx() );
					if( Item->Selected )
						DrawSelect( DrawText, PaintRect );
					else
						DrawOrd( DrawText, PaintRect, Item->Index );
					break;
				case 15: // "昨收"
					if((Ticks = Record->GetTickList( )) != NULL )
						Ticks->ToTNFormat( Record->GetClosingPx(), DrawText );
					else
						DrawText.printf( L"%.*f", Record->GetDigit(), Record->GetClosingPx() );
					if( Item->Selected )
						DrawSelect( DrawText, PaintRect );
					else
						DrawOrd( DrawText, PaintRect, Item->Index );
					break;
				case 16:// "漲跌"
					if( Record->GetTradePx() > 0 &&  Record->GetRefPx() > 0  )
					{
						double DiffPx = Record->GetTradePx() - Record->GetRefPx();
						DrawPxDiff( Item->Index, DiffPx, PaintRect, Digit, Item->Selected, Record );
					}
					else
						DrawDoubleDash( Item->Index, PaintRect, Item->Selected );
					break;
				case 17: // "漲跌幅"
					if( Record->GetTradePx() > 0 &&  Record->GetRefPx() > 0  )
					{
						double DiffPx = Record->GetTradePx() - Record->GetRefPx();
						DrawPercentage( Item->Index, DiffPx, PaintRect, Record->GetRefPx(), Item->Selected );
					}
					else
						DrawDoubleDash( Item->Index, PaintRect, Item->Selected );
					break;
				case 18: //  "成交時間"
					if( Record->IsMatched() == true )
					{
						TTextFormat Formats;
						String DrawStr( Record->GetTradeTime().c_str() );

						Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
						if( Item->Selected == true )
							FBufferBmp->Canvas->Font->Color = clWhite;
						else
							FBufferBmp->Canvas->Font->Color = FTextColor;//clWhite;
						FBufferBmp->Canvas->Brush->Style = bsClear;
						FBufferBmp->Canvas->TextRect( PaintRect, DrawStr, Formats );
					}
					else
						DrawDoubleDash( Item->Index, PaintRect, Item->Selected );
					break;
				default: break;

			}
		}
		Canvas->Draw( Rect.Left, Rect.Top, FBufferBmp );
	}
}
//---------------------------------------------------------------------------
//  functions
//  Drawing different fields
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::DrawPxDiff( int Row, double DiffPx, const Types::TRect &ARect, int Digit, bool IsSelected , TMDListRecord* Record )
{
	int ItemLeft = ARect.Left + 5;
	int ItemTop = ARect.Top;
	int ItemHeight = ARect.Height();
	TTickList* Ticks;

	///< Draw background
	if( IsSelected == true )
	{
		FBufferBmp->Canvas->Brush->Color = clNavy;
	}
	else
	{
		if( Row % 2 )
			FBufferBmp->Canvas->Brush->Color = FEvenRowColor;
		else
			FBufferBmp->Canvas->Brush->Color = FOddRowColor;
	}
	FBufferBmp->Canvas->FillRect( ARect );
	if( DiffPx != 0 )
	{
		if( IsSelected )
			FBufferBmp->Canvas->Font->Color  = clWhite;
		else
		{
			if( DiffPx > 0 )
				FBufferBmp->Canvas->Font->Color  = FRiseBKColor;
			else
				FBufferBmp->Canvas->Font->Color  = FFallBKColor;
		}
		String      Text;
		double      Value = DiffPx;
		TTextFormat Formats;
		TRect       PaintRect( ARect );

		PaintRect.Left  += 5;
		PaintRect.Right -= 5;
		Formats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
		if( Value < 0 )
			Value *= -1;
		if((Ticks = Record->GetTickList( )) != NULL )
			Ticks->ToTNFormat( Value, Text );
		else
			Text.printf( L"%.*f", Digit, Value );
		FBufferBmp->Canvas->TextRect( PaintRect, Text, Formats );
	}
	else
	{
		String Text( L"--" );
		TRect  PaintRect( ARect );
		TTextFormat Formats;

		Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		FBufferBmp->Canvas->Pen->Color = clGray;
		FBufferBmp->Canvas->TextRect( PaintRect, Text, Formats );
	}

	if( DiffPx > 0 )
	{
		FBufferBmp->Canvas->Brush->Color = FRiseBKColor;
		FBufferBmp->Canvas->Pen->Color = FRiseBKColor;
		TPoint  TriPoint[4] = {TPoint(ItemLeft + ( ItemHeight - 10 )/2, ItemTop+5),
							   TPoint(ItemLeft ,ItemTop+ItemHeight - 5),
							   TPoint(ItemLeft+(ItemHeight - 10), ItemTop+ItemHeight - 5 ),
							   TPoint(ItemLeft + (ItemHeight - 10)/2,ItemTop+5)};
		FBufferBmp->Canvas->Polygon( TriPoint, 3 );
	}
	else if( DiffPx < 0 )
	{
		FBufferBmp->Canvas->Brush->Color = FFallBKColor;
		FBufferBmp->Canvas->Pen->Color = FFallBKColor;
		TPoint  TriPoint[4] = {TPoint(ItemLeft,ItemTop+5),
							   TPoint(ItemLeft+(ItemHeight - 10),ItemTop+5),
							   TPoint(ItemLeft+(ItemHeight - 10)/2,ItemTop+ItemHeight-5),
							   TPoint(ItemLeft,ItemTop+5)};
		FBufferBmp->Canvas->Polygon( TriPoint, 3 );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::DrawPercentage( int Row, double DiffPx, const Types::TRect &ARect, double RefPx, bool IsSelected )
{
	int ItemLeft = ARect.Left + 5;
	int ItemTop = ARect.Top;
	int ItemHeight = ARect.Height();

	///< Draw background
	if( IsSelected == true )
	{
		FBufferBmp->Canvas->Brush->Color = clNavy;
	}
	else
	{
		if( Row % 2 )
			FBufferBmp->Canvas->Brush->Color = FEvenRowColor;
		else
			FBufferBmp->Canvas->Brush->Color = FOddRowColor;
	}
	FBufferBmp->Canvas->FillRect( ARect );
	if( DiffPx != 0 )
	{
		if( IsSelected )
			FBufferBmp->Canvas->Font->Color  = clWhite;
		else
		{
			if( DiffPx > 0 )
				FBufferBmp->Canvas->Font->Color  = FRiseBKColor;
			else
				FBufferBmp->Canvas->Font->Color  = FFallBKColor;
		}

		String Text;
		TRect  PaintRect( ARect );
		TTextFormat Formats;

		PaintRect.Left  += 5;
		PaintRect.Right -= 5;
		Text.printf( L"%.2f%", ((fabs( DiffPx ))/ RefPx) * 100  );
		Formats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
		FBufferBmp->Canvas->TextRect( PaintRect, Text, Formats );
	}
	else
	{
		String Text( "--" );
		TRect  PaintRect( ARect );
		TTextFormat Formats;

		PaintRect.Left  += 5;
		PaintRect.Right -= 5;
		Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
		FBufferBmp->Canvas->Pen->Color = clGray;
		FBufferBmp->Canvas->TextRect( PaintRect, Text, Formats );
	}
	if( DiffPx > 0 )
	{
		FBufferBmp->Canvas->Brush->Color = FRiseBKColor;
		FBufferBmp->Canvas->Pen->Color = FRiseBKColor;
		TPoint  TriPoint[4] = {TPoint(ItemLeft + ( ItemHeight - 10 )/2, ItemTop+5),
							   TPoint(ItemLeft ,ItemTop+ItemHeight - 5),
							   TPoint(ItemLeft+(ItemHeight - 10), ItemTop+ItemHeight - 5 ),
							   TPoint(ItemLeft + (ItemHeight - 10)/2,ItemTop+5)};
		FBufferBmp->Canvas->Polygon( TriPoint, 3 );
	}
	else if( DiffPx < 0 )
	{
		FBufferBmp->Canvas->Brush->Color = FFallBKColor;
		FBufferBmp->Canvas->Pen->Color = FFallBKColor;
		TPoint  TriPoint[4] = {TPoint(ItemLeft,ItemTop+5),
							   TPoint(ItemLeft+(ItemHeight - 10),ItemTop+5),
							   TPoint(ItemLeft+(ItemHeight - 10)/2,ItemTop+ItemHeight-5),
							   TPoint(ItemLeft,ItemTop+5)};
		FBufferBmp->Canvas->Polygon( TriPoint, 3 );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::DrawPrice( TMDListRecord* Record, double ComparePrice, UnicodeString& Text,  const Types::TRect &ARect, int Row, bool IsSelected )
{

	if( IsSelected )
		DrawSelect( Text, ARect );
	else if( ComparePrice == Record->GetBullPx() )
		DrawLimitUp( Text, ARect );
	else if( ComparePrice > Record->GetRefPx() )
		DrawPriceUp( Text, ARect, Row );
	else if( ComparePrice < Record->GetRefPx() )
		DrawPriceDown( Text, ARect, Row );
	else if( ComparePrice == Record->GetBearPx() )
		DrawLimitDown( Text, ARect );
	else
		DrawOrd( Text, ARect, Row );
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::DrawDoubleDash( int Row, const Types::TRect &ARect, bool IsSelected )
{
	String Text( L"--" );
	TRect  PaintRect( ARect );
	TTextFormat Formats;

	if( IsSelected )
	{
		FBufferBmp->Canvas->Brush->Color = clNavy;
		FBufferBmp->Canvas->Font->Color = clWhite;
	}
	else
	{
		if( Row % 2 )
			FBufferBmp->Canvas->Brush->Color = FEvenRowColor;
		else
			FBufferBmp->Canvas->Brush->Color = FOddRowColor;
		FBufferBmp->Canvas->Font->Color = FTextColor;
	}
	PaintRect.Left  += 5;
	PaintRect.Right -= 5;
	Formats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
	FBufferBmp->Canvas->FillRect( ARect );
	FBufferBmp->Canvas->TextRect( PaintRect, Text, Formats );
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::DrawLimitUp( UnicodeString& drawtext, const Types::TRect &ARect )
{
	TRect  PaintRect( ARect );
	TTextFormat Formats;

	PaintRect.Left  += 5;
	PaintRect.Right -= 5;
	Formats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
	FBufferBmp->Canvas->Font->Color  = clWhite;
	FBufferBmp->Canvas->Brush->Color = FRiseBKColor;
	FBufferBmp->Canvas->FillRect( ARect );
	FBufferBmp->Canvas->TextRect( PaintRect, drawtext,Formats );
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::DrawLimitDown( UnicodeString& drawtext, const Types::TRect &ARect )
{
	TRect  PaintRect( ARect );
	TTextFormat Formats;

	PaintRect.Left  += 5;
	PaintRect.Right -= 5;
	Formats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
	FBufferBmp->Canvas->Font->Color  = clWhite;
	FBufferBmp->Canvas->Brush->Color = FFallBKColor;
	FBufferBmp->Canvas->FillRect( ARect );
	FBufferBmp->Canvas->TextRect( PaintRect, drawtext,Formats );
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::DrawSelect( UnicodeString& drawtext,  const Types::TRect &ARect )
{
	TRect  PaintRect( ARect );
	TTextFormat Formats;

	PaintRect.Left  += 5;
	PaintRect.Right -= 5;
	Formats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
	FBufferBmp->Canvas->Font->Color  = clWhite;
	FBufferBmp->Canvas->Brush->Color = clNavy;
	FBufferBmp->Canvas->FillRect( ARect );
	FBufferBmp->Canvas->TextRect( PaintRect, drawtext,Formats );
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::DrawPriceUp( UnicodeString& drawtext,  const Types::TRect &ARect, int Row )
{
	TRect  PaintRect( ARect );
	TTextFormat Formats;

	PaintRect.Left  += 5;
	PaintRect.Right -= 5;
	Formats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
	FBufferBmp->Canvas->Font->Color  = FRiseBKColor;
	if( Row % 2 )
		FBufferBmp->Canvas->Brush->Color = FEvenRowColor;
	else
		FBufferBmp->Canvas->Brush->Color = FOddRowColor;
	FBufferBmp->Canvas->FillRect( ARect );
	FBufferBmp->Canvas->TextRect( PaintRect, drawtext,Formats );
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::DrawPriceDown( UnicodeString& drawtext,  const Types::TRect &ARect, int Row )
{
	TRect  PaintRect( ARect );
	TTextFormat Formats;

	PaintRect.Left  += 5;
	PaintRect.Right -= 5;
	Formats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
	FBufferBmp->Canvas->Font->Color  = FFallBKColor;
	if( Row % 2 )
		FBufferBmp->Canvas->Brush->Color = FEvenRowColor;
	else
		FBufferBmp->Canvas->Brush->Color = FOddRowColor;
	FBufferBmp->Canvas->FillRect( ARect );
	FBufferBmp->Canvas->TextRect( PaintRect, drawtext,Formats );
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::DrawOrd( UnicodeString& drawtext, const Types::TRect &ARect, int Row )
{
	TRect  PaintRect( ARect );
	TTextFormat Formats;

	PaintRect.Left  += 5;
	PaintRect.Right -= 5;
	Formats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
	FBufferBmp->Canvas->Font->Color  = FTextColor;
	if( Row % 2 )
		FBufferBmp->Canvas->Brush->Color = FEvenRowColor;
	else
		FBufferBmp->Canvas->Brush->Color = FOddRowColor;
	FBufferBmp->Canvas->FillRect( ARect );
	FBufferBmp->Canvas->TextRect( PaintRect, drawtext,Formats );
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::DrawFillQty( UnicodeString& drawtext, const Types::TRect &ARect, int Row, bool Rise )
{
	TRect  PaintRect( ARect );
	TTextFormat Formats;

	PaintRect.Left  += 5;
	PaintRect.Right -= 5;
	Formats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
	if( Rise == true )
		FBufferBmp->Canvas->Font->Color  = FRiseBKColor;
	else
		FBufferBmp->Canvas->Font->Color  = FFallBKColor;
	if( Row % 2 )
		FBufferBmp->Canvas->Brush->Color = FEvenRowColor;
	else
		FBufferBmp->Canvas->Brush->Color = FOddRowColor;
	FBufferBmp->Canvas->FillRect( ARect );
	FBufferBmp->Canvas->TextRect( PaintRect, drawtext,Formats );
}
//---------------------------------------------------------------------------
//
//  Functions implement IMarketDataListener
//
//---------------------------------------------------------------------------
void TMarketDataList::OnMarketDataUpdate( MarketDataMessage* Msg )
{
	TMDListRecord* Record;
	if( ( Record = SearchFromRecordTable( Msg->GetExchange(), Msg->GetSymbol() ) ) != NULL )
	{
		Record->SetAskPx( Msg->GetSellPrice1() );
		Record->SetBidPx( Msg->GetBuyPrice1() );
		Record->SetBidQty( Msg->GetBuyQty1() );
		Record->SetAskQty( Msg->GetSellQty1() );
		Record->SetClosingPx( Msg->GetClosingPrice() );
		Record->SetTradeVolume( Msg->GetTradeVolume() );
		if( Record->GetTradePx() == 0.0 )
			Record->SetTradePx( Msg->GetTradePrice() );
		Record->SetTradeQty( Msg->GetTradeQty() );
		Record->SetDayHighPx( Msg->GetDayHighPrice() );
		Record->SetDayLowPx( Msg->GetDayLowPrice() );
		Record->SetOpenInterest( Msg->GetOpenInterest() );
		Record->SetSettlementPx( Msg->GetSettlementPrice() );
		Record->SetOpeningPx( Msg->GetOpeningPrice() );
		Record->SetTotalBidQty( Msg->GetBuyTotalQty() );
		Record->SetTotalAskQty( Msg->GetSellTotalQty() );
		Record->SetTradeTime( Msg->GetTradeTime() );

		TListItem* dItem = SearchFromListItemTable( Msg->GetExchange(), Msg->GetSymbol() );
		if( dItem != NULL )
		{
			TRect Rect;
			ListView_GetItemRect( this->Handle, GetItemIndex( dItem ), &Rect, LVIR_BOUNDS );
			if( dItem->Selected )
				DrawItem( dItem, Rect, (Windows::TOwnerDrawState)odSelected );
			else
				DrawItem( dItem, Rect, (Windows::TOwnerDrawState)odDefault );
		}
	}
}
//---------------------------------------------------------------------------
void TMarketDataList::OnMarketDataUpdate( OrderBookData* Msg )
{
	TMDListRecord* Record;
	if( ( Record = SearchFromRecordTable( Msg->GetExchange(), Msg->GetSymbol() ) ) != NULL )
	{
		Record->SetAskPx( Msg->GetSellPrice1() );
		Record->SetBidPx( Msg->GetBuyPrice1() );
		Record->SetAskQty( Msg->GetSellQty1() );
		Record->SetBidQty( Msg->GetBuyQty1() );
		TListItem* dItem = SearchFromListItemTable( Msg->GetExchange(), Msg->GetSymbol() );
		if( dItem != NULL )
		{
			TRect Rect;
			ListView_GetItemRect( this->Handle, GetItemIndex( dItem ), &Rect, LVIR_BOUNDS );
			if( dItem->Selected )
				DrawItem( dItem, Rect, (Windows::TOwnerDrawState)odSelected );
			else
				DrawItem( dItem, Rect, (Windows::TOwnerDrawState)odDefault );
		}
	}
}
//---------------------------------------------------------------------------
void TMarketDataList::OnMarketDataUpdate( MatchInfo* Msg )
{
	TMDListRecord* Record;
	if( ( Record = SearchFromRecordTable( Msg->GetExchange(), Msg->GetSymbol() ) ) != NULL &&
		( Msg->GetMatchPrice() >= Record->GetBearPx() &&
		  Msg->GetMatchPrice() <= Record->GetBullPx() ) )
	{
		double OldRefPx = Record->GetRefPx();
		double NewRefPx = Msg->GetRefPx();

		Record->SetTradePx( Msg->GetMatchPrice());
		Record->SetTradeTime( Msg->GetMatchTime() );
		Record->SetTradeQty( Msg->GetMatchQty() );
		if( NewRefPx != OldRefPx && NewRefPx <= Record->GetBullPx() && NewRefPx >= Record->GetBearPx())
		{
			BasicInformation* Info = FStore->GetBasicInformation( Msg->GetExchange(), Msg->GetSymbol());
			Record->SetRefPx( NewRefPx );
			if( Info != NULL )
				Info->SetRefPrice( NewRefPx );
		}
		TListItem* dItem = SearchFromListItemTable( Msg->GetExchange(), Msg->GetSymbol() );
		if( dItem != NULL )
		{
			TRect Rect;
			ListView_GetItemRect( this->Handle, GetItemIndex( dItem ), &Rect, LVIR_BOUNDS );
			if( dItem->Selected )
				DrawItem( dItem, Rect, (Windows::TOwnerDrawState)odSelected );
			else
				DrawItem( dItem, Rect, (Windows::TOwnerDrawState)odDefault );
		}
	}
}
//---------------------------------------------------------------------------
void TMarketDataList::OnMarketDataUpdate( OpeningInfo* Msg )
{
	UnicodeString Symbol = Msg->GetSymbol().c_str();
	TMDListRecord* Record;
	if( ( Record = SearchFromRecordTable( Msg->GetExchange(), Msg->GetSymbol() ) ) != NULL )
	{
		Record->SetOpeningPx( Msg->GetOpeningPrice() );

		TListItem* drawItem = SearchFromListItemTable( Msg->GetExchange(), Msg->GetSymbol() );
		if( drawItem != NULL )
		{

			TRect Rect;
			ListView_GetItemRect( this->Handle, GetItemIndex( drawItem ), &Rect, LVIR_BOUNDS );
			if( drawItem->Selected )
				DrawItem( drawItem, Rect, (Windows::TOwnerDrawState)odSelected );
			else
				DrawItem( drawItem, Rect, (Windows::TOwnerDrawState)odDefault );
		}
	}
}
//---------------------------------------------------------------------------
void TMarketDataList::OnMarketDataUpdate( TotalMatch* Msg )
{
	UnicodeString Symbol = Msg->GetSymbol().c_str();
	TMDListRecord* Record;
	if( ( Record = SearchFromRecordTable( Msg->GetExchange(), Msg->GetSymbol() ) ) != NULL )
	{
		Record->SetTradeVolume( Msg->GetTotalMatchQty() );

		TListItem* drawItem = SearchFromListItemTable( Msg->GetExchange(), Msg->GetSymbol() );
		if( drawItem != NULL )
		{
			TRect Rect;
			ListView_GetItemRect( this->Handle, GetItemIndex( drawItem ), &Rect, LVIR_BOUNDS );
			if( drawItem->Selected )
				DrawItem( drawItem, Rect, (Windows::TOwnerDrawState)odSelected );
			else
				DrawItem( drawItem, Rect, (Windows::TOwnerDrawState)odDefault );
		}
	}
}
//---------------------------------------------------------------------------
void TMarketDataList::OnMarketDataUpdate( SumOfOrderInfo* Msg )
{
	UnicodeString Symbol = Msg->GetSymbol().c_str();
	TMDListRecord* Record;
	if( ( Record = SearchFromRecordTable( Msg->GetExchange(), Msg->GetSymbol() ) ) != NULL )
	{
		Record->SetTotalBidQty( Msg->GetBuyQty() );
		Record->SetTotalAskQty( Msg->GetSellQty() );

		TListItem* drawItem = SearchFromListItemTable( Msg->GetExchange(), Msg->GetSymbol() );
		if( drawItem != NULL )
		{
			TRect Rect;
			ListView_GetItemRect( this->Handle, GetItemIndex( drawItem ), &Rect, LVIR_BOUNDS );
			if( drawItem->Selected )
				DrawItem( drawItem, Rect, (Windows::TOwnerDrawState)odSelected );
			else
				DrawItem( drawItem, Rect, (Windows::TOwnerDrawState)odDefault );
		}
	}
}
//---------------------------------------------------------------------------
void TMarketDataList::OnMarketDataUpdate( UnderlyingIndexInfo* Msg )
{
	UnicodeString Symbol = Msg->GetSymbol().c_str();
	TMDListRecord* Record;
	if( ( Record = SearchFromRecordTable( Msg->GetExchange(), Msg->GetSymbol() ) ) != NULL )
	{
		if( Msg->GetIndexValue() > 0 )
		{
			double MatchPx = Msg->GetIndexValue();
			Record->SetTradePx( MatchPx );
			if( Msg->GetShowTime().Length() > 0  )
				Record->SetTradeTime( Msg->GetShowTime() );
		}

		if( Msg->GetTotalQty() > 0 )
			Record->SetTradeVolume( Msg->GetTotalQty() );

		TListItem* drawItem = SearchFromListItemTable( Msg->GetExchange(), Msg->GetSymbol()  );
		if( drawItem != NULL )
		{
			TRect Rect;
			ListView_GetItemRect( this->Handle, GetItemIndex( drawItem ), &Rect, LVIR_BOUNDS );
			if( drawItem->Selected )
				DrawItem( drawItem, Rect, (Windows::TOwnerDrawState)odSelected );
			else
				DrawItem( drawItem, Rect, (Windows::TOwnerDrawState)odDefault );
		}
	}
}
//---------------------------------------------------------------------------
void TMarketDataList::OnMarketDataUpdate( DayHighLowPrice* Msg )
{
	UnicodeString Symbol = Msg->GetSymbol().c_str();
	TMDListRecord* Record;
	if( ( Record = SearchFromRecordTable( Msg->GetExchange(), Msg->GetSymbol() ) ) != NULL )
	{
		Record->SetDayHighPx( Msg->GetDayHighPrice() );
		Record->SetDayLowPx( Msg->GetDayLowPrice() );

		TListItem* drawItem = SearchFromListItemTable( Msg->GetExchange(), Msg->GetSymbol() );
		if( drawItem != NULL )
		{
			TRect Rect;
			ListView_GetItemRect( this->Handle, GetItemIndex( drawItem ), &Rect, LVIR_BOUNDS );
			if( drawItem->Selected )
				DrawItem( drawItem, Rect, (Windows::TOwnerDrawState)odSelected );
			else
				DrawItem( drawItem, Rect, (Windows::TOwnerDrawState)odDefault );
		}
	}
}
//---------------------------------------------------------------------------
void TMarketDataList::OnMarketDataUpdate( ClosingMarketData* Msg )
{
	UnicodeString Symbol = Msg->GetSymbol().c_str();
	TMDListRecord* Record;
	if( ( Record = SearchFromRecordTable( Msg->GetExchange(), Msg->GetSymbol() ) ) != NULL )
	{
		if( Msg->GetMsgType() == mtClosingMarketDataWithSettlementPrice )
		{
			Record->SetSettlementPx( ( ( ClosingMarketDataWithSettlementPrice*)Msg)->GetSettlementPrice() );
		}
		else if( Msg->GetMsgType() ==  mtClosingMarketDataWithSettlementPriceAndOpenInterest )
		{
			Record->SetSettlementPx( ( ( ClosingMarketDataWithSettlementPriceAndOpenInterest*)Msg)->GetSettlementPrice() );
			Record->SetOpenInterest( ( ( ClosingMarketDataWithSettlementPriceAndOpenInterest*)Msg)->GetOpenInterest() );
		}

		TListItem* drawItem = SearchFromListItemTable( Msg->GetExchange(), Msg->GetSymbol() );
		if( drawItem != NULL )
		{
			TRect Rect;
			ListView_GetItemRect( this->Handle, GetItemIndex( drawItem ), &Rect, LVIR_BOUNDS );
			if( drawItem->Selected )
				DrawItem( drawItem, Rect, (Windows::TOwnerDrawState)odSelected );
			else
				DrawItem( drawItem, Rect, (Windows::TOwnerDrawState)odDefault );
		}
	}
}
//---------------------------------------------------------------------------
Classes::TStringList* __fastcall TMarketDataList::GetSymbols( void )
{
	return NULL;
}
//---------------------------------------------------------------------------
void TMarketDataList::OnMarketDataUpdate( BasicData* Msg ) {}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::SetTextColor( TColor Color )
{
	FTextColor = Color;
	if( ComponentState.Contains( csDesigning ) )
		Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::SetEvenRowColor( TColor Color )
{
	FEvenRowColor = Color;
	if( ComponentState.Contains( csDesigning ) )
		Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::SetOddRowColor( TColor Color )
{
	FOddRowColor = Color;
	FEarseBKCanvas->Brush->Color = FOddRowColor;
	if( ComponentState.Contains( csDesigning ) )
		Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::SetRiseBKColor( TColor Color )
{
	FRiseBKColor = Color;
	if( ComponentState.Contains( csDesigning ) )
		Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::SetFallBKColor( TColor Color )
{
	FFallBKColor = Color;
	if( ComponentState.Contains( csDesigning ) )
		Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::SetSymbolColor( TColor Color )
{
	FSymbolColor = Color;
	if( ComponentState.Contains( csDesigning ) )
		Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::EraseBK( HDC DC )
{
	HWND HeaderHandle = ListView_GetHeader( Handle );
	TRect HeaderItemRect, HeaderRect;

	FEarseBKCanvas->Handle = DC;

	Header_GetItemRect( HeaderHandle, 0, &HeaderItemRect );
	HeaderRect = Rect( 0, 0, Width, HeaderItemRect.Height() );
	TRect RepaintRect = Rect( 0, 0, Width, Height );
	FEarseBKCanvas->Brush->Color = FEvenRowColor;
	HRGN MainRgn = CreateRectRgnIndirect( &RepaintRect );
	HRGN HeaderRgn = CreateRectRgnIndirect( &HeaderRect );
	CombineRgn( MainRgn, MainRgn, HeaderRgn, RGN_DIFF );

	if( Items->Count == 0 )
		FillRgn( DC, MainRgn, FEarseBKCanvas->Brush->Handle );
	else
	{
		TRect ItemRect;
		ListView_GetItemRect( Handle, Items->Count - 1, &ItemRect, LVIR_BOUNDS );
		int width, height;
		if( ItemRect.Bottom > Height )
			height = Height;
		else
			height = ItemRect.Bottom;
		width = ItemRect.Width();
		TRect TmpRect = Rect( 0, HeaderRect.Height(), width, height);
		HRGN ItemRgn = CreateRectRgnIndirect( &TmpRect );
		CombineRgn( MainRgn, MainRgn, ItemRgn, RGN_DIFF );
		FillRgn( DC, MainRgn, FEarseBKCanvas->Brush->Handle );
		DeleteObject( (HGDIOBJ)ItemRgn );
	}
	DeleteObject( (HGDIOBJ)MainRgn );
	DeleteObject( (HGDIOBJ)HeaderRgn );
}
//---------------------------------------------------------------------------
int __fastcall TMarketDataList::GetMinWidth( int Col )
{
	return GetMinFieldWidth( Columns->Items[ Col ]->Tag );
}
//---------------------------------------------------------------------------
int __fastcall TMarketDataList::GetMinFieldWidth( int FieldID )
{
	int            MinWidth = FBufferBmp->Canvas->TextWidth( ListViewHeaderString[FieldID] ) * 1.5;
	TMDListRecord* Record = FRecordTable.First();
	TTickList*     Ticks;

	switch( FieldID )
	{
		case 0: ///<DisplayName
			while( Record )
			{
				int SymbolWidth = FBufferBmp->Canvas->TextWidth( Record->GetDisplayName() ) * 1.2;
				if( SymbolWidth > MinWidth )
					MinWidth = SymbolWidth;
				Record = FRecordTable.Next();
			}
			break;
		case 1:   ///< Px
		case 2:
		case 3:
		case 10:
		case 11:
		case 12:
		case 14:
		case 15:
			while( Record )
			{
				String PxStr;

				if((Ticks = Record->GetTickList( )) != NULL )
					Ticks->ToTNFormat( Record->GetBullPx(), PxStr );
				else
					PxStr.printf( L"%.*f", Record->GetDigit(),Record->GetBullPx() );
				int PxWidth = FBufferBmp->Canvas->TextWidth( PxStr ) * 1.5;
				if( PxWidth > MinWidth )
					MinWidth = PxWidth;
				Record = FRecordTable.Next();
			}
			break;
		case 4: ///< TradeQty
			while( Record )
			{
				String QtyStr;
				QtyStr.printf( L"%d", Record->GetTradeQty() );
				int QtyWidth = FBufferBmp->Canvas->TextWidth( QtyStr ) * 1.5;
				if( QtyWidth > MinWidth )
					MinWidth = QtyWidth;
				Record = FRecordTable.Next();
			}
			break;
		case 5: ///< BuyQty 1
			while( Record )
			{
				String QtyStr;
				QtyStr.printf( L"%d", Record->GetBidQty() );
				int QtyWidth = FBufferBmp->Canvas->TextWidth( QtyStr ) * 1.5;
				if( QtyWidth > MinWidth )
					MinWidth = QtyWidth;
				Record = FRecordTable.Next();
			}
			break;
		case 6: ///< AskQty
			while( Record )
			{
				String QtyStr;
				QtyStr.printf( L"%d", Record->GetAskQty() );
				int QtyWidth = FBufferBmp->Canvas->TextWidth( QtyStr ) * 1.5;
				if( QtyWidth > MinWidth )
					MinWidth = QtyWidth;
				Record = FRecordTable.Next();
			}
			break;
		case 7:
			while( Record )
			{
				String QtyStr;
				QtyStr.printf( L"%d", Record->GetTradeVolume() );
				int QtyWidth = FBufferBmp->Canvas->TextWidth( QtyStr ) * 1.5;
				if( QtyWidth > MinWidth )
					MinWidth = QtyWidth;
				Record = FRecordTable.Next();
			}
			break;
		case 8:
			while( Record )
			{
				String QtyStr;
				QtyStr.printf( L"%d", Record->GetTotalBidQty() );
				int QtyWidth = FBufferBmp->Canvas->TextWidth( QtyStr ) * 1.5;
				if( QtyWidth > MinWidth )
					MinWidth = QtyWidth;
				Record = FRecordTable.Next();
			}
			break;
		case 9:
			while( Record )
			{
				String QtyStr;
				QtyStr.printf( L"%d", Record->GetTotalAskQty() );
				int QtyWidth = FBufferBmp->Canvas->TextWidth( QtyStr ) * 1.5;
				if( QtyWidth > MinWidth )
					MinWidth = QtyWidth;
				Record = FRecordTable.Next();
			}
			break;
		case 13:
			while( Record )
			{
				String QtyStr;
				QtyStr.printf( L"%d", Record->GetOpenInterest() );
				int QtyWidth = FBufferBmp->Canvas->TextWidth( QtyStr ) * 1.5;
				if( QtyWidth > MinWidth )
					MinWidth = QtyWidth;
				Record = FRecordTable.Next();
			}
			break;
		case 18:
			if( Record )
			{
				int TimeWidth = FBufferBmp->Canvas->TextWidth( "12:34:56.xxx" ) * 1.5;
				if( TimeWidth > MinWidth )
					MinWidth = TimeWidth;
			}
			break;
		case 16:
        	while( Record )
			{
				String DiffPxStr;
				TRect Rect;

				if((Ticks = Record->GetTickList( )) != NULL )
					Ticks->ToTNFormat( Record->GetTradePx() - Record->GetRefPx(), DiffPxStr );
				else
					DiffPxStr.printf( L"%.*f", Record->GetDigit(),Record->GetTradePx() - Record->GetRefPx() );
				DiffPxStr += L"▲";
				ListView_GetItemRect( Handle, 0, &Rect, LVIR_BOUNDS );
				if( Rect.Height() )
				{
					int DiffPxWidth = ( FBufferBmp->Canvas->TextWidth( DiffPxStr ) )* 1.4 ;
					if( DiffPxWidth > MinWidth )
						MinWidth = DiffPxWidth;
					Record = FRecordTable.Next();
				}
			}
			break;
		case 17:
		   while( Record )
			{
				String DiffPxStr;
				TRect Rect;

				if((Ticks = Record->GetTickList( )) != NULL )
					Ticks->ToTNFormat( Record->GetTradePx() - Record->GetRefPx(), DiffPxStr );
				else
					DiffPxStr.printf( L"%.*f", Record->GetDigit(), Record->GetTradePx() - Record->GetRefPx() );
				DiffPxStr += L"▲";
				ListView_GetItemRect( Handle, 0, &Rect, LVIR_BOUNDS );
				if( Rect.Height() )
				{
					int DiffPxWidth = ( FBufferBmp->Canvas->TextWidth( DiffPxStr ) )* 1.7 ;
					if( DiffPxWidth > MinWidth )
						MinWidth = DiffPxWidth;
					Record = FRecordTable.Next();
				}
			}
			break;

		default: break;
	}
	return MinWidth;
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::WndProc( TMessage &Msg )
{
	NMHDR* pnmh;
	POINT Point;
	switch( Msg.Msg )
	{
		case WM_NOTIFY:
			pnmh=((NMHDR*)Msg.LParam);
			if( pnmh->code == NM_RCLICK )
			{
				GetCursorPos( &Point );
				PopupMenu->Popup( Point.x, Point.y );
				Msg.Result = 1;
			}
			else
				inherited::WndProc( Msg );
			break;
		case WM_ERASEBKGND:
			if(  ViewStyle == vsReport )
			{
				EraseBK( (HDC) Msg.WParam );
				Msg.Result = 1;
			}
			else
				inherited::WndProc( Msg );
			break;
		default:
			inherited::WndProc( Msg );
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::BeginUpdate( void )
{
	Items->BeginUpdate();
	FUpdating = true;
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::EndUpdate( void )
{
	Items->EndUpdate();
	if( FCustomSort )
		CustomSort( SymbolSort, 0 );
	for( register int i = 0; i < Columns->Count; i++ )
	{
		int width = GetMinWidth( i );
		//if( width > Columns->Items[i]->Width )
		{
			Columns->Items[i]->MinWidth = width;
			Columns->Items[i]->MaxWidth = width / 1.5 * 2.5;
			if( Columns->Items[i]->Width < width || Columns->Items[i]->Width > Columns->Items[i]->MaxWidth )
				Columns->Items[i]->Width = width;
		}
	}
	FUpdating = false;
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::GetColumns( UnicodeString& Str )
{
	UnicodeString RtStr;
	UFC::NameValueMessage Msg;
	for( int i = 1; i < Columns->Count ; i++ )
	{
		TListColumn* Col = Columns->Items[i];
		Msg.Append( UFC::AnsiString( Col->Tag ), (long)Col->Width );
	}
	Str = Msg.ToString().c_str();
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::SetPopupMenuChecked( bool Checked )
{
	for( register int i = 0; i < PopupMenu->Items->Count; i++ )
		PopupMenu->Items->Items[i]->Checked = Checked;
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::SetColumns( const String& ColStr )
{
	AnsiString            AnsiColStr = ColStr;
	UFC::NameValueMessage Msg;
	UFC::AnsiString       FieldID, FieldW;

	SetPopupMenuChecked( false );
	Msg.FromString( AnsiColStr.c_str() );
	Columns->BeginUpdate();
	Columns->Clear();
	AddColField( 0 );
	for( register int i = 0; i < Msg.ItemCount(); i++ )
	{
		if( Msg.Get( i, FieldID, FieldW ) )
		{
			int        Index = FieldID.ToInt();
			TMenuItem* Item  = PopupMenu->Items->Items[ Index  - 1 ]; ///< Zero base
			if( Item != NULL )
			{
				Item->Checked = true;
				AddColField( Index, FieldW.ToInt() );
			}
		}
	}
	Columns->EndUpdate();
}
//---------------------------------------------------------------------------
int __stdcall SymbolSort(long lParam1, long lParam2, long ParamSort)
{
	TListItem* Item1 = reinterpret_cast<TListItem *>(lParam1);
	TListItem* Item2 = reinterpret_cast<TListItem *>(lParam2);

	if ((Item1 == NULL) || (Item2 == NULL))
		return 0;
	BasicInformation* Info1 = (BasicInformation*)Item1->Data;
	BasicInformation* Info2 = (BasicInformation*)Item2->Data;

	if( Info1->GetTradeFlag() == false )
		return -1;

	if( Info2->GetTradeFlag() == false )
		return 1;

	if( Info1 != NULL && Info2 != NULL )
	{
		if( Info1->GetEndDate().ToInt() > Info2->GetEndDate().ToInt() )
			return 1;
		else if( Info1->GetEndDate().ToInt() < Info2->GetEndDate().ToInt() )
			return -1;
	}
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
{
	TCustomListView::MouseDown( Button, Shift, X, Y );
	TListItem* ClickedItem = GetItemAt( X, Y );
	TRect ColRect;

	if( ClickedItem != NULL && Shift.Contains( ssLeft ) )
	{
		int ColLeft = 0;
		int ColRight = 0;

		X += ViewOrigin.x; ///< Add the scroll X.
		for( register int i = 0; i < Columns->Count; i++ )
		{
			TListColumn* Col = Columns->Items[i];
			ColRight += Col->Width;

			if( Col->Tag == 1 && ( ColLeft < X && ColRight > X ) )
			{
				BasicInformation* Info = (BasicInformation*)ClickedItem->Data;
				TMDListRecord* Record = SearchFromRecordTable( Info->GetExchange(), Info->GetSymbol() );
				double Px = Record->GetBidPx();
				if( FOnMouseClickSymbolEvent != NULL )
					FOnMouseClickSymbolEvent( this,Info->GetExchange().c_str(), ClickedItem->Caption, nsOrderMessageDefine::sBuy, Px );
				return;
			}
			else if( Col->Tag == 2 && ( ColLeft < X && ColRight > X ) )
			{
				BasicInformation* Info = (BasicInformation*)ClickedItem->Data;
				TMDListRecord* Record = SearchFromRecordTable( Info->GetExchange(), Info->GetSymbol() );
				double Px = Record->GetAskPx();
				if( FOnMouseClickSymbolEvent != NULL )
					FOnMouseClickSymbolEvent( this,Info->GetExchange().c_str(), ClickedItem->Caption, nsOrderMessageDefine::sSell, Px );
				return;
			}
			ColLeft += Col->Width;
		}
		if( FOnMouseClickItemEvent != NULL )
		{
			BasicInformation* Info = (BasicInformation*)ClickedItem->Data;
			FOnMouseClickItemEvent( this, Info->GetExchange().c_str(), ClickedItem->Caption );
		}
		if( Sort == false )
			BeginDrag( false, 3 );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::UserCompare(System::TObject* Sender, TListItem* Item1, TListItem* Item2, int Data, int &Compare)
{
	AnsiString Symbol1 = Item1->Caption;
	AnsiString Symbol2 = Item2->Caption;
	int Index1 = FOrderList.IndexOf( Symbol1.c_str() );
	int Index2 = FOrderList.IndexOf( Symbol2.c_str() );
	if( Index1 < Index2 )
		Compare =  -1;
	else if( Index1 > Index2 )
		Compare =  1;
	else
		Compare =  0;
}
//---------------------------------------------------------------------------
//
//  Functions
//  FRecordTable  Exchange, Symbol maps to market data record.
//
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::AddToRecordTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, TMDListRecord* Record )
{
	if( Record != NULL )
	{
		UFC::AnsiString Key;
		Key.Printf( "%s.%s", Exchange.c_str(), Symbol.c_str() );
		FRecordTable.Add( Key, Record );
	}
}
//---------------------------------------------------------------------------
TMDListRecord* __fastcall TMarketDataList::SearchFromRecordTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol )
{
	UFC::AnsiString Key;
	Key.Printf( "%s.%s", Exchange.c_str(), Symbol.c_str() );
	return FRecordTable.GetObjectByKey( Key );
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::RemoveFromRecordTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol )
{
	UFC::AnsiString Key;
	Key.Printf( "%s.%s", Exchange.c_str(), Symbol.c_str() );
	FRecordTable.DeleteByKey( Key );
}
//---------------------------------------------------------------------------
//
//  Functions
//  FListItemTable  Exchange, Symbol maps to ListItem
//
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::AddToListItemTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, TListItem* Record )
{
	if( Record != NULL )
	{
		UFC::AnsiString Key;
		Key.Printf( "%s.%s", Exchange.c_str(), Symbol.c_str() );
		FListItemTable.Add( Key, Record );
	}
}
//---------------------------------------------------------------------------
TListItem* __fastcall TMarketDataList::SearchFromListItemTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol )
{
	UFC::AnsiString Key;
	Key.Printf( "%s.%s", Exchange.c_str(), Symbol.c_str() );
	return FListItemTable.GetObjectByKey( Key );
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::RemoveFromListItemTable( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol )
{
	UFC::AnsiString Key;
	Key.Printf( "%s.%s", Exchange.c_str(), Symbol.c_str() );
	FListItemTable.DeleteByKey( Key );
}
//---------------------------------------------------------------------------
//  Drag drop functions
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::DragDrop(System::TObject* Source, int X, int Y)
{
	TListItem* SourceItem = Selected;
	TListItem* MoveToItem = GetItemAt( X,Y );

	if( SourceItem != NULL && MoveToItem != NULL )
	{
		BasicInformation* Info = (BasicInformation*)SourceItem->Data;
		TListItem* NewItem;

		Items->BeginUpdate();
		RemoveFromListItemTable( Info->GetExchange(), Info->GetSymbol());
		if( MoveToItem->Index < Items->Count - 1  )
		{
			if( SourceItem->Index > MoveToItem->Index )
				NewItem = Items->Insert( MoveToItem->Index  );
			else
				NewItem = Items->Insert( MoveToItem->Index + 1 );
		}
		else
			NewItem = Items->Add();
		NewItem->Assign( SourceItem );
		Items->Delete( SourceItem->Index );
		AddToListItemTable( Info->GetExchange(), Info->GetSymbol(), NewItem );
		Items->EndUpdate();
		if( FOnItemPosChanged != NULL)
			FOnItemPosChanged( this );
	}
}
//---------------------------------------------------------------------------
void __fastcall TMarketDataList::DragOver( TObject *Source, int X,
		  int Y, TDragState State, bool &Accept)
{
	TListItem* OverItem = GetItemAt( X,Y );
	if( OverItem != NULL && OverItem != Selected )
		Accept = true;
	else
		Accept = FALSE;
}
//---------------------------------------------------------------------------
namespace Marketdatalist
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TMarketDataList)};
		RegisterComponents(L"Speedy", classes, 0);
	}
}
//---------------------------------------------------------------------------
