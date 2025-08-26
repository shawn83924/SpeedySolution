// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "GCRMConnection.h"
//------------------------------------------------------------------------------
BOOL            DumpOnly = FALSE;
BOOL            RecoverMode = TRUE;
//------------------------------------------------------------------------------
UFC::AnsiString AppName = "GCSRiskManager";
//------------------------------------------------------------------------------
void PrintHelp( void )
{    
    printf( "GCSRiskManager Copyright (c) July 2011 MDBS\n" );
    printf( "    -D            Debug mode.\n" );
    printf( "    -F            First time running.\n" );
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
            else if( Param == "-F" || Param == "-f" )
                RecoverMode = FALSE;            
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
                
    GCRMConnection* GCRM = new GCRMConnection();          
    
    while( TRUE )
    {
        UFC::SleepMS( 1000 );
        
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );        
    }
    delete GCRM;
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
