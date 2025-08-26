/*
 * File:   zlibutility.cpp
 * Author: simon
 * 
 * Created on November 25, 2008, 5:41 PM
 */

#include "zlibutility.h"
#include "NetUtility.h"
#include "BufferedLog.h"
#include "../zlib/zlib.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
CompressedData::CompressedData( int CompressedSize, UInt8* Data )
:FCompressedSize( CompressedSize ),
 FSourceSize( UFC::NInt32::ToInt32( Data ) ),
 FCompressedData( new UInt8[ CompressedSize] )
{
    memcpy( FCompressedData, Data, CompressedSize );
}
//---------------------------------------------------------------------------
CompressedData::CompressedData( UFC::PStream* Stream, UInt32 Size )
:FCompressedSize( Size ),
 FSourceSize( 0 ),
 FCompressedData( new UInt8[ Size] )
{
    Stream->Read( FCompressedData, FCompressedSize );
    FSourceSize = UFC::NInt32::ToInt32( FCompressedData );
}
//---------------------------------------------------------------------------
CompressedData::~CompressedData()
{
    if( FCompressedData != NULL )
	delete [] FCompressedData;
}
//---------------------------------------------------------------------------
UFC::CompressedData*  Compress( const UInt8* Data, UInt32 Size )
{
    uLongf  cSize  = Size + (Size/100) + 1 + 12; ///< The destination buffer must be at least 0.1% larger than sourceLen plus 12 bytes.
    UInt8*  cData  = new UInt8[ cSize + 4 ];
    int     RtnVal = compress ( cData + 4 , (uLongf*)&cSize, Data, (uLong)Size );

    if( RtnVal == Z_OK )
    {
	UFC::NInt32::ToInt32( Size, cData ); ///< First 4 bytes keep Original data size
	return new CompressedData( Size, cSize + 4, cData );
    }
    else if( RtnVal == Z_MEM_ERROR )
        UFC::BufferedLog::Printf( " There was not enough memory for zlib to compress data." );
    else if( RtnVal == Z_BUF_ERROR )
        UFC::BufferedLog::Printf( " There was not enough room in the output buffer to compress data." );
    delete [] cData;
    return NULL;
}
//------------------------------------------------------------------------------
BOOL Uncompress( UFC::CompressedData* Data, UInt32* Size, UInt8* Result )
{
    uLongf UncompressedSize = *Size;
    int    RtnVal = uncompress( Result, &UncompressedSize, Data->GetData()+4 ,(uLong)(Data->GetSize()-4));

    if( RtnVal  == Z_OK )
    {
        *Size = UncompressedSize;
        return TRUE;
    }
    else if( RtnVal == Z_MEM_ERROR )
        UFC::BufferedLog::Printf( " There was not enough memory for zlib to uncompress data." );
    else if( RtnVal == Z_BUF_ERROR )
        UFC::BufferedLog::Printf( " There was not enough room in the output buffer to uncompress data." );
    else if( RtnVal == Z_DATA_ERROR )
        UFC::BufferedLog::Printf( " The input data for uncompress was corrupted or incomplete." );
    *Size = 0;
    return FALSE;
}
//------------------------------------------------------------------------------
int CompressFile(char* SourceFileName, char* CompressFileName)
{
	if (SourceFileName == 0) return -1;
	if (CompressFileName == 0) return -2;

	FileStream* FSourceStream;
	try
	{
		FSourceStream = new FileStream(SourceFileName, O_RDONLY | O_BINARY);
	}
	catch (...)
	{
		return -3;
	}

	char ReadBuffer[512];
	int ReadCount;
	int TotalReadCount = 0;
	gzFile gDesc = gzopen(CompressFileName, "wb");

	try
	{
		while ((ReadCount = FSourceStream->Read(ReadBuffer, 512)) > 0)
		{
			TotalReadCount += ReadCount;
			gzwrite(gDesc, ReadBuffer, ReadCount);
		}
	}
	catch (...)
	{
		gzclose(gDesc);
		delete FSourceStream;
		return -4;
	}

	gzclose(gDesc);
	delete FSourceStream;
	return TotalReadCount;
}  //CompressFile()

//------------------------------------------------------------------------------
int CompressFile(char* AdditionInformation, char* SourceFileName, char* CompressFileName)
{
	if (SourceFileName == 0) return -1;
	if (CompressFileName == 0) return -2;

	FileStream* FSourceStream;
	try
	{
		FSourceStream = new FileStream(SourceFileName, O_RDONLY | O_BINARY);
	}
	catch (...)
	{
		return -3;
	}

	char ReadBuffer[512];
	int ReadCount;
	int TotalReadCount = 0;
	gzFile gDesc = gzopen(CompressFileName, "wb");

	try
	{
		if ((AdditionInformation != 0) && (strlen(AdditionInformation) > 0))
		{
			gzwrite(gDesc, AdditionInformation, (unsigned int)strlen(AdditionInformation));
		}

		while ((ReadCount = FSourceStream->Read(ReadBuffer, 512)) > 0)
		{
			TotalReadCount += ReadCount;
			gzwrite(gDesc, ReadBuffer, ReadCount);
		}
	}
	catch (...)
	{
		gzclose(gDesc);
		delete FSourceStream;
		return -4;
	}

	gzclose(gDesc);
	delete FSourceStream;
	return TotalReadCount;
}  //CompressFile()

//------------------------------------------------------------------------------
int UncompressFile(char* SourceFileName, char* UncompressFileName)
{
	if (SourceFileName == 0) return -1;
	if (UncompressFileName == 0) return -2;

	FileStream* FDescStream;
	try
	{
		FDescStream = new FileStream( UncompressFileName, O_CREAT| O_WRONLY | O_BINARY);
	}
	catch (...)
	{
		return -3;
	}
	char ReadBuffer[ 512 ];
	int  ReadSize;
	int  TotalReadCount;
	gzFile gSrc = gzopen( SourceFileName, "rb");

	try
	{
		while( (ReadSize = gzread(gSrc, ReadBuffer, 512)) > 0 )
		{
			FDescStream->Write( ReadBuffer, ReadSize );
		}
	}
	catch (...)
	{
		gzclose(gSrc);
		delete FDescStream;
		return -4;
	}
	gzclose(gSrc);
	TotalReadCount = FDescStream->GetPosition();
	delete FDescStream;
	return TotalReadCount;
}  ///< UncompressFile()
//------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
