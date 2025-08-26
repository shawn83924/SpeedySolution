//------------------------------------------------------------------------------------------------------------------------
#include "RCQGNameValueSet.h"
//------------------------------------------------------------------------------------------------------------------------
// class RCQGNameValueSet
//------------------------------------------------------------------------------------------------------------------------
RCQGNameValueSet::RCQGNameValueSet()
{
}
//------------------------------------------------------------------------------------------------------------------------
RCQGNameValueSet::RCQGNameValueSet( RCQGNameValueSet& NameValueInst )
{
	AssignNameValueSet( NameValueInst );
}
//------------------------------------------------------------------------------------------------------------------------
RCQGNameValueSet::~RCQGNameValueSet()
{
	FHashTable.clear();
}
//------------------------------------------------------------------------------------------------------------------------
void RCQGNameValueSet::AssignNameValueSet( RCQGNameValueSet& NameValueInst )
{
	TIterator iter;

	FHashTable.clear();
	for ( iter = NameValueInst.FHashTable.begin(); iter != NameValueInst.FHashTable.end(); iter++ )
		FHashTable.insert( make_pair( iter->first, iter->second ) );
}
//------------------------------------------------------------------------------------------------------------------------
const char* RCQGNameValueSet::GetValue( const char* Name )
{
	TIterator iter;
	if ( (iter = FHashTable.find(Name)) == FHashTable.end() )
	    return NULL;
	return iter->second.c_str();
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCQGNameValueSet::GetValue( const UFC::AnsiString& Name, UFC::AnsiString& Value )
{
	TIterator iter;
	if ( (iter = FHashTable.find(Name)) == FHashTable.end() )
	    return FALSE;
	Value = iter->second;
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void RCQGNameValueSet::SetValue( const char* Name, const char* Value )
{
	TIterator iter;
	if ( (iter = FHashTable.find(Name)) == FHashTable.end() )
		FHashTable.insert( make_pair( Name, Value ) );
	else
		iter->second = Value;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCQGNameValueSet::DeleteNameValue( const UFC::AnsiString& Name  )
{
	TIterator iter;
	if ( (iter = FHashTable.find(Name)) == FHashTable.end() )
		return FALSE;
	FHashTable.erase(iter);
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCQGNameValueSet::GetNameValueByIndex( int Element, UFC::AnsiString& Name, UFC::AnsiString& Value )
{
	int i = 0;
	TIterator iter;
	for ( iter = FHashTable.begin(); iter != FHashTable.end(); iter++ )
	{
		if ( i == Element )
		{
			Name  = iter->first;   
			Value = iter->second; 
			return TRUE;
		}
		i++;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------
void RCQGNameValueSet::SaveToStream( UFC::PStream* Stream )
{
	UFC::NInt32 N32 = (Int32)FHashTable.size();
	N32.SaveToStream( Stream );

	TIterator iter;
	for ( iter = FHashTable.begin(); iter != FHashTable.end(); iter++ )
	{
		iter->first.SaveToStream( Stream ); ///< Write Name string
		iter->second.SaveToStream( Stream );///< Write value String
	}
}
//------------------------------------------------------------------------------------------------------------------------
void RCQGNameValueSet::LoadFromStream( UFC::PStream* Stream )
{
	UFC::AnsiString	Name, Value;
	UFC::NInt32		PairCount;

	FHashTable.clear();
	PairCount.LoadFromStream( Stream );
	for ( int i = 0; i < (Int32) PairCount; i++ )
	{
		Name.LoadFromStream( Stream );
		Value.LoadFromStream( Stream );
		FHashTable.insert( make_pair( Name, Value ) );
	}
}
//------------------------------------------------------------------------------------------------------------------------
RCQGNameValueSet& RCQGNameValueSet::operator = ( RCQGNameValueSet& rhs )
{
	AssignNameValueSet( rhs );
	return *this;
}
//------------------------------------------------------------------------------------------------------------------------
void RCQGNameValueSet::Print()
{
	TIterator iter;

	printf("------------------- Begin ------------------\n" );
	for (iter = FHashTable.begin(); iter != FHashTable.end(); iter++)
		printf("Name:%s Value:%s\n", iter->first.c_str(), iter->second.c_str() );
	printf("-------------------- End -------------------\n" );
}
//------------------------------------------------------------------------------------------------------------------------
