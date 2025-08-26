// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "ChinaTrustExecConnection.h"
//------------------------------------------------------------------------------
UFC::PInt32     FRecoverBeginTime;
UFC::UTime      FTime;
BOOL            DumpOnly      = FALSE;
BOOL            NoQuoteCancel = FALSE;
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
    UFC::AnsiString ShmName( "ChinaTrustExecConnection.RecoverBeginTime" );
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
    printf( "ChinaTrustExecConnection Copyright (c) September 2008 MDBS\n" );
    printf( "    -F                 First time start up.\n" );
    printf( "    -D                 Dump execution to screen only.(Not send to queue.)\n" );
    printf( "    -XQC               Filter out Quote canceled execution.\n" );
    printf( "    -R(HHMMSS)         Resend execution from HHMMSS.\n" );
    printf( "    -R(HHMMSS:hhmmss)  Resend execution from HHMMSS to hhmmss.\n" );
    printf( "    -RC(HHMMSS)        Resend order confirmation from HHMMSS.\n" );
    printf( "    -RC(HHMMSS:hhmmss) Resend order confirmation from HHMMSS to hhmmss.\n" );
    printf( "    -RF(HHMMSS)        Resend fill execution from HHMMSS.\n" );
    printf( "    -RF(HHMMSS:hhmmss) Resend fill execution from HHMMSS to hhmmss.\n" );
    exit( 1 );
}
//------------------------------------------------------------------------------
int GetTime( const UFC::AnsiString& TimeStr, UFC::AnsiString& FormatTime )
{
    Int32 Time = TimeStr.ToInt();
    Int32 H = Time/10000;
    Int32 M = (Time%10000)/100;
    Int32 S = Time%100;

    if( Time <= 0 )
    {
        printf( " Wrong time string [%s].\n", TimeStr.c_str() );
        return -1;
    }
    if( H >23 )
    {
        printf( " Hour should between 0 to 23.\n" );
        return -1;
    }
    if( M >59 )
    {
        printf( " Minutes should between 0 to 59.\n" );
        return -1;
    }
    if( S >59 )
    {
        printf( " Seconds should between 0 to 59.\n" );
        return -1;
    }
    FormatTime.Printf( "%02d:%02d:%02d", H, M, S );
    return (H*10000 + M*100 + S);
}
//------------------------------------------------------------------------------
void Resend( const UFC::AnsiString& TimeStr, char Flag  )
{
    UFC::AnsiString FormatTime,EndFormatTime,OutStr;

    if( TimeStr.Length() == 6  )  ///< begin
    {
        int BeginTime = GetTime( TimeStr, FormatTime );
        if( BeginTime == -1 ) ///< error
            return;
        if( Flag == 'A' )
            printf( " Recover all execution from %s.\n", FormatTime.c_str() );
        else if( Flag == 'C' )
            printf( " Recover order confirmations from %s.\n", FormatTime.c_str());
        else if( Flag == 'F' )
            printf( " Recover fill executions from %s.\n",  FormatTime.c_str());
        OutStr.Printf( "%06d%06d%c", BeginTime, 0, Flag );
        UFC::FileStreamEx File( "CCCTime", "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
        File.Write( OutStr.c_str() , OutStr.Length() + 1 );
    }
    else if( TimeStr.Length() == 13) ///< Begin to end
    {
        int BeginTime = GetTime( TimeStr.SubString(0,6), FormatTime );
        int EndTime   = GetTime( TimeStr.SubString(7,6), EndFormatTime );
        if( BeginTime == -1 || EndTime == -1 ) ///< error
            return;
        if( EndTime < BeginTime )
        {
            printf( " Error! %s earlier than %s.\n", EndFormatTime.c_str(), FormatTime.c_str() );
            return;
        }
        if( Flag == 'A' )
            printf( " Recover all execution from %s to %s.\n", FormatTime.c_str(), EndFormatTime.c_str() );
        else if( Flag == 'C' )
            printf( " Recover order confirmations from %s to %s.\n", FormatTime.c_str(), EndFormatTime.c_str());
        else if( Flag == 'F' )
            printf( " Recover fill executions from %s to %s.\n",  FormatTime.c_str(), EndFormatTime.c_str());
        OutStr.Printf( "%06d%06d%c", BeginTime, EndTime, Flag );
        UFC::FileStreamEx File( "CCCTime", "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
        File.Write( OutStr.c_str() , OutStr.Length() + 1 );
    }
    else
        printf( " Wrong time string [%s].\n", TimeStr.c_str() );
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
            if( Param == "-H" || Param == "-h" )
            {
                PrintHelp();
                exit( 0 );
            }            
            else if( Param.AnsiPos("-RC") != -1  || Param.AnsiPos("-rc") != -1 )
            {
                 UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 3) );
                 Resend( TimeStr, 'C' );
                 exit( 0 );
            }
            else if( Param.AnsiPos("-RF") != -1  || Param.AnsiPos("-rf") != -1 )
            {
                 UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 3) );
                 Resend( TimeStr, 'F' );
                 exit( 0 );
            }
            else if( Param.AnsiPos("-R") != -1  || Param.AnsiPos("-r") != -1 )
            {
                 UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 2) );
                 Resend( TimeStr, 'A' );
                 exit( 0 );
            }
            else if( Param == "-F" || Param == "-f")
                FRecoverBeginTime = 0;
            else if( Param == "-D" || Param == "-d" )
                DumpOnly = TRUE;
            else if( Param == "-XQC" )
                NoQuoteCancel = TRUE;
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
    
    stat("CCCTime", &FileStat );
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
// Get current time and save to file.
//------------------------------------------------------------------------------
void CreateTimeFile( void )
{
    char Time[16];
    UFC::FileStreamEx File( "CCCTime", "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    
    FTime.setCurrent();
    sprintf( Time, "%02d%02d%02d000000A",FTime.getHour(),FTime.getMinute(),FTime.getSecond() );
    File.Write( Time , 14 );
}    
//------------------------------------------------------------------------------
UFC::AnsiString RecoverBeginTime( void )
{
    UFC::FileStreamEx File( "CCCTime", "r+" );
    char Time[ 8 ];
    
    File.Read( Time , 6 );
    return UFC::AnsiString( Time, 6 );
}
//------------------------------------------------------------------------------
UFC::AnsiString RecoverEndTime( void )
{
    UFC::FileStreamEx File( "CCCTime", "r+" );
    char Time[ 8 ];

    File.Read( Time , 6 );///< Begin
    File.Read( Time , 6 );///< End
    return UFC::AnsiString( Time, 6 );
}
//------------------------------------------------------------------------------
char RecoverFlag( void )
{
    UFC::FileStreamEx File( "CCCTime", "r+" );
    char Time[ 16 ];
    char Flag;

    File.Read( Time , 12 ); ///< Begin + End
    File.Read( &Flag , 1 );
    return Flag;
}
//------------------------------------------------------------------------------
//
// main
//
//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    CheckArgs( argc, argv );
    
    ChinaTrustExecConnection ChinaTrustExecConnection( FRecoverBeginTime.ToInt32() ); ///< FRecoverBeginTime from share memory
    CreateTimeFile(); ///< Create file use current time.
    UFC::BufferedLog::SetDebugMode( DumpOnly );
    
    while( TRUE )
    {
        UFC::SleepMS( 1000 );
        
        FTime.setCurrent();
        UpdateRecoverBeginTime();        
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
        if( CheckConfigModifiaction() == TRUE )
            ChinaTrustExecConnection.Recover( RecoverBeginTime(), RecoverEndTime(), RecoverFlag() );
    }
    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
