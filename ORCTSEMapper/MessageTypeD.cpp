//---------------------------------------------------------------------------
#include "OrcTSEMapper.h"
//---------------------------------------------------------------------------
//  Receive FIX message NewOrderSingle from counterparty
//
//  Transfer this FIX message to  TAIFEX order message  R010
//  and send to TAIFEX via Speedy.
//---------------------------------------------------------------------------
void OrcTSEMapper::Reject(  const char*						OIDStr,
                            const char*						ErrorText,
                            FIX::Side						Side,
                            FIX::ClOrdID&                   ClOrdID,
                            FIX::Symbol&                    Symbol,
                            FIX::OrdType                    OrderType,
                            FIX::OrderQty                   OrderQty )
{
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
void OrcTSEMapper::onMessage( const FIX44::NewOrderSingle& Order, const FIX::SessionID& sessionID )
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
	FIX::TimeInForce                   timeInForce( FIX::TimeInForce_DAY ); ///< Tag 59: 0:ROD 3:IOC 4:FOK
    FIX::Price                         Price( 0.0 ); ///< Tag:44
	FIX::CFICode                       CFICode;
    ///< For Program
    //ClientOrderID                      NID;
    UFC::AnsiString                    OIDStr = "00000";
    UFC::AnsiString                    R010;
    UFC::AnsiString                    SaleID = "    ";
    FIX44::ExecutionReport             PendingNew;

    ///< Fetch Required fields
    Order.get( ClOrdID );
    Order.get( OrderType );    ///< Linmit Order or Market Order.
    Order.get( Side );
    Order.get( Symbol );
    Order.get( OrderQty );

    UFC::BufferedLog::DebugPrintf(" OrderQty = %d", (Int32)OrderQty.getValue());
    if( (Int32)OrderQty.getValue() > 999 )
    {
        Reject( OIDStr.c_str(), GetErrorMessage("ODD0014").c_str(), Side, ClOrdID, Symbol, OrderType, OrderQty ); ///<Odd lot orders cannot exceed 999 shares.
        return;
    }
    ///< Fetch Other fields
	if( Order.isSetField( Price ) )
        Order.get( Price );
	if( Order.isSetField( Account ) )
        Order.get( Account );
    if( Order.isSetField( timeInForce ) )
        Order.get( timeInForce );
	if( Order.isSetField( CFICode ))
		Order.get( CFICode );
	if( Order.isSetField( SecurityID ))
		Order.get( SecurityID );

    ///< Fetch User define fields
    ClientOrderID NID = ClientOrderID::CreateClientOrderID( mkStock, tkNewOrder);
    if( OrderType == FIX::OrdType_MARKET )
        Price.setValue( 0.0 ); ///< A market order don't have tag 44(Price)

    ///<check LeftQty
    if( Side == FIX::Side_SELL) ///< 2 for sell
    {
        UFC::AnsiString  ChkAccount = Account.getString().c_str();
        UFC::AnsiString  SymbolStr = Symbol.getString().c_str();
        UFC::AnsiString  Reason;

        if( LStore->CheckLeftQty( TRUE , ChkAccount, SymbolStr, (Int32)OrderQty.getValue(), SaleID, Reason ) == FALSE )
        {
            UFC::BufferedLog::DebugPrintf( " Reason = [%s]", Reason.c_str() );
            if( Reason.Length() <= 7 ) ///< Its an error code.
                Reason = GetErrorMessage( Reason );
            Reject( OIDStr.c_str(), Reason, Side, ClOrdID, Symbol, OrderType, OrderQty ) ;
            return;
        }
    }
    else ///< Buy side, check account only.
    {
        UFC::AnsiString  ChkAccount = Account.getString().c_str();
        UFC::AnsiString  Reason;

        if( LStore->AccountExists( ChkAccount, SaleID ) == FALSE )
        {
            Reason = GetErrorMessage( "31014" );
            Reject( OIDStr.c_str(), Reason, Side, ClOrdID, Symbol, OrderType, OrderQty * 1000) ;
            return;
        }
    }

	TSCTSEOrderRecord* NewOrder = new TSCTSEOrderRecord( NID, OIDStr, ClOrdID, Account, Symbol, OrderQty, Price, Side, timeInForce, SaleID, FMsgSource );
    FStore->AddOrder( (Int32)NID, NewOrder );
    NewOrder->NewOrderString( R010 );
    if( Application->SendOrder( (Int32)NID, R010, FIXMapper::WebID.c_str() ) == FALSE )
    {
        Reject( OIDStr.c_str(), GetErrorMessage("33003").c_str(), Side, ClOrdID, Symbol, OrderType, OrderQty );
        return;
    }
    ///< Send back the PENDING NEW ExecutionReport
    NewOrder->FillPendingNew( PendingNew );
    Application->SendToimFIX( FMsgSource, PendingNew );
}
//---------------------------------------------------------------------------
