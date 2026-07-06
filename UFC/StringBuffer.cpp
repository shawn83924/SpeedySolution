//---------------------------------------------------------------------------
#include "StringBuffer.h"
//-----------------------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
const int INIT_BUFFER_SIZE = 128;
const UInt16 MAX_HAND_SIZE = 65505; // The Max MemHandle size
//---------------------------------------------------------------------------
PStringBuffer::PStringBuffer()
:FLength(0),FCapacity(INIT_BUFFER_SIZE)
{
	InitCap(FCapacity);
	StrBuffer[0] ='\0';
}
//---------------------------------------------------------------------------
PStringBuffer::PStringBuffer( const char* src )
:FCapacity(INIT_BUFFER_SIZE)
{
	InitStr(src, (UFCType::UInt16)strlen(src));
}
//---------------------------------------------------------------------------
PStringBuffer::PStringBuffer( const AnsiString &src )
:FCapacity(INIT_BUFFER_SIZE)
{
	InitStr(src.c_str(), src.Length());
}
//---------------------------------------------------------------------------
PStringBuffer::PStringBuffer( UInt16 InitCapacity )
:FLength(0)
{
	if (InitCapacity == 0)
		FCapacity = INIT_BUFFER_SIZE;
	else
		FCapacity = InitCapacity;
	InitCap(FCapacity);
	StrBuffer[0] ='\0';
}
//---------------------------------------------------------------------------
PStringBuffer::~PStringBuffer( )
{
	delete [] StrBuffer;
}
//---------------------------------------------------------------------------
void PStringBuffer::InitCap(UInt16 length)
{
	while (length > FCapacity)
		FCapacity += INIT_BUFFER_SIZE;
	if (FCapacity < MAX_HAND_SIZE)
	{
		StrBuffer = new char[FCapacity]; // + 1];
	}
	else
	{
		StrBuffer = new char[MAX_HAND_SIZE];
		FCapacity = MAX_HAND_SIZE - 1;
	}
}
//---------------------------------------------------------------------------
void PStringBuffer::InitStr(const char *Str, const UInt16 Len )
{
	InitCap( Len + 1 );
	memcpy(StrBuffer, Str, Len);
	FLength = Len;
	StrBuffer[FLength] = '\0';
}
//---------------------------------------------------------------------------
void PStringBuffer::SAppend(const char *Str, const UInt16 StrLength)
{
	UInt16 len = FLength + StrLength + 1;
	if (len < MAX_HAND_SIZE)
		EnsureCapacity(len);
	else
		EnsureCapacity(MAX_HAND_SIZE);
	memcpy(StrBuffer + FLength, Str, StrLength);
	FLength += StrLength;
	StrBuffer[FLength] = '\0';
}
//---------------------------------------------------------------------------
PStringBuffer& PStringBuffer::Append(bool b)
{
	if(b)
		SAppend("true", 4);
	else
		SAppend("false", 5);
	return *this;
}
//---------------------------------------------------------------------------
PStringBuffer& PStringBuffer::Append(char c)
{
	if (c != '\0')
	{
		if (FLength + 1 < FCapacity)
		{
			StrBuffer[FLength] = c;
			FLength++;
			StrBuffer[FLength] = '\0';;
		}
		else
		{
			char str[2] = {c, '\0'};
			SAppend(str, 1);
		}
	}
	return *this;
}
//---------------------------------------------------------------------------
PStringBuffer& PStringBuffer::Append(const char* str)
{
	if (str)
		SAppend(str, (UFCType::UInt16)strlen(str));
	return *this;
}
//---------------------------------------------------------------------------
PStringBuffer& PStringBuffer::Append(const char* str, Int32 offset, UInt16 length)
{
	if (str)
		SAppend(str + offset, length);
	return *this;
}
//---------------------------------------------------------------------------
PStringBuffer& PStringBuffer::Append(const AnsiString &src)
{
	if (src.Length())
		SAppend(src.c_str(), src.Length());
	return *this;
}
//---------------------------------------------------------------------------
PStringBuffer& PStringBuffer::Append(Int32 i)
{
	AnsiString s(i);
	SAppend(s.c_str(), s.Length());
	return *this;
}
//---------------------------------------------------------------------------
PStringBuffer& PStringBuffer::Delete(UInt16 index, UInt16 count)
{
	if (index < FLength)
	{
        if((index + count) < FLength)
        {
            memmove( StrBuffer + index, StrBuffer + index + count, FLength - (index + count) + 1 );
            FLength -= count;
        }
        else
        {
            StrBuffer[index] = '\0';
            FLength = index;
        }
	}
	return *this;
}
//---------------------------------------------------------------------------
void PStringBuffer::Pack()
{
    UInt16 Org = FCapacity;
    UInt16 realLen = FLength + 1;
    while ((FCapacity - realLen) >= INIT_BUFFER_SIZE)
    {
        FCapacity -= INIT_BUFFER_SIZE;
    }  //while ((FCapacity - realLen) > INIT_BUFFER_SIZE)

    if (FCapacity == 0) FCapacity = INIT_BUFFER_SIZE;

    if (Org != FCapacity)
    {
        char* tempBuffer = new char[FCapacity];
        memcpy(tempBuffer, StrBuffer, (size_t)FLength + 1);
        delete []StrBuffer;
        StrBuffer = tempBuffer;
    }  
}  //PStringBuffer::Pack()
//---------------------------------------------------------------------------
void PStringBuffer::EnsureCapacity(UInt16 minimumCapacity)
{
    if (minimumCapacity > FCapacity)
    {
        while( FCapacity < minimumCapacity )
        {
            if( FCapacity >= 32768 ) { FCapacity = 65535; break; }
            FCapacity *= 2;
        }
        char* tempBuffer = new char[FCapacity];
        memcpy(tempBuffer, StrBuffer, (size_t)FLength + 1);
        delete []StrBuffer;
        StrBuffer = tempBuffer;
    }
}

//---------------------------------------------------------------------------
void PStringBuffer::SInsert(UInt16 offset, const char *Str, UInt16 length)
{
	if ( (FLength + length + 1) >= FCapacity )
		EnsureCapacity(FLength + length + 1);
	if (offset > FLength)
		offset = FLength;
	//StrCopy(StrBuffer + offset + length, StrBuffer + offset);
	memcpy(StrBuffer + offset + length, StrBuffer + offset, (size_t)FLength - (size_t)offset + 1);
	memcpy(StrBuffer + offset, Str, length);
	FLength += length;
}
//---------------------------------------------------------------------------
PStringBuffer& PStringBuffer::Insert(UInt16 offset, bool b)
{
	if (b)
		SInsert(offset, "true", 4);
	else
		SInsert(offset, "false", 5);
	return *this;
}
//---------------------------------------------------------------------------
PStringBuffer& PStringBuffer::Insert(UInt16 offset, char c)
{
	if (c != '\0')
	{
		char s[2] = { c, '\0' };
		SInsert(offset, s, 1);
	}
	return *this;
}
//---------------------------------------------------------------------------
PStringBuffer& PStringBuffer::Insert(UInt16 offset, const char* str)
{
	if (str)
		SInsert(offset, str, (UFCType::UInt16)strlen(str));
	return *this;
}
//---------------------------------------------------------------------------
PStringBuffer& PStringBuffer::Insert(UInt16 offset, const char* str, Int32 strOffset,
								     UInt16 length)
{
	if (str)
		SInsert(offset, str + strOffset, length);
	return *this;
}
//---------------------------------------------------------------------------
PStringBuffer& PStringBuffer::Insert(UInt16 offset, const AnsiString &src)
{
	if (src.Length())
		SInsert(offset, src.c_str(), src.Length());
	return *this;
}
//---------------------------------------------------------------------------
PStringBuffer& PStringBuffer::Insert(UInt16 offset, Int32 i)
{
	AnsiString si(i);
	SInsert(offset, si.c_str(), si.Length());
	return *this;
}
//---------------------------------------------------------------------------
PStringBuffer& PStringBuffer::Reverse()
{
	register int i = 0;
	register int j = FLength - 1;
	char tmpChar;

	while (i < j)
	{
		tmpChar = StrBuffer[i];
		StrBuffer[i] = StrBuffer[j];
		StrBuffer[j] = tmpChar;
		i++;
		j--;
	}
	return *this;
}
//---------------------------------------------------------------------------
PStringBuffer& PStringBuffer::SetLength(UInt16 length)
{
	EnsureCapacity(length + 1);
	if (length < FLength)
	{
		StrBuffer[length] = '\0';
		FLength = length;
	}
	return *this;
}
//---------------------------------------------------------------------------
AnsiString PStringBuffer::SubString( UInt16 index, UInt16 count )
{
	if (index < FLength)
	{
		if (index + count >= FLength)
			return (StrBuffer + index);
		else
			return AnsiString (StrBuffer + index, count );
	}
	return AnsiString();
}
//---------------------------------------------------------------------------
UInt16 PStringBuffer::AppendPrintf( const char* FormatStr, ... )
{
    UInt16   Count;
    char     Buffer[ UFC::AnsiString::MAX_STR_BUFFER ];
    va_list  va;

    Buffer[0] = '\0';
    va_start( va , FormatStr );
    Count = vsnprintf( Buffer, UFC::AnsiString::MAX_STR_BUFFER, FormatStr, va );
    va_end( va );

    if( Count > 0 )
    {
    	EnsureCapacity( FLength + Count + 1 );
    	memcpy( StrBuffer + FLength, Buffer, Count + 1 );
        FLength += Count;
    }
	return Count;
}
//---------------------------------------------------------------------------
UInt16 PStringBuffer::Printf( const char* FormatStr, ... )
{
    UInt16   Count;
    char     Buffer[ UFC::AnsiString::MAX_STR_BUFFER ];
    va_list  va;

    Buffer[0] = '\0';
    va_start( va , FormatStr );
    Count = vsnprintf( Buffer, UFC::AnsiString::MAX_STR_BUFFER, FormatStr, va );
    va_end( va );

    if( Count > 0 )
    {
        EnsureCapacity( Count + 1);
        memcpy( StrBuffer, Buffer, Count + 1 );
        FLength = Count;
    }
    return Count;
}
//---------------------------------------------------------------------------
PStringBuffer&  PStringBuffer::operator = (const char* rhs)
{
	if (rhs)
	{
		UInt16 strLen = (UFCType::UInt16)strlen(rhs);
		EnsureCapacity(strLen + 1);
		memcpy(StrBuffer, rhs, strLen + 1);
		FLength = strLen;
	}
	return *this;
}
//---------------------------------------------------------------------------
PStringBuffer&  PStringBuffer::operator = (const AnsiString& rhs)
{
	if (rhs.Length())
	{
		EnsureCapacity(rhs.FLength + 1);
		memcpy(StrBuffer, rhs.c_str(), rhs.Length() + 1);
		FLength = rhs.Length();
	}
	return *this;
}
//---------------------------------------------------------------------------
PStringBuffer&  PStringBuffer::operator = (const PStringBuffer& rhs)
{
	if (rhs.FLength)
	{
		EnsureCapacity(rhs.FLength + 1);
		memcpy(StrBuffer, rhs.StrBuffer, rhs.FLength + 1);
		FLength = rhs.FLength;
	}
	return *this;
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

