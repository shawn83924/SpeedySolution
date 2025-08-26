//---------------------------------------------------------------------------
#include <basepch.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
#pragma link "Gdiplus.lib"
#pragma link "D2D1.lib"
//#pragma link "C:\\src\\COM\\Lib\\HSufxAPI\\HSufxAPIFactory\\t2sdk.lib"
#if !defined(_WIN64)
	#pragma link "c:\\MBus\\Lib\\Win32\\UFC_XE7.lib"
	#pragma link "c:\\MBus\\Lib\\Win32\\StarWaveAPILib_XE7.lib"
	#pragma link "c:\\MBus\\Lib\\Win32\\SpeedyAPILib_XE7.lib"
#else
	#pragma link "c:\\MBus\\Lib\\Win64\\UFC_XE7.a"
	#pragma link "c:\\MBus\\Lib\\Win64\\StarWaveAPILib_XE7.a"
	#pragma link "c:\\MBus\\Lib\\Win64\\SpeedyAPILib_XE7.a"
#endif
//---------------------------------------------------------------------------
//   Package source.
//---------------------------------------------------------------------------
#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)
{
	return 1;
}
//---------------------------------------------------------------------------
