#include "MarketDataConnection.h"
//------------------------------------------------------------------------------
void MarketDataConnection::OnHKExMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MTree* Data )
{
	UFC::Int32 Type(0);
	UFC::Int32 mkt;
	int ReadSize;
	char* Buffer;

	if ( Data->get( "MsgType", Type )
            && Data->get( "MKT", mkt )
            && Data->get( "DATA", Buffer, ReadSize ) )
	{
		UFC::MemoryReadStream Stream( Buffer, ReadSize );

		switch( Type )
		{
			case mtTrade:               OnHKExTrade( Exchange, Symbol, (Market)mkt, &Stream ); break;
			case mtTotalTrade:          OnHKExTotalTrade( Exchange, Symbol, (Market)mkt, &Stream );break;
			case mtDayHighLow:          OnHKExDayHighLow( Exchange, Symbol, (Market)mkt, &Stream ); break;
			case mtOpeningInfo:         OnHKExOpen( Exchange, Symbol, (Market)mkt, &Stream ); break;
			case mtUnderlyingIndexInfo: OnHKExIndex( Exchange, Symbol, (Market)mkt, &Stream ); break;
			case mtClosingMarketData:   OnHKExClose( Exchange, Symbol, (Market)mkt, &Stream ); break;
			case mtOrderBookData:       OnHKExOrderBook( Exchange, Symbol, (Market)mkt, &Stream );break;
			case mtRecover: break;
			case mtBasicInfo: break;
			case mtSumOfOrderInfo: break;
			case mtNews: break;
			case mtClosingMarketDataWithSettlementPrice: break;
			case mtClosingMarketDataWithSettlementPriceAndOpenInterest: break;
			case mtCombinenationProductClosingMarketData: break;
			case mtQuoteRequest: break;
			case mtFullSnapShot: break;
			default: break;
		}
	}
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnHKExTrade( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{

	UFC::NDouble NPx;
	UFC::NInt32 NQty;
	UFC::UDateTime MatchTime;
	UFC::AnsiString MatchTimeStr;

	MatchTime.LoadFromStream( Stream );
	NPx.LoadFromStream( Stream );
	NQty.LoadFromStream( Stream );

	ToTimeStamp( MatchTime, MatchTimeStr );
	MatchInfo Match( Exchange, mkt, Symbol );

	Match.SetMessageTime( MatchTimeStr );
	Match.SetMatchTime( MatchTimeStr );
	Match.SetMatchPrice( NPx.ToDouble() );
	Match.SetMatchQty( NQty.ToInt32() );	
	FListener->OnMarketDataMessage( Exchange, Symbol, &Match );
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnTWSETrade() LastPx[%0.2f] Qty[%d] MatchTime[%02d:%02d:%02d]",
			Symbol.c_str(), NPx.ToDouble(), NQty.ToInt32(), MatchTime.getHour(), MatchTime.getMinute(), MatchTime.getSecond() );

}
//------------------------------------------------------------------------------
void MarketDataConnection::OnHKExTotalTrade( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::NInt32 NQty;
	UFC::UDateTime Time;
	UFC::AnsiString MatchTimeStr;

	TotalMatch Total( Exchange, mkt, Symbol );
	Time.LoadFromStream( Stream );
	NQty.LoadFromStream( Stream );

	ToTimeStamp( Time, MatchTimeStr );
	Total.SetTotalMatchQty( NQty.ToInt32() );
	Total.SetMessageTime( MatchTimeStr );	
	FListener->OnMarketDataMessage( Exchange, Symbol, &Total );
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnTWSETotalTrade() TotalVol[%d] MessageTime[%s]",
		Symbol.c_str(), NQty.ToInt32(), MatchTimeStr.c_str() );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnHKExOrderBook( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	OrderBookData Msg( Exchange, mkt, Symbol );
	UFC::UDateTime MatchTime;
	UFC::AnsiString MatchTimeStr;

	MatchTime.LoadFromStream( Stream );
	Msg.LoadDepthFromStream( Stream );

	ToTimeStamp( MatchTime, MatchTimeStr );
	Msg.SetMessageTime( MatchTimeStr );	
	FListener->OnMarketDataMessage( Exchange, Symbol, &Msg );
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnTWSEOrderBook() MessageTime[%s]", Symbol.c_str(), MatchTimeStr.c_str() );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnHKExDayHighLow( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	DayHighLowPrice DayHighLow( Exchange, mkt, Symbol );
	UFC::NDouble NDayHighPx, NDayLowPx;

	UFC::AnsiString MessageTime;
	UFC::UDateTime MsgTime;

	MsgTime.LoadFromStream( Stream );
	ToTimeStamp( MsgTime, MessageTime );

	NDayHighPx.LoadFromStream( Stream );
	NDayLowPx.LoadFromStream( Stream );

	DayHighLow.SetDayHighPrice( NDayHighPx.ToDouble() );
	DayHighLow.SetDayLowPrice( NDayLowPx.ToDouble() );
	DayHighLow.SetMessageTime( MessageTime );

	FListener->OnMarketDataMessage( Exchange, Symbol, &DayHighLow );
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnTWSEDayHighLow() HighPx[%0.2f] LowPx[%0.2f] MessageTime[%s]",
		Symbol.c_str(), NDayHighPx.ToDouble(), NDayLowPx.ToDouble(), MessageTime.c_str() );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnHKExOpen( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::NDouble NOpenPx;

	UFC::AnsiString MessageTime;
	UFC::UDateTime MsgTime;

	OpeningInfo Info( Exchange, mkt, Symbol );

	MsgTime.LoadFromStream( Stream );
	NOpenPx.LoadFromStream( Stream );
	ToTimeStamp( MsgTime, MessageTime );

	Info.SetOpeningPrice( NOpenPx.ToDouble() );
	Info.SetMessageTime( MessageTime );

	FListener->OnMarketDataMessage( Exchange, Symbol, &Info );
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnTWSEOpen() OpenPx[%0.2f] MessageTime[%s]",
		Symbol.c_str(), NOpenPx.ToDouble(), MessageTime.c_str() );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnHKExIndex( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::NDouble Px;
	UFC::UDateTime Time;
	UFC::NInt32 TotalQty;
	UFC::NInt32 TotalCount;
	UFC::NInt64 TotalAmount;
	UFC::AnsiString MessageTime;

	Time.LoadFromStream( Stream );
	Px.LoadFromStream( Stream );
	TotalQty.LoadFromStream( Stream );
	TotalCount.LoadFromStream( Stream );
	TotalAmount.LoadFromStream( Stream );

	ToTimeStamp( Time, MessageTime );

	UnderlyingIndexInfo Info( Exchange, mkt, Symbol );

	Info.SetIndexValue( Px.ToDouble() );
	Info.SetTotalQty( TotalQty.ToInt32() );
	Info.SetTotalCount( TotalCount.ToInt32());
	Info.SetTotalAmount( TotalAmount.ToInt64( ));
	Info.SetMessageTime( MessageTime );
	Info.SetShowTime( MessageTime );

	FListener->OnMarketDataMessage( Exchange, Symbol, &Info );
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnTWSEIndex() Index[%0.2f] Qty[%d] count[%d] MessageTime[%s]",
		Symbol.c_str(), Px.ToDouble(), TotalQty.ToInt32(), TotalCount.ToInt32(), MessageTime.c_str() );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnHKExClose( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::NDouble OpenPx, HighPx, LowPx, LastPx;
	UFC::NInt32 TotalQty;
	UFC::UDateTime Time;
	UFC::AnsiString TimeStr, symbol;

	ClosingMarketData CloseData( Exchange, mkt, Symbol );

	symbol.LoadFromStream( Stream );
	OpenPx.LoadFromStream( Stream );
	HighPx.LoadFromStream( Stream );
	LowPx.LoadFromStream( Stream );
	LastPx.LoadFromStream( Stream );
	TotalQty.LoadFromStream( Stream );
	Time.LoadFromStream( Stream );
	ToTimeStamp( Time, TimeStr );

	CloseData.SetOpenPx( OpenPx.ToDouble() );
	CloseData.SetDayHighPx( HighPx.ToDouble() );
	CloseData.SetDayLowPx( LowPx.ToDouble() );
	CloseData.SetClosePx( LastPx.ToDouble() );
	CloseData.SetMessageTime( TimeStr );

	FListener->OnMarketDataMessage( Exchange, Symbol, &CloseData );
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnTWSEClose() Open[%0.2f] High[%0.2f] Low[%0.2f] Close[%0.2f] MessageTime[%s]",
		Symbol.c_str(), OpenPx.ToDouble(), HighPx.ToDouble(), LowPx.ToDouble(), LastPx.ToDouble(), TimeStr.c_str() );
}
//------------------------------------------------------------------------------




