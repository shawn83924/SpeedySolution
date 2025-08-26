//-----------------------------------------------------------------------------------------------
#include "SendingThreadPool.h"
//-----------------------------------------------------------------------------------------------
// Class  JobQueue
//-----------------------------------------------------------------------------------------------
// A multi-thread safe JobQueue class with "Busy" mechanism.
//-----------------------------------------------------------------------------------------------
JobQueue::JobQueue( int Size )
:FMaxSize( Size )
,FQueue( Size )
{
}
//-----------------------------------------------------------------------------------------------
//
// Add a job to the queue.
//
//-----------------------------------------------------------------------------------------------
BOOL JobQueue::AddJob( UFC::PClientSocket* NewJob )
{
    UFC::PLockObject Lock( FSetCS ); ///< Enter critical section

    if( !FSet.Exists( NewJob->GetHandle() )) ///< Is this object NOT in Job queue?
    {
        if( FQueue.InqueueNoWait( NewJob ) ) ///< Add a job to queue.
        {
            FSet.Add( NewJob->GetHandle() ); ///< Add to Job socket FD set.
            return TRUE;
        }
    }
    return FALSE;
}
//-----------------------------------------------------------------------------------------------
//
// Remove the given job from the queue.
//
//-----------------------------------------------------------------------------------------------
BOOL JobQueue::RemoveJob( UFC::PClientSocket* DelJob )
{
    UFC::PLockObject Lock( FSetCS );

    if( FSet.Exists( DelJob->GetHandle() )) ///< Is this object IN Job queue?
    {
        FSet.Delete( DelJob->GetHandle() ); ///< Remove Job from socket FD set.
        DeleteJobInQueue( DelJob );         ///< Remove the given job from job queue
        return TRUE;
    }
    return FALSE;
}
//-----------------------------------------------------------------------------------------------
// private function.
//
// only use by function RemoveJob.
// Function RemoveJob will enter critical section before call this function. no lock needed.
//-----------------------------------------------------------------------------------------------
void JobQueue::DeleteJobInQueue( UFC::PClientSocket* DelJob )
{
    UFC::PtrQueue<UFC::PClientSocket>  TmpQueue( FMaxSize ); ///< A temp queue
    UFC::PClientSocket* Job;

    while( (Job = FQueue.DequeueNoWait()) != NULL )  ///< Dequeue all job from queue
    {
        if( Job != DelJob )                ///< Is the given job object ?
            TmpQueue.InqueueNoWait( Job ); ///< No...Add to temp queue
    }
    while( (Job = TmpQueue.DequeueNoWait()) != NULL ) ///< Dequeue all job from temp queue
    {
        FQueue.InqueueNoWait( Job ); ///< add back to queue
    }
}
//-----------------------------------------------------------------------------------------------
//
// Get a job from the queue
// Remove: TURE  -> Get the job object and mark the job was finished.
//         FALSE -> Get the job object and mark the job is busy now. (You can't add this job to queue until it marked as finished.)
//
//-----------------------------------------------------------------------------------------------
UFC::PClientSocket* JobQueue::GetJob( BOOL Remove )
{
    UFC::PClientSocket* Job = FQueue.Dequeue( 1 ); ///< Dequeue a job from Job queue
    if( Job!= NULL )
    {
        UFC::PLockObject    Lock( FSetCS );

        if( FSet.Exists( Job->GetHandle() ) ) ///< Is the socket FD in set ?
        {
            if( Remove == TRUE ) ///< The job was finished?
                FSet.Delete( Job->GetHandle() ); ///< Yes! Remove it from busy set.
            return Job;
        }
    }
    return NULL;
}
//-----------------------------------------------------------------------------------------------
//
// Finished a job ( mark the job was finished. )
//
//-----------------------------------------------------------------------------------------------
void JobQueue::Done( UFC::PClientSocket* OK )
{
    UFC::PLockObject    Lock( FSetCS );

    FSet.Delete( OK->GetHandle() );
}
//-----------------------------------------------------------------------------------------------
// Class  SendingThread
//-----------------------------------------------------------------------------------------------
// Inherit from PThread
// The thread object work for thread pool.( class  SendingThreadPool)
//-----------------------------------------------------------------------------------------------
SendingThread::SendingThread( SendingThreadPool* Parent )
:UFC::PThread( NULL, FALSE )
,FParent( Parent )
{
    Start();
}
//-----------------------------------------------------------------------------------------------
//
// Serialize the data to the socket.
//
//-----------------------------------------------------------------------------------------------
void SendingThread::WriteDataToSocket( UFC::PClientSocket* WriteSocket )
{
    Int32 Result;

    while( TRUE )///< Serialize data to socket until all data sent or error occur
    {
        try
        {
            if( WriteSocket->GetUserData() == -1 )
                return;
            ///< Call "ProcessQueue" to write data to Socket.
            if( (Result = WriteSocket->ProcessQueue()) == 0 )///< Send all data out! 
            {
                UFC::PLockObject Lock( WriteSocket->IOLock ); ///< Lock Socket IOLock to prevent write data to queue.

                if( WriteSocket->IsQueueEmpty() == TRUE ) ///< any data in queue ?
                {
                    FParent->FinishJob( WriteSocket ); ///< Job finished.
                    return;
                }
                else          ///< Data is coming during sending out data.  
                    continue; ///< Still have data...Continue....
            }
            else if( Result == -1 ) ///< Can't write data now, try latter...
            {
                if( WriteSocket->GetUserData( ) != -1 )
                    FParent->RetryJob( WriteSocket ); ///< Job not finished. tell thread pool try it later.
                return;
            }            
            else
                continue; ///< Write data ok!...Continue....            
        }
        catch( UFC::SocketException& e) ///< Error occur
        {
            UFC::BufferedLog::Printf(" Write data to FD:%d failed.", WriteSocket->GetHandle());
            FParent->FinishJob( WriteSocket ); ///< Job finished.
            UFC::BufferedLog::Printf(" Stop Sending thread and trigger error for FD:%d.", WriteSocket->GetHandle());
            FParent->TriggerOnError( WriteSocket, &e );
            return;
        }
    };
}
//-----------------------------------------------------------------------------------------------
//
// Implement the PThread virtual function Execute.
// It's the thread function.
//
//-----------------------------------------------------------------------------------------------
void SendingThread::Execute( void )
{
    UFC::PClientSocket* WriteSocket;

    while( TRUE )
    {
        if( ( WriteSocket = FParent->GetJob()) != NULL ) ///< Get a job from queue
              WriteDataToSocket( WriteSocket );          ///< Write data to socket
    }
}
//-----------------------------------------------------------------------------------------------
// Class SendingThreadPool
//-----------------------------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------------------------
SendingThreadPool::SendingThreadPool( SendingThreadPoolListener* Listener, int ThreadNumber, int MaxJob, const UFC::AnsiString& Rule )
:UFC::PThread( NULL, FALSE )
,FListener( Listener)
,FThreadCount( ThreadNumber )
,FMaxJobs( MaxJob )
,FJobQueue( FMaxJobs )
,FRetryJobQueue(  FMaxJobs )
,FBindRule( Rule )
{
    Start();
}
//-----------------------------------------------------------------------------------------------
// Add a job to job queue
//-----------------------------------------------------------------------------------------------
void SendingThreadPool::AddJob( UFC::PClientSocket* SendSocket )
{
    FJobQueue.AddJob( SendSocket );
}
//-----------------------------------------------------------------------------------------------
// Remove a job from job queue
//-----------------------------------------------------------------------------------------------
void SendingThreadPool::RemoveJob( UFC::PClientSocket* SendSocket )
{
    SendSocket->SetUserData( -1 );          ///< Set user data to -1 to mark this socket had been delete.
    FRetryJobQueue.RemoveJob( SendSocket ); ///< Remove it from retry job queue.
    FJobQueue.RemoveJob( SendSocket );      ///< Remove it from job queue.
}
//-----------------------------------------------------------------------------------------------
// Run function
// Call this function to start the SendingThreadPool
//-----------------------------------------------------------------------------------------------
void SendingThreadPool::Run( void )
{
    SendingThread*   JobThread;
    UFC::PInt32List  Cores;
    bool             NeedBind;   

    NeedBind = UFC::PThread::ThreadBindingCores( FBindRule, Cores );                    
    for( int i = 0; i < FThreadCount; i ++ )
    {
         JobThread = new SendingThread( this );         ///< Create sending threads.
         if( NeedBind == TRUE )
         {
             int UseCore = Cores[ i % Cores.ItemCount() ];                
             JobThread->PThread_setaffinity(  UseCore );
         }
    }
}
//-----------------------------------------------------------------------------------------------
//
// Private member functions.
//
//-----------------------------------------------------------------------------------------------
// Get a job from job queue
//-----------------------------------------------------------------------------------------------
UFC::PClientSocket* SendingThreadPool::GetJob( void )
{
    return FJobQueue.GetJob();
}
//-----------------------------------------------------------------------------------------------
//  Job done
//-----------------------------------------------------------------------------------------------
void SendingThreadPool::FinishJob( UFC::PClientSocket* JobOk )
{
    FJobQueue.Done( JobOk );
}
//-----------------------------------------------------------------------------------------------
// Retry a job
//
// Remove it from job queue, and add it to retry job queue
//-----------------------------------------------------------------------------------------------
void SendingThreadPool::RetryJob( UFC::PClientSocket* Job )
{
    FJobQueue.Done( Job );
    if( FRetryJobQueue.AddJob( Job ) == TRUE )
        UFC::BufferedLog::Printf(" Add retry job to job queue FD:%d.", Job->GetHandle());
}
//-----------------------------------------------------------------------------------------------
// Call this function to trigger the error event to the SendingThreadPoolListener
//-----------------------------------------------------------------------------------------------
void SendingThreadPool::TriggerOnError(  UFC::PClientSocket* SendSocket, UFC::Exception* e  )
{
    FJobQueue.RemoveJob( SendSocket );      ///< Remove it from job queue.
    FRetryJobQueue.RemoveJob( SendSocket ); ///< Remove it from retry job queue.
    if( FListener != NULL )
        FListener->OnError( SendSocket, e );///< Trigger OnError event.
}
//-----------------------------------------------------------------------------------------------
// Thread function
// Get retry job (timeout 1sec), and add it to the job queue
//-----------------------------------------------------------------------------------------------
void SendingThreadPool::Execute( void )
{
    UFC::PClientSocket* RetryJob;

    while( TRUE )
    {
        if( (RetryJob = FRetryJobQueue.GetJob( TRUE )) != NULL ) ///< Get a job from retry job queue
            AddJob( RetryJob );                                  ///< Add it to job queue again!
    }
}
//-----------------------------------------------------------------------------------------------


