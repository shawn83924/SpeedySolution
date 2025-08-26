//---------------------------------------------------------------------------
#include "TTaifexConnection.h"
#include "../ExchangeUtility/Utility.h"
//---------------------------------------------------------------------------
extern UFC::BufferedLog* Glog;
//---------------------------------------------------------------------------
inline nsOrderMessageDefine::OrderSourceEnum ToOrderSource(  char Code )
{
    switch( Code )
    {
        case 'D': return nsOrderMessageDefine::osDedicatedLine;
        case 'A': return nsOrderMessageDefine::osAPI;
        case 'M': return nsOrderMessageDefine::osMobile;
        case 'W': return nsOrderMessageDefine::osWeb;
        case 'P': return nsOrderMessageDefine::osPCApplication;
        case 'V': return nsOrderMessageDefine::osVoice;
        case 'G':
        default:  return nsOrderMessageDefine::osGeneral;
    }
}
//---------------------------------------------------------------------------
nsOrderMessageDefine::TradingSessionIDEnum TTaifexConnection::GetMessageTradeingSession( const UFC::AnsiString& Msg )
{
	int System = UFC::AnsiString::StrToInt32( Msg.c_str() , 2 );

	switch( System )
	{
		case 30:
		case 93:
			return nsOrderMessageDefine::tsNormal;
		case 40:
		case 94:
			return nsOrderMessageDefine::tsOddLot;
		case 33:
		case 83:
			return nsOrderMessageDefine::tsIntradayOdd;
		case 32:
		case 98:
			return nsOrderMessageDefine::tsOffHour;
		case 70:
			return nsOrderMessageDefine::tsAuction;
		case 96:
			return nsOrderMessageDefine::tsNegotiatePx;
		case 31:
		case 90:
			return nsOrderMessageDefine::tsLend;
		case 41:
		case 97:
			return nsOrderMessageDefine::tsTender;
		case 42:
		case 89:
			return nsOrderMessageDefine::tsTenderEx;
    }
    return nsOrderMessageDefine::tsNormal;
}
//---------------------------------------------------------------------------
UFC::TRenderData* TTaifexConnection::GetR010Render( nsOrderMessageDefine::MarketEnum Market, nsOrderMessageDefine::TradingSessionIDEnum  TradingSession )
{
    switch( Market )
    {
        case nsOrderMessageDefine::mTWFutures:
		case nsOrderMessageDefine::mTWOptions: return &FR010Render;
		case nsOrderMessageDefine::mES:        return &FTSET010Render;
		case nsOrderMessageDefine::mTSE:
				switch( TradingSession )
				{
						case nsOrderMessageDefine::tsAuto:
						case nsOrderMessageDefine::tsNormal:
						case nsOrderMessageDefine::tsIntradayOdd: return &FTSET010Render;
						case nsOrderMessageDefine::tsOddLot:  if( FIsTWSENewVersion == TRUE )
																  return &FTSEO010ExRender;
															  else
																  return &FTSEO010Render;
						case nsOrderMessageDefine::tsOffHour: if( FIsTWSENewVersion == TRUE )
																  return &FTSEP010ExRender;
															  else
																  return &FTSEP010Render;
						case nsOrderMessageDefine::tsAuction: if( FIsTWSEExNewVersion == TRUE )
																  return &FTSEA010ExRender;
															  else
																  return &FTSEA010Render;
						case nsOrderMessageDefine::tsLend:    if( FIsTWSEExNewVersion == TRUE )
																  return &FTSEV010ExRender;
															  else
																  return &FTSEV010Render;
						case nsOrderMessageDefine::tsTender:  if( FIsTWSEExNewVersion == TRUE )
																  return &FTSEE010ExRender;
															  else
																  return &FTSEE010Render;
						case nsOrderMessageDefine::tsTenderEx:if( FIsTWSEExNewVersion == TRUE )
																  return &FTSEEx010ExRender;
															  else
																  return &FTSEEx010Render;
						default : break;
				}
		case nsOrderMessageDefine::mOTC:
				switch( TradingSession )
				{
						case nsOrderMessageDefine::tsAuto:
						case nsOrderMessageDefine::tsNormal:
						case nsOrderMessageDefine::tsIntradayOdd:return &FOTCT010Render;
						case nsOrderMessageDefine::tsOddLot:  if( FIsTWSENewVersion == TRUE )
																  return &FOTCO010ExRender;
															  else
																  return &FOTCO010Render;
						case nsOrderMessageDefine::tsOffHour: if( FIsTWSENewVersion == TRUE )
																  return &FOTCP010ExRender;
															  else
																  return &FOTCP010Render;
						case nsOrderMessageDefine::tsLend:    if( FIsTWSEExNewVersion == TRUE )
																  return &FOTCV010ExRender;
															  else
																  return &FOTCV010Render;
						case nsOrderMessageDefine::tsTender:  if( FIsTWSEExNewVersion == TRUE )
																  return &FOTCE010ExRender;
															  else
																  return &FOTCE010Render;
						case nsOrderMessageDefine::tsTenderEx:if( FIsTWSEExNewVersion == TRUE )
																  return &FOTCEx010ExRender;
															  else
																  return &FOTCEx010Render;
						default : return NULL;
                }
                default: break;
	}
	return NULL;
}
//---------------------------------------------------------------------------
UFC::TRecord* TTaifexConnection::ParseConfirmReport( nsOrderMessageDefine::MarketEnum Market, nsOrderMessageDefine::MessageTypeEnum MessageType, const UFC::AnsiString& ConfirmMessage )
{
    int Length = ConfirmMessage.Length();

	if( Market == nsOrderMessageDefine::mTWOptions || Market == nsOrderMessageDefine::mTWFutures )
    {
        switch( MessageType )
        {
            case nsOrderMessageDefine::mtNew:
            case nsOrderMessageDefine::mtReplace:
            case nsOrderMessageDefine::mtCancel:
			case nsOrderMessageDefine::mtOrderStatusRequest: return FR020Parser.Parse( ConfirmMessage.c_str(), Length );
            case nsOrderMessageDefine::mtQuote:
            case nsOrderMessageDefine::mtQuoteCancel:        return FR100Parser.Parse( ConfirmMessage.c_str(), Length );
            case nsOrderMessageDefine::mtQuoteRequest:       return FR080Parser.Parse( ConfirmMessage.c_str(), Length );
            default: break;
        }
    }
	else if( Market == nsOrderMessageDefine::mTSE || Market == nsOrderMessageDefine::mES )
    {
        switch( MessageType )
        {
            case nsOrderMessageDefine::mtNew:
            case nsOrderMessageDefine::mtReplace:
            case nsOrderMessageDefine::mtReplacePx:
            case nsOrderMessageDefine::mtCancel:
            case nsOrderMessageDefine::mtOrderStatusRequest:
                switch( GetMessageTradeingSession( ConfirmMessage ) )
                {
                    case nsOrderMessageDefine::tsNormal:  if( FIsTWSENewVersion == TRUE )
															  return FTSET020ExParser.Parse( ConfirmMessage.c_str(), Length );
														  else
															  return FTSET020Parser.Parse( ConfirmMessage.c_str(), Length );
					case nsOrderMessageDefine::tsOddLot:  if( FIsTWSENewVersion == TRUE )
															  return FTSEO020ExParser.Parse( ConfirmMessage.c_str(), Length );
														  else
															  return FTSEO020Parser.Parse( ConfirmMessage.c_str(), Length );
					case nsOrderMessageDefine::tsIntradayOdd: return FTSET020ExParser.Parse( ConfirmMessage.c_str(), Length );
                    case nsOrderMessageDefine::tsOffHour: if( FIsTWSENewVersion == TRUE )
                                                              return FTSEP020ExParser.Parse( ConfirmMessage.c_str(), Length );
                                                          else
                                                              return FTSEP020Parser.Parse( ConfirmMessage.c_str(), Length );
                    case nsOrderMessageDefine::tsAuction: if( FIsTWSEExNewVersion == TRUE )
                                                              return FTSEA020ExParser.Parse( ConfirmMessage.c_str(), Length );
                                                          else
                                                              return FTSEA020Parser.Parse( ConfirmMessage.c_str(), Length );
                    case nsOrderMessageDefine::tsLend:    if( FIsTWSEExNewVersion == TRUE )
                                                              return FTSEV020ExParser.Parse( ConfirmMessage.c_str(), Length );
                                                          else
                                                              return FTSEV020Parser.Parse( ConfirmMessage.c_str(), Length );
                    case nsOrderMessageDefine::tsTender:  if( FIsTWSEExNewVersion == TRUE )
                                                              return FTSEE020ExParser.Parse( ConfirmMessage.c_str(), Length );
                                                          else
                                                              return FTSEE020Parser.Parse( ConfirmMessage.c_str(), Length );
                    case nsOrderMessageDefine::tsTenderEx:if( FIsTWSEExNewVersion == TRUE )
                                                              return FTSEEx020ExParser.Parse( ConfirmMessage.c_str(), Length );
                                                          else
                                                              return FTSEEx020Parser.Parse( ConfirmMessage.c_str(), Length );
                    default : break;
                }
                break;
            default: break;
         }
    }
    else if( Market == nsOrderMessageDefine::mOTC )
    {
        switch( MessageType )
        {
            case nsOrderMessageDefine::mtNew:
            case nsOrderMessageDefine::mtReplace:
            case nsOrderMessageDefine::mtReplacePx:
            case nsOrderMessageDefine::mtCancel:
            case nsOrderMessageDefine::mtOrderStatusRequest:
                    switch( GetMessageTradeingSession( ConfirmMessage ) )
                    {
						case nsOrderMessageDefine::tsNormal:  if( FIsTWSENewVersion == TRUE )
																  return FOTCT020ExParser.Parse( ConfirmMessage.c_str(), Length );
															  else
																  return FOTCT020Parser.Parse( ConfirmMessage.c_str(), Length );
						case nsOrderMessageDefine::tsOddLot:  if( FIsTWSENewVersion == TRUE )
																  return FOTCO020ExParser.Parse( ConfirmMessage.c_str(), Length );
															  else
																  return FOTCO020Parser.Parse( ConfirmMessage.c_str(), Length );
						case nsOrderMessageDefine::tsIntradayOdd: return FOTCT020ExParser.Parse( ConfirmMessage.c_str(), Length );
						case nsOrderMessageDefine::tsOffHour: if( FIsTWSENewVersion == TRUE )
																  return FOTCP020ExParser.Parse( ConfirmMessage.c_str(), Length );
															  else
																  return FOTCP020Parser.Parse( ConfirmMessage.c_str(), Length );
						case nsOrderMessageDefine::tsLend:    if( FIsTWSEExNewVersion == TRUE )
																  return FOTCV020ExParser.Parse( ConfirmMessage.c_str(), Length );
															  else
																  return FOTCV020Parser.Parse( ConfirmMessage.c_str(), Length );
						case nsOrderMessageDefine::tsTender:  if( FIsTWSEExNewVersion == TRUE )
																  return FOTCE020ExParser.Parse( ConfirmMessage.c_str(), Length );
															  else
																  return FOTCE020Parser.Parse( ConfirmMessage.c_str(), Length );
						case nsOrderMessageDefine::tsTenderEx:if( FIsTWSEExNewVersion == TRUE )
																  return FOTCEx020ExParser.Parse( ConfirmMessage.c_str(), Length );
															  else
																  return FOTCEx020Parser.Parse( ConfirmMessage.c_str(), Length );
						default : break;
                    }
                    break;
            default: break;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
int TTaifexConnection::GetTAIFEXPricePrecision( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& Symbol )
{
	if( Market  == nsOrderMessageDefine::mTWFutures )
	{
		if( FFUTSymbol != NULL && Symbol.Length() >= 5 )
		{
			UFC::AnsiString futSecName,prodID,PriceDigi;
			UFC::Section*   secPtr;

			if( Symbol[4] == 'X' && Symbol[3] == 'F' ) ///< Is FLEX
			{
				futSecName = "FLEX";
				prodID = Symbol.SubString( 0, 5 );
			}
			else
			{
				futSecName = "FuturesProducts";
				prodID = Symbol.SubString( 0, 3 );
			}
			if( (secPtr = FFUTSymbol->GetSection(futSecName)) != NULL )
			{
				if( secPtr->GetValue( prodID, PriceDigi ) == TRUE )
					return PriceDigi.ToInt();
				else
					Glog->fprintf( " *** Can not find price decimal locator for Futures Prod[%s] Symbol:%s. %d***", prodID.c_str(), Symbol.c_str(), secPtr->ItemCount() );
			}
			else
				Glog->fprintf( " *** Futures price decimal locator Section[%s] not Exist. ***", futSecName.c_str() );
		}
		else
			Glog->fprintf( " *** Futures price decimal locator table not Exist. ***" );
	}
	else if( Market  == nsOrderMessageDefine::mTWOptions )
	{
		if( FOPTSymbol != NULL  && Symbol.Length() >= 5 )
		{
			UFC::AnsiString optSecName,prodID,PriceDigi;
			UFC::Section*   secPtr;

			if( Symbol[4] == 'X' && Symbol[3] == 'O') ///< Is FLEX
			{
                            optSecName = "FLEX";
                            prodID = Symbol.SubString( 0, 5 );
			}
			else
			{
                            optSecName = "OptionProducts";
                            prodID = Symbol.SubString( 0, 3 );
			}
			if( (secPtr = FOPTSymbol->GetSection(optSecName)) != NULL )
			{
                            if( secPtr->GetValue( prodID, PriceDigi ) == TRUE )
                                return PriceDigi.ToInt();
                            else
                                Glog->fprintf( " *** Can not find price decimal locator for Options Prod[%s] Symbol:%s. %d***", prodID.c_str(), Symbol.c_str(), secPtr->ItemCount() );
			}
			else
                            Glog->fprintf( " *** Options price decimal locator Section[%s] not Exist. ***", optSecName.c_str() );
		}
		else
                    Glog->fprintf( " *** Options price decimal locator table not Exist. ***" );
	}
	return -1;
}
//---------------------------------------------------------------------------
int TTaifexConnection::GetPricePrecision( nsOrderMessageDefine::MarketEnum Market, nsOrderMessageDefine::TradingSessionIDEnum TradeSession, const UFC::AnsiString& Symbol )
{
    UFC::AnsiString PriceDigi;
    if( Market  == nsOrderMessageDefine::mTWFutures )
    {
        int Digi = 2;
		if( FFUTSymbol != NULL )
		{
			UFC::AnsiString futSecName,prodID;
			UFC::Section*   secPtr;

			if( Symbol[4] == 'X' && Symbol[3] == 'F' ) ///< Is FLEX
			{
				futSecName = "FLEX";
				prodID = Symbol.SubString( 0, 5 );
			}
			else
			{
				futSecName = "FuturesProducts";
				prodID = Symbol.SubString( 0, 3 );
			}
			if( (secPtr = FFUTSymbol->GetSection(futSecName)) != NULL )
			{
				if( secPtr->GetValue( prodID, PriceDigi ) == TRUE )
					Digi = PriceDigi.ToInt();
				else
					Glog->fprintf( " *** Can not find price decimal locator for Futures Prod[%s] Symbol:%s. %d***", prodID.c_str(), Symbol.c_str(), secPtr->ItemCount() );
			}
			else
				Glog->fprintf( " *** Futures price decimal locator Section[%s] not Exist. ***", futSecName.c_str() );
		}
		else
			Glog->fprintf( " *** Futures price decimal locator table not Exist. ***" );
		return Digi;
	}
	else if( Market  == nsOrderMessageDefine::mTWOptions )
	{
		int Digi = 3;
		if( FOPTSymbol != NULL )
		{
			UFC::AnsiString optSecName,prodID;
			UFC::Section*   secPtr;

			if( Symbol[4] == 'X' && Symbol[3] == 'O') ///< Is FLEX
			{
				optSecName = "FLEX";
				prodID = Symbol.SubString( 0, 5 );
			}
			else
			{
				optSecName = "OptionProducts";
				prodID = Symbol.SubString( 0, 3 );
			}
			if( (secPtr = FOPTSymbol->GetSection(optSecName)) != NULL )
			{
				if( secPtr->GetValue( prodID, PriceDigi ) == TRUE )
					Digi = PriceDigi.ToInt();
				else
					Glog->fprintf( " *** Can not find price decimal locator for Options Prod[%s] Symbol:%s. %d***", prodID.c_str(), Symbol.c_str(), secPtr->ItemCount() );
			}
			else
				Glog->fprintf( " *** Options price decimal locator Section[%s] not Exist. ***", optSecName.c_str() );
		}
		else
			Glog->fprintf( " *** Options price decimal locator table not Exist. ***" );
		return Digi;
	}
	else ///< TSE or OTC
	{
		switch( TradeSession )
		{
			case nsOrderMessageDefine::tsNormal:
			case nsOrderMessageDefine::tsOddLot:
			case nsOrderMessageDefine::tsOffHour:
				if( FIsTWSENewVersion == true )///< New version price 9(5)V9(4) 2020/03/23
					return 4;
				return 2;///< price 9(4)V9(2)
			case nsOrderMessageDefine::tsIntradayOdd:
				return 4;
			case nsOrderMessageDefine::tsAuction:
			case nsOrderMessageDefine::tsTender:
			case nsOrderMessageDefine::tsTenderEx:
				if( FIsTWSEExNewVersion == true )///< New version price 9(5)V9(4) 2020/03/02
					return 4;
				return 2;///< price 9(4)V9(2)
			case nsOrderMessageDefine::tsLend:    ///< Lend session price 9(3)V9(4) new version 9(4)V9(4)
				return 4;
			default:
				if( FIsTWSENewVersion == true )///< New version price 9(5)V9(4)
					return 4;
				return 2;
        }
    }
}
//------------------------------------------------------------------------------
bool TTaifexConnection::IsExecutionDup( nsOrderMessageDefine::MarketEnum  Market, const UFC::AnsiString& ExecID )
{
    UFC::PLockObject Locker( FExecIDCS );

    switch( Market )
    {
        case nsOrderMessageDefine::mTWFutures:
                                        if( FFUTExecIDSet.Exists( ExecID ) == FALSE )
                                        {
                                            FFUTExecIDSet.Add( ExecID ); ///< Add to hash set.
                                            return false;
                                        }
                                        break;
        case nsOrderMessageDefine::mTWOptions:
                                        if( FOPTExecIDSet.Exists( ExecID ) == FALSE )
                                        {
                                            FOPTExecIDSet.Add( ExecID ); ///< Add to hash set.
                                            return false;
                                        }
                                        break;
		case nsOrderMessageDefine::mTSE:if( FTSEExecIDSet.Exists( ExecID ) == FALSE )
										{
											FTSEExecIDSet.Add( ExecID ); ///< Add to hash set.
											return false;
										}
										break;
		case nsOrderMessageDefine::mES: if( FESExecIDSet.Exists( ExecID ) == FALSE )
										{
											FESExecIDSet.Add( ExecID ); ///< Add to hash set.
                                            return false;
                                        }
                                        break;
        case nsOrderMessageDefine::mOTC:
                                        if( FOTCExecIDSet.Exists( ExecID ) == FALSE )
                                        {
                                            FOTCExecIDSet.Add( ExecID ); ///< Add to hash set.
                                            return false;
                                        }
                                        break;
        case nsOrderMessageDefine::mForeignStock:
        case nsOrderMessageDefine::mForeignFutures:
        case nsOrderMessageDefine::mForeignOptions:
                                        if( FPATSExecIDSet.Exists( ExecID ) == FALSE )
                                        {
                                            FPATSExecIDSet.Add( ExecID ); ///< Add to hash set.
                                            return false;
                                        }
                                        break;
        default:                        break;
    }
    return true;
}
//---------------------------------------------------------------------------
void TTaifexConnection::SetExecSide( UFC::TRecord* pRecord,  TExecutionReportMessage* ExecutionReport )
{        
    UFC::AnsiString Value;
    
    if( pRecord->GetField( "Side", Value ) )
    {
        if( Value == "B" )
            ExecutionReport->SetSide( nsOrderMessageDefine::sBuy );
        else if( Value == "S" )
            ExecutionReport->SetSide( nsOrderMessageDefine::sSell );
        else
            ExecutionReport->SetSide( nsOrderMessageDefine::sBuy );
    }    
}
//---------------------------------------------------------------------------
void TTaifexConnection::SetExecOrderType( UFC::TRecord* pRecord,  TExecutionReportMessage* ExecutionReport )
{        
    UFC::AnsiString Value;
    
    if( pRecord->GetField( "OrderType", Value ) )
    {
        if( Value == "1" || Value == "M")
            ExecutionReport->SetOrderType( nsOrderMessageDefine::otMarket );
        else if( Value == "2" || Value == "L")
            ExecutionReport->SetOrderType( nsOrderMessageDefine::otLimit );
        else if( Value == "3" || Value == "P" )
            ExecutionReport->SetOrderType( nsOrderMessageDefine::otMarketWithProtection );
        else
            ExecutionReport->SetOrderType( nsOrderMessageDefine::otLimit );
    }
    else
        ExecutionReport->SetOrderType( nsOrderMessageDefine::otLimit );
}
//---------------------------------------------------------------------------
void TTaifexConnection::SetExecTimeInForce( nsOrderMessageDefine::MarketEnum Market, UFC::TRecord* pRecord,  TExecutionReportMessage* ExecutionReport )
{
    UFC::AnsiString Value;

    if( pRecord->GetField( "TimeInForce", Value ) )
    {
        if( Market == nsOrderMessageDefine::mTSE || Market == nsOrderMessageDefine::mOTC || Market == nsOrderMessageDefine::mES )
        {
            if(  Value == "3" )
                    ExecutionReport->SetTimeInForce( nsOrderMessageDefine::tifIOC );
            else if( Value == "4" )
                    ExecutionReport->SetTimeInForce( nsOrderMessageDefine::tifFOK );
            else
                    ExecutionReport->SetTimeInForce( nsOrderMessageDefine::tifROD );
        }
        else
        {
            if( Value == "R" )
                    ExecutionReport->SetTimeInForce( nsOrderMessageDefine::tifROD );
            else if( Value == "I" || Value == "Q" )///< quote Q order.
                    ExecutionReport->SetTimeInForce( nsOrderMessageDefine::tifIOC );
            else if( Value == "F" )
                    ExecutionReport->SetTimeInForce( nsOrderMessageDefine::tifFOK );
            else if( Value == "4" )
                    ExecutionReport->SetTimeInForce( nsOrderMessageDefine::tifGTC );
            else if( Value == "5" )
                    ExecutionReport->SetTimeInForce( nsOrderMessageDefine::tifGTD );
        }
    }
    else
        ExecutionReport->SetTimeInForce( nsOrderMessageDefine::tifROD );
}
//---------------------------------------------------------------------------
void TTaifexConnection::FillRejectExecution( nsOrderMessageDefine::MessageTypeEnum MessageType,
                                             nsOrderMessageDefine::MarketEnum Market,
                                             nsOrderMessageDefine::TradingSessionIDEnum TradingSession,
                                             UFC::TRecord* pRecord,
                                             UFC::AnsiString StatusCode,
                                             UFC::AnsiString ErrMsg,
                                             TExecutionReportMessage* ExecutionReport,
                                             int& Precision )
{
    UFC::AnsiString Value,Symbol,OldSymbol,MaturityMonthYear;
    int BidPx,BidQty;

    ///< Fetch the symbol
    if( pRecord->GetField( "Symbol", Symbol ) )
    {
    	Symbol.TrimRight();
        if( Symbol.Length() > 0 )
        {
            OldSymbol = Symbol;
            Precision = GetPricePrecision( Market, TradingSession, Symbol );
            pRecord->GetField( "MaturityMonthYear", MaturityMonthYear );///< New Futures R020 don't have this field
            if( Market == nsOrderMessageDefine::mTWFutures && FNewFutSymbol == FALSE )
                    FFutSymbol.ConvertToOldSymbol( Symbol, OldSymbol, MaturityMonthYear);
            if( MaturityMonthYear.Length() == 6 )
                    ExecutionReport->SetMaturityMonthYear( MaturityMonthYear.c_str() );
            ExecutionReport->SetSymbol( OldSymbol.c_str() );
        }
    }
    ExecutionReport->SetPxDigit( Precision );
    if( pRecord->GetField( "BrokerID", Value ) )
        ExecutionReport->SetBrokerID( Value.c_str() );
    if( pRecord->GetField( "OrderID", Value ) )
        ExecutionReport->SetOrderID( Value.c_str() );
    if( pRecord->GetField( "Account", Value ) )
        ExecutionReport->SetAccount( Value.c_str() );
    if( pRecord->GetField( "AccountFlag", Value ) )
        ExecutionReport->SetAccountFlag( Value.c_str() );
    if( pRecord->GetField( "Price", Value ) )
        ExecutionReport->SetPrice( UFC::IntToDouble( StringToInt( Value, 0 ), Precision ) );
    if( pRecord->GetField( "Qty", Value ) )
        ExecutionReport->SetOrderQty( StringToInt( Value, 0 ) );
    if( pRecord->GetField( "OrderTime", Value ) ) ///< Time from T020 new version
    {
        Value.PadThis( 9,'0' );
        ExecutionReport->SetTransactTime( Value.c_str() );
    }
    else if( pRecord->GetField( "MessageTime", Value ) ) ///< Reject order have only header.
    {
        Value.PadThis( 8,'0' );
        ExecutionReport->SetTransactTime( Value.c_str() );
    }
    SetExecSide( pRecord,  ExecutionReport );
    SetExecOrderType( pRecord,  ExecutionReport );
    SetExecTimeInForce( Market, pRecord, ExecutionReport );
    int functionCode = 0;
    if( pRecord->GetField( "FunctionCode", Value ) ) functionCode = StringToInt( Value, 0 );    
    switch( MessageType )
    {
            case nsOrderMessageDefine::mtNew:
                    FillRejectMsg( nsOrderMessageDefine::crrNew, StatusCode, ErrMsg, ExecutionReport );
                    break;
            case nsOrderMessageDefine::mtReplace:
                    if( (Market == nsOrderMessageDefine::mTSE || Market == nsOrderMessageDefine::mOTC|| Market == nsOrderMessageDefine::mES) && functionCode == 6)
                        ExecutionReport->SetReplacePx( true );
                    else
                        ExecutionReport->SetReplacePx( false );
                    FillRejectMsg( nsOrderMessageDefine::crrReplace, StatusCode, ErrMsg, ExecutionReport );
                    break;
            case nsOrderMessageDefine::mtReplacePx:
                    ExecutionReport->SetReplacePx( true );
                    FillRejectMsg( nsOrderMessageDefine::crrReplace, StatusCode, ErrMsg, ExecutionReport );
                    break;
            case nsOrderMessageDefine::mtCancel:
                    FillRejectMsg( nsOrderMessageDefine::crrCancel, StatusCode, ErrMsg, ExecutionReport );
                    break;
            case nsOrderMessageDefine::mtOrderStatusRequest:
                    FillRejectMsg( nsOrderMessageDefine::crrOrderStatus, StatusCode, ErrMsg, ExecutionReport );
                    break;
            case nsOrderMessageDefine::mtQuote:
                    if( pRecord->GetField( "BidPrice", Value ) )
                        BidPx = Value.ToInt();
                    else
                        BidPx = 0;
                    if( pRecord->GetField( "BidQty", Value ) )
                        BidQty = Value.ToInt();
                    else
                        BidQty = 0;
                    if( BidPx == 0 && BidQty == 0 )
                        ExecutionReport->SetSide( nsOrderMessageDefine::sSell );
                    else
                        ExecutionReport->SetSide( nsOrderMessageDefine::sBuy );
                    FillRejectMsg( nsOrderMessageDefine::crrQuote, StatusCode, ErrMsg, ExecutionReport );
                    break;
            case nsOrderMessageDefine::mtQuoteCancel:
                    if( pRecord->GetField( "BidPrice", Value ) )
                        BidPx = Value.ToInt();
                    else
                        BidPx = 0;
                    if( pRecord->GetField( "BidQty", Value ) )
                        BidQty = Value.ToInt();
                    else
                        BidQty = 0;
                    if( BidPx == 0 && BidQty == 0 )
                        ExecutionReport->SetSide( nsOrderMessageDefine::sSell );
                    else
                        ExecutionReport->SetSide( nsOrderMessageDefine::sBuy );
                    FillRejectMsg( nsOrderMessageDefine::crrQuoteCancel, StatusCode, ErrMsg, ExecutionReport );
                    break;
            case nsOrderMessageDefine::mtQuoteRequest:
                    FillRejectMsg( nsOrderMessageDefine::crrQuoteRequest, StatusCode, ErrMsg, ExecutionReport );
                    break;
            default: break;
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::FillExecution( nsOrderMessageDefine::MessageTypeEnum MessageType,
                                       nsOrderMessageDefine::MarketEnum Market,
                                       nsOrderMessageDefine::TradingSessionIDEnum TradingSession,
                                       UFC::TRecord* pRecord,
                                       TExecutionReportMessage* ExecutionReport,
                                       int& Precision )
{
    UFC::AnsiString Value,Symbol,OldSymbol,MaturityMonthYear;
    int BidPx,AskPx,BidQty,AskQty;

    ///< Fetch the symbol
    if( pRecord->GetField( "Symbol", Symbol ) )
    {
        Symbol.TrimRight();
        OldSymbol = Symbol;
		Precision = GetPricePrecision( Market, TradingSession, Symbol );
        pRecord->GetField( "MaturityMonthYear", MaturityMonthYear );///< New Futures R020 don't have this field
        if( Market == nsOrderMessageDefine::mTWFutures && FNewFutSymbol == FALSE )
            FFutSymbol.ConvertToOldSymbol( Symbol, OldSymbol, MaturityMonthYear);
        if( MaturityMonthYear.Length() == 6 )
            ExecutionReport->SetMaturityMonthYear( MaturityMonthYear.c_str() );
        ExecutionReport->SetSymbol( OldSymbol.c_str() );
    }
    ExecutionReport->SetPxDigit( Precision );
    if( pRecord->GetField( "OrderTime", Value ) ) ///< Time from T020 new version
    {
        Value.PadThis( 9,'0' );
        ExecutionReport->SetTransactTime( Value.c_str() );
    }
    else if( pRecord->GetField( "ProcessTime", Value ) ) ///< Get from confirm execution.
    {
        ExecutionReport->SetTransactTime( Value.c_str() );
    }
    else if( pRecord->GetField( "MessageTime", Value ) )///< Old Futures R020 message don't have field "ProcessTime"
    {
        Value.PadThis( 8,'0' );
        ExecutionReport->SetTransactTime( Value.c_str() );
    }        
    if( pRecord->GetField( "BrokerID", Value ) )
        ExecutionReport->SetBrokerID( Value.c_str() );
    if( pRecord->GetField( "OrderID", Value ) )
        ExecutionReport->SetOrderID( Value.c_str() );
    if( pRecord->GetField( "Account", Value ) )
        ExecutionReport->SetAccount( Value.c_str() );
    if( pRecord->GetField( "AccountFlag", Value ) )
        ExecutionReport->SetAccountFlag( Value.c_str() );
    if( pRecord->GetField( "MaturityMonthYear", Value ) ) ///< New Futures R020 don't have this field
        ExecutionReport->SetMaturityMonthYear( Value.c_str() );
    if( pRecord->GetField( "StrikePrice", Value ) )        ///< New Futures R020 don't have this field
        ExecutionReport->SetStrikePrice( UFC::IntToDouble( StringToInt( Value, 0 ), Precision ) );
    if( pRecord->GetField( "Price", Value ) )
        ExecutionReport->SetPrice( UFC::IntToDouble( StringToInt( Value, 0 ), Precision ) );
    if( pRecord->GetField( "Qty", Value ) )
        ExecutionReport->SetOrderQty( StringToInt( Value, 0 ) );
    if( pRecord->GetField( "BeforeQty", Value ) )
        ExecutionReport->SetBeforeQty( StringToInt( Value, 0 ) );
    if( pRecord->GetField( "AfterQty", Value ) )
    {
        int lqty = StringToInt( Value, 0 );

        ExecutionReport->SetAfterQty( lqty );
        ExecutionReport->SetLeavesQty( lqty );
    }
    if( pRecord->GetField( "STK-SEQ-NO", Value ) )
        ExecutionReport->SetStockSeqNo( StringToInt( Value, 0 ) );
    if( pRecord->GetField( "ClearMemberID", Value ) )
        ExecutionReport->SetCMID(  Value );
    SetExecSide( pRecord,  ExecutionReport );        
    SetExecOrderType( pRecord,  ExecutionReport );
    SetExecTimeInForce( Market, pRecord, ExecutionReport );	
    if( pRecord->GetField( "OpenOffsetFlag", Value ) )
    {
        if( Value == "0" )
            ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peOpen );
        else if( Value == "1" )
            ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peClose );
        else if( Value == "2" )
			ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peDayTrade );
        else if( Value == "A" )
            ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peTAIFEXAuto );
        else if( Value == "7" )
            ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peForceClose );        
        else if( Value == "9" )
            ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peTMPMarketMaker );
    }
    else
        ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peOpen );
    
    int functionCode = 0;
    if( pRecord->GetField( "FunctionCode", Value ) ) functionCode = StringToInt( Value, 0 );
            
    switch( MessageType )
    {
            case nsOrderMessageDefine::mtNew:
                            if( functionCode == 4 )  //Canceled by Exchange
                            {
                                ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osCanceled );
                                ExecutionReport->SetExecType( nsOrderMessageDefine::etCanceled );
                            }
                            else
                            {
                                ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osNew );
                                ExecutionReport->SetExecType( nsOrderMessageDefine::etNew );
                            }
                            break;
            case nsOrderMessageDefine::mtReplace:
                            ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osReplaced );
                            ExecutionReport->SetExecType( nsOrderMessageDefine::etReplaced );
                            if( (Market == nsOrderMessageDefine::mTSE || Market == nsOrderMessageDefine::mOTC|| Market == nsOrderMessageDefine::mES) && functionCode == 6)
                                ExecutionReport->SetReplacePx( true );
                            else
                                ExecutionReport->SetReplacePx( false );
                            break;
            case nsOrderMessageDefine::mtReplacePx:
                            ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osReplaced );
                            ExecutionReport->SetExecType( nsOrderMessageDefine::etReplaced );
                            ExecutionReport->SetReplacePx( true );
                            break;
            case nsOrderMessageDefine::mtCancel:
                            if( Market == nsOrderMessageDefine::mTSE || Market == nsOrderMessageDefine::mOTC  || Market == nsOrderMessageDefine::mES )
                            {   ///< TSE/OTC are one step order canceled process.
                                ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osCanceled );
                                ExecutionReport->SetExecType( nsOrderMessageDefine::etCanceled );
                                if( TradingSession == nsOrderMessageDefine::tsAuction )
                                {
                                    if( pRecord->GetField( "BEFORE-PRICE", Value ) )
                                        ExecutionReport->SetPrice( UFC::IntToDouble( StringToInt( Value, 0 ), Precision ) );
                                }
                            }
                            else
                            {
                                ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osPendingCancel );
                                ExecutionReport->SetExecType( nsOrderMessageDefine::etPendingCancel );
                            }
                            break;
            case nsOrderMessageDefine::mtOrderStatusRequest:
                            ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osNew );
                            ExecutionReport->SetExecType( nsOrderMessageDefine::etOrderStatus );
                            break;
            case nsOrderMessageDefine::mtQuoteRequest:
                            ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osQuoteRequestAccept );
                            ExecutionReport->SetExecType( nsOrderMessageDefine::etQuoteRequestAccept );
                            break;
            case nsOrderMessageDefine::mtQuote:
                            ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osQuoteAccept );
                            ExecutionReport->SetExecType( nsOrderMessageDefine::etQuoteAccept );
                            if( pRecord->GetField( "BidPrice", Value ) )
                            {
                                BidPx = Value.ToInt();
                                ExecutionReport->SetLegPrice1( UFC::IntToDouble( BidPx,Precision) );
                            }
                            else
                                BidPx = 0;
                            if( pRecord->GetField( "AskPrice", Value ) )
                            {
								AskPx = Value.ToInt();
                                ExecutionReport->SetLegPrice2( UFC::IntToDouble( AskPx,Precision) );
                            }
                            else
                                AskPx = 0;
                            if( pRecord->GetField( "BidQty", Value ) )
                            {
                                BidQty = Value.ToInt();
                                ExecutionReport->SetLegQty1( BidQty );
                            }
                            else
                                BidQty = 0;
                            if( pRecord->GetField( "AskQty", Value ) )
                            {
                                AskQty = Value.ToInt();
                                ExecutionReport->SetLegQty2( AskQty );
                            }
                            else
                                AskQty = 0;
                            if( BidPx == 0 && BidQty == 0 )
                            {
                                ExecutionReport->SetPrice( UFC::IntToDouble( AskPx, Precision ) );
                                ExecutionReport->SetOrderQty( AskQty );
                                ExecutionReport->SetSide( nsOrderMessageDefine::sSell );
                            }
                            else
                            {
                                ExecutionReport->SetPrice( UFC::IntToDouble( BidPx, Precision ) );
                                ExecutionReport->SetOrderQty( BidQty );
                                ExecutionReport->SetSide( nsOrderMessageDefine::sBuy );
                            }
                            break;
            case nsOrderMessageDefine::mtQuoteCancel:
                            ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osPendingCancel );
                            ExecutionReport->SetExecType( nsOrderMessageDefine::etPendingCancel );
                            if( pRecord->GetField( "BidPrice", Value ) )
                            {
                                BidPx = Value.ToInt();
                                ExecutionReport->SetLegPrice1( UFC::IntToDouble( BidPx,Precision) );
                            }
                            else
                                BidPx = 0;
                            if( pRecord->GetField( "AskPrice", Value ) )
                            {
                                AskPx = Value.ToInt();
                                ExecutionReport->SetLegPrice2( UFC::IntToDouble( AskPx,Precision) );
                            }
                            else
                                AskPx = 0;
                            if( pRecord->GetField( "BidQty", Value ) )
							{
                                BidQty = Value.ToInt();
                                ExecutionReport->SetLegQty1( BidQty );
                            }
                            else
                                BidQty = 0;
                            if( pRecord->GetField( "AskQty", Value ) )
                            {
                                AskQty = Value.ToInt();
                                ExecutionReport->SetLegQty2( AskQty );
                            }
                            else
                                AskQty = 0;
                            if( BidPx == 0 && BidQty == 0 )
                            {
                                ExecutionReport->SetPrice( UFC::IntToDouble( AskPx, Precision ) );
                                ExecutionReport->SetOrderQty( AskQty );
                                ExecutionReport->SetSide( nsOrderMessageDefine::sSell );
                            }
                            else
                            {
                                ExecutionReport->SetPrice( UFC::IntToDouble( BidPx, Precision ) );
                                ExecutionReport->SetOrderQty( BidQty );
                                ExecutionReport->SetSide( nsOrderMessageDefine::sBuy );
                            }
                            break;
            default: break;
	}
}
//------------------------------------------------------------------------------
void TTaifexConnection::UpdateTMPFields( UFC::AnsiString& TMPExtStr, TExecutionReportMessage& ExecutionReport, int Precision )
{
	TMPExtFields    TMPFields( TMPExtStr );
	UFC::AnsiString Time;

	ExecutionReport.SetTMPExt( TMPExtStr );
	ExecutionReport.SetReportSequence( TMPFields.RptSeq );
	ExecutionReport.SetTMPExecType( (nsOrderMessageDefine::TMPExecTypeEnum) TMPFields.ExecType );
	ExecutionReport.SetTMPBrokerID( TMPFields.LineBrokerID );
	ExecutionReport.SetTMPStstusCode( TMPFields.StatusCode );
        ExecutionReport.SetTMPPartID( TMPFields.PartID );
	ExecutionReport.SetTMPQty( TMPFields.Qty );
	ExecutionReport.SetCumQty( TMPFields.CumQty );
	ExecutionReport.SetLeavesQty( TMPFields.LeavesQty );
	ExecutionReport.SetBeforeQty( TMPFields.BeforeQty );
	ExecutionReport.SetAfterQty( TMPFields.LeavesQty );
	Time.Printf( "%09d", TMPFields.OrgTransTime );
	ExecutionReport.SetOrgTransactTime( Time.c_str() );
	Time.Printf( "%09d", TMPFields.TransTime );
	ExecutionReport.SetTransactTime( Time.c_str() );
	ExecutionReport.SetTMPUniqueID( TMPFields.UniqID );
	ExecutionReport.SetOrderType( (nsOrderMessageDefine::OrderTypeEnum)TMPFields.OrdType );
	switch( TMPFields.TimeInForce )
	{
		case 3:  ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifIOC );break;
		case 4:  ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifFOK );break;
		case 8:  ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifTFXQ );break;
		default: ExecutionReport.SetTimeInForce( nsOrderMessageDefine::tifROD );break;
	}
	switch( TMPFields.PositionEffect )
	{
		case 'O': ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peOpen );break;
		case 'C': ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peClose );break;
		case 'D': ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peDayTrade );break;
                case '7': ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peForceClose );break;        
		case '9': ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peTMPMarketMaker );break;
                case 'A': ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peTAIFEXAuto );break;
		default: ExecutionReport.SetPositionEffect( nsOrderMessageDefine::peOpen );break;
	}
	///< Order state == New
	if( ExecutionReport.GetOrderStatus() == nsOrderMessageDefine::osNew )
	{
		if( FX25StyleQtyPx == true )///< X.25 style Price/qty
			ExecutionReport.SetLastPx( UFC::IntToDouble( TMPFields.Price, Precision ) );
		else ///< TMP/FIX Style Price/qty
			ExecutionReport.SetPrice( UFC::IntToDouble( TMPFields.Price, Precision ));
		ExecutionReport.SetReplacePx( false );
	}///< Order state == Canceled
	else if( ExecutionReport.GetOrderStatus() == nsOrderMessageDefine::osCanceled )
	{
		 if( FX25StyleQtyPx == true )///< X.25 style Price/qty
			 ExecutionReport.SetOrderQty( TMPFields.BeforeQty - TMPFields.LastQty );
		 else
			 ExecutionReport.SetOrderQty( TMPFields.Qty ); ///< Original Order qty.
		 ExecutionReport.SetLastQty( 0 );
		 ExecutionReport.SetLastPx( UFC::IntToDouble( TMPFields.Price, Precision ) );
		 ExecutionReport.SetPrice( ExecutionReport.GetLastPx( ));
		 ExecutionReport.SetReplacePx( false );
	} ///< Order state == Partially Filled
	else if( ExecutionReport.GetOrderStatus() == nsOrderMessageDefine::osPartiallyFilled )
	{
		if( FX25StyleQtyPx == true )///< X.25 style Price/qty
		{   ///< 1. OrderQty, Last filled qty.
			///< 2. Price, Last filled price.
			///< 3. LastPx, Original order price.
			ExecutionReport.SetLastQty( TMPFields.LastQty );
			ExecutionReport.SetLastPx( UFC::IntToDouble( TMPFields.Price, Precision ) );
		}
		else ///< TMP/FIX Style Price/qty
		{
			ExecutionReport.SetOrderQty( TMPFields.Qty );
			ExecutionReport.SetPrice(UFC::IntToDouble( TMPFields.Price, Precision ));
		}
		if( TMPFields.LeavesQty == 0 &&	FEnableFilledState == TRUE )
		{   ///< Change Order state to filled.
			ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osFilled );
			ExecutionReport.SetExecType( nsOrderMessageDefine::etFilled );
		}
		ExecutionReport.SetTMPPxSubTotal( (double)TMPFields.PxSubTotal );
		if( TMPFields.CumQty > 0 )
			ExecutionReport.SetAvgPx( (double)TMPFields.PxSubTotal/(double)TMPFields.CumQty );
		ExecutionReport.SetReplacePx( false );
	} ///< Execution for replace Px.
	else if( ( ExecutionReport.GetTMPExecType( ) == nsOrderMessageDefine::tetPxReplaced ||
		ExecutionReport.GetTMPExecType( ) == nsOrderMessageDefine::tetPxReplaced2 )
		&& TMPFields.StatusCode == 0 )
	{
		ExecutionReport.SetReplacePx( true );
		if( FX25StyleQtyPx == true )///< X.25 style Price/qty
			ExecutionReport.SetOrderQty( TMPFields.BeforeQty ); ///< Available qty.
		ExecutionReport.SetLastPx( ExecutionReport.GetPrice());
		ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osReplaced );
		ExecutionReport.SetExecType( nsOrderMessageDefine::etReplaced );
	}
	else
		ExecutionReport.SetReplacePx( false );
	///< For TAIFEX dynamic price limitation, 47,48 means canceled by exchange, not error
        ///< For TAIFEX COD, 54  means canceled by exchange, not error
	if( TMPFields.StatusCode == 47 || TMPFields.StatusCode == 48 || TMPFields.StatusCode == 54 )
	{
            UFC::AnsiString ErrorStr;
            UFC::AnsiString Code( TMPFields.StatusCode );

            GetRejectMsg( Code.c_str(), "", &ExecutionReport, ErrorStr );        	            
            ExecutionReport.SetStatusCode( Code.c_str() );
            ExecutionReport.SetText( ErrorStr.c_str());
            Glog->fprintf( " Canceled by TAIFEX with StatusCode[%s]", Code.c_str() );
	}
	Glog->fprintf( " Symbol[%s] Side[%d] Qty[%d] Before[%d] Leaves[%d] Last[%d] Cum[%d] LastPx:[%lf] OrdStatus[%d] ExexType[%d]", ExecutionReport.GetSymbol(), ExecutionReport.GetSide(), ExecutionReport.GetOrderQty(), TMPFields.BeforeQty, TMPFields.LeavesQty, TMPFields.LastQty, TMPFields.CumQty, UFC::IntToDouble( TMPFields.Price, Precision ), ExecutionReport.GetOrderStatus(), ExecutionReport.GetExecType() );
}
//---------------------------------------------------------------------------
void TTaifexConnection::FillStockExecution( nsOrderMessageDefine::MessageTypeEnum MessageType,
                                            nsOrderMessageDefine::MarketEnum Market,
                                            nsOrderMessageDefine::TradingSessionIDEnum TradingSession,
                                            UFC::TRecord* pRecord,
                                            TExecutionReportMessage* ExecutionReport,
                                            int& Precision )
{
	UFC::AnsiString Value,Symbol,OldSymbol,MaturityMonthYear;
	int functionCode = 0;

	if( pRecord->GetField( "FunctionCode", Value ) )
            functionCode = StringToInt( Value, 0 );
	///< Fetch the symbol
	if( pRecord->GetField( "Symbol", Symbol ) )
	{
            Symbol.TrimRight();
            Precision = GetPricePrecision( Market, TradingSession, Symbol );
            ExecutionReport->SetSymbol( Symbol.c_str() );
	}
	ExecutionReport->SetPxDigit( Precision );
	///< Fetch TransactTime
	if( pRecord->GetField( "OrderTime", Value )   || ///< Time from confirm execution.(T020,O020,P020)
		pRecord->GetField( "ProcessTime", Value ) || ///< Get from Filled execution.(R3)
		pRecord->GetField( "ORDER-TIME", Value )  || ///< Special Session.(A020,E020,V020,Ex020)
		pRecord->GetField( "MessageTime", Value ) )  ///< Use header time.
	{
            Value.PadThis( 9,'0' );
            ExecutionReport->SetTransactTime( Value.c_str() );
	}
	if( pRecord->GetField( "BrokerID", Value ) )
            ExecutionReport->SetBrokerID( Value.c_str() );
	if( pRecord->GetField( "OrderID", Value ) )
            ExecutionReport->SetOrderID( Value.c_str() );
	if( pRecord->GetField( "Account", Value ) )
            ExecutionReport->SetAccount( Value.c_str() );
	if( pRecord->GetField( "AccountFlag", Value ) )
            ExecutionReport->SetAccountFlag( Value.c_str() );
	if( pRecord->GetField( "Price", Value ) )
            ExecutionReport->SetPrice( UFC::IntToDouble( StringToInt( Value, 0 ), Precision ) );
	if( pRecord->GetField( "Qty", Value ) )
            ExecutionReport->SetOrderQty( StringToInt( Value, 0 ) );
	if( pRecord->GetField( "BeforeQty", Value ) )
            ExecutionReport->SetBeforeQty( StringToInt( Value, 0 ) );
	if( pRecord->GetField( "AfterQty", Value ) )
	{
            int lqty = StringToInt( Value, 0 );
            ExecutionReport->SetAfterQty( lqty );
            ExecutionReport->SetLeavesQty( lqty );
	}
	if( pRecord->GetField( "STK-SEQ-NO", Value ) ) ///< E020,Ex020
            ExecutionReport->SetStockSeqNo( StringToInt( Value, 0 ) );
	SetExecSide( pRecord,  ExecutionReport );
	SetExecOrderType( pRecord,  ExecutionReport );
	SetExecTimeInForce( Market, pRecord, ExecutionReport );
	ExecutionReport->SetPositionEffect( nsOrderMessageDefine::peOpen );
	switch( MessageType )
        {
            case nsOrderMessageDefine::mtNew:
                                            if( functionCode == 4 )  //Canceled by Exchange
                                            {
                                                ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osCanceled );
                                                ExecutionReport->SetExecType( nsOrderMessageDefine::etCanceled );
                                            }
                                            else
                                            {
                                                ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osNew );
                                                ExecutionReport->SetExecType( nsOrderMessageDefine::etNew );
                                            }
                                            break;
            case nsOrderMessageDefine::mtReplace:
                                            ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osReplaced );
                                            ExecutionReport->SetExecType( nsOrderMessageDefine::etReplaced );
                                            if( functionCode == 6 )
												ExecutionReport->SetReplacePx( true );
                                            else
												ExecutionReport->SetReplacePx( false );
                                            break;
            case nsOrderMessageDefine::mtReplacePx:
                                            ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osReplaced );
                                            ExecutionReport->SetExecType( nsOrderMessageDefine::etReplaced );
                                            ExecutionReport->SetReplacePx( true );
                                            break;
            case nsOrderMessageDefine::mtCancel:
                                            ///< TSE/OTC are one step order canceled process.
                                            ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osCanceled );
                                            ExecutionReport->SetExecType( nsOrderMessageDefine::etCanceled );
                                            break;
            case nsOrderMessageDefine::mtOrderStatusRequest:
                                            ExecutionReport->SetOrderStatus( nsOrderMessageDefine::osNew );
                                            ExecutionReport->SetExecType( nsOrderMessageDefine::etOrderStatus );
                                            break;
            default: break;
	}
}
//------------------------------------------------------------------------------
bool TTaifexConnection::IsTAIFEXSucceed( const UFC::AnsiString& StatusCode )
{
	int Code = StatusCode.ToInt();

	switch( Code )
	{
		case 0:  ///< OK
		case 32: ///< Reduce Qty OK, but reduce Qty > leaves Qty
		case 47: ///< OK, but some lots price exceed the dynamic limitation price will be canceled.
		case 48: ///< Order canceled and the price filed set the dynamic limitation price.
				return true;
		default:return false;
	}
	return false;
}
//------------------------------------------------------------------------------
bool TTaifexConnection::IsTWSESucceed( const UFC::AnsiString& StatusCode )
{
	int Code = StatusCode.ToInt();

	switch( Code )
	{
		case 0:  ///< OK
		case 31: ///< OK, but the confirmed sell short qty < order qty
		case 32: ///< Reduce Qty OK, but reduce Qty > leaves Qty
		case 51: ///< touch dynamic limit price, partial ok
				return true;
		default:return false;
	}
	return false;
}
//---------------------------------------------------------------------------
void TTaifexConnection::TAIFEXConfirmExecID( bool IsFut, UFC::AnsiString& Seq, const UFC::AnsiString& PBNO, int NID, int PartID, UFC::AnsiString& ExecID )
{
    Seq.PadThis( 8, '0' );
    if( IsFut == true )
    {
        if( Seq.ToInt() == 0 ) ///< Fate New
            ExecID.Printf( "FUTc%s%10d", PBNO.c_str(), NID );
        else
            ExecID.Printf( "FUTC%s%d%s", PBNO.c_str(), PartID, Seq.c_str());
    }
    else
    {
        if( Seq.ToInt() == 0 ) ///< Fate New
            ExecID.Printf( "OPTc%s%10d", PBNO.c_str(), NID );
        else
            ExecID.Printf( "OPTC%s%d%s", PBNO.c_str(), PartID, Seq.c_str());
    }
}
//------------------------------------------------------------------------------
//
// For Futures and Option
//
//------------------------------------------------------------------------------
void TTaifexConnection::ReceiveFutConfirmMessage( MTree* pTree  )
{
    nsOrderMessageDefine::MessageTypeEnum MessageType;
    UFC::AnsiString ConfirmMessage, Key, AE, ExecID, Seq, PBNO, PVC, StatusCode, ErrMsg, eTime;
    UFC::TRecord*   pRecord;
    Int32           NID,us;

    Glog->fprintf( " --------------------- TAIFEX Futures Confirm -------------------" );
    if( pTree->get( "CONFIRM_ORDER", ConfirmMessage ) &&
        pTree->get( "NID", NID )   && pTree->get( "KEY", Key ) &&
        pTree->get( "PBNO", PBNO ) && pTree->get( "PVC", PVC ) &&
        (NID != 0 ))
    {
        if( ConfirmMessage.Length() < 99 )
            ConfirmMessage.PadThis( 99, ' ' );///< Length of R020 = 99.
        MessageType = (nsOrderMessageDefine::MessageTypeEnum)( FNetworkID.GetMessageType( NID ) );
        if( UFC::AnsiString( ConfirmMessage.c_str()+ 2 , 2 ).ToInt() == 5 )
            MessageType = nsOrderMessageDefine::mtOrderStatusRequest;
        ///< Find Parser and parse the Exchange message.
	pRecord = ParseConfirmReport( nsOrderMessageDefine::mTWFutures, MessageType, ConfirmMessage );
        if( pRecord != NULL && pRecord->GetField( "StatusCode", StatusCode ) )
        {
            TExecutionReportMessage ExecutionReport;
            int Precision = 2;

            Glog->fprintf( " CONFIRM[%u][%s]", (UInt32)NID, ConfirmMessage.c_str() );
	    UpdateTAIFEXAEUDD(  pTree, Key,  AE,  ExecutionReport );
            if( pTree->get( "us", us ) == TRUE )
                ExecutionReport.SetUseus( us );
            if( pTree->get( "ETIME", eTime ) == TRUE )
                ExecutionReport.SetMessageTime( eTime.c_str() );
            ExecutionReport.SetMarket( nsOrderMessageDefine::mTWFutures );
            ExecutionReport.SetAE( AE.c_str() );
            ExecutionReport.SetNID( (UInt32)NID );
            ExecutionReport.SetSrc( ConfirmMessage );
            ExecutionReport.SetLINBRN( PBNO.c_str() );
            ExecutionReport.SetPVC( PVC.c_str() );
            UpdateTAIFEXTradingSession( pTree, ExecutionReport );
            Glog->fprintf( " StatusCode[%s] PVC[%s] PBNO[%s]", StatusCode.c_str(), PVC.c_str(), PBNO.c_str() );
            ExecutionReport.SetStatusCode( StatusCode.c_str() );            
            if( IsTAIFEXSucceed( StatusCode ) == true  ) ///< Succeed execution.
            {
                pRecord->GetField( "Seq", Seq );
                TAIFEXConfirmExecID( true, Seq, PBNO, NID, ExecutionReport.GetTMPPartID(), ExecID );
                FillExecution( MessageType, nsOrderMessageDefine::mTWFutures, nsOrderMessageDefine::tsNormal,pRecord, &ExecutionReport, Precision );
            }
            else  ///< Reject, Status code != 0
            {
                pTree->get( "MSG", ErrMsg );
                ExecID.Printf( "FUTR%s%10d", PBNO.c_str(), NID );
                FillRejectExecution( MessageType, nsOrderMessageDefine::mTWFutures, nsOrderMessageDefine::tsNormal, pRecord, StatusCode, ErrMsg, &ExecutionReport, Precision );
                Glog->fprintf( " Error[%s]", ExecutionReport.GetText() );
            }            
            UpdateTAIFEXTMPExt( pTree, ExecutionReport, Precision );
            ExecutionReport.SetExecID( ExecID.c_str() );
            
            bool isProxyAccountExist = true;
            
            if( FIsProxy && ( FAdmin == 0 ) )
            {
                UFC::AnsiString rptAccount( ExecutionReport.GetAccount() );
                if( rptAccount.Length() > 0 )
                {
                    if( FAccountSet.Exists( rptAccount ) == FALSE )
                        isProxyAccountExist = false;
                }
                else
                    isProxyAccountExist = false;
            }            
            if( isProxyAccountExist )
            {
                if( ExecutionReport.GetReportSequence( ) == 0 && ExecutionReport.GetExecType() != nsOrderMessageDefine::etOrderStatus )
                    TrigerOnExecutionReport( &ExecutionReport, edSpeedyGenerate );
                else
                    TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( nsOrderMessageDefine::mTWFutures, ExecID ));
            }
            delete pRecord;
        }
    }
}
//------------------------------------------------------------------------------
void TTaifexConnection::ReceiveOptConfirmMessage( MTree* pTree )
{
    nsOrderMessageDefine::MessageTypeEnum MessageType;
    UFC::AnsiString ConfirmMessage, Key, AE, Seq, ExecID, PBNO, PVC, StatusCode, ErrMsg, eTime;
    UFC::TRecord*   pRecord;
    Int32           NID, us;

    Glog->fprintf( " --------------------- TAIFEX Options Confirm -------------------" );
    if( pTree->get( "CONFIRM_ORDER", ConfirmMessage ) &&
        pTree->get( "NID", NID ) &&   pTree->get( "KEY", Key ) &&
        pTree->get( "PBNO", PBNO ) && pTree->get( "PVC", PVC ) &&
        (NID != 0) ) 
    {
        if( ConfirmMessage.Length() < 99 )
            ConfirmMessage.PadThis( 99, ' ' );///< Length of R020 = 99.
        MessageType = (nsOrderMessageDefine::MessageTypeEnum)( FNetworkID.GetMessageType( NID ) );
        if( UFC::AnsiString( ConfirmMessage.c_str()+ 2 , 2 ).ToInt() == 5 )
            MessageType = nsOrderMessageDefine::mtOrderStatusRequest;
        ///< Find Parser and parse the Exchange message.
        pRecord = ParseConfirmReport( nsOrderMessageDefine::mTWOptions, MessageType, ConfirmMessage );
        if( pRecord != NULL && pRecord->GetField( "StatusCode", StatusCode )  )
        {
            TExecutionReportMessage ExecutionReport;
            int Precision = 2;

            Glog->fprintf( " CONFIRM[%u][%s]", (UInt32)NID, ConfirmMessage.c_str() );
            UpdateTAIFEXAEUDD(  pTree, Key,  AE,  ExecutionReport );
            if( pTree->get( "us", us ) == TRUE )
                ExecutionReport.SetUseus( us );
            if( pTree->get( "ETIME", eTime ) == TRUE )
                ExecutionReport.SetMessageTime( eTime.c_str() );
            ExecutionReport.SetMarket( nsOrderMessageDefine::mTWOptions );
            ExecutionReport.SetAE( AE.c_str() );
            ExecutionReport.SetNID( (UInt32)NID );
            ExecutionReport.SetSrc( ConfirmMessage );
            ExecutionReport.SetLINBRN( PBNO.c_str() );
            ExecutionReport.SetPVC( PVC.c_str() );
            UpdateTAIFEXTradingSession( pTree, ExecutionReport );
            Glog->fprintf( " StatusCode[%s] PVC[%s] PBNO[%s]", StatusCode.c_str(), PVC.c_str(), PBNO.c_str() );
            ExecutionReport.SetStatusCode( StatusCode.c_str() );            
            if( IsTAIFEXSucceed( StatusCode ) == true  )///< Succeed execution.
            {
                pRecord->GetField( "Seq", Seq );
                TAIFEXConfirmExecID( false, Seq, PBNO, NID, ExecutionReport.GetTMPPartID(), ExecID );
                FillExecution( MessageType, nsOrderMessageDefine::mTWOptions, nsOrderMessageDefine::tsNormal,pRecord, &ExecutionReport, Precision );
            }
            else ///< Reject, Status code != 0
            {
                pTree->get( "MSG", ErrMsg );
                ExecID.Printf( "OPTR%s%10d", PBNO.c_str(), NID );
                FillRejectExecution( MessageType, nsOrderMessageDefine::mTWOptions, nsOrderMessageDefine::tsNormal,pRecord, StatusCode, ErrMsg, &ExecutionReport , Precision );
            }            
            UpdateTAIFEXTMPExt( pTree, ExecutionReport, Precision );
            ExecutionReport.SetExecID ( ExecID.c_str() );
                        
            bool isProxyAccountExist = true;
            if( FIsProxy && ( FAdmin == 0 ) )
            {
                UFC::AnsiString rptAccount( ExecutionReport.GetAccount() );
                if( rptAccount.Length() > 0 )
                {
                    if( FAccountSet.Exists( rptAccount ) == FALSE )
                        isProxyAccountExist = false;
                }
                else
                    isProxyAccountExist = false;
            }

            if( isProxyAccountExist )
            {
                if( ExecutionReport.GetReportSequence( ) == 0 && ExecutionReport.GetExecType() != nsOrderMessageDefine::etOrderStatus )
                    TrigerOnExecutionReport( &ExecutionReport, edSpeedyGenerate );
                else
                    TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( nsOrderMessageDefine::mTWOptions, ExecID ));
            }
            delete pRecord;
        }
    }
}
//------------------------------------------------------------------------------
//
// For TSE,OTC and ES
//
//------------------------------------------------------------------------------
void  TTaifexConnection::ReceiveTSEConfirmMessage( MTree* pTree )
{
    nsOrderMessageDefine::MessageTypeEnum MessageType;
    UFC::AnsiString ConfirmMessage, Key, AE, Data, OID, Func, ExecID, PBNO, PVC, StatusCode, ExchangeCode, TSEObjectBroker, TSEOrderType, TradeKind, PHost, CDKey, Value, eTime, orderTime = "";
    UFC::TRecord*   pRecord;
    nsOrderMessageDefine::TradingSessionIDEnum   TradeSession;
    Int32           NID, OrigNID, AfterQty;

    Glog->fprintf( " --------------------------- TSE Confirm ------------------------" );
    if( pTree->get( "CONFIRM_ORDER", ConfirmMessage ) &&
        pTree->get( "NID", NID ) &&
        pTree->get( "KEY", Key ) &&
        pTree->get( "PBNO", PBNO ) &&
        pTree->get( "PVC", PVC ) &&
        (NID != 0) )
    {
	MessageType = (nsOrderMessageDefine::MessageTypeEnum)( FNetworkID.GetMessageType( NID ) );
	TradeSession = GetMessageTradeingSession( ConfirmMessage );
	pRecord = ParseConfirmReport( nsOrderMessageDefine::mTSE, MessageType, ConfirmMessage );
	if( (pRecord != NULL) && pRecord->GetField( "OrderID", OID ) && pRecord->GetField( "FunctionCode", Func ) && pRecord->GetField( "AfterQty", Value ) )
        {
            TExecutionReportMessage ExecutionReport;
            int Precision = (FIsTWSENewVersion == true )? 4 : 2;

            Glog->fprintf( " CONFIRM[%u][%s]", (UInt32)NID, ConfirmMessage.c_str() );
            if( pTree->get( "CKEY", CDKey ) == TRUE )
            {
                if( ParseUserID( CDKey, AE, Data ) )
                    ExecutionReport.SetAllUserData( Data.c_str());
                Glog->fprintf( " CKEY[%s]", CDKey.c_str() );
            }
            else
            {
                if( ParseUserID( Key, AE, Data ) )
                    ExecutionReport.SetAllUserData( Data.c_str());
                Glog->fprintf( " KEY[%s]", Key.c_str() );
            }
            if( pTree->get( "ONID", OrigNID ) == TRUE )
                ExecutionReport.SetOrigNID( OrigNID );
            ExecutionReport.SetAE( AE.c_str() );
            ExecutionReport.SetNID( (UInt32)NID );
            ExecutionReport.SetMarket( nsOrderMessageDefine::mTSE );
            ExecutionReport.SetSrc( ConfirmMessage );
            ExecutionReport.SetLINBRN( PBNO.c_str() );
            ExecutionReport.SetTradingSessionID( GetMessageTradeingSession(ConfirmMessage) );
            ExecutionReport.SetPVC( PVC.c_str() );
            if( pTree->get( "ETIME", eTime ) == TRUE )
                ExecutionReport.SetMessageTime( eTime.c_str() );
            if( pTree->get( "PHOST", PHost ) )
				ExecutionReport.SetProcessHost( PHost );
            if( pRecord->GetField( "TSEOrderType", TSEOrderType ) )
                ExecutionReport.SetTSEOrderType( TSEOrderType.c_str() );
            else
                ExecutionReport.SetTSEOrderType( "0" );
            if( pRecord->GetField( "ObjectBroker", TSEObjectBroker ) )
                ExecutionReport.SetTSEObjectBroker( TSEObjectBroker.c_str() );
            else
                ExecutionReport.SetTSEObjectBroker( "0" );
            if( pRecord->GetField( "ExchangeCode", ExchangeCode ) )
                ExecutionReport.SetTSEExchangeCode( ExchangeCode.c_str() );
            else
                ExecutionReport.SetTSEExchangeCode( "0" );
            if( pRecord->GetField( "TradeKind", TradeKind ) )
                ExecutionReport.SetTSETradeKind( TradeKind.c_str() );
            else
                ExecutionReport.SetTSETradeKind( "0" );
            SetExecOrderType( pRecord, &ExecutionReport );
            SetExecTimeInForce( nsOrderMessageDefine::mTSE, pRecord, &ExecutionReport );
            if( pRecord->GetField( "StatusCode", StatusCode ) )
            {
                Glog->fprintf( " StatusCode[%s] PVC[%s] PBNO[%s]", StatusCode.c_str(), PVC.c_str(), PBNO.c_str() );
                ExecutionReport.SetStatusCode( StatusCode.c_str() );
                if( IsTWSESucceed( StatusCode ) == true )
                {
                    AfterQty = Value.ToInt();
                    if( !pRecord->GetField( "OrderTime", orderTime ) ) orderTime = "";
			FillExecution( MessageType, nsOrderMessageDefine::mTSE, TradeSession, pRecord, &ExecutionReport, Precision );
                    if( Func[0] == '6' )
                    {
                         ExecID.Printf( "TSEC%s%s%s%06d%010u", ExecutionReport.GetBrokerID(), OID.c_str(), Func.c_str(), AfterQty, NID );   
                    }
                    else
                    {
                        if( orderTime.Length() > 0 )
                            ExecID.Printf( "TSEC%s%s%s%06d%s", ExecutionReport.GetBrokerID(), OID.c_str(), Func.c_str(), AfterQty, orderTime.c_str() );
                        else
                            ExecID.Printf( "TSEC%s%s%s%06d", ExecutionReport.GetBrokerID(), OID.c_str(), Func.c_str(), AfterQty );
                    }
                }
                else ///< Reject, Status code != 0
                {
                    UFC::AnsiString ErrMsg( "" );
                    pTree->get( "MSG",ErrMsg );
                    FillRejectExecution( MessageType,nsOrderMessageDefine::mTSE, TradeSession, pRecord, StatusCode, ErrMsg, &ExecutionReport, Precision );
                    ExecID.Printf( "TSECR%s%010d", OID.c_str(), NID );
                }
                ExecutionReport.SetExecID( ExecID.c_str() );
                
                bool isProxyAccountExist = true;
                if( FIsProxy && ( FAdmin == 0 ) )
                {
                    UFC::AnsiString rptAccount( ExecutionReport.GetAccount() );
                    if( rptAccount.Length() > 0 )
                    {
                        if( FAccountSet.Exists( rptAccount ) == FALSE )
                            isProxyAccountExist = false;
                    }
                    else
                        isProxyAccountExist = false;
                }
                
                if( isProxyAccountExist )
                    TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( nsOrderMessageDefine::mTSE, ExecID ) );
            }
            delete pRecord;
        }
    }
}
//------------------------------------------------------------------------------
void  TTaifexConnection::ReceiveOTCConfirmMessage( MTree* pTree )
{
    nsOrderMessageDefine::MessageTypeEnum      MessageType;
    nsOrderMessageDefine::TradingSessionIDEnum TradeSession;
    UFC::AnsiString ConfirmMessage, Key, AE, Data, OID, Func, ExecID, PBNO, PVC, StatusCode, ExchangeCode, TSEObjectBroker, TSEOrderType, TradeKind, PHost,CDKey, Value, eTime, orderTime = "";
    UFC::TRecord*   pRecord;
    Int32           NID, OrigNID, AfterQty;

    Glog->fprintf( " --------------------------- OTC Confirm ------------------------" );
    if( pTree->get( "CONFIRM_ORDER", ConfirmMessage ) &&
        pTree->get( "NID", NID ) &&
        pTree->get( "KEY", Key ) &&
        pTree->get( "PBNO", PBNO ) &&
        pTree->get( "PVC", PVC ) &&
        (NID != 0) )
    {
        MessageType = (nsOrderMessageDefine::MessageTypeEnum)( FNetworkID.GetMessageType( NID ) );
        TradeSession = GetMessageTradeingSession( ConfirmMessage );
        if( TradeSession == nsOrderMessageDefine::tsNegotiatePx )
        {
            TExecutionReportMessage ExecutionReport;

            ExecID.Printf( "OTCC%s%10d", PBNO.c_str(), NID);
            ExecutionReport.SetNID( (UInt32)NID );
            ExecutionReport.SetExecID( ExecID.c_str() );
            ExecutionReport.SetMarket( nsOrderMessageDefine::mOTC );
            ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osNew );
            ExecutionReport.SetSrc( ConfirmMessage );
            ExecutionReport.SetLINBRN( PBNO.c_str() );
            ExecutionReport.SetTradingSessionID( nsOrderMessageDefine::tsNegotiatePx );
            ExecutionReport.SetPVC( PVC.c_str() );
            if( pTree->get( "PHOST", PHost ) )
                ExecutionReport.SetProcessHost( PHost );
            if( ParseUserID( Key, AE, Data ) )
                ExecutionReport.SetAllUserData( Data.c_str());
            ExecutionReport.SetAE( AE.c_str() );
            TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( nsOrderMessageDefine::mOTC, ExecID ));
        }
        else
        {
            pRecord = ParseConfirmReport( nsOrderMessageDefine::mOTC, MessageType, ConfirmMessage );
            if( pRecord != NULL && pRecord->GetField( "OrderID", OID ) && pRecord->GetField( "FunctionCode", Func ) && pRecord->GetField( "AfterQty", Value ) )
            {
                TExecutionReportMessage ExecutionReport;
                int Precision = (FIsTWSENewVersion == true )? 4 : 2;

                Glog->fprintf( " CONFIRM[%u][%s]", (UInt32)NID, ConfirmMessage.c_str() );
                if( pTree->get( "CKEY", CDKey ) == TRUE )
                {
                    if( ParseUserID( CDKey, AE, Data ) )
                        ExecutionReport.SetAllUserData( Data.c_str());
                    Glog->fprintf( " CKEY[%s]", CDKey.c_str() );
                }
                else
                {
                    if( ParseUserID( Key, AE, Data ) )
                        ExecutionReport.SetAllUserData( Data.c_str());
                    Glog->fprintf( " KEY[%s]", Key.c_str() );
                }
                if( pTree->get( "ONID", OrigNID ) == TRUE )
                    ExecutionReport.SetOrigNID( OrigNID );
                ExecutionReport.SetAE( AE.c_str() );
                ExecutionReport.SetNID( (UInt32)NID );
                ExecutionReport.SetMarket( nsOrderMessageDefine::mOTC );
                ExecutionReport.SetSrc( ConfirmMessage );
                ExecutionReport.SetLINBRN( PBNO.c_str() );
                ExecutionReport.SetTradingSessionID( TradeSession );
                ExecutionReport.SetPVC( PVC.c_str() );
                if( pTree->get( "ETIME", eTime ) == TRUE )
                    ExecutionReport.SetMessageTime( eTime.c_str() );
                if( pTree->get( "PHOST", PHost ) )
                    ExecutionReport.SetProcessHost( PHost );
                if( pRecord->GetField( "TSEOrderType", TSEOrderType ) )
                    ExecutionReport.SetTSEOrderType( TSEOrderType.c_str() );
                else
                    ExecutionReport.SetTSEOrderType( "0" );
                if( pRecord->GetField( "ObjectBroker", TSEObjectBroker ) )
                    ExecutionReport.SetTSEObjectBroker( TSEObjectBroker.c_str() );
                else
                    ExecutionReport.SetTSEObjectBroker( "0" );
                if( pRecord->GetField( "ExchangeCode", ExchangeCode ) )
                    ExecutionReport.SetTSEExchangeCode( ExchangeCode.c_str() );
                else
					ExecutionReport.SetTSETradeKind( "0" );
                if( pRecord->GetField( "TradeKind", TradeKind ) )
                    ExecutionReport.SetTSETradeKind( TradeKind.c_str() );
                else
                    ExecutionReport.SetTSETradeKind( "0" );
                SetExecOrderType( pRecord, &ExecutionReport );
                SetExecTimeInForce( nsOrderMessageDefine::mOTC, pRecord, &ExecutionReport );
                if( pRecord->GetField( "StatusCode", StatusCode ) )
                {
                    Glog->fprintf( " StatusCode[%s] PVC[%s] PBNO[%s]", StatusCode.c_str(), PVC.c_str(), PBNO.c_str() );
                    ExecutionReport.SetStatusCode( StatusCode.c_str() );
                    if( IsTWSESucceed( StatusCode ) == true )
                    {
                        AfterQty = Value.ToInt();
                        if( !pRecord->GetField( "OrderTime", orderTime ) ) orderTime = "";
                        FillExecution( MessageType, nsOrderMessageDefine::mOTC, TradeSession, pRecord, &ExecutionReport, Precision );
                        if( orderTime.Length() > 0 )
                            ExecID.Printf( "OTCC%s%s%s%06d%s", ExecutionReport.GetBrokerID(), OID.c_str(), Func.c_str(), AfterQty, orderTime.c_str() );
                        else
                            ExecID.Printf( "OTCC%s%s%s%06d", ExecutionReport.GetBrokerID(), OID.c_str(), Func.c_str(), AfterQty );
                    }            
                    else ///< Reject, Status code != 0
                    {
                        UFC::AnsiString ErrMsg( "" );
                        pTree->get( "MSG",ErrMsg );
                        FillRejectExecution( MessageType, nsOrderMessageDefine::mOTC, TradeSession, pRecord, StatusCode, ErrMsg,&ExecutionReport, Precision );
                        ExecID.Printf( "OTCCR%s%010d", OID.c_str(), NID );
                    }
                    ExecutionReport.SetExecID( ExecID.c_str() );
                                        
                    bool isProxyAccountExist = true;
                    if( FIsProxy && ( FAdmin == 0 ) )
                    {
                        UFC::AnsiString rptAccount( ExecutionReport.GetAccount() );
                        if( rptAccount.Length() > 0 )
                        {
                            if( FAccountSet.Exists( rptAccount ) == FALSE )
                                isProxyAccountExist = false;
                        }
                        else
                            isProxyAccountExist = false;
                    }
                        
                    if( isProxyAccountExist )
                        TrigerOnExecutionReport( &ExecutionReport, (ExecDup) IsExecutionDup( nsOrderMessageDefine::mOTC, ExecID ));
                }
                delete pRecord;
            }
        }
    }
}
//------------------------------------------------------------------------------
void  TTaifexConnection::ReceiveESConfirmMessage( MTree* pTree )
{
    nsOrderMessageDefine::MessageTypeEnum MessageType;
    UFC::AnsiString ConfirmMessage, Key, AE, Data, OID, Func, ExecID, PBNO, PVC, StatusCode, ExchangeCode, TSEObjectBroker, TSEOrderType,TradeKind, PHost,CDKey, Value,eTime;
    UFC::TRecord*   pRecord;
    nsOrderMessageDefine::TradingSessionIDEnum   TradeSession;
    Int32           NID, AfterQty;

    Glog->fprintf( " --------------------------- ES Confirm ------------------------" );
    if( pTree->get( "CONFIRM_ORDER", ConfirmMessage ) &&
        pTree->get( "NID", NID ) &&
        pTree->get( "KEY", Key ) &&
        pTree->get( "PBNO", PBNO ) &&
        pTree->get( "PVC", PVC ) &&
        (NID != 0) )
    {
        MessageType = (nsOrderMessageDefine::MessageTypeEnum)( FNetworkID.GetMessageType( NID ) );
        TradeSession = GetMessageTradeingSession( ConfirmMessage );
        pRecord = ParseConfirmReport( nsOrderMessageDefine::mES, MessageType, ConfirmMessage );
        if( (pRecord != NULL) && pRecord->GetField( "OrderID", OID ) && pRecord->GetField( "FunctionCode", Func ) && pRecord->GetField( "AfterQty", Value ) )
        {
            TExecutionReportMessage ExecutionReport;
            int Precision = (FIsTWSENewVersion == true )? 4 : 2;

            Glog->fprintf( " CONFIRM[%u][%s]", (UInt32)NID, ConfirmMessage.c_str() );
            if( pTree->get( "CKEY", CDKey ) == TRUE )
            {
                if( ParseUserID( CDKey, AE, Data ) )
                    ExecutionReport.SetAllUserData( Data.c_str());
                Glog->fprintf( " CKEY[%s]", CDKey.c_str() );
            }
            else
            {
                if( ParseUserID( Key, AE, Data ) )
                    ExecutionReport.SetAllUserData( Data.c_str());
                Glog->fprintf( " KEY[%s]", Key.c_str() );
            }
            ExecutionReport.SetAE( AE.c_str() );
            Glog->fprintf( " AE[%s] UserData[%s]", ExecutionReport.GetAE(), ExecutionReport.GetAllUserData() );
            ExecutionReport.SetNID( (UInt32)NID );
            ExecutionReport.SetMarket( nsOrderMessageDefine::mES );
            ExecutionReport.SetSrc( ConfirmMessage );
            ExecutionReport.SetLINBRN( PBNO.c_str() );
            ExecutionReport.SetTradingSessionID( GetMessageTradeingSession(ConfirmMessage) );
            ExecutionReport.SetPVC( PVC.c_str() );
            if( pTree->get( "ETIME", eTime ) == TRUE )
                ExecutionReport.SetMessageTime( eTime.c_str() );
            if( pTree->get( "PHOST", PHost ) )
                                ExecutionReport.SetProcessHost( PHost );
            if( pRecord->GetField( "TSEOrderType", TSEOrderType ) )
                ExecutionReport.SetTSEOrderType( TSEOrderType.c_str() );
            else
                ExecutionReport.SetTSEOrderType( "0" );
            if( pRecord->GetField( "ObjectBroker", TSEObjectBroker ) )
                ExecutionReport.SetTSEObjectBroker( TSEObjectBroker.c_str() );
            else
                ExecutionReport.SetTSEObjectBroker( "0" );
            if( pRecord->GetField( "ExchangeCode", ExchangeCode ) )
                ExecutionReport.SetTSEExchangeCode( ExchangeCode.c_str() );
            else
                ExecutionReport.SetTSEExchangeCode( "0" );
            if( pRecord->GetField( "TradeKind", TradeKind ) )
                ExecutionReport.SetTSETradeKind( TradeKind.c_str() );
            else
                ExecutionReport.SetTSETradeKind( "0" );
            SetExecOrderType( pRecord, &ExecutionReport );
            SetExecTimeInForce( nsOrderMessageDefine::mES, pRecord, &ExecutionReport );
            if( pRecord->GetField( "StatusCode", StatusCode ) )
            {
                Glog->fprintf( " StatusCode[%s] PVC[%s] PBNO[%s]", StatusCode.c_str(), PVC.c_str(), PBNO.c_str() );
                ExecutionReport.SetStatusCode( StatusCode.c_str() );
                if( IsTWSESucceed( StatusCode ) == true )
                {
                    AfterQty = Value.ToInt();
                    FillExecution( MessageType, nsOrderMessageDefine::mES, TradeSession, pRecord, &ExecutionReport, Precision );
                    ExecID.Printf( "ESC%s%s%s%03d", ExecutionReport.GetBrokerID(), OID.c_str(), Func.c_str(), AfterQty );
                }
                else ///< Reject, Status code != 0
                {
                    UFC::AnsiString ErrMsg( "" );
                    pTree->get( "MSG",ErrMsg );
                    FillRejectExecution( MessageType,nsOrderMessageDefine::mES, TradeSession, pRecord, StatusCode, ErrMsg, &ExecutionReport, Precision );
                    ExecID.Printf( "ESCR%s%010d", OID.c_str(), NID );
                }
                Glog->fprintf( " %s() StatusCode[%s] ExecID[%s]", __func__, StatusCode.c_str(), ExecID.c_str() );
                ExecutionReport.SetExecID( ExecID.c_str() );
                TrigerOnExecutionReport( &ExecutionReport, (ExecDup)IsExecutionDup( nsOrderMessageDefine::mES, ExecID ) );
            }
            delete pRecord;
        }
    }
}
//------------------------------------------------------------------------------





