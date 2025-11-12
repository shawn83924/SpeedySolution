#include "TMdTransport.h"
//---------------------------------------------------------------------------
extern UFC::BufferedLog* Glog;
//------------------------------------------------------------------------------
TTransport* TTransport::CreateTransport(  HINSTANCE Instance, MAppListener* pListener, const char* AppName, BOOL IsWin32GUIApp )
{
    return new TTransport( Instance, pListener, AppName, IsWin32GUIApp );
}
//------------------------------------------------------------------------------
// implement TMdTransport
//------------------------------------------------------------------------------
TTransport::TTransport( HINSTANCE Instance, MAppListener* pListener, const char* AppName, BOOL  IsWin32GUIApp )
:FThread( NULL )
,FApp( NULL )
{
#ifndef _WIN32
    FApp = new MApp();
#else
    if( IsWin32GUIApp==TRUE )
        FApp = new MWinApp( Instance, MApp::MAPP_DEF_PORT, MApp::MSG_QUEUE_SIZE );
    else
        FApp = new MApp( MApp::MAPP_DEF_PORT, MApp::MSG_QUEUE_SIZE  );
#endif
    FApp->SetAppName( AppName );
    FApp->SetListener( pListener );
    FThread = new TThreadApp( FApp );
}
//------------------------------------------------------------------------------
TTransport::~TTransport( void )
{
    Disconnect();
    delete FThread;
    delete FApp;
}
//------------------------------------------------------------------------------
void TTransport::Connect( const char* RemoteIP, long RemotePort, int TimeoutSec )
{
    Disconnect();
    FApp->SetHost( RemoteIP );
    FApp->SetPort( RemotePort );
    FApp->Connect( TimeoutSec, 8000 );
}
//------------------------------------------------------------------------------
void TTransport::Disconnect( void )
{
    if( FApp->IsConnected() == TRUE )
    {
        Glog->fprintf( " MApp->Disconnect()" );
        FApp->Disconnect();
    }
}
//------------------------------------------------------------------------------
BOOL TTransport::Send( const char* Subject, const char* Key, TMdMessage* Msg )
{
    MTree* pTree = Msg->GetMTree();
    
    if( FApp->GetLocalIPAddress().Length() > 0 )
        Msg->SetStringValue( "PEERIP", FApp->GetLocalIPAddress() );
    return FApp->send_by_key( Subject, Key, *pTree, false );
}
//------------------------------------------------------------------------------
BOOL TTransport::Send( UFC::MemoryStream* WriteStream )
{
    return FApp->send_direct( WriteStream );
}
//------------------------------------------------------------------------------
void TTransport::Subscribe( MSubscriber* Subscribe )
{
    FApp->AddSubscriber( Subscribe );
}
//------------------------------------------------------------------------------
void TTransport::Unsubscribe( MSubscriber* Subscribe )
{
    FApp->DeleteSubscriber( Subscribe );
}
//------------------------------------------------------------------------------

