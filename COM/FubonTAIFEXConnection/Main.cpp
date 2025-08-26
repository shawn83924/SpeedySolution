// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "FubonTAIFEXConnection.h"
//------------------------------------------------------------------------------
UFC::AnsiString FConfigFileName;
UFC::AnsiString GBrokerID = "";
UFC::AnsiString FLogonID;
UFC::AnsiString FRecoverTimeFileName;
UFC::PInt32     FRecoverBeginTime;
Int32           FOverlapTime = 30;
//------------------------------------------------------------------------------
Int32 CurrentTimeInt( void )
{
    UFC::UTime Now;
    Now.setCurrent();
    return ( (Now.getHour() * 10000) + (Now.getMinute() * 100) + Now.getSecond() );
}
//------------------------------------------------------------------------------
Int32 TimeToInt( int hh, int mm, int ss )
{
    return ( (hh * 10000) + (mm * 100) + ss );
}
//------------------------------------------------------------------------------
void UpdateRecoverBeginTime( void )
{
    FRecoverBeginTime = CurrentTimeInt( );
}
//------------------------------------------------------------------------------
void GetRecoverBeginTime( void )
{
    UFC::AnsiString ShmName;
    
    ShmName.Printf( "FFOC_%s_%s",GBrokerID.c_str(), FLogonID.c_str() );    
    FRecoverBeginTime.Attach( ShmName );    
    
    int LastTime = FRecoverBeginTime.ToInt32(); ///< Last time( from sharememory)
    int TimeNow  = CurrentTimeInt();            ///< Time now.
    
    if( TimeNow < LastTime ) ///< Over one day.
    {
        FRecoverBeginTime = TimeToInt( 6,0,0 );///< Reset time to 06:00:00.    
    }
    else ///< Recover from (last time - overlap time)
    {
        UFC::UTime  RecoverTime; ///< Create last time.
        
        RecoverTime.setHour(    LastTime / 10000 );
        RecoverTime.setMinute( (LastTime % 10000) / 100 );
        RecoverTime.setSecond(  LastTime % 100 );
        RecoverTime -= FOverlapTime;
        FRecoverBeginTime = TimeToInt( RecoverTime.getHour(),RecoverTime.getMinute(),RecoverTime.getSecond() );    
    }
}
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "FubonTAIFEXConnection Copyright(c) 2010~%d MDBS\n", UFC::ThisYear() );
    printf( "Build Date:%s\n", __DATE__ );
    printf( "    -H                   Show this page.\n" );    
    printf( "    -B [BrokerID]        BrokerID.(required)\n" );
    printf( "    -L [LogonID]         LogonID.(Default=BrokerID)\n" );
    printf( "    -C [filename]        config file name.(default=../cfg/FubonTAIFEXConnection.cfg)\n" );
    printf( "    -T [second]          Recover overlap time.(default=30sec)\n" );    
    printf( "    -R [HHMMSS]          Resend execution from HHMMSS(Ask for the running instance).\n" );    
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
    FConfigFileName = "../cfg/FubonTAIFEXConnection.cfg";
    
    try
    {
        for( int i = 1; i < argc; i++ )
        {
            UFC::AnsiString Param( argv[i] );
            if( Param == "-C" || Param == "-c" )
            {
                i++;
                if( i < argc )
                    FConfigFileName = argv[i];
                else
                    throw( UFC::Exception("arg error\n") );
            }
            else if( Param == "-B" || Param == "-b")
            {
                i++;
                if( i < argc )
                    GBrokerID = argv[i];
                else
                    throw( UFC::Exception("arg error\n") );
            }            
            else if( Param == "-L" )
            {
                i++;
                if( i < argc )
                    FLogonID = argv[i];
                else
                    throw( UFC::Exception("arg error\n") );
            }            
            else if( Param == "-T" )
            {
                i++;
                if( i < argc )
                {
                    FOverlapTime = atoi( argv[i] );
                    printf( " Recover overlap time %d sec.\n", FOverlapTime );                    
                }
                else
                    throw( UFC::Exception("arg error\n") );
            }            
            else if( Param == "-h" || Param == "-H" || Param == "-?")
            {
                PrintHelp( );                
            }
            else if( Param.AnsiPos("-R") != -1  )
            {                 
                i++;
                if( i < argc )
                {
                    if( GBrokerID.Length() == 0 )
                        throw( UFC::Exception("-B should before -R\n") );
                    if( FLogonID.Length() == 0 )
                        throw( UFC::Exception("-L should before -R\n") );
                    FRecoverTimeFileName.Printf( "./FFOC%s%sRecoverTime", GBrokerID.c_str() ,FLogonID.c_str() );
                    UFC::AnsiString TimeStr( argv[i] );
                    Resend( TimeStr );
                    exit( 0 );
                }
                else
                    throw( UFC::Exception("arg error\n") );
            }
        }        
        if( GBrokerID.Length() < 7 )
            throw( UFC::Exception("missing arg BrokerID.(-B)\n") );
        if( FLogonID.Length() == 0 )
            FLogonID = GBrokerID;
    }
    catch( UFC::Exception& ex )
    {
        printf( ex.what() );
        PrintHelp();
    }    
    FRecoverTimeFileName.Printf( "./FFOC%s%sRecoverTime", GBrokerID.c_str(),FLogonID.c_str() );
}
//--------------------------------------------------------------------------------------------------------------------
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
    UFC::FileStreamEx File( FRecoverTimeFileName, "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    UFC::UTime Now;
    char Time[8];
    
    Now.setCurrent();
    sprintf( Time, "%02d%02d%02d",Now.getHour(),Now.getMinute(),Now.getSecond() );    
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
    ///< Get the process last alive time. get 00:00:00 if over one day.           
    GetRecoverBeginTime(); 
    ///< Create TFubonOrderConnection object, 
    ///<
    ///< FConfigFileName: Config file name.
    ///< FRecoverBeginTime: Recover execution from this time.
    TFubonTAIFEXConnection FubonTAIFEXConnection( FConfigFileName );    
    ///< Write recover time to file.
    CreateTimeFile( );
    
    while( TRUE )
    {
        ///< Update process alive time to share memory.
        UFC::SleepMS( 1000 );   
        if( FubonTAIFEXConnection.GetConnectionState() == focReady )
            UpdateRecoverBeginTime();
        ///< Flush log file
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
        if( CheckConfigModifiaction() == TRUE )
            FubonTAIFEXConnection.Recover( GetRecoverTime());
    }    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
