#include "TCancelOrderMessage.h"
//---------------------------------------------------------------------------
TCancelOrderMessage::TCancelOrderMessage( void )
:TBaseMessage( )
,FMarketDataSource( "999" )
,FExchangeCode( "TAIFEX" )
,FTSEOrderType( "0" )
,FTSEExchangeCode( "0" )
,FTSETradeKind( "2" )
,FTSEObjectBroker( "0000" )
,FPrice( 0.0 )
,FOrderQty( 0 )
,FStockSeqNo( 0 )
,FCNID( 0 )
,FOrderType( nsOrderMessageDefine::otLimit )
,FSide( nsOrderMessageDefine::sNone )
,FTimeInForce( nsOrderMessageDefine::tifNone )
,FOrderSource( nsOrderMessageDefine::osDedicatedLine )
,FTradingSessionID( nsOrderMessageDefine::tsAuto )
{
	SetMessageType( nsOrderMessageDefine::mtCancel );
}
//---------------------------------------------------------------------------
TCancelOrderMessage::~TCancelOrderMessage(void)
{
}
//---------------------------------------------------------------------------
void TCancelOrderMessage::SetMarketDataSource( const char* Value )
{
    FMarketDataSource = Value;
    FMarketDataSource.PadThis( 3, '9' );
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::TradingSessionIDEnum TCancelOrderMessage::GetTradingSessionID( void )
{
	if( GetMarket( ) == nsOrderMessageDefine::mTWFutures || GetMarket( ) == nsOrderMessageDefine::mTWOptions )
	{
		switch( FTradingSessionID )
		{
			case nsOrderMessageDefine::tsOffHour: return nsOrderMessageDefine::tsOffHour;
			case nsOrderMessageDefine::tsNormal:  return nsOrderMessageDefine::tsNormal;
			case nsOrderMessageDefine::tsAuto:
			default:                              return nsOrderMessageDefine::tsAuto;
		}
	}
	else
	{
		if( FTradingSessionID == nsOrderMessageDefine::tsAuto )
			return nsOrderMessageDefine::tsNormal;
		return FTradingSessionID;
	}
}
//---------------------------------------------------------------------------
