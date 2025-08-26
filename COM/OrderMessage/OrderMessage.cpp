//$$---- axlib proj source ---- (stAXLibProjectSource)
#pragma hdrstop
#include <vcl.h>
#include <atl\atlvcl.h>
#include <float.h>

#include "BaseMessageImpl.h"
#include "MDBaseMessageImpl.h"

///< Order Messages
#include "NewOrderMessageImpl.h"
#include "ReplaceOrderMessageImpl.h"
#include "CancelOrderMessageImpl.h"
#include "OrderStatusRequestMessageImpl.h"

///< Quote Messages
#include "QuoteMessageImpl.h"
#include "QuoteCancelMessageImpl.h"
#include "QuoteRequestMessageImpl.h"

///< Execution report message
#include "ExecutionReportMessageImpl.h"

///< News Message
#include "NewsMessageImpl.h"

#pragma package(smart_init)

#pragma link "UFC.lib"
#pragma link "SpeedyAPILib.lib"
#pragma resource "*.tlb"

TComModule  Project1Module;
TComModule &_Module = Project1Module;

// The ATL Object map holds an array of _ATL_OBJMAP_ENTRY structures that
// described the objects of your OLE server. The MAP is handed to your
// project's CComModule-derived _Module object via the Init method.
//
BEGIN_OBJECT_MAP(ObjectMap)
  OBJECT_ENTRY(CLSID_BaseMessage, TBaseMessageImpl)
  OBJECT_ENTRY(CLSID_NewOrderMessage, TNewOrderMessageImpl)
  OBJECT_ENTRY(CLSID_ReplaceOrderMessage, TReplaceOrderMessageImpl)
  OBJECT_ENTRY(CLSID_CancelOrderMessage, TCancelOrderMessageImpl)
  OBJECT_ENTRY(CLSID_QuoteMessage, TQuoteMessageImpl)
  OBJECT_ENTRY(CLSID_QuoteCancelMessage, TQuoteCancelMessageImpl)
  OBJECT_ENTRY(CLSID_QuoteRequestMessage, TQuoteRequestMessageImpl)
  OBJECT_ENTRY(CLSID_OrderStatusRequestMessage, TOrderStatusRequestMessageImpl)
  OBJECT_ENTRY(CLSID_ExecutionReportMessage, TExecutionReportMessageImpl)
  OBJECT_ENTRY(CLSID_MDBaseMessage, TMDBaseMessageImpl)
  OBJECT_ENTRY(CLSID_NewsMessage, TNewsMessageImpl)
END_OBJECT_MAP()

// Entry point of your Server invoked by Windows for processes or threads are
// initialized or terminated.
//
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)
{
    if (reason == DLL_PROCESS_ATTACH)
	{
		_control87(MCW_EM, MCW_EM);
		_Module.Init(ObjectMap, hinst);
        DisableThreadLibraryCalls(hinst);
    }
    return TRUE;
}

// _Module.Term is typically invoked from the DLL_PROCESS_DETACH of your
// DllEntryPoint. However, this may result in an incorrect shutdown sequence.
// Instead an Exit routine is setup to invoke the cleanup routine
// CComModule::Term.
//
void ModuleTerm(void)
{
    _Module.Term();
}
#pragma exit ModuleTerm 63

// Entry point of your Server invoked to inquire whether the DLL is no
// longer in use and should be unloaded.
//
STDAPI __export DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

// Entry point of your Server allowing OLE to retrieve a class object from
// your Server
//
STDAPI __export DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

// Entry point of your Server invoked to instruct the server to create
// registry entries for all classes supported by the module
//
STDAPI __export DllRegisterServer(void)
{                                                            
    return _Module.RegisterServer(TRUE);
}

// Entry point of your Server invoked to instruct the server to remove
// all registry entries created through DllRegisterServer.
//
STDAPI __export DllUnregisterServer(void)
{
	return _Module.UnregisterServer();
}
//---------------------------------------------------------------------------
 