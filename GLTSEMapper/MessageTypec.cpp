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
void OrcTFXMapper::SendSecurityDefinitionFUT( const AnsiString & IniFilePath,const FIX44::SecurityDefinitionRequest & Request )
{
	FIX44::SecurityDefinition SecurityDefinition;
	SecurityDefinitionParser  sdp( IniFilePath );
	TRecord*				  rec;
	Int32					  RecCount = sdp.GetRecordCount();
	UFC::AnsiString			  _55,_15502,_231,SecurityResponseID;

	SecurityDefinition.setField( 22, "8" );
	SecurityDefinition.setField( 323, "4" );
	SecurityDefinition.setField( 320, Request.getField( 320 ) );
	for( register Int32 i = 0; i < RecCount; i++ )
    {
         rec = sdp.GetRecord( i );
         UFC::AnsiString CommId( rec->GetField( 0 ) );
         UFC::AnsiString SettMonth( rec->GetField( 1 ) );
         UFC::AnsiString RefPrice( rec->GetField( 2 ) );
         UFC::AnsiString EndDate( rec->GetField( 3 ) );
         UFC::AnsiString _48 = CommId + SettMonth;
         double Num = ( double )( RefPrice.ToInt() );

         CommId.TrimRight();
		 _55 = TFXSymbolUtility.FuturesTradeSymbolToMarketDataSymbol( CommId );
		 if( TFXSymbolUtility.FuturesPriceDigi( CommId ) == 3 )
		 {
			 _15502.Printf( "%.3f", (Num / 1000.0)+0.0001 );
			 _231.Printf( "%d.000",TFXSymbolUtility.ContractMultiplier( CommId, mkFutures));
		 }
		 else
		 {
			 _15502.Printf( "%.2f", (Num / 100.0)+0.001 );
			 _231.Printf( "%d.00",TFXSymbolUtility.ContractMultiplier( CommId, mkFutures));
		 }
         SecurityResponseID.Printf( "FAT_%d", i );
         SecurityDefinition.setField( 461, "FXXXXX" );
         SecurityDefinition.setField( 322, SecurityResponseID.c_str() );
         SecurityDefinition.setField( 48, _48.c_str() );;
         SecurityDefinition.setField( 55, _55.c_str() );
         SecurityDefinition.setField( 15502, _15502.c_str() );
         SecurityDefinition.setField( 541, EndDate.c_str() );
		 SecurityDefinition.setField( 231,_231.c_str());
         Application->SendToimFIX( FMsgSource, SecurityDefinition );
	}
}
//---------------------------------------------------------------------------
void OrcTFXMapper::SendSecurityDefinitionOPT( const AnsiString & IniFilePath,const FIX44::SecurityDefinitionRequest & Request )
{
	FIX44::SecurityDefinition SecurityDefinition;
	SecurityDefinitionParser  sdp( IniFilePath );
	TRecord *                 rec;
	Int32                     RecCount = sdp.GetRecordCount();
	UFC::AnsiString           _202,_461,_15502,_231,SecurityResponseID;

	SecurityDefinition.setField( 22, "8" );
	SecurityDefinition.setField( 323, "4" );
	SecurityDefinition.setField( 320, Request.getField( 320 ) );
    for( register Int32 i = 0; i < RecCount; i++ )
    {
         rec = sdp.GetRecord( i );
         UFC::AnsiString CommId( rec->GetField( 0 ) );
         UFC::AnsiString StrikePrice( rec->GetField( 1 ) );
         UFC::AnsiString CP( rec->GetField( 2 ) );
         UFC::AnsiString EndDate( rec->GetField( 3 ) );
         UFC::AnsiString Premium( rec->GetField( 4 ) );
         UFC::AnsiString _48( CommId );
         UFC::AnsiString _55( CommId.SubString( 0, 3 ) );
         double Num202 = ( double )( StrikePrice.ToInt() );
         double Num15502 = ( double )( Premium.ToInt() );

         _202.Printf( "%.3f", ( Num202 / 1000.0 )+0.0001 );
         _15502.Printf( "%.3f", ( Num15502 / 1000.0)+0.0001 );
		 _231.Printf( "%d.000",TFXSymbolUtility.ContractMultiplier( CommId, mkOptions));
         if( CP == "P" )
             _461 = "OPEXXX";///< Puts
		 else
			 _461 = "OCEXXX";///< Call
         SecurityResponseID.Printf( "OPT_%d", i );
         SecurityDefinition.setField( 322, SecurityResponseID.c_str() );
         SecurityDefinition.setField( 48,    _48.c_str() );
         SecurityDefinition.setField( 55,    _55.c_str() );
         SecurityDefinition.setField( 15502, _15502.c_str() );
         SecurityDefinition.setField( 461,   _461.c_str() );
         SecurityDefinition.setField( 202,   _202.c_str() );
		 SecurityDefinition.setField( 541,   EndDate.c_str() );
		 SecurityDefinition.setField( 231, _231.c_str());
         Application->SendToimFIX( FMsgSource, SecurityDefinition );
	}
}

//---------------------------------------------------------------------------
