/* 
 * File:   PEasyFTPClient.h
 * Author: yuan
 *
 * Created on February 1, 2011, 2:51 PM
 */

#ifndef PEASYFTPCLIENT_H
#define	PEASYFTPCLIENT_H

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include "AnsiString.h"
#include "Exception.h"
#include "BufferedLog.h"
#include "FileStream.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace UFC
{
#ifndef _WIN32
//------------------------------------------------------------------------------
class PEasyFTPClient
{
private:
    static size_t WriteCallback( void* Buffer, size_t Size, size_t nmemb, void* StoreFile );
    static size_t ReadCallback( char* Buffer, size_t Size, size_t nitems, void* UploadFile );
    static size_t CheckExistCallback( void *ptr, size_t size, size_t nmemb, void *data );
private:
    AnsiString  FHost;
    Int32       FPort;
    AnsiString  FUser;
    AnsiString  FPassword;
    CURL*       FCurl;    
public:
    PEasyFTPClient( const AnsiString& Host, Int32 Port );
    PEasyFTPClient( const AnsiString& Host, Int32 Port, const AnsiString& User, const AnsiString& Password );
    ~PEasyFTPClient();
    void SetNameAndPassword( const AnsiString& Name, const AnsiString& Password );
    AnsiString& GetHost( void ) { return FHost; }
    Int32 GetPort( void ) { return FPort; }
    AnsiString& GetUser( void ) { return FUser; }
    AnsiString& GetPassword( void ) { return FPassword; }    
    void Download( const AnsiString& RemoteFilename, const AnsiString& LocalFilename );
    void Upload( const AnsiString& RemoteFilename, const AnsiString& LocalFilename );
    BOOL IsFileExist( const AnsiString& File );
};
#endif
//------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
#endif	/* PEASYFTPCLIENT_H */

