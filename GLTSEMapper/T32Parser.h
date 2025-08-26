/* 
 * File:   T32Parser.h
 * Author: yuan
 *
 * Created on June 5, 2009, 12:34 PM
 */

#ifndef _T32PARSER_H
#define	_T32PARSER_H
#include "UFC.h"
#include "TParseData.h"
#include "TFixedFormat.h"
//------------------------------------------------------------------------------
class T32Parser : public UFC::TFixedFormat
{
private:
  UFC::AnsiString	FFilePath;
  UFC::TParseData	FParseData;
  UFC::TDataset*	FDataset;
  virtual void LoadConfigSettingEx( UFC::UiniFile* pIniFile );

public:
  T32Parser( const UFC::AnsiString & iniFilePath );
  UFC::TRecord* GetRecord( Int32 RecordIndex );
  Int32	GetRecordCount();
};
//------------------------------------------------------------------------------
class TradeUnit
{
private:
    UFC::AnsiString FSymbol;
    Int32 FTradeUnit;
    double FWarningHighPrice;
    double FWarningLowPrice;
public:
    TradeUnit( UFC::AnsiString Symbol, Int32 Unit, double WarningHighPrice, double WarningLowPrice );
    UFC::AnsiString& GetSymbol( void ) { return FSymbol; }
    Int32 GetTradeUnit( void ) { return FTradeUnit; }
    double GetWarningHighPrice( void ) { return FWarningHighPrice; }
    double GetWarningLowPrice( void ) { return FWarningLowPrice; }
};
//------------------------------------------------------------------------------
#endif	/* _T32PARSER_H */

