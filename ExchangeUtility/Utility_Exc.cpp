/**@file Utility.cpp
*  @brief Some useful utility function/class for FCM Simulator.
*  @author Simon Chang
*
*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include  "Utility.h"
#include  "TMPDefines.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MillIntToHHMMSSMMString( char *str, int t )
{
    int h,m,Result;

    h = t / (60*60*1000);
    t -= h*60*60*1000;
    m = t / (60*1000);
    t -= m*60*1000;
    t /= 10;
    Result = h*1000000 + m*10000 + t;
    sprintf( str, "%08d", Result );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*int MillInt( void )
{
	int t;
#ifdef _WIN32
	SYSTEMTIME SystemTime;

	GetLocalTime( &SystemTime );
	t = SystemTime.wHour*60*60*1000+SystemTime.wMinute*60*1000+SystemTime.wSecond*1000+SystemTime.wMilliseconds;
#else
	struct tm tmbuf;
        struct timespec now;
    
        clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    	
        struct tm *pTM = localtime_r( &now.tv_sec, &tmbuf );
	t = pTM->tm_hour*60*60*1000+pTM->tm_min*60*1000+pTM->tm_sec*1000+ (int)(now.tv_nsec/1000000);
#endif
	return t;
}*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*long GetTimeMS( void )
{
#ifdef _WIN32
	return GetTickCount();
#else
	struct timeb  t;
	long RtnMS;

	ftime( &t );
	RtnMS = t.time*1000 + t.millitm;
	return RtnMS;
#endif
}*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void strncpynt( char *s1, const char *s2, int n )
{
    strncpy( s1, s2, n );
    *(s1+n) = 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetCurrentTimeString( char *TimeStr ) // get HHMMSS string format time.
{
    char  TimeStrPtr[7];

    ThreadClock::GetTimeString( TimeStrPtr, FALSE );
    memcpy( TimeStr, TimeStrPtr, 6 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetCurrentTimeStringNT( char *TimeStr ) // get HHMMSS string format time.
{
    ThreadClock::GetTimeString( TimeStr, TRUE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString GetDateString( void )
{
	UFC::AnsiString RtnStr;
#ifdef _WIN32
	SYSTEMTIME SystemTime;

	GetLocalTime( &SystemTime );
	RtnStr.Printf( "%02d%02d%02d", SystemTime.wYear -1911, SystemTime.wMonth, SystemTime.wDay );
#else
	struct tm  TM;
	time_t tTime;
	int y,m,d;

	time( &tTime );
	localtime_r( &tTime, &TM );
	y = TM.tm_year - 11;
	m = TM.tm_mon + 1;
	d = TM.tm_mday;
	RtnStr.Printf("%02d%02d%02d",y,m,d);
#endif
	return RtnStr;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString GetDateStringYYYYMMDD( void )
{
	UFC::AnsiString RtnStr;
#ifdef _WIN32
	SYSTEMTIME SystemTime;

	GetLocalTime( &SystemTime );
	RtnStr.Printf( "%04d%02d%02d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay );
#else
	struct tm  TM;
	time_t tTime;
	int y,m,d;

	time( &tTime );
	localtime_r( &tTime, &TM );
	y = TM.tm_year;
	m = TM.tm_mon + 1;
	d = TM.tm_mday;
	RtnStr.Printf("%04d%02d%02d",y,m,d);
#endif
	return RtnStr;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Class LogFile
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LogFile::LogFile(  const char* FileName, const char*Mode )
{
    char Msg [64];

    FFileHandle = fopen( FileName, Mode );
    if( FFileHandle == NULL )
    {
        sprintf(Msg,"Can not open file:%s",FileName );
        throw( UFC::FileException( Msg ));
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LogFile::~LogFile()
{
    if( FFileHandle )
    {
        fflush( FFileHandle );
        fclose( FFileHandle );
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LogFile::Begin( void )
{
    fseek( FFileHandle, 0L, SEEK_SET );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int LogFile::WriteLine(char* Buffer )
{
    int WriteSize = fputs( Buffer, FFileHandle );
    fflush( FFileHandle );
    return WriteSize;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char* LogFile::ReadLine(char* Buffer,int MaxLength )
{
    return fgets( Buffer, MaxLength, FFileHandle );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Class LogManager
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LogManager::LogManager( UFC::AnsiString FileName, int BufferLength )
:FFileStream( NULL ),
 FBuffer( NULL ),
 FPos( 0 ),
 FBufferLength( BufferLength )
{
    FFileStream = new UFC::FileStreamEx( FileName, "a" );
    FBuffer = new char[ BufferLength ];
    memset( FBuffer, 0, FBufferLength );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LogManager::~LogManager()
{
    Flush();
    delete [] FBuffer;
    delete FFileStream;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LogManager::WriteString( UFC::AnsiString& LogString )
{
    WriteString( LogString.c_str());
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LogManager::WriteString( const char* LogString )
{
    UFC::PLockObject Lock( FCS );

    int Len = (int)strlen( LogString ) + 1;
    if( FPos + Len >= FBufferLength )
    {
    	FFileStream->Write( FBuffer, FPos );
        FFileStream->Flush();
    	FPos = 0;
    }
    memcpy( FBuffer + FPos, LogString, Len - 1 );
    FPos += Len;
    FBuffer[ FPos - 1 ] = '\n';
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LogManager::Write( const UInt8* Data, Int32 DataLen )
{
    UFC::PLockObject Lock( FCS );

    int Len = DataLen + 1;
    if( FPos + Len >= FBufferLength )
    {
        FFileStream->Write( FBuffer, FPos );
        FFileStream->Flush();
        FPos = 0;
    }
    if( Len > FBufferLength )
    {
        FBufferLength = 2 * Len;
        delete [] FBuffer;
        FBuffer = new char[ FBufferLength ];
    }
    memcpy( FBuffer + FPos, Data, Len - 1 );
    FPos += Len;
    FBuffer[ FPos - 1 ] = '\n';
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LogManager::Flush( void )
{
    if( FPos > 0 )
    {
        UFC::PLockObject Lock( FCS );
        FFileStream->Write( FBuffer, FPos );
        FFileStream->Flush();
        FPos = 0;
    }
}
//----------------------------------------------------------------------------------------------------------
//  class ThreadClock
//----------------------------------------------------------------------------------------------------------
#ifdef _WIN32
SYSTEMTIME   ThreadClock::FSysTime;
BOOL         ThreadClock::FIsInit = FALSE;
unsigned int ThreadClock::FRefreshTime;
UFC::PCriticalSection ThreadClock::FTimerCS;
//----------------------------------------------------------------------------------------------------------
void  ThreadClock::InitThreadClock( unsigned int TimeMS )
{
	DWORD ThreadID;

	FRefreshTime = TimeMS;
	if( CreateThread( NULL, 0,( LPTHREAD_START_ROUTINE)ThreadClock::Execute,NULL,0,&ThreadID)==NULL)
		throw( UFC::ThreadException( GetLastError()));
	UpdateTime();
	FIsInit = TRUE;
}
//----------------------------------------------------------------------------------------------------------
int ThreadClock::SecondOfToday( void )
{
	return FSysTime.wHour*3600 + FSysTime.wMinute*60 + FSysTime.wSecond;
}
//----------------------------------------------------------------------------------------------------------
void ThreadClock::GetTimeString( char* TimeStr, BOOL WithSeparate, BOOL WithMS )
{
	if( FIsInit == FALSE )
		UpdateTime();

	UFC::PLockObject Lock( FTimerCS );

	if( WithMS == TRUE )
	{
		if( WithSeparate == TRUE )
			sprintf( TimeStr,"%02d:%02d:%02d.%03d",FSysTime.wHour,FSysTime.wMinute,FSysTime.wSecond,FSysTime.wMilliseconds );
		else
			sprintf( TimeStr,"%02d%02d%02d%03d",FSysTime.wHour,FSysTime.wMinute,FSysTime.wSecond,FSysTime.wMilliseconds);
	}
	else
	{
		if( WithSeparate == TRUE )
			sprintf( TimeStr,"%02d:%02d:%02d:",FSysTime.wHour,FSysTime.wMinute,FSysTime.wSecond );
		else
			sprintf( TimeStr,"%02d%02d%02d",FSysTime.wHour,FSysTime.wMinute,FSysTime.wSecond );
	}
}
//----------------------------------------------------------------------------------------------------------
void ThreadClock::GetTimeStringMS( char* TimeStrMS )
{
	char   TimeStr[ 20 ];
	
	if( FIsInit == FALSE )
		UpdateTime();

	UFC::PLockObject Lock( FTimerCS );

	sprintf( TimeStr,"%02d:%02d:%02d:%03d",FSysTime.wHour,FSysTime.wMinute,FSysTime.wSecond,FSysTime.wMilliseconds);
	memcpy( TimeStrMS, TimeStr, 12 );
}
//----------------------------------------------------------------------------------------------------------
BOOL  ThreadClock::IsInit( void )
{
	return FIsInit;
}
//----------------------------------------------------------------------------------------------------------
void  ThreadClock::UpdateTime( void )
{
	UFC::PLockObject Lock( FTimerCS );

	GetLocalTime( &FSysTime );
}
//----------------------------------------------------------------------------------------------------------
void* ThreadClock::Execute( void* )
{
	while( TRUE )
	{
		UpdateTime();
		UFC::SleepMS( ThreadClock::FRefreshTime );
	}
}
//----------------------------------------------------------------------------------------------------------
#else
//----------------------------------------------------------------------------------------------------------
BOOL           ThreadClock::FIsInit = FALSE;
struct tm      ThreadClock::FTime;
struct timeval ThreadClock::FTimeVal;
unsigned int   ThreadClock::FRefreshTime;
UFC::PCriticalSection ThreadClock::FTimerCS;
//----------------------------------------------------------------------------------------------------------
void  ThreadClock::InitThreadClock( unsigned int TimeMS )
{
    pthread_t ThreadID;

    FRefreshTime = TimeMS;
    pthread_create( &ThreadID, NULL, ThreadClock::Execute, NULL );
    UpdateTime();
    FIsInit = TRUE;
}
//----------------------------------------------------------------------------------------------------------
int ThreadClock::SecondOfToday( void )
{
    if( FIsInit == FALSE )
	UpdateTime();    
    return FTime.tm_hour*3600 + FTime.tm_min*60 + FTime.tm_sec;
}
//----------------------------------------------------------------------------------------------------------
void ThreadClock::GetTimeString( char* TimeStr , BOOL WithSeparate, BOOL WithMS )
{
    if( FIsInit == FALSE )
        UpdateTime();

    UFC::PLockObject Lock( FTimerCS );

    if( WithMS == TRUE )
    {
        if( WithSeparate == TRUE )
            sprintf( TimeStr,"%02d:%02d:%02d.%03d",FTime.tm_hour,FTime.tm_min,FTime.tm_sec,(int) FTimeVal.tv_usec /1000 );
        else
            sprintf( TimeStr,"%02d%02d%02d%03d",FTime.tm_hour,FTime.tm_min,FTime.tm_sec,(int) FTimeVal.tv_usec /1000 );
    }
    else
    {
        if( WithSeparate == TRUE )
            sprintf( TimeStr,"%02d:%02d:%02d:",FTime.tm_hour,FTime.tm_min,FTime.tm_sec );
        else
            sprintf( TimeStr,"%02d%02d%02d",FTime.tm_hour,FTime.tm_min,FTime.tm_sec );
    }
}
//----------------------------------------------------------------------------------------------------------
void ThreadClock::GetTimeStringMS( char* TimeStrMS )
{
    char   TimeStr[ 20 ];

    if( FIsInit == FALSE )
        UpdateTime();

    UFC::PLockObject Lock( FTimerCS );

    sprintf( TimeStr,"%02d:%02d:%02d.%03d",FTime.tm_hour,FTime.tm_min,FTime.tm_sec, (int)FTimeVal.tv_usec /1000 );
    memcpy( TimeStrMS, TimeStr, 12 );
}
//----------------------------------------------------------------------------------------------------------
void ThreadClock::GetTimeStringMS_US( char* TimeStr, BOOL UseUS )
{    
    if( FIsInit == FALSE )
        UpdateTime();

    UFC::PLockObject Lock( FTimerCS );

    if( UseUS == TRUE )
        sprintf( TimeStr,"%02d:%02d:%02d.%06d",FTime.tm_hour,FTime.tm_min,FTime.tm_sec, (int)FTimeVal.tv_usec );
    else
        sprintf( TimeStr,"%02d:%02d:%02d.%03d",FTime.tm_hour,FTime.tm_min,FTime.tm_sec, (int)FTimeVal.tv_usec /1000 );
    
}
//----------------------------------------------------------------------------------------------------------
inline BOOL ThreadClock::IsInit( void )
{
    return FIsInit;
}
//----------------------------------------------------------------------------------------------------------
void  ThreadClock::UpdateTime( void )
{
    UFC::PLockObject Lock( FTimerCS );
    
	
    gettimeofday( &FTimeVal, NULL ); ///< Get the current time.
    int Today_sec = FTimeVal.tv_sec % 86400;    
    FTime.tm_hour = ((Today_sec / 3600 ) + 8 ) % 24;
    FTime.tm_min  = ( Today_sec % 3600 ) / 60;
    FTime.tm_sec  = Today_sec % 60;  
}
//----------------------------------------------------------------------------------------------------------
void* ThreadClock::Execute( void* )
{
    while( TRUE )
    {
        UpdateTime();
        UFC::SleepMS( ThreadClock::FRefreshTime );
    }
    return NULL;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
StringTokenizer::StringTokenizer( char* TokenString )
:FTokens( new UFC::PList() )
{
    char* StrPtr = TokenString;
    char* FindStr;
    int   Length = (int)strlen( TokenString );

    while( TRUE )
    {
        FindStr = strstr( StrPtr, "," );
        if( FindStr != NULL )
        {
            if( FindStr == StrPtr )
                FTokens->Add( new UFC::AnsiString( "Empty" ) );
            else
                FTokens->Add( new UFC::AnsiString( StrPtr, FindStr - StrPtr ) );
            StrPtr = FindStr + 1;
        }
        else
        {
            FTokens->Add( new UFC::AnsiString( StrPtr, Length - (StrPtr-TokenString) ) );
            return;
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
StringTokenizer::~StringTokenizer()
{
    for( register int i = 0; i < FTokens->ItemCount(); i++ )
         delete (UFC::AnsiString*) FTokens->GetItem( i );
    delete FTokens;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ConfigReader::ConfigReader( char* FileName )
:FNames( NULL ),
 FValues( NULL )
{
    LogFile*    FFile;
    char        Buffer[ 512 ];
    int         CutPos;

    FFile   = new LogFile( FileName, "rw" );
    FNames  = new UFC::PList();
    FValues = new UFC::PList();
    while( FFile->ReadLine(	Buffer, 512 ) != NULL )
    {
        UFC::AnsiString Str( Buffer );
        if( Str.AnsiPos( "##" ) == -1 )
        {
            if( (CutPos = Str.AnsiPos( "=" )) != -1 )
            {
                UFC::AnsiString* Name  = new UFC::AnsiString( Str.SubString( 0, CutPos ));
                UFC::AnsiString* Value = new UFC::AnsiString( Str.SubString( CutPos + 1, Str.Length()-1 ) );

                Name->TrimLeft();
                Name->TrimRight();
                Value->TrimLeft();
                Value->TrimRight();
                FNames->Add( Name );
                FValues->Add( Value );
            }
        }
    }
    delete FFile;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ConfigReader::~ConfigReader()
{
    if( FNames != NULL && FValues != NULL )
    {
        for( register int i = 0; i < FNames->ItemCount(); i++ )
        {
             delete (UFC::AnsiString*) FNames->GetItem( i );
             delete (UFC::AnsiString*) FValues->GetItem( i );
        }
        delete FNames;
        delete FValues;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString ConfigReader::GetName( int Index )
{
    if( Index >= 0 && Index < FNames->ItemCount() )
        return *((UFC::AnsiString*)FNames->GetItem( Index ));
    throw( UFC::IndexOutOfBoundException( Index ));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString ConfigReader::GetValue( int Index )
{
    if( Index >= 0 && Index < FValues->ItemCount() )
        return *((UFC::AnsiString*)FValues->GetItem( Index ));
    throw( UFC::IndexOutOfBoundException( Index ));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString ConfigReader::GetValueByName( UFC::AnsiString Name )
{
    for( register int i = 0; i < FNames->ItemCount(); i++ )
    {
        if( Name == *((UFC::AnsiString*)FNames->GetItem( i )) )
            return *((UFC::AnsiString*)FValues->GetItem( i ));
    }
    UFC::AnsiString Msg = "Config file:" + Name + " not found.";
    throw( ConfigFileException( (char*)Msg.c_str() ) );
}
//------------------------------------------------------------------------------
//  Format for recover executions
//  for Futures,Option TMP execution
//------------------------------------------------------------------------------
// RptSeq         9( 10 )  0~9
// ExecType       X( 1  )  10~11
// StatusCode     9( 3  )  11~14 
// LastQty        9( 4  )  14~18
// CumQty         9( 4  )  18~22
// LeavesQty      9( 4  )  22~26
// BeforeQty      9( 4  )  26~30
// PxSubTotal     9( 12 )  30~42
// OrgTransTime   9( 9  )  42~51
// TransTime      9( 9  )  51~60
// UniqID         9( 10 )  60~70
// LineBrokerID   9( 5  )  70~75
// Price          9( 9  )  75~84
// OrdType        9( 1  )  84~85
// TimeInForce    9( 1  )  85~86
// PositionEffect X( 1  )  86~87
// Qty            9( 4  )  87~91
// StatusCode     9( 5  )  91~96 ---> New FLEX Version
// Part_ID        9( 3  )  96~99 ---> New FLEX Version
// Total length: 99
//
//------------------------------------------------------------------------------
BOOL TMPExtFields::IsValidTMPExtStringLength( Int32 Len  )
{
    if( Len >= 91 )
        return TRUE;
    return FALSE;
}
//------------------------------------------------------------------------------
void TMPExtFields::ToString( UFC::AnsiString& Result ) const
{
    Result.Printf("%010d%c%03d%04d%04d%04d%04d%012lld%09d%09d%010d%05d%09d%c%c%c%04d%05d%03d",
                  RptSeq,
                  ExecType,
                  StatusCode%1000,
                  LastQty,
                  CumQty,
                  LeavesQty,
                  BeforeQty,
                  PxSubTotal,
                  OrgTransTime,
                  TransTime,
                  UniqID,
                  LineBrokerID,
                  Price,
                  OrdType + '0',
                  TimeInForce + '0',
                  PositionEffect,
                  Qty,
                  StatusCode,
                  PartID);
}
//------------------------------------------------------------------------------
void TMPExtFields::FromString( const UFC::AnsiString& Str )
{
    if( Str.Length() >= 10 ) ///< First Speedy TCP version
        RptSeq       = Str.SubString(  0, 10 ).ToInt();
    if( Str.Length() >= 75 ) ///< First SpeedyTMP version
    {
        ExecType     = Str[ 10 ];
        StatusCode   = Str.SubString( 11, 3  ).ToInt();
        LastQty      = Str.SubString( 14, 4  ).ToInt();
        CumQty       = Str.SubString( 18, 4  ).ToInt();
        LeavesQty    = Str.SubString( 22, 4  ).ToInt();
        BeforeQty    = Str.SubString( 26, 4  ).ToInt();
        PxSubTotal   = Str.SubString( 30, 12 ).ToInt64();
        OrgTransTime = Str.SubString( 42, 9  ).ToInt();
        TransTime    = Str.SubString( 51, 9  ).ToInt();
        UniqID       = Str.SubString( 60, 10 ).ToInt();
        LineBrokerID = Str.SubString( 70, 5  ).ToInt();
    }
    if( Str.Length() >= 91 ) ///< Last SpeedyTMP version
    {
        Price          = Str.SubString( 75, 9  ).ToInt();
        OrdType        = Str[84] - '0';
        TimeInForce    = Str[85] - '0';
        PositionEffect = Str[86];   
        Qty            = Str.SubString( 87, 4  ).ToInt();
    }
    if( Str.Length() >= 99 ) ///< FLEX Version
    {
        StatusCode     = Str.SubString( 91, 5  ).ToInt();
        PartID         = Str.SubString( 96, 3  ).ToInt();
    }
}
//------------------------------------------------------------------------------
//  Format for recover executions
//  for Futures,Option,TSE and OTC
//------------------------------------------------------------------------------
//                                           __ Key (AE for SpeedyGateway, Veriable length)
//                                           |
//  Index:     0   |1|    2     |     3    | 4 |        5       |   6   |   7    |   8   |    9    |
//  Field:   hhmmss|C|0123456789|0123456789|Key|Exchange message|F006000|Hostname| PVCID | TMP Ext |--- TMP Execution Ext Info(Length:66, TMP only)
//             |    |    |            |                  |           |               |_ PVCID(Length:2) or SessionID(Length:3)
//             |    |    |            |                  |           |_ Broker ID (Veriable length)  
//             |    |    |            |                  |_ Message from exchange. (Veriable length)
//             |    |    |            |_ Execution Sequence
//             |    |    |_ Network ID. (Length:10)
//             |    |
//             |    |_ C/F: Confirm or fill execution. (Length:1)
//             |    
//             |_____ Time to write this log. (Length:6)
//
// Samples:
//
//  195217|C|0016860387|0000000001|yuan|30010119521700F006000E100780232142FITX   20061200000000B20060000000100F0060000001000000008|S132000|MD1500|001
//  195217|F|0016860387|0000000002|yuan|1F006000E100780232142FITX   20061200000000B20060000000050F0061952172800000006F|S132000|MD1500|001
//  195220|C|0016860397|0000000003|yuan|30010119522000F006000E100880232142FITX   20061200000000B20060000000100F0060000001000000009|S132000|MD1500|028
//
// String Field 0: Exec Time
// String Field 1: Type, 'C'onfirm or 'F'ill. 'c' or 'f' for off-hour.
// String Field 2: Network ID
// String Field 3: Message Sequence
// String Field 4: Key (AE+UserDefine)
// String Field 5: Exchange message
// String Field 6: Broker ID.( CFFEX SysOrderID)
// String Field 7: Which Host process this execution.
// String Field 8: Which PVC process this execution.  <-------------- TSEC/CFFEX formats.( 9 fields )
// String Field 9: Symbol.
// String Field 10: Price.
// String Field 11: Qty.                                               <-------------- TMP Binary format.( 12 fields )
// String Field 12: TAIFEX TMP execution report seq( field: "rpt_seq").<-------------- TMP X.25 format.( 13 fields )
//------------------------------------------------------------------------------
UFC::AnsiString ExecutionParser::EMPTY_TMP_EXT("0000000000");
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ExecutionParser::ExecutionParser( const UFC::AnsiString& Log )
:FData( NULL )
,FSize( 0 )
{
    FStrings.SetStrings( Log, "|\n" );

    if (FStrings.ItemCount() < 9 )
        throw UFC::Exception("Parse Execution log error. fields less then 9."); 
    if (FStrings[ 0 ].Length() != 6)
        throw UFC::Exception("Not a valid Time field.(HHMMSS)");
    if (FStrings[ 1 ].Length() != 1)
        throw UFC::Exception("Not a valid Type field.(C/F)");
    if (FStrings[ 3 ].Length() != 10)
        throw UFC::Exception("Not a valid Seq field.(9(10))");
    if (FStrings[ 4 ].Length() == 0)
        throw UFC::Exception("Not a valid Key field.(Length=0)");
    if (FStrings[ 5 ].Length() < 14)
        throw UFC::Exception("Not a valid Message field.(Length<14)");
    if (FStrings[ 6 ].Length() < 4)
        throw UFC::Exception("Not a valid BrokerID field.(Length<4)");
    if (FStrings[ 7 ].Length() == 0)
        throw UFC::Exception("Not a valid Host name field.(Length=0)");
    if (FStrings[ 8 ].Length() < 2 )
        throw UFC::Exception("Not a valid PVC field.(Length<2)");    
    if( IsBinary() == TRUE )              ///< TAIFEX Binary format. contains 12 fields
    {
        if( FStrings.ItemCount() != 12 )
            throw UFC::Exception( "Not a valid TMP binary format log."); 
        FIsTMP = TRUE;
        FSize  = FStrings[ 5 ].Length()/2;
        FData  = new UInt8[ FSize ];
        UFC::HexStringToBinary( FStrings[ 5 ], FData, FSize );
    }
    else
    {
        if( FStrings.ItemCount() == 13 ) ///<  TAIFEX X.25 text format. contains 13 fields
        {
            if( TMPExtFields::IsValidTMPExtStringLength( FStrings[ 12 ].Length() ) == FALSE )
                throw UFC::Exception("Not a valid TMP Ext fields string.");
            FIsTMP = TRUE;
            FTMPFields.FromString( FStrings[ 12 ] );            
        }
        else                            ///< TSEC format. contains 9 fields
        {
            if( FStrings.ItemCount() != 9 )
                throw UFC::Exception( "Not a valid TMP binary format log."); 
            FIsTMP = FALSE; 
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ExecutionParser::~ExecutionParser( void )
{
    if( FData != NULL && FSize > 0 )
        delete [] FData;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int32 ExecutionParser::GetTime( void )
{
    return FStrings[ 0 ].ToInt();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL  ExecutionParser::IsConfirm( void )
{
    char Type = FStrings[ 1 ][ 0 ];
    
    if( Type == 'C' || Type == 'c' || Type == 'E' || Type == 'e')        
        return TRUE;
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL  ExecutionParser::IsFill( void )
{
    char Type = FStrings[ 1 ][ 0 ];
    char ExecType = FStrings[ 5 ][0];
    
    if( Type == 'F' || Type == 'f' || Type == 'M' || Type == 'm')        
    {
        if( FIsTMP == TRUE ) ///< TAIFEX Message
        { 
            if( ExecType == '1' || ExecType == '2' )
                return TRUE;
        }
        else  ///< TWSE/OTC Message
            return TRUE;            
    }
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL ExecutionParser::IsOffHour( void )
{
    char Type = FStrings[ 1 ][ 0 ];
    
    if( Type == 'c' || Type == 'f' || Type == 'e' || Type == 'm')
        return TRUE;
    return FALSE;    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL ExecutionParser::IsBinary( void )
{
    char Type = FStrings[ 1 ][ 0 ];
    
    if( Type == 'e' || Type == 'm'|| Type == 'E' || Type == 'M' )
        return TRUE;
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int32 ExecutionParser::GetNID( void )
{
    return FStrings[ 2 ].ToInt();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int32 ExecutionParser::GetSequence( void )
{
    return FStrings[ 3 ].ToInt();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const UFC::AnsiString& ExecutionParser::GetKey( void )
{
    return FStrings[ 4 ];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const UFC::AnsiString& ExecutionParser::GetData( void )
{
    return FStrings[ 5 ];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL ExecutionParser::GetData( UInt8*& Data, Int32& Size )
{
    if( FData != NULL && FSize > 0 )
    {
        Data = FData;
        Size = FSize;
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const UFC::AnsiString& ExecutionParser::GetPBNO( void )
{
    return FStrings[ 6 ];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const UFC::AnsiString& ExecutionParser::GetHostname( void )
{
    return FStrings[ 7 ];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const UFC::AnsiString& ExecutionParser::GetPVCID( void )
{
    return FStrings[ 8 ];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const UFC::AnsiString& ExecutionParser::GetSymbol( void )
{
    return FStrings[ 9 ];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int32 ExecutionParser::GetQty( void )
{
    return FStrings[ 10 ].ToInt();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int32 ExecutionParser::GetPrice( void )
{
    return FStrings[ 11 ].ToInt();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const UFC::AnsiString& ExecutionParser::GetTMPExtFields( void )
{
    if( FIsTMP == TRUE && FStrings.ItemCount() >= 13 )
        return FStrings[ 12 ];
    return EMPTY_TMP_EXT;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Formats for TSEC
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExecutionParser::Render(   UFC::AnsiString&       LogString,/// [Out]
                                BOOL                   IsConfirm,
                                Int32                  NID,
                                Int32                  Seq,
                                const UFC::AnsiString& Key,
                                const UFC::AnsiString& Data,
                                const UFC::AnsiString& PBNO,
                                const UFC::AnsiString& Host,
                                const UFC::AnsiString& PVCID )
{
    UFC::AnsiString TimeNow;
    char            Type = (IsConfirm == TRUE) ? 'C':'F';

    GetHHMMSS( TimeNow );    
    if( Key.Length() == 0 )
        LogString.Printf( "%s|%c|%010u|%010u|%s|%s|%s|%s|%s\n",TimeNow.c_str(),Type,NID,Seq, "null", Data.c_str(), PBNO.c_str(),Host.c_str(),PVCID.c_str());
    else
        LogString.Printf( "%s|%c|%010u|%010u|%s|%s|%s|%s|%s\n",TimeNow.c_str(),Type,NID,Seq, Key.c_str(), Data.c_str(), PBNO.c_str(),Host.c_str(),PVCID.c_str());
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Formats for TAIFEX ( X.25 format )
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExecutionParser::Render(   UFC::AnsiString&       LogString,/// [Out]
                                BOOL                   IsConfirm,
                                BOOL                   IsOffHour,
                                Int32                  NID,
                                Int32                  Seq,
                                const UFC::AnsiString& Key,
                                const UFC::AnsiString& Data,
                                const UFC::AnsiString& PBNO,
                                const UFC::AnsiString& Host,
                                const UFC::AnsiString& PVCID,
                                const UFC::AnsiString& Symbol, 
                                Int32                  Qty,    
                                Int32                  Price,  
                                const UFC::AnsiString& TMPExtStr )
{
    UFC::AnsiString TimeNow;
    char            Type;

    if( IsOffHour == TRUE )
        Type = (IsConfirm == TRUE) ? 'c':'f';
    else
        Type = (IsConfirm == TRUE) ? 'C':'F';
    UFC::GetHHMMSS( TimeNow );
    if( TMPExtStr.Length() == 0 )
    {
        if( Key.Length() == 0 )
            LogString.Printf( "%s|%c|%010u|%010u|%s|%s|%s|%s|%s|%s|%d|%d\n",TimeNow.c_str(),Type,NID,Seq, "null", Data.c_str(), PBNO.c_str(),Host.c_str(),PVCID.c_str(),Symbol.c_str(),Qty,Price);
        else
            LogString.Printf( "%s|%c|%010u|%010u|%s|%s|%s|%s|%s|%s|%d|%d\n",TimeNow.c_str(),Type,NID,Seq, Key.c_str(), Data.c_str(), PBNO.c_str(),Host.c_str(),PVCID.c_str(),Symbol.c_str(),Qty,Price);
    }
    else
    {
        if( Key.Length() == 0 )
            LogString.Printf( "%s|%c|%010u|%010u|%s|%s|%s|%s|%s|%s|%d|%d|%s\n",TimeNow.c_str(),Type,NID,Seq, "null", Data.c_str(), PBNO.c_str(),Host.c_str(),PVCID.c_str(),Symbol.c_str(),Qty,Price,TMPExtStr.c_str());
        else
            LogString.Printf( "%s|%c|%010u|%010u|%s|%s|%s|%s|%s|%s|%d|%d|%s\n",TimeNow.c_str(),Type,NID,Seq, Key.c_str(), Data.c_str(), PBNO.c_str(),Host.c_str(),PVCID.c_str(),Symbol.c_str(),Qty,Price,TMPExtStr.c_str());
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExecutionParser::Render(   UFC::AnsiString&       LogString,/// [Out]
                                BOOL                   IsConfirm,///[In]
                                BOOL                   IsOffHour,///[In]
                                Int32                  NID,      ///[In]
                                Int32                  Seq,      ///[In] 
                                const UFC::AnsiString& Key,      ///[In] 
                                const UFC::AnsiString& Data,     ///[In]
                                const UFC::AnsiString& PBNO,     ///[In]
                                const UFC::AnsiString& Host,     ///[In]
                                const UFC::AnsiString& PVCID,    ///[In]
                                const UFC::AnsiString& Symbol,   ///[In]
                                Int32                  Qty,      ///[In]
                                Int32                  Price,    ///[In]
                                const TMPExtFields*    TMPFields)///[In]
{
    UFC::AnsiString TimeNow,TMPExtStr;
    char            Type;

    if( IsOffHour == TRUE )
        Type = (IsConfirm == TRUE) ? 'c':'f';
    else
        Type = (IsConfirm == TRUE) ? 'C':'F';
    UFC::GetHHMMSS( TimeNow );
    if( TMPFields != NULL )
    {
        UFC::AnsiString TMPFieldsStr;
        TMPFields->ToString( TMPFieldsStr );
        TMPExtStr.Printf( "|%s\n",  TMPFieldsStr.c_str() );
    }
    else
        TMPExtStr = "\n";    
    if( Key.Length() == 0 )
        LogString.Printf( "%s|%c|%010u|%010u|%s|%s|%s|%s|%s|%s|%d|%d%s",TimeNow.c_str(),Type,NID,Seq, "null", Data.c_str(), PBNO.c_str(),Host.c_str(),PVCID.c_str(),Symbol.c_str(),Qty,Price,TMPExtStr.c_str());
    else
        LogString.Printf( "%s|%c|%010u|%010u|%s|%s|%s|%s|%s|%s|%d|%d%s",TimeNow.c_str(),Type,NID,Seq, Key.c_str(), Data.c_str(), PBNO.c_str(),Host.c_str(),PVCID.c_str(),Symbol.c_str(),Qty,Price,TMPExtStr.c_str());
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExecutionParser::Render(   char*                  LogString,/// [Out]
                                BOOL                   IsConfirm,///[In]
                                BOOL                   IsOffHour,///[In]
                                Int32                  NID,      ///[In]
                                Int32                  Seq,      ///[In] 
                                const UFC::AnsiString& Key,      ///[In] 
                                const UFC::AnsiString& Data,     ///[In]
                                const UFC::AnsiString& PBNO,     ///[In]
                                const UFC::AnsiString& Host,     ///[In]
                                const UFC::AnsiString& PVCID,    ///[In]
                                const UFC::AnsiString& Symbol,   ///[In]
                                Int32                  Qty,      ///[In]
                                Int32                  Price,    ///[In]
                                const TMPExtFields*    TMPFields)///[In]
{
    UFC::AnsiString TimeNow,TMPExtStr;
    char            Type;

    if( IsOffHour == TRUE )
        Type = (IsConfirm == TRUE) ? 'c':'f';
    else
        Type = (IsConfirm == TRUE) ? 'C':'F';
    UFC::GetHHMMSS( TimeNow );
    if( TMPFields != NULL )
    {
        UFC::AnsiString TMPFieldsStr;
        TMPFields->ToString( TMPFieldsStr );
        TMPExtStr.Printf( "|%s\n",  TMPFieldsStr.c_str() );
    }
    else
        TMPExtStr = "\n";    
    if( Key.Length() == 0 )
        sprintf( LogString, "%s|%c|%010u|%010u|%s|%s|%s|%s|%s|%s|%d|%d%s",TimeNow.c_str(),Type,NID,Seq, "null", Data.c_str(), PBNO.c_str(),Host.c_str(),PVCID.c_str(),Symbol.c_str(),Qty,Price,TMPExtStr.c_str());
    else
        sprintf( LogString, "%s|%c|%010u|%010u|%s|%s|%s|%s|%s|%s|%d|%d%s",TimeNow.c_str(),Type,NID,Seq, Key.c_str(), Data.c_str(), PBNO.c_str(),Host.c_str(),PVCID.c_str(),Symbol.c_str(),Qty,Price,TMPExtStr.c_str());
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Formats for TAIFEX ( TMP Binary format )
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExecutionParser::Render(   UFC::AnsiString&       LogString,///[Out]
                                BOOL                   IsConfirm,///[In]
                                BOOL                   IsOffHour,///[In]    
                                Int32                  NID,      ///[In]
                                Int32                  Seq,      ///[In]
                                const UFC::AnsiString& Key,      ///[In]
                                UInt8*                 Data,     ///[In]
                                Int32                  Size,     ///[In]
                                const UFC::AnsiString& PBNO,     ///[In]
                                const UFC::AnsiString& Host,     ///[In]
                                const UFC::AnsiString& PVCID,    ///[In] 
                                const UFC::AnsiString& Symbol,   ///[In]
                                Int32                  Qty,      ///[In]
                                Int32                  Price )   ///[In]
{
    UFC::AnsiString TimeNow,HexString;
    char            Type;
    
    if( IsOffHour == TRUE )///< Binary
        Type = (IsConfirm == TRUE) ? 'e':'m';
    else
        Type = (IsConfirm == TRUE) ? 'E':'M';    
    UFC::GetHHMMSS( TimeNow );
    UFC::BinaryToHexString( Data, Size, HexString );
    if( Key.Length() == 0 )
        LogString.Printf( "%s|%c|%010u|%010u|%s|%s|%s|%s|%s|%s|%d|%d\n",TimeNow.c_str(),Type,NID,Seq, "null", HexString.c_str(), PBNO.c_str(),Host.c_str(),PVCID.c_str(),Symbol.c_str(),Qty,Price );
    else
        LogString.Printf( "%s|%c|%010u|%010u|%s|%s|%s|%s|%s|%s|%d|%d\n",TimeNow.c_str(),Type,NID,Seq, Key.c_str(), HexString.c_str(), PBNO.c_str(),Host.c_str(),PVCID.c_str(),Symbol.c_str(),Qty,Price);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExecutionParser::Render(   char*                  LogString,///[Out]
                                BOOL                   IsConfirm,///[In]
                                BOOL                   IsOffHour,///[In]    
                                Int32                  NID,      ///[In]
                                Int32                  Seq,      ///[In]
                                const UFC::AnsiString& Key,      ///[In]
                                UInt8*                 Data,     ///[In]
                                Int32                  Size,     ///[In]
                                const UFC::AnsiString& PBNO,     ///[In]
                                const UFC::AnsiString& Host,     ///[In]
                                const UFC::AnsiString& PVCID,    ///[In] 
                                const UFC::AnsiString& Symbol,   ///[In]
                                Int32                  Qty,      ///[In]
                                Int32                  Price )   ///[In]
{
    UFC::AnsiString TimeNow,HexString;
    char            Type;
    
    if( IsOffHour == TRUE )///< Binary
        Type = (IsConfirm == TRUE) ? 'e':'m';
    else
        Type = (IsConfirm == TRUE) ? 'E':'M';    
    UFC::GetHHMMSS( TimeNow );
    UFC::BinaryToHexString( Data, Size, HexString );
    if( Key.Length() == 0 )
        sprintf( LogString,"%s|%c|%010u|%010u|%s|%s|%s|%s|%s|%s|%d|%d\n",TimeNow.c_str(),Type,NID,Seq, "null", HexString.c_str(), PBNO.c_str(),Host.c_str(),PVCID.c_str(),Symbol.c_str(),Qty,Price );
    else
        sprintf( LogString,"%s|%c|%010u|%010u|%s|%s|%s|%s|%s|%s|%d|%d\n",TimeNow.c_str(),Type,NID,Seq, Key.c_str(), HexString.c_str(), PBNO.c_str(),Host.c_str(),PVCID.c_str(),Symbol.c_str(),Qty,Price);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageType OrderType( BOOL IsTAIFEX, Int32 HeaderInt )
{
    if( IsTAIFEX == FALSE)
    {
        switch( HeaderInt )
        {        
            case 300100:///< T010 TSE Buy order
            case 300200:///< T010 TSE Sell order
            case 320100:///< P010 TSE off Hour Buy order
            case 320200:///< P010 TSE off Hour Sell order             
            case 330100:///< O110 TSE OddEx Buy order
            case 330200:///< O110 TSE OddEx Sell order             
            case 400100:///< O010 TSE odd Buy order
            case 400200:///< O010 TSE odd Sell order             
            case 930100:///< T010 OTC Buy order
            case 930200:///< T010 OTC Sell order
            case 980100:///< P010 OTC off Hour Buy order
            case 980200:///< P010 OTC off Hour Sell order
            case 830100:///< O110 OTC OddEx Buy order
            case 830200:///< O110 OTC Oddex Sell order
            case 940100:///< O010 OTC odd Buy order
            case 940200:///< O010 OTC odd Sell order                                     
            case 900100:///< V010 OTC
            case 310100:///< V010 TSE
            case 700100:///< A010 TSE
            case 970100:///< E010 OTC
            case 410100:///< E010 TSE
            case 890100:///< Ex010 OTC
            case 420100:///< Ex010 TSE            
                        return mtNewOrder;     ///< New order message                               
            case 300300:
            case 330300:
            case 830300:
            case 320300:
            case 400300:    
            case 930300:
            case 940300:
            case 980300: 
            case 900300: ///< V010 OTC 
            case 310300: ///< V010 TSE           
            case 700300: ///< A010 TSE           
            case 970300: ///< E010 OTC            
            case 410300: ///< E010 TSE
            case 890300: ///< Ex010 OTC      
            case 420300: ///< Ex010 TSE    
                        return mtReduceOrder;  ///< R010 reduce order message
            case 300400:
            case 330400:
            case 830400:
            case 320400:
            case 400400:
            case 930400:
            case 940400:
            case 980400: 
            case 900200: ///< V010 OTC 
            case 310200: ///< V010 TSE           
            case 700200: ///< A010 TSE           
            case 970200: ///< E010 OTC            
            case 410200: ///< E010 TSE
            case 890200: ///< Ex010 OTC
            case 420200: ///< Ex010 TSE
                        return mtCancelOrder;  ///< R010 delete order message
            case 300600:///< T010 TSE 
            case 930600:///< T010 OTC 
                        return mtReplacePrice;
            case 300500:
            case 330500:
            case 830500:                
            case 320500:
            case 400500:    
            case 930500:
            case 940500:
            case 980500: 
                        return mtOrderStatus;
            default:    return mtUnknownMsg;
        }
    }
    else
    {
        switch( HeaderInt )
        {
            case 300100: return mtNewOrder;     ///< R010 new order message
            case 300600: return mtReplacePrice; ///< R010 Replace Px    
            case 300200: return mtReduceOrder;  ///< R010 reduce order message
            case 300300: return mtCancelOrder;  ///< R010 delete order message
            case 300500: return mtOrderStatus;
            case 300108: return mtNewQuote;     ///< R090 New Quote message
            case 300208: return mtReduceQuote;  ///< R090 reduce Quote message
            case 300308: return mtCancelQuote;  ///< R090 delete Quote message
            case 300508: return mtQuoteStatus;
            case 300106: return mtQuoteRequest; ///< R070 Quote request message
            default:     return mtUnknownMsg;
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------
TradeSession GetTradeSessionBySubsystemID( int Subsys )
{    
    switch( Subsys )
    {              
        case 30:
        case 93: return tsNormal;
        case 40:
        case 94: return tsOdd;
        case 33:
        case 83: return tsOddEx;
        case 32:
        case 98: return tsPost;
        case 31:
        case 90: return tsLend;
        case 70: return tsAuction;
        case 41:
        case 97: return tsTender;
        case 42:
        case 89: return tsTenderEx;
        case 96: return tsNegotiatePx;
        default: return tsNormal;
    }    
}
//------------------------------------------------------------------------------
BOOL IsNewOrder( const UFC::AnsiString & Order, BOOL IsTAIFEX )
{
    Int32 Function = Order.SubString( 0,6 ).ToInt();
    
    if( IsTAIFEX )
    {
        switch( Function )    
        {      
            case 300101:                  ///< R020 new order confirm message        
            case 300109: return TRUE;     ///< R100 new Quote confirm message        
            default:     return FALSE;        
        }        
    }
    else
    {
        switch( Function )
        {       
            case 300101:
            case 300201: ///< T020 TSEC
            case 930101:
            case 930201: ///< T020 OTC
            case 400101:
            case 400201: ///< O020 TSEC
            case 940101:
            case 940201: ///< O020 OTC            
                
            case 330101:
            case 330201: ///< O120 TSEC
            case 830101:
            case 830201: ///< O120 OTC            
                
            case 320101:                
            case 320201: ///< P020 TSE
            case 980101:
            case 980201: ///< P020 OTC  (new order confirm message)
            case 700101:
            case 310101: ///< A020
            case 900101:
            case 410101: ///< E020 
            case 970101: ///< V020
            case 420101:
            case 890101: ///< Ex020
                         return TRUE;
            default:     return FALSE;        
        }
    }
}
//------------------------------------------------------------------------------
BOOL IsTMPNewOrder( UInt8* Order, Int32 Size )
{
    if( Size > TMP_HEADER_FULL_SIZE  )
    {
        UInt8 TMPType  = Order[ TMP_HEADER_MSGTYPE_OFFSET ];
        char  ExecType = (char)Order[ TMP_HEADER_FULL_SIZE ];

        if( TMPType == 101 || TMPType == 109 ) ///< R01 Order, R09 Quote
        {
            if( ExecType == '0' )
                return TRUE;
        }
    }
    return FALSE;
}
//------------------------------------------------------------------------------
char GetTMPExecType(  UInt8* Order, Int32  )
{
    return (char)Order[ TMP_HEADER_FULL_SIZE ];
}
//------------------------------------------------------------------------------
BOOL GetTMPOrderID( UInt8* Order, Int32 Size, UFC::AnsiString& OID )
{
    if( Size >  TMP_HEADER_FULL_SIZE  )
    {
        UInt8 TMPType  = Order[ TMP_HEADER_MSGTYPE_OFFSET ];

        if( TMPType == 101 ) //R01
        {
            OID = UFC::AnsiString( (const char*)Order + TMP_R01_ORDERID_OFFSET , 5 );
            return TRUE;
        }
        else if( TMPType == 107 ) //R09 Quote
        {
            OID = UFC::AnsiString( (const char*)Order + TMP_R07_ORDERID_OFFSET , 5 );
            return TRUE;
        }
        else if( TMPType == 109 ) //R09 Quote
        {
            OID = UFC::AnsiString( (const char*)Order + TMP_R09_ORDERID_OFFSET , 5 );
            return TRUE;
        }
        else if( TMPType == 102 ) // R02
        {
            OID = UFC::AnsiString( (const char*)Order + TMP_R02_ORDERID_OFFSET , 5 );
            return TRUE;
        }
        else  if( TMPType == 122 ) // R22
        {
            OID = UFC::AnsiString( (const char*)Order + TMP_R22_ORDERID_OFFSET , 5 );
            return TRUE;
        }
        else  if( TMPType == 103 ) // R22
        {
            OID = UFC::AnsiString( (const char*)Order + TMP_R03_ORDERID_OFFSET , 5 );
            return TRUE;
        }
    }
    OID = "00000";
    return FALSE;
}
//------------------------------------------------------------------------------
MessageType GetTMPMessageType( UInt8* Order, Int32 Size, UFC::AnsiString& Account,BOOL& IsOrder )
{
    if( Size >  TMP_HEADER_FULL_SIZE  )
    {
        UInt8 TMPType  = Order[ TMP_HEADER_MSGTYPE_OFFSET ];
        char  ExecType = (char)Order[ TMP_HEADER_FULL_SIZE ];

        if( TMPType == 101 ) ///< R01 Order
        {
            IsOrder = TRUE;
            if( Size < TMP_R01_ACCOUNT_OFFSET + 4 )
                return mtUnknownMsg;
            int AccNum = UFC::NInt32::ToInt32( Order + TMP_R01_ACCOUNT_OFFSET );
            Account.Printf( "%07d", AccNum );
            switch( ExecType )
            {
                case '0': return mtNewOrder;
                case '4': return mtCancelOrder;
                case '5': return mtReduceOrder;
                case 'm':
                case 'M': return mtReplacePrice;
                case 'I': return mtOrderStatus;
                default : return mtUnknownMsg;
            }
        }
        else if( TMPType == 109 ) //R09 Quote
        {
            IsOrder = FALSE;
            if( Size < TMP_R09_ACCOUNT_OFFSET + 4 )
                return mtUnknownMsg;
            int AccNum = UFC::NInt32::ToInt32( Order + TMP_R09_ACCOUNT_OFFSET );
            Account.Printf( "%07d", AccNum );
            switch( ExecType )
            {
                case '0': return mtNewQuote;
                case '4': return mtCancelQuote;
                case '5': return mtReduceQuote;
                case 'I': return mtOrderStatus;
                default : return mtUnknownMsg;
            }
        }
        else if( TMPType == 107 ) //R07 Quote
        {
            IsOrder = FALSE;
            return mtQuoteRequest;
        }
    }
    return mtUnknownMsg;
}
//------------------------------------------------------------------------------
MessageType TMPOrderType( UInt8* Order, Int32 Size )
{
    if( Size >  TMP_HEADER_FULL_SIZE  )
    {
        UInt8 TMPType  = Order[ TMP_HEADER_MSGTYPE_OFFSET ];
        char  ExecType = (char)Order[ TMP_HEADER_FULL_SIZE ];

        if( TMPType == 101 ) ///< R01 Order
        {
            switch( ExecType )
            {
                case '0': return mtNewOrder;
                case '4': return mtCancelOrder;
                case '5': return mtReduceOrder;
                case 'm':
                case 'M': return mtReplacePrice;
                case 'I': return mtOrderStatus;
                default : return mtUnknownMsg;
            }
        }
        else if( TMPType == 109 ) //R09 Quote
        {
            switch( ExecType )
            {
                case '0': return mtNewQuote;
                case '4': return mtCancelQuote;
                case '5': return mtReduceQuote;
                case 'I': return mtOrderStatus;
                default : return mtUnknownMsg;
            }
        }
        else if( TMPType == 107 ) //R07 Quote
        {
            return mtQuoteRequest;
        }
    }
    return mtUnknownMsg;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char  GetOrderType(  const char* Order,  Int32 Size )
{
    if( Size <= TSE_ORDERTYPE_OFFSET )
        return '2'; ///< 1:Market 2:Limit
    return Order[ TSE_ORDERTYPE_OFFSET ];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageType  GetOrderID( BOOL IsTAIFEX, BOOL IsBin,  UInt8* Order, Int32 Size, UFC::AnsiString& OrderID )
{
    MessageType Type;
    
    if( IsTAIFEX == TRUE )///< For TAIFEX.
    {
        if( IsBin == TRUE ) ///< Binary format.
        {
            UInt8 TMPType  = Order[ TMP_HEADER_MSGTYPE_OFFSET ];
            char  ExecType = (char)Order[ TMP_HEADER_FULL_SIZE ];

            if( TMPType == 101 ) ///< R01 Order
            {                
                if( Size < TMP_R01_ORDERID_OFFSET + 5 )
                    return mtUnknownMsg;
                OrderID = UFC::AnsiString( (const char*)Order + TMP_R01_ORDERID_OFFSET , 5 );
                switch( ExecType )
                {
                    case '0': return mtNewOrder;
                    case '4': return mtCancelOrder;
                    case '5': return mtReduceOrder;
                    case 'm':
                    case 'M': return mtReplacePrice;
                    case 'I': return mtOrderStatus;
                    default : return mtUnknownMsg;
                }
            }
            else if( TMPType == 109 ) //R09 Quote
            {
                if( Size < TMP_R09_ORDERID_OFFSET + 5 )
                    return mtUnknownMsg;
                OrderID = UFC::AnsiString( (const char*)Order + TMP_R09_ORDERID_OFFSET , 5 );                
                switch( ExecType )
                {
                    case '0': return mtNewQuote;
                    case '4': return mtCancelQuote;
                    case '5': return mtReduceQuote;
                    case 'I': return mtOrderStatus;
                    default : return mtUnknownMsg;
                }
            }
            else if( TMPType == 107 ) //R07 Quote request
            {
                if( Size < TMP_R07_ORDERID_OFFSET + 5 )
                    return mtUnknownMsg;
                OrderID = UFC::AnsiString( (const char*)Order + TMP_R07_ORDERID_OFFSET , 5 );
                return mtQuoteRequest;
            }
        }
        else ///< Text format
        {
            Type = OrderType( IsTAIFEX, UFC::AnsiString( (const char*)Order , 6 ).ToInt() );
            if( Type != mtQuoteRequest )
                OrderID = UFC::AnsiString( (const char*)Order + TFX_ORDERID_OFFSET, 5 );
            else
                OrderID = "00000";
            return Type;
        }
    }
    else ///< For TSE/OTC
    {
        Type = OrderType( IsTAIFEX, UFC::AnsiString( (const char*)Order , 6 ).ToInt() );
        OrderID = UFC::AnsiString( (const char*)Order +TSE_ORDERID_OFFSET, 5 );
        return Type;
    }
    return mtUnknownMsg;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageType  GetOrderID( BOOL IsTAIFEX, BOOL IsBin,  UInt8* Order, Int32 Size, char* OrderID )
{
    MessageType Type;
    
    if( IsTAIFEX == TRUE )///< For TAIFEX.
    {
        if( IsBin == TRUE ) ///< Binary format.
        {
            UInt8 TMPType  = Order[ TMP_HEADER_MSGTYPE_OFFSET ];
            char  ExecType = (char)Order[ TMP_HEADER_FULL_SIZE ];

            if( TMPType == 101 ) ///< R01 Order
            {                
                if( Size < TMP_R01_ORDERID_OFFSET + 5 )
                    return mtUnknownMsg;
                strncpynt( OrderID, (const char*)Order + TMP_R01_ORDERID_OFFSET , 5 );                
                switch( ExecType )
                {
                    case '0': return mtNewOrder;
                    case '4': return mtCancelOrder;
                    case '5': return mtReduceOrder;
                    case 'm':
                    case 'M': return mtReplacePrice;
                    case 'I': return mtOrderStatus;
                    default : return mtUnknownMsg;
                }
            }
            else if( TMPType == 109 ) //R09 Quote
            {
                if( Size < TMP_R09_ORDERID_OFFSET + 5 )
                    return mtUnknownMsg;
                strncpynt( OrderID, (const char*)Order + TMP_R09_ORDERID_OFFSET , 5 );                                
                switch( ExecType )
                {
                    case '0': return mtNewQuote;
                    case '4': return mtCancelQuote;
                    case '5': return mtReduceQuote;
                    case 'I': return mtOrderStatus;
                    default : return mtUnknownMsg;
                }
            }
            else if( TMPType == 107 ) //R07 Quote request
            {
                if( Size < TMP_R07_ORDERID_OFFSET + 5 )
                    return mtUnknownMsg;
                strncpynt( OrderID, (const char*)Order + TMP_R07_ORDERID_OFFSET , 5 );                
                return mtQuoteRequest;
            }
        }
        else ///< Text format
        {
            Type = OrderType( IsTAIFEX, UFC::AnsiString::StrToInt32( (const char*)Order , 6 ) );
            if( Type != mtQuoteRequest )
                strncpynt( OrderID, (const char*)Order + TFX_ORDERID_OFFSET, 5 );
            else
                strncpynt( OrderID, "00000", 5 );            
            return Type;
        }
    }
    else ///< For TSE/OTC
    {
        Type = OrderType( IsTAIFEX, UFC::AnsiString::StrToInt32( (const char*)Order , 6 ) );                
        strncpynt( OrderID, (const char*)Order +TSE_ORDERID_OFFSET, 5 );        
        return Type;
    }
    return mtUnknownMsg;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TradeMessageType GetTradeMessageType( const UFC::AnsiString& X010 )
{
    int SubSystem = X010.SubString( 0, 2).ToInt();
    int FuncCode  = X010.SubString( 2, 2).ToInt();
    int MsgType   = X010.SubString( 4, 2).ToInt();

    switch( SubSystem ) 
    {        
        case 93:
        case 30: if( MsgType == 0 && ( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 || FuncCode == 5 || FuncCode == 6 ) )           
                     return tmtT010;
                 break;    
        case 33:
        case 83: if( MsgType == 0 && ( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 || FuncCode == 5 ) )
                    return tmtO110;
                 break;           
        case 98:
        case 32: if( MsgType == 0 && ( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 || FuncCode == 5 ) )
                    return tmtP010;        
                 break;
        case 94:
        case 40: if( MsgType == 0 && ( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 || FuncCode == 5 ) )
                    return tmtO010;        
                 break;        
        case 70: if( MsgType == 0 && ( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 ) )
                    return tmtA010;        
                 break;
        case 90:
        case 31: if( MsgType == 0 && ( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 ) )
                    return tmtV010;        
                 break;
        case 97:
        case 41: if( MsgType == 0 && ( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 ) )
                    return tmtE010;        
                 break;
        default: break;
    }    
    return tmtUnknown;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void X010ToX020( const UFC::AnsiString& Order, int ErrorCode, UFC::AnsiString& X020, BOOL NewVer, BOOL NewExVer )
{
    UFC::AnsiString  X25Time,Today,ErrStr,BefQty,AfterQty,Price;
    UFC::AnsiString  X010( Order );
    UFC::UDateTime   Now;         
        
    ErrStr.Printf( "%02d", ErrorCode );    
    switch( GetTradeMessageType( X010 )  )
    {
        case tmtT010:///< Normal Transection sub-system. OK
                    if( NewVer == TRUE ) ///< New Version
                    {
                        Today.Printf( "%04d%02d%02d", Now.getYear(), Now.getMonth(), Now.getDate() );   
                        UFC::GetTimeString( X25Time, FALSE ); ///< HHMMSSmmm
                        X010.PadThis( 59,' ' );
                        X020.Printf( "%s%s%s%06d%06d",X010.c_str(),Today.c_str(),X25Time.c_str(),0,0);                                         
                    }
                    else
                    {
                        Today.Printf( "00%02d%02d", Now.getMonth(), Now.getDate() );   
                        UFC::GetHHMMSS( X25Time );
                        X010.PadThis( 51,' ' );
                        X020.Printf( "%s%s%s%03d%03d",X010.c_str(),Today.c_str(),X25Time.c_str(),0,0);                 
                    }
                    break;        
        case tmtP010:///< Post Transection sub-system. OK                                
                    if( NewVer == TRUE ) ///< New Version
                    {
                        Today.Printf( "%04d%02d%02d", Now.getYear(), Now.getMonth(), Now.getDate() );   
                        UFC::GetTimeString(  X25Time, FALSE ); ///< HHMMSSmmm
                        X010.PadThis( 59,' ' );    
                        X020.Printf( "%s%s%s%06d%06d",X010.c_str(),Today.c_str(),X25Time.c_str(),0,0);                      
                    }
                    else
                    {
                        Today.Printf( "00%02d%02d", Now.getMonth(), Now.getDate() );   
                        UFC::GetHHMMSS( X25Time );
                        X010.PadThis(51,' ' );
                        X020.Printf( "%s%s%s%03d%03d",X010.c_str(),Today.c_str(),X25Time.c_str(),0,0);                 
                    }
                    break;        
        case tmtO010:///< Odd Transection sub-system. OK                    
                    if( NewVer == TRUE ) ///< New Version
                    {
                        Today.Printf( "%04d%02d%02d", Now.getYear(), Now.getMonth(), Now.getDate() );   
                        UFC::GetTimeString(  X25Time, FALSE ); ///< HHMMSSmmm
                        X010.PadThis( 59,' ' );    
                        X020.Printf( "%s%s%s%06d%06d",X010.c_str(),Today.c_str(),X25Time.c_str(),0,0);                      
                    }
                    else
                    {
                        Today.Printf( "00%02d%02d", Now.getMonth(), Now.getDate() );   
                        UFC::GetHHMMSS( X25Time );
                        X010.PadThis( 58,' ' );    
                        X020.Printf( "%s%s%s%08d%08d",X010.c_str(),Today.c_str(),X25Time.c_str(),0,0);                      
                    }
                    break;
        case tmtO110:///< Odd Ex Transection sub-system. OK                    
                    Today.Printf( "%04d%02d%02d", Now.getYear(), Now.getMonth(), Now.getDate() );   
                    UFC::GetTimeString( X25Time, FALSE ); ///< HHMMSSmmm
                    X010.PadThis( 59,' ' );
                    X020.Printf( "%s%s%s%06d%06d",X010.c_str(),Today.c_str(),X25Time.c_str(),0,0);                                                             
                    break;                     
        case tmtV010:///< Lend Transection sub-system. OK            
                    if( NewExVer == TRUE || NewVer == TRUE)
                    {                        
                        Today.Printf( "%04d%02d%02d", Now.getYear(), Now.getMonth(), Now.getDate() );   
                        UFC::GetHHMMSSmm( X25Time ); ///< HHMMSSmm                                                
                        X010.PadThis( 53,' ' );
                        X020.Printf( "%s%s%s%06d%06d",X010.c_str(),Today.c_str(),X25Time.c_str(),0,0);                      
                    }
                    else
                    {
                        Today.Printf( "00%02d%02d", Now.getMonth(), Now.getDate() );   
                        UFC::GetHHMMSS( X25Time );
                        X010.PadThis( 52,' ' );
                        X020.Printf( "%s%s%s%06d%06d",X010.c_str(),Today.c_str(),X25Time.c_str(),0,0);                      
                    }
                    break;
        case tmtA010:///< Auction Transection sub-system. OK                    
                    if( NewExVer == TRUE || NewVer == TRUE)
                    {                        
                        Today.Printf( "%04d%02d%02d", Now.getYear(), Now.getMonth(), Now.getDate() );                           
                        UFC::GetHHMMSSmm( X25Time ); ///< HHMMSSmm                        
                        X010.PadThis( 59,' ' );                        
                        BefQty.PadThis( 12,'0');
                        AfterQty.PadThis( 12,'0');
                        X020.Printf( "%s%s%s%s%s%09d%09d",X010.c_str(),Today.c_str(),X25Time.c_str(),BefQty.c_str(),AfterQty.c_str(),0,0);                                     
                    }
                    else
                    {
                        Today.Printf( "00%02d%02d", Now.getMonth(), Now.getDate() );   
                        UFC::GetHHMMSS( X25Time );    
                        X010.PadThis( 56,' ' );                    
                        Price.PadThis( 6,'0');
                        BefQty.PadThis( 12,'0');
                        AfterQty.PadThis( 12,'0');
                        X020.Printf( "%s%s%s%s%s%s%06d",X010.c_str(),Today.c_str(),X25Time.c_str(),BefQty.c_str(),AfterQty.c_str(),Price.c_str(),0);                                     
                    }    
                    break;               
        case tmtE010:///< Tender Transection sub-system. OK                                 
                    if( NewExVer == TRUE || NewVer == TRUE)
                    {                        
                        Today.Printf( "%04d%02d%02d", Now.getYear(), Now.getMonth(), Now.getDate() );   
                        UFC::GetHHMMSSmm( X25Time ); ///< HHMMSSmm                        
                        X010.PadThis( 61,' ' );                        
                        BefQty.PadThis( 12,'0');
                        AfterQty.PadThis( 12,'0');
                        X020.Printf( "%s%s%s%s%s%09d%09d",X010.c_str(),Today.c_str(),X25Time.c_str(),BefQty.c_str(),AfterQty.c_str(),0,0);                                     
                    }
                    else
                    {                        
                        Today.Printf( "00%02d%02d", Now.getMonth(), Now.getDate() );   
                        UFC::GetHHMMSS( X25Time );
                        X010.PadThis( 58,' ' );
                        Price.PadThis( 6,'0');
                        BefQty.PadThis( 12,'0');
                        AfterQty.PadThis( 12,'0');
                        X020.Printf( "%s%s%s%s%s%s%06d",X010.c_str(),Today.c_str(),X25Time.c_str(),BefQty.c_str(),AfterQty.c_str(),Price.c_str(),0);                                     
                    }        
                    break;                       
        default:    if( NewVer == TRUE ) ///< New Version
                    {
                        Today.Printf( "%04d%02d%02d", Now.getYear(), Now.getMonth(), Now.getDate() );   
                        UFC::GetTimeString(  X25Time, FALSE ); ///< HHMMSSmmm                        
                        X020.Printf( "%s%s%s%06d%06d",X010.c_str(),Today.c_str(),X25Time.c_str(),0,0);                                         
                    }
                    else
                    {            
                        Today.Printf( "00%02d%02d", Now.getMonth(), Now.getDate() );   
                        UFC::GetHHMMSS( X25Time );
                        X020.Printf( "%s%s%s%03d%03d",X010.c_str(),Today.c_str(),X25Time.c_str(),0,0);                 
                    }
                    UFC::BufferedLog::Printf( " [X010ToX020] Unknown message[%s]", Order.c_str() );                    
                    break;            
    }
    ///< Modify Function code.( To X020)
    X020[ 4 ] = '0';
    X020[ 5 ] = '1';
    ///< Modify Error code
    X020[ 12 ] = ErrStr[ 0 ];
    X020[ 13 ] = ErrStr[ 1 ];
    X020[ 18 ] = '0';
    X020[ 19 ] = '0';    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
