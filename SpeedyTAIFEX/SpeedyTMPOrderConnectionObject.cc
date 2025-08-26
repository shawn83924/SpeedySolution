#include "SpeedyOrderConnectionObject.h"
#include "SpeedyObjectsManager.h"
#include "SpeedyOrderClient.h"
#include "Profiler.h"
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
SpeedyTMPOrderConnectionObject::SpeedyTMPOrderConnectionObject( SpeedyObjectsManager*  ObjManager,
                                                                ConnectionParameter& info,
                                                                ConnectionListener* CListener,
                                                                UFC::ThreadListener* TListener,
                                                                BOOL IsOpt )
:ConnectionObjectBase( info ),
 UFC::PThread( TListener, TRUE ),
 FObjectManager( ObjManager ), 
 FOrderFCM( NULL ),
 FConnectionListener( CListener ),
 FReconnect( FALSE ),
 FHandshake( FALSE ),
 FEnabled( info.IsActive() ),
 FReadException( FALSE ),
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
    FPBNO.Printf( "%s%03d", info.GetBrokerID().SubString(0,4).c_str(), info.GetPVC() );
    FPVCID.Printf( "%03d", info.GetPVC() );
    if( FParams.GetGroup() == 0 )
        FMType = 0;
    else
        FMType = 100 + FParams.GetGroup();
}
//----------------------------------------------------------------------------------------------------------------------
SpeedyTMPOrderConnectionObject::~SpeedyTMPOrderConnectionObject()
{
    if( IsTerminated() == FALSE )
    {
        Terminate(); ///< Terminate the thread.
        WaitFor();   ///< Wait for thread terminated.
    }
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderConnectionObject::FetchData( UInt8* Data, Int32 Size, Int32& NID, Int32& Tick, char* Key, char* Host, char* Message, BOOL Dump )
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
BOOL SpeedyTMPOrderConnectionObject::IsExpired( MessageHeader* InMessage, Int32 NID, Int32 InqueueTick, const UFC::AnsiString& Key, const UFC::AnsiString& Host )
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
        Data.append( "PBNO", FPBNO );
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
Int32 SpeedyTMPOrderConnectionObject::RecordInfo( MessageHeader* OrderMessage, Int32 NID, const UFC::AnsiString& Key, UFC::AnsiString& OrderID, UFC::AnsiString& IB )
{
    TMessageType Type         = OrderMessage->MessageType( );
    Int32        FunctionCode = OrderMessage->GetFunctionCode();

    if( Type == mtR010 ) ///< Order message
    {
        MessageR010* R010 = static_cast<MessageR010*>(OrderMessage);        
       
        OrderID =  UFC::AnsiString( R010->OrderNo, 5 );   
        IB      =  UFC::AnsiString( R010->FcmNo + 4, 3 );           
    }
    else if( Type == mtR090 ) /// Quote message
    {
        MessageR090* R090 = static_cast<MessageR090*>(OrderMessage);

        OrderID =  UFC::AnsiString( R090->OrderNo, 5 );   
        IB      =  UFC::AnsiString( R090->FcmNo + 4, 3 );   
    }
    else
        return  FunctionCode;
    switch( FunctionCode )
    {
        case 1: FObjectManager->OrderInfo.AddOrder( OrderID, IB, Key, NID, TRUE );
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " AddOrder NID[%d] OrderID[%s] Key[%s]",NID,OrderID.c_str(), Key.c_str() );
                break;///< New Order,TMP don't need special handle for IOC Order.
        case 2: FObjectManager->OrderInfo.PushReplaceNID(  /*In*/OrderID, /*In*/IB, /*In*/NID, /*In*/Key, /*In*/TRUE );
                break; ///< Reduce Qty
        case 3: FObjectManager->OrderInfo.PushCancelNID(  /*In*/OrderID, /*In*/IB, /*In*/NID, /*In*/Key, /*In*/TRUE );
                break; ///< Cancel Order
        case 5: FObjectManager->OrderInfo.AddNID( OrderID, IB, NID, Key );
                break; ///< Order Status request.
    }
    return  FunctionCode;
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyTMPOrderConnectionObject::ProcessOrder()
{
    MessageHeader* InMessage;
    Int32 NID, InqueueTick,FunctionCode = 1;
    char Key[ MAX_DATA_SIZE ];
    char Host[ MAX_DATA_SIZE ];
    char Message[ MAX_DATA_SIZE ];    
    UFC::AnsiString OrderID,IB;    
    
    try ///< Dequeue an order from Speedy.
    {   
        ///< Get order data from queue. 
        FOrder.SetMType( FMType );
        if( FQueue.Recv( FOrder, FOrderFCM->GetHeartBeatInterval( ) ) == FALSE )            
            return FALSE;
        ///< Get order information from queue.
        FetchData( (UInt8*)FOrder.GetData(), FOrder.GetSize(), NID, InqueueTick, Key, Host, Message );
        
        if( (InMessage = MessageHeader::CreateMessage( Message,(Int32)strlen(Message))) == NULL ) 
        {   ///< Dump error
            FetchData( (UInt8*)FOrder.GetData(), FOrder.GetSize(), NID, InqueueTick, Key, Host, Message, TRUE );
            return FALSE;
        }
        ///< Stay in queue too long?? Check order expired.
        if( IsExpired( InMessage, NID, InqueueTick, Key, Host ) == TRUE ) 
        {
            delete InMessage;
            return TRUE;
        }
        FunctionCode = RecordInfo( InMessage, NID, Key, OrderID, IB ); ///< Record the order information.
        if( FObjectManager->IsOptions() == TRUE )
            UFC::Profiler::SetPerformanceFlag( 2, 3, UFC::AnsiString(NID), OrderID );
        else
            UFC::Profiler::SetPerformanceFlag( 1, 3, UFC::AnsiString(NID), OrderID );
        FOrderFCM->OnOrder( InMessage, NID );
        FObjectManager->IncreaceOrderCount(); ///< Increace the processed order count.        
        delete InMessage;
        return TRUE;        
    }
    catch( UFC::Exception& e ) 
    {
        UFC::BufferedLog::Printf( " FQueue.Recv Exception:%s", e.what() );
        UFC::BufferedLog::Printf( " MType:%d Len:%d", FOrder.GetMType(), FOrder.GetSize() );
        return FALSE;
    }
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyTMPOrderConnectionObject::OpenMarket()
{
    FOrderFCM->OnOpenMarket();///< FCMClient:: Open market
    
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
             else if( FReadException == TRUE )
             {
                  FReadException = FALSE;                  
                  throw( X25Exception(-19,"Read socket exception."));
             }                 
             else if( FReconnect == TRUE ) ///< Timeout or reconnect
             {
                  FReconnect = FALSE;
                  FOrderFCM->Handshake( ); ///< FCMClient:: Rehandshake with TAIFEX.
             }
             else if( FHandshake == TRUE ) ///< Receive a L010...go back to handshake.
             {
                  FHandshake = FALSE;
                  FOrderFCM->Handshake( TRUE ); ///< FCMClient:: Rehandshake with TAIFEX.
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
        }
        catch( X25ReconnectException& )  // Timeout or reconnect
        {
             FOrderFCM->Handshake( );             
        }
     }
     FOrderFCM->OnCloseMarket(); ///< FCMClient:: Close market.
     return TRUE;
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderConnectionObject::SetSystemTime( UInt32 Tick )
{
       Int32     hh =  Tick / 3600000;         ///< 1 hour = 60*60*1000 ms
       Int32     mm = (Tick % 3600000)/60000;  ///< 1 minutes = 60*1000 ms
       Int32     ss = (Tick % 60000)/1000;     ///< 1 sec        = 1000 ms
       Int32     ms =  Tick % 1000;
       Int32     hhmmss;
       MTree     Data;

       hhmmss = hh*10000 + mm*100 + ss;
       Data.append( "Now", hhmmss );
       Data.append( "ms", ms );
       MessageObj->Send( "SPEEDY.TIME", UFC::Hostname, Data );
       UFC::BufferedLog::Printf( " Set System Time to %02d:%02d:%02d.%03d", hh,mm,ss,ms );
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderConnectionObject::OnCommand( int Command, void* Data )
{
    switch( Command )
    {
        case COMMAND_GET_STATE:     if( FOrderFCM != NULL )
                                        *((int*)Data ) = FOrderFCM->GetStatus();
                                    else
                                        *((int*)Data ) = PVC_STATUS_BROKEN;
                                    break;
        case EVENT_PVC_READ:
        case EVENT_PVC_WRITE:       FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_READ_WRITE, Data );
                                    break;
        case EVENT_TMP_RECOVER_MSG: if( FOrderFCM != NULL )
                                        FOrderFCM->OnTMPMessage((TMP::TMPMessage*)Data);
                                    break;                                
        case EVENT_TMP_RECONNECT:   FReadException = TRUE;
                                    FQueue.Interrupt();
                                    break;
        case EVENT_TMP_GET_MSG:     if( FOrderFCM != NULL )
                                        FOrderFCM->OnOrder((TMP::TMPMessage*)Data, 0);
                                    break;
        case EVENT_TMP_SET_TIME:    SetSystemTime( (UInt32) Data );
                                    break;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderConnectionObject::Listen( BOOL IsListen )
{
    if( FOrderFCM != NULL )
        FOrderFCM->Listen( IsListen );
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderConnectionObject::Enable( BOOL IsEnabled )
{
    if( FEnabled != IsEnabled )
    {
        if( IsEnabled == FALSE )
        {
            FOrderFCM->SetStatus( PVC_STATUS_DISABLEING );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_UPDATE_STATE, NULL );
            FOrderFCM->StopHandshake();
            FOrderFCM->Close();
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
void SpeedyTMPOrderConnectionObject::DisableLoop( void )
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
        throw( X25Exception(-20,"User activate this connection, try to reconnect."));
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyTMPOrderConnectionObject::Execute( void )
{
    FOrderFCM = new SpeedyTMPOrderClient( FObjectManager, FParams,FConnectionListener, this );
    for( int i = 0; i < PVC_BUSY_RETRY_TIMES; i++ )
    {
        try
        {
            DisableLoop();
            FOrderFCM->Open();
            OpenMarket();
            UFC::BufferedLog::Printf( " Market closed" );
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
            FReadException = FALSE;                  
            FReconnect     = FALSE;
            FHandshake     = FALSE;
            UFC::BufferedLog::Printf( " %s. Try to reconnect after 5 sec....",e.what() );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
            sleep( 5 );
        }
        catch( std::exception& e )
        {
            FOrderFCM->Close();
            FReadException = FALSE;                  
            FReconnect     = FALSE;
            FHandshake     = FALSE;
            UFC::BufferedLog::Printf( " Exception:%s",e.what() );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN );
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
