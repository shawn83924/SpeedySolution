/******************************************************************************
 * Module   : UException.h
 * Author   : Allen Lee
 * Modified : 2005/5/4
 * Purpose  : Unix Exception definition. 
 ******************************************************************************/
//------------------------------------------------------------------------------------------------------------------------
#ifndef __UIPCException_H
#define __UIPCException_H
//------------------------------------------------------------------------------------------------------------------------
#ifndef _WIN32
//------------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _AIX433
	#include <strings.h>
#endif
#include <exception>
#include <errno.h>
#include "AnsiString.h"
//------------------------------------------------------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------------------------------------------------
class UException : public std::exception
{
protected:
	int			FErrNo;
	AnsiString	FErrMsg;
public:
	UException() : FErrNo( 0 ), FErrMsg( "Unknow UFC framework Exception" ) {}
	UException( int ErrNo );
	UException( AnsiString ErrStr, int ErrNo );
	//UException( const char* ErrStr, int ErrNo );
	UException( AnsiString &ErrStr ) : FErrNo( 0 ), FErrMsg( ErrStr ) {}
	UException( const char* ErrStr ) : FErrNo( 0 ), FErrMsg( ErrStr ) {}
	virtual ~UException( ) throw() {}
	virtual const char *what( ) const throw();
};
//------------------------------------------------------------------------------------------------------------------------
class UIPCException : public UException
{
public:
	typedef enum {
		msgget, msgsnd, msgrcv, msgctl
	} FunctionType;
public:
	UIPCException( int ErrNo, FunctionType ftype );
	UIPCException( AnsiString &ErrStr ) : UException( ErrStr ) {}
	UIPCException( const char* ErrStr ) : UException( ErrStr ) {}
	virtual ~UIPCException() throw() {}
	int GetErrno( void ) { return FErrNo; }
};
//------------------------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
