#include "Exception.h"
#include "AnsiString.h"
#include "UFCType.h"

//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
void IndexOutOfBoundException::ErrorToWhat( int Index )
{
    sprintf( Message, "Index %d out of bound", Index );
}
//---------------------------------------------------------------------------
void ThreadException::ErrorToWhat( int Error )
{
    switch( Error )
    {
        case ENOMEM: strcpy( Message, "PThread error: The system lacked the necessary resources to create another thread.");return;
        case EINVAL: strcpy( Message, "PThread error: Thread or attr is an invalid pointer.");return;
        case EPERM:  strcpy( Message, "PThread error: Attr is not an initialized thread attribute object.");return;
        default:     break;
    }
    sprintf( Message, "Thread error[%d][%s].", Error, strerror( Error ) );
}
//---------------------------------------------------------------------------
void SocketException::ErrorToWhat( int ErrorCode )
{
    switch( ErrorCode )
    {

#if _MSC_VER >= 1600 ///< for >= VC10 	
        case WSAEBADF:       strncpy( Message,  "Socket error: One or more of the file descriptor sets specified a file descriptor that is not a valid open file descriptor or specified a file descriptor that does not support selection.", ERR_MSG_MAX);return;
        case WSAEINTR:       strcpy( Message,  "Socket error: A signal interrupted the call.");return;
        case EISDIR:         strcpy( Message,  "Socket error: One or more the file descriptor sets specified a file descriptor that refers to an open directory.");return;
        case WSAEINVAL:      strcpy( Message,  "Socket error: A parameter is outside the acceptable range.");return;
        case WSAEFAULT:      strcpy( Message,  "Socket error: addr or addrlen is an invalid pointer, or addrlen is too small.");return;
        case WSAENETDOWN:    strcpy( Message,  "Socket error: The network subsystem has failed.");return;
        case WSAENOBUFS:     strcpy( Message,  "Socket error: No buffer space is available.");return;
        case WSAENOTSOCK:    strcpy( Message,  "Socket error: The descriptor is not a socket.");return;
        case WSAEMFILE:      strcpy( Message,  "Socket error: Cannot open a file.");return;
        case WSAEOPNOTSUPP:  strcpy( Message,  "Socket error: The referenced socket is not a type that supports connection-oriented service.");return;
        case WSAECONNRESET:  strcpy( Message,  "Socket error: Connection reset by peer.");return;
        case WSAEWOULDBLOCK: strcpy( Message,  "Socket error: The socket is marked as non-blocking and no connections are present to be accepted.");return;
        case WSAENETUNREACH: strcpy( Message,  "Socket error: Network is unreachable");return;
        case WSAENETRESET:   strcpy( Message,  "Socket error: Network dropped connection on reset");return;
        case WSAECONNABORTED:strcpy( Message,  "Socket error: Software caused connection abort");return;
        case WSAEISCONN:     strcpy( Message,  "Socket error: Socket is already connected");return;
        case WSAENOTCONN:    strcpy( Message,  "Socket error: Socket is not connected");return;
        case WSAESHUTDOWN:   strcpy( Message,  "Socket error: Can't send after socket shutdown");return;
        case WSAETIMEDOUT:   strcpy( Message,  "Socket error: Connection timed out");return;
        case WSAECONNREFUSED:strcpy( Message,  "Socket error: Connection refused");return;
        case WSAEHOSTDOWN:   strcpy( Message,  "Socket error: Host is down");return;
        case WSAEHOSTUNREACH:strcpy( Message,  "Socket error: No route to host");return;
#else
        case EBADF:       strncpy( Message,  "Socket error: One or more of the file descriptor sets specified a file descriptor that is not a valid open file descriptor or specified a file descriptor that does not support selection.",ERR_MSG_MAX);return;
        case EINTR:       strcpy( Message,  "Socket error: A signal interrupted the call.");return;
        case EISDIR:      strcpy( Message,  "Socket error: One or more the file descriptor sets specified a file descriptor that refers to an open directory.");return;
        case EINVAL:      strcpy( Message,  "Socket error: A parameter is outside the acceptable range.");return;
        case EFAULT:      strcpy( Message,  "Socket error: addr or addrlen is an invalid pointer, or addrlen is too small.");return;
        case ENETDOWN:    strcpy( Message,  "Socket error: The network subsystem has failed.");return;
        case ENOBUFS:     strcpy( Message,  "Socket error: No buffer space is available.");return;
        case ENOTSOCK:    strcpy( Message,  "Socket error: The descriptor is not a socket.");return;
        case EMFILE:      strcpy( Message,  "Socket error: Cannot open a file.");return;
        case EOPNOTSUPP:  strcpy( Message,  "Socket error: The referenced socket is not a type that supports connection-oriented service.");return;
        case ECONNRESET:  strcpy( Message,  "Socket error: Connection reset by peer.");return;
        case EWOULDBLOCK: strcpy( Message,  "Socket error: The socket is marked as non-blocking and no connections are present to be accepted.");return;
        case ENETUNREACH: strcpy( Message,  "Socket error: Network is unreachable");return;
        case ENETRESET:   strcpy( Message,  "Socket error: Network dropped connection on reset");return;
        case ECONNABORTED:strcpy( Message,  "Socket error: Software caused connection abort");return;
        case EISCONN:     strcpy( Message,  "Socket error: Socket is already connected");return;
        case ENOTCONN:    strcpy( Message,  "Socket error: Socket is not connected");return;
        case ESHUTDOWN:   strcpy( Message,  "Socket error: Can't send after socket shutdown");return;
        case ETIMEDOUT:   strcpy( Message,  "Socket error: Connection timed out");return;
        case ECONNREFUSED:strcpy( Message,  "Socket error: Connection refused");return;
        case EHOSTDOWN:   strcpy( Message,  "Socket error: Host is down");return;
        case EHOSTUNREACH:strcpy( Message,  "Socket error: No route to host");return;
#endif
        default:          break;
    }    
    sprintf( Message, "Socket error[%d][%s].", ErrorCode, strerror( ErrorCode ) );
}

//---------------------------------------------------------------------------
#ifndef _WIN32
//---------------------------------------------------------------------------
void ShareMenException::ErrorToWhat( int ErrorCode )
{
    switch( ErrorCode )
    {
        case EACCES:	strncpy( Message, "Shared memory error: Operation permission as specified by the permission bits of shmflg would not be granted.",ERR_MSG_MAX);return;
        case EEXIST:	strncpy( Message, "Shared memory error: Both (shmflg&IPC_CREAT) and (shmflg&IPC_EXCL) are non-zero.",ERR_MSG_MAX);return;
        case EINVAL:	strncpy( Message, "Shared memory error: The size of the segment associated with it is less than size and size is not equal to zero.size is less than the system-imposed minimum or greater than the system-imposed maximum.",ERR_MSG_MAX);return;
        case ENOENT:	strncpy( Message, "Shared memory error: A shared memory identifier does not exist for key and (shmflg&IPC_CREAT) is zero.",ERR_MSG_MAX);return;
        case ENOMEM:	strncpy( Message, "Shared memory error: The amount of available memory is not sufficient to fulfill the request.",ERR_MSG_MAX);return;
        case ENOSPC:	strncpy( Message, "Shared memory error: The system-imposed limit on the maximum number of allowed shared memory identifiers system wide would be exceeded.",ERR_MSG_MAX);return;
        case EMFILE:    strncpy( Message, "Shared memory error: The number of shared memory segments attached to the calling process exceeds the system-imposed limit.",ERR_MSG_MAX);return;
        default:		break;
    }
    sprintf( Message, "Shared memory error[%d][%s].", ErrorCode, strerror( ErrorCode ) );
}
//---------------------------------------------------------------------------
void MutexException::ErrorToWhat( int ErrorCode )
{
    switch( ErrorCode )
    {
        case EAGAIN: strcpy( Message, "Mutex error: System resources (other than memory) are unavailable.");return;
        case EFAULT: strcpy( Message, "Mutex error: mutex or attr is an invalid pointer.");return;
        case ENOMEM: strcpy( Message, "Mutex error: Insufficient memory exists to initialize the mutex or mutex attribute object.");return;
        default:     break;
    }
    sprintf( Message, "Mutex error[%d][%s]", ErrorCode,strerror(ErrorCode) );
}
//---------------------------------------------------------------------------
void ConditionVariableException::ErrorToWhat( int ErrorCode )
{
    switch( ErrorCode )
    {
        case EAGAIN: strcpy( Message, "Condition Variable error: System resources (other than memory) are unavailable.");return;
        case EFAULT: strcpy( Message, "Condition Variable error: Condition Variable or attr is an invalid pointer.");return;
        case ENOMEM: strcpy( Message, "Condition Variable error: Insufficient memory exists to initialize the Condition Variable or Condition Variable attribute object.");return;
        default:     break;
    }
    sprintf( Message, "Condition Variable error[%d][%s]", ErrorCode,strerror(ErrorCode) );    
}
//---------------------------------------------------------------------------
void QueueException::ErrorToWhat( int ErrorCode )
{
    switch( ErrorCode )
    {
        case EAGAIN:	strcpy( Message, "Queue error EAGAIN: Resource temporarily unavailable.");return;
		case EACCES:	strcpy( Message, "Queue error EACCES: Permission denied.");return;
        case EFAULT:	strcpy( Message, "Queue error EFAULT: Bad address.");return;
        case EIDRM:	strcpy( Message, "Queue error EIDRM: Identifier removed.");return;
        case EINTR:	strcpy( Message, "Queue error EINTR: interrupted system call.");return;
        case EINVAL:	strcpy( Message, "Queue error EINVAL: Invalid argument\n msgsz is less than 0 or greater than the system-imposed limit.\n msqid is not a valid message queue identifier.\n mtype is less than 1.");return;
        case ENOMSG:	strcpy( Message, "Queue error ENOMSG: No message of desired type.");return;
        default:        break;
    }
	sprintf( Message, "Queue error[%d][%s]", ErrorCode,strerror(ErrorCode) );
}
//---------------------------------------------------------------------------
#else
//---------------------------------------------------------------------------
void ShareMenException::ErrorToWhat( int ErrorCode )
{
	switch( ErrorCode )
	{
		case -1:	strcpy( Message, "File mapping error: Create mapping file error.");return;
		case -2:	strcpy( Message, "File mapping error: Create file mapping object error.");return;
		case -4:	strcpy( Message, "File mapping error: Map view Of file error.");return;
		default:	break;
	}
	sprintf( Message, "File mapping error[%d][%s]", ErrorCode,strerror(ErrorCode) );
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

