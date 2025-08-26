// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "FubonFTConnection.h"
//------------------------------------------------------------------------------
UFC::AnsiString FConfigFileName( "../cfg/FubonFTConnection.cfg" );
UFC::AnsiString FAppendName( "Def" );
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "FubonFTConnection Copyright (c) September 2010~%d MDBS\n", UFC::ThisYear() );
    printf( "Build Date:%s\n",__DATE__ );
    printf( "    -C [filename]        config file name.(default=../cfg/FubonFTConnection.cfg)\n" );
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
    UFC::BufferedLog::Printf( "   Fubon TWSE,OTC File Transfer module    " );
    UFC::BufferedLog::Printf( "   Copyright (c) 2010~%d MDBS             ", UFC::ThisYear() );
    UFC::BufferedLog::Printf( "   Startup at: %s.                        ", UFC::Hostname );
    UFC::BufferedLog::Printf( "   %d bit mode                            ", sizeof(void*)*8 );                    
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   Build Date:%s                          ",__DATE__ );    
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
