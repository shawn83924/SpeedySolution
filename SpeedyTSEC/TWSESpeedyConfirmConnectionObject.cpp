#include "TWSESpeedyConfirmConnectionObject.h"
#include "TWSESpeedyObjectsManager.h"
#include "Profiler.h"
#include "../TSECLib/LinkSubSystem.h"
#include "../TSECLib/Connection.h"
#include "../Migo/Sigo.h"
#include "../SpeedyGateway/Administrator.h"
#include "../TSECLib/TWSERMClass.h"                   
#ifdef __RMLib
#include "../TSECLib/USunRiskManager.h"
#endif
//----------------------------------------------------------------------------------------------------------------------
extern MessageObject*         MessageObj;
extern BOOL                   RecoverModeExchange;
extern BOOL                   ExecToQueue;
extern BOOL                   SyncFillPVC;
extern BOOL                   NotSpeedyToOthers;
extern BOOL                   ExtremeVersion;
//------------------------------------------------------------------------------------------------------------------
extern TWSE::ExecutionListener* gTSEFilled;
extern TWSE::ExecutionListener* gOTCFilled;
//----------------------------------------------------------------------------------------------------------------------
int MaxSetTime = 5;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
//
//  ### FCM side ###
//  Connection object for AP code 3,8,9 Confirm subsystem.
//
//----------------------------------------------------------------------------------------------------------------------
SpeedyConfirmConnectionObject::SpeedyConfirmConnectionObject( SpeedyObjectsManager*  ObjManager,
                                                              ConnectionParameter& info,
                                                              ConnectionListener* CListener,
                                                              UFC::ThreadListener* TListener,
                                                              char APCode,
                                                              BOOL IsTWSE)
:ConnectionObjectBase( info ),
 UFC::PThread( TListener, FALSE ), 
 FNeedLock( FALSE ),
 FHandshakeReady( FALSE ),
 FEnabled( info.IsActive() ),
 F030Count( 0 ),
 FAPCode( APCode ),
 FBroken( FALSE ),
 FConnection( NULL ),
 FConnectionListener( CListener ),
 FObjectManager( ObjManager ),
 FIsTWSE( IsTWSE )
{
    UFC::AnsiString FillSeqName;

    FLineBrokerID = FParams.GetBrokerID();    
    if( IsTWSE == TRUE )   
        FillSeqName.Printf( "TSE.FILLRPT.%c.%d,%s,%s", APCode,FParams.GetLink(),FParams.GetPVCID().c_str(),FParams.GetBrokerID().c_str() );
    else
        FillSeqName.Printf( "OTC.FILLRPT.%c.%d,%s,%s", APCode,FParams.GetLink(),FParams.GetPVCID().c_str(),FParams.GetBrokerID().c_str() );
    FSeqNumber = new UFC::PInt32( FillSeqName, 1 );
    if( RecoverModeExchange == FALSE )
    {
       UFC::BufferedLog::Printf( " Port:%d PVCID:%s Reset the execution sequence number to 1.",FParams.GetLink(),FParams.GetPVCID().c_str());
       *FSeqNumber = 1;
    }
    else
        UFC::BufferedLog::Printf( " Port:%d PVCID:%s Execution sequence number:%d.",FParams.GetLink(),FParams.GetPVCID().c_str(),(Int32)*FSeqNumber);
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
}
//----------------------------------------------------------------------------------------------------------------------
// Fetch original order NID, key form OrderID + BrokerID. 
//----------------------------------------------------------------------------------------------------------------------
BOOL SpeedyConfirmConnectionObject::FetchOrderInfo( R3Body* R3, Int32& OrigNID, UFC::AnsiString& Key, UFC::AnsiString& OID )
{  
    UFC::AnsiString BrokerID( R3->FBroker, 4 );
    ///< Use brokerID , Order ID as order unique key.
    OID = UFC::AnsiString( R3->FOrderNo, 5 );    
    if( FObjectManager->OrderInfo.GetOrderInfo( OID, BrokerID, Key, OrigNID ) == FALSE )
    {   ///< Order not found.
        UFC::BufferedLog::Printf( " Can not get NID and Key for BrokerID[%s] OrderID[%s]", BrokerID.c_str(), OID.c_str());
        Key = "null";
        OrigNID = 0;
        return FALSE;
    }
    return TRUE;
}
//---------------------------------------------------------------------------------------------------------------------
//
//  Send out the fill report.
//  Modify this function to interface with other OMS
//  Exp: Add to queue to interface with EDS OMS
//
//---------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::SendFillMessage( char* Buffer, const char* TSECTime, const char* LineTime, Int32 OrigNID, const char* Key, BOOL IsSpeedy )
{    
    if( MessageObj != NULL )
    {
        MTree   Data;

        Data.append( "FILL_ORDER", Buffer );
        Data.append( "PVC", FParams.GetPVCID() );
        Data.append( "PBNO",  FLineBrokerID );
        Data.append( "NID",   OrigNID );
        Data.append( "TTIME", TSECTime );
        Data.append( "XTIME", LineTime );
        Data.append( "PHOST", UFC::Hostname );         
        if( Key != NULL && strlen( Key ) > 0 )
            Data.append( "KEY", Key );        
        ///< Speedy Pro. Send to queue.
        if( ExecToQueue == TRUE && ExtremeVersion == FALSE )
            FObjectManager->SendExecutionToQueue( 2, &Data );                    
        if( IsSpeedy == TRUE ) ///< Speedy Order, Send to SpeedyGateway
        {
            if( ExtremeVersion == TRUE )
            {
               if( FIsTWSE == TRUE )    
                   gTSEFilled->OnExecution( Buffer, OrigNID, Key, "", &Data  );
               else
                   gOTCFilled->OnExecution( Buffer, OrigNID, Key, "", &Data  );
            }
            else
                MessageObj->Send( FObjectManager->GetFillSubject(), UFC::Hostname, Data );                 
        }
        else if( NotSpeedyToOthers == TRUE ) ///< Not Speedy Order, send to User [Others]                     
        {
            Data.append( "KEY", "Others" ); 
            if( ExtremeVersion == TRUE )
            {
                if( FIsTWSE == TRUE )    
                    gTSEFilled->OnExecution( Buffer, OrigNID, Key, "", &Data  );
                else
                    gOTCFilled->OnExecution( Buffer, OrigNID, Key, "", &Data  );
            }
            else
                MessageObj->Send( FObjectManager->GetFillSubject(), UFC::Hostname, Data );    
        }
        else ///< Not Speedy Order                     
        {
            if( SyncFillPVC == TRUE ) ///< Send to order Speedy as a fill PVC.
                MessageObj->Send( FObjectManager->GetSimPVCSubject(), UFC::Hostname, Data );
            else ///< Not Speedy Order. Send to MBus.
                MessageObj->Send( FObjectManager->GetNotSpeedyFillSubject(), UFC::Hostname, Data );
        }
    }    
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::SetSystemTime( Int32 hhmmss )
{
    MTree   Data;     

    Data.append( "Now", hhmmss );
    MessageObj->Send( "SPEEDY.TIME", UFC::Hostname, Data );       
    UFC::BufferedLog::Printf( " Ask SpeedyTop to set time[%02d:%02d:%02d]", hhmmss/10000, (hhmmss%10000)/100, hhmmss%100);
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::FCMHandshake( void )
{
    MessageHeader*  InMsg;
    MessageR1       R1( FIsTWSE, (char*)FParams.GetBrokerID().c_str(),(Int32)*FSeqNumber );

    if( FConnection->SendMessage( &R1, TWSE_ORDERSUBSYSTEM_TIMEOUT ))
    {
        if( (InMsg = FConnection->ReceiveMessage( TWSE_ORDERSUBSYSTEM_TIMEOUT )) != NULL )
        {
            if( InMsg->IsMessage( mtR2 ))
            {
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
        FCMHandshake();
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::ReceiveR3( MessageR3* R3 )
{
    UFC::AnsiString logHead = "SpeedyConfirmConnectionObject::ReceiveR3()";
    R3Body          *Msg;
    char            Buffer[ 1024 ];
    int             c = R3->GetMessageCount();
    int             RecNo, Seq, Diff, OrigNID;//, mhTime;    
    UFC::AnsiString RecvTime, TFXTime, Key, ExecKey, OID;    

    UFC::GetTimeString( RecvTime, FALSE );
    F030Count += c;
    for( int i = 0; i < c; i++ )
    {
        if( ( Msg = R3->GetMessage( i ) ) != NULL )
        {
            UFC::AnsiString StockNo(Msg->FStockID, 6);
            StockNo.TrimRight(' ');
            UFC::AnsiString OrderNo(Msg->FOrderNo, 5);
            RecNo  = Msg->GetRecNo();
            Seq    = Msg->GetSeq();
            //mhTime = UFC::AnsiString( Msg->FMatchTime, 8 ).ToInt();
            Diff   = Seq - (Int32)*FSeqNumber;
            char side = Msg->FBuySellCode;
            if( Diff > 1 ) ///< Skip some execution!
            {
                *FSeqNumber = Seq;
                UFC::BufferedLog::Printf( " %s Miss some Execution, expect seq:%d but receive seq:%d.", logHead.c_str(), ((Int32)*FSeqNumber)+1, Seq );               
                throw( X25ReconnectException( "Miss some Execution." ) );
            }
            else if( Diff == 1 || Diff == 0 )           
                *FSeqNumber = Seq; ///< OK!
            else
                UFC::BufferedLog::Printf( " %s Recover executions, expect seq:%d but receive seq:%d.", logHead.c_str(), ((Int32)*FSeqNumber)+1, Seq );
            
//            ///< Use line broker ID + report sequence + match time as unique key.
//            ExecKey.Printf( "%s_%08d_%08d%c", FLineBrokerID.c_str(), mhTime, Seq, side );
            ///< Use line report StockNo + side + RecNo as unique key.
            if (side == 'B')
                ExecKey.Printf( "%s_%s_1%08d", OrderNo.c_str(), StockNo.c_str(), RecNo );
            else if (side == 'S')
                ExecKey.Printf( "%s_%s_2%08d", OrderNo.c_str(), StockNo.c_str(), RecNo );
            else
                ExecKey.Printf( "%s_%s_%c%08d", OrderNo.c_str(), StockNo.c_str(), side, RecNo );
            UFC::BufferedLog::Printf( " %s Execution Key:%s.", logHead.c_str(), ExecKey.c_str() );
                
            if( FObjectManager->OrderInfo.IsExecExist( ExecKey ) == FALSE ) ///< Execution not sent
            {
                Msg->AsString( Buffer );
                TFXTime.Printf( "%09d", Msg->ProcessingTime()*10 );
                if( FetchOrderInfo( /*In*/Msg, /*Out*/OrigNID, /*Out*/Key, /*Out*/OID ) == TRUE )
                {
//RM
                    if( FObjectManager->IsRiskManagerEnabled() && ( FObjectManager->FRMObjPtr != 0 ) )
                    {
                        if( !FObjectManager->FRMObjPtr->IsFillRptExecExist( ExecKey, UFC::rwLockForRead ) )
                        {
                            FObjectManager->FRMObjPtr->AddFillRptExec(ExecKey, UFC::rwLockForWrite);
                            FObjectManager->FRMObjPtr->AddRiskMatchQty(Msg, ExecKey); 
                        }
                        else
                            twserm::CTWSERiskManagerClass::GetLogObject()->fprintf(" %s [%s][%s] has Processed. skip it.", logHead.c_str(), OID.c_str(), ExecKey.c_str());
                    }                    
#ifdef __RMLib
                    if ( rm::IBasicRiskManagerObject::IsRMControlSetting() ) 
                    {
                        if (!rm::rmObjPtr->IsFillRptExecExist(ExecKey, UFC::rwLockForRead))
                        {
                            rm::rmObjPtr->AddFillRptExec(ExecKey, UFC::rwLockForWrite);
                            rm::rmObjPtr->AddRiskMatchQty(Msg, FParams.GetPVCID(), ExecKey);
                        }
                        else
                            rm::WriteRiskLog(logHead, "[%s][%s] has Processed. skip it.", OID.c_str(), ExecKey.c_str());
                    }
#endif
//RM                   
                    SendFillMessage( Buffer, TFXTime.c_str(),RecvTime.c_str(), OrigNID, Key, TRUE  );
                    FObjectManager->OrderInfo.WriteExecutionLog( FALSE, OrigNID, Key, Buffer, FLineBrokerID, UFC::Hostname, FParams.GetPVCID() ); ///< Write log                              
                }
                else ///< Original order not send by Speedy
                {
                    if( NotSpeedyToOthers == TRUE ) ///< Sent as AE "Others".                   
                    {                       
                        OrigNID = 1;
                        Key     = "Others";
                        OID     = UFC::AnsiString( Msg->FOrderNo, 5 );
                        SendFillMessage( Buffer, TFXTime.c_str(),RecvTime.c_str(), OrigNID, Key, FALSE  );
                        FObjectManager->OrderInfo.WriteExecutionLog( FALSE, OrigNID, Key, Buffer, FLineBrokerID, UFC::Hostname, FParams.GetPVCID() ); ///< Write log                                  
                    }
                    else
                        SendFillMessage( Buffer, TFXTime.c_str(),RecvTime.c_str(), OrigNID, Key, FALSE  );
                }
            }
            else
                UFC::BufferedLog::Printf( " %s Execution:%s already exist, skip it.", logHead.c_str(), ExecKey.c_str() );
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
        	FConnection->StopHandshake();
            FConnection->SetStatus( PVC_STATUS_DISABLEING );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_UPDATE_STATE, NULL );
        }
        else
        {
            FConnection->SetStatus( PVC_STATUS_ENABLE );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_ENABLED, NULL );
        }
        FEnabled = IsEnabled;
    }
}
//----------------------------------------------------------------------------------------------------------------------
BOOL SpeedyConfirmConnectionObject::IsEnable( void )
{
    return FEnabled;
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyConfirmConnectionObject::CheckTime( char* ExchangeTime )
{
    UFC::AnsiString  ExTime( ExchangeTime, 6 );
    Int32            Now  = UFC::GetHHMMSS();
    Int32            Diff = Now - ExTime.ToInt(); 
            
    UFC::BufferedLog::Printf( " Receive heartbeat,Exchange time[%s] local time[%06d]", ExTime.c_str(), Now );
    if( MaxSetTime > 0 && Diff != 0  )
    {        
        MaxSetTime--;
        SetSystemTime( ExTime.ToInt() );
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
                    return;
                else                         // Normal state...Process the message.
                {
                    MessageHeader* InMsg;
                    MessageR5      ConfirmConnectionMessage( FIsTWSE );
                    MessageL080    ConfirmCloseMarket( FIsTWSE );

                    if( FHandshakeReady == FALSE )
                        throw( X25ReconnectException("Confirm subsystem: SendMessage failed." ) );
                    if( FBroken == TRUE )
                    {
                        FBroken = FALSE;
                        throw( X25ReconnectException("Asked to re-handshake." ) );
                    }
                    if( (InMsg = FConnection->ReceiveMessage( TWSE_ORDERSUBSYSTEM_TIMEOUT )) != NULL )
                    {
                         int MsgType = InMsg->MessageType();

                          switch( MsgType )
                          {
                              case mtL070: ConfirmCloseMarket.UpdateTime();
                                           FConnection->SendMessage( &ConfirmCloseMarket, TWSE_ORDERSUBSYSTEM_TIMEOUT );
                                           delete InMsg;
                                           return;
                              case mtR4:   CheckTime( InMsg->GetTimeStr() );
                                           ConfirmConnectionMessage.UpdateTime();                                             
                                           FConnection->SendMessage( &ConfirmConnectionMessage, TWSE_ORDERSUBSYSTEM_TIMEOUT );
                                           delete InMsg;
                                           break;          // Idel handshake message.
                              case mtR3:   ReceiveR3( static_cast< MessageR3* >(InMsg) );// Order confirm message
                                           delete InMsg;
                                           break;
                              case mtR6:   delete InMsg;
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
            ConfirmsubSystemHandshake( TRUE );
        }
        catch( X25ReconnectException& e )  // Timeout or reconnect
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
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
      FConnection = new LinkSubSystemFCM( FIsTWSE, FParams );
      FConnection->SetLogManager( FObjectManager->FillLog );
      for( int i = 0; i< TWSE_PVC_BUSY_RETRY_TIMES; i++ )
      {
          try
          {
              DisableLoop();
              FConnection->Open();
              ConfirmsubSystemHandshake( FALSE );
              MessageLoop(); ///< Return when market closed.
              FConnection->Close();
              FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_NORMAL_CLOSE );// Close the connection normaly.
              break;
          }
          catch( X25Exception& e )
          {
              FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
              UFC::BufferedLog::Printf( " TMP Exception:[%s]",e.what() );
              UFC::BufferedLog::Printf( " Try to reconnect after 5 sec." );
              FConnection->Close();              
              sleep( 5 );
          }
          catch( X25RehandshakeException& e )
          {
              FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
              UFC::BufferedLog::Printf( " Exception:[%s]",e.what() );
              UFC::BufferedLog::Printf( " Try to reconnect after 5 sec." );
              FConnection->Close();              
              sleep( 5 );
          }
          catch( UFC::SocketException& e )
          {
              UFC::BufferedLog::Printf( " Socket Exception:[%s]",e.what() );
              UFC::BufferedLog::Printf( " Try to reconnect after 5 sec." );
              FConnection->Close();
              sleep( 5 );
          }
          catch( std::exception &e )
          {
              FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR );// Unknown exception.
              UFC::BufferedLog::Printf( " Unknown Exception:[%s]",e.what() );
              UFC::BufferedLog::Printf( " Try to reconnect after 10 sec." );
              FConnection->Close();
              sleep( 10 );
          }
          catch( ... )
          {
              FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR );// Unknown exception.
              UFC::BufferedLog::Printf( " PVC:%s Unknown connection error.",FParams.GetPVCID().c_str() );
              return;
          }
          UFC::BufferedLog::Printf( " PVC:%s Reconnect %d times.", FParams.GetPVCID().c_str(), i );
      }
      delete FConnection;
      return;
}
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
