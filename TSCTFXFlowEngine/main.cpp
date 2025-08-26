//---------------------------------------------------------------------------
#include "SocketSend.h"
#include "MessageDispatcher.h"
//---------------------------------------------------------------------------
const UFC::AnsiString CFG_FILE = "../cfg/TSCTFXFlowEngine.cfg";
//---------------------------------------------------------------------------
void CheckArgs(int argc, char* argv[], bool &FTRunning, bool &FDebugMode )
{
  for(int i = 0; i < argc ;i++)
  {
      UFC::AnsiString Param( argv[i] );
      if( Param == "-H" || Param == "-h" || Param == "-?" )
      {
          printf( "\nUsage: TSCFlowEngine [argument]\n");
          printf( "   -H  Show this page.\n");
          printf( "   -D  Debug mode.\n" );
          printf( "   -F  First time startup.( Reset the Network ID, clear all order data).\n" );
          exit( 1 );
      }
      else if( Param == "-D" || Param == "-d" )
          FDebugMode = true;
      else if(Param == "-F" || Param == "-f")
          FTRunning = true;
  }
}
//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    bool          FirstTimeRunning = false;
    bool          DebugMode        = false;
    UFC::AnsiString         YYYYMMDD;
    UFC::GetYYYYMMDD( YYYYMMDD );
    
    UFC::BufferedLog*    FlowEngineLog = new UFC::BufferedLog( "../Userlog/FlowEngine" + YYYYMMDD, 512, true );
    UFC::BufferedLog::SetLogObject( FlowEngineLog );
    
    CheckArgs( argc, argv, FirstTimeRunning, DebugMode );
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   Flow control module, TSCTFXFlowEngine  " );
    UFC::BufferedLog::Printf( "   startup at %s.            ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   FirstTimeRunning: [%s]", FirstTimeRunning ? "TRUE":"FALSE");
    UFC::BufferedLog::Printf( "   DebugMode: [%s]", DebugMode ? "TRUE":"FALSE");
    UFC::BufferedLog::Printf( "   For TSC TAIFEX Version." );
    UFC::BufferedLog::Printf( "   Ver:2.0.0.0 Build Date:%s ",__DATE__ );
    UFC::BufferedLog::Printf( "   Copyright 2006-2007 by MDBS Software Inc.");
    UFC::BufferedLog::Printf( "   All right reserved.                      ");
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::SetDebugMode( DebugMode );
    
    if( UFC::FileExists( CFG_FILE ) == TRUE )
    {
        MessageDispatcher Dispatcher( CFG_FILE, FirstTimeRunning );
        Dispatcher.Run();
    }
    else
        UFC::BufferedLog::Printf( " Config file:%s not found.", CFG_FILE.c_str() );
    return 0;
}
//---------------------------------------------------------------------------














