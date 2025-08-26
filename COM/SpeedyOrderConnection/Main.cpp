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
BOOL            FIsTWSE = TRUE;
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
    printf( "SpeedyOrderConnection Copyright (c) September 2007~%d MDBS\n", UFC::ThisYear() );
    printf( "    -H                   Help.\n" );
    printf( "    -F                   first time start up.\n" );
    printf( "    -N                   Don't recover.\n" );
    printf( "    -TFX                 For TAIFEX.(Default TWSE)\n" );
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
                FRecoverBeginTime = 53030; ///< Recover from AM 5:30:30
            }
            else if( Param == "-H" )
            {
                PrintHelp();
                exit( 0 );
            }
            else if( Param == "-N" )
            {
                FTime.setCurrent();
                UpdateRecoverBeginTime();
            }
            else if( Param == "-TFX" )
            {
                FIsTWSE = FALSE;
            }             
        }
    }
    catch( UFC::Exception& ex )
    {
        printf( "Error:%s\n", ex.what() );
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
    
    SpeedyOrderConnection* OrderConnection = new SpeedyOrderConnection( FRecoverBeginTime.ToInt32() );
    int Count = 0;
    
    while( TRUE )
    {
        UFC::SleepMS( 1000 );
        
        if( Count%10 == 0 )
        {
            if( OrderConnection->IsConnect() == false )
                OrderConnection->Connect();
        }        
        FTime.setCurrent();
        UpdateRecoverBeginTime();        
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
        Count++;
    }
    delete OrderConnection;
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
