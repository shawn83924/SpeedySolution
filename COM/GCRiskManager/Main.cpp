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
Int32           AS400Check = 1;
UFC::UTime      FTime;
UFC::AnsiString AppName = "GCRiskManager";
UFC::AnsiString FRecoverTimeFileName = "GCRM";
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
    UFC::AnsiString ShmName( AppName );
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
    printf( "GCRiskManager Copyright (c) July 2011~%d MDBS\n", UFC::ThisYear() );
    printf( "Dimerco Data System Risk control module.\n" );    
    printf( "    -F            First time start up.\n" );
    printf( "    -D            Dump execution to screen only.\n" );
    printf( "    -R(HHMMSS)    Recover executions from the given time.\n" );    
    
    exit( 1 );
}
//------------------------------------------------------------------------------
void Resend( const UFC::AnsiString& TimeStr )
{
    Int32 Time = TimeStr.ToInt();
    if( Time == 0 )
    {
        printf( " Wrong time string.\n" );
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
    UFC::FileStreamEx File( FRecoverTimeFileName, "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    
    File.Write( TimeStr.c_str() , 7 );    
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
            else if( Param == "-R"  )
            {                 
                i++;
                if( i < argc )
                {
                    UFC::AnsiString TimeStr( argv[i] );
                    UFC::BufferedLog::Printf( " RecoverTime:[%s]", TimeStr.c_str() );
                    Resend( TimeStr );
                    exit( 0 );
                }
                else
                    throw( UFC::Exception("arg error\n") );
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
BOOL CheckConfigModifiaction( void )
{
    static time_t ModifyTime = 0;
    struct stat FileStat;
    
    stat( FRecoverTimeFileName.c_str() , &FileStat );    
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
    char Time[7];
    UFC::FileStreamEx File( FRecoverTimeFileName, "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    
    FTime.setCurrent();
    sprintf( Time, "%02d%02d%02d",FTime.getHour(),FTime.getMinute(),FTime.getSecond() );    
    File.Write( Time , 7 );    
}    
//------------------------------------------------------------------------------
UFC::AnsiString GetRecoverTime( void )
{
    UFC::FileStreamEx File( FRecoverTimeFileName, "r+" );
    char Time[7];    
    memset( Time, '\0', 7 );
    File.Read( Time , 7 );                
    return UFC::AnsiString( Time );
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
    int Time = 71030;
    if( RecoverMode )
        Time = GetRecoverTime().ToInt();    
            
    GCRMConnection GCRM( Time );   
    
    int Counter = 7;    
    CreateTimeFile( );    
    while( TRUE )
    {
        UFC::SleepMS( 1000 );
        
        FTime.setCurrent();
        UpdateRecoverBeginTime();        
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
        if( Counter % 10 == 0 ) ///< Check connection every 10 sec         
            GCRM.CheckConnection();
        if( CheckConfigModifiaction() == TRUE )
            GCRM.Recover( GetRecoverTime());
        Counter++;
    }
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
