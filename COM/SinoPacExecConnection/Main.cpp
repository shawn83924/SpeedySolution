// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "SinoPacExecConnection.h"
//------------------------------------------------------------------------------
UFC::PInt32     FRecoverBeginTime;
UFC::UTime      FTime;
BOOL            DumpOnly = FALSE;
UFC::AnsiString FConfigFileName;
UFC::AnsiString FRecoverTimeFileName;
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
    UFC::AnsiString ShmName( FConfigFileName );
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
#ifdef _FOR_EC
    printf( "SinoPacFIXExecConnection Copyright (c) 2009~%d MDBS\n", UFC::ThisYear() );
    printf( "Build Date:%s \n",__DATE__ );
    printf( "    -F            First time start up.\n" );
    printf( "    -D            Dump execution to screen only.(Not send to RV.)\n" );
    printf( "    -C [filename] config file name.(default: ../cfg/SinoPacFIXExecConnection.cfg)\n" );
    printf( "    -N [nickname] nickname.(default: SPFEC)\n" );
    printf( "    -R (HHMMSS)   Resend execution from HHMMSS.(Ask for the running instance).\n" );
#else
    printf( "SinoPacExecConnection Copyright (c) 2009~%d MDBS\n", UFC::ThisYear() );
    printf( "Build Date:%s \n",__DATE__ );
    printf( "    -F            First time start up.\n" );
    printf( "    -D            Dump execution to screen only.(Not send to RV.)\n" );
    printf( "    -C [filename] config file name.(default: ../cfg/SinoPacExecConnection.cfg)\n" );
    printf( "    -N [nickname] nickname.(default: SPEC)\n" );
    printf( "    -R (HHMMSS)   Resend execution from HHMMSS.(Ask for the running instance).\n" );
#endif   
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
#ifdef _FOR_EC
    FConfigFileName = "../cfg/SinoPacFIXExecConnection.cfg";    
    FRecoverTimeFileName = "SPFEC";
#else
    FConfigFileName = "../cfg/SinoPacExecConnection.cfg";    
    FRecoverTimeFileName = "SPEC";
#endif    

    
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
            else if( Param == "-N" )
            {
                i++;
                if( i < argc )
                    FRecoverTimeFileName = argv[i];
                else
                    throw( UFC::Exception("arg error\n") );
            }
            else if( Param == "-C" )
            {
                i++;
                if( i < argc )
                    FConfigFileName = argv[i];
                else
                    throw( UFC::Exception("arg error\n") );
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
    GetRecoverBeginTime();
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
    
    SinoPacExecConnection SinoPacExec( FRecoverBeginTime.ToInt32() );
    int                   Counter = 2;
    
    CreateTimeFile( );    
    while( TRUE )
    {
        UFC::SleepMS( 1000 );
        
        FTime.setCurrent();
        UpdateRecoverBeginTime();        
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
        if( Counter % 5 == 0 ) ///< Check connection every 5 sec         
            SinoPacExec.CheckConnection();
        if( CheckConfigModifiaction() == TRUE )
            SinoPacExec.Recover( GetRecoverTime());        
        Counter++;
    }
    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
