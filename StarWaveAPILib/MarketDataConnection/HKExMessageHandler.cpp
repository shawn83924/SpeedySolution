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
			case mtOrderBookData:       OnHKExOrderBook( Exchange, Symbol, (Market)mkt, &Stream, tsNormal );break;
			case mtNews:                OnHKExNews( Exchange, Symbol, (Market)mkt, &Stream );break;
			///< HKEx only message type
			case mtOddOrderBookData:	OnHKExOrderBook( Exchange, Symbol, (Market)mkt, &Stream, tsOdd );break;
			case mtBuyBrokerQueueData:  OnHKExBrokerQueue( Exchange, Symbol, (Market)mkt, true, &Stream );break;
			case mtSellBrokerQueueData: OnHKExBrokerQueue( Exchange, Symbol, (Market)mkt, false, &Stream );break;
			case mtSuspensionIndicator: OnHKExSuspensionIndicator( Exchange, Symbol, (Market)mkt, &Stream ); break;
			///< Not imp
			case mtRecover:
			case mtBasicInfo:
			case mtQuoteRequest:
			case mtFullSnapShot:
			case mtSumOfOrderInfo:
			case mtClosingMarketDataWithSettlementPrice:
			case mtCombinenationProductClosingMarketData:
			case mtClosingMarketDataWithSettlementPriceAndOpenInterest: break;
			default: break;
		}
	}
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnHKExTrade( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{

	UFC::NDouble NPx;
	UFC::NDouble NRefPx;
	UFC::NInt32 NQty;
	UFC::NInt32 NSession;
	UFC::UDateTime MatchTime;
	UFC::AnsiString MatchTimeStr;

	MatchTime.LoadFromStream( Stream );
	NPx.LoadFromStream( Stream );
	NQty.LoadFromStream( Stream );
	NRefPx.LoadFromStream( Stream );
	NSession.LoadFromStream( Stream );

	ToTimeStamp( MatchTime, MatchTimeStr );
	MatchInfo Match( Exchange, mkt, Symbol );

	Match.SetMessageTime( MatchTimeStr );
	Match.SetMatchTime( MatchTimeStr );
	Match.SetMatchPrice( NPx.ToDouble() );
	Match.SetMatchQty( NQty.ToInt32() );
	Match.SetRefPx( NRefPx.ToDouble() );
	if( NSession.ToInt32() == 102 )
		Match.SetTradingSession( tsOdd );
	else
		Match.SetTradingSession( tsNormal );
	Match.SetHKExTrdType( NSession.ToInt32() );
	FListener->OnMarketDataMessage( Exchange, Symbol, &Match );
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnHKExTrade() LastPx[%0.2f] Qty[%d] MatchTime[%02d:%02d:%02d]",
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
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnHKExTotalTrade() TotalVol[%d] MessageTime[%s]",
		Symbol.c_str(), NQty.ToInt32(), MatchTimeStr.c_str() );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnHKExOrderBook( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream, int TSession )
{
	OrderBookData Msg( Exchange, mkt, Symbol );
	UFC::UDateTime MatchTime;
	UFC::AnsiString MatchTimeStr;

	Msg.HasNumOfOrders = true;
	MatchTime.LoadFromStream( Stream );
	Msg.LoadDepthFromStream( Stream );
	Stream->Read( Msg.BuyNumOfOrders, sizeof(UInt32)*MAX_DEPTH );
	Stream->Read( Msg.SellNumOfOrders, sizeof(UInt32)*MAX_DEPTH );
	ToTimeStamp( MatchTime, MatchTimeStr );
	Msg.SetMessageTime( MatchTimeStr );
	Msg.SetTradingSession( (TradingSession)TSession );
	FListener->OnMarketDataMessage( Exchange, Symbol, &Msg );
	//UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnHKExOrderBook() MessageTime[%s]", Symbol.c_str(), MatchTimeStr.c_str() );
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
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnHKExDayHighLow() HighPx[%0.2f] LowPx[%0.2f] MessageTime[%s]",
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
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnHKExIndex() Index[%0.2f] Qty[%d] count[%d] MessageTime[%s]",
		Symbol.c_str(), Px.ToDouble(), TotalQty.ToInt32(), TotalCount.ToInt32(), MessageTime.c_str() );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnHKExNews( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	UFC::AnsiString NewsMessage;

	NewsMessage.LoadFromStream( Stream );
	FListener->OnNews( "HKEx", NewsMessage );
	UFC::BufferedLog::Printf( " HKEx News [%s]", NewsMessage.c_str() );
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
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnHKExClose() Open[%0.2f] High[%0.2f] Low[%0.2f] Close[%0.2f] MessageTime[%s]",
		Symbol.c_str(), OpenPx.ToDouble(), HighPx.ToDouble(), LowPx.ToDouble(), LastPx.ToDouble(), TimeStr.c_str() );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnHKExBrokerQueue( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, bool IsBuy, UFC::PStream* Stream )
{
	HKBrokerQueue BQ(  Exchange, mkt, Symbol, IsBuy );

	BQ.LoadFromStream( Stream );
	FListener->OnMarketDataMessage( Exchange, Symbol, &BQ );
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnHKExBrokerQueue() Side[%s] Count[%d]", IsBuy? "Buy":"Sell", BQ.Count() );

}
//------------------------------------------------------------------------------
void MarketDataConnection::OnHKExSuspensionIndicator( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
	SystemMessage SysMsg( Exchange, Symbol, mkt  );
	UFC::UDateTime Time;
	UFC::NInt32    SuspensionIndicator;

	Time.LoadFromStream( Stream );
	SuspensionIndicator.LoadFromStream( Stream );

	SysMsg.SetCode( SuspensionIndicator.ToInt32() );
	SysMsg.SetData( "SuspensionIndicator" );

	FListener->OnSystemMessage( Exchange, &SysMsg );
}
//------------------------------------------------------------------------------




