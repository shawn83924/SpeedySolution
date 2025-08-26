//---------------------------------------------------------------------------
#include "OrcTFXMapper.h"
//---------------------------------------------------------------------------
void OrcTFXMapper::RejectCancelNotFound( FIX::ClOrdID ClOrdID,
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
void OrcTFXMapper::RejectCancelCanceled( FIX::ClOrdID     ClOrdID,
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
void OrcTFXMapper::RejectCancelFilled( FIX::ClOrdID     ClOrdID,
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
//  Transfer this FIX message to  TAIFEX order message  R010
//  and send to TAIFEX via Speedy.
//---------------------------------------------------------------------------
void OrcTFXMapper::onMessage( const FIX44::OrderCancelRequest& Cancel, const FIX::SessionID& sessionID )
{
    UFC::AnsiString        R010;
    FIX::ClOrdID           ClOrdID;
	FIX::OrigClOrdID       OrigClOrdID;
    FIX::OrderID           OrderID;
    FIX::Symbol            Symbol;
    FIX::Side              Side;
    FIX::OrderQty          OrderQty;
	FIX::CFICode		   CFICode;
    TSCTFXOrderRecord*     OrgOrd;
	MarketKind			   Market;
    UFC::AnsiString        ChkAccount;
    AccountAttribute*      AccountAttr;
    ///< Retrive Required tags
    Cancel.get( ClOrdID );
	Cancel.get( OrigClOrdID );
    Cancel.get( Side );
    Cancel.get( OrderQty );
    ///< Retrive Others
	if( Cancel.isSetField( Symbol ) )
		Cancel.get( Symbol );
    else
        PrintFieldNotfond( "OrderCancelRequest", "Symbol");
	if( Cancel.isSetField( CFICode ) )
		Cancel.get( CFICode );
    else
        PrintFieldNotfond( "OrderCancelRequest", "CFICode");
	///< Find out the market using CFICode (tag 461)
    Market = CFICodeUtility::Market( CFICode.getString().c_str());
    ///< Query the OrderRecoder using OrigClOrdID.
    if( (OrgOrd = (TSCTFXOrderRecord*)FStore->GetOrder( OrigClOrdID.getString().c_str() )) != NULL )
    {
   		if( OrgOrd->GetState() == osCanceled )
			RejectCancelCanceled(ClOrdID,OrigClOrdID,FIX::OrderID(OrgOrd->GetOrderID().c_str()),FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST));
        else if( OrgOrd->GetState() == osFilled )
            RejectCancelFilled(ClOrdID,OrigClOrdID,FIX::OrderID(OrgOrd->GetOrderID().c_str()),FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST));
        else
        {
            ClientOrderID  NID = ClientOrderID::CreateClientOrderID( Market, tkOrderCancel );

            ChkAccount = OrgOrd->GetAccount();

            if( ( AccountAttr = FAccountList.GetObjectByKey( ChkAccount )) == NULL )
            {                                                                          ///< Account isn't in the list, reject this order!
                FIX::OrdType OrderType = ((OrgOrd->IsMarketOrder()==TRUE)?FIX::OrdType_MARKET:FIX::OrdType_LIMIT);
                Reject( OrgOrd->GetOrderID().c_str(),"This account is not authorized to order on the channel.",Side,ClOrdID,Symbol,OrderType,OrderQty);
                UFC::BufferedLog::DebugPrintf(" This account is not authorized to order on the channel");
                return;
            }

            UFC::BufferedLog::DebugPrintf( " Account = [%s]", ChkAccount.c_str() );
            UFC::BufferedLog::DebugPrintf( " BrokerID = [%s]", AccountAttr->GetBrokerID().c_str());
            UFC::BufferedLog::DebugPrintf( " WebID =[%s]", AccountAttr->GetAccountWebID().c_str() );

            OrgOrd->CancelString( (Int32)NID, R010, AccountAttr );          ///< Get the order cancel string.(Send to Brocker)
            OrgOrd->SetLastClOrdID( ClOrdID.getString().c_str() );    ///< Set the LastClOrdID to the ClOrdID of this OrderCancelRequest
            FStore->AddRef( (Int32)NID, OrgOrd );                     ///< This NID ref. to the original order record.
            FStore->AddRef( ClOrdID.getString().c_str(), OrgOrd );    ///< This ClOrdID ref. to the original order record.
            FStore->UpdateOrder( OrgOrd );                            ///< Sync to file.
            UFC::BufferedLog::DebugPrintf(" Cancel:[%s]", R010.c_str());
            if( Application->SendOrder( (Int32)NID, R010, AccountAttr->GetAccountWebID().c_str() ) == FALSE )
            {
                FIX::OrdType OrderType = ((OrgOrd->IsMarketOrder()==TRUE)?FIX::OrdType_MARKET:FIX::OrdType_LIMIT);
                Reject( OrgOrd->GetOrderID().c_str(),"9098 MBus service not available.",Side,ClOrdID,Symbol,OrderType,OrderQty);
            }
            if( FIXMapper::UseSimTX == FALSE )
                ReplyConfirmExecution( OrgOrd, FIX::ExecType_PENDING_CANCEL, FIX::OrdStatus_PENDING_CANCEL );
        }
    }
    else  ///< Order not found! Send OrderCancelReject back.
		RejectCancelNotFound( ClOrdID,OrigClOrdID,FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST) );
}
//---------------------------------------------------------------------------
