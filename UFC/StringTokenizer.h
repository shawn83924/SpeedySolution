#ident "@(#) $Id: StringTokenizer.h,v 1.6 2004/05/21 10:34:23 ap27 Exp $"
//---------------------------------------------------------------------------
// This is a part of the Palm Foundation Classes Library for Palm OS (R) platforms.
// Copyright (c) 2002 MobileDigital Development Inc.
// All rights reserved.
//
// Description: String Tokenizing Class. Token string by delimiters.
// Author: Simon Chang and Nash Tsai
//
// This source code is only intended as a supplement to the
// Object Class Library for Palm OS (R) platforms Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Object Class Library for Palm OS (R) platforms product.
//---------------------------------------------------------------------------
#ifndef StringTokenizer
#define StringTokenizer
//---------------------------------------------------------------------------
#include "Type.h"
#include "AnsiString.h"
#include "PList.h"
#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
class PCheckPoint
{
public:
	const Char* Offset;
	Int32       Count;
	PCheckPoint( void ):Offset( NULL ),Count( 0 ){};
	PCheckPoint( const Char *offset, Int32 CharCount ):Offset( offset ),Count( CharCount ){}
};
//---------------------------------------------------------------------------
class PStringTokenizer
{
private:
	void Init(const AnsiString &deli) throw( AllocException );
	void Pack();

	PList*       TokenList;
	AnsiString  *CopyStr;
    BOOL     	 DelimFound;
    BOOL     	 FDelStr;
    BOOL 		 FQuote;
    BOOL		 FWithDelim;

public:
    static BOOL SeparateString(const AnsiString &Source,
								  const Char Delimiter,
								  AnsiString &String1,
								  AnsiString &String2,
								  Boolean Inclusive);
    static BOOL SeparateString(const AnsiString &Source,
    							  const Int16 Index,
    							  AnsiString &String1,
    							  AnsiString &String2,
    							  Boolean Inclusive);
    static void ReplaceChars(AnsiString &Replace, const AnsiString RepChar,
    						 char ToRep);

	PStringTokenizer () throw();
    PStringTokenizer (const AnsiString &str, const AnsiString& deli, Boolean quote = false) throw (AllocException);
    PStringTokenizer (const AnsiString &str, Boolean quote = false) throw (AllocException);
    PStringTokenizer (char* str, const AnsiString& deli, Boolean quote = false) throw (AllocException);
    PStringTokenizer (char* str, Boolean quote = false) throw (AllocException); 
    ~PStringTokenizer() throw();

	void  SetWithDelimiter(BOOL withDelim) { FWithDelim = withDelim; }
    BOOL  HasMoreTokens() throw() { return (TokenList->Count() != 0); }
    Int16 CountTokens() throw() { return TokenList->Count(); }
    AnsiString NextToken(Char *DelimChar = NULL) throw();
    AnsiString GetToken(Int16 Index, Char *DelimChar = NULL) throw();
    const Char* GetToken(Int16 Index, Int32& CharCount, Char *DelimChar = NULL ) throw();
	BOOL AnyDelim() throw() { return DelimFound; }

	void DeleteTokens(Int16 Count) throw();

	void SetToken(const char *str, const AnsiString &deli, BOOL quote);
	void ReToken(const AnsiString& Delimiter);
};
//---------------------------------------------------------------------------
}	/* namespace PFC */
//---------------------------------------------------------------------------
#endif

