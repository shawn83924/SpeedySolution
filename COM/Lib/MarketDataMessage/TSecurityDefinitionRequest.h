//---------------------------------------------------------------------------

#ifndef TSecurityDefinitionRequestH
#define TSecurityDefinitionRequestH
//---------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#include "TMDBaseMessage.h"
//--------------------------------------------------------------------------
class TSecurityDefinitionRequestMessage :
	public TMDBaseMessage
{
public:
	TSecurityDefinitionRequestMessage( void );
public:
	~TSecurityDefinitionRequestMessage( void );
public:
	void SetRequestMarket( const char* Market );
	const char* GetRequestMarket( void );
};
//--------------------------------------------------------------------------
#endif
