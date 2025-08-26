//---------------------------------------------------------------------------
#ifndef UFC_TYPE_H
#define UFC_TYPE_H
//---------------------------------------------------------------------------
namespace UFCType
{

//	typedef bool               Boolean;
	typedef unsigned char      UInt8;
	typedef unsigned short     UInt16;
	
	#ifdef __APPLE__
	typedef unsigned long       UInt32;
	#else
	typedef unsigned int       UInt32;
	#endif
	
	typedef char               Int8;
	typedef short              Int16;
	typedef int                Int32;
	typedef long long          Int64;
	typedef unsigned long long UInt64;
	#ifndef _WIN32  ///< for Linux,UNIX, Windows already define BOOL and Char
		//#if ( _MSC_VER < 1310 ) 
		//#if( || __BORLANDC__ < 0x0610 )///< Before C++ Builder 2009 not define BOOL,Char	 
	#ifdef __APPLE__
	typedef signed char  BOOL;
	#else
	typedef int          BOOL;
	#endif
	
		typedef char		       Char;	
	#endif
}
//---------------------------------------------------------------------------
#if (defined(_WIN32) ||defined(_WIN64))   ///< for Windows
	#include <winsock2.h>
	#define IP_MULTICAST_LOOPBACK 11 ///< Replace IP_MULTICAST_LOOP in Winsock.h
	///< map the Windows socket error code to UNIX.
	/// Allen mark at 20190614
	#if !defined(_WIN64)
		#ifndef _MSC_VER 
		#define EWOULDBLOCK             WSAEWOULDBLOCK
	//	#define EINPROGRESS             WSAEINPROGRESS
	//	#define EALREADY                WSAEALREADY
		#define ENOTSOCK                WSAENOTSOCK
	//	#define EDESTADDRREQ            WSAEDESTADDRREQ
	//	#define EMSGSIZE                WSAEMSGSIZE
	//	#define EPROTOTYPE              WSAEPROTOTYPE
	//	#define ENOPROTOOPT             WSAENOPROTOOPT
		#define EOPNOTSUPP              WSAEOPNOTSUPP
	//	#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
	//	#define EAFNOSUPPORT            WSAEAFNOSUPPORT
	//	#define EADDRINUSE              WSAEADDRINUSE
	//	#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
		#define ENETDOWN                WSAENETDOWN
		#define ENETUNREACH             WSAENETUNREACH
		#define ENETRESET               WSAENETRESET
		#define ECONNABORTED            WSAECONNABORTED
		#define ECONNRESET              WSAECONNRESET
		#define ENOBUFS                 WSAENOBUFS
		#define EISCONN                 WSAEISCONN
		#define ENOTCONN                WSAENOTCONN
		#define ETIMEDOUT               WSAETIMEDOUT
		#define ECONNREFUSED            WSAECONNREFUSED
	//	#define ELOOP                   WSAELOOP
		#define EHOSTUNREACH            WSAEHOSTUNREACH
		#endif
	#endif

		#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
		#define EPFNOSUPPORT            WSAEPFNOSUPPORT
        #define ESHUTDOWN               WSAESHUTDOWN
        #define ETOOMANYREFS            WSAETOOMANYREFS
        #define EHOSTDOWN               WSAEHOSTDOWN
        #define EPROCLIM                WSAEPROCLIM
        #define EUSERS                  WSAEUSERS
        #define EDQUOT                  WSAEDQUOT
        #define ESTALE                  WSAESTALE
        #define EREMOTE                 WSAEREMOTE
        #define PATH_MAX                MAX_PATH
        #define sleep(x)                SleepEx((x*1000),FALSE)  ///< Define sleep( sec )
        #define chdir(p)				SetCurrentDirectory(p)   ///< Mapping chdir to SetCurrentDirectory
	#ifdef _MSC_VER  ///< for Microsoft Visual C++
	  /// warning C:4786 identifier was truncated to 'number' characters in the debug information
	  /// warning C:4290 A function is declared using exception specification, which Visual C++ accepts but does not implement.
	  #pragma warning( disable : 4786 4290 4503 4355 4996 4244)
	  //#pragma warning( disable : 4786 4290 )//4503 4355  )
	  #if(_MSC_VER < 1500) ///< Before VC++ 2008
		 #define vsnprintf _vsnprintf
	  #endif 
	  #pragma once
	#endif
	#define O_SYNC 0 ///< in Win32 open not support O_SYNC flag
#else   ///< for UNIX ( Solaris )
        #define O_BINARY 0 ///< Linux/UNIX not support O_BINARY flag
	#define TRUE  1
	#define FALSE 0
	#define stricmp            strcasecmp
	#define strnicmp           strncasecmp
	#define SOCKET_ERROR       -1
	typedef void*              HANDLE;
	typedef void*              LPVOID;
	typedef HANDLE             HINSTANCE;
	typedef unsigned long      DWORD;
#endif
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
