#include "MBusServer.h"
#include "FileStream.h"
#include <TLicenseKey.h>
//------------------------------------------------------------------------------
#ifdef _WIN32
    #include "PProcess.h"
    #pragma link "UFC.lib"
    #pragma link "psapi.lib"
#endif
//------------------------------------------------------------------------------
BOOL  BENCHMARK_MODE   = FALSE;
BOOL  DAEMON_MODE      = FALSE;
BOOL  SILENCE_MODE     = FALSE;
BOOL  PERMANENT        = FALSE;
BOOL  X_LICENSE        = FALSE;
Int32 MBUS_PORT        = 12345;
Int32 MCAST_PORT       = 12345;
Int32 RECV_THREAD      = 32;
Int32 SEND_THREAD      = 5;
Int32 MCAST_TTL        = 5;
Int32 MAX_CONNECTIONS  = 32;
Int32 HEARTBEAT_INTER  = 40;///<Sec
Int32 THREAD_SWITCH_MS = 16000;///< 16Sec
Int32 DEBUG_LEVEL     = UFC::dlFatalError;
UFC::PStringList      ForceIPList;
UFC::PStringHashedSet AcceptIPs;
UFC::PStringHashedSet AcceptClientIPs;
UFC::PStringHashedSet MonitorSubjects;
MBusServer*           MServer = NULL;
//---------------------------------------------------------------------------
class ForceServers : public UFC::PThread
{
private:
    UFC::PStringList* FIPs;
    MBusServer*       FServer;
private:
    void Execute( void )
    {
        UFC::SleepMS( 3000 );
        while( TRUE )
        {
            for( int i = 0; i < FIPs->ItemCount() ;i++ )
                 FServer->AddNewAddressIntoNewIPList( FIPs->GetItem( i ) );
            UFC::SleepMS( 20000 );
        }
    }
public:
    ForceServers( UFC::PStringList* IPs, MBusServer* Server )
    :UFC::PThread( NULL, FALSE )
    ,FIPs(IPs)
    ,FServer(Server)
    {
        Start();
    }
};
//------------------------------------------------------------------------------
class TestListener : public UFC::SocketClientListener
{
private:
    UFC::PEvent  FDisconnectEvent;
public:
    TestListener(){}
    ~TestListener(){}
    void WaitForDisconnect() { FDisconnectEvent.WaitFor(); }
    void OnIdle( UFC::PClientSocket * Socket ){}
    void OnConnect( UFC::PClientSocket * Socket )
    {
        UFC::BufferedLog::Printf(" Test connection connected, send test exists ack to server.");
        MigoHeader Header( ACK_TEST_EXISTS, 0 ,0 );
        Header.SerializeToSocket( Socket );
    }
    void OnDisconnect( UFC::PClientSocket * Socket, BOOL NeedReconnect = FALSE )
    {
        UFC::BufferedLog::Printf(" Test connection disconnected.");
	FDisconnectEvent.SetEvent();
    }
    BOOL OnDataArrived( UFC::PClientSocket * Socket )
    {
        return FALSE; ///< Return FALSE to request disconnect.
    }
};
//------------------------------------------------------------------------------
#ifdef _WIN32
//------------------------------------------------------------------------------
BOOL WINAPI OnWin32Exit( DWORD dwCtrlType)
{
    switch( dwCtrlType )
    {
        case CTRL_C_EVENT:	 UFC::BufferedLog::Printf( " Ctrl-C event" );
                                 break;
        case CTRL_CLOSE_EVENT: 	 UFC::BufferedLog::Printf( " Ctrl-Close event" );
                                 break;
        case CTRL_BREAK_EVENT:   UFC::BufferedLog::Printf( " Ctrl-Break event" );
                                 break;
        case CTRL_LOGOFF_EVENT:  UFC::BufferedLog::Printf( " Ctrl-Logoff event" );
                                 break;
        case CTRL_SHUTDOWN_EVENT:UFC::BufferedLog::Printf( " Ctrl-Shutdown event" );
                                 break;
        default:		 UFC::BufferedLog::Printf( " Unknown event:%d", dwCtrlType );
                                 return FALSE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
void KillOldServer( void )
{
    #ifdef _WIN32
    PProcess Proc;
    DWORD*  AllProcesses = Proc.GetAllProcesses();
    DWORD   CurrentProcessId = Proc.GetCurrentProcessID();
    Int8*   CurrentModuleName = Proc.GetCurrentModuleName();
	DWORD   Count = Proc.GetProcessesCount();

    DWORD   ProcessID;
    HANDLE  hProcess;
    Int8    ModName[MAX_PATH];
    DWORD   ErrNo;
    for( DWORD i = 0; i < Count; ++i )
    {
        if( (ProcessID = AllProcesses[i]) != CurrentProcessId )
        {
            if( (hProcess = Proc.Open(ProcessID)) != NULL )
            {
                Proc.GetModuleName( hProcess, ModName );
                if( strcmp( ModName, CurrentModuleName ) == 0 )
                {
                    ErrNo = Proc.Terminate( hProcess );
                    UFC::BufferedLog::Printf( " Kill Process ID: %u, Module Name: %s, ErrNo: %u", ProcessID, ModName, ErrNo );
                }
                Proc.Close( hProcess );
            }
        }
    }
    #endif
}
//------------------------------------------------------------------------------
BOOL TraceServer()
{
    UFC::PClientSocket* Trace = NULL;
    try
    {
        TestListener Listener;

        Trace = new UFC::PClientSocket( "127.0.0.1", MBUS_PORT, TRUE );
        Trace->SetListener( &Listener );
        UFC::SleepMS( 100 );
        Trace->Connect( 1 );
        UFC::BufferedLog::Printf( " __________________________________________" );
        UFC::BufferedLog::Printf( "                                           " );
        UFC::BufferedLog::Printf( "       MBus server already exists          " );
        UFC::BufferedLog::Printf( "       Ver:%s Build Date:%s", VERSION_STR, __DATE__  );        
        UFC::BufferedLog::Printf( " __________________________________________" );
        delete Trace;
        return TRUE;
    }
    catch( ... )
    {
        UFC::BufferedLog::Printf( " Start a new server" );
        if( Trace != NULL )
            delete Trace;
        KillOldServer();
    	return FALSE;
    }
}
//------------------------------------------------------------------------------
void RunAsDeamon()
{
    #ifndef _WIN32
     pid_t pid;

     if( ( pid = fork( ) ) < 0 )
         exit( 0 );
     else if( pid != 0)
         exit( 0 );
     setsid( );
     umask( 0 );
     #endif
}
//------------------------------------------------------------------------------
void LicenseInfo( void )
{
    if( !(UFC::FileExists("License")) )
    {
	printf( "license file not found.\n" );
        return;
    }

    UFC::AnsiString Today, Plat, Date, Key;
    UFC::UiniFile iniFile( "License" );
    iniFile.GetValue( "MBus", "Date", Date );
    iniFile.GetValue( "MBus", "Key", Key );
#ifdef _WIN32
	Plat = "WIN32";
#endif
#ifdef __SOLARIS
	Plat = "SOLARIS";
#endif
#ifdef __AIX
	Plat = "AIX";
#endif
#ifdef __LINUX
	Plat = "LINUX";
#endif
    printf( "------------ License for MBus ------------\n" );
    printf( "Expired Date[%s]\n", Date.c_str() );
    printf( "Paltform[%s]\n", Plat.c_str() );
    printf( "Hostname[%s]\n", UFC::Hostname );
    printf( "Input Key[%s]\n",Key.c_str() );
    if( (Date.Length() > 0) && (Key.Length() > 0)  )
    {
	UFC::GetYYYYMMDD( Today );
	if( Date.ToInt() >= Today.ToInt() )
    	{
            UFC::TLicenseKey LicenseKey( Plat, "MBus", UFC::Hostname, Date );
            if( LicenseKey.CompareKey( Key ) )
            {
                printf( "Get Correct license key.\n" );
            }
            else
            {
                printf( "Should be[%s].\n", LicenseKey.ToString().c_str() );
                printf( "Bad license key.\n" );
            }
    	}
    	else
            printf( "License expired.\n" );
    }
    else
    	printf( "Incorrect license format.\n" );
    printf( "------------------ End -------------------\n" );
    return;
}
//------------------------------------------------------------------------------
void CheckArgs( int argc, char * argv[] )
{
    for( int i = 1; i < argc; i++ )
    {
        UFC::AnsiString Param( argv[i] );
        if( Param == "-H" || Param == "-h" || Param == "-?" )
        {
            printf( "MBus daemon Ver:%s Build Date:%s\n", VERSION_STR, __DATE__  );        
            printf( " %d bit mode.\n", (int)sizeof(void*)*8 );                    
            printf( "Copyright (c) 2003~%d by MDBS Software Inc.\n", UFC::ThisYear());            
            printf( "All right reserved.\n\n");            
            printf( "Usage: MBus [options]\n" );
            printf( "   -H         Show this page.\n" );
            printf( "   -B         Benchmark mode.\n" );
            printf( "   -D         Daemon mode( UNIX only ).\n" );
            printf( "   -F(IP)     Force connect to IP address.\n" );
            printf( "   -P(Port)   Service port.( default 12345 )\n" );
            printf( "   -permanent If present, MBus runs indefinitely until terminated.\n" );
            printf( "              If not present , MBus exits after 2 minutes\n" );
            printf( "              during which no connections are connected to it.\n" );
            printf( "   -S         Silence mode.( Disable UDP Multi-cast ).\n" );
            printf( "   -d(fipw)   Debug mode switch.\n" );
            printf( "              f: print fatal error debug information.\n" );
            printf( "              i: print debug information.\n" );
            printf( "              p: print performance debug information.\n" );
            printf( "              w: print warning debug information.\n" );
            exit( 1 );
        }
        else if( Param == "-B" || Param == "-b" )
            BENCHMARK_MODE = TRUE;
        else if( Param == "-D" )
            DAEMON_MODE    = TRUE;
        else if( Param.AnsiPos("-P") != -1 )
            MBUS_PORT = UFC::AnsiString( (const char*)(Param.c_str() + 2) ).ToInt();        
        else if( Param.AnsiPos("-permanent") != -1 )
            PERMANENT = TRUE;
        else if( Param == "-S" || Param == "-s")
            SILENCE_MODE = TRUE;
        else if( Param.AnsiPos("-F") != -1 )
        {
            UFC::AnsiString IP = UFC::AnsiString( Param.c_str() + 2 );
            if( IP.Length() > 7 )
                ForceIPList.Add( IP );
        }
        else if( Param.AnsiPos("-d") == 0 )
        {
             if( Param.AnsiPos("f")!=-1 )
                 DEBUG_LEVEL |= UFC::dlFatalError;
             if( Param.AnsiPos("i")!=-1 )
                 DEBUG_LEVEL |= UFC::dlInformation;
             if( Param.AnsiPos("p")!=-1 )
                 DEBUG_LEVEL |= UFC::dlPerformance;
             if( Param.AnsiPos("w")!=-1 )
                 DEBUG_LEVEL |= UFC::dlWarning;
        }
        else if( Param.AnsiPos("-MDBS") == 0 )
        {
             X_LICENSE = TRUE;
        }
        else if( Param == "-LI" )
        {
             LicenseInfo();
             exit(1);
        }
    }
}
//------------------------------------------------------------------------------
void LoadSetting( void )
{
    try
    {
        UFC::AnsiString Value,Name;
        UFC::UiniFile   Config( "MBus.ini" );

        if( Config.GetValue( "TCP Setting", "MaxConnections",Value ) == TRUE )
            MAX_CONNECTIONS = Value.ToInt();
        if( Config.GetValue( "TCP Setting", "SendThreadCount",Value ) == TRUE )
            SEND_THREAD = Value.ToInt();
        if( Config.GetValue( "TCP Setting", "ReceiveThreadCount",Value ) == TRUE )
            RECV_THREAD = Value.ToInt();
        if( Config.GetValue( "TCP Setting", "ThreadSwitchMS",Value ) == TRUE )            
            THREAD_SWITCH_MS = Value.ToInt();
        if( Config.GetValue( "TCP Setting", "HeartbeatInterval",Value ) == TRUE )
            HEARTBEAT_INTER = Value.ToInt();
        if( Config.GetValue( "Setting", "BenchmarkMode",Value ) == TRUE )
        {
            if( Value.ToInt() == 1 )
                BENCHMARK_MODE = TRUE;
            else
                BENCHMARK_MODE = FALSE;
        }
        if( Config.GetValue( "Setting", "DaemonMode",Value ) == TRUE )
        {
            if( Value.ToInt() == 1 )
                DAEMON_MODE = TRUE;
            else
                DAEMON_MODE = FALSE;
        }
        if( Config.GetValue( "Setting", "SilenceMode",Value ) == TRUE )
        {
            if( Value.ToInt() == 1 )
                SILENCE_MODE = TRUE;
            else
                SILENCE_MODE = FALSE;
        }
        if( Config.GetValue( "Setting", "Permanent",Value ) == TRUE )
        {
            if( Value.ToInt() == 1 )
                PERMANENT = TRUE;
            else
                PERMANENT = FALSE;
        }
        if( Config.GetValue( "Setting", "ServicePort",Value ) == TRUE )
        {
            MBUS_PORT = Value.ToInt();
        }
        if( Config.GetValue( "Setting", "MulticastTTL",Value ) == TRUE )
        {
            MCAST_TTL = Value.ToInt();
        }
        if( Config.GetValue( "Setting", "MulticastPort",Value ) == TRUE )
        {
            MCAST_PORT = Value.ToInt();
        }
        if( Config.GetValue( "Setting", "Debug", Value ) == TRUE )
        {
            if( Value.AnsiPos("f")!=-1 )
                DEBUG_LEVEL |= UFC::dlFatalError;
            if( Value.AnsiPos("i")!=-1 )
                DEBUG_LEVEL |= UFC::dlInformation;
            if( Value.AnsiPos("p")!=-1 )
                DEBUG_LEVEL |= UFC::dlPerformance;
            if( Value.AnsiPos("w")!=-1 )
                DEBUG_LEVEL |= UFC::dlWarning;
        }
        for( int i = 1;; i++ )
        {
            Name.Printf( "IP%d", i );
            if( Config.GetValue( "IPList", Name, Value ) == TRUE )
            {
                 AcceptIPs.Add( Value );
                 UFC::BufferedLog::Printf(" Can accept IP[%s] ", Value.c_str() );
            }
            else
                 break;
        }
        for( int i = 1;; i++ )
        {
            Name.Printf( "IP%d", i );
            if( Config.GetValue( "ForceIPList", Name, Value ) == TRUE )
            {
                ForceIPList.Add( Value );
                UFC::BufferedLog::Printf(" Force connect to IP[%s] ", Value.c_str() );
            }
            else
                 break;
        }
        for( int i = 1;; i++ )
        {
            Name.Printf( "IP%d", i );
            if( Config.GetValue( "ClientIPList", Name, Value ) == TRUE )
            {
                 AcceptClientIPs.Add( Value );
                UFC::BufferedLog::Printf(" Can accept client IP[%s] ", Value.c_str() );
            }
            else
                 break;
        }
        for( int i = 1;; i++ )
        {
            Name.Printf( "Subject%d", i );
            if( Config.GetValue( "Subjects", Name, Value ) == TRUE )
            {
                 MonitorSubjects.Add( Value );
                 UFC::BufferedLog::Printf(" Monitor Subject[%s] ", Value.c_str() );
            }
            else
                 break;
        }
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" MBus.ini not found, use default settings");
    }
}
//------------------------------------------------------------------------------
bool CheckLicense( const UFC::AnsiString& Prod, const UFC::AnsiString& Plat, const UFC::AnsiString& Date, const UFC::AnsiString& Key )
{
    if( (Date.Length() > 0) && (Key.Length() > 0)  )
    {
        UFC::AnsiString Today;

        UFC::GetYYYYMMDD( Today );
        if( Date.ToInt() >= Today.ToInt() ) ///< Check License expired.
        {
            UFC::TLicenseKey LicenseKey( Plat, Prod, UFC::Hostname, Date );
            
            if( LicenseKey.CompareKey( Key ) ) ///< Check MD5 hash code.
                return TRUE; ///< Get correct license!
            else
                printf( "Bad license key:%s.\n",  Key.c_str() );
        }
        else
            printf( "License expired.\n" );
    }
    else
          printf( "Incorrect license format.\n" );    
    return FALSE;
}
//------------------------------------------------------------------------------
void CheckLicenseKey( void )
{
    if( !(UFC::FileExists("License")) )
    {
	    printf( "license file not found.\n" );
	    exit(1);
    }
    	
    UFC::AnsiString Today, Plat, Date, Key;
    UFC::UiniFile iniFile( "License" );

#ifdef _WIN32
	Plat = "WIN32";
#endif
#ifdef __SOLARIS
	Plat = "SOLARIS";
#endif
#ifdef __AIX
	Plat = "AIX";
#endif
#ifdef __LINUX
	Plat = "LINUX";
#endif
    if( iniFile.GetValue( "MBus", "Date", Date ) == TRUE && 
        iniFile.GetValue( "MBus", "Key", Key ) == TRUE )    
    {
        if( CheckLicense( "MBus", Plat, Date,  Key ) == TRUE )
        {
            printf( "[Rights] Get License for Speedy.\n" );
            return ;
        }
    }
    else if( iniFile.GetValue( "MBusD", "Date", Date ) == TRUE && 
             iniFile.GetValue( "MBusD", "Key", Key ) == TRUE )    
    {
        if( CheckLicense( "MBusD", Plat, Date,  Key ) == TRUE )
        {
            printf( "[Rights] Get License for MBus Daemon .\n" );
            return ;
        }
    }
    printf( "Bad license file.\n" );
    exit( 1 );   
}

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void OnExit()
{   
    UFC::BufferedLog::Printf( " OnExit MBus." );	
    if (MServer!=NULL)
    {
        delete MServer;
        MServer = NULL;
    }
    UFC::BufferedLog::Printf( "   Exit MBus." );
    UFC::BufferedLog::FlushToFile();
}
#ifdef _WIN32
//------------------------------------------------------------------------------
BOOL WINAPI OnWin32Exit( DWORD dwCtrlType)
{
    switch( dwCtrlType )
    {
	case CTRL_C_EVENT:       UFC::BufferedLog::Printf( " Ctrl-C event" );
				 break;
	case CTRL_CLOSE_EVENT: 	 UFC::BufferedLog::Printf( " Ctrl-Close event" );
				 break;
	case CTRL_BREAK_EVENT:   UFC::BufferedLog::Printf( " Ctrl-Break event" );
				 break;
	case CTRL_LOGOFF_EVENT:  UFC::BufferedLog::Printf( " Ctrl-Logoff event" );
				 break;
	case CTRL_SHUTDOWN_EVENT:UFC::BufferedLog::Printf( " Ctrl-Shutdown event" );
				 break;
	default:		 UFC::BufferedLog::Printf( " Unknown event:%d", dwCtrlType );
                                 break;
    }
    OnExit();
    return FALSE;
}
//------------------------------------------------------------------------------
#endif

//------------------------------------------------------------------------------
int main( int argc, char * argv[] )
{
    LoadSetting();
    
    CheckArgs( argc, argv );     ///< Parse command line arguments.
    
    if( X_LICENSE == FALSE ) ///< Pypass check license?
        CheckLicenseKey();
    else
        printf( "[Rights] Use MDBS License.\n" );
        
    if( UFC::Endian.IsLittleEndian() ) ///< Check the CPU is little endian or big endian.
        UFC::BufferedLog::Printf(" Running on X86 CPU.....");
    else
        UFC::BufferedLog::Printf(" Running on RISC CPU.....");

    if( TraceServer() == FALSE ) ///< Local server not exists.
    {
        if( DAEMON_MODE == TRUE )
            RunAsDeamon();       ///< Run as a deamon
        if( BENCHMARK_MODE == TRUE  )
            DEBUG_LEVEL |= UFC::dlPerformance;
        UFC::BufferedLog::SetDebugMode( DEBUG_LEVEL );
        UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( "MBus Log",
                                                              "bin",
                                                              "MBusLog",
                                                              "txt",
                                                              10240, TRUE ) );                
        #ifdef _WIN32
            SetConsoleCtrlHandler( OnWin32Exit, TRUE );
        #else
            atexit( OnExit );            
        #endif
        try
        {
            MServer = new MBusServer( &AcceptIPs, &AcceptClientIPs );            
            if( ForceIPList.ItemCount() > 0 )
                new ForceServers( &ForceIPList, MServer );
            MServer->Start();
        }
        catch( UFC::Exception& e )
        {
            UFC::BufferedLog::Printf( " MBus daemon: %s", e.what() );
        }
        catch( ... )
        {
            UFC::BufferedLog::Printf( " MBus daemon: Unknown exception" );
        }                    
    }
    UFC::BufferedLog::Printf( " Close MBus message daemon." );

    OnExit();
    return 0;
}
//------------------------------------------------------------------------------

