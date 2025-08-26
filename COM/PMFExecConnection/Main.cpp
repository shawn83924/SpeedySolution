//
// File:   Main.cpp
// Author: zhen
//
// Created on April 10, 2009, 11:45 AM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "MessageStore.h"
#include "PMFExecConnection.h"
#include "TandemAdapter.h"

#include <stdio.h>
//------------------------------------------------------------------------------
UFC::PInt32     FRecoverBeginTime;
UFC::UTime      FTime;
BOOL            DumpOnly = FALSE;
BOOL            MatchOnly = FALSE;
BOOL            FirstTimeStart = FALSE;
BOOL            ResendTimeFlag = FALSE;
UFC::AnsiString ApplicationName;
UFC::AnsiString ConfigFileName;
int             DataLength;
UFC::AnsiString TandemFutureIP;
UInt32          TandemFuturePort;
UFC::AnsiString TandemOptionIP;
UInt32          TandemOptionPort;
UFC::AnsiString ResendTimeStr;
UFC::AnsiString ResendFileName;

TMessageStore*  OptMessageStore;
TMessageStore*  FutMessageStore;
MessageObject*  MessageObj;
UFC::UiniFile*  pConfig;

//----------------------------------------------------------------------------------------------------------------------
void ProcessMonitor::OnProcessConnected( BOOL IsTheFirstOne )
{
    if( IsTheFirstOne == FALSE )
    {
        UFC::BufferedLog::Printf( " ***** Process SpeedyManager already exists *****"  );
        sleep( 2 );
        exit( 0 );
    }
}  //OnProcessConnected

//----------------------------------------------------------------------------------------------------------------------
void WriteUserlog( MTree *Data )
{
    UFC::AnsiString Account,Type,Msg;

    if( Data->get( "ID", Account ) && Data->get( "TYPE", Type ) && Data->get( "MSG", Msg ))
    {
        UFC::BufferedLog::Printf(",%s,%s,%s", Account.c_str(),Type.c_str(),Msg.c_str());
    }
}  //WriteUserlog

//------------------------------------------------------------------------------
void UpdateRecoverBeginTime( void )
{
    FTime.setCurrent();
    FRecoverBeginTime = ( (FTime.getHour() * 10000) +
                          (FTime.getMinute() * 100) +
                          (FTime.getSecond()) );
}  //UpdateRecoverBeginTime

//------------------------------------------------------------------------------
void GetRecoverBeginTime( void )
{
    UFC::AnsiString ShmName;
    ShmName.Printf("%s.RecoverBeginTime", ApplicationName.c_str());
    
    FRecoverBeginTime.Attach(ShmName);

    int t = FRecoverBeginTime.ToInt32();
    FTime.setHour(t / 10000);
    FTime.setMinute((t % 10000) / 100);
    FTime.setSecond(t % 100);
    FTime -= 60;
}  //GetRecoverBeginTime

//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "PMFExecConnection Copyright (c) September 2007 MDBS\n" );
    printf( "    -F         First time start up.\n" );
    printf( "    -D         Dump execution to screen only.(Not send to MBus.)\n" );
    printf( "    -M         Match Only.\n" );
    printf( "    -R(HHMMSS) Resend execution from HHMMSS.\n" );
    exit( 1 );
}  //PrintHelp

//------------------------------------------------------------------------------
void Resend( const UFC::AnsiString& TimeStr )
{
    Int32 Time = TimeStr.ToInt();
    if( Time == 0 )
    {
        printf( " Wrong time string.\n" );
        return;
    }
    
    if( (Time / 10000) > 23 )
    {
        printf( " Hour should between 0 to 23.\n" );
        return;
    }
    
    if( ((Time % 10000) / 100) > 59 )
    {
        printf( " Minutes should between 0 to 59.\n" );
        return;
    }

//    UFC::FileStreamEx File( "YECTime", "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    UFC::FileStreamEx File(ResendFileName, "w+");	// "w+" = O_CREAT|O_TRUNC|O_RDWR

    File.Write( TimeStr.c_str() , 7 );
}  //Resend

//------------------------------------------------------------------------------
void CheckArgs(int argc, char *argv[])
{
    try
    {
        for (int i = 1; i < argc; i++)
        {
            UFC::AnsiString Param(argv[i]);
            if ((Param == "-H") || ((Param == "-h")))
            {
                PrintHelp();
                exit( 0 );
            }
            else if ((Param.AnsiPos("-R") != -1) || (Param.AnsiPos("-r") != -1))
            {
                ResendTimeStr.Printf("%s", (const char*)(Param.c_str() + 2));
                ResendTimeFlag = TRUE;
//                UFC::AnsiString TimeStr((const char*)(Param.c_str() + 2));
//                Resend(TimeStr);
//                exit(0);
            }
            else if ((Param == "-F") || (Param == "-f"))
            {
                FirstTimeStart = TRUE;
            }
            else if ((Param == "-D") || (Param == "-d"))
            {
                DumpOnly = TRUE;
            }
            else if ((Param == "-M") || (Param == "-m"))
            {
                MatchOnly = TRUE;
            }
//            else if ((Param == "-C") || (Param == "-c"))
//            {
//                i++;
//                if (i < argc)
//                    ConfigFileName = argv[i];
//                else
//                    throw(UFC::Exception("arg error\n"));
//            }
        }  //for (int i = 1; i < argc; i++)
    }
    catch( UFC::Exception& ex )
    {
        printf( ex.what() );
        PrintHelp();
    }
}  //CheckArgs

//--------------------------------------------------------------------------------------------------------------------
BOOL CheckConfigModifiaction( void )
{
    static time_t ModifyTime = 0;
    struct stat FileStat;

    stat(ResendFileName.c_str(), &FileStat);
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
}  //CheckConfigModifiaction

//------------------------------------------------------------------------------
void CreateTimeFile( void )
{
    char Time[7];
    UFC::FileStreamEx File(ResendFileName, "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR

    FTime.setCurrent();
    sprintf( Time, "%02d%02d%02d", FTime.getHour(), FTime.getMinute(), FTime.getSecond() );
    File.Write( Time , 7 );
}  //CreateTimeFile

//------------------------------------------------------------------------------
UFC::AnsiString GetRecoverTime( void )
{
    UFC::FileStreamEx File( ResendFileName, "r+" );
    char Time[7];

    File.Read( Time , 7 );
    return UFC::AnsiString( Time );
}  //GetRecoverTime

//------------------------------------------------------------------------------
void LoadSetting() {
    pConfig = new UFC::UiniFile(ConfigFileName);

    if (pConfig->GetValue("Setting", "AppName", ApplicationName) == FALSE)
        ApplicationName.Printf("PMFExecConnection");

    GetRecoverBeginTime();
    if (FirstTimeStart == TRUE) {
        FRecoverBeginTime = 0;
    }

    UFC::AnsiString Value;
    if (pConfig->GetValue("Setting", "DataLength", Value) == TRUE)
        DataLength = Value.ToInt();

    if (pConfig->GetValue("Tandem", "OptionIP", TandemOptionIP) == FALSE)
        TandemOptionIP.Printf("127.0.0.1");
    if (pConfig->GetValue("Tandem", "OptionPort", Value) == TRUE)
        TandemOptionPort = Value.ToInt();

    if (pConfig->GetValue("Tandem", "FutureIP", TandemFutureIP) == FALSE)
        TandemFutureIP.Printf("127.0.0.1");
    if (pConfig->GetValue("Tandem", "FuturePort", Value) == TRUE)
        TandemFuturePort = Value.ToInt();
}  //LoadSetting

//------------------------------------------------------------------------------
//
// main
//
//------------------------------------------------------------------------------
int main(int argc, char** argv) {
    ApplicationName.Printf("PMFExecConnection");
    ConfigFileName.Printf("../cfg/PMFExecConnection.cfg");
    DataLength = 400;
    TandemFutureIP.Printf("127.0.0.1");
    TandemFuturePort = 15801;
    TandemOptionIP.Printf("127.0.0.1");
    TandemOptionPort = 15801;
    
    CheckArgs(argc, argv);
    LoadSetting();

    ResendFileName.Printf("%sTime", ApplicationName.c_str());
    if (ResendTimeFlag == TRUE) {
        Resend(ResendTimeStr);
        exit(0);
    }

    MessageObj = new MessageObject(ApplicationName, "1.0", "XXXXX");
    MessageObj->SetMonitorListener(new ProcessMonitor());
    MessageObj->Start(); ///< Start Migo Message pump.
    MessageObj->WaitForConnected();

    BOOL NeedToRecoverStore = TRUE;
    if (FirstTimeStart == TRUE) NeedToRecoverStore = FALSE;
    UFC::AnsiString FutRecoverFileName;
    FutRecoverFileName.Printf("%sFut.DATA", ApplicationName.c_str());
    FutMessageStore = new TMessageStore(FutRecoverFileName, DataLength, NeedToRecoverStore);
    FutMessageStore->Start();

    UFC::AnsiString OptRecoverFileName;
    OptRecoverFileName.Printf("%sOpt.DATA", ApplicationName.c_str());
    OptMessageStore = new TMessageStore(OptRecoverFileName, DataLength, NeedToRecoverStore);
    OptMessageStore->Start();
    
    PMFExecConnection ExecConnection(FRecoverBeginTime.ToInt32());
    CreateTimeFile( );

    TandemAdapter* FutTandemAdapter = NULL;
    TandemAdapter* OptTandemAdapter = NULL;

    if (DumpOnly == FALSE) {
        FutTandemAdapter = new TandemAdapter(FALSE, TandemFutureIP, TandemFuturePort, FutMessageStore);
        OptTandemAdapter = new TandemAdapter(FALSE, TandemOptionIP, TandemOptionPort, OptMessageStore);
    }

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
}  //main
