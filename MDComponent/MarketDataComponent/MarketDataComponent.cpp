//---------------------------------------------------------------------------
#include <basepch.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("SortListForm.cpp", SortForm);
USEFORM("StopOrderSetting.cpp", StopOrderSettingForm);
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
#pragma link "Gdiplus.lib"
#pragma link "c:\\MBus\\Lib\\UFC.lib"
#pragma link "c:\\MBus\\Lib\\StarWaveAPILib.lib"
#pragma link "c:\\MBus\\Lib\\SpeedyAPILib.lib"
//---------------------------------------------------------------------------
//   Package source.
//---------------------------------------------------------------------------
#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)
{
	return 1;
}
//---------------------------------------------------------------------------
