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
extern UFC::PThreadObjectPool<OrderRecord>* OrderRecodPoolPtr;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
    //---------------------------------------------------------------------------------------------------------------------
MessageHeader* SpeedyOrderConnectionObject::RebuildIncompleteOrderAndGenerateBrokenQueryMessage( OrderRecord& OI )
{   
    if( IsTransactionComplete() == FALSE ) ///< A unreplay message exists.
    {
        MessageHeader*  OrigMsg;        
        
        if( ( OrigMsg = RollbackTransaction( OI ) ) != NULL )///< Rebuild the last order.	    
        { 
            delete OrigMsg;                        
            switch( FSubSystem ) ///< Generate last execution query message.
            {
                case 93: UFC::BufferedLog::Printf( " PVC:%s Query last execution for Normal trade session", FParams.GetPVCID().c_str() );                      
                         return new MessageT060( FALSE ); ///< Normal
                case 30: UFC::BufferedLog::Printf( " PVC:%s Query last execution for Normal trade session", FParams.GetPVCID().c_str() );                      
                         return new MessageT060( TRUE ); ///< Normal
                case 94: UFC::BufferedLog::Printf( " PVC:%s Query last execution for Odd trade session", FParams.GetPVCID().c_str() );  
                         return new MessageO090( FALSE ); ///< Odd stock
                case 40: UFC::BufferedLog::Printf( " PVC:%s Query last execution for Odd trade session", FParams.GetPVCID().c_str() );  
                         return new MessageO090( TRUE ); ///< Odd stock
                         
                case 83: UFC::BufferedLog::Printf( " PVC:%s Query last execution for OddEx trade session", FParams.GetPVCID().c_str() );                      
                         return new MessageO160( FALSE ); ///< Odd Ex
                case 33: UFC::BufferedLog::Printf( " PVC:%s Query last execution for OddEx trade session", FParams.GetPVCID().c_str() );                      
                         return new MessageO160( TRUE ); ///< Odd Ex                         
                         
                case 98: UFC::BufferedLog::Printf( " PVC:%s Query last execution for Post trade session", FParams.GetPVCID().c_str() );
                         return new MessageP060( FALSE ); ///< Post trade                           
                case 32: UFC::BufferedLog::Printf( " PVC:%s Query last execution for Post trade session", FParams.GetPVCID().c_str() );
                         return new MessageP060( TRUE ); ///< Post trade        
                case 96: return NULL;              ///< Negotiate Price session not support broken query.
                case 70: UFC::BufferedLog::Printf( " PVC:%s Query last execution for Aution offer trade session", FParams.GetPVCID().c_str() );
                         return new MessageA060(); ///< Aution trade                                                  
                case 97: UFC::BufferedLog::Printf( " PVC:%s Query last execution for Tender offer trade session", FParams.GetPVCID().c_str() );
                         return new MessageE060( FALSE ); ///< Tender offer trade                                            
                case 41: UFC::BufferedLog::Printf( " PVC:%s Query last execution for Tender offer trade session", FParams.GetPVCID().c_str() );
                         return new MessageE060( TRUE ); ///< Tender offer trade                         
                case 89: UFC::BufferedLog::Printf( " PVC:%s Query last execution for TenderEx offer trade session", FParams.GetPVCID().c_str() );
                         return new MessageEx060( FALSE ); ///< Tender offer trade                                                  
                case 42: UFC::BufferedLog::Printf( " PVC:%s Query last execution for TenderEx offer trade session", FParams.GetPVCID().c_str() );
                         return new MessageEx060( TRUE ); ///< Tender offer trade                                                  
                case 90: UFC::BufferedLog::Printf( " PVC:%s Query last execution for Lending trade session", FParams.GetPVCID().c_str() );
                         return new MessageV060( FALSE ); ///< Lending trade                   
                case 31: UFC::BufferedLog::Printf( " PVC:%s Query last execution for Lending trade session", FParams.GetPVCID().c_str() );
                         return new MessageV060( TRUE ); ///< Lending trade
                default: UFC::BufferedLog::Printf( " Unknown subsystem:[%d]", FSubSystem ); 
                         return NULL;
            } 
        }
        else
        {
            UFC::BufferedLog::Printf( " PVC:%s Invalid Last Order:[%s]", FParams.GetPVCID().c_str(), OI.GetOrder() );
            EndTransaction();  
            return NULL;
        }
    }
    UFC::BufferedLog::Printf( " PVC:%s No incomplete transaction, skip last execution query",  FParams.GetPVCID().c_str() );
    return NULL;
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::ProcessOrder()
{
    MessageHeader*   ReplyMsg;    
    MessageHeader*   InMessage = NULL; 
    OrderRecord      OI;
    OrderInfo*       Info;
    char             APCodeNow = FParams.GetAPCode();
    BOOL             IsNew;
    TMarketStatus    SessionStatus = FObjectManager->GetMarketStatus( APCodeNow );       
    
    UFC::BufferedLog::DebugPrintf( UFC::dlPerformance, " SpeedyOrderConnectionObject::ProcessOrder() APCodeNow='%c' SessionStatus=%d(%s) ICanTry=%d SubSystem=%d", APCodeNow, SessionStatus, DescribeMarketStatus(SessionStatus).c_str(), FICanTry, FSubSystem );
    if( SessionStatus == msWait || SessionStatus == msTryOrder ) ///< Process running, whaiting for try order. or begin try order.
    {
        if( FICanTry == TRUE )
        {               
            if( WaitTry() == FALSE ) ///< TRUE, Begin try or Market opened
                return FALSE; ///< FALSE, Idle handshake.
            FOrderFCM->SetStatus( PVC_STATUS_TRY_OPEN );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_TRY_OPEN, NULL );
            if( (InMessage = GenerateTryOrder( OI )) == NULL )
                return FALSE; ///< Timeout, Perform idle handshake.        
            ///< Send an order to exchang, and then receiveing a reply message from exchange.
            Info = RecordInfo( InMessage, OI, IsNew, TRUE ); ///< Record the order information.                                
            if( FOrderFCM->OnOrder( InMessage, OI ) == TRUE ) ///< FCMClient:: Send order to EXCHANGE.
            {
                TMessageType    ReplyMsgType;
                UFC::AnsiString ExecKey,OrigKey,BrokerID;
                BOOL            NeedLog,IsCD;
                
                if( ( ReplyMsg = FOrderFCM->OnHandleReplyMessage( ReplyMsgType, NeedLog, IsCD, ExecKey,OrigKey,BrokerID )) != NULL ) ///< FCMClient:: Retrive reply from TAIFEX.
                {
                     UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OI.NID ),OI.GetOrder() );        
                     if( Info != NULL )
                         FObjectManager->OrderInfo.LogNewOrder( Info );
                     HandleExchangeReplyTryMessage( ReplyMsg, OI, ReplyMsgType, NeedLog, IsCD, ExecKey, OrigKey/*,BrokerID*/  );
                     delete ReplyMsg;
                }               
            }
            if( InMessage != NULL )
                delete InMessage;    
            return TRUE;
        }
        else           
        {     
            if( FObjectManager->CanITry( FParams.GetAPCode()) == TRUE ) ///< Can I try order?
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
        if( ( SessionStatus != msOpened ) && 
            ( ( APCodeNow == '0' ) || ( APCodeNow =='4' ) || ( APCodeNow =='9' ) ) )
        {
            UFC::BufferedLog::Printf( " PVCID[%s] ProcessOrder Market Closed.", FParams.GetPVCID().c_str() );
            return FALSE;            
        }
        else if( FICanTry == TRUE )
        {            
            FICanTry = FALSE;
            FOrderFCM->SetStatus( PVC_STATUS_ONLINE_BUSY );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_UPDATE_STATE, NULL );                            
            if( (InMessage = GenerateTryOrder( OI )) == NULL )
                return FALSE; ///< Timeout, Perform idle handshake.                    
            FObjectManager->IncreaceOrderCount();
        }
        else if( FBrokenRequest == TRUE  ) ///< Just reconnected.perform last execution query.
        {            
            FBrokenRequest = FALSE; ///< Turn off broken query flag.        
            if( (InMessage = RebuildIncompleteOrderAndGenerateBrokenQueryMessage( OI ) ) == NULL)
                return TRUE; ///< Don't need to do last execution query.(No incomplete order in share memory.)
        }
        else  ///< Dequeue an order from Speedy.
        {            
            if( GenerateOrderMessageFromQueue( InMessage, OI ) == FALSE )                
                return FALSE; ///< Timeout, Perform idle handshake.            
            if( InMessage == NULL )                
                return TRUE; ///< Get message from queue failed, skip it.            
        }        
        ///< Send an order to exchang, and then receiveing a reply message from exchange.                
        if( (Info = RecordInfo( InMessage, OI, IsNew, FALSE )) == NULL  && IsNew == TRUE) ///< Record the order information.                    
        {
            UFC::AnsiString Msg;

            EndTransaction();
            Msg.Printf( "Duplicate NID[%010u]", OI.NID );        
            FObjectManager->SpeedyReject( OI, ORDER_CHECKER_REJECT, TRUE, Msg ); 			        
            delete InMessage;        
            return TRUE;            
        }
        if( FOrderFCM->OnOrder( InMessage, OI ) == TRUE ) ///< FCMClient:: Send order to EXCHANGE.
        {            
            try
            {
                TMessageType    ReplyMsgType; ///< Out variables from func OnHandleReplyMessage 
                UFC::AnsiString ExecKey,OrigKey,BrokerID; ///< Out variables from func OnHandleReplyMessage 
                BOOL            NeedLog,IsCD; ///< Out variables from func OnHandleReplyMessage 

                UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppSpeedyOrdDeQueure, UFC::AnsiString(OI.NID), OI.GetOrderID() );                
                if( Info != NULL )
                    FObjectManager->OrderInfo.LogNewOrder( Info );
                if( ( ReplyMsg = FOrderFCM->OnHandleReplyMessage( ReplyMsgType, NeedLog, IsCD, ExecKey,OrigKey,BrokerID )) != NULL ) ///< FCMClient:: Retrive reply from TAIFEX.
                {
                     UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OI.NID ),OI.GetOrder() );        
                     HandleExchangeReplyMessage( ReplyMsg, OI, 0, ReplyMsgType, NeedLog, IsCD, ExecKey, OrigKey );
                     delete ReplyMsg;
                }               
                else ///< time out.
                    HandleExchangeReplyTimeout( OI );        
            }
            catch( UFC::Exception& e )
            {
                HandleExchangeReplyTimeout( OI, SPEEDY_ERROR_CONNECTION );
                throw X25ReconnectException( "Exchange replay confirm execution timeout" );
            }
        }
        if( InMessage != NULL )
            delete InMessage;    
        return TRUE;
    }
    return TRUE;
}    
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::ProcessOrderEx()
{
    MessageHeader*   ReplyMsg;    
    MessageHeader*   InMessage = NULL; 
    OrderRecord*     OI = NULL;
    OrderInfo*       Info;
    BOOL             IsNew;
    char             APCodeNow = FParams.GetAPCode();
    TMarketStatus    SessionStatus = FObjectManager->GetMarketStatus( APCodeNow );       
    UFC::Int64       Begin = 0;    
    UFC::Int64       Now = 0;
    UFC::Int64       RecordOrder = 0;
    UFC::Int64       SendOrder = 0;        
  
    if( SessionStatus == msWait || SessionStatus == msTryOrder ) ///< Process running, whaiting for try order. or begin try order.    
    {
        if( FICanTry == TRUE )
        {               
            if( WaitTry() == FALSE ) ///< TRUE, Begin try or Market opened
                return FALSE; ///< FALSE, Idle handshake.            
            if( (InMessage = GenerateTryOrderEx( OI )) == NULL )
                return FALSE; ///< Timeout, Perform idle handshake.        
            ///< Send an order to exchang, and then receiveing a reply message from exchange.
            Info = RecordInfo( InMessage, *OI, IsNew, TRUE ); ///< Record the order information.                                
            if( FOrderFCM->OnOrder( InMessage, *OI ) == TRUE ) ///< FCMClient:: Send order to EXCHANGE.
            {
                TMessageType    ReplyMsgType;
                UFC::AnsiString ExecKey,OrigKey,BrokerID;
                BOOL            NeedLog,IsCD;
                
                if( ( ReplyMsg = FOrderFCM->OnHandleReplyMessage( ReplyMsgType, NeedLog, IsCD, ExecKey,OrigKey,BrokerID )) != NULL ) ///< FCMClient:: Retrive reply from TAIFEX.
                {
                     UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OI->NID ),OI->GetOrder() );        
                     if( Info != NULL )
                         FObjectManager->OrderInfo.LogNewOrder( Info );
                     HandleExchangeReplyTryMessage( ReplyMsg, *OI, ReplyMsgType, NeedLog, IsCD, ExecKey, OrigKey/*,BrokerID*/  );
                     delete ReplyMsg;
                }               
            }
            if( InMessage != NULL )
                delete InMessage;    
            return TRUE;
        }
        else           
        {     
            if( FObjectManager->CanITry( FParams.GetAPCode()) == TRUE ) ///< Can I try order?
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
        if( ( SessionStatus != msOpened ) && 
            ( ( APCodeNow == '0' ) || ( APCodeNow =='4' ) || ( APCodeNow =='9' ) ) )
        {
            UFC::BufferedLog::Printf( " PVCID[%s] ProcessOrderEx Market Closed.", FParams.GetPVCID().c_str() );
            return FALSE;
        }
        else if( FICanTry == TRUE )
        {            
            FICanTry = FALSE;
            if( (InMessage = GenerateTryOrderEx( OI )) == NULL )
                return FALSE; ///< Timeout, Perform idle handshake.                    
            FObjectManager->IncreaceOrderCount();
        }
        else if( FBrokenRequest == TRUE  ) ///< Just reconnected.perform last execution query.
        {            
            FBrokenRequest = FALSE; ///< Turn off broken query flag.        
            OI = OrderRecodPoolPtr->GetInstance();
            OI->ClearOrderObject();
            if( (InMessage = RebuildIncompleteOrderAndGenerateBrokenQueryMessage( *OI ) ) == NULL)
                return TRUE; ///< Don't need to do last execution query.(No incomplete order in share memory.)
        }
        else  ///< Dequeue an order from Speedy. (OI from Queue)
        {    
            if( GenerateOrderMessageFromQueueEx( InMessage, OI, true ) == FALSE )                
                return FALSE; ///< Timeout, Perform idle handshake.            
            if( InMessage == NULL )                
                return TRUE; ///< Get message from queue failed, skip it.            
            if( BENCHMARK_MODE == TRUE )
                Begin = UFC::GetTickCountUS();    
        }
        ///< Send an order to exchang, and then receiveing a reply message from exchange.                
        if( (Info = RecordInfo( InMessage, *OI,IsNew, FALSE )) == NULL  && IsNew == TRUE ) ///< Record the order information.                    
        {
            UFC::AnsiString Msg;

            EndTransaction();
            Msg.Printf( "Duplicate NID[%010u]", OI->NID );        
            FObjectManager->SpeedyReject( *OI, ORDER_CHECKER_REJECT, TRUE, Msg ); 			        
            delete InMessage;        
            return TRUE;            
        }
        if( BENCHMARK_MODE == TRUE )
            RecordOrder = UFC::GetTickCountUS();

        if( FOrderFCM->OnOrder( InMessage, *OI ) == TRUE ) ///< FCMClient:: Send order to EXCHANGE.
        {      
            if( BENCHMARK_MODE == TRUE )
                SendOrder = UFC::GetTickCountUS();
            try
            {
                UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppSpeedyOrdDeQueure, UFC::AnsiString(OI->NID), OI->GetOrderID() );                            
                if( BENCHMARK_MODE == TRUE )
                {
                    Now = UFC::GetTickCountUS();
                    UFC::BufferedLog::Printf( " [ProcessTextModeOrder] Record[%d] Send[%d] Total[%d]us", RecordOrder-Begin, SendOrder-RecordOrder, Now - Begin );    
                }
                if( Info != NULL )
                    FObjectManager->OrderInfo.LogNewOrder( Info );
                TMessageType    ReplyMsgType; ///< Out variables from func OnHandleReplyMessage 
                UFC::AnsiString ExecKey,OrigKey,BrokerID; ///< Out variables from func OnHandleReplyMessage 
                BOOL            NeedLog,IsCD; ///< Out variables from func OnHandleReplyMessage 

                if( ( ReplyMsg = FOrderFCM->OnHandleReplyMessage( ReplyMsgType, NeedLog, IsCD, ExecKey,OrigKey,BrokerID )) != NULL ) ///< FCMClient:: Retrive reply from TAIFEX.
                {
                     UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OI->NID ),OI->GetOrder() );        
                     HandleExchangeReplyMessage( ReplyMsg, *OI,  0, ReplyMsgType, NeedLog, IsCD, ExecKey, OrigKey );
                     delete ReplyMsg;
                }               
                else ///< time out.
                    HandleExchangeReplyTimeout( *OI );        
            }
            catch( UFC::Exception& e )
            {
                HandleExchangeReplyTimeout( *OI, SPEEDY_ERROR_CONNECTION );
                throw X25ReconnectException( "Exchange replay confirm execution timeout" );
            }
        }
        if( InMessage != NULL )
            delete InMessage;    
        if( OI != NULL )
        {
            OI->ClearOrderObject();
            OrderRecodPoolPtr->Recycle( OI );
        }
        //        delete OI;    
        return TRUE;    
    }
    return TRUE;
}
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------
