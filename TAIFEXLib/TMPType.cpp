#include "TMPTypes.h"
//-----------------------------------------------------------------------------------------
namespace TMP
{
//------------------------------------------------------------------------------------------------------
TMPDataType ToTMPDataType( const UFC::AnsiString& TypeStr )
{
    if( TypeStr == "Char" )    
        return ttChar;
    if( TypeStr == "CharArray" )
        return ttCharArray;
    if( TypeStr == "Int8" )
        return ttInt8;
    if( TypeStr == "UInt8" )
        return ttUInt8;
    if( TypeStr == "Int16" )
        return ttInt16;
    if( TypeStr == "UInt16" )
        return ttUInt16;
    if( TypeStr == "Int32" )
        return ttInt32;
    if( TypeStr == "UInt32" )
        return ttUInt32;    
    if( TypeStr == "Time" )            
        return ttTime;
    if( TypeStr == "TimeEx" )            
        return ttTimeEx;
    if( TypeStr == "Symbol" )            
        return ttSymbol;
    if( TypeStr == "LongSymbol" )            
        return ttLongSymbol;    
    if( TypeStr == "Int64" )
        return ttInt64;        
    if( TypeStr == "UInt64" )
        return ttUInt64;            
    UFC::AnsiString ErrMsg;
    
    ErrMsg.Printf( "TMP format file error, unknown type:%s",TypeStr.c_str() );
    throw( UFC::Exception( ErrMsg.c_str() ) );
}
//------------------------------------------------------------------------------------------------------
const char* ToTMPTypeString( TMPDataType Type )
{
     switch( Type )
     {
         case ttChar:      return "Char";    
         case ttCharArray: return "CharArray";    
         case ttInt8:      return "Int8";    
         case ttUInt8:     return "UInt8";    
         case ttInt16:     return "Int16";    
         case ttUInt16:    return "UInt16";    
         case ttInt32:     return "Int32";    
         case ttUInt32:    return "UInt32";    
         case ttTime:      return "Time";    
         case ttTimeEx:    return "TimeEx";    
         case ttSymbol:    return "Symbol";    
         case ttLongSymbol:return "LongSymbol"; 
         case ttInt64:     return "Int64";
         case ttUInt64:    return "UInt64";
         default:          return "UnknownType";
     }
}
//------------------------------------------------------------------------------------------------------
//  Class TMPType
//  Implements the abstract class for all TMP types.
//------------------------------------------------------------------------------------------------------
TMPType& TMPType::operator = ( const UFC::AnsiString& Val )
{
    this->FromString( Val );
    return *this;
}
//------------------------------------------------------------------------------------------------------
TMPType& TMPType::operator = ( Int32 Val )
{
    this->FromInteger( Val );
    return *this;
}
//------------------------------------------------------------------------------------------------------
TMPType& TMPType::operator = ( const TMPSymbol& ref )
{
    if( FType == ttSymbol )
    {
        TMPSymbol* ThisObj = static_cast<TMP::TMPSymbol*>(this);
        *ThisObj = ref;
        return *this;
    }
    else    
        throw( TMPFieldTypeException( "Symbol", ToTMPTypeString(FType) ) );
}
//------------------------------------------------------------------------------------------------------
TMPType& TMPType::operator = ( const TMPLongSymbol& ref )
{
    if( FType == ttLongSymbol )
    {
        TMPLongSymbol* ThisObj = static_cast<TMP::TMPLongSymbol*>(this);
        *ThisObj = ref;
        return *this;
    }
    else    
        throw( TMPFieldTypeException( "LongSymbol", ToTMPTypeString(FType) ) );
}
//------------------------------------------------------------------------------------------------------
UFC::AnsiString TMPType::ToString( void )
{
    char Buffer[ 512 ];
    
    this->ToString( Buffer, 512 );
    return UFC::AnsiString( Buffer );
}
//------------------------------------------------------------------------------------------------------
TMPType& TMPType::operator = ( const TMPTime& ref )
{
    if( FType == ttTime )
    {
        TMPTime* ThisObj = static_cast<TMP::TMPTime*>(this);
        *ThisObj = ref;
        return *this;
    }
    else    
        throw( TMPFieldTypeException( "Time", ToTMPTypeString(FType) ) );
}
//------------------------------------------------------------------------------------------------------
TMPType* TMPType::CreateTMPTypeObject( TMPDataType type, const UFC::AnsiString& Default, BOOL IsBin )
{
    switch( type )
    {
        case ttChar:      return new TMPChar( Default );
        case ttCharArray: return new TMPCharArray( Default,IsBin );
        case ttInt8:      return new TMPInt8( Default ); 
        case ttUInt8:     return new TMPUInt8( Default ); 
        case ttInt16:     return new TMPInt16( Default ); 
        case ttUInt16:    return new TMPUInt16( Default ); 
        case ttInt32:     return new TMPInt32( Default ); 
        case ttUInt32:    return new TMPUInt32( Default ); 
        case ttInt64:     return new TMPInt64( Default ); 
        case ttUInt64:    return new TMPUInt64( Default ); 
        case ttTime:      return new TMPTime( Default );
        case ttTimeEx:    return new TMPTimeEx( Default );
        case ttSymbol:    return new TMPSymbol( Default );
        case ttLongSymbol:return new TMPLongSymbol( Default );
        default:          return NULL;
    }
}
//------------------------------------------------------------------------------------------------------
TMPType* TMPType::CreateTMPTypeObject( TMPDataType type, Int32 Size, BOOL IsBin )
{
    switch( type )
    {
        case ttChar:      return new TMPChar( ' ' );
        case ttCharArray: return new TMPCharArray( Size, IsBin );
        case ttInt8:      return new TMPInt8( (Int8)0 ); 
        case ttUInt8:     return new TMPUInt8( (UInt8)0 ); 
        case ttInt16:     return new TMPInt16( (Int16)0 ); 
        case ttUInt16:    return new TMPUInt16( (UInt16)0 ); 
        case ttInt32:     return new TMPInt32( (Int32)0); 
        case ttUInt32:    return new TMPUInt32( (UInt32)0 ); 
        case ttInt64:     return new TMPInt64( (Int64)0 ); 
        case ttUInt64:    return new TMPUInt64( (UInt64)0 ); 
        case ttTime:      return new TMPTime( );
        case ttTimeEx:    return new TMPTimeEx( );
        case ttSymbol:    return new TMPSymbol( "                    " );
        case ttLongSymbol:return new TMPLongSymbol( "                                        " );
        default:          return NULL;
    }
}
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------

