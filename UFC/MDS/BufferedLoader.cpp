//------------------------------------------------------------------------------------------------------------------------
#include "BufferedLoader.h"
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
// class BufferedLoader
//------------------------------------------------------------------------------------------------------------------------
BufferedLoader::BufferedLoader( int BufferSiezeBytes )
: FFileHandle( NULL )
, FBufferSize( BufferSiezeBytes )
, FRemainSize( 0 )
, FPos( 0 )
{
	FBuffer = new char[ FBufferSize ];
}
//------------------------------------------------------------------------------------------------------------------------
BufferedLoader::~BufferedLoader()
{
	delete [] FBuffer;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL BufferedLoader::NativeRead( )
{
	if ( FRemainSize > 0 )
		memmove( FBuffer, FBuffer+FPos, FRemainSize );

	int Size = FBufferSize-FRemainSize;
	int ReadSize = fread( FBuffer+FRemainSize, sizeof(char), Size, FFileHandle );

	if ( ReadSize < Size )
	{
		if ( ferror( FFileHandle ) )
			return FALSE;
	}

	FRemainSize += ReadSize;
	FPos = 0;

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL BufferedLoader::Open( const UFC::AnsiString FileName )
{
	if ( FFileHandle != NULL )
	{
		fclose( FFileHandle );
		FFileHandle = NULL;
	}

	if ( ( FFileHandle = fopen( FileName.c_str(), "r" ) ) == NULL )
		return FALSE;

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
int BufferedLoader::Read( void* Buffer, int Size )
{
	if ( FFileHandle == NULL )
		return -1;

	if ( Size > FBufferSize )
		return -1;

	if ( Size > FRemainSize )
	{
		if ( NativeRead( ) == FALSE )
			return -1;
	}

	int RtnSize = ( ( Size > FRemainSize ) ? FRemainSize : Size );

	if ( RtnSize > 0 )
	{
		memcpy( Buffer, FBuffer+FPos, RtnSize );
		FRemainSize -= RtnSize;
		FPos += RtnSize;
	}

	return RtnSize;
}
//------------------------------------------------------------------------------------------------------------------------
int BufferedLoader::ReadLine( void* Buffer, int BufferSize )
{
	int RtnSize;
	int i;
	while( true )
	{
		if ( FRemainSize == 0 )
		{
			if ( feof( FFileHandle ) )
			{
				RtnSize = 0;
				break;
			}
		}
		for ( i = 0; i < FRemainSize; i++ )
		{
			if ( FBuffer[i] == '\n' )
				break;
		}
		if ( i == FRemainSize )
		{
			if ( feof( FFileHandle ) )
			{
				RtnSize = FRemainSize;
				if ( (RtnSize+1) > BufferSize )
					return -1;
				memcpy( Buffer, FBuffer+FPos, RtnSize );
				FRemainSize -= RtnSize;
				FPos += RtnSize;
				*((char*)Buffer+RtnSize) = 0;
				break;
			}
			else
			{
				if ( NativeRead() == FALSE )
					return -1;
			}
		}
		else
		{
			RtnSize = i+1;
			if ( (RtnSize+1) > BufferSize )
				return -1;
			memcpy( Buffer, FBuffer+FPos, RtnSize );
			FRemainSize -= RtnSize;
			FPos += RtnSize;
			*((char*)Buffer+RtnSize) = 0;
			break;
		}
	}
	return RtnSize;
}
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
