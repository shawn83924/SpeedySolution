#ifndef __UFC_MEMORYSTREAM_H
#define __UFC_MEMORYSTREAM_H
//---------------------------------------------------------------------------
// This is a part of the UNIX Foundation Classes Library for UNIX platforms.
// Copyright (c) 2003 MobileDigital Development Inc.
// All rights reserved.
//
// Description: Manipulate memory as a stream.
// Author: Frank
//
// This source code is only intended as a supplement to the
// Object Class Library for UNIX platforms Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// UNIX Foundation Classes Library for UNIX platforms product.
//---------------------------------------------------------------------------
#include <string.h>
#ifdef _AIX433
    #include <strings.h>
#endif
#include "UFCType.h"
#include "Exception.h"
#include "Stream.h"
#include "AnsiString.h"
//------------------------------------------------------------------------------------------------------
namespace UFC
{
//-----------------------------------------------------------------------------------------------------------------
class FixSizeMemoryStream: public PStream
{
private:
    Int32   FSize;
    char*   FPtr;
    BOOL    FOwn;
public: ///< Implement interface PStream.
    virtual Int32 Read( void* Buffer, Int32 Size );
    virtual Int32 Write( const void* Buffer,  Int32 Size );
    virtual Int64 Seek( Int64 Offset, Int32 Origin );
    virtual const Int64	GetSize( void ) { return FSize; }    
public:
    Int32 WriteByte( UInt8 Data );
    Int32 WriteByte( UInt8 Data, UInt8 AppendChar );
    Int32 Write( const void* Buffer,  Int32 Size, UInt8 AppendChar );
    Int32 Write( const void* Buffer,  Int32 Size, UInt8 val, UInt8 AppendChar );
    Int32 Write( const void* Buffer,  Int32 Size, const void* Buffer1,  Int32 Size1, UInt8 AppendChar );
public:
    FixSizeMemoryStream( Int32 Size );
    FixSizeMemoryStream( void* Buffer, Int32 Size );
    FixSizeMemoryStream( FixSizeMemoryStream& Src );
    virtual ~FixSizeMemoryStream();
public:
    const void*	GetBuffer() const { return (void*)FPtr; }    
};    
//-----------------------------------------------------------------------------------------------------------------
class MemoryStream: public PStream
{
private:
    Int32   FAllocSize;
    Int32   FSize;
    char*   FPtr;
    void    GrowSize( Int32 NeedSize );
public: ///< Implement interface PStream.
    virtual Int32 Read( void* Buffer, Int32 Size );
    virtual Int32 Write( const void* Buffer,  Int32 Size );
    virtual Int64 Seek( Int64 Offset, Int32 Origin );
    virtual const Int64	GetSize( void ) { return FSize; }
public:
    MemoryStream( Int32 AllocSize = 32, Int32 Size = 0 );
    MemoryStream( MemoryStream& Src );
    virtual ~MemoryStream();
    Int32 CopyFrom( MemoryStream* Stream, Int32 Size );
    Int32 Write( Int32 Size );
public:
    void 	SetBuffer( void* Buffer, Int32 Size );
    const void*	GetBuffer() const { return (void*)FPtr; }
    void	SetSize( Int32 Size );
    void        Clear( void );
};
//-----------------------------------------------------------------------------------------------------------------
class MemoryReadStream: public PStream
{
private:
    Int32   FSize;
    char*   FPtr;    
public: ///< Implement interface PStream.
    virtual Int32 Read( void* Buffer, Int32 Size );
    virtual Int32 Write( const void* Buffer,  Int32 Size );
    virtual Int64 Seek( Int64 Offset, Int32 Origin );
    virtual const Int64	GetSize( void ) { return FSize; }
public:
    MemoryReadStream( void* Buffer, Int32 Size );
    MemoryReadStream( MemoryReadStream& Src );
    virtual ~MemoryReadStream() {}
    BOOL ReadLine( UFC::AnsiString& Line );
public:
    void SetBuffer( void* Buffer, Int32 Size );
    void SetSize( Int32 Size );
    const void*	GetBuffer() const { return (void*)FPtr; }
};
//-----------------------------------------------------------------------------------------------------------------
#ifndef _WIN32
class PShareMemoryStream: public PStream
{
private:
    Int32	FSize;
    char*       FPtr;
    int         FHandle;
public: ///< Implement interface PStream.
    virtual Int32 Read( void* Buffer, Int32 Size );
    virtual Int32 Write( const void* Buffer,  Int32 Size );
    virtual Int64 Seek( Int64 Offset, Int32 Origin );
    virtual const Int64	GetSize( void ) { return FSize; }
public:
    PShareMemoryStream( Int32 Key, Int32 Size = 64 );
    void Destroy();
    virtual ~PShareMemoryStream();
public:
    const void*	GetBuffer() const { return (void*)FPtr; }
};
#endif
//------------------------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------------------------------
#endif
