#ident "@(#)$Id: ConfirmConnectionObject.cpp,v 1.54 2004/08/14 08:03:34 ap25 Exp $"
/**@file ConfirmConnectionObject.cpp
*  @brief Class for sending confirm message to FCM.
*  @author Simon Chang
*
*/
#include "LinkSubSystem.h"
#include "ConfirmConnectionObject.h"
//----------------------------------------------------------------------------------------------------------------------
//
//  ### TAIFEX side ###
//  Connection object for AP code 3 Confirm subsystem.
//
//----------------------------------------------------------------------------------------------------------------------
extern BOOL IsOptions;
extern BOOL IsNewVersion;
//----------------------------------------------------------------------------------------------------------------------
TAIFEXConfirmConnectionObject::TAIFEXConfirmConnectionObject( ConnectionParameter& info,
                                                              ConnectionListener*  Listener,
                                                              UFC::ThreadListener* TListener )
:ConnectionObjectBase( info ),
 UFC::PThread( TListener, FALSE ),
 FHandshakeReady( FALSE ),
 FNeedLock( FALSE ),
 FNeedReset( FALSE ),
 FConnectionListener( Listener ),
 FConnection( NULL ),
 FSendPosition( 0 ),
 FReadPosition( 0 ),
 FEndPosition( 0 ),
 FTotalCount( 0 ),
 FConnected( FALSE ),
 FIsTSCD( FALSE ),
 FBackup( FALSE ),
 FLastMsg( NULL ),
 FLog( NULL )
{
	UFC::AnsiString LinkStr;

	LinkStr.Printf( "%06d", info.GetLink() );
	strncpy( FPVCInfo.line, LinkStr.c_str(),6 );
	FPVCInfo.apcode = info.GetAPCode();
	if( FPVCInfo.apcode == '3' )
	{
		strncpy( FPVCInfo.brokerid, info.GetBrokerID().c_str(), 7);
		if( strncmp( FPVCInfo.brokerid, "F099000", 7) == 0 )
			FIsTSCD = TRUE;
	}
	else  ///< for CM ap code 8,9
		strncpy( FPVCInfo.brokerid, info.GetBrokerID().c_str(), 4);
	FPVCInfo.pvc  = info.GetPVC();
	FMessages     = new UFC::PList( XBIO3_MESSAGEQUEUE_SIZE + 1 );
	FCCHReader    = new CCH_DF_Reader();
	FReportBuffer = new RPTLOG[ XBIO3_MESSAGEQUEUE_SIZE + 1 ];
}
//----------------------------------------------------------------------------------------------------------------------
TAIFEXConfirmConnectionObject::~TAIFEXConfirmConnectionObject()
{
    Close();
	delete [] FReportBuffer;
	delete FCCHReader;
	delete FMessages;
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmConnectionObject::UpdateConnectionParameter( ConnectionParameter& Info )
{
	char LinkNoStr[16];

	sprintf( LinkNoStr,"%06d", Info.GetLink() );
	strncpy( FPVCInfo.line, LinkNoStr, 6 );
	FPVCInfo.apcode = Info.GetAPCode();
	FPVCInfo.pvc    = Info.GetPVC();
	if( FPVCInfo.apcode == '3' )
	{
		strncpy( FPVCInfo.brokerid, Info.GetBrokerID().c_str(), 7);
		if( strncmp( FPVCInfo.brokerid, "F099000", 7) == 0 )
			FIsTSCD = TRUE;
	}
	else ///< for CM ap code 8,9
		strncpy( FPVCInfo.brokerid, Info.GetBrokerID().c_str(), 4);
	FParameters = Info;
	FNeedLock   = FALSE;
	FNeedReset  = TRUE;
	FBackup     = FALSE;
	UFC::BufferedLog::Printf( " PVC:%d Changed to FCM:%s Password:%s", FParameters.GetPVC(), FParameters.GetBrokerID().c_str(), FParameters.GetPassword().c_str());
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmConnectionObject::OnCommand( int Command, void* Data )
{
	switch( Command )
	{
		case CONNECTION_CCH_UPDATE_SEQ: FEndPosition = ( int ) Data;
										printf("_______________________________________________\n" );
										if( FPVCInfo.apcode == '3' )
											printf(" AP:%c FCM:%s Position = %d\n", FPVCInfo.apcode, UFC::AnsiString( FPVCInfo.brokerid,7).c_str(), FEndPosition );
										else ///< for CM ap code 8,9
											printf(" AP:%c FCM:%s Position = %d\n", FPVCInfo.apcode, UFC::AnsiString( FPVCInfo.brokerid,4).c_str(), FEndPosition );
										printf("_______________________________________________\n" );
										break;
		case CONNECTION_CCH_LAST_MSG:   FTotalCount = FEndPosition;
										memcpy( &FLastReport, Data, sizeof( RPTLOG_MSG_LAST ) );
										break;
		case COMMAND_OPEN_MARKET:       break;
		case COMMAND_CLOSE_MARKET:      Terminate();
									    break;
		case COMMAND_UNLOCK_PVC:        FNeedLock  = FALSE;
										FNeedReset = TRUE;
										FBackup    = FALSE;
										break;
		case COMMAND_LOCK_PVC:      	FNeedLock  = TRUE;
										FBackup    = TRUE;
										break;
	    case COMMAND_GET_STATE:         if( FBackup == TRUE )
	    									*((int*)Data ) = PVC_STATUS_BACKUP;
	    								else
	    									*((int*)Data ) = FConnection->GetStatus();
	    						     	break;
	}
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmConnectionObject::Close( void )
{
	FConnection->StopHandshake();
    if( IsTerminated() == FALSE ) ///< If it's a Non-blocking socket.
    {
        Terminate(); ///< Terminate the socket polling thread.
        WaitFor();   ///< Wait for thread terminated.
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmConnectionObject::AddReportMessage( int ReportCount, char ApCode )
{
    MessageC032* C032 = NULL;
	MessageC031* C031 = NULL;
	MessageC030* C030 = NULL;
	C03XBody*    NewMsg;
	int          ProcessingTime;

	if( ApCode == '3' )
	{
		for( int i = 0; i < ReportCount; i++ )
		{
			if( C030 == NULL )
			{
				C030 = new MessageC030( );
				C030->SetBroker( FPVCInfo.brokerid );
			}
			NewMsg = C03XBody::CreateFromRPTLog( &FReportBuffer[i], '3', FIsTSCD );
			ProcessingTime = NewMsg->ProcessingTime();
			TotalCount++;
			TotalProcessingTime += ProcessingTime;
			if( ProcessingTime > MaxProcessingTime )
				MaxProcessingTime = ProcessingTime;
			if( (C030->AddMessage( NewMsg ) == FALSE) || (i == ReportCount-1) )
			{
		       	UFC::PLockObject Lock( FMsgCriticalSection );

				FMessages->Add( (void*) C030 );
				C030 = NULL;
			}
	   }
	}
    else if( ApCode == '8' )
    {
        if( IsOptions == TRUE || IsNewVersion == TRUE )
        {
             for( int i = 0; i < ReportCount; i++ )
             {
                  if( C032 == NULL )
                  {
                      C032 = new MessageC032( );
                      C032->SetBroker( FPVCInfo.brokerid );
                  }
                  NewMsg = C03XBody::CreateFromRPTLog( &FReportBuffer[i], '8', FALSE );
                  ProcessingTime = NewMsg->ProcessingTime();
                  TotalCount++;
                  TotalProcessingTime += ProcessingTime;
                  if( ProcessingTime > MaxProcessingTime )
                      MaxProcessingTime = ProcessingTime;
                  if( C032->AddMessage( NewMsg ) == FALSE || i == ReportCount-1 )
                  {
                      FMsgCriticalSection.Enter();
                      FMessages->Add( (void*) C032 );
                      FMsgCriticalSection.Leave();
                      C032 = NULL;
                  }
             }
        }
        else
        {
           for( int i = 0; i < ReportCount; i++ )
           {
                if( C031 == NULL )
                {
                    C031 = new MessageC031( );
                    C031->SetBroker( FPVCInfo.brokerid );
                }
                NewMsg = C03XBody::CreateFromRPTLog( &FReportBuffer[i], '8', FALSE );
                ProcessingTime = NewMsg->ProcessingTime();
                TotalCount++;
                TotalProcessingTime += ProcessingTime;
                if( ProcessingTime > MaxProcessingTime )
                    MaxProcessingTime = ProcessingTime;
                if( C031->AddMessage( NewMsg ) == FALSE || i == ReportCount-1 )
                {
                    FMsgCriticalSection.Enter();
                    FMessages->Add( (void*) C031 );
                    FMsgCriticalSection.Leave();
                    C031 = NULL;
                }
           }
        }
    }
	else if( ApCode == '9' )
	{
	   for( int i = 0; i < ReportCount; i++ )
	   {
			if( C031 == NULL )
			{
				C031 = new MessageC031( );
				C031->SetBroker( FPVCInfo.brokerid );
			}
			NewMsg = C03XBody::CreateFromRPTLog( &FReportBuffer[i], '9', FALSE );
			ProcessingTime = NewMsg->ProcessingTime();
			TotalCount++;
			TotalProcessingTime += ProcessingTime;
			if( ProcessingTime > MaxProcessingTime )
				MaxProcessingTime = ProcessingTime;
			if( C031->AddMessage( NewMsg ) == FALSE || i == ReportCount-1 )
			{
		       	FMsgCriticalSection.Enter();
				FMessages->Add( (void*) C031 );
				FMsgCriticalSection.Leave();
				C031 = NULL;
			}
	   }
	}
}
//----------------------------------------------------------------------------------------------------------------------
BOOL TAIFEXConfirmConnectionObject::ReadMessage()
{
	int ReportCount;
	int EndPos;

	if( FEndPosition > 0 && FReadPosition < FEndPosition )
	{
		EndPos = FReadPosition + XBIO3_MESSAGEQUEUE_SIZE;
		if( EndPos > FEndPosition )
		    EndPos = FEndPosition;
		ReportCount = FCCHReader->Read_IDR( FReportBuffer, &FPVCInfo, FReadPosition + 1, EndPos );
		if( ReportCount > 0 )
		{
			AddReportMessage( ReportCount, FPVCInfo.apcode );
			FReadPosition += ReportCount;
			return TRUE;
		}
	}
	return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmConnectionObject::AddLastMessage( )
{
	MessageHeader* OutMsg;

	if( FIsTSCD == TRUE ) // Is TSCD??
		OutMsg = new MessageC030( FLastReport.Total_Buy_Qnty, FLastReport.Total_Sell_Qnty,FLastReport.Total_Record );
	else
		OutMsg = new MessageC060( FTotalCount );
	FLastMsg = OutMsg;
	FMsgCriticalSection.Enter();
	FMessages->Add( (void*) OutMsg );
	FMsgCriticalSection.Leave();
}
//----------------------------------------------------------------------------------------------------------------------
BOOL TAIFEXConfirmConnectionObject::SendConfirmMessage( )
{
	MessageHeader* OutMsg;
	int MsgCount;

	FMsgCriticalSection.Enter();
	OutMsg = static_cast< MessageHeader* >(FMessages->GetItem( 0 ));
	FMsgCriticalSection.Leave();
	if( FPVCInfo.apcode == '3' )
	    MsgCount = (static_cast< MessageC030* >(OutMsg))->GetMessageCount();
	else
		MsgCount = (static_cast< MessageC031* >(OutMsg))->GetMessageCount();
	FPreviousTime = ThreadClock::SecondOfToday();
	if( FConnection->SendMessage( OutMsg, ORDERSUBSYSTEM_TIMEOUT ) == TRUE )
	{
		FMsgCriticalSection.Enter();
		FMessages->Delete( 0 );
		FMsgCriticalSection.Leave();
		FSendPosition += MsgCount;
		if( FLastMsg != NULL && OutMsg == FLastMsg )
		{
			FLastMsg   = NULL;
			FConnectionListener->OnConnectionNotify( this, CONNECTION_CONFIRM_FINASHED);
			MessageL070 L070;
			FConnection->SendMessage( &L070, LINKSUBSYSTEM_TIMEOUT );
			FConnection->ReceiveMessage( LINKSUBSYSTEM_TIMEOUT );
			FNeedLock  = TRUE;
			if( FConnection != NULL )
			    FConnection->SetStatus( PVC_STATUS_FINISHED );
		}
		delete OutMsg;
		return TRUE;
	}
	return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
BOOL TAIFEXConfirmConnectionObject::MessageOut( )
{
	try
	{
		if( FHandshakeReady == TRUE && FConnected == TRUE )
		{
			int Count;

			if( FTotalCount != 0 && FSendPosition == FTotalCount )
				AddLastMessage();	  			    // Add the Last report message to list.
			FMsgCriticalSection.Enter();
			Count = FMessages->ItemCount( );
			FMsgCriticalSection.Leave();
			if( Count == 0 )						// Check whether need to read messages.
				return ReadMessage();
			else if( Count > 0 )
				return SendConfirmMessage();	    // Send the confirm report.
		}
		return FALSE;
	}
	catch(...)
	{
		FHandshakeReady = FALSE; // Force re-handshake
		return FALSE;
	}
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmConnectionObject::IdleHandshake( void )
{
	MessageC040    C040;
	MessageHeader* InMsg;

	if( FConnection->SendMessage( &C040, ORDERSUBSYSTEM_TIMEOUT ) == TRUE )
	{
		if( IsTerminated() == TRUE ) return;
		if( (InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT )) == NULL )
		{
			TMessageType MT = InMsg->MessageType();
    		delete InMsg;
			if( MT != mtC050 )
			    throw( X25ReconnectException("Confirm subsystem: Not a idle handshake replay message." ));
		}
	}
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmConnectionObject::ClearMessageList()
{
	FMsgCriticalSection.Enter();
	for( register int i = FMessages->ItemCount()-1; i > 0; i-- )
		 delete ( MessageHeader* )FMessages->GetItem( i );
    FMessages->Clear();
	FMsgCriticalSection.Leave();
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TAIFEXConfirmConnectionObject::CheckC010( MessageC010* C010 )
{
	MessageC020* C020;
	int          RequestSeq = C010->GetSeq();

	if( RequestSeq > FEndPosition )
	    RequestSeq = FEndPosition;
	if( RequestSeq < 1 )
		RequestSeq = 1;
	ClearMessageList();
	FSendPosition = RequestSeq - 1;  // Change the sending position to the position that FCM gived.
	FReadPosition = RequestSeq - 1;
	C020 = new MessageC020( C010->GetBrokerID(), RequestSeq );
	return C020;
}
//------------------------------------------------------------------------------------ ----------------------------------
MessageHeader* TAIFEXConfirmConnectionObject::CheckC011( MessageC011* C011 )
{
	MessageC021* C021;
	int          RequestSeq = C011->GetSeq();

	if( RequestSeq > FEndPosition )
	    RequestSeq = FEndPosition;
	if( RequestSeq < 1 )
		RequestSeq = 1;
	ClearMessageList();
	FSendPosition = RequestSeq - 1;  // Change the sending position to the position that FCM gived.
	FReadPosition = RequestSeq - 1;
	C021 = new MessageC021( C011->GetCMID(), RequestSeq );
	return C021;
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmConnectionObject::ConfirmsubSystemHandshake( BOOL ReceivedL010 )
{
	FHandshakeReady = FALSE;
	if( FConnected == TRUE )
	{
		FConnected = FALSE;
		FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
		ClearMessageList();
		FSendPosition  = 0;
 		FReadPosition  = 0;
  	}
	FNeedLock = (!FConnection->OnlineHandshake( ReceivedL010 ));
	if( !FNeedLock )
	{
		MessageHeader*  InMsg;
		MessageHeader*  OutMsg;

		if( (InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT )) != NULL )
		{
			TMessageType MsgType = InMsg->MessageType( );
			if( MsgType == mtC010 || MsgType == mtC011 )
			{
				FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_READY, (void*) &FEndPosition );
				if( FPVCInfo.apcode == '3' )
					OutMsg = CheckC010( static_cast<MessageC010*>(InMsg) );
				else ///< For AP Code 8,9
					OutMsg = CheckC011( static_cast<MessageC011*>(InMsg) );
				delete InMsg;
				FConnection->SendMessage( OutMsg, ORDERSUBSYSTEM_TIMEOUT );
				delete OutMsg;
				FHandshakeReady = TRUE;
				FConnected = TRUE;
				FPreviousTime = ThreadClock::SecondOfToday();
				return;
			}
		}
	}
	else
	{
		FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_LOCKED );// Lock this connection.
		FPreviousTime = ThreadClock::SecondOfToday();
	}
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmConnectionObject::LockLoop( void )
{
	while( FNeedLock == TRUE ) // This PVC had been lock.
	{
		sleep( 2 );
	}
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmConnectionObject::MessageLoop( void )
{
	while(  IsTerminated() == FALSE )
	{
		try
		{
			do
			{
				if( IsTerminated() == TRUE ) // Close this connection.
				    return;
				else if( FNeedLock == TRUE ) // This PVC had been lock.
				{
					sleep( 2 );
				}
				else if( FNeedReset == TRUE ) // Received PVC reset command.
				{
					FNeedReset = FALSE;
					throw( X25Exception( 0, "Received PVC reset.") );
				}
				else                         // Normal state...Process the message.
				{
					if( FHandshakeReady == FALSE )
					 	throw( X25ReconnectException("Confirm subsystem: SendMessage failed." ) );
					if( ThreadClock::SecondOfToday() - FPreviousTime > IDLEHANDSHAKE_TIME_XBIO3 )
					{
						IdleHandshake();    // Idle handshake.
						FPreviousTime = ThreadClock::SecondOfToday();
					}
					else
					{
						if( MessageOut() == FALSE )
						{
							MessageHeader* InMsg; // Handle FCM Send L010.
							if( (InMsg = FConnection->ReceiveMessage( 1L, FALSE )) != NULL )
							{
								TMessageType mt = InMsg->MessageType( );
								delete InMsg;
								if( mt == mtL010 )
									throw( X25RehandshakeException("Received a L010") );
								else
									throw( X25ReconnectException("Reconnect.") );
							}
						}
					}
				}
			} while(  IsTerminated() == FALSE );
		}
		catch( X25RehandshakeException& e ) // Receive a L010...go back to linksubsystem handshake.
		{
			ConfirmsubSystemHandshake( TRUE );
		}
		catch( X25ReconnectException& e )  // Timeout or reconnect
		{
			ConfirmsubSystemHandshake( TRUE );
		}
	}
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmConnectionObject::SetLogManager( LogManager* LogObj )
{
    FLog = LogObj;
    if( FConnection != NULL )
        FConnection->SetLogManager( FLog );
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXConfirmConnectionObject::Execute( void )
{
	FConnection = new LinkSubSystemTAIFEX( FParameters );
    if( FLog != NULL )
        FConnection->SetLogManager( FLog );
	for( int i = 0; i< PVC_BUSY_RETRY_TIMES; i++ )
	{
		try
		{
			if( i == PVC_BUSY_RETRY_TIMES - 1 )
			{
				i = 0;
				FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_X25ERROR );// Can not establish the X.25 connection
				FNeedLock = TRUE;
				LockLoop();
			}
			FConnection->Open( FParameters );
			ConfirmsubSystemHandshake( TRUE );
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
