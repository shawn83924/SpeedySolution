//---------------------------------------------------------------------------
#include "TTaifexTMPConnection.h"
//------------------------------------------------------------------------------
const char ChLUT[] = { "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" };
//---------------------------------------------------------------------------
inline char OrderSourceCode( nsOrderMessageDefine::OrderSourceEnum OrdSrc )
{
	switch( OrdSrc )
	{
		case nsOrderMessageDefine::osDedicatedLine: return 'D';
		case nsOrderMessageDefine::osAPI:           return 'A';
		case nsOrderMessageDefine::osMobile:        return 'M';
		case nsOrderMessageDefine::osWeb:           return 'W';
		case nsOrderMessageDefine::osPCApplication: return 'P';
		case nsOrderMessageDefine::osVoice:         return 'V';
		case nsOrderMessageDefine::osGeneral:
		default:              						return 'G';
	}
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::GenerateNID( TBaseMessage* Msg )
{
	if( Msg->GetNID() == 0 ) ///< NID not exists.
	{
		Int32 NewNID = TNetworkID::CreateClientOrderID( FCurrentConnectionID, Msg->GetMessageType() );
		Msg->SetNID( NewNID );
	}
}
//---------------------------------------------------------------------------
BOOL TTaifexTMPConnection::SendToOrderServer( nsOrderMessageDefine::MarketEnum Market,const UFC::AnsiString& Data, TMP::TMPMessage& OutMsg, long NID )
{
	if( FIsLogon == true && FCanSendOrder == TRUE )
	{
		UFC::MemoryStream  Stream( 256 );
		UFC::AnsiString    PublishKey;
		TMdMessage         Msg;

		OutMsg.ToStream( &Stream );
		Msg.GetMTree()->append( "ORDER", (const unsigned char*)Stream.GetBuffer(),Stream.GetSize() );
		Msg.SetIntegerValue( "BIN", 1 );
		Msg.SetIntegerValue( "NID", NID );
		Msg.SetStringValue( "ID", FID.c_str() );
		if( Data.Length() > 0 )///< Format: AE,User Data
			PublishKey.Printf("%s,%s", FID.c_str(), Data.c_str() );
		else
			PublishKey = FID;
		if( UFC::BufferedLog::DebugLevelExist(UFC::dlInformation ) )
		{
			UFC::AnsiString TMPMsg;

			OutMsg.ToString( TMPMsg );
			UFC::BufferedLog::DebugPrintf( UFC::dlInformation," TMP Out[%s]",TMPMsg.c_str() );
		}
		if( Market == nsOrderMessageDefine::mTWFutures )
			return FTransport->Send( SUBJECT_ORDER_FUT, PublishKey, &Msg );
		else if( Market == nsOrderMessageDefine::mTWOptions )
			return FTransport->Send( SUBJECT_ORDER_OPT, PublishKey, &Msg );
	}
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation," Send failed, not logon or can't send order." );
	return false;
}
///---------------------------------------------------------------------------
int TTaifexTMPConnection::GetPricePrecision( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& Symbol )
{
	int Digi;
	int pgseq = TMP::TMPMessage::GetSymbol( Symbol, Digi );
	if( pgseq == 0 )///<symbol not exist.
	{
		if( Market  == nsOrderMessageDefine::mTWFutures )
			return 2;
		else if( Market  == nsOrderMessageDefine::mTWOptions )
			return 3;
	}
	return Digi;
}
//---------------------------------------------------------------------------
const char* TTaifexTMPConnection::GetExchangePositionEffect( nsOrderMessageDefine::PositionEffectEnum pe )
{
    switch( pe ) 
    {
		case nsOrderMessageDefine::peOpen:          return "O";
		case nsOrderMessageDefine::peClose:         return "C";
		case nsOrderMessageDefine::peRolled:        return "D";
		case nsOrderMessageDefine::peDayTradeOpen:  return "D";
		case nsOrderMessageDefine::peTMPMarketMaker:return "9";
		default: return "0";
	}
}
//---------------------------------------------------------------------------
unsigned short TTaifexTMPConnection::GetBrokerIDIndex( TBaseMessage* Msg )
{
	UInt16  BrokerIndex = TMP::TMPMessage::GetBrokerID( Msg->GetBrokerID() );
	if( BrokerIndex == 0 )
		return FBrokerIndex;
	return BrokerIndex;
}
//---------------------------------------------------------------------------
int TTaifexTMPConnection::FillOrderBase( nsOrderMessageDefine::MarketEnum Market, TBaseMessage* Msg, TMP::TMPMessage& R01 )
{
	UFC::AnsiString Account( Msg->GetAccount() );
	TMP::TMPSymbol  TmpSymbol( Msg->GetSymbol() );
	UFC::AnsiString OrderID( Msg->GetOrderID() );
	UInt16          BrokerIndex = GetBrokerIDIndex( Msg );
	TMP::TMPInt64*  UsderDefInt64 = static_cast<TMP::TMPInt64*>(R01.GetField("user_define"));
	int             Precision = GetPricePrecision( Market, Msg->GetSymbol() );
	UInt8           Side = (UInt8)Msg->GetSide();

	if( BrokerIndex == 0 )
		return 12; ///< Incorrect FCM ID.
	if( FCMIDIndex == 0 )
		return 43; ///< Incorrect CM ID.
	if( TmpSymbol.ToBinaryMode( Side ) == FALSE )
	{
		UFC::BufferedLog::Printf( " Can not convert Symbol[%s] to binary mode", Msg->GetSymbol() );
		return 20; ///< incorrect symbol.
	}
	OrderID.PadThis( 5, '0' );
	UsderDefInt64->FromInteger( Msg->GetNID() );
	R01[ "cm_id" ]         = FCMIDIndex;
	R01[ "fcm_id" ]        = BrokerIndex;
	R01[ "order_no" ]      = OrderID;
	R01[ "Price" ]         = UFC::DoubleToInt( Msg->GetPrice(), Precision );
	R01[ "qty" ]           = Msg->GetOrderQty();
	R01[ "investor_acno" ] = Account.ToInt();
	R01[ "investor_flag" ] = Msg->GetAccountFlag();
	R01[ "Symbol" ]        = TmpSymbol;
	///< Set Side
	R01[ "Side" ] = Side;
	R01[ "ord_id" ] = Side*10000000 + Account.ToInt();
	///< Set OrderType
	switch( Msg->GetOrderType() )
	{
		case nsOrderMessageDefine::otMarket:		   	  R01[ "OrdType" ] = 1; break;
		case nsOrderMessageDefine::otMarketWithProtection:R01[ "OrdType" ] = 3; break;
		case nsOrderMessageDefine::otLimit:
		default:                                          R01[ "OrdType" ] = 2; break;
	}
	///< Set TimeInForce
	switch( Msg->GetTimeInForce() )
	{
		case nsOrderMessageDefine::tifIOC:  R01[ "TimeInForce" ] = 3; break;
		case nsOrderMessageDefine::tifFOK:  R01[ "TimeInForce" ] = 4; break;
		case nsOrderMessageDefine::tifTFXQ: R01[ "TimeInForce" ] = 8; break;
		default:                            R01[ "TimeInForce" ] = 0; break;
	}
	///< Set PositionEffect
	R01[ "PositionEffect" ] = GetExchangePositionEffect(Msg->GetPositionEffect());
	return 0;
}
//---------------------------------------------------------------------------
bool TTaifexTMPConnection::TrigerBeforeSendEvent( nsOrderMessageDefine::MarketEnum Market,
												  SendMessageType Type,
												  nsOrderMessageDefine::CxlRejResponseToEnum RejTo,
												  TBaseMessage* Msg )
{
	if( FSendEventListener != NULL )
	{
		bool CanSend = TRUE;
		UFC::AnsiString ErrMsg;

		FSendEventListener->OnBeforeSend( Market, Type, Msg, CanSend, ErrMsg );
		if( CanSend == false )
		{
			Reject( RejTo, ErrMsg, Msg );
			return FALSE;
		}
		return TRUE;
	}
	return TRUE;
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::RejectByAPI( nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo, TBaseMessage* Msg, int Code )
{
	UFC::AnsiString ErrMsg;

	ErrMsg.Printf( "Reject by SpeedyAPI code[%d]", Code );
	Reject( CxlRejResponseTo, ErrMsg, Msg );
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::NewOrder( TNewOrderMessage* Msg )
{
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation," TTaifexConnection::NewOrder" );
	nsOrderMessageDefine::MarketEnum  Market = Msg->GetMarket();
	UFC::AnsiString                   UserData( Msg->GetUserData());
	TMP::TMPMessage                   R01( TMP::tmtR01 );
	int                               Result;

	GenerateNID( Msg );
	R01[ "ExecType" ] = "0";///< New Order
	if( (Result = FillOrderBase( Market, Msg, R01 )) == 0 )
	{
		if( R01.FieldExists( "order_source") == true )
			R01[ "order_source" ] = OrderSourceCode( Msg->GetOrderSource() );
		if( R01.FieldExists( "info_source") == true )
			R01[ "info_source" ] = Msg->GetMarketDataSource();
		if( TrigerBeforeSendEvent( Market, smtNew, nsOrderMessageDefine::crrNew, Msg ) == TRUE )
			SendToOrderServer( Market, UserData, R01, Msg->GetNID() );
	}
	else
		RejectByAPI( nsOrderMessageDefine::crrNew, Msg,Result );
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::CancelOrder( TCancelOrderMessage* Msg )
{
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation," TTaifexConnection::CancelOrder" );
	nsOrderMessageDefine::MarketEnum  Market         = Msg->GetMarket();
	UFC::AnsiString                   UserData( Msg->GetUserData());
	TMP::TMPMessage 				  R01( TMP::tmtR01 );

	GenerateNID( Msg );
	R01[ "ExecType" ] = "4";///< Cancel Order
	FillOrderBase( Market, Msg, R01 );
	if( R01.FieldExists( "order_source") == true )
		R01[ "order_source" ] = OrderSourceCode( Msg->GetOrderSource() );
	if( R01.FieldExists( "info_source") == true )
		R01[ "info_source" ] = Msg->GetMarketDataSource();
	if( TrigerBeforeSendEvent( Market, smtCancel, nsOrderMessageDefine::crrCancel, Msg ) == TRUE )
		SendToOrderServer( Market, UserData, R01, Msg->GetNID() );
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::ReplaceOrder( TReplaceOrderMessage* Msg )
{
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation," TTaifexConnection::ReplaceOrder" );
	nsOrderMessageDefine::MarketEnum  Market         = Msg->GetMarket();
	UFC::AnsiString                   UserData( Msg->GetUserData());
	TMP::TMPMessage                   R01( TMP::tmtR01 );

	GenerateNID( Msg );
	if( Msg->GetOrderQty() == 0 )
		R01[ "ExecType" ] = "M";///< Replace Price
	else
		R01[ "ExecType" ] = "5";///< Replace Order
	FillOrderBase( Market, Msg, R01 );
	if( R01.FieldExists( "order_source") == true )
		R01[ "order_source" ] = OrderSourceCode( Msg->GetOrderSource() );
	if( R01.FieldExists( "info_source") == true )
		R01[ "info_source" ] = Msg->GetMarketDataSource();
	if( TrigerBeforeSendEvent( Market, smtReplace, nsOrderMessageDefine::crrReplace, Msg ) == TRUE )
		SendToOrderServer( Market, UserData, R01, Msg->GetNID() );
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::OrderStatusRequest( TOrderStatusRequest* Msg )
{
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation," TTaifexConnection::OrderStatusRequest" );
	nsOrderMessageDefine::MarketEnum  Market         = Msg->GetMarket();
	UFC::AnsiString                   UserData( Msg->GetUserData());
	TMP::TMPMessage 				  R01( TMP::tmtR01 );

	GenerateNID( Msg );
	R01[ "ExecType" ] = "I";///< Order status request
	FillOrderBase( Market, Msg, R01 );
	SendToOrderServer( Market, UserData, R01, Msg->GetNID() );
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::Quote( TQuoteMessage* Msg )
{
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation," TTaifexConnection::Quote" );
	nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
	UFC::AnsiString                  Account( Msg->GetAccount());
	TMP::TMPSymbol                   Symbol( Msg->GetSymbol() );
	UFC::AnsiString                  OrderID( Msg->GetQuoteID());
	UFC::AnsiString                  UserData( Msg->GetUserData());
	TMP::TMPMessage                  R09( TMP::tmtR09 );
	UInt16                           BrokerIndex = GetBrokerIDIndex( Msg );
	int                              Precision = GetPricePrecision( Market, Symbol );

	if( BrokerIndex == 0 )
	{
		RejectByAPI( nsOrderMessageDefine::crrQuote, Msg, 12 );
		return;
	}
	if( Symbol.ToBinaryMode( 1 ) == false )
	{
		RejectByAPI( nsOrderMessageDefine::crrQuote, Msg, 20 );
		return;
	}
	GenerateNID( Msg );
	OrderID.PadThis( 5, '0' );
	R09[ "ExecType" ]      = '0';///< New Quote
	R09[ "cm_id" ]         = FCMIDIndex;
	R09[ "fcm_id" ]        = BrokerIndex;
	R09[ "order_no" ]      = OrderID;
	R09[ "investor_acno" ] = Account.ToInt();
	R09[ "investor_flag" ] = Msg->GetAccountFlag();
	R09[ "Symbol" ]        = Symbol;
	R09[ "BidPx" ]         = UFC::DoubleToInt( Msg->GetBidPrice(), Precision );
	R09[ "OfferPx" ]       = UFC::DoubleToInt( Msg->GetAskPrice(), Precision );
	R09[ "BidSize" ]       = Msg->GetBidQty();
	R09[ "OfferSize"]      = Msg->GetAskQty();
	R09[ "PositionEffect"] = '9'; ///< For Market maker.
	if( Msg->GetTimeInForce() != nsOrderMessageDefine::tifROD )
		R09[ "TimeInForce" ] = 8; ///< 20 sec auto delete
	else
		R09[ "TimeInForce" ] = 0; ///< ROD Quote
	if( TrigerBeforeSendEvent( Market, smtQuoteNew, nsOrderMessageDefine::crrQuote, Msg ) == TRUE );
		SendToOrderServer( Market, UserData, R09, Msg->GetNID() );
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::QuoteCancel( TQuoteCancelMessage* Msg )
{
	UFC::BufferedLog::DebugPrintf( UFC::dlInformation," TTaifexConnection::QuoteCancel" );
	nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
	UFC::AnsiString                  Account( Msg->GetAccount());
	TMP::TMPSymbol                   Symbol( Msg->GetSymbol() );
	UFC::AnsiString                  OrderID( Msg->GetQuoteID());
	UFC::AnsiString                  UserData( Msg->GetUserData() );
	UInt16                           BrokerIndex = GetBrokerIDIndex( Msg );
	int                              Precision = GetPricePrecision( Market, Symbol );
	TMP::TMPMessage                  R09( TMP::tmtR09 );

	if( BrokerIndex == 0 )
	{
		RejectByAPI( nsOrderMessageDefine::crrQuote, Msg, 12 );
		return;
	}
	if( Symbol.ToBinaryMode( 1 ) == false )
	{
		RejectByAPI( nsOrderMessageDefine::crrQuote, Msg, 20 );
		return;
	}
	GenerateNID( Msg );
	OrderID.PadThis( 5, '0' );
	R09[ "ExecType" ]      = '4';///< Cancel Quote
	R09[ "cm_id" ]         = FCMIDIndex;
	R09[ "fcm_id" ]        = BrokerIndex;
	R09[ "order_no" ]      = OrderID;
	R09[ "investor_acno" ] = Account.ToInt();
	R09[ "investor_flag" ] = Msg->GetAccountFlag();
	R09[ "Symbol" ]        = Symbol;
	R09[ "BidPx" ]         = UFC::DoubleToInt( Msg->GetBidPrice(), Precision );
	R09[ "OfferPx" ]       = UFC::DoubleToInt( Msg->GetAskPrice(), Precision );
	R09[ "BidSize" ]       = 0;
	R09[ "OfferSize"]      = 0;
	R09[ "TimeInForce" ]   = 0; ///< ROD Quote
	R09[ "PositionEffect"] = '9'; ///< For Market maker.
	if( TrigerBeforeSendEvent( Market, smtQuoteCancel, nsOrderMessageDefine::crrQuoteCancel, Msg ) == TRUE );
		SendToOrderServer( Market, UserData, R09, Msg->GetNID() );
}
//------------------------------------------------------------------------------
char IntToChar( Int32 index )
{
    if( index >= 0 && index <= 61 )
        return ChLUT[ index ];
    return '0';
}
//------------------------------------------------------------------------------
void IntToOID( Int32 Seq, UFC::AnsiString& OID )
{
    char  ch1,ch2,ch3;
    Int32 IntPart;

    IntPart = Seq%100;
	ch3 =  IntToChar( (Seq/100)%62 );
    ch2 =  IntToChar( (Seq/(62*100))%62 );
    ch1 =  IntToChar( (Seq/(62*62*100))%62 );
	OID.Printf( "%c%c%c%02d", ch1,ch2,ch3,IntPart );
}
//---------------------------------------------------------------------------
void TTaifexTMPConnection::QuoteRequest( TQuoteRequestMessage* Msg )
{
	UFC::AnsiString OrderID;
	TMP::TMPMessage R07( TMP::tmtR07 );

	GenerateNID( Msg );
	IntToOID( (Int32)(Msg->GetNID() >> 12 ) , OrderID );
	R07[ "ord_id" ]        = Msg->GetNID();
	R07[ "order_no" ]      = OrderID;
	R07[ "fcm_id" ]        = TMP::TMPMessage::GetBrokerID( Msg->GetBrokerID() );
	R07[ "Symbol" ]        = Msg->GetSymbol();
	SendToOrderServer( Msg->GetMarket(), "", R07, Msg->GetNID() );
}
//---------------------------------------------------------------------------
