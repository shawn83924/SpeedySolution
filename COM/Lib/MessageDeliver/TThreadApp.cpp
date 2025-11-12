#include "TThreadApp.h"
//---------------------------------------------------------------------------
extern UFC::BufferedLog* Glog;
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
        Glog->fprintf( " Stop MApp." );
        FApp->stop();
        Glog->fprintf( " Terminate thread." );
        Terminate();
        this->WaitFor( 10 );
    }
}
//------------------------------------------------------------------------------
void TThreadApp::Execute( void )
{
    Glog->fprintf( " [TThreadApp] Start MApp message pump" );
    Glog->fprintf( " [TThreadApp] MApp->Start()" );
    FApp->start( FALSE );
    fflush( stdout );
}
//------------------------------------------------------------------------------
