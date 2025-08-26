#include "FileConnectionObject.h"
#include "LinkSubSystem.h"
//------------------------------------------------------------------------------
extern BOOL IsOptions;

//------------------------------------------------------------------------------
TAIFEXFileConnectionObject::TAIFEXFileConnectionObject( ConnectionParameter & info, ConnectionListener * CListener, ThreadListener * TListener )
:ConnectionObjectBase( info )
,FConnectionListener( CListener )
,PThread( TListener, TRUE )
,FNeedLock( FALSE )
,FNeedReset( FALSE )
,FSendMsg( FALSE )
,FHandshakeReady( FALSE )
,FFile( NULL )
,FReplyType( 0x2A )
,FIdleTime( 0 )
{
     AnsiString LinkStr;
     LinkStr.Printf( "%06d", info.GetLink() );
     strncpy( FPVCInfo.line, LinkStr.c_str(), 6 );
     strncpy( FPVCInfo.brokerid, info.GetBrokerID().c_str(), 7 );
     FApCode = info.GetAPCode();
     FPVCInfo.apcode = '3';
     FPVCInfo.pvc = info.GetPVC();
}
//------------------------------------------------------------------------------
TAIFEXFileConnectionObject::~TAIFEXFileConnectionObject( void )
{
    FConnection->StopHandshake();
    if ( IsTerminated() == FALSE )
    {
        Terminate(); ///< Terminate the thread.
        WaitFor(); ///< Wait for thread terminated.
    }
}

//----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::LockLoop( void )
{
    while ( FNeedLock == TRUE && IsTerminated() == FALSE ) // This PVC had been lock.
    {
        sleep( 2 );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::OnCommand( int Command, void * Data )
{
    switch ( Command )
    {
        case COMMAND_OPEN_MARKET:
        break;
        case COMMAND_CLOSE_MARKET:
            Terminate();
        break;
        case COMMAND_UNLOCK_PVC:
            FNeedLock = FALSE;
            if ( FHandshakeReady == TRUE )
                FNeedReset = TRUE;
        break;
        case COMMAND_LOCK_PVC:
            FNeedLock = TRUE;
        break;
        case COMMAND_GET_STATE:
            if ( FConnection != NULL )
                * ( ( int * ) Data ) = FConnection->GetStatus();
        break;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::UpdateConnectionParameter( ConnectionParameter & Info )
{
    FParameters = Info;
    FNeedLock = FALSE;
    FNeedReset = TRUE;
}

//------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::Execute( void )
{
    FConnection = new LinkSubSystemTAIFEX( FParameters );
    for ( int i = 0; i < PVC_BUSY_RETRY_TIMES; i++ )
    {
        try
        {
            if ( i == PVC_BUSY_RETRY_TIMES - 1 )
            {
                FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_X25ERROR ); // Can not establish the X.25 connection
                i = 0;
                FNeedLock = TRUE;
                LockLoop();
            }
            FConnection->Open( FParameters );
            FileSubSystemHandshake( FALSE );
            MessageLoop();
            FConnection->Close();
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_NORMAL_CLOSE ); // Close the connection normaly.
            break;

        }
        catch ( X25Exception & e )
        {
            UFC::BufferedLog::Printf( " %s. Try to reconnect after 5 sec...", e.what() );
            FConnection->Close();
            sleep( 5 );
            continue;
        }
        catch ( exception & e )
        {
            UFC::BufferedLog::Printf( " PVC %d Exception:%s", GetConnectionParameter()->GetPVC(), e.what() );
            FNeedLock = TRUE;
            LockLoop();
        }
        catch ( ... )
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR ); // Unknown exception.
            FNeedLock = TRUE;
            LockLoop();
        }

    }
    delete FConnection;
    FConnection = NULL;
}
//------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::FileSubSystemHandshake( BOOL ReceivedL010 )
{
    CloseWork();
    FHandshakeReady = FALSE;
    FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
    FNeedLock = ( !FConnection->OnlineHandshake( ReceivedL010 ) );
    if ( FNeedLock == TRUE )
    {
        FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_LOCKED ); // Lock this connection.
    }
    else
    {
        FHandshakeReady = TRUE;
        FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_READY );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::MessageLoop( void )
{
    AnsiString StrBuffer;

    while ( TRUE )
    {
        try
        {
            do
            {
                if ( IsTerminated() == TRUE ) ///< Close this connection.
                    return;
                else if ( FNeedLock == TRUE ) ///< This PVC had been locked.
                    sleep( 2 );
                else if ( FNeedReset == TRUE ) ///< Received PVC reset command.
                {
                    FNeedReset = FALSE;
                    throw( X25Exception( 0, "Received PVC reset." ) );
                }
                else ///<  Normal state...Process the message.
                {
                    if ( FApCode == '1' )
                        Responser();
                    else
                        Initiator();
                }
                sleep( 1 );
            }
            while ( TRUE );
        }
        catch ( X25RehandshakeException & e ) // Receive a L010...go back to linksubsystem handshake.
        {
            printf( "----%s\n", e.what() );
            FileSubSystemHandshake( TRUE );
        }
        catch ( X25ReconnectException & e ) // Timeout or reconnect
        {
            printf( "----%s\n", e.what() );
            FileSubSystemHandshake( FALSE );
        }
        catch ( FileException & e )
        {
            printf( "----%s\n", e.what() );
        }
        fflush( stdout );
    }
}

//----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::Responser( void )
{
    MessageHeader * InMsg;

    fflush( stdout );
    if ( ( InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT, FALSE ) ) != NULL )
    {
        int MsgType = InMsg->MessageType();
        FMessageHeader * InFMsg = static_cast < FMessageHeader * > ( InMsg );

        switch ( MsgType )
        {
            case mtL010: throw( X25RehandshakeException( "Rehandshake" ) );
            case mtF010:
            case mtF070:
            case mtF170: ReplyConfirmMessage( InFMsg );
                         break;
            case mtF030: ReplyMessageF040( InFMsg );
                         break;
            case mtF050: ReplyMessageF060( InFMsg );
                         break;
            default:     throw( X25ReconnectException( "File subsystem: Receive unknown message." ) );
        }
        delete InMsg;
        InMsg = NULL;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::Initiator( void )
{
    if ( FSendMsg == TRUE )
    {
        ChekcReceiveMessage();
        return;
    }
    FMessageHeader* FMsg = GetNewJob();
    if ( FMsg != NULL )
    {
        switch( FMsg->MessageType() )
        {
            case mtF050: Work( FMsg );
                         return;
            case mtF170: SendF170CheckConnection( FMsg );
                         return;
        }
    }
    FIdleTime++;
    if( FIdleTime >= 55 )
    {
        MessageF170 F170( "F000000", FPVCInfo.brokerid );
        FConnection->SendMessage( &F170, ORDERSUBSYSTEM_TIMEOUT );
        FIdleTime = 0;
        MessageHeader * InMsg= FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT, FALSE );
        if( InMsg != NULL )
        {
            if( InMsg->MessageType() != mtF180 )
            {
                delete InMsg;
                throw( X25ReconnectException( "File subsystem: Can't receive heartbeat." ) );
            }
            delete InMsg;
        }
        else
            throw( X25ReconnectException( "File subsystem: Receive heartbeat timeout." ) );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::ReplyConfirmMessage( FMessageHeader * InFMsg )
{
    InFMsg->SwapID();
    InFMsg->SetMessageType( ( InFMsg->GetMessageType() ) + 1 );
    if ( FConnection->SendMessage( InFMsg, ORDERSUBSYSTEM_TIMEOUT ) != TRUE )
    {
        delete InFMsg;
        InFMsg = NULL;
        throw( X25ReconnectException( "File subsystem: SendConfirmMessage failed." ) );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::ReplyMessageF040( FMessageHeader * InFMsg )
{
    MessageF030 * F030 = static_cast < MessageF030 * > ( InFMsg );
    FSourceID = F030->GetDestinationID();
    FDestinationID = F030->GetSourceID();
    FFileCode = F030->GetFileCode();
    MessageF040 * F040 = new MessageF040( ( char * ) FSourceID.c_str(), ( char * ) FDestinationID.c_str(), ( char * ) FFileCode.c_str(), F030->GetEOF() );
    if ( FConnection->SendMessage( F040, ORDERSUBSYSTEM_TIMEOUT ) != TRUE )
    {
        delete InFMsg;
        InFMsg = NULL;
        delete F040;
        throw( X25ReconnectException( "File subsystem: SendF040Message failed." ) );
    }
    delete F040;
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::ReplyMessageF060( FMessageHeader * InFMsg )
{
    MessageF050 * F050 = static_cast < MessageF050 * > ( InFMsg );
    FFileCode = F050->GetFileCode();
    FReplyType = F050->GetReplyType();
    FStartSequence = F050->GetStartSequence();
    FEndSequence = F050->GetEndSequence();
    FRequestID = F050->GetRequestID();
    Int32 stCode = 0;
    CheckFileStatus( & stCode );

    if ( stCode == 0 )
    {
        printf( "responser(%d) reply f060 msg.\n", FParameters.GetPVC() );
        fflush( stdout );
        FConnectionListener->OnConnectionNotify( this, CONNECTION_RECV_MSG_F050, ( void * ) InFMsg );
    }
    else
        InFMsg->SetStatusCode( stCode );

    ReplyConfirmMessage( InFMsg );
}
//----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::ChekcReceiveMessage( void )
{
    MessageHeader * InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT, TRUE );
    if ( InMsg != NULL )
    {
        FMessageHeader * InFMsg = static_cast < FMessageHeader * > ( InMsg );
        switch ( CheckStatusCodeOfReceivedMessage( InFMsg ) )
        {
            case 0:
                switch ( InFMsg->MessageType() )
                {
                    case mtL010:
                        delete InMsg;
                        throw( X25RehandshakeException( "receive message L010." ) );

                    case mtF020:
                        ReplyMessageF030( InFMsg );
                        break;

                    default:
                        break;
                }
                break;

            case -2:
                OffLine( InFMsg->GetDestinationID(), InFMsg->GetSourceID() );
                break;

            default:
                break;
        }

        delete InMsg;
    }

    CloseWork();
}

//----------------------------------------------------------------------------------------------------------------------
int TAIFEXFileConnectionObject::CheckStatusCodeOfReceivedMessage( FMessageHeader * FMsg )
{
    if ( FMsg != NULL )
    {
        int stCode = FMsg->GetStatusCode();
        if ( stCode == 0 )
            return 0;
        if ( stCode >= 80 )
            return -2;
    }
    return -1;
}

//----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::Work(FMessageHeader* FMsg)
{
    printf( "initiator(%d) start work.\n", FParameters.GetPVC() );
    fflush( stdout );

    MessageF050* F050 = static_cast < MessageF050* > ( FMsg );
    FSourceID = F050->GetDestinationID();
    FDestinationID = F050->GetSourceID();
    FFileCode = F050->GetFileCode();
    FReplyType = F050->GetReplyType();
    FStartSequence = F050->GetStartSequence();
    FEndSequence = F050->GetEndSequence();
    FRequestID = F050->GetRequestID();
    delete F050;

    Int32 stCode = 0;
    Int32 FileSize = CheckFileStatus( & stCode );
    MessageF010 * F010 = new MessageF010( ( char * ) FSourceID.c_str(), ( char * ) FDestinationID.c_str(), ( char * ) FFileCode.c_str(), FileSize );
    F010->SetStatusCode( stCode );
    FSendMsg = SendFMessage( F010 );
}

//----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::CloseWork( void )
{
    FReplyType = 0x2A;
    FStartSequence = -1;
    FEndSequence = -1;
    FRequestID = "";
    FSourceID = "";
    FDestinationID = "";
    FFileCode = "";
    FSendMsg = FALSE;
}

//----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::ReplyMessageF030( FMessageHeader * InFMsg )
{
    printf( "initiator(%d) reply f030.\n", FParameters.GetPVC() );
    fflush( stdout );

    MessageF020 * F020 = static_cast < MessageF020 * > ( InFMsg );
    FSourceID = F020->GetDestinationID();
    FDestinationID = F020->GetSourceID();
    FFileCode = F020->GetFileCode();
    Int32 LeftSize = F020->GetFileSize();

    char * tBuffer = new char[LeftSize];
    ReadFileData( tBuffer, LeftSize );

    Int32 Pos = 0;
    MessageF030 * F030 = NULL;
    Int32 SendSize = 988;
    char FileEOF = '0';
    char Buffer[988];
    do
    {
        if ( LeftSize <= 988 )
        {
            SendSize = LeftSize;
            FileEOF = '1';
        }

        memset( Buffer, 0, 988 );
        memcpy( Buffer, tBuffer + Pos, SendSize );
        F030 = new MessageF030( ( char * ) FSourceID.c_str(), ( char * ) FDestinationID.c_str(), SendSize + 4, ( char * ) FFileCode.c_str(), FileEOF, Buffer );
        SendFMessage( F030 );
        if ( ReceiveF040() == FALSE )
            break;

        Pos += SendSize;
        LeftSize -= SendSize;
        printf( "LeftSize = %d\n", LeftSize );
        fflush( stdout );

    }
    while ( FileEOF == '0' );

    delete[] tBuffer;

//    MessageF070 * F070 = new MessageF070( ( char * ) FSourceID.c_str(), ( char * ) FDestinationID.c_str() );
//    FConnection->SendMessage( F070, ORDERSUBSYSTEM_TIMEOUT );
//    delete F070;

//    MessageHeader * InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT, FALSE );
//    delete InMsg;
}

//----------------------------------------------------------------------------------------------------------------------
BOOL TAIFEXFileConnectionObject::ReceiveF040( void )
{
    printf( "initiator(%d) receive f040\n", FParameters.GetPVC() );
    fflush( stdout );

    BOOL Result = FALSE;
    MessageHeader * InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT );
    if ( InMsg != NULL )
    {
        FMessageHeader * InFMsg = static_cast < FMessageHeader * > ( InMsg );
        if ( CheckStatusCodeOfReceivedMessage( InFMsg ) == 0 )
        {
            if ( InFMsg->MessageType() == mtF040 )
            {
                MessageF040 * F040 = static_cast < MessageF040 * > ( InFMsg );
                if ( F040->GetEOF() == 0 )
                    Result = TRUE;
            }
        }
        delete InMsg;
    }
    return Result;
}

//----------------------------------------------------------------------------------------------------------------------
Int32 TAIFEXFileConnectionObject::CheckFileStatus( Int32 * stCode )
{
    Int32 FileSize = 0;

    try
    {
        if ( FFile != NULL )
            delete FFile;

        if ( FFileCode == "R30" )
        {
            FileSize = ReadR30Data( NULL, 0 );
        }
        else if ( FFileCode == "R40" )
        {
            FileSize = ReadR40Data( NULL, 0 );
        }
        else if ( FFileCode == "R50" )
        {
            FileSize = ReadR50Data( NULL, 0 );
        }
        else
        {
            AnsiString fPath = FFolderName + FFileCode;
            FFile = new FileStream( fPath, O_RDONLY );
            FileSize = FFile->GetSize();
        }
    }
    catch ( ... )
    {
        FileSize = -1;
    }

    if ( FFile != NULL )
    {
        delete FFile;
        FFile = NULL;
    }

    if ( stCode != NULL )
    {
        if ( FileSize < 0 )
            * stCode = 14;
        else if ( FileSize == 0 )
            * stCode = 17;
        else
            * stCode = 0;
    }

    return FileSize;
}

//----------------------------------------------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::AddNewJob(FMessageHeader* FMsg)
{
    FCS.Enter();
    FJobs.Add( (void*) FMsg );
    FCS.Leave();
}

//----------------------------------------------------------------------------------------------------------------------
FMessageHeader* TAIFEXFileConnectionObject::GetNewJob( void )
{
    FMessageHeader* FMsg = NULL;
    FCS.Enter();
    if ( FJobs.ItemCount() > 0 )
    {
        FMsg = (FMessageHeader*) ( FJobs.GetItem( 0 ) );
        FJobs.Delete( 0 );
    }
    FCS.Leave();
    return FMsg;
}

//------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::SendF170CheckConnection(FMessageHeader* FMsg)
{
    SendFMessage( FMsg );
    MessageHeader* InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT );
    int MsgType = InMsg->MessageType();
    delete InMsg;
    CloseWork();

    if( MsgType != mtF180 )
        throw( X25RehandshakeException( "not F180 message." ) );
}

//------------------------------------------------------------------------------
BOOL TAIFEXFileConnectionObject::SendFMessage( FMessageHeader * FMsg )
{
    FIdleTime = 0;
    BOOL Result = FALSE;
    if ( FMsg != NULL )
    {
        Result = FConnection->SendMessage( FMsg, ORDERSUBSYSTEM_TIMEOUT );
        delete FMsg;
        FMsg = NULL;

        if ( Result != TRUE )
            throw( X25RehandshakeException( "send message timeout." ) );
    }
    return Result;
}

//------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::OffLine( const AnsiString & SourceID, const AnsiString & DestinationID )
{
    CloseWork();

    Terminate();
}

//------------------------------------------------------------------------------
void TAIFEXFileConnectionObject::ReadFileData( char * Buffer, Int32 Size )
{
    try
    {
        if ( FFile != NULL )
            delete FFile;
        if ( FFileCode == "R30" )
            ReadR30Data( Buffer, Size );
        else if ( FFileCode == "R40" )
            ReadR40Data( Buffer, Size );
        else if ( FFileCode == "R50" )
            ReadR50Data( Buffer, Size );
        else
        {
            AnsiString fPath = FFolderName + FFileCode;
            FFile = new FileStream( fPath, O_RDONLY );
            FFile->Read( Buffer, Size );
        }
        for ( int i = 0; i < Size; i++ )
        {
            if ( Buffer[i] == 0 )
                Buffer[i] = 0x20;
        }
    }
    catch ( ... )
    {
        memset( Buffer, 0, Size );
    }
    if ( FFile != NULL )
    {
        delete FFile;
        FFile = NULL;
    }
}

//------------------------------------------------------------------------------
Int32 TAIFEXFileConnectionObject::ReadR30Data( char * Buffer, Int32 Size )
{
    printf( "read r30 data\n" );
    fflush( stdout );
    return ReadRData( Buffer, Size, '3' );
}

//------------------------------------------------------------------------------
Int32 TAIFEXFileConnectionObject::ReadR40Data( char * Buffer, Int32 Size )
{
    printf( "ReplyType = %c\n", FReplyType );
    fflush( stdout );
    if ( IsOptions == FALSE )
    {
        if ( FReplyType == 'o' || FReplyType == 'O' )
            return ReadR50Data( Buffer, Size );
        else if ( FReplyType != 'm' && FReplyType != 'M' )
            return 0;
    }

    printf( "read r40 data\n" );
    fflush( stdout );
    return ReadRData( Buffer, Size, '9' );
}

//------------------------------------------------------------------------------
Int32 TAIFEXFileConnectionObject::ReadR50Data( char * Buffer, Int32 Size )
{
    printf( "read r50 data\n" );
    fflush( stdout );
    return ReadRData( Buffer, Size, '8' );
}

//------------------------------------------------------------------------------
Int32 TAIFEXFileConnectionObject::ReadRData( char * Buffer, Int32 Size, char ApCode )
{
    CCH_DF_Reader * FCCHReader = new CCH_DF_Reader();
    FPVCInfo.apcode = ApCode;
    RPTLOG * ReportBuffer = new RPTLOG[FEndSequence - FStartSequence + 1];
    Int32 ReportCount = FCCHReader->Read_IDR( ReportBuffer, & FPVCInfo, FStartSequence, FEndSequence );
    if ( ReportCount > 0 )
    {
        if ( Buffer != NULL && Size > 0 )
        {
            if ( ( ReportCount * 127 ) > Size )
                ReportCount = Size / 127;
            C03XBody * NewMsg = NULL;
            memset( Buffer, 0, Size );
            Int32 Pos = 0;
            for ( Int32 i = 0; i < ReportCount; i++ )
            {
                NewMsg = C03XBody::CreateFromRPTLog( & ReportBuffer[i], FPVCInfo.apcode, FALSE );
                NewMsg->AsString( Buffer + Pos );
                delete NewMsg;
                Pos += 127;
            }
        }
    }
    delete[] ReportBuffer;
    delete FCCHReader;
    return ( ReportCount * 127 );
}
//------------------------------------------------------------------------------
