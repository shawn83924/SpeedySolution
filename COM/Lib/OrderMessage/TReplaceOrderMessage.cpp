#include "TReplaceOrderMessage.h"
//---------------------------------------------------------------------------
TReplaceOrderMessage::TReplaceOrderMessage( void  )
:TBaseMessage( )
{
    SetMessageType( nsOrderMessageDefine::mtReplace );
}
//---------------------------------------------------------------------------
TReplaceOrderMessage::~TReplaceOrderMessage(void)
{
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetMaturityMonthYear( const char* Value )
{
    SetStringValue( "200", Value );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetOrigClOrdID( const char* Value )
{
    SetStringValue( "41", Value );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetOrderType( nsOrderMessageDefine::OrderTypeEnum Value )
{
    SetIntegerValue( "40", Value );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetSide( nsOrderMessageDefine::SideEnum Value )
{
    SetIntegerValue( "54", Value );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetTimeInForce( nsOrderMessageDefine::TimeInForceEnum Value )
{
    SetIntegerValue( "59", Value );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetPositionEffect( nsOrderMessageDefine::PositionEffectEnum Value )
{
    SetIntegerValue( "77", Value );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetOrderQty( long Value )
{
    SetIntegerValue( "38", Value );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetPrice( double Value )
{
    SetDoubleValue( "44", Value );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetStopPx( double Value )
{
    SetDoubleValue( "99", Value );
}
//---------------------------------------------------------------------------
const char* TReplaceOrderMessage::GetMaturityMonthYear( void )
{
    return GetStringValue( "200" );
}
//---------------------------------------------------------------------------
const char* TReplaceOrderMessage::GetOrigClOrdID( void )
{
    return GetStringValue( "41" );
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::OrderTypeEnum TReplaceOrderMessage::GetOrderType( void )
{
    return (nsOrderMessageDefine::OrderTypeEnum)( GetIntegerValue( "40", 0 ) );
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::SideEnum TReplaceOrderMessage::GetSide( void )
{
    return (nsOrderMessageDefine::SideEnum)( GetIntegerValue( "54", 0 ) );
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::TimeInForceEnum TReplaceOrderMessage::GetTimeInForce( void )
{
    return (nsOrderMessageDefine::TimeInForceEnum)( GetIntegerValue( "59", 0 ) );
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::PositionEffectEnum TReplaceOrderMessage::GetPositionEffect( void )
{
    return (nsOrderMessageDefine::PositionEffectEnum)( GetIntegerValue( "77", 0 ) );
}
//---------------------------------------------------------------------------
long TReplaceOrderMessage::GetOrderQty( void )
{
    return GetIntegerValue( "38", 0 );
}
//---------------------------------------------------------------------------
double TReplaceOrderMessage::GetPrice( void )
{
    return GetDoubleValue( "44", 0.0 );
}
//---------------------------------------------------------------------------
double TReplaceOrderMessage::GetStopPx( void )
{
	return GetDoubleValue( "99", 0.0 );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetTSEOrderType( const char* Value )
{
    SetStringValue( "20001", Value );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetTSEExchangeCode( const char* Value )
{
    SetStringValue( "20002", Value );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetExchangeCode( const char* Value )
{
    SetStringValue( "ExC", Value );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetTSETradeKind( const char* Value )
{
    SetStringValue( "20003", Value );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetTSEObjectBroker(  const char* Value )
{
    SetStringValue( "20004", Value );
}
//---------------------------------------------------------------------------
const char* TReplaceOrderMessage::GetTSEOrderType( void )
{
    return GetStringValue( "20001", "0" ); ///< 0 means normal order
}
//---------------------------------------------------------------------------
const char* TReplaceOrderMessage::GetTSEExchangeCode( void )
{
    return GetStringValue( "20002", "0" );
}
//---------------------------------------------------------------------------
const char* TReplaceOrderMessage::GetExchangeCode( void )
{
    return GetStringValue( "ExC", "TAIFEX" );
}
//---------------------------------------------------------------------------
const char* TReplaceOrderMessage::GetTSETradeKind( void )
{
    return GetStringValue( "20003", "2" ); ///< 2 means normal, 1 means mistake
}
//---------------------------------------------------------------------------
const char* TReplaceOrderMessage::GetTSEObjectBroker( void )
{
    return GetStringValue( "20004", "0000" );
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::TradingSessionIDEnum TReplaceOrderMessage::GetTradingSessionID( void )
{
    if( GetMarket( ) == nsOrderMessageDefine::mTWFutures || GetMarket( ) == nsOrderMessageDefine::mTWOptions )
        return (nsOrderMessageDefine::TradingSessionIDEnum)( GetIntegerValue( "336", nsOrderMessageDefine::tsAuto ) );
    else
        return (nsOrderMessageDefine::TradingSessionIDEnum)( GetIntegerValue( "336", nsOrderMessageDefine::tsNormal ) );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetTradingSessionID( nsOrderMessageDefine::TradingSessionIDEnum Value )
{
    SetIntegerValue( "336", Value );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetStockSeqNo( long Value )
{
    SetIntegerValue( "EO", Value );
}
//---------------------------------------------------------------------------
long TReplaceOrderMessage::GetStockSeqNo( void )
{
    return GetIntegerValue( "EO", 0 );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetKeep(  const char* Value )
{
    SetStringValue( "EN", Value );
}
//---------------------------------------------------------------------------
const char* TReplaceOrderMessage::GetKeep( void )
{
    return GetStringValue( "EN", "" );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetSrc(  const char* Value )
{
    SetStringValue( "Src", Value );
}
//---------------------------------------------------------------------------
const char* TReplaceOrderMessage::GetSrc( void )
{
    return GetStringValue( "Src", "" );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetOrderSource( nsOrderMessageDefine::OrderSourceEnum Value )
{
    SetIntegerValue( "ordsrc", (long)Value  );
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::OrderSourceEnum  TReplaceOrderMessage::GetOrderSource( void )
{
    return (nsOrderMessageDefine::OrderSourceEnum)( GetIntegerValue( "ordsrc", 0 ) );
}
//---------------------------------------------------------------------------
const char* TReplaceOrderMessage::GetMarketDataSource( void )
{
    return GetStringValue( "mdsrc", "999" );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetMarketDataSource( const char* Value )
{
    UFC::AnsiString mdsrc( Value );

    mdsrc.PadThis( 3, '9' );
    SetStringValue( "mdsrc", mdsrc.c_str() );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetIBNO( const char* Value )
{
    SetStringValue( "ibno", Value );
}
//---------------------------------------------------------------------------
const char* TReplaceOrderMessage::GetIBNO( void )
{
    return GetStringValue( "ibno", "" );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetEUCD( const char* Value )
{
    SetStringValue( "eucd", Value );
}
//---------------------------------------------------------------------------
const char* TReplaceOrderMessage::GetEUCD( void )
{
    return GetStringValue( "eucd", "" );
}
//---------------------------------------------------------------------------
void TReplaceOrderMessage::SetExpireDate( const char* Value )
{
    SetStringValue( "432", Value );
}
//---------------------------------------------------------------------------
const char* TReplaceOrderMessage::GetExpireDate( void )
{
    return GetStringValue( "432", "" );
}
//---------------------------------------------------------------------------
