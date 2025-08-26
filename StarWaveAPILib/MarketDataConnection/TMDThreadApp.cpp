#include "TMDThreadApp.h"
//------------------------------------------------------------------------------
TMDThreadApp::TMDThreadApp( MApp* pApp )
:UFC::PThread( NULL, FALSE ) ///< Listener: NULL FreeOnTerminate: false
,FApp( pApp )
{
    Start();
}
//------------------------------------------------------------------------------
TMDThreadApp::~TMDThreadApp( void )
{
	if( FApp != NULL )
	{
		UFC::BufferedLog::Printf( " Stop MApp." );
		FApp->stop();
		UFC::BufferedLog::Printf( " Terminate thread." );
		Terminate();
		this->WaitFor( 10 );
//		delete FApp;
//		FApp = NULL;
	}
}
//------------------------------------------------------------------------------
void TMDThreadApp::Execute( void )
{
	UFC::BufferedLog::Printf( " Start MApp message pump" );
	if( FApp != NULL )
		FApp->start( FALSE );
}
//------------------------------------------------------------------------------

