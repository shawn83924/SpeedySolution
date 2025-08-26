//---------------------------------------------------------------------------
#include "OrcTFXMapper.h"
#include "SecurityDefinitionParser.h"
#include "../UFC/TParseData.h"
//---------------------------------------------------------------------------
//  Receive FIX message SecurityDefinitionRequest from counterparty
//
//  Transfer this FIX message to  TAIFEX order message  R010
//  and send to TAIFEX via Speedy.
//---------------------------------------------------------------------------
void OrcTFXMapper::onMessage( const FIX44::SecurityDefinitionRequest & Request, const FIX::SessionID & sessionID )
{
	SendSecurityDefinitionFUT( "../cfg/Futures_T30.ini", Request );
	SendSecurityDefinitionOPT( "../cfg/Options_P01.ini", Request );
}
//---------------------------------------------------------------------------
void OrcTFXMapper::SendSecurityDefinitionFUT( const UFC::AnsiString & IniFilePath,const FIX44::SecurityDefinitionRequest & Request )
{
	FIX44::SecurityDefinition SecurityDefinition;
	SecurityDefinitionParser  sdp( IniFilePath );

	UFC::AnsiString CommId, SettMonth, RefPrice, EndDate, UpperPrice, LowerPrice;
	UFC::AnsiString	_48, _55, _15500, _15501, _15502, _231, SecurityResponseID;

	SecurityDefinition.setField( 22, "8" );
	SecurityDefinition.setField( 323, "4" );
	SecurityDefinition.setField( 320, Request.getField( 320 ) );

	Int32 RecCount = sdp.GetRecordCount();
	for( register Int32 i = 0; i < RecCount; i++ )
	{
		UFC::TRecord* rec = sdp.GetRecord(i);

		rec->GetField( 0, CommId );
		rec->GetField( 1, SettMonth );
		rec->GetField( 2, RefPrice );
		rec->GetField( 3, EndDate );
        rec->GetField( "UpperPrice", UpperPrice );
        rec->GetField( "LowerPrice", LowerPrice );

        _48 = CommId + SettMonth;

		CommId.TrimRight();
		_55 = TFXSymbolUtility.FuturesTradeSymbolToMarketDataSymbol( CommId );

		if( TFXSymbolUtility.FuturesPriceDigi( CommId ) == 3 )
		{
            _15500.Printf( "%.3f", (FIX::DoubleConvertor::convert(UpperPrice.c_str()) / 1000.0) + 0.0001 );
            _15501.Printf( "%.3f", (FIX::DoubleConvertor::convert(LowerPrice.c_str()) / 1000.0) + 0.0001 );
			_15502.Printf( "%.3f", (FIX::DoubleConvertor::convert(RefPrice.c_str()) / 1000.0) + 0.0001 );
			_231.Printf( "%d.000", TFXSymbolUtility.ContractMultiplier( CommId, mkFutures) );
		}
		else
		{
            _15500.Printf( "%.2f", (FIX::DoubleConvertor::convert(UpperPrice.c_str()) / 100.0) + 0.001 );
            _15501.Printf( "%.2f", (FIX::DoubleConvertor::convert(LowerPrice.c_str()) / 100.0) + 0.001 );
			_15502.Printf( "%.2f", (FIX::DoubleConvertor::convert(RefPrice.c_str()) / 100.0) + 0.001 );
			_231.Printf( "%d.00", TFXSymbolUtility.ContractMultiplier( CommId, mkFutures));
		}

		SecurityResponseID.Printf( "FAT_%d", i );
		SecurityDefinition.setField( 461, "FXXXXX" );
		SecurityDefinition.setField( 322, SecurityResponseID.c_str() );
		SecurityDefinition.setField( 48, _48.c_str() );
		SecurityDefinition.setField( 55, _55.c_str() );
        SecurityDefinition.setField( 15500, _15500.c_str() );
        SecurityDefinition.setField( 15501, _15501.c_str() );
		SecurityDefinition.setField( 15502, _15502.c_str() );
		SecurityDefinition.setField( 541, EndDate.c_str() );
		SecurityDefinition.setField( 231,_231.c_str());
		Application->SendToimFIX( FMsgSource, SecurityDefinition );
	}
}
//---------------------------------------------------------------------------
void OrcTFXMapper::SendSecurityDefinitionOPT( const UFC::AnsiString & IniFilePath,const FIX44::SecurityDefinitionRequest & Request )
{
	FIX44::SecurityDefinition SecurityDefinition;
	SecurityDefinitionParser  sdp( IniFilePath );

	UFC::AnsiString CommId, StrikePrice, CP, EndDate, Premium, UpperPrice, LowerPrice;
	UFC::AnsiString _48, _55, _202, _231, _461, _15500, _15501, _15502, SecurityResponseID;

	SecurityDefinition.setField( 22, "8" );
	SecurityDefinition.setField( 323, "4" );
	SecurityDefinition.setField( 320, Request.getField( 320 ) );

	Int32 RecCount = sdp.GetRecordCount();
    for( register Int32 i = 0; i < RecCount; i++ )
    {
         UFC::TRecord * rec = sdp.GetRecord(i);

		 rec->GetField( 0, CommId );
		 rec->GetField( 1, StrikePrice );
		 rec->GetField( 2, CP );
		 rec->GetField( 3, EndDate );
		 rec->GetField( 4, Premium );
         rec->GetField( "UpperPrice", UpperPrice );
         rec->GetField( "LowerPrice", LowerPrice );

		 _48 = CommId;
		 _55 = CommId.SubString( 0, 3 );
		 _461 = ( CP == "P" ) ? "OPEXXX" : "OCEXXX";
         _202.Printf( "%.3f", ( FIX::DoubleConvertor::convert(StrikePrice.c_str()) / 1000.0 )+0.0001 );
         _15500.Printf( "%.3f", ( FIX::DoubleConvertor::convert(UpperPrice.c_str()) / 1000.0)+0.0001 );
         _15501.Printf( "%.3f", ( FIX::DoubleConvertor::convert(LowerPrice.c_str()) / 1000.0)+0.0001 );
         _15502.Printf( "%.3f", ( FIX::DoubleConvertor::convert(Premium.c_str()) / 1000.0)+0.0001 );
		 _231.Printf( "%d.000", TFXSymbolUtility.ContractMultiplier( CommId, mkOptions) );

         SecurityResponseID.Printf( "OPT_%d", i );
         SecurityDefinition.setField( 322, SecurityResponseID.c_str() );
         SecurityDefinition.setField( 48, _48.c_str() );
         SecurityDefinition.setField( 55, _55.c_str() );
         SecurityDefinition.setField( 15500, _15500.c_str() );
         SecurityDefinition.setField( 15501, _15501.c_str() );
         SecurityDefinition.setField( 15502, _15502.c_str() );
         SecurityDefinition.setField( 461, _461.c_str() );
         SecurityDefinition.setField( 202, _202.c_str() );
		 SecurityDefinition.setField( 541, EndDate.c_str() );
		 SecurityDefinition.setField( 231, _231.c_str());

         Application->SendToimFIX( FMsgSource, SecurityDefinition );
	}
}

//---------------------------------------------------------------------------
