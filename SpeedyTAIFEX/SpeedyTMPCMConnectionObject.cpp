#include "SpeedyConfirmConnectionObject.h"
#include "SpeedyObjectsManager.h"
#include "../TAIFEXLib/LinkSubSystem.h"
#include "../TAIFEXLib/Connection.h"
#include "../Migo/Sigo.h"
//----------------------------------------------------------------------------------------------------------------------
extern MessageObject*   MessageObj;
extern BOOL             RecoverMode;
//----------------------------------------------------------------------------------------------------------------------
//
//  ### FCM side ###
//  Connection object for AP code 3,8,9 Confirm subsystem.
//
//----------------------------------------------------------------------------------------------------------------------
SpeedyTMPCMConnectionObject::SpeedyTMPCMConnectionObject( SpeedyObjectsManager* ObjManager,
                                                          ConnectionParameter&  info,
                                                          ConnectionListener*   CListener,
                                                          UFC::ThreadListener*  TListener,
                                                          char APCode,
                                                          BOOL IsOpt )
:ConnectionObjectBase( info ),
 UFC::PThread( TListener, TRUE ),
 FObjectManager( ObjManager ),
 FNeedLock( FALSE ),
 FHandshakeReady( FALSE ),
 FEnabled( TRUE ),
 FBroken( FALSE ),
 FAPCode( APCode ),
 FBrokerIndex( info.GetBrokerIndex() ),
 FSessionID( info.GetPVC()),
 FExecCount( 0 ),
 FSendHeartbeat( 0 ),
 FRecvHeartbeat( 0 ),
 FConnectionListener( CListener ),
 FCMID( info.GetBrokerID() ),
 FSID( info.GetPVC() ) 
{
    UFC::AnsiString FillSeqName;

    if( IsOpt )
    {
        if( FExecQueue.Open( OPT_EXEC_QUEUE ) == FALSE )
        {
            UFC::BufferedLog::Printf( " Open Execution queue:%x failed", OPT_EXEC_QUEUE );
            exit(0);
        }
    }
    else
    {
        if( FExecQueue.Open( FUT_EXEC_QUEUE ) == FALSE )
        {
            UFC::BufferedLog::Printf( " Open Execution queue:%x failed", FUT_EXEC_QUEUE );
            exit(0);
        }
    }    
    FConnection = new LinkSubSystemFCM( FObjectManager->IsOptions(), info.GetLink(),info.GetPVC(), (char*)info.GetPassword().c_str(), (char*)info.GetBrokerID().c_str(), '8', (char*)info.GetURL().c_str() );
    FCMID.PadThis(4, '0' );
    FillSeqName.Printf( "%s.%c.%d,%d", (IsOpt)?"Option":"Futures", APCode, FBrokerIndex, FSessionID );
    FSeqNumber = new UFC::PInt32( FillSeqName, 1 );///< Set sequence to 1 if share memory not exists.
    if( RecoverMode == FALSE )
       *FSeqNumber = 1;
    FConnection->SetLogManager( FObjectManager->GetFillLog() );
}
//----------------------------------------------------------------------------------------------------------------------
SpeedyTMPCMConnectionObject::~SpeedyTMPCMConnectionObject()
{
    delete FSeqNumber;
    if( IsTerminated() == FALSE )
    {
        Terminate(); ///< Terminate the thread.
        WaitFor();   ///< Wait for thread terminated.
    }
    delete FConnection;
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPCMConnectionObject::ConfirmsubSystemHandshake( BOOL ReceivedL010 )
{
    if( FConnection->OnlineHandshake( ReceivedL010 ))
    {
        TMP::TMPMessage*  InMsg;
        TMP::TMPMessage   R15( TMP::tmtR15 );

        R15[ "cm_id" ]      = FBrokerIndex;
        R15[ "req_cm_seq" ] = FSeqNumber->ToInt32();
        R15[ "target_id" ]  = FAPCode -'0';
        if( FConnection->SendTMPMessage( &R15, TMP_TIMEOUT, FALSE ) == TRUE )
        {
            if( (InMsg = FConnection->ReceiveTMPMessage( TMP_TIMEOUT, FALSE )) != NULL )
            {
                if( InMsg->GetMessageType() == TMP::tmtR16 )
                {
                    delete InMsg;
                    FHandshakeReady = TRUE;
                    FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_READY,(void*)FSeqNumber);
                    return;
                }
                delete InMsg;
            }
        }
        FHandshakeReady = FALSE;
        FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
    }
    else
        throw X25Exception( -9,"Handshake exception.");

}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPCMConnectionObject::ReceiveC030( TMP::TMPMessage* R02 )
{
    UFC::MemoryStream MemWriter( 256 );
    
    *FSeqNumber = R02->GetField( "rpt_seq" )->ToInteger();
    R02->ToStream( &MemWriter );
    ///< Use the Sub-System name as MType of message
    UFC::UQueueData OrderData( (long)FSessionID, (char*)MemWriter.GetBuffer(), MemWriter.GetSize() );
    FExecQueue.Send( OrderData );     
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPCMConnectionObject::OnCommand( int Command, void* Data )
{
    switch( Command )
    {
        case COMMAND_GET_STATE: *((int*)Data ) = FConnection->GetStatus();
                                break;
        case EVENT_PVC_READ:
        case EVENT_PVC_WRITE:   FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_READ_WRITE, Data );
                                break;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPCMConnectionObject::Listen( BOOL IsListen )
{
    if( FConnection != NULL )
    {
        if( IsListen == TRUE )
            FConnection->SetListener( this );
        else
            FConnection->SetListener( NULL );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPCMConnectionObject::Enable( BOOL IsEnabled )
{
    if( FEnabled != IsEnabled )
    {
        if( IsEnabled == FALSE )
        {
            FConnection->SetStatus( PVC_STATUS_DISABLEING );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_UPDATE_STATE, NULL );
            FConnection->StopHandshake();
            FEnabled = FALSE;
        }
        else
        {
            FConnection->SetStatus( PVC_STATUS_ENABLE );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_ENABLED, NULL );
            FEnabled = TRUE;
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPCMConnectionObject::ReceiveExecution( void )
{
    TMP::TMPMessage*  InMsg;

    if( (InMsg = FConnection->ReceiveTMPMessage( (long)1, FALSE )) != NULL)  ///< Try to receive data from TAIFEX
    {
        FRecvHeartbeat = 0;
        TMP::TMPMessageType MsgType = InMsg->GetMessageType( );

        if( MsgType == TMP::tmtR02 )
        {
            ReceiveC030( InMsg );
        }
        else if( MsgType == TMP::tmtR04 )
        {
            TMP::TMPMessage  R05(TMP::tmtR05 ); // R050 Confirm Connection message
            FConnection->SendTMPMessage( &R05, TMP_TIMEOUT, TRUE  );// Send the confirm connection message
            FSendHeartbeat = 0;
        }
        else if( MsgType == TMP::tmtR05 )
        {
            UFC::BufferedLog::Printf( " Recv Heartbeat reply." );
        }
        
        delete InMsg;
    }
    else
    {
        FSendHeartbeat++;
        FRecvHeartbeat++;
    }
    ///< Check heartbeat.
    if( FSendHeartbeat == FConnection->GetHeartBeatInterval() ||
        FRecvHeartbeat == FConnection->GetHeartBeatInterval() ) ///< Time is up!
    {
        TMP::TMPMessage  R04(TMP::tmtR04 ); // R040 Confirm Connection message
        FConnection->SendTMPMessage( &R04, TMP_TIMEOUT, TRUE  );// Send the confirm connection message
        FSendHeartbeat = 0;
    }
    if(  FSendHeartbeat > FConnection->GetHeartBeatInterval() + 5 ||
         FRecvHeartbeat > FConnection->GetHeartBeatInterval() + 5  ) ///< Broker Timeout!!
    {
         UFC::BufferedLog::Printf( " Heartbeat Timeout" );
         throw( X25ReconnectException( " Confirm subsystem: SendMessage failed." ) );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPCMConnectionObject::MessageLoop( void )
{
    while( TRUE )
    {
        try
        {
            if( IsTerminated() == TRUE ) // Close this connection.
            {
                return;
            }
            else if( FEnabled == FALSE ) ///< Disable this connection.
            {
                FHandshakeReady = FALSE; ///< Need to re-handshake
                sleep( 1 );
                FConnection->SetStatus( PVC_STATUS_DISABLE);
                FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_DISABLED, NULL );
            }
            else                         // Normal state...Process the message.
            {
                if( FHandshakeReady == FALSE )
                    throw( X25ReconnectException("Confirm subsystem: SendMessage failed." ) );
                if( FBroken == TRUE )
                {
                   FBroken = FALSE;
                   throw( X25ReconnectException("Asked to rehandshake." ) );
                }
                ReceiveExecution(  );
            }
        }
        catch( X25RehandshakeException& e ) // Receive a L010...go back to linksubsystem handshake.
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
            UFC::BufferedLog::Printf( " Session[%d]:%s", FSessionID, e.what() );
            ConfirmsubSystemHandshake( TRUE );
        }
        catch( X25ReconnectException& e )  // Timeout or reconnect
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
            UFC::BufferedLog::Printf( " Session[%d]:%s", FSessionID, e.what() );
            ConfirmsubSystemHandshake( FALSE );
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPCMConnectionObject::DisableLoop( void )
{
    if( FEnabled == FALSE )
    {
        BOOL FirstTime = TRUE;
        while( FEnabled == FALSE ) ///< Disable this connection.
        {
            sleep( 1 );
            if( FirstTime == TRUE )
            {
                FConnection->SetStatus( PVC_STATUS_DISABLE);
                FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_DISABLED, NULL );
                FirstTime = FALSE;
            }
        }
        FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN, NULL );
    }
}
//----------------------------------------------------------------------------------------------------------------------
BOOL SpeedyTMPCMConnectionObject::IsReady( void )
{
    if( FConnection != NULL )
        return FConnection->IsReady();
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPCMConnectionObject::Execute( void )
{
    for( int i = 0; i< PVC_BUSY_RETRY_TIMES; i++ )
    {
        try
        {
            DisableLoop();
            FConnection->Open();
            ConfirmsubSystemHandshake( FALSE );
            MessageLoop(); ///< Return when market closed.
            FConnection->Close();
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_NORMAL_CLOSE );// Close the connection normaly.
            UFC::BufferedLog::Printf(" Session:%d Thread stopped. Market closed.", FSessionID );
            return;
        }        
        catch( X25Exception& e )
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
            FConnection->Close();
            UFC::BufferedLog::Printf(" %s. Try to reconnect after 5 sec...",e.what() );
            sleep( 5 );
        }
        catch( UFC::Exception &e )
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
            FConnection->Close();
            UFC::BufferedLog::Printf( " Exception:%s",e.what());
            sleep( 2 );
        }
        catch( std::exception& )
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR );// Unknown exception.
            return;
        }
    }
    UFC::BufferedLog::Printf(" Session:%d Thread stopped. Retry too many times.", FSessionID );
}
//----------------------------------------------------------------------------------------------------------------------
