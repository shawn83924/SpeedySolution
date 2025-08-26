//------------------------------------------------------------------------------------------------------------------------
#ifndef _WIN32
//------------------------------------------------------------------------------------------------------------------------
#include "Utility.h"
//------------------------------------------------------------------------------------------------------------------------
// sleep microsecond
//------------------------------------------------------------------------------------------------------------------------
void sleep_us(unsigned int nusecs)
{
	struct timeval tval;
	tval.tv_sec = nusecs / 1000000;
	tval.tv_usec = nusecs % 1000000;
	select(0, NULL, NULL, NULL, &tval);
}
//------------------------------------------------------------------------------------------------------------------------
// sleep millisecond
//------------------------------------------------------------------------------------------------------------------------
void sleep_ms(unsigned int nmsecs)
{
	struct timeval tval;
	tval.tv_sec = nmsecs / 1000;
	tval.tv_usec = (nmsecs % 1000) * 1000;
	select(0, NULL, NULL, NULL, &tval);
}
//------------------------------------------------------------------------------------------------------------------------
// alarm microsecond
//------------------------------------------------------------------------------------------------------------------------
void alarm_us(unsigned int nusecs)
{
	struct itimerval intval;
	
	intval.it_value.tv_sec = nusecs / 1000000;
	intval.it_value.tv_usec = nusecs % 1000000;
	intval.it_interval.tv_sec = nusecs / 1000000;
	intval.it_interval.tv_usec = nusecs % 1000000;
	
	setitimer(ITIMER_REAL, &intval, NULL);
}
//------------------------------------------------------------------------------------------------------------------------
// alarm millisecond
//------------------------------------------------------------------------------------------------------------------------
void alarm_ms(unsigned int nmsecs)
{
	struct itimerval intval;
	intval.it_value.tv_sec = nmsecs / 1000;
	intval.it_value.tv_usec = (nmsecs % 1000) * 1000;
	intval.it_interval.tv_sec = nmsecs / 1000;
	intval.it_interval.tv_usec = (nmsecs % 1000) * 1000;
	
	setitimer(ITIMER_REAL, &intval, NULL);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
inline int cns2ms( long int  nsec )
{
    return (int)nsec/1000000;
}
//------------------------------------------------------------------------------------------------------------------------
// get time (millisecond)
// On success, return result. On error, it returns 0. 
// MT-safe. version
//------------------------------------------------------------------------------------------------------------------------
int time_ms(void)
{	
	struct tm     tblock;
	long   ret;
        struct timespec now;
    
        clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    
	if( localtime_r( &now.tv_sec, &tblock ) == 0)
            return 0;
	ret = tblock.tm_hour;
	ret = ret * 60 + tblock.tm_min;
	ret = ret * 60 + tblock.tm_sec;
	ret = ret * 1000 + cns2ms(now.tv_nsec) ;
	return ret;
}
//------------------------------------------------------------------------------------------------------------------------
// copies up to n characters, and append a terminating null character
//------------------------------------------------------------------------------------------------------------------------
char *strncpy_zero(char *s1, const char *s2, size_t n)
{
	strncpy(s1, s2, n);
	s1[n] = 0;
	return s1;
}
//------------------------------------------------------------------------------------------------------------------------
// get environment variable, if environment variable is not exist, return default value
//------------------------------------------------------------------------------------------------------------------------
char *getenv_default(char *result, const char *name, char *default_value)
{
	char *s1;

	s1 = getenv(name);
	if (s1 == 0 || strlen(s1) == 0)
		strcpy(result, default_value);
	else
		strcpy(result, s1);

	return result;
}
/*------------------------------------------------------------------------------------------------------------------------*/
/*
 * read "n" bytes from a descriptor.
 * wrap the read in bread, loops read until "n" bytes has read except for 
 * either a error occurs or EOF
 * parameter:
 * fd   : argument socket file descriptor
 * vptr : argument point to buffer
 * n    : argument how many bytes for read
 * return value:
 * n    : Successful how many bytes has read
 * 0    : EOF
 * -1   : Failure
 */
/*------------------------------------------------------------------------------------------------------------------------*/
ssize_t breadn(int fd, void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = (char *)vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;		/* and call read() again */
			else
				return(-1);
		} else if (nread == 0)
			break;				/* EOF */

		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);		/* return >= 0 */
}
/*------------------------------------------------------------------------------------------------------------------------*/
/* 
 * write "n" bytes to a descriptor.
 * wrap the write in bwrite, loops write until "n" bytes has write except for 
 * either a error occurs
 * parameter:
 * fd   : argument socket file descriptor
 * vptr : argument point to buffer
 * n    : argument how many bytes for write
 * return value:
 * n    : Successful how many bytes has read
 * -1   : Failure
 */
/*------------------------------------------------------------------------------------------------------------------------*/
ssize_t bwriten(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = (char *)vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
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
//------------------------------------------------------------------------------------------------------------------------
// get date for format YYYYMMDD X(8)
//------------------------------------------------------------------------------------------------------------------------
void getYYYYMMDD(char *strbuf)
{
	struct tm	FTime;
	time_t		tTime;

	time( &tTime );
	localtime_r( &tTime, &FTime );
	sprintf( strbuf, "%04d%02d%02d", FTime.tm_year+1900, FTime.tm_mon+1, FTime.tm_mday );
}
//------------------------------------------------------------------------------------------------------------------------
// get date for format YYMMDD X(6) - Taiwan years
//------------------------------------------------------------------------------------------------------------------------
void getYYMMDD(char *strbuf)
{
	struct tm	FTime;
	time_t		tTime;

	time( &tTime );
	localtime_r( &tTime, &FTime );
	sprintf( strbuf, "%02d%02d%02d", FTime.tm_year+1900-1911, FTime.tm_mon+1, FTime.tm_mday );
}
//------------------------------------------------------------------------------------------------------------------------
// get date for format YYYMMDD X(7) - Taiwan years
//------------------------------------------------------------------------------------------------------------------------
void getYYYMMDD(char *strbuf)
{
	struct tm	FTime;
	time_t		tTime;

	time( &tTime );
	localtime_r( &tTime, &FTime );
	sprintf( strbuf, "%03d%02d%02d", FTime.tm_year+1900-1911, FTime.tm_mon+1, FTime.tm_mday );
}
//------------------------------------------------------------------------------------------------------------------------
// get time for format HHMMSS X(6)
//------------------------------------------------------------------------------------------------------------------------
void getHHMMSS(char *strbuf)
{
    struct tm    FTime;
	time_t		tTime;

    time( &tTime );
    localtime_r( &tTime, &FTime );

	sprintf( strbuf, "%02d%02d%02d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec );
}
//------------------------------------------------------------------------------------------------------------------------
// get time for format HH:MM:SS X(8)
//------------------------------------------------------------------------------------------------------------------------
void getHH_MM_SS(char *strbuf)
{
    struct tm    FTime;
	time_t		tTime;

    time( &tTime );
    localtime_r( &tTime, &FTime );
    sprintf( strbuf, "%02d:%02d:%02d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec );
}
//------------------------------------------------------------------------------------------------------------------------
// get time for format HHMMSSmmm X(9)
//------------------------------------------------------------------------------------------------------------------------
void getHHMMSSmmm(char *strbuf)
{
    struct tm    FTime;
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    	
    localtime_r( &now.tv_sec, &FTime );
    sprintf( strbuf, "%02d%02d%02d%03d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec, cns2ms(now.tv_nsec) );
}
//------------------------------------------------------------------------------------------------------------------------
// get time for format HH:MM:SS.mmm X(12)
//------------------------------------------------------------------------------------------------------------------------
void getHH_MM_SS_mmm(char *strbuf)
{
    struct tm    FTime;
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    	
    localtime_r( &now.tv_sec, &FTime );
    sprintf( strbuf, "%02d:%02d:%02d.%03d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec, cns2ms(now.tv_nsec) );
}
//------------------------------------------------------------------------------------------------------------------------
// get time for format HHMMSSmm X(8)
//------------------------------------------------------------------------------------------------------------------------
void getHHMMSSmm(char *strbuf)
{
    struct tm    FTime;
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    	
    localtime_r( &now.tv_sec, &FTime );
    sprintf( strbuf, "%02d%02d%02d%02d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec, (cns2ms(now.tv_nsec)/10) );
}
//------------------------------------------------------------------------------------------------------------------------
// get time for format HH:MM:SS.mm X(11)
//------------------------------------------------------------------------------------------------------------------------
void getHH_MM_SS_mm(char *strbuf)
{
    struct tm    FTime;
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    	
    localtime_r( &now.tv_sec, &FTime );
    sprintf( strbuf, "%02d:%02d:%02d.%02d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec, (cns2ms(now.tv_nsec)/10) );
}
//------------------------------------------------------------------------------------------------------------------------
// check file if exist
//------------------------------------------------------------------------------------------------------------------------
BOOL uIsFileExists( const char* FileName )
{
    int FD = open( FileName, O_RDONLY );

    if( FD == -1 )
        return FALSE;
    close( FD );
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
// delete file
//------------------------------------------------------------------------------------------------------------------------
BOOL uDeleteFile( const char* FileName )
{
	if( unlink( FileName ) == -1 )
		return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
// create file
//------------------------------------------------------------------------------------------------------------------------
BOOL uCreateFile( const char* FileName, unsigned long Permissions, BOOL IsTruncateFile )
{
	if ( IsTruncateFile )
	{
		if ( open( FileName, O_CREAT|O_TRUNC, Permissions ) == -1 )
			return FALSE;
	}
	else
	{
		if ( open( FileName, O_CREAT, Permissions ) == -1 )
			return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
// create file with Exception version
//------------------------------------------------------------------------------------------------------------------------
void ueCreateFile( const char* FileName, unsigned long Permissions, BOOL IsTruncateFile )
{
	if ( uCreateFile( FileName, Permissions, IsTruncateFile ) )
		return;
	else
	{
		UFC::AnsiString Msg;
#ifdef _SOLARIS
		// On Solaris platform, strerror is thread-safe, prototype as following. returns NULL if errnum is out-of-range.
		// --char *strerror (int errnum);
		char* tmpStr = strerror(errno);
		if ( tmpStr == NULL )
			Msg.Printf( "Open file %s failed - Unknow UFC framework Exception", FileName );
		else
			Msg.Printf( "Open file %s failed - %s", FileName, tmpStr );
#else
		char tmpStr[256];
		// Call reentrant strerror_r function, prototype as following.
		// --extern int strerror_r(int errnum, char *buf, size_t buflen);
		if ( strerror_r(errno, tmpStr, 256) == 0 )
			Msg.Printf( "Open file %s failed - Unknow UFC framework Exception", FileName );
		else
			Msg.Printf( "Open file %s failed - %s", FileName, tmpStr );
#endif
		throw( UFC::Exception( (char*) Msg.c_str()));
	}
}
//------------------------------------------------------------------------------------------------------------------------
// Right trim c null terminate string
//------------------------------------------------------------------------------------------------------------------------
char *uCstrRTrim( char* str )
{
	char *p;
	int len;

	if (str==NULL) return str;

	if ((len=strlen(str)) > 0) 
	{
		for (p=str+len-1; isspace((int)(*p)); p--) 
		{
			*p='\0';
			if (p == str)
				break;
		}
	}
	return str;
}
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
