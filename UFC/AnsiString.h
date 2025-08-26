//---------------------------------------------------------------------------
// This is a part of the UNIX Foundation Classes Library for UNIX platforms.
// Copyright (c) 2003 MobileDigital Development Inc.
// All rights reserved.
//
// Description: String Manipulating Class.
// Author: Simon Chang
//
// This source code is only intended as a supplement to the
// Object Class Library for UNIX platforms Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// UNIX Foundation Classes Library for UNIX platforms product.
//---------------------------------------------------------------------------
#ifndef __UFC_ANSISTRING_H
#define __UFC_ANSISTRING_H
//---------------------------------------------------------------------------
#include "UFCType.h"
#include <string.h>
#ifdef _AIX433
    #include <strings.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#if (defined(__LINUX) || (defined(__AIX) && defined(__ICONV)))
#include <iconv.h>
#endif

//---------------------------------------------------------------------------
namespace UFC
{
using namespace UFCType;
//------------------------------------------------------------------------------
class PStream;
class PStringBuffer;
//------------------------------------------------------------------------------
class PInitAnsiString
{
public:
	PInitAnsiString();
	~PInitAnsiString();
};
//---------------------------------------------------------------------------
/**
 * The <code>AnsiString</code> class represents character strings.
 */
class AnsiString
{   
    friend class PStringBuffer;
    friend class PInitAnsiString;
private:
    static UInt8*  DelimiterLUT;
    static char*   WCToMBBuffer;    
private:
    char   *StrBuffer;
    UInt16 FLength;
private:
    void   SetSize(UInt16 Size, bool KeepData = true);
    void   Append( const char *Str, const UInt16 Length);
    void   TrimCheck(AnsiString &ResultStr, bool left, bool right) const throw();
    UInt16 StrNLen( const char *Str, const UInt16 MaxLength );
public:
    /**
     * Default Constructor with empty characters
     */
    AnsiString():StrBuffer( NULL ),FLength(0) {}
    /**
     * Copy array of characters
     * Copy number of characters from array of characters
     * Assign array of characters
     */
    AnsiString( const wchar_t* src); ///< Convert from wide string
    AnsiString( const char* src);

    /**
     * Copy number of characters from array of characters
     */
    AnsiString( const char* src, Int32 len);

    /**
     * Copy Constructor
     */
    AnsiString( const AnsiString &src );

    /**
     * Construct a string with only one character
     */
    AnsiString( char src );

    /**
     * Construct a string by converting numbers (Int32) to string
     */
    AnsiString( Int32 num );

    /**
     * Construct a string by converting numbers (Int64) to string
     */
    AnsiString( Int64 num );        

    ~AnsiString( );
    int LengthToMBStr( const wchar_t* src );
    /**
     * Case Sensititive Compare
     *
     * @return (0) if equal, (< 0) if smaller than src and (> 0) if greater
     *         than src
     */
    int          AnsiCompare( const AnsiString& src) const throw();
    int          AnsiCompare( const char* src) const throw();
    /**
     * Case Insensititive Compare
     *
     * @return (0) if equal, (< 0) if smaller than src and (> 0) if greater
     *         than src
     */
    int          AnsiCompareIC( const AnsiString& src) const throw();
    int          AnsiCompareIC( const char* src) const throw();
    int   	     AnsiNCompare( const char* src, Int16 Count ) const throw();
    int	         AnsiNCompareIC( const char* src, Int16 Count ) const throw();
    /**
     * @return the address of last character, NULL if empty
     */
    char*        AnsiLastChar( void )const  { return (StrBuffer + FLength - 1 ); }

    /**
     * @return the index position of matching SubString, (-1) if no matching found
     */
    Int32        AnsiPos( const AnsiString& subStr) const throw();
    Int32 		 AnsiPos( const char *subStr) const throw();
    Int32		 AnsiPos( const char subChar) const throw();

    /**
     * Delete the count of characters from index position
     */
    AnsiString&  Delete(UInt16 index, UInt16 count);

    /**
     * Set Delimiter for FirstDelimiter(), FirstDelimiter(UInt16), LastDelimiter() functions
     */
    void         SetDelimiter( const AnsiString& delimiters ) const throw();

    /**
     * Find index position of delimiter found from the beginning of the string
     */
    Int32        FirstDelimiter( void ) const throw();

    /**
     * Find index position of delimiters found from the startIndex position of the string
     */
    Int32        FirstDelimiter( UInt16 startIndex ) const throw();

    /**
     * Find index position of delimiters found from the end of the string
     */
    Int32        LastDelimiter( void ) const throw();

    /**
     * Find index position of delimiters found from the end of the string
     */
    Int32        LastDelimiter( const AnsiString& delimiters ) const throw();

    /**
     * Find index position of delimiter found from the beginning of the string
     */
    Int32        FirstDelimiter( const AnsiString& delimiters ) const throw();

    /**
     * Find index position of delimiters found from the startIndex position of the string
     */
    Int32        FirstDelimiter( UInt16 startIndex, const AnsiString& delimiters ) const throw();

    /**
     * @return true if string is empty else false
     */
    bool         IsEmpty( void )const { return (FLength == 0); }

    /**
     * @return true if delimiters were found in the string from startIndex position
     */
    bool         IsDelimiter( const AnsiString& delimiters, UInt16 startIndex) const throw();

    /**
     * Set the length of this AnsiString, no effect if setLength >= the lenght of this AnsiString
     */
    AnsiString&  SetLength(UInt16 setLength);

    /**
     * @return a new AnsiString that is a substring of this AnsiString.
     *           The substring contains count characters beginning at index.
     */
    AnsiString   SubString(UInt16 startIndex, UInt16 count) const;
    AnsiString   SubLast( UInt16 count) const;

    /**
     * Assign char* for this AnsiString, ##DO NOT FREE MEMORY OR USE char[]##
     */
    //void         Assign(const char *Str) throw();
    /**
     * Converts a string to an integer, throwing an exception if the string is invalid.
     *
     * ToInt converts the given string to an integer value and returns the integer.
     * If the string doesn't contain a valid value, an PConvertError exception is thrown.
     */
    Int32        ToInt() const  { return atoi( StrBuffer ); }
    UInt32       ToUInt() const { return strtoul(StrBuffer, (char **)NULL, 10); }
    double       ToDouble() const;
    Int64        ToInt64() const;
	UInt64       ToUInt64() const;
    /**
     * Manipulate printf function in C Language
     */
    UInt16       Printf( const char* formatStr, ... );
    /**
     * Manipulate printf function in C Language and append at the end of string
     */
    UInt16       AppendPrintf( const char* FormatStr, ... );
    /**
     * @return the length of this AnsiString
     */
    UInt16	 Length( void ) const { return FLength; }
    /**
     * @return the address of start Characters array
     */
    const char*  c_str( ) const { return StrBuffer; }
    char* Rowdata( ) { return StrBuffer; }

    AnsiString&  PadThis( Int32 PadLength, char PadChar, bool PadRight = true );
    AnsiString&  LowerThis( void ) throw();
    AnsiString&  UpperThis( void ) throw();
    /**
     * @return a copy of the string in lowercase
     */
    AnsiString   LowerCase( void ) const;
    /**
     * @return a copy of the string in uppercase
     */
    AnsiString   UpperCase( void ) const;
    void 		 TrimLeft( char TrinCh = 10 );
    void 		 TrimRight( char TrinCh = 10 );

    bool EndWith(const char* EndingStr) const throw();
    bool EndWithIC(const char* EndingStr) const throw();

    char FirstChar( void )
    {
        if( StrBuffer )
            return *StrBuffer;
        return 0;
    }

    char LastChar( void )
    {
        if( FLength > 0 )
            return *( StrBuffer + FLength - 1 );
        return 0;
    }
    ///< Persistency functions
    void Copy( const char* src, int Len );
    void LoadFromStream( PStream* Stream );
    void SaveToStream( PStream* Stream ) const;    
    // Operators
    operator size_t() const;
    operator const char*() const { return StrBuffer; }
    char&        operator [] (const int idx) throw();
    const char   operator [] (const int idx) const  throw();
    AnsiString&  operator =( const wchar_t* rhs );
    AnsiString&  operator =( const char* rhs );
    AnsiString&  operator =( const AnsiString& rhs ) throw();

    AnsiString&  operator +=( const char* rhs );
    AnsiString&  operator +=( const AnsiString& rhs );
    AnsiString&  operator +=( const char rhs );

    AnsiString   operator +( const char* strB ) const;
    AnsiString   operator +( const AnsiString& strB ) const;

    bool      operator ==( const AnsiString &rhs ) const { return AnsiCompare(rhs) == 0; }
    bool      operator ==( const char* rhs ) const { return AnsiCompare(rhs) == 0; }

    bool      operator !=( const AnsiString &rhs ) const { return AnsiCompare(rhs) != 0; }
    bool      operator !=( const char* rhs ) const { return AnsiCompare(rhs) != 0; }

    bool      operator <( const AnsiString &rhs ) const { return  (AnsiCompare(rhs) < 0); }
    bool      operator <( const char* rhs ) const { return  (AnsiCompare(rhs) < 0); }
    
    bool      operator <=( const AnsiString &rhs ) const { return (AnsiCompare(rhs) <= 0); }
    bool      operator <=( const char* rhs ) const { return (AnsiCompare(rhs) <= 0); }

    bool      operator >( const AnsiString &rhs ) const { return (AnsiCompare(rhs) > 0); }
    bool      operator >( const char* rhs ) const { return (AnsiCompare(rhs) > 0); }

    bool      operator >=( const AnsiString &rhs ) const { return (AnsiCompare(rhs) >= 0); }
    bool      operator >=( const char* rhs ) const { return (AnsiCompare(rhs) >= 0); }
public:
    static AnsiString IntToHex( UInt32 value ) throw();
    static void AnsiStrToLower( char* str ) throw();
    static void AnsiStrToLower( char* dst, const char* src ) throw();
    //static void MoveString(AnsiString& Dst, AnsiString& Src) throw();
    static Int32 StrToInt32( const char* str, int len );
    friend AnsiString operator +( const char* StrA, const AnsiString& StrB);
    static const Int32 MAX_STR_BUFFER = 12288;
};
//---------------------------------------------------------------------------
extern AnsiString operator +( const char* StrA, const AnsiString& StrB);
extern size_t StrLCpy(char *dst, const char *src, size_t siz);
//-----------------------------------------------------------------------------------------
#if (defined(__LINUX) || (defined(__AIX) && defined(__ICONV)))
//------------------------------------------------------------------------------
int TranscodeCharacter(const char *OriginalCodeName, const char *TargetCodeName,
                       char *OriginalDataBuffer, size_t OriginalDataSizeInByte,
                       size_t MaxTargetDataSizeInByte, char *TargetDataBuffer, AnsiString& TranscodeMsg);
//------------------------------------------------------------------------------
int TranscodeCharacter(const char *OriginalCodeName, const char *TargetCodeName,
                       char *OriginalDataBuffer, size_t OriginalDataSizeInByte,
                       AnsiString& TargetStr, AnsiString& TranscodeMsg);
//------------------------------------------------------------------------------
int TranscodeCharacter(const char *OriginalCodeName, const char *TargetCodeName,
                       const char *OriginalDataBuffer, size_t OriginalDataSizeInByte,
                       AnsiString& TargetStr, AnsiString& TranscodeMsg);
//------------------------------------------------------------------------------
int TranscodeCharacter(const AnsiString& OriginalCodeName, const AnsiString& TargetCodeName,
                       const AnsiString& OriginalStr, AnsiString& TargetStr, AnsiString& TranscodeMsg);
#endif
//-----------------------------------------------------------------------------------------
}	/* namespace UFC */
//-----------------------------------------------------------------------------------------
#endif

