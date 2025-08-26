// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "YuantaExecConnection.h"
//------------------------------------------------------------------------------
BOOL            DumpOnly  = FALSE;
BOOL            DisableLog = FALSE;
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "SpeedyToYB Copyright (c) 2010 MDBS\n" );
    printf( "Build Date:%s\n",__DATE__ );
    printf( "    -H         Show this.\n" );
    printf( "    -NL        Disable log.\n" );
    printf( "    -D         Dump execution to screen only.(Not send to queue.)\n" );
    exit( 1 );
}
//------------------------------------------------------------------------------
void CheckArgs( int argc, char *argv[] )
{    
    for( int i = 1; i < argc; i++ )
    {
       UFC::AnsiString Param( argv[i] );
       
       if( Param == "-H" || Param == "-h" )
       {
           PrintHelp();
           exit( 0 );
       }
       else if( Param == "-NL" )
           DisableLog = TRUE;
       else if( Param == "-D" )
           DumpOnly = TRUE;
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
    YuantaExecConnection ExecToYB;
    
    if( DisableLog == TRUE )
        UFC::BufferedLog::SetDebugMode( FALSE );
    else
        UFC::BufferedLog::SetDebugMode( TRUE );
    while( TRUE )
    {
        UFC::SleepMS( 1000 );
        
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );        
    }    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
