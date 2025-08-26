//------------------------------------------------------------------------------------------------------------------------
#include "MTable.h"
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
//
//  Win32 not define strkon_r, copy src from FreeBSD
//
//------------------------------------------------------------------------------------------------------------------------
#ifdef _WIN32
char* strtok_r(char *s1, const char *s2, char **lasts)
{
	char *ret;

	if( s1 == NULL)
		s1 = *lasts;
	while( *s1 && strchr(s2, *s1) )
    	++s1;
	if( *s1 == '\0')
		return NULL;
	ret = s1;
	while( *s1 && !strchr(s2, *s1))
		++s1;
	if( *s1 )
		*s1++ = '\0';
	*lasts = s1;
	return ret;
}
#endif
//------------------------------------------------------------------------------------------------------------------------
// class MTable
//------------------------------------------------------------------------------------------------------------------------
MTable::MTable( const UFC::AnsiString& Name,
				const UFC::AnsiString& SchemaDir,
				const UFC::AnsiString& TableDir,
				MTableListener* Listener,
				MTransactionStore* TransactionStore )
: FTableName( Name )
, FSchemaDir( SchemaDir )
, FTableDir( TableDir )
, FTableListener( Listener )
, FTransactionStore( TransactionStore )
, FTableSchema( Name, SchemaDir )
, FDefaultSeqNum( 0 )
, FPkIndex( NULL )
{
	// Register MTable
	if ( FTransactionStore != NULL )
		FTransactionStore->RegisterTable( this );

	FRowIDHash_CurrIter = FRowIDHash.begin();

	try
	{
		UFC::AnsiString FileNameTable;
		FileNameTable.Printf( "%s/%s.data", FTableDir.c_str(), Name.c_str() );

		CreateIndexCollection( );
		LoadFromDataFile( FileNameTable );
	}
	catch( MDSException& ex )
	{
		MDSInitialFailException exVar;
		exVar.Throw( ex.what() );
	}
	catch( UFC::Exception& ex )
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MTable::MTable -> %s", ex.what() );
	}
	catch( ... )
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MTable::MTable -> Unknow Exception" );
	}
}
//------------------------------------------------------------------------------------------------------------------------
MTable::~MTable( void )
{
	int i;
	TIntToMRowPtr::iterator RowIter;

	// delete all MIndexObjectUnique of FUniqueIndexsVector
	for ( i = (int)FUniqueIndexsVector.size() - 1; i >= 0; i-- )
		delete FUniqueIndexsVector[i];
	FUniqueIndexsVector.clear();
	FUniqueIndexs.clear();

	// delete all MIndexObjectMultiple of FMultipleIndexs
	for ( i = (int)FMultipleIndexsVector.size() - 1; i >= 0; i-- )
		delete FMultipleIndexsVector[i];
	FMultipleIndexsVector.clear();
	FMultipleIndexs.clear();

	// delete all element of FRowIDHash
	for ( RowIter = FRowIDHash.begin(); RowIter != FRowIDHash.end(); RowIter++ )
	{
		if ( FTableListener != NULL )
			FTableListener->OnDeleteRow( RowIter->second, NULL );
		delete RowIter->second;
	}
	FRowIDHash.clear();
}
//------------------------------------------------------------------------------------------------------------------------
void MTable::CreateIndexCollection( void )
{
	for ( int i = 0; i < FTableSchema.FNameToIndexSchemaMap.ItemCount(); i++ )
	{
		MIndexSchema* pIndexSchema;
		if ( FTableSchema.FNameToIndexSchemaMap.GetItem( i, pIndexSchema ) == FALSE )
			continue;
		if ( pIndexSchema == NULL )
			continue;

		if ( pIndexSchema->FIsUnique || pIndexSchema->FIsPK )
		{
			MIndexObjectUnique* pIndexObjectUnique;
			// Unique Index ( include PK index )
			if ( FUniqueIndexs.count( pIndexSchema->FIndexName ) == 0 )
			{
				pIndexObjectUnique = new MIndexObjectUnique( pIndexSchema, FTableSchema.FExpectRowAmount );
				FUniqueIndexs[ pIndexSchema->FIndexName ] = pIndexObjectUnique; ///< Add into FUniqueIndexs hash
				FUniqueIndexsVector.push_back( pIndexObjectUnique ); ///< Add into FUniqueIndexsVector
				if ( pIndexSchema->FIsPK )
					FPkIndex = pIndexObjectUnique;
			}
		}
		else
		{
			MIndexObjectMultiple* pIndexObjectMultiple;
			// Multiple Index
			if ( FMultipleIndexs.count( pIndexSchema->FIndexName ) == 0 )
			{
				pIndexObjectMultiple = new MIndexObjectMultiple( pIndexSchema, FTableSchema.FExpectRowAmount );
				FMultipleIndexs[ pIndexSchema->FIndexName ] = pIndexObjectMultiple; ///< Add into FMultipleIndexs hash
				FMultipleIndexsVector.push_back( pIndexObjectMultiple ); ///< Add into FMultipleIndexsVector
			}
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MTable::LoadFromDataFile( const UFC::AnsiString& FileName )
{
	char			TextBuffer[8192];
	FILE*			FileHandle;

	if ( ( FileHandle = fopen( FileName.c_str(), "r" ) ) == NULL )
	{
		MDSFileNotFoundException exVar;
		exVar.Throw( "MTable::LoadFromDataFile -> File [%s] not found", FileName.c_str() );
	}
	int LineNo = 0;
	while ( fgets( TextBuffer, 8192, FileHandle ) != NULL )
	{
		MRow Row;
		LineNo++; ///< counting line no

		if ( FTableSchema.FIsFixedDataFile == TRUE ) ///< Fixed data file
		{
			int Len = strlen( TextBuffer );
			// Check if Text length < FFixedDataTotalLength then ignore this record
			if ( Len < FTableSchema.FFixedDataTotalLength )
				continue;

			if ( NewRowByFixedTextData( Row, TextBuffer ) == FALSE )
			{
				UFC::BufferedLog::Printf( " MTable::LoadFromDataFile() File [%s] Bad TextData(Line:%d) -> <%s>"
										, FileName.c_str()
										, LineNo
										, TextBuffer );
				continue;
			}
		}
		else ///< Delimiter data file
		{
			// Check if Text length < delimiter then ignore this record
			UFC::AnsiString BufferStr( TextBuffer );
			BufferStr.TrimRight();
			if ( BufferStr.Length() < (FTableSchema.FColumnDefines.ItemCount() - 1) )
				continue;

			if ( NewRowByTextData( Row, TextBuffer ) == FALSE )
			{
				UFC::BufferedLog::Printf( " MTable::LoadFromDataFile() File [%s] Bad TextData(Line:%d) -> <%s>"
										, FileName.c_str()
										, LineNo
										, TextBuffer );
				continue;
			}
		}

		if ( AddIntoRowList( Row, NULL ) == FALSE )
		{
			UFC::BufferedLog::Printf( " MTable::LoadFromDataFile() File [%s] Row duplicate(Line:%d) -> <%s>"
									  , FileName.c_str()
									  , LineNo
									  , TextBuffer );
		}
		// Clear All Transaction state except ROW_STATE_DELETE
		Row.FInstance->SetState( Row.FInstance->GetState() & ROW_STATE_DELETE );
	}
	fclose( FileHandle );
}
//------------------------------------------------------------------------------------------------------------------------
void MTable::SetBinaryObject( MDSBinaryObj& BinaryObj, MIndexSchema* pIndexSchema, MRow* pRow )
{
	for ( int i = 0; i < pIndexSchema->FIndexColumnIdxList.ItemCount(); i++ )
	{
		int Idx = pIndexSchema->FIndexColumnIdxList.GetItem( i );
		MColumn* pColumn = pRow->GetColumn( Idx );
		if ( pColumn->FDefinition->FType == ctString )
			BinaryObj.AppendBuffer( (char*)pColumn->FData, (pColumn->FDefinition->FMaxLength - 1) );
		else
			BinaryObj.AppendBuffer( (char*)pColumn->FData, (pColumn->FDefinition->FMaxLength) );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MTable::AddToIndexObjectUnique( MIndexObjectUnique* pIndexObject, MRow* pRow )
{
	MIndexSchema* pIndexSchema = pIndexObject->FIndexSchema;
	MDSBinaryObj  BinaryObj( pIndexSchema->FIndexSize );

	SetBinaryObject( BinaryObj, pIndexSchema, pRow );
	pIndexObject->Add( BinaryObj, pRow );
}
//------------------------------------------------------------------------------------------------------------------------
void MTable::AddToIndexObjectMultiple( MIndexObjectMultiple* pIndexObject, MRow* pRow )
{
	MIndexSchema* pIndexSchema = pIndexObject->FIndexSchema;
	MDSBinaryObj  BinaryObj( pIndexSchema->FIndexSize );

	SetBinaryObject( BinaryObj, pIndexSchema, pRow );
	pIndexObject->Add( BinaryObj, pRow );
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::AddIntoRowList( MRow& Row, void* ExtraParm )
{
	// 1. Check if duplicate
	if ( IsDuplicateRow( Row ) )
		return FALSE;

	// 2. new MRow to keep MRowInstance reference
	MRow* pRow = new MRow( Row );

	int i;
	// 3. Build each Unique Index Collection
	for ( i = 0; i < (int)FUniqueIndexsVector.size(); i++ )
		AddToIndexObjectUnique( FUniqueIndexsVector[i], pRow );

	// 4. Build each Multiple Index Collection
	for ( i = 0; i < (int)FMultipleIndexsVector.size(); i++ )
		AddToIndexObjectMultiple( FMultipleIndexsVector[i], pRow );

	// 5. Build FRowIDHash Index
	FRowIDHash[ pRow->GetRowID() ] = pRow;

	// 6. Call MTableListener->OnNewRow() when have Listener
	if ( FTableListener != NULL )
		FTableListener->OnNewRow( pRow, ExtraParm );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void MTable::RemoveFromIndexObjectUnique( MIndexObjectUnique* pIndexObject, MRow* pRow )
{
	MIndexSchema* pIndexSchema = pIndexObject->FIndexSchema;
	MDSBinaryObj  BinaryObj( pIndexSchema->FIndexSize );

	SetBinaryObject( BinaryObj, pIndexSchema, pRow );
	pIndexObject->Delete( BinaryObj, pRow );
}
//------------------------------------------------------------------------------------------------------------------------
void MTable::RemoveFromIndexObjectMultiple( MIndexObjectMultiple* pIndexObject, MRow* pRow )
{
	MIndexSchema* pIndexSchema = pIndexObject->FIndexSchema;
	MDSBinaryObj  BinaryObj( pIndexSchema->FIndexSize );

	SetBinaryObject( BinaryObj, pIndexSchema, pRow );
	pIndexObject->Delete( BinaryObj, pRow );
}
//------------------------------------------------------------------------------------------------------------------------
void MTable::RemoveFromRowList( MRow& Row, void* ExtraParm )
{
	TIntToMRowPtr::iterator RowIter;
	// 1. get MRow* to delete MRowInstance reference
	if ( ( RowIter = FRowIDHash.find( Row.GetRowID() ) ) == FRowIDHash.end() )
		return;
	MRow* pRow = RowIter->second;

	int i;
	// 2. Erase each Unique Index Collection
	for ( i = 0; i < (int)FUniqueIndexsVector.size(); i++ )
		RemoveFromIndexObjectUnique( FUniqueIndexsVector[i], pRow );

	// 3. Erase each Multiple Index Collection
	for ( i = 0; i < (int)FMultipleIndexsVector.size(); i++ )
		RemoveFromIndexObjectMultiple( FMultipleIndexsVector[i], pRow );

	// 4. Erase FRowIDHash Index
	FRowIDHash.erase( pRow->GetRowID() );

	// 5. Call MTableListener->OnDeleteRow() when have Listener
	if ( FTableListener != NULL )
		FTableListener->OnDeleteRow( pRow, ExtraParm );

	// 6. delete MRow* reference
	delete pRow;
}
//------------------------------------------------------------------------------------------------------------------------
void MTable::Sync( char Action, MRow& Row, int TransID )
{
	// 1. Sync. to TransactionStore file if own
	if ( FTransactionStore != NULL )
		FTransactionStore->SyncSimpleAction( Action, FTableName, Row, TransID );

	// 2. Clear All Transaction state except ROW_STATE_DELETE
	Row.FInstance->SetState( Row.FInstance->GetState() & ROW_STATE_DELETE );
}
//------------------------------------------------------------------------------------------------------------------------
void MTable::DoStrAction( UFC::AnsiString& StrValue, MColumnDefine* pColumnDefine )
{
	switch ( pColumnDefine->FStrAction )
	{
		case sftNone      : return;
		case sftPadding   : StrValue.PadThis( pColumnDefine->FMaxLength, ' ' ); break;
		case sftTrimAll   : StrValue.TrimLeft( ); StrValue.TrimRight( ); break;
		case sftTrimLeft  : StrValue.TrimLeft( ); break;
		case sftTrimRight : StrValue.TrimRight( ); break;
	}
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::NewRowByFixedTextData( MRow& Row, char* TextData )
{
	FDefaultSeqNum++;
	MRow NewRowObj( new MRowInstance( FDefaultSeqNum, &FTableSchema ) );
	int ColumnPos, ColumnLen;

	// SetColumn by all columns of FColumnDefines
	for ( int i = 0; i < FTableSchema.FColumnDefines.ItemCount(); i++ )
	{
		MColumnDefine* pColumnDefine = FTableSchema.GetColumnDefine( i );
		ColumnPos = pColumnDefine->FDataFilePos;
		ColumnLen = pColumnDefine->FDataFileLen;
		if ( ColumnPos + ColumnLen > FTableSchema.FFixedDataTotalLength )
			return FALSE;
		UFC::AnsiString ColumnStr( TextData+ColumnPos, ColumnLen );
		// Check if Numerical Type , then Column String TrimLeft()
		if ( ( pColumnDefine->FType != ctString ) && ( pColumnDefine->FType != ctChar ) )
			ColumnStr.TrimLeft( );
		// Check if String Type, Do StrAction
		if ( pColumnDefine->FType == ctString )
			DoStrAction( ColumnStr, pColumnDefine );
		NewRowObj.SetColumn( i, ColumnStr );
	}
	Row = NewRowObj; ///< Assign new Row
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::NewRowByTextData( MRow& Row, char* TextData )
{
	FDefaultSeqNum++;
	MRow NewRowObj( new MRowInstance( FDefaultSeqNum, &FTableSchema ) );

	char			Splits[] = "|\n";
	char*			StatusPoint;  ///< the status variable to record its strtok_r() progress
	char*			TokenStr;
	UFC::AnsiString	ColumnStr;

	// SetColumn by all columns of FColumnDefines
	for ( int i = 0; i < FTableSchema.FColumnDefines.ItemCount(); i++ )
	{
		MColumnDefine* pColumnDefine = FTableSchema.GetColumnDefine( i );
		if ( i == 0 )
			TokenStr = strtok_r( TextData, Splits, &StatusPoint );
		else
			TokenStr = strtok_r( NULL, Splits, &StatusPoint );
		if ( TokenStr == NULL )
			return FALSE;
		ColumnStr = TokenStr;

		// Check if Numerical Type , then Column String TrimLeft()
		if ( ( pColumnDefine->FType != ctString ) && ( pColumnDefine->FType != ctChar ) )
			ColumnStr.TrimLeft( );
		// Check if String Type, Do StrAction
		if ( pColumnDefine->FType == ctString )
			DoStrAction( ColumnStr, pColumnDefine );
		NewRowObj.SetColumn( i, ColumnStr );
	}
	Row = NewRowObj; ///< Assign new Row
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::RetrieveRowByPKofTextData( MRow& Row, UFC::PStringList& StrList, const UFC::AnsiString& TextData )
{
	char			Splits[] = "|\n";
	char*			StatusPoint;  ///< the status variable to record its strtok_r() progress
	char*			TokenStr;
	UFC::AnsiString	ColumnStr;
	int				i;
	
	// Get string by all columns of FColumnDefines
	for ( i = 0; i < FTableSchema.FColumnDefines.ItemCount(); i++ )
	{
		if ( i == 0 )
			TokenStr = strtok_r( (char*)TextData.c_str(), Splits, &StatusPoint );
		else
			TokenStr = strtok_r( NULL, Splits, &StatusPoint );
		if ( TokenStr == NULL )
		{
			UFC::BufferedLog::Printf( " MTable::RetrieveRowByPKofTextData() Bad TextData ->" );
			return FALSE;
		}
		ColumnStr = TokenStr;
		StrList.Add( ColumnStr );
	}

	MIndexSchema* PKIndexSchema = FPkIndex->FIndexSchema;
	MDSBinaryObj BinaryObj( PKIndexSchema->FIndexSize );

	// Set BinaryObj by PK index
	for ( i = 0; i < PKIndexSchema->FIndexColumnIdxList.ItemCount(); i++ )
	{
		int IndexColumnIdx = PKIndexSchema->FIndexColumnIdxList[ i ];
		if ( IndexColumnIdx >= StrList.ItemCount() )
		{
			UFC::BufferedLog::Printf( " MTable::RetrieveRowByPKofTextData() IndexColumnIdx(%d) exceed data column count(%d) ->", IndexColumnIdx, StrList.ItemCount() );
			return FALSE;
		}
		MColumnDefine* pColumnDefine = FTableSchema.GetColumnDefine( IndexColumnIdx );
		switch ( pColumnDefine->FType )
		{
		case ctString : BinaryObj.AppendAnsiString( StrList.GetItem(IndexColumnIdx), pColumnDefine->FMaxLength - 1 ); break;
		case ctChar   : if ( StrList.GetItem(IndexColumnIdx).Length() > 0 )
							BinaryObj.AppendChar( StrList.GetItem(IndexColumnIdx)[0] );
						else
							BinaryObj.AppendChar( ' ' );
						break;
		case ctInt    : BinaryObj.AppendInt( StrList.GetItem(IndexColumnIdx).ToInt() ); break;
		case ctInt64  : BinaryObj.AppendInt64( StrList.GetItem(IndexColumnIdx).ToInt64() ); break;
		case ctDouble : BinaryObj.AppendDouble( StrList.GetItem(IndexColumnIdx).ToDouble() ); break;
		default       : return FALSE;
		}
	}

	return GetRowByPK( Row, BinaryObj );
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::RecoverInsertAction( char* DataBuffer )
{
	MRow Row;
	if ( NewRowByTextData( Row, DataBuffer ) == FALSE )
	{
		UFC::BufferedLog::Printf( " MTable::RecoverInsertAction() Bad TextData ->" );
		return FALSE;
	}
	if ( AddIntoRowList( Row, NULL ) == FALSE )
	{
		UFC::BufferedLog::Printf( " MTable::RecoverInsertAction() Row duplicate ->" );
		return FALSE;
	}

	// Clear All Transaction state except ROW_STATE_DELETE
	Row.FInstance->SetState( Row.FInstance->GetState() & ROW_STATE_DELETE );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::RecoverDeleteAction( char* DataBuffer )
{
	MRow				Row;
	UFC::PStringList	StrList; ///< to keep all columns by as AnsiString
	if ( RetrieveRowByPKofTextData( Row, StrList, DataBuffer ) == FALSE )
		return FALSE;

	RemoveFromRowList( Row, NULL );

	// Clear All Transaction state except ROW_STATE_DELETE
	Row.FInstance->SetState( Row.FInstance->GetState() & ROW_STATE_DELETE );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::RecoverUpdateAction( char* DataBuffer )
{
	MRow				Row;
	UFC::PStringList	StrList; ///< to keep all columns by as AnsiString
	if ( RetrieveRowByPKofTextData( Row, StrList, DataBuffer ) == FALSE )
		return FALSE;

	// Set All Columns
	for ( int i = 0; i < StrList.ItemCount(); i++ )
	{
		Row.SetColumn( i, StrList.GetItem(i) );
	}

	// Clear All Transaction state except ROW_STATE_DELETE
	Row.FInstance->SetState( Row.FInstance->GetState() & ROW_STATE_DELETE );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::IsDuplicateRow( MRow& Row )
{
	if ( FRowIDHash.find( Row.GetRowID() ) != FRowIDHash.end() )
		return TRUE;

	int i;
	// Just find in Unique Index Collection
	for ( i = 0; i < (int)FUniqueIndexsVector.size(); i++ )
	{
		MIndexObjectUnique* pIndexObject = FUniqueIndexsVector[i];
		MIndexSchema*		pIndexSchema = pIndexObject->FIndexSchema;
		MDSBinaryObj		BinaryObj( pIndexSchema->FIndexSize );
		SetBinaryObject( BinaryObj, pIndexSchema, &Row );
		if ( pIndexObject->GetRowByKey( BinaryObj ) != NULL )
			return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------
MRow MTable::NewRow( void )
{
	// Enter CriticalSection
	UFC::PLockObject Lock( FCS );

	FDefaultSeqNum++;
	return MRow( new MRowInstance( FDefaultSeqNum, &FTableSchema ) );
}
//------------------------------------------------------------------------------------------------------------------------
//
//    ROW_STATE_DELETE----RemoveFromRowList----Sync.-----END1
//    |
//   (N)
//    |
//    ROW_STATE_NEW-------AddIntoRowList----Sync.--------END2
//    |		              |
//   (N)                  Duplicate Error return FALSE---END3
//    |
//    |
//    ROW_STATE_MODIFY----Sync.--------------------------END4
//
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::Update( MRow& Row, int TransID, void* ExtraParm )
{
	// Enter CriticalSection
	UFC::PLockObject Lock( FCS );

	if ( Row.CheckState( ROW_STATE_DELETE ) )
	{
		RemoveFromRowList( Row, ExtraParm );
		Sync( MDS::mdsActionDelete, Row, TransID ); ///< END1
	}
	else if ( Row.CheckState( ROW_STATE_NEW ) )
	{
		if ( AddIntoRowList( Row, ExtraParm ) == FALSE )
			return FALSE; ///< END3
		Sync( MDS::mdsActionInsert, Row, TransID ); ///< END2
	}
	else if ( Row.CheckState( ROW_STATE_MODIFY ) )
	{
		Sync( MDS::mdsActionUpdate, Row, TransID ); ///< END4
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
int MTable::GetIndexSizeByIndexName( const UFC::AnsiString& IndexName )
{
	// avoid GetObjectByKey() core dump
	if ( IndexName.c_str() == NULL )
		return 0;

	MIndexSchema* pIndexSchema;
	if ( FTableSchema.FNameToIndexSchemaMap.GetObjectByKey( IndexName, pIndexSchema ) == FALSE )
		return 0;
	if ( pIndexSchema == NULL )
		return 0;
	return pIndexSchema->FIndexSize;
}
//------------------------------------------------------------------------------------------------------------------------
MIndexObjectUnique* MTable::GetIndexObjectUnique( const UFC::AnsiString& IndexName )
{
	// Get IndexObject by name
	TNameToIndexObjU::iterator Iter;
	if ( ( Iter = FUniqueIndexs.find( IndexName ) ) == FUniqueIndexs.end() )
		return NULL;
	return (MIndexObjectUnique*)Iter->second;
}
//------------------------------------------------------------------------------------------------------------------------
MIndexObjectMultiple* MTable::GetIndexObjectMultiple( const UFC::AnsiString& IndexName )
{
	// Get IndexObject by name
	TNameToIndexObjM::iterator Iter;
	if ( ( Iter = FMultipleIndexs.find( IndexName ) ) == FMultipleIndexs.end() )
		return NULL;
	return (MIndexObjectMultiple*)Iter->second;
}
//------------------------------------------------------------------------------------------------------------------------
int MTable::GetRowCount( void )
{
	return (int)FRowIDHash.size();
}
//------------------------------------------------------------------------------------------------------------------------
MRow MTable::GetRowByID( int RowID )
{
	TIntToMRowPtr::iterator RowIter;

	// Enter CriticalSection
	UFC::PLockObject Lock( FCS );

	if ( ( RowIter = FRowIDHash.find( RowID ) ) == FRowIDHash.end() )
		return MRow( (MRow*)NULL );
	else
		return *RowIter->second;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::GetRowByPK( MRow& Row, MDSBinaryObj& BinaryObj )
{
	MRow* pRow;

	if ( FPkIndex == NULL )
		return FALSE;

	// Enter CriticalSection
	UFC::PLockObject Lock( FCS );

	if ( ( pRow = FPkIndex->GetRowByKey( BinaryObj ) ) == NULL )
		return FALSE;

	Row = *pRow;
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::GetRowByIndexName( MRow& Row, const UFC::AnsiString& IndexName, MDSBinaryObj& BinaryObj )
{
	MRow* pRow;

	// Get IndexObject by name
	TNameToIndexObjU::iterator Iter;
	if ( ( Iter = FUniqueIndexs.find( IndexName ) ) == FUniqueIndexs.end() )
		return FALSE;
	MIndexObjectUnique* pIndexObject= Iter->second;

	// Enter CriticalSection
	UFC::PLockObject Lock( FCS );

	if ( ( pRow = pIndexObject->GetRowByKey( BinaryObj ) ) == NULL )
		return FALSE;

	Row = *pRow;
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::GetRowByIndexObject( MRow& Row, MIndexObjectUnique* pIndexObject, MDSBinaryObj& BinaryObj )
{
	MRow* pRow;

	// Enter CriticalSection
	UFC::PLockObject Lock( FCS );

	if ( ( pRow = pIndexObject->GetRowByKey( BinaryObj ) ) == NULL )
		return FALSE;

	Row = *pRow;
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::GetRowSetByIndexName( MRowSet& RowSet, const UFC::AnsiString& IndexName, MDSBinaryObj& BinaryObj )
{
	// Get IndexObject by name
	TNameToIndexObjM::iterator Iter;
	if ( ( Iter = FMultipleIndexs.find( IndexName ) ) == FMultipleIndexs.end() )
		return FALSE;
	MIndexObjectMultiple* pIndexObject = Iter->second;

	RowSet.Clear( ); ///< Clear RowSet when RowCount > 0

	// Enter CriticalSection
	UFC::PLockObject Lock( FCS );

	pIndexObject->GetRowSetByKey( RowSet, BinaryObj );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::GetRowSetByIndexObject( MRowSet& RowSet, MIndexObjectMultiple* pIndexObject, MDSBinaryObj& BinaryObj )
{
	RowSet.Clear( ); ///< Clear RowSet when RowCount > 0

	// Enter CriticalSection
	UFC::PLockObject Lock( FCS );

	pIndexObject->GetRowSetByKey( RowSet, BinaryObj );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::MoveFirst( void )
{
	if ( ( FRowIDHash_CurrIter = FRowIDHash.begin() ) == FRowIDHash.end() ) ///< check if head == tail
		return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTable::MoveNext( void )
{
	if ( FRowIDHash_CurrIter == FRowIDHash.end() ) ///< check if reach tail
		return FALSE;
	FRowIDHash_CurrIter++; ///< move to next row
	if ( FRowIDHash_CurrIter == FRowIDHash.end() ) ///< check again if reach tail
		return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
MRow MTable::GetCurrRow( void )
{
	if ( FRowIDHash_CurrIter == FRowIDHash.end() ) ///< check if reach tail
		return MRow( (MRow*)NULL );
	else
		return *FRowIDHash_CurrIter->second;
}
//------------------------------------------------------------------------------------------------------------------------
UFC::List<int>*	MTable::GetIndexListByIndexName( const UFC::AnsiString& Name )
{
	// avoid GetObjectByKey() core dump
	if ( Name.c_str() == NULL )
		return NULL;

	MIndexSchema* Idx;
	if ( FTableSchema.FNameToIndexSchemaMap.GetObjectByKey( Name, Idx ) == FALSE )
		return NULL;
	if ( Idx == NULL )
		return NULL;
	return &(Idx->FIndexColumnIdxList);
}
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
