//---------------------------------------------------------------------------
// Unit1.cpp
//---------------------------------------------------------------------------
#include "TSCTSEOrder.h"
//---------------------------------------------------------------------------
const char* WEB_CODE    = "TT2";
const char  SOURCE_CODE = 'F'; ///< From FIX
UFC::TFixedFormat*	TSCTSEOrderRecord::FOrderFormat  = NULL;
UFC::TRenderData*	TSCTSEOrderRecord::FOrderRender  = NULL;
//------------------------------------------------------------------------------
TSCTSEOrderRecord::TSCTSEOrderRecord( )
:OrderRecord( )
{
    if( FOrderFormat == NULL && FOrderRender == NULL)
    {
        FOrderFormat = new UFC::TFixedFormat();
        FOrderFormat->SetIniFile( "../cfg/TseOrder.ini" );
        FOrderRender = new UFC::TRenderData();
        FOrderRender->SetDataFormat( FOrderFormat );
    }
}
//------------------------------------------------------------------------------
// For construct a Order object
//---------------------------------------------------------------------------
TSCTSEOrderRecord::TSCTSEOrderRecord( ClientOrderID&                      CNID,
                                      UFC::AnsiString&                    OrderID,
                                      FIX::ClOrdID&                       ClOrdID,
                                      FIX::Account&                       Account,
                                      FIX::Symbol&                        Symbol,
                                      FIX::OrderQty&                      OrderQty,
                                      FIX::Price&                         Price,
                                      FIX::Side&                          Side,
                                      FIX::TimeInForce&                   TimeInForce,
                                      UFC::AnsiString&                    SalesID,
                                      UFC::AnsiString&                    Sender)
:OrderRecord( CNID,OrderID,ClOrdID,Symbol,OrderQty,Price,Side,TimeInForce,Sender)
,FAccount( Account.getString().c_str() )
,FSalesID( SalesID )
{
    if( FOrderFormat == NULL && FOrderRender == NULL)
    {
        FOrderFormat = new UFC::TFixedFormat();
        FOrderFormat->SetIniFile( "../cfg/TseOrder.ini" );
        FOrderRender = new UFC::TRenderData();
  	    FOrderRender->SetDataFormat( FOrderFormat );
    }
    FAccount.PadThis( 7, '0' );
}
//------------------------------------------------------------------------------
void TSCTSEOrderRecord::SaveToBuffer( UFC::MemoryStream& Stream )
{
    FAccount.SaveToStream( &Stream );
	FSecurityID.SaveToStream( &Stream );
}
//------------------------------------------------------------------------------
void TSCTSEOrderRecord::LoadFromBuffer( UFC::MemoryStream& Stream )
{
    FAccount.LoadFromStream( &Stream );
    FSecurityID.LoadFromStream( &Stream );
}
//------------------------------------------------------------------------------
void TSCTSEOrderRecord::FillPendingNew( FIX44::ExecutionReport& report )
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
    report.set( FIX::Symbol( FSymbol.c_str()) );
    report.set( FIX::ClOrdID( FClOrdID.c_str()) );
    report.set( FIX::Account( FAccount.c_str()) );
}

//------------------------------------------------------------------------------
void TSCTSEOrderRecord::NewOrderString( UFC::AnsiString& Order )
{
    if( FOrderRender == NULL || FMarket != mkStock )
        return;

    UFC::AnsiString HHMMSS;
    UFC::AnsiString YYYYMMDD;
    UFC::GetHHMMSS( HHMMSS );
    UFC::GetYYYYMMDD( YYYYMMDD );

    FOrderRender->SetField( "ITL020", (FSide == FIX::Side_BUY) ? "01" : "02" );
    FOrderRender->SetField( "ITL040", HHMMSS );
    FOrderRender->SetField( "ITL070", FIXMapper::BrokerID );
    FOrderRender->SetField( "ITL110120", FAccount.SubString( 0,7 ) );
    FOrderRender->SetField( "ITL140", GetSymbol() );
    FOrderRender->SetField( "ITL150", (int)((FPrice+0.001 )* 100) );
    FOrderRender->SetField( "ITL160", FQty );
    FOrderRender->SetField( "ITL170", (FSide == FIX::Side_BUY) ? "B" : "S" );
    FOrderRender->SetField( "ITL520", FSalesID );
    FOrderRender->SetField( "ITL530", FNID );
    FOrderRender->SetField( "ITL540", FNID );
    FOrderRender->SetField( "ITL550", YYYYMMDD );
    FOrderRender->SetField( "ITL560", HHMMSS );
    FOrderRender->SetField( "ITL570", 0 );
    FOrderRender->SetField( "ITL580", FNID );
    FOrderRender->SetField( "ITL610", FIXMapper::WebID );
    FOrderRender->SetField( "ITL650", FIXMapper::WebID );
    FOrderRender->SetField( "ITIDAT", YYYYMMDD );
    FOrderRender->SetField( "ITITIM", HHMMSS );
    Order = FOrderRender->Render();
}
//------------------------------------------------------------------------------
void TSCTSEOrderRecord::CancelString(  Int32 NID, UFC::AnsiString& Cancel )
{

    if( FOrderRender == NULL || FMarket != mkStock )
        return;

    UFC::AnsiString HHMMSS;
    UFC::AnsiString YYYYMMDD;

    UFC::GetHHMMSS( HHMMSS );
    UFC::GetYYYYMMDD( YYYYMMDD );

    FOrderRender->SetField( "ITL020", "04" ); ///< Cancel
    FOrderRender->SetField( "ITL040", HHMMSS );
    FOrderRender->SetField( "ITL070", FIXMapper::BrokerID );
    FOrderRender->SetField( "ITL110120", FAccount.SubString( 0,7 ) );
    FOrderRender->SetField( "ITL140", GetSymbol() );
    FOrderRender->SetField( "ITL150", (int)((FPrice + 0.001) * 100) );
    FOrderRender->SetField( "ITL160", FQty );
    FOrderRender->SetField( "ITL170", (FSide == FIX::Side_BUY) ? "B" : "S" );
    FOrderRender->SetField( "ITL530", NID );
    FOrderRender->SetField( "ITL540", NID );
    FOrderRender->SetField( "ITL550", YYYYMMDD );
    FOrderRender->SetField( "ITL560", 0 );
    FOrderRender->SetField( "ITL570", 0 );
    FOrderRender->SetField( "ITL580", FNID );
    FOrderRender->SetField( "ITL610", FIXMapper::WebID );
    FOrderRender->SetField( "ITL650", FIXMapper::WebID );
    FOrderRender->SetField( "ITIDAT", YYYYMMDD );
    FOrderRender->SetField( "ITITIM", HHMMSS );
    Cancel = FOrderRender->Render();
    UFC::BufferedLog::DebugPrintf(" To Flow Engine:[%s]", Cancel.c_str());

}
//------------------------------------------------------------------------------
void TSCTSEOrderRecord::ReduceString(  Int32 NID, Int32 NewQty, UFC::AnsiString& Reduce )
{
    if( FOrderRender == NULL || FMarket != mkStock )
        return;

    UFC::AnsiString HHMMSS;
    UFC::AnsiString YYYYMMDD;

    UFC::GetHHMMSS( HHMMSS );
    UFC::GetYYYYMMDD( YYYYMMDD );
    FOrderRender->SetField( "ITL020", "03" ); ///< Reduce
    FOrderRender->SetField( "ITL040", HHMMSS );
    FOrderRender->SetField( "ITL070", FIXMapper::BrokerID );
    FOrderRender->SetField( "ITL110120", FAccount.SubString( 0, 7 ) );
    FOrderRender->SetField( "ITL140", this->GetSymbol() );
    FOrderRender->SetField( "ITL150", (int)((FPrice + 0.001) * 100) );
    FOrderRender->SetField( "ITL160", NewQty );
    FOrderRender->SetField( "ITL170", (FSide == FIX::Side_BUY) ? "B" : "S" );
    FOrderRender->SetField( "ITL530", NID );
    FOrderRender->SetField( "ITL540", NID );
    FOrderRender->SetField( "ITL550", YYYYMMDD );
    FOrderRender->SetField( "ITL560", 0 );
    FOrderRender->SetField( "ITL570", 0 );
    FOrderRender->SetField( "ITL580", FNID );
    FOrderRender->SetField( "ITL610", FIXMapper::WebID );
    FOrderRender->SetField( "ITL650", FIXMapper::WebID );
    FOrderRender->SetField( "ITIDAT", YYYYMMDD );
    FOrderRender->SetField( "ITITIM", HHMMSS );

    Reduce = FOrderRender->Render();
}
//------------------------------------------------------------------------------
bool TSCTSEConfirmParser::Parse( MarketKind Market, const UFC::AnsiString& Confirm )
{
    UFC::TRecord* pRecord;
    UFC::AnsiString  Value;
    UFC::AnsiString  Value2;
    UFC::AnsiString  OrderType;
    FErrorCode = 0;
    FQty       = 0;
    FOrderID   = "00000";

    if( ( pRecord = FOrderMKParser.Parse( Confirm.c_str(), Confirm.Length() )) != NULL )
    {
        pRecord->GetField( "ITR050", Value );
        FErrorCode = Value.ToInt();
        pRecord->GetField( "ITR090", Value );
        pRecord->GetField( "ITR100", Value2 );
        FOrderID  = Value + Value2;
        pRecord->GetField( "ITR600", FErrorMsg );
        pRecord->GetField( "ITR020", OrderType );
        switch( OrderType.ToInt() )
        {
           case 1:
           case 2:FTransKind = tkNewOrder;
                  pRecord->GetField( "ITR160", Value );
                  FQty       = Value.ToInt();
                  break;
           case 3:FTransKind = tkOrderReplace;
                  pRecord->GetField( "ITR220", Value );
                  pRecord->GetField( "ITR230", Value2 );
                  FQty       = Value.ToInt() - Value2.ToInt();
                  break;
           case 4:FTransKind = tkOrderCancel;
                  break;
        }
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
BOOL TSCTSEExecutionParser::Parse( MarketKind Market, const UFC::AnsiString& Execution )
{
    if( Market != mkStock )
       return FALSE;

    UFC::AnsiString WebID;
    UFC::AnsiString Value;
    UFC::AnsiString Value2;

    UFC::TRecord*   pRecord = FFillParser.Parse( Execution.c_str(), Execution.Length() );

    if( pRecord != NULL )
    {

        FExecutionType = etFilledSingle;
        pRecord->GetField( "ITM140", Value );
        pRecord->GetField( "ITM150", Value2 );
        FOrderID       =  Value + Value2 ;
        pRecord->GetField( "ITM210", Value );
        FExecutionSeq  = Value.ToInt();
        pRecord->GetField( "ITM100", Value );
        FPrice         = Value.ToInt();
        pRecord->GetField( "ITM090", Value );
        FQty           = Value.ToInt();
        pRecord->GetField( "ITM130", Value );
        FSide          = ( Value == "B" )?(FIX::Side_BUY):(FIX::Side_SELL);
        pRecord->GetField( "ITM050", Value );
        FErrorCode     = Value.ToInt();
        pRecord->GetField( "ITM580", WebID );
        pRecord->GetField( "ITM540", Value );
        FOrigNID       = Value.ToInt();
    }
    else
      UFC::BufferedLog::DebugPrintf( " Excution parser return no record." );

    if( WebID == FIXMapper::WebID ) ///< Web Code match TT2.
        FForceProcess = TRUE;
    else
        FForceProcess = FALSE;
    return TRUE;
}
//------------------------------------------------------------------------------
