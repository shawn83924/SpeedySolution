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
	virtual ~DataStoreException( void ) UFC_NOEXCEPT;
	virtual const char* what( void ) const UFC_NOEXCEPT;

public:
	void Throw( const char* FormatStr, ... );
};
//---------------------------------------------------------------------------
class InitialFailException : public DataStoreException
{
public:
	InitialFailException( void ) {}
	virtual ~InitialFailException( void ) UFC_NOEXCEPT {}
};
//---------------------------------------------------------------------------
class DuplicateRecordException : public DataStoreException
{
public:
	DuplicateRecordException( void ) {}
	virtual ~DuplicateRecordException( void ) UFC_NOEXCEPT {}
};
//---------------------------------------------------------------------------
class NullUniqueFieldException : public DataStoreException
{
public:
	NullUniqueFieldException( void ) {}
	virtual ~NullUniqueFieldException( void ) UFC_NOEXCEPT {}
};
//---------------------------------------------------------------------------
class NotUniqueFieldException : public DataStoreException
{
public:
	NotUniqueFieldException( void ) {}
	virtual ~NotUniqueFieldException( void ) UFC_NOEXCEPT {}
};
//---------------------------------------------------------------------------
class NotIndexFieldException : public DataStoreException
{
public:
	NotIndexFieldException( void ) {}
	virtual ~NotIndexFieldException( void ) UFC_NOEXCEPT {}
};
//---------------------------------------------------------------------------
class FieldNotFoundException : public DataStoreException
{
public:
	FieldNotFoundException( void ) {}
	virtual ~FieldNotFoundException( void ) UFC_NOEXCEPT {}
};
//---------------------------------------------------------------------------
class RecordNotFoundException : public DataStoreException
{
public:
        RecordNotFoundException( void ) {}
	virtual ~RecordNotFoundException( void ) UFC_NOEXCEPT {}
};
//---------------------------------------------------------------------------
};//namespace nsDataStore
#endif
