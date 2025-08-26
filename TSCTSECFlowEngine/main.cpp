//---------------------------------------------------------------------------
#include "SocketSend.h"
#include "MessageDispatcher.h"
//---------------------------------------------------------------------------
const UFC::AnsiString CFG_FILE = "../cfg/TSCTSECFlowEngine.cfg";
//---------------------------------------------------------------------------
void CheckArgs(int argc, char* argv[], bool &FTRunning, bool &FDebugMode, bool &FBenchMarkMode, bool &FTestingMode)
{
  for(int i = 0; i < argc ;i++)
  {
      UFC::AnsiString Param( argv[i] );
      if( Param == "-H" || Param == "-h" || Param == "-?" )
      {
          printf( "\nUsage: TSCTSECFlowEngine [argument]\n");
          printf( "   -H  Show this page.\n");
          printf( "   -D  Debug mode.\n" );
          printf( "   -F  First time startup.( Reset the Network ID, clear all Order data).\n" );
          printf( "   -B  BenchMark Mode.\n");

          exit( 1 );
      }
      else if( Param == "-D" || Param == "-d" )
          FDebugMode = true;
      else if(Param == "-F" || Param == "-f" )
          FTRunning = true;
      else if(Param == "-B" || Param == "-b" )
          FBenchMarkMode = true;
      else if(Param == "-T" || Param == "-t")
          FTestingMode = true;
  }
}
//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    bool        FirstTimeRunning = false;
    bool        DebugMode        = false;
    bool        BenchMarkMode    = false;
    bool        TestingMode      = false;

    CheckArgs(argc, argv, FirstTimeRunning, DebugMode, BenchMarkMode, TestingMode );
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   Flow control module, TSCFlowEngine   " );
    UFC::BufferedLog::Printf( "   startup at %s.            ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   FirstTimeRunning: [%s]", FirstTimeRunning ? "TRUE":"FALSE");
    UFC::BufferedLog::Printf( "   DebugMode: [%s]", DebugMode ? "TRUE":"FALSE");
    UFC::BufferedLog::Printf( "   BenchMark Mode: [%s]", BenchMarkMode ? "TRUE":"FALSE");
    UFC::BufferedLog::Printf( "   Testing Mode:[%s]", TestingMode ? "TRUE":"FALSE");
    UFC::BufferedLog::Printf( "   For TSC TSEC Version." );
    UFC::BufferedLog::Printf( "   Ver:1.2.21 Build Date:%s ",__DATE__ );
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::SetDebugMode( DebugMode );

    if( UFC::FileExists( CFG_FILE ) == TRUE )
    {
        MessageDispatcher Dispatcher( CFG_FILE, FirstTimeRunning, BenchMarkMode, TestingMode );
        Dispatcher.Run();
    }
    else
        UFC::BufferedLog::Printf( " Config file:%s not found.", CFG_FILE.c_str() );

    return 0;
}
//---------------------------------------------------------------------------














