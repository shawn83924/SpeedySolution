//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "HSufxBCBInit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
HSufxApiInit gHSufxAPI;
HINSTANCE    gHSufxDLLInstance = NULL;
//---------------------------------------------------------------------------
HSufxApiInit::HSufxApiInit()
: FSupport( false )
{
	gHSufxDLLInstance = NULL;
	// Dynamic load dll by APIType
	gHSufxDLLInstance = ::LoadLibraryW( L"t2sdk.dll" );

	if ( gHSufxDLLInstance != NULL )
	{
		if ( (FFuncRtn  = InitFunctions()) != 0 )
		{
			sprintf( FNotSupportReason, "Dynamic bind function %d failed.", FFuncRtn  );
			return;
		}
		sprintf( FNotSupportReason, "Load t2sdk.dll OK!" );
		FSupport = true; ///< Load CTP dll OK!
	}
	else
		sprintf( FNotSupportReason, "Can't dynamic load t2sdk.dll" );
}
//---------------------------------------------------------------------------
HSufxApiInit::~HSufxApiInit()
{
}
//---------------------------------------------------------------------------
int HSufxApiInit::InitFunctions( void )
{
	if ( (pGetVersionInfo    = (lpGetVersionInfo   )::GetProcAddress(gHSufxDLLInstance, "GetVersionInfo")) == NULL )
		return 1;
	if ( (pNewConfig         = (lpNewConfig        )::GetProcAddress(gHSufxDLLInstance, "NewConfig")) == NULL )
		return 2;
	if ( (pNewConnection     = (lpNewConnection    )::GetProcAddress(gHSufxDLLInstance, "NewConnection")) == NULL )
		return 3;
	if ( (pNewPacker         = (lpNewPacker        )::GetProcAddress(gHSufxDLLInstance, "NewPacker")) == NULL )
		return 4;
	if ( (pNewUnPacker       = (lpNewUnPacker      )::GetProcAddress(gHSufxDLLInstance, "NewUnPacker")) == NULL )
		return 5;
	if ( (pNewUnPackerV1     = (lpNewUnPackerV1    )::GetProcAddress(gHSufxDLLInstance, "NewUnPackerV1")) == NULL )
		return 6;
	if ( (pGetPackVersion    = (lpGetPackVersion   )::GetProcAddress(gHSufxDLLInstance, "GetPackVersion")) == NULL )
		return 7;
	if ( (pEncode            = (lpEncode           )::GetProcAddress(gHSufxDLLInstance, "Encode")) == NULL )
		return 8;
	if ( (pEncodeEx          = (lpEncodeEx         )::GetProcAddress(gHSufxDLLInstance, "EncodeEx")) == NULL )
		return 9;
	if ( (pNewFilter         = (lpNewFilter        )::GetProcAddress(gHSufxDLLInstance, "NewFilter")) == NULL )
		return 10;
	if ( (pNewSubscribeParam = (lpNewSubscribeParam)::GetProcAddress(gHSufxDLLInstance, "NewSubscribeParam")) == NULL )
		return 11;
	if ( (pNewBizMessage     = (lpNewBizMessage    )::GetProcAddress(gHSufxDLLInstance, "NewBizMessage")) == NULL )
		return 12;
	return 0;
}
//---------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
