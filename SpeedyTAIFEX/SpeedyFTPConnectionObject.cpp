//---------------------------------------------------------------------------
// SpeedyFTPConnectionObject.cpp
//---------------------------------------------------------------------------
#include "SpeedyFTPConnectionObject.h"
#include "SpeedyObjectsManager.h"
//---------------------------------------------------------------------------
TSpeedyFTPConnectionObject::TSpeedyFTPConnectionObject( SpeedyObjectsManager* ObjManager,
                                                        ConnectionParameter& info, 
                                                        UFC::ThreadListener* TListener, 
                                                        ConnectionListener* CListener,
                                                        BOOL IsOpt )
:ConnectionObjectBase( info )
,UFC::PThread( TListener, TRUE )
,X25Connection( NULL )
,FX25Stat( stX25Normal )
,X25ConnectionListener( CListener )
,FObjectManager( ObjManager )
,FSourceID( "" )
,FDestinationID( "" )
,FFolderName( "" )
,FIdleTime( 0 )
,FIsOptions( IsOpt )
{
}
//---------------------------------------------------------------------------
void TSpeedyFTPConnectionObject::Execute( void )
{
    X25Connection = new LinkSubSystemFCM( FParameters );
    X25Connection->SetLogManager( FObjectManager->GetFileTransferLog() );

    for( int i = 1; i <= PVC_BUSY_RETRY_TIMES; i++ )
    {
        try
        {
            X25Connection->Open();
            X25ConnectionHandshake( FALSE );
            MessageLoop();
            X25Connection->Close();
            break;
        }
        catch( X25Exception& e )
        {
            X25Connection->Close();
            X25ConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_X25ERROR );// Can not establish the X.25 connection
            UFC::BufferedLog::Printf( " %s. Try to reconnect after 5 sec....\n", e.what() );
            sleep( 5 );
        }
        catch( UFC::FileException& e )
        {
            UFC::BufferedLog::Printf( " %s\n", e.what() );
            break;
        }
        catch(...)
        {
            UFC::BufferedLog::Printf( " Unknown exception" );
            X25ConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR ); // Unknown exception.
            break;
        }
    }
    X25Connection->Close();
    delete X25Connection;
    X25Connection = NULL;
}
//---------------------------------------------------------------------------
void TSpeedyFTPConnectionObject::MessageLoop( void )
{
    while( ! IsTerminated() )
    {
        try
        {
            if( FX25Stat & stX25Handshake )
                X25ConnectionHandshake( TRUE );
            CustomMessageLoopFunction();
        }
        catch( X25RehandshakeException& e ) // Receive a L010...go back to handshake.
        {
            UFC::BufferedLog::Printf( " %s", e.what() );
            FIdleTime = 0;
            X25ConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
            FX25Stat |= stX25Handshake;
        }
        catch( X25ReconnectException& e ) // Timeout or reconnect
        {
            UFC::BufferedLog::Printf( " %s", e.what() );
            X25ConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
            FX25Stat |= stX25Handshake;
        }
    }
}
//---------------------------------------------------------------------------
void TSpeedyFTPConnectionObject::OnCommand( int Command, void* Data )
{
    switch ( Command )
    {
        case COMMAND_CLOSE_MARKET:  Terminate();
                                    break;
        case COMMAND_GET_STATE:     if( X25Connection != NULL )
                                        *( (int*)Data ) = X25Connection->GetStatus();
                                    else
                                        *( (int*)Data ) = PVC_STATUS_BROKEN;
                                    break;
        case EVENT_PVC_READ:
        case EVENT_PVC_WRITE: X25ConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_READ_WRITE, Data );
                              break;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TSpeedyFTPConnectionObject::Listen( BOOL IsListen )
{
    if( X25Connection != NULL )
    {
        if( IsListen == TRUE )
            X25Connection->SetListener( this );
        else
            X25Connection->SetListener( NULL );
    }
}
//---------------------------------------------------------------------------
void TSpeedyFTPConnectionObject::X25ConnectionHandshake( BOOL ReceivedL010 )
{
    if( X25Connection->OnlineHandshake(ReceivedL010) )
    {
        FX25Stat = stX25Normal;
        X25ConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_READY );
    }
}
//---------------------------------------------------------------------------
BOOL TSpeedyFTPConnectionObject::SendMessage( FMessageHeader* FMsg, long TimeoutSec )
{
    if( FMsg )
    {
        try
        {
            return X25Connection->SendMessage( FMsg, TimeoutSec );
        }
        catch(...)
        {
        }
    }
    return FALSE;
}
//---------------------------------------------------------------------------
MessageHeader* TSpeedyFTPConnectionObject::ReceiveMessage( long TimeoutSec, TMessageType RequireType )
{
    MessageHeader* InMsg = NULL;

    if( (InMsg = X25Connection->ReceiveMessage( TimeoutSec, FALSE )) != NULL )
    {
        if( RequireType != mtUnknown && InMsg->MessageType() != RequireType )
        {
            delete InMsg;
            InMsg = NULL;
        }
    }
    return InMsg;
}
//-----------------------------------------------------------------------------
void TSpeedyFTPConnectionObject::SetFolderName( const UFC::AnsiString& FolderName )
{
    FFolderName = FolderName + "/";
}
//-----------------------------------------------------------------------------
void TSpeedyFTPConnectionObject::ReportStatusCode( const UFC::AnsiString& FileCode, int StatusCode )
{
    TFtpStatusObject* pObj = new TFtpStatusObject( FileCode, StatusCode );

    X25ConnectionListener->OnConnectionNotify( this, FTP_STATUS_REPORT, (void*)pObj );
}
//-----------------------------------------------------------------------------
