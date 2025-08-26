//---------------------------------------------------------------------------

#pragma hdrstop

//#include "System.hpp"

//#include "IFSCATLAPIImpl.h"
//#include "ITWCAAPIImpl.h"

#include "ApiCADllObject.h"

//----------------------------- CApiCADllObject -----------------------------
//---------------------------------------------------------------------------
CApiCADllObject::CApiCADllObject(ApiCAIDEnum CAID, const UFC::AnsiString& CommonName, UFC::BufferedLog* LogPtr)
:UFC::CMainKeyObject(UFC::cstCreateLock)
,FCAID(CAID)
,FCommonName(CommonName)
,FDLLFileName("")
,FLogPtr(LogPtr)
,FSignerCertFilter("")
#ifdef WIN32
,FDllHandle(0)
#endif
//,FCreateFSCAPIInstance(0)
//,FFSCObjPtr(0)
//,FCreateTWCAAPIInstance(0)
//,FTWCAObjPtr(0)
,FIsWorking(false)
{
}  //CApiCADllObject::CApiCADllObject()
//---------------------------------------------------------------------------
CApiCADllObject::CApiCADllObject(ApiCAIDEnum CAID, const UFC::AnsiString& CommonName, const UFC::AnsiString& DllFileName, UFC::BufferedLog* LogPtr)
:UFC::CMainKeyObject(UFC::cstCreateLock)
,FCAID(CAID)
,FCommonName(CommonName)
,FDLLFileName(DllFileName)
,FLogPtr(LogPtr)
,FSignerCertFilter("")
#ifdef WIN32
,FDllHandle(0)
#endif
//,FCreateFSCAPIInstance(0)
//,FFSCObjPtr(0)
//,FCreateTWCAAPIInstance(0)
//,FTWCAObjPtr(0)
,FIsWorking(false)
{
	if (LoadCADynamicLibrary())
		FIsWorking = true;
	else
		FIsWorking = false;
}  //CApiCADllObject::CApiCADllObject()
//---------------------------------------------------------------------------
CApiCADllObject::~CApiCADllObject()
{
	if (FDllHandle != NULL) ::FreeLibrary(FDllHandle);
}  //CApiCADllObject::~CApiCADllObject()
//---------------------------------------------------------------------------
bool CApiCADllObject::LoadCADynamicLibrary()
{
	bool isSuccess = false;
#ifdef WIN32
//	String dllFileNameW = String(FDLLFileName.c_str());

//	FDllHandle = ::LoadLibrary(dllFileNameW.c_str());
	FDllHandle = ::LoadLibraryA(FDLLFileName.c_str());

	if (FDllHandle != NULL)
	{
		FLogPtr->fprintf(" %s() Load Dll[%s] success !", __func__, FDLLFileName.c_str());
		isSuccess = true;
	}
	else
		FLogPtr->fprintf(" %s() Load Dll[%s] falied.", __func__, FDLLFileName.c_str());
#endif  //#ifdef WIN32

	return isSuccess;
}  //CApiCADllObject::LoadCADynamicLibrary()
//---------------------------------------------------------------------------
bool CApiCADllObject::CreateActiveXCAObject()
{
	return false;
}  //CApiCADllObject::CreateCAObject()
/*
//---------------------------------------------------------------------------
bool CApiCADllObject::CreateTWCAObject()
{
	bool isSuccess = false;
   if (ITWCAAPIImpl::CreateInstance(&FTWCAObjPtr))
	{
		FLogPtr->fprintf("%s() Create TWCA Object Success.", __func__);
		isSuccess = true;
	}
	else
		FLogPtr->fprintf("%s() Create TWCA Object Failed.", __func__);
	return isSuccess;
}  //CApiCADllObject::CreateTWCAObject()
*/
#pragma package(smart_init)
