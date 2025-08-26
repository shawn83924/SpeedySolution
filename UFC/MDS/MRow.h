//------------------------------------------------------------------------------------------------------------------------
#ifndef __MRow_H
#define __MRow_H
//------------------------------------------------------------------------------------------------------------------------
#include "MDSType.h"
#include "MSchema.h"
#include "MColumn.h"
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
using namespace std;
#ifndef __GXX_EXPERIMENTAL_CXX0X__
using namespace Sgi;
#endif
//------------------------------------------------------------------------------------------------------------------------
// class MRowInstance
//------------------------------------------------------------------------------------------------------------------------
class MRowInstance
{
//-------------------------
friend class MRow;
friend class MTable;
friend class MTransactionStore;
//-------------------------
private:
	typedef vector< MColumn* >	TVectorColumn;
private:
	int							FID;
	int							FRef;
	UFC::PCriticalSection		FCS;
	MTableSchema*				FTableSchemaPtr;
	UInt8*						FData;
	void*						FAttachObject; ///< new and delete control by caller
	TVectorColumn*				FColumns;
private:
	void     Initial( void );
	MColumn* GetColumn( const UFC::AnsiString& ColumnName );
	MColumn* GetColumn( int Idx );
	UInt8    GetState( void );
	void     SetState( UInt8 Value );
	BOOL     CanModifyColumn( MColumn* pColumn );
public:
	MRowInstance( int RowID, MTableSchema* pTableSchema, UInt8* pData = NULL );
	virtual ~MRowInstance( void );
};
//------------------------------------------------------------------------------------------------------------------------
// class MRow
//------------------------------------------------------------------------------------------------------------------------
class MRow
{
//-------------------------
friend class MTable;
friend class MTransactionStore;
//-------------------------
private:
	MRowInstance*	FInstance;
private:
	void			IncreaseReferenceCount( void );
	void			IncreaseReferenceCount( MRowInstance* InstPtr );
	void			DecreaseReferenceCount( void );
	void			DecreaseReferenceCount( MRowInstance* InstPtr );
public:
	UFC::AnsiString GetColumnAsString( const UFC::AnsiString& ColumnName ) const;
	UFC::AnsiString GetColumnAsString( int Idx ) const;
	char			GetColumnAsChar( const UFC::AnsiString& ColumnName ) const;
	char			GetColumnAsChar( int Idx ) const;
	Int32			GetColumnAsInt( const UFC::AnsiString& ColumnName ) const;
	Int32			GetColumnAsInt( int Idx ) const;
	Int64			GetColumnAsInt64( const UFC::AnsiString& ColumnName ) const;
	Int64			GetColumnAsInt64( int Idx ) const;
	double			GetColumnAsDouble( const UFC::AnsiString& ColumnName ) const;
	double			GetColumnAsDouble( int Idx ) const;
	MColumn*		GetColumn( const UFC::AnsiString& ColumnName );
	MColumn*		GetColumn( int Idx );
	void			SetColumn( const UFC::AnsiString& ColumnName, const UFC::AnsiString& ColumnValue );
	void			SetColumn( const UFC::AnsiString& ColumnName, const char ColumnValue );
	void			SetColumn( const UFC::AnsiString& ColumnName, const int ColumnValue );
	void			SetColumn( const UFC::AnsiString& ColumnName, const Int64 ColumnValue );
	void			SetColumn( const UFC::AnsiString& ColumnName, const double ColumnValue );
	void			SetColumn( int Idx, const UFC::AnsiString& ColumnValue );
	void			SetColumn( int Idx, const char ColumnValue );
	void			SetColumn( int Idx, const int ColumnValue );
	void			SetColumn( int Idx, const Int64 ColumnValue );
	void			SetColumn( int Idx, const double ColumnValue );
public:
	BOOL			IsNull( void );
	void			SetNull( void );
	void			Delete( void ); ///< Just remark delete flag, until MTable::Update(), then Remove from memory at MTable
	BOOL			CheckState( UInt8 Mask ) const;
	int				GetRowID( void ) const;
	void			SetAttachObject( void* Obj );
	void*			GetAttachObject( void ) const; ///< return NULL when no AttachObject
public:
	MRow&			operator = ( const MRow& Row );
public:
	MRow( void );
	MRow( MRowInstance* pInstance );
	MRow( const MRow& Row );
	MRow( MRow* pRow );
	virtual ~MRow( void );
};
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
