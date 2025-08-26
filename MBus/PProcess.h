//---------------------------------------------------------------------------
#ifndef PProcessH
#define PProcessH
#include <windows.h>
#include <stdio.h>
#include "psapi.h"
#include "../UFC/UFCType.h"
//---------------------------------------------------------------------------
class PProcess
{
private:
	DWORD     FCurrentProcessId;
	HANDLE    FCurrentProcessHandle;
	UFCType::Int8      FCurrentModuleName[ MAX_PATH ];
	DWORD     FAllProcesses[1024];
	DWORD     FProcessesCount;
	void      EnumAllProcesses( void );
public:
    PProcess( void );
	DWORD      GetCurrentProcessID( void ) { return FCurrentProcessId; }
	HANDLE     GetCurrentProcessHandle( void ) { return FCurrentProcessHandle; }
	UFCType::Int8*      GetCurrentModuleName( void ) { return FCurrentModuleName; }
	DWORD*     GetAllProcesses( void ) { return FAllProcesses; }
	DWORD      GetProcessesCount( void ) { return FProcessesCount; }
	void       GetModuleName( HANDLE ProcessHandle, UFCType::Int8* ModuleName );
	HANDLE     Open( DWORD processID );
	DWORD      Terminate( HANDLE hProcess );
	DWORD      Close( HANDLE hProcess );
};
//------------------------------------------------------------------------------
#endif
