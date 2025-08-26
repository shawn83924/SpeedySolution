//---------------------------------------------------------------------------
#include "TMDQuoteRequestMessage.h"
//---------------------------------------------------------------------------
TMDQuoteRequestMessage::TMDQuoteRequestMessage()
{}
//---------------------------------------------------------------------------
TMDQuoteRequestMessage::~TMDQuoteRequestMessage()
{}
//---------------------------------------------------------------------------
const char* TMDQuoteRequestMessage::GetSymbol( void )
{
	return GetStringValue( "Symbol" );
}
//---------------------------------------------------------------------------
void TMDQuoteRequestMessage::SetSymbol( const char* Symbol )
{
	SetStringValue( "Symbol", Symbol );
}
//---------------------------------------------------------------------------
const char* TMDQuoteRequestMessage::GetSecurityID( void )
{
	return GetStringValue( "SecurityID" );
}
//---------------------------------------------------------------------------
void TMDQuoteRequestMessage::SetSecurityID( const char* SecurityID )
{
	SetStringValue( "SecurityID", SecurityID );  
}
//---------------------------------------------------------------------------
