//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#include "APIParams.h"
#include "UFC.h"
//---------------------------------------------------------------------------
// Class TCTPParam define param for CTP callback function.
//---------------------------------------------------------------------------
TCTPParam::TCTPParam( )
:FType( cdtNotUse )
{
}
//---------------------------------------------------------------------------
TCTPParam::TCTPParam( int Val )
:FType( cdtInt )
{
	FData.AsInt = Val;
}
//---------------------------------------------------------------------------
TCTPParam::TCTPParam( bool Val )
:FType( cdtBool )
{
	FData.AsBool = Val;
}
//---------------------------------------------------------------------------
TCTPParam::TCTPParam( void* Ptr, int Size )
:FType( cdtRowData )
{
	if( Ptr != NULL )
	{
		FData.AsRowdata = new char[ Size ];
		memcpy( FData.AsRowdata, Ptr, Size );
	}
	else
		FData.AsRowdata = NULL;
}
//---------------------------------------------------------------------------
TCTPParam::~TCTPParam()
{
	if( FType == cdtRowData && FData.AsRowdata != NULL )
		delete [] FData.AsRowdata;
}
//---------------------------------------------------------------------------
//  CTP callback function params object.
//  Up to 4 params
//---------------------------------------------------------------------------
TCTPParams::TCTPParams( int Val )
:Param1( Val )
{
}
//---------------------------------------------------------------------------
TCTPParams::TCTPParams( bool Val )
:Param1( Val )
{
}
//---------------------------------------------------------------------------
TCTPParams::TCTPParams( void* Ptr1, int Size1)
:Param1( Ptr1, Size1 )
{

}
//---------------------------------------------------------------------------
TCTPParams::TCTPParams( int Val, int Val1 )
:Param1( Val )
,Param2( Val1 )
{

}
//---------------------------------------------------------------------------
TCTPParams::TCTPParams( void* Ptr1, int Size1, int Val1, bool Val2 )
:Param1( Ptr1, Size1 )
,Param2( Val1 )
,Param3( Val2 )
{

}
//---------------------------------------------------------------------------
TCTPParams::TCTPParams( void* Ptr1, int Size1, void* Ptr2, int Size2 )
:Param1( Ptr1, Size1 )
,Param2( Ptr2, Size2 )
{

}
//---------------------------------------------------------------------------
TCTPParams::TCTPParams( void* Ptr1, int Size1, void* Ptr2, int Size2, int Val3, bool Val4 )
:Param1( Ptr1, Size1 )
,Param2( Ptr2, Size2 )
,Param3( Val3 )
,Param4( Val4 )
{

}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
