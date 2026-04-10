//---------------------------------------------------------------------------
#include <math.h>
#include <bitset>
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
	FExMap.Add( "HKE",    etCMELike ); // CTF
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
	FExMap.Add( "CQG.CME",       etCQGLike );
    FExMap.Add( "CQG.COMEX",     etCQGLike );
    FExMap.Add( "CQG.CBOT",      etCQGLike );
	FExMap.Add( "CQG.CBT",       etCQGLike );
    FExMap.Add( "CQG.NYMEX",     etCQGLike );
	FExMap.Add( "CQG.NYM",       etCQGLike );
    FExMap.Add( "CQG.SGX",       etCQGLike );
	FExMap.Add( "CQG.HKF",       etCQGLike );
	FExMap.Add( "CQG.HKE",       etCQGLike );
	FExMap.Add( "CQG.EURONEXT",  etCQGLike );
	
    FExMap.Add( "CQG.EUREX",     etCQGLike );
	FExMap.Add( "CQG.EUX",       etCQGLike );
	FExMap.Add( "CQG.EUXTW",     etCQGLike );
	
	FExMap.Add( "CQG.TOCOM",     etCQGLike );
    FExMap.Add( "CQG.TCE",       etCQGLike );
    FExMap.Add( "CQG.OSE",       etCQGLike );
	
    FExMap.Add( "CQG.NYB",       etCQGLike );
	FExMap.Add( "CQG.NYBOT",     etCQGLike );
    
    FExMap.Add( "CQG.ICE",       etCQGLike );
	FExMap.Add( "CQG.ICEUS",     etCQGLike );
	FExMap.Add( "CQG.ICEEU",     etCQGLike );
	FExMap.Add( "CQG.ICEUK",     etCQGLike );
    FExMap.Add( "CQG.ICESG",     etCQGLike );
	
    FExMap.Add( "CQG.IPE",       etCQGLike );
    FExMap.Add( "CQG.KBT",       etCQGLike );
    FExMap.Add( "CQG.LIF",       etCQGLike );
    FExMap.Add( "CQG.LME",       etCQGLike );
    FExMap.Add( "CQG.MTF",       etCQGLike );
    FExMap.Add( "CQG.SFE",       etCQGLike );
    FExMap.Add( "CQG.CBOE",      etCQGLike );

	FExMap.Add( "PATS.CME",      etCQGLike ); 
	FExMap.Add( "PATS.COMEX",    etCQGLike ); 
	FExMap.Add( "PATS.CBOT",     etCQGLike ); 
	FExMap.Add( "PATS.CBT",      etCQGLike );
	FExMap.Add( "PATS.NYMEX",    etCQGLike );
	FExMap.Add( "PATS.NYM",      etCQGLike );
	FExMap.Add( "PATS.SGX",      etCQGLike );
	FExMap.Add( "PATS.HKF",      etCQGLike );
	FExMap.Add( "PATS.HKE",      etCQGLike );
	FExMap.Add( "PATS.EURONEXT", etCQGLike );
	
	FExMap.Add( "PATS.EUREX",    etCQGLike );
	FExMap.Add( "PATS.EUX",      etCQGLike );
	FExMap.Add( "PATS.EUXTW",    etCQGLike );
	
	FExMap.Add( "PATS.TOCOM",    etCQGLike );
	FExMap.Add( "PATS.TCE",      etCQGLike );
	FExMap.Add( "PATS.OSE",      etCQGLike );
	
	FExMap.Add( "PATS.ICE",      etCQGLike );
	FExMap.Add( "PATS.ICEUS",    etCQGLike );
	FExMap.Add( "PATS.ICEEU",    etCQGLike );
	FExMap.Add( "PATS.ICEUK",    etCQGLike );
	FExMap.Add( "PATS.ICESG",    etCQGLike );
	
	FExMap.Add( "PATS.NYB",      etCQGLike );
	FExMap.Add( "PATS.NYBOT",    etCQGLike );
	
	FExMap.Add( "PATS.IPE",      etCQGLike );
	FExMap.Add( "PATS.KBT",      etCQGLike );
	FExMap.Add( "PATS.LIF",      etCQGLike );
	FExMap.Add( "PATS.LME",      etCQGLike );
	FExMap.Add( "PATS.MTF",      etCQGLike );
	FExMap.Add( "PATS.SFE",      etCQGLike );
	FExMap.Add( "PATS.CBOE",     etCQGLike );
	
	FExMap.Add( "TT.CME",        etCQGLike );
    FExMap.Add( "TT.COMEX",      etCQGLike );
    FExMap.Add( "TT.CBOT",       etCQGLike );
	FExMap.Add( "TT.CBT",        etCQGLike );
    FExMap.Add( "TT.NYMEX",      etCQGLike );
	FExMap.Add( "TT.NYM",        etCQGLike );
    FExMap.Add( "TT.SGX",        etCQGLike );
	FExMap.Add( "TT.HKF",        etCQGLike );
	FExMap.Add( "TT.HKE",        etCQGLike );
	FExMap.Add( "TT.EURONEXT",   etCQGLike );
	
    FExMap.Add( "TT.EUREX",      etCQGLike );
	FExMap.Add( "TT.EUX",        etCQGLike );
	FExMap.Add( "TT.EUXTW",      etCQGLike );
	
	FExMap.Add( "TT.TOCOM",      etCQGLike );
    FExMap.Add( "TT.TCE",        etCQGLike );
    FExMap.Add( "TT.OSE",        etCQGLike );
	
    FExMap.Add( "TT.NYB",        etCQGLike );
	FExMap.Add( "TT.NYBOT",      etCQGLike );
    
    FExMap.Add( "TT.ICE",        etCQGLike );
	FExMap.Add( "TT.ICEUS",      etCQGLike );
	FExMap.Add( "TT.ICEEU",      etCQGLike );
	FExMap.Add( "TT.ICEUK",      etCQGLike );
    FExMap.Add( "TT.ICESG",      etCQGLike );
	
    FExMap.Add( "TT.IPE",        etCQGLike );
    FExMap.Add( "TT.KBT",        etCQGLike );
    FExMap.Add( "TT.LIF",        etCQGLike );
    FExMap.Add( "TT.LME",        etCQGLike );
    FExMap.Add( "TT.MTF",        etCQGLike );
    FExMap.Add( "TT.SFE",        etCQGLike );
    FExMap.Add( "TT.CBOE",       etCQGLike );
	
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

	if( FExMap.GetObjectByKey( Exchange, ExType ) == TRUE ) /// Allen Modify at 20190615
		return ExType;
	return etUnknownEx;
}
//---------------------------------------------------------------------------
TTAIFEXDigiType Lookup::TAIFEXDigiType( const UFC::AnsiString& Prod )
{
	TTAIFEXDigiType DigiType;

	if( FDigiMap.GetObjectByKey( Prod, DigiType ) == TRUE ) /// Allen Modify at 20190615
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
,FMaturityDate1("")
,FMaturityDate2("")
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
,FStockWarnCode(0)
,FStockDayTradingIndicator( ' ' )
,FStockIsMarginSale( false )
,FStockIsSecuritiesLendingSale( false )
,FStockIsNonTenTWDValue( false )
,FStockIsAbnormalRecommendation( false )
,FStockIsAbnormalSecurities( false )
{

}
//---------------------------------------------------------------------------
BasicInformation::BasicInformation( const UFC::AnsiString& Msg )
:FMsg( "|" )
,FEndDate("20150430")
,FFirstNoticeDate("")
,FMaturityDate("201504")
,FMaturityDate1("")
,FMaturityDate2("")
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
,FStockWarnCode(0)
,FStockDayTradingIndicator( ' ' )
,FStockIsMarginSale( false )
,FStockIsSecuritiesLendingSale( false )
,FStockIsNonTenTWDValue( false )
,FStockIsAbnormalRecommendation( false )
,FStockIsAbnormalSecurities( false )
{
	LoadFromString( Msg );
}
//---------------------------------------------------------------------------
// Tick Rule for TWSE
//
//-----------------------
// Price range  | Tick
//-----------------------
// 0.01~10      | 0.01
// 10~50        | 0.05
// 50~100       | 0.1
// 100~500      | 0.5
// 500~1000     | 1
// > 1000       | 5
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
	if( FMarket == mTSE || FMarket == mOTC )
		FContractMultiplier = 1000.0;
	else if( FMsg.Get( "CM", Value ) )
		FContractMultiplier = Value.ToDouble();
	else
		FContractMultiplier = 50.0;
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
    {
        if( FMsg.Get("MaturityDate1", Value ))
            FMaturityDate1 = Value;
        if( FMsg.Get("MaturityDate2", Value ))
            FMaturityDate2 = Value;
    }
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
		
		if ( FMsg.Get( "StatusCode", Value ) )
		{
			int StockStatus = Value.ToInt();
			UFC::AnsiString StockStatus_bit = std::bitset<STOCK_STATUS_BIT>(StockStatus).to_string().c_str();
			int BitLastPosition = StockStatus_bit.Length() -1;
			FStockWarnCode = StockStatus % 8;
			
			if (StockStatus_bit[BitLastPosition-3] == '1')
				FStockDayTradingIndicator = 'A';
			else if (StockStatus_bit[BitLastPosition-4] == '1')
				FStockDayTradingIndicator = 'B';
			
			if (StockStatus_bit[BitLastPosition-5] == '1')
				FStockIsMarginSale = true;
			
			if (StockStatus_bit[BitLastPosition-6] == '1')
				FStockIsSecuritiesLendingSale = true;
			
			if (StockStatus_bit[BitLastPosition-7] == '1')
				FStockIsNonTenTWDValue = true;
			
			if (StockStatus_bit[BitLastPosition-8] == '1')
				FStockIsAbnormalRecommendation = true; 
			
			if (StockStatus_bit[BitLastPosition-9] == '1')
				FStockIsAbnormalSecurities = true;

		}
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
			FPreSettlementPx = Value.ToDouble();
		if ( FMsg.Get( "SettlementPx", Value ) )
			FSettlementPx = Value.ToDouble();
		if ( FMsg.Get( "FirstNoticeDate", Value ) )
		{
			if( Value.Length() > 0 )
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
UFC::AnsiString& BasicInformation::GetMaturityDate1( void )
{
	return FMaturityDate1;
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
,FMinutes( 0 )
,FExtraSecondBeforeOpen( BEFORE_OPEN )
,FExtraSecondAfterClose( AFTER_CLOSE )
,FTimeDiff( 8 )
,FMinutesEx( 0 )
,FSupportOffhour( false )
,FDays( 7 )
{
	LoadSessions( ExchangeConfig );
	LoadTradingSupports( ExchangeConfig );
	FOffhourBeginMinutes = FDays * FMinutes;
}
//---------------------------------------------------------------------------
void ExchangeInfo::GetHMFromString( UFC::AnsiString& Time, int& H, int& M )
{
	int SplitPos = Time.AnsiPos( ':' );

	H = Time.SubString( 0, SplitPos ).ToInt();
	M = Time.SubString( SplitPos + 1, Time.Length() - SplitPos - 1 ).ToInt();
}
//---------------------------------------------------------------------------
void ExchangeInfo::LoadSessions( UFC::UiniFile& ExchangeConfig )
{
	UFC::AnsiString SessionCount;

	if( ExchangeConfig.GetValue( FExchangeName, "Session", SessionCount ) == true ) ///< How many sessions?
	{
		UFC::UDateTime  Now;
		int             SecDiff;

		FSessionCount = SessionCount.ToInt();
		///< Check open program time.
		if( Now.getHour() <= 5 && Now.getHour() > 0 ) ///< Now 0:XX~5:XX
			SecDiff = -86400;  ///< Open program after 23:59:59, Trading date should be Prev. day.
		else
			SecDiff = 0; ///< Today.
		///< Read Sessions setting
		for( register int i = 1; i <= FSessionCount; i++ )
		{
			UFC::AnsiString StartName, EndName, StartValue, EndValue;

			StartName.Printf( "Session%dStart", i );
			EndName.Printf( "Session%dEnd", i );
			if( ExchangeConfig.GetValue( FExchangeName, StartName, StartValue ) == true &&
				ExchangeConfig.GetValue( FExchangeName, EndName, EndValue ) == true )
			{
				UFC::UDateTime  StartTimeEx, EndTimeEx;
				int             Hour,Minute;
				///< Add Start/End time string to list.
				FStartTimeList.Add( StartValue );
				FEndTimeList.Add( EndValue );
				///< Setup Start Time.
				GetHMFromString( StartValue, Hour, Minute );
				UFC::UDateTime StartTime( Hour, Minute, 0,0, 0 );
				StartTime += SecDiff;
				StartTimeEx = StartTime;
				///< Setup End Time.
				GetHMFromString( EndValue, Hour, Minute );
				UFC::UDateTime EndTime( Hour, Minute, 0,0, 0 );
				EndTime += SecDiff;
				EndTimeEx = EndTime;
				///< Check session seconds for StockChartEx
				if( i == 1 ) ///< First session
					StartTimeEx -= FExtraSecondBeforeOpen;
				if( i == FSessionCount ) ///< Last session
					EndTimeEx += FExtraSecondAfterClose;
				///< This session cross day
				if( EndTime < StartTime )
				{
					EndTime   += 86400;
					EndTimeEx += 86400;
				}
				///< Session Start time less than the prev session end time.(Session cross day)
				if( i > 1 && StartTime < FEndList[i - 2] )
				{
					StartTime   += 86400;
					StartTimeEx += 86400;
					EndTime     += 86400;
					EndTimeEx   += 86400;
				}
				///< Add Start/End time to session time list.
				FStartList.Add( StartTime );
				FEndList.Add( EndTime );
				FStartListEx.Add( StartTimeEx );
				FEndListEx.Add( EndTimeEx );
				///< Session times.(Minutes/Secends)
				FSessionMinutes.Add( ( EndTime - StartTime )/60 );
				FSessionSecendsEx.Add( ( EndTimeEx - StartTimeEx ) );
				FMinutes   += ( EndTime - StartTime )/60; ///< Trading minutes
				FMinutesEx += EndTimeEx - StartTimeEx;    ///< Trading secends
			}
		}
	}
}
//---------------------------------------------------------------------------
void ExchangeInfo::LoadTradingSupports( UFC::UiniFile& ExchangeConfig )
{
	UFC::AnsiString Value;

	if( ExchangeConfig.GetValue( FExchangeName, "TimeDiff", Value ) ) ///< Time Zone.(Taipei +8)
		FTimeDiff = Value.ToInt();
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
void ExchangeInfo::SetTAIFEXOffhour( bool SupportOffHour )
{
	FSupportOffhour = SupportOffHour;
	FOffhourBeginMinutes = FDays * FMinutes;
	if( FSupportOffhour == true )
	{
		UFC::UDateTime LastSessionEnd( FEndList.GetItem( FEndList.ItemCount() - 1 ) );

		FOffhourOpenTime.setMillisecond( 0 );
		FOffhourOpenTime.setMicrosecond( 0 );
		FOffhourOpenTime.setSecond( 0 );
		FOffhourOpenTime.setMinute( 0 );
		FOffhourOpenTime.setHour( 15 );
		FOffhourOpenTime.setDate( LastSessionEnd.getDate() );
		FOffhourOpenTime.setMonth( LastSessionEnd.getMonth() );
		FOffhourOpenTime.setYear( LastSessionEnd.getYear() );
		FOffhourOpenTime += 0;
		FOffhourCloseTime = FOffhourOpenTime;
		FOffhourCloseTime += (14*3600);
	}
}
//---------------------------------------------------------------------------
void ExchangeInfo::SetKBarDays( int Days )
{
	FDays = Days;
	FOffhourBeginMinutes = FDays * FMinutes;
}
//---------------------------------------------------------------------------
//
// Covnert KBar index to Time String
// In: Index
//     from 0 to Day 1 minute K Bars count * Histroy Days + Today's 1 minute K Bars count.
// Out: TimeStr
//     format HH:MM
//
//---------------------------------------------------------------------------
void ExchangeInfo::IndexToTime( int i, UFC::AnsiString& TimeStr )
{
	if( FSupportOffhour == true && i >= FOffhourBeginMinutes ) ///< TAIFEX Offhour session.
	{
		int Index = i - FOffhourBeginMinutes + 1; ///< Today's minutes.(index)
		int H = (15 + Index/60)%24;               ///< From 15 PM to 5 AM.
		int M = Index%60;                         ///< Minutes

		TimeStr.Printf( "%02d:%02d", H, M );
	}
	else
	{
		i = i % FMinutes; ///< Today's minutes.(index)

		if( i <= 0 )  ///< Less then 0, Get the Start Time
			TimeStr.Printf( "%02d:%02d", FStartList[0].getHour(), FStartList[0].getMinute() + 1 );
		else if( i >= FMinutes - 1 ) ///< Last minutes.
			TimeStr.Printf( "%02d:%02d", FEndList[FEndList.ItemCount() - 1].getHour(), FEndList[FEndList.ItemCount() - 1].getMinute()  );
		else
		{
			int Cum = 0;     ///< Cum Minutes sessions.
			int Session = 0; ///< Session count.
			int Index = i;   ///< Minutes in this trading session.

			while( i >= Cum )
			{
				Cum += FSessionMinutes[Session];
				if( i >= Cum ) ///< Not in this Trading Session, try next.
				{
					Index -= FSessionMinutes[Session];
					Session++;
				}
				else ///< In this session.
					break;
			}
			UFC::UDateTime Time = FStartList[Session];
			Time += ((Index+1) * 60);
			TimeStr.Printf( "%02d:%02d", Time.getHour(), Time.getMinute() );
		}
	}
}
//---------------------------------------------------------------------------
//
// Is the given time in the trading session?
// In: hour
// In: mimutes
// return: true: in trading time
//         false: market closed.
//
//---------------------------------------------------------------------------
bool ExchangeInfo::IsTradingTime( int hour, int minute, bool IsToday )
{
	int TimeInt = hour*100 + minute; ///< format: HHMM

	///< Search for each session time.
	for( register int i = 0; i < FSessionCount; i++ )
	{
		int StartInt = FStartList[i].getHour()*100 + FStartList[i].getMinute();
		int EndInt   = FEndList[i].getHour()*100 + FEndList[i].getMinute();
		if( TimeInt >= StartInt && TimeInt < EndInt )
			return true;
	}
	///< Not in Normal trading session
	///< Check 15:00~05:00 is TAIFEX offhour trading session.
	if( FSupportOffhour == true  && IsToday == true  )
	{
		if( TimeInt >= 1500 || TimeInt <= 500 )
			return true;
	}
	return false;
}
//---------------------------------------------------------------------------
//
// Convert the gevin time to Todays menutes.(index)
// In: UDateTime
// return: int: Minutes after today's opentime.
//
//---------------------------------------------------------------------------
int	ExchangeInfo::TimeToIndex( const UFC::UDateTime& DateTime, bool IsToday )
{
	int Index = 0;
	UFC::UDateTime RefTime = DateTime;

	RefTime += 0;

	///< Given time < First Session Start time
	///< Should be the next day.
	if( RefTime < FStartList.GetItem( 0 ) )
		RefTime += 86400;

	///< Given time > Last Session End time
	///< Should out of bound, or it,s TAIFEX offhour.
	if( RefTime > FEndList.GetItem( FEndList.ItemCount() - 1 ) )
	{
		if( FSupportOffhour == true && IsToday == true  && RefTime <= FOffhourCloseTime && RefTime >= FOffhourOpenTime )
		{
			int Offset = ( RefTime - FOffhourOpenTime )/60;

			Index = FMinutes;
			if( Offset > 0 )
				Index += Offset;
			return Index;
		}
		else
			return 0;
	}
	///< Given time > First Session start time
	if( RefTime >= FStartList[0] )
	{
		for( register int i = 0; i < FSessionCount; i++ )
		{
			if( RefTime < FEndList[i] ) ///< in this trading session
			{
				int Offset = ( RefTime - FStartList[i] )/60;
				if( Offset > 0 )
					Index += Offset;
				break;
			}
			else ///< In the later trading session.
				Index += ( FEndList[i] - FStartList[i] )/60;
		}
	}
	return Index;
}
//---------------------------------------------------------------------------
//
// Convert the gevin time to Todays menutes.(index)
// In: hours
// In: minutes
// return: int: Minutes after today's opentime.
//
//---------------------------------------------------------------------------
int ExchangeInfo::TimeToIndex( int hour, int minute, bool IsToday )
{
	UFC::UDateTime LastSessionEnd( FEndList.GetItem( FEndList.ItemCount() - 1 ) );
	UFC::UDateTime time(  hour,  minute, 0, LastSessionEnd.getDate(), LastSessionEnd.getMonth(), LastSessionEnd.getYear() );

	return TimeToIndex( time, IsToday );
}
//---------------------------------------------------------------------------
void ExchangeInfo::IndexToTimeEx( int i, UFC::AnsiString& TimeStr )
{
	i = i % FMinutesEx;

	if( i <= 0 )
		TimeStr.Printf( "%02d:%02d:%02d", FStartListEx[0].getHour(), FStartListEx[0].getMinute(), FStartListEx[0].getSecond() );
	else if( i >= FMinutesEx - 1 )
		TimeStr.Printf( "%02d:%02d:%02d", FEndListEx[FEndListEx.ItemCount() - 1].getHour(), FEndListEx[FEndListEx.ItemCount() - 1].getMinute(), FEndListEx[FEndListEx.ItemCount() - 1].getSecond() );
	else
	{
		int Cum = 0;
		int Session = 0;
		int Index = i;

		while( i >= Cum )
		{
			Cum += FSessionSecendsEx[Session];

			if( i < Cum )
				break;
			else
			{
				Index -= FSessionSecendsEx[Session];
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



