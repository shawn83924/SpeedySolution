//---------------------------------------------------------------------------
#include "TMarketDataConnection.h"
//---------------------------------------------------------------------------
const UFC::AnsiString	SUBJECT_SECURITY_DEFINITION_REQUEST = "STARWAVE.SD.REQUEST";
const UFC::AnsiString	SUBJECT_SECURITY_DEFINITION = "STARWAVE.SD";
const UFC::AnsiString	SUBJECT_SUBSCRIBE = "STARWAVE.SUBSCRIBE";
const UFC::AnsiString	SUBJECT_NEWS = "STARWAVE.NEWS";
const UFC::AnsiString	SUBJECT_QUOTE_REQUEST = "STARWAVE.QUOTE.REQUEST";
//---------------------------------------------------------------------------
TMarketDataConnection::TMarketDataConnection( HINSTANCE AppInstance, const char* AppName, TMarketDataConnectionEventListener* TMarketDataConnectionEventListener ):
FAppName( AppName ),
FListener( TMarketDataConnectionEventListener ),
FInstance( AppInstance )
{
	FTransport = new TMdTransport( AppInstance, this, FAppName, TRUE );

	FQRListener = new TMdListener( FTransport, this, "all", SUBJECT_QUOTE_REQUEST );
	FSDListener = new TMdListener( FTransport, this, SUBJECT_SECURITY_DEFINITION, "all" );
	FNewsListener = new TMdListener( FTransport, this, "all", SUBJECT_NEWS );
}
//---------------------------------------------------------------------------
TMarketDataConnection::~TMarketDataConnection( void )
{
	Disconnect();
	delete FTransport;
	delete FQRListener;
	delete FSDListener;
	delete FNewsListener;
}
//---------------------------------------------------------------------------
void TMarketDataConnection::Connect( const char* RemoteIP, long RemotePort )
{
	FTransport->Connect( RemoteIP, RemotePort );
}
//---------------------------------------------------------------------------
void TMarketDataConnection::Disconnect( void )
{
	FTransport->Disconnect();
}
//---------------------------------------------------------------------------
void TMarketDataConnection::OnMAppConnected( void )
{
	if( FListener != NULL )
	{
		UFC::SleepMS( 330 );
		FListener->OnConnected();
	}
}
//---------------------------------------------------------------------------
void TMarketDataConnection::OnMAppDisconnected( void )
{
	if( FListener != NULL )
		FListener->OnDisconnected();
}
//---------------------------------------------------------------------------
void TMarketDataConnection::OnMAppError( PMAppError )
{
	if( !(FTransport->IsConnected()) )
		if( FListener != NULL )
			FListener->OnDisconnected();
}
//---------------------------------------------------------------------------
void TMarketDataConnection::OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* pTree )
{
	//UFC::BufferedLog::Printf( " OnMigoMessage :Subject = %s, Key = %s", Subject.c_str(), Key.c_str() );
	if( FListener != NULL )
	{
		if( Subject == SUBJECT_SECURITY_DEFINITION )
		{
			ReceivedSecurityDefinition( pTree );
		}
		else if( Subject == SUBJECT_SUBSCRIBE )
		{
			ReceivedMarketData( pTree );
		}
		else if( Key == SUBJECT_QUOTE_REQUEST )
		{
			ReceivedQuoteRequest( pTree );
		}
		else if( Key == SUBJECT_NEWS )
		{
			ReceivedNews( pTree );
		}
		else if( FRegister.GetObjectByKey( Key ) != NULL )
		{
			ReceivedMarketData( pTree );
		}
	}
}
//---------------------------------------------------------------------------
void TMarketDataConnection::Subscribe( const char* Market, const char* SecurityID )
{
	//UFC::BufferedLog::Printf( " Subscribe %s:%s", Market, SecurityID );
	//fflush(stdout);
	AddToRegister( SecurityID, new TMdListener( FTransport, this, Market, SecurityID ) );

	TMdMessage Message;
	Message.SetStringValue( "Market", Market );
	Message.SetStringValue( "SecurityID", SecurityID );

	FTransport->Send( SUBJECT_SUBSCRIBE, UFC::Hostname, &Message );
}
//---------------------------------------------------------------------------
void TMarketDataConnection::UnSubscribe( const char*, const char* SecurityID )
{
	RemoveFromRegister( SecurityID );
}
//---------------------------------------------------------------------------
void TMarketDataConnection::AddToRegister( const char* Key, TMdListener* Listener )
{
	UFC::PLockObject	Lock( FCS );

	if( FRegister.GetObjectByKey( Key ) == NULL )
		FRegister.Add( Key, Listener );
}
//---------------------------------------------------------------------------
void TMarketDataConnection::RemoveFromRegister( const char* Key )
{
	UFC::PLockObject	Lock( FCS );

	TMdListener* ListenerPtr;
	if( ( ListenerPtr = FRegister.GetObjectByKey( Key )) != NULL )
	{
		delete ListenerPtr;
		FRegister.DeleteByKey( Key );
	}
}
//---------------------------------------------------------------------------
void TMarketDataConnection::ReceivedMarketData( MTree* pTree )
{
	TMarketDataMessage MarketDataMessage;
	int Depth;
    UFC::AnsiString	Market;
	UFC::AnsiString SecurityID;
	UFC::AnsiString	Symbol;

	if( pTree->get("Depth", Depth ) && pTree->get("Market", Market ) && pTree->get( "SecurityID", SecurityID) )
	{
		MarketDataMessage.SetMarketDepth( Depth );
		MarketDataMessage.SetMarket( Market.c_str());
		MarketDataMessage.SetSecurityID( SecurityID.c_str() );

		UFC::AnsiString	FieldName;

		int				IntValue;
		double			DoubleValue;

		for( register int i = 1; i <= Depth; i++  )
		{
			FieldName.Printf( "BidPx%d", i );

			if( pTree->get( FieldName, DoubleValue ) )
				MarketDataMessage.SetBidPrice( i, DoubleValue );

			FieldName.Printf( "OfferPx%d", i );

			if( pTree->get( FieldName, DoubleValue ))
				MarketDataMessage.SetOfferPrice( i, DoubleValue );

			FieldName.Printf( "BidSize%d", i );

			if( pTree->get( FieldName, IntValue ))
				MarketDataMessage.SetBidSize( i, IntValue );

			FieldName.Printf( "OfferSize%d", i );

			if( pTree->get( FieldName, IntValue ))
				MarketDataMessage.SetOfferSize( i, IntValue );
		}

		if( pTree->get( "Symbol", Symbol ))
			MarketDataMessage.SetSymbol( Symbol );

		if( pTree->get( "TradePx", DoubleValue ))
			MarketDataMessage.SetTradePrice( DoubleValue );

		if( pTree->get("TradeSize", IntValue))
			MarketDataMessage.SetTradeSize(IntValue);

		if( pTree->get("OpeningPrice", DoubleValue ))
			MarketDataMessage.SetOpeningPrice( DoubleValue );

		if( pTree->get("ClosingPrice", DoubleValue ))
			MarketDataMessage.SetClosingPrice( DoubleValue );

		if( pTree->get("SettlementPrice", DoubleValue ))
			MarketDataMessage.SetSettlementPrice( DoubleValue );

		if( pTree->get("TradingSessionHighPrice", DoubleValue ))
			MarketDataMessage.SetTradingSessionHighPrice( DoubleValue );

		if( pTree->get("TradingSessionLowPrice", DoubleValue ))
			MarketDataMessage.SetTradingSessionLowPrice( DoubleValue );

		if( pTree->get("TradeVolume", IntValue))
			MarketDataMessage.SetTradeVolume(IntValue);

		if( pTree->get("OpenInterest", IntValue))
			MarketDataMessage.SetOpenInterest(IntValue);

		if( pTree->get("RefPrice", DoubleValue))
			MarketDataMessage.SetRefPrice(DoubleValue);

		if( pTree->get("RiseLimitPrice", DoubleValue))
			MarketDataMessage.SetRiseLimitPrice(DoubleValue);

		if( pTree->get("FallLimitPrice", DoubleValue))
			MarketDataMessage.SetFallLimitPrice(DoubleValue);

		if( pTree->get("StrikePrice", IntValue))
			MarketDataMessage.SetStrikePrice(IntValue);

		if( FListener != NULL )
			FListener->OnMarketDataMessage( &MarketDataMessage );
	}
}
//---------------------------------------------------------------------------
void TMarketDataConnection::SecurityDefinitionRequest( TSecurityDefinitionRequestMessage* Msg )
{
	UFC::AnsiString	RequestMarket;

	RequestMarket = Msg->GetRequestMarket();
	UFC::BufferedLog::Printf(" Length = [%d]", RequestMarket.Length());
	if( RequestMarket.Length() > 0 )
	{
		TMdMessage	Message;
		Message.SetStringValue( "RequestMarket", RequestMarket.c_str() );

		if( FTransport != NULL )
		{
			FTransport->Send( SUBJECT_SECURITY_DEFINITION_REQUEST, RequestMarket, &Message );
		}
	}
	else
		UFC::BufferedLog::Printf(" RequestMarket.Length() == 0");
}
//---------------------------------------------------------------------------
void TMarketDataConnection::ReceivedQuoteRequest( MTree* pTree )
{
	UFC::AnsiString	Market;
	UFC::AnsiString	Symbol;
	UFC::AnsiString	SecurityID;
	TMDQuoteRequestMessage QRMessage;

	if( pTree->get("Market", Market) && pTree->get("SecurityID", SecurityID ) && pTree->get("Symbol", Symbol) )
	{
		QRMessage.SetMarket( Market.c_str() );
		QRMessage.SetSecurityID( SecurityID.c_str() );
		QRMessage.SetSymbol( Symbol.c_str() );

		if( FListener != NULL )
			FListener->OnQuoteRequest( &QRMessage );
	}
}
//---------------------------------------------------------------------------
void TMarketDataConnection::ReceivedSecurityDefinition( MTree* pTree )
{
	UFC::AnsiString	Market;
	UFC::AnsiString	SecurityID;
	UFC::AnsiString	Symbol;
	int MaturityDate;
	int	SymbolCount;
	TSecurityDefinitionMessage SDMessage;
	UFC::AnsiString	Count;
	double RefPrice;
	double RiseLimitPrice;
	double FallLimitPrice;
	int StrikePrice;

	if( pTree->get("Market", Market ) && pTree->get("Count", SymbolCount ) )
	{
		SDMessage.SetMarket( Market.c_str() );
		SDMessage.SetSymbolCount( SymbolCount );

		for( register int i = 0; i < SymbolCount; i++ )
		{
			Count.Printf( "SecurityID%d", i );
			if( pTree->get( Count, SecurityID ) )
			{
				SDMessage.SetSecurityID( SecurityID.c_str(), i );
			}
			Count.Printf( "Symbol%d", i );
			if( pTree->get( Count, Symbol) )
			{
				SDMessage.SetSymbol( Symbol.c_str(), i );
			}
			Count.Printf( "MaturityDate%d", i );
			if( pTree->get( Count, MaturityDate ) )
			{
				SDMessage.SetMaturityDate( MaturityDate, i );
			}
			Count.Printf( "RefPrice%d", i );
			if( pTree->get( Count, RefPrice ))
			{
				SDMessage.SetRefPrice( RefPrice, i );
			}
			//UFC::BufferedLog::Printf( " SecurityID:[%s] RefPrice = [%f]", SecurityID.c_str(), RefPrice );
			Count.Printf( "RiseLimitPrice%d", i );
			if( pTree->get( Count, RiseLimitPrice ))
			{
				SDMessage.SetRiseLimitPrice( RiseLimitPrice, i );
			}
			Count.Printf( "FallLimitPrice%d", i );
			if( pTree->get( Count, FallLimitPrice ))
			{
				SDMessage.SetFallLimitPrice( FallLimitPrice , i );
			}
			Count.Printf( "StrikePrice%d", i );
			if( pTree->get( Count, StrikePrice ))
			{
				SDMessage.SetStrikePrice( StrikePrice, i );
			}
		}
		if( FListener != NULL )
			FListener->OnSecurityDefinition( &SDMessage );
	}
}
//---------------------------------------------------------------------------
void TMarketDataConnection::ReceivedNews( MTree* pTree )
{
	UFC::AnsiString	Market;
	UFC::AnsiString	Headline;
	UFC::AnsiString Text;
	TNewsMessage	News;

	if( FListener != NULL )
	{
		if( pTree->get( "Market", Market ))
			News.SetMarket( Market.c_str() );

		if( pTree->get( "148", Headline ))
			News.SetHeadline( Headline.c_str() );

		if( pTree->get( "58", Text ))
			News.SetText( Text.c_str() );
		UFC::BufferedLog::Printf(" OnNews: Market:[%s], Headline:[%s], Text:[%s]", Market.c_str(), Headline.c_str(), Text.c_str());
		FListener->OnNews( &News );
	}
}
//---------------------------------------------------------------------------

