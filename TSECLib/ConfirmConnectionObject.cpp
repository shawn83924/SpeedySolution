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
extern BOOL IsTSEC;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
TSECConfirmConnectionObject::TSECConfirmConnectionObject( ConnectionParameter& info,
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
 FLog( NULL )
{
    FConfirmReader = new ConfirmReader();
}
//----------------------------------------------------------------------------------------------------------------------
TSECConfirmConnectionObject::~TSECConfirmConnectionObject()
{
    Close();	
}
//----------------------------------------------------------------------------------------------------------------------
void TSECConfirmConnectionObject::UpdateConnectionParameter( ConnectionParameter& Info )
{
    FParams = Info;
    FNeedLock   = FALSE;
    FNeedReset  = TRUE;
    UFC::BufferedLog::Printf( " PVC:%d Changed to FCM:%s Password:%s", FParams.GetPVC(), FParams.GetBrokerID().c_str(), FParams.GetPassword().c_str());
}
//----------------------------------------------------------------------------------------------------------------------
void TSECConfirmConnectionObject::OnCommand( int Command, void* Data )
{
    switch( Command )
    {
        case CONNECTION_CCH_UPDATE_SEQ: FEndPosition = ( Int64 )((int*) Data);						
                                        break;
        case COMMAND_OPEN_MARKET:       break;
        case COMMAND_CLOSE_MARKET:      Terminate();
                                        break;
        case COMMAND_UNLOCK_PVC:        FNeedLock  = FALSE;
                                        FNeedReset = TRUE;
                                        break;
        case COMMAND_LOCK_PVC:      	FNeedLock  = TRUE;
                                        break;
        case COMMAND_GET_STATE:         *((int*)Data ) = FConnection->GetStatus();
                                        break;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TSECConfirmConnectionObject::Close( void )
{
    FConnection->StopHandshake();
    if( IsTerminated() == FALSE ) ///< If it's a Non-blocking socket.
    {
        Terminate(); ///< Terminate the socket polling thread.
        WaitFor();   ///< Wait for thread terminated.
    }
}
//-----------------------------------------------------------------------------------------------------------------------
BOOL TSECConfirmConnectionObject::ReadMessage()
{
    if( FEndPosition > 0 && FReadPosition < FEndPosition )
    {        
        UFC::PStringList ExecList;
        
        if( FConfirmReader->Read( ExecList, FReadPosition + 1 ) > 0 )
        {
            AddReportMessage( ExecList );
            FReadPosition += ExecList.ItemCount() ;
            return TRUE;
        }
    }
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
BOOL TSECConfirmConnectionObject::MessageOut( )
{
    try
    {
        if( FHandshakeReady == TRUE && FConnected == TRUE )
        {
            int Count = 0;
            FMsgCriticalSection.Enter();
            Count = FMessages.ItemCount( );
            FMsgCriticalSection.Leave();

#ifndef SIMULATOR
            if( Count == 0 )			    // Check whether need to read messages.
                return TRUE;
#else
            if( Count == 0 )			    // Check whether need to read messages.
                return ReadMessage();
#endif
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
void TSECConfirmConnectionObject::IdleHandshake( void )
{
    MessageR4      R4( IsTSEC );
    MessageHeader* InMsg;

    if( FConnection->SendMessage( &R4, TWSE_ORDERSUBSYSTEM_TIMEOUT ) == TRUE )
    {
        if( IsTerminated() == TRUE ) return;
        if( (InMsg = FConnection->ReceiveMessage( TWSE_ORDERSUBSYSTEM_TIMEOUT )) == NULL )
        {
            TMessageType MT = InMsg->MessageType();
            delete InMsg;
            if( MT != mtR5 )
                throw( X25ReconnectException("Confirm subsystem: Not a idle handshake replay message." ));
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TSECConfirmConnectionObject::ClearMessageList()
{
    UFC::PLockObject Lock( FMsgCriticalSection );
    
    for( register int i = FMessages.ItemCount()-1; i > 0; i-- )
	 delete FMessages.GetItem( i );
    FMessages.Clear();    
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TSECConfirmConnectionObject::CheckR1( MessageR1* R1 )
{
    MessageR2* R2;
    int        RequestSeq = R1->GetSeq();

    if( RequestSeq > FEndPosition )
        RequestSeq = FEndPosition;
    if( RequestSeq < 1 )
        RequestSeq = 1;
    ClearMessageList();
    FSendPosition = RequestSeq - 1;  // Change the sending position to the position that FCM gived.
    FReadPosition = RequestSeq - 1;
    R2 = new MessageR2( IsTSEC, R1->GetBrokerID(), RequestSeq );
    return R2;
}
//----------------------------------------------------------------------------------------------------------------------
void TSECConfirmConnectionObject::ConfirmsubSystemHandshake( BOOL ReceivedL010 )
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

        if( (InMsg = FConnection->ReceiveMessage( TWSE_ORDERSUBSYSTEM_TIMEOUT )) != NULL )
        {
            TMessageType MsgType = InMsg->MessageType( );
            if( MsgType == mtR1 )
            {
                FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_READY, (void*) &FEndPosition );
                OutMsg = CheckR1( static_cast<MessageR1*>(InMsg) );
                delete InMsg;
                FConnection->SendMessage( OutMsg, TWSE_ORDERSUBSYSTEM_TIMEOUT );
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
void TSECConfirmConnectionObject::LockLoop( void )
{
    while( FNeedLock == TRUE ) // This PVC had been lock.
    {
        sleep( 2 );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TSECConfirmConnectionObject::WaitForConnected( void )
{
    while( FConnection->GetSocketFD() == -1 ) 
    {
        UFC::SleepMS( 200 );
    }
}
//----------------------------------------------------------------------------------------------------------------------
BOOL TSECConfirmConnectionObject::Accept( int FD )
{
    if( FConnection == NULL )
        return FALSE;
    FConnection->SetSocketFD( FD );
    return TRUE;
}
//----------------------------------------------------------------------------------------------------------------------
void TSECConfirmConnectionObject::MessageLoop( void )
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
                    if( ThreadClock::SecondOfToday() - FPreviousTime > TWSE_EX_IDLEHANDSHAKE_TIME_XBIO3 )
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
void TSECConfirmConnectionObject::SetLogManager( UFC::BufferedLog* LogObj )
{
    FLog = LogObj;
    if( FConnection != NULL )
        FConnection->SetLogManager( FLog );
}
//----------------------------------------------------------------------------------------------------------------------
void TSECConfirmConnectionObject::Execute( void )
{
    
    FConnection = new LinkSubSystemTAIFEX( IsTSEC,*GetConnectionParameter());
    if( FLog != NULL )
        FConnection->SetLogManager( FLog );
    for( int i = 0; i< TWSE_PVC_BUSY_RETRY_TIMES; i++ )
    {
        try
        {
            if( i == TWSE_PVC_BUSY_RETRY_TIMES - 1 )
            {
                i = 0;
                FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_X25ERROR );// Can not establish the X.25 connection
                FNeedLock = TRUE;
                LockLoop();
            }
            WaitForConnected();
            FConnection->Open( *GetConnectionParameter() );
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
            FConnection->SetSocketFD( -1 );
            sleep( 2 );
            continue;
        }
        catch( exception& e )
        {
            UFC::BufferedLog::Printf( " PVC %d Exception:%s",GetConnectionParameter( )->GetPVC(),e.what());
            FConnection->SetSocketFD( -1 );            
            FNeedLock = TRUE;
            LockLoop();
        }
        catch(...)
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR );// Unknown exception.
            FConnection->SetSocketFD( -1 );            
            FNeedLock = TRUE;
            LockLoop();
        }
    }
    delete FConnection;
    FConnection = NULL;
}
//----------------------------------------------------------------------------------------------------------------------
BOOL TSECConfirmConnectionObject::SendConfirmMessage( void )
{
    MessageR3* OutMsg   = NULL;
    int        MsgCount = 0;

    FMsgCriticalSection.Enter();
    OutMsg = FMessages.GetItem( 0 );
    FMsgCriticalSection.Leave();
    
    MsgCount = OutMsg->GetMessageCount();
    FPreviousTime = ThreadClock::SecondOfToday();
    
    if( FConnection->SendMessage( OutMsg, TWSE_ORDERSUBSYSTEM_TIMEOUT ) == TRUE )
    {
        FMsgCriticalSection.Enter();
        FMessages.Delete( 0 );
        FMsgCriticalSection.Leave();
        FSendPosition += MsgCount;
        delete OutMsg;
        UFC::SleepMS( 1 );
        return TRUE;
    }    
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
void TSECConfirmConnectionObject::AddReportMessage( UFC::PStringList& ExecList )
{
    MessageR3* R3 = NULL;
    R3Body*    NewMsg;
    int        ProcessingTime,Offset;
    int        ReportCount = ExecList.ItemCount();

    for( int i = 0; i < ReportCount; i++ )
    {
        if( R3 == NULL )
            R3 = new MessageR3( IsTSEC );               
        NewMsg = R3Body::CreateFromString( (char*)ExecList.GetItem(i).c_str(), Offset );
        ProcessingTime = NewMsg->ProcessingTime();
        TotalCount++;
        TotalProcessingTime += ProcessingTime;
        if( ProcessingTime > MaxProcessingTime )
            MaxProcessingTime = ProcessingTime;
        if( ( R3->AddMessage( NewMsg ) == FALSE) || (i == ReportCount-1) )
        {
            AddReportMessage( R3 );
            R3 = NULL;
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TSECConfirmConnectionObject::AddReportMessage( MessageR3* R3 )
{
    if( R3 != NULL )
    {
        UFC::PLockObject Lock( FMsgCriticalSection );
        FMessages.Add( R3 );
    }
}
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
