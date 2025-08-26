//-----------------------------------------------------------------------------------------
// This is a part of the UNIX Foundation Classes Library for UNIX platforms.
// Copyright (c) 2003 MobileDigital Development Inc.
// All rights reserved.
//
// Description: Thread Pool Class.
// Author: Simon Chang
//
// This source code is only intended as a supplement to the
// Object Class Library for UNIX platforms Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// UNIX Foundation Classes Library for UNIX platforms product.
//-----------------------------------------------------------------------------------------
#ifndef __UFC_THREADPOOL_H
#define __UFC_THREADPOOL_H
//-----------------------------------------------------------------------------------------
#include "PThread.h"
#include "List.h"
//-----------------------------------------------------------------------------------------
namespace UFC
{
//-----------------------------------------------------------------------------------------
class PThreadPool;
//-----------------------------------------------------------------------------------------
// Abstract class for a task object.
//
// Any object want to run as a task need to inhert this interface.
//-----------------------------------------------------------------------------------------
class PTaskObject
{
public:
	/**
	 * This function will be called when the program activate a PTaskThread in the pool.
	 */
    virtual Int32 DoTask( void* Param ) = 0;
    virtual ~PTaskObject( void ){}
};
//-----------------------------------------------------------------------------------------
// Task thread class.
//
// This object work only with PThreadPool object. Internal use only!
//-----------------------------------------------------------------------------------------
class PTaskThread : public PThread
{
private:
    PEvent       FWaitEvent;
    PThreadPool* FPool;
    PTaskObject* FTask;
    void*        FParam;
    PCriticalSection FCS;
private:
	virtual void Execute( void );
public:
    PTaskThread( PThreadPool* Owner );
    ~PTaskThread( void );
    void Activate( PTaskObject* Task, void* Param );
};
//-----------------------------------------------------------------------------------------
class QueuedJob
{
public:
    PTaskObject* Task;
    void*        Param;
public:
    BOOL Same( PTaskObject* T, void* P ){ return ((T == Task )&&(P==Param));}
    QueuedJob(PTaskObject* T, void* P ):Task(T),Param(P){}
};
//-----------------------------------------------------------------------------------------
// Thread pool class
//-----------------------------------------------------------------------------------------
class PThreadPool : public PThread
{
friend class PTaskThread;
private:

private:
    PtrList<PTaskThread>  FThreadList;     ///< All threads list
    PtrList<PTaskThread>  FIdleThreadList; ///< Idle threads list
    PtrList<QueuedJob>    FJobQueue;
    PCriticalSection      FIdleThreadListCS;
    PCriticalSection      FJobQueueCS;
    PEvent                FTaskFinishedEvent;
private:
    void         AddToQueue( PTaskObject* Task, void* Param );
    QueuedJob*   GetFromQueue( void );
    int          FindJob( PTaskObject* Task, void* Param );
    virtual void Execute( void );
    virtual void TaskFinished( PTaskThread* Task, PTaskObject* FinishedTask, Int32 ReturnValue );
public:
    PThreadPool( Int32 ThreadSize, const UFC::AnsiString& Rule = "Def" );
    ~PThreadPool( );
    BOOL  ExecuteTask( PTaskObject* Task, void* Param );
    Int32 AvailableThread( void ){ return FIdleThreadList.ItemCount(); }
    void  CancelJob( PTaskObject* Task, void* Param );
};
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------
