#ifndef DataStore_FieldH
#define DataStore_FieldH
//---------------------------------------------------------------------------
#include "./Exception.h"
//---------------------------------------------------------------------------
namespace nsDataStore
{
//---------------------------------------------------------------------------
typedef enum
{
	ftString = 0,
	ftInt    = 1,
	ftDouble = 2
	
} enumFieldType;
//---------------------------------------------------------------------------
class TFieldProperties
{
public:
	UFC::AnsiString FName;
	enumFieldType   FType;
	bool            FUnique;
	bool            FIndex;
	int             FMaxLength;
};
//---------------------------------------------------------------------------
class TField
{
private:
	TFieldProperties* fProperties;
	UInt8*            fData;

public:
	TField( TFieldProperties* pProperties, UInt8* Data );
	virtual ~TField( void );

public:
	const UFC::AnsiString ToString( void );
	const int             ToInt( void );
	const double          ToDouble( void );

public:
	void FromString( const UFC::AnsiString& Data );
	void FromInt( const int Data );
	void FromDouble( const double Data );

public:
	bool IsUnique( void );
	bool IsIndex( void );
};
//---------------------------------------------------------------------------
}
#endif
