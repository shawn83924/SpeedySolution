#include "../UFC/UFC.h"
//------------------------------------------------------------------------------------------------------------------------
// class BO_SymbolDeCompose
//------------------------------------------------------------------------------------------------------------------------
class BO_SymbolDeCompose
{
private:
	int				FCYear1Base;		// 2009 represent 9
	int				FCYear3Base;		// 2004 represent 200
	int				FNYear3Base;		// 2004 represent 201
public:
	typedef enum {
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
		stUnKnown			// Invalid Symbol Format
                        
	} StrategicType;
public:
	StrategicType	FStrategicType;
	UFC::AnsiString	FSymbol;
	char		FBS_Code;
	char		FComm1[11];
	char		FComm2[11];
	char		FExcercisePrice1[10];	// 9(6)V999
	char		FExcercisePrice2[10];	// 9(6)V999
	char		FSettleMonth1[7];
	char		FSettleMonth2[7];
	char		FBS_Code1;				// 'B' or 'S'
	char		FBS_Code2;				// 'B' or 'S'
	char		FCP_Code1;				// 'C' or 'P'
	char		FCP_Code2;				// 'C' or 'P'
private:
	void Initialize( void );
	BOOL GetExcercisePrice( const char* SymbolPos, const char* SymbolPricePos, char* ePrice );
	BOOL GetMonthAndCP( const char* SymbolMonthPos, char* SettleMonth, char* CP_Code );
private:
        ///< For Options
	BOOL SetSingle( void );
	BOOL SetPriceSpread( void );
	BOOL SetTimeSpread( void );
	BOOL SetStraddle( void );
	BOOL SetStrangle( void );
	BOOL SetConRev( void );
        //< For Futures
        BOOL SetFuturesSingle( void );
        BOOL SetFuturesTimeSpread( void );
public:
	BOOL SetSymbol( UFC::AnsiString Symbol, char BS_Code );
public:
	BO_SymbolDeCompose( UFC::AnsiString Symbol, char BS_Code );
	BO_SymbolDeCompose();
	~BO_SymbolDeCompose();
};
//------------------------------------------------------------------------------------------------------------------------
