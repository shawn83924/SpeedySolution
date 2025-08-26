//---------------------------------------------------------------------------

#ifndef TSecurityDefinitionH
#define TSecurityDefinitionH
//---------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#include "TMDBaseMessage.h"
//--------------------------------------------------------------------------
class TSecurityDefinitionMessage :
	public TMDBaseMessage
{
public:
	TSecurityDefinitionMessage( void );
public:
	~TSecurityDefinitionMessage( void );
public:
	void SetSecurityID( const char* SecurityID, int Index );
	void SetSymbol( const char* Symbol, int Index );
	void SetSymbolCount( int Count );
	void SetMaturityDate( int MaturityDate, int Index );
	void SetRefPrice( double RefPrice, int Index );
	void SetRiseLimitPrice( double RiseLimitPrice, int Index );
	void SetFallLimitPrice( double FallLimitPrice, int Index );
	void SetStrikePrice( int StrikePrice, int Index );

	const char* GetSecurityID( int Index );
	const char* GetSymbol( int Index );
	int GetMaturityDate( int Index );
	int GetSymbolCount( void );
	double GetRefPrice( int Index );
	double GetRiseLimitPrice( int Index );
	double GetFallLimitPrice( int Index );
	int GetStrikePrice( int Index );
};
//--------------------------------------------------------------------------
#endif
