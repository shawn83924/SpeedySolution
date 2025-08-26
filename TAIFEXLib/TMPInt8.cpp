#include "TMPTypes.h"
//-----------------------------------------------------------------------------------------
namespace TMP
{
//------------------------------------------------------------------------------------------------------
// Class TMPChar
// Implement Char class for TMP protocol.
//------------------------------------------------------------------------------------------------------
TMPChar::TMPChar( void )
:TMPType(ttChar)
,FData(' ') 
{
}
//------------------------------------------------------------------------------------------------------
TMPChar::TMPChar( char ch )
:TMPType(ttChar)
,FData( ch )
{
}
//------------------------------------------------------------------------------------------------------    
TMPChar::TMPChar( const UFC::AnsiString& Str )
:TMPType(ttChar)
{    
    FromString( Str );
}
//------------------------------------------------------------------------------------------------------
TMPChar::TMPChar( UFC::PStream* Stream )
:TMPType( ttChar ) 
{
    if( Stream->Read( (void*)&FData, 1 ) !=  1 )
        throw( TMPBufferException( 1 ) );
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPChar::ToBinary( UInt8* Buffer )
{
    memcpy( Buffer, &FData, sizeof( char ) );
    return sizeof( char ); 
}        
//------------------------------------------------------------------------------------------------------
Int32 TMPChar::ToStream( UFC::PStream* Stream ) 
{
    return Stream->Write( (void*)&FData, sizeof( char ) );
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPChar::ToString( char* Str, UInt32 DataLength ) 
{
    if( DataLength < 1 )
        throw( TMPBufferException( 1 ) );
    *(Str) = FData;
    *(Str+1) = 0;
    return sizeof( char );
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPChar::ToInteger( void )
{
    return (Int32) FData;
}
//------------------------------------------------------------------------------------------------------    
TMPChar& TMPChar::operator = ( char Val ) 
{
    FData = Val;
    return *this;
}
//------------------------------------------------------------------------------------------------------    
TMPChar& TMPChar::operator = ( const TMPChar& ref ) 
{
    FData = ref.FData;
    return *this;
}
//------------------------------------------------------------------------------------------------------        
void TMPChar::FromInteger( Int32 Val ) 
{    
    if( Val < -128 || Val > 127 )
        throw( TMPValueRangeException( Val, "TMPChar" ) );	   
    FData = (char) Val;
}
//------------------------------------------------------------------------------------------------------        
void TMPChar::FromString( const UFC::AnsiString& Str )
{
    if( Str.Length() < 1 )
        throw( TMPFieldException( "NULL Char string" ) );	   
    FData = Str[0];
}
//------------------------------------------------------------------------------------------------------        
TMPType* TMPChar::FromStream( UFC::PStream* Stream  ) 
{ 
    return new TMPChar(Stream); 
}
//------------------------------------------------------------------------------------------------------    
TMPType* TMPChar::Duplicate( void  )                  
{ 
    return new TMPChar( FData ); 
}
//------------------------------------------------------------------------------------------------------
void TMPChar::Assign( TMPType& ref )
{
    TMPChar* RefPtr = static_cast<TMPChar*>( &ref );
    if( FType != ref.GetType() )
        throw( TMPFieldTypeException( ToTMPTypeString( ref.GetType() ), ToTMPTypeString(FType) ) );
    FData = RefPtr->FData;
}
//------------------------------------------------------------------------------------------------------
// Class TMPUInt8
// Implement UInt8 class for TMP protocol.
//------------------------------------------------------------------------------------------------------    
TMPUInt8::TMPUInt8( void )
:TMPType(ttUInt8)
,FData( 0 )
{
}
//------------------------------------------------------------------------------------------------------    
TMPUInt8::TMPUInt8( UInt8 Val )
:TMPType(ttUInt8)
,FData( Val )
{
}
//------------------------------------------------------------------------------------------------------    
TMPUInt8::TMPUInt8( const UFC::AnsiString& Str )
:TMPType(ttUInt8)
{
    FromString( Str );
}
//------------------------------------------------------------------------------------------------------    
TMPUInt8::TMPUInt8( UFC::PStream* Stream )
:TMPType(ttUInt8)
{
   if( Stream->Read( (void*)&FData, 1 ) !=  1 )
       throw( TMPBufferException( 1 ) );        
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPUInt8::ToBinary( UInt8* Buffer )
{
    memcpy( Buffer, &FData, sizeof( UInt8 ) );
    return sizeof( UInt8 ); 
}        
//------------------------------------------------------------------------------------------------------    
Int32 TMPUInt8::ToStream( UFC::PStream* Stream )
{
   return Stream->Write( (void*)&FData, sizeof( UInt8 ) ); 
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPUInt8::ToString( char* Str, UInt32 DataLength )
{
   char    tmpstr[8];///< 0~255
   UInt32  Len;
        
   sprintf( tmpstr, "%u", FData );
   Len = (UInt32)strlen( tmpstr );
   if( DataLength < Len )
       throw( TMPBufferException( Len ) );
   strcpy( Str, tmpstr );
   return Len;
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPUInt8::ToInteger( void )
{
    return (Int32) FData;
}
//------------------------------------------------------------------------------------------------------        
void TMPUInt8::FromInteger( Int32 Val ) 
{    
    if( Val < 0 || Val > 255 )
        throw( TMPValueRangeException( Val, "TMPUInt8" ) );	   
    FData = (UInt8) Val;
}
//------------------------------------------------------------------------------------------------------        
void TMPUInt8::FromString( const UFC::AnsiString& Str )
{
    if( Str.Length() == 0 )
        throw( TMPFieldException( "NULL UInt8 string" ) );	       
    Int32 Val = Str.ToInt();
    if( Val < 0 || Val > 255 )
        throw( TMPFieldException( Str.c_str() ) );	   
    FData = (UInt8) Val;
}   
//------------------------------------------------------------------------------------------------------    
TMPType* TMPUInt8::FromStream( UFC::PStream* Stream  ) 
{ 
   return new TMPUInt8( Stream ); 
}
//------------------------------------------------------------------------------------------------------    
TMPType* TMPUInt8::Duplicate( void  ) 
{ 
   return new TMPUInt8( FData ); 
}
//------------------------------------------------------------------------------------------------------
void TMPUInt8::Assign( TMPType& ref )
{
    TMPUInt8* RefPtr = static_cast<TMPUInt8*>( &ref );
    if( FType != ref.GetType() )
        throw( TMPFieldTypeException( ToTMPTypeString( ref.GetType() ), ToTMPTypeString(FType) ) );
    FData = RefPtr->FData;
}
//------------------------------------------------------------------------------------------------------    
TMPUInt8& TMPUInt8::operator = ( UInt8 Val ) 
{ 
   FData = Val; 
   return *this; 
}   
//------------------------------------------------------------------------------------------------------    
TMPUInt8& TMPUInt8::operator = ( const TMPUInt8& ref ) 
{ 
   FData = ref.FData; 
   return *this; 
}
//------------------------------------------------------------------------------------------------------    
// Class TMPInt8
// Implement Int8 class for TMP protocol.
//------------------------------------------------------------------------------------------------------    
TMPInt8::TMPInt8( void )
:TMPType(ttInt8)
,FData( 0 )
{
}
//------------------------------------------------------------------------------------------------------    
TMPInt8::TMPInt8( Int8 Val )
:TMPType(ttInt8)
,FData( Val )
{
}
//------------------------------------------------------------------------------------------------------    
TMPInt8::TMPInt8( const UFC::AnsiString& Str )
:TMPType(ttInt8)
{
    FromString( Str );
}
//------------------------------------------------------------------------------------------------------    
TMPInt8::TMPInt8( UFC::PStream* Stream )
:TMPType(ttInt8) 
{
    if( Stream->Read( (void*)&FData, 1 ) !=  1 )
    throw( TMPBufferException( 1 ) );
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPInt8::ToBinary( UInt8* Buffer )
{
    memcpy( Buffer, &FData, sizeof( Int8 ) );
    return sizeof( Int8 ); 
}        
//------------------------------------------------------------------------------------------------------    
Int32 TMPInt8::ToStream( UFC::PStream* Stream ) 
{
    return Stream->Write( (void*)&FData, sizeof( Int8 ) );
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPInt8::ToInteger( void )
{
    return (Int32) FData;
}
//------------------------------------------------------------------------------------------------------        
void TMPInt8::FromInteger( Int32 Val ) 
{    
    if( Val < -128 || Val > 127 )
        throw( TMPValueRangeException( Val, "TMPInt8" ) );	   
    FData = (Int8) Val;
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPInt8::ToString( char* Str, UInt32 DataLength ) 
{
    char tmpstr[8];///< -128~127
    
    sprintf( tmpstr, "%d", FData );
    if( DataLength < strlen(tmpstr) )
        throw( TMPBufferException( (Int32)strlen(tmpstr) ) );
    strcpy( Str, tmpstr );
    return (Int32)strlen(tmpstr);
}
//------------------------------------------------------------------------------------------------------        
void TMPInt8::FromString( const UFC::AnsiString& Str )
{
    if( Str.Length() == 0 )
        throw( TMPFieldException( "NULL Int8 string" ) );	           
    Int32 Val = Str.ToInt();
    if( Val < -128 || Val > 127 )
        throw( TMPFieldException( Str.c_str() ) );	   
    FData = (Int8) Val;    
}
//------------------------------------------------------------------------------------------------------    
TMPType* TMPInt8::FromStream( UFC::PStream* Stream  )
{ 
    return new TMPInt8( Stream ); 
}
//------------------------------------------------------------------------------------------------------    
TMPType* TMPInt8::Duplicate( void  ) 
{ 
    return new TMPInt8( FData ); 
}
//------------------------------------------------------------------------------------------------------
void TMPInt8::Assign( TMPType& ref )
{
    TMPInt8* RefPtr = static_cast<TMPInt8*>( &ref );
    if( FType != ref.GetType() )
        throw( TMPFieldTypeException( ToTMPTypeString( ref.GetType() ), ToTMPTypeString(FType) ) );
    FData = RefPtr->FData;
}
//------------------------------------------------------------------------------------------------------    
TMPInt8& TMPInt8::operator = ( Int8 Val ) 
{
    FData = Val;
    return *this;
}
//------------------------------------------------------------------------------------------------------    
TMPInt8& TMPInt8::operator = ( const TMPInt8& ref ) 
{
    FData = ref.FData;
    return *this;
}
//------------------------------------------------------------------------------------------------------    
}
//------------------------------------------------------------------------------------------------------



