#ifndef SecurityDefinitionParserH
#define SecurityDefinitionParserH
//---------------------------------------------------------------------------
#include "../UFC/TFixedFormat.h"
#include "../UFC/TParseData.h"
//---------------------------------------------------------------------------
class SecurityDefinitionParser : public UFC::TFixedFormat
{
private:
  UFC::AnsiString	FFilePath;
  UFC::TParseData	FParseData;
  UFC::TDataset*	FDataset;

  virtual void LoadConfigSettingEx(UFC::UiniFile* pIniFile);

public:
  SecurityDefinitionParser(const UFC::AnsiString & iniFilePath);
  UFC::TRecord* GetRecord(Int32 RecordIndex);
  Int32			GetRecordCount();
};
//---------------------------------------------------------------------------
#endif
