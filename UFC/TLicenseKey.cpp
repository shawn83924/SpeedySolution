#include "TLicenseKey.h"
#include "NetUtility.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
TLicenseKey::TLicenseKey( const AnsiString& Plat, const AnsiString& Prod, const AnsiString& Host, const AnsiString& Date )
:FKey( "" )
,FPlat( Plat )
,FProd( Prod )
,FHost( Host )
,FDate( Date )
{
    if( (FProd.Length() > 0) && (FHost.Length() > 0) && (FDate.Length() > 0) )
    {
	AnsiString Str;
	Str.Printf( "[%s]-[%s]-[%s]-[%08d]", FPlat.UpperCase().c_str(), FProd.UpperCase().c_str(), FHost.UpperCase().c_str(), abs(FDate.ToInt()) % 100000000 );

        UInt8 Buffer[16];
        MD5 KeyMD5( (UInt8*)(Str.c_str()), Str.Length() );
        memcpy( Buffer, KeyMD5.GetCode(), 16 );;

        Int32 n1 = abs( NInt32::ToInt32( Buffer )      % 10000 );
        Int32 n2 = abs( NInt32::ToInt32( Buffer + 4 )  % 10000 );
        Int32 n3 = abs( NInt32::ToInt32( Buffer + 8 )  % 10000 );
        Int32 n4 = abs( NInt32::ToInt32( Buffer + 12 ) % 10000 );

        FKey.Printf( "%04d-%04d-%04d-%04d", n1, n2, n3, n4 );
    }
}
//---------------------------------------------------------------------------
bool TLicenseKey::CompareKey( const AnsiString& Key )
{
    if( (FKey.Length() > 0) && (Key.Length() > 0) )
        return ( FKey == Key );

    return false;
}
//---------------------------------------------------------------------------
AnsiString TLicenseKey::ToString( void )
{
	return FKey;
}
//---------------------------------------------------------------------------
AnsiString TLicenseKey::GetPlat( void )
{
	return FPlat;
}
//---------------------------------------------------------------------------
AnsiString TLicenseKey::GetProd( void )
{
	return FProd;
}
//---------------------------------------------------------------------------
AnsiString TLicenseKey::GetHost( void )
{
	return FHost;
}
//---------------------------------------------------------------------------
AnsiString TLicenseKey::GetDate( void )
{
	return FDate;
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

