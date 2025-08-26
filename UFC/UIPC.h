/******************************************************************************
 * Module   : UIPC.h
 * Author   : Allen Lee
 * Modified : 2005/4/25
 * Purpose  : Unix IPC classes inclue Message queue , Shared memory. 
 ******************************************************************************/
//------------------------------------------------------------------------------------------------------------------------
#ifndef __UIPC_H
#define __UIPC_H
//------------------------------------------------------------------------------------------------------------------------
#ifndef _WIN32
//------------------------------------------------------------------------------------------------------------------------
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#ifdef _AIX433
	#include <strings.h>
#endif
#include <vector>
#include <errno.h>
#include "UFCType.h"
#include "PThread.h"
#include "AnsiString.h"
//------------------------------------------------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------------------------------------------------
// class UMessageQueue
//
//  Exceptions :
//    -
//
//  Description :
//    Provide the facility for Unix message queue manipulation.
//    Be careful below-
//    1. IsRefresh set true with each GetXXX methods is mean that, to call msgctl() system call to
//       retrieve status from queue each time when you call the GetXXX and IsRefresh is true.
//       Otherwise, just get current data of this->FMsgStatus without retrieve status from queue.
//       you can call RefreshStatus() method, then call GetXXX without IsRefresh.
//       you can also call GetStatus() method, to get current this->FMsgStatus at a time.
//    2. When IsNoWait set true with InQueue() and DeQueue() methods, if Exception occurs, you should
//       check EAGAIN and ENOMSG error number to commit state by UIPCException.GetErrno() method.
//    3. Function:DeQueue() returns the number of bytes actually placed into mtext. 
//       On failure, DeQueue() returns a value of -1.
//
//  Refernece :
//	  User message buffer template definition for InQueue and DeQueue method.
//    struct msgbuf {
//        long    mtype;     /* message type */
//        char    mtext[N];  /* message text */
//    };
//
    
    
#define IPCSMQ_DEFAULTSIZE 1024    
//------------------------------------------------------------------------------------------------------------------------
class UMessageQueue
{
private:
	BOOL	FIsAttach;
	int		FQueueID;
	key_t	FMsgKey;
	struct msqid_ds		FMsgStatus;

public:	// Error no & message member
	int		LastErrNo;
	char	LastErrStr[256];

private:
	void	SetLastError( void );
	void	SetLastError( int ErrNo, const char *ErrStr );

private:
	void	InitializeMember(void);

public:
	BOOL	AttachQueue(key_t Key, int Permission, BOOL IsCreate = FALSE, BOOL IsFailIfExist = FALSE);
	// ex: Obj.AttachQueue(0x88887777, 0666, TRUE, FALSE);
	BOOL	ClearQueue(long type);
	BOOL	RemoveQueue(void);
	BOOL	RefreshStatus(void);
	BOOL	InQueue(const void *msg_buffer, int nbytes, BOOL IsNoWait = FALSE);
	int		DeQueue(void *msg_buffer, int nbytes, long type, BOOL IsNoWait = FALSE, BOOL IsNoTruncatErr = FALSE);
public:
	BOOL	SetQueueBytes(unsigned long nbytes);
public:
	BOOL	IsAttach(){ return FIsAttach; }
	int		GetQueueID(){ return FQueueID; }
	int		GetQ_Uid(BOOL IsRefresh = FALSE);				// owner's effective user id
	int		GetQ_Gid(BOOL IsRefresh = FALSE);				// owner's effective group id
	int		GetQ_Cuid(BOOL IsRefresh = FALSE);				// creator's effective user id
	int		GetQ_Cgid(BOOL IsRefresh = FALSE);				// creator's effective group id
	UInt32	GetQ_Mode(BOOL IsRefresh = FALSE);				// access modes
	UInt32	GetQ_Seq(BOOL IsRefresh = FALSE);				// slot usage sequence number
	key_t	GetQ_Key(BOOL IsRefresh = FALSE);				// key
	UInt32	GetQ_CurrentBytes(BOOL IsRefresh = FALSE);		// current # bytes on queue
	UInt32	GetQ_Number(BOOL IsRefresh = FALSE);			// # of messages on queue
	UInt32	GetQ_MaxBytes(BOOL IsRefresh = FALSE);			// max # of bytes on queue
	int		GetQ_LastSendPid(BOOL IsRefresh = FALSE);		// pid of last msgsnd()
	int		GetQ_LastRecvPid(BOOL IsRefresh = FALSE);		// pid of last msgrcv()
	time_t	GetQ_LastSendTime(BOOL IsRefresh = FALSE);		// last-msgsnd() time
	time_t	GetQ_LastRecvTime(BOOL IsRefresh = FALSE);		// last-msgrcv() time
	time_t	GetQ_LastChangeTime(BOOL IsRefresh = FALSE);	// last-change time
	struct msqid_ds		GetStatus(BOOL IsRefresh = FALSE);
public:
	UMessageQueue();
	~UMessageQueue();
};
//----------------------------------------------------------------------------------------------------------------------
// struct UQueueStruct
//----------------------------------------------------------------------------------------------------------------------
typedef struct {
	long	mtype;
	char	mtext[IPCSMQ_DEFAULTSIZE];
} UQueueStruct;
//----------------------------------------------------------------------------------------------------------------------
// Interface UQueueListener
//----------------------------------------------------------------------------------------------------------------------
class UQueueListener
{
public:
	virtual void OnQueueDataArrived( UQueueStruct *QueueData, int Len ) = 0;
	virtual ~UQueueListener() {}
};
//------------------------------------------------------------------------------------------------------------------------
// class UQueueSubject
//------------------------------------------------------------------------------------------------------------------------
class UQueueSubject : public PThread
{
private:
	UMessageQueue		FQueue;
	key_t				FQKey;
	long				FQmtype;
	int					FQPermission;
	int					FQBytes;
	BOOL				FQIsCreate;
	/////////////////////////////////////////////////////////////////
	UQueueListener*		FQListenerData;
	UQueueStruct		FQRcvData;
private:	/// PThread implementation
	void	Execute( void );
private:
	void	AttachQueue( void );
public:
	void	SetQueueListener( UQueueListener* QListener );
	void	Run( void );
public:
	UQueueSubject( key_t Key, long MessageType, int Q_Permission = 0666, int nbytes = IPCSMQ_DEFAULTSIZE, BOOL IsCreateQueue = TRUE );
	~UQueueSubject();
};
//----------------------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
