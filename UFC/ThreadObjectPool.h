/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ThreadAlloc.h
 * Author: yuan
 *
 * Created on May 12, 2017, 4:52 PM
 */

#ifndef THREADALLOC_H
#define THREADALLOC_H
//------------------------------------------------------------------------------
#include "PThread.h"
#include "PPtrQueue.h"
#include "PQueue.h"
//#ifdef GCC_CPP11_SUPPORT
//    #include "include/ff/mpmc/MPMCqueues.hpp"  
//#endif
#include <queue>
//------------------------------------------------------------------------------
namespace UFC
{
#ifdef GCC_CPP11_SUPPORT    
template < class T>
class PThreadObjectPool  : public UFC::PThread
{
private:
    ff::MPMC_Ptr_Queue    FQ;
    AtomicCount           FCount;
    size_t                FPoolSize;
    int                   FNewPer50ms;    
private:        
    //--------------------------------------------------------------------------
    void Execute( void )     
    {   
        while( TRUE )
        {
            ///< Check objects in pool every 50 ms.
            UFC::SleepMS( 50 );  
            ///< New objects
            if( FCount.Value() <  FPoolSize )           
            {
                for( register int i = 0; i < FNewPer50ms; i++ )
                {
                    if( FQ.push( (void*) new T() ) == true )                            
                        FCount += 1;
                    if( FCount.Value() > FPoolSize )
                        break;
                }
            }
        }
    }
    //--------------------------------------------------------------------------
public:
    //--------------------------------------------------------------------------
    PThreadObjectPool( size_t PoolSize = 2048, int NewObjectPerSec = 500 )
    :UFC::PThread( NULL, TRUE )
    ,FCount(0)
    ,FPoolSize( PoolSize )
    ,FNewPer50ms( NewObjectPerSec / 20 )
    { 
        FQ.init( PoolSize*2 );
        for( register size_t i = 0; i < FPoolSize; i++ )
        {
            if( FQ.push( (void*) new T() ) == true )
                FCount += 1;
        }
        Start();
    }   
    //--------------------------------------------------------------------------
    T* GetInstance( void )
    {
        T* Obj;
        
        if( FQ.pop( (void**) &Obj) == true  )
        {
            FCount -= 1;
            if( Obj != NULL )
                return Obj;
        }
        return new T();                
    }    
    //--------------------------------------------------------------------------
    void Recycle( T* Obj )
    {        
        if( Obj != 0 && FQ.push( (void*) Obj ) == true )
            FCount += 1;
        
    }
    //--------------------------------------------------------------------------
};    
#else    
template < class T>
class PThreadObjectPool  : public UFC::PThread
{
private:
    queue<T*>             FQ;     
    UFC::PCriticalSection FCS;
    size_t                FPoolSize;
    int                   FNewPer50ms;    
private:   
    //--------------------------------------------------------------------------
    void Execute( void )     
    {   
        while( TRUE )
        {
            ///< Check objects in pool every 50 ms.
            UFC::SleepMS( 50 );            
            
            ///< New objects
            if( FQ.size() <  FPoolSize )           
            {
                UFC::PLockObject Lock( FCS );                
                for( register int i = 0; i < FNewPer50ms; i++ )
                     FQ.push( new T() );                            
            }            
        }
    }
    //--------------------------------------------------------------------------
public:
    //--------------------------------------------------------------------------
    PThreadObjectPool( size_t PoolSize = 2048, int NewObjectPerSec = 500 )
    :UFC::PThread( NULL, TRUE )
    ,FPoolSize( PoolSize )
    ,FNewPer50ms( NewObjectPerSec / 20 )
    {         
        for( register size_t i = 0; i < FPoolSize; i++ )
             FQ.push( new T() );
        Start();
    }   
    //--------------------------------------------------------------------------
    T* GetInstance( void )
    {
        if( FCS.TryEnter( ) == true )
        {
            T* Item = FQ.front();
            FQ.pop();
            FCS.Leave();
            return Item;
        }
        else
            return new T();        
    }    
    //--------------------------------------------------------------------------
    void Recycle( T* Obj )
    {
        if( FCS.TryEnter( ) == true )
        {
            FQ.push( Obj );
            FCS.Leave();
        }
        else
            delete Obj;
    }    
    //--------------------------------------------------------------------------
};    
#endif  

}
//------------------------------------------------------------------------------
#endif /* THREADALLOC_H */

