//---------------------------------------------------------------------------
#include "TSCTSEOrder.h"
//------------------------------------------------------------------------------
// For construct a Order object
//---------------------------------------------------------------------------
UFC::TFixedFormat*	TSCTSEOrderRecord::FOrderFormat = NULL;
UFC::TRenderData*	TSCTSEOrderRecord::FOrderRender = NULL;
//---------------------------------------------------------------------------
void TSCTSEOrderRecord::InitRender()
{
    TSCTSEOrderRecord::FOrderFormat = new UFC::TFixedFormat();
    TSCTSEOrderRecord::FOrderFormat->SetIniFile( "../cfg/TseOrder.ini" );
    TSCTSEOrderRecord::FOrderRender = new UFC::TRenderData();
    TSCTSEOrderRecord::FOrderRender->SetDataFormat( FOrderFormat );
}
//---------------------------------------------------------------------------
TSCTSEOrderRecord::TSCTSEOrderRecord( )
:OrderRecord( )
{
}
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
                                      UFC::AnsiString&                    Sender,
                                      BOOL                                IsMarketOrder )
:OrderRecord( CNID,OrderID,ClOrdID,Symbol,OrderQty,Price,Side,TimeInForce,Sender, IsMarketOrder)
,FAccount( Account.getString().c_str() )
,FSalesID( SalesID )
{
    FAccount.PadThis( 7, '0' );
}
//------------------------------------------------------------------------------
void TSCTSEOrderRecord::SaveToBuffer( UFC::MemoryStream& Stream )
{
    FAccount.SaveToStream( &Stream );
    FSalesID.SaveToStream( &Stream );
}
//------------------------------------------------------------------------------
void TSCTSEOrderRecord::LoadFromBuffer( UFC::MemoryStream& Stream )
{
    FAccount.LoadFromStream( &Stream );
    FSalesID.LoadFromStream( &Stream );
}
//------------------------------------------------------------------------------
void TSCTSEOrderRecord::FillPendingNew( FIX44::ExecutionReport& report )
{
    report = FIX44::ExecutionReport( FIX::OrderID( FOrderID.c_str() ),
                                     FIX::ExecID( FIXMapperApp::GenerateExecID().c_str()),
                                     FIX::ExecType_PENDING_NEW,
                                     FIX::OrdStatus_PENDING_NEW,
                                     FSide,
                                     FIX::LeavesQty( FQty  ),
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
    if( FOrderRender == NULL )
        return;
    UFC::AnsiString HHMMSS;
    UFC::AnsiString YYYYMMDD;

    UFC::GetHHMMSS( HHMMSS );
    UFC::GetYYYYMMDD( YYYYMMDD );
    FOrderRender->SetField( "ITL020", (FSide == FIX::Side_BUY) ? "01" : "02" ); ///< New
    FOrderRender->SetField( "ITL040", HHMMSS );
    FOrderRender->SetField( "ITL070", FIXMapper::BrokerID );
    FOrderRender->SetField( "ITL110120", FAccount );
    FOrderRender->SetField( "ITL140", GetSymbol() );
    FOrderRender->SetField( "ITL150", UFC::DoubleToInt( FPrice, 2 ) );
    FOrderRender->SetField( "ITL160", FQty / 1000 );
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
    if( FOrderRender == NULL )
        return;
    
    UFC::AnsiString HHMMSS;
    UFC::AnsiString YYYYMMDD;    
    
    UFC::GetHHMMSS( HHMMSS );
    UFC::GetYYYYMMDD( YYYYMMDD );
    FOrderRender->SetField( "ITL020", "04" ); ///< Cancel
    FOrderRender->SetField( "ITL040", HHMMSS );
    FOrderRender->SetField( "ITL070", FIXMapper::BrokerID );
    FOrderRender->SetField( "ITL110120", FAccount );
    FOrderRender->SetField( "ITL140", GetSymbol() );
    FOrderRender->SetField( "ITL150", UFC::DoubleToInt( FPrice, 2 ) );
    FOrderRender->SetField( "ITL160", FQty / 1000 );
    FOrderRender->SetField( "ITL170", (FSide == FIX::Side_BUY) ? "B" : "S" );
    FOrderRender->SetField( "ITL520", FSalesID );
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
}
//------------------------------------------------------------------------------
void TSCTSEOrderRecord::ReduceString(  Int32 NID, Int32 NewQty, UFC::AnsiString& Reduce )
{
    if( FOrderRender == NULL )
	return;
    
    UFC::AnsiString HHMMSS;
    UFC::AnsiString YYYYMMDD;

    UFC::GetHHMMSS( HHMMSS );
    UFC::GetYYYYMMDD( YYYYMMDD );
    FOrderRender->SetField( "ITL020", "03" ); ///< Reduce
    FOrderRender->SetField( "ITL040", HHMMSS );
    FOrderRender->SetField( "ITL070", FIXMapper::BrokerID );
    FOrderRender->SetField( "ITL110120", FAccount.SubString( 0,7 ) );
    FOrderRender->SetField( "ITL140", GetSymbol() );
    FOrderRender->SetField( "ITL150", UFC::DoubleToInt( FPrice, 2 ) );
    FOrderRender->SetField( "ITL160", NewQty / 1000 );
    FOrderRender->SetField( "ITL170", (FSide == FIX::Side_BUY) ? "B" : "S" );
    FOrderRender->SetField( "ITL520", FSalesID );
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
BOOL TSCTSEConfirmParser::Parse( MarketKind Market, const UFC::AnsiString& Confirm )
{
    UFC::TRecord* pRecord;
    UFC::AnsiString Value, Value2, WebID, Symbol;
    
    FErrorMsg = "000000";
    FErrorCode = 0;
    FQty       = 0;
    FOrderID   = "00000";    
    
    if( ( pRecord = FOrderMKParser.Parse( Confirm.c_str(), Confirm.Length() )) != NULL )
    {
        pRecord->GetField("ITR580", WebID );
        if( WebID != FIXMapper::WebID )
        {
            UFC::BufferedLog::DebugPrintf( " Mapper's WebID:[%s] Confirm WebID:[%s]", FIXMapper::WebID.c_str(), WebID.c_str() );
            return false;
        }
        
        pRecord->GetField( "ITR140", Symbol ); 
        Symbol.TrimRight();
        UFC::BufferedLog::DebugPrintf( " Symbol:[%s]", Symbol.c_str() );
        
        pRecord->GetField("ITR050", Value);
	FErrorCode = Value.ToInt();
        pRecord->GetField( "ITR090", Value);
        pRecord->GetField( "ITR100", Value2 );
	FOrderID   = Value + Value2;
        pRecord->GetField( "ITR600", FErrorMsg );
        pRecord->GetField( "ITR020", Value );
        switch( Value.ToInt() )
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
        
        FQty *= 1000;        
        return true;
    }
    else
    {
        UFC::BufferedLog::Printf( "Parsing Confirm fail, drop it." );
        return false;
    }
}
//------------------------------------------------------------------------------
BOOL TSCTSEExecutionParser::Parse( MarketKind Market, const UFC::AnsiString& Execution )
{
    if( Market != mkTSE || Market != mkOTC )
       return FALSE;

    UFC::AnsiString WebID, Value, Value2, Symbol;
    UFC::TRecord*   pRecord = FFillParser.Parse( Execution.c_str(), Execution.Length() );
    TradeUnit* UnitRecord;
    Int32 Unit = 1000;
    
    if( pRecord != NULL )
    {
        pRecord->GetField( "ITM580", WebID );
        if( WebID != FIXMapper::WebID  )
            return FALSE;
        
        FExecutionType = etFilledSingle;

        pRecord->GetField( "ITM080", Symbol );
        Symbol.TrimRight();
        UFC::BufferedLog::DebugPrintf( " Symbol:[%s]", Symbol.c_str() );
        if( ( UnitRecord = TSCTSEOrderRecord::FTradeUnitTable.GetObjectByKey( Symbol ) ) != NULL )
            Unit = UnitRecord->GetTradeUnit();
        
        pRecord->GetField( "ITM140", Value );
        pRecord->GetField( "ITM150", Value2 );
        FOrderID       = Value + Value2;

        pRecord->GetField( "ITM210", Value );
        FExecutionSeq  = Value.ToInt();

        pRecord->GetField( "ITM100", Value );
        FPrice         = Value.ToInt();

        pRecord->GetField( "ITM090", Value );
        FQty           = Value.ToInt();
        FQty *= Unit;
        UFC::BufferedLog::DebugPrintf( " Qty:[%d] TradeUnit:[%d]", FQty, Unit );
        
        pRecord->GetField( "ITM130", Value );
        FSide          = ( Value == "B" )?(FIX::Side_BUY):(FIX::Side_SELL);

        pRecord->GetField( "ITM050", Value );
        FErrorCode     = Value.ToInt();

        pRecord->GetField( "ITM540", Value );
        FOrigNID       = Value.ToInt();
        pRecord->GetField( "ITM160", Value );
        pRecord->GetField( "ITM170", Value2 );
        FAccount       = Value + Value2;
        
        FExecID.Printf( "%s%s%s%08d%08d", FAccount.c_str(), FOrderID.c_str(), WebID.c_str(), FOrigNID, FExecutionSeq );
        UFC::BufferedLog::DebugPrintf( " ExecID:[%s]", FExecID.c_str());
    }
    if( WebID == FIXMapper::WebID ) ///< Web Code match TT2.
        FForceProcess = TRUE;
    else
    {
        FForceProcess = FALSE;
        return FALSE;
    }
	return TRUE;
}
//------------------------------------------------------------------------------

