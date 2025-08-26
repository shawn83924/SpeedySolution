#include "TMPTypes.h"
#ifdef _WIN32
	#include <stdio.h>
	#include <sys/timeb.h>
	#include <time.h>
#endif
//-----------------------------------------------------------------------------------------
namespace TMP
{
//------------------------------------------------------------------------------------------------------
// Class TMPTime
// Implement Time class for TMP protocol.
//------------------------------------------------------------------------------------------------------
TMPTimeEx::TMPTimeEx( Int32 epoch, UInt32 ns )
:TMPType(ttTimeEx) 
{
    FTime.Assign( epoch );
    Fns.Assign( ns );    
}    
//------------------------------------------------------------------------------------------------------
TMPTimeEx::TMPTimeEx( void )
:TMPType(ttTimeEx) 
{
    CurrentTime();
}
//------------------------------------------------------------------------------------------------------
TMPTimeEx::TMPTimeEx( Int8 HH, Int8 MM, Int8 SS, UInt32 ns )
:TMPType(ttTimeEx) 
{
    struct tm SetTime;
#ifndef _WIN32
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    	
    localtime_r( &now.tv_sec, &SetTime ); ///< Convert to local time.    
#else
    struct timeb tb;

    ftime(  &tb );
    SetTime = *localtime( &tb.time );
#endif
    SetTime.tm_hour = HH;
    SetTime.tm_min  = MM;
    SetTime.tm_sec  = SS;
    FTime.Assign( mktime( &SetTime ) );
    Fns.Assign( ns ) ;
}
//------------------------------------------------------------------------------------------------------
// String format
// HH:MM:SS.sssssssss
//------------------------------------------------------------------------------------------------------
TMPTimeEx::TMPTimeEx( const UFC::AnsiString& TimeStr ) 
:TMPType(ttTimeEx) 
{
    FromString( TimeStr );
}
//------------------------------------------------------------------------------------------------------
TMPTimeEx::TMPTimeEx( UFC::PStream* Stream )
:TMPType(ttTimeEx) 
{
    if( FTime.LoadFromStream( Stream ) != 4 )
        throw( TMPBufferException( 4 ) );
    if( Fns.LoadFromStream( Stream ) != 4 )
        throw( TMPBufferException( 4 ) );
}
//------------------------------------------------------------------------------------------------------
TMPTimeEx::TMPTimeEx( const TMPTimeEx& Time )
:TMPType(ttTimeEx) 
{
    FTime.Assign( Time.FTime.ToInt32() );
    Fns.Assign( Time.Fns.ToInt32() );
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPTimeEx::ToBinary( UInt8* Buffer )
{
   memcpy( Buffer, FTime.GetBytes() , sizeof(Int32) ); 
   memcpy( Buffer + sizeof(Int32), Fns.GetBytes() , sizeof(Int32) );
   return 8;
}
//------------------------------------------------------------------------------------------------------
Int32 TMPTimeEx::ToStream( UFC::PStream* Stream ) 
{
    int WriteSize = 0;
    WriteSize += FTime.SaveToStream( Stream );
    WriteSize += Fns.SaveToStream( Stream );
    return WriteSize;
}
//------------------------------------------------------------------------------------------------------
Int32 TMPTimeEx::ToString( char* Str, UInt32 DataLength ) 
{
    char         tmpstr[ 24 ];///< HH:MM:SS.nnnnnnnnn
    struct tm    time;

#ifndef _WIN32    
    time_t        Epoch = FTime.ToInt32();

    localtime_r( &Epoch, &time );    
#else
    time_t  Epoch = FTime.ToInt32();

    time = *localtime( &Epoch  );
#endif
    UInt32 Len = (UInt32)sprintf( tmpstr, "%02d:%02d:%02d.%09d", time.tm_hour, time.tm_min, time.tm_sec, Fns.ToInt32() );
    if( DataLength < Len )
	throw( TMPBufferException( Len ) );
    strcpy( Str, tmpstr );
    return Len;
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPTimeEx::ToInteger( void )
{    
    struct tm    time;
#ifndef _WIN32
    time_t       Epoch = FTime.ToInt32();

    localtime_r( &Epoch, &time );
#else    
    time_t       Epoch = FTime.ToInt32();

    time = *localtime( &Epoch  );
#endif
    return  (Int32)(time.tm_hour*10000000+time.tm_min*100000+ time.tm_sec*1000 + Fns.ToInt32()/1000000 );
}    
//------------------------------------------------------------------------------------------------------        
Int64 TMPTimeEx::ToInteger64( void )
{   
    struct tm    time;
#ifndef _WIN32
    time_t       Epoch = FTime.ToInt32();

    localtime_r( &Epoch, &time );
#else    
    time_t       Epoch = FTime.ToInt32();

    time = *localtime( &Epoch  );
#endif
    return ((((Int64)time.tm_hour*100+time.tm_min)*100+time.tm_sec)*1000000)+Fns.ToInt32();
}    
//------------------------------------------------------------------------------------------------------        
void TMPTimeEx::FromInteger( Int32 )
{    
    throw( TMPFieldTypeException( "Int32", "TMPTime" ) );	   
}
//------------------------------------------------------------------------------------------------------        
void TMPTimeEx::FromString( const UFC::AnsiString& TimeStr )
{
    struct tm SetTime;

    if( TimeStr.Length() < 12 )
        throw( TMPFieldException( TimeStr.c_str() ) );	   
    if( TimeStr[2] ==':' && TimeStr[5] ==':' && TimeStr[8] =='.' ) ///< Chack format HH:MM:SS.nnnnnnnnn
    {
	#ifndef _WIN32	    
            struct timespec now;
    
            clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    	
            localtime_r( &now.tv_sec, &SetTime ); ///< Convert to local time.
	#else
            struct timeb tb;

            ftime(  &tb );
            SetTime = *localtime( &tb.time );
	#endif
        SetTime.tm_hour = TimeStr.SubString( 0,2).ToInt();
        SetTime.tm_min  = TimeStr.SubString( 3,2).ToInt();
        SetTime.tm_sec  = TimeStr.SubString( 6,2).ToInt();
        FTime.Assign( mktime( &SetTime ) );
        Fns.Assign( (Int32)(TimeStr.SubString( 9,9 ).ToInt()) );
    }
    else
        throw( TMPFieldException( TimeStr.c_str() ) );	
} 
//------------------------------------------------------------------------------------------------------
TMPType* TMPTimeEx::FromStream( UFC::PStream* Stream  ) 
{ 
    return new TMPTimeEx( Stream ); 
}
//------------------------------------------------------------------------------------------------------
TMPType* TMPTimeEx::Duplicate( void  ) 
{ 
    return new TMPTimeEx( *this ); 
}
//------------------------------------------------------------------------------------------------------
void TMPTimeEx::Assign( TMPType& ref )
{
    TMPTimeEx* RefPtr = static_cast<TMPTimeEx*>( &ref );
    if( FType != ref.GetType() )
        throw( TMPFieldTypeException( ToTMPTypeString( ref.GetType() ), ToTMPTypeString(FType) ) );
    FTime.Assign( RefPtr->FTime.ToInt32() );
    Fns.Assign( RefPtr->Fns.ToInt32() );
}
//------------------------------------------------------------------------------------------------------
TMPTimeEx& TMPTimeEx::operator = ( const TMPTimeEx& ref )
{
    FTime.Assign( ref.FTime.ToInt32() );
    Fns.Assign( ref.Fns.ToInt32() );
    return *this;
}
//------------------------------------------------------------------------------------------------------
Int64 TMPTimeEx::DiffNS( const TMPTimeEx& PassTime )
{
    UInt64 Now  = FTime.ToInt32()*1000000000 + (Fns.ToInt32()%1000000000);
    UInt64 Pass = PassTime.FTime.ToInt32()*1000000000 + (PassTime.Fns.ToInt32()%1000000000);
    
    return (Int64)(Now - Pass);
}        
//------------------------------------------------------------------------------------------------------
void  TMPTimeEx::CurrentTime( void )
{
    struct tm SetTime;
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    	
    localtime_r( &now.tv_sec, &SetTime ); ///< Convert to local time.        
    FTime.Assign( mktime( &SetTime ) );
    Fns.Assign( now.tv_nsec ) ;    
    
}
//------------------------------------------------------------------------------------------------------
Int32  TMPTimeEx::GetHour( void )  const
{
    struct tm    time;

#ifndef _WIN32
    time_t        Epoch = FTime.ToInt32();

    localtime_r( &Epoch, &time );
#else
	time_t        Epoch = FTime.ToInt32();

	time = *localtime( &Epoch  );
#endif
	return time.tm_hour;
}
//------------------------------------------------------------------------------------------------------
Int32  TMPTimeEx::GetMinute( void )  const
{
struct tm    time;

#ifndef _WIN32
	time_t        Epoch = FTime.ToInt32();

	localtime_r( &Epoch, &time );
#else
	time_t  Epoch = FTime.ToInt32();

	time = *localtime( &Epoch  );
#endif
    return time.tm_min;
}
//------------------------------------------------------------------------------------------------------
Int32  TMPTimeEx::GetSecond( void )  const
{
    struct tm    time;

#ifndef _WIN32
    time_t        Epoch = FTime.ToInt32();

    localtime_r( &Epoch, &time );
#else
	time_t  Epoch = FTime.ToInt32();

	time = *localtime( &Epoch  );
#endif
	return time.tm_sec;
}
//------------------------------------------------------------------------------------------------------
UInt32  TMPTimeEx::GetNanoSecond( void ) const
{
	return Fns.ToInt32();
}
//------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------



