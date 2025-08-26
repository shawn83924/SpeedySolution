#ifdef _WIN32
#ifndef _TShareInteger_H_
#define _TShareInteger_H_
//--------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#include <UFC.h>
//--------------------------------------------------------------------------
class TShareInteger
{
private:
	int*    FInt32Ptr;
	HANDLE  FFileMap;

public:
	TShareInteger(void);
public:
	~TShareInteger(void);

private:
	bool OpenShareMemory( const char* ShareMemoryName );
	bool CreateShareMemory( const char* ShareMemoryName );
	bool MapIntegeAddress( void );
	void Detach( void );

public:
	void Attach( const char* ShareMemoryName );
	int  ToInt32( void );

public:
	TShareInteger& operator = ( int Val );
    TShareInteger& operator += ( int Val );
};
//--------------------------------------------------------------------------
#endif //#ifndef _TShareInteger_H_
#endif //#ifdef _WIN32
