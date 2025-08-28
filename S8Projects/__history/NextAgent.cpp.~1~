
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include "AgentForm.h"
#include <Registry.hpp>
//------------------------------------------------------------------------------
#pragma link "C:\\MBus\\Lib\\UFC.lib"
//------------------------------------------------------------------------------
#define MUTEX_NAME     "SpeedyUnify"      /// The PowerBox main program mutex name.
//------------------------------------------------------------------------------
USEFORM("AgentForm.cpp", AgentMain );
//------------------------------------------------------------------------------
/*void ExecuteProgram( HINSTANCE hInstance, UFC::AnsiString& Command )
{
	SHELLEXECUTEINFO si;
	/// Fill SHELLEXECUTEINFO structure for ShellExecute Win API.
	ZeroMemory( &si, sizeof(si) );
	si.cbSize = sizeof(si);
	si.lpVerb = "open";
	si.lpFile = MAIN_EXE;
	si.lpParameters = Command.c_str();
	si.hInstApp = hInstance;
	si.nShow = SW_SHOWDEFAULT;
	si.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShellExecuteEx( &si );   /// Execute the PMFNext
} */
//------------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR CMD, int)
{
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TAgentMain), &AgentMain);
		Application->Run();
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
	//ExecuteProgram( hInstance, Command );
	return 0;
}
//------------------------------------------------------------------------------
