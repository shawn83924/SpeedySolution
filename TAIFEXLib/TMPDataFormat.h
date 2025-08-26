// 
// File:   TMPdataFormat.h
// Author: Simon Chang
//
// Created on June 22, 2008, 6:57 PM
//
//------------------------------------------------------------------------------------------------------
#ifndef _TMPDATAFORMAT_H
#define	_TMPDATAFORMAT_H
//------------------------------------------------------------------------------------------------------
#include "TMPTypes.h"
#include "../UFC/iniFile.h"
//-----------------------------------------------------------------------------------------
namespace TMP
{
//------------------------------------------------------------------------------------------------------
class TMPMessage;   
class TMPDataFormat;
//------------------------------------------------------------------------------------------------------
class TMPFieldInfo
{
friend class TMPDataFormat;
public:
    UFC::AnsiString FName;
    TMPType*        FDefaultVal;
private:    
    TMPType*        CreateFieldFromStream( UFC::PStream* Stream );
    TMPType*        CreateField( void );    
public:
    /**
     * Constructor
     * Create a field information object
     * @Param Name Name of this field.
     * @Param DefVal, TMP type object with default value. 
     */
    TMPFieldInfo( const UFC::AnsiString& Name, TMPType* DefVal );
    /**
     * Destructor
     */
    ~TMPFieldInfo( void );        
    /**
     * Get the field name.
     */
    const UFC::AnsiString& GetFieldName( void ) { return FName; }
    /**
     * Get the field type.
     */
    TMPDataType            GetFieldType( void ) { return FDefaultVal->GetType( ); }
    /**
     * Get the field size.
     */
    Int32                  GetSize( void )      { return FDefaultVal->GetSize( ); }

};
//------------------------------------------------------------------------------------------------------
// Class TMPDataFormat
// Class to keep TMP message format.
//------------------------------------------------------------------------------------------------------
// Ini format file example:
//
// [Header]
// MessageType = 5
// MessageName = TestMsg
//
// [FCMID]
// Type = CharArray
// Length = 7
// Default = F999000
//
// [Data]
// Type = CharArray
// Length = -20
//        
// [TimeInForce]
// Type = UInt8
// Default = 0 
//
// [PositionEffect]
// Type = Char
// Default = C
//
// [TransactTime]
// Type = Time
// Default = 12:00:00.000
//
// [Symbol_x]
// Type = Symbol
// Default = 100,200,1,2,0
//
//------------------------------------------------------------------------------------------------------
class TMPDataFormat
{
friend class TMPMessage;    
protected:
    UInt8                                     FMsgType;  
    UInt16                                    FMsgLen;    
    UFC::AnsiString                           FMsgName;
    UFC::PtrList<TMPFieldInfo>                FFields;
    UFC::PHashMap<UFC::AnsiString,Int32>      FFieldIndex;
    BOOL                                      FIsFIXLength;
private:
    void          LoadFormat( const UFC::AnsiString& FormatFile );
    void          LoadFieldSetting( UFC::UiniFile* pIniFile );
    Int16         AddFieldInfo( const UFC::AnsiString& SectionName, UFC::Section* pSection );
    TMPFieldInfo* AddCharArrayField( const UFC::AnsiString& SectionName, UFC::Section* pSection );
    TMPFieldInfo* AddOthersField( TMPDataType  Datatype, const UFC::AnsiString& SectionName, UFC::Section* pSection );
private: ///< functions call by TMPMessage class.
    /**
     * Add all fields to a fields list with default value.
     */
    void  AddTMPFields( UFC::PtrList<TMPType>&  Fields );
    /**
     * Add all fields to a field list with values load from stream.
     */
    void  AddTMPFieldsFromStream( UFC::PtrList<TMPType>& Fields, UFC::PStream* Stream );
public:
    /**
     * Constructor.
     * Create a TMPDataFormat object from given ini format file.
     */
    TMPDataFormat( const UFC::AnsiString& FormatFile );
    /**
     * Destructor.
     */
    ~TMPDataFormat();        
public:
    /**
     *  Get the field information.
     */
    TMPFieldInfo*          GetFieldInfo( Int32 FieldIndex ) { return FFields.GetItem(FieldIndex); }
    /**
     *  Get the field Name by index.
     */
    const UFC::AnsiString& GetFieldName( Int32 Index)       { return FFields.GetItem( Index )->GetFieldName(); }
    /**
     *  Get the field index by field name.
     */
    Int32                  GetFieldIndex( const UFC::AnsiString& Name );   
    /**
     *  How many fields in this TMP message.
     *  (Not include header fields and Checksum field.)
     */    
    Int32                  GetFieldsCount( void )           { return FFields.ItemCount(); }    
    /**
     *  Get the TMP message type.
     */    
    UInt8                  GetMessageType( void )           { return FMsgType; }        
    /**
     *  Is this a fix length TMP message ?
     */    
    BOOL                   IsFixLength( void )              { return FIsFIXLength; }        
    /**
     *  Get the TMP message length.
     */
    UInt16                 GetMessageLength( void )         { return FMsgLen; }        
    /**
     *  Get the TMP message name.
     */
    const UFC::AnsiString& GetMessageName( void )           { return FMsgName; }
    
};
//------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------
#endif	/* _TMPDATAFORMAT_H */

