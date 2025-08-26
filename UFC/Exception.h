/**@file Exception.h
*  @brief header file for exception classes.
*  @author Simon Chaang
*
*/
//--------------------------------------------------------------------------------------------------------------------
#ifndef _EXCEPTION_H
#define _EXCEPTION_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#include <string.h>
#ifdef _AIX433
    #include <strings.h>
#endif
#include <stdexcept>
#include <errno.h>
#include <sstream>
#include "AnsiString.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const int ERR_MSG_MAX = 512;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Exception : public std::exception
{
public:
	char Message[ ERR_MSG_MAX + 1 ];
public:
	Exception( const char* ErrStr ){ strncpy( Message, ErrStr, ERR_MSG_MAX ); }
	//Exception( const wchar_t* ErrStr ):Message( ErrStr ){}
        Exception( const UFC::AnsiString& ErrStr ){ strncpy( Message, ErrStr.c_str(), ERR_MSG_MAX ); }
        Exception( void ) { Message[ 0 ] = 0; }
	virtual ~Exception() throw(){}
	virtual const char *what()  const throw()
	{
            return Message;
	}
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class AllocException : public UFC::Exception
{
public:
    AllocException( ):Exception( "Alloc memory failed" ){}
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class FileException : public UFC::Exception
{
public:
    FileException( const char* Str ):Exception( Str ){}
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class IndexOutOfBoundException : public UFC::Exception
{
public:
    IndexOutOfBoundException( int Index ){ ErrorToWhat( Index ); }
private:    
    void ErrorToWhat( int Index );
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ObjectNotFoundException : public UFC::Exception
{
public:
    ObjectNotFoundException( ):Exception( "Object not in the container." ){}
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ThreadException : public UFC::Exception
{
public:
    ThreadException( int Error ){ ErrorToWhat( Error ); }
private:    
    void ErrorToWhat( int Error );
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SocketException : public UFC::Exception
{
public:
    SocketException( int Error ){ ErrorToWhat( Error ); }
    SocketException( const char* ErrStr ):Exception( ErrStr ){}
private:    
    void ErrorToWhat( int Error );
};
//---------------------------------------------------------------------------
class ShareMenException : public UFC::Exception
{
public:
    ShareMenException( const char* errmsg ):Exception( errmsg ){}
    ShareMenException( int Error ){ ErrorToWhat( Error ); }
private:    
    void ErrorToWhat( int Error );
};
//---------------------------------------------------------------------------
class MutexException : public UFC::Exception
{
public:
    MutexException( const char* errmsg ):Exception( errmsg ){}
    MutexException( int Error ){ ErrorToWhat( Error ); }
private:    
    void ErrorToWhat( int Error );
};
//---------------------------------------------------------------------------
class ConditionVariableException : public UFC::Exception
{
public:
    ConditionVariableException( const char* errmsg ):Exception( errmsg ){}
    ConditionVariableException( int Error ){ ErrorToWhat( Error ); }
private:    
    void ErrorToWhat( int Error );
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class QueueException : public UFC::Exception
{
public:
    QueueException( int Error ){ ErrorToWhat( Error ); }
private:
    void ErrorToWhat( int Error );
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

