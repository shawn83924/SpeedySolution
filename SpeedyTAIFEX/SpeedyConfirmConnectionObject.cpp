#include "SpeedyConfirmConnectionObject.h"
#include "SpeedyObjectsManager.h"
#include "../TAIFEXLib/LinkSubSystem.h"
#include "../TAIFEXLib/Connection.h"
#include "../Migo/Sigo.h"
//----------------------------------------------------------------------------------------------------------------------
extern MessageObject*		  MessageObj;
extern BOOL			  RecoverMode;
//----------------------------------------------------------------------------------------------------------------------
//
//  ### FCM side ###
//  Connection object for AP code 3,8,9 Confirm subsystem.
//
//----------------------------------------------------------------------------------------------------------------------
SpeedyConfirmConnectionObject::SpeedyConfirmConnectionObject( SpeedyObjectsManager* ObjManager,
                                                              ConnectionParameter& info,
                                                              ConnectionListener* CListener,
                                                              UFC::ThreadListener* TListener,
                                                              char APCode,
                                                              BOOL IsOpt)
:ConnectionObjectBase( info ),
 UFC::PThread( TListener, TRUE ),
 FObjectManager( ObjManager ),
 FNeedLock( FALSE ),
 FHandshakeReady( FALSE ),
 FEnabled( TRUE ),
 F030Count( 0 ),    
 F031Count( 0 ),
 F032Count( 0 ),
 FAPCode( APCode ),
 FBroken( FALSE ),
 FConnection( new LinkSubSystemFCM( FParameters ) ),
 FConnectionListener( CListener )
{
    UFC::AnsiString FillReportName,FillSeqName;

    FLineBrokerID = FParameters.GetBrokerID();
    FPVCID.Printf( "%02d", FParameters.GetPVC() );
    FmType = ((FLineBrokerID[1] -'0')*256 +(FLineBrokerID[2] -'0')*16 + (FLineBrokerID[3] -'0')) << 12;
    FillSeqName.Printf( "%s.%c.%d,%d", (IsOpt)?"Option":"Futures", APCode, FParameters.GetLink(), FParameters.GetPVC() );
    FSeqNumber = new UFC::PInt32( FillSeqName, 1 );///< Set sequence to 1 if share memory not exists.
    if( RecoverMode == FALSE )
       *FSeqNumber = 1;
    FConnection->SetLogManager( FObjectManager->GetFillLog() );
}
//----------------------------------------------------------------------------------------------------------------------
SpeedyConfirmConnectionObject::~SpeedyConfirmConnectionObject()
{
    delete FSeqNumber;
    if( IsTerminated() == FALSE )
    {
        Terminate(); ///< Terminate the thread.
        WaitFor();   ///< Wait for thread terminated.
    }
    delete FConnection;
}
//---------------------------------------------------------------------------------------------------------------------
//
//  Send out the fill report.
//  Modify this function to interface with other OMS
//  Exp: Add to queue to interface with EDS OMS
//
//---------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::SendFillMessage( char* Buffer, const char* TAIFEXTime, const char* X25Time, Int32 OrigNID, const char* Key, const char* CKey )
{
    if( MessageObj != NULL )
    {
        MTree   Data;

        Data.append( "FILL_ORDER", Buffer );
        //Data.append( "MTYPE", FmType );
        Data.append( "PVC",   FPVCID );
        Data.append( "PBNO",  FLineBrokerID );        
        Data.append( "TTIME", TAIFEXTime );
        Data.append( "XTIME", X25Time );
        Data.append( "NID",   OrigNID );
        if( Key != NULL && strlen( Key ) > 0 )
            Data.append( "KEY", Key );
        if( CKey != NULL && strlen( CKey ) > 0 )
            Data.append( "CKEY", CKey );
        Data.append( "PHOST", UFC::Hostname );
        MessageObj->Send( FObjectManager->GetFillSubject(), UFC::Hostname, Data );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::SetSystemTime( Int32 hhmmss )
{	
    MTree   Data;     

    Data.append( "Now", hhmmss );
    MessageObj->Send( "SPEEDY.TIME", UFC::Hostname, Data );       
    UFC::BufferedLog::Printf( " Set System Time to %02d:%02d:%02d", hhmmss/10000, (hhmmss%10000)/100, hhmmss%100);        
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::FCMHandshake( void )
{
    MessageHeader*  InMsg;
    Int32           RequestSeq = (Int32)*FSeqNumber;
    if( RequestSeq == 1 )
    	  RequestSeq = 0;
    	  
    MessageC010     C010( (char*)FParameters.GetBrokerID().c_str(),(Int32)*FSeqNumber );

    if( FConnection->SendMessage( &C010, ORDERSUBSYSTEM_TIMEOUT ))
    {
        if( (InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT )) != NULL )
        {
            if( InMsg->IsMessage( mtC020 ))
            {
                SetSystemTime( UFC::AnsiString( InMsg->GetTimeStr(), 6 ).ToInt() );
                delete InMsg;
                FHandshakeReady = TRUE;
                FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_READY,(void*)FSeqNumber);
                return;
            }
            delete InMsg;
        }
    }
    FHandshakeReady = FALSE;
    FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::CMHandshake( void )
{
    MessageHeader*  InMsg;
    MessageC011     C011( (char*)FParameters.GetBrokerID().c_str(),(Int32)*FSeqNumber );

    if( FConnection->SendMessage( &C011, ORDERSUBSYSTEM_TIMEOUT ))
    {
        if( (InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT )) != NULL )
        {
            if( InMsg->IsMessage( mtC021 ))
            {
                SetSystemTime( UFC::AnsiString( InMsg->GetTimeStr(), 6 ).ToInt() );
                delete InMsg;
                FHandshakeReady = TRUE;
                FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_READY,(void*)FSeqNumber);
                return;
            }
            delete InMsg;
        }
    }
    FHandshakeReady = FALSE;
    FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::ConfirmsubSystemHandshake( BOOL ReceivedL010 )
{
    if( FConnection->OnlineHandshake( ReceivedL010 ))
    {
        switch( FAPCode )
        {
            case '3': FCMHandshake(); break;
            case '8':
            case '9': CMHandshake();  break;
        }
    }
    else
        throw X25Exception( -9,"Handshake exception.");

}
//----------------------------------------------------------------------------------------------------------------------
char SpeedyConfirmConnectionObject::FetchOrderInfo( C03XBody* C030, Int32& OrigNID, UFC::AnsiString& Key, UFC::AnsiString& OID, UFC::AnsiString& IB, UFC::AnsiString& CKey )
{
    char Type = C030->GetType();
    if( Type == '1')
    {
        C030SingleBody* C030Body =  (C030SingleBody*)C030;
        OID = UFC::AnsiString( C030Body->FOrderNo, 5 );
        IB  = UFC::AnsiString( C030Body->FBroker + 4,3  );
        FObjectManager->OrderInfo.GetOrderInfo( OID, IB, Key, OrigNID );
        CKey = "";
    }
    else if( Type == '2')
    {
        C030ComboBody* C030Body =  (C030ComboBody*)C030;
        OID = UFC::AnsiString( C030Body->FOrderNo, 5 );
        IB  = UFC::AnsiString( C030Body->FBroker + 4,3  );
        FObjectManager->OrderInfo.GetOrderInfo( OID, IB, Key, OrigNID );
        CKey = "";
    }
    else if( Type == '3')
    {
        C030ReduceCancelBody* C030Body =  (C030ReduceCancelBody*)C030;
        OID = UFC::AnsiString( C030Body->FOrderNo, 5 );
        IB  = UFC::AnsiString( C030Body->FBroker + 4,3  );
        if( C030Body->FTransCode == '2' ) ///< Reduce
            FObjectManager->OrderInfo.PopReplaceNID( OID, IB, OrigNID, Key, CKey  );
        else if( C030Body->FTransCode == '3' ) ///< Cancel
            FObjectManager->OrderInfo.PopCancelNID( OID, IB, OrigNID, Key, CKey  );
    }
    else if( Type == '4')
    {
        C030QuoteReduceCancelBody* C030Body =  (C030QuoteReduceCancelBody*)C030;
        OID = UFC::AnsiString( C030Body->FOrderNo, 5 );
        IB  = UFC::AnsiString( C030Body->FBroker + 4,3  );
        if( C030Body->FTransCode == '2' ) ///< Reduce
            FObjectManager->OrderInfo.PopReplaceNID( OID, IB, OrigNID, Key, CKey  );
        else if( C030Body->FTransCode == '3' ) ///< Cancel
        {
            FObjectManager->OrderInfo.PopCancelNID( OID, IB, OrigNID, Key, CKey  );
            if( OrigNID == -1 ) ///< TAIFEX auto delete the Quote
                FObjectManager->OrderInfo.GetOrderInfo( OID, IB, Key, OrigNID );
        }
    }
    return Type;
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::ReceiveC030( MessageC030* C030 )
{
    C03XBody *Msg;
    Int32     c = C030->GetMessageCount();
    Int32     Seq,OrigNID;
    UFC::AnsiString RecvTime,TFXTime,Key,ExecKey,OID,CKey,IB;

    GetTimeString( RecvTime, FALSE );
    F030Count += c;
    for( int i = 0;i< c; i++ )
    {
       if( (Msg = C030->GetMessage( i ))!=NULL)
       {
           Seq = Msg->GetSeq(); ///< Get the TAIFEX execution sequence.
           int Diff = Seq - (Int32)*FSeqNumber;
               
           if( Diff > 1 ) ///< Skip some execution!
           {
               *FSeqNumber = Seq;
               UFC::BufferedLog::Printf( " Miss some Execution, expect seq:%d but receive seq:%d.", ((Int32)*FSeqNumber)+1, Seq );               
               throw( X25ReconnectException( "Miss some Execution." ) );
           }
           else if( Diff == 1 || Diff == 0 )           
               *FSeqNumber = Seq; ///< OK!
           else
               UFC::BufferedLog::Printf( " Recover executions, expect seq:%d but receive seq:%d.", ((Int32)*FSeqNumber)+1, Seq );                               
           ExecKey.Printf( "%s%08d",FLineBrokerID.c_str(), Seq );
           if( FObjectManager->OrderInfo.IsExecExist( ExecKey ) == FALSE ) ///< Execution not sent
           {
               Msg->AsString( FMsgBuffer );
               TFXTime.Printf( "%09d", Msg->ProcessingTime()*10 );
               FetchOrderInfo( Msg, OrigNID, Key, OID, IB, CKey );
               SendFillMessage( FMsgBuffer, TFXTime.c_str(),RecvTime.c_str(), OrigNID, Key, CKey );
               FObjectManager->OrderInfo.WriteExecutionLog( FALSE, OrigNID, Key, FMsgBuffer, FLineBrokerID, UFC::Hostname, FPVCID, -1 ); ///< Write log
               FObjectManager->FillIOC( this, Msg, OID, IB, FLineBrokerID );
           }
           else
               UFC::BufferedLog::Printf( " Execution:%s already exist, skip it.", ExecKey.c_str() );
       }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::ReceiveC031( MessageC031* C031 )
{
    C03XBody *Msg;
    int       c = C031->GetMessageCount();
    int       Seq;
    UFC::AnsiString RecvTime,TFXTime,ExecKey;

    GetTimeString( RecvTime, FALSE );
    F031Count += c;
    for( int i = 0; i < c; i++ )
    {
        if( (Msg = C031->GetMessage( i ))!=NULL)
        {
            Seq = Msg->GetSeq();
            int Diff = Seq - (Int32)*FSeqNumber;
            
            if( Diff > 1 )
            {
                *FSeqNumber = Seq;
                UFC::BufferedLog::Printf( " Miss some Execution, expect seq:%d but receive seq:%d.", ((Int32)*FSeqNumber)+1, Seq );               
                throw( X25ReconnectException( "Miss some Execution." ) );
            }
            else if( Diff == 1 || Diff == 0  )            
                *FSeqNumber = Seq;
            else
                UFC::BufferedLog::Printf( " Recover executions, expect seq:%d but receive seq:%d.", ((Int32)*FSeqNumber)+1, Seq );                               
            
            ExecKey.Printf("%s%08d",FLineBrokerID.c_str(),Seq);
            if( FObjectManager->OrderInfo.IsExecExist( ExecKey ) == FALSE ) ///< Execution not sent
            {
                Msg->AsString( FMsgBuffer );
                TFXTime.Printf( "%09d", Msg->ProcessingTime()*10 );
                SendFillMessage( FMsgBuffer,TFXTime.c_str(),RecvTime.c_str() );
            }
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::ReceiveC032( MessageC032* C032 )
{
    C03XBody *Msg;
    int       c = C032->GetMessageCount();
    int       Seq;
    UFC::AnsiString RecvTime,TFXTime,ExecKey;

    GetTimeString( RecvTime, FALSE );
    F032Count += c;
    for( int i = 0; i < c; i++ )
    {
       if( (Msg = C032->GetMessage( i )) !=NULL )
       {
           Seq = Msg->GetSeq();
           int Diff = Seq - (Int32)*FSeqNumber;
               
           if( Diff > 1 ) ///< Skip some execution!
           {
               *FSeqNumber = Seq;
               UFC::BufferedLog::Printf( " Miss some Execution, expect seq:%d but receive seq:%d.", ((Int32)*FSeqNumber)+1, Seq );               
               throw( X25ReconnectException( "Miss some Execution." ) );
           }
           else if( Diff == 1 || Diff == 0  )
               *FSeqNumber = Seq; ///< OK!
           else
               UFC::BufferedLog::Printf( " Recover executions, expect seq:%d but receive seq:%d.", ((Int32)*FSeqNumber)+1, Seq );               
           ExecKey.Printf("%s%08d",FLineBrokerID.c_str(),Seq);
           if( FObjectManager->OrderInfo.IsExecExist( ExecKey ) == FALSE ) ///< Execution not sent
           {
               Msg->AsString( FMsgBuffer );
               TFXTime.Printf( "%09d", Msg->ProcessingTime()*10 );
               SendFillMessage( FMsgBuffer,TFXTime.c_str(),RecvTime.c_str());
           }
       }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::OnCommand( int Command, void* Data )
{
    switch( Command )
    {
        case COMMAND_GET_STATE: *((int*)Data ) = FConnection->GetStatus();
                                break;
        case EVENT_PVC_READ:
        case EVENT_PVC_WRITE:   FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_READ_WRITE, Data );
                                break;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::Listen( BOOL IsListen )
{
    if( FConnection != NULL )
    {
        if( IsListen == TRUE )
            FConnection->SetListener( this );
        else
            FConnection->SetListener( NULL );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::Enable( BOOL IsEnabled )
{
    if( FEnabled != IsEnabled )
    {
        if( IsEnabled == FALSE )
        {
            FConnection->SetStatus( PVC_STATUS_DISABLEING );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_UPDATE_STATE, NULL );
            FConnection->StopHandshake();
            FEnabled = FALSE;
        }
        else
        {
            FConnection->SetStatus( PVC_STATUS_ENABLE );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_ENABLED, NULL );
            FEnabled = TRUE;
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::MessageLoop( void )
{
    while( TRUE )
    {
        try
        {
            do
            {
                if( FEnabled == FALSE ) ///< Disable this connection.
                {
                    FHandshakeReady = FALSE; ///< Need to re-handshake
                    sleep( 1 );
                    FConnection->SetStatus( PVC_STATUS_DISABLE);
                    FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_DISABLED, NULL );
                }
                else if( IsTerminated() == TRUE ) // Close this connection.
                {
                    return;
                }
                else                         // Normal state...Process the message.
                {
                    MessageHeader* InMsg;
                    MessageC050    ConfirmConnectionMessage;
                    MessageL080    ConfirmCloseMarket;

                    if( FHandshakeReady == FALSE )
                        throw( X25ReconnectException("Confirm subsystem: SendMessage failed." ) );
                    if( FBroken == TRUE )
                    {
                        FBroken = FALSE;
                        throw( X25ReconnectException("Asked to rehandshake." ) );
                    }
                    if( (InMsg = FConnection->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT )) != NULL )
                    {
                         int MsgType = InMsg->MessageType();

                          switch( MsgType )
                          {
                              case mtL070: ConfirmCloseMarket.UpdateTime();
                                           FConnection->SendMessage( &ConfirmCloseMarket, ORDERSUBSYSTEM_TIMEOUT );
                                           delete InMsg;
                                           return;
                              case mtC040: ConfirmConnectionMessage.UpdateTime();
                                           FConnection->SendMessage( &ConfirmConnectionMessage, ORDERSUBSYSTEM_TIMEOUT );
                                           delete InMsg;
                                           break;          // Idel handshake message.
                              case mtC030: ReceiveC030( static_cast< MessageC030* >(InMsg) );// Order confirm message
                                           delete InMsg;
                                           break;
                              case mtC031: ReceiveC031( static_cast< MessageC031* >(InMsg) );
                                           delete InMsg;
                                           break;
                              case mtC032: ReceiveC032( static_cast< MessageC032* >(InMsg) );
                                           delete InMsg;
                                           break;
                              case mtC060: delete InMsg;
                                           return;         // End confirm message
                              default:     delete InMsg;
                          }
                    }
                }
            } while( TRUE );
        }
        catch( X25RehandshakeException& e ) // Receive a L010...go back to linksubsystem handshake.
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
            UFC::BufferedLog::Printf( " PVC %d:%s", FParameters.GetPVC(), e.what() );
            ConfirmsubSystemHandshake( TRUE );
        }
        catch( X25ReconnectException& e )  // Timeout or reconnect
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
            UFC::BufferedLog::Printf( " PVC %d:%s", FParameters.GetPVC(), e.what() );
            ConfirmsubSystemHandshake( FALSE );
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::DisableLoop( void )
{
    if( FEnabled == FALSE )
    {
        BOOL FirstTime = TRUE;
        while( FEnabled == FALSE ) ///< Disable this connection.
        {
            sleep( 1 );
            if( FirstTime == TRUE )
            {
                FConnection->SetStatus( PVC_STATUS_DISABLE);
                FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_DISABLED, NULL );
                FirstTime = FALSE;
            }
        }
        FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN, NULL );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::Execute( void )
{
    for( int i = 0; i< PVC_BUSY_RETRY_TIMES; i++ )
    {
        try
        {
            DisableLoop();
            FConnection->Open();
            ConfirmsubSystemHandshake( FALSE );
            MessageLoop(); ///< Return when market closed.
            FConnection->Close();
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_NORMAL_CLOSE );// Close the connection normaly.
            UFC::BufferedLog::Printf(" PVC:%d Thread stopped. Market closed.", FParameters.GetPVC() );
            return;
        }
        catch( X25Exception& e )
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
            FConnection->Close();
            UFC::BufferedLog::Printf(" %s. Try to reconnect after 5 sec...",e.what() );
            sleep( 5 );
        }
        catch( std::exception& )
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR );// Unknown exception.
            return;
        }
    }
    UFC::BufferedLog::Printf(" PVC:%d Thread stopped. Retry too many times.", FParameters.GetPVC() );
}
//----------------------------------------------------------------------------------------------------------------------
BOOL SpeedyConfirmConnectionObject::IsReady( void ) 
{ 
    if( FConnection != NULL )
        return FConnection->IsReady();
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
