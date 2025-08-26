#include "MarketDataConnection.h"
//------------------------------------------------------------------------------
void MarketDataConnection::OnChinaMarketData( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MTree* Data )
{
	UFC::Int32 Type(0);
	UFC::Int32 mkt;
	int ReadSize;
	char* Buffer;

	if( Data->get( "MsgType", Type )
		&& Data->get( "MKT", mkt )
		&& Data->get( "DATA", Buffer, ReadSize ) )
	{
		UFC::MemoryReadStream Stream( Buffer, ReadSize );

		switch( Type )
		{
			/*// 20150319 by Tim Lin // for "SSE" ²{³f 510050.SS
			case mtDayHighLow:  OnCMEDayHighLow( Exchange, Symbol, (Market)mkt, &Stream ); break; // add for SSE 510050.SS
			case mtOpeningInfo: OnCMEOpen( Exchange, Symbol, (Market)mkt, &Stream ); break; // add for SSE 510050.SS
			case mtUnderlyingIndexInfo: OnCMEIndex( Exchange, Symbol, (Market)mkt, &Stream ); break; // add for SSE 510050.SS
            */
			case mtFullSnapShot:
				OnChinaFullSnapShot( Exchange, Symbol, (Market)mkt, &Stream );
				break;

			default: break;
		}
	}
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnChinaFullSnapShot( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::NDouble BidPx1, BidPx2, BidPx3, BidPx4, BidPx5;
	UFC::NInt32  BidQty1,BidQty2, BidQty3, BidQty4, BidQty5;
	UFC::NDouble AskPx1, AskPx2, AskPx3, AskPx4, AskPx5;
	UFC::NInt32  AskQty1, AskQty2, AskQty3, AskQty4, AskQty5, LastQty, TotalVolumn;
	UFC::NInt32  Time, MS;
	UFC::AnsiString TimeStamp;
	UFC::NDouble    LastPx, HighPx, LowPx, OpenPx, PreClosePx;
	UFC::NDouble    PreSettlementPx, ClosePx, SettlementPx;
	UFC::NInt32     OpenInterest, PreOpenInterest;

	BidPx1.LoadFromStream( Stream );
	BidPx2.LoadFromStream( Stream );
	BidPx3.LoadFromStream( Stream );
	BidPx4.LoadFromStream( Stream );
	BidPx5.LoadFromStream( Stream );

	BidQty1.LoadFromStream( Stream );
	BidQty2.LoadFromStream( Stream );
	BidQty3.LoadFromStream( Stream );
	BidQty4.LoadFromStream( Stream );
	BidQty5.LoadFromStream( Stream );

	AskPx1.LoadFromStream( Stream );
	AskPx2.LoadFromStream( Stream );
	AskPx3.LoadFromStream( Stream );
	AskPx4.LoadFromStream( Stream );
	AskPx5.LoadFromStream( Stream );

	AskQty1.LoadFromStream( Stream );
	AskQty2.LoadFromStream( Stream );
	AskQty3.LoadFromStream( Stream );
	AskQty4.LoadFromStream( Stream );
	AskQty5.LoadFromStream( Stream );

	LastPx.LoadFromStream( Stream );
	LastQty.LoadFromStream( Stream );
	TotalVolumn.LoadFromStream( Stream );
	HighPx.LoadFromStream( Stream );
	LowPx.LoadFromStream( Stream );
	OpenPx.LoadFromStream( Stream );
	PreClosePx.LoadFromStream( Stream );
	PreOpenInterest.LoadFromStream( Stream );
	PreSettlementPx.LoadFromStream( Stream );
	ClosePx.LoadFromStream( Stream );
	OpenInterest.LoadFromStream( Stream );
	SettlementPx.LoadFromStream( Stream );
	Time.LoadFromStream( Stream );
	MS.LoadFromStream( Stream );

	IntToTimeStamp( Time.ToInt32(), MS.ToInt32(), TimeStamp );

	OrderBookData Depth5( Exchange, mkt, Symbol );
	MatchInfo Match( Exchange, mkt, Symbol );
	TotalMatch TotalTradeInfo( Exchange, mkt, Symbol );
	DayHighLowPrice DayHighLow( Exchange, mkt, Symbol );
	OpeningInfo Info( Exchange, mkt, Symbol );
	ClosingMarketDataWithSettlementPriceAndOpenInterest CloseData( Exchange, mkt, Symbol );

	Depth5.SetMessageTime( TimeStamp );
	Depth5.SetBuyDepth( 1, BidPx1, BidQty1.ToInt32() );
	Depth5.SetBuyDepth( 2, BidPx2, BidQty2.ToInt32() );
	Depth5.SetBuyDepth( 3, BidPx3, BidQty3.ToInt32() );
	Depth5.SetBuyDepth( 4, BidPx4, BidQty4.ToInt32() );
	Depth5.SetBuyDepth( 5, BidPx5, BidQty5.ToInt32() );

	Depth5.SetSellDepth( 1, AskPx1, AskQty1.ToInt32() );
	Depth5.SetSellDepth( 2, AskPx2, AskQty2.ToInt32() );
	Depth5.SetSellDepth( 3, AskPx3, AskQty3.ToInt32() );
	Depth5.SetSellDepth( 4, AskPx4, AskQty4.ToInt32() );
	Depth5.SetSellDepth( 5, AskPx5, AskQty5.ToInt32() );
	Depth5.SetDerivedFlag( false );

	if( LastQty.ToInt32() > 0 )
	{
		Match.SetMessageTime( TimeStamp );
		Match.SetMatchTime( TimeStamp );
		Match.SetMatchPrice( LastPx.ToDouble() );
		Match.SetMatchQty( LastQty.ToInt32() );
        Match.SetRefPx( PreSettlementPx.ToDouble() );
        
		TotalTradeInfo.SetMessageTime( TimeStamp );
		TotalTradeInfo.SetTotalMatchQty( TotalVolumn.ToInt32() );

		FListener->OnMarketDataMessage( Exchange, Symbol, &Match );
		FListener->OnMarketDataMessage( Exchange, Symbol, &TotalTradeInfo );
	}

	DayHighLow.SetDayHighPrice( HighPx.ToDouble() );
	DayHighLow.SetDayLowPrice( LowPx.ToDouble() );
	DayHighLow.SetShowTime( TimeStamp );

	Info.SetOpeningPrice( OpenPx.ToDouble() );
	Info.SetMessageTime( TimeStamp );

	CloseData.SetDayHighPx( HighPx.ToDouble() );
	CloseData.SetDayLowPx( LowPx.ToDouble() );
	CloseData.SetBuyPx( BidPx1.ToDouble() );
	CloseData.SetSellPx( AskPx1.ToDouble() );
	CloseData.SetOpenPx( OpenPx.ToDouble() );
	CloseData.SetClosePx( ClosePx.ToDouble() );
	CloseData.SetMessageTime( TimeStamp );
	CloseData.SetTotalTradeQty( TotalVolumn.ToInt32() );
	CloseData.SetOpenInterest( OpenInterest.ToInt32() );
	CloseData.SetSettlementPrice( PreSettlementPx.ToDouble() );

	FListener->OnMarketDataMessage( Exchange, Symbol, &Depth5 );
	FListener->OnMarketDataMessage( Exchange, Symbol, &DayHighLow );
	FListener->OnMarketDataMessage( Exchange, Symbol, &Info );
	FListener->OnMarketDataMessage( Exchange, Symbol, &CloseData );
}
//------------------------------------------------------------------------------





