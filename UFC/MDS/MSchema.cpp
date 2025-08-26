//------------------------------------------------------------------------------------------------------------------------
#include "MSchema.h"
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
// class MIndexSchema
//------------------------------------------------------------------------------------------------------------------------
MIndexSchema::MIndexSchema()
: FIndexSize( 0 )
{
}
//------------------------------------------------------------------------------------------------------------------------
MIndexSchema::~MIndexSchema( void )
{
}
//------------------------------------------------------------------------------------------------------------------------
// class MTableSchema
//------------------------------------------------------------------------------------------------------------------------
MTableSchema::MTableSchema( const UFC::AnsiString& Name, const UFC::AnsiString& SchemaDir )
: FSchemaName( Name )
, FSchemaDir( SchemaDir )
, FRowSize( 1 ) ///< dirty bits
{
	try
	{
		FColumnDefines.SetOwnObjects( TRUE ); ///< set own object will delete the element when Container destory

		UFC::AnsiString FileNameComm;
		UFC::AnsiString FileNameSchema;
		UFC::AnsiString FileNameIndex;
		FileNameComm.Printf( "%s/%s.comm", FSchemaDir.c_str(), Name.c_str() );
		FileNameSchema.Printf( "%s/%s.schema", FSchemaDir.c_str(), Name.c_str() );
		FileNameIndex.Printf( "%s/%s.index", FSchemaDir.c_str(), Name.c_str() );

		LoadSchema_CommFile( FileNameComm );	///< Load from xxx.comm file
		LoadSchema_SchemaFile( FileNameSchema );///< Load from xxx.schema file
		LoadSchema_IndexFile( FileNameIndex );	///< Load from xxx.index file
	}
	catch ( MDSException& ex )
	{
		MDSInitialFailException exVar;
		exVar.Throw( ex.what() );
	}
	catch ( UFC::Exception& ex )
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MTableSchema::MTableSchema -> %s", ex.what() );
	}
	catch ( ... )
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MTableSchema::MTableSchema -> Unknow Exception" );
	}
}
//------------------------------------------------------------------------------------------------------------------------
MTableSchema::~MTableSchema( void )
{
	FColumnDefines.Clear( ); ///< Will delete the element he owns then clear

	// delete all element of FNameToIndexSchemaMap
	int i;
	MIndexSchema* pIndexSchema;
	for ( i = FNameToIndexSchemaMap.ItemCount() - 1; i >= 0; i-- )
	{
		if ( FNameToIndexSchemaMap.GetItem( i, pIndexSchema ) == FALSE )
			continue;
		if ( pIndexSchema == NULL )
			continue;
		delete pIndexSchema;
	}
}
//------------------------------------------------------------------------------------------------------------------------
enumColumnType MTableSchema::ReadType( UFC::Section* pSection )
{
	enumColumnType  Result = ctString;
	UFC::AnsiString Type;

	if ( pSection->GetValue( "Type", Type ) == FALSE )
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MTableSchema::ReadType -> No Type in [%s-%s]", FSchemaName.c_str(), pSection->GetSectionName().c_str() );
	}

	Type.UpperThis();

	if ( Type == "STRING" )
		Result = ctString;
	else if ( Type == "CHAR" )
		Result = ctChar;
	else if ( Type == "INT" )
		Result = ctInt;
	else if ( Type == "INT64" )
		Result = ctInt64;
	else if ( Type == "DOUBLE" )
		Result = ctDouble;
	else
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MTableSchema::ReadType -> Unknow Type in [%s-%s]", FSchemaName.c_str(), pSection->GetSectionName().c_str() );
	}

	return Result;
}
//------------------------------------------------------------------------------------------------------------------------
int MTableSchema::ReadMaxLength( UFC::Section* pSection )
{
	UFC::AnsiString StrValue;
	if ( pSection->GetValue( "MaxLength", StrValue ) == FALSE )
		return 0;
	return StrValue.ToInt();
}
//------------------------------------------------------------------------------------------------------------------------
enumStrActionType MTableSchema::ReadStrAction( UFC::Section* pSection )
{
	enumStrActionType	Result;
	UFC::AnsiString		Type;
	if ( pSection->GetValue( "StrAction", Type ) == TRUE )
	{
		Type.UpperThis();
		if ( Type == "PADDING" )
			Result = sftPadding;
		else if ( Type == "TRIMALL" )
			Result = sftTrimAll;
		else if ( Type == "TRIMLEFT" )
			Result = sftTrimLeft;
		else if ( Type == "TRIMRIGHT" )
			Result = sftTrimRight;
		else
			Result = sftNone;
	}
	else
		Result = sftNone;

	return Result;
}
//------------------------------------------------------------------------------------------------------------------------
int MTableSchema::ReadShowPoint( UFC::Section* pSection )
{
	UFC::AnsiString StrValue;
	if ( pSection->GetValue( "ShowPoint", StrValue ) == FALSE )
		return 2; ///< use 2 default value if ShowPoint no set
	return StrValue.ToInt();
}
//------------------------------------------------------------------------------------------------------------------------
int MTableSchema::ReadDataFilePos( UFC::Section* pSection )
{
	UFC::AnsiString StrValue;
	// if IsFixedDataFile -> DataFilePos should set
	if ( pSection->GetValue( "DataFilePos", StrValue ) == FALSE )
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MTableSchema::ReadDataFilePos -> Column [%s]-DataFilePos value not found", pSection->GetSectionName().c_str() );
	}
	return StrValue.ToInt();
}
//------------------------------------------------------------------------------------------------------------------------
int MTableSchema::ReadDataFileLen( UFC::Section* pSection )
{
	UFC::AnsiString StrValue;
	// if IsFixedDataFile -> DataFileLen should set
	if ( pSection->GetValue( "DataFileLen", StrValue ) == FALSE )
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MTableSchema::ReadDataFileLen -> Column [%s]-DataFileLen value not found", pSection->GetSectionName().c_str() );
	}
	return StrValue.ToInt();
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTableSchema::ReadBoolValue( UFC::Section* pSection, const UFC::AnsiString& Name )
{
	UFC::AnsiString Value;

	if( !pSection->GetValue( Name, Value ) )
		return FALSE;

	Value.UpperThis();
	if( Value == "TRUE" )
		return TRUE;
	if( Value == "YES" )
		return TRUE;
	if( Value == "1" )
		return TRUE;

	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------
int MTableSchema::GetColumnIdxByName( const UFC::AnsiString& ColumnName )
{
	// avoid GetObjectByKey() core dump
	if ( ColumnName.c_str() == NULL )
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MTableSchema::GetColumnIdxByName -> ColumnName [null] not found" );
	}

	int ColumnIdx = 0;
	if ( FNameToColumnIdxMap.GetObjectByKey( ColumnName, ColumnIdx ) == FALSE )
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MTableSchema::GetColumnIdxByName -> ColumnName [%s] not found", ColumnName.c_str() );
	}

	return ColumnIdx;
}
//------------------------------------------------------------------------------------------------------------------------
MColumnDefine* MTableSchema::GetColumnDefine( int Idx )
{
	MColumnDefine* pColumnDefine = FColumnDefines.GetItem( Idx );
	if ( pColumnDefine == NULL )
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MTableSchema::GetColumnDefine -> Column [idx(%d)] not found", Idx );
	}

	return pColumnDefine;
}
//------------------------------------------------------------------------------------------------------------------------
void MTableSchema::AddColumnDefine( const UFC::AnsiString& ColumnName,
									enumColumnType Type,
									int MaxLength,
									enumStrActionType StrAction,
									int ShowPoint,
									int DataFilePos,
									int DataFileLen )
{
	for ( int i = 0; i < FColumnDefines.ItemCount(); i++ )
	{
		if ( ColumnName == FColumnDefines.GetItem(i)->FName )
		{
			MDSInitialFailException exVar;
			exVar.Throw( "MTableSchema::AddColumnDefine -> Duplicate column name:%s", ColumnName.c_str() );
		}
	}

	switch( Type )
	{
	case ctString :	MaxLength++; break; ///< MaxLength add 1 for null terminate char
	case ctChar   : MaxLength = sizeof(char); break;
	case ctInt    : MaxLength = sizeof(int); break;
	case ctInt64  : MaxLength = sizeof(Int64); break;
	case ctDouble : MaxLength = sizeof(double); break;
	}

	MColumnDefine* NewDefine = new MColumnDefine( );
	NewDefine->FName          = ColumnName;
	NewDefine->FType          = Type;
	NewDefine->FIsIndexMember = FALSE;
	NewDefine->FMaxLength     = MaxLength;
	NewDefine->FStrAction     = StrAction;
	NewDefine->FShowPoint     = ShowPoint;
	NewDefine->FDataFilePos   = DataFilePos;
	NewDefine->FDataFileLen   = DataFileLen;
	FColumnDefines.Add( NewDefine );
	FRowSize += MaxLength;
}
//------------------------------------------------------------------------------------------------------------------------
void MTableSchema::AddIndexToMap( const UFC::AnsiString& IndexName, UFC::Section* pSection )
{
	UFC::AnsiString		Name, Value;
	int					Count, ColIdx;
	MColumnDefine		*pColumnDefine;
	MIndexSchema		*pIndexSchema;

	// Check IndexName if duplicate
	for ( int i = 0; i < FNameToIndexSchemaMap.ItemCount(); i++ )
	{
		if ( FNameToIndexSchemaMap.GetItem( i, pIndexSchema ) == FALSE )
			continue;
		if ( pIndexSchema == NULL )
			continue;
		if ( IndexName == pIndexSchema->FIndexName )
		{
			MDSInitialFailException exVar;
			exVar.Throw( "MTableSchema::AddIndexToMap -> Duplicate index name:%s", IndexName.c_str() );
		}
	}

	MIndexSchema* IndexSchema = new MIndexSchema( );
	IndexSchema->FIndexName = IndexName;
	IndexSchema->FIsUnique  = ReadBoolValue( pSection, "IsUnique" );
	IndexSchema->FIsPK      = ReadBoolValue( pSection, "IsPK" );

	Name = "Column1";
	Count = 1;
	while ( pSection->GetValue( Name, Value ) )
	{
		// Get ColumnIdx by name
		ColIdx = GetColumnIdxByName( Value );
		IndexSchema->FIndexColumnIdxList.Add( GetColumnIdxByName( Value ) );
		// Count index size
		pColumnDefine = GetColumnDefine( ColIdx );
		if ( pColumnDefine->FType == ctString )
            IndexSchema->FIndexSize += ( pColumnDefine->FMaxLength - 1 ); ///< just occupied column length
		else
			IndexSchema->FIndexSize += pColumnDefine->FMaxLength;
		// Set IsIndexMember column of ColumnDefine
		pColumnDefine->FIsIndexMember = TRUE;

		// set next name
		Count++;
		Name.Printf( "Column%d", Count );
	}
	// Add to index map
	FNameToIndexSchemaMap.Add( IndexName, IndexSchema );
}
//------------------------------------------------------------------------------------------------------------------------
void MTableSchema::BuildNameToColumnIdxMap( void )
{
	for ( int i = 0; i < FColumnDefines.ItemCount(); i++ )
	{
		FNameToColumnIdxMap.Add( FColumnDefines.GetItem(i)->FName, i );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MTableSchema::LoadSchema_CommFile( const UFC::AnsiString& FileName )
{
	UFC::UiniFile		iniFile( FileName );
	UFC::AnsiString		ValueStr;
	UFC::Section*		pSection;
	if ( ( pSection = iniFile.GetSection( "Comm" ) ) == NULL )
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MTableSchema::LoadSchema_CommFile -> [Comm] section not found" );
	}
	if ( pSection->GetValue( "ExpectRowAmount", ValueStr ) == FALSE )
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MTableSchema::LoadSchema_CommFile -> [Comm]-ExpectRowAmount not found" );
	}
	FExpectRowAmount = ValueStr.ToInt( );
	FIsFixedDataFile = ReadBoolValue( pSection, "IsFixedDataFile" ); ///< default(if not found) is FALSE
}
//------------------------------------------------------------------------------------------------------------------------
void MTableSchema::LoadSchema_SchemaFile( const UFC::AnsiString& FileName )
{
	UFC::UiniFile		iniFile( FileName );
	UFC::AnsiString		ColumnName;
	UFC::Section		*pSection;

	FFixedDataTotalLength = 0; ///< initial FFixedDataTotalLength to zero
	// Build ColumnDefine array list
	FColumnDefines.Clear( );
	for ( int i = 0; i < iniFile.SectionCount(); i++ )
	{
		pSection = iniFile.GetSection( i );
		ColumnName = pSection->GetSectionName( );
		if ( ColumnName.Length() == 0 )
		{
			MDSInitialFailException exVar;
			exVar.Throw( "MTableSchema::LoadSchema_SchemaFile -> Column name = null string" );
		}
		if ( FIsFixedDataFile == TRUE )
		{
			int DataFileLen = ReadDataFileLen( pSection );
			AddColumnDefine( ColumnName,
							 ReadType( pSection ),
							 ReadMaxLength( pSection ),
							 ReadStrAction( pSection ),
							 ReadShowPoint( pSection ),
							 ReadDataFilePos( pSection ),
							 DataFileLen );
			FFixedDataTotalLength += DataFileLen;
		}
		else
		{
			AddColumnDefine( ColumnName,
							 ReadType( pSection ),
							 ReadMaxLength( pSection ),
							 ReadStrAction( pSection ),
							 ReadShowPoint( pSection ),
							 0,		///< use delimiter, DataFilePos no need to set
							 0 );	///< use delimiter, DataFileLen no need to set
		}
	}
	// Build ColumenName to Column array index map
	BuildNameToColumnIdxMap( );
}
//------------------------------------------------------------------------------------------------------------------------
void MTableSchema::LoadSchema_IndexFile( const UFC::AnsiString& FileName )
{
	UFC::UiniFile		iniFile( FileName );
	UFC::AnsiString		IndexName;
	UFC::Section		*pSection;

	// Build IndexName to IndexObject map
	for ( int i = 0; i < iniFile.SectionCount(); i++ )
	{
		pSection = iniFile.GetSection( i );
		IndexName = pSection->GetSectionName( );
		if ( IndexName.Length() == 0 )
		{
			MDSInitialFailException exVar;
			exVar.Throw( "MTableSchema::LoadSchema_IndexFile -> Index name = null string" );
		}
		AddIndexToMap( IndexName, pSection );
	}
}
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
