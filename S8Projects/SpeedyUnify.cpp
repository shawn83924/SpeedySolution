//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>
#include <Gdiplus.h>
#include "UnifyDlg.h"
//---------------------------------------------------------------------------
#pragma link "Gdiplus.lib"
#pragma link "sapi.lib"
#pragma link "dsound.lib"
#pragma link "roome_client_bcb.lib"
//---------------------------------------------------------------------------
USEFORM("OrderConfirm.cpp", OrdConfirmForm);
USEFORM("Roomi.cpp", RoomiForm);
USEFORM("SearchUnit.cpp", SearchSymbolForm);
USEFORM("SelAccount.cpp", DefAccForm);
USEFORM("Login.cpp", LoginForm);
USEFORM("main.cpp", MainForm);
USEFORM("TrainingDlg.cpp", TrainingDlgForm);
USEFORM("UnifyDlg.cpp", UnifyDlgs);
USEFORM("WebBrowserForm.cpp", BrowserForm);
USEFORM("TBarForm.cpp", OptionsTBarForm);
USEFORM("SetFontForm.cpp", FontForm);
USEFORM("StopSetting.cpp", StopSettingForm);
USEFORM("CASetting.cpp", CASettingForm);
USEFORM("ChartForm.cpp", LineChartForm);
USEFORM("ContractForm.cpp", ContractInfoForm);
USEFORM("ContractList.cpp", ContractListForm);
USEFORM("ContractViewer.cpp", ContractViewerForm);
USEFORM("CancelWorkingTimeForm.cpp", CancelWorkingForm);
USEFORM("AggrementUnit.cpp", AggrementForm);
USEFORM("AskCloseAll.cpp", AskCloseAllForm);
USEFORM("AskForUpdateForm.cpp", AskUpdateForm);
USEFORM("AskRoomi.cpp", AskRoomiForm);
USEFORM("HoldOpenInterest.cpp", HoldOpenInterestForm);
USEFORM("LeaderBoard.cpp", LeaderBoardForm);
USEFORM("LifeUpdateForm.cpp", UpdateForm);
USEFORM("ExecutionViewerForm.cpp", ExecutionForm);
USEFORM("DepthTrade.cpp", DepthForm);
//---------------------------------------------------------------------------
String GCommand;
bool   GGDIPlus       = false;
bool   GIsTestingMode = true;
bool   GSimMatch      = false;
HINSTANCE GHinstance;
//---------------------------------------------------------------------------
#if !defined(_WIN64)
	#pragma link "c:\\MBus\\Lib\\Win32\\UFC_XE7.lib"
	#pragma link "c:\\MBus\\Lib\\Win32\\StarWaveAPILib_XE7.lib"
	#pragma link "c:\\MBus\\Lib\\Win32\\SpeedyAPILib_XE7.lib"
#else
	#pragma link "c:\\MBus\\Lib\\Win64\\UFC_XE7.a"
	#pragma link "c:\\MBus\\Lib\\Win64\\StarWaveAPILib_XE7.a"
	#pragma link "c:\\MBus\\Lib\\Win64\\SpeedyAPILib_XE7.a"
#endif
//------------------------------------------------------------------------------
BOOL TerminateProcessEx(DWORD dwProcessId, UINT uExitCode)
{
	DWORD dwDesiredAccess = PROCESS_TERMINATE;
	BOOL  bInheritHandle  = FALSE;
	HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
	if (hProcess == NULL)
		return FALSE;

	BOOL result = TerminateProcess(hProcess, uExitCode);

	CloseHandle(hProcess);

	return result;
}

//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR CMD, int)
{
	GHinstance = hInstance;
	try
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		HANDLE    hMutex;
		AnsiString MutexName,WindowCaption;

		GCommand = String( CMD );
		if( GCommand.Pos(L"-t") != 0 || GCommand.Pos(L"-T") != 0 || GCommand.Pos(L"-test") != 0 )
			GIsTestingMode = true;
		else
			GIsTestingMode = false;
		if( GCommand.Pos(L"-s") != 0 || GCommand.Pos(L"-S") != 0 || GCommand.Pos(L"-sim") != 0 )
		{
			GSimMatch = true;
			MutexName = "SpeedyUnify_Sim";
			WindowCaption = "Speedy Unify Sim";
		}
		else
		{
			GSimMatch = false;
			MutexName = "SpeedyUnify";
			WindowCaption = "Speedy Unify";
		}
		if((hMutex = OpenMutexA( MUTEX_ALL_ACCESS, FALSE, MutexName.c_str() ))!=NULL)
		{
			HWND  SUHwnd;
			DWORD SUProcessId;

			SUHwnd = FindWindowA( "TMainForm", WindowCaption.c_str() );
			if( MessageDlg( L"Speedy Unify已啟動,確定要關閉之前的程式?", mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0 ) == mrOk )
			{
				GetWindowThreadProcessId( SUHwnd, & SUProcessId );
				TerminateProcessEx( SUProcessId, 0 );
			}
			else //If client already running, bring it's Window to top.
			{
				PostMessage( SUHwnd, WM_USER + 100 ,NULL, NULL);
				CloseHandle( hMutex );
				return 0; /// Next aleady Running.
			}
		}
		///< Create a mutex.
		CreateMutexA( NULL, TRUE, MutexName.c_str() );

		if( Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) == Gdiplus::Ok )
			GGDIPlus = true;
		::CoInitializeEx( NULL, COINIT_MULTITHREADED );
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		TStyleManager::TrySetStyle("Windows10");
		Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->CreateForm(__classid(TLoginForm), &LoginForm);
		Application->CreateForm(__classid(TContractInfoForm), &ContractInfoForm);
		Application->CreateForm(__classid(TContractViewerForm), &ContractViewerForm);
		Application->CreateForm(__classid(TUpdateForm), &UpdateForm);
		Application->CreateForm(__classid(TSearchSymbolForm), &SearchSymbolForm);
		Application->CreateForm(__classid(TLeaderBoardForm), &LeaderBoardForm);
		Application->CreateForm(__classid(TDefAccForm), &DefAccForm);
		Application->CreateForm(__classid(TAskRoomiForm), &AskRoomiForm);
		Application->CreateForm(__classid(TDefAccForm), &DefAccForm);
		Application->CreateForm(__classid(TAskRoomiForm), &AskRoomiForm);
		Application->CreateForm(__classid(TAskCloseAllForm), &AskCloseAllForm);
		Application->CreateForm(__classid(TCancelWorkingForm), &CancelWorkingForm);
		Application->CreateForm(__classid(TOrdConfirmForm), &OrdConfirmForm);
		Application->CreateForm(__classid(TFontForm), &FontForm);
		Application->Run();
		if( GGDIPlus == true )
			Gdiplus::GdiplusShutdown(gdiplusToken);
		::CoUninitialize();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
