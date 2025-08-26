// 
// File:   Main.cpp
// Author: yuan
//
// Created on April 4, 2012, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "KGIPTRM.h"
//------------------------------------------------------------------------------
BOOL            DumpOnly     = FALSE;
BOOL            RecoverMode = TRUE;
//------------------------------------------------------------------------------
UFC::AnsiString AppName = "KGIPTRM";
//------------------------------------------------------------------------------
void PrintHelp( void )
{    
    printf( "KGIPTRM Copyright (c) 2015~%d MDBS\n",UFC::ThisYear() );
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
                
    KGIPTRiskManager* PTRM = new KGIPTRiskManager();          
    
    while( TRUE )
    {
        UFC::SleepMS( 1000 );
        
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );        
        PTRM->CountDown();
    }
    delete PTRM;
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
