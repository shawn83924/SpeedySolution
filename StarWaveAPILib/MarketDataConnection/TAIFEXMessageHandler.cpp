#include "MarketDataConnection.h"
//------------------------------------------------------------------------------
void MarketDataConnection::OnTAIFEXMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MTree* Data )
{
	UFC::Int32 Type(0);
	UFC::Int32 mkt;
	int ReadSize,TSession = 0;
	char* Buffer;

	if( Data->get( "MsgType", Type ) &&
		Data->get( "MKT", mkt ) &&
		Data->get( "DATA", Buffer, ReadSize ) )
	{
		UFC::MemoryReadStream Stream( Buffer, ReadSize );

        if( FIsDebugPerformance )
        {
            Int64   TUS0, TUS1, TUS2, TUS3;
            if( Data->get( "TUS0", TUS0 ) && Data->get( "TUS1", TUS1 ) && Data->get( "TUS2", TUS2 ) && Data->get( "TUS3", TUS3 ) )
            {
                FTickCountUS[0] = TUS0;
                FTickCountUS[1] = TUS1;
                FTickCountUS[2] = TUS2;
                FTickCountUS[3] = TUS3;
                FTickCountUS[4] = (Int64)UFC::GetTickCountUS();

                FTickLagUS[0] = FTickCountUS[4] - FTickCountUS[0];
                FTickLagUS[1] = FTickCountUS[1] - FTickCountUS[0];
                FTickLagUS[2] = FTickCountUS[2] - FTickCountUS[1];
                FTickLagUS[3] = FTickCountUS[3] - FTickCountUS[2];
                FTickLagUS[4] = FTickCountUS[4] - FTickCountUS[3];

                for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
                {
                    if( FTickLagUS[i] < 0 )
                        FTickLagUS[i] += (Int64)86400*1000000;
                }       
                UFC::BufferedLog::DebugPrintf( UFC::dlPerformance, " [%s] OnTAIFEXMessage(), Type:[%d], TUS:[%lld][%lld][%lld][%lld][%lld], LagUS:[%lld][%lld][%lld][%lld][%lld]",
                    Symbol.c_str(), Type, FTickCountUS[0], FTickCountUS[1], FTickCountUS[2], FTickCountUS[3], FTickCountUS[4],
                    FTickLagUS[0], FTickLagUS[1], FTickLagUS[2], FTickLagUS[3], FTickLagUS[4] );
            }
            else
            {
                for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
                {
                    FTickCountUS[i] = 0;
                    FTickLagUS[i] = 0;
                }
            }       
		}
		switch( Type )
		{
			case mtOrderBookData:
				Data->get( "Sim", TSession );
				OnTAIFEXOrderBookMessage( Exchange, Symbol, (Market)mkt, &Stream, TSession );
				break;
			case mtTrade:
				Data->get( "Sim", TSession );
				OnTAIFEXMatchMessage( Exchange, Symbol, (Market)mkt, &Stream, TSession );
				break;
			case mtUnderlyingIndexInfo:
				OnTAIFEXUnderlyingIndex( Exchange, Symbol, (Market)mkt, &Stream );
				break;
			case mtDayHighLow:
				OnTAIFEXDayHighLow( Exchange, Symbol, (Market)mkt, &Stream );
				break;
			case mtSumOfOrderInfo:
				OnTAIFEXSumOfOrderInfo( Exchange, Symbol, (Market)mkt, &Stream );
				break;
			case mtOpeningInfo:
				OnTAIFEXOpeningInfo( Exchange, Symbol, (Market)mkt, &Stream );
				break;
			case mtClosingMarketData:
				OnTAIFEXCloseMarketData( Exchange, Symbol, (Market)mkt, &Stream );
				break;
			case mtClosingMarketDataWithSettlementPrice:
				OnTAIFEXCloseMarketDataWithSettlementPx( Exchange, Symbol, (Market)mkt, &Stream );
				break;
			case mtClosingMarketDataWithSettlementPriceAndOpenInterest:
				OnTAIFEXCloseMarketDataWithSettlementPxAndOpenInterest( Exchange, Symbol, (Market)mkt, &Stream );
				break;
			case mtSystemMsg:
				OnTAIFEXSystemMsg( Exchange, (Market)mkt, &Stream, Data );
				break;
			default: break;
		}
	}
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnTAIFEXOrderBookMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream, int TSession )
{
	OrderBookData Msg( Exchange, mkt, Symbol );
	int ReadSize;
	char* Buffer;
	UFC::Int8 DecimalLocator;

	UFC::NInt32 BuyPx1, BuyPx2, BuyPx3, BuyPx4, BuyPx5, SellPx1, SellPx2, SellPx3, SellPx4, SellPx5;
	UFC::NInt32 BuyQty1, BuyQty2, BuyQty3, BuyQty4, BuyQty5, SellQty1, SellQty2, SellQty3, SellQty4, SellQty5;

	FetchHeader( Stream, Msg );
	///< Body
	Stream->Read( (void*)&DecimalLocator, 1 );
	BuyPx1.LoadFromStream( Stream );
	BuyQty1.LoadFromStream( Stream );
	BuyPx2.LoadFromStream( Stream );
	BuyQty2.LoadFromStream( Stream );
	BuyPx3.LoadFromStream( Stream );
	BuyQty3.LoadFromStream( Stream );
	BuyPx4.LoadFromStream( Stream );
	BuyQty4.LoadFromStream( Stream );
	BuyPx5.LoadFromStream( Stream );
	BuyQty5.LoadFromStream( Stream );
	SellPx1.LoadFromStream( Stream );
	SellQty1.LoadFromStream( Stream );
	SellPx2.LoadFromStream( Stream );
	SellQty2.LoadFromStream( Stream );
	SellPx3.LoadFromStream( Stream );
	SellQty3.LoadFromStream( Stream );
	SellPx4.LoadFromStream( Stream );
	SellQty4.LoadFromStream( Stream );
	SellPx5.LoadFromStream( Stream );
	SellQty5.LoadFromStream( Stream );

	Msg.SetBuyDepth( 1, UFC::IntToDouble( BuyPx1.ToInt32(), DecimalLocator ), BuyQty1.ToInt32() );
	Msg.SetBuyDepth( 2, UFC::IntToDouble( BuyPx2.ToInt32(), DecimalLocator ), BuyQty2.ToInt32() );
	Msg.SetBuyDepth( 3, UFC::IntToDouble( BuyPx3.ToInt32(), DecimalLocator ), BuyQty3.ToInt32() );
	Msg.SetBuyDepth( 4, UFC::IntToDouble( BuyPx4.ToInt32(), DecimalLocator ), BuyQty4.ToInt32() );
	Msg.SetBuyDepth( 5, UFC::IntToDouble( BuyPx5.ToInt32(), DecimalLocator ), BuyQty5.ToInt32() );

	Msg.SetSellDepth( 1, UFC::IntToDouble( SellPx1.ToInt32(), DecimalLocator ), SellQty1.ToInt32() );
	Msg.SetSellDepth( 2, UFC::IntToDouble( SellPx2.ToInt32(), DecimalLocator ), SellQty2.ToInt32() );
	Msg.SetSellDepth( 3, UFC::IntToDouble( SellPx3.ToInt32(), DecimalLocator ), SellQty3.ToInt32() );
	Msg.SetSellDepth( 4, UFC::IntToDouble( SellPx4.ToInt32(), DecimalLocator ), SellQty4.ToInt32() );
	Msg.SetSellDepth( 5, UFC::IntToDouble( SellPx5.ToInt32(), DecimalLocator ), SellQty5.ToInt32() );

	if( TSession == tsPreOpen )
		Msg.SetTestMatch( true );
	else
		Msg.SetTestMatch( false );
	Msg.SetTradingSession( (TradingSession)TSession );
	///Derived px and qty
	UFC::NInt16 Flag;

	Flag.LoadFromStream( Stream );
	if( Flag.ToInt16() > 0 )
	{
		Msg.SetDerivedFlag( true );
		UFC::NInt32 DerivedBuyPx, DerivedBuyQty, DerivedSellPx, DerivedSellQty;

		DerivedBuyPx.LoadFromStream( Stream );
		DerivedBuyQty.LoadFromStream( Stream );
		DerivedSellPx.LoadFromStream( Stream );
		DerivedSellQty.LoadFromStream( Stream );
		Msg.SetDerivedBuyPrice( UFC::IntToDouble( DerivedBuyPx.ToInt32(), DecimalLocator ) );
		Msg.SetDerivedBuyQty( DerivedBuyQty.ToInt32() );
		Msg.SetDerivedSellPrice( UFC::IntToDouble( DerivedSellPx.ToInt32(), DecimalLocator ) );
		Msg.SetDerivedSellQty( DerivedSellQty.ToInt32() );
	}
	else
		Msg.SetDerivedFlag( false );

    if( FIsDebugPerformance )
    {
        for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
        {
            Msg.SetTickCountUS( i, FTickCountUS[i] );
            Msg.SetTickLagUS( i, FTickLagUS[i] );
		}
    }
	FListener->OnMarketDataMessage( Exchange, Symbol, &Msg );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnTAIFEXMatchMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream, int TSession )
{
	UFC::Int8 Version, DecimalLocator, StatusCode;
	UFC::NInt16 Count;
	UFC::NInt32 Time, Seq, MatchTime, TotalTrade, BuyTotalCount, SellTotalCount;
	UFC::AnsiString TimeStamp, MatchTimeStamp;

	///<Header
	Time.LoadFromStream( Stream );
	Seq.LoadFromStream( Stream );
	Stream->Read((void*)&Version, 1 );
	DigitToTimeStamp( Time.ToInt32(), TimeStamp );

	///< Body
	Stream->Read( (void*)&StatusCode, 1 );
	Stream->Read( (void*)&DecimalLocator, 1 );
	MatchTime.LoadFromStream( Stream );
	DigitToTimeStamp( MatchTime.ToInt32(), MatchTimeStamp );

	Count.LoadFromStream( Stream );
	int count = Count.ToInt16();
	for( register int i = 0; i < count; i++ )
	{
		UFC::NInt32 Px;
		UFC::NInt16 Qty;

		Px.LoadFromStream( Stream );
		Qty.LoadFromStream( Stream );
		MatchInfo Match( Exchange, mkt, Symbol );
		Match.SetMessageTime( TimeStamp );
		Match.SetSequence( Seq.ToInt32() );
		Match.SetVersion( Version );
		Match.SetMatchTime( MatchTimeStamp );
		Match.SetMatchPrice( UFC::IntToDouble( Px.ToInt32(), DecimalLocator ) );
		Match.SetMatchQty( Qty.ToInt16() );
		if( TSession == tsPreOpen )
			Match.SetTestMatch( true );
		else
			Match.SetTestMatch( false );
		Match.SetTradingSession( (TradingSession)TSession );

		if( FIsDebugPerformance )
		{
			for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
			{
				Match.SetTickCountUS( i, FTickCountUS[i] );
				Match.SetTickLagUS( i, FTickLagUS[i] );
			}
		}
		FListener->OnMarketDataMessage( Exchange, Symbol, &Match );
	}
	///< Total Match
	TotalMatch TotalTradeInfo( Exchange, mkt, Symbol );
	TotalTradeInfo.SetMessageTime( TimeStamp );
	TotalTradeInfo.SetSequence( Seq.ToInt32() );
	TotalTradeInfo.SetVersion( Version );
	TotalTrade.LoadFromStream( Stream );
	BuyTotalCount.LoadFromStream( Stream );
	SellTotalCount.LoadFromStream( Stream );
	TotalTradeInfo.SetTotalMatchQty( TotalTrade.ToInt32() );
	TotalTradeInfo.SetBuyMatchCount( BuyTotalCount.ToInt32() );
	TotalTradeInfo.SetSellMatchCount( SellTotalCount.ToInt32() );
	if( TSession == tsPreOpen )
		TotalTradeInfo.SetTestMatch( true );
	else
		TotalTradeInfo.SetTestMatch( false );
	TotalTradeInfo.SetTradingSession( (TradingSession)TSession );
    if( FIsDebugPerformance )
    {
        for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
        {
            TotalTradeInfo.SetTickCountUS( i, FTickCountUS[i] );
            TotalTradeInfo.SetTickLagUS( i, FTickLagUS[i] );
        }
    }
	FListener->OnMarketDataMessage( Exchange, Symbol, &TotalTradeInfo );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnTAIFEXUnderlyingIndex( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::NInt32 IndexValue, ShowTime;
	UFC::AnsiString TimeStamp;
	UnderlyingIndexInfo Index( Exchange, mkt, Symbol );

	FetchHeader( Stream, Index );

	UFC::NInt64 MatchValue, BuyValue, SellValue, FixValue, Time;
	UFC::AnsiString Date, TimeString;

	///<Body
	MatchValue.LoadFromStream( Stream );
	BuyValue.LoadFromStream( Stream );
	SellValue.LoadFromStream( Stream );
	FixValue.LoadFromStream( Stream );
	Time.LoadFromStream( Stream );
	Date.LoadFromStream( Stream );

	DigitToTimeStamp( Time.ToInt64() / 10000, TimeStamp );
	Index.SetShowTime( TimeStamp );
	Index.SetIndexValue( UFC::Int64ToDouble( MatchValue.ToInt64(), 5 ) );
	Index.SetBuyValue( UFC::Int64ToDouble( BuyValue.ToInt64(), 5 ) );
	Index.SetSellValue( UFC::Int64ToDouble( SellValue.ToInt64(), 5 ) );
	Index.SetFixValue( UFC::Int64ToDouble( FixValue.ToInt64(), 5 ) );
	Index.SetExchangeDate( Date );

	if( FIsDebugPerformance )
	{
		for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
		{
			Index.SetTickCountUS( i, FTickCountUS[i] );
			Index.SetTickLagUS( i, FTickLagUS[i] );
		}
	}
	FListener->OnMarketDataMessage( Exchange, Symbol, &Index );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnTAIFEXDayHighLow( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::Int8 DecimalLocator;
	UFC::NInt32 DayHighPx, DayLowPx, ShowTime;
	UFC::AnsiString TimeStamp;
	DayHighLowPrice DayHighLow( Exchange, mkt, Symbol );

	///< Header
	FetchHeader( Stream, DayHighLow );
	Stream->Read( ( void* )&DecimalLocator, 1 );
	DayHighPx.LoadFromStream( Stream );
	DayLowPx.LoadFromStream( Stream );
	ShowTime.LoadFromStream( Stream );
	DigitToTimeStamp( ShowTime.ToInt32(), TimeStamp );
	DayHighLow.SetDayHighPrice( UFC::IntToDouble( DayHighPx.ToInt32(),DecimalLocator  ) );
	DayHighLow.SetDayLowPrice( UFC::IntToDouble( DayLowPx.ToInt32(), DecimalLocator ) );
	DayHighLow.SetShowTime( TimeStamp );

    if( FIsDebugPerformance )
    {
        for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
        {
            DayHighLow.SetTickCountUS( i, FTickCountUS[i] );
            DayHighLow.SetTickLagUS( i, FTickLagUS[i] );
        }
    }
	FListener->OnMarketDataMessage( Exchange, Symbol, &DayHighLow );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnTAIFEXSumOfOrderInfo( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::NInt32 BuyOrderCount, BuyOrderQty, SellOrderCount, SellOrderQty;
	SumOfOrderInfo SumInfo( Exchange, mkt, Symbol );

	FetchHeader( Stream, SumInfo );
	BuyOrderCount.LoadFromStream( Stream );
	BuyOrderQty.LoadFromStream( Stream );
	SellOrderCount.LoadFromStream( Stream );
	SellOrderQty.LoadFromStream( Stream );
	SumInfo.SetBuyOrderCount( BuyOrderCount.ToInt32() );
	SumInfo.SetBuyQty( BuyOrderQty.ToInt32() );
	SumInfo.SetSellOrderCount( SellOrderCount.ToInt32() );
	SumInfo.SetSellQty( SellOrderQty.ToInt32() );

    if( FIsDebugPerformance )
    {
        for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
        {
            SumInfo.SetTickCountUS( i, FTickCountUS[i] );
            SumInfo.SetTickLagUS( i, FTickLagUS[i] );
        }
    }
	FListener->OnMarketDataMessage( Exchange, Symbol, &SumInfo );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnTAIFEXOpeningInfo( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::Int8 DecimalLocator;
	UFC::NInt32 OpeningPx, OpeningQty, MatchTime;
	UFC::AnsiString TimeStamp;
	OpeningInfo Info( Exchange, mkt, Symbol );

	FetchHeader( Stream, Info );
	Stream->Read( ( void* )&DecimalLocator, 1 );
	OpeningPx.LoadFromStream( Stream );
	OpeningQty.LoadFromStream( Stream );
	MatchTime.LoadFromStream( Stream );
	DigitToTimeStamp( MatchTime.ToInt32(), TimeStamp );
	Info.SetOpeningPrice( UFC::IntToDouble( OpeningPx.ToInt32(), DecimalLocator ) );
	Info.SetOpeningQty( OpeningQty.ToInt32() );
	Info.SetMessageTime( TimeStamp );

    if( FIsDebugPerformance )
    {
        for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
        {
            Info.SetTickCountUS( i, FTickCountUS[i] );
            Info.SetTickLagUS( i, FTickLagUS[i] );
        }
    }
	FListener->OnMarketDataMessage( Exchange, Symbol, &Info );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnTAIFEXCloseMarketData( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::Int8         DecimalLocator;
	ClosingMarketData CloseData( Exchange, mkt, Symbol );

	FetchHeader( Stream, CloseData );
	Stream->Read( ( void* )&DecimalLocator, 1 );
	FillCloseMarketData( Stream, CloseData, DecimalLocator );

    if( FIsDebugPerformance )
    {
        for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
        {
            CloseData.SetTickCountUS( i, FTickCountUS[i] );
            CloseData.SetTickLagUS( i, FTickLagUS[i] );
        }
    }
	FListener->OnMarketDataMessage( Exchange, Symbol, &CloseData );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnTAIFEXCloseMarketDataWithSettlementPx( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::Int8   DecimalLocator;
	UFC::NInt32 SettlementPx;
	ClosingMarketDataWithSettlementPrice CloseData( Exchange, mkt, Symbol );

	FetchHeader( Stream, CloseData );
	Stream->Read( ( void* )&DecimalLocator, 1 );
	FillCloseMarketData( Stream, CloseData, DecimalLocator );
	SettlementPx.LoadFromStream( Stream );
	CloseData.SetSettlementPrice( UFC::IntToDouble( SettlementPx.ToInt32(), DecimalLocator ) );

    if( FIsDebugPerformance )
    {
        for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
        {
            CloseData.SetTickCountUS( i, FTickCountUS[i] );
            CloseData.SetTickLagUS( i, FTickLagUS[i] );
        }
    }
	FListener->OnMarketDataMessage( Exchange, Symbol, &CloseData );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnTAIFEXCloseMarketDataWithSettlementPxAndOpenInterest( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::Int8   DecimalLocator;
	UFC::NInt32 SettlementPx, OpenInterest ;
	ClosingMarketDataWithSettlementPriceAndOpenInterest CloseData( Exchange, mkt, Symbol );

	FetchHeader( Stream, CloseData );
	Stream->Read( ( void* )&DecimalLocator, 1 );
	FillCloseMarketData( Stream, CloseData, DecimalLocator );
	SettlementPx.LoadFromStream( Stream );
	CloseData.SetSettlementPrice( UFC::IntToDouble( SettlementPx.ToInt32(), DecimalLocator ) );
	OpenInterest.LoadFromStream( Stream );
	CloseData.SetOpenInterest( OpenInterest.ToInt32() );

    if( FIsDebugPerformance )
    {
        for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
        {
            CloseData.SetTickCountUS( i, FTickCountUS[i] );
            CloseData.SetTickLagUS( i, FTickLagUS[i] );
        }
    }
	FListener->OnMarketDataMessage( Exchange, Symbol, &CloseData );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnTAIFEXSystemMsg( const UFC::AnsiString& Exchange, Market mkt, UFC::PStream* Stream, MTree* Data )
{
	int Code;

	if( Data->get( "FnCode", Code ) && FListener != NULL )
	{
		SystemMessage SysMsg( Exchange, "SYS", mkt  );
		UFC::AnsiString Data;

		SysMsg.SetCode( Code );
		FetchHeader( Stream, SysMsg );
		Data.LoadFromStream( Stream );
		SysMsg.SetData( Data );

		if( FIsDebugPerformance )
		{
			for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
			{
				SysMsg.SetTickCountUS( i, FTickCountUS[i] );
				SysMsg.SetTickLagUS( i, FTickLagUS[i] );
			}
		}
        FListener->OnSystemMessage( Exchange, &SysMsg );
    }
}
//------------------------------------------------------------------------------



