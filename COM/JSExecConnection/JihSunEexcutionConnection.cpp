/* 
 * File:   JihSunEexcutionConnection.cpp
 * Author: Zhen Fan
 *
 * Created on 2012/06/27 11:01
 */
#include "JihSunEexcutionConnection.h"
#include "TMPMessage.h"  //TAIFEXLib
namespace jihsun
{
//------------------------ CCapitalExecutionConnection -------------------------
//------------------------------------------------------------------------------
CJSExecutionConnection::CJSExecutionConnection(const UFC::AnsiString& ConfigFileName,
                                               RecoverMarket ReceiveMarket, ConnectionType ExecutionType,
                                               Int32 RecoverBeginTime, 
                                               CDataExchangePool *DataPoolPtr)
:UFC::PThread(NULL, FALSE),
 FConfigFileName(ConfigFileName),
 FReceiveMarket(ReceiveMarket),
 FExecutionType(ExecutionType),
 FApplicationName("JSExecConnection"),
 FRecoverBeginTime(RecoverBeginTime),
 FDataPoolPtr(DataPoolPtr),
 FTaifexIP("127.0.0.1"),
 FTaifexPort(23456),
 FID("Admin"),
 FPassword("1234rewq"),
 FAccount("0000001"),
 FBrokerID("F020999"),
 FReconnectInterval(3000),
 FNeedSendQuoteExecutionReport(false),
 FTaifexConnectionPtr(0),
 FIsConnected(false),
 FIsLogon(false),
 FIsFirstRecover(true),
 FConnectCount(0),
 FLogonCount(0),
 FRecoverCount(0)
{
    LoadSetting();
    PrintSetting();
    FTaifexConnectionPtr = new TTaifexConnection(FApplicationName, this);
    FTaifexConnectionPtr->SetUseNewFuturesSymbol(TRUE);
    ///< Set the broker ID of this connection.
    FTaifexConnectionPtr->SetBrokerID(nsOrderMessageDefine::mTWFutures, FBrokerID.c_str());
    FTaifexConnectionPtr->SetCMID(FBrokerID.SubString(0, 4).c_str());
    ///< Load TAIFEX message format config files.
//    FTaifexConnectionPtr->SetTSEDataFormatResourceID( IDR_TSE_T010, IDR_TSE_O010, IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
//    FTaifexConnectionPtr->SetOTCDataFormatResourceID( IDR_OTC_T010, IDR_OTC_O010, IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
//    FTaifexConnectionPtr->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010, IDR_TSE_E010, IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020, IDR_TSE_Ex020, "INI" );
//    FTaifexConnectionPtr->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );
    FTaifexConnectionPtr->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FTaifexConnectionPtr->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );

    TMP::TMPMessage::InitTMPMessageLibrary(TMP::tlmBoth);
    ///< Start running thread.
    Start();
}  //CJSExecutionConnection::CJSExecutionConnection()

//------------------------------------------------------------------------------
CJSExecutionConnection::~CJSExecutionConnection()
{
}  //CJSExecutionConnection::~CJSExecutionConnection()

//------------------------------------------------------------------------------
void CJSExecutionConnection::LoadSetting()
{
    UFC::AnsiString logHead = "CJSExecutionConnection::LoadSetting()";
    UFC::UiniFile *ConfigFilePtr = 0;
    try 
    {
        ConfigFilePtr = new UFC::UiniFile( FConfigFileName );
    }
    catch (UFC::FileException fe)
    {
        UFC::BufferedLog::Printf( " %s [%s] Error: %s", logHead.c_str(), FConfigFileName.c_str(), fe.what() );
        ConfigFilePtr = NULL;
    }
    
    if (ConfigFilePtr != NULL)
    {
        UFC::AnsiString TempStr;
        if (ConfigFilePtr->GetValue( "Setting", "AppName", TempStr ) == FALSE )
            UFC::BufferedLog::Printf( " %s Application Name is Not Setting in ConfigFile, Use Default Value \"%s\".", logHead.c_str(), FApplicationName.c_str() );
        else
            FApplicationName = TempStr;
        
        if (ConfigFilePtr->GetValue( "Setting", "IP", TempStr ) == FALSE )
            UFC::BufferedLog::Printf( " %s Taifex IP is Not Setting in ConfigFile, Use Default Value \"%s\".", logHead.c_str(), FTaifexIP.c_str() );
        else
            FTaifexIP = TempStr;

        if (ConfigFilePtr->GetValue( "Setting", "Port", TempStr ) == FALSE )
            UFC::BufferedLog::Printf( " %s Taifex Port is Not Setting in ConfigFile, Use Default Value %d!", logHead.c_str(), FTaifexPort );
        else
            FTaifexPort = TempStr.ToInt();

        if (ConfigFilePtr->GetValue( "Setting", "ID", TempStr ) == FALSE )
            UFC::BufferedLog::Printf( " %s ID is Not Setting in ConfigFile, Use Default Value \"%s\".", logHead.c_str(), FID.c_str() );
        else
            FID = TempStr;

        if (ConfigFilePtr->GetValue( "Setting", "Password", TempStr ) == FALSE )
            UFC::BufferedLog::Printf( " %s Password is Not Setting in ConfigFile, Use Default Value \"%s\".", logHead.c_str(), FPassword.c_str() );
        else
            FPassword = TempStr;

        if (ConfigFilePtr->GetValue( "Setting", "Account", TempStr ) == FALSE )
            UFC::BufferedLog::Printf(" %s Account is Not Setting in ConfigFile, Use Default Value \"%s\".", logHead.c_str(), FAccount.c_str() );
        else
            FAccount = TempStr;

        if (ConfigFilePtr->GetValue( "Setting", "BrokerID", TempStr) == FALSE )
            UFC::BufferedLog::Printf( " %s BrokerID is Not Setting in ConfigFile, Use Default Value \"%s\".", logHead.c_str(), FBrokerID.c_str() );
        else
            FBrokerID = TempStr;

        if (ConfigFilePtr->GetValue( "Setting", "ReconnectInterval", TempStr) == FALSE )
            UFC::BufferedLog::Printf( " %s ReconnectInterval is Not Setting in ConfigFile, Use Default Value %d.", logHead.c_str(), FReconnectInterval );
        else
            FReconnectInterval = TempStr.ToInt();

        if (ConfigFilePtr->GetValue( "Setting", "SendQuoteExec", TempStr ) == FALSE )
            UFC::BufferedLog::Printf( " %s SendQuoteExec is Not Setting in ConfigFile, Use Default Value false", logHead.c_str() );
        else
        {
            if ( ( TempStr == "1" ) || ( TempStr == "Y" ) || ( TempStr == "y" ) )
                FNeedSendQuoteExecutionReport = true;
            else
                FNeedSendQuoteExecutionReport = false;
        }

        if (ConfigFilePtr->GetValue( "Setting", "ExcludeBrokerID", TempStr ) == TRUE )
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
            UFC::BufferedLog::Printf( " %s No Exclude BrokerID Setting in ConfigFile", logHead.c_str() );
        
        delete ConfigFilePtr;
    }  //if (ConfigFilePtr != NULL)
    UFC::BufferedLog::FlushToFile();    
}  //CJSExecutionConnection::LoadSetting()

//------------------------------------------------------------------------------
void CJSExecutionConnection::Recover(const UFC::AnsiString& BeginTime)
{
    if (FIsLogon == TRUE)
    {
        UFC::BufferedLog::Printf(" Recover from: %s", BeginTime.c_str());
        RecoverDataType recoverType;
        if (FExecutionType == ctBoth)
            recoverType = rdBoth;
        else if (FExecutionType == ctRecvFillOnly)
            recoverType = rdFill;
        else if (FExecutionType == ctRecvComfirmOnly)
            recoverType = rdConfirm;
        else
            return;
        FTaifexConnectionPtr->Recover(BeginTime.c_str(), recoverType, FReceiveMarket);
    }
}  //CCapitalExecutionConnection::Recover()

//------------------------------------------------------------------------------
void CJSExecutionConnection::PrintSetting()
{
    UFC::BufferedLog::Printf( " -------------------------------  CJSExecutionConnection  -------------------------------");
    UFC::BufferedLog::Printf( " [%s] Recover Begin[%d]", FApplicationName.c_str(), FRecoverBeginTime );
    UFC::BufferedLog::Printf( " Config File[%s]", FConfigFileName.c_str() );
    UFC::BufferedLog::Printf( " Taifex IP:Port [%s:%d]", FTaifexIP.c_str(), FTaifexPort );
    UFC::BufferedLog::Printf( " ID[%s] Password[%s] Account[%s] BrokerID[%s]", FID.c_str(), FPassword.c_str(), FAccount.c_str(), FBrokerID.c_str() );
    if ( FNeedSendQuoteExecutionReport )
        UFC::BufferedLog::Printf( " Need Send Quote Execution Report" );
    else
        UFC::BufferedLog::Printf( " Do Not Send Quote Execution Report" );
    
    UFC::AnsiString ExcludeBrokerIDStr = "";
    for (int i = 0; i < FExcludeBrokerIDSet.ItemCount(); i++)
    {
        if (ExcludeBrokerIDStr.Length() > 0) ExcludeBrokerIDStr = ExcludeBrokerIDStr + ", ";
        ExcludeBrokerIDStr = ExcludeBrokerIDStr + FExcludeBrokerIDSet.GetItem(i);
    }
    if (ExcludeBrokerIDStr.Length() > 0)
        UFC::BufferedLog::Printf( " Exclude Broker ID: %s", ExcludeBrokerIDStr.c_str() );
    else
        UFC::BufferedLog::Printf( " No Exclude Broker ID." );
    
    UFC::BufferedLog::Printf( " -------------------------------  CJSExecutionConnection  -------------------------------" );
    UFC::BufferedLog::Printf( " " );
    UFC::BufferedLog::FlushToFile();    
}  //CJSExecutionConnection::PrintSetting()

//------------------------------------------------------------------------------
void CJSExecutionConnection::OnConnected(void)
{
    FConnectCount++;
    UFC::BufferedLog::Printf( " OnConnected() %s %dth Connect to [%s:%d].", FApplicationName.c_str(), FConnectCount, FTaifexIP.c_str(), FTaifexPort);
    UFC::BufferedLog::FlushToFile();
    FTaifexConnectionPtr->Logon(FID.c_str(), FPassword.c_str(), FAccount.c_str());
}  //CCapitalExecutionConnection::OnConnected()

//------------------------------------------------------------------------------
void CJSExecutionConnection::OnDisconnected(void)
{
    FIsLogon = false;
    UFC::BufferedLog::Printf( " OnDisconnected() %s disconnect from [%s:%d].", FApplicationName.c_str(), FTaifexIP.c_str(), FTaifexPort);
    UFC::BufferedLog::FlushToFile();
}  //CCapitalExecutionConnection::OnDisconnected()

//------------------------------------------------------------------------------
void CJSExecutionConnection::OnLogonReply(const char* ReplyString, LogonResult Result, int CID)
{
    if ( Result == lrOk )
    {
        FLogonCount++;
        FIsLogon = true;
        UFC::BufferedLog::Printf(" OnLogonReply() %s(CID=%d) ID[%s] Account[%s] %dth logon OK.", ReplyString, CID, FID.c_str(), FAccount.c_str(), FLogonCount);
        UFC::AnsiString beginTime = "000000";
        Int32 tandemMsgCount = FDataPoolPtr->GetTandemMsgArrayCount(true);
        if (tandemMsgCount > 0)
        {
            CTandemMessage *lastTandemMsgPtr = FDataPoolPtr->GetTandemMsgFromArray(tandemMsgCount - 1, true);
            if (lastTandemMsgPtr != 0)
            {
                UFC::AnsiString msgTransactTime = lastTandemMsgPtr->GetTransactTime();
                if (msgTransactTime.Length() == 6) beginTime = msgTransactTime;
            }
        }
        Recover(beginTime);
        UFC::BufferedLog::FlushToFile();
    }
    else
    {
        UFC::BufferedLog::Printf(" OnLogonReply() %s ID[%s], Account[%s] logon failed.", ReplyString, FID.c_str(), FAccount.c_str());
        UFC::BufferedLog::FlushToFile();
        exit(0);
    }    
}  //CJSExecutionConnection::OnLogonReply()

//------------------------------------------------------------------------------
void CJSExecutionConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup ) 
{
    Int32 rptNID = Msg->GetNID();
    nsOrderMessageDefine::OrderStatusEnum rptOrderStatus = Msg->GetOrderStatus();
    nsOrderMessageDefine::MarketEnum rptMarket = Msg->GetMarket();
    UFC::AnsiString rptBrokerID = Msg->GetBrokerID();
    UFC::AnsiString rptExecId = Msg->GetExecID();
    UFC::AnsiString rptOrderId = Msg->GetOrderID();
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " Process ExecRpt[%s] OrderId[%s] Market[%d(%s)] NID[%d] OrderStatus[%d(%s)].", rptExecId.c_str(), rptOrderId.c_str(), rptMarket, DescribeExecRptMarket(rptMarket), rptNID, rptOrderStatus, DescribeExecRptOrderStatus(rptOrderStatus));
    if (FExcludeBrokerIDSet.Exists(rptBrokerID))
    {
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " Ignore execution report from Broker %s.", rptBrokerID.c_str());
        UFC::BufferedLog::FlushToFile();
        return;
    }

    if (((rptMarket != nsOrderMessageDefine::mTWFutures) && (rptMarket != nsOrderMessageDefine::mTWOptions)) ||
        ((rptMarket == nsOrderMessageDefine::mTWFutures) && ((FReceiveMarket != rmAll) && (FReceiveMarket != rmFutures))) ||
        ((rptMarket == nsOrderMessageDefine::mTWOptions) && ((FReceiveMarket != rmAll) && (FReceiveMarket != rmOption))))
    {
        UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " Ignore execution report from Market %d(%s). NID=%d, ExecID[%s].", rptMarket, DescribeExecRptMarket(rptMarket), rptNID, rptExecId.c_str());
        UFC::BufferedLog::FlushToFile();
        return;
    }
    
    CTandemMessage *tandemMsgPtr = FDataPoolPtr->FindTandemMsgByExecID(rptExecId, true);  //Do Lock
    if (tandemMsgPtr != 0)
    {
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " Duplicate execution report from Market %d(%s). NID=%d, ExecID[%s].", rptMarket, DescribeExecRptMarket(rptMarket), rptNID, rptExecId.c_str());
        UFC::BufferedLog::FlushToFile();
        return;
    }
    
    UFC::AnsiString prefixStr = "";
    nsOrderMessageDefine::TMPExecTypeEnum rptTMPExecType = Msg->GetTMPExecType();
    switch (rptOrderStatus)
    {
        case nsOrderMessageDefine::osQuoteAccept:
            if ( FNeedSendQuoteExecutionReport && 
                 ((FExecutionType == ctBoth) || (FExecutionType == ctRecvComfirmOnly)))
            {
                tandemMsgPtr = new CTandemMessage( Msg );
                prefixStr = "Quote";
            }  //if ( FNeedSendQuoteExecutionReport )
            break;
        case nsOrderMessageDefine::osNew:
            if (rptTMPExecType != nsOrderMessageDefine::tetNewAndFilled)
            {
                if ((FExecutionType == ctBoth) || (FExecutionType == ctRecvComfirmOnly))
                {
                    tandemMsgPtr = new CTandemMessage( Msg );
                    prefixStr = "Confirm";
                }  //if ((rptOrderStatus != nsOrderMessageDefine::osQuoteAccept) || FNeedSendQuoteExecutionReport)
            }
            else
                UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " Ignore execution report %d(%s), TMPExecType'%c', NID=%d, ExecID[%s].", rptOrderStatus, DescribeExecRptOrderStatus(rptOrderStatus), rptTMPExecType, rptNID, rptExecId.c_str());
            break;
        case nsOrderMessageDefine::osReplaced:
        case nsOrderMessageDefine::osCanceled:
            if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifIOC &&
                Msg->GetCumQty() > 0 )
                return; ///< Skip IOC partially filled cancel.               
            //if( Msg->GetTMPStstusCode() == 47 && Msg->GetReportSequence() == 0  )
            //    return; ///< Skip fate cancel.               
            if ((FExecutionType == ctBoth) || (FExecutionType == ctRecvComfirmOnly))
            {
                tandemMsgPtr = new CTandemMessage( Msg );
                prefixStr = "Confirm";
            }  //if ((rptOrderStatus != nsOrderMessageDefine::osQuoteAccept) || FNeedSendQuoteExecutionReport)
            break;
        case nsOrderMessageDefine::osPartiallyFilled:
        case nsOrderMessageDefine::osFilled:
            if ((FExecutionType == ctBoth) || (FExecutionType == ctRecvFillOnly))
            {                
                tandemMsgPtr = new CTandemMessage( Msg );
                prefixStr = "Fill";
            }
            break;
        case nsOrderMessageDefine::osPendingNew:
        case nsOrderMessageDefine::osPendingReplace:
        case nsOrderMessageDefine::osPendingCancel:
        case nsOrderMessageDefine::osRejected:
        default:
            UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " Ignore execution report OrderStatus[%d(%s)], NID=%d, ExecID[%s].", rptOrderStatus, DescribeExecRptOrderStatus( rptOrderStatus ), rptNID, rptExecId.c_str());
    }  //switch (rptOrderStatus)
    
    if (tandemMsgPtr != 0)
    {
        if (FDataPoolPtr != 0)
        {
            if (FDataPoolPtr->InsertTandemMsgExecIDIndex(tandemMsgPtr, false, true))  //Do not Check, Do Lock
            {
                if (FDataPoolPtr->InsertTandemMsgArray(tandemMsgPtr, true))
                {
                    if (!FIsFirstRecover) FDataPoolPtr->PushTandemMsg(tandemMsgPtr, true);
                }
                else
                    UFC::BufferedLog::Printf(" OnExecutionReport() Insert Tandem Message Array Failed. %s[%s]", rptExecId.c_str(), tandemMsgPtr->GetTandemData());
            }
            else
            {
                UFC::BufferedLog::Printf(" OnExecutionReport() Insert Exec ID Index Failed. %s[%s]", rptExecId.c_str(), tandemMsgPtr->GetTandemData());
                delete tandemMsgPtr;
            }
        }
        else
        {
            UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " %s [%s]", prefixStr.c_str(), tandemMsgPtr->GetTandemData());
            delete tandemMsgPtr;
        }  //if (FDataPoolPtr != 0)
    }  //if (tandemMsgPtr != 0)
    else
        UFC::BufferedLog::Printf(" OnExecutionReport() Tandem Message is null.");
        
    UFC::BufferedLog::FlushToFile();
    return;
}  //CJSExecutionConnection::OnExecutionReport()

//------------------------------------------------------------------------------
void CJSExecutionConnection::OnRecoverFinished(int Count)
{
    ++FRecoverCount;
    UFC::BufferedLog::Printf(" CJSExecutionConnection::OnRecoverFinished() %dth recover %d reports.", FRecoverCount, Count);
    UFC::BufferedLog::FlushToFile();
    if (FIsFirstRecover) FIsFirstRecover = false;
    UFC::SleepMS(1000);
    LockForRecover();
    SignalRecoverWaiting();
    UnlockForRecover();
}  //CJSExecutionConnection::OnRecoverFinished()

//------------------------------------------------------------------------------
void CJSExecutionConnection::Execute(void)
{
    while (!IsTerminated())
    {
        if (!FIsLogon)
        {
            UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " CJSExecutionConnection::Execute() Do connect.");
            FTaifexConnectionPtr->Connect(FTaifexIP.c_str(), FTaifexPort, 10);
        }
        UFC::SleepMS(FReconnectInterval);
    }  //while (!IsTerminated())
}  //CJSExecutionConnection::Execute()
//------------------------------------------------------------------------------
}  //namespace jihsun
