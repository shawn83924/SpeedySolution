//---------------------------------------------------------------------------
#include "OrcTFXMapper.h"
//---------------------------------------------------------------------------
//  Receive FIX message NewOrderSingle from counterparty
//
//  Transfer this FIX message to  TAIFEX order message  R010
//  and send to TAIFEX via Speedy.
//---------------------------------------------------------------------------
void OrcTFXMapper::PrintFieldNotfond( const char* Message, const char* FieldName )
{
    UFC::BufferedLog::Printf( " Can't fetch field:%s forom message:%s ",FieldName,Message);
}
//---------------------------------------------------------------------------
void OrcTFXMapper::Reject(  const char*						OIDStr,
                            const char*						ErrorText,
                            FIX::Side						Side,
                            FIX::ClOrdID&                   ClOrdID,
                            FIX::Symbol&                    Symbol,
                            FIX::OrdType                    OrderType,
                            FIX::OrderQty                   OrderQty )
{
    UFC::BufferedLog::DebugPrintf( "Reject this order.\n");
    UFC::AnsiString ExecID = FIXMapperApp::GenerateExecID( );

    FIX44::ExecutionReport reject( FIX::OrderID( OIDStr ),
                                   FIX::ExecID( ExecID.c_str()),
                                   FIX::ExecType( FIX::ExecType_REJECTED ),
                                   FIX::OrdStatus( FIX::OrdStatus_REJECTED ),
                                   Side,
                                   FIX::LeavesQty( 0 ),
                                   FIX::CumQty( 0 ),
                                   FIX::AvgPx( 0 ));
    reject.set( OrderQty );
    reject.set( OrderType );
    reject.set( Symbol );
    reject.set( ClOrdID );
    reject.set( FIX::Text( ErrorText ) );
    reject.set( FIX::NoLegs( 0 ) );
    Application->SendToimFIX( FMsgSource, reject );
}
//---------------------------------------------------------------------------
void OrcTFXMapper::onMessage( const FIX44::NewOrderSingle& Order, const FIX::SessionID& sessionID )
{
    ///< Required tags
	FIX::ClOrdID                       ClOrdID;   ///< Tag:11
	FIX::OrdType                       OrderType; ///< Tag:40 1:Market 2:Limit
	FIX::Side                          Side;      ///< Tag:54
	FIX::Symbol                        Symbol;    ///< Tag:55
	FIX::OrderQty                      OrderQty;  ///< Tag:38
    ///< Other tags
	FIX::Account                       Account;
	FIX::SecurityID                    SecurityID;
	FIX::TimeInForce                   timeInForce; ///< Tag 59: 0:ROD 3:IOC 4:FOK
    FIX::Price                         Price( 0.0 ); ///< Tag:44
	FIX::CFICode                       CFICode;
    ///< For Program
    TSCTFXOrderRecord*                 NewOrder;
    ClientOrderID                      NID;
    UFC::AnsiString                    OIDStr = "00000";
    UFC::AnsiString                    R010;
    UFC::AnsiString                    AE = FIXMapper::AE;
    FIX44::ExecutionReport             PendingNew;
	MarketKind                         Market;
    UFC::AnsiString                    ChkAccount;
    AccountAttribute*                  AccountAttr;

    ///< Fetch Required fields
    Order.get( ClOrdID );
    Order.get( OrderType );    ///< Linmit Order or Market Order.
    Order.get( Side );
    Order.get( Symbol );
    Order.get( OrderQty );
    ///< Fetch Other fields
	if( Order.isSetField( Price ) )
        Order.get( Price );
    else
        PrintFieldNotfond( "NewOrderSingle", "Price");
	if( Order.isSetField( Account ) )
        Order.get( Account );
    else
        PrintFieldNotfond( "NewOrderSingle", "Account");
    if( Order.isSetField( timeInForce ) )
        Order.get( timeInForce );
    else
        PrintFieldNotfond( "NewOrderSingle", "timeInForce");
	if( Order.isSetField( CFICode ))
		Order.get( CFICode );
    else
        PrintFieldNotfond( "NewOrderSingle", "CFICode");
	if( Order.isSetField( SecurityID ))
		Order.get( SecurityID );
    else
        PrintFieldNotfond( "NewOrderSingle", "SecurityID");

    if( Order.isSetField( Account ) == FALSE )
    {
        Reject( OIDStr.c_str(),"No Account in this order.  ",Side,ClOrdID,Symbol,OrderType,OrderQty);
        UFC::BufferedLog::DebugPrintf(" No Account in this order. ");
        return;
    }
    ///< Check Account and get the brokerID
    ChkAccount = Account.getString().c_str();

    if( (AccountAttr = FAccountList.GetObjectByKey( ChkAccount )) == NULL )
    {                                                                          ///< Account isn't in the list, reject this order!
        Reject( OIDStr.c_str(),"This account is not authorized to order on the channel ",Side,ClOrdID,Symbol,OrderType,OrderQty);
        UFC::BufferedLog::DebugPrintf(" This account is not authorized to order on the channel");
        return;
    }

    ///< Fetch User define fields
    if( Order.isSetField( 14001 ) )
        AE = Order.getField( 14001 ).c_str();
	Market = CFICodeUtility::Market( CFICode.getString().c_str() );
    NID = ClientOrderID::CreateClientOrderID( Market, tkNewOrder);

    if( OrderType == FIX::OrdType_MARKET )
        Price.setValue( 0.0 ); ///< A market order don't have tag 44(Price)
	if( Market == mkFutures && timeInForce != FIX::TimeInForce_DAY )           ///< It's a Futures order
    {                                                                          ///< TAIFEX Futures supports ROD order only!
        Reject( OIDStr.c_str(), "FIX Gateway: TAIFEX Futures supports ROD order only.", Side, ClOrdID, Symbol, OrderType, OrderQty); ///< Reject this order.
        return;
    }
    NewOrder = new TSCTFXOrderRecord( NID,OIDStr,ClOrdID,Account,AE,SecurityID,Symbol,OrderQty,Price,Side,timeInForce,FMsgSource);
    FStore->AddOrder( (Int32)NID, NewOrder );


    UFC::BufferedLog::DebugPrintf(" Account = [%s]", ChkAccount.c_str() );
    UFC::BufferedLog::DebugPrintf(" BrokerID = [%s]", AccountAttr->GetBrokerID().c_str());
    UFC::BufferedLog::DebugPrintf(" WebID = [%s]", AccountAttr->GetAccountWebID().c_str());

    NewOrder->NewOrderString( R010, AccountAttr );
    UFC::BufferedLog::DebugPrintf(" Order:[%s]", R010.c_str());
    if( Application->SendOrder( (Int32)NID, R010, AccountAttr->GetAccountWebID().c_str() ) == FALSE )
    {
        Reject( OIDStr.c_str(),"9098 MBus service not available.",Side,ClOrdID,Symbol,OrderType,OrderQty);
        return;
    }
    ///< Send back the PENDING NEW ExecutionReport
    UFC::BufferedLog::DebugPrintf( "Send back the PENDING NEW\n");
    NewOrder->FillPendingNew( PendingNew );
    Application->SendToimFIX( FMsgSource, PendingNew );
}
//---------------------------------------------------------------------------
