//---------------------------------------------------------------------------
#include "TSecurityDefinition.h"
//---------------------------------------------------------------------------
TSecurityDefinitionMessage::TSecurityDefinitionMessage( void )
{}
//---------------------------------------------------------------------------
TSecurityDefinitionMessage::~TSecurityDefinitionMessage( void )
{}
//---------------------------------------------------------------------------
void TSecurityDefinitionMessage::SetSymbol( const char* Symbol, int Index )
{
	UFC::AnsiString	ItemCount;
	ItemCount.Printf( "Symbol%d", Index );
	SetStringValue( ItemCount.c_str(), Symbol );
}
//---------------------------------------------------------------------------
void TSecurityDefinitionMessage::SetSecurityID( const char* SecurityID, int Index )
{
	UFC::AnsiString	ItemCount;
	ItemCount.Printf( "SecurityID%d", Index );
	SetStringValue( ItemCount.c_str(), SecurityID );
}
//---------------------------------------------------------------------------
void TSecurityDefinitionMessage::SetMaturityDate( int MaturityDate, int Index )
{
	UFC::AnsiString	ItemCount;
	ItemCount.Printf( "MaturityDate%d", Index );
	SetIntegerValue( ItemCount.c_str(), MaturityDate );
}
//---------------------------------------------------------------------------
void TSecurityDefinitionMessage::SetRefPrice( double RefPrice, int Index )
{
	UFC::AnsiString ItemCount;
	ItemCount.Printf( "RefPrice%d", Index );
	SetDoubleValue( ItemCount.c_str(), RefPrice );
}
//---------------------------------------------------------------------------
void TSecurityDefinitionMessage::SetRiseLimitPrice( double RiseLimitPrice, int Index )
{
 	UFC::AnsiString ItemCount;
	ItemCount.Printf( "RiseLimitPrice%d", Index );
	SetDoubleValue( ItemCount.c_str(), RiseLimitPrice );
}
//---------------------------------------------------------------------------
void TSecurityDefinitionMessage::SetFallLimitPrice( double FallLimitPrice, int Index )
{
	UFC::AnsiString ItemCount;
	ItemCount.Printf( "FallLimitPrice%d", Index );
	SetDoubleValue( ItemCount.c_str(), FallLimitPrice );
}
//---------------------------------------------------------------------------
void TSecurityDefinitionMessage::SetStrikePrice( int StrikePrice, int Index )
{
	UFC::AnsiString ItemCount;
	ItemCount.Printf( "StrikePrice%d", Index );
	SetIntegerValue( ItemCount.c_str(), StrikePrice );
}
//---------------------------------------------------------------------------
const char* TSecurityDefinitionMessage::GetSymbol( int Index )
{
	UFC::AnsiString	ItemCount;
	ItemCount.Printf( "Symbol%d", Index );
	return GetStringValue( ItemCount );
}
//---------------------------------------------------------------------------
const char* TSecurityDefinitionMessage::GetSecurityID( int Index )
{
	UFC::AnsiString	ItemCount;
	ItemCount.Printf( "SecurityID%d", Index );
	return GetStringValue( ItemCount );
}
//---------------------------------------------------------------------------
int TSecurityDefinitionMessage::GetMaturityDate( int Index )
{
	UFC::AnsiString	ItemCount;
	ItemCount.Printf( "MaturityDate%d", Index );
	return GetIntegerValue( ItemCount, 0 );
}
//---------------------------------------------------------------------------
void TSecurityDefinitionMessage::SetSymbolCount( int Count )
{
	SetIntegerValue( "Count", Count );
}
//---------------------------------------------------------------------------
int TSecurityDefinitionMessage::GetSymbolCount( void )
{
	return GetIntegerValue( "Count", 0 );
}
//---------------------------------------------------------------------------
double TSecurityDefinitionMessage::GetRefPrice( int Index )
{
	UFC::AnsiString	ItemCount;
	ItemCount.Printf( "RefPrice%d", Index );
	return GetDoubleValue( ItemCount, 0.0 );
}
//---------------------------------------------------------------------------
double TSecurityDefinitionMessage::GetRiseLimitPrice( int Index )
{
	UFC::AnsiString	ItemCount;
	ItemCount.Printf( "RiseLimitPrice%d", Index );
	return GetDoubleValue( ItemCount, 0.0 );
}
//---------------------------------------------------------------------------
double TSecurityDefinitionMessage::GetFallLimitPrice( int Index )
{
	UFC::AnsiString	ItemCount;
	ItemCount.Printf( "FallLimitPrice%d", Index );
	return GetDoubleValue( ItemCount, 0.0 );
}
//---------------------------------------------------------------------------
int TSecurityDefinitionMessage::GetStrikePrice( int Index )
{
	UFC::AnsiString	ItemCount;
	ItemCount.Printf( "StrikePrice%d", Index );
	return GetIntegerValue( ItemCount, 0 );
}
//---------------------------------------------------------------------------

