// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "GCExecConnection.h"
//------------------------------------------------------------------------------
UFC::PInt32     FRecoverBeginTime;
UFC::UTime      FTime;
BOOL            DumpOnly = FALSE;
MessageObject*  MessageObj;
const char*     START_TIME_FILENAME = "GCSECTime";
//----------------------------------------------------------------------------------------------------------------------
void ProcessMonitor::OnProcessConnected( BOOL IsTheFirstOne )
{
    if( IsTheFirstOne == FALSE )
    {
        UFC::BufferedLog::Printf( " ***** Process SpeedyManager already exists *****"  );
        sleep( 2 );
        exit( 0 );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void WriteUserlog( MTree *Data )
{
    UFC::AnsiString Account,Type,Msg;

    if( Data->get( "ID", Account ) && Data->get( "TYPE", Type ) && Data->get( "MSG", Msg ))
    {
        UFC::BufferedLog::Printf(",%s,%s,%s", Account.c_str(),Type.c_str(),Msg.c_str());
    }
}
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
    UFC::AnsiString ShmName( "GCSExecConnection.RecoverBeginTime" );
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
    printf( "GCSExecConnection Copyright (c) July 2012 MDBS\n" );
    printf( "    -F         First time start up.\n" );
    printf( "    -D         Dump execution to screen only.(Not send to MBus.)\n" );
    printf( "    -R(HHMMSS) Resend all execution from HHMMSS.\n" );    
    exit( 1 );
}
//------------------------------------------------------------------------------
void Resend( const UFC::AnsiString& TimeStr, char Type )
{
    Int32 Time = TimeStr.ToInt();
    if( Time == 0 )
    {
        printf( " Wrong time string[%s].\n", TimeStr.c_str() );
        return;
    }
    if( (Time/10000)>23 )
    {
        printf( " Hour should between 0 to 23.\n" );
        return;
    }
    if( ((Time%10000)/100)>59 )
    {
        printf( " Minutes should between 0 to 59.\n" );
        return;
    }    
    UFC::FileStreamEx File( START_TIME_FILENAME, "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    UFC::AnsiString   DataStr;
    
    DataStr.Printf("%s%c", TimeStr.c_str(), Type );
    File.Write( DataStr.c_str() , 8 );    
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
            if( Param == "-H" )
            {
                PrintHelp();
                exit( 0 );
            }            
            else if( Param.AnsiPos("-RTF") != -1  )
            {
                 UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 4) );
                 Resend( TimeStr, 'F' );
                 exit( 0 );
            }
            else if( Param.AnsiPos("-RTO") != -1  )
            {
                 UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 4) );
                 Resend( TimeStr, 'O' );
                 exit( 0 );
            }
            else if( Param.AnsiPos("-RP") != -1  )
            {
                 UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 3 ) );
                 Resend( TimeStr, 'P' );
                 exit( 0 );
            }
            else if( Param.AnsiPos("-R") != -1  )
            {
                 UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 2) );
                 Resend( TimeStr, 'A' );
                 exit( 0 );
            }
            else if( Param == "-F" )
            {
                FRecoverBeginTime = 0;
            }
            else if( Param == "-D" )
            {
                DumpOnly = TRUE;
            }
        }             
    }
    catch( UFC::Exception& ex )
    {
        printf( ex.what() );
        PrintHelp();
    }
}
//--------------------------------------------------------------------------------------------------------------------
BOOL CheckConfigModifiaction( void )
{
    static time_t ModifyTime = 0;
    struct stat FileStat;
    
    stat( START_TIME_FILENAME, &FileStat );
    if( ModifyTime == 0 )    
    {        
        ModifyTime = FileStat.st_mtime;
        return FALSE;
    }
    else
    {
        if( ModifyTime != FileStat.st_mtime )
        {
            ModifyTime = FileStat.st_mtime;
            return TRUE;    
        }
        return FALSE;
    }
}
//------------------------------------------------------------------------------
void CreateTimeFile( void )
{
    char Buffer[8];
    UFC::FileStreamEx File( START_TIME_FILENAME, "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    
    FTime.setCurrent();
    sprintf( Buffer, "%02d%02d%02dA",FTime.getHour(),FTime.getMinute(),FTime.getSecond() );    
    File.Write( Buffer , 8 );    
}    
//------------------------------------------------------------------------------
BOOL GetRecoverTime( UFC::AnsiString& Time, char& Type  )
{
    UFC::FileStreamEx File( START_TIME_FILENAME, "r+" );
    char Buffer[ 8 ];    
    
    if( File.Read( Buffer , 8 ) == 8 )
    {
        Time = UFC::AnsiString( Buffer, 6 ); 
        Type = Buffer[ 6 ];
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
//
// main
//
//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    CheckArgs( argc, argv );
    
    MessageObj = new MessageObject( "GCSExecConnection", "1.0", "XXXXX" );
    MessageObj->SetMonitorListener( new ProcessMonitor() );    
    MessageObj->Start(); ///< Start Migo Message pump.    
    MessageObj->WaitForConnected();            
    
    GCExecConnection ExecConnection( FRecoverBeginTime.ToInt32() );
    CreateTimeFile( );
    
    while( TRUE )
    {
        UFC::SleepMS( 1000 );
        
        FTime.setCurrent();
        UpdateRecoverBeginTime();        
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
        if( CheckConfigModifiaction() == TRUE )
        {
            UFC::AnsiString Time;
            char Type;
            
            if( GetRecoverTime( Time, Type ) == TRUE )
                ExecConnection.Recover( Time, Type );        
        }
            
    }    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
