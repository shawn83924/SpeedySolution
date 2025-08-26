#include "TThreadApp.h"
//------------------------------------------------------------------------------
TThreadApp::TThreadApp( MApp* pApp )
:UFC::PThread( NULL, FALSE )
,FApp( pApp )
{
    Start();
}
//------------------------------------------------------------------------------
TThreadApp::~TThreadApp( void )
{
    if( FApp != NULL )
    {
		UFC::BufferedLog::DebugPrintf( " Stop MApp." );
		FApp->stop();
		UFC::BufferedLog::DebugPrintf( " Terminate thread." );
		Terminate();
	}
}
//------------------------------------------------------------------------------
void TThreadApp::Execute( void )
{
	UFC::BufferedLog::DebugPrintf( " Start MApp message pump" );
	UFC::BufferedLog::DebugPrintf( " MApp->Start()" );
	FApp->start( FALSE );
	fflush( stdout );
}
//------------------------------------------------------------------------------
