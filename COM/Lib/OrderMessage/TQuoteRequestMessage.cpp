#include "TQuoteRequestMessage.h"
//---------------------------------------------------------------------------
TQuoteRequestMessage::TQuoteRequestMessage( void )
:TBaseMessage( )
{
    SetMessageType( nsOrderMessageDefine::mtQuoteRequest );
}
//---------------------------------------------------------------------------
TQuoteRequestMessage::~TQuoteRequestMessage(void)
{
}
//---------------------------------------------------------------------------
void TQuoteRequestMessage::SetTradingSessionID( nsOrderMessageDefine::TradingSessionIDEnum Value )
{
    SetIntegerValue( "336", Value );
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::TradingSessionIDEnum TQuoteRequestMessage::GetTradingSessionID( void )
{
	if( GetMarket( ) == nsOrderMessageDefine::mTWFutures || GetMarket( ) == nsOrderMessageDefine::mTWOptions )
		return (nsOrderMessageDefine::TradingSessionIDEnum)( GetIntegerValue( "336", nsOrderMessageDefine::tsAuto ) );
	else
		return (nsOrderMessageDefine::TradingSessionIDEnum)( GetIntegerValue( "336", nsOrderMessageDefine::tsNormal ) );
}
//---------------------------------------------------------------------------

