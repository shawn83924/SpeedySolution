//------------------------------------------------------------------------------------------------------------------------
#include "TestMDS.h"
//------------------------------------------------------------------------------------------------------------------------
BOOL gIsTurnOnDestory;
MTable* gCurrentTable;
TestMTableListener  gTableListener;
UFC::UiniFile*		gMDSTableList = NULL;
UFC::AnsiString		gSelectedTable;
//------------------------------------------------------------------------------------------------------------------------
BOOL			AcceptKey( const char* KeyName, MDSBinaryObj& BinaryObj );
void			PrintToScreen( MRow& Row );
MDSBinaryObj	AcceptInputData( MRow& Row );
//------------------------------------------------------------------------------------------------------------------------
//
//  Win32 not define strkon_r, copy src from FreeBSD
//
//------------------------------------------------------------------------------------------------------------------------
#ifdef _WIN32
char* strtok_r(char *s1, const char *s2, char **lasts)
{
	char *ret;

	if( s1 == NULL)
		s1 = *lasts;
	while( *s1 && strchr(s2, *s1) )
		++s1;
	if( *s1 == '\0')
		return NULL;
	ret = s1;
	while( *s1 && !strchr(s2, *s1))
		++s1;
	if( *s1 )
		*s1++ = '\0';
	*lasts = s1;
	return ret;
}
#endif
//------------------------------------------------------------------------------------------------------------------------
// class TestMTableListener
//------------------------------------------------------------------------------------------------------------------------
void TestMTableListener::OnNewRow( MRow* pRow, void* ExtraParm )
{
	pRow->SetAttachObject( (void*)new TestAttachObject() );
}
//------------------------------------------------------------------------------------------------------------------------
void TestMTableListener::OnDeleteRow( MRow* pRow, void* ExtraParm )
{
	TestAttachObject* pAttach = (TestAttachObject*)pRow->GetAttachObject();
	if ( pAttach != NULL )
	{
		delete pAttach;
		pRow->SetAttachObject( NULL );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void BmGetRowByPK( void )
{
	int times;
	printf( "How many times to Get : " ); scanf( "%d", &times );

	MDSBinaryObj BinaryObj;
	if ( AcceptKey( "PK", BinaryObj ) == FALSE )
		return;

	int BinaryObjBufferSize = BinaryObj.GetSize();
	printf("BinaryObj.GetSize():%d\n", BinaryObjBufferSize);

	char BinaryObjBuffer[BinaryObjBufferSize + 1];
	memcpy(BinaryObjBuffer, BinaryObj.GetBuffer(), BinaryObjBufferSize); 
	BinaryObjBuffer[BinaryObjBufferSize] = '\0';
	printf("BinaryObj.FBuffer[]:%s\n", BinaryObjBuffer);

	MRow Row;

	int DoTimes, BeginMs, UseMs;

	//////////////////////////////////////////////////////////////////
	UFC::BufferedLog::Printf( " ##1 Begin to Get--------" );
	UFC::BufferedLog::Printf( " 1. new MDSBinaryObj" );
	UFC::BufferedLog::Printf( " 2. MDSBinaryObj.AppendBuffer" );
	UFC::BufferedLog::Printf( " 3. Search Data" );
	BeginMs = time_ms();
	for ( int i = 0; i < times; i++ )
	{
		MDSBinaryObj BinaryObj_1( gCurrentTable->GetIndexSizeByIndexName( "PK" ) );
		BinaryObj_1.AppendBuffer( BinaryObjBuffer, BinaryObjBufferSize );
		if ( gCurrentTable->GetRowByPK( Row, BinaryObj_1 ) == FALSE )
		{
			printf( "Row not found!!\n");
		}
	}
	UseMs = ( time_ms() - BeginMs );
	DoTimes = times / ( UseMs ? UseMs : 1 );
	printf( "    Do %d times per 1ms\n", DoTimes );
	UFC::BufferedLog::Printf( " ##1 End to Get----------\n" );
	//////////////////////////////////////////////////////////////////
	UFC::BufferedLog::Printf( " ##2 Begin to Get--------" );
	UFC::BufferedLog::Printf( " 1. MDSBinaryObj.AppendBuffer" );
	UFC::BufferedLog::Printf( " 2. Search Data" );
	MDSBinaryObj BinaryObj_2( gCurrentTable->GetIndexSizeByIndexName( "PK" ) );
	BeginMs = time_ms();
	for ( int i = 0; i < times; i++ )
	{
		BinaryObj_2.Seek( 0 );
		BinaryObj_2.AppendBuffer( BinaryObjBuffer, BinaryObjBufferSize );
		if ( gCurrentTable->GetRowByPK( Row, BinaryObj_2 ) == FALSE )
		{
			printf( "Row not found!!\n");
		}
	}
	UseMs = ( time_ms() - BeginMs );
	DoTimes = times / ( UseMs ? UseMs : 1 );
	printf( "    Do %d times per 1ms\n", DoTimes );
	UFC::BufferedLog::Printf( " ##2 End to Get----------\n" );
	//////////////////////////////////////////////////////////////////
	UFC::BufferedLog::Printf( " ##3 Begin to Get--------" );
	UFC::BufferedLog::Printf( " 1. Just Search Data" );
	BeginMs = time_ms();
	for ( int i = 0; i < times; i++ )
	{
		if ( gCurrentTable->GetRowByPK( Row, BinaryObj ) == FALSE )
		{
			printf( "Row not found!!\n");
		}
	}
	UseMs = ( time_ms() - BeginMs );
	DoTimes = times / ( UseMs ? UseMs : 1 );
	printf( "    Do %d times per 1ms\n", DoTimes );
	UFC::BufferedLog::Printf( " ##3 End to Get----------\n" );
}
//------------------------------------------------------------------------------------------------------------------------
void BmGetRowByIndexName( void )
{
	char InputBuffer[64];
	printf(" Input Index Name:"); scanf("%s", InputBuffer);
	UFC::AnsiString IndexNameStr( InputBuffer );

	int times;
	printf( "How many times to Get : " ); scanf( "%d", &times );

	MDSBinaryObj BinaryObj;
	if ( AcceptKey( InputBuffer, BinaryObj ) == FALSE )
		return;

	int BinaryObjBufferSize = BinaryObj.GetSize();
	printf("BinaryObj.GetSize():%d\n", BinaryObjBufferSize);

	char BinaryObjBuffer[BinaryObjBufferSize + 1];
	memcpy(BinaryObjBuffer, BinaryObj.GetBuffer(), BinaryObjBufferSize); 
	BinaryObjBuffer[BinaryObjBufferSize] = '\0';
	printf("BinaryObj.FBuffer[]:%s\n\n", BinaryObjBuffer);

	MRow Row;

	int DoTimes, BeginMs, UseMs;

	//////////////////////////////////////////////////////////////////
	UFC::BufferedLog::Printf( " ##1 Begin to Get--------" );
	UFC::BufferedLog::Printf( " 1. new MDSBinaryObj" );
	UFC::BufferedLog::Printf( " 2. MDSBinaryObj.AppendBuffer" );
	UFC::BufferedLog::Printf( " 3. Search Data" );
	BeginMs = time_ms();
	for ( int i = 0; i < times; i++ )
	{
		MDSBinaryObj BinaryObj_1( gCurrentTable->GetIndexSizeByIndexName( IndexNameStr ) );
		BinaryObj_1.AppendBuffer( BinaryObjBuffer, BinaryObjBufferSize );
		if ( gCurrentTable->GetRowByIndexName( Row, IndexNameStr, BinaryObj_1 ) == FALSE )
		{
			printf( "Row not found!!\n");
		}
	}
	UseMs = ( time_ms() - BeginMs );
	DoTimes = times / ( UseMs ? UseMs : 1 );
	printf( "    Do %d times per 1ms\n", DoTimes );
	UFC::BufferedLog::Printf( " ##1 End to Get----------\n" );
	//////////////////////////////////////////////////////////////////
	UFC::BufferedLog::Printf( " ##2 Begin to Get--------" );
	UFC::BufferedLog::Printf( " 1. MDSBinaryObj.AppendBuffer" );
	UFC::BufferedLog::Printf( " 2. Search Data" );
	MDSBinaryObj BinaryObj_2( gCurrentTable->GetIndexSizeByIndexName( IndexNameStr ) );
	BeginMs = time_ms();
	for ( int i = 0; i < times; i++ )
	{
		BinaryObj_2.Seek( 0 );
		BinaryObj_2.AppendBuffer( BinaryObjBuffer, BinaryObjBufferSize );
		if ( gCurrentTable->GetRowByIndexName( Row, IndexNameStr, BinaryObj_2 ) == FALSE )
		{
			printf( "Row not found!!\n");
		}
	}
	UseMs = ( time_ms() - BeginMs );
	DoTimes = times / ( UseMs ? UseMs : 1 );
	printf( "    Do %d times per 1ms\n", DoTimes );
	UFC::BufferedLog::Printf( " ##2 End to Get----------\n" );
	//////////////////////////////////////////////////////////////////
	UFC::BufferedLog::Printf( " ##3 Begin to Get--------" );
	UFC::BufferedLog::Printf( " 1. Just Search Data" );
	BeginMs = time_ms();
	for ( int i = 0; i < times; i++ )
	{
		if ( gCurrentTable->GetRowByIndexName( Row, IndexNameStr, BinaryObj ) == FALSE )
		{
			printf( "Row not found!!\n");
		}
	}
	UseMs = ( time_ms() - BeginMs );
	DoTimes = times / ( UseMs ? UseMs : 1 );
	printf( "    Do %d times per 1ms\n", DoTimes );
	UFC::BufferedLog::Printf( " ##3 End to Get----------\n" );
}
//------------------------------------------------------------------------------------------------------------------------
void GetRowByPK( void )
{
	MDSBinaryObj BinaryObj;
	if ( AcceptKey( "PK", BinaryObj ) == FALSE )
		return;

	MRow Row;
	if ( gCurrentTable->GetRowByPK( Row, BinaryObj ) == TRUE )
		PrintToScreen(Row);
	else
		printf( "Row not found!!\n" );
}
//------------------------------------------------------------------------------------------------------------------------
void GetRowByIndexName( void )
{
	char InputBuffer[64];
	printf(" Input Index Name:"); scanf("%s", InputBuffer);
	MDSBinaryObj BinaryObj;
	if ( AcceptKey( InputBuffer, BinaryObj ) == FALSE )
		return;

	MRow Row;
	if ( gCurrentTable->GetRowByIndexName( Row, InputBuffer, BinaryObj ) == TRUE )
		PrintToScreen(Row);
	else
		printf( "Row not found!!\n" );
}
//------------------------------------------------------------------------------------------------------------------------
void AddData( void )
{
	UFC::AnsiString ColumnName;
	enumColumnType ColumnType;
	int ColumnMaxLength;
	char InputBuffer[256];

	MRow Row = gCurrentTable->NewRow( );

	printf( "----Input Data----\n" );
	for (int i=0; i < gCurrentTable->GetColumnCount(); i++ )			
	{	
		ColumnName = gCurrentTable->GetColumnDefine(i)->FName;
		ColumnType = gCurrentTable->GetColumnDefine(i)->FType;
		ColumnMaxLength = (gCurrentTable->GetColumnDefine(i)->FMaxLength) -1;
	
		if ( ColumnType == ctString ) ///< ctString
		{
			printf( " Input '%s' [Type:string] [MaxLength:%d]\n", ColumnName.c_str(), ColumnMaxLength );
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

	if ( gCurrentTable->Update( Row ) == TRUE )
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

    MDSBinaryObj BinaryObj;
	if ( AcceptKey( "PK", BinaryObj ) == FALSE )
		return;

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
			printf( " Input '%s' [Type:char]\n",  ColumnName.c_str() );
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

	if ( gCurrentTable->Update( Row ) == TRUE )
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
	MDSBinaryObj BinaryObj;
	if ( AcceptKey( "PK", BinaryObj ) == FALSE )
		return;

	MRow Row;
	if ( gCurrentTable->GetRowByPK( Row, BinaryObj ) == TRUE )
	{
		PrintToScreen(Row);

		Row.Delete();
		if ( gCurrentTable->Update( Row ) == TRUE )
			printf( "Delete succeed.\n" );
		else
			printf( "Delete failed.\n" );
	}
	else
		printf( "Row not found!!\n");
}
//------------------------------------------------------------------------------------------------------------------------
BOOL AcceptKey( const char* KeyName, MDSBinaryObj& BinaryObj )
{
	UFC::AnsiString ColumnName;
	enumColumnType ColumnType;
	int ColumnMaxLength;
	int IndexSize = 0;

	char InputBuffer[256];
	UFC::List<int> *KeyList = gCurrentTable->GetIndexListByIndexName( KeyName );
	if ( KeyList == NULL )
	{
		printf( "IndexName:%s not found!!\n", KeyName );
		return FALSE;
	}
	IndexSize = gCurrentTable->GetIndexSizeByIndexName( KeyName );
	printf(" PKIndexSize=%d\n", IndexSize);

	BinaryObj.ReAllocBuffer( IndexSize );

	for (int i=0; i < KeyList->ItemCount(); i++) 
	{
		ColumnName = gCurrentTable->GetColumnDefine(KeyList->GetItem(i))->FName;
		ColumnType = gCurrentTable->GetColumnDefine(KeyList->GetItem(i))->FType;
		ColumnMaxLength = (gCurrentTable->GetColumnDefine(KeyList->GetItem(i))->FMaxLength) -1;

		if ( ColumnType == ctString ) ///< ctString
		{
			printf( " Input PKEY[%d]:%s [Type:string] [MaxLength:%d]\n", KeyList->GetItem(i), ColumnName.c_str(), ColumnMaxLength );
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
			printf( " Input PKEY[%d]:%s [Type:char]\n", KeyList->GetItem(i), ColumnName.c_str() );
			scanf( "%s", InputBuffer );
			BinaryObj.AppendChar( InputBuffer[0] );
		}
		else if ( ColumnType == ctInt ) ///< ctInt
		{
			printf( " Input PKEY[%d]:%s [Type:int]\n", KeyList->GetItem(i), ColumnName.c_str() );
			scanf( "%s", InputBuffer );

			int ColumnValue = atoi(InputBuffer);
			BinaryObj.AppendInt( ColumnValue );
		}
		else if ( ColumnType == ctInt64 ) ///< ctInt64
		{
			printf( " Input PKEY[%d]:%s [Type:Int64]\n", KeyList->GetItem(i), ColumnName.c_str() );
			scanf( "%s", InputBuffer );

			Int64 ColumnValue = strtoll( InputBuffer, (char**)NULL, 10 );
			BinaryObj.AppendInt64( ColumnValue );
		}
		else if ( ColumnType == ctDouble ) ///< ctDouble
		{
			printf( " Input PKEY[%d]:%s [Type:double]\n", KeyList->GetItem(i), ColumnName.c_str() );
			scanf( "%s", InputBuffer );

			double ColumnValue = atof(InputBuffer);
			BinaryObj.AppendDouble( ColumnValue );
		}
	}
	return TRUE;
}
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
		{
			printf( "[%s:%lf]", ColumnName.c_str(), Row.GetColumnAsDouble( ColumnName ) );
			printf( "[%s:%s]", ColumnName.c_str(), Row.GetColumnAsString( ColumnName ).c_str() );
		}
		printf("\n");
	}
}
//------------------------------------------------------------------------------------------------------------------------
void GetRowByID()
{
	MRow Row;
	char InputBuffer[256];
	UFC::AnsiString ColumnName;
	UFC::AnsiString ColumnType;

	printf(" Input RowID:"); scanf( "%s", InputBuffer );

	int RowID = atoi(InputBuffer);
	Row = gCurrentTable->GetRowByID(RowID);
	PrintToScreen(Row);
}
//------------------------------------------------------------------------------------------------------------------------
void CountAttachObject()
{
	MDSBinaryObj BinaryObj;
	if ( AcceptKey( "PK", BinaryObj ) == FALSE )
		return;

	MRow Row;
	if ( gCurrentTable->GetRowByPK( Row, BinaryObj ) == FALSE )
	{
		printf( "Row not found!!\n");
		return;
	}
	PrintToScreen(Row);
	printf("\n");
	TestAttachObject* pAttach = (TestAttachObject*)Row.GetAttachObject();
	if ( pAttach == NULL )
	{
		printf( "AttachObject not found!!\n");
		return;
	}

	UFC::PLockObject( pAttach->FCS );
	printf(" Before Count : %d\n", pAttach->FCount);
	pAttach->FCount++;
	printf(" After Count  : %d\n", pAttach->FCount);
}
//------------------------------------------------------------------------------------------------------------------------
void LoopPrintRowSet( MRowSet& RowSet )
{
	char InputBuffer[64];
	printf(" Input Print Column Name:"); scanf("%s", InputBuffer);
	UFC::AnsiString ColumnName( InputBuffer );
	MRow Row;
	int BeginMs = time_ms();

	BeginMs = time_ms();
	RowSet.MoveFirst();
	do
	{
		Row = RowSet.GetCurrRow( );
		printf( "[%s:%s]\n"
				, ColumnName.c_str()
				, Row.GetColumnAsString( ColumnName ).c_str() );
	} while ( RowSet.MoveNext() == TRUE );
	printf( " Use %dms\n", time_ms()-BeginMs );
}
//------------------------------------------------------------------------------------------------------------------------
void GetRowSetByIndexName()
{
	char InputBuffer[64];
	printf(" Input Index Name:"); scanf("%s", InputBuffer);
	MDSBinaryObj BinaryObj;
	if ( AcceptKey( InputBuffer, BinaryObj ) == FALSE )
		return;

	int BeginMs;

	BeginMs = time_ms();
	MRowSet RowSet;
	if ( gCurrentTable->GetRowSetByIndexName( RowSet, InputBuffer, BinaryObj ) == FALSE )
	{
		printf( "No Record found!!\n" );
		return;
	}
	printf( " Use %dms\n", time_ms()-BeginMs );

	MRow Row;
	// RowSet menu.
	char press[ 256 ];
	do
	{
		printf("------------------ Menu ----------------\n");
		printf(" 1. RowSet - MoveFirst()                \n");
		printf(" 2. RowSet - MoveLast()                 \n");
		printf(" 3. RowSet - MoveNext()                 \n");
		printf(" 4. RowSet - MoveMovePrevious()         \n");
		printf(" 5. RowSet - GetCurrRow()               \n");
		printf(" 6. RowSet - GetRowCount()              \n");
		printf(" 7. RowSet - LoopPrintRowSet()          \n");
		printf(" m. Return to Function Menu.            \n");
		printf(" x. exit.                               \n");
		printf("----------------------------------------\n");
		scanf( "%s", press );
		switch( press[0] )
		{
			case '1':
				if ( RowSet.MoveFirst( ) == FALSE )
				{
					printf( " RowSet.MoveFirst() return FALSE!!\n" );
					break;
				}
				Row = RowSet.GetCurrRow( );
				PrintToScreen( Row );
				break;
			case '2':
				if ( RowSet.MoveLast( ) == FALSE )
				{
					printf( " RowSet.MoveLast() return FALSE!!\n" );
					break;
				}
				Row = RowSet.GetCurrRow( );
				PrintToScreen( Row );
				break;
			case '3':
				if ( RowSet.MoveNext( ) == FALSE )
				{
					printf( " RowSet.MoveNext() return FALSE!!\n" );
					break;
				}
				Row = RowSet.GetCurrRow( );
				PrintToScreen( Row );
				break;
			case '4':
				if ( RowSet.MovePrevious( ) == FALSE )
				{
					printf( " RowSet.MovePrevious() return FALSE!!\n" );
					break;
				}
				Row = RowSet.GetCurrRow( );
				PrintToScreen( Row );
				break;
			case '5':
				Row = RowSet.GetCurrRow( );
				PrintToScreen( Row );
				break;
			case '6': printf( " RowCount = %d\n", RowSet.GetRowCount() ); break;
			case '7': LoopPrintRowSet( RowSet ); break;
			case 'M':
			case 'm': return ; break;
			case 'X':
			case 'x': exit(0);
			default:  printf("Please select 1,2,3 ...\n");
					  break;
		}
	} while( TRUE );
}
//------------------------------------------------------------------------------------------------------------------------
void BrowseGetCurrRow()
{
	MRow Row = gCurrentTable->GetCurrRow();
	PrintToScreen( Row );
}
//------------------------------------------------------------------------------------------------------------------------
void BrowseMoveFirst()
{
	if ( gCurrentTable->MoveFirst( ) == FALSE )
		printf( " MoveFirst() return FALSE!!\n" );
	BrowseGetCurrRow();
}
//------------------------------------------------------------------------------------------------------------------------
void BrowseMoveNext()
{
	if ( gCurrentTable->MoveNext( ) == FALSE )
		printf( " MoveNext() return FALSE!!\n" );
	BrowseGetCurrRow();
}
//------------------------------------------------------------------------------------------------------------------------
void Run()
{
	char press[ 256 ];

	do
	{
		printf("------------------------------------ Menu ----------------------------------\n");
		printf(" 1. Add Data                           a. Table Browse - MoveFirst()        \n");
		printf(" 2. Delete Data                        b. Table Browse - MoveNext()         \n");
		printf(" 3. Modify Data                        c. Table Browse - GetCurrRow()       \n");
		printf(" 4. GetRowByPK()                       d. Count AttachObject                \n");
		printf(" 5. GetRowByIndexName()                e. Benchmark --- GetRowByPK          \n");
		printf("    -just find MIndexObjectUnique      f. Benchmark --- GetRowByIndexName() \n");
		printf(" 6. GetRowByID()                       g. GetRowCount()                     \n");
		printf(" 7. GetRowSetByIndexName()                                                  \n");
		printf("    -just find MIndexObjectMultiple                                         \n");
		printf(" m. Return to Table Menu.                                                   \n");
		printf(" x. exit.                                                                   \n");
		printf("----------------------------------------------------------------------------\n");
		scanf( "%s", press );
		switch( press[0] )
		{
			case '1': AddData( ); break;
			case '2': DeleteData( ); break;
			case '3': ModifyData( ); break;
			case '4': GetRowByPK( ); break;
			case '5': GetRowByIndexName( ); break;
			case '6': GetRowByID( ); break;
			case '7': GetRowSetByIndexName( ); break;
			case 'A':
			case 'a': BrowseMoveFirst( ); break;
			case 'B':
			case 'b': BrowseMoveNext( ); break;
			case 'C':
			case 'c': BrowseGetCurrRow( ); break;
			case 'D':
			case 'd': CountAttachObject( ); break;
			case 'E':
			case 'e': BmGetRowByPK( ); break;
			case 'F':
			case 'f': BmGetRowByIndexName( ); break;
			case 'G':
			case 'g': printf( " RowCount = %d\n", gCurrentTable->GetRowCount() ); break;
			case 'M':
			case 'm': return ; break;
			case 'X':
			case 'x': exit(0);
			default:  printf("Please select 1,2,3 ...\n");
					  break;
		}
	} while( TRUE );
}
//------------------------------------------------------------------------------------------------------------------------
void ParseText( char* TextBuffer )
{
	char			Splits[] = "|\n";
	char*			StatusPoint;  ///< the status variable to record its strtok_r() progress
	char*			TokenStr;
	UFC::AnsiString	ColumnStr;

	// SetColumn by all columns of FColumnDefines
	for ( int i = 0; i < 999; i++ )
	{
		if ( i == 0 )
			TokenStr = strtok_r( TextBuffer, Splits, &StatusPoint );
		else
			TokenStr = strtok_r( NULL, Splits, &StatusPoint );
		if ( TokenStr == NULL )
			return;
		ColumnStr = TokenStr;
		ColumnStr.TrimLeft();
	}
	return;
}
//------------------------------------------------------------------------------------------------------------------------
void FGetsFile( const char* FilePath )
{
	char	TextBuffer[8192];
	FILE*	FileHandle;
	int		BeginMs;
	int		EndMs;

	BeginMs = time_ms();
	if ( ( FileHandle = fopen( FilePath, "r" ) ) == NULL )
	{
		printf( " File [%s] not found\n", FilePath );
		return;
	}
	int LineNo = 0;
	while ( fgets( TextBuffer, 8192, FileHandle ) != NULL )
	{
		LineNo++; ///< counting line no
		ParseText( TextBuffer );
	}
	EndMs = time_ms();
	printf( " spent %d ms, LoadedCount=%d\n", EndMs - BeginMs, LineNo );
	fclose( FileHandle );
}
//------------------------------------------------------------------------------------------------------------------------
void MenuTableList()
{
	UFC::AnsiString Name;
	UFC::AnsiString Value;
	int	Count;
	int SelectItem;
	char press[ 256 ];

	do
	{
		Name = "Table1";
		Count = 1;
		gSelectedTable = "";

		printf("------- MDS Table List -------\n");
		while( gMDSTableList->GetValue( "MDSTables", Name, Value ) )
		{
			printf(" %3d. %s\n", Count, Value.c_str() );
			Name.Printf("Table%d", ++Count );
		}

		printf("\n   x. exit\n");
		printf("--------------------\n");
		scanf( "%s", press );
		if ( press[0] == 'x' || press[0] == 'X') exit( TRUE ) ;

		SelectItem = atoi(press);
		if ( SelectItem == 0 ) continue ;
		Name.Printf("Table%d", SelectItem );

		BOOL IsJustFgets, IsAttachObject;
		printf("Do you want just fgets() file(Y/N) ?"); scanf( "%s", press );
		IsJustFgets = ( press[0] == 'y' || press[0] == 'Y' ) ? TRUE : FALSE;

		printf("Do you want Attach Object(Y/N) ?"); scanf( "%s", press );
		IsAttachObject = ( press[0] == 'y' || press[0] == 'Y' ) ? TRUE : FALSE;

		if ( gMDSTableList->GetValue("MDSTables", Name, Value) ) 
		{
			gSelectedTable = Value;

			try
			{
				int BeginMs;
				int EndMs;

				printf( "Loading %s File.......\n", gSelectedTable.c_str() ); fflush( NULL );
				if ( IsJustFgets == TRUE )
				{
					UFC::AnsiString FilePath;
					FilePath.Printf( "../table/%s.data", gSelectedTable.c_str() );
					FGetsFile( FilePath.c_str() );
					continue;
				}

				BeginMs = time_ms();
				// Load Table from File
				if ( IsAttachObject )
					gCurrentTable = new MTable( gSelectedTable, "../schema", "../table", &gTableListener );
				else
                    gCurrentTable = new MTable( gSelectedTable, "../schema", "../table" );
				EndMs = time_ms();
				printf( " spent %d ms, LoadedCount=%d\n", EndMs - BeginMs, gCurrentTable->GetRowCount() );

				Run();

				// Release Table memory space
				// !!!!!!!!!!!!!!!!!!! gSelectedTable, gIndexDefineCollection need delete to avoid memory leak !!!!!!!!!!!!!!!!!!!!!!!
				if ( gIsTurnOnDestory )
				{
					printf( "\nRelease %s Table memory space.......\n", gSelectedTable.c_str() ); fflush( NULL );
					BeginMs = time_ms();
					delete gCurrentTable;
					gCurrentTable = NULL;
					EndMs = time_ms();
					printf( " spent %d ms.\n", EndMs - BeginMs );
				}
			}
			catch( MDSException& ex )
			{
				printf( "%s\n", ex.what() );
				exit( 0 );
			}		
		}		
	} while( TRUE );
}
//------------------------------------------------------------------------------------------------------------------------
// Main program
//------------------------------------------------------------------------------------------------------------------------
int main( int argc, char * argv[] )
{
	if ( argc < 2 )
	{
		printf( "Usage:%s <IsTurnOnDestory>\n", argv[0] );
		printf( "  IsTurnOnDestory -> 0 : Off\n" );
		printf( "  IsTurnOnDestory -> 1 : On\n" );
		exit( 0 );
	}
	gIsTurnOnDestory = atoi( argv[1] );
	try
	{
		gMDSTableList = new UFC::UiniFile( "../schema/MDSTables.list" );
	}
	catch( UFC::Exception &e )
	{
		printf( "%s\n", "Load MDSTables.list Failed!!" );
		exit (-1);
	}

	MenuTableList();
}
//------------------------------------------------------------------------------------------------------------------------
