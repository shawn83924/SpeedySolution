#include "Connection.h"
#include "LinkSubSystem.h"
#include "X25Connection.h"
#include "SocketConnection.h"
#include "ConnectionObject.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Class LinkSubSystemTAIFEX
//  Implement the link sub-system TAIFEX side protocol.
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LinkSubSystemTAIFEX::LinkSubSystemTAIFEX( BOOL IsOptions, int Line, int PVCNumber, char* Password, char* BrokerID,char APCode, const char* URL )
:LinkSubSystemBase( TRUE, IsOptions, Line, PVCNumber, Password, BrokerID, APCode, URL )
{

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Session End of day
// Reset session sequence.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemTAIFEX::SessionEOD( Int32  )
{
    FSequenceIn  = (Int32)0;
    FSequenceOut = (Int32)0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TAIFEX Logon handshake protocol.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemTAIFEX::OnlineHandshake(  BOOL ReceivedL010, UInt8 StatusCode )
{
    int   ErrorCode;
    int   LoginFailedCount = 0;
    BOOL  SendL010 = ReceivedL010;

    while( TRUE )
    {
        if( StatusCode == 0 )
            SetConnectionState( FALSE, PVC_STATUS_BROKEN ); ///< Link SubSystem offline.
        else
            SetConnectionState( FALSE, StatusCode );///< Link SubSystem offline with error.
        if( FStopHandShake == TRUE ) ///< Force stop handshake.
        {
            FStopHandShake = FALSE;
            throw( X25Exception( 0, "Force stop OnlineHandshake.") );
            return FALSE;
        }
        try
        {
            if( SendL010 == TRUE )
            {
                SendL010 = FALSE;
                SendMessageL010( StatusCode );
                if( StatusCode == 202 )
                {
                   UFC::SleepMS( 100 );
                   throw( X25Exception( StatusCode, "BrokerID error.") );
                }
                if(  StatusCode == 205 )
                {
                   UFC::SleepMS( 100 );
                   throw( X25Exception( StatusCode, "SessionID error.") );
                }
                if(  StatusCode == 240 )
                {
                   UFC::SleepMS( 100 );
                   throw( X25Exception( StatusCode, "Throughput exceed limition") );
                }
            }
            if( WakeUpHandshake( ) == TRUE &&	  ///< FCM Server wakeup handshake.(S:L010,R:L020)
                LoginHandshake() == TRUE )	  ///< FCM Server login handshake.(S:L030)
            {                
                while( LoginFailedCount < PVC_LOCK_TIMES )
                {
                    if( (ErrorCode = SubsystemStartupHandshake()) == 0 )///< Subsystem startup handshake.(R:L040,S:L050)
                    {                        
                        ConfirmSubSysStartup( );
                        LoginFailedCount = 0;
                        return TRUE;
                    }
                    else
                    {
                        LoginFailedCount++;
                        if( ErrorCode != -1 )
                            LoginHandshake( ErrorCode );  ///< Resent L030 with errorcode
                        else
                            throw( X25RehandshakeException( "Not a L040 message"));
                    }
                }
                SetConnectionState( FALSE, FLockReason ); ///< Link SubSystem offline.
                return FALSE;
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
//  Function WakeUpHandshake:
//  Handle the WakeUp Handshake,Try to receive the message L020..if not reply the message L010.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemTAIFEX::WakeUpHandshake( void ) ///< Handle L010 and reply L020
{
    if( FStopHandShake == TRUE ) ///< User force stop thehandshake.
        return FALSE;

    if( FIsTCP == FALSE )
    {
        MessageHeader* Msg;
        // Modify on 2004/08/14 by Simon.
        if( (Msg = ReceiveMessage( mtL020, 10 )) != NULL )
        {
            delete Msg;
            return TRUE;  ///< OK! Is a L020, goto the next handshake.
        }
        else
        {
            SendMessageL010( 0 );
            return FALSE;///< Time out, Send a L010 back!!
        }
    }
    else /// For TCP/IP TMP protocol
    {
        TMP::TMPMessage* Msg;

        try
        {
            if( (Msg = ReceiveTMPMessage( (UInt8)20, TMP_TIMEOUT )) != NULL )
            {
                delete Msg;
                return TRUE;  ///< OK! Is a L020, goto the next handshake.
            }
            else
            {
                SendMessageL010( 0 );
                return FALSE;///< Time out, Send a L010 back!!
            }
        }
        catch( TMPRehandshakeException& e )
        {
            SendMessageL010( e.GetErrorCode() );
            return FALSE; ///< Received a L10, Send a L010 back!!
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Function LoginHandshake:
//  Handle the Login Handshake.
//  AppendNo: Rendom genarated. between 1~999.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemTAIFEX::LoginHandshake( int ErrorCode ) ///< Send L030, Default ErrorCode = 0
{
    FAppendNo = (rand()+1)%1000;
    FKeyValue = ((FAppendNo * FPassword.ToInt())/100)%100; ///< Need thousand & hundred digi.

    if( FIsTCP == FALSE )
    {
        MessageL030 l030( FAppendNo );

        l030.SetStatusCode( ErrorCode );
        if( SendMessage( &l030, TXIFEX_PULL_INTERVAL, FALSE ) == TRUE )///< Send Register notice message
        {
            if( FStopHandShake == TRUE )
                return FALSE;
            return TRUE;
        }
    }
    else
    {
        TMP::TMPMessage L030( TMP::tmtL30 );

        L030.SetField( "status_code", ErrorCode );
        L030.SetField( "append_no", FAppendNo );
        L030.SetField( "system_type", GetTMPSystemType());
        FListener->OnCommand( EVENT_TMP_GET_SEQ, (void*)&FTAIFEXOutSeq );
        FSequenceOut = FTAIFEXOutSeq;
        L030.SetField( "end_out_bound_num", FTAIFEXOutSeq );
        FSequenceIn = 0; ///< Broken, Reset inbound sequence.
        if( SendTMPMessage( &L030, TMP_TIMEOUT, FALSE ) == TRUE )///< Send Register notice message
        {
            if( FStopHandShake == TRUE )
                return FALSE;
            return TRUE;
        }
    }
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Link Sub-System: Login handshake failed! ");
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Function SubsystemStartupHandshake :
//  Subsystem Startup Handshake, receive a register message (L040)
//  and reply a subsystem startup message.(L050)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int LinkSubSystemTAIFEX::SubsystemStartupHandshake( ) ///< Receive L040 and Send L050
{
    int ErrCode;

    if( FIsTCP == FALSE )
    {
        MessageHeader* Msg;
        ///< Modify on 2004/09/14 by Simon. Ignore other message durning 10 sec.
        if( (Msg = ReceiveMessage( mtL040, 10 )) != NULL )
        {
            if( (ErrCode = Authentication( static_cast< MessageL040* >( Msg ))) != 0 )
            {
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Authentication fail! locked." );
                SetConnectionState( FALSE, FLockReason ); ///< Link SubSystem offline.
                return ErrCode;  ///< Authentication failed.
            }
            else
            {
                MessageL050    L050;
                if( SendMessage( &L050, TXIFEX_PULL_INTERVAL, FALSE ) == TRUE )
                    return 0; ///< Startup handshake succeed!
            }
        }
    }
    else
    {
        TMP::TMPMessage* Msg;

        if( (Msg = ReceiveTMPMessage( (UInt8)40, TMP_TIMEOUT )) != NULL )
        {
            if( (ErrCode = Authentication( Msg )) != 0 )
            {
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Authentication fail! locked." );
                SetConnectionState( FALSE, FLockReason ); ///< Link SubSystem offline.
                delete Msg;
                return ErrCode; ///< Authentication failed.
            }
            else
            {
                ///< recover missing messages.
                RecoverMissingMessages( Msg );
                delete Msg;
                ///< Reply L050 Startup handshake connfirm message.
                TMP::TMPMessage  L050( TMP::tmtL50 );

                L050[ "max_flow_ctrl_cnt" ] = FMaxThroughput;
                L050[ "HeartBtInt"        ] = FHeartBtInt;
                FSendTick = UFC::GetTickCountMS( );
                if( SendTMPMessage( &L050, TMP_TIMEOUT, FALSE ) == TRUE )
                    return 0; ///< Startup handshake succeed!
            }
        }
    }
    return -1;  ///< Time out, No L040 incoming during this 10 sec.
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Function ConfirmSubSysStartup :
//  Try to receive the Confirm subsystem online message.(L060)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemTAIFEX::ConfirmSubSysStartup( void )
{
    if( FIsTCP == FALSE )
    {
        MessageHeader* L060Msg = ReceiveMessage( LINKSUBSYSTEM_TIMEOUT, TRUE );///< (R:L060)
        ///< if L060Msg ==NULL, the ReceiveMessage function will throw exception.
        TMessageType Type = L060Msg->MessageType( );
        delete L060Msg;
        if( Type == mtL060 )///< Is a L060! Subsystem startup succeed.
        {
            SetConnectionState( TRUE, PVC_STATUS_ONLINE_READY ); ///< Link SubSystem online!!!(Only entry to Online the subsys.)
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex, " LinkSubSystem Online!");
        }
        else
            throw( X25RehandshakeException( "Not a L060 message"));
    }
    else
    {
        TMP::TMPMessage* L060 = ReceiveTMPMessage( TMP_TIMEOUT, TRUE );///< (R:L060)
        ///< if L060Msg ==NULL, the ReceiveMessage function will throw exception.
        UInt8 Type = L060->GetMessageType();
        delete L060;
        if( Type == 60 )///< Is a L060! Subsystem startup succeed.
        {
            Int32 Roundtrip = UFC::GetTickCountMS( ) - FSendTick;
            if( Roundtrip < 5 ) ///< RTT < 5 ms
            {
                FBandwidth = lbE1;
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," 2048k Leased line [RTT:%d ms]",Roundtrip);
            }
            else if( Roundtrip >= 5  && Roundtrip < 15 )
            {
                FBandwidth = lb512K;
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," 512K Leased line [RTT:%d ms]",Roundtrip);
            }
            else
            {
                FBandwidth = lb128K;
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," 128K Leased line [RTT:%d ms]",Roundtrip);
            }
            SetConnectionState( TRUE, PVC_STATUS_ONLINE_READY ); ///< Link SubSystem online!!!(Only entry to Online the subsys.)
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," LinkSubSystem Online!");
        }
        else
            throw( X25RehandshakeException( "Not a L060 message"));
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Function Authentication :
//  Check the AppenNo,Broker ID, APCode and the random generate KeyValue.( Between 1~999 )
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int LinkSubSystemTAIFEX::Authentication( MessageL040* L040 )
{
    if( L040->GetAppendNo().ToInt() != FAppendNo  )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Link Sub-System: L040 Append number error.");
        FLockReason = PVC_STATUS_PW_ERR;
        return 1; ///< Append No error.
    }
    if( L040->GetBrokerID() != FBrokerID )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Link Sub-System: L040 Broker ID error Broker:%s.",FBrokerID.c_str());
        FLockReason = PVC_STATUS_FCM_ERR;
        return 2; ///< Broker ID error.
    }
    if( L040->GetAPCode() != FAPCode )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Link Sub-System: L040 AP Code error.");
        FLockReason = PVC_STATUS_AP_ERR;
        return 3; ///< AP Code error.
    }    
    if( L040->GetKeyValue().ToInt() != FKeyValue )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Link Sub-System: L040 Key value error PW = %s.",FPassword.c_str());
        FLockReason = PVC_STATUS_PW_ERR;
        return 4; ///< Key value error.(Password error)
    }
    return 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Check Password, APCode, Session ID.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int LinkSubSystemTAIFEX::Authentication( TMP::TMPMessage* L040 )
{
    char FCMAPCode = L040->GetField("ap_code")->ToInteger() + '0';

    if( L040->GetField("append_no")->ToInteger() != FAppendNo  )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Link Sub-System: L040 Append number error.");
        FLockReason = PVC_STATUS_PW_ERR;
        return 1201; ///< Append No error.
    }
    if( L040->GetField("fcm_id")->ToInteger() != FBrokerIndex )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Link Sub-System: L040 Broker ID error Broker:%d.",FBrokerIndex);
        FLockReason = PVC_STATUS_FCM_ERR;
        return 1202; ///< Broker ID error.
    }
    if( (FAPCode == '4' || FAPCode == '6' ) && ( FCMAPCode !='4' && FCMAPCode != '6') )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Link Sub-System: L040 AP Code error.");
        FLockReason = PVC_STATUS_AP_ERR;
        return 1203; ///< AP Code error.
    }
    else
        FAPCode = FCMAPCode;
    if( (FAPCode == '8' || FAPCode == '3') && ( FCMAPCode != '8' && FCMAPCode != '3')   )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Link Sub-System: L040 AP Code error.");
        FLockReason = PVC_STATUS_AP_ERR;
        return 1203; ///< AP Code error.
    }
    else
        FAPCode = FCMAPCode;
    if( L040->GetField("key_value")->ToInteger() != FKeyValue )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Link Sub-System: L040 Key value error PW = %s.",FPassword.c_str());
        FLockReason = PVC_STATUS_PW_ERR;
        return 1204; ///< Key value error.(Password error)
    }
    if( L040->GetField("session_id")->ToInteger() !=  FPVCNumber )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Link Sub-System: L040 Session ID not equal." );
        FLockReason = PVC_STATUS_SID_ERR;
        return 1205; ///< system type error.
    }
    if( L040->GetField("system_type")->ToInteger() !=  GetTMPSystemType() )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Link Sub-System: L040 system type value error Type:%d.",L040->GetField("system_type")->ToInteger() );
        FLockReason = PVC_STATUS_SYS_ERR;
        return 1206; ///< system type error.
    }
    return 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemTAIFEX::SendL41( UFC::MemoryStream& Stream, UInt32 Totalsize, BOOL IsEOF )
{
    TMP::TMPMessage     L041( TMP::tmtL41 );
    TMP::TMPCharArray*  DataField = (TMP::TMPCharArray*)L041.GetField( "data" );

    if( IsEOF == TRUE )
        L041.SetField( "is_eof", 1 );
    else
        L041.SetField( "is_eof", 0 );
    L041.SetField( "file_size", Totalsize );
    DataField->SetBinaryData((UInt8*)Stream.GetBuffer(), Stream.GetSize() );
    SendTMPMessage( &L041, TMP_TIMEOUT, FALSE );

    TMP::TMPMessage* L42 = ReceiveTMPMessage( TMP_TIMEOUT, TRUE );///< (R:L42)
    if( L42 == NULL )
        throw( X25RehandshakeException( "Recv L42 message timeout"));
    if( L42->GetMessageType() == TMP::tmtL42 )
        delete L42; ///< Is a L42! Continue recover.
    else
        throw( X25RehandshakeException( "Send a L41 but not recv a L42 message"));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Handle recover missing messages.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemTAIFEX::RecoverMissingMessages( TMP::TMPMessage* L040 )
{
    UInt32             ReqStartSeq = L040->GetField( "request_start_num" )->ToInteger();


    if( ReqStartSeq < FTAIFEXOutSeq && (FAPCode == '6'|| FAPCode == '4') )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex, " ---------------------------------------------");
        UInt32 Start = ReqStartSeq;
        UInt32 L41Count = 0;
        UInt32 Count = 0;
        UInt32 TotalSize = 0;
        UInt32 End;
        UInt32 BeginTime = UFC::GetTickCountMS();
        BOOL   LastL41 = FALSE;
        BOOL   CompactExec = FALSE;
        if( this->FAPCode == '6' )
            CompactExec = TRUE;

        while( !LastL41 )
        {
            End = Start + TMP_MAX_MSG_PER_L41;
            if( End >= FTAIFEXOutSeq )
                LastL41 = TRUE;
            TMPRecoverMessages RecoverMessages( Start, End, CompactExec );
            UFC::MemoryStream  MStream( TMP_MAX_DATA_SIZE );

            ///< Ask missing messages
            FListener->OnCommand( EVENT_TMP_GET_MSG, (void*)&RecoverMessages );
            if( RecoverMessages.BinSize == 0 )
            {
                UFC::BufferedLog::DebugPrintf( UFC::dlFatalError,FPrefixIndex," *************************************************************" );
                UFC::BufferedLog::DebugPrintf( UFC::dlFatalError,FPrefixIndex,"  Fatal error! Read session execution report spilt file error" );
                UFC::BufferedLog::DebugPrintf( UFC::dlFatalError,FPrefixIndex,"               Skip L41,L42 recover procedure" );
                UFC::BufferedLog::DebugPrintf( UFC::dlFatalError,FPrefixIndex," *************************************************************" );
                break;
            }
            ///< Save messages to stream.
            Count += RecoverMessages.Messages.ItemCount();
            for( register Int32 i = 0; i < RecoverMessages.Messages.ItemCount(); i ++ )
                RecoverMessages.Messages.GetItem( i )->ToStream( &MStream );
            TotalSize += RecoverMessages.BinSize;
            ///< Send L41 to broker.
            SendL41( MStream, RecoverMessages.BinSize, LastL41); ///< EOF
            L41Count++;
            Start = End;
        }
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Recover from message[%d] to [%d] use[%d ms].", ReqStartSeq, FTAIFEXOutSeq, UFC::GetTickCountMS() - BeginTime );
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," Use %d L41 message.", L41Count);
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," %d messages recovered,Size:%d", Count, TotalSize );
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,FPrefixIndex," ---------------------------------------------");
    }

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
