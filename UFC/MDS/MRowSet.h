//------------------------------------------------------------------------------------------------------------------------
#ifndef __MRowSet_H
#define __MRowSet_H
//------------------------------------------------------------------------------------------------------------------------
#include <list>
#include "MDSType.h"
#include "MRow.h"
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
class MRowSet
{
//-------------------------
friend class MTable;
friend class MIndexObjectMultiple;
//-------------------------
private:
	// Note: you should not assume list.size() is constant time.
	// It is permitted to be O(N), where N is the number of elements in the list.
	// If you wish to test whether a list is empty, you should write list.empty() rather than list.size() == 0. 
	list<MRow*>				FRowList;
private:
	list<MRow*>::iterator	FCurrIter;
	int						FRowCount; ///< !! control by MIndexObjectMultiple
public:
	BOOL	MoveFirst( void );
	BOOL	MoveLast( void );
	BOOL	MoveNext( void );
	BOOL	MovePrevious( void );
	MRow	GetCurrRow( void );
	void	AddRow( MRow* RowPtr );
	int		GetRowCount( void );
	void	Clear( void );
public:
	MRowSet( void );
	virtual ~MRowSet( void );
};
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
