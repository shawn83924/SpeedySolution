// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "FubonDBExecConnection.h"
//------------------------------------------------------------------------------
UFC::PInt32     FRecoverBeginTime;
UFC::UTime      FTime;
BOOL            DumpOnly = FALSE;
//----------------------------------------------------------------------------------------------------------------------
UInt16 COMP_6(const UFC::AnsiString& in,UInt8*& out)
{
    if( in.IsEmpty() )
        return 0;
    UFC::AnsiString data;
    if( in.Length() % 2 != 0 )
        data.Printf("0%s",in.c_str());
    else
        data = in;
    UInt16 halfsize = data.Length() / 2;
    UInt8 data1,data2,tdata;
    out = new UInt8[halfsize];
    for( int i = 0 ; i < halfsize ; i++ )
    {
        data1 = data[i*2] - '0';
        data1 <<= 4;
        data1 &= 0xF0;
        data2 = data[i*2 + 1] - '0';
        tdata = data1 + data2;
        out[i] = tdata;
    }
    return halfsize;
}
/*UInt16 COMP_6(int in,UInt8*& out)
{
    UFC::AnsiString data;
    data.Printf("%d",in);
    return COMP_6_C(data,out);
}
UInt16 COMP_6(long in,UInt8*& out)
{
    UFC::AnsiString data;
    data.Printf("%d",in);
    return COMP_6_C(data,out);
}*/
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
    UFC::AnsiString ShmName( "FubonDBExecConnection.RecoverBeginTime" );
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
    printf( "FubonDBExecConnection Copyright (c) September 2007 MDBS\n" );
    printf( "    -F         First time start up.\n" );
    printf( "    -D         Dump execution to screen only.(Not send to MBus.)\n" );
    printf( "    -R(HHMMSS) Resend execution from HHMMSS.\n" );
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
    UFC::FileStreamEx File( "YECTime", "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    
    File.Write( TimeStr.c_str() , 7 );    
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
            else if( Param.AnsiPos("-R") != -1  )
            {
                 UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 2) );
                 Resend( TimeStr );
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
    
    stat("YECTime", &FileStat );    
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
    UFC::FileStreamEx File( "YECTime", "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    
    FTime.setCurrent();
    sprintf( Time, "%02d%02d%02d",FTime.getHour(),FTime.getMinute(),FTime.getSecond() );    
    File.Write( Time , 7 );    
}    
//------------------------------------------------------------------------------
UFC::AnsiString GetRecoverTime( void )
{
    UFC::FileStreamEx File( "YECTime", "r+" );
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

    FubonDBExecConnection ExecConnection( FRecoverBeginTime.ToInt32() );
    CreateTimeFile( );
    
    while( TRUE )
    {
        UFC::SleepMS( 1000 );
        
        FTime.setCurrent();
        UpdateRecoverBeginTime();        
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
        if( CheckConfigModifiaction() == TRUE )
            ExecConnection.Recover( GetRecoverTime());
        
    }
    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
