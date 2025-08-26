#ifndef _MDefinitions_h_
#define _MDefinitions_h_
//------------------------------------------------------------------------------
#include "../UFC/UFC.h"
//------------------------------------------------------------------------------
typedef enum
{
	M_UNKNOW	 =  0,
	M_TREE	     =  2,
	M_STRING	 =  3,
	M_BOOLEAN    =  4, ///< Not implement yet.
	M_INT	     =  5,
	M_UINT       =  6,
	M_INT64      =  7,
	M_DATETIME	 =  8, ///< Not implement yet.
	M_SHORT	     = 11,
	M_USHORT	 = 12,
	M_LONG	     = 13,
	M_ULONG	     = 14,
	M_FLOAT	     = 15,
	M_DOUBLE	 = 16,
	M_ROWDATA    = 17,
    M_COMPRESSEDDATA = 18

} MDataType;
//------------------------------------------------------------------------------
typedef long             MSize;
typedef UFC::AnsiString  MString;
typedef UFC::PStringList MStringList;
typedef BOOL             Mboolean;
//------------------------------------------------------------------------------
#define Mtrue TRUE
#define Mfalse FALSE

#endif // MDefinitions_h

