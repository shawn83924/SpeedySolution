#ifdef _WIN32

#else
    #ident "@(#) $Id: AnsiString.cpp,v 1.12 2004/05/21 10:34:23 ap27 Exp $"
#endif

#include "AnsiString.h"
#include "StringBuffer.h"
#include "Stream.h"
#include "NetUtility.h"
#include "BufferedLog.h"
//------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------
UInt8*  AnsiString::DelimiterLUT = NULL;
char*   AnsiString::WCToMBBuffer = NULL;
//------------------------------------------------------------------------------
PInitAnsiString  InitAnsiString;
//------------------------------------------------------------------------------
PInitAnsiString::PInitAnsiString()
{
    if( AnsiString::DelimiterLUT == NULL )
        AnsiString::DelimiterLUT = new UInt8[ 256 ];
    if( AnsiString::WCToMBBuffer == NULL )
        AnsiString::WCToMBBuffer = new char[ MB_CUR_MAX ];                     
    // Internationalized programs must call setlocale() to initiate a specific language operation.
    // setlocale( LC_ALL, "");    
}
//------------------------------------------------------------------------------
PInitAnsiString::~PInitAnsiString()
{
    if( AnsiString::DelimiterLUT == NULL )
        delete [] AnsiString::DelimiterLUT;
    if( AnsiString::WCToMBBuffer == NULL )
        delete [] AnsiString::WCToMBBuffer;
}
//------------------------------------------------------------------------------
AnsiString AnsiString::IntToHex(UInt32 value) throw()
{
    char s[11];

    sprintf(s,"%x",value);
    return AnsiString(s);
}
//------------------------------------------------------------------------------
int AnsiString::LengthToMBStr( const wchar_t* src )
{
    register int      MBStrLen = 0;
    register int      NeedSize;
    register wchar_t* Str;

    if( WCToMBBuffer == NULL )
            WCToMBBuffer = new char[ MB_CUR_MAX ];
    for( Str = (wchar_t*)src; *Str != 0; Str++ )
    {
       NeedSize = wctomb( WCToMBBuffer, *Str );
       if( NeedSize == -1 )
               return 0;
       MBStrLen += NeedSize;
    }
    return MBStrLen;
}
//------------------------------------------------------------------------------
AnsiString::AnsiString( const wchar_t* src)
:StrBuffer( NULL )
,FLength( 0 )
{
    if( src != NULL )
    {
        size_t NeedSize = LengthToMBStr( src );///< returns the required size of the destination string.

        if( NeedSize > 0 )
        {
            try
            {
                size_t StrSize;

                StrBuffer = new char[ NeedSize + 1 ];
                StrSize   = wcstombs( StrBuffer, src, NeedSize );
                if( StrSize == (size_t)-1 )
                    throw exception();
                FLength = (UFCType::UInt16)StrSize;
                StrBuffer[ FLength ] = 0;
            }
            catch( ... ) ///< Alloc memory failed.
            {
                if( StrBuffer != NULL )
                    delete [] StrBuffer;
                StrBuffer = NULL;
                FLength = 0;
            }
        }
    }
}
//---------------------------------------------------------------------------
AnsiString::AnsiString(const char* src)
:StrBuffer( NULL )
{
    if( src != NULL )
    {
        FLength = (UFCType::UInt16)strlen( src );
        if( FLength )
        {
            try
            {
                StrBuffer = new char[ FLength + 1 ];
                memcpy(StrBuffer, src, FLength + 1);
            }
            catch( ... )
            {
                StrBuffer = NULL;
                FLength = 0;
            }
        }
    }
    else
    {
        FLength = 0;
    }
}
//---------------------------------------------------------------------------
AnsiString::AnsiString( const AnsiString& src)
:StrBuffer( NULL )
{
    FLength = src.FLength;
    if( FLength )
    {
        try
        {
            StrBuffer = new char [FLength + 1];
            memcpy( StrBuffer, src.StrBuffer, FLength + 1);
        }
        catch( ... )
        {
            StrBuffer = NULL;
            FLength = 0;
        }
    }
}
//---------------------------------------------------------------------------
AnsiString::AnsiString( const char* src, Int32 len )
:StrBuffer( NULL ),FLength(0)
{
    if( src != NULL )
    {
        UInt16 length = StrNLen( src, len  );
        
        if( length < len )
            len = length;
        FLength = len;
        if( FLength )
        {
            try
            {
                StrBuffer = new char [FLength + 1];
                memcpy( StrBuffer, src, FLength );
                StrBuffer[FLength] = '\0';
                return;
            }
            catch( ... )
            {
                ///< Memory allocation failed.
            }
        }
    }
    StrBuffer = NULL;
    FLength = 0;
}
//---------------------------------------------------------------------------
AnsiString::AnsiString( char src )
{
    if (src == '\0')
    {
        FLength = 0;
        StrBuffer = NULL;
    }
    else
    {
        FLength = 1;
        try
        {
            StrBuffer = new char [2];
            *( StrBuffer )     = src;
            *( StrBuffer + 1 ) = '\0';
        }
        catch( ... )
        {
            FLength = 0;
            StrBuffer = NULL;
        }
    }
}
//---------------------------------------------------------------------------
AnsiString::AnsiString( Int32 num )
{
    try
    {
        StrBuffer = new char [ 11 ];
        sprintf(StrBuffer,"%d",num);
        FLength = (UFCType::UInt16)strlen(StrBuffer);
    }
    catch( ... )
    {
        FLength = 0;
        StrBuffer = NULL;
    }
}
//---------------------------------------------------------------------------
AnsiString::AnsiString( Int64 num )
{
    try
    {
        StrBuffer = new char [ 21 ];
        sprintf(StrBuffer,"%lld", num);
        FLength = (UFCType::UInt16)strlen(StrBuffer);
    }
    catch( ... )
    {
        FLength = 0;
        StrBuffer = NULL;
    }
}
//---------------------------------------------------------------------------
AnsiString::~AnsiString()
{
    if( StrBuffer != NULL )
        delete []StrBuffer;
}
//---------------------------------------------------------------------------
void AnsiString::Append( const char *Str, const UInt16 StrLen )
{
	if (!FLength)
	{
		if( StrLen )
		{
			StrBuffer = new char [ StrLen + 1 ];
			memcpy( StrBuffer, Str, StrLen );
			StrBuffer[ StrLen ] = '\0';
			FLength = StrLen;
		}
	}
	else
	{
		SetSize( FLength + StrLen );
		memcpy( StrBuffer + FLength, Str, StrLen);
		FLength += StrLen;
		StrBuffer[ FLength ] = '\0';
	}
}
//---------------------------------------------------------------------------
void AnsiString::SetSize(UInt16 Size, bool KeepData)
{
	if (FLength)
	{
		char *temp = new char [Size + 1];

		if ( KeepData )
		{
			if( FLength <= Size ) //grow
			{
				strcpy( temp, StrBuffer );
			}
			else //shrink
			{
				memcpy( temp, StrBuffer, Size );
			}
		}
		delete[]StrBuffer;
		StrBuffer = temp;
		StrBuffer[ Size ] = '\0';
	}
	else
	{
		StrBuffer = new char [Size + 1];
		StrBuffer[0] = '\0';
	}
}
//---------------------------------------------------------------------------
UInt16 AnsiString::StrNLen( const char *Str, const UInt16 MaxLength )
{
    for( register Int32 i = 0; i < MaxLength; i++ )
    {
        if( *(Str+i) == '\0' )
            return i;
    }
    return MaxLength;
}
//---------------------------------------------------------------------------
int AnsiString::AnsiCompare( const char* rhs ) const throw()
{
	if (rhs && FLength)
	{
		return strcmp(StrBuffer, rhs);
	}
	else
	{
		if (!rhs && !FLength )
			return 0;
		else if (FLength)
			return 1;
		else
			return -1;
	}
}
//---------------------------------------------------------------------------
int AnsiString::AnsiCompare( const AnsiString& rhs) const throw()
{
	return AnsiCompare(rhs.StrBuffer);
}
//---------------------------------------------------------------------------
int	AnsiString::AnsiNCompare( const char* src, Int16 Count ) const throw()
{
    if( StrBuffer == NULL )
        return -1;
    else
        return strncmp( StrBuffer, src, Count);
}
//---------------------------------------------------------------------------
int	AnsiString::AnsiNCompareIC( const char* src, Int16 Count ) const throw()
{
    if( StrBuffer == NULL )
        return -1;
    else
        return strnicmp( StrBuffer, src, Count );
}
//---------------------------------------------------------------------------
int AnsiString::AnsiCompareIC( const char* rhs ) const throw()
{
	if (rhs && FLength)
	{
		return stricmp(StrBuffer, rhs);
	}
	else
	{
		if (!rhs && !FLength)
			return 0;
		else if (FLength)
			return 1;
		else
			return -1;
	}
}
//---------------------------------------------------------------------------
int AnsiString::AnsiCompareIC( const AnsiString& rhs) const throw()
{
	return AnsiCompareIC(rhs.StrBuffer);
}
//---------------------------------------------------------------------------
Int32 AnsiString::AnsiPos( const AnsiString& subStr) const throw()
{
	return AnsiPos( subStr.StrBuffer );
}
//---------------------------------------------------------------------------
Int32 AnsiString::AnsiPos( const char *subStr) const throw()
{
	if (StrBuffer == NULL)
	{
		return -1;
	}
	else
	{
		char *pos= strstr( StrBuffer, subStr );
		if( pos == NULL)
			return -1;
		else
			return ( pos - StrBuffer );
	}
}
//---------------------------------------------------------------------------
Int32 AnsiString::AnsiPos( const char subChar) const throw()
{
	if (StrBuffer == NULL)
	{
		return -1;
	}
	else
	{
		char* pos = strchr (StrBuffer, subChar);
		if( pos == NULL)
			return -1;
		else
			return ( pos - StrBuffer );
	}
}
//---------------------------------------------------------------------------
AnsiString&  AnsiString::Delete(UInt16 index, UInt16 count)
{
	if ( index < FLength && count )
	{
		char *start = StrBuffer + index;
		if ( index + count < FLength )
		{
			// buffer overlap need use memmove()
			// memmove() ensures that the original source bytes in the overlapping region are copied before being overwritten.
			// memmove() are copied in a preserving manner (unlike memcpy() and strcpy())
			memmove( start, start+count, (FLength-index-count+1) );
		}
		else
			start[0] = '\0';

		FLength = (UFCType::UInt16)strlen(StrBuffer);
		if ( FLength )
		{
			char *temp = new char [FLength + 1];

			memcpy(temp, StrBuffer, FLength);
			temp[FLength] = '\0';
			delete [] StrBuffer;
			StrBuffer = temp;
		}
		else
		{
			delete [] StrBuffer;
			StrBuffer = NULL;
		}
	}
	return *this;
}
//---------------------------------------------------------------------------
bool AnsiString::IsDelimiter( const AnsiString& delimiters, UInt16 index) const throw()
{
	if( index < FLength )
	{
		for( register int i = 0; i < delimiters.FLength; i++ )
		{
			if( *(StrBuffer + index ) ==  *( delimiters.StrBuffer + i ) )
				return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
void AnsiString::SetDelimiter( const AnsiString& delimiters) const throw()
{
	register UInt8 *DeliPtr;

	memset( DelimiterLUT, 0, 256 );

	DeliPtr = ( UInt8* ) delimiters.StrBuffer;
	for( register Int16 i = 0; i < delimiters.FLength; i++ )
	{
		 DelimiterLUT[ *DeliPtr ] = 1;
		 DeliPtr++;
	}
}
//---------------------------------------------------------------------------
Int32 AnsiString::FirstDelimiter( UInt16 index ) const throw()
{
	for( register Int32 i = index; i < FLength ; i++ )
		 if( DelimiterLUT[ (UInt8)*( StrBuffer + i ) ] == 1)
	    	 return i;
	return -1;
}
//---------------------------------------------------------------------------
Int32 AnsiString::FirstDelimiter( void ) const throw()
{
	for( register Int32 i =0; i < FLength ; i++ )
	     if( DelimiterLUT[ (UInt8)*( StrBuffer + i) ] == 1 )
	    	 return i;
	return -1;
}
//---------------------------------------------------------------------------
Int32 AnsiString::LastDelimiter( void ) const throw()
{
	for( register Int32 i = FLength - 1; i >= 0; i-- )
	     if( DelimiterLUT[ (UInt8)*( StrBuffer + i) ] == 1)
	     	 return i;
	return -1;
}
//---------------------------------------------------------------------------
Int32 AnsiString::FirstDelimiter( UInt16 index, const AnsiString& delimiters ) const throw()
{
	SetDelimiter( delimiters );

	for( register Int32 i = index; i < FLength ; i++ )
	     if( DelimiterLUT[ (UInt8)*( StrBuffer + i ) ] == 1)
	    	 return i;
	return -1;
}
//---------------------------------------------------------------------------
Int32 AnsiString::FirstDelimiter( const AnsiString& delimiters ) const throw()
{
	SetDelimiter( delimiters );

	for( register Int32 i =0; i < FLength ; i++ )
	     if( DelimiterLUT[ (UInt8)*( StrBuffer + i) ] == 1 )
	    	 return i;
	return -1;
}
//---------------------------------------------------------------------------
Int32 AnsiString::LastDelimiter( const AnsiString& delimiters ) const throw()
{
	SetDelimiter( delimiters );

	for( register Int32 i = FLength - 1; i >= 0; i-- )
		 if( DelimiterLUT[ (UInt8)*( StrBuffer + i) ] == 1)
	     	 return i;
	return -1;
}
//------------------------------------------------------------------------------------------
AnsiString AnsiString::UpperCase() const
{
	UInt8          *temp   = new UInt8[ FLength + 1];
	register UInt8 *Ptr    = temp;
	register UInt8 *StrPtr = ( UInt8* ) StrBuffer;

	for( register Int16 i = 0; i < FLength; i++ )
		*( Ptr++ ) = toupper( *( StrPtr++ ) );
	*Ptr = 0;
	AnsiString ret;
	ret.StrBuffer = ( char* ) temp;
	ret.FLength = FLength;
	return ret;
}
//---------------------------------------------------------------------------
AnsiString& AnsiString::PadThis( Int32 PadLength, char PadChar, bool PadRight )
{
	if( PadLength == FLength ) ///< Same size, skip.
	{
		return *this;
	}
	else if( PadLength == 0 ) ///< Set to Empty string.
	{
		if( StrBuffer != NULL )
			delete []StrBuffer;
		StrBuffer = NULL;
		FLength = 0;
		return *this;
	}
	else
	{
		if( PadRight == true ) ///< Pad right
		{
			if( PadLength < FLength )
				SetLength( PadLength );
			else if( PadLength > FLength )
			{
				char *Temp = new char [ PadLength + 1 ];

				memset( Temp, PadChar, PadLength );
				Temp[ PadLength ] = '\0';
				if( StrBuffer != NULL )
				{
					memcpy( Temp, StrBuffer, FLength );
					delete []StrBuffer;
				}
				StrBuffer = Temp;
				FLength = PadLength;
			}
		}
		else ///< Pad left
		{
			char *Temp = new char [ PadLength + 1 ];
			int   Offset = PadLength - FLength;

			memset( Temp, PadChar, PadLength );
			Temp[ PadLength ] = '\0';
			if( StrBuffer != NULL )
			{
				if( Offset < 0 )
					memcpy( Temp, StrBuffer - Offset, PadLength  );
				else
					memcpy( Temp + Offset, StrBuffer, PadLength - Offset );
				delete []StrBuffer;
			}
			StrBuffer = Temp;
			FLength = PadLength;
		}
		return *this;
	}
}
//---------------------------------------------------------------------------
AnsiString& AnsiString::UpperThis( void ) throw()
{
	register UInt8* Ptr = ( UInt8* )StrBuffer;

	for( register Int16 i = 0; i < FLength; i++ )
	{
		if( islower( *( Ptr ) ))
			*( Ptr ) = toupper(*( Ptr ));
		Ptr++;
	}
	return *this;
}
//---------------------------------------------------------------------------
AnsiString& AnsiString::LowerThis( void ) throw()
{
	register UInt8* Ptr = ( UInt8* )StrBuffer;

	for( register Int16 i = 0; i < FLength; i++ )
	{
		if( isupper( *( Ptr ) ))
			*( Ptr ) = tolower(*( Ptr ));
		Ptr++;
	}
	return *this;
}
//---------------------------------------------------------------------------
void AnsiString::AnsiStrToLower( char* str ) throw()
{
	AnsiStrToLower(str, str);
}
//---------------------------------------------------------------------------
void AnsiString::AnsiStrToLower( char* dst, const char* src ) throw()
{
	UInt16 Length = (UFCType::UInt16)strlen( src );
	UInt8* SrcPtr = (UInt8* )src;
	UInt8* DstPtr = (UInt8* )dst;

	for( register UInt16 i = 0; i < Length; i++ )
	{
		*( DstPtr ) = tolower( *( SrcPtr ) );
		SrcPtr++;
		DstPtr++;
	}
}
//---------------------------------------------------------------------------
/*void AnsiString::MoveString(AnsiString& Dst, AnsiString& Src) throw()
{
	if ( &Dst != &Src )
	{
		Dst.SetLength(0);
		memcpy(&Dst, &Src, sizeof(AnsiString));
		memset(&Src, 0, sizeof(AnsiString));
	}
}*/
//---------------------------------------------------------------------------
AnsiString AnsiString::LowerCase() const
{
	UInt8          *temp   = new UInt8[ FLength + 1];
	register UInt8 *Ptr    = temp;
	register UInt8 *StrPtr = ( UInt8* ) StrBuffer;

	for( register Int16 i = 0; i < FLength; i++ )
		*( Ptr++ ) = tolower( *( StrPtr++ ) );
	*Ptr = 0;
	AnsiString ret;
	ret.StrBuffer = ( char* ) temp;
	ret.FLength = FLength;
	return ret;
}
//---------------------------------------------------------------------------
AnsiString& AnsiString::SetLength(UInt16 length )
{
	if( length < FLength )
	{
		if( FLength )
		{
			if (length == 0)
			{
				delete []StrBuffer;
				StrBuffer = NULL;
			}
			else
			{
				char *Temp = new char [length + 1];
				memcpy(Temp, StrBuffer, length);
				delete []StrBuffer;
				StrBuffer = Temp;
				StrBuffer[length] = '\0';
			}
			FLength = length;
		}
	}
    else
    {
        SetSize( length, TRUE );
        FLength = length;
    }
	return *this;
}
//---------------------------------------------------------------------------
void AnsiString::Copy( const char* src, int len )
{
    if( len <= 0 )
    {
        if( StrBuffer != NULL ) //if ( FLength > 0 )
           delete []StrBuffer;
        StrBuffer = NULL;
        FLength = 0;
        return;
    }
    else
    {
        if( StrBuffer != NULL ) //if( FLength > 0 && StrBuffer != NULL )
            delete [] StrBuffer;
        StrBuffer = new char [ len + 1];
    }
    memcpy( StrBuffer, src, len );				
    StrBuffer[ len ] = '\0';
    FLength = len;        
}
//---------------------------------------------------------------------------
AnsiString  AnsiString::SubString(UInt16 index, UInt16 count) const
{
	if (index < FLength)
	{
		if ((index + count) > FLength)
			count = FLength - index;
		return AnsiString (StrBuffer + index, count);
	}
	return AnsiString();
}
//---------------------------------------------------------------------------
AnsiString  AnsiString::SubLast( UInt16 count) const
{
	int Index = FLength - count;

	if( count == 0 )
		return AnsiString();
	if( Index < 0 )
		Index = 0;
	return AnsiString( StrBuffer + Index );
}
//---------------------------------------------------------------------------
bool AnsiString::EndWith(const char* EndingStr) const throw()
{
	if ( EndingStr )
	{
		Int16 sLen = (UFCType::UInt16)strlen(EndingStr);
		if ( sLen && sLen <= FLength )
			return strncmp( StrBuffer + FLength - sLen, EndingStr, sLen ) == 0;
	}
	return false;
}
//---------------------------------------------------------------------------
bool AnsiString::EndWithIC(const char* EndingStr) const throw()
{
	if ( EndingStr )
	{
		Int16 sLen = (UFCType::UInt16)strlen(EndingStr);
		if ( sLen && sLen <= FLength )
			return strnicmp( StrBuffer + FLength - sLen, EndingStr, sLen ) == 0;
	}
	return false;

}
//---------------------------------------------------------------------------
AnsiString::operator size_t() const
{
	size_t hashval = 5381;
	char *p = (char*)StrBuffer;
	char *e = p + FLength;

	while( p != e )
			hashval = ((hashval << 5) + hashval) + (*p++);
	return hashval;
}
//---------------------------------------------------------------------------
char& AnsiString::operator [](const int idx)  throw()
{
	if (idx < FLength && idx >= 0)
		return StrBuffer[idx];
	else
		return StrBuffer[FLength];
}
//---------------------------------------------------------------------------
const char AnsiString::operator [] (const int idx) const  throw()
{
	if (idx < FLength && idx >= 0)
		return StrBuffer[idx];
	else
		return StrBuffer[FLength];
}
//---------------------------------------------------------------------------
AnsiString& AnsiString::operator =( const wchar_t* rhs )
{
	size_t NeedSize = wcstombs( NULL, rhs, 0 );///< returns the required size of the destination string.

	if( NeedSize > 0 && NeedSize != (size_t)-1 )
	{
		try
		{
			if( FLength < (UFCType::UInt16)NeedSize )
				SetSize( (UFCType::UInt16)NeedSize, false ); ///< Resize string buffer
			if( wcstombs( StrBuffer, rhs, NeedSize ) == (size_t)-1 )
				throw exception();
			FLength = (UFCType::UInt16)NeedSize;
			StrBuffer[ FLength ] = 0;
		}
		catch( ... ) ///< Alloc memory failed.
		{
			if( StrBuffer != NULL )
				delete[] StrBuffer;
			StrBuffer = NULL;
			FLength = 0;
		}
	}
	else
    {
		if( StrBuffer != NULL )
		{
			delete[] StrBuffer;
			StrBuffer = NULL;
		}
		FLength = 0;
    }
    return *this;
}
//---------------------------------------------------------------------------
AnsiString& AnsiString::operator =( const char* rhs )
{   
    if( rhs == StrBuffer )
        return *this;
    if( rhs != NULL )
    {
        UInt16 len = (UFCType::UInt16)strlen( rhs );
        if( len == 0 ) ///< the given string length = 0
        {
			if( StrBuffer != NULL )
			{
				delete[] StrBuffer;
				StrBuffer = NULL;
            }
            FLength = 0;
        }
        else
        {
			if( FLength >= len )
			{
                memcpy( StrBuffer, rhs, len + 1);
                FLength = len;
            }
            else
            {
				SetSize( len, false );
                memcpy( StrBuffer, rhs, len + 1);
                FLength = len;
            }
        }
    }
    else
    {
        if( StrBuffer != NULL )
        {
    	    delete[] StrBuffer;
		    StrBuffer = NULL;
        }
		FLength = 0;
    }
    return *this;
}
//---------------------------------------------------------------------------
AnsiString& AnsiString::operator =( const AnsiString& rhs ) throw()
{
    if( this != &rhs )
    {
        if( FLength > 0 && StrBuffer != NULL )
        {
            delete [] StrBuffer;
            StrBuffer = NULL;
        }
        FLength = rhs.FLength;
        if( FLength )
        {
            try
            {
                StrBuffer = new char [FLength + 1];
                memcpy( StrBuffer, rhs.StrBuffer, FLength + 1);
            }
            catch( ... )
            {
                StrBuffer = NULL;
                FLength = 0;
            }
        }
    }
    return *this;
}
//---------------------------------------------------------------------------
AnsiString& AnsiString::operator += ( const AnsiString& StrB )
{
	Append(StrB.StrBuffer, StrB.FLength);
    return *this;
}
//---------------------------------------------------------------------------
AnsiString& AnsiString::operator += ( const char* rhs )
{
	if (rhs)
		Append(rhs, (UFCType::UInt16)strlen(rhs));
    return *this;
}
//---------------------------------------------------------------------------
AnsiString& AnsiString::operator +=( const char rhs )
{
	if (rhs != 0)
	{
		char str[2];
		str[1] = 0;
		str[0] = rhs;
		Append(str, 1);
	}
	return *this;
}
//---------------------------------------------------------------------------
AnsiString operator +( const char* StrA, const AnsiString& StrB)
{
    PStringBuffer strBuffer;
    strBuffer.Append(StrA).Append(StrB);
	return strBuffer.c_str();
}
//---------------------------------------------------------------------------
AnsiString AnsiString::operator +( const char* StrB ) const
{
	if (StrB)
	{
		PStringBuffer strBuffer(*this);
		strBuffer.Append(StrB);
    	return strBuffer.c_str();
	}
	else
	{
		return *this;
	}
}
//---------------------------------------------------------------------------
AnsiString AnsiString::operator +( const AnsiString& StrB) const
{
	if (StrB.Length())
	{
		PStringBuffer strBuffer(*this);
		strBuffer.Append(StrB.StrBuffer, 0, StrB.FLength);
	    return strBuffer.c_str();
	}
	else
	{
		return *this;
	}
}
//---------------------------------------------------------------------------
UInt16 AnsiString::AppendPrintf( const char* FormatStr, ... )
{
	UInt16   Count;
	char     Buffer[MAX_STR_BUFFER];
    va_list  va;

    va_start( va , FormatStr );
    Count = vsnprintf( Buffer, MAX_STR_BUFFER, FormatStr,va);
    va_end( va );
    Append( Buffer, (UFCType::UInt16)strlen( Buffer ) );
	return Count;
}
//---------------------------------------------------------------------------
UInt16 AnsiString::Printf( const char* FormatStr, ... )
{
    Int32       CharCount;
    va_list     va;

    va_start( va , FormatStr );
#ifndef _WIN32//__LINUX
    va_list     vacpy;
    va_copy( vacpy, va );
    CharCount = vsnprintf( NULL, 0, FormatStr, vacpy );
    va_end( vacpy );
#else
	CharCount = vsnprintf( NULL, 0, FormatStr, va );
#endif
	if( CharCount <= 0 )
	{
		if( StrBuffer != NULL && FLength != 0 )
			delete [] StrBuffer;
		StrBuffer = NULL;
		FLength = 0;
		va_end( va );
		return 0;
	}
	if( CharCount > FLength )
	{
		if( StrBuffer != NULL && FLength != 0 )
			delete [] StrBuffer;
		FLength = CharCount;
		StrBuffer = new char[ FLength + 1 ];
	}
	else
		FLength = CharCount;
	CharCount = vsnprintf( StrBuffer, FLength + 1, FormatStr, va );
	va_end( va );
    return (UInt16)CharCount;
}
//---------------------------------------------------------------------------
void AnsiString::TrimLeft( char TrimCh  )
{
    if( StrBuffer != NULL )
    {
    	UInt16  Pos = 0;
	    char    ch;

    	for( register UInt16 i = 0; i< FLength; i++ )
		{
		    ch = *(StrBuffer + i);
			if( ch == '\t' ||
				ch == ' '  ||
				ch == TrimCh )
	    		Pos++;
		    else
			    break;
    	}
		if ( Pos )
		{
    		FLength -= Pos;
			// buffer overlap need use memmove()
			// memmove() ensures that the original source bytes in the overlapping region are copied before being overwritten.
			// memmove() are copied in a preserving manner (unlike memcpy() and strcpy())
			memmove( StrBuffer, StrBuffer + Pos, FLength+1 );
		}
    }
}
//---------------------------------------------------------------------------
void AnsiString::TrimRight( char TrimCh )
{
	if( StrBuffer != NULL )
    {
    	UInt16  Pos = 0;
		char    ch;

   		for( register Int32 i = FLength - 1; i >= 0; i-- )
		{
			ch = *(StrBuffer + i);
			if( ch == ' '  || ///< Space
				ch == 0x09 || ///< Tab   '\t'  9
				ch == 0x0A || ///< LF    '\r' 10
				ch == 0x0D || ///< CR    '\n' 13
				TrimCh == ch )
				Pos++;
			else
				break;
		}
		FLength -= Pos;
		*(StrBuffer + FLength) = 0;
	}
}
//---------------------------------------------------------------------------
void AnsiString::LoadFromStream( PStream* Stream )
{
    NInt32  StringLength;

    StringLength.LoadFromStream( Stream ); ///< Read String length from Stream
    if( FLength > 0 && StrBuffer != NULL ) ///< Release old string buffer
        delete [] StrBuffer;
    FLength = (UInt16)StringLength.ToInt32();
    if( FLength > 0  ) ///< Not empty string
    {
	StrBuffer = new char[ FLength ];
        Stream->Read( StrBuffer, FLength );
        FLength -= 1;
    }
    else ///< Empty String
    {
        FLength = 0;
        StrBuffer = NULL;
    }
}
//---------------------------------------------------------------------------
void AnsiString::SaveToStream( PStream* Stream ) const
{
    if( FLength == 0)
    {
        NInt32  StringLength( 0 );

        StringLength.SaveToStream( Stream );
    }
    else
    {
        Int32   SaveSize = FLength + 1;
        NInt32  StringLength( SaveSize );

        StringLength.SaveToStream( Stream );
        Stream->Write( StrBuffer, SaveSize );
    }
}
//---------------------------------------------------------------------------
double AnsiString::ToDouble() const
{
    const char * i = StrBuffer;
    // Catch null strings
    if(i==NULL) return 0.0 ;
    if( !*i )   return 0.0;
    // Eat leading '-' and recheck for null string
    if( *i == '-' && !*++i ) return 0.0;
    // Eat leading '+' and recheck for null string
    if( *i == '+' && !*++i ) return 0.0;

    bool haveDigit = false;

    if( isdigit(*i) )
    {
      haveDigit = true;
      while( isdigit (*++i) );
    }

    if( *i == '.' && isdigit(*++i) )
    {
      haveDigit = true;
      while( isdigit (*++i) );
    }

    if( *i || !haveDigit ) return 0.0;
    return strtod( StrBuffer, 0 );
}
//---------------------------------------------------------------------------
Int64 AnsiString::ToInt64() const
{
#ifdef _WIN32
    return ( Int64 )_atoi64( StrBuffer );
#else
    return ( Int64 )strtoll( StrBuffer, (char **)NULL, 10 );
#endif	
}
//---------------------------------------------------------------------------
UInt64 AnsiString::ToUInt64() const
{
//#ifdef _WIN32
//    return ( UInt64 )_strtoui64( StrBuffer , (char **)NULL, 10 );
//#else
    return ( UInt64 )strtoull( StrBuffer , (char **)NULL, 10 );
//#endif	
}
//---------------------------------------------------------------------------
Int32 AnsiString::StrToInt32( const char* str, int len )
{
    char Buf[ 16 ];
    if( len <= 0 || len > 15 )
        return 0;    
    memcpy( Buf, str, len );
    Buf[ len ] = 0;
    return atoi( Buf );
}
//---------------------------------------------------------------------------
//
// Copy src to string dst of size siz.  At most siz-1 characters
// will be copied.  Always NUL terminates (unless siz == 0).
// Returns strlen(src); if retval >= siz, truncation occurred.
//
//---------------------------------------------------------------------------
size_t StrLCpy( char *dst, const char *src, size_t siz )
{
	register char *d = dst;
	register const char *s = src;
	register size_t n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0 && --n != 0) {
		do {
			if ((*d++ = *s++) == 0)
				break;
		} while (--n != 0);
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = '\0';		/* NUL-terminate dst */
		while (*s++)
			;
	}

	return(s - src - 1);	/* count does not include NUL */
}
//---------------------------------------------------------------------------
#if (defined(__LINUX) || (defined(__AIX) && defined(__ICONV)))
int TranscodeCharacter(const char *OriginalCodeName, const char *TargetCodeName,
                       char *OriginalDataBuffer, size_t OriginalDataSizeInByte,
                       size_t MaxTargetDataSizeInByte, char *TargetDataBuffer, AnsiString& TranscodeMsg)
{
    int convertCount = 0;
    if (OriginalDataSizeInByte <= 0) return convertCount;
    
    AnsiString errMsg;
    iconv_t cd = iconv_open(TargetCodeName, OriginalCodeName);
    int errorNumber = errno;
    if ((long)cd == -1)
    {
        convertCount = -1;
        if (errorNumber == EINVAL)
            TranscodeMsg.Printf("iconv_open() EINVAL:The conversion from [%s] to [%s] is not supported by the implementation.", OriginalCodeName, TargetCodeName);
        else
            TranscodeMsg.Printf("iconv_open() Unknown error:%d.", errorNumber);
        return convertCount;
    }

    size_t inBytesLeft = OriginalDataSizeInByte;
    size_t outBytesLeft = MaxTargetDataSizeInByte;
    char* inBuf = OriginalDataBuffer;
    char* outBuf = TargetDataBuffer;
    size_t nonreversibleCount = iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft);
    errorNumber = errno;
    iconv_close(cd);

    if (static_cast<int>(nonreversibleCount) == -1)
    {
        convertCount = -1;
        if (errorNumber == E2BIG)
            TranscodeMsg = "iconv() E2BIG:There is not sufficient room st output.";
        else if (errorNumber == EILSEQ)
            TranscodeMsg = "iconv() EILSEQ:An invalid multibyte sequence has been encountered in the input.";
        else if (errorNumber == EINVAL)
            TranscodeMsg = "iconv() EINVAL:An incomplete multibyte sequence has been encountered in the input.";
        else
            TranscodeMsg.Printf("iconv() Unknown error:%d.", errorNumber);
    }
    else
        convertCount = static_cast<int>(MaxTargetDataSizeInByte - outBytesLeft);
        
    return convertCount;
}  //TranscodeCharacter()
//------------------------------------------------------------------------------
int TranscodeCharacter(const char *OriginalCodeName, const char *TargetCodeName,
                       char *OriginalDataBuffer, size_t OriginalDataSizeInByte,
                       AnsiString& TargetStr, AnsiString& TranscodeMsg)
{
    int convertCount = 0;
    if (OriginalDataSizeInByte <= 0) return convertCount;
    
    iconv_t cd = iconv_open(TargetCodeName, OriginalCodeName);
    int errorNumber = errno;
    if ((long)cd == -1)
    {
        convertCount = -1;
        if (errorNumber == EINVAL)
            TranscodeMsg.Printf("iconv_open() EINVAL:The conversion from [%s] to [%s] is not supported by the implementation.", OriginalCodeName, TargetCodeName);
        else
            TranscodeMsg.Printf("iconv_open() Unknown error:%d.", errorNumber);
        return convertCount;
    }

    size_t targetBufferSize = OriginalDataSizeInByte * 4;
    char *targetBuffer = new char[targetBufferSize + 1];
    memset(targetBuffer, 0, targetBufferSize + 1);
    size_t inBytesLeft = OriginalDataSizeInByte;
    size_t outBytesLeft = targetBufferSize;
    char* inBuf = OriginalDataBuffer;
    char* outBuf = targetBuffer;
    size_t nonreversibleCount = iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft);
    errorNumber = errno;
    iconv_close(cd);
    
    if (static_cast<int>(nonreversibleCount) != -1)
    {
        convertCount = static_cast<int>(targetBufferSize - outBytesLeft);
        TargetStr = AnsiString(targetBuffer);
    }
    else
    {
        convertCount = -1;
        if (errorNumber == E2BIG)
            TranscodeMsg = "E2BIG:There is not sufficient room st output.";
        else if (errorNumber == EILSEQ)
            TranscodeMsg = "EILSEQ:An invalid multibyte sequence has been encountered in the input.";
        else if (errorNumber == EINVAL)
            TranscodeMsg = "EINVAL:An incomplete multibyte sequence has been encountered in the input.";
        else
            TranscodeMsg.Printf("iconv() Unknown error:%d.", errorNumber);
    }  //if (static_cast<int>(nonreversibleCount) != -1)
        
    delete [] targetBuffer;
    return convertCount;
}  //TranscodeCharacter()
//------------------------------------------------------------------------------
int TranscodeCharacter(const char *OriginalCodeName, const char *TargetCodeName,
                       const char *OriginalDataBuffer, size_t OriginalDataSizeInByte,
                       AnsiString& TargetStr, AnsiString& TranscodeMsg)
{
    int convertCount = 0;
    if (OriginalDataSizeInByte <= 0) return convertCount;
    
    iconv_t cd = iconv_open(TargetCodeName, OriginalCodeName);
    int errorNumber = errno;
    if ((long)cd == -1)
    {
        convertCount = -1;
        if (errorNumber == EINVAL)
            TranscodeMsg.Printf("iconv_open() EINVAL:The conversion from [%s] to [%s] is not supported by the implementation.", OriginalCodeName, TargetCodeName);
        else
            TranscodeMsg.Printf("iconv_open() Unknown error:%d.", errorNumber);
        return convertCount;
    }

    char *tempOriginalBuffer = new char[OriginalDataSizeInByte + 1];
    memcpy(tempOriginalBuffer, OriginalDataBuffer, OriginalDataSizeInByte);
    tempOriginalBuffer[OriginalDataSizeInByte] = 0;
    size_t targetBufferSize = OriginalDataSizeInByte * 4;
    char *targetBuffer = new char[targetBufferSize + 1];
    memset(targetBuffer, 0, targetBufferSize + 1);
    size_t inBytesLeft = OriginalDataSizeInByte;
    size_t outBytesLeft = targetBufferSize;
    char* inBuf = tempOriginalBuffer;
    char* outBuf = targetBuffer;
    size_t nonreversibleCount = iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft);
    errorNumber = errno;
    iconv_close(cd);
    
    if (static_cast<int>(nonreversibleCount) != -1)
    {
        convertCount = static_cast<int>(targetBufferSize - outBytesLeft);
        TargetStr = AnsiString(targetBuffer);
    }
    else
    {
        convertCount = -1;
        if (errorNumber == E2BIG)
            TranscodeMsg = "E2BIG:There is not sufficient room st output.";
        else if (errorNumber == EILSEQ)
            TranscodeMsg = "EILSEQ:An invalid multibyte sequence has been encountered in the input.";
        else if (errorNumber == EINVAL)
            TranscodeMsg = "EINVAL:An incomplete multibyte sequence has been encountered in the input.";
        else
            TranscodeMsg.Printf("iconv() Unknown error:%d.", errorNumber);
    }  //if (static_cast<int>(nonreversibleCount) != -1)
    
    delete [] tempOriginalBuffer;
    delete [] targetBuffer;
    return convertCount;
}  //TranscodeCharacter()
//------------------------------------------------------------------------------
int TranscodeCharacter(const AnsiString& OriginalCodeName, const AnsiString& TargetCodeName,
                       const AnsiString& OriginalStr, AnsiString& TargetStr, AnsiString& TranscodeMsg)
{
    int convertCount = 0;
    size_t originalBufferSize = OriginalStr.Length();
    if (originalBufferSize <= 0) return convertCount;
    
    iconv_t cd = iconv_open(TargetCodeName.c_str(), OriginalCodeName.c_str());
    int errorNumber = errno;
    if ((long)cd == -1)
    {
        convertCount = -1;
        if (errorNumber == EINVAL)
            TranscodeMsg.Printf("iconv_open() EINVAL:The conversion from [%s] to [%s] is not supported by the implementation.", OriginalCodeName.c_str(), TargetCodeName.c_str());
        else
            TranscodeMsg.Printf("iconv_open() Unknown error:%d.", errorNumber);
        return convertCount;
    }
    
    char *originalBuffer = new char[originalBufferSize + 1];
    memcpy(originalBuffer, OriginalStr.c_str(), originalBufferSize);
    originalBuffer[originalBufferSize] = 0;
    size_t targetBufferSize = originalBufferSize * 4;
    char *targetBuffer = new char[targetBufferSize + 1];
    memset(targetBuffer, 0, targetBufferSize + 1);
    size_t inBytesLeft = originalBufferSize;
    size_t outBytesLeft = targetBufferSize;
    char* inBuf = originalBuffer;
    char* outBuf = targetBuffer;
    size_t nonreversibleCount = iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft);
    errorNumber = errno;
    iconv_close(cd);
    
    if (static_cast<int>(nonreversibleCount) != -1)
    {
        convertCount = static_cast<int>(targetBufferSize - outBytesLeft);
        TargetStr = AnsiString(targetBuffer);
//            TranscodeMsg.Printf(" iconv inByteLeft[%d]=>[%d] outByteLeft[%d]=>[%d]", static_cast<int>(originalBufferSize), static_cast<int>(inBytesLeft), static_cast<int>(targetBufferSize), static_cast<int>(outBytesLeft));
    }
    else
    {
        if (errorNumber == E2BIG)
            TranscodeMsg = "E2BIG:There is not sufficient room st output.";
        else if (errorNumber == EILSEQ)
            TranscodeMsg = "EILSEQ:An invalid multibyte sequence has been encountered in the input.";
        else if (errorNumber == EINVAL)
            TranscodeMsg = "EINVAL:An incomplete multibyte sequence has been encountered in the input.";
        else
            TranscodeMsg = "Unknown error.";
    }
        
    delete [] originalBuffer;
    delete [] targetBuffer;
    return convertCount;
}  //TranscodeCharacter()
#endif
}
//---------------------------------------------------------------------------

