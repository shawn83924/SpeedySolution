#ifdef _MSC_VER
	#include "config_windows.h"
#endif
#include "OrcTFXMapper.h"
#include "TSCTFXOrder.h"
#include "imFIXMapper.h"
//------------------------------------------------------------------------------
FIXMapperApp* Application;
//------------------------------------------------------------------------------
BOOL RecoverMode = TRUE;
BOOL DebugMode   = FALSE;
BOOL IsStandby   = FALSE;
BOOL BenchmarkMode = FALSE;
//------------------------------------------------------------------------------
//  **************************** Importent ****************************
//
//  You must give this mapper an unique name.( MAPPER_NAME)
//  MBus will use this name to identify this process, (for process monitoring)
//  and you should prepare a config file nameed MAPPER_NAME.cfg
//  in directory ..\cfg\.
//------------------------------------------------------------------------------
const char* MAPPER_NAME = "ORCTFXMapper";
//------------------------------------------------------------------------------
void CheckArgs( int argc, char *argv[] )
{
    for( int i = 1; i < argc; i++ )
    {
       UFC::AnsiString Param( argv[i] );
       if( Param == "-H" || Param == "-h" || Param == "-?" )
       {
           printf( "Usage: %s [options]\n", MAPPER_NAME );
           printf( "   -H  Show this page.\n" );
           printf( "   -D  Debug mode.\n" );
           printf( "   -S  Standby mode.\n" );
           printf( "   -F  First time startup.( Reset the Network ID, clear all Order data).\n" );
           exit( 1 );
       }
       else if( Param == "-B" || Param == "-b" )
            BenchmarkMode = TRUE;
       else if( Param == "-F" || Param == "-f" )
            RecoverMode = FALSE;
       else if( Param == "-D" || Param == "-d" )
            DebugMode = TRUE;
       else if( Param == "-S" || Param == "-s" )
            IsStandby = TRUE;
    }
}
//------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    CheckArgs( argc,argv );

    UFC::BufferedLog::Printf(" ____________________________________________");
    UFC::BufferedLog::Printf("                                             ");
	UFC::BufferedLog::Printf("    FIX Mapper:%s  ", MAPPER_NAME);
    UFC::BufferedLog::Printf("    Copyright 2004-2006 by MDBS Software Inc.");
    UFC::BufferedLog::Printf("    All right reserved.                      ");
    UFC::BufferedLog::Printf("                                             ");
    UFC::BufferedLog::Printf("    Ver:1.2.22 Build Date:%s",__DATE__ );
    if( RecoverMode == TRUE )
        UFC::BufferedLog::Printf("    Recover Mode:ON" );
    else
        UFC::BufferedLog::Printf("    Recover Mode:OFF" );
    if( IsStandby == TRUE )
        UFC::BufferedLog::Printf("    Standby Mode:ON" );
    else
        UFC::BufferedLog::Printf("    Standby Mode:OFF" );
    if( BenchmarkMode == TRUE )
        UFC::BufferedLog::Printf("    Benchmark Mode:ON" );
    else
        UFC::BufferedLog::Printf("    Benchmark Mode:OFF" );
    UFC::BufferedLog::Printf(" ____________________________________________");
    UFC::BufferedLog::SetDebugMode( DebugMode );

	TSCTFXOrderFactory    Factory;
	TSCTFXConfirmParser   Confirm;
	TSCTFXExecutionParser Execution;
    NIDRuleFor8Digi       Rule;
    OrcTFXMapper          MyMapper( MAPPER_NAME, &Factory,&Rule, RecoverMode );

    Application = new FIXMapperApp( MAPPER_NAME, &MyMapper,&Confirm,&Execution );
    Application->SetBenchmarkMode( BenchmarkMode );
    Application->Run();
    delete Application;
    return 1;
}
//------------------------------------------------------------------------------
