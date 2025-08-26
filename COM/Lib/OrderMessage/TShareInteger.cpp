#include "TShareInteger.h"
#include <SysUtils.hpp>
//---------------------------------------------------------------------------
TShareInteger::TShareInteger(void)
:FInt32Ptr( NULL )
,FFileMap( NULL )
{
}
//---------------------------------------------------------------------------
TShareInteger::~TShareInteger(void)
{
	Detach();
}
//---------------------------------------------------------------------------
void TShareInteger::Attach( const char* ShareMemoryName )
{
	if( !OpenShareMemory( ShareMemoryName ) )
		if( !CreateShareMemory( ShareMemoryName ) )
			return;

	if( !MapIntegeAddress() )
		Detach();
}
//---------------------------------------------------------------------------
bool TShareInteger::OpenShareMemory( const char* ShareMemoryName )
{
	Detach();
	FFileMap = OpenFileMapping( FILE_MAP_ALL_ACCESS,// access mode
								0,                  // inherit flag
								ShareMemoryName );	// pointer to name of file-mapping object

	if( (FFileMap != INVALID_HANDLE_VALUE) && (FFileMap != NULL) )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool TShareInteger::CreateShareMemory( const char* ShareMemoryName )
{
	UFC::AnsiString FileDir,FileName;

	FileDir.Printf( "%s\\ShareInteger", GetCurrentDir().c_str() );
	FileName.Printf( "%s\\%s", FileDir.c_str(), ShareMemoryName );
	CreateDirectory( FileDir.c_str(), NULL );

	HANDLE hFile = CreateFile( FileName.c_str(),                // pointer to name of the file
							   GENERIC_READ | GENERIC_WRITE,    // access (read-write) mode
							   0,                               // share mode
							   NULL,                            // pointer to security attributes
							   OPEN_ALWAYS,                     // how to create
							   FILE_ATTRIBUTE_NORMAL,           // file attributes
							   NULL );                          // handle to file with attributes to copy

	if( (hFile != INVALID_HANDLE_VALUE) && (hFile != NULL) )
	{
		FFileMap = CreateFileMapping( hFile,                    // handle to file to map
									  NULL,                     // optional security attributes
									  PAGE_READWRITE,           // protection for mapping object
									  0,                        // high-order 32 bits of object size
									  4,                        // low-order 32 bits of object size
									  ShareMemoryName );		// name of file-mapping object

		CloseHandle( hFile );
		if( (FFileMap != INVALID_HANDLE_VALUE) && (FFileMap != NULL) )
			return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool TShareInteger::MapIntegeAddress( void )
{
	FInt32Ptr = (int*)MapViewOfFile( FFileMap,              // file-mapping object to map into address space
									 FILE_MAP_ALL_ACCESS,	// access mode
									 0,                     // high-order 32 bits of file offset
									 0,                     // low-order 32 bits of file offset
									 4 );                   // number of bytes to map
	if( FInt32Ptr != NULL )
		return true;
	return false;
}
//---------------------------------------------------------------------------
void TShareInteger::Detach( void )
{
	if( FInt32Ptr != NULL )
	{
		FlushViewOfFile( FInt32Ptr, 4 );
		UnmapViewOfFile( (void*)FInt32Ptr );
	}

	if( (FFileMap != INVALID_HANDLE_VALUE) && (FFileMap != NULL) )
		CloseHandle( FFileMap );

	FInt32Ptr   = NULL;
	FFileMap    = NULL;
}
//---------------------------------------------------------------------------
int TShareInteger::ToInt32( void )
{
	if( FInt32Ptr == NULL )
		return 0;
	return *FInt32Ptr;
}
//---------------------------------------------------------------------------
TShareInteger& TShareInteger::operator = ( int Val )
{
	if( FInt32Ptr != NULL )
	{
		*FInt32Ptr = Val;
		FlushViewOfFile( FInt32Ptr, 4 );
	}
	return *this;
}
//---------------------------------------------------------------------------
TShareInteger& TShareInteger::operator += ( int Val )
{
	if( FInt32Ptr != NULL )
	{
		*FInt32Ptr += Val;
		FlushViewOfFile( FInt32Ptr, 4 );
	}
	return *this;
}
//---------------------------------------------------------------------------

