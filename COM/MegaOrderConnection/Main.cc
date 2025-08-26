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
BOOL    	      DumpOnly = FALSE;
Int32                 FOverlapTime = 30;
Int32                 FGroup = 0;
UFC::PInt32           FRecoverBeginTime;
UFC::AnsiString       GRecoverTimeFileName;
UFC::AnsiString       GRecoverTimeStr("");
UFC::AnsiString       GAppName( "MegaOrderConnection" );
UFC::AnsiString       CONFIG_FILE( "../cfg/MegaOrderConnection.cfg" );
TTaifexConnection*    FConnection;
TConnectionStateEnum  GConnectionState = csDisconnect;
//------------------------------------------------------------------------------
const Int32 MSG_LENGTH = 205;
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
    
    ShmName.Printf( "MOC_%s",GAppName.c_str() );    
    FRecoverBeginTime.Attach( ShmName, 230000 );    
    
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
    printf( " MegaOrderConnection Copyright (c) 2012~%d MDBS\n", UFC::ThisYear() );
    printf( " %d bit mode. Build Date:%s Time:%s\n", (int)sizeof(void*)*8, __DATE__ , __TIME__);    
    printf( "    -D                  Dump execution to screen only.(Not send to Queue.)\n" );
    printf( "    -N [Name]           Application sub name.\n" );
    printf( "    -R [HHMMSS]         Resend execution from HHMMSS(Ask for the running instance).\n" );    
    exit( 1 );
}
//------------------------------------------------------------------------------
BOOL CheckArgs( int argc, char *argv[] )
{    
    BOOL IsResend = FALSE;
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
	    else if( Param == "-N" )
            {
                i++;
                if( i < argc )		
		{
		    GAppName.Printf( "MegaOrderConnection%s",argv[i] );
		    CONFIG_FILE.Printf( "../cfg/%s.cfg", GAppName.c_str() );
		}			
                else
                    throw( UFC::Exception("arg error\n") );
            }
            else if( Param == "-D" || Param == "-d" )
                DumpOnly = TRUE;
            else if( Param == "-F"  || Param == "-f" )
                FRecoverBeginTime = 0;
            else if( Param.AnsiPos("-R") != -1  )
            {                 
                i++;
                if( i < argc )
                {
                    GRecoverTimeStr = argv[i];                    
                    IsResend = TRUE;
                }
                else
                    throw( UFC::Exception("arg -R error\n") );
            }	                
        }                 
	GRecoverTimeFileName.Printf( "./%sRecoverTime", GAppName.c_str());	    	
    }
    catch( UFC::Exception& ex )
    {
        printf( ex.what() );
        PrintHelp();
    }    
    return IsResend;
}
//------------------------------------------------------------------------------
void Resend( const UFC::AnsiString& TimeStr )
{
    Int32             Time = TimeStr.ToInt();

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
    UFC::FileStreamEx File( GRecoverTimeFileName, "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    
    printf( "---------------------------------------------.\n" );
    printf( " Recover Executions from Time[%s].\n", TimeStr.c_str() );
    printf( "---------------------------------------------.\n" );
    File.Write( TimeStr.c_str() , 7 );    
}  
//------------------------------------------------------------------------------
void CreateTimeFile( void )
{    
    UFC::UTime        Now;
    char              Time[8];
    UFC::FileStreamEx File( GRecoverTimeFileName, "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    
    Now.setCurrent();
    sprintf( Time, "%02d%02d%02d",Now.getHour(),Now.getMinute(),Now.getSecond() );    
    File.Write( Time , 7 );    
}    
//------------------------------------------------------------------------------
UFC::AnsiString GetRecoverTime( void )
{
    UFC::FileStreamEx File( GRecoverTimeFileName, "r+" );
    char Time[ 7 ];    
    
    File.Read( Time , 7 );    
    return UFC::AnsiString( Time ); 
}
//--------------------------------------------------------------------------------------------------------------------
BOOL CheckConfigModifiaction( void )
{
    static time_t ModifyTime = 0;
    struct stat FileStat;
        
    stat( GRecoverTimeFileName.c_str() , &FileStat );    
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
//
// main
//
//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    if( CheckArgs( argc, argv ) == TRUE )
    {
	Resend( GRecoverTimeStr );
	return (EXIT_SUCCESS);
    }
    GetRecoverBeginTime();    
    
    UFC::BufferedLog::SetDebugMode( DumpOnly );
    UFC::AnsiString LogName = "../log/" + GAppName + UFC::GetDateString() + ".log";    
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogName, 10240, TRUE ) );
        
    UFC::BufferedLog::Printf( "______________________________________________" );
    UFC::BufferedLog::Printf( "                                              " );
    UFC::BufferedLog::Printf( "   Speedy - Mega backend interface module " );
    UFC::BufferedLog::Printf( "   Startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "   %d bit mode.                ", (int)sizeof(void*)*8 );                    
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s Time:%s", __DATE__ , __TIME__);        
    UFC::BufferedLog::Printf( "   Application name:%s ", GAppName.c_str() ); 
    UFC::BufferedLog::Printf( "   Recover time file name:%s", GRecoverTimeFileName.c_str() ); 
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Mode:On" );    
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off" );    
        
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::DebugPrintf( " FRecoverBeginTime:[%d]", FRecoverBeginTime.ToInt32() );
    
    MessageDispatcher OrderConnection;	
    
    CreateTimeFile();    
    
    while( TRUE )
    {           
       fflush( stdout );
       if( GConnectionState == csReady  )
           UpdateRecoverBeginTime();    
	else
	    OrderConnection.CheckConnection();
	if( CheckConfigModifiaction() == TRUE )
           OrderConnection.Recover( GetRecoverTime());	
        UFC::BufferedLog::FlushToFile();
        UFC::SleepMS( 5000 );        
    }    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
