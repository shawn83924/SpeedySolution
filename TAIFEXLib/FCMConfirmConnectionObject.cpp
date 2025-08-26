#include "FCMConfirmConnectionObject.h"
#include "../Migo/Sigo.h"
#include "Connection.h"
#include "LinkSubSystem.h"
//----------------------------------------------------------------------------------------------------------------------
extern MessageObject*  MessageObj;
extern BOOL            IsOptions;
extern AnsiString      FILL_REPORT_SUBJECT;
//----------------------------------------------------------------------------------------------------------------------
//
//  ### FCM side ###
//  Connection object for AP code 3,8,9 Confirm subsystem.
//
//----------------------------------------------------------------------------------------------------------------------
FCMConfirmConnectionObject::FCMConfirmConnectionObject( ConnectionParameter& info, ConnectionListener* CListener, ThreadListener* TListener, char APCode, char* IPAddress, int Port )
:ConnectionObjectBase( info ),
 PThread( TListener, TRUE ),
 FNeedLock( FALSE ),
 FHandshakeReady( FALSE ),
 FSeqNumber( 1 ),
 F030Count( 0 ),
 F031Count( 0 ),
 F032Count( 0 ),
 FAPCode( APCode ),
 FBroken( FALSE ),
 FConnection( NULL ),
 FConnectionListener( CListener )
{
	Start();
}
//----------------------------------------------------------------------------------------------------------------------
FCMConfirmConnectionObject::~FCMConfirmConnectionObject()
{
    if( IsTerminated() == FALSE ) ///< If it's a Non-blocking socket.
    {
        Terminate(); ///< Terminate the socket polling thread.
        WaitFor();   ///< Wait for thread terminated.
    }
}
//---------------------------------------------------------------------------------------------------------------------
void FCMConfirmConnectionObject::SendSocketMessage( BOOL IsSend, MessageHeader* Msg )
{
    char    Buffer[ 1024 ];
    MTree   Data;

    if( MessageObj != NULL )
    {
    	Msg->AsString( Buffer );
        Data.append( "FILL_ORDER", Buffer );
        MessageObj->Send( FILL_REPORT_SUBJECT, "all", Data );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void FCMConfirmConnectionObject::FCMHandshake( void )
{
	MessageHeader*  InMsg;
	MessageC010     C010( (char*)FParameters.GetBrokerID().c_str(),FSeqNumber );

	if( FConnection->SendMessage( &C010, ORDERSUBSYSTEM_TIMEOUT ))
	{
		if( (InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT )) != NULL )
		{
			if( InMsg->IsMessage( mtC020 ))
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
//----------------------------------------------------------------------------------------------------------------------
void FCMConfirmConnectionObject::CMHandshake( void )
{
	MessageHeader*  InMsg;
	MessageC011     C011( (char*)FParameters.GetBrokerID().c_str(),FSeqNumber );

	if( FConnection->SendMessage( &C011, ORDERSUBSYSTEM_TIMEOUT ))
	{
		if( (InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT )) != NULL )
		{
			if( InMsg->IsMessage( mtC021 ))
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
//----------------------------------------------------------------------------------------------------------------------
void FCMConfirmConnectionObject::ConfirmsubSystemHandshake( BOOL ReceivedL010 )
{
	if( FConnection->OnlineHandshake( ReceivedL010 ))
	{
		switch( FAPCode )
		{
			case '3':   FCMHandshake(); break;
			case '8':
			case '9': 	CMHandshake();  break;
		}
	}
}
//----------------------------------------------------------------------------------------------------------------------
void FCMConfirmConnectionObject::ReceiveC030( MessageC030* C030 )
{
    SendSocketMessage( FALSE, C030 );
	F030Count += C030->GetMessageCount();
	printf( "***** C030 Count = %d ******\n", F030Count );
}
//----------------------------------------------------------------------------------------------------------------------
void FCMConfirmConnectionObject::ReceiveC031( MessageC031* C031 )
{
    SendSocketMessage( FALSE, C031 );
	F031Count += C031->GetMessageCount();
	printf( "***** C031 Count = %d ******\n", F031Count );
}
//----------------------------------------------------------------------------------------------------------------------
void FCMConfirmConnectionObject::ReceiveC032( MessageC032* C032 )
{
	SendSocketMessage( FALSE, C032 );
	F032Count += C032->GetMessageCount();
	printf( "***** C032 Count = %d ******\n", F032Count );
}
//----------------------------------------------------------------------------------------------------------------------
void FCMConfirmConnectionObject::MessageLoop( void )
{
	while( TRUE )
	{
		try
		{
			do
			{
				if( IsTerminated() == TRUE ) // Close this connection.
				    return;
				else                         // Normal state...Process the message.
				{
					MessageHeader* InMsg;
					MessageC050    ConfirmConnectionMessage;

					if( FHandshakeReady == FALSE )
					 	throw( X25ReconnectException("Confirm subsystem: SendMessage failed." ) );
					if( FBroken == TRUE )
					{
						FBroken = FALSE;
						throw( X25ReconnectException("Asked to rehandshake." ) );
					}
					if( (InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT )) != NULL )
					{
						int MsgType = InMsg->MessageType();

						switch( MsgType )
						{
							case mtC040: ConfirmConnectionMessage.UpdateTime();
										 FConnection->SendMessage( &ConfirmConnectionMessage, ORDERSUBSYSTEM_TIMEOUT );
										 delete InMsg;
										 break;          // Idel handshake message.
							case mtC030: ReceiveC030( static_cast< MessageC030* >(InMsg) );// Order confirm message
						                 delete InMsg;
										 break;
							case mtC031: ReceiveC031( static_cast< MessageC031* >(InMsg) );
						                 delete InMsg;
										 break;
							case mtC032: ReceiveC032( static_cast< MessageC032* >(InMsg) );
						                 delete InMsg;
										 break;
							case mtC060: delete InMsg;
										 return;         // End confirm message
							default:     delete InMsg;
						}
					}
				}
			} while( TRUE );
		}
		catch( X25RehandshakeException& e ) // Receive a L010...go back to linksubsystem handshake.
		{
			printf("----Rehandshake Exception\n");
			ConfirmsubSystemHandshake( TRUE );
		}
		catch( X25ReconnectException& e )  // Timeout or reconnect
		{
			printf("----Reconnect Exception\n");
			ConfirmsubSystemHandshake( FALSE );
		}
	}
}
//----------------------------------------------------------------------------------------------------------------------
void FCMConfirmConnectionObject::Execute( void )
{
	printf("ConnectObject PVC:%d Run.\n",FParameters.GetPVC());
	FConnection = new LinkSubSystemFCM( FParameters );
	for( int i = 0; i< PVC_BUSY_RETRY_TIMES; i++ )
	{
		try
		{
			FConnection->Open();
			ConfirmsubSystemHandshake( FALSE );
			MessageLoop();
		  	FConnection->Close();
		  	FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_NORMAL_CLOSE );// Close the connection normaly.
			return;
		}
		catch( X25Exception& e )
		{
			FConnection->Close();
			printf("%s. Try to reconnect after 5 sec....\n",e.what() );
			sleep( 5 );
			continue;
		}
		catch( exception &e )
		{
			FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR );// Unknown exception.
			return;
		}
	}
	FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_X25ERROR );// Can not establish the X.25 connection
	delete FConnection;
	return;
}
//----------------------------------------------------------------------------------------------------------------------
