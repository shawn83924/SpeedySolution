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
const char*     START_TIME_FILENAME = "KGIExecConnectionTime";
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
    UFC::AnsiString ShmName( "KGIExecConnection.RecoverBeginTime" );
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
    printf( "KGIExecConnection Copyright (c) July 2007~2014 MDBS, Build Date:%s %s\n", __DATE__, __TIME__ );
    printf( "    \n" );
    printf( "    -F            First time start up.\n" );
    printf( "    -D            Dump execution to screen only.(Not send to MBus.)\n" );
    printf( "    -R(HHMMSS)    Resend all execution from HHMMSS.(Both TAIFEX and PATS)\n" );
    printf( "    -RP(HHMMSS)   Resend PATS execution from HHMMSS.\n" );
    printf( "    -RTF(HHMMSS)  Resend TAIFEX Futures execution from HHMMSS.\n" );
    printf( "    -RTO(HHMMSS)  Resend TAIFEX Options execution from HHMMSS.\n" );
    printf( "    \n" );
    printf( "    -RC(HHMMSS)   Resend all confirm execution from HHMMSS.(Both TAIFEX and PATS)\n" );
    printf( "    -RPC(HHMMSS)  Resend PATS confirm execution from HHMMSS.\n" );
    printf( "    -RTFC(HHMMSS) Resend TAIFEX Futures confirm execution from HHMMSS.\n" );
    printf( "    -RTOC(HHMMSS) Resend TAIFEX Options confirm execution from HHMMSS.\n" );
    printf( "    \n" );
    printf( "    -RF(HHMMSS)   Resend all filled execution from HHMMSS.(Both TAIFEX and PATS)\n" );
    printf( "    -RPF(HHMMSS)  Resend PATS filled execution from HHMMSS.\n" );
    printf( "    -RTFF(HHMMSS) Resend TAIFEX Futures filled execution from HHMMSS.\n" );
    printf( "    -RTOF(HHMMSS) Resend TAIFEX Options filled execution from HHMMSS.\n\n" );
    exit( 1 );
}
//------------------------------------------------------------------------------
void Resend( const UFC::AnsiString& TimeStr, char Type, char EType )
{
    Int32 Time = TimeStr.ToInt();
    if( Time == 0 )
    {
        printf( " Wrong time string[%s].\n", TimeStr.c_str() );
        return;
    }
    if( (Time/10000)>23 || (Time/10000)<1 )
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
    UFC::AnsiString   DataStr,ETStr;
    
    DataStr.Printf("%06d%c%c", Time, Type, EType );
    File.Write( DataStr.c_str() , 9 );    
    
    printf( "\n\nAsk running KGIExecConnection resend\n" );
    switch( EType )
    {
        case 'A': ETStr =" ";break;
        case 'C': ETStr =" Confirm";break;
        case 'F': ETStr =" Filled";break;
    }
    switch( Type )
    {
        case 'A':printf( "[All]%s Executions\n",ETStr.c_str() );break;
        case 'P':printf( "[PATS]%s Executions\n",ETStr.c_str() );break;
        case 'O':printf( "[Options]%s Executions\n",ETStr.c_str() );break;
        case 'F':printf( "[Futures]%s Executions\n",ETStr.c_str() );break;
    }
    printf( "From[%02d:%02d:%02d]\n", Time/10000, (Time/100)%100, Time%100 );
    printf( "--------------------------------------------------\n" );
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
            if( ( Param == "-H" ) || ( Param == "-h" ) ) 
            {
                PrintHelp();
                exit( 0 );
            }                        
            else if( Param.AnsiPos("-RTFC") != -1  )
            {
                if( Param.Length() > 5 )
                {      
                     UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 5) );
                     Resend( TimeStr, 'F', 'C' );
                }
                else
                {
                    printf( "\nResend TAIFEX Futures confirm execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RTFC123059\n");
                }
                exit( 0 );                    
            }
            else if( Param.AnsiPos("-RTOC") != -1  )
            {
                if( Param.Length() > 5 )
                {
                    UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 5) );
                    Resend( TimeStr, 'O', 'C' );
                }
                else
                {                    
                    printf( "\nResend TAIFEX Options confirm execution from HHMMSS failed.\n");                    
                    printf( "Example:KGIExecConnection -RTOC123059\n");
                }
                exit( 0 );
            }
            else if( Param.AnsiPos("-RTFF") != -1  )
            {
                if( Param.Length() > 5 )
                {      
                     UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 5) );
                     Resend( TimeStr, 'F', 'F' );
                }
                else
                {
                    printf( "\nResend TAIFEX Futures filled execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RTFF123059\n");
                }
                exit( 0 );                    
            }
            else if( Param.AnsiPos("-RTOF") != -1  )
            {
                if( Param.Length() > 5 )
                {
                    UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 5) );
                    Resend( TimeStr, 'O', 'F' );
                }
                else
                {                    
                    printf( "\nResend TAIFEX Options filled execution from HHMMSS failed.\n");                    
                    printf( "Example:KGIExecConnection -RTOF123059\n");
                }
                exit( 0 );
            }
            else if( Param.AnsiPos("-RPC") != -1  )
            {
                if( Param.Length() > 4 )
                {
                    UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 4 ) );
                    Resend( TimeStr, 'P', 'C' );
                }
                else
                {                    
                    printf( "\nResend PATS confirm execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RPC123059\n");
                }
                exit( 0 );
            }
            else if( Param.AnsiPos("-RTF") != -1  )
            {
                if( Param.Length() > 4 )
                {      
                     UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 4) );
                     Resend( TimeStr, 'F', 'A' );
                }
                else
                {
                    printf( "\nResend TAIFEX Futures execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RTF123059\n");
                }
                exit( 0 );                    
            }
            else if( Param.AnsiPos("-RTO") != -1  )
            {
                if( Param.Length() > 4 )
                {
                    UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 4) );
                    Resend( TimeStr, 'O', 'A' );
                }
                else
                {                    
                    printf( "\nResend TAIFEX Options execution from HHMMSS failed.\n");                    
                    printf( "Example:KGIExecConnection -RTO123059\n");
                }
                exit( 0 );
            }
            else if( Param.AnsiPos("-RPF") != -1  )
            {
                if( Param.Length() > 4 )
                {
                    UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 4 ) );
                    Resend( TimeStr, 'P', 'F' );
                }
                else
                {                    
                    printf( "\nResend PATS filled execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RPF123059\n");
                }
                exit( 0 );
            }
            else if( Param.AnsiPos("-RP") != -1  )
            {
                if( Param.Length() > 3 )
                {
                    UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 3 ) );
                    Resend( TimeStr, 'P', 'A' );
                }
                else
                {                    
                    printf( "\nResend PATS execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RP123059\n");
                }
                exit( 0 );
            }            
            else if( Param.AnsiPos("-RC") != -1  )
            {
                if( Param.Length() > 3 )
                {
                    UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 3) );
                    Resend( TimeStr, 'A', 'C' );
                }
                else
                {                    
                    printf( "\nResend all confirm execution from HHMMSS failed.\n");                                        
                    printf( "Example:KGIExecConnection -RC123059\n");
                }
                exit( 0 );
            }
            else if( Param.AnsiPos("-RF") != -1  )
            {
                if( Param.Length() > 3 )
                {
                    UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 3) );
                    Resend( TimeStr, 'A', 'F' );
                }
                else
                {                    
                    printf( "\nResend all filled execution from HHMMSS failed.\n");                                        
                    printf( "Example:KGIExecConnection -RF123059\n");
                }
                exit( 0 );
            }
            else if( Param.AnsiPos("-R") != -1  )
            {
                if( Param.Length() > 2 )
                {
                    UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 2) );
                    Resend( TimeStr, 'A', 'A' );
                }
                else
                {                    
                    printf( "\nResend all execution from HHMMSS failed.\n");                                        
                    printf( "Example:KGIExecConnection -R123059\n");
                }
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
    char Buffer[16];
    UFC::FileStreamEx File( START_TIME_FILENAME, "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    
    FTime.setCurrent();
    sprintf( Buffer, "%02d%02d%02dAA",FTime.getHour(),FTime.getMinute(),FTime.getSecond() );    
    File.Write( Buffer , 9 );    
}    
//------------------------------------------------------------------------------
BOOL GetRecoverTime( UFC::AnsiString& Time, char& Type, char& EType  )
{
    UFC::FileStreamEx File( START_TIME_FILENAME, "r+" );
    char Buffer[ 16 ];    
    
    if( File.Read( Buffer , 9 ) == 9 )
    {
        Time = UFC::AnsiString( Buffer, 6 ); 
        Type = Buffer[ 6 ];
        EType = Buffer[ 7 ];
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
    
    MessageObj = new MessageObject( "KGIExecConnection", "2.0", "XXXXX" );
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
            char Type,EType;
            
            UFC::BufferedLog::Printf( " ----- Receive Recover Event -----" );    
            if( GetRecoverTime( Time, Type ,EType ) == TRUE )
                ExecConnection.Recover( Time, Type ,EType );        
        }
            
    }    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
