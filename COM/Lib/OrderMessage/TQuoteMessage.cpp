#include "TQuoteMessage.h"
//---------------------------------------------------------------------------
TQuoteMessage::TQuoteMessage( void )
:TBaseMessage()
{
    SetMessageType( nsOrderMessageDefine::mtQuote );
}
//---------------------------------------------------------------------------
TQuoteMessage::~TQuoteMessage(void)
{
}
//---------------------------------------------------------------------------
void TQuoteMessage::SetTimeInForce( nsOrderMessageDefine::TimeInForceEnum Value )
{
	SetIntegerValue( "59", Value );
}
//---------------------------------------------------------------------------
void TQuoteMessage::SetBidQty( long Value )
{
    SetIntegerValue( "134", Value );
}
//---------------------------------------------------------------------------
void TQuoteMessage::SetAskQty( long Value )
{
    SetIntegerValue( "135", Value );
}
//---------------------------------------------------------------------------
void TQuoteMessage::SetBidPrice( double Value )
{
    SetDoubleValue( "132", Value );
}
//---------------------------------------------------------------------------
void TQuoteMessage::SetAskPrice( double Value )
{
    SetDoubleValue( "133", Value );
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::TimeInForceEnum TQuoteMessage::GetTimeInForce( void )
{
    return (nsOrderMessageDefine::TimeInForceEnum)( GetIntegerValue( "59", 0 ) );
}
//---------------------------------------------------------------------------
long TQuoteMessage::GetBidQty( void )
{
    return GetIntegerValue( "134", 0 );
}
//---------------------------------------------------------------------------
long TQuoteMessage::GetAskQty( void )
{
	return GetIntegerValue( "135", 0 );
}
//---------------------------------------------------------------------------
double TQuoteMessage::GetBidPrice( void )
{
    return GetDoubleValue( "132", 0.0 );
}
//---------------------------------------------------------------------------
double TQuoteMessage::GetAskPrice( void )
{
    return GetDoubleValue( "133", 0.0 );
}
//---------------------------------------------------------------------------
void TQuoteMessage::SetQuoteID( const char* Value )
{
    SetStringValue( "117", Value );
}
//---------------------------------------------------------------------------
const char* TQuoteMessage::GetQuoteID( void )
{
    return GetStringValue( "117", "00000" );
}
//---------------------------------------------------------------------------
void TQuoteMessage::SetOrderSource( nsOrderMessageDefine::OrderSourceEnum Value )
{
	SetIntegerValue( "ordsrc", (long)Value  );
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::OrderSourceEnum  TQuoteMessage::GetOrderSource( void )
{
	return (nsOrderMessageDefine::OrderSourceEnum)( GetIntegerValue( "ordsrc", 0 ) );
}
//---------------------------------------------------------------------------
const char* TQuoteMessage::GetMarketDataSource( void )
{
	return GetStringValue( "mdsrc", "999" );
}
//---------------------------------------------------------------------------
void TQuoteMessage::SetMarketDataSource( const char* Value )
{
	UFC::AnsiString mdsrc( Value );

	mdsrc.PadThis( 3, '9' );
	SetStringValue( "mdsrc", mdsrc.c_str() );
}
//---------------------------------------------------------------------------
void TQuoteMessage::SetTradingSessionID( nsOrderMessageDefine::TradingSessionIDEnum Value )
{
    SetIntegerValue( "336", Value );
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::TradingSessionIDEnum TQuoteMessage::GetTradingSessionID( void )
{
	if( GetMarket( ) == nsOrderMessageDefine::mTWFutures || GetMarket( ) == nsOrderMessageDefine::mTWOptions )
		return (nsOrderMessageDefine::TradingSessionIDEnum)( GetIntegerValue( "336", nsOrderMessageDefine::tsAuto ) );
	else
		return (nsOrderMessageDefine::TradingSessionIDEnum)( GetIntegerValue( "336", nsOrderMessageDefine::tsNormal ) );
}
//---------------------------------------------------------------------------

