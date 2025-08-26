// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <cstdio>
#include <cstdlib>

#include "GlobalVariable.h"
#include "MBusAdapter.h"
#include "PMFExecToFile.h"
//------------------------------------------------------------------------------
void UpdateRecoverBeginTime( void )
{
    gRecoverTime.setCurrent();
    gRecoverBeginTime = ( ( gRecoverTime.getHour() * 10000 ) +
                          ( gRecoverTime.getMinute() * 100 ) +
                          gRecoverTime.getSecond() );
}
//------------------------------------------------------------------------------
void GetRecoverBeginTime( void )
{
    UFC::AnsiString shmName( gRecoverTimeFilePathName );
    gRecoverBeginTime.Attach( shmName );
    
    int t = gRecoverBeginTime.ToInt32();
    gRecoverTime.setHour( t / 10000 );
    gRecoverTime.setMinute( ( t % 10000 ) / 100 );
    gRecoverTime.setSecond( t % 100 );
    gRecoverTime -= 60;
}
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "PMFExecToFile Copyright (c) September 2009~%d MDBS, Build: %s %s\n", UFC::ThisYear() ,__DATE__, __TIME__ );
    printf( "    -F            First time start up.\n" );
    printf( "    -Cfg(FileName) Configure File Name.)\n");
    printf( "    -D            Dump execution to screen only.(Not send to RV.)\n" );    
    printf( "    -EDYYYMMDD    Execution Report File Date.\n" );    
    printf( "    -R (HHMMSS)   Resend execution from HHMMSS.(Ask for the running instance).\n" );
    printf( "    -N (FileName) Recover time File Name.\n" );
    exit( 1 );
}
//------------------------------------------------------------------------------
void Resend( const UFC::AnsiString& TimeStr )
{
    if( gRecoverTimeFileName.Length() > 0 )
        gRecoverTimeFilePathName.Printf( "%s/%s", gRecoverTimeFileDirectory.c_str(), gRecoverTimeFileName.c_str() );
    
    Int32 Time = TimeStr.ToInt();
    if( Time > 0 )
    {
        if( ( Time / 10000 ) <= 23 )
        {
            if( ( ( Time % 10000 ) / 100 ) <= 59 )
            {
                try
                {
                    UFC::FileStreamEx resendFile( gRecoverTimeFilePathName, "w+", FALSE, 0 );  // "w+" = O_CREAT|O_TRUNC|O_RDWR
                    resendFile.Write( TimeStr.c_str(), 6 );
                    std::printf( " Write resend Time[%s] to File[%s].\n", TimeStr.c_str(), gRecoverTimeFilePathName.c_str() );
                }
                catch( UFC::FileException& fe )
                {
                    std::printf( " Write resend Time[%s] to File[%s] exception:%s.\n", TimeStr.c_str(), gRecoverTimeFilePathName.c_str(), fe.what() );
                }
            }
            else
                std::printf( " [%s] Minutes should between 0 to 59.\n", TimeStr.c_str() );
        }
        else
            std::printf( " [%s] Hour should between 0 to 23.\n", TimeStr.c_str() );
    }
    else
        std::printf( " Wrong time string[%s].\n", TimeStr.c_str() );
}    
//------------------------------------------------------------------------------
void CheckArgs( int argc, char *argv[] )
{
    try
    {
        for( int i = 1; i < argc; i++ )
        {
            UFC::AnsiString Param( argv[i] );
            UFC::AnsiString upperParam = Param.UpperCase();
            if( ( upperParam == "-H" ) || ( upperParam == "--HELP" ) )
            {
                PrintHelp();
                exit( 0 );
            }
            else if( Param == "-N" )
            {
                i++;
                if( i < argc )
                    gRecoverTimeFileName = argv[i];
                else
                    throw( UFC::Exception( "argument error\n" ) );
            }
            else if( upperParam.SubString(0, 4) == "-CFG" )
            {
                gConfigureFileName = Param.SubString(4, Param.Length() - 4);
                if( gConfigureFileDirectory[gConfigureFileDirectory.Length() - 1] == '/' )
                    gConfigureFilePathName.Printf( "%s%s", gConfigureFileDirectory.c_str(), gConfigureFileName.c_str() );
                else
                    gConfigureFilePathName.Printf( "%s/%s", gConfigureFileDirectory.c_str(), gConfigureFileName.c_str() );
            }
            else if( Param.AnsiPos( "-R" ) != -1 )
            {
                i++;
                if( i < argc )
                    gResendTimeStr = argv[i];
                else
                    throw( UFC::Exception( "argument error\n" ) );
            }
            else if( Param == "-F" )
            {
                gIsFirstTimeStartup = true;
            }
            else if( Param == "-D" )
            {
                gDumpOnly = true;
            }
            else if( Param.AnsiPos( "-ED" ) != -1 )
            {
                if( Param.Length() == 10 )
                {
                    gExecRptFileDate = Param.SubString( 3, 7 );
                }
            }
        }  //for( int i = 1; i < argc; i++ )
    }
    catch( UFC::Exception& ex )
    {
        printf( "%s",  ex.what() );
        PrintHelp();
    }
}
//--------------------------------------------------------------------------------------------------------------------
BOOL CheckConfigModifiaction( void )
{
    static time_t ModifyTime = 0;
    struct stat FileStat;
    
    stat( gRecoverTimeFilePathName.c_str() , &FileStat );
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
    char timeBuffer[7];
    UFC::FileStreamEx File( gRecoverTimeFilePathName, "w+", FALSE, 0 );  // "w+" = O_CREAT|O_TRUNC|O_RDWR
    
    gRecoverTime.setCurrent();
    sprintf( timeBuffer, "%02d%02d%02d", gRecoverTime.getHour(), gRecoverTime.getMinute(), gRecoverTime.getSecond() );    
    File.Write( timeBuffer, 6 );    
}    
//------------------------------------------------------------------------------
UFC::AnsiString GetRecoverTime( void )
{
    UFC::FileStreamEx File( gRecoverTimeFilePathName, "r+", FALSE, 0 );
    char timeBuffer[7];
    timeBuffer[6] = 0;
    
    File.Read( timeBuffer, 6 );
    return UFC::AnsiString( timeBuffer );
}
//------------------------------------------------------------------------------
//
// main
//
//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    gAppStartUpTime.setCurrent();
    
    CheckArgs( argc, argv );
    if( gResendTimeStr.Length() > 0 )
    {
        Resend( gResendTimeStr );
        exit( 0 );
    }
    
    gLoadSetting();
    
    UFC::AnsiString logFileName = "";
    logFileName.Printf( "../log/%s%04d%02d%02d.log", gAppName.c_str(), gAppStartUpTime.getYear(), gAppStartUpTime.getMonth(), gAppStartUpTime.getDate() );
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( logFileName, 1024, TRUE ) );
    gPrintSetting();

    GetRecoverBeginTime();
    if( gIsFirstTimeStartup )
        gRecoverBeginTime = 0;
    else
        UpdateRecoverBeginTime();        
    
    CMBusAdapter *mbusAdpPtr = new CMBusAdapter();
    
    PMFExecToFile ExecToFile( gRecoverBeginTime.ToInt32() );
    int Counter = 2;
    
    CreateTimeFile( );
    while( TRUE )
    {
        UFC::SleepMS( 1000 );
        
        UpdateRecoverBeginTime();        
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
        if( Counter % 5 == 0 ) ///< Check connection every 5 sec         
            ExecToFile.CheckConnection();
        if( CheckConfigModifiaction() == TRUE )
            ExecToFile.Recover( GetRecoverTime());        
        Counter++;
    }
    
    delete mbusAdpPtr;
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
