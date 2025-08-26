//---------------------------------------------------------------------------
#include "OrcTSEMapper.h"
//---------------------------------------------------------------------------
void OrcTSEMapper::RejectCancelNotFound( FIX::ClOrdID ClOrdID,
										 FIX::OrigClOrdID OrigClOrdID,
										 FIX::CxlRejResponseTo CxlRejResponseTo )
{
	UFC::AnsiString			 ErrorStr;
	FIX44::OrderCancelReject reject( FIX::OrderID("00000"),ClOrdID,OrigClOrdID,FIX::OrdStatus_REJECTED,CxlRejResponseTo );

	ErrorStr.Printf( "Order OrigClOrdID:%s not found", OrigClOrdID.getString().c_str());
	reject.set( FIX::Text( ErrorStr.c_str()) );
	Application->SendToimFIX( FMsgSource,reject );
}
//---------------------------------------------------------------------------
void OrcTSEMapper::RejectCancelCanceled( FIX::ClOrdID     ClOrdID,
                                         FIX::OrigClOrdID OrigClOrdID,
                                         FIX::OrderID     OrderID,
                                         FIX::CxlRejResponseTo CxlRejResponseTo )
{
    UFC::AnsiString			 ErrorStr;
    FIX44::OrderCancelReject reject( OrderID,ClOrdID,OrigClOrdID,FIX::OrdStatus_CANCELED,CxlRejResponseTo );

    ErrorStr.Printf( "Order OrderID:%s canceled", OrderID.getString().c_str());
    reject.set( FIX::Text( ErrorStr.c_str()) );
    Application->SendToimFIX( FMsgSource,reject );
}
//---------------------------------------------------------------------------
void OrcTSEMapper::RejectCancelFilled( FIX::ClOrdID     ClOrdID,
                                       FIX::OrigClOrdID OrigClOrdID,
                                       FIX::OrderID     OrderID,
                                       FIX::CxlRejResponseTo CxlRejResponseTo )
{
    UFC::AnsiString			 ErrorStr;
    FIX44::OrderCancelReject reject( OrderID,ClOrdID,OrigClOrdID,FIX::OrdStatus_FILLED,CxlRejResponseTo );

    ErrorStr.Printf( "Order OrderID:%s Filled", OrderID.getString().c_str());
    reject.set( FIX::Text( ErrorStr.c_str()) );
    Application->SendToimFIX( FMsgSource,reject );
}
//---------------------------------------------------------------------------
//  Receive FIX message OrderCancelRequest from counterparty
//
//  Transfer this FIX message to  TSEC order message.
//---------------------------------------------------------------------------
void OrcTSEMapper::onMessage( const FIX44::OrderCancelRequest& Cancel, const FIX::SessionID& sessionID )
{
    UFC::AnsiString        R010;
    FIX::ClOrdID           ClOrdID;
	FIX::OrigClOrdID       OrigClOrdID;
    FIX::OrderID           OrderID;
    FIX::Symbol            Symbol;
    FIX::Side              Side;
    FIX::OrderQty          OrderQty;
	FIX::CFICode		   CFICode;
    TSCTSEOrderRecord*     OrgOrd;

    ///< Retrive Required tags
    Cancel.get( ClOrdID );
	Cancel.get( OrigClOrdID );
    Cancel.get( Side );
    Cancel.get( OrderQty );
    ///< Retrive Others

	if( Cancel.isSetField( Symbol ) )
		Cancel.get( Symbol );
	if( Cancel.isSetField( CFICode ) )
		Cancel.get( CFICode );
    ///< Query the OrderRecoder using OrigClOrdID.
    if( (OrgOrd =(TSCTSEOrderRecord*) FStore->GetOrder( OrigClOrdID.getString().c_str() )) != NULL )
    {

        FIX::OrdType OrderType = ((OrgOrd->IsMarketOrder()==TRUE)?FIX::OrdType_MARKET:FIX::OrdType_LIMIT);

        if( (Int32)OrderQty.getValue() > 999 )
        {
          Reject( OrgOrd->GetOrderID().c_str(), GetErrorMessage("ODD0014").c_str(), Side, ClOrdID, Symbol, OrderType, OrderQty ); ///<Odd lot orders cannot exceed 999 shares.
          return;
        }

   		if( OrgOrd->GetState() == osCanceled )
            RejectCancelCanceled(ClOrdID,OrigClOrdID,FIX::OrderID(OrgOrd->GetOrderID().c_str()),FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST));
        else if( OrgOrd->GetState() == osFilled )
            RejectCancelFilled(ClOrdID,OrigClOrdID,FIX::OrderID(OrgOrd->GetOrderID().c_str()),FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST));
        else
        {
            ClientOrderID  NID = ClientOrderID::CreateClientOrderID( mkStock, tkOrderCancel );

            OrgOrd->CancelString( (Int32)NID, R010 );                 ///< Get the order cancel string.(Send to Brocker)
            OrgOrd->SetLastClOrdID( ClOrdID.getString().c_str() );    ///< Set the LastClOrdID to the ClOrdID of this OrderCancelRequest
            FStore->AddRef( (Int32)NID, OrgOrd );                     ///< This NID ref. to the original order record.
            FStore->AddRef( ClOrdID.getString().c_str(), OrgOrd );    ///< This ClOrdID ref. to the original order record.
            FStore->UpdateOrder( OrgOrd );                            ///< Sync to file.

            if( Application->SendOrder( (Int32)NID, R010, FIXMapper::WebID.c_str()) == FALSE )
            {
                Reject( OrgOrd->GetOrderID().c_str(),GetErrorMessage("33003").c_str(), Side, ClOrdID, Symbol, OrderType, OrderQty * 1000);
            }
        }
    }
    else  ///< Order not found! Send OrderCancelReject back.
		RejectCancelNotFound( ClOrdID,OrigClOrdID,FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST) );
}
//---------------------------------------------------------------------------
