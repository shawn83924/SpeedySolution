#include "Exception.h"
namespace nsDataStore
{
//---------------------------------------------------------------------------
DataStoreException::DataStoreException( void )
:UFC::Exception( "" )
,FMessage( "" )
{

}
//---------------------------------------------------------------------------
DataStoreException::~DataStoreException( void ) throw()
{

}
//---------------------------------------------------------------------------
void DataStoreException::Throw( const char* FormatStr, ... )
{
	FMessage = "";
        
        if( FormatStr != NULL )
	{
		char Buffer[ 512 ];
		Buffer[0] = '\0';

		va_list va;
		va_start( va , FormatStr );
		int Count = vsnprintf( Buffer, 512, FormatStr, va );
		va_end( va );

		if( Count > 0 )
			FMessage = Buffer;
	}
        
	throw( *this );
}
//---------------------------------------------------------------------------
const char* DataStoreException::what( void ) const throw()
{
	return FMessage.c_str();
}
//---------------------------------------------------------------------------
}

