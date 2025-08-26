/******************************************************************************
 * Module   : UException.cpp
 * Author   : Allen Lee
 * Modified : 2005/5/4
 * Purpose  : Unix Exception definition.
 ******************************************************************************/
//------------------------------------------------------------------------------------------------------------------------
#ifndef _WIN32
//--------------------------------------------------------------------------------------------------------------------
#include "UException.h"
//--------------------------------------------------------------------------------------------------------------------
namespace UFC
{
//--------------------------------------------------------------------------------------------------------------------
UException::UException(int ErrNo)
: FErrNo( ErrNo )
{
#ifdef _SOLARIS
	// On Solaris platform, strerror is thread-safe, prototype as following. returns NULL if errnum is out-of-range.
	// --char *strerror (int errnum);
	char* tmpStr = strerror(FErrNo);
	if ( tmpStr == NULL )
		FErrMsg = "Unknow UFC framework Exception";
	else
		FErrMsg = tmpStr;
#else
	char tmpStr[256];
	// Call reentrant strerror_r function, prototype as following.
	// --extern int strerror_r(int errnum, char *buf, size_t buflen);
	if ( strerror_r(FErrNo, tmpStr, 256) == 0 )
		FErrMsg = tmpStr;
	else
		FErrMsg = "Unknow UFC framework Exception";
#endif
}
//--------------------------------------------------------------------------------------------------------------------
UException::UException( AnsiString ErrStr, int ErrNo )
: FErrNo( ErrNo )
{
#ifdef _SOLARIS
	// On Solaris platform, strerror is thread-safe, prototype as following. returns NULL if errnum is out-of-range.
	// --char *strerror (int errnum);
	const char* tmpStr = strerror(FErrNo);
	if ( tmpStr == NULL )
		FErrMsg.Printf( "%s : %s", ErrStr.c_str(), "Unknow UFC framework Exception" );
	else
		FErrMsg.Printf( "%s : %s", ErrStr.c_str(), tmpStr );
#else
	char tmpStr[256];

	// Call reentrant strerror_r function, prototype as following. On success, the strerror_r() function returns 0.
	// --extern int strerror_r(int errnum, char *buf, size_t buflen);
	if ( strerror_r(FErrNo, tmpStr, 256) == 0 )
		FErrMsg.Printf( "%s : %s", ErrStr.c_str(), tmpStr );
	else
		FErrMsg.Printf( "%s : %s", ErrStr.c_str(), "Unknow UFC framework Exception" );
#endif
}
//--------------------------------------------------------------------------------------------------------------------
const char* UException::what() const throw()
{
	return FErrMsg.c_str();
}
//--------------------------------------------------------------------------------------------------------------------
UIPCException::UIPCException(int ErrNo, FunctionType ftype)
: UException( " " )
{
	FErrNo = ErrNo;

	switch ( ftype )
	{
		case msgget:
			// msgget()
   			switch( ErrNo )
   			{
   				case EACCES:	FErrMsg = "IPC error: A message queue identifier exists for key, but operation permission as specified by permission bits of msgflg would not be granted."; break;
				case EEXIST:	FErrMsg = "IPC error: A message queue identifier exists for key, but both (msgflg & IPC_CREAT) and (msgflg & IPC_EXCL) are both non-zero."; break;
				case ENOENT:	FErrMsg = "IPC error: A message queue identifier does not exist for key and (msgflg & IPC_CREAT) is zero."; break;
				case ENOSPC:	FErrMsg = "IPC error: A message queue identifier is to be created, but the system-imposed limit on the maximum number of allowed message queue identifiers system wide would be exceeded."; break;
				default:		FErrMsg = "IPC error: Unknown error."; break;
			}
			break;
		case msgsnd:
			// msgsnd()
			switch( ErrNo )
			{
   				case EACCES:	FErrMsg = "IPC error: Operation permission is denied to the calling process."; break;
				case EFAULT:	FErrMsg = "IPC error: msgp is an invalid pointer."; break;
				case EIDRM:		FErrMsg = "IPC error: msqid was removed."; break;
				case EINTR:		FErrMsg = "IPC error: A signal interrupted the call."; break;
				case EINVAL:	FErrMsg = "IPC error: msgsz is less than 0 or greater than the system-imposed limit.\n";
								FErrMsg += "msqid is not a valid message queue identifier.\n";
								FErrMsg += "mtype is less than 1.";
								break;
				case ENOMSG:	FErrMsg = "IPC error: The message cannot be sent and (msgflg & IPC_NOWAIT) is non-zero."; break;
				case EAGAIN:	FErrMsg = "IPC error: The message queue is full and (msgflg & IPC_NOWAIT) is non-zero."; break;
				default:		FErrMsg = "IPC error: Unknown error."; break;
			}
			break;
		case msgrcv:
			// msgrcv()
			switch( ErrNo )
			{
				case E2BIG:		FErrMsg = "IPC error: The length of mtext is greater than msgsz and (msgflg & MSG_NOERROR) is zero."; break;
				case EACCES:	FErrMsg = "IPC error: Operation permission is denied to the calling process."; break;
				case EFAULT:	FErrMsg = "IPC error: msgp is an invalid pointer."; break;
				case EIDRM:		FErrMsg = "IPC error: msqid was removed."; break;
				case EINTR:		FErrMsg = "IPC error: A signal interrupted the call."; break;
				case EINVAL:	FErrMsg = "IPC error: msgsz is less than 0.\n";
								FErrMsg += "msqid is not a valid message queue identifier.";
								break;
				case ENOMSG:	FErrMsg = "IPC error: The queue does not contain a message of the desired type and (msgtyp & IPC_NOWAIT) is non-zero."; break;
				default:		FErrMsg = "IPC error: Unknown error."; break;
			}
			break;
		case msgctl:
			// msgctl()
			switch( ErrNo )
			{
				case EACCES:	FErrMsg = "IPC error: cmd is IPC_STAT and operation permission is denied to the calling process."; break;
				case EFAULT:	FErrMsg = "buf is an invalid pointer."; break;
				case EINVAL:	FErrMsg = "IPC error: cmd is IPC_SET and msg_perm.uid or msg_perm.gid is not valid.\n";
								FErrMsg += "cmd is not a valid command.\n";
								FErrMsg += "msqid is not a valid message queue identifier.";
								break;
				case EPERM:		FErrMsg = "cmd is IPC_RMID or IPC_SET and the effective user ID of the calling process is not equal to that of super user nor to the value of msg_perm.cuid or msg_perm.uid in the data structure associated with msqid.\n";
								FErrMsg += "cmd is IPC_SET and you are trying to increase msg_qbytes and the effective user ID of the calling process is not equal to that of super user.";
								break;
				default:		FErrMsg = "IPC error: Unknown error."; break;
			}
			break;
	}
}
//--------------------------------------------------------------------------------------------------------------------
}
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
