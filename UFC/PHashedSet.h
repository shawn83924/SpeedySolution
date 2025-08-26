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
#ifndef HASHED_SET_H
#define HASHED_SET_H
#include "stl_config.h"
#include <set>
//------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------
using namespace std;
#ifndef __GXX_EXPERIMENTAL_CXX0X__     ///< -std=c++0x
using namespace Sgi;
#endif
//------------------------------------------------------------------------------
template <class T>
class PHashedSet
{
private:
#if (__BORLANDC__ >= 0x0580 || _MSC_VER >= 1600)  ///< For BDS 2006 Dinkum C++ Library / Visual C++ >=10
	  typedef hash_set< T >					   HashSet;
	  typedef typename hash_set< T >::iterator TIterator;
#else 
	  typedef hash_set< T, hash<T> > HashSet;
	  typedef typename hash_set< T, hash<T> >::iterator TIterator;
#endif

private:
	  HashSet   FHashSet;
	  TIterator Fiter;
public:

#if (__BORLANDC__ >= 0x0580 || _MSC_VER >= 1600)  ///< For BDS 2006 Dinkum C++ Library / Visual C++ (>=10)
	  PHashedSet( UInt32 Size = 32 ):FHashSet(){ }
#else
	  PHashedSet( UInt32 Size = 32 ):FHashSet( Size ){}
#endif
	  void  Add( const T& New )    { FHashSet.insert( New );  }
	  void  Clear( void )          { FHashSet.clear(); }
	  Int32 Delete( const T& Del ) { return (Int32)FHashSet.erase( Del ); }
	  Int32 ItemCount( void )      { return (Int32)FHashSet.size(); }
	  T GetItem( Int32 Index )     { return (*this)[ Index ]; }
	  //------------------------------------------------------------------------------
	  T operator [] ( Int32 Index )
	  {
		 TIterator iter = FHashSet.begin();
		 for( register Int32 i = 0; i < Index; i++ )
			  iter++;
		 return *iter;
	  }
	  //------------------------------------------------------------------------------
	  BOOL Exists( const T& Find )
	  {
		  TIterator it = FHashSet.find( Find );
		  if( it != FHashSet.end() )
				  return TRUE;
		  return FALSE;
	  }
	  //------------------------------------------------------------------------------
	  BOOL First( T& Item )
	  {
		  if( FHashSet.size() == 0 )
			  return FALSE;
		  Fiter = FHashSet.begin();
		  Item = *Fiter;
		  return TRUE;
	  }
	  //------------------------------------------------------------------------------
	  BOOL Next( T& Item )
	  {
		  Fiter++;
		  if( Fiter == FHashSet.end() )
			  return FALSE;
		  Item =  *Fiter;
		  return TRUE;
	  }
	  //------------------------------------------------------------------------------
};
//------------------------------------------------------------------------------
template <class T>
class PSet
{
private:
	  typedef set< T >					  TSet;
	  typedef typename set< T >::iterator TIterator;
private:
	  TSet      FSet;
	  TIterator Fiter;
public:

	  PSet(  ):FSet(){ }
	  void  Add( const T& New )    { FSet.insert( New );  }
	  void  Clear( void )          { FSet.clear(); }
	  Int32 Delete( const T& Del ) { return (Int32)FSet.erase( Del ); }
	  Int32 ItemCount( void )      { return (Int32)FSet.size(); }
	  T GetItem( Int32 Index )     { return (*this)[ Index ]; }
	  //------------------------------------------------------------------------------
	  T operator [] ( Int32 Index )
	  {
		 TIterator iter = FSet.begin();
		 for( register Int32 i = 0; i < Index; i++ )
			  iter++;
		 return *iter;
	  }
	  //------------------------------------------------------------------------------
	  BOOL Exists( const T& Find )
	  {
		  TIterator it = FSet.find( Find );
		  if( it != FSet.end() )
				  return TRUE;
		  return FALSE;
	  }
	  //------------------------------------------------------------------------------
	  BOOL First( T& Item )
	  {
		  if( FSet.size() == 0 )
			  return FALSE;
		  Fiter = FSet.begin();
		  Item = *Fiter;
		  return TRUE;
	  }
	  //------------------------------------------------------------------------------
	  BOOL Next( T& Item )
	  {
		  Fiter++;
		  if( Fiter == FSet.end() )
			  return FALSE;
		  Item =  *Fiter;
		  return TRUE;
	  }
	  //------------------------------------------------------------------------------
};
//------------------------------------------------------------------------------
typedef PHashedSet<int>        PIntHashedSet;
typedef PHashedSet<Int32>      PInt32HashedSet;
typedef PHashedSet<AnsiString> PStringHashedSet;
//------------------------------------------------------------------------------
typedef PSet<int>        PIntSet;
typedef PSet<Int32>      PInt32Set;
typedef PSet<AnsiString> PStringSet;
//------------------------------------------------------------------------------
}	/* namespace UFC */
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
