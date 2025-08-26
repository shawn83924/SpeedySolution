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
bool            gRMIsRecoverMode = true;
UFC::UTime      FTime;
UFC::AnsiString gFullProgramName = "GCRiskManager";
UFC::AnsiString gProgramName = "GCRiskManager";
UFC::AnsiString gApplicationNamePrefix = "GC";
UFC::AnsiString gApplicationMajorName = "RiskManager";
UFC::AnsiString gAppName = "GCRiskManager";
UFC::AnsiString FRecoverTimeFileName = "GCRM";

UFC::AnsiString gFuturesP14FilePathName = "../ftFUT/P14.20";
UFC::AnsiString gOptionsP14FilePathName = "../ftOPT/P14.10";
UFC::AnsiString gDayFuturesP14FilePathName = "../ftFUT/P14Day.20";
UFC::AnsiString gDayOptionsP14FilePathName = "../ftOPT/P14Day.10";

UFC::BufferedLog* gUniRMLogPtr = 0;
    
//------------------------------------------------------------------------------
UFC::AnsiString ParseProgramName(const UFC::AnsiString& FullProgramName)
{
    UFC::AnsiString programName = "";
    int fullNameLength = FullProgramName.Length();
    if (fullNameLength <= 0) return programName;
    int startIndex = 0;
    for (int i = fullNameLength - 1; i >= 0; i--)
    {
        if (FullProgramName[i] == '/') 
        {
            startIndex = i + 1;
            break;
        }
    }
    
    if (startIndex >= fullNameLength) return programName;
    programName = FullProgramName.SubString(startIndex, fullNameLength - startIndex);
    return programName;
}  //ParseProgramName()
//------------------------------------------------------------------------------
void Resend( const UFC::AnsiString& TimeStr )
{
    Int32 Time = TimeStr.ToInt();
    if( Time == 0 )
    {
        printf( " Wrong time string.\n" );
        return;
    }
    if( (Time / 10000 ) > 23 )
    {
        printf( " Hour should between 0 to 23.\n" );
        return;
    }
    if( ( ( Time % 10000 ) / 100 ) > 59 )
    {
        printf( " Minutes should between 0 to 59.\n" );
        return;
    }    
    UFC::FileStreamEx File( FRecoverTimeFileName, "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    
    File.Write( TimeStr.c_str(), 7 );    
}    
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "%s Copyright (c) May 2015 MDBS, Build Date:%s %s\n", gProgramName.c_str(), __DATE__, __TIME__ );
    printf( "    -N(Application Name Prefix) Application Name Prefix.\n" );
    printf( "    -F                          First time start up.\n" );
    printf( "    -D                          Dump execution to screen only.\n" );
    printf( "    -R(HHMMSS)                  Recover executions from the given time.\n" );

/*
GCRMConnection::LoadP09File("../ftOPT/P09.10", false);
GCRMConnection::LoadP09File("../ftFUT/P09.20", true);
GCRMConnection::LoadP14File("../ftFUT/P14.20");
GCRMConnection::LoadP14File("../ftOPT/P14.10");
GCRMConnection::DumpCommodity();
UFC::BufferedLog::Printf(" ");
UFC::BufferedLog::Printf(" ");
UFC::BufferedLog::Printf(" ");
UFC::BufferedLog::Printf(" ");
UFC::BufferedLog::Printf(" ");

GCRMConnection::LoadP08File("../cfg/Futures_P08.ini");
GCRMConnection::LoadP08File("../cfg/Options_P08.ini");
GCRMConnection::DumpContract();
UFC::BufferedLog::Printf(" ");
UFC::BufferedLog::Printf(" ");
UFC::BufferedLog::Printf(" ");
UFC::BufferedLog::Printf(" ");
UFC::BufferedLog::Printf(" ");

GCRMConnection::BuildNearMonthIndex();
GCRMConnection::DumpNearMonthContract();
*/
    exit( 1 );
}
//------------------------------------------------------------------------------
void CheckArgs( int argc, char *argv[] )
{
    gFullProgramName = argv[0];
    gProgramName = ParseProgramName(gFullProgramName);
    gAppName = gProgramName;
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
            {
                RecoverMode = FALSE;
                gRMIsRecoverMode = false;
            }
            else if( ( Param.SubString( 0, 2 ) == "-N" ) || ( Param.SubString( 0, 2 ) == "-n" ) )
                gApplicationNamePrefix = Param.SubString( 2, Param.Length() - 2 );
        }
    }
    catch( UFC::Exception& ex )
    {
        printf( "%s", ex.what() );
        PrintHelp();
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
    UFC::AnsiString ShmName( gAppName );
    FRecoverBeginTime.Attach( ShmName );
    
    int t = FRecoverBeginTime.ToInt32();
    FTime.setHour( t / 10000 );
    FTime.setMinute( (t % 10000) / 100 );
    FTime.setSecond( t % 100 );
    FTime -= 60;
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
    sprintf( Time, "%02d%02d%02d",FTime.getHour(), FTime.getMinute(), FTime.getSecond() );    
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
    if (gApplicationNamePrefix.Length() > 0)
    {
        gAppName.Printf("%s%s", gApplicationNamePrefix.c_str(), gApplicationMajorName.c_str());
        FRecoverTimeFileName.Printf("%sRM", gApplicationNamePrefix.c_str());
    }
    

    UFC::AnsiString logName;
    UFC::UDateTime now;
    logName.Printf( "../log/%s.%04d%02d%02d.log", gAppName.c_str(), now.getYear(), now.getMonth(), now.getDate() );
    gUniRMLogPtr = new UFC::BufferedLog( logName, 1024, TRUE );
    UFC::BufferedLog::SetLogObject( gUniRMLogPtr );
    UFC::BufferedLog::Printf( " LogName:[%s]", logName.c_str() );
    
    UFC::BufferedLog::SetDebugMode( DumpOnly );
    int Time = 80000;
    if( RecoverMode == TRUE )
        Time = GetRecoverTime().ToInt();    
            
    GCRMConnection::LoadP09File("../ftOPT/P09.10", false);
    GCRMConnection::LoadP09File("../ftFUT/P09.20", true);
   if (gRMIsRecoverMode)  //not FirstTime
    {
        if (UFC::FileExists(gDayFuturesP14FilePathName) == TRUE)
            GCRMConnection::LoadP14File(gDayFuturesP14FilePathName, false, gDayFuturesP14FilePathName);  //need not copy
        if (UFC::FileExists(gDayOptionsP14FilePathName) == TRUE)
            GCRMConnection::LoadP14File(gDayOptionsP14FilePathName, false, gDayOptionsP14FilePathName);  //need not copy
    }
    GCRMConnection::LoadP14File(gFuturesP14FilePathName, !gRMIsRecoverMode, gDayFuturesP14FilePathName);
    GCRMConnection::LoadP14File(gOptionsP14FilePathName, !gRMIsRecoverMode, gDayOptionsP14FilePathName);
    
    GCRMConnection::LoadP14File("../ftFUT/P14.20");
    GCRMConnection::LoadP14File("../ftOPT/P14.10");
    GCRMConnection::DumpCommodity();
    
    GCRMConnection::LoadP08File("../cfg/Options_P08.ini");
    GCRMConnection::LoadP08File("../cfg/Futures_P08.ini");
    GCRMConnection::DumpContract();
    
    GCRMConnection::BuildNearMonthIndex();
    GCRMConnection::DumpNearMonthContract();
    
    GCRMConnection GCRM( Time );

    UFC::AnsiString clearAmountFileName = GCRMConnection::FClearAmountFileName;
    long long clearAmountFileSecond = 0LL;
    bool isClearFileExist = false;
    if (clearAmountFileName.Length() > 0)
        isClearFileExist = UFC::GetFileModificationSecond(clearAmountFileName, clearAmountFileSecond);
    
    int loopCount = 0;
    CreateTimeFile( );    
    while( TRUE )
    {
        UFC::SleepMS( 1000 );
        
        if ((clearAmountFileName.Length() > 0) && (loopCount % 3 == 0))
        {
            long long newClearAmountFileSecond = 0LL;
            isClearFileExist = UFC::GetFileModificationSecond(clearAmountFileName, newClearAmountFileSecond);
            if ((isClearFileExist) && (newClearAmountFileSecond > clearAmountFileSecond))
            {
                UFC::SleepMS(100);
                if (GCRM.ClearOrderAmount(clearAmountFileName))
                {
                    clearAmountFileSecond = newClearAmountFileSecond;
                }
            }
        }
        
        if ( GCRMConnection::FCheckLogPtr != 0 ) GCRMConnection::FCheckLogPtr->Flush();
        
        FTime.setCurrent();
        UpdateRecoverBeginTime();        
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
        
        if( loopCount % 20 == 0 ) ///< Check connection every 20 sec         
            GCRM.CheckConnection();
        
        if( CheckConfigModifiaction() == TRUE )
            GCRM.Recover( GetRecoverTime() );
        
        loopCount++;
    }
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
