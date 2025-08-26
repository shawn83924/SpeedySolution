#ifndef DataStore_ExceptionH
#define DataStore_ExceptionH
//---------------------------------------------------------------------------
#include "../UFC.h"
//---------------------------------------------------------------------------
namespace nsDataStore
{
//---------------------------------------------------------------------------
class DataStoreException : public UFC::Exception
{
private:
	UFC::AnsiString FMessage;

public:
	DataStoreException( void );
	virtual ~DataStoreException( void ) throw();
	virtual const char* what( void ) const throw();

public:
	void Throw( const char* FormatStr, ... );
};
//---------------------------------------------------------------------------
class InitialFailException : public DataStoreException
{
public:
	InitialFailException( void ) {}
	virtual ~InitialFailException( void ) throw() {}
};
//---------------------------------------------------------------------------
class DuplicateRecordException : public DataStoreException
{
public:
	DuplicateRecordException( void ) {}
	virtual ~DuplicateRecordException( void ) throw() {}
};
//---------------------------------------------------------------------------
class NullUniqueFieldException : public DataStoreException
{
public:
	NullUniqueFieldException( void ) {}
	virtual ~NullUniqueFieldException( void ) throw() {}
};
//---------------------------------------------------------------------------
class NotUniqueFieldException : public DataStoreException
{
public:
	NotUniqueFieldException( void ) {}
	virtual ~NotUniqueFieldException( void ) throw() {}
};
//---------------------------------------------------------------------------
class NotIndexFieldException : public DataStoreException
{
public:
	NotIndexFieldException( void ) {}
	virtual ~NotIndexFieldException( void ) throw() {}
};
//---------------------------------------------------------------------------
class FieldNotFoundException : public DataStoreException
{
public:
	FieldNotFoundException( void ) {}
	virtual ~FieldNotFoundException( void ) throw() {}
};
//---------------------------------------------------------------------------
class RecordNotFoundException : public DataStoreException
{
public:
        RecordNotFoundException( void ) {}
	virtual ~RecordNotFoundException( void ) throw() {}
};
//---------------------------------------------------------------------------
};//namespace nsDataStore
#endif
