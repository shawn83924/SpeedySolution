#include "TMdTransport.h"
#include <stdio.h>
#include <string.h>
//---------------------------------------------------------------------------
extern UFC::BufferedLog* Glog;
//------------------------------------------------------------------------------
// PROXY protocol v2 header
//------------------------------------------------------------------------------
namespace
{
/**
 *  Access MApp::FSockClient (protected) without modifying Migo.
 *  Never instantiated, only used to form a pointer-to-member of MApp.
 */
struct MAppSocketAccessor : public MApp
{
    static UFC::PClientSocket* GetSocket( MApp* App )
    {
        return App->*( &MAppSocketAccessor::FSockClient );
    }
};
//------------------------------------------------------------------------------
const int           PROXY_V2_IPV4_SIZE = 28;
const unsigned char PROXY_V2_SIGNATURE[12] = { 0x0D,0x0A,0x0D,0x0A,0x00,0x0D,0x0A,0x51,0x55,0x49,0x54,0x0A };
//------------------------------------------------------------------------------
bool ParseIPv4( const UFC::AnsiString& IP, unsigned char Out[4] )
{
    unsigned int a, b, c, d;
    char         Tail;

    if( sscanf( IP.c_str(), "%u.%u.%u.%u%c", &a, &b, &c, &d, &Tail ) != 4 )
        return false;
    if( a > 255 || b > 255 || c > 255 || d > 255 )
        return false;
    Out[0] = (unsigned char)a;
    Out[1] = (unsigned char)b;
    Out[2] = (unsigned char)c;
    Out[3] = (unsigned char)d;
    return true;
}
}
//------------------------------------------------------------------------------
/**
 *  Header layout follows SpeedyProxy MigoHeader::ProcessProxyProtocol():
 *      [0..11]  Signature
 *      [12]     0x21 (v2, PROXY)
 *      [13]     0x11 (IPv4, TCP)
 *      [14..15] Length 12
 *      [16..19] LocalIP  -> server ClientSocket local IP
 *      [20..21] LocalPort (log only)
 *      [22..25] PeerIP   -> server ClientSocket->GetPeerIPAddress()
 *      [26..27] PeerPort (log only)
 *  NOTE: This is NOT the standard PROXY v2 address order, it matches the server parser.
 */
BOOL TTransport::SendProxyHeader( const UFC::AnsiString& LocalIP, const UFC::AnsiString& PeerIP )
{
    unsigned char Header[ PROXY_V2_IPV4_SIZE ];
    unsigned char LocalAddr[4], PeerAddr[4];

    if( ParseIPv4( LocalIP, LocalAddr ) == false || ParseIPv4( PeerIP, PeerAddr ) == false )
    {
        Glog->fprintf( " SendProxyHeader() skip, not IPv4 Local[%s] Peer[%s].", LocalIP.c_str(), PeerIP.c_str() );
        return FALSE;
    }
    memset( Header, 0, sizeof( Header ) );
    memcpy( Header, PROXY_V2_SIGNATURE, sizeof( PROXY_V2_SIGNATURE ) );
    Header[12] = 0x21;
    Header[13] = 0x11;
    Header[14] = 0x00;
    Header[15] = 0x0C;
    memcpy( Header + 16, LocalAddr, 4 );
    memcpy( Header + 22, PeerAddr, 4 );     ///< Ports [20..21],[26..27] keep 0.
    try
    {
        UFC::PClientSocket* Socket = MAppSocketAccessor::GetSocket( FApp );

        if( Socket == NULL || FApp->IsConnected() == FALSE )
        {
            Glog->fprintf( " SendProxyHeader() skip, not connected." );
            return FALSE;
        }
        UFC::PLockObject Lock( Socket->IOLock ); ///< Same lock as Migo message sending, avoid interleave.
        Socket->BlockSend( (char*)Header, sizeof( Header ) );
    }
    catch( ... )
    {
        Glog->fprintf( " SendProxyHeader() send failed." );
        return FALSE;
    }
    Glog->fprintf( " SendProxyHeader() Local[%s] Peer[%s].", LocalIP.c_str(), PeerIP.c_str() );
    return TRUE;
}
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

