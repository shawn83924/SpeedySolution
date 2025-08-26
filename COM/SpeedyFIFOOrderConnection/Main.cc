// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "MessageDispatcher.h"
#include "SpeedyFIFOOrderConnection.h"
//------------------------------------------------------------------------------
BOOL    	DumpOnly = FALSE;
BOOL		IsRecoverFinished = FALSE;
UFC::PInt32     FRecoverBeginTime;
UFC::UTime      FTime;
TTaifexConnection*    FConnection;
const Int32 MSG_LENGTH = 205;
//------------------------------------------------------------------------------
void UpdateRecoverBeginTime( void )
{
    FTime.setCurrent();
    FRecoverBeginTime = ( (FTime.getHour() * 10000) +
                          (FTime.getMinute() * 100) +
                          (FTime.getSecond()) );
}
//------------------------------------------------------------------------------
void GetRecoverBeginTime( void )
{
    UFC::AnsiString ShmName( "SpeedyFIFOOrderConnection" );
    FRecoverBeginTime.Attach( ShmName );
    
    int t = FRecoverBeginTime.ToInt32();
    FTime.setHour( t / 10000 );
    FTime.setMinute( (t % 10000) / 100 );
    FTime.setSecond( t % 100 );
    FTime -= 60;
}
//------------------------------------------------------------------------------
void PrintHelp( void )
{    
    printf( "SpeedyFIFOConnection Copyright (c) 2010 MDBS\n" );
    printf( "    -D       Dump execution to screen only.(Not send to FIFO.)\n" );
    
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
            else if( Param == "-F"  || Param == "-f" )
                FRecoverBeginTime = 0;
            
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
    GetRecoverBeginTime();
    CheckArgs( argc, argv );
    UFC::BufferedLog::SetDebugMode( DumpOnly );
    UFC::AnsiString LogName = "../log/SpeedyFIFOConnection" + UFC::GetDateString() + ".log";    
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogName, 1024, TRUE ) );
        
    UFC::BufferedLog::Printf( "______________________________________________" );
    UFC::BufferedLog::Printf( "                                              " );
    UFC::BufferedLog::Printf( "   Speedy control backend interface module " );
    UFC::BufferedLog::Printf( "   Startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );
    
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Mode:On" );    
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off" );    
        
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::DebugPrintf( " FRecoverBeginTime:[%d]", FRecoverBeginTime.ToInt32() );
    
    MessageDispatcher OrderConnection;	
    
    while( TRUE )
    {           
        fflush( stdout );
       	if( IsRecoverFinished  )
       	{
            FTime.setCurrent();
            UpdateRecoverBeginTime();    
        }
        OrderConnection.CheckConnection();
        UFC::BufferedLog::FlushToFile();
        UFC::SleepMS( 5000 );        
    }    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
