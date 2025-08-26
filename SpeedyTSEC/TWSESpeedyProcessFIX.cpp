#include "TWSESpeedyOrderConnectionObject.h"
#include "TWSESpeedyObjectsManager.h"
#include "TWSESpeedyOrderClient.h"
#include "Profiler.h"
#include "../TSECLib/LinkSubSystem.h"
#include "../TSECLib/Connection.h"
#include "../SpeedyGateway/Administrator.h"
#include "Speedy.h"
//-----------------------------------------------------------------------------------------
extern BOOL                   BENCHMARK_MODE;
extern Int32                  MAX_DATA_SIZE;
extern UFC::PThreadObjectPool<OrderRecord>* OrderRecodPoolPtr;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//-----------------------------------------------------------------------------------------
//  Send direct to socket.(bypass queue)    
//-----------------------------------------------------------------------------------------
// BENCHMARK_MODE = TRUE    
//-----------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::SendFIXOrder( OrderRecord* OI, Int64& SendOrderTick ) 
{
    MessageHeader*  InMessage;          
    OrderInfo*      Info;    
    BOOL            IsNew;        
    Int64           RecordTick;
    Int64           Begin = UFC::GetTickCountUS(); ///< Begin RecordInfo           

    if( FBusying == TRUE || FOrderFCM->IsReady() == false  )
    {        
        UFC::BufferedLog::Printf( " Send %s FIX Session[%s] IsReady[%d] IsBusy[%d] failed ", (FIsTWSE == TRUE)? "TWSE":"OTC", FPVCID, FOrderFCM->IsReady(), FBusying );            
        return FALSE;    ///< Not ready or idleing.    
    }
    FBusying = TRUE;
    {
        if( (InMessage = BeginTransactionEx( OI, TRUE ) ) == NULL )                
        {
             UFC::BufferedLog::Printf( " --- FIX Session[%s] BeginTransactionEx failed ---", FPVCID );            
             FBusying = FALSE;
             return FALSE;   ///< Get message from  OrderRecord failed, skip it.                
        }            
        FObjectManager->IncreaceOrderCount();
        if( (Info = RecordInfo( InMessage, *OI, IsNew, FALSE )) == NULL  && IsNew == TRUE  )///< Record the order information.                                
        {
            char Msg[ 64 ];

            EndTransaction();
            snprintf( Msg, 64, "Duplicate NID[%010u]", OI->NID );        
            FObjectManager->SpeedyReject( *OI, ORDER_CHECKER_REJECT, TRUE, Msg ); 			        
            OI->ClearOrderObject();
            delete InMessage;        
            FBusying = FALSE;
            return TRUE;            
        }
        RecordTick = UFC::GetTickCountUS(); ///< End RecordInfo                         
        FObjectManager->PushOrderRecord( OI->NID, OI );            
        FOrderFCM->OnOrder( InMessage, *OI ); ///< FCMClient:: Send order to EXCHANGE.            
        SendOrderTick = UFC::GetTickCountUS();          
        ///<  Performance Point 3 (3-2 Inqueue time)            
        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppSpeedyOrdDeQueure, UFC::AnsiString(OI->NID), OI->GetOrderID() );                                        
        UFC::BufferedLog::Printf( " #3 [SpeedyFIXOrderClient::OnOrder] total #1+#2 use[%d]us",  SendOrderTick- RecordTick );    
        UFC::BufferedLog::Printf( " #4 [SpeedyOrderConnectionObject::SendFIXOrder] call RecordInfo[%d]us", RecordTick - Begin );                
        if( Info != NULL )
            FObjectManager->OrderInfo.LogNewOrder( Info );    
        ControlFIXTroughput( ); ///< Control throughput.    
        OI->ClearOrderObject();
    }
    FBusying = FALSE;        
    
    delete InMessage;   
    return TRUE;        
}       
//-----------------------------------------------------------------------------------------
// BENCHMARK_MODE = FALSE
//-----------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::SendFIXOrder( OrderRecord* OI )
{    
    MessageHeader*  InMessage;          
    OrderInfo*      Info;    
    BOOL            IsNew;            
    
    if( FOrderFCM->IsReady() == false ||  FBusying == TRUE )
    {        
        if( FIsTWSE == TRUE )
            UFC::BufferedLog::Printf( " Send TSWE FIX Session[%s] IsReady[%d] IsBusy[%d] failed ", FPVCID, FOrderFCM->IsReady(), FBusying );            
        else
            UFC::BufferedLog::Printf( " Send OTC FIX Session[%s] IsReady[%d] IsBusy[%d] failed ", FPVCID, FOrderFCM->IsReady(), FBusying );            
        return FALSE;    ///< Not ready or idleing.    
    }
    FBusying = TRUE;
    {
        if( (InMessage = BeginTransactionEx( OI, TRUE ) ) == NULL )                
        {
             UFC::BufferedLog::Printf( " --- FIX Session[%s] BeginTransactionEx failed ---", FPVCID );            
             FBusying = FALSE;
             return FALSE;   ///< Get message from  OrderRecord failed, skip it.                
        }
        FObjectManager->IncreaceOrderCount();
        if( (Info = RecordInfo( InMessage, *OI, IsNew, FALSE )) == NULL  && IsNew == TRUE  )///< Record the order information.                                
        {
            char Msg[ 64 ];

            EndTransaction();
            snprintf( Msg, 64, "Duplicate NID[%010u]", OI->NID );        
            FObjectManager->SpeedyReject( *OI, ORDER_CHECKER_REJECT, TRUE, Msg ); 			        
            OI->ClearOrderObject();
            delete InMessage;        
            FBusying = FALSE;
            return TRUE;            
        }
        FObjectManager->PushOrderRecord( OI->NID, OI );            
        FOrderFCM->OnOrder( InMessage, *OI ); ///< FCMClient:: Send order to EXCHANGE.
        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppSpeedyOrdDeQueure, UFC::AnsiString(OI->NID), OI->GetOrderID() );                            
        if( Info != NULL )
            FObjectManager->OrderInfo.LogNewOrder( Info );    
        ControlFIXTroughput( ); ///< Control throughput.    
        OI->ClearOrderObject();
    }
    FBusying = FALSE;
    
    delete InMessage;   
    return TRUE;        
}    
//-----------------------------------------------------------------------------------------
//  Control FIX throughput.
//------------------------------------------------------------------------------
void  SpeedyOrderConnectionObject::ControlFIXTroughput( void )
{
    UInt32 Now = UFC::GetTickCountMS();
    Int32  TimeDiff = Now - FFirstOrderTick;
    
    FOutCount++;    
    ///< Control Orderds per second.
    if( TimeDiff < 0 ) ///< Timer overflow or reset...
    {
        FFirstOrderTick = Now;///< Reset timer.
        FOutCount       = 0;  ///< Reset counter.
        UFC::BufferedLog::Printf( " *** Tick reset from[%d] to [%d] ***", FFirstOrderTick, Now );            
    }
    else if( TimeDiff >= 1000 ) ///< More then 1 sec
    {
        FFirstOrderTick = Now;///< Reset timer.
        FOutCount       = 0;  ///< Reset counter.
    }
    else 
    {
        if( FOutCount >= FOrderPerSec )
        {
            Int32 sms = (1000 - TimeDiff)%1000;            
            
            UFC::BufferedLog::Printf( " *** BrokerID[%s] PVCID[%s] throughput[%d] sleep [%d]ms ***", FLineBrokerID.c_str(),FPVCID, FOutCount, sms );            
            UFC::SleepMS( sms );
            FFirstOrderTick = Now;///< Reset timer.
            FOutCount       = 0;  ///< Reset counter.            
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------
//
//    Speedy Standard version
//    Process FIX Order
//
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::ProcessFIXOrder()
{
    MessageHeader*   InMessage     = NULL;     
    OrderRecord*     OI            = new OrderRecord();    
    OrderInfo*       Info;
    BOOL             IsNew;
    char             APCodeNow = FParams.GetAPCode();
    TMarketStatus    SessionStatus = FObjectManager->GetMarketStatus( APCodeNow );           
    
    if( FOrderFCM->IsReady() == false )
    {
        FHandshake = TRUE;
        return FALSE; ///< Session broken.        
    }
    if( SessionStatus == msWait || SessionStatus == msTryOrder ) ///< Process running, whaiting for try order. or begin try order.
    {
        if( FICanTry == TRUE )
        {               
            if( WaitTry() == FALSE ) ///< TRUE, Begin try or Market opened
                return FALSE; ///< FALSE, Idle handshake.
            FOrderFCM->SetStatus( PVC_STATUS_TRY_OPEN );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_TRY_OPEN, NULL );
            if( (InMessage = GenerateTryOrder( *OI )) == NULL )
                return FALSE; ///< Timeout, Perform idle handshake.                    
             ///< Send an order to exchang, and then receiveing a reply message from exchange.
            Info = RecordInfo( InMessage, *OI, IsNew, TRUE ); ///< Record the order information.                    
            FObjectManager->PushOrderRecord( OI->NID, OI );
            UFC::Profiler::SetPerformanceFlag( FIsTWSE? 3:4, 3, UFC::AnsiString(OI->NID), OI->GetOrderID() );                        
            FOrderFCM->OnOrder( InMessage, *OI ); ///< FCMClient:: Send order to EXCHANGE.
            if( FTryEvent.WaitFor( TWSE_IDLEHANDSHAKE_TIME_ORDER ) == TRUE ) ///< Wait for confirm.
            {    
                if( Info != NULL )
                    FObjectManager->OrderInfo.LogNewOrder( Info );
            }
            else ///< TWSE response timeout
                return FALSE;              
            if( InMessage != NULL )
                delete InMessage;                
            return TRUE;
        }
        else           
        {     
            if( FObjectManager->CanITry( APCodeNow ) == TRUE ) ///< Can I try order?
            {
                FICanTry = TRUE; ///< Good luck! This is a lucky PVC! It can try to send order.
                FOrderFCM->SetStatus( PVC_STATUS_TRY_OPEN );
                FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_TRY_OPEN, NULL );            
                UFC::BufferedLog::Printf( " PVC:%s Can try order.", FParams.GetPVCID().c_str() );
                return TRUE;
            }                         ///< Waiting for the market open.  
            else
                return WaitMarketOpen( ); ///< return TRUE  for market opened, FALSE for idle handshake.
        }
    }
    else
    {   
        if( FICanTry == TRUE )
        {            
            FICanTry = FALSE;
            FOrderFCM->SetStatus( PVC_STATUS_ONLINE_BUSY );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_UPDATE_STATE, NULL );                            
            if( (InMessage = GenerateTryOrder( *OI )) == NULL )
                return FALSE; ///< Timeout, Perform idle handshake.                    
            FObjectManager->IncreaceOrderCount();
        }
        else  ///< Dequeue an order from Speedy.
        {
            if( GenerateOrderMessageFromQueue( InMessage, *OI ) == FALSE )                
            {
                delete OI;
                return FALSE; ///< Timeout, Perform idle handshake.            
            }        
            if( InMessage == NULL )                
            {
                delete OI;
                return TRUE; ///< Get message from queue failed, skip it.
            }                
        }
        ///< Send an order to exchange, and then receiveing a reply message from exchange.
        if( (Info = RecordInfo( InMessage, *OI, IsNew, FALSE )) == NULL && IsNew == TRUE ) ///< Record the order information.                    
        {
            UFC::AnsiString Msg;

            EndTransaction();
            Msg.Printf( "Duplicate NID[%010u]", OI->NID );        
            FObjectManager->SpeedyReject( *OI, ORDER_CHECKER_REJECT, TRUE, Msg ); 			        
            delete InMessage;        
            return TRUE;            
        }
        FObjectManager->PushOrderRecord( OI->NID, OI );
        FOrderFCM->OnOrder( InMessage, *OI ); ///< FCMClient:: Send order to EXCHANGE.
        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppSpeedyOrdDeQueure, UFC::AnsiString(OI->NID), OI->GetOrderID() );                        
        if( Info != NULL )
            FObjectManager->OrderInfo.LogNewOrder( Info );
        ControlFIXTroughput( ); ///< Control throughput.
        delete InMessage;    
        return TRUE;    
    }
    return TRUE;
}
//---------------------------------------------------------------------------------------------------------------------
//
//    Speedy Extreme version
//    Process FIX Order
//
//---------------------------------------------------------------------------------------------------------------------
///< FIX Step 1
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::ProcessFIXOrderEx()
{    
    if( FOrderFCM->IsReady() == FALSE ) ///< FIX Session broken.
    {
        FHandshake = TRUE;
        return FALSE;        
    }
    else ///< FIX Session connected.
    {
        TMarketStatus SessionStatus = FObjectManager->GetMarketStatus( FParams.GetAPCode() );          
        
        ///< Process running, whaiting for try order. or begin try order.
        if( SessionStatus == msWait || SessionStatus == msTryOrder ) 
            return ProcessFIXTryOrderEx(); ///< FIX Ster 2 (try)  
        else ///< Market opened.
        {
            if( BENCHMARK_MODE == TRUE )
                return ProcessFIXNormalOrderExBenchmark();
            return ProcessFIXNormalOrderEx();
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------
///< FIX Step 2 (try)
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::ProcessFIXTryOrderEx()
{
    MessageHeader*  InMessage = NULL;     
    OrderRecord*    OI = NULL;    
    OrderInfo*      Info = NULL;    
    BOOL            IsNew;    
    
    if( FICanTry == TRUE )  ///< Has rights to try order.
    {               
        if( WaitTry() == TRUE ) ///< TRUE, Begin try or Market opened
        {
            if( (InMessage = GenerateTryOrderEx( OI )) != NULL ) ///< FIX Step 3 (try) Generate a try order from queue.
            {
                Info = RecordInfo( InMessage, *OI, IsNew, TRUE );///< Record the order information.                                        
                FObjectManager->PushOrderRecord( OI->NID, OI ); ///< Push order record to stack.
                UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppSpeedyOrdDeQueure, UFC::AnsiString(OI->NID), OI->GetOrderID() );                                    
                FOrderFCM->OnOrder( InMessage, *OI ); ///< FCMClient:: Send order to EXCHANGE.
                if( FTryEvent.WaitFor( TWSE_IDLEHANDSHAKE_TIME_ORDER ) == TRUE ) ///< Wait for TWSE confirm.
                {    
                    if( Info != NULL )
                        FObjectManager->OrderInfo.LogNewOrder( Info );
                    delete InMessage;      
                    OI->ClearOrderObject();
                    return TRUE;
                }  
            }
        }
        return FALSE; ///< FALSE, Idle handshake.
    }
    else  ///< No rights to try order         
    {     
        char APCodeNow = FParams.GetAPCode();
        
        if( FObjectManager->CanITry( APCodeNow ) == TRUE ) ///< Can I try order? Ask for the rights.
        {
            FICanTry = TRUE; ///< Good luck! This is a lucky PVC! It can try to send order.
            FOrderFCM->SetStatus( PVC_STATUS_TRY_OPEN );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_TRY_OPEN, NULL );            
            UFC::BufferedLog::Printf( " PVC:%s Can try order.", FParams.GetPVCID().c_str() );
            return TRUE;
        }                         
        else ///< Waiting for the market open.  
            return WaitMarketOpen( ); ///< return TRUE  for market opened, FALSE for idle handshake.
    }    
}
//---------------------------------------------------------------------------------------------------------------------
///< FIX Step 3 (try)
//---------------------------------------------------------------------------------------------------------------------
MessageHeader* SpeedyOrderConnectionObject::GenerateTryOrderEx( OrderRecord*& OI )
{    
    MessageHeader *TryMsg = NULL;
    
    ///< Send session status to SpeedyCenter.
    FOrderFCM->SetStatus( PVC_STATUS_ONLINE_BUSY );
    FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_UPDATE_STATE, NULL );                                
    if( IsTransactionComplete() == FALSE ) ///< A test message exists.
    {        	    
        OI = OrderRecodPoolPtr->GetInstance();
        OI->ClearOrderObject();
        if( ( TryMsg = RollbackTransaction( *OI ) ) != NULL ) ///< Rebuild the last order from share memory
        { 
            if( TryMsg->GetSubSystem() != FSubSystem )                
            {
                UFC::BufferedLog::Printf( " It's a previous session, Order:[%s]", OI->GetOrder() );
                delete TryMsg;                
                EndTransaction();        
                TryMsg = NULL;                
            }
            else
                UFC::BufferedLog::DebugPrintf( UFC::dlPerformance, " Get a try Order[%s] from share memory.", OI->GetOrder() );            
        }
        else
        {
            UFC::BufferedLog::Printf( " Invalid Last try Order:[%s]", OI->GetOrder() );
            EndTransaction();
        }
    }
    else ///< No trying order in share memory. Create a new one from Lock free Queue.
    {
        try
        {            
            UFC::PLongList MTypes;
        
            MTypeSet2( MTypes );
            if( FLFQueue->pop( MTypes, &OI , TWSE_IDLEHANDSHAKE_TIME_ORDER ) == true ) ////< De-queue from lock free queue
            {
                if( (TryMsg = BeginTransactionEx( OI, true /*Keep Order in share memory*/) ) == NULL ) ///< FIX Step 4 (try)                       
                {
                    UFC::BufferedLog::Printf( " ----------------------- Order Data format Error!!--------------------------------");
                    UFC::BufferedLog::Printf( " PVC:%s Receive invalid message from Queue.", FParams.GetPVCID().c_str() );
                    UFC::BufferedLog::Printf( " Data [%s].", OI->GetOrder() );
                    UFC::BufferedLog::Printf( " Try to reconnect to Exchange." );
                    UFC::BufferedLog::Printf( " ---------------------------------------------------------------------------------");
                    FReconnect = TRUE;
                    EndTransaction();
                }   
            }
            else
                UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " PVC:%s get try order from queue timeout. SubSystem:%d", FParams.GetPVCID().c_str(), FSubSystem );                            
        }
        catch( UFC::Exception& e )
        {
            UFC::BufferedLog::Printf( " PVC:%s FQueue.Recv Exception:%s", FParams.GetPVCID().c_str(), e.what() );
            UFC::BufferedLog::Printf( " MType:%d Len:%d", FQueueData.GetMType(), FQueueData.GetSize() );
            FReconnect = TRUE;
            EndTransaction();        
            TryMsg = NULL;
        }
    }  
    if( TryMsg != NULL )    
        SetTrying( TRUE );
    return TryMsg;
}  
//---------------------------------------------------------------------------------------------------------------------
///< FIX Step 4 (try rollback order)
//---------------------------------------------------------------------------------------------------------------------
MessageHeader* SpeedyOrderConnectionObject::RollbackTransaction(  OrderRecord& OI  )
{        
    MessageHeader *OrderMessage;
            
    OI.ProcessingStockID = 0;
    OI.IsBroken = TRUE;    
    FetchOrderFromSHM( OI );///< Rebuild the last order.       
    ///< Create order object from row data.
    if( (OrderMessage = MessageHeader::CreateMessage( OI.GetOrder(), OI.GetSize())) != NULL )
    {            
        OI.MsgType  = OrderMessage->MessageType();
        OI.FuncCode = OrderMessage->GetFunctionCode();
        GetOrderID( (TMessageType)OI.MsgType, OrderMessage, OI.GetOrderID() );
        UFC::BufferedLog::Printf( " RollbackTransaction PVC[%s] OID[%s] FuncCode[%d] SubSystem[%d].", FParams.GetPVCID().c_str(), OI.GetOrderID(), OI.FuncCode, FSubSystem);
        if( ( ( OI.FuncCode == 1 ) || ( OI.FuncCode == 2 ) ) && 
            ( OI.MsgType == mtT010 ) && ( FIsTWSE == TRUE ) ) ///< Is a TSE new order
             OI.ProcessingStockID = UFC::AnsiString( ((MessageT010*)OrderMessage)->StockID, 6 ).ToInt(); ///< Get the processing Stock ID.        			         	            	
    }  //if( OrderMessage != NULL )
    return OrderMessage;
} 
//---------------------------------------------------------------------------------------------------------------------
///< FIX Step 4 (try new order)
//---------------------------------------------------------------------------------------------------------------------
MessageHeader* SpeedyOrderConnectionObject::BeginTransactionEx( OrderRecord*& OI, bool KeepOrder  )
{    
    MessageHeader* OrderMessage;
    
    if( KeepOrder == true )
    {
        SaveOrderToSHM( *OI );/// Store the Order to share memory.              
        OpenTransaction(); ///< Set the first byte to 1, mark it's dirty. 
    }
    ///<  Get Order object from OrderRecord class.     
    if( OI->GetOrderObject() != NULL )
        OrderMessage = (MessageHeader*)(OI->GetOrderObject());
    else ///< Create order object from row data.
        OrderMessage = MessageHeader::CreateMessage( OI->GetOrder(), OI->GetSize() );            
    if( OrderMessage != NULL )
    {
        OI->MsgType  = OrderMessage->MessageType();
        OI->FuncCode = OrderMessage->GetFunctionCode();
        GetOrderID( (TMessageType)OI->MsgType , OrderMessage, OI->FOID );        
        #ifndef __EXTREME_VERSION ///< Extreme version not support rush stock function.  
        if( ( FIsTWSE == TRUE ) &&                              ///< Is TWSE  system
            ( OI->MsgType == mtT010 ) &&                        ///< Is T010 Message
            ( ( OI->FuncCode == 1 ) || ( OI->FuncCode == 2 ) ) ) ///< Is a new order
             OI->ProcessingStockID = UFC::AnsiString::StrToInt32( ((MessageT010*)OrderMessage)->StockID, 6 ); ///< Get the processing Stock ID.
        #endif
    }
    return OrderMessage;
}    
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::ProcessFIXNormalOrderEx()
{
    MessageHeader*  InMessage = NULL;     
    OrderRecord*    OI = NULL;    
    OrderInfo*      Info;
    char            Msg[ 64 ];
    BOOL            IsNew;
        
    if( FICanTry == TRUE )   ///< Has rights to try order.
    {            
        FICanTry = FALSE;
        if( (InMessage = GenerateTryOrderEx( OI )) == NULL )
            return FALSE; ///< Timeout, Perform idle handshake.                    
        FObjectManager->IncreaceOrderCount();
    }
    else  ///< Dequeue an order from Speedy order queue.
    {        
        if( GenerateOrderMessageFromQueueEx( InMessage, OI, false ) == FALSE )                
            return FALSE; ///< Timeout, Perform idle handshake.            
        if( InMessage == NULL )                
            return FALSE; ///< Get message from queue failed, skip it.                
    }    
    if( (Info = RecordInfo( InMessage, *OI, IsNew, FALSE )) == NULL  && IsNew == TRUE  )///< Record the order information.                                
    {
        EndTransaction();
        snprintf( Msg, 64, "Duplicate NID[%010u]", OI->NID );        
        FObjectManager->SpeedyReject( *OI, ORDER_CHECKER_REJECT, TRUE, Msg ); 			        
        OI->ClearOrderObject();
        delete InMessage;        
        return TRUE;            
    }
    FObjectManager->PushOrderRecord( OI->NID, OI );            
    FOrderFCM->OnOrder( InMessage, *OI ); ///< FCMClient:: Send order to EXCHANGE.
    UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppSpeedyOrdDeQueure, UFC::AnsiString(OI->NID), OI->GetOrderID() );                            
    if( Info != NULL )
        FObjectManager->OrderInfo.LogNewOrder( Info );
    ControlFIXTroughput( ); ///< Control throughput.    
    OI->ClearOrderObject();
    delete InMessage;   
    return TRUE;        
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::ProcessFIXNormalOrderExBenchmark()
{
    MessageHeader*  InMessage = NULL;     
    OrderRecord*    OI = NULL;    
    UFC::UInt64     Begin,RecordOrder,SendOrder;
    UFC::Int64      DQTotal,Total,InQ;
    OrderInfo*      Info;
    char            Msg[ 64 ];
    BOOL            IsNew;
        
    if( FICanTry == TRUE )   ///< Has rights to try order.
    {            
        FICanTry = FALSE;
        if( (InMessage = GenerateTryOrderEx( OI )) == NULL )
            return FALSE; ///< Timeout, Perform idle handshake.                    
        FObjectManager->IncreaceOrderCount();
    }
    else  ///< Dequeue an order from Speedy order queue.
    {        
        if( GenerateOrderMessageFromQueueEx( InMessage, OI, false ) == FALSE )                
            return FALSE; ///< Timeout, Perform idle handshake.            
        if( InMessage == NULL )                
            return FALSE; ///< Get message from queue failed, skip it.                        
    }    
    Begin = UFC::GetTickCountUS();           
    if( (Info = RecordInfo( InMessage, *OI, IsNew, FALSE )) == NULL  && IsNew == TRUE  )///< Record the order information.                                
    {
        EndTransaction();
        snprintf( Msg, 64, "Duplicate NID[%010u]", OI->NID );        
        FObjectManager->SpeedyReject( *OI, ORDER_CHECKER_REJECT, TRUE, Msg ); 			        
        OI->ClearOrderObject();
        delete InMessage;        
        return TRUE;            
    }
    FObjectManager->PushOrderRecord( OI->NID, OI );        
    RecordOrder = UFC::GetTickCountUS();   
    FOrderFCM->OnOrder( InMessage, *OI ); ///< FCMClient:: Send order to EXCHANGE.
    SendOrder = UFC::GetTickCountUS();
    UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppSpeedyOrdDeQueure, UFC::AnsiString(OI->NID), OI->GetOrderID() );                        
    
    DQTotal = SendOrder - Begin;
    Total   = SendOrder - OI->Tick;
    InQ     = Begin - OI->InQTick;
    
    UFC::BufferedLog::Printf( " [ProcessTextModeOrder] Record[%d]+[OnOrder][%d]=[%d]us", RecordOrder - Begin, SendOrder - RecordOrder, DQTotal );            
    UFC::BufferedLog::Printf( " [In-Queue] Time[%d]us", InQ );
    UFC::BufferedLog::Printf( " --- NewOrder use [ProcessTextModeOrder]+[In-Queue]=[%d]us ---", Total );    
    if( Info != NULL )
        FObjectManager->OrderInfo.LogNewOrder( Info );
    ControlFIXTroughput( ); ///< Control throughput.
    delete InMessage;   
    OI->ClearOrderObject();
    return TRUE;        
}    
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------

