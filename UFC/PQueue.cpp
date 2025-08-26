#include "PQueue.h"
#include "List.h"
#include <stdio.h>
#include <string.h>
#include "NetUtility.h"
#include "BufferedLog.h"
#ifndef _WIN32
    #include <sys/msg.h>
    #include <errno.h>
#else
    #include "TFileMapping.h"
#endif
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
UQueueData::UQueueData( long mType, int Size)
:FSize( Size )	
{
    FArray = new UInt8[ FSize + sizeof( long ) ];
    FData  = (QueueData*)(void*)FArray;
    FData->mtype = mType;
}
//---------------------------------------------------------------------------
UQueueData::UQueueData( long mType, char* Str )
:FSize( (int)strlen( Str ) + 1 )
{
    FArray = new UInt8[ FSize + sizeof( long ) ];
    FData  = (QueueData*)(void*)FArray;
    FData->mtype = mType;
    strcpy( FData->Data, Str );
}
//---------------------------------------------------------------------------
UQueueData::UQueueData( long mType, char* Data, int Size )
:FSize( Size )
{
    FArray = new UInt8[ FSize + sizeof( long ) ];
    FData  = (QueueData*)(void*)FArray;
    FData->mtype = mType;
    memcpy( FData->Data, Data, FSize );
}
//---------------------------------------------------------------------------
UQueueData::UQueueData( long mType, char* Data, int DataSize, int Size )
:FSize( Size )
{
    FArray = new UInt8[ FSize + sizeof( long ) ];
    FData  = (QueueData*)(void*)FArray;
    FData->mtype = mType;
    if( DataSize < FSize )
        memcpy( FData->Data, Data, DataSize );
    else
        memcpy( FData->Data, Data, FSize );
}
//---------------------------------------------------------------------------
UQueueData::~UQueueData()
{
    delete [] FArray;
}
//---------------------------------------------------------------------------
#ifndef _WIN32
//---------------------------------------------------------------------------
PQueue::PQueue( BOOL SharedEvent )
:FQueueID( -1 )
,FInterrupt( FALSE ) 
,FShared( SharedEvent )
,FEnqueueEvent( NULL )
{
}
//---------------------------------------------------------------------------
PQueue::~PQueue(void)
{
    if( FEnqueueEvent != NULL )
        delete FEnqueueEvent;
}
//---------------------------------------------------------------------------
bool PQueue::Open( key_t QueueKey, bool CreateNew)
{
    FQueueID = -1;
    if( QueueKey > 0 ) 
    {
        FQueueID = msgget( QueueKey, 0666 );
        if( FQueueID == -1 ) 
        {
            if( CreateNew )
                FQueueID = msgget( QueueKey, IPC_CREAT|0666 );
            else
                throw( QueueException(errno));
        }
    }
    UFC::BufferedLog::Printf( " Queue [0x%08X] opened,ID:%d", QueueKey, FQueueID );
    if( FQueueID >= 0 )
    {
        UFC::AnsiString QName;
        QName.Printf( "Event_For_Queue_0x%08X", QueueKey );
        FEnqueueEvent = new UFC::PEvent( QName, FShared ); ///< shared event, cross process
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void PQueue::Clear()
{
    UQueueData Data( 0, 1024 );
    errno = 0;
    while( RecvNoWait( Data ) == TRUE)
    {
        Data.SetMType(0);        
    }
    if (errno != ENOMSG)
    {
       UFC::BufferedLog::Printf( " Queue ID:%d Clear() Error No:%d", FQueueID, errno );
    }
}
//---------------------------------------------------------------------------
void PQueue::Interrupt( void )
{
    FInterrupt = TRUE;
    FEnqueueEvent->SetEvent();
}
//---------------------------------------------------------------------------
bool PQueue::SetQueueBytes(int Value)
{
    if( Value > 0 && FQueueID > 0 ) 
    {
        struct msqid_ds QueueInfo;
        
        if( msgctl(FQueueID, IPC_STAT, &QueueInfo) == 0 ) 
        {
            QueueInfo.msg_qbytes = Value;
            if( msgctl(FQueueID, IPC_SET, &QueueInfo) == 0 )
                return true;
        }
        throw( QueueException(errno));
    }
    return false;
}
//---------------------------------------------------------------------------
void PQueue::Send( UQueueData& Data )
{
    int Result = msgsnd( FQueueID, (void*)Data.FData, Data.GetSize(), IPC_NOWAIT );
    FEnqueueEvent->SetEvent();
    if( Result < 0 )
        throw( QueueException(errno));
}
//---------------------------------------------------------------------------
void PQueue::Recv( UQueueData& Data )
{
    int Result = msgrcv( FQueueID, (void*)Data.FData, Data.FSize, Data.GetMType(), 0 );
    if( Result < 0 )
        throw( QueueException(errno));
    Data.FSize = Result;
}
//---------------------------------------------------------------------------
BOOL PQueue::RecvNoWait( UQueueData& Data )
{
    int Result = msgrcv( FQueueID, (void*)Data.FData, Data.FSize, Data.GetMType(), IPC_NOWAIT|MSG_NOERROR );
    if( Result < 0 )
        return FALSE;
    Data.FSize = Result;
    return TRUE;
}
//---------------------------------------------------------------------------
BOOL PQueue::Recv( UQueueData& Data, const int TimeoutSec )
{
    const Int32 WaitTimeMS  = TimeoutSec * 1000;
    const Int32 BeginTimeMS = UFC::GetTickCountMS();
    Int32 Result,TimeDiff,Now;
    
    do
    {
        if( (Result = msgrcv( FQueueID,(void*)Data.FData,Data.FSize,Data.GetMType(),IPC_NOWAIT|MSG_NOERROR)) < 0 )
        {   ///< Get message from queue failed.
            if( errno != ENOMSG )    ///< Get message failed. 
                throw( QueueException( errno ) );
            if( FInterrupt == TRUE ) ///< Interrupt by User
            {
                FInterrupt = FALSE;
                return FALSE;
            }
        }
        else 
        {   ///< OK!
            Data.FSize = Result;
            return TRUE;
        }        
        if( FEnqueueEvent->WaitFor( 1 ) == TRUE )
            FEnqueueEvent->ResetEvent();
        Now = (Int32)UFC::GetTickCountMS();
        TimeDiff = Now - BeginTimeMS;
        if( TimeDiff < 0 ) ///< Timer overflow or reset...
        {
            TimeDiff = WaitTimeMS;            
            UFC::BufferedLog::Printf( " *** PQueue.Recv Tick overflow! reset from[%d] to [%d] ***", BeginTimeMS, Now );            
        }        
    }
    while( TimeDiff < WaitTimeMS );            
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL PQueue::Recv( UQueueData& Data, UFC::PLongList& MTypes, const int TimeoutSec )
{
    const Int32 WaitTimeMS  = TimeoutSec * 1000;
    const Int32 BeginTimeMS = UFC::GetTickCountMS();
    Int32 Result,TimeDiff,Now;
    
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Begin Dequeue %d MTypes.", MTypes.ItemCount() );
    do
    {
        for( register int i = 0; i < MTypes.ItemCount(); i++ ) 
        {
            if( (Result = msgrcv( FQueueID,(void*)Data.FData, Data.FSize, MTypes[ i ], IPC_NOWAIT|MSG_NOERROR)) < 0 ) 
            {   ///< Get message from queue failed.
                if( errno != ENOMSG )     ///< Get message failed.
                    throw( QueueException( errno ) );
                if( FInterrupt == TRUE )  ///< Interrupt by User
                {
                    FInterrupt = FALSE;
                    return FALSE;
                }          
            }
            else 
            {
                Data.FSize = Result;
                Data.SetMType( MTypes[ i ] );
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Dequeue a [%06d]MType data OK.", MTypes[ i ] );
                return TRUE;
            }
        }        
        if( FEnqueueEvent->WaitFor( 1 ) == TRUE )
            FEnqueueEvent->ResetEvent();
        Now = (Int32)UFC::GetTickCountMS();
        TimeDiff = Now - BeginTimeMS;
        if( TimeDiff < 0 ) ///< Timer overflow or reset...
        {
            TimeDiff = WaitTimeMS;            
            UFC::BufferedLog::Printf( " *** PQueue.Recv Tick overflow! reset from[%d] to [%d] ***", BeginTimeMS, Now );            
        }        
    }
    while( TimeDiff < WaitTimeMS );
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Dequeue %d MTypes timeout [%d]sec.", MTypes.ItemCount(), TimeoutSec );
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL PQueue::Recv( UQueueData& Data, UFC::PLongList& MTypes )
{
    Int32 Result;
    
    for( register int i = 0; i < MTypes.ItemCount(); i++ ) 
    {
        if( (Result = msgrcv( FQueueID,(void*)Data.FData, Data.FSize, MTypes[ i ], IPC_NOWAIT|MSG_NOERROR)) < 0 ) 
        {   ///< Get message from queue failed.
            if( errno != ENOMSG )     ///< Get message failed.
                throw( QueueException( errno ) );            
        }
        else 
        {
            Data.FSize = Result;
            Data.SetMType( MTypes[ i ] );
            return TRUE;
        }
    }        
    return FALSE;
}
//---------------------------------------------------------------------------
UInt16 PQueue::Count( void )
{
    struct msqid_ds QueueInfo;
    int Result = msgctl( FQueueID, IPC_STAT, &QueueInfo );
    if( Result < 0 )
        return 0;
    return QueueInfo.msg_qnum;
}
//---------------------------------------------------------------------------
UInt32 PQueue::Size( void )
{
    struct msqid_ds QueueInfo;
    int Result = msgctl( FQueueID, IPC_STAT, &QueueInfo );
    if( Result < 0 )
        return 0;
    return QueueInfo.msg_qbytes;
}
//---------------------------------------------------------------------------
#else  ///< Windows implement.
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
PQueue::PQueue( BOOL )
:FQueueID( 0 )
,FMutexTimeout( 5000 )
,FMaximunMessageCount( 4096 )
,FHeader( NULL )
,FBody( NULL )
{
    FHeaderSize = FMaximunMessageCount * sizeof( QHeader );
    FBodySize   = FMaximunMessageCount * sizeof( QBody );
}
//---------------------------------------------------------------------------
PQueue::~PQueue(void)
{
}
//---------------------------------------------------------------------------
bool PQueue::Open( long QueueKey, bool /*CreateNew*/ )
{
    UFC::AnsiString Key( (int)QueueKey );
    
    if( FMutex.Open( Key, true ) ) 
    {
        void* Address = FFileMapping.Open( Key, FHeaderSize + FBodySize, false );
        bool NeedToInitial = false;
        if( !Address ) 
        {
            Address = FFileMapping.Open( Key, FHeaderSize + FBodySize, true );
            NeedToInitial = true;
        }
        if( Address ) 
        {
            FQueueID = QueueKey;
            FHeader  = (QHeader*)Address;
            FBody    = (QBody*)( FHeader + FMaximunMessageCount );
            if( NeedToInitial ) 
            {
                for( register long i = 0; i < FMaximunMessageCount; i++ ) 
                {
                    QHeader* pHeader = FHeader + i;
                    pHeader->type    = 0;
                    pHeader->offset  = i;
                    pHeader->size    = -1;
                }
                memset( FBody, 0, FBodySize );
            }
            FFileMapping.Flush( Address, 0 );
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
void PQueue::Clear()
{
    UQueueData Data( 0, 1024 );
    while( RecvNoWait( Data ) == TRUE )
        Data.SetMType(0);        
}
//---------------------------------------------------------------------------
bool PQueue::SetQueueBytes(int /*Value*/)
{
    return false;
}
//---------------------------------------------------------------------------
void PQueue::Send( UQueueData& Data )
{
    if( !FQueueID || !FHeader || !FBody )
        return;
    
    QHeader* pHeader = NULL;
    try 
    {
        FMutex.WaitFor( FMutexTimeout );
        for( register int i = 0; i < FMaximunMessageCount; i++ ) 
        {
            if( FHeader[i].size < 0 ) 
            {
                pHeader = FHeader + i;
                break;
            }
        }
        
        if( pHeader ) 
        {
            pHeader->type = Data.GetMType();
            pHeader->size = Data.GetSize();
            QBody* pBody  = FBody + pHeader->offset;
            memcpy( pBody, Data.GetData(), pHeader->size );
            
            FFileMapping.Flush( (void*)pHeader, sizeof(QHeader) );
            FFileMapping.Flush( (void*)pBody, sizeof(QBody) );
        }
    }
    catch( UFC::Exception& ex ) 
    {
        UFC::BufferedLog::Printf( " PQueue::Send execption:%s", ex.what() );
    }
    catch( ... ) 
    {
        UFC::BufferedLog::Printf( " Unknown execption in PQueue::Send." );
    }
    FMutex.Release();
}
//---------------------------------------------------------------------------
void PQueue::Recv( UQueueData& Data )
{
    while( RecvNoWait( Data )== FALSE ) 
    {
        UFC::SleepMS( 10 );
    }
}
//---------------------------------------------------------------------------
void PQueue::CheckMessageType( long dataType, long& maxType, long& minType )
{
    if( dataType > 0 ) 
    {
        maxType = dataType;
        minType = dataType;
    }
    else if( dataType < 0 ) 
    {
        maxType = -dataType;
        minType = 0;
    }
    else 
    {
        maxType = 0x7FFFFFFF;
        minType = 0;
    }
}
//---------------------------------------------------------------------------
BOOL PQueue::CopyQueueData( UQueueData& Data )
{
    long dataType = Data.GetMType();
    long maxType, minType;
    QHeader* pHeader;
    QBody*   pBody;
    
    CheckMessageType( dataType, maxType, minType );
    for( register int i = 0; i < FMaximunMessageCount; i++ ) 
    {
        pHeader = FHeader + i;
        if( pHeader->size >= 0 ) 
        {
            dataType = pHeader->type;
            if( dataType >= minType && dataType <= maxType ) 
            {
                long offset = pHeader->offset;
                pBody = FBody + offset;
                if( Data.FSize > pHeader->size )
                    Data.FSize = pHeader->size;
                memcpy( Data.GetData(), pBody, Data.FSize );
                Data.SetMType( dataType );
                
                memcpy( pHeader, pHeader + 1, (FMaximunMessageCount - (i + 1)) * sizeof(QHeader) );
                pHeader = FHeader + ( FMaximunMessageCount - 1 );
                pHeader->type   = 0;
                pHeader->size   = -1;
                pHeader->offset = offset;
                FFileMapping.Flush( (void*)pHeader, sizeof(QHeader) );
                FFileMapping.Flush( (void*)pBody, sizeof(QBody) );
                return TRUE;
            }
        }
    }
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL PQueue::RecvNoWait( UQueueData& Data )
{
    if( !FQueueID || !FHeader || !FBody )
        return FALSE;
    
    BOOL Result;
    
    try 
    {
        FMutex.WaitFor( FMutexTimeout );
        Result = CopyQueueData( Data );
    }
    catch( UFC::Exception& ex ) 
    {
        printf( "%s\n", ex.what() );
        Result = FALSE;
    }
    catch( ... ) 
    {
        printf( "unknown exception\n" );
        Result = FALSE;
    }
    FMutex.Release();
    return Result;
}
//---------------------------------------------------------------------------
BOOL PQueue::Recv( UQueueData& Data, int TimeoutSec )
{
    for( int i = 0; i < TimeoutSec * 60; i++ ) 
    {
        if( RecvNoWait( Data )== TRUE )
            return TRUE;
        if( FInterrupt == TRUE ) 
        {
            FInterrupt = FALSE;
            return FALSE;
        }
        UFC::SleepMS( 10 );
    }
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL PQueue::Recv( UQueueData& Data, UFC::PLongList& MTypes, int TimeoutSec )
{
    Int32 WaitTimeMS = TimeoutSec * 1000;
    Int32 BeginTime  = UFC::GetTickCountMS();
    
    do
    {
	for( int j = 0; j < MTypes.ItemCount(); j ++ )
        {
            Data.SetMType( MTypes[ j ] );
            if( RecvNoWait( Data )== TRUE )
            {
                return TRUE;
            }
            else if( FInterrupt == TRUE )
            {
		FInterrupt = FALSE;
		return FALSE;
            }
        }
        UFC::SleepMS( 5 );        
    }
    while( ((Int32)UFC::GetTickCountMS() - BeginTime ) < WaitTimeMS );
    return FALSE;
}
//---------------------------------------------------------------------------
UInt16 PQueue::Count()
{
    UInt16 DataCount = 0;
    
    for( register int i = 0; i < FMaximunMessageCount; i++ ) 
    {
        if( FHeader[i].size > 0 )
            DataCount ++;
    }
    return DataCount;
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

