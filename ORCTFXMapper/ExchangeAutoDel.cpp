//---------------------------------------------------------------------------
#include "OrcTFXMapper.h"
//---------------------------------------------------------------------------
void OrcTFXMapper::onCancelOrder( Int32 NID )
{
    OrderRecord*                   OrgOrd;

    if( (OrgOrd = FStore->GetOrder( NID ))!= NULL )
    {
        UFC::BufferedLog::DebugPrintf( "Options IOC Order Canceled\n");
        UFC::AnsiString ExecID = FIXMapperApp::GenerateExecID( );

        FIX44::ExecutionReport report( FIX::OrderID( OrgOrd->GetOrderID().c_str()),
                                       FIX::ExecID( ExecID.c_str()),
                                       FIX::ExecType( FIX::ExecType_CANCELED ),
                                       FIX::OrdStatus( FIX::OrdStatus_CANCELED ),
                                       FIX::Side( OrgOrd->GetSide( ) ),
                                       FIX::LeavesQty( 0 ),
                                       FIX::CumQty( OrgOrd->GetCumQty( ) ),
                                       FIX::AvgPx( 0 ));
        report.set( FIX::OrderQty( OrgOrd->GetOrderQty() ) );
        if( OrgOrd->IsMarketOrder() == FALSE )
            report.set( FIX::Price( OrgOrd->GetPrice() ) );
        report.set( FIX::LastPx( 0 ) );
        report.set( FIX::LastQty( 0 ) );
		report.set( FIX::SecurityID( static_cast<TSCTFXOrderRecord*>(OrgOrd)->GetSecurityID().c_str() ) );
        report.set( FIX::Symbol( OrgOrd->GetSymbol().c_str() ) );
        report.set( FIX::OrigClOrdID(OrgOrd->GetClOrdID().c_str() ) );
        report.set( FIX::ClOrdID( OrgOrd->GetClOrdID().c_str() ) );
        Application->SendToimFIX( OrgOrd->GetSender(),report );
    }
}
//---------------------------------------------------------------------------
