#include "List.h"
#include "FileStream.h"
//---------------------------------------------------------------------------
namespace UFC
{
//----------------------------------------------------------------------------------
PList::PList( CompareInterface* CMPFunc, Int32 InitialSize )
:FTotalAlloc( InitialSize ),
 FPosition( 0 ),
 FArray( NULL ),
 FCompare( CMPFunc )
{
	if( FTotalAlloc < LIST_MINIUM_SIZE )
		FTotalAlloc = LIST_MINIUM_SIZE;
	FArray = ( LPVOID* )malloc( FTotalAlloc * sizeof( LPVOID ) );
	if( FArray == NULL )
		throw AllocException();
}
//----------------------------------------------------------------------------------
PList::PList( Int32 InitialSize )
:FTotalAlloc( InitialSize ),
 FPosition( 0 ),
 FArray( NULL ),
 FCompare( NULL )
{
	if( FTotalAlloc < LIST_MINIUM_SIZE )
		FTotalAlloc = LIST_MINIUM_SIZE;
	FArray = ( LPVOID* )malloc( FTotalAlloc * sizeof( LPVOID ) );
	if( FArray == NULL )
		throw AllocException();
}
//---------------------------------------------------------------------------
PList::~PList()
{
	if( FArray )
		free( FArray );
}
//---------------------------------------------------------------------------
void PList::Clear( void )
{
	FPosition = 0;
	Pack();
}
//---------------------------------------------------------------------------
void PList::Grow( void )
{
	Int32 Delta;

	if( FTotalAlloc > 128 )
		Delta = FTotalAlloc/4;
	else if( FTotalAlloc > 64 )
     	Delta = 16;
    else
		Delta = 8;
	SetCapacity( FTotalAlloc + Delta );
}
//---------------------------------------------------------------------------
Int32 PList::append( void *Data )
{
	Int32 RetPos = FPosition;
	if( FPosition == FTotalAlloc )
		Grow();
	*(FArray + FPosition) = Data;
		FPosition++;
	return RetPos;
}
//---------------------------------------------------------------------------
Int32 PList::insert( int Pos, void* Data )
{
	if( Pos < 0 )
	    Pos = 0;
	if( Pos >= FPosition )
		return append( Data );
	if( FPosition == FTotalAlloc)
		Grow();
	memmove( FArray + Pos + 1, FArray + Pos, sizeof(LPVOID) * (FPosition - Pos));
	*( FArray + Pos ) = Data;
	FPosition++;
    return Pos;
}
//---------------------------------------------------------------------------
Int32 PList::Add( void *Data )
{
	if( FCompare == NULL ) ///< Not needs sort, append to end.
		return append( Data );
   	if( FPosition == 0 )   ///< The first one, just append to end.
        return append( Data );
	///< Need sorting.
    int Result,First = 0;
    int Last  = FPosition - 1;
    int Half  = (Last - First)>>1;
    ///< Smaller than the Smallest object.
    if( FCompare->Compare( Data, *FArray ) <= 0 )
    	return insert( First, Data );
    ///< Larger than the largest object.
    if( FCompare->Compare( Data, *(FArray+Last) ) >= 0 )
    	return append( Data );
    for(;;)
    {
	    Result = FCompare->Compare( Data, *( FArray + Half ) );
	    if( Result > 0 )     ///< Larger than the middle object.
   	    	First = Half;
	    else if( Result < 0 )///< Smaller than the middle object.
   	    	Last  = Half;
        else                 ///< Bingo! Same as the middle object.
            return insert( Half, Data );
        Half  = First + ((Last - First)>>1);
        if( First == Half || Last == Half )
            return insert( Last, Data );
    }
}
//---------------------------------------------------------------------------
Int32 PList::Insert(int Pos, void* Data)
{
	if( FCompare != NULL )
		throw UFC::Exception( "Can not call Insert in a sorted PList." );
	return insert( Pos, Data );
}
//---------------------------------------------------------------------------
void PList::Pack( void )
{
	Int32 Org = FTotalAlloc;

	if( FPosition > 128 )
		FTotalAlloc = FPosition;
	else
		FTotalAlloc = (FPosition/8)*8 + 8;
	if( FTotalAlloc < LIST_MINIUM_SIZE )
		FTotalAlloc = LIST_MINIUM_SIZE;
	if( Org != FTotalAlloc)
		SetCapacity( FTotalAlloc );
}
//---------------------------------------------------------------------------
void PList::SetCapacity( Int32 Capacity )
{
	Int32 NewAllocSize = Capacity * sizeof( LPVOID );

	LPVOID *Temp = ( LPVOID* )malloc( NewAllocSize );
	if( Temp == NULL )
		throw AllocException( );
	memcpy( Temp, FArray, sizeof( LPVOID ) * FPosition );
	free( FArray );
	FArray = Temp;

	FTotalAlloc = Capacity;
	if( FPosition > FTotalAlloc )
		FPosition = FTotalAlloc;
}
//---------------------------------------------------------------------------
void* PList::Delete( Int32 Index )
{
	if( Index >= 0 && Index < FPosition )
	{
		void* delObj = *( FArray + Index );
		FPosition--;
		if( Index != FPosition )
		    memmove( FArray + Index, FArray + Index + 1 , sizeof(LPVOID) * ( FPosition - Index ) );
		return delObj;
	}
	return NULL;
}
//---------------------------------------------------------------------------
Int32 PList::Remove( void *Data)
{
    Int32 index = IndexOf( Data );
    if( index != -1 )
    {
    	FPosition--;
		if( index != FPosition)
			memmove( FArray + index , FArray + index + 1 , sizeof(LPVOID) * (FPosition - index) );
    }
	return index;
}
//---------------------------------------------------------------------------
void* PList::GetItem( Int32 Index )
{
    if( Index < 0 || Index >= FPosition )
    {
        return NULL;
    }
    return *( FArray + Index );
}
//---------------------------------------------------------------------------
void* PList::GetConstItem( Int32 Index ) const
{
    if( Index < 0 || Index >= FPosition )
        return NULL;
    return *( FArray + Index );
}
//---------------------------------------------------------------------------
void* PList::SetItem( Int32 Index, void* NewData )
{
	if( FCompare != NULL )
		throw UFC::Exception( "Can not call SetItem in a sorted PList." );
    if( Index >= 0 && Index < FPosition )
    {
		void* ToReturn = *(FArray + Index );
        *( FArray + Index ) = NewData;
       	return ToReturn;
	}
    return NULL;
}
//---------------------------------------------------------------------------
Int32 PList::IndexOf( void *ToFind )
{
    register void *Data;

	for( register Int32 i = 0; i < FPosition; i++)
	{
        Data = *( FArray + i );
		if( ToFind == Data)
			return i;
	}
	return -1;
}
//---------------------------------------------------------------------------
//
//  Win32 not define strkon_r, copy src from FreeBSD
//
//---------------------------------------------------------------------------
#ifdef _WIN32
char* strtok_r(char *s1, const char *s2, char **lasts)
{
	char *ret;

	if( s1 == NULL)
		s1 = *lasts;
	while( *s1 && strchr(s2, *s1) )
    	++s1;
	if( *s1 == '\0')
		return NULL;
	ret = s1;
	while( *s1 && !strchr(s2, *s1))
		++s1;
	if( *s1 )
		*s1++ = '\0';
	*lasts = s1;
	return ret;
}
#endif
//---------------------------------------------------------------------------
bool IsDelimiter( char ch, const UFC::AnsiString& Delimiter )
{
	if( Delimiter.AnsiPos( ch ) == -1)
		return false;
	return true;
}
//---------------------------------------------------------------------------
//
// function for PStringList
//
//---------------------------------------------------------------------------
PStringList::PStringList( const PStringList& Ref )
{
	int CpyCount = Ref.ItemCount();

	for( register int i = 0; i < CpyCount; i++ )
		 Add( Ref.GetItem( i ) );    
}
//---------------------------------------------------------------------------
void PStringList::SetQuoteStrings( const UFC::AnsiString& Strings, const UFC::AnsiString& Delimiter )
{
	int len = Strings.Length();
	char ch;
	const char* StrPtr = Strings.c_str();

	if( len > 0 )
	{
		int  StarPos = 0;
		bool InQuote = false;
		UFC::AnsiString Token;

		Clear();
		for( register int i = 0; i < len; i++ )
		{
			ch = *( StrPtr + i );
			if( ch == '"' )
				InQuote = !InQuote;
			if( InQuote == false )
			{
				if( IsDelimiter( ch,  Delimiter ) == true )
				{
					Token = Strings.SubString( StarPos, i - StarPos );
					Add( Token );
					StarPos = i + 1;
				}
			}
		}
		Token = Strings.SubString( StarPos, len - StarPos );
		Add( Token );
	}
}
//---------------------------------------------------------------------------
//
// function for PStringList
//
//---------------------------------------------------------------------------
void PStringList::SetStrings( const UFC::AnsiString& Strings, const UFC::AnsiString& Delimiter )
{
    char* CopyStrings = NULL;
    char* Token = NULL;
    char* SavePtr;
    int len = Strings.Length();

    if( len > 0 )
    {
            CopyStrings = new char[ len + 1];
            strcpy( CopyStrings, Strings.c_str() );
            Token = strtok_r( CopyStrings, (char*)Delimiter.c_str(), &SavePtr );
    }
    Clear();
    while( Token != NULL )
    {
        Add( Token );
        Token = strtok_r( NULL, Delimiter.c_str(), &SavePtr );
    }
    if( CopyStrings != NULL )
        delete [] CopyStrings;
}
//---------------------------------------------------------------------------
void PStringList::SetStrings( const AnsiString& Strings, const AnsiString& Delimiter, bool KeepEmptyField )
{
    Clear();
    Int32 sourceLength = Strings.Length();
    if( sourceLength <= 0 ) return;
    
    Int32 delimiterLength = Delimiter.Length();
    if( delimiterLength <= 0 )
    {
        Add( Strings );
        return;
    }
    
    char *sourceBuffer = new char[ sourceLength + 1];
    strcpy( sourceBuffer, Strings.c_str() );
    char *delimiterBuffer = new char[ delimiterLength + 1];
    strcpy( delimiterBuffer, Delimiter.c_str() );    
    
    char *curPosition = sourceBuffer;
    char *curFieldStartPosition = 0;
    Int32 curFieldLength = 0;
    bool lastCharIsDelimiter = false;
    while( *curPosition != '\0' )
    {
        if( strchr( delimiterBuffer, *curPosition ) != 0 )
        {
            lastCharIsDelimiter = true;
            if( KeepEmptyField || ( curFieldLength > 0 ) )
            {
                AnsiString newFieldStr( curFieldStartPosition, curFieldLength );
                Add( newFieldStr );
            }
            curFieldStartPosition = 0;
            curFieldLength = 0;
        }
        else
        {
            lastCharIsDelimiter = false;
            if( curFieldLength == 0 ) curFieldStartPosition = curPosition;
            curFieldLength++;
        }
        curPosition++;
    }  //while ( *curPosition != '\n' )

    if( ( KeepEmptyField && lastCharIsDelimiter ) || ( curFieldLength > 0 ) )
    {
        AnsiString newFieldStr( curFieldStartPosition, curFieldLength );
        Add( newFieldStr );
    }
    
    delete [] sourceBuffer;
    delete [] delimiterBuffer;
}
//---------------------------------------------------------------------------
Int32 PStringList::IndexOf( const UFC::AnsiString& FindString )
{
	for( register Int32 i = 0; i < ItemCount(); i++)
		 if( FArray[ i ] == FindString )
			return i;
	return -1;
}
//---------------------------------------------------------------------------
Int32 PStringList::CopyFrom( UFC::PStringList& SrcList )
{
	int CpyCount = SrcList.ItemCount();

	for( register int i = 0; i < CpyCount; i++ )
		 Add( SrcList[i] );
	return CpyCount;
}
//---------------------------------------------------------------------------
void PStringList::Save( const UFC::AnsiString& FileName )
{
    UFC::FileStream saveini( FileName, O_CREAT|O_TRUNC|O_WRONLY  );    
    int             CpyCount = ItemCount();
    char            buffer[ 32 ];
    
    for( register int i = 0; i < CpyCount; i++ )
    {
        UFC::AnsiString&  Line = FArray[ i ];                    
        saveini.Write( Line.c_str(), Line.Length() );
    }
    strcpy( buffer, "\n" );
    saveini.Write( buffer,(int)strlen(buffer));    
    
}
//---------------------------------------------------------------------------
UFC::AnsiString PStringList::DelimiterString( char Delimiter )
{
    PStringBuffer StringBuf;    
    int           Count = ItemCount();
        
    for( register int i = 0; i < Count; i++ )
    {
        StringBuf.Append( FArray[ i ] );
        if( i != Count - 1 )
            StringBuf.Append( Delimiter );
    }
    return StringBuf.ToString();
}
//---------------------------------------------------------------------------
Int32 Hash( const AnsiString& Str )
{
	hash<const char*> hasher;
	return (UFCType::Int32)hasher( Str.c_str() );
}
//---------------------------------------------------------------------------
} ///< Close namespace UFC
