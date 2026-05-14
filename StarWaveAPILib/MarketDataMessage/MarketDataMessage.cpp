//---------------------------------------------------------------------------
#include "MarketDataMessage.h"
#include "Utility.h"
//---------------------------------------------------------------------------
//	class BaseMessage
//---------------------------------------------------------------------------
BaseMessage::BaseMessage( MsgType Type, const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol )
:FType( Type )
,FExchange( Exchange )
,FMarket( mkt )
,FSymbol( Symbol )
,FSequence(0)
,FExchangeFormat(0)
{
	UFC::GetTimeString( FTime, TRUE );
	for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
	{
		FTickCountUS[i] = 0;
		FTickLagUS[i] = 0;
	}
}
//---------------------------------------------------------------------------
BaseMessage::BaseMessage( const BaseMessage& Msg )
{
	CopyBase( Msg );
}
//---------------------------------------------------------------------------
void BaseMessage::CopyBase( const BaseMessage& Msg )
{
	FType     = Msg.FType;
	FExchange = Msg.FExchange;
	FMarket   = Msg.FMarket;
	FSymbol   = Msg.FSymbol;
	FSequence = Msg.FSequence;
	FTime     = Msg.FTime;
	FVersion  = Msg.FVersion;
	FExchangeFormat = Msg.FExchangeFormat;
	for( register int i = 0; i < MAX_TICK_COUNT_NO; i++)
	{
		FTickCountUS[i] = Msg.FTickCountUS[i];
		FTickLagUS[i]   = Msg.FTickLagUS[i];
	}
}
//---------------------------------------------------------------------------
//	class BasicMessage
//---------------------------------------------------------------------------
BasicData::BasicData( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol )
:BaseMessage( mtBasicInfo, Exchange, mkt, Symbol )
,FBullPx(0)
,FBearPx(0)
,FRefPx(0)
,FStrikePx(0)
,FDecimalLocator(0)
,FStrikeDecimalLocator(0)
{}
//---------------------------------------------------------------------------
//	class BrokerQueue
//---------------------------------------------------------------------------
HKBrokerQueue::HKBrokerQueue( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol, bool Buy )
:BaseMessage( mtBrokerQueue, Exchange, mkt, Symbol )
,FIsBuy( Buy )
,FCount( 0 )
{
}
//---------------------------------------------------------------------------
BrokerQueueItemType HKBrokerQueue::GetType( int index )
{
	 if( index < FCount )
		 return  (BrokerQueueItemType)FBrokerQueue[index].Type;
	 return bqtNull;
}
//---------------------------------------------------------------------------
int  HKBrokerQueue::GetValue( int index )
{
	 if( index < FCount )
		 return  FBrokerQueue[index].Item;
	 return 0;
}
//---------------------------------------------------------------------------
void HKBrokerQueue::LoadFromStream( UFC::PStream* Stream )
{
	Stream->Read( FBrokerQueue, sizeof(BrokerQueueItem)*40 );
	for( int i = 0; i < 40; i++ )
	{
		if( FBrokerQueue[i].Item == 0 )
		{
			FCount = i+1;
			return;
		}
	}
	FCount = 40;
}
//---------------------------------------------------------------------------
//	class MatchInfo
//---------------------------------------------------------------------------
MatchInfo::MatchInfo( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol )
:BaseMessage( mtTrade, Exchange, mkt, Symbol )
,FTradeQty(0)
,FTradePx(0)
,FRefPx( 0 )
,FIsTest( false )
,FHKExTrdType( 0 )
{
	UFC::GetTimeString( FTradeTime, TRUE );
}
//---------------------------------------------------------------------------
//	class TotalMatch
//---------------------------------------------------------------------------
TotalMatch::TotalMatch( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol )
:BaseMessage( mtTotalTrade, Exchange, mkt, Symbol )
,FTotalMatchQty(0)
,FBuyMatchCount(0)
,FSellMatchCount(0)
,FIsTest( false )
{}
//---------------------------------------------------------------------------
// class DayHighLowPrice
//---------------------------------------------------------------------------
DayHighLowPrice::DayHighLowPrice( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol )
:BaseMessage( mtDayHighLow, Exchange, mkt, Symbol )
,FDayHighPx(0)
,FDayLowPx(0)
{}
//---------------------------------------------------------------------------
//	class OpeningInfo
//---------------------------------------------------------------------------
OpeningInfo::OpeningInfo( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol )
:BaseMessage( mtOpeningInfo, Exchange, mkt, Symbol )
,FOpeningQty(0)
,FOpeningPx(0)
{
    UFC::GetTimeString( FMatchTime );
}
//---------------------------------------------------------------------------
// class OrderBookData
//---------------------------------------------------------------------------
OrderBookData::OrderBookData( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol )
:BaseMessage( mtOrderBookData, Exchange, mkt, Symbol )
,FDerivedFlag(false)
,FDerivedBuyPrice(0)
,FDerivedSellPrice(0)
,FDerivedBuyQty(0)
,FDerivedSellQty(0)
,FIsTest( false )
,HasNumOfOrders( false )
{
    FDepth.Reset();
}
//---------------------------------------------------------------------------
OrderBookData::OrderBookData( const OrderBookData& Msg )
:BaseMessage( Msg )
{
	CopyOrderBook( Msg );
}
//---------------------------------------------------------------------------
void OrderBookData::CopyOrderBook( const OrderBookData& Msg )
{
	double Px;
	int Qty;

	for( register int i = 1; i <= MAX_DEPTH; i++ )
	{
		if( Msg.GetBuyDepth( i, Px, Qty ) )
			SetBuyDepth( i, Px, Qty );
		if( Msg.GetSellDepth( i, Px, Qty ) )
			SetSellDepth( i, Px, Qty );
	}

	FDerivedFlag      = Msg.FDerivedFlag;
	FDerivedBuyPrice  = Msg.FDerivedBuyPrice;
	FDerivedSellPrice = Msg.FDerivedSellPrice;
	FDerivedBuyQty    = Msg.FDerivedBuyQty;
	FDerivedSellQty   = Msg.FDerivedSellQty;
	FIsTest           = Msg.FIsTest;
	HasNumOfOrders    = Msg.HasNumOfOrders;
	if( HasNumOfOrders == true )
	{
		memcpy( BuyNumOfOrders, Msg.BuyNumOfOrders, sizeof( UInt32)*MAX_DEPTH );
		memcpy( SellNumOfOrders, Msg.SellNumOfOrders, sizeof( UInt32)*MAX_DEPTH );
    }
}
//---------------------------------------------------------------------------
//	class ClosingMarketData
//---------------------------------------------------------------------------
ClosingMarketData::ClosingMarketData( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol )
:BaseMessage( mtClosingMarketData, Exchange, mkt, Symbol )
,FTermHighPx(0)
,FTermLowPx(0)
,FDayHighPx(0)
,FDayLowPx(0)
,FOpenPx(0)
,FBuyPx(0)
,FSellPx(0)
,FClosePx(0)
,FBuyTotalCount(0)
,FBuyTotalQty(0)
,FSellTotalCount(0)
,FSellTotalQty(0)
,FTotalTradeCount(0)
,FTotalTradeQty(0)
,FCombineBuyTotalCount(0)
,FCombineBuyTotalQty(0)
,FCombineSellTotalCount(0)
,FCombineSellTotalQty(0)
{}
//---------------------------------------------------------------------------
ClosingMarketDataWithSettlementPrice::ClosingMarketDataWithSettlementPrice( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol )
:ClosingMarketData( Exchange, mkt, Symbol )
,FSettlementPx(0)
{
	FType = mtClosingMarketDataWithSettlementPrice;
}
//---------------------------------------------------------------------------
ClosingMarketDataWithSettlementPriceAndOpenInterest::ClosingMarketDataWithSettlementPriceAndOpenInterest( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol )
:ClosingMarketDataWithSettlementPrice( Exchange, mkt, Symbol )
,FOpenInterest(0)
{
	FType = mtClosingMarketDataWithSettlementPriceAndOpenInterest;
}
//---------------------------------------------------------------------------
//	class MarketDataMessage
//---------------------------------------------------------------------------
MarketDataMessage::MarketDataMessage( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol )
:OrderBookData( Exchange, mkt, Symbol )
,FTradePx(0)
,FTradeQty(0)
,FOpeningPx(0)
,FClosingPx(0)
,FSettlementPx(0)
,FDayHighPx(0)
,FDayLowPx(0)
,FTradeVolume(0)
,FOpenInterest(0)
,FBuyTotalQty(0)
,FSellTotalQty(0)
,FPreCloseQty(0)
,FTradeTime( "--:--:--.---" )
,FTradeDate( "--------" )
,FPreClosePx( 0 )
,FCPCode( cpNone )
,FTradingSession( tsNormal )
,FLastFilled( 0 )
,FLastDepth( 0 )
,FLastNTrade( 0 )
,FSuspensionIndicator( 1 )
{
	FType = mtRecover;
}
//---------------------------------------------------------------------------
MarketDataMessage::MarketDataMessage( const MarketDataMessage& Msg )
:OrderBookData( Msg )
{
	FType = mtRecover;
	FTradePx = Msg.FTradePx;
	FTradeQty = Msg.FTradeQty;
	FTradeVolume = Msg.FTradeVolume;
	FBuyTotalQty = Msg.FBuyTotalQty;
	FSellTotalQty = Msg.FSellTotalQty;
	FOpeningPx = Msg.FOpeningPx;
	FClosingPx = Msg.FClosingPx;
	FSettlementPx = Msg.FSettlementPx;
	FDayHighPx = Msg.FDayHighPx;
	FDayLowPx = Msg.FDayLowPx;
	FOpenInterest = Msg.FOpenInterest;
	FTradeTime = Msg.FTradeTime;
	FTradeDate = Msg.FTradeDate;
	FPreClosePx = Msg.FPreClosePx;
	FProductID = Msg.FProductID;
	FCPCode = Msg.FCPCode;
	FTradingSession = Msg.FTradingSession;
	FLastFilled  = Msg.FLastFilled;
	FLastDepth   = Msg.FLastDepth;
	FLastNTrade  = Msg.FLastNTrade;
	FPreCloseQty = Msg.FPreCloseQty;
	for( int i = 0; i < FLastNTrade; i++ )
	{
		FTrdTime[i] = Msg.FTrdTime[i];
		FTrdPx[i]   = Msg.FTrdPx[i];
		FTrdQty[i]  = Msg.FTrdQty[i];
		FTrdType[i] = Msg.FTrdType[i];
	}
	FSuspensionIndicator = Msg.FSuspensionIndicator;
}
//---------------------------------------------------------------------------
MarketDataMessage::MarketDataMessage( UFC::PStream* Stream )
{
	UFC::UDateTime 	DateTime;
	UFC::Int8		CMarket, DFlag, Range;
	UFC::NInt32     NMatchQty, NBuyTotalQty, NSellTotalQty, NTotalQty, NOpenInterest, NBuyTotalCount, NSellTotalCount, NLastNTrd, NSuspensionIndicator, PreCloseQty;
	UFC::NDouble    NMatchPx, NOpenPx, NHighPx, NLowPx, NSettlementPx, NClosePx, NPreClosePx, NSettlePx;
	UFC::NDouble	BullPx, BearPx, RefPx, StrikePx;

    FType = mtRecover;
    FExchange.LoadFromStream( Stream );
    FSymbol.LoadFromStream( Stream );

	Stream->Read( (void*)&CMarket, 1 );
    FMarket = ( Market )CMarket;

	DateTime.LoadFromStream( Stream );
	FTradeTime.Printf( "%02d:%02d:%02d.%03d", DateTime.getHour(), DateTime.getMinute(), DateTime.getSecond(), DateTime.getMillisecond() );
	FTradeDate.Printf( "%04d%02d%02d", DateTime.getYear(), DateTime.getMonth(),DateTime.getDate());

    NMatchPx.LoadFromStream( Stream );
    FTradePx = NMatchPx.ToDouble();

    NMatchQty.LoadFromStream( Stream );
    FTradeQty = NMatchQty.ToInt32();

    FDepth.LoadFromStream( Stream );

    NOpenPx.LoadFromStream( Stream );
    FOpeningPx = NOpenPx.ToDouble();

    NHighPx.LoadFromStream( Stream );
    FDayHighPx = NHighPx.ToDouble();

    NLowPx.LoadFromStream( Stream );
    FDayLowPx = NLowPx.ToDouble();

    NSettlementPx.LoadFromStream( Stream );
    FSettlementPx = NSettlementPx.ToDouble();

    NClosePx.LoadFromStream( Stream );
    FClosingPx = NClosePx.ToDouble();

    NBuyTotalQty.LoadFromStream( Stream );
    FBuyTotalQty = NBuyTotalQty.ToInt32();

    NSellTotalQty.LoadFromStream( Stream );
    FSellTotalQty = NSellTotalQty.ToInt32();

    NTotalQty.LoadFromStream( Stream );
    FTradeVolume = NTotalQty.ToInt32();

    NOpenInterest.LoadFromStream( Stream );
    FOpenInterest = NOpenInterest.ToInt32();

    Stream->Read( (void*)&DFlag, 1 );

    if( DFlag == 0 )
        FDerivedFlag = false;
    else
        FDerivedFlag = true;

    if( FDerivedFlag )
    {
        UFC::NDouble	DBuyPx;
		UFC::NInt32		DBuyQty;
		UFC::NDouble	DSellPx;
        UFC::NInt32		DSellQty;
        DBuyPx.LoadFromStream( Stream );
        DBuyQty.LoadFromStream( Stream );
        DSellPx.LoadFromStream( Stream );
        DSellQty.LoadFromStream( Stream );

        FDerivedBuyPrice = DBuyPx.ToDouble();
        FDerivedBuyQty = DBuyQty.ToInt32();
        FDerivedSellPrice = DSellPx.ToDouble();
        FDerivedSellQty = DSellQty.ToInt32();
    }

    NBuyTotalCount.LoadFromStream( Stream );
    FBuyTotalCount = NBuyTotalCount.ToInt32();

    NSellTotalCount.LoadFromStream( Stream );
    FSellTotalCount = NSellTotalCount.ToInt32();

    NPreClosePx.LoadFromStream( Stream );

    if(FExchange == "TAIFEX" || FExchange == "TWSE" || FExchange == "OTC" || FExchange == "TWSEOdd" || FExchange == "OTCOdd" || FExchange == "HKEx" )
        FPreClosePx = NPreClosePx.ToDouble();
    else
        FPreClosePx = 0;

	Stream->Read( (void*)&Range, 1 );
	FRange = Range;

	FProductID.LoadFromStream( Stream );

	BullPx.LoadFromStream( Stream );
	FBullPx = BullPx.ToDouble();

	BearPx.LoadFromStream( Stream );
	FBearPx = BearPx.ToDouble();

	RefPx.LoadFromStream( Stream );
	FRefPx = RefPx.ToDouble();

	StrikePx.LoadFromStream( Stream );
	FStrikePx = StrikePx.ToDouble();

	char CPVal;
	Stream->Read( (void*)&CPVal, sizeof(char) );
	FCPCode = (CallPutCode)CPVal;

	FSettleMonth.LoadFromStream( Stream );
	FEndDate.LoadFromStream( Stream );

	///< Last N trade, up to 10 trade.(for HKEx)
	NLastNTrd.LoadFromStream( Stream );
	FLastNTrade = NLastNTrd.ToInt32();
	if( FLastNTrade > 10 )
		FLastNTrade = 10;
	for( int i = 0; i < FLastNTrade; i++)
	{
		UFC::NInt32  TrdTime;
		UFC::NDouble TrdPx;
		UFC::NInt32  TrdQty;
		UFC::NInt32  TrdType;

		TrdTime.LoadFromStream( Stream );
		TrdPx.LoadFromStream( Stream );
		TrdQty.LoadFromStream( Stream );
		TrdType.LoadFromStream( Stream );
		FTrdTime[i] = TrdTime.ToInt32();
		FTrdPx[i]   = TrdPx.ToDouble();
		FTrdQty[i]  = TrdQty.ToInt32();
		FTrdType[i] = TrdType.ToInt32();
	}
	NSuspensionIndicator.LoadFromStream( Stream );
	FSuspensionIndicator = NSuspensionIndicator.ToInt32();
	FTradingSession = tsNormal;
	FLastFilled = 0;
	FLastDepth = 0;

	PreCloseQty.LoadFromStream( Stream );
	FPreCloseQty = PreCloseQty.ToInt32();
}
//---------------------------------------------------------------------------
void MarketDataMessage::CopyFilled( MatchInfo& Matched )
{
	Matched.CopyBase( *this );
	Matched.SetMatchTime( FTradeTime );
	Matched.SetMatchQty( FTradeQty );
	Matched.SetMatchPrice( FTradePx );
	Matched.SetRefPx( FRefPx );
	Matched.SetTestMatch( false );
	Matched.SetTradingSession( FTradingSession );
}
//---------------------------------------------------------------------------
void MarketDataMessage::CopyDepth( OrderBookData& Depth )
{
	Depth.CopyBase( *this );
    Depth.CopyOrderBook( *this );
}
//---------------------------------------------------------------------------
//	class UnderlyingIndexInfo
//---------------------------------------------------------------------------
UnderlyingIndexInfo::UnderlyingIndexInfo( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol )
:BaseMessage( mtUnderlyingIndexInfo, Exchange, mkt, Symbol )
,FIndexValue( 0 )
,FBuyValue( 0 )
,FSellValue( 0 )
,FFixValue( 0 )
,FHighValue( 0 )
,FLowValue( 0 )
,FOpenValue( 0 )
,FCloseValue( 0 )
,FTotalQty( 0 )
,FTotalCount( 0 )
,FTotalAmount( 0 )
{}
//---------------------------------------------------------------------------
//	class SunOfOrderInfo
//---------------------------------------------------------------------------
SumOfOrderInfo::SumOfOrderInfo( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol )
:BaseMessage( mtSumOfOrderInfo, Exchange, mkt, Symbol )
,FBuyOrderCount(0)
,FSellOrderCount(0)
,FBuyQty(0)
,FSellQty(0)
{}
//---------------------------------------------------------------------------
// class NewMessage
//---------------------------------------------------------------------------
NewsMessage::NewsMessage( const UFC::AnsiString& Exchange, Market mkt, int Key, const UFC::AnsiString& Data )
:BaseMessage( mtNews, Exchange, mkt, "News" )
,FKey( Key )
,FData( Data )
{}
//---------------------------------------------------------------------------
// class SystemMessage
//---------------------------------------------------------------------------
SystemMessage::SystemMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt )
:BaseMessage( mtSystemMsg, Exchange, mkt, Symbol )
{}
//---------------------------------------------------------------------------
// class ErrorMessage
//---------------------------------------------------------------------------
ErrorMessage::ErrorMessage( const UFC::AnsiString& Exchange, Market mkt )
:BaseMessage( mtErrorMsg, Exchange, mkt, "Error" )
{}
//---------------------------------------------------------------------------
ErrorMessage::ErrorMessage( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol )
:BaseMessage( mtErrorMsg, Exchange, mkt, Symbol )
{}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Advanced Message
//---------------------------------------------------------------------------
AdvancedMessage::AdvancedMessage(const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol, UFC::PStream* Data, MTree* TreeData)
:BaseMessage(mtAdvancedMsg, Exchange, mkt, Symbol)
,FAdvancedData( Data )
{
	if (!TreeData->get("FunctionCode", FFunctionCode)) 
	{
		FFunctionCode = 0;
	}
}
//---------------------------------------------------------------------------
//// class  TAIFEXBasicMessage
////---------------------------------------------------------------------------
//TAIFEXBasicMessage::TAIFEXBasicMessage(const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol)
//:BaseMessage(mtErrorMsg, Exchange, mkt, Symbol)
//{
//
//}
////---------------------------------------------------------------------------
//// class  TAIFEXSystemMessage
////---------------------------------------------------------------------------
//TAIFEXSystemMessage::TAIFEXSystemMessage(const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol)
//:BaseMessage(mtErrorMsg, Exchange, mkt, Symbol)
//{
//
//}
////---------------------------------------------------------------------------
//// class  TWSEBasicMessage
////---------------------------------------------------------------------------
//TWSEBasicMessage::TWSEBasicMessage(const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol)
//	:BaseMessage(mtErrorMsg, Exchange, mkt, Symbol)
//{
//
//}
////---------------------------------------------------------------------------
//// class  TWSEOrderStatsMessage
////---------------------------------------------------------------------------
//TWSEOrderStatsMessage::TWSEOrderStatsMessage(const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol) 
//:BaseMessage(mtErrorMsg, Exchange, mkt, Symbol)
//{
//
//}
////---------------------------------------------------------------------------
//// class  TWSETradeStatsMessage
////---------------------------------------------------------------------------
//TWSETradeStatsMessage::TWSETradeStatsMessage(const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol) 
//:BaseMessage(mtErrorMsg, Exchange, mkt, Symbol)
//{
//
//}
//---------------------------------------------------------------------------
