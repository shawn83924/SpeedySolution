/* 
 * File:   Profiler.h
 * Author: yuan
 *
 * Created on March 24, 2009, 12:31 AM
 */

#ifndef _PROFILER_H
#define	_PROFILER_H
//---------------------------------------------------------------------------
#include "PInt32.h"
#include "PQueue.h"
#include "PThread.h"
#include "PPtrQueue.h"
#include "Stream.h"
#include "AnsiString.h"
#ifdef _WIN32
#else
    #include <sys/time.h>
#endif
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
const int psTWFUT  = 1;    
const int psTWOPT  = 2;
const int psTWTSE  = 3;
const int psTWOTC  = 4;
const int psPATS   = 5;
const int psCNFUT  = 6;
const int psTWES   = 7;
//---------------------------------------------------------------------------
const int ppGatewayRecvOrd     = 1;
const int ppSpeedyOrdInQueue   = 2;
const int ppSpeedyOrdDeQueure  = 3;
const int ppExchangeOrdConfirm = 4;
const int ppGatewayOrdConfirm  = 5;
const int ppCheckResult        = 9;
//---------------------------------------------------------------------------
class TimeVal
{
private:
    struct timespec FNow;
public:
    TimeVal( void );
    TimeVal( const TimeVal&  );
    void   UpdateTime( void );
    long   GetEpoch( void ) { return (long)FNow.tv_sec; }
    long   Getusec( void )  { return ns2us(FNow.tv_nsec); }
    void   SaveToStream( UFC::PStream* );
    void   LoadFromStream( UFC::PStream* );
    Int64  Diffusec( const TimeVal& Time ) const;
    UFC::AnsiString ToString( void );
public:
    static TimeVal GetCurrentTime( void );
    long ns2us( long int  nsec ) const { return (long)nsec/1000;}    
};
//---------------------------------------------------------------------------
class ProfileData
{
public:
    TimeVal   FTimeStemp;   
    long      FGroup;
    Int8      FFlagID;
    char      FKey1[48];
    char      FKey2[48];
public:   
    ProfileData(){}
    ProfileData( TimeVal& Time ,long Group, Int8 Flag, const AnsiString& Key1, const AnsiString& Key2 );
    void Init( TimeVal& Time ,long Group, Int8 Flag, const AnsiString& Key1, const AnsiString& Key2 );
    void SaveToStream( UFC::PStream* );
    static void LoadFromStream( UFC::PStream* , TimeVal& Time , Int8& Flag, AnsiString& Key1, AnsiString& Key2);
};
//---------------------------------------------------------------------------
class ProfilerInqueueThread : public PThread
{
public:    
#ifdef GCC_CPP11_SUPPORT
    UFC::LFQueue<ProfileData>  FLFPtrQueue;
#else    
    UFC::PtrQueue<ProfileData> FPtrQueue;
#endif        
public:        
    ProfilerInqueueThread();    
    virtual void Execute( void );    
};
//---------------------------------------------------------------------------
class Profiler 
{
private:
    UFC::PInt32                FTickNow;    
    UFC::PCriticalSection      FProfilerCS;
    BOOL                       FIsInit;
    BOOL                       IsEnable;    
    ProfilerInqueueThread*     FInQThread; 
public:          
    UFC::PQueue*               FPerformanceQueue;
public:
    Profiler( void );
    BOOL  CanSetPerformanceFlag( TimeVal& Now );
    BOOL  CanGetPerformanceFlag( void );
    void  SetTimer( void );
    Int32 GetTimer( void );
    void  Init( void );
    void  InQueue( ProfileData* Data );
public: ///< Functions for SpeedyProfile only.
    static BOOL GetPerformanceFlag( long& Group, TimeVal& Time,Int8& FlagID, AnsiString& Key1, AnsiString& Key2 );
    static void UpdateTimer( void );
public:
    static void SetPerformanceFlag( long Group, Int8 FlagID, const AnsiString& Key1, const AnsiString& Key2 );
    static void EnableProfiler( BOOL Enabled );
    ///< Cheap guard for hot-path callers: SetPerformanceFlag's AnsiString parameters are
    ///< constructed BEFORE its internal enable check, so callers on a latency-critical path
    ///< should skip the call entirely when profiling is off:
    ///<     if( UFC::Profiler::Enabled() ) SetPerformanceFlag( ... );
    static BOOL Enabled( void );
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif	/* _PROFILER_H */

