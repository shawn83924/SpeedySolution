/**@file TransactionObject.cpp
*  @brief Core Class to process all kinds of orders.
*  @author Simon Chaang
*
*/
#include "SimTSEC.h"
#include "TransactionObject.h"
#include "ConnectionObject.h"
#include "ExchangeConst.h"
#include "../UFC/TFixedFormat.h"
#include "../UFC/TParseData.h"
#include "OrderObjectsManagerThread.h"
//----------------------------------------------------------------------------------------------------------------------
extern MessageObject*  MessageObj;
extern TradeSession    CurrentSession;
extern BOOL            IsTSEC;
extern BOOL            MatchBySetting;
//----------------------------------------------------------------------------------------------------------------------
extern UFC::PHashMap<UFC::AnsiString,OrderMatchInfo*>  gMatchRule;
//----------------------------------------------------------------------------------------------------------------------
UFC::PStringList  Executions;
UFC::PEvent       ExecutionsEvent;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
//
//  Class TransactionObject
//
//----------------------------------------------------------------------------------------------------------------------
TransactionObject::TransactionObject(  TSECOrderManager* Manager  )
:FOpenSwitch( 0 ),
FMatchedOrder( 0 ),
FFillSeq( 1 ),
FOrderCount( 0 ),
FTotalResponseTime( 0 ),
FMaxResponseTime( 0 ),
FCheckPrice( FALSE ),
FManager( Manager )
{
    if( IsTSEC == TRUE )
    {
        if( UFC::FileExists( "ftTSE/T30" ) == TRUE )
        {
            UFC::TFixedFormat T30Format;
            UFC::TParseData   Parser;
            UFC::TDataset*    Dataset;        
            UFC::TRecord*     Stock;
            UFC::AnsiString   StockID,TPx,BPx,CPx;
            
            T30Format.SetIniFile( "cfg/TSE_T30.ini" );
            UFC::FileStream fs( "ftTSE/T30", O_RDONLY );
            Parser.SetDataFormat( &T30Format );
            Dataset = Parser.Parse( &fs );
            FCheckPrice = TRUE;   
            for( int i = 0; i < Dataset->GetRecordCount(); i++ )
            {
                Stock = Dataset->GetRecord( i );
                Stock->GetField( "Symbol", StockID );
                Stock->GetField( "BullPrice", TPx );
                Stock->GetField( "LDCPrice", CPx );
                Stock->GetField( "BearPrice", BPx );
                UFC::BufferedLog::Printf( " Stock:%s TPx:%s BPx:%s CPx:%s.",StockID.c_str(),TPx.c_str(),BPx.c_str(),CPx.c_str() );        
                FStocks.Add( StockID, new StockInfo( TPx.ToInt(), BPx.ToInt(), CPx.ToInt()) );
                OrderInfo::FStockLastPxs.Add( StockID, new int(CPx.ToInt()) );
            }
        }
        else
            UFC::BufferedLog::Printf( " ftTSE/T30 not found, disable price check." );        
    }
    else
    {
        if( UFC::FileExists("ftOTC/T30" ) == TRUE )
        {
            UFC::TFixedFormat T30Format;
            UFC::TParseData   Parser;
            UFC::TDataset*    Dataset;        
            UFC::TRecord*     Stock;
            UFC::AnsiString   StockID,TPx,BPx,CPx;
            
            T30Format.SetIniFile( "cfg/OTC_T30.ini" );
            UFC::FileStream fs( "ftOTC/T30", O_RDONLY );
            Parser.SetDataFormat( &T30Format );
            Dataset = Parser.Parse( &fs );
            FCheckPrice = TRUE;   
            for( int i = 0; i < Dataset->GetRecordCount(); i++ )
            {
                Stock = Dataset->GetRecord( i );
                Stock->GetField( "Symbol", StockID );
                Stock->GetField( "BullPrice", TPx );
                Stock->GetField( "LDCPrice", CPx );
                Stock->GetField( "BearPrice", BPx );
                UFC::BufferedLog::Printf( " Stock:%s TPx:%s BPx:%s CPx:%s.",StockID.c_str(),TPx.c_str(),BPx.c_str(),CPx.c_str() );        
                FStocks.Add( StockID, new StockInfo( TPx.ToInt(), BPx.ToInt(), CPx.ToInt()) );                
                OrderInfo::FStockLastPxs.Add( StockID, new int(CPx.ToInt()) );
            }
        }
        else
            UFC::BufferedLog::Printf( " ftOTC/T30 not found, disable price check." );        
    }
}
//----------------------------------------------------------------------------------------------------------------------
int TransactionObject::CheckPx( const UFC::AnsiString& Symbol, int Px )
{
    StockInfo* PxInfo;
    if( FCheckPrice == FALSE )
        return 1; ///< DON'T check price,Pass!
    if( (PxInfo = FStocks.GetObjectByKey( Symbol ) ) != NULL )
    {
        if( Px > PxInfo->TPrice || Px < PxInfo->BPrice )
        {
            UFC::BufferedLog::Printf( " Symbol[%s] should between[%d][%d] but received [%d]",Symbol.c_str(),PxInfo->TPrice, PxInfo->BPrice,Px );        
            return 0; ///< Wrong price.
        }
        else
            return 1; ///< OK
    }
    else
    {
        UFC::BufferedLog::Printf( " Symbol[%s] not exists.",Symbol.c_str() );        
        return -1; ///< Stock ID not exists.   
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::Reject( MessageHeader* RejectMsg, int Func, int Error )
{
    RejectMsg->SetFunctionCode( Func );
    RejectMsg->SetStatusCode( Error );
    return RejectMsg;
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::NormalReject( OrderInfo* OrderRecord, const UFC::AnsiString& PVCID,int Func, int Error, BOOL IsFIX  )
{
    MessageHeader* RejectMsg;
    
    if( IsFIX == TRUE )
        RejectMsg = OrderRecord->ConfirmT020( PVCID );   
    else
        RejectMsg = new MessageT030( IsTSEC );
    RejectMsg->SetFunctionCode( Func );
    RejectMsg->SetStatusCode( Error );
    return RejectMsg;
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::NewOrder( MessageT010* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );    
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::AnsiString   StockID( Order->StockID, 6 );        
    int               Px = UFC::AnsiString( Order->OrderPrice, 9 ).ToInt();
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;        
    if( (OrderRecord = FNormalOrders.GetObjectByKey( BrokerID + OID )) == NULL )
    {        
        int Result = CheckPx( StockID, Px );
        
        if( Order->OrderType == '1' ) ///< Market Order
            Result = 1;
        
        if( Result == 1 )
        {
            UFC::BufferedLog::Printf( " PVC:%s place a new normal order:%s.",PVCID.c_str(),OID.c_str() );        
            OrderRecord = new OrderInfo( OID, Order, PVCID, BrkID, IsFIX );
            FNormalOrders.Add( BrokerID + OID, OrderRecord );
            MessageT020* T020 = OrderRecord->ConfirmT020( PVCID );
            SetLastConfirm( PVCID, T020 );
            AddToMatchList( OrderRecord );        
            return T020;
        }
        else if( Result == -1 )
        {
            OrderInfo  RejectOrderRecord( OID, Order, PVCID, BrkID, IsFIX );
            return NormalReject( &RejectOrderRecord, PVCID, Order->GetFunctionCode(),20, IsFIX );
        }
        else //if( Result == 0 )
        {
            OrderInfo  RejectOrderRecord( OID, Order, PVCID, BrkID, IsFIX );
            return NormalReject( &RejectOrderRecord, PVCID, Order->GetFunctionCode(),21, IsFIX );            
        }
    }
    else ///< Order exists
    {
        UFC::BufferedLog::Printf( " PVC:%s place new order:%s already exists.",PVCID.c_str(),OID.c_str() );        
        OrderInfo  RejectOrderRecord( OID, Order, PVCID, BrkID, IsFIX );
        return NormalReject( &RejectOrderRecord, PVCID, Order->GetFunctionCode(), 5, IsFIX );              
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::PostReject( OrderInfo* OrderRecord, const UFC::AnsiString& PVCID,int Func, int Error, BOOL IsFIX  )
{
    MessageHeader* RejectMsg;
    
    if( IsFIX == TRUE )
        RejectMsg = OrderRecord->ConfirmP020( PVCID );   
    else
        RejectMsg = new MessageT030( IsTSEC );
    RejectMsg->SetFunctionCode( Func );
    RejectMsg->SetStatusCode( Error );
    return RejectMsg;
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::NewOrder( MessageP010* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::AnsiString   StockID( Order->StockID, 6 );        
    int               Px = UFC::AnsiString( Order->OrderPrice, 9 ).ToInt();    
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;        
    if( (OrderRecord = FPostOrders.GetObjectByKey( BrokerID + OID )) == NULL )
    {
        int Result = CheckPx( StockID, Px );
        if( Result == 1 )
        {        
            OrderRecord = new OrderInfo( OID, Order, PVCID, BrkID, IsFIX );
            FPostOrders.Add( BrokerID + OID, OrderRecord );        
            MessageP020* P020 = OrderRecord->ConfirmP020( PVCID );
            SetLastConfirm( PVCID, P020 );
            //AddToMatchList( OrderRecord );
            return P020;
        }    
        else if( Result == -1 )
        {
            OrderInfo  RejectOrderRecord( OID, Order, PVCID, BrkID, IsFIX );
            return PostReject( &RejectOrderRecord, PVCID, Order->GetFunctionCode(),20, IsFIX );
        }
        else //if( Result == 0 )
        {
            OrderInfo  RejectOrderRecord( OID, Order, PVCID, BrkID, IsFIX );
            return PostReject( &RejectOrderRecord, PVCID, Order->GetFunctionCode(),21, IsFIX );            
        }
        
    }
    else ///< Order exists
    {
        OrderInfo  RejectOrderRecord( OID, Order, PVCID, BrkID, IsFIX );
        return PostReject( &RejectOrderRecord, PVCID, Order->GetFunctionCode(), 40, IsFIX );                                                  
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::OddReject( OrderInfo* OrderRecord, const UFC::AnsiString& PVCID,int Func, int Error, BOOL IsFIX  )
{
    MessageHeader* RejectMsg;
    
    if( IsFIX == TRUE )
        RejectMsg = OrderRecord->ConfirmO020( PVCID );   
    else
        RejectMsg = new MessageT030( IsTSEC );
    RejectMsg->SetFunctionCode( Func );
    RejectMsg->SetStatusCode( Error );
    return RejectMsg;
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::NewOrder( MessageO010* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::AnsiString   StockID( Order->StockID, 6 );        
    int               Px = UFC::AnsiString( Order->OrderPrice, 6 ).ToInt();        
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;        
    if( (OrderRecord = FOddOrders.GetObjectByKey( BrokerID + OID )) == NULL )
    {
        int Result = CheckPx( StockID, Px );
        if( Result == 1 )
        {                
            OrderRecord = new OrderInfo( OID, Order, PVCID, BrkID, IsFIX );
            FOddOrders.Add( BrokerID + OID, OrderRecord );
            MessageO020* O020 = OrderRecord->ConfirmO020( PVCID );
            SetLastConfirm( PVCID, O020 );
            //AddToMatchList( OrderRecord );
            return O020;
        }
        else if( Result == -1 )
        {
            OrderInfo  RejectOrderRecord( OID, Order, PVCID, BrkID, IsFIX );
            return OddReject( &RejectOrderRecord, PVCID, Order->GetFunctionCode(),20, IsFIX );
        }
        else //if( Result == 0 )
        {
            OrderInfo  RejectOrderRecord( OID, Order, PVCID, BrkID, IsFIX );
            return OddReject( &RejectOrderRecord, PVCID, Order->GetFunctionCode(),21, IsFIX );            
        }
        
    }
    else ///< Order exists
    {
        OrderInfo  RejectOrderRecord( OID, Order, PVCID, BrkID, IsFIX );
        return OddReject( &RejectOrderRecord, PVCID, Order->GetFunctionCode(), 24, IsFIX );                                                                  
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::IntradayOddReject( OrderInfo* OrderRecord, const UFC::AnsiString& PVCID,int Func, int Error, BOOL IsFIX  )
{
    MessageHeader* RejectMsg;
    
    if( IsFIX == TRUE )
        RejectMsg = OrderRecord->ConfirmO120( PVCID );   
    else
        RejectMsg = new MessageT030( IsTSEC );
    RejectMsg->SetFunctionCode( Func );
    RejectMsg->SetStatusCode( Error );
    return RejectMsg;
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::NewOrder( MessageO110* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::AnsiString   StockID( Order->StockID, 6 );        
    int               Px = UFC::AnsiString( Order->OrderPrice, 9 ).ToInt();        
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;        
    if( (OrderRecord = FIntradayOddOrders.GetObjectByKey( BrokerID + OID )) == NULL )
    {
        int Result = CheckPx( StockID, Px );
        if( Result == 1 )
        {                
            OrderRecord = new OrderInfo( OID, Order, PVCID, BrkID, IsFIX );
            FIntradayOddOrders.Add( BrokerID + OID, OrderRecord );
            MessageO120* O120 = OrderRecord->ConfirmO120( PVCID );
            SetLastConfirm( PVCID, O120 );
            AddToMatchList( OrderRecord );
            return O120;
        }
        else if( Result == -1 )
        {
            OrderInfo  RejectOrderRecord( OID, Order, PVCID, BrkID, IsFIX );
            return IntradayOddReject( &RejectOrderRecord, PVCID, Order->GetFunctionCode(),20, IsFIX );
        }
        else //if( Result == 0 )
        {
            OrderInfo  RejectOrderRecord( OID, Order, PVCID, BrkID, IsFIX );
            return IntradayOddReject( &RejectOrderRecord, PVCID, Order->GetFunctionCode(),21, IsFIX );                    
        }
    }
    else ///< Order exists
    {
        OrderInfo  RejectOrderRecord( OID, Order, PVCID, BrkID, IsFIX );
        return IntradayOddReject( &RejectOrderRecord, PVCID, Order->GetFunctionCode(), 24, IsFIX );                    
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::NewOrder( MessageA010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::AnsiString   StockID( Order->StockID, 6 );        
    int               Px = UFC::AnsiString( Order->OrderPrice, 6 ).ToInt();        
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;        
    if( (OrderRecord = FAuctionOrders.GetObjectByKey( BrokerID + OID )) == NULL )
    {
        int Result = CheckPx( StockID, Px );
        if( Result == 1 )
        {                        
            OrderRecord = new OrderInfo( OID, Order );
            FAuctionOrders.Add( BrokerID + OID, OrderRecord );
            MessageA020* A020 = OrderRecord->ConfirmA020( PVCID );
            SetLastConfirm( PVCID, A020 );
            return A020;
        }
        else if( Result == -1 )
            return Reject( new MessageA030(),Order->GetFunctionCode(),20 );
        else //if( Result == 0 )
            return Reject( new MessageA030(),Order->GetFunctionCode(),21 );                    
    }
    else ///< Order exists
        return Reject( new MessageA030(),Order->GetFunctionCode(),22 );                    
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::NewOrder( MessageE010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::AnsiString   StockID( Order->StockID, 6 );        
    int               Px = UFC::AnsiString( Order->OrderPrice, 6 ).ToInt();            
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;        
    if( (OrderRecord = FTenderOrders.GetObjectByKey( BrokerID + OID )) == NULL )
    {
        int Result = CheckPx( StockID, Px );
        if( Result == 1 )
        {                        
            OrderRecord = new OrderInfo( OID, Order );
            FTenderOrders.Add( BrokerID + OID, OrderRecord );
            MessageE020* E020 = OrderRecord->ConfirmE020( PVCID );
            SetLastConfirm( PVCID, E020 );
            return E020;
        }
        else if( Result == -1 )
            return Reject( new MessageE030(IsTSEC),Order->GetFunctionCode(),20 );
        else //if( Result == 0 )
            return Reject( new MessageE030(IsTSEC),Order->GetFunctionCode(),21 );                    
    }
    else ///< Order exists
        return Reject( new MessageE030(IsTSEC),Order->GetFunctionCode(),22 );                    
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::NewOrder( MessageEx010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::AnsiString   StockID( Order->StockID, 6 );        
    int               Px = UFC::AnsiString( Order->OrderPrice, 6 ).ToInt();                
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;        
    if( (OrderRecord = FTenderExOrders.GetObjectByKey( BrokerID + OID )) == NULL )
    {
        int Result = CheckPx( StockID, Px );
        if( Result == 1 )
        {                        
            OrderRecord = new OrderInfo( OID, Order );
            FTenderExOrders.Add( BrokerID + OID, OrderRecord );
            MessageEx020* Ex020 = OrderRecord->ConfirmEx020( PVCID );
            SetLastConfirm( PVCID, Ex020 );
            return Ex020;
        }
        else if( Result == -1 )
            return Reject( new MessageEx030(IsTSEC),Order->GetFunctionCode(),20 );
        else //if( Result == 0 )
            return Reject( new MessageEx030(IsTSEC),Order->GetFunctionCode(),21 );                    
    }
    else ///< Order exists
        return Reject( new MessageEx030(IsTSEC),Order->GetFunctionCode(),22 );        
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::NewOrder( MessageV010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::AnsiString   StockID( Order->StockID, 6 );        
    int               Px = UFC::AnsiString( Order->OrderPrice, 6 ).ToInt();                
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;        
    if( (OrderRecord = FLendOrders.GetObjectByKey( BrokerID + OID )) == NULL )
    {
        int Result = CheckPx( StockID, Px );
        if( Result == 1 )
        {                        
            OrderRecord = new OrderInfo( OID, Order );
            FLendOrders.Add( BrokerID + OID, OrderRecord );
            MessageV020* V020 = OrderRecord->ConfirmV020( PVCID );
            SetLastConfirm( PVCID, V020 );
            return V020;
        }
        else if( Result == -1 )
            return Reject( new MessageV030(IsTSEC),Order->GetFunctionCode(),20 );
        else //if( Result == 0 )
            return Reject( new MessageV030(IsTSEC),Order->GetFunctionCode(),21 );                    
    }
    else ///< Order exists
        return Reject( new MessageV030(IsTSEC),Order->GetFunctionCode(),22 );                        
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::CancelOrder( MessageT010* Order, const UFC::AnsiString& PVCID, BOOL IsFIX )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );
    
    FOrderCount++;        
    if( (OrderRecord = FNormalOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
#ifndef SIMULATOR
        return NULL;
#else
        UFC::BufferedLog::Printf( " PVC:%s a normal order:%s canceled.",PVCID.c_str(),OID.c_str() );        
        MessageHeader* T020 = OrderRecord->CancelT020( PVCID );
        DeleteOrder( OrderRecord );        
        return T020;
#endif
    }
    else ///< Order not exists
    {
        UFC::BufferedLog::Printf( " PVC:%s cancel order:%s not exists.",PVCID.c_str(),OID.c_str() );                
        if( IsFIX == TRUE )
        {
            char X010[256];
            UFC::AnsiString X020;
        
            Order->AsString( X010 ); 
            X010ToX020( X010, 5, X020, true, true ); ///< Error Code 5, Order not found            
            return new MessageT020( X020.c_str() );        
        }
        else
        {        
            MessageT030* T030 = new MessageT030(IsTSEC);
            T030->SetFunctionCode( Order->GetFunctionCode() );
            T030->SetStatusCode( 5 ); ///< Error Code 5, Order not found
            return T030;        
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::CancelOrder( MessageP010* Order, const UFC::AnsiString& PVCID , BOOL IsFIX)
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;        
    if( (OrderRecord = FPostOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
#ifndef SIMULATOR
        return NULL;
#else
        MessageHeader* P020 = OrderRecord->CancelP020( PVCID );
        SetLastConfirm( PVCID, P020 );
        DeleteOrder( OrderRecord );
        return P020;
#endif
    }
    else ///< Order not exists
    {
        UFC::BufferedLog::Printf( " PVC:%s cancel order:%s not exists.",PVCID.c_str(),OID.c_str() );
        if( IsFIX == TRUE )
        {
            char X010[256];
            UFC::AnsiString X020;
        
            Order->AsString( X010 ); 
            X010ToX020( X010, 5, X020, true, true ); ///< Error Code 5, Order not found            
            return new MessageP020( X020.c_str() );        
        }
        else
        {        
            MessageP030* P030 = new MessageP030(IsTSEC);
            P030->SetFunctionCode( Order->GetFunctionCode() );
            P030->SetStatusCode( 5 );///< Order not found
            return P030;        
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::CancelOrder( MessageO010* Order, const UFC::AnsiString& PVCID, BOOL IsFIX )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;        
    if( (OrderRecord = FOddOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
#ifndef SIMULATOR
        return NULL;
#else
        MessageHeader* O020 = OrderRecord->CancelO020( PVCID );
        SetLastConfirm( PVCID, O020 );
        DeleteOrder( OrderRecord );
        return O020;
#endif
    }
    else ///< Order not exists
    {
        UFC::BufferedLog::Printf( " PVC:%s cancel order:%s not exists.",PVCID.c_str(),OID.c_str() );
        if( IsFIX == TRUE )
        {
            char X010[256];
            UFC::AnsiString X020;
        
            Order->AsString( X010 ); 
            X010ToX020( X010, 5, X020, true, true ); ///< Error Code 5, Order not found            
            return new MessageO020( X020.c_str() );        
        }
        else
        {        
            MessageO030* O030 = new MessageO030(IsTSEC);
            O030->SetFunctionCode( Order->GetFunctionCode() );
            O030->SetStatusCode( 24 );///< Order not found
            return O030;        
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::CancelOrder( MessageO110* Order, const UFC::AnsiString& PVCID, BOOL IsFIX )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );
    
    FOrderCount++;        
    if( (OrderRecord = FIntradayOddOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
        UFC::BufferedLog::Printf( " PVC:%s a Intra-day Odd order:%s canceled.",PVCID.c_str(),OID.c_str() );        
        MessageHeader* O120 = OrderRecord->CancelO120( PVCID );
        DeleteOrder( OrderRecord );        
        return O120;
    }
    else ///< Order not exists
    {
        UFC::BufferedLog::Printf( " PVC:%s cancel order:%s not exists.",PVCID.c_str(),OID.c_str() );        
        if( IsFIX == TRUE )
        {
            char X010[256];
            UFC::AnsiString X020;
        
            Order->AsString( X010 ); 
            X010ToX020( X010, 5, X020, true, true ); ///< Error Code 5, Order not found            
            return new MessageO120( X020.c_str() );        
        }
        else
        {        
            MessageO130* O130 = new MessageO130(IsTSEC);
            O130->SetFunctionCode( Order->GetFunctionCode() );
            O130->SetStatusCode( 5 ); ///< Order not found
            return O130;        
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::CancelOrder( MessageA010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;        
    if( (OrderRecord = FAuctionOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
#ifndef SIMULATOR
        return NULL;
#else
        MessageHeader* A020 = OrderRecord->CancelA020( PVCID );
        SetLastConfirm( PVCID, A020 );
        DeleteOrder( OrderRecord, FALSE );
        return A020;
#endif
    }
    else ///< Order not exists
    {
        MessageA030* A030 = new MessageA030();
        A030->SetFunctionCode( 2 );
        A030->SetStatusCode( 24 );///< Order not found
        return A030;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::CancelOrder( MessageE010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;        
    if( (OrderRecord = FTenderOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
#ifndef SIMULATOR
        return NULL;
#else
        MessageHeader* E020 = OrderRecord->CancelE020( PVCID );
        SetLastConfirm( PVCID, E020 );
        DeleteOrder( OrderRecord, FALSE );
        return E020;
#endif
    }
    else ///< Order not exists
    {
        MessageE030* E030 = new MessageE030(IsTSEC);
        E030->SetFunctionCode( 2 );
        E030->SetStatusCode( 24 );///< Order not found
        return E030;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::CancelOrder( MessageEx010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;        
    if( (OrderRecord = FTenderExOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
#ifndef SIMULATOR
        return NULL;
#else
        MessageHeader* Ex020 = OrderRecord->CancelEx020( PVCID );
        SetLastConfirm( PVCID, Ex020 );
        DeleteOrder( OrderRecord, FALSE );
        return Ex020;
#endif
    }
    else ///< Order not exists
    {
        MessageEx030* Ex030 = new MessageEx030(IsTSEC);
        Ex030->SetFunctionCode( 2 );
        Ex030->SetStatusCode( 24 );///< Order not found
        return Ex030;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::CancelOrder( MessageV010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;        
    if( (OrderRecord = FLendOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
#ifndef SIMULATOR
        return NULL;
#else
        MessageHeader* V020 = OrderRecord->CancelV020( PVCID );
        SetLastConfirm( PVCID, V020 );
        DeleteOrder( OrderRecord, FALSE );
        return V020;
#endif
    }
    else ///< Order not exists
    {
        MessageV030* V030 = new MessageV030(IsTSEC);
        V030->SetFunctionCode( 2 );
        V030->SetStatusCode( 17 );///< Order not found
        return V030;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::ReplaceOrder( MessageT010* Order, const UFC::AnsiString& PVCID, BOOL IsFIX )
{
    OrderInfo*        OrderRecord;      
    BOOL              Canceled;
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;     
    if( (OrderRecord = FNormalOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
#ifndef SIMULATOR
        return NULL;
#else
        MessageHeader* T020;
        if( Order->GetFunctionCode() == 3 ) ///< Reduce Qty
        {
            int ReduceQty = UFC::AnsiString( Order->OrderQty , 6 ).ToInt();
            
            UFC::BufferedLog::Printf( " PVC:%s a normal order:%s Quantity Replaced.",PVCID.c_str(),OID.c_str() );                    
            T020 = OrderRecord->ReplaceT020( PVCID, ReduceQty ,Canceled );        
            SetLastConfirm( PVCID, T020 );
            if( Canceled == TRUE )
                DeleteOrder( OrderRecord );
        }
        else ///< replace Price
        {
            int Rpx = UFC::AnsiString( Order->OrderPrice , 9 ).ToInt();
            
            UFC::BufferedLog::Printf( " PVC:%s a normal order:%s Price Replaced.",PVCID.c_str(),OID.c_str() );                    
            T020 = OrderRecord->ReplaceT020( PVCID, Rpx );        
            SetLastConfirm( PVCID, T020 );
        }
        return T020;
#endif
    }
    else ///< Order not exists
    {
        UFC::BufferedLog::Printf( " PVC:%s replace order:%s not exists.",PVCID.c_str(),OID.c_str() );        
        if( IsFIX == TRUE )
        {
            char X010[256];
            UFC::AnsiString X020;
        
            Order->AsString( X010 ); 
            X010ToX020( X010, 5, X020, true, true ); ///< Error Code 5, Order not found            
            return new MessageT020( X020.c_str() );        
        }
        else
        {        
            MessageT030* T030 = new MessageT030(IsTSEC);
            T030->SetFunctionCode( Order->GetFunctionCode() );
            T030->SetStatusCode( 5 ); ///< Order not found
            return T030;        
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::ReplaceOrder( MessageP010* Order, const UFC::AnsiString& PVCID, BOOL IsFIX )
{
    OrderInfo*        OrderRecord;      
    BOOL              Canceled;
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );
    
    FOrderCount++;    
    if( (OrderRecord = FPostOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
#ifndef SIMULATOR
        return NULL;
#else
        int ReduceQty = UFC::AnsiString( Order->OrderQty , 6 ).ToInt();
        MessageHeader* P020 = OrderRecord->ReplaceP020( PVCID, ReduceQty ,Canceled );        
        SetLastConfirm( PVCID, P020 );
        if( Canceled == TRUE )
            DeleteOrder( OrderRecord );        
        return P020;
#endif
    }
    else ///< Order not exists
    {
        if( IsFIX == TRUE )
        {
            char X010[256];
            UFC::AnsiString X020;
        
            Order->AsString( X010 ); 
            X010ToX020( X010, 5, X020, true, true ); ///< Error Code 5, Order not found            
            return new MessageP020( X020.c_str() );        
        }
        else
        {        
            MessageP030* P030 = new MessageP030(IsTSEC);
            P030->SetFunctionCode( Order->GetFunctionCode() );
            P030->SetStatusCode( 5 ); ///< Order not found
            return P030;        
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::ReplaceOrder( MessageO010* Order, const UFC::AnsiString& PVCID, BOOL IsFIX )
{
    OrderInfo*        OrderRecord;      
    BOOL              Canceled;
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;    
    if( (OrderRecord = FOddOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
#ifndef SIMULATOR
        return NULL;
#else
        int ReduceQty = UFC::AnsiString( Order->OrderQty , 6 ).ToInt();
        
        MessageHeader* O020 = OrderRecord->ReplaceO020( PVCID, ReduceQty ,Canceled );        
        SetLastConfirm( PVCID, O020 );
        if( Canceled == TRUE )
            DeleteOrder( OrderRecord );        
        return O020;
#endif
    }
    else ///< Order not exists
    {
        if( IsFIX == TRUE )
        {
            char X010[256];
            UFC::AnsiString X020;
        
            Order->AsString( X010 ); 
            X010ToX020( X010, 24, X020, true, true ); ///< Error Code 5, Order not found            
            return new MessageO020( X020.c_str() );        
        }
        else
        {        
            MessageO030* O030 = new MessageO030(IsTSEC);
            O030->SetFunctionCode( Order->GetFunctionCode() );
            O030->SetStatusCode( 24 ); ///< Order not found
            return O030;        
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::ReplaceOrder( MessageO110* Order, const UFC::AnsiString& PVCID, BOOL IsFIX )
{
    OrderInfo*        OrderRecord;      
    BOOL              Canceled;
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;    
    if( (OrderRecord = FIntradayOddOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        

        int ReduceQty = UFC::AnsiString( Order->OrderQty , 6 ).ToInt();
        
        MessageHeader* O120 = OrderRecord->ReplaceO120( PVCID, ReduceQty ,Canceled );        
        SetLastConfirm( PVCID, O120 );
        if( Canceled == TRUE )
            DeleteOrder( OrderRecord );        
        return O120;
    }
    else ///< Order not exists
    {
        if( IsFIX == TRUE )
        {
            char X010[256];
            UFC::AnsiString X020;
        
            Order->AsString( X010 ); 
            X010ToX020( X010, 24, X020, true, true ); ///< Error Code 5, Order not found            
            return new MessageO120( X020.c_str() );        
        }
        else
        {        
            MessageO130* O130 = new MessageO130(IsTSEC);
            O130->SetFunctionCode( Order->GetFunctionCode() );
            O130->SetStatusCode( 24 ); ///< Order not found
            return O130;        
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::ReplaceOrder( MessageA010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    BOOL              Canceled;
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;    
    if( (OrderRecord = FAuctionOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
#ifndef SIMULATOR
        return NULL;
#else
        int ReduceQty = UFC::AnsiString( Order->OrderQty , 12 ).ToInt();
        
        MessageHeader* A020 = OrderRecord->ReplaceA020( PVCID, ReduceQty ,Canceled );        
        SetLastConfirm( PVCID, A020 );
        if( Canceled == TRUE )
            DeleteOrder( OrderRecord, FALSE );        
        return A020;
#endif
    }
    else ///< Order not exists
    {
        MessageA030* A030 = new MessageA030();
        A030->SetFunctionCode( 3 );
        A030->SetStatusCode( 24 ); ///< Order not found
        return A030;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::ReplaceOrder( MessageE010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    BOOL              Canceled;
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;    
    if( (OrderRecord = FTenderOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
#ifndef SIMULATOR
        return NULL;
#else
        int ReduceQty = UFC::AnsiString( Order->OrderQty , 12 ).ToInt();
        
        MessageHeader* E020 = OrderRecord->ReplaceE020( PVCID, ReduceQty ,Canceled );        
        SetLastConfirm( PVCID, E020 );
        if( Canceled == TRUE )
            DeleteOrder( OrderRecord, FALSE );        
        return E020;
#endif
    }
    else ///< Order not exists
    {
        MessageE030* E030 = new MessageE030(IsTSEC);
        E030->SetFunctionCode( 3 );
        E030->SetStatusCode( 24 ); ///< Order not found
        return E030;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::ReplaceOrder( MessageEx010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    BOOL              Canceled;
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;    
    if( (OrderRecord = FTenderExOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
#ifndef SIMULATOR
        return NULL;
#else
        int ReduceQty = UFC::AnsiString( Order->OrderQty , 12 ).ToInt();
        
        MessageHeader* Ex020 = OrderRecord->ReplaceEx020( PVCID, ReduceQty ,Canceled );        
        SetLastConfirm( PVCID, Ex020 );
        if( Canceled == TRUE )
            DeleteOrder( OrderRecord, FALSE );        
        return Ex020;
#endif
    }
    else ///< Order not exists
    {
        MessageEx030* Ex030 = new MessageEx030(IsTSEC);
        Ex030->SetFunctionCode( 3 );
        Ex030->SetStatusCode( 24 ); ///< Order not found
        return Ex030;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::ReplaceOrder( MessageV010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    BOOL              Canceled;
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );

    FOrderCount++;    
    if( (OrderRecord = FLendOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
#ifndef SIMULATOR
        return NULL;
#else
        int ReduceQty = UFC::AnsiString( Order->OrderQty , 6 ).ToInt();
        
        MessageHeader* V020 = OrderRecord->ReplaceA020( PVCID, ReduceQty ,Canceled );        
        SetLastConfirm( PVCID, V020 );
        if( Canceled == TRUE )
            DeleteOrder( OrderRecord, FALSE );        
        return V020;
#endif
    }
    else ///< Order not exists
    {
        MessageV030* V030 = new MessageV030(IsTSEC);
        V030->SetFunctionCode( 3 );
        V030->SetStatusCode( 17 ); ///< Order not found
        return V030;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::QueryOrder( MessageT010* Order, const UFC::AnsiString& PVCID ) ///< In T010, out T020,T030
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );
    
    if( (OrderRecord = FNormalOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
        UFC::BufferedLog::Printf( " Query PVC:%s order:%s status.",PVCID.c_str(),OID.c_str() );                
        return OrderRecord->QueryT020( PVCID );
    }
    else ///< Order not exists
    {
        UFC::BufferedLog::Printf( " PVC:%s query order:%s not exists.",PVCID.c_str(),OID.c_str() );        
        MessageT030* T030 = new MessageT030(IsTSEC);
        T030->SetFunctionCode( Order->GetFunctionCode() );
        T030->SetStatusCode( 5 ); ///< Order not found
        return T030;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::QueryOrder( MessageP010* Order, const UFC::AnsiString& PVCID ) ///< In P010, out P020,P030
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );
    
    if( (OrderRecord = FPostOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
        UFC::BufferedLog::Printf( " Query PVC:%s order:%s status.",PVCID.c_str(),OID.c_str() );                
        return OrderRecord->QueryP020( PVCID );
    }
    else ///< Order not exists
    {
        UFC::BufferedLog::Printf( " PVC:%s query order:%s not exists.",PVCID.c_str(),OID.c_str() );        
        MessageP030* P030 = new MessageP030(IsTSEC);
        P030->SetFunctionCode( Order->GetFunctionCode() );
        P030->SetStatusCode( 5 ); ///< Order not found
        return P030;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::QueryOrder( MessageO010* Order, const UFC::AnsiString& PVCID ) ///< In O010, out O020,O030
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );
    
    if( (OrderRecord = FOddOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
        UFC::BufferedLog::Printf( " Query PVC:%s order:%s status.",PVCID.c_str(),OID.c_str() );                
        return OrderRecord->QueryO020( PVCID );
    }
    else ///< Order not exists
    {
        UFC::BufferedLog::Printf( " PVC:%s query order:%s not exists.",PVCID.c_str(),OID.c_str() );        
        MessageO030* O030 = new MessageO030(IsTSEC);
        O030->SetFunctionCode( Order->GetFunctionCode() );
        O030->SetStatusCode( 24 ); ///< Order not found
        return O030;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::QueryOrder( MessageO110* Order, const UFC::AnsiString& PVCID ) ///< In O010, out O020,O030
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );
    
    if( (OrderRecord = FIntradayOddOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
        UFC::BufferedLog::Printf( " Query PVC:%s order:%s status.",PVCID.c_str(),OID.c_str() );                
        return OrderRecord->QueryO120( PVCID );
    }
    else ///< Order not exists
    {
        UFC::BufferedLog::Printf( " PVC:%s query order:%s not exists.",PVCID.c_str(),OID.c_str() );        
        MessageO130* O130 = new MessageO130(IsTSEC);
        O130->SetFunctionCode( Order->GetFunctionCode() );
        O130->SetStatusCode( 24 ); ///< Order not found
        return O130;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::QueryOrder( MessageA010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );
    
    if( (OrderRecord = FAuctionOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
        UFC::BufferedLog::Printf( " Query PVC:%s order:%s status.",PVCID.c_str(),OID.c_str() );                
        return OrderRecord->QueryA020( PVCID );
    }
    else ///< Order not exists
    {
        UFC::BufferedLog::Printf( " PVC:%s query order:%s not exists.",PVCID.c_str(),OID.c_str() );        
        MessageA030* A030 = new MessageA030();
        A030->SetFunctionCode( 4 );
        A030->SetStatusCode( 24 ); ///< Order not found
        return A030;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::QueryOrder( MessageE010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );
    
    if( (OrderRecord = FTenderOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
        UFC::BufferedLog::Printf( " Query PVC:%s order:%s status.",PVCID.c_str(),OID.c_str() );                
        return OrderRecord->QueryE020( PVCID );
    }
    else ///< Order not exists
    {
        UFC::BufferedLog::Printf( " PVC:%s query order:%s not exists.",PVCID.c_str(),OID.c_str() );        
        MessageE030* E030 = new MessageE030(IsTSEC);
        E030->SetFunctionCode( 4 );
        E030->SetStatusCode( 24 ); ///< Order not found
        return E030;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::QueryOrder( MessageEx010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );
    
    if( (OrderRecord = FTenderExOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
        UFC::BufferedLog::Printf( " Query PVC:%s order:%s status.",PVCID.c_str(),OID.c_str() );                
        return OrderRecord->QueryEx020( PVCID );
    }
    else ///< Order not exists
    {
        UFC::BufferedLog::Printf( " PVC:%s query order:%s not exists.",PVCID.c_str(),OID.c_str() );        
        MessageEx030* Ex030 = new MessageEx030(IsTSEC);
        Ex030->SetFunctionCode( 4 );
        Ex030->SetStatusCode( 24 ); ///< Order not found
        return Ex030;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::QueryOrder( MessageV010* Order, const UFC::AnsiString& PVCID )
{
    OrderInfo*        OrderRecord;      
    UFC::AnsiString   OID( Order->OrderNo, 5 );
    UFC::AnsiString   BrokerID( Order->BrokerID, 4 );    
    UFC::PLockObject  UpdateLock( FCS );
    
    if( (OrderRecord = FLendOrders.GetObjectByKey( BrokerID + OID )) != NULL )
    {        
        UFC::BufferedLog::Printf( " Query PVC:%s order:%s status.",PVCID.c_str(),OID.c_str() );                
        return OrderRecord->QueryV020( PVCID );
    }
    else ///< Order not exists
    {
        UFC::BufferedLog::Printf( " PVC:%s query order:%s not exists.",PVCID.c_str(),OID.c_str() );        
        MessageV030* V030 = new MessageV030(IsTSEC);
        V030->SetFunctionCode( 4 );
        V030->SetStatusCode( 17 ); ///< Order not found
        return V030;        
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::GetLastConfirm( const UFC::AnsiString& PVCID )
{
    char* Buffer;
    UFC::PLockObject Lock( FLastConfirmCS );
    
    if( (Buffer = FLastConfirm.GetObjectByKey( PVCID )) != NULL )
        return MessageHeader::CreateMessage( Buffer );
    return NULL;
}
//----------------------------------------------------------------------------------------------------------------------
void TransactionObject::SetLastConfirm( const UFC::AnsiString& PVCID , MessageHeader* Confirm )
{
    char* Buffer;
    UFC::PLockObject Lock( FLastConfirmCS );
    
    if( (Buffer = FLastConfirm.GetObjectByKey( PVCID )) == NULL )
    {
        Buffer = new char[ 256 ];
        FLastConfirm.Add( PVCID, Buffer );
    }
    Confirm->AsString( Buffer );    
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::ReconnectQuery( TradeSession Session, const UFC::AnsiString& PVCID )
{
    MessageHeader* Reply;
    
    if( (Reply = GetLastConfirm( PVCID )) == NULL )
    {    
        switch( Session )
        {
            case tsNormal:   Reply = new MessageT030(IsTSEC); break;
            case tsOdd:      Reply = new MessageO030(IsTSEC); break;
            case tsOddEx:    Reply = new MessageO130(IsTSEC); break;
            case tsPost:     Reply = new MessageP030(IsTSEC); break;
            case tsAuction:  Reply = new MessageA030(); break;
            case tsTender:   Reply = new MessageE030(IsTSEC); break;
            case tsTenderEx: Reply = new MessageEx030(IsTSEC); break;
            case tsLend:     Reply = new MessageV030(IsTSEC); break;
            default:         return NULL;
        }
        Reply->SetFunctionCode( 1 );
        Reply->SetStatusCode( 24 );///< Order not found
    }
    return Reply;        
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::HandleNormalOrder( MessageT010* Order, const UFC::AnsiString& PVCID,const UFC::AnsiString& BrkID,BOOL IsFIX)
{
    switch( Order->GetFunctionCode() )
    {
        case 1: 
        case 2: return NewOrder( Order, PVCID, BrkID, IsFIX  );
        case 3:
        case 6: return ReplaceOrder( Order, PVCID, IsFIX );
        case 4: return CancelOrder( Order, PVCID, IsFIX );
        case 5: return QueryOrder( Order, PVCID );
        default: return NULL; 
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::HandlePostOrder( MessageP010* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX )
{
    switch( Order->GetFunctionCode() )
    {
        case 1: 
        case 2: return NewOrder( Order, PVCID, BrkID, IsFIX );
        case 3: return ReplaceOrder( Order, PVCID, IsFIX );
        case 4: return CancelOrder( Order, PVCID, IsFIX );
        case 5: return QueryOrder( Order, PVCID );
        default: return NULL;  
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::HandleOddOrder( MessageO010* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX )
{
    switch( Order->GetFunctionCode() )
    {
        case 1: 
        case 2: return NewOrder( Order, PVCID, BrkID, IsFIX );
        case 3: return ReplaceOrder( Order, PVCID, IsFIX );
        case 4: return CancelOrder( Order, PVCID, IsFIX );
        case 5: return QueryOrder( Order, PVCID );
        default: return NULL;  
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::HandleIntradayOddOrder( MessageO110* Order, const UFC::AnsiString& PVCID,const UFC::AnsiString& BrkID,BOOL IsFIX)
{
    switch( Order->GetFunctionCode() )
    {
        case 1: 
        case 2: return NewOrder( Order, PVCID, BrkID, IsFIX  );
        case 3: return ReplaceOrder( Order, PVCID, IsFIX );
        case 4: return CancelOrder( Order, PVCID, IsFIX );                
        case 5: return QueryOrder( Order, PVCID );
        default: return NULL; 
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::HandleAuctionOrder( MessageA010* Order, const UFC::AnsiString& PVCID )
{
    switch( Order->GetFunctionCode() )
    {
        case 1: return NewOrder( Order, PVCID );
        case 2: return CancelOrder( Order, PVCID );        
        case 3: return ReplaceOrder( Order, PVCID );
        case 4: return QueryOrder( Order, PVCID );
        default: return NULL;  
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::HandleTenderOrder( MessageE010* Order, const UFC::AnsiString& PVCID )
{
    switch( Order->GetFunctionCode() )
    {
        case 1: return NewOrder( Order, PVCID );
        case 2: return CancelOrder( Order, PVCID );        
        case 3: return ReplaceOrder( Order, PVCID );
        case 4: return QueryOrder( Order, PVCID );
        default: return NULL;  
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::HandleTenderExOrder( MessageEx010* Order, const UFC::AnsiString& PVCID )
{
    switch( Order->GetFunctionCode() )
    {
        case 1: return NewOrder( Order, PVCID );
        case 2: return CancelOrder( Order, PVCID );        
        case 3: return ReplaceOrder( Order, PVCID );
        case 4: return QueryOrder( Order, PVCID );
        default: return NULL;  
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::HandleLendOrder( MessageV010* Order, const UFC::AnsiString& PVCID )
{
    switch( Order->GetFunctionCode() )
    {
        case 1: return NewOrder( Order, PVCID );
        case 2: return CancelOrder( Order, PVCID );        
        case 3: return ReplaceOrder( Order, PVCID );
        case 4: return QueryOrder( Order, PVCID );
        default: return NULL;  
    }
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::CheckMarketState( TradeSession Session, int Func )
{
    MessageHeader* Reply = NULL;
    
    if( FOpenSwitch < OS_OPEN_MARKET ) ///< Market not open
    {
        switch( Session )
        {
            case tsNormal: Reply = new MessageT030(IsTSEC); break;
            case tsOdd:    Reply = new MessageO030(IsTSEC); break;
            case tsPost:   Reply = new MessageP030(IsTSEC); break;
            case tsOddEx:  Reply = new MessageO130(IsTSEC); break;
            case tsAuction:
            case tsTender:
            case tsTenderEx:
            case tsLend:   
            default:       return NULL;               
        }
        Reply->SetFunctionCode( Func );
        Reply->SetStatusCode( 2 );///< Not open yet.
        return Reply;
    }
    else if( FOpenSwitch > OS_OPEN_MARKET ) ///< Market closed
    {        
        switch( Session )
        {
            case tsNormal: Reply = new MessageT030(IsTSEC); break;
            case tsOdd:    Reply = new MessageO030(IsTSEC); break;
            case tsPost:   Reply = new MessageP030(IsTSEC); break;
            case tsOddEx:  Reply = new MessageO130(IsTSEC); break;
            case tsAuction:
            case tsTender:
            case tsTenderEx:
            case tsLend:   
            default:       return NULL;               
        }
        Reply->SetFunctionCode( Func );
        Reply->SetStatusCode( 1 );///< Order not found
        return Reply;
    }
    else
        return NULL;
}
//----------------------------------------------------------------------------------------------------------------------
MessageHeader* TransactionObject::TransactionObject::OnMessage( ConnectionObjectBase* Connection, MessageHeader* Order, BOOL IsFIX )
{
    MessageHeader*       Reply;
    ConnectionParameter* cp = Connection->GetConnectionParameter();
    UInt32               Tick = UFC::GetTickCountMS();
        
    switch( Order->MessageType() )
    {        
        ///< Handle Heartbeat message.
        case mtT040: if( CurrentSession != tsNormal ) 
                         throw( X25ReconnectException( "Wrong trading session, Expect normal session." ) );
                     return new MessageT050( IsTSEC);
        ///< Handle query last confirm message.        
        case mtT060: if( CurrentSession != tsNormal ) 
                         throw( X25ReconnectException( "Wrong trading session, Expect normal session." ) );
                     return ReconnectQuery( tsNormal,cp->GetPVCID() );
        ///< Handle order messages.( New/Cancel/Replace and query order state messages).
        case mtT010:  if( CurrentSession != tsNormal ) 
                         throw( X25ReconnectException( "Wrong trading session, Expect normal session." ) );
                     if( (Reply = CheckMarketState( tsNormal, Order->GetFunctionCode( ))) == NULL )
                     {
                          Reply = HandleNormalOrder( (MessageT010*)Order, cp->GetPVCID(), cp->GetBrokerID(), IsFIX );
                          UpdateStates( Tick );
                     }
                     return Reply;
        case mtP040: if( CurrentSession != tsPost ) 
                         throw( X25ReconnectException( "Wrong trading session, Expect Off-hour session." ) );
                     return new MessageP050( IsTSEC);                     
        case mtP060: if( CurrentSession != tsPost ) 
                         throw( X25ReconnectException( "Wrong trading session, Expect Off-hour session." ) );
                     return ReconnectQuery( tsPost, cp->GetPVCID() );
        case mtP010: if( IsFIX == false && CurrentSession != tsPost ) 
                         throw( X25ReconnectException( "Wrong trading session, Expect Off-hour session." ) );                     
                     if( (Reply = CheckMarketState( tsPost, Order->GetFunctionCode( ))) == NULL )
                     {
                         Reply = HandlePostOrder( (MessageP010*)Order, cp->GetPVCID(), cp->GetBrokerID(), IsFIX );
                         UpdateStates( Tick );
                     }
                     return Reply;
        case mtO090: if( CurrentSession != tsOdd ) 
                         throw( X25ReconnectException( "Wrong trading session, Expect Odd lot session." ) );
                     return ReconnectQuery( tsOdd, cp->GetPVCID() ); ///< Reconnection query                     
        case mtO070: if( CurrentSession != tsOdd ) 
                         throw( X25ReconnectException( "Wrong trading session, Expect Odd lot session." ) );
                     return new MessageO080( IsTSEC);                      
        case mtO010: if( IsFIX == false && CurrentSession != tsOdd ) 
                         throw( X25ReconnectException( "Wrong trading session, Expect Odd lot session." ) );
                     if( (Reply = CheckMarketState( tsOdd, Order->GetFunctionCode( ))) == NULL )
                     {
                        Reply = HandleOddOrder( (MessageO010*)Order, cp->GetPVCID(), cp->GetBrokerID(), IsFIX );
                        UpdateStates( Tick );
                     }
                     return Reply;        
        ///< Handle Heartbeat message.
        case mtO140: if( CurrentSession != tsNormal ) 
                         throw( X25ReconnectException( "Wrong trading session, Expect normal session." ) );
                     return new MessageO150( IsTSEC);
        ///< Handle query last confirm message.        
        case mtO160: if( CurrentSession != tsNormal ) 
                         throw( X25ReconnectException( "Wrong trading session, Expect normal session." ) );
                     return ReconnectQuery( tsOddEx,cp->GetPVCID() );
        ///< Handle order messages.( New/Cancel/Replace and query order state messages).
        case mtO110:  if( CurrentSession != tsNormal ) 
                         throw( X25ReconnectException( "Wrong trading session, Expect normal session." ) );
                     if( (Reply = CheckMarketState( tsOddEx, Order->GetFunctionCode( ))) == NULL )
                     {
                          Reply = HandleIntradayOddOrder( (MessageO110*)Order, cp->GetPVCID(), cp->GetBrokerID(), IsFIX );
                          UpdateStates( Tick );
                     }
                     return Reply;                     
        case mtA040: return new MessageA050( );                     
        case mtA060: return ReconnectQuery( tsAuction, cp->GetPVCID() );
        case mtA010: Reply = HandleAuctionOrder( (MessageA010*)Order, cp->GetPVCID() );
                     UpdateStates( Tick );                     
                     return Reply;                     
        case mtE040: return new MessageE050( IsTSEC);                     
        case mtE060: return ReconnectQuery( tsTender, cp->GetPVCID() );
        case mtE010: Reply = HandleTenderOrder( (MessageE010*)Order, cp->GetPVCID() );
                     UpdateStates( Tick );                     
                     return Reply;                                          
        case mtEx040: return new MessageEx050( IsTSEC);                     
        case mtEx060: return ReconnectQuery( tsTenderEx, cp->GetPVCID() );
        case mtEx010: Reply = HandleTenderExOrder( (MessageEx010*)Order, cp->GetPVCID() );
                      UpdateStates( Tick );                     
                      return Reply;                                                               
        case mtV040: return new MessageV050( IsTSEC);                     
        case mtV060: return ReconnectQuery( tsLend, cp->GetPVCID() );
        case mtV010: Reply = HandleLendOrder( (MessageV010*)Order, cp->GetPVCID() );
                     UpdateStates( Tick );                     
                     return Reply;   
        case mtS130: return new MessageS140( );                                          
        ///< Unknown essage
        default : return NULL;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TransactionObject::UpdateStates( UInt32 Tick )
{
    UInt32 Interval = UFC::GetTickCountMS() - Tick;
    
    FTotalResponseTime += Interval;
    if( Interval > (UInt32)FMaxResponseTime )
        FMaxResponseTime = Interval;    
}
//----------------------------------------------------------------------------------------------------------------------
void TransactionObject::SetOpenSwitch( int OpenSwitch )
{
    if( FOpenSwitch != OpenSwitch )
        FOpenSwitch = OpenSwitch;
}
//----------------------------------------------------------------------------------------------------------------------
void TransactionObject::GetStates( OrderStates* States )
{
    States->MaxOrderResponseTime       = FMaxResponseTime;
    States->CurrentOrderCount          = FOrderCount;    
    if( FOrderCount > 0 )
        States->OrderAverageResponseTime   = FTotalResponseTime / FOrderCount;    
}
//----------------------------------------------------------------------------------------------------------------------
void TransactionObject::DeleteOrder( OrderInfo* Order, BOOL DelOrderBook )
{
    if( DelOrderBook == TRUE )
        DeleteFromMatchList( Order );
    switch( Order->FSession )
    {
        case tsNormal:  FNormalOrders.DeleteByKey( Order->FBrokerID + Order->FOID );break;
        case tsPost:    FPostOrders.DeleteByKey( Order->FBrokerID + Order->FOID );break;
        case tsOdd:     FOddOrders.DeleteByKey( Order->FBrokerID + Order->FOID );break;
        case tsOddEx:   FIntradayOddOrders.DeleteByKey( Order->FBrokerID + Order->FOID );break;
        case tsAuction: FAuctionOrders.DeleteByKey( Order->FBrokerID + Order->FOID );break;
        case tsTender:  FTenderOrders.DeleteByKey( Order->FBrokerID + Order->FOID );break;
        case tsTenderEx:FTenderExOrders.DeleteByKey( Order->FBrokerID + Order->FOID );break;
        case tsLend:    FLendOrders.DeleteByKey( Order->FBrokerID + Order->FOID );break;
        default:       return;               
    }
    delete Order;
}
//----------------------------------------------------------------------------------------------------------------------
void TransactionObject::AddToMatchList( OrderInfo* Order )
{
    FOrderBook.Add( Order );  ///< Add to Match list
    FOIDs.Add( Order->FBrokerID + Order->FOID ); ///< Add OID to hashed set
}
//----------------------------------------------------------------------------------------------------------------------
void TransactionObject::DeleteFromMatchList( OrderInfo* Order )
{
    if( FOIDs.Exists( Order->FBrokerID + Order->FOID ) == TRUE ) ///< In Match list??
    {
        FOIDs.Delete( Order->FBrokerID + Order->FOID ); ///< remove OID from hashed set
        for( register int i = 0; i < FOrderBook.ItemCount(); i++ )
        {
            if( FOrderBook.GetItem(i)->FOID      == Order->FOID &&
                FOrderBook.GetItem(i)->FBrokerID == Order->FBrokerID )
            {
                FOrderBook.Delete( i );
                return;
            }
        }        
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TransactionObject::SendFIXFill( OrderInfo* MatchOrder, char* Msg, BOOL Filled )
{   
    
    R3Body*    FillMsg;
    int        Offset;    
    
    if( (FillMsg = R3Body::CreateFromString( Msg, Offset ) ) != NULL )
    {
        UFC::AnsiString TargetCompID,SenderCompID;
        
        if( IsTSEC == TRUE )
        {
            TargetCompID.Printf( "T%s%s", MatchOrder->FLineBrokerID.c_str(), MatchOrder->FPVCID.c_str() );
            SenderCompID = "XTAI";
        }
        else
        {
            TargetCompID.Printf( "O%s%s", MatchOrder->FLineBrokerID.c_str(), MatchOrder->FPVCID.c_str() );
            SenderCompID = "ROCO";
        }    
        FIX::SessionID  sessionID( FIX::BeginString_FIX44, SenderCompID.c_str(), TargetCompID.c_str());        
        TWSEConnection* FIXClient = FManager->GetFIXManager()->GetFIXClient( sessionID );    
        
        if( FIXClient != NULL )
        {
            FIXClient->SendFIXFill( FillMsg, MatchOrder, Filled );
            UFC::SleepMS( 1 ); 
        }
        delete FillMsg;
    }
}
//-----------------------------------------------------------------------------------------
int TransactionObject::Match()
{
    if( MatchBySetting == FALSE )
        return MatchAuto();
    else
        return MatchByRule();
}
//----------------------------------------------------------------------------------------------------------------------
int TransactionObject::MatchAuto()
{    
    BOOL                       Canceled;
    BOOL                       ExecCount = 0;
    StockInfo*                 info;
    OrderInfo*                 MatchOrder;
    char                       Buffer[ 256 ];
    UFC::PtrList< OrderInfo >  TempOrderBook;    

    UFC::PLockObject  UpdateLock( FCS );
    
    for( register int i = 0; i < FOrderBook.ItemCount(); i++ )
    {        
        MatchOrder = FOrderBook.GetItem(i);
        ///< Market Order -> Filled
        ///< At the moncy -> Fill 1, and continue fill 1 until filled.
        if( FCheckPrice && (info = FStocks.GetObjectByKey(MatchOrder->FSymbol))!= NULL )
        {   
            ///< It's a market order, Filled             
            if( MatchOrder->FPrice == info->TPrice ||MatchOrder->FPrice == info->BPrice || MatchOrder->FOrdType == '1'  )
            {   
                MatchOrder->Fill( Buffer, FFillSeq, FMatchedOrder );
                Executions.Add( Buffer );
                if( MatchOrder->FIsFIX == TRUE )
                    SendFIXFill( MatchOrder, Buffer, TRUE );                    
                DeleteOrder( MatchOrder, FALSE );            
                FFillSeq++;
                ExecCount++;
                FMatchedOrder++;
                continue;
            }
            else if( MatchOrder->FPrice == info->CPrice )
            {   ///< It's a at the money order, Fill 1.      
                MatchOrder->PartialFill( Buffer, FFillSeq, FMatchedOrder, 1, Canceled);
                Executions.Add( Buffer );
                if( MatchOrder->FIsFIX == TRUE )
                    SendFIXFill( MatchOrder, Buffer, Canceled );                    
                if( Canceled == TRUE )                 
                    DeleteOrder( MatchOrder, FALSE ); ///< Filled.
                else
                    TempOrderBook.Add( MatchOrder );
                FFillSeq++;
                ExecCount++;
                FMatchedOrder++;
                continue;
            }
        }        
        switch( FMatchedOrder%3 )
        {
            ///< No match.
            case 0: break; 
            ///< Partial fill.
            case 1: MatchOrder->PartialFill( Buffer, FFillSeq,FMatchedOrder, Canceled);
                    Executions.Add( Buffer );
                    if( MatchOrder->FIsFIX == TRUE )
                        SendFIXFill( MatchOrder, Buffer, Canceled );                    
                    if( Canceled == TRUE ) 
                        DeleteOrder( MatchOrder, FALSE );
                    FFillSeq++;
                    ExecCount++;
                    break; 
            ///< Filled.
            case 2: MatchOrder->Fill( Buffer, FFillSeq,FMatchedOrder);
                    Executions.Add( Buffer );
                    if( MatchOrder->FIsFIX == TRUE )
                        SendFIXFill( MatchOrder, Buffer, TRUE );                    
                    DeleteOrder( MatchOrder, FALSE );            
                    FFillSeq++;
                    ExecCount++;
                    break; ///< Filled.            
        }        
        FMatchedOrder++;
    }
    ///< Clear all orders in match list.
    FOrderBook.Clear();
    FOIDs.Clear();    
    if( ExecCount > 0 )
        UFC::BufferedLog::Printf(" Match finished, %d fill execution.", ExecCount );    
    ///< Order with at the money price, will join the next match.
    for( int i = 0; i < TempOrderBook.ItemCount(); i++ )
    {
        if( (MatchOrder = TempOrderBook.GetItem(i))!= NULL )        
            AddToMatchList( MatchOrder );        
    }
    return ExecCount;    
}
//-----------------------------------------------------------------------------------------
int TransactionObject::MatchByRule()
{
    BOOL                       Canceled;
    BOOL                       ExecCount = 0;    
    OrderInfo*                 MatchOrder;
    char                       Buffer[ 256 ];
    OrderMatchInfo*            OInfo;  
    UFC::PtrList< OrderInfo >  TempOrderBook;    

    UFC::PLockObject  UpdateLock( FCS );
    
    for( register int i = 0; i < FOrderBook.ItemCount(); i++ )
    {   
        UFC::AnsiString RuleKey;
        
        MatchOrder = FOrderBook.GetItem(i);
        RuleKey = MatchOrder->FBrokerID + MatchOrder->FOID;
        ///< Has match rule?
        if( gMatchRule.GetObjectByKey( RuleKey, OInfo ) == TRUE )
        {
            if( MatchOrder->IsSetMatchRule() == FALSE )
                MatchOrder->SetMatchRule( OInfo->MatchSec, OInfo->MatchQty, OInfo->MatchPx );
            if( MatchOrder->CountDown() <= 0 )
            {
                MatchOrder->FillByRule( Buffer, FFillSeq, FMatchedOrder, Canceled);                
                Executions.Add( Buffer );
                if( MatchOrder->FIsFIX == TRUE )
                    SendFIXFill( MatchOrder, Buffer, Canceled );                    
                if( Canceled == TRUE )                 
                    DeleteOrder( MatchOrder, FALSE ); ///< Filled.                
                FFillSeq++;
                ExecCount++;
                FMatchedOrder++;
            }
            else ///< Not matched, Join the next matched.
                TempOrderBook.Add( MatchOrder );
        }
    }
    ///< Clear all orders in match list.
    FOrderBook.Clear();
    FOIDs.Clear();    
    if( ExecCount > 0 )
        UFC::BufferedLog::Printf(" Match finished, %d fill execution.", ExecCount );    
    ///< Order with at the money price, will join the next match.
    for( int i = 0; i < TempOrderBook.ItemCount(); i++ )
    {
        if( (MatchOrder = TempOrderBook.GetItem(i))!= NULL )        
            AddToMatchList( MatchOrder );        
    }
    return ExecCount;        
}
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
