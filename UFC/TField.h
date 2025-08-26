#ifndef TFieldH
#define TFieldH
#include "List.h"
#include "PHashedList.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
typedef enum
{
	FDT_STRING,
	FDT_INT
    
} TFieldDataType;
//---------------------------------------------------------------------------
class TFieldSetting
{
public:
	Int32           FIndex;
	AnsiString	    FName;
	TFieldDataType	FDataType;
	AnsiString	    FDefaultValue;

public:
	TFieldSetting( Int32 FieldIndex, const AnsiString& FieldName, TFieldDataType DataType = FDT_STRING, const AnsiString& DefaultValue = "" )
	:FIndex(FieldIndex), FName(FieldName), FDataType(DataType), FDefaultValue(DefaultValue) {}
};
//---------------------------------------------------------------------------
class TFieldSchema
{
private:
	PtrList<TFieldSetting>					        FFieldList;
	PHashedList<AnsiString, TFieldSetting*>	FNameHash;

public:
	TFieldSchema();
	virtual ~TFieldSchema();

private:

public:
	void			    Clear();
	TFieldSetting*      GetFieldSetting(Int32 FieldIndex);
	TFieldSetting*      GetFieldSetting(const AnsiString& FieldName, bool CreateNewIfNotFound = false);
	Int32			    GetFieldIndex(const AnsiString& FieldName);
	AnsiString		GetFieldName(Int32 FieldIndex);
	TFieldDataType	    GetDataType(Int32 FieldIndex);
	TFieldDataType	    GetDataType(const AnsiString& FieldName);
	AnsiString		GetDefaultValue(Int32 FieldIndex);
	AnsiString		GetDefaultValue(const AnsiString& FieldName);
	Int32			    GetFieldCount() { return FFieldList.ItemCount(); }
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
