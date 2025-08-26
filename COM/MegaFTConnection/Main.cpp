// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "MegaFTConnection.h"
//------------------------------------------------------------------------------
UFC::AnsiString FConfigFileName( "../cfg/MegaFTConnection.cfg" );
UFC::AnsiString FAppendName( "Def" );
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( " MegaFTConnection Copyright (c) 2012~%d MDBS\n", UFC::ThisYear() );
    printf( " Build Date:%s\n",__DATE__ );    
    printf( " %d bit mode.\n", (int)sizeof(void*)*8 );                        
    printf( "    -C [filename]        config file name.(default=../cfg/MegaFTConnection.cfg)\n" );
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
    UFC::BufferedLog::Printf( "   Mega File I/O module        " );
    UFC::BufferedLog::Printf( "   startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );    
    UFC::BufferedLog::Printf( "__________________________________________" );      
    
    TMegaFTConnection* MegaFTConnection = new TMegaFTConnection( FConfigFileName );       
    while( TRUE )
    {
        UFC::SleepMS( 1000 );       
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
    }
    delete MegaFTConnection;
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
