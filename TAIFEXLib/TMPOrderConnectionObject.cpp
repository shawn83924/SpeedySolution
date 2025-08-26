#include "TMPOrderConnectionObject.h"
#include "LinkSubSystem.h"
#include "../TMP34/OdrUtility.h"
#include "../../inc/TMP_msg.h"
#include "../../inc/msglib.h"
//----------------------------------------------------------------------------------------------------------------------
extern int TimeDiffMax;
//------------------------------------------------------------------------------
// Throughput control
//------------------------------------------------------------------------------
extern float FirstThroughputWarning;
extern float LastThroughputWarning;
extern float DisconnectThroughput;
extern float DisconnectCumThroughput;
extern Int32 CumThroughputSec;
extern Int32 StopSessionTimes;
extern BOOL  DisableOrderFlowCtrl;
extern BOOL  DisableExecFlowCtrl;
//----------------------------------------------------------------------------------------------------------------------    
ThroughputCum::ThroughputCum( Int32 Sec )
:FPos( 0 )
,FCount( Sec )        
{
    FThroughputs = new Int32[ Sec ];
    memset( FThroughputs, 0, sizeof(Int32)*FCount );
}        
//----------------------------------------------------------------------------------------------------------------------    
ThroughputCum::~ThroughputCum( void )
{
    delete [] FThroughputs;
}
//----------------------------------------------------------------------------------------------------------------------
void  ThroughputCum::Clear( void )
{
    FPos = 0;
    memset( FThroughputs, 0, sizeof(Int32)*FCount );
}
//----------------------------------------------------------------------------------------------------------------------    
void  ThroughputCum::PutThroughput( Int32 Orders )
{
    *( FThroughputs + FPos ) = Orders;
    FPos++;
    if( FPos == FCount )
        FPos = 0;
}
//----------------------------------------------------------------------------------------------------------------------    
Int32 ThroughputCum::GetCumThroughput( void )
{
    Int32 Total = 0;
    for( register Int32 i = 0; i < FCount; i++ )
         Total += *( FThroughputs + i );
    return Total;
}       
//----------------------------------------------------------------------------------------------------------------------
// Sending Execution thread.
//----------------------------------------------------------------------------------------------------------------------
SendingThread::SendingThread( TMPOrderConnectionObject* Connection )
:PThread( NULL, FALSE )
,FOdrConnection( Connection )
,IsRunning( FALSE )
,FCompactMode( FALSE )
,FMessagesPerSend( 1 )
,FSessionPrefixIndex( 0 )
{
    Start();
}
//----------------------------------------------------------------------------------------------------------------------
BOOL  SendingThread::AddExecution( odm_Rpt* Exec, ExecutionSource Src )
{
    return FExecutions.AddExecution( Exec->session_seq, Exec, Src );
}
//----------------------------------------------------------------------------------------------------------------------
void  SendingThread::SetSessionPrefixIndex( Int32 Index )
{
    FSessionPrefixIndex = Index;
    FExecutions.SetSessionPrefixIndex( Index );
}
//----------------------------------------------------------------------------------------------------------------------
void  SendingThread::Execute( void )
{
     while( TRUE )   
     {
         if( IsRunning == TRUE )
         {
            if( FOdrConnection->IsReady() == TRUE )
            {
                UFC::List<TMP::TMPMessage*> ReportMessages;
                odm_Rpt* NewExec;
                BOOL     BufferFilled = FALSE;

                if( FExecutions.PopExecution( NewExec, 2 ) == TRUE )
                {
                    ReportMessages.Add( OdrUtility::odmRptToTMPMessage( NewExec, FCompactMode ) ); ///< Add to sending list.
                    while( FExecutions.PopExecutionNoWait( NewExec ) == TRUE )
                    {
                        ReportMessages.Add( OdrUtility::odmRptToTMPMessage(  NewExec, FCompactMode ) ); ///< Add to sending list.
                        if( ReportMessages.ItemCount() >= FMessagesPerSend )
                        {
                            BufferFilled = TRUE;
                            break;
                        }
                    }
                    if( BufferFilled == FALSE ) ///< Buffer nor filled, keep collect executions.
                    {
                        UFC::SleepNS( TMP_EXEC_COLLECT_TIME );///< wait executions for 1 ms
                        while( FExecutions.PopExecutionNoWait( NewExec )  == TRUE )
                        {
                            ReportMessages.Add( OdrUtility::odmRptToTMPMessage( NewExec, FCompactMode ) ); ///< Add to sending list.
                            if( ReportMessages.ItemCount() >= FMessagesPerSend )
                            {
                                BufferFilled = TRUE;
                                break;
                            }
                        }
                    }
                    FExecutions.ControlTroughput( ReportMessages.ItemCount() );
                    FOdrConnection->SendExecutions( ReportMessages );
                    for( register int i = 0; i < ReportMessages.ItemCount(); i ++ )
                         delete ReportMessages.GetItem( i );
                }
            }
            else
            {
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Session broken, Stop execution thread" );
                IsRunning = FALSE;
                UFC::SleepMS( 100 );
            }
         }
         else
             UFC::SleepMS( 100 );
     }
}
//----------------------------------------------------------------------------------------------------------------------
void  SendingThread::StartAt( Int32 StartPos,Int32 Throuthput, BOOL CompactExec, Int32 MessagesPerSend )
{
    FCompactMode     = CompactExec;
    FMessagesPerSend = MessagesPerSend;
    FExecutions.SetThroughput( Throuthput );
    FExecutions.Goto( StartPos );
    IsRunning = TRUE;
}
//----------------------------------------------------------------------------------------------------------------------
//
//  ### TAIFEX side ###
//  Connection object for AP code 4 Order subsystem.
//
//----------------------------------------------------------------------------------------------------------------------
TMPOrderConnectionObject::TMPOrderConnectionObject( ConnectionParameter& Info, 
                                                    TransactionObject* TObj, 
                                                    ConnectionListener* CListener,
                                                    UFC::ThreadListener* TListener )
:ConnectionObjectBase( Info ),
 PThread( TListener, FALSE ),
 FNeedLock( FALSE ),
 FNeedReset( FALSE ),
 FConnected( FALSE ),
 FBackup( FALSE ),
 FSendHeartbeat( 0 ),
 FRecvHeartbeat( 0 ),
 FTransactionObj( TObj ),
 FConnectionListener( CListener ),
 FConnection( NULL ),
 FLog( NULL ),
 FTick( UFC::GetTickCountMS() ),         
 FThroughputCount( 0 ),
 FThroughputCountPrev( 0 ),
 FThroughput( 0 ),
 FCompactMode( FALSE ),
 FWarningThroughput( (Int32)(FirstThroughputWarning*(float)Info.GetThroughput()) ),
 FAlarmThroughput((Int32)(LastThroughputWarning*(float)Info.GetThroughput())),
 FDisconnectThroughput((Int32)(DisconnectThroughput*(float)Info.GetThroughput())),
 FMaxCumThroughput((Int32)(DisconnectCumThroughput*(float)Info.GetThroughput())),
 FThroughputOverCount( 0 ),
 FThroughputCum( CumThroughputSec ),
 FSendingThread( this ),
 FLastR12Tick( 0 ),
 FReconnectTimes( 0 )
{
    UFC::AnsiString SessionPrefix;
    
    SessionPrefix.Printf( "[%s,%03d]", FParameters.GetBrokerID().c_str() , FParameters.GetPVC() );
    FSessionPrefixIndex = UFC::BufferedLog::AddPrefix( SessionPrefix );
    FSendingThread.SetSessionPrefixIndex( FSessionPrefixIndex );
}
//----------------------------------------------------------------------------------------------------------------------
TMPOrderConnectionObject::~TMPOrderConnectionObject()
{
    Close();
}
//----------------------------------------------------------------------------------------------------------------------
BOOL  TMPOrderConnectionObject::CanProcessR12( void )
{
    UInt32 TickNow = UFC::GetTickCountMS();
    UInt32 Diff    = TickNow - FLastR12Tick;

    if( Diff/1000 >= 10 ) ///< OK! lager than 10 sec.
    {
        FLastR12Tick = TickNow; //< Update last R12 tick.
        return TRUE;
    }
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
void TMPOrderConnectionObject::UpdateConnectionParameter( ConnectionParameter& Info )
{
    FParameters = Info;
    FNeedLock   = FALSE;
    FNeedReset  = TRUE;
    FBackup     = FALSE;
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Changed to FCM:%s Password:%s",  FParameters.GetBrokerID().c_str(), FParameters.GetPassword().c_str());
}
//----------------------------------------------------------------------------------------------------------------------
void TMPOrderConnectionObject::OnCommand( int Command, void* Data )
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
                                    FConnection->ResetStat( );
                                    break;
        case COMMAND_LOCK_PVC:      FNeedLock  = TRUE;
                                    FBackup    = TRUE;
                                    break;
        case COMMAND_GET_STATE:     *((int*)Data ) = FConnection->GetStatus();
                                    break;
        case COMMAND_GET_BANDWIDTH: *((int*)Data ) = (int)FConnection->GetSessionBandwidth();
                                    break;
        case EVENT_TMP_EXECUTION:   SendExecution( (TMP::TMPMessage*)Data  );
                                    break;
        case EVENT_TMP_NOTIFY:      SendExecution( (TMP::TMPMessage*)Data, TRUE  );
                                    break;
        case EVENT_TMP_GET_SEQ:     FConnectionListener->OnConnectionNotify( this, CONNECTION_TMP_SESSION_SEQ, Data );
                                    break;
        case EVENT_TMP_GET_MSG:     FConnectionListener->OnConnectionNotify( this, CONNECTION_TMP_RECOVER_MSG, Data );
                                    break;
    }       
}
//----------------------------------------------------------------------------------------------------------------------
void TMPOrderConnectionObject::CheckExecutionDelay( TMP::TMPMessage* Report )
{
    TMP::TMPMessageType Type = Report->GetMessageType();

    if( Type == TMP::tmtR02 || Type == TMP::tmtR22) ///Confirm or fill execution
    {
        char ExecType;
        Report->GetField("ExecType")->ToBinary((UInt8*) & ExecType);
        if (ExecType != 'I') ///< Order status execution skip this check.
        {   ///< Check the Match /Sent time defference.
            TMP::TMPTime* MatchTime = (TMP::TMPTime*)Report->GetField("TransactTime");
            TMP::TMPTime& SentTime  = Report->GetMessageTime();
            Int32 DiffInMS = SentTime.DiffMS(*MatchTime);

            if (DiffInMS >= TMP_EXEC_THROUGHPUT_1) ///< Reach warning level 1
            {
                if (DiffInMS >= TMP_EXEC_THROUGHPUT_2) ///< Reach warning level 2 ?
                    Report->SetField("status_code", 247);
                else
                    Report->SetField("status_code", 246);
            }
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TMPOrderConnectionObject::SendExecution( TMP::TMPMessage* Report, BOOL KeepIfBroken )
{
    try
    {        
        if( FConnection->GetSocketFD() != -1 && FConnection->IsReady() == TRUE )
        {
            CheckExecutionDelay( Report );
            FConnection->SendTMPMessage( Report, TMP_TIMEOUT );
            FSendHeartbeat = 0;
        }
        else
        {
            if( KeepIfBroken == FALSE )
            {
                UFC::AnsiString MsgString;
                Report->ToString( MsgString );
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," Send Execution to broken session.");
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," ignore msg[%s].",MsgString.c_str());
            }
            else
            {
                TMP::TMPMessage* CopyReport = new TMP::TMPMessage( *Report );
                FNotifies.Add( CopyReport );
            }
        }
    }
    catch (X25Exception& e) 
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," Send Execution Exception:%s.", e.what());
        FNeedReset = TRUE;
    } 
    catch (exception& e) 
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," Send Execution Exception:%s", e.what());
        FNeedReset = TRUE;
    } 
    catch (...) 
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Send Execution failed, unknown error");
        FNeedReset = TRUE;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TMPOrderConnectionObject::SendExecutions( UFC::List< TMP::TMPMessage*>& Reports )
{
    try
    {
        if( FConnection->GetSocketFD() != -1 )
        {
            if( FConnection->IsReady() == TRUE )
            {
                for( int i = 0; i < Reports.ItemCount(); i ++ )
                     CheckExecutionDelay( Reports.GetItem( i ) );
                FConnection->SendTMPMessages( Reports, TMP_TIMEOUT );
                FSendHeartbeat = 0;
            }
            else
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," Send Executions to broken session.");
        }
        else
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," Send Executions to socket broken session.");
    }
    catch (X25Exception& e)
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," Send Executions Exception:%s.", e.what());
        FNeedReset = TRUE;
    }
    catch (exception& e)
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," Send Executions Exception:%s", e.what());
        FNeedReset = TRUE;
    }
    catch (...)
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex," Send Executions failed, unknown error");
        FNeedReset = TRUE;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TMPOrderConnectionObject::Close( void )
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
void TMPOrderConnectionObject::ConfirmsubSystemHandshake( BOOL ReceivedL010, UInt8 StatusCode )
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
        FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_LOCKED );// Lock this connection.
    }
    else
    {
        FConnected = TRUE;
        if( FConnection->GetAPCode() == '6' )
        {
            FCompactMode = TRUE;
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Exeution format: R22" );
        }
        else
        {
            FCompactMode = FALSE;
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Exeution format: R02" );
        }
        switch( FConnection->GetSessionBandwidth() )
        {
            case lb128K: FDisconnectThroughput = (Int32)(DisconnectThroughput*(float)FParameters.GetThroughput()*0.625);
                         if( FCompactMode == TRUE )
                             FMessagesPerSend = TMP_128K_CMSG_MAX;
                         else
                             FMessagesPerSend = TMP_128K_MSG_MAX;
                         UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Leased line: 128K" );
                         break;
            case lb256K:
            case lb512K: FDisconnectThroughput = (Int32)(DisconnectThroughput*(float)FParameters.GetThroughput()*0.8);
                         if( FCompactMode == TRUE )
                             FMessagesPerSend = TMP_512K_CMSG_MAX;
                         else
                             FMessagesPerSend = TMP_512K_MSG_MAX;
                         UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Leased line: 512K" );
                         break;
            case lbE1:   FDisconnectThroughput = (Int32)(DisconnectThroughput*(float)FParameters.GetThroughput());
                         if( FCompactMode == TRUE )
                             FMessagesPerSend = TMP_2M_CMSG_MAX;
                         else
                             FMessagesPerSend = TMP_2M_MSG_MAX;
                         UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Leased line: 2048K" );
                         break;
        }
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Throughput:%d", FParameters.GetThroughput());
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Warning Throughput:%d",FWarningThroughput);
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Alarm Throughput:%d",FAlarmThroughput);
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Max Throughput:%d",FDisconnectThroughput);
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Cum Throughput:%d",FMaxCumThroughput );
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Exeution Throughput:%d", FDisconnectThroughput );
        FThroughputCount = FThroughputCountPrev; ///< Reset throughput calculation
        FThroughputCum.Clear();                  ///< Reset Cum throughput calculation.
        FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_READY );
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Execution Start at:%d",FConnection->GetOutboundSeq() );
        if( DisableExecFlowCtrl == TRUE )
            FSendingThread.StartAt( FConnection->GetOutboundSeq(), 2000, FCompactMode, FMessagesPerSend );
        else
            FSendingThread.StartAt( FConnection->GetOutboundSeq(), FDisconnectThroughput, FCompactMode, FMessagesPerSend );
        FReconnectTimes++;
        if( FReconnectTimes %100 == 0 )
            COUT.MSG( TMP34_WARNING, "BrokerID[%s] Session[%03d] reconnect % times.", FParameters.GetBrokerID().c_str() , FParameters.GetPVC(), FReconnectTimes );
        if( FNotifies.ItemCount() > 0 ) ///< Resend R14 20 msg/sec
        {            
            for( register int i = 0; i < FNotifies.ItemCount(); i ++ )
            {
                TMP::TMPMessage* ResendNotify = FNotifies.GetItem( i );
                SendExecution( ResendNotify );
                delete ResendNotify;
                UFC::SleepMS( 50 );
            }
            FNotifies.Clear();
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
TMP::TMPMessage* TMPOrderConnectionObject::CheckReceiveMessage( void )
{
    TMP::TMPMessage* InMsg;
  
    if( FNeedReset == TRUE ) // Received PVC reset command.
    {
        FNeedReset = FALSE;
        throw( X25Exception( 0, "Received Session reset command.") );
    }
    if( ( InMsg = FConnection->ReceiveTMPMessage( (long) 1, FALSE ) ) != NULL )
    {        
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
void TMPOrderConnectionObject::HandleThroughputExeedException( const UFC::AnsiString& Reason ) 
{
    FThroughputOverCount++;
    if( FThroughputOverCount >= StopSessionTimes )
    {
        FThroughputOverCount = 0;
        FNeedLock = TRUE;
        FConnection->SetStatus( PVC_STATUS_THROUGHPUT_ERR );
        FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_LOCKED );// Lock this connection.        
    }
    throw( TMPRehandshakeException( (char*)Reason.c_str(), 240) );
}    
//----------------------------------------------------------------------------------------------------------------------
UInt8 TMPOrderConnectionObject::CalculateThroughput( void) 
{
    UInt32 Now = UFC::GetTickCountMS();
    Int32  TimeDiff = Now - FTick;
    UInt8  WarningCode = 0;

    FThroughputCount++;    
    Int32  OrderDiff = FThroughputCount - FThroughputCountPrev;
    if( TimeDiff >= 1000 ) 
    {        
        FThroughput = (double)OrderDiff*1000.0 / (double) TimeDiff;
        FTick = Now;
        FThroughputCountPrev = FThroughputCount;
        int IdleSec = (TimeDiff/1000)-1;
        for( int i = 0; i < IdleSec; i++ )
             FThroughputCum.PutThroughput( 0 );
        FThroughputCum.PutThroughput( (Int32) FThroughput );
        if( FThroughput < FWarningThroughput )
        {
            WarningCode = 0;
        }        
        else if( FThroughput >= FWarningThroughput && FThroughput < FAlarmThroughput)
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlPerformance,FSessionPrefixIndex, " Throughput exceed %d.",FWarningThroughput );
            WarningCode = 248;
        }
        else if( FThroughput >= FAlarmThroughput && FThroughput < FDisconnectThroughput )
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlPerformance,FSessionPrefixIndex, " Throughput exceed %d.",FAlarmThroughput );
            WarningCode = 249;
        }
        else 
        {        
            UFC::BufferedLog::DebugPrintf( UFC::dlPerformance,FSessionPrefixIndex,  " Throughput exceed %d.",FDisconnectThroughput );
            HandleThroughputExeedException( "Throughput exceed limition" );
        }        
    }
    else
    {
        if( OrderDiff < FWarningThroughput )
        {
            WarningCode = 0;
        }        
        else if( OrderDiff >= FWarningThroughput && OrderDiff < FAlarmThroughput)
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlPerformance,FSessionPrefixIndex, " Throughput exceed %d.",FWarningThroughput );
            WarningCode = 248;
        }
        else if( OrderDiff >= FAlarmThroughput && OrderDiff < FDisconnectThroughput )
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlPerformance,FSessionPrefixIndex, " Throughput exceed %d.",FAlarmThroughput );
            WarningCode = 249;
        }
        else 
        {        
            UFC::BufferedLog::DebugPrintf( UFC::dlPerformance,FSessionPrefixIndex, " Burst throughput exceed %d.",FDisconnectThroughput );
            HandleThroughputExeedException( "Burst throughput exceed limition" );
        }   
    }
    Int32 CumThroughput = FThroughputCum.GetCumThroughput();

    if( CumThroughput >= FMaxCumThroughput )
    {        
        UFC::BufferedLog::DebugPrintf( UFC::dlPerformance,FSessionPrefixIndex, " Cum Throughput exceed %d.", FMaxCumThroughput );
        HandleThroughputExeedException( "Cum throughput exceed limition" );
    }
    UFC::BufferedLog::DebugPrintf( UFC::dlPerformance,FSessionPrefixIndex, " Throughput[%3.1f] CumThroughput[%d].", GetThroughput(), CumThroughput );
    return WarningCode;
}        
//----------------------------------------------------------------------------------------------------------------------
void TMPOrderConnectionObject::ReplyHeartbeat( TMP::TMPMessage* InMsg, UInt8 ThroughputWarning )
{
    TMP::TMPMessage R05( TMP::tmtR05 ); // R050 Confirm Connection message                       
    Int32 TDiff = R05.GetMessageTime().ToInteger() - InMsg->GetMessageTime().ToInteger();

    if (ThroughputWarning != 0)
        R05.SetField("status_code", ThroughputWarning);
    else if (TDiff > TimeDiffMax || TDiff < -1 * TimeDiffMax)
        R05.SetField("status_code", 200); 
    FConnection->SendTMPMessage(&R05, TMP_TIMEOUT, TRUE);
    FSendHeartbeat = 0;
}
//----------------------------------------------------------------------------------------------------------------------
void TMPOrderConnectionObject::MessageLoop( void )
{
    TMP::TMPMessage* InMsg;
    TMP::TMPMessage* OutMsg;
    UInt8            ThroughputWarning = 0;

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
                        FRecvHeartbeat    = 0;
                        if( DisableOrderFlowCtrl == FALSE )
                            ThroughputWarning = CalculateThroughput();
                        if( InMsg->GetMessageType() == TMP::tmtR04 )
                        {
                            ReplyHeartbeat( InMsg, ThroughputWarning );
                        }
                        else
                        {                            
                            if( ( OutMsg = FTransactionObj->OnMessage( this, InMsg, ThroughputWarning ) ) != NULL )
                            {
                                FConnection->SendTMPMessage( OutMsg, TMP_TIMEOUT, TRUE );
                                FSendHeartbeat = 0;
                                delete OutMsg;
                            }
                        }
                        delete InMsg;
                    }
                    else                    
                        FRecvHeartbeat++;
                    FSendHeartbeat++;
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
                        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Heartbeat Timeout" );
                        throw( X25Exception( 0, "Heartbeat Timeout.") );
                    }          
                }
     
            } while(  IsTerminated() == FALSE );
        }
        catch( TMPRehandshakeException& e )//< Error, send back a L01 with errorcode.
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlWarning,FSessionPrefixIndex, " %s Code:%d", e.what(), e.GetErrorCode() );
            ConfirmsubSystemHandshake( TRUE, e.GetErrorCode() );
        }
        catch( X25RehandshakeException& e) // Receive a L010...go back to handshake.
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlWarning,FSessionPrefixIndex, " %s", e.what() );
            ConfirmsubSystemHandshake( TRUE );
        }
        catch( X25ReconnectException& e)  // Timeout or reconnect
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlWarning,FSessionPrefixIndex, " %s", e.what() );
            ConfirmsubSystemHandshake( FALSE );
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TMPOrderConnectionObject::LockLoop( void )
{
    while( FNeedLock == TRUE &&  IsTerminated() == FALSE ) // This PVC had been lock.
    {
        sleep( 2 );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TMPOrderConnectionObject::SetLogManager( LogManager* LogObj )
{
    FLog = LogObj;
    if( FConnection != NULL )
        FConnection->SetLogManager( FLog );
}
//----------------------------------------------------------------------------------------------------------------------
void TMPOrderConnectionObject::WaitForConnected( void )
{
    while( FConnection->GetSocketFD() == -1 ) 
    {
        UFC::SleepMS( 200 );
    }
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Accept a Socket FD[%d].",FConnection->GetSocketFD());
}
//----------------------------------------------------------------------------------------------------------------------
BOOL TMPOrderConnectionObject::IsReady( void ) 
{ 
    if( FConnection != NULL )
        return FConnection->IsReady();
    return FALSE;
}        
//----------------------------------------------------------------------------------------------------------------------
BOOL TMPOrderConnectionObject::Accept( int FD )
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
void TMPOrderConnectionObject::Execute( void )
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
            sleep( 3 );
            continue;
        }
        catch( exception& e )
        {
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FSessionPrefixIndex, " Exception:%s", e.what());
            FConnected = FALSE;
            FConnection->Close();
            FConnection->SetSocketFD( -1 );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
            sleep( 3 );
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
