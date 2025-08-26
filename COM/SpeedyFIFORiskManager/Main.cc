// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "MessageDispatcher.h"
//------------------------------------------------------------------------------
BOOL DumpOnly = FALSE;
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "SpeedyFIFORiskManager Copyright (c) September 2010 MDBS, Build Date:%s %s\n", __DATE__, __TIME__ );
    printf( "    -H                Show this page.\n" );   
    printf( "    -D                Enable debug log.\n" );   
    exit( 1 );
}
//------------------------------------------------------------------------------
void CheckArgs( int argc, char *argv[] )
{    
    try
    {
        for( int i = 1; i < argc; i++ )
        {
            UFC::AnsiString Param( argv[i] );
            if( Param == "-H" || Param == "-h" )
            {
                PrintHelp();
                exit( 0 );
            }
            else if( Param == "-D" || Param == "-d" )
                DumpOnly = TRUE;
        }             
    }
    catch( UFC::Exception& ex )
    {
        printf( ex.what() );
        PrintHelp();
    }    
}
//------------------------------------------------------------------------------
//
// main
//
//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    CheckArgs( argc, argv );
    UFC::UDateTime now;
    UFC::AnsiString LogName;
    LogName.Printf("../log/SpeedyFIFORiskManager%04d%02d%02d.log", now.getYear(), now.getMonth(), now.getDate());
    SpeedyFIFOConnection::FProcessedNIDFileName.Printf("FIFORM%04d%02d%02d.log", now.getYear(), now.getMonth(), now.getDate());
    
    UFC::BufferedLog::SetDebugMode( DumpOnly );    
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogName, 1024, TRUE ) );        
    UFC::BufferedLog::Printf( "______________________________________________" );
    UFC::BufferedLog::Printf( "                                              " );
    UFC::BufferedLog::Printf( "   Speedy HoMai risk manager interface module " );
    UFC::BufferedLog::Printf( "   Startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s %s", __DATE__, __TIME__ );    
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Log:On" );    
    else
        UFC::BufferedLog::Printf( "   Debug Log:Off" );            
    UFC::BufferedLog::Printf( "__________________________________________" );
    
    MessageDispatcher FIFODispatcher;
    
    while( TRUE )
    {
        UFC::SleepMS( 5000 );
        UFC::BufferedLog::FlushToFile();
    }    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
