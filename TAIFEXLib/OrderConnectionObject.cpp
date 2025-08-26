#ident "@(#) $ Id: OrderConnectionObject.cpp,v 1.40 2004/05/31 02:11:22 ap27 Exp $"
#include "OrderConnectionObject.h"
#include "LinkSubSystem.h"
//----------------------------------------------------------------------------------------------------------------------
//
//  ### TAIFEX side ###
//  Connection object for AP code 4 Order subsystem.
//
//----------------------------------------------------------------------------------------------------------------------
TAIFEXOrderConnectionObject::TAIFEXOrderConnectionObject( ConnectionParameter& Info, TransactionObject* TObj, ConnectionListener* CListener,ThreadListener* TListener )
:ConnectionObjectBase( Info ),
 PThread( TListener, FALSE ),
 FNeedLock( FALSE ),
 FNeedReset( FALSE ),
 FConnected( FALSE ),
 FBackup( FALSE ),
 FTransactionObj( TObj ),
 FConnectionListener( CListener ),
 FConnection( NULL ),
 FLog( NULL )
{
	
}
//----------------------------------------------------------------------------------------------------------------------
TAIFEXOrderConnectionObject::~TAIFEXOrderConnectionObject()
{
    Close();
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderConnectionObject::UpdateConnectionParameter( ConnectionParameter& Info )
{
    FParameters = Info;
    FNeedLock   = FALSE;
    FNeedReset  = TRUE;
    FBackup     = FALSE;
    UFC::BufferedLog::Printf( " PVC:%d Changed to FCM:%s Password:%s", FParameters.GetPVC(), FParameters.GetBrokerID().c_str(), FParameters.GetPassword().c_str());
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderConnectionObject::OnCommand( int Command, void* Data )
{
    switch( Command )
    {
        case COMMAND_OPEN_MARKET:   break;
        case COMMAND_CLOSE_MARKET:  Terminate();
                                    break;
        case COMMAND_UNLOCK_PVC:    FNeedLock  = FALSE;
                                    FBackup    = FALSE;
                                    if( FConnected == TRUE )
                                            FNeedReset = TRUE;
                                    break;
        case COMMAND_LOCK_PVC:      FNeedLock  = TRUE;
                                    FBackup    = TRUE;
                                    break;
        case COMMAND_GET_STATE:     if( FBackup == TRUE )
                                            *((int*)Data ) = PVC_STATUS_BACKUP;
                                    else
                                            *((int*)Data ) = FConnection->GetStatus();
                                    break;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderConnectionObject::Close( void )
{
    FConnection->StopHandshake();
    if( IsTerminated() == FALSE ) ///< If it's a Non-blocking socket.
    {
        Terminate(); ///< Terminate the socket polling thread.
        WaitFor();   ///< Wait for thread terminated.
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderConnectionObject::ConfirmsubSystemHandshake( BOOL ReceivedL010 )
{
    if( FConnected == TRUE )
    {
        FConnected = FALSE;
        FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
    }
    FNeedLock = (!FConnection->OnlineHandshake( ReceivedL010 ));
    if( FNeedLock == TRUE )
    {
        FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_LOCKED );// Lock this connection.
    }
    else
    {
        FConnected = TRUE;
        FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_READY );
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TAIFEXOrderConnectionObject::CheckReceiveMessage( void )
{
    MessageHeader* InMsg;

    for( int i = 0; i < ORDERSUBSYSTEM_TIMEOUT; i++ )
    {
         if( IsTerminated() == TRUE ) ///< Close this connection.
             return NULL;
         if( FNeedReset == TRUE ) // Received PVC reset command.
         {
            FNeedReset = FALSE;
            throw( X25Exception( 0, "Received PVC reset.") );
         }
         if( ( InMsg = FConnection->ReceiveMessage( 1, FALSE ) ) != NULL )
         {
            if( InMsg->IsMessage( mtL010 ) == TRUE )
                throw( X25ReconnectException( "Receive a L010." ) );
            return InMsg;
         }
    }
    ///< Not receive any message during ORDERSUBSYSTEM_TIMEOUT.
    throw( X25ReconnectException( "Receive data timeout" ) );
    return NULL;
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderConnectionObject::MessageLoop( void )
{
    MessageHeader* InMsg;
    MessageHeader* OutMsg;
    MessageR050    R050;

    while( IsTerminated() == FALSE )
    {
        try
        {
            do
            {
                if( IsTerminated() == TRUE ) ///< Force close this connection.
                    return;
                else if( FNeedLock == TRUE ) ///< This PVC had been locked.
                    sleep( 2 );
                else                         ///< Normal state...Process the message.
                {
                    if( (InMsg = CheckReceiveMessage() ) != NULL )
                    {
                        if( ( OutMsg = FTransactionObj->OnMessage( this, InMsg ) ) != NULL )
                             FConnection->SendMessage( OutMsg, ORDERSUBSYSTEM_TIMEOUT );
                        delete InMsg;
                    }
                    ///< Should else need to throw reconnect exception ???
                }
            } while(  IsTerminated() == FALSE );
        }
        catch( X25RehandshakeException& e) // Receive a L010...go back to handshake.
        {
            ConfirmsubSystemHandshake( TRUE );
        }
        catch( X25ReconnectException& e)  // Timeout or reconnect
        {
            ConfirmsubSystemHandshake( FALSE );
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderConnectionObject::LockLoop( void )
{
    while( FNeedLock == TRUE &&  IsTerminated() == FALSE ) // This PVC had been lock.
    {
        sleep( 2 );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderConnectionObject::SetLogManager( LogManager* LogObj )
{
    FLog = LogObj;
    if( FConnection != NULL )
        FConnection->SetLogManager( FLog );
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXOrderConnectionObject::Execute( void )
{
    FConnection = new LinkSubSystemTAIFEX( FParameters );
    if( FLog != NULL )
        FConnection->SetLogManager( FLog );
    for( int i = 0; i< PVC_BUSY_RETRY_TIMES,  IsTerminated() == FALSE ; i++ )
    {
        try
        {
            if( i == PVC_BUSY_RETRY_TIMES - 1 )
            {
                FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_X25ERROR );// Can not establish the X.25 connection
                i = 0;
                FNeedLock = TRUE;
                LockLoop();
            }
            FConnection->Open( FParameters );
            ConfirmsubSystemHandshake( FALSE );
            MessageLoop();
            FConnection->Close();
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_NORMAL_CLOSE );// Close the connection normaly.
            break;
        }
        catch( X25Exception& e )
        {
            UFC::BufferedLog::Printf(" %s. Try to reconnect after 2 sec...",e.what() );
            FConnection->Close();
            sleep( 2 );
            continue;
        }
        catch( exception& e )
        {
            UFC::BufferedLog::Printf( " PVC %d Exception:%s",GetConnectionParameter( )->GetPVC(),e.what());
            FNeedLock = TRUE;
            LockLoop();
        }
        catch(...)
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR );// Unknown exception.
            FNeedLock = TRUE;
            LockLoop();
        }
    }
    delete FConnection;
    FConnection = NULL;
}
//----------------------------------------------------------------------------------------------------------------------
