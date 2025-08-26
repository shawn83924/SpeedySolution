#include "T32Parser.h"
//---------------------------------------------------------------------------
TradeUnit::TradeUnit( UFC::AnsiString Symbol, Int32 Unit, double WarningHighPrice, double WarningLowPrice )
:FSymbol( Symbol )
,FTradeUnit( Unit )
,FWarningHighPrice( WarningHighPrice )
,FWarningLowPrice( WarningLowPrice )
{}
//---------------------------------------------------------------------------
T32Parser::T32Parser( const UFC::AnsiString & iniFilePath )
:FFilePath( "" )
{
    SetIniFile( iniFilePath );
    UFC::FileStream fs( FFilePath, O_RDONLY );
    FParseData.SetDataFormat( this );
    FDataset = FParseData.Parse( &fs );
}
//---------------------------------------------------------------------------
void T32Parser::LoadConfigSettingEx( UFC::UiniFile * pIniFile )
{
    pIniFile->GetValue( this->GetDefaultConfigSectionName(), "FilePath", FFilePath );
}
//---------------------------------------------------------------------------
UFC::TRecord* T32Parser::GetRecord( Int32 RecordIndex )
{
    return FDataset->GetRecord( RecordIndex );
}
//---------------------------------------------------------------------------
Int32 T32Parser::GetRecordCount()
{
    return FDataset->GetRecordCount();
}
//---------------------------------------------------------------------------
