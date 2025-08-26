//---------------------------------------------------------------------------
#include "TSecurityDefinitionRequest.h"
//---------------------------------------------------------------------------
TSecurityDefinitionRequestMessage::TSecurityDefinitionRequestMessage( void )
{}
//---------------------------------------------------------------------------
TSecurityDefinitionRequestMessage::~TSecurityDefinitionRequestMessage( void )
{}
//---------------------------------------------------------------------------
const char* TSecurityDefinitionRequestMessage::GetRequestMarket( void )
{
	return GetStringValue( "RequestMarket" );
}
//---------------------------------------------------------------------------
void TSecurityDefinitionRequestMessage::SetRequestMarket( const char* Market)
{
	SetStringValue( "RequestMarket", Market );
}
//---------------------------------------------------------------------------
