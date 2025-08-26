// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "FubonFTConnection.h"
#include "FileStream.h"
//------------------------------------------------------------------------------
UFC::AnsiString FConfigFileName( "../cfg/FubonFFTConnection.cfg" );
UFC::AnsiString FAppendName( "Def" );
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "FubonFFTConnection Copyright (c) September 2012 MDBS\n" );
    printf( "    -C [filename]        Config file name.(default=../cfg/FubonFFTConnection.cfg)\n" );
    printf( "    -N [name]            Append name.(Default:%s)\n",FAppendName.c_str() );
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
                    throw( UFC::Exception("Filename arg error\n") );
            } 
            if( Param == "-H" )
            {
            	  PrintHelp();
            	  exit( 0 ); 
            }	           
            else if( Param == "-N" )
            {
                i++;
                if( i < argc )
                    FAppendName = argv[i];
                else
                    throw( UFC::Exception("Name arg error\n") );
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
    UFC::BufferedLog::Printf( "   Fubon FTP module for TAIFEX      " );
    UFC::BufferedLog::Printf( "   startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );    
    UFC::BufferedLog::Printf( "__________________________________________" );      
    
    TFubonOrderConnection FubonFTConnection( FConfigFileName );       
    while( TRUE )
    {
        UFC::SleepMS( 1000 );       
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
    }
    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
