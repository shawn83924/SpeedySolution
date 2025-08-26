#include "SpeedyAgent.h"
#include "../UFC/List.h"
/*#include <sys/param.h>
#include <sys/user.h>
#include <sys/sysctl.h>
#include <stdlib.h>*/
//----------------------------------------------------------------------------------------------------------------------
extern UFC::AnsiString  CurrentDir;
extern void SendToSpeedyCenter( const UFC::AnsiString& Msg);
extern MessageObject*   MessageObj;
extern UFC::AnsiString  CurrentClientKey;
//--------------------------------------------------------------------------------------------------------------------
#ifdef _WIN32
#include "psapi.h"
//--------------------------------------------------------------------------------------------------------------------
typedef DWORD (CALLBACK *PtrGetProcessId)(HANDLE Process);
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
void Stop( const char* ProcessName, bool Force )
{
    UFC::AnsiString PIDFile,ShellCMD,PID;
    
    PIDFile.Printf( "%s.pid", ProcessName );
    if( UFC::FileExists( PIDFile ) == TRUE )
    {
        try
        {
            UFC::FileStreamEx File( PIDFile, "r+" );
        
            File.ReadLine(  PID );        
            UFC::BufferedLog::Printf(" *** Terminate process:%s PID:%d ***", ProcessName,PID.ToInt());
            HANDLE ProcessHandle = OpenProcess( PROCESS_ALL_ACCESS, false, PID.ToInt() );
            GenerateConsoleCtrlEvent( CTRL_BREAK_EVENT, PID.ToInt() ); ///< Send a Ctrl-Break signal to app
            if( WaitForSingleObject( ProcessHandle, 3000 ) == WAIT_TIMEOUT ) ///< Wait 5 sec for app exit.
                TerminateProcess( ProcessHandle, 0 ); ///< Not exit...force kill it!
            CloseHandle( ProcessHandle );
        }
        catch( UFC::Exception& e )    
        {
            UFC::BufferedLog::Printf(" Kill process %s failed:%s", ProcessName, e.what() );
        }        
        UFC::DeleteFile( PIDFile );       
    }
    else
        UFC::BufferedLog::Printf(" Process %s not running", ProcessName );   
}
//--------------------------------------------------------------------------------------------------------------------
void Start( const char* ProcessName,const char* BinaryName, const char* Param, BOOL Clean )
{
    UFC::AnsiString         PIDFile,ShellCMD,Parameter;
    char                    Dir[ 256 ];

    PIDFile.Printf( "%s.pid", ProcessName );
    if( Clean == TRUE && UFC::FileExists( PIDFile ) == TRUE ) ///< Force startup the process
        Stop( ProcessName, true );
    if( UFC::FileExists( PIDFile ) == FALSE )
    {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        if( Clean == TRUE )
            Parameter.Printf( "%s -F", Param );
        else
            Parameter = Param;
        GetCurrentDirectory( 256, Dir );
        ShellCMD.Printf( "%s.exe %s", BinaryName, Parameter.c_str() );
        ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof( si );
        ZeroMemory( &pi, sizeof(pi) );
        // Start the child process.
        if( CreateProcess(  NULL,   // No module name (use command line)
                            (LPSTR)ShellCMD.c_str(),      // Command line
                            NULL,           // Process handle not inheritable
                            NULL,           // Thread handle not inheritable
                            FALSE,          // Set handle inheritance to FALSE
                            CREATE_NEW_PROCESS_GROUP,
                            NULL,           // Use parent's environment block
                            NULL,           // Use parent's starting directory
                            &si,            // Pointer to STARTUPINFO structure
                            &pi ) != 0  ) // Pointer to PROCESS_INFORMATION structure
        {
            std::ofstream pidfile;
            Sleep( 500 );
            pidfile.open( PIDFile.c_str() );
            pidfile << pi.dwProcessId;
            pidfile.close();
            CloseHandle( pi.hProcess );
        }
        else
        {
            LPVOID lpMsgBuf;
            DWORD dw = GetLastError();

            FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                           NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                           (LPTSTR) &lpMsgBuf, 0, NULL );
			
			UFC::AnsiString ErrMsg;
			
            ErrMsg.Printf("  Run process %s failed Message:%s", ProcessName,lpMsgBuf );
			printf("%s\n",ErrMsg.c_str() );
            LocalFree(lpMsgBuf);
        }
    }
    else
        UFC::BufferedLog::Printf(" Process %s already running", ProcessName );
}
//--------------------------------------------------------------------------------------------------------------------
#else
//--------------------------------------------------------------------------------------------------------------------
KillThread::KillThread( const UFC::AnsiString& Path, const UFC::AnsiString& Name, int MS )
:UFC::PThread( NULL )
,FPath( Path )
,FName( Name )
,FMS( MS )
{
    Start();
}
//--------------------------------------------------------------------------------------------------------------------
void KillThread::Execute( void )
{
    UFC::SleepMS( FMS );
    UFC::AnsiString PIDFile,ShellCMD,PID;

    PIDFile.Printf("%s/%s.pid",FPath.c_str(), FName.c_str() );
    if( UFC::FileExists( PIDFile ) == TRUE )
    {
        try
        {
            UFC::FileStreamEx File( PIDFile, "r+" );

            File.ReadLine( PID );
            ShellCMD.Printf("kill -9 %s",PID.c_str());
            UFC::BufferedLog::Printf(" *** Tarminate process:%s PID:%d ***", FName.c_str(),PID.ToInt());
            system( ShellCMD.c_str() );
        }
        catch( UFC::Exception& e )
        {
            UFC::BufferedLog::Printf(" Kill process %s failed:%s", FName.c_str(), e.what() );
        }
        ShellCMD.Printf("rm %s", PIDFile.c_str());
        system( ShellCMD.c_str() );
    }
    else
        UFC::BufferedLog::Printf(" Process %s not running", FName.c_str() );
}
//--------------------------------------------------------------------------------------------------------------------
void Stop( const char* ProcessName , bool Force )
{
    if( MessageObj != NULL &&  Force  == false )
    {
        MTree KillData;

        KillData.append( "CMD", ADMIN_KILL_PROCESS ); ///< Kill command
        KillData.append( "APP", ProcessName );
        KillData.append( "HOST", UFC::Hostname );
        KillData.append( "KILLER", CurrentClientKey );
        MessageObj->Send( "MBUS", UFC::Hostname, KillData, FALSE );
        UFC::BufferedLog::Printf(" Send Kill process Message to %s", ProcessName );
        new KillThread( CurrentDir, ProcessName, 1000 );
    }
    else
        new KillThread( CurrentDir, ProcessName, 10 );
}
//--------------------------------------------------------------------------------------------------------------------
void Start( const char* ProcessName, const char* BinaryName, const char* Param, BOOL Clean )
{
    UFC::AnsiString PIDFile = "", ShellCMD = "", PIDFileExistStr = "";

    PIDFile.Printf("%s/%s.pid", CurrentDir.c_str(), ProcessName );
    bool isPIDFileExist = false;
    if( UFC::FileExists( PIDFile ) == TRUE )
    {
        isPIDFileExist = true;
        PIDFileExistStr.Printf( "PIDFile[%s] Exist", PIDFile.c_str() );
    }
    else
        PIDFileExistStr.Printf( "PIDFile[%s] not Exist", PIDFile.c_str() );
    
    if( Clean == TRUE )
        UFC::BufferedLog::Printf(" %s() Process[%s] Binary[%s] Params[%s] Clean, %s.", __func__, ProcessName, BinaryName, Param, PIDFileExistStr.c_str() );
    else
        UFC::BufferedLog::Printf(" %s() Process[%s] Binary[%s] Params[%s], %s.", __func__, ProcessName, BinaryName, Param, PIDFileExistStr.c_str() );
    
    if( ( Clean == TRUE ) && isPIDFileExist ) ///< Force startup the process
        Stop( ProcessName, true );
    
    if( !isPIDFileExist )
    {        
        if( Clean == TRUE )
        {
            ShellCMD.Printf( "./%s %s -F & echo $! > %s", BinaryName, Param, PIDFile.c_str() );
            UFC::BufferedLog::Printf( " %s() Launch process: %s and clear old data", __func__, ProcessName );
        }
        else
        {
            ShellCMD.Printf( "./%s %s & echo $! > %s", BinaryName, Param, PIDFile.c_str() );
            UFC::BufferedLog::Printf( " %s() Launch process: %s", __func__, ProcessName );
        }
        int systemResult = system( ShellCMD.c_str() );
        UFC::BufferedLog::Printf(" %s() system[%s] result:%d", __func__, ShellCMD.c_str(), systemResult );
        
        ShellCMD.Printf("chmod 660 %s", PIDFile.c_str());
        system( ShellCMD.c_str() ); 
		
        UFC::AnsiString Message;
        Message.Printf(" SpeedyAgent Launch process[%s] result[%d]", ProcessName, systemResult);
        //UFC::SendLineNotifyMessage(UFC::lmtOK, Message.c_str());
    }
    else
    {
        UFC::AnsiString Message;
        Message.Printf( " Process %s carshed, click \"restart\" to run it.", ProcessName );
        SendToSpeedyCenter( Message );
        UFC::BufferedLog::Printf(" %s() Process %s already running", __func__, ProcessName );
    }
}
//--------------------------------------------------------------------------------------------------------------------
BOOL  AllParamsExist( const UFC::AnsiString& cmdline, UFC::PStringList& Params )
{
    if( Params.ItemCount() == 0 )
        return false;
    for( register int i = 0; i < Params.ItemCount(); i++ )
    {
        if( cmdline.AnsiPos( Params[i] ) == -1 )    
            return false;
    }
    return true;
}
//--------------------------------------------------------------------------------------------------------------------
pid_t  GetProcessId( const UFC::AnsiString& ProcessName )
{
    DIR* Dir = opendir("/proc");
    struct dirent* Item;
    struct stat FileStat;        

    if (Dir != NULL)
    {
        UFC::AnsiString  FullPath,Line;
        UFC::PStringList Params; 
        
        Params.SetStrings( ProcessName, " \n" );        
        while ((Item = readdir(Dir)) != NULL)
        {
            FullPath.Printf("/proc/%s", Item->d_name);
            stat( FullPath.c_str(), &FileStat );                        
            if( S_ISDIR( FileStat.st_mode ) == TRUE ) ///< A directory
            {                
                Int32 PID = UFC::AnsiString( Item->d_name ).ToInt();
                
                if( PID > 1 )
                {
                    FullPath.Printf("/proc/%s/cmdline", Item->d_name);                    
                    UFC::FileStreamEx FS( FullPath, "r" );
                    
                    FS.ReadLine( Line );
                    if( AllParamsExist( Line, Params ) == TRUE )
                    {
                        closedir( Dir );
                        return (pid_t)PID;
                    }                          
                }                
            }
        }
        closedir( Dir );
    }
    else
        UFC::BufferedLog::Printf(" opendir( /proc ) failed" );
    return -1;
} 
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
