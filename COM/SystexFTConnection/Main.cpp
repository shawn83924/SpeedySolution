// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "SystexFTConnection.h"
//------------------------------------------------------------------------------
UFC::AnsiString FConfigFileName( "../cfg/SystexFTConnection.cfg" );
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "SystexFTConnection Copyright (c) September 2007 MDBS\n" );
    printf( "    -C [filename]        config file name.(default=../cfg/SystexFTConnection.cfg)\n" );
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
            if( Param == "-C" )
            {
                i++;
                if( i < argc )
                    FConfigFileName = argv[i];
                else
                    throw( UFC::Exception("arg error\n") );
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
        
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   Systex File I/O module        " );
    UFC::BufferedLog::Printf( "   startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );    
    UFC::BufferedLog::Printf( "__________________________________________" );      
    
    TSystexFTConnection SystexFTConnection( FConfigFileName );       
    
    SystexFTConnection.Run();
    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
