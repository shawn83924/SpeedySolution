//------------------------------------------------------------------------------------------------------------------------
#include "MDSCommObj.h"
//------------------------------------------------------------------------------------------------------------------------
// class MDSBinaryObj
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
MDSBinaryObj::MDSBinaryObj()
: FBuffer( NULL )
, FSize( 0 )
, FPos( 0 )
{
}
//------------------------------------------------------------------------------------------------------------------------
MDSBinaryObj::MDSBinaryObj( Int32 Size )
: FSize( Size )
, FPos( 0 )
{
	FBuffer = new char[FSize];
}
//------------------------------------------------------------------------------------------------------------------------
MDSBinaryObj::MDSBinaryObj( const char* Src, Int32 Size )
: FSize( Size )
{
	FBuffer = new char[FSize];
	memcpy( FBuffer, Src, FSize );
	FPos = FSize;
}
//------------------------------------------------------------------------------------------------------------------------
MDSBinaryObj::MDSBinaryObj( const MDSBinaryObj &src )
: FSize( src.FSize )
, FPos( src.FPos )
{
	if( FSize )
	{
		FBuffer = new char[FSize];
		memcpy( FBuffer, src.FBuffer, FSize);
	}
}
//------------------------------------------------------------------------------------------------------------------------
MDSBinaryObj::~MDSBinaryObj()
{
	if( FBuffer )
	{
		delete [] FBuffer;
		FBuffer = NULL;
	}
}
//------------------------------------------------------------------------------------------------------------------------
Int32 MDSBinaryObj::Compare( const MDSBinaryObj& rhs ) const throw()
{
	if ( rhs.FBuffer && rhs.FSize )
		return memcmp( FBuffer, rhs.FBuffer, FSize );
	return -1; 
}
//------------------------------------------------------------------------------------------------------------------------
void MDSBinaryObj::PaddingZero( int Size )
{
	if ( ( FPos+Size > FSize ) )
	{
		MDSLengthErrorException exVar;
		exVar.Throw( "MDSBinaryObj::PaddingZero -> Over buffer size" );
	}

	memset( (FBuffer+FPos), '\0', Size );
	FPos += Size;
}
//------------------------------------------------------------------------------------------------------------------------
void MDSBinaryObj::AppendBuffer( const char* Buf, Int32 Size )
{
	if ( FPos+Size > FSize )
	{
		MDSLengthErrorException exVar;
		exVar.Throw( "MDSBinaryObj::AppendBuffer -> Over buffer size [%d] [%d] [%d]"
					 , FPos+Size, FSize, FPos );
	}
	if ( Buf == NULL )
	{
		MDSLengthErrorException exVar;
		exVar.Throw( "MDSBinaryObj::AppendBuffer -> Buffer is null error" );
	}
	if ( Size < 1 )
	{
		MDSLengthErrorException exVar;
		exVar.Throw( "MDSBinaryObj::AppendBuffer -> Buffer size < 1 error" );
	}

	memcpy( (FBuffer+FPos), Buf, Size );
	FPos += Size;
}
//------------------------------------------------------------------------------------------------------------------------
void MDSBinaryObj::AppendAnsiString( const UFC::AnsiString& Str, int DataLen )
{
	int Len = (Str.Length() > DataLen) ? DataLen : Str.Length();

	AppendBuffer( Str.c_str(), Len );
	// Padding '\0'
	if ( DataLen > Len )
		PaddingZero( DataLen - Len );
}
//------------------------------------------------------------------------------------------------------------------------
void MDSBinaryObj::AppendCString( const char* Str, int DataLen )
{
	int StrLen = strlen(Str);
	int Len = (StrLen > DataLen) ? DataLen : StrLen;

	AppendBuffer( Str, Len );
	// Padding '\0'
	if ( DataLen > Len )
		PaddingZero( DataLen - Len );
}
//------------------------------------------------------------------------------------------------------------------------
void MDSBinaryObj::AppendChar( const char Value )
{
	if ( FPos+1 > FSize )
	{
		MDSLengthErrorException exVar;
		exVar.Throw( "MDSBinaryObj::AppendChar -> Over buffer size" );
	}

	FBuffer[ FPos ] = Value;
	FPos += 1;
}
//------------------------------------------------------------------------------------------------------------------------
void MDSBinaryObj::AppendInt( int Value )
{
	int Size = sizeof(int);
	if ( FPos+Size > FSize )
	{
		MDSLengthErrorException exVar;
		exVar.Throw( "MDSBinaryObj::AppendInt -> Over buffer size" );
	}

	memcpy( (FBuffer+FPos), (const void*)&Value, Size );
	FPos += Size;
}
//------------------------------------------------------------------------------------------------------------------------
void MDSBinaryObj::AppendInt64( Int64 Value )
{
	int Size = sizeof(Int64);
	if ( FPos+Size > FSize )
	{
		MDSLengthErrorException exVar;
		exVar.Throw( "MDSBinaryObj::AppendInt64 -> Over buffer size" );
	}

	memcpy( (FBuffer+FPos), (const void*)&Value, Size );
	FPos += Size;
}
//------------------------------------------------------------------------------------------------------------------------
void MDSBinaryObj::AppendDouble( double Value )
{
	int Size = sizeof(double);
	if ( FPos+Size > FSize )
	{
		MDSLengthErrorException exVar;
		exVar.Throw( "MDSBinaryObj::AppendDouble -> Over buffer size" );
	}

	memcpy( (FBuffer+FPos), (const void*)&Value, Size );
	FPos += Size;
}
//------------------------------------------------------------------------------------------------------------------------
void MDSBinaryObj::Printf( const char* FormatStr, ... )
{	
	char     Buffer[ MAX_STR_BUFFER ];
	va_list  va;	

	if ( FBuffer != NULL )
	{
		delete [] FBuffer;
		FBuffer = NULL;
	}
	Buffer[0] = '\0';

	va_start( va , FormatStr );
	vsnprintf( Buffer, MAX_STR_BUFFER, FormatStr, va );
	va_end( va );

	FPos = 0; ///< Reset FPos to start of Buffer
	FSize = strlen( Buffer );
	if( FSize )
	{
            FBuffer = new char[ FSize ];
            memcpy( FBuffer, Buffer, FSize );
            FPos += FSize;
	}
}
//------------------------------------------------------------------------------------------------------------------------
Int32 MDSBinaryObj::Seek( Int32 Offset, Int32 Origin )
{
	switch( Origin )
	{
	case UFC::soFromBeginning :	FPos = Offset;
								break;
	case UFC::soFromCurrent   : FPos = FPos + Offset;
								break;
	case UFC::soFromEnd       :	if( Offset <= 0 )
									FPos = FSize + Offset;
								break;
	}
	if( FPos > FSize )
		FPos = FSize;
	else if( FPos < 0 )
		FPos = 0;
	return FPos;
}
//------------------------------------------------------------------------------------------------------------------------
void MDSBinaryObj::ReAllocBuffer( Int32 Size )
{
	// Delete FBuffer if not NULL
	if ( FBuffer != NULL )
	{
		delete [] FBuffer;
		FBuffer = NULL;
	}
	// check Size then 0
	if ( Size <= 0 )
	{
		FSize = 0;
		FPos = 0;
		return;
	}

	// ReAllocate Buffer & Assign member
	FSize = Size;
	FPos  = 0;
	FBuffer = new char[ FSize ];
}
//------------------------------------------------------------------------------------------------------------------------
MDSBinaryObj& MDSBinaryObj::operator = ( const MDSBinaryObj& rhs )
{
	if ( this == &rhs )
		return *this;

	// Delete FBuffer if not NULL
	if ( FBuffer != NULL )
	{
		delete [] FBuffer;
		FBuffer = NULL;
	}
	// check size of rhs if bigger then 0
	if ( rhs.FSize <= 0 )
	{
		FSize = 0;
		FPos = 0;
		return *this;
	}

	// allocate Buffer & Assign member
	FSize   = rhs.FSize;
	FPos    = rhs.FPos;
	FBuffer = new char[ FSize ];
	memcpy( FBuffer, rhs.FBuffer, FSize );

	return *this;
}
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
