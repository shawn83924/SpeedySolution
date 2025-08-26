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
AnsiString TFieldSchema::GetFieldName(Int32 FieldIndex)
{
	TFieldSetting* pField = GetFieldSetting( FieldIndex );
	
	if( pField != NULL  )
		return pField->FName;
	
	return "";
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
AnsiString TFieldSchema::GetDefaultValue(Int32 FieldIndex)
{
	TFieldSetting* pField = GetFieldSetting( FieldIndex );
	
	if( pField != NULL  )
		return pField->FDefaultValue;
	
	return "";
}
//---------------------------------------------------------------------------
AnsiString TFieldSchema::GetDefaultValue(const AnsiString& FieldName)
{
	TFieldSetting* pField = GetFieldSetting( FieldName );
	
	if( pField != NULL  )
		return pField->FDefaultValue;
	
	return "";
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

