// 
// File:   Main.cpp
// Author: yuan
//
// Created on November 2, 2008, 11:08 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "MLExecConnection.h"
//------------------------------------------------------------------------------
UFC::AnsiString   AppName = "";
UFC::AnsiString   ConfigName = "";
UFC::AnsiString   YECTime;
UFC::AnsiString   LogName;
BOOL              DumpOnly = FALSE;
BOOL              IsFirstStart = FALSE;
Int32             FOverlapTime = 30;
UFC::PInt32       FRecoverBeginTime;
UFC::PInt32       FOverseasSeqNo;
UFC::UTime        FTime;
MLExecConnection* ExecConnection;
MessageObject*    MessageObj;
UFC::PEvent*      MBusConnectedEvent;
ConnectionType    FExecType = ctBoth;
ExecutionMarket   FExecMarket = emAll;
//------------------------------------------------------------------------------
void LoadAppName( const UFC::AnsiString& FilePath )
{
    UFC::UiniFile Config( FilePath );
    if( Config.GetValue( "Setting", "AppName", AppName ) == FALSE)
    {
        UFC::BufferedLog::Printf( " AppName not set." );
        exit( 0 );
    }
    else
        UFC::BufferedLog::Printf( " AppName:[%s]", AppName.c_str() );
}
//------------------------------------------------------------------------------
void WriteUserlog(MTree *Data)
{
    UFC::AnsiString Account, Type, Msg;

    if (Data->get("ID", Account) && Data->get("TYPE", Type) && Data->get("MSG", Msg))
    {
        UFC::BufferedLog::Printf(",%s,%s,%s", Account.c_str(), Type.c_str(), Msg.c_str());
    }
}
//------------------------------------------------------------------------------
void GetReconnectBeginTime(UFC::AnsiString& DisconnectTime)
{
    if (DisconnectTime.Length() != 6)
    {
        UFC::UTime time;
        time.setCurrent();
        time -= FOverlapTime;
        DisconnectTime.Printf("%02d%02d%02d", time.getHour(), time.getMinute(), time.getSecond());
    }
}
//------------------------------------------------------------------------------
Int32 TimeToInt(void)
{
    return ( (FTime.getHour() * 10000) + (FTime.getMinute() * 100) + FTime.getSecond());
}
//------------------------------------------------------------------------------
void UpdateRecoverBeginTime(void)
{
    FTime.setCurrent();
    FRecoverBeginTime = TimeToInt();
}
//------------------------------------------------------------------------------
void GetRecoverBeginTime(void)
{
    FTime.setCurrent();
    UFC::AnsiString ShmName;
    ShmName.Printf("%s.RecoverBeginTime", AppName.c_str());
    FRecoverBeginTime.Attach(ShmName);
    int t = FRecoverBeginTime.ToInt32();
    int Now = TimeToInt();

    if (Now < t) ///< Over one day.
    {
        FRecoverBeginTime = 0;
    }
    else
    {
        FTime.setHour(t / 10000);
        FTime.setMinute((t % 10000) / 100);
        FTime.setSecond(t % 100);
        FTime -= FOverlapTime;
        FRecoverBeginTime = TimeToInt();
    }
}
//------------------------------------------------------------------------------
void GetOverseasSequenceNo(void)
{
    UFC::AnsiString sharedMemoryName;
    
    sharedMemoryName.Printf( "MLExec.OverseasSeqNo.%s", AppName.c_str() );
    FOverseasSeqNo.Attach( sharedMemoryName );
    if( IsFirstStart == TRUE ) 
        FOverseasSeqNo = 0;    
    UFC::BufferedLog::Printf(" Overseas Sequence No Start at[%d].", FOverseasSeqNo.ToInt32() );
}  //GetOverseasSequenceNo() 
//------------------------------------------------------------------------------
void PrintHelp(void)
{
    printf(" MLExecConnection Copyright (c)2011~%d MDBS\n",UFC::ThisYear());
    printf(" Send Speedy execution report to backend program for MasterLink.\n");    
    printf(" Support TAIFEX.\n");        
    printf(" Build Date:%s %s\n", __DATE__, __TIME__);
    printf("    -F                     First time start up.\n");
    printf("    -D                     Dump execution to screen only.(Not send to MBus.)\n");
    printf("    -C(Configuration File) Config file name.(Required)\n" );
    printf("    -R(HHMMSS)             Resend execution from HHMMSS.(for TAIFEX, TSE,OTC)\n");
    printf("      (HHMMSS,SID,Seq)     Resend Session SID executions from HHMMSS to now and rpt_seq > Seq.\n");
    printf("                           ( for TAIFEX only)\n");    
    printf("    -OnlyConfirm           Receive order confirm execution only.\n" );      
    printf("    -OnlyFill              Receive order filled execution only.\n" );
    printf("    -OnlyTAIFEX            Receive TAIFEX executions only.\n" );
    printf("    -OnlyTSE               Receive TWSE executions only.\n" );
    printf("    -OnlyOTC               Receive OTC executions only.\n" );
    printf("    -OnlyPATS              Receive PATS executions only.\n" );
    exit(1);
}
//------------------------------------------------------------------------------
void Resend( const UFC::AnsiString& TimeStr )
{	
    Int32 Time;
    Int32 SID = 0;
    Int32 Rpt_Seq = 0;
    Int32 Rpt_Seq_End = 0;
    UFC::PStringList Strings;

    Strings.SetStrings( TimeStr, ",|\n" );
    Time = Strings[0].ToInt();

    if( Strings.ItemCount() >= 2 )
        SID = Strings[1].ToInt();
    
    if( Strings.ItemCount() >= 3 )
        Rpt_Seq = Strings[2].ToInt();

    if( Strings.ItemCount() >= 4 )
        Rpt_Seq_End = Strings[3].ToInt();

    if (Time == 0)
    {
        printf(" Wrong time string.\n");
        return;
    }
    if ((Time / 10000) > 23)
    {
        printf(" Hour should between 0 to 23.\n");
        return;
    }
    if (((Time % 10000) / 100) > 59)
    {
        printf(" Minutes should between 0 to 59.\n");
        return;
    }
    if( YECTime.Length() == 0 )
    {
        printf(" -C(Config file) must assign.\n");
        return;
    }
    printf(" Ask existing MLExecConnection to recover executions\n");
    printf(" Write to file[%s]\n", YECTime.c_str() );
    if( SID > 0 )
        printf(" Session:%d\n", SID );
    else
        printf(" All sessions.\n" );
    printf(" From Time:%06d\n", Time );
    printf(" From rpt_seq %d", Rpt_Seq );
    if( Rpt_Seq_End > Rpt_Seq )
        printf(" to %d\n", Rpt_Seq_End );
    else
        printf("\n" );
    UFC::FileStreamEx File( YECTime.c_str(), "w+"); // "w+" = O_CREAT|O_TRUNC|O_RDWR
    UFC::AnsiString   RecoverStr;

    RecoverStr.Printf("%06d%03d%010d%010d", Time, SID, Rpt_Seq, Rpt_Seq_End );
    File.Write( RecoverStr.c_str(), 30 );
}
//------------------------------------------------------------------------------
BOOL CheckConfigModifiaction(void)
{
    static time_t ModifyTime = 0;
    struct stat FileStat;

    stat(YECTime.c_str(), &FileStat);
    if (ModifyTime == 0)
    {
        ModifyTime = FileStat.st_mtime;
        return FALSE;
    }
    else
    {
        if (ModifyTime != FileStat.st_mtime)
        {
            ModifyTime = FileStat.st_mtime;
            return TRUE;
        }
        return FALSE;
    }
}
//------------------------------------------------------------------------------
void CreateTimeFile(void)
{
    char Time[64];
    UFC::FileStreamEx File(YECTime.c_str(), "w+"); // "w+" = O_CREAT|O_TRUNC|O_RDWR

    FTime.setCurrent();
    sprintf(Time, "%02d%02d%02d%03d%010d%010d", FTime.getHour(), FTime.getMinute(), FTime.getSecond(),0,0,0);
    File.Write(Time, 30 );
}
//------------------------------------------------------------------------------
void GetRecoverTime( UFC::AnsiString& Time, Int32&  SID, Int32& Seq, Int32& EndSeq )
{
    char Params[ 64 ];
    UFC::FileStreamEx File(YECTime.c_str(), "r+");
    
    File.Read( Params, 30 );
    Time    = UFC::AnsiString( Params, 6 );
    SID     = UFC::AnsiString( Params + 6, 3 ).ToInt();
    Seq     = UFC::AnsiString( Params + 9, 10 ).ToInt();
    EndSeq  = UFC::AnsiString( Params + 19, 10 ).ToInt();
}
//------------------------------------------------------------------------------
void CheckArgs(int argc, char *argv[])
{    
    try
    {
        for (int i = 1; i < argc; i++)
        {
            UFC::AnsiString Param(argv[i]);
            if (Param == "-H" || Param == "-h")
            {
                PrintHelp();
                exit(0);
            }
            else if (Param.AnsiPos("-R") != -1)
            {
            	if( Param.Length() > 3 )
            	{
                    UFC::AnsiString TimeStr((const char*) (Param.c_str() + 2));
                    Resend(TimeStr);
                }
                else
                    UFC::BufferedLog::Printf(" Invalid param:%s.", Param.c_str() );
                exit(0);
            }
            else if( Param.AnsiPos("-C") != -1)
            {
                if( Param.Length() > 3 )
                {
                    ConfigName.Printf( "../cfg/%s", Param.SubString( 2, Param.Length() - 2).c_str() );
                    UFC::BufferedLog::Printf( " Config:[%s]", ConfigName.c_str() );
                    LoadAppName( ConfigName );
                    YECTime.Printf("YECTime.%s", AppName.c_str());
                    GetRecoverBeginTime();
                }
                else
                {
                    UFC::BufferedLog::Printf(" Invalid config file." );
                    exit( 0 );
                }
            }
            else if (Param == "-F" || Param == "-f")
            {
                FRecoverBeginTime = 0;
                IsFirstStart = TRUE;
            }
            else if (Param == "-D" || Param == "-d")
            {
                DumpOnly = TRUE;
            }
            else if (Param == "-OnlyConfirm" ) 
            {
                FExecType = ctRecvComfirmOnly;
            }
            else if (Param == "-OnlyFill" ) 
            {
                FExecType = ctRecvFillOnly;
            }
            else if (Param == "-OnlyTSE" ) 
            {
                FExecMarket = emTSE;
            }
            else if (Param == "-OnlyOTC" ) 
            {
                FExecMarket = emOTC;
            }
            else if (Param == "-OnlyPATS" ) 
            {
                FExecMarket = emPATS;
            }
            else if (Param == "-OnlyTAIFEX" ) 
            {
                FExecMarket = emTAIFEX;
            }
        }
        if( ConfigName.Length() < 7 )
        {
            UFC::BufferedLog::Printf( " Config file not set." );
            exit( 0 );
        }
        if (FRecoverBeginTime != 0)
        {
            try
            {
                UFC::UiniFile Config(ConfigName);
                UFC::AnsiString Value;
                if (Config.GetValue("Setting", "OverlapTime", Value) == FALSE)
                {

                }
                else
                {
                    FOverlapTime = Value.ToInt();
                    if( FOverlapTime < 0 )
                        FOverlapTime = 0;
                }
                if (Config.GetValue("Setting", "SendFromFirst", Value) == FALSE)
                {

                }
                else if (Value.ToInt() != 0)
                {
                    FRecoverBeginTime = 0;
                    Config.SetValue("Setting", "SendFromFirst", "0");
                    Config.Save();
                }
            }
            catch (...)
            {
                UFC::BufferedLog::Printf(" %s not found.", ConfigName.c_str());
                exit(1);
            }
        }
    }
    catch (UFC::Exception& ex)
    {
        printf("%s", ex.what());
        PrintHelp();
    }
}
//------------------------------------------------------------------------------
void makeCheckWord(UFC::AnsiString &result)
{
    UInt16 data = 0;
    for( UInt16 i = 0 ; i < result.Length() ; i++ )
    {
        data += result[i];
    }
    UInt8 byte1 = data / 256;
    if( byte1 == 0 )
        byte1 = 20;
    UInt8 byte2 = data - (byte1*256);
    if( byte1 == 0 )
        byte1 = 20;
    UFC::AnsiString resultTmp(result.c_str());
    result.Printf("%s%c%c",resultTmp.c_str(),byte1,byte2);
}
//------------------------------------------------------------------------------
//
// main
//
//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    
    CheckArgs(argc, argv);
    
    MBusConnectedEvent = new UFC::PEvent();
    LogName.Printf("../log/%s_%s.log", AppName.c_str(), UFC::GetDateString().c_str());
    UFC::BufferedLog::SetDebugMode( TRUE );
    UFC::BufferedLog* LogObject = new UFC::BufferedLog(LogName, 1024, TRUE);
    UFC::BufferedLog::SetLogObject( LogObject );
    
    GetOverseasSequenceNo();    
    
    ExecConnection = new MLExecConnection;
    CreateTimeFile();
    ExecConnection->ConnectToBeckend();
    while (TRUE)
    {
        UFC::SleepMS( 1000 );
        UpdateRecoverBeginTime();
        ExecConnection->Update();
        if (CheckConfigModifiaction() == TRUE)
        {
            UFC::AnsiString Time;
            Int32 SID,Seq,EndSeq;

            GetRecoverTime( Time, SID, Seq, EndSeq  );
            ExecConnection->Recover( Time, SID, Seq, EndSeq );
        }
        UFC::BufferedLog::FlushToFile();
    }
    delete ExecConnection;    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
