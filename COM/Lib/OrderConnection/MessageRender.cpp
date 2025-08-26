//---------------------------------------------------------------------------
#include "TTaifexConnection.h"
#include "MessageRender.h"
//---------------------------------------------------------------------------
TAIFEXR01::TAIFEXR01( void )
:End( 0 )
{
}
//---------------------------------------------------------------------------
void TAIFEXR01::SetFunction( int Func )
{
	switch( Func )
	{
		case 1:  memcpy( Header, "30010012115900" , 14 );break;
		case 2:  memcpy( Header, "30020012125900" , 14 );break;
		case 3:  memcpy( Header, "30030012135900" , 14 );break;
		case 4:  memcpy( Header, "30040012145900" , 14 );break;
		case 5:  memcpy( Header, "30050012155900" , 14 );break;
		case 6:  memcpy( Header, "30060012165900" , 14 );break;
		default: memcpy( Header, "30000012165900" , 14 );break;
	}
	End = 0;
}
//---------------------------------------------------------------------------
char* TAIFEXR01::ToString()
{
	return Header;
}
//---------------------------------------------------------------------------
//
//   For TAIFEX
//
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderTAIFEXR010( int Func, TBaseMessage* Msg, char* R010, nsOrderMessageDefine::TradingSessionIDEnum  TradingSession )
{
    TAIFEXR01*      R01 = reinterpret_cast<TAIFEXR01*>( R010 );
    UFC::AnsiString Symbol( Msg->GetSymbol() );
    UFC::AnsiString BrokerID( Msg->GetBrokerID() );
    UFC::AnsiString Account( Msg->GetAccount() );
    int   	    Precision  = GetTAIFEXPricePrecision( Msg->GetMarket(), Symbol );
    int  	    Price;
    char            Buf[16];

    if( Precision >= 0 )
    {
	R01->SetFunction( Func );
	if( Msg->GetOrderType() == nsOrderMessageDefine::otMarket ||
	    Msg->GetOrderType() == nsOrderMessageDefine::otMarketWithProtection ) ///< Market Order
	    Price = 0;
	else
	    Price = UFC::DoubleToInt( Msg->GetPrice(), Precision );
	if( BrokerID.Length() < 7 )
	    BrokerID = GetBrokerID( Msg->GetMarket() );
	Account.PadThis( 7, '0' );
	Symbol.PadThis( 20, ' ' );
	memcpy( R01->FcmNo, BrokerID.c_str(),       7 );  //< FCM ID
	memcpy( R01->OrderNo, Msg->GetOrderID(),    5 );  //< Order ID
	memcpy( R01->InvestorAcno, Account.c_str(), 7 );  //< Investor account
	R01->InvestorFlag = Msg->GetAccountFlag()[0];      //< Investor flag
	memcpy( R01->CommodityId, Symbol.c_str(),   20 );  //< Commodity ID ( Oprions 20 Futures 7) */
	if( Msg->GetSide() == nsOrderMessageDefine::sBuy)
            R01->BuySellCode = 'B';
	else
            R01->BuySellCode = 'S';
	switch( Msg->GetOrderType() )					  //< Order type
	{
		case nsOrderMessageDefine::otMarket:               R01->OrderType = 'M';break;
		case nsOrderMessageDefine::otMarketWithProtection: R01->OrderType = 'P';break;
		default:                                           R01->OrderType = 'L';break;
	}
        sprintf( Buf, "%09d",  Price );                  //< Order price
        memcpy( R01->OrderPrice, Buf, 9 );
        sprintf( Buf, "%04d", (int) Msg->GetOrderQty() );//< Order quantity
        memcpy( R01->OrderQty, Buf, 4 ); 
	switch( Msg->GetTimeInForce() )					      //< Order condition
	{
		case nsOrderMessageDefine::tifROD:  R01->OrderCondition = 'R'; break;
		case nsOrderMessageDefine::tifIOC:  R01->OrderCondition = 'I'; break;
		case nsOrderMessageDefine::tifFOK:  R01->OrderCondition = 'F'; break;
		default:                            R01->OrderCondition = 'R'; break;
	}
	switch( Msg->GetPositionEffect() ) 				 //< Open offset flag
	{
		case nsOrderMessageDefine::peOpen:          R01->OpenOffsetFlag = '0';break;
		case nsOrderMessageDefine::peClose:         R01->OpenOffsetFlag = '1';break;
		case nsOrderMessageDefine::peDayTrade:      R01->OpenOffsetFlag = '2';break;
		case nsOrderMessageDefine::peAuto:          R01->OpenOffsetFlag = ' ';break;
		case nsOrderMessageDefine::peDayTradeOpen:  R01->OpenOffsetFlag = '3';break;
		case nsOrderMessageDefine::peAutoToday:     R01->OpenOffsetFlag = '5';break;
		case nsOrderMessageDefine::peForceClose:    R01->OpenOffsetFlag = '7';break;
		case nsOrderMessageDefine::peTMPMarketMaker:R01->OpenOffsetFlag = '9';break;
		case nsOrderMessageDefine::peTAIFEXAuto:    R01->OpenOffsetFlag = 'A';break;
		default:                                    R01->OpenOffsetFlag = '0';break;
	}
	memcpy( R01->ClearMemberId, FCMID.c_str(), 4 );   //< Clear member ID
	return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderTAIFEXNewOrder(  TNewOrderMessage* Msg, char* R010, UFC::AnsiString& Source )
{    
    Source.Printf( "%c%s", OrderSourceCode( Msg->GetOrderSource()) , Msg->GetMarketDataSource() );
    return RenderTAIFEXR010( 1, Msg, R010, Msg->GetTradingSessionID() );
}
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderTAIFEXCancelOrder( TCancelOrderMessage* Msg, char* R010, UFC::AnsiString& Source )
{
    Source.Printf( "%c%s", OrderSourceCode( Msg->GetOrderSource()) , Msg->GetMarketDataSource() );
    return RenderTAIFEXR010( 3, Msg, R010, Msg->GetTradingSessionID() );
}
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderTAIFEXReplaceOrder( TReplaceOrderMessage* Msg, char* R010, UFC::AnsiString& Source )
{
    Source.Printf( "%c%s", OrderSourceCode( Msg->GetOrderSource()) , Msg->GetMarketDataSource() );
    if( Msg->GetOrderQty() == 0 )
        return RenderTAIFEXR010( 6, Msg, R010, Msg->GetTradingSessionID() ); ///< Replace Px
    else
        return RenderTAIFEXR010( 2, Msg, R010, Msg->GetTradingSessionID() ); ///< Replace Qty
}
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderTAIFEXOrderStatus( TOrderStatusRequest* Msg, char* R010, UFC::AnsiString& Source )
{
    Source.Printf( "%c%s", 'G', "999" );
    return RenderTAIFEXR010( 5, Msg, R010, Msg->GetTradingSessionID() );
}
//---------------------------------------------------------------------------
//
//   For PATS.( Oversea Futures)
//
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderForeignNewOrder( TNewOrderMessage* Msg, char* R010 )
{
    UFC::NameValueMessage PATSMessage("^\n");

    PATSMessage.Append( "cmd", "0" );
    PATSMessage.Append( "exh", Msg->GetExchangeCode() );
    PATSMessage.Append( "spx", (double)Msg->GetStopPx() );
    if( ( Msg->GetIBNO() != NULL ) && ( strlen( Msg->GetIBNO() ) > 0 ) )
        PATSMessage.Append( "ibno", Msg->GetIBNO() );
    if( ( Msg->GetEUCD() != NULL ) && ( strlen( Msg->GetEUCD() ) > 0 ) )
        PATSMessage.Append( "eucd", Msg->GetEUCD() );
    if( Msg->GetDayTrade( ) == 0 )
        PATSMessage.Append( "dtover", "N" );
    else
        PATSMessage.Append( "dtover", "Y" );
    
    switch( Msg->GetTradingSessionID() )
    {
        case nsOrderMessageDefine::tsNormal:      PATSMessage.Append( "tss", 0L ); break;  // 0
        case nsOrderMessageDefine::tsOddLot:      PATSMessage.Append( "tss", 1L ); break;  // 1
        case nsOrderMessageDefine::tsOffHour:     PATSMessage.Append( "tss", 2L ); break;  // 2
        case nsOrderMessageDefine::tsAuction:     PATSMessage.Append( "tss", 3L ); break;  // 3
        case nsOrderMessageDefine::tsLend:        PATSMessage.Append( "tss", 4L ); break;  // 4
        case nsOrderMessageDefine::tsTender:      PATSMessage.Append( "tss", 5L ); break;  // 5
        case nsOrderMessageDefine::tsTenderEx:    PATSMessage.Append( "tss", 6L ); break;  // 6
        case nsOrderMessageDefine::tsNegotiatePx: PATSMessage.Append( "tss", 7L ); break;  // 7
        case nsOrderMessageDefine::tsAuto:                                                 // 8
        default:                                  PATSMessage.Append( "tss", 8L );
    }  //switch( Msg->GetTradingSessionID() )

    UFC::AnsiString expireDate(Msg->GetExpireDate());
    switch( Msg->GetTimeInForce() )
    {
        case nsOrderMessageDefine::tifIOC:  //2
            PATSMessage.Append( "tif", "I" ); 
            break;
        case nsOrderMessageDefine::tifFOK:  //3
            PATSMessage.Append( "tif", "F" ); 
            break;
        case nsOrderMessageDefine::tifGTC:  //4
            PATSMessage.Append( "tif", "4" ); 
            break;
        case nsOrderMessageDefine::tifGTD:  //5
            PATSMessage.Append( "tif", "5" );
            if (expireDate.Length() > 0) PATSMessage.Append( "exdt", expireDate );
            break;
        case nsOrderMessageDefine::tifOPG:  //6
            PATSMessage.Append( "tif", "6" );
            break;
        case nsOrderMessageDefine::tifCLS:  //7 
            PATSMessage.Append( "tif", "7" );
            break;
        case nsOrderMessageDefine::tifGTX:  //8
            PATSMessage.Append( "tif", "8" );
            break;
        case nsOrderMessageDefine::tifROD:  //1
        default:
            PATSMessage.Append( "tif", "R" );
    }  //switch( Msg->GetTimeInForce() )
    
    UFC::Int32 attr_count = Msg->GetAttributeCount();
    for (UFC::Int32 i = 0; i < attr_count; ++i)
    {
        UFC::AnsiString  attr_name;
        UFC::AnsiString  attr_value;
        if (Msg->GetAttribute(i, attr_name, attr_value))
            PATSMessage.Append(attr_name, attr_value);
    }

    FillPATSOrderBase(Msg, &PATSMessage);
    strcpy( R010, PATSMessage.ToString().c_str() );
    return true;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderForeignCancelOrder( TCancelOrderMessage* Msg, char* R010 )
{
    UFC::NameValueMessage PATSMessage("^\n");

    PATSMessage.Append("cmd", "1");
    PATSMessage.Append("exh", Msg->GetExchangeCode());
    if( Msg->GetIBNO() != NULL && strlen( Msg->GetIBNO() ) > 0 )
        PATSMessage.Append("ibno", Msg->GetIBNO());
    if( Msg->GetEUCD() != NULL && strlen( Msg->GetEUCD() ) > 0 )
         PATSMessage.Append("eucd", Msg->GetEUCD() );
    switch( Msg->GetTradingSessionID() )
    {
        case nsOrderMessageDefine::tsNormal:      PATSMessage.Append( "tss", 0L ); break;  // 0
        case nsOrderMessageDefine::tsOddLot:      PATSMessage.Append( "tss", 1L ); break;  // 1
        case nsOrderMessageDefine::tsOffHour:     PATSMessage.Append( "tss", 2L ); break;  // 2
        case nsOrderMessageDefine::tsAuction:     PATSMessage.Append( "tss", 3L ); break;  // 3
        case nsOrderMessageDefine::tsLend:        PATSMessage.Append( "tss", 4L ); break;  // 4
        case nsOrderMessageDefine::tsTender:      PATSMessage.Append( "tss", 5L ); break;  // 5
        case nsOrderMessageDefine::tsTenderEx:    PATSMessage.Append( "tss", 6L ); break;  // 6
        case nsOrderMessageDefine::tsNegotiatePx: PATSMessage.Append( "tss", 7L ); break;  // 7
        case nsOrderMessageDefine::tsAuto:                                                 // 8
        default:                                  PATSMessage.Append( "tss", 8L );
    }  //switch( Msg->GetTradingSessionID() )

    nsOrderMessageDefine::TimeInForceEnum timeInForce = Msg->GetTimeInForce();
    if( timeInForce == nsOrderMessageDefine::tifROD )       //1
        PATSMessage.Append( "tif", "R" );
    else if( timeInForce == nsOrderMessageDefine::tifIOC )  //2
        PATSMessage.Append( "tif", "I" );
    else if( timeInForce == nsOrderMessageDefine::tifFOK )  //3
        PATSMessage.Append( "tif", "F" );
    else if( timeInForce == nsOrderMessageDefine::tifGTC )  //4
        PATSMessage.Append( "tif", "4" );
    else if( timeInForce == nsOrderMessageDefine::tifGTD )  //5
        PATSMessage.Append( "tif", "5" );
    else if( timeInForce == nsOrderMessageDefine::tifOPG )  //6
        PATSMessage.Append( "tif", "6" );
    else if( timeInForce == nsOrderMessageDefine::tifCLS )  //7
        PATSMessage.Append( "tif", "7" );
    else if( timeInForce == nsOrderMessageDefine::tifGTX )  //8
        PATSMessage.Append( "tif", "8" );
        
    FillPATSOrderBase( Msg, &PATSMessage );
    strcpy( R010, PATSMessage.ToString().c_str() );
    return true;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderForeignReplaceOrder( TReplaceOrderMessage* Msg, char* R010 )
{
    UFC::NameValueMessage PATSMessage( "^\n" );

    PATSMessage.Append( "cmd", "2" );
    PATSMessage.Append( "exh", Msg->GetExchangeCode( ));
    PATSMessage.Append( "spx", Msg->GetStopPx( ));
    if( ( Msg->GetEUCD() != NULL ) && ( strlen( Msg->GetEUCD() ) > 0 ) )
        PATSMessage.Append( "eucd", Msg->GetEUCD() );
    switch( Msg->GetTradingSessionID() )
    {
        case nsOrderMessageDefine::tsNormal:      PATSMessage.Append( "tss", 0L ); break;  // 0
        case nsOrderMessageDefine::tsOddLot:      PATSMessage.Append( "tss", 1L ); break;  // 1
        case nsOrderMessageDefine::tsOffHour:     PATSMessage.Append( "tss", 2L ); break;  // 2
        case nsOrderMessageDefine::tsAuction:     PATSMessage.Append( "tss", 3L ); break;  // 3
        case nsOrderMessageDefine::tsLend:        PATSMessage.Append( "tss", 4L ); break;  // 4
        case nsOrderMessageDefine::tsTender:      PATSMessage.Append( "tss", 5L ); break;  // 5
        case nsOrderMessageDefine::tsTenderEx:    PATSMessage.Append( "tss", 6L ); break;  // 6
        case nsOrderMessageDefine::tsNegotiatePx: PATSMessage.Append( "tss", 7L ); break;  // 7
        case nsOrderMessageDefine::tsAuto:                                                 // 8
        default:                                  PATSMessage.Append( "tss", 8L );
    }  //switch( Msg->GetTradingSessionID() )
    
    nsOrderMessageDefine::TimeInForceEnum timeInForce = Msg->GetTimeInForce();
    if (timeInForce == nsOrderMessageDefine::tifROD)       //1
        PATSMessage.Append( "tif", "R" );
    else if (timeInForce == nsOrderMessageDefine::tifIOC)  //2
        PATSMessage.Append( "tif", "I" );
    else if (timeInForce == nsOrderMessageDefine::tifFOK)  //3
        PATSMessage.Append( "tif", "F" );
    else if (timeInForce == nsOrderMessageDefine::tifGTC)  //4
        PATSMessage.Append( "tif", "4" );
    else if (timeInForce == nsOrderMessageDefine::tifGTD)  //5
    {
        PATSMessage.Append( "tif", "5" );
        UFC::AnsiString expireDate(Msg->GetExpireDate());
        if (expireDate.Length() > 0) PATSMessage.Append( "exdt", expireDate );
    }
    else if (timeInForce == nsOrderMessageDefine::tifOPG)  //6
        PATSMessage.Append( "tif", "6" );
    else if (timeInForce == nsOrderMessageDefine::tifCLS)  //7
        PATSMessage.Append( "tif", "7" );
    else if (timeInForce == nsOrderMessageDefine::tifGTX)  //8
        PATSMessage.Append( "tif", "8" );
        
    FillPATSOrderBase( Msg, &PATSMessage );
    strcpy( R010, PATSMessage.ToString().c_str() );
    return true;
}
//---------------------------------------------------------------------------
//
//   For China
//
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderChinaNewOrder( TNewOrderMessage* Msg, char* R010 )
{
	UFC::NameValueMessage GLHQMessage("^\n");

	GLHQMessage.Append( "cmd", "0");
	GLHQMessage.Append( "exh", Msg->GetExchangeCode());
	FillChinaOrderBase( Msg, &GLHQMessage);
	strcpy( R010, GLHQMessage.ToString().c_str() );
	return true;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderChinaCancelOrder( TCancelOrderMessage* Msg, char* R010 )
{
	UFC::NameValueMessage GLHQMessage("^\n");

	GLHQMessage.Append("cmd", "1");
	GLHQMessage.Append("exh", Msg->GetExchangeCode());
	FillChinaOrderBase( Msg, &GLHQMessage);
	strcpy( R010, GLHQMessage.ToString().c_str() );
	return true;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderChinaReplaceOrder( TReplaceOrderMessage* Msg, char* R010 )
{
	UFC::NameValueMessage GLHQMessage("^\n");

	GLHQMessage.Append("cmd", "2");
	GLHQMessage.Append("exh", Msg->GetExchangeCode());
	FillChinaOrderBase( Msg, &GLHQMessage);
	strcpy( R010, GLHQMessage.ToString().c_str() );
	return true;
}
//---------------------------------------------------------------------------
//
//   For TWSE/OTC
//
//---------------------------------------------------------------------------
TWSET01::TWSET01( void )
:End( 0 )
{
}
//---------------------------------------------------------------------------
void TWSET01::SetFunction( BOOL IsTSE, int Func )
{
    char  Data[ 16 ];
    Int32 Now = UFC::GetHHMMSS();
    Int32 System = (IsTSE == TRUE) ? 30 : 93;

/// Allen modify at 20190615
#ifdef _MSC_VER
    _snprintf( Data, 16, "%02d%02d00%06d00", System, Func, Now );
#else
    snprintf( Data, 16, "%02d%02d00%06d00", System, Func, Now );
#endif
    memcpy( Header, Data , 14 );
    End = 0;
}
//---------------------------------------------------------------------------
char* TWSET01::ToString()
{
	return Header;
}
//---------------------------------------------------------------------------
TWSET01Ex::TWSET01Ex( void )
:End( 0 )
{
}
//---------------------------------------------------------------------------
void TWSET01Ex::SetFunction( BOOL IsTSE, int Func )
{
	char  Data[ 16 ];
	Int32 Now = UFC::GetHHMMSS();
	Int32 System = (IsTSE == TRUE) ? 30 : 93;

/// Allen modify at 20190615
#ifdef _MSC_VER
	_snprintf( Data, 16, "%02d%02d00%06d00", System, Func, Now );
#else
	snprintf( Data, 16, "%02d%02d00%06d00", System, Func, Now );
#endif
	memcpy( Header, Data , 14 );
	End = 0;
}
//---------------------------------------------------------------------------
void TWSET01Ex::SetIntradayOddFunc( BOOL IsTSE, int Func )
{
	char  Data[ 16 ];
	Int32 Now = UFC::GetHHMMSS();
	Int32 System = (IsTSE == TRUE) ? 33 : 83;

#ifdef _MSC_VER
	_snprintf( Data, 16, "%02d%02d00%06d00", System, Func, Now );
#else
	snprintf( Data, 16, "%02d%02d00%06d00", System, Func, Now );
#endif
	memcpy( Header, Data , 14 );
	End = 0;
}
//---------------------------------------------------------------------------
char* TWSET01Ex::ToString()
{
    return Header;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderTWSET010( int Func, TBaseMessage* Msg, char* R010, char EC, char OT )
{
    TWSET01*        T01 = reinterpret_cast<TWSET01*>( R010 );
    UFC::AnsiString BrokerID( Msg->GetBrokerID() );
    UFC::AnsiString Symbol( Msg->GetSymbol() );
    int  	    Price = UFC::DoubleToInt( Msg->GetPrice(), 2 );
    char            Buf[16];

    if( Msg->GetMarket() == nsOrderMessageDefine::mTSE || Msg->GetMarket() == nsOrderMessageDefine::mES )
        T01->SetFunction( TRUE,  Func );
    else
        T01->SetFunction( FALSE, Func );
    Symbol.PadThis( 6, ' ' );
    if( BrokerID.Length() < 4 )
        BrokerID = GetBrokerID( Msg->GetMarket() );
    BrokerID.PadThis( 4, ' ' );
    memcpy( T01->BrokerID,     BrokerID.c_str(),  4 );    ///< Broker ID
    strcpy( T01->PVCID, "  " );                           ///< PVCID
    memcpy( T01->OrderNo,      Msg->GetOrderID(), 5 );    ///< Order ID
    memcpy( T01->InvestorAcno, Msg->GetAccount(), 7 );    ///< Investor account
    T01->InvestorFlag = Msg->GetAccountFlag()[0];         ///< Investor flag
    memcpy( T01->StockID, Symbol.c_str(),   6 );          ///< Stock ID
    
    sprintf( Buf, "%06d",Price );             ///< Order price
    memcpy( T01->OrderPrice, Buf, 9 );
    sprintf( Buf,  "%03d",(int)Msg->GetOrderQty() );///< Order quantity
    memcpy( T01->OrderQty, Buf, 3 );
    if( Msg->GetSide() == nsOrderMessageDefine::sBuy)     ///< Buy/Sell code.
        T01->BuySellCode = 'B';
    else
        T01->BuySellCode = 'S';
    T01->ExchangeCode = EC;                               ///< Exchange Code.
    T01->OrderType    = OT;                               ///< Order Type.
    return true;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderTWSET010Ex( int Func, TBaseMessage* Msg, char* R010, char EC, char OT )
{
    TWSET01Ex*      T01 = reinterpret_cast<TWSET01Ex*>( R010 );
    UFC::AnsiString BrokerID( Msg->GetBrokerID() );
    UFC::AnsiString Symbol( Msg->GetSymbol() );
    int 	    Price = UFC::DoubleToInt( Msg->GetPrice(), 4 );
    char            Buf[16];        

    if( Msg->GetMarket() == nsOrderMessageDefine::mTSE || Msg->GetMarket() == nsOrderMessageDefine::mES )
	T01->SetFunction( TRUE,  Func );
    else
	T01->SetFunction( FALSE, Func );
    Symbol.PadThis( 6, ' ' );
    if( BrokerID.Length() < 4 )
	BrokerID = GetBrokerID( Msg->GetMarket() );
    BrokerID.PadThis( 4, ' ' );
    memcpy( T01->BrokerID,     BrokerID.c_str(),  4 );    ///< Broker ID
    strcpy( T01->PVCID, "  " );                           ///< PVCID
    memcpy( T01->OrderNo,      Msg->GetOrderID(), 5 );    ///< Order ID
    memcpy( T01->InvestorAcno, Msg->GetAccount(), 7 );    ///< Investor account
    T01->InvestorFlag = Msg->GetAccountFlag()[0];         ///< Investor flag
    memcpy( T01->StockID, Symbol.c_str(),   6 );          ///< Stock ID
    sprintf( Buf, "%09d",Price );             ///< Order price
    memcpy( T01->OrderPrice, Buf, 9);
    sprintf( Buf, "%06d",(int)Msg->GetOrderQty() );///< Order quantity
    memcpy( T01->OrderQty, Buf, 6);
    if( Msg->GetSide() == nsOrderMessageDefine::sBuy)     ///< Buy/Sell code.
	T01->BuySellCode = 'B';
    else
	T01->BuySellCode = 'S';
    T01->ExchangeCode = EC;                               ///< Exchange Code.
    T01->OrderType    = OT;                               ///< TWSE Order Type.
    switch( Msg->GetOrderType() )///< *New* '1': Market '2':Limit
    {
	case nsOrderMessageDefine::otMarket: T01->PriceType = '1';break;
	case nsOrderMessageDefine::otLimit:  T01->PriceType = '2';break;
	default:                             T01->PriceType = '2';break;
    }
    switch( Msg->GetTimeInForce())///< *New* '0': ROD '3':IOC '4':FOK
    {
	case nsOrderMessageDefine::tifROD:  T01->TimeInForce = '0'; break;
	case nsOrderMessageDefine::tifIOC:  T01->TimeInForce = '3'; break;
	case nsOrderMessageDefine::tifFOK:  T01->TimeInForce = '4'; break;
	default:                            T01->TimeInForce = '0'; break;
    }
    return true;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderTWSEO110Ex( int Func, TBaseMessage* Msg, char* R010 )
{
    TWSET01Ex*      T01 = reinterpret_cast<TWSET01Ex*>( R010 );
    UFC::AnsiString BrokerID( Msg->GetBrokerID() );
    UFC::AnsiString Symbol( Msg->GetSymbol() );
    int             Price = UFC::DoubleToInt( Msg->GetPrice(), 4 );
    char            Buf[16];   

    if( Msg->GetMarket() == nsOrderMessageDefine::mTSE || Msg->GetMarket() == nsOrderMessageDefine::mES )
	T01->SetIntradayOddFunc( TRUE,  Func );
    else
    	T01->SetIntradayOddFunc( FALSE, Func );
    Symbol.PadThis( 6, ' ' );
    if( BrokerID.Length() < 4 )
	BrokerID = GetBrokerID( Msg->GetMarket() );
    BrokerID.PadThis( 4, ' ' );
    memcpy( T01->BrokerID,     BrokerID.c_str(),  4 );    ///< Broker ID
    strcpy( T01->PVCID, "  " );                           ///< PVCID
    memcpy( T01->OrderNo,      Msg->GetOrderID(), 5 );    ///< Order ID
    memcpy( T01->InvestorAcno, Msg->GetAccount(), 7 );    ///< Investor account
    T01->InvestorFlag = Msg->GetAccountFlag()[0];         ///< Investor flag
    memcpy( T01->StockID, Symbol.c_str(),   6 );          ///< Stock ID
    sprintf( Buf, "%09d",Price );             ///< Order price
    memcpy( T01->OrderPrice, Buf, 9);
    sprintf( Buf, "%06d",(int)Msg->GetOrderQty() );///< Order quantity
    memcpy( T01->OrderQty, Buf, 6);
    if( Msg->GetSide() == nsOrderMessageDefine::sBuy)     ///< Buy/Sell code.
	T01->BuySellCode = 'B';
    else
	T01->BuySellCode = 'S';
    T01->ExchangeCode = '2';                              ///< Exchange Code. 2 Odd lot
    T01->OrderType    = '0';                              ///< TWSE Order Type. 0 Normal
    T01->PriceType    = '2';                              ///< '2':Limit only
    T01->TimeInForce  = '0';                              ///< '0': ROD only
    return true;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderTSEOTCNewOrder( TNewOrderMessage* Msg, char* R010 )
{
	int iSide;

	if( Msg->GetSide() == nsOrderMessageDefine::sBuy)
		iSide = 1; ///< Buy
	else
		iSide = 2;///< Sell
	if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsNormal )
		return (this->*FTWSERender)( iSide, Msg, R010, Msg->GetTSEExchangeCode()[0], Msg->GetTSEOrderType()[0] );
	else if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsIntradayOdd )
			return RenderTWSEO110Ex( iSide, Msg, R010 );

	UFC::TRenderData*  R010Render = GetR010Render( Msg->GetMarket(), Msg->GetTradingSessionID() );

	if( R010Render != NULL )
	{
		UFC::PLockObject Locker( FRenderCS );

		FillOrderBase( Msg->GetTradingSessionID(), Msg->GetMarket(), Msg, R010Render );
		FillNewOrderFunctionCode( Msg->GetTradingSessionID(), Msg->GetMarket(), Msg, R010Render );
		strcpy( R010, R010Render->Render().c_str() );
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderTSEOTCCancelOrder( TCancelOrderMessage* Msg, char* R010)
{
	if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsNormal )
		return (this->*FTWSERender)( 4, Msg, R010,  Msg->GetTSEExchangeCode()[0], Msg->GetTSEOrderType()[0] ); ///< Cancel
	else if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsIntradayOdd )
		return RenderTWSEO110Ex( 4, Msg, R010 );

	UFC::TRenderData*  R010Render = GetR010Render( Msg->GetMarket(), Msg->GetTradingSessionID() );

	if( R010Render != NULL )
	{
		UFC::PLockObject Locker( FRenderCS );

		FillOrderBase( Msg->GetTradingSessionID(), Msg->GetMarket(), Msg, R010Render);
		FillCancelOrderFunctionCode( Msg->GetTradingSessionID(), Msg->GetMarket(), Msg, R010Render);
		strcpy( R010, R010Render->Render().c_str() );
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderTSEOTCReplaceOrder( TReplaceOrderMessage* Msg, char* R010 )
{
	if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsNormal )
	{
		if( Msg->GetOrderQty() == 0 )
			return (this->*FTWSERender)(6, Msg, R010, Msg->GetTSEExchangeCode()[0], Msg->GetTSEOrderType()[0] ); ///< Replace Px
		else
			return (this->*FTWSERender)(3, Msg, R010, Msg->GetTSEExchangeCode()[0], Msg->GetTSEOrderType()[0] ); ///< Replace Qty
	}
	else if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsIntradayOdd )
		return RenderTWSEO110Ex( 3, Msg, R010 );

	UFC::TRenderData*  R010Render = GetR010Render( Msg->GetMarket(), Msg->GetTradingSessionID() );

    if( R010Render != NULL )
    {
        UFC::PLockObject Locker( FRenderCS );

        FillOrderBase( Msg->GetTradingSessionID(), Msg->GetMarket(), Msg, R010Render);
        FillReplaceOrderFunctionCode( Msg->GetTradingSessionID(), Msg->GetMarket(), Msg, R010Render );
        strcpy( R010, R010Render->Render().c_str() );
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::RenderTSEOTCOrderStatus( TOrderStatusRequest* Msg, char* R010 )
{
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsNormal )
		return (this->*FTWSERender)( 5, Msg, R010,  Msg->GetTSEExchangeCode()[0], Msg->GetTSEOrderType()[0] ); ///< OrderStatus request
	else if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsIntradayOdd )
		return RenderTWSEO110Ex( 5, Msg, R010 );

    UFC::TRenderData*  R010Render = GetR010Render( Msg->GetMarket(), Msg->GetTradingSessionID() );

    if( R010Render != NULL )
    {
        UFC::PLockObject Locker( FRenderCS );

        FillOrderBase( Msg->GetTradingSessionID(), Msg->GetMarket(), Msg, R010Render);
        FillOrderStatusRequestFunctionCode( Msg->GetTradingSessionID(), Msg->GetMarket(), Msg, R010Render );        
        strcpy( R010, R010Render->Render().c_str() );
        return true;
    }
    return false;    
}
//---------------------------------------------------------------------------
