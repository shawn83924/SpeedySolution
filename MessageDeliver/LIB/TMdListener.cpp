#include "TMdListener.h"
//------------------------------------------------------------------------------
TMdListener::TMdListener( TTransport* pTransport, MessageListener* pEventListener, const char* Subject, const char* Key )
:FTransport( pTransport )
,FSubscriber( NULL )
,FName( "" )
,FSubject( Subject )
,FKey( Key )
{
	FName.Printf( "Subscriber_%s_%s_%p", Subject, Key, this );
	FSubscriber = new MSubscriber( FName, FSubject, FKey );
	FSubscriber->addListener( pEventListener );
	FTransport->Subscribe( FSubscriber );
}
//------------------------------------------------------------------------------
TMdListener::~TMdListener( void )
{
	FTransport->Unsubscribe( FSubscriber );
	delete FSubscriber;
}
//---------------------------------------------------------------------------

