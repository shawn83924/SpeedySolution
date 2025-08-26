//---------------------------------------------------------------------------
#include "TNewsMessage.h"
//---------------------------------------------------------------------------
TNewsMessage::TNewsMessage()
{}
//---------------------------------------------------------------------------
TNewsMessage::~TNewsMessage()
{}
//---------------------------------------------------------------------------
const char*	TNewsMessage::GetHeadline( void )
{
	return	GetStringValue( "148" );
}
//---------------------------------------------------------------------------
const char*	TNewsMessage::GetText( void )
{
	return  GetStringValue( "58" );
}
//---------------------------------------------------------------------------
long TNewsMessage::GetID( void )
{
	return  GetIntegerValue( "NUID", 0 );
}
//---------------------------------------------------------------------------
long TNewsMessage::GetMsgCount( void )
{
	return  GetIntegerValue( "MCNT", 1 );
}
//---------------------------------------------------------------------------
long TNewsMessage::GetMsgNum( void )
{
	return  GetIntegerValue( "MIDX", 1 );
}
//---------------------------------------------------------------------------
void TNewsMessage::SetMarket( const char* Market )
{
	SetStringValue( "Market", Market );
}
//---------------------------------------------------------------------------
const char* TNewsMessage::GetMarket( void )
{
	return GetStringValue( "Market" );
}
//---------------------------------------------------------------------------
void TNewsMessage::SetHeadline( const char* Headline )
{
	SetStringValue( "148", Headline );
}
//---------------------------------------------------------------------------
void TNewsMessage::SetText( const char* Text )
{
	SetStringValue( "58", Text );
}
//---------------------------------------------------------------------------
void TNewsMessage::SetID( long UID )
{
	SetIntegerValue( "NUID", UID );
}
//---------------------------------------------------------------------------
void TNewsMessage::SetMsgCount( long Count )
{
	SetIntegerValue( "MCNT", Count );
}
//---------------------------------------------------------------------------
void TNewsMessage::SetMsgNum( long Index )
{
	SetIntegerValue( "MIDX", Index );
}
//---------------------------------------------------------------------------
