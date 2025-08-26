#include "SpeedyOrderConnectionObject.h"
#include "SpeedyObjectsManager.h"
#include "SpeedyOrderClient.h"
#include "../TAIFEXLib/LinkSubSystem.h"
#include "../TAIFEXLib/Connection.h"
#include "../Migo/Sigo.h"
//---------------------------------------------------------------------------------------------------------------------
extern MessageObject*         MessageObj;
extern BOOL                   RecoverMode;
extern Int32                  ExpiredMS;
extern Int32                  IOCExpired;   
//----------------------------------------------------------------------------------------------------------------------
//
//  ### FCM side ###
//  Connection object for AP code 4 Order subsystem.
//
//----------------------------------------------------------------------------------------------------------------------
SpeedyOrderConnectionObject::SpeedyOrderConnectionObject( SpeedyObjectsManager* ObjManager,
                                                          ConnectionParameter&  info,
                                                          ConnectionListener*   CListener,
                                                          UFC::ThreadListener*  TListener,
                                                          BOOL                  IsOpt )
:ConnectionObjectBase( info ),
 UFC::PThread( TListener, TRUE ),
 FObjectManager( ObjManager ),
 FOrderFCM( NULL ),
 FConnectionListener( CListener ),
 FReconnect( FALSE ),
 FHandshake( FALSE ),
 FBrokenRequest( TRUE ),
 FEnabled( info.IsActive() ),
 FOrder( 0, 1024 ),
 FParams( info )
{
    UFC::AnsiString UniqueStr;
    if( IsOpt )
    {
        UniqueStr.Printf( "%s Option Link %d PVC %d",UFC::Hostname,info.GetLink(),info.GetPVC());
        if( FQueue.Open( OPT_X25_QUEUE ) == FALSE )
        {
            UFC::BufferedLog::Printf( " Open queue:%x failed", OPT_X25_QUEUE );
            exit(0);
        }
    }
    else
    {
        UniqueStr.Printf( "%s Futures Link %d PVC %d",UFC::Hostname,info.GetLink(),info.GetPVC());
        if( FQueue.Open( FUT_X25_QUEUE ) == FALSE )
        {
            UFC::BufferedLog::Printf( " Open queue:%x failed", FUT_X25_QUEUE );
            exit(0);
        }
    }
    FLastNID = new UFC::PInt32( UniqueStr );
    if( RecoverMode == FALSE )
        *FLastNID = 0;
    FPVCID.Printf("%02d",info.GetPVC() );
    if( FParams.GetGroup() == 0 )
        FMType = 0;
    else
        FMType = 100 + FParams.GetGroup();
}
//----------------------------------------------------------------------------------------------------------------------
SpeedyOrderConnectionObject::~SpeedyOrderConnectionObject()
{
    delete FLastNID;
    if( IsTerminated() == FALSE )
    {
        Terminate(); ///< Terminate the thread.
        WaitFor();   ///< Wait for thread terminated.
    }
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::FetchData( UInt8* Data, Int32 Size, Int32& NID, Int32& Tick, char* Key, char* Host, char* Message, BOOL Dump )
{  
    ///< Binary Layout: NID( 4 bytes )| Tick ( 4 bytes )| Key size( 4 Bytes) | Key data |Host size(4 bytes)| Host data|Data size(4 bytes)| Order    
    Int32 KeySize,HostSize,DataSize;       
    MemoryStreamReader MemReader( (char*)Data );
    
    MemReader.Read( (char*)&NID, sizeof(Int32) );     /// Fetch the NID
    MemReader.Read( (char*)&Tick, sizeof(Int32) );    /// Fetch the Inqueue Tick.
    MemReader.Read( (char*)&KeySize, sizeof(Int32) ); /// Fetch the Key String size.   
    if( KeySize > 0 )
        MemReader.ReadString( Key, KeySize );
    else
        Key[ 0 ] = 0;    
    MemReader.Read( (char*)&HostSize, sizeof(Int32) );/// Fetch the Host String size.    
    if( HostSize > 0 )
        MemReader.ReadString( Host, HostSize );
    else
        Host[0] = 0;
    MemReader.Read( (char*)&DataSize, sizeof(Int32) ); /// Fetch the Message String size.   
    MemReader.ReadString( Message, DataSize );
    
    if( Dump == TRUE )
    {
        UFC::BufferedLog::Printf( " ------------ Invalid message,Debug dump----------------" );                
        UFC::BufferedLog::Printf( " NID:[%d] Tick:[%d]",NID,Tick );                
        UFC::BufferedLog::Printf( " Key:[%s] Size:[%d]",Key,KeySize );                                
        UFC::BufferedLog::Printf( " Host:[%s] Size:[%d]",Host, HostSize );                                                
        UFC::BufferedLog::Printf( " Order:[%s] Size:[%d]",Message, DataSize );                                                
        UFC::BufferedLog::Printf( " --------------------------------------------------------" );              
    }
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::IsExpired( MessageHeader* InMessage, Int32 NID, Int32 InqueueTick, const UFC::AnsiString& Key, const UFC::AnsiString& Host )
{
    if( InMessage->MessageType( ) == mtR010 && ((Int32)UFC::GetTickCountMS() - InqueueTick) > ExpiredMS ) ///< Is an expired order
    {
        MTree Data;
        MessageR030 Error;
        UFC::AnsiString X25Time;

        GetTimeString( X25Time, FALSE );
        Error.SetStatusCode( SPEEDY_ERROR_TIMEOUT );
        Error.AsString( FDataStr );
        Data.append( "CONFIRM_ORDER", FDataStr );
        Data.append( "PVC",  FPVCID );
        Data.append( "PBNO", FParams.GetBrokerID() );
        Data.append( "NID",  NID );
        Data.append( "TTIME", X25Time );
        Data.append( "XTIME", X25Time );
        if( Key.Length() > 0 )
            Data.append( "KEY", Key );        
        if( Host.Length() > 0 )
            Data.append( "HOST", Host );        
        else
            Data.append( "HOST", UFC::Hostname );
        Data.append( "PHOST", UFC::Hostname );
        MessageObj->Send( FObjectManager->GetConfirmSubject(), UFC::Hostname, Data  );
        return TRUE;
    }
    return FALSE;
}

//---------------------------------------------------------------------------------------------------------------------
Int32 SpeedyOrderConnectionObject::RecordInfo( MessageHeader* OrderMessage, Int32 NID, const UFC::AnsiString& Key, UFC::AnsiString& OrderID, UFC::AnsiString& IB )
{
    TMessageType Type = OrderMessage->MessageType( );

    if( Type == mtR010 ) ///< Order or quote message
    {
        MessageR010* R010 = (MessageR010*)OrderMessage;        
        UFC::AnsiString Qty( R010->OrderQty, 4 );
        
        OrderID =  UFC::AnsiString( R010->OrderNo, 5 );   
        IB      =  UFC::AnsiString( R010->FcmNo + 4, 3 );   
        switch( R010->GetFunctionCode())
        {
            case 1: FObjectManager->OrderInfo.AddOrder( OrderID, IB, Key, NID, TRUE );///< New Order
                    if( R010->OrderCondition == 'I' && IOCExpired > 0  ) ///< It's a IOC Order.
                        FObjectManager->NewIOC( OrderID, IB, Qty.ToInt() );
                    break;
            case 2: FObjectManager->OrderInfo.PushReplaceNID( OrderID, IB, NID, Key );
                    break;///< Reduce Qty
            case 3: FObjectManager->OrderInfo.PushCancelNID( OrderID, IB, NID, Key );
                    break;///< Cancel Order
        }
    }
    else if( Type == mtR090 )
    {
        MessageR090* R090 = (MessageR090*)OrderMessage;

        OrderID =  UFC::AnsiString( R090->OrderNo, 5 );   
        IB      =  UFC::AnsiString( R090->FcmNo + 4, 3 );   
        switch( R090->GetFunctionCode())
        {
            case 1: FObjectManager->OrderInfo.AddOrder( OrderID, IB, Key, NID, TRUE );break; ///< New Order
            case 2: FObjectManager->OrderInfo.PushReplaceNID( OrderID, IB, NID, Key );break;///< Reduce Qty
            case 3: FObjectManager->OrderInfo.PushCancelNID( OrderID, IB, NID, Key );break;///< Cancel Order
        }
    }
    return  OrderMessage->GetFunctionCode();
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::ReplyMessage( MessageHeader* Msg, Int32 NID, UFC::AnsiString Key, TMessageType& Type , BOOL& NeedLog, UFC::AnsiString& ExecKey, const UFC::AnsiString& Host )
{
    MTree Data;
    UFC::AnsiString TFXTime( Msg->GetTimeStr(), 6 );
    UFC::AnsiString X25Time;

    if( (Type == mtR020 || Type == mtR100 ) && Msg->GetFunctionCode() ==5 )
    {
         NeedLog = FALSE; ///< Order query result don't need to log
                          ///< and skip duplication check.
    }
    else if( Type == mtR020 || Type == mtR100 || Type == mtR080 ) ///< Executions, Must check duplicate.
    {
        if( FObjectManager->OrderInfo.IsExecExist( ExecKey ) == TRUE )
            return; ///< Duplicate...Skip it.
    }    
    TFXTime.PadThis( 9, '0' );
    UFC::GetTimeString( X25Time, FALSE );
    Msg->AsString( FDataStr );
    Data.append( "CONFIRM_ORDER", FDataStr );
    Data.append( "PVC", FPVCID );
    Data.append( "PBNO", FParams.GetBrokerID() );
    Data.append( "NID", NID );
    Data.append( "TTIME", TFXTime );
    Data.append( "XTIME", X25Time );
    Data.append( "PHOST", UFC::Hostname );
    if( Host.Length() > 0 )
        Data.append( "HOST", Host );        
    else     
        Data.append( "HOST", UFC::Hostname );
    if( Key.Length() > 0 )
    {
        Data.append( "KEY", Key );
        if( NeedLog == TRUE )
            FObjectManager->OrderInfo.WriteExecutionLog( TRUE, NID, Key, FDataStr, FParams.GetBrokerID(), UFC::Hostname, FPVCID,-1 ); ///< Write log
    }
    else if( NeedLog == TRUE )
        FObjectManager->OrderInfo.WriteExecutionLog( TRUE, NID, "null", FDataStr, FParams.GetBrokerID(),UFC::Hostname, FPVCID,-1 ); ///< Write log
    MessageObj->Send( FObjectManager->GetConfirmSubject(), UFC::Hostname, Data  );
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::ProcessOrder()
{
    MessageHeader* ReplyMsg;
    MessageHeader* InMessage;
    Int32 NID, InqueueTick,FunctionCode = 1;
    char Key[MAX_DATA_SIZE];
    char Message[MAX_DATA_SIZE];
    char Host[MAX_DATA_SIZE];
    UFC::AnsiString OrderID,IB;

    if( FBrokenRequest == TRUE ) ///< Perform the PVC re-connected query.(R060)
    {
        FBrokenRequest = FALSE;
        InMessage = new MessageR060();
        NID = (Int32)*FLastNID;
    }
    else  ///< Dequeue an order from Speedy.
    {
        try
        {
            FOrder.SetMType( FMType );
            if( FQueue.Recv( FOrder, IDLEHANDSHAKE_TIME_XBIO4 ) == FALSE )
                return FALSE;
            FetchData( (UInt8*)FOrder.GetData(), FOrder.GetSize(), NID, InqueueTick, Key, Host, Message );            
                
            InMessage = MessageHeader::CreateMessage( Message, (Int32)strlen( Message ) );
            if( InMessage == NULL )
            {   ///< Dump error             
                FetchData( (UInt8*)FOrder.GetData(), FOrder.GetSize(), NID,InqueueTick,Key,Host,Message,TRUE );            
                return FALSE;    
            }
            if( IsExpired( InMessage, NID, InqueueTick, Key, Host ) == TRUE ) ///< Stay in queue too long??
                return TRUE;
            FObjectManager->IncreaceOrderCount(); ///< Increace the processed order count.
            FunctionCode = RecordInfo( InMessage, NID, Key, OrderID, IB ); ///< Record the order information.
            *FLastNID = NID; ///< keep the last NID
        }
        catch( UFC::Exception& e )
        {
            UFC::BufferedLog::Printf( " FQueue.Recv Exception:%s",e.what() );
            UFC::BufferedLog::Printf( " MType:%d Len:%d", FOrder.GetMType(), FOrder.GetSize() );            
            return FALSE;
        }
    }
    if( InMessage != NULL )
    {
        InMessage->UpdateTime();
        if( FOrderFCM->OnOrder( InMessage ) == TRUE ) ///< FCMClient:: Send order to TAIFEX.
        {
            TMessageType    ReplyMsgType;
            UFC::AnsiString ExecKey;
            BOOL            NeedLog;

            if( ( ReplyMsg = FOrderFCM->OnHandleReplyMessage( ReplyMsgType, NeedLog, ExecKey )) != NULL ) ///< FCMClient:: Retrive reply from TAIFEX.
            {
		 if( InMessage->GetMessageType() == mtR060 )///< broken query.
		 {
			 if( FObjectManager->OrderInfo.GetOID( NID,OrderID, IB) == TRUE )
			 {
			     UFC::AnsiString OrdKey;
  			     FObjectManager->OrderInfo.GetOrderInfo( OrderID, IB,OrdKey,NID );
			     strcpy( Key, OrdKey.c_str());
			 }
		 }
                 if( ReplyMsgType == mtR030 ) ///< Order Rejected.
                 {
                     UFC::AnsiString OrigKey,CDKey;
                     Int32 OrigNID;
                     
                     if( FunctionCode == 2 ) ///< Reduce
                         FObjectManager->OrderInfo.PopReplaceNID( OrderID, IB, OrigNID, OrigKey, CDKey  );
                     else if( FunctionCode == 3 ) ///< Cancel
                         FObjectManager->OrderInfo.PopCancelNID( OrderID, IB, OrigNID, OrigKey, CDKey  );
                 }
                 if( NID != 0 )
                     ReplyMessage( ReplyMsg, NID, Key, ReplyMsgType, NeedLog, ExecKey, Host );
                 delete ReplyMsg;
            }
            else
                 FReconnect = TRUE;
        }
        delete InMessage;
    }
    return TRUE;
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::OpenMarket()
{
    FOrderFCM->OnOpenMarket();///< FCMClient:: Open market
    FBrokenRequest = TRUE;
    while( TRUE )
    {
        try
        {
             if( FEnabled == FALSE )
             {
                  FReconnect = TRUE;
                  sleep( 1 );
                  if( FOrderFCM->GetStatus() != PVC_STATUS_DISABLE )
                  {
                      FOrderFCM->SetStatus( PVC_STATUS_DISABLE );
                      FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_DISABLED, NULL );
                  }
             }
             else if( FReconnect == TRUE ) ///< Timeout or reconnect
             {
                  FReconnect = FALSE;
                  FOrderFCM->Handshake( ); ///< FCMClient:: Rehandshake with TAIFEX.
                  FBrokenRequest = TRUE;
             }
             else if( FHandshake == TRUE ) ///< Receive a L010...go back to handshake.
             {
                  FHandshake = FALSE;
                  FOrderFCM->Handshake( TRUE ); ///< FCMClient:: Rehandshake with TAIFEX.
                  FBrokenRequest = TRUE;
             }
             else
             {
                if( ProcessOrder() == FALSE )
                    FOrderFCM->OnIdle(); ///< FCMClient:: Perform the idle handshake.
             }
        }
        catch( MarketClosedException&  )
        {
            break;///< Close market.
        }
        catch( X25RehandshakeException& ) // Receive a L010...go back to handshake.
        {
             FOrderFCM->Handshake( TRUE );
             FBrokenRequest = TRUE;
        }
        catch( X25ReconnectException& )  // Timeout or reconnect
        {
             FOrderFCM->Handshake( );
             FBrokenRequest = TRUE;
        }
     }
     FOrderFCM->OnCloseMarket(); ///< FCMClient:: Close market.
     return TRUE;
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::OnCommand( int Command, void* Data )
{
    switch( Command )
    {
        case COMMAND_GET_STATE: if( FOrderFCM != NULL )
                                    *((int*)Data ) = FOrderFCM->GetStatus();
                                else
                                    *((int*)Data ) = PVC_STATUS_BROKEN;
                                break;
        case EVENT_PVC_READ:
        case EVENT_PVC_WRITE:   FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_READ_WRITE, Data );
                                break;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::Listen( BOOL IsListen )
{
    if( FOrderFCM != NULL )
        FOrderFCM->Listen( IsListen );
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::Enable( BOOL IsEnabled )
{
    if( FEnabled != IsEnabled )
    {
        if( IsEnabled == FALSE )
        {
            FOrderFCM->SetStatus( PVC_STATUS_DISABLEING );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_UPDATE_STATE, NULL );
            FOrderFCM->StopHandshake();            
        }
        else
        {
            FOrderFCM->SetStatus( PVC_STATUS_ENABLE );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_ENABLED, NULL );            
        }
        FEnabled = IsEnabled;
        if( FEnabled == FALSE )
	    FQueue.Interrupt();
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::DisableLoop( void )
{
    BOOL FirstTime = TRUE;    
    
    if( FEnabled == FALSE ) ///< Disable this connection.
    {
        while( FEnabled == FALSE ) ///< While this connection disabled.
        {
            sleep( 1 );
            if( FirstTime == TRUE )
            {
                FirstTime = FALSE;
                FOrderFCM->SetStatus( PVC_STATUS_DISABLE);
                FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_DISABLED, NULL );                                
            }
        };
        FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN, NULL );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::Execute( void )
{
    FOrderFCM = new SpeedyOrderClient( FParams, FConnectionListener, this, FObjectManager->GetOrderLog() );
    for( int i = 0; i < PVC_BUSY_RETRY_TIMES; i++ )
    {
        try
        {
            DisableLoop();
            FOrderFCM->Open();
            OpenMarket();
            FOrderFCM->Close();
            break;
        }
        catch( UFC::FileException& e )
        {
            UFC::BufferedLog::Printf( " %s", e.what() );
            return;
        }
        catch( X25Exception& e )
        {
            FOrderFCM->Close();
            UFC::BufferedLog::Printf( " %s. Try to reconnect after 5 sec....",e.what() );
            sleep( 5 );
        }
        catch( std::exception& e )
        {
            FOrderFCM->Close();
            UFC::BufferedLog::Printf( " Exception:%s",e.what() );
            sleep( 5 );
        }
        catch(...)
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR );// Unknown exception.
            return;
        }
    }
    delete FOrderFCM;
    FOrderFCM = NULL;
    return;
}
//----------------------------------------------------------------------------------------------------------------------
