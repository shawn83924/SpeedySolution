#include "TOrderStatusRequest.h"
//---------------------------------------------------------------------------
TOrderStatusRequest::TOrderStatusRequest( void )
:TBaseMessage()
{
    SetMessageType( nsOrderMessageDefine::mtOrderStatusRequest );
}
//---------------------------------------------------------------------------
TOrderStatusRequest::~TOrderStatusRequest( void )
{
}
//---------------------------------------------------------------------------
void TOrderStatusRequest::SetTSEOrderType( const char* Value )
{
    SetStringValue( "20001", Value );
}
//---------------------------------------------------------------------------
const char* TOrderStatusRequest::GetTSEOrderType( void )
{
    return GetStringValue( "20001", "0" ); ///< 0 means normal order
}
//---------------------------------------------------------------------------
void TOrderStatusRequest::SetPrice( double Value )
{
    SetDoubleValue( "44", Value );
}
//---------------------------------------------------------------------------
double TOrderStatusRequest::GetPrice( void )
{
    return GetDoubleValue( "44", 0.0 );
}
//---------------------------------------------------------------------------
void TOrderStatusRequest::SetSide( nsOrderMessageDefine::SideEnum Value )
{
    SetIntegerValue( "54", Value );
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::SideEnum TOrderStatusRequest::GetSide( void )
{
    return (nsOrderMessageDefine::SideEnum)( GetIntegerValue( "54", 0 ) );
}
//---------------------------------------------------------------------------
void TOrderStatusRequest::SetKeep(  const char* Value )
{
    SetStringValue( "EN", Value );
}
//---------------------------------------------------------------------------
const char* TOrderStatusRequest::GetKeep( void )
{
    return GetStringValue( "EN", "" );
}
//---------------------------------------------------------------------------
void TOrderStatusRequest::SetStockSeqNo( long Value )
{
    SetIntegerValue( "EO", Value );
}
//---------------------------------------------------------------------------
long TOrderStatusRequest::GetStockSeqNo( void )
{
    return GetIntegerValue( "EO", 0 );
}
//---------------------------------------------------------------------------
void TOrderStatusRequest::SetTradingSessionID( nsOrderMessageDefine::TradingSessionIDEnum Value )
{
    SetIntegerValue( "336", Value );
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::TradingSessionIDEnum TOrderStatusRequest::GetTradingSessionID( void )
{
	if( GetMarket( ) == nsOrderMessageDefine::mTWFutures || GetMarket( ) == nsOrderMessageDefine::mTWOptions )
		return (nsOrderMessageDefine::TradingSessionIDEnum)( GetIntegerValue( "336", nsOrderMessageDefine::tsAuto ) );
	else
		return (nsOrderMessageDefine::TradingSessionIDEnum)( GetIntegerValue( "336", nsOrderMessageDefine::tsNormal ) );
}
//---------------------------------------------------------------------------
void TOrderStatusRequest::SetTSETradeKind( const char* Value )
{
    SetStringValue( "20003", Value );
}
//---------------------------------------------------------------------------
const char* TOrderStatusRequest::GetTSETradeKind( void )
{
    return GetStringValue( "20003", "2" ); ///< 2 means normal, 1 means mistake
}
//---------------------------------------------------------------------------
void TOrderStatusRequest::SetTSEObjectBroker(  const char* Value )
{
    SetStringValue( "20004", Value );
}
//---------------------------------------------------------------------------
const char* TOrderStatusRequest::GetTSEObjectBroker( void )
{
    return GetStringValue( "20004", "0000" );
}
//---------------------------------------------------------------------------
void TOrderStatusRequest::SetTSEExchangeCode( const char* Value )
{
	SetStringValue( "20002", Value );
}
//---------------------------------------------------------------------------
void TOrderStatusRequest::SetExchangeCode( const char* Value )
{
	SetStringValue( "ExC", Value );
}
//---------------------------------------------------------------------------
const char* TOrderStatusRequest::GetTSEExchangeCode( void )
{
	return GetStringValue( "20002", "0" );
}
//---------------------------------------------------------------------------
const char* TOrderStatusRequest::GetExchangeCode( void )
{
	return GetStringValue( "ExC", "TAIFEX" );
}
//---------------------------------------------------------------------------
void TOrderStatusRequest::SetSrc(  const char* Value )
{
	SetStringValue( "SRC", Value );
}
//---------------------------------------------------------------------------
const char* TOrderStatusRequest::GetSrc( void )
{
	return GetStringValue( "SRC" );
}
//---------------------------------------------------------------------------
void TOrderStatusRequest::SetOrigNID( long Value )
{
    SetIntegerValue( "N41", Value );
}
//---------------------------------------------------------------------------
long TOrderStatusRequest::GetOrigNID( void )
{
    return GetIntegerValue( "N41", 0 );
}
//---------------------------------------------------------------------------
