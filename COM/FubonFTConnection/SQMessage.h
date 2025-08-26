// 
// File:   SQMessage.h
// Author: yuan
//
// Created on September 5, 2007, 12:27 PM
//
//------------------------------------------------------------------------------
#ifndef _SQMessage_H
#define	_SQMessage_H
//------------------------------------------------------------------------------
#include <UFC.h>
//------------------------------------------------------------------------------
class TSQMessage
{
private:
    UFC::PStringList FList;
    UFC::PHashedList<UFC::AnsiString, UFC::AnsiString*> FNameValues;
    
public:
    TSQMessage( void );
    virtual ~TSQMessage( void );

public:
    void Append( const UFC::AnsiString& Name, const UFC::AnsiString& Value );
    void Append( const UFC::AnsiString& Name, long Value );
    void Append( const UFC::AnsiString& Name, double Value );
    UFC::AnsiString ToString( void );

public:
    void FromString( const UFC::AnsiString& Str );
    BOOL Get( const UFC::AnsiString& Name, UFC::AnsiString& Value );

private:
    void Clear( void );
};
//------------------------------------------------------------------------------
#endif	/* _SQMessage_H */
