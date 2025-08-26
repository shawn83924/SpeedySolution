#include "TFXLinkSubSystem.h"
#include "TFXConnection.h"
#include "TFXConnectionObject.h"
#include "TFXSocketConnection.h"
#include "../UFC/Profiler.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//const int        MAX_MESSAGE_SIZE         = 1025;
//const long       TXIFEX_PULL_INTERVAL     = 100L;
//const long       LINKSUBSYSTEM_TIMEOUT    = 180L; 
//const int        PVC_BUSY_RETRY_TIMES     = 50000;
//-----------------------------------------------------------------------------------------
namespace TFX
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 BOOL  LinkSubSystemBase::UseBinaryLog = FALSE;
 BOOL  LinkSubSystemBase::FDisableLog  = FALSE;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Class LinkSubSystemBase
// Constructor
// Construct a X.25 or TCP LinkSubSystem object
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LinkSubSystemBase::LinkSubSystemBase( BOOL IsTAIFEX, BOOL IsOptions, BOOL IsOffHour, int Line, int PVCNumber, UFC::AnsiString Password, UFC::AnsiString BrokerID, char APCode, const char* URL )
:FLine( Line ),
 FPVCNumber( PVCNumber ),
 FAPCode( APCode ),
 FBrokerID( BrokerID ),
 FPassword( Password ),
 FURL( URL ), 
 FBackupIndex( 0 ),
 FSequenceOut( 0 ),
 FHeartBtInt( 60 ),
 FMaxThroughput( 10 ),
 FStatus( PVC_STATUS_UNKNOWN ),
 FLastError( PVC_STATUS_UNKNOWN ),
 FLockReason( PVC_STATUS_SUBSYS_ERR ),
 FSocketFD( -1 ),
 FLinkSubSysStatus( FALSE ), 
 FStopHandShake( FALSE ),
 FIsOptions( IsOptions ),
 FIsTAIFEXSide( IsTAIFEX ),
 FIsListen( FALSE ),
 FIsOffHour( IsOffHour ),
 FListener( NULL ),
 FLogManager( NULL ),
 FBandwidth( lb128K ),
 FPrefixIndex( 0 )
{
    UFC::AnsiString PrefixStr;
    
    FIsTCP = TRUE;        
    FConnection    = new SocketConnection();
    FBrokerIndex   = TMP::TMPMessage::GetBrokerID( FBrokerID );
    FBuffer        = new char[ TMP_MAX_DATA_SIZE ];
    FLogBufferSize = TMP_MAX_DATA_SIZE*2;
    FLogDataSize   = FLogBufferSize - 16;
    #ifdef _FOR_FCM
        UFC::AnsiString UID = GetUniqueID();
        FSequenceIn.Attach( UID, 256 );
    #else
        FSequenceIn = 0;
    #endif
    snprintf( FLinkPVCSendStr, 32, "TMP[%s,%03d] Send:",FBrokerID.c_str(), FPVCNumber );
    snprintf( FLinkPVCRecvStr, 32, "TMP[%s,%03d] Recv:",FBrokerID.c_str(), FPVCNumber );
    
    if( UFC::BufferedLog::IsEnableLog_us() == TRUE )
        FLogPrefixLen = (Int32)strlen( FLinkPVCRecvStr) + 15; /// HH:MM:SS.uuuuuu length =15
    else
        FLogPrefixLen = (Int32)strlen( FLinkPVCRecvStr) + 12; /// HH:MM:SS.mmm length =12
    PrefixStr.Printf("[%s,%03d]",FBrokerID.c_str(),FPVCNumber);
    FPrefixIndex = UFC::BufferedLog::AddPrefix( PrefixStr );
    FSendLogBuffer  = new char[ FLogBufferSize ];
    FRecvLogBuffer  = new char[ FLogBufferSize ];
    memset(FRecvTime, 0, sizeof(FRecvTime));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LinkSubSystemBase::LinkSubSystemBase( BOOL IsTAIFEX, BOOL IsOptions, ConnectionParameter& Params )
:FLine( Params.GetLink() ),
 FPVCNumber( Params.GetPVC() ),
 FAPCode( Params.GetAPCode() ),
 FBrokerID( Params.GetBrokerID() ),
 FPassword( Params.GetPassword() ),
 FURL( Params.GetURL() ),
 FBackupIndex( 0 ),
 FSequenceOut( 0 ),
 FHeartBtInt( Params.GetHeartBtInt() ),
 FMaxThroughput( Params.GetThroughput() ),
 FStatus( PVC_STATUS_UNKNOWN ),
 FLastError( PVC_STATUS_UNKNOWN ),
 FLockReason( PVC_STATUS_SUBSYS_ERR ),
 FSocketFD( -1 ),
 FLinkSubSysStatus( FALSE ),
 FStopHandShake( FALSE ), 
 FIsTCP( Params.IsTCPConnection()),
 FIsOptions( IsOptions ),
 FIsTAIFEXSide( IsTAIFEX ),
 FIsListen( FALSE ),
 FIsOffHour( Params.IsOffHour() ),
 FListener( NULL ),
 FLogManager( NULL ),
 FBandwidth( lb128K ),
 FPrefixIndex( 0 )
{
    UFC::AnsiString PrefixStr;

    FConnection    = new SocketConnection();
    FBrokerIndex   = TMP::TMPMessage::GetBrokerID( FBrokerID );                
    FBuffer        = new char[ TMP_MAX_DATA_SIZE ];
    FLogBufferSize = TMP_MAX_DATA_SIZE*2;
    FLogDataSize   = FLogBufferSize - 16;
    #ifdef _FOR_FCM
        UFC::AnsiString UID = GetUniqueID();
        FSequenceIn.Attach( UID, 256 );
    #else
        FSequenceIn = 0;
    #endif
    snprintf( FLinkPVCSendStr, 32, "TMP[%s,%03d] Send:",FBrokerID.c_str(), FPVCNumber );
    snprintf( FLinkPVCRecvStr, 32, "TMP[%s,%03d] Recv:",FBrokerID.c_str(), FPVCNumber );    
    if( UFC::BufferedLog::IsEnableLog_us() == TRUE )
        FLogPrefixLen = (Int32)strlen( FLinkPVCRecvStr) + 15; /// HH:MM:SS.uuuuuu length =15
    else
        FLogPrefixLen = (Int32)strlen( FLinkPVCRecvStr) + 12; /// HH:MM:SS.mmm length =12
    PrefixStr.Printf("[%s,%03d]",FBrokerID.c_str(),FPVCNumber);
    FPrefixIndex = UFC::BufferedLog::AddPrefix( PrefixStr );
    FSendLogBuffer  = new char[ FLogBufferSize ];
    FRecvLogBuffer  = new char[ FLogBufferSize ];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int LinkSubSystemBase::GetTMPSystemType( void )
{
    if( FIsOptions == TRUE )
        return 10 + (int)FIsOffHour;
    else
        return 20 + (int)FIsOffHour;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString LinkSubSystemBase::GetUniqueID( void )
{
    UFC::AnsiString UniqueKey;

    UniqueKey.Printf( "Sys_%d_fcm_%d_SID_%d", GetTMPSystemType(),FBrokerIndex,FPVCNumber );
    return UniqueKey;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::SendMessageL010( UInt8 StatusCode )
{
    if( FIsTCP == FALSE ) ///< For X.25 connection            
    {
        MessageL010 L010;
        
        L010.SetStatusCode( StatusCode );
        SendMessage( &L010, TXIFEX_PULL_INTERVAL, FALSE );
    }
    else
    {
        TMP::TMPMessage L010( TMP::tmtL10 );

        L010.SetField( "start_in_bound_num", 0 );
        L010.SetField( "status_code",StatusCode );
        SendTMPMessage( &L010, TMP_TIMEOUT, TRUE );
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemBase::IsErrorStat( int PVCState )
{
    if( PVCState == PVC_STATUS_THROUGHPUT_ERR  ||
        PVCState == PVC_STATUS_SYS_ERR    ||
        PVCState == PVC_STATUS_SID_ERR    ||
        PVCState == PVC_STATUS_SUBSYS_ERR ||
        PVCState == PVC_STATUS_PW_ERR  ||
        PVCState == PVC_STATUS_FCM_ERR ||
        PVCState == PVC_STATUS_AP_ERR  ||
        PVCState > 1000  )
        return TRUE;
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemBase::IsBrokenStat( int PVCState )
{
    if( PVCState == PVC_STATUS_THROUGHPUT_ERR  ||
        PVCState == PVC_STATUS_SYS_ERR    ||
        PVCState == PVC_STATUS_SID_ERR    ||
        PVCState == PVC_STATUS_SUBSYS_ERR ||
        PVCState == PVC_STATUS_PW_ERR   ||
        PVCState == PVC_STATUS_FCM_ERR  ||
        PVCState == PVC_STATUS_AP_ERR   ||
        PVCState == PVC_STATUS_BROKEN   ||   ///< Include broken.(No error)
        PVCState == PVC_STATUS_OFFLINE_BUSY  ||
        PVCState == PVC_STATUS_OFFLINE_READY ||
        PVCState > 1000  )
        return TRUE;
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::ResetStat( void )
{
    FStatus     = PVC_STATUS_UNKNOWN;
    FLastError  = PVC_STATUS_UNKNOWN;
    FLockReason = PVC_STATUS_SUBSYS_ERR;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int LinkSubSystemBase::GetStatus( void )
{
    if( IsBrokenStat( FStatus ) == TRUE  )
        return FLastError;
    return FStatus;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::SetConnectionState( BOOL IsConnected ,int PVCState )
{
    FLinkSubSysStatus = IsConnected;

    if( IsConnected == FALSE ) ///< Disconnected
        FLastError = PVCState;    
    else
        FLastError = PVC_STATUS_UNKNOWN; ///< Reset the last error
    SetStatus( PVCState );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::SetConnectionState( int PVCState )
{
    if( FStatus > PVC_STATUS_WAIT_TRY || FStatus < PVC_STATUS_SLEEPING	)
    {
        if( PVCState == PVC_STATUS_IDLE )
        {
            if( FLinkSubSysStatus == TRUE )
                SetStatus( PVC_STATUS_ONLINE_READY );
            else
                SetStatus( PVC_STATUS_OFFLINE_READY );
        }
        else if( PVCState == PVC_STATUS_BUSY )
        {
            if( FLinkSubSysStatus == TRUE )
                SetStatus( PVC_STATUS_ONLINE_BUSY );
            else
                SetStatus( PVC_STATUS_OFFLINE_BUSY );
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::Open( ConnectionParameter& Params )
{
    FLine      = Params.GetLink();
    FPVCNumber = Params.GetPVC();
    FBrokerID  = Params.GetBrokerID();
    FPassword  = Params.GetPassword();
    FAPCode    = Params.GetAPCode();
    FURL       = Params.GetURL();
    FIsTCP     = Params.IsTCPConnection();
    
    FStatus           = PVC_STATUS_BROKEN;
    FLinkSubSysStatus = FALSE;
    FStopHandShake    = FALSE;
    
    if( FIsTCP == FALSE )
    {
        snprintf( FLinkPVCSendStr, FLogBufferSize, "Port%05d PVC%03d Send:",FLine, FPVCNumber );
        snprintf( FLinkPVCRecvStr, FLogBufferSize, "Port%05d PVC%03d Recv:",FLine, FPVCNumber );
    }
    else
    {
        snprintf( FLinkPVCSendStr, FLogBufferSize, "TMP[%s,%03d] Send:",FBrokerID.c_str(), FPVCNumber );
        snprintf( FLinkPVCRecvStr, FLogBufferSize, "TMP[%s,%03d] Recv:",FBrokerID.c_str(), FPVCNumber );
    }
    if( UFC::BufferedLog::IsEnableLog_us() == TRUE )
        FLogPrefixLen = (Int32)strlen( FLinkPVCRecvStr) + 15; /// HH:MM:SS.uuuuuu length =15
    else
        FLogPrefixLen = (Int32)strlen( FLinkPVCRecvStr) + 12; /// HH:MM:SS.mmm length =12
    if( FSocketFD == -1 )
    {
        UFC::PStringList URLs;
        
        URLs.SetStrings( FURL, "^" );
        FBackupIndex %= URLs.ItemCount();
        FConnection->Open( FLine, FPVCNumber, (char*)URLs[ FBackupIndex ].c_str() );
    }
    else
        FConnection->Open( FSocketFD );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::Open( void )
{
    FStatus           = PVC_STATUS_BROKEN;
    FLinkSubSysStatus = FALSE;
    FStopHandShake    = FALSE;
    if( FSocketFD == -1 )
    {
        UFC::PStringList URLs;        
        
        URLs.SetStrings( FURL, "^" );              
        FBackupIndex %= URLs.ItemCount();
        FConnection->Open( FLine, FPVCNumber, (char*)URLs[ FBackupIndex ].c_str() );
    }
    else
        FConnection->Open( FSocketFD );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::SetBackupIndex( int index ) 
{ 
    UFC::PStringList URLs;        
        
    URLs.SetStrings( FURL, "^" );                  
    FBackupIndex = index % URLs.ItemCount(); 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::StopHandshake( bool COD )
{
    FStopHandShake = FALSE;    
    if( COD == FALSE )
    {
        TMP::TMPMessage L070( TMP::tmtL70 );        

        if( SendTMPMessage( &L070, TMP_TIMEOUT, FALSE ) == TRUE )
            UFC::SleepMS( 50 );
        else
            UFC::BufferedLog::Printf( " Send L070 failed!"  );
    }     
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::Close( void )
{    
    
    FStatus           = PVC_STATUS_BROKEN;
    FLinkSubSysStatus = FALSE;    
    FStopHandShake    = FALSE;
    FConnection->Close();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LinkSubSystemBase::~LinkSubSystemBase()
{
    FConnection->Close();
    SetStatus( PVC_STATUS_UNKNOWN );
    delete [] FSendLogBuffer;
    delete [] FRecvLogBuffer;
    delete [] FBuffer;
    delete FConnection;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::WriteLog( BOOL IsSend, UInt8 *Msg, int Length, BOOL UseOrginalTime )
{
    if( FDisableLog == FALSE && FLogManager != NULL )
    {
        if( UseBinaryLog == TRUE )
        {
            FLogManager->WriteString( (const char*)Msg, Length );
            return;
        }
        char* LogBuffer;
        Int32 CMD;

        if( IsSend == TRUE )
        {
            CMD = EVENT_PVC_WRITE;
            LogBuffer = FSendLogBuffer;
            UFC::GetTimeString_ms_us( LogBuffer, UFC::BufferedLog::IsEnableLog_us(), TRUE );            
            strncat( LogBuffer, FLinkPVCSendStr, FLogDataSize );
        }
        else
        {            
            CMD = EVENT_PVC_READ;
            LogBuffer = FRecvLogBuffer;
            if( UseOrginalTime )
                strncpy( LogBuffer, FRecvTime, 16 );
            else
                UFC::GetTimeString_ms_us( LogBuffer, UFC::BufferedLog::IsEnableLog_us(), TRUE );            
            strncat( LogBuffer, FLinkPVCRecvStr, FLogDataSize );
        }
        ///< Copy log to buffer.
        memcpy( LogBuffer + FLogPrefixLen, Msg, Length );
        ///< append \0 to end the string
        *(LogBuffer + FLogPrefixLen + Length ) = 0;
        ///< Write to log
        FLogManager->WriteString( (const char*)LogBuffer, FLogPrefixLen + Length );
        ///< Trigger event
        if( FIsListen == TRUE && FListener != NULL )
            FListener->OnCommand( CMD, (void*)LogBuffer);
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Network I/O functions for X.25
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Send data to X.25 stream.
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemBase::SendMessage( MessageHeader* Message, long Timeout, BOOL ThrowReconnectException )
{
    int MessageLen;

    SetConnectionState( PVC_STATUS_BUSY );
    Message->AsString( FBuffer );    
    MessageLen = (int)strlen( FBuffer );
    int Result = FConnection->Send( FBuffer, MessageLen, Timeout );
    if( Result >= PVC_OK )	 ///< Send X.25 message succeed.
    {
        SetConnectionState( PVC_STATUS_IDLE );
        WriteLog( TRUE, (UInt8*)FBuffer, MessageLen );
        return TRUE;
    }
    else		     ///< X.25 timeout or disconnect
    {
        if( ThrowReconnectException == TRUE )
        {
            if( Result == PVC_RESET )
                throw( X25ReconnectException( "Receive PVC Reset request" ) );
            else
                throw( X25ReconnectException( "Send data timeout" ) );
        }
        return FALSE;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Receive data from X.25 stream.
//  Modify this function on 2007/10/14 by Simon, Check all the return values.
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader* LinkSubSystemBase::ReceiveMessage( long TimeOut, BOOL ThrowReconnectException )
{
    MessageHeader* Msg;
    int ReceiveTime = ThreadClock::SecondOfToday();
    int SelectTime  = TimeOut;
    int Result;

    SetConnectionState( PVC_STATUS_BUSY ); ///< Set state to Busy.
    while( TRUE )
    {
        memset( FBuffer, 0, MAX_MESSAGE_SIZE );
        Result = FConnection->Receive( FBuffer, MAX_MESSAGE_SIZE, SelectTime );
        // Return value of function Receive
        // >0               : Return received data length.
        //  0 PVC_OK        : Ok
        // -2 PVC_TIMEOUT   : Received data timeout.
        // -3 PVC_DISCONNECT: Received PVC disconnected control command.
        // -4 PVC_RESET     : Received PVC reset control command.        
        if( Result >= PVC_OK ) ///< Receive a normal message
        {
            WriteLog( FALSE, (UInt8*)FBuffer, Result );
            if( (Msg = MessageHeader::CreateMessage( FBuffer, Result )) != NULL ) ///< OK! Create a message from received data.
            {
                if( ThrowReconnectException == TRUE && Msg->IsMessage( mtL010 ) == TRUE )
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, FPrefixIndex, " LinkSubSystem::ReceiveMessage, Receive a L010 throw RehandshakeException ");
                    throw( X25RehandshakeException("Received a L010") );
                }
                SetConnectionState( PVC_STATUS_IDLE );
                return Msg;
            }
            else ///< Failed! Unknown message.
            {
                UFC::BufferedLog::DebugPrintf( UFC::dlWarning, FPrefixIndex, " ************ Received Unknown Message ********");
                UFC::BufferedLog::DebugPrintf( UFC::dlWarning, FPrefixIndex, " Data: %s", FBuffer);
                UFC::BufferedLog::DebugPrintf( UFC::dlWarning, FPrefixIndex," Receive length: %d", Result);
                UFC::BufferedLog::DebugPrintf( UFC::dlWarning, FPrefixIndex," Length: %d",strlen(FBuffer) );
                UFC::BufferedLog::DebugPrintf( UFC::dlWarning, FPrefixIndex," **********************************************");
                throw( X25ReconnectException("Received Unknown Message.") );
            }
        }
        else if( Result == PVC_RESET ) ///< It's a PVC Reset control command.
        {
            UFC::BufferedLog::Printf( " LinkSubSystem::ReceiveMessage, Port:%d PVC:%d Receive PVC REST command", FLine,FPVCNumber);            
            if( ThrowReconnectException == FALSE )
            {
                SelectTime = (int)TimeOut - (ThreadClock::SecondOfToday()-ReceiveTime);
                if( SelectTime <= 0 ) ///< Reach the timeout ?
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," LinkSubSystem::ReceiveMessage, Receive PVC REST command and timeout" );
                    return NULL; ///< Time out
                }
            }
            else
                throw( X25ReconnectException( "Receive PVC Reset request" ) );
        }
        else if( Result == PVC_TIMEOUT )///< It's timeout.
        {
            if( ThrowReconnectException == TRUE )
                throw( X25ReconnectException( "Receive data timeout" ) );
            return NULL;
        }
        else if( Result == PVC_DISCONNECT )///< It's a PVC Disconnect control command.
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," LinkSubSystem::ReceiveMessage, Receive PVC Disconnected command" );
            if( ThrowReconnectException == TRUE )
                throw( X25ReconnectException( "Receive a Disconnected control packet" ) );
            return NULL; 
        }
        else
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," LinkSubSystem::ReceiveMessage,Receive unknown command[%d]", Result );
            if( ThrowReconnectException == TRUE )
                throw( X25ReconnectException( "Receive a unknown control command" ) );
            return NULL; ///< Time out without exception.            
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Add this function on 2004/08/14 by Simon
//
//  Ignore other message during the given timeout period
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader* LinkSubSystemBase::ReceiveMessage( TMessageType RequireType, int Timeout )
{
    int BeginTime = ThreadClock::SecondOfToday();
    int WaitTime  = Timeout;
    MessageHeader* Msg;

    while( WaitTime > 0 )
    {
        if( ( Msg = ReceiveMessage( WaitTime, FALSE )) != NULL )///< Receive the wake up message replay from the FCM server.
        {
            if( RequireType == Msg->MessageType( ) )
                return Msg;
            else
                delete Msg;
        }
        WaitTime -= ( ThreadClock::SecondOfToday() - BeginTime );
        BeginTime = ThreadClock::SecondOfToday();
    }
    return NULL; ///< Receive message timeout.
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Network I/O functions for TCP/IP
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Send data to socket stream.
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemBase::SendTMPMessage( TMP::TMPMessage* Message, long Timeout, BOOL ThrowReconnectException, BOOL WithSeq )
{    
    UFC::MemoryStream BufferStream( 256 );    
    int               Result;
    UFC::AnsiString   MsgString;
    
    if( FConnection->IsOpened() == FALSE )
        return FALSE;
    SetConnectionState( PVC_STATUS_BUSY );
    Message->SetFCMID( FBrokerIndex );
    Message->SetSessionID( FPVCNumber );
    if( WithSeq == TRUE )
    {
        FSequenceOut = FSequenceOut + 1;
        Message->SetMessageSeq( FSequenceOut );
    }
    else
        Message->SetMessageSeq( 0 );
    Message->UpdateTime();         
    if( UseBinaryLog == FALSE )
        Message->ToStreamString( &BufferStream, MsgString );
    else
        Message->ToStream( &BufferStream );        
    if( (Result= FConnection->Send( (char*)BufferStream.GetBuffer(),BufferStream.GetSize(),Timeout )) >= PVC_OK )	
    {
        if( UseBinaryLog == FALSE )
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlFatalError,FPrefixIndex," Send[%s]", MsgString.c_str() );            
            WriteLog( TRUE, (UInt8*)MsgString.c_str(), MsgString.Length() );
        }
        else
            WriteLog( TRUE, (UInt8*)BufferStream.GetBuffer(), BufferStream.GetSize() );
        SetConnectionState( PVC_STATUS_IDLE );        
        return TRUE;
    }
    else ///< TCP/IP timeout or disconnect
    {
        if( ThrowReconnectException == TRUE )
        {
            if( Result == PVC_RESET )
                throw( UFC::SocketException( "Receive TCP Reset request" ) );
            else
                throw( UFC::SocketException( "Send TCP data timeout" ) );
        }
        return FALSE;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemBase::SendTMPMessage( TMP::TMPMessage* Message, long Timeout, long Group, Int8 FlagID, const UFC::AnsiString& Key1, const UFC::AnsiString& Key2 )
{    
    UFC::MemoryStream BufferStream( 256 );
    UFC::AnsiString   MsgString;
    int               Result;
    
    if( FConnection->IsOpened() == FALSE )
        return FALSE;
    FSequenceOut = FSequenceOut + 1;
    SetConnectionState( PVC_STATUS_BUSY );    
    Message->SetFCMID( FBrokerIndex );
    Message->SetSessionID( FPVCNumber );    
    Message->SetMessageSeq( FSequenceOut );    
    Message->UpdateTime();    
    if( UseBinaryLog == FALSE )
        Message->ToStreamString( &BufferStream, MsgString );
    else
        Message->ToStream( &BufferStream );            
    if( (Result= FConnection->Send( (char*)BufferStream.GetBuffer(),BufferStream.GetSize(),Timeout )) >= PVC_OK )	
    {
        UFC::Profiler::SetPerformanceFlag( Group, FlagID, Key1,Key2 );
        if( UseBinaryLog == FALSE )
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlFatalError,FPrefixIndex," Send[%s]", MsgString.c_str() );            
            WriteLog( TRUE, (UInt8*)MsgString.c_str(), MsgString.Length() );        
        }
        else
            WriteLog( TRUE, (UInt8*)BufferStream.GetBuffer(), BufferStream.GetSize() );
        SetConnectionState( PVC_STATUS_IDLE );        
        return TRUE;
    }
    else ///< TCP/IP timeout or disconnect
    {
        if( Result == PVC_RESET )
            throw( UFC::SocketException( "Receive TCP Reset request" ) );
        else
            throw( UFC::SocketException( "Send TCP data timeout" ) );
        return FALSE;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemBase::SendTMPMessage( TMP::FastTMPMessage* Message, long Timeout, long Group, Int8 FlagID, const UFC::AnsiString& Key1, const UFC::AnsiString& Key2 )
{    
    if( FConnection->IsOpened() == TRUE )
    {
        int   Result;
        
        Message->Update( FBrokerIndex , FPVCNumber , FSequenceOut );                
        if( (Result= FConnection->Send( (char*)Message->GetBytes(),Message->Length(),Timeout )) >= PVC_OK )	
        {
            UFC::Profiler::SetPerformanceFlag( Group, FlagID, Key1,Key2 );            
            return TRUE;
        }
        else ///< TCP/IP timeout or disconnect
        {
            if( Result == PVC_RESET )
                throw( UFC::SocketException( "Receive TCP Reset request" ) );
            else
                throw( UFC::SocketException( "Send TCP data timeout" ) );        
        }
    }
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::LogSendTMPMessage( TMP::FastTMPMessage* Message )
{    
    char  LogStr[ 2048 ];
    int   len = Message->ToString( LogStr, 2048 );
    
    if( len > 0 && len < 2048)
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlFatalError,FPrefixIndex," Send[%s]", LogStr );            
        WriteLog( TRUE, (UInt8*)LogStr, len );        
    }
    else
        UFC::BufferedLog::Printf( " Send[%s]", LogStr );  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemBase::SendTMPMessage( TMP::FastTMPMessage* Message, long Timeout, BOOL ThrowReconnectException, BOOL WithSeq )
{    
    if( FConnection->IsOpened() == TRUE )
    {        
        char  LogStr[2048];
        int   Result,Seq = 0 ;
        
        SetConnectionState( PVC_STATUS_BUSY );
        if( WithSeq == TRUE )
        {
            FSequenceOut = FSequenceOut + 1;
            Seq = FSequenceOut;
        }        
        Message->Update( FBrokerIndex , FPVCNumber , Seq );        
        Message->ToString( LogStr, 2048 );
        if( (Result= FConnection->Send(  (char*)Message->GetBytes(),Message->Length(),Timeout )) >= PVC_OK )	
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlFatalError,FPrefixIndex," Send[%s]", LogStr );            
            WriteLog( TRUE, (UInt8*)LogStr, strlen( LogStr ) );        
            SetConnectionState( PVC_STATUS_IDLE );        
            return TRUE;
        }
        else ///< TCP/IP timeout or disconnect
        {
            if( ThrowReconnectException == TRUE )
            {
                if( Result == PVC_RESET )
                    throw( UFC::SocketException( "Receive TCP Reset request" ) );
                else
                    throw( UFC::SocketException( "Send TCP data timeout" ) );
            }         
        }
    }
    return FALSE;        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Send data to socket stream.
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemBase::SendTMPMessages( UFC::List< TMP::TMPMessage*>& Messages, long Timeout, BOOL ThrowReconnectException, BOOL WithSeq )
{
    UFC::MemoryStream BufferStream( TMP_MAX_DATA_SIZE );
    TMP::TMPMessage*  Message;
    int Result;

    if( FConnection->IsOpened() == FALSE )
        return FALSE;
    SetConnectionState( PVC_STATUS_BUSY );
    for( int i = 0; i < Messages.ItemCount(); i++ )
    {
        if( (Message = Messages.GetItem(i)) != NULL )
        {
            UFC::AnsiString MsgString;

            Message->SetFCMID( FBrokerIndex );
            Message->SetSessionID( FPVCNumber );
            if( WithSeq == TRUE )
            {
                FSequenceOut = FSequenceOut + 1;
                Message->SetMessageSeq( FSequenceOut );
            }
            else
                Message->SetMessageSeq( 0 );
            Message->UpdateTime();
            Message->ToStream( &BufferStream );            
        }
    }
    if( (Result = FConnection->Send( (char*)BufferStream.GetBuffer(),BufferStream.GetSize(),Timeout )) >= PVC_OK )
    {
        UFC::AnsiString MsgString;

        SetConnectionState( PVC_STATUS_IDLE );
        if( UseBinaryLog == FALSE )
        {
            for( int i = 0; i < Messages.ItemCount(); i++ )
            {
                if( (Message = Messages.GetItem(i)) != NULL )
                {
                    Message->ToString( MsgString );
                    UFC::BufferedLog::DebugPrintf( UFC::dlFatalError,FPrefixIndex, " Send[%s]", MsgString.c_str());
                    WriteLog( TRUE, (UInt8*)MsgString.c_str(), MsgString.Length() );
                }
            }
        }
        else
            WriteLog( TRUE, (UInt8*)BufferStream.GetBuffer(), BufferStream.GetSize() );
        return TRUE;
    }
    else ///< TCP/IP timeout or disconnect
    {
        if( ThrowReconnectException == TRUE )
        {
            if( Result == PVC_RESET )
                throw( X25ReconnectException( "Receive TCP Reset request" ) );
            else
                throw( X25ReconnectException( "Send TCP data timeout" ) );
        }
        return FALSE;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::DumpData( UInt8* Data, Int32 Len )
{
    UFC::AnsiString ErrorData;
    TMP::TMPMessage::ToHexString( ErrorData,Data,Len);
    UFC::BufferedLog::DebugPrintf( UFC::dlWarning, FPrefixIndex," Length[%d] Data[%s]", Len, ErrorData.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::CheckMessageChecksum( UInt8* Buffer, Int16 Len )
{
    Int16 MsgLen = UFC::NInt16::ToInt16( Buffer );

    if( MsgLen + 3 <= Len )
    {
        UInt8          CheckSum      = *( Buffer + MsgLen + 2 );
        register UInt8 RightCheckSum = 0;

        for( register Int16 i = 0; i < MsgLen + 2; i++ )
             RightCheckSum += *(Buffer+i);
        if( CheckSum != RightCheckSum )
            throw( TMPCheckSumException( CheckSum, RightCheckSum ) );
    }
    else
        throw( TMPDataErrorException( MsgLen, Len ) );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::WriteTMPMessageLog( TMP::TMPMessage* Msg, Int32 size, BOOL UseOrginalTime )
{
    /*if( IsReady() == FALSE && Msg->GetMessageType() > 100 )
    {   ///< Shoulde drop this message.
        if( UseBinaryLog == FALSE ) ///< Write text log.
        {
            UFC::AnsiString MsgString;

            Msg->ToString( MsgString );
            UFC::BufferedLog::DebugPrintf( UFC::dlWarning,FPrefixIndex, " Drop[%s]", MsgString.c_str());
        }
        delete Msg;
        throw( TMPRehandshakeException( "Received Application message on session level") );
    }
    else*/
    {
        if( UseBinaryLog == FALSE ) ///< Write text log.
        {
            UFC::AnsiString MsgString;

            Msg->ToString( MsgString );
            UFC::BufferedLog::DebugPrintf( UFC::dlFatalError, FPrefixIndex, " Recv[%s]", MsgString.c_str());
            WriteLog( FALSE, (UInt8*)MsgString.c_str(), MsgString.Length(), UseOrginalTime );
        }
        else
            WriteLog( FALSE, (UInt8*)FBuffer, size, UseOrginalTime );
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::CheckTMPMessageSeq( TMP::TMPMessage* Msg )
{
    UInt32 InSeq = Msg->GetMessageSeq( );
    UInt8  PartID = 0;
    
    if( Msg->FieldExists("part_id") == TRUE )
        PartID = Msg->GetField("part_id")->ToInteger();
    if( InSeq != 0 ) ///< Skip message without sequence.
    {
        if( FIsTAIFEXSide == TRUE && IsReady() ) ///< TAIFEX side && Connected.
        {
            if (FSequenceIn[PartID] == 0) ///< First message with sequence.
            {
                FSequenceIn[PartID] = (Int32) InSeq;
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex, " Start inbound seq[%d]", (Int32) FSequenceIn[PartID]);
            }
            else if (InSeq - (Int32) FSequenceIn[PartID] > 1) ///< Check inbound seq continues.
            {
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex, " Expect seq[%d] but recv seq[%d] ", (Int32) FSequenceIn[PartID] + 1, InSeq);
                SetConnectionState( FALSE, 1207);
                delete Msg;
                throw ( TMPRehandshakeException("Inbound seq out of order.", 207));
            }
            else ///< OK! Update sequence.
                FSequenceIn[PartID] = (Int32) InSeq;
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::CheckTMPMessage( TMP::TMPMessage* Msg, BOOL ThrowReconnectException )
{
    if (Msg->GetFCMID() != FBrokerIndex)///< Check BrokerID
    {
        UFC::AnsiString Err;

        Err.Printf("BrokerID error,expect [%d] but get [%d]", FBrokerIndex, Msg->GetFCMID());
        SetConnectionState(FALSE, 1202);
        delete Msg;
        throw ( TMPRehandshakeException((char*) Err.c_str(), 202)); ///< FCMID error
    }
    ///< Check Session ID.
    if (Msg->GetSessionID() != FPVCNumber)///< Check SessionID.
    {
        UFC::AnsiString Err;
        Err.Printf("SessionID error,expect [%d] but get [%d]", FPVCNumber, Msg->GetSessionID());
        SetConnectionState(FALSE, 1205);
        delete Msg;
        throw ( TMPRehandshakeException((char*) Err.c_str(), 205)); ///< SessionID error
    }
    ///< Check L10.
    if (ThrowReconnectException == TRUE && Msg->GetMessageType() == 10)
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex, " LinkSubSystem::ReceiveTMPMessage, Receive a L10 and throw TMPRehandshakeException ");
        delete Msg;
        throw ( TMPRehandshakeException("Received a L10"));
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Receive data from Socket stream.
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TMP::TMPMessage* LinkSubSystemBase::ReceiveTMPMessage( long TimeOut, BOOL ThrowReconnectException )
{
    TMP::TMPMessage* Msg;
    int Result;

    SetConnectionState( PVC_STATUS_BUSY ); ///< Set state to Busy.
    while( TRUE )
    {   
        if( (Result = FConnection->Receive(FBuffer,TMP_MAX_DATA_SIZE,TimeOut)) >= PVC_OK ) ///< Receive a TMP message
        {
            UFC::MemoryReadStream Stream( FBuffer, Result );
            
            try
            {
                UFC::GetTimeString_ms_us( FRecvTime, UFC::BufferedLog::IsEnableLog_us(), TRUE );
                ///< Create a message from received data.
                if( TMP::TMPMessage::IsCheckSumEnabled() == TRUE )
                    CheckMessageChecksum( (UInt8*)FBuffer, Result );
                if( (Msg = new TMP::TMPMessage( &Stream )) !=NULL )
                {                    
                    WriteTMPMessageLog( Msg, Result );///< 1. Write log
                    CheckTMPMessageSeq( Msg );///< 2. Check sequence.
                    CheckTMPMessage( Msg, ThrowReconnectException  );///< 3. Check FCM/Session ID or received L010.
                    SetConnectionState( PVC_STATUS_IDLE );
                    return Msg;
                }
                else ///< Failed! Unknown message.
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Received Unknown TMP Message.");
                    DumpData( (UInt8*)FBuffer, Result);
                    throw( TMPRehandshakeException( "Received Unknown TMP Message.") );
                }
            }            
            catch( TMPCheckSumException& e )
            {   ///<Checksum error
                DumpData( (UInt8*)FBuffer, Result);
                throw e;
            }            
        }        
        else if( Result == PVC_TIMEOUT )///< It's timeout.
        {
            if( ThrowReconnectException == TRUE )
                throw( TMPRehandshakeException( "Receive TMP data timeout" ) );
            return NULL;
        }        
        else
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," LinkSubSystem::ReceiveTMPMessage, Receive unknown command[%d]", Result );
            if( ThrowReconnectException == TRUE )
                throw( TMPRehandshakeException( "Receive a unknown control command" ) );
            return NULL; ///< Time out without exception.            
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TMP::TMPMessage* LinkSubSystemBase::ReceiveTMPMessageNotLog( long TimeOut, int& Result, BOOL ThrowReconnectException )
{
    TMP::TMPMessage* Msg;    

    SetConnectionState( PVC_STATUS_BUSY ); ///< Set state to Busy.
    while( TRUE )
    {      
        if( (Result = FConnection->Receive( FBuffer, TMP_MAX_DATA_SIZE,TimeOut)) >= PVC_OK ) ///< Receive a TMP message
        {            
            try
            {
                UFC::GetTimeString_ms_us( FRecvTime, UFC::BufferedLog::IsEnableLog_us(), TRUE );                
                UFC::MemoryReadStream Stream( FBuffer, Result );                                
                
                if( TMP::TMPMessage::IsCheckSumEnabled() == TRUE )
                    CheckMessageChecksum( (UInt8*)FBuffer, Result );
                ///< Create a message from received data.
                if( (Msg = new TMP::TMPMessage( &Stream )) !=NULL )
                {   
                    CheckTMPMessageSeq( Msg );///< 2. Check sequence.
                    CheckTMPMessage( Msg, ThrowReconnectException  );///< 3. Check FCM/Session ID or received L010.
                    SetConnectionState( PVC_STATUS_IDLE );
                    return Msg;
                }
                else ///< Failed! Unknown message.
                {
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Received Unknown TMP Message.");
                    DumpData( (UInt8*)FBuffer, Result);
                    throw( TMPRehandshakeException( "Received Unknown TMP Message.") );
                }
            }            
            catch( TMPCheckSumException& e )
            {   ///<Checksum error
                DumpData( (UInt8*)FBuffer, Result);
                throw e;
            }            
        }        
        else if( Result == PVC_TIMEOUT )///< It's timeout.
        {
            if( ThrowReconnectException == TRUE )
                throw( TMPRehandshakeException( "Receive TMP data timeout" ) );
            return NULL;
        }        
        else
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," LinkSubSystem::ReceiveTMPMessage, Receive unknown command[%d]", Result );
            if( ThrowReconnectException == TRUE )
                throw( TMPRehandshakeException( "Receive a unknown control command" ) );
            return NULL; ///< Time out without exception.            
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


