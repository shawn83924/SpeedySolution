//---------------------------------------------------------------------------
// This is a part of the Palm Foundation Classes Library for Palm OS (R) platforms.
// Copyright (c) 2002 MobileDigital Development Inc.
// All rights reserved.
//
// Description: StringBuffer Class. Provide faster string concatenation, insertion,
//              partial removal, reverse order.
// Author: Simon
//
// This source code is only intended as a supplement to the
// Object Class Library for Palm OS (R) platforms Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Object Class Library for UNIX platforms product.
//---------------------------------------------------------------------------
#ifndef StringBufferH
#define StringBufferH
//---------------------------------------------------------------------------
#include "AnsiString.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class PStringBuffer
{
	friend class AnsiString;
private:
	char*  StrBuffer;
	UInt16 FLength;
	UInt16 FCapacity;
	void InitCap(UInt16 length);
	void InitStr( const char *Str, const UInt16 Length);
	void SAppend( const char *Str, const UInt16 Length);
	void SInsert(UInt16 offset, const char *Str, UInt16 length);

public:
	PStringBuffer();
	PStringBuffer( const char* src );
	PStringBuffer( const AnsiString &src );
	PStringBuffer( UInt16 InitCapacity );
	~PStringBuffer( );

	PStringBuffer& Append(bool b);
	PStringBuffer& Append(char c);
	PStringBuffer& Append(const char* str);
	PStringBuffer& Append(const char* str, Int32 offset, UInt16 length);
	PStringBuffer& Append(const AnsiString &src);
	PStringBuffer& Append( Int32 i);
	PStringBuffer& Delete( UInt16 index, UInt16 count );
	void           EnsureCapacity( UInt16 minimumCapacity );
	void           Pack();
	PStringBuffer& Insert( UInt16 offset, bool b);
	PStringBuffer& Insert( UInt16 offset, char c);
	PStringBuffer& Insert( UInt16 offset, const char* str);
	PStringBuffer& Insert( UInt16 offset, const char* str, Int32 strOffset, UInt16 length);
	PStringBuffer& Insert( UInt16 offset, const AnsiString &src);
	PStringBuffer& Insert( UInt16 offset, Int32 i);
	PStringBuffer& Reverse();
	PStringBuffer& SetLength(UInt16 length);
	AnsiString     SubString(UInt16 index, UInt16 count);
	AnsiString     ToString() const { return AnsiString(StrBuffer); }

	UInt16        AppendPrintf( const char* formatStr, ... );
	UInt16        Printf( const char* formatStr, ... );

	// Inline functions
	UInt16    Length( void ) const { return FLength; }
	UInt16    Capacity() const { return FCapacity; }

	// need to lock when use
        char* c_str( void ) const { return StrBuffer; }

	// Operators
    	const char operator [](const int idx) const
        {
            if (idx < FLength)
                return StrBuffer[idx];
            else
                return StrBuffer[FLength];
	}
	PStringBuffer&  operator = (const char* rhs);
	PStringBuffer&  operator = (const AnsiString& rhs);
	PStringBuffer&  operator = (const PStringBuffer& rhs);
};
//---------------------------------------------------------------------------
}	/* namespace UFC */
//---------------------------------------------------------------------------
#endif

