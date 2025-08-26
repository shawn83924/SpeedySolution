#ifndef TFileMappingH
#define TFileMappingH
//---------------------------------------------------------------------------
#include "Exception.h"
#include "AnsiString.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
class TMutex
{
private:
    HANDLE FHandle;
public:
    TMutex( void );
    virtual ~TMutex( void );
public:
    bool Open( const UFC::AnsiString& Key, bool CreateNew = false );
    void Close( void );
    void WaitFor( long Timeout = 5000 );
    void Release( void );
};
//---------------------------------------------------------------------------
class TFileMapping
{
private:
    HANDLE FHandle;
    void*  FAddress;
    long   FFileSize;
public:
    TFileMapping( void );
    virtual ~TFileMapping( void );
private:
    void* MapView( void );
public:
    void* Open( const UFC::AnsiString& Key, long Size, bool CreateNew = false );
    void  Flush(  void* Address, long Size );
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

