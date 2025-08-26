//------------------------------------------------------------------------------------------------------------------------
#ifndef __MTransactionStore_H
#define __MTransactionStore_H
//------------------------------------------------------------------------------------------------------------------------
#include "MDSType.h"
#include "MTable.h" ///< Mutual include with MTransactionStore.h
#include <queue>
//------------------------------------------------------------------------------------------------------------------------
// *** MDS TransactionStore tlog file format Description
//------------------------------------------------------------------------------------------------------------------------
// Path        : ../dat/xxx.tlog
// Format      : TransID(10)|Action(1)|TableName(n)|Data1(n)|Data2(n)|...|MagicFlag(1)
//               TransID(10)|Action-FinishTrans(1)|@
// TransID     : "0000000000" is mean no Trans, else mean is Trans
// Action      : 'I' -> Insert(New)
//               'D' -> Delete
//               'U' -> Update
//               'F' -> FinishTrans
// TableName   : Table name
// Data1~n     : format same as Table.DATA
// MagicFlag   : '@'
//
// Example     :
// 0000000000|I|CUSTOM|0002219|D200827981|1|N|0004|N|004|F04|@
// 0000000000|U|CUSTOM|0002219|D200827981|1|N|0004|Y|004|F04|@
// 0000000001|U|POSITION|0012452|TXFH8     |1    |0    |0|0|0|0|@
// 0000000001|U|ALARM_LIMIT|P101495079|TXF       |1|N|0|6.0000         |0.0000         |0|0|@
// 0000000001|F|@
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
const char mdsActionInsert      = 'I';
const char mdsActionDelete      = 'D';
const char mdsActionUpdate      = 'U';
const char mdsActionFinishTrans = 'F';
//------------------------------------------------------------------------------------------------------------------------
// class MTransactionStore
//------------------------------------------------------------------------------------------------------------------------
class MTable; ///< Predefinition for MTransactionStore
//------------------------------------------------------------------------------------------------------------------------
class MTransactionStore
{
//-------------------------
friend class MTable;
//-------------------------
private:
	typedef std::queue< UFC::AnsiString* >				TQ_TransLine;
#if (__BORLANDC__ >= 0x0580) ///< For BDS 2006 Dinkum C++ Library
	typedef hash_map< int, TQ_TransLine* >	TM_Trans;
#else
	typedef hash_map< int, TQ_TransLine*, hash<int> >	TM_Trans;
#endif
private:
	UFC::AnsiString			FTransactionStoreName;
	UFC::AnsiString			FTransactionStoreDir;
	BOOL					FIsTruncateStore;
	BOOL					FIsRecovered; ///< to Handle just can call Recover() one time
	int						FTransID;
private:
	UFC::AnsiString			FTransLogFileName;
	UFC::PCriticalSection	FTransLogFileCS;
	UFC::FileStreamEx*		FTransLogFile;
	UFC::AnsiString			FTransErrFileName;
	UFC::FileStreamEx*		FTransErrFile;
private:
	UFC::PHashMap< UFC::AnsiString, MTable* >	FNameToTableMap;
	TM_Trans									FTransIDToLineQMap;
private:
	// friend to MTable call
	void	RegisterTable( MTable* Table );
	void	SyncSimpleAction( char Action, const UFC::AnsiString& TableName, MRow& Row, int TransID = 0 );
private:
	void	KeepActionByTransID( int TransID, char* LineBuffer );
	void	DoAllActionByTransID( int TransID );
	BOOL	ParseAction( char* LineBuffer );
	BOOL	RecoverFromString( char* LineBuffer );
	void	GetActionHeader2( char* LineBuffer, int& TransID, char& Action );
	BOOL	GetActionHeader3( char* LineBuffer, int& TransID, char& Action, char* TableName, char*& DataBeginPos );
	void	CheckRecoverError( void );
public:
	int		GetTransID( void );
	void	FinishTransaction( int TransID );
	void	Recover( void ); ///< Just one thread can call this method.
public:
	MTransactionStore( const UFC::AnsiString& Name, BOOL IsTruncateStore, const UFC::AnsiString& TransactionStoreDir );
	virtual ~MTransactionStore( void );
};
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
