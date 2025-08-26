/* 
 * File:   CapitalExecutionConnection.cpp
 * Author: Zhen Fan
 *
 * Created on 2011/12/15 15:01
 */

#include "CapitalExecutionConnection.h"
#include "TMPMessage.h"  //TAIFEXLib
namespace capital
{
//------------------------ CCapitalExecutionConnection -------------------------
//------------------------------------------------------------------------------
CCapitalExecutionConnection::CCapitalExecutionConnection(const UFC::AnsiString& ApplicationName, 
                                                         const UFC::AnsiString& ConfigFileName, 
                                                         Int32 RecoverBeginTime, 
                                                         CDataExchangePool *DataPoolPtr)
:UFC::PThread(NULL, FALSE),
 FConfigFileName(ConfigFileName),
 FApplicationName(ApplicationName),
 FRecoverBeginTime(RecoverBeginTime),
 FDataPoolPtr(DataPoolPtr),
 FTaifexIP("127.0.0.1"),
 FTaifexPort(23456),
 FID("Admin"),
 FPassword("1234rewq"),
 FAccount("0000001"),
 FBrokerID("F020999"),
 FApKind("s"),
 FNeedSendQuoteExecutionReport(false),
 FTaifexConnectionPtr(0),
 FIsConnected(false),
 FIsLogon(false)
        
{
    LoadSetting();
    PrintSetting();
    FTaifexConnectionPtr = new TTaifexConnection(FApplicationName, this);
    FTaifexConnectionPtr->SetUseNewFuturesSymbol(TRUE);
    ///< Set the broker ID of this connection.
    FTaifexConnectionPtr->SetBrokerID( nsOrderMessageDefine::mTWFutures, FBrokerID.c_str() );
    FTaifexConnectionPtr->SetCMID(FBrokerID.SubString(0,4).c_str());
    ///< Load TAIFEX message format config files.
    FTaifexConnectionPtr->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FTaifexConnectionPtr->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );
    ///< Load TSEC/OTC  message format config files.
    FTaifexConnectionPtr->SetTSEDataFormatResourceID( IDR_TSE_T010, IDR_TSE_O010, IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
    FTaifexConnectionPtr->SetOTCDataFormatResourceID( IDR_OTC_T010, IDR_OTC_O010, IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
    FTaifexConnectionPtr->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010, IDR_TSE_E010, IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020, IDR_TSE_Ex020, "INI" );
    FTaifexConnectionPtr->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );
    
    TMP::TMPMessage::InitTMPMessageLibrary(TMP::tlmBoth);
    ///< Start running thread.
    Start();
}
//------------------------------------------------------------------------------
CCapitalExecutionConnection::~CCapitalExecutionConnection()
{
    
}  //CCapitalExecutionConnection::~CCapitalExecutionConnection()

//------------------------------------------------------------------------------
void CCapitalExecutionConnection::LoadSetting()
{
    UFC::UiniFile *ConfigFilePtr = 0;
    try 
    {
        ConfigFilePtr = new UFC::UiniFile(FConfigFileName);
    }
    catch (UFC::FileException fe)
    {
        UFC::BufferedLog::Printf(" CMBusAdapter::LoadSetting() [%s] Error: %s", FConfigFileName.c_str(), fe.what());
        ConfigFilePtr = NULL;
    }
    
    if (ConfigFilePtr != NULL)
    {
        UFC::AnsiString TempStr;
        if (ConfigFilePtr->GetValue("Setting", "IP", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CCapitalExecutionConnection::LoadSetting() Taifex IP is Not Setting in ConfigFile, Use Default Value \"%s\".", FTaifexIP.c_str());
        else
            FTaifexIP = TempStr;

        if (ConfigFilePtr->GetValue("Setting", "Port", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CCapitalExecutionConnection::LoadSetting() Taifex Port is Not Setting in ConfigFile, Use Default Value %d!", FTaifexPort);
        else
            FTaifexPort = TempStr.ToInt();

        if (ConfigFilePtr->GetValue("Setting", "ID", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CCapitalExecutionConnection::LoadSetting() ID is Not Setting in ConfigFile, Use Default Value \"%s\".", FID.c_str());
        else
            FID = TempStr;

        if (ConfigFilePtr->GetValue("Setting", "Password", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CCapitalExecutionConnection::LoadSetting() Password is Not Setting in ConfigFile, Use Default Value \"%s\".", FPassword.c_str());
        else
            FPassword = TempStr;

        if (ConfigFilePtr->GetValue("Setting", "Account", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CCapitalExecutionConnection::LoadSetting() Account is Not Setting in ConfigFile, Use Default Value \"%s\".", FAccount.c_str());
        else
            FAccount = TempStr;

        if (ConfigFilePtr->GetValue("Setting", "BrokerID", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CCapitalExecutionConnection::LoadSetting() BrokerID is Not Setting in ConfigFile, Use Default Value \"%s\".", FBrokerID.c_str());
        else
            FBrokerID = TempStr;

        if (ConfigFilePtr->GetValue("Setting", "ApKind", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CCapitalExecutionConnection::LoadSetting() ApKind is Not Setting in ConfigFile, Use Default Value \"%s\".", FApKind.c_str());
        else
            FApKind = TempStr;

        if (ConfigFilePtr->GetValue("Setting", "SendQuoteExec", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CCapitalExecutionConnection::LoadSetting() SendQuoteExec is Not Setting in ConfigFile, Use Default Value false");
        else
        {
            if ((TempStr == "1") || (TempStr == "Y") || (TempStr == "y"))
                FNeedSendQuoteExecutionReport = true;
            else
                FNeedSendQuoteExecutionReport = false;
        }

        if (ConfigFilePtr->GetValue("Setting", "ExcludeBrokerID", TempStr) == TRUE)
        {
            UFC::PStringList Brokers;
            Brokers.SetStrings(TempStr, ",");
            for (register int i = 0; i < Brokers.ItemCount(); i++)
            {
                if (Brokers.GetItem(i).Length() > 0)
                    FExcludeBrokerIDSet.Add(Brokers.GetItem(i));
            }
        }
        else
            UFC::BufferedLog::Printf(" CCapitalExecutionConnection::LoadSetting() No Ecclude BrokerID Setting in ConfigFile");
        
        delete ConfigFilePtr;
    }  //if (ConfigFilePtr != NULL)
    UFC::BufferedLog::FlushToFile();    
}  //CCapitalExecutionConnection::LoadSetting()

//------------------------------------------------------------------------------
void CCapitalExecutionConnection::Recover(const UFC::AnsiString& BeginTime)
{
    if (FIsLogon == TRUE)
    {
        FTaifexConnectionPtr->Recover(BeginTime.c_str(), rdBoth, rmAll);
        UFC::BufferedLog::Printf(" Recover from: %s", BeginTime.c_str());
    }
}  //CCapitalExecutionConnection::Recover()

//------------------------------------------------------------------------------
void CCapitalExecutionConnection::PrintSetting()
{
    UFC::BufferedLog::Printf(" -------------------------------  CCapitalExecutionConnection  -------------------------------");
    UFC::BufferedLog::Printf(" [%s] Recover Begin[%d]", FApplicationName.c_str(), FRecoverBeginTime);
    UFC::BufferedLog::Printf(" Taifex IP:Port [%s:%d]", FTaifexIP.c_str(), FTaifexPort);
    UFC::BufferedLog::Printf(" ID[%s] Password[%s] Account[%s] BrokerID[%s] ApKind[%s]", FID.c_str(), FPassword.c_str(), FAccount.c_str(), FBrokerID.c_str(), FApKind.c_str());
    if (FNeedSendQuoteExecutionReport)
        UFC::BufferedLog::Printf(" Need Send Quote Execution Report");
    else
        UFC::BufferedLog::Printf(" Do Not Send Quote Execution Report");
    
    UFC::AnsiString ExcludeBrokerIDStr = "";
    for (int i = 0; i < FExcludeBrokerIDSet.ItemCount(); i++)
    {
        if (ExcludeBrokerIDStr.Length() > 0) ExcludeBrokerIDStr = ExcludeBrokerIDStr + ", ";
        ExcludeBrokerIDStr = ExcludeBrokerIDStr + FExcludeBrokerIDSet.GetItem(i);
    }
    if (ExcludeBrokerIDStr.Length() > 0)
        UFC::BufferedLog::Printf(" Exclude Broker ID: %s", ExcludeBrokerIDStr.c_str());
    else
        UFC::BufferedLog::Printf(" No Exclude Broker ID.");
    
    UFC::BufferedLog::Printf(" -------------------------------  CCapitalExecutionConnection  -------------------------------");
    UFC::BufferedLog::Printf(" ");
    UFC::BufferedLog::FlushToFile();    
}  //CCapitalExecutionConnection::PrintSetting()

//------------------------------------------------------------------------------
void CCapitalExecutionConnection::OnConnected(void)
{
    FIsLogon = true;
    UFC::BufferedLog::Printf( " CCapitalExecutionConnection::OnConnected() %s Connect to [%s:%d].", FApplicationName.c_str(), FTaifexIP.c_str(), FTaifexPort);
    UFC::BufferedLog::FlushToFile();
    FTaifexConnectionPtr->Logon(FID.c_str(), FPassword.c_str(), FAccount.c_str());
}  //CCapitalExecutionConnection::OnConnected()

//------------------------------------------------------------------------------
void CCapitalExecutionConnection::OnDisconnected(void)
{
    FIsLogon = false;
}  //CCapitalExecutionConnection::OnDisconnected()

//------------------------------------------------------------------------------
void CCapitalExecutionConnection::OnLogonReply(const char* ReplyString, LogonResult Result, int CID)
{
    if (Result == lrOk)
    {
        FIsLogon = true;
        UFC::BufferedLog::Printf(" CCapitalExecutionConnection::OnLogonReply() %s(CID=%d) ID[%s], Account[%s] logon OK.", ReplyString, CID, FID.c_str(), FAccount.c_str());
        UFC::AnsiString BeginTime;
        BeginTime.Printf("%06d", FRecoverBeginTime);
        UFC::BufferedLog::Printf(" CCapitalExecutionConnection::OnLogonReply() FTaifexConnectionPtr->Recover(%s, rdBoth, rmAll).", BeginTime.c_str() );
        FTaifexConnectionPtr->Recover(BeginTime.c_str(), rdBoth, rmAll);
        UFC::BufferedLog::FlushToFile();
    }
    else
    {
        FIsLogon = false;
        UFC::BufferedLog::Printf(" CCapitalExecutionConnection::OnLogonReply() %s ID[%s], Account[%s] logon failed.", ReplyString, FID.c_str(), FAccount.c_str());
        UFC::BufferedLog::FlushToFile();
        exit(0);
    }    
}  //CCapitalExecutionConnection::OnLogonReply()
//------------------------------------------------------------------------------
void CCapitalExecutionConnection::AddExecution(TExecutionReportMessage* Msg)
{
    CTandemMessage* tandemMsgPtr = new CTandemMessage( Msg );
    if( FDataPoolPtr != 0)
    {
        UFC::AnsiString execID = tandemMsgPtr->GetExecID();
        bool isExecIDExisted = false;
        if ((execID.Length() > 0) &&!FDataPoolPtr->InsertExecIDIndex(execID, true, true))  //Do Check, do Lock
        {
            UFC::AnsiString orderNo = tandemMsgPtr->GetOrderNo();
            UFC::BufferedLog::Printf(" Duplicate ExecID[%s], OrderNo[%s].", execID.c_str(), orderNo.c_str());
            isExecIDExisted = true;
        }  //if (execID.Length() > 0)
        if (!isExecIDExisted) 
            FDataPoolPtr->PushTandemMsg(tandemMsgPtr, true);
        else
            delete tandemMsgPtr;
    }
    else
    {
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " Tandem[%s]", tandemMsgPtr->GetTandemData());
        delete tandemMsgPtr;
    }    
}
//------------------------------------------------------------------------------
void CCapitalExecutionConnection::OnExecutionReport(TExecutionReportMessage* Msg, ExecDup PossDup)
{
    nsOrderMessageDefine::OrderStatusEnum rptOrderStatus = Msg->GetOrderStatus();
    UFC::AnsiString                       rptBrokerID = Msg->GetBrokerID();
    
    if (FExcludeBrokerIDSet.Exists(rptBrokerID))
    {
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " Drop execution report from Broker %s.", rptBrokerID.c_str());
        UFC::BufferedLog::FlushToFile();
        return;
    }
    
    switch (rptOrderStatus)
    {
        case nsOrderMessageDefine::osNew:
        case nsOrderMessageDefine::osReplaced:
        case nsOrderMessageDefine::osPartiallyFilled:
        case nsOrderMessageDefine::osFilled:
                                   AddExecution( Msg );
                                   break;
        case nsOrderMessageDefine::osQuoteAccept: 
        case nsOrderMessageDefine::osCanceled:                                   
                                   if( Msg->GetPositionEffect() == nsOrderMessageDefine::peTMPMarketMaker &&
                                       FNeedSendQuoteExecutionReport == false ) 
                                       return;        
                                   AddExecution( Msg );
                                   break;
        case nsOrderMessageDefine::osPendingNew:
        case nsOrderMessageDefine::osPendingReplace:
        case nsOrderMessageDefine::osPendingCancel:
        case nsOrderMessageDefine::osRejected:
        default: UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " Drop execution report %d(%s)", rptOrderStatus, DescribeExecRptOrderStatus(rptOrderStatus));
                 break;
    }  //switch (rptOrderStatus)
    UFC::BufferedLog::FlushToFile();
}  
//------------------------------------------------------------------------------
void CCapitalExecutionConnection::OnRecoverFinished(int Count)
{
    UFC::BufferedLog::Printf(" CaptialExecConnection::OnRecoverFinished()  %d reports recovered.", Count);
    UFC::BufferedLog::FlushToFile();
    UFC::SleepMS( 1000 );
}  //CCapitalExecutionConnection::OnRecoverFinished()

//------------------------------------------------------------------------------
void CCapitalExecutionConnection::Execute(void)
{
    while (!IsTerminated())
    {
        if (!FIsLogon)
            FTaifexConnectionPtr->Connect(FTaifexIP.c_str() , FTaifexPort, 10);
        UFC::SleepMS(10000);
    }  //while (!IsTerminated())
}  //CCapitalExecutionConnection::Execute()
//------------------------------------------------------------------------------
}  //namespace capital
//------------------------------------------------------------------------------
