//---------------------------------------------------------------------------
#include "TTaifexConnection.h"
//---------------------------------------------------------------------------
//  Class FuturesSymbolUtility
//---------------------------------------------------------------------------
FuturesSymbolUtility::FuturesSymbolUtility()
{
	UFC::UDateTime Today;

	FBaseYear = (Today.getYear()/10)*10;
	FYearDigi = Today.getYear()%10;
	FOldToNewFutSymbols.Add("FITX",  new UFC::AnsiString("TXF"));
	FOldToNewFutSymbols.Add("FITE",  new UFC::AnsiString("EXF"));
	FOldToNewFutSymbols.Add("FITF",  new UFC::AnsiString("FXF"));
	FOldToNewFutSymbols.Add("FIMTX", new UFC::AnsiString("MXF"));
	FOldToNewFutSymbols.Add("FIMTX1", new UFC::AnsiString("MX1")); ///< for MasterLint weekly MXF
	FOldToNewFutSymbols.Add("FIMTX2", new UFC::AnsiString("MX2")); ///< for MasterLint weekly MXF
	FOldToNewFutSymbols.Add("FIMTX4", new UFC::AnsiString("MX4")); ///< for MasterLint weekly MXF
	FOldToNewFutSymbols.Add("FIMTX5", new UFC::AnsiString("MX5")); ///< for MasterLint weekly MXF
	FOldToNewFutSymbols.Add("FIT5",  new UFC::AnsiString("T5F"));
	FOldToNewFutSymbols.Add("FIGB",  new UFC::AnsiString("GBF"));
	FOldToNewFutSymbols.Add("FICP",  new UFC::AnsiString("CPF"));
	FOldToNewFutSymbols.Add("FIGD",  new UFC::AnsiString("GDF"));
	FOldToNewFutSymbols.Add("FIMS",  new UFC::AnsiString("MSF"));
	FOldToNewFutSymbols.Add("FIXI",  new UFC::AnsiString("XIF")); ///< for Diwa
	FOldToNewFutSymbols.Add("FIGT",  new UFC::AnsiString("GTF")); ///< for Diwa

	FNewToOldFutSymbols.Add("TXF", new UFC::AnsiString("FITX"));
	FNewToOldFutSymbols.Add("EXF", new UFC::AnsiString("FITE"));
	FNewToOldFutSymbols.Add("FXF", new UFC::AnsiString("FITF"));
	FNewToOldFutSymbols.Add("MXF", new UFC::AnsiString("FIMTX"));
	FNewToOldFutSymbols.Add("MX1", new UFC::AnsiString("FIMTX1")); ///< for MasterLint weekly MXF
	FNewToOldFutSymbols.Add("MX2", new UFC::AnsiString("FIMTX2")); ///< for MasterLint weekly MXF
	FNewToOldFutSymbols.Add("MX4", new UFC::AnsiString("FIMTX4")); ///< for MasterLint weekly MXF
	FNewToOldFutSymbols.Add("MX5", new UFC::AnsiString("FIMTX5")); ///< for MasterLint weekly MXF
	FNewToOldFutSymbols.Add("T5F", new UFC::AnsiString("FIT5"));
	FNewToOldFutSymbols.Add("GBF", new UFC::AnsiString("FIGB"));
    FNewToOldFutSymbols.Add("CPF", new UFC::AnsiString("FICP"));
    FNewToOldFutSymbols.Add("GDF", new UFC::AnsiString("FIGD"));
    FNewToOldFutSymbols.Add("MSF", new UFC::AnsiString("FIMS"));
    FNewToOldFutSymbols.Add("XIF", new UFC::AnsiString("FIXI")); ///< for Diwa
    FNewToOldFutSymbols.Add("GTF", new UFC::AnsiString("FIGT")); ///< for Diwa
}
//---------------------------------------------------------------------------
void FuturesSymbolUtility::ConvertToNewSymbol( UFC::AnsiString& Symbol,
											   const UFC::AnsiString& OldSymbol,
											   const UFC::AnsiString& Date )
{
    UFC::AnsiString* NewSymbol;
    ///< Symbol exists in old symbol table, and date string length =6 (YYYYMM)
    if( Date.Length() == 6 )
    {
        if( FOldToNewFutSymbols.GetObjectByKey( OldSymbol, NewSymbol ) == TRUE )///< It's an Old Symbol!
        {
            char Month = 'A' + ( Date.SubString(4, 2).ToInt() - 1 );
            Symbol.Printf("%s%c%c", NewSymbol->c_str(), Month, Date[3] );
			return;
        }
        else
        {
            if( OldSymbol.Length() == 3 )///< It's a new Symbol.
            {
                char Month = 'A' + ( Date.SubString(4, 2).ToInt() - 1 );
                Symbol.Printf("%s%c%c", OldSymbol.c_str(), Month, Date[3] );
                return;
			}
        }
    }
    Symbol = OldSymbol;
}
//---------------------------------------------------------------------------
void FuturesSymbolUtility::ConvertToOldSymbol( const UFC::AnsiString& New,
											   UFC::AnsiString& Old,
											   UFC::AnsiString& Date )
{
	if( New.Length() == 5 ) ///< single order
	{
		UFC::AnsiString NewSymbol = New.SubString( 0 , 3 );
		UFC::AnsiString* OldSymbol;

		if( FNewToOldFutSymbols.GetObjectByKey( NewSymbol, OldSymbol ) == TRUE )
			Old = *OldSymbol; ///< It's a old Futures symbol
		else
			Old = NewSymbol;
		int Year;
		int YearDigi = New[4] - '0';

		if( ( FYearDigi == 9 || FYearDigi == 8 ) && YearDigi == 0 ) ///< example This year is 2009
			Year = FBaseYear + YearDigi + 10;
		else if( FYearDigi == 0 && ( YearDigi == 9 || YearDigi == 8 ) ) ///< example This year is 2010
			Year = FBaseYear + YearDigi - 10;
		else
			Year = FBaseYear + YearDigi;
		int Month = New[3] - 'A' + 1;
		Date.Printf( "%04d%02d", Year, Month );
	}
	else ///< Multi-leg symbol or error symbol
	{
		Old  = New;
		Date = "";
	}
}
//---------------------------------------------------------------------------
