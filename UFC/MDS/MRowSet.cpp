//------------------------------------------------------------------------------------------------------------------------
#include "MRowSet.h"
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
// class MRowSet
//------------------------------------------------------------------------------------------------------------------------
MRowSet::MRowSet( void )
: FRowCount( 0 )
{
	FCurrIter = FRowList.begin();
}
//------------------------------------------------------------------------------------------------------------------------
MRowSet::~MRowSet()
{
	Clear( );
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MRowSet::MoveFirst( void )
{
	if ( ( FCurrIter = FRowList.begin() ) == FRowList.end() ) ///< check if head == tail
		return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MRowSet::MoveLast( void )
{
	if ( ( FCurrIter = FRowList.end() ) == FRowList.begin() ) ///< check if tail == head
		return FALSE;
	FCurrIter--; ///< move to last row
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MRowSet::MoveNext( void )
{
	if ( FCurrIter == FRowList.end() ) ///< check if reach tail
		return FALSE;
	FCurrIter++; ///< move to next row
	if ( FCurrIter == FRowList.end() ) ///< check again if reach tail
		return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MRowSet::MovePrevious( void )
{
	if ( FCurrIter == FRowList.begin() ) ///< check if reach head
		return FALSE;
	FCurrIter--; ///< move to previous row
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
MRow MRowSet::GetCurrRow( void )
{
	if ( FCurrIter == FRowList.end() ) ///< check if reach head
		return MRow( (MRow*)NULL );
	else
		return **FCurrIter;
}
//------------------------------------------------------------------------------------------------------------------------
void MRowSet::AddRow( MRow* RowPtr )
{
	FRowList.push_back( new MRow( RowPtr ) );
	FRowCount++;
}
//------------------------------------------------------------------------------------------------------------------------
int MRowSet::GetRowCount( void )
{
	return FRowCount;
}
//------------------------------------------------------------------------------------------------------------------------
void MRowSet::Clear( void )
{
	if ( FRowCount > 0 )
	{
		list<MRow*>::iterator RowIter;
		for ( RowIter = FRowList.begin(); RowIter != FRowList.end(); RowIter++ )
		{
			delete (*RowIter);
			*RowIter = NULL;
		}
		FRowList.clear();
		FRowCount = 0;
	}
}
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
