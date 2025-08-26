#ident "@(#) $Id: FCMFileConnectionObject.cpp,v 1.4 2004/05/21 10:34:20 ap27 Exp $"
#include "FCMFileConnectionObject.h"
//----------------------------------------------------------------------------------------------------------------------
//
//  ### FCM side ###
//  Connection object for AP code 1,2 File subsystem.
//
//----------------------------------------------------------------------------------------------------------------------
FCMFileConnectionObject::FCMFileConnectionObject( ConnectionParameter & info,
												  ConnectionListener * CListener,
												  ThreadListener * TListener,
												  const AnsiString & IPAddress,
												  Int32 Port )
:ConnectionObjectBase( info )
,PThread( TListener, TRUE )
,FHandshakeReady( FALSE )
,FPVCLock( FALSE )
,FSendMsg( FALSE )
,FRequest( FALSE )
,FBroken( FALSE )
,FIPAddress( IPAddress )
,FPort( Port )
,FSeqNumber( 1 )
,FFileData( NULL )
,FConnectionListener( CListener )
,FSocketClient( NULL )
{
	FAPCode = info.GetAPCode();
	//	if( FAPCode == '1' )
	//		OpenActionFile( );
	
	if ( FIPAddress.Length() > 0 )
		FSocketClient = new PClientSocket( FIPAddress, FPort );
	FSocketClient->SetListener( this );
	/* AnsiString LogFileName;
	
	LogFileName.Printf( "gConfirm.%d.%d.log", FParameters.GetLink(), FParameters.GetPVC() );
	FLogFile  = new LogFile( LogFileName.c_str(), "w"); */
	
	Start();
}

//----------------------------------------------------------------------------------------------------------------------
FCMFileConnectionObject::~FCMFileConnectionObject()
{
  //	delete FLogFile;
  if ( FSocketClient != NULL )
    delete FSocketClient;
}

//----------------------------------------------------------------------------------------------------------------------
void FCMFileConnectionObject::OnConnect( PClientSocket * Socket )
{
  RegData RegisterData;

  RegisterData.APCode = FParameters.GetAPCode();
  strcpy( RegisterData.BrokerID, FParameters.GetBrokerID().c_str() );
  RegisterData.IsHistroy = 1;
  SendPackage( COMMAND_REGISTER, sizeof( RegData ), & RegisterData );
}

//----------------------------------------------------------------------------------------------------------------------
BOOL FCMFileConnectionObject::OnDataArrived( PClientSocket * Socket )
{
  DataHeader ReceiveData( Socket );
  switch ( ReceiveData.GetCommand() )
  {
    case COMMAND_START:
    case COMMAND_STOP:
    case COMMAND_PAUSE:
    break;
    case COMMAND_X25_BROKEN:
      FSeqNumber = atoi( ReceiveData.GetData() );
      FBroken = TRUE;
    break;
    case COMMAND_TURNON_MSG:
      FSendMsg = TRUE;
    break;
    case COMMAND_TURNOFF_MSG:
      FSendMsg = FALSE;
    break;
    case COMMAND_FILE_TRANS:
      {
        if ( !( ReceiveData.GetData() == NULL ) )
        {
          FRequest = TRUE;
          FFileData = ( FileData * ) ReceiveData.GetData();
        }
      break;

      }

  }
}

//---------------------------------------------------------------------------------------------------------------------
void FCMFileConnectionObject::ConnectToServer()
{
  if ( FSocketClient == NULL )
    return;
  while ( FSocketClient->IsConnect() != TRUE )
  {
    FSocketClient->Connect();
    sleep( 2 );
  }
}

//---------------------------------------------------------------------------------------------------------------------
BOOL FCMFileConnectionObject::SendPackage( unsigned char CMD, int Size, void * Data )
{
  if ( FSocketClient == NULL )
    return FALSE;
  DataHeader RegHeader( CMD, ( unsigned char ) FParameters.GetLink(), ( unsigned char ) FParameters.GetPVC(), Size, Data );
  return RegHeader.Serialize( FSocketClient );
}

//---------------------------------------------------------------------------------------------------------------------
void FCMFileConnectionObject::SendSocketMessage( BOOL IsSend, MessageHeader * Msg )
{
  char Buffer[1024];

  if ( IsSend )
    strcpy( Buffer, "Sent:" );
  else
    strcpy( Buffer, "Received:" );
  Msg->AsString( Buffer + strlen( Buffer ) );
  SendPackage( COMMAND_MESSAGE, strlen( Buffer ), Buffer );
}

//----------------------------------------------------------------------------------------------------------------------
void FCMFileConnectionObject::FileSubSystemHandshake( BOOL ReceivedL010 )
{
  if ( FConnection->OnlineHandshake( ReceivedL010 ) )
  {
    FHandshakeReady = TRUE;
    FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_READY );
    SendPackage( COMMAND_X25_READY );
    return;
  }
  FHandshakeReady = FALSE;
  FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
  SendPackage( COMMAND_X25_BROKEN );
}

//----------------------------------------------------------------------------------------------------------------------
void FCMFileConnectionObject::ChekcReceiveMessage()
{
  MessageHeader * InMsg;
  if ( ( InMsg = FConnection->ReceiveMessage( 1L, FALSE ) ) != NULL )
  {
    FMessageHeader * InFMsg = static_cast < FMessageHeader * > ( InMsg );
    int MsgType = InMsg->MessageType();
    switch ( MsgType )
    {
      case mtL010:
        {
          delete InMsg;
          throw( X25RehandshakeException( "Rehandshake" ) );
        }
      case mtF170:
        {
          ShowMessage( false, InFMsg );
          ReplyConfirmMessage( InFMsg );
        break;
        }
      default:
        delete InMsg;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FCMFileConnectionObject::ShowMessage( bool IsSend, FMessageHeader * InFMsg )
{
  if ( FSendMsg )
    SendSocketMessage( IsSend, InFMsg );
  else
    InFMsg->print();
}

//----------------------------------------------------------------------------------------------------------------------
void FCMFileConnectionObject::ReplyConfirmMessage( FMessageHeader * InFMsg )
{
  ShowMessage( false, InFMsg );
  InFMsg->SwapID();
  InFMsg->SetMessageType( ( InFMsg->GetMessageType() ) + 1 );
  if ( FConnection->SendMessage( InFMsg, ORDERSUBSYSTEM_TIMEOUT ) != TRUE )
    throw( X25ReconnectException( "File subsystem: SendConfirmMessage failed." ) );
  ShowMessage( true, InFMsg );
  delete InFMsg;
}

//----------------------------------------------------------------------------------------------------------------------
void FCMFileConnectionObject::ReplyMessageF040( FMessageHeader * InFMsg )
{
  ShowMessage( false, InFMsg );
  MessageF030 * F030 = static_cast < MessageF030 * > ( InFMsg );
  MessageF040 * F040 = new MessageF040( F030->GetDestinationID(), F030->GetSourceID(), F030->GetFileCode(), F030->GetEOF() );
  if ( FConnection->SendMessage( F040, ORDERSUBSYSTEM_TIMEOUT ) != TRUE )
    throw( X25ReconnectException( "File subsystem: SendF040Message failed." ) );
  ShowMessage( true, F040 );
  delete F040;
  delete InFMsg;
}

//----------------------------------------------------------------------------------------------------------------------
void FCMFileConnectionObject::SendMessageF030( FMessageHeader * InFMsg )
{
  AnsiString FStrBuffer;
  int FCurPos;
  char FFileEOF;

  FFileEOF = '0';

  FCurPos = FDataFile->Seek( 0, soFromCurrent );
  FStrBuffer = FDataFile->ReadLine();
  FStrBuffer = FDataFile->ReadLine();
  if ( FStrBuffer == NULL )
    FFileEOF = '1';
  FDataFile->Seek( FCurPos, soFromBeginning );
  FStrBuffer = FDataFile->ReadLine();

  MessageF020 * F020 = static_cast < MessageF020 * > ( InFMsg );
  ShowMessage( false, F020 );
  MessageF030 * F030 = new
       MessageF030( F020->GetDestinationID(), F020->GetSourceID(), FMessageHeader::GetDefMsgLen( F020->GetFileCode() ), F020->GetFileCode(),
       FFileEOF, ( char * ) FStrBuffer.c_str() );
  if ( FConnection->SendMessage( F030, ORDERSUBSYSTEM_TIMEOUT ) != TRUE )
    throw( X25ReconnectException( "File subsystem: SendF030Message failed." ) );
  ShowMessage( true, F030 );
  delete F030;
  delete InFMsg;
}

//----------------------------------------------------------------------------------------------------------------------
void FCMFileConnectionObject::SendMessageF070( FMessageHeader * InFMsg )
{
  MessageF070 * F070 = new MessageF070( InFMsg->GetDestinationID(), InFMsg->GetSourceID() );
  ShowMessage( false, F070 );
  if ( FConnection->SendMessage( F070, ORDERSUBSYSTEM_TIMEOUT ) != TRUE )
    throw( X25ReconnectException( "File subsystem: SendF070Message failed." ) );
  ShowMessage( true, InFMsg );
  delete F070;
  delete InFMsg;
}

//----------------------------------------------------------------------------------------------------------------------
void FCMFileConnectionObject::SendFile( char * FileCode, char * FileName )
{
  int FPreviousMsgType;
  int FFileSize;

  FDataFile = new FileStream( AnsiString( FileName ), 'r' );
  if ( ( FFileSize = FDataFile->Seek( 0, soFromEnd ) ) == 0 )
  {
    printf( "File:%s no data !\n", FileName );
    return;
  }
  FDataFile->Seek( 0, soFromBeginning );
  MessageF010 * F010 = new MessageF010( ( char * ) FParameters.GetBrokerID().c_str(), "1111111", FileCode, FFileSize );
  if ( FConnection->SendMessage( F010, ORDERSUBSYSTEM_TIMEOUT ) != TRUE )
    throw( X25ReconnectException( "File subsystem: SendF010Message failed." ) );
  ShowMessage( false, F010 );
  delete F010;
  FPreviousMsgType = mtF010;
  while ( TRUE )
  {
    MessageHeader * InMsg;
    if ( ( InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT ) ) != NULL )
    {
      FMessageHeader * InFMsg = static_cast < FMessageHeader * > ( InMsg );
      int MsgType = InMsg->MessageType();
      if ( MsgType == mtL010 )
      {
        throw( X25RehandshakeException( "Rehandshake" ) );
      }
      if ( MsgType == mtF170 )
      {
        ReplyConfirmMessage( InFMsg );
      }
      else
      {
        if ( FPreviousMsgType != ( MsgType - 1 ) )
          throw( X25ReconnectException( "File subsystem: ReceiveMessage failed." ) );
        switch ( MsgType )
        {
          case mtF020:
            {
              SendMessageF030( InFMsg );
              FPreviousMsgType = mtF030;
            break;
            }
          case mtF040:
            {
              MessageF040 * F040 = static_cast < MessageF040 * > ( InFMsg );
              if ( F040->GetEOF() == 0 )
              {
                SendMessageF030( InFMsg );
                FPreviousMsgType = mtF030;
              }
              else
              {
                SendMessageF070( InFMsg );
                FPreviousMsgType = mtF070;
              }
            break;
            }
          case mtF080:
            {
              ShowMessage( false, InFMsg );
              FPreviousMsgType = 0;
              return;
            }
          default:
            throw( X25ReconnectException( "File subsystem: ReceiveMessage failed." ) );
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FCMFileConnectionObject::SendRequest()
{
  char * RequestData = NULL;

  strncat( RequestData, FFileData->StartSeq, 8 );
  strncat( RequestData, FFileData->EndSeq, 8 );
  strncat( RequestData, FFileData->BrokerID, 7 );
  strcat( RequestData, "\0" );
  MessageF050 * F050 = new
       MessageF050( ( char * ) FParameters.GetBrokerID().c_str(), "1111111", FMessageHeader::GetDefMsgLen( FFileData->FileCode ),
       FFileData->FileCode, RequestData );
  if ( FConnection->SendMessage( F050, ORDERSUBSYSTEM_TIMEOUT ) != TRUE )
    throw( X25ReconnectException( "File subsystem: SendF050Message failed." ) );
  ShowMessage( false, F050 );
  delete F050;
  while ( TRUE )
  {
    MessageHeader * InMsg;
    if ( ( InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT ) ) != NULL )
    {
      FMessageHeader * InFMsg = static_cast < FMessageHeader * > ( InMsg );
      int MsgType = InMsg->MessageType();
      switch ( MsgType )
      {
        case mtL010:
          {
            throw( X25RehandshakeException( "Rehandshake" ) );
          }
        case mtF060:
          {
            ShowMessage( false, InFMsg );
            FPVCLock = FALSE;
            delete InMsg;
            return;
          }
        case mtF170:
          {
            ReplyConfirmMessage( InFMsg );
          break;
          }
        default:
          {
            printf( "PVC:%d Got UnKnow OK.\n", FParameters.GetPVC() );
            throw( X25ReconnectException( "File subsystem: ReceiveMessage failed." ) );
          }
      }
    }
    delete InMsg;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FCMFileConnectionObject::MessageLoop( void )
{
  AnsiString StrBuffer;

  while ( TRUE )
  {
    try
    {
      do
      {
        if ( IsTerminated() == TRUE ) // Close this connection.
          return;
        else // Normal state...Process the message.
        {
          if ( FHandshakeReady == FALSE )
            throw( X25ReconnectException( "File subsystem: SendMessage failed." ) );
          if ( FBroken == TRUE )
          {
            FBroken = FALSE;
            throw( X25ReconnectException( "Asked to rehandshake." ) );
          }
          if ( FAPCode == '2' )
          {
            MessageHeader * InMsg;
            if ( ( InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT ) ) != NULL )
            {
              int MsgType = InMsg->MessageType();
              FMessageHeader * InFMsg = static_cast < FMessageHeader * > ( InMsg );

              switch ( MsgType )
              {
                case mtL010:
                  {
                    throw( X25RehandshakeException( "Rehandshake" ) );
                  }
                case mtF010:
                case mtF050:
                case mtF070:
                case mtF170:
                  ReplyConfirmMessage( InFMsg );
                break;
                case mtF030:
                  ReplyMessageF040( InFMsg );
                break;
                default:
                  throw( X25ReconnectException( "File subsystem: ReceiveMessage failed." ) );
              }
            }
            delete InMsg;
          }
          else
          {
            ChekcReceiveMessage();
            if ( FRequest )
            {
              SendRequest();
              /* MessageHeader* Msg = new MessageHeader( "20", (char*)StrBuffer.SubString( 7, 2 ).c_str(),
              (char*)StrBuffer.SubString( 9, 2 ).c_str() );

              int MsgType = Msg->MessageType(); switch(MsgType) { case mtF010: SendFile( (char*)StrBuffer.SubString( 11, 3
              ).c_str(), (char*)StrBuffer.SubString( 14, 989 ).c_str() ); break;
              case mtF050: SendRequest( (char*)StrBuffer.SubString( 11, 3 ).c_str(), (char*)StrBuffer.SubString( 14, 989
              ).c_str() ); break; } delete Msg; */
            }
          }
        }
      }
      while ( TRUE );
    }
    catch ( X25RehandshakeException & e ) // Receive a L010...go back to linksubsystem handshake.
    {
      SendPackage( COMMAND_X25_BROKEN );
      printf( "----Rehandshake Exception\n" );
      FileSubSystemHandshake( TRUE );
      SendPackage( COMMAND_X25_READY );
    }
    catch ( X25ReconnectException & e ) // Timeout or reconnect
    {
      SendPackage( COMMAND_X25_BROKEN );
      printf( "----Reconnect Exception\n" );
      FileSubSystemHandshake( FALSE );
      SendPackage( COMMAND_X25_READY );
    }
    catch ( FileException & e )
    {
      printf( "----File Exceptions\n" );
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FCMFileConnectionObject::Execute( void )
{
  printf( "ConnectObject PVC:%d Run.\n", FParameters.GetPVC() );

  for ( int i = 0; i < PVC_BUSY_RETRY_TIMES; i++ )
  {
    try
    {
      FConnection = new LinkSubSystemFCM( FParameters );
      ConnectToServer();
      FileSubSystemHandshake( FALSE );
      MessageLoop();
      delete FConnection;
      FConnection = NULL;
      FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_NORMAL_CLOSE ); // Close the connection normaly.
      return;
    }
    catch ( X25Exception & e )
    {
      if ( FConnection != NULL )
      {
        delete FConnection;
        FConnection = NULL;
      }
      printf( "%s. Try to reconnect after 5 sec....\n", e.what() );
      sleep( 5 );
      continue;
    }
    catch ( exception & e )
    {
      FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR ); // Unknown exception.
      return;
    }
  }
  FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_X25ERROR ); // Can not establish the X.25 connection
  return;
}
//----------------------------------------------------------------------------------------------------------------------
