//---------------------------------------------------------------------------
#include <vcl.h>
#include <Math.h>
#include <float.h>
#include "GraphUtility.h"
#pragma hdrstop
#include "OptionsStrikePriceView.h"
#include "MDComponentStrings.hpp"
#pragma package(smart_init)
//---------------------------------------------------------------------------
UFC::AnsiString gOptFieldsTypeName[] = { "BuyPx",
										 "SellPx",
										 "TradePx",
										 "DiffPx",
										 "TradeQty",
										 "TotalQty",
										 "HighPx",
										 "LowPx",
										 "SettlementPx",
										 "OpenInterest",
										 "Delta",
										 "Gamma",
										 "Theta",
										 "Vega",
										 "Rho",
										 "StrikePx",
										 "DiffOI",
										 "BuyQty",
										 "SellQty",
										 "ImpVar",
										 "TheoryPX" };
//---------------------------------------------------------------------------
TStringList*    gOptFieldsDisplayName = NULL;
//---------------------------------------------------------------------------
int __fastcall OptSymbolCompare(void *Item1, void *Item2)
{
	TOptRecord* Rec1 = ( TOptRecord* )Item1;
	TOptRecord* Rec2 = ( TOptRecord* )Item2;
	double YM1 = Rec1->GetStrikePx();
	double YM2 = Rec2->GetStrikePx();

	if( YM1 > YM2 )
		return 1;
	else if( YM1 < YM2 )
		return -1;
	else
		return 0;
}
//---------------------------------------------------------------------------
//
//  Class TOptRecord
//  Object to keep Options pin record
//
//---------------------------------------------------------------------------
TOptRecord::TOptRecord( BasicInformation* Info, bool Underlying )
:FExchange( Info->GetExchange().c_str() )
,FSymbol( Info->GetSymbol().c_str() )
,FBullPx( UFC::DoubleToInt( Info->GetBullPrice(), 4 ) )
,FBearPx( UFC::DoubleToInt( Info->GetBearPrice(), 4 ) )
,FRefInt( UFC::DoubleToInt( Info->GetRefPrice(), 4 ) )
,FBidPx( 0.0 )
,FAskPx( 0.0 )
,FBidQty( 0 )
,FAskQty( 0 )
,FTradePx( 0.0 )
,FRefPx( Info->GetRefPrice() )
,FTradeQty( 0 )
,FTotalQty( 0 )
,FHighPx( 0.0 )
,FLowPx( 0.0 )
,FCallPut( Info->GetCallPut() )
,FDigit( Info->GetDigit() )
,FStrikePx( Info->GetStrikePrice() )
,FSettlementPx( Info->GetRefPrice() )
,FYearMonth( Info->GetMaturityDate().c_str() )
,FOpenInterest( 0 )
,FOpenInterestDiff( 0 )
,FIsUnderlying( Underlying )
,FValueWidth( 0 )
,FProdID( Info->GetProductID().c_str())
,FMaxHDays( Info->GetMaxHistoryVolDays() )
,FExpiryDays((double)Info->GetExpiryDays())
,FHistoryVol(Info->GetHistoryVol( FMaxHDays ) )
,FDspBuf( NULL )        ///< Underlying display buffer
,FStrikePxRate( 0.5 )   ///< StrikePx pos in this grid
,FRowIndex(0)           ///< Row index in the grid.
,FStrikePxAt(0)         ///< Strike px index in FStrikePrices.
,FPrevStrikePxIndex( 0 )
,FStrikePxIndex( 0 )
,Next( NULL )
,Underlying( NULL )
{
	if( FIsUnderlying == true )
	{
		FDisplayName = String( Info->GetDisplayName() );
		FDspBuf      = new Graphics::TBitmap();
	}
	else
	{
		if( FHistoryVol <= 0.0 )
			FHistoryVol = 0.15;
		if( FExpiryDays <= 0.0 )
			FExpiryDays = CalExpiryDays( Info->GetLastTradeDate().c_str());
	}
	if( FExchange.Length() == 0 || FSymbol.Length() == 0 )
		throw Exception( "No Symbol/Exchange Name");
}
//---------------------------------------------------------------------------
TOptRecord::~TOptRecord( void )
{
	if( FDspBuf != NULL )
		delete FDspBuf;
}
//---------------------------------------------------------------------------
int TOptRecord::CalExpiryDays( const String& sLastTradeDate )
{
	int Days = 20;

	if (sLastTradeDate != "" && sLastTradeDate.Length() == 8)
	{
		try
		{
			int    year  = sLastTradeDate.SubString( 1, 4 ).ToInt();
			int    month = sLastTradeDate.SubString( 5, 2 ).ToInt();
			int    day   = sLastTradeDate.SubString( 7, 2 ).ToInt();
			if( year < 1971 || month > 12 || day > 31 )
				return Days;

			String sDate =  Format( "%.4d/%.2d/%.2d", ARRAYOFCONST( ( year, month, day ) ) );
			TDate deDT   = StrToDate(sDate);
			TDate deNOW  = TDate::CurrentDate();
			Days = (int)(deDT - deNOW);
			if( Days < 0 )
				Days = 0;
		}
		catch(...){}
	}
	return Days;
}
//---------------------------------------------------------------------------
void TOptRecord::StrikePxPosition( double Pxdif, double StrikePxStep )
{
	FStrikePxRate = Pxdif / StrikePxStep + 0.5;
	if( FStrikePxRate < 0 )
		FStrikePxRate = 0;
	if( FStrikePxRate > 1 )
		FStrikePxRate = 1;
}
//---------------------------------------------------------------------------
int TOptRecord::FindNearistStrikePx( int Last, double Px, double StrikePxStep )
{
	int Prev = Last - 1;
	int Next = Last + 1;

	if( Prev < 0 )
		Prev = 0;
	if( Next > FStrikePrices.ItemCount() - 1 )
		Next = FStrikePrices.ItemCount() - 1;

	double d1 = fabs( Px - FStrikePrices[ Prev ] );
	double d2 = fabs( Px - FStrikePrices[ Last ] );
	double d3 = fabs( Px - FStrikePrices[ Next ] );

	if( d1 <= d2 && d1 <= d3 )
	{
		StrikePxPosition( Px - FStrikePrices[ Prev ], StrikePxStep );
		return Prev;
	}
	else if( d2 <= d1 && d2 <= d3 )
	{
		StrikePxPosition( Px - FStrikePrices[ Last ], StrikePxStep );
		return Last;
	}
	else
	{
		StrikePxPosition( Px - FStrikePrices[ Next ], StrikePxStep );
		return Next;
	}
}
//---------------------------------------------------------------------------
int TOptRecord::UnderlyingTradePxAt( double Px )
{
	if( FStrikePrices.ItemCount() > 1 )
	{
		register int First = 0;
		register int Last  = FStrikePrices.ItemCount() - 1;
		register int Half  = (Last - First)>>1;
		double StrikePxStep = (FStrikePrices[ Last ] - FStrikePrices[ First ])/(double)FStrikePrices.ItemCount();

		if( Px  <= FStrikePrices[ First ] )///< Smaller than the first Strike Px.
		{
			StrikePxPosition( Px - FStrikePrices[ First ], StrikePxStep );
			return First;
		}
		if( Px  >= FStrikePrices[ Last ] ) ///< Larger than the last Strike Px.
		{
			StrikePxPosition( Px - FStrikePrices[ First ], StrikePxStep );
			return Last;
		}
		for(;;) ///< Binary search.
		{
			if( Px  > FStrikePrices[Half] )
				First = Half;
			else if( Px  < FStrikePrices[Half] )
				Last  = Half;
			else  ///< Bingo! Same as the middle object.
			{
				FStrikePxRate = 0.5; //< line in the middle of the strike px cell.
				return Half;
			}
			Half  = First + ((Last - First)>>1);
			if( First == Half || Last == Half )
			   return FindNearistStrikePx( Last, Px, StrikePxStep );
		}
	}
	else
		return 0;
}
//---------------------------------------------------------------------------
void TOptRecord::SetTradePx( double Px )
{
	FTradePx = Px;
	if( FIsUnderlying == true ) ///< Is a underlying record.
	{
		FStrikePxAt = UnderlyingTradePxAt( Px ); ///< Update the underlying price index at FStrikePrices.
		FPrevStrikePxIndex = FStrikePxIndex;     ///< Keep the prev row index in grid.
		if( FStrikePrices.ItemCount() == 0 )     ///< Update the underlying price at which row in grid.
			FStrikePxIndex = 0;
		else
			FStrikePxIndex = FStrikePxAt + FRowIndex + 1;
	}
}
//---------------------------------------------------------------------------
void TOptRecord::SetOpenInterest( int Qty )
{
	if( FOpenInterest == 0  )
	{
		FOpenInterestDiff = 0;
		FOpenInterest = Qty;
	}
	else
	{
		FOpenInterestDiff = Qty - FOpenInterest;
		FOpenInterest = Qty;
	}
}
//---------------------------------------------------------------------------
bool TOptRecord::IsUnderlyingStrikePx( int RowIndex )
{
	if( FStrikePxIndex == 0 )
	   return false;
	else if( RowIndex  == FStrikePxIndex )
	   return true;
	return false;
}
//---------------------------------------------------------------------------
bool TOptRecord::IsUpeerLimit( double Px )
{
	if( UFC::DoubleToInt( Px, 4 ) == FBullPx )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool TOptRecord::IsLowerLimit( double Px )
{
	if( UFC::DoubleToInt( Px, 4 ) == FBearPx )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool TOptRecord::IsFlat( double Px )
{
	if( UFC::DoubleToInt( Px, 4 ) == FRefInt )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool TOptRecord::IsClimbed( double Px )
{
	if( Px > FRefPx )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool TOptRecord::IsFell( double Px )
{
	if( Px < FRefPx )
		return true;
	return false;
}
//---------------------------------------------------------------------------
void TOptRecord::UpdateGreeks( double TimeToClose )
{
	double OptTradePx          = FTradePx;
	double dExpiryDays         = FExpiryDays + TimeToClose;
	double dYearTradingDays    = YEAR_TRADING_DAYS;
	double dNoRiskInterestRate = 0.05; // 無風險利率
	double SpotPx;

	if( Underlying != NULL )
	{
		try
		{
		   SpotPx = Underlying->GetTradePx();
		   if( SpotPx == 0 )
			   SpotPx = Underlying->GetRefPx();
		   if( OptTradePx == 0 )
			   OptTradePx = FRefPx;
		   OPTGreeks::GetOPTGreeks( FGreeks, this->GetCallPut(), SpotPx, OptTradePx, FStrikePx,
									dExpiryDays, dYearTradingDays, FHistoryVol, dNoRiskInterestRate);
		}
		catch(...){	} ///< Math error
	}
}
//---------------------------------------------------------------------------
double TOptRecord::GetBKBarValue( TOptBKColName Field )
{
	switch( Field )
	{
		case obTOTAL_QTY:     return (double)FTotalQty;
		case obOPEN_INTEREST: return (double)FOpenInterest;
		case obIMP_VAR:       return FGreeks.dImpv;
	}
	return 0.0;
}
//---------------------------------------------------------------------------
void  TOptRecord::UpdateValueWidth( TOptBKColName Field, double MaxVal, int TotalWidth )
{
	double Rate;//  = 0.0;
	double Value = GetBKBarValue( Field );

	if( Value <= 0 )
		Rate = 0.0;
	else if( Value >= MaxVal )
		Rate = 1.0;
	else
		Rate = Value/MaxVal;
	FValueWidth = (int)(Rate*(double)TotalWidth);
}
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TOptionsStrikePriceView *)
{
	new TOptionsStrikePriceView(NULL);
}
//---------------------------------------------------------------------------
//
// class TOptionsStrikePriceView
// Options TBar component for C++ Builder
//
//---------------------------------------------------------------------------
__fastcall TOptionsStrikePriceView::TOptionsStrikePriceView(TComponent* Owner)
:TCustomGrid(Owner)
,FSymbol( L"TXO" )
,FExchange( L"TAIFEX" )
,FYearMonth( L"200910" )
,FDisplayName( L"台指期 TXO" )
,FIsLoaded( false )
,FHeaderCallColor( clBlack )
,FHeaderCallBKColor( clSkyBlue )
,FHeaderPutColor( clBlack )
,FHeaderPutBKColor( clMoneyGreen )
,FHeaderCommodColor( clWhite )
,FHeaderCommodBKColor( clHighlight )
,FFieldNameCallColor( clWhite )
,FFieldNameCallBKColor( clDefault )
,FFieldNamePutColor( clWhite )
,FFieldNamePutBKColor( clDefault )
,FFieldNameCommodColor( clWhite )
,FFieldNameCommodBKColor( clGray )
,FCallBKColor( clBlack )///< Call side background color
,FPutBKColor( clBlack )///< Put side background color
,FStkPxColor( clWhite )
,FStkPxBKColor( clHighlight )
,FRisePxColor( clRed )   ///< Price go rise
,FFallPxColor( clGreen ) ///< Price to fall
,FEqualPxColor( clGray ) ///< Same price.
,FHGridColor( clBlack ) ///< H Grid color
,FVGridColor( clBlack ) ///< V Grid color
,FBKBarColor( clOlive )
,FIsReverse( false )
,FStore( NULL )
,FBestFitWidth( 0 )
,FOnTBarMouseDown( NULL )
,FOnTBarMouseClick( NULL )
,FOnTBarStrikePxMouseDown( NULL )
,FBKCol( obOPEN_INTEREST )
,FStrikePxColIndex( 8 )
,FLastColColIndex( 7 )
,FFieldsString( "BuyPx^SellPx^TradePx^DiffPx^TradeQty^TotalQty^HighPx^LowPx" )
,FTimeToClose( 0 )
,FTotalWidth( 100 )
,FBKValueMax( 1.0 )
{
	TGridOptions OptionSet;

	Options    = OptionSet;
	FixedCols  = 0;
	FixedRows  = 2;
	Font->Size = 12;
	FMouseCoord.X = -1;
	FMouseCoord.Y = -1;
	EditorMode     = false;
	DefaultDrawing = false;
	EditorMode     = false;
	OnMouseLeave     = MouseLeave;
	OnMouseEnter     = MouseEnter;
	OnMouseWheelDown = MyMouseWheelDown;
	OnMouseWheelUp   = MyMouseWheelUp;

	FBufferBmp        = new Graphics::TBitmap();
	FAlphaBmp         = new Graphics::TBitmap();
	FCallList         = new TList();
	FPutList          = new TList();
	FStrikePriceList  = new TStringList();
	FHeaderRowText    = new TStringList();
	HeaderString      = new TStringList();
	FTimer            = new TTimer( this );
	FTimer->Interval  = 60000;
	FTimer->OnTimer   = TradetimeTimer;
	FTimer->Enabled   = false;
	FHeaderRowText->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_CALL );
	FHeaderRowText->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_PUT );
	PopupMenu = new TPopupMenu(this);
	PopupMenu->AutoPopup = true;

	InitFieldNameValue( );
	InitGrid();
}
//---------------------------------------------------------------------------
__fastcall TOptionsStrikePriceView::~TOptionsStrikePriceView()
{
	FTimer->Enabled = false;
	Clear( );
	delete FTimer;
	delete FBufferBmp;
	delete FAlphaBmp;
	delete FCallList;
	delete FPutList;
	delete FStrikePriceList;
	delete FHeaderRowText;
	delete HeaderString;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::InitFieldNameValue( )
{
	///< String to Field Type Enum.
	FColNameColTypeMap.Add( "BuyPx",        BUY_PX );
	FColNameColTypeMap.Add( "SellPx",       SELL_PX );
	FColNameColTypeMap.Add( "TradePx",      TRADE_PX );
	FColNameColTypeMap.Add( "DiffPx",       DIFF_PX );
	FColNameColTypeMap.Add( "TradeQty",     TRADE_QTY );
	FColNameColTypeMap.Add( "TotalQty",     TOTAL_QTY );
	FColNameColTypeMap.Add( "HighPx",       HIGH_PX );
	FColNameColTypeMap.Add( "LowPx",        LOW_PX );
	FColNameColTypeMap.Add( "SettlementPx", SETTLEMENT_PX );
	FColNameColTypeMap.Add( "OpenInterest", OPEN_INTEREST );
	FColNameColTypeMap.Add( "Delta",        DELTA );
	FColNameColTypeMap.Add( "Gamma",        GAMMA );
	FColNameColTypeMap.Add( "Theta",        THETA );
	FColNameColTypeMap.Add( "Vega",         VEGA  );
	FColNameColTypeMap.Add( "Rho",          RHO );
	FColNameColTypeMap.Add( "StrikePx",     STRIKE_PRICE );

	FColNameColTypeMap.Add( "DiffOI",       DIFF_OI  );
	FColNameColTypeMap.Add( "BuyQty",       BUY_QTY  );
	FColNameColTypeMap.Add( "SellQty",      SELL_QTY );
	FColNameColTypeMap.Add( "ImpVar",       IMP_VAR );
	FColNameColTypeMap.Add( "TheoryPX",     THEORY_PX );
	///< Index to Field Name string
	if( gOptFieldsDisplayName == NULL )
	{
		gOptFieldsDisplayName = new TStringList();
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_BIDPX );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_ASKPX );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_FILLPX );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_UPDOWNPX );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_QTY );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_TOTALQTY );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_HIGH );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_LOW );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_SETTLEMENT_PX );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_OPEN_INTEREST );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_DELTA );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_GAMMA );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_THETA );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_VEGA );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_RHO );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_STRIKEPX );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_DIFF_OI );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_BUY_QTY );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_SELL_QTY );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_IMP_VAR );
		gOptFieldsDisplayName->Add( Mdcomponentstrings_MD_OPTIONS_TBAR_THEORY_PX );
	}
	for( register int i = 0; i < gOptFieldsDisplayName->Count; i++ )
	{
		TMenuItem* Item = new TMenuItem(PopupMenu);
		PopupMenu->Items->Add( Item );
		Item->Caption = gOptFieldsDisplayName->Strings[i];
		Item->Tag = i;
		Item->Checked = false;
		Item->OnClick = MenuClicked;
	}
}
//---------------------------------------------------------------------------
int __fastcall TOptionsStrikePriceView::GetMaxWidth( optStkColName Type )
{
	switch( Type )
	{
		case STRIKE_PRICE: 	return StrikePxFieldWidth();
		case DIFF_PX:       return FBufferBmp->Canvas->TextWidth( L"9999.999" ) + 4;
		case BUY_PX:
		case SELL_PX:
		case TRADE_PX:
		case HIGH_PX:
		case LOW_PX:
		case THEORY_PX:
		case SETTLEMENT_PX: return FBufferBmp->Canvas->TextWidth( L"9999.999" ) + 4;
		case TRADE_QTY:
		case BUY_QTY:
		case SELL_QTY:
		case DIFF_OI:       return FBufferBmp->Canvas->TextWidth( L"9999" ) + 4;
		case TOTAL_QTY:
		case OPEN_INTEREST: return FBufferBmp->Canvas->TextWidth( L"9999999" ) + 4;
		case IMP_VAR:       return FBufferBmp->Canvas->TextWidth( L"999.99%" ) + 4;
		case DELTA:
		case GAMMA:
		case THETA:
		case VEGA:
		case RHO:           return FBufferBmp->Canvas->TextWidth( L"-9.9999" ) + 8;
		default: break;
	}
	return FBufferBmp->Canvas->TextWidth( L"9999.999" ) + 4;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetPopupMenuChecked( bool Checked )
{
	for( register int i = 0; i < PopupMenu->Items->Count; i++ )
		 PopupMenu->Items->Items[i]->Checked = Checked;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::MenuClicked( TObject* Sender )
{
	TMenuItem* ClickedItem = dynamic_cast<TMenuItem*>(Sender);
	if( ClickedItem != NULL )
	{
		UFC::PStringList CheckedFields;

		ClickedItem->Checked = !ClickedItem->Checked;
		for( register int i = 0; i < PopupMenu->Items->Count; i++ )
		{
			 if( PopupMenu->Items->Items[ i ]->Checked == true )
				 CheckedFields.Add( gOptFieldsTypeName[ i ] );
		}
		FFieldsString = CheckedFields.DelimiterString( '^' );
		InitGrid();
		CalSize();
		Invalidate();
		if( FOnTBarFieldsChanged != NULL )
			FOnTBarFieldsChanged( this );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::InitGrid( void )
{
	UFC::PStringList Fields;
	OptStkColName    FieldType;

	Fields.SetStrings( FFieldsString, "^\n" );
	FCallFieldTypeMap.Clear();
	FPutFieldTypeMap.Clear();
	HeaderString->Clear();
	SetPopupMenuChecked( false );
	///< Remove invalid field name.
	for( int i = Fields.ItemCount()-1; i >= 0; i-- )
	{
		if( FColNameColTypeMap.GetObjectByKey( Fields[ i ] , FieldType ) == FALSE )
			Fields.Delete( i );
	}
	///< Build call column.
	for( int i = 0; i < Fields.ItemCount(); i ++ )
	{
		int index = i;
		if( FIsReverse == true )
			index =  Fields.ItemCount() - 1 - i;
		FColNameColTypeMap.GetObjectByKey( Fields[index] , FieldType );
		FFieldTypes[ i ] = FieldType;
		HeaderString->Add( gOptFieldsDisplayName->Strings[(int)FieldType] );
		PopupMenu->Items->Items[(int)FieldType]->Checked = true;
		FCallFieldTypeMap.Add( FieldType , i );
	}
	///< Build strike price column.
	FStrikePxColIndex = HeaderString->Count;
	FLastColColIndex  = FStrikePxColIndex -1;
	FFieldTypes[ FStrikePxColIndex ] = STRIKE_PRICE;
	HeaderString->Add( gOptFieldsDisplayName->Strings[(int)STRIKE_PRICE] );
	///< Build put column.
	for( int i = 0; i < Fields.ItemCount() ; i ++ )
	{
		 FColNameColTypeMap.GetObjectByKey( Fields[i] , FieldType );
		 FFieldTypes[ FStrikePxColIndex + 1 + i] = FieldType;
		 HeaderString->Add( gOptFieldsDisplayName->Strings[(int)FieldType] );
		 FPutFieldTypeMap.Add( FieldType , FStrikePxColIndex + 1 + i );
	}
	ColCount     = HeaderString->Count;
	FCallTextCol = Fields.ItemCount()/2;
	FPutTextCol  = HeaderString->Count - 1 - FCallTextCol;
	FixedRows    = 2;
}
//---------------------------------------------------------------------------
int __fastcall TOptionsStrikePriceView::StrikePxFieldWidth( void )
{
	int NameWidth    = FBufferBmp->Canvas->TextWidth( FDisplayName ) + 8;
	int CaptionWidth = FBufferBmp->Canvas->TextWidth( Mdcomponentstrings_MD_OPTIONS_TBAR_STRIKEPX ) + 8;
	if( NameWidth >  CaptionWidth )
		return NameWidth;
	return CaptionWidth;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::CalSize( void )
{
	FBufferBmp->Canvas->Font->Assign( Font );
	FBestFitWidth = 0;
	FTotalWidth = 0;
	FStrikePxWidth = ColWidths[ FStrikePxColIndex ];

	for( register int i = 0; i < ColCount; i++  )
	{
		ColWidths[ i ] = GetMaxWidth( FFieldTypes[ i ] );
		FBestFitWidth += ColWidths[i];
		if( i < FStrikePxColIndex )
			FTotalWidth += ColWidths[i];
	}
	FBestFitWidth += GetSystemMetrics( SM_CXVSCROLL );
	DefaultRowHeight = FBufferBmp->Canvas->TextHeight( L"9" ) + 6;
	for( register int i = 0; i < FCallList->Count; i++  )
	{
		TOptRecord* Rec =	(TOptRecord*)(FCallList->Items[i]);
		if( Rec != NULL && Rec->IsUnderlying() == true )
			UpdateUnderlyingBuffer( Rec );
	}
	UpdateBKBar( false );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::UpdateBKBar( bool FindMax )
{
	double      CurrVal;
	TOptRecord* Rec;

	if( FindMax == true )
	{
		FBKValueMax = 0.0;
		for( register int i = 0; i < FCallList->Count; i++  )
		{
			if( (Rec = (TOptRecord*)(FCallList->Items[i])) != NULL && Rec->IsUnderlying() == false )
			{
				CurrVal = Rec->GetBKBarValue( FBKCol );
				if( CurrVal > FBKValueMax )
					FBKValueMax = CurrVal;
			}
		}
		for( register int i = 0; i < FPutList->Count; i++  )
		{
			if( (Rec = (TOptRecord*)(FPutList->Items[i])) != NULL && Rec->IsUnderlying() == false )
			{
				CurrVal = Rec->GetBKBarValue( FBKCol );
				if( CurrVal > FBKValueMax )
					FBKValueMax = CurrVal;
			}
		}
	}
	for( register int i = 0; i < FCallList->Count; i++  )
	{
		if( (Rec = (TOptRecord*)(FCallList->Items[i])) != NULL && Rec->IsUnderlying() == false )
			Rec->UpdateValueWidth( FBKCol, FBKValueMax, FTotalWidth );
	}
	for( register int i = 0; i < FPutList->Count; i++  )
	{
		if( (Rec = (TOptRecord*)(FPutList->Items[i])) != NULL && Rec->IsUnderlying() == false )
			Rec->UpdateValueWidth( FBKCol, FBKValueMax, FTotalWidth );
	}
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetSymbol( UnicodeString Symbol )
{
	FSymbol = Symbol;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetExchange( UnicodeString Exchange )
{
	FExchange = Exchange;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetYearMonth( UnicodeString YearMonth )
{
	FYearMonth = YearMonth;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetDisplayName( UnicodeString Name )
{
	FDisplayName = Name;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetMarketDataStore( TCMarketDataStore* Store )
{
	if( FStore !=  Store)
		FStore = Store;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::UpdateCell( int ColIndex, int RowIndex )
{
	if( ColIndex < ColCount && ColIndex >= 0  && RowIndex < RowCount && RowIndex >= 0 )
	{
		TRect UpdateRect = CellRect( ColIndex,  RowIndex );

		DrawCell( ColIndex, RowIndex , UpdateRect, (TGridDrawState)0 );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::UpdateRow( int RowIndex )
{
	for( register int i = 0; i < ColCount; i++ )
		 UpdateCell( i, RowIndex );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::UpdateCallRow( int RowIndex )
{
	for( register int i = 0; i < FStrikePxColIndex; i++ )
		 UpdateCell( i, RowIndex );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::UpdatePutRow( int RowIndex )
{
	for( register int i = FStrikePxColIndex + 1; i < ColCount; i++ )
		 UpdateCell( i, RowIndex );
}
//---------------------------------------------------------------------------
//  Receive full SnapShot
//  Update all fields of this Symbol.
//---------------------------------------------------------------------------
void TOptionsStrikePriceView::OnMarketDataUpdate( MarketDataMessage* Msg )
{
	TOptRecord* Rec = FTable.GetObjectByKey( Msg->GetSymbol() );
	if( Rec != NULL )
	{
		int RowIndex;

		if( Rec->IsUnderlying() == false )
		{
			Rec->SetBidPx( Msg->GetBuyPrice1() );
			Rec->SetAskPx( Msg->GetSellPrice1() );
			Rec->SetBidQty( Msg->GetBuyQty1() );
			Rec->SetAskQty( Msg->GetSellQty1() );
			Rec->SetTradePx( Msg->GetTradePrice() );
			Rec->SetTradeQty( Msg->GetTradeQty() );
			Rec->SetTotalQty( Msg->GetTradeVolume() );
			Rec->SetHighPx( Msg->GetDayHighPrice() );
			Rec->SetLowPx( Msg->GetDayLowPrice() );
			Rec->SetOpenInterest( Msg->GetOpenInterest() );
			Rec->UpdateGreeks( FTimeToClose );
			if( Rec->GetCallPut() == cpCall )
			{
				RowIndex = FCallList->IndexOf( Rec );
				if( RowIndex >= 0 )
					UpdateCallRow( RowIndex + 2 );
			}
			else
			{
				RowIndex = FPutList->IndexOf( Rec );
				if( RowIndex >= 0 )
					UpdatePutRow( RowIndex + 2 );
			}
		}
		else
		{
			TOptRecord* CurrRec = Rec;

			while( CurrRec != NULL )
			{
				CurrRec->SetBidPx( Msg->GetBuyPrice1() );
				CurrRec->SetAskPx( Msg->GetSellPrice1() );
				if( Msg->GetTradePrice() == 0.0 || Msg->GetTradeQty() == 0 )
					CurrRec->SetTradePx( Msg->GetRefPx() );
				else
					CurrRec->SetTradePx( Msg->GetTradePrice() );
				CurrRec->SetTradeQty( Msg->GetTradeQty() );
				CurrRec->SetTotalQty( Msg->GetTradeVolume() );
				CurrRec->SetHighPx( Msg->GetDayHighPrice() );
				CurrRec->SetLowPx( Msg->GetDayLowPrice() );
				UpdateUnderlyingBuffer( CurrRec  );
				UpdateRow( CurrRec->GetRowIndex() );
				int Index = CurrRec->GetUnderlyingStrikePxIndex( );
				int PrevIndex = CurrRec->GetPrevUnderlyingStrikePxIndex( );
				if( Index != 0 )
					UpdateCell( FStrikePxColIndex, Index );
				if( Index != PrevIndex )
					UpdateCell( FStrikePxColIndex, PrevIndex );
				CurrRec = CurrRec->Next;
			}
		}
		FResponseCount++;
		if( FResponseCount >= FSubsceibeSymbolList.ItemCount() )
		{
			FResponseCount = 0;
			UpdateBKBar( true );
        }
	}
}
//---------------------------------------------------------------------------
//  Receive Match
//  Update TradePx,TradeQty,DiffPx.
//---------------------------------------------------------------------------
void TOptionsStrikePriceView::OnMarketDataUpdate( MatchInfo* Msg )
{
	TOptRecord* Rec = FTable.GetObjectByKey( Msg->GetSymbol() );
	if( Rec != NULL )
	{
		int RowIndex,ColIndex;
		bool IsUpdateRow = false;
		UFC::PHashMap<OptStkColName, int >*  FieldTypeMap;

		if( Rec->IsUnderlying() == false )
		{
			Rec->SetTradePx( Msg->GetMatchPrice() );
			Rec->SetTradeQty( Msg->GetMatchQty() );
			Rec->UpdateGreeks( FTimeToClose );
			if( FBKCol == obIMP_VAR )
			{
				if( Rec->GetGreeks().dImpv > FBKValueMax )
				{
					FBKValueMax = Rec->GetBKBarValue( FBKCol );
					UpdateBKBar( false );
					return;
				}
				else
				{
					Rec->UpdateValueWidth( FBKCol, FBKValueMax, FTotalWidth );
					IsUpdateRow = true;
				}
			}
			if( Rec->GetCallPut() == cpCall )
			{
				RowIndex = FCallList->IndexOf( Rec );
				FieldTypeMap = &FCallFieldTypeMap;
			}
			else
			{
				RowIndex = FPutList->IndexOf( Rec );
				FieldTypeMap = &FPutFieldTypeMap;
			}
			if( RowIndex >= 0 )
			{
				RowIndex += 2;
				if( IsUpdateRow == true )
				{
					if( Rec->GetCallPut() == cpCall )
						UpdateCallRow( RowIndex );
					else
						UpdatePutRow( RowIndex );
				}
				else
				{
					if( FieldTypeMap->GetObjectByKey( TRADE_PX, ColIndex ) == true )
						UpdateCell( ColIndex, RowIndex );
					if( FieldTypeMap->GetObjectByKey( TRADE_QTY, ColIndex ) == true )
						UpdateCell( ColIndex, RowIndex );
					if( FieldTypeMap->GetObjectByKey( DIFF_PX, ColIndex ) == true )
						UpdateCell( ColIndex, RowIndex );
				}
			}
		}
		else
		{
			TOptRecord* CurrRec = Rec;

			while( CurrRec != NULL )
			{
				CurrRec->SetTradePx( Msg->GetMatchPrice() );
				CurrRec->SetTradeQty( Msg->GetMatchQty() );
				UpdateUnderlyingBuffer( CurrRec  );
				UpdateRow( CurrRec->GetRowIndex() );
				int Index = CurrRec->GetUnderlyingStrikePxIndex( );
				int PrevIndex = CurrRec->GetPrevUnderlyingStrikePxIndex( );
				if( Index != 0 )
					UpdateCell( FStrikePxColIndex, Index );
				if( Index != PrevIndex )
					UpdateCell( FStrikePxColIndex, PrevIndex );
				CurrRec = CurrRec->Next;
			}
		}
	}
}
//---------------------------------------------------------------------------
//  Receive Total Match
//  Update TotalQty.
//---------------------------------------------------------------------------
void TOptionsStrikePriceView::OnMarketDataUpdate( TotalMatch* Msg )
{
	TOptRecord* Rec = FTable.GetObjectByKey( Msg->GetSymbol() );
	if( Rec != NULL )
	{
		int RowIndex,ColIndex;
		bool IsUpdateRow = false;
		UFC::PHashMap<OptStkColName, int >*  FieldTypeMap;

		Rec->SetTotalQty( Msg->GetTotalMatchQty() );
		if( FBKCol == obTOTAL_QTY && Rec->IsUnderlying() == false )
		{
			if( (double)Msg->GetTotalMatchQty() > FBKValueMax )
			{
				FBKValueMax = Rec->GetBKBarValue( FBKCol );
				UpdateBKBar( false );
				return;
			}
			else
			{
				IsUpdateRow = true;
				Rec->UpdateValueWidth( FBKCol, FBKValueMax, FTotalWidth );
			}
		}
		if( Rec->GetCallPut() == cpCall )
		{
			RowIndex = FCallList->IndexOf( Rec );
			FieldTypeMap = &FCallFieldTypeMap;
		}
		else
		{
			RowIndex = FPutList->IndexOf( Rec );
			FieldTypeMap = &FPutFieldTypeMap;
		}
		if( RowIndex >= 0 )
		{
			RowIndex += 2;
			if( IsUpdateRow == true )
			{
				if( Rec->GetCallPut() == cpCall )
					UpdateCallRow( RowIndex );
				else
					UpdatePutRow( RowIndex );
			}
			else
			{
				if( FieldTypeMap->GetObjectByKey( TOTAL_QTY, ColIndex ) == true )
					UpdateCell( ColIndex, RowIndex );
			}
		}
	}
}
//---------------------------------------------------------------------------
//  Receive Order book
//  Update BuyPx,SellPx.
//---------------------------------------------------------------------------
void TOptionsStrikePriceView::OnMarketDataUpdate( OrderBookData* Msg )
{
	TOptRecord* Rec = FTable.GetObjectByKey( Msg->GetSymbol() );
	if( Rec != NULL )
	{
		int RowIndex,ColIndex;
		UFC::PHashMap<OptStkColName, int >*  FieldTypeMap;

		Rec->SetBidPx( Msg->GetBuyPrice1() );
		Rec->SetBidQty( Msg->GetBuyQty1() );
		Rec->SetAskPx( Msg->GetSellPrice1() );
		Rec->SetAskQty( Msg->GetSellQty1() );
		if( Rec->GetCallPut() == cpCall )
		{
			RowIndex = FCallList->IndexOf( Rec );
			FieldTypeMap = &FCallFieldTypeMap;
		}
		else
		{
			RowIndex = FPutList->IndexOf( Rec );
			FieldTypeMap = &FPutFieldTypeMap;
		}
		if( RowIndex >= 0 )
		{
			RowIndex += 2;
			if( FieldTypeMap->GetObjectByKey( BUY_PX, ColIndex ) == true )
				UpdateCell( ColIndex, RowIndex );
			if( FieldTypeMap->GetObjectByKey( SELL_PX, ColIndex ) == true )
				UpdateCell( ColIndex, RowIndex );
			if( FieldTypeMap->GetObjectByKey( BUY_QTY, ColIndex ) == true )
				UpdateCell( ColIndex, RowIndex );
			if( FieldTypeMap->GetObjectByKey( SELL_QTY, ColIndex ) == true )
				UpdateCell( ColIndex, RowIndex );
		}
	}
}
//---------------------------------------------------------------------------
//  Receive Day High/Low price
//  Update BuyPx,SellPx.
//---------------------------------------------------------------------------
void TOptionsStrikePriceView::OnMarketDataUpdate( DayHighLowPrice* Msg )
{
	TOptRecord* Rec = FTable.GetObjectByKey( Msg->GetSymbol() );
	if( Rec != NULL )
	{
		int RowIndex,ColIndex;
		UFC::PHashMap<OptStkColName, int >*  FieldTypeMap;

		Rec->SetHighPx( Msg->GetDayHighPrice() );
		Rec->SetLowPx( Msg->GetDayLowPrice() );
		if( Rec->GetCallPut() == cpCall )
		{
			RowIndex = FCallList->IndexOf( Rec );
			FieldTypeMap = &FCallFieldTypeMap;
		}
		else
		{
			RowIndex = FPutList->IndexOf( Rec );
			FieldTypeMap = &FPutFieldTypeMap;
		}
		if( RowIndex >= 0 )
		{
			RowIndex += 2;
			if( FieldTypeMap->GetObjectByKey( HIGH_PX, ColIndex ) == true )
				UpdateCell( ColIndex, RowIndex );
			if( FieldTypeMap->GetObjectByKey( LOW_PX, ColIndex ) == true )
				UpdateCell( ColIndex, RowIndex );
		}
	}
}
//---------------------------------------------------------------------------
//  Receive Day Close/Settlement/Open Interest price
//  Update Open Interest.
//---------------------------------------------------------------------------
void TOptionsStrikePriceView::OnMarketDataUpdate( ClosingMarketData* Msg )
{
	TOptRecord* Rec = FTable.GetObjectByKey( Msg->GetSymbol() );
	if( Rec != NULL )
	{
		int  RowIndex,ColIndex;
		bool IsUpdateRow = false;
		UFC::PHashMap<OptStkColName, int >*  FieldTypeMap;

		if( Msg->GetMsgType() ==  mtClosingMarketDataWithSettlementPriceAndOpenInterest )
		{
			Rec->SetOpenInterest( ( ( ClosingMarketDataWithSettlementPriceAndOpenInterest*)Msg)->GetOpenInterest() );
			if( FBKCol == obOPEN_INTEREST )
			{
				if( (double)Rec->GetOpenInterest() > FBKValueMax )
				{
					FBKValueMax = Rec->GetBKBarValue( FBKCol );
					UpdateBKBar( false );
					return;
				}
				else
				{
					IsUpdateRow = true;
					Rec->UpdateValueWidth( FBKCol, FBKValueMax, FTotalWidth );
				}
			}
			if( Rec->GetCallPut() == cpCall )
			{
				RowIndex = FCallList->IndexOf( Rec );
				FieldTypeMap = &FCallFieldTypeMap;
			}
			else
			{
				RowIndex = FPutList->IndexOf( Rec );
				FieldTypeMap = &FPutFieldTypeMap;
			}
			if( RowIndex >= 0 )
			{
				RowIndex += 2;
				if( IsUpdateRow == true )
				{
					if( Rec->GetCallPut() == cpCall )
						UpdateCallRow( RowIndex );
					else
						UpdatePutRow( RowIndex );
				}
				else
				{
					if( FieldTypeMap->GetObjectByKey( OPEN_INTEREST, ColIndex ) == true )
						UpdateCell( ColIndex, RowIndex );
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void TOptionsStrikePriceView::OnMarketDataUpdate( BasicData* Msg ) {}
//---------------------------------------------------------------------------
void TOptionsStrikePriceView::OnMarketDataUpdate( OpeningInfo* Msg ) {}
//---------------------------------------------------------------------------
void TOptionsStrikePriceView::OnMarketDataUpdate( UnderlyingIndexInfo* Msg ){}
//---------------------------------------------------------------------------
void TOptionsStrikePriceView::OnMarketDataUpdate( SumOfOrderInfo* Msg ){}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::UpdateUnderlyingBuffer( TOptRecord* UnderlyingRec  )
{
	TRect  PaintRect = Rect( 0, 0, FBestFitWidth, DefaultRowHeight );
	String DrawText,TabText;
	double Px,DiffPx;
	int    FieldLeft, TabWidth;
	Graphics::TBitmap* Buffer = UnderlyingRec->GetBuffer();

	if( Buffer != NULL )
	{
		TColor BKColor   = FCallBKColor;
		TColor TXTColor  = GDIUtility::ComplementaryColor( FCallBKColor );
		TColor LineColor = FHGridColor;

		if( Buffer->Width != FBestFitWidth || Buffer->Height != DefaultRowHeight )
		{
			Buffer->Width  = FBestFitWidth;
			Buffer->Height = DefaultRowHeight;
		}
		Buffer->Canvas->Font->Assign( Font );
		Buffer->Canvas->Brush->Color = BKColor;//FHeaderCallBKColor;
		Buffer->Canvas->Pen->Color   = LineColor;//clWhite;
		Buffer->Canvas->Pen->Width   = 2;
		Buffer->Canvas->Font->Color  = TXTColor;//FHeaderCallColor;
		///< Get Text width
		TabText.printf( L"%s[%s] %s %.*f 漲 999  ",UnderlyingRec->GetYearMonth(), UnderlyingRec->GetDisplayName(), Mdcomponentstrings_MD_OPTIONS_TBAR_FILLPX, UnderlyingRec->GetDigit(), Px );
		FieldLeft = Buffer->Canvas->TextWidth( UnderlyingRec->GetYearMonth());
		TabWidth  = Buffer->Canvas->TextWidth( TabText );
		///< Draw background
		GDIUtility::DrawGradientBar( Buffer->Canvas, PaintRect, FHGridColor, FHGridColor, true );
		//GDIUtility::DrawGradientBar( Buffer->Canvas, PaintRect, BKColor, TXTColor, true );
		//GDIUtility::DrawGradientBar( Buffer->Canvas, PaintRect, FCallBKColor, FHeaderCallBKColor, true );
		Buffer->Canvas->MoveTo( 0, Buffer->Height -1 );
		Buffer->Canvas->LineTo( Buffer->Width, Buffer->Height -1 );
		///< Draw Year Month tab
		Buffer->Canvas->Brush->Color = BKColor;//FCallBKColor;
		Buffer->Canvas->RoundRect( 10, 3, 30 + TabWidth + DefaultRowHeight + 60 /*FieldLeft*/, DefaultRowHeight + 5, 5, 5 );
		Buffer->Canvas->TextOut( 20, 4, UnderlyingRec->GetYearMonth() );
		Buffer->Canvas->Pen->Width  = 1;
		///< Draw Display name
		FieldLeft += 35;
		DrawText.printf( L"[%s]", UnderlyingRec->GetDisplayName() );
		Buffer->Canvas->Brush->Style = bsClear;
		Buffer->Canvas->Font->Color  = TXTColor;//FHeaderCommodColor;
		Buffer->Canvas->TextOut( FieldLeft, 4, 	DrawText );
		///< Draw Trade Px text
		FieldLeft += Buffer->Canvas->TextWidth( DrawText )+5;
		Buffer->Canvas->TextOut( FieldLeft, 4, 	Mdcomponentstrings_MD_OPTIONS_TBAR_FILLPX );
		///< Draw Trade Px
		FieldLeft += Buffer->Canvas->TextWidth( Mdcomponentstrings_MD_OPTIONS_TBAR_FILLPX )+5;
		if( UnderlyingRec->GetTotalQty() > 0 )
			Px = UnderlyingRec->GetTradePx();
		else
			Px = UnderlyingRec->GetSettlementPx();
		DrawText.printf( L"%.*f", UnderlyingRec->GetDigit(), Px );
		if( UnderlyingRec->IsUpeerLimit( Px ) ) ///< Bull Price
		{
			Buffer->Canvas->Brush->Color = FRisePxColor;
			Buffer->Canvas->Font->Color  = clWhite;
		}
		else if( UnderlyingRec->IsLowerLimit( Px ) ) ///< Bear Price
		{
			Buffer->Canvas->Brush->Color = FFallPxColor;
			Buffer->Canvas->Font->Color  = clWhite;
		}
		else ///< Between Bull/Bear Price.
		{
			Buffer->Canvas->Brush->Style = bsClear;
			if( UnderlyingRec->IsFlat( Px ) )
				Buffer->Canvas->Font->Color  = FEqualPxColor;
			else if( UnderlyingRec->IsClimbed( Px ) )
				Buffer->Canvas->Font->Color  = FRisePxColor;
			else
				Buffer->Canvas->Font->Color  = FFallPxColor;
		}
		Buffer->Canvas->TextOut( FieldLeft, 4, 	DrawText );
		///< Draw Rectangle.
		FieldLeft += Buffer->Canvas->TextWidth( DrawText )+ 15;
		DiffPx = Px - UnderlyingRec->GetRefPx();
		if( DiffPx > 0 )
		{
			Buffer->Canvas->Brush->Color = FRisePxColor;
			Buffer->Canvas->Pen->Color   = FRisePxColor;
			TPoint  TriPoint[4] = {TPoint(FieldLeft + ( DefaultRowHeight - 10 )/2, 5),
								   TPoint(FieldLeft , DefaultRowHeight - 5),
								   TPoint(FieldLeft + (DefaultRowHeight - 10), DefaultRowHeight - 5 ),
								   TPoint(FieldLeft + (DefaultRowHeight - 10)/2, 5)};
			Buffer->Canvas->Polygon( TriPoint, 3 );
		}
		else if( DiffPx < 0 )
		{
			Buffer->Canvas->Brush->Color = FFallPxColor;
			Buffer->Canvas->Pen->Color   = FFallPxColor;
			TPoint  TriPoint[4] = {TPoint(FieldLeft,  5),
								   TPoint(FieldLeft+(DefaultRowHeight - 10), 5),
								   TPoint(FieldLeft+(DefaultRowHeight - 10)/2, DefaultRowHeight-5),
								   TPoint(FieldLeft, 5)};
			Buffer->Canvas->Polygon( TriPoint, 3 );
			DiffPx *= -1;
		}
		///< Draw Price different
		FieldLeft += DefaultRowHeight;
		Buffer->Canvas->Brush->Style = bsClear;
		DrawText.printf( L"%.*f", UnderlyingRec->GetDigit(), DiffPx );
		Buffer->Canvas->TextOut( FieldLeft, 4, DrawText );
	}
}
//---------------------------------------------------------------------------
void _fastcall TOptionsStrikePriceView::DrawCell(int ACol, int ARow, const Types::TRect &ARect, TGridDrawState AState )
{
	if( FIsLoaded == false )
		return;
	if( ARow == 0 ) ///< Draw Header.( Call  ProductID  Put )
	{
		DrawHeader( ACol, ARect, AState );
	}
	else if( ARow == 1 ) ///< Draw Col name.
	{
		DrawFieldName( ACol, ARect, AState );
	}
	else
	{
		if( FCallList->Count > 0 && FPutList->Count > 0 )
		{
			CallPutCode CallPut;
			TOptRecord* DrawRec = (TOptRecord*)(FCallList->Items[ ARow - 2 ]);

			if( DrawRec->IsUnderlying( ) == false )
			{
				if( ACol < FStrikePxColIndex )
					CallPut = cpCall;
				else
					CallPut = cpPut;
				if( FMouseCoord.X == ACol && FMouseCoord.Y == ARow )
					AState << gdHotTrack;
				else
					AState >> gdHotTrack;
				switch( FFieldTypes[ ACol ] )
				{
					case STRIKE_PRICE: 	DrawStrikePrice( ARow, ARect, AState ); 		          break;
					case DIFF_PX:       DrawDiffPx( ARow, ARect, AState, CallPut );         	  break;
					case IMP_VAR:       DrawImpVar( ARow, ARect, AState, CallPut );               break;
					///< Price type
					case BUY_PX:		DrawPxField( ARow, ARect, AState, CallPut, BUY_PX); 	  break;
					case SELL_PX:       DrawPxField( ARow, ARect, AState, CallPut, SELL_PX); 	  break;
					case TRADE_PX:      DrawPxField( ARow, ARect, AState, CallPut, TRADE_PX, true ); 	  break;
					case HIGH_PX:       DrawPxField( ARow, ARect, AState, CallPut,HIGH_PX, true  ); 	  break;
					case LOW_PX:        DrawPxField( ARow, ARect, AState, CallPut,LOW_PX, true  ); 	  break;
					case SETTLEMENT_PX: DrawPxField( ARow, ARect, AState, CallPut,SETTLEMENT_PX );break;
					case THEORY_PX:     DrawPxField( ARow, ARect, AState, CallPut,THEORY_PX, true  );    break;
					///< Qty Type
					case TRADE_QTY:     DrawQtyField( ARow, ARect, AState, CallPut, TRADE_QTY );     break;
					case TOTAL_QTY:     DrawQtyField( ARow, ARect, AState, CallPut, TOTAL_QTY );     break;
					case BUY_QTY:       DrawQtyField( ARow, ARect, AState, CallPut, BUY_QTY ); 	     break;
					case SELL_QTY:      DrawQtyField( ARow, ARect, AState, CallPut, SELL_QTY );      break;
					case OPEN_INTEREST: DrawQtyField( ARow, ARect, AState, CallPut, OPEN_INTEREST ); break;
					case DIFF_OI:       DrawQtyField( ARow, ARect, AState, CallPut, DIFF_OI );       break;
					///< float type
					case DELTA:         DrawGeeksField( ARow, ARect, AState, CallPut, DELTA );       break;
					case GAMMA:         DrawGeeksField( ARow, ARect, AState, CallPut, GAMMA );       break;
					case THETA:         DrawGeeksField( ARow, ARect, AState, CallPut, THETA );       break;
					case VEGA:          DrawGeeksField( ARow, ARect, AState, CallPut, VEGA );        break;
					case RHO:           DrawGeeksField( ARow, ARect, AState, CallPut, RHO );         break;
					default: break;
				}
			}
			else
			{
				Graphics::TBitmap* Buffer = DrawRec->GetBuffer();
				if( Buffer != NULL )
				{
					TRect BufferRect = ARect;
					BufferRect.Top = 0;
					BufferRect.Bottom = Buffer->Height;
					Canvas->CopyRect( ARect, Buffer->Canvas, BufferRect  );
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawHeader( int Col, const Types::TRect &ARect, TGridDrawState AState )
{
	TRect PaintRect = Rect( 0, 0, ColWidths[Col], DefaultRowHeight );

	if( FBufferBmp->Width != PaintRect.Width() || FBufferBmp->Height != PaintRect.Height() )
	{
		FBufferBmp->Width = PaintRect.Width();
		FBufferBmp->Height = PaintRect.Height();
	}
	FBufferBmp->Canvas->Font->Assign( Font );
	if( Col < FStrikePxColIndex ) ///< Call
	{
		FBufferBmp->Canvas->Brush->Color = FHeaderCallBKColor;
		FBufferBmp->Canvas->Font->Color = FHeaderCallColor;
	}
	else if( Col > FStrikePxColIndex ) ///< Put
	{
		FBufferBmp->Canvas->Brush->Color = FHeaderPutBKColor;
		FBufferBmp->Canvas->Font->Color = FHeaderPutColor;
	}
	else  ///< Strike Px
	{
		FBufferBmp->Canvas->Brush->Color = FHeaderCommodBKColor;
		FBufferBmp->Canvas->Font->Color = FHeaderCommodColor;
	}
	FBufferBmp->Canvas->FillRect( PaintRect );
	if( Col == FCallTextCol ) ///< Text call
	{
		int TextWidth = FBufferBmp->Canvas->TextWidth( FHeaderRowText->Strings[0] );
		int TextHeight = FBufferBmp->Canvas->TextHeight( FHeaderRowText->Strings[0] );
		FBufferBmp->Canvas->TextOut( ( PaintRect.Width() - TextWidth )/2, ( PaintRect.Height() - TextHeight )/2, FHeaderRowText->Strings[0] );
	}
	else if( Col == FPutTextCol ) ///< Text Put
	{
		int TextWidth = FBufferBmp->Canvas->TextWidth( FHeaderRowText->Strings[1] );
		int TextHeight = FBufferBmp->Canvas->TextHeight( FHeaderRowText->Strings[1] );
		FBufferBmp->Canvas->TextOut( ( PaintRect.Width() - TextWidth )/2, ( PaintRect.Height() - TextHeight )/2, FHeaderRowText->Strings[1] );
	}
	else if( Col == FStrikePxColIndex ) ///< Text Options Product Name
	{
		int TextWidth  = FBufferBmp->Canvas->TextWidth( FDisplayName );
		int TextHeight = FBufferBmp->Canvas->TextHeight( FDisplayName );

		FBufferBmp->Canvas->TextOut( ( PaintRect.Width() - TextWidth )/2, ( PaintRect.Height() - TextHeight )/2, FDisplayName );
	}
	Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawFieldName( int Col, const Types::TRect &ARect, TGridDrawState AState )
{
	TRect PaintRect = Rect( 0, 0, ColWidths[Col], DefaultRowHeight );

	if( FBufferBmp->Width != PaintRect.Width() || FBufferBmp->Height != PaintRect.Height() )
	{
		FBufferBmp->Width = PaintRect.Width();
		FBufferBmp->Height = PaintRect.Height();
	}
    FBufferBmp->Canvas->Font->Assign( Font );
	if( Col < FStrikePxColIndex ) ///< Call
	{
		FBufferBmp->Canvas->Brush->Color = FFieldNameCallBKColor;
		FBufferBmp->Canvas->Font->Color = FFieldNameCallColor;
	}
	else if( Col > FStrikePxColIndex ) ///< Put
	{
		FBufferBmp->Canvas->Brush->Color = FFieldNamePutBKColor;
		FBufferBmp->Canvas->Font->Color = FFieldNamePutColor;
	}
	else
	{
		FBufferBmp->Canvas->Brush->Color = FFieldNameCommodBKColor;
		FBufferBmp->Canvas->Font->Color = FFieldNameCommodColor;
	}
	int TextWidth = FBufferBmp->Canvas->TextWidth( HeaderString->Strings[Col] );
	int TextHeight = FBufferBmp->Canvas->TextHeight( HeaderString->Strings[Col] );

	FBufferBmp->Canvas->FillRect( PaintRect );
	FBufferBmp->Canvas->TextOut( ( PaintRect.Width() - TextWidth )/2, ( PaintRect.Height() - TextHeight )/2, HeaderString->Strings[Col] );
	Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
}
//---------------------------------------------------------------------------
TOptRecord* __fastcall TOptionsStrikePriceView::GetRecord( CallPutCode CallPut, int Index )
{
	try
	{
		TOptRecord* Rec;

		if( CallPut == cpCall )
			Rec = (TOptRecord*)(FCallList->Items[Index]);
		else
			Rec = (TOptRecord*)(FPutList->Items[Index]);
		return Rec;
	}
	catch(...)
	{
		return NULL;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawPxField( int Row, const Types::TRect &ARect, TGridDrawState AState, CallPutCode CallPut, OptStkColName FieldType, bool CheckTotol  )
{
	if( FCallList->Count > 0 && FPutList->Count > 0 )
	{
		TOptRecord* DrawRec = GetRecord( CallPut, Row - 2  );

		if( DrawRec != NULL )
		{
			double DrawPx = 0.0;
			double MinVal = pow( 0.1, (double) DrawRec->GetDigit() );

			switch( FieldType )
			{
				case BUY_PX:        DrawPx = DrawRec->GetBidPx();            break;
				case SELL_PX:       DrawPx = DrawRec->GetAskPx();            break;
				case TRADE_PX:      DrawPx = DrawRec->GetTradePx();          break;
				case HIGH_PX:       DrawPx = DrawRec->GetHighPx();           break;
				case LOW_PX:        DrawPx = DrawRec->GetLowPx();            break;
				case SETTLEMENT_PX: DrawPx = DrawRec->GetSettlementPx();     break;
				case THEORY_PX:     DrawPx = DrawRec->GetGreeks().dTheoryPx; break;
			}
			if( CheckTotol == true )
			{
				if( DrawRec->GetTotalQty() > 0 && DrawPx > MinVal )
					DrawPrice( ARect, AState, DrawRec, DrawPx );
				else
					DrawDoubleDash( ARect, AState, DrawRec );
			}
			else
			{
				if( DrawPx > MinVal )
					DrawPrice( ARect, AState, DrawRec, DrawPx );
				else
					DrawDoubleDash( ARect, AState, DrawRec );
            }
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawGrid( TOptRecord* Record )
{
	TColor BKColor;

	FBufferBmp->Canvas->Pen->Width = 1;
	if( Record->GetCallPut() == cpCall )
		BKColor = FCallBKColor;
	else
		BKColor = FPutBKColor;
	if( FHGridColor != BKColor )
	{
		FBufferBmp->Canvas->Pen->Color = FHGridColor;
		FBufferBmp->Canvas->MoveTo( 0, FBufferBmp->Height -1 );
		FBufferBmp->Canvas->LineTo( FBufferBmp->Width , FBufferBmp->Height -1 );
	}
	if( FVGridColor != BKColor )
	{
		FBufferBmp->Canvas->Pen->Color = FVGridColor;
		FBufferBmp->Canvas->MoveTo( FBufferBmp->Width - 1, FBufferBmp->Height -1 );
		FBufferBmp->Canvas->LineTo( FBufferBmp->Width - 1, 0 );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawBackground( const Types::TRect &ARect, Types::TRect& PaintRect, TOptRecord* Record, bool IsMouseOver )
{
	///< Draw solid background
	if( IsMouseOver == true )
		FBufferBmp->Canvas->Brush->Color = GDIUtility::BrighterColor( FBufferBmp->Canvas->Brush->Color, 64 );
	FBufferBmp->Canvas->Brush->Style = bsSolid;
	FBufferBmp->Canvas->FillRect( PaintRect );
	///< Draw Gradient Bar background
	if( FBKCol != obNone )
	{
		if( Record->GetCallPut() == cpCall )
		{
			int XPos        = FTotalWidth - Record->GetValueWidth();
			int SolidWidth  = XPos - ARect.left;

			if( SolidWidth < PaintRect.Width() )
			{
				//TColor UpCol   = FCallBKColor;
				TColor DownCol = FBKBarColor;

				if( IsMouseOver == true )
				{
					//UpCol   = GDIUtility::BrighterColor( UpCol, 64 );
					DownCol = GDIUtility::BrighterColor( DownCol, 64 );
				}
				if( SolidWidth < 0 )  SolidWidth = 0;
				int GradientWidth = PaintRect.Width() - SolidWidth;
				TRect GradientRect = TRect( 0, 0, GradientWidth, DefaultRowHeight );
				if( FAlphaBmp->Height != DefaultRowHeight )
					FAlphaBmp->Height = DefaultRowHeight;
				if(	FAlphaBmp->Width != GradientWidth )
					FAlphaBmp->Width  = GradientWidth;
				GDIUtility::DrawGradientBar( FAlphaBmp->Canvas, GradientRect, DownCol, DownCol, true );
				FBufferBmp->Canvas->Draw( SolidWidth,0, FAlphaBmp, 200 );
			}
		}
		else
		{
			int XPos = FTotalWidth + FStrikePxWidth + Record->GetValueWidth();
			int SolidWidth  = ARect.right - XPos;

			if( SolidWidth < PaintRect.Width() )
			{
				//TColor UpCol   = FPutBKColor;
				TColor DownCol = FBKBarColor;

				if( IsMouseOver == true )
				{
					//UpCol   = GDIUtility::BrighterColor( UpCol, 64 );
					DownCol = GDIUtility::BrighterColor( DownCol, 64 );
				}
				if( SolidWidth < 0 )  SolidWidth = 0;
				int GradientWidth = PaintRect.Width() - SolidWidth;
				TRect GradientRect = TRect( 0, 0, GradientWidth, DefaultRowHeight );
				if( FAlphaBmp->Height != DefaultRowHeight )
					FAlphaBmp->Height = DefaultRowHeight;
				if(	FAlphaBmp->Width != GradientWidth )
					FAlphaBmp->Width  = GradientWidth;
				GDIUtility::DrawGradientBar( FAlphaBmp->Canvas, GradientRect, DownCol, DownCol, true );
				FBufferBmp->Canvas->Draw( 0,0, FAlphaBmp, 200 );
			}
		}
	}
	FBufferBmp->Canvas->Brush->Style = bsClear;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawPrice( const Types::TRect &ARect, TGridDrawState AState, TOptRecord* Record, double Px )
{
	TRect       PaintRect = Rect( 0, 0, ARect.Width(), ARect.Height() );
	TTextFormat Formats;
	String      DrawText;

	Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
	if( FBufferBmp->Width != PaintRect.Width() || FBufferBmp->Height != PaintRect.Height() )
	{
		FBufferBmp->Width = PaintRect.Width();
		FBufferBmp->Height = PaintRect.Height();
	}
	if( Record->IsUpeerLimit( Px ) ) ///< Bull Price
	{
		FBufferBmp->Canvas->Brush->Color = FRisePxColor;
		FBufferBmp->Canvas->Font->Color  = clWhite;
	}
	else if( Record->IsLowerLimit( Px ) ) ///< Bear Price
	{
		FBufferBmp->Canvas->Brush->Color = FFallPxColor;
		FBufferBmp->Canvas->Font->Color  = clWhite;
	}
	else ///< Between Bull/Bear Price.
	{
		if( Record->GetCallPut() == cpCall )
			FBufferBmp->Canvas->Brush->Color = FCallBKColor;
		else
			FBufferBmp->Canvas->Brush->Color = FPutBKColor;
		if( Record->IsFlat( Px ) )
			FBufferBmp->Canvas->Font->Color  = FEqualPxColor;
		else if( Record->IsClimbed( Px ) )
			FBufferBmp->Canvas->Font->Color  = FRisePxColor;
		else
			FBufferBmp->Canvas->Font->Color  = FFallPxColor;
	}
	DrawBackground( ARect, PaintRect, Record, AState.Contains( gdHotTrack ) );
	DrawText.printf( L"%.*f", Record->GetDigit(), Px );
	PaintRect.Right -= 2;
	FBufferBmp->Canvas->TextRect( PaintRect, DrawText, Formats );
	DrawGrid( Record );
	Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawGeeksField( int Row, const Types::TRect &ARect, TGridDrawState AState, CallPutCode CallPut, OptStkColName FieldType )
{
	if( FCallList->Count > 0 && FPutList->Count > 0 )
	{
		TOptRecord* DrawRec = GetRecord( CallPut, Row - 2 );
		double Value = 0.0;

		if( DrawRec != NULL )
		{
			switch( FieldType )
			{
				case DELTA: Value = DrawRec->GetGreeks().dDelta ;break;
				case GAMMA: Value = DrawRec->GetGreeks().dGamma ;break;
				case THETA: Value = DrawRec->GetGreeks().dTheta ;break;
				case VEGA:  Value = DrawRec->GetGreeks().dVega  ;break;
				case RHO:   Value = DrawRec->GetGreeks().dRho   ;break;
			}
		}
		if( fabs( Value ) < 0.0001 )
			DrawDoubleDash(  ARect, AState, DrawRec );
		else
			DrawValue( ARect, AState, DrawRec, Value );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawValue( const Types::TRect &ARect, TGridDrawState AState, TOptRecord* Record, double Value )
{
	TRect  PaintRect = Rect( 0, 0, ARect.Width(), ARect.Height() );
	String DrawText;
	TTextFormat Formats;

	Formats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
	if( FBufferBmp->Width != PaintRect.Width() || FBufferBmp->Height != PaintRect.Height() )
	{
		FBufferBmp->Width = PaintRect.Width();
		FBufferBmp->Height = PaintRect.Height();
	}
	if( Value > 0  ) ///< + Value
		FBufferBmp->Canvas->Font->Color  = FRisePxColor;
	else if( Value < 0 ) ///< - value
		FBufferBmp->Canvas->Font->Color = FFallPxColor;
	else ///< = 0
		FBufferBmp->Canvas->Font->Color  = FEqualPxColor;
	if( Record->GetCallPut() == cpCall )
		FBufferBmp->Canvas->Brush->Color = FCallBKColor;
	else
		FBufferBmp->Canvas->Brush->Color = FPutBKColor;
	DrawBackground( ARect, PaintRect, Record, AState.Contains( gdHotTrack ) );
	DrawText.printf( L"%.4f", Value );
	PaintRect.Right -= 2;
	FBufferBmp->Canvas->TextRect( PaintRect, DrawText, Formats );
	DrawGrid( Record );
	Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawImpVar( int Row, const Types::TRect &ARect, TGridDrawState AState, CallPutCode CallPut )
{
	if( FCallList->Count > 0 && FPutList->Count > 0 )
	{
		TOptRecord* DrawRec = GetRecord( CallPut, Row - 2 );
		double Value;// = 0.0;

		if( DrawRec != NULL  )
		{
			Value = DrawRec->GetGreeks().dImpv*100.0;
			if( fabs( Value ) > 0.009 )
				DrawPercentage(  ARect, AState, DrawRec, Value );
			else
				DrawDoubleDash(  ARect, AState, DrawRec );
		}
		else
			DrawDoubleDash(  ARect, AState, DrawRec );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawPercentage( const Types::TRect &ARect, TGridDrawState AState, TOptRecord* Record, double Value )
{
	TRect       PaintRect = Rect( 0, 0, ARect.Width() , ARect.Height() );
	String      DrawText;
	TTextFormat Formats;

	Formats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
	if( FBufferBmp->Width != PaintRect.Width() || FBufferBmp->Height != PaintRect.Height() )
	{
		FBufferBmp->Width = PaintRect.Width();
		FBufferBmp->Height = PaintRect.Height();
	}
	FBufferBmp->Canvas->Font->Color  = FEqualPxColor;
	if( Record->GetCallPut() == cpCall )
		FBufferBmp->Canvas->Brush->Color = FCallBKColor;
	else
		FBufferBmp->Canvas->Brush->Color = FPutBKColor;
	DrawBackground( ARect, PaintRect, Record, AState.Contains( gdHotTrack ) );
	DrawText.printf( L"%.2f%", Value );
	PaintRect.Right -= 2;
	FBufferBmp->Canvas->TextRect( PaintRect, DrawText, Formats );
	DrawGrid( Record );
	Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawQtyField( int Row, const Types::TRect &ARect, TGridDrawState AState, CallPutCode CallPut, OptStkColName FieldType )
{
	if( FCallList->Count > 0 && FPutList->Count > 0 )
	{
		TOptRecord* DrawRec = GetRecord( CallPut, Row - 2 );

		if( DrawRec != NULL )
		{
			switch( FieldType )
			{
				case TRADE_QTY: DrawQty( ARect, AState, DrawRec, DrawRec->GetTradeQty() );break;
				case TOTAL_QTY: DrawQty( ARect, AState, DrawRec, DrawRec->GetTotalQty() );break;
				case BUY_QTY:   DrawQty( ARect, AState, DrawRec, DrawRec->GetBidQty() );  break;
				case SELL_QTY:  DrawQty( ARect, AState, DrawRec, DrawRec->GetAskQty() );  break;
				case OPEN_INTEREST: DrawQty( ARect, AState, DrawRec, DrawRec->GetOpenInterest() );  break;
				case DIFF_OI:       DrawQty( ARect, AState, DrawRec, DrawRec->GetOpenInterestDiff() );  break;
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawQty( const Types::TRect &ARect, TGridDrawState AState, TOptRecord* Record, int Qty )
{
	TRect       PaintRect = Rect( 0, 0, ARect.Width(), ARect.Height() );
	String      DrawText;
	TTextFormat Formats;

	Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
	if( FBufferBmp->Width != PaintRect.Width() || FBufferBmp->Height != PaintRect.Height() )
	{
		FBufferBmp->Width = PaintRect.Width();
		FBufferBmp->Height = PaintRect.Height();
	}
	if( Record->GetCallPut() == cpCall )
		FBufferBmp->Canvas->Brush->Color = FCallBKColor;
	else
		FBufferBmp->Canvas->Brush->Color = FPutBKColor;
	FBufferBmp->Canvas->Font->Color = FEqualPxColor;
	DrawBackground( ARect, PaintRect, Record, AState.Contains( gdHotTrack ) );
	DrawText.printf( L"%d", Qty );
	PaintRect.Right -= 2;
	FBufferBmp->Canvas->TextRect( PaintRect, DrawText, Formats );
	DrawGrid( Record );
	Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawDoubleDash( const Types::TRect &ARect, TGridDrawState AState, TOptRecord* Record )
{
	TRect       PaintRect = Rect( 0, 0, ARect.Width() , ARect.Height() );
	String      DrawText( L"--" );
	TTextFormat Formats;

	Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
	if( FBufferBmp->Width != PaintRect.Width() || FBufferBmp->Height != PaintRect.Height() )
	{
		FBufferBmp->Width = PaintRect.Width();
		FBufferBmp->Height = PaintRect.Height();
	}
	if( Record->GetCallPut() == cpCall )
		FBufferBmp->Canvas->Brush->Color = FCallBKColor;
	else
		FBufferBmp->Canvas->Brush->Color = FPutBKColor;
	FBufferBmp->Canvas->Font->Color = FEqualPxColor;
	DrawBackground( ARect, PaintRect, Record, AState.Contains( gdHotTrack ) );
	PaintRect.Right -= 2;
	FBufferBmp->Canvas->TextRect( PaintRect, DrawText, Formats );
	DrawGrid( Record );
	Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawStrikePrice( int Row, const Types::TRect &ARect, TGridDrawState AState )
{
	if( Row > FStrikePriceList->Count )
		return;
	TRect       PaintRect = Rect( 0, 0, ARect.Width() , ARect.Height() );
	String      Str( FStrikePriceList->Strings[ Row ] );
	TTextFormat Formats;

	Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter<<tfEndEllipsis;
	if( FBufferBmp->Width != PaintRect.Width() || FBufferBmp->Height != PaintRect.Height() )
	{
		FBufferBmp->Width = PaintRect.Width();
		FBufferBmp->Height = PaintRect.Height();
	}
	FBufferBmp->Canvas->Brush->Style = bsSolid;
	if( FMouseCoord.X == FStrikePxColIndex && FMouseCoord.Y == Row )
	{
		TRect LeftRect  = PaintRect;
		TRect RightRect = PaintRect;
		int   HalfWidth = PaintRect.Width()/2;

		LeftRect.Right -= HalfWidth;
		RightRect.Left += HalfWidth;
		FBufferBmp->Canvas->Brush->Color = clSilver;
		FBufferBmp->Canvas->FillRect( LeftRect );
		FBufferBmp->Canvas->Brush->Color = clWhite;
		FBufferBmp->Canvas->FillRect( RightRect );
		FBufferBmp->Canvas->Font->Color  = FStkPxBKColor;
	}
	else
	{
		TOptRecord* Rec = (TOptRecord*)FCallList->Items[ Row - 2 ];
		if( Rec != NULL &&	Rec->Underlying != NULL &&	Rec->Underlying->IsUnderlyingStrikePx( Row ) == true )
		{
			int YOffset = (int)(Rec->Underlying->GetStrikePxPosRate()* (double)(DefaultRowHeight-2)) + 1;
			FBufferBmp->Canvas->Brush->Color = FStkPxBKColor;
			FBufferBmp->Canvas->Pen->Color   = FStkPxColor;
			FBufferBmp->Canvas->Rectangle( PaintRect );
			FBufferBmp->Canvas->Pen->Color  = clRed;
			FBufferBmp->Canvas->MoveTo( 1 , YOffset );
			FBufferBmp->Canvas->LineTo( 1 + STRIKE_PX_LINE_LEN, YOffset );
			FBufferBmp->Canvas->MoveTo( FBufferBmp->Width - 1 - STRIKE_PX_LINE_LEN, YOffset  );
			FBufferBmp->Canvas->LineTo( FBufferBmp->Width - 1, YOffset );
		}
		else
		{
			FBufferBmp->Canvas->Brush->Color = FStkPxBKColor;
			FBufferBmp->Canvas->FillRect( PaintRect );
		}
		FBufferBmp->Canvas->Font->Color  = FStkPxColor;
	}
	FBufferBmp->Canvas->Brush->Style = bsClear;
	FBufferBmp->Canvas->TextRect( PaintRect, Str, Formats );
	Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawDiffPx( int Row, const Types::TRect &ARect, TGridDrawState AState, CallPutCode CallPut )
{
	if( FCallList->Count > 0 && FPutList->Count > 0 )
	{
		TOptRecord* DrawRec = GetRecord( CallPut, Row - 2 );

		if( DrawRec != NULL )
			DrawDiff( ARect, AState, DrawRec );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::DrawDiff( const Types::TRect &ARect, TGridDrawState AState, TOptRecord* Record )
{
	if( Row > FStrikePriceList->Count )
		return;
	TRect PaintRect = Rect( 0, 0,ARect.Width(), ARect.Height());
	int ItemLeft = PaintRect.Left;
	int ItemTop = PaintRect.Top;
	int ItemHeight = PaintRect.Height();

	if( FBufferBmp->Width != PaintRect.Width() || FBufferBmp->Height != PaintRect.Height() )
	{
		FBufferBmp->Width = PaintRect.Width();
		FBufferBmp->Height = PaintRect.Height();
	}

	if( Record->IsUpeerLimit( Record->GetTradePx() ) )
	{
		FBufferBmp->Canvas->Brush->Color = FRisePxColor;
		FBufferBmp->Canvas->Font->Color = clWhite;
	}
	else if( Record->IsLowerLimit( Record->GetTradePx() ) )
	{
		FBufferBmp->Canvas->Brush->Color = FFallPxColor;
		FBufferBmp->Canvas->Font->Color = clWhite;
	}
	else
	{
		if( Record->GetCallPut() == cpCall )
			FBufferBmp->Canvas->Brush->Color = FCallBKColor;
		else
			FBufferBmp->Canvas->Brush->Color = FPutBKColor;

		if( Record->IsFlat( Record->GetTradePx() ) )
			FBufferBmp->Canvas->Font->Color = FEqualPxColor;
		else if( Record->IsClimbed( Record->GetTradePx() ) )
			FBufferBmp->Canvas->Font->Color = FRisePxColor;
		else
			FBufferBmp->Canvas->Font->Color = FFallPxColor;
	}
	DrawBackground( ARect, PaintRect, Record, AState.Contains( gdHotTrack ) );

	String      Text;
	TTextFormat Formats;
	double      Px = Record->GetTradePx() - Record->GetRefPx();

	if( Record->GetTotalQty() > 0 )
	{
		double Value = Px;
		if( Value < 0 )
			Value *= -1;
		Text.printf( L"%.*f", Record->GetDigit(), Value );
	}
	else
	{
		Text.printf( L"--" );
		FBufferBmp->Canvas->Font->Color = FEqualPxColor;
	}
	ItemLeft += PaintRect.Width() - FBufferBmp->Canvas->TextWidth( Text ) - (ItemHeight - 10) - 5;
	PaintRect.right -= 2;
	Formats <<tfSingleLine<<tfRight<<tfVerticalCenter<<tfEndEllipsis;
	FBufferBmp->Canvas->TextRect( PaintRect, Text, Formats );
	if( Record->GetTotalQty() > 0 )  ///< Draw Up/Down triangle.(Red/Green)
	{
		if( Px > 0 )
		{
			FBufferBmp->Canvas->Brush->Color = FRisePxColor;
			FBufferBmp->Canvas->Pen->Color   = FRisePxColor;
			TPoint  TriPoint[4] = {TPoint(ItemLeft + ( ItemHeight - 10 )/2, ItemTop+5),
								   TPoint(ItemLeft ,ItemTop+ItemHeight - 5),
								   TPoint(ItemLeft+(ItemHeight - 10), ItemTop+ItemHeight - 5 ),
								   TPoint(ItemLeft + (ItemHeight - 10)/2,ItemTop+5)};
			FBufferBmp->Canvas->Polygon( TriPoint, 3 );
		}
		else if( Px < 0 )
		{
			FBufferBmp->Canvas->Brush->Color = FFallPxColor;
			FBufferBmp->Canvas->Pen->Color   = FFallPxColor;
			TPoint  TriPoint[4] = {TPoint(ItemLeft,ItemTop+5),
								   TPoint(ItemLeft+(ItemHeight - 10),ItemTop+5),
								   TPoint(ItemLeft+(ItemHeight - 10)/2,ItemTop+ItemHeight-5),
								   TPoint(ItemLeft,ItemTop+5)};
			FBufferBmp->Canvas->Polygon( TriPoint, 3 );
		}
	}
	DrawGrid( Record );
	Canvas->Draw( ARect.Left, ARect.Top, FBufferBmp );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::Loaded(void)
{
	if( ComponentState.Contains( csDesigning ) )
		Invalidate();
	CalSize();
	FIsLoaded = true;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetReverse( bool IsReverse )
{
	FIsReverse = IsReverse;
	InitGrid();
	for( int i = 0; i < FStrikePxColIndex; i++ )
		 InvalidateCol( i );
}
//---------------------------------------------------------------------------
String __fastcall TOptionsStrikePriceView::GetFieldsString( void )
{
	return String( FFieldsString.c_str() );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetFieldsString( const String& Fields )
{
	AnsiString AnsiFields = Fields;

	FFieldsString = AnsiFields.c_str();
	InitGrid();
}
//---------------------------------------------------------------------------
bool __fastcall TOptionsStrikePriceView::StrikePxExists( double StkPx, TList* CPList )
{
	TOptRecord* Rec;
	for( int j = CPList->Count - 1; j >=0; j-- )
	{
		Rec = (TOptRecord*)CPList->Items[ j ];

		if( StkPx == Rec->GetStrikePx() )
			return true; ///< call/Put StrikePx match.
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::InAButNotInB( TList* AList,
													   TList* BList,
													   UFC::PHashedList<UFC::AnsiString, TOptRecord*>& AllPinsTable )
{
	for( register int i = AList->Count - 1; i >=0; i-- )
	{
		TOptRecord* Rec = (TOptRecord*)AList->Items[i];///< Get an item from A list.

		///< Item in A list, but not in B list.
		if( Rec != NULL && StrikePxExists( Rec->GetStrikePx(), BList ) == false )
		{
			AnsiString Key( Rec->GetSymbol() );///< Unicode to Ansi

			if( AllPinsTable.IsExists( Key.c_str() ) == true )
				AllPinsTable.DeleteByKey( Key.c_str());
			AList->Delete( i );
			delete Rec;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::CheckCallPutPair( TList* CallList,
														   TList* PutList,
														   UFC::PHashedList<UFC::AnsiString, TOptRecord*>& PinsTable )
{
	if( CallList->Count != PutList->Count )
	{
		InAButNotInB( CallList, PutList, PinsTable ); ///< In call list, but not in put list.
		InAButNotInB( PutList, CallList, PinsTable ); ///< In put list, but not in call list.
	}
}
//---------------------------------------------------------------------------
String PrevYearMonth( const String& YearMonth )
{
	int year  = YearMonth.SubString( 1,4 ).ToInt();
	int month = YearMonth.SubString( 5,2 ).ToInt() - 1;
	String PrevYM;

	if( month <= 0 )
	{
		month = 12;
		year -= 1;
	}
	PrevYM.printf( L"%04d%02d",year,month );
	return PrevYM;
}
//---------------------------------------------------------------------------
UFC::AnsiString __fastcall TOptionsStrikePriceView::GetUnderlyingProductID( BasicInformation* Info)
{
	UFC::AnsiString ProductID( Info->GetProductID());
	UFC::AnsiString AnsiExchange( Info->GetExchange( ));

	if( AnsiExchange == "TAIFEX" )
	{
		if( ProductID.AnsiPos("TX") == 0)
			return "TXF";
		if( ProductID.AnsiPos("TEO") == 0)
			return "EXF";
		if( ProductID.AnsiPos("TFO") == 0)
			return "FXF";
		if( ProductID.AnsiPos("XIO") == 0)
			return "XIF";
		if( ProductID.AnsiPos("TGO") == 0)
			return "GDF";
		if( ProductID.AnsiPos("GTO") == 0)
			return "GTF";
	}
	else if( AnsiExchange == "KRX" )
	{
		if (ProductID.AnsiPos(L"KS") == 0 )
			return  "KS";
	}
	else if( AnsiExchange == "CFFEX" )
	{
		if (ProductID.AnsiPos( "IO") == 0 )
			return "IF";
		else if (ProductID.AnsiPos( "HO") == 0)
			return "IH";
	}
	else if( AnsiExchange == "DCE" )
	{
		if (ProductID.AnsiPos( "m") == 0)
			return "m";
	}
	else if( AnsiExchange == "CZCE" )
	{
		if (ProductID.AnsiPos( "SR" ) == 0)
			return "SR";
	}
	else if( AnsiExchange == "SHFE" )
	{
		if (ProductID.AnsiPos( "au" ) == 0)
			return "au";
		if (ProductID.AnsiPos( "cu" ) == 0)
			return "cu";
	}
	return ProductID;
}
//---------------------------------------------------------------------------
BasicInformation* __fastcall TOptionsStrikePriceView::GetUnderlying( BasicInformation* OptInfo, const String& YearMonth )
{
	BasicInformation* UnderingInfo  = NULL;
	UFC::AnsiString   AnsiExchange( OptInfo->GetExchange() );
	UFC::AnsiString   Underlying( OptInfo->GetUnderlying( ) );

	///< Underlying From StarWave
	if( Underlying.Length() > 0 )
		UnderingInfo = FStore->GetBasicInformation(  AnsiExchange, Underlying ,false );
	if( UnderingInfo == NULL )
	{
		 String   UnderlyingPID( GetUnderlyingProductID( OptInfo ).c_str() );
		 ///< Find underlying by ProductID, MonthYear
		 if( (UnderingInfo = FStore->GetFUTInfo( FExchange, UnderlyingPID, YearMonth )) == NULL)
		 {
				String  SymFront = UnderlyingPID + L"Front1";
				///< Find underlying by Exchange, front month.
				if( (UnderingInfo = FStore->GetBasicInformation( FExchange, SymFront, false  )) == NULL )
				{
					///< Find underlying by Exchange, prev month.
					if( (UnderingInfo = FStore->GetFUTInfo( FExchange, UnderlyingPID, PrevYearMonth( YearMonth ) )) == NULL )
						UnderingInfo =	OptInfo; ///< Can't find any Info,return self.
				}
		 }
	}
	return UnderingInfo;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::AddOptionsByMonth( const String& YearMonth,
															TList* CallList,
															TList* PutList,
															UFC::PHashedList<UFC::AnsiString, TOptRecord*>& PinsTable,
															UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*>& ExcludeStkPxSet )
{
	UFC::List<BasicInformation*>* pList = (UFC::List<BasicInformation*>*)FStore->GetOPTMDSeriesWithYearMonth( FExchange, FSymbol, YearMonth );

	if( pList != NULL && pList->ItemCount() > 0 )
	{
		TOptRecord*       UnderlyingRec;
		TOptRecord*       NewRec;
		AnsiString        AnsiYearMonth = YearMonth;
		BasicInformation* Info = GetUnderlying( pList->GetItem( 0 ), YearMonth );
		UFC::PHashedSet<double>* StkPxSet = NULL;

		if( Info != NULL ) ///< Underlying exists.
		{
			UFC::AnsiString   UnderlyingSymbol = Info->GetSymbol() ;
			///< Create Underlying Record.
			UnderlyingRec = new TOptRecord( Info, true );
			UnderlyingRec->SetYearMonth( YearMonth );
			///< Get exclude strike price set of this year month.
			ExcludeStkPxSet.GetObjectByKey( AnsiYearMonth.c_str(), StkPxSet );
			///< Add all Options symbol pins in list.
			for( register int i = 0; i < pList->ItemCount(); i++ )
			{
				if( (Info = pList->GetItem( i )) != NULL )
				{
					TList* CPList;

					if( Info->GetCallPut() == cpPut )
						CPList = PutList;
					else
						CPList = CallList;
					if(	StrikePxExists( Info->GetStrikePrice(), CPList ) == false && ///< Strike price not exists.
						( StkPxSet == NULL ||                                        ///< No exclude set.
						  StkPxSet->Exists( Info->GetStrikePrice() ) == false) )     ///< Not in exclude set.
					{
						NewRec = new TOptRecord( Info, false );
						CPList->Add( NewRec );
						NewRec->Underlying = UnderlyingRec;
						PinsTable.Add( Info->GetSymbol(), NewRec );
					}
				}
			}
			///< Sort by strike price
			CallList->Sort( OptSymbolCompare );
			PutList->Sort( OptSymbolCompare );
			///< Check both Call/Put pin exists in same strike price.
			CheckCallPutPair( CallList, PutList, PinsTable );
			///< Insert underlying symbol
			AddUnderlyingRecord( UnderlyingRec,PinsTable );
			///< Insert underlying record tho the top of the list.
			CallList->Insert( 0, UnderlyingRec );
			PutList->Insert( 0, UnderlyingRec );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::AddUnderlyingRecord( TOptRecord* UnderlyingRec, UFC::PHashedList<UFC::AnsiString, TOptRecord*>& PinsTable )
{
	TOptRecord* CurRec;
	TOptRecord* RootRec; ///< First record.
	AnsiString UnderlyingSymbol = UnderlyingRec->GetSymbol();

	///< any other Options pins reference this Underlying ?
	if( (RootRec = PinsTable.GetObjectByKey( UnderlyingSymbol.c_str() ))!= NULL )
	{    ///< Yes!
		 CurRec = RootRec;
		 while( CurRec->Next != NULL ) ///< seek to the end.
		 {
			CurRec = CurRec->Next;
		 };
		 CurRec->Next = UnderlyingRec; ///< add to the end of the Underlying record linking list.
		 UnderlyingRec->Next = NULL;
	}
	else ///< No add to record table.
		PinsTable.Add( UnderlyingSymbol.c_str(), UnderlyingRec );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::BuidPinList( TList* CallPins,
													  TList* PutPins,
													  UFC::PHashedList<UFC::AnsiString, TOptRecord*>& PinsTable,
													  UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*>& ExcludeStkPxSet )
{
	TList* CallList = new TList();
	TList* PutList  = new TList();
	///< Parse YearMonth string.
	TStringList* MYlist = new TStringList();

	MYlist->Delimiter = '^';
	MYlist->DelimitedText = FYearMonth;
	/// < Add by Month
	for( int i = 0; i < MYlist->Count; i++ )
	{
		String MYearMonth = MYlist->Strings[i];

		CallList->Clear();
		PutList->Clear();
		AddOptionsByMonth( MYearMonth, CallList, PutList, PinsTable, ExcludeStkPxSet );
		for( register int i=0; i< CallList->Count; i++ )
			CallPins->Add( CallList->Items[i] );
		for( register int i=0; i< PutList->Count; i++ )
			PutPins->Add( PutList->Items[i] );
	}
	delete MYlist;

	delete CallList;
	delete PutList;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::UpdateTable( UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*>& ExcludeStkPxSet )
{
	Clear( );

	if( !ComponentState.Contains( csDesigning ) && FIsLoaded && FStore != NULL  )
	{
		BuidPinList( FCallList, FPutList, FTable, ExcludeStkPxSet );
		BuildStrikePriceList();
		TradetimeTimer( this );
		FTimer->Enabled = true;
	}
	CalSize();
	Invalidate();
	Subscribe();
	FixedRows = 2;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::UpdateTable( void )
{
	UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*> ExcludeStkPxSet;

	UpdateTable( ExcludeStkPxSet );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::Subscribe( void )
{
	TOptRecord* Rec;
	AnsiString Ex = FExchange;

	FSubsceibeExchange = Ex.c_str();
	FSubsceibeSymbolList.Clear();
	FBKValueMax = 0.0;
	for( register int i = 0; i < FCallList->Count; i++ )
	{
		if((Rec = GetRecord( cpCall, i ))!= NULL )
		{
			AnsiString Sym( Rec->GetSymbol() );
			if( FSubsceibeSymbolList.IndexOf( Sym.c_str() ) == -1 )
				FSubsceibeSymbolList.Add( Sym.c_str() );
		}
	}
	for( register int i = 0; i < FPutList->Count; i++ )
	{
		if((Rec = GetRecord( cpPut, i ))!= NULL )
		{
			AnsiString Sym( Rec->GetSymbol() );
			if( FSubsceibeSymbolList.IndexOf( Sym.c_str() ) == -1 )
				FSubsceibeSymbolList.Add( Sym.c_str() );
		}
	}
	FResponseCount = 0;
	for( int i = 0; i < FSubsceibeSymbolList.ItemCount(); i++ )
	{
		UFC::AnsiString Sym = FSubsceibeSymbolList[ i ];

		FStore->Subscribe( FSubsceibeExchange, Sym, this );
		UFC::SleepMS( 2 );
		if( i % 10 == 0 )
			Application->ProcessMessages();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::BuildStrikePriceList( void )
{
	TOptRecord* UnderlyingRec = NULL;
	TOptRecord* PinRec;
	String      StkPx;

	FStrikePriceList->BeginUpdate();
	FStrikePriceList->Clear();
	FStrikePriceList->Add( L"0_Header" );
	FStrikePriceList->Add( L"0_FieldName" );

	for( register int i = 0; i < FCallList->Count; i++ )
	{
		if((PinRec = GetRecord( cpCall, i ))!= NULL )
		{
			PinRec->SetRowIndex( FStrikePriceList->Count );
			if( PinRec->IsUnderlying() == false ) ///< Options pin.
			{
				if( UnderlyingRec != NULL )
					UnderlyingRec->AddStrikePx( PinRec->GetStrikePx() );
				StkPx.printf( L"%.*f", FStore->GetStrikePriceDigit( PinRec->GetExchange(), PinRec->GetSymbol() ), PinRec->GetStrikePx() );
				FStrikePriceList->Add( StkPx );
			}
			else ///< Underlying Futures.
			{
				UnderlyingRec = PinRec;
				FStrikePriceList->Add( PinRec->GetSymbol() );
			}
		}
	}
	RowCount = FStrikePriceList->Count;
	FStrikePriceList->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::ClearList( TList* Call, TList* Put )
{
	TOptRecord* pDelRec;

	for( register int i = 0; i < Call->Count; i++ )
	{
		if( (pDelRec = (TOptRecord*)(Call->Items[i]))!= NULL )
			if( pDelRec->IsUnderlying() == false )
				delete pDelRec;
	}
	for( register int i = 0; i < Put->Count; i++ )
	{
		if( (pDelRec = (TOptRecord*)(Put->Items[i]))!= NULL )
			delete pDelRec;
	}
	Call->Clear();
	Put->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::Clear( void )
{
	TOptRecord* pDelRec;

	FTable.Clear(); ///< Clear Contract table.
	RowCount = 2;

	for( int i = 0; i < FSubsceibeSymbolList.ItemCount(); i++ )
	{
		FStore->Unsubscribe( FSubsceibeExchange, FSubsceibeSymbolList[ i ], this );
		UFC::SleepMS( 2 );
	}
	ClearList( FCallList, FPutList );
}
//---------------------------------------------------------------------------
Classes::TStringList* __fastcall TOptionsStrikePriceView::GetSymbols( void )
{
	Classes::TStringList* List = new Classes::TStringList();
	TOptRecord* Rec;

	for( register int i = 0; i < FCallList->Count; i++ )
	{
		if( (Rec = GetRecord( cpCall, i ))!= NULL )
		{
			String Symbol( Rec->GetSymbol() );
			List->Add( Symbol );
		}
	}
	for( register int i = 0; i < FCallList->Count; i++ )
	{
		if( (Rec = GetRecord( cpPut, i ))!= NULL )
		{
			String Symbol( Rec->GetSymbol() );
			List->Add( Symbol );
		}
	}
	return List;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::GetStrikePxInfo( int& Digit, UFC::PStringList& YM, UFC::PDoubleList& StrikePx, UFC::PDoubleList& SpotPx )
{
	TOptRecord* Rec;
	UFC::PHashMap<UFC::AnsiString, UFC::PHashedSet<double>*> ExcludeStkPxSet;
	UFC::PHashedList<UFC::AnsiString, TOptRecord*> AllPinTable;
	TList* AllCallList = new TList();
	TList* AllPutList  = new TList();

	YM.Clear();
	StrikePx.Clear();
	SpotPx.Clear();
	BuidPinList( AllCallList, AllPutList, AllPinTable, ExcludeStkPxSet );
	for( register int i = 0; i < AllCallList->Count; i++  )
	{
		if( (Rec = (TOptRecord*)(AllCallList->Items[i])) != NULL && Rec->IsUnderlying() == false )
		{
			AnsiString YearMonth = Rec->GetYearMonth();
			YM.Add( YearMonth.c_str());
			StrikePx.Add( Rec->GetStrikePx() );
			if( Rec->Underlying->GetTradePx() == 0.0 )
				SpotPx.Add( Rec->Underlying->GetRefPx() );
			else
				SpotPx.Add( Rec->Underlying->GetTradePx() );
		}
	}
	Digit = FStore->GetStrikePriceDigit( Rec->GetExchange(), Rec->GetSymbol() );
	ClearList( AllCallList, AllPutList );
	delete AllCallList;
	delete AllPutList;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::ScrollUp( int Tick )
{
	if( TopRow > 2 )
		TopRow -= 1;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::ScrollDown( int Tick )
{
	if( TopRow < RowCount - 1 )
		TopRow += 1;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::MyMouseWheelDown( System::TObject* Sender, Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled )
{
	ScrollDown( 1 );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::MyMouseWheelUp( System::TObject* Sender, Classes::TShiftState Shift, const Types::TPoint &MousePos, bool &Handled )
{
	ScrollUp( 1 );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::HandleMouseDown( Controls::TMouseButton Button, Classes::TShiftState Shift, int X,int Y, TOptRecord* SelRec, int CoordX, CallPutCode CallPut )
{
	if( SelRec->IsUnderlying() == false )
	{
		if( FFieldTypes[ CoordX ] == BUY_PX && FOnTBarMouseDown != NULL )
			FOnTBarMouseDown( this, SelRec->GetSymbol(), SelRec->GetYearMonth(), nsOrderMessageDefine::sBuy, SelRec->GetBidPx(), CallPut, Button, Shift, X, Y );
		else if( FFieldTypes[ CoordX ] == SELL_PX && FOnTBarMouseDown != NULL )
			FOnTBarMouseDown( this, SelRec->GetSymbol(), SelRec->GetYearMonth(), nsOrderMessageDefine::sSell, SelRec->GetAskPx(), CallPut,Button, Shift, X, Y  );
		else if( FOnTBarMouseClick != NULL )
			FOnTBarMouseClick( this, SelRec->GetSymbol(), CallPut, Button, Shift, X, Y );
	}
	else
		FOnTBarStrikePxMouseDown( this, SelRec->GetSymbol(), cpNone );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
{
	TCustomGrid::MouseDown( Button, Shift, X, Y );
	if( Button == mbLeft )
	{
		TGridCoord Coord = MouseCoord( X, Y );
		if( Coord.Y < 2 && Coord.X == FStrikePxColIndex )
		{
			if( FOnTBarStrikePxClick != NULL )
				FOnTBarStrikePxClick( this );
		}
		else if( Coord.Y < 2 || Coord.Y > RowCount )
		{
			return;
		}
		else
		{
			TOptRecord* SelRec;

			if( Coord.X < FStrikePxColIndex )
			{
				if( (SelRec = (TOptRecord*)FCallList->Items[ Coord.Y - 2 ]) != NULL )
					HandleMouseDown( Button, Shift, X, Y, SelRec, Coord.X, cpCall );
			}
			else if( Coord.X > FStrikePxColIndex )
			{
				if((SelRec = (TOptRecord*)FPutList->Items[ Coord.Y - 2 ]) != NULL )
					HandleMouseDown( Button, Shift, X, Y, SelRec, Coord.X, cpPut );
			}
			else if( Coord.X == FStrikePxColIndex )
			{
				bool  IsCall;
				TRect CellR = CellRect( Coord.X, Coord.Y );

				if( X < CellR.Left + CellR.Width()/2 )
					IsCall = true;
				else
					IsCall = false;
				if( IsCall == true )
					SelRec = (TOptRecord*)FCallList->Items[ Coord.Y - 2 ];
				else
					SelRec = (TOptRecord*)FPutList->Items[ Coord.Y - 2 ];
				if( SelRec != NULL && FOnTBarStrikePxMouseDown != NULL )
				{
					if( IsCall == true )
						FOnTBarStrikePxMouseDown( this, SelRec->GetSymbol(), cpCall );
					else
						FOnTBarStrikePxMouseDown( this, SelRec->GetSymbol(), cpPut );
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::MouseMove(Classes::TShiftState Shift, int X, int Y)
{
	TGridCoord OrigCoord  = FMouseCoord;

	FMouseCoord = MouseCoord( X, Y );
	if( OrigCoord.X != FMouseCoord.X || OrigCoord.Y != FMouseCoord.Y )
	{
		///< Erase old hot track cell
		if( OrigCoord.X < ColCount && OrigCoord.X >= 0  && OrigCoord.Y < RowCount && OrigCoord.Y >= 0 )
			DrawCell( OrigCoord.X, OrigCoord.Y, CellRect( OrigCoord.X, OrigCoord.Y ), (TGridDrawState)0);
		///< Draw new hot track cell
		if( FMouseCoord.X < ColCount && FMouseCoord.X >= 0  && FMouseCoord.Y < RowCount && FMouseCoord.Y >= 0 )
			DrawCell( FMouseCoord.X, FMouseCoord.Y, CellRect( FMouseCoord.X, FMouseCoord.Y ), (TGridDrawState)0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::MouseLeave( System::TObject* Sender )
{
	if( FMouseCoord.X == FStrikePxColIndex )
		DrawCell( FMouseCoord.X, FMouseCoord.Y, CellRect( FMouseCoord.X, FMouseCoord.Y ), (TGridDrawState)0);
	FMouseCoord.X = -1;
	FMouseCoord.Y = -1;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::MouseEnter(  System::TObject* Sender )
{
//	if( ::GetFocus( ) != Handle )
//		::SetFocus( Handle );
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::WndProc( TMessage &Msg )
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
			 Msg.Result = 1;
			 break;
		default:
			 inherited::WndProc( Msg );
			 break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::TradetimeTimer( System::TObject* Sender )
{
	ExchangeInfo*  ExInfo;

	if( FStore != NULL && (ExInfo = FStore->GetExchangeInfo( FExchange, FSymbol )) != NULL )
	{
		UFC::AnsiString CloseTime = ExInfo->GetSessionEndTime( ExInfo->GetSessionCount()-1 ); // 15:00
		int iPos = CloseTime.AnsiPos(':');
		if( iPos > 0 )
		{
			TTime tNow   = StrToDateTime( Now().FormatString("hh:nn") );
			TTime tClose = StrToDateTime(CloseTime.c_str());
			double FTimeToClose = tClose - tNow;

			if( FTimeToClose < 0 )
				FTimeToClose = 0.0;
		}
	}
	else
	{
		FTimer->Enabled = false;
		FTimeToClose = 0.0;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetBKcol( TOptBKColName Col )
{
	if( FBKCol != Col )
	{
		FBKCol = Col;
		UpdateBKBar( true );
    }
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetHeaderCommodColor( TColor Col )
{
	FHeaderCommodColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetHeaderCommodBKColor( TColor Col )
{
	FHeaderCommodBKColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetHeaderCallColor( TColor Col )
{
	FHeaderCallColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetHeaderCallBKColor( TColor Col )
{
	FHeaderCallBKColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetHeaderPutColor( TColor Col )
{
	FHeaderPutColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetHeaderPutBKColor( TColor Col )
{
	FHeaderPutBKColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetFieldNameCallColor( TColor Col )
{
	FFieldNameCallColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetFieldNameCallBKColor( TColor Col )
{
	FFieldNameCallBKColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetFieldNamePutColor( TColor Col )
{
	FFieldNamePutColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetFieldNamePutBKColor( TColor Col )
{
	FFieldNamePutBKColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetFieldNameCommodColor( TColor Col )
{
	FFieldNameCommodColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetFieldNameCommodBKColor( TColor Col )
{
	FFieldNameCommodBKColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetCallBKColor( TColor Col )
{
	FCallBKColor  = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetPutBKColor( TColor Col )
{
	FPutBKColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetRisePxColor( TColor Col )
{
	FRisePxColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetFallPxColor( TColor Col )
{
	FFallPxColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetEqualPxColor( TColor Col )
{
	FEqualPxColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetHGridColor( TColor Col )
{
	FHGridColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetVGridColor( TColor Col )
{
	FVGridColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetBKBarColor( TColor Col )
{
	FBKBarColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetStkPxColor( TColor Col )
{
	FStkPxColor = Col;
}
//---------------------------------------------------------------------------
void __fastcall TOptionsStrikePriceView::SetStkPxBKColor( TColor Col )
{
	FStkPxBKColor = Col;
}
//---------------------------------------------------------------------------
namespace Optionsstrikepriceview
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TOptionsStrikePriceView)};
		RegisterComponents(L"Speedy", classes, 0);
	}
}
//---------------------------------------------------------------------------



