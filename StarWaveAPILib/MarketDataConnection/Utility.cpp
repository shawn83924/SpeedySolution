//---------------------------------------------------------------------------
#include <math.h>
#include "Utility.h"
//---------------------------------------------------------------------------
Lookup GLookup;
//---------------------------------------------------------------------------
Lookup::Lookup()
{
	FExMap.Add( "TAIFEX", etTAIFEX );
	FExMap.Add( "TFX",    etTAIFEX );

	FExMap.Add( "TWSE",   etTWStock );
	FExMap.Add( "OTC",    etTWStock );
	FExMap.Add( "TWSEOdd",etTWStock );// 3
	FExMap.Add( "OTCOdd", etTWStock );// 3
	FExMap.Add( "ES",     etTWStock );
	
	FExMap.Add( "SHFE",   etChinaFutures );
	FExMap.Add( "CZCE",   etChinaFutures );
	FExMap.Add( "DCE",    etChinaFutures );
	FExMap.Add( "CFFEX",  etChinaFutures );
	FExMap.Add( "INE",    etChinaFutures );
	FExMap.Add( "SSE",    etChinaFutures );

	FExMap.Add( "CME",    etCMELike );
	FExMap.Add( "CBOT",   etCMELike );
	FExMap.Add( "SGX",    etCMELike );
	FExMap.Add( "NYMEX",  etCMELike );
	FExMap.Add( "NYBOT",  etCMELike );
	
	FExMap.Add( "HKE",    etCMELike );
	FExMap.Add( "HKEx",   etCMELike );
	FExMap.Add( "TOCOM",  etCMELike );
	FExMap.Add( "OSE",    etCMELike );
	FExMap.Add( "TGE",    etCMELike );
    FExMap.Add( "LIF",    etCMELike );
    FExMap.Add( "EUX",    etCMELike );
    FExMap.Add( "LME",    etCMELike );
    FExMap.Add( "KRX",    etCMELike );
    FExMap.Add( "ICE",    etCMELike );
    FExMap.Add( "BMD",    etCMELike );
    FExMap.Add( "ASX",    etCMELike );
	
	//<CQG
	FExMap.Add( "CQG.SGX",    etCQGLike );
	FExMap.Add( "CQG.COMEX",  etCQGLike );
	FExMap.Add( "CQG.CBOT",   etCQGLike );
	FExMap.Add( "CQG.NYMEX",  etCQGLike );
	FExMap.Add( "CQG.CME",    etCQGLike );
	FExMap.Add( "CQG.EUREX",  etCQGLike );
	FExMap.Add( "CQG.HKF",    etCQGLike );
	FExMap.Add( "CQG.CBT",    etCQGLike );
	FExMap.Add( "CQG.EUX",    etCQGLike );
	FExMap.Add( "CQG.EUXTW",    etCQGLike );
	FExMap.Add( "CQG.HKE",    etCQGLike );
	FExMap.Add( "CQG.ICE",    etCQGLike );
	FExMap.Add( "CQG.ICESG",  etCQGLike );
	FExMap.Add( "CQG.IPE",    etCQGLike );
	FExMap.Add( "CQG.KBT",    etCQGLike );
	FExMap.Add( "CQG.LIF",    etCQGLike );
	FExMap.Add( "CQG.LME",    etCQGLike );
	FExMap.Add( "CQG.MTF",    etCQGLike );
	FExMap.Add( "CQG.NYB",    etCQGLike );
	FExMap.Add( "CQG.NYM",    etCQGLike );
	FExMap.Add( "CQG.SFE",    etCQGLike );
	FExMap.Add( "CQG.TCE",    etCQGLike );
	FExMap.Add( "CQG.CBOE",   etCQGLike );
	FExMap.Add( "CQG.OSE",    etCQGLike );

	FExMap.Add( "PATS.CME",    etCQGLike ); 
	FExMap.Add( "PATS.COMEX",  etCQGLike ); 
	FExMap.Add( "PATS.CBOT",   etCQGLike ); 
	FExMap.Add( "PATS.NYMEX",  etCQGLike );
	FExMap.Add( "PATS.SGX",    etCQGLike );
	FExMap.Add( "PATS.HKF",    etCQGLike );
	FExMap.Add( "PATS.EURONEXT",  etCQGLike );
	FExMap.Add( "PATS.TOCOM",  etCQGLike );
	FExMap.Add( "PATS.OSE",    etCQGLike );
	FExMap.Add( "PATS.EUREX",  etCQGLike );
	FExMap.Add( "PATS.ICEUS",  etCQGLike );
	FExMap.Add( "PATS.ICEEU",  etCQGLike );
	FExMap.Add( "PATS.CBT",    etCQGLike );
	FExMap.Add( "PATS.EUX",    etCQGLike );
	FExMap.Add( "PATS.EUXTW",  etCQGLike );
	FExMap.Add( "PATS.HKE",    etCQGLike );
	FExMap.Add( "PATS.ICE",    etCQGLike );
	FExMap.Add( "PATS.ICESG",  etCQGLike );
	FExMap.Add( "PATS.IPE",    etCQGLike );
	FExMap.Add( "PATS.KBT",    etCQGLike );
	FExMap.Add( "PATS.IPE",    etCQGLike );
	FExMap.Add( "PATS.LIF",    etCQGLike );
	FExMap.Add( "PATS.LME",    etCQGLike );
	FExMap.Add( "PATS.MTF",    etCQGLike );
	FExMap.Add( "PATS.NYB",    etCQGLike );
	FExMap.Add( "PATS.NYM",    etCQGLike );
	FExMap.Add( "PATS.SFE",    etCQGLike );
	FExMap.Add( "PATS.TCE",    etCQGLike );
	FExMap.Add( "PATS.CBOE",   etCQGLike );
	
	FDigiMap.Add( "TXF",  tdtTXFLike );
	FDigiMap.Add( "XIO",  tdtTXFLike );
	FDigiMap.Add( "MXF",  tdtTXFLike );
	FDigiMap.Add( "T5F",  tdtTXFLike );

	FDigiMap.Add( "FXF", tdtFXFLike );
	FDigiMap.Add( "MSF", tdtFXFLike );
	FDigiMap.Add( "GDF", tdtFXFLike );
	FDigiMap.Add( "TGF", tdtFXFLike );

	FDigiMap.Add( "EXF", tdtEXFLike );
	FDigiMap.Add( "GTF", tdtEXFLike );

	FDigiMap.Add( "GBF", tdtGBFLike );
	FDigiMap.Add( "CPF", tdtGBFLike );

	FDigiMap.Add( "TXO", tdtTXOLike );

	FDigiMap.Add( "TEO", tdtTEOLike );
	FDigiMap.Add( "TFO", tdtTEOLike );
	FDigiMap.Add( "MSO", tdtTEOLike );
	FDigiMap.Add( "GTO", tdtTEOLike );

	FDigiMap.Add( "TGO", tdtTGOLike );

	FDigiMap.Add( "RHO", tdtRHOLike );
	FDigiMap.Add( "RTO", tdtRHOLike );
        
}
//---------------------------------------------------------------------------
TExchangeType Lookup::ExchangeType( const UFC::AnsiString& Exchange )
{
	TExchangeType ExType;

	if( FExMap.GetObjectByKey( Exchange, ExType ) == true )
		return ExType;
	return etUnknownEx;
}
//---------------------------------------------------------------------------
TTAIFEXDigiType Lookup::TAIFEXDigiType( const UFC::AnsiString& Prod )
{
	TTAIFEXDigiType DigiType;

	if( FDigiMap.GetObjectByKey( Prod, DigiType ) == true )
		return DigiType;
	return tdtUnknown;
}
//---------------------------------------------------------------------------
double StdDev(double data[], int n)
{
	if(n == 0)
		return 0.0;
	double sum = 0;
	for(int i = 0; i < n; ++i)
	   sum += data[i];
	double mean = sum / n;
	double sq_diff_sum = 0;
	for(int i = 0; i < n; ++i)
	{
	   double diff = data[i] - mean;
	   sq_diff_sum += diff * diff;
	}
	double variance = sq_diff_sum / n;
	return sqrt(variance);
}
//---------------------------------------------------------------------------
BasicInformation::BasicInformation( void )
:FMsg( "|" )
,FEndDate("20150430")
,FFirstNoticeDate("")
,FMaturityDate("201504")
,FMaturityDate2("201504")
,FAliasName("")
,FBullPrice( 0.0 )
,FBearPrice( 0.0 )
,FBullPrice2( 0.0 )
,FBearPrice2( 0.0 )
,FBullPrice3( 0.0 )
,FBearPrice3( 0.0 )
,FRefPrice( 0.0 )
,FTick( 0.01 )
,FDigit( 2)
,FDecimalLocator( 2 )
,FDisplayName( NULL )
,FTradeFlag(true)
,FIsWarrant( false )
,FExecuteQty( 0 )
,FCancelQty( 0 )
,FShares( 0 )
,FExecuteRate( 0 )
,FTradeUnit( 1000 )
,FTimeDiff( 0 )
,FExpiryDays( 0 )
,FPreNCloseCount( 0 )
,FMaxHistoryVolDays( 0 )
,FNearMonthCode( 0 )
,FIsMostActive( 0 )
,FSettlementPx( 0.0 )
,FPreSettlementPx( 0.0 )
{

}
//---------------------------------------------------------------------------
BasicInformation::BasicInformation( const UFC::AnsiString& Msg )
:FMsg( "|" )
,FEndDate("20150430")
,FFirstNoticeDate("")
,FMaturityDate("201504")
,FMaturityDate2("201504")
,FAliasName("")
,FBullPrice( 0.0 )
,FBearPrice( 0.0 )
,FBullPrice2( 0.0 )
,FBearPrice2( 0.0 )
,FBullPrice3( 0.0 )
,FBearPrice3( 0.0 )
,FRefPrice( 0.0 )
,FTick( 0.01 )
,FDigit( 2)
,FDecimalLocator( 2 )
,FDisplayName( NULL )
,FTradeFlag(true)
,FIsWarrant( false )
,FExecuteQty( 0 )
,FCancelQty( 0 )
,FShares( 0 )
,FExecuteRate( 0 )
,FTradeUnit( 1000 )
,FTimeDiff( 0 )
,FExpiryDays( 0 )
,FPreNCloseCount( 0 )
,FMaxHistoryVolDays( 0 )
,FNearMonthCode( 0 )
,FIsMostActive( 0 )
,FSettlementPx( 0.0 )
,FPreSettlementPx( 0.0 )
{
	LoadFromString( Msg );
}
//---------------------------------------------------------------------------
void BasicInformation::LoadFromString( const UFC::AnsiString& Msg )
{
	UFC::AnsiString Value;
	TExchangeType   ExchangeType;

	FMsg.FromString( Msg );
	FMsg.Get( "Symbol", FSymbol );
	FAliasName = FSymbol;
	FMsg.Get( "Exchange", FExchange );
	ExchangeType = GLookup.ExchangeType( FExchange );
	FMsg.Get( "ProductID", FProductID );
	FMsg.Get( "EndDate", FEndDate );
	if( FMsg.Get( "MKT", Value ) )
		FMarket = (Market)Value.ToInt();
	if( FMsg.Get( "RefPx", Value ) )
		FRefPrice = Value.ToDouble();
	if( FMsg.Get( "BullPx", Value ) )
		FBullPrice = Value.ToDouble();
	if( FMsg.Get( "BearPx", Value ) )
		FBearPrice = Value.ToDouble();
		if( FMsg.Get( "BullPx2", Value ) )
		FBullPrice2 = Value.ToDouble();
	if( FMsg.Get( "BearPx2", Value ) )
		FBearPrice2 = Value.ToDouble();
		if( FMsg.Get( "BullPx3", Value ) )
		FBullPrice3 = Value.ToDouble();
	if( FMsg.Get( "BearPx3", Value ) )
		FBearPrice3 = Value.ToDouble();
	if( FMsg.Get( "StkPx", Value ) )
		FStrikePrice = Value.ToDouble();
	if( FMsg.Get( "CM", Value ) )
		FContractMultiplier = Value.ToDouble();
	if( FMsg.Get( "CP", Value ) )
	{
		if( Value == "C" )
			FCallPut = cpCall;
		else if( Value == "P" )
			FCallPut = cpPut;
		else
			FCallPut = cpNone;
	}
	if( FMsg.Get( "Tick", Value ) )
		FTick = Value.ToDouble();
	if( FMsg.Get("DecimalLocator", Value ) )
		FDecimalLocator = Value.ToInt();
	FMsg.Get( "MaturityDate", FMaturityDate );
	if( FSymbol.AnsiPos( '/' ) > 0 )
		FMsg.Get( "MaturityDate2", FMaturityDate2 );
	else
		FMaturityDate2 = FMaturityDate;
	if( FMsg.Get( "TradeFlag", Value ) )
	{
		if( Value.ToInt() > 0 )
			FTradeFlag = true;
		else
			FTradeFlag = false;
	}
	if( ExchangeType == etTAIFEX )
	{
		UFC::AnsiString Prefix = FSymbol.SubString( 0, 3 );
		TTAIFEXDigiType DigiType = GLookup.TAIFEXDigiType( Prefix );

		if( DigiType == tdtTXFLike )
			FDigit = 0;
		else if( DigiType == tdtFXFLike )
			FDigit = 1;
		else if( DigiType == tdtEXFLike )
			FDigit = 2;
		else if( DigiType == tdtGBFLike )
			FDigit = 3;
		else if( DigiType == tdtTXOLike )
		{
			if( FBearPrice < 50 )
				FDigit = 1;
			else
				FDigit = 0;
		}
		else if( DigiType == tdtTEOLike )
		{
			if( FBearPrice < 2.5 )
				FDigit = 3;
			else if( FBearPrice < 50 )
				FDigit = 2;
			else
				FDigit = 1;
		}
		else if( DigiType == tdtTGOLike )
		{
			FDigit = 1;
		}
		else if( DigiType == tdtRHOLike )
		{
			FDigit = 4;
		}
		else if( FMarket == mTWFutures )
		{
			///< ETF Futures
			if( FProductID == "NZF" || Prefix == "NYF" || Prefix == "OAF" )
			{
			   FDigit = 2;
			}
			else
			{
                FDigit = FDecimalLocator; // modify by joe for RMB Futures
				/*if( FBearPrice < 50 )
					FDigit = 2;
				else if( FBearPrice < 500 )
					FDigit = 1;
				else
					FDigit = 0;*/
			}

			if( FProductID == "TJF" )  // TOPIX���f
			{
				if( FBullPrice2 != 0 )
					FBullPrice = FBullPrice2;

				if( FBearPrice2 != 0 )
					FBearPrice = FBearPrice2;

				if( FBullPrice3 != 0 )
					FBullPrice = FBullPrice3;

				if( FBearPrice3 != 0 )
					FBearPrice = FBearPrice3;
            }
		}
		else
		{
			if( FBearPrice < 15 )
				FDigit = 2;
			else if( FBearPrice < 150  )
				FDigit = 1;
			else
				FDigit = 0;
		}
	}
	else if( ExchangeType == etChinaFutures )
	{
		if(FMsg.Get( "Digit", Value )) // 20150319 by Tim Lin // for "SSE" �{�f 510050.SS
		{
			FDigit = Value.ToInt();
			if( FDigit == 3 )
				FTick  = 0.001;
		}
		else
		{
			if( FTick >= 1 )
				FDigit = 0;
			else if ( FTick >= 0.5 )
				FDigit = 1;
			else if( FTick == 0.25 )
				FDigit = 2;
			else if( FTick >= 0.1 )
				FDigit = 1;
			else if( FTick >= 0.01 )
				FDigit = 2;
			else if ( FTick == 0.0025 )
				FDigit = 4;
			else if( FTick >= 0.001 )
				FDigit = 3;
			else if( FTick >= 0.0001 )
				FDigit = 4;
			else if( FTick >= 0.00001 )
				FDigit = 5;
		}
	}
	else if( ExchangeType == etTWStock )
	{
		if( FBearPrice >= 500 )      ///< 1~5
			FDigit = 0;
		else if( FBearPrice >= 50 ) ///< 0.1~0.5
			FDigit = 1;
		else
			FDigit = 2;
	}
	else if( ExchangeType == etCMELike && FMsg.Get( "Digit", Value ) )
		FDigit = Value.ToInt();
	else if( ExchangeType == etCQGLike ) //vicky
	{
		if ( FMsg.Get( "Digit", Value ) )
			FDigit = Value.ToInt();
		if ( FMsg.Get( "NearMonth", Value ) )
			FNearMonthCode = Value.ToInt();
		if ( FMsg.Get( "IsMostActive", Value ) )
		{
			if (Value.ToInt() > 0)
				FIsMostActive = Value.ToInt();
		}
		if ( FMsg.Get( "PreSettlementPx", Value ) )
		{
			FPreSettlementPx = Value.ToDouble();
		}
		if ( FMsg.Get( "SettlementPx", Value ) )
		{
			FSettlementPx = Value.ToDouble();
		}
		if ( FMsg.Get( "FirstNoticeDate", Value ) )
		{
			if( Value.Length() == 6 )
				FFirstNoticeDate = Value;
		}
	}

	if( ExchangeType == etTWStock )
	{
		if( FMsg.Get( "TradeUnit", Value ) )
			FTradeUnit = Value.ToInt();
		if( FMsg.Get( "IsWarrant", Value ) )
		{
			if( Value.ToInt() > 0 )
			{
				FIsWarrant = true;
				if( FMsg.Get( "ExecuteQty", Value ) )
					FExecuteQty = Value.ToInt();
				if( FMsg.Get( "CancelQty", Value ) )
					FExecuteQty = Value.ToInt();
				if( FMsg.Get( "Shares", Value ) )
					FShares = Value.ToInt();
				if( FMsg.Get( "ExecuteRate", Value ) )
					FExecuteRate = Value.ToDouble();
				FMsg.Get( "EndDate", FEndDate );
				FMsg.Get( "LastTradeDate", FLastTradeDate );
			}
		}
		if( FMsg.Get( "Chinese", Value ) )
			FChineseName = Value;
	}
	else
	{
     	FLastTradeDate = FEndDate;
	}

	if( FMsg.Get( "Spot", Value ) )
	{
		UFC::PStringList StringList;
		StringList.SetStrings( Value, "," );
		for( register int i = 0; i < StringList.ItemCount(); i++ )
			FSpotList.Add( StringList[i] );
	}
    if( FMsg.Get( "Underlying", Value ) )
		FUnderlying = Value;
	if( FMsg.Get( "TradeSymbol", Value ) == TRUE )
		FTradeSymbol = Value;
	else
		FTradeSymbol = FSymbol;
	if( !FMsg.Get( "Currency", FCurrency ) )
		FCurrency = "NTD";
	if( FMsg.Get( "TimeDiff", Value ) )
		FTimeDiff = Value.ToInt();
	if( FMsg.Get( "ExpiryDays", Value ) )
		FExpiryDays = Value.ToInt();
	if( FMsg.Get( "PreNClosePx", Value ) )
	{
		UFC::PStringList  slPreNClosePx;
		slPreNClosePx.SetStrings( Value, "," );
		FPreNCloseCount = slPreNClosePx.ItemCount();
		if(	FPreNCloseCount > MAX_PRE_CLOSE_DAYS )
			FPreNCloseCount = MAX_PRE_CLOSE_DAYS;
		for(int i = 0; i < FPreNCloseCount; i++ )
			FPreNClosePx[i] = slPreNClosePx[i].ToDouble() ;
        SetPreNChangeRange();
	}
}
//---------------------------------------------------------------------------
void BasicInformation::LoadFromStream( UFC::PStream* Stream )
{
	UFC::AnsiString DataString;

	DataString.LoadFromStream( Stream );
	LoadFromString( DataString );
}
//---------------------------------------------------------------------------
void BasicInformation::SaveToStream( UFC::PStream* Stream )
{
	UFC::AnsiString DataString = FMsg.ToString();

	DataString.SaveToStream( Stream );
}
//---------------------------------------------------------------------------
void BasicInformation::SetPreNChangeRange( void )
{
    FMaxHistoryVolDays = 0;
	for( int i = 0; i < FPreNCloseCount-1; i++ )
	{
		FPreNChangeRange[i] = (FPreNClosePx[i] - FPreNClosePx[i+1]) / FPreNClosePx[i+1];
		FMaxHistoryVolDays++;
    }	
}
//---------------------------------------------------------------------------
void BasicInformation::SetAliasName( const UFC::AnsiString& AliasName )
{
	FAliasName = AliasName;
}
//---------------------------------------------------------------------------
void BasicInformation::SetMaturityDate( const UFC::AnsiString& Date )
{
	FMaturityDate = Date;
}
//---------------------------------------------------------------------------
void BasicInformation::SetCallPut( CallPutCode CP )
{
	FCallPut = CP;
}
//---------------------------------------------------------------------------
void BasicInformation::SetStrikePrice( double Price )
{
	FStrikePrice = Price;
}
//---------------------------------------------------------------------------
void BasicInformation::SetBullPrice( double Price )
{
	FBullPrice = Price;
}
//---------------------------------------------------------------------------
void BasicInformation::SetBearPrice( double Price )
{
	FBearPrice = Price;
}
//---------------------------------------------------------------------------
void BasicInformation::SetBullPrice2( double Price )
{
	FBullPrice2 = Price;
}
//---------------------------------------------------------------------------
void BasicInformation::SetBearPrice2( double Price )
{
	FBearPrice2 = Price;
}
//---------------------------------------------------------------------------
void BasicInformation::SetBullPrice3( double Price )
{
	FBullPrice3 = Price;
}
//---------------------------------------------------------------------------
void BasicInformation::SetBearPrice3( double Price )
{
	FBearPrice3 = Price;
}
//---------------------------------------------------------------------------
void BasicInformation::SetRefPrice( double Price )
{
	FRefPrice = Price;
}
//---------------------------------------------------------------------------
void BasicInformation::SetPreSettlementPrice( double Price )
{
	FPreSettlementPx = Price;
}
//---------------------------------------------------------------------------
void BasicInformation::SetSettlementPrice( double Price )
{
	FSettlementPx = Price;
}
//---------------------------------------------------------------------------
void BasicInformation::SetDigit( int Digit )
{
	FDigit = Digit;
}
//---------------------------------------------------------------------------
void BasicInformation::SetDisplayName( wchar_t* DisplayName )
{
	size_t StrLength = wcslen( DisplayName ) + 1;

	if( FDisplayName != NULL )
		delete [] FDisplayName;
	FDisplayName = new wchar_t[ StrLength ];
	wcscpy( FDisplayName, DisplayName );
}
//---------------------------------------------------------------------------
void BasicInformation::SetContractMultiplier( double CM )
{
	FContractMultiplier = CM;
}
//---------------------------------------------------------------------------
double BasicInformation::GetStrikePrice( void )
{
	return FStrikePrice;
}
//---------------------------------------------------------------------------
double BasicInformation::GetBullPrice( void )
{
	return FBullPrice;
}
//---------------------------------------------------------------------------
double BasicInformation::GetBearPrice( void )
{
	return FBearPrice;
}
//---------------------------------------------------------------------------
double BasicInformation::GetBullPrice2( void )
{
	return FBullPrice2;
}
//---------------------------------------------------------------------------
double BasicInformation::GetBearPrice2( void )
{
	return FBearPrice2;
}
//---------------------------------------------------------------------------
double BasicInformation::GetBullPrice3( void )
{
	return FBullPrice3;
}
//---------------------------------------------------------------------------
double BasicInformation::GetBearPrice3( void )
{
	return FBearPrice3;
}
//---------------------------------------------------------------------------
double BasicInformation::GetRefPrice( void )
{
	return FRefPrice;
}
//---------------------------------------------------------------------------
Market BasicInformation::GetMarket( void )
{
	return FMarket;
}
//---------------------------------------------------------------------------
UFC::AnsiString& BasicInformation::GetEndDate( void )
{
	return FEndDate;
}
//---------------------------------------------------------------------------
UFC::AnsiString& BasicInformation::GetFirstNoticeDate( void )
{
	return FFirstNoticeDate;
}
//---------------------------------------------------------------------------
int BasicInformation::GetDigit( void )
{
	return FDigit;
}
//---------------------------------------------------------------------------
int BasicInformation::GetDecimalLocator( void )
{
	return FDecimalLocator;
}
//---------------------------------------------------------------------------
UFC::AnsiString BasicInformation::operator[] ( const UFC::AnsiString& FieldName )
{
	UFC::AnsiString Value( "" );
	FMsg.Get( FieldName, Value );
	return Value;
}
//---------------------------------------------------------------------------
CallPutCode BasicInformation::GetCallPut( void )
{
	return FCallPut;
}
//---------------------------------------------------------------------------
UFC::AnsiString& BasicInformation::GetMaturityDate( void )
{
	return FMaturityDate;
}
//---------------------------------------------------------------------------
UFC::AnsiString& BasicInformation::GetMaturityDate2( void )
{
	return FMaturityDate2;
}
//---------------------------------------------------------------------------
UFC::AnsiString BasicInformation::GetTickListSymbol( void )
{
	UFC::AnsiString TickSymbol;
	TickSymbol.Printf( "%s.%s", FExchange.c_str(), FSymbol.c_str() );
	return TickSymbol;
}
//---------------------------------------------------------------------------
bool BasicInformation::IsMultileg( void )
{
	if( ( FExchange == "TAIFEX" && FMarket == mTWFutures && FSymbol.AnsiPos( '/' ) > 0 ) || 
        ( FExchange == "TFX" && FMarket == mPATSFutures && FSymbol.AnsiPos( '/' ) > 0 ) )
		return true;
	return false;
}
//---------------------------------------------------------------------------
double BasicInformation::GetHistoryVol( int &iDays )
{
	if( iDays > FMaxHistoryVolDays )
		iDays = FMaxHistoryVolDays;
	return StdDev( FPreNChangeRange, iDays ) * sqrt( (float)YEAR_TRADING_DAYS );
}
//---------------------------------------------------------------------------
//
//	Class ExchangeInfo
//
//---------------------------------------------------------------------------
#define BEFORE_OPEN 30
#define AFTER_CLOSE 180*4
//---------------------------------------------------------------------------
ExchangeInfo::ExchangeInfo( const UFC::AnsiString& ExchangeName, UFC::UiniFile& ExchangeConfig )
:FExchangeName( ExchangeName )
,FAmendQty( false )
,FClAmendQty( true )
,FReplacePx( false )
,FClReplacePx( true )
,FDayTrade( false )
,FStopOrder( false )
,FSWLOrder( false )
,FTimeSpread( false )
,FROD( false )
,FIOC( false )
,FFOK( false )
,FMarketOrd( false )
,FRODMarket( true )
,FTicks( 0 )
,FExtraSecondBeforeOpen( BEFORE_OPEN )
,FExtraSecondAfterClose( AFTER_CLOSE )
,FTimeDiff( 8 )
,FTicksEx( 0 )
{
	UFC::AnsiString Value;

	ExchangeConfig.GetValue( FExchangeName, "Session", Value );
	FSessionCount = Value.ToInt();
	if( ExchangeConfig.GetValue( FExchangeName, "TimeDiff", Value ) )
		FTimeDiff = Value.ToInt();
	for( register int i = 1; i <= FSessionCount; i++ )
	{
		UFC::AnsiString StartName, EndName, StartValue, EndValue;
		UFC::UDateTime StartTime, EndTime, StartTimeEx, EndTimeEx;

		StartName.Printf( "Session%dStart", i );
		EndName.Printf( "Session%dEnd", i );
		ExchangeConfig.GetValue( FExchangeName, StartName, StartValue );
		ExchangeConfig.GetValue( FExchangeName, EndName, EndValue );
		FStartTimeList.Add( StartValue );
		FEndTimeList.Add( EndValue );

		if( i == 1 )
		{
			StartTime.setCurrent();
			EndTime.setCurrent();
		}
		else
		{
			StartTime = FEndList[i - 2];
			EndTime = FEndList[i - 2];
		}

		StartTime.setHour( StartValue.SubString( 0, StartValue.AnsiPos( ':' ) ).ToInt() );
		StartTime.setMinute( StartValue.SubString( StartValue.AnsiPos( ':' ) + 1, StartValue.Length() - StartValue.AnsiPos( ':' ) - 1 ).ToInt() );
		StartTime.setSecond( 0 );
		StartTime.setMillisecond( 0 );
		StartTime += 0;
		StartTimeEx = StartTime;

		if( i == 1 )
			StartTimeEx -= FExtraSecondBeforeOpen;

		EndTime.setHour( EndValue.SubString( 0, EndValue.AnsiPos( ':' ) ).ToInt() );
		EndTime.setMinute( EndValue.SubString( EndValue.AnsiPos( ':' ) + 1, EndValue.Length() - EndValue.AnsiPos( ':' ) - 1 ).ToInt() );
		EndTime.setSecond( 0 );
		EndTime.setMillisecond( 0 );
		EndTime += 0;
		EndTimeEx = EndTime;

		if( i == FSessionCount )
			EndTimeEx += FExtraSecondAfterClose;

		if( EndTime < StartTime )
		{
			EndTime += 86400;
			EndTimeEx += 86400;
		}

		if( i > 1 && StartTime < FEndList[i - 2] )
		{
			StartTime += 86400;
			StartTimeEx += 86400;
			EndTime += 86400;
			EndTimeEx += 86400;
		}

		FStartList.Add( StartTime );
		FEndList.Add( EndTime );
		FStartListEx.Add( StartTimeEx );
		FEndListEx.Add( EndTimeEx );

		FSessionTicks.Add( ( EndTime - StartTime )/60 );
		FTicks += ( EndTime - StartTime )/60;

		FSessionTicksEx.Add( ( EndTimeEx - StartTimeEx ) );
		FTicksEx += EndTimeEx - StartTimeEx;
	}

	if( ExchangeConfig.GetValue( FExchangeName, "StopOrd", Value ) && Value == "Y" )
		FStopOrder = true;
	if( ExchangeConfig.GetValue( FExchangeName, "SWLOrd", Value ) && Value == "Y" )
		FSWLOrder = true;
	if( ExchangeConfig.GetValue( FExchangeName, "AmendQty", Value ) && Value == "Y" )
		FAmendQty = true;
	if( ExchangeConfig.GetValue( FExchangeName, "ClientAmendQty", Value ) && Value == "N" )
		FAmendQty = false;
	if( ExchangeConfig.GetValue( FExchangeName, "ReplacePx", Value ) && Value == "Y" )
		FReplacePx = true;
	if( ExchangeConfig.GetValue( FExchangeName, "ClientReplacePx", Value ) && Value == "N" )
		FClReplacePx = false;
	if( ExchangeConfig.GetValue( FExchangeName, "DayTrade", Value ) && Value == "Y" )
		FDayTrade = true;
	if( ExchangeConfig.GetValue( FExchangeName, "TimeSpread", Value ) && Value == "Y" )
		FTimeSpread = true;
	if( ExchangeConfig.GetValue( FExchangeName, "ROD", Value ) && Value == "Y" )
		FROD = true;
	if( ExchangeConfig.GetValue( FExchangeName, "IOC", Value ) && Value == "Y" )
		FIOC = true;
	if( ExchangeConfig.GetValue( FExchangeName, "FOK", Value ) && Value == "Y" )
		FFOK = true;
	if( ExchangeConfig.GetValue( FExchangeName, "Market", Value ) && Value == "Y" )
		FMarketOrd = true;
	if( FExchangeName == "TAIFEX" ||
		FExchangeName == "TAIFEX.RMB" ||
		//FExchangeName == "TFX" ||
		FExchangeName == "SHFE" ||
		FExchangeName == "CZCE" ||
		FExchangeName == "DCE" ||
		FExchangeName == "CFFEX" )
		FRODMarket = false;
}
//---------------------------------------------------------------------------
void ExchangeInfo::IndexToTime( int i, UFC::AnsiString& TimeStr )
{
	i = i % FTicks;

	if( i <= 0 )
		TimeStr.Printf( "%02d:%02d", FStartList[0].getHour(), FStartList[0].getMinute() );
	else if( i >= FTicks - 1 )
		TimeStr.Printf( "%02d:%02d", FEndList[FEndList.ItemCount() - 1].getHour(), FEndList[FEndList.ItemCount() - 1].getMinute() - 1 );
	else
	{
		int Cum = 0;
		int Session = 0;
		int Index = i;

		while( i >= Cum )
		{
			Cum += FSessionTicks[Session];

			if( i < Cum )
				break;
			else
			{
				Index -= FSessionTicks[Session];
				Session++;
			}
		}
		UFC::UDateTime Time = FStartList[Session];
		Time += Index * 60;
		TimeStr.Printf( "%02d:%02d", Time.getHour(), Time.getMinute() );
	}
}
//---------------------------------------------------------------------------
int	ExchangeInfo::TimeToIndex( const UFC::UDateTime& DateTime )
{
	int Index = 0;
	UFC::UDateTime RefTime = DateTime;
	RefTime.setSecond( 0 );
	RefTime.setMillisecond( 0 );
	RefTime.setYear( FEndList.GetItem( FEndList.ItemCount() - 1 ).getYear() );
	RefTime.setMonth( FEndList.GetItem( FEndList.ItemCount() - 1 ).getMonth() );
	RefTime.setDate( FEndList.GetItem( FEndList.ItemCount() - 1 ).getDate() );
	RefTime += 0;

	if( RefTime > FEndList.GetItem( FEndList.ItemCount() - 1 ) )
		RefTime -= 86400;

	if( RefTime >= FStartList[0] )
	{
		for( register int i = 0; i < FSessionCount; i++ )
		{
			if( RefTime < FEndList[i] )
			{
				int Offset = ( RefTime - FStartList[i] )/60;
				if( Offset > 0 )
					Index += Offset;
				break;
			}
			else
			{
				Index += ( FEndList[i] - FStartList[i] )/60;
			}
		}
	}
	return Index;
}
//---------------------------------------------------------------------------
int ExchangeInfo::TimeToIndex( int hour, int minute )
{
	UFC::UDateTime time;
	time.setHour( hour );
	time.setMinute( minute );
	return TimeToIndex( time );
}
//---------------------------------------------------------------------------
void ExchangeInfo::IndexToTimeEx( int i, UFC::AnsiString& TimeStr )
{
	i = i % FTicksEx;

	if( i <= 0 )
		TimeStr.Printf( "%02d:%02d:%02d", FStartListEx[0].getHour(), FStartListEx[0].getMinute(), FStartListEx[0].getSecond() );
	else if( i >= FTicksEx - 1 )
		TimeStr.Printf( "%02d:%02d:%02d", FEndListEx[FEndListEx.ItemCount() - 1].getHour(), FEndListEx[FEndListEx.ItemCount() - 1].getMinute(), FEndListEx[FEndListEx.ItemCount() - 1].getSecond() );
	else
	{
		int Cum = 0;
		int Session = 0;
		int Index = i;

		while( i >= Cum )
		{
			Cum += FSessionTicksEx[Session];

			if( i < Cum )
				break;
			else
			{
				Index -= FSessionTicksEx[Session];
				Session++;
			}
		}
		UFC::UDateTime Time = FStartListEx[Session];
		Time += Index;
		TimeStr.Printf( "%02d:%02d:%02d", Time.getHour(), Time.getMinute(), Time.getSecond() );
	}
}
//---------------------------------------------------------------------------
int	ExchangeInfo::TimeToIndexEx( const UFC::UDateTime& DateTime )
{
	int Index = 0;
	UFC::UDateTime RefTime = DateTime;
	RefTime.setMillisecond( 0 );
	RefTime.setYear( FEndListEx.GetItem( FEndListEx.ItemCount() - 1 ).getYear() );
	RefTime.setMonth( FEndListEx.GetItem( FEndListEx.ItemCount() - 1 ).getMonth() );
	RefTime.setDate( FEndListEx.GetItem( FEndListEx.ItemCount() - 1 ).getDate() );
	RefTime += 0;

	if( RefTime > FEndListEx.GetItem( FEndListEx.ItemCount() - 1 ) )
		RefTime -= 86400;

	if( RefTime >= FStartListEx[0] )
	{
		for( register int i = 0; i < FSessionCount; i++ )
		{
			if( RefTime < FEndListEx[i] )
			{
				int Offset = RefTime - FStartListEx[i];
				if( Offset > 0 )
					Index += Offset;
				break;
			}
			else
			{
				Index += FEndListEx[i] - FStartListEx[i];
			}
		}
	}
	return Index;
}
//---------------------------------------------------------------------------
int ExchangeInfo::TimeToIndexEx( int hour, int minute, int second )
{
	UFC::UDateTime time;
	time.setHour( hour );
	time.setMinute( minute );
	time.setSecond( second );
	return TimeToIndexEx( time );
}
//---------------------------------------------------------------------------
#ifdef _WIN32
int __fastcall GenerateTicks( const UFC::AnsiString& Exchange,
							  const UFC::AnsiString& Symbol,
							  double BullPx,
							  double BearPx,
							  std::vector<double>& TickVector )
{
	double Tick = -1;
	if( Exchange == "TAIFEX" || Exchange == "TFX" )
	{
		UFC::AnsiString Prefix( Symbol.SubString( 0, 2 ) );

		if( Symbol.Length() == 5 || Symbol.Length() == 8 ) /// Futures, Fix tick.
		{
			double TickPx = BullPx;
			if( Prefix == "TX" || Prefix == "MX" || Prefix == "T5" || Prefix == "XI" )
				Tick = 1;
			else if( Prefix == "EX" || Prefix == "GT" )
				Tick = 0.05;
			else if( Prefix == "FX" )
				Tick = 0.2;
			else if( Prefix == "GB" || Prefix == "CP" )
				Tick = 0.005;
			else if( Prefix == "MS" || Prefix == "GD" )
				Tick = 0.1;
			else if( Prefix == "TG" )
				Tick = 0.5;
			if( Tick > 0 )
			{
				while( TickPx >= BearPx )
				{
					TickVector.push_back( TickPx );
					TickPx -= Tick;
				}
			}
			else ///< Stock Futures
			{
            	int TickPx = UFC::DoubleToInt( BullPx, 3 );
            	int EndPx = UFC::DoubleToInt( BearPx , 3 );
				while( TickPx >= EndPx )
				{
					TickVector.push_back( UFC::IntToDouble( TickPx, 3 ) );
					if( TickPx > 1000000 )
						TickPx -= 5000;
					else if( TickPx > 500000 )
						TickPx -= 1000;
					else if( TickPx > 100000 )
						TickPx -= 500;
					else if( TickPx > 50000 )
						TickPx -= 100;
					else if( TickPx > 10000 )
						TickPx -= 50;
					else
						TickPx -= 10;
				}
			}
		}
		else ///< Options
		{
			double TickPx = BullPx;
			if( Prefix == "TX")
			{
				while( TickPx >= BearPx )
				{
					TickVector.push_back( TickPx );
					if( TickPx > 1000 )
						TickPx -= 10;
					else if( TickPx > 500 )
						TickPx -= 5;
					else if( TickPx > 50 )
						TickPx -= 1;
					else if( TickPx > 10 )
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 50, 2 );
					}
					else
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 10, 2 );
					}
				}
			}
			else if( Prefix == "TE" || Prefix == "MS" || Prefix == "GT" )
			{
				while( TickPx >= BearPx )
				{
					TickVector.push_back( TickPx );
					if( TickPx > 50 )
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 50, 2 );
					}
					else if( TickPx > 25 )
					{
						int TmpValue = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 25, 2 );
					}
					else if( TickPx > 2.5 )
					{
						int TmpValue = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 5, 2 );

					}
					else if( TickPx > 0.5 )
					{
						int TmpValue = UFC::DoubleToInt( TickPx, 3 );
						TickPx = UFC::IntToDouble( TmpValue - 25, 3 );
					}
					else
					{
						int TmpValue = UFC::DoubleToInt( TickPx, 3 );
						TickPx = UFC::IntToDouble( TmpValue - 5, 3 );
					}
				}
			}
			else if( Prefix == "TF" )
			{
				while( TickPx >= BearPx )
				{
					TickVector.push_back( TickPx );
					if( TickPx > 200 )
						TickPx -= 2;
					else if( TickPx > 100 )
						TickPx -= 1;
					else if( TickPx > 10 )
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 20, 2 );

					}
					else if( TickPx > 2 )
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 10, 2 );
					}
					else
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 2, 2 );
					}
				}
			}
			else if( Prefix == "XI" )
			{
				while( TickPx >= BearPx )
				{
					TickVector.push_back( TickPx );
					if( TickPx > 2000 )
						TickPx -= 20;
					else if( TickPx > 1000 )
						TickPx -= 10;
					else if( TickPx > 100 )
						TickPx -= 2;
					else if( TickPx > 20 )
						TickPx -= 1;
					else
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 20, 2 );
					}
				}
			}
			else if( Prefix == "TG" )
			{
				while( TickPx >= BearPx )
				{
					TickVector.push_back( TickPx );
					TickPx -= 0.5;
				}
			}
			else ///< Stock Options
			{
				while( TickPx >= BearPx )
				{
					TickVector.push_back( TickPx );
					if( TickPx > 1000 )
						TickPx -= 5;
					else if( TickPx > 150 )
						TickPx -= 1;
					else if( TickPx > 50 )
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 50, 2 );
					}
					else if( TickPx > 15 )
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 10, 2 );
					}
					else if( TickPx > 5 )
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 5, 2 );
					}
					else
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 1, 2 );
					}
				}
			}
		}
	}
	else
	{
		int Tick = UFC::DoubleToInt( 0.2, 3 );
		int TickPx = UFC::DoubleToInt( BullPx, 3 );
		int EndPx = UFC::DoubleToInt( BearPx , 3 );
		if( Tick > 0 )
		{
			while( TickPx >= EndPx )
			{
				TickVector.push_back( UFC::IntToDouble( TickPx, 3 ) );
				TickPx -= Tick;
			}
		}
	}
	return TickVector.size();
}
#endif
//---------------------------------------------------------------------------



