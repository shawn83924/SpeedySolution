//---------------------------------------------------------------------------
#include "OrcTFXMapper.h"
//---------------------------------------------------------------------------
void OrcTFXMapper::OrderCancelReject( Int32 NID, OrderRecord* OrgOrd,UFC::AnsiString& ErrorStr, BOOL IsCancel )
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
void OrcTFXMapper::RejectExecution( OrderRecord* OrgOrd, UFC::AnsiString& ErrorStr )
{
  FIX44::ExecutionReport report( FIX::OrderID( OrgOrd->GetOrderID().c_str()),
                                 FIX::ExecID( FIXMapperApp::GenerateExecID().c_str()),
                                 FIX::ExecType( FIX::ExecType_REJECTED ),
                                 FIX::OrdStatus( FIX::OrdStatus_REJECTED ),
                                 FIX::Side( OrgOrd->GetSide() ),
                                 FIX::LeavesQty( OrgOrd->GetLeavesQty() ),
                                 FIX::CumQty( OrgOrd->GetCumQty() ),
                                 FIX::AvgPx( 0 ));
  report.set( FIX::ClOrdID(OrgOrd->GetClOrdID().c_str()));///< Its a new order.
  report.set( FIX::Text( ErrorStr.c_str()) );
  report.set( FIX::Symbol(OrgOrd->GetSymbol().c_str() ) );
  report.set( FIX::SecurityID( static_cast<TSCTFXOrderRecord*>(OrgOrd)->GetSecurityID().c_str() ) );
  report.set( FIX::NoLegs(0));
  Application->SendToimFIX( OrgOrd->GetSender(),report );
}
//---------------------------------------------------------------------------
// Receive an Order Check Error message from TAIFEX.
// If it's a New/Reduce order:
//    Map to FIX ExecutionReport message.( MsgType = 8 )
// If it's a Cancel order:
//    Map to FIX OrderCancelReject message.( MsgType = 9 )
//
// Function onFuturesError: Handle TAIFEX futures order error.
// Function onOptionError:  Handle TAIFEX Option order error.
//---------------------------------------------------------------------------
void OrcTFXMapper::onFuturesError( Int32 NID,  ConfirmParser* Result )
{
    UFC::AnsiString ErrorStr;
    OrderRecord*    OrgOrd = FStore->GetOrder( NID );

    if( OrgOrd != NULL )
    {
        //ErrorStr.Printf( "%04d:%s", Result->GetErrorCode(), Result->GetErrorMessage().c_str());
        if( ErrorMessage.ValueExists( "ErrorMessage", Result->GetErrorMessage() ) == TRUE )
        {
            ErrorMessage.GetValue( "ErrorMessage", Result->GetErrorMessage(), ErrorStr );
        }
        else
            ErrorStr.Printf( "Unknown error. Error Code = %s.", Result->GetErrorMessage().c_str() );
        switch( ClientOrderID::GetTransactionKind( NID ) )
        {
            case tkNewOrder:     if( OrgOrd->GetState() == osPendingNew )
                                 {
                                     OrgOrd->SetState( osRejected );
                                     FStore->UpdateOrder( OrgOrd ); ///< Sync to file.
                                     UFC::BufferedLog::DebugPrintf( "Reject Futures Order\n" );
                                     RejectExecution( OrgOrd, ErrorStr );
                                 }
                                 break;///< Reject a new Futures Order
            case tkOrderCancel:  UFC::BufferedLog::DebugPrintf( "Reject Futures OrderCancelRequest\n" );
                                 OrderCancelReject( NID,OrgOrd, ErrorStr, TRUE );
                                 break;///< Reply the OrderCancelReject message
            case tkOrderReplace: UFC::BufferedLog::DebugPrintf( "Reject Futures OrderCancelReplaceRequest\n" );
                                 OrderCancelReject( NID,OrgOrd, ErrorStr, FALSE );
                                 break;///< Reject a Replace Order
        }
    }
}
//---------------------------------------------------------------------------
void OrcTFXMapper::onOptionsError( Int32 NID, ConfirmParser* Result )
{
    UFC::AnsiString ErrorStr;
    TransactionKind TransKind = ClientOrderID::GetTransactionKind( NID );
    OrderRecord*    OrgOrd = FStore->GetOrder( NID );

    if( OrgOrd != NULL  )
    {
        //ErrorStr.Printf( "%04d:%s", Result->GetErrorCode(), Result->GetErrorMessage().c_str());
        if( ErrorMessage.ValueExists( "ErrorMessage", Result->GetErrorMessage() ) == TRUE )
        {
            ErrorMessage.GetValue( "ErrorMessage", Result->GetErrorMessage(), ErrorStr );
        }
        else
            ErrorStr.Printf( "Unknown error. Error Code = %s.", Result->GetErrorMessage().c_str() );

        switch( TransKind )
        {
            case tkNewOrder:     if( OrgOrd->GetState() == osPendingNew )
                                 {
                                     OrgOrd->SetState( osRejected );
                                     FStore->UpdateOrder( OrgOrd ); ///< Sync to file.
                                     UFC::BufferedLog::DebugPrintf( "Reject Options Order\n" );
                                     RejectExecution( OrgOrd, ErrorStr );
                                 }
                                 break;///< Reject a new Options Order
            case tkOrderCancel:  UFC::BufferedLog::DebugPrintf( "Reject Options OrderCancelRequest\n" );
                                 OrderCancelReject( NID,OrgOrd, ErrorStr, TRUE );
                                 break;///< Reply the OrderCancelReject message
            case tkOrderReplace: UFC::BufferedLog::DebugPrintf( "Reject Options OrderCancelReplaceRequest\n" );
                                 OrderCancelReject( NID,OrgOrd, ErrorStr, FALSE );
                                 break;///< Reject a Replace Order
            case tkNewQuote:     if( OrgOrd->GetState() == osPendingNew )
                                 {
                                     OrgOrd->SetState( osRejected );
                                     FStore->UpdateOrder( OrgOrd ); ///< Sync to file.
                                     UFC::BufferedLog::DebugPrintf( "Reject Options Quote\n" );
                                     ReportQuoteStatus( FIX::QuoteID( OrgOrd->GetClOrdID().c_str()), FIX::QuoteStatus( FIX::QuoteStatus_REJECTED ),FIX::Text(ErrorStr.c_str()));
                                 }
                                 break;///< Reject a new Options Quote
            case tkQuoteCancel:  UFC::BufferedLog::DebugPrintf( "Reject a QuoteCancel\n" );
                                 ReportQuoteStatus( FIX::QuoteID( OrgOrd->GetClOrdID().c_str()), FIX::QuoteStatus( FIX::QuoteStatus_REJECTED ),FIX::Text(ErrorStr.c_str()));
                                 break;///< Reject the QuoteCancel message
        }
    }
    else if( TransKind == tkQuoteRequest )
    {
        QuoteRequestRecord* QuoteRequest;
        if( (QuoteRequest = FStore->GetQuoteRequest( NID ))!=NULL )
        {
            FStore->DeleteQuoteRequest( NID );
        }
    }
    else
        UFC::BufferedLog::Printf( " Options confirm NID:%08d not found!\n",NID);
}
//---------------------------------------------------------------------------
void OrcTFXMapper::onStockError( Int32 NID, ConfirmParser* Result )
{
    UFC::AnsiString ErrorStr;
    OrderRecord*    OrgOrd = FStore->GetOrder( NID );

    if( OrgOrd != NULL  )
    {
       ErrorStr.Printf( "%04d:%s", Result->GetErrorCode(), Result->GetErrorMessage().c_str());
       switch( ClientOrderID::GetTransactionKind(NID) )
       {

           case tkNewOrder:     if( OrgOrd->GetState() == osPendingNew )
                                {
                                   OrgOrd->SetState( osRejected );
                                   FStore->UpdateOrder( OrgOrd ); ///< Sync to file.
                                   UFC::BufferedLog::DebugPrintf( "Reject Stock Order\n" );
                                   RejectExecution( OrgOrd, ErrorStr );
                                }
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
