#ifndef __BO_SymbolDeCompose_H
#define __BO_SymbolDeCompose_H


#include "../UFC/UFC.h"
#include "../UFC/iniFile.h"
#include "../UFC/Utility.h"

//------------------------------------------------------------------------------------------------------------------------
// class BO_SymbolDeCompose
//------------------------------------------------------------------------------------------------------------------------
class BO_SymbolDeCompose
{
private:        
        static UFC::UiniFile StrikePxTable;
private:
	int				FCYear1Base;		// 2009 represent 9
        int				FCYear2Base;		// 2009 represent 20
	int				FCYear3Base;		// 2004 represent 200
	int				FNYear3Base;		// 2004 represent 201
public:
	typedef enum 
        {
            stSingle,
            stCallPriceSpread,
            stPutPriceSpread,
            stCallTimeSpread,
            stPutTimeSpread,
            stStraddle,
            stStrangle,
            stConRev,
            stFuturesTimeSpread,
            stFuturesSingle,
            stFuturesFLEXSingle,        
            stFLEXSingle,
            stUnKnown			// Invalid Symbol Format
                        
	} StrategicType;
public:
	StrategicType	FStrategicType;
	UFC::AnsiString	FSymbol;
	char		FBS_Code;
	char		FComm1[21];
	char		FComm2[21];
	char		FExcercisePrice1[10];	// 9(6)V999 length 9
	char		FExcercisePrice2[10];	// 9(6)V999
	char		FSettleMonth1[7];
	char		FSettleMonth2[7];
	char		FBS_Code1;				// 'B' or 'S'
	char		FBS_Code2;				// 'B' or 'S'
	char		FCP_Code1;				// 'C' or 'P'
	char		FCP_Code2;				// 'C' or 'P'
        BOOL            FIsFLEX;
private:
	void Initialize( void );
	BOOL GetExcercisePrice( const char* SymbolPos, const char* SymbolPricePos, char* ePrice );
        BOOL GetFLEXExcercisePrice( const char* SymbolPos, char* ePrice );
	BOOL GetMonthAndCP( const char* SymbolMonthPos, char* SettleMonth, char* CP_Code );
private:
        ///< For Options    
	BOOL SetSingle( void );
	BOOL SetPriceSpread( void );
	BOOL SetTimeSpread( void );
        BOOL SetWeeklyOPTimeSpread( void );        
	BOOL SetStraddle( void );
	BOOL SetStrangle( void );
	BOOL SetConRev( void );
        BOOL SetFLEXSingle( void );
        //< For Futures
        BOOL SetFuturesSingle( void );
        BOOL SetFuturesTimeSpread( void );
        BOOL SetWeeklyFUTTimeSpread( void );
        BOOL SetFuturesFLEXSingle( void );
public:
	BOOL SetSymbol( UFC::AnsiString Symbol, char BS_Code );
public:
	BO_SymbolDeCompose( UFC::AnsiString Symbol, char BS_Code );
	BO_SymbolDeCompose();
	~BO_SymbolDeCompose();
};
//------------------------------------------------------------------------------------------------------------------------
#endif


