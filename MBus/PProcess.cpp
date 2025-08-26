//---------------------------------------------------------------------------
#include "PProcess.h"
//---------------------------------------------------------------------------
PProcess::PProcess( void )
:FProcessesCount( 0 )
{
    FCurrentProcessId = GetCurrentProcessId();
    FCurrentProcessHandle = GetCurrentProcess();
    FCurrentModuleName[ MAX_PATH ];
    GetModuleName( FCurrentProcessHandle, FCurrentModuleName );
    EnumAllProcesses();
}
//------------------------------------------------------------------------------
void PProcess::EnumAllProcesses( void )
{
	UFCType::UInt32 EnumSize = sizeof( DWORD ) * 1024;
    memset( FAllProcesses, 0, EnumSize );
    FProcessesCount = 0;
    if( EnumProcesses( FAllProcesses, EnumSize, &FProcessesCount ) )
        FProcessesCount /= sizeof( DWORD );
}
//------------------------------------------------------------------------------
void PProcess::GetModuleName( HANDLE hProcess, char* pModuleName )
{
    HMODULE hMods[1024];
    DWORD cbNeeded;
    strcpy( pModuleName, "" );
    if( EnumProcessModules( hProcess, hMods, sizeof( hMods ), &cbNeeded ) )
    {
        UFCType::Int8 ModuleName[ MAX_PATH ];
        if( GetModuleFileNameEx(hProcess, hMods[0], ModuleName, sizeof(ModuleName)) )
            strcpy( pModuleName, ModuleName );
    }
}
//------------------------------------------------------------------------------
HANDLE PProcess::Open( DWORD processID )
{
    return OpenProcess( PROCESS_ALL_ACCESS, FALSE, processID );
}
//------------------------------------------------------------------------------
DWORD PProcess::Terminate( HANDLE hProcess )
{
    DWORD ErrNo = 0;
    if( TerminateProcess( hProcess, 0 ) == FALSE )
        ErrNo =  GetLastError();
    return ErrNo;
}
//------------------------------------------------------------------------------
DWORD PProcess::Close( HANDLE hProcess )
{
    DWORD ErrNo = 0;
    if( CloseHandle( hProcess ) == FALSE )
        ErrNo =  GetLastError();
    return ErrNo;
}
//------------------------------------------------------------------------------

