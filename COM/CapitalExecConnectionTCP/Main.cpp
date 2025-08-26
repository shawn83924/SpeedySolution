/* 
 * File:   Main.cpp
 * Author: Zhen Fan
 *
 * Created on 2011/12/08 15:56
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>

#include "UFC.h"
#include "iniFile.h"
#include "PInt32.h"

#include "TandemObject.h"
#include "MBusAdapter.h"
#include "TandemAdapter.h"
#include "NoticeServer.h"
#include "CapitalExecutionConnection.h"
#include "CDataExchangePool.h"

UFC::UDateTime   G_ApplicationStartTime;
UFC::AnsiString  G_ApplicationName;
UFC::PInt32      G_RecoverBeginTime;
BOOL             G_IsDumpOnly;
BOOL             G_IsNoSocket;
BOOL             G_IsFirstName;
Int32            G_DebugLevel;
UFC::AnsiString  YECTime;
UFC::UTime       FTime;
Int32            G_OverlapTime = 30;

UFC::AnsiString  G_Today;
UFC::AnsiString  G_NextTradeDay;
UFC::AnsiString  G_LogFileDirectory;
UFC::AnsiString  G_LogFileName;
UFC::AnsiString  G_ScreenLogFileName;
UFC::AnsiString  G_ConfigFileDirectory;
UFC::AnsiString  G_ConfigFileName;
UFC::AnsiString  G_TWFuturesIP;
Int32            G_TWFuturesPort;
Int32            G_TWFuturesTimeOutInterval;
UFC::AnsiString  G_TWOptionsIP;
Int32            G_TWOptionsPort;
Int32            G_TWOptionsTimeOutInterval;
UFC::AnsiString  G_TWStockIP;
Int32            G_TWStockPort;
Int32            G_TWStockTimeOutInterval;
BOOL             G_FuturesAdapExist = FALSE;
BOOL             G_OptionsAdapExist = FALSE;
BOOL             G_StockAdapExist   = FALSE;


Int32            G_ServerPort = 0;
Int32            G_NoticeSize = 80;
UFC::BufferedLog *G_ScreenLogPtr;


UFC::AnsiString gSpeedySeqPrefix( "992" ); 
UFC::AnsiString gSpeedySourceID( "s " );    
//------------------------------------------------------------------------------
Int32 TimeToInt(const UFC::UTime& TargetTime)
{
    return ((TargetTime.getHour() * 10000) + (TargetTime.getMinute() * 100) + TargetTime.getSecond());
}  //TimeToInt()

//------------------------------------------------------------------------------
void UpdateRecoverBeginTime(const UFC::UTime& TargetTime)
{
    G_RecoverBeginTime = TimeToInt(TargetTime);
}//UpdateRecoverBeginTime()

//------------------------------------------------------------------------------
void GetRecoverBeginTime(void)
{
    FTime.setCurrent();
    UFC::AnsiString ShmName;
    ShmName.Printf("%s.RecoverBeginTime", G_ApplicationName.c_str());
    UFC::BufferedLog::DebugPrintf(" ShmName:[%s]", ShmName.c_str());
    G_RecoverBeginTime.Attach(ShmName);
    int intOrigRecoverBeginTime = G_RecoverBeginTime.ToInt32();
    int intNow = TimeToInt(FTime);
    
    if( intNow < intOrigRecoverBeginTime) ///< Over one day.
        G_RecoverBeginTime = 63000;
    else
    {
        FTime.setHour(intOrigRecoverBeginTime / 10000);
        FTime.setMinute((intOrigRecoverBeginTime % 10000) / 100);
        FTime.setSecond(intOrigRecoverBeginTime % 100);
        FTime -= G_OverlapTime;
        G_RecoverBeginTime = TimeToInt(FTime);
    }
    
std::printf("GetRecoverBeginTime() OrigRecoverBeginTime=%d Now=%d RecoverBeginTime=%d\n", intOrigRecoverBeginTime, intNow, G_RecoverBeginTime.ToInt32());
}  //GetRecoverBeginTime()

//------------------------------------------------------------------------------
void PrintHelp(void)
{
    std::printf( " CapitalExecConnectionTCP Copyright (c) September 2007~%d MDBS\n", UFC::ThisYear() );
    std::printf( " Build date[%s]\n", __DATE__ );
    std::printf( "    -N(Extension Name) Application Extension Name.\n" );
    std::printf( "    -F                 First time start up.\n" );
    std::printf( "    -D                 Dump execution to screen only.(Not send to MBus.)\n" );
    std::printf( "    -R(HHMMSS)         Resend execution from HHMMSS.\n" );
    exit( 1 );
}  //PrintHelp()
//------------------------------------------------------------------------------
void Resend(const UFC::AnsiString& ResendTimeStr)
{
    Int32 Time = ResendTimeStr.ToInt();
    if ( Time == 0 )
    {
        std::printf("Resend() Wrong time string.[%s]\n", ResendTimeStr.c_str());
        return;
    }
    if ((Time / 10000) > 23)
    {
        printf("Hour should between 0 to 23.[%s]\n", ResendTimeStr.c_str() );
        return;
    }
    if (((Time % 10000) / 100) > 59)
    {
        printf("Minutes should between 0 to 59.[%s]\n", ResendTimeStr.c_str() );
        return;
    }
    
    UFC::FileStreamEx File(YECTime.c_str(), "w+");  // "w+" = O_CREAT | O_TRUNC | O_RDWR
    File.Write(ResendTimeStr.c_str(), 7);
}  //Resend()

//------------------------------------------------------------------------------
void CheckArgs(int argc, char *argv[])
{
    UFC::AnsiString ExtensionStr = "";
    UFC::AnsiString ResendTimeStr = "";
    for (int i = 1; i < argc; i++)
    {
        UFC::AnsiString Param(argv[i]);
        if (Param.Length() < 2)
        {
            std::printf( "Wrong Parameter [%s]\n", Param.c_str());
            PrintHelp();            
            exit( 0 );
        }
        else if (Param.Length() == 2)
        {
            if( Param == "-F" || Param == "-f" )
                G_IsFirstName = TRUE;
            else if( Param == "-D" || Param == "-d" )
                G_IsDumpOnly = TRUE;
            else if( Param == "-S" || Param == "-s" )
                G_IsNoSocket = TRUE;
            else
            {
                if ((Param != "-H") && (Param != "-h"))
                    std::printf( "Wrong Parameter [%s]\n", Param.c_str());
                PrintHelp();            
                exit( 0 );
            }
        }
        else 
        {
            if (Param.AnsiPos("-N") != -1)
                ExtensionStr = (const char*)(Param.c_str() + 2);
            else if (Param.AnsiPos("-R") != -1)
                ResendTimeStr = (const char*)(Param.c_str() + 2);
            else if ((Param.AnsiPos("-d") != -1) || ((Param.AnsiPos("-D") != -1)))
            {
                if (Param.AnsiPos("f") != -1) G_DebugLevel |= UFC::dlFatalError;
                if (Param.AnsiPos("i") != -1) G_DebugLevel |= UFC::dlInformation;
                if (Param.AnsiPos("p") != -1) G_DebugLevel |= UFC::dlPerformance;
                if (Param.AnsiPos("w") != -1) G_DebugLevel |= UFC::dlWarning;
            }
            else
            {
                std::printf( "Wrong Parameter [%s]\n", Param.c_str());
                PrintHelp();            
                exit( 0 );
            }
        }
    }  
        
    if (ExtensionStr.Length() > 0)
        G_ApplicationName.Printf("CapitalExecConnectionTCP%s", ExtensionStr.c_str());
    YECTime.Printf("YECTime.%s", G_ApplicationName.c_str());
    
    if (ResendTimeStr.Length() > 0)
    {
        Resend(ResendTimeStr);
        std::printf( "ReSend Time[%s]\n", ResendTimeStr.c_str());
        exit(0);
    }
    GetRecoverBeginTime();
    if (G_IsFirstName == TRUE) G_RecoverBeginTime = 0;
}  
//------------------------------------------------------------------------------
void LoadSetting()
{    
    try 
    {
        UFC::UiniFile *ConfigFilePtr =  new UFC::UiniFile(G_ConfigFileName);
        
        UFC::AnsiString TempStr;
        
        if( G_RecoverBeginTime != 0 )
        {
            if( (ConfigFilePtr->GetValue("Setting", "SendFromFirst", TempStr) == TRUE) && (TempStr.Length() > 0))
                G_RecoverBeginTime = TempStr.ToInt();
        }
        if( ConfigFilePtr->ValueExists( "TW_Futures", "IP" ) ==TRUE &&
            ConfigFilePtr->ValueExists( "TW_Futures", "Port") == TRUE &&
            ConfigFilePtr->ValueExists( "TW_Futures", "HeartbeatTime") == TRUE )
        {
            ConfigFilePtr->GetValue("TW_Futures", "IP", G_TWFuturesIP);        
            ConfigFilePtr->GetValue("TW_Futures", "Port", TempStr);
            G_TWFuturesPort = TempStr.ToInt();    
            ConfigFilePtr->GetValue("TW_Futures", "HeartbeatTime", TempStr);
            G_TWFuturesTimeOutInterval = TempStr.ToInt() * 60000;
            G_FuturesAdapExist = TRUE;
        }
        if( ConfigFilePtr->ValueExists( "TW_Options", "IP" ) ==TRUE &&
            ConfigFilePtr->ValueExists( "TW_Options", "Port") == TRUE &&
            ConfigFilePtr->ValueExists( "TW_Options", "HeartbeatTime") == TRUE )
        {

            ConfigFilePtr->GetValue("TW_Options", "IP", G_TWOptionsIP);
            ConfigFilePtr->GetValue("TW_Options", "Port", TempStr);
            G_TWOptionsPort = TempStr.ToInt();
            ConfigFilePtr->GetValue("TW_Options", "HeartbeatTime", TempStr);
            G_TWOptionsTimeOutInterval = TempStr.ToInt() * 60000;
            G_OptionsAdapExist = TRUE;
        }
        if( ConfigFilePtr->ValueExists( "TW_Stock", "IP" ) ==TRUE &&
            ConfigFilePtr->ValueExists( "TW_Stock", "Port") == TRUE &&
            ConfigFilePtr->ValueExists( "TW_Stock", "HeartbeatTime") == TRUE )
        {    
            ConfigFilePtr->GetValue("TW_Stock", "IP", G_TWStockIP);         
            ConfigFilePtr->GetValue("TW_Stock", "Port", TempStr);
            G_TWStockPort = TempStr.ToInt();    
            ConfigFilePtr->GetValue("TW_Stock", "HeartbeatTime", TempStr);
            G_TWStockTimeOutInterval = TempStr.ToInt() * 60000;
            if( ConfigFilePtr->GetValue("TW_Stock", "SeqPrefix", gSpeedySeqPrefix) == true )
                gSpeedySeqPrefix.PadThis( 3, '0');
            if( ConfigFilePtr->GetValue("TW_Stock", "SourceID", gSpeedySourceID) == true )            
                gSpeedySourceID.PadThis( 2, ' ', TRUE );
            G_StockAdapExist = TRUE;
        }    

        if( ConfigFilePtr->GetValue("ServerSetting", "Port", TempStr) == TRUE )
            G_ServerPort = TempStr.ToInt();
        if( ConfigFilePtr->GetValue("ServerSetting", "NoticeSize", TempStr) == TRUE )
            G_NoticeSize = TempStr.ToInt();

        delete ConfigFilePtr;        
    }
    catch (UFC::FileException fe)
    {
        std::printf("Config File [%s] Open Error: %s\n", G_ConfigFileName.c_str(), fe.what());
        exit( 1 );
    }
}  
//------------------------------------------------------------------------------
void CreateTimeFile( void )
{
    UFC::FileStreamEx TimeFile(YECTime.c_str(), "w+");  //"w+" = O_CREAT | O_TRUNC | O_RDWR
    UFC::UDateTime Now;
    Now.setCurrent();
    char TimeBuffer[7];
    std::sprintf(TimeBuffer, "%02d%02d%02d", Now.getHour(), Now.getMinute(), Now.getSecond());
    TimeFile.Write(TimeBuffer, 7);    
}  //CreateTimeFile()

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
        if( ModifyTime != FileStat.st_mtime )
        {
            ModifyTime = FileStat.st_mtime;
            return TRUE;    
        }
        return FALSE;
    }
}  //CheckConfigModifiaction()

//------------------------------------------------------------------------------
UFC::AnsiString GetRecoverTime(void)
{
    UFC::FileStreamEx TimeFile(YECTime.c_str(), "r+");
    char TimeBuffer[7];
    TimeFile.Read(TimeBuffer, 7);    
    return UFC::AnsiString(TimeBuffer); 
}  //GetRecoverTime()
//------------------------------------------------------------------------------
void GetNextTradeDay( )
{
    UFC::UDate                       Today = UFC::GetTradeDate( );
    UFC::PHashedSet<UFC::AnsiString> RestDays; 
    FILE*                            FFileHandle;
    char                             Buffer[ 256 ];
    int                              i;
    
    G_Today.Printf( "%04d%02d%02d", Today.getYear(), Today.getMonth(), Today.getDate() );
    if( (FFileHandle = fopen( "../cfg/FUTREST", "r" )) != NULL )
    {     
        while( fgets( Buffer, 256, FFileHandle ) != NULL ) ///< Get the first execution.
        {             
           UFC::AnsiString RestDay( Buffer, 8);
           RestDays.Add( RestDay );
        }
        fclose( FFileHandle );
        UFC::BufferedLog::Printf(" [%d] holiday loaded.", RestDays.ItemCount() );    
        for( i = 0; i < 14; i++ )
        {
            Today +=1;
            G_NextTradeDay.Printf( "%04d%02d%02d", Today.getYear(), Today.getMonth(), Today.getDate() );
            if( RestDays.Exists( G_NextTradeDay ) == FALSE )
                break;
        }
        if( i == 13 )
            UFC::BufferedLog::Printf(" Can't find next trading date." );    
            
    }
    else
    {
        do
        {
            Today +=1;
        }
        while( Today.getWeekDay() > 5 );
        G_NextTradeDay.Printf( "%04d%02d%02d", Today.getYear(), Today.getMonth(), Today.getDate() );
        UFC::BufferedLog::Printf(" Can't open file[../cfg/FUTREST]" );    
    }
    UFC::BufferedLog::Printf(" Trading Date now [%s]", G_Today.c_str() );
    UFC::BufferedLog::Printf(" Next trading Date[%s]", G_NextTradeDay.c_str() );
}
//------------------------------------------------------------------------------
//
// 
//
//------------------------------------------------------------------------------
int main(int argc, char** argv) 
{
    G_ApplicationStartTime.setCurrent();
    G_ApplicationName = "CapitalExecConnectionTCP";
    G_IsFirstName = FALSE;
    G_IsDumpOnly = FALSE;
    G_IsNoSocket = FALSE;
    G_DebugLevel = 0;
    G_LogFileDirectory = "../log";
    G_ConfigFileDirectory = "../cfg";
    G_TWFuturesIP = "127.0.0.1";
    G_TWFuturesPort = 11122;
    G_TWFuturesTimeOutInterval = 30000;
    G_TWOptionsIP = "127.0.0.1";
    G_TWOptionsPort = 11122;
    G_TWOptionsTimeOutInterval = 30000;
    G_TWStockIP = "127.0.0.1"; // 172.17.122.19
    G_TWStockPort = 65120;
    G_TWStockTimeOutInterval = 30000;
    
    CheckArgs(argc, argv);
    G_LogFileName.Printf("%s/%s_%04d%02d%02d.log", G_LogFileDirectory.c_str(), G_ApplicationName.c_str(),
                         G_ApplicationStartTime.getYear(), G_ApplicationStartTime.getMonth(), G_ApplicationStartTime.getDate());
    G_ScreenLogFileName.Printf("%s/%sSendMiss%s.log", G_LogFileDirectory.c_str(), G_ApplicationName.c_str(), UFC::GetDateString().c_str());
    G_ConfigFileName.Printf("%s/%s.cfg", G_ConfigFileDirectory.c_str(), G_ApplicationName.c_str());
    LoadSetting();
    UFC::BufferedLog::SetLogObject(new UFC::BufferedLog(G_LogFileName, 1024, TRUE));
    UFC::BufferedLog::Printf(" ");
    UFC::BufferedLog::Printf(" --------------------------------------------------------------------------------");
    UFC::BufferedLog::Printf(" %s start at %04d/%02d/%02d %02d:%02d:%02d, Build Date:%s", G_ApplicationName.c_str(),
                             G_ApplicationStartTime.getYear(), G_ApplicationStartTime.getMonth(), G_ApplicationStartTime.getDate(),
                             G_ApplicationStartTime.getHour(), G_ApplicationStartTime.getMinute(), G_ApplicationStartTime.getSecond(),  __DATE__);
    GetNextTradeDay( );
    UFC::BufferedLog::Printf(" Startup at %s.             ", UFC::Hostname );    
    UFC::BufferedLog::Printf(" Build Date:%s Time:%s", __DATE__, __TIME__ );        
    UFC::BufferedLog::Printf(" Recover Begin Time [%d]", G_RecoverBeginTime.ToInt32() );
    if (G_IsFirstName == TRUE)
        UFC::BufferedLog::Printf(" First Time Start Up is TRUE.");
    else
        UFC::BufferedLog::Printf(" First Time Start Up is FALSE.");
    if (G_IsDumpOnly == TRUE)
        UFC::BufferedLog::Printf(" Dump Only is TRUE.");
    else
        UFC::BufferedLog::Printf(" Dump Only is FALSE.");
    if (G_IsNoSocket == TRUE) 
        UFC::BufferedLog::Printf(" No Socket is TRUE.");
    else
        UFC::BufferedLog::Printf(" No Socket is FALSE.");
    UFC::BufferedLog::Printf(" Config File [%s]", G_ConfigFileName.c_str());
    UFC::BufferedLog::Printf(" Log File [%s]", G_LogFileName.c_str());
    UFC::BufferedLog::Printf(" Debug Level %d", G_DebugLevel);
    UFC::BufferedLog::Printf(" TW Futures Server [%s:%d]. TimeOut %d milliSecond", G_TWFuturesIP.c_str(), G_TWFuturesPort, G_TWFuturesTimeOutInterval);
    UFC::BufferedLog::Printf(" TW Options Server [%s:%d]. TimeOut %d milliSecond", G_TWOptionsIP.c_str(), G_TWOptionsPort, G_TWOptionsTimeOutInterval);
    UFC::BufferedLog::Printf(" TW Stocks  Server [%s:%d]. TimeOut %d milliSecond", G_TWStockIP.c_str(), G_TWStockPort, G_TWStockTimeOutInterval);
    if( G_ServerPort != 0 )
        UFC::BufferedLog::Printf(" Notice Server Port:[%d] Data Size[%d].", G_ServerPort, G_NoticeSize );
    UFC::BufferedLog::Printf(" --------------------------------------------------------------------------------");
    UFC::BufferedLog::FlushToFile();
    UFC::BufferedLog::SetDebugMode(G_DebugLevel);

    capital::CTandemMessage::LoadNewSymbolRootIndex(G_ConfigFileName, true);
    capital::CTandemMessage::DumpNewSymbolRootIndex(UFC::dlInformation, true);
    

    capital::CDataExchangePool*           DataPoolPtr = new capital::CDataExchangePool();    
    capital::CMBusAdapter*                MBusAdpPtr = new capital::CMBusAdapter(G_ApplicationName, G_ConfigFileName);
    capital::CNoticeServer*               NoticeServerPtr;
    capital::CCapitalExecutionConnection* ExecRptConnPtr = new capital::CCapitalExecutionConnection(G_ApplicationName, G_ConfigFileName, G_RecoverBeginTime.ToInt32(), DataPoolPtr);        
    
    if( G_ServerPort != 0 )
        NoticeServerPtr = new capital::CNoticeServer(G_ServerPort, DataPoolPtr, G_NoticeSize);        
    UFC::BufferedLog::SetDebugMode(G_DebugLevel);
    UFC::SleepMS(500);
    
    capital::CTandemAdapter *TWFuturesAdpPtr = NULL;
    capital::CTandemAdapter *TWOptionsAdpPtr = NULL;
    capital::CTandemAdapter *TWStockAdpPtr   = NULL;
    if (G_IsDumpOnly == FALSE)
    {
        if( G_FuturesAdapExist == TRUE )
        {
            TWFuturesAdpPtr =  new capital::CTandemAdapter(nsOrderMessageDefine::mTWFutures, false,G_TWFuturesIP, G_TWFuturesPort, DataPoolPtr,G_TWFuturesTimeOutInterval);
            TWFuturesAdpPtr->Start();
            UFC::SleepMS(500);
        }
        if( G_OptionsAdapExist == TRUE )
        {
            TWOptionsAdpPtr = new capital::CTandemAdapter(nsOrderMessageDefine::mTWOptions, false,G_TWOptionsIP, G_TWOptionsPort, DataPoolPtr,G_TWOptionsTimeOutInterval);
            TWOptionsAdpPtr->Start();
            UFC::SleepMS(500);
        }
        if( G_StockAdapExist == TRUE )
        {
            TWStockAdpPtr  =  new capital::CTandemAdapter(nsOrderMessageDefine::mTSE, false, G_TWStockIP, G_TWStockPort, DataPoolPtr,G_TWStockTimeOutInterval);
            TWStockAdpPtr->Start();
            UFC::SleepMS(500);
        }
    }       
    CreateTimeFile();
    G_ScreenLogPtr = new UFC::BufferedLog(G_ScreenLogFileName, 1024, TRUE);
    while( true )
    {
        Int32 WaitResult = 0;
        DataPoolPtr->LockTandemMsgQueue();
        if ((DataPoolPtr->GetWaitingTandemMsgCount(false) <= 0) &&
            (DataPoolPtr->GetWaitingAlertMsgCount(false) <= 0) &&
            (DataPoolPtr->GetWaitingNoticeMsgCount(false) <= 0))
            WaitResult = DataPoolPtr->WaitTandemMsgQueue( 1000 );
        
        if (WaitResult == 0)
        {
            while (DataPoolPtr->GetWaitingAlertMsgCount(false) > 0)
            {
                UFC::AnsiString *AlertMsgPtr = DataPoolPtr->PopAlertMsg(false);
                MBusAdpPtr->SendAlert(*AlertMsgPtr);
                delete AlertMsgPtr;
            }             
            while (DataPoolPtr->GetWaitingNoticeMsgCount(false) > 0)
            {
                UFC::AnsiString *NoticeMsgPtr = DataPoolPtr->PopNoticeMsg(false);
                MBusAdpPtr->SendNotice(*NoticeMsgPtr);
                delete NoticeMsgPtr;
            }              
            while (DataPoolPtr->GetWaitingTandemMsgCount(false) > 0)
            {
                capital::CTandemMessage *tandemMsgPtr = DataPoolPtr->PopTandemMsg(false);
                nsOrderMessageDefine::MarketEnum execRptMarket = tandemMsgPtr->GetExecRptMarket();
                nsOrderMessageDefine::OrderStatusEnum execRptOrderStatus = tandemMsgPtr->GetExecRptOrderStatus();
                bool IsSendSuccess = false;
                bool IsRecvSuccess = false;
                Int8 RecvBuffer[tandemMsgPtr->GetTandemDataLength() + 1];
                UFC::AnsiString LogHeadStr = "";
                
                if( execRptMarket == nsOrderMessageDefine::mTWFutures)
                {
                    if ((execRptOrderStatus == nsOrderMessageDefine::osPartiallyFilled) ||
                        (execRptOrderStatus == nsOrderMessageDefine::osFilled))
                    {
                        UFC::BufferedLog::Printf(" <Fill> <%s>", tandemMsgPtr->GetTandemData());
                        LogHeadStr = "Fill";
                    }
                    else
                    {
                        UFC::BufferedLog::Printf(" <Confirm> <%s>", tandemMsgPtr->GetTandemData());    
                        LogHeadStr = "Confirm";
                    }
                    
                    if( !G_IsDumpOnly && TWFuturesAdpPtr != NULL )
                    {
                        IsSendSuccess = TWFuturesAdpPtr->SendData(tandemMsgPtr->GetTandemData(), tandemMsgPtr->GetTandemDataLength());
                        if (IsSendSuccess)
                            IsRecvSuccess = TWFuturesAdpPtr->ReceiveData(RecvBuffer, tandemMsgPtr->GetTandemDataLength());
                    } 
                }
                else if (execRptMarket == nsOrderMessageDefine::mTWOptions)
                {
                    if ((execRptOrderStatus == nsOrderMessageDefine::osPartiallyFilled) ||
                        (execRptOrderStatus == nsOrderMessageDefine::osFilled))
                    {
                        UFC::BufferedLog::Printf(" <Fill> <%s>", tandemMsgPtr->GetTandemData());
                        LogHeadStr = "Fill_Opt";
                    }
                    else
                    {
                        UFC::BufferedLog::Printf(" <Confirm> <%s>", tandemMsgPtr->GetTandemData());    
                        LogHeadStr = "Confirm_OPT";
                    }
                    
                    if( !G_IsDumpOnly && TWOptionsAdpPtr != NULL )
                    {
                        IsSendSuccess = TWOptionsAdpPtr->SendData(tandemMsgPtr->GetTandemData(), tandemMsgPtr->GetTandemDataLength());
                        if (IsSendSuccess)
                            IsRecvSuccess = TWOptionsAdpPtr->ReceiveData(RecvBuffer, tandemMsgPtr->GetTandemDataLength());
                    } 
                }  
                else if (execRptMarket == nsOrderMessageDefine::mTSE || execRptMarket == nsOrderMessageDefine::mOTC )
                {
                    UFC::AnsiString Ex;
                    if (execRptMarket == nsOrderMessageDefine::mTSE )
                        Ex = "TSE";
                    else
                        Ex = "OTC";
                    if ((execRptOrderStatus == nsOrderMessageDefine::osPartiallyFilled) ||
                        (execRptOrderStatus == nsOrderMessageDefine::osFilled))
                    {
                        UFC::BufferedLog::Printf(" <Fill> <%s>", tandemMsgPtr->GetTandemData());
                        LogHeadStr = "Fill_"+Ex;
                    }
                    else
                    {
                        UFC::BufferedLog::Printf(" <Confirm> <%s>", tandemMsgPtr->GetTandemData());    
                        LogHeadStr = "Confirm_"+Ex;
                    }                    
                    if (!G_IsDumpOnly && TWStockAdpPtr != NULL )
                    {
                        IsSendSuccess = TWStockAdpPtr->SendData(tandemMsgPtr->GetTandemData(), tandemMsgPtr->GetTandemDataLength());
                        if (IsSendSuccess)
                            IsRecvSuccess = TWStockAdpPtr->ReceiveData(RecvBuffer, tandemMsgPtr->GetTandemDataLength());
                    } 
                }  
                
                UFC::BufferedLog::Printf( " <SEND> %s", LogHeadStr.c_str());
                if (!G_IsDumpOnly)
                {
                    if (IsRecvSuccess)
                    {
                        RecvBuffer[tandemMsgPtr->GetTandemDataLength()] = 0;
                        UFC::BufferedLog::Printf(" Recv Data: [%s]", RecvBuffer);
                    }
                    
                    if( !IsSendSuccess)
                    {
                        UFC::UDateTime SendFailTime;
                        SendFailTime.setCurrent();
                        UFC::BufferedLog::Printf( " %s send fail:[%s]", LogHeadStr.c_str(), tandemMsgPtr->GetTandemData());
                        UFC::AnsiString ScreenLogStr;
                        ScreenLogStr.Printf("%s time:%02d%02d%02d DATA[%s]\n", LogHeadStr.c_str(), SendFailTime.getHour(), SendFailTime.getMinute(), SendFailTime.getSecond(), tandemMsgPtr->GetTandemData());
                        G_ScreenLogPtr->WriteString(ScreenLogStr);
                        G_ScreenLogPtr->Flush();
                    }
                } 
                UFC::BufferedLog::FlushToFile();
                delete tandemMsgPtr;
            } 
            DataPoolPtr->UnlockTandemMsgQueue();
        }
        else if ((WaitResult == ETIMEDOUT) || (WaitResult == ETIME))
        {

            DataPoolPtr->UnlockTandemMsgQueue();
            if (CheckConfigModifiaction() == TRUE) ExecRptConnPtr->Recover(GetRecoverTime());
            
            if (G_IsDumpOnly == FALSE)
            {
                UFC::AnsiString DisconnectTime = "";
                if ((TWFuturesAdpPtr != NULL ) && TWFuturesAdpPtr->IsConnected())
                {
                    DisconnectTime = TWFuturesAdpPtr->GetDisconnectTime();
                    if (DisconnectTime.Length() > 0)
                    {
                        TWFuturesAdpPtr->ClearDisconnectTime();
                        ExecRptConnPtr->Recover(DisconnectTime);
                    }
                }                
                if ((TWOptionsAdpPtr != NULL ) && TWOptionsAdpPtr->IsConnected())
                {
                    DisconnectTime = TWOptionsAdpPtr->GetDisconnectTime();
                    if (DisconnectTime.Length() > 0)
                    {
                        TWOptionsAdpPtr->ClearDisconnectTime();
                        ExecRptConnPtr->Recover(DisconnectTime);
                    }
                }
                if ((TWStockAdpPtr != NULL ) && TWStockAdpPtr->IsConnected())
                {
                    DisconnectTime = TWStockAdpPtr->GetDisconnectTime();
                    if (DisconnectTime.Length() > 0)
                    {
                        TWStockAdpPtr->ClearDisconnectTime();
                        ExecRptConnPtr->Recover(DisconnectTime);
                    }
                }
            } 
        }
        else
            UFC::BufferedLog::Printf(" main() Waiting Tandem Message Error %d.", WaitResult);
    }     
    delete G_ScreenLogPtr;
    delete MBusAdpPtr;
    delete NoticeServerPtr;    
    delete ExecRptConnPtr;
    delete DataPoolPtr;
    return 0;
}

