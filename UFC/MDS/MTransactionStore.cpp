//------------------------------------------------------------------------------------------------------------------------
#include "MTransactionStore.h"
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
// class MTransactionStore
//------------------------------------------------------------------------------------------------------------------------
MTransactionStore::MTransactionStore( const UFC::AnsiString& Name, BOOL IsTruncateStore, const UFC::AnsiString& TransactionStoreDir )
: FTransactionStoreName( Name )
, FTransactionStoreDir( TransactionStoreDir )
, FIsTruncateStore( IsTruncateStore )
, FIsRecovered( FALSE )
, FTransID( 0 )
{
	if ( IsTruncateStore == TRUE )
		FIsRecovered = TRUE; ///< if Truncate Store, no need to Recover

	// Open .tlog & .terr
	FTransLogFileName.Printf( "%s/%s.tlog", FTransactionStoreDir.c_str(), Name.c_str() );
	FTransErrFileName.Printf( "%s/%s.terr", FTransactionStoreDir.c_str(), Name.c_str() );
	if ( FIsTruncateStore )
	{
		// .tlog
		FTransLogFile = new UFC::FileStreamEx( FTransLogFileName, "w+" );
		UFC::BufferedLog::Printf( " [%s] TStore Open [%s] By Truncate."
								  , FTransactionStoreName.c_str(), FTransLogFileName.c_str() );
		// .terr
		FTransErrFile = new UFC::FileStreamEx( FTransErrFileName, "w+" );
		UFC::BufferedLog::Printf( " [%s] TStore Open [%s] By Truncate."
								  , FTransactionStoreName.c_str(), FTransErrFileName.c_str() );
	}
	else
	{
		// .tlog
                if ( UFC::FileExists( FTransLogFileName ) == TRUE )
                {
                        FTransLogFile = new UFC::FileStreamEx( FTransLogFileName, "r+" );
                        FTransLogFile->Seek( 0, UFC::soFromEnd ); ///< Seek to end of file, becuase "r+" will not do this
                }
                else
                {
                        FTransLogFile = new UFC::FileStreamEx( FTransLogFileName, "w+" );
                }
		UFC::BufferedLog::Printf( " [%s] TStore Open [%s]", FTransactionStoreName.c_str(), FTransLogFileName.c_str() );
		// .terr
		FTransErrFile = new UFC::FileStreamEx( FTransErrFileName, "a+" );
		UFC::BufferedLog::Printf( " [%s] TStore Open [%s]", FTransactionStoreName.c_str(), FTransErrFileName.c_str() );
	}
}
//------------------------------------------------------------------------------------------------------------------------
MTransactionStore::~MTransactionStore( void )
{
}
//------------------------------------------------------------------------------------------------------------------------
void MTransactionStore::RegisterTable( MTable* Table )
{
	FNameToTableMap.Add( Table->FTableName, Table );
}
//------------------------------------------------------------------------------------------------------------------------
void MTransactionStore::SyncSimpleAction( char Action, const UFC::AnsiString& TableName, MRow& Row, int TransID )
{
	char LineBuffer[8192]; ///< attention this -> limitation of bytes for sync. to file.
	char *Pos = LineBuffer;

	if ( Row.FInstance == NULL )
		return; ///< ignore any internal error

	sprintf( Pos, "%010d|%c|%s|", TransID, Action, TableName.c_str() );
	Pos += ( 14 + TableName.Length() );

	vector<MColumn*> *pColumns = Row.FInstance->FColumns;
	for ( int i = 0; i < (int)pColumns->size(); i++ )
	{
		MColumn* pColumn = (*pColumns)[ i ];
		if ( pColumn == NULL )
			return; ///< ignore any internal error

		UFC::AnsiString	TempStr    = pColumn->ToString();
		int				TempStrLen = TempStr.Length();

		memcpy( Pos, TempStr.c_str(), TempStrLen );
		Pos[ TempStrLen ] = '|';
		Pos += ( TempStrLen + 1 );
	}
	strcpy( Pos, "@\n" );

	// Write to file
	UFC::PLockObject Locker( FTransLogFileCS );
	FTransLogFile->Write( LineBuffer, strlen(LineBuffer) );
	FTransLogFile->Flush( );
}
//------------------------------------------------------------------------------------------------------------------------
void MTransactionStore::KeepActionByTransID( int TransID, char* LineBuffer )
{
	TM_Trans::iterator	Iter;
	TQ_TransLine*		Q_TransLine;

	// Find Trans queue by TransID
	if ( ( Iter = FTransIDToLineQMap.find( TransID ) ) == FTransIDToLineQMap.end() )
	{
		Q_TransLine = new TQ_TransLine;
		FTransIDToLineQMap[ TransID ] = Q_TransLine; ///< Add TQ_TransLine
	}
	else
		Q_TransLine = Iter->second;

	// Push Line to Trans queue
	Q_TransLine->push( new UFC::AnsiString(LineBuffer) );
}
//------------------------------------------------------------------------------------------------------------------------
void MTransactionStore::DoAllActionByTransID( int TransID )
{
	TM_Trans::iterator	Iter;
	TQ_TransLine*		Q_TransLine;

	// Find Trans queue by TransID
	if ( ( Iter = FTransIDToLineQMap.find( TransID ) ) == FTransIDToLineQMap.end() )
	{
		UFC::BufferedLog::Printf( " [%s] TStore TransID:%d can not find Q_TransLine!!"
								  , FTransactionStoreName.c_str(), TransID );
		return;
	}

	Q_TransLine = Iter->second;
	while ( Q_TransLine->size() > 0 )
	{
		UFC::AnsiString* Line = Q_TransLine->front();
		if ( RecoverFromString( (char*)Line->c_str() ) == FALSE )
		{
			UFC::BufferedLog::Printf( " [%s] MTransactionStore::DoAllActionByTransID() TransID:%d Error ->\n[%s]"
									  , FTransactionStoreName.c_str(), TransID, Line->c_str() );
		}

		delete Line;
		Q_TransLine->pop();
	}

	FTransIDToLineQMap.erase( TransID );
	delete Q_TransLine;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTransactionStore::ParseAction( char* LineBuffer )
{
	int LineLen = strlen( LineBuffer );
	if ( LineLen < 14 || LineBuffer[LineLen-2] != '@' ) ///< least need 14 digi ( least -> TransID(10)|Action(1)|@\n )
		return FALSE;

	int  TransID;
	char Action;

	GetActionHeader2( LineBuffer, TransID, Action );

	if ( TransID == 0 )
	{
		if ( Action == mdsActionFinishTrans )
			return FALSE; ///< TransID = 0 should not have FinshTrans Action

		return RecoverFromString( LineBuffer );
	}
	else
	{
		// Update last TransID
		if ( TransID > FTransID )
			FTransID = TransID;
		// When FinishTrans do all action by TransID else Keep this Action
		if ( Action == mdsActionFinishTrans )
			DoAllActionByTransID( TransID );
		else
			KeepActionByTransID( TransID, LineBuffer );
	}

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTransactionStore::RecoverFromString( char* LineBuffer )
{
	int  TransID;
	char Action;
	char TableName[64];
	char *DataBeginPos;
	BOOL RtnVal;

	if ( GetActionHeader3( LineBuffer, TransID, Action, TableName, DataBeginPos ) == FALSE )
		return FALSE;

	MTable* Table;
	if ( FNameToTableMap.GetObjectByKey( TableName, Table ) == FALSE )
		return FALSE;
	if ( Table == NULL )
		return FALSE;

	switch ( Action )
	{
	case mdsActionInsert:
		RtnVal = Table->RecoverInsertAction( DataBeginPos );
		break;
	case mdsActionDelete:
		RtnVal = Table->RecoverDeleteAction( DataBeginPos );
		break;
	case mdsActionUpdate:
		RtnVal = Table->RecoverUpdateAction( DataBeginPos );
		break;
	default:
		return FALSE;
	}

	return RtnVal;
}
//------------------------------------------------------------------------------------------------------------------------
void MTransactionStore::GetActionHeader2( char* LineBuffer, int& TransID, char& Action )
{
	char strTransID[11];

	strncpy_zero( strTransID, LineBuffer, 10 );
	TransID = atoi( strTransID );
	Action = LineBuffer[11];
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MTransactionStore::GetActionHeader3( char* LineBuffer, int& TransID, char& Action, char* TableName, char*& DataBeginPos )
{
	char strTransID[11];

	strncpy_zero( strTransID, LineBuffer, 10 );
	TransID = atoi( strTransID );
	Action = LineBuffer[11];

	if ( Action == mdsActionFinishTrans )
	{
		TableName[0] = 0;
		DataBeginPos = NULL;
	}
	else
	{
		char *Pos, *PosDelimiter;

		Pos = LineBuffer+13;
		PosDelimiter = strchr( Pos, '|' );
		if ( PosDelimiter == NULL )
			return FALSE;

		int Len = PosDelimiter - Pos;
		strncpy_zero( TableName, Pos, Len );
		DataBeginPos = LineBuffer + 14 + Len;
	}

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void MTransactionStore::CheckRecoverError( void )
{
	if ( FTransIDToLineQMap.size() == 0 )
		return;

	UFC::AnsiString Msg, TimeNow;
	GetTimeString( TimeNow );
	// Log Header Message
	Msg.Printf( "%s [%s] TStore : There are some Trans data have not be finished ->\n"
				, TimeNow.c_str(), FTransactionStoreName.c_str() );
	FTransErrFile->Write( (char*)Msg.c_str(), Msg.Length() );

	TM_Trans::iterator	Iter;
	// delete all element of FTransIDToLineQMap
	for ( Iter = FTransIDToLineQMap.begin(); Iter != FTransIDToLineQMap.end(); Iter++ )
	{
		TQ_TransLine* Q_TransLine = Iter->second;
		while ( Q_TransLine->size() > 0 )
		{
			UFC::AnsiString* Line = Q_TransLine->front();
			FTransErrFile->Write( (char*)Line->c_str(), Line->Length() ); ///< log Line
			delete Line;
			Q_TransLine->pop();
		}
		delete Q_TransLine;
	}
	FTransErrFile->Flush();		///< flush file
	FTransIDToLineQMap.clear();	///< clear all element
}
//------------------------------------------------------------------------------------------------------------------------
int MTransactionStore::GetTransID( void )
{
	return ( ++FTransID );
}
//------------------------------------------------------------------------------------------------------------------------
void MTransactionStore::FinishTransaction( int TransID )
{
	char LineBuffer[64];
	if ( TransID <= 0 )
		return;

	sprintf( LineBuffer, "%010d|F|@\n", TransID );

	// Write to file
	UFC::PLockObject Locker( FTransLogFileCS );
	FTransLogFile->Write( LineBuffer, strlen(LineBuffer) );
	FTransLogFile->Flush( );
}
//------------------------------------------------------------------------------------------------------------------------
void MTransactionStore::Recover( void )
{
	if ( FIsRecovered == TRUE )
		return;
	FIsRecovered = TRUE;

	char	LineBuffer[8192];
	int		LineCount = 0;
	FILE*	FileHandle;
	int		BeginMs = time_ms();

	UFC::BufferedLog::Printf( " [%s] TStore Begin Recover from [%s]"
							  , FTransactionStoreName.c_str(), FTransLogFileName.c_str() );
	if ( ( FileHandle = fopen(FTransLogFileName.c_str(), "r") ) == NULL )
	{
		UFC::BufferedLog::Printf( " [%s] TStore Open recover file %s failed!!"
								  , FTransactionStoreName.c_str(), FTransLogFileName.c_str() );
		exit(1);
	}

	while ( fgets( LineBuffer, 8192, FileHandle ) != NULL )
	{
		LineCount++;
		if ( ParseAction( LineBuffer ) == FALSE )
		{
			UFC::BufferedLog::Printf( " [%s] TStore Recover encounter illegal line(%d) ->\n[%s]"
									  , FTransactionStoreName.c_str(), LineCount, LineBuffer );
		}
	}
	fclose( FileHandle );
	UFC::BufferedLog::Printf( " [%s] TStore Recover %d lines, use %d ms."
							  , FTransactionStoreName.c_str(), LineCount, time_ms()-BeginMs );

	// When Recover finish, to check if error.
	CheckRecoverError( );
}
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
