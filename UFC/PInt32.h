//---------------------------------------------------------------------------
// This is a part of the UNIX Foundation Classes Library for Solaris(R) platforms.
// Copyright (c) 2002 MobileDigital Development Inc.
// All rights reserved.
//
// Description: Class PInt32 (Persistent Int 32.)
//              In UNIX, use share memory mechanism to keep the Int32.
//              In Win32, use File Mapping mechanism to keep the Int32.
//
// Author: Simon Chang
//
// This source code is only intended as a supplement to the
// Object Class Library for Solaris(R) platforms Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Object Class Library for Solaris(R) platforms product.
//---------------------------------------------------------------------------
#ifndef PINT_H
#define PINT_H
#include <errno.h>
#include <string.h>
#ifdef _AIX433
	#include <strings.h>
#endif
#include "UFCType.h"
#include "List.h"
#include "Exception.h"
//------------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
#ifdef _WIN32
//---------------------------------------------------------------------------
class PInt32
{
//field
private:
    HANDLE     FFileMap;
    HANDLE     FFile;
    Int32*     FInt32Ptr;
    AnsiString FFileName;

//constructor and distructor
public:
    PInt32( void );
    PInt32( const char* Name );
    PInt32( AnsiString& Name );
    PInt32( AnsiString& Name, Int32 InitVal );
    ~PInt32( void );

//method
private:
    bool OpenShareMemory( const UFC::AnsiString& ShareMemoryName );
    bool CreateShareMemory( const UFC::AnsiString& ShareMemoryName );
    bool MapIntegerAddress( void );

//method
public:
    void  Attach( AnsiString& Name );
    void  Attach( AnsiString& Name, Int32 InitVal );
    void  Attach( long ID );
    void  Attach( long ID, Int32 InitVal );
    void  Detach( void );
    void  Remove( void );
    Int32 ToInt32( void );

//operator
public:
    PInt32& operator = ( Int32 Val );
    PInt32& operator +=( Int32 Val );
    operator Int32 ( void ) { return ToInt32(); }
};
//------------------------------------------------------------------------------
template <class T>
class PArray
{
private:
    T* FData;  ///< Use Big-Endian as internal byte order.
    HANDLE FFileMap;
    HANDLE FFile;
    Int32  FCount;
public:
    PArray( void );
    PArray( const AnsiString& Name, Int32 Count );
    PArray( UInt32 ID, Int32 Count );    
    void Attach( const AnsiString& Name, Int32 Count );
    void Attach( UInt32 ID, Int32 Count );
    ~PArray( );
    T& operator []( int idx ) throw( IndexOutOfBoundException  );
    T* Buffer() { return FData; }
    int GetUpperBound( void ) { return (FCount - 1); }
};
//------------------------------------------------------------------------------
template <class T>
PArray<T>::PArray( void )
:FData( NULL )
,FCount( 0 )
{
}
//------------------------------------------------------------------------------
template <class T>
PArray<T>::PArray( UInt32 ID, Int32 Count )
:FData( NULL )
{
	Attach( ID, Count );
}
//------------------------------------------------------------------------------
template <class T>
PArray<T>::PArray( const AnsiString& Name, Int32 Count )
:FData( NULL )
{
	Attach( Name, Count );
}
//---------------------------------------------------------------------------
template <class T>
void PArray<T>::Attach( const AnsiString& Name, Int32 Count )
{
	UFC::AnsiString FileName;

	FCount = Count;
	FileName.Printf( "Persistent\\%s",Name.c_str());
		CreateDirectoryA( "Persistent",NULL );
		if((FFile = CreateFileA( FileName.c_str(),                  // name of existing file
                                               GENERIC_READ | GENERIC_WRITE,      // read/write access
                                               0,                                 // no sharing
						NULL,                              // default security
						OPEN_ALWAYS,                       // open existing or new
						FILE_ATTRIBUTE_NORMAL,             // file attributes
						NULL))== INVALID_HANDLE_VALUE)     // no template
		throw( ShareMenException( -1 ));
	if(( FFileMap = CreateFileMapping( FFile, NULL, PAGE_READWRITE, 0, FCount* sizeof(T), Name.c_str() ))== INVALID_HANDLE_VALUE)
		throw( ShareMenException( -2 ));
	if((FData = (T*)MapViewOfFile( FFileMap, FILE_MAP_ALL_ACCESS, 0, 0, FCount* sizeof(T) )) == NULL )
		throw( ShareMenException( -3 ));
}
//---------------------------------------------------------------------------
template <class T>
void PArray<T>::Attach( UInt32 ID, Int32 Count )
{
	UFC::AnsiString FileName,Name;

	FCount = Count;
	FileName.Printf( "Persistent\\ID_%d",ID );
	Name.Printf( "ID_%d",ID );
		CreateDirectoryA( "Persistent",NULL );
		if((FFile = CreateFileA( FileName.c_str(),                  // name of existing file
                                               GENERIC_READ | GENERIC_WRITE,      // read/write access
                                               0,                                 // no sharing
						NULL,                              // default security
						OPEN_ALWAYS,                       // open existing or new
						FILE_ATTRIBUTE_NORMAL,             // file attributes
						NULL))== INVALID_HANDLE_VALUE)     // no template
		throw( ShareMenException( -1 ));
	if(( FFileMap = CreateFileMapping( FFile, NULL, PAGE_READWRITE, 0, FCount* sizeof(T), Name.c_str() ))== INVALID_HANDLE_VALUE)
		throw( ShareMenException( -2 ));
	if((FData = (T*)MapViewOfFile( FFileMap, FILE_MAP_ALL_ACCESS, 0, 0, FCount* sizeof(T) )) == NULL )
		throw( ShareMenException( -3 ));
}
//---------------------------------------------------------------------------
template <class T>
PArray<T>::~PArray( )
{
    if( FData != NULL )
    {
        FlushViewOfFile( FData, FCount* sizeof(T) );
        UnmapViewOfFile( (void*)FData );
    }
    CloseHandle( FFileMap );
    CloseHandle( FFile );
}
//---------------------------------------------------------------------------
template <class T>
T& PArray<T>::operator []( int index ) throw( IndexOutOfBoundException )
{
    if( index < 0 || index >= FCount )
        throw IndexOutOfBoundException( index );
    return *( FData + index );
}
//---------------------------------------------------------------------------
#else
//---------------------------------------------------------------------------
#include <sys/shm.h>
//---------------------------------------------------------------------------
UFC::AnsiString ShmNameToFileName( const UFC::AnsiString& ShmName );
extern key_t GetShareMemoryUniqueKey( const UFC::AnsiString& Name );
//---------------------------------------------------------------------------
class PInt32
{
//field
private:
    Int32  FShmID;
    Int32* FInt32Ptr;  ///< Use Big-Endian as internal byte order.
    UFC::AnsiString FName;
//constructor and distructor
public:
    PInt32( void );
    PInt32( const char* Name );
    PInt32( AnsiString& Name );
    PInt32( AnsiString& Name, Int32 InitVal );
    ~PInt32( void );

//method
public:
    void  Attach( AnsiString& Name );
    void  Attach( AnsiString& Name, Int32 InitVal );
    void  Attach( long ID );
    void  Attach( long ID, Int32 InitVal );    
    void  Detach( void );
    void  Remove( void );
    void  Set( Int32 Val );
    Int32 ToInt32( void );    
    BOOL  IsAttached( void ) { return (FShmID!=-1 );}
    BOOL Exists( const AnsiString& Name );
//operator
public:
    PInt32& operator = ( Int32 Value );
    PInt32& operator +=( Int32 Val );
    operator Int32 ( void ) { return ToInt32(); }
};
//------------------------------------------------------------------------------
template <class T>
class PArray
{
private:
    Int32   FShmID;
    UInt8* FAttach;
    T*       FData;
    Int32   FCount;    
public:
    PArray( void );
    PArray( const AnsiString& Name, Int32 Count );
    PArray( UInt32 ID, Int32 Count );    
    void Attach( const AnsiString& Name, Int32 Count );
    void Attach( UInt32 ID, Int32 Count );
    ~PArray( );
    T& operator []( int idx );// throw( IndexOutOfBoundException  );
    T* Buffer() { return FData; }
    int GetUpperBound( void ) { return (FCount - 1); }
};
//------------------------------------------------------------------------------
template <class T>
PArray<T>::PArray( void )
:FData( NULL )
,FCount( 0 )
{
}
//------------------------------------------------------------------------------
template <class T>
PArray<T>::PArray( UInt32 ID, Int32 Count )
:FData( NULL )
{
	Attach( ID, Count );
}
//------------------------------------------------------------------------------
template <class T>
PArray<T>::PArray( const AnsiString& Name, Int32 Count )
:FData( NULL )
{
	Attach( Name, Count );
}
//------------------------------------------------------------------------------
template <class T>
PArray<T>::~PArray( void )
{
    if( FAttach != NULL )
        shmdt( (void*)FAttach );
}
//---------------------------------------------------------------------------
//   In Memory Layout
// 
//   SHMARRAY| Size| Count * sizeof Data Type
//       8              4          
//---------------------------------------------------------------------------
template <class T>
void PArray<T>::Attach( UInt32 ID, Int32 Count )
{
    const char* ShmName = "SHMARRAY";
    UInt32  SizeofName = strlen( ShmName );
    UInt32  SizeofType   = sizeof( T );
    UInt32  SizeofData   = Count * SizeofType;
    bool     NeedInit       = false;    
    struct shmid_ds s;
    
    s.shm_segsz = SizeofName + sizeof(UInt32) + SizeofData;
    FShmID = shmget(  ID,  s.shm_segsz, 0666 );
    if( FShmID < 0 )
    {
        if( errno == ENOENT )
        {
            FShmID = shmget( ID, s.shm_segsz, IPC_CREAT | 0666 );
            NeedInit = true;
        }
        else
        {
            FShmID = shmget( ID, 1, 0666 );
            if( FShmID < 0 )
                throw( ShareMenException( errno ) );
        }
    }    
    if( (FShmID < 0) || (shmctl( FShmID, IPC_STAT, &s ) < 0) )
        throw( ShareMenException( errno ) );
    if( SizeofData > 0 )
    {
        FAttach = (UInt8*)(shmat( FShmID, NULL, 0 ) );
        if( NeedInit  == TRUE )
        {
            memset( FAttach, 0x00, s.shm_segsz );
            memcpy( FAttach, ShmName, SizeofName );
            memcpy( FAttach + SizeofName, &SizeofType, sizeof(UInt32) );
        }        
        if( memcmp( FAttach, ShmName, SizeofName ) != 0 )
        {
            UFC::AnsiString OrigName( (char*)FAttach, SizeofName  );
            UFC::AnsiString ErrMsg;
            
            ErrMsg.Printf( "shm PArray name not match Orig[%s] Now[%s]",OrigName.c_str(), ShmName );
             throw( ShareMenException( ErrMsg.c_str() ) );
        }
        if( memcmp( FAttach + SizeofName, &SizeofType, sizeof(UInt32) ) != 0 )
        {
            UFC::AnsiString ErrMsg;
            UInt32 OrigSize;
            
            memcpy( &OrigSize, FAttach + SizeofName, sizeof(UInt32) );
            ErrMsg.Printf( "shm PArray type size not match Orig[%d] Now[%d]",OrigSize, SizeofType );
            throw( ShareMenException( ErrMsg.c_str() ) );
        }
        FData = (T*)(FAttach + SizeofName + sizeof(UInt32));
        FCount = SizeofData / SizeofType;                   
    }
}
//---------------------------------------------------------------------------
template <class T>
void PArray<T>::Attach( const AnsiString& Name, Int32 Count )
{
	Attach( (long)GetShareMemoryUniqueKey( Name ), Count );
}
//---------------------------------------------------------------------------
template <class T>
T& PArray<T>::operator []( int index ) //throw( IndexOutOfBoundException )
{
    if( index < 0 || index >= FCount )
        throw IndexOutOfBoundException( index );
    return *( FData + index );
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
typedef PArray<UFCType::Int32>  PInt32Array;
typedef PArray<UFCType::Int8>   PInt8Array;
typedef PArray<char>            PCharArray;
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
