//---------------------------------------------------------------------------
#include "GLTSEMapper.h"
//---------------------------------------------------------------------------
// Receive Futures Order Filled message from TAIFEX.(C030)
//
// Map to FIX ExecutionReport
//---------------------------------------------------------------------------
void OrcTSEMapper::onStockExecution( ExecutionParser* Execution )
{
      OrderRecord*  OrgOrd;

      if( (OrgOrd = FStore->GetOrder( mkTSE, Execution->GetOrderID())) == NULL  ) ///< Use OrderID to query order object.
      {
          if( Execution->GetForceProcess() == TRUE )
          {
              if( (OrgOrd = FStore->GetOrder( Execution->GetOrigNID() ) ) == NULL) ///< Use NID to query order object.
              {
                  UFC::BufferedLog::Printf( " Stock fill report NID:%d Not found!", Execution->GetOrigNID());
                  return;
              }
          }
          else
          {
              UFC::BufferedLog::Printf( " Stock fill report OrderID:%s Not found!", Execution->GetOrderID().c_str());
              return;
          }
      }
      StockFilled( OrgOrd, Execution );
}
//---------------------------------------------------------------------------
//  Stock order filled.
//---------------------------------------------------------------------------
void OrcTSEMapper::StockFilled( OrderRecord* OrgOrd, ExecutionParser* Execution )
{
    UFC::AnsiString ExecID = FIXMapperApp::GenerateExecID( );
    FIX::OrdStatus  OrdStatus;

    OrgOrd->FillQty( Execution->GetQty() );
    if( OrgOrd->GetLeavesQty() == 0)
    {
        OrdStatus = FIX::OrdStatus_FILLED;
        OrgOrd->SetState( osFilled );     ///< change order state
    }
    else
    {
        OrdStatus = FIX::OrdStatus_PARTIALLY_FILLED;
        OrgOrd->SetState( osPartialFilled );     ///< change order state
    }
    FStore->UpdateOrder( OrgOrd ); ///< Sync to file.
    FIX44::ExecutionReport report( FIX::OrderID( OrgOrd->GetOrderID().c_str()),
                                   FIX::ExecID( ExecID.c_str()),
                                   FIX::ExecType_TRADE,
                                   OrdStatus,
                                   Execution->GetSide(),
                                   FIX::LeavesQty( OrgOrd->GetLeavesQty() ),      
                                   FIX::CumQty( OrgOrd->GetCumQty( ) ),           
                                   FIX::AvgPx( 0 ));
    report.set( FIX::OrderQty( OrgOrd->GetOrderQty() ) );                         
    if( OrgOrd->IsMarketOrder() == FALSE )
        report.set( FIX::Price( OrgOrd->GetPrice() ) );
    report.set( FIX::LastPx( UFC::IntToDouble( Execution->GetPrice() , 2 ) ) );    
    report.set( FIX::LastQty( Execution->GetQty() ) );                            
    report.set( FIX::Symbol(OrgOrd->GetSymbol().c_str() ) );
    report.set( FIX::ClOrdID(OrgOrd->GetClOrdID().c_str() ) );
    report.set( FIX::TransactTime());
    report.set( FIX::SecondaryExecID( UFC::AnsiString(Execution->GetSequenceNumber()).c_str()));
    report.set( FIX::Account( ((TSCTSEOrderRecord*)OrgOrd)->GetAccount().c_str()));
    Application->SendToimFIX( OrgOrd->GetSender(),report );
}
//---------------------------------------------------------------------------
