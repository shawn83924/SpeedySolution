//---------------------------------------------------------------------------

#include <basepch.h>

#pragma hdrstop

#include "Subscriber.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
static inline void ValidCtrCheck( TSubscriber * )
{
	new TSubscriber(NULL);
}
//---------------------------------------------------------------------------
namespace Subscriber
{
	void __fastcall PACKAGE Register()
	{
		 TComponentClass classes[1] = {__classid(TSubscriber)};
		 RegisterComponents("MBus", classes, 0);
	}
}
//---------------------------------------------------------------------------
//
//   class TSubscriber
//   Wrap MSubscriber to VCL TSubscriber
//
//---------------------------------------------------------------------------
__fastcall TSubscriber::TSubscriber( TComponent* Owner )
:TComponent( Owner )
,FApp( NULL )
,FSubscriber( NULL )
,FSubject( L"all" )
,FKey( L"all" )
,FAnsiSubject( "all" )
,FAnsiKey( "all" )
,FListener( NULL )
,FDoEventListener( FListener )
{
}
//---------------------------------------------------------------------------
__fastcall TSubscriber::~TSubscriber( void )
{
	if( FSubscriber != NULL )
	{
		if( FApp != NULL )
			FApp->DeleteSubscriber( FSubscriber );
		delete FSubscriber;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSubscriber::Loaded( void )
{
	if( ComponentState.Contains( csDesigning ) )
		return;
	if( FSubscriber != NULL )
	{
		if( FApp != NULL )
			FApp->DeleteSubscriber( FSubscriber );
		delete FSubscriber;
	}
	AnsiString CopyName = Name;

	FSubscriber = new MSubscriber( CopyName.c_str(), FAnsiSubject, FAnsiKey );
	FSubscriber->addListener( &FDoEventListener );
	if( FApp != NULL )
		FApp->AddSubscriber( FSubscriber );
}
//---------------------------------------------------------------------------
void __fastcall TSubscriber::SetMessageObject( TMessageObject* NewMapp )
{
	if( ComponentState.Contains( csDesigning ) )
	{
		FApp = NewMapp;
		return;
	}
	if( FApp != NewMapp )  ///< If MessageObjects not the same
	{
		if( FSubscriber != NULL  )
		{
			if( FApp != NULL )
				FApp->DeleteSubscriber( FSubscriber );
		}
		else
		{
			AnsiString CopyName = Name;

			FSubscriber = new MSubscriber( CopyName.c_str(), FAnsiSubject, FAnsiKey );
			FSubscriber->addListener( &FDoEventListener );
		}
		FApp = NewMapp;
		if( FApp != NULL )
			FApp->AddSubscriber( FSubscriber );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSubscriber::SetSubject( System::UnicodeString Subject )
{
	if( Subject.Compare( FSubject ) != 0 )
	{
		AnsiString NewSubject = Subject;

		FSubject     = Subject;
		FAnsiSubject = NewSubject.c_str();
		Update();
	}
}
//---------------------------------------------------------------------------
void __fastcall TSubscriber::SetKey( System::UnicodeString Key )
{
	if( Key.Compare( FKey ) != 0 )
	{
		AnsiString NewKey = Key;

		FKey     = Key;
		FAnsiKey = NewKey.c_str();
		Update();
	}
}
//---------------------------------------------------------------------------
void __fastcall TSubscriber::Update( void )
{
	if( ComponentState.Contains( csDesigning ) )
		return;
	if( FSubscriber != NULL )
	{
		if( FApp != NULL )
			FApp->DeleteSubscriber( FSubscriber );
		delete FSubscriber;
	}
	AnsiString CopyName = Name;

	FSubscriber = new MSubscriber( CopyName.c_str(), FAnsiSubject, FAnsiKey );
	FSubscriber->addListener( &FDoEventListener );
	if( FApp != NULL )
		FApp->AddSubscriber( FSubscriber );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

