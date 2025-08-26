#include "TMdTransport.h"
//------------------------------------------------------------------------------
TTransport* TTransport::CreateTransport(  HINSTANCE Instance, MAppListener* pListener, const char* AppName, BOOL IsWin32GUIApp )
{
	return new TMdTransport( Instance, pListener, AppName, IsWin32GUIApp );
}
//------------------------------------------------------------------------------
// implement TMdTransport
//------------------------------------------------------------------------------
TMdTransport::TMdTransport( HINSTANCE Instance, MAppListener* pListener, const char* AppName, BOOL  IsWin32GUIApp )
:FThread( NULL )
,FApp( NULL )
{
#ifndef _WIN32
	FApp = new MApp();
#else
	if( IsWin32GUIApp==TRUE )
		FApp = new MWinApp( Instance, MAPP_DEF_PORT, MSG_QUEUE_SIZE );
	else
		FApp = new MApp(  MAPP_DEF_PORT, MSG_QUEUE_SIZE  );
#endif
	FApp->SetAppName( AppName );
	FApp->SetListener( pListener );
    FThread = new TThreadApp( FApp );
}
//------------------------------------------------------------------------------
TMdTransport::~TMdTransport( void )
{
    Disconnect();
    delete FThread;
    delete FApp;
}
//------------------------------------------------------------------------------
void TMdTransport::Connect( const char* RemoteIP, long RemotePort, int TimeoutSec )
{
	Disconnect();
	UFC::BufferedLog::DebugPrintf( " MApp->SetHost( %s )", RemoteIP );
	FApp->SetHost( RemoteIP );
	UFC::BufferedLog::DebugPrintf( " MApp->SetPort( %d )", RemotePort );
	FApp->SetPort( RemotePort );
	UFC::BufferedLog::DebugPrintf( " MApp->Connect( %d )", TimeoutSec );
	FApp->Connect( TimeoutSec );
}
//------------------------------------------------------------------------------
void TMdTransport::Disconnect( void )
{
	if( FApp->IsConnected() == TRUE )
	{
		UFC::BufferedLog::DebugPrintf( " MApp->Disconnect()" );
		FApp->Disconnect();
	}
}
//------------------------------------------------------------------------------
BOOL TMdTransport::Send( const char* Subject, const char* Key, TMdMessage* Msg )
{
	MTree* pTree = Msg->GetMTree();
	return FApp->send_by_key( Subject, Key, *pTree, false );
}
//------------------------------------------------------------------------------
void TMdTransport::Subscribe( MSubscriber* Subscribe )
{
	FApp->AddSubscriber( Subscribe );
}
//------------------------------------------------------------------------------
void TMdTransport::Unsubscribe( MSubscriber* Subscribe )
{
	FApp->DeleteSubscriber( Subscribe );
}
//------------------------------------------------------------------------------

