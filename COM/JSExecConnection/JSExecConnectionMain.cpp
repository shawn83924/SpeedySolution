/* 
 * File:   JSExecConnectionMain.cpp
 * Author: Zhen Fan
 *
 * Created on 2012/06/22 16:29
 */

#include <cstdio>
#include <cstdlib>

#include "UFC.h"
#include "iniFile.h"
#include "PInt32.h"
#include "OrderConnection/TTaifexConnection.h"

#include "TandemObjects.h"
#include "MBusAdapter.h"
#include "JihSunEexcutionConnection.h"
#include "TandemAdapter.h"


UFC::UDateTime  G_ApplicationStartTime;
UFC::AnsiString YECTime = "";
UFC::UTime      FTime;
UFC::PInt32     G_IRecoverBeginTime = 0;
Int32           G_OverlapTime = 30;
UFC::AnsiString G_LogDirectory = "/Speedy/log";
UFC::AnsiString G_LogFileNamePrefix = "JSExecConnection_";
UFC::AnsiString G_LogFileName = "";
UFC::AnsiString G_ConfigDirectory = "../cfg";
UFC::AnsiString G_ConfigFileName = "";
UFC::AnsiString G_ResendData = "";
bool            G_DoResend = false;         
BOOL            G_IsDumpOnly = FALSE;
BOOL            G_IsFirstTimeStart = FALSE;
RecoverMarket   G_ReceiveMarket = rmAll;
ConnectionType  G_ExecutionType = ctBoth;
Int32           G_DebugLevel = 0;

UFC::AnsiString G_AppName                 = "";
UFC::AnsiString G_TandemIP                = "localhost";
Int32           G_TandemPort              = 11120;
Int32           G_TandemHeartbeatInterval = 30000; ///< 30 sec
Int32           G_TandemCheckInterval     = 1000;  ///< 1 sec

//------------------------------------------------------------------------------
Int32 TimeToInt(UFC::UTime& TargetTime)
{
    return ((TargetTime.getHour() * 10000) + (TargetTime.getMinute() * 100) + TargetTime.getSecond());
}  //TimeToInt()
//------------------------------------------------------------------------------
void UpdateRecoverBeginTime(void)
{
    UFC::UTime newRecoverBeginTime;
    G_IRecoverBeginTime = TimeToInt(newRecoverBeginTime);
}  //UpdateRecoverBeginTime(
//------------------------------------------------------------------------------
void GetRecoverBeginTime(void)
{
    UFC::UTime newRecoverBeginTime;
    UFC::AnsiString shareMemoryName;
    shareMemoryName.Printf("%s.RecoverBeginTime", G_AppName.c_str());
    G_IRecoverBeginTime.Attach(shareMemoryName);
    int iRecoverBeginTime = G_IRecoverBeginTime.ToInt32();
    int iNewRecoverBeginTime = TimeToInt(newRecoverBeginTime);
    if (iNewRecoverBeginTime < iRecoverBeginTime) // Over one day.
        G_IRecoverBeginTime = 0;
    else
    {
        newRecoverBeginTime.setHour(iRecoverBeginTime / 10000);
        newRecoverBeginTime.setMinute((iRecoverBeginTime % 10000) / 100);
        newRecoverBeginTime.setSecond(iRecoverBeginTime % 100);
        newRecoverBeginTime -= G_OverlapTime;
        G_IRecoverBeginTime = TimeToInt(newRecoverBeginTime);
    }
}  //GetRecoverBeginTime()
//------------------------------------------------------------------------------
void PrintHelp(void)
{
    std::printf(" JSExecConnection Copyright (c)%d MDBS\n", UFC::ThisYear() );
    std::printf(" Send Speedy execution report to backend program for Jih Sun.\n");    
    std::printf(" Support TAIFEX, TSE, OTC.\n");        
    std::printf(" Build Date:%s %s\n", __DATE__, __TIME__);
    std::printf("    -F                     First time start up.\n");
    std::printf("    -D                     Dump execution to screen only.(Not send to MBus.)\n");
    std::printf("    -C(Configuration File) Config file name.(Required)\n" );
    std::printf("    -R(HHMMSS)             Resend execution from HHMMSS.(for TAIFEX, TSE,OTC)\n");
    std::printf("    -OnlyTwFutures         Receive Taiwan Futures execution only.\n" );      
    std::printf("    -OnlyTwOptions         Receive Taiwan Options execution only.\n" );      
    std::printf("    -OnlyConfirm           Receive order confirm execution only.\n" );      
    std::printf("    -OnlyFill              Receive order filled execution only.\n" );
    exit(1);
}  //PrintHelp()
//------------------------------------------------------------------------------
void CheckArgs(int argc, char *argv[])
{    
    try
    {
        UFC::AnsiString configFileName = "JSExecConnection.cfg";
        for (int i = 1; i < argc; i++)
        {
            UFC::AnsiString param(argv[i]);
            if ((param == "-H") || (param == "-h"))
                PrintHelp();
            else if (param.AnsiPos("-R") != -1)
            {
            	if (param.Length() > 3)
            	{
                    G_ResendData = (const char*)(param.c_str() + 2);
                    G_DoResend = true;
                }
                else
                    UFC::BufferedLog::Printf(" Invalid param:%s.", param.c_str());
            }
            else if (param.AnsiPos("-C") != -1)
            {
                if (param.Length() > 3)
                    configFileName = param.SubString(2, param.Length() - 2).c_str();
                else
                {
                    UFC::BufferedLog::Printf(" Invalid config file.");
                    exit( 0 );
                }
            }
            else if ((param == "-F") || (param == "-f"))
                G_IsFirstTimeStart = TRUE;
            else if ((param == "-D" ) || (param == "-d"))
                G_IsDumpOnly = TRUE;
            else if (param == "-OnlyTwFutures") 
            {
                G_ReceiveMarket = rmFutures;
            }
            else if (param == "-OnlyTwOptions") 
            {
                G_ReceiveMarket = rmOption;
            }
            else if (param == "-OnlyConfirm") 
            {
                G_ExecutionType = ctRecvComfirmOnly;
            }
            else if (param == "-OnlyFill") 
            {
                G_ExecutionType = ctRecvFillOnly;
            }
            else if ((param.AnsiPos("-d") != -1) || (param.AnsiPos("-D") != -1 ))
            {
                if (param.AnsiPos("f") != -1) G_DebugLevel |= UFC::dlFatalError;
                if (param.AnsiPos("i") != -1) G_DebugLevel |= UFC::dlInformation;
                if (param.AnsiPos("p") != -1) G_DebugLevel |= UFC::dlPerformance;
                if (param.AnsiPos("w") != -1) G_DebugLevel |= UFC::dlWarning;
            }
        }  //for ( int i = 1; i < argc; i++ )
        G_ConfigFileName.Printf("%s/%s", G_ConfigDirectory.c_str(), configFileName.c_str());
        UFC::BufferedLog::Printf(" Config File Name:[%s]", G_ConfigFileName.c_str());
        if (G_ConfigFileName.Length() < 7)
        {
            UFC::BufferedLog::Printf(" Config file not set.(FileName Length < 7) ");
            exit(0);
        }
    }
    catch (UFC::Exception& ex)
    {
        printf(ex.what());
        PrintHelp();
    }
}  //CheckArgs()
//------------------------------------------------------------------------------
void LoadSetting()
{
    UFC::UiniFile *ConfigFilePtr = 0;
    try 
    {
        ConfigFilePtr = new UFC::UiniFile(G_ConfigFileName);
    }
    catch (UFC::FileException fe)
    {
        std::printf("Config File [%s] Open Error: %s\n", G_ConfigFileName.c_str(), fe.what());
        ConfigFilePtr = NULL;
    }
    if (ConfigFilePtr == NULL) exit(-1);

    UFC::AnsiString TempStr;
  
   
    if (ConfigFilePtr->GetValue("Setting", "AppName", TempStr) == TRUE)
        G_AppName = TempStr;
    else
    {
        std::printf("LoadSetting() Application Name not Setting in config file, program exit !\n");
        exit(1);
    }
        
    if (ConfigFilePtr->GetValue("Tandem", "IP", TempStr) == TRUE)
        G_TandemIP = TempStr;
    else
    {
        std::printf("LoadSetting() Tandem IP not Setting in config file, program exit !\n");
        exit(1);
    }
        
    if (ConfigFilePtr->GetValue("Tandem", "Port", TempStr) == TRUE)
        G_TandemPort = TempStr.ToInt();
    else
    {
        std::printf("LoadSetting() Tandem Port not Setting in config file, program exit !\n");
        exit(1);
    }
        
    if (ConfigFilePtr->GetValue("Tandem", "HeartbeatInterval", TempStr) == TRUE)
        G_TandemHeartbeatInterval = TempStr.ToInt();
    else
    {
        std::printf("LoadSetting() Tandem Heartbeat Interval not Setting in config file, program exit !\n");
        exit(1);
    }

    if (ConfigFilePtr->GetValue("Tandem", "CheckInterval", TempStr) == TRUE)
        G_TandemCheckInterval = TempStr.ToInt();
    else
    {
        std::printf("LoadSetting() Tandem Check Interval not Setting in config file, program exit !\n");
        exit(1);
    }
    delete ConfigFilePtr;
}  //LoadSetting()
//------------------------------------------------------------------------------
void CreateTimeFile(void)
{
    UFC::AnsiString ResendData;
    UFC::FileStreamEx File(YECTime.c_str(), "w+"); // "w+" = O_CREAT|O_TRUNC|O_RDWR
    UFC::UTime now;
    now.setCurrent();
    ResendData.Printf("%02d%02d%02d%03d%010d%010d", now.getHour(), now.getMinute(), now.getSecond(), 0, 0, 0);
    File.Write(ResendData.c_str(), ResendData.Length());
}  //CreateTimeFile()
//------------------------------------------------------------------------------
void Resend(const UFC::AnsiString& ResendStr)
{	
    Int32 resendTime = 0;
    Int32 SID = 0;
    Int32 reportSeqStart = 0;
    Int32 reportSeqEnd = 0;
    UFC::PStringList Strings;
    Strings.SetStrings( ResendStr, ",|\n" );

    if (Strings.ItemCount() >= 1) resendTime = Strings[0].ToInt();
    if (Strings.ItemCount() >= 2) SID = Strings[1].ToInt();
    if (Strings.ItemCount() >= 3) reportSeqStart = Strings[2].ToInt();
    if (Strings.ItemCount() >= 4) reportSeqEnd = Strings[3].ToInt();

    if (resendTime == 0)
    {
        UFC::BufferedLog::Printf( " Resend() Wrong time string." );
        return;
    }
    
    if ((resendTime / 10000) > 23)
    {
        UFC::BufferedLog::Printf(" Resend() Hour should between 0 to 23.");
        return;
    }
    
    if (((resendTime % 10000) / 100) > 59)
    {
        UFC::BufferedLog::Printf(" Resend() Minutes should between 0 to 59.");
        return;
    }
    
    if (YECTime.Length() == 0)
    {
        UFC::BufferedLog::Printf(" Resend() -C(Config file) must assign.");
        return;
    }  //if ( YECTime.Length() == 0 )
    UFC::BufferedLog::Printf(" Resend() Ask existing JSExecConnection to recover executions.");
    UFC::BufferedLog::Printf(" Resend() Write to YECTime file [%s].", YECTime.c_str());
    
    if (SID > 0)
        UFC::BufferedLog::Printf(" Resend() Session:%d", SID);
    else
        UFC::BufferedLog::Printf(" Resend() All sessions.");
    
    UFC::BufferedLog::Printf(" Resend() from Time:%06d", resendTime);
    if (reportSeqEnd > reportSeqStart)
        UFC::BufferedLog::Printf( "Resend() from report sequence %d to %d", reportSeqStart, reportSeqEnd);
    else
        UFC::BufferedLog::Printf( "Resend() from report sequence %d", reportSeqStart);
    
    UFC::FileStreamEx File(YECTime.c_str(), "w+"); // "w+" = O_CREAT|O_TRUNC|O_RDWR
    UFC::AnsiString recoverStr;
    recoverStr.Printf("%06d%03d%010d%010d", resendTime, SID, reportSeqStart, reportSeqEnd);
    File.Write(recoverStr.c_str(), recoverStr.Length());
    UFC::BufferedLog::FlushToFile();
}  //Resend()
//------------------------------------------------------------------------------
void GetRecoverTime(UFC::AnsiString& RecoverTimeStr, Int32& SessionID, Int32& StartSeq, Int32& EndSeq)
{
    char Params[64];
    UFC::FileStreamEx File(YECTime.c_str(), "r+");
    File.Read(Params, 30);
    RecoverTimeStr = UFC::AnsiString(Params, 6);
    SessionID      = UFC::AnsiString(Params + 6, 3).ToInt();
    StartSeq       = UFC::AnsiString(Params + 9, 10).ToInt();
    EndSeq         = UFC::AnsiString(Params + 19, 10).ToInt();
}  //GetRecoverTime()
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
    }  //if (ModifyTime == 0)
}  //CheckConfigModifiaction()
//------------------------------------------------------------------------------
int main(int argc, char** argv) 
{
    CheckArgs(argc, argv);
    LoadSetting();
    YECTime.Printf("YECTime.%s", G_AppName.c_str());
    G_ApplicationStartTime.setCurrent();
    G_LogFileName.Printf("%s/%s_%04d%02d%02d.log", G_LogDirectory.c_str(), G_AppName.c_str(),
                         G_ApplicationStartTime.getYear(), G_ApplicationStartTime.getMonth(), G_ApplicationStartTime.getDate());
    UFC::BufferedLog::SetLogObject(new UFC::BufferedLog( G_LogFileName, 1024, TRUE));
    UFC::BufferedLog::Printf(" --------------------------------------------------------------------------------");
    UFC::BufferedLog::Printf(" JSExecConnection Copyright (c)%d MDBS", UFC::ThisYear() );
    UFC::BufferedLog::Printf(" Build Date:%s %s", __DATE__, __TIME__);
    UFC::BufferedLog::Printf("                     ");
    UFC::BufferedLog::Printf(" %s start at %04d/%02d/%02d %02d:%02d:%02d", G_AppName.c_str(),
                             G_ApplicationStartTime.getYear(), G_ApplicationStartTime.getMonth(), G_ApplicationStartTime.getDate(),
                             G_ApplicationStartTime.getHour(), G_ApplicationStartTime.getMinute(), G_ApplicationStartTime.getSecond());
    UFC::BufferedLog::Printf(" Config File [%s], Build Date:%s %s", G_ConfigFileName.c_str(), __DATE__, __TIME__);
    UFC::BufferedLog::Printf(" Log File [%s]", G_LogFileName.c_str());
    UFC::BufferedLog::Printf(" YEC Time [%s]", YECTime.c_str());
    
    if (G_DoResend)
        UFC::BufferedLog::Printf(" Resend Data [%s]\n", G_ResendData.c_str());
    else
    {
        if (G_IsDumpOnly == TRUE) UFC::BufferedLog::Printf( " Dump Execution Report Only !");
        UFC::BufferedLog::Printf(" Tandem Server [%s:%d], Heartbeat Interval %d, Check Interval %d.", G_TandemIP.c_str(), G_TandemPort, G_TandemHeartbeatInterval, G_TandemCheckInterval);
        UFC::BufferedLog::Printf(" Debug Level [%d].", G_DebugLevel);

        if (G_ReceiveMarket == rmAll)
            UFC::BufferedLog::Printf(" Receive Futures and Options reports.");
        else if (G_ReceiveMarket == rmFutures)
            UFC::BufferedLog::Printf(" Receive Futures reports.");
        else if (G_ReceiveMarket == rmOption)
            UFC::BufferedLog::Printf(" Receive Options reports.");

        if (G_ExecutionType == ctBoth)
            UFC::BufferedLog::Printf(" Receive confirm and filled reports.");
        else if (G_ExecutionType == ctRecvComfirmOnly)
            UFC::BufferedLog::Printf(" Receive confirm reports.");
        else if (G_ExecutionType == ctRecvFillOnly)
            UFC::BufferedLog::Printf(" Receive filled reports.");
                
    }  //if (G_DoResend)
    UFC::BufferedLog::Printf(" --------------------------------------------------------------------------------");
    UFC::BufferedLog::FlushToFile();
    UFC::BufferedLog::SetDebugMode(G_DebugLevel);
    if (G_DoResend)
    {
        Resend(G_ResendData);
        return 0;
    }  //if ( G_DoResend )

    jihsun::CMBusAdapter *MBusAdpPtr = new jihsun::CMBusAdapter(G_ConfigFileName);
    UFC::SleepMS(1000);
    if (MBusAdpPtr->IsTerminate()) 
    {
        delete MBusAdpPtr;
        return 0;
    }
    
    jihsun::CTandemMessage::LoadNewSymbolRootIndex(G_ConfigFileName, true);
    jihsun::CTandemMessage::DumpNewSymbolRootIndex(UFC::dlInformation, true);
    jihsun::CDataExchangePool *DataPoolPtr = new jihsun::CDataExchangePool();
    
    if (G_IsFirstTimeStart == FALSE)
        GetRecoverBeginTime();
    else 
        G_IRecoverBeginTime = 0;
        
    CreateTimeFile();
    
    jihsun::CJSExecutionConnection *JsExecConnPtr = 
        new jihsun::CJSExecutionConnection(G_ConfigFileName, G_ReceiveMarket, G_ExecutionType, G_IRecoverBeginTime.ToInt32(), DataPoolPtr);
    UFC::BufferedLog::SetDebugMode(G_DebugLevel);
    JsExecConnPtr->LockForRecover();
    JsExecConnPtr->WaitForRecover();
    UFC::BufferedLog::Printf(" main() Recover Finished.");
    JsExecConnPtr->UnlockForRecover();
    
    jihsun::CTandemAdapter *tandemAdpPtr = new jihsun::CTandemAdapter(G_TandemIP, G_TandemPort, DataPoolPtr, G_TandemHeartbeatInterval, G_TandemCheckInterval);
    tandemAdpPtr->Start();
    
    while ( TRUE )
    {
        Int32 WaitResult = 0;
        DataPoolPtr->LockTandemMsgQueue();
        if ((DataPoolPtr->GetWaitingTandemMsgCount(false) <= 0) &&
            (DataPoolPtr->GetWaitingAlertMsgCount(false) <= 0)) 
            WaitResult = DataPoolPtr->WaitTandemMsgQueue(1000);
        if (WaitResult == 0)
        {
            Int32 waitingAlertMsgCount = DataPoolPtr->GetWaitingAlertMsgCount(false);
            Int32 waitingTandemMsgCount = DataPoolPtr->GetWaitingTandemMsgCount(false);
            DataPoolPtr->UnlockTandemMsgQueue();

            for (Int32 curAlertMsgIndex = 1; curAlertMsgIndex <= waitingAlertMsgCount; curAlertMsgIndex++)
            {
                UFC::AnsiString *AlertMsgPtr = DataPoolPtr->PopAlertMsg(true);
                MBusAdpPtr->SendAlert(*AlertMsgPtr);
                delete AlertMsgPtr;
            }  //for (Int32 curAlertMsgIndex = 1; curAlertMsgIndex <= waitingAlertMsgCount; curAlertMsgIndex++)
            
            for (Int32 curTandemMsgIndex = 1; curTandemMsgIndex <= waitingTandemMsgCount; curTandemMsgIndex++)
            {
                jihsun::CTandemMessage *tandemMsgPtr = DataPoolPtr->PopTandemMsg(true);
                nsOrderMessageDefine::MarketEnum execRptMarket = tandemMsgPtr->GetExecRptMarket();
                nsOrderMessageDefine::OrderStatusEnum execRptOrderStatus = tandemMsgPtr->GetExecRptOrderStatus();
                bool isSendSuccess = false;
                UFC::AnsiString LogHeadStr = "";                
                if (execRptMarket == nsOrderMessageDefine::mTWFutures)
                {
                    if ( ( execRptOrderStatus == nsOrderMessageDefine::osPartiallyFilled ) ||
                         ( execRptOrderStatus == nsOrderMessageDefine::osFilled ) )
                    {
                        LogHeadStr = "Fill_Fut";
                        UFC::BufferedLog::Printf(" <%s> <%s> %d/%d", LogHeadStr.c_str(), tandemMsgPtr->GetTandemData(), curTandemMsgIndex, waitingTandemMsgCount);
                    }
                    else
                    {
                        LogHeadStr = "Confirm_Fut";
                        UFC::BufferedLog::Printf(" <%s> <%s> %d/%d", LogHeadStr.c_str(), tandemMsgPtr->GetTandemData(), curTandemMsgIndex, waitingTandemMsgCount);    
                    }
                    
                    if ( G_IsDumpOnly == FALSE )
                    {
                        jihsun::CTandemTelegraph tandemTelegraph("ADD", tandemMsgPtr->GetSequenceNo(), tandemMsgPtr->GetTandemData());
                        isSendSuccess = tandemAdpPtr->SendData((Int8*)tandemTelegraph.GetTelegraphBuffer(), tandemTelegraph.GetTelegraphLength() + 2);
                    }
                }
                else if (execRptMarket == nsOrderMessageDefine::mTWOptions)
                {
                    if ( ( execRptOrderStatus == nsOrderMessageDefine::osPartiallyFilled ) ||
                         ( execRptOrderStatus == nsOrderMessageDefine::osFilled ) )
                    {
                        LogHeadStr = "Fill_Opt";
                        UFC::BufferedLog::Printf(" <%s> <%s> %d/%d", LogHeadStr.c_str(), tandemMsgPtr->GetTandemData(), curTandemMsgIndex, waitingTandemMsgCount);
                    }
                    else
                    {
                        LogHeadStr = "Confirm_Opt";
                        UFC::BufferedLog::Printf(" <%s> <%s> %d/%d", LogHeadStr.c_str(), tandemMsgPtr->GetTandemData(), curTandemMsgIndex, waitingTandemMsgCount);    
                    }
                    
                    if ( G_IsDumpOnly == FALSE )
                    {
                        jihsun::CTandemTelegraph tandemTelegraph("ADD", tandemMsgPtr->GetSequenceNo(), tandemMsgPtr->GetTandemData());
                        isSendSuccess = tandemAdpPtr->SendData((Int8*)tandemTelegraph.GetTelegraphBuffer(), tandemTelegraph.GetTelegraphLength() + 2);
                    }
                }
                else
                    UFC::BufferedLog::Printf(" main() Ignore Market [%d(%s)]", execRptMarket, jihsun::DescribeExecRptMarket(execRptMarket));
                
                if (G_IsDumpOnly == FALSE)
                {
                    if (isSendSuccess)
                        UFC::BufferedLog::DebugPrintf( UFC::dlPerformance, " <SEND> %s success.", LogHeadStr.c_str());
                    else
                        UFC::BufferedLog::DebugPrintf( " <SEND> %s failed.", LogHeadStr.c_str());
                }
                UFC::BufferedLog::FlushToFile();
            }  //for (Int32 curTandemMsgIndex = 1; curTandemMsgIndex <= waitingTandemMsgCount; curTandemMsgIndex++)
        }
        else if ((WaitResult == ETIMEDOUT) || (WaitResult == ETIME))
        {
            UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " main() Waiting Message Timeout %d.", WaitResult);
            DataPoolPtr->UnlockTandemMsgQueue();
            UFC::AnsiString RecoverTimeStr;
            Int32 SessionID, StartSeq, EndSeq;
            GetRecoverTime(RecoverTimeStr, SessionID, StartSeq, EndSeq);
            if ( CheckConfigModifiaction() == TRUE ) JsExecConnPtr->Recover(RecoverTimeStr);
        }
        else
            UFC::BufferedLog::Printf(" main() Waiting Tandem Message Error %d.", WaitResult);
    }  //while ( TRUE )
    delete DataPoolPtr;
    delete MBusAdpPtr;
    return 0;
}
//------------------------------------------------------------------------------
