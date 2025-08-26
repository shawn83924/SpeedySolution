//---------------------------------------------------------------------------
#include <basepch.h>

#pragma link "c:\\MBus\\Lib\\UFC.lib"

#pragma hdrstop
#pragma package(smart_init)

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
 