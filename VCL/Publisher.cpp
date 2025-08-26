//---------------------------------------------------------------------------
#include <basepch.h>
#pragma hdrstop
#include "Publisher.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
namespace Publisher
{
	void __fastcall PACKAGE Register()
	{
		 TComponentClass classes[1] = {__classid(TPublisher)};
		 RegisterComponents("MBus", classes, 0);
	}
}
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TPublisher *)
{
	new TPublisher(NULL);
}
//---------------------------------------------------------------------------
__fastcall TPublisher::TPublisher(TComponent* Owner)
:TComponent( Owner )
,FApp( NULL )
,FSubject( L"all" )
,FKey( L"all" )
,FAnsiSubject( "all" )
,FAnsiKey( "all" )
{
}
//---------------------------------------------------------------------------
__fastcall TPublisher::~TPublisher( void )
{
}
//---------------------------------------------------------------------------
void __fastcall TPublisher::SetMessageObject( TMessageObject* MessageObject )
{
	if( FApp != MessageObject )
		FApp = MessageObject;
}
//---------------------------------------------------------------------------
void __fastcall TPublisher::SetSubject( const System::UnicodeString Subject )
{
	if( Subject.Compare( FSubject ) != 0 )
	{
		AnsiString NewSubject = Subject;

		FSubject     = Subject;
		FAnsiSubject = NewSubject.c_str();
	}
}
//---------------------------------------------------------------------------
void __fastcall TPublisher::SetKey( const System::UnicodeString Key )
{
	if( Key.Compare( FKey ) != 0 )
	{
		AnsiString NewKey = Key;

		FKey     = Key;
		FAnsiKey = NewKey.c_str();
	}
}
//---------------------------------------------------------------------------
const System::UnicodeString __fastcall TPublisher::GetSubject( void )
{
	return FSubject;
}
//---------------------------------------------------------------------------
const System::UnicodeString __fastcall TPublisher::GetKey( void )
{
	return FKey;
}
//---------------------------------------------------------------------------
void __fastcall TPublisher::SendData( MTree* Data )
{
	if( FApp != NULL && FApp->FThreadMApp != NULL )
		FApp->FApp->send_by_key( FAnsiSubject, FAnsiKey, *Data, FALSE );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

