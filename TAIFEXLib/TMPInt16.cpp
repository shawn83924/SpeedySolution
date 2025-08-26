#include "TMPTypes.h"
//-----------------------------------------------------------------------------------------
namespace TMP
{
//------------------------------------------------------------------------------------------------------    
// Class TMPInt16
// Implement Int16 class for TMP protocol.
//------------------------------------------------------------------------------------------------------    
TMPInt16::TMPInt16( void )
:TMPType(ttInt16)
,FData( 0 )
{
}
//------------------------------------------------------------------------------------------------------    
TMPInt16::TMPInt16( Int16 Val )
:TMPType(ttInt16)
,FData( Val )
{
}
//------------------------------------------------------------------------------------------------------    
TMPInt16::TMPInt16( const UFC::AnsiString& Str )
:TMPType(ttInt16)
{
    FromString( Str );
}
//------------------------------------------------------------------------------------------------------    
TMPInt16::TMPInt16( UFC::PStream* Stream )
:TMPType(ttInt16)
{
    if( FData.LoadFromStream( Stream ) !=  2 )
        throw( TMPBufferException( 2 ) );        
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPInt16::ToBinary( UInt8* Buffer )
{
    memcpy( Buffer, FData.GetBytes(), sizeof( Int16 ) );
    return sizeof( Int16 ); 
}        
//------------------------------------------------------------------------------------------------------    
Int32 TMPInt16::ToStream( UFC::PStream* Stream )
{
    return FData.SaveToStream( Stream );
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPInt16::ToString( char* Str, UInt32 DataLength ) 
{
    char tmpstr[8];///< -32768~32767
    
    sprintf( tmpstr, "%d", FData.ToInt16() );
    if( DataLength < strlen(tmpstr) )
        throw( TMPBufferException( (Int32)strlen(tmpstr) ) );
    strcpy( Str, tmpstr );
    return (Int32)strlen(tmpstr);
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPInt16::ToInteger( void )
{
    return (Int32)(FData.ToInt16());
}
//------------------------------------------------------------------------------------------------------        
void TMPInt16::FromInteger( Int32 Val ) 
{    
    if( Val < -32768 || Val > 32767 )
        throw( TMPValueRangeException( Val, "TMPInt16" ) );	   
    FData.Assign( (Int16) Val );
}
//------------------------------------------------------------------------------------------------------        
void TMPInt16::FromString( const UFC::AnsiString& Str )
{
    if( Str.Length() == 0 )
        throw( TMPFieldException( "NULL Int16 string" ) );	               
    Int32 Val = Str.ToInt();
    if( Val < -32768 || Val > 32767 )
        throw( TMPFieldException( Str.c_str() ) );	   
    FData.Assign( (Int16) Val );    
}    
//------------------------------------------------------------------------------------------------------    
TMPType* TMPInt16::FromStream( UFC::PStream* Stream  ) 
{ 
    return new TMPInt16( Stream ); 
}
//------------------------------------------------------------------------------------------------------    
TMPType* TMPInt16::Duplicate( void  ) 
{ 
    return new TMPInt16( FData.ToInt16() ); 
}
//------------------------------------------------------------------------------------------------------
void TMPInt16::Assign( TMPType& ref )
{
    TMPInt16* RefPtr = static_cast<TMPInt16*>( &ref );
    if( FType != ref.GetType() )
        throw( TMPFieldTypeException( ToTMPTypeString( ref.GetType() ), ToTMPTypeString(FType) ) );
    FData = RefPtr->FData.ToInt16();
}
//------------------------------------------------------------------------------------------------------    
TMPInt16& TMPInt16::operator = ( Int16 Val ) 
{
    FData.Assign( Val );
    return *this;
}
//------------------------------------------------------------------------------------------------------    
TMPInt16& TMPInt16::operator = ( const TMPInt16& ref ) 
{
    FData.Assign( ref.FData.ToInt16() );
    return *this;
}
//------------------------------------------------------------------------------------------------------    
// Class TMPUInt16
// Implement UInt16 class for TMP protocol.
//------------------------------------------------------------------------------------------------------    
TMPUInt16::TMPUInt16( void )
:TMPType(ttUInt16)
,FData( 0 )
{
}
//------------------------------------------------------------------------------------------------------    
TMPUInt16::TMPUInt16( UInt16 Val )
:TMPType(ttUInt16)
,FData( (UInt16)Val )
{
}
//------------------------------------------------------------------------------------------------------    
TMPUInt16::TMPUInt16( const UFC::AnsiString& Str )
:TMPType(ttUInt16)
{
    FromString( Str );
}
//------------------------------------------------------------------------------------------------------    
TMPUInt16::TMPUInt16( UFC::PStream* Stream )
:TMPType(ttUInt16)
{
    if( FData.LoadFromStream( Stream ) !=  2 )
        throw( TMPBufferException( 2 ) );            
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPUInt16::ToBinary( UInt8* Buffer )
{
    memcpy( Buffer, FData.GetBytes(), sizeof( UInt16 ) );
    return sizeof( Int16 ); 
}        
//------------------------------------------------------------------------------------------------------    
Int32 TMPUInt16::ToStream( UFC::PStream* Stream )
{
    return FData.SaveToStream( Stream );
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPUInt16::ToString( char* Str, UInt32 DataLength )
{
    char tmpstr[ 8 ];///< 0~65535
        
    sprintf( tmpstr, "%u", (UInt16)FData.ToInt16() );
    if( DataLength < strlen(tmpstr) )
        throw( TMPBufferException( (Int32)strlen(tmpstr) ) );
    strcpy( Str, tmpstr );
    return (Int32)strlen(tmpstr);
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPUInt16::ToInteger( void )
{
    return (Int32)FData.ToInt16();
}
//------------------------------------------------------------------------------------------------------        
void TMPUInt16::FromInteger( Int32 Val ) 
{    
    if( Val < 0 || Val > 65535 )
        throw( TMPValueRangeException( Val, "TMPUInt16" ) );	   
    FData.Assign( (UInt16) Val );
}
//------------------------------------------------------------------------------------------------------        
void TMPUInt16::FromString( const UFC::AnsiString& Str )
{
    if( Str.Length() == 0 )
        throw( TMPFieldException( "NULL UInt16 string" ) );	                   
    Int32 Val = Str.ToInt();
    if( Val < 0 || Val > 65535 )
        throw( TMPFieldException( Str.c_str() ) );	   
    FData.Assign( (UInt16) Val );    
}    
//------------------------------------------------------------------------------------------------------    
TMPType* TMPUInt16::FromStream( UFC::PStream* Stream  ) 
{ 
    return new TMPUInt16( Stream ); 
}
//------------------------------------------------------------------------------------------------------    
TMPType* TMPUInt16::Duplicate( void  ) 
{ 
    return new TMPUInt16( (UInt16)FData.ToInt16() ); 
}
//------------------------------------------------------------------------------------------------------
void TMPUInt16::Assign( TMPType& ref )
{
    TMPUInt16* RefPtr = static_cast<TMPUInt16*>( &ref );
    if( FType != ref.GetType() )
        throw( TMPFieldTypeException( ToTMPTypeString( ref.GetType() ), ToTMPTypeString(FType) ) );
    FData = RefPtr->FData.ToInt16();
}
//------------------------------------------------------------------------------------------------------    
TMPUInt16& TMPUInt16::operator = ( UInt16 Val ) 
{
    FData.Assign( (Int16)Val );
    return *this;
}
//------------------------------------------------------------------------------------------------------    
TMPUInt16& TMPUInt16::operator = ( const TMPUInt16& ref ) 
{
    FData.Assign( ref.FData.ToInt16() );
    return *this;
}
//------------------------------------------------------------------------------------------------------    
}
//------------------------------------------------------------------------------------------------------



