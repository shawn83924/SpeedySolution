/******************************************************************************
 * Module   : UIPC.cpp
 * Author   : Allen Lee
 * Modified : 2005/4/25
 * Purpose  : Unix IPC classes inclue Message queue , Shared memory.
 ******************************************************************************/
//------------------------------------------------------------------------------------------------------------------------
#ifndef _WIN32
//------------------------------------------------------------------------------------------------------------------------
#include "UIPC.h"
#include "BufferedLog.h"
//------------------------------------------------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------------------------------------------------
// class UMessageQueue
//------------------------------------------------------------------------------------------------------------------------
UMessageQueue::UMessageQueue()
{
	InitializeMember();
}
//------------------------------------------------------------------------------------------------------------------------
UMessageQueue::~UMessageQueue()
{
}
//------------------------------------------------------------------------------------------------------------------------
void UMessageQueue::InitializeMember(void)
{
	FIsAttach = FALSE;
	FQueueID = -1;
	FMsgKey = -1;
	memset( (void*)&FMsgStatus, 0, sizeof(FMsgStatus) );
}
//------------------------------------------------------------------------------------------------------------------------
BOOL UMessageQueue::AttachQueue(key_t Key, int Permission, BOOL IsCreate, BOOL IsFailIfExist)
{
	int n;

	if (IsCreate)
	{
		if (IsFailIfExist)
			n = msgget(Key, Permission | IPC_CREAT | IPC_EXCL);
		else
			n = msgget(Key, Permission | IPC_CREAT);
	}
	else
	{
		n = msgget(Key, Permission);
	}

	if (n < 0)
	{
		SetLastError();
		InitializeMember();
		return FALSE;
	}

	FIsAttach = TRUE;
	FMsgKey = Key;
	FQueueID = n;

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL UMessageQueue::ClearQueue(long type)
{
	struct {
		long	mtype;
		char	mtext[1];
	} msgbuf;

	int n;

	for (;;)
	{
		n = DeQueue(&msgbuf, 1, type, TRUE, TRUE);
		if (n < 0)
		{
			if ( LastErrNo == ENOMSG )
				return TRUE;
			else
				return FALSE;
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL UMessageQueue::RemoveQueue(void)
{
	int n;

	if (!FIsAttach)
	{
		SetLastError( 999, "IPC error: Message Queue have not attach, attach it first." );
		return FALSE;
	}

	n = msgctl(FQueueID, IPC_RMID, 0);

	if (n < 0)
	{
		SetLastError();
		return FALSE;
	}

	InitializeMember();

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL UMessageQueue::RefreshStatus(void)
{
	int n;

	if (!FIsAttach)
	{
		SetLastError( 999, "IPC error: Message Queue have not attach, attach it first." );
		return FALSE;
	}

	n = msgctl(FQueueID, IPC_STAT, &FMsgStatus);

	if (n < 0)
	{
		SetLastError();
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL UMessageQueue::InQueue(const void *msg_buffer, int nbytes, BOOL IsNoWait)
{
	int n;

	if (!FIsAttach)
	{
		SetLastError( 999, "IPC error: Message Queue have not attach, attach it first." );
		return FALSE;
	}

	if (IsNoWait)
		n = msgsnd(FQueueID, msg_buffer, nbytes, IPC_NOWAIT);
	else
		n = msgsnd(FQueueID, msg_buffer, nbytes, 0);

	if (n < 0)
	{
		SetLastError();
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
int UMessageQueue::DeQueue(void *msg_buffer, int nbytes, long type, BOOL IsNoWait, BOOL IsNoTruncatErr)
{
	int n;
	
	if (!FIsAttach)
	{
		SetLastError( 999, "IPC error: Message Queue have not attach, attach it first." );
		return -1;
	}

	if (IsNoWait)
	{
		if (IsNoTruncatErr)
			n = msgrcv(FQueueID, msg_buffer, nbytes, type, IPC_NOWAIT | MSG_NOERROR);
		else
			n = msgrcv(FQueueID, msg_buffer, nbytes, type, IPC_NOWAIT);
	}
	else
	{
		if (IsNoTruncatErr)
			n = msgrcv(FQueueID, msg_buffer, nbytes, type, MSG_NOERROR);
		else
			n = msgrcv(FQueueID, msg_buffer, nbytes, type, 0);
	}

	if (n < 0)
	{
		SetLastError();
	}

	return n;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL UMessageQueue::SetQueueBytes(unsigned long nbytes)
{
	int n;

	if (!RefreshStatus())
	{
		return FALSE;
	}

	FMsgStatus.msg_qbytes = nbytes;

	n = msgctl(FQueueID, IPC_SET, &FMsgStatus);

	if (n < 0)
	{
		SetLastError();
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
int UMessageQueue::GetQ_Uid(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

	return FMsgStatus.msg_perm.uid;
}
//------------------------------------------------------------------------------------------------------------------------
int UMessageQueue::GetQ_Gid(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

	return FMsgStatus.msg_perm.gid;
}
//------------------------------------------------------------------------------------------------------------------------
int UMessageQueue::GetQ_Cuid(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

	return FMsgStatus.msg_perm.cuid;
}
//------------------------------------------------------------------------------------------------------------------------
int UMessageQueue::GetQ_Cgid(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

	return FMsgStatus.msg_perm.cgid;
}
//------------------------------------------------------------------------------------------------------------------------
UInt32 UMessageQueue::GetQ_Mode(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

	return FMsgStatus.msg_perm.mode;
}
//------------------------------------------------------------------------------------------------------------------------
UInt32 UMessageQueue::GetQ_Seq(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

#ifdef __LINUX
	return FMsgStatus.msg_perm.__seq;
#else
	return FMsgStatus.msg_perm.seq;
#endif
}
//------------------------------------------------------------------------------------------------------------------------
key_t UMessageQueue::GetQ_Key(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

#ifdef __LINUX
	return FMsgStatus.msg_perm.__key;
#else
	return FMsgStatus.msg_perm.key;
#endif
}
//------------------------------------------------------------------------------------------------------------------------
UInt32 UMessageQueue::GetQ_CurrentBytes(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

	return FMsgStatus.msg_cbytes;
}
//------------------------------------------------------------------------------------------------------------------------
UInt32 UMessageQueue::GetQ_Number(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

	return FMsgStatus.msg_qnum;
}
//------------------------------------------------------------------------------------------------------------------------
UInt32 UMessageQueue::GetQ_MaxBytes(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

	return FMsgStatus.msg_qbytes;
}
//------------------------------------------------------------------------------------------------------------------------
int UMessageQueue::GetQ_LastSendPid(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

	return FMsgStatus.msg_lspid;
}
//------------------------------------------------------------------------------------------------------------------------
int UMessageQueue::GetQ_LastRecvPid(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

	return FMsgStatus.msg_lrpid;
}
//------------------------------------------------------------------------------------------------------------------------
time_t UMessageQueue::GetQ_LastSendTime(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

	return FMsgStatus.msg_stime;
}
//------------------------------------------------------------------------------------------------------------------------
time_t UMessageQueue::GetQ_LastRecvTime(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

	return FMsgStatus.msg_rtime;
}
//------------------------------------------------------------------------------------------------------------------------
time_t UMessageQueue::GetQ_LastChangeTime(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

	return FMsgStatus.msg_ctime;
}
//------------------------------------------------------------------------------------------------------------------------
struct msqid_ds UMessageQueue::GetStatus(BOOL IsRefresh)
{
	if (IsRefresh)
		RefreshStatus();

	return FMsgStatus;
}
//------------------------------------------------------------------------------------------------------------------------
void UMessageQueue::SetLastError( void )
{
	LastErrNo = errno;
#ifdef _SOLARIS
		char* tmpStr = strerror(LastErrNo);
		if ( tmpStr == NULL )
			strcpy( LastErrStr, "Unknow Error." );
		else
			strcpy( LastErrStr, tmpStr );
#else
		if ( strerror_r( LastErrNo, LastErrStr, 256 ) != 0 )
			strcpy( LastErrStr, "Unknow Error." );
#endif
}
//------------------------------------------------------------------------------------------------------------------------
void UMessageQueue::SetLastError( int ErrNo, const char *ErrStr )
{
	LastErrNo = ErrNo;
	strcpy( LastErrStr, ErrStr );
}
//------------------------------------------------------------------------------------------------------------------------
// class UQueueSubject
//------------------------------------------------------------------------------------------------------------------------
UQueueSubject::UQueueSubject( key_t Key, long MessageType, int Q_Permission, int nbytes, BOOL IsCreateQueue )
: PThread( NULL )
, FQKey( Key )
, FQmtype( MessageType )
, FQPermission( Q_Permission )
, FQBytes( nbytes )
, FQIsCreate( IsCreateQueue )
{	
	AttachQueue();
}
//------------------------------------------------------------------------------------------------------------------------
UQueueSubject::~UQueueSubject()
{
}
//------------------------------------------------------------------------------------------------------------------------
void UQueueSubject::AttachQueue( void )
{

	if ( !FQueue.AttachQueue(FQKey, FQPermission, FQIsCreate) )
	{
		UFC::BufferedLog::Printf(" Attach Queue 0x%x failed. <Reason:%s(%d)>"
			,FQKey
			,FQueue.LastErrStr
			,FQueue.LastErrNo );
		exit(0);
	}
		
	UFC::BufferedLog::Printf(" AttachQueue 0x%x success.", FQKey);
}
//------------------------------------------------------------------------------------------------------------------------
void UQueueSubject::Execute( void )
{
	int n;

	memset(&FQRcvData, 0, sizeof(FQRcvData));
	while ( TRUE )
	{
		if ( (n = FQueue.DeQueue(&FQRcvData, FQBytes, FQmtype, FALSE)) < 0 )
		{
			BufferedLog::Printf(" Receive Queue 0x%x failed. <Reason:%s(%d)>"
				, FQKey
				, FQueue.LastErrStr
				, FQueue.LastErrNo );
			exit(0);
		}
		FQRcvData.mtext[n] = 0;
		FQListenerData->OnQueueDataArrived( &FQRcvData, n );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void UQueueSubject::SetQueueListener( UQueueListener* QListener )
{
	FQListenerData = QListener;
}
//------------------------------------------------------------------------------------------------------------------------
void UQueueSubject::Run( void )
{
	Start();
}
//------------------------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
