#include "MemoryStream.h"
#ifndef _WIN32
    #include <sys/shm.h>
#endif
//----------------------------------------------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------------------------------
FixSizeMemoryStream::FixSizeMemoryStream( Int32 Size  )
:PStream(),
 FSize( ((Size+7)>>3)<<3 ),
 FPtr( NULL ),
 FOwn( TRUE )
{
    if( (FPtr = (char*)malloc( FSize )) == NULL )
        throw AllocException( );
}
//------------------------------------------------------------------------------------------------------
FixSizeMemoryStream::FixSizeMemoryStream( void* Buffer, Int32 Size  )
:PStream(),
 FSize( Size ),
 FPtr( (char*) Buffer ),
 FOwn( FALSE )
{
}
//------------------------------------------------------------------------------------------------------
FixSizeMemoryStream::FixSizeMemoryStream( FixSizeMemoryStream& Src )
:PStream( Src ),
 FSize( Src.FSize ), 
 FPtr( NULL ),
 FOwn( Src.FOwn )
{
    if( FOwn == TRUE )
    {
        if( (FPtr = (char*)malloc( FSize )) == NULL )
            throw AllocException( );
        memcpy( (void*)FPtr, (void*)Src.FPtr, FSize );
    }
    else
        FPtr = Src.FPtr;
}
//------------------------------------------------------------------------------------------------------
FixSizeMemoryStream::~FixSizeMemoryStream()
{
    if( FOwn == TRUE && FPtr != NULL )
    {
        free( FPtr );
        FPtr = NULL;
    }
}
//------------------------------------------------------------------------------------------------------
Int32 FixSizeMemoryStream::Read( void* Buffer, Int32 Size )
{
    if( FPos + Size > FSize )
        Size = FSize - FPos;
    if( Size > 0 )
    {
        memcpy( Buffer, (void*)(FPtr+(size_t)FPos), Size );
        FPos += Size;
    }
    return Size;
}
//------------------------------------------------------------------------------------------------------
Int32 FixSizeMemoryStream::Write( const void* Buffer, Int32 Size )
{
    int WriteSize = Size;
    
    if( FPos + Size > FSize  )
        WriteSize =  FSize - FPos;
    if( WriteSize > 0 )
    {        
  	    memcpy( (void*)(FPtr+(size_t)FPos), Buffer, WriteSize );
        FPos += WriteSize;
        return WriteSize;
    }
    else
        return 0;
}
//------------------------------------------------------------------------------------------------------
Int32 FixSizeMemoryStream::Write( const void* Buffer, Int32 Size, UInt8 AppendChar )
{
    int WriteSize = Size;
    
    if( FPos + Size  > FSize -1  )
        WriteSize =  FSize -1 - FPos;
    if( WriteSize <= 0 )
        return 0;
    memcpy( (void*)(FPtr+(size_t)FPos), Buffer, WriteSize );
    *(FPtr + (size_t)FPos + WriteSize ) = AppendChar;
    FPos += WriteSize + 1;
    return WriteSize + 1;
}
//------------------------------------------------------------------------------------------------------
Int32 FixSizeMemoryStream::Write( const void* Buffer,  Int32 Size, UInt8 val, UInt8 AppendChar )
{
    int WriteSize = Size + 1;
    
    if( FPos + WriteSize  > FSize - 1  )        
        return 0;
    memcpy( (void*)(FPtr+(size_t)FPos), Buffer, Size );
    *(FPtr + (size_t)FPos + Size) = val;
    *(FPtr + (size_t)FPos + WriteSize ) = AppendChar;
    FPos += (WriteSize + 1);
    return WriteSize + 1;    
}
//------------------------------------------------------------------------------------------------------
Int32 FixSizeMemoryStream::Write( const void* Buffer, Int32 Size, const void* Buffer1, Int32 Size1,UInt8 AppendChar )
{
    int WriteSize = Size + Size1;
    
    if( FPos + WriteSize  > FSize - 1  )        
        return 0;
    memcpy( (void*)(FPtr+(size_t)FPos), Buffer, Size );
    memcpy( (void*)(FPtr+(size_t)FPos+ Size), Buffer1, Size1 );
    *(FPtr + (size_t)FPos + WriteSize ) = AppendChar;
    FPos += (WriteSize + 1);
    return WriteSize + 1;
}
//------------------------------------------------------------------------------------------------------
Int64 FixSizeMemoryStream::Seek( Int64 Offset, Int32 Origin )
{
    switch( Origin )
    {
        case soFromBeginning:	FPos = Offset;
                                break;
        case soFromCurrent:  	FPos = FPos + Offset;
                                break;
        case soFromEnd:         if( Offset <= 0 )
                                    FPos = FSize + Offset;
                                break;
    }
    if( FPos > FSize )
        FPos = FSize;
    else if( FPos < 0 )
        FPos = 0;
    return FPos;
}
//------------------------------------------------------------------------------------------------------    
Int32 FixSizeMemoryStream::WriteByte( UInt8 Data )
{
    if( FPos + 1 > FSize  )
        return 0;
    
    *(FPtr+(size_t)FPos) = Data;
    FPos += 1;
    return 1;    
}
//------------------------------------------------------------------------------------------------------    
Int32 FixSizeMemoryStream::WriteByte( UInt8 Data, UInt8 AppendChar )
{
    if( FPos + 2 > FSize  )
        return 0;
    
    *(FPtr+(size_t)FPos)   = Data;
    *(FPtr+(size_t)FPos+1) = AppendChar;
    FPos += 2;
    return 2;        
}
//------------------------------------------------------------------------------------------------------    
//    
//    
//------------------------------------------------------------------------------------------------------
MemoryStream::MemoryStream( Int32 AllocSize , Int32 Size )
:PStream(),
 FAllocSize( ((AllocSize+7)>>3)<<3 ),
 FSize( Size ),
 FPtr( NULL )
{
    if( (FPtr = (char*)malloc( FAllocSize )) == NULL )
        throw AllocException( );
}
//------------------------------------------------------------------------------------------------------
MemoryStream::MemoryStream( MemoryStream& Src )
:PStream( Src ),
 FAllocSize( ((Src.FSize+7)>>3)<<3 ),
 FSize( Src.FSize ),
 FPtr( NULL )
{
    if( (FPtr = (char*)malloc( FAllocSize )) == NULL )
        throw AllocException( );
    memcpy( (void*)FPtr, (void*)Src.FPtr, FSize );
}
//------------------------------------------------------------------------------------------------------
MemoryStream::~MemoryStream()
{
    if( FPtr != NULL )
    {
        free( FPtr );
        FPtr = NULL;
    }
}
//------------------------------------------------------------------------------------------------------
Int32 MemoryStream::Read( void* Buffer, Int32 Size )
{
    if( FPos + Size > FSize )
        Size = FSize - FPos;
    if( Size > 0 )
    {
        memcpy( Buffer, (void*)(FPtr+(size_t)FPos), Size );
        FPos += Size;
    }
    return Size;
}
//------------------------------------------------------------------------------------------------------
void MemoryStream::GrowSize( Int32 NeedSize )
{
    Int32 NewSize = (( NeedSize + 7 )>>3)<<3;
    char* TempData;

    if( NeedSize > 1024 )
        FAllocSize = NewSize * 2;
    else if( NeedSize > 512 )
        FAllocSize = NewSize + 128;
    else
        FAllocSize = NewSize + 16;
    
    if( ( TempData = (char*) malloc( FAllocSize )) == NULL )
        throw AllocException( );
    if( FPtr != NULL )
    {
        memcpy( TempData, FPtr, FSize );
        free( FPtr );
    }
    FPtr = TempData;
}
//------------------------------------------------------------------------------------------------------
Int32 MemoryStream::CopyFrom( MemoryStream* MemStream, Int32 Size )
{
    Int32 CopySize;

    if( MemStream->FPos + Size > MemStream->FSize )
    {
        CopySize = MemStream->FSize - MemStream->FPos;
    }
    else
    {
        CopySize = Size;
    }
    return Write( MemStream->FPtr + (size_t)MemStream->FPos, CopySize );
}
//------------------------------------------------------------------------------------------------------
Int32 MemoryStream::Write( const void* Buffer, Int32 Size )
{
    if( FPos + Size > FAllocSize )
        GrowSize( FPos + Size );
    if( FPos + Size > FSize  )
    {
        FSize = FPos + Size;
    }
    memcpy( (void*)(FPtr+(size_t)FPos), Buffer, Size );
    FPos += Size;
    return Size;
}
//------------------------------------------------------------------------------------------------------
Int32 MemoryStream::Write( Int32 Size )
{
    if( FPos + Size > FAllocSize )
        GrowSize( FPos + Size );
    if( FPos + Size > FSize  )
        FSize = FPos + Size;
    FPos += Size;
    return Size;
}
//------------------------------------------------------------------------------------------------------
Int64 MemoryStream::Seek( Int64 Offset, Int32 Origin )
{
    switch( Origin )
    {
        case soFromBeginning:	FPos = Offset;
                                break;
        case soFromCurrent:  	FPos = FPos + Offset;
                                break;
        case soFromEnd:         if( Offset <= 0 )
                                    FPos = FSize + Offset;
                                break;
    }
    if( FPos > FSize )
        FPos = FSize;
    else if( FPos < 0 )
        FPos = 0;
    return FPos;
}
//------------------------------------------------------------------------------------------------------
void MemoryStream::SetBuffer( void* Buffer, Int32 Size)
{
    if( FPtr != NULL )
        free( FPtr );
    FAllocSize = Size;
    FSize = Size;
    FPtr = (char*)Buffer;
    FPos = 0;
}
//------------------------------------------------------------------------------------------------------
void MemoryStream::SetSize( Int32 Size )
{
    if( Size > FAllocSize )
        GrowSize( Size );
    FSize = Size;
}
//------------------------------------------------------------------------------------------------------
void MemoryStream::Clear( void )
{
    FPos  = 0;
    FSize = 0;
}
//------------------------------------------------------------------------------------------------------
//
// class MemoryReadStream
//
//------------------------------------------------------------------------------------------------------    
MemoryReadStream::MemoryReadStream( void* Buffer, Int32 Size )
:PStream(),
 FSize( Size ),
 FPtr( (char*)Buffer )
{
}
//------------------------------------------------------------------------------------------------------    
MemoryReadStream::MemoryReadStream( MemoryReadStream& Src )
:PStream(),
 FSize( Src.FSize ),
 FPtr( Src.FPtr )
{
}
//------------------------------------------------------------------------------------------------------
void MemoryReadStream::SetBuffer( void* Buffer, Int32 NewSize )
{
    if( Buffer != NULL )
    {
        FPtr  = (char*)Buffer;
        SetSize( NewSize );            
    }
}
//------------------------------------------------------------------------------------------------------
void MemoryReadStream::SetSize( Int32 NewSize )
{
    FSize = NewSize;
    if( FPos > FSize )
        FPos = FSize;        
}
//------------------------------------------------------------------------------------------------------
Int32 MemoryReadStream::Read( void* Buffer, Int32 Size )
{
    if( FPos + Size > FSize )
        Size = FSize - FPos;
    if( Size > 0 )
    {
        memcpy( Buffer,(void*)(FPtr+(size_t)FPos), Size );
        FPos += Size;
    }
    return Size;
}
//------------------------------------------------------------------------------------------------------
Int32 MemoryReadStream::Write( const void* /*Buffer*/, Int32 /*Size*/ )
{
    throw( UFC::Exception("MemoryReadStream is a read-only stream.") );
}    
//------------------------------------------------------------------------------------------------------
Int64 MemoryReadStream::Seek( Int64 Offset, Int32 Origin )
{
    switch( Origin )
    {
        case soFromBeginning:	FPos = Offset;
                                break;
        case soFromCurrent:	    FPos = FPos + Offset;
                                break;
        case soFromEnd:		    if( Offset <= 0 )
                                    FPos = FSize + Offset;
                                break;
    }
    if( FPos > FSize )
        FPos = FSize;
    else if( FPos < 0 )
        FPos = 0;
    return FPos;
}
//------------------------------------------------------------------------------------------------------
BOOL MemoryReadStream::ReadLine( UFC::AnsiString& Line )
{
    int BeginPos = FPos;

    for( register int i = BeginPos; i < FSize; i++ )
    {
        if( FPtr[ (size_t)FPos ] ==  0x0A || ///< LF
            FPtr[ (size_t)FPos ] ==  0x0D  ) ///< CR
        {
            if( FPos == BeginPos )///< Skip empty line
            {
                BeginPos++;
                FPos++;
            }
            else
            {
                Line = UFC::AnsiString( FPtr + BeginPos, FPos - BeginPos + 1 );
                return true;
            }
        }
        else
            FPos++;
    }
    return false;
}
//------------------------------------------------------------------------------------------------------
#ifndef _WIN32
//------------------------------------------------------------------------------------------------------
// class PShareMemoryStream
//------------------------------------------------------------------------------------------------------
PShareMemoryStream::PShareMemoryStream( Int32 Key, Int32 Size )
:PStream(),
 FSize( Size ),
 FPtr( NULL )
{
    if( ( FHandle = shmget( Key, FSize, IPC_CREAT|0666 )) == -1 )
        throw AllocException( );
    if( ( FPtr = (char*)shmat( FHandle, NULL, 0 )) == (char*)-1 )
    {
        FPtr = NULL;
        throw AllocException( );
    }
}
//------------------------------------------------------------------------------------------------------
PShareMemoryStream::~PShareMemoryStream()
{
    if( FPtr != NULL )
        shmdt( FPtr );
}
//------------------------------------------------------------------------------------------------------
void PShareMemoryStream::Destroy()
{
    struct shmid_ds* shmidds = NULL;
    if( FHandle != -1 )
    {
        if( FPtr != NULL )
            shmdt( FPtr );
        shmctl( FHandle, IPC_RMID, shmidds );
        FPtr    = NULL;
        FHandle = -1;
    }
}
//------------------------------------------------------------------------------------------------------
Int32 PShareMemoryStream::Read( void* Buffer, Int32 Size )
{
    if( FPtr == NULL )
        return 0;
    if( FPos + Size > FSize )
        Size = FSize - FPos;
    if( Size > 0 )
    {
        memcpy( Buffer, FPtr + FPos, Size );
        FPos += Size;
    }
    return Size;
}
//------------------------------------------------------------------------------------------------------
Int32 PShareMemoryStream::Write( const void* Buffer,  Int32 Size )
{
    if( FPtr == NULL )
        return 0;
    if( FPos + Size > FSize )
        Size = FSize - FPos;
    memcpy( FPtr + FPos, Buffer, Size );
    FPos += Size;
    return Size;
}
//------------------------------------------------------------------------------------------------------
Int64 PShareMemoryStream::Seek( Int64 Offset, Int32 Origin )
{
    switch( Origin )
    {
        case soFromBeginning:	FPos = Offset;
                                break;
        case soFromCurrent:	FPos = FPos + Offset;
                                break;
        case soFromEnd:		if( Offset <= 0 )
                                    FPos = FSize + Offset;
                                break;
    }
    if( FPos > FSize )
        FPos = FSize;
    else if( FPos < 0 )
        FPos = 0;
    return FPos;
}
//------------------------------------------------------------------------------------------------------
#endif
}
//----------------------------------------------------------------------------------------------------------------------
