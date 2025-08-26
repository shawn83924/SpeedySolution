//------------------------------------------------------------------------------------------------------------------------
#include "MRow.h"
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
// class MRowInstance
//------------------------------------------------------------------------------------------------------------------------
MRowInstance::MRowInstance( int RowID, MTableSchema* pTableSchema, UInt8* pData )
: FID( RowID )
, FRef( 0 )
, FTableSchemaPtr( pTableSchema )
, FData( pData )
, FAttachObject( NULL )
, FColumns( NULL )
{
	FColumns = new TVectorColumn( FTableSchemaPtr->FColumnDefines.ItemCount() ); ///< New array object by array size

	try
	{
		Initial( );
	}
	catch ( MDSException& ex )
	{
		MDSInitialFailException exVar;
		exVar.Throw( ex.what() );
	}
	catch ( UFC::Exception& ex )
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MRowInstance::MRowInstance -> [%s] %s"
					 , FTableSchemaPtr->FSchemaName.c_str()
					 , ex.what() );
	}
	catch ( ... )
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MRowInstance::MRowInstance -> [%s] Unknow Exception"
					 , FTableSchemaPtr->FSchemaName.c_str() );
	}
}
//------------------------------------------------------------------------------------------------------------------------
MRowInstance::~MRowInstance( void )
{
	if ( FColumns != NULL )
	{
		for ( int i = 0; i < (int)FColumns->size(); i++ )
		{
			if ( (*FColumns)[ i ] != NULL )
			{
				delete (*FColumns)[ i ];
				(*FColumns)[ i ] = NULL;
			}
		}
		FColumns->clear();
		delete FColumns;
		FColumns = NULL;
	}

	if ( FData != NULL )
	{
		delete [] FData;
		FData = NULL;
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MRowInstance::Initial( void )
{
	if ( FTableSchemaPtr == NULL )
	{
		MDSInitialFailException exVar;
		exVar.Throw( "MRowInstance::Initial -> FSchema = null" );
	}

	int RowSize = FTableSchemaPtr->FRowSize;

	if ( FData == NULL )
	{
		FData = new UInt8[ RowSize ];
		memset( FData, 0x00, RowSize );
		(*FData) |= ROW_STATE_NEW;
	}

	int            Offset        = 1; ///< dirty bits
	MColumn*       pColumn       = NULL;
	MColumnDefine* pColumnDefine = NULL;

	for ( int i = 0; i < FTableSchemaPtr->FColumnDefines.ItemCount(); i++ )
	{
		pColumnDefine = FTableSchemaPtr->FColumnDefines.GetItem( i );
		pColumn = new MColumn( pColumnDefine, FData + Offset );
		(*FColumns)[ i ] = pColumn;
		Offset += pColumnDefine->FMaxLength;
	}
}
//------------------------------------------------------------------------------------------------------------------------
MColumn* MRowInstance::GetColumn( const UFC::AnsiString& ColumnName )
{
	// avoid GetObjectByKey() core dump
	if ( ColumnName.c_str() == NULL )
	{
		MDSColumnNotFoundException exVar;
		exVar.Throw( "MRowInstance::GetColumn -> [%s] ColumnName [null] not found"
					 , FTableSchemaPtr->FSchemaName.c_str() );
	}

	// find index from TableSchema
	int ColumnIdx = 0;
	if ( FTableSchemaPtr->FNameToColumnIdxMap.GetObjectByKey( ColumnName, ColumnIdx ) == FALSE )
	{
		MDSColumnNotFoundException exVar;
		exVar.Throw( "MRowInstance::GetColumn -> [%s] ColumnName [%s] not found"
					 , FTableSchemaPtr->FSchemaName.c_str()
					 , ColumnName.c_str() );
	}

	// Check if FColumns is NULL
	if ( FColumns == NULL )
	{
		MDSColumnNotFoundException exVar;
		exVar.Throw( "MRowInstance::GetColumn -> [%s] Column [%d] not found (FColumns is NULL)"
					 , FTableSchemaPtr->FSchemaName.c_str()
					 , ColumnIdx );
	}
	// Check index boundary
	if ( (ColumnIdx+1) > (int)FColumns->size() )
	{
		MDSColumnNotFoundException exVar;
		exVar.Throw( "MRowInstance::GetColumn -> [%s] Column [%s(%d)] not found"
					 , FTableSchemaPtr->FSchemaName.c_str()
					 , ColumnName.c_str()
					 , ColumnIdx );
	}
	// get MColumn* from Array
	MColumn* pColumn = (*FColumns)[ ColumnIdx ];
	// Check if pColumn is NULL
	if ( pColumn == NULL )
	{
		MDSColumnNotFoundException exVar;
		exVar.Throw( "MRowInstance::GetColumn -> [%s] Column [%s(%d)] not found"
					 , FTableSchemaPtr->FSchemaName.c_str()
					 , ColumnName.c_str()
					 , ColumnIdx );
	}

	return pColumn;
}
//------------------------------------------------------------------------------------------------------------------------
MColumn* MRowInstance::GetColumn( int Idx )
{
	// Check if FColumns is NULL
	if ( FColumns == NULL )
	{
		MDSColumnNotFoundException exVar;
		exVar.Throw( "MRowInstance::GetColumn -> [%s] Column [%d] not found (FColumns is NULL)"
					 , FTableSchemaPtr->FSchemaName.c_str()
					 , Idx );
	}
	// Check index boundary
	if ( (Idx+1) > (int)FColumns->size() )
	{
		MDSColumnNotFoundException exVar;
		exVar.Throw( "MRowInstance::GetColumn -> [%s] Column [%d] not found (index out of size)"
					 , FTableSchemaPtr->FSchemaName.c_str()
					 , Idx );
	}
	// get MColumn* from Array
	MColumn* pColumn = (*FColumns)[ Idx ];
	// Check if pColumn is NULL
	if ( pColumn == NULL )
	{
		MDSColumnNotFoundException exVar;
		exVar.Throw( "MRowInstance::GetColumn -> [%s] Column [%d] not found (pColumn is NULL)"
					 , FTableSchemaPtr->FSchemaName.c_str()
					 , Idx );
	}

	return pColumn;
}
//------------------------------------------------------------------------------------------------------------------------
UInt8 MRowInstance::GetState( void )
{
	return (*FData);
}
//------------------------------------------------------------------------------------------------------------------------
void MRowInstance::SetState( UInt8 Value )
{
	*FData = Value;
}
//------------------------------------------------------------------------------------------------------------------------
//
//    FIsIndexMember----ROW_STATE_NEW----return TRUE-----END1
//    |                 |
//   (N)               (N)
//    |                 |
//    |                 return FALSE---------------------END2
//    |
//    |
//    return TRUE----------------------------------END3
//
//------------------------------------------------------------------------------------------------------------------------
BOOL MRowInstance::CanModifyColumn( MColumn* pColumn )
{
	if ( pColumn->IsIndexMemberColumn() )
	{
		if ( ( GetState() & ROW_STATE_NEW ) != 0 )
			return TRUE;  ///< END1
		else
			return FALSE; ///< END2
	}

	return TRUE; ///< END3
}
//------------------------------------------------------------------------------------------------------------------------
// class MRow
//------------------------------------------------------------------------------------------------------------------------
MRow::MRow( void )
: FInstance( NULL )
{
}
//------------------------------------------------------------------------------------------------------------------------
MRow::MRow( MRowInstance* pInstance )
: FInstance( pInstance )
{
	IncreaseReferenceCount( );
}
//------------------------------------------------------------------------------------------------------------------------
MRow::MRow( const MRow& Row )
: FInstance( Row.FInstance )
{
	IncreaseReferenceCount( );
}
//------------------------------------------------------------------------------------------------------------------------
MRow::MRow( MRow* pRow )
: FInstance( NULL )
{
	if ( pRow != NULL )
	{
		FInstance = pRow->FInstance;
		IncreaseReferenceCount( );
	}
}
//------------------------------------------------------------------------------------------------------------------------
MRow::~MRow( void )
{
	DecreaseReferenceCount( );
}
//------------------------------------------------------------------------------------------------------------------------
MRow& MRow::operator = ( const MRow& Row )
{
	if ( this != &Row )
	{
		// Increase first
		IncreaseReferenceCount( Row.FInstance );
		// Keep old FInstance
		MRowInstance* OldInstPtr = FInstance;
		// Assign new FInstance
		FInstance = Row.FInstance;
		// Decrease last
		DecreaseReferenceCount( OldInstPtr );
	}
	return *this;
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::IncreaseReferenceCount( void )
{
	if ( FInstance != NULL )
	{
		UFC::PLockObject Lock( FInstance->FCS );
		FInstance->FRef++;
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::IncreaseReferenceCount( MRowInstance* InstPtr )
{
	if ( InstPtr != NULL )
	{
		UFC::PLockObject Lock( InstPtr->FCS );
		InstPtr->FRef++;
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::DecreaseReferenceCount( void )
{
	if ( FInstance != NULL )
	{
		UFC::PLockObject Lock( FInstance->FCS );
		FInstance->FRef--;
		if ( FInstance->FRef <= 0 )
		{
			delete FInstance;
			FInstance = NULL;
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::DecreaseReferenceCount( MRowInstance* InstPtr )
{
	if ( InstPtr != NULL )
	{
		UFC::PLockObject Lock( InstPtr->FCS );
		InstPtr->FRef--;
		if ( InstPtr->FRef <= 0 )
		{
			delete InstPtr;
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString MRow::GetColumnAsString( const UFC::AnsiString& ColumnName ) const
{
	if ( FInstance == NULL )
		return "";

	MColumn* pColumn = FInstance->GetColumn( ColumnName );
	return pColumn->ToString( );
}
//------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString MRow::GetColumnAsString( int Idx ) const
{
	if ( FInstance == NULL )
		return "";

	MColumn* pColumn = FInstance->GetColumn( Idx );
	return pColumn->ToString( );
}
//------------------------------------------------------------------------------------------------------------------------
char MRow::GetColumnAsChar( const UFC::AnsiString& ColumnName ) const
{
	if ( FInstance == NULL )
		return ' ';

	MColumn* pColumn = FInstance->GetColumn( ColumnName );
	return pColumn->ToChar( );
}
//------------------------------------------------------------------------------------------------------------------------
char MRow::GetColumnAsChar( int Idx ) const
{
	if ( FInstance == NULL )
		return ' ';

	MColumn* pColumn = FInstance->GetColumn( Idx );
	return pColumn->ToChar( );
}
//------------------------------------------------------------------------------------------------------------------------
int MRow::GetColumnAsInt( const UFC::AnsiString& ColumnName ) const
{
	if ( FInstance == NULL )
		return 0;

	MColumn* pColumn = FInstance->GetColumn( ColumnName );
	return pColumn->ToInt( );
}
//------------------------------------------------------------------------------------------------------------------------
Int32 MRow::GetColumnAsInt( int Idx ) const
{
	if ( FInstance == NULL )
		return 0;

	MColumn* pColumn = FInstance->GetColumn( Idx );
	return pColumn->ToInt( );
}
//------------------------------------------------------------------------------------------------------------------------
Int64 MRow::GetColumnAsInt64( const UFC::AnsiString& ColumnName ) const
{
	if ( FInstance == NULL )
		return 0LL;

	MColumn* pColumn = FInstance->GetColumn( ColumnName );
	return pColumn->ToInt64( );
}
//------------------------------------------------------------------------------------------------------------------------
Int64 MRow::GetColumnAsInt64( int Idx ) const
{
	if ( FInstance == NULL )
		return 0LL;

	MColumn* pColumn = FInstance->GetColumn( Idx );
	return pColumn->ToInt64( );
}
//------------------------------------------------------------------------------------------------------------------------
double MRow::GetColumnAsDouble( const UFC::AnsiString& ColumnName ) const
{
	if ( FInstance == NULL )
		return 0.0;

	MColumn* pColumn = FInstance->GetColumn( ColumnName );
	return pColumn->ToDouble( );
}
//------------------------------------------------------------------------------------------------------------------------
double MRow::GetColumnAsDouble( int Idx ) const
{
	if ( FInstance == NULL )
		return 0.0;

	MColumn* pColumn = FInstance->GetColumn( Idx );
	return pColumn->ToDouble( );
}
//------------------------------------------------------------------------------------------------------------------------
MColumn* MRow::GetColumn( const UFC::AnsiString& ColumnName )
{
	if ( FInstance == NULL )
		return NULL;

	return FInstance->GetColumn( ColumnName );
}
//------------------------------------------------------------------------------------------------------------------------
MColumn* MRow::GetColumn( int Idx )
{
	if ( FInstance == NULL )
		return NULL;

	return FInstance->GetColumn( Idx );
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::SetColumn( const UFC::AnsiString& ColumnName, const UFC::AnsiString& ColumnValue )
{
	if ( FInstance != NULL )
	{
		MColumn* pColumn = FInstance->GetColumn( ColumnName );
		if ( FInstance->CanModifyColumn( pColumn ) )
		{
            pColumn->FromString( ColumnValue );
			FInstance->SetState( FInstance->GetState() | ROW_STATE_MODIFY );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::SetColumn( const UFC::AnsiString& ColumnName, const char ColumnValue )
{
	if ( FInstance != NULL )
	{
		MColumn* pColumn = FInstance->GetColumn( ColumnName );
		if ( FInstance->CanModifyColumn( pColumn ) )
		{
			pColumn->FromChar( ColumnValue );
			FInstance->SetState( FInstance->GetState() | ROW_STATE_MODIFY );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::SetColumn( const UFC::AnsiString& ColumnName, const int ColumnValue )
{
	if ( FInstance != NULL )
	{
		MColumn* pColumn = FInstance->GetColumn( ColumnName );
		if ( FInstance->CanModifyColumn( pColumn ) )
		{
            pColumn->FromInt( ColumnValue );
			FInstance->SetState( FInstance->GetState() | ROW_STATE_MODIFY );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::SetColumn( const UFC::AnsiString& ColumnName, const Int64 ColumnValue )
{
	if ( FInstance != NULL )
	{
		MColumn* pColumn = FInstance->GetColumn( ColumnName );
		if ( FInstance->CanModifyColumn( pColumn ) )
		{
			pColumn->FromInt64( ColumnValue );
			FInstance->SetState( FInstance->GetState() | ROW_STATE_MODIFY );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::SetColumn( const UFC::AnsiString& ColumnName, const double ColumnValue )
{
	if ( FInstance != NULL )
	{
		MColumn* pColumn = FInstance->GetColumn( ColumnName );
		if ( FInstance->CanModifyColumn( pColumn ) )
		{
            pColumn->FromDouble( ColumnValue );
			FInstance->SetState( FInstance->GetState() | ROW_STATE_MODIFY );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::SetColumn( int Idx, const UFC::AnsiString& ColumnValue )
{
	if ( FInstance != NULL )
	{
		MColumn* pColumn = FInstance->GetColumn( Idx );
		if ( FInstance->CanModifyColumn( pColumn ) )
		{
			pColumn->FromString( ColumnValue );
			FInstance->SetState( FInstance->GetState() | ROW_STATE_MODIFY );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::SetColumn( int Idx, const char ColumnValue )
{
	if ( FInstance != NULL )
	{
		MColumn* pColumn = FInstance->GetColumn( Idx );
		if ( FInstance->CanModifyColumn( pColumn ) )
		{
			pColumn->FromChar( ColumnValue );
			FInstance->SetState( FInstance->GetState() | ROW_STATE_MODIFY );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::SetColumn( int Idx, const int ColumnValue )
{
	if ( FInstance != NULL )
	{
		MColumn* pColumn = FInstance->GetColumn( Idx );
		if ( FInstance->CanModifyColumn( pColumn ) )
		{
			pColumn->FromInt( ColumnValue );
			FInstance->SetState( FInstance->GetState() | ROW_STATE_MODIFY );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::SetColumn( int Idx, const Int64 ColumnValue )
{
	if ( FInstance != NULL )
	{
		MColumn* pColumn = FInstance->GetColumn( Idx );
		if ( FInstance->CanModifyColumn( pColumn ) )
		{
			pColumn->FromInt64( ColumnValue );
			FInstance->SetState( FInstance->GetState() | ROW_STATE_MODIFY );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::SetColumn( int Idx, const double ColumnValue )
{
	if ( FInstance != NULL )
	{
		MColumn* pColumn = FInstance->GetColumn( Idx );
		if ( FInstance->CanModifyColumn( pColumn ) )
		{
			pColumn->FromDouble( ColumnValue );
			FInstance->SetState( FInstance->GetState() | ROW_STATE_MODIFY );
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MRow::IsNull( void )
{
	if ( FInstance == NULL )
		return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::SetNull( void )
{
	DecreaseReferenceCount( );
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::Delete( void )
{
	// Just remark delete flag, until MTable::Update(), then Remove from memory at MTable
	if ( FInstance != NULL )
		FInstance->SetState( FInstance->GetState() | ROW_STATE_MODIFY | ROW_STATE_DELETE );
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MRow::CheckState( UInt8 Mask ) const
{
	if ( FInstance != NULL )
		return ( (FInstance->GetState() & Mask) != 0 );
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------
int MRow::GetRowID( void ) const
{
	if ( FInstance != NULL )
		return FInstance->FID;
	return 0;
}
//------------------------------------------------------------------------------------------------------------------------
void MRow::SetAttachObject( void* Obj )
{
	if ( FInstance != NULL )
		FInstance->FAttachObject = Obj;
}
//------------------------------------------------------------------------------------------------------------------------
void* MRow::GetAttachObject( void ) const
{
	if ( FInstance != NULL )
		return FInstance->FAttachObject;
	return NULL;
}
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
