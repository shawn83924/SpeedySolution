//------------------------------------------------------------------------------------------------------------------------
#include "RiskManager.h"
//------------------------------------------------------------------------------------------------------------------------
// global Objects
//------------------------------------------------------------------------------------------------------------------------
MessageObject*		gMessageObj  = NULL;
//------------------------------------------------------------------------------------------------------------------------
UFC::BufferedLog*	gOrderLog    = NULL;
//------------------------------------------------------------------------------------------------------------------------
// non-global Objects
//------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString		LogFileName;
//------------------------------------------------------------------------------------------------------------------------
UFC::BufferedLog*	ScreenLog    = NULL;
BOOL			IsDeamon     = FALSE;
BOOL			RecoverMode  = TRUE;
//----------------------------------------------------------------------------------------------------------------------
void ProcessMonitor::OnProcessConnected( BOOL IsTheFirstOne )
{
    if( IsTheFirstOne == FALSE )
    {
        UFC::BufferedLog::Printf( " ***** Process RiskManager already exists *****"  );
        sleep( 2 );
        exit( 0 );
    }
}
//------------------------------------------------------------------------------------------------------------------------
void FE_AtExit( void )
{
    if( gOrderLog )
        gOrderLog->Flush();
    UFC::BufferedLog::FlushToFile();
}
//------------------------------------------------------------------------------------------------------------------------
void RunAsDeamon( void )
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
//------------------------------------------------------------------------------------------------------------------------
void CheckArgs( int argc, char *argv[] )
{
    for( int i = 1; i < argc; i++ ) 
    {
        UFC::AnsiString Param( argv[i] );
        if( Param == "-H" || Param == "-h" || Param == "-?" ) 
        {
            printf( "Usage: RiskManager [options]\n" );
            printf( "   -H  Show this page.\n" );
            printf( "   -D  Run as Deamon.\n" );
            printf( "   -F  First time startup.(Clear all queues and order records)\n" );
            exit( 1 );
        }
        else if( Param == "-D" || Param == "-d" ) 
        {
            IsDeamon = TRUE;
            RunAsDeamon();
        }
        else if( Param == "-F" || Param == "-f" )
            RecoverMode = FALSE;
    }
}
//------------------------------------------------------------------------------------------------------------------------
void PrintStartUp( void )
{
    UFC::BufferedLog::Printf( " ____________________________________________" );
    UFC::BufferedLog::Printf( "                                             " );
    UFC::BufferedLog::Printf( "    Yuanta RiskManager                       " );
    UFC::BufferedLog::Printf( "    For Futures / Options                    " );
    UFC::BufferedLog::Printf( "    Startup on %s at %s ", UFC::Hostname, UFC::GetDateString().c_str() );
    UFC::BufferedLog::Printf( "                                             ");
    UFC::BufferedLog::Printf( "    Ver : 2.0.0 Build Date:%s     ", __DATE__ );
    if( RecoverMode )
        UFC::BufferedLog::Printf( "    Recover Mode: ON" );
    else
        UFC::BufferedLog::Printf( "    Recover Mode: OFF" );
    UFC::BufferedLog::Printf( " ____________________________________________" );
}
//------------------------------------------------------------------------------------------------------------------------
void Initialize( void )
{
    // Specify function to be called when program terminates
    atexit( FE_AtExit );
}
//------------------------------------------------------------------------------------------------------------------------
int main( int argc, char * argv[] )
{
    AccountStore*       AccountStoreThread;  ///< AccountStore to stroe all accounts information.
   
    // Set Screen Log
    ScreenLog = new UFC::BufferedLog("../log/rm" + UFC::GetDateString() + ".log", LOG_BUFFER_SIZE, TRUE );
    UFC::BufferedLog::SetLogObject( ScreenLog );
    CheckArgs( argc, argv );
    PrintStartUp();
    Initialize();	
    try
    {
	// Create RiskManager order log object
	LogFileName      = "../log/rm" + UFC::GetDateString() + ".dlog";
	gOrderLog        = new UFC::BufferedLog( LogFileName, LOG_BUFFER_SIZE, TRUE );
	// Create AccountStore thread.
	AccountStoreThread = new AccountStore( "AccountStoreThread", RecoverMode );
	// Start MessageObject thread.
        gMessageObj = new MessageObject( "RiskManager", "1.0", "XXXXX" );
        gMessageObj->AddListener( "RiskManager.FUT", UFC::Hostname, new CheckManager( FALSE, AccountStoreThread ) );
        gMessageObj->AddListener( "RiskManager.OPT", UFC::Hostname, new CheckManager( TRUE, AccountStoreThread ) );
        gMessageObj->SetMonitorListener( new ProcessMonitor() );    
        gMessageObj->Start(); ///< Start Message pump.    
        gMessageObj->WaitForConnected();                
	while( TRUE )
	{
            sleep( 1 );
            gOrderLog->Flush();
            UFC::BufferedLog::FlushToFile();
	}
    }
    catch( UFC::Exception &e )
    {
    	UFC::BufferedLog::Printf(" RiskManager UFC Exception. <Reason:%s>\n", e.what() );
    }
    catch(...)
    {
	UFC::BufferedLog::Printf(" RiskManager Unknown Exception.\n" );
    }
    return 0;
}
//------------------------------------------------------------------------------------------------------------------------
