#include "Table.h"
namespace nsDataStore
{
//---------------------------------------------------------------------------
TRecordEnumerator::TRecordEnumerator( void )
{
}
//---------------------------------------------------------------------------
TRecordEnumerator::~TRecordEnumerator( void )
{
	FList.Clear();
}
//---------------------------------------------------------------------------
int TRecordEnumerator::Count( void )
{
	return FList.ItemCount();
}
//---------------------------------------------------------------------------
TRecord TRecordEnumerator::Get( int Index )
{
	return TRecord( FList.GetItem( Index ) );
}
//---------------------------------------------------------------------------
//
// TTable
//
//---------------------------------------------------------------------------
TTable::TTable( const UFC::AnsiString& Name )
:FFile( NULL )
,FSchema( Name )
,FDefaultSeqNum( 0 )
{
	try
	{
            UFC::AnsiString FileName;
            FileName.Printf( "%s.table", Name.c_str() );
            Initial( FileName );
	}
	catch( DataStoreException& ex )
	{
            InitialFailException ifex;
            ifex.Throw( ex.what() );
	}
	catch( UFC::Exception& ex )
	{
            InitialFailException ifex;
            ifex.Throw( "TTable::TTable\n %s\n", ex.what() );
	}
	catch( ... )
	{
            InitialFailException ifex;
            ifex.Throw( "TTable::TTable\n unknown error\n" );
	}
}
//---------------------------------------------------------------------------
TTable::~TTable( void )
{
    for( int i = FUniqueStringHash.ItemCount() - 1; i >= 0; i-- )
    {
        delete FUniqueStringHash.GetItem(i);
    }
    for( int i = FUniqueIntHash.ItemCount() - 1; i >= 0; i-- )
    {
        delete FUniqueIntHash.GetItem(i);
    }
    for( int i = FUniqueDoubleHash.ItemCount() - 1; i >= 0; i-- )
    {
        delete FUniqueDoubleHash.GetItem(i);
    }
    for( int i = FIndexStringHash.ItemCount() - 1; i >= 0; i-- )
    {
        delete FIndexStringHash.GetItem(i);
    }
    for( int i = FIndexIntHash.ItemCount() - 1; i >= 0; i-- )
    {
        delete FIndexIntHash.GetItem(i);
    }
    for( int i = FIndexDoubleHash.ItemCount() - 1; i >= 0; i-- )
    {
        delete FIndexDoubleHash.GetItem(i);
    }

    while( FDefaultSeqNum > 0 )
    {
        TRecord* pRecord = FRecordIDHash.GetObjectByKey( FDefaultSeqNum );
        if( pRecord != NULL )
        {
                delete pRecord;
        }
        FDefaultSeqNum--;
    }

    if( FFile != NULL )
        delete FFile;
}
//---------------------------------------------------------------------------
void TTable::Initial( const UFC::AnsiString& FileName )
{
    UFC::PLockObject LockObject( FCS );

    if( UFC::FileExists( FileName ) == false )
    {
            UFC::FileStreamEx CreateFile( FileName, "ab" );
            int Count = FSchema.FFields.ItemCount();
            int Size  = FSchema.FRecordSize;
            CreateFile.Write( &Count, sizeof(int) );
            CreateFile.Write( &Size, sizeof(int) );
    }

    FFile = new UFC::FileStreamEx( FileName, "rb+" );

    for( int i = 0; i < FSchema.FFields.ItemCount(); i++ )
    {
        TFieldProperties* pProp = FSchema.FFields.GetItem(i);
        if( pProp->FUnique )
        {
            switch( pProp->FType )
            {
                case ftString:
                    FUniqueStringHash.Add( pProp->FName, new TStringHashList() );
                    break;
                case ftInt:
                    FUniqueIntHash.Add( pProp->FName, new TIntHashList() );
                    break;
                case ftDouble:
                    FUniqueDoubleHash.Add( pProp->FName, new TDoubleHashList() );
                    break;
            }
        }
        if( pProp->FIndex )
        {
            switch( pProp->FType )
            {
                case ftString:
                    FIndexStringHash.Add( pProp->FName, new TStringHashList() );
                    break;
                case ftInt:
                    FIndexIntHash.Add( pProp->FName, new TIntHashList() );
                    break;
                case ftDouble:
                    FIndexDoubleHash.Add( pProp->FName, new TDoubleHashList() );
                    break;
            }
        }
    }

    BuildRecordLists();
}
//---------------------------------------------------------------------------
void TTable::BuildRecordLists( void )
{
    UInt8*           Data;
    int              FieldCount, RecordSize, ReadSize, FileOffset;
    UFC::AnsiString  FieldValue;

    FFile->Seek( 0, SEEK_SET );
    FFile->Read( &FieldCount, sizeof(int) );
    if( FieldCount != FSchema.FFields.ItemCount() )
    {
        InitialFailException ifex;
        ifex.Throw( "TTable::BuildRecordLists\n field count not match\n" );
    }

    FFile->Read( &RecordSize, sizeof(int) );
    if( RecordSize != FSchema.FRecordSize )
    {
        InitialFailException ifex;
        ifex.Throw( "TTable::BuildRecordLists\n record size not match\n" );
    }

    FileOffset = sizeof(int) * 2;

    while( true )
    {
        Data = new UInt8[ RecordSize ];
        ReadSize = FFile->Read( Data, RecordSize );

        if( ReadSize != RecordSize )
        {
            delete [] Data;
            if( ReadSize != 0 )
            {
                InitialFailException ifex;
                ifex.Throw( "TTable::BuildRecordLists\n bad data\n" );
            }
            else
            {
                break;
            }
        }

        if( ((*Data) & REC_STATE_DELETE) == 0 )
        {
            FDefaultSeqNum++;
            AddIntoRecordList( TRecord( new TRecordInstance( FDefaultSeqNum, &FSchema, Data, FileOffset ) ) );
        }
        else
        {
            delete [] Data;
        }

        FileOffset += RecordSize;
    }
}
//---------------------------------------------------------------------------
TStringHashList* TTable::GetStringHashList( TFieldProperties* pProp )
{
    if( pProp->FUnique )
        return FUniqueStringHash.GetObjectByKey( pProp->FName );
    else if( pProp->FIndex )
        return FIndexStringHash.GetObjectByKey( pProp->FName );
    return NULL;
}
//---------------------------------------------------------------------------
TIntHashList* TTable::GetIntHashList( TFieldProperties* pProp )
{
    if( pProp->FUnique )
        return FUniqueIntHash.GetObjectByKey( pProp->FName );
    else if( pProp->FIndex )
        return FIndexIntHash.GetObjectByKey( pProp->FName );
    return NULL;
}
//---------------------------------------------------------------------------
TDoubleHashList* TTable::GetDoubleHashList( TFieldProperties* pProp )
{
    if( pProp->FUnique )
        return FUniqueDoubleHash.GetObjectByKey( pProp->FName );
    else if( pProp->FIndex )
        return FIndexDoubleHash.GetObjectByKey( pProp->FName );
    return NULL;
}
//---------------------------------------------------------------------------
void TTable::CheckDuplicateRecord( const TRecord& Record )
{
    TFieldProperties* pProp;
    TStringHashList*  pStringHashList;
    TIntHashList*     pIntHashList;
    TDoubleHashList*  pDoubleHashList;
    UFC::AnsiString   StringValue;
    int               IntValue;
    double            DoubleValue;

    if( FRecordIDHash.GetObjectByKey( Record.GetRecordID() ) != NULL )
    {
        DuplicateRecordException drex;
        drex.Throw( "TTable::CheckDuplicateRecord\n recordid = %d\n", Record.GetRecordID() );
    }

    for( int i = 0; i < FSchema.FFields.ItemCount(); i++ )
    {
        pProp = FSchema.FFields.GetItem(i);
        switch( pProp->FType )
        {
            case ftString:
                pStringHashList = GetStringHashList( pProp );
                if( pStringHashList != NULL )
                {
                    StringValue = Record.GetFieldAsString( pProp->FName );
                    if( StringValue.Length() > 0 )
                    {
                        if( pStringHashList->GetObjectByKey(StringValue) != NULL )
                        {
                            DuplicateRecordException drex;
                            drex.Throw( "TTable::CheckDuplicateRecord\n uniquefield: name=[%s], stringvalue=[%s]\n", pProp->FName.c_str(), StringValue.c_str() );
                        }
                    }
                }
                break;

            case ftInt:
                pIntHashList = GetIntHashList( pProp );
                if( pIntHashList != NULL )
                {
                    IntValue = Record.GetFieldAsInt( pProp->FName );
                    if( pIntHashList->GetObjectByKey(IntValue) != NULL )
                    {
                        DuplicateRecordException drex;
                        drex.Throw( "TTable::CheckDuplicateRecord\n uniquefield: name=[%s], intvalue=[%d]\n", pProp->FName.c_str(), IntValue );
                    }
                }
                break;

            case ftDouble:
                pDoubleHashList = GetDoubleHashList( pProp );
                if( pDoubleHashList != NULL )
                {
                    DoubleValue = Record.GetFieldAsDouble( pProp->FName );
                    if( pDoubleHashList->GetObjectByKey(DoubleValue) != NULL )
                    {
                        DuplicateRecordException drex;
                        drex.Throw( "TTable::CheckDuplicateRecord\n uniquefield: name=[%s], doublevalue=[%lf]\n", pProp->FName.c_str(), DoubleValue );
                    }
                }
                break;
        }
    }
}
//---------------------------------------------------------------------------
void TTable::AddIntoRecordList( const TRecord& Record )
{
    CheckDuplicateRecord( Record );

    TFieldProperties* pProp;
    TStringHashList*  pStringHashList;
    TIntHashList*     pIntHashList;
    TDoubleHashList*  pDoubleHashList;
    UFC::AnsiString   StringValue;
    int               IntValue;
    double            DoubleValue;

    TRecord* pRecord = new TRecord( Record );
    FRecordIDHash.Add( pRecord->GetRecordID(), pRecord );

    for( int i = 0; i < FSchema.FFields.ItemCount(); i++ )
    {
        pProp = FSchema.FFields.GetItem(i);
        switch( pProp->FType )
        {
            case ftString:
                pStringHashList = GetStringHashList( pProp );
                if( pStringHashList != NULL )
                {
                    StringValue = pRecord->GetFieldAsString( pProp->FName );
                    if( StringValue.Length() > 0 )
                    {
                        pStringHashList->Add( StringValue, pRecord );
                    }
                    else if( pProp->FUnique )
                    {
                        NullUniqueFieldException nufex;
                        nufex.Throw( "TTable::AddIntoRecordList\n uniquefield: name=[%s]\n", pProp->FName.c_str() );
                    }
                }
                break;

            case ftInt:
                pIntHashList = GetIntHashList( pProp );
                if( pIntHashList != NULL )
                {
                    IntValue = pRecord->GetFieldAsInt( pProp->FName );
                    pIntHashList->Add( IntValue, pRecord );
                }
                break;

            case ftDouble:
                pDoubleHashList = GetDoubleHashList( pProp );
                if( pDoubleHashList != NULL )
                {
                    DoubleValue = pRecord->GetFieldAsDouble( pProp->FName );
                    pDoubleHashList->Add( DoubleValue, pRecord );
                }
                break;
        }
    }
}
//---------------------------------------------------------------------------
void TTable::RemoveFromRecordList( const TRecord& Record )
{
    TFieldProperties* pProp;
    TStringHashList*  pStringHashList;
    TIntHashList*     pIntHashList;
    TDoubleHashList*  pDoubleHashList;
    UFC::AnsiString   StringValue;
    int               IntValue;
    double            DoubleValue;

    TRecord* pRecord = FRecordIDHash.GetObjectByKey( Record.GetRecordID() );

    if( pRecord != NULL )
    {
        FRecordIDHash.DeleteByKey( pRecord->GetRecordID() );

        for( int i = 0; i < FSchema.FFields.ItemCount(); i++ )
        {
            pProp = FSchema.FFields.GetItem(i);
            switch( pProp->FType )
            {
                case ftString:
                    pStringHashList = GetStringHashList( pProp );
                    if( pStringHashList != NULL )
                    {
                        StringValue = pRecord->GetFieldAsString( pProp->FName );
                        if( StringValue.Length() > 0 )
                                pStringHashList->DeleteByKey( StringValue );
                    }
                    break;

                case ftInt:
                    pIntHashList = GetIntHashList( pProp );
                    if( pIntHashList != NULL )
                    {
                            IntValue = pRecord->GetFieldAsInt( pProp->FName );
                            pIntHashList->DeleteByKey( IntValue );
                    }
                    break;

                case ftDouble:
                    pDoubleHashList = GetDoubleHashList( pProp );
                    if( pDoubleHashList != NULL )
                    {
                            DoubleValue = pRecord->GetFieldAsDouble( pProp->FName );
                            pDoubleHashList->DeleteByKey( DoubleValue );
                    }
                    break;
            }
        }
        delete pRecord;
    }
}
//---------------------------------------------------------------------------
TRecord TTable::GetRecordByUniqueField( const UFC::AnsiString& FieldName, const UFC::AnsiString& FieldValue )
{
	TStringHashList* pStringHashList = FUniqueStringHash.GetObjectByKey( FieldName );

	if( pStringHashList == NULL )
        {
            NotUniqueFieldException nufex;
            nufex.Throw( "TTable::GetRecordByUniqueField\n fieldname=[%s]\n", FieldName.c_str() );
        }

	return TRecord( pStringHashList->GetObjectByKey( FieldValue ) );
}
//---------------------------------------------------------------------------
TRecord TTable::GetRecordByUniqueField( const UFC::AnsiString& FieldName, int FieldValue )
{
	TIntHashList* pIntHashList = FUniqueIntHash.GetObjectByKey( FieldName );

	if( pIntHashList == NULL )
        {
            NotUniqueFieldException nufex;
            nufex.Throw( "TTable::GetRecordByUniqueField\n fieldname=[%s]\n", FieldName.c_str() );
        }

	return TRecord( pIntHashList->GetObjectByKey( FieldValue ) );
}
//---------------------------------------------------------------------------
TRecord TTable::GetRecordByUniqueField( const UFC::AnsiString& FieldName, double FieldValue )
{
	TDoubleHashList* pDoubleHashList = FUniqueDoubleHash.GetObjectByKey( FieldName );

	if( pDoubleHashList == NULL )
        {
            NotUniqueFieldException nufex;
            nufex.Throw( "TTable::GetRecordByUniqueField\n fieldname=[%s]\n", FieldName.c_str() );
        }

	return TRecord( pDoubleHashList->GetObjectByKey( FieldValue ) );
}
//---------------------------------------------------------------------------
TRecord TTable::GetRecordByIndexField( const UFC::AnsiString& FieldName, const UFC::AnsiString& FieldValue )
{
	TStringHashList* pStringHashList = FIndexStringHash.GetObjectByKey( FieldName );

	if( pStringHashList == NULL )
        {
            NotIndexFieldException nifex;
            nifex.Throw( "TTable::GetRecordByIndexField\n fieldname=[%s]\n", FieldName.c_str() );
        }

	return TRecord( pStringHashList->GetObjectByKey( FieldValue ) );
}
//---------------------------------------------------------------------------
TRecord TTable::GetRecordByIndexField( const UFC::AnsiString& FieldName, int FieldValue )
{
	TIntHashList* pIntHashList = FIndexIntHash.GetObjectByKey( FieldName );

	if( pIntHashList == NULL )
        {
            NotIndexFieldException nifex;
            nifex.Throw( "TTable::GetRecordByIndexField\n fieldname=[%s]\n", FieldName.c_str() );
        }

	return TRecord( pIntHashList->GetObjectByKey( FieldValue ) );
}
//---------------------------------------------------------------------------
TRecord TTable::GetRecordByIndexField( const UFC::AnsiString& FieldName, double FieldValue )
{
	TDoubleHashList* pDoubleHashList = FIndexDoubleHash.GetObjectByKey( FieldName );

	if( pDoubleHashList == NULL )
        {
            NotIndexFieldException nifex;
            nifex.Throw( "TTable::GetRecordByIndexField\n fieldname=[%s]\n", FieldName.c_str() );
        }

	return TRecord( pDoubleHashList->GetObjectByKey( FieldValue ) );
}
//---------------------------------------------------------------------------
/*

  REC_STATE_DELETE----REC_STATE_NEW---------------------------------END1
  |                   |
 (N)                 (N)
  |                   |
  |                   RemoveFromRecordList----pRecord->WriteFile----END2
  |
  |
  |
  REC_STATE_NEW-------AddIntoRecordList-------pRecord->WriteFile----END3
  |		      |
 (N)                  (DuplicateRecordException)
  |		      |
  |		      throw DuplicateRecordException----------------END4
  |
  |
  |
  REC_STATE_MODIFY----pRecord->WriteFile----------------------------END5

*/
//---------------------------------------------------------------------------
void TTable::Update( const TRecord& Record )
{
    UFC::AnsiString errorMsg = "";
    int writeResult = 0;
    UFC::PLockObject LockObject( FCS );
    try
    {
        if( Record.CheckState( REC_STATE_DELETE ) )
        {
            if( Record.CheckState( REC_STATE_NEW ) )
            {
                return;//END1
            }
            else
            {
                RemoveFromRecordList( Record );
                writeResult = Record.WriteFile( FFile, errorMsg );//END2
            }
        }
        else
        {
            if( Record.CheckState( REC_STATE_NEW ) )
            {
                AddIntoRecordList( Record );
                writeResult = Record.WriteFile( FFile, errorMsg );//END3
            }
            else
            {
                writeResult = Record.WriteFile( FFile, errorMsg );//END5
            }
        }
        
        if( writeResult < 0 )
            UFC::BufferedLog::Printf("TTable::Update() failed:%d[%s]", writeResult, errorMsg.c_str());
    }
    catch( DuplicateRecordException& ex )
    {
        RemoveFromRecordList( Record );
        throw( ex );//END4
    }
    FFile->Flush();
}
//---------------------------------------------------------------------------
int TTable::Update( const TRecord& Record, UFC::AnsiString& ErrorMessage )
{
    UFC::PLockObject LockObject( FCS );
    int upcodeCode = 0;
    try
    {
        if( Record.CheckState( REC_STATE_DELETE ) )
        {
            if( Record.CheckState( REC_STATE_NEW ) )
                return upcodeCode;//END1
            else
            {
                RemoveFromRecordList( Record );
                upcodeCode = Record.WriteFile( FFile, ErrorMessage );//END2
            }
        }
        else
        {
            if( Record.CheckState( REC_STATE_NEW ) )
            {
                AddIntoRecordList( Record );
                upcodeCode = Record.WriteFile( FFile, ErrorMessage );//END3
            }
            else
                upcodeCode = Record.WriteFile( FFile, ErrorMessage );//END5
        }
    }
    catch( DuplicateRecordException& ex )
    {
        upcodeCode = -3;
        RemoveFromRecordList( Record );
        throw( ex );//END4
    }
    FFile->Flush();
    return upcodeCode;
}
//---------------------------------------------------------------------------
TRecord TTable::NewRecord( void )
{
	FDefaultSeqNum++;
	return TRecord( new TRecordInstance( FDefaultSeqNum, &FSchema, NULL, -1 ) );
}
//---------------------------------------------------------------------------
int TTable::GetRecordCount( void )
{
	return FRecordIDHash.ItemCount();
}
//---------------------------------------------------------------------------
TRecord TTable::GetRecordByID( int RecordID )
{
	return TRecord( FRecordIDHash.GetObjectByKey( RecordID ) );
}
//---------------------------------------------------------------------------
void TTable::EnumerateRecord( TRecordEnumerator& RecordEnumerator )
{
	UFC::PLockObject LockObject( FCS );

	RecordEnumerator.FList.Clear();

	for( int i = 1; i <= FDefaultSeqNum; i++ )
	{
		TRecord* pRecord = FRecordIDHash.GetObjectByKey( i );
		if( pRecord != NULL )
			RecordEnumerator.FList.Add( pRecord );
        }
}
//---------------------------------------------------------------------------
void TTable::CompactDataFile( void )
{
    UFC::PLockObject LockObject( FCS );
    
    UFC::AnsiString FileName( FFile->GetFileName() );
    delete FFile; // Close data file
    
    /* Create a new data file. */
    FFile = new UFC::FileStreamEx( FileName, "w" );
    int Count = FSchema.FFields.ItemCount();
    int Size  = FSchema.FRecordSize;
    FFile->Write( &Count, sizeof(int) );
    FFile->Write( &Size, sizeof(int) );
    delete FFile;
    
    /* change file open mode */
    FFile = new UFC::FileStreamEx( FileName, "rb+" );
    FFile->Seek( 0, SEEK_END );
    
    for( int i = 1; i <= FDefaultSeqNum; i++ )
    {
        TRecord* pRecord = FRecordIDHash.GetObjectByKey( i );
        if( pRecord != NULL )
        {
            pRecord->GetInstance()->SetFileOffset( FFile->GetPosition() );
            pRecord->WriteFile( FFile );
        }
    }
    
    FFile->Flush();
}
//---------------------------------------------------------------------------
}

