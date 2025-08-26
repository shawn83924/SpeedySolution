//------------------------------------------------------------------------------
// This is a part of the UNIX Foundation Classes Library for Solaris(R) platforms.
// Copyright (c) 2002 MobileDigital Development Inc.
// All rights reserved.
//
// Description: Array List Class. Provide container functions.
//              Wrap STL containers.
// Author: Simon Chang
//
// This source code is only intended as a supplement to the
// Object Class Library for Solaris(R) platforms Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Object Class Library for Solaris(R) platforms product.
//------------------------------------------------------------------------------
#ifndef PTR_QUEUE_H
#define PTR_QUEUE_H
#include "stl_config.h"
#include "PThread.h"
//------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------------
// Class PtrQueue
//
// A fast FIFO queue class
//------------------------------------------------------------------------------
template <class T>
class PtrQueue
{
private:
	Int32     FQueueSize; ///< Max size of this queue.
        Int32     FHeadPos;   ///< Position for the first item.
	Int32     FCount;     ///< Item count.
	T**       FArray;
private:
	PEvent           FDequeueEvent;
	PEvent           FInqueueEvent;
	PCriticalSection FQueueCS;
private:
	BOOL      AddToQueue( T* Object );
	BOOL      AddToFrontOfQueue( T* Object );
	T*        GetFromQueue( void );
public:
	PtrQueue( Int32 Size = PTR_QUEUE_MAX_SIZE );
	~PtrQueue( void );
	Int32     Size( void ) const { return FQueueSize; }
	Int32     ItemCount( void ) const { return FCount; }
	BOOL      Inqueue( T* Object, Int32 TimeOutSec = 10 );
	BOOL      InqueueFront( T* Object, Int32 TimeOutSec = 10 );
	T*        Dequeue( Int32 TimeOutSec = 10  );
	T*        DequeueNoWait( void );
	BOOL      InqueueNoWait( T* Object );
	void      Clear( BOOL DeleteData = FALSE );
};
//---------------------------------------------------------------------------
template <class T>
PtrQueue<T>::PtrQueue( Int32 PSize  )
:FQueueSize( PSize ),
 FHeadPos( 0 ),
 FCount( 0 )
{
    FArray = new T*[ PSize ];
}
//---------------------------------------------------------------------------
template <class T>
PtrQueue<T>::~PtrQueue( void )                       
{ 
    Clear( FALSE );
    delete [] FArray; 
}
//---------------------------------------------------------------------------
template <class T>
BOOL PtrQueue<T>::AddToQueue( T* Object )
{
    UFC::PLockObject Lock( FQueueCS );

    if( FCount == FQueueSize ) ///< Queue full...return FALSE
        return FALSE;
    FCount++;
    Int32 CurrentPos = FHeadPos + FCount -1;
    if( CurrentPos >= FQueueSize )
        CurrentPos -= FQueueSize;
    *( FArray + CurrentPos ) = Object;
    return TRUE;
}
//---------------------------------------------------------------------------
template <class T>
BOOL PtrQueue<T>::AddToFrontOfQueue( T* Object )
{
    UFC::PLockObject Lock( FQueueCS );

    if( FCount == FQueueSize ) ///< Queue full...return FALSE
        return FALSE;
    FCount++;
    FHeadPos--;
    if( FHeadPos < 0 )
        FHeadPos = FQueueSize - 1;
    *( FArray + FHeadPos ) = Object;
    return TRUE;
}
//---------------------------------------------------------------------------
template <class T>
BOOL PtrQueue<T>::Inqueue( T* Object, Int32 TimeOutSec )
{
    while( TRUE )
    {
        if( FCount == FQueueSize ) ///< Queue is fulled.
        {
            if( FDequeueEvent.WaitFor( TimeOutSec ) == FALSE )///< Wait for another thread de-queue a object.
                return FALSE;
        }
        else
        {
            if( AddToQueue( Object ) == TRUE )///< Add to queue
            {
                FInqueueEvent.SetEvent();
                return TRUE;
            }
            ///< Queue still full...try again
        }
    }
}
//---------------------------------------------------------------------------
template <class T>
BOOL PtrQueue<T>::InqueueFront( T* Object, Int32 TimeOutSec )
{
    while( TRUE )
    {
        if( FCount == FQueueSize )  ///< Queue is fulled.
        {
            if( FDequeueEvent.WaitFor(TimeOutSec)==FALSE)///< Wait for another thread de-queue a object.
                return FALSE;
        }
        else
        {
            if( AddToFrontOfQueue( Object ) == TRUE )///< Add to queue
            {
                FInqueueEvent.SetEvent();
                return TRUE;
            }
            ///< Queue still full...try again
        }
    }
}
//---------------------------------------------------------------------------
template <class T>
BOOL PtrQueue<T>::InqueueNoWait( T* Object )
{
    if( AddToQueue( Object ) == TRUE )///< Add to queue
    {
        FInqueueEvent.SetEvent();
        return TRUE;
    }
    return FALSE;
}
//---------------------------------------------------------------------------
template <class T>
T* PtrQueue<T>::GetFromQueue( void )
{
    T* RtnObj;
    UFC::PLockObject Lock( FQueueCS );

    if( FCount == 0 ) ///< Queue still empty...return NULL
        return NULL;
    RtnObj = *( FArray + FHeadPos );///< first in ...first out, Dequeue from the header
    FHeadPos++;
    if( FHeadPos >= FQueueSize ) ///< Reach end of the array element
        FHeadPos -= FQueueSize;  ///< Roll to the begin of the array.
    FCount--; ///< Dequeue OK! Reduce count
    return RtnObj;
}
//---------------------------------------------------------------------------
template <class T>
T* PtrQueue<T>::Dequeue( Int32 TimeOutSec )
{
    T* Obj;
    while( TRUE )
    {
        if( FCount == 0 ) ///< Queue empty
        {
            if( FInqueueEvent.WaitFor( TimeOutSec ) == FALSE )
                return NULL; ///< Wait inqueue event time out
        }
        else
        {
            if( (Obj = GetFromQueue( )) != NULL )///< Remove from queue
            {
                FDequeueEvent.SetEvent(); ///< Trigger the dequeue event.
                return Obj;
            }
            ///< No item in queue, try again.
        }
    }
}
//---------------------------------------------------------------------------
template <class T>
T* PtrQueue<T>::DequeueNoWait( void )
{
    T* Obj;

    if( (Obj = GetFromQueue( )) != NULL )///< Remove from queue
    {
        FDequeueEvent.SetEvent();///< Trigger the dequeue event.
        return Obj;
    }
    return NULL;
}
//---------------------------------------------------------------------------
template <class T>
void PtrQueue<T>::Clear( BOOL DeleteData )
{
    T* Del;
    while( ( Del = DequeueNoWait() ) != NULL ) 
    {
        if( DeleteData == TRUE )
            delete Del;
    }
}
//------------------------------------------------------------------------------
}	/* namespace UFC */
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
