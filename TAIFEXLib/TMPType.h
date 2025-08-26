// 
// File:   TMPType.h
// Author: Simon Chang
//
// Created on June 14, 2008, 7:15 PM
//
//------------------------------------------------------------------------------------------------------
#ifndef _TMPTYPE_H
#define	_TMPTYPE_H
//------------------------------------------------------------------------------------------------------
#include "TMPDefines.h"
#include "../UFC/UFC.h"
#include "../ExchangeUtility/TFXException.h"
//-----------------------------------------------------------------------------------------
namespace TMP
{
using namespace UFCType;
//------------------------------------------------------------------------------------------------------
//
// Enum TMPDataType
//
// All TMP data types.
//
//------------------------------------------------------------------------------------------------------
typedef enum
{
    ttChar       = 0,
    ttCharArray  = 1,
    ttInt8       = 2,
    ttUInt8      = 3,
    ttInt16      = 4,
    ttUInt16     = 5,
    ttInt32      = 6,
    ttUInt32     = 7,
    ttInt64      = 8,
    ttUInt64     = 13,            
    ttTime       = 9,
    ttTimeEx     = 12,
    ttSymbol     = 10,
    ttLongSymbol = 11    
            
} TMPDataType;
//------------------------------------------------------------------------------------------------------
//
// Class TMPType
//
// Abstract class for all TMP data type.
//
//------------------------------------------------------------------------------------------------------
class TMPTime;
class TMPSymbol;
class TMPLongSymbol;
//------------------------------------------------------------------------------------------------------
class TMPType
{
protected:
    TMPDataType FType;
public: ///< Constructor, Destructor.
    TMPType( TMPDataType Type ):FType( Type ){}
    virtual ~TMPType( void ){} 
public: ///< Pure virture functions
    /**
     * Serializing this TMP type to memory.
     */
    virtual Int32    ToBinary( UInt8* Buffer ) = 0;
    /**
     * Serializing this TMP type to stream.
     */
    virtual Int32    ToStream( UFC::PStream* Stream ) = 0;
    /**
     * Conver this TMP type to string persentation.
     */
    virtual Int32    ToString( char* Data, UInt32 DataLength ) = 0;    
    /**
     * Conver this TMP type to integer.
     */
    virtual Int32    ToInteger( void ) = 0;
    virtual Int64    ToInteger64( void ) = 0;
    /**
     * Set value from string.
     */
    virtual void     FromString( const UFC::AnsiString& String ) = 0;    
    /**
     * Set value from Integer.
     */
    virtual void     FromInteger( Int32 ) = 0;    
    /**
     * Create a copy insteance, but read properties from stream.
     */
    virtual TMPType* FromStream( UFC::PStream* Stream  ) = 0;
    /**
     * Assign value from a existence instance.
     */
    virtual void Assign( TMPType& Src ) = 0;
    /**
     * Create a copy instance.
     */
    virtual TMPType* Duplicate( void  ) = 0;
    /**
     * Size need to store this TMP type.(Binary mode)
     */
    virtual Int32    GetSize( void  ) = 0; 
public: ///< Public functions.    
    /**
     * Get the data type of this object.
     */
    TMPDataType GetType( void ) { return FType; }
    /**
     * Convert this field to string.
     */
    UFC::AnsiString ToString( void );
    /**
     * Assign a string value to this TMP type.
     */
    TMPType& operator = ( const UFC::AnsiString& Val );
    /**
     * Assign a integer value to this TMP type.
     */
    TMPType& operator = ( Int32 Val );
    /**
     * Assign a Symbol type  value to this type object.
     */
    TMPType& operator = ( const TMPSymbol& ref );
    TMPType& operator = ( const TMPLongSymbol& ref );
    /**
     * Assign a Time type  value to this type object.
     */
    TMPType& operator = ( const TMPTime& ref );    
    /**
     * Operator (int) convert this field to integer.
     */
    operator int ( void )             { return ToInteger(); }
    /**
     * Operator (AnsiString) convert this field to UFC::AnsiString.
     */
    operator UFC::AnsiString ( void ) { return ToString(); }
public:
    static TMPType*    CreateTMPTypeObject( TMPDataType type, const UFC::AnsiString& Default, BOOL IsBin );
    static TMPType*    CreateTMPTypeObject( TMPDataType type, Int32 Size, BOOL IsBin );    
};
//------------------------------------------------------------------------------------------------------
extern TMPDataType ToTMPDataType( const UFC::AnsiString& TypeStr );
extern const char* ToTMPTypeString( TMPDataType Type );
//------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------
#endif	/* _TMPTYPE_H */

