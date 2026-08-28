//---------------------------------------------------------------------------
// This is a part of the UNIX Foundation Classes Library for UNIX platforms.
// Copyright (c) 2003 MobileDigital Development Inc.
// All rights reserved.
//
// Description: Thread Manipulating Class.
// Author: Simon Chang
//
// This source code is only intended as a supplement to the
// Object Class Library for UNIX platforms Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// UNIX Foundation Classes Library for UNIX platforms product.
//---------------------------------------------------------------------------
#ifndef __UFC_PTHREAD_H
#define __UFC_PTHREAD_H
//---------------------------------------------------------------------------
#include "Exception.h"
#include "AnsiString.h"
#include "PHashedList.h"
#if __cplusplus >= 201103L
#include <atomic>
#endif

//---------------------------------------------------------------------------
#ifndef _WIN32
    #include <sys/shm.h>
    #include <unistd.h>
    #include <pthread.h>
#endif
//---------------------------------------------------------------------------
#ifdef __APPLE__ 
#include <sys/time.h>
#endif
//---------------------------------------------------------------------------
#ifdef __LINUX
    #include <sys/time.h>
    #include <limits.h>
#endif
//---------------------------------------------------------------------------
namespace UFC
{
extern void SleepMS( unsigned int ms );
extern void SleepUS( unsigned int us );
extern void SleepNS( UInt32 ns );
//----------------------------------------------------------------------------------------------------------------------
class PThread;
//----------------------------------------------------------------------------------------------------------------------
/**
 * The <code>PCriticalSection</code> PCriticalSection allows a thread in a multi-threaded application to temporarily block other threads from accessing a block of code.
 */
class PCriticalSection
{
private:
#ifdef _WIN32
    CRITICAL_SECTION FMutex;
#else
    pthread_mutex_t FMutex;
    pthread_t       FthreadID;
    int             FCount;
#endif
public:
	/**
	 * Creates an instance of PCriticalSection.
	 */
	PCriticalSection( void );
	/**
	 * Destroys an instance of PCriticalSection.
	 */
	virtual ~PCriticalSection( void );
	/**
	 * Call Acquire to block all other threads from acquiring this critical section until the Release or Leave method is called. Acquire does the same thing as the Enter method.
	 */
	virtual void Acquire( void );
#ifndef _WIN32
    virtual bool TryAcquire( void );
#endif
	/**
	 * Call Release when the thread no longer needs to block other threads from executing code that is protected by this critical section.
	 * Each call to Release should be balance by an earlier call to Acquire or Enter that blocked other threads from executing.
	 * Release does the same thing as the Leave method.
	 */
	virtual void Release( void );
	/**
	 * Call Enter to block all other threads from entering code protected by this critical section until the Leave or Release method is called. Enter calls the Acquire method to bind the critical section to the calling thread.
	 */
	void Enter( void );
#ifndef _WIN32
    bool TryEnter( void );
#endif
	/**
	 * Call Leave at the end of a thread-sensitive section of code. Each call to Leave should be balanced by a call to Enter or Acquire at the beginning of the thread-sensitive section.
	 * Leave calls Release to allow other threads to acquire the critical section.
	 */
	void Leave( void );
};
//----------------------------------------------------------------------------------------------------------------------
/**
 *  The <code>PLockObject</code> PLockObject manipulate a PCriticalSection object to temporarily block other threads from accessing a block of code.
 *  The PLockObject will block threads when it's been construction, and unlock when it is been destruction.
 */
class PLockObject
{
private:
     PCriticalSection& FCS;
public:
	 /**
	 * Creates an instance of PLockObject and enter the critical section.
	 */
	 PLockObject( PCriticalSection& CS ):FCS( CS ){ FCS.Enter();}
	 /**
	 * Destroys an instance of PCriticalSection and leave the critical section.
	 */
	 ~PLockObject( ){ FCS.Leave(); }
};
//----------------------------------------------------------------------------------------------------------------------
/**
 *  The <code>PLockObject</code> PLockObject manipulate a PCriticalSection object to temporarily block other threads from accessing a block of code.
 *  The PLockObject will block threads when it's been construction, and unlock when it is been destruction.
 */
template <class T>
class PtrDeleteObject
{
private:
     T& FPtr;
public:
     /**
     * Creates an instance of PtrDeleteObject and reference an object.
     */
     PtrDeleteObject( T& Ptr ):FPtr( Ptr ){ }
     /**
     * Destroys an instance of PtrDeleteObject and delete the reference object.
     */
     ~PtrDeleteObject( )
     {
         if( FPtr != NULL )
             delete FPtr;
         FPtr = NULL;
     }
};
//----------------------------------------------------------------------------------------------------------------------
#ifndef _WIN32
//----------------------------------------------------------------------------------------------------------------------
class EventInfo
{
public:
    UFC::AnsiString FName;
    const AnsiString& GetName( void ) { return FName; }
public:
    EventInfo( const AnsiString& Name ):FName( Name ){}
    virtual ~EventInfo( void ) {}
    virtual void   RefEvent( pthread_cond_t*& Event, pthread_mutex_t*& Mutex ) = 0;
    virtual Int32  DeRef( void ) = 0;
    virtual void   IncreaseTrigger( void ) = 0;
    virtual BOOL   DecreaseTrigger( void ) = 0;
    virtual Int32  TriggerCount( void )  = 0;
};
//----------------------------------------------------------------------------------------------------------------------
class PrviateEventInfo : public EventInfo
{
public:
    pthread_cond_t   FEvent;
    pthread_mutex_t  FMutex;
    Int32            FCount;
    Int32            FTriggerCount;
public:    
    PrviateEventInfo( const AnsiString& Name = "" );
    ~PrviateEventInfo( void );
    void              RefEvent( pthread_cond_t*& Event, pthread_mutex_t*& Mutex );
    Int32             DeRef( void );    
    void              IncreaseTrigger( void );
    BOOL              DecreaseTrigger( void );
    Int32             TriggerCount( void );    
};
//----------------------------------------------------------------------------------------------------------------------
class SharedEventInfo : public EventInfo
{
public:
    typedef struct
    {
        pthread_cond_t   HEvent;
        pthread_mutex_t  HMutex;
        pthread_mutex_t  HCountMutex;
        Int32            RefCount;
        Int32            TriggerCount;

    } SharedEventStruct;
public:
    SharedEventStruct* FEventStructPtr;
    Int32              FShmID;
private:
    void               CreateIPCS( key_t Key );
    void               DestroyIPCS( void );
    void               AttachShareMemmory( void );
public:
    SharedEventInfo( const AnsiString& Name );
    ~SharedEventInfo( void );
    void              RefEvent( pthread_cond_t*& Event, pthread_mutex_t*& Mutex );
    Int32             DeRef( void );
    void              IncreaseTrigger( void );
    BOOL              DecreaseTrigger( void ); 
    Int32             TriggerCount( void );        
};
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------
/**
 * The <code>PEvent</code> Class PEvent represents an external event.
 */
class PEvent
{
private:
#ifdef _WIN32
	HANDLE FEvent;
	BOOL   FTriggered;
#else
    pthread_cond_t*  m_eventRef;
    pthread_mutex_t* m_mutexRef;
    EventInfo*       FEventInfo;
public:
    ///< Construct-on-first-use accessors: a global PEvent's constructor may run before this
    ///< translation unit's static objects when UFC is statically linked, so plain static
    ///< members would be used before their constructors run and crash at startup.
    static UFC::PHashedList<UFC::AnsiString, EventInfo*>& EventsTable( void );
    static UFC::PCriticalSection&                         EventsTableCS( void );
#endif
public:
    /**
     * Constructor for named event object.
     *
     * Create a named event object. thread wants to get event just needs to create
     * an event object with the same name.
     *
     * @Param EventName, Name of this event object
     * @Param Shared, TRUE  Process sharing of this event object.
     *
     */
    PEvent( const UFC::AnsiString& EventName, BOOL Shared = FALSE );
    /**
     * Default constructor for event object.
     *
     * Create a non-named event object. thread wants to get event should reference
     * the same event object instance.
     *
     */
    PEvent( void );
    /**
     *
     * Destructor
     * Destroys an instance of PEvent.
     *
     */
    ~PEvent( void );
    /**
     *
     * Call SetEvent to signal that the event has occurred or the desired state was achieved.
     * Other threads or processes that wait for the event will resume execution with a return value
     * indicating that the signal was set.
     *
     */
    void SetEvent( void );
    void ResetEvent( void );
    /**
     *
     * Call WaitFor to wait until the signal of the event object is turned on.
     * If the signal is not set after TimeOut seconds, WaitFor returns anyway.
     *
     * @Param Timeout, Timeout in seconds.
     * @Param ms, Timeout in ms.
     *
     * both Timeout,ms equal 0 means wait for one day.
     *
     */
    BOOL WaitFor( int TimeOut = 0, int ms = 0 );
    BOOL WaitForUS( int us );
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SInt
{
#if __cplusplus >= 201103L
private:
    std::atomic<int> FInt;
public:
    SInt( int InitVal = 0 ) : FInt( InitVal ) {}
    SInt( const SInt& rhs ) : FInt( rhs.FInt.load() ) {}
    SInt& operator =( const int Value ) { FInt.store( Value );          return *this; }
    SInt  operator ++( int )            { SInt old(*this); FInt.fetch_add( 1 ); return old; }
    SInt  operator --( int )            { SInt old(*this); FInt.fetch_sub( 1 ); return old; }
    SInt& operator +=( const int Value ){ FInt.fetch_add( Value );      return *this; }
    SInt& operator -=( const int Value ){ FInt.fetch_sub( Value );      return *this; }
    operator int() const { return FInt.load(); }
    int GetValue( void ) const { return FInt.load(); }
#else
private:
    PCriticalSection FCountCS;
    int FInt;
    void ChangeValue( int val )
    {
        FCountCS.Enter();
        FInt += val;
        FCountCS.Leave();
    }
public:
    SInt( int InitVal = 0 ) : FInt( InitVal ) {}
    SInt& operator =( const int Value )
    {
        FCountCS.Enter();
        FInt = Value;
        FCountCS.Leave();
        return *this;
    }
    SInt  operator ++( int ) { SInt old(*this); ChangeValue(  1 ); return old; }
    SInt  operator --( int ) { SInt old(*this); ChangeValue( -1 ); return old; }
    SInt& operator +=( const int Value ) { ChangeValue(  Value ); return *this; }
    SInt& operator -=( const int Value ) { ChangeValue( -Value ); return *this; }
    operator int() const { return FInt; }
    int GetValue( void ) { return FInt; }
#endif
};
//----------------------------------------------------------------------------------------------------------------------
#ifndef _WIN32
    const Int32	THREAD_PRIORITY_TIME_CRITICAL= -18;
    const Int32	THREAD_PRIORITY_HIGHEST      = -18;
    const Int32	THREAD_PRIORITY_ABOVE_NORMAL = -10;
    const Int32	THREAD_PRIORITY_NORMAL       =   0;
    const Int32	THREAD_PRIORITY_BELOW_NORMAL =   9;
    const Int32	THREAD_PRIORITY_LOWEST       =  17;
    const Int32	THREAD_PRIORITY_IDLE         =  17;
#endif
    
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
    rwNotLock      = 0,
    rwLockForRead  = 1,
    rwLockForWrite = 2
            
} RWLockTypeEnum;
#if defined(__LINUX) || defined(__AIX)
//----------------------------------------------------------------------------------------------------------------------
AnsiString GetLockMessageByCode(int LockResult);
#endif
//----------------------------------------------------------------------------------------------------------------------
class PReadWriteLock
{
private:
#ifdef _WIN32
    SRWLOCK FReadWriteLockStruct;
#else
    pthread_rwlock_t FReadWriteLockStruct;
#endif
    int        FInitialErrorNo;
    int        FLockErrorNo;
    AnsiString FErrorMessage;    
public:
    PReadWriteLock();
#ifndef _WIN32
    PReadWriteLock(int SharedAttribute);
#endif    
    ~PReadWriteLock();
    
    int LockForRead();
    int LockForWrite();
    int Lock(RWLockTypeEnum LockType);
#ifndef _WIN32
    int TryLockForRead();
    int TryLockForWrite();
    int TryLock(RWLockTypeEnum LockType);
    int TryAndLockForRead(const AnsiString& FuncName, const AnsiString& ObjName, void* LogPtr);
    int TryAndLockForWrite(const AnsiString& FuncName, const AnsiString& ObjName, void* LogPtr);
    int TryAndLock(RWLockTypeEnum LockType, const AnsiString& FuncName, const AnsiString& ObjName, void* LogPtr);
    int Unlock();
#endif
    int UnlockForRead();
    int UnlockForWrite();
    int Unlock(RWLockTypeEnum LockType);
};  //PReadWriteLock
//---------------------------------------------------------------------------
class PReadLockObject
{
private:
    PReadWriteLock& FCS;
public:    
    PReadLockObject( PReadWriteLock& CS ):FCS( CS ){ FCS.LockForRead();}    
    ~PReadLockObject( ){ FCS.UnlockForRead(); }
};
//---------------------------------------------------------------------------
class PWriteLockObject
{
private:
    PReadWriteLock& FCS;
public:    
    PWriteLockObject( PReadWriteLock& CS ):FCS( CS ){ FCS.LockForWrite();}    
    ~PWriteLockObject( ){ FCS.UnlockForWrite(); }
};
//---------------------------------------------------------------------------
void DoReadWriteLock(PReadWriteLock *LockPtr, RWLockTypeEnum LockType);
void DoReadWriteUnlock(PReadWriteLock *LockPtr, RWLockTypeEnum LockType);
//---------------------------------------------------------------------------
template<typename VariableType> 
VariableType GetVariableValue(const VariableType& Variable, PReadWriteLock *LockPtr, RWLockTypeEnum LockType)
{
    DoReadWriteLock(LockPtr, LockType);
    VariableType resultValue = Variable;
    DoReadWriteUnlock(LockPtr, LockType);
    return resultValue;
}  //GetVariableValue()
//---------------------------------------------------------------------------
template<typename VariableType> 
void SetVariableValue(const VariableType& NewValue, VariableType& Variable, PReadWriteLock *LockPtr, RWLockTypeEnum LockType)
{
    DoReadWriteLock(LockPtr, LockType);
    Variable = NewValue;
    DoReadWriteUnlock(LockPtr, LockType);
}  //SetVariableValue()

//---------------------------------------------------------------------------
const bool cstCreateLock = true;
const bool cstNotCreateLock = false;
//---------------------------------------------------------------------------
class CBasicRWLockObject
{
protected:
    PReadWriteLock *FRWLockPtr;

public:
    CBasicRWLockObject(bool CreateLock);
    virtual ~CBasicRWLockObject();

    bool HasLock();

#if defined(__LINUX) || defined(__AIX)
    virtual int TryLock(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    virtual int TryLockForRead();
    virtual int TryLockForWrite();
    virtual int Lock(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    virtual int Unlock(UFC::RWLockTypeEnum LockType);
    virtual int Unlock();
    virtual int LockForRead();
    virtual int UnLockForRead();
    virtual int LockForWrite();
    virtual int UnLockForWrite();
#else  //_WIN32
    virtual void Lock(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    virtual void Unlock(UFC::RWLockTypeEnum LockType);
    virtual void LockForRead();
    virtual void UnLockForRead();
    virtual void LockForWrite();
    virtual void UnLockForWrite();
#endif
    
    template<typename fieldType> fieldType GetFieldValue(const fieldType& MemberField, UFC::RWLockTypeEnum LockType)
    {
        Lock(LockType);
        fieldType resultValue = MemberField;
        Unlock(LockType);
        return resultValue;
    }  //GetFieldValue()

    template<typename fieldType> void SetFieldValue(const fieldType& NewValue, fieldType& MemberField, UFC::RWLockTypeEnum LockType)
    {
        Lock(LockType);
        MemberField = NewValue;
        Unlock(LockType);
    }  //SetFieldValue()
};  //CBasicRWLockObject

//------------------------------------------------------------------------------
class CMainKeyObject : public CBasicRWLockObject
{
protected:
    UFC::AnsiString FMainIndexKey;
    
public:
    CMainKeyObject(bool NeedLock);
    CMainKeyObject(bool NeedLock, const UFC::AnsiString MainIndexKey);
    
    virtual UFC::AnsiString GetMainIndexKey(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<UFC::AnsiString>(FMainIndexKey, LockType);};
    virtual void SetMainIndexKey(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<UFC::AnsiString>(NewValue, FMainIndexKey, LockType);};
    
    virtual void GenerateMainIndexKey(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {};
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<UFC::AnsiString>(FMainIndexKey, LockType);};
};  //CMainKeyObject
//------------------------------------------------------------------------------
class CFOMainKeyObjectLess  //Compare Function Object
{
public:
    CFOMainKeyObjectLess() {};
    ~CFOMainKeyObjectLess() {};

    bool operator()(CMainKeyObject* MainKeyObjPtr1, CMainKeyObject* MainKeyObjPtr2)
        {return MainKeyObjPtr1->GetMainIndexKey(UFC::rwLockForRead) < MainKeyObjPtr2->GetMainIndexKey(UFC::rwLockForRead);};
};  //CFOMainKeyObjectLess
//------------------------------------------------------------------------------
class CFOMainKeyObjectLessEqual  //Compare Function Object
{
public:
    CFOMainKeyObjectLessEqual() {};
    ~CFOMainKeyObjectLessEqual() {};

    bool operator()(CMainKeyObject* MainKeyObjPtr1, CMainKeyObject* MainKeyObjPtr2)
        {return MainKeyObjPtr1->GetMainIndexKey(UFC::rwLockForRead) <= MainKeyObjPtr2->GetMainIndexKey(UFC::rwLockForRead);};
};  //CFOMainKeyObjectLessEqual
//------------------------------------------------------------------------------
class CFOMainKeyObjectGreater  //Compare Function Object
{
public:
    CFOMainKeyObjectGreater() {};
    ~CFOMainKeyObjectGreater() {};

    bool operator()(CMainKeyObject* MainKeyObjPtr1, CMainKeyObject* MainKeyObjPtr2)
        {return MainKeyObjPtr1->GetMainIndexKey(UFC::rwLockForRead) > MainKeyObjPtr2->GetMainIndexKey(UFC::rwLockForRead);};
};  //CFOMainKeyObjectGreater
//------------------------------------------------------------------------------
class CFOMainKeyObjectGreaterEqual  //Compare Function Object
{
public:
    CFOMainKeyObjectGreaterEqual() {};
    ~CFOMainKeyObjectGreaterEqual() {};

    bool operator()(CMainKeyObject* MainKeyObjPtr1, CMainKeyObject* MainKeyObjPtr2)
        {return MainKeyObjPtr1->GetMainIndexKey(UFC::rwLockForRead) >= MainKeyObjPtr2->GetMainIndexKey(UFC::rwLockForRead);};
};  //CFOMainKeyObjectGreaterEqual
//------------------------------------------------------------------------------
class CFOMainKeyObjectEqual  //Compare Function Object
{
private:
    UFC::AnsiString FCompareKey;

public:
    CFOMainKeyObjectEqual(const UFC::AnsiString& CompareKey):FCompareKey(CompareKey) {};
    ~CFOMainKeyObjectEqual() {};

    bool operator()(CMainKeyObject* MainKeyObjPtr) {return MainKeyObjPtr->GetMainIndexKey(UFC::rwLockForRead) == FCompareKey;};
};  //CFOMainKeyObjectEqual

//------------------------------------------------------------------------------
class CMainKeyObjectL : public CBasicRWLockObject
{
protected:
    UFC::AnsiString FMainIndexKey;
    
public:
    CMainKeyObjectL(bool NeedLock);
    CMainKeyObjectL(bool NeedLock, const UFC::AnsiString MainIndexKey);
    
    virtual UFC::AnsiString GetMainIndexKey() {return FMainIndexKey;};
    virtual UFC::AnsiString GetMainIndexKey_L(UFC::RWLockTypeEnum LockType) {return GetFieldValue<UFC::AnsiString>(FMainIndexKey, LockType);};
    virtual void SetMainIndexKey(const UFC::AnsiString& NewValue) {FMainIndexKey = NewValue;};
    virtual void SetMainIndexKey_L(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType) {SetFieldValue<UFC::AnsiString>(NewValue, FMainIndexKey, LockType);};
    
    virtual void GenerateMainIndexKey() {};
    virtual void GenerateMainIndexKey_L(UFC::RWLockTypeEnum LockType);
    
    virtual UFC::AnsiString ToString() {return FMainIndexKey;};
    virtual UFC::AnsiString ToString_L(UFC::RWLockTypeEnum LockType);
};  //CMainKeyObjectL

//------------------------------------------------------------------------------
class CFOMainKeyObjectL_Less  //Compare Function Object
{
public:
    CFOMainKeyObjectL_Less() {};
    ~CFOMainKeyObjectL_Less() {};

    bool operator()(CMainKeyObjectL* MainKeyObjPtr1, CMainKeyObjectL* MainKeyObjPtr2)
        {return MainKeyObjPtr1->GetMainIndexKey() < MainKeyObjPtr2->GetMainIndexKey();};
};  //CFOMainKeyObjectL_Less
//------------------------------------------------------------------------------
class CFOMainKeyObjectL_LessL  //Compare Function Object
{
public:
    CFOMainKeyObjectL_LessL() {};
    ~CFOMainKeyObjectL_LessL() {};

    bool operator()(CMainKeyObjectL* MainKeyObjPtr1, CMainKeyObjectL* MainKeyObjPtr2)
        {return MainKeyObjPtr1->GetMainIndexKey_L(UFC::rwLockForRead) < MainKeyObjPtr2->GetMainIndexKey_L(UFC::rwLockForRead);};
};  //CFOMainKeyObjectL_LessL
//------------------------------------------------------------------------------
class CFOMainKeyObjectL_LessEqual  //Compare Function Object
{
public:
    CFOMainKeyObjectL_LessEqual() {};
    ~CFOMainKeyObjectL_LessEqual() {};

    bool operator()(CMainKeyObjectL* MainKeyObjPtr1, CMainKeyObjectL* MainKeyObjPtr2)
        {return MainKeyObjPtr1->GetMainIndexKey() <= MainKeyObjPtr2->GetMainIndexKey();};
};  //CFOMainKeyObjectL_LessEqual
//------------------------------------------------------------------------------
class CFOMainKeyObjectL_LessEqualL  //Compare Function Object
{
public:
    CFOMainKeyObjectL_LessEqualL() {};
    ~CFOMainKeyObjectL_LessEqualL() {};

    bool operator()(CMainKeyObjectL* MainKeyObjPtr1, CMainKeyObjectL* MainKeyObjPtr2)
        {return MainKeyObjPtr1->GetMainIndexKey_L(UFC::rwLockForRead) <= MainKeyObjPtr2->GetMainIndexKey_L(UFC::rwLockForRead);};
};  //CFOMainKeyObjectL_LessEqual
//------------------------------------------------------------------------------
class CFOMainKeyObjectL_Greater  //Compare Function Object
{
public:
    CFOMainKeyObjectL_Greater() {};
    ~CFOMainKeyObjectL_Greater() {};

    bool operator()(CMainKeyObjectL* MainKeyObjPtr1, CMainKeyObjectL* MainKeyObjPtr2)
        {return MainKeyObjPtr1->GetMainIndexKey() > MainKeyObjPtr2->GetMainIndexKey();};
};  //CFOMainKeyObjectL_Greater
//------------------------------------------------------------------------------
class CFOMainKeyObjectL_GreaterL  //Compare Function Object
{
public:
    CFOMainKeyObjectL_GreaterL() {};
    ~CFOMainKeyObjectL_GreaterL() {};

    bool operator()(CMainKeyObjectL* MainKeyObjPtr1, CMainKeyObjectL* MainKeyObjPtr2)
        {return MainKeyObjPtr1->GetMainIndexKey_L(UFC::rwLockForRead) > MainKeyObjPtr2->GetMainIndexKey_L(UFC::rwLockForRead);};
};  //CFOMainKeyObjectL_GreaterL
//------------------------------------------------------------------------------
class CFOMainKeyObjectL_GreaterEqual  //Compare Function Object
{
public:
    CFOMainKeyObjectL_GreaterEqual() {};
    ~CFOMainKeyObjectL_GreaterEqual() {};

    bool operator()(CMainKeyObjectL* MainKeyObjPtr1, CMainKeyObjectL* MainKeyObjPtr2)
        {return MainKeyObjPtr1->GetMainIndexKey() >= MainKeyObjPtr2->GetMainIndexKey();};
};  //CFOMainKeyObjectL_GreaterEqual
//------------------------------------------------------------------------------
class CFOMainKeyObjectL_GreaterEqualL  //Compare Function Object
{
public:
    CFOMainKeyObjectL_GreaterEqualL() {};
    ~CFOMainKeyObjectL_GreaterEqualL() {};

    bool operator()(CMainKeyObjectL* MainKeyObjPtr1, CMainKeyObjectL* MainKeyObjPtr2)
        {return MainKeyObjPtr1->GetMainIndexKey_L(UFC::rwLockForRead) >= MainKeyObjPtr2->GetMainIndexKey_L(UFC::rwLockForRead);};
};  //CFOMainKeyObjectL_GreaterEqualL
//------------------------------------------------------------------------------
class CFOMainKeyObjectL_Equal  //Compare Function Object
{
private:
    UFC::AnsiString FCompareKey;

public:
    CFOMainKeyObjectL_Equal(const UFC::AnsiString& CompareKey):FCompareKey(CompareKey) {};
    ~CFOMainKeyObjectL_Equal() {};

    bool operator()(CMainKeyObjectL* MainKeyObjPtr) {return MainKeyObjPtr->GetMainIndexKey() == FCompareKey;};
};  //CFOMainKeyObjectL_Equal
//------------------------------------------------------------------------------
class CFOMainKeyObjectL_EqualL  //Compare Function Object
{
private:
    UFC::AnsiString FCompareKey;

public:
    CFOMainKeyObjectL_EqualL(const UFC::AnsiString& CompareKey):FCompareKey(CompareKey) {};
    ~CFOMainKeyObjectL_EqualL() {};

    bool operator()(CMainKeyObjectL* MainKeyObjPtr) {return MainKeyObjPtr->GetMainIndexKey_L(UFC::rwLockForRead) == FCompareKey;};
};  //CFOMainKeyObjectL_EqualL

//---------------------------------------------------------------------------
class PConditionMutex
{
private:
#ifdef _WIN32
    CRITICAL_SECTION   FMutex;
    CONDITION_VARIABLE FCondition;
    DWORD              FOwnerThreadId;
#else
    pthread_mutex_t FMutex;
    pthread_cond_t  FCondition;
    pthread_t       FOwnerThreadId;
#endif
    bool FIsLocked;
    
    // Declared, but not implemented to prevent default behavior generated by compiler
    PConditionMutex(const PConditionMutex &);
    PConditionMutex & operator=(const PConditionMutex &);
    
public:
    PConditionMutex();
    virtual ~PConditionMutex();

    int TryLock();
#ifdef _WIN32
    void Lock();
    void Unlock();
#else  //defined(__LINUX) || defined(__AIX)
    int Lock();
    int TryAndLock(const AnsiString& FuncName, const AnsiString& ObjName, void* LogPtr);
    int Unlock();
#endif
    
    void Signal();
    void Broadcast();
    int Wait();
    int Wait(int WaitingMilliseconds);
    bool IsLocked() {return FIsLocked;};
#ifdef _WIN32
    DWORD GetOwnerThreadId() {return FOwnerThreadId;};
#else
    pthread_t GetOwnerThreadId() {return FOwnerThreadId;};
#endif
    bool IsLockedByMe();
};  //CConditionMutex

//----------------------------------------------------------------------------------------------------------------------
class ThreadListener
{
public:
	virtual void OnTerminate( PThread* TerminateThread  ) = 0;
        virtual ~ThreadListener( void ){}
};
//----------------------------------------------------------------------------------------------------------------------
/**
 * The <code>PThread</code> PThread is an abstract class that enables creation of separate threads of execution in an application.
 */
class PThread
{
private:
    static PCriticalSection GThreadCS;
	static void* GlobalExecute( void * );
private:
	ThreadListener *FListener;
	BOOL            FFreeOnTerminate;
	BOOL            FTerminated;
	BOOL            FStopped;
	Int32           FPriority;
	PEvent          FTerminateEvent;        
#ifdef _WIN32
        HANDLE          FThreadID;
#else
	pthread_t       FThreadID;
#endif  
	char            FName[64];
	char            FFileName[ PATH_MAX ];
public:
	/**
	 * Return the thread handle.
	 */
#ifdef _WIN32
	HANDLE  GetThreadID( void ){ return FThreadID; };
#else
	int  GetThreadID( void ){ return (int)FThreadID; };
#endif
	/**
	 * Creates an instance of a thread object.
	 *
	 * @param Listener Assign the OnTerminate event listener object.
	 * @param FreeOnTerminate Determines whether the thread object is automatically destroyed when the thread terminates.
	 */
	PThread( ThreadListener *Listener, BOOL FreeOnTerminate = TRUE, Int32 Priority = THREAD_PRIORITY_NORMAL );
	/**
	 * Destroys the thread object and releases the memory allocated to it.
	 */
	virtual ~PThread();
	/**
         * Waits for the thread to terminate.
         * @return TRUE for thread to terminated.
         *         FALSE for timeout.
         */
	BOOL WaitFor( int Sec = 0 );
        /**
	 * Starts the execution of a thread.
	 */
	void Start( UInt32 StackSize = 128*1024 );
	/**
	 * Signals the thread to terminate by setting the FTerminated to true.
	 */
	void Terminate( void );
	/**
	 * Indicates whether the thread has been asked to terminate.
	 */
	BOOL IsTerminated( void ) { return FTerminated; }
	/**
	 * Provides a pure virtual method to contain the code which executes when the thread is run.
	 *
	 * Override Execute and insert the code that should be executed when the thread runs.
	 * Execute is responsible for checking the value of the Terminated property to determine if the thread needs to exit.
	 */
	virtual void Execute( void ) = 0;
        /**
         * Call sched_yield(), let this thread release CPU for a while.
         */
	static  void PThread_Yield( void );
        static  bool ThreadBindingCores( const UFC::AnsiString& Name, UFC::PInt32List& Cores );
        /**
         *  Limit this thread  to run only on the processors represented
         *  in CPUSET.  
         */
        void PThread_setaffinity( int CPUID );
};

//-----------------------------------------------------------------------------------------
}	/* namespace UFC */
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------
