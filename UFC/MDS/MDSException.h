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
	virtual ~MDSException( void ) UFC_NOEXCEPT;
	virtual const char* what( void ) const UFC_NOEXCEPT;
public:
	void Throw( const char* FormatStr, ... );
};
//------------------------------------------------------------------------------------------------------------------------
class MDSInitialFailException : public MDSException
{
public:
	MDSInitialFailException( void ) {}
	virtual ~MDSInitialFailException( void ) UFC_NOEXCEPT {}
};
//------------------------------------------------------------------------------------------------------------------------
class MDSColumnNotFoundException : public MDSException
{
public:
	MDSColumnNotFoundException( void ) {}
	virtual ~MDSColumnNotFoundException( void ) UFC_NOEXCEPT {}
};
//------------------------------------------------------------------------------------------------------------------------
class MDSFileNotFoundException : public MDSException
{
public:
	MDSFileNotFoundException( void ) {}
	virtual ~MDSFileNotFoundException( void ) UFC_NOEXCEPT {}
};
//------------------------------------------------------------------------------------------------------------------------
class MDSLengthErrorException : public MDSException
{
public:
	MDSLengthErrorException( void ) {}
	virtual ~MDSLengthErrorException( void ) UFC_NOEXCEPT {}
};
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
