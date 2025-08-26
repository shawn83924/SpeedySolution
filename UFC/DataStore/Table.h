#ifndef DataStore_TableH
#define DataStore_TableH
//---------------------------------------------------------------------------
#include "./Record.h"
//---------------------------------------------------------------------------
namespace nsDataStore
{
//---------------------------------------------------------------------------
typedef UFC::PHashedList<UFC::AnsiString, TRecord*> TStringHashList;
typedef UFC::PHashedList<int,             TRecord*> TIntHashList;
typedef UFC::PHashedList<double,          TRecord*> TDoubleHashList;
//---------------------------------------------------------------------------
class TRecordEnumerator
{
friend class TTable;

private:
	UFC::PtrList<TRecord> FList;
	
public:
	TRecordEnumerator( void );
	virtual ~TRecordEnumerator( void );
	
public:
	int     Count( void );
	TRecord Get( int Index );
};
//------------------------------------------------------------------------------
class TTable
{
private:
	UFC::PCriticalSection FCS;
	UFC::FileStreamEx*    FFile;
	TSchema               FSchema;
	int                   FDefaultSeqNum;
	TIntHashList          FRecordIDHash;

private:
	UFC::PHashedList<UFC::AnsiString, TStringHashList*> FUniqueStringHash;
	UFC::PHashedList<UFC::AnsiString, TIntHashList*>    FUniqueIntHash;
	UFC::PHashedList<UFC::AnsiString, TDoubleHashList*> FUniqueDoubleHash;

	UFC::PHashedList<UFC::AnsiString, TStringHashList*> FIndexStringHash;
	UFC::PHashedList<UFC::AnsiString, TIntHashList*>    FIndexIntHash;
	UFC::PHashedList<UFC::AnsiString, TDoubleHashList*> FIndexDoubleHash;

public:
	TTable( const UFC::AnsiString& Name );
	virtual ~TTable( void );

private:
	void             Initial( const UFC::AnsiString& FileName );
	void             BuildRecordLists( void );
	TStringHashList* GetStringHashList( TFieldProperties* pProp );
	TIntHashList*    GetIntHashList( TFieldProperties* pProp );
	TDoubleHashList* GetDoubleHashList( TFieldProperties* pProp );
	void             AddIntoRecordList( const TRecord& Record );
	void             RemoveFromRecordList( const TRecord& Record );
	void             CheckDuplicateRecord( const TRecord& Record );
        
public:
	TRecord GetRecordByUniqueField( const UFC::AnsiString& FieldName, const UFC::AnsiString& FieldValue );
	TRecord GetRecordByUniqueField( const UFC::AnsiString& FieldName, int FieldValue );
	TRecord GetRecordByUniqueField( const UFC::AnsiString& FieldName, double FieldValue );
	TRecord GetRecordByIndexField( const UFC::AnsiString& FieldName, const UFC::AnsiString& FieldValue );
	TRecord GetRecordByIndexField( const UFC::AnsiString& FieldName, int FieldValue );
	TRecord GetRecordByIndexField( const UFC::AnsiString& FieldName, double FieldValue );

public:
	void    Update( const TRecord& Record );
	int     Update( const TRecord& Record, UFC::AnsiString& ErrorMessage );
	TRecord NewRecord( void );
	int     GetRecordCount( void );
	TRecord GetRecordByID( int RecordID );
	void    EnumerateRecord( TRecordEnumerator& RecordEnumerator );        
        void    CompactDataFile( void );
};//class TTable
//---------------------------------------------------------------------------
};//namespace nsDataStore
#endif

