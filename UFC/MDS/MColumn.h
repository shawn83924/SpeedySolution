//------------------------------------------------------------------------------------------------------------------------
#ifndef __MColumn_H
#define __MColumn_H
//------------------------------------------------------------------------------------------------------------------------
#include "MDSType.h"
//------------------------------------------------------------------------------------------------------------------------
#ifndef mdsFloatEqual
#define mdsFloatEqual_Tolerance 0.0000001
#define mdsFloatEqual(x,v) ( ( ( (v) - mdsFloatEqual_Tolerance) < (x) ) && ( (x) < ( (v) + mdsFloatEqual_Tolerance ) ) )
#endif
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
// class MColumnDefine
//------------------------------------------------------------------------------------------------------------------------
class MColumnDefine
{
public:
	UFC::AnsiString		FName;
	enumColumnType		FType;
	BOOL				FIsIndexMember;
	// The length of Column, STRING column will append '\0'
	// ex : STRING(5) --> this column will occupied 6 bytes
	// ex : STRING(5) Data = "Allen" Data = "Allen\0"
	// ex : STRING(7) Data = "Allen" Data = "Allen\0\0\0"
	int					FMaxLength;		///< When type is String, Need set.
	enumStrActionType	FStrAction;		///< When type is String, Could set.
	int					FShowPoint;		///< When type is Double, Could set.
	int					FDataFilePos;	///< When data file is fixed format, Need set.
	int					FDataFileLen;	///< When data file is fixed format, Need set.
};
//------------------------------------------------------------------------------------------------------------------------
// class MColumn
//------------------------------------------------------------------------------------------------------------------------
class MColumn
{
//-------------------------
friend class MTable;
//-------------------------
private:
	MColumnDefine*	FDefinition;
	UInt8*			FData;	///< just buffer pointer to offset of MRow::FData
public:
	const UFC::AnsiString	ToString( void );
	const char				ToChar( void );
	const int				ToInt( void );
	const Int64				ToInt64( void );
	const double			ToDouble( void );
public:
	void FromString( const UFC::AnsiString& Data );
	void FromChar( const char Data );
	void FromInt( const int Data );
	void FromInt64( const Int64 Data );
	void FromDouble( const double Data );
public:
	BOOL IsIndexMemberColumn( void );
public:
	MColumn( MColumnDefine* pColumnDefine, UInt8* Data );
	virtual ~MColumn( void );
};
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
