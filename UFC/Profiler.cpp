//------------------------------------------------------------------------------
#include "Profiler.h"
#include "Stream.h"
#include "MemoryStream.h"
#include "BufferedLog.h"
#include "ThreadObjectPool.h"
//------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------
const int DATA_MAX_QUEUE = 128;    
//------------------------------------------------------------------------------
Profiler* StaticProfiler = NULL;
PThreadObjectPool<ProfileData>* StaticDataPool = NULL;
//------------------------------------------------------------------------------
ProfileData::ProfileData( TimeVal& Time ,long Group, Int8 Flag, const AnsiString& Key1, const AnsiString& Key2 )
:FTimeStemp( Time )
,FGroup( Group )
,FFlagID( Flag )    
{
    strncpy( FKey1, Key1.c_str(), 47 );
    strncpy( FKey2, Key2.c_str(), 47 );
}    
//------------------------------------------------------------------------------
void ProfileData::Init( TimeVal& Time ,long Group, Int8 Flag, const AnsiString& Key1, const AnsiString& Key2 )
{
    FTimeStemp = Time;
    FGroup     = Group;
    FFlagID    = Flag;        
    strncpy( FKey1, Key1.c_str(), 47 );
    strncpy( FKey2, Key2.c_str(), 47 );
}
//------------------------------------------------------------------------------
void ProfileData::SaveToStream( UFC::PStream* Stream )
{
    Stream->Write( (char*)&FFlagID, sizeof(Int8) );
    FTimeStemp.SaveToStream( Stream );
    Stream->Write( (char*)FKey1, 48 );
    Stream->Write( (char*)FKey2, 48 );
}
//------------------------------------------------------------------------------
void ProfileData::LoadFromStream( UFC::PStream* Stream, TimeVal& Time , Int8& Flag, AnsiString& Key1, AnsiString& Key2)
{
    char Key[48];
    Stream->Read( (char*)&Flag, sizeof(Int8) );
    Time.LoadFromStream( Stream );
    Stream->Read( Key, 48 );
    if( strlen( Key ) == 0 )
        Key1 = "null";
    else
        Key1 = Key;
    Stream->Read( Key, 48 );
    if( strlen( Key ) == 0 )
        Key2 = "null";
    else
        Key2 = Key;                
}
//------------------------------------------------------------------------------
//
// Class TimeVal
//    
//------------------------------------------------------------------------------
TimeVal::TimeVal( void )
{
    
}
//------------------------------------------------------------------------------
TimeVal::TimeVal( const TimeVal& ref )
{
    FNow.tv_sec  = ref.FNow.tv_sec;
    FNow.tv_nsec = ref.FNow.tv_nsec;
}
//------------------------------------------------------------------------------
void TimeVal::UpdateTime( void )
{
    clock_gettime( CLOCK_REALTIME ,&FNow ); ///< Get the current time.            
}
//------------------------------------------------------------------------------
TimeVal TimeVal::GetCurrentTime( void )
{
    TimeVal Now;
    
    clock_gettime( CLOCK_REALTIME ,&Now.FNow ); ///< Get the current time.        
    return Now;
}
//------------------------------------------------------------------------------
void TimeVal::LoadFromStream( UFC::PStream* Stream )
{
    Stream->Read( &FNow, sizeof(struct timespec));    
}
//------------------------------------------------------------------------------
void TimeVal::SaveToStream( UFC::PStream* Stream )
{
    Stream->Write( &FNow, sizeof(struct timespec));    
}
//------------------------------------------------------------------------------
Int64 TimeVal::Diffusec( const TimeVal& Time ) const
{
    Int64 ThisTime = FNow.tv_sec*1000000 + ns2us(FNow.tv_nsec);
    Int64 DiffTime = Time.FNow.tv_sec*1000000 + ns2us(Time.FNow.tv_nsec);

    return ThisTime - DiffTime;
}
//------------------------------------------------------------------------------
UFC::AnsiString TimeVal::ToString( void )
{
    UFC::AnsiString TimeStr;

    ///< convert to local time
    int Today_sec =  FNow.tv_sec % 86400;    
    int tm_hour   =  ((Today_sec / 3600 ) + 8 ) % 24;
    int tm_min    =  ( Today_sec % 3600 ) / 60;
    int tm_sec    =  Today_sec % 60;       
    
    TimeStr.Printf( "%02d:%02d:%02d.%06d", tm_hour, tm_min, tm_sec, ns2us(FNow.tv_nsec) );
    return TimeStr;
}
//------------------------------------------------------------------------------
//
// Class Profiler
//
//------------------------------------------------------------------------------
Profiler::Profiler( void )
:FIsInit( FALSE )
,IsEnable( FALSE )
,FInQThread( NULL )
,FPerformanceQueue( NULL )
{
    if( StaticDataPool == NULL )
        StaticDataPool = new PThreadObjectPool<ProfileData>( 4096, 1024 );
}
//------------------------------------------------------------------------------
void Profiler::Init( void )
{
    UFC::PLockObject Lobj( FProfilerCS );

    try
    {
        if( FIsInit == FALSE )
        {
            FPerformanceQueue = new PQueue( FALSE );
            FPerformanceQueue->Open( 0x00001111, TRUE );
            FTickNow.Attach( 0x00001111 ); ///< Open share memory for SpeedyProfile 
            FTickNow = (Int32) 0;          ///< to write tick as heartbeat. 
            FIsInit = TRUE;
            FInQThread = new ProfilerInqueueThread( );
        }
    }
    catch( UFC::Exception& e )
    {
        printf( " Initialize profiler failed:%s",e.what() );
        exit( 0 );
    }
}
//------------------------------------------------------------------------------
BOOL Profiler::CanSetPerformanceFlag( TimeVal& Now )
{
    if( IsEnable == TRUE && FIsInit == TRUE )
    {
        Int32  TimeDiffSec;
        
        Now.UpdateTime(); ///< Get the current time.
        TimeDiffSec = Now.GetEpoch() - (long)GetTimer();
        if( TimeDiffSec < 3 && TimeDiffSec > -3 ) ///< Time difference less than 3 sec
            return TRUE;    
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL Profiler::CanGetPerformanceFlag( void )
{
    return (IsEnable && FIsInit);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int32 Profiler::GetTimer( void )
{
    UFC::PLockObject Lobj( Profiler::FProfilerCS );
    return FTickNow.ToInt32();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Profiler::SetTimer( void )
{
    if( FIsInit == TRUE )
    {
        TimeVal Now; 

        Now.UpdateTime( ); ///< Get the current time.
        UFC::PLockObject Lobj( Profiler::FProfilerCS );
        
        FTickNow = (Int32)Now.GetEpoch();
    }    
}
//------------------------------------------------------------------------------
ProfilerInqueueThread::ProfilerInqueueThread()
:PThread( NULL, TRUE )
{
    Start();
}
//------------------------------------------------------------------------------
void ProfilerInqueueThread::Execute( void )
{
    ProfileData* Data;
    
    while( !IsTerminated() )
    {
    #ifdef GCC_CPP11_SUPPORT
        if( FLFPtrQueue.pop( &Data, 1000 ) == true)
    #else    
        if((Data = FPtrQueue.Dequeue( 1 )) != NULL )
    #endif                        
        {              
            try
            {
                UFC::UQueueData     PerformanceData( Data->FGroup,  DATA_MAX_QUEUE );
                FixSizeMemoryStream Stream( PerformanceData.GetData(), DATA_MAX_QUEUE );

                Data->SaveToStream( &Stream );    
                if( StaticProfiler != NULL )
                    StaticProfiler->FPerformanceQueue->Send( PerformanceData );            
            }
            catch( UFC::Exception& e )
            {
                UFC::BufferedLog::Printf( " SetPerformanceFlag failed:%s",e.what() );
            }
            StaticDataPool->Recycle( Data );
        }
    }
}
//------------------------------------------------------------------------------
void Profiler::SetPerformanceFlag( long Group, Int8 FlagID, const AnsiString& Key1, const AnsiString& Key2 )
{

    if( StaticProfiler != NULL && StaticProfiler->IsEnable == TRUE )
    {
        ProfileData* Data;                
        TimeVal CurrTime;
            
        if( StaticProfiler->CanSetPerformanceFlag( CurrTime ) == TRUE  )
        {
            if( (Data = StaticDataPool->GetInstance())!= NULL )
            {   
                Data->Init( CurrTime, Group, FlagID, Key1, Key2 );
                StaticProfiler->InQueue( Data );
            }
        }
    }
}
//------------------------------------------------------------------------------
BOOL Profiler::Enabled( void )
{
    return ( StaticProfiler != NULL && StaticProfiler->IsEnable == TRUE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Profiler::EnableProfiler( BOOL Enabled )
{
    if( StaticProfiler == NULL )
        StaticProfiler = new Profiler();
    
    if( StaticProfiler->IsEnable == FALSE )
    {
        if( Enabled == TRUE )
            StaticProfiler->Init();
        StaticProfiler->IsEnable = Enabled;    
    }
    else
    {
        if( Enabled == FALSE )
        {
                
            if( StaticProfiler->FInQThread != NULL )
                StaticProfiler->FInQThread->Terminate();            
            if( StaticProfiler->FPerformanceQueue != NULL )
            {
                StaticProfiler->FPerformanceQueue->Interrupt();
                UFC::SleepMS( 1000 );
                delete StaticProfiler->FPerformanceQueue;
                StaticProfiler->FPerformanceQueue = NULL;
            }
            StaticProfiler->IsEnable = Enabled;    
        }
    }    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Call by SpeedyProfile only!
// get performance information from queue. 
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL Profiler::GetPerformanceFlag( long& Group,TimeVal& Time, Int8& FlagID, AnsiString& Key1, AnsiString& Key2 )
{
    try
    {
        if( StaticProfiler != NULL && StaticProfiler->CanGetPerformanceFlag() )
        {
            UFC::UQueueData PerformanceData( 0, DATA_MAX_QUEUE );

            if( StaticProfiler->FPerformanceQueue->Recv( PerformanceData, 1 ) == TRUE )
            {
                MemoryReadStream Stream( PerformanceData.GetData(), DATA_MAX_QUEUE );
                
                Group = PerformanceData.GetMType();
                ProfileData::LoadFromStream( &Stream, Time, FlagID, Key1, Key2 );
                return TRUE;
            }
            else
                return FALSE;
        }
        return FALSE;
    }
    catch( UFC::Exception& e )
    {
        UFC::BufferedLog::Printf( " GetPerformanceFlag failed:%s",e.what() );
        return FALSE;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Call by SpeedyProfile only!
// to tell other Speedy programs that SpeedyProfile is running. 
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Profiler::UpdateTimer( void )
{
    if( StaticProfiler != NULL )
        StaticProfiler->SetTimer();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Profiler::InQueue( ProfileData* Data )
{
    #ifdef GCC_CPP11_SUPPORT
        FInQThread->FLFPtrQueue.push( Data );
    #else    
        FInQThread->FPtrQueue.InqueueNoWait( Data ); 
    #endif            
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
