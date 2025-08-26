#ifndef DataStore_RecordH
#define DataStore_RecordH
//---------------------------------------------------------------------------
#include "./Schema.h"
//---------------------------------------------------------------------------
namespace nsDataStore
{
//---------------------------------------------------------------------------
const UInt8 REC_STATE_NEW    = 0x01;
const UInt8 REC_STATE_MODIFY = REC_STATE_NEW << 1;
const UInt8 REC_STATE_DELETE = REC_STATE_NEW << 2;
//---------------------------------------------------------------------------
class TRecordInstance
{
friend class TRecord;
        UFC::PCriticalSection FCS;
private:
	int      FID;
	int      FRef;
	int      FFileOffset;
	TSchema* fSchema;
	UInt8*   FData;
	UFC::PHashedList<UFC::AnsiString, TField*> FFieldHashList;

private:
	void    Initial( void );
	TField* GetField( const UFC::AnsiString& FieldName );
	UInt8   GetState( void );
	void    SetState( UInt8 Value );
	bool    CanModifyField( TField* pField );

public:
	TRecordInstance( int RecordID, TSchema* pSchema, UInt8* pData, int FileOffset );
	virtual ~TRecordInstance( void );
        void SetFileOffset( Int32 Offset );
        int IncreaseRef( void );
        int DecreaseRef( void );
        int GetRefCount( void ) { return FRef; }
};//class TRecordInstance
//---------------------------------------------------------------------------
class TRecord
{
private:
	TRecordInstance* FInstance;
        
public:
	TRecord( TRecordInstance* pInstance );
	TRecord( const TRecord& Record );
	TRecord( TRecord* pRecord );
	TRecord( void );
	virtual ~TRecord( void );

public:
	TRecord& operator = ( const TRecord& Record );
	
private:
	void            IncreaseReferenceCount( void );
	void            DecreaseReferenceCount( void );

public:
	UFC::AnsiString GetFieldAsString( const UFC::AnsiString& FieldName ) const;
	int             GetFieldAsInt( const UFC::AnsiString& FieldName ) const;
	double          GetFieldAsDouble( const UFC::AnsiString& FieldName ) const;
        
	void            SetField( const UFC::AnsiString& FieldName, const UFC::AnsiString& FieldValue );
	void            SetField( const UFC::AnsiString& FieldName, const int FieldValue );
	void            SetField( const UFC::AnsiString& FieldName, const double FieldValue );

public:
	void            Delete( void );
	bool            WriteFile( UFC::FileStreamEx* fs ) const;
	int             WriteFile( UFC::FileStreamEx* fs, UFC::AnsiString& ErrorMsg ) const;
	bool            CheckState( UInt8 Mask ) const;
	int             GetRecordID( void ) const;
        TRecordInstance* GetInstance() { return FInstance; } 
};//class TRecord
//---------------------------------------------------------------------------
};//namespace nsDataStore
#endif

