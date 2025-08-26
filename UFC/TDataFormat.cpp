#include "TDataFormat.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
TDataFormat::TDataFormat(TDataFormatType FormatType)
:FFormatType(FormatType)
{
	FFieldOffset.SetOwnObjects( TRUE );
}
//---------------------------------------------------------------------------
void TDataFormat::ClearField()
{
	FFieldOffset.Clear();
	FFieldSchema.Clear();
}
//---------------------------------------------------------------------------
void TDataFormat::AddField( const AnsiString& FieldName, Int32 StartAt, Int32 Size, TFieldDataType DataType, const AnsiString& DefaultValue )
{
	FFieldOffset.Add( new TFieldOffset( StartAt, Size ) );

    TFieldSetting* pFieldSetting    = FFieldSchema.GetFieldSetting( FieldName, true );
    pFieldSetting->FDataType        = DataType;
    pFieldSetting->FDefaultValue    = DefaultValue;
}
//---------------------------------------------------------------------------
TDataset* TDataFormat::NewDataset()
{
	if( FFieldSchema.GetFieldCount() > 0 )
		return ( new TDataset( &FFieldSchema ) );

	return NULL;
}
//---------------------------------------------------------------------------
Int32 TDataFormat::GetFieldSize(Int32 FieldIndex)
{
  TFieldOffset* pFieldOffset = FFieldOffset.GetItem( FieldIndex );

  if( pFieldOffset != NULL )
  	return pFieldOffset->FSize;

  return 0;
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

