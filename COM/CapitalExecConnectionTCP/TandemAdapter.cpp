/* 
 * File:   TandemAdapter.cpp
 * Author: Zhen Fan
 *
 * Created on 2011/12/26  9:50
 */

#include <cstring>
#include "TandemAdapter.h"

namespace capital
{
//------------------------------- CTandemAdapter -------------------------------
//------------------------------------------------------------------------------
CTandemAdapter::CTandemAdapter(nsOrderMessageDefine::MarketEnum Market, bool IsFillMessageAdapter,
                               const UFC::AnsiString& TandemServerAddress, Int32 TandemServerPort, 
                               CDataExchangePool *MsgPoolPtr,
                               Int32 HeartbeatInterval, Int32 CheckInterval)
:UFC::PThread(NULL, FALSE),
 FMarket(Market),
 FIsFillMessageAdapter(IsFillMessageAdapter),
 FTandemServerAddress(TandemServerAddress),
 FTandemServerPort(TandemServerPort),
 FMsgPoolPtr(MsgPoolPtr),
 FHeartbeatInterval(HeartbeatInterval),
 FCheckInterval(CheckInterval),
 FTandemConnectionPtr(0)
{//mTWOptions
    if (FMarket == nsOrderMessageDefine::mTWFutures)
    {
        if (IsFillMessageAdapter)
            FAdapterDescription = "Fill";
        else
            FAdapterDescription = "Confirm";
    }
    else if (FMarket == nsOrderMessageDefine::mTWOptions)
    {
        if (IsFillMessageAdapter)
            FAdapterDescription = "Fill_OPT";
        else
            FAdapterDescription = "Confirm_OPT";
    }
    else
    {
        if (IsFillMessageAdapter)
            FAdapterDescription.Printf("Fill_%s", DescribeExecRptMarket(FMarket));
        else
            FAdapterDescription.Printf("Confirm_%s", DescribeExecRptMarket(FMarket));
    }
    FDisconnectTimeStr = "";
    FMaxWaitCount = FHeartbeatInterval / FCheckInterval;
    if ((FHeartbeatInterval % FCheckInterval) > 0) FMaxWaitCount++;
    FWaitCount = 0;
    CreateConnection();
}  //CTandemAdapter::CTandemAdapter()
    
//------------------------------------------------------------------------------
CTandemAdapter::~CTandemAdapter()
{
    if (FTandemConnectionPtr != 0)
    {
        if (FTandemConnectionPtr->IsConnect()) FTandemConnectionPtr->Disconnect(FALSE);
        delete FTandemConnectionPtr;
    }
}  //CTandemAdapter::~CTandemAdapter()

//------------------------------------------------------------------------------
bool CTandemAdapter::CreateConnection()
{
    try 
    {
        FTandemConnectionPtr = new UFC::PClientSocket(FTandemServerAddress, FTandemServerPort, FALSE);
        FTandemConnectionPtr->SetListener(this);
        return true;
    }
    catch (UFC::SocketException& e)
    {
        if (FTandemConnectionPtr != 0) delete FTandemConnectionPtr;
        FTandemConnectionPtr = 0;
        UFC::BufferedLog::Printf(" CTandemAdapter::CreateConnection() %s Create UFC::PClientSocket Error: %s", FAdapterDescription.c_str(), e.what());
        UFC::BufferedLog::FlushToFile();
    }
    return false;
}  //CTandemAdapter::CreateConnection()

//------------------------------------------------------------------------------
UFC::AnsiString CTandemAdapter::GetDisconnectTime() 
{
    FTimeLocker.Acquire();
    UFC::AnsiString DisconnectTime = FDisconnectTimeStr;
    FTimeLocker.Release();
    return DisconnectTime;
}  //CTandemAdapter::GetDisconnectTime()

//------------------------------------------------------------------------------
void CTandemAdapter::SetDisconnectTime(const UFC::AnsiString& DisconnectTime) 
{
    FTimeLocker.Acquire();
    FDisconnectTimeStr = DisconnectTime;
    FTimeLocker.Release();
}  //CTandemAdapter::SetDisconnectTime()

//------------------------------------------------------------------------------
void CTandemAdapter::ClearDisconnectTime() 
{
    FTimeLocker.Acquire();
    FDisconnectTimeStr = "";
    FTimeLocker.Release();
}  //CTandemAdapter::ClearDisconnectTime()

//------------------------------------------------------------------------------
bool CTandemAdapter::IsConnected()
{
    if ((FTandemConnectionPtr != 0) && (FTandemConnectionPtr->IsConnect() == TRUE))
        return true;
    else
        return false;
}  //CTandemAdapter::IsConnected()

//------------------------------------------------------------------------------
bool CTandemAdapter::Connect()
{
    bool IsSuccess = true;
    if (IsConnected()) 
    {
        UFC::BufferedLog::Printf(" CTandemAdapter::Connect() %s has Connected to [%s:%d]!", FAdapterDescription.c_str(), FTandemServerAddress.c_str(), FTandemServerPort);
        UFC::BufferedLog::FlushToFile();
        return IsSuccess;
    }
    
    try
    {
        FTandemConnectionPtr->Connect();
    }
    catch (UFC::SocketException& e)
    {
        UFC::BufferedLog::Printf(" CTandemAdapter::Connect() %s connect to [%s:%d] Exception:[%s]", FAdapterDescription.c_str(), FTandemServerAddress.c_str(), FTandemServerPort, e.what());
        UFC::BufferedLog::FlushToFile();
        IsSuccess = false;
        if (FMsgPoolPtr != 0)
        {
            UFC::AnsiString *AlertMsgPtr = new UFC::AnsiString();
            AlertMsgPtr->Printf("%s Connection broken [%s:%d]", FAdapterDescription.c_str(), FTandemServerAddress.c_str(), FTandemServerPort);    
            FMsgPoolPtr->PushAlertMsg(AlertMsgPtr, true);
        }
    }
    return IsSuccess;
}  //CTandemAdapter::Connect()

//------------------------------------------------------------------------------
bool CTandemAdapter::DisConnect()
{
    bool IsSuccess = true;
    if (!IsConnected()) 
    {
        UFC::BufferedLog::Printf(" CTandemAdapter::DisConnect() %s is not Connected!", FAdapterDescription.c_str());
        UFC::BufferedLog::FlushToFile();
        return IsSuccess;
    }

    try
    {
        FTandemConnectionPtr->Disconnect(TRUE);  //TriggerOnDisconnectEvent
    }
    catch (UFC::SocketException& e)
    {
        UFC::BufferedLog::Printf(" CTandemAdapter::DisConnect() %s Exception:[%s]", FAdapterDescription.c_str(), e.what());
        UFC::BufferedLog::FlushToFile();
        IsSuccess = false;
    }
    return IsSuccess;
}  //CTandemAdapter::DisConnect()
    
//------------------------------------------------------------------------------
void CTandemAdapter::OnConnect(UFC::PClientSocket *Socket)
{   //Implement UFC::SocketClientListener
    UFC::AnsiString *AlertMsgPtr = new UFC::AnsiString();
    AlertMsgPtr->Printf("%s Connection established [%s:%d]", FAdapterDescription.c_str(), Socket->GetPeerIPAddress().c_str(), Socket->GetPort());
    UFC::BufferedLog::Printf(" %s", AlertMsgPtr->c_str());
    UFC::BufferedLog::FlushToFile();
    
    FTimeLocker.Acquire();
    FWaitCount = 0;
    FTimeLocker.Release();

    if (FMsgPoolPtr != 0)
        FMsgPoolPtr->PushAlertMsg(AlertMsgPtr, true);
    else
        delete AlertMsgPtr;
}  //CTandemAdapter::OnConnect()

//------------------------------------------------------------------------------
void CTandemAdapter::OnDisconnect(UFC::PClientSocket *Socket)
{   //Implement UFC::SocketClientListener
    UFC::AnsiString *AlertMsgPtr = new UFC::AnsiString();
    AlertMsgPtr->Printf("%s Connection broken [%s:%d]", FAdapterDescription.c_str(), Socket->GetPeerIPAddress().c_str(), Socket->GetPort());    
    UFC::BufferedLog::Printf(" %s", AlertMsgPtr->c_str());
    UFC::BufferedLog::FlushToFile();
    
    UFC::UTime DisConnectTme;
    DisConnectTme.setCurrent();
    DisConnectTme -= 60;
    FTimeLocker.Acquire();
    FDisconnectTimeStr.Printf("%02d%02d%02d", DisConnectTme.getHour(), DisConnectTme.getMinute(), DisConnectTme.getSecond());
    FTimeLocker.Release();
    
    if (FMsgPoolPtr != 0)
        FMsgPoolPtr->PushAlertMsg(AlertMsgPtr, true);
    else
        delete AlertMsgPtr;
}  //CTandemAdapter::OnDisonnect()

//------------------------------------------------------------------------------
BOOL CTandemAdapter::OnDataArrived(UFC::PClientSocket *Socket)
{   //Implement UFC::SocketClientListener
    Int32 RecvDataSize = 0;
    if (Socket == 0) return FALSE;
    
    char RecvBuffer[FReceiveBufferSize + 1];
    std::memset(RecvBuffer, 0, FReceiveBufferSize);
    try 
    {
        RecvDataSize = Socket->Receive((UInt8 *)RecvBuffer, FReceiveBufferSize);
        if (RecvDataSize < FReceiveBufferSize)
            RecvBuffer[RecvDataSize] = 0;
        else
            RecvBuffer[FReceiveBufferSize] = 0;
        UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " CTandemAdapter::OnDataArrived() %s received %d bytes data. [%s]", FAdapterDescription.c_str(), RecvBuffer);
    } 
    catch (UFC::SocketException e)
    {
        UFC::BufferedLog::Printf(" CTandemAdapter::OnDataArrived() %s SocketException:[%s]", FAdapterDescription.c_str(), e.what());
    }  //try
    
    UFC::BufferedLog::FlushToFile();
    return TRUE;
}  //CTandemAdapter::OnDataArrived()

//------------------------------------------------------------------------------
void CTandemAdapter::OnIdle(UFC::PClientSocket *Socket)
{   //Implement UFC::SocketClientListener
    return;
}  //CTandemAdapter::OnIdle()

//------------------------------------------------------------------------------
bool CTandemAdapter::ReceiveData(Int8 *DataBuffer, Int32 DataSize)
{
    bool IsSuccess = false;
    if (!IsConnected() || (DataBuffer == 0) || (DataSize <= 0)) return IsSuccess;
    try 
    {
        FTandemConnectionPtr->BlockRecv((UInt8*)DataBuffer, DataSize);
        IsSuccess = true;
    }
    catch (UFC::SocketException e)
    {
        UFC::BufferedLog::DebugPrintf(" CTandemAdapter::ReceiveData() %s Exception: %s", FAdapterDescription.c_str(), e.what());
        UFC::BufferedLog::FlushToFile();
    }
    return IsSuccess;
}  //CTandemAdapter::ReceiveData()

//------------------------------------------------------------------------------
bool CTandemAdapter::SendData(Int8 *DataBuffer, Int32 DataSize)
{
    bool IsSuccess = false;
    if (!IsConnected() || (DataBuffer == 0) || (DataSize <= 0)) return IsSuccess;
    try 
    {
        FTandemConnectionPtr->BlockSend(DataBuffer, DataSize);
        IsSuccess = true;
    }
    catch (UFC::SocketException e)
    {
        UFC::BufferedLog::DebugPrintf(" CTandemAdapter::SendData() %s Exception: %s", FAdapterDescription.c_str(), e.what());
        UFC::BufferedLog::FlushToFile();
    }
    return IsSuccess;
}  //CTandemAdapter::SendData()

//------------------------------------------------------------------------------
bool CTandemAdapter::DoFillHeartbeat()
{
    Int8 HeartbeatBuffer[FFillHeartbeatBufferSize + 1];
    std::memset(HeartbeatBuffer, ' ', FFillHeartbeatBufferSize);
    HeartbeatBuffer[FFillHeartbeatBufferSize] = 0;
    std::memcpy(HeartbeatBuffer, "18888888", 8);
    
    bool IsHeartbeatSuccess = false;
    UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " %s HeartbeatFill begin", FAdapterDescription.c_str());
    if (SendData(HeartbeatBuffer, FFillHeartbeatBufferSize))
    {
        Int8 EchoBuffer[FFillHeartbeatBufferSize + 1];
        std::memset(EchoBuffer, ' ', FFillHeartbeatBufferSize);
        if (ReceiveData(EchoBuffer, FFillHeartbeatBufferSize)) 
        {
            EchoBuffer[FConfirmHeartbeatBufferSize] = 0;
            IsHeartbeatSuccess = true;
        }
    }
    UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " %s HeartbeatFill send", FAdapterDescription.c_str());
    return IsHeartbeatSuccess;
}  //CTandemAdapter::SendFillHeartbeat()

//------------------------------------------------------------------------------
bool CTandemAdapter::DoConfirmHeartbeat()
{
    Int8 HeartbeatBuffer[FConfirmHeartbeatBufferSize + 1];
    std::memset(HeartbeatBuffer, ' ', FConfirmHeartbeatBufferSize);
    HeartbeatBuffer[FConfirmHeartbeatBufferSize] = 0;
    std::memcpy(HeartbeatBuffer, "18888888", 8);
    
    bool IsHeartbeatSuccess = false;
    UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " %s HeartbeatConfirm begin", FAdapterDescription.c_str());
    if (SendData(HeartbeatBuffer, FConfirmHeartbeatBufferSize))
    {
        Int8 EchoBuffer[FConfirmHeartbeatBufferSize + 1];
        std::memset(EchoBuffer, ' ', FConfirmHeartbeatBufferSize);
        if (ReceiveData(EchoBuffer, FConfirmHeartbeatBufferSize)) 
        {
            EchoBuffer[FConfirmHeartbeatBufferSize] = 0;
            IsHeartbeatSuccess = true;
        }
    }
    UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " %s HeartbeatConfirm send", FAdapterDescription.c_str());
    return IsHeartbeatSuccess;
}  //CTandemAdapter::DoConfirmHeartbeat()

//------------------------------------------------------------------------------
bool CTandemAdapter::DoHeartbeat()
{
    if (FIsFillMessageAdapter)
        return DoFillHeartbeat();
    else
        return DoConfirmHeartbeat();
}  //CTandemAdapter::SendHeartbeat()

//------------------------------------------------------------------------------
void CTandemAdapter::Execute(void)
{   //Implement UFC::PThread interface
    while(!IsTerminated())
    {
        if (!IsConnected()) 
        {
            if (FTandemConnectionPtr != 0) delete FTandemConnectionPtr;
            if (CreateConnection()) Connect();
            UFC::SleepMS(2 * FCheckInterval);
        }  //if (!IsConnected())
        
        if (IsConnected()) 
        {
            FTimeLocker.Acquire();
            bool IsNeedHeartBeat = (FWaitCount >= FMaxWaitCount);
            if (IsNeedHeartBeat) FWaitCount = 0;
            FTimeLocker.Release();
            if (IsNeedHeartBeat) 
                if (!DoHeartbeat()) DisConnect();
        }  //if (IsConnected())
        UFC::SleepMS(FCheckInterval);
        FTimeLocker.Acquire();
        FWaitCount++;
        FTimeLocker.Release();
    }  //while(!IsTerminated())
}  //CTandemAdapter::Execute()
}  //namespace capital

