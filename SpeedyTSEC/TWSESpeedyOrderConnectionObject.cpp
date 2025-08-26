#include "TWSESpeedyOrderConnectionObject.h"
#include "TWSESpeedyObjectsManager.h"
#include "TWSESpeedyOrderClient.h"
#include "Profiler.h"
#include "../TSECLib/LinkSubSystem.h"
#include "../TSECLib/Connection.h"
#include "../SpeedyGateway/Administrator.h"
#include "Speedy.h"

//---------------------------------------------------------------------------------------------------------------------
#ifdef __RMLib
#include "../TSECLib/USunRiskManager.h"  //RM
#include "../TSECLib/MegaRiskManager.h"  //RM
#endif
//---------------------------------------------------------------------------------------------------------------------
extern MessageObject*         MessageObj;
extern BOOL                   RecoverModeExchange;
extern BOOL                   MareketOrderGroup;
extern BOOL                   ExecToQueue;
extern Int64                  ExpiredUS;
extern Int32                  MAX_DATA_SIZE;
extern double                 ThroughputRatio;
extern BOOL                   ExtremeVersion;
extern BOOL                   UseLockLessQueue;
extern BOOL                   BENCHMARK_MODE;
extern BOOL                   DynamicSessionFIX;
//------------------------------------------------------------------------------------------------------------------
extern TWSE::ExecutionListener* gTSEConfirm;
extern TWSE::ExecutionListener* gOTCConfirm;
extern UFC::PMPMCQueue<OrderRecord>* TWSEQueue; 
extern UFC::PMPMCQueue<OrderRecord>* OTCQueue;
extern UFC::PMPMCQueue<OrderRecord>* FUTQueue; 
extern UFC::PMPMCQueue<OrderRecord>* OPTQueue;
extern UFC::PThreadObjectPool<OrderRecord>* OrderRecodPoolPtr;
//----------------------------------------------------------------------------------------------------------------------
//
//  ### FCM side ###
//  Connection object for AP code 4 Order subsystem.
//
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
SpeedyOrderConnectionObject::SpeedyOrderConnectionObject( SpeedyObjectsManager* ObjManager,
                                                          ConnectionParameter&  info,
                                                          ConnectionListener*   CListener,
                                                          UFC::ThreadListener*  TListener,
                                                          UInt32                SHMID,
                                                          int                   Group,
                                                          BOOL                  IsTWSE )
:ConnectionObjectBase( info ),
 UFC::PThread( TListener, FALSE ),
 FOrderFCM( NULL ),
 FConnectionListener( CListener ), 
 FPVCGroup( Group ),
 FReconnect( FALSE ),
 FHandshake( FALSE ),
 FEnabled( info.IsActive() ),
 FBrokenRequest( TRUE ), 
 FQueue( TRUE ), ///< share event queue.        
 FQueueData( 0, MAX_DATA_SIZE ),
 FOrigAPCode( info.GetAPCode()), ///< The original AP code of this line.
 FICanTry( FALSE ),
 FIsFIX( FALSE ), 
 FBusying( FALSE ),
 FLineBrokerID( info.GetBrokerID() ),
 FObjectManager( ObjManager ),
 FIsTWSE( IsTWSE ),
 FProcessOrder( NULL ),        
 FOutCount( 0 ),
 FFirstOrderTick( UFC::GetTickCountMS() ), 
 FNormalSystem((FIsTWSE == TRUE )? 30:93), ///< Normal
 FOddSystem((FIsTWSE == TRUE )? 40:94),    ///< Odd stock
 FOddExSystem((FIsTWSE == TRUE )? 33:83),  ///< OddEx stock
 FPostSystem((FIsTWSE == TRUE )? 32:98),   ///< Post trade        	            
 FLendSystem((FIsTWSE == TRUE )? 31:90),   ///< Lend   
 FAuctionSystem( 70 ),                     ///< Auction only for TWSE
 FTenderSystem((FIsTWSE == TRUE )?41:97),  ///< Tender   
 FTenderExSystem((FIsTWSE == TRUE )?42:89) ///< TenderEx   
{    
    ///< Attach share memory for TSEC Order Transaction   
    try
    {
        FShareMemData.Attach( SHMID, MAX_DATA_SIZE + 1 );    
        if( RecoverModeExchange == FALSE )
            EndTransaction( ); ///< Clear the dirty byte. means no incomplete transaction.    	            
    }
    catch( UFC::Exception& e )
    {
        UFC::BufferedLog::Printf( " Open share memory ID[0x%08X] for Port:%d PVC:%d failed", SHMID, info.GetLink(), info.GetPVC() );
        UFC::BufferedLog::Printf( " Reason:%s", e.what() );        
        exit(0);
    }
    
    ///< Open Order queue
    bool isQueueOpenSuccess = false;
    if( FIsTWSE == TRUE ) 
    {        
        UFC::BufferedLog::Printf( " SpeedyOrderConnectionObject() Open Order Queue for TSEC" );
        try
        {
            isQueueOpenSuccess = FQueue.Open( STOCK_ORD_QUEUE );
        }
        catch( UFC::QueueException& QueuueEx )
        {   ///< Queue fulled(Up to the system limitation), Reject to client.(As a order reject )
            isQueueOpenSuccess = false;
            UFC::BufferedLog::Printf( " SpeedyOrderConnectionObject() Open TSEC Order QueueException:[%s]", QueuueEx.what() );
        }
        if (!isQueueOpenSuccess)
        {
            UFC::BufferedLog::Printf( " SpeedyOrderConnectionObject() Open TSEC Order queue failed.");
            UFC::BufferedLog::FlushToFile();
            exit( 0 );
        }
        FLFQueue   = TWSEQueue;        
        FProfileID = UFC::psTWTSE;                
    }
    else 
    {        
        UFC::BufferedLog::Printf( " SpeedyOrderConnectionObject() Open Order Queue for OTC" );
        try
        {
            isQueueOpenSuccess = FQueue.Open( OTC_ORD_QUEUE );
        }
        catch( UFC::QueueException& QueuueEx )
        {   ///< Queue fulled(Up to the system limitation), Reject to client.(As a order reject )
            isQueueOpenSuccess = false;
            UFC::BufferedLog::Printf( " SpeedyOrderConnectionObject() Open OTC Order QueueException:[%s]", QueuueEx.what() );
        }
        if (!isQueueOpenSuccess)
        {
            UFC::BufferedLog::Printf( " SpeedyOrderConnectionObject() Open OTC Order queue failed.");
            UFC::BufferedLog::FlushToFile();
            exit( 0 );
        }
        FLFQueue   = OTCQueue;
        FProfileID = UFC::psTWOTC;
    }  
    if( FParams.GetPVC() == FIX_SESSION_PVC )
        FIsFIX = TRUE;    
    UFC::StrLCpy( FPVCID, info.GetPVCID().c_str(), 3 );
    FBrokerInt = info.GetBrokerID().SubString(0,2).ToInt();
    FSubSystem = FObjectManager->ToSubsystemID( FOrigAPCode );	
    CheckAPCodeNow();           
}
//----------------------------------------------------------------------------------------------------------------------
SpeedyOrderConnectionObject::~SpeedyOrderConnectionObject()
{
    if( IsTerminated() == FALSE )
    {
        Terminate(); ///< Terminate the thread.
        WaitFor();   ///< Wait for thread terminated.
    }
}
//---------------------------------------------------------------------------------------------------------------------
// MsgType(mtT010,mtO010,mtP010)
// FuncCode(1,2,3,4,5)
// *OrderID   OK
// *NID,      OK
// *Key,      OK  
// Host name 
//---------------------------------------------------------------------------------------------------------------------
OrderInfo* SpeedyOrderConnectionObject::AddTOPRecordInfo( Int32 Func, const char* oid, const char* brokerid, OrderRecord& OI, BOOL& IsNew, BOOL Force )
{    
    char       OID[ 8 ]; 
    char       BrokerID[ 8 ];    
    OrderInfo* OrdInfo = NULL;
    
    strncpy_zero( OID, oid, 5);
    strncpy_zero( BrokerID, brokerid, 4 );    
    switch( Func )
    {
        case 1: ///< New Order, Buy                
        case 2: ///< New Order, Sell
                IsNew = TRUE;
                OrdInfo = FObjectManager->OrderInfo.AddOrder( /*In*/OID, /*In*/BrokerID, /*In*/OI, /*In*/Force );
                break;
        case 3: ///< Reduce Qty   
        case 4: ///< Cancel Order
        case 6: ///< Replace Price
                IsNew = FALSE;
                if( FObjectManager->OrderInfo.GetOrderInfo( OID, BrokerID, OrdInfo ) == FALSE ) 
                    OrdInfo = FObjectManager->OrderInfo.AddOrder( /*In*/OID, /*In*/BrokerID, /*In*/OI, /*In*/Force );///< Not Speedy Order.
                break;
        case 5: ///< Skip order query
                IsNew = FALSE;
                break;  
    }    
    return OrdInfo;
}
//---------------------------------------------------------------------------------------------------------------------
OrderInfo* SpeedyOrderConnectionObject::AddAEVRecordInfo( Int32 Func, const char* oid, const char* brokerid, OrderRecord& OI, BOOL& IsNew, BOOL Force )
{
    char       OID[ 8 ]; 
    char       BrokerID[ 8 ];    
    OrderInfo* OrdInfo = NULL;
    
    strncpy_zero( OID, oid, 5);
    strncpy_zero( BrokerID, brokerid, 4 );    
    switch( Func )
    {
        case 1: ///< New Order
                IsNew = TRUE;
                OrdInfo = FObjectManager->OrderInfo.AddOrder( /*In*/OID, /*In*/BrokerID, /*In*/OI, /*In*/Force );///< New Order, Sell
                break;
        case 2: ///<  Cancel Order 
        case 3: ///<  Reduce Qty
                IsNew = FALSE;
                if( FObjectManager->OrderInfo.GetOrderInfo( OID, BrokerID, OrdInfo ) == FALSE ) 
                    OrdInfo = FObjectManager->OrderInfo.AddOrder( /*In*/OID, /*In*/BrokerID, /*In*/OI, /*In*/Force );///< Not Speedy Order.
                break;
        case 4: ///< Skip order status query 
                IsNew = FALSE;
                break; 
    }    
    return OrdInfo;
}
//---------------------------------------------------------------------------------------------------------------------
OrderInfo* SpeedyOrderConnectionObject::RecordInfo( MessageHeader* OrderMessage, OrderRecord& OI, BOOL& IsNew, BOOL Force )
{
    int             Func = OrderMessage->GetFunctionCode();
    OrderInfo*      OrdInfo = NULL;
    MessageT010*    T010;
    MessageO010*    O010;
    MessageO110*    O110;
    MessageP010*    P010;
    MessageA010*    A010;
    MessageE010*    E010;
    MessageEx010*   Ex010;
    MessageV010*    V010;    
    
    
    switch( OrderMessage->MessageType( ) )
    {
        case mtT010:   T010 = (MessageT010*)OrderMessage;                                              
                       if( (OrdInfo = AddTOPRecordInfo( Func, T010->OrderNo, T010->BrokerID,OI,IsNew, Force)) != NULL )
                       {
                            switch( Func )
                            {
                                 case 1: ///< New Order, Buy                
                                 case 2: ///< New Order, Sell
                                         OrdInfo->SetLastPx( T010->GetOrderPrice() );
                                         OrdInfo->SetLeavesQty( T010->GetOrderQty());
                                         break;
                                 default: break;

                            }
                       }
                       break;
        case mtO010:   O010 = (MessageO010*)OrderMessage;                       
                       if( (OrdInfo = AddTOPRecordInfo( Func, O010->OrderNo, O010->BrokerID ,OI,IsNew, Force)) != NULL )
                       {
                            switch( Func )
                            {
                                 case 1: ///< New Order, Buy                
                                 case 2: ///< New Order, Sell
                                         OrdInfo->SetLastPx( O010->GetOrderPrice() );
                                         OrdInfo->SetLeavesQty( O010->GetOrderQty());
                                         break;
                                 default: break;

                            }         
                       }   
                       break;
        case mtO110:   O110    = (MessageO110*)OrderMessage;                       
                       if( (OrdInfo = AddTOPRecordInfo( Func,O110->OrderNo,O110->BrokerID,OI,IsNew, Force)) != NULL )
                       {
                            switch( Func )
                            {
                                 case 1: ///< New Order, Buy                
                                 case 2: ///< New Order, Sell
                                         OrdInfo->SetLastPx( O110->GetOrderPrice() );
                                         OrdInfo->SetLeavesQty( O110->GetOrderQty());
                                         break;
                                 default: break;

                            }    
                       }
                       break;                       
        case mtP010:   P010    = (MessageP010*)OrderMessage;                       
                       if( (OrdInfo = AddTOPRecordInfo( Func, P010->OrderNo, P010->BrokerID,OI,IsNew, Force)) != NULL )
                       {
                            switch( Func )
                            {
                                 case 1: ///< New Order, Buy                
                                 case 2: ///< New Order, Sell
                                         OrdInfo->SetLastPx( P010->GetOrderPrice() );
                                         OrdInfo->SetLeavesQty( P010->GetOrderQty());
                                         break;
                                 default: break;

                            }
                       }
                       break;
        case mtA010:   A010     = (MessageA010*)OrderMessage;                       
                       OrdInfo = AddAEVRecordInfo( Func, A010->OrderNo, A010->BrokerID,OI,IsNew, Force);                       
                       break;
        case mtE010:   E010     = (MessageE010*)OrderMessage;                       
                       OrdInfo = AddAEVRecordInfo( Func, E010->OrderNo, E010->BrokerID,OI,IsNew, Force);
                       break;
        case mtEx010:  Ex010    = (MessageEx010*)OrderMessage;
                       OrdInfo = AddAEVRecordInfo( Func, Ex010->OrderNo, Ex010->BrokerID,OI,IsNew, Force);
                       break;
        case mtV010:   V010     = (MessageV010*)OrderMessage;                       
                       OrdInfo = AddAEVRecordInfo( Func, V010->OrderNo, V010->BrokerID,OI,IsNew, Force);
                       break;
        default:       break; ///< Not an Order message    
    }
    return OrdInfo;
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::SetAPCode( char NewAPCode )
{
    char OldAPCode = FParams.GetAPCode( );
    
    FSubSystem = FObjectManager->ToSubsystemID( NewAPCode );	
    FParams.SetAPCode( NewAPCode );    
    if( FOrderFCM != NULL )
        FOrderFCM->SetTradeSession( NewAPCode, FSubSystem );    
    if( OldAPCode != NewAPCode )
    {        
        ///< Already connected. need to reconnect.
        if( FOrderFCM != NULL && FOrderFCM->IsReady() == FALSE )
            FReconnect = TRUE;
        if( UseLockLessQueue == true )
            FLFQueue->Interrupt();
        else
            FQueue.Interrupt();
        UFC::BufferedLog::Printf( " Swap from APcode[%c] to [%c], Reconnect this socket session.", OldAPCode,  NewAPCode );            
    }    
}    
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::SetTradeSession( char NewAPCode )
{    
    char OldAPCode = FParams.GetAPCode( );    
    
    Enable( FALSE );    
    UFC::BufferedLog::Printf( " Wait 3sec to stop APCode[%c] session", OldAPCode );                            
    UFC::SleepMS( 3000 );            
    
    if( OldAPCode != NewAPCode )
    {
        UFC::BufferedLog::Printf( " Swap from APcode[%c] to [%c], Reconnect this socket session.", OldAPCode,  NewAPCode );            
        FSubSystem = FObjectManager->ToSubsystemID( NewAPCode );	
        FParams.SetAPCode( NewAPCode );        
        if( FOrderFCM != NULL )
            FOrderFCM->SetTradeSession( NewAPCode, FSubSystem );    
    }
    Enable( TRUE );    
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::SendReply( Int32 NID, 
                                             Int32 OrigNID,   
                                             const UFC::AnsiString& Key, 
                                             const UFC::AnsiString& Host, 
                                             MessageHeader* ReplyMsg,                                             
                                             BOOL  NeedLog,BOOL IsCD, 
                                             const UFC::AnsiString& ExecKey, 
                                             const UFC::AnsiString& OrigKey,
                                             const UFC::AnsiString& ErrMsg )
{
    if( NID != 0 )
    {
         MTree           Data;
         UFC::AnsiString TFXTime( ReplyMsg->GetTimeStr(), 6 );
         UFC::AnsiString X25Time;
         TMessageType    Type = ReplyMsg->MessageType();
         Int32           StatusCode = ReplyMsg->GetStatusCode();
         
         if( Type == mtT020 || Type == mtO020 || Type == mtP020 ) ///< Executions, Must check duplicate.
         {
             int Func = ReplyMsg->GetFunctionCode(); 
             if( ( Func == 1 || Func == 2 ) && FObjectManager->OrderInfo.IsExecExist( ExecKey ) == TRUE )
             {
                 UFC::BufferedLog::Printf( " Execution[%s] Duplicate,skip it", FDataStr );                      
                 return; ///< Duplicate...Skip it.
             }
         }
         TFXTime.PadThis( 9, '0' );
         UFC::GetTimeString( X25Time, FALSE );
         ReplyMsg->AsString( FDataStr );
         Data.append( "CONFIRM_ORDER", FDataStr );
         Data.append( "PVC", FParams.GetPVCID() );
         Data.append( "PBNO", FParams.GetBrokerID() );
         Data.append( "NID", NID );
         if( OrigNID != 0 && OrigNID != NID )
             Data.append( "ONID", OrigNID );
         Data.append( "TTIME", TFXTime );
         Data.append( "XTIME", X25Time );
        //UFC::BufferedLog::Printf(" SpeedyOrderConnectionObject::%s() %012d[%s]", __func__, NID, FDataStr);         
         if( IsCD == TRUE ) ///< Cancel/Delete order
         {
            if( Key.Length() > 0 )
            {
                Data.append( "CKEY", Key );
                if( NeedLog == TRUE )                 
                    FObjectManager->OrderInfo.WriteExecutionLog( TRUE, NID, Key, FDataStr, FParams.GetBrokerID(),UFC::Hostname, FParams.GetPVCID() ); ///< Write log
            }
            else if( NeedLog == TRUE )
                FObjectManager->OrderInfo.WriteExecutionLog( TRUE, NID, "null", FDataStr, FParams.GetBrokerID(),UFC::Hostname, FParams.GetPVCID() ); ///< Write log         
            if( OrigKey.Length() > 0 )
                Data.append( "KEY", OrigKey );
         }
         else
         {
            if( Key.Length() > 0 )
            {
                Data.append( "KEY", Key );
                if( NeedLog == TRUE )                 
                    FObjectManager->OrderInfo.WriteExecutionLog( TRUE, NID, Key, FDataStr, FParams.GetBrokerID(),UFC::Hostname, FParams.GetPVCID() ); ///< Write log
            }
            else if( NeedLog == TRUE )
                FObjectManager->OrderInfo.WriteExecutionLog( TRUE, NID, "null", FDataStr, FParams.GetBrokerID(),UFC::Hostname, FParams.GetPVCID() ); ///< Write log         
         }
         if( Host.Length() > 0 )
             Data.append( "HOST", Host );
         else
             Data.append( "HOST", UFC::Hostname );
         if( StatusCode != 0 )
         {
            Data.append( "REASON", StatusCode );
            if( ErrMsg.Length() > 0 )
                Data.append( "MSG", ErrMsg );
         }
         Data.append( "PHOST", UFC::Hostname );         
         if( ExtremeVersion == TRUE ) ///< Same process, direct call back OnExecution.
         {
            if( FIsTWSE == TRUE )    
            {
                if( IsCD == TRUE ) 
                    gTSEConfirm->OnExecution( FDataStr, NID, Key, OrigKey,  &Data  );
                else
                    gTSEConfirm->OnExecution( FDataStr, NID, Key, "", &Data  );
            }
            else
            {
                if( IsCD == TRUE ) 
                    gOTCConfirm->OnExecution( FDataStr, NID, Key, OrigKey, &Data  );
                else
                    gOTCConfirm->OnExecution( FDataStr, NID, Key, "", &Data  );
            }
         }
         else
         {         
            if( ExecToQueue == TRUE )
                FObjectManager->SendExecutionToQueue( 1, &Data );    
            MessageObj->Send( FObjectManager->GetConfirmSubject(), UFC::Hostname, Data  );         
         }
    }
    else
    {
        ReplyMsg->AsString( FDataStr );
        UFC::BufferedLog::Printf( " Execution[%s] with zero NID, skip it.", FDataStr ); 
    }
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::ReplyFateX020( Int32 NID, TMessageType Type, const UFC::AnsiString& Key, const UFC::AnsiString& Host, const UFC::AnsiString& Message, Int32 Code )
{
    if( NID != 0 )
    {          
         UFC::AnsiString RecvTime,X020;         
         MTree           Data;         
         
         X010ToX020( Message, Code, X020, TWSE::MessageHeader::FNewVersion, TWSE::MessageHeader::FNewExVersion );
         UFC::StrLCpy( FDataStr, X020.c_str(), 512 );
         FDataStr[ 18 ] = FPVCID[ 0 ];
         FDataStr[ 19 ] = FPVCID[ 1 ];
         UFC::GetTimeString( RecvTime, FALSE );                         
         Data.append( "CONFIRM_ORDER", FDataStr );
         Data.append( "PVC", FParams.GetPVCID() );
         Data.append( "PBNO", FParams.GetBrokerID() );
         Data.append( "NID", NID );
         Data.append( "TTIME", RecvTime );
         Data.append( "XTIME", RecvTime );
                 
         if( Key.Length() > 0 )
         {
             Data.append( "KEY", Key );
             FObjectManager->OrderInfo.WriteExecutionLog( TRUE, NID, Key, FDataStr, FParams.GetBrokerID(),UFC::Hostname, FParams.GetPVCID() ); ///< Write log                      
         }
         else
             FObjectManager->OrderInfo.WriteExecutionLog( TRUE, NID, "null", FDataStr, FParams.GetBrokerID(),UFC::Hostname, FParams.GetPVCID() ); ///< Write log                      
         if( Host.Length() > 0 )
             Data.append( "HOST", Host );
         else
             Data.append( "HOST", UFC::Hostname );
         Data.append( "PHOST", UFC::Hostname );    
         if( ExtremeVersion == TRUE ) ///< Same process, direct call back OnExecution.
         {
            if( FIsTWSE == TRUE )    
                gTSEConfirm->OnExecution( FDataStr, NID, Key, "", &Data  );
            else
                gOTCConfirm->OnExecution( FDataStr, NID, Key, "", &Data  );
         }
         else
         {         
            if( ExecToQueue == TRUE )
                FObjectManager->SendExecutionToQueue( 1, &Data );    
            MessageObj->Send( FObjectManager->GetConfirmSubject(), UFC::Hostname, Data  );         
         }         
    }	
    else
    {
        UFC::AnsiString X020;         
         
        X010ToX020( Message, Code, X020, TWSE::MessageHeader::FNewVersion, TWSE::MessageHeader::FNewExVersion );         
        UFC::BufferedLog::Printf( " Execution[%s] with zero NID, skip it.", X020.c_str() ); 
    }
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::LogTimeoutOrder( OrderRecord& OI )
{
    UFC::AnsiString ErrMsg;    
    
    ErrMsg.Printf( "OrderID:%s timeout at PVC:%s BrokerID:%s",OI.GetOrderID()/*.c_str()*/, FParams.GetPVCID().c_str(), FParams.GetBrokerID().c_str() );
    UFC::WriteErrorLog( "SpeedySolution", "Speedy" , ErrMsg );
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::GetOrderID( TMessageType Type, MessageHeader* Message, char* OrderID )
{
    switch( Type ) 
    {   
        ///< New Order
        case mtT010: UFC::StrLCpy( OrderID, ((MessageT010*)Message)->OrderNo,6 );break;///< Normal trade
        case mtO010: UFC::StrLCpy( OrderID, ((MessageO010*)Message)->OrderNo,6 );break;///< Odd trade
        case mtO110: UFC::StrLCpy( OrderID, ((MessageO110*)Message)->OrderNo,6 );break;///< OddEx trade        
        case mtP010: UFC::StrLCpy( OrderID, ((MessageP010*)Message)->OrderNo,6 );break;///< Post trade
        case mtA010: UFC::StrLCpy( OrderID, ((MessageA010*)Message)->OrderNo,6 );break;///< Aution trade
        case mtE010: UFC::StrLCpy( OrderID, ((MessageE010*)Message)->OrderNo,6 );break;///< Tender offer trade
        case mtEx010:UFC::StrLCpy( OrderID, ((MessageEx010*)Message)->OrderNo,6 );break;///< TenderEx
        case mtV010: UFC::StrLCpy( OrderID, ((MessageV010*)Message)->OrderNo,6 );break;///< Lend trade
        ///< Confirm order
        case mtT020: UFC::StrLCpy( OrderID, ((MessageT020*)Message)->OrderNo,6 );break;///< Normal trade execution
        case mtO020: UFC::StrLCpy( OrderID, ((MessageO020*)Message)->OrderNo,6 );break;///< Odd trade execution
        case mtO120: UFC::StrLCpy( OrderID, ((MessageO120*)Message)->OrderNo,6 );break;///< OddEx trade execution
        case mtP020: UFC::StrLCpy( OrderID, ((MessageP020*)Message)->OrderNo,6 );break;///< Post trade execution
        case mtA020: UFC::StrLCpy( OrderID, ((MessageA020*)Message)->OrderNo,6 );break;///< Aution trade execution
        case mtE020: UFC::StrLCpy( OrderID, ((MessageA020*)Message)->OrderNo,6 );break;///< Tender offer trade execution
        case mtEx020:UFC::StrLCpy( OrderID, ((MessageEx020*)Message)->OrderNo,6 );break;///< TenderEx execution
        case mtV020: UFC::StrLCpy( OrderID, ((MessageA020*)Message)->OrderNo,6 );break;        
        default:     UFC::StrLCpy( OrderID, "00000", 5 );break;
    }            	            	 
    OrderID[ 5 ] = 0;
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::FetchOrderFromSHM( OrderRecord& OI ){ OI.FromBinary( FALSE, (UInt8*)FShareMemData.Buffer() + 1 );}    
void SpeedyOrderConnectionObject::SaveOrderToSHM( OrderRecord& OI ){ OI.ToBinary( (UInt8*)FShareMemData.Buffer() + 1, MAX_DATA_SIZE );}
void SpeedyOrderConnectionObject::FetchOrderFromQueue( OrderRecord& OI ){OI.FromBinary( FALSE, (UInt8*)FQueueData.GetData() );}
void SpeedyOrderConnectionObject::OpenTransaction( void ){ FShareMemData[ 0 ] = 1;  }///< Add the dirty byte. 
void SpeedyOrderConnectionObject::EndTransaction( void ){ FShareMemData[ 0 ] = 0;  }///< Clearn the dirty byte.                     
BOOL SpeedyOrderConnectionObject::IsTransactionComplete( void ){ return ( FShareMemData[0] == 0 );} ///< Check the dirty byte.
//---------------------------------------------------------------------------------------------------------------------
MessageHeader* SpeedyOrderConnectionObject::BeginTransaction(  OrderRecord& OI  )
{
    OI.ProcessingStockID = 0;
    OI.IsBroken = FALSE;    
    FetchOrderFromQueue( OI );///< Fetch order from Queue data.    
    SaveOrderToSHM( OI );     ///< Store the Order to share memory.      
    OpenTransaction();        ///< Set the first byte to 1, mark it's dirty.      
    
    ///< Create order object from row data.
    MessageHeader *OrderMessage = MessageHeader::CreateMessage( OI.GetOrder(), OI.GetSize() );
    if( OrderMessage != NULL )
    {
        OI.MsgType  = OrderMessage->MessageType();
        OI.FuncCode = OrderMessage->GetFunctionCode();
        GetOrderID( (TMessageType)OI.MsgType , OrderMessage, OI.FOID );
        
        if( ( FIsTWSE == TRUE ) &&                              ///< Is TWSE  system
            ( OI.MsgType == mtT010 ) &&                        ///< Is T010 Message
            ( ( OI.FuncCode == 1 ) || ( OI.FuncCode == 2 ) ) ) ///< Is a new order
             OI.ProcessingStockID = UFC::AnsiString( ((MessageT010*)OrderMessage)->StockID, 6 ).ToInt(); ///< Get the processing Stock ID.
    }  //if( OrderMessage != NULL )
    return OrderMessage;
}  
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::AddFIXMTypeSet( int Group, UFC::PLongList& MTypes )
{ 
    if( Group == PVC_ALL_GROUP  ) ///< Group 99 , all groups
    {
        for( register int i = 0; i < FObjectManager->OrderQueueGroupCount; i ++ )        
        {
            MTypes.Add( i*10000 + FBrokerInt*100 + FNormalSystem );
            MTypes.Add( i*10000 + FBrokerInt*100 + FOddSystem );
            MTypes.Add( i*10000 + FBrokerInt*100 + FOddExSystem );
            MTypes.Add( i*10000 + FBrokerInt*100 + FPostSystem );                        
        }
    }
    else
    {
        MTypes.Add( Group*10000 + FBrokerInt*100 + FNormalSystem );
        MTypes.Add( Group*10000 + FBrokerInt*100 + FOddSystem );
        MTypes.Add( Group*10000 + FBrokerInt*100 + FOddExSystem );
        MTypes.Add( Group*10000 + FBrokerInt*100 + FPostSystem );                        
    }
}
//---------------------------------------------------------------------------------------------------------------------
//  For IPC Queue
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::MTypeSet( UFC::PLongList& MTypes )
{ 
    if( FIsFIX == TRUE )
    {       
        AddFIXMTypeSet( FPVCGroup,  MTypes );
        if( MareketOrderGroup == TRUE ) ///< Group 98
            MTypes.Add( PVC_MARKET_GROUP*10000 + FBrokerInt*100 + FNormalSystem );
        if( DynamicSessionFIX == TRUE ) ///< Special session all same group.
        {
            MTypes.Add( FBrokerInt*100 + FLendSystem );    
            MTypes.Add( FBrokerInt*100 + FAuctionSystem );    
            MTypes.Add( FBrokerInt*100 + FTenderSystem );    
            MTypes.Add( FBrokerInt*100 + FTenderExSystem );    
        }
    }
    else ///< TMP Session
    {
        if( FObjectManager->IsDynamicSession( FSubSystem ) ) 
        {
            if( FObjectManager->IsIntradayOddSession( FSubSystem ) == true )
                MTypes.Add( FPVCGroup*10000 + FBrokerInt * 100 + FSubSystem );            
            else
                MTypes.Add( FBrokerInt * 100 + FSubSystem ); ///< Dynamic session always group 0            
        }
        else
        {
            if( FPVCGroup == PVC_ALL_GROUP  )
            {
                for( register int i = 0; i < FObjectManager->OrderQueueGroupCount; i ++ )
                     MTypes.Add( i * 10000 + FBrokerInt * 100 + FSubSystem ); ///< Use SybSystem number as key.
            }    
            else
                MTypes.Add( FPVCGroup * 10000 + FBrokerInt * 100 + FSubSystem );
            if( MareketOrderGroup == TRUE && FObjectManager->NoFIXSession() == TRUE )
                MTypes.Add( PVC_MARKET_GROUP * 10000 + FBrokerInt * 100 + FSubSystem );
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------
//  For Lock less queue
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::MTypeSet2( UFC::PLongList& MTypes )
{ 
    if( FIsFIX == TRUE )
    {       
        int NormalSystem = lsNormal; ///< Normal         0       
        int OddSystem    = lsOdd;    ///< Odd stock      1  
        int OddExSystem  = lsOddEx;  ///< Intra-day Odd  8
        int PostSystem   = lsPost;   ///< Post trade     2    	            

        if( FPVCGroup == PVC_ALL_GROUP  )
        {
            for( register int i = 0; i < FObjectManager->OrderQueueGroupCount; i ++ )
            {
                MTypes.Add( i*100 + FBrokerInt + NormalSystem*10000 );
                MTypes.Add( i*100 + FBrokerInt + OddSystem*10000 );
                MTypes.Add( i*100 + FBrokerInt + OddExSystem*10000 );
                MTypes.Add( i*100 + FBrokerInt + PostSystem*10000 );
            }
        }                
        else
        {
            MTypes.Add( FPVCGroup*100 + FBrokerInt + NormalSystem*10000 );
            MTypes.Add( FPVCGroup*100 + FBrokerInt + OddSystem*10000 );
            MTypes.Add( FPVCGroup*100 + FBrokerInt + OddExSystem*10000 );
            MTypes.Add( FPVCGroup*100 + FBrokerInt + PostSystem*10000 );        
        }        
        MTypes.Add( PVC_MARKET_GROUP*100 + FBrokerInt + NormalSystem*10000  );
    }
    else ///< TMP Session
    {
        Int32 TS = TWSE::MessageHeader::GetTradingSession( FSubSystem );
        if( FPVCGroup == PVC_ALL_GROUP  )
        {
            for( register int i = 0; i < FObjectManager->OrderQueueGroupCount; i ++ )
                 MTypes.Add( i * 100 + FBrokerInt  + TS*10000 ); ///< Use SybSystem number as key.
        }    
        else
            MTypes.Add( FPVCGroup * 100 + FBrokerInt  + TS*10000 );
        if( TS == lsNormal && FObjectManager->NoFIXSession() == TRUE )
            MTypes.Add( PVC_MARKET_GROUP*100 + FBrokerInt + TS*10000  );
    }
}

//---------------------------------------------------------------------------------------------------------------------
//  Return     TRUE    TRUE     FALSE
//  Message    NULL    Obj      NULL
//             skip    Order    Idle
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::GenerateOrderMessageFromQueue( MessageHeader*& NewMessage, OrderRecord& OI )
{    
    try
    {        
        UFC::PLongList MTypes;
        
        MTypeSet( MTypes );        
        if( FQueue.Recv( FQueueData, MTypes, TWSE_IDLEHANDSHAKE_TIME_ORDER ) == TRUE )
        {
            //UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " PVC:%s get an order from queue. SubSystem:%d", FParams.GetPVCID().c_str(), FSubSystem );                
            FObjectManager->IncreaceOrderCount(); ///< Incrace the Stock PVC count.        
            if( (NewMessage = BeginTransaction( OI ) ) != NULL )        
            {
                if( FObjectManager->IsRiskManagerEnabled() && ( FObjectManager->FRMObjPtr != 0 ) )
                {
                    UFC::AnsiString checkMsg;
                    if( !FObjectManager->FRMObjPtr->CheckRiskCondition( NewMessage, OI, checkMsg ) )
                    {
                        FObjectManager->SpeedyReject( OI, SPEEDY_ERROR_NOT_ENOUGH_QTY_FOR_SELL, TRUE, checkMsg );
                        delete NewMessage;
                        EndTransaction();
                        NewMessage = NULL;
                    }
                }
//RM
#ifdef __RMLib
                if( rm::IBasicRiskManagerObject::IsRMControlSetting())
                {
                    UFC::AnsiString checkMsg;
                    if( !rm::rmObjPtr->CheckRiskCondition(NewMessage, OI, FParams.GetPVCID(), checkMsg) )
                    {
                        FObjectManager->SpeedyReject( OI, SPEEDY_ERROR_NOT_ENOUGH_QTY_FOR_SELL, TRUE, checkMsg );
                        delete NewMessage;
                        EndTransaction();
                        NewMessage = NULL;
                    }
                }                
#endif                
//RM                
                return TRUE; ///< OK! Create an order from queue.
            }
            else
            {
                UFC::BufferedLog::Printf( " ----------------------- Order Data format Error!!--------------------------------");
                UFC::BufferedLog::Printf( " PVC:%s Receive invalid message from Queue.", FParams.GetPVCID().c_str() );
                UFC::BufferedLog::Printf( " Data [%s].", OI.GetOrder() );
                UFC::BufferedLog::Printf( " Try to reconnect to Exchange." );
                UFC::BufferedLog::Printf( " ---------------------------------------------------------------------------------");
                FReconnect = TRUE;
                EndTransaction();                
                return TRUE;
            }   
        }
        else
        {
            UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " PVC:%s get order from queue timeout. SubSystem:%d", FParams.GetPVCID().c_str(), FSubSystem );
            NewMessage = NULL; ///< Timeout! perform Idle handshake with exchange.            
            return FALSE; 
        }        
    }
    catch( UFC::Exception& e )
    {
        UFC::BufferedLog::Printf( " PVC:%s FQueue.Recv Exception:%s", FParams.GetPVCID().c_str(), e.what() );
        UFC::BufferedLog::Printf( " MType:%d Len:%d", FQueueData.GetMType(), FQueueData.GetSize() );
        UFC::BufferedLog::FlushToFile();
        FReconnect = TRUE;
        EndTransaction();
        NewMessage = NULL;
        return TRUE;
    }
    return TRUE;
}
//---------------------------------------------------------------------------------------------------------------------
/// Only Try order need to keep in share memory
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::GenerateOrderMessageFromQueueEx( MessageHeader*& NewMessage, OrderRecord*& OI, bool KeepOrder )
{    
    try
    {        
        UFC::PLongList MTypes;
        
        MTypeSet2( MTypes );
        if( FLFQueue->pop( MTypes, &OI , TWSE_IDLEHANDSHAKE_TIME_ORDER ) == true )
        {
            FObjectManager->IncreaceOrderCount(); ///< Incrace the Stock PVC count.        
            if( (NewMessage = BeginTransactionEx( OI, KeepOrder ) ) != NULL )        
            {           
                return TRUE; ///< OK! Create an order from queue.
            }
            else
            {
                UFC::BufferedLog::Printf( " ----------------------- Order Data format Error!!--------------------------------");
                UFC::BufferedLog::Printf( " PVC:%s Receive invalid message from Queue.", FParams.GetPVCID().c_str() );
                UFC::BufferedLog::Printf( " Data [%s].", OI->GetOrder() );
                UFC::BufferedLog::Printf( " Try to reconnect to Exchange." );
                UFC::BufferedLog::Printf( " ---------------------------------------------------------------------------------");
                FReconnect = TRUE;
                EndTransaction();
                return TRUE;
            }   
        }
        else
        {
            //UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " PVC:%s get order from queue timeout. SubSystem:%d", FParams.GetPVCID().c_str(), FSubSystem );
            NewMessage = NULL; ///< Timeout! perform Idle handshake with exchange.
            return FALSE; 
        }        
    }
    catch( UFC::Exception& e )
    {
        UFC::BufferedLog::Printf( " PVC:%s FQueue.Recv Exception:%s", FParams.GetPVCID().c_str(), e.what() );
        UFC::BufferedLog::Printf( " MType:%d Len:%d", FQueueData.GetMType(), FQueueData.GetSize() );
        UFC::BufferedLog::FlushToFile();
        FReconnect = TRUE;
        EndTransaction();
        NewMessage = NULL;
        return TRUE;
    }
    return TRUE;
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::HandleExchangeReplyMessage( MessageHeader* ReplyMsg, 
                                                              OrderRecord& OI,  
                                                              Int32 OrigNID,  
                                                              TMessageType ReplyMsgType,
                                                              BOOL NeedLog,
                                                              BOOL IsCD, 
                                                              const UFC::AnsiString& ExecKey, 
                                                              const UFC::AnsiString& OrigKey, 
                                                              const UFC::AnsiString& ErrMsg )
{
    EndTransaction( );  ///< Clearn the dirty byte. when Exchange reply any message
    if( OI.MsgType == mtT010 || OI.MsgType == mtO010 || OI.MsgType == mtP010 || OI.MsgType == mtO110 ||
        OI.MsgType == mtA010 || OI.MsgType == mtE010 || OI.MsgType == mtEx010 ||OI.MsgType == mtV010) ///< Is an order message
    {
        //UFC::BufferedLog::Printf(" SpeedyOrderConnectionObject::%s() Rpt MsgType[%d] StatusCode[%s]", __func__, (int)ReplyMsgType, ReplyMsg->GetStatusCodeStr());
        if( ReplyMsgType == mtT020 || ReplyMsgType == mtO020 || ReplyMsgType == mtP020  || ReplyMsgType == mtO120 ||
            ReplyMsgType == mtA020 || ReplyMsgType == mtE020 || ReplyMsgType == mtEx020 || ReplyMsgType == mtV020 ) ///< Exchange reply execution
        {
            int functionCode = OI.FuncCode;
            char ReplyOID[8];
            
            if( ( ( functionCode == 1 ) || ( functionCode == 2 ) ) && ( OI.MsgType == mtT010 ) && ( FIsTWSE == TRUE ) ) ///< TSE New order
                FObjectManager->DecreaseLockPVC( OI.ProcessingStockID );
            GetOrderID( ReplyMsgType, ReplyMsg, ReplyOID );
            //UFC::BufferedLog::Printf(" SpeedyOrderConnectionObject::%s() RptOID[%s] RecOID[%s]", __func__, ReplyOID, OI.GetOrderID());
            if( strcmp( ReplyOID, OI.GetOrderID() ) == 0 )            ///< OID matched!
            {
//RM
                if( FObjectManager->IsRiskManagerEnabled() && ( FObjectManager->FRMObjPtr != 0 ) &&
                    ( ( ReplyMsgType == mtT020 ) || ( ReplyMsgType == mtO020 )  || ( ReplyMsgType == mtO120 ) || ( ReplyMsgType == mtP020 ) ) )
                {
                    if( ( functionCode == 1 ) || ( functionCode == 2 ) )
                    {
                        int msgStatusCode = ReplyMsg->GetStatusCode();
                        if ((msgStatusCode !=  0) && (msgStatusCode != 31) && 
                            (msgStatusCode != 48) && (msgStatusCode != 49) &&
                            (msgStatusCode != 51) && (msgStatusCode != 52))
                            FObjectManager->FRMObjPtr->ProcessRiskRejectMessage( ReplyMsg, OI );
                        else
                            FObjectManager->FRMObjPtr->ProcessRiskBuySellMessage( ReplyMsg, OI );
                    }
                    else if( ( functionCode == 3 ) || ( functionCode == 4 ) )
                        FObjectManager->FRMObjPtr->ProcessRiskCancelReplaceMessage( ReplyMsg, OI );
                }
#ifdef __RMLib
                if( rm::IBasicRiskManagerObject::IsRMControlSetting() && 
                    ( ( ReplyMsgType == mtT020 ) || ( ReplyMsgType == mtO020 ) || ( ReplyMsgType == mtP020 ) ) )
                    rm::rmObjPtr->ProcessRiskCancelReplaceMessage(ReplyMsg, OI, FParams.GetPVCID());
#endif
                if( OI.IsBroken == TRUE )
                    UFC::BufferedLog::Printf( " PVC:%s Last execution recovered. OrderID:%s", FParams.GetPVCID().c_str(), ReplyOID/*.c_str()*/ );                        
                SendReply( OI.NID, OrigNID, OI.Key, OI.Hostname, ReplyMsg, NeedLog,IsCD, ExecKey, OrigKey, ErrMsg ); ///< Send correct reply message.
            }
            else
            {
//RM
                if( FObjectManager->IsRiskManagerEnabled() && ( FObjectManager->FRMObjPtr != 0 ) &&
                    ( ( ReplyMsgType == mtT020 ) || ( ReplyMsgType == mtO020 )  || ( ReplyMsgType == mtO120 ) || ( ReplyMsgType == mtP020 ) ) )
                    FObjectManager->FRMObjPtr->ProcessRiskRejectMessage( ReplyMsg, OI);
#ifdef __RMLib
                if( rm::IBasicRiskManagerObject::IsRMControlSetting() && 
                    ( ( OI.MsgType == mtT010 ) || ( OI.MsgType == mtO010 ) || ( OI.MsgType == mtP010 ) ) )
                    rm::rmObjPtr->ProcessRiskRejectMessage(ReplyMsg, OI, FParams.GetPVCID());
#endif
//RM
                UFC::BufferedLog::Printf( " PVC:%s Expected OrderID:%s but received %s.", FParams.GetPVCID().c_str(), OI.GetOrderID()/*.c_str()*/,ReplyOID/*.c_str()*/ );
                memcpy( OI.GetOrder() + 6 , ReplyMsg->GetTimeStr(), 6 );
                ReplyFateX020( OI.NID, (TMessageType)OI.MsgType, OI.Key, OI.Hostname, OI.GetOrder(), SPEEDY_ERROR_ORDERID_MISS ); ///< Return X020 with OID missmatch.
            }
        }
        else if( ReplyMsgType == mtT030 || ReplyMsgType == mtO030 || ReplyMsgType == mtP030  || ReplyMsgType == mtO130 ||
                 ReplyMsgType == mtA030 || ReplyMsgType == mtE030 || ReplyMsgType == mtEx030 || ReplyMsgType == mtV030 ) ///< Exchange reply error        	
        {
//RM
            if( FObjectManager->IsRiskManagerEnabled() && ( FObjectManager->FRMObjPtr != 0 ) &&
                ( ( ReplyMsgType == mtT030 ) || ( ReplyMsgType == mtO130 ) ) )
                FObjectManager->FRMObjPtr->ProcessRiskRejectMessage( ReplyMsg, OI);
#ifdef __RMLib
            if( rm::IBasicRiskManagerObject::IsRMControlSetting() && ( ReplyMsgType == mtT030 ) ) rm::rmObjPtr->ProcessRiskRejectMessage(ReplyMsg, OI, FParams.GetPVCID());
#endif            
//RM            
            memcpy( OI.GetOrder() + 6 , ReplyMsg->GetTimeStr(), 6 );
            ReplyFateX020( OI.NID, (TMessageType)OI.MsgType, OI.Key, OI.Hostname, OI.GetOrder(), ReplyMsg->GetStatusCode() ); ///< Return X020 with error code.        	
            if( ReplyMsg->GetStatusCode() == 1 )///< Market closed.
            {                 
                MarketSchedule* ThisMarket = FObjectManager->GetScheduleObjectBySubsystemID( FSubSystem );
                if( ThisMarket != NULL ) 
                    ThisMarket->CloseMarket( );
            }    
        }
    }
    else
    {
        UFC::BufferedLog::Printf( " OI.MsgType Not Order" );
        SendReply( OI.NID, OrigNID, OI.Key, OI.Hostname, ReplyMsg, NeedLog, IsCD, ExecKey, OrigKey, ErrMsg );
    }
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::HandleExchangeReplyMessage(   MessageHeader* ReplyMsg, 
                                                                OrderInfo* OI,
                                                                TMessageType ReplyMsgType, 
                                                                BOOL NeedLog,
                                                                BOOL IsCD, 
                                                                const UFC::AnsiString& ExecKey, 
                                                                const UFC::AnsiString& OrigKey, 
                                                                const UFC::AnsiString& ErrMsg )
{
    TMessageType MsgType = (TMessageType)OI->GetMsgType(); 
    EndTransaction( );  ///< Clearn the dirty byte. when Exchange reply any message
    if( MsgType == mtT010 || MsgType == mtO010 || MsgType == mtP010 || MsgType == mtO110 ||
        MsgType == mtA010 || MsgType == mtE010 || MsgType == mtEx010 ||MsgType == mtV010) ///< Is an order message
    {        	
        if( ReplyMsgType == mtT020 || ReplyMsgType == mtO020 || ReplyMsgType == mtP020  || ReplyMsgType == mtO120 ||
            ReplyMsgType == mtA020 || ReplyMsgType == mtE020 || ReplyMsgType == mtEx020 || ReplyMsgType == mtV020 ) ///< Exchange reply execution
        {
            char ReplyOID[8];

            
            GetOrderID( ReplyMsgType, ReplyMsg, ReplyOID );
            if( strcmp( ReplyOID, OI->GetOrderID() ) == 0 )            ///< OID matched!
                SendReply( OI->GetNID(), 0, OI->GetKeyString(), OI->GetHostname(), ReplyMsg, NeedLog,IsCD, ExecKey, OrigKey, ErrMsg ); ///< Send correct reply message.
            else
                UFC::BufferedLog::Printf( " PVC:%s Expected OrderID:%s but received %s.",FParams.GetPVCID().c_str(), OI->GetOrderID()/*.c_str()*/,ReplyOID/*.c_str()*/ );
        }
        if( ReplyMsg->GetStatusCode() == 1 )///< Market closed.
        {                 
            MarketSchedule* ThisMarket = FObjectManager->GetScheduleObjectBySubsystemID( FSubSystem );
            if( ThisMarket != NULL ) 
                ThisMarket->CloseMarket( );
        }    
    }
    else
    {
        UFC::BufferedLog::Printf( " OI.MsgType Not Order" );
        SendReply( OI->GetNID(), 0, OI->GetKeyString(), OI->GetHostname(), ReplyMsg, NeedLog, IsCD, ExecKey, OrigKey, ErrMsg );
    }
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::HandleExchangeReplyTimeout( OrderRecord& OI, int ErrorCode )
{
    if( OI.MsgType == mtT010 || OI.MsgType == mtO010 || OI.MsgType == mtP010 )   
    {
        if( OI.ProcessingStockID != 0 )///< TSE New order, Add rush stock reference.
        {
            FObjectManager->IncreaseLockPVC( OI.ProcessingStockID );///< Rush Stock!!
            if( FObjectManager->IsStockRush( OI.ProcessingStockID ) == TRUE ) ///< Rush Stock!!
            {
                UFC::AnsiString ErrMsg;
                ErrMsg.Printf( "Half of PVC in this server processing the same Stock:%d ", OI.ProcessingStockID );
                UFC::WriteErrorLog( "SpeedySolution", "Speedy" , ErrMsg );                  
            }
        }        
        ReplyFateX020( OI.NID, (TMessageType)OI.MsgType, OI.Key, OI.Hostname, OI.GetOrder(), ErrorCode );///< Exchange time out!!!
        LogTimeoutOrder( OI ); 
    }
    FReconnect = TRUE;
}
//---------------------------------------------------------------------------------------------------------------------
MessageHeader* SpeedyOrderConnectionObject::GenerateTryOrder( OrderRecord& OI )
{    
    MessageHeader *TryMsg = NULL;    
    
    if( IsTransactionComplete() == FALSE ) ///< A trying message exists.
    {        	    
        
        if( ( TryMsg = RollbackTransaction( OI ) ) != NULL ) ///< Rebuild the last order.
        { 
            if( TryMsg->GetSubSystem() == FSubSystem )
                UFC::BufferedLog::DebugPrintf( UFC::dlPerformance, " Get a try Order[%s] from share memory.", OI.GetOrder() );
            else
            {
                delete TryMsg;
                EndTransaction();        
                UFC::BufferedLog::Printf( " It's a previous session, Order:[%s]", OI.GetOrder() );
                TryMsg = NULL;
            }
        }
        else
        {
            UFC::BufferedLog::Printf( " Invalid Last try Order:[%s]", OI.GetOrder() );
            EndTransaction();        
        }  
    }  
    else
    {
        try
        {
            UFC::PLongList MTypes;
        
            MTypeSet( MTypes );        
            if( FQueue.Recv( FQueueData, MTypes, TWSE_IDLEHANDSHAKE_TIME_ORDER ) == TRUE )
            {
                UFC::BufferedLog::DebugPrintf( UFC::dlPerformance, " PVC:%s get a try order from queue. SubSystem:%d", FParams.GetPVCID().c_str(), FSubSystem );
                if( ( TryMsg = BeginTransaction( OI ) ) == NULL )
                {
                    UFC::BufferedLog::Printf( " ----------------------- Order Data format Error!!--------------------------------");
                    UFC::BufferedLog::Printf( " PVC:%s Receive invalid message from Queue.", FParams.GetPVCID().c_str() );
                    UFC::BufferedLog::Printf( " Data [%s].", OI.GetOrder() );
                    UFC::BufferedLog::Printf( " Try to reconnect to Exchange." );
                    UFC::BufferedLog::Printf( " ---------------------------------------------------------------------------------");                    
                    FReconnect = TRUE;
                    EndTransaction();
                }  
            }
            else  ///< Timeout! perform Idle handshake with exchange.
                UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " PVC:%s get try order from queue timeout. SubSystem:%d", FParams.GetPVCID().c_str(), FSubSystem );
        }
        catch( UFC::Exception& e )
        {
            UFC::BufferedLog::Printf( " PVC:%s FQueue.Recv Exception:%s", FParams.GetPVCID().c_str(), e.what() );
            UFC::BufferedLog::Printf( " MType:%d Len:%d", FQueueData.GetMType(), FQueueData.GetSize() );
            UFC::BufferedLog::FlushToFile();
            FReconnect = TRUE;
            EndTransaction();        
            TryMsg = NULL;
        }
    }
    
    if( ( TryMsg != NULL ) && FObjectManager->IsRiskManagerEnabled() && ( FObjectManager->FRMObjPtr != 0 ) )
    {
        UFC::AnsiString checkMsg;
        if( !FObjectManager->FRMObjPtr->CheckRiskCondition( TryMsg, OI, checkMsg ) )
        {
            FObjectManager->SpeedyReject( OI, SPEEDY_ERROR_NOT_ENOUGH_QTY_FOR_SELL, TRUE, checkMsg );
            delete TryMsg;
            TryMsg = NULL;
            EndTransaction();
        }
    }  //if( ( TryMsg != NULL ) && FObjectManager->IsRiskManagerEnabled() && ( FObjectManager->FRMObjPtr != 0 ) )
//RM
#ifdef __RMLib
    if(  TryMsg != NULL && rm::IBasicRiskManagerObject::IsRMControlSetting() )
    {
        UFC::AnsiString checkMsg;
        if( !rm::rmObjPtr->CheckRiskCondition(TryMsg, OI, FParams.GetPVCID(), checkMsg) )
        {
            FObjectManager->SpeedyReject( OI, SPEEDY_ERROR_NOT_ENOUGH_QTY_FOR_SELL, TRUE, checkMsg );
            delete TryMsg;
            TryMsg = NULL;
            EndTransaction();
        }
    } 
#endif
//RM
    if( TryMsg != NULL )    
        SetTrying( TRUE );
    return TryMsg;
}  //SpeedyOrderConnectionObject::GenerateTryOrder()
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::FinishTry( void  )
{
    FICanTry = FALSE;  ///< Finished try order.  
    EndTransaction();  ///< Clearn the dirty byte.
    FObjectManager->IncreaceOrderCount(); ///< Incrace the Stock PVC count.
    if( FObjectManager->GetMarketStatus( FParams.GetAPCode() ) != msOpened )
    {
        FObjectManager->SetMarketStatus( FParams.GetAPCode(), msOpened );///< Try Success!! Change market status to Opened!!
        UFC::BufferedLog::Printf( " PVC:%s Bingo! Market is opening.", FParams.GetPVCID().c_str() );                          
    }
    else
        UFC::BufferedLog::Printf( " PVC:%s Stop trying. Another PVC open the market.", FParams.GetPVCID().c_str() );     
    FOrderFCM->SetStatus( PVC_STATUS_ONLINE_BUSY );
    FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_UPDATE_STATE, NULL );                    
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::HandleExchangeReplyTryMessage( MessageHeader* ReplyMsg, OrderRecord& OI,TMessageType Type,BOOL NeedLog,BOOL IsCD,const UFC::AnsiString& ExecKey, const UFC::AnsiString& OrigKey/*, UFC::AnsiString&  BrokerID*/  )
{
    TMessageType ReplyMsgType = ReplyMsg->MessageType( );                                
    
    
    if( ReplyMsgType == mtT020 || ReplyMsgType == mtO020 || ReplyMsgType == mtP020  ) ///< Exchange reply execution
    {
        if( ReplyMsg->GetStatusCode() == 2 ) ///< Market not open.
        {
            ///< Market not open yet.
            UFC::SleepMS( FObjectManager->RetryInterval( FParams.GetAPCode() ) );
        }
        else
        {
            FinishTry( );
            int functionCode = OI.FuncCode;
//RM
            if( FObjectManager->IsRiskManagerEnabled() && ( FObjectManager->FRMObjPtr != 0 ) &&
                ( ( ReplyMsgType == mtT020 ) || ( ReplyMsgType == mtO020 ) || ( ReplyMsgType == mtP020 ) ) )
            {
                if( ( functionCode == 1 ) || ( functionCode == 2 ) )
                {
                    int msgStatusCode = ReplyMsg->GetStatusCode();
                    if ((msgStatusCode !=  0) && (msgStatusCode != 31) && 
                        (msgStatusCode != 48) && (msgStatusCode != 49) &&
                        (msgStatusCode != 51) && (msgStatusCode != 52))
                        FObjectManager->FRMObjPtr->ProcessRiskRejectMessage( ReplyMsg, OI );
                    else
                        FObjectManager->FRMObjPtr->ProcessRiskBuySellMessage( ReplyMsg, OI );
                }
                else if( ( functionCode == 3 ) || ( functionCode == 4 ) )
                    FObjectManager->FRMObjPtr->ProcessRiskCancelReplaceMessage( ReplyMsg, OI );
            }
#ifdef __RMLib
        if( rm::IBasicRiskManagerObject::IsRMControlSetting() && 
            ( ( ReplyMsgType == mtT020 ) || ( ReplyMsgType == mtO020 ) || ( ReplyMsgType == mtP020 ) ) )
            rm::rmObjPtr->ProcessRiskCancelReplaceMessage(ReplyMsg, OI, FParams.GetPVCID());
#endif
            SendReply( OI.NID, 0, OI.Key, OI.Hostname, ReplyMsg, NeedLog, IsCD, ExecKey, OrigKey ); ///< Send correct reply message.        	
        }
    }
    else if(  ReplyMsgType == mtO120 || ReplyMsgType == mtA020 || ReplyMsgType == mtE020 || ReplyMsgType == mtEx020 || ReplyMsgType == mtV020 ) ///< Exchange reply execution
    {        
        FinishTry( );
        int functionCode = OI.FuncCode;
//RM
        if( FObjectManager->IsRiskManagerEnabled() && ( FObjectManager->FRMObjPtr != 0 ) && ( ReplyMsgType == mtO120 ) )
        {
            if( ( functionCode == 1 ) || ( functionCode == 2 ) )
            {
                int msgStatusCode = ReplyMsg->GetStatusCode();
                if ((msgStatusCode !=  0) && (msgStatusCode != 31) && 
                    (msgStatusCode != 48) && (msgStatusCode != 49) &&
                    (msgStatusCode != 51) && (msgStatusCode != 52))
                    FObjectManager->FRMObjPtr->ProcessRiskRejectMessage( ReplyMsg, OI );
                else
                    FObjectManager->FRMObjPtr->ProcessRiskBuySellMessage( ReplyMsg, OI );
            }
            else if( ( functionCode == 3 ) || ( functionCode == 4 ) )
                FObjectManager->FRMObjPtr->ProcessRiskCancelReplaceMessage( ReplyMsg, OI );
        }
//RM
        SendReply( OI.NID, 0, OI.Key, OI.Hostname, ReplyMsg, NeedLog, IsCD, ExecKey, OrigKey ); ///< Send correct reply message.        	
    }
    else if( ReplyMsgType == mtT030 || ReplyMsgType == mtO030 || ReplyMsgType == mtP030  || ReplyMsgType == mtO130 ||
             ReplyMsgType == mtA030 || ReplyMsgType == mtE030 || ReplyMsgType == mtEx030 || ReplyMsgType == mtV030   ) ///< Exchange reply error        	
    {
//RM
        if( FObjectManager->IsRiskManagerEnabled() && ( FObjectManager->FRMObjPtr != 0 ) &&
            ( ( ReplyMsgType == mtT030 ) || ( ReplyMsgType == mtO130 ) ) )
            FObjectManager->FRMObjPtr->ProcessRiskRejectMessage( ReplyMsg, OI );
#ifdef __RMLib
        if( rm::IBasicRiskManagerObject::IsRMControlSetting() && ( ReplyMsgType == mtT030 ) ) rm::rmObjPtr->ProcessRiskRejectMessage( ReplyMsg, OI, FParams.GetPVCID() );
#endif
        if( ReplyMsg->GetStatusCode() != 2 ) ///< Market not open yet.
        {
            FinishTry( );
            memcpy( OI.GetOrder() + 6 , ReplyMsg->GetTimeStr(), 6 );
            ReplyFateX020( OI.NID, (TMessageType)OI.MsgType, OI.Key, OI.Hostname, OI.GetOrder(), ReplyMsg->GetStatusCode() ); ///< Return X020 with error code.        		    
        }
        else
            UFC::SleepMS( FObjectManager->RetryInterval( FParams.GetAPCode() ) );
    }
    SetTrying( FALSE );    
    FTryEvent.SetEvent();
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::WaitMarketOpen( )
{
    FOrderFCM->SetStatus( PVC_STATUS_WAIT_OPEN );
    FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_WAIT_OPEN, NULL );
    UFC::BufferedLog::Printf( " PVC:%s Wait for market opened", FParams.GetPVCID().c_str() );                                  
    for( int i = 0; i < TWSE_IDLEHANDSHAKE_TIME_ORDER; i++ )
    {
        for( int i = 0; i < 20; i++ )
        {
            if( FEnabled == FALSE || FReconnect == TRUE )
                return TRUE;
            if( FObjectManager->GetMarketStatus( FParams.GetAPCode() ) == msOpened )
            {
                UFC::BufferedLog::Printf( " PVC:%s wakeup! Market is opening.", FParams.GetPVCID().c_str() );                      
                FOrderFCM->SetStatus( PVC_STATUS_ONLINE_BUSY );
                FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_UPDATE_STATE, NULL );                
                return TRUE;
            }		
            UFC::SleepMS( 50 );
        }	
        FOrderFCM->SetStatus( PVC_STATUS_WAIT_OPEN );
    }
    return FALSE;///< Timeout
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::WaitTry( void )
{    
    for( int i = 0; i < TWSE_IDLEHANDSHAKE_TIME_ORDER; i++ )
    {
        for( int i = 0; i < 20; i++ )
        {
            TMarketStatus SessionStatus = FObjectManager->GetMarketStatus( FParams.GetAPCode() );
            if( FEnabled == FALSE || FReconnect == TRUE)
                return TRUE;
            if( SessionStatus == msTryOrder ) ///< Process running, whaiting for try order.
            {
                return TRUE;
            }
            else if( SessionStatus == msOpened ) ///< Market opened.
            {
                FICanTry = FALSE;
                UFC::BufferedLog::Printf( " PVC:%s give up try order. Market already opened.", FParams.GetPVCID().c_str() );
                FOrderFCM->SetStatus( PVC_STATUS_ONLINE_BUSY );
                FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_UPDATE_STATE, NULL );                
                return TRUE;
            }
            UFC::SleepMS( 50 );
        }
        FOrderFCM->SetStatus( PVC_STATUS_WAIT_TRY );    
        FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_UPDATE_STATE, NULL );                        
    }       
    return FALSE; ///< Timeout
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::IsMarketClosed()
{
    if( FIsFIX == FALSE  ) ///< Check TMP session only
    {
        char             APCodeNow = FParams.GetAPCode();
        TMarketStatus    SessionStatus = FObjectManager->GetMarketStatus( APCodeNow );       
        
        if( SessionStatus >= msClosed &&  ( ( APCodeNow == '0' ) || ( APCodeNow =='4' ) || ( APCodeNow =='9' ) ) )
            return TRUE;             
    }
    return FALSE;
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::OpenMarket()
{
    FOrderFCM->OnOpenMarket();///< FCMClient:: Open market, and finished handshake
    FReconnect = FALSE;
    FBrokenRequest = TRUE;
    
    while( TRUE )
    {	
        try
        {	
             if( FEnabled == FALSE ) ///< User disable this PVC.
             {                 
                UFC::BufferedLog::Printf( " PVCID[%s], User disable this session", FParams.GetPVCID().c_str() );    
                UFC::SleepMS( 1000 );
                if( FFirstTimeDisable == TRUE )
                {
                    FFirstTimeDisable  = FALSE;		      
                    FReconnect = TRUE;
                    return FALSE;
                }
             }
             else if( FReconnect == TRUE ) ///< Timeout or reconnect
             {
                UFC::BufferedLog::Printf( " PVCID[%s], FReconnect = TRUE, Try to reconnect", FParams.GetPVCID().c_str() );                      
                FReconnect = FALSE;
                CheckAPCodeNow();
                FOrderFCM->Handshake( ); ///< FCMClient:: Rehandshake with TAIFEX.
                FBrokenRequest = TRUE;                
             }
             else if( FHandshake == TRUE ) ///< Receive a L010...go back to handshake.
             {
                FHandshake = FALSE;
                CheckAPCodeNow();
                FOrderFCM->Handshake( TRUE ); ///< FCMClient:: Rehandshake with TAIFEX.
                FBrokenRequest = TRUE;                
             }	     
             else if( FProcessOrder != NULL )
             {                 
                 if( (this->*FProcessOrder)( ) == FALSE ) ///< Process Order    
                 {                     
                    if( IsMarketClosed() == TRUE )
                    {
                        UFC::BufferedLog::Printf( " PVCID[%s], Market Closed.", FParams.GetPVCID().c_str() );                      
                        FReconnect = TRUE;
                        FBrokenRequest = TRUE;
                        return FALSE;
                    }          
                    FOrderFCM->OnIdle();                 ///< Perform the idle handshake.                                   
                 }
             }
        }
        catch( X25RehandshakeException& e) // Receive a L010...go back to handshake.
        {
            FReconnect = FALSE;
            CheckAPCodeNow();
            FOrderFCM->Handshake( TRUE );
            FBrokenRequest = TRUE;
        }
        catch( X25ReconnectException& e)  // Timeout or reconnect
        {
            FReconnect = FALSE;
            CheckAPCodeNow();
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
        case COMMAND_GET_STATE:   if( FOrderFCM != NULL )
                                      *((int*)Data ) = FOrderFCM->GetStatus();
                                  else
                                      *((int*)Data ) = PVC_STATUS_BROKEN;
                                  break;
        case COMMAND_GET_IN_SEQ:  if( FOrderFCM != NULL )                                
                                      *((int*)Data ) = FOrderFCM->GetSequence( TRUE );  
                                  else      
                                      *((int*)Data ) = 1;
                                  break;      
        case COMMAND_GET_OUT_SEQ: if( FOrderFCM != NULL )                                
                                      *((int*)Data ) = FOrderFCM->GetSequence( FALSE );  
                                  else      
                                      *((int*)Data ) = 1;
                                  break;      
        case EVENT_PVC_READ:
        case EVENT_PVC_WRITE:     FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_READ_WRITE, Data );
                                  break;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::SetFIXSeq( int InSeq, int OutSeq )
{
    if( FOrderFCM != NULL && FOrderFCM->IsFIX() == TRUE )    
        FOrderFCM->SetSequence( InSeq, OutSeq );       
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
            UFC::BufferedLog::Printf( " --- SpeedyOrderConnectionObject::Enable( FALSE ) PVC:%s.", FParams.GetPVCID().c_str() );                              
            FFirstTimeDisable = TRUE;
            FOrderFCM->SetStatus( PVC_STATUS_DISABLEING );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_UPDATE_STATE, NULL );
            FOrderFCM->StopHandshake();
        }
        else
        {
            FOrderFCM->SetStatus( PVC_STATUS_ENABLE );
            //FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_ENABLED, NULL );
        }
        FEnabled = IsEnabled;
        if( FEnabled == FALSE )
        {    
            FQueue.Interrupt();
            UFC::BufferedLog::Printf( " --- FQueue.Interrupt( ) PVC:%s.", FParams.GetPVCID().c_str() );
            UFC::BufferedLog::FlushToFile();
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderConnectionObject::IsEnable( void )
{
    return FEnabled;
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::DisableLoop( void )
{    
    if( FEnabled == FALSE )
    {        
        FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_DISABLED, NULL );                
        while( FEnabled == FALSE ) ///< Disable this connection.
        {
            if( FOrderFCM->IsReady() == true )
                FOrderFCM->Close();
            UFC::SleepMS( 1000 );            
            FOrderFCM->SetStatus( PVC_STATUS_DISABLE);
            if( FFirstTimeDisable  == TRUE )
            {
                FFirstTimeDisable  = FALSE;
                FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_DISABLED, NULL );                
            }
        }        
        FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_PVC_BROKEN, NULL );
    }
}
//----------------------------------------------------------------------------------------------------------------------
const char* StatusStr( TMarketStatus Status )
{
    switch( Status )
    {
        case msWait: return "Wait";
        case msTryOrder: return "Try";
        case msOpened:   return "Opened";
        case msClosed:   return "Closed";
        case msStop:     return "Stopped";    
    }
    return "---";
}        
//----------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::SessionWaitingLoop( char APCode )
{
    if( FIsFIX == FALSE )    
    {
        TMarketStatus Status = FObjectManager->GetMarketStatus( APCode );

        if( Status == msStop || Status == msClosed  ) ///< Wait forever !!
        {
            FOrderFCM->SetStatus( PVC_STATUS_SLEEPING );
            FConnectionListener->OnConnectionNotify( this, CONNECTION_PVC_SLEEPING, NULL );	
            while( TRUE )
            {
                UFC::SleepMS( 500 );	
                ///< Session status reset or user disable this PVC
                if( FObjectManager->GetMarketStatus( APCode ) < msClosed || FEnabled == FALSE )
                    return;
            }	    
        }    
    }
}
//----------------------------------------------------------------------------------------------------------------------
// Handles AP code 0,4,9 only...
// Other trading session don't need "Try order"
//----------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::WaitingLoop( void )
{
    if( FOrigAPCode == '0')      ///< Pure Normal trade session line.
        SessionWaitingLoop('0');
    else if( FOrigAPCode == '4') ///< Odd trade line. In morning trading session, It should be a Normal trading line
        SessionWaitingLoop('4'); ///< In the afternoon trading session, It should be a Odd trading line.
    else if( FOrigAPCode == '9') ///< Post trade line. In morning trading session, It should be a Normal trading line    
        SessionWaitingLoop('9'); ///< In the afternoon trading session, It should be a Post trading line.	
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::CheckAPCodeNow( void )
{
    char APCodeMow = FParams.GetAPCode( );
    TMarketStatus State;
    
    if( APCodeMow == '5' || APCodeMow == '6' || APCodeMow == '7' || APCodeMow == '8'  )///< Switch to Auction/NegotiatePx,Tender,TenderEx,Lend session.
        return;
    if(  FOrigAPCode == '0' )    ///< Pure Normal trade session line
    {
        SetAPCode( '0' );
    }
    else if( FOrigAPCode == '4') ///< Odd trade line. In morning trading session, It should be a Normal trading line
    {                            ///< In the afternoon trading session, It should be a Odd trading line.
        State = FObjectManager->GetMarketStatus( '0' );
        if( State == msStop || State == msClosed ) ///< Normal market closed. Switch to Odd trading session.
            SetAPCode( '4' );
        else
            SetAPCode( '0' );
    }    
    else if( FOrigAPCode == '9') ///< Post trade line. In morning trading session, It should be a Normal trading line
    {                            ///< In the afternoon trading session, It should be a Post trading line.	
        State = FObjectManager->GetMarketStatus( '0' );
        if( State == msStop || State == msClosed )///< Normal market closed. Switch to Post trading session.
            SetAPCode( '9' );
        else
            SetAPCode( '0' );
    }    
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyOrderConnectionObject::Execute( void )
{    
    UFC::SleepMS( rand()%50 *10 );
    UFC::BufferedLog::Printf( " --- SpeedyOrderConnectionObject ID:%s running ---", FParams.GetPVCID().c_str() );                              
    if( FIsFIX == TRUE )
    {
        FOrderFCM = new SpeedyFIXOrderClient( FObjectManager, FParams,FConnectionListener,FSubSystem,this, FIsTWSE );                   
        FOrderPerSec = (int)((double)FParams.GetThroughput()*ThroughputRatio);
        UFC::BufferedLog::Printf( " FIX PVC[%s] Exchange Throughput[%d] Ratio[%0.2f] Throughput limit[%d]", FPVCID, FParams.GetThroughput(),ThroughputRatio, FOrderPerSec );        
        if( UseLockLessQueue == TRUE )                  
            FProcessOrder = &SpeedyOrderConnectionObject::ProcessFIXOrderEx; ///< Process FIX order (Extreme version)                           
        else
            FProcessOrder = &SpeedyOrderConnectionObject::ProcessFIXOrder;   ///< Process FIX order
    }
    else ///< TMP session
    {
        FOrderFCM = new SpeedyOrderClient( FObjectManager, FParams,FConnectionListener,FSubSystem,this, FIsTWSE );            
        FOrderPerSec = 30;
        if( UseLockLessQueue == TRUE ) 
            FProcessOrder = &SpeedyOrderConnectionObject::ProcessOrderEx;    ///< Process TMP order (Extreme version)                           
        else       
            FProcessOrder = &SpeedyOrderConnectionObject::ProcessOrder;      ///< Process TMP order
    }    

    for( int i = 0; i < TWSE_PVC_BUSY_RETRY_TIMES; i++ )
    {
        try
        {
            DisableLoop( ); ///< Is this PVC disable??
            WaitingLoop( ); ///< Is the market opened??
            CheckAPCodeNow( );
            FOrderFCM->Open();            
            OpenMarket();            
            FOrderFCM->Close();            
        }
        catch( MarketClosedException& e )
        {
            UFC::BufferedLog::Printf( " PVC:%s %s", FParams.GetPVCID().c_str(), e.what() );                              
            FOrderFCM->Close();
        }
        catch( X25Exception& e )
        {
            UFC::BufferedLog::Printf( " TMP Exception:[%s]",e.what() );
            UFC::BufferedLog::Printf( " Try to reconnect after 5 sec." );
            FOrderFCM->Close();            
            sleep( 5 );
        }               
        catch( X25RehandshakeException& e )
        {
            UFC::BufferedLog::Printf( " Exception:[%s]",e.what() );
            UFC::BufferedLog::Printf( " Try to reconnect after 5 sec." );
            FOrderFCM->Close();              
            sleep( 5 );
        }
        catch( UFC::SocketException& e )
        {
            UFC::BufferedLog::Printf( " Socket Exception:[%s]",e.what() );
            UFC::BufferedLog::Printf( " Try to reconnect after 5 sec." );
            FOrderFCM->Close();
            sleep( 5 );
        }
        catch( UFC::FileException& e )
        {
            UFC::BufferedLog::Printf( " File Exception:[%s]",e.what() );
            return;
        }
        catch( UFC::Exception& e )
        {
            UFC::BufferedLog::Printf( " UFC Exception:[%s]",e.what() );
            return;
        }
        catch(...)
        {
            FConnectionListener->OnConnectionNotify( this, CONNECTION_OBJECT_UNKNOWN_ERROR );// Unknown exception.
            UFC::BufferedLog::Printf( " PVC:%s Unknown connection error.",FParams.GetPVCID().c_str() );
            return;
        }
        UFC::BufferedLog::Printf( " PVC:%s Reconnect %d times.", FParams.GetPVCID().c_str(), i );
    }    
    FOrderFCM->Close();               
    delete FOrderFCM;
    FOrderFCM = NULL;    
    return;
}
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
