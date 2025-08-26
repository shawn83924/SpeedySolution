#include "TMPTypes.h"
//-----------------------------------------------------------------------------------------
namespace TMP
{
//------------------------------------------------------------------------------------------------------    
// Class TMPInt32
// Implement Int32 class for TMP protocol.
//------------------------------------------------------------------------------------------------------    
TMPInt32::TMPInt32( void )
:TMPType(ttInt32)
,FData( 0 )
{
}
//------------------------------------------------------------------------------------------------------    
TMPInt32::TMPInt32( Int32 Val )
:TMPType(ttInt32)
,FData( Val )
{
}
//------------------------------------------------------------------------------------------------------    
TMPInt32::TMPInt32( const UFC::AnsiString& Str )
:TMPType(ttInt32)
{
    FromString( Str );
}
//------------------------------------------------------------------------------------------------------    
TMPInt32::TMPInt32( UFC::PStream* Stream )
:TMPType(ttInt32) 
{
    if( FData.LoadFromStream( Stream ) !=  4 )
        throw( TMPBufferException( 4 ) );
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPInt32::ToBinary( UInt8* Buffer )
{
    memcpy( Buffer, FData.GetBytes(), sizeof( Int32 ) );
    return sizeof( Int32 ); 
}        
//------------------------------------------------------------------------------------------------------    
Int32 TMPInt32::ToStream( UFC::PStream* Stream ) 
{
    return FData.SaveToStream( Stream );
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPInt32::ToString( char* Str, UInt32 DataLength ) 
{
    char tmpstr[16];///< -2,147,483,648~2,147,483,648
    
    sprintf( tmpstr, "%d", FData.ToInt32() );
    if( DataLength < strlen(tmpstr) )
        throw( TMPBufferException( (Int32)strlen(tmpstr) ) );
    strcpy( Str, tmpstr );
    return (Int32)strlen(tmpstr);
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPInt32::ToInteger( void )
{
    return (Int32)FData;
}
//------------------------------------------------------------------------------------------------------        
void TMPInt32::FromInteger( Int32 Val ) 
{    
    FData.Assign( Val );
}
//------------------------------------------------------------------------------------------------------        
void TMPInt32::FromString( const UFC::AnsiString& Str )
{
    if( Str.Length() == 0 )
        throw( TMPFieldException( "NULL Int32 string" ) );	               
    FData.Assign( Str.ToInt() );
}    
//------------------------------------------------------------------------------------------------------    
TMPType* TMPInt32::FromStream( UFC::PStream* Stream  ) 
{ 
    return new TMPInt32( Stream ); 
}
//------------------------------------------------------------------------------------------------------    
TMPType* TMPInt32::Duplicate( void  ) 
{ 
    return new TMPInt32( FData ); 
}
//------------------------------------------------------------------------------------------------------
void TMPInt32::Assign( TMPType& ref )
{
    TMPInt32* RefPtr = static_cast<TMPInt32*>( &ref );
    if( FType != ref.GetType() )
        throw( TMPFieldTypeException( ToTMPTypeString( ref.GetType() ), ToTMPTypeString(FType) ) );
    FData = RefPtr->FData.ToInt32();
}
//------------------------------------------------------------------------------------------------------    
TMPInt32& TMPInt32::operator = ( Int32 Val ) 
{
    FData.Assign( Val );
    return *this;
}
//------------------------------------------------------------------------------------------------------    
TMPInt32& TMPInt32::operator = ( const TMPInt32& ref ) 
{
    FData.Assign( ref.FData.ToInt32() );
    return *this;
}
//------------------------------------------------------------------------------------------------------    
// Class TMPuInt32
// Implement uInt32 class for TMP protocol.
//------------------------------------------------------------------------------------------------------    
TMPUInt32::TMPUInt32( void )
:TMPType(ttUInt32)
,FData( 0 )
{
}
//------------------------------------------------------------------------------------------------------    
TMPUInt32::TMPUInt32( UInt32 Val )
:TMPType(ttUInt32)
,FData( (UInt32)Val )
{    
}
//------------------------------------------------------------------------------------------------------    
TMPUInt32::TMPUInt32( const UFC::AnsiString& Str )
:TMPType(ttUInt32)
{
    FromString( Str );
}
//------------------------------------------------------------------------------------------------------    
TMPUInt32::TMPUInt32( UFC::PStream* Stream )
:TMPType(ttUInt32)
{
    if( FData.LoadFromStream( Stream ) !=  4 )
    throw( TMPBufferException( 4 ) );
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPUInt32::ToBinary( UInt8* Buffer )
{
    memcpy( Buffer, FData.GetBytes(), sizeof( UInt32 ) );
    return sizeof( UInt32 ); 
}        
//------------------------------------------------------------------------------------------------------    
Int32 TMPUInt32::ToStream( UFC::PStream* Stream ) 
{
    return FData.SaveToStream( Stream );
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPUInt32::ToString( char* Str, UInt32 DataLength ) 
{
    char tmpstr[16];///< 0~4,294,967,296
    
    sprintf( tmpstr, "%u", (UInt32)FData.ToInt32() );
    if( DataLength < strlen(tmpstr) )
        throw( TMPBufferException( (Int32)strlen(tmpstr) ) );
    strcpy( Str, tmpstr );
    return (Int32)strlen(tmpstr);
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPUInt32::ToInteger( void )
{
    return (Int32)FData;
}
//------------------------------------------------------------------------------------------------------        
void TMPUInt32::FromInteger( Int32 Val ) 
{    
    FData.Assign( (UInt32)Val );
}
//------------------------------------------------------------------------------------------------------        
void TMPUInt32::FromString( const UFC::AnsiString& Str )
{
    if( Str.Length() == 0 )
        throw( TMPFieldException( "NULL UInt32 string" ) );	                   
    FData.Assign( (UInt32)Str.ToInt() );
}    
//------------------------------------------------------------------------------------------------------    
TMPType* TMPUInt32::FromStream( UFC::PStream* Stream  ) 
{ 
    return new TMPUInt32( Stream ); 
}
//------------------------------------------------------------------------------------------------------    
TMPType* TMPUInt32::Duplicate( void  ) 
{ 
    return new TMPUInt32( FData ); 
}
//------------------------------------------------------------------------------------------------------
void TMPUInt32::Assign( TMPType& ref )
{
    TMPUInt32* RefPtr = static_cast<TMPUInt32*>( &ref );
    if( FType != ref.GetType() )
        throw( TMPFieldTypeException( ToTMPTypeString( ref.GetType() ), ToTMPTypeString(FType) ) );
    FData = RefPtr->FData.ToInt32();
}
//------------------------------------------------------------------------------------------------------    
TMPUInt32& TMPUInt32::operator = ( UInt32 Val ) 
{
    FData.Assign( (Int32)Val );
    return *this;
}
//------------------------------------------------------------------------------------------------------    
TMPUInt32& TMPUInt32::operator = ( const TMPUInt32& ref ) 
{
    FData.Assign( ref.FData.ToInt32() );
    return *this;
}
//------------------------------------------------------------------------------------------------------    
}
//------------------------------------------------------------------------------------------------------



