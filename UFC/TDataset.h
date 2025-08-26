#ifndef TDatasetH
#define TDatasetH
//---------------------------------------------------------------------------
#include "TRecord.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
class TDataset
{
private:
	TFieldSchema*		    FRefFieldSchema;
	PtrList<TRecord>	    FRecords;
public:
	TDataset(TFieldSchema* RefFieldSchema);
	virtual ~TDataset() {}
public:
	TRecord*	NewRecord( BOOL Owned = TRUE );
	void		DeleteRecord(Int32 RecordIndex);
	void		DeleteRecord(TRecord* pRecord);
	void		DeleteAllRecords();
	TRecord*	GetRecord(Int32 RecordIndex);
	Int32		GetRecordCount() { return FRecords.ItemCount(); }
	Int32		GetFieldCount();
	Int32		GetFieldIndex(const AnsiString& FieldName);
	AnsiString	GetFieldName(Int32 FieldIndex);
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
