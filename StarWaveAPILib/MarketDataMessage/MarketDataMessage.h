//---------------------------------------------------------------------------

#ifndef MarketDataMessageH
#define MarketDataMessageH
//---------------------------------------------------------------------------
#include "UFC.h"
#include "Sigo.h"
#include "DepthObject.h"
//---------------------------------------------------------------------------
#define MAX_TICK_COUNT_NO   5
//---------------------------------------------------------------------------
enum Market
{
	mTWFutures     = 0, ///< TAIFEX Taiwan Futures
	mTWOptions     = 1, ///< TAIFEX Taiwan Options
	mTSE           = 2, ///< TSEC   Taiwan equity
	mOTC           = 3, ///< OTC    Taiwan
	mPATSFutures   = 4, ///< PATS          Futures
	mPATSOptions   = 5, ///< PATS          Options
	mCNFutures     = 6,  ///< GLQH   China  Futures
	mCNOptions     = 7,  ///< GLQH   China  Options
	mUnderlying    = 8,
	mFX            = 9,
	mPATSAll       = 10, ///< PATS  Futures & Options
	mES            = 11, ///< ES   Taiwan  stock
	mUnknownMarket = 12,
	mStockETF      = 13
};
//---------------------------------------------------------------------------
enum MsgType
{
	mtRecover = 0,
	mtBasicInfo = 1,
	mtTrade = 2,
	mtTotalTrade = 3,
	mtDayHighLow = 4,
	mtOpeningInfo = 5,
	mtSumOfOrderInfo = 6,
	mtNews = 7,
	mtUnderlyingIndexInfo = 8,
	mtClosingMarketData = 9,
	mtClosingMarketDataWithSettlementPrice = 10,
	mtClosingMarketDataWithSettlementPriceAndOpenInterest = 11,
	mtCombinenationProductClosingMarketData = 12,
	mtOrderBookData = 13,
	mtQuoteRequest = 14,
	mtFullSnapShot = 15,
    mtSystemMsg = 16,
    mtPreNClosePx = 17,
    mtResetForOpen = 18,
	mtErrorMsg = 19,
	mtBrokerQueue = 20,
	///< HKEx Only
	mtSuspensionIndicator = 21,
	mtPreClosingPrice = 22,
	///< AdvancedMsg
	mtAdvancedMsg = 50,
	mtAdvancedBasicMsg = 51,
	mtAdvancedTAIFEXSystemMsg = 52,
	mtAdvancedTWSEOrderStatisticMsg = 53,
	mtAdvancedTWSETradeStatisticMsg = 54,
	///<
	mtOddOrderBookData = 97,
	mtBuyBrokerQueueData = 98,
	mtSellBrokerQueueData = 99
	///<

};///< StarWave Message Type
//---------------------------------------------------------------------------
enum CallPutCode
{
	cpCall = 0,
	cpPut  = 1,
	cpNone = 2
};
//---------------------------------------------------------------------------
enum TradingSession
{
	tsNormal  = 0,
	tsPreOpen = 1,
	tsOffHour = 2,
	tsOdd     = 3
};

//---------------------------------------------------------------------------
typedef enum
{
	bqtBrokerID  = 'B',
	bqtNumberofSpread = 'S',
	bqtNull = 0

} BrokerQueueItemType;
//---------------------------------------------------------------------------
//						class BaseMessage
//			The base class of market data message.
//---------------------------------------------------------------------------
class BaseMessage
{
protected:
	MsgType				FType;
	UFC::AnsiString 	FExchange;
	Market  			FMarket;
	UFC::AnsiString		FSymbol;
	UFC::AnsiString		FTime; ///< HH:MM:ss.mmm
	UFC::Int32			FSequence; ///< Sequence from Exchange(TAIFEX), 0 for other data source.
	UFC::Int32			FVersion;
	UFC::Int32			FExchangeFormat;
    UFC::Int64			FTickCountUS[MAX_TICK_COUNT_NO];
    UFC::Int64			FTickLagUS[MAX_TICK_COUNT_NO];
public: /// constructor
	BaseMessage( MsgType Type, const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol );
	BaseMessage( const BaseMessage& );
	BaseMessage() {}
	void CopyBase( const BaseMessage& );
public:
	///< Getter
	MsgType GetMsgType( void ) { return FType; }
	UFC::AnsiString& GetExchange( void ) { return FExchange; }
	Market GetMarket( void ) { return FMarket; }
	UFC::AnsiString& GetSymbol( void ) { return FSymbol; }
	UFC::AnsiString& GetMessageTime( void ) { return FTime; }
	UFC::Int32 GetSequence( void ) { return FSequence; }
	UFC::Int32 GetVersion( void ) { return FVersion; }
	UFC::Int32 GetExchangeFormat(void) { return FExchangeFormat; }
	UFC::Int64 GetTickCountUS( int index ) { return FTickCountUS[index]; }
    UFC::Int64 GetTickLagUS( int index ) { return FTickLagUS[index]; }
	/// Setter
	void SetMessageTime( const UFC::AnsiString Time ) { FTime = Time; }
	void SetSequence( UFC::Int32 Seq ) { FSequence = Seq; }
	void SetExchangeFormat( UFC::Int32 Format) { FExchangeFormat = Format; }
	void SetVersion( const UFC::Int32 Version ) { FVersion = Version; }
	void SetMarket( Market market ) { FMarket = market; }
    void SetTickCountUS( int index, UFC::Int64 TickCountUS ) { FTickCountUS[index] = TickCountUS; }
	void SetTickLagUS( int index, UFC::Int64 TickLagUS ) { FTickLagUS[index] = TickLagUS; }
};
//---------------------------------------------------------------------------
typedef struct
{
	UFC::UInt16  Item;
	UFC::UInt8   Type;
	char         Filler;

}BrokerQueueItem ;
//---------------------------------------------------------------------------
class HKBrokerQueue : public BaseMessage
{
protected:
	BrokerQueueItem  FBrokerQueue[ 40 ];
	bool             FIsBuy;
	int              FCount;
public:
	HKBrokerQueue( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol, bool Buy );
	int Count( void ) { return FCount; }
	BrokerQueueItemType GetType( int index );
	int                 GetValue( int index );
	bool                IsBuy( void ) { return FIsBuy; }
	void LoadFromStream( UFC::PStream* Stream );
};
//---------------------------------------------------------------------------
// 						class BasicData
//  1.Message timing: send before market, interval of 60 seconds between messages,
//	  disclose for all products.
//	2.Reset yesterday's order book, and prepare for market open.
//
//---------------------------------------------------------------------------
class BasicData : public BaseMessage
{
protected:
	double 		FBullPx;
	double 		FBearPx;
	double 		FRefPx;
	double		FStrikePx;
	UFC::Int32	FDecimalLocator;
	UFC::Int32	FStrikeDecimalLocator;
public:
	BasicData( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol );
public:
	double GetBullPrice( void ) { return FBullPx; }
	double GetBearPrice( void ) { return FBearPx; }
	double GetReferencePrice( void ) { return FRefPx; }
	double GetStrikePrice( void ) { return FStrikePx; }	///< For options, 0 for futures and other markets.
	UFC::Int32 GetDecimalLocator( void ) { return FDecimalLocator; }
	UFC::Int32 GetStrikeDecimalLocator( void ) { return FStrikeDecimalLocator; }

	void SetBullPrice( double Price ) { FBullPx = Price; }
	void SetBeatPrice( double Price ) { FBearPx = Price; }
	void SetReferencePrice( double Price ) { FRefPx = Price; }
	void SetStrikePrice( double Price ) { FStrikePx = Price; }
	void SetDecimalLocator( UFC::Int32 Digit ) { FDecimalLocator = Digit; }
	void SetStrikeDecimalLocator( UFC::Int32 Digit ) { FStrikeDecimalLocator = Digit; }
};
//---------------------------------------------------------------------------
//						class MatchInfo
//     Message timing: Intra-day real-time disclose for all products
//---------------------------------------------------------------------------
class MatchInfo : public BaseMessage
{
protected:
    UFC::AnsiString     FTradeTime;
    UFC::Int32          FTradeQty;
    UFC::Int32          FTotalQty;
    double              FTradePx;
    double              FRefPx;
    bool                FIsTest;
    TradingSession      FTradingSession;
    UFC::Int32          FHKExTrdType;
public:
    MatchInfo( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol );
    MatchInfo() {}
public:
    UFC::AnsiString& GetMatchTime( void ) { return FTradeTime; }
    UFC::Int32       GetMatchQty( void ) { return  FTradeQty; }
    UFC::Int32       GetTotalQty( void ) { return  FTotalQty; }
    double           GetMatchPrice( void ) {return FTradePx; }
    double           GetRefPx( void ) { return FRefPx; }
    bool             IsTestMatch( void ) { return FIsTest; }
    TradingSession   GetTradingSession( void ) { return FTradingSession; }
    UFC::Int32       GetHKExTrdType( void ) { return FHKExTrdType; }

    void SetMatchTime( const UFC::AnsiString& Time ) { FTradeTime = Time; }
    void SetMatchQty( UFC::Int32 Qty ) { FTradeQty = Qty; }
    void SetTotalQty( UFC::Int32 Qty ) { FTotalQty = Qty; }
    void SetMatchPrice( double Price ) { FTradePx = Price; }
    void SetRefPx( double Px ) { FRefPx = Px; }
    void SetTestMatch( bool YN ) { FIsTest = YN; }
    void SetTradingSession( TradingSession TS ){ FTradingSession =TS; }
    void SetHKExTrdType( int TT ){FHKExTrdType = TT; }
};
//---------------------------------------------------------------------------
//						class TotalMatch
//      Message timing: Intra-day real-time disclose for all products
//---------------------------------------------------------------------------
class TotalMatch : public BaseMessage
{
protected:
	UFC::Int32 FTotalMatchQty;
	UFC::Int32 FBuyMatchCount;
	UFC::Int32 FSellMatchCount;
	bool       FIsTest;
	TradingSession      FTradingSession;
public:
	TotalMatch( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol );
public:
	UFC::Int32 GetTotalMatchQty( void )      { return FTotalMatchQty; }
	UFC::Int32 GetBuyMatchCount( void )      { return FBuyMatchCount; }
	UFC::Int32 GetSellMatchCount( void )     { return FSellMatchCount; }
	bool       IsTestMatch( void )           { return FIsTest; }
	TradingSession GetTradingSession( void ) { return FTradingSession; }

	void SetTotalMatchQty( UFC::Int32 Qty )    { FTotalMatchQty = Qty; }
	void SetBuyMatchCount( UFC::Int32 Count )  { FBuyMatchCount = Count; }
	void SetSellMatchCount( UFC::Int32 Count ) { FSellMatchCount = Count; }
	void SetTestMatch( bool YN )               { FIsTest = YN; }
	void SetTradingSession( TradingSession TS ){ FTradingSession =TS; }
};
//---------------------------------------------------------------------------
//						class DayHighLowPrice
//		Message timing: TAIFEX sends intra-day real-time changes of highs and lows.
//---------------------------------------------------------------------------
class DayHighLowPrice : public BaseMessage
{
protected:
	double FDayHighPx;
	double FDayLowPx;
	UFC::AnsiString FShowTime;
public:
	DayHighLowPrice( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol );
public:
	double GetDayHighPrice( void ) { return FDayHighPx; }
	double GetDayLowPrice( void ) { return FDayLowPx; }
	const UFC::AnsiString& GetShowTime( void ) { return FShowTime; }

	void SetDayHighPrice( double Price ) { FDayHighPx = Price; }
	void SetDayLowPrice( double Price ) { FDayLowPx = Price; }
	void SetShowTime( const UFC::AnsiString& Time ) { FShowTime = Time; }
};
//---------------------------------------------------------------------------
// 						class OpeningInfo
//	Message timing: TAIFEX sends the first match price and quantity of the day when it takes place
//	three times at one minute apart between each time.
//---------------------------------------------------------------------------
class OpeningInfo : public BaseMessage
{
protected:
	double 			FOpeningPx;
	UFC::Int32      FOpeningQty;
	UFC::AnsiString	FMatchTime;
public:
	OpeningInfo( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol );
public:
	UFC::AnsiString& GetMatchTime( void ) { return FMatchTime; }
	UFC::Int32 GetOpeningQty( void ) { return  FOpeningQty; }
	double GetOpeningPrice( void ) {return FOpeningPx; }

	void SetMatchTime( const UFC::AnsiString& Time ) { FMatchTime = Time; }
	void SetOpeningQty( UFC::Int32 Qty ) { FOpeningQty = Qty; }
	void SetOpeningPrice( double Price ) { FOpeningPx = Price; }
};
//---------------------------------------------------------------------------
//						class OrderBookData
//   TAIFEX sends order book data for all products during market hours.
//	 The messages are 0.25 seconds apart; disclose only when there are changes to the quantity and/or
//	 price of the best five bid/ask prices (include order and quote).
//---------------------------------------------------------------------------
class OrderBookData : public BaseMessage
{
protected:
	DepthObject	FDepth;
	bool 		FDerivedFlag;
	double 	    FDerivedBuyPrice;
	UFC::Int32 	FDerivedBuyQty;
	double      FDerivedSellPrice;
	UFC::Int32	FDerivedSellQty;
	bool        FIsTest;
	TradingSession FTradingSession;
public:
	OrderBookData( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol );
	OrderBookData( const OrderBookData& Msg );
	OrderBookData() {}
	void CopyOrderBook( const OrderBookData& Msg );
public:
	UFC::UInt32 BuyNumOfOrders[MAX_DEPTH];
	UFC::UInt32 SellNumOfOrders[MAX_DEPTH];
	bool HasNumOfOrders;
public:
	double GetBuyPx( int Depth ) { return FDepth.GetBidPx( Depth ); }
	UFC::Int32 GetBuyQty( int Depth ) { return FDepth.GetBidQty( Depth ); }
	double GetSellPx( int Depth ) { return FDepth.GetAskPx( Depth ); }
	UFC::Int32 GetSellQty( int Depth ) { return FDepth.GetAskQty( Depth ); }

	double GetBuyPrice1( void ) { return GetBuyPx( 1 ); }
	double GetBuyPrice2( void ) { return GetBuyPx( 2 ); }
	double GetBuyPrice3( void ) { return GetBuyPx( 3 ); }
	double GetBuyPrice4( void ) { return GetBuyPx( 4 ); }
	double GetBuyPrice5( void ) { return GetBuyPx( 5 ); }
	UFC::Int32 GetBuyQty1( void ) { return GetBuyQty( 1 ); }
	UFC::Int32 GetBuyQty2( void ) { return GetBuyQty( 2 ); }
	UFC::Int32 GetBuyQty3( void ) { return GetBuyQty( 3 ); }
	UFC::Int32 GetBuyQty4( void ) { return GetBuyQty( 4 ); }
	UFC::Int32 GetBuyQty5( void ) { return GetBuyQty( 5 ); }
	double GetSellPrice1( void ) { return GetSellPx( 1 ); }
	double GetSellPrice2( void ) { return GetSellPx( 2 ); }
	double GetSellPrice3( void ) { return GetSellPx( 3 ); }
	double GetSellPrice4( void ) { return GetSellPx( 4 ); }
	double GetSellPrice5( void ) { return GetSellPx( 5 ); }
	UFC::Int32 GetSellQty1( void ) { return GetSellQty( 1 ); }
	UFC::Int32 GetSellQty2( void ) { return GetSellQty( 2 ); }
	UFC::Int32 GetSellQty3( void ) { return GetSellQty( 3 ); }
	UFC::Int32 GetSellQty4( void ) { return GetSellQty( 4 ); }
	UFC::Int32 GetSellQty5( void ) { return GetSellQty( 5 ); }
	bool GetBuyDepth( int Depth, double& Px, int& Qty ) const { return FDepth.GetBidDepth( Depth, Px, Qty ); }
	bool GetSellDepth( int Depth, double& Px, int& Qty ) const { return FDepth.GetAskDepth( Depth, Px, Qty ); }
	bool GetDerivedFlag( void ) { return FDerivedFlag; }
	double GetDerivedBuyPrice( void ) { return FDerivedBuyPrice; }
	double GetDerivedSellPrice( void ) { return FDerivedSellPrice; }
	UFC::Int32 GetDerivedBuyQty( void ) { return FDerivedBuyQty; }
	UFC::Int32 GetDerivedSellQty( void ) { return FDerivedSellQty; }
	bool       IsTestMatch( void ) { return FIsTest; }
	TradingSession GetTradingSession( void ) { return FTradingSession; }

	void SetBuyDepth( int Depth, double Px, int Qty ) { FDepth.SetBidDepth( Depth, Px, Qty ); }
	void SetSellDepth( int Depth, double Px, int Qty ) { FDepth.SetAskDepth( Depth, Px, Qty ); }
	void SetDerivedFlag( bool Flag  ) { FDerivedFlag = Flag; }
	void SetDerivedBuyPrice( double Price ) { FDerivedBuyPrice = Price; }
	void SetDerivedBuyQty( UFC::Int32 Qty ) { FDerivedBuyQty = Qty; }
	void SetDerivedSellPrice( double Price ) { FDerivedSellPrice = Price; }
	void SetDerivedSellQty( UFC::Int32 Qty ) { FDerivedSellQty = Qty; }
	void SetTestMatch( bool YN ) { FIsTest = YN; }
	void SetTradingSession( TradingSession TS ){ FTradingSession =TS; }

	void LoadDepthFromStream( UFC::PStream* Stream ) { FDepth.LoadFromStream( Stream ); }
};
//---------------------------------------------------------------------------
//							class ClosingData
//   TAIFEX sends closing market data of all products after market closing.
//---------------------------------------------------------------------------
class ClosingMarketData : public BaseMessage
{
protected:
	double FTermHighPx;
	double FTermLowPx;
	double FDayHighPx;
	double FDayLowPx;
	double FOpenPx;
	double FBuyPx;
	double FSellPx;
	double FClosePx;
	UFC::Int32 FBuyTotalCount;
	UFC::Int32 FBuyTotalQty;
	UFC::Int32 FSellTotalCount;
	UFC::Int32 FSellTotalQty;
	UFC::Int32 FTotalTradeCount;
	UFC::Int32 FTotalTradeQty;
	UFC::Int32 FCombineBuyTotalCount;
	UFC::Int32 FCombineBuyTotalQty;
	UFC::Int32 FCombineSellTotalCount;
	UFC::Int32 FCombineSellTotalQty;
	UFC::Int32 FCombineTotalQty;
	bool FWithSettlePx;
	bool FWithOI;
public:
	ClosingMarketData( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol );
public:
	double GetTermHighPrice( void ) { return FTermHighPx; }
	double GetTermLowPrice( void ) { return FTermLowPx; }
	double GetDayHighPrice( void ) { return FDayHighPx; }
	double GetDayLowPrice( void ) { return FDayLowPx; }
	double GetOpenPrice( void ) { return FOpenPx; }
	double GetBuyPrice( void ) { return FBuyPx; }
	double GetSellPrice( void  ) { return FSellPx; }
	double GetClosePrice( void ) { return FClosePx; }
	UFC::Int32 GetBuyTotalCount( void ) { return FBuyTotalCount; }
	UFC::Int32 GetBuyTotalQty( void ) { return FBuyTotalQty; }
	UFC::Int32 GetSellTotalCount( void ) { return FSellTotalCount; }
	UFC::Int32 GetSellTotalQty( void ) { return FSellTotalQty; }
	UFC::Int32 GetTotalTradeCount( void ) { return FTotalTradeCount; }
	UFC::Int32 GetTotalTradeQty( void ) { return FTotalTradeQty; }
	UFC::Int32 GetCombineTotalBuyCount( void ) { return FCombineBuyTotalCount; }
	UFC::Int32 GetCombineTotalBuyQty( void ) { return FCombineBuyTotalQty; }
	UFC::Int32 GetCombineTotalSellCount( void ) { return FCombineSellTotalCount; }
	UFC::Int32 GetCombineTotalSellQty( void ) { return FCombineSellTotalQty; }
	UFC::Int32 GetCombineTotalQty( void ) { return FCombineTotalQty; }

	void SetTermHighPx( double Px ) { FTermHighPx = Px; }
	void SetTermLowPx( double Px ) { FTermLowPx = Px; }
	void SetDayHighPx( double Px ) { FDayHighPx = Px; }
	void SetDayLowPx( double Px ) { FDayLowPx = Px; }
	void SetOpenPx( double Px ) { FOpenPx = Px; }
	void SetBuyPx( double Px ) { FBuyPx = Px; }
	void SetSellPx( double Px ) { FSellPx = Px; }
	void SetClosePx( double Px ) { FClosePx = Px; }
	void SetBuyTotalCount( UFC::Int32 Count ) { FBuyTotalCount = Count; }
	void SetBuyTotalQty( UFC::Int32 Qty ) { FBuyTotalQty = Qty; }
	void SetSellTotalCount( UFC::Int32 Count ) { FSellTotalCount = Count; }
	void SetSellTotalQty( UFC::Int32 Qty ) { FSellTotalQty = Qty; }
	void SetTotalTradeCount( UFC::Int32 Count ){ FTotalTradeCount = Count; }
	void SetTotalTradeQty( UFC::Int32 Qty ) { FTotalTradeQty = Qty; }
	void SetCombineBuyTotalCount( UFC::Int32 Count ) { FCombineBuyTotalCount = Count; }
	void SetCombineBuyTotalQty( UFC::Int32 Qty ) { FCombineBuyTotalQty = Qty; }
	void SetCombineSellTotalCount( UFC::Int32 Count ) { FCombineSellTotalCount = Count; }
	void SetCombineSellTotalQty( UFC::Int32 Qty ) { FCombineSellTotalQty = Qty; }
	void SetCombineTotalQty( UFC::Int32 Qty ) { FCombineTotalQty = Qty; }
};
//---------------------------------------------------------------------------
class ClosingMarketDataWithSettlementPrice: public ClosingMarketData
{
protected:
	double FSettlementPx;
public:
	ClosingMarketDataWithSettlementPrice( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol );
public:
	double GetSettlementPrice( void ) { return FSettlementPx; }
	void SetSettlementPrice( double Px ) { FSettlementPx = Px; }
};
//---------------------------------------------------------------------------
class ClosingMarketDataWithSettlementPriceAndOpenInterest: public ClosingMarketDataWithSettlementPrice
{
protected:
	UFC::Int32 FOpenInterest;
public:
	ClosingMarketDataWithSettlementPriceAndOpenInterest( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol );
public:
	UFC::Int32 GetOpenInterest( void ) { return FOpenInterest; }
	void SetOpenInterest( UFC::Int32 OI ) { FOpenInterest = OI; }
};
//---------------------------------------------------------------------------
//						class MarketDataMessage
//  			The full refresh of the market data.
//---------------------------------------------------------------------------
class MarketDataMessage : public OrderBookData
{
protected:
	double			FTradePx;
	UFC::Int32		FTradeQty;
	UFC::AnsiString	FTradeTime;
	UFC::AnsiString	FTradeDate;
	double			FOpeningPx;
	double			FDayHighPx;
	double			FDayLowPx;
	double 			FClosingPx;
	double 			FSettlementPx;
	UFC::Int32 		FOpenInterest;
	UFC::Int32		FTradeVolume;
	UFC::Int32		FBuyTotalQty;
	UFC::Int32		FSellTotalQty;
	UFC::Int32		FBuyTotalCount;
	UFC::Int32		FSellTotalCount;
	UFC::Int32      FPreCloseQty;
    double			FPreClosePx;
    int             FRange;
    UFC::AnsiString FProductID;
    double          FBullPx;
    double          FBearPx;
	double          FRefPx;
    double          FStrikePx;
	CallPutCode     FCPCode;
	UFC::AnsiString	FSettleMonth;
	UFC::AnsiString FEndDate;
	TradingSession  FTradingSession;
	UFC::AnsiString FActualSymbol; //Actual Symbol for Near month Symbol subscribe
private:
	UFCType::UInt32 FLastFilled;
	UFCType::UInt32 FLastDepth;
private:
	int FLastNTrade;
	int FTrdTime[10];
	double FTrdPx[10];
	int FTrdQty[10];
	int FTrdType[10];
	int FSuspensionIndicator;
public:
	UFCType::UInt32 GetFilledUpdateTick( void ) { return FLastFilled; }
	UFCType::UInt32 GetDepthUpdateTick( void ) { return FLastDepth; }
	void   SetFilledUpdateTick( UFCType::UInt32 tick ) { FLastFilled = tick; }
	void   SetDepthUpdateTick( UFCType::UInt32 tick ) { FLastDepth = tick; }
	void   CopyFilled( MatchInfo& Matched );
	void   CopyDepth( OrderBookData& Depth );
public:
	MarketDataMessage( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol );
	MarketDataMessage( const MarketDataMessage& Msg );
	MarketDataMessage( UFC::PStream* Stream );
public:
	double 		GetTradePrice( void ) { return FTradePx; }
	UFC::Int32  GetTradeQty( void ) { return FTradeQty; }
	UFC::AnsiString GetTradeTime( void ){ return FTradeTime; }
	UFC::AnsiString GetTradeDate( void ){ return FTradeDate; }
	double		GetOpeningPrice( void ) { return FOpeningPx; }
	double		GetDayHighPrice( void ) { return FDayHighPx; }
	double		GetDayLowPrice( void ) { return FDayLowPx; }
	UFC::Int32	GetTradeVolume( void ) { return FTradeVolume; } ///< Total Trade
	double GetClosingPrice( void ) { return FClosingPx; }
	double GetSettlementPrice( void ) { return FSettlementPx; }
	UFC::Int32 GetOpenInterest( void ) { return FOpenInterest; }
	UFC::Int32 GetBuyTotalQty( void ) { return FBuyTotalQty; }
	UFC::Int32 GetSellTotalQty( void ) { return FSellTotalQty; }
	UFC::Int32 GetBuyTotalCount( void ) { return FBuyTotalCount; }
	UFC::Int32 GetSellTotalCount( void ) { return FSellTotalCount; }
	double GetPreClosePx( void ) { return FPreClosePx; }
	double GetPreCloseQty(void) { return FPreCloseQty; }
    int GetRange( void ) { return FRange; }
    UFC::AnsiString& GetProductID( void ) { return FProductID; } 
    double GetBullPx( void ) { return FBullPx; }
    double GetBearPx( void ) { return FBearPx; }
    double GetRefPx( void ) { return FRefPx; }
    double GetStrikePx( void ) { return FStrikePx; }
	CallPutCode GetCallPut( void ) { return FCPCode; }
	const UFC::AnsiString& GetSettleMonth( void ) { return FSettleMonth; }
	const UFC::AnsiString& GetEndDate( void ) { return FEndDate; }
	TradingSession GetTradingSession( void ) { return FTradingSession; }
	UFC::AnsiString& GetActualSymbol( void ) { return FActualSymbol; } 
	int GetSuspensionIndicator( void ) { return FSuspensionIndicator; }

	void SetTradePrice( double Price ) { FTradePx = Price; }
	void SetTradeQty( int Qty ) { FTradeQty = Qty; }
	void SetTradeTime( UFC::AnsiString& Time ) { FTradeTime = Time; }
	void SetOpeningPrice( double Price ) { FOpeningPx = Price; }
	void SetDayHighPrice( double Price ) { FDayHighPx = Price; }
	void SetDayLowPrice( double Price ) { FDayLowPx = Price; }
	void SetClosingPrice( double Price ) { FClosingPx = Price; }
	void SetSettlementPrice( double Price ) { FSettlementPx = Price; }
	void SetOpenInterest( UFC::Int32 Qty ) { FOpenInterest = Qty; }
	void SetTradeVolume( UFC::Int32 Qty ) { FTradeVolume = Qty; }
	void SetBuyTotalQty( UFC::Int32 Qty ) { FBuyTotalQty = Qty; }
	void SetSellTotalQty( UFC::Int32 Qty ) { FSellTotalQty = Qty; }
	void SetBuyTotalCount( UFC::Int32 Count ) { FBuyTotalCount = Count; }
	void SetSellTotalCount( UFC::Int32 Count ) { FSellTotalCount = Count; }
	void SetPreClosePx( double Px ) { FPreClosePx = Px; }
	void SetRange( int Range ) { FRange = Range; }
	void SetTradingSession( TradingSession TS ){ FTradingSession =TS; }
	void SetActualSymbol( UFC::AnsiString& ActualSymbol ) { FActualSymbol = ActualSymbol; }
	void SetSuspensionIndicator( int Indicator ) { FSuspensionIndicator = Indicator; }

	int    GetLastNTrade( void )     { return  FLastNTrade; }
	int    GetLastTime( int Index )  { return FTrdTime[Index]; }
	double GetLastPrice( int Index ) { return FTrdPx[Index]; }
	int    GetLastQty( int Index )   { return FTrdQty[Index]; }
	int    GetLastTradeType( int Index ) { return FTrdType[Index]; }
};
//---------------------------------------------------------------------------
//						class  UnderlyingIndexInfo
//		The messages are 3 seconds apart when there are changes to the
//		underlyingindex.
//---------------------------------------------------------------------------
class UnderlyingIndexInfo : public BaseMessage
{
protected:
	double FIndexValue;
	double FBuyValue;
	double FSellValue;
	double FFixValue;
	double FHighValue;
	double FLowValue;
	double FOpenValue;
	double FCloseValue;
	UFC::AnsiString FShowTime;
	UFC::AnsiString FExchangeDate;
	int        FTotalQty;
	int        FTotalCount;
	UFC::Int64 FTotalAmount;
public:
	UnderlyingIndexInfo( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol );
public:
	double GetIndexValue( void ) { return FIndexValue; }
	double GetBuyValue( void ) { return FBuyValue; }
	double GetSellValue( void ) { return FSellValue; }
	double GetFixValue( void ) { return FFixValue; } ///< For TAIFEX
	double GetHighValue(void) { return FHighValue; }
	double GetLowValue(void) { return FLowValue; }
	double GetOpenValue(void) { return FOpenValue; }
	double GetCloseValue(void) { return FCloseValue; }
	const UFC::AnsiString& GetShowTime( void ) { return FShowTime; }
	const UFC::AnsiString& GetExchangeDate( void ) { return FExchangeDate; }
	int   GetTotalQty( void ) { return FTotalQty; }
	int   GetTotalCount( void ) { return FTotalCount; }
	UFC::Int64   GetTotalAmount( void ) { return FTotalAmount; }

	void SetIndexValue( double Index ) { FIndexValue = Index; }
	void SetBuyValue( double Index ) { FBuyValue = Index; }
	void SetSellValue( double Index ) { FSellValue = Index; }
	void SetFixValue( double Index ) { FFixValue = Index; }
	void SetHighValue(double Index) { FHighValue = Index; }
	void SetLowValue(double Index) { FLowValue = Index; }
	void SetOpenValue(double Index) { FOpenValue = Index; }
	void SetCloseValue(double Index) { FCloseValue = Index; }
	void SetShowTime( const UFC::AnsiString& Time ){ FShowTime = Time; }
	void SetExchangeDate( const UFC::AnsiString& Date ){ FExchangeDate = Date; }
	void SetTotalQty( int Qty ) { FTotalQty = Qty; }
	void SetTotalCount( int Count ) { FTotalCount = Count; }
	void SetTotalAmount( UFC::Int64 Amount ) { FTotalAmount = Amount; }

};
//---------------------------------------------------------------------------
class SumOfOrderInfo: public BaseMessage
{
protected:
	UFC::Int32	FBuyOrderCount;
	UFC::Int32	FSellOrderCount;
	UFC::Int32	FBuyQty;
	UFC::Int32	FSellQty;
public:
	SumOfOrderInfo( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol );
public:
	UFC::Int32 GetBuyOrderCount( void ) { return FBuyOrderCount; }
	UFC::Int32 GetSellOrderCount( void ) { return FSellOrderCount; }
	UFC::Int32 GetBuyQty( void ) { return FBuyQty; }
	UFC::Int32 GetSellQty( void ) { return FSellQty; }

	void SetBuyOrderCount( UFC::Int32 Count ) { FBuyOrderCount = Count; }
	void SetSellOrderCount( UFC::Int32 Count ) { FSellOrderCount = Count; }
	void SetBuyQty( UFC::Int32 Qty ) { FBuyQty = Qty; }
	void SetSellQty( UFC::Int32 Qty ) { FSellQty = Qty; }
};
//---------------------------------------------------------------------------
class NewsMessage: public BaseMessage
{
protected:
	int FKey;
	UFC::AnsiString FData;
public:
	NewsMessage( const UFC::AnsiString& Exchange, Market mkt, int Key, const UFC::AnsiString& Data );
	UFC::AnsiString& GetData( void ) { return FData; }
	int GetKey( void ) { return FKey; }
};
//---------------------------------------------------------------------------
class SystemMessage : public  BaseMessage
{
protected:
	int FCode;
	UFC::AnsiString FData;
public:
	SystemMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt );
	UFC::AnsiString& GetData( void ) { return FData; }
	int              GetCode( void ) { return FCode; }
	void SetData( const UFC::AnsiString& Data ) { FData = Data; }
    void SetCode( int Code ) { FCode = Code; }
};
//---------------------------------------------------------------------------
class ErrorMessage : public  BaseMessage
{
protected:	
    UFC::AnsiString FProductID;    
    double          FStrikePx;
	CallPutCode     FCPCode;
	UFC::AnsiString	FSettleMonth;
	int             FErrorCode;
	UFC::AnsiString FErrorMsg;
public:
    ErrorMessage( const UFC::AnsiString& Exchange, Market mkt );
    ErrorMessage( const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol );
    UFC::AnsiString& GetProductID( void ) { return FProductID; }
    double GetStrikePx( void ) { return FStrikePx; }
	CallPutCode GetCallPut( void ) { return FCPCode; }
	const UFC::AnsiString& GetSettleMonth( void ) { return FSettleMonth; }
    int GetErrorCode( void ) { return FErrorCode; }
	UFC::AnsiString& GetErrorMsg( void ) { return FErrorMsg; }

    void SetProductID( const UFC::AnsiString& ProductID ) { FProductID = ProductID; }
    void SetStrikePrice( double Price ) { FStrikePx = Price; }
    void SetCallPut( CallPutCode CPCode ) { FCPCode = CPCode; }
    void SetSettleMonth( const UFC::AnsiString& SettleMonth ) { FSettleMonth = SettleMonth; }
    void SetErrorCode( int ErrorCode ) { FErrorCode = ErrorCode; }
    void SetErrorMsg( const UFC::AnsiString& ErrorMsg ) { FErrorMsg = ErrorMsg; }
};
//---------------------------------------------------------------------------
// Advanced Msg
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class AdvancedMessage : public  BaseMessage
{
protected:
	Int32			FFunctionCode;
	UFC::PStream*   FAdvancedData;
public:
	AdvancedMessage(const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol, UFC::PStream* Data, MTree* TreeData);
	Int32 GetFunctionCode() { return FFunctionCode; }
	UFC::PStream* GetAdvancedData(){return FAdvancedData;}
};
//// class  TAIFEXBasicMessage
////---------------------------------------------------------------------------
//class TAIFEXBasicMessage : public  BaseMessage 
//{
//protected:
//	Int32			FFunctionCode;
//public:
//	TAIFEXBasicMessage(const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol);
//};
////---------------------------------------------------------------------------
//// class  TAIFEXSystemMessage
////---------------------------------------------------------------------------
//class TAIFEXSystemMessage : public  BaseMessage
//{
//protected:
//	Int32			FFunctionCode;
//public:
//	TAIFEXSystemMessage(const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString & Symbol);
//};
////---------------------------------------------------------------------------
//// class  TWSEBasicMessage
////---------------------------------------------------------------------------
//class TWSEBasicMessage : public  BaseMessage
//{
//protected:
//	Int32			FFunctionCode;
//public:
//	TWSEBasicMessage(const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol);
//};
////---------------------------------------------------------------------------
//// class  TWSEOrderStatsMessage
////---------------------------------------------------------------------------
//class TWSEOrderStatsMessage : public  BaseMessage
//{
//protected:
//	Int32			FFunctionCode;
//public:
//	TWSEOrderStatsMessage(const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol);
//};
////---------------------------------------------------------------------------
//// class  TWSETradeStatsMessage
////---------------------------------------------------------------------------
//class TWSETradeStatsMessage : public  BaseMessage
//{
//protected:
//	Int32			FFunctionCode;
//public:
//	TWSETradeStatsMessage(const UFC::AnsiString& Exchange, Market mkt, const UFC::AnsiString& Symbol);
//};
////---------------------------------------------------------------------------
#endif
