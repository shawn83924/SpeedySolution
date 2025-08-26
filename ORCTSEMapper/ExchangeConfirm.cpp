//---------------------------------------------------------------------------
#include "OrcTSEMapper.h"
//---------------------------------------------------------------------------
void OrcTSEMapper::ReplyConfirmExecution( OrderRecord* OrgOrd, FIX::ExecType ExecType, FIX::OrdStatus OrdStatus )
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
// Function onStockConfirm:   Handle TSE order confirm.
//---------------------------------------------------------------------------
void OrcTSEMapper::onStockConfirm( Int32 NID, ConfirmParser* Confirm )
{
    ClientOrderID   CNID( NID );
    FIX::OrdStatus  OrdStatus;
    OrderRecord*    OrgOrd;

    if( (OrgOrd = FStore->GetOrder( NID ))!= NULL )
    {
        if( Confirm->GetTransactionKind() == tkNewOrder )
        {
            if( OrgOrd->GetState() != osPendingNew )
                return;                    ///< Duplicate Order confirm

            FStore->AddRef( mkStock, Confirm->GetOrderID(),OrgOrd );
            OrgOrd->SetOrderID( Confirm->GetOrderID() );///< Set the OrderID to OrderRecoder.

            OrgOrd->SetState( osNew );     ///< change order state
            FStore->UpdateOrder( OrgOrd ); ///< Sync state to file.
            ReplyConfirmExecution( OrgOrd, FIX::ExecType_NEW, FIX::OrdStatus_NEW );
        }
        else if( Confirm->GetTransactionKind() == tkOrderCancel )
        {
            if( OrgOrd->GetState() == osCanceled )
                return;                     ///< Duplicate Order confirms
            if( OrgOrd->GetSide() == FIX::Side_SELL) ///< 2 for sell
            {
                UFC::AnsiString  Account = ((TSCTSEOrderRecord*)OrgOrd)->GetAccount();
                UFC::AnsiString  SymbolStr = OrgOrd->GetSymbol();
                UFC::AnsiString  Reason,SaleID;

                if( LStore->CheckLeftQty( FALSE ,Account,SymbolStr,OrgOrd->GetLeavesQty(),SaleID,Reason ) == FALSE )
                    UFC::BufferedLog::DebugPrintf( " CheckLeftQty return FALSE, reason:[%s]", Reason.c_str() );
                else
                    UFC::BufferedLog::DebugPrintf( " CheckLeftQty return TRUE.");
            }

            OrgOrd->SetState( osCanceled ); ///< change order statetate
            FStore->UpdateOrder( OrgOrd );  ///< Sync state to file.
            ReplyConfirmExecution( OrgOrd, FIX::ExecType_CANCELED, FIX::OrdStatus_CANCELED );
        }
        else if( Confirm->GetTransactionKind() == tkOrderReplace )
        {
            UFC::BufferedLog::DebugPrintf(" CancelReplace confirm:");

            UFC::BufferedLog::DebugPrintf(" Confirm->GetQty() = %d", Confirm->GetQty());
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

            if( OrgOrd->GetSide() == FIX::Side_SELL) ///< 2 for sell
            {
                UFC::AnsiString  Account = ((TSCTSEOrderRecord*)OrgOrd)->GetAccount();
                UFC::AnsiString  SymbolStr = OrgOrd->GetSymbol();
                UFC::AnsiString  Reason,SaleID;

                if( LStore->CheckLeftQty( FALSE ,Account,SymbolStr,Confirm->GetQty(),SaleID,Reason ) == FALSE )
                    UFC::BufferedLog::DebugPrintf( " CheckLeftQty return FALSE, reason:[%s]", Reason.c_str());
                else
                    UFC::BufferedLog::DebugPrintf( " CheckLeftQty return TRUE.");
            }

            ReplyConfirmExecution( OrgOrd, FIX::ExecType_REPLACE, OrdStatus );
            OrgOrd->SetOrigClOrdID( OrgOrd->GetLastClOrdID().c_str() );
            FStore->UpdateOrder( OrgOrd ); ///< Sync to file.
        }
    }
    else
        UFC::BufferedLog::Printf( " Stock confirm NID:%08d not found!\n", NID);
}
//---------------------------------------------------------------------------

