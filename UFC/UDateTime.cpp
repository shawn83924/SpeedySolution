#include "UDateTime.h"
#include "NetUtility.h"

namespace UFC
{
//--------------------------------------------------------------------------------------------
BOOL UDateTime::FIsGMT = FALSE;
//--------------------------------------------------------------------------------------------
#ifdef _WIN32
//--------------------------------------------------------------------------------------------
tm time_gmtime( const time_t* t ) ///< return the GMT time
{
	tm result;

#ifdef _MSC_VER
	if( gmtime_s( &result, t ) != 0 )
#else
    if( gmtime_s( t, &result ) == NULL )
#endif
	{
		time_t tTime;

		time( &tTime );
#ifdef _MSC_VER
        gmtime_s( &result, &tTime );
#else
		gmtime_s( &tTime, &result );
#endif
	}
	return result;
}
//--------------------------------------------------------------------------------------------
tm time_localtime( const time_t* t) ///< return the local time
{
	tm result;

#ifdef _MSC_VER
    if( localtime_s( &result, t ) != 0 )
#else
	if( localtime_s( t, &result ) == NULL )
#endif
	{
		time_t tTime;

		time( &tTime );
#ifdef _MSC_VER
        localtime_s( &result, &tTime );
#else
		localtime_s( &tTime, &result );
#endif
	}
	return result;
}
//--------------------------------------------------------------------------------------------
#else
//--------------------------------------------------------------------------------------------
tm time_gmtime( const time_t* t )
{
    tm result;

    if( gmtime_r( t, &result ) == NULL )
    {
        time_t tTime;

        time( &tTime );
        gmtime_r( &tTime, &result );
    }
    return result;
}
//--------------------------------------------------------------------------------------------
tm time_localtime( const time_t* t)
{
    tm result;

    if( localtime_r( t, &result ) == NULL )
    {
        time_t tTime;

        time( &tTime );
        localtime_r( &tTime, &result );
    }
    return result;
}
//--------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString GetDateString( void )
{
    AnsiString RtnStr;
    struct tm  TM;
    time_t tTime;
    int y,m,d;

    time( &tTime );
    TM = time_localtime( &tTime );
    y  = TM.tm_year - 11;
    m  = TM.tm_mon + 1;
    d  = TM.tm_mday;
    if( y < 100 )
            RtnStr.Printf("%02d%02d%02d",y,m,d);
    else
            RtnStr.Printf("%03d%02d%02d",y,m,d);
    return RtnStr;
}
///--------------------------------------------------------------------------------------------
BOOL UDateTime::UseGMT( BOOL IsGMT )
{
    BOOL OldVal = FIsGMT;
    FIsGMT = IsGMT;
    return OldVal;
}
///--------------------------------------------------------------------------------------------
/// Defaults to the current date and time
///--------------------------------------------------------------------------------------------
UDateTime::UDateTime()
:m_ms( 0 )
,m_us( 0 )
{
    setCurrent();
}
///--------------------------------------------------------------------------------------------
UDateTime::UDateTime( const UDateTime& rhs )
{
    *(static_cast<tm*>(this)) = *(static_cast<const tm*>(&rhs));
    m_ms = rhs.m_ms;
    m_us = rhs.m_us;    
}
///--------------------------------------------------------------------------------------------
/// Defaults to the current date
///--------------------------------------------------------------------------------------------
UDateTime::UDateTime( int hour, int minute, int second, int millisecond, int microsecond )
:m_ms( millisecond )
,m_us( microsecond )
{
    setCurrent();
    setHour( hour );
    setMinute( minute );
    setSecond( second );
    setMillisecond( millisecond );
    setMicrosecond( microsecond );
}
//--------------------------------------------------------------------------------------------
UDateTime::UDateTime( int hour, int minute, int second, int date, int month, int year )
:m_ms( 0 )
,m_us( 0 )
{
    setCurrent();
    setHour( hour );
    setMinute( minute );
    setSecond( second );
    setDate( date );
    setMonth( month );
    setYear( year );
    time_t t = mktime( (tm*)this );
    *((tm*)this) = time_localtime( &t );
    tm_isdst = -1;
}
//--------------------------------------------------------------------------------------------
UDateTime::UDateTime( int hour, int minute, int second, int millisecond, int date, int month, int year )
:m_ms( millisecond )
,m_us( 0 )
{
    setCurrent();
    setHour( hour );
    setMinute( minute );
    setSecond( second );
    setDate( date );
    setMonth( month );
    setYear( year );
    time_t t = mktime( (tm*)this );
    *((tm*)this) = time_localtime( &t );
    tm_isdst = -1;
}
///--------------------------------------------------------------------------------------------
UDateTime::UDateTime( const UFC::AnsiString& Time )
:m_us( 0 )
{
    int hour,minute,second;
    int millis = 0;
    int len = Time.Length();
    bool KnownFormat = true;

    setCurrent();
    if( len == 4 ) ///< format HHMM
    {
        hour   = UFC::AnsiString::StrToInt32( Time.c_str() , 2 );
        minute = UFC::AnsiString::StrToInt32( Time.c_str() + 2, 2 );
        second = 0;
    }
    else if( len == 5 ) ///< format HH:MM
    {
        hour   = UFC::AnsiString::StrToInt32( Time.c_str() , 2 );
        minute = UFC::AnsiString::StrToInt32( Time.c_str() + 3, 2 );
        second = 0;
    }
    else if( len == 6 ) ///< format HHMMSS
    {
        hour   = UFC::AnsiString::StrToInt32( Time.c_str() , 2 );
        minute = UFC::AnsiString::StrToInt32( Time.c_str() + 2, 2 );
        second = UFC::AnsiString::StrToInt32( Time.c_str() + 4, 2 );
    }
    else if( len == 8 )
    {
        if( Time[2] == ':' )  ///< format HH:MM:SS
        {
            hour   = UFC::AnsiString::StrToInt32( Time.c_str() , 2 );
            minute = UFC::AnsiString::StrToInt32( Time.c_str() + 3, 2 );
            second = UFC::AnsiString::StrToInt32( Time.c_str() + 6, 2 );
        }
        else   ///< format HHMMSSmm
        {
            hour   = UFC::AnsiString::StrToInt32( Time.c_str() , 2 );
            minute = UFC::AnsiString::StrToInt32( Time.c_str() + 2, 2 );
            second = UFC::AnsiString::StrToInt32( Time.c_str() + 4, 2 );
            millis = UFC::AnsiString::StrToInt32( Time.c_str() + 6, 2 )*10;
        }
    }
    else if( len == 9 ) ///< format HHMMSSmmm
    {
        hour   = UFC::AnsiString::StrToInt32( Time.c_str() , 2 );
        minute = UFC::AnsiString::StrToInt32( Time.c_str() + 2, 2 );
        second = UFC::AnsiString::StrToInt32( Time.c_str() + 4, 2 );
        millis = UFC::AnsiString::StrToInt32( Time.c_str() + 6, 3 );
    }
    else if( len == 12 ) ///< format HH:MM:SS.mmm
    {
        hour   = UFC::AnsiString::StrToInt32( Time.c_str() , 2 );
        minute = UFC::AnsiString::StrToInt32( Time.c_str() + 3, 2 );
        second = UFC::AnsiString::StrToInt32( Time.c_str() + 6, 2 );
        millis = UFC::AnsiString::StrToInt32( Time.c_str() + 9, 3 );
    }
    else
        KnownFormat = false;
    if( KnownFormat == true )
    {
        setHour( hour );
        setMinute( minute );
        setSecond( second );
        setMillisecond( millis );
        setMicrosecond( 0 );
    }
}
//--------------------------------------------------------------------------------------------
UDateTime::UDateTime( time_t time, int millisecond )
:m_us( 0 )
{
    if( FIsGMT == TRUE )
        *((tm*)this) = time_gmtime( &time );
    else
        *((tm*)this) = time_localtime( &time );
    setMillisecond(millisecond);
}
//--------------------------------------------------------------------------------------------
UDateTime::UDateTime( const tm* time, int millisecond )
:m_us( 0 )
{
    *((tm*)this) = *time;
    setMillisecond(millisecond);
}
//--------------------------------------------------------------------------------------------
void UDateTime::setCurrent()
{
#ifdef _WIN32    
    
    timeb tb;
    
    ::ftime( &tb );
    time_t time = (time_t)tb.time;
    m_ms = tb.millitm;
    m_us = 0;
#else
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.        
    time_t time = (time_t)now.tv_sec;
    int us = (int)now.tv_nsec/1000;
    m_ms = us/1000;
    m_us = us%1000;
#endif    
    if( FIsGMT == TRUE )
        *((tm*)this) = time_gmtime( &time );
    else
        *((tm*)this) = time_localtime( &time );
    tm_isdst = -1;
}
//--------------------------------------------------------------------------------------------
void UDateTime::setTime( const UTime& time )
{
    setHour( time.getHour() );
    setMinute( time.getMinute() );
    setSecond( time.getSecond() );
    setMillisecond( time.getMillisecond() );
    setMicrosecond( time.getMicrosecond() );
}
//--------------------------------------------------------------------------------------------
int  UDateTime::getWeekOfYear( void )
{
    char buff[ 32 ];
    
    if( strftime( buff, 32, "%W", (tm*)this )  != 0 )
        return atoi( buff );
    return 0;
}
//---------------------------------------------------------------------------
/*UDateTime& UDateTime::operator =( const UDateTime& rhs )
{
    if( this != &rhs )
    {
        this->tm_sec   = rhs.tm_sec;	
        this->tm_min    = rhs.tm_min;	
        this->tm_hour   = rhs.tm_hour;	
        this->tm_mday   = rhs.tm_mday;	
        this->tm_mon    = rhs.tm_mon;	
        this->tm_year   = rhs.tm_year;	
        this->tm_wday   = rhs.tm_wday;	
        this->tm_yday   = rhs.tm_yday;	
        this->tm_isdst  = rhs.tm_isdst;	
        this->tm_gmtoff = rhs.tm_gmtoff;
        this->tm_zone   = rhs.tm_zone;
        this->m_ms = rhs.m_ms;
        this->m_us = rhs.m_us;
    }
    return *this;
}*/
//--------------------------------------------------------------------------------------------
void UDateTime::operator+=( long seconds )
{
    tm copy = *this;
    time_t time = mktime( const_cast < tm* > ( &copy ) );
    time += seconds;
    *static_cast < tm* > ( this ) = time_localtime( &time );
    tm_isdst = -1;
}
//--------------------------------------------------------------------------------------------
void UDateTime::operator-=( long seconds )
{
    tm copy = *this;
    time_t time = mktime( const_cast < tm* > ( &copy ) );
    time -= seconds;
    *static_cast < tm* > ( this ) = time_localtime( &time );
    tm_isdst = -1;
}
//--------------------------------------------------------------------------------------------
void UDateTime::SaveToStream( PStream* Stream, BOOL WithMS )
{
    NInt32 TimeSec = mktime( this );

    TimeSec.SaveToStream( Stream );
    if( WithMS == TRUE )
    {
        NInt32 US = m_ms*1000 + m_us;        
        US.SaveToStream( Stream );
    }
}
//--------------------------------------------------------------------------------------------
void UDateTime::LoadFromStream( PStream* Stream, BOOL WithMS )
{
    NInt32 TimeSec,US;
    
    TimeSec.LoadFromStream( Stream );    
    time_t timesec = TimeSec.ToInt32();        
    *((tm*)this) = time_localtime( &timesec  );    
    if( WithMS == TRUE )
    {
        US.LoadFromStream( Stream );
        m_ms = US.ToInt32()/1000;
        m_us = US.ToInt32()%1000;
    }
}
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void UTime::setCurrent()
{
    UDateTime::setCurrent();
    clearDate();
}
//--------------------------------------------------------------------------------------------
void UTime::clearDate()
{
    tm_mday = 1;
    tm_mon = 0;
    tm_year = 71;
    tm_wday = 0;
    tm_yday = 0;
    tm_isdst = -1;
    m_ms = 0;
    m_us = 0;
}
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void UDate::setCurrent()
{
    UDateTime::setCurrent();
    clearTime();
}
//--------------------------------------------------------------------------------------------
void UDate::clearTime()
{
    setHour( 0 );
    setMinute( 0 );
    setSecond( 0 );
    setMillisecond( 0 );
    setMicrosecond( 0 );
}
//--------------------------------------------------------------------------------------------
// inline operator functions.
//--------------------------------------------------------------------------------------------
bool operator==( const UDateTime& lhs, const UDateTime& rhs )
{
  return
    lhs.m_us == rhs.m_us          
    && lhs.m_ms == rhs.m_ms
    && lhs.tm_sec == rhs.tm_sec
    && lhs.tm_min == rhs.tm_min
    && lhs.tm_hour == rhs.tm_hour
    && lhs.tm_mday == rhs.tm_mday
    && lhs.tm_mon == rhs.tm_mon
    && lhs.tm_year == rhs.tm_year
    && lhs.tm_wday == rhs.tm_wday
    && lhs.tm_yday == rhs.tm_yday;
}
//--------------------------------------------------------------------------------------------
bool operator<( const UDateTime& lhs, const UDateTime& rhs )
{
  tm l_copy = *(const tm*)lhs; tm r_copy = *(const tm*)rhs;
  time_t l_time = mktime( &l_copy );
  time_t r_time = mktime( &r_copy );
  double diff = difftime( l_time, r_time );

  if(diff==0.0)
     return (lhs.m_ms - rhs.m_ms) < 0;
  else
     return diff < 0;
}
//--------------------------------------------------------------------------------------------
bool operator>( const UDateTime& lhs, const UDateTime& rhs )
{
    return !( lhs <= rhs );
}
//--------------------------------------------------------------------------------------------
bool operator<=( const UDateTime& lhs, const UDateTime& rhs )
{
    return ( lhs == rhs ) || ( lhs < rhs );
}
//--------------------------------------------------------------------------------------------
bool operator>=( const UDateTime& lhs, const UDateTime& rhs )
{
    return ( lhs == rhs ) || ( lhs > rhs );
}
//--------------------------------------------------------------------------------------------
long operator-( const UDateTime& lhs, const UDateTime& rhs )
{
    tm l_copy = *(const tm*)lhs; tm r_copy = *(const tm*)rhs;
    time_t l_time = mktime( &l_copy );
    time_t r_time = mktime( &r_copy );
    return l_time - r_time;
}
//--------------------------------------------------------------------------------------------
bool operator==( const UTime& lhs, const UTime& rhs )
{
    return
    lhs.m_us == rhs.m_us
    && lhs.m_ms == rhs.m_ms
    && lhs.tm_sec == rhs.tm_sec
    && lhs.tm_min == rhs.tm_min
    && lhs.tm_hour == rhs.tm_hour;
}
//--------------------------------------------------------------------------------------------
bool operator<( const UTime& lhs, const UTime& rhs )
{
    if ( lhs.tm_hour < rhs.tm_hour ) return true;
    if ( lhs.tm_hour > rhs.tm_hour ) return false;
    if ( lhs.tm_min < rhs.tm_min ) return true;
    if ( lhs.tm_min > rhs.tm_min ) return false;
    if ( lhs.tm_sec < rhs.tm_sec ) return true;
    if ( lhs.tm_sec > rhs.tm_sec ) return false;
    if ( lhs.m_ms < rhs.m_ms ) return true;

    return false;
}
//--------------------------------------------------------------------------------------------
bool operator>( const UTime& lhs, const UTime& rhs )
{
    return !( lhs <= rhs );
}
//--------------------------------------------------------------------------------------------
bool operator<=( const UTime& lhs, const UTime& rhs )
{
    return ( lhs == rhs ) || ( lhs < rhs );
}
//--------------------------------------------------------------------------------------------
bool operator>=( const UTime& lhs, const UTime& rhs )
{
    return ( lhs == rhs ) || ( lhs > rhs );
}
//--------------------------------------------------------------------------------------------
bool operator==( const UDate& lhs, const UDate& rhs )
{
    return
    lhs.tm_mday == rhs.tm_mday
    && lhs.tm_mon == rhs.tm_mon
    && lhs.tm_year == rhs.tm_year;

}
//--------------------------------------------------------------------------------------------
bool operator<( const UDate& lhs, const UDate& rhs )
{
    if ( lhs.tm_year < rhs.tm_year ) return true;
    if ( lhs.tm_year > rhs.tm_year ) return false;
    if ( lhs.tm_mon < rhs.tm_mon ) return true;
    if ( lhs.tm_mon > rhs.tm_mon ) return false;
    if ( lhs.tm_mday < rhs.tm_mday ) return true;
    return false;
}
//--------------------------------------------------------------------------------------------
bool operator>( const UDate& lhs, const UDate& rhs )
{
    return !( lhs <= rhs );
}
//--------------------------------------------------------------------------------------------
bool operator<=( const UDate& lhs, const UDate& rhs )
{
    return ( lhs == rhs ) || ( lhs < rhs );
}
//--------------------------------------------------------------------------------------------
bool operator>=( const UDate& lhs, const UDate& rhs )
{
    return ( lhs == rhs ) || ( lhs > rhs );
}
//--------------------------------------------------------------------------------------------
long operator-( const UDate& lhs, const UDate& rhs )
{
    tm l_copy = *(const tm*)lhs; tm r_copy = *(const tm*)rhs;
    time_t l_time = mktime( &l_copy );
    time_t r_time = mktime( &r_copy );
    return ( l_time - r_time ) / ONE_DAY;
}
//--------------------------------------------------------------------------------------------
}
