#include "AnsiString.h"
#include "NetUtility.h"
#include "iniFile.h"
#include "BufferedLog.h"
#include "PSocket.h"
#include <limits.h>
#include <math.h>
#ifndef _WIN32
#include <unistd.h>
#include <sys/wait.h>
#endif
#if defined(_AIX)
    #define SIGUNUSED SIGMAX              ///< SIGUNUSED not define in AIX    
	typedef void (*__sighandler_t)(int);  ///< __sighandler_t not define in AIX
#elif defined(__SOLARIS)
    #include <sys/signal.h>
    #define SIGUNUSED MAXSIG              ///< SIGUNUSED not define in Solaris
    typedef void (*__sighandler_t)(int);  ///< __sighandler_t not define in Solaris
#elif defined(__LINUX)
    #if !defined(SIGUNUSED)
        #define SIGUNUSED SIGSYS
#endif
#endif

#define _USE_CLOCK_GETTIME
//---------------------------------------------------------------------------
#if !defined(CLOCK_MONOTONIC_COARSE)    
    #define CLOCK_MONOTONIC_COARSE CLOCK_MONOTONIC
#endif    
#if !defined(CLOCK_REALTIME_COARSE)    
    #define CLOCK_REALTIME_COARSE CLOCK_REALTIME
#endif    
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
PEndian      Endian;
PInitNetLib  InitNetLib;
char         Hostname[ 64 ];
char         WorkingDir[ PATH_MAX ];
int          GOpenTime     = 550;  ///< 0650
int 	     GCloseTime    = 515;  ///< 0615
int          GTimeZone     = +8;
BOOL         GLog_us       = FALSE;
BOOL         GLogThread    = FALSE;
Int64        Tickus;
UFC::AnsiString GLineShell = "./cloud_monitor.sh";
UFC::AnsiString GLineNotifyShell = "sh line_notify_monitor.sh";
int          GLineGroup  = 1;       
int          GMinSleepUS = 10;
int          GDeqPerUS   = 500;
//------------------------------------------------------------------------------
int          GYear;
int          GMonth;
int          GDay;
//------------------------------------------------------------------------------
#ifndef _WIN32
static void RunShellScript( const char* const argv[] )
{
    pid_t pid = fork();
    if( pid < 0 )
        return;
    if( pid == 0 )
    {
        execvp( argv[0], (char* const*)argv );
        _exit( 127 );
    }
    waitpid( pid, NULL, 0 );
}
#endif
//------------------------------------------------------------------------------
void SendLineMessage( LineMsgTypeEnum Type, const UFC::AnsiString& Message )
{
    UFC::AnsiString MsgArg;
    UFC::AnsiString GroupStr;
    UFC::AnsiString TypeStr;
    UFC::AnsiString IPStr = PSocket::GetLocalIP();

    MsgArg.Printf(  "%s_%s",  UFC::Hostname, Message.c_str() );
    GroupStr.Printf( "%d", GLineGroup );
    TypeStr.Printf(  "%d", (int)Type );

    UFC::BufferedLog::Printf(" Command[%s %s %s %s %s]",
        GLineShell.c_str(), GroupStr.c_str(), MsgArg.c_str(), TypeStr.c_str(), IPStr.c_str() );
#ifndef _WIN32
    const char* args[] = {
        GLineShell.c_str(), GroupStr.c_str(), MsgArg.c_str(),
        TypeStr.c_str(), IPStr.c_str(), NULL
    };
    RunShellScript( args );
#endif
}
//------------------------------------------------------------------------------
void SendLineNotifyMessage( LineMsgTypeEnum Type, const UFC::AnsiString& Message )
{
    UFC::AnsiString TypeStr;
    UFC::AnsiString MsgArg;

    TypeStr.Printf( "%d", (int)Type );
    MsgArg.Printf( "\nIP: %s \nHostname: %s \nMessage: %s",
        PSocket::GetLocalIP().c_str(), UFC::Hostname, Message.c_str() );

    UFC::BufferedLog::Printf(" Command[sh %s %s ...]",
        GLineNotifyShell.c_str(), TypeStr.c_str() );
#ifndef _WIN32
    const char* args[] = {
        "sh", GLineNotifyShell.c_str(), TypeStr.c_str(), MsgArg.c_str(), NULL
    };
    RunShellScript( args );
#endif
}
//------------------------------------------------------------------------------
void SendLineNotifyMessage( LineMsgTypeEnum Type, const UFC::AnsiString& Message, const UFC::AnsiString& NotifyShellName )
{
    UFC::AnsiString TypeStr;
    UFC::AnsiString MsgArg;

    TypeStr.Printf( "%d", (int)Type );
    MsgArg.Printf( "\nIP: %s \nHostname: %s \nMessage: %s",
        PSocket::GetLocalIP().c_str(), UFC::Hostname, Message.c_str() );

    UFC::BufferedLog::Printf(" Command[sh %s %s ...]",
        NotifyShellName.c_str(), TypeStr.c_str() );
#ifndef _WIN32
    const char* args[] = {
        "sh", NotifyShellName.c_str(), TypeStr.c_str(), MsgArg.c_str(), NULL
    };
    RunShellScript( args );
#endif
}
//------------------------------------------------------------------------------------------------------------------------
void BeginTickus( void )
{
    Tickus = UFC::GetTickCountUS();
}
//------------------------------------------------------------------------------------------------------------------------
Int32 EndTickus( void )
{
    return  (Int32)(UFC::GetTickCountUS() - Tickus );
}
//------------------------------------------------------------------------------------------------------------------------
inline void LocalTime_hms( time_t tv_sec, struct tm *pttm )
{
    int Today_sec = tv_sec % 86400;
    
    pttm->tm_hour =  (((Today_sec / 3600 ) + GTimeZone ) % 24 + 24) % 24;
    pttm->tm_min  =  ( Today_sec % 3600 ) / 60;
    pttm->tm_sec  =  Today_sec % 60;    
}
//------------------------------------------------------------------------------
void GetSignalMessage( int Sig, char* Msg )
{
#ifdef _WIN32
	sprintf( Msg, "Sig=%d", Sig );
#else
	switch(Sig)
	{
		case 	SIGHUP		:  strcpy( Msg, "Hangup (POSIX)." );  break;
		case 	SIGINT		:  strcpy( Msg, "Interrupt (ANSI)." );  break;
		case 	SIGQUIT		:  strcpy( Msg, "Quit (POSIX)." );  break;
		case 	SIGILL		:  strcpy( Msg, "Illegal instruction (ANSI)." );  break;
		case 	SIGTRAP		:  strcpy( Msg, "Trace trap (POSIX)." );  break;
		case 	SIGABRT		:  strcpy( Msg, "Abort (ANSI)." );  break;
		//case 	SIGIOT		:  strcpy( Msg, "IOT trap (4.2 BSD)." );  break;
		case 	SIGBUS		:  strcpy( Msg, "BUS error (4.2 BSD)." );  break;
		case 	SIGFPE		:  strcpy( Msg, "Floating-point exception (ANSI)." );  break;
		case 	SIGKILL		:  strcpy( Msg, "Kill, unblockable (POSIX)." );  break;
		case 	SIGUSR1		:  strcpy( Msg, "User-defined signal 1 (POSIX)." );  break;
		case 	SIGSEGV		:  strcpy( Msg, "Segmentation violation (ANSI)." );  break;
		case 	SIGUSR2		:  strcpy( Msg, "User-defined signal 2 (POSIX)." );  break;
		case 	SIGPIPE		:  strcpy( Msg, "Broken pipe (POSIX)." );  break;
		case 	SIGALRM		:  strcpy( Msg, "Alarm clock (POSIX)." );  break;
		case 	SIGTERM		:  strcpy( Msg, "Termination (ANSI)." );  break;
#ifdef __LINUX
		case 	SIGSTKFLT	:  strcpy( Msg, "Stack fault." );  break;
#endif
		//case 	SIGCLD		:  strcpy( Msg, "Same as SIGCHLD (System V).";  break;
		case 	SIGCHLD		:  strcpy( Msg, "Child status has changed (POSIX)." );  break;
		case 	SIGCONT		:  strcpy( Msg, "Continue (POSIX)." );  break;
		case 	SIGSTOP		:  strcpy( Msg, "Stop, unblockable (POSIX)." );  break;
		case 	SIGTSTP		:  strcpy( Msg, "Keyboard stop (POSIX)." );  break;
		case 	SIGTTIN		:  strcpy( Msg, "Background read from tty (POSIX)." );  break;
		case 	SIGTTOU		:  strcpy( Msg, "Background write to tty (POSIX)." );  break;
		case 	SIGURG		:  strcpy( Msg, "Urgent condition on socket (4.2 BSD)." );  break;
		case 	SIGXCPU		:  strcpy( Msg, "CPU limit exceeded (4.2 BSD)." );  break;
		case 	SIGXFSZ		:  strcpy( Msg, "File size limit exceeded (4.2 BSD)." );  break;
		case 	SIGVTALRM	:  strcpy( Msg, "Virtual alarm clock (4.2 BSD)." );  break;
		case 	SIGPROF		:  strcpy( Msg, "Profiling alarm clock (4.2 BSD)." );  break;
		case 	SIGWINCH	:  strcpy( Msg, "Window size change (4.3 BSD, Sun)." );  break;
		//case 	SIGPOLL		:  strcpy( Msg, "Pollable event occurred (System V)." );  break;
		case 	SIGIO		:  strcpy( Msg, "now possible (4.2 BSD)." );  break;
		case 	SIGPWR		:  strcpy( Msg, "Power failure restart (System V)." );  break;
		case    SIGSYS		:  strcpy( Msg, "Bad system call." );  break;
		default             :  strcpy( Msg, "Unknown Signal" );    break ;
	} //end switch
#endif
}
//---------------------------------------------------------------------------
void OnSignal(int Sig)
{
    char Msg[ 64 ];

    GetSignalMessage( Sig, Msg );
    UFC::BufferedLog::Printf(" Receive signal:%d, %s", Sig, Msg );
    UFC::BufferedLog::FlushToFile();
    fflush(stdout);
    exit(0);
}
//---------------------------------------------------------------------------
void OnSignalNotExit(int Sig)
{
    char Msg[ 64 ];

    GetSignalMessage( Sig, Msg );
    UFC::BufferedLog::Printf(" Receive signal:%d, %s", Sig, Msg );
}
//---------------------------------------------------------------------------
#ifndef _WIN32
//---------------------------------------------------------------------------
void OnSignalCoredump(int Sig)
{
    char Msg[ 64 ];

    GetSignalMessage( Sig, Msg );
    UFC::BufferedLog::Printf(" Receive signal:%d, %s", Sig, Msg );
    UFC::BufferedLog::FlushToFile();
    fflush(stdout);
    ///<  coredmp
    signal( Sig, SIG_DFL );
    raise(Sig) ;
}
//---------------------------------------------------------------------------
BOOL SetSignalHandler(int Sig, __sighandler_t __handler)
{
    if (signal( Sig, __handler ) == SIG_ERR)
    {
        char Msg[ 64 ];

        GetSignalMessage( Sig, Msg );
        printf(" Set Signal Handler fail: %d(%s)\n", Sig, Msg );
        return FALSE;
    }
    return TRUE;
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
PEndian::PEndian()
{
    UInt8  LowHi[ 2 ] = { 0,0xFF }; ///< Low: 0 High:FF
    UInt16 Value = 0xFF00;          ///< Value: 0xFF00

    if( memcmp( LowHi, &Value, 2 ) == 0 )
        FIsLittleEndian = TRUE;
    else
        FIsLittleEndian = FALSE;
    #ifndef _WIN32
        pthread_attr_t attr;

        pthread_attr_init(&attr);
        pthread_attr_getstacksize (&attr, &FThreadStackSize);
            #ifdef __SOLARIS
               if( FThreadStackSize == 0 )
                       FThreadStackSize = 1024*1024;///< Default 1Mb
            #endif
        pthread_attr_destroy(&attr);
    #endif
}
//-------------------------------------------------------------------------------------------------------------------
void PInitNetLib::Sleep1US( void )
{
	struct timeval tval;
	tval.tv_sec  = 0;
	tval.tv_usec = 1;
    select( 0, NULL, NULL, NULL, &tval );
}
//---------------------------------------------------------------------------
void PInitNetLib::Now( )
{
#ifndef _WIN32
    struct tm    FTime;
    struct timespec now;    
    
    clock_gettime( CLOCK_REALTIME_COARSE, &now );
    gmtime_r( &now.tv_sec, &FTime );    
    printf( "[UFCLib] Timet in sec:%ld\n", (long)now.tv_sec );
    printf( "[UFCLib] UTC Time:%04d%02d%02d-%02d:%02d:%02d\n",
				   FTime.tm_year+1900, FTime.tm_mon+1, FTime.tm_mday,
				   FTime.tm_hour, FTime.tm_min, FTime.tm_sec );        
    GYear  = FTime.tm_year+1900;
    GMonth = FTime.tm_mon+1;
    GDay   = FTime.tm_mday;
    FLastSec = (now.tv_sec / 86400 + 1) * 86400 - 1;
    printf( "[UFCLib] Today last sec:%ld\n", (long)FLastSec );
#endif
}
//---------------------------------------------------------------------------
void PInitNetLib::CheckNextDay( time_t& Time )
{
#ifndef _WIN32
    if( Time >=  FLastSec )
    {
        struct tm    FTime;

        gmtime_r( &Time, &FTime );
        GYear  = FTime.tm_year+1900;
        GMonth = FTime.tm_mon+1;
        GDay   = FTime.tm_mday;
        FLastSec = (Time / 86400 + 1) * 86400 - 1;
    }
#endif
}
//---------------------------------------------------------------------------
PInitNetLib::PInitNetLib()
{
    #ifdef _WIN32
    WSADATA wsaData;
    WORD    wVersionRequested = MAKEWORD( 2, 2 );
    int     err = WSAStartup( wVersionRequested, &wsaData );

    if( err != 0 )
    {
            printf( "Windows: Initialize Socket library failed.\n" );
            exit( -1 );
    }	
    #else
    if (signal( SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        printf( "UNIX: Can't ignore SIGPIPE signal.\n");
        exit(-1);
    }      
        #ifndef NOT_HANDLE_SIGNAL
        for (int Sig=SIGHUP ; Sig<SIGUNUSED ; Sig++)
        {
            switch (Sig)
            {
                ///< Exit
                case SIGINT:                              
                case SIGQUIT:                    
                case SIGTERM:
                #ifdef __LINUX
                case SIGSTKFLT:
                #endif
                             SetSignalHandler( Sig, OnSignal); 
                             break;                                                          
                ///< Coredump
                case SIGHUP:                                      
                case SIGILL:
                case SIGABRT:
                case SIGBUS:
                case SIGFPE:
                case SIGSEGV:
                case SIGSYS: ///< SIGSYS is no effect !
                             SetSignalHandler( Sig, OnSignalCoredump );        
                             break;                                          
                ///< Log only                             
                case SIGCONT:
                case SIGURG:
                case SIGXCPU:
                case SIGXFSZ:
                case SIGALRM:
                case SIGVTALRM:
                case SIGPROF:
                case SIGIO:
                case SIGPWR: SetSignalHandler( Sig, OnSignalNotExit );  
                             break;                             
                default:     break;                   
            }//end switch
        }
        #endif
    #endif

    if( gethostname( Hostname, 64 ) != 0 )
    {
        printf( "Can't get the host name.\n");
        exit(-1);
    }

#if defined(__LINUX) || defined(__AIX) || defined(__SOLARIS)
    UInt64 Begin = UFC::GetTickCountUS();

    for( int j = 0; j < 1000; j++ ) ///< 1000us = 1ms
         UFC::SleepUS( 1 );

    UInt64 Diff = UFC::GetTickCountUS() - Begin;
    GMinSleepUS = (int)(Diff/1000);

#ifdef GCC_CPP11_SUPPORT
    UFC::PMPMCQueue<int>  FQueue( 10, 10  );
    int    loopTimes = 1000000;
    UInt64 useus;
    int*   Obj;

    Begin = UFC::GetTickCountUS();
    for( int i = 0; i < loopTimes;i++ )
         FQueue.pop( 0, &Obj );
    useus = UFC::GetTickCountUS() - Begin;
    if( useus > 0 )
        GDeqPerUS = loopTimes / (int)useus;
#endif    
#endif    
    
#if defined( _WIN32 )
	GetCurrentDirectoryA( PATH_MAX, WorkingDir );
#elif defined( __LINUX )
	if( getcwd( WorkingDir, PATH_MAX ) == NULL )
            strncpy( WorkingDir, "./", 3 );
#else
	getwd( WorkingDir );
#endif
    
    if( UFC::FileExists( "../cfg/UFCLib.cfg" ) == TRUE )
    {
        UFC::UiniFile   Config( "../cfg/UFCLib.cfg" );
        UFC::AnsiString Value;
        int OpenH = 5;
        int OpenM = 50;
        int CloseH = 5;
        int CloseM = 15;

        if( Config.GetValue( "Setting", "OpenH",Value ) == TRUE )
            OpenH = Value.ToInt();
        if( Config.GetValue( "Setting", "OpenM",Value ) == TRUE )
            OpenM = Value.ToInt();
        if( Config.GetValue( "Setting", "CloseH",Value ) == TRUE )
            CloseH = Value.ToInt();
        if( Config.GetValue( "Setting", "CloseM",Value ) == TRUE )
            CloseM = Value.ToInt();
        SetTradeingHour( OpenH, OpenM, CloseH, CloseM );
        printf( "[UFCLib] Set Trading Hours[%02d:%02d]~[%02d:%02d]\n", GOpenTime/100, GOpenTime%100, GCloseTime/100, GCloseTime%100);
        if( Config.GetValue( "Setting", "TimeZone",Value ) == TRUE )
            GTimeZone = Value.ToInt();
        if( Config.GetValue( "Setting", "LogUseus",Value ) == TRUE )
            GLog_us = Value.ToInt();
        if( Config.GetValue( "Setting", "LogThreads",Value ) == TRUE )                
            GLogThread = Value.ToInt();
        if( Config.GetValue( "Setting", "Line",Value ) == TRUE )
            GLineGroup = Value.ToInt();
        else
            GLineGroup = 1;
        if( Config.GetValue( "Setting", "LineShell", GLineShell ) == FALSE )
            GLineShell = "./cloud_monitor.sh";
        if( Config.GetValue( "Setting", "LineNotifyShell", GLineNotifyShell ) == FALSE )
            GLineNotifyShell = "./line_notify_monitor.sh";
    }
    else
    {
        printf( "[UFCLib] Default Trading Hours[%02d:%02d]~[%02d:%02d]\n", GOpenTime/100, GOpenTime%100, GCloseTime/100, GCloseTime%100);
        GLineGroup = 1;
    }
#if defined(__LINUX) || defined(__AIX) || defined(__SOLARIS)
    printf( "[UFCLib] minimum sleep %dus\n", GMinSleepUS );
    printf( "[UFCLib] dequeue %d times per us\n", GDeqPerUS );
#endif
    printf( "[UFCLib] Time zone:%d\n", GTimeZone );
    printf( "[UFCLib] Log use us:%s\n", GLog_us ? "TRUE" : "FALSE" );    
    printf( "[UFCLib] Log Thread:%s\n", GLogThread ? "TRUE" : "FALSE" );    
    printf( "[UFCLib] Line shell:%s\n", GLineShell.c_str() );    
    printf( "[UFCLib] Line group:%d\n", GLineGroup );    
    printf( "[UFCLib] Working Dir:%s\n", WorkingDir );  
    Now( );
    UFC::BufferedLog::EnableLog_us( GLog_us );
}
//---------------------------------------------------------------------------
//
//	NInt32
//
//---------------------------------------------------------------------------
NInt32::NInt32()
:FInt32( 0 )
{
}
//---------------------------------------------------------------------------
NInt32::NInt32( Int32 I )
{
    Assign( I );
}
//---------------------------------------------------------------------------
Int32 NInt32::ToInt32() const
{
    if( Endian.IsLittleEndian() )
        return SWAP_LONG( FInt32 ); ///< Convert to Big-Endin
    else
        return FInt32;
}
//---------------------------------------------------------------------------
void NInt32::Assign( Int32 I )
{
    if( Endian.IsLittleEndian() )
        FInt32 = SWAP_LONG(I);    ///< Convert to Big-Endin
    else
        FInt32 = I;
}
//---------------------------------------------------------------------------
int NInt32::LoadFromStream( PStream* Stream )
{
    return Stream->Read( &FInt32, 4 );
}
//---------------------------------------------------------------------------
int NInt32::SaveToStream( PStream* Stream )
{
    return Stream->Write( &FInt32, 4 );
}
//---------------------------------------------------------------------------
Int32 NInt32::ToInt32( UInt8* Data )
{
    Int32 Value;
    memcpy( &Value, Data, 4 );
    if( Endian.IsLittleEndian() )
        return SWAP_LONG( Value );
    return Value;
}
//---------------------------------------------------------------------------
void NInt32::ToInt32( Int32 Value, UInt8* Data )
{
    if( Endian.IsLittleEndian() )
    {
        Int32 SwapValue = SWAP_LONG( Value );
        memcpy( Data, &SwapValue, 4 );
    }
    else
        memcpy( Data, &Value, 4 );
}
//---------------------------------------------------------------------------
NInt32& NInt32::operator = ( Int32 Value )
{
    Assign( Value );
	return *this;
}
//---------------------------------------------------------------------------
//
//	NInt16
//
//---------------------------------------------------------------------------
NInt16::NInt16()
:FInt16( 0 )
{
}
//---------------------------------------------------------------------------
NInt16::NInt16( Int16 I )
{
    Assign( I );
}
//---------------------------------------------------------------------------
Int16 NInt16::ToInt16() const
{
    if( Endian.IsLittleEndian() )
        return SWAP_SHORT( FInt16 ); ///< Convert to Big-Endin
    else
        return FInt16;
}
//---------------------------------------------------------------------------
void NInt16::Assign( Int16 I )
{
    if( Endian.IsLittleEndian() )
        FInt16 = SWAP_SHORT(I);    ///< Convert to Big-Endin
    else
        FInt16 = I;
}
//---------------------------------------------------------------------------
int  NInt16::LoadFromStream( PStream* Stream )
{
    return Stream->Read( &FInt16, 2 );
}
//---------------------------------------------------------------------------
int  NInt16::SaveToStream( PStream* Stream )
{
    return Stream->Write( &FInt16, 2 );
}
//---------------------------------------------------------------------------
Int16 NInt16::ToInt16( UInt8* Data )
{
    Int16 Value;

    memcpy( &Value, Data, 2 );
    if( Endian.IsLittleEndian() )
        return SWAP_SHORT( Value );
    return Value;
}
//---------------------------------------------------------------------------
void NInt16::ToInt16( Int16 Value, UInt8* Data )
{
    if( Endian.IsLittleEndian() )
    {
        Int16 SwapValue = SWAP_SHORT( Value );
        memcpy( Data, &SwapValue, 2 );
    }
    else
        memcpy( Data, &Value, 2 );
}
//---------------------------------------------------------------------------
NInt16& NInt16::operator = ( Int16 Value )
{
    Assign( Value );
	return *this;
}
//---------------------------------------------------------------------------
//
//	NInt64
//
//---------------------------------------------------------------------------
NInt64::NInt64()
:FInt64( 0 )
{
}
//---------------------------------------------------------------------------
NInt64::NInt64( Int64 I )
{
    Assign( I );
}
//---------------------------------------------------------------------------
Int64 NInt64::ToInt64() const
{
    return NInt64::SwapInt64( FInt64 );
}
//---------------------------------------------------------------------------
void NInt64::Assign( Int64 I )
{
    FInt64 = NInt64::SwapInt64( I );
}
//---------------------------------------------------------------------------
int NInt64::LoadFromStream( PStream* Stream )
{
    return Stream->Read( &FInt64, 8 );
}
//---------------------------------------------------------------------------
int NInt64::SaveToStream( PStream* Stream )
{
    return Stream->Write( &FInt64, 8 );
}
//---------------------------------------------------------------------------
Int64 NInt64::ToInt64( UInt8* Data )
{
    Int64 Value;
    memcpy( &Value, Data, 8 );
    return NInt64::SwapInt64( Value );
}
//---------------------------------------------------------------------------
Int64 NInt64::SwapInt64(Int64 inValue)
{
    if( Endian.IsLittleEndian() )
    {
        UInt64 swapValue;
        
        memcpy( &swapValue, &inValue, 8 );
        swapValue = SWAP_64( swapValue ); ///< Convert to Big-Endin
        return (Int64)swapValue;
    }
    return inValue;
}
//---------------------------------------------------------------------------
NInt64& NInt64::operator = ( Int64 Value )
{
    Assign( Value );
    return *this;
}
//---------------------------------------------------------------------------
//
//	NDouble
//
//---------------------------------------------------------------------------
NDouble::NDouble()
:FDouble( 0 )
{
}
//---------------------------------------------------------------------------
NDouble::NDouble( double I )
{
    Assign( I );
}
//---------------------------------------------------------------------------
double NDouble::ToDouble() const
{
    return NDouble::SwapDouble( FDouble );
}
//---------------------------------------------------------------------------
void NDouble::Assign( double I )
{
    FDouble = NDouble::SwapDouble( I );
}
//---------------------------------------------------------------------------
int NDouble::LoadFromStream( PStream* Stream )
{
    return Stream->Read( &FDouble, 8 );
}
//---------------------------------------------------------------------------
int NDouble::SaveToStream( PStream* Stream )
{
    return Stream->Write( &FDouble, 8 );
}
//---------------------------------------------------------------------------
double NDouble::ToDouble( UInt8* Data )
{
    double Value;
    memcpy( &Value, Data, 8 );
    return NDouble::SwapDouble( Value );
}
//---------------------------------------------------------------------------
double NDouble::SwapDouble(double inValue)
{
    if (Endian.IsLittleEndian())
    {
        UInt64 swapValue;
        double rtValue;

        memcpy(&swapValue, &inValue, 8);
        swapValue = SWAP_64(swapValue); ///< Convert to Big-Endin
        memcpy(&rtValue, &swapValue, 8);

        return rtValue;
    }
    return inValue;
}
//---------------------------------------------------------------------------
NDouble& NDouble::operator = ( double Value )
{
    Assign( Value );
    return *this;
}
//---------------------------------------------------------------------------
//  
//  Time functions
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
inline int ns2us( long int  nsec )
{
    return (int)nsec/1000;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
inline int ns2ms( long int  nsec )
{
    return (int)nsec/1000000;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UInt32 GetTickCountMS( void )
{
#ifdef _WIN32
    return GetTickCount();
#else
    UInt32 Tick;
    #ifdef _USE_CLOCK_GETTIME
    struct timespec now;
    clock_gettime( CLOCK_REALTIME_COARSE ,&now ); ///< Get the current time. 
    Tick = ((UInt32)(now.tv_sec % 86400)) * 1000 + ns2ms( now.tv_nsec );
    #else
    struct timeb t;
    ftime( &t );
    Tick = t.time * 1000 + t.millitm;
    #endif
    return Tick;
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UInt64 GetTickCountUS( void )
{
#ifdef _WIN32
    static UInt64 Feq = 0;
    UInt64 Count;

    if( Feq == 0 )
        QueryPerformanceFrequency( (LARGE_INTEGER*)&Feq );
    if( QueryPerformanceCounter( (LARGE_INTEGER*)&Count ) )
        return Count / Feq * 1000000 + (Count % Feq) * 1000000 / Feq;
    else
        return (UInt64)GetTickCount() * 1000;
#else
    UInt64 Tick;
    #ifdef _USE_CLOCK_GETTIME
    struct timespec now;
    clock_gettime( CLOCK_MONOTONIC, &now ); ///< Get the current time.  CLOCK_MONOTONIC resolution is nanosecond.
    Tick = (UInt64)now.tv_sec * 1000000 + ns2us(now.tv_nsec);
    #else
    struct timeval now;
    gettimeofday( &now, NULL ); ///< Get the current time.
    Tick = (UInt64)now.tv_sec * 1000000 + now.tv_usec;
    #endif
    return Tick;
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UInt64 GetHHMMSSmmmuuu( void )
{
    UInt64 Result = 0ULL;
#ifndef _WIN32
    struct tm    time;
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.        
    LocalTime_hms( now.tv_sec, &time ); ///< convert to local time    
    Result = ((Int64)time.tm_hour * 10000000000LL) + ((Int64)time.tm_min * 100000000LL) + ((Int64)time.tm_sec * 1000000LL) + ns2us(now.tv_nsec);
#endif
    return Result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetTimeString_us( AnsiString& TimeNow , BOOL WithSep )
{
#ifdef _WIN32
    SYSTEMTIME SystemTime;
    GetLocalTime( &SystemTime );
    if( WithSep == TRUE )
        TimeNow.Printf( "%02d:%02d:%02d.%06d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds*1000 );
    else
        TimeNow.Printf( "%02d%02d%02d%06d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds*1000 );
#else
    struct tm    time;
    #ifdef _USE_CLOCK_GETTIME   
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.        
    LocalTime_hms( now.tv_sec, &time ); ///< convert to local time
    if( WithSep == TRUE )
        TimeNow.Printf( "%02d:%02d:%02d.%06d", time.tm_hour, time.tm_min, time.tm_sec, ns2us(now.tv_nsec) );
    else
        TimeNow.Printf( "%02d%02d%02d%06d", time.tm_hour, time.tm_min, time.tm_sec, ns2us(now.tv_nsec) );
    #else
    struct timeval now;

    gettimeofday( &now, NULL ); ///< Get the current time.
    LocalTime_hms( now.tv_sec, &time ); ///< convert to local time
    if( WithSep == TRUE )
        TimeNow.Printf( "%02d:%02d:%02d.%06d", time.tm_hour, time.tm_min, time.tm_sec, now.tv_usec );
    else
        TimeNow.Printf( "%02d%02d%02d%06d", time.tm_hour, time.tm_min, time.tm_sec, now.tv_usec );
     #endif
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetTimeString( AnsiString& TimeNow , BOOL WithSep )
{
#ifdef _WIN32
    SYSTEMTIME SystemTime;
    GetLocalTime( &SystemTime );
    if( WithSep == TRUE )
        TimeNow.Printf( "%02d:%02d:%02d.%03d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds );
    else
        TimeNow.Printf( "%02d%02d%02d%03d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds );
#else
    struct tm    FTime;
    #ifdef _USE_CLOCK_GETTIME   
    struct timespec now;
    //struct timeb tb;
    clock_gettime( CLOCK_REALTIME_COARSE, &now );
    LocalTime_hms( now.tv_sec, &FTime );
    if( WithSep == TRUE )
        TimeNow.Printf( "%02d:%02d:%02d.%03d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec, ns2ms(now.tv_nsec) );
    else
        TimeNow.Printf( "%02d%02d%02d%03d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec, ns2ms(now.tv_nsec) );
    #else    
    struct timeb tb;

    ftime( &tb );
    LocalTime_hms( tb.time, &FTime );
    if( WithSep == TRUE )
        TimeNow.Printf( "%02d:%02d:%02d.%03d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec, tb.millitm );
    else
        TimeNow.Printf( "%02d%02d%02d%03d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec, tb.millitm );
    #endif
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
inline void TimeString( char* Buffer , int h, int m,int s, int ms, BOOL WithSep, BOOL Useus )
{
    register char* Ptr;
    register int i, tmpval;
    int msus;
    
    if( Useus == TRUE )
        msus = 6; ///< us 6 digis
    else
        msus = 3; ///< ms 3 digis   
    if( WithSep == TRUE )
    {
        Ptr = Buffer + 9 + msus;
        *Ptr-- = 0; //< NULL
        for( i = 0; i< msus ;i ++ )
        {
            tmpval = ms;
            ms /= 10;
            *Ptr-- = '0' + (tmpval - ms * 10);        
        }                
        *Ptr-- = '.';
        for( i = 0; i< 2 ;i ++ )
        {
            tmpval = s;
            s /= 10;
            *Ptr-- = '0' + (tmpval - s * 10);        
        }            
        *Ptr-- = ':';    
        for( i = 0; i< 2 ;i ++ )
        {
            tmpval = m;
            m /= 10;
            *Ptr-- = '0' + (tmpval - m * 10);        
        }            
        *Ptr-- = ':';    
        for( i = 0; i< 2 ;i ++ )
        {
            tmpval = h;
            h /= 10;
            *Ptr-- = '0' + (tmpval - h * 10);        
        }
    }
    else
    {
        Ptr = Buffer + 6 + msus;
        *Ptr-- = 0; //< NULL
        for( i = 0; i< msus ;i ++ )
        {
            tmpval = ms;
            ms /= 10;
            *Ptr-- = '0' + (tmpval - ms * 10);        
        }                        
        for( i = 0; i< 2 ;i ++ )
        {
            tmpval = s;
            s /= 10;
            *Ptr-- = '0' + (tmpval - s * 10);        
        }                    
        for( i = 0; i< 2 ;i ++ )
        {
            tmpval = m;
            m /= 10;
            *Ptr-- = '0' + (tmpval - m * 10);        
        }                        
        for( i = 0; i< 2 ;i ++ )
        {
            tmpval = h;
            h /= 10;
            *Ptr-- = '0' + (tmpval - h * 10);        
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetUTCTimeString( char* TimeNow, BOOL WithSep )
{
#ifndef _WIN32
    struct tm    FTime;
    struct timespec now;    
    
    clock_gettime( CLOCK_REALTIME_COARSE, &now );
    gmtime_r( &now.tv_sec, &FTime );    
    TimeString( TimeNow , FTime.tm_hour, FTime.tm_min, FTime.tm_sec, ns2ms(now.tv_nsec), WithSep, FALSE );        
#endif
}    
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
inline void FIXTimeString( char* Buffer , int Y, int M, int D, int h, int m,int s, int ms )
{
///<   YYYYMMDD-HH:MM:SS.mmm
///<          9+       9+  3     
    register char* Ptr;
    register int i, tmpval;
    const int DateTimeL = 18;
    const int msus = 3; ///< ms 3 digis   
    
    h -= GTimeZone;
    if( h < 0 )
        h += 24;
    else if( h >= 24 )
        h -= 24;

    Ptr = Buffer + DateTimeL + msus;
    *Ptr-- = 0; //< NULL
    for( i = 0; i< msus ;i ++ )
    {
        tmpval = ms;
        ms /= 10;
        *Ptr-- = '0' + (tmpval - ms * 10);        
    }                
    *Ptr-- = '.';
    for( i = 0; i< 2 ;i ++ )
    {
        tmpval = s;
        s /= 10;
        *Ptr-- = '0' + (tmpval - s * 10);        
    }            
    *Ptr-- = ':';    
    for( i = 0; i< 2 ;i ++ )
    {
        tmpval = m;
        m /= 10;
        *Ptr-- = '0' + (tmpval - m * 10);        
    }            
    *Ptr-- = ':';    
    for( i = 0; i< 2 ;i ++ )
    {
        tmpval = h;
        h /= 10;
        *Ptr-- = '0' + (tmpval - h * 10);        
    }
    *Ptr-- = '-';    
    for( i = 0; i< 2 ;i ++ )
    {
        tmpval = D;
        D /= 10;
        *Ptr-- = '0' + (tmpval - D * 10);        
    }    
    for( i = 0; i< 2 ;i ++ )
    {
        tmpval = M;
        M /= 10;
        *Ptr-- = '0' + (tmpval - M * 10);        
    }
    for( i = 0; i< 4 ;i ++ )
    {
        tmpval = Y;
        Y /= 10;
        *Ptr-- = '0' + (tmpval - Y * 10);        
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void localtime_fast_r( bool IsGMT, struct tm* FTime )
{
#ifndef _WIN32
    struct timespec now;

    clock_gettime( CLOCK_REALTIME, &now );
    time_t tt = (time_t)now.tv_sec;

    InitNetLib.CheckNextDay( tt );

    int Today_sec = now.tv_sec % 86400;

    if( IsGMT )
        FTime->tm_hour = Today_sec / 3600;
    else
        FTime->tm_hour = (((Today_sec / 3600) + GTimeZone) % 24 + 24) % 24;
    FTime->tm_min  = ( Today_sec % 3600 ) / 60;
    FTime->tm_sec  = Today_sec % 60;
    FTime->tm_year = GYear;
    FTime->tm_mon  = GMonth;
    FTime->tm_mday = GDay;
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetFIXTimeString( char* TimeNow )
{
#ifndef _WIN32
    struct tm    FTime;
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME_COARSE, &now );    
    time_t tt = (time_t)now.tv_sec;     
    InitNetLib.CheckNextDay( tt );    

    LocalTime_hms( now.tv_sec, &FTime ); 
    FIXTimeString( TimeNow, GYear, GMonth, GDay, FTime.tm_hour, FTime.tm_min, FTime.tm_sec, ns2ms(now.tv_nsec) );    
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetTimeString( char* TimeNow , BOOL WithSep )
{
#ifdef _WIN32
    SYSTEMTIME SystemTime;
    GetLocalTime( &SystemTime );
    if( WithSep == TRUE )
        sprintf( TimeNow,"%02d:%02d:%02d.%03d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds );
    else
        sprintf( TimeNow,"%02d%02d%02d%03d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds );
#else
    struct tm    FTime;
    #ifdef _USE_CLOCK_GETTIME       
    struct timespec now;
    clock_gettime( CLOCK_REALTIME_COARSE, &now );
    LocalTime_hms( now.tv_sec, &FTime );
    TimeString( TimeNow , FTime.tm_hour, FTime.tm_min, FTime.tm_sec, ns2ms(now.tv_nsec), WithSep, FALSE );    
    #else    
    struct timeb tb;
    ftime( &tb );
    LocalTime_hms( tb.time, &FTime );
    TimeString( TimeNow , FTime.tm_hour, FTime.tm_min, FTime.tm_sec, tb.millitm, WithSep, FALSE );  
    #endif
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetTimeString_us( char* TimeNow , BOOL WithSep )
{
#ifdef _WIN32
    SYSTEMTIME SystemTime;
    GetLocalTime( &SystemTime );
    if( WithSep == TRUE )
            sprintf( TimeNow ,"%02d:%02d:%02d.%06d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds*1000 );
    else
            sprintf( TimeNow ,"%02d%02d%02d%06d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds*1000 );
#else
    struct tm    time;
    #ifdef _USE_CLOCK_GETTIME       
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.            
    LocalTime_hms( now.tv_sec, &time ); ///< convert to local time
    TimeString( TimeNow ,  time.tm_hour, time.tm_min, time.tm_sec,ns2us(now.tv_nsec), WithSep, TRUE );    
    #else    
    struct timeval now;
    
    gettimeofday( &now, NULL ); ///< Get the current time.
    LocalTime_hms( now.tv_sec, &time ); ///< convert to local time
    TimeString( TimeNow ,  time.tm_hour, time.tm_min, time.tm_sec,(int) now.tv_usec, WithSep, TRUE );
    #endif
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetTimeString_ms_us( char* TimeNow , BOOL Use_us, BOOL WithSep )
{
#ifdef _WIN32
    SYSTEMTIME SystemTime;
    GetLocalTime( &SystemTime );
    if( Use_us == TRUE )
    {
        if( WithSep == TRUE )
            sprintf( TimeNow ,"%02d:%02d:%02d.%06d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds*1000 );
        else
            sprintf( TimeNow ,"%02d%02d%02d%06d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds*1000 );
    }
    else
    {
        if( WithSep == TRUE )
            sprintf( TimeNow ,"%02d:%02d:%02d.%03d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds );
        else
            sprintf( TimeNow ,"%02d%02d%02d%03d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds );
    }    
#else
    struct tm     time;
    int msus;        
    #ifdef _USE_CLOCK_GETTIME       
    struct timespec now;
    
    if( Use_us == TRUE )
    {
        clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.        
        LocalTime_hms( now.tv_sec, &time ); ///< convert to local time        
        msus = ns2us( now.tv_nsec );
    }
    else
    {
        clock_gettime( CLOCK_REALTIME_COARSE ,&now ); ///< Get the current time.        
        LocalTime_hms( now.tv_sec, &time ); ///< convert to local time        
        msus = ns2ms( now.tv_nsec );
    }
    TimeString( TimeNow ,  time.tm_hour, time.tm_min, time.tm_sec, msus, WithSep, Use_us );    
    #else
    struct timeval now;

    gettimeofday( &now, NULL ); ///< Get the current time.
    LocalTime_hms( now.tv_sec, &time ); ///< convert to local time        
    if( Use_us == TRUE )
        msus = (int) now.tv_usec;
    else
        msus = (int) now.tv_usec/1000;
    TimeString( TimeNow ,  time.tm_hour, time.tm_min, time.tm_sec, msus, WithSep, Use_us );    
    #endif
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetHHMMSSmm( AnsiString& TimeNow )
{
#ifdef _WIN32
    SYSTEMTIME SystemTime;
    GetLocalTime( &SystemTime );
    TimeNow.Printf( "%02d%02d%02d%02d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds/10 );
#else
    struct tm    FTime;
    #ifdef _USE_CLOCK_GETTIME       
    struct timespec now;
    clock_gettime( CLOCK_REALTIME_COARSE, &now );
    LocalTime_hms( now.tv_sec, &FTime );
    TimeNow.Printf( "%02d%02d%02d%02d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec, ns2ms(now.tv_nsec)/10 );
    #else    
    struct timeb tb;
    ftime( &tb );
    LocalTime_hms( tb.time, &FTime );
    TimeNow.Printf( "%02d%02d%02d%02d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec, tb.millitm/10 );
    #endif
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetHHMMSSmmm( AnsiString& TimeNow )
{
#ifdef _WIN32
    SYSTEMTIME SystemTime;
    GetLocalTime( &SystemTime );
    TimeNow.Printf( "%02d%02d%02d%03d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds );
#else
    struct tm    FTime;
    #ifdef _USE_CLOCK_GETTIME
    struct timespec now;
    clock_gettime( CLOCK_REALTIME_COARSE, &now );
    LocalTime_hms( now.tv_sec, &FTime );
    TimeNow.Printf( "%02d%02d%02d%03d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec, ns2ms(now.tv_nsec) );
    #else    
    struct timeb tb;
    ftime( &tb );
    LocalTime_hms( tb.time, &FTime );
    TimeNow.Printf( "%02d%02d%02d%03d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec, tb.millitm );
    #endif
#endif
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetHHMMSS( AnsiString& TimeNow )
{
#ifdef _WIN32
    SYSTEMTIME SystemTime;

    GetLocalTime( &SystemTime );
    TimeNow.Printf( "%02d%02d%02d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond );
#else
    struct tm    FTime;
    #ifdef _USE_CLOCK_GETTIME           
    struct timespec now;
    //struct timeb tb;
    clock_gettime( CLOCK_REALTIME_COARSE ,&now ); ///< Get the current time.        
    LocalTime_hms( now.tv_sec, &FTime );
    TimeNow.Printf( "%02d%02d%02d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec );
    #else
    struct timeb tb;

    ftime( &tb );
    LocalTime_hms( tb.time, &FTime );
    TimeNow.Printf( "%02d%02d%02d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec );
#endif
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
inline void TimeString( char* Buffer , int h, int m,int s )
{
    register char* Ptr;
    register int i, tmpval;
    
    Ptr = Buffer + 5;
    //*Ptr-- = 0; //< NULL
    for( i = 0; i< 2 ;i ++ )
    {
        tmpval = s;
        s /= 10;
        *Ptr-- = '0' + (tmpval - s * 10);        
    }                    
    for( i = 0; i< 2 ;i ++ )
    {
        tmpval = m;
        m /= 10;
        *Ptr-- = '0' + (tmpval - m * 10);        
    }                        
    for( i = 0; i< 2 ;i ++ )
    {
        tmpval = h;
        h /= 10;
        *Ptr-- = '0' + (tmpval - h * 10);        
    }    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetHHMMSS_No_Null( char* TimeNow )
{
#ifdef _WIN32
    SYSTEMTIME SystemTime;

    GetLocalTime( &SystemTime );
	sprintf( TimeNow, "%02d%02d%02d", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond );
#else
    struct tm    FTime;
    #ifdef _USE_CLOCK_GETTIME               
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME_COARSE ,&now ); ///< Get the current time.        
    LocalTime_hms( now.tv_sec, &FTime );
    TimeString( TimeNow , FTime.tm_hour, FTime.tm_min, FTime.tm_sec );
    #else
    struct timeb tb;

    ftime( &tb );
    LocalTime_hms( tb.time, &FTime );
    TimeString( TimeNow , FTime.tm_hour, FTime.tm_min, FTime.tm_sec );
    #endif
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetHHMMSS_Null( char* TimeNow )
{
    GetHHMMSS_No_Null( TimeNow );
    TimeNow[ 6 ] = 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int32 GetHHMMSS( void )
{
#ifdef _WIN32
    SYSTEMTIME SystemTime;

    GetLocalTime( &SystemTime );
    return SystemTime.wHour*10000 + SystemTime.wMinute*100 +SystemTime.wSecond;
#else
    struct tm    FTime;
    #ifdef _USE_CLOCK_GETTIME                   
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME_COARSE ,&now ); ///< Get the current time.        
    LocalTime_hms( now.tv_sec, &FTime );
    return FTime.tm_hour*10000 + FTime.tm_min*100 + FTime.tm_sec;
    #else
    struct timeb tb;

    ftime( &tb );
    LocalTime_hms( tb.time, &FTime );
    return FTime.tm_hour*10000 + FTime.tm_min*100 + FTime.tm_sec;
    #endif
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetYYYYMMDDHHMMSSmmm( AnsiString& DTime )
{
#ifdef _WIN32
	SYSTEMTIME SystemTime;
	GetLocalTime( &SystemTime );
	DTime.Printf( "%04d%02d%02d%02d%02d%02d%03d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds );
#else
	struct tm      FTime;
    #ifdef _USE_CLOCK_GETTIME                   	
        struct timespec now;
    
        clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    
	localtime_r( &now.tv_sec, &FTime );
	DTime.Printf( "%04d%02d%02d%02d%02d%02d%03d",
				   FTime.tm_year+1900, FTime.tm_mon+1, FTime.tm_mday,
				   FTime.tm_hour, FTime.tm_min, FTime.tm_sec,
				   ns2ms(now.tv_nsec) );
    #else
    struct timeb tb;

    ftime( &tb );
    localtime_r( &tb.time, &FTime );
    DTime.Printf( "%04d%02d%02d%02d%02d%02d%03d",
                   FTime.tm_year+1900, FTime.tm_mon+1, FTime.tm_mday,
                   FTime.tm_hour, FTime.tm_min, FTime.tm_sec,
                   (int)tb.millitm );
    #endif
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetFIXYYYYMMDDHHMMSSmmm( AnsiString& DTime )
{
#ifdef _WIN32
	SYSTEMTIME SystemTime;
	GetLocalTime( &SystemTime );
	DTime.Printf( "%04d%02d%02d-%02d:%02d:%02d.%03d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds );
#else
	struct tm      FTime;
    #ifdef _USE_CLOCK_GETTIME
        struct timespec now;

        clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.
        localtime_r( &now.tv_sec, &FTime );
        DTime.Printf( "%04d%02d%02d-%02d:%02d:%02d.%03d",
                       FTime.tm_year+1900, FTime.tm_mon+1, FTime.tm_mday,
                       FTime.tm_hour, FTime.tm_min, FTime.tm_sec,
                       ns2ms(now.tv_nsec) );
    #else
    struct timeb tb;

    ftime( &tb );
    localtime_r( &tb.time, &FTime );
    DTime.Printf( "%04d%02d%02d-%02d:%02d:%02d.%03d",
                   FTime.tm_year+1900, FTime.tm_mon+1, FTime.tm_mday,
                   FTime.tm_hour, FTime.tm_min, FTime.tm_sec,
                   (int)tb.millitm );
    #endif
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetYYYYMMDD( AnsiString& Today, BOOL WithSlash )
{
#ifdef _WIN32
	SYSTEMTIME SystemTime;

	GetLocalTime( &SystemTime );
	if( WithSlash == FALSE )
		Today.Printf( "%04d%02d%02d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay );
	else
		Today.Printf( "%04d/%02d/%02d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay );
#else
    #ifdef _USE_CLOCK_GETTIME                   	
	struct tm    FTime;
    struct timespec now;        

    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    
	localtime_r( &now.tv_sec, &FTime );
	if( WithSlash == FALSE )
	    Today.Printf( "%04d%02d%02d", FTime.tm_year+1900, FTime.tm_mon+1, FTime.tm_mday );
	else
	    Today.Printf( "%04d/%02d/%02d", FTime.tm_year+1900, FTime.tm_mon+1, FTime.tm_mday );
    #else
	struct tm    FTime;
	struct timeb tb;

	ftime( &tb );
	localtime_r( &tb.time, &FTime );
	if( WithSlash == FALSE )
		Today.Printf( "%04d%02d%02d", FTime.tm_year+1900, FTime.tm_mon+1, FTime.tm_mday );
	else
		Today.Printf( "%04d/%02d/%02d", FTime.tm_year+1900, FTime.tm_mon+1, FTime.tm_mday );
	#endif
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int32 ThisYear( void )
{
#ifdef _WIN32
	SYSTEMTIME SystemTime;

	GetLocalTime( &SystemTime );
    return (Int32)SystemTime.wYear;
#else
    struct tm    FTime;
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    
    localtime_r( &now.tv_sec, &FTime );        
    return (Int32) (FTime.tm_year + 1900);	
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SetTradeingHour( int OpenHr, int OpenMin, int CloseHr, int CloseMin )
{
    GCloseTime = CloseHr*100 + CloseMin;
    GOpenTime  = OpenHr*100 + OpenMin;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int32 GetSwitchTradeDateHHMMSS( void )
{
    return GCloseTime*100;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::UDate GetTradeDate( void )
{
	int TimeNow;
#ifdef _WIN32
	SYSTEMTIME SystemTime;

	GetLocalTime( &SystemTime );
	TimeNow = SystemTime.wHour *100 + SystemTime.wMinute;
	if( TimeNow < GCloseTime )
	{
		UDate TradeDate( SystemTime.wDay, SystemTime.wMonth, SystemTime.wYear );
		TradeDate -= 1;
		return TradeDate;
	}
	else
        return UDate( SystemTime.wDay, SystemTime.wMonth, SystemTime.wYear );
#else
	struct tm    FTime;
        struct timespec now;
    
        clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    
        localtime_r( &now.tv_sec, &FTime );        
	TimeNow = FTime.tm_hour *100 + FTime.tm_min;
	if( TimeNow < GCloseTime )
	{
		UDate TradeDate( FTime.tm_mday, FTime.tm_mon+1, FTime.tm_year+1900 );
		TradeDate -= 1;
		return TradeDate;
	}
	else
        return UDate( FTime.tm_mday, FTime.tm_mon+1, FTime.tm_year+1900 );
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetTradeYYYYMMDD( AnsiString& Today, BOOL WithSlash )
{
	int TimeNow;
#ifdef _WIN32
	SYSTEMTIME SystemTime;

	GetLocalTime( &SystemTime );
	TimeNow = SystemTime.wHour *100 + SystemTime.wMinute;
	if( TimeNow < GCloseTime )
	{
		UDate TradeDate( SystemTime.wDay, SystemTime.wMonth, SystemTime.wYear );
		TradeDate -= 1;
		if( WithSlash == FALSE )
			Today.Printf( "%04d%02d%02d", TradeDate.getYear(), TradeDate.getMonth(), TradeDate.getDate() );
		else
			Today.Printf( "%04d/%02d/%02d", TradeDate.getYear(), TradeDate.getMonth(), TradeDate.getDate() );
	}
	else
	{
		if( WithSlash == FALSE )
			Today.Printf( "%04d%02d%02d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay );
		else
			Today.Printf( "%04d/%02d/%02d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay );
	}
#else
	struct tm    FTime;
        struct timespec now;
    
        clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    
        localtime_r( &now.tv_sec, &FTime );                	
	TimeNow = FTime.tm_hour *100 + FTime.tm_min;
	if( TimeNow < GCloseTime )
	{
		UDate TradeDate( FTime.tm_mday, FTime.tm_mon+1, FTime.tm_year+1900 );
		TradeDate -= 1;
		if( WithSlash == FALSE )
			Today.Printf( "%04d%02d%02d", TradeDate.getYear(), TradeDate.getMonth(), TradeDate.getDate() );
		else
			Today.Printf( "%04d/%02d/%02d", TradeDate.getYear(), TradeDate.getMonth(), TradeDate.getDate() );
	}
	else
	{
		if( WithSlash == FALSE )
			Today.Printf( "%04d%02d%02d", FTime.tm_year+1900, FTime.tm_mon+1, FTime.tm_mday );
		else
			Today.Printf( "%04d/%02d/%02d", FTime.tm_year+1900, FTime.tm_mon+1, FTime.tm_mday );
	}
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetTradeYYYMMDD( AnsiString& Today )
{
	int TimeNow;
#ifdef _WIN32
	SYSTEMTIME SystemTime;

	GetLocalTime( &SystemTime );
	TimeNow = SystemTime.wHour *100 + SystemTime.wMinute;
	if( TimeNow < GCloseTime )
	{
		UDate TradeDate( SystemTime.wDay, SystemTime.wMonth, SystemTime.wYear );
		TradeDate -= 1;
		Today.Printf( "%03d%02d%02d", TradeDate.getYear()- 1911, TradeDate.getMonth(), TradeDate.getDate() );
	}
	else
		Today.Printf( "%03d%02d%02d", SystemTime.wYear- 1911, SystemTime.wMonth, SystemTime.wDay );
#else
	struct tm    FTime;
        struct timespec now;
    
        clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    
        localtime_r( &now.tv_sec, &FTime );        	
	TimeNow = FTime.tm_hour *100 + FTime.tm_min;
	if( TimeNow < GCloseTime )
	{
	    UDate TradeDate( FTime.tm_mday, FTime.tm_mon+1, FTime.tm_year+1900 );
	    TradeDate -= 1;
	    Today.Printf( "%03d%02d%02d", TradeDate.getYear()- 1911, TradeDate.getMonth(), TradeDate.getDate() );
	}
	else
	    Today.Printf( "%03d%02d%02d", FTime.tm_year+1900- 1911, FTime.tm_mon+1, FTime.tm_mday );
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetYYMMDD( AnsiString& Today )
{
#ifdef _WIN32
    SYSTEMTIME SystemTime;

    GetLocalTime( &SystemTime );
	Today.Printf( "%03d%02d%02d", SystemTime.wYear - 1911 , SystemTime.wMonth, SystemTime.wDay );
#else
    struct tm    FTime;
    struct timespec now;

    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.
    localtime_r( &now.tv_sec, &FTime );
    Today.Printf( "%03d%02d%02d", FTime.tm_year+1900 - 1911, FTime.tm_mon+1, FTime.tm_mday );
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetYYYMMDD( AnsiString& Today )
{
#ifdef _WIN32
    SYSTEMTIME SystemTime;

    GetLocalTime( &SystemTime );
    Today.Printf( "%03d%02d%02d", (SystemTime.wYear - 1911), SystemTime.wMonth, SystemTime.wDay );
#else
    struct tm    FTime;
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    
    localtime_r( &now.tv_sec, &FTime );            
    Today.Printf( "%03d%02d%02d", (FTime.tm_year+1900 - 1911), FTime.tm_mon+1, FTime.tm_mday );
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int32 GetSecondsToday( void )
{
#ifdef _WIN32
	SYSTEMTIME SystemTime;

	GetLocalTime( &SystemTime );
	return SystemTime.wHour*3600+SystemTime.wMinute*60+SystemTime.wSecond;
#else
    struct tm    FTime;
    struct timespec now;
    
    clock_gettime( CLOCK_REALTIME ,&now ); ///< Get the current time.    
    LocalTime_hms( now.tv_sec, &FTime );
    return FTime.tm_hour*3600+FTime.tm_min*60+FTime.tm_sec;
#endif
}
//---------------------------------------------------------------------------
void SetTime( Int32 Hour, Int32 Minute,Int32 Second, Int32 ms )
{
#ifdef _WIN32
    SYSTEMTIME Time;
    GetLocalTime( &Time );
    Time.wHour   = Hour;
    Time.wMinute = Minute;
	Time.wSecond = Second;
	Time.wMilliseconds = ms;
    SetLocalTime( &Time );
    UFC::BufferedLog::Printf( " Set system time using Exchange time.(%02d:%02d:%02d)", Hour, Minute, Second );
#else 
    UFC::AnsiString SetTimeCmd;
    #ifdef __AIX
        UDateTime Date;
        SetTimeCmd.Printf("date %02d%02d%02d%02d.%02d%02d",Date.getMonth(),Date.getDate(), Hour, Minute, Second, Date.getYear()%100 );
    #endif
	#ifdef __LINUX
		SetTimeCmd.Printf("date -s %02d:%02d:%02d.%03d", Hour, Minute, Second, ms );
    #endif
    #ifdef __SOLARIS
        SetTimeCmd.Printf("date %02d%02d.%02d", Hour, Minute, Second );
    #endif
    if( system( SetTimeCmd.c_str() ) == -1 )
        UFC::BufferedLog::Printf( " Set system time failed command[%s].", SetTimeCmd.c_str() );
    else
        UFC::BufferedLog::Printf( " Set system time to %02d:%02d:%02d.%03d", Hour, Minute, Second, ms );
#endif
}
//--------------------------------------------------------------------------------------------------------------------
bool IsYYYYMMStr( const AnsiString& YYYYMMStr  )
{
    int yyyyMMStrLen = YYYYMMStr.Length();
    if( yyyyMMStrLen != 6 ) return false;
    char firstMMChar = ' ';
    for( int i = 0; i < yyyyMMStrLen; i++ )
    {
        char curChar = YYYYMMStr[i];
        if( i <= 3 )
        {
            if ( ( curChar < '0' ) || ( curChar > '9' ) ) return false;
        }
        else if( i == 4 ) 
        {
            if( ( curChar != '0' ) && ( curChar != '1' ) )
                return false;
            else
                firstMMChar = curChar;
        }
        else
        {
            if ( curChar < '0' ) 
                return false;
            else if( ( firstMMChar == '0' ) && ( curChar == '0' ) )
                return false;
            else if( ( firstMMChar == '1' ) && ( curChar > '2' ) ) 
                    return false;
            else if ( curChar > '9' ) 
                return false;
        }
    }
    return true;
}
//   End of Time functions
bool IsMMMMonthStr( const AnsiString& MMMStr )
{
    if( MMMStr.Length() != 3 ) return false;
    UFC::AnsiString upperMMMStr = MMMStr.UpperCase();
    if( ( upperMMMStr == "JAN" ) || ( upperMMMStr == "FEB" ) || ( upperMMMStr == "MAR" ) ||
        ( upperMMMStr == "APR" ) || ( upperMMMStr == "MAY" ) || ( upperMMMStr == "JUN" ) ||
        ( upperMMMStr == "JUL" ) || ( upperMMMStr == "AUG" ) || ( upperMMMStr == "SEP" ) ||
        ( upperMMMStr == "OCT" ) || ( upperMMMStr == "NOV" ) || ( upperMMMStr == "DEC" ) )
        return true;
    else
        return false;
}
//--------------------------------------------------------------------------------------------------------------------
UFC::AnsiString ConvertMonthMMMToMM( const AnsiString& MMMStr )
{
    UFC::AnsiString monthMMStr = "";
    if( MMMStr.Length() != 3 ) return monthMMStr;
    UFC::AnsiString upperMMMStr = MMMStr.UpperCase();
    if      (upperMMMStr == "JAN") monthMMStr = "01";
    else if (upperMMMStr == "FEB") monthMMStr = "02";
    else if (upperMMMStr == "MAR") monthMMStr = "03";
    else if (upperMMMStr == "APR") monthMMStr = "04";
    else if (upperMMMStr == "MAY") monthMMStr = "05";
    else if (upperMMMStr == "JUN") monthMMStr = "06";
    else if (upperMMMStr == "JUL") monthMMStr = "07";
    else if (upperMMMStr == "AUG") monthMMStr = "08";
    else if (upperMMMStr == "SEP") monthMMStr = "09";
    else if (upperMMMStr == "OCT") monthMMStr = "10";
    else if (upperMMMStr == "NOV") monthMMStr = "11";
    else if (upperMMMStr == "DEC") monthMMStr = "12";
    else                           monthMMStr = "";
    return monthMMStr;
}
//--------------------------------------------------------------------------------------------------------------------
UFC::AnsiString ConvertMMMYYToYYYYMM( const AnsiString& MMMYYStr )
{
    UFC::AnsiString yyyyMMStr = "";
    if( MMMYYStr.Length() != 5 ) return yyyyMMStr;
    
    UFC::AnsiString mmmMonthStr = MMMYYStr.SubString( 0, 3 );
    UFC::AnsiString mmMonthStr = ConvertMonthMMMToMM( mmmMonthStr );
    if( mmMonthStr.Length() != 2 ) return yyyyMMStr;
    
    UFC::AnsiString yyYearStr = MMMYYStr.SubString( 3, 2 );
    if( !IsDigitalStr( yyYearStr ) ) return yyyyMMStr;
    
    int yyYear = yyYearStr.ToInt();
    UFC::UDateTime now;
    int ccCentury = now.getYear() / 100;
    yyyyMMStr.Printf( "%02d%02d%s", ccCentury, yyYear, mmMonthStr.c_str());
    return yyyyMMStr;
}
//--------------------------------------------------------------------------------------------------------------------
//   End of Time functions
//--------------------------------------------------------------------------------------------------------------------
UFC::AnsiString GetCurrentDir( void )
{
	char Dir[ PATH_MAX ];
#if defined( _WIN32 )
	GetCurrentDirectoryA( PATH_MAX, Dir );
#elif defined( __LINUX )
	if( getcwd( Dir, PATH_MAX ) == NULL )
            return UFC::AnsiString( "./" );
#else
	getwd( Dir );
#endif
	return UFC::AnsiString( Dir );
}

//---------------------------------------------------------------------------
Int32 FileList( const AnsiString& Path, const AnsiString& Ext, UFC::PStringList& Files )
{
    AnsiString DirPath(Path.c_str());
    Int32 Count = 0;
#ifdef _WIN32
        if (DirPath.LastChar() != '\\')
            DirPath += "\\";

        DirPath += "*";
        DirPath += Ext;

		WIN32_FIND_DATAA FindFileData;
        HANDLE hFind; // INVALID_HANDLE_VALUE;

		hFind = FindFirstFileA( DirPath.c_str(), &FindFileData);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                AnsiString FileName(FindFileData.cFileName);
                Files.Add(FileName);
                Count++;

            } while (FindNextFileA(hFind, &FindFileData) != 0);
        }

        FindClose(hFind);
        return Count;
#else
        if (DirPath.LastChar() != '/')
            DirPath += "/";

        DIR* Dir = opendir(DirPath.c_str());
        struct dirent* Item;
        struct stat FileStat;

        if (Dir != NULL)
        {
            while ((Item = readdir(Dir)) != NULL)
            {
                UFC::AnsiString FullPath;

                FullPath.Printf("%s%s", DirPath.c_str(), Item->d_name);
                if( stat(FullPath.c_str(), &FileStat) != 0 )
                    continue;
                if ((FileStat.st_mode & S_IFDIR) == FALSE)
                {
                    AnsiString FileName(Item->d_name);
                    int extLen  = Ext.Length();
                    int fileLen = FileName.Length();
                    if (extLen > 0 && fileLen >= extLen &&
                        FileName.SubString(fileLen - extLen, extLen) == Ext)
                    {
                        Files.Add(FileName);
                        Count++;
                    }
                }
            }
            closedir(Dir);
            return Count;
        }
        else
            printf("Open directory failed.\n");
        return 0;
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WriteErrorLog(const UFC::AnsiString& App, const UFC::AnsiString& Obj,const UFC::AnsiString& Msg)
{
    try
    {
        UFC::AnsiString FileName,MailGroup,MsgGroup;
    
        UFC::UiniFile   Config( "../cfg/SpeedyErrorLog.cfg" );
        if( Config.GetValue( "ErrorLog", "LogFileName", FileName ) == TRUE &&
            Config.GetValue( "ErrorLog", "MailGroup", MailGroup ) == TRUE &&
            Config.GetValue( "ErrorLog", "MsgGroup", MsgGroup ) == TRUE )
        {
            UFC::AnsiString Date,Time;
            UFC::AnsiString LogMsg;
            
            GetYYYYMMDD( Date );
            GetHHMMSS( Time );
            LogMsg.Printf( "%s;%s;%s;%s;%s;%s;%sERROR\n", Date.c_str(), Time.c_str(), App.c_str(),Obj.c_str(),MailGroup.c_str(),MsgGroup.c_str(),Msg.c_str());
            FileStreamEx File( FileName, "a" );
            File.Write( LogMsg.c_str(), LogMsg.Length() );            
            File.Flush();
        }
        else
            UFC::BufferedLog::Printf( " App:%s Object:%s Message:%s", App.c_str(), Obj.c_str(), Msg.c_str() );
    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " App:%s Object:%s Message:%s", App.c_str(), Obj.c_str(), Msg.c_str() );
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ClearErrorLog( void )
{
    try
    {
        UFC::AnsiString FileName;
    
        UFC::UiniFile   Config( "../cfg/SpeedyErrorLog.cfg" );
        if( Config.GetValue( "ErrorLog", "LogFileName", FileName ) == TRUE)
        {
            FileStreamEx File( FileName, "w" );
            File.Flush();
        }        
    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " Clear Error log failed" );
    }
}
//------------------------------------------------------------------------------
double IntToDouble( int IntVal, int Digi )
{
    double dividend = pow( 10.0, (double)Digi );
    return ((double)IntVal)/dividend;
}
//------------------------------------------------------------------------------
double Int64ToDouble( Int64 IntVal, int Digi )
{
    double dividend = pow( 10.0, (double)Digi );
    return ((double)IntVal)/dividend;
}
//------------------------------------------------------------------------------
int DoubleToInt( double DoubleVal, int Digi)
{
    double precision = pow(0.1, (double) (Digi + 1));
    double multiple = pow(10.0, (double) Digi);
    if (DoubleVal >= 0.0)
        return (int) ((DoubleVal + precision) * multiple);
    else
        return (int) ((DoubleVal - precision) * multiple);
}
//------------------------------------------------------------------------------
Int64 DoubleToInt64( double DoubleVal, int Digi)
{
    double precision = pow(0.1, (double) (Digi + 1));
    double multiple = pow(10.0, (double) Digi);
    if (DoubleVal >= 0.0)
        return (Int64) ((DoubleVal + precision) * multiple);
    else
        return (Int64) ((DoubleVal - precision) * multiple);
}
//------------------------------------------------------------------------------
bool IsDigitalStr( const AnsiString& IntegerStr )
{
    if( IntegerStr.Length() == 0 ) return false;
    for( int i = 0; i < IntegerStr.Length(); i++ )
    {
        char curChar = IntegerStr[i];
        if( ( curChar < '0' ) || ( curChar > '9' ) ) return false;
    }
    return true;
}
//------------------------------------------------------------------------------
bool IsIntegerStr( const AnsiString& IntegerStr )
{
    bool isDigitalCharAppear = false;
    for( int i = 0; i < IntegerStr.Length(); i++ )
    {
        char curChar = IntegerStr[i];
        if( ( curChar >= '0' ) && ( curChar <= '9' ) )
            isDigitalCharAppear = true;
        else if( ( curChar == '+' ) || ( curChar == '-' ) )
        {
            if( isDigitalCharAppear ) return false;
        }
        else
            return false;
    }
    return isDigitalCharAppear;
}
//------------------------------------------------------------------------------
bool IsFloatingStr( const AnsiString& FloatingStr )
{
    bool isDigitalCharAppear = false;
    int pointCount = 0;
    for( int i = 0; i < FloatingStr.Length(); i++ )
    {
        char curChar = FloatingStr[i];
        if( ( curChar >= '0' ) && ( curChar <= '9' ) )
            isDigitalCharAppear = true;
        else if( curChar == '.' )
        {
            pointCount++;
            if( pointCount > 1 )
                return false;
        }
        else if( ( curChar == '+' ) || ( curChar == '-' ) )
        {
            if( isDigitalCharAppear ) return false;
        }
        else
            return false;
    }
    return isDigitalCharAppear;
}
//------------------------------------------------------------------------------
UInt32 IPToInt( const UFC::AnsiString& IP )
{
	UFC::PStringList IPNodes;
	IPNodes.SetStrings( IP, "." );
	if( IPNodes.ItemCount( ) == 4 )
	{
		UInt32 Seg1 = IPNodes[0].ToInt();
		UInt32 Seg2 = IPNodes[1].ToInt();
		UInt32 Seg3 = IPNodes[2].ToInt();
		UInt32 Seg4 = IPNodes[3].ToInt();
		return (Seg1<<24 | Seg2 <<16 | Seg3 <<8 | Seg4 );
	}
	return 0;
}
//------------------------------------------------------------------------------
AnsiString  IntToIP( UInt32 IPInt )
{
	UInt32 Addr;
	AnsiString IPStr;

	UFC::NInt32::ToInt32( IPInt, (UInt8*)&Addr );
	IPStr.Printf("%d.%d.%d.%d", (Addr<<24) >> 24, (Addr<<16)>>24, (Addr<<8)>>24, (Addr)>>24 );
	return IPStr;
}
//------------------------------------------------------------------------------
const char  LUT16[] = { "0123456789ABCDEF" }; ///< 0~15
const UInt8 ASCII16[] = { 0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,0,10,11,12,13,14,15 }; ///< 0~15
const UInt8 LowMask = 0x0F;
//----------------------------------------------------------------------------------------------------------
void BinaryToHexString( const UInt8*     Binary,   ///[in]
                        const Int32      Length, ///[in]
                        UFC::AnsiString& Result )///[Out]
{
    Result.SetLength( Length * 2 );
    char*    HexStr = (char*)Result.c_str();
    UInt8    AByte;
    register int i,j = 0;

    for( i = 0; i < Length; i++ )
    {
         AByte = *( Binary + i );

         *(HexStr+j) = LUT16[ (AByte >> 4 ) ];
         j++;
         *(HexStr+j) = LUT16[ AByte & LowMask ];
         j++;
    }
    *(HexStr+j) = 0; ///< Null term.
}
//----------------------------------------------------------------------------------------------------------
static int HexCharToNibble( char c )
{
    if( c >= '0' && c <= '9' ) return c - '0';
    if( c >= 'A' && c <= 'F' ) return c - 'A' + 10;
    if( c >= 'a' && c <= 'f' ) return c - 'a' + 10;
    return -1;
}

BOOL HexStringToBinary( const UFC::AnsiString& Str, ///[In]
                        UInt8*                 Binary, ///[Out]
                        const Int32            Length ) ///[in]

{
    Int32 BinLen = Str.Length() / 2;

    if( BinLen <= Length )
    {
        const char* HexStr = (char*)Str.c_str();
        register int i, j = 0;

        for( i = 0; i < BinLen; i++ )
        {
            int Hi = HexCharToNibble( *(HexStr+j) ); j++;
            int Lo = HexCharToNibble( *(HexStr+j) ); j++;
            if( Hi < 0 || Lo < 0 )
                return FALSE;
            *(Binary+i) = (UInt8)((Hi<<4)|Lo);
        }
        return TRUE;
    }
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------
BOOL Is64Bits( void )
{
	static int Size_Of_Pointor = sizeof( void* );
	if( Size_Of_Pointor == 8 )
        return true;
    return false;
}
//----------------------------------------------------------------------------------------------------------
#ifdef __LINUX
BOOL GetMacAddress( const AnsiString& Interface, AnsiString& Address )
{
	int s;
	struct ifreq buffer;

	s = socket(PF_INET, SOCK_DGRAM, 0);
	if( s < 0 )
		return FALSE;

	memset(&buffer, 0x00, sizeof(buffer));
	strncpy(buffer.ifr_name, Interface.c_str(), IFNAMSIZ - 1);
	buffer.ifr_name[IFNAMSIZ - 1] = '\0';

	if( ioctl(s, SIOCGIFHWADDR, &buffer) < 0 )
	{
		close(s);
		return FALSE;
	}
	close(s);

	Address.Printf("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
		(unsigned char)buffer.ifr_hwaddr.sa_data[0],
		(unsigned char)buffer.ifr_hwaddr.sa_data[1],
		(unsigned char)buffer.ifr_hwaddr.sa_data[2],
		(unsigned char)buffer.ifr_hwaddr.sa_data[3],
		(unsigned char)buffer.ifr_hwaddr.sa_data[4],
		(unsigned char)buffer.ifr_hwaddr.sa_data[5]);
	return TRUE;
}
#endif
//------------------------------------------------------------------------------
int GetCPUs( void )
{
    #ifdef __LINUX
        return sysconf( _SC_NPROCESSORS_ONLN );
    #else
        return 1;
    #endif
}
//---------------------------------------------------------------------------
//
//   class IntToStr
//
//---------------------------------------------------------------------------
IntToStr::IntToStr( Int32 value )
{    
    bool IsNegative = (value < 0)? true: false;
    register char* ptr = FBuffer + 10;    
    register int tmp_value;
        
    do 
    {
        tmp_value = value;
        value /= 10;
        *ptr-- = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * 10)];
            
    } while( value );
    // Apply negative sign
    if( IsNegative == true ) 
        *ptr-- = '-'; 
    FBuffer[ 11 ] = 0;
    FResult = ptr + 1;
}
//---------------------------------------------------------------------------
IntToStr::IntToStr( Int32 value, int digi, bool PadZero )
{
    bool IsNegative = (value < 0)? true: false;
    register char* ptr = FBuffer + 10;    
    register int tmp_value;
    char FillChar;
    
    if( digi > 10 )
        digi = 10;
    if( PadZero == false )
        FillChar = ' ';
    else
        FillChar = '0';        
    if( IsNegative == true ) 
        digi--;     
    do 
    {
        tmp_value = value;
        value /= 10;
        *ptr-- = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * 10)];
        digi--;
            
    } while( value );    
    // Apply negative sign. ("   -321")
    if( PadZero == false && IsNegative == true )
        *ptr-- = '-';    
    // pad with zero or space.
    while( digi > 0 )
    {
        *ptr-- = FillChar;
        digi--;
    };    
    // Apply negative sign.( "-000321" )
    if( PadZero == true && IsNegative == true )        
        *ptr-- = '-';     
    FBuffer[ 11 ] = 0;
    FResult = ptr + 1;    
}
//---------------------------------------------------------------------------
int IntToStr::Length( void )
{
    return 11 - ( FResult -FBuffer );
}
//---------------------------------------------------------------------------
//
//   class UIntToStr
//
//---------------------------------------------------------------------------
UIntToStr::UIntToStr( UInt32 value )
{    
    register char* ptr = FBuffer + 10;///< seek to the end of the buffer.    
    register UInt32 tmp_value;
        
    do 
    {
        tmp_value = value;
        value /= 10;
        *ptr-- = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * 10)];
            
    } while( value );    
    FBuffer[ 11 ] = 0;
    FResult = ptr + 1;
}
//---------------------------------------------------------------------------
UIntToStr::UIntToStr( UInt32 value, int digi, bool PadZero )
{
    register char* ptr = FBuffer + 10;    
    register UInt32 tmp_value;
    char FillChar;
    
    if( digi > 10 )
        digi = 10;
    if( PadZero == false )
        FillChar = ' ';
    else
        FillChar = '0';        
    do 
    {
        tmp_value = value;
        value /= 10;
        *ptr-- = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * 10)];
        digi--;
            
    } while( value );        
    // pad with zero or space.
    while( digi > 0 )
    {
        *ptr-- = FillChar;
        digi--;
    };        
    FBuffer[ 11 ] = 0;
    FResult = ptr + 1;    
}
//---------------------------------------------------------------------------
int UIntToStr::Length( void )
{
    return 11 - ( FResult -FBuffer );
}
//---------------------------------------------------------------------------
//
//   class DoubleToStr
//
//---------------------------------------------------------------------------
DoubleToStr::DoubleToStr( double Num )
{
    Int64 IntValue   = (Int64)Num;
    Int64 IntDecimal = (Int64)(fabs(Num - (double)IntValue)*1000000.0); ///< default 6
    bool  IsNegative = (Num < 0.0 )? true: false;
    register char* ptr = FBuffer + 30;
    register Int64 tmp_value;

    if( IntValue > 1000000000000000LL || IntValue < -1000000000000000LL )
    {
        strcpy(FBuffer, "Value too large." );
        FResult = FBuffer;
        return;
    }
    ///< Decimal parts to string
    for( int i=0; i< 6; i++ )
    {
        tmp_value = IntDecimal;
        IntDecimal /= 10;
        *ptr-- = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - IntDecimal * 10)];        
    }
    *ptr-- = '.';         
    ///< Integer parts to string
    do 
    {
        tmp_value = IntValue;
        IntValue /= 10;
        *ptr-- = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - IntValue * 10)];
            
    } while( IntValue );
    // Apply negative sign
    if( IsNegative == true ) 
        *ptr-- = '-'; 
    FBuffer[ 31 ] = 0;
    FResult = ptr + 1;    
}       
//---------------------------------------------------------------------------
DoubleToStr::DoubleToStr( double Num, int Width, int Precision,  bool PadZero )
{
    Int64 IntValue   = (Int64)Num;
    static const double kPow10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };
    if( Precision < 0 ) Precision = 0;
    if( Precision > 9 ) Precision = 9;
    Int64 IntDecimal = (Int64)(fabs(Num - static_cast<double>(IntValue)) * kPow10[Precision]);
    bool  IsNegative = (Num < 0.0 )? true: false;
    register char* ptr = FBuffer + 30;
    register Int64 tmp_value;
    char FillChar;

    if( IntValue > 1000000000000000LL || IntValue < -1000000000000000LL )
    {
        strcpy(FBuffer, "Value too large." );
        FResult = FBuffer;
        return;
    }
    if( Width > 30 )
        Width = 30;    
    ///< Decimal parts to string
    for( int i=0; i< Precision; i++ )
    {
        tmp_value = IntDecimal;
        IntDecimal /= 10;
        *ptr-- = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - IntDecimal * 10)];        
    }
    *ptr-- = '.';             
    Width -= Precision+1;
    

    if( PadZero == false )
        FillChar = ' ';
    else
        FillChar = '0';        
    if( IsNegative == true ) 
        Width--;     
    do 
    {
        tmp_value = IntValue;
        IntValue /= 10;
        *ptr-- = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - IntValue * 10)];
        Width--;
            
    } while( IntValue );    
    // Apply negative sign. ("   -321")
    if( PadZero == false && IsNegative == true )
        *ptr-- = '-';    
    // pad with zero or space.
    while( Width > 0 )
    {
        *ptr-- = FillChar;
        Width--;
    };    
    // Apply negative sign.( "-000321" )
    if( PadZero == true && IsNegative == true )        
        *ptr-- = '-';     
    FBuffer[ 31 ] = 0;
    FResult = ptr + 1;            
}
//---------------------------------------------------------------------------
int DoubleToStr::Length( void )
{
    return 31 - ( FResult -FBuffer );
}
//---------------------------------------------------------------------------
BOOL IsValidIP( const UFC::AnsiString& IP )
{
	if( IP.Length() < 7 || IP.Length() > 15 )
		return false;
	UFC::PStringList Strs;

	Strs.SetStrings( IP, "." );
	if( Strs.ItemCount() == 4 )
	{
		for( int i = 0; i < 4; i++ ) ///< Check each number.
		{
			int number  = Strs[ i ].ToInt();
			if( number < 0 || number > 255 )
				return false;
		}
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
