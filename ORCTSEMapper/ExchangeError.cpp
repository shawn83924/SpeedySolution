//---------------------------------------------------------------------------
#include "OrcTSEMapper.h"
//---------------------------------------------------------------------------
void OrcTSEMapper::OrderCancelReject( Int32 NID, OrderRecord* OrgOrd,UFC::AnsiString& ErrorStr, BOOL IsCancel )
{
	FIX::CxlRejResponseTo CxlRejResponseTo;

	if( IsCancel ) ///< Reply for the OrderCancelRequest message
		CxlRejResponseTo = FIX::CxlRejResponseTo( '1' );
    else          ///< Reply for the OrderCancelReplaceRequest message
		CxlRejResponseTo = FIX::CxlRejResponseTo( '2' );
    FIX44::OrderCancelReject reject( FIX::OrderID( OrgOrd->GetOrderID().c_str()),
                                     //FIX::ClOrdID( FStore->GetCLOrdID( NID )->c_str()) , ///< Its a Cancel/replace order.
                                     FIX::ClOrdID( OrgOrd->GetLastClOrdID().c_str()),
                                     FIX::OrigClOrdID( OrgOrd->GetOrigClOrdID().c_str()),
                                     FIX::OrdStatus( FIX::OrdStatus_REJECTED ),
                                     CxlRejResponseTo );
    reject.set( FIX::Text( ErrorStr.c_str()) );
    Application->SendToimFIX( OrgOrd->GetSender(),reject );
}
//---------------------------------------------------------------------------
void OrcTSEMapper::RejectExecution( OrderRecord* OrgOrd, UFC::AnsiString& ErrorStr )
{
    UFC::BufferedLog::DebugPrintf(" RejectExecution.");
    FIX44::ExecutionReport report( FIX::OrderID( OrgOrd->GetOrderID().c_str()),
                                   FIX::ExecID( FIXMapperApp::GenerateExecID().c_str()),
                                   FIX::ExecType( FIX::ExecType_REJECTED ),
                                   FIX::OrdStatus( FIX::OrdStatus_REJECTED ),
                                   FIX::Side( OrgOrd->GetSide() ),
                                   FIX::LeavesQty( OrgOrd->GetLeavesQty()),
                                   FIX::CumQty( OrgOrd->GetCumQty() ),
                                   FIX::AvgPx( 0 ));
    report.set( FIX::ClOrdID(OrgOrd->GetClOrdID().c_str()));///< Its a new order.
    report.set( FIX::Text( ErrorStr.c_str()) );
    report.set( FIX::Symbol(OrgOrd->GetSymbol().c_str() ) );
    report.set( FIX::NoLegs(0));
    Application->SendToimFIX( OrgOrd->GetSender(),report );

    if( OrgOrd->GetSide() == FIX::Side_SELL) ///< 2 for sell
    {
        UFC::AnsiString  Account = ((TSCTSEOrderRecord*)OrgOrd)->GetAccount();
        UFC::AnsiString  SymbolStr = OrgOrd->GetSymbol();
        UFC::AnsiString  FInqType("1");
        UFC::AnsiString  Reason,SaleID;

        if( LStore->CheckLeftQty( FALSE ,Account,SymbolStr,OrgOrd->GetLeavesQty(),SaleID,Reason ) == FALSE )
            UFC::BufferedLog::Printf(" Add qty failed.");
    }
}

//---------------------------------------------------------------------------
// Receive an Order Check Error message from TSEC.
// If it's a New/Reduce order:
//    Map to FIX ExecutionReport message.( MsgType = 8 )
// If it's a Cancel order:
//    Map to FIX OrderCancelReject message.( MsgType = 9 )
//
// Function onStockError: Handle TSEC stock order error.
//---------------------------------------------------------------------------
void OrcTSEMapper::onStockError( Int32 NID, ConfirmParser* Result )
{
    UFC::AnsiString ErrorStr;
    OrderRecord*    OrgOrd = FStore->GetOrder( NID );

    if( OrgOrd != NULL  )
    {
      ErrorStr = Result->GetErrorMessage();
      ErrorStr.TrimRight();
      UFC::BufferedLog::DebugPrintf(" Error code = [%s]", ErrorStr.c_str());
      ErrorStr = GetErrorMessage( ErrorStr );
      UFC::BufferedLog::DebugPrintf(" Error message = [%s]", ErrorStr.c_str());

       switch( ClientOrderID::GetTransactionKind(NID) )
       {

           case tkNewOrder:     UFC::BufferedLog::DebugPrintf( "Reject Stock Order\n" );
                                RejectExecution( OrgOrd, ErrorStr );
                                break;///< Reply the ExecutionReport message
           case tkOrderCancel:  UFC::BufferedLog::DebugPrintf( "Reject Stock OrderCancelRequest\n" );
                                OrderCancelReject( NID, OrgOrd, ErrorStr, TRUE );
                                break;///< Reply the OrderCancelReject message
           case tkOrderReplace: UFC::BufferedLog::DebugPrintf( "Reject Stock OrderCancelReplaceRequest\n" );
                                OrderCancelReject( NID, OrgOrd, ErrorStr, FALSE );
                                break;///< Reject a Replace Order
       }
    }
}
//---------------------------------------------------------------------------
