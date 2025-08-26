//---------------------------------------------------------------------------
// Unit1.cpp
//---------------------------------------------------------------------------
#include "TSCTFXOrder.h"
#include <float.h>
//---------------------------------------------------------------------------
const char* MARKET_MAKER_ACCOUNT = "8888888";
const char  SOURCE_CODE = 'F'; ///< From FIX
//------------------------------------------------------------------------------
// For construct a Order object
//---------------------------------------------------------------------------
TSCTFXOrderRecord::TSCTFXOrderRecord( ClientOrderID&                      CNID,
									  UFC::AnsiString&                    OrderID,
									  FIX::ClOrdID&                       ClOrdID,
									  FIX::Account&                       Account,
                                      UFC::AnsiString&                    SubAccount,
									  FIX::SecurityID&                    SecurityID,
									  FIX::Symbol&                        Symbol,
									  FIX::OrderQty&                      OrderQty,
									  FIX::Price&                         Price,
									  FIX::Side&                          Side,
									  FIX::TimeInForce&                   TimeInForce,
									  UFC::AnsiString&                    Sender)
:OrderRecord( CNID,OrderID,ClOrdID,Symbol,OrderQty,Price,Side,TimeInForce,Sender)
,FAccount( Account.getString().c_str() )
,FSubAccount( SubAccount )
,FSecurityID( SecurityID.getString().c_str() )
{
    FSubAccount.PadThis( 4, '0' );
    FAccount.PadThis( 7, '0' );
	GetYYYYMMDD( FTradeDate );
}
//------------------------------------------------------------------------------
// For construct a quote object
//------------------------------------------------------------------------------
TSCTFXOrderRecord::TSCTFXOrderRecord( ClientOrderID&                      CNID,
									  UFC::AnsiString&                    OrderID,
									  FIX::QuoteID&                       QuoteID,
									  FIX::Account&                       Account,
                                      UFC::AnsiString&                    SubAccount,
									  FIX::SecurityID&                    SecurityID,
									  FIX::Symbol&                        Symbol,
									  FIX::BidPx&                         BidPx,
									  FIX::BidSize&                       BidSize,
									  FIX::OfferPx&                       OfferPx,
									  FIX::OfferSize&                     OfferSize,
									  UFC::AnsiString&                    Sender)
:OrderRecord( CNID,OrderID,QuoteID,Symbol,BidPx,BidSize,OfferPx,OfferSize,Sender)
,FAccount( Account.getString().c_str() )
,FSubAccount( SubAccount )
,FSecurityID( SecurityID.getString().c_str() )
{
    FSubAccount.PadThis( 4, '0' );
    FAccount.PadThis( 7, '0' );
	GetYYYYMMDD( FTradeDate );
}
//------------------------------------------------------------------------------
void TSCTFXOrderRecord::SaveToBuffer( UFC::MemoryStream& Stream )
{
    FAccount.SaveToStream( &Stream );
    FSubAccount.SaveToStream( &Stream );
	FSecurityID.SaveToStream( &Stream );
}
//------------------------------------------------------------------------------
void TSCTFXOrderRecord::LoadFromBuffer( UFC::MemoryStream& Stream )
{
    FAccount.LoadFromStream( &Stream );
    FSubAccount.LoadFromStream( &Stream );
    FSecurityID.LoadFromStream( &Stream );
	GetYYYYMMDD( FTradeDate );
}
//------------------------------------------------------------------------------
char TSCTFXOrderRecord::GetOrderCondition( void )
{
    if( FTimeInForce == FIX::TimeInForce_DAY )
        return 'R'; ///< ROD Order
    else if( FTimeInForce == FIX::TimeInForce_FILL_OR_KILL )
        return 'F'; ///< FOK Order
    else if( FTimeInForce == FIX::TimeInForce_IMMEDIATE_OR_CANCEL )
        return 'I'; ///< IOC Order
	return 'R';
}
//------------------------------------------------------------------------------
void TSCTFXOrderRecord::FillPendingNew( FIX44::ExecutionReport& report )
{
    report = FIX44::ExecutionReport( FIX::OrderID( FOrderID.c_str() ),
                                     FIX::ExecID( FIXMapperApp::GenerateExecID().c_str()),
                                     FIX::ExecType_PENDING_NEW,
                                     FIX::OrdStatus_PENDING_NEW,
                                     FSide,
                                     FIX::LeavesQty( FQty ),
                                     FIX::CumQty( 0 ),
                                     FIX::AvgPx( 0 ));
    report.set( FIX::OrderQty( FQty ) );
    report.set( FIX::OrdType( (FIsMarketOrder==TRUE) ? FIX::OrdType_MARKET : FIX::OrdType_LIMIT ));
	report.set( FIX::SecurityID( FSecurityID.c_str() ) );
    report.set( FIX::Symbol( FSymbol.c_str()) );
    report.set( FIX::ClOrdID( FClOrdID.c_str()) );
	report.set( FIX::Account( FAccount.c_str()) );
	report.set( FIX::SecurityID( FSecurityID.c_str()) );
}
//------------------------------------------------------------------------------
void TSCTFXOrderRecord::NewOrderString( UFC::AnsiString& Order, AccountAttribute* AccountAttr )
{
	if( FMarket == mkFutures )
    {
     	UFC::AnsiString TFXSymbol    = FSecurityID.SubString( 0, 7 );
        UFC::AnsiString MaturityDate = FSecurityID.SubString( 7, 6);

		Order.Printf("11%s%08d%s%s%s0%s%s00000000%c%c%08d%04d%c%s%s%c%s", FSubAccount.c_str(),
                                                                          FNID,
                                                                          AccountAttr->GetBrokerID().c_str(),
                                                                          "     ",
                                                                          FAccount.c_str(),
                                                                          TFXSymbol.c_str(),
                                                                          MaturityDate.c_str(),
                                                                          (FSide == FIX::Side_BUY)?'B':'S',
                                                                          (FIsMarketOrder == TRUE )?'1':'2',
                                                                          TFXSymbolUtility.ToIntPrice( TFXSymbol, FPrice ),
                                                                          FQty,
                                                                          '9',///< 9 auto open/close
                                                                          FIXMapper::CMID.c_str(),
                                                                          AccountAttr->GetAccountWebID().c_str(),
                                                                          SOURCE_CODE,
                                                                          FTradeDate.c_str());
    }
    else
    {
        char TFXSymbol[21] = "                    ";
        memcpy( TFXSymbol, FSecurityID.c_str(), FSecurityID.Length());
        Order.Printf("21%s%08d%s%s%s0%s%c%c%09d%04d%c%c%s%s%c%s",FSubAccount.c_str(),
                                                                 FNID,
                                                                 AccountAttr->GetBrokerID().c_str(),
                                                                 "     ",
                                                                 FAccount.c_str(),
                                                                 TFXSymbol,
                                                                 (FSide == FIX::Side_BUY)?'B':'S',
                                                                 (FIsMarketOrder == TRUE )?'M':'L',
                                                                 TFXSymbolUtility.ToIntPrice( TFXSymbol, FPrice ),
                                                                 FQty,
                                                                 GetOrderCondition(),
                                                                 ' ', ///< ' ' for auto open/close
                                                                 FIXMapper::CMID.c_str(),
                                                                 AccountAttr->GetAccountWebID().c_str(),
                                                                 SOURCE_CODE,
                                                                 FTradeDate.c_str());
    }
}

//------------------------------------------------------------------------------
void TSCTFXOrderRecord::CancelString( Int32 NID, UFC::AnsiString& Cancel, AccountAttribute* AccountAttr )
{
    if(  FMarket == mkFutures )
    {
		UFC::AnsiString TFXSymbol    = FSecurityID.SubString( 0, 7 );
        UFC::AnsiString MaturityDate = FSecurityID.SubString( 7, 6 );

		Cancel.Printf("13%s%08d%s%s%s0%s%s00000000%c%c%08d%04d9%s%s%c%s", FSubAccount.c_str(),
                                                                          NID,
                                                                          AccountAttr->GetBrokerID().c_str(),
                                                                          FOrderID.c_str(),
                                                                          FAccount.c_str(),
                                                                          TFXSymbol.c_str(),
                                                                          MaturityDate.c_str(),
                                                                          (FSide == FIX::Side_BUY)?'B':'S',
                                                                          (FIsMarketOrder == TRUE )?'1':'2',
                                                                          TFXSymbolUtility.ToIntPrice( TFXSymbol, FPrice ),
                                                                          FQty,
                                                                          FIXMapper::CMID.c_str(),
                                                                          AccountAttr->GetAccountWebID().c_str(),
                                                                          SOURCE_CODE,
                                                                          FTradeDate.c_str());
    }
    else
    {
        char TFXSymbol[21] = "                    ";

        memcpy( TFXSymbol, FSecurityID.c_str(), FSecurityID.Length());
		Cancel.Printf("23%s%08d%s%s%s0%s%c%c%09d%04d%c0%s%s%c%s",FSubAccount.c_str(),
                                                                 NID,
                                                                 AccountAttr->GetBrokerID().c_str(),
                                                                 FOrderID.c_str(),
                                                                 FAccount.c_str(),
                                                                 TFXSymbol,
                                                                 (FSide == FIX::Side_BUY)?'B':'S',
                                                                 (FIsMarketOrder == TRUE )?'M':'L',
                                                                 TFXSymbolUtility.ToIntPrice( TFXSymbol, FPrice ),
                                                                 FQty,
                                                                 GetOrderCondition(),
                                                                 FIXMapper::CMID.c_str(),
                                                                 AccountAttr->GetAccountWebID().c_str(),
                                                                 SOURCE_CODE,
                                                                 FTradeDate.c_str());
    }
}
//------------------------------------------------------------------------------
void TSCTFXOrderRecord::ReduceString( Int32 NID, Int32 NewQty, UFC::AnsiString& Reduce, AccountAttribute* AccountAttr )
{
    if( FMarket == mkFutures )
    {
        UFC::AnsiString TFXSymbol    = FSecurityID.SubString( 0, 7 );
        UFC::AnsiString MaturityDate = FSecurityID.SubString( 7, 6);

        Reduce.Printf("12%s%08d%s%s%s0%s%s00000000%c%c%08d%04d9%s%s%c%s", FSubAccount.c_str(),
                                                                          NID,
                                                                          AccountAttr->GetBrokerID().c_str(),
                                                                          FOrderID.c_str(),
                                                                          FAccount.c_str(),
                                                                          TFXSymbol.c_str(),
                                                                          MaturityDate.c_str(),
                                                                          (FSide == FIX::Side_BUY)?'B':'S',
                                                                          (FIsMarketOrder == TRUE )?'1':'2',
                                                                          TFXSymbolUtility.ToIntPrice( TFXSymbol, FPrice ),
                                                                          GetOrderQty() - NewQty,
                                                                          FIXMapper::CMID.c_str(),
                                                                          AccountAttr->GetAccountWebID().c_str(),
                                                                          SOURCE_CODE,
                                                                          FTradeDate.c_str());
    }
    else
    {
        char TFXSymbol[21] = "                    ";

        memcpy( TFXSymbol, FSecurityID.c_str(), FSecurityID.Length());
		Reduce.Printf("22%s%08d%s%s%s0%s%c%c%09d%04d%c0%s%s%c%s",FSubAccount.c_str(),
                                                                 NID,
                                                                 AccountAttr->GetBrokerID().c_str(),
                                                                 FOrderID.c_str(),
                                                                 FAccount.c_str(),
                                                                 TFXSymbol,
                                                                 (FSide == FIX::Side_BUY)?'B':'S',
                                                                 (FIsMarketOrder == TRUE )?'M':'L',
                                                                 TFXSymbolUtility.ToIntPrice( TFXSymbol, FPrice ),
                                                                 GetOrderQty() - NewQty,
                                                                 GetOrderCondition(),
                                                                 FIXMapper::CMID.c_str(),
                                                                 AccountAttr->GetAccountWebID().c_str(),
                                                                 SOURCE_CODE,
                                                                 FTradeDate.c_str());
    }
}
//------------------------------------------------------------------------------
void TSCTFXOrderRecord::NewQuoteString( UFC::AnsiString& NewQuote )
{
    char QuoteType = FIsQuoteResponding? 'Q':'R';
    char TFXSymbol[21] = "                    ";

    memcpy( TFXSymbol, FSecurityID.c_str(), FSecurityID.Length());
    NewQuote.Printf("41%s%08d%s%s%s0%s%09d%09d%04d%04d%c%c%s%s%c%s", FSubAccount.c_str(),
                                                                     FNID,
                                                                     FIXMapper::BrokerID.c_str(),
                                                                     "00000",
                                                                     MARKET_MAKER_ACCOUNT,
                                                                     TFXSymbol,
                                                                     TFXSymbolUtility.ToIntPrice( TFXSymbol, FBidPrice ),
                                                                     TFXSymbolUtility.ToIntPrice( TFXSymbol, FPrice ),
                                                                     FBidQty,
                                                                     FQty,
                                                                     QuoteType,
                                                                     '9', ///< '9' for Quote open/close flag
                                                                     FIXMapper::CMID.c_str(),
                                                                     FIXMapper::WebID.c_str(),
                                                                     SOURCE_CODE,
                                                                     FTradeDate.c_str());
}
//------------------------------------------------------------------------------
void TSCTFXOrderRecord::CancelQuoteString( Int32 NID,UFC::AnsiString& CancelQuote )
{   ///< generate a TAIFEX R090 Cancel Quote message
	char TFXSymbol[21] = "                    ";

    memcpy( TFXSymbol, FSecurityID.c_str(), FSecurityID.Length());
    CancelQuote.Printf("43%s%08d%s%s%s0%s%09d%09d%04d%04d%c%c%s%s%c%s",FSubAccount.c_str(),
                                                                       NID,
                                                                       FIXMapper::BrokerID.c_str(),
                                                                       FOrderID.c_str(),
                                                                       MARKET_MAKER_ACCOUNT,
                                                                       TFXSymbol,
                                                                       TFXSymbolUtility.ToIntPrice( TFXSymbol, FBidPrice ),
                                                                       TFXSymbolUtility.ToIntPrice( TFXSymbol, FPrice ),
                                                                       FBidQty,
                                                                       FQty,
                                                                       'R', ///< ROD Quote
                                                                       '9', ///< '9' for Quote open/close flag
                                                                       FIXMapper::CMID.c_str(),
                                                                       FIXMapper::WebID.c_str(),
                                                                       SOURCE_CODE,
                                                                       FTradeDate.c_str());
}
//------------------------------------------------------------------------------
void TSCTFXConfirmParser::Parse( MarketKind Market, const UFC::AnsiString& Confirm )
{
	  ///< TSC system has 2 bytes header.
      if( Confirm[1] == '1' ) ///< TAIFEX Accept this order.
      {
          FErrorCode = 0;
          FErrorMsg  = "OK";
          switch( Confirm[0] )
          {
              case '1':///< Futures Order
                       FOrderID = Confirm.SubString( 21 + 2, 5 );
                       switch( Confirm[5] )
                       {
                          case '1':FTransKind = tkNewOrder;
                                   FQty       = UFC::AnsiString( Confirm.c_str() + 65 + 2 , 4 ).ToInt();
                                   break;
                          case '2':FTransKind = tkOrderReplace;
                                   FQty       = Confirm.SubString( 74 + 2 , 4 ).ToInt() - Confirm.SubString( 78 + 2 , 4 ).ToInt();
                                   break;
                          case '3':FTransKind = tkOrderCancel;
                                   FQty       = 0;
                                   break;
                       }
                       break;
              case '2':///< Options Order
                       if( FIXMapper::UseSimTX == TRUE )
                       {
                           if( Confirm[2] == '3' )///<Is an order 300100.......
                           {
                                FOrderID     = Confirm.SubString( 21 + 2, 5 );
                                FQty         = UFC::AnsiString( Confirm.c_str() + 65 + 2 , 4 ).ToInt();
                           }
                           else if( Confirm.Length() < 50 )///<QuoteRequest length = 41
                           {
                           }
                           else ///< Options Quote
                           {
                               FOrderID     = Confirm.SubString( 7 + 2, 5 );
                               FQty         = UFC::AnsiString( Confirm.c_str() + 64 + 2 , 4 ).ToInt();
                           }
                       }
                       else
                       {
                           if( memcmp(Confirm.c_str(),"213001",6) == 0 )///<Is an order 300100.......
                           {
                               FTransKind   = tkNewOrder;
                               FOrderID     = Confirm.SubString( 21 + 2, 5 );
                               FQty         = UFC::AnsiString( Confirm.c_str() + 65 + 2 , 4 ).ToInt();
                           }
                           else if( memcmp(Confirm.c_str(),"213",3) == 0 ) ///<Is an order Cancel/Replace
                           {
                               FOrderID   = Confirm.SubString( 10 + 2, 5 );
                               FErrorCode = Confirm.SubString( 1 + 2, 2 ).ToInt();
                               switch( Confirm[ 48 + 2 ] )
                               {
                                  case '3':FTransKind = tkOrderCancel;///< Cancel
                                           FQty       = 0;
                                           break;
                                  case '2':FTransKind = tkOrderReplace;///< Replace
                                           FQty       = Confirm.SubString( 49 + 2, 4 ).ToInt() - Confirm.SubString( 53 + 2, 4 ).ToInt();
                                           if( FErrorCode == 32 )///< reduce qty > leaves Qty.
                                               FErrorCode = 0;
                                           break;
                               }
                           }
                       }
                       break;
             case '3': FTransKind = tkQuoteRequest;///< QuoteRequest length = 41
                       FQty       = 0;
                       FOrderID   = "     ";
                       break;
             case '4': if( memcmp( Confirm.c_str(), "414", 3 ) == 0 ) ///<Is an Quote cancel.
                       {
                           FTransKind = tkQuoteCancel;
                           FQty       = 0;
                           FOrderID   = Confirm.SubString( 10 + 2, 5 );
                           FErrorCode = Confirm.SubString( 1 + 2, 2 ).ToInt();
                       }
                       else if( memcmp( Confirm.c_str(), "41", 2 ) == 0 )///< Options Quote confirm.
                       {
                           FTransKind   = tkNewQuote;
                           FOrderID     = Confirm.SubString( 7 + 2, 5 );
                           FQty         = UFC::AnsiString( Confirm.c_str() + 64 + 2 , 4 ).ToInt();
                       }
                       break;
          }
      }
      else ///< Reject
      {
          FOrderID     = "-----";
          FQty         = 0;
          if( Confirm.Length() > 14 + 2 )
              FErrorCode   = Confirm.SubString( 12 + 2, 2 ).ToInt();
          else
              FErrorCode   = 99; ///< Invalidate confirm
          FErrorMsg = Confirm.SubString( Confirm.Length() - 7, 7 );
          UFC::BufferedLog::DebugPrintf(" Error Message = [%s]", FErrorMsg.c_str() );
      }
}
//------------------------------------------------------------------------------
BOOL TSCTFXExecutionParser::Parse( MarketKind Market, const UFC::AnsiString& Execution )
{
      int   TSC_OFFSET;

      if( FIXMapper::UseSimTX == TRUE )
          TSC_OFFSET = 0;
      else
          TSC_OFFSET = 2;

      UFC::AnsiString WebID;
      const char*     DataStr = Execution.c_str();
      char            Type = Execution[ 0 + TSC_OFFSET ];

      if( Market == mkFutures )
      {
          UFC::BufferedLog::DebugPrintf( " FUT Execution:%s", Execution.c_str() );
          if( FIXMapper::UseSimTX == TRUE )
          {
              if( Type == '1' ) ///< Filled
              {
                  FExecutionType = etFilledSingle;
                  FOrderID      = UFC::AnsiString( DataStr + 8  + TSC_OFFSET, 5 );
                  FExecutionSeq = UFC::AnsiString( DataStr + 69 + TSC_OFFSET, 8 ).ToInt();
                  FPrice        = UFC::AnsiString( DataStr + 44 + TSC_OFFSET, 8 ).ToInt();
                  FQty          = UFC::AnsiString( DataStr + 52 + TSC_OFFSET, 4 ).ToInt();
                  FSide         = (Execution[ 42 + TSC_OFFSET ] == 'B')?(FIX::Side_BUY):(FIX::Side_SELL);
                  FErrorCode    = 0;
                  WebID         = Execution.SubString( Execution.Length()-20,3);
                  FOrigNID      = Execution.SubString( Execution.Length()-16,8).ToInt();
              }
              else if( Type == '3' ) ///< Canceled/ Replace
              {
                  if( Execution[ 49 + TSC_OFFSET ] == '3' ) ///< Cancel
                      FExecutionType = etCanceled;
                  else if( Execution[ 49 + TSC_OFFSET ] == '2' ) ///< Replace
                      FExecutionType = etReplaced;
                  FOrderID      = UFC::AnsiString( DataStr + 10 + TSC_OFFSET, 5 );
                  FExecutionSeq = UFC::AnsiString( DataStr + 66 + TSC_OFFSET, 8 ).ToInt();
                  FBeforeQty    = UFC::AnsiString( DataStr + 50 + TSC_OFFSET, 4 ).ToInt();
                  FAfterQty     = UFC::AnsiString( DataStr + 54 + TSC_OFFSET, 4 ).ToInt();
                  FSide         = (Execution[ 44 + TSC_OFFSET ] == 'B')?(FIX::Side_BUY):(FIX::Side_SELL);
                  FErrorCode    = UFC::AnsiString( DataStr + 1  + TSC_OFFSET, 2 ).ToInt();
                  WebID         = Execution.SubString( Execution.Length()-20,3);
                  FOrigNID      = Execution.SubString( Execution.Length()-16,8).ToInt();
              }
          }
          else
          {
              FOrigNID = 0;
              FExecutionType = etFilledSingle;
              FOrderID      = UFC::AnsiString( DataStr + 7  + TSC_OFFSET, 5 );
              FExecutionSeq = UFC::AnsiString( DataStr + 68 + TSC_OFFSET, 8 ).ToInt();
              FPrice        = UFC::AnsiString( DataStr + 43 + TSC_OFFSET, 8 ).ToInt();
              FQty          = UFC::AnsiString( DataStr + 51 + TSC_OFFSET, 4 ).ToInt();
              FSide         = (Execution[ 41 + TSC_OFFSET ] == 'B')?(FIX::Side_BUY):(FIX::Side_SELL);
              FErrorCode    = 0;
              WebID         = Execution.SubString( Execution.Length()-20,3);
              FOrigNID      = Execution.SubString( Execution.Length()-16,8).ToInt();
          }
      }
      else if( Market == mkOptions )
      {
          UFC::BufferedLog::DebugPrintf( " OPT Execution:%s", Execution.c_str() );
          FOrigNID = 0;
          if( Type == '1' ) ///< Filled OPT Single
          {
              FExecutionType = etFilledSingle;
              FOrderID      = UFC::AnsiString( DataStr + 8  + TSC_OFFSET, 5 );
              FExecutionSeq = UFC::AnsiString( DataStr + 69 + TSC_OFFSET, 8 ).ToInt();
              FPrice        = UFC::AnsiString( DataStr + 43 + TSC_OFFSET, 9 ).ToInt();
              FQty          = UFC::AnsiString( DataStr + 52 + TSC_OFFSET, 4 ).ToInt();
              FSide         = (Execution[ 41 + TSC_OFFSET ] == 'B')?(FIX::Side_BUY):(FIX::Side_SELL);
              FErrorCode    = 0;
              WebID         = Execution.SubString( Execution.Length()-20,3);
              FOrigNID      = Execution.SubString( Execution.Length()-16,8).ToInt();
          }
          else if( Type == '2' ) ///< Filled OPT Multileg
          {
              FExecutionType = etFilledMultileg;
              FOrderID      = UFC::AnsiString( DataStr + 8   + TSC_OFFSET,5 );
              FExecutionSeq = UFC::AnsiString( DataStr + 117 + TSC_OFFSET,8 ).ToInt();
              FPrice        = UFC::AnsiString( DataStr + 91  + TSC_OFFSET,9).ToInt();
              FLeg1Price    = UFC::AnsiString( DataStr + 52  + TSC_OFFSET,9).ToInt();
              FLeg2Price    = UFC::AnsiString( DataStr + 76  + TSC_OFFSET,9).ToInt();
              FQty          = UFC::AnsiString( DataStr + 100 + TSC_OFFSET,4).ToInt();
              FSide         = (Execution[89 + TSC_OFFSET] == 'B')?(FIX::Side_BUY):(FIX::Side_SELL);
              FLeg1Side     = (Execution[51 + TSC_OFFSET] == 'B')?(FIX::Side_BUY):(FIX::Side_SELL);
              FLeg2Side     = (Execution[75 + TSC_OFFSET] == 'B')?(FIX::Side_BUY):(FIX::Side_SELL);
              FErrorCode    = 0;
              WebID         = Execution.SubString( Execution.Length()-20,3);
              FOrigNID      = Execution.SubString( Execution.Length()-16,8).ToInt();
          }
          else if( Type == '3' ) ///< Canceled/ Replace
          {
              if( Execution[ 48 + TSC_OFFSET ] == '3' ) ///< Cancel
                  FExecutionType = etCanceled;
              else if( Execution[ 48 + TSC_OFFSET ] == '2' ) ///< Replace
                  FExecutionType = etReplaced;
              FOrderID      = UFC::AnsiString( DataStr + 10 + TSC_OFFSET, 5 );
              FExecutionSeq = UFC::AnsiString( DataStr + 65 + TSC_OFFSET, 8 ).ToInt();
              FBeforeQty    = UFC::AnsiString( DataStr + 49 + TSC_OFFSET, 4 ).ToInt();
              FAfterQty     = UFC::AnsiString( DataStr + 53 + TSC_OFFSET, 4 ).ToInt();
              FSide         = (Execution[43+ TSC_OFFSET] == 'B')?(FIX::Side_BUY):(FIX::Side_SELL);
              FErrorCode    = UFC::AnsiString( DataStr + 1+ TSC_OFFSET,2).ToInt();
              WebID         = Execution.SubString( Execution.Length()-12,3);
              FOrigNID      = Execution.SubString( Execution.Length()-20,8).ToInt();
          }
          else if( Type == '4' ) ///< Quote Canceled/ Replace
          {
			  if( Execution[ 48 + TSC_OFFSET ] == '3' ) ///< Cancel
                  FExecutionType = etQuoteCanceled;
              else if( Execution[ 48 + TSC_OFFSET ] == '2' ) ///< Replace
                  FExecutionType = etQuoteReplaced;          ///< *** FIX Quote not support chenge qty. ****
              FOrderID      = UFC::AnsiString( DataStr + 10 + TSC_OFFSET, 5 );
              FSide         = (Execution[43+ TSC_OFFSET] == 'B')?(FIX::Side_BUY):(FIX::Side_SELL);
              FBeforeQty    = UFC::AnsiString( DataStr + 49 + TSC_OFFSET, 4 ).ToInt();
              FAfterQty     = UFC::AnsiString( DataStr + 53 + TSC_OFFSET, 4 ).ToInt();
              FExecutionSeq = UFC::AnsiString( DataStr + 65 + TSC_OFFSET, 8 ).ToInt();
              FErrorCode    = UFC::AnsiString( DataStr + 1+ TSC_OFFSET,2).ToInt();
              WebID         = Execution.SubString( Execution.Length()-12,3);
              FOrigNID      = Execution.SubString( Execution.Length()-20,8).ToInt();
          }
      }
      else
          return FALSE; ///< Return false to skip this execution.
      if( FIXMapper::UseSimTX == TRUE )
      {
          if( WebID == FIXMapper::WebID ) ///< Web Code match TT2.
              FForceProcess = TRUE;
          else
              FForceProcess = FALSE;
      }
      else
      {
          FForceProcess = TRUE;
      }
      return TRUE;
}
//------------------------------------------------------------------------------
