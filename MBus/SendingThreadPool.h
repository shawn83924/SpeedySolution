//---------------------------------------------------------------------------
#ifndef MBUS_SEND_THREAD_POOL_H
#define MBUS_SEND_THREAD_POOL_H
//---------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "MigoHeader.h"
//---------------------------------------------------------------------------
class SendingThreadPool;
//---------------------------------------------------------------------------
class SendingThreadPoolListener
{
 public:
	virtual void OnError( UFC::PClientSocket* SendSocket, UFC::Exception* e ) = 0;
	virtual ~SendingThreadPoolListener( void ) {}
};
//---------------------------------------------------------------------------
class JobQueue
{
private:
    Int32                              FMaxSize;
    UFC::PCriticalSection              FSetCS;        
    UFC::PtrQueue<UFC::PClientSocket>  FQueue;
    UFC::PIntHashedSet                 FSet;            
private:
    void DeleteJobInQueue( UFC::PClientSocket* DelJob );
public:
	/*
	 *  Constructor, create a JobQueue which max size = Size  
	 */
    JobQueue( int Size );
	/*
	 *  Add a Job to Queue
	 */
    BOOL  AddJob( UFC::PClientSocket* AddJob );
	/*
	 *  Remove a Job from Queue
	 */
    BOOL  RemoveJob( UFC::PClientSocket* DelJob );
    UFC::PClientSocket* GetJob( BOOL Remove = FALSE );
    void Done( UFC::PClientSocket* OK );
};
//---------------------------------------------------------------------------
class SendingThread : public UFC::PThread
{
private:
	SendingThreadPool* FParent;
private:
	void WriteDataToSocket( UFC::PClientSocket* WriteSocket );
	virtual void Execute( void );
public:
	SendingThread( SendingThreadPool* Parent );
};
//---------------------------------------------------------------------------
class SendingThreadPool : public UFC::PThread
{
friend class SendingThread;
private:
    SendingThreadPoolListener*         FListener;
    Int32                              FThreadCount;
    Int32                              FMaxJobs;
    JobQueue                           FJobQueue; 
    JobQueue                           FRetryJobQueue;
    UFC::AnsiString                    FBindRule;
private:
    UFC::PClientSocket* GetJob( void );
    void FinishJob( UFC::PClientSocket* );
    void RetryJob( UFC::PClientSocket* );
    void TriggerOnError(  UFC::PClientSocket* SendSocket, UFC::Exception* e  );
    virtual void Execute( void );
public:
    /**
     * Constructor a thread pool which can handle up to "MaxJob" jobs with "ThreadNumber" threads.
     */
    SendingThreadPool( SendingThreadPoolListener* Listener, int ThreadNumber, int MaxJob = 32, const UFC::AnsiString& Rule = "SendPool" );
    /**
     *  Start running the thread pool.
     */
    void Run( void );
    /**
      * Add a Job to the thread pool. (PClientSocket object with un-send data)
      */
    void AddJob( UFC::PClientSocket* SendSocket );
    /**
      * Remove a Job from the thread pool.
      */
    void RemoveJob( UFC::PClientSocket* SendSocket );
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
