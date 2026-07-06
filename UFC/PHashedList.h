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
#ifndef HASHED_LIST_H
#define HASHED_LIST_H
#include "stl_config.h"
#include "List.h"

//------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------
using namespace std;
#ifndef  __GXX_EXPERIMENTAL_CXX0X__ 
using namespace Sgi;
#endif
//------------------------------------------------------------------------------
template <class K, class T>
class PHashedList
{
private:

#if (__BORLANDC__ >= 0x0580 || _MSC_VER >= 1600)  ///< For BDS 2006 Dinkum C++ Library / Visual C++ >=10
    typedef hash_map< K, LPVOID > HashTable;
    typedef typename hash_map< K, LPVOID >::iterator Iterator;
#else
	typedef hash_map< K, LPVOID, hash<K> > HashTable;
	typedef typename hash_map< K, LPVOID, hash<K> >::iterator Iterator;
#endif

private:
    HashTable    FHashTable;
    Iterator     FIter;
public:
    PHashedList():FIter( FHashTable.begin() ){}

#if (__BORLANDC__ >= 0x0580 || _MSC_VER >= 1600)  ///< For BDS 2006 Dinkum C++ Library / Visual C++ >=10
	PHashedList( UInt32 Size ):FHashTable( ),FIter( FHashTable.begin() ){}
#else
	PHashedList( UInt32 Size ):FHashTable( Size ),FIter( FHashTable.begin() ){}
#endif
    ~PHashedList(){}
    void     Clear( void );
	///< Functions access objects by using hash table.
	BOOL     Add( const K& Key, T Object );
	Int32    DeleteByKey( const K& Key );
	T        GetObjectByKey( const K& Key );
	T        SetObjectByKey( const K& Key, T Object );
        BOOL     SetObjectByKeyIfExists( const K& Key, T Object );
	BOOL     IsExists( const K& Key );
	///< Random access function.
	Int32    ItemCount( void ) const { return (Int32)FHashTable.size(); }
	T        GetItem( Int32 Index );
	BOOL     GetItem( Int32 Index, K& Key, T& Obj );
	///< Sequential access function.
	T        First( void );
	T        Next( void );
	K        CurrentKey( void );
};
//------------------------------------------------------------------------------
template <class K,class T>
BOOL PHashedList<K,T>::Add( const K& Key, T Object )
{
    return FHashTable.insert( std::make_pair(Key, (LPVOID)Object) ).second ? TRUE : FALSE;
}
//------------------------------------------------------------------------------
template <class K,class T>
Int32 PHashedList<K,T>::DeleteByKey( const K& Key )
{
    return (Int32)FHashTable.erase( Key );
}
//------------------------------------------------------------------------------
template <class K,class T>
T PHashedList<K,T>::GetObjectByKey( const K& Key )
{
    Iterator iter = FHashTable.find( Key );
    if( iter != FHashTable.end() )
	return (T)iter->second;
    return NULL;
}
//------------------------------------------------------------------------------
template <class K,class T>
BOOL PHashedList<K,T>::IsExists( const K& Key )
{
    Iterator iter = FHashTable.find( Key );
    if( iter != FHashTable.end() )
	return TRUE;
    return FALSE;
}
//------------------------------------------------------------------------------
template <class K,class T>
T PHashedList<K,T>::SetObjectByKey( const K& Key, T Object )
{
    Iterator iter = FHashTable.find( Key );
    if( iter != FHashTable.end() ) ///< Key exists! replace the old one.
    {
        T OldObj = (T)iter->second; ///< Keep the old object.
        iter->second = (LPVOID)Object;   ///< Assign the new one.
        return OldObj;
    }
    ///< Add a new key/Object pair.
    FHashTable[ Key ] = (LPVOID)Object;
    return NULL;
}
//------------------------------------------------------------------------------
template <class K,class T>
BOOL PHashedList<K,T>::SetObjectByKeyIfExists( const K& Key, T Object )
{
    Iterator iter = FHashTable.find( Key );
    
    if( iter == FHashTable.end() ) ///< Key not exists!
    {
        ///< Add a new key/Object pair.
        FHashTable[ Key ] = (LPVOID)Object;
        return TRUE;
    }
    return FALSE;    
}
//------------------------------------------------------------------------------
template <class K,class T>
T PHashedList<K,T>::GetItem( Int32 Index )
{
	if( Index >= (Int32)FHashTable.size() || Index < 0 )
		return NULL;
	Iterator iter = FHashTable.begin();
	for( register Int32 i = 0; i < Index; i++ )
		 iter++;
	return (T)iter->second;
}
//------------------------------------------------------------------------------
template <class K,class T>
BOOL PHashedList<K,T>::GetItem( Int32 Index, K& Key, T& Obj )
{
   if( Index >= (Int32)FHashTable.size() || Index < 0 )
	   return FALSE;
   Iterator iter = FHashTable.begin();
   for( register Int32 i = 0; i < Index; i++ )
		iter++;
   Key = iter->first;
   Obj = (T)iter->second;
   return TRUE;
}
//------------------------------------------------------------------------------
template <class K,class T>
void  PHashedList<K,T>::Clear( void )
{
	FHashTable.clear();
}
//------------------------------------------------------------------------------
template <class K,class T>
T  PHashedList<K,T>::First( void )
{
	if( FHashTable.size() == 0 )
		return NULL;
	FIter = FHashTable.begin();
	return (T)FIter->second;
}
//------------------------------------------------------------------------------
template <class K,class T>
T  PHashedList<K,T>::Next( void )
{
	FIter++;
	if( FIter == FHashTable.end() )
		return NULL;
	return (T)FIter->second;
}
//------------------------------------------------------------------------------
template <class K,class T>
K PHashedList<K,T>::CurrentKey( void )
{
	return (T)FIter->first;
}
//------------------------------------------------------------------------------
//
// Please use PHashMap to replace PHashedList
// PHashedList not support 64bit compile.
//
//------------------------------------------------------------------------------
template <class K, class T>
class PHashMap
{
private:
#if (__BORLANDC__ >= 0x0580 || _MSC_VER >= 1600)  ///< For BDS 2006 Dinkum C++ Library / Visual C++ >=10
	typedef hash_map< K, T > HashTable;
	typedef typename hash_map< K, T >::iterator Iterator;
#else
	typedef hash_map< K, T, hash<K> > HashTable;
	typedef typename hash_map< K, T, hash<K> >::iterator Iterator;
#endif

private:
    HashTable    FHashTable;
    Iterator     FIter;
public:
    PHashMap():FIter( FHashTable.begin() ){}

#if (__BORLANDC__ >= 0x0580 || _MSC_VER >= 1600)  ///< For BDS 2006 Dinkum C++ Library / Visual C++ >=10
    PHashMap( UInt32 ):FHashTable( ),FIter( FHashTable.begin() ){}
#else
    PHashMap( UInt32 Size ):FHashTable( Size ),FIter( FHashTable.begin() ){}
#endif
    ~PHashMap(){}
    void     Clear( void );
    ///< Functions access objects by using hash table.
    BOOL     Add( const K& Key, T Object );
    Int32    DeleteByKey( const K& Key );
    BOOL     IsExists( const K& Key );
    BOOL     GetObjectByKey( const K& Key, T& Object );
    T        SetObjectByKey( const K& Key, T Object );
    BOOL     SetObjectByKeyIfExists( const K& Key, T Object );
    ///< Random access function.
    Int32    ItemCount( void ) const { return (Int32)FHashTable.size(); }
    BOOL     GetItem( Int32 Index, T& Obj );
    BOOL     GetItem( Int32 Index, K& Key, T& Obj );
    ///< Sequential access function.
    T        First( void );
    T        Next( void );
};
//------------------------------------------------------------------------------
template <class K,class T>
BOOL PHashMap<K,T>::Add( const K& Key, T Object )
{
    return FHashTable.insert( std::make_pair(Key, (T)Object) ).second ? TRUE : FALSE;
}
//------------------------------------------------------------------------------
template <class K,class T>
Int32 PHashMap<K,T>::DeleteByKey( const K& Key )
{
    return (Int32)FHashTable.erase( Key );
}
//------------------------------------------------------------------------------
template <class K,class T>
BOOL PHashMap<K,T>::GetObjectByKey( const K& Key, T& Object )
{
    Iterator iter = FHashTable.find( Key );
    if( iter != FHashTable.end() )
    {
		Object = (T)iter->second;
		return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
template <class K,class T>
T PHashMap<K,T>::SetObjectByKey( const K& Key, T Object )
{
    Iterator iter = FHashTable.find( Key );

    if( iter != FHashTable.end() ) ///< Key exists! replace the old one.
    {
        T OldObj = iter->second;
        iter->second = Object;     ///< Assign the new one.
        return OldObj;
    }
    else
    {
        FHashTable[ Key ] = (T)Object;///< Add a new key/Object pair.
        return Object;
    }
}
//------------------------------------------------------------------------------
template <class K,class T>
BOOL PHashMap<K,T>::SetObjectByKeyIfExists( const K& Key, T Object )
{
    Iterator iter = FHashTable.find( Key );
    
    if( iter == FHashTable.end() ) ///< Key not exists!
    {
        ///< Add a new key/Object pair.
        FHashTable[ Key ] = Object;
        return TRUE;
    }
    return FALSE;    
}
//------------------------------------------------------------------------------
template <class K,class T>
BOOL PHashMap<K,T>::IsExists( const K& Key )
{
    if( FHashTable.find( Key ) != FHashTable.end() )
	return TRUE;
    return FALSE;
}
//------------------------------------------------------------------------------
template <class K,class T>
BOOL PHashMap<K,T>::GetItem( Int32 Index, T& Obj )
{
    if( Index >= (Int32)FHashTable.size() || Index < 0 )
        return FALSE;
    Iterator iter = FHashTable.begin();
    for( register Int32 i = 0; i < Index; i++ )
         iter++;
    Obj = (T)iter->second;
	return TRUE;
}
//------------------------------------------------------------------------------
template <class K,class T>
BOOL PHashMap<K,T>::GetItem( Int32 Index, K& Key, T& Obj )
{
   if( Index >= (Int32)FHashTable.size() || Index < 0 )
       return FALSE;
   Iterator iter = FHashTable.begin();
   for( register Int32 i = 0; i < Index; i++ )
        iter++;
   Key = iter->first;
   Obj = (T)iter->second;
   return TRUE;
}
//------------------------------------------------------------------------------
template <class K,class T>
void  PHashMap<K,T>::Clear( void )
{
    FHashTable.clear();
}
//------------------------------------------------------------------------------
template <class K,class T>
T PHashMap<K,T>::First( void )
{
    if( FHashTable.size() == 0 )
        return NULL;
    FIter = FHashTable.begin();
    return (T)FIter->second;
}
//------------------------------------------------------------------------------
template <class K,class T>
T PHashMap<K,T>::Next( void )
{
    FIter++;
    if( FIter == FHashTable.end() )
        return NULL;
    return (T)FIter->second;
}
//------------------------------------------------------------------------------
}	/* namespace UFC */
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
