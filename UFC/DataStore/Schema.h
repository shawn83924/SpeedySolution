#ifndef DataStore_SchemaH
#define DataStore_SchemaH
//---------------------------------------------------------------------------
#include "./Field.h"
#include "../iniFile.h"
//---------------------------------------------------------------------------
namespace nsDataStore
{
//---------------------------------------------------------------------------
class TSchema
{
public:
	int                            FRecordSize;
	UFC::PtrList<TFieldProperties> FFields;

public:
	TSchema( const UFC::AnsiString& Name );
	virtual ~TSchema( void );

private:
	enumFieldType ReadType( UFC::Section* pSection );
	int           ReadMaxLength( UFC::Section* pSection );
	bool          ReadUnique( UFC::Section* pSection );
	bool          ReadIndex( UFC::Section* pSection );
	void          AddFieldProperties( const UFC::AnsiString& Name, enumFieldType Type, int MaxLength, bool Unique, bool Index );
	void          Initial( const UFC::AnsiString& Name );
};//class TSchema
//---------------------------------------------------------------------------
};//namespace nsDataStore
#endif

