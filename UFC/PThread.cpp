#include "PThread.h"
#include "BufferedLog.h"
#include "iniFile.h"
#include <typeinfo>
#include <errno.h>
#ifdef __AIX
#include <unistd.h>
#endif
//---------------------------------------------------------------------------
namespace UFC
{
PCriticalSection PThread::GThreadCS;
//-------------------------------------------------------------------------------------------------------------------
#ifdef _WIN32
//-------------------------------------------------------------------------------------------------------------------
void SleepNS( UInt32 ns )
{
	::SleepEx( ns/1000000, TRUE );
}
//-------------------------------------------------------------------------------------------------------------------
/// Allen Add at 20190614
void SleepUS( unsigned int us )
{
	::SleepEx( us/1000, TRUE );
}
//-------------------------------------------------------------------------------------------------------------------
void SleepMS( unsigned int ms )
{
    ::SleepEx( ms, TRUE );
}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
PEvent::PEvent( void )
:FTriggered( FALSE )
{
    FEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
}
//-------------------------------------------------------------------------------------------------------------------
PEvent::~PEvent( void )
{
    CloseHandle( FEvent );
}
//-------------------------------------------------------------------------------------------------------------------
void PEvent::SetEvent( void )
{
    FTriggered = TRUE;
    ::SetEvent( FEvent );
}
//-------------------------------------------------------------------------------------------------------------------
void PEvent::ResetEvent( void )
{
    FTriggered = FALSE;
    ::ResetEvent( FEvent ); ///< Also clear the kernel event, or a stale signal survives into the next WaitFor.
}
//-------------------------------------------------------------------------------------------------------------------
BOOL PEvent::WaitFor( int Timeout,int ms )
{
    UInt32 TimeoutMilliseconds;
    
    if( FTriggered == TRUE )
    {
        FTriggered = FALSE;
        return TRUE;
    }
    if( Timeout == 0 )  ///< Wait infinite time...
        TimeoutMilliseconds = INFINITE;
    else
		TimeoutMilliseconds = Timeout*1000 + ms;
    if( WaitForSingleObject( FEvent, TimeoutMilliseconds ) == WAIT_OBJECT_0	)
	return TRUE;
    return FALSE;
}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
PCriticalSection::PCriticalSection( void )
{
	InitializeCriticalSection( &FMutex );
}
//-------------------------------------------------------------------------------------------------------------------
PCriticalSection::~PCriticalSection( void )
{
	DeleteCriticalSection( &FMutex );
}
//-------------------------------------------------------------------------------------------------------------------
void PCriticalSection::Enter( void )
{
	Acquire();
}
//-------------------------------------------------------------------------------------------------------------------
void PCriticalSection::Leave( void )
{
	Release();
}
//-------------------------------------------------------------------------------------------------------------------
void PCriticalSection::Acquire( void )
{
	EnterCriticalSection( &FMutex );
}
//-------------------------------------------------------------------------------------------------------------------
void PCriticalSection::Release( void )
{
	LeaveCriticalSection( &FMutex );
}
//-------------------------------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------------------------------
// class PCriticalSection
//-------------------------------------------------------------------------------------------------------------------
PCriticalSection::PCriticalSection( void )
:FthreadID( 0 ),
 FCount( 0 )
{
    if( pthread_mutex_init( &FMutex, NULL ) != 0 )
        throw( MutexException( errno ) );
}
//-------------------------------------------------------------------------------------------------------------------
PCriticalSection::~PCriticalSection( void )
{
    pthread_mutex_destroy( &FMutex );
}
//-------------------------------------------------------------------------------------------------------------------
void PCriticalSection::Enter( void )
{
    Acquire();
}
//-------------------------------------------------------------------------------------------------------------------
bool PCriticalSection::TryEnter( void )
{
    return TryAcquire();
}
//-------------------------------------------------------------------------------------------------------------------
void PCriticalSection::Leave( void )
{
    Release();
}
//-------------------------------------------------------------------------------------------------------------------
bool PCriticalSection::TryAcquire( void )
{
    if( FCount && FthreadID == pthread_self() )
    {
        FCount++;
        return true;
    }
    if( pthread_mutex_trylock( &FMutex ) == 0 )
    {
        FCount++;
        FthreadID = pthread_self();
        return true;
    }
    return false;
}
//-------------------------------------------------------------------------------------------------------------------
void PCriticalSection::Acquire( void )
{
    if( FCount && FthreadID == pthread_self() )
    {
        FCount++;
        return;
    }
    pthread_mutex_lock( &FMutex );
    FCount++;
    FthreadID = pthread_self();
}
//-------------------------------------------------------------------------------------------------------------------
void PCriticalSection::Release( void )
{
    if( FCount > 1 )
    {
        FCount--;
        return;
    }
    FCount--;
    FthreadID = 0;
    pthread_mutex_unlock( &FMutex );
}
//-------------------------------------------------------------------------------------------------------------------
void SleepMS( unsigned int ms )
{
#ifdef __LINUX
    int Sec = ms/1000;
    int us  = (ms%1000)*1000;
    for( int i = 0;  i < Sec; i ++ )
         usleep( 999*1000 );
    usleep( us );
#else   
    struct timeval tval;
    tval.tv_sec  = ms / 1000;
    tval.tv_usec = (ms % 1000) * 1000;
    select( 0, NULL, NULL, NULL, &tval );
#endif       
}
//-------------------------------------------------------------------------------------------------------------------
void SleepUS( unsigned int us )
{
    struct timeval tval;
    tval.tv_sec  = 0;
    tval.tv_usec = us;
    select( 0, NULL, NULL, NULL, &tval );
}
//-------------------------------------------------------------------------------------------------------------------
const UInt32 SPARC_DELAY_INACCURACY = 20000000; //< 20 ms
//-------------------------------------------------------------------------------------------------------------------
void SleepNS( UInt32 ns )
{
    struct timespec req;
#ifdef __sparc
    Int64 minimum_residual; // time spent in gethrtime()
    Int64 n;
    hrtime_t  start_time = gethrtime();
    hrtime_t  now        = gethrtime();
    
    minimum_residual = now - start_time;
    if( ns > SPARC_DELAY_INACCURACY)
    {
        req.tv_sec  = 0;
        req.tv_nsec = ns;
        while( nanosleep( &req, &req ) == -1)
        {
            if (errno != EINTR)
               break;
        }
    }
    n = ns;
    while( now - start_time < ( n - minimum_residual ) )
    {   // repeat calling gethrtime() to use up time until about right
        now = gethrtime();
    };
#else
    // ref:http://tldp.org/HOWTO/IO-Port-Programming-4.html
    // on Linux's nanosleep use the same algorithm as previous internal.
    req.tv_sec  = 0;
    req.tv_nsec = ns;
    while( nanosleep(&req, &req) == -1 )
    {
        if( errno != EINTR)
            return;
    }
#endif
}
//----------------------------------------------------------------------------------------------------------------------
UFC::PHashedList<UFC::AnsiString, EventInfo*>  PEvent::EventsTable;
UFC::PCriticalSection                          PEvent::EventsTableCS;
//----------------------------------------------------------------------------------------------------------------------
PrviateEventInfo::PrviateEventInfo( const AnsiString& Name )
:EventInfo( Name )  ///< Name of this event object
,FCount( 0 )        ///< Reference count = 0
,FTriggerCount( 0 ) ///< Trigger count = 0
{
    if( pthread_mutex_init( &FMutex, 0 ) != 0 )
        throw( MutexException( errno ));
    if( pthread_cond_init( &FEvent, 0 ) != 0 )
        throw( ConditionVariableException( errno ));
}
//-------------------------------------------------------------------------------------------------------------------
PrviateEventInfo::~PrviateEventInfo( void )
{
    pthread_cond_destroy( &FEvent );
    pthread_mutex_destroy( &FMutex );    
}
//-------------------------------------------------------------------------------------------------------------------     
void PrviateEventInfo::RefEvent( pthread_cond_t*& Event, pthread_mutex_t*& Mutex )
{
   UFC::PLockObject Lock( PEvent::EventsTableCS );

   Event = &FEvent;
   Mutex = &FMutex;
   FCount++;
}
//-------------------------------------------------------------------------------------------------------------------        
Int32 PrviateEventInfo::DeRef( void )
{
   UFC::PLockObject Lock( PEvent::EventsTableCS );

   FCount--;
   return FCount;
}
//---------------------------------------------------------------------------
void PrviateEventInfo::IncreaseTrigger( void )
{
   UFC::PLockObject Lock( PEvent::EventsTableCS );
  
   FTriggerCount ++; 
}    
//---------------------------------------------------------------------------
BOOL PrviateEventInfo::DecreaseTrigger( void )
{
   UFC::PLockObject Lock( PEvent::EventsTableCS );    
   
   if( FTriggerCount >= 1 )
   {
       FTriggerCount--;
       return TRUE;
   }
   FTriggerCount = 0;
   return FALSE;
}    
//---------------------------------------------------------------------------
Int32 PrviateEventInfo::TriggerCount( void )
{
   return FTriggerCount; 
} 
//---------------------------------------------------------------------------
extern key_t GetShareMemoryUniqueKey( const UFC::AnsiString& Name );
//----------------------------------------------------------------------------------------------------------------------
SharedEventInfo::SharedEventInfo( const AnsiString& Name )
:EventInfo( Name )
,FEventStructPtr( NULL )
{
    key_t Key = GetShareMemoryUniqueKey( Name );

    if( (FShmID = shmget( Key, sizeof( SharedEventStruct ), 0666)) != -1 ) ///< Try to open share memory.
    {   ///< Already exists
        shmid_ds ds;

        if( shmctl( FShmID, IPC_STAT, &ds ) != 0 )
            throw( ShareMenException( errno ));
        if( ds.shm_nattch == 0 ) ///< Exists but no process attach it.
        {
            AttachShareMemmory( );
            DestroyIPCS( );    ///< Delete old IPC objects.
            CreateIPCS( Key ); ///< Create new IPC objects.
        }
        else
            AttachShareMemmory();           
    }
    else ///< shm not exists.
        CreateIPCS( Key ); ///< Create new IPC objects.
}
//----------------------------------------------------------------------------------------------------------------------
SharedEventInfo::~SharedEventInfo( void )
{
    DestroyIPCS( );
}
//----------------------------------------------------------------------------------------------------------------------
void SharedEventInfo::AttachShareMemmory( void )
{
    if( (FEventStructPtr = (SharedEventStruct*)shmat( FShmID, NULL, 0)) == (SharedEventStruct*)-1 )
        throw( ShareMenException( errno ));
}
//----------------------------------------------------------------------------------------------------------------------
void SharedEventInfo::DestroyIPCS( void )
{
   if( FEventStructPtr != NULL )
    {
        shmid_ds ds;

        ///< Destroy condition variable
        //pthread_cond_destroy( &FEventStructPtr->HEvent );
        ///< Destroy mutex        
        pthread_mutex_destroy( &FEventStructPtr->HMutex );
        pthread_mutex_destroy( &FEventStructPtr->HCountMutex );      
       
        ///< Detach share memory.
        if( shmdt( (void*)FEventStructPtr )!= 0 )
            throw( ShareMenException( errno ) );
        ///< Remove share memory.
        if( shmctl( FShmID, IPC_STAT, &ds ) != 0 || shmctl( FShmID, IPC_RMID, &ds ) != 0 )
            throw( ShareMenException( errno ) );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SharedEventInfo::CreateIPCS( key_t Key )
{
    pthread_mutexattr_t MutexAttr;
    pthread_condattr_t  CondAttr;
    
    ///< Get share memory Handle.
    if( (FShmID = shmget( Key, sizeof( SharedEventStruct ), IPC_CREAT | 0666)) == -1 )
         throw( ShareMenException( errno ));
    ///< Attach share memory.
    AttachShareMemmory();
    ///< Create Mutex attribute object.    
    if( pthread_mutexattr_init( &MutexAttr ) != 0 )
        throw( MutexException( errno ));
    ///< Set Shared property to Mutex attribute object.
    if( pthread_mutexattr_setpshared( &MutexAttr, PTHREAD_PROCESS_SHARED ) != 0 )
        throw( MutexException( errno ));
    ///< Create Condition Variable attribute object.
    if( pthread_condattr_init( &CondAttr ) != 0 )
        throw( ConditionVariableException( errno ));
    ///< Set Shared property to Condition Variable attribute object.
    if( pthread_condattr_setpshared( &CondAttr, PTHREAD_PROCESS_SHARED ) != 0 )
        throw( ConditionVariableException( errno ));
    ///< Create Mutex
    if( pthread_mutex_init( &FEventStructPtr->HCountMutex, &MutexAttr )!= 0 )
        throw( MutexException( errno ));
    pthread_mutex_lock( &( FEventStructPtr->HCountMutex) );
        if( pthread_mutex_init( &FEventStructPtr->HMutex, &MutexAttr )!= 0 )
            throw( MutexException( errno ));
        if( pthread_cond_init( &FEventStructPtr->HEvent, &CondAttr )!= 0 )
            throw( ConditionVariableException( errno ));
        FEventStructPtr->RefCount = 0;
    pthread_mutex_unlock( &( FEventStructPtr->HCountMutex) );
    pthread_mutexattr_destroy( &MutexAttr );
    pthread_condattr_destroy( &CondAttr );
}
//----------------------------------------------------------------------------------------------------------------------
void SharedEventInfo::RefEvent( pthread_cond_t*& Event, pthread_mutex_t*& Mutex )
{
    pthread_mutex_lock( &( FEventStructPtr->HCountMutex) );

    Event = &( FEventStructPtr->HEvent );
    Mutex = &( FEventStructPtr->HMutex );
    FEventStructPtr->RefCount++;

    pthread_mutex_unlock( &( FEventStructPtr->HCountMutex) );
}
//----------------------------------------------------------------------------------------------------------------------
Int32 SharedEventInfo::DeRef( void )
{
    Int32 Result;
    
    pthread_mutex_lock( &( FEventStructPtr->HCountMutex) );
    FEventStructPtr->RefCount--;
    Result = FEventStructPtr->RefCount;
    pthread_mutex_unlock( &( FEventStructPtr->HCountMutex) );
    return Result;
}
//---------------------------------------------------------------------------
void SharedEventInfo::IncreaseTrigger( void )
{
   pthread_mutex_lock( &( FEventStructPtr->HCountMutex) );    
   FEventStructPtr->TriggerCount++;
   pthread_mutex_unlock( &( FEventStructPtr->HCountMutex) );
}    
//---------------------------------------------------------------------------
BOOL SharedEventInfo::DecreaseTrigger( void )
{
   BOOL rtn;
   pthread_mutex_lock( &( FEventStructPtr->HCountMutex) );    
   if( FEventStructPtr->TriggerCount >= 1 )
   {
       FEventStructPtr->TriggerCount--;
       rtn = TRUE;
   }
   else
   {
       FEventStructPtr->TriggerCount = 0;
       rtn = FALSE;
   }
   pthread_mutex_unlock( &( FEventStructPtr->HCountMutex) );
   return rtn;
}    
//---------------------------------------------------------------------------
Int32 SharedEventInfo::TriggerCount( void )
{
   return FEventStructPtr->TriggerCount; 
}    
//-------------------------------------------------------------------------------------------------------------------
// class PEvent
//
// Constructor for a named evnet object.
//-------------------------------------------------------------------------------------------------------------------
PEvent::PEvent( const UFC::AnsiString& EventName, BOOL Shared )
{    
    UFC::PLockObject Lock( EventsTableCS );
    try
    {
        ///< EventName object exists?
        if( (FEventInfo = EventsTable.GetObjectByKey( EventName )) == NULL )
        {   ///< No, create a new one, and add to event hash table.
            if( Shared == FALSE )
                FEventInfo = new PrviateEventInfo( EventName  );
            else
                FEventInfo = new SharedEventInfo( EventName  );
            EventsTable.Add( EventName, FEventInfo );
        }
        ///< Reference the existing event object.
        FEventInfo->RefEvent( m_eventRef, m_mutexRef );
    }
    catch( UFC::Exception& e )
    {
        printf( " Create nemed event[%s] failed:%s\n", EventName.c_str(), e.what() );
        exit( 0 );
    }
}
//-------------------------------------------------------------------------------------------------------------------
// class PEvent
//
// Constructor for a event object.
//-------------------------------------------------------------------------------------------------------------------
PEvent::PEvent( void )
:FEventInfo( new PrviateEventInfo(  ) )
{
    ///< Reference the existing event object.
    FEventInfo->RefEvent( m_eventRef, m_mutexRef );    
}
//-------------------------------------------------------------------------------------------------------------------
PEvent::~PEvent()
{
    if( FEventInfo != NULL  ) 
    {
        if( FEventInfo->DeRef() <= 0 ) ///< No objects reference this event object
        {
            if( FEventInfo->GetName( ).Length() > 0 ) ///< Named event.
            {
                UFC::PLockObject Lock( EventsTableCS );
                ///< Remove Event info object.
                EventsTable.DeleteByKey( FEventInfo->GetName( ));
            }
            delete FEventInfo;
        }        
    }    
}
//-------------------------------------------------------------------------------------------------------------------
void PEvent::SetEvent( void )
{
    pthread_mutex_lock( m_mutexRef );
    FEventInfo->IncreaseTrigger();    
    pthread_cond_broadcast( m_eventRef );
    pthread_mutex_unlock( m_mutexRef );
}
//-------------------------------------------------------------------------------------------------------------------
void PEvent::ResetEvent( void )
{
    pthread_mutex_lock( m_mutexRef );
    while( FEventInfo->DecreaseTrigger() == TRUE )
        ;   ///< Clear all pending triggers, or a stale signal survives into the next WaitFor.
    pthread_mutex_unlock( m_mutexRef );
}
//-------------------------------------------------------------------------------------------------------------------
BOOL PEvent::WaitFor( int Timeout, int ms )
{
    BOOL Result = FALSE;
    timespec ts;
    int WaitMS;
    struct timeval now;    

    pthread_mutex_lock( m_mutexRef );
    gettimeofday( &now, NULL ); ///< Get the current time.        
    if( FEventInfo->TriggerCount() > 0 )
    {
        if( FEventInfo->DecreaseTrigger() == TRUE )
        {
            pthread_mutex_unlock( m_mutexRef );
            return TRUE;
        }
    }
    WaitMS = (now.tv_usec/1000) + ms;
    if( Timeout <= 0 && ms <= 0 ) 
        ts.tv_sec = now.tv_sec + 86400; ///< No timeout,Wait for one day!!
    else
        ts.tv_sec = now.tv_sec + Timeout + (WaitMS/1000);
    ts.tv_nsec = ( WaitMS % 1000 )*1000000;
    if( pthread_cond_timedwait( m_eventRef, m_mutexRef, &ts ) == 0 )
    {
        FEventInfo->DecreaseTrigger(); ///< Auto reset.   
        Result = TRUE;
    }
    pthread_mutex_unlock( m_mutexRef );
    return Result;
}
//-------------------------------------------------------------------------------------------------------------------
BOOL PEvent::WaitForUS( int us )
{
    BOOL Result = FALSE;
    timespec ts;
    long int WaitUS;
    struct timeval now;    

    pthread_mutex_lock( m_mutexRef );
    gettimeofday( &now, NULL ); ///< Get the current time.        
    if( FEventInfo->TriggerCount() > 0 )
    {
        if( FEventInfo->DecreaseTrigger() == TRUE )
        {
            pthread_mutex_unlock( m_mutexRef );
            return TRUE;
        }
    }
    WaitUS = now.tv_usec + us;
    if( WaitUS > 1000000 ) ///< more then 1 sec
    {
        int WaitSec = WaitUS/1000000;
        WaitUS %= 1000000;
        ts.tv_sec  = now.tv_sec + WaitSec;
    }
    else
        ts.tv_sec = now.tv_sec;
    ts.tv_nsec = WaitUS * 1000;    
    if( pthread_cond_timedwait( m_eventRef, m_mutexRef, &ts ) == 0 )
    {
        FEventInfo->DecreaseTrigger(); ///< Auto reset.   
        Result = TRUE;
    }
    pthread_mutex_unlock( m_mutexRef );
    return Result;
}
//-------------------------------------------------------------------------------------------------------------------
#endif

#if defined(__LINUX) || defined(__AIX)
//-------------------------------------------------------------------------------------------------------------------
AnsiString GetLockMessageByCode(int LockCode)
{
    UFC::AnsiString errMsg;
    switch (LockCode)
    {
        case 0:       errMsg.Printf("0:Success");                                         break;
        case EPERM:   errMsg.Printf("%d_EPERM:Operation not permitted", EPERM);           break;  // 1
        case ENOENT:  errMsg.Printf("%d_ENOENT:No such file or directory", ENOENT);       break;  // 2
        case ESRCH:   errMsg.Printf("%d_ESRCH:No such process", ESRCH);                   break;  // 3
        case EINTR:   errMsg.Printf("%d_EINTR:Interrupted system call", EINTR);           break;  // 4
        case EIO:     errMsg.Printf("%d_EIO:I/O error", EIO);                             break;  // 5
        case ENXIO:   errMsg.Printf("%d_ENXIO:No such device or address", ENXIO);         break;  // 6
        case E2BIG:   errMsg.Printf("%d_E2BIG:Argument list too long", E2BIG);            break;  // 7
        case ENOEXEC: errMsg.Printf("%d_ENOEXEC:Exec format error", ENOEXEC);             break;  // 8
        case EBADF:   errMsg.Printf("%d_EBADF:Bad file number", EBADF);                   break;  // 9
        case ECHILD:  errMsg.Printf("%d_ECHILD:No child processes", ECHILD);              break;  //10
        case EAGAIN:  errMsg.Printf("%d_EAGAIN:Try again", EAGAIN);                       break;  //11
        case ENOMEM:  errMsg.Printf("%d_ENOMEM:Out of memory", ENOMEM);                   break;  //12
        case EACCES:  errMsg.Printf("%d_EACCES:Permission denied", EACCES);               break;  //13
        case EFAULT:  errMsg.Printf("%d_EFAULT:Bad address", EFAULT);                     break;  //14
        case ENOTBLK: errMsg.Printf("%d_ENOTBLK:Block device required", ENOTBLK);         break;  //15
        case EBUSY:   errMsg.Printf("%d_EBUSY:Device or resource busy", EBUSY);           break;  //16
        case EEXIST:  errMsg.Printf("%d_EEXIST:File exists", EEXIST);                     break;  //17
        case EXDEV:   errMsg.Printf("%d_EXDEV:Cross-device link", EXDEV);                 break;  //18
        case ENODEV:  errMsg.Printf("%d_ENOENT:No such device", ENODEV);                  break;  //19
        case ENOTDIR: errMsg.Printf("%d_ENOTDIR:Not a directory", ENOTDIR);               break;  //20
        case EISDIR:  errMsg.Printf("%d_EISDIR:Is a directory", EISDIR);                  break;  //21
        case EINVAL:  errMsg.Printf("%d_EINVAL:Invalid argument", EINVAL);                break;  //22
        case ENFILE:  errMsg.Printf("%d_ENFILE:File table overflow", ENFILE);             break;  //23
        case EMFILE:  errMsg.Printf("%d_EMFILE:Too many open files", EMFILE);             break;  //24
        case ENOTTY:  errMsg.Printf("%d_ENOTTY:Not a typewriter", ENOTTY);                break;  //25
        case ETXTBSY: errMsg.Printf("%d_ETXTBSY:Text file busy", ETXTBSY);                break;  //26
        case EFBIG:   errMsg.Printf("%d_EFBIG:File too large", EFBIG);                    break;  //27
        case ENOSPC:  errMsg.Printf("%d_ENOSPC:No space left on device", ENOSPC);         break;  //28
        case ESPIPE:  errMsg.Printf("%d_ESPIPE:Illegal seek", ESPIPE);                    break;  //29
        case EROFS:   errMsg.Printf("%d_EROFS:Read-only file system", EROFS);             break;  //30
        case EMLINK:  errMsg.Printf("%d_EMLINK:Too many links", EMLINK);                  break;  //31
        case EPIPE:   errMsg.Printf("%d_EPIPE:Broken pipe", EPIPE);                       break;  //32
        case EDOM:    errMsg.Printf("%d_EDOM:Math argument out of domain of func", EDOM); break;  //33
        case ERANGE:  errMsg.Printf("%d_ERANGE:Math result not representable", ERANGE);   break;  //34
        default:      errMsg.Printf("%d_Unknown Error", LockCode);
    }
    return errMsg;
}  //GetLockMessageByCode()
#endif

//------------------------------- PReadWriteLock -------------------------------
//------------------------------------------------------------------------------
PReadWriteLock::PReadWriteLock()
{
    FInitialErrorNo = 0;
    FLockErrorNo = 0;
    FErrorMessage = "";
#ifdef _WIN32
    InitializeSRWLock(&FReadWriteLockStruct);
#else
    FInitialErrorNo = pthread_rwlock_init(&FReadWriteLockStruct, 0);
    if (FInitialErrorNo != 0)
        FErrorMessage.Printf("Initial Read/Write Lock Failed! ErrorCode=%d", FInitialErrorNo);
#endif
}  //PReadWriteLock::PReadWriteLock()
//------------------------------------------------------------------------------
#ifndef _WIN32
PReadWriteLock::PReadWriteLock(int SharedAttribute)
{   //PTHREAD_PROCESS_SHARED, PTHREAD_PROCESS_PRIVATE
    FInitialErrorNo = 0;
    FLockErrorNo = 0;
    FErrorMessage = "";
    pthread_rwlockattr_t attributeOfLock;
    FInitialErrorNo = pthread_rwlockattr_init(&attributeOfLock);
    if (FInitialErrorNo == 0)
    {
        FInitialErrorNo = pthread_rwlockattr_setpshared(&attributeOfLock, SharedAttribute);
        if (FInitialErrorNo == 0)
        {
            FInitialErrorNo = pthread_rwlock_init(&FReadWriteLockStruct, &attributeOfLock);
            if (FInitialErrorNo != 0)
                FErrorMessage.Printf("Initial Read/Write Lock Failed! ErrorCode=%d", FInitialErrorNo);
        }
        else
            FErrorMessage.Printf("Set Read/Write Lock Attribute Failed! ErrorCode=%d", FInitialErrorNo);    
        pthread_rwlockattr_destroy(&attributeOfLock);
    }
    else
        FErrorMessage.Printf("Initial Read/Write Lock Attribute Failed! ErrorCode=%d", FInitialErrorNo);
}  //PReadWriteLock::PReadWriteLock()
#endif
//------------------------------------------------------------------------------
PReadWriteLock::~PReadWriteLock()
{
#ifndef _WIN32
    pthread_rwlock_destroy(&FReadWriteLockStruct);
#endif
}  //PReadWriteLock::~PReadWriteLock()
//------------------------------------------------------------------------------
int PReadWriteLock::LockForRead()
{
    FLockErrorNo = 0;
#ifdef _WIN32
    AcquireSRWLockShared(&FReadWriteLockStruct);
#else
    FLockErrorNo = pthread_rwlock_rdlock(&FReadWriteLockStruct);
#endif
    return FLockErrorNo;
}  //PReadWriteLock::LockForRead()
//------------------------------------------------------------------------------
int PReadWriteLock::LockForWrite()
{
    FLockErrorNo = 0;
#ifdef _WIN32
    AcquireSRWLockExclusive(&FReadWriteLockStruct);
#else
    FLockErrorNo = pthread_rwlock_wrlock(&FReadWriteLockStruct);
#endif
    return FLockErrorNo;
}  //PReadWriteLock::LockForWrite()
//------------------------------------------------------------------------------
int PReadWriteLock::Lock(RWLockTypeEnum LockType)
{
    if (LockType == rwLockForRead)
        return LockForRead();
    else if (LockType == rwLockForWrite) 
        return LockForWrite();
    else
    {
        FLockErrorNo = 0;
        return FLockErrorNo;
    }
}  //PReadWriteLock::Lock()
//------------------------------------------------------------------------------
#ifndef _WIN32
//------------------------------------------------------------------------------
int PReadWriteLock::TryLockForRead()
{
    FLockErrorNo = pthread_rwlock_tryrdlock(&FReadWriteLockStruct);
    return FLockErrorNo;
}  //PReadWriteLock::TryLockForRead()
//------------------------------------------------------------------------------
int PReadWriteLock::TryLockForWrite()
{
    FLockErrorNo = pthread_rwlock_trywrlock(&FReadWriteLockStruct);
    return FLockErrorNo;
}  //PReadWriteLock::TryLockForWrite()
//------------------------------------------------------------------------------
int PReadWriteLock::TryLock(RWLockTypeEnum LockType)
{
    if (LockType == rwLockForRead)
        return TryLockForRead();
    else if (LockType == rwLockForWrite) 
        return TryLockForWrite();
    else
    {
        FLockErrorNo = 0;
        return FLockErrorNo;
    }
}  //PReadWriteLock::TryLock()
//------------------------------------------------------------------------------
int PReadWriteLock::TryAndLockForRead(const AnsiString& FuncName, const AnsiString& ObjName, void* LogPtr)
{
    BufferedLog* logPtr = static_cast<BufferedLog*>(LogPtr);
    int errorNo = pthread_rwlock_tryrdlock(&FReadWriteLockStruct);
    if (errorNo == EBUSY)
    {
        logPtr->Printf(" %s() Waiting %s Read Lock.", FuncName.c_str(), ObjName.c_str());
        logPtr->Flush();
        errorNo = pthread_rwlock_rdlock(&FReadWriteLockStruct);
        if (errorNo == 0)
            logPtr->Printf(" %s() Lock %s for Read.", FuncName.c_str(), ObjName.c_str());
        else
            logPtr->Printf(" %s() Lock %s for Read failed:%s.", FuncName.c_str(), ObjName.c_str(), GetLockMessageByCode(errorNo).c_str());        
    }
    else if (errorNo != 0)
        logPtr->Printf(" %s() Try Lock %s for Read failed:%s.", FuncName.c_str(), ObjName.c_str(), GetLockMessageByCode(errorNo).c_str());        
    logPtr->Flush();
    
    FLockErrorNo = errorNo;
    return errorNo;    
}  //PReadWriteLock::TryAndLockForRead()
//------------------------------------------------------------------------------
int PReadWriteLock::TryAndLockForWrite(const AnsiString& FuncName, const AnsiString& ObjName, void* LogPtr)
{
    BufferedLog* logPtr = static_cast<BufferedLog*>(LogPtr);
    int errorNo = pthread_rwlock_trywrlock(&FReadWriteLockStruct);
    if (errorNo == EBUSY)
    {
        logPtr->Printf(" %s() Waiting %s Write Lock.", FuncName.c_str(), ObjName.c_str());
        logPtr->Flush();
        errorNo = pthread_rwlock_wrlock(&FReadWriteLockStruct);
        if (errorNo == 0)
            logPtr->Printf(" %s() Lock %s for Write.", FuncName.c_str(), ObjName.c_str());
        else
            logPtr->Printf(" %s() Lock %s for Write failed:%s.", FuncName.c_str(), ObjName.c_str(), GetLockMessageByCode(errorNo).c_str());        
    }
    else if (errorNo != 0)
        logPtr->Printf(" %s() Try Lock %s for Write failed:%s.", FuncName.c_str(), ObjName.c_str(), GetLockMessageByCode(errorNo).c_str());        
    logPtr->Flush();
    
    FLockErrorNo = errorNo;
    return errorNo;    
}  //PReadWriteLock::TryAndLockForWrite()
//------------------------------------------------------------------------------
int PReadWriteLock::TryAndLock(RWLockTypeEnum LockType, const AnsiString& FuncName, const AnsiString& ObjName, void* LogPtr)
{
    int errorNo = 0;
    if (LockType == rwLockForRead)
        errorNo = TryAndLockForRead(FuncName, ObjName, LogPtr);
    else if (LockType == rwLockForWrite)
        errorNo = TryAndLockForWrite(FuncName, ObjName, LogPtr);
    
    FLockErrorNo = errorNo;
    return errorNo;
}  //PReadWriteLock::TryAndLock()
//------------------------------------------------------------------------------
int PReadWriteLock::Unlock()
{
    FLockErrorNo = pthread_rwlock_unlock(&FReadWriteLockStruct);    
    return FLockErrorNo;
}  //PReadWriteLock::Unlock()
#endif
//------------------------------------------------------------------------------
int PReadWriteLock::UnlockForRead()
{
    FLockErrorNo = 0;
#ifdef _WIN32
    ReleaseSRWLockShared(&FReadWriteLockStruct);
#else
    FLockErrorNo = Unlock();
#endif    
    return FLockErrorNo;
}  //PReadWriteLock::UnlockForRead()
//------------------------------------------------------------------------------
int PReadWriteLock::UnlockForWrite()
{
    FLockErrorNo = 0;
#ifdef _WIN32
    ReleaseSRWLockExclusive(&FReadWriteLockStruct);
#else
    FLockErrorNo = Unlock();
#endif
    return FLockErrorNo;
}  //PReadWriteLock::UnlockForWrite()
//------------------------------------------------------------------------------
int PReadWriteLock::Unlock(RWLockTypeEnum LockType)
{
    if (LockType == rwLockForRead)
        return UnlockForRead();
    else if (LockType == rwLockForWrite) 
        return UnlockForWrite();
    else
    {
        FLockErrorNo = 0;
        return FLockErrorNo;
    }
}  //PReadWriteLock::Unlock()
//------------------------------------------------------------------------------
void DoReadWriteLock(PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    if (LockPtr != 0) LockPtr->Lock(LockType);
}  //DoReadWriteLock()
//------------------------------------------------------------------------------
void DoReadWriteUnlock(PReadWriteLock *LockPtr, UFC::RWLockTypeEnum LockType)
{
    if (LockPtr != 0) LockPtr->Unlock(LockType);
}  //DoReadWriteUnlock()

//-------------------------------------------------------------------------------------------------------------------
// CBasicRWLockObject
CBasicRWLockObject::CBasicRWLockObject(bool CreateLock)
:FRWLockPtr(0)
{
    if (CreateLock) FRWLockPtr = new UFC::PReadWriteLock();
}  //CBasicRWLockObject::CBasicRWLockObject()
//-------------------------------------------------------------------------------------------------------------------
CBasicRWLockObject::~CBasicRWLockObject()
{
    if (FRWLockPtr != 0) delete FRWLockPtr;
}  //CBasicRWLockObject::~CBasicRWLockObject()
//-------------------------------------------------------------------------------------------------------------------
// for Windows,Solaris,AIX and Linux
bool CBasicRWLockObject::HasLock()
{
    if (FRWLockPtr != 0) 
        return true;
    else
        return false;
}  //CBasicRWLockObject::HasLock()
#if defined(__LINUX) || defined(__AIX)
//-------------------------------------------------------------------------------------------------------------------
int CBasicRWLockObject::TryLock(UFC::RWLockTypeEnum LockType)
{
    int lockErrorNo = 0;
    if (FRWLockPtr != 0)
    {
        if (LockType == rwLockForRead)
            lockErrorNo = FRWLockPtr->TryLockForRead();
        else if (LockType == rwLockForWrite) 
            lockErrorNo = FRWLockPtr->TryLockForWrite();
    }
    return lockErrorNo;
}  //CBasicRWLockObject::Lock()
//------------------------------------------------------------------------------
int CBasicRWLockObject::TryLockForRead()
{
    int lockErrorNo = 0;
    if (FRWLockPtr != 0)  lockErrorNo = FRWLockPtr->TryLockForRead();
    return lockErrorNo;
}  //CBasicRWLockObject::TryLockForRead()
//------------------------------------------------------------------------------
int CBasicRWLockObject::TryLockForWrite()
{
    int lockErrorNo = 0;
    if (FRWLockPtr != 0)  lockErrorNo = FRWLockPtr->TryLockForWrite();
    return lockErrorNo;
}  //CBasicRWLockObject::TryLockForWrite()
//-------------------------------------------------------------------------------------------------------------------
int CBasicRWLockObject::Lock(UFC::RWLockTypeEnum LockType)
{
    int lockErrorNo = 0;
    if (FRWLockPtr != 0)
    {
        if (LockType == rwLockForRead)
            lockErrorNo = FRWLockPtr->LockForRead();
        else if (LockType == rwLockForWrite) 
            lockErrorNo = FRWLockPtr->LockForWrite();
    }
    return lockErrorNo;
}  //CBasicRWLockObject::Lock()
//-------------------------------------------------------------------------------------------------------------------
int CBasicRWLockObject::Unlock(UFC::RWLockTypeEnum LockType)
{
    int lockErrorNo = 0;
    if (FRWLockPtr != 0)
    {
        if (LockType == rwLockForRead)
            lockErrorNo = FRWLockPtr->UnlockForRead();
        else if (LockType == rwLockForWrite) 
            lockErrorNo = FRWLockPtr->UnlockForWrite();
    }
    return lockErrorNo;
}  //CBasicRWLockObject::Unlock()
//-------------------------------------------------------------------------------------------------------------------
int CBasicRWLockObject::Unlock()
{
    int lockErrorNo = 0;
    if (FRWLockPtr != 0) lockErrorNo = FRWLockPtr->Unlock();
    return lockErrorNo;
}  //CBasicRWLockObject::Unlock()
//------------------------------------------------------------------------------
int CBasicRWLockObject::LockForRead()
{
    int lockErrorNo = 0;
    if (FRWLockPtr != 0) lockErrorNo = FRWLockPtr->LockForRead();
    return lockErrorNo;
}  //CBasicRWLockObject::LockForRead()
//------------------------------------------------------------------------------
int CBasicRWLockObject::UnLockForRead()
{
    int lockErrorNo = 0;
    if (FRWLockPtr != 0) lockErrorNo = FRWLockPtr->UnlockForRead();
    return lockErrorNo;
}  //CBasicRWLockObject::UnLockForRead()
//------------------------------------------------------------------------------
int CBasicRWLockObject::LockForWrite()
{
    int lockErrorNo = 0;
    if (FRWLockPtr != 0) lockErrorNo = FRWLockPtr->LockForWrite();
    return lockErrorNo;
}  //CBasicRWLockObject::LockForWrite()
//------------------------------------------------------------------------------
int CBasicRWLockObject::UnLockForWrite()
{
    int lockErrorNo = 0;
    if (FRWLockPtr != 0) lockErrorNo = FRWLockPtr->UnlockForWrite();
    return lockErrorNo;
}  //CBasicRWLockObject::UnLockForWrite()
#else  //_WIN32
//-------------------------------------------------------------------------------------------------------------------
void CBasicRWLockObject::Lock(UFC::RWLockTypeEnum LockType)
{
    if (FRWLockPtr != 0)
    {
        if (LockType == rwLockForRead)
            FRWLockPtr->LockForRead();
        else if (LockType == rwLockForWrite) 
            FRWLockPtr->LockForWrite();
    }
}  //CBasicRWLockObject::Lock()
//-------------------------------------------------------------------------------------------------------------------
void CBasicRWLockObject::Unlock(UFC::RWLockTypeEnum LockType)
{
    if (FRWLockPtr != 0)
    {
        if (LockType == rwLockForRead)
            FRWLockPtr->UnlockForRead();
        else if (LockType == rwLockForWrite) 
            FRWLockPtr->UnlockForWrite();
    }
}  //CBasicRWLockObject::Unlock()
//------------------------------------------------------------------------------
void CBasicRWLockObject::LockForRead()
{
    if (FRWLockPtr != 0) FRWLockPtr->LockForRead();
}  //CBasicRWLockObject::LockForRead()
//------------------------------------------------------------------------------
void CBasicRWLockObject::UnLockForRead()
{
    if (FRWLockPtr != 0) FRWLockPtr->UnlockForRead();
}  //CBasicRWLockObject::UnLockForRead()
//------------------------------------------------------------------------------
void CBasicRWLockObject::LockForWrite()
{
    if (FRWLockPtr != 0) FRWLockPtr->LockForWrite();
}  //CBasicRWLockObject::LockForWrite()
//------------------------------------------------------------------------------
void CBasicRWLockObject::UnLockForWrite()
{
    if (FRWLockPtr != 0) FRWLockPtr->UnlockForWrite();
}  //CBasicRWLockObject::UnLockForWrite()
#endif

//------------------------------- CMainKeyObject -------------------------------
//------------------------------------------------------------------------------
CMainKeyObject::CMainKeyObject(bool NeedLock)
:CBasicRWLockObject(NeedLock)
,FMainIndexKey("")
{
}  //CMainKeyObject::CMainKeyObject()
//------------------------------------------------------------------------------
CMainKeyObject::CMainKeyObject(bool NeedLock, const UFC::AnsiString MainIndexKey)
:UFC::CBasicRWLockObject(NeedLock)
,FMainIndexKey(MainIndexKey)
{
}  //CMainKeyObject::CMainKeyObject()

//------------------------------ CMainKeyObjectL ------------------------------_
//------------------------------------------------------------------------------
CMainKeyObjectL::CMainKeyObjectL(bool NeedLock)
:CBasicRWLockObject(NeedLock)
,FMainIndexKey("")
{
}  //CMainKeyObjectL::CMainKeyObjectL()
//------------------------------------------------------------------------------
CMainKeyObjectL::CMainKeyObjectL(bool NeedLock, const UFC::AnsiString MainIndexKey)
:UFC::CBasicRWLockObject(NeedLock)
,FMainIndexKey(MainIndexKey)
{
}  //CMainKeyObjectL::CMainKeyObjectL()
//------------------------------------------------------------------------------
void CMainKeyObjectL::GenerateMainIndexKey_L(UFC::RWLockTypeEnum LockType)
{
    if (FRWLockPtr != 0) FRWLockPtr->Lock(LockType);
    GenerateMainIndexKey();
    if (FRWLockPtr != 0) FRWLockPtr->Unlock(LockType);
}  //CMainKeyObjectL::GenerateMainIndexKey_L()
//------------------------------------------------------------------------------
UFC::AnsiString CMainKeyObjectL::ToString_L(UFC::RWLockTypeEnum LockType)
{
    if (FRWLockPtr != 0) FRWLockPtr->Lock(LockType);
    UFC::AnsiString objStr = ToString();
    if (FRWLockPtr != 0) FRWLockPtr->Unlock(LockType);
    return objStr;
}  //CMainKeyObjectL::ToString_L()

//------------------------------ PConditionMutex -------------------------------
//------------------------------------------------------------------------------
PConditionMutex::PConditionMutex()
:FOwnerThreadId(0)
,FIsLocked(false)
{
#ifdef _WIN32
    InitializeCriticalSection(&FMutex);
    InitializeConditionVariable(&FCondition);
#else
    pthread_mutex_init(&FMutex, 0);
    pthread_cond_init(&FCondition, 0);
#endif
}  //PConditionMutex::PConditionMutex()
//------------------------------------------------------------------------------
PConditionMutex::~PConditionMutex()
{
#ifdef _WIN32
    DeleteCriticalSection(&FMutex);
#else
    pthread_cond_destroy(&FCondition);
    pthread_mutex_destroy(&FMutex);
#endif
}  //PConditionMutex::~PConditionMutex()
//------------------------------------------------------------------------------
 int PConditionMutex::TryLock()
 {
     int tryResult = 0;
#ifdef _WIN32
    tryResult = TryEnterCriticalSection(&FMutex);
    if (tryResult != 0) FOwnerThreadId = GetCurrentThreadId();
#else
    tryResult = pthread_mutex_trylock(&FMutex);
    if (tryResult == 0) FOwnerThreadId = pthread_self();
#endif
     return tryResult;
 }  //PConditionMutex::TryLock()
 
#ifdef _WIN32
//------------------------------------------------------------------------------
void PConditionMutex::Lock()
{
#ifdef _WIN32
    EnterCriticalSection(&FMutex);
    FOwnerThreadId = GetCurrentThreadId();
#else
    pthread_mutex_lock(&FMutex);
    FOwnerThreadId = pthread_self();
#endif
    FIsLocked = true;
}  //PConditionMutex::Lock()
//------------------------------------------------------------------------------
void PConditionMutex::Unlock()
{
    FIsLocked = false;
    FOwnerThreadId = 0;
    LeaveCriticalSection(&FMutex);
}  //PMessageMutex::Unlock()
#else
//------------------------------------------------------------------------------
int PConditionMutex::Lock()
{
    int errorNo = pthread_mutex_lock(&FMutex);
    if (errorNo == 0)
    {
        FOwnerThreadId = pthread_self();
        FIsLocked = true;
    }
    return errorNo;
}  //PConditionMutex::Lock()
//------------------------------------------------------------------------------
int PConditionMutex::TryAndLock(const AnsiString& FuncName, const AnsiString& ObjName, void* LogPtr)
{
    BufferedLog* logPtr = static_cast<BufferedLog*>(LogPtr);
    int errorNo = pthread_mutex_trylock(&FMutex);
    if (errorNo == EBUSY)
    {
        logPtr->Printf(" %s() Waiting %s Write Lock.", FuncName.c_str(), ObjName.c_str());
        logPtr->Flush();
        errorNo = pthread_mutex_lock(&FMutex);
        if (errorNo == 0)
            logPtr->Printf(" %s() Lock %s for Write.", FuncName.c_str(), ObjName.c_str());
        else
            logPtr->Printf(" %s() Lock %s for Write failed:%s.", FuncName.c_str(), ObjName.c_str(), GetLockMessageByCode(errorNo).c_str());        
    }
    else if (errorNo != 0)
        logPtr->Printf(" %s() Try Lock %s for Write failed:%s.", FuncName.c_str(), ObjName.c_str(), GetLockMessageByCode(errorNo).c_str());        
    logPtr->Flush();
    
    return errorNo;    
}  //PConditionMutex::TryAndLock()
//------------------------------------------------------------------------------
int PConditionMutex::Unlock()
{
    int errorNo = pthread_mutex_unlock(&FMutex);
    if (errorNo == 0)
    {
        FIsLocked = false;
        FOwnerThreadId = 0;
    }
    return errorNo;
}  //PMessageMutex::Unlock()
#endif

//------------------------------------------------------------------------------
void PConditionMutex::Signal()
{
#ifdef _WIN32
    WakeConditionVariable(&FCondition);
#else
    pthread_cond_signal(&FCondition);
#endif
}  //PMessageMutex::Signal()
//------------------------------------------------------------------------------
void PConditionMutex::Broadcast()
{
#ifdef _WIN32
    WakeAllConditionVariable(&FCondition);
#else
    pthread_cond_broadcast(&FCondition);
#endif
}  //PConditionMutex::Broadcast()
//------------------------------------------------------------------------------
int PConditionMutex::Wait()
{
    FIsLocked = false;
    FOwnerThreadId = 0;
#ifdef _WIN32
    int WaitResult = SleepConditionVariableCS(&FCondition, &FMutex, INFINITE);    
#else
    int WaitResult = pthread_cond_wait(&FCondition, &FMutex);
#endif
    return WaitResult;
}  //PMessageMutex::Wait()
//------------------------------------------------------------------------------
int PConditionMutex::Wait(int WaitingMilliseconds)
{
    FIsLocked = false;
    FOwnerThreadId = 0;
    int WaitResult = 0;
    if (WaitingMilliseconds <= 0)
    {
#ifdef _WIN32
        WaitResult = SleepConditionVariableCS(&FCondition, &FMutex, INFINITE);    
#else
        WaitResult = pthread_cond_wait(&FCondition, &FMutex);
#endif
        return WaitResult;
    }

#ifdef _WIN32
    WaitResult = SleepConditionVariableCS(&FCondition, &FMutex, WaitingMilliseconds);    
#else
    struct timeval tvNow;  //__suseconds_t timeval.tv_usec is microsecond(0.000001)
    gettimeofday(&tvNow, 0);
    long tempMicorseconds = tvNow.tv_usec + WaitingMilliseconds * 1000L;
    time_t WaitingSecond = tempMicorseconds / 1000000L;
    struct timespec tsTimeOut;  //long int timespec.tv_nsec is nanosecond(0.000000001)
//std::printf("Now timeval.tv_sec=%ld  timeval.tv_usec=%ld\n", tvNow.tv_sec, tvNow.tv_usec);
    tsTimeOut.tv_nsec = (tempMicorseconds % 1000000L) * 1000L;
    tsTimeOut.tv_sec = tvNow.tv_sec + WaitingSecond;
//std::printf("Now timespec.tv_sec=%ld  timespec.tv_nsec=%ld\n", tsTimeOut.tv_sec, tsTimeOut.tv_nsec);
    WaitResult = pthread_cond_timedwait(&FCondition, &FMutex, &tsTimeOut);
#endif
    return WaitResult;
}  //PConditionMutex::Wait()
//------------------------------------------------------------------------------
bool PConditionMutex::IsLockedByMe()
{
#ifdef _WIN32
    DWORD curThreadId = GetCurrentThreadId();
#else
    pthread_t curThreadId = pthread_self();
#endif
    return (FIsLocked && (FOwnerThreadId == curThreadId));
}  //PConditionMutex::IsLockedByMe()

//-------------------------------------------------------------------------------------------------------------------
// Class PThread
// 
// for Windows,Solaris,AIX and Linux
//-------------------------------------------------------------------------------------------------------------------
// Static vares & functions for PThread class
//-------------------------------------------------------------------------------------------------------------------
void* PThread::GlobalExecute( void * ThisThread )
{
    PThread*  ThreadObj = (PThread*)ThisThread;
    BOOL      FreeOnTerminate = ThreadObj->FFreeOnTerminate;

    ThreadObj->FTerminated = FALSE;
    ///< Call the "Execute" function.    
    try
    {
        ThreadObj->Execute(); 
        ///< Trigger the OnTerminate event.
        if( ThreadObj->FListener != NULL )
            ThreadObj->FListener->OnTerminate( ThreadObj );         
    }
    catch( Exception& e )
    {
        UFC::BufferedLog::Printf(" Thread %d exception:%s.", ThreadObj->FThreadID, e.what() );
    }
    catch( ... )
    {
        UFC::BufferedLog::Printf(" Thread %d stopped. Unknown exception.", ThreadObj->FThreadID );
    }
    ///< Set the stopped flag to "true".
    ThreadObj->FStopped = TRUE; 
    ///< Trigger the thread terminated event.
    ThreadObj->FTerminateEvent.SetEvent(); 
    
    if( GLogThread == TRUE )
    {
        AnsiString Now;
        char Buffer[256];
                
        UFC::GetTimeString( Now, TRUE );
		/// Allen modify at 20190614
#ifdef _MSC_VER
        #if defined(_WIN64)
            _snprintf( Buffer, 256, "%s ID[%09llu] Thread[%s] Stopped\n", Now.c_str(), (unsigned long long)ThreadObj->FThreadID, ThreadObj->FName );
        #else
            _snprintf(Buffer, 256, "%s ID[%09u] Thread[%s] Stopped\n", Now.c_str(), (unsigned int)ThreadObj->FThreadID, ThreadObj->FName);
        #endif
#else
        snprintf( Buffer, 256, "%s ID[%09u] Thread[%s] Stopped\n", Now.c_str(), (unsigned int)ThreadObj->FThreadID, ThreadObj->FName );
#endif
        UFC::PLockObject Lock( PThread::GThreadCS);        
        FileStreamEx File( ThreadObj->FFileName, "a+" );                    
        File.Write( Buffer, strlen(Buffer) ); 
    }
    
    
    ///< If FreeOnTerminate equal true, Delete the thread object after it stopped.
    if( FreeOnTerminate == TRUE )
        delete ThreadObj;        
#ifdef __LINUX    
    pthread_exit( 0 );
#endif        
    return NULL;
}
//-------------------------------------------------------------------------------------------------------------------
PThread::PThread( ThreadListener *Listener, BOOL FreeOnTerminate, Int32 Priority )
:FListener(Listener)
,FFreeOnTerminate( FreeOnTerminate )
,FTerminated( TRUE )
,FStopped( TRUE )
,FPriority( Priority )
#ifdef _WIN32
,FThreadID( NULL )
#else
,FThreadID( 0 )
#endif
{
}
//-------------------------------------------------------------------------------------------------------------------
PThread::~PThread( void )
{
#ifdef _WIN32
    if( FThreadID != NULL )
        CloseHandle( FThreadID );
#endif
}
//-------------------------------------------------------------------------------------------------------------------
void PThread::Start( UInt32 StackSize )
{
    FStopped = FALSE;
#ifdef _WIN32
    DWORD ThreadID;
    ///< Create thread using Win32 API
    if( (FThreadID = CreateThread( NULL, StackSize,( LPTHREAD_START_ROUTINE)PThread::GlobalExecute,this,0,&ThreadID))==NULL)
        throw( ThreadException( GetLastError()));
    if( SetThreadPriority( FThreadID, FPriority ) == FALSE )
        throw( ThreadException( GetLastError()));
#else
    int             ErrCode;
    pthread_attr_t  ThreadAttrib;
    ///< Create thread attbibute
    if( (ErrCode = pthread_attr_init( &ThreadAttrib )) != 0 )
    	throw( ThreadException( ErrCode ));
    if( StackSize < 16*1024 )
        StackSize = 16*1024; ///< Stack size at least 16 kb
    if( (ErrCode = pthread_attr_setstacksize(&ThreadAttrib, StackSize )) )
    {
         printf("ERROR: rc from pthread_attr_setstacksize() is %d\n", ErrCode);
         throw( ThreadException( ErrCode ));
    }
    if( GLogThread == TRUE )
    {
        AnsiString Today;
        
        UFC::GetYYYYMMDD( Today );
        UFC::StrLCpy( FName, typeid(*this).name(), 64 );       
#ifdef __AIX
        snprintf( FFileName, PATH_MAX, "%s/%dThread_%s.txt", WorkingDir, getpid(), Today.c_str() );
#else
        snprintf( FFileName, PATH_MAX, "%s/%sThread_%s.txt", WorkingDir, program_invocation_short_name, Today.c_str() );                
#endif
    }        
    ///< Create the thread    
    if( ( ErrCode = pthread_create( &FThreadID, &ThreadAttrib, PThread::GlobalExecute, this ))!=0 )
        throw( ThreadException( ErrCode ));    
    ///< Log each thread 
    if( GLogThread == TRUE )
    {
        AnsiString Now;
        char Buffer[ 256 ];
        
        UFC::GetTimeString( Now, TRUE );                
        snprintf( Buffer, 256, "%s ID[%09u] Thread[%s] Stack[%d] running.\n",Now.c_str(), (unsigned int)FThreadID, FName, StackSize );        
        
        UFC::PLockObject Lock( PThread::GThreadCS);        
        
        FileStreamEx File( FFileName, "a+" );        
        File.Write( Buffer, strlen(Buffer) ); 
    }       
#endif
}
//-------------------------------------------------------------------------------------------------------------------
BOOL PThread::WaitFor( int Sec )
{
    if( FStopped == TRUE )
        return TRUE; ///< Thread already stopped. return directly.
    return FTerminateEvent.WaitFor( Sec ); ///< Wait for the thread terminated event.
}
//-------------------------------------------------------------------------------------------------------------------
void PThread::Terminate( void )
{
    FTerminated = TRUE;
}
//-------------------------------------------------------------------------------------------------------------------
void PThread::PThread_Yield( void )
{
#ifdef _WIN32
	///< No need yield in Windows system.
#else
	sched_yield();
#endif
}
//-------------------------------------------------------------------------------------------------------------------
void PThread::PThread_setaffinity( int CPUID )
{
#ifdef _WIN32
	///< No pthread_setaffinity_np in Windows system.
#else 
#ifndef __AIX
    cpu_set_t cpuset;
    int rtn;
            
    CPU_ZERO( &cpuset );
    CPU_SET( CPUID, &cpuset );
    rtn = pthread_setaffinity_np ( FThreadID, sizeof(cpu_set_t),  &cpuset ) ;
    if( rtn != 0 )
        UFC::BufferedLog::Printf( " Set thread[%u] affinity failed. code[%d]", FThreadID, rtn );        
#endif
#endif
}
//-------------------------------------------------------------------------------------------------------------------
bool PThread::ThreadBindingCores( const UFC::AnsiString& Name, UFC::PInt32List& Cores )
{
#ifdef __LINUX    
    if( UFC::FileExists( "../cfg/BindingCore.cfg") == TRUE )
    {
        UFC::AnsiString CoreString;
        UFC::UiniFile   Config( "../cfg/BindingCore.cfg" );        
        
        if( Config.GetValue( Name, "core", CoreString ) == TRUE )
        {
            UFC::PStringList CoreList;
            
            CoreList.SetStrings( CoreString, ",\n" );
            if( CoreList.ItemCount() > 0 )
            {
                int CMax = UFC::GetCPUs( );
                int Core;
                
                Cores.Clear();
                for( int i = 0; i < CoreList.ItemCount(); i++ )
                {
                    Core = CoreList[i].ToInt();
                    ///< Check core exists. 
                    if( Core < 0 ) 
                        Core = 0;
                    if( Core >= CMax ) 
                        Core = CMax-1;
                    ///< Add to list if not exists.
                    if( Cores.IndexOf( Core ) == -1  )
                        Cores.Add( Core );                    
                }
                return true;                
            }            
        }        
    }
#endif    
    return false;
}
//-------------------------------------------------------------------------------------------------------------------
}
//-------------------------------------------------------------------------------------------------------------------

