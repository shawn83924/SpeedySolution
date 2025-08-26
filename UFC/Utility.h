#ifndef __UFC_UTILITY_H
#define __UFC_UTILITY_H
//------------------------------------------------------------------------------------------------------------------------
#ifndef _WIN32
//------------------------------------------------------------------------------------------------------------------------
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h> 
#include <time.h> 
#include <signal.h> 
#include <sys/timeb.h>
#include <string.h>
#ifdef _AIX433
	#include <strings.h>
#endif
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "UFC.h"
//------------------------------------------------------------------------------------------------------------------------
// Macro
//------------------------------------------------------------------------------------------------------------------------
// Maximum Macro
#define UM_MAX(a,b)  (((a) > (b)) ? (a) : (b))
//------------------------------------------------------------------------------------------------------------------------
// Function
//------------------------------------------------------------------------------------------------------------------------
// sleep microsecond
extern void sleep_us(unsigned int nusecs);
// sleep millisecond
extern void sleep_ms(unsigned int nmsecs);
// alarm microsecond
extern void alarm_us(unsigned int nusecs);
// alarm millisecond
extern void alarm_ms(unsigned int nmsecs);
// get time detail to millisecond
extern int time_ms(void);
// copies up to n characters, and append a terminating null character
extern char *strncpy_zero(char *s1, const char *s2, size_t n);
// get environment variable, if environment variable is not exist, return default value
extern char *getenv_default(char *result, const char *name, char *default_value);
// block read n bytes
extern ssize_t	breadn(int fd, void *vptr, size_t n);
// block write n bytes
extern ssize_t	bwriten(int fd, const void *vptr, size_t n);
//------------------------------------------------------------------------------------------------------------------------
// get date for format YYYYMMDD X(8)
extern void getYYYYMMDD(char *strbuf);
// get date for format YYMMDD X(6) - Taiwan years
extern void getYYMMDD(char *strbuf);
// get date for format YYYMMDD X(7) - Taiwan years
extern void getYYYMMDD(char *strbuf);
// get time for format HHMMSS X(6)
extern void getHHMMSS(char *strbuf);
// get time for format HH:MM:SS X(8)
extern void getHH_MM_SS(char *strbuf);
// get time for format HHMMSSmmm X(9)
extern void getHHMMSSmmm(char *strbuf);
// get time for format HH:MM:SS.mmm X(12)
extern void getHH_MM_SS_mmm(char *strbuf);
// get time for format HHMMSSmm X(8)
extern void getHHMMSSmm(char *strbuf);
// get time for format HH:MM:SS.mm X(11)
extern void getHH_MM_SS_mm(char *strbuf);
//------------------------------------------------------------------------------------------------------------------------
extern BOOL uIsFileExists( const char* FileName );
extern BOOL uDeleteFile( const char* FileName );
extern BOOL uCreateFile( const char* FileName, unsigned long Permissions, BOOL IsTruncateFile = FALSE );
extern void ueCreateFile( const char* FileName, unsigned long Permissions, BOOL IsTruncateFile = FALSE );
//------------------------------------------------------------------------------------------------------------------------
extern char *uCstrRTrim( char* str );
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
