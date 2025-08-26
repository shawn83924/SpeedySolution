#include "EmbadedResource.h"
#include "../TMPOrderConnection/MessageDataFormat.h"
//------------------------------------------------------------------------------
#ifndef _WIN32
//------------------------------------------------------------------------------
HRSRC FindResource( HMODULE hModule, const UFC::AnsiString& Name, const UFC::AnsiString& Type )
{
	if( (hModule != NULL) && (Name.Length() > 0) )
		return hModule->GetObjectByKey( Name );
	return NULL;
}
//------------------------------------------------------------------------------
HGLOBAL LoadResource( HMODULE hModule, HRSRC hResInfo )
{
	return hResInfo;
}
//------------------------------------------------------------------------------
DWORD SizeofResource( HMODULE hModule, HRSRC hResInfo )
{
	if( hResInfo != NULL )
		return UFC::NInt32::ToInt32( hResInfo );
	return 0;
}
//------------------------------------------------------------------------------
void* LockResource( HGLOBAL hResData )
{
	if( hResData != NULL )
		return (hResData + 4);
	return NULL;
}
//------------------------------------------------------------------------------
void UnlockResource( HGLOBAL hResData )
{
}
//------------------------------------------------------------------------------
#endif	/* #ifndef _WIN32 */
//------------------------------------------------------------------------------
//
// class TResource
//
//------------------------------------------------------------------------------
TResource::TResource( HMODULE hMODULE, const UFC::AnsiString& Name, const UFC::AnsiString& Type )
:FSize( 0 )
,FData( NULL )
{
    HRSRC hRSRC = FindResource( hMODULE, Name, Type );
    if( hRSRC != NULL )
    {
        hGLOBAL = LoadResource( hMODULE, hRSRC );
        if( hGLOBAL != NULL )
        {
            FSize = SizeofResource( hMODULE, hRSRC );
            if( FSize > 0 )
            {
                FData = (UInt8*)( LockResource( hGLOBAL ) );
            }
        }
    }    
    if( (hGLOBAL == NULL) || (FSize == 0) || (FData == NULL) )
        throw( UFC::Exception( "null resource" ) );
}
//------------------------------------------------------------------------------
TResource::~TResource( void )
{
    if( hGLOBAL != NULL )
        UnlockResource( hGLOBAL );
}
//------------------------------------------------------------------------------
DWORD TResource::GetSize( void )
{
    return FSize;
}
//------------------------------------------------------------------------------
UInt8* TResource::GetData( void )
{
    return FData;
}
//------------------------------------------------------------------------------
void TResource::SaveToFile( const UFC::AnsiString& FileName )
{
    UFC::FileStream fs( FileName, O_CREAT | O_WRONLY
#ifdef _WIN32
	| O_BINARY
#endif
	);
	fs.Write( GetData(), GetSize() );
	fs.Flush();
}
//------------------------------------------------------------------------------
bool GetResourceName( DWORD ResID, UFC::AnsiString& Name )
{
	switch(ResID)
	{
		case IDR_TMP_L10: Name = "L10.ini";break;
		case IDR_TMP_L20: Name = "L20.ini";break;
		case IDR_TMP_L30: Name = "L30.ini";break;
		case IDR_TMP_L40: Name = "L40.ini";break;
		case IDR_TMP_L41: Name = "L41.ini";break;
		case IDR_TMP_L42: Name = "L42.ini";break;
		case IDR_TMP_L50: Name = "L50.ini";break;
		case IDR_TMP_L60: Name = "L60.ini";break;
		case IDR_TMP_L70: Name = "L70.ini";break;
		case IDR_TMP_L80: Name = "L80.ini";break;
		///<  For TMP Application level protocol
		case IDR_TMP_R01: Name = "R01.ini";break;
		case IDR_TMP_R02: Name = "R02.ini";break;
		case IDR_TMP_R22: Name = "R22.ini";break;
		case IDR_TMP_R03: Name = "R03.ini";break;
		case IDR_TMP_R04: Name = "R04.ini";break;
		case IDR_TMP_R05: Name = "R05.ini";break;
		case IDR_TMP_R07: Name = "R07.ini";break;
		case IDR_TMP_R08: Name = "R08.ini";break;
		case IDR_TMP_R09: Name = "R09.ini";break;
		case IDR_TMP_R11: Name = "R11.ini";break;
		case IDR_TMP_R12: Name = "R12.ini";break;
		case IDR_TMP_R13: Name = "R13.ini";break;
		case IDR_TMP_R14: Name = "R14.ini";break;
		case IDR_TMP_R15: Name = "R15.ini";break;
		case IDR_TMP_R16: Name = "R16.ini";break;
		///< Error message table
		case IDR_ERROR_ENG: Name = "TAIFEXErrorEN.ini"; break;
		case IDR_ERROR_CHT: Name = "TAIFEXErrorCH.ini"; break;
		default: return false;
	}
	return true;
}
//------------------------------------------------------------------------------
