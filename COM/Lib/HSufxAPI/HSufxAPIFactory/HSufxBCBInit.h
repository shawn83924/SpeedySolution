//---------------------------------------------------------------------------
#ifndef HSufxBCBInitH
#define HSufxBCBInitH
//---------------------------------------------------------------------------
#include "t2sdk_interface.h"
//---------------------------------------------------------------------------
extern "C"
{
//---------------------------------------------------------------------------
// Define all export function pointors
//---------------------------------------------------------------------------
typedef int                       (__stdcall *lpGetVersionInfo)();
typedef CConfigInterface*         (__stdcall *lpNewConfig)();
typedef CConnectionInterface*     (__stdcall *lpNewConnection)(CConfigInterface *lpConfig);
typedef IF2Packer*                (__stdcall *lpNewPacker)(int iVersion);
typedef IF2UnPacker*              (__stdcall *lpNewUnPacker)(void * lpBuffer, unsigned int iLen);
typedef IF2UnPacker*              (__stdcall *lpNewUnPackerV1)(void * lpBuffer, unsigned int iLen);
typedef int                       (__stdcall *lpGetPackVersion)(const void *lpBuffer);
typedef char*                     (__stdcall *lpEncode)(char *EncodePass, const char* Password, int nReserve = 0 );
typedef int                       (__stdcall *lpEncodeEx)(const char *pIn, char *pOut);
typedef CFilterInterface*         (__stdcall *lpNewFilter)();
typedef CSubscribeParamInterface* (__stdcall *lpNewSubscribeParam)();
typedef IBizMessage*              (__stdcall *lpNewBizMessage)();
}
//---------------------------------------------------------------------------
//
// Class to initialize HSufx (T2SDK) API
//
//---------------------------------------------------------------------------
class HSufxApiInit
{
private:
	bool FSupport;  ///< Support CTP API ?
	char FNotSupportReason[ 128 ]; ///< Reason of load Femas API failed.
	int  FFuncRtn;  ///< Bind which function failed ?
private:
	int InitFunctions( void );  ///< Bind all functions from DLL
public:
	bool SupportUFXAPI( void ) { return FSupport;}
	const char* Reason( void ) { return FNotSupportReason; }
public:
	HSufxApiInit();
	~HSufxApiInit();
public:
	lpGetVersionInfo    pGetVersionInfo;
	lpNewConfig         pNewConfig;
	lpNewConnection     pNewConnection;
	lpNewPacker         pNewPacker;
	lpNewUnPacker       pNewUnPacker;
	lpNewUnPackerV1     pNewUnPackerV1;
	lpGetPackVersion    pGetPackVersion;
	lpEncode            pEncode;
	lpEncodeEx          pEncodeEx;
	lpNewFilter         pNewFilter;
	lpNewSubscribeParam pNewSubscribeParam;
	lpNewBizMessage     pNewBizMessage;
};
//---------------------------------------------------------------------------
// global Object
//---------------------------------------------------------------------------
extern HSufxApiInit gHSufxAPI;
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
