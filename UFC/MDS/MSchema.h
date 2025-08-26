//------------------------------------------------------------------------------------------------------------------------
#ifndef __MSchema_H
#define __MSchema_H
//------------------------------------------------------------------------------------------------------------------------
#include "MDSType.h"
#include "MColumn.h"
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
// class MIndexSchema
//------------------------------------------------------------------------------------------------------------------------
class MIndexSchema
{
//-------------------------
friend class MTableSchema;
friend class MTable;
//-------------------------
private:
	UFC::AnsiString		FIndexName;
	// the binary size of this index, when STRING column
	// 1. will no append '\0'
	// 2. need padding '\0' if data is not rech Column length
	// ex : STRING(5) --> this column will occupied 5 bytes
	// ex : STRING(5) Column = "Allen" Index = "Allen"
	// ex : STRING(7) Column = "Allen" Index = "Allen\0\0"
	int					FIndexSize;
	BOOL				FIsPK;
	BOOL				FIsUnique;
	UFC::List<int>		FIndexColumnIdxList;///< the ColumnIdx list of this index
private:
	MIndexSchema( void );
	virtual ~MIndexSchema( void );
};
//------------------------------------------------------------------------------------------------------------------------
// class MTableSchema
//------------------------------------------------------------------------------------------------------------------------
class MTableSchema
{
//-------------------------
friend class MRowInstance;
friend class MRow;
friend class MTable;
//-------------------------
private:
	UFC::AnsiString										FSchemaName;
	UFC::AnsiString										FSchemaDir;
	// The length of Row, STRING column will append '\0'
	// ex : STRING(5) --> this column will occupied 6 bytes
	// ex : STRING(5) Data = "Allen" Data = "Allen\0"
	// ex : STRING(7) Data = "Allen" Data = "Allen\0\0\0"
	int													FRowSize;
private: ///< define at xxx.comm file
	int													FExpectRowAmount;
	BOOL												FIsFixedDataFile;
	int													FFixedDataTotalLength;
private: ///< For columns and indexs
	UFC::PtrList<MColumnDefine>							FColumnDefines;			///< Array of Column Definition
	UFC::PHashMap<UFC::AnsiString, int>					FNameToColumnIdxMap;	///< Column name to ColumnIdx map
	UFC::PHashMap<UFC::AnsiString, MIndexSchema*>		FNameToIndexSchemaMap;	///< Index name to Index map
private:
	enumColumnType		ReadType( UFC::Section* pSection );
	int					ReadMaxLength( UFC::Section* pSection );
	enumStrActionType	ReadStrAction( UFC::Section* pSection );
	int					ReadShowPoint( UFC::Section* pSection );
	int					ReadDataFilePos( UFC::Section* pSection );
	int					ReadDataFileLen( UFC::Section* pSection );
	BOOL				ReadBoolValue( UFC::Section* pSection, const UFC::AnsiString& Name );
private:
	int					GetColumnIdxByName( const UFC::AnsiString& ColumnName );
	MColumnDefine*		GetColumnDefine( int Idx );
	void				AddColumnDefine( const UFC::AnsiString& ColumnName,
										 enumColumnType Type,
										 int MaxLength,
										 enumStrActionType StrAction,
										 int ShowPoint,
										 int DataFilePos,
										 int DataFileLen );
	void				AddIndexToMap( const UFC::AnsiString& IndexName, UFC::Section* pSection );
private:
	void				BuildNameToColumnIdxMap( void );
	void				LoadSchema_CommFile( const UFC::AnsiString& FileName );
	void				LoadSchema_SchemaFile( const UFC::AnsiString& FileName );
	void				LoadSchema_IndexFile( const UFC::AnsiString& FileName );
public:
	MTableSchema( const UFC::AnsiString& Name, const UFC::AnsiString& SchemaDir );
	virtual ~MTableSchema( void );
};
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
