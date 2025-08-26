// 
// File:   NameValueMessage.h
// Author: yuan
//
// Created on September 5, 2007, 12:27 PM
//
//------------------------------------------------------------------------------
#ifndef _NameValueMessage_H
#define	_NameValueMessage_H
//------------------------------------------------------------------------------
#include "AnsiString.h"
#include "PHashedList.h"
#include "MemoryStream.h"
//------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------
class NameValueMessage
{
private:
	UFC::PStringList FList;
	UFC::PHashedList<UFC::AnsiString, UFC::AnsiString*> FNameValues;
	UFC::AnsiString  FDelimeter;
	UFC::AnsiString  FEquals;
public:
	NameValueMessage( const UFC::AnsiString& Delimeter = ",", const UFC::AnsiString& Equals = "=" );        
        virtual ~NameValueMessage( void );
public:
        void SetDelimeter( const UFC::AnsiString& Delimeter ) { FDelimeter = Delimeter; }
        void SetEquals( const UFC::AnsiString& Equals ) { FEquals = Equals; }
	/**
	 * Append Name = Value to the string.
	 * Append string type value.
	 * Append long type value.
	 * Append double type value.
	 */
	void Append( const UFC::AnsiString& Name, const UFC::AnsiString& Value );
	void Append( const UFC::AnsiString& Name, long Value );
	void Append( const UFC::AnsiString& Name, double Value, bool TrimZero = false );
	/**
	 * Add Name = Value to front of the string.
	 * Add string type value.
	 * Add long type value.
	 * Add double type value.
	 */
	void AddFront( const UFC::AnsiString& Name, const UFC::AnsiString& Value );
	void AddFront( const UFC::AnsiString& Name, long Value );
	void AddFront( const UFC::AnsiString& Name, double Value, bool TrimZero = false );
	/**
	 * output to the Name,Value string.
	 */
	UFC::AnsiString ToString(  bool WithRtn = false );
	/**
	 * Construct from Name-Value string.
	 */
	void FromString( const UFC::AnsiString& Str, bool TrimQuote = false );
	/**
	 * Get value.
	 */
	BOOL Get( const UFC::AnsiString& Name, UFC::AnsiString& Value, BOOL ThrowException = FALSE );
	BOOL Get( int Index, UFC::AnsiString& Name, UFC::AnsiString& Value );
	BOOL Delete( const UFC::AnsiString& Name );
	/**
	 * Is Name-Value exists..
	 */
	BOOL IsExists( const UFC::AnsiString& Name );
	/**
	 * How many name-value pairs in this object.
	 */
	int ItemCount( void );
	/**
	 * Clear all name-value pairs.
	 */
	void Clear( void );
};
//------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
#endif	/* _SQMessage_H */
