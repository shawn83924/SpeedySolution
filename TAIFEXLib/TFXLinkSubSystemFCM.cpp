#include "TFXConnection.h"
#include "TFXLinkSubSystem.h"
#include "TFXSocketConnection.h"
#include "TFXConnectionObject.h"
//-----------------------------------------------------------------------------------------
namespace TFX
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Class LinkSubSystemFCM
//  Implement the link sub-system FCM side protocol.
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LinkSubSystemFCM::LinkSubSystemFCM( BOOL IsOptions, BOOL IsOffHour,int Line, int PVCNumber, UFC::AnsiString Password, UFC::AnsiString BrokerID,char APCode, const char* URL, Int32 CancelOrderSec )
:LinkSubSystemBase( FALSE, IsOptions, IsOffHour, Line, PVCNumber, Password, BrokerID, APCode, URL )
,FCancelOrderSec( CancelOrderSec )
{

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Handshake with Exchange
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemFCM::OnlineHandshake( BOOL ReceivedL010, UInt8 StatusCode )
{
    BOOL  SendL010 = ReceivedL010;
    Int32 ReqSeq   = (Int32)FSequenceIn[0];

    while( TRUE )
    {
        SetConnectionState( FALSE, PVC_STATUS_BROKEN );
        if( FStopHandShake == TRUE )   ///< Force stop handshake.
        {
            FStopHandShake = FALSE;
            throw( X25Exception( 0, "Force stop OnlineHandshake.") );
            return FALSE;
        }
        try
        {
            if( SendL010 == TRUE )///< Need send L010 first.
            {
                SendL010 = FALSE;
                SendMessageL010( StatusCode );
            }
            if( FIsTCP == FALSE ) ///< For X.25 connection
            {
                if( WakeUpHandshake() == TRUE  &&        ///< Pass Server wakeup handshake
                    LoginHandshake() == TRUE &&          ///< and login handshake.
                    SubsystemStartupHandshake() == TRUE )///< and Subsystem startup handshake
                {
                    SetConnectionState( TRUE, PVC_STATUS_ONLINE_READY );///< Link SubSystem online.
                    return TRUE;				        ///< Subsystem startup succeed.
                }
            }
            else ///< For TCP TMP connection.
            {
                if( WakeUpHandshakeTMP() == TRUE  &&        ///< Pass Server wakeup handshake
                    LoginHandshakeTMP() == TRUE &&          ///< and login handshake.
                    SubsystemStartupHandshakeTMP( ReqSeq ) == TRUE )///< and Subsystem startup handshake
                {
                    SetConnectionState(   TRUE, PVC_STATUS_ONLINE_READY );///< Link SubSystem online.
                    return TRUE;				        ///< Subsystem startup succeed.
                }
            }
        }
        catch( X25RehandshakeException& )
        {
            SendL010 = TRUE; ///< Received X25RehandshakeException message, Try to rehandshake.
        }
        catch( X25ReconnectException& )
        {
            SendL010 = TRUE; ///< Received X25RehandshakeException message, Try to rehandshake.
        }
    };
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemFCM::SessionEOD( UInt8 PartID, Int32 InSeq )
{
    if( InSeq == 0 )
	FSequenceIn[PartID]  = (Int32)1; ///< Inbound
    else
        FSequenceIn[PartID]  = InSeq; ///< Inbound
    UFC::BufferedLog::Printf( " Set PartID[%03d] Session[%03d] in_bound_num[%d]", PartID, FPVCNumber, FSequenceIn[PartID] );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Step 1
//  Send a L010 and receive a L010 from Exchange
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemFCM::WakeUpHandshake( void )///< For X.25 connection
{
    MessageL010    L010;
    MessageHeader* Msg;

    if( SendMessage( &L010, TXIFEX_PULL_INTERVAL, FALSE ) == FALSE )		 ///< Send wake up message L010
        return FALSE;
    if( FStopHandShake == TRUE )
        return FALSE;
    while( TRUE )
    {
        if( FStopHandShake == TRUE )
            return FALSE;
        if( (Msg = ReceiveMessage( LINKSUBSYSTEM_TIMEOUT, FALSE )) != NULL ) ///< Receive the wake up message replay from the TAIFEX server.
        {
            if( Msg->IsMessage( mtL010 ) == TRUE )	 ///< yes! a L010
            {
                delete Msg;
                return TRUE;
            }
            delete Msg;
        }
    };
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemFCM::WakeUpHandshakeTMP( void )///< For TCP/IP TMP connection.
{
    TMP::TMPMessage L010( TMP::tmtL10 );
    TMP::TMPMessage* Msg;

    if( SendTMPMessage( &L010, TMP_TIMEOUT, FALSE ) == FALSE )		 ///< Send wake up message L010
        return FALSE;
    if( FStopHandShake == TRUE )
        return FALSE;
    while( TRUE )
    {
        if( FStopHandShake == TRUE )
            return FALSE;
        if( (Msg = ReceiveTMPMessage( TMP_TIMEOUT, FALSE )) != NULL ) ///< Receive the wake up message replay from the TAIFEX server.
        {
            if( Msg->GetMessageType( ) == 10 )	 ///< yes! a L010
            {
                FSequenceOut = Msg->GetField( "start_in_bound_num")->ToInteger();
                delete Msg;
                return TRUE;
            }
            delete Msg;
        }
        TMP::TMPMessage SendL010( TMP::tmtL10 );
        if( SendTMPMessage( &SendL010, TMP_TIMEOUT, FALSE ) == FALSE )		 ///< Send wake up message L010
            return FALSE;
    };
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Step 2
//  Send a L020 and receive a L030 from Exchange
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemFCM::LoginHandshake( void )
{
    MessageL020 L020;		      ///< on line subsystem,power on process,power on confirm message.

    if( SendMessage( &L020, TXIFEX_PULL_INTERVAL, FALSE ) == FALSE )	   ///< Send confirm wakeup message
        return FALSE;
    if( FStopHandShake == TRUE )
        return FALSE;
    MessageHeader* Msg = ReceiveMessage( LINKSUBSYSTEM_TIMEOUT, FALSE );
    if( Msg != NULL && Msg->IsMessage( mtL030 ) == TRUE )
    {
        FAppendNo = (static_cast<MessageL030*>(Msg))->GetAppendNo(); ///< get the Append No
        delete Msg;
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemFCM::Bendwidth( void )
{
    Int32 Roundtrip = UFC::GetTickCountMS( ) - FSendTick;
    if( Roundtrip < 5 )                         ///< RTT < 5 ms
        FBandwidth = lbE1;
    else if( Roundtrip >= 5  && Roundtrip < 11 )///< 5 ms < RTT < 11 ms
        FBandwidth = lb512K;
    else
        FBandwidth = lb128K;                    ///< RTT > 11 ms    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemFCM::LoginHandshakeTMP( void )
{
    TMP::TMPMessage  L020( TMP::tmtL20 );		      ///< on line subsystem,power on process,power on confirm message.
    FSendTick = UFC::GetTickCountMS( );
    BOOL IsFirstL30 = TRUE;

    if( TMP::TMPMessage::IsFLEXEnabled() == TRUE )  ///< Enable FLEX
        L020["status_code"] = 1;                    ///< Version = 1
    if( SendTMPMessage( &L020, TMP_TIMEOUT, FALSE ) == FALSE )	   ///< Send confirm wakeup message
        return FALSE;
    if( FStopHandShake == TRUE )
        return FALSE;
    while( TRUE )
    {
        TMP::TMPMessage* Msg = ReceiveTMPMessage( TMP_TIMEOUT, FALSE );
        if( Msg != NULL && ( Msg->GetMessageType( ) == TMP::tmtL30 || Msg->GetMessageType( ) == TMP::tmtLX30 ) )
        {
            TMP::TMPMessageType Type = Msg->GetMessageType( );
            UInt8  PartID = 0;
            UInt32 end_out_bound_num;
            
            if( IsFirstL30 == TRUE )
            {
                IsFirstL30 = FALSE;
                Bendwidth( );
            }
            end_out_bound_num = Msg->GetField( "end_out_bound_num" )->ToInteger();
            if( Type == TMP::tmtLX30 )
            {
                PartID = Msg->GetField( "part_id" )->ToInteger();
                FPartIDs.Add( PartID );
                UFC::BufferedLog::Printf( " PartID[%03d] Session[%03d] end_out_bound_num[%d]", PartID, FPVCNumber, end_out_bound_num );
            }
            else
            {
                FAppendNo = UFC::AnsiString( Msg->GetField( "append_no" )->ToInteger() );
                UFC::BufferedLog::Printf( " Session[%03d] end_out_bound_num[%d] Logon OK.", FPVCNumber, end_out_bound_num );
            }
            delete Msg;
            if( Type == TMP::tmtL30 ) ///< Receive L30
                return TRUE;          ///< Login OK!
        }
        else
            break;
    }
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Step 3 (final)
//  Send a L040 and receive a L050 from Exchange
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemFCM::SubsystemStartupHandshake( void )
{
    char APCode = FAPCode;
    
    MessageL040 L040( (char*)FAppendNo.c_str(), (char*)FBrokerID.c_str(), APCode, (char*)FPassword.c_str() );

    if( SendMessage( &L040, LINKSUBSYSTEM_TIMEOUT ) == FALSE )	    ///< Replay login message L040
        return FALSE;
    if( FStopHandShake == TRUE )
        return FALSE;
    MessageHeader* Msg = ReceiveMessage( LINKSUBSYSTEM_TIMEOUT, FALSE );
    if( Msg != NULL  )
    {
        if( Msg->IsMessage( mtL050 ) == TRUE ) ///< Receive subsystem startup message
        {
            MessageL060 L060;						///< On line subsystem,login process,AP confirm message.
            delete Msg;
            return SendMessage( &L060, TXIFEX_PULL_INTERVAL, FALSE );	///< Replay subsystem startup confirm message
        }
        else
        {
            UFC::BufferedLog::Printf( " Port:%d PVC:%d Startup Handshake failed Code:%02d.", FLine, FPVCNumber, Msg->GetStatusCode() );
            delete Msg;
            return FALSE;
        }
    }
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemFCM::SubsystemStartupHandshakeTMP( Int32 ReqSeq )
{
    TMP::TMPMessage*   Msg;
    TMP::TMPMessage    L040( TMP::tmtL40 );
    UFC::AnsiString    FileName;
    UInt8              KeyValue = ((FAppendNo.ToInt() * FPassword.ToInt() )/100)%100;/* we need thousand & hundred */
    BOOL               IsEOF = FALSE;

    FileName.Printf( "FCM_%d_Port_%d_SID_%d.Recover.data", FBrokerIndex, FLine, FPVCNumber );
    L040[ "append_no"]         = FAppendNo.ToInt();
    L040[ "fcm_id" ]           = FBrokerIndex;
    L040[ "session_id"]        = FPVCNumber;
    L040[ "system_type"]       = GetTMPSystemType();
    L040[ "key_value"]         = KeyValue;
    L040[ "request_start_num"] = ReqSeq;
    if( FAPCode == '3' || FAPCode == '8' || FAPCode == '9' )
        L040[ "ap_code"] = 8;             ///< 3,8,9: for CM. 
    else
        L040[ "ap_code"] = FAPCode - '0'; ///< 4,6: for FCM.    
    //if( FAPCode != '4' && FAPCode != '6' && L040.FieldExists( "cancel_order_sec" ) == TRUE )///< For TAIFEX Replace Px
    L040[ "cancel_order_sec"] = FCancelOrderSec;                 ///< Only order session needs cancel_order_sec.
    if( SendTMPMessage( &L040, TMP_TIMEOUT, FALSE ) == FALSE )	 ///< Replay login message L040
        return FALSE;
    if( FStopHandShake == TRUE )
        return FALSE;
    UInt32  BeginTime = UFC::GetTickCountMS();
    Int32   RecoveredCount = 0;

    while( (Msg = ReceiveTMPMessage(TMP_TIMEOUT, FALSE ) ) != NULL )
    {
        if( Msg->GetMessageType( ) == 50 ) ///< Receive subsystem startup message
        {
            TMP::TMPTime Now;
            TMP::TMPTime& ExTime = Msg->GetMessageTime();

            Int32 TimeDiff = Now.DiffMS( ExTime );
            if(  TimeDiff > 1600 || TimeDiff < -1600 )
            {
                if( FListener != NULL )
                {
                    long  Time = ExTime.GetHour()*3600000 +
                                 ExTime.GetMinute()*60000 +
                                 ExTime.GetSecond()*1000  +
                                 ExTime.GetMillisecond();
                    void* TimePtr = (void*)(Time);
                    
                    FListener->OnCommand( EVENT_TMP_SET_TIME, TimePtr );
                    UFC::BufferedLog::Printf( " Exchange time: %02d:%02d:%02d.%03d", ExTime.GetHour(),ExTime.GetMinute(),ExTime.GetSecond(),ExTime.GetMillisecond() );
                }
            }
            /// UFC::SetTime(  Msg->GetMessageTime().GetHour(), Msg->GetMessageTime().GetMinute(), Msg->GetMessageTime().GetSecond(),Msg->GetMessageTime().GetMillisecond());
            FHeartBtInt    = Msg->GetField( "HeartBtInt" )->ToInteger();
            FMaxThroughput = Msg->GetField( "max_flow_ctrl_cnt" )->ToInteger();
            delete Msg;
            UFC::BufferedLog::Printf( " Port[%d] SID[%d] Connected.", FLine, FPVCNumber );
            TMP::TMPMessage  L060( TMP::tmtL60 );		    ///< On line subsystem,login process,AP confirm message.
            
            if( TMP::TMPMessage::IsFLEXEnabled() == TRUE )  ///< Enable FLEX
            {
                SendTMPMessage( &L060, TMP_TIMEOUT, FALSE );///< Replay subsystem startup confirm message
                return RecoverFLEX();
            }   
            else
                return SendTMPMessage( &L060, TMP_TIMEOUT, FALSE );///< Replay subsystem startup confirm message
        }
        else if( Msg->GetMessageType( ) == 41 )///< Recover messages
        {
            if( IsEOF == TRUE )
                return FALSE;
            UFC::MemoryStream RecoverMsgs( TMP_MAX_DATA_SIZE  );

            Msg->GetField( "data" )->ToStream( &RecoverMsgs );
            RecoveredCount += OnRecoverMessage( 0, RecoverMsgs );
            if( Msg->GetField( "is_eof" )->ToInteger() == 1 )
            {
                UFC::BufferedLog::DebugPrintf( UFC::dlPerformance, " Recover[%d] TMP messages use[%d ms]",RecoveredCount, UFC::GetTickCountMS()- BeginTime );
                IsEOF = TRUE;
            }
            delete Msg;
            TMP::TMPMessage L042( TMP::tmtL42 );
            if( SendTMPMessage( &L042, TMP_TIMEOUT, FALSE ) == FALSE )
                return FALSE;
        }
        else
        {
            UFC::BufferedLog::Printf( " Port:%d SID:%d Startup Handshake failed Code:%02d.", FLine, FPVCNumber, Msg->GetField( "status_code" )->ToInteger() );
            delete Msg;
            return FALSE;
        }
    }
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int32 LinkSubSystemFCM::OnRecoverMessage( UInt8 PartID, UFC::MemoryStream& Stream  )
{
    TMP::TMPMessage* Msg;
    int RecoverCount = 0;

    try
    {
       Stream.Seek( 0, UFC::soFromBeginning );
       while( ( Msg = new TMP::TMPMessage( &Stream ) ) != NULL )
       {
           if( FListener != NULL )
           {
               FListener->OnCommand( EVENT_TMP_RECOVER_MSG, (void*)Msg);
               UFC::SleepMS( 1 );
           }
           FSequenceIn[ PartID ] = (Int32)Msg->GetMessageSeq();
           RecoverCount++;
           delete Msg;
       }
    }
    catch( UFC::Exception& e )
    {
       return RecoverCount;
    }
    return RecoverCount;
}
//-----------------------------------------------------------------------------------------
void LinkSubSystemFCM::OnRecoverFLEX( TMP::TMPMessage* RX41 )
{
    UInt8             PartID;
    UFC::MemoryStream RecoverMsgs( TMP_MAX_DATA_SIZE  );

    PartID = RX41->GetField( "part_id" )->ToInteger();        
    RX41->GetField( "data" )->ToStream( &RecoverMsgs );    
    OnRecoverMessage( PartID, RecoverMsgs );
    if( RX41->GetField( "is_eof" )->ToInteger() == 1 )
        UFC::BufferedLog::DebugPrintf( UFC::dlPerformance, " Recover PartID[%d] TMP messages finished.", PartID );

}
//-----------------------------------------------------------------------------------------
BOOL LinkSubSystemFCM::RecoverFLEX()
{
    for( int i = 0; i < FPartIDs.ItemCount(); i++ )
    {
        TMP::TMPMessage  RX40( TMP::tmtRX40 );		 
        UInt8            PartID = FPartIDs[ i ];
        
        RX40[ "part_id" ]           = PartID;
        RX40[ "request_start_seq" ] = (Int32)FSequenceIn[PartID];
        if( SendTMPMessage( &RX40, TMP_TIMEOUT, FALSE ) == FALSE ) ///< Reecover PartID Executions.            
            return FALSE;
        UFC::SleepMS( 1 );
    }    
    return TRUE;
}
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
