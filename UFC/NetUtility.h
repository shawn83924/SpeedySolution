//---------------------------------------------------------------------------
#ifndef NetUtility_H
#define NetUtility_H
//------------------------------------------------------------------------------
// This is a part of the UNIX Foundation Classes Library for Solaris(R) platforms.
// Copyright (c) 2002 MobileDigital Development Inc.
// All rights reserved.
//
// Description: Utilities for network.
//              NInt32,NInt16: network order integer.
//              PEndian: Detect the Little-Endian/Big-Endian of the system.
// Author: Simon Chang
//
// This source code is only intended as a supplSeement to the
// Object Class Library for Solaris(R) platforms Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Object Class Library for Solaris(R) platforms product.
//---------------------------------------------------------------------------
#include "Exception.h"
#include "Stream.h"
#include "FileStream.h"
#include "MemoryStream.h"
#include "PThread.h"
#include "List.h"
#include "UDateTime.h"
#include <float.h>
#ifdef _WIN32
    typedef int socklen_t;
#else
    #include <sys/socket.h>		/* basic socket definitions */
    #include <netdb.h>
    #include <netinet/in.h>		/* sockaddr_in{} and other Internet defns */
    #include <arpa/inet.h>		/* inet(3) functions */
    #include <sys/time.h>
    #include <sys/timeb.h>
    #include <sys/stat.h>
    #include <dirent.h>
    #include <signal.h>
    #ifdef __LINUX               ///< In AIX, include these headers will course link error.    
	#include <net/if.h>
	#include <sys/ioctl.h>
    #endif	
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace UFC
{
#define SWAP_SHORT(x) ( ( ( (x) & 0xFF00 ) >> 8 ) + ( ( (x) & 0x00FF ) << 8 ) )
#define SWAP_LONG(x) ( ( ( (x) & 0xFF000000 ) >> 24 ) + ( ( (x) & 0x00FF0000 ) >> 8 ) + ( ( (x) & 0x0000FF00 ) << 8 ) + ( ( (x) & 0x000000FF ) << 24 ) )
#define SWAP_64(x) ( ( SWAP_LONG( (x) & 0xFFFFFFFF ) << 32 ) | SWAP_LONG( (x) >> 32 ) )
//------------------------------------------------------------------------------
//
//  Class to detect the Little-Endian/Big-Endian of the system.
//
//------------------------------------------------------------------------------
class PEndian
{
private:
    BOOL   FIsLittleEndian;
    size_t FThreadStackSize;
public:
    PEndian();
    BOOL   IsLittleEndian(){ return FIsLittleEndian; } ///< Is X86 system?
    UInt32 ThreadStackSize( ){ return (UInt32)FThreadStackSize; }
};
//------------------------------------------------------------------------------
extern PEndian      Endian;
//------------------------------------------------------------------------------
class PInitNetLib
{
private:
    time_t FLastSec;
    void Sleep1US( void );    
public:
    PInitNetLib();
    void Now( );
    void CheckNextDay( time_t& Time );
};
//------------------------------------------------------------------------------
//
//	NInt32
//
//---------------------------------------------------------------------------
class NInt32
{
private:
    UInt32 FInt32;  ///< Use Big-Endian as internal byte order.
public:
    NInt32();
    NInt32( Int32 I );
    Int32 ToInt32( void ) const;
    void Assign( Int32 I );
    UInt8* GetBytes( ){ return (UInt8*)&FInt32; }
    int   LoadFromStream( PStream* Stream );
    int   SaveToStream( PStream* Stream );
    NInt32& operator = ( Int32 Value );
    operator Int32 ( ) { return ToInt32(); }
    static Int32 ToInt32( UInt8* Data );
    static void  ToInt32( Int32 Value, UInt8* Data );
};
//---------------------------------------------------------------------------
//
//	NInt16
//
//---------------------------------------------------------------------------
class NInt16
{
private:
    UInt16 FInt16;
public:
    NInt16();
    NInt16( Int16 I );
    Int16 ToInt16( void ) const;
    void Assign( Int16 I );
    UInt8* GetBytes( ){ return (UInt8*)&FInt16; }
    int   LoadFromStream( PStream* Stream );
    int   SaveToStream( PStream* Stream );
    NInt16& operator = ( Int16 Value );
    static Int16 ToInt16( UInt8* Data );
    static void  ToInt16( Int16 Value, UInt8* Data );
};
//------------------------------------------------------------------------------
//
//	NInt64
//
//---------------------------------------------------------------------------
class NInt64
{
private:
    Int64 FInt64;  ///< Use Big-Endian as internal byte order.
public:
    NInt64();
    NInt64( Int64 I );
    Int64 ToInt64( void ) const;
    void Assign( Int64 I );
    UInt8* GetBytes( ){ return (UInt8*)&FInt64; }
    int   LoadFromStream( PStream* Stream );
    int   SaveToStream( PStream* Stream );
    NInt64& operator = ( Int64 Value );
    operator Int64 ( ) { return ToInt64(); }
    static Int64 SwapInt64(Int64 inValue);
    static Int64 ToInt64( UInt8* Data );
};
//------------------------------------------------------------------------------
//
//	NDouble
//
//---------------------------------------------------------------------------
class NDouble
{
private:
    double FDouble;  ///< Use Big-Endian as internal byte order.
public:
    NDouble();
    NDouble( double I );
    double ToDouble( void ) const;
    void Assign( double I );
    UInt8* GetBytes( ){ return (UInt8*)&FDouble; }
    int   LoadFromStream( PStream* Stream );
    int   SaveToStream( PStream* Stream );
    NDouble& operator = ( double Value );
    operator double ( ) { return ToDouble(); }
    static double SwapDouble(double inValue);
    static double ToDouble( UInt8* Data );
};
//------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class RNCharArray
{
private:
    UInt8* FPChar;  
    int    FLen;
public:
    RNCharArray( UInt8* Buffer, int Offset, int Len ):FPChar( (UInt8*)( Buffer + Offset )),FLen(Len) { }
    void  Set( const char* Val ) 
    { 
        memcpy( FPChar, Val, FLen);  
    }
    void  Get( char* OutVal ) 
    { 
       memcpy( OutVal, FPChar, FLen); 
       OutVal[ FLen ] = 0;
    }    
    UInt8*  Data( void ) 
    { 
        return FPChar;
    }
};
//---------------------------------------------------------------------------
class RNChar
{
private:
    UInt8* FPChar;  
public:
    RNChar( UInt8* Buffer, int Offset ):FPChar( (UInt8*) Buffer + Offset ) { }
    void  Set( char Val )  { *FPChar = Val;  }
    char  Get( void )      { return *FPChar; }    
};
//---------------------------------------------------------------------------
class RNUInt8
{
private:
    UInt8* FPInt8;  
public:
    RNUInt8( UInt8* Buffer, int Offset ):FPInt8( (UInt8*) Buffer + Offset ) { }
    void  Set( UInt8 Val )  { *FPInt8 = Val;  }
    UInt8 Get( void )       { return *FPInt8; }    
};
//---------------------------------------------------------------------------
class RNUInt16
{
private:
    UInt16* FPInt16;  
public:
    RNUInt16( UInt8* Buffer, int Offset ):FPInt16( (UInt16*) (Buffer + Offset) ) { }
    void Set( UInt16 Val )
    {
        if( Endian.IsLittleEndian() )
            *FPInt16 = SWAP_SHORT( Val );    ///< Convert to Big-Endin
        else
            *FPInt16 = Val;
    }
    UInt16 Get( void )
    {
        if( Endian.IsLittleEndian() )
            return SWAP_SHORT( *FPInt16 ); ///< Convert to Big-Endin
        else
            return *FPInt16;
    }    
};
//---------------------------------------------------------------------------
class RNUInt32
{
private:
    UInt32* FPInt32;  
public:
    RNUInt32( UInt8* Buffer, int Offset ):FPInt32( (UInt32*)( Buffer + Offset) ) { }
    void Set( UInt32 Val )
    {
        if( Endian.IsLittleEndian() )
            *FPInt32 = SWAP_LONG( Val );    ///< Convert to Big-Endin
        else
            *FPInt32 = Val;
    }
    UInt32 Get( void )
    {
        if( Endian.IsLittleEndian() )
            return SWAP_LONG( *FPInt32 ); ///< Convert to Big-Endin
        else
            return *FPInt32;
    }    
};
//---------------------------------------------------------------------------
class RNUInt64
{
private:
    UInt64* FPInt64;  
public:
    RNUInt64( UInt8* Buffer, int Offset ):FPInt64( (UInt64*) (Buffer + Offset) ) { }
    void   Set( UInt64 Val )  { *FPInt64 = NInt64::SwapInt64( Val );  }
    UInt64 Get( void )        { return NInt64::SwapInt64( *FPInt64 ); }    
};
//------------------------------------------------------------------------------
class PDateTime
{
private:
    time_t FDateTime;
public:
    PDateTime( Int16 Year, Int16 Month, Int16 Day );
    PDateTime&  operator =( const PDateTime& rhs );
    bool        operator ==( const PDateTime& rhs );
    PDateTime   operator +( const int Days );
    PDateTime&  operator ++( );
    PDateTime&  operator +=(const int rhs);
    PDateTime&  operator --( );
    PDateTime&  operator -=(const int rhs);
    void DecodeDate( Int16& year, Int16& month, Int16& day );
    void DecodeTime( Int16& hour, Int16& min,   Int16& sec );
    static PDateTime CurrentDate();
    static PDateTime CurrentDateTime();
};
//------------------------------------------------------------------------------
class CharToStr
{
private:
    char FBuffer[ 2 ];
public:
    CharToStr( char ch ) 
    {
        FBuffer[0] = ch;
        FBuffer[1] = 0;
    }
    operator const char*() const {  return FBuffer;  }
    int Length( void ) { return 1; }
};
//------------------------------------------------------------------------------
class IntToStr
{
private:
    char FBuffer[ 12 ];
    char* FResult;
public:
    IntToStr( Int32 Num );
    IntToStr( Int32 Num, int IntWidth, bool PadZero = false );
    operator const char*() const {  return FResult;  }
    int Length( void );
};
//------------------------------------------------------------------------------
class UIntToStr
{
private:
    char FBuffer[ 12 ];
    char* FResult;
public:
    UIntToStr( UInt32 Num );
    UIntToStr( UInt32 Num, int IntWidth, bool PadZero = false );
    operator const char*() const {  return FResult;  }
    int Length( void );
};
//------------------------------------------------------------------------------
class DoubleToStr
{
private:
    char FBuffer[ 32 ]; ///< 15 integer parts, 16 decimal parts.
    char* FResult;
public:
    DoubleToStr( double Num );
    DoubleToStr( double Num, int IntWidth, int Precision,  bool PadZero = false );
    operator const char*() const {  return FResult;  }
    int Length( void );
};
//------------------------------------------------------------------------------
class Int64ToStr
{
private:
    char FBuffer[ 24 ];
    char* FResult;
public:
    Int64ToStr( Int64 Num );
    Int64ToStr( Int64 Num, int IntWidth, bool PadZero = false );
    operator const char*() const {  return FResult;  }
    int Length( void );
};
//------------------------------------------------------------------------------
extern PEndian         Endian;
extern char            Hostname[];
extern char            WorkingDir[];
extern BOOL            GLog_us;
extern BOOL            GLogThread;
//------------------------------------------------------------------------------
extern void            BeginTickus( void );
extern Int32           EndTickus( void );
//------------------------------------------------------------------------------
extern UInt32          IPToInt( const AnsiString& IP );
extern AnsiString      IntToIP( UInt32 IPInt );
//------------------------------------------------------------------------------
typedef enum
{
    lmtOK      = 1,
    lmtWarning = 2,
    lmtError   = 3
            
} LineMsgTypeEnum;
//------------------------------------------------------------------------------
extern void            SendLineMessage( LineMsgTypeEnum Type, const UFC::AnsiString& Message );
extern void            SendLineNotifyMessage( LineMsgTypeEnum Type, const UFC::AnsiString& Message );
extern void            SendLineNotifyMessage( LineMsgTypeEnum Type, const UFC::AnsiString& Message, const UFC::AnsiString& NotifyShellName );
extern UInt64          GetTickCountUS( void );
extern UInt32          GetTickCountMS( void );
extern Int32           GetSecondsToday( void );
extern void            GetFIXTimeString( char* TimeNow );
extern void            GetTimeString( AnsiString& TimeNow, BOOL WithSep = TRUE );
extern void            GetUTCTimeString( char* TimeNow, BOOL WithSep = TRUE );
extern void            GetTimeString( char* TimeNow, BOOL WithSep = TRUE );
extern void            GetTimeString_us( AnsiString& TimeNow , BOOL WithSep = TRUE );
extern void            GetTimeString_us( char* TimeNow , BOOL WithSep = TRUE );
extern void            GetTimeString_ms_us( char* TimeNow , BOOL Use_us, BOOL WithSep );
extern void            GetYYYYMMDDHHMMSSmmm( AnsiString& DTime );
extern void            GetFIXYYYYMMDDHHMMSSmmm( AnsiString& DTime );
extern void            GetHHMMSS( AnsiString& TimeNow );
extern void            GetHHMMSSmm( AnsiString& TimeNow );
extern void            GetHHMMSSmmm( AnsiString& TimeNow );
extern void            GetHHMMSS_Null( char* TimeNow );
extern void            GetHHMMSS_No_Null( char* TimeNow );
extern Int32           GetHHMMSS( void );
extern UInt64          GetHHMMSSmmmuuu( void );
extern void            GetYYYYMMDD( AnsiString& Today, BOOL WithSlash = FALSE );
extern void            GetYYMMDD( AnsiString& Today );
extern void            GetYYYMMDD( AnsiString& Today );
extern Int32           ThisYear( void );
extern Int32           GetSwitchTradeDateHHMMSS( void );
extern UFC::UDate      GetTradeDate( void );
extern void            GetTradeYYYYMMDD( AnsiString& Today, BOOL WithSlash = FALSE  );
extern void            GetTradeYYYMMDD( AnsiString& Today );
extern void            SetTradeingHour( int OpenHr, int OpenMin, int CloseHr, int CloseMin );
extern void            SetTime( Int32 Hour, Int32 Minute,Int32 Second, Int32 ms = 0  );
extern bool            IsYYYYMMStr( const AnsiString& YYYYMMStr );
extern bool            IsMMMMonthStr( const AnsiString& MMMStr );
extern UFC::AnsiString ConvertMonthMMMToMM( const AnsiString& MMMStr );
extern UFC::AnsiString ConvertMMMYYToYYYYMM( const AnsiString& MMMYYStr );
//------------------------------------------------------------------------------
extern Int32           FileList( const AnsiString& Path, const AnsiString& Filter, UFC::PStringList& Files );
extern UFC::AnsiString GetCurrentDir( void );
extern void            WriteErrorLog(const UFC::AnsiString& App, const UFC::AnsiString& Obj,const UFC::AnsiString& Msg);
extern void            ClearErrorLog( void );
//------------------------------------------------------------------------------
extern double          IntToDouble( int IntVal, int Digi );
extern double          Int64ToDouble( Int64 IntVal, int Digi );
extern int 	       DoubleToInt( double DoubleVal, int Digi );
extern Int64           DoubleToInt64( double DoubleVal, int Digi );
extern const char*     Int64ToBase62( UInt64 Value, UFC::AnsiString& OutBuf, BOOL PadToFull = FALSE, char PadChar = '0' );
extern UInt64          Base62ToInt64( const UFC::AnsiString& Base62 );
extern bool            IsDigitalStr( const AnsiString& IntegerStr );
extern bool            IsIntegerStr( const AnsiString& IntegerStr );
extern bool            IsFloatingStr( const AnsiString& FloatingStr );
//------------------------------------------------------------------------------
extern void            BinaryToHexString( const UInt8* Binary, const Int32 Length, UFC::AnsiString& Result );
extern BOOL            HexStringToBinary( const UFC::AnsiString& Str, UInt8* Binary, const Int32 Length );
//------------------------------------------------------------------------------
extern BOOL            Is64Bits( void );
extern BOOL            IsValidIP( const UFC::AnsiString& IP );
//------------------------------------------------------------------------------
// get the number of available processors.
extern int             GetCPUs( void );
#ifdef __LINUX
//------------------------------------------------------------------------------
extern BOOL            GetMacAddress( const AnsiString& Interface, AnsiString& Address );
//------------------------------------------------------------------------------
#endif
}
//------------------------------------------------------------------------------
#endif // NetUtility_H
//------------------------------------------------------------------------------
