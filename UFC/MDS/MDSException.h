//------------------------------------------------------------------------------------------------------------------------
#ifndef __MDSException_H
#define __MDSException_H
//------------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
class MDSException : public UFC::Exception
{
private:
	UFC::AnsiString FMessage;
public:
	MDSException( void );
	virtual ~MDSException( void ) throw();
	virtual const char* what( void ) const throw();
public:
	void Throw( const char* FormatStr, ... );
};
//------------------------------------------------------------------------------------------------------------------------
class MDSInitialFailException : public MDSException
{
public:
	MDSInitialFailException( void ) {}
	virtual ~MDSInitialFailException( void ) throw() {}
};
//------------------------------------------------------------------------------------------------------------------------
class MDSColumnNotFoundException : public MDSException
{
public:
	MDSColumnNotFoundException( void ) {}
	virtual ~MDSColumnNotFoundException( void ) throw() {}
};
//------------------------------------------------------------------------------------------------------------------------
class MDSFileNotFoundException : public MDSException
{
public:
	MDSFileNotFoundException( void ) {}
	virtual ~MDSFileNotFoundException( void ) throw() {}
};
//------------------------------------------------------------------------------------------------------------------------
class MDSLengthErrorException : public MDSException
{
public:
	MDSLengthErrorException( void ) {}
	virtual ~MDSLengthErrorException( void ) throw() {}
};
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
