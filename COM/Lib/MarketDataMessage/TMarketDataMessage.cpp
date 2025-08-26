//---------------------------------------------------------------------------
#include "TMarketDataMessage.h"
//---------------------------------------------------------------------------
TMarketDataMessage::TMarketDataMessage(void)
{}
//---------------------------------------------------------------------------
TMarketDataMessage::~TMarketDataMessage(void)
{}
//---------------------------------------------------------------------------
double	TMarketDataMessage::GetBidPrice( int Depth )
{
	MString FieldName;
	FieldName.Printf( "BidPx%d", Depth );
	return GetDoubleValue( FieldName.c_str(), 0 );
}
//---------------------------------------------------------------------------
int     TMarketDataMessage::GetBidSize( int Depth )
{
	MString FieldName;
	FieldName.Printf( "BidSize%d", Depth );
	return GetIntegerValue( FieldName.c_str(), 0 );
}
//---------------------------------------------------------------------------
double	TMarketDataMessage::GetOfferPrice( int Depth )
{
	MString FieldName;
	FieldName.Printf( "OfferPx%d", Depth );
	return GetDoubleValue( FieldName.c_str(), 0 );
}
//---------------------------------------------------------------------------
int		TMarketDataMessage::GetOfferSize( int Depth )
{
	MString FieldName;
	FieldName.Printf( "OfferSize%d", Depth );
	return GetIntegerValue( FieldName.c_str(), 0 );
}
//---------------------------------------------------------------------------
double	TMarketDataMessage::GetTradePrice( void )
{
	return GetDoubleValue( "TradePx", 0 );
}
//---------------------------------------------------------------------------
double	TMarketDataMessage::GetTradeSize( void )
{
	return GetIntegerValue( "TradeSize", 0 );
}
//---------------------------------------------------------------------------
double	TMarketDataMessage::GetOpeningPrice( void )
{
	return GetDoubleValue( "OpeningPrice", 0 );
}
//---------------------------------------------------------------------------
double	TMarketDataMessage::GetClosingPrice( void )
{
	return GetDoubleValue( "ClosingPrice", 0 );
}
//---------------------------------------------------------------------------
double	TMarketDataMessage::GetSettlementPrice( void )
{
	return GetDoubleValue( "SettlementPrice", 0 );
}
//---------------------------------------------------------------------------
double	TMarketDataMessage::GetTradingSessionHighPrice( void )
{
	return GetDoubleValue( "TradingSessionHighPrice", 0 );
}
//---------------------------------------------------------------------------
double	TMarketDataMessage::GetTradingSessionLowPrice( void )
{
	return GetDoubleValue( "TradingSessionLowPrice", 0 );
}
//---------------------------------------------------------------------------
int		TMarketDataMessage::GetTradeVolume( void )
{
	return GetIntegerValue( "TradeVolume", 0 );
}
//---------------------------------------------------------------------------
int		TMarketDataMessage::GetOpenInterest( void )
{
	return GetIntegerValue( "OpenInterest", 0 );
}
//---------------------------------------------------------------------------
double TMarketDataMessage::GetRefPrice( void )
{
	return GetDoubleValue( "RefPrice", 0.0 );
}
//---------------------------------------------------------------------------
int TMarketDataMessage::GetMarketDepth( void )
{
	return GetIntegerValue( "Depth", 0 );
}
//---------------------------------------------------------------------------
double TMarketDataMessage::GetRiseLimitPrice( void )
{
	return GetDoubleValue( "RiseLimitPrice", 0.0 );
}
//---------------------------------------------------------------------------
double TMarketDataMessage::GetFallLimitPrice( void )
{
	return GetDoubleValue( "FallLimitPrice", 0.0 );
}
//---------------------------------------------------------------------------
int TMarketDataMessage::GetStrikePrice( void )
{
	return GetIntegerValue( "StrikePrice", 0 );
}
//---------------------------------------------------------------------------
void TMarketDataMessage::SetMarketDepth( int Depth )
{
	SetIntegerValue( "Depth", Depth );
}
//---------------------------------------------------------------------------
void TMarketDataMessage::SetRefPrice( double Price )
{
	SetDoubleValue( "RefPrice", Price );
}
//---------------------------------------------------------------------------
void	TMarketDataMessage::SetBidPrice( int Depth, double Price )
{
	MString FieldName;
	FieldName.Printf( "BidPx%d", Depth );
	SetDoubleValue( FieldName.c_str(), Price );
}
//---------------------------------------------------------------------------
void     TMarketDataMessage::SetBidSize( int Depth, int Size )
{
	MString FieldName;
	FieldName.Printf( "BidSize%d", Depth );
	SetIntegerValue( FieldName.c_str(), Size );
}
//---------------------------------------------------------------------------
void	TMarketDataMessage::SetOfferPrice( int Depth, double Price )
{
	MString FieldName;
	FieldName.Printf( "OfferPx%d", Depth );
	SetDoubleValue( FieldName.c_str(), Price );
}
//---------------------------------------------------------------------------
void		TMarketDataMessage::SetOfferSize( int Depth, int Size )
{
	MString FieldName;
	FieldName.Printf( "OfferSize%d", Depth );
	SetIntegerValue( FieldName.c_str(), Size );
}
//---------------------------------------------------------------------------
void	TMarketDataMessage::SetTradePrice( double Price )
{
	SetDoubleValue( "TradePx", Price );
}
//---------------------------------------------------------------------------
void	TMarketDataMessage::SetTradeSize( int Size )
{
	SetIntegerValue( "TradeSize", Size );
}
//---------------------------------------------------------------------------
void	TMarketDataMessage::SetOpeningPrice( double Price )
{
	SetDoubleValue( "OpeningPrice", Price );
}
//---------------------------------------------------------------------------
void	TMarketDataMessage::SetClosingPrice( double Price )
{
	SetDoubleValue( "ClosingPrice", Price );
}
//---------------------------------------------------------------------------
void	TMarketDataMessage::SetSettlementPrice( double Price )
{
	SetDoubleValue( "SettlementPrice", Price );
}
//---------------------------------------------------------------------------
void	TMarketDataMessage::SetTradingSessionHighPrice( double Price )
{
	SetDoubleValue( "TradingSessionHighPrice", Price );
}
//---------------------------------------------------------------------------
void	TMarketDataMessage::SetTradingSessionLowPrice( double Price )
{
	SetDoubleValue( "TradingSessionLowPrice", Price );
}
//---------------------------------------------------------------------------
void		TMarketDataMessage::SetTradeVolume( int Size )
{
	SetIntegerValue( "TradeVolume", Size );
}
//---------------------------------------------------------------------------
void		TMarketDataMessage::SetOpenInterest( int Size )
{
	SetIntegerValue( "OpenInterest", Size );
}
//---------------------------------------------------------------------------
const char* TMarketDataMessage::GetSymbol( void )
{
	return GetStringValue( "Symbol" );
}
//---------------------------------------------------------------------------
void TMarketDataMessage::SetSymbol( const char* Symbol )
{
	SetStringValue( "Symbol", Symbol );
}
//---------------------------------------------------------------------------
const char* TMarketDataMessage::GetSecurityID( void )
{
	return GetStringValue( "SecurityID" );
}
//---------------------------------------------------------------------------
void TMarketDataMessage::SetSecurityID( const char* SecurityID )
{
	SetStringValue( "SecurityID", SecurityID );
}
//---------------------------------------------------------------------------
int TMarketDataMessage::GetMaturityDate( void )
{
	return GetIntegerValue( "MaturityDate", 0 );
}
//---------------------------------------------------------------------------
void TMarketDataMessage::SetMaturityDate( int Date )
{
	SetIntegerValue( "MaturityDate", Date );
}
//---------------------------------------------------------------------------
void TMarketDataMessage::SetRiseLimitPrice( double Price )
{
	SetDoubleValue( "RiseLimitPrice", Price );
}
//---------------------------------------------------------------------------
void TMarketDataMessage::SetFallLimitPrice( double Price )
{
	SetDoubleValue( "FallLimitPrice", Price );
}
//---------------------------------------------------------------------------
void TMarketDataMessage::SetStrikePrice( int Price )
{
	SetIntegerValue( "StrikePrice", Price );
}
//---------------------------------------------------------------------------
