#include "TMPTypes.h"
#include <limits.h>
//-----------------------------------------------------------------------------------------
namespace TMP
{
//------------------------------------------------------------------------------------------------------    
// Class TMPInt64
// Implement Int64 class for TMP protocol.
//------------------------------------------------------------------------------------------------------
TMPInt64::TMPInt64( const UFC::AnsiString& Str  )
:TMPType(ttInt64) 
{    
    FromString( Str );
}
//------------------------------------------------------------------------------------------------------
TMPInt64::TMPInt64( UFC::PStream* Stream )
:TMPType(ttInt64) 
{
    if( FData.LoadFromStream( Stream ) != 8 )
        throw( TMPBufferException( 8 ) );
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPInt64::ToBinary( UInt8* Buffer )
{
    memcpy( Buffer, FData.GetBytes(), sizeof( Int64 ) );
    return sizeof( Int64 );
}        
//------------------------------------------------------------------------------------------------------
Int32 TMPInt64::ToStream( UFC::PStream* Stream ) 
{
    return FData.SaveToStream( Stream );
}
//------------------------------------------------------------------------------------------------------
Int32 TMPInt64::ToString( char* Str, UInt32 DataLength ) 
{
    char tmpstr[24];///< -9,223,372,036,854,775,808~9,223,372,036,854,775,807
    
    sprintf( tmpstr, "%lld", FData.ToInt64() );
    if( DataLength < strlen(tmpstr) )
        throw( TMPBufferException( (Int32)strlen(tmpstr) ) );
    strcpy( Str, tmpstr );
    return (Int32)strlen(tmpstr);
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPInt64::ToInteger( void )
{
    Int64 Val = (Int64)FData;
    if( Val < INT_MIN || Val > UINT_MAX )
    {
        char tmpstr[64];
    
        sprintf( tmpstr, "%lld", FData.ToInt64() );
        throw( TMPFieldException( tmpstr ) );	   
    }
    return (Int32)Val;
}
//------------------------------------------------------------------------------------------------------        
void TMPInt64::FromInteger( Int32 Val ) 
{    
    FData.Assign( (Int64)Val );
}
//------------------------------------------------------------------------------------------------------        
void TMPInt64::FromString( const UFC::AnsiString& Str )
{
    if( Str.Length() == 0 )
        throw( TMPFieldException( "NULL Int64 string" ) );	                   
    FData.Assign( Str.ToInt64());
} 
//------------------------------------------------------------------------------------------------------
TMPType* TMPInt64::FromStream( UFC::PStream* Stream  ) 
{ 
    return new TMPInt64( Stream ); 
}
//------------------------------------------------------------------------------------------------------
TMPType* TMPInt64::Duplicate( void  ) 
{ 
    return new TMPInt64( FData ); 
}
//------------------------------------------------------------------------------------------------------
void TMPInt64::Assign( TMPType& ref )
{
    TMPInt64* RefPtr = static_cast<TMPInt64*>( &ref );
    if( FType != ref.GetType() )
        throw( TMPFieldTypeException( ToTMPTypeString( ref.GetType() ), ToTMPTypeString(FType) ) );
    FData = RefPtr->FData.ToInt64();
}
//------------------------------------------------------------------------------------------------------
TMPInt64& TMPInt64::operator = ( Int64 Val ) 
{
    FData.Assign( Val );
    return *this;
}
//------------------------------------------------------------------------------------------------------
TMPInt64& TMPInt64::operator = ( const TMPInt64& ref )
{
    FData.Assign( ref.FData.ToInt64() );
    return *this;
}
//------------------------------------------------------------------------------------------------------    
// Class TMPUInt64
// Implement UInt64 class for TMP protocol.
//------------------------------------------------------------------------------------------------------
TMPUInt64::TMPUInt64( const UFC::AnsiString& Str  )
:TMPType(ttUInt64) 
{    
    FromString( Str );
}
//------------------------------------------------------------------------------------------------------
TMPUInt64::TMPUInt64( UFC::PStream* Stream )
:TMPType(ttUInt64) 
{
    if( FData.LoadFromStream( Stream ) != 8 )
        throw( TMPBufferException( 8 ) );
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPUInt64::ToBinary( UInt8* Buffer )
{
    memcpy( Buffer, FData.GetBytes(), sizeof( UInt64 ) );
    return sizeof( UInt64 );
}        
//------------------------------------------------------------------------------------------------------
Int32 TMPUInt64::ToStream( UFC::PStream* Stream ) 
{
    return FData.SaveToStream( Stream );
}
//------------------------------------------------------------------------------------------------------
Int32 TMPUInt64::ToString( char* Str, UInt32 DataLength ) 
{
    char tmpstr[24];///< -9,223,372,036,854,775,808~9,223,372,036,854,775,807
    
    sprintf( tmpstr, "%llu", (UInt64)FData.ToInt64() );
    if( DataLength < strlen(tmpstr) )
        throw( TMPBufferException( (Int32)strlen(tmpstr) ) );
    strcpy( Str, tmpstr );
    return (Int32)strlen(tmpstr);
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPUInt64::ToInteger( void )
{
    UInt64 Val = (UInt64)(Int64)FData;
#ifdef __AIX    
    if( Val > ULONGLONG_MAX )
#else
    if( Val > ULONG_LONG_MAX )
#endif        
    {
        char tmpstr[64];
    
        sprintf( tmpstr, "%llu", (UInt64)FData.ToInt64() );
        throw( TMPFieldException( tmpstr ) );	   
    }
    return (Int32)Val;
}
//------------------------------------------------------------------------------------------------------        
void TMPUInt64::FromInteger( Int32 Val ) 
{    
    FData.Assign( (UInt64)Val );
}
//------------------------------------------------------------------------------------------------------        
void TMPUInt64::FromString( const UFC::AnsiString& Str )
{
    if( Str.Length() == 0 )
        throw( TMPFieldException( "NULL UInt64 string" ) );	                   
    FData.Assign( Str.ToInt64());
} 
//------------------------------------------------------------------------------------------------------
TMPType* TMPUInt64::FromStream( UFC::PStream* Stream  ) 
{ 
    return new TMPUInt64( Stream ); 
}
//------------------------------------------------------------------------------------------------------
TMPType* TMPUInt64::Duplicate( void  ) 
{ 
    return new TMPUInt64( FData ); 
}
//------------------------------------------------------------------------------------------------------
void TMPUInt64::Assign( TMPType& ref )
{
    TMPUInt64* RefPtr = static_cast<TMPUInt64*>( &ref );
    if( FType != ref.GetType() )
        throw( TMPFieldTypeException( ToTMPTypeString( ref.GetType() ), ToTMPTypeString(FType) ) );
    FData = RefPtr->FData.ToInt64();
}
//------------------------------------------------------------------------------------------------------
TMPUInt64& TMPUInt64::operator = ( UInt64 Val ) 
{
    FData.Assign( (Int64)Val );
    return *this;
}
//------------------------------------------------------------------------------------------------------
TMPUInt64& TMPUInt64::operator = ( const TMPUInt64& ref )
{
    FData.Assign( ref.FData.ToInt64() );
    return *this;
}
//------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------



