//---------------------------------------------------------------------------
#include "OrcTSEMapper.h"
#include "SecurityDefinitionParser.h"
#include "../UFC/TParseData.h"
//---------------------------------------------------------------------------
//  Receive FIX message SecurityDefinitionRequest from counterparty
//
//  Transfer this FIX message to  TAIFEX order message  R010
//  and send to TAIFEX via Speedy.
//---------------------------------------------------------------------------
void OrcTSEMapper::onMessage( const FIX44::SecurityDefinitionRequest& Request, const FIX::SessionID & sessionID )
{
  UFC::BufferedLog::DebugPrintf(" OnFIX Message, Type = c.( Security Definition Request )");
  ParseT30V( Request );
  ParseT30OV( Request );
}
//---------------------------------------------------------------------------
void OrcTSEMapper::ParseT30V(const FIX44::SecurityDefinitionRequest& Request)
{
	UFC::TFixedFormat SecurityDefinitionFormat;
	UFC::BufferedLog::DebugPrintf(" ParseT30V");
    //SecurityDefinitionFormat.SetIniFile( "../cfg/Stock_T30V.ini" );
    UFC::BufferedLog::DebugPrintf(" SetIniFile");
	SecurityDefinitionParser  TSecurityDefinitionParser( "../cfg/Stock_T30V.ini" );
    //TSecurityDefinitionParser.SetDataFormat( &SecurityDefinitionFormat );

	SendSecurityDefinition( "TSE", Request.getField(320).c_str(), TSecurityDefinitionParser );
}
//---------------------------------------------------------------------------
void OrcTSEMapper::ParseT30OV(const FIX44::SecurityDefinitionRequest& Request)
{
	UFC::TFixedFormat SecurityDefinitionFormat;
    UFC::BufferedLog::DebugPrintf(" ParseT30OV");
	//SecurityDefinitionFormat.SetIniFile( "../cfg/Stock_T30OV.ini" );
	//UFC::TParseData SecurityDefinitionParser;
	//SecurityDefinitionParser.SetDataFormat( &SecurityDefinitionFormat );
    SecurityDefinitionParser TSecurityDefinitionParser("../cfg/Stock_T30OV.ini");
    UFC::BufferedLog::DebugPrintf(" SendSecurityDefinition");
	SendSecurityDefinition( "OTC", Request.getField(320).c_str(), TSecurityDefinitionParser );
}
//---------------------------------------------------------------------------
void OrcTSEMapper::SendSecurityDefinition(const char* PrefixString, const char* RequestID, SecurityDefinitionParser &TSecurityDefinitionParser)
{

    Int32 RecCount = TSecurityDefinitionParser.GetRecordCount();
    UFC::BufferedLog::DebugPrintf(" Record Count = %d", RecCount);
    if( RecCount <= 0 )
        return;

	FIX44::SecurityDefinition	SecurityDefinitionResponse;
	UFC::TRecord*				rec;
	UFC::AnsiString				_48, _55, _15502, _541, _322;

	SecurityDefinitionResponse.setField( 22, "8" );
	SecurityDefinitionResponse.setField( 320, RequestID );
	SecurityDefinitionResponse.setField( 323, "4" );
	SecurityDefinitionResponse.setField( 326, "24" );
	SecurityDefinitionResponse.setField( 461, "ESXXXX" );

	for( register Int32 i = 0; i < RecCount; i++ )
	{
		rec = TSecurityDefinitionParser.GetRecord(i);

		_322.Printf( "%s_%d", PrefixString, i );
		rec->GetField("STOCK", _55);
        _55.TrimRight();
		_48 = _55;
		rec->GetField("CPRICE", _15502);
        _15502.TrimLeft();
		//_15502.Printf( "%.2f", (_15502.ToInt() / 100.0) + 0.001 );
		//_541 = rec->GetField("LDATE");             Stock don't need this field.

        UFC::BufferedLog::DebugPrintf(" Symbol = %s", _55.c_str());
		UFC::BufferedLog::DebugPrintf(" CPrice = %s", _15502.c_str());
        SecurityDefinitionResponse.setField( 322, _322.c_str() );
		SecurityDefinitionResponse.setField( 48, _48.c_str() );
		SecurityDefinitionResponse.setField( 55, _55.c_str() );
		SecurityDefinitionResponse.setField( 15502, _15502.c_str() );
		//SecurityDefinitionResponse.setField( 541, _541.c_str() );

		Application->SendToimFIX( FMsgSource, SecurityDefinitionResponse );
	}
}
//---------------------------------------------------------------------------
