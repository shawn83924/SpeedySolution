//---------------------------------------------------------------------------
#include "PInt32.h"
#include "FileStream.h"
#include "BufferedLog.h"
//---------------------------------------------------------------------------
namespace UFC
{
#ifndef _WIN32
//---------------------------------------------------------------------------
UFC::AnsiString  ShmNameToFileName( const UFC::AnsiString& ShmName )
{
    UFC::AnsiString  FileName, TmpName( ShmName );
    
    for( int i = 0; i < TmpName.Length(); i ++ )
        if( TmpName[i] == '/' || TmpName[i] == '.' || TmpName[i] == ' ')///< Replace '/ ''.' and space char.
            TmpName[i] = '_';    
    FileName.Printf( "/tmp/UFC/shm/%s", TmpName.c_str() );          
    return FileName;
}
//---------------------------------------------------------------------------
key_t GetShareMemoryUniqueKey( const UFC::AnsiString& Name )
{
    UFC::AnsiString   FileText;
    UFC::AnsiString   FileName = ShmNameToFileName( Name );
    key_t             UKey;    
    bool              FirstTimeCreateFile = false;

    UFC::CreateDir( "/tmp/UFC" );
    UFC::CreateDir( "/tmp/UFC/shm" );        
    umask( 0 );
    if( UFC::FileExists( FileName ) == FALSE ) ///< File not exists. first time create file.
    {    
        UFC::FileStream  FileForUKey( FileName, O_CREAT|O_TRUNC|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO  );
       
        FileText.Printf( "[0x%08x] Get IPC key from file[%s].\n\n", (long)0, FileName.c_str());
        FileForUKey.Write( FileText.c_str(), FileText.Length() + 1 );
        FileForUKey.Flush();
        FirstTimeCreateFile = true;
    }        
    if( (UKey = ftok( FileName.c_str(),  1  )) != -1 ) ///< get unique token from file.   
    {
        UFC::FileStream  FileForUKey( FileName, O_RDWR,  S_IRWXU|S_IRWXG|S_IRWXO  );
       
        FileText.Printf( "[0x%08x]", (long)UKey );
        if( FirstTimeCreateFile == true ) ///< First time create the file, Update unique token to file.
        {
            FileForUKey.Seek( 0, soFromBeginning );
            FileForUKey.Write( FileText.c_str(), FileText.Length()  );
            FileForUKey.Flush();                        
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Get IPC key[0x%08x] from file[%s].", (long)UKey, FileName.c_str());                 
        }
        else ///< File already exists, compare the unique token.
        {
            char PrevTok[ 32 ];
            
            memset( PrevTok, 0, 32 );            
            FileForUKey.Seek( 0, soFromBeginning );
            FileForUKey.Read( PrevTok, FileText.Length()  );
            if( FileText.AnsiCompare( PrevTok ) != 0 )
            {
                UFC::BufferedLog::Printf( " Warning!! Get IPC key from file[%s] changed!", FileName.c_str() );
                UFC::BufferedLog::Printf( " Previous Key[%s] Now[%s]", PrevTok, FileText.c_str() );
                FileText.Printf( "[0x%08x] Get IPC key from file[%s].\n\n", (long)UKey, FileName.c_str());
                FileForUKey.Seek( 0, soFromBeginning );                
                FileForUKey.Write( FileText.c_str(), FileText.Length() + 1 );
                FileForUKey.Flush();
            }    
            else
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Get IPC key[0x%08x] from file[%s].", (long)UKey, FileName.c_str());                 
        }
    }         
    else ///< Get token failed.
    {
         UFC::BufferedLog::Printf( " Get IPC key from file[%s] failed, Code[%d]", FileName.c_str(), errno );
         exit( 0 );
    }    
    return UKey;
}
//---------------------------------------------------------------------------
PInt32::PInt32( void )
:FShmID( -1 )
,FInt32Ptr( NULL )
,FName( "" )
{
}
//---------------------------------------------------------------------------
PInt32::PInt32( const char* Name )
:FShmID( -1 )
,FInt32Ptr( NULL )
,FName( Name )
{
    UFC::AnsiString UniqueName( Name );
    Attach( UniqueName );
}
//---------------------------------------------------------------------------
PInt32::PInt32( AnsiString& Name )
:FShmID( -1 )
,FInt32Ptr( NULL )
,FName( Name )
{
    Attach( Name );
}
//---------------------------------------------------------------------------
PInt32::PInt32( AnsiString& Name, Int32 Val )
:FInt32Ptr( NULL )
,FName( Name )
{
    Attach( Name, Val );
}
//---------------------------------------------------------------------------
PInt32::~PInt32( void )
{
    Detach();
}
//---------------------------------------------------------------------------
BOOL PInt32::Exists( const AnsiString& Name )
{
    key_t Key = GetShareMemoryUniqueKey( Name );
    Int32  ShmID;

    UFC::BufferedLog::Printf( " Check shm[%s] ID[0x%08x] exists.", Name.c_str(), (long)Key );
    if( (ShmID = shmget( Key, 4, 0666)) != -1 )///< shm exists.
        return TRUE;
    return FALSE;
}
//---------------------------------------------------------------------------
void PInt32::Attach( AnsiString& Name )
{
    key_t Key = GetShareMemoryUniqueKey( Name );

    FName = Name;
    UFC::BufferedLog::Printf( " Attach shm[%s] ID[0x%08x]", Name.c_str(), (long)Key );
    Attach( Key );    
}
//---------------------------------------------------------------------------
void PInt32::Attach( AnsiString& Name, Int32 InitVal )
{
    key_t Key = GetShareMemoryUniqueKey( Name );

    FName = Name;
    UFC::BufferedLog::Printf( " Attach shm[%s] ID[0x%08x]", Name.c_str(), (long)Key );
    Attach( Key, InitVal );
}
//---------------------------------------------------------------------------
void PInt32::Attach( long Key )
{
    Detach();    
    if( (FShmID = shmget( Key, 4, IPC_CREAT | 0666)) == -1 )
        throw( ShareMenException( errno ));
    if( (FInt32Ptr = (Int32*)shmat( FShmID, NULL, 0)) == (Int32*)-1 )
        throw( ShareMenException( errno ));        
}
//---------------------------------------------------------------------------
void PInt32::Attach( long Key, Int32 InitVal )
{
    Detach();    
    if( (FShmID = shmget( Key, 4, 0666)) == -1 )///< shm not exists.
    {
        if( (FShmID = shmget( Key, 4, IPC_CREAT | 0666)) == -1 )
            throw( ShareMenException( errno ));
        if( (FInt32Ptr = (Int32*)shmat( FShmID, NULL, 0)) == (Int32*)-1 )
            throw( ShareMenException( errno ));
        *FInt32Ptr = InitVal;
    }
    else
    {        
        if( (FInt32Ptr = (Int32*)shmat( FShmID, NULL, 0)) == (Int32*)-1 )
            throw( ShareMenException( errno ));
    }
}
//---------------------------------------------------------------------------
void PInt32::Detach( void )
{
    if( FInt32Ptr != NULL )
    {
        shmdt( (void*)FInt32Ptr );
        FInt32Ptr = NULL;
    }
}
//---------------------------------------------------------------------------
void PInt32::Remove( void )
{
    if( FShmID >= 0 )
    {
        Detach();
        shmid_ds ds;

        if( shmctl( FShmID, IPC_STAT, &ds ) == 0 )
            if( shmctl( FShmID, IPC_RMID, &ds ) == 0 )
                FShmID = -1;        
        if( FShmID >= 0 )
            throw( ShareMenException( errno ) );
        if( FName.Length() > 0 )
        {
            UFC::AnsiString FileName = ShmNameToFileName( FName );
            UFC::RemoveFile( FileName );        
        }        
    }
}
//---------------------------------------------------------------------------
void PInt32::Set( Int32 Val )
{
    if( FInt32Ptr != NULL )        
        *FInt32Ptr = Val;
}
//---------------------------------------------------------------------------
Int32 PInt32::ToInt32( void )
{
    if( FInt32Ptr == NULL )
        return 0;
    return *FInt32Ptr;
}
//---------------------------------------------------------------------------
PInt32& PInt32::operator = ( Int32 Val )
{
    if( FInt32Ptr != NULL )
        *FInt32Ptr = Val;
    return *this;
}
//---------------------------------------------------------------------------
PInt32& PInt32::operator +=( Int32 Val )
{
    if( FInt32Ptr != NULL )
        *FInt32Ptr += Val;
    return *this;
}
//---------------------------------------------------------------------------
#else
//---------------------------------------------------------------------------
PInt32::PInt32( void )
:FFileMap( NULL )
,FFile( NULL )
,FInt32Ptr( NULL )
,FFileName( "" )
{
}
//---------------------------------------------------------------------------
PInt32::PInt32( const char* Name )
:FFileMap( NULL )
,FFile( NULL )
,FInt32Ptr( NULL )
,FFileName( "" )
{
    UFC::AnsiString UniqueName( Name );
    Attach( UniqueName );
}
//---------------------------------------------------------------------------
PInt32::PInt32( AnsiString& Name )
:FFileMap( NULL )
,FFile( NULL )
,FInt32Ptr( NULL )
,FFileName( "" )
{
    Attach( Name );
}
//---------------------------------------------------------------------------
PInt32::PInt32( AnsiString& Name, Int32 Val )
:FFileMap( NULL )
,FFile( NULL )
,FInt32Ptr( NULL )
,FFileName( "" )
{
    Attach( Name, Val );
}
//---------------------------------------------------------------------------
PInt32::~PInt32( void )
{
    Detach();
}
//---------------------------------------------------------------------------
void PInt32::Attach( AnsiString& Name )
{
	UFC::BufferedLog::Printf( " Attach shm[%s]", Name.c_str());
	FFileName.Printf( "Persistent\\%s",Name.c_str());
	CreateDirectoryA( "Persistent",NULL );

	if((FFile = CreateFileA( FFileName.c_str(),                  // name of existing file
							GENERIC_READ | GENERIC_WRITE,      // read/write access
							0,                                 // no sharing
							NULL,                              // default security
							OPEN_ALWAYS,                       // open existing or new
							FILE_ATTRIBUTE_NORMAL,             // file attributes
							NULL))== INVALID_HANDLE_VALUE)     // no template
		throw( ShareMenException( -1 ));

	if(( FFileMap = CreateFileMappingA( FFile, NULL, PAGE_READWRITE, 0, 4, Name.c_str() ))== INVALID_HANDLE_VALUE)
		throw( ShareMenException( -2 ));

	if((FInt32Ptr = (Int32*)MapViewOfFile( FFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 4 )) == NULL )
		throw( ShareMenException( -3 ));
}
//---------------------------------------------------------------------------
void PInt32::Attach( AnsiString& Name, Int32 InitVal )
{
	BOOL Exist = TRUE;

	UFC::BufferedLog::Printf( " Attach shm[%s]", Name.c_str());
	FFileName.Printf( "Persistent\\%s",Name.c_str());
	CreateDirectoryA( "Persistent",NULL );
    if( UFC::FileExists( FFileName ) == FALSE )
        Exist = FALSE;
	if((FFile = CreateFileA( FFileName.c_str(),                  // name of existing file
							GENERIC_READ | GENERIC_WRITE,      // read/write access
							0,                                 // no sharing
							NULL,                              // default security
							OPEN_ALWAYS,                       // open existing or new
							FILE_ATTRIBUTE_NORMAL,             // file attributes
							NULL))== INVALID_HANDLE_VALUE)     // no template
		throw( ShareMenException( -1 ));
	if(( FFileMap = CreateFileMappingA( FFile, NULL, PAGE_READWRITE, 0, 4, Name.c_str() ))== INVALID_HANDLE_VALUE)
		throw( ShareMenException( -2 ));
	if((FInt32Ptr = (Int32*)MapViewOfFile( FFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 4 )) == NULL )
		throw( ShareMenException( -3 ));
	if( Exist == FALSE )
		*FInt32Ptr = InitVal;
}
//---------------------------------------------------------------------------
void PInt32::Attach( long Key )
{
    UFC::AnsiString Name;

    Name.Printf( "ID%d", Key );
    Attach( Name );
}    
//---------------------------------------------------------------------------
void PInt32::Attach( long Key, Int32 InitVal )
{
    UFC::AnsiString Name;

    Name.Printf( "ID%d", Key );
    Attach( Name, InitVal );    
}    
//---------------------------------------------------------------------------
void PInt32::Detach( void )
{
    if( FInt32Ptr != NULL )
    {
        FlushViewOfFile( FInt32Ptr, 4 );
        UnmapViewOfFile( (void*)FInt32Ptr );
		FInt32Ptr = NULL;
		if( FFileMap != NULL )
		{
			CloseHandle( FFileMap );
			FFileMap = NULL;
			if( FFile != NULL )
			{
				CloseHandle( FFile );
				FFile = NULL;
			}
		}
	}
	FFileName = "";
}
//---------------------------------------------------------------------------
void PInt32::Remove( void )
{
	AnsiString FileName = FFileName;
    
    Detach();
    
    if( FileName.Length() > 0 )
        UFC::RemoveFile( FileName );
}
//---------------------------------------------------------------------------
Int32 PInt32::ToInt32( void )
{
    if( FInt32Ptr == NULL )
        return 0;
    return *FInt32Ptr;
}
//---------------------------------------------------------------------------
PInt32& PInt32::operator = ( Int32 Val )
{
    if( FInt32Ptr != NULL )
    {
        *FInt32Ptr = Val;
        FlushViewOfFile( FInt32Ptr, 4 );
    }
    return *this;
}
//---------------------------------------------------------------------------
PInt32& PInt32::operator +=( Int32 Val )
{
    if( FInt32Ptr != NULL )
    {
        *FInt32Ptr += Val;
        FlushViewOfFile( FInt32Ptr, 4 );
    }
    return *this;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
/*PInt32Array::PInt32Array( void )
:FInt32Ptr( NULL )
,FCount( 0 )
{
}
//---------------------------------------------------------------------------
PInt32Array::PInt32Array( AnsiString& Name, Int32 Count )
:FInt32Ptr( NULL )
{
    Attach( Name, Count );
}
//---------------------------------------------------------------------------
PInt32Array::PInt32Array( AnsiString& Name, Int32 Count, Int32 InitVal )
:FInt32Ptr( NULL )
{
    Attach( Name, Count );
    for( int i = 0; i < Count; i++ )
         *(FInt32Ptr+i) = InitVal;
}
//---------------------------------------------------------------------------
void PInt32Array::Attach( const AnsiString& Name, Int32 Count )
{
	UFC::AnsiString FileName;

	FileName.Printf( "Persistent\\%s",Name.c_str());
    CreateDirectory( "Persistent",NULL );
    if((FFile = CreateFile( FileName.c_str(),                  // name of existing file
                            GENERIC_READ | GENERIC_WRITE,      // read/write access
                            0,                                 // no sharing
                            NULL,                              // default security
                            OPEN_ALWAYS,                       // open existing or new
                            FILE_ATTRIBUTE_NORMAL,             // file attributes
                            NULL))== INVALID_HANDLE_VALUE)     // no template
        throw( ShareMenException( -1 ));
    if(( FFileMap = CreateFileMapping( FFile, NULL, PAGE_READWRITE, 0, Count* sizeof(Int32), Name.c_str() ))== INVALID_HANDLE_VALUE)
        throw( ShareMenException( -2 ));
    if((FInt32Ptr = (Int32*)MapViewOfFile( FFileMap, FILE_MAP_ALL_ACCESS, 0, 0, Count* sizeof(Int32) )) == NULL )
        throw( ShareMenException( -3 ));
}
//---------------------------------------------------------------------------
PInt32Array::~PInt32Array( )
{
    if( FInt32Ptr != NULL )
    {
        FlushViewOfFile( FInt32Ptr, 4 );
        UnmapViewOfFile( (void*)FInt32Ptr );
    }
    CloseHandle( FFileMap );
    CloseHandle( FFile );
}
//---------------------------------------------------------------------------
Int32& PInt32Array::operator []( int index ) throw( IndexOutOfBoundException )
{
    if( index < 0 || index >= FCount )
        throw IndexOutOfBoundException( index );
    return *(FInt32Ptr+index);
}*/
//---------------------------------------------------------------------------
#endif
}
//---------------------------------------------------------------------------
