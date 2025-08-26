#include "SecurityDefinitionParser.h"
//---------------------------------------------------------------------------
SecurityDefinitionParser::SecurityDefinitionParser(const UFC::AnsiString & iniFilePath)
:FFilePath( "" )
{
	SetIniFile( iniFilePath );
	UFC::FileStream fs( FFilePath, O_RDONLY );
	FParseData.SetDataFormat( this );
	FParseData.Parse( &fs );
}
//---------------------------------------------------------------------------
void SecurityDefinitionParser::LoadConfigSettingEx( UFC::UiniFile * pIniFile )
{
  pIniFile->GetValue( this->GetDefaultConfigSectionName(), "FilePath", FFilePath );
}
//---------------------------------------------------------------------------
UFC::TRecord * SecurityDefinitionParser::GetRecord( Int32 RecordIndex )
{
  return FParseData.GetRecord( RecordIndex );
}
//---------------------------------------------------------------------------
Int32 SecurityDefinitionParser::GetRecordCount()
{
  return FParseData.GetRecordCount();
}
//---------------------------------------------------------------------------
