/* 
 * File:   zlibutility.h
 * Author: simon
 *
 * Created on November 25, 2008, 5:41 PM
 */
//------------------------------------------------------------------------------
#ifndef _ZLIBUTILITY_H
#define	_ZLIBUTILITY_H
//------------------------------------------------------------------------------
#include "UFCType.h"
#include "Stream.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------
class CompressedData;
//------------------------------------------------------------------------------
class CompressedData
{
friend UFC::CompressedData*  Compress( const UInt8* Data, UInt32 Size );
private:
	UInt32 FCompressedSize;
	UInt32 FSourceSize;
	UInt8* FCompressedData;
private:
	CompressedData( ):FCompressedSize(0),FSourceSize(0),FCompressedData( NULL ){}
	CompressedData( UInt32 SrcSize, UInt32 CSize, UInt8* Data ):FCompressedSize( CSize ),FSourceSize( SrcSize ),FCompressedData( Data ){ }
public:
	CompressedData( int CompressedSize, UInt8* Data );
	CompressedData( UFC::PStream* Stream, UInt32 Size );
	~CompressedData();
	UInt32       GetSize()       { return FCompressedSize; }
	UInt32       GetSourceSize() { return FSourceSize; }
	const UInt8* GetData()       { return FCompressedData; }
};
//------------------------------------------------------------------------------
extern UFC::CompressedData*  Compress( const UInt8* Data, UInt32 Size );
extern BOOL Uncompress( UFC::CompressedData*, UInt32* Size, UInt8* Result );
extern int CompressFile(char* SourceFileName, char* CompressFileName);
extern int CompressFile(char* AdditionInformation, char* SourceFileName, char* CompressFileName);
//------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
#endif	/* _ZLIBUTILITY_H */

