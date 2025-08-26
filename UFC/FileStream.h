//---------------------------------------------------------------------------
// This is a part of the UNIX Foundation Classes Library for UNIX platforms.
// Copyright (c) 2003 MobileDigital Development Inc.
// All rights reserved.
//
// Description: Manipulae file as a stream.
// Author: Simon Chang
//
// This source code is only intended as a supplement to the
// Object Class Library for UNIX platforms Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// UNIX Foundation Classes Library for UNIX platforms product.
//---------------------------------------------------------------------------
#ifndef __FILESTREAM_H
#define __FILESTREAM_H
//---------------------------------------------------------------------------
#include "Exception.h"
#include "AnsiString.h"
#include "StringBuffer.h"
#include "PThread.h"
#include "Stream.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
class FileChangedListrner
{
public: ///< Implement interface PStream.
    virtual ~FileChangedListrner() {}
    virtual void OnFileChanged( const UFC::AnsiString& FileName, void* data ) = 0;
};
//---------------------------------------------------------------------------
class FileChangedWatchDog : public UFC::PThread
{
private:    
    FileChangedListrner* FListener;
    UFC::AnsiString      FFileName;
    time_t               FModifyTime;        
    int                  FTimeOutSec;
    void*                FUDD;
private:    
    virtual void Execute( void );
    BOOL CheckLoop( Int64 BeginSize );    
public:    
    FileChangedWatchDog( const UFC::AnsiString& FileName, FileChangedListrner* Listener, void* Data = NULL, int Timeout = 30 );    
    virtual ~FileChangedWatchDog(){}
};    
//---------------------------------------------------------------------------
class FileStream : public PStream
{
private:
	int FFD;
	UFC::AnsiString FFileName;
public: ///< Implement interface PStream.
	virtual int Read( void* Buffer, int Size );
	virtual int Write( const void* Buffer, int Size );
	virtual Int64 Seek( Int64 Offset, int Origin );
	virtual const Int64 GetSize( void );
public:
	FileStream( UFC::AnsiString FileName,int Mode, unsigned long Permissions = 0664 ); ///< Default permissions Group:rw Owner:rw
    virtual ~FileStream();
public:
    int GetHandle(){return FFD; }
	BOOL ReadyToRead( int TimeoutSec );
	BOOL ReadyToWrite( int TimeoutSec );
	UFC::AnsiString ReadLine( void );
	UFC::AnsiString GetFileName(void) { return FFileName; }
	void Flush();
};
//---------------------------------------------------------------------------
class FileStreamEx : public PStream
{
private:
	FILE*           FHandle;
	UFC::AnsiString FFileName;
    BOOL            FFlushAfterWrite;
    char*           FBuffer;  
public: ///< Implement interface PStream.
	virtual int Read( void* Buffer, int Size );
	virtual int Write( const void* Buffer, int Size );
	virtual Int64 Seek( Int64 Offset, int Origin );
	virtual const Int64 GetSize( void );
public:
	FileStreamEx( UFC::AnsiString FileName,const char* Mode, BOOL FlushAfterWrite = FALSE , Int32 StreamBufferSize = 0 );
        virtual ~FileStreamEx();
public:
	BOOL            ReadLine( UFC::AnsiString& Line );
	UFC::AnsiString GetFileName(void) { return FFileName; }
	int             Flush()           { return fflush( FHandle ); }
};
//---------------------------------------------------------------------------
class FileStream64 : public PStream
{
private:
    FILE*           FHandle;
    UFC::AnsiString FFileName;
    BOOL            FFlushAfterWrite;
    char*           FBuffer;  
public: ///< Implement interface PStream.
    virtual int   Read( void* Buffer, int Size );
    virtual int   Write( const void* Buffer, int Size );
    virtual Int64 Seek( Int64 Offset, int Origin );
    virtual const Int64 GetSize( void );
public:
    FileStream64( UFC::AnsiString FileName,const char* Mode, BOOL FlushAfterWrite = FALSE , Int32 StreamBufferSize = 0 );
    virtual ~FileStream64();
public:
    UFC::AnsiString GetFileName(void)  { return FFileName; }
    int             Flush()            { return fflush( FHandle ); }
};
//---------------------------------------------------------------------------
extern UFC::AnsiString ExtractFileName(  const UFC::AnsiString& PathName );
extern UFC::AnsiString ExtractFilePath(  const UFC::AnsiString& PathName );
extern BOOL FileExists( const UFC::AnsiString& FileName );
extern BOOL RemoveFile( const UFC::AnsiString& FileName );
extern BOOL RenameFile( const UFC::AnsiString& OldFileName, const UFC::AnsiString& NewFileName );
extern BOOL CreateDir( const UFC::AnsiString& DirName );
extern bool IsTodayFile( const UFC::AnsiString& FilePathName );
extern bool GetFileModificationTime( const UFC::AnsiString& FilePathName,
                                     int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second );
extern bool GetFileModificationSecond( const UFC::AnsiString& FilePathName, long long& ModificationSecond );
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
