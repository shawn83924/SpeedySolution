#ifndef TRecordH
#define TRecordH
//---------------------------------------------------------------------------
#include "TField.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
class TRecord
{
private:
	TFieldSchema*		                                FRefFieldSchema;
	PtrList<AnsiString>	                                FValueList;
	Int32				                                FFieldCount;

public:
	TRecord( TFieldSchema* RefFieldSchema );
	virtual ~TRecord();

private:
    void            AddFieldValues();

public:
	void            Reset();
	bool            SetField(Int32 FieldIndex, const AnsiString& Value);
	bool            SetField(const AnsiString& FieldName, const AnsiString& Value);

	Int32		    GetFieldCount() { return FFieldCount; }
	bool	        GetField( Int32 FieldIndex, AnsiString& Value );
	bool        	GetField( const AnsiString& FieldName, AnsiString& Value );
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
