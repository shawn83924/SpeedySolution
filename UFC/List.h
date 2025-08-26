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
#ifndef ListH
#define ListH
//------------------------------------------------------------------------------
#include "stl_config.h"
//------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------
const int LIST_MINIUM_SIZE = 16;
const int PTR_QUEUE_MAX_SIZE = 1024;
//------------------------------------------------------------------------------
#ifndef  __GXX_EXPERIMENTAL_CXX0X__ 
using namespace Sgi;
#endif
using namespace std;
//------------------------------------------------------------------------------
// elem1 <  elem2 fcmp returns an integer < 0
// elem1 == elem2 fcmp returns 0
// elem1 >  elem2 fcmp returns an integer > 0
//------------------------------------------------------------------------------
class CompareInterface 
{
public:
    virtual ~CompareInterface( void ) {}
    virtual int Compare( void* elem1, void* elem2 )= 0;
};
//------------------------------------------------------------------------------
class PList
{
private:
	Int32             FTotalAlloc;
	Int32             FPosition;
	LPVOID*           FArray;
        CompareInterface* FCompare;
private:
	void    Grow( void );
	void    Pack( void );
	void    SetCapacity( Int32 Capacity );
        Int32   append( void *Data );
	Int32   insert( Int32 Pos, void* Data );
public:
	// set Default initial size LIST_MINIUM_SIZE
	PList( CompareInterface* CMPFunc, Int32 InitialSize = LIST_MINIUM_SIZE );
	PList( Int32 InitialSize = LIST_MINIUM_SIZE );
	virtual ~PList() ;
	Int32    Add( void *Object );
	Int32    Insert( Int32 Pos, void* Object);
	void*    Delete( int Index );
	Int32    Remove( void *Object ) ;
	Int32    ItemCount( void ) const { return FPosition; }
	Int32    IndexOf( void *Object ) ;
	void*    GetItem( Int32 Index ) ;
	void*    GetConstItem( Int32 Index ) const;
	void*    SetItem( Int32 Index, void* NewObject );
	void     Clear( void ) ;
	void**   GetBuffer( void ) { return FArray; }
};
//------------------------------------------------------------------------------
template <class T>
class List
{
private:
    vector<T> FArray;
public:
    List() {};
	void   Add( const T& Object )            { FArray.push_back( Object ); }
	void   Delete( int Index )               { FArray.erase( FArray.begin()+Index ); }
	Int32  ItemCount( void ) const           { return (Int32)FArray.size();}
	void   Clear( void )                     { FArray.clear(); }
	T&     GetItem( int Index )              { return FArray[ Index ]; }
	T&     operator []( Int32 Index )        { return FArray[ Index ]; }
    Int32  IndexOf( const T& ) ;    
};
//------------------------------------------------------------------------------
template <class T>
Int32 List<T>::IndexOf( const T& Val )
{
    for( register int i = 0; i < (Int32)FArray.size(); i ++ )
    {
        if( Val == FArray[ i ] )
            return i;
    }
    return -1;
}
//------------------------------------------------------------------------------
typedef List<UFCType::Int32>     PInt32List;
typedef List<UFCType::Int8>      PInt8List;
typedef List<UFCType::UInt8>     PUInt8List;
typedef List<long>               PLongList;
typedef List<double>             PDoubleList;
//------------------------------------------------------------------------------
class PStringList
{
private:
    vector<UFC::AnsiString> FArray;
public:
    PStringList() {};
    PStringList( const PStringList& Ref );
    void                 AddFront( const UFC::AnsiString& String ) { FArray.insert( FArray.begin(), String); }
    void                 Add( const UFC::AnsiString& String ) { FArray.push_back( String ); }
    void                 Delete( int Index )                  { FArray.erase( FArray.begin()+Index ); }
    Int32                ItemCount( void )const               { return (Int32)FArray.size();}
    void                 Clear( void )                        { FArray.clear(); }
    const UFC::AnsiString&  GetItem( int Index ) const        { return FArray[ Index ]; }
    UFC::AnsiString&     operator []( Int32 Index )           { return FArray[ Index ]; }
    void                 SetStrings( const UFC::AnsiString& Strings, const UFC::AnsiString& Delimiter );
    void                 SetStrings( const AnsiString& Strings, const AnsiString& Delimiter, bool KeepEmptyField );
    void 		         SetQuoteStrings( const UFC::AnsiString& Strings, const UFC::AnsiString& Delimiter );
    Int32 		         IndexOf( const UFC::AnsiString& FindString );
    Int32 		         CopyFrom( UFC::PStringList& SrcList );
    void                 Save( const UFC::AnsiString& FileName );
    UFC::AnsiString      DelimiterString( char Delimiter = '^' );    
};
//------------------------------------------------------------------------------
template <class T>
class PtrList
{
private:
	BOOL     FOwnObjects;
	PList   *FList;
public:
	PtrList( Int32 InitialSize = LIST_MINIUM_SIZE );
	virtual  ~PtrList();
	void      Clear( void );
	void      Insert( Int32 Pos, T* Object );
	Int32     Add( T* Object );
        Int32     ItemCount( void ) const;
	Int32     IndexOf( T* Object );
	Int32     Remove( T* Object );
	T*        Delete( int Index );
	T*        GetItem( Int32 Index ) throw();
	T*        SetItem( Int32 Index, T* NewObject );
        T**       GetBuffer( void );
	void      SetOwnObjects( BOOL Own ){ FOwnObjects = Own; }
};
//------------------------------------------------------------------------------
template <class T>
PtrList<T>::PtrList( Int32 InitialSize )
{
    FOwnObjects = FALSE;
	FList = new PList( InitialSize );
}
//------------------------------------------------------------------------------
template <class T>
PtrList<T>::~PtrList()
{
	Clear();
	delete FList;
}
//---------------------------------------------------------------------------
template <class T>
Int32 PtrList<T>::Add( T* Object )
{
	return FList->Add( (void*)Object );
}
//---------------------------------------------------------------------------
template <class T>
void PtrList<T>::Insert( Int32 Pos, T* Object)
{
	FList->Insert( Pos, (void*)Object );
}
//---------------------------------------------------------------------------
template <class T>
T* PtrList<T>::Delete( int Index )
{
	return ((T*)FList->Delete( Index ));
}
//---------------------------------------------------------------------------
template <class T>
Int32 PtrList<T>::Remove( T* Object )
{
	return FList->Remove((void*)Object );
}
//---------------------------------------------------------------------------
template <class T>
Int32 PtrList<T>::ItemCount( void ) const
{
	return FList->ItemCount();
}
//---------------------------------------------------------------------------
template <class T>
Int32 PtrList<T>::IndexOf( T* Object )
{
	return FList->IndexOf((void*)Object);
}
//---------------------------------------------------------------------------
template <class T>
T* PtrList<T>::GetItem( Int32 Index ) throw()
{
	return (( T*)FList->GetItem( Index ));
}
//---------------------------------------------------------------------------
template <class T>
T* PtrList<T>::SetItem( Int32 Index, T* Object )
{
	return ((T*)FList->SetItem( Index, (void*)Object));
}
//---------------------------------------------------------------------------
template <class T>
void PtrList<T>::Clear( void )
{
	if( FOwnObjects == TRUE )
	{
		for( Int32 i = ItemCount() -1; i >=0;  i-- )
			 delete GetItem( i );
	}
	FList->Clear();
}
//---------------------------------------------------------------------------
template <class T>
T** PtrList<T>::GetBuffer( void )
{
	return (( T** )FList->GetBuffer());
}
//---------------------------------------------------------------------------
extern Int32 Hash( const AnsiString& Str );
//---------------------------------------------------------------------------
}	/* namespace UFC */
//---------------------------------------------------------------------------
#endif
