#include "TWSESpeedyOrderClient.h"
#include "TWSESpeedyObjectsManager.h"
#include "TWSESpeedyFIXManager.h"
#include "ThreadObjectPool.h"
#include "Profiler.h"
#include "../TSECLib/LinkSubSystem.h"
#include "../TSECLib/Connection.h"
#include "../TSECLib/ConnectionObject.h"
#include "../SpeedyGateway/Administrator.h"
#include "../TSECLib/TWSERMClass.h"
extern UFC::PThreadObjectPool<OrderRecord>* OrderRecodPoolPtr;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::FIXConfirmLend( const FIX44::ExecutionReport& Report, OrderInfo* OI )
{
    FIX::TargetSubID  TargetSubID;
    FIX::OrderID      OrderID; //Req
    FIX::ClOrdID      ClOrdID; 
    FIX::ExecID       ExecID;  //Req *
    FIX::ExecType     ExecType;//Req
    FIX::OrdType      OrdType( FIX::OrdType_LIMIT );  
    FIX::TimeInForce  TimeInForce( FIX::TimeInForce_DAY );    
    FIX::Account      Account;
    FIX::Symbol       Symbol;//Req
    FIX::Side         Side;  //Req
    FIX::TransactTime TransactTime;//Req 
    FIX::OrderQty     OrderQty( 0.0 );
    FIX::Price        Price( 0.0 );
    FIX::LeavesQty    LeavesQty;//Req 
    //FIX::Text         Text; 
    string            TwseAccFlag( " " ); ///< 10000
    string            TwseOrdType( "0" ); ///< 10001
    string            TwseExCode( "0" );  ///< 10002
    UFC::AnsiString   Msg;
    UFC::AnsiString   OrderNO;
    UFC::AnsiString   BrokerID;
    UFC::AnsiString   ExecKey;
    int               Y,M,D,h,m,s,ms;
    int               NID;
    OrderRecord*      OR;             
    
    Report.getHeader().get( TargetSubID );
    Report.get( OrderID );
    Report.get( ExecID );    
    Report.get( ExecType );    
    Report.get( Symbol );
    Report.get( Side );    
    Report.get( TransactTime );
    Report.get( LeavesQty );    
    if( Report.isSet( ClOrdID ) )
        Report.get( ClOrdID );
    if( Report.isSet( Account ) )
        Report.get( Account );
    if( Report.isSet( Price ) )
        Report.get( Price );    
    if( Report.isSet( OrderQty ) )
        Report.get( OrderQty );
    if( Report.isSet( OrdType ) )
        Report.get( OrdType );    
    if( Report.isSet( TimeInForce ) )
        Report.get( TimeInForce );        
    if( Report.isSetField( 10000 ) )
        TwseAccFlag = Report.getField( 10000 );
    if( Report.isSetField( 10001 ) )
        TwseOrdType = Report.getField( 10001 );
    if( Report.isSetField( 10002 ) )
        TwseExCode  = Report.getField( 10002 );    
    OrderNO  = OrderID.getString().c_str();
    ExecKey  = ExecID.getString().c_str();
    BrokerID = TargetSubID.getString().c_str();
    NID = UFC::AnsiString( ClOrdID.getString().c_str() + 2 ).ToInt();
    TransactTime.getValue().getYMD( Y,M,D );
    TransactTime.getValue().getHMS( h, m, s, ms );       
    if( (OR = FObjectManager->PopOrderRecord( NID ) ) == NULL )
         OR = FObjectManager->PopOrderRecord( ClOrdID.getString().c_str() ); ///< From SMP protocol , pop OrderRecord from ClOrdID
    if( OR != NULL  || ExecType == FIX::ExecType_RESTATED )
    {
        if( OrderNO.AnsiCompare( OR->GetOrderID() ) == 0  )
        {   ///< Fill V020 fields
            MessageV020       LendReply( FIsTWSE ); 

            memcpy( LendReply.PVCID,        FPVCID.c_str(), 2 );
            memcpy( LendReply.OrderNo,      OrderID.getString().c_str(), 5 );
            memcpy( LendReply.BrokerID,     BrokerID.c_str(), 4 );
            memcpy( LendReply.InvestorAcno, Account.getString().c_str(), 7 );            
            memcpy( LendReply.StockID,      Symbol.getString().c_str(), 6 );
            LendReply.Keep = ' ';
            LendReply.SetPrice( Price.getValue() );
            LendReply.SetQty( (int)OrderQty.getValue() );                                
            LendReply.SetDate( Y, M, D );
            LendReply.SetTime( (h+8)%24, m, s, ms );              
            
            switch( ExecType )
            {
                case FIX::ExecType_NEW: ///< NewOrder.
                                        ///< Before Qty = 000   
                                        LendReply.SetBeforeQty( 0 );
                                        ///< After Qty = LeavesQty
                                        LendReply.SetAfterQty( (int)LeavesQty.getValue() );
                                        ProcessFIXErrorMsg( Report, &LendReply, Msg );
                                        LendReply.SetFunctionCode( 1 );
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                        FOwner->HandleExchangeReplyMessage( &LendReply, *OR, 0, mtV020, TRUE, FALSE, ExecKey, OR->Key );
                                        break;
                case FIX::ExecType_REPLACE:  ///< Replaced. ( Not support replace price )
                                        ///< Before Qty = OrderQty + LeavesQty   
                                        LendReply.SetBeforeQty( (int)(OrderQty.getValue() + LeavesQty.getValue()) );
                                        ///< After Qty = LeavesQty
                                        LendReply.SetAfterQty( (int)LeavesQty.getValue() );
                                        LendReply.SetStatusCode( 0 );
                                        LendReply.SetFunctionCode( 3 ); 
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                        FOwner->HandleExchangeReplyMessage( &LendReply,*OR, 0, mtV020,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                        break;
                case FIX::ExecType_RESTATED: 
                                        LendReply.SetBeforeQty( (int)OrderQty.getValue() );                                        
                                        LendReply.SetAfterQty( (int)LeavesQty.getValue() );
                                        LendReply.SetStatusCode( 51 );
                                        LendReply.SetFunctionCode( 2 );             
                                        FOwner->HandleExchangeReplyMessage( &LendReply, OI, mtV020, TRUE, TRUE, ExecKey, OI->GetKeyString(), "Unsolicited cancel by Exchange" );
                                        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_RESTATED OrderID[%s] canceled by exchange.", OrderNO.c_str() );                                                     
                                        break;
                case FIX::ExecType_CANCELED: ///< Canceled.       
                                        ///< Before Qty = OrderQty
                                        LendReply.SetBeforeQty( (int)OrderQty.getValue() );
                                        ///< After Qty = LeavesQty
                                        LendReply.SetAfterQty( (int)LeavesQty.getValue() );
                                        LendReply.SetStatusCode( 0 );
                                        LendReply.SetFunctionCode( 2 );  
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                        FOwner->HandleExchangeReplyMessage( &LendReply,*OR, 0, mtV020,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                        break;
                case FIX::ExecType_REJECTED: ///< New order Reject.
                                        if( ProcessFIXRejectMsg(  Report, &LendReply, Msg ) == true )
                                        {
                                            LendReply.SetBeforeQty( 0 );  ///< Before Qty = 000      
                                            LendReply.SetAfterQty( (int)LeavesQty.getValue() );                                                 
                                            LendReply.SetFunctionCode( 1 );
                                            UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                            FOwner->HandleExchangeReplyMessage( &LendReply,*OR,  0, mtV020,TRUE,TRUE,ExecKey, OI->GetKeyString(), Msg );                                    
                                            if( LendReply.GetStatusCode() == 89 && FObjectManager->StopOnOverLimit() )
                                            {
                                                UFC::BufferedLog::Printf( " Disable Session[%s:%d:%s] [89]Error Over Limit", FLineBrokerID.c_str(), FPort, FPVCID.c_str());       
                                                FObjectManager->EnableConnection( FLineBrokerID, FPort, FPVCID, false );
                                            }                                        
                                        }
                                        else
                                           UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_REJECTED OrderID[%s] without Tag 58.", OrderNO.c_str() );                                                     
                                        break;                
                case FIX::ExecType_ORDER_STATUS: ///< Order ststus
                                        LendReply.SetBeforeQty( (int)OrderQty.getValue() );
                                        LendReply.SetAfterQty( (int)OrderQty.getValue() );
                                        LendReply.SetStatusCode( 0 );
                                        LendReply.SetFunctionCode( 4 ); 
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );                                                
                                        FOwner->HandleExchangeReplyMessage( &LendReply,*OR,  0, mtV020,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                        break;       
                default:                UFC::BufferedLog::Printf( " [ExecutionReport] un-handled ExecType[%c]",ExecType.getValue() );       
                                        break;     
            }
        }
        else
            UFC::BufferedLog::Printf( " [ExecutionReport] Original OrderID[%s] BrokerID[%s] exec OrderID[%s] not match.", OrderNO.c_str(), BrokerID.c_str(), OR->GetOrderID()/*.c_str()*/ );    
        //delete OR; ///< delete object get from PopOrderRecord.
        OrderRecodPoolPtr->Recycle( OR );
    }
    else
        UFC::BufferedLog::Printf( " [ExecutionReport] Execution without New,Cancel, OrderID[%s] BrokerID[%s]", OrderNO.c_str(), BrokerID.c_str() );    
        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::FIXConfirmAuction( const FIX44::ExecutionReport& Report, OrderInfo* OI )
{
    FIX::TargetSubID  TargetSubID;
    FIX::OrderID      OrderID; //Req
    FIX::ClOrdID      ClOrdID; 
    FIX::ExecID       ExecID;  //Req *
    FIX::ExecType     ExecType;//Req
    FIX::OrdType      OrdType( FIX::OrdType_LIMIT );  
    FIX::TimeInForce  TimeInForce( FIX::TimeInForce_DAY );    
    FIX::Account      Account;
    FIX::Symbol       Symbol;//Req
    FIX::Side         Side;  //Req
    FIX::TransactTime TransactTime;//Req 
    FIX::OrderQty     OrderQty( 0.0 );
    FIX::Price        Price( 0.0 );
    FIX::LeavesQty    LeavesQty;//Req 
    //FIX::Text         Text; 
    string            TwseAccFlag( " " ); ///< 10000
    string            TwseOrdType( "0" ); ///< 10001
    string            TwseExCode( "0" );  ///< 10002
    UFC::AnsiString   Msg;
    UFC::AnsiString   OrderNO;
    UFC::AnsiString   BrokerID;
    UFC::AnsiString   ExecKey;
    int               Y,M,D,h,m,s,ms;
    int               NID;
    OrderRecord*      OR;             
    
    Report.getHeader().get( TargetSubID );
    Report.get( OrderID );
    Report.get( ExecID );    
    Report.get( ExecType );    
    Report.get( Symbol );
    Report.get( Side );    
    Report.get( TransactTime );
    Report.get( LeavesQty );    
    if( Report.isSet( ClOrdID ) )
        Report.get( ClOrdID );
    if( Report.isSet( Account ) )
        Report.get( Account );
    if( Report.isSet( Price ) )
        Report.get( Price );    
    if( Report.isSet( OrderQty ) )
        Report.get( OrderQty );
    if( Report.isSet( OrdType ) )
        Report.get( OrdType );    
    if( Report.isSet( TimeInForce ) )
        Report.get( TimeInForce );        
    if( Report.isSetField( 10000 ) )
        TwseAccFlag = Report.getField( 10000 );
    if( Report.isSetField( 10001 ) )
        TwseOrdType = Report.getField( 10001 );
    if( Report.isSetField( 10002 ) )
        TwseExCode  = Report.getField( 10002 );    
    OrderNO  = OrderID.getString().c_str();
    ExecKey  = ExecID.getString().c_str();
    BrokerID = TargetSubID.getString().c_str();
    NID = UFC::AnsiString( ClOrdID.getString().c_str() + 2 ).ToInt();
    TransactTime.getValue().getYMD( Y,M,D );
    TransactTime.getValue().getHMS( h, m, s, ms );       
    if( (OR = FObjectManager->PopOrderRecord( NID ) ) == NULL )
         OR = FObjectManager->PopOrderRecord( ClOrdID.getString().c_str() ); ///< From SMP protocol , pop OrderRecord from ClOrdID
    if( OR != NULL  || ExecType == FIX::ExecType_RESTATED )
    {
        if( OrderNO.AnsiCompare( OR->GetOrderID() ) == 0  )
        {   ///< Fill A020 fields
            MessageA020       AuctionReply( FIsTWSE ); 
    
            memcpy( AuctionReply.BrokerID,     BrokerID.c_str(), 4 );
            memcpy( AuctionReply.PVCID,        FPVCID.c_str(), 2 );
            memcpy( AuctionReply.OrderNo,      OrderID.getString().c_str(), 5 );            
            memcpy( AuctionReply.InvestorAcno, Account.getString().c_str(), 7 );            
            memcpy( AuctionReply.StockID,      Symbol.getString().c_str(), 6 );
            AuctionReply.SetPrice( Price.getValue() );
            AuctionReply.SetQty( (Int64)OrderQty.getValue() );                                
            AuctionReply.SetDate( Y, M, D );
            AuctionReply.SetTime( (h+8)%24, m, s, ms );              
            
            switch( ExecType )
            {
                case FIX::ExecType_NEW: ///< NewOrder.
                                        ///< Before Qty = 000   
                                        AuctionReply.SetBeforeQty( 0 );
                                        ///< After Qty = LeavesQty
                                        AuctionReply.SetAfterQty( (Int64)LeavesQty.getValue() );
                                        ProcessFIXErrorMsg( Report, &AuctionReply, Msg );
                                        AuctionReply.SetFunctionCode( 1 );
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );                                                
                                        FOwner->HandleExchangeReplyMessage( &AuctionReply, *OR,  0, mtA020, TRUE, FALSE, ExecKey, OR->Key );
                                        break;
                case FIX::ExecType_REPLACE:  ///< Replaced. ( Not support replace price )
                                        ///< Before Qty = OrderQty + LeavesQty   
                                        AuctionReply.SetBeforeQty( (Int64)(OrderQty.getValue() + LeavesQty.getValue()) );
                                        ///< After Qty = LeavesQty
                                        AuctionReply.SetAfterQty( (Int64)LeavesQty.getValue() );
                                        AuctionReply.SetStatusCode( 0 );
                                        AuctionReply.SetFunctionCode( 3 ); 
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                        FOwner->HandleExchangeReplyMessage( &AuctionReply,*OR, 0,  mtA020,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                        break;
                case FIX::ExecType_RESTATED: 
                                        AuctionReply.SetBeforeQty( (Int64)OrderQty.getValue() );                                        
                                        AuctionReply.SetAfterQty( (Int64)LeavesQty.getValue() );
                                        AuctionReply.SetStatusCode( 51 );
                                        AuctionReply.SetFunctionCode( 2 );             
                                        FOwner->HandleExchangeReplyMessage( &AuctionReply, OI, mtA020, TRUE, TRUE, ExecKey, OI->GetKeyString(), "Unsolicited cancel by Exchange" );
                                        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_RESTATED OrderID[%s] canceled by exchange.", OrderNO.c_str() );                                                     
                                        break;
                case FIX::ExecType_CANCELED: ///< Canceled.       
                                        ///< Before Qty = OrderQty
                                        AuctionReply.SetBeforeQty( (Int64)OrderQty.getValue() );
                                        ///< After Qty = LeavesQty
                                        AuctionReply.SetAfterQty( (Int64)LeavesQty.getValue() );
                                        AuctionReply.SetStatusCode( 0 );
                                        AuctionReply.SetFunctionCode( 2 );  
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );                                                
                                        FOwner->HandleExchangeReplyMessage( &AuctionReply,*OR, 0,  mtA020,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                        break;
                case FIX::ExecType_REJECTED: ///< New order Reject.
                                        if( ProcessFIXRejectMsg(  Report, &AuctionReply, Msg ) == true )
                                        {
                                            AuctionReply.SetBeforeQty( 0 );  ///< Before Qty = 000      
                                            AuctionReply.SetAfterQty( (Int64)LeavesQty.getValue() );                                                 
                                            AuctionReply.SetFunctionCode( 1 );
                                            UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                            FOwner->HandleExchangeReplyMessage( &AuctionReply,*OR, 0,  mtA020,TRUE,TRUE,ExecKey, OI->GetKeyString(), Msg );                                    
                                            if( AuctionReply.GetStatusCode() == 89 && FObjectManager->StopOnOverLimit() )
                                            {
                                                UFC::BufferedLog::Printf( " Disable Session[%s:%d:%s] [89]Error Over Limit", FLineBrokerID.c_str(), FPort, FPVCID.c_str());       
                                                FObjectManager->EnableConnection( FLineBrokerID, FPort, FPVCID, false );
                                            }                                        
                                        }
                                        else
                                           UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_REJECTED OrderID[%s] without Tag 58.", OrderNO.c_str() );                                                     
                                        break;                
                case FIX::ExecType_ORDER_STATUS: ///< Order ststus
                                        AuctionReply.SetBeforeQty( (Int64)OrderQty.getValue() );
                                        AuctionReply.SetAfterQty( (Int64)OrderQty.getValue() );
                                        AuctionReply.SetStatusCode( 0 );
                                        AuctionReply.SetFunctionCode( 4 ); 
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );                                                
                                        FOwner->HandleExchangeReplyMessage( &AuctionReply,*OR, 0,  mtA020,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                        break;       
                default:                UFC::BufferedLog::Printf( " [ExecutionReport] un-handled ExecType[%c]",ExecType.getValue() );       
                                        break;     
            }
        }
        else
            UFC::BufferedLog::Printf( " [ExecutionReport] Original OrderID[%s] BrokerID[%s] exec OrderID[%s] not match.", OrderNO.c_str(), BrokerID.c_str(), OR->GetOrderID()/*.c_str()*/ );    
        //delete OR; ///< delete object get from PopOrderRecord.
        OrderRecodPoolPtr->Recycle( OR );
    }
    else
        UFC::BufferedLog::Printf( " [ExecutionReport] Execution without New,Cancel, OrderID[%s] BrokerID[%s]", OrderNO.c_str(), BrokerID.c_str() );    
        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::FIXConfirmTender( const FIX44::ExecutionReport& Report, OrderInfo* OI )
{
    FIX::TargetSubID  TargetSubID;
    FIX::OrderID      OrderID; //Req
    FIX::ClOrdID      ClOrdID; 
    FIX::ExecID       ExecID;  //Req *
    FIX::ExecType     ExecType;//Req
    FIX::OrdType      OrdType( FIX::OrdType_LIMIT );  
    FIX::TimeInForce  TimeInForce( FIX::TimeInForce_DAY );    
    FIX::Account      Account;
    FIX::Symbol       Symbol;//Req
    FIX::Side         Side;  //Req
    FIX::TransactTime TransactTime;//Req 
    FIX::OrderQty     OrderQty( 0.0 );
    FIX::Price        Price( 0.0 );
    FIX::LeavesQty    LeavesQty;//Req 
    //FIX::Text         Text; 
    string            TwseAccFlag( " " ); ///< 10000
    string            TwseOrdType( "0" ); ///< 10001
    string            TwseExCode( "0" );  ///< 10002
    string            StkSeqNo( "  "); ///< 1080              
    UFC::AnsiString   Msg;
    UFC::AnsiString   OrderNO;
    UFC::AnsiString   BrokerID;
    UFC::AnsiString   ExecKey;
    int               Y,M,D,h,m,s,ms;
    int               NID;
    OrderRecord*      OR;             
    
    Report.getHeader().get( TargetSubID );
    Report.get( OrderID );
    Report.get( ExecID );    
    Report.get( ExecType );    
    Report.get( Symbol );
    Report.get( Side );    
    Report.get( TransactTime );
    Report.get( LeavesQty );    
    if( Report.isSet( ClOrdID ) )
        Report.get( ClOrdID );
    if( Report.isSet( Account ) )
        Report.get( Account );
    if( Report.isSet( Price ) )
        Report.get( Price );    
    if( Report.isSet( OrderQty ) )
        Report.get( OrderQty );
    if( Report.isSet( OrdType ) )
        Report.get( OrdType );    
    if( Report.isSet( TimeInForce ) )
        Report.get( TimeInForce );        
    if( Report.isSetField( 10000 ) )
        TwseAccFlag = Report.getField( 10000 );
    if( Report.isSetField( 10001 ) )
        TwseOrdType = Report.getField( 10001 );
    if( Report.isSetField( 10002 ) )
        TwseExCode  = Report.getField( 10002 );    
    if( Report.isSetField( 1080 ) )
        StkSeqNo  = Report.getField( 1080 );        
    OrderNO  = OrderID.getString().c_str();
    ExecKey  = ExecID.getString().c_str();
    BrokerID = TargetSubID.getString().c_str();
    NID = UFC::AnsiString( ClOrdID.getString().c_str() + 2 ).ToInt();
    TransactTime.getValue().getYMD( Y,M,D );
    TransactTime.getValue().getHMS( h, m, s, ms );       
    if( (OR = FObjectManager->PopOrderRecord( NID ) ) == NULL )
         OR = FObjectManager->PopOrderRecord( ClOrdID.getString().c_str() ); ///< From SMP protocol , pop OrderRecord from ClOrdID
    if( OR != NULL  || ExecType == FIX::ExecType_RESTATED )
    {
        if( OrderNO.AnsiCompare( OR->GetOrderID() ) == 0  )
        {   ///< Fill E020 fields
            MessageE020       TenderReply( FIsTWSE ); 
    
            memcpy( TenderReply.BrokerID,     BrokerID.c_str(), 4 );
            memcpy( TenderReply.PVCID,        FPVCID.c_str(), 2 );
            memcpy( TenderReply.OrderNo,      OrderID.getString().c_str(), 5 );            
            memcpy( TenderReply.InvestorAcno, Account.getString().c_str(), 7 );            
            memcpy( TenderReply.StockID,      Symbol.getString().c_str(), 6 );
            memcpy( TenderReply.StkSeqNo, StkSeqNo.c_str(), 2 );
            TenderReply.SetPrice( Price.getValue() );
            TenderReply.SetQty( (Int64)OrderQty.getValue() );                                
            TenderReply.SetDate( Y, M, D );
            TenderReply.SetTime( (h+8)%24, m, s, ms );              
            
            switch( ExecType )
            {
                case FIX::ExecType_NEW: ///< NewOrder.
                                        ///< Before Qty = 000   
                                        TenderReply.SetBeforeQty( 0 );
                                        ///< After Qty = LeavesQty
                                        TenderReply.SetAfterQty( (Int64)LeavesQty.getValue() );
                                        ProcessFIXErrorMsg( Report, &TenderReply, Msg );
                                        TenderReply.SetFunctionCode( 1 );
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );                                                
                                        FOwner->HandleExchangeReplyMessage( &TenderReply, *OR,  0, mtE020, TRUE, FALSE, ExecKey, OR->Key );
                                        break;
                case FIX::ExecType_REPLACE:  ///< Replaced. ( Not support replace price )
                                        ///< Before Qty = OrderQty + LeavesQty   
                                        TenderReply.SetBeforeQty( (Int64)(OrderQty.getValue() + LeavesQty.getValue()) );
                                        ///< After Qty = LeavesQty
                                        TenderReply.SetAfterQty( (Int64)LeavesQty.getValue() );
                                        TenderReply.SetStatusCode( 0 );
                                        TenderReply.SetFunctionCode( 3 ); 
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                        FOwner->HandleExchangeReplyMessage( &TenderReply,*OR,  0, mtE020,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                        break;
                case FIX::ExecType_RESTATED: 
                                        TenderReply.SetBeforeQty( (Int64)OrderQty.getValue() );                                        
                                        TenderReply.SetAfterQty( (Int64)LeavesQty.getValue() );
                                        TenderReply.SetStatusCode( 51 );
                                        TenderReply.SetFunctionCode( 2 );             
                                        FOwner->HandleExchangeReplyMessage( &TenderReply, OI, mtE020, TRUE, TRUE, ExecKey, OI->GetKeyString(), "Unsolicited cancel by Exchange" );
                                        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_RESTATED OrderID[%s] canceled by exchange.", OrderNO.c_str() );                                                     
                                        break;
                case FIX::ExecType_CANCELED: ///< Canceled.       
                                        ///< Before Qty = OrderQty
                                        TenderReply.SetBeforeQty( (Int64)OrderQty.getValue() );
                                        ///< After Qty = LeavesQty
                                        TenderReply.SetAfterQty( (Int64)LeavesQty.getValue() );
                                        TenderReply.SetStatusCode( 0 );
                                        TenderReply.SetFunctionCode( 2 );  
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );                                                
                                        FOwner->HandleExchangeReplyMessage( &TenderReply,*OR,  0, mtE020,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                        break;
                case FIX::ExecType_REJECTED: ///< New order Reject.
                                        if( ProcessFIXRejectMsg(  Report, &TenderReply, Msg ) == true )
                                        {
                                            TenderReply.SetBeforeQty( 0 );  ///< Before Qty = 000      
                                            TenderReply.SetAfterQty( (Int64)LeavesQty.getValue() );                                                 
                                            TenderReply.SetFunctionCode( 1 );
                                            UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                            FOwner->HandleExchangeReplyMessage( &TenderReply,*OR,  0, mtE020,TRUE,TRUE,ExecKey, OI->GetKeyString(), Msg );                                    
                                            if( TenderReply.GetStatusCode() == 89 && FObjectManager->StopOnOverLimit() )
                                            {
                                                UFC::BufferedLog::Printf( " Disable Session[%s:%d:%s] [89]Error Over Limit", FLineBrokerID.c_str(), FPort, FPVCID.c_str());       
                                                FObjectManager->EnableConnection( FLineBrokerID, FPort, FPVCID, false );
                                            }                                        
                                        }
                                        else
                                           UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_REJECTED OrderID[%s] without Tag 58.", OrderNO.c_str() );                                                     
                                        break;                
                case FIX::ExecType_ORDER_STATUS: ///< Order ststus
                                        TenderReply.SetBeforeQty( (Int64)OrderQty.getValue() );
                                        TenderReply.SetAfterQty( (Int64)OrderQty.getValue() );
                                        TenderReply.SetStatusCode( 0 );
                                        TenderReply.SetFunctionCode( 4 ); 
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );                                                
                                        FOwner->HandleExchangeReplyMessage( &TenderReply,*OR,  0, mtE020,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                        break;       
                default:                UFC::BufferedLog::Printf( " [ExecutionReport] un-handled ExecType[%c]",ExecType.getValue() );       
                                        break;     
            }
        }
        else
            UFC::BufferedLog::Printf( " [ExecutionReport] Original OrderID[%s] BrokerID[%s] exec OrderID[%s] not match.", OrderNO.c_str(), BrokerID.c_str(), OR->GetOrderID()/*.c_str()*/ );    
        //delete OR; ///< delete object get from PopOrderRecord.
        OrderRecodPoolPtr->Recycle( OR );
    }
    else
        UFC::BufferedLog::Printf( " [ExecutionReport] Execution without New,Cancel, OrderID[%s] BrokerID[%s]", OrderNO.c_str(), BrokerID.c_str() );    
        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyFIXOrderClient::FIXConfirmTenderEx( const FIX44::ExecutionReport& Report, OrderInfo* OI )
{
    FIX::TargetSubID  TargetSubID;
    FIX::OrderID      OrderID; //Req
    FIX::ClOrdID      ClOrdID; 
    FIX::ExecID       ExecID;  //Req *
    FIX::ExecType     ExecType;//Req
    FIX::OrdType      OrdType( FIX::OrdType_LIMIT );  
    FIX::TimeInForce  TimeInForce( FIX::TimeInForce_DAY );    
    FIX::Account      Account;
    FIX::Symbol       Symbol;//Req
    FIX::Side         Side;  //Req
    FIX::TransactTime TransactTime;//Req 
    FIX::OrderQty     OrderQty( 0.0 );
    FIX::Price        Price( 0.0 );
    FIX::LeavesQty    LeavesQty;//Req 
    //FIX::Text         Text; 
    string            TwseAccFlag( " " ); ///< 10000
    string            TwseOrdType( "0" ); ///< 10001
    string            TwseExCode( "0" );  ///< 10002
    string            StkSeqNo( "  "); ///< 1080              
    UFC::AnsiString   Msg;
    UFC::AnsiString   OrderNO;
    UFC::AnsiString   BrokerID;
    UFC::AnsiString   ExecKey;
    int               Y,M,D,h,m,s,ms;
    int               NID;
    OrderRecord*      OR;             
    
    Report.getHeader().get( TargetSubID );
    Report.get( OrderID );
    Report.get( ExecID );    
    Report.get( ExecType );    
    Report.get( Symbol );
    Report.get( Side );    
    Report.get( TransactTime );
    Report.get( LeavesQty );    
    if( Report.isSet( ClOrdID ) )
        Report.get( ClOrdID );
    if( Report.isSet( Account ) )
        Report.get( Account );
    if( Report.isSet( Price ) )
        Report.get( Price );    
    if( Report.isSet( OrderQty ) )
        Report.get( OrderQty );
    if( Report.isSet( OrdType ) )
        Report.get( OrdType );    
    if( Report.isSet( TimeInForce ) )
        Report.get( TimeInForce );        
    if( Report.isSetField( 10000 ) )
        TwseAccFlag = Report.getField( 10000 );
    if( Report.isSetField( 10001 ) )
        TwseOrdType = Report.getField( 10001 );
    if( Report.isSetField( 10002 ) )
        TwseExCode  = Report.getField( 10002 );    
    if( Report.isSetField( 1080 ) )
        StkSeqNo  = Report.getField( 1080 );        
    OrderNO  = OrderID.getString().c_str();
    ExecKey  = ExecID.getString().c_str();
    BrokerID = TargetSubID.getString().c_str();
    NID = UFC::AnsiString( ClOrdID.getString().c_str() + 2 ).ToInt();
    TransactTime.getValue().getYMD( Y,M,D );
    TransactTime.getValue().getHMS( h, m, s, ms );       
    if( (OR = FObjectManager->PopOrderRecord( NID ) ) == NULL )
         OR = FObjectManager->PopOrderRecord( ClOrdID.getString().c_str() ); ///< From SMP protocol , pop OrderRecord from ClOrdID
    if( OR != NULL  || ExecType == FIX::ExecType_RESTATED )
    {
        if( OrderNO.AnsiCompare( OR->GetOrderID() ) == 0  )
        {   ///< Fill E020 fields
            MessageEx020       TenderReply( FIsTWSE ); 
    
            memcpy( TenderReply.BrokerID,     BrokerID.c_str(), 4 );
            memcpy( TenderReply.PVCID,        FPVCID.c_str(), 2 );
            memcpy( TenderReply.OrderNo,      OrderID.getString().c_str(), 5 );            
            memcpy( TenderReply.InvestorAcno, Account.getString().c_str(), 7 );            
            memcpy( TenderReply.StockID,      Symbol.getString().c_str(), 6 );
            memcpy( TenderReply.StkSeqNo, StkSeqNo.c_str(), 2 );
            TenderReply.SetPrice( Price.getValue() );
            TenderReply.SetQty( (Int64)OrderQty.getValue() );                                
            TenderReply.SetDate( Y, M, D );
            TenderReply.SetTime( (h+8)%24, m, s, ms );              
            
            switch( ExecType )
            {
                case FIX::ExecType_NEW: ///< NewOrder.
                                        ///< Before Qty = 000   
                                        TenderReply.SetBeforeQty( 0 );
                                        ///< After Qty = LeavesQty
                                        TenderReply.SetAfterQty( (Int64)LeavesQty.getValue() );
                                        ProcessFIXErrorMsg( Report, &TenderReply, Msg );
                                        TenderReply.SetFunctionCode( 1 );
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );                                                
                                        FOwner->HandleExchangeReplyMessage( &TenderReply, *OR, 0,  mtEx020, TRUE, FALSE, ExecKey, OR->Key );
                                        break;
                case FIX::ExecType_REPLACE:  ///< Replaced. ( Not support replace price )
                                        ///< Before Qty = OrderQty + LeavesQty   
                                        TenderReply.SetBeforeQty( (Int64)(OrderQty.getValue() + LeavesQty.getValue()) );
                                        ///< After Qty = LeavesQty
                                        TenderReply.SetAfterQty( (Int64)LeavesQty.getValue() );
                                        TenderReply.SetStatusCode( 0 );
                                        TenderReply.SetFunctionCode( 3 ); 
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                        FOwner->HandleExchangeReplyMessage( &TenderReply,*OR,  0, mtEx020,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                        break;
                case FIX::ExecType_RESTATED: 
                                        TenderReply.SetBeforeQty( (Int64)OrderQty.getValue() );                                        
                                        TenderReply.SetAfterQty( (Int64)LeavesQty.getValue() );
                                        TenderReply.SetStatusCode( 51 );
                                        TenderReply.SetFunctionCode( 2 );             
                                        FOwner->HandleExchangeReplyMessage( &TenderReply, OI, mtEx020, TRUE, TRUE, ExecKey, OI->GetKeyString(), "Unsolicited cancel by Exchange" );
                                        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_RESTATED OrderID[%s] canceled by exchange.", OrderNO.c_str() );                                                     
                                        break;
                case FIX::ExecType_CANCELED: ///< Canceled.       
                                        ///< Before Qty = OrderQty
                                        TenderReply.SetBeforeQty( (Int64)OrderQty.getValue() );
                                        ///< After Qty = LeavesQty
                                        TenderReply.SetAfterQty( (Int64)LeavesQty.getValue() );
                                        TenderReply.SetStatusCode( 0 );
                                        TenderReply.SetFunctionCode( 2 );  
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );                                                
                                        FOwner->HandleExchangeReplyMessage( &TenderReply,*OR,  0, mtEx020,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                        break;
                case FIX::ExecType_REJECTED: ///< New order Reject.
                                        if( ProcessFIXRejectMsg(  Report, &TenderReply, Msg ) == true )
                                        {
                                            TenderReply.SetBeforeQty( 0 );  ///< Before Qty = 000      
                                            TenderReply.SetAfterQty( (Int64)LeavesQty.getValue() );                                                 
                                            TenderReply.SetFunctionCode( 1 );
                                            UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );        
                                            FOwner->HandleExchangeReplyMessage( &TenderReply,*OR,  0, mtEx020,TRUE,TRUE,ExecKey, OI->GetKeyString(), Msg );                                    
                                            if( TenderReply.GetStatusCode() == 89 && FObjectManager->StopOnOverLimit() )
                                            {
                                                UFC::BufferedLog::Printf( " Disable Session[%s:%d:%s] [89]Error Over Limit", FLineBrokerID.c_str(), FPort, FPVCID.c_str());       
                                                FObjectManager->EnableConnection( FLineBrokerID, FPort, FPVCID, false );
                                            }                                        
                                        }
                                        else
                                           UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [ExecutionReport] ExecType_REJECTED OrderID[%s] without Tag 58.", OrderNO.c_str() );                                                     
                                        break;                
                case FIX::ExecType_ORDER_STATUS: ///< Order ststus
                                        TenderReply.SetBeforeQty( (Int64)OrderQty.getValue() );
                                        TenderReply.SetAfterQty( (Int64)OrderQty.getValue() );
                                        TenderReply.SetStatusCode( 0 );
                                        TenderReply.SetFunctionCode( 4 ); 
                                        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppExchangeOrdConfirm, UFC::AnsiString( OR->NID ), OR->GetOrderID() );                                                
                                        FOwner->HandleExchangeReplyMessage( &TenderReply,*OR,  0, mtEx020,TRUE,TRUE,ExecKey, OI->GetKeyString() );
                                        break;       
                default:                UFC::BufferedLog::Printf( " [ExecutionReport] un-handled ExecType[%c]",ExecType.getValue() );       
                                        break;     
            }
        }
        else
            UFC::BufferedLog::Printf( " [ExecutionReport] Original OrderID[%s] BrokerID[%s] exec OrderID[%s] not match.", OrderNO.c_str(), BrokerID.c_str(), OR->GetOrderID()/*.c_str()*/ );    
        //delete OR; ///< delete object get from PopOrderRecord.
        OrderRecodPoolPtr->Recycle( OR );
    }
    else
        UFC::BufferedLog::Printf( " [ExecutionReport] Execution without New,Cancel, OrderID[%s] BrokerID[%s]", OrderNO.c_str(), BrokerID.c_str() );    
        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------