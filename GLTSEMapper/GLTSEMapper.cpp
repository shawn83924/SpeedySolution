#ifdef _MSC_VER
	#include "config_windows.h"
#endif
#include "GLTSEMapper.h"
#include "TSCTSEOrder.h"
#include "imFIXMapper.h"
//------------------------------------------------------------------------------
#define REFRESH_ACCOUNTS    10001
#define REFRESH_ALLOC_TABLE 10002
#define LOCK_TRANSACTION    10003
#define UNLOCK_TRANSACTION  10004
//------------------------------------------------------------------------------
FIXMapperApp* Application;
//------------------------------------------------------------------------------
BOOL RecoverMode   = TRUE;
BOOL DebugMode     = FALSE;
BOOL IsStandby     = FALSE;
BOOL BenchmarkMode = FALSE;
//------------------------------------------------------------------------------
//  **************************** Importent ****************************
//
//  You must give this mapper an unique name.( MAPPER_NAME)
//  MBus will use this name to identify this process, (for process monitoring)
//  and you should prepare a config file nameed MAPPER_NAME.cfg
//  in directory ..\cfg\.
//------------------------------------------------------------------------------
const char* MAPPER_NAME = "GLTSEMapper";
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
           printf( "   -B  BenchmarkMode.\n");
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
#include "CheckLeftQty.h"
int main( int argc, char** argv )
{
    CheckArgs( argc,argv );

    UFC::BufferedLog ScreenLog( "../log/GLTSEMapper" + UFC::GetDateString()+".log", 2048, TRUE );
    UFC::BufferedLog::SetLogObject( &ScreenLog );
    UFC::BufferedLog::Printf(" ____________________________________________");
    UFC::BufferedLog::Printf("                                             ");
	UFC::BufferedLog::Printf("    FIX Mapper:%s  ", MAPPER_NAME);
    UFC::BufferedLog::Printf("    Copyright 2004-2006 by MDBS Software Inc.");
    UFC::BufferedLog::Printf("    All right reserved.                      ");
    UFC::BufferedLog::Printf("                                             ");
    UFC::BufferedLog::Printf("    Ver:1.2.21 Build Date:%s",__DATE__ );
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

    TSCTSEOrderFactory    Factory;
    TSCTSEConfirmParser   Confirm;
    TSCTSEExecutionParser Execution;
    NIDRuleFor6Digi       Rule;
    LeftQtyStore          StockStore( RecoverMode );
    OrcTSEMapper          MyMapper( MAPPER_NAME, &Factory, &Rule, RecoverMode, &StockStore );

    TSCTSEOrderRecord::InitRender();    
    
    Application = new FIXMapperApp( MAPPER_NAME, &MyMapper,&Confirm,&Execution );
    Application->SetBenchmarkMode( BenchmarkMode );
    Application->Run();
    delete Application;
    return 1;
}
//------------------------------------------------------------------------------
UFC::AnsiString OrcTSEMapper::GetErrorMessage( UFC::AnsiString ErrorCode)
{
    UFC::AnsiString   ErrorMessage;

    if(ErrMsgFile.GetValue("ErrorMessage", ErrorCode, ErrorMessage) == FALSE )
        ErrorMessage.Printf( "Unknown Error code:%s", ErrorCode.c_str()) ;
    return ErrorMessage;
}
//------------------------------------------------------------------------------
void OrcTSEMapper::onCommand( Int32 CMD, UFC::AnsiString Data )
{
    switch( CMD )
    {
        case REFRESH_ACCOUNTS:      UFC::BufferedLog::Printf(" Reload the account list." );
                                    LStore->ReloadAccountList();
                                    break;
        case REFRESH_ALLOC_TABLE:   UFC::BufferedLog::Printf(" Reload the prelock data." );
                                    LStore->ReloadCheckStatus();
                                    break;
        case LOCK_TRANSACTION:      UFC::BufferedLog::Printf(" Lock the GLMapper." );
                                    LStore->Lock();
                                    break;
        case UNLOCK_TRANSACTION:    UFC::BufferedLog::Printf(" Unlock the GLMapper." );
                                    LStore->Unlock();
                                    break;
    }
}
//------------------------------------------------------------------------------
void OrcTSEMapper::LoadT32( const UFC::AnsiString& IniPath )
{
    T32Parser TSE_T32Parser( IniPath );
    for( register int i = 0; i < TSE_T32Parser.GetRecordCount(); i++ )
    {
        UFC::AnsiString Symbol, Value;
        Int32 Unit;
        double WarningHighPrice = 0.0;
        double WarningLowPrice = 0.0;

        UFC::TRecord* Record = TSE_T32Parser.GetRecord(i);
        Record->GetField( "Symbol", Symbol );
        Symbol.TrimRight();
        Record->GetField( "TradeUnit", Value );
        Unit = Value.ToInt();
        UFC::BufferedLog::DebugPrintf( " Symbol:[%s] Trade Unit:[%d]", Symbol.c_str(), Unit );

        if( Symbol.ToInt() < 9500 && Symbol.ToInt() > 9300 )
        {
            Record->GetField( "WarningPriceHigh", Value );
            WarningHighPrice = UFC::IntToDouble( Value.ToInt(), 2 );
            Record->GetField( "WarningPriceLow", Value );
            WarningLowPrice = UFC::IntToDouble( Value.ToInt(), 2 );
            UFC::BufferedLog::DebugPrintf( " Warning High Px:[%.2f] Warning Low Px:[%.2f]", WarningHighPrice, WarningLowPrice );
        }
        FT32Table.Add( Symbol, new TradeUnit( Symbol, Unit, WarningHighPrice, WarningLowPrice ) );
    }
}
//---------------------------------------------------------------------------
BOOL OrcTSEMapper::T32Check( const UFC::AnsiString& Symbol )
{
    TradeUnit* Record = FT32Table.GetObjectByKey( Symbol );
    if( Record != NULL )
        return FALSE;
    return TRUE;
}
//---------------------------------------------------------------------------
OrcTSEMapper::OrcTSEMapper( const UFC::AnsiString& MapperName, OrderFactory* Factory, NIDRule* Rule, BOOL Recover,LeftQtyStore* LeftStore)
:FIXMapper( MapperName, Factory, Rule, Recover )
,LStore( LeftStore )
,ErrMsgFile( "../cfg/ErrorMessage.ini" )
{
    FOrderFormat.SetIniFile( "../cfg/TseOrder.ini" );
      FOrderRender.SetDataFormat( &FOrderFormat );
}
//---------------------------------------------------------------------------
