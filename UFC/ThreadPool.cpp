#include "ThreadPool.h"
#include "NetUtility.h"
#include "BufferedLog.h"
//-----------------------------------------------------------------------------------------
namespace UFC
{
//-----------------------------------------------------------------------------------------
// class PTaskThread
// A thread to do the given task
// This thread should work with PThreadPool object.
//-----------------------------------------------------------------------------------------
PTaskThread::PTaskThread( PThreadPool* Owner )
:PThread( NULL, FALSE ),
 FPool( Owner ),
 FTask( NULL ),
 FParam( NULL )
{
    Start();
}
//-----------------------------------------------------------------------------------------
PTaskThread::~PTaskThread( void )
{
    FTask  = NULL;
    FParam = NULL;
    Terminate();
    FWaitEvent.SetEvent();
    WaitFor();
}
//-----------------------------------------------------------------------------------------
// Activate this thread
// This function will set an event to wakeup this thread.
//-----------------------------------------------------------------------------------------
void PTaskThread::Activate( PTaskObject* Task, void* Param )
{
    PLockObject Lock( FCS );
    FTask  = Task;   ///< Keep the task object address.
    FParam = Param;  ///< Keep the parameters memory address of this job
    FWaitEvent.SetEvent(); ///< Set the event to wake up this task thread.
}
//-----------------------------------------------------------------------------------------
void PTaskThread::Execute( void )
{
    while( IsTerminated() == FALSE )
    {
        FWaitEvent.WaitFor( );///< Waiting for the wakeup event.
        if( FTask != NULL && FParam != NULL )
        {
            PLockObject Lock( FCS );
            Int32 Result;
            try
            {
                Result = FTask->DoTask( FParam );
            }
            catch(...)
            {
                Result = 0;///< Failed.
            }
            FPool->TaskFinished( this, FTask, Result );///< Tell the ThreadPool job finished.
            FTask  = NULL;
            FParam = NULL;
        }
    };
}
//-----------------------------------------------------------------------------------------
// class PThreadPool
//-----------------------------------------------------------------------------------------
PThreadPool::PThreadPool( Int32 ThreadSize, const UFC::AnsiString& Rule  )
:PThread( NULL, FALSE )
{
    PTaskThread*       NewThread;    
    UFC::PInt32List    Cores;
    bool               NeedBind;
   

    NeedBind = UFC::PThread::ThreadBindingCores( Rule, Cores );                

    for( int i = 0; i < ThreadSize; i++ )
    {
         NewThread = new PTaskThread( this ); ///< Create threads.
         FThreadList.Add( NewThread );        ///< Add to the threads list.
         FIdleThreadList.Add( NewThread );    ///< Add to the idle threads list.
         if( NeedBind == true )
         {
             int UseCore = Cores[ i % Cores.ItemCount() ];                
             
             NewThread->PThread_setaffinity( UseCore );
         }
    }
    Start();                                  ///< Start the ThreadPool manager thread.
}
//-----------------------------------------------------------------------------------------
PThreadPool::~PThreadPool()
{
    for( int i = FThreadList.ItemCount()-1; i >= 0; i-- )
         delete FThreadList.GetItem( i );   ///< Delete all threads in pool.
    Terminate();                            ///< Terminate the ThreadPool thread.
    FTaskFinishedEvent.SetEvent();
    WaitFor();                              ///< Wait for the ThreadPool thread terminated.
}
//-----------------------------------------------------------------------------------------
void PThreadPool::TaskFinished( PTaskThread* Task,PTaskObject* /*FinishedTask*/, Int32 /*ReturnValue*/ )
{
    PLockObject LockObj( FIdleThreadListCS ); ///< Enter the Idle thread list CS.

    FIdleThreadList.Add( Task );   ///< Add this thread to the idle list.
    FTaskFinishedEvent.SetEvent(); ///< Set the task finished event.
                                   ///  let the manager known that a thread is available now.
}
//-----------------------------------------------------------------------------------------
void PThreadPool::AddToQueue( PTaskObject* Task, void* Param )
{
    PLockObject LockObj( FJobQueueCS );
#ifdef _DEBUG
    if( FindJob( Task,Param ) != -1 )///< Find the job!
    {
        UFC::BufferedLog::Printf("##################################");
        UFC::BufferedLog::Printf("# Error!!! Job already exists!!! #");
        UFC::BufferedLog::Printf("##################################");
    }
#endif
    FJobQueue.Add( new QueuedJob( Task, Param ) ); ///< Add to job queue.
}
//-----------------------------------------------------------------------------------------
QueuedJob* PThreadPool::GetFromQueue( void )
{
    PLockObject LockObj( FJobQueueCS );

    QueuedJob*  Job = (QueuedJob*)FJobQueue.GetItem( 0 ); ///< Get the first job in the queue.(FIFO)
    FJobQueue.Delete( 0 );                                ///< Dequeue it.
    return Job;
}
//-----------------------------------------------------------------------------------------
int  PThreadPool::FindJob( PTaskObject* Task, void* Param )
{
    for( register int i = FJobQueue.ItemCount()-1; i >= 0; i-- )
    {
         if( FJobQueue.GetItem( i )->Same( Task, Param ) == TRUE )///< Find the job!
             return i;
    }
    return -1; ///< Not found!!
}
//-----------------------------------------------------------------------------------------
// Public function.
// Cancel the job in queue.
//-----------------------------------------------------------------------------------------
void PThreadPool::CancelJob( PTaskObject* Task, void* Param )
{
    int Index;
    PLockObject LockObj( FJobQueueCS );

    if( (Index = FindJob( Task,Param )) != -1 ) ///< Find the job!
        FJobQueue.Delete( Index );              ///< Remove it from queue.
}
//-----------------------------------------------------------------------------------------
// Public function.
// Call by MultiplexingServer TriggerDataArrived function only.
// means that only one thread can call this function at the same time.
//-----------------------------------------------------------------------------------------
BOOL PThreadPool::ExecuteTask( PTaskObject* Task, void* Param )
{
    PTaskThread* TaskThread;
    PLockObject  LockObj( FIdleThreadListCS ); ///< Lock the idle thread list.

    if( FIdleThreadList.ItemCount() > 0 && ///< Is there any idle thread ? Yes!
        FJobQueue.ItemCount() == 0 )       ///< Is there any job in queue ? No!
    {
        TaskThread = (PTaskThread*)FIdleThreadList.GetItem( 0 ); ///< Get a idle thread from idle threads list.
        FIdleThreadList.Delete( 0 );                             ///< remove the first thread from list.
        TaskThread->Activate( Task, Param );                     ///< Activate this thread doing this job.
        return TRUE;
    }
    else                           ///< No! all threads are busy now!
    {
        AddToQueue( Task, Param ); ///< No more thread...add a job object to job queue.
        //printf("### No more thread, add to job queue, job inqueue:[%d] ###\n", FJobQueue.ItemCount());
        return FALSE;
    }
}
//-----------------------------------------------------------------------------------------
// Job queue manager thread.
// Works only when jobs in queue.
//-----------------------------------------------------------------------------------------
void PThreadPool::Execute( void )
{
    PTaskThread* TaskThread;
    QueuedJob*   Job;

    while( IsTerminated() == FALSE )
    {
        FTaskFinishedEvent.WaitFor();   ///< Wait for task finished event.
        if( FJobQueue.ItemCount() > 0 &&      ///< Is there any job in queued ? Yes!
            FIdleThreadList.ItemCount() > 0 ) ///< Is there any idled thread in pool? Yes!
        {
            PLockObject LockObj( FIdleThreadListCS ); ///< Lock the thread list.

            if( ( TaskThread = (PTaskThread*)FIdleThreadList.GetItem( 0 )) != NULL ) ///< Get a idle thread from idle threads list.
            {
               if( (Job = GetFromQueue()) != NULL ) ///< Dequeue a job object.
               {    
                   FIdleThreadList.Delete( 0 );                   ///< remove the first thread from idle list.(means this thread will be busy!)
                   TaskThread->Activate( Job->Task, Job->Param ); ///< Activate this thread.
                   delete Job;                                    ///< Delete a job object.
               }
            }
        }
    };
}
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------

