//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "FormAbout.h"
#include "..\UFC\NetUtility.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAboutForm *AboutForm;
//---------------------------------------------------------------------------
__fastcall TAboutForm::TAboutForm(TComponent* Owner)
    : TForm(Owner)
{
    DateLabel->Caption = __DATE__;
	DWORD n = GetFileVersionInfoSizeA( PROCESS_NAME, NULL);
	if( n > 0 )
	{
    	char *pBuf = new char[ n ];
		VS_FIXEDFILEINFO *vsinfo;
        UINT Len;

		GetFileVersionInfoA( PROCESS_NAME, 0, n, pBuf );

   	    if( VerQueryValueA( pBuf, "\\", (void **)&vsinfo, &Len))
		{
			AnsiString VersionString;
			int Version  = HIWORD(vsinfo->dwFileVersionMS);
			int MVersion = LOWORD(vsinfo->dwFileVersionMS);
			int Build    = LOWORD(vsinfo->dwFileVersionLS);

			VersionString.printf("%d.%d build:%d", Version, MVersion, Build );
			VerLabel->Caption = VersionString;
		}
		delete [] pBuf;
	}
	String CopyrightString;
	CopyrightString.printf( L"Speedy Center®\nCopyright© 2002-%d MDBS Digital Technology Co., Ltd.", UFC::ThisYear() );
	CopyRightLabel->Caption = CopyrightString;
}
//---------------------------------------------------------------------------

