//------------------------------------------------------------------------------------------------------------------------
#include "RCUtility.h"
#include <math.h>
//------------------------------------------------------------------------------------------------------------------------
// class RCUtility
//------------------------------------------------------------------------------------------------------------------------
void RCUtility::PackBcdDecode( UInt8* Target, UInt8* Source, int SourceLen )
{
	// User bits operation, so need use unsigned char type
	for ( int i = 0; i < SourceLen; i++ )
	{
		Target[2*i]   = ( Source[i] >> 4   ) + 0x30; ///< Get first 4 bits to become 1 byte.
		Target[2*i+1] = ( Source[i] & 0x0f ) + 0x30; ///< Get last 4 bits to become 1 byte.
	}
}
//------------------------------------------------------------------------------------------------------------------------
void RCUtility::PackBcdDecodeToCStr( UInt8* Target, UInt8* Source, int SourceLen )
{
	// Use bits operation, so need use unsigned char type
	for ( int i = 0; i < SourceLen; i++ )
	{
		Target[2*i]   = ( Source[i] >> 4   ) + 0x30; ///< Get first 4 bits to become 1 byte.
		Target[2*i+1] = ( Source[i] & 0x0f ) + 0x30; ///< Get last 4 bits to become 1 byte.
	}
	Target[ SourceLen*2 ] = 0; ///< pad for null terminate string
}
//------------------------------------------------------------------------------------------------------------------------
// Round( double Value, int Precision ) : Currency practice
//------------------------------------------------------------------------------------------------------------------------
double RCUtility::Round( double Value, int Precision )
{
	static const double doBase = 10.0;
	double Complete5, Complete5Int;
    
	Complete5 = Value * pow( doBase, (double)Precision );
	if ( Value < 0.0 )
		Complete5 = Complete5 - 0.5 - mFloatEqual_Tolerance;
	else
		Complete5 = Complete5 + 0.5 + mFloatEqual_Tolerance;
	modf( Complete5, &Complete5Int );

	return Complete5Int / pow( doBase, (double)Precision );
}
//------------------------------------------------------------------------------------------------------------------------
// Ceiling( double Value, int Precision ) : Currency practice
//------------------------------------------------------------------------------------------------------------------------
double RCUtility::Ceiling( double Value, int Precision )
{
	static const double doBase = 10.0;
	double Complete;

	if ( Value < 0.0 )
	{
		Complete = ( Value + mFloatEqual_Tolerance ) * pow( doBase, (double)Precision );
		Complete = floor( Complete );
	}
	else
	{
		Complete = ( Value - mFloatEqual_Tolerance ) * pow( doBase, (double)Precision );
		Complete = ceil( Complete );
	}

	return Complete / pow( doBase, (double)Precision );
}
//------------------------------------------------------------------------------------------------------------------------
// Floor( double Value, int Precision ) : Currency practice
//------------------------------------------------------------------------------------------------------------------------
double RCUtility::Floor( double Value, int Precision )
{
	static const double doBase = 10.0;
	double Complete;

	if ( Value < 0.0 )
	{
		Complete = ( Value - mFloatEqual_Tolerance ) * pow( doBase, (double)Precision );
		Complete = ceil( Complete );
	}
	else
	{
		Complete = ( Value + mFloatEqual_Tolerance ) * pow( doBase, (double)Precision );
		Complete = floor( Complete );
	}

	return Complete / pow( doBase, (double)Precision );
}
//------------------------------------------------------------------------------------------------------------------------
double RCUtility::ShiftIntToDouble( int SrcValue, int Digit )
{
	double Dividend = pow( 10.0, (double)Digit );
	return ( (double)SrcValue ) / Dividend;
}
//------------------------------------------------------------------------------------------------------------------------
int RCUtility::ShiftDoubleToInt( double SrcValue, int Digit )
{
	double Precision = pow( 0.1, (double)(Digit+1) );
	double Multiple  = pow( 10.0, (double)Digit );
	if ( SrcValue >= 0.0 )
		return (int)( ( SrcValue + Precision ) * Multiple );
	else
		return (int)( ( SrcValue - Precision ) * Multiple );
}
//------------------------------------------------------------------------------------------------------------------------
void RCUtility::AssignDoubleToString( double SrcValue, UFC::AnsiString& DestStr, int ShowPoint )
{
	char strFormat[32];
	switch ( ShowPoint )
	{
		case 0 : DestStr.Printf( "%.0lf", SrcValue ); break;
		case 1 : DestStr.Printf( "%.1lf", SrcValue ); break;
		case 2 : DestStr.Printf( "%.2lf", SrcValue ); break;
		case 3 : DestStr.Printf( "%.3lf", SrcValue ); break;
		case 4 : DestStr.Printf( "%.4lf", SrcValue ); break;
		default: sprintf( strFormat, "%%.%dlf", ShowPoint );
				 DestStr.Printf( strFormat, SrcValue );
				 break;
	}
}
//------------------------------------------------------------------------------------------------------------------------
int	RCUtility::GetHHMMSSmmmAsInt( void ) ///< get time for HHMMSSmmm as Int
{
	struct timeb  t;
	struct tm     tblock;
	int    ret;

	ftime(&t);
	if ( localtime_r(&t.time, &tblock) == 0 )
		return 0;
	ret = tblock.tm_hour;
	ret = ret * 100 + tblock.tm_min;
	ret = ret * 100 + tblock.tm_sec;
	ret = ret * 1000 + t.millitm;
	return ret;
}
//------------------------------------------------------------------------------------------------------------------------
Int64 RCUtility::GetHHMMSSm6AsInt64( void )	///< get time for HHMMSSmmmmmm as Int64
{
	struct timeval	tv;
	struct tm		tblock;
	Int64			ret;

	gettimeofday( &tv, NULL );
	if ( localtime_r(&tv.tv_sec, &tblock) == 0 )
		return 0;
	ret = tblock.tm_hour;
	ret = ret * 100 + tblock.tm_min;
	ret = ret * 100 + tblock.tm_sec;
	ret = ret * 1000000 + tv.tv_usec;
	return ret;
}
//------------------------------------------------------------------------------------------------------------------------
Int64 RCUtility::time_us( void ) ///< get microseconds of today
{
	struct timeval	tv;
	struct tm		tblock;
	Int64			ret;

	gettimeofday( &tv, NULL );
	if ( localtime_r(&tv.tv_sec, &tblock) == 0 )
		return 0;
	ret = tblock.tm_hour;
	ret = ret * 60 + tblock.tm_min;
	ret = ret * 60 + tblock.tm_sec;
	ret = ret * 1000000 + tv.tv_usec;
	return ret;
}
//------------------------------------------------------------------------------------------------------------------------
void RCUtility::GetHH_MM_SS_m6( char* strBuf ) ///< get time for HH:MM:SS.mmmmmm X(15)
{
	struct timeval	tv;
    struct tm		tblock;

	gettimeofday( &tv, NULL );
	localtime_r( &tv.tv_sec, &tblock);

	sprintf( strBuf, "%02d:%02d:%02d.%06d", tblock.tm_hour, tblock.tm_min, tblock.tm_sec, (int)tv.tv_usec );
}
//------------------------------------------------------------------------------------------------------------------------
Int64 RCUtility::GetHH_MM_SS_m6AndTime_us( char* strBuf ) ///< get time for HH:MM:SS.mmmmmm X(15) and return time_us()
{
	struct timeval	tv;
	struct tm		tblock;
	Int64			ret;

	gettimeofday( &tv, NULL );
	localtime_r(&tv.tv_sec, &tblock);

	sprintf( strBuf, "%02d:%02d:%02d.%06d", tblock.tm_hour, tblock.tm_min, tblock.tm_sec, (int)tv.tv_usec );

	ret = tblock.tm_hour;
	ret = ret * 60 + tblock.tm_min;
	ret = ret * 60 + tblock.tm_sec;
	ret = ret * 1000000 + tv.tv_usec;

	return ret;
}
//------------------------------------------------------------------------------------------------------------------------
void RCUtility::IntToHH_MM_SSString( Int32 Time, char* strBuf ) ///< HHMMSSm3 Int32 Time to HH::MM::SS String
{
	Int32 Hour = Time / 10000000;
	Int32 Minute = ( Time % 10000000 ) / 100000;
	Int32 Second = ( Time % 100000 ) / 10000;
        sprintf( strBuf, "%02d:%02d:%02d", Hour, Minute, Second );
}
//------------------------------------------------------------------------------------------------------------------------
int RCUtility::RightTrimLineFeed( char* strBuf )
{
	int Len = strlen( strBuf );
	int Pos = 0;
	for ( int i = Len-1; i >= 0; i-- )
	{
		if ( strBuf[i] == '\n' || strBuf[i] == 10 || strBuf[i] == 13 )
			Pos++;
		else
			break;
	}
	Len -= Pos;
	strBuf[Len] = 0;
	return Len;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCUtility::IsFileExist( const char* strPath )
{
	int fd = open( strPath, O_RDONLY, 0640 );
	if ( fd == -1 )
		return FALSE;
	close( fd );
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
// Memory Queue Functionality
//------------------------------------------------------------------------------------------------------------------------
// class MemoryQueueSubject
//------------------------------------------------------------------------------------------------------------------------
MemoryQueueSubject::MemoryQueueSubject( TMemoryQueue* MemoryQueue, MemoryQueueListener* QueueListenerObject )
: UFC::PThread( NULL )
, FMemoryQueue ( MemoryQueue )
, FQueueListenerObject( QueueListenerObject )
{
}
//------------------------------------------------------------------------------------------------------------------------
void MemoryQueueSubject::Execute( void )
{
	while ( TRUE )
	{
		// Blocking Until Message In.
		UFC::AnsiString *QueueData = FMemoryQueue->Dequeue( 0 );
		FQueueListenerObject->OnQueueDataArrived( QueueData );
		delete QueueData;
	}
}
//------------------------------------------------------------------------------------------------------------------------
void MemoryQueueSubject::Run( void )
{
	Start();
}
//------------------------------------------------------------------------------------------------------------------------
// TrashCan Functionality
//------------------------------------------------------------------------------------------------------------------------
UFC::PCriticalSection	gRCCommTrashCan_CS;
RCCommTrashCan*			gRCCommTrashCan = NULL;
//------------------------------------------------------------------------------------------------------------------------
// class RCCommTrashBase
//------------------------------------------------------------------------------------------------------------------------
RCCommTrashBase::RCCommTrashBase()
: FSecondsToday( 0 )
, FStaySeconds( 0 )
{
}
//------------------------------------------------------------------------------------------------------------------------
void RCCommTrashBase::CreateGlobalTrashCanIfNeed( void )
{
	UFC::PLockObject Lock( gRCCommTrashCan_CS );
	if ( gRCCommTrashCan == NULL )
		gRCCommTrashCan = new RCCommTrashCan( );
}
//------------------------------------------------------------------------------------------------------------------------
void RCCommTrashBase::ToTrashCan( Int32 StaySeconds )
{
	CreateGlobalTrashCanIfNeed( );

	FStaySeconds  = StaySeconds;
	FSecondsToday = UFC::GetSecondsToday( );

	UFC::PLockObject Lock( gRCCommTrashCan->FTrashContainer_CS );
	gRCCommTrashCan->FTrashContainer.push_back( this );
}
//------------------------------------------------------------------------------------------------------------------------
// class RCCommTrashCan
//------------------------------------------------------------------------------------------------------------------------
RCCommTrashCan::RCCommTrashCan( Int32 CircuitSeconds )
: UFC::PThread( NULL )
, FCircuitSeconds( CircuitSeconds )
{
	FCircuitMS = CircuitSeconds * 1000;
}
//------------------------------------------------------------------------------------------------------------------------
void RCCommTrashCan::Execute( void )
{
	int Index;
	while ( TRUE )
	{
		UFC::SleepMS( FCircuitMS ); ///< check every FCircuitMS

		UFC::PLockObject Lock( FTrashContainer_CS );
		for ( Index = FTrashContainer.size()-1; Index >= 0; Index-- )
		{
			RCCommTrashBase* TrashObject = FTrashContainer[ Index ];
			if ( UFC::GetSecondsToday() - TrashObject->FSecondsToday > TrashObject->FStaySeconds )
			{
				delete TrashObject;
				FTrashContainer.erase( FTrashContainer.begin()+Index );
			}
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
