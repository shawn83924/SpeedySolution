//---------------------------------------------------------------------------
#include "OrcTFXMapper.h"
//---------------------------------------------------------------------------
// Receive Futures Order Filled message from TAIFEX.(C030)
//
// Map to FIX ExecutionReport
//---------------------------------------------------------------------------
void OrcTFXMapper::onFuturesExecution( ExecutionParser* Execution )
{
      OrderRecord*  OrgOrd;

      if( (OrgOrd = FStore->GetOrder( mkFutures, Execution->GetOrderID())) == NULL  ) ///< Use OrderID to query order object.
      {
          if( Execution->GetForceProcess() == TRUE )
          {
              if( (OrgOrd = FStore->GetOrder( Execution->GetOrigNID() ) ) == NULL) ///< Use NID to query order object.
              {
                  UFC::BufferedLog::Printf( " Futures fill report NID:%d Not found!", Execution->GetOrigNID());
                  return;
              }
          }
          else
          {
              UFC::BufferedLog::Printf( " Futures fill report OrderID:%s Not found!", Execution->GetOrderID().c_str());
              return;
          }
      }
      switch( Execution->GetType() )
      {
        case etFilledSingle: FutFilled( OrgOrd, Execution );return;
        case etCanceled:     FutCanceled( OrgOrd, Execution );return;
        case etReplaced:     FutReplaced( OrgOrd, Execution );return;
      }
}
//---------------------------------------------------------------------------
//  Futures order filled.
//---------------------------------------------------------------------------
void OrcTFXMapper::FutFilled( OrderRecord* OrgOrd, ExecutionParser* Execution )
{
    UFC::AnsiString ExecID = FIXMapperApp::GenerateExecID( );
    FIX::OrdStatus  OrdStatus;

    OrgOrd->FillQty( Execution->GetQty());
    if( OrgOrd->GetLeavesQty() == 0)
    {
        OrdStatus = FIX::OrdStatus_FILLED;
        OrgOrd->SetState( osFilled );     ///< change order state
        UFC::BufferedLog::DebugPrintf( "Futures Order Filled\n");
    }
    else
    {
        OrdStatus = FIX::OrdStatus_PARTIALLY_FILLED;
        OrgOrd->SetState( osPartialFilled );     ///< change order state
        UFC::BufferedLog::DebugPrintf( "Futures Order Partially Filled\n");
    }
    OrgOrd->SetOrderID( Execution->GetOrderID() );///< Set the OrderID to OrderRecoder.
    FStore->UpdateOrder( OrgOrd ); ///< Sync to file.
    FIX44::ExecutionReport report( FIX::OrderID( OrgOrd->GetOrderID().c_str()),
                                   FIX::ExecID( ExecID.c_str()),
                                   FIX::ExecType_TRADE,
                                   OrdStatus,
                                   Execution->GetSide(),
                                   FIX::LeavesQty( OrgOrd->GetLeavesQty() ),
                                   FIX::CumQty( OrgOrd->GetCumQty( ) ),
                                   FIX::AvgPx( 0 ));
	report.set( FIX::SecurityID( static_cast<TSCTFXOrderRecord*>(OrgOrd)->GetSecurityID().c_str() ) );
    report.set( FIX::OrderQty( OrgOrd->GetOrderQty() ) );
    if( OrgOrd->IsMarketOrder() == FALSE )
        report.set( FIX::Price( OrgOrd->GetPrice() ) );
    if( TFXSymbolUtility.PriceDigi( OrgOrd->GetSymbol())==3)
        report.set( FIX::LastPx( (double)Execution->GetPrice()/1000.000 ) );
    else
        report.set( FIX::LastPx( (double)Execution->GetPrice()/100.000 ) );
    report.set( FIX::LastQty( Execution->GetQty()) );
    report.set( FIX::Symbol(OrgOrd->GetSymbol().c_str() ) );
    report.set( FIX::ClOrdID(OrgOrd->GetClOrdID().c_str() ) );
    report.set( FIX::TransactTime());
    report.set( FIX::SecondaryExecID( UFC::AnsiString(Execution->GetSequenceNumber()).c_str()));
    Application->SendToimFIX( OrgOrd->GetSender(),report );
}
//---------------------------------------------------------------------------
//  Futures order Canceled.
//---------------------------------------------------------------------------
void OrcTFXMapper::FutCanceled( OrderRecord* OrgOrd, ExecutionParser* Execution  )
{
    UFC::AnsiString ExecID    = FIXMapperApp::GenerateExecID( );
    Int32           ErrorCode = Execution->GetErrorCode();

    if( ErrorCode!=0 )
    {
        if( ErrorCode == 10 ) ///< Order not in orderbook
            RejectCancelCanceled(FIX::ClOrdID(OrgOrd->GetLastClOrdID().c_str()),
                                 FIX::OrigClOrdID(OrgOrd->GetOrigClOrdID().c_str()),
                                 FIX::OrderID(OrgOrd->GetOrderID().c_str()),
                                 FIX::CxlRejResponseTo('1'));
        UFC::BufferedLog::DebugPrintf("Futures Error Report code:%d\n",ErrorCode );
        return;
    }
    UFC::BufferedLog::DebugPrintf("Futures Order Canceled\n");
    OrgOrd->SetState( osCanceled );
    FStore->UpdateOrder( OrgOrd );
    FIX44::ExecutionReport report( FIX::OrderID( OrgOrd->GetOrderID().c_str()),
                                   FIX::ExecID( ExecID.c_str()),
                                   FIX::ExecType_CANCELED,
                                   FIX::OrdStatus_CANCELED,
                                   Execution->GetSide(),
                                   FIX::LeavesQty( 0 ),
                                   FIX::CumQty( OrgOrd->GetCumQty( ) ),
                                   FIX::AvgPx( 0 ));
	report.set( FIX::SecurityID( static_cast<TSCTFXOrderRecord*>(OrgOrd)->GetSecurityID().c_str() ) );
    report.set( FIX::OrderQty( OrgOrd->GetOrderQty() ) );
    if( OrgOrd->IsMarketOrder() == FALSE )
        report.set( FIX::Price( OrgOrd->GetPrice() ) );
    report.set( FIX::LastPx( OrgOrd->GetPrice() ) );
    report.set( FIX::LastQty( 0 ) );
    report.set( FIX::Symbol( OrgOrd->GetSymbol().c_str() ) );
    report.set( FIX::OrigClOrdID(OrgOrd->GetOrigClOrdID().c_str() ) );
    report.set( FIX::ClOrdID( OrgOrd->GetLastClOrdID().c_str() ) );
    report.set( FIX::SecondaryExecID( UFC::AnsiString(Execution->GetSequenceNumber()).c_str()));
    Application->SendToimFIX( OrgOrd->GetSender(),report );
}
//---------------------------------------------------------------------------
//  Futures order Replaced.
//---------------------------------------------------------------------------
void OrcTFXMapper::FutReplaced( OrderRecord* OrgOrd, ExecutionParser* Execution )
{
    UFC::AnsiString ExecID  = FIXMapperApp::GenerateExecID( );
    Int32           ErrorCode = Execution->GetErrorCode();
    FIX::OrdStatus  OrdStatus;

    if( ErrorCode!=0 && ErrorCode!= 32 )
    {
        if( ErrorCode == 10 ) ///< Order not in orderbook
            RejectCancelCanceled(FIX::ClOrdID(OrgOrd->GetLastClOrdID().c_str()),
                                 FIX::OrigClOrdID(OrgOrd->GetOrigClOrdID().c_str()),
                                 FIX::OrderID(OrgOrd->GetOrderID().c_str()),
                                 FIX::CxlRejResponseTo('1'));
        UFC::BufferedLog::DebugPrintf("Futures Error Report code:%d\n",ErrorCode);
        return;
    }
    UFC::BufferedLog::DebugPrintf("Futures Order Replaced\n");
    OrgOrd->ReduceQty( Execution->GetBeforeQty() - Execution->GetAfterQty());
    if( OrgOrd->GetLeavesQty( ) == 0 )
    {
        OrdStatus = FIX::OrdStatus_FILLED;
        OrgOrd->SetState( osFilled );
    }
    else if( OrgOrd->GetCumQty() == 0 )
    {
        OrdStatus = FIX::OrdStatus_NEW;
        OrgOrd->SetState( osNew );
    }
    else
    {
        OrdStatus = FIX::OrdStatus_PARTIALLY_FILLED;
        OrgOrd->SetState( osPartialFilled );
    }
    FIX44::ExecutionReport report( FIX::OrderID( OrgOrd->GetOrderID().c_str()),
                                   FIX::ExecID( ExecID.c_str()),
                                   FIX::ExecType_REPLACE,
                                   OrdStatus,
                                   Execution->GetSide(),
                                   FIX::LeavesQty( OrgOrd->GetLeavesQty( ) ),
                                   FIX::CumQty( OrgOrd->GetCumQty( ) ),
                                   FIX::AvgPx( 0 ));
	report.set( FIX::SecurityID( static_cast<TSCTFXOrderRecord*>(OrgOrd)->GetSecurityID().c_str() ) );
    report.set( FIX::OrderQty( OrgOrd->GetOrderQty()) );
    if( OrgOrd->IsMarketOrder() == FALSE )
        report.set( FIX::Price( OrgOrd->GetPrice() ) );
    report.set( FIX::LastPx( OrgOrd->GetPrice() ) );
    report.set( FIX::LastQty( 0 ) );
    report.set( FIX::Symbol(OrgOrd->GetSymbol().c_str() ) );
    report.set( FIX::OrigClOrdID(OrgOrd->GetOrigClOrdID().c_str() ) );
    report.set( FIX::ClOrdID(OrgOrd->GetLastClOrdID().c_str() ) );
    report.set( FIX::SecondaryExecID( UFC::AnsiString(Execution->GetSequenceNumber()).c_str()));
    OrgOrd->SetOrigClOrdID( OrgOrd->GetLastClOrdID().c_str() );
    FStore->UpdateOrder( OrgOrd ); ///< Sync to file.
    Application->SendToimFIX( OrgOrd->GetSender(),report );
}
//---------------------------------------------------------------------------
// Receive Option Order Filled message from TAIFEX.(C030)
//
// Map to FIX ExecutionReport
//---------------------------------------------------------------------------
void OrcTFXMapper::onOptionsExecution( ExecutionParser* Execution  )
{
    OrderRecord*  OrgOrd;

    if( (OrgOrd = FStore->GetOrder( mkOptions, Execution->GetOrderID()) ) == NULL  ) ///< Use OrderID to query order object.
    {
        if( Execution->GetForceProcess() == TRUE )
        {
            if( (OrgOrd = FStore->GetOrder( Execution->GetOrigNID() ) ) == NULL) ///< Use NID to query order object.
            {
                UFC::BufferedLog::Printf( " Options fill report NID:%d Not found!", Execution->GetOrigNID());
                return;
            }
        }
        else
        {
            UFC::BufferedLog::Printf( " Options fill report OrderID:%s Not found!", Execution->GetOrderID().c_str());
            return;
        }
    }
    switch( Execution->GetType() )
    {
      case etFilledSingle:   OptFilled( OrgOrd, Execution );break;
      case etFilledMultileg: break;
      case etCanceled:       OptCanceled( OrgOrd, Execution );break;
      case etReplaced:       OptReplaced( OrgOrd, Execution );break;
      case etQuoteCanceled:  QuoteCanceled( OrgOrd, Execution );break;
      case etQuoteReplaced:  break; ///< !!!Not support by FIX, FIX Quote can canceled only!!!
    }
}
//---------------------------------------------------------------------------
//  Options order filled.
//---------------------------------------------------------------------------
void OrcTFXMapper::OptFilled( OrderRecord* OrgOrd, ExecutionParser* Execution )
{
    UFC::AnsiString ExecID = FIXMapperApp::GenerateExecID( );
    FIX::OrdStatus  OrdStatus;
    FIX::ExecType   ExecType;
    int             OrderQty,CumQty,LeavesQty,LastQty;
    double          Price;

    if( Execution->GetQty() == 0 ) ///< Cancel
    {
        ExecType  = FIX::ExecType_CANCELED;
        OrdStatus = FIX::OrdStatus_CANCELED;
        OrderQty  = OrgOrd->GetOrderQty();
        LeavesQty = 0;
        LastQty   = 0;
        CumQty    = OrgOrd->GetCumQty( );
        Price     = OrgOrd->GetPrice();
        OrgOrd->SetState( osCanceled );
        UFC::BufferedLog::DebugPrintf( "Option Order Canceled\n");
    }
    else ///<  Partially Filled or Filled
    {
        if( OrgOrd->IsOrder() == FALSE && Execution->GetSide() == FIX::Side_BUY )///< Is a Quote
        {
            OrgOrd->FillBidQty( Execution->GetQty() );
            OrderQty  = OrgOrd->GetBidOrigQty();
            LeavesQty = OrgOrd->GetBidLeavesQty( );
            ExecType  = FIX::ExecType_TRADE;
            LastQty   = Execution->GetQty();
            CumQty    = OrgOrd->GetBidCumQty( );
            Price     = OrgOrd->GetBidPrice();
        }
        else ///< Is an order or the sell side of a quote.
        {
            OrgOrd->FillQty( Execution->GetQty() );
            OrderQty  = OrgOrd->GetOrderQty();
            LeavesQty = OrgOrd->GetLeavesQty( );
            ExecType  = FIX::ExecType_TRADE;
            LastQty   = Execution->GetQty();
            CumQty    = OrgOrd->GetCumQty( );
            Price     = OrgOrd->GetPrice();
        }
        if( LeavesQty == 0 )
        {
            OrdStatus = FIX::OrdStatus_FILLED;
            OrgOrd->SetState( osFilled );     ///< change order state
            UFC::BufferedLog::DebugPrintf( "Option Order Filled\n");
        }
        else
        {
            OrdStatus = FIX::OrdStatus_PARTIALLY_FILLED;
            OrgOrd->SetState( osPartialFilled );     ///< change order state
            UFC::BufferedLog::DebugPrintf( "Option Order Partially Filled\n");
        }
	}
    OrgOrd->SetOrderID( Execution->GetOrderID() );
    FStore->UpdateOrder( OrgOrd ); ///< Sync to file.
    FIX44::ExecutionReport report( FIX::OrderID( OrgOrd->GetOrderID().c_str()),
                                   FIX::ExecID( ExecID.c_str()),
                                   ExecType,
                                   OrdStatus,
                                   Execution->GetSide(),
                                   FIX::LeavesQty( LeavesQty ),
                                   FIX::CumQty( CumQty ),
                                   FIX::AvgPx( 0 ));
	report.set( FIX::SecurityID( static_cast<TSCTFXOrderRecord*>(OrgOrd)->GetSecurityID().c_str() ) );
    report.set( FIX::OrderQty( OrderQty ) );
    if( OrgOrd->IsMarketOrder()== FALSE )
        report.set( FIX::Price( Price ) );
    report.set( FIX::LastPx( (double)(Execution->GetPrice())/1000.0 ) );
    report.set( FIX::LastQty( LastQty ));
    report.set( FIX::Symbol( OrgOrd->GetSymbol().c_str() ) );
    report.set( FIX::ClOrdID( OrgOrd->GetClOrdID().c_str() ) );
    report.set( FIX::TransactTime());
    report.set( FIX::SecondaryExecID( UFC::AnsiString( Execution->GetSequenceNumber()).c_str()));
    ///< Set the OrigClOrdID if this is a canceled report.
    if( OrdStatus == FIX::OrdStatus_CANCELED )
        report.set( FIX::OrigClOrdID( OrgOrd->GetOrigClOrdID().c_str() ) );
    Application->SendToimFIX( OrgOrd->GetSender(),report );
    ///< Delete an IOC partial filled order after n sec.
    if( OrdStatus == FIX::OrdStatus_PARTIALLY_FILLED && OrgOrd->GetTimeInForce() == FIX::TimeInForce_IMMEDIATE_OR_CANCEL )
        Application->CanclOrder( OrgOrd->GetNID(), Application->GetCancelInterval() );
}
//---------------------------------------------------------------------------
void OrcTFXMapper::OptCanceled( OrderRecord* OrgOrd, ExecutionParser* Execution )
{
    UFC::AnsiString ExecID    = FIXMapperApp::GenerateExecID( );
    Int32           ErrorCode = Execution->GetErrorCode();

    if( ErrorCode !=0 )
    {
        if( ErrorCode == 10 ) ///< Order not in orderbook
            RejectCancelCanceled(FIX::ClOrdID(OrgOrd->GetLastClOrdID().c_str()),
                                 FIX::OrigClOrdID(OrgOrd->GetOrigClOrdID().c_str()),
                                 FIX::OrderID(OrgOrd->GetOrderID().c_str()),
                                 FIX::CxlRejResponseTo('1'));
        UFC::BufferedLog::DebugPrintf("Option Error Report code:%d\n",Execution->GetErrorCode() );
        return;
    }
    OrgOrd->SetState( osCanceled );
    FStore->UpdateOrder( OrgOrd ); ///< Sync to file.
    UFC::BufferedLog::DebugPrintf("Option Order Canceled\n");
    FIX44::ExecutionReport report( FIX::OrderID( OrgOrd->GetOrderID().c_str()),
                                   FIX::ExecID( ExecID.c_str()),
                                   FIX::ExecType_CANCELED,
                                   FIX::OrdStatus_CANCELED,
                                   Execution->GetSide(),
                                   FIX::LeavesQty( 0 ),
                                   FIX::CumQty( OrgOrd->GetCumQty( ) ),
                                   FIX::AvgPx( 0 ));
	report.set( FIX::SecurityID( static_cast<TSCTFXOrderRecord*>(OrgOrd)->GetSecurityID().c_str() ) );
    if( OrgOrd->IsMarketOrder() == FALSE )
        report.set( FIX::Price( OrgOrd->GetPrice() ) );
    report.set( FIX::OrderQty( OrgOrd->GetOrderQty() ) );
    report.set( FIX::LastQty( 0 ) );
    report.set( FIX::Symbol(OrgOrd->GetSymbol().c_str() ) );
    report.set( FIX::OrigClOrdID(OrgOrd->GetOrigClOrdID().c_str() ) );
    report.set( FIX::ClOrdID(OrgOrd->GetLastClOrdID().c_str()) );
    report.set( FIX::SecondaryExecID( UFC::AnsiString(Execution->GetSequenceNumber()).c_str()));
    Application->SendToimFIX( OrgOrd->GetSender(),report );
}
//---------------------------------------------------------------------------
void OrcTFXMapper::OptReplaced( OrderRecord* OrgOrd, ExecutionParser* Execution )
{
    UFC::AnsiString ExecID    = FIXMapperApp::GenerateExecID( );
    Int32           ErrorCode = Execution->GetErrorCode();
    FIX::OrdStatus  OrdStatus;

    if( ErrorCode!=0 && ErrorCode != 32 )
    {
       if( ErrorCode == 10 ) ///< Order not in orderbook
           RejectCancelCanceled(FIX::ClOrdID(OrgOrd->GetLastClOrdID().c_str()),
                                FIX::OrigClOrdID(OrgOrd->GetOrigClOrdID().c_str()),
                                FIX::OrderID(OrgOrd->GetOrderID().c_str()),
                                FIX::CxlRejResponseTo('1'));
        UFC::BufferedLog::DebugPrintf("Option Error Report code:%d\n",Execution->GetErrorCode());
        return;
    }
    UFC::BufferedLog::DebugPrintf("Option Order Reduced\n");
    OrgOrd->ReduceQty( Execution->GetBeforeQty() - Execution->GetAfterQty() );
    if( OrgOrd->GetLeavesQty( ) == 0 )
    {
        OrdStatus = FIX::OrdStatus_FILLED;           ///< Filled.
        OrgOrd->SetState( osFilled );
    }
    else if( OrgOrd->GetCumQty() == 0 )
    {
        OrdStatus = FIX::OrdStatus_NEW;              ///< New order
        OrgOrd->SetState( osNew );
    }
    else
    {
        OrdStatus = FIX::OrdStatus_PARTIALLY_FILLED; ///<Partially filled
        OrgOrd->SetState( osPartialFilled );
    }
    FIX44::ExecutionReport report( FIX::OrderID( OrgOrd->GetOrderID().c_str()),
                                   FIX::ExecID( ExecID.c_str()),
                                   FIX::ExecType_REPLACE,
                                   OrdStatus,
                                   Execution->GetSide(),
                                   FIX::LeavesQty( OrgOrd->GetLeavesQty( ) ),
                                   FIX::CumQty( OrgOrd->GetCumQty( ) ),
                                   FIX::AvgPx( 0 ));
	report.set( FIX::SecurityID( static_cast<TSCTFXOrderRecord*>(OrgOrd)->GetSecurityID().c_str() ) );
    if( OrgOrd->IsMarketOrder() == FALSE )
        report.set( FIX::Price( OrgOrd->GetPrice() ) );
    report.set( FIX::OrderQty( OrgOrd->GetOrderQty() ) );
    report.set( FIX::LastQty( 0 ) );
    report.set( FIX::Symbol(OrgOrd->GetSymbol().c_str() ) );
    report.set( FIX::OrigClOrdID(OrgOrd->GetOrigClOrdID().c_str() ) );
    report.set( FIX::ClOrdID(OrgOrd->GetLastClOrdID().c_str()) );
    report.set( FIX::SecondaryExecID(UFC::AnsiString(Execution->GetSequenceNumber()).c_str()));
    OrgOrd->SetOrigClOrdID( OrgOrd->GetLastClOrdID().c_str() );
    FStore->UpdateOrder( OrgOrd ); ///< Sync to file.
    Application->SendToimFIX( OrgOrd->GetSender(),report );
}
//---------------------------------------------------------------------------
void OrcTFXMapper::QuoteCanceled( OrderRecord* OrgOrd, ExecutionParser* Execution )
{
    if( Execution->GetErrorCode() == 0  )
    {
        if( OrgOrd->GetState( ) != osCanceled )///< Not canceled yet
        {
            OrgOrd->SetState( osCanceled );
            FStore->UpdateOrder( OrgOrd ); ///< Sync to file.
	    	ReportQuoteStatus( FIX::QuoteID( OrgOrd->GetClOrdID().c_str()), FIX::QuoteStatus( FIX::QuoteStatus_CANCELED_FOR_SYMBOL ),FIX::Text("Quote canceled") );
        }
    }
	else
        UFC::BufferedLog::DebugPrintf("Quote cancel Report code:%d\n",Execution->GetErrorCode() );
}
//---------------------------------------------------------------------------
