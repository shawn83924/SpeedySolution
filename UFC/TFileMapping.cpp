#include "TFileMapping.h"
#include "NetUtility.h"
//---------------------------------------------------------------------------
// TMutex
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
TMutex::TMutex( void )
:FHandle( NULL )
{
}
//---------------------------------------------------------------------------
TMutex::~TMutex( void )
{
    Close();
}
//---------------------------------------------------------------------------
bool TMutex::Open( const UFC::AnsiString& Key, bool CreateNew )
{
    Close();
    UFC::AnsiString MutexName;
    MutexName.Printf( "mutex_%s", Key.c_str() );

    if( !CreateNew )
    {
		FHandle = OpenMutexA( NULL,                //安全屬性。NULL表示使用預設的屬性
							 FALSE,               //如果希望呼叫CreateMutex的這個執行緒擁有產生出來mutex就設為TRUE
							 MutexName.c_str() ); //mutex的名稱
		if( FHandle )
			return true;
	}
	else
	{
		FHandle = CreateMutexA( NULL,                //安全屬性。NULL表示使用預設的屬性
							   FALSE,               //如果希望呼叫CreateMutex的這個執行緒擁有產生出來mutex就設為TRUE
							   MutexName.c_str() ); //mutex的名稱
        if( FHandle )
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void TMutex::Close( void )
{
    if( FHandle )
        CloseHandle( FHandle );
}
//---------------------------------------------------------------------------
void TMutex::WaitFor( long Timeout )
{
    DWORD ReturnValue = WaitForSingleObject( FHandle, Timeout );
    switch( ReturnValue )
    {
        case WAIT_FAILED:    throw( UFC::Exception( "wait fail" ) );
        case WAIT_ABANDONED: throw( UFC::Exception( "ownership of the mutex object is granted to the calling thread, and the mutex is set to nonsignaled." ) );
        case WAIT_TIMEOUT:   throw( UFC::Exception( "wait timeout" ) );
    }
}
//---------------------------------------------------------------------------
void TMutex::Release( void )
{
    ReleaseMutex( FHandle );
}
//---------------------------------------------------------------------------
// TFileMapping
//---------------------------------------------------------------------------
TFileMapping::TFileMapping( void )
:FHandle( NULL )
,FAddress( NULL )
,FFileSize( 0 )
{
}
//---------------------------------------------------------------------------
TFileMapping::~TFileMapping( void )
{
    if( FHandle )
    {
        if( FAddress )
        {
            Flush( FAddress, 0 );
            UnmapViewOfFile( FAddress );
        }
        CloseHandle( FHandle );
        FFileSize = 0;
    }
}
//---------------------------------------------------------------------------
void* TFileMapping::MapView( void )
{
	FAddress = MapViewOfFile( FHandle,
							  FILE_MAP_READ | FILE_MAP_WRITE,
							  0,
							  0,
							  FFileSize );
	return FAddress;
}
//---------------------------------------------------------------------------
void* TFileMapping::Open( const UFC::AnsiString& Key, long Size, bool CreateNew )
{
	char TempPath[ 256 ];
	UFC::AnsiString Name;

	GetTempPathA( 256, TempPath );
	Name.Printf( "%s\\filemap_%s", TempPath, Key.c_str() );
	HANDLE hFile = CreateFileA( Name.c_str(),                       // pointer to name of the file
							   GENERIC_READ | GENERIC_WRITE,       // access (read-write) mode
							   FILE_SHARE_WRITE | FILE_SHARE_READ, // share mode
							   NULL,                               // pointer to security attributes
							   OPEN_EXISTING,                      // how to create
							   FILE_ATTRIBUTE_NORMAL,              // file attributes
							   NULL );                             // handle to file with attributes to copy
	if( hFile == INVALID_HANDLE_VALUE && CreateNew )
	{
		hFile = CreateFileA( Name.c_str(),                       // pointer to name of the file
							GENERIC_READ | GENERIC_WRITE,       // access (read-write) mode
							FILE_SHARE_WRITE | FILE_SHARE_READ, // share mode
							NULL,                               // pointer to security attributes
							OPEN_ALWAYS,                        // how to create
							FILE_ATTRIBUTE_NORMAL,              // file attributes
							NULL );                             // handle to file with attributes to copy
	}
	if( hFile != INVALID_HANDLE_VALUE )
	{
		Name.Printf( "filemap_%s", Key.c_str() );
		SYSTEM_INFO sinfo;
		GetSystemInfo( &sinfo );
		while( FFileSize < Size )
		{
			FFileSize += sinfo.dwAllocationGranularity;
		}

		FHandle = CreateFileMappingA( hFile,          // handle to file to map
									 NULL,           // optional security attributes
									 PAGE_READWRITE, // protection for mapping object
									 0,              // high-order 32 bits of object size
									 FFileSize,      // low-order 32 bits of object size
									 Name.c_str() ); // name of file-mapping object
		CloseHandle( hFile );
	}
	if( FHandle != NULL )
		return MapView();
	return NULL;
}
//---------------------------------------------------------------------------
void TFileMapping::Flush( void* Address, long Size )
{
    if( !FlushViewOfFile( Address, Size ) )
    {
        UFC::AnsiString ErrMsg( (int)GetLastError() );
        throw( UFC::Exception( ErrMsg.c_str() ) );
    }
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
