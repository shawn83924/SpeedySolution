//---------------------------------------------------------------------------
#include "OrcTFXMapper.h"
//---------------------------------------------------------------------------
//  Receive FIX message OrderCancelReplaceRequest from counterparty
//
//  Transfer this FIX message to  TAIFEX order message  R010
//  and send to TAIFEX via Speedy.
//---------------------------------------------------------------------------
void OrcTFXMapper::onMessage( const FIX44::OrderCancelReplaceRequest& Cancel, const FIX::SessionID& sessionID )
{
    UFC::AnsiString        R010;
    TSCTFXOrderRecord*     OrgOrd;
    FIX::ClOrdID           ClOrdID;
	FIX::OrigClOrdID       OrigClOrdID;
    FIX::OrdType           OrderType;
    FIX::Symbol            Symbol;
    FIX::Side              Side;
    FIX::Price             Price;
    FIX::OrderQty          OrderQty;
	FIX::CFICode           CFICode;
    Int32                  NewQty;
	MarketKind			   Market;
    UFC::AnsiString        ChkAccount;
    AccountAttribute*      AccountAttr;

    ///<Required tags
    Cancel.get( Symbol );
    Cancel.get( OrderQty );
    Cancel.get( ClOrdID );
	Cancel.get( OrigClOrdID );
    Cancel.get( OrderType );
    Cancel.get( Side );
	NewQty = (Int32)OrderQty.getValue();
    ///<Other tags
    Cancel.get( Price );
	Cancel.get( CFICode );
	///< Query the OrderRecoder using OrigClOrdID.
	Market = CFICodeUtility::Market( CFICode.getString().c_str());
    if( (OrgOrd = (TSCTFXOrderRecord*)FStore->GetOrder( OrigClOrdID.getString().c_str() )) != NULL ) ///< Order exists.
    {
        if( OrgOrd->GetState() == osCanceled ) ///< This already canceled.
            RejectCancelCanceled(ClOrdID,OrigClOrdID,FIX::OrderID(OrgOrd->GetOrderID().c_str()),FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST));
        else if( OrgOrd->GetState() == osFilled ) ///< This Order was filled.
            RejectCancelFilled(ClOrdID,OrigClOrdID,FIX::OrderID(OrgOrd->GetOrderID().c_str()),FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST));
        else if( NewQty >= OrgOrd->GetOrderQty()) ///< TAIFEX not support increase order qty.
            Reject( OrgOrd->GetOrderID().c_str(),"0022 TAIFEX not support increase order qty.",Side,ClOrdID,Symbol,OrderType,OrderQty);
        else
        {
            ClientOrderID  NID = ClientOrderID::CreateClientOrderID( Market, tkOrderReplace );

            ChkAccount = OrgOrd->GetAccount();

            if( (AccountAttr = FAccountList.GetObjectByKey( ChkAccount )) == NULL )
            {                                                                          ///< Account isn't in the list, reject this order!
                Reject( OrgOrd->GetOrderID().c_str(),"This account is not authorized to order on the channel.",Side,ClOrdID,Symbol,OrderType,OrderQty);
                UFC::BufferedLog::DebugPrintf(" This account is not authorized to order on the channel.");
                return;
            }
            UFC::BufferedLog::DebugPrintf( " Account = [%s]", ChkAccount.c_str() );
            UFC::BufferedLog::DebugPrintf( " BrokerID = [%s]", AccountAttr->GetBrokerID().c_str());
            UFC::BufferedLog::DebugPrintf( " WebID =[%s]", AccountAttr->GetAccountWebID().c_str() );

            OrgOrd->ReduceString( (Int32)NID, NewQty, R010, AccountAttr );
            OrgOrd->SetLastClOrdID( ClOrdID.getString().c_str() ); ///< Set the LastClOrdID to the ClOrdID of this OrderCancelReplaceRequest
            FStore->AddRef( (Int32)NID, OrgOrd );                  ///< This NID will ref. to this order object.
			FStore->AddRef( ClOrdID.getString().c_str(), OrgOrd ); ///< The ClOrdID will ref. this order object.
            FStore->UpdateOrder( OrgOrd );                         ///< Sync to file.
            UFC::BufferedLog::DebugPrintf(" CancelReplace:[%s]", R010.c_str());
            if( Application->SendOrder( (Int32)NID, R010, AccountAttr->GetAccountWebID().c_str() ) == FALSE )
                Reject( OrgOrd->GetOrderID().c_str(),"9098 MBus service not available.",Side,ClOrdID,Symbol,OrderType,OrderQty);
            if( FIXMapper::UseSimTX == FALSE )
                ReplyConfirmExecution( OrgOrd, FIX::ExecType_PENDING_REPLACE, FIX::OrdStatus_PENDING_REPLACE );
        }
    }
    else
		RejectCancelNotFound( ClOrdID, OrigClOrdID, FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST) );
}
//---------------------------------------------------------------------------
