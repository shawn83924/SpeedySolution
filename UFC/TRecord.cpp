#include "TRecord.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
TRecord::TRecord( TFieldSchema* RefFieldSchema )
:FRefFieldSchema( RefFieldSchema )
,FFieldCount(0)
{
    FValueList.SetOwnObjects( TRUE );
    AddFieldValues();
    Reset();
}
//---------------------------------------------------------------------------
TRecord::~TRecord()
{
    FValueList.Clear();
}
//---------------------------------------------------------------------------
void TRecord::AddFieldValues()
{
	FValueList.Clear();

	if( FRefFieldSchema != NULL )
	{
        FFieldCount = FRefFieldSchema->GetFieldCount();
		for( register Int32 i = 0;i < FFieldCount; i++ )
		{
            FValueList.Add( new AnsiString( FRefFieldSchema->GetDefaultValue(i) ) );
        }
    }
}
//---------------------------------------------------------------------------
void TRecord::Reset()
{
    for( register Int32 i = 0;i < FFieldCount; i++ )
    {
        *( FValueList.GetItem(i) ) = FRefFieldSchema->GetDefaultValue(i);
    }
}
//---------------------------------------------------------------------------
bool TRecord::GetField( Int32 FieldIndex, AnsiString& Value )
{
    if( FieldIndex >= 0 && FieldIndex < FFieldCount )
    {
        AnsiString* pField = FValueList.GetItem( FieldIndex );
        if( pField != NULL /*&&  pField->Length() > 0*/ )
        {
            Value = *pField;
            return true;
        }
    }
    Value = "";
    return false;
}
//---------------------------------------------------------------------------
bool TRecord::SetField(Int32 FieldIndex, const AnsiString& Value)
{
	if( FieldIndex >= 0 && FieldIndex < FFieldCount )
	{
		AnsiString* pField = FValueList.GetItem( FieldIndex );
		if( pField != NULL /*&& pField->Length() > 0*/ )
		{
			*pField = Value;
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool TRecord::GetField( const AnsiString& FieldName, AnsiString& Value )
{
	return GetField( FRefFieldSchema->GetFieldIndex( FieldName ), Value );
}
//---------------------------------------------------------------------------
bool TRecord::SetField(const AnsiString& FieldName, const AnsiString& Value)
{
	return SetField( FRefFieldSchema->GetFieldIndex( FieldName ), Value );
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

