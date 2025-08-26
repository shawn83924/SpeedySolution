#include "TDataset.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
TDataset::TDataset(TFieldSchema* RefFieldSchema)
:FRefFieldSchema(RefFieldSchema)
{
    FRecords.SetOwnObjects( TRUE );
}
//---------------------------------------------------------------------------
Int32 TDataset::GetFieldCount()
{
    if( FRefFieldSchema != NULL )
	return FRefFieldSchema->GetFieldCount();
    return 0;
}
//---------------------------------------------------------------------------
TRecord* TDataset::NewRecord( BOOL Owned )
{
    TRecord* rec = NULL;

    if( FRefFieldSchema != NULL )
    {
	rec = new TRecord( FRefFieldSchema );
        if( Owned == TRUE )
            FRecords.Add( rec );
    }
    return rec;
}
//---------------------------------------------------------------------------
void TDataset::DeleteRecord(Int32 RecordIndex)
{
    if( RecordIndex >= 0 && RecordIndex < FRecords.ItemCount() )
    {
	TRecord* pRecord = FRecords.Delete( RecordIndex );

	if( pRecord != NULL )
            delete pRecord;
    }
}
//---------------------------------------------------------------------------
void TDataset::DeleteRecord(TRecord* pRecord)
{
    if( pRecord != NULL )
    {
	FRecords.Remove( pRecord );
	delete pRecord;
    }
}
//---------------------------------------------------------------------------
void TDataset::DeleteAllRecords()
{
    FRecords.Clear();
}
//---------------------------------------------------------------------------
TRecord* TDataset::GetRecord(Int32 RecordIndex)
{
    return FRecords.GetItem( RecordIndex );
}
//---------------------------------------------------------------------------
Int32 TDataset::GetFieldIndex(const AnsiString& FieldName)
{
    if( FRefFieldSchema!= NULL )
	return FRefFieldSchema->GetFieldIndex( FieldName );
    return -1;
}
//---------------------------------------------------------------------------
AnsiString TDataset::GetFieldName(Int32 FieldIndex)
{
    if( FRefFieldSchema!= NULL )
	return FRefFieldSchema->GetFieldName( FieldIndex );
    return "";
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

