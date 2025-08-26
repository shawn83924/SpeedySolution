//------------------------------------------------------------------------------------------------------------------------
#include "MIndex.h"
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
// class MIndexObjectUnique
//------------------------------------------------------------------------------------------------------------------------
MIndexObjectUnique::MIndexObjectUnique( MIndexSchema* pIndexSchema, int MapSize )
: FIndexSchema( pIndexSchema )
#if (__BORLANDC__ >= 0x0580) ///< For BDS 2006 Dinkum C++ Library
, FContainer( )
#else
, FContainer( MapSize )
#endif
{
}
//------------------------------------------------------------------------------------------------------------------------
MIndexObjectUnique::~MIndexObjectUnique()
{
	FContainer.clear();
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MIndexObjectUnique::Add( MDSBinaryObj& BinaryObj, MRow* pRow )
{
	TBinToMap::iterator Iter;
	if ( ( Iter = FContainer.find( BinaryObj ) ) != FContainer.end() )
		return FALSE;	// duplicate Key
	FContainer.insert( make_pair( BinaryObj, pRow ) );
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void MIndexObjectUnique::Delete( MDSBinaryObj& BinaryObj, MRow* pRow )
{
	FContainer.erase( BinaryObj );
}
//------------------------------------------------------------------------------------------------------------------------
MRow* MIndexObjectUnique::GetRowByKey( MDSBinaryObj& BinaryObj )
{
	TBinToMap::iterator Iter;
	if ( ( Iter = FContainer.find( BinaryObj ) ) == FContainer.end() )
		return NULL;
	return Iter->second;
}
//------------------------------------------------------------------------------------------------------------------------
void MIndexObjectUnique::GetRowSetByKey( MRowSet& RowSet, MDSBinaryObj& BinaryObj )
{
}
//------------------------------------------------------------------------------------------------------------------------
// class MIndexObjectMultiple
//------------------------------------------------------------------------------------------------------------------------
MIndexObjectMultiple::MIndexObjectMultiple( MIndexSchema* pIndexSchema, int MapSize )
: FIndexSchema( pIndexSchema )
#if (__BORLANDC__ >= 0x0580) ///< For BDS 2006 Dinkum C++ Library
, FContainer( )
#else
, FContainer( MapSize )
#endif
{
}
//------------------------------------------------------------------------------------------------------------------------
MIndexObjectMultiple::~MIndexObjectMultiple()
{
	FContainer.clear();
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MIndexObjectMultiple::Add( MDSBinaryObj& BinaryObj, MRow* pRow )
{
	FContainer.insert( make_pair( BinaryObj, pRow ) );
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void MIndexObjectMultiple::Delete( MDSBinaryObj& BinaryObj, MRow* pRow )
{
	TBinToMultiMap::iterator Iter;
	if ( ( Iter = FContainer.find( BinaryObj ) ) == FContainer.end() )
		return;
	int Count = FContainer.count( BinaryObj );
	for ( int i = 0; i < Count; i++ )
	{
		if ( pRow->GetRowID() == Iter->second->GetRowID() )
		{
			FContainer.erase( Iter );
			break;
		}
		Iter++;
	}
}
//------------------------------------------------------------------------------------------------------------------------
MRow* MIndexObjectMultiple::GetRowByKey( MDSBinaryObj& BinaryObj )
{
	TBinToMultiMap::iterator Iter;
	if ( ( Iter = FContainer.find( BinaryObj ) ) == FContainer.end() )
		return NULL;
	return Iter->second;
}
//------------------------------------------------------------------------------------------------------------------------
void MIndexObjectMultiple::GetRowSetByKey( MRowSet& RowSet, MDSBinaryObj& BinaryObj )
{
	TBinToMultiMap::iterator Iter;
	if ( ( Iter = FContainer.find( BinaryObj ) ) == FContainer.end() )
		return;
	int RecordCount = FContainer.count( BinaryObj );

	for ( int i = 0; i < RecordCount; i++ )
	{
		RowSet.FRowList.push_back( new MRow( Iter->second ) );
		Iter++;
	}
	RowSet.FRowCount = RecordCount;
}
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
