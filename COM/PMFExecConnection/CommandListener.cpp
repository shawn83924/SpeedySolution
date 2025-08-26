#include "NetUtility.h"
#include "CommandListener.h"
//---------------------------------------------------------------------------
const UFC::AnsiString CMD_HTBT = "HTBT";
const UFC::AnsiString CMD_SVON = "SVON";
const UFC::AnsiString CMD_SVOF = "SVOF";
const UFC::AnsiString CMD_RESD = "RESD";
const UFC::AnsiString CMD_DASD = "DASD";
//------------------------------------------------------------------------------
const int  HEADER_SIZE   = 26;
const int  RESERVED_SIZE = 10;
const char RESERVED_CHAR = 0x20;
//------------------------------------------------------------------------------
TCommandListener::TCommandListener( const UFC::AnsiString& RemoteIP, int RemotePort, int ClientHeartbeatInterval, int TandemHeartbeatInterval, TMessageStore* pMessageStore )
:UFC::PThread( NULL )
,FRemoteIP( RemoteIP )
,FRemotePort( RemotePort )
,FMessageStore( pMessageStore )
,FReconnectTimes( 0 )
,FClientHeartbeatInterval( ClientHeartbeatInterval )
,FTandemHeartbeatInterval( TandemHeartbeatInterval )
,FClientIdleSec( 0 )
,FServerIdleSec( 0 )
{
    Start();
    FSocket.SetListener(this);
}
//---------------------------------------------------------------------------
TCommandListener::~TCommandListener( void )
{
    Terminate();
    WaitFor();
}
//---------------------------------------------------------------------------
void TCommandListener::Execute( void )
{
    while( !IsTerminated() )
    {
        if( FSocket.IsConnect() )
            HTBT();
        else
            Reconnect();

        sleep( 1 );
    }
}
//---------------------------------------------------------------------------
// *when the realreport client sends the data to the tandem, sequence number
//  must be given to each data message so that it can be recovered from the
//  fault.
//
// DASD
// |----------------|---------------|--------------|--------------------|----------------|-----------|
// | message length | message type  | sequence no  | latest sequence no | reserved       | user data |
// | int (4 byte)   | char (4 byte) | int (4 byte) | int (4 byte)       | char (10 byte) | char      |
// |----------------|---------------|--------------|--------------------|----------------|-----------|
//---------------------------------------------------------------------------
void TCommandListener::OnNewReport( UFC::AnsiString* Message, int Seq )
{
    try
    {        
//UFC::BufferedLog::Printf( " OnNewReport:[%s] Seq%d", Message->c_str(), Seq );
        UFC::NInt32 MsgLen( HEADER_SIZE + Message->Length() );
        UFC::NInt32 MsgSeq( Seq );
        char Reserved[ RESERVED_SIZE ];
        memset( Reserved, RESERVED_CHAR, RESERVED_SIZE );

        UFC::MemoryStream ms;
        MsgLen.SaveToStream( &ms );
        ms.Write( CMD_DASD.c_str(), CMD_DASD.Length() );
        MsgSeq.SaveToStream( &ms );
        MsgSeq.SaveToStream( &ms );
        ms.Write( Reserved, RESERVED_SIZE );
        ms.Write( Message->c_str(), Message->Length() );
        
        FSocket.BlockSend( (char*)(ms.GetBuffer()), ms.GetSize() );
        FClientIdleSec = time( NULL );
        
        UFC::BufferedLog::Printf( " Send:[%s]", Message->c_str() );
    }
    catch( UFC::SocketException& SktEx )
    {
        UFC::BufferedLog::Printf( " OnNewReport UFC::SocketException:[%s]", SktEx.what() );
//        printf( "%s\n", SktEx.what() );
        EndSession();
    }
    catch( UFC::Exception& Ex )
    {
        UFC::BufferedLog::Printf( " OnNewReport UFC::Exception:[%s]", Ex.what() );
//        printf( "%s\n", Ex.what() );
    }
    catch(...)
    {
//        printf( "unknown exception\n" );
        UFC::BufferedLog::Printf( " OnNewReport unknown exception" );
    }
}
//---------------------------------------------------------------------------
// the initial state after the connection for the first time is OFF(SVOF).
// however, even in this state, heartbeats must be sent.
//---------------------------------------------------------------------------
void TCommandListener::OnConnect( UFC::PClientSocket * Socket )
{
    SVOF();
    FReconnectTimes = 0;
}
//---------------------------------------------------------------------------
void TCommandListener::OnDisconnect( UFC::PClientSocket * Socket, BOOL NeedReconnect )
{
    SVOF();
}
//---------------------------------------------------------------------------
int TCommandListener::ReceiveIntegerFromSocketBuffer( UFC::PClientSocket* Socket )
{
    UInt8 Buffer[4];
    Socket->BlockRecv( Buffer, 4 );
    return UFC::NInt32::ToInt32( Buffer );
}
//---------------------------------------------------------------------------
UFC::AnsiString TCommandListener::ReceiveStringFromSocketBuffer( UFC::PClientSocket* Socket, int Length )
{
    UInt8* Buffer = new UInt8[Length];
    Socket->BlockRecv( Buffer, Length );
    UFC::AnsiString STR( (char*)Buffer, Length );
    delete [] Buffer;
    return STR;
}
//---------------------------------------------------------------------------
// *the tandem can send control messages to the realreport client, and control
//  messages include service stop(SVOF), service start(SVON), and
//  retransmission(RESD).
//
// SVON
// |----------------|---------------|--------------|
// | message length | message type  | sequence no  |
// | int (4 byte)   | char (4 byte) | int (4 byte) |
// |----------------|---------------|--------------|
//
// SVOF
// |----------------|---------------|
// | message length | message type  |
// | int (4 byte)   | char (4 byte) |
// |----------------|---------------|
//
// RESD
// |----------------|---------------|--------------|
// | message length | message type  | sequence no  |
// | int (4 byte)   | char (4 byte) | int (4 byte) |
// |----------------|---------------|--------------|
//---------------------------------------------------------------------------
BOOL TCommandListener::OnDataArrived( UFC::PClientSocket* Socket )
{
    try
    {
        FServerIdleSec          = time( NULL );
        int MsgLen              = ReceiveIntegerFromSocketBuffer( Socket );
        UFC::AnsiString MsgType = ReceiveStringFromSocketBuffer( Socket, 4 );
        //UFC::BufferedLog::DebugPrintf( " [%s:%d]Recvive [%d]", FRemoteIP.c_str(), FRemotePort, MsgLen );
        if( MsgLen == 8 )
        {
            if( MsgType == CMD_SVOF )
            {
//UFC::BufferedLog::Printf( " [%s:%d]Recvive SVOF", FRemoteIP.c_str(), FRemotePort );
                SVOF();
            }
            else if( MsgType == CMD_HTBT )
            {
//UFC::BufferedLog::Printf( " [%s:%d]Recvive CMD_HTBT", FRemoteIP.c_str(), FRemotePort );
            }
            else
            {
//UFC::BufferedLog::Printf( " [%s:%d]Recvive Unknown Msg, Length = 8", FRemoteIP.c_str(), FRemotePort );
                return FALSE;
            }
        }
        else if( MsgLen == 12 )
        {
            int MsgSeq = ReceiveIntegerFromSocketBuffer( Socket );

            if( MsgType == CMD_SVON )
            {
                //UFC::BufferedLog::DebugPrintf( " [%s:%d]Recvive SVON", FRemoteIP.c_str(), FRemotePort );
                SVON( MsgSeq );
            }
            else if( MsgType == CMD_RESD )
            {
                //UFC::BufferedLog::DebugPrintf( " [%s:%d]Recvive RESD", FRemoteIP.c_str(), FRemotePort );
                RESD( MsgSeq );
            }
            else
            {
                printf( "unknown message type: %s\n", MsgType.c_str() );
                return FALSE;
            }
        }
        else
        {
            printf( "incorrect message length: %d\n", MsgLen );
            return FALSE;
        }
    }
    catch( UFC::SocketException& SktEx )
    {
        printf( "%s\n", SktEx.what() );
        return FALSE;
    }
    catch( UFC::Exception& Ex )
    {
        printf( "%s\n", Ex.what() );
        return FALSE;
    }
    catch(...)
    {
        printf( "unknown exception\n" );
        return FALSE;
    }

    return TRUE;
}
//---------------------------------------------------------------------------
// *tandem send heartbeat data when there is no data to send during the idle
//  time interval.
//
// *if no data is received for three idle time intervals, the tandem and the
//  realreport will end the session.
//---------------------------------------------------------------------------
void TCommandListener::OnIdle( UFC::PClientSocket * Socket )
{
    long Sec = time( NULL );

    if( ((Sec - FServerIdleSec) / FTandemHeartbeatInterval) >= 3 )
    {
        printf( "no data is received for three idle time intervals\n" );
        EndSession();
    }
}
//---------------------------------------------------------------------------
void TCommandListener::Reconnect( void )
{
    try
    {
        FReconnectTimes++;
        //printf( "%05d connecting to tandem( %s:%d )\n", FReconnectTimes, FRemoteIP.c_str(), FRemotePort );
        FSocket.Connect( FRemoteIP, FRemotePort );
    }
    catch( UFC::SocketException& SktEx )
    {
        printf( "%s\n", SktEx.what() );
    }
    catch( UFC::Exception& Ex )
    {
        printf( "%s\n", Ex.what() );
    }
    catch(...)
    {
        printf( "unknown exception\n" );
    }
}
//---------------------------------------------------------------------------
void TCommandListener::EndSession( void )
{
    try
    {
        printf( "end session:[%s:%d]\n", FRemoteIP.c_str(), FRemotePort );
        SVOF();
        FSocket.Disconnect();
    }
    catch( UFC::SocketException& SktEx )
    {
        EndSession();
        printf( "%s\n", SktEx.what() );
    }
    catch( UFC::Exception& Ex )
    {
        printf( "%s\n", Ex.what() );
    }
    catch(...)
    {
        printf( "unknown exception\n" );
    }
}
//---------------------------------------------------------------------------
// HTBT
// |----------------|---------------|
// | message length | message type  |
// | int (4 byte)   | char (4 byte) |
// |----------------|---------------|
//
// *if the realreport client fails to send the HTBT, the realreport client will
//  end the session and attempt reconnection.
//---------------------------------------------------------------------------
void TCommandListener::HTBT( void )
{
    try
    {
        long Sec = time( NULL );
        if( (Sec - FClientIdleSec) >= FClientHeartbeatInterval )
        {
            UFC::NInt32 MsgLen( 8 );
            UFC::MemoryStream ms( 8 );
            MsgLen.SaveToStream( &ms );
            ms.Write( CMD_HTBT.c_str(), CMD_HTBT.Length() );

            FSocket.BlockSend( (char*)(ms.GetBuffer()), ms.GetSize() );
            UFC::BufferedLog::DebugPrintf( " Send HTBT to %s:%d", FRemoteIP.c_str(), FRemotePort );
            FClientIdleSec = time( NULL );
        }
    }
    catch( UFC::SocketException& SktEx )
    {
        EndSession();
        printf( "%s\n", SktEx.what() );
    }
    catch( UFC::Exception& Ex )
    {
        printf( "%s\n", Ex.what() );
    }
    catch(...)
    {
        printf( "unknown exception\n" );
    }
}
//---------------------------------------------------------------------------
// *sent by the tandem to the realreport client to request the corresponding
//  adapter to restart the service.
//
// *the realreport client sets the sequence number of the data to receive.
//
// *when the realreport client receives SVOF from the randem, the realreport
//  client immediately starts data transmission.
//---------------------------------------------------------------------------
void TCommandListener::SVON( int Seq )
{
    FMessageStore->SeekTo( Seq );
    UFC::BufferedLog::Printf( "  SVON Seq:[%d]", Seq );
    FMessageStore->SetEventHandler( this );
}
//---------------------------------------------------------------------------
// *sent by the tandem server to the realreport client to request the
//  corresponding adapter to stop the service.
//
// *when the realreport client receives SVOF from the randem, the realreport
//  client immediately stops data transmission.
//---------------------------------------------------------------------------
void TCommandListener::SVOF( void )
{
    FClientIdleSec  = time( NULL );
    FServerIdleSec  = time( NULL );
    FMessageStore->SetEventHandler( NULL );
}
//---------------------------------------------------------------------------
// *sent by the tandem server to the realreport client to request retransmission
//  of the data.
//---------------------------------------------------------------------------
void TCommandListener::RESD( int Seq )
{
    FMessageStore->SeekTo( Seq );
}
//---------------------------------------------------------------------------
