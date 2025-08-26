#include "TMPExecConnectionObject.h"
#include "LinkSubSystem.h"
#include "../../inc/repprotolib.h"
#include "../TMP34/OdrUtility.h"
#include "../../inc/TMP_msg.h"
#include "../../inc/msglib.h"
//----------------------------------------------------------------------------------------------------------------------
extern UFC::AnsiString ReportFilePath;
//----------------------------------------------------------------------------------------------------------------------
//
//  ### TAIFEX side ###
//  Connection object for AP code 8,9 Execution subsystem.
//
//----------------------------------------------------------------------------------------------------------------------
TMPExecConnectionObject::TMPExecConnectionObject( ConnectionParameter& Info, 
                                                  ConnectionListener*  CListener,
                                                  UFC::ThreadListener* TListener )
:ConnectionObjectBase( Info ),
 PThread( TListener, FALSE ),
 FNeedLock( FALSE ),
 FNeedReset( FALSE ),
 FConnected( FALSE ),
 FBackup( FALSE ),
 FSendHeartbeat( 0 ),
 FRecvHeartbeat( 0 ),
 FConnectionListener( CListener ),
 FConnection( NULL ),
 FLog( NULL ),
 FBeginSeq( 0 ),
 FReportFile( NULL ),
 FIsTMP89Connection( TRUE ),
 FReconnectTimes( 0 )
{
    UFC::AnsiString SessionPrefix;

    SessionPrefix.Printf( "[%s,%03d]", FParameters.GetBrokerID().c_str() , FParameters.GetPVC() );
    FSessionPrefixIndex = UFC::BufferedLog::AddPrefix( SessionPrefix );
}
//----------------------------------------------------------------------------------------------------------------------
TMPExecConnectionObject::~TMPExecConnectionObject()
{
    if( FReportFile != NULL  )
        delete FReportFile;
    Close();
}
//----------------------------------------------------------------------------------------------------------------------
void TMPExecConnectionObject::UpdateConnectionParameter( ConnectionParameter& Info )
{
    FParameters = Info;
    FNeedLock   = FALSE;
    FNeedReset  = TRUE;
    FBackup     = FALSE;
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," Changed to CM:%s Password:%s", FParameters.GetBrokerID().c_str(), FParameters.GetPassword().c_str());
}
//----------------------------------------------------------------------------------------------------------------------
void TMPExecConnectionObject::OnCommand( int Command, void* Data )
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
        case EVENT_TMP_EXECUTION:   SendExecution( (TMP::TMPMessage*)Data  );
                                    break;
        case EVENT_TMP_GET_SEQ:     *((int*)Data ) = 0;
                                    break;
        case EVENT_TMP_GET_MSG:     FConnectionListener->OnConnectionNotify( this, CONNECTION_TMP_RECOVER_MSG, Data );
                                    break;
    }           
}
//----------------------------------------------------------------------------------------------------------------------
void TMPExecConnectionObject::SendExecution( TMP::TMPMessage* Report ) 
{
    try
    {        
        if( FConnection->GetSocketFD() != -1 )
        {
            FConnection->SendTMPMessage( Report, TMP_TIMEOUT );
            FSendHeartbeat = 0; 
        }
        else
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," Send Execution to broken session.");
    }
    catch (X25Exception& e) 
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Send Execution Exception:%s.", e.what());
        FNeedReset = TRUE;
    } 
    catch (exception& e) 
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Send Execution Exception:%s", e.what());
        FNeedReset = TRUE;
    } 
    catch (...) 
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Send Execution failed, unknown error");
        FNeedReset = TRUE;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TMPExecConnectionObject::Close( void )
{
    FConnection->StopHandshake();
    if( IsTerminated() == FALSE ) ///< If it's a Non-blocking socket.
    {
        Terminate(); ///< Terminate the socket polling thread.
        WaitFor();   ///< Wait for thread terminated.
        FSendHeartbeat = 0;
        FRecvHeartbeat = 0;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TMPExecConnectionObject::InitFileName( UInt16 CMIndex, UInt8 TargetID )
{
    UFC::AnsiString CMID = TMP::TMPMessage::GetBrokerID( CMIndex );

    if( TargetID == 8 ) ///< Clear Member Confirm/Fill execution.
    {
        CMID.PadThis( 4, ' ');
        FIsTMP89Connection = TRUE;
        FReportFileName.Printf( "%s/%s.89",ReportFilePath.c_str(),CMID.c_str() );
    }
    else if( TargetID == 3 ) ///< FCM Fill execution.
    {
        if( CMID == "F099000" )///<It's TSCD connection
        {
            FIsTMP89Connection = FALSE;
            FReportFileName.Printf( "%s/%s.3",ReportFilePath.c_str(),CMID.c_str());
        }
        else
        {
            CMID.PadThis( 4, ' ');
            FIsTMP89Connection = FALSE;
            FReportFileName.Printf( "%s/%s.3",ReportFilePath.c_str(),CMID.c_str());
        }
    }
    else
    {
        UFC::AnsiString ErrMsg;
        ErrMsg.Printf( "Received unsupport TargetID[%d]", TargetID );
        throw( TMPRehandshakeException( (char*)ErrMsg.c_str() ) );
    }
}
//----------------------------------------------------------------------------------------------------------------------
Int32 TMPExecConnectionObject::GetReportCount( void )
{
    try
    {
        if( UFC::FileExists( FReportFileName ) == TRUE )
        {
            UFC::FileStreamEx  RptFile( FReportFileName, "r" );
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Size of file [%s] is [%d]",FReportFileName.c_str(),RptFile.GetSize() );
            return RptFile.GetSize()/sizeof( REPLOG );
        }
        else
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," File [%s] no exists",FReportFileName.c_str() );
            return 0;
        }
    }
    catch( UFC::Exception& e )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," Open %s exception[%s]",FReportFileName.c_str(),e.what() );
        return 0;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TMPExecConnectionObject::CMInitialiseHandshake( void )
{
    TMP::TMPMessage* Msg;

    if( (Msg = FConnection->ReceiveTMPMessage( TMP_TIMEOUT, TRUE )) != NULL ) ///< Receive the wake up message replay from the TAIFEX server.
    {
        if( Msg->GetMessageType( ) == TMP::tmtR15 )	 ///< yes! a R15
        {
            TMP::TMPMessage R16( TMP::tmtR16 );
            Int32           ReqSeq   = Msg->GetField("req_cm_seq")->ToInteger();
            char            TargetID = Msg->GetField("target_id")->ToInteger();
            Int32           CurrentSeq;

            InitFileName( Msg->GetFCMID(), TargetID );// Msg->GetField("cm_id")->ToInteger(), TargetID );
            CurrentSeq = GetReportCount();
            if( ReqSeq == 0 || ReqSeq >= CurrentSeq)
                FBeginSeq = CurrentSeq;
            else
                FBeginSeq = ReqSeq;
            R16[ "status_code" ] = 0;
            R16[ "target_id" ]   = TargetID;
            R16[ "cm_id" ]       = Msg->GetField("cm_id")->ToInteger();
            R16[ "req_cm_seq" ]  = FBeginSeq;
            FConnection->SendTMPMessage( &R16, TMP_TIMEOUT, TRUE );
            OpenReportFile();
            delete Msg;
        }
        else
        {
            delete Msg;
            throw( TMPRehandshakeException( "Should receive R15 ") );
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TMPExecConnectionObject::OpenReportFile( void )
{
    try
    {
        if( UFC::FileExists(FReportFileName ) == TRUE )
        {
            Int32 FileOffset = (FBeginSeq-1)*sizeof( REPLOG );
            if( FileOffset < 0 )
                FileOffset = 0;
            if( FReportFile != NULL)
                delete FReportFile;
            FReportFile = new UFC::FileStream( FReportFileName, O_RDONLY );
            FReportFile->Seek( FileOffset, UFC::soFromBeginning );
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " File [%s] opened, Begin seq[%d]",FReportFileName.c_str(),FBeginSeq );
        }
    }
    catch( UFC::Exception& e )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Open %s failed[%s]",FReportFileName.c_str(),e.what() );
    }

}
//----------------------------------------------------------------------------------------------------------------------
void TMPExecConnectionObject::ConfirmsubSystemHandshake( BOOL ReceivedL010, UInt8 StatusCode )
{
    if( FConnected == TRUE )
    {
        FConnected = FALSE;
        FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
    }
    FNeedLock = (!FConnection->OnlineHandshake( ReceivedL010, StatusCode ));
    FSendHeartbeat = 0;
    FRecvHeartbeat = 0;
    if( FNeedLock == TRUE )
    {
        FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_LOCKED ); // Lock this connection.
    }
    else
    {
        CMInitialiseHandshake();
        FConnected = TRUE;        
        FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_READY );        
        FReconnectTimes++;
        if( FReconnectTimes %100 == 0 )
            COUT.MSG( TMP89_WARNING,"BrokerID[%s] Session[%03d] reconnect % times.", FParameters.GetBrokerID().c_str() , FParameters.GetPVC(), FReconnectTimes );
    }
}
//----------------------------------------------------------------------------------------------------------------------
TMP::TMPMessage* TMPExecConnectionObject::CheckExecution( void )
{
    REPLOG NewReport;
    TMP::TMPMessage* InMsg;
    TMP::TMPMessage* OutMsg;
    
    if( FReportFile != NULL )
    {
        Int32 ReadPos  = FReportFile->GetPosition();
        Int32 ReadSize = FReportFile->Read((void*)&NewReport,sizeof(REPLOG));

        if( ReadSize == sizeof(REPLOG)) //OK!!
        {
            if( FIsTMP89Connection == TRUE )
            {
                NewReport.RPT.session_seq = NewReport.TMP89_SEQ;                
                if( NewReport.CM8_SEQ == 0 )
                {
                    OutMsg = OdrUtility::odmRptToTMPMessage( (odm_Rpt*)&(NewReport.RPT), FALSE, 9 );
                    OutMsg->SetField( "uniq_id", NewReport.CM9_SEQ );
                }
                else
                {
                    OutMsg = OdrUtility::odmRptToTMPMessage( (odm_Rpt*)&(NewReport.RPT), FALSE, 8 );
                    OutMsg->SetField( "uniq_id", NewReport.CM8_SEQ );
                }
                if( OutMsg->GetField("ExecType")->ToInteger() == '6' )
                {
                    if( NewReport.RPT.combined_match_type == 0 ) ///< New Order.
                        OutMsg->SetField( "ExecType", '0' ); ///< New Order.
                    else
                        OutMsg->SetField( "ExecType", 'F' ); ///< Order filled.
                }
            }
            else
            {
                if( NewReport.RPT.header.protocol == PRO_X25 )
                {
                    UFC::AnsiString SrcFCM = TMP::TMPMessage::GetBrokerID( NewReport.RPT.header.source_fcm_id );
                    Int32 LBID = SrcFCM.SubString( 4, 3 ).ToInt();
                    NewReport.RPT.SysOrderID = LBID*1000000+ NewReport.FCM_SEQ;
                }
                else
                    NewReport.RPT.SysOrderID = NewReport.RPT.header.session_id*1000000+ NewReport.RPT.session_seq;
                NewReport.RPT.session_seq = NewReport.HB3_SEQ;
                OutMsg = OdrUtility::odmRptToTMPMessage( (odm_Rpt*)&(NewReport.RPT), FALSE, 3 );
            }
            
            return OutMsg;
        }
        else if( ReadSize > 0 )
             FReportFile->Seek( ReadPos, UFC::soFromBeginning );
    }
    else
        OpenReportFile();
    if( FNeedReset == TRUE ) // Received PVC reset command.
    {
        FNeedReset = FALSE;
        throw( X25Exception( 0, "Received Session reset command.") );
    }
    if( ( InMsg = FConnection->ReceiveTMPMessage( (long) 1, FALSE ) ) != NULL )
    {
        FRecvHeartbeat = 0;
        if( InMsg->GetMessageType( ) == TMP::tmtL10 )
        {
            delete InMsg;
            throw( X25ReconnectException( "Receive a L010." ) );
        }
        return InMsg;
    }
    return NULL;///< Not receive any message during ORDERSUBSYSTEM_TIMEOUT.
}
//----------------------------------------------------------------------------------------------------------------------
void TMPExecConnectionObject::MessageLoop( void )
{
    TMP::TMPMessage* OutMsg;

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
                    if( (OutMsg = CheckExecution() ) != NULL ) ///< Any execution?
                    {
                        if( OutMsg->GetMessageType() == TMP::tmtR04 ) ///< Received Heartbeat, Reply confirm.
                        {
                            TMP::TMPMessage R05( TMP::tmtR05 ); // R050 Confirm Connection message                       
                            FConnection->SendTMPMessage( &R05, TMP_TIMEOUT, TRUE);
                            FSendHeartbeat = 0;
                            delete OutMsg;
                        }
                        else if( OutMsg->GetMessageType() == TMP::tmtR05 ) ///< Received Heartbeat message.
                        {
                            FSendHeartbeat = 0;
                            delete OutMsg;
                        }
                        else if( OutMsg->GetMessageType() == TMP::tmtR02 )
                        {
                            FConnection->SendTMPMessage( OutMsg, TMP_TIMEOUT, TRUE );
                            FSendHeartbeat = 0;
                            delete OutMsg;
                        }
                        else
                        {
                            UFC::AnsiString ErrStr;
                            ErrStr.Printf( "Received unhandled TMP message type[%d]", OutMsg->GetMessageType() );
                            delete OutMsg;
                            throw( X25RehandshakeException( (char*)ErrStr.c_str() ) );
                        }
                    }
                    else
                    {
                        FSendHeartbeat++;
                        FRecvHeartbeat++;
                    }
                    if( FSendHeartbeat == FConnection->GetHeartBeatInterval() ||
                        FRecvHeartbeat == FConnection->GetHeartBeatInterval()) ///< Time is up!
                    {
                        TMP::TMPMessage  R04(TMP::tmtR04 ); // R040 Confirm Connection message                    
                        FConnection->SendTMPMessage( &R04, TMP_TIMEOUT, TRUE );// Send the confirm connection message    
                        FSendHeartbeat = 0;
                    }                   
                    if( FSendHeartbeat > FConnection->GetHeartBeatInterval() + 5  ||
                        FRecvHeartbeat > FConnection->GetHeartBeatInterval() + 5 )///< Broker Timeout!!
                    {
                        UFC::BufferedLog::Printf( " Heartbeat Timeout" );
                        throw( X25Exception( 0, "Heartbeat Timeout.") );
                    }          
                }     
            } while(  IsTerminated() == FALSE );
        }
        catch( TMPRehandshakeException& e )//< Error, send back a L01 with errorcode.
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlWarning, " %s Code:%d", e.what(), e.GetErrorCode() );
            ConfirmsubSystemHandshake( TRUE, e.GetErrorCode() );
        }
        catch( X25RehandshakeException& e) // Receive a L010...go back to handshake.
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlWarning, " %s", e.what() );
            ConfirmsubSystemHandshake( TRUE );
        }
        catch( X25ReconnectException& e)  // Timeout or reconnect
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlWarning, " %s", e.what() );
            ConfirmsubSystemHandshake( FALSE );
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TMPExecConnectionObject::LockLoop( void )
{
    while( FNeedLock == TRUE &&  IsTerminated() == FALSE ) // This PVC had been lock.
    {
        sleep( 2 );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TMPExecConnectionObject::SetLogManager( LogManager* LogObj )
{
    FLog = LogObj;
    if( FConnection != NULL )
        FConnection->SetLogManager( FLog );
}
//----------------------------------------------------------------------------------------------------------------------
void TMPExecConnectionObject::WaitForConnected( void )
{
    while( FConnection->GetSocketFD() == -1 ) 
    {
        UFC::SleepMS( 200 );
    }
}
//----------------------------------------------------------------------------------------------------------------------
BOOL TMPExecConnectionObject::IsReady( void ) 
{ 
    if( FConnection != NULL )
        return FConnection->IsReady();
    return FALSE;
}        
//----------------------------------------------------------------------------------------------------------------------
BOOL TMPExecConnectionObject::Accept( int FD )
{
    if( FConnection == NULL  ) ///< Connection object NOT exists.
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Accept connection failed, Connection object not exists." );
        return FALSE;
    }
    if( FConnection->IsReady())///< Connection exists, and pass link-subsystem.
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Accept connection failed, Session already in used." );
        return FALSE;
    }
    try
    {
        if( FConnection->GetSocketFD() != -1 )
        {
            UFC::PClientSocket CheckSocket( FConnection->GetSocketFD() );
            CheckSocket.CheckConnection( 200000000 );///< Check 200 ms.
            CheckSocket.Detach( );
            if( FConnection->IsReady() == FALSE ) ///< Not pass link-sub system
            {
                FConnection->SetSocketFD( -1 );
                FNeedReset = TRUE;
                FConnection->StopHandshake();
                return FALSE;
            }
        }
    }
    catch( UFC::SocketException& )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Holds a bad socket. Release it." );
        FConnection->SetSocketFD( -1 );
        FNeedReset = TRUE;
        FConnection->StopHandshake();
        return FALSE;
    }
    ///< Is this socket in use ?
    if( FConnected == TRUE || FConnection->GetSocketFD() != -1 ) ///< Yes, reject the new connection.
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," Accept connection failed, Session already in use." );
        return FALSE;
    }
    else ///< No, accept the new connection.
    {
        FSendHeartbeat = 0;
        FRecvHeartbeat = 0;
        FConnection->SetSocketFD( FD );
        return TRUE;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TMPExecConnectionObject::Execute( void )
{
    FConnection = new LinkSubSystemTAIFEX( FParameters );
    FConnection->SetListener( this );
    if( FLog != NULL )
        FConnection->SetLogManager( FLog );
    for( int i = 0; i< PVC_BUSY_RETRY_TIMES; i++ )
    {
        if( IsTerminated() == TRUE )
            break;
        try
        {
            if( i == PVC_BUSY_RETRY_TIMES - 1 )
            {
                FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_X25ERROR );// Can not establish the X.25 connection
                i = 0;
                FNeedLock = TRUE;
                LockLoop();
            }
            WaitForConnected();
            FConnection->Open( FParameters );
            ConfirmsubSystemHandshake( FALSE );
            MessageLoop();
            FConnection->Close();
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_NORMAL_CLOSE );// Close the connection normaly.
            break;
        }
        catch( X25Exception& e )
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," %s. Try to reconnect after 2 sec...",e.what() );
            FConnected = FALSE;
            FConnection->Close();
            FConnection->SetSocketFD( -1 );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_X25ERROR );
            sleep( 2 );
            continue;
        }
        catch( exception& e )
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," Exception:%s", e.what());
            FConnected = FALSE;
            FConnection->Close();
            FConnection->SetSocketFD( -1 );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
            sleep( 2 );
            continue;
        }
        catch(...)
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR );// Unknown exception.
            FNeedLock = TRUE;
            FConnected = FALSE;
            FConnection->SetSocketFD( -1 );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_X25ERROR );
            LockLoop();
        }
    }
    delete FConnection;
    FConnection = NULL;
}
//----------------------------------------------------------------------------------------------------------------------
