  //---------------------------------------------------------------------------
#include "OrcTFXMapper.h"
//---------------------------------------------------------------------------
void OrcTFXMapper::FillOrderStatus( ClientOrderID& CNID, FIX::ExecType& ExecType, FIX::OrdStatus& OrdStatus )
{
    switch( CNID.GetTransactionKind() )
    {
        case tkNewOrder:     ExecType  = FIX::ExecType_NEW;
                             OrdStatus = FIX::OrdStatus_NEW;
                             UFC::BufferedLog::DebugPrintf( "Send back the NEW ExecutionReport\n");
                             break;
        case tkOrderCancel:  ExecType  = FIX::ExecType_PENDING_CANCEL;
                             OrdStatus = FIX::OrdStatus_PENDING_CANCEL;
                             UFC::BufferedLog::DebugPrintf( "Send back the PENDING CANCEL ExecutionReport\n");
                             break;
        case tkOrderReplace: ExecType  = FIX::ExecType_PENDING_REPLACE;
                             OrdStatus = FIX::OrdStatus_PENDING_REPLACE;
                             UFC::BufferedLog::DebugPrintf( "Send back the PENDING REPLACE ExecutionReport\n");
                             break;
        default:             ExecType  = FIX::ExecType_STOPPED;
                             OrdStatus = FIX::OrdStatus_STOPPED;
                             break;
    }
}
//---------------------------------------------------------------------------
void OrcTFXMapper::ReplyConfirmExecution( OrderRecord* OrgOrd, FIX::ExecType ExecType, FIX::OrdStatus OrdStatus )
{
    FIX::ExecID ExecID( FIXMapperApp::GenerateExecID().c_str() );

    FIX44::ExecutionReport report( FIX::OrderID( OrgOrd->GetOrderID().c_str() ),
                                   ExecID,
                                   ExecType,
                                   OrdStatus,
                                   FIX::Side( OrgOrd->GetSide()),
                                   FIX::LeavesQty( OrgOrd->GetLeavesQty() ),
                                   FIX::CumQty( OrgOrd->GetCumQty() ),
                                   FIX::AvgPx( 0 ) );
    if( OrgOrd->IsMarketOrder() == FALSE )
        report.set( FIX::Price( OrgOrd->GetPrice() ) ); ///< Set price TAG
    report.set( FIX::OrdType(((OrgOrd->IsMarketOrder())? FIX::OrdType_MARKET:FIX::OrdType_LIMIT)) );
	report.set( FIX::SecurityID( static_cast<TSCTFXOrderRecord*>(OrgOrd)->GetSecurityID().c_str() ) );
    report.set( FIX::Symbol(OrgOrd->GetSymbol().c_str() ) );
    report.set( FIX::OrderQty( OrgOrd->GetOrderQty() ) );
    if( ExecType ==  FIX::ExecType_NEW )
    {
        report.set( FIX::ClOrdID(OrgOrd->GetClOrdID().c_str()));///< Its a new order.
    }
    else
    {
        report.set( FIX::OrigClOrdID( OrgOrd->GetOrigClOrdID().c_str() ) );///< Its a replace/cancel order.
        report.set( FIX::ClOrdID( OrgOrd->GetLastClOrdID().c_str()) );
    }
    Application->SendToimFIX( OrgOrd->GetSender(), report );
}
//---------------------------------------------------------------------------
// Receive NewOrder/Cancel/Replace confirm from Exchanges.
// and then map to FIX ExecutionReport (MsgType = 8 )
//
// Function onFuturesConfirm: Handle TAIFEX futures order confirm.
// Function onOptionsConfirm: Handle TAIFEX Option order confirm.
// Function onStockConfirm:   Handle TSE order confirm.
//---------------------------------------------------------------------------
void OrcTFXMapper::onFuturesConfirm( Int32 NID, ConfirmParser* Confirm )
{
    ClientOrderID   CNID( NID );
    FIX::ExecType   ExecType;
    FIX::OrdStatus  OrdStatus;
    OrderRecord*    OrgOrd;

    UFC::BufferedLog::DebugPrintf( "Futures ");
    if( (OrgOrd = FStore->GetOrder( NID ))!= NULL )
    {
        if( FIXMapper::UseSimTX == TRUE )
        {
            FillOrderStatus( CNID, ExecType, OrdStatus );
            if( CNID.GetTransactionKind() == tkNewOrder )
            {
                if( OrgOrd->GetState() != osPendingNew )
                    return;                    ///< Duplicate Order confirm
                FStore->AddRef( mkFutures, Confirm->GetOrderID(),OrgOrd );
                OrgOrd->SetOrderID( Confirm->GetOrderID() );///< Set the OrderID to OrderRecoder.
                OrgOrd->SetState( osNew );     ///< change order state
                FStore->UpdateOrder( OrgOrd ); ///< Sync state to file.
            }
            ReplyConfirmExecution( OrgOrd, ExecType, OrdStatus );
        }
        else
        {
            if( Confirm->GetTransactionKind() == tkNewOrder )
            {
                if( OrgOrd->GetState() != osPendingNew )
                    return;                    ///< Duplicate Order confirm
                FStore->AddRef( mkFutures, Confirm->GetOrderID(),OrgOrd );
                OrgOrd->SetOrderID( Confirm->GetOrderID() );///< Set the OrderID to OrderRecoder.
                OrgOrd->SetState( osNew );     ///< change order state
                FStore->UpdateOrder( OrgOrd ); ///< Sync state to file.
                ReplyConfirmExecution( OrgOrd, FIX::ExecType_NEW, FIX::OrdStatus_NEW );
            }
            else if( Confirm->GetTransactionKind() == tkOrderCancel )
            {
                if( OrgOrd->GetState() == osCanceled )
                    return;                     ///< Duplicate Order confirm
                OrgOrd->SetState( osCanceled ); ///< change order state
                FStore->UpdateOrder( OrgOrd );  ///< Sync state to file.
                ReplyConfirmExecution( OrgOrd, FIX::ExecType_CANCELED, FIX::OrdStatus_CANCELED );
            }
            else if( Confirm->GetTransactionKind() == tkOrderReplace )
            {
                OrgOrd->ReduceQty( Confirm->GetQty() );
                if( OrgOrd->GetLeavesQty( ) == 0 )
                {
                    OrdStatus = FIX::OrdStatus_FILLED;
                    OrgOrd->SetState( osFilled );
                }
                else if( OrgOrd->GetCumQty() == 0 )
                {
                    OrdStatus = FIX::OrdStatus_NEW;
                    OrgOrd->SetState( osNew );
                }
                else
                {
                    OrdStatus = FIX::OrdStatus_PARTIALLY_FILLED;
                    OrgOrd->SetState( osPartialFilled );
                }
                ReplyConfirmExecution( OrgOrd, FIX::ExecType_REPLACE, OrdStatus );
                OrgOrd->SetOrigClOrdID( OrgOrd->GetLastClOrdID().c_str() );
                FStore->UpdateOrder( OrgOrd ); ///< Sync to file.
            }
        }
    }
    else
        UFC::BufferedLog::Printf( " Futrres confirm NID:%08d not found!\n",NID);
}
//---------------------------------------------------------------------------
void OrcTFXMapper::onOptionsConfirm( Int32 NID, ConfirmParser* Confirm )
{
    ClientOrderID   CNID( NID );
    FIX::ExecType   ExecType;
    FIX::OrdStatus  OrdStatus;
    OrderRecord*    OrgOrd;

    UFC::BufferedLog::DebugPrintf( "Options ");
    if( (OrgOrd = FStore->GetOrder( NID ))!= NULL )
    {
        UFC::AnsiString Message;

        if( FIXMapper::UseSimTX == TRUE )
        {
            FillOrderStatus( CNID, ExecType, OrdStatus );
            if( ExecType == FIX::ExecType_NEW && OrgOrd->GetState() != osPendingNew )
                return;                    ///< Duplicate Order confirm
            if( CNID.GetTransactionKind() == tkNewOrder || CNID.GetTransactionKind() == tkNewQuote )
                OrgOrd->SetOrderID( Confirm->GetOrderID() );///< Set the OrderID to OrderRecoder.
            if( OrgOrd->GetState() == osPendingNew )
                OrgOrd->SetState( osNew ); ///< change order state
            FStore->AddRef( mkOptions, Confirm->GetOrderID(),OrgOrd );
            FStore->UpdateOrder( OrgOrd ); ///< Sync state to file.
            switch( CNID.GetTransactionKind() )
            {
                case tkNewOrder:
                case tkOrderCancel:
                case tkOrderReplace:ReplyConfirmExecution( OrgOrd, ExecType, OrdStatus );
                                    break;///< Reply a New Order Execution
                case tkNewQuote:    Message.Printf( "TAIFEX accepted ID:%s ",Confirm->GetOrderID().c_str());
                                    ReportQuoteStatus( FIX::QuoteID( OrgOrd->GetClOrdID().c_str()), FIX::QuoteStatus( FIX::QuoteStatus_ACCEPTED ),FIX::Text(Message.c_str()) );
                                    break;///< Accepted a new quote.
                case tkQuoteCancel: ReportQuoteStatus( FIX::QuoteID( OrgOrd->GetClOrdID().c_str()), FIX::QuoteStatus( FIX::QuoteStatus_PENDING ),FIX::Text("Quote Pending cancel") );
                                    break;///< Pending cancel a quote.
            }
        }
        else
        {
            switch( Confirm->GetTransactionKind() )
            {
                case tkNewOrder:
                               if( OrgOrd->GetState() != osPendingNew )
                                   return;                    ///< Duplicate Order confirm
                               FStore->AddRef( mkOptions, Confirm->GetOrderID(),OrgOrd );
                               OrgOrd->SetOrderID( Confirm->GetOrderID() );///< Set the OrderID to OrderRecoder.
                               OrgOrd->SetState( osNew );     ///< change order state
                               FStore->UpdateOrder( OrgOrd ); ///< Sync state to file.
                               ReplyConfirmExecution( OrgOrd, FIX::ExecType_NEW, FIX::OrdStatus_NEW  );
                               break;
                case tkOrderCancel:
                               if( OrgOrd->GetState() == osCanceled )
                                   return;                     ///< Duplicate Order confirm
                               OrgOrd->SetState( osCanceled ); ///< change order state
                               FStore->UpdateOrder( OrgOrd );  ///< Sync state to file.
                               ReplyConfirmExecution( OrgOrd, FIX::ExecType_CANCELED, FIX::OrdStatus_CANCELED );
                               break;
                case tkOrderReplace:
                               OrgOrd->ReduceQty( Confirm->GetQty() );
                               if( OrgOrd->GetLeavesQty( ) == 0 )
                               {
                                   OrdStatus = FIX::OrdStatus_FILLED;
                                   OrgOrd->SetState( osFilled );
                               }
                               else if( OrgOrd->GetCumQty() == 0 )
                               {
                                   OrdStatus = FIX::OrdStatus_NEW;
                                   OrgOrd->SetState( osNew );
                               }
                               else
                               {
                                   OrdStatus = FIX::OrdStatus_PARTIALLY_FILLED;
                                   OrgOrd->SetState( osPartialFilled );
                               }
                               ReplyConfirmExecution( OrgOrd, FIX::ExecType_REPLACE, OrdStatus );
                               OrgOrd->SetOrigClOrdID( OrgOrd->GetLastClOrdID().c_str() );
                               FStore->UpdateOrder( OrgOrd ); ///< Sync to file.
                               break;
                case tkNewQuote:
                               Message.Printf( "TAIFEX accepted ID:%s ",Confirm->GetOrderID().c_str());
                               FStore->AddRef( mkOptions, Confirm->GetOrderID(),OrgOrd );
                               OrgOrd->SetOrderID( Confirm->GetOrderID() );///< Set the OrderID to Quote.
                               OrgOrd->SetState( osNew );     ///< change order state
                               FStore->UpdateOrder( OrgOrd ); ///< Sync state to file.
                               ReportQuoteStatus( FIX::QuoteID( OrgOrd->GetClOrdID().c_str()), FIX::QuoteStatus( FIX::QuoteStatus_ACCEPTED ),FIX::Text(Message.c_str()) );
                               break;///< Accepted a new quote.
                case tkQuoteCancel:
                               if( OrgOrd->GetState( ) != osCanceled )///< Not canceled yet
                               {
                                   OrgOrd->SetState( osCanceled );
                                   FStore->UpdateOrder( OrgOrd ); ///< Sync to file.
                                   ReportQuoteStatus( FIX::QuoteID( OrgOrd->GetClOrdID().c_str()), FIX::QuoteStatus( FIX::QuoteStatus_CANCELED_FOR_SYMBOL ),FIX::Text("Quote canceled") );
                               }
                               break;///< Pending cancel a quote.

            }
        }
    }
    else if( Confirm->GetTransactionKind() == tkQuoteRequest || CNID.GetTransactionKind() == tkQuoteRequest )
    {
        UFC::BufferedLog::Printf( " Accept Quote request found!\n",NID);
        FStore->DeleteQuoteRequest( NID );
    }
    else
        UFC::BufferedLog::Printf( " Options confirm NID:%08d not found!\n",NID);
}
//---------------------------------------------------------------------------
void OrcTFXMapper::onStockConfirm( Int32 NID, ConfirmParser* Confirm )
{
    ClientOrderID   CNID( NID );
    FIX::ExecType   ExecType;
    FIX::OrdStatus  OrdStatus;
    OrderRecord*    OrgOrd;

    UFC::BufferedLog::DebugPrintf( "Stocks ");
    FillOrderStatus( CNID, ExecType, OrdStatus );
    if( (OrgOrd = FStore->GetOrder( NID ))!= NULL )
    {
        if( ExecType == FIX::ExecType_NEW )
            return;                    ///< Duplicate Order confirm
        if( CNID.GetTransactionKind() == tkNewOrder || CNID.GetTransactionKind() == tkNewQuote )
            OrgOrd->SetOrderID( Confirm->GetOrderID() );///< Set the OrderID to OrderRecoder.
		if( OrgOrd->GetState() == osPendingNew )
			OrgOrd->SetState( osNew );     ///< change order state
        FStore->AddRef( mkStock, OrgOrd->GetOrderID(),OrgOrd );
        FStore->UpdateOrder( OrgOrd ); ///< Sync state to file.
        ReplyConfirmExecution( OrgOrd, ExecType, OrdStatus );
    }
    else
        UFC::BufferedLog::Printf( " Stock confirm NID:%08d not found!\n",NID);
}
//---------------------------------------------------------------------------

