#ifndef  _UDATETIME_H
#define  _UDATETIME_H
//--------------------------------------------------------------------------------------------
#include "Stream.h"
#include "AnsiString.h"
//--------------------------------------------------------------------------------------------
#include <time.h>
#include <sys/timeb.h>
//--------------------------------------------------------------------------------------------
namespace UFC
{
//--------------------------------------------------------------------------------------------
class UTime;
class UDate;
//--------------------------------------------------------------------------------------------
const int ONE_DAY = 86400;
extern UFC::AnsiString GetDateString( void );
extern UFC::AnsiString GetTradeDateYYYYMMDD( void );
tm time_gmtime( const time_t* t );
tm time_localtime( const time_t* t );
//--------------------------------------------------------------------------------------------
class UDateTime: public tm
{
protected:
  int m_ms;
  int m_us;
  static BOOL FIsGMT; 
public:
  UDateTime();
  UDateTime( const UDateTime& );
  UDateTime( int hour, int minute, int second, int millisecond = 0, int microsecond = 0 );
  UDateTime( int hour, int minute, int second, int date, int month, int year );
  UDateTime( int hour, int minute, int second, int millisecond, int date, int month, int year );
  UDateTime( const UFC::AnsiString& TimeStr );
  UDateTime( time_t time, int millisecond = 0 );
  UDateTime( const tm* time, int millisecond = 0 );
  operator tm*() 		{ return this; }
  operator const tm*() const    { return this; }
  void setMillisecond( int ms ) { m_ms = ms; }
  void setMicrosecond( int us ) { m_us = us; }
  void setSecond( int s )       { tm_sec = s; }
  void setMinute( int m )       { tm_min = m; }
  void setHour( int h )         { tm_hour = h; }
  void setDate( int d )         { tm_mday = d; }
  void setMonth( int m )        { tm_mon = m - 1; }
  void setYear( int y )         { tm_year = y - 1900; }
  void setWeekDay( int wd )     { tm_wday = wd - 1; }
  void setYearDay( int yd )     { tm_yday = yd - 1; }
  int  getMillisecond() const   { return m_ms; }
  int  getMicrosecond() const   { return m_us; }
  int  getSecond() const        { return tm_sec; }
  int  getMinute() const        { return tm_min; }
  int  getHour() const          { return tm_hour; }
  int  getDate() const          { return tm_mday; }
  int  getMonth() const         { return tm_mon + 1; }
  int  getYear() const          { return tm_year + 1900; }
  int  getWeekDay() const       { return tm_wday + 1; }
  int  getYearDay() const       { return tm_yday + 1; }
  int  getWeekOfYear();  
  void setTime( const UTime& time );
  void operator+=( long seconds );
  void operator-=( long seconds );
  void setCurrent();
  void SaveToStream( PStream* Stream, BOOL WithMS = TRUE );
  void LoadFromStream( PStream* Stream, BOOL WithMS = TRUE );
//  UDateTime&  operator =( const UDateTime& rhs );
  friend bool operator==( const UDateTime&, const UDateTime& );
  friend bool operator<( const UDateTime&, const UDateTime& );
  friend bool operator>( const UDateTime&, const UDateTime& );
  friend bool operator<=( const UDateTime&, const UDateTime& );
  friend bool operator>=( const UDateTime&, const UDateTime& );
  friend long operator-( const UDateTime&, const UDateTime& );
  static BOOL UseGMT( BOOL IsGMT );
};
///--------------------------------------------------------------------------------------------
/// Time only represented in UDateTime.
///--------------------------------------------------------------------------------------------
class UTime : public UDateTime
{
private:
  void clearDate(); ///< This class is time only represented. Call this function to clear the date parts.
public:
  /// Defaults to the current time
  UTime():UDateTime(){ clearDate(); }
  UTime( int hour, int minute, int second, int millisecond = 0 ):UDateTime( hour, minute, second, millisecond ){ clearDate(); }
  UTime( time_t time, int millisecond = 0 ):UDateTime( time, millisecond ){ clearDate(); }
  UTime( const tm* time, int millisecond = 0 ): UDateTime( time, millisecond ){ clearDate(); }
  operator tm*() 				{ return UDateTime::operator tm*(); }
  operator const tm*() const    { return UDateTime::operator const tm*(); }
  void operator+=( long seconds){ UDateTime::operator+=( seconds ); }
  void operator-=( long seconds){ UDateTime::operator-=( seconds ); }
  void setCurrent();  ///< Set to the current time.

  void setMillisecond( int ms ) { UDateTime::setMillisecond( ms ); }
  void setSecond( int s ) 		{ UDateTime::setSecond( s ); }
  void setMinute( int m ) 		{ UDateTime::setMinute( m ); }
  void setHour( int h ) 		{ UDateTime::setHour( h ); }
  int getMillisecond() const 	{ return UDateTime::getMillisecond(); }
  int getSecond() const 		{ return UDateTime::getSecond(); }
  int getMinute() const 		{ return UDateTime::getMinute(); }
  int getHour() const 			{ return UDateTime::getHour(); }

  friend bool operator== ( const UTime&, const UTime& );
  friend bool operator<  ( const UTime&, const UTime& );
  friend bool operator>  ( const UTime&, const UTime& );
  friend bool operator<= ( const UTime&, const UTime& );
  friend bool operator>= ( const UTime&, const UTime& );
};
//--------------------------------------------------------------------------------------------
/// Date only represented in UDateTime.
//--------------------------------------------------------------------------------------------
class UDate : public UDateTime
{
private:
  void clearTime(); ///< This class is date only represented. Call this function to clear the Time parts.
public:
  /// Defaults to the current date
  UDate() : UDateTime(){ clearTime(); }
  UDate( int date, int month, int year ):UDateTime( 0, 0, 0, date, month, year ){ clearTime();}
  UDate( long sec ):UDateTime( sec, 0 ) { clearTime(); }
  UDate( const tm* time ):UDateTime( time, 0 ) { clearTime(); }
  operator tm*() { return UDateTime::operator tm*(); }
  operator const tm*() const { return UDateTime::operator const tm*(); }
  void operator+=( int days){ UDateTime::operator+=( ONE_DAY * days ); }
  void operator-=( int days){ UDateTime::operator-=( ONE_DAY * days ); }
  void setCurrent(); ///< Set to the current time.
  void setDate( int d )  { UDateTime::setDate( d ); }
  void setMonth( int m ) { UDateTime::setMonth( m ); }
  void setYear( int y )  { UDateTime::setYear( y ); }
  int getDate() const    { return UDateTime::getDate(); }
  int getMonth() const   { return UDateTime::getMonth(); }
  int getYear() const    { return UDateTime::getYear(); }
  int getWeekDay() const { return UDateTime::getWeekDay(); }
  int getYearDay() const { return UDateTime::getYearDay(); }

  friend bool operator==( const UDate&, const UDate& );
  friend bool operator<( const UDate&, const UDate& );
  friend bool operator>( const UDate&, const UDate& );
  friend bool operator<=( const UDate&, const UDate& );
  friend bool operator>=( const UDate&, const UDate& );
  friend long operator-( const UDate&, const UDate& );
};
//--------------------------------------------------------------------------------------------
}
//--------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------
