#include "MarketDataConnection.h"
//------------------------------------------------------------------------------
void MarketDataConnection::OnCQGMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MTree* Data ){
    //UFC::BufferedLog::Printf("OnCQGMessage [%s] [%s]", Exchange.c_str(), Symbol.c_str());
	Int32 Type(0);
    UFC::Int32 mkt;
    int ReadSize;
    char* Buffer;
    
    if(Data->get( "MsgType", Type ) && Data->get( "MKT", mkt ) && Data->get( "DATA", Buffer, ReadSize ) )
    {
        UFC::MemoryReadStream Stream( Buffer, ReadSize );
        switch( Type )
        {
			case mtNews:				  OnCQGNews( Exchange, Symbol, (Market)mkt, &Stream ); break;
            case mtBasicInfo:             break;
            case mtTrade:                 OnCQGTrade( Exchange, Symbol, (Market)mkt, &Stream ); break;
            case mtTotalTrade:            OnCQGTotalTrade( Exchange, Symbol, (Market)mkt, &Stream ); break;
            case mtDayHighLow:            OnCQGDayHighLow( Exchange, Symbol, (Market)mkt, &Stream ); break;
            case mtOpeningInfo:           OnCQGOpen( Exchange, Symbol, (Market)mkt, &Stream ); break;
            case mtUnderlyingIndexInfo:   break;
            case mtOrderBookData:         OnCQGOrderBookData( Exchange, Symbol, (Market)mkt, &Stream ); break;
            case mtClosingMarketData:     break;
			case mtClosingMarketDataWithSettlementPrice: OnCQGSettlementPx( Exchange, Symbol, (Market)mkt, &Stream ); break;
			case mtClosingMarketDataWithSettlementPriceAndOpenInterest: OnCQGSettlementPx( Exchange, Symbol, (Market)mkt, &Stream ); break;
            default: break;
        }
    }
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnCQGNews( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
    UFC::AnsiString NewsMessage;
    NewsMessage.LoadFromStream( Stream );
    FListener->OnNews( Exchange, NewsMessage );
    UFC::BufferedLog::Printf( " %s News [%s]", Exchange.c_str(), NewsMessage.c_str() );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnCQGOpen( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
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
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnCQGOpen() OpenPx[%0.2f] MessageTime[%s]",
            Symbol.c_str(), NOpenPx.ToDouble(), MessageTime.c_str() );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnCQGOrderBookData( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream)
{
    OrderBookData Msg( Exchange, mkt, Symbol );
    UFC::UDateTime MatchTime;
    UFC::AnsiString MatchTimeStr;

    MatchTime.LoadFromStream( Stream );
    Msg.LoadDepthFromStream( Stream );

    ToTimeStamp( MatchTime, MatchTimeStr );
    Msg.SetMessageTime( MatchTimeStr );
    //Msg.SetTestMatch( false );
    Msg.SetTradingSession( tsNormal );
    FListener->OnMarketDataMessage( Exchange, Symbol, &Msg );
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnCQGOrderBook() MessageTime[%s]", Symbol.c_str(), MatchTimeStr.c_str() );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnCQGTrade( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream){
    UFC::NDouble NPx,RPx;
    UFC::NInt32 NQty;
    UFC::UDateTime MatchTime;
    UFC::AnsiString MatchTimeStr;

    MatchTime.LoadFromStream( Stream );
    NPx.LoadFromStream( Stream );
    NQty.LoadFromStream( Stream );
    RPx.LoadFromStream( Stream );

    ToTimeStamp( MatchTime, MatchTimeStr );
    MatchInfo Match( Exchange, mkt, Symbol );

    Match.SetMessageTime( MatchTimeStr );
    Match.SetMatchTime( MatchTimeStr );
    Match.SetMatchPrice( NPx.ToDouble() );
    Match.SetMatchQty( NQty.ToInt32() );
    //Match.SetTestMatch( false );
    Match.SetRefPx( RPx.ToDouble() );
    Match.SetTradingSession( tsNormal );
    FListener->OnMarketDataMessage( Exchange, Symbol, &Match );
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnCQGTrade() LastPx[%0.2f] Qty[%d] MatchTime[%02d:%02d:%02d]",
                    Symbol.c_str(), NPx.ToDouble(), NQty.ToInt32(), MatchTime.getHour(), MatchTime.getMinute(), MatchTime.getSecond() );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnCQGTotalTrade( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream){
    UFC::NInt32 NQty;
    UFC::UDateTime Time;
    UFC::AnsiString MatchTimeStr;

    TotalMatch Total( Exchange, mkt, Symbol );
    Time.LoadFromStream( Stream );
    NQty.LoadFromStream( Stream );

    ToTimeStamp( Time, MatchTimeStr );
    Total.SetTotalMatchQty( NQty.ToInt32() );
    Total.SetMessageTime( MatchTimeStr );
    //Total.SetTestMatch( false );
    Total.SetTradingSession( tsNormal );
    FListener->OnMarketDataMessage( Exchange, Symbol, &Total );
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnTWSETotalTrade() TotalVol[%d] MessageTime[%s]",
                Symbol.c_str(), NQty.ToInt32(), MatchTimeStr.c_str() );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnCQGDayHighLow( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
    UFC::NDouble NDayHighPx, NDayLowPx, DayOpenPx, DayClose;

    UFC::AnsiString MessageTime;
    UFC::UDateTime MsgTime;
    MsgTime.LoadFromStream( Stream );
    ToTimeStamp( MsgTime, MessageTime );

    DayHighLowPrice DayHighLow( Exchange, mkt, Symbol );
    DayOpenPx.LoadFromStream( Stream );
    NDayHighPx.LoadFromStream( Stream );
    NDayLowPx.LoadFromStream( Stream );
    DayClose.LoadFromStream( Stream );
    DayHighLow.SetDayHighPrice( NDayHighPx.ToDouble() );
    DayHighLow.SetDayLowPrice( NDayLowPx.ToDouble() );
    DayHighLow.SetMessageTime( MessageTime );
    FListener->OnMarketDataMessage( Exchange, Symbol, &DayHighLow );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnCQGSettlementPx( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
    UFC::NDouble SettlementPx, NDayHighPx, NDayLowPx, DayOpenPx, DayClose;

    UFC::AnsiString MessageTime;
    UFC::UDateTime MsgTime;
    MsgTime.LoadFromStream( Stream );
    ToTimeStamp( MsgTime, MessageTime );

    ClosingMarketDataWithSettlementPrice SettlementPriceMsg( Exchange, mkt, Symbol );
    DayOpenPx.LoadFromStream( Stream );
    NDayHighPx.LoadFromStream( Stream );
    NDayLowPx.LoadFromStream( Stream );
    DayClose.LoadFromStream( Stream );
    SettlementPx.LoadFromStream( Stream );

    SettlementPriceMsg.SetMessageTime( MessageTime );
    SettlementPriceMsg.SetOpenPx( DayOpenPx.ToDouble() );
    SettlementPriceMsg.SetDayHighPx( NDayHighPx.ToDouble() );
    SettlementPriceMsg.SetDayLowPx( NDayLowPx.ToDouble() );
    SettlementPriceMsg.SetClosePx( DayClose.ToDouble() );
    SettlementPriceMsg.SetSettlementPrice( SettlementPx.ToDouble() );

    FListener->OnMarketDataMessage( Exchange, Symbol, &SettlementPriceMsg );
}
//------------------------------------------------------------------------------
void MarketDataConnection::OnCQGSettlementPxWithOpenInterest( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream )
{
    UFC::NDouble SettlementPx, OpenInterest, NDayHighPx, NDayLowPx, DayOpenPx, DayClose;

    UFC::AnsiString MessageTime;
    UFC::UDateTime MsgTime;
    MsgTime.LoadFromStream( Stream );
    ToTimeStamp( MsgTime, MessageTime );

    ClosingMarketDataWithSettlementPriceAndOpenInterest SettlementPriceMsg( Exchange, mkt, Symbol );
    DayOpenPx.LoadFromStream( Stream );
    NDayHighPx.LoadFromStream( Stream );
    NDayLowPx.LoadFromStream( Stream );
    DayClose.LoadFromStream( Stream );
    SettlementPx.LoadFromStream( Stream );
    OpenInterest.LoadFromStream( Stream );

    SettlementPriceMsg.SetMessageTime( MessageTime );
    SettlementPriceMsg.SetOpenPx( DayOpenPx.ToDouble() );
    SettlementPriceMsg.SetDayHighPx( NDayHighPx.ToDouble() );
    SettlementPriceMsg.SetDayLowPx( NDayLowPx.ToDouble() );
    SettlementPriceMsg.SetClosePx( DayClose.ToDouble() );
    SettlementPriceMsg.SetSettlementPrice( SettlementPx.ToDouble() );
    SettlementPriceMsg.SetOpenInterest( OpenInterest.ToDouble() );

    FListener->OnMarketDataMessage( Exchange, Symbol, &SettlementPriceMsg );
}
//------------------------------------------------------------------------------