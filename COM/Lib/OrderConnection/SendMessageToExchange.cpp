//---------------------------------------------------------------------------
#include "TTaifexConnection.h"
//---------------------------------------------------------------------------
extern UFC::BufferedLog* Glog;
//---------------------------------------------------------------------------
inline bool IsNumber( char ch )
{
    if( ch >= '0' && ch <= '9' )
        return true;
    return false;
}
//---------------------------------------------------------------------------
inline bool IsAlpha( char ch )
{
    if( (ch >= '0' && ch <= '9' ) ||
            (ch >= 'a' && ch <= 'z' ) ||
            (ch >= 'A' && ch <= 'Z' )  )
       return true;
    return false;
}
//---------------------------------------------------------------------------
char TTaifexConnection::OrderSourceCode( nsOrderMessageDefine::OrderSourceEnum OrdSrc )
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
        default:          			    return 'G';
    }
}
//---------------------------------------------------------------------------
bool TTaifexConnection::CheckOrderID( const char* OrderID )
{
    UFC::AnsiString OID( OrderID );

    if( OID.Length( ) != 5 ) return false;
    for( register int i = 0; i < 5; i++ )
    {
        if( IsAlpha( OID[i] ) == false )
            return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::CheckOrderID( nsOrderMessageDefine::MarketEnum OrderMarket, const char*OrderID )
{
    UFC::AnsiString orderIDStr( OrderID );
    Int32 orderIDLength = orderIDStr.Length();

    if( ( OrderMarket == nsOrderMessageDefine::mForeignFutures ) ||
            ( OrderMarket == nsOrderMessageDefine::mForeignOptions ) )
    {
        for( register int i = 0; i < orderIDLength; i++ )
        {
            if( !IsAlpha( orderIDStr[i] ) && ( orderIDStr[i] != '_' ) )
                return false;
        }
    }
    else if ( OrderMarket == nsOrderMessageDefine::mForeignStock )
    {
        if ( orderIDLength == 0 )
            return false;
    }
    else
    {
        if( orderIDLength != 5 ) return false;
        for( register int i = 0; i < orderIDLength; i++ )
        {
            if( !IsAlpha( orderIDStr[i] ) )
                return false;
        }
    }
    return true;
}  //TTaifexConnection::CheckOrderID()
//---------------------------------------------------------------------------
bool TTaifexConnection::IsPATS( nsOrderMessageDefine::MarketEnum Market )
{
	if( Market == nsOrderMessageDefine::mForeignFutures ||
		Market == nsOrderMessageDefine::mForeignOptions )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::IsChina( nsOrderMessageDefine::MarketEnum Market )
{
	if(	Market == nsOrderMessageDefine::mCNFutures ||
		Market == nsOrderMessageDefine::mCNOptions )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::IsTAIWAN( nsOrderMessageDefine::MarketEnum Market )
{
	switch( Market )
	{
		case nsOrderMessageDefine::mOTC:
		case nsOrderMessageDefine::mTSE:
		case nsOrderMessageDefine::mES:
		case nsOrderMessageDefine::mTWFutures:
		case nsOrderMessageDefine::mTWOptions: return true;
		default : return false;
	}
}
//---------------------------------------------------------------------------
Int64 TTaifexConnection::GenerateNID( nsOrderMessageDefine::MessageTypeEnum Type )
{
    return FNetworkID.CreateClientOrderID( FCurrentConnectionID, Type );
}
//---------------------------------------------------------------------------
void TTaifexConnection::GenerateNID( TBaseMessage* Msg )
{
    ///< NID not exists(=0). or always generate new NID.
    if( FAlwaysGenNID == TRUE || Msg->GetNID() == 0 )
    {
        Int64 NewNID = GenerateNID( Msg->GetMessageType() );
        Msg->SetNID( NewNID );
    }
}
//---------------------------------------------------------------------------
int TTaifexConnection::GetCurrentMessageTime( void )
{
    return UFC::GetHHMMSS();
}
//---------------------------------------------------------------------------
int TTaifexConnection::StringToInt( const UFC::AnsiString& StringValue, long DefaultValue )
{
    int len = StringValue.Length();
    if( len > 0 )
    {
        if( len < 10 )
	    return StringValue.ToInt(); ///< Int32 
        else
            return (int)StringValue.ToInt64(); ///< Int64
    }
    else
	return DefaultValue;
}
//---------------------------------------------------------------------------
void TTaifexConnection::GetAPIError( UFC::AnsiString& ErrorStr, int ErrorCode, const UFC::AnsiString& DefStr )
{
	UFC::AnsiString StatusCode( ErrorCode );
	UFC::AnsiString LUTErr;

	if( FExchangeError->GetValue( "API", StatusCode, LUTErr ) == TRUE )
		ErrorStr = LUTErr;
	else
		ErrorStr = DefStr;
}
//---------------------------------------------------------------------------
UFC::AnsiString  TTaifexConnection::GetExchangeOrderType( nsOrderMessageDefine::MarketEnum Market, nsOrderMessageDefine::OrderTypeEnum Type )
{
	if( Market  == nsOrderMessageDefine::mTWFutures || Market  == nsOrderMessageDefine::mTWOptions )
    {
        if( Type == nsOrderMessageDefine::otMarket )
                return "M";
        else if( Type == nsOrderMessageDefine::otMarketWithProtection )
                return "P";
        else
                return "L";
    }
	else if( Market  == nsOrderMessageDefine::mTSE || Market  == nsOrderMessageDefine::mOTC || Market  == nsOrderMessageDefine::mES)
    {
        if( Type == nsOrderMessageDefine::otMarket )
                return "1";
        else if( Type == nsOrderMessageDefine::otMarketWithProtection )
                return "2";
        else
                return "2";
    }
    else
		return "L";
}
//---------------------------------------------------------------------------
bool TTaifexConnection::IsTWFutures( const UFC::AnsiString& Symbol )
{
	if( FFUTSymbol != NULL && FFUTSymbol->ValueExists( "FuturesProducts", Symbol.SubString( 0, 3 ) ) == TRUE )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::IsTWOptions( const UFC::AnsiString& Symbol )
{
	if( FOPTSymbol != NULL && FOPTSymbol->ValueExists( "OptionProducts", Symbol.SubString( 0, 3 ) ) == TRUE )
		return true;
	return false;
}
//---------------------------------------------------------------------------
const char* TTaifexConnection::GetExchangePositionEffect( nsOrderMessageDefine::PositionEffectEnum pe )
{
    switch( pe )
    {
        case nsOrderMessageDefine::peOpen:          return "0";
        case nsOrderMessageDefine::peClose:         return "1";
		case nsOrderMessageDefine::peDayTrade:      return "2";
        case nsOrderMessageDefine::peAuto:          return " ";
        case nsOrderMessageDefine::peDayTradeOpen:  return "3";
        case nsOrderMessageDefine::peAutoToday:     return "5";
        case nsOrderMessageDefine::peForceClose:    return "7";
        case nsOrderMessageDefine::peTMPMarketMaker:return "9";
        case nsOrderMessageDefine::peTAIFEXAuto:    return "A";
        default: return "0";
    }
}
//---------------------------------------------------------------------------
//
// Fill TAIFEX,TWSE,OTC Order basic information
//
//---------------------------------------------------------------------------
void TTaifexConnection::FillOrderBase( nsOrderMessageDefine::TradingSessionIDEnum TradingSession, nsOrderMessageDefine::MarketEnum Market, TBaseMessage* Msg, UFC::TRenderData* R010Render )
{
	UFC::AnsiString Account  = Msg->GetAccount();
	UFC::AnsiString Symbol   = Msg->GetSymbol();
	UFC::AnsiString OrderID  = Msg->GetOrderID();
	UFC::AnsiString BrokerID = Msg->GetBrokerID();
	UFC::AnsiString MaturityMonthYear = Msg->GetMaturityMonthYear();
	UFC::AnsiString Side( (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
	UFC::AnsiString OrderType = GetExchangeOrderType( Market, Msg->GetOrderType());
	UFC::AnsiString NewSymbol;
	int             Precision;

	Symbol.TrimRight();
	NewSymbol = Symbol;
	if( Market == nsOrderMessageDefine::mTWFutures && FNewFutSymbol == FALSE )
		FFutSymbol.ConvertToNewSymbol( NewSymbol, Symbol, MaturityMonthYear );
	Precision = GetPricePrecision( Market, TradingSession, NewSymbol );
	R010Render->Reset();
	R010Render->SetField( "MessageTime", GetCurrentMessageTime() );
	if( BrokerID.Length() > 0 )
		R010Render->SetField( "BrokerID", BrokerID.c_str() );
	else
		R010Render->SetField( "BrokerID", GetBrokerID( Market ).c_str() );
	R010Render->SetField( "Account", StringToInt( Account, 0 ) );
	R010Render->SetField( "AccountFlag", Msg->GetAccountFlag() );
	R010Render->SetField( "MaturityMonthYear", StringToInt( MaturityMonthYear, 0 ) );
	R010Render->SetField( "Symbol", NewSymbol.c_str() );
	R010Render->SetField( "StrikePrice", UFC::DoubleToInt( Msg->GetStrikePrice(), Precision ) );
	R010Render->SetField( "Side", Side.c_str() );
	R010Render->SetField( "OrderType", OrderType.c_str() );
	R010Render->SetField( "OrderID", OrderID.c_str() );
	R010Render->SetField( "Qty", Msg->GetOrderQty() );
	R010Render->SetField( "ClearMemberID", FCMID.c_str() );
	R010Render->SetField( "OpenOffsetFlag", GetExchangePositionEffect( Msg->GetPositionEffect()));
	if( Msg->GetOrderType() == nsOrderMessageDefine::otMarket ||
		Msg->GetOrderType() == nsOrderMessageDefine::otMarketWithProtection ) ///< Market Order
		R010Render->SetField( "Price", 0 );
	else
		R010Render->SetField( "Price", UFC::DoubleToInt(Msg->GetPrice(), Precision) );
	if( Market == nsOrderMessageDefine::mTSE || Market == nsOrderMessageDefine::mOTC || Market == nsOrderMessageDefine::mES )
	{
		switch( Msg->GetTimeInForce() )
		{
			case nsOrderMessageDefine::tifROD:  R010Render->SetField( "TimeInForce", "0" ); break;
			case nsOrderMessageDefine::tifIOC:  R010Render->SetField( "TimeInForce", "3" ); break;
			case nsOrderMessageDefine::tifFOK:  R010Render->SetField( "TimeInForce", "4" ); break;
			default:                            R010Render->SetField( "TimeInForce", "0" ); break;
		}
	}
	else
	{
		switch( Msg->GetTimeInForce() )
		{
			case nsOrderMessageDefine::tifROD:  R010Render->SetField( "TimeInForce", "R" ); break;
			case nsOrderMessageDefine::tifIOC:  R010Render->SetField( "TimeInForce", "I" ); break;
			case nsOrderMessageDefine::tifFOK:  R010Render->SetField( "TimeInForce", "F" ); break;
			default:                            R010Render->SetField( "TimeInForce", "R" ); break;
		}
	}
}
//---------------------------------------------------------------------------
//
// Fill globol market Order basic information
//
//---------------------------------------------------------------------------
void TTaifexConnection::FillPATSOrderBase( TBaseMessage* Msg, UFC::NameValueMessage* PATSMessage )
{
    nsOrderMessageDefine::MessageTypeEnum MsgType = Msg->GetMessageType();
    UFC::AnsiString Account  = Msg->GetAccount();
    UFC::AnsiString Symbol   = Msg->GetSymbol();
    UFC::AnsiString OrderID  = Msg->GetOrderID();
    UFC::AnsiString BrokerID = Msg->GetBrokerID();
    UFC::AnsiString MaturityMonthYear = Msg->GetMaturityMonthYear();
    UFC::AnsiString Side( (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
    UFC::AnsiString localIP = Msg->GetLocalIP();
    UFC::AnsiString aeno = Msg->GetAENO();
    UFC::AnsiString orderChannel = Msg->GetOrderChannel();
    UFC::AnsiString TimeNow, Today;

    UFC::GetTimeString( TimeNow, TRUE );
    UFC::GetTradeYYYYMMDD( Today, TRUE );
    Symbol.TrimRight();
    PATSMessage->Append( "oid", OrderID );
    if( BrokerID.Length() > 0 ) PATSMessage->Append( "fcm", BrokerID );
    PATSMessage->Append( "acc", Account );

    UFC::AnsiString orderTypeStr = "";
    switch( Msg->GetOrderType() )
    {
        case nsOrderMessageDefine::otMarket:               orderTypeStr = "MKT"; break;
        case nsOrderMessageDefine::otStop:                 orderTypeStr = "STP"; break;
        case nsOrderMessageDefine::otStopLimit:            orderTypeStr = "SWL"; break;
        case nsOrderMessageDefine::otMarketWithProtection: orderTypeStr = "MWP"; break;
        case nsOrderMessageDefine::otMarketOnClose:        orderTypeStr = "MOC"; break;
        case nsOrderMessageDefine::otLimitOnClose:         orderTypeStr = "LOC"; break;
        case nsOrderMessageDefine::otLimit:
        default:                                           orderTypeStr = "LMT";
    }
    if( MsgType == nsOrderMessageDefine::mtCancel )
    {
        PATSMessage->Append( "oty", "CXL" );
        if( orderTypeStr.Length() > 0 ) PATSMessage->Append( "ooty", orderTypeStr );
    }
    else if( MsgType == nsOrderMessageDefine::mtReplace )
    {
        PATSMessage->Append( "oty", "UPD" );
        if( orderTypeStr.Length() > 0 ) PATSMessage->Append( "ooty", orderTypeStr );
    }
    else
        PATSMessage->Append( "oty", orderTypeStr );

    switch( Msg->GetPositionEffect() )
    {
        case nsOrderMessageDefine::peOpen:         PATSMessage->Append("pe", "O" ); break;
        case nsOrderMessageDefine::peClose:        PATSMessage->Append("pe", "C" ); break;
        case nsOrderMessageDefine::peDayTrade:     PATSMessage->Append("pe", "D" ); break;
        case nsOrderMessageDefine::peDayTradeOpen: PATSMessage->Append("pe", "d" ); break;
        case nsOrderMessageDefine::peAutoToday:    PATSMessage->Append("pe", "a" ); break;
        case nsOrderMessageDefine::peAuto:         PATSMessage->Append("pe", "A" ); break;
        default:                                   PATSMessage->Append("pe", "O" );
    }  //switch( Msg->GetPositionEffect() )

    PATSMessage->Append( "sym", Symbol );
    if( MaturityMonthYear.Length() > 0 ) PATSMessage->Append( "mmy", MaturityMonthYear );
    PATSMessage->Append( "qty", Msg->GetOrderQty() );
    PATSMessage->Append( "px",  (double)Msg->GetPrice(), true );
    PATSMessage->Append( "pxb", (long)Msg->GetPxBase() );
    PATSMessage->Append( "bs",  Side );
    
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mForeignFutures: 
            PATSMessage->Append( "ct", "0" );
            PATSMessage->Append( "cp", "N" );
            break;
        case nsOrderMessageDefine::mForeignOptions: 
            PATSMessage->Append( "ct", "1");
            PATSMessage->Append( "stkpx", (double)Msg->GetStrikePrice(), true );
            
            if (Msg->GetEventType() == nsOrderMessageDefine::evtCall)
                PATSMessage->Append( "cp", "C" );
            else if (Msg->GetEventType() == nsOrderMessageDefine::evtPut)
                PATSMessage->Append( "cp", "P" );
            break;
        case nsOrderMessageDefine::mForeignStock:
            PATSMessage->Append( "ct", "2" );
            break;
        default: break;
    }
    
    if( aeno.Length() > 0 )
        PATSMessage->Append( "aeno", aeno );
    
    if( localIP.Length() > 0 ) PATSMessage->Append( "lip", localIP );
    if ( orderChannel.Length() > 0 ) PATSMessage->Append( "och", orderChannel );
    PATSMessage->Append( "dt", Today );
    PATSMessage->Append( "rct", TimeNow );
    PATSMessage->Append( "cd", "S001" );
}
//---------------------------------------------------------------------------
//
// Fill China market Order basic information
//
//---------------------------------------------------------------------------
void TTaifexConnection::FillChinaOrderBase( TBaseMessage* Msg, UFC::NameValueMessage* GLQHMessage )
{
	UFC::AnsiString      Symbol      = Msg->GetSymbol();
	UFC::AnsiString      Side( (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
	UFC::AnsiString      TimeNow,Today;
	TNewOrderMessage*    oNewOrder;
	TCancelOrderMessage* oCancelOrder;
	TOrderStatusRequest* oOrderStatus;

	UFC::GetTimeString( TimeNow , TRUE );
	Symbol.TrimRight();
	GLQHMessage->Append("bs",  Side );
	GLQHMessage->Append("sym", Symbol );
	if( (oNewOrder = dynamic_cast<TNewOrderMessage*>(Msg)) != NULL )
	{
		UFC::GetTradeYYYYMMDD( Today , TRUE );
		GLQHMessage->Append("acc", Msg->GetAccount() );
		GLQHMessage->Append("qty", Msg->GetOrderQty() );
		GLQHMessage->Append("px",  (double)Msg->GetPrice(), true );
		if( oNewOrder->GetOrderType() == nsOrderMessageDefine::otMarket ||
			oNewOrder->GetOrderType() == nsOrderMessageDefine::otMarketWithProtection )
			GLQHMessage->Append("oty", "MKT" );
		else
			GLQHMessage->Append("oty", "LMT" );
		if( oNewOrder->GetPositionEffect() == nsOrderMessageDefine::peOpen )
			GLQHMessage->Append("pe", "O" );  //Open
		else if( oNewOrder->GetPositionEffect() == nsOrderMessageDefine::peClose )
			GLQHMessage->Append("pe", "C" );  //Close
		else if( oNewOrder->GetPositionEffect() == nsOrderMessageDefine::peDayTrade )
			GLQHMessage->Append("pe", "D" );  //Close Today
		else if( oNewOrder->GetPositionEffect() == nsOrderMessageDefine::peAutoToday )
			GLQHMessage->Append("pe", "a" );  //Auto Today
		else
			GLQHMessage->Append("pe", "A" );  //Auto
		GLQHMessage->Append("dt",  Today );
		switch( oNewOrder->GetTimeInForce() )
		{
			case nsOrderMessageDefine::tifROD:  GLQHMessage->Append("tif", "R" ); break;
			case nsOrderMessageDefine::tifIOC:  GLQHMessage->Append("tif", "I" ); break;
			case nsOrderMessageDefine::tifFOK:  GLQHMessage->Append("tif", "F" ); break;
			default: GLQHMessage->Append("tif", "R" ); break;
		}
	}
	else if( (oCancelOrder = dynamic_cast<TCancelOrderMessage*>(Msg)) != NULL )
	{
		GLQHMessage->Append( "cnid",(long)oCancelOrder->GetCNID() );
		GLQHMessage->Append( "oid", Msg->GetOrderID());
	}
	else if( (oOrderStatus = dynamic_cast<TOrderStatusRequest*>(Msg)) != NULL )
	{
		GLQHMessage->Append( "cnid",oOrderStatus->GetOrigNID() );
		GLQHMessage->Append( "oid", Msg->GetOrderID());
	}
	GLQHMessage->Append("rct",   TimeNow );
}
//---------------------------------------------------------------------------
void TTaifexConnection::WriteStringToStream( const UFC::AnsiString& Name, const UFC::AnsiString& Value, UFC::MemoryStream& WriteStream )
{
    UFC::NInt32  NetInt;
    int          StrSize = Value.Length() + 1;

    Name.SaveToStream( &WriteStream );
    NetInt = StrSize;
    NetInt.SaveToStream( &WriteStream );
    NetInt = M_STRING;
    NetInt.SaveToStream( &WriteStream );
    WriteStream.Write( Value.c_str() , StrSize );
}
//---------------------------------------------------------------------------
void TTaifexConnection::WriteIntToStream( const UFC::AnsiString& Name, int Value, UFC::MemoryStream& WriteStream )
{
    UFC::NInt32  NetInt;

    Name.SaveToStream( &WriteStream );
    NetInt = sizeof(int);
    NetInt.SaveToStream( &WriteStream );
    NetInt = M_INT;
    NetInt.SaveToStream( &WriteStream );
    NetInt = Value;
    NetInt.SaveToStream( &WriteStream );
}
//---------------------------------------------------------------------------
void TTaifexConnection::WriteInt64ToStream( const UFC::AnsiString& Name, Int64 Value, UFC::MemoryStream& WriteStream )
{
    UFC::NInt32  NetInt;
    UFC::NInt64  NetInt64;

    Name.SaveToStream( &WriteStream );   ///< Save name
    NetInt = sizeof(int);
    NetInt.SaveToStream( &WriteStream ); ///< Save size
    NetInt = M_INT64;
    NetInt.SaveToStream( &WriteStream ); ///< Save data type
    NetInt64 = Value;
    NetInt64.SaveToStream( &WriteStream ); ///< Save binary data
}
//------------------------------------------------------------------------------
bool  TTaifexConnection::ControlTroughput( void )
{
    UInt32 Now = UFC::GetTickCountMS();
    UInt32 TimeDiff = Now - FFirstOrderTick;

    FOutCount++;
    if( TimeDiff >= 1000 ) ///< More then 1 sec
    {
        FFirstOrderTick = Now;///< Reset timer.
        FOutCount       = 0;  ///< Reset counter.
        return true;
    }
    else ///< Less then 1 sec
    {
        if( FOutCount >= FOrderPerSec )
            return false;
        else
			return true;
	}
}
//---------------------------------------------------------------------------
bool TTaifexConnection::TriggerEventAndSend( TBaseMessage* Msg,
                                             const UFC::AnsiString& Order,
                                             const UFC::AnsiString& UserData,
                                             SendMessageType SendMsgType,
                                             nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo,
                                             nsOrderMessageDefine::TradingSessionIDEnum TradingSession,
                                             const UFC::AnsiString& Source,
                                             Int64 CNID )
{
    if( Order.Length() > 0 )
    {
        UFC::AnsiString ErrorStr = "", TmpStr;
        UFC::AnsiString msgCAPlainText = Msg->GetCAPlainText();
        UFC::AnsiString signatureStr = Msg->GetCASignature();
        UFC::AnsiString caSerialNumber = "";
        bool CanSend = false;

        ///< Check CA
        CAResultData caResult;
        if (FApiCAObjPtr != 0)
        {
            if( FApiCAObjPtr->IsWorking() )
            {
                caResult.SetCAObj( FApiCAObjPtr );
                caResult.SetDefaultBrokerID( GetBrokerID( Msg->GetMarket() ) );
                if (FApiCAObjPtr->GenerateReqMsgSignatureAndCAData(Msg, Order, caResult) == 0)
                {
                    CanSend = true;
                    msgCAPlainText = caResult.GetPlainText();
                    signatureStr = caResult.GetSignature();
                    caSerialNumber = caResult.GetSerialNumber();
//                    Glog->fprintf( " %s() Signature[%s].", __func__, signatureStr.c_str() );
                    Glog->fprintf( " %s() PlainText[%s].", __func__, msgCAPlainText.c_str() );
                    Glog->fprintf( " %s() Subject[%s].", __func__, caResult.GetSubject().c_str() );
                    Glog->fprintf( " %s() NotBefore[%s].", __func__, caResult.GetNotBefore().c_str() );
                    Glog->fprintf( " %s() NotAfter[%s].", __func__, caResult.GetNotAfter().c_str() );
                    Glog->fprintf( " %s() SerialNumber[%s].", __func__, caSerialNumber.c_str() );
                }
                else
                {
                    ErrorStr = caResult.GetResultMsg();
                    Glog->fprintf( " %s() Error:%s.", __func__, ErrorStr.c_str() );
                }
            }
            else
                ErrorStr.Printf( "CA Object is not Working." );
        }
        else
            CanSend = true;
/*
		if( FCAObjPtr != 0 )
		{
			if( FCAObjPtr->IsWork() )
			{
				int generateResult = FCAObjPtr->GenerateSignature( Order, signatureStr );
				if( generateResult == 0 )
					CanSend = true;
				else
					ErrorStr.Printf( "Generate New Order Signature failed[%d]", generateResult );
			}
			else
				ErrorStr.Printf( "CA Object is not working, %s", FCAObjPtr->GetErrorMessage().c_str() );
		}
		else
			CanSend = true;
*/
        ///< Check OnBeforeSend callback function.
        if( CanSend && ( FSendEventListener != NULL ) )
            FSendEventListener->OnBeforeSend( Msg->GetMarket(), SendMsgType, Msg, CanSend, ErrorStr );
        ///< Send order to SpeedyGateway
        if( CanSend == true )
        {
            if( ControlTroughput() == true ) ///< Pass troughput control
            {
                UFC::AnsiString peerIP = Msg->GetPeerIP();
                UFC::AnsiString msgCASessionID = Msg->GetCASessionID();
                /* added & modified by Kenny to support Stop Price Strategy. 2026/01/02 */
                UFC::AnsiString strategy = "";
                TNewOrderMessage* neworder_message = dynamic_cast<TNewOrderMessage*>(Msg);
                if (neworder_message)
                    strategy = neworder_message->GetStopOrderSetting();
                BOOL isSendSuccess = SendToOrderServerEx( Msg->GetMarket(), UserData, Order.c_str(), TradingSession, Source, Msg->GetGroup(), Msg->GetNID(), CNID, peerIP, signatureStr, msgCAPlainText, msgCASessionID, caSerialNumber, strategy );
                /* end added & modified by Kenny */
                if (isSendSuccess == TRUE)
                {
                    Glog->fprintf( " %s() Send to Order Server success.", __func__ );
                    return true;
                }
                else
                {
                    Glog->fprintf( " %s() Send to Order Server failed.", __func__ );
                    return false;
                }
//                return SendToOrderServerEx( Msg->GetMarket(), UserData, Order.c_str(), TradingSession, Source, Msg->GetGroup(), Msg->GetNID(), CNID, peerIP, signatureStr, msgCAPlainText, msgCASessionID, caSerialNumber );
            }
            else ///< exceed throughput limit
            {
                GetAPIError( TmpStr, 1001, "Throughput exceed %d orders per sec." );
                ErrorStr.Printf( TmpStr.c_str(), FOrderPerSec );
                Reject( CxlRejResponseTo, ErrorStr, UserData, Msg, TradingSession );
                return false;
            }
        }
        else
        {
            Reject( CxlRejResponseTo, ErrorStr, UserData, Msg, TradingSession );
            return false;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
void TTaifexConnection::NegotiatePriceMessage( const UFC::AnsiString& Msg, const UFC::AnsiString& Data )
{
    Int32 NewNID = FNetworkID.CreateClientOrderID( FCurrentConnectionID, nsOrderMessageDefine::mtNew );

    SendToOrderServerEx( nsOrderMessageDefine::mOTC, Data, Msg, nsOrderMessageDefine::tsNegotiatePx, "", 0, NewNID, 0, "", "" );
}
//---------------------------------------------------------------------------
void TTaifexConnection::NoOrderIDReject( nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo, TBaseMessage* Msg, const UFC::AnsiString& UserData, nsOrderMessageDefine::TradingSessionIDEnum TradingSession )
{
    UFC::AnsiString ErrorStr;

    if( ( FExchangeError == NULL ) ||  ( FExchangeError->GetValue( "TAIFEX", "18", ErrorStr) == FALSE ) )
         ErrorStr.Printf("Exchange error code:18" );
    Reject( CxlRejResponseTo, ErrorStr, UserData, Msg, TradingSession );
}
//---------------------------------------------------------------------------
void TTaifexConnection::FillNewOrderFunctionCode( nsOrderMessageDefine::TradingSessionIDEnum TradingSession, nsOrderMessageDefine::MarketEnum Market, TNewOrderMessage* Msg, UFC::TRenderData* R010Render )
{
    if (Market == nsOrderMessageDefine::mOTC || Market == nsOrderMessageDefine::mTSE || Market == nsOrderMessageDefine::mES )
    {
        switch (TradingSession)
        {
            case nsOrderMessageDefine::tsNormal:
            case nsOrderMessageDefine::tsOddLot:
            case nsOrderMessageDefine::tsOffHour:
                    if (Msg->GetSide() == nsOrderMessageDefine::sBuy)
                        R010Render->SetField("FunctionCode", 1);
                    else
                        R010Render->SetField("FunctionCode", 2);
                    if (TradingSession == nsOrderMessageDefine::tsOddLot)
                    {
                        R010Render->SetField("ExchangeCode", "2" ); ///< New T010
                        R010Render->SetField("TradeKind", Msg->GetTSETradeKind());
                        R010Render->SetField("ObjectBroker", Msg->GetTSEObjectBroker());
                    }
                    else
                    {
                        R010Render->SetField("ExchangeCode", Msg->GetTSEExchangeCode());
                        R010Render->SetField("TSEOrderType", Msg->GetTSEOrderType());
                    }
                    break;
            case nsOrderMessageDefine::tsAuction:
            case nsOrderMessageDefine::tsLend:
            case nsOrderMessageDefine::tsTender:
            case nsOrderMessageDefine::tsTenderEx:
                    R010Render->SetField("FunctionCode", 1);
                    break;
            default: break;
        }
        R010Render->SetField("STK-SEQ-NO", Msg->GetStockSeqNo());
        R010Render->SetField("KEEP", Msg->GetKeep());
    }
    else ///< for TAIFEX
        R010Render->SetField("FunctionCode", 1); ///< Futures and Option
}
//---------------------------------------------------------------------------
void TTaifexConnection::FillCancelOrderFunctionCode( nsOrderMessageDefine::TradingSessionIDEnum TradingSession, nsOrderMessageDefine::MarketEnum Market, TCancelOrderMessage* Msg, UFC::TRenderData* R010Render )
{
	if( Market == nsOrderMessageDefine::mOTC || Market == nsOrderMessageDefine::mTSE|| Market == nsOrderMessageDefine::mES)
    {
        switch (TradingSession)
        {
            case nsOrderMessageDefine::tsNormal:
            case nsOrderMessageDefine::tsOddLot:
            case nsOrderMessageDefine::tsOffHour:
                    R010Render->SetField("FunctionCode", 4);
                    if (TradingSession == nsOrderMessageDefine::tsOddLot)
                    {
                        R010Render->SetField("ExchangeCode", "2" ); ///< New T010
                        R010Render->SetField("TradeKind", Msg->GetTSETradeKind());
                        R010Render->SetField("ObjectBroker", Msg->GetTSEObjectBroker());
                    }
                    else
                    {
                        R010Render->SetField("ExchangeCode", Msg->GetTSEExchangeCode());
                        R010Render->SetField("TSEOrderType", Msg->GetTSEOrderType());
                    }
                    break;
            case nsOrderMessageDefine::tsAuction:
            case nsOrderMessageDefine::tsLend:
            case nsOrderMessageDefine::tsTender:
            case nsOrderMessageDefine::tsTenderEx:
                    R010Render->SetField("FunctionCode", 2);
                    break;
            default: break;
        }
        R010Render->SetField("STK-SEQ-NO", Msg->GetStockSeqNo());
        R010Render->SetField("KEEP", Msg->GetKeep());
    }
    else ///< for TAIFEX
        R010Render->SetField("FunctionCode", 3);
}
//---------------------------------------------------------------------------
void TTaifexConnection::FillReplaceOrderFunctionCode( nsOrderMessageDefine::TradingSessionIDEnum TradingSession, nsOrderMessageDefine::MarketEnum Market, TReplaceOrderMessage* Msg, UFC::TRenderData* R010Render )
{
	if( Market == nsOrderMessageDefine::mOTC || Market == nsOrderMessageDefine::mTSE|| Market == nsOrderMessageDefine::mES )
    {        
        if( TradingSession == nsOrderMessageDefine::tsOddLot )
        {
            R010Render->SetField( "FunctionCode", 3 );
            R010Render->SetField( "ExchangeCode", "2" ); ///< New T010
            R010Render->SetField( "TradeKind",    Msg->GetTSETradeKind());
            R010Render->SetField( "ObjectBroker", Msg->GetTSEObjectBroker());
        }
        else
        {
            if( Msg->GetOrderQty() == 0 )
                R010Render->SetField( "FunctionCode", 6 ); ///< Replace Px
            else
                R010Render->SetField( "FunctionCode", 3 ); ///< Replace Qty
            R010Render->SetField( "TSEOrderType", Msg->GetTSEOrderType() );
            R010Render->SetField( "ExchangeCode", Msg->GetTSEExchangeCode() );
        }
        R010Render->SetField( "STK-SEQ-NO", Msg->GetStockSeqNo() );
        R010Render->SetField( "KEEP", Msg->GetKeep() );
    }
    else ///< for TAIFEX
    {
        if( Msg->GetOrderQty() == 0 )
            R010Render->SetField( "FunctionCode", 6 ); ///< Replace Px
        else
            R010Render->SetField( "FunctionCode", 2 ); ///< Replace Qty
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::FillOrderStatusRequestFunctionCode( nsOrderMessageDefine::TradingSessionIDEnum TradingSession, nsOrderMessageDefine::MarketEnum Market, TOrderStatusRequest* Msg, UFC::TRenderData* R010Render )
{
	if (Market == nsOrderMessageDefine::mOTC || Market == nsOrderMessageDefine::mTSE|| Market == nsOrderMessageDefine::mES)
    {
        switch (TradingSession)
        {
            case nsOrderMessageDefine::tsNormal:
            case nsOrderMessageDefine::tsOddLot:
            case nsOrderMessageDefine::tsOffHour:
                    R010Render->SetField("FunctionCode", 5);
                    if (TradingSession == nsOrderMessageDefine::tsOddLot)
                    {
                        R010Render->SetField("ExchangeCode", "2" ); ///< New T010
                        R010Render->SetField("TradeKind", Msg->GetTSETradeKind());
                        R010Render->SetField("ObjectBroker", Msg->GetTSEObjectBroker());
                    }
                    else
                    {
                        R010Render->SetField("ExchangeCode", Msg->GetTSEExchangeCode());
                        R010Render->SetField("TSEOrderType", Msg->GetTSEOrderType());
                    }
                    break;
            case nsOrderMessageDefine::tsAuction:
            case nsOrderMessageDefine::tsLend:
            case nsOrderMessageDefine::tsTender:
            case nsOrderMessageDefine::tsTenderEx:
                    R010Render->SetField("FunctionCode", 4);
                    break;
            default: break;
        }
        R010Render->SetField("STK-SEQ-NO", Msg->GetStockSeqNo());
        R010Render->SetField("KEEP", Msg->GetKeep());
    }
    else  ///< for TAIFEX
        R010Render->SetField("FunctionCode", 5); ///< Futures and Option
}
//---------------------------------------------------------------------------
bool TTaifexConnection::SupportSellSide( TBaseMessage* Msg )
{
    bool IsSupport = FALSE;

    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mOTC:
		case nsOrderMessageDefine::mTSE:
		case nsOrderMessageDefine::mES:            IsSupport = FSupportSellSide.Exists( ssTSEOTC );break;
		case nsOrderMessageDefine::mTWFutures:
		case nsOrderMessageDefine::mTWOptions:     IsSupport = FSupportSellSide.Exists( ssTAIFEX );break;
        case nsOrderMessageDefine::mForeignStock:
        case nsOrderMessageDefine::mForeignFutures:
        case nsOrderMessageDefine::mForeignOptions:IsSupport = FSupportSellSide.Exists( ssForeignExchange ); break;
        default:                                   IsSupport = false;break;    
    }
    return IsSupport;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FLEX Symbol
// TXOOX
// MXFFX
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool TTaifexConnection::CheckFLEXSymbol( const UFC::AnsiString& Symbol )
{
	if( Symbol.Length() > 5 ) ///< FLEX Symbol length must > 5
	{
		if( Symbol[4] == 'X' && (Symbol[3] == 'O'||Symbol[3] == 'F') ///< Is FLEX
			&& FSupportFLEX == false )  ///< Server not Support FLEX
			return false;
	}
	return true;
}
//------------------------------------------------------------------------------
//
//  Function to place a new order.
//
//------------------------------------------------------------------------------
void TTaifexConnection::NewOrder( TNewOrderMessage* Msg )
{
    Msg->SetIsRequestSend(false);
    UFC::AnsiString peerIP = Msg->GetPeerIP();
    if( FUseAPI == atSpeedy  )
    {
        UFC::AnsiString ExchangeSrc( Msg->GetSrc() );
        UFC::AnsiString UserData( Msg->GetAllUserData() );
        UFC::AnsiString Source("");

        GenerateNID( Msg );
        Source.Printf("%c%s", OrderSourceCode( Msg->GetOrderSource()) , Msg->GetMarketDataSource() );
        if( ExchangeSrc.Length() > 14 )///< Send by order channel
        {
            SendToOrderServerEx( Msg->GetMarket(), UserData, ExchangeSrc, Msg->GetTradingSessionID(), Source, Msg->GetGroup(), Msg->GetNID(), 0, peerIP );
            return;
        }
        else ///< Speedy API
        {
            UFC::AnsiString TestSymbol( Msg->GetSymbol() );
            UFC::AnsiString TestAccount( Msg->GetAccount() );
            UFC::AnsiString ErrorStr,TmpStr;
            char R010[ 256 ];
            bool RenderOK = false;
            Int64 Render, Now;
            Int64 Begin = UFC::GetTickCountUS();

            ///< Check Order QTY
            if( FNeedCheckOrdQty0 && ( Msg->GetOrderQty() <= 0 ) )
            {
                GetAPIError( TmpStr, 1002, "NewOrder failed, OrderQty can't be[%d]" );
                ErrorStr.Printf( TmpStr.c_str(), Msg->GetOrderQty() );
                Reject( nsOrderMessageDefine::crrNew, ErrorStr, UserData, Msg, Msg->GetTradingSessionID() );
                return;
            }
            ///< Check Symbol
            TestSymbol.TrimRight();
            if( TestSymbol.Length() < 1 )
            {
                GetAPIError( ErrorStr, 1003, "NewOrder failed, Missing Symbol field." );
                Msg->SetSymbol( "NULL" );
                Reject( nsOrderMessageDefine::crrNew, ErrorStr, UserData, Msg, Msg->GetTradingSessionID() );
                return;
            }
            ///< Check Account
            if( TestAccount.Length() < 5 )
            {
                GetAPIError( ErrorStr, 1004, "NewOrder failed, Missing Account field." );
                Msg->SetAccount( "NULL" );
                Reject( nsOrderMessageDefine::crrNew, ErrorStr, UserData, Msg, Msg->GetTradingSessionID() );
                return;
            }
            ///< LogonProxy should check account exists.
            if( FIsProxy == true && FAdmin == 0 && FAccountSet.Exists( TestAccount ) == false )
            {
                ErrorStr.Printf( "No rights to use account[%s]", TestAccount.c_str());
                Reject( nsOrderMessageDefine::crrNew, ErrorStr, UserData, Msg, Msg->GetTradingSessionID() );
                return;
            }
            ///< Check support sell side
            if( SupportSellSide( Msg ) == false )
            {
                GetAPIError( ErrorStr, 1005, "Not support this market." );
                Reject( nsOrderMessageDefine::crrNew, ErrorStr, UserData, Msg, Msg->GetTradingSessionID() );
                return;
            }
            ///< Render the new order message
            switch( Msg->GetMarket() )
            {
                case nsOrderMessageDefine::mOTC:
                case nsOrderMessageDefine::mTSE:
                case nsOrderMessageDefine::mES:             RenderOK = RenderTSEOTCNewOrder( Msg, R010 ); break;
                case nsOrderMessageDefine::mCNFutures:
                case nsOrderMessageDefine::mCNOptions:      RenderOK = RenderChinaNewOrder( Msg, R010 ); break;
                case nsOrderMessageDefine::mForeignStock:
                case nsOrderMessageDefine::mForeignFutures:
                case nsOrderMessageDefine::mForeignOptions: RenderOK = RenderForeignNewOrder( Msg, R010 ); break;
                case nsOrderMessageDefine::mTWFutures:      ///< Check FLEX Symbol
                case nsOrderMessageDefine::mTWOptions:      if( CheckFLEXSymbol( TestSymbol )== false )
                                                            {
                                                                GetAPIError( ErrorStr, 1006, "Not support TAIFEX FLEX." );
                                                                Reject( nsOrderMessageDefine::crrNew, ErrorStr, UserData, Msg, Msg->GetTradingSessionID() );
                                                                return;
                                                            }
                                                            ///< Render R010 and check Product price digit.
                                                            if( (RenderOK = RenderTAIFEXNewOrder( Msg, R010, Source )) == false )
                                                            {
                                                                UFC::AnsiString Prod( Msg->GetSymbol(), 3 );

                                                                ErrorStr.Printf( "Product[%s] not found!", Prod.c_str() );
                                                                Glog->fprintf( " TTaifexConnection::NewOrder() Error[%s]", ErrorStr.c_str() );
                                                                Reject( nsOrderMessageDefine::crrNew, ErrorStr, UserData, Msg, Msg->GetTradingSessionID() );
                                                                return;
                                                            }
                                                            break;
                case nsOrderMessageDefine::mUnknown:        RenderOK = false;
                                                            break;

            }
            if( !RenderOK )
            {
                ErrorStr.Printf( "Render New Order failed, market[%d]", Msg->GetMarket() );
                Glog->fprintf( " TTaifexConnection::NewOrder() Error[%s]", ErrorStr.c_str() );
                Reject( nsOrderMessageDefine::crrNew, ErrorStr, UserData, Msg, Msg->GetTradingSessionID() );
                return;
            }
            Render = UFC::GetTickCountUS();
            Glog->fprintf( " NewOrder[%lld][%s] UserData[%s]", Msg->GetNID(), R010, UserData.c_str());
            if( TriggerEventAndSend( Msg, R010, UserData, smtNew, nsOrderMessageDefine::crrNew, Msg->GetTradingSessionID(), Source ) == TRUE )
            {
                Msg->SetIsRequestSend(true);
                if( ( FEnablePendingNewAck == TRUE ) && IsTAIWAN( Msg->GetMarket() ) == TRUE )
                    PendingNew( Msg );
            }
            else
                Glog->fprintf( " %s() Send Failed.", __func__ );
            Now = UFC::GetTickCountUS();
            Glog->fprintf( " NewOrder Render[%d]us Send[%d]us Total[%d]us", Render - Begin, Now - Render, Now - Begin );
        }
        Glog->Flush();
        Glog->FlushToFile();
    }
}
//------------------------------------------------------------------------------
//
//  Function to cancel an order.
//
//------------------------------------------------------------------------------
void TTaifexConnection::CancelOrder( TCancelOrderMessage* Msg )
{
    Msg->SetIsRequestSend(false);
    if( FUseAPI == atSpeedy  )
    {
        UFC::AnsiString  ExchangeSrc( Msg->GetSrc() );
        UFC::AnsiString  UserData( Msg->GetAllUserData() );
        UFC::AnsiString  Source("");

        GenerateNID( Msg );
        Source.Printf("%c%s", OrderSourceCode( Msg->GetOrderSource()) , Msg->GetMarketDataSource() );
        if( ExchangeSrc.Length() > 14 )///< Send by order channel
        {
            UFC::AnsiString peerIP = Msg->GetPeerIP();
            SendToOrderServerEx( Msg->GetMarket(), UserData, ExchangeSrc, Msg->GetTradingSessionID(), Source, Msg->GetGroup(), Msg->GetNID(), Msg->GetCNID(), peerIP );
            return;
        }
        else
        {
            char R010[ 256 ];
            bool RenderOK = false;
            UFC::AnsiString TestAccount( Msg->GetAccount() );

            ///< Reject Cancel order without OrderID
            nsOrderMessageDefine::MarketEnum orderMarket = Msg->GetMarket();
            if( CheckOrderID( orderMarket, Msg->GetOrderID()) == false )
            {
                NoOrderIDReject( nsOrderMessageDefine::crrCancel, Msg, UserData, Msg->GetTradingSessionID() );
                return;
            }
            ///< LogonProxy should check account exists.
            if( FIsProxy == true && FAdmin == 0 && FAccountSet.Exists( TestAccount ) == false )
            {
                UFC::AnsiString errorStr;
                errorStr.Printf( "No rights to use account[%s]", TestAccount.c_str());
                Reject( nsOrderMessageDefine::crrCancel, errorStr, UserData, Msg, Msg->GetTradingSessionID() );
                return;
            }
            ///< Render the cancel order message
            switch( orderMarket )
            {
                case nsOrderMessageDefine::mOTC:
                case nsOrderMessageDefine::mTSE:
                case nsOrderMessageDefine::mES:             RenderOK = RenderTSEOTCCancelOrder( Msg, R010 ); break;
                case nsOrderMessageDefine::mTWFutures:
                case nsOrderMessageDefine::mTWOptions:      RenderOK = RenderTAIFEXCancelOrder( Msg, R010, Source ); break;
                case nsOrderMessageDefine::mCNFutures:
                case nsOrderMessageDefine::mCNOptions:      RenderOK = RenderChinaCancelOrder( Msg, R010 ); break;
                case nsOrderMessageDefine::mForeignStock:
                case nsOrderMessageDefine::mForeignFutures:
                case nsOrderMessageDefine::mForeignOptions: RenderOK = RenderForeignCancelOrder( Msg, R010 ); break;
                case nsOrderMessageDefine::mUnknown:        RenderOK = false; break;
            }
            if( !RenderOK )
            {
                UFC::AnsiString errorStr = "";
                errorStr.Printf( "Render Cancel Order failed, market[%d]", Msg->GetMarket() );
                Glog->fprintf( " TTaifexConnection::CancelOrder() Error[%s]", errorStr.c_str() );
                Reject( nsOrderMessageDefine::crrCancel, errorStr, UserData, Msg, Msg->GetTradingSessionID() );
                return;
            }
            Glog->fprintf( " CancelOrder[%lld][%s] UserData[%s]", Msg->GetNID(), R010, UserData.c_str());
            if( TriggerEventAndSend( Msg, R010, UserData, smtCancel, nsOrderMessageDefine::crrCancel, Msg->GetTradingSessionID(), Source, Msg->GetCNID() ) == TRUE )
                Msg->SetIsRequestSend(true);
            else
		Glog->fprintf( " %s() Send Failed.", __func__ );
	}
	Glog->Flush();
	Glog->FlushToFile();
    }
}
//------------------------------------------------------------------------------
//
//  Function to replace an order.
//
//--------------------------------------------------------------------------- ---
void TTaifexConnection::ReplaceOrder( TReplaceOrderMessage* Msg )
{
    Msg->SetIsRequestSend(false);
    if( FUseAPI == atSpeedy  )
    {
        UFC::AnsiString  ExchangeSrc( Msg->GetSrc() );
        UFC::AnsiString  UserData( Msg->GetAllUserData());
        UFC::AnsiString  Source("");

        GenerateNID( Msg );
        Source.Printf("%c%s", OrderSourceCode( Msg->GetOrderSource()) , Msg->GetMarketDataSource() );
        if( ExchangeSrc.Length( ) > 14 )///< Send by order channel
        {
            UFC::AnsiString peerIP = Msg->GetPeerIP();
            SendToOrderServerEx( Msg->GetMarket(), UserData, ExchangeSrc, Msg->GetTradingSessionID(), Source, Msg->GetGroup(), Msg->GetNID(), 0, peerIP );
            return;
        }
        else
        {
            char R010[ 256 ];
            bool RenderOK = false;
            UFC::AnsiString TestAccount( Msg->GetAccount() );

            ///< Reject Cancel order without OrderID
            nsOrderMessageDefine::MarketEnum orderMarket = Msg->GetMarket();
            if( CheckOrderID( orderMarket, Msg->GetOrderID() ) == false )
            {
                NoOrderIDReject( nsOrderMessageDefine::crrCancel, Msg, UserData, Msg->GetTradingSessionID() );
                return;
            }
            ///< LogonProxy should check account exists.
            if( FIsProxy == true && FAdmin == 0 && FAccountSet.Exists( TestAccount ) == false )
            {
                    UFC::AnsiString errorStr;
                    errorStr.Printf( "No rights to use account[%s]", TestAccount.c_str());
                    Reject( nsOrderMessageDefine::crrCancel, errorStr, UserData, Msg, Msg->GetTradingSessionID() );
                    return;
            }
            ///< Render the new order message
            switch( orderMarket )
            {
                case nsOrderMessageDefine::mOTC:
                case nsOrderMessageDefine::mTSE:
                case nsOrderMessageDefine::mES:            RenderOK = RenderTSEOTCReplaceOrder( Msg, R010 ); break;
                case nsOrderMessageDefine::mTWFutures:
                case nsOrderMessageDefine::mTWOptions:     RenderOK = RenderTAIFEXReplaceOrder( Msg, R010, Source ); break;
                case nsOrderMessageDefine::mCNFutures:
                case nsOrderMessageDefine::mCNOptions:     RenderOK = RenderChinaReplaceOrder( Msg, R010 ); break;
                case nsOrderMessageDefine::mForeignStock:
                case nsOrderMessageDefine::mForeignFutures:
                case nsOrderMessageDefine::mForeignOptions:RenderOK = RenderForeignReplaceOrder( Msg, R010 ); break;
                case nsOrderMessageDefine::mUnknown:       RenderOK = false; break;
            }
            if( !RenderOK )
            {
                UFC::AnsiString errorStr = "";
                errorStr.Printf( "Render Replace Order failed, market[%d]", Msg->GetMarket() );
                Glog->fprintf( " TTaifexConnection::ReplaceOrder() Error[%s]", errorStr.c_str() );
                Reject( nsOrderMessageDefine::crrReplace, errorStr, UserData, Msg, Msg->GetTradingSessionID() );
                return;
            }
            Glog->fprintf( " ReplaceOrder[%lld][%s] UserData[%s]", Msg->GetNID(), R010, UserData.c_str());
            if( TriggerEventAndSend( Msg, R010, UserData, smtReplace, nsOrderMessageDefine::crrReplace, Msg->GetTradingSessionID(), Source ) == TRUE )
                Msg->SetIsRequestSend(true);
            else
                Glog->fprintf( " %s() Send Failed.", __func__ );
        }
        Glog->Flush();
        Glog->FlushToFile();
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::OrderStatusRequest( TOrderStatusRequest* Msg )
{
    Msg->SetIsRequestSend(false);
    nsOrderMessageDefine::MarketEnum           Market = Msg->GetMarket();
    UFC::AnsiString                            ExchangeSrc( Msg->GetSrc() );
    UFC::AnsiString                            UserData( Msg->GetAllUserData());
    UFC::AnsiString                            Source("");

    if( FUseAPI == atSpeedy )
    {
        GenerateNID(Msg);
        if( ExchangeSrc.Length() > 14 )///< Send by order channel
        {
            UFC::AnsiString peerIP = Msg->GetPeerIP();
            SendToOrderServerEx(Market, UserData, ExchangeSrc, Msg->GetTradingSessionID(), Source, Msg->GetGroup(), Msg->GetNID(), Msg->GetOrigNID(), peerIP );
            return;
        }
        else
        {
            char R010[ 256 ];
            bool RenderOK = false;
            
            ///< Reject Order status request without OrderID
            if( CheckOrderID( Market, Msg->GetOrderID() ) == false )
            {
                NoOrderIDReject( nsOrderMessageDefine::crrOrderStatus, Msg, UserData, Msg->GetTradingSessionID() );
                return;
            }
            ///< Render the Order status request message
            switch( Market )
            {
                case nsOrderMessageDefine::mES:
                case nsOrderMessageDefine::mOTC:
                case nsOrderMessageDefine::mTSE:           RenderOK = RenderTSEOTCOrderStatus( Msg, R010 ); break;
                case nsOrderMessageDefine::mTWFutures:
                case nsOrderMessageDefine::mTWOptions:     RenderOK = RenderTAIFEXOrderStatus( Msg, R010, Source ); break;
                case nsOrderMessageDefine::mCNFutures:
                case nsOrderMessageDefine::mCNOptions:     
                case nsOrderMessageDefine::mForeignStock:
                case nsOrderMessageDefine::mForeignFutures:
                case nsOrderMessageDefine::mForeignOptions: RenderOK = false; break;
                case nsOrderMessageDefine::mUnknown:        RenderOK = false; break;
            }
            if( RenderOK == false )
            {
                UFC::AnsiString errorStr;
                errorStr.Printf( "Order Status Request failed, market[%d]", Msg->GetMarket() );
                Glog->fprintf( " TTaifexConnection::CancelOrder() Error[%s]", errorStr.c_str() );
                Reject( nsOrderMessageDefine::crrOrderStatus, errorStr, UserData, Msg, Msg->GetTradingSessionID() );
                return;
            }
            Glog->fprintf(" OrderStatusRequest[%lld][%s]", Msg->GetNID(), R010 );
            if( SendToOrderServerEx( Market, UserData, R010, Msg->GetTradingSessionID(), Source, Msg->GetGroup(), Msg->GetNID(), Msg->GetOrigNID()) == TRUE )
                Msg->SetIsRequestSend(true);
        }
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::Quote( TQuoteMessage* Msg )
{
    Msg->SetIsRequestSend(false);
    UFC::AnsiString Account  = Msg->GetAccount();
    UFC::AnsiString Symbol   = Msg->GetSymbol();
    UFC::AnsiString QuoteID  = Msg->GetQuoteID();
    UFC::AnsiString BrokerID = Msg->GetBrokerID();
    UFC::AnsiString UserData = Msg->GetAllUserData();
    UFC::AnsiString R090,Source("");

    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    Symbol.TrimRight();
    int Precision = GetPricePrecision( Market, nsOrderMessageDefine::tsNormal, Symbol );

    if( FUseAPI == atSpeedy )
    {
        GenerateNID( Msg );
        {
            UFC::PLockObject Locker( FRenderCS );
            FR090Render.Reset();
            FR090Render.SetField( "FunctionCode", 1 );
            FR090Render.SetField( "MessageTime", GetCurrentMessageTime() );
            FR090Render.SetField( "BrokerID", Msg->GetBrokerID() );
            if( BrokerID.Length() > 0 )
                FR090Render.SetField( "BrokerID", BrokerID.c_str() );
            else
                FR090Render.SetField( "BrokerID", GetBrokerID( Market ).c_str() );
            FR090Render.SetField( "Account", Account.c_str() );
            FR090Render.SetField( "AccountFlag", Msg->GetAccountFlag() );
            FR090Render.SetField( "OrderID", QuoteID.c_str() );
            FR090Render.SetField( "Symbol", Symbol.c_str() );
            FR090Render.SetField( "BidPrice", UFC::DoubleToInt( Msg->GetBidPrice(), Precision ) );
            FR090Render.SetField( "BidQty", Msg->GetBidQty() );
            FR090Render.SetField( "AskPrice", UFC::DoubleToInt( Msg->GetAskPrice(), Precision ) );
            FR090Render.SetField( "AskQty", Msg->GetAskQty() );
            FR090Render.SetField( "ClearMemberID", FCMID.c_str() );
            if( Msg->GetTimeInForce() != nsOrderMessageDefine::tifROD )
				FR090Render.SetField( "TimeInForce", "Q" );
            else
				FR090Render.SetField( "TimeInForce", "R" );
            Source.Printf("%c%s", OrderSourceCode( Msg->GetOrderSource()) , Msg->GetMarketDataSource() );
            R090  = FR090Render.Render();
        }
        if( TriggerEventAndSend( Msg, R090, UserData, smtQuoteNew, nsOrderMessageDefine::crrQuote, Msg->GetTradingSessionID(), Source ) == TRUE )
            Msg->SetIsRequestSend(true);
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::QuoteCancel( TQuoteCancelMessage* Msg )
{
    Msg->SetIsRequestSend(false);
    UFC::AnsiString Account  = Msg->GetAccount();
    UFC::AnsiString QuoteID  = Msg->GetQuoteID();
    UFC::AnsiString Symbol   = Msg->GetSymbol();
    UFC::AnsiString BrokerID = Msg->GetBrokerID();
    UFC::AnsiString UserData = Msg->GetAllUserData();
    UFC::AnsiString R090,Source("");
    double BidPrice          = Msg->GetBidPrice();
    double AskPrice          = Msg->GetAskPrice();
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    Symbol.TrimRight();
    int Precision = GetPricePrecision( Market, nsOrderMessageDefine::tsNormal, Symbol );

    if( FUseAPI == atSpeedy )
    {
        GenerateNID( Msg );
        {
            UFC::PLockObject Locker( FRenderCS );

            FR090Render.Reset();
            FR090Render.SetField( "FunctionCode", 3 );
            FR090Render.SetField( "MessageTime", GetCurrentMessageTime() );
            if( BrokerID.Length() > 0 )
                FR090Render.SetField( "BrokerID", BrokerID.c_str() );
            else
                FR090Render.SetField( "BrokerID", GetBrokerID( Market ).c_str() );
            FR090Render.SetField( "Account", Account.c_str() );
            FR090Render.SetField( "AccountFlag", Msg->GetAccountFlag() );
            FR090Render.SetField( "OrderID", QuoteID.c_str() );
            FR090Render.SetField( "Symbol", Symbol.c_str() );
            FR090Render.SetField( "BidPrice", UFC::DoubleToInt( BidPrice, Precision) );
            FR090Render.SetField( "AskPrice", UFC::DoubleToInt( AskPrice, Precision));
            FR090Render.SetField( "TimeInForce", "R" );
            FR090Render.SetField( "ClearMemberID", FCMID.c_str() );
            Source.Printf("%c%s", OrderSourceCode( Msg->GetOrderSource()) , Msg->GetMarketDataSource() );
            R090 = FR090Render.Render();
        }
        if ( TriggerEventAndSend( Msg, R090, UserData, smtQuoteCancel, nsOrderMessageDefine::crrQuoteCancel, Msg->GetTradingSessionID(), Source ) == TRUE )
            Msg->SetIsRequestSend(true);
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::QuoteRequest( TQuoteRequestMessage* Msg )
{
    Msg->SetIsRequestSend(false);
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    UFC::AnsiString                  ExchangeSrc( Msg->GetSrc() );
    UFC::AnsiString                  UserData( Msg->GetAllUserData());
    UFC::AnsiString R070,Source("");

    if( FUseAPI == atSpeedy )
    {
        GenerateNID( Msg );
        if( ExchangeSrc.Length() > 14 )///< Send by order channel
        {
            UFC::AnsiString peerIP = Msg->GetPeerIP();
            
            SendToOrderServerEx( Market, UserData, ExchangeSrc, Msg->GetTradingSessionID(), Source, Msg->GetGroup(), Msg->GetNID(), 0, peerIP );
            Msg->SetIsRequestSend(true);
            return;
        }
        else
        {
            UFC::PLockObject Locker( FRenderCS );

            FR070Render.Reset();
            FR070Render.SetField( "BrokerID", GetBrokerID( Market ).c_str() );
            FR070Render.SetField( "Symbol", Msg->GetSymbol() );
            FR070Render.SetField( "OrderID", Msg->GetOrderID() );
            R070 = FR070Render.Render();
        }
        if( TriggerEventAndSend( Msg, R070, UserData, smtQuoteRequest, nsOrderMessageDefine::crrQuoteRequest, Msg->GetTradingSessionID(), Source ) == TRUE )
            Msg->SetIsRequestSend(true);
    }
}
//---------------------------------------------------------------------------
int TTaifexConnection::SendNewsRequest( TNewsMessage* Msg )
{
	TMdMessage ReqMsg;

	if( FUseAPI == atSpeedy ) ///< Sand News message to Speedy Server
	{
		if( Msg->GetID() == 0 ) ///< News ID not exists.
				Msg->SetID( FNetworkID.CreateClientNewsID( FCurrentConnectionID ) );
		ReqMsg.SetIntegerValue( "UID", Msg->GetID() );
		if( (Msg->GetHeadline() != 0 ) && ( strlen( Msg->GetHeadline() ) > 0 ) )
				ReqMsg.SetStringValue( "FUNC", Msg->GetHeadline() );
		if( (Msg->GetText() != 0 ) && ( strlen( Msg->GetText() ) > 0 ))
				ReqMsg.SetStringValue( "DATA", Msg->GetText() );
		if( Msg->GetMsgCount() > 0 )
				ReqMsg.SetIntegerValue( "COUNT", Msg->GetMsgCount() );
		if( Msg->GetMsgNum() > 0 )
				ReqMsg.SetIntegerValue( "INDEX", Msg->GetMsgNum() );
		return FTransport->Send( SUBJECT_NEWS_REQUEST, FID, &ReqMsg );
	}
	return 0;
}
//---------------------------------------------------------------------------
int TTaifexConnection::SendNewsRequest( const UFC::AnsiString& FuncStr, const UFC::AnsiString& DataStr, int Count,int index )
{
	TMdMessage ReqMsg;

	FLastReqUID = FNetworkID.CreateClientNewsID( FCurrentConnectionID );
	ReqMsg.SetIntegerValue( "UID", FLastReqUID );
	ReqMsg.SetStringValue( "FUNC", FuncStr );
	ReqMsg.SetStringValue( "DATA", DataStr );
	ReqMsg.SetIntegerValue( "COUNT", Count );
	ReqMsg.SetIntegerValue( "INDEX", index );
	return FTransport->Send( SUBJECT_NEWS_REQUEST, FID, &ReqMsg );
}
//---------------------------------------------------------------------------
bool TTaifexConnection::MarginPositionRequest( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Account, UFC::AnsiString& Result )
{
	UFC::AnsiString DataStr;

	///< Check BrokerID, Account
	if( BrokerID.Length() != 7 )
	{
		Result = "{\"Error\":\"Invalid BrokerID.\"}";
		return false;
	}
	if( FAdmin == 0 && FAccountSet.Exists( Account ) == FALSE )
	{
		Result.Printf( "{\"Error\":\"No permission to obtain the margin of %s.\"}", Account.c_str() );
		return false;
	}

	///< Send Request to SpeedyGataway
	DataStr.Printf("%s,%s", BrokerID.c_str(), Account.c_str() );
	SendNewsRequest( "ReqFUTMargin", DataStr, 0, 0 );
	FRequestEvent->ResetEvent();
	Result = "{\"Error\":\"Request failed.\"}";

	///< Wait for Response.
	if( FRequestEvent->WaitFor( 5 ) == true )
	{
		Result = FResponseData;
		if( FResponseData.AnsiPos( "Error" ) == -1 )
			return true;
		return false;
	}
	else
	{
		Result = "{\"Error\":\"Request timeout.\"}";
		return false;
	}
}
//------------------------------------------------------------------------------
//
// Direct send Exchange message to SpeedyGateway
// Currently, support TWSE/OTC only
//
//------------------------------------------------------------------------------
BOOL TTaifexConnection::SendDirect( Int64 NID,
                                    nsOrderMessageDefine::MarketEnum Market,
                                    const UFC::AnsiString& OrderString,
									const UFC::AnsiString& Data,
									int Group )
{
    UFC::AnsiString PublishKey;
    MTHandle        MHandle;
    MApp*           MBusClient = FTransport->GetMApp();

    if( Data.Length() > 0 ) ///< Contains User define data.
        PublishKey.Printf( "%s,%s", FID.c_str(), Data.c_str() );
    else
        PublishKey = FID;    
    switch( Market ) ///< Prepare data to write to socket.
    {
        case nsOrderMessageDefine::mTWFutures:      MBusClient->BeginSend( MHandle, SUBJECT_ORDER_FUT, PublishKey );break;
        case nsOrderMessageDefine::mTWOptions:      MBusClient->BeginSend( MHandle, SUBJECT_ORDER_OPT, PublishKey );break;
        case nsOrderMessageDefine::mTSE:            MBusClient->BeginSend( MHandle, SUBJECT_ORDER_TSE, PublishKey );break;
		case nsOrderMessageDefine::mOTC:            MBusClient->BeginSend( MHandle, SUBJECT_ORDER_OTC, PublishKey );break;
		case nsOrderMessageDefine::mES:             MBusClient->BeginSend( MHandle, SUBJECT_ORDER_ES,  PublishKey );break;
		case nsOrderMessageDefine::mForeignStock:   MBusClient->BeginSend( MHandle, SUBJECT_ORDER_FOREIGN_STK, PublishKey );break;
		case nsOrderMessageDefine::mForeignFutures: MBusClient->BeginSend( MHandle, SUBJECT_ORDER_FOREIGN_FUT, PublishKey );break;
		case nsOrderMessageDefine::mForeignOptions: MBusClient->BeginSend( MHandle, SUBJECT_ORDER_FOREIGN_OPT, PublishKey );break;
		default: return false;
	}
	if( FIsProxy == true )
		MBusClient->WriteZipString( MHandle, "ZORDER", OrderString );
	else
		MBusClient->WriteString( MHandle, "ORDER", OrderString );
	MBusClient->WriteString( MHandle, "ID", FID );
	MBusClient->WriteInt32( MHandle, "CID", FCurrentConnectionID );
	if( Group != 0 )            ///< Order Session Group.
		MBusClient->WriteInt32( MHandle, "GROUP", Group );
    if( FUseNID64 == true )  ///< NID(NetworkID)
        MBusClient->WriteInt64( MHandle, "N64", NID );
    else
        MBusClient->WriteInt32( MHandle, "NID", (int)NID );
    return MBusClient->EndSend( MHandle );
}
//------------------------------------------------------------------------------
//
// Send Exchange message to SpeedyGateway
// Currently, support TWSE/OTC/TAIFEX/Foreign Futures
//
//---------------------------------------------------------------------------
BOOL TTaifexConnection::SendToOrderServerEx( nsOrderMessageDefine::MarketEnum Market,
                                             const UFC::AnsiString& Data,
                                             const UFC::AnsiString& OrderString,
                                             nsOrderMessageDefine::TradingSessionIDEnum TradingSession,
                                             const UFC::AnsiString& Source,
                                             int   Group,
                                             Int64 NID,
                                             Int64 CNID,
                                             const UFC::AnsiString& PeerIP,
                                             const UFC::AnsiString& CASignature,
                                             const UFC::AnsiString& CAPlainText,
                                             const UFC::AnsiString& CASessionID,
                                             const UFC::AnsiString& CASerialNumber,
                                             const UFC::AnsiString& StopPriceStrategy)
{
    UFC::AnsiString PublishKey;
    UFC::AnsiString sendPeerIP;
    bool            IsTAIFEX = false;
    MTHandle        MHandle;
    MApp*           MBusClient = FTransport->GetMApp();

    if( Data.Length() > 0 ) ///< Contains User define data.
        PublishKey.Printf( "%s,%s", FID.c_str(), Data.c_str() );
    else
        PublishKey = FID;
    ///< Prepare data to write to socket.
    switch( Market )
    {
        case nsOrderMessageDefine::mTWFutures:      MBusClient->BeginSend( MHandle, SUBJECT_ORDER_FUT, PublishKey );
                                                    IsTAIFEX = true;
                                                    break;
        case nsOrderMessageDefine::mTWOptions:      MBusClient->BeginSend( MHandle, SUBJECT_ORDER_OPT, PublishKey );
                                                    IsTAIFEX = true;
                                                    break;
        case nsOrderMessageDefine::mTSE:            MBusClient->BeginSend( MHandle, SUBJECT_ORDER_TSE, PublishKey );break;
        case nsOrderMessageDefine::mOTC:            MBusClient->BeginSend( MHandle, SUBJECT_ORDER_OTC, PublishKey );break;
        case nsOrderMessageDefine::mES:             MBusClient->BeginSend( MHandle, SUBJECT_ORDER_ES,  PublishKey );break;
        case nsOrderMessageDefine::mForeignStock:   MBusClient->BeginSend( MHandle, SUBJECT_ORDER_FOREIGN_STK, PublishKey );break;
        case nsOrderMessageDefine::mForeignFutures: MBusClient->BeginSend( MHandle, SUBJECT_ORDER_FOREIGN_FUT, PublishKey );break;
        case nsOrderMessageDefine::mForeignOptions: MBusClient->BeginSend( MHandle, SUBJECT_ORDER_FOREIGN_OPT, PublishKey );break;
        default:
            Glog->fprintf(" %s() Unknown Market.", __func__ );
            return FALSE;
    }
    if( FEncode == 1 )
        MBusClient->WriteZipString( MHandle, "ZORDER", OrderString );
    else
        MBusClient->WriteString( MHandle, "ORDER", OrderString );
    MBusClient->WriteString( MHandle, "ID", FID );
    MBusClient->WriteInt32( MHandle, "CID", FCurrentConnectionID );
    ///< for CA
    if( CASignature.Length() > 0 )
        MBusClient->WriteString( MHandle, "CASIGNATURE", CASignature );
    if( CAPlainText.Length() > 0 )
        MBusClient->WriteString( MHandle, "CAPLAINTEXT", CAPlainText );
    if( CASessionID.Length() > 0 )
        MBusClient->WriteString( MHandle, "CASESSIONID", CASessionID );
    if( CASerialNumber.Length() > 0 )
        MBusClient->WriteString( MHandle, "CASERIALNUMBER", CASerialNumber );
    ///< NID, CNID, Group, PeerIP
    //<  OFFHOUR, SOURCE  TAIFEX only
    ///< TOKEN for SpeedyProxy
    if( FUseNID64 == true )  ///< NID(NetworkID)
        MBusClient->WriteInt64( MHandle, "N64", NID );
    else
        MBusClient->WriteInt32( MHandle, "NID", (int)NID );
    if( CNID != 0 )  ///< Cancel or Order status request by NID.
    {
        if( FUseNID64 == true )
            MBusClient->WriteInt64( MHandle, "CN64", CNID );
        else
            MBusClient->WriteInt32( MHandle, "CNID", (int)CNID );
    }
    if( IsTAIFEX == true ) ///< TAIFEX trading session
    {
        if( TradingSession == nsOrderMessageDefine::tsNormal )
            MBusClient->WriteInt32( MHandle, "OFFHOUR", 0 );
        else if( TradingSession == nsOrderMessageDefine::tsOffHour )
            MBusClient->WriteInt32( MHandle, "OFFHOUR", 1 );
    }
    if( Group != 0 )            ///< Order Session Group.
        MBusClient->WriteInt32( MHandle, "GROUP", Group );
    if( FToken.Length(  ) > 1 ) ///< SpeedyProxy Token
        MBusClient->WriteString( MHandle, "TOKEN", FToken );
    if( Source.Length() > 0 )   ///< With TAIFEX New Order/MarketData source information.
        MBusClient->WriteString( MHandle, "SOURCE", Source );
    if( PeerIP.Length() > 0 )
        sendPeerIP = PeerIP;
    else
        sendPeerIP = MBusClient->GetLocalIPAddress();
    if( sendPeerIP.Length() > 0 )
        MBusClient->WriteString( MHandle, "PeerIP", sendPeerIP );
    if ( StopPriceStrategy.Length() > 0)
        MBusClient->WriteString( MHandle, "STRATEGY", StopPriceStrategy );
    return MBusClient->EndSend( MHandle );
}
//---------------------------------------------------------------------------
BOOL  TTaifexConnection::CancelTouchOrder(const char* touch_order_id)
{
    if (!touch_order_id || (touch_order_id[0] != 'T' && touch_order_id[0] != 'O'))
        return FALSE;

    UFC::AnsiString PublishKey;
    MTHandle        MHandle;
    MApp* MBusClient = FTransport->GetMApp();
    
    PublishKey = FID;
    if ('T' == touch_order_id[0])
        MBusClient->BeginSend(MHandle, SUBJECT_TSE_STRATEGY, PublishKey);
    else
        MBusClient->BeginSend(MHandle, SUBJECT_OTC_STRATEGY, PublishKey);

    UFC::AnsiString order;
    order.Printf("cmd=cxl&oid=%s", touch_order_id);

    MBusClient->WriteString(MHandle, "ORDER", order);
    return MBusClient->EndSend(MHandle);
}
