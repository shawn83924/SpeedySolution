//------------------------------------------------------------------------------------------------------------------------
#ifndef __RCQGNameValueSet_H
#define __RCQGNameValueSet_H
//------------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../UFC/PClientSocket.h"
#include "../UFC/MemoryStream.h"
//------------------------------------------------------------------------------------------------------------------------
using namespace std;
#ifndef __GXX_EXPERIMENTAL_CXX0X__    
using namespace Sgi;
#endif
//------------------------------------------------------------------------------------------------------------------------
class RCQGNameValueSet
{
private:
#if (__BORLANDC__ >= 0x0580) ///< For BDS 2006 Dinkum C++ Library
	typedef hash_map< UFC::AnsiString, UFC::AnsiString >			THashTable;
	typedef hash_map< UFC::AnsiString, UFC::AnsiString >::iterator	TIterator;
#else
	typedef hash_map< UFC::AnsiString, UFC::AnsiString, hash<UFC::AnsiString> >				THashTable;
	typedef hash_map< UFC::AnsiString, UFC::AnsiString, hash<UFC::AnsiString> >::iterator	TIterator;
#endif
	THashTable FHashTable;
private:
	void		AssignNameValueSet( RCQGNameValueSet& NameValueInst );
public:
	const char*	GetValue( const char* Name );
	BOOL		GetValue( const UFC::AnsiString& Name, UFC::AnsiString& Value );
	void		SetValue( const char* Name, const char* Value );
	BOOL		DeleteNameValue( const UFC::AnsiString& Name );
	BOOL		GetNameValueByIndex( int Element, UFC::AnsiString& Name, UFC::AnsiString& Value );
public:
	BOOL		IsEmpty( ) { return !(FHashTable.size() > 0); }
	void		SaveToStream( UFC::PStream* Stream );
	void		LoadFromStream( UFC::PStream* Stream  );
	void		Print();
	RCQGNameValueSet& operator = ( RCQGNameValueSet& rhs );
public:
	RCQGNameValueSet();
	RCQGNameValueSet( RCQGNameValueSet& NameValueInst );
	~RCQGNameValueSet();
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
