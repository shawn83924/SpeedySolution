#include "TQuoteCancelMessage.h"
//---------------------------------------------------------------------------
TQuoteCancelMessage::TQuoteCancelMessage( void )
:TBaseMessage( )
{
    SetMessageType( nsOrderMessageDefine::mtQuoteCancel );
}
//---------------------------------------------------------------------------
TQuoteCancelMessage::~TQuoteCancelMessage(void)
{
}
//---------------------------------------------------------------------------
void TQuoteCancelMessage::SetQuoteID( const char* Value )
{
    SetStringValue( "117", Value );
}
//---------------------------------------------------------------------------
void TQuoteCancelMessage::SetBidPrice( double Value )
{
    SetDoubleValue( "132", Value );
}
//---------------------------------------------------------------------------
void TQuoteCancelMessage::SetAskPrice( double Value )
{
    SetDoubleValue( "133", Value );
}
//---------------------------------------------------------------------------
const char* TQuoteCancelMessage::GetQuoteID( void )
{
    return GetStringValue( "117" );
}
//---------------------------------------------------------------------------
double TQuoteCancelMessage::GetBidPrice( void )
{
    return GetDoubleValue( "132", 0.0 );
}
//---------------------------------------------------------------------------
double TQuoteCancelMessage::GetAskPrice( void )
{
    return GetDoubleValue( "133", 0.0 );
}
//---------------------------------------------------------------------------
void TQuoteCancelMessage::SetOrderSource( nsOrderMessageDefine::OrderSourceEnum Value )
{
	SetIntegerValue( "ordsrc", (long)Value  );
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::OrderSourceEnum  TQuoteCancelMessage::GetOrderSource( void )
{
	return (nsOrderMessageDefine::OrderSourceEnum)( GetIntegerValue( "ordsrc", 0 ) );
}
//---------------------------------------------------------------------------
const char* TQuoteCancelMessage::GetMarketDataSource( void )
{
	return GetStringValue( "mdsrc", "999" );
}
//---------------------------------------------------------------------------
void TQuoteCancelMessage::SetMarketDataSource( const char* Value )
{
	UFC::AnsiString mdsrc( Value );

	mdsrc.PadThis( 3, '9' );
	SetStringValue( "mdsrc", mdsrc.c_str() );
}
//---------------------------------------------------------------------------
void TQuoteCancelMessage::SetTradingSessionID( nsOrderMessageDefine::TradingSessionIDEnum Value )
{
    SetIntegerValue( "336", Value );
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::TradingSessionIDEnum TQuoteCancelMessage::GetTradingSessionID( void )
{
	if( GetMarket( ) == nsOrderMessageDefine::mTWFutures || GetMarket( ) == nsOrderMessageDefine::mTWOptions )
		return (nsOrderMessageDefine::TradingSessionIDEnum)( GetIntegerValue( "336", nsOrderMessageDefine::tsAuto ) );
	else
		return (nsOrderMessageDefine::TradingSessionIDEnum)( GetIntegerValue( "336", nsOrderMessageDefine::tsNormal ) );
}
//---------------------------------------------------------------------------


