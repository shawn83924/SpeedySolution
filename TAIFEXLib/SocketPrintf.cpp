#ident "@(#) $Id: SocketPrintf.cpp,v 1.15 2004/05/21 10:34:21 ap27 Exp $"
//---------------------------------------------------------------------------
#include <pthread.h> 

#include <sys/types.h>		/* basic system data types */
#include <sys/socket.h>		/* basic socket definitions */
#include <netdb.h>
#include <sys/uio.h>
#include <string.h>
#include <netinet/in.h>		/* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>		/* inet(3) functions */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>


#include <string.h>
#include <ctype.h> 
#include <stdarg.h>
#include "../UFC/PThread.h"
#include "Utility.h"

//---------------------------------------------------------------------------
int SocketConnect( const char *host, const short port, int timeout_sec )
{
	int					sd;
	struct sockaddr_in	sin;
	struct hostent		*hp;
	fd_set				rset, wset;
	int					flags, n, error;
	socklen_t			len;
	struct timeval		tval;

	/* get an internet domain socket */
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return -1;

	// Set socket nonblocing
	if ((flags = fcntl(sd, F_GETFL, 0)) == -1)
		return -1;
	if (fcntl(sd, F_SETFL, flags | O_NONBLOCK) < 0)
		return -1;

	/* complete the socket structure */
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons((short)port);

	/* get the IP address of the requested host */
	if ((hp = gethostbyname(host)) == 0)
		return -1;

	sin.sin_addr.s_addr = ((struct in_addr *)(void*)(hp->h_addr))->s_addr;

	error = 0;
	if ( (n = connect(sd, (struct sockaddr *) &sin, sizeof(sin))) < 0)
		if (errno != EINPROGRESS)
			return -1;


	if (n != 0)
	{
		FD_ZERO(&rset);
		FD_SET(sd, &rset);
		wset = rset;
		tval.tv_sec = timeout_sec;
		tval.tv_usec = 0;
	
		if ( (n = select(sd+1, &rset, &wset, NULL, timeout_sec ? &tval : NULL)) == 0 )
		{
			// close socket fd, if fail then ignore, because will return failure
			if ( close(sd) < 0) { }
			errno = ETIMEDOUT;	// timeout
			return -1;
		}

		if (FD_ISSET(sd, &rset) || FD_ISSET(sd, &wset))
		{
			len = sizeof(error);
			if (getsockopt(sd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
			{
				// close socket fd, if fail then ignore, because will return failure
				if ( close(sd) < 0) { }
				return -1;			// Solaris pending error
			}
		}
		else
		{
			// close socket fd, if fail then ignore, because will return failure
			if ( close(sd) < 0) { }
			return -1;
		}
	}

	// Restore file status flags
	if (fcntl(sd, F_SETFL, flags) < 0)
	{
		// close socket fd, if fail then ignore, because will return failure
		if ( close( sd ) < 0) { }
		return -1;
	}
	
	// Check error through getsockopt
	if (error)
	{
		// close socket fd, if fail then ignore, because will return failure
		if ( close(sd) < 0) { }
		errno = error;
		return -1;
	}
	return sd;
}
//------------------------------------------------------------------------------
ssize_t SocketWriten(int fd, const void *vptr, size_t n, int flag )
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = (char *)vptr;
	nleft = n;
	while (nleft > 0) 
	{
		if ( (nwritten = send( fd, ptr, nleft, flag ) ) <= 0) 
		{
			if (errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}
#ifdef __SOCKET_LOG
//------------------------------------------------------------------------------
UFC::PCriticalSection SocketCriticalSection;
int  DebugSocketFD = -1;
BOOL AsPrintf = FALSE;
//------------------------------------------------------------------------------
int SocketPrintf( const char* FormatStr, ... )
{
	int      Count;     
	char     TmpBuffer[255];
	char     Buffer[ 255 ];
	va_list  va;
	
	va_start( va , FormatStr ); 
    Count = vsnprintf( TmpBuffer , 255, FormatStr, va );
    va_end( va );	
		
	SocketCriticalSection.Enter();
	if( DebugSocketFD == -1 )
	{
		if( !AsPrintf )
		{
			printf("------------------------------\n");
			printf(" Try to connect log server...\n");
			printf("------------------------------\n");
	    	if(( DebugSocketFD = SocketConnect( MonitorIP , MonitorPort, 2 )) == -1)
	        	 AsPrintf = TRUE;	        	 
	    }
	}
	if( AsPrintf == TRUE )
	{
		printf("%s",TmpBuffer );
	}
	else
	{
		int ThisThreadID  = pthread_self( ); 
		int ThisProcessID = getpid( ); 
		
		sprintf( Buffer,"%04d%04d:%s",ThisProcessID, ThisThreadID, TmpBuffer );
		if( SocketWriten( DebugSocketFD, Buffer, strlen( Buffer ), 0 ) == -1 )
		{
			printf("%s",TmpBuffer );
			AsPrintf = TRUE;
		}
	}	
	SocketCriticalSection.Leave();
	return Count;
}
#else
int SocketPrintf( const char* FormatStr, ... )
{
	int      Count;     
	char     Buffer[ 255 ];
	va_list  va;
	
	va_start( va , FormatStr ); 
    Count = vsnprintf( Buffer , 255, FormatStr, va );
    va_end( va );			
	printf("%s",Buffer );
	return Count;
}
#endif
//---------------------------------------------------------------------------
//SocketDebuger DebugerConsole( MonitorIP, CommandPort);
//---------------------------------------------------------------------------
/*SocketDebuger::SocketDebuger( const char* ip, const short Port )
:PThread( NULL ),FCommandFD( -1 ),FConnected( FALSE ),FListener( NULL )
{
	 if(( FCommandFD = SocketConnect( ip , Port,3 )) != -1 )
	 {
	      FConnected = TRUE;
	      Start();
	 }
}
//---------------------------------------------------------------------------
void SocketDebuger::SendCommand( int Command, void* Data, int DataSize )
{
	if( FConnected == TRUE )
	{
		if( SocketWriten( FCommandFD, Data, DataSize, MSG_OOB ) == -1 )
			FConnected = FALSE; 
	}
}
//---------------------------------------------------------------------------
void SocketDebuger::Execute( void  )
{
	FReady = select( FCommandFD + 1, &FReadSet, NULL, NULL, &FIdleTime );
	if( FReadyCount < 0 )
	{
			throw( SocketException( errno ));
	}
	else if( FReadyCount == 1 )
		CheckDataArrive();	
	
}
//---------------------------------------------------------------------------*/


