// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "SpeedyOrderConnection.h"
//------------------------------------------------------------------------------
UFC::PInt32     FRecoverBeginTime;
UFC::UTime      FTime;
//------------------------------------------------------------------------------
void UpdateRecoverBeginTime( void )
{
    FRecoverBeginTime = ( (FTime.getHour() * 10000) +
                          (FTime.getMinute() * 100) +
                          (FTime.getSecond()) );
}
//------------------------------------------------------------------------------
void GetRecoverBeginTime( void )
{
    UFC::AnsiString ShmName( "SpeedyOrderConnection.FRecoverBeginTime" );
    FRecoverBeginTime.Attach( ShmName );
    
    int t = FRecoverBeginTime.ToInt32();
    FTime.setHour( t / 10000 );
    FTime.setMinute( (t % 10000) / 100 );
    FTime.setSecond( t % 100 );
    FTime -= 60;
    
    UpdateRecoverBeginTime();
}
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "SpeedyOrderConnection Copyright (c) September 2007 MDBS\n" );
    printf( "    -F                   first time start up.\n" );
    exit( 1 );
}
//------------------------------------------------------------------------------
void CheckArgs( int argc, char *argv[] )
{    
    GetRecoverBeginTime();
    
    try
    {
        for( int i = 1; i < argc; i++ )
        {
            UFC::AnsiString Param( argv[i] );
            if( Param == "-F" )
            {
                FRecoverBeginTime = 0;
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
    
    SpeedyOrderConnection SpeedyOrderConnection( FRecoverBeginTime.ToInt32() );
    
    while( TRUE )
    {
        UFC::SleepMS( 1000 );
        
        FTime.setCurrent();
        UpdateRecoverBeginTime();
        
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
    }
    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
