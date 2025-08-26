#include "PEasyFTPClient.h"
//--------------------------------------------------------------------------------------
namespace UFC
{
#ifndef _WIN32
//--------------------------------------------------------------------------------------
size_t PEasyFTPClient::WriteCallback( void* Buffer, size_t size, size_t nmemb, void* StoreFile )
{
    FILE* DataFile = (FILE*)StoreFile;
    if( DataFile != NULL )
        return fwrite( Buffer, size, nmemb, DataFile );
    return size * nmemb;
}
//--------------------------------------------------------------------------------------
size_t PEasyFTPClient::ReadCallback( char* Buffer, size_t Size, size_t nitems, void* UploadFile )
{
    size_t ReadBytes = fread( Buffer, Size, nitems, (FILE*)UploadFile );

    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " *** We read %d bytes from file ***", ReadBytes );
    return ReadBytes;
}
//--------------------------------------------------------------------------------------
size_t PEasyFTPClient::CheckExistCallback( void *ptr, size_t size, size_t nmemb, void *data )
{
    return (size_t)( size * nmemb );
}
//--------------------------------------------------------------------------------------
PEasyFTPClient::PEasyFTPClient( const AnsiString& Host, Int32 Port )
:FHost( Host )
,FPort( Port )
{
    curl_global_init( CURL_GLOBAL_DEFAULT );
    FCurl = curl_easy_init();
}
//--------------------------------------------------------------------------------------
PEasyFTPClient::PEasyFTPClient( const AnsiString& Host, Int32 Port, const AnsiString& User, const AnsiString& Password )
:FHost( Host )
,FPort( Port )
{
    SetNameAndPassword( User, Password );
}
//--------------------------------------------------------------------------------------
PEasyFTPClient::~PEasyFTPClient()
{
    curl_easy_cleanup( FCurl );
    curl_global_cleanup();
}
//--------------------------------------------------------------------------------------
void PEasyFTPClient::SetNameAndPassword( const AnsiString& Name, const AnsiString& Password )
{
    FUser = Name;
    FPassword = Password;
    curl_easy_setopt( FCurl, CURLOPT_USERPWD, FUser.c_str() );
    curl_easy_setopt( FCurl, CURLOPT_USERPWD, FPassword.c_str() );
}
//--------------------------------------------------------------------------------------
void PEasyFTPClient::Download( const AnsiString& RemoteFilename, const AnsiString& LocalFilename )
{
    if( FCurl != NULL )
    {
        AnsiString URL;
        FILE *fd;
        CURLcode Result;
        double Speed, Total;

        fd = fopen( LocalFilename.c_str(), "w" );
        if( !fd )
        {
            UFC::AnsiString Msg = "File Exception: Open file " + LocalFilename + " failed.";
            throw( FileException( (char*) Msg.c_str()));
        }

        URL.Printf( "ftp://%s:%d/%s", FHost.c_str(), FPort, RemoteFilename.c_str() );
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Download:[%s]", URL.c_str() );

        curl_easy_setopt( FCurl, CURLOPT_USERNAME, FUser.c_str() );
        curl_easy_setopt( FCurl, CURLOPT_PASSWORD, FPassword.c_str() );
        curl_easy_setopt( FCurl, CURLOPT_URL, URL.c_str() );
        curl_easy_setopt( FCurl, CURLOPT_WRITEFUNCTION, PEasyFTPClient::WriteCallback );
        curl_easy_setopt( FCurl, CURLOPT_WRITEDATA, fd );
        

        /* enable verbose for easier tracing */
        //if( UFC::BufferedLog::DebugLevelExist( UFC::dlInformation ) )
        //    curl_easy_setopt( FCurl, CURLOPT_VERBOSE, 1L );

        Result = curl_easy_perform( FCurl );
        if( Result != CURLE_OK )
            throw UFC::SocketException( curl_easy_strerror( Result ) );
            
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Download file:[%s] OK!", RemoteFilename.c_str() );
        fclose( fd );
        /* now extract transfer info */
        
        curl_easy_getinfo( FCurl, CURLINFO_SPEED_UPLOAD, &Speed );
        curl_easy_getinfo( FCurl, CURLINFO_TOTAL_TIME, &Total );

        UFC::BufferedLog::Printf( " Download [%s] completed. Speed: %.3f bytes/sec during %.3f seconds",
                RemoteFilename.c_str(), Speed, Total );
        
        curl_easy_reset( FCurl );
    }
}
//--------------------------------------------------------------------------------------
void PEasyFTPClient::Upload( const AnsiString& RemoteFilename, const AnsiString& LocalFilename )
{
    if( FCurl != NULL )
    {
        AnsiString URL;
        FILE *fd;
        struct stat FileInfo;
        CURLcode Result;
        double Speed, Total;

        fd = fopen( LocalFilename.c_str(), "r"); /* open file to upload */
        if( !fd )
        {
            UFC::AnsiString Msg = "File Exception: Open file " + LocalFilename + " failed.";
            throw( FileException( (char*) Msg.c_str()));
        }

        /* to get the file size */
        if(fstat(fileno(fd), &FileInfo) != 0)
        {
            UFC::AnsiString Msg = "File Exception: Get file info: " + LocalFilename + " failed.";
            throw( FileException( (char*) Msg.c_str()));
        }

        /* upload to this place */
        URL.Printf( "ftp://%s:%d/%s", FHost.c_str(), FPort, RemoteFilename.c_str() );
        curl_easy_setopt( FCurl, CURLOPT_URL, URL.c_str() );
        curl_easy_setopt( FCurl, CURLOPT_USERNAME, FUser.c_str() );
        curl_easy_setopt( FCurl, CURLOPT_PASSWORD, FPassword.c_str() );
        /* tell it to "upload" to the URL */
        curl_easy_setopt( FCurl, CURLOPT_UPLOAD, 1L);

        /* set where to read from (on Windows you need to use READFUNCTION too) */
        curl_easy_setopt( FCurl, CURLOPT_READDATA, fd );

        /* and give the size of the upload (optional) */
        curl_easy_setopt( FCurl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)FileInfo.st_size);

        /* enable verbose for easier tracing */
        //if( UFC::BufferedLog::DebugLevelExist( UFC::dlInformation ) )
        //    curl_easy_setopt( FCurl, CURLOPT_VERBOSE, 1L);

        Result = curl_easy_perform( FCurl );
        if( Result != CURLE_OK )
            throw UFC::SocketException( curl_easy_strerror( Result ) );

        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Upload file:[%s] OK!", RemoteFilename.c_str() );
        
        /* now extract transfer info */
        curl_easy_getinfo( FCurl, CURLINFO_SPEED_UPLOAD, &Speed );
        curl_easy_getinfo( FCurl, CURLINFO_TOTAL_TIME, &Total );

        UFC::BufferedLog::Printf( " Upload [%s] completed. Speed: %.3f bytes/sec during %.3f seconds",
                LocalFilename.c_str(), Speed, Total );
        fclose( fd );
        curl_easy_reset( FCurl );
    }
}
//--------------------------------------------------------------------------------------
BOOL PEasyFTPClient::IsFileExist( const AnsiString& File )
{
    if( FCurl != NULL )
    {
        AnsiString URL;
        CURLcode Result;

        URL.Printf( "ftp://%s:%d/%s", FHost.c_str(), FPort, File.c_str() );
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Check:[%s]", URL.c_str() );
        curl_easy_setopt( FCurl, CURLOPT_URL, URL.c_str() );
        curl_easy_setopt( FCurl, CURLOPT_USERNAME, FUser.c_str() );
        curl_easy_setopt( FCurl, CURLOPT_PASSWORD, FPassword.c_str() );
        curl_easy_setopt( FCurl, CURLOPT_NOBODY, 1L );
        /* Ask for filetime */
        curl_easy_setopt( FCurl, CURLOPT_FILETIME, 1L );
        curl_easy_setopt( FCurl, CURLOPT_HEADERFUNCTION, PEasyFTPClient::CheckExistCallback );
        curl_easy_setopt( FCurl, CURLOPT_WRITEFUNCTION, NULL );
        curl_easy_setopt( FCurl, CURLOPT_HEADER, 0L );

        /* enable verbose for easier tracing */
        //if( UFC::BufferedLog::DebugLevelExist( UFC::dlInformation ) )
        //    curl_easy_setopt( FCurl, CURLOPT_VERBOSE, 1L );
        
        Result = curl_easy_perform( FCurl );
        if( Result == CURLE_OK )
        {
            if( UFC::BufferedLog::DebugLevelExist( UFC::dlInformation ) )
            {
                time_t filetime;
                double filesize;

                Result = curl_easy_getinfo( FCurl, CURLINFO_FILETIME, &filetime );
                if( ( CURLE_OK == Result ) && filetime )
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " filetime [%s]: %s", File.c_str(), ctime(&filetime) );

                Result = curl_easy_getinfo( FCurl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &filesize );
                if( ( CURLE_OK == Result ) && ( filesize > 0 ) )
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " filesize [%s]: %0.0f bytes", File.c_str(), filesize );
            }
            curl_easy_reset( FCurl );
            return TRUE;
        }
        else
        {
            if( UFC::BufferedLog::DebugLevelExist( UFC::dlInformation ) )
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " %s", curl_easy_strerror( Result ) );

        }
    }
    curl_easy_reset( FCurl );
    return FALSE;
}
//--------------------------------------------------------------------------------------
#endif
}
//--------------------------------------------------------------------------------------


