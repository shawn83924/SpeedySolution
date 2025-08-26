#ident "@(#) $Id: StringTokenizer.cpp,v 1.6 2004/05/21 10:34:23 ap27 Exp $"
//---------------------------------------------------------------------------
//#include "PFC.h"
#include "AnsiString.h"
#include "List.h"
#include "StringTokenizer.h"
//---------------------------------------------------------------------------
namespace PFC
{
//---------------------------------------------------------------------------
PStringTokenizer::PStringTokenizer () throw()
	:TokenList( new PList() )
	,FDelStr( FALSE )
	,FQuote( FALSE )
	,FWithDelim( FALSE )
{
	CopyStr = new AnsiString( );
}
//---------------------------------------------------------------------------
PStringTokenizer::PStringTokenizer ( char *str, const AnsiString &deli, Boolean quote) throw( AllocException )
	:TokenList( new PList() )
	,FDelStr( FALSE )
	,FQuote( quote )
	,FWithDelim( FALSE )
{
	CopyStr = new AnsiString( );
	CopyStr->Assign( str );
	try
	{
		if ( FQuote )
			Init( deli + AnsiString("\"'") );
		else
	    	Init( deli );
    }
    catch( AllocException& pae )
    {
    	delete CopyStr;
    	delete TokenList;
    	throw pae;
    }
}
//---------------------------------------------------------------------------
PStringTokenizer::PStringTokenizer ( char *str, Boolean quote) throw ( AllocException )
	:TokenList(new PList() )
	,FDelStr( FALSE )
	,FQuote(quote)
	,FWithDelim( FALSE )
{
	CopyStr = new AnsiString( );
	CopyStr->Assign(str);
	try
	{
		if ( FQuote )
			Init( AnsiString(" \n\r\t\f\"'") );
		else
		    Init( AnsiString(" \n\r\t\f") );
    }
    catch( AllocException& pae)
    {
    	delete CopyStr;
    	delete TokenList;
    	throw pae;
    }
}
//---------------------------------------------------------------------------
PStringTokenizer::PStringTokenizer (const AnsiString &str, const AnsiString &deli, 
									Boolean quote )
	throw( AllocException )
	:TokenList(new PList())
	,CopyStr(new AnsiString(str))
	,FDelStr( true )
	,FQuote(quote)
	,FWithDelim(false)
{
	try
	{
		if ( FQuote )
			Init( deli + AnsiString("\"'") );
		else
	    	Init( deli );
    }
    catch (AllocException& pae)
    {
    	delete CopyStr;
    	delete TokenList;
    	throw pae;
    }
}
//---------------------------------------------------------------------------
PStringTokenizer::PStringTokenizer (const AnsiString &str, Boolean quote)
	throw (PAllocException)
	:TokenList(new PList())
	,CopyStr(new AnsiString(str))
	,FDelStr( TRUE )
	,FQuote(quote)
	,FWithDelim( FALSE )
{
	try
	{
		if ( FQuote )
			Init( AnsiString(" \n\r\t\f\"'") );
		else
			Init( AnsiString(" \n\r\t\f") );
    }
    catch ( AllocException& pae)
    {
    	delete CopyStr;
    	delete TokenList;
    	throw pae;
    }
}
//---------------------------------------------------------------------------
PStringTokenizer::~PStringTokenizer() throw()
{
    delete TokenList;
    if( !FDelStr )
    {
    	CopyStr->StrBuffer = NULL;
    	CopyStr->FLength = 0;
	}
	delete CopyStr;
}
//---------------------------------------------------------------------------
void PStringTokenizer::Init( const AnsiString &deli )
	throw ( AllocException)
{
	if (CopyStr->Length())
	{

	   	register UInt16 index = 0;
	   	register Int32  found;
	    PCheckPoint     cp;
	   	const Char*     StrPtr = CopyStr->c_str();
	   	UInt16          charCount;
	   	char            word[2];
	   	word[1] = '\0';

	    CopyStr->SetDelimiter( deli );
	    while( true )
	   	{
   		    found = CopyStr->FirstDelimiter( index );
   		   	if( found == -1 ) // No more delimiter.
       		{
       			cp = PCheckPoint( StrPtr + index , CopyStr->Length() - index );
   				if (cp.Count > 0)
    				TokenList->Add((void*)cp);
   				return;
	       	} 
			else if( FQuote && found == index && 
					(StrPtr[found] == '"' || StrPtr[found] == '\'') )
			{
        		word[0] = StrPtr[found];
        		found ++;
        		Char* foundPos = StrStr( StrPtr + found, word );
        		if ( foundPos )
        		{
        			charCount = (foundPos - (StrPtr + found));
        			cp = PCheckPoint( StrPtr + found , charCount);
        			TokenList->Add( cp );
        			charCount++;
        			index = found + charCount;
        			continue;
        		}
			}
   	    	else if( found != index )
        	{
   	   			cp = PCheckPoint( StrPtr + index , found - index );
       			TokenList->Add( cp );
        		if ( FQuote && 
        			(StrPtr[found] == '"' || StrPtr[found] == '\''))
        		{
       				word[0] = StrPtr[found];
       				found ++;
       				Char* foundPos = StrStr( StrPtr + found, word );
       				if ( foundPos )
       				{
       					charCount = (foundPos - (StrPtr + found));
       					cp = PCheckPoint( StrPtr + found , charCount);
       					TokenList->Add( cp );
       					charCount++;
       					index = found + charCount;
       					continue;
       				}
        		}
	       	}
   		    index = found + 1;
		}
	}
	TokenList->Pack();
}
//---------------------------------------------------------------------------
void PStringTokenizer::SetToken(const char *str, const AnsiString &deli, Boolean quote)
{
	CopyStr->Assign( str );
	FQuote = quote;
	if ( FQuote )
		Init( deli + AnsiString("\"'") );
	else
    	Init( deli );
}
//---------------------------------------------------------------------------
void PStringTokenizer::ReToken(const AnsiString& Delimiter)
{
	TokenList->Clear();
	Init( Delimiter );
}
//---------------------------------------------------------------------------
AnsiString PStringTokenizer::GetToken(Int16 Index, Char *DelimChar)
	throw()
{
	if( Index >= TokenList->Count() )
    	return AnsiString();
	PCheckPoint cp = TokenList->Get(Index);
   	if( DelimChar != NULL )
   		*DelimChar = *( cp.Offset + cp.Count );
   	if ( FWithDelim)
   		return AnsiString(cp.Offset, cp.Count + 1);
   	else
	    return AnsiString(cp.Offset, cp.Count);
}
//---------------------------------------------------------------------------
const Char* PStringTokenizer::GetToken(Int16 Index, Int32& CharCount, Char *DelimChar)
	throw()
{
	if( Index >= TokenList->Count() )
    	return NULL;
	PCheckPoint cp = TokenList->Get(Index);
   	if( DelimChar != NULL )
   		*DelimChar = *( cp.Offset + cp.Count );
   	CharCount = cp.Count;
   	if (FWithDelim)
   		CharCount++;
   	return cp.Offset;
}
//---------------------------------------------------------------------------
AnsiString PStringTokenizer::NextToken(Char *DelimChar) throw()
{
	if( TokenList->Count() )
	{
	    if( !FDelStr && !(TokenList->Count() % 10))
		    Pack();
		PCheckPoint cp = TokenList->Delete(0);
    	if( DelimChar != NULL )
    		*DelimChar = cp.Offset[ cp.Count ];
    	if ( FWithDelim)
	    	return AnsiString (cp.Offset, cp.Count + 1 );
    	else
	    	return AnsiString (cp.Offset, cp.Count );
    }
    return AnsiString();
}
//---------------------------------------------------------------------------
void PStringTokenizer::DeleteTokens(Int16 Count) throw()
{
	while ( TokenList->Count() && Count )
	{
		TokenList->Delete(0);
		Count--;
	}
}
//---------------------------------------------------------------------------
void PStringTokenizer::Pack()
{
	if (TokenList->Count() > 0)
	{
		PCheckPoint cp = TokenList->Get(0);
		register UInt32 diff = cp.Offset - CopyStr->StrBuffer;
		if (diff)
		{
			CopyStr->Delete(0, diff);
			cp.Offset -= diff;
			TokenList->Set(0, cp);
			for ( register int i = 1 ; i < TokenList->Count() ; i++ )
			{
				cp = TokenList->Get(i);
				cp.Offset -= diff;
				TokenList->Set(i, cp);
			}
		}
	}
	else
	{
		CopyStr->SetLength(0);
	}
}
//---------------------------------------------------------------------------
Boolean PStringTokenizer::SeparateString(const AnsiString &Source,
										 const Char Delimiter,
										 AnsiString &String1,
										 AnsiString &String2,
										 Boolean Inclusive)
{
    Int16 Index;

    Index = Source.FirstDelimiter(AnsiString(Delimiter));
    if (Index > 1)
    {
        String1 = Source.SubString(1, Index - 1);
        if (Inclusive)
            String2 = Source.SubString(Index , Source.Length());
        else
            String2 = Source.SubString(Index + 1, Source.Length());
        if (String2.Length())
            return true;
        return false;
    }
    else
    {
        String1 = Source;
        return false;
    }
}
//---------------------------------------------------------------------------
Boolean PStringTokenizer::SeparateString(const AnsiString &Source,
										 const Int16 Index,
										 AnsiString &String1,
										 AnsiString &String2,
										 Boolean Inclusive)
{
    if (Index <= 0)
    {
        String1 = Source;
        String2.SetLength(0);
        return false;
    }

    String1 = Source.SubString(0, Index);

    if (Inclusive)
        String2 = Source.SubString(Index , Source.Length());
    else
        String2 = Source.SubString(Index + 1, Source.Length());
    if (String2.Length())
        return true;
    return false;
}
//---------------------------------------------------------------------------
void PStringTokenizer::ReplaceChars(AnsiString &Replace,
									const AnsiString RepChars,
									char ToRep)
{
	register int pos;
	register int i = 1;

	Replace.SetDelimiter( RepChars );
	if (ToRep == '\0')
	{
		while (true)
		{
			if ( i )
			{
				pos = Replace.FirstDelimiter();
				i--;
			}
			else
			{
				pos = Replace.LastDelimiter();
				i++;
			}
			if (pos != -1)
				Replace.Delete(pos, 1);
			else
				break;
		}
	}
	else
	{
		while (true)
		{
			if (i)
			{
				pos = Replace.FirstDelimiter();
				i--;
			}
			else
			{
				pos = Replace.LastDelimiter();
				i++;
			}
			if (pos != -1)
				Replace[pos] = ToRep;
			else
				break;
		}
	}
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
