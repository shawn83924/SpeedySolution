//------------------------------------------------------------------------------------------------------------------------
#ifndef __MIndex_H
#define __MIndex_H
//------------------------------------------------------------------------------------------------------------------------
#include "MDSType.h"
#include "MSchema.h"
#include "MRow.h"
#include "MRowSet.h"
#include "MDSCommObj.h"
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
using namespace std;
#ifndef __GXX_EXPERIMENTAL_CXX0X__    
using namespace Sgi;
#endif
//------------------------------------------------------------------------------------------------------------------------
#if (__BORLANDC__ >= 0x0580) ///< For BDS 2006 Dinkum C++ Library
	typedef hash_map< MDSBinaryObj, MRow* >		TBinToMap;
	typedef hash_multimap< MDSBinaryObj, MRow* >	TBinToMultiMap;
#else
	typedef hash_map< MDSBinaryObj, MRow*, hash< MDSBinaryObj > >		TBinToMap;
	typedef hash_multimap< MDSBinaryObj, MRow*, hash< MDSBinaryObj > >	TBinToMultiMap;
#endif
//------------------------------------------------------------------------------------------------------------------------
// class MIndexObjectUnique
//------------------------------------------------------------------------------------------------------------------------
class MIndexObjectUnique
{
//-------------------------
friend class MTable;
//-------------------------
private:
	MIndexSchema*	FIndexSchema;
	TBinToMap		FContainer;
public:
	BOOL			Add( MDSBinaryObj& BinaryObj, MRow* pRow );
	void			Delete( MDSBinaryObj& BinaryObj, MRow* pRow );
	MRow*			GetRowByKey( MDSBinaryObj& BinaryObj );
	// Caller need maintain RowSet life cycle, for example, caller need pass an RowSet is empty in this function
	void			GetRowSetByKey( MRowSet& RowSet, MDSBinaryObj& BinaryObj );
public:
	MIndexObjectUnique( MIndexSchema* pIndexSchema, int MapSize );
	virtual ~MIndexObjectUnique();
};
//------------------------------------------------------------------------------------------------------------------------
// class MIndexObjectMultiple
//------------------------------------------------------------------------------------------------------------------------
class MIndexObjectMultiple
{
//-------------------------
friend class MTable;
//-------------------------
private:
	MIndexSchema*	FIndexSchema;
	TBinToMultiMap	FContainer;
public:
	BOOL			Add( MDSBinaryObj& BinaryObj, MRow* pRow );
	void			Delete( MDSBinaryObj& BinaryObj, MRow* pRow );
	MRow*			GetRowByKey( MDSBinaryObj& BinaryObj );
	// Caller need maintain RowSet life cycle, for example, caller need pass an RowSet is empty in this function
	void			GetRowSetByKey( MRowSet& RowSet, MDSBinaryObj& BinaryObj );
public:
	MIndexObjectMultiple( MIndexSchema* pIndexSchema, int MapSize );
	virtual ~MIndexObjectMultiple();
};
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
