#include "EmbadedResource.h"
#include "../SpeedyAPIXE7/MessageDataFormat.h"
//------------------------------------------------------------------------------
#ifndef _WIN32
//------------------------------------------------------------------------------
HRSRC FindResourceA( HMODULE hModule, const UFC::AnsiString& Name, const UFC::AnsiString& Type )
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
TFormatResource::TFormatResource( HMODULE hMODULE, const UFC::AnsiString& Name, const UFC::AnsiString& Type )
:FSize( 0 )
,FData( NULL )
{
	HRSRC hRSRC = FindResourceA( hMODULE, Name, Type );
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
TFormatResource::~TFormatResource( void )
{
    if( hGLOBAL != NULL )
        UnlockResource( hGLOBAL );
}
//------------------------------------------------------------------------------
DWORD TFormatResource::GetSize( void )
{
    return FSize;
}
//------------------------------------------------------------------------------
UInt8* TFormatResource::GetData( void )
{
    return FData;
}
//------------------------------------------------------------------------------
void TFormatResource::SaveToFile( const UFC::AnsiString& FileName )
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
		///< For new version Futures/Option.
		case IDR_OPT_R010:	 Name =	"OPT_R010.ini"; break;
		case IDR_OPT_R020:	 Name =	"OPT_R020.ini"; break;
		case IDR_OPT_C030:	 Name =	"OPT_C030.ini"; break;
		case IDR_OPT_C030M:	 Name =	"OPT_C030M.ini"; break;
		case IDR_OPT_C030CR: Name =	"OPT_C030CR.ini"; break;
		case IDR_OPT_R070:	 Name =	"OPT_R070.ini"; break;
		case IDR_OPT_R080:	 Name =	"OPT_R080.ini"; break;
		case IDR_OPT_R090:	 Name =	"OPT_R090.ini"; break;
		case IDR_OPT_R100:	 Name =	"OPT_R100.ini"; break;
		case IDR_OPT_C030QCR:Name =	"OPT_C030QCR.ini"; break;
		///< For TSE Normal,Odd,Post trading session
		case IDR_TSE_T010:	Name =	"TSE_T010.ini"; break;
		case IDR_TSE_O010:	Name =	"TSE_O010.ini"; break;
		case IDR_TSE_O010EX:Name =	"TSE_O010Ex.ini"; break;
		case IDR_TSE_P010:	Name =	"TSE_P010.ini"; break;
		case IDR_TSE_P010EX:Name =	"TSE_P010Ex.ini"; break;
		case IDR_TSE_T020:	Name =	"TSE_T020.ini"; break;
		case IDR_TSE_T020EX:Name =	"TSE_T020Ex.ini"; break;
		case IDR_TSE_O020:	Name =	"TSE_O020.ini"; break;
		case IDR_TSE_O020EX:Name =	"TSE_O020Ex.ini"; break;
		case IDR_TSE_P020:	Name =	"TSE_P020.ini"; break;
		case IDR_TSE_P020EX:Name =	"TSE_P020Ex.ini"; break;
		case IDR_TSE_R030:	Name =	"TSE_R030.ini"; break;
		case IDR_TSE_R030EX:Name =	"TSE_R030Ex.ini"; break;
		///< For TSE Auction,Lend,Tender,TenderEx trade session.
		case IDR_TSE_A010:	Name =	"TSE_A010.ini"; break;
		case IDR_TSE_A010EX:Name =	"TSE_A010Ex.ini"; break;
		case IDR_TSE_A020:	Name =	"TSE_A020.ini"; break;
		case IDR_TSE_A020EX:Name =	"TSE_A020Ex.ini"; break;
		case IDR_TSE_V010:	Name =	"TSE_V010.ini"; break;
		case IDR_TSE_V010EX:Name =	"TSE_V010Ex.ini"; break;
		case IDR_TSE_V020:	Name =	"TSE_V020.ini"; break;
		case IDR_TSE_V020EX:Name =	"TSE_V020Ex.ini"; break;
		case IDR_TSE_E010:	Name =	"TSE_E010.ini"; break;
		case IDR_TSE_E010EX:Name =	"TSE_E010Ex.ini"; break;
		case IDR_TSE_E020:	Name =	"TSE_E020.ini"; break;
		case IDR_TSE_E020EX:Name =	"TSE_E020Ex.ini"; break;
		case IDR_TSE_Ex010:	 Name =	"TSE_Ex010.ini"; break;
		case IDR_TSE_Ex010EX:Name =	"TSE_Ex010Ex.ini"; break;
		case IDR_TSE_Ex020:	 Name =	"TSE_Ex020.ini"; break;
		case IDR_TSE_Ex020EX:Name =	"TSE_Ex020Ex.ini"; break;
		///< For OTC
		case IDR_OTC_T010:	Name =	"OTC_T010.ini"; break;
		case IDR_OTC_O010:	Name =	"OTC_O010.ini"; break;
		case IDR_OTC_O010EX:Name =	"OTC_O010Ex.ini"; break;
		case IDR_OTC_P010:	Name =	"OTC_P010.ini"; break;
		case IDR_OTC_P010EX:Name =	"OTC_P010Ex.ini"; break;
		case IDR_OTC_T020:	Name =	"OTC_T020.ini"; break;
		case IDR_OTC_T020EX:Name =	"OTC_T020Ex.ini"; break;
		case IDR_OTC_O020:	Name =	"OTC_O020.ini"; break;
		case IDR_OTC_O020EX:Name =	"OTC_O020Ex.ini"; break;
		case IDR_OTC_P020:	Name =	"OTC_P020.ini"; break;
		case IDR_OTC_P020EX:Name =	"OTC_P020Ex.ini"; break;
		case IDR_OTC_R030:	Name =	"OTC_R030.ini"; break;
		case IDR_OTC_R030EX:Name =	"OTC_R030Ex.ini"; break;
		case IDR_OTC_V010:	Name =	"OTC_V010.ini"; break;
		case IDR_OTC_V010EX:Name =	"OTC_V010Ex.ini"; break;
		case IDR_OTC_V020:	Name =	"OTC_V020.ini"; break;
		case IDR_OTC_V020EX:Name =	"OTC_V020Ex.ini"; break;
		case IDR_OTC_E010:	Name =	"OTC_E010.ini"; break;
		case IDR_OTC_E010EX:Name =	"OTC_E010Ex.ini"; break;
		case IDR_OTC_E020:	Name =	"OTC_E020.ini"; break;
		case IDR_OTC_E020EX:Name =	"OTC_E020Ex.ini"; break;
		case IDR_OTC_Ex010:	 Name =	"OTC_Ex010.ini"; break;
		case IDR_OTC_Ex010EX:Name =	"OTC_Ex010Ex.ini"; break;
		case IDR_OTC_Ex020:  Name =	"OTC_Ex020.ini"; break;
		case IDR_OTC_Ex020EX:Name =	"OTC_Ex020Ex.ini"; break;
		///< Error message table
		case IDR_ERROR_ENG: Name = "TAIFEXErrorEN.ini"; break;
		case IDR_ERROR_CHT: Name = "TAIFEXErrorCH.ini"; break;
		///< Produnts table for Futures/Option.
		case IDR_FUTURES_PROD: Name = "FUTSymbol.ini"; break;
		case IDR_OPTIONS_PROD: Name = "OPTSymbol.ini"; break;
		default: return false;
	}
	return true;
}
//------------------------------------------------------------------------------
