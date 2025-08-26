#ifndef TLicenseKeyH
#define TLicenseKeyH
//---------------------------------------------------------------------------
#include "MD5.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
class TLicenseKey
{
private:
	AnsiString FKey;
	AnsiString FPlat;
	AnsiString FProd;
	AnsiString FHost;
	AnsiString FDate;

public:
	TLicenseKey( const AnsiString& Plat, const AnsiString& Prod, const AnsiString& Host, const AnsiString& Date );

public:
	bool       CompareKey( const AnsiString& Key );
	AnsiString ToString( void );

public:
	AnsiString GetPlat( void );
	AnsiString GetProd( void );
	AnsiString GetHost( void );
	AnsiString GetDate( void );
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
