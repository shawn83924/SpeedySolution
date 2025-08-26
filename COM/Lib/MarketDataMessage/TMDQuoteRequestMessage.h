//---------------------------------------------------------------------------

#ifndef TMDQuoteRequestMessageH
#define TMDQuoteRequestMessageH
//---------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#include "TMDBaseMessage.h"
//--------------------------------------------------------------------------
class TMDQuoteRequestMessage :
	public TMDBaseMessage
{
public:
	TMDQuoteRequestMessage(void);
public:
	~TMDQuoteRequestMessage(void);
public:
	const char* GetSymbol( void );
	void 		SetSymbol( const char* Symbol );
	const char* GetSecurityID( void );
	void 		SetSecurityID( const char* SecurityID );

};
//--------------------------------------------------------------------------

#endif
