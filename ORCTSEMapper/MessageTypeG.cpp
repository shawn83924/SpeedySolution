//---------------------------------------------------------------------------
#include "OrcTSEMapper.h"
//---------------------------------------------------------------------------
//  Receive FIX message OrderCancelReplaceRequest from counterparty
//
//  Transfer this FIX message to  TSEC order message.
//---------------------------------------------------------------------------
void OrcTSEMapper::onMessage( const FIX44::OrderCancelReplaceRequest& Cancel, const FIX::SessionID& sessionID )
{
    UFC::AnsiString        R010;
    TSCTSEOrderRecord*     OrgOrd;
    FIX::ClOrdID           ClOrdID;
	FIX::OrigClOrdID       OrigClOrdID;
    FIX::OrdType           OrderType;
    FIX::Symbol            Symbol;
    FIX::Side              Side;
    FIX::Price             Price;
    FIX::OrderQty          OrderQty;
	FIX::CFICode           CFICode;
    Int32                  NewQty;

    ///<Required tags
    Cancel.get( Symbol );
    Cancel.get( OrderQty );
    Cancel.get( ClOrdID );
	Cancel.get( OrigClOrdID );
    Cancel.get( OrderType );
    Cancel.get( Side );
	NewQty = (Int32)OrderQty.getValue();
    ///<Other tags
	if( Cancel.isSetField( Price ) )
		Cancel.get( Price );
	if( Cancel.isSetField( CFICode ) )
		Cancel.get( CFICode );
	///< Query the OrderRecoder using OrigClOrdID.
    if( (OrgOrd =(TSCTSEOrderRecord*) FStore->GetOrder( OrigClOrdID.getString().c_str() )) != NULL ) ///< Order exists.
    {
        if( (Int32)OrderQty.getValue() > 999 )
        {
            Reject( OrgOrd->GetOrderID().c_str(), GetErrorMessage("ODD0014").c_str(), Side, ClOrdID, Symbol, OrderType, OrderQty ); ///<Odd lot orders cannot exceed 999 shares.
            return;
        }
        if( OrgOrd->GetState() == osCanceled ) ///< This already canceled.
            RejectCancelCanceled(ClOrdID,OrigClOrdID,FIX::OrderID(OrgOrd->GetOrderID().c_str()),FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST));
        else if( OrgOrd->GetState() == osFilled ) ///< This Order was filled.
            RejectCancelFilled(ClOrdID,OrigClOrdID,FIX::OrderID(OrgOrd->GetOrderID().c_str()),FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST));
        else if( (OrgOrd->IsMarketOrder() == FALSE) && (fabs ( Price.getValue() - OrgOrd->GetPrice() ) >= 0.001) ) ///< TAIFEX not support change order price.
        {
            UFC::BufferedLog::DebugPrintf(" fabs() = %f", fabs ( Price.getValue() - OrgOrd->GetPrice() ));
            Reject( OrgOrd->GetOrderID().c_str(),GetErrorMessage("POT0021").c_str(),Side,ClOrdID,Symbol,OrderType,OrderQty );
        }
        else if( NewQty >= OrgOrd->GetOrderQty()) ///< TAIFEX not support increase order qty.
            Reject( OrgOrd->GetOrderID().c_str(),GetErrorMessage("POT0032").c_str(),Side,ClOrdID,Symbol,OrderType,OrderQty );

        else
        {
            ClientOrderID  NID = ClientOrderID::CreateClientOrderID( mkStock, tkOrderReplace );

            OrgOrd->ReduceString( (Int32)NID, OrgOrd->GetOrderQty() - NewQty, R010 );
            OrgOrd->SetLastClOrdID( ClOrdID.getString().c_str() ); ///< Set the LastClOrdID to the ClOrdID of this OrderCancelReplaceRequest
            FStore->AddRef( (Int32)NID, OrgOrd );                  ///< This NID will ref. to this order object.
			FStore->AddRef( ClOrdID.getString().c_str(), OrgOrd ); ///< The ClOrdID will ref. this order object.
            FStore->UpdateOrder( OrgOrd );                         ///< Sync to file.

            if( Application->SendOrder( (Int32)NID, R010, FIXMapper::WebID.c_str() ) == FALSE )
                Reject( OrgOrd->GetOrderID().c_str(),"9098 MBus service not available.",Side,ClOrdID,Symbol,OrderType,OrderQty * 1000);
        }
    }
    else
		RejectCancelNotFound( ClOrdID, OrigClOrdID, FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST) );
}
//---------------------------------------------------------------------------
