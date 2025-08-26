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
UFC::PInt32     FRecoverBeginTime;
BOOL            DumpOnly = FALSE;
BOOL            RecoverMode = TRUE;
UFC::UTime      FTime;
UFC::AnsiString AppName              = "ESFTPSetting";
UFC::AnsiString FRecoverTimeFileName = "JSRM";
//------------------------------------------------------------------------------
void PrintHelp( void )
{        
    printf( "ESFTPSetting Copyright (c) June 2015 MDBS\n" );
    printf( "-Build Date:%s \n",__DATE__ );    
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
            else if( Param.SubString( 0, 2 ) == "-N" || Param.SubString( 0, 2 ) == "-n" )
            {
                AppName = Param.SubString( 2, Param.Length() - 2 );
                UFC::BufferedLog::Printf( " AppName:[%s]", AppName.c_str() );
            }
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
        
    UFC::BufferedLog::SetDebugMode( DumpOnly );
    GCRMConnection GCRM;   
    
    
    while( TRUE )
    {
        UFC::SleepMS( 1000 );
        
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );        
        GCRM.Download();
    }
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
