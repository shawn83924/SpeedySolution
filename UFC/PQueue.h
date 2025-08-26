//---------------------------------------------------------------------------
// PQueue
//---------------------------------------------------------------------------
#ifndef PQueue_H
#define PQueue_H
//---------------------------------------------------------------------------
#include <string.h>
#include "UFCType.h"
#include "PThread.h"
#include "Exception.h"
#include "NetUtility.h"
#ifndef __IBMZ
    #ifdef GCC_CPP11_SUPPORT
        #include "include/ff/mpmc/MPMCqueues.hpp"
    #endif
#endif
#ifdef _WIN32
	#include "TFileMapping.h"
    #define key_t long
#endif
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
extern int GMinSleepUS;    
extern int GDeqPerUS;
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
    dqLessCPU  = 0,
    dqBalance  = 1,
    dqBusyLoop = 2
            
} DequeTypeEnum;    
//---------------------------------------------------------------------------
class UQueueData
{
friend class PQueue;
private:
    typedef struct MsgBuffer
    {
        long mtype;
        char Data[1];

    } QueueData;
private:
    UInt8*     FArray;
    QueueData* FData;
    int        FSize;
public:
    UQueueData( long mType = 0, int Size = 256 ); ///< For receive
    UQueueData( long mType, char* Str );
    UQueueData( long mType, char* Data, int Size );
    UQueueData( long mType, char* Data, int DataSize, int Size );
    ~UQueueData();
    void* GetData()              { return (void*)FData->Data; }
    int   GetSize()              { return FSize; }
    long  GetMType()             { return FData->mtype; }
    void  SetMType( long mtype ) { FData->mtype = mtype; }
};
//---------------------------------------------------------------------------
class PQueue
{
#ifdef _WIN32
private:
    typedef struct struct_QHeader
    {
        long type;
        long offset;
        long size;
    } QHeader;
    
    typedef struct struct_QBody
    {
        char data[ 512 ];
    } QBody;

    UFC::TMutex  FMutex;
    TFileMapping FFileMapping;
    long         FMutexTimeout;
    long         FMaximunMessageCount;
    long         FHeaderSize;
    long         FBodySize;
    QHeader*     FHeader;
    QBody*       FBody;
private:
    void CheckMessageType( long dataType, long& maxType, long& minType );
    BOOL CopyQueueData( UQueueData& Data );
#endif
private:
    int          FQueueID;
    BOOL         FInterrupt;
    BOOL         FShared;
    UFC::PEvent* FEnqueueEvent;
public:
    PQueue( BOOL SharedEvent = TRUE );
    ~PQueue( void );
public:
    bool    Open( key_t QueueKey, bool CreateNew = true );
    bool    SetQueueBytes(int Value);
    void    Send( UQueueData& Data );
    void    Recv( UQueueData& Data );
    BOOL    IsOpened( void ) { return (FQueueID != -1 ); }
    BOOL    RecvNoWait( UQueueData& Data );
    BOOL    Recv( UQueueData& Data, const int TimeoutSec );
    BOOL    Recv( UQueueData& Data, UFC::PLongList& MTypes, const int TimeoutSec );
    BOOL    Recv( UQueueData& Data, UFC::PLongList& MTypes );
    void    Interrupt( void );
    void    Clear( void );
    UInt16  Count( void );
    UInt32  Size( void );
};
//---------------------------------------------------------------------------
#ifdef GCC_CPP11_SUPPORT
//---------------------------------------------------------------------------    
class AtomicCount
{
public:    
    std::atomic<unsigned int> FCount;
public:        
    AtomicCount( unsigned int iniVal )    
    {
        FCount.store( iniVal );
    }
    void  operator =( unsigned int Val )
    {
        FCount.store( Val );                
    }    
    void  operator +=( unsigned int Val )
    {
        unsigned int CurVal = FCount.load();
        FCount.store( CurVal + Val );                
    }
    void  operator -=( unsigned int Val )
    {
        unsigned int CurVal = FCount.load() ;
        
        if( Val > CurVal)
            FCount.store( 0 );                
        else
            FCount.store( CurVal - Val );                
    }
    unsigned int Value()
    {
        return FCount.load();
    }
};
//---------------------------------------------------------------------------
//
//  Warp MPMC Queue from FastDlow
//
//---------------------------------------------------------------------------
typedef ff::MPMC_Ptr_Queue* PMPMC_Ptr_Queue;
//---------------------------------------------------------------------------
template <class T>
class PMPMCQueue
{
private:
    unsigned int               FMTypeMax;
    unsigned int               FSize;        
    bool                       FInterrupt;
    PMPMC_Ptr_Queue*           FQueues;        
    DequeTypeEnum              FType; 
private:    
    AtomicCount                FCount;
    AtomicCount                FPopCount;
private:
    void IncPopCount( void );
    void DecPopCount( void );
public:    
    PMPMCQueue( unsigned int Size, unsigned int MTypeMax );    
    ~PMPMCQueue( );    
    bool push( unsigned int MType, T* Data );
    
    bool pop( unsigned int MType, T** Data );
    bool pop( UFC::PLongList& MTypes, T** Data , const int TimeoutSec );
    bool pop( UFC::PLongList& MTypes, T** Data  );
    bool pop( T** Data );    
    void Interrupt( void );    
    void SetType( DequeTypeEnum Type ) { FType = Type; }
    unsigned long  Count( void );
};
//------------------------------------------------------------------------------
template <class T>
PMPMCQueue<T>::PMPMCQueue( unsigned int Size, unsigned int MTypeMax )
:FMTypeMax( MTypeMax )
,FSize( Size )
,FInterrupt( false )
,FType( dqBalance )
,FCount( 0 )
,FPopCount( 0 )
{
    FQueues = new PMPMC_Ptr_Queue[ FMTypeMax ];
    for( unsigned int i = 0; i < FMTypeMax; i++ )
         FQueues[ i ] = NULL;
    
    ff::MPMC_Ptr_Queue* queue = new ff::MPMC_Ptr_Queue();
    queue->init( FSize );    
    FQueues[ 0 ] = queue;    
}
//---------------------------------------------------------------------------
template <class T>
void PMPMCQueue<T>::IncPopCount( void )
{
    FPopCount+=1; ///< Increace the pop thread count.
}    
//---------------------------------------------------------------------------
template <class T>
void PMPMCQueue<T>::DecPopCount( void )
{
    if( FPopCount.Value() <= 1 )  ///< all pop thread notice this interrupt          
    {        
        if( FInterrupt == true )
            FInterrupt = false; ///< Turn off the interrupt flag.
        FPopCount = 0;
    }    
    else ///< decreace the pop thread count.
        FPopCount -= 1;    
}    
//------------------------------------------------------------------------------
template <class T>
PMPMCQueue<T>::~PMPMCQueue( )
{
    for( unsigned int i = 0; i < FMTypeMax; i++ )
    {
        if( FQueues[ i ] != NULL )
            delete FQueues[ i ];
    }
    delete [] FQueues;
}
//---------------------------------------------------------------------------
//  push object to given MType queue
//---------------------------------------------------------------------------
template <class T>
bool PMPMCQueue<T>::push( unsigned int MType, T* Data )
{    
    ff::MPMC_Ptr_Queue* queue;
    
    if( MType < FMTypeMax )
    {
        queue = FQueues[ MType ];
        if( queue == NULL ) ///< Queue not exist, create it
        {
            queue = new ff::MPMC_Ptr_Queue();
            queue->init( FSize );    
            FQueues[ MType ] = queue;
        }
        if( queue->push( (void*)Data ) == true  )
        {        
            FCount += 1;
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
// pop given MType queue
//---------------------------------------------------------------------------
template <class T>
bool PMPMCQueue<T>::pop( unsigned int MType, T** Data )
{
    ff::MPMC_Ptr_Queue* queue;
    
    if( MType < FMTypeMax )
    {
        ///< Queue exists ?    
        if( (queue = FQueues[ MType ]) != NULL )
        {   
            ///< Yes! dequeue it.
            if( queue->pop( (void**) Data ) == true )
            {
                FCount -= 1;
                return true;        
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
//  pop all MType from queue
//---------------------------------------------------------------------------
template <class T>
bool PMPMCQueue<T>::pop( T** Data )
{
    for( register unsigned int i = 0; i < FMTypeMax; i++ ) 
    {
        if( pop( i, Data ) == true )        
            return true;
    }   
    return false;
}
//---------------------------------------------------------------------------
//  pop given MTypes from queue 
//---------------------------------------------------------------------------
template <class T>
bool PMPMCQueue<T>::pop( UFC::PLongList& MTypes, T** Data )
{    
    for( register int i = 0; i < MTypes.ItemCount(); i++ ) 
    {                            
        if( pop( MTypes[ i ], Data ) == true )
            return true;        
    } 
    return false;
}
//---------------------------------------------------------------------------
//  pop given MTypes from queue with timeout
//---------------------------------------------------------------------------
template <class T>
bool PMPMCQueue<T>::pop( UFC::PLongList& MTypes, T** Data, const int TimeoutSec )
{
    Int32  MSLoopTimes = 100000/GMinSleepUS;                         ///< 100ms loop times
    Int32  DeQTimes    = (GDeqPerUS*GMinSleepUS)/MTypes.ItemCount(); ///< dequeue times equals to  GMinSleepUS
    Int32  WaitTimeMS  = TimeoutSec * 1000;
    Int32  BeginTimeMS = UFC::GetTickCountMS();    
        
    if( FType == dqLessCPU )
        DeQTimes /= 4;
    IncPopCount();
    do
    {        
        for( int t = 0; t < MSLoopTimes; t++ ) ///< MSLoopTimes*GMinSleepUS = 200ms
        {            
            for( int j = 0; j < DeQTimes; j++ ) ///< loops equals to GMinSleepUS
            {
                for( register int i = 0; i < MTypes.ItemCount(); i++ ) 
                {                            
                    if( FInterrupt == true  ) ///< Interrupt by User
                    {
                        DecPopCount();
                        return false;
                    }
                    if( pop( MTypes[ i ], Data ) == true ) ///< Dequeue from queue
                    {
                        DecPopCount();
                        return true;                    
                    }
                }                            
            }   
            if( FType != dqBusyLoop )
                UFC::SleepUS( GMinSleepUS );
        }
    }
    while( ((Int32)UFC::GetTickCountMS() - BeginTimeMS ) < WaitTimeMS );
    DecPopCount( );
    return false;
}
//---------------------------------------------------------------------------
template <class T>
void PMPMCQueue<T>::Interrupt( void )
{
    FInterrupt = true;
}
//---------------------------------------------------------------------------
template <class T>
unsigned long  PMPMCQueue<T>::Count( void )
{
    return FCount.Value();
}
//---------------------------------------------------------------------------
//
//  Warp MPMC Queue from FastDlow
//
//---------------------------------------------------------------------------
template <class T>
class LFQueue
{
private:
    PEvent             FInQEvent;
    ff::MPMC_Ptr_Queue FQueue;
    std::atomic<unsigned long> FCount;
    BOOL         FInterrupt;
public:    
    LFQueue( int Size = 1024);
    bool push( T* Data );
    bool pop( T** Data );
    bool pop( T** Data , const int TimeoutSec );
    void Interrupt( void );    
    unsigned long  Count( void );
};
//------------------------------------------------------------------------------
template <class T>
LFQueue<T>::LFQueue( int Size )
:FInterrupt( FALSE )
{
    FQueue.init( Size );    
    FCount.store(0);
}
//---------------------------------------------------------------------------
template <class T>
bool LFQueue<T>::push(  T* Data )
{    
    if( FQueue.push((void*)Data) )
    {
        int size = FCount.load();
        FCount.store( size + 1);                
        FInQEvent.SetEvent();
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
template <class T>
bool LFQueue<T>::pop(  T** Data )
{
   
    if( FQueue.pop((void**) Data ) )
    {
        int size = FCount.load();
        FCount.store( size - 1 );        
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
template <class T>
bool LFQueue<T>::pop(  T** Data, const int TimeoutSec )
{
    const Int32 WaitTimeMS  = TimeoutSec * 1000;
    const Int32 BeginTimeMS = UFC::GetTickCountMS();    
    
    do
    {        
        for( int j = 0; j < 300; j++ )
        {
            for( register int i = 0; i < 10; i++ ) 
            {            
                if( FQueue.pop((void**) Data ) )
                {
                    int size = FCount.load();
                    FCount.store( size - 1 );        
                    return true;
                }                        
                if( FInterrupt == TRUE ) ///< Interrupt by User
                {
                    FInterrupt = FALSE;
                    return false;
                }
            }
            UFC::SleepNS( 3 );            
        } 
        FInQEvent.WaitFor( 0, 5 );        
    }
    while( ((Int32)UFC::GetTickCountMS() - BeginTimeMS ) < WaitTimeMS );
    return false;
}
//---------------------------------------------------------------------------
template <class T>
void LFQueue<T>::Interrupt( void )
{
    FInterrupt = TRUE;
}
//---------------------------------------------------------------------------
#else
//---------------------------------------------------------------------------
//  Compile not support C++11 standard
//  disable atomic and FastFlow 
//---------------------------------------------------------------------------
template <class T>
class PMPMCQueue
{
public:    
    PMPMCQueue( unsigned int Size, unsigned int MTypeMax ) {}
    ~PMPMCQueue( ){}
    bool push( unsigned int MType, T* Data ){ return false; }
    bool pop( unsigned int MType, T** Data ){ return false; }    
    bool pop( UFC::PLongList& MTypes, T** Data , const int TimeoutSec ){ return false; }
    bool pop( UFC::PLongList& MTypes, T** Data  ) { return false; }
    bool pop( T** Data ){ return false; }
    void SetType( DequeTypeEnum Type ) { }
    void Interrupt( void ){ }
    unsigned long  Count( void ){ return 0; }
};
//---------------------------------------------------------------------------
template <class T>
class LFQueue
{
public:    
    LFQueue( int Size = 1024) {}
    bool push( T* Data ){ return false; }
    bool pop( T** Data ){ return false; }
    bool pop( T** Data , const int TimeoutSec ){ return false; }
    void Interrupt( void ){}
    unsigned long  Count( void ){ return 0; }
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif // PQueue_H
//---------------------------------------------------------------------------
