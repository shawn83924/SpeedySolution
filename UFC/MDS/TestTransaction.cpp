//------------------------------------------------------------------------------------------------------------------------
#include "TestTransaction.h"
//------------------------------------------------------------------------------------------------------------------------
MTransactionStore*	gTransactionStore;
MTable*				gCustom;
MTable*				gPosition;
MTable*				gAlarmLimit;
MTable*				gCurrentTable;
BOOL				gRecoverMode;
int					gTransID;
//------------------------------------------------------------------------------------------------------------------------
MDSBinaryObj	AcceptPKey( void );
void			PrintToScreen( MRow& Row );
//------------------------------------------------------------------------------------------------------------------------
void PrintToScreen( MRow& Row )
{
	UFC::AnsiString ColumnName;
	enumColumnType ColumnType;

	for (int i=0; i < gCurrentTable->GetColumnCount(); i++ )			
	{	
		ColumnName = gCurrentTable->GetColumnDefine(i)->FName;
		ColumnType = gCurrentTable->GetColumnDefine(i)->FType;

		if ( ColumnType == ctString ) ///< ctString
			printf( "[%s:%s]", ColumnName.c_str(), Row.GetColumnAsString( ColumnName ).c_str() );
		else if ( ColumnType == ctChar ) ///< ctChar
			printf( "[%s:%c]", ColumnName.c_str(), Row.GetColumnAsChar( ColumnName ) );
		else if ( ColumnType == ctInt ) ///< ctInt
			printf( "[%s:%d]", ColumnName.c_str(), Row.GetColumnAsInt( ColumnName ) );
		else if ( ColumnType == ctInt64 ) ///< ctInt64
			printf( "[%s:%lld]", ColumnName.c_str(), Row.GetColumnAsInt64( ColumnName ) );
		else if ( ColumnType == ctDouble ) ///< ctDouble
			printf( "[%s:%lf]", ColumnName.c_str(), Row.GetColumnAsDouble( ColumnName ) );
		printf("\n");
	}
}
//------------------------------------------------------------------------------------------------------------------------
MDSBinaryObj AcceptPKey( void )
{
	UFC::AnsiString ColumnName;
	enumColumnType ColumnType;
	int ColumnMaxLength;
	int IndexSize = 0;

	char InputBuffer[256];
	UFC::List<int>	*PKList = gCurrentTable->GetIndexListByIndexName( "PK" );

	IndexSize = gCurrentTable->GetIndexSizeByIndexName("PK");
	printf(" PKIndexSize=%d\n", IndexSize);

	MDSBinaryObj BinaryObj( IndexSize );

	for (int i=0; i < PKList->ItemCount(); i++) 
	{
		ColumnName = gCurrentTable->GetColumnDefine(PKList->GetItem(i))->FName;
		ColumnType = gCurrentTable->GetColumnDefine(PKList->GetItem(i))->FType;
		ColumnMaxLength = (gCurrentTable->GetColumnDefine(PKList->GetItem(i))->FMaxLength) -1;

		if ( ColumnType == ctString ) ///< ctString
		{
			printf( " Input PKEY[%d]:%s [Type:string] [MaxLength:%d]\n", PKList->GetItem(i), ColumnName.c_str(), ColumnMaxLength );
			scanf( "%s", InputBuffer );

			int len = strlen(InputBuffer);
			if ( (ColumnMaxLength - len) > 0 )
			{
				memset(InputBuffer+len, ' ', (ColumnMaxLength - len ));
			}
			InputBuffer[ColumnMaxLength] = '\0';
			UFC::AnsiString ColumnValue = InputBuffer;
			BinaryObj.AppendAnsiString( ColumnValue, ColumnMaxLength );
		}
		else if ( ColumnType == ctChar ) ///< ctChar
		{
			printf( " Input PKEY[%d]:%s [Type:char]\n", PKList->GetItem(i), ColumnName.c_str() );
			scanf( "%s", InputBuffer );

			BinaryObj.AppendChar( InputBuffer[0] );
		}
		else if ( ColumnType == ctInt ) ///< ctInt
		{
			printf( " Input PKEY[%d]:%s [Type:int]\n", PKList->GetItem(i), ColumnName.c_str() );
			scanf( "%s", InputBuffer );

			int ColumnValue = atoi(InputBuffer);
			BinaryObj.AppendInt( ColumnValue );
		}
		else if ( ColumnType == ctInt64 ) ///< ctInt64
		{
			printf( " Input PKEY[%d]:%s [Type:Int64]\n", PKList->GetItem(i), ColumnName.c_str() );
			scanf( "%s", InputBuffer );

			Int64 ColumnValue = strtoll( InputBuffer, (char**)NULL, 10 );
			BinaryObj.AppendInt64( ColumnValue );
		}
		else if ( ColumnType == ctDouble ) ///< ctDouble
		{
			printf( " Input PKEY[%d]:%s [Type:double]\n", PKList->GetItem(i), ColumnName.c_str() );
			scanf( "%s", InputBuffer );

			double ColumnValue = atof(InputBuffer);
			BinaryObj.AppendDouble( ColumnValue );
		}

	}	
	return BinaryObj;
}
//------------------------------------------------------------------------------------------------------------------------
void GetDataByPK( void )
{
	MDSBinaryObj BinaryObj = AcceptPKey();

	MRow Row;
	if ( gCurrentTable->GetRowByPK( Row, BinaryObj ) == TRUE )
	{
		PrintToScreen(Row);
	}
	else
		printf( "Row not found!!\n");
}
//------------------------------------------------------------------------------------------------------------------------
void AddData( void )
{
	UFC::AnsiString ColumnName;
	enumColumnType ColumnType;
	int ColumnMaxLength;
	char InputBuffer[256];

	MRow Row =gCurrentTable->NewRow( );

	printf( "----Input Data----\n" );
	for (int i=0; i < gCurrentTable->GetColumnCount(); i++ )			
	{	
		ColumnName = gCurrentTable->GetColumnDefine(i)->FName;
		ColumnType = gCurrentTable->GetColumnDefine(i)->FType;
		ColumnMaxLength = (gCurrentTable->GetColumnDefine(i)->FMaxLength) -1;
	
		if ( ColumnType == ctString ) ///< ctString
		{
			printf( " Input '%s' [Type:string] [MaxLength:%d]\n",  ColumnName.c_str(), ColumnMaxLength );
			scanf( "%s", InputBuffer );
			if ( InputBuffer[0] != '!' )
			{
				int len = strlen(InputBuffer);
				if ( (ColumnMaxLength - len) > 0 )
				{
					memset(InputBuffer+len, ' ', (ColumnMaxLength - len ));
				}
				InputBuffer[ColumnMaxLength] = '\0';
				UFC::AnsiString ColumnValue = InputBuffer;
				Row.SetColumn( i, ColumnValue );
			}
		}
		else if ( ColumnType == ctChar ) ///< ctChar
		{
			printf( " Input '%s' [Type:char]\n", ColumnName.c_str() );
			scanf( "%s", InputBuffer );
			if ( InputBuffer[0] != '!' )
				Row.SetColumn( i, InputBuffer[0] );
		}
		else if ( ColumnType == ctInt ) ///< ctInt
		{
			printf( " Input '%s' [Type:int]\n", ColumnName.c_str() );
			scanf( "%s", InputBuffer );
			if ( InputBuffer[0] != '!' )
			{
				int ColumnValue = atoi(InputBuffer);
				Row.SetColumn( i, ColumnValue );
			}
		}
		else if ( ColumnType == ctInt64 ) ///< ctInt64
		{
			printf( " Input '%s' [Type:Int64]\n", ColumnName.c_str() );
			scanf( "%s", InputBuffer );
			if ( InputBuffer[0] != '!' )
			{
				Int64 ColumnValue = strtoll( InputBuffer, (char**)NULL, 10 );
				Row.SetColumn( i, ColumnValue );
			}
		}
		else if ( ColumnType == ctDouble ) ///< ctDouble
		{
			printf( " Input '%s' [Type:double]\n", ColumnName.c_str() );
			scanf( "%s", InputBuffer );
			if ( InputBuffer[0] != '!' )
			{
				double ColumnValue = atof(InputBuffer);
				Row.SetColumn( i, ColumnValue );
			}
		}
	}

	if ( gCurrentTable->Update( Row, gTransID ) == TRUE )
	{
		printf( "Add succeed.\n" );
		PrintToScreen(Row);
	}
	else
		printf( "Add failed.\n" );
}
//------------------------------------------------------------------------------------------------------------------------
void ModifyData( void )
{
	UFC::AnsiString ColumnName;
	enumColumnType ColumnType;
	int ColumnMaxLength;
	char InputBuffer[256];

    MDSBinaryObj BinaryObj = AcceptPKey(); //find out record first

	MRow Row;
	if ( gCurrentTable->GetRowByPK( Row, BinaryObj ) != TRUE )
	{
		printf( "Data Not Found!!\n" );
		return;
	}

	printf( "Before Modify--->\n" );
	PrintToScreen(Row);

	printf( "----Input New Data----('!':No change)\n" );
	for (int i=0; i < gCurrentTable->GetColumnCount(); i++ )			
	{	

		if ( (gCurrentTable->GetColumnDefine(i)->FIsIndexMember) == TRUE )//if (IndexSection > 0 )-->don't work
			continue;

		ColumnName = gCurrentTable->GetColumnDefine(i)->FName;
		ColumnType = gCurrentTable->GetColumnDefine(i)->FType;
		ColumnMaxLength = (gCurrentTable->GetColumnDefine(i)->FMaxLength) -1;
	
		if ( ColumnType == ctString ) ///< ctString
		{
			printf( " Input '%s' [Type:string] [MaxLength:%d]\n",  ColumnName.c_str(), ColumnMaxLength );
			scanf( "%s", InputBuffer );
			if ( InputBuffer[0] != '!' )
			{
				int len = strlen(InputBuffer);
				if ( (ColumnMaxLength - len) > 0 )
				{
					memset(InputBuffer+len, ' ', (ColumnMaxLength - len ));
				}
				InputBuffer[ColumnMaxLength] = '\0';
				UFC::AnsiString ColumnValue = InputBuffer;
				Row.SetColumn( i, ColumnValue );
			}
		}
		else if ( ColumnType == ctChar ) ///< ctChar
		{
			printf( " Input '%s' [Type:char]\n", ColumnName.c_str() );
			scanf( "%s", InputBuffer );
			if ( InputBuffer[0] != '!' )
				Row.SetColumn( i, InputBuffer[0] );
		}
		else if ( ColumnType == ctInt ) ///< ctInt
		{
			printf( " Input '%s' [Type:int]\n",  ColumnName.c_str() );
			scanf( "%s", InputBuffer );
			if ( InputBuffer[0] != '!' )
			{
				int ColumnValue = atoi(InputBuffer);
				Row.SetColumn( i, ColumnValue );
			}
		}
		else if ( ColumnType == ctInt64 ) ///< ctInt64
		{
			printf( " Input '%s' [Type:Int64]\n",  ColumnName.c_str() );
			scanf( "%s", InputBuffer );
			if ( InputBuffer[0] != '!' )
			{
				Int64 ColumnValue = strtoll( InputBuffer, (char**)NULL, 10 );
				Row.SetColumn( i, ColumnValue );
			}
		}
		else if ( ColumnType == ctDouble ) ///< ctDouble
		{
			printf( " Input '%s' [Type:double]\n",  ColumnName.c_str() );
			scanf( "%s", InputBuffer );
			if ( InputBuffer[0] != '!' )
			{
				double ColumnValue = atof(InputBuffer);
				Row.SetColumn( i, ColumnValue );
			}
		}
	}

	if ( gCurrentTable->Update( Row, gTransID ) == TRUE )
	{
		printf( "Modify succeed.\n" );
		printf( "After Modify--->\n" );
		PrintToScreen(Row);
	}
	else
		printf( "Update failed.\n" );
}
//------------------------------------------------------------------------------------------------------------------------
void DeleteData( void )
{
	MDSBinaryObj BinaryObj = AcceptPKey();

	MRow Row;
	if ( gCurrentTable->GetRowByPK( Row, BinaryObj ) == TRUE )
	{
		PrintToScreen(Row);

		Row.Delete();
		if ( gCurrentTable->Update( Row, gTransID ) == TRUE )
			printf( "Delete succeed.\n" );
		else
			printf( "Delete failed.\n" );
	}
	else
		printf( "Row not found!!\n");
}
//------------------------------------------------------------------------------------------------------------------------
void LevelTwoMenu()
{
	char press[ 256 ];

	do
	{
		printf("------- Menu -------\n");
		printf(" 1. Get Data By PK\n");
		printf(" 2. Add Data\n");
		printf(" 3. Delete Data\n");
		printf(" 4. Modify Data\n\n");
		printf(" m. Return to Table Menu.\n");
		printf(" x. exit.\n");
		printf("--------------------\n");
		scanf( "%s", press );
		switch( press[0] )
		{
			case '1': GetDataByPK( ); break;
			case '2': AddData( ); break;
			case '3': DeleteData( ); break;
			case '4': ModifyData( ); break;
			case 'M': 
			case 'm': return ; break;
			case 'X': 
			case 'x': exit (TRUE);
			default:  printf("Please select 1,2,3,4 ...\n");
					  break;
		}
	} while( TRUE );
}
//------------------------------------------------------------------------------------------------------------------------
void LevelOneMenu()
{
	char press[ 256 ];
	do
	{
		printf("------- MDS Table List -------\n");
		printf("   1. CUSTOM      Action      \n");
		printf("   2. POSITION    Action      \n");
		printf("   3. ALARM_LIMIT Action      \n\n");
		printf("   g. GetTrans                \n");
		printf("   f. FinishTransaction       \n");
		printf("\n   x. exit\n");
		printf("--------------------\n");
		scanf( "%s", press );

		if ( press[0] == 'x' || press[0] == 'X')
			exit( TRUE ) ;

		switch( press[0] )
		{
		case '1':
			gCurrentTable = gCustom;
			LevelTwoMenu();
			break;
		case '2':
			gCurrentTable = gPosition;
			LevelTwoMenu();
			break;
		case '3':
			gCurrentTable = gAlarmLimit;
			LevelTwoMenu();
			break;
		case 'G':
		case 'g':
			gTransID = gTransactionStore->GetTransID( );
			printf("#TransID : %d\n", gTransID);
			break;
		case 'F':
		case 'f':
			gTransactionStore->FinishTransaction( gTransID );
			printf("#Finish TransID : %d\n", gTransID);
			gTransID = 0;
			break;
		default :
			printf("Please select 1,2,3 ....\n");
			break;
		}
	} while( TRUE );
}
//------------------------------------------------------------------------------------------------------------------------
void Run( const UFC::AnsiString SysName )
{
	// new Transaction Object
	if ( gRecoverMode )
		gTransactionStore = new MTransactionStore( SysName, FALSE, "../data" );
	else
		gTransactionStore = new MTransactionStore( SysName, TRUE, "../data" );

	// new Table Object
	printf( " Load CUSTOM Table..." ); fflush( stdout );
	gCustom     = new MTable( "CUSTOM", "../schema", "../table", NULL, gTransactionStore );
	printf( " complete.\n" );
	/////////
	printf( " Load POSITION Table..." ); fflush( stdout );
	gPosition   = new MTable( "POSITION", "../schema", "../table", NULL, gTransactionStore );
	printf( " complete.\n" );
	/////////
	printf( " Load ALARM_LIMIT Table..." ); fflush( stdout );
	gAlarmLimit = new MTable( "ALARM_LIMIT", "../schema", "../table" ); ///< no TransactionStore object
	printf( " complete.\n" );
	/////////

	// Check if need to Recover Tables
	if ( gRecoverMode )
	{
		gTransactionStore->Recover();
	}

	// Run Main menu
	LevelOneMenu( );
}
//------------------------------------------------------------------------------------------------------------------------
// Main program
//------------------------------------------------------------------------------------------------------------------------
int main( int argc, char * argv[] )
{
	if ( argc < 2 )
	{
		printf( "Usage:%s <RecoverMode -> 1:On 0:Off\n", argv[0] );
		exit( 0 );
	}

	if ( argv[1][0] == '1' )
		gRecoverMode = TRUE;
	else
		gRecoverMode = FALSE;

	try
	{
		Run( argv[0] );
	}
	catch( UFC::Exception &e )
	{
		UFC::BufferedLog::Printf(" TestTransaction UFC Exception. <Reason:%s>\n", e.what() );
	}
	catch(...)
	{
		UFC::BufferedLog::Printf(" TestTransaction Unknown Exception.\n" );
	}

	return 0;
}
//------------------------------------------------------------------------------------------------------------------------
