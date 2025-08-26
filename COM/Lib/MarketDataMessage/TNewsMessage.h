//---------------------------------------------------------------------------

#ifndef TNewsMessageH
#define TNewsMessageH
//---------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#include "TMDBaseMessage.h"
//--------------------------------------------------------------------------
class TNewsMessage : public TMdMessage
{
public:
	TNewsMessage();
public:
	~TNewsMessage();
public:
	const char*	GetHeadline( void );
	const char*	GetText( void );
	long        GetID( void );
	long        GetMsgCount( void );
	long        GetMsgNum( void );
	const char* GetMarket( void );

	void  		SetMarket( const char* Market );
	void		SetHeadline( const char* Headline );
	void		SetText( const char* Text );
	void        SetID( long UID );
	void        SetMsgCount( long count );
	void        SetMsgNum( long index );
};
#endif
