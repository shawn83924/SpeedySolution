#include "TField.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
TFieldSchema::TFieldSchema()
{
	FFieldList.SetOwnObjects( TRUE );
}
//---------------------------------------------------------------------------
TFieldSchema::~TFieldSchema()
{
}
//---------------------------------------------------------------------------
void TFieldSchema::Clear()
{
	FFieldList.Clear();
	FNameHash.Clear();
}
//---------------------------------------------------------------------------
TFieldSetting* TFieldSchema::GetFieldSetting(Int32 FieldIndex)
{
	if( FieldIndex >= 0 && FieldIndex < GetFieldCount() )
		return FFieldList.GetItem( FieldIndex );
	
	return NULL;
}
//---------------------------------------------------------------------------
TFieldSetting* TFieldSchema::GetFieldSetting( const AnsiString& FieldName, bool CreateNewIfNotFound )
{
    TFieldSetting* pFieldSetting = FNameHash.GetObjectByKey( FieldName );

    if( (pFieldSetting == NULL) && CreateNewIfNotFound )
    {
        pFieldSetting = new TFieldSetting( GetFieldCount(), FieldName );
		FFieldList.Add( pFieldSetting );
		FNameHash.Add( FieldName, pFieldSetting );
    }
	return pFieldSetting;
}
//---------------------------------------------------------------------------
Int32 TFieldSchema::GetFieldIndex(const AnsiString& FieldName)
{
	TFieldSetting* pField = GetFieldSetting( FieldName );
	
	if( pField != NULL  )
		return pField->FIndex;
	
	return -1;
}
//---------------------------------------------------------------------------
const AnsiString& TFieldSchema::GetFieldName(Int32 FieldIndex)
{
	static const AnsiString EmptyString;
	TFieldSetting* pField = GetFieldSetting( FieldIndex );
	if( pField != NULL )
		return pField->FName;
	return EmptyString;
}
//---------------------------------------------------------------------------
TFieldDataType TFieldSchema::GetDataType(Int32 FieldIndex)
{
	TFieldSetting* pField = GetFieldSetting( FieldIndex );
	
	if( pField != NULL  )
		return pField->FDataType;
	
	return FDT_STRING;
}
//---------------------------------------------------------------------------
TFieldDataType TFieldSchema::GetDataType(const AnsiString& FieldName)
{
	TFieldSetting* pField = GetFieldSetting( FieldName );
	
	if( pField != NULL  )
		return pField->FDataType;
	
	return FDT_STRING;
}
//---------------------------------------------------------------------------
const AnsiString& TFieldSchema::GetDefaultValue(Int32 FieldIndex)
{
	static const AnsiString EmptyString;
	TFieldSetting* pField = GetFieldSetting( FieldIndex );
	if( pField != NULL )
		return pField->FDefaultValue;
	return EmptyString;
}
//---------------------------------------------------------------------------
const AnsiString& TFieldSchema::GetDefaultValue(const AnsiString& FieldName)
{
	static const AnsiString EmptyString;
	TFieldSetting* pField = GetFieldSetting( FieldName );
	if( pField != NULL )
		return pField->FDefaultValue;
	return EmptyString;
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

