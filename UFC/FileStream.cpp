#include "FileStream.h"
#include "List.h"

//----------------------------------------------------------------------------------------------------------------------
namespace UFC
{
 //----------------------------------------------------------------------------------------------------------------------
UFC::AnsiString ExtractFileName(  const UFC::AnsiString& FullPathName )
{
    UFC::PStringList Paths;
    UFC::AnsiString  FileName( FullPathName );
        
    Paths.SetStrings( FullPathName, "/\\" );
    if( Paths.ItemCount() > 0 )
        FileName = Paths[ Paths.ItemCount()-1];
    return FileName;   
}    
//----------------------------------------------------------------------------------------------------------------------
UFC::AnsiString ExtractFilePath(  const UFC::AnsiString& FullPathName )
{
    UFC::PStringList Paths;
    UFC::AnsiString  FilePath(".");
        
    Paths.SetStrings( FullPathName, "/\\" );
    if( Paths.ItemCount() > 0 )
    {
        Paths.Delete( Paths.ItemCount()-1 );
        FilePath.Printf( "/%s/", Paths.DelimiterString('/').c_str() );	 
    }
    return FilePath;   
}    
//----------------------------------------------------------------------------------------------------------------------
BOOL CreateDir( const UFC::AnsiString& DirName )
{
#ifndef _WIN32
    if( mkdir( DirName.c_str(), 0666 ) == 0 )
    {
        UFC::AnsiString CMD;

        CMD.Printf( "chmod 755 %s", DirName.c_str() );
        system( CMD.c_str() );
        return true;
    }
    return false;
#else
	return CreateDirectoryA( DirName.c_str(), NULL );
#endif
}
//----------------------------------------------------------------------------------------------------------------------
BOOL RemoveFile( const UFC::AnsiString& FileName )
{
#ifdef _MSC_VER
	if( _unlink( FileName.c_str() ) == -1 )
		return FALSE;
#else
	if( unlink( FileName.c_str() ) == -1 )
		return FALSE;
#endif
	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------
BOOL RenameFile( const UFC::AnsiString& OldFileName, const UFC::AnsiString& NewFileName )
{
/// Allen mark at 20190614
//#ifdef _MSC_VER
//	if( _rename( OldFileName.c_str(), NewFileName.c_str() ) != 0 )
//		return FALSE;
//#else
	if( rename( OldFileName.c_str(), NewFileName.c_str() ) == -1 )
		return FALSE;
//#endif
	return TRUE;
}
//----------------------------------------------------------------------------------------------------------------------
BOOL FileExists( const UFC::AnsiString& FileName )
{
#ifdef _WIN32
	WIN32_FIND_DATAA FindData;
	HANDLE           FindHandle;

	FindHandle = FindFirstFileA( FileName.c_str(), &FindData );
	if( FindHandle != INVALID_HANDLE_VALUE  )
	{
		 FindClose( FindHandle );
		 return !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY );
	}
	return FALSE;
#else
	int FD = open( FileName.c_str(), O_RDONLY, 0664 );

	if( FD  == -1 )
		return FALSE;
	close( FD );
	return TRUE;
#endif
}
//---------------------------------------------------------------------------
bool IsTodayFile( const UFC::AnsiString& FilePathName )
{
#ifndef _WIN32
    struct stat fileStat;
    if (stat(FilePathName.c_str(), &fileStat) == 0)
    {
        struct tm* fileTimeInfo = localtime(&fileStat.st_mtime );
        int fileDay = fileTimeInfo->tm_mday;

        time_t now;
        time(&now);
        struct tm* nowTM = localtime(&now);
        int nowDay = nowTM->tm_mday;
        if (fileDay == nowDay) return true;
    }
    return false;
#else
    return false;
#endif
}  //IsTodayFile()
//---------------------------------------------------------------------------
bool GetFileModificationTime( const UFC::AnsiString& FilePathName,
                              int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second )
{
#ifndef _WIN32
    struct stat fileStat;
    if( stat( FilePathName.c_str(), &fileStat ) == 0 )
    {
        struct tm* fileTimeInfo = localtime( &fileStat.st_mtime );
        Year = fileTimeInfo->tm_year + 1900;
        Month = fileTimeInfo->tm_mon + 1;
        Day = fileTimeInfo->tm_mday;
        Hour = fileTimeInfo->tm_hour;
        Minute = fileTimeInfo->tm_min;
        Second = fileTimeInfo->tm_sec;
        return true;
    }
    else
        return false;
#else
    return false;
#endif    
}  //GetFileModificationTime()
//---------------------------------------------------------------------------
bool GetFileModificationSecond( const UFC::AnsiString& FilePathName, long long& ModificationSecond )
{
#if !defined(_WIN32) && !defined(_AIX)
    struct stat fileStat;
    if( stat( FilePathName.c_str(), &fileStat ) == 0 )
    {
        ModificationSecond = static_cast<long long>( fileStat.st_mtim.tv_sec );
        return true;
    }
    else
        return false;
#else
    return false;
#endif    
}  //GetFileModificationSecond()

//----------------------------------------------------------------------------------------------------------------------
//
// Class FileStream
// wrap the basic file I/O function to a file stream class.( open, close, seek, read,write...)
//
//----------------------------------------------------------------------------------------------------------------------
FileStream::FileStream( UFC::AnsiString FileName,int Mode, unsigned long Permissions )
:PStream( )
,FFD( -1 )
,FFileName( "" )
{
    if( ( FFD = open( FileName.c_str(), Mode, Permissions )) == -1 )
    {
        UFC::AnsiString Msg = "File Exception: Open file " + FileName + " failed.";
        throw( FileException( (char*) Msg.c_str()));
    }
    FFileName = FileName;
}
//----------------------------------------------------------------------------------------------------------------------
FileStream::~FileStream()
{
    FPos = 0;
    if( FFD != -1 )
    {
        close( FFD );
        FFD = -1;
    }
}
//----------------------------------------------------------------------------------------------------------------------
int FileStream::Read( void* Buffer, int Size )
{
    int ReadBytes = read( FFD, Buffer, Size );

    if( ReadBytes == 0 ) // EOF
    {
        return 0;
    }
    else if( ReadBytes < 0 ) // Error
    {
        throw( FileException("File Exception: Read file failed."));
    }
    else
    {
        FPos += ReadBytes;
        return ReadBytes;
    }
}
//----------------------------------------------------------------------------------------------------------------------
int FileStream::Write( const void* Buffer, int Size )
{
    int WriteBytes = write( FFD, Buffer, Size );
    if( WriteBytes >= 0 )
    {
        FPos += WriteBytes;
        return WriteBytes;
    }
    else
    {
        throw( FileException("File Exception: Write file failed."));
    }
}
//----------------------------------------------------------------------------------------------------------------------
Int64 FileStream::Seek( Int64 Offset, int Origin )
{
    FPos = lseek( FFD, (long) Offset, Origin );
    return FPos;
}
//----------------------------------------------------------------------------------------------------------------------
const Int64 FileStream::GetSize( void )
{
    Int32 CurrentPos = FPos;
    Int32 Size = Seek( 0, soFromEnd );

    Seek( CurrentPos, soFromBeginning );
    return Size;
}
//----------------------------------------------------------------------------------------------------------------------
AnsiString FileStream::ReadLine( void )
{
    Int32  ReadBytes = 0;
    char               ReadChar;
    
    UFC::PStringBuffer StringBuffer( 256 );

    do
    {
        if( read( FFD, &ReadChar, 1 ) == 1 )
        {
            StringBuffer.Append( ReadChar );
            ReadBytes ++;
        }
        else
            return UFC::AnsiString();

    }while( ReadChar != '\n' );
    
    FPos += ReadBytes;
    return StringBuffer.ToString();
}
//----------------------------------------------------------------------------------------------------------------------
BOOL FileStream::ReadyToRead( int TimeoutSec )
{
    fd_set readfd;
    struct timeval timeout;

    timeout.tv_sec  = TimeoutSec;
    timeout.tv_usec = 0;
    FD_ZERO( &readfd );
	FD_SET( (u_int)FFD, &readfd );
    select( FFD+1, &readfd, NULL, NULL, &timeout );
    if( FD_ISSET( FFD, &readfd ) )
        return TRUE;
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
BOOL FileStream::ReadyToWrite( int TimeoutSec )
{
    fd_set writefd;
    struct timeval timeout;

    timeout.tv_sec  = TimeoutSec;
    timeout.tv_usec = 0;
    FD_ZERO( &writefd );
    FD_SET( (u_int)FFD, &writefd );
    select( FFD+1, NULL, &writefd, NULL, &timeout );
    if( FD_ISSET( FFD, &writefd ) )
        return TRUE;
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
void FileStream::Flush()
{
#ifdef _WIN32
	_commit( FFD );
#else
    fsync( FFD );
#endif
}
//----------------------------------------------------------------------------------------------------------------------
//
// Class FileStreamEx
// wrap the buffered file I/O function to a file stream class.( fopen, fclose, fseek, fread, fwrite...)
//
//----------------------------------------------------------------------------------------------------------------------
FileStreamEx::FileStreamEx( UFC::AnsiString FileName, const char* Mode, BOOL FlushAfterWrite, Int32 StreamBufferSize )
:PStream( )
,FHandle( NULL )
,FFileName( FileName )
,FFlushAfterWrite( FlushAfterWrite )
,FBuffer( NULL )
{
#ifdef _WIN32
        if( ( FHandle = fopen( FileName.c_str(), Mode ) ) == NULL )
        {
            UFC::AnsiString Msg = "File Exception: Open file " + FileName + " failed.";
            throw( FileException( (char*) Msg.c_str()));
        }
#else
        if( ( FHandle = fopen64( FileName.c_str(), Mode )) == NULL )
        {
            UFC::AnsiString Msg = "File Exception: Open file " + FileName + " failed.";
            throw( FileException( (char*) Msg.c_str()));
        }
#endif
        if (StreamBufferSize > 0)
        {
            errno = 0;
            FBuffer = new char[StreamBufferSize];
            if (0 != setvbuf( FHandle, FBuffer, _IOFBF, StreamBufferSize))
            {
                if (errno==0 || errno==EINVAL)
                {
                    printf(" Open file [%s] setvbuf(_IOFBF, %d) fail, errno=%d(%s), use default\n", FileName.c_str(), StreamBufferSize, errno, strerror(errno) ) ;
                }
                else
                {
                    UFC::AnsiString Msg = "";
                    Msg.Printf(" Error Open file [%s] setvbuf(_IOFBF, %d) fail, errno=%d(%s)\n", FileName.c_str(), StreamBufferSize, errno, strerror(errno) ) ;
                    throw( FileException( (char*) Msg.c_str()));
                }
            }
        }
}
//----------------------------------------------------------------------------------------------------------------------
FileStreamEx::~FileStreamEx()
{
    FPos = 0;
    if( FHandle != NULL )
    {
        fclose( FHandle );
        FHandle = NULL;
    }
    if (FBuffer != NULL)
    {
       delete FBuffer;
       FBuffer = NULL;
    }    
}
//----------------------------------------------------------------------------------------------------------------------
int FileStreamEx::Read( void* Buffer, int Size )
{
    int ReadBytes = (int)fread( Buffer, 1, Size, FHandle );
    if( ReadBytes == 0 )
    {
        return 0;
    }
    else if( ReadBytes == EOF )
    {
        throw( FileException("File Exception: Read file failed."));
    }
    else
    {
        FPos += ReadBytes;
        return ReadBytes;
    }
}
//----------------------------------------------------------------------------------------------------------------------
int FileStreamEx::Write( const void* Buffer, int Size )
{
    int WriteItem = (int)fwrite( Buffer, Size, 1, FHandle );

    if( WriteItem == 0 )
    {
        return 0;
    }
    else if( WriteItem == EOF )
    {
        throw( FileException("File Exception: Write file failed."));
    }
    else
    {
        if( FFlushAfterWrite == TRUE )
            Flush();
        FPos += Size;
        return Size;
    }
}
//----------------------------------------------------------------------------------------------------------------------
Int64 FileStreamEx::Seek( Int64 Offset, int Origin )
{
#ifdef _WIN32
	if( _fseeki64( FHandle, Offset, Origin ) == -1 )
		throw( FileException("File Exception: seek file failed."));
	FPos = _ftelli64( FHandle );
	return FPos;
#else
	if( fseeko64( FHandle, Offset, Origin ) == -1 )
		throw( FileException("File Exception: seek file failed."));
	FPos = ftello64( FHandle );
	return FPos;
#endif
}
//----------------------------------------------------------------------------------------------------------------------
const Int64 FileStreamEx::GetSize( void )
{
    Int64 CurrentPos = FPos;
    Int64 Size = Seek( 0, soFromEnd ); ///< Seek to end of file

    Seek( CurrentPos, soFromBeginning );
    return Size;
}
//----------------------------------------------------------------------------------------------------------------------
BOOL FileStreamEx::ReadLine( UFC::AnsiString&  Line )
{
    int  ReadChar;
    int  ReadBytes = 0;
    BOOL NotEOF = TRUE;
    UFC::PStringBuffer StringBuffer(512);

    do
    {
        if ((ReadChar = fgetc(FHandle)) == EOF)
        {
            NotEOF = FALSE;
            clearerr( FHandle );
            break;
        }
        StringBuffer.Append((char)ReadChar);
        ReadBytes ++;
    } while(ReadChar != '\n');
    
    Line = StringBuffer.ToString();
    FPos += ReadBytes;
    
    return  NotEOF;
}
//----------------------------------------------------------------------------------------------------------------------
//
// Class FileStream64
// wrap the buffered file I/O function to a file stream class.( fopen, fclose, fseek, fread, fwrite...)
//
//----------------------------------------------------------------------------------------------------------------------
FileStream64::FileStream64( UFC::AnsiString FileName, const char* Mode, BOOL FlushAfterWrite, Int32 StreamBufferSize )
:PStream()
,FHandle( NULL )
,FFileName( FileName )
,FFlushAfterWrite( FlushAfterWrite )
,FBuffer( NULL )
{
#ifdef _WIN32
        if( ( FHandle = fopen( FileName.c_str(), Mode ) ) == NULL )
        {
                UFC::AnsiString Msg = "File Exception: Open file " + FileName + " failed.";
                throw( FileException( (char*) Msg.c_str()));
        }
#else
        if( ( FHandle = fopen64( FileName.c_str(), Mode ) ) == NULL )
        {
                UFC::AnsiString Msg = "File Exception: Open file " + FileName + " failed.";
                throw( FileException( (char*) Msg.c_str()));
        }
#endif
        if (StreamBufferSize > 0)
        {
            errno = 0;
            FBuffer = new char[StreamBufferSize];
            if (0 != setvbuf( FHandle, FBuffer, _IOFBF, StreamBufferSize))
            {
                if (errno==0 || errno==EINVAL)
                {
                    printf(" Open file [%s] setvbuf(_IOFBF, %d) fail, errno=%d(%s), use default\n", FileName.c_str(), StreamBufferSize, errno, strerror(errno) ) ;
                }
                else
                {
                    UFC::AnsiString Msg = "";
                    Msg.Printf(" Error Open file [%s] setvbuf(_IOFBF, %d) fail, errno=%d(%s)\n", FileName.c_str(), StreamBufferSize, errno, strerror(errno) ) ;
                    throw( FileException( (char*) Msg.c_str()));
                }
            }
        }
}
//----------------------------------------------------------------------------------------------------------------------
FileStream64::~FileStream64()
{
    FPos = 0;
    if( FHandle != NULL )
    {
        fclose( FHandle );
        FHandle = NULL;
    }
    if (FBuffer != NULL)
    {
       delete FBuffer;
       FBuffer = NULL;
    }
}
//----------------------------------------------------------------------------------------------------------------------
int FileStream64::Read( void* Buffer, int Size )
{    
    int ReadBytes = (int)fread( Buffer, 1, Size, FHandle );

    if( ReadBytes == 0 )
    {
        return 0;
    }
    else if( ReadBytes == EOF )
    {
        throw( FileException("File Exception: Read file failed."));
    }
    else
    {
        FPos += ReadBytes;
        return ReadBytes;
    }
}
//----------------------------------------------------------------------------------------------------------------------
int FileStream64::Write( const void* Buffer, int Size )
{
    int WriteItem = (int)fwrite( Buffer, Size, 1, FHandle );

    if( WriteItem == 0 )
    {
        return 0;
    }
    else if( WriteItem == EOF )
    {
        throw( FileException("File Exception: Write file failed."));
    }
    else
    {
        if( FFlushAfterWrite == TRUE )
            Flush();
        FPos += Size;
        return Size;
    }
}
//----------------------------------------------------------------------------------------------------------------------
Int64 FileStream64::Seek( Int64 Offset, int Origin )
{
#ifdef _WIN32
	if( _fseeki64( FHandle, Offset, Origin ) == -1 )
		throw( FileException("File Exception: seek file failed."));
	FPos = _ftelli64( FHandle );
	return FPos;
#else
	if( fseeko64( FHandle, Offset, Origin ) == -1 )
            throw( FileException("File Exception: seek file failed."));
	FPos = ftello64( FHandle );
	return FPos;
#endif
}
//----------------------------------------------------------------------------------------------------------------------
const Int64 FileStream64::GetSize( void )
{
    Int64 CurrentPos = FPos;
    Int64 Size = Seek( 0, soFromEnd ); ///< Seek to end of file

    Seek( CurrentPos, soFromBeginning );
    return Size;
}
//----------------------------------------------------------------------------------------------------------------------
FileChangedWatchDog::FileChangedWatchDog( const UFC::AnsiString& FileName, FileChangedListrner* Listener, void* Data, int Timeout )
:PThread( NULL, TRUE )
,FListener( Listener )
,FFileName( FileName )
,FModifyTime( 0 )
,FTimeOutSec( Timeout )
,FUDD( Data )
{
    if( FileExists( FFileName ) == TRUE )
    {
        struct stat  FileStat;

        stat( FFileName.c_str(), &FileStat );                
        FModifyTime = FileStat.st_mtime;
    }
    Start();
}    
//----------------------------------------------------------------------------------------------------------------------
BOOL FileChangedWatchDog::CheckLoop( Int64 BeginSize )    
{
    int         CountDown = FTimeOutSec;
    Int64       CurrSize = BeginSize;
    struct stat FileStat;
    
    while( CountDown > 0 )
    {        
        UFC::SleepMS( 1000 );
        if( FileExists( FFileName ) == TRUE )
        {
            stat( FFileName.c_str(), &FileStat );                        
            if( CurrSize != FileStat.st_size )    
            {   ///< File size changed
                CurrSize  = FileStat.st_size; ///< Assign new size
                CountDown = FTimeOutSec;      ///< Reset counter 
            }
            else
                CountDown --;
        }
        else
           return FALSE;///< File deleted                
    }
    FModifyTime = FileStat.st_mtime;
    return TRUE;
}
//----------------------------------------------------------------------------------------------------------------------
void FileChangedWatchDog::Execute( void )
{
    while( TRUE )
    {
        if( FileExists( FFileName ) == TRUE )
        {
            struct stat  FileStat;
    
            stat( FFileName.c_str(), &FileStat );            
            if( FModifyTime != FileStat.st_mtime )    
            {
                if( CheckLoop( FileStat.st_size ) == TRUE &&
                    FListener != NULL )
                    FListener->OnFileChanged( FFileName, FUDD );
            }    
        }
        UFC::SleepMS( 1000 );
    };
}
//----------------------------------------------------------------------------------------------------------------------
} ///< Close namespace UFC
//----------------------------------------------------------------------------------------------------------------------


