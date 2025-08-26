//---------------------------------------------------------------------------

#include <basepch.h>
#pragma hdrstop
#pragma package(smart_init)
#pragma link "\\MBus\\Lib\\UFC.lib"
#pragma link "\\MBus\\Lib\\Migo.lib"
//---------------------------------------------------------------------------

//   Package source.
//---------------------------------------------------------------------------
HINSTANCE DLLInstance;
#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)
{
    DLLInstance = hinst;
    return 1;
}
//---------------------------------------------------------------------------
 