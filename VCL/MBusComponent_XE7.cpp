//---------------------------------------------------------------------------
#include <basepch.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("NewValueForm.cpp", AddValueForm);
//---------------------------------------------------------------------------
#pragma package(smart_init)
#if !defined(_WIN64)
	#pragma link "c:\\MBus\\Lib\\Win32\\UFC_XE7.lib"
#else
	#pragma link "c:\\MBus\\Lib\\Win64\\UFC_XE7.a"
#endif
//---------------------------------------------------------------------------

//   Package source.
//---------------------------------------------------------------------------
//HINSTANCE DLLInstance;
#pragma argsused
extern "C" int _libmain(unsigned long reason)
//int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)
{
//    DLLInstance = hinst;
    return 1;
}
//---------------------------------------------------------------------------
 