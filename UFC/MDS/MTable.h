//------------------------------------------------------------------------------------------------------------------------
#ifndef __MTable_H
#define __MTable_H
//------------------------------------------------------------------------------------------------------------------------
#include "MDSType.h"
#include "MRow.h"
#include "MIndex.h"
#include "MTransactionStore.h" ///< Mutual include with MTable.h
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
// class MTableListener
//------------------------------------------------------------------------------------------------------------------------
class MTableListener
{
public:
	// When use MTableListener to Attach Object to Row, caller should delete Attach Object OnDeleteRow()
	// because MRow doesn't know variable type of Attach Object, so, MRow could not delete it.
	virtual void OnNewRow( MRow* pRow, void* ExtraParm ) = 0;
	virtual void OnDeleteRow( MRow* pRow, void* ExtraParm ) = 0;
public:
	virtual ~MTableListener() {}
};
//------------------------------------------------------------------------------------------------------------------------
// class MTable
//------------------------------------------------------------------------------------------------------------------------
class MTransactionStore; ///< Predefinition for MTable
//------------------------------------------------------------------------------------------------------------------------
class MTable
{
//-------------------------
friend class MTransactionStore;
//-------------------------
private:
#if (__BORLANDC__ >= 0x0580) ///< For BDS 2006 Dinkum C++ Library
	typedef hash_map< UFC::AnsiString, MIndexObjectUnique* >		TNameToIndexObjU;
	typedef hash_map< UFC::AnsiString, MIndexObjectMultiple* >		TNameToIndexObjM;
#else
	typedef hash_map< UFC::AnsiString, MIndexObjectUnique*, hash< UFC::AnsiString > >		TNameToIndexObjU;
	typedef hash_map< UFC::AnsiString, MIndexObjectMultiple*, hash< UFC::AnsiString > >		TNameToIndexObjM;
#endif
	typedef vector< MIndexObjectUnique* >													TVectorIndexObjU;
	typedef vector< MIndexObjectMultiple* >													TVectorIndexObjM;
	typedef hash_map< int, MRow*>															TIntToMRowPtr;
private:
	UFC::AnsiString					FTableName;
	UFC::AnsiString					FSchemaDir;
	UFC::AnsiString					FTableDir;
	MTableListener*					FTableListener;
	MTransactionStore*				FTransactionStore;
	MTableSchema					FTableSchema;
	UFC::PCriticalSection			FCS;
	int								FDefaultSeqNum;
	TIntToMRowPtr					FRowIDHash;
	TIntToMRowPtr::iterator			FRowIDHash_CurrIter;
private: ///< Index member
	MIndexObjectUnique*				FPkIndex;				///< Primary Key Index reference
	TNameToIndexObjU				FUniqueIndexs;			///< IndexName to Unique Index Collection
	TNameToIndexObjM				FMultipleIndexs;		///< IndexName to Multiple Index Collection
	// For Performance issue, use vector to visit all element of index
	// because use hash_map.begin() will invoke stl to go to left leaf
	TVectorIndexObjU				FUniqueIndexsVector;	///< Unique Index Vector
	TVectorIndexObjM				FMultipleIndexsVector;	///< Multiple Index Vector
private:
	void	CreateIndexCollection( void );
	void	LoadFromDataFile( const UFC::AnsiString& FileName );
	void	SetBinaryObject( MDSBinaryObj& BinaryObj, MIndexSchema* pIndexSchema, MRow* pRow );
	void	AddToIndexObjectUnique( MIndexObjectUnique* pIndexObject, MRow* pRow );
	void	AddToIndexObjectMultiple( MIndexObjectMultiple* pIndexObject, MRow* pRow );
	BOOL	AddIntoRowList( MRow& Row, void* ExtraParm );	///< 1. Check if duplicate
															///< 2. new MRow to keep MRowInstance reference
															///< 3. Build each Unique Index Collection
															///< 4. Build each Multiple Index Collection
															///< 5. Build FRowIDHash Index
															///< 6. Call MTableListener->OnNewRow() when have Listener
	void	RemoveFromIndexObjectUnique( MIndexObjectUnique* pIndexObject, MRow* pRow );
	void	RemoveFromIndexObjectMultiple( MIndexObjectMultiple* pIndexObject, MRow* pRow );
	void	RemoveFromRowList( MRow& Row, void* ExtraParm );///< 1. get MRow* to delete MRowInstance reference
															///< 2. Erase each Unique Index Collection
															///< 3. Erase each Multiple Index Collection
															///< 4. Erase FRowIDHash Index
															///< 5. Call MTableListener->OnDeleteRow() when have Listener
															///< 6. delete MRow* reference
	void	Sync( char Action, MRow& Row, int TransID = 0 ); ///< 1. Sync. to TransactionStore file if own
															 ///< 2. Clear All Transaction state except ROW_STATE_DELETE
private:
	void	DoStrAction( UFC::AnsiString& StrValue, MColumnDefine* pColumnDefine );
	BOOL	NewRowByFixedTextData( MRow& Row, char* TextData );
	BOOL	NewRowByTextData( MRow& Row, char* TextData );
	BOOL	RetrieveRowByPKofTextData( MRow& Row, UFC::PStringList& StrList, const UFC::AnsiString& TextData );
	BOOL	RecoverInsertAction( char* DataBuffer );	///< friend to MTransactionStore::RecoverFromString call
	BOOL	RecoverDeleteAction( char* DataBuffer );	///< friend to MTransactionStore::RecoverFromString call
	BOOL	RecoverUpdateAction( char* DataBuffer );	///< friend to MTransactionStore::RecoverFromString call
public:
	BOOL	IsDuplicateRow( MRow& Row );
	MRow	NewRow( void );
	BOOL	Update( MRow& Row, int TransID = 0, void* ExtraParm = NULL );
public:
	int						GetIndexSizeByIndexName( const UFC::AnsiString& IndexName );
	MIndexObjectUnique*		GetIndexObjectUnique( const UFC::AnsiString& IndexName );
	MIndexObjectMultiple*	GetIndexObjectMultiple( const UFC::AnsiString& IndexName );
public: ///< GetRow functions
	int		GetRowCount( void );
	MRow	GetRowByID( int RowID );
	BOOL	GetRowByPK( MRow& Row, MDSBinaryObj& BinaryObj );
	// GetRowByIndexName() just find data by all MIndexObjectUnique
	BOOL	GetRowByIndexName( MRow& Row, const UFC::AnsiString& IndexName, MDSBinaryObj& BinaryObj );
	BOOL	GetRowByIndexObject( MRow& Row, MIndexObjectUnique* pIndexObject, MDSBinaryObj& BinaryObj );
	// GetRowSetByIndexName just find data by all MIndexObjectMultiple
	// Caller need maintain RowSet life cycle, for example, caller need pass an RowSet is empty in this function
	BOOL	GetRowSetByIndexName( MRowSet& RowSet, const UFC::AnsiString& IndexName, MDSBinaryObj& BinaryObj );
	BOOL	GetRowSetByIndexObject( MRowSet& RowSet, MIndexObjectMultiple* pIndexObject, MDSBinaryObj& BinaryObj );
public: ///< Browse table functions
	BOOL	MoveFirst( void );
	BOOL	MoveNext( void );
	MRow	GetCurrRow( void );
public:
	int				GetColumnCount( void )									{ return FTableSchema.FColumnDefines.ItemCount(); }
	MColumnDefine*	GetColumnDefine( int Idx )								{ return FTableSchema.GetColumnDefine( Idx ); }
	UFC::List<int>*	GetIndexListByIndexName( const UFC::AnsiString& Name );
public:
	MTable( const UFC::AnsiString& Name,
			const UFC::AnsiString& SchemaDir,
			const UFC::AnsiString& TableDir,
			MTableListener* Listener = NULL,
			MTransactionStore* TransactionStore = NULL );
	virtual ~MTable( void );
};
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
