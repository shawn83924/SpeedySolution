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
TMPTime::TMPTime( Int32 epoch, UInt16 ms )
:TMPType(ttTime) 
{
    FTime.Assign( epoch );
    Fms.Assign( ms );    
}    
//------------------------------------------------------------------------------------------------------
TMPTime::TMPTime( void )
:TMPType(ttTime) 
{
    CurrentTime();
}
//------------------------------------------------------------------------------------------------------
TMPTime::TMPTime( Int8 HH, Int8 MM, Int8 SS, Int16 ms )
:TMPType(ttTime) 
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
    Fms.Assign( ms ) ;
}
//------------------------------------------------------------------------------------------------------
// String format
// HH:MM:SS.sss
//------------------------------------------------------------------------------------------------------
TMPTime::TMPTime( const UFC::AnsiString& TimeStr ) 
:TMPType(ttTime) 
{
    FromString( TimeStr );
}
//------------------------------------------------------------------------------------------------------
TMPTime::TMPTime( UFC::PStream* Stream )
:TMPType(ttTime) 
{
    if( FTime.LoadFromStream( Stream ) != 4 )
        throw( TMPBufferException( 4 ) );
    if( Fms.LoadFromStream( Stream ) != 2 )
        throw( TMPBufferException( 2 ) );
}
//------------------------------------------------------------------------------------------------------
TMPTime::TMPTime( const TMPTime& Time )
:TMPType(ttTime) 
{
    FTime.Assign( Time.FTime.ToInt32() );
    Fms.Assign( Time.Fms.ToInt16() );
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPTime::ToBinary( UInt8* Buffer )
{
   memcpy( Buffer, FTime.GetBytes() , sizeof(Int32) ); 
   memcpy( Buffer + sizeof(Int32), Fms.GetBytes() , sizeof(Int16) );
   return 6;
}
//------------------------------------------------------------------------------------------------------
Int32 TMPTime::ToStream( UFC::PStream* Stream ) 
{
    int WriteSize = 0;
    WriteSize += FTime.SaveToStream( Stream );
    WriteSize += Fms.SaveToStream( Stream );
    return WriteSize;
}
//------------------------------------------------------------------------------------------------------
Int32 TMPTime::ToString( char* Str, UInt32 DataLength ) 
{
    char         tmpstr[ 16 ];///< HH:MM:SS.mmm
    struct tm    time;

#ifndef _WIN32    
    time_t        Epoch = FTime.ToInt32();

    localtime_r( &Epoch, &time );    
#else
    time_t  Epoch = FTime.ToInt32();

    time = *localtime( &Epoch  );
#endif
    UInt32 Len = (UInt32)sprintf( tmpstr, "%02d:%02d:%02d.%03d", time.tm_hour, time.tm_min, time.tm_sec, Fms.ToInt16() );
    if( DataLength < Len )
	throw( TMPBufferException( Len ) );
    strcpy( Str, tmpstr );
    return Len;
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPTime::ToInteger( void )
{    
    struct tm    time;
#ifndef _WIN32
    time_t       Epoch = FTime.ToInt32();

    localtime_r( &Epoch, &time );
#else    
    time_t       Epoch = FTime.ToInt32();

    time = *localtime( &Epoch  );
#endif
    return  (Int32)(time.tm_hour*10000000+time.tm_min*100000+ time.tm_sec*1000 + Fms.ToInt16() );
}    
//------------------------------------------------------------------------------------------------------        
void TMPTime::FromInteger( Int32 )
{    
    throw( TMPFieldTypeException( "Int32", "TMPTime" ) );	   
}
//------------------------------------------------------------------------------------------------------        
void TMPTime::FromString( const UFC::AnsiString& TimeStr )
{
    struct tm SetTime;

    if( TimeStr.Length() < 12 )
        throw( TMPFieldException( TimeStr.c_str() ) );	   
    if( TimeStr[2] ==':' && TimeStr[5] ==':' && TimeStr[8] =='.' ) ///< Chack format HH:MM:SS.sss
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
		Fms.Assign( (Int16)(TimeStr.SubString( 9,3 ).ToInt()) );
    }
    else
        throw( TMPFieldException( TimeStr.c_str() ) );	
} 
//------------------------------------------------------------------------------------------------------
TMPType* TMPTime::FromStream( UFC::PStream* Stream  ) 
{ 
    return new TMPTime( Stream ); 
}
//------------------------------------------------------------------------------------------------------
TMPType* TMPTime::Duplicate( void  ) 
{ 
    return new TMPTime( *this ); 
}
//------------------------------------------------------------------------------------------------------
void TMPTime::Assign( TMPType& ref )
{
    TMPTime* RefPtr = static_cast<TMPTime*>( &ref );
    if( FType != ref.GetType() )
        throw( TMPFieldTypeException( ToTMPTypeString( ref.GetType() ), ToTMPTypeString(FType) ) );
    FTime.Assign( RefPtr->FTime.ToInt32() );
    Fms.Assign( RefPtr->Fms.ToInt16() );
}
//------------------------------------------------------------------------------------------------------
TMPTime& TMPTime::operator = ( const TMPTime& ref )
{
    FTime.Assign( ref.FTime.ToInt32() );
    Fms.Assign( ref.Fms.ToInt16() );
    return *this;
}
//------------------------------------------------------------------------------------------------------
Int32 TMPTime::DiffMS( const TMPTime& PassTime )
{
    UInt64 Now  = FTime.ToInt32()*1000 + (Fms.ToInt16()%1000);
    UInt64 Pass = PassTime.FTime.ToInt32()*1000 + (PassTime.Fms.ToInt16()%1000);
    
    return (Int32)(Now - Pass);
}        
//------------------------------------------------------------------------------------------------------
void  TMPTime::CurrentTime( void )
{
    struct tm SetTime;
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    	
    localtime_r( &now.tv_sec, &SetTime ); ///< Convert to local time.        
    FTime.Assign( mktime( &SetTime ) );
    Fms.Assign( (int)(now.tv_nsec/1000000) );     
}
//------------------------------------------------------------------------------------------------------
Int32  TMPTime::GetHour( void )  const
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
Int32  TMPTime::GetMinute( void )  const
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
Int32  TMPTime::GetSecond( void )  const
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
Int32  TMPTime::GetMillisecond( void ) const
{
	return Fms.ToInt16();
}
//------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------



