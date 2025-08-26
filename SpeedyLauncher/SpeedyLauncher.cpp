
#include <fstream>
#include <stdio.h>
#include "../UFC/UFC.h"
#include "../Migo/Sigo.h"
#include "psapi.h"
//--------------------------------------------------------------------------------------------------------------------
typedef DWORD (CALLBACK *PtrGetProcessId)(HANDLE);
//--------------------------------------------------------------------------------------------------------------------
BOOL IsXP2003()
{
	OSVERSIONINFO osvi;

	ZeroMemory( &osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &osvi );
	if( osvi.dwMajorVersion == 5 && (osvi.dwMinorVersion ==1 || osvi.dwMinorVersion ==2))
		return TRUE;
	return FALSE; 
}
//--------------------------------------------------------------------------------------------------------------------
DWORD GetProcessIDXP2003( HANDLE hProcess )
{
	PtrGetProcessId FuncGetProcessId;
	DWORD           Rtn = 0;
	HMODULE         Lib = LoadLibrary( "kernel32.dll" );

	if( ( Lib = LoadLibrary( "kernel32.dll" ) )!= NULL )
	{
		if( (FuncGetProcessId = (PtrGetProcessId)GetProcAddress( Lib,"GetProcessId"))!=NULL )
			Rtn = FuncGetProcessId(  hProcess );
		FreeLibrary( Lib );
	}
	return Rtn;
}
//--------------------------------------------------------------------------------------------------------------------
BOOL GetProcessFileName( HANDLE hProcess, char* ProcessFileName, int Size)
{
   // Get the process file name.
   if( GetModuleBaseName( hProcess, NULL, ProcessFileName, Size ) == 0 )
	   return FALSE;
   return TRUE;
}
//--------------------------------------------------------------------------------------------------------------------
DWORD GetProcessIDfromHandle( HANDLE hFindProcess )
{
   // Get the list of process identifiers.
   if( IsXP2003() == TRUE )
	   return GetProcessIDXP2003( hFindProcess );	

   DWORD ProcessIDs[ 1024 ], cbNeeded;
   char  FindProcessFileName[ MAX_PATH ] = "unknown";
   char  ProcessFileName[ MAX_PATH ];

   ///< get the process file name we want to find.
   if( GetProcessFileName( hFindProcess, FindProcessFileName, MAX_PATH ) == TRUE )
   {
	   ///< Enum all process.
	   if( EnumProcesses( ProcessIDs, sizeof(ProcessIDs), &cbNeeded ) != 0 )
	   {
		   ///< Calculate how many process identifiers were returned.
		   int ProcessCount = cbNeeded / sizeof( DWORD );
		   ///< Search the processes.
		   for ( int i = 0; i < ProcessCount; i++ )
		   {
			   HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, ProcessIDs[i] );
			   if( hProcess )
			   {
				   GetProcessFileName( hProcess,ProcessFileName, MAX_PATH ); 	
     			   CloseHandle( hProcess );
				   if( strcmp( ProcessFileName,FindProcessFileName) == 0)
					   return ProcessIDs[ i ];
			   }
		   }
	   }
   }
   return 0;
}
//--------------------------------------------------------------------------------------------------------------------
void Start( const char* ProcessName,const char* BinaryName, const char* Param, BOOL CreatePIDFile )
{
    UFC::AnsiString         PIDFile,ShellCMD;
	char                    Dir[ 256 ];

    PIDFile.Printf( "%s.pid", ProcessName );
    if( UFC::FileExists( PIDFile ) == FALSE || CreatePIDFile == FALSE )
    {         
		 SHELLEXECUTEINFO ShellExecInfo;

		 GetCurrentDirectory( 256, Dir );
		 ShellCMD.Printf( "%s.exe", BinaryName );
		 ZeroMemory( &ShellExecInfo, sizeof( SHELLEXECUTEINFO ) );
		 ShellExecInfo.cbSize       = sizeof( SHELLEXECUTEINFO );
		 ShellExecInfo.fMask        = SEE_MASK_NOCLOSEPROCESS;
		 ShellExecInfo.hwnd         = NULL;
		 ShellExecInfo.lpVerb       = "open";
		 ShellExecInfo.lpFile       = ShellCMD.c_str();
		 ShellExecInfo.lpParameters = Param;
		 ShellExecInfo.lpDirectory  = Dir;
		 ShellExecInfo.nShow        = SW_HIDE;
		 if( ShellExecuteEx( &ShellExecInfo ) == TRUE )
		 {
			 printf("  Process %s is running.\n", ProcessName );			 
			 if( CreatePIDFile )
			 {
				 Sleep( 500 );
				 std::ofstream pidfile;
				 DWORD Processid = GetProcessIDfromHandle( ShellExecInfo.hProcess );
				 
				 pidfile.open( PIDFile.c_str() );
				 pidfile << Processid;
				 pidfile.close();
			 }
             CloseHandle( ShellExecInfo.hProcess );            
		 }
		 else
		 {
			LPVOID lpMsgBuf;
			DWORD dw = GetLastError(); 

			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						NULL,
						dw,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						(LPTSTR) &lpMsgBuf,
						0, NULL );
			printf("  Run process %s failed Message:%s\n", ProcessName,lpMsgBuf );
			LocalFree(lpMsgBuf);
		}
    }
    else
        printf("  Process %s already running\n", ProcessName );
}
//--------------------------------------------------------------------------------------------------------------------
void Stop( const char* ProcessName )
{
    UFC::AnsiString PIDFile,ShellCMD;
    std::ifstream   pidfile;
    DWORD           pid;

    PIDFile.Printf( "%s.pid", ProcessName );
    pidfile.open( PIDFile.c_str());
    if( pidfile.is_open())
    {
        pidfile >> pid;
        pidfile.close();
        UFC::DeleteFile( PIDFile );
		if( pid != 0 )
		{
			HANDLE ProcessHandle = OpenProcess( PROCESS_ALL_ACCESS, false, pid );
			TerminateProcess( ProcessHandle, 0 );
			printf("  Kill process %s ID:%d\n", ProcessName, pid );
			CloseHandle( ProcessHandle );
		}
		else	
			printf(" Invalidate Process ID for %s\n", ProcessName );
    }
    else
        printf("  Process %s not running\n", ProcessName );
}
//------------------------------------------------------------------------------
void CheckArgs( int argc, char * argv[] )
{
    printf( "________________________________________________\n\n" );
	printf( "  SpeedyLauncher Copyright (c) 2003,2005 MDBS\n");
    printf( "  Ver:1.2.21 Build Date:%s\n\n",__DATE__ );

    for( int i = 1; i < argc; i++ )
    {
        UFC::AnsiString Param( argv[i] );
        if( Param == "-H" || Param == "-h" || Param == "-?" )
        {
            
            printf( "  Usage: SpeedyLauncher [options]\n" );
            printf( "    -H         Show this page.\n" );
            printf( "    -R         Run Speedy system.\n" );
            printf( "    -RC        Run Speedy system and clear all existing data.\n" );
            printf( "    -T         Terminat Speedy system.\n" );
			return;
        }
        else if( Param == "-R" )
        {
			Start( "SpeedyAgent", "SpeedyAgent", "-StartAll", TRUE );            
			printf( "  Starting Speedy System....\n");
			printf( "________________________________________________\n" );
			return;
        }
        else if( Param == "-RC" )
        {
            Start( "SpeedyAgent", "SpeedyAgent", "-StartAll -Clear", TRUE);
			printf( "  Starting Speedy System clear....\n");
			printf( "________________________________________________\n" );
			return;
        }
        else if( Param == "-T" )
        {
			Start( "SpeedyAgent", "SpeedyAgent", "-StopAll", FALSE );          
			printf( "  Stop all process" );
			for( int i = 0; i < 10; i++ )
			{
				sleep( 1 );
				printf( "." );
			}
			printf( "\n" );
			Stop( "SpeedyAgent" );
			printf( "  Stoping Speedy System....\n");
			printf( "________________________________________________\n" );
			return;
        }
	}
    printf( "________________________________________________\n" );    
}
//--------------------------------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
	CheckArgs( argc, argv );
	exit( 0 );
}
//--------------------------------------------------------------------------------------------------------------------

