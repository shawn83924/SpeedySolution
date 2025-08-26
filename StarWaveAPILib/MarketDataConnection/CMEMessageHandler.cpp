#include "MarketDataConnection.h"
//------------------------------------------------------------------------------
void MarketDataConnection::OnCMEMarketData( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MTree* Data )
{
	UFC::Int32 Type( 0 );
	UFC::Int32 mkt;
	int ReadSize;
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
                UFC::BufferedLog::DebugPrintf( UFC::dlPerformance, " [%s] OnCMEMarketData(), Type:[%d], TUS:[%lld][%lld][%lld][%lld][%lld], LagUS:[%lld][%lld][%lld][%lld][%lld]",
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
		    case mtRecover: break;
		    case mtBasicInfo: break;
		    case mtTrade:       OnCMETrade( Exchange, Symbol, (Market)mkt, &Stream ); break;
		    case mtTotalTrade:  OnCMETotalVol( Exchange, Symbol, (Market)mkt, &Stream );break;
		    case mtDayHighLow:  OnCMEDayHighLow( Exchange, Symbol, (Market)mkt, &Stream ); break;
		    case mtOpeningInfo: OnCMEOpen( Exchange, Symbol, (Market)mkt, &Stream ); break;
		    case mtSumOfOrderInfo: break;
		    case mtNews: break;
		    case mtUnderlyingIndexInfo: OnCMEIndex( Exchange, Symbol, (Market)mkt, &Stream ); break;
		    case mtClosingMarketData: break;
		    case mtClosingMarketDataWithSettlementPrice: OnCMESettlePx( Exchange, Symbol, (Market)mkt, &Stream ); break;
		    case mtClosingMarketDataWithSettlementPriceAndOpenInterest: break;
		    case mtCombinenationProductClosingMarketData: break;
		    case mtOrderBookData: OnCMEOrderBook( Exchange, Symbol, (Market)mkt, &Stream ); break;
		    case mtQuoteRequest: break;
		    case mtFullSnapShot: break;
		    default: break;
	    }
    }
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnCMETrade( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::AnsiString MessageTime;
	UFC::UDateTime MsgTime;

	MsgTime.setCurrent();
	ToTimeStamp( MsgTime, MessageTime );
	if( Symbol.AnsiPos( "=" ) > 0 && Exchange != "LME" ) ///< FX Spot
	{
		UFC::NDouble NPx, NBidPx, NAskPx, NRefPx;
		UFC::UDateTime MatchTime;
		UFC::AnsiString MatchTimeStr;

		NPx.LoadFromStream( Stream );
		NBidPx.LoadFromStream( Stream );
		NAskPx.LoadFromStream( Stream );
		MatchTime.LoadFromStream( Stream );
        NRefPx.LoadFromStream( Stream );
		ToTimeStamp( MatchTime, MatchTimeStr );

		MatchInfo Match( Exchange, mkt, Symbol );
		OrderBookData Msg( Exchange, mkt, Symbol );
		///<  Trigger FX Spot
		Match.SetMessageTime( MessageTime );
		Match.SetMatchTime( MatchTimeStr );
		Match.SetMatchPrice( NPx.ToDouble() );
		Match.SetRefPx( NRefPx.ToDouble() );
		Match.SetMatchQty( 1 );

        if( FIsDebugPerformance )
        {
            for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
            {
                Match.SetTickCountUS( i, FTickCountUS[i] );
                Match.SetTickLagUS( i, FTickLagUS[i] );
            }
        }
		FListener->OnMarketDataMessage( Exchange, Symbol, &Match );

		///< Trigger Depth for FX Spot
		Msg.SetBuyDepth( 1, NBidPx.ToDouble(), 1 );
		Msg.SetSellDepth( 1, NAskPx.ToDouble(), 1 );
		Msg.SetMessageTime( MessageTime );
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
	else ///< FX Futures
	{
		UFC::NDouble    NPx;
		UFC::NDouble    NewRefPx;
		UFC::NInt32     NQty,NEnd;
		UFC::UDateTime  MatchTime;
		UFC::AnsiString MatchTimeStr;

		NPx.LoadFromStream( Stream );
		NQty.LoadFromStream( Stream );
		MatchTime.LoadFromStream( Stream );
		NewRefPx.LoadFromStream( Stream );
		NEnd.LoadFromStream( Stream );

		ToTimeStamp( MatchTime, MatchTimeStr );
		MatchInfo Match( Exchange, mkt, Symbol );
		double    DRefPx = NewRefPx.ToDouble();

		///<  Trigger FX Spot
		Match.SetMessageTime( MessageTime );
		Match.SetMatchTime( MatchTimeStr );
		Match.SetMatchPrice( NPx.ToDouble() );
		Match.SetMatchQty( NQty.ToInt32() );
		Match.SetRefPx( DRefPx );     

		UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnCMETrade(), LastPx:[%f], Qty:[%d], MatchTime:[%02d:%02d:%02d]",
			Symbol.c_str(), NPx.ToDouble(), NQty.ToInt32(), MatchTime.getHour(), MatchTime.getMinute(), MatchTime.getSecond() );

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
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnCMETotalVol( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::AnsiString MessageTime;
	UFC::UDateTime MsgTime;
	UFC::NInt32 NTotalVol;
	TotalMatch TotalTradeInfo( Exchange, mkt, Symbol );

	MsgTime.setCurrent();
	ToTimeStamp( MsgTime, MessageTime );
	NTotalVol.LoadFromStream( Stream );

	TotalTradeInfo.SetMessageTime( MessageTime );
	TotalTradeInfo.SetTotalMatchQty( NTotalVol.ToInt32() );

	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnCMETotalVol(), TotalVol:[%d], MessageTime:[%s]",
		Symbol.c_str(), NTotalVol.ToInt32(), MessageTime.c_str() );

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
void MarketDataConnection::OnCMEOrderBook( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	OrderBookData Msg( Exchange, mkt, Symbol );
	Msg.LoadDepthFromStream( Stream );

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
void MarketDataConnection::OnCMEDayHighLow( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::NDouble NDayHighPx, NDayLowPx;

	UFC::AnsiString MessageTime;
	UFC::UDateTime MsgTime;
	MsgTime.setCurrent();
	ToTimeStamp( MsgTime, MessageTime );

	DayHighLowPrice DayHighLow( Exchange, mkt, Symbol );
	NDayHighPx.LoadFromStream( Stream );
	NDayLowPx.LoadFromStream( Stream );
	DayHighLow.SetDayHighPrice( NDayHighPx.ToDouble() );
	DayHighLow.SetDayLowPrice( NDayLowPx.ToDouble() );
	DayHighLow.SetMessageTime( MessageTime );

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
void MarketDataConnection::OnCMEOpen( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::NDouble NOpenPx;

	UFC::AnsiString MessageTime;
	UFC::UDateTime MsgTime;
	MsgTime.setCurrent();
	ToTimeStamp( MsgTime, MessageTime );

	OpeningInfo Info( Exchange, mkt, Symbol );
	NOpenPx.LoadFromStream( Stream );

	Info.SetOpeningPrice( NOpenPx.ToDouble() );
	Info.SetMessageTime( MessageTime );

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
void MarketDataConnection::OnCMEIndex( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::NDouble Px;
	UFC::NInt32 NQty;
	UFC::UDateTime Time;
	UFC::AnsiString MessageTime;

	Px.LoadFromStream( Stream );
	NQty.LoadFromStream( Stream );
	Time.LoadFromStream( Stream );
	ToTimeStamp( Time, MessageTime );

	UnderlyingIndexInfo Info( Exchange, mkt, Symbol );
	Info.SetIndexValue( Px.ToDouble() );
	Info.SetTotalQty( NQty.ToInt32() );
	Info.SetMessageTime( MessageTime );
	Info.SetShowTime( MessageTime );

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
void MarketDataConnection::OnCMESettlePx( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::NDouble NSettlePx( 0 );
	UFC::NDouble NClosePx( 0 );

	NSettlePx.LoadFromStream( Stream );
	NClosePx.LoadFromStream( Stream );

	ClosingMarketDataWithSettlementPrice CloseData( Exchange, mkt, Symbol );
	CloseData.SetClosePx( NClosePx.ToDouble() );
	CloseData.SetSettlementPrice( NSettlePx.ToDouble() );

    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnCMESettlePx(), SettlePx:[%f], ClosePx:[%f]",
        Symbol.c_str(), NSettlePx.ToDouble(), NClosePx.ToDouble() );

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
