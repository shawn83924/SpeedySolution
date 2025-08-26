//---------------------------------------------------------------------------

#ifndef TMDBaseMessageH
#define TMDBaseMessageH
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
#include "../MessageDeliver/TMdMessage.h"
//---------------------------------------------------------------------------
class TMDBaseMessage : public TMdMessage
{
public:
	TMDBaseMessage();
	~TMDBaseMessage();
public:
	void  		SetMarket( const char* Market );
	const char* GetMarket( void );
};

#endif
