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
BOOL    DumpOnly = FALSE;
UFC::UiniFile* ErrorCodeTable = NULL;
//------------------------------------------------------------------------------
void PrintHelp( void )
{    
    printf( "SpeedyQueueRiskManager Copyright(c) 2011~%d MDBS\n",UFC::ThisYear() );
    printf( "Build Date:%s\n",__DATE__ );    
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
    UFC::AnsiString LogName = "../log/SpeedyQueueRiskManager" + UFC::GetDateString() + ".log";
    
    ErrorCodeTable  = new UFC::UiniFile( "../cfg/QueueRMErrorCode.ini", TRUE  );
    UFC::BufferedLog::SetDebugMode( DumpOnly );    
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogName, 1024, TRUE ) );        
    UFC::BufferedLog::Printf( "______________________________________________" );
    UFC::BufferedLog::Printf( "                                              " );
    UFC::BufferedLog::Printf( "   Speedy Queue interface Risk Manager module " );
    UFC::BufferedLog::Printf( "   Copyright (c) 2011~%d MDBS",UFC::ThisYear()  );    
    UFC::BufferedLog::Printf( "   Startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                               ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );    
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Log:On" );    
    else
        UFC::BufferedLog::Printf( "   Debug Log:Off" );            
    UFC::BufferedLog::Printf( "   Use Error Code Table:%s ", "../cfg/QueueRMErrorCode.ini"  );        
    UFC::BufferedLog::Printf( "______________________________________________" );
    
    MessageDispatcher* FIFODispatcher = new MessageDispatcher();
    
    while( TRUE )
    {
        UFC::SleepMS( 5000 );
        UFC::BufferedLog::FlushToFile();
    }    
    delete  ErrorCodeTable;
    delete  FIFODispatcher;
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
