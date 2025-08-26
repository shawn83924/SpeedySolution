/* 
 * File:   TandemAdapter.cpp
 * Author: Zhen Fan
 *
 * Created on 2012/06/28 10:24
 */

#include <cstdio>
#include <cstring>
#include "TandemAdapter.h"

namespace jihsun
{
//------------------------------- CTandemAdapter -------------------------------
//------------------------------------------------------------------------------
CTandemAdapter::CTandemAdapter(const UFC::AnsiString& TandemServerAddress, Int32 TandemServerPort, 
                               CDataExchangePool *MsgPoolPtr,
                               Int32 HeartbeatInterval, Int32 CheckInterval)
:UFC::PThread(NULL, FALSE),
 FTandemServerAddress(TandemServerAddress),
 FTandemServerPort(TandemServerPort),
 FMsgPoolPtr(MsgPoolPtr),
 FHeartbeatInterval(HeartbeatInterval),
 FCheckInterval(CheckInterval),
 FTandemConnectionPtr(0)
{
    FDisconnectTimeStr = "";
    FMaxWaitCount = FHeartbeatInterval / FCheckInterval;
    if ( ( FHeartbeatInterval % FCheckInterval ) > 0 ) FMaxWaitCount++;
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
        FTandemConnectionPtr = new UFC::PClientSocket(FTandemServerAddress, FTandemServerPort, TRUE);
        FTandemConnectionPtr->SetListener(this);
        return true;
    }
    catch (UFC::SocketException& e)
    {
        if (FTandemConnectionPtr != 0) delete FTandemConnectionPtr;
        FTandemConnectionPtr = 0;
        UFC::BufferedLog::Printf(" CTandemAdapter::CreateConnection() Create UFC::PClientSocket Error: %s", e.what());
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
    if ( ( FTandemConnectionPtr != 0 ) && ( FTandemConnectionPtr->IsConnect() == TRUE ) )
        return true;
    else
        return false;
}  //CTandemAdapter::IsConnected()

//------------------------------------------------------------------------------
bool CTandemAdapter::Connect()
{
    bool IsSuccess = true;
    if ( IsConnected() ) 
    {
        UFC::BufferedLog::Printf(" CTandemAdapter::Connect() Connected to [%s:%d]!", FTandemServerAddress.c_str(), FTandemServerPort);
        UFC::BufferedLog::FlushToFile();
        return IsSuccess;
    }
    
    try
    {
        FTandemConnectionPtr->Connect();
        FTandemConnectionPtr->SetBlockMode(FALSE);
        FTandemConnectionPtr->SetTCPNoDelay(TRUE);
    }
    catch (UFC::SocketException& e)
    {
        UFC::BufferedLog::Printf(" CTandemAdapter::Connect() connect to [%s:%d] Exception:[%s]", FTandemServerAddress.c_str(), FTandemServerPort, e.what());
        UFC::BufferedLog::FlushToFile();
        IsSuccess = false;
        if( FMsgPoolPtr != 0 )
        {
            UFC::AnsiString *AlertMsgPtr = new UFC::AnsiString();
            AlertMsgPtr->Printf("Connection broken [%s:%d]", FTandemServerAddress.c_str(), FTandemServerPort);    
            FMsgPoolPtr->PushAlertMsg(AlertMsgPtr, true);
        }
    }
    return IsSuccess;
}  //CTandemAdapter::Connect()

//------------------------------------------------------------------------------
bool CTandemAdapter::DisConnect()
{
    bool IsSuccess = true;
    if ( !IsConnected() ) 
    {
        UFC::BufferedLog::Printf( " CTandemAdapter::DisConnect() is not Connected!" );
        UFC::BufferedLog::FlushToFile();
        return IsSuccess;
    }

    try
    {
        FTandemConnectionPtr->Disconnect( TRUE );  //TriggerOnDisconnectEvent
    }
    catch (UFC::SocketException& e)
    {
        UFC::BufferedLog::Printf( " CTandemAdapter::DisConnect() Exception:[%s]", e.what() );
        UFC::BufferedLog::FlushToFile();
        IsSuccess = false;
    }
    return IsSuccess;
}  //CTandemAdapter::DisConnect()
    
//------------------------------------------------------------------------------
void CTandemAdapter::OnConnect(UFC::PClientSocket *Socket)
{   //Implement UFC::SocketClientListener
    UFC::AnsiString *AlertMsgPtr = new UFC::AnsiString();
    AlertMsgPtr->Printf("Connection established [%s:%d]", Socket->GetPeerIPAddress().c_str(), Socket->GetSocketPort());
    UFC::BufferedLog::Printf(" CTandemAdapter::OnConnect() %s", AlertMsgPtr->c_str());
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
void CTandemAdapter::OnDisconnect(UFC::PClientSocket *Socket, BOOL NeedReconnect )
{   //Implement UFC::SocketClientListener
    UFC::AnsiString *AlertMsgPtr = new UFC::AnsiString();
    AlertMsgPtr->Printf("Connection broken [%s:%d]", Socket->GetPeerIPAddress().c_str(), Socket->GetPort());    
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
bool CTandemAdapter::ReadTelegraphLength(UFC::PClientSocket *SocketPtr, Int16& TelegraphLength)
{
    TelegraphLength = 0;
    bool IsReceiveSuccess = false;
    int BufferSize = sizeof(unsigned short) + 1;
    char RecvHeadBuffer[BufferSize];
    std::memset(RecvHeadBuffer, 0, BufferSize);
    try 
    {
        SocketPtr->BlockRecv((UInt8 *)RecvHeadBuffer, sizeof(unsigned short));
        IsReceiveSuccess = true;
    }
    catch (UFC::SocketException e)
    {
        UFC::BufferedLog::Printf(" CTandemAdapter::ReadTelegraphLength() SocketException:[%s]", e.what());
        UFC::BufferedLog::FlushToFile();
    }  //try
    
    if (IsReceiveSuccess)
    {
        unsigned short tempData;
        std::memcpy(&tempData, RecvHeadBuffer, sizeof(unsigned short));
        TelegraphLength = ntohs(tempData);
    }  //if (IsReceiveSuccess)
    return IsReceiveSuccess;
}  //CTandemAdapter::ReadTelegraphLength()

//------------------------------------------------------------------------------
bool CTandemAdapter::ReadTelegraphBody(UFC::PClientSocket *SocketPtr, Int16 TelegraphLength, UFC::AnsiString& TelegraphBodyStr)
{
    TelegraphBodyStr = "";
    int BufferSize = TelegraphLength + 1;
    char telegraphBuffer[BufferSize];
    std::memset(telegraphBuffer, 0, BufferSize);
    bool IsReceiveSuccess = false;
    try 
    {
        SocketPtr->BlockRecv((UInt8 *)telegraphBuffer, TelegraphLength);
        IsReceiveSuccess = true;
    }
    catch (UFC::SocketException e)
    {
        UFC::BufferedLog::Printf(" CTandemAdapter::ReadTelegraphBody() SocketException:[%s]", e.what());
        UFC::BufferedLog::FlushToFile();
    }  //try

    if (IsReceiveSuccess)
    {
        telegraphBuffer[TelegraphLength] = 0;
        TelegraphBodyStr = telegraphBuffer;
    }  //if (IsReceiveSuccess)
    return IsReceiveSuccess;
}  //CTandemAdapter::ReadTelegraphBody()

//------------------------------------------------------------------------------
CTandemTelegraph *CTandemAdapter::ReceiveTelegraph(UFC::PClientSocket *SocketPtr)
{
    CTandemTelegraph *telegraphPtr = 0;
    Int16 telegraphLength;
    if (ReadTelegraphLength(SocketPtr, telegraphLength))
    {       
        UFC::AnsiString telegraphBodyStr = "";
        if (ReadTelegraphBody(SocketPtr, telegraphLength, telegraphBodyStr))
            telegraphPtr = new CTandemTelegraph(telegraphLength, telegraphBodyStr);
    }  //if (ReadTelegraphLength(SocketPtr, telegraphLength))
    return telegraphPtr;
}  //CTandemTelegraph *ReceiveTelegraph()

//------------------------------------------------------------------------------
CTandemTelegraph *CTandemAdapter::ReceiveTelegraph()
{
    return ReceiveTelegraph(FTandemConnectionPtr);
}  //CTandemAdapter::ReceiveTelegraph()

//------------------------------------------------------------------------------
bool CTandemAdapter::SendTelegraph(CTandemTelegraph *TelegraphPtr)
{
    bool IsSuccess = false;
    if (!IsConnected() || (TelegraphPtr == 0)) return IsSuccess;
    FTandemConnectionLocker.Acquire();
    try 
    {
        
        FTandemConnectionPtr->BlockSend((Int8 *)TelegraphPtr->GetTelegraphBuffer(), TelegraphPtr->GetTelegraphLength() + sizeof(unsigned short));
        IsSuccess = true;
    }
    catch (UFC::SocketException e)
    {
        UFC::BufferedLog::Printf(" CTandemAdapter::SendTelegraph() Exception: %s", e.what());
        UFC::BufferedLog::FlushToFile();
    }
    FTandemConnectionLocker.Release();
    return IsSuccess;
}  //CTandemAdapter::SendTelegraph()

//------------------------------------------------------------------------------
void CTandemAdapter::ProcessCNTTelegraph(CTandemTelegraph *TelegraphPtr)
{
    if (FMsgPoolPtr == 0) return;
    UFC::AnsiString recvCommand = TelegraphPtr->GetCommand();
    Int32 recvMsgCount = TelegraphPtr->GetNumber(); 
    if (recvMsgCount < 0) 
    {
        UFC::BufferedLog::Printf(" ProcessCNTTelegraph() %s[%d], Wrong Message Count.", recvCommand.c_str(), recvMsgCount);
        return;
    }
    
    Int32 tandemMsgCount = FMsgPoolPtr->GetTandemMsgArrayCount(true);
    if (tandemMsgCount <= recvMsgCount)
    {
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " ProcessCNTTelegraph() %s[%d], tandemMsgCount[%d] <= receive Msg Count.", 
                                      recvCommand.c_str(), recvMsgCount, tandemMsgCount);
        return;
    }
        
    Int32 startSendMsgIndex = recvMsgCount;  //Start from next Message Index
    Int32 endSendMsgIndex = tandemMsgCount - 1;
    UFC::BufferedLog::Printf(" ProcessCNTTelegraph() %s[%d], from index[%d] to [%d]", recvCommand.c_str(), recvMsgCount, startSendMsgIndex, endSendMsgIndex);
        
    UFC::AnsiString telegraphStr = "";
    Int32 tandemReportCount = 0;
    Int32 processReportCount = 0;
    Int32 LastSeqNo = 0;
    for (Int32 curSendMsgIndex = startSendMsgIndex; curSendMsgIndex <= endSendMsgIndex; curSendMsgIndex++)
    {
        CTandemMessage *tandemMsgPtr = FMsgPoolPtr->GetTandemMsgFromArray(curSendMsgIndex, true);
        if (tandemMsgPtr != 0) 
        {
            UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " ProcessCNTTelegraph() process Index[%d] tandem data.", curSendMsgIndex);
            processReportCount++;
        }
        else
            continue;
        
        if (tandemReportCount >= CTandemTelegraph::GetMaxTandemReportCount())
        {
            if ((telegraphStr.Length() > 0) && (LastSeqNo > 0))
            {
                CTandemTelegraph curTelegraph("ADD", LastSeqNo, telegraphStr);
                if (SendTelegraph(&curTelegraph))
                    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " ProcessCNTTelegraph() %d send %s[%d][%s]%d.", 
                                                  curSendMsgIndex, curTelegraph.GetCommand().c_str(), curTelegraph.GetNumber(), curTelegraph.GetData().c_str(), curTelegraph.GetData().Length());
                else
                    UFC::BufferedLog::Printf(" ProcessCNTTelegraph() %d send %s[%d][%s]%d failed.", 
                                             curSendMsgIndex, curTelegraph.GetCommand().c_str(), curTelegraph.GetNumber(), curTelegraph.GetData().c_str(), curTelegraph.GetData().Length());
            }
            telegraphStr = "";
            LastSeqNo = 0;
            tandemReportCount = 0;
        }

        tandemReportCount++;
        UFC::AnsiString tandemDataStr = tandemMsgPtr->GetTandemData();
        telegraphStr = telegraphStr + tandemDataStr;
        LastSeqNo = tandemMsgPtr->GetSequenceNo();
    }  //for (Int32 curSendMsgIndex = startSendMsgIndex; curSendMsgIndex <= endSendMsgIndex; curSendMsgIndex++)    
    UFC::BufferedLog::Printf(" ProcessCNTTelegraph() process [%d] tandem Report.", processReportCount);
    
    if ((telegraphStr.Length() > 0) && (LastSeqNo > 0))
    {
        CTandemTelegraph lastTelegraph("ADD", LastSeqNo, telegraphStr);
        if (SendTelegraph(&lastTelegraph))
            UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " ProcessCNTTelegraph() send %s[%d][%s]%d.", 
                                          lastTelegraph.GetCommand().c_str(), lastTelegraph.GetNumber(), lastTelegraph.GetData().c_str(), lastTelegraph.GetData().Length());
        else
            UFC::BufferedLog::Printf(" ProcessCNTTelegraph() send %s[%d][%s]%d failed.", 
                                     lastTelegraph.GetCommand().c_str(), lastTelegraph.GetNumber(), lastTelegraph.GetData().c_str(), lastTelegraph.GetData().Length());
    }
    UFC::BufferedLog::FlushToFile();
}  //CTandemAdapter::ProcessCNTTelegraph()

//------------------------------------------------------------------------------
void CTandemAdapter::ProcessACKTelegraph(CTandemTelegraph *TelegraphPtr)
{
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " ProcessACKTelegraph() %s[%d]", 
                                  TelegraphPtr->GetCommand().c_str(), TelegraphPtr->GetNumber());
    return;
}  //CTandemAdapter::ProcessACKTelegraph()

//------------------------------------------------------------------------------
void CTandemAdapter::ProcessRSTelegraph(CTandemTelegraph *TelegraphPtr)
{
    UFC::AnsiString recvCommand = TelegraphPtr->GetCommand();
    Int32 startMsgTime = TelegraphPtr->GetNumber();
     UFC::BufferedLog::Printf(" ProcessRSTelegraph() %s[%d].", recvCommand.c_str(), startMsgTime);
    Int32 msgCount = FMsgPoolPtr->GetTandemMsgArrayCount(true);
    Int32 processReportCount = 0;
    for (Int32 curSendMsgIndex = 0; curSendMsgIndex < msgCount; curSendMsgIndex++)
    {
        CTandemMessage *tandemMsgPtr = FMsgPoolPtr->GetTandemMsgFromArray(curSendMsgIndex, true);
        if (tandemMsgPtr == 0) continue;
        UFC::AnsiString msgRecvTimeStr = tandemMsgPtr->GetReceiveTime();
        Int32 msgRecvTime = 0;
        if (msgRecvTimeStr.Length() > 0) msgRecvTime = msgRecvTimeStr.ToInt();
        if (msgRecvTime < startMsgTime) continue;
        
        processReportCount++;
        UFC::AnsiString tandemDataStr = tandemMsgPtr->GetTandemData();
        CTandemTelegraph curTelegraph("UPD", curSendMsgIndex + 1, tandemDataStr);
        if (SendTelegraph(&curTelegraph))
            UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " ProcessRSTelegraph() send %s[%d][%s].", 
                                          curTelegraph.GetCommand().c_str(), curTelegraph.GetNumber(), curTelegraph.GetData().c_str());
        else
            UFC::BufferedLog::Printf(" ProcessRSTelegraph() send %s[%d][%s] failed.", 
                                     curTelegraph.GetCommand().c_str(), curTelegraph.GetNumber(), curTelegraph.GetData().c_str());
    }  //for (Int32 curSendMsgIndex = 0; curSendMsgIndex < msgCount; curSendMsgIndex++)
    UFC::BufferedLog::Printf(" ProcessRSTelegraph() process [%d] tandem Report.", processReportCount);
    UFC::BufferedLog::FlushToFile();
}  //CTandemAdapter::ProcessRSTelegraph()

//------------------------------------------------------------------------------
void CTandemAdapter::ProcessCFMTelegraph(CTandemTelegraph *TelegraphPtr)
{
    UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " CTandemAdapter::ProcessCFMTelegraph() %s[%d].", 
                                  TelegraphPtr->GetCommand().c_str(), TelegraphPtr->GetNumber());
    return;
}  //CTandemAdapter::ProcessCFMTelegraph()

//------------------------------------------------------------------------------
void CTandemAdapter::ProcessTelegraph(Int16 TelegraphLength, const UFC::AnsiString& TelegraphBodyStr)
{
    CTandemTelegraph *telegraphPtr = new CTandemTelegraph(TelegraphLength, TelegraphBodyStr);
    UFC::AnsiString telegraphCmd = telegraphPtr->GetCommand();
    if (telegraphCmd == "CNT")
        ProcessCNTTelegraph(telegraphPtr);
    else if (telegraphCmd == "ACK")
        ProcessACKTelegraph(telegraphPtr);
    else if (telegraphCmd == "RS")
        ProcessRSTelegraph(telegraphPtr);
    else if (telegraphCmd == "CFM")
        ProcessCFMTelegraph(telegraphPtr);
    else
        UFC::BufferedLog::Printf(" ProcessTelegraph() Unknown Command. telegraph:[%s]", TelegraphBodyStr.c_str());        
    delete telegraphPtr;
}  //CTandemAdapter::ProcessTelegraph()

//------------------------------------------------------------------------------
BOOL CTandemAdapter::OnDataArrived( UFC::PClientSocket *Socket )
{   //Implement UFC::SocketClientListener
    Int16 TelegraphLength = 0;
    if (ReadTelegraphLength(Socket, TelegraphLength))
    {
        UFC::AnsiString telegraphBodyStr;
        if (ReadTelegraphBody(Socket, TelegraphLength, telegraphBodyStr))
        {
            CTandemAdapter::ProcessTelegraph(TelegraphLength, telegraphBodyStr);
            return TRUE;
        }
    }  //if (ReadTeleGraphLength(Socket, Int16& TeleGraphLength))
    return FALSE;
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
        UFC::BufferedLog::Printf(" CTandemAdapter::ReceiveData() Exception: %s", e.what());
        UFC::BufferedLog::FlushToFile();
    }
    return IsSuccess;
}  //CTandemAdapter::ReceiveData()

//------------------------------------------------------------------------------
bool CTandemAdapter::SendData(Int8 *DataBuffer, Int32 DataSize)
{
    bool IsSuccess = false;
    if (!IsConnected() || (DataBuffer == 0) || (DataSize <= 0)) return IsSuccess;
    FTandemConnectionLocker.Acquire();
    try 
    {
        FTandemConnectionPtr->BlockSend(DataBuffer, DataSize);
        IsSuccess = true;
    }
    catch (UFC::SocketException e)
    {
        UFC::BufferedLog::Printf(" CTandemAdapter::SendData() Exception: %s", e.what());
        UFC::BufferedLog::FlushToFile();
    }
    FTandemConnectionLocker.Release();
    return IsSuccess;
}  //CTandemAdapter::SendData()

//------------------------------------------------------------------------------
bool CTandemAdapter::DoHeartbeat()
{
    Int32 msgCount = FMsgPoolPtr->GetTandemMsgArrayCount(true);
    CTandemTelegraph *telegraphPtr = new CTandemTelegraph("CFM", msgCount, "");
    if (SendTelegraph(telegraphPtr))
        UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " DoHeartbeat() %s[%d]", telegraphPtr->GetCommand().c_str(), telegraphPtr->GetNumber()); 
    else
        UFC::BufferedLog::Printf(" DoHeartbeat() %s[%d] failed.", telegraphPtr->GetCommand().c_str(), telegraphPtr->GetNumber()); 
    delete telegraphPtr;
    return true;
}  //CTandemAdapter::SendHeartbeat()
//------------------------------------------------------------------------------
void CTandemAdapter::Execute(void)
{   //Implement UFC::PThread interface
    int FailedTimes = 0;
    
    while( !IsTerminated() )
    {
        if ( !IsConnected() ) 
        {
            if ( FTandemConnectionPtr != 0 ) 
                 delete FTandemConnectionPtr;
            if ( CreateConnection() ) 
                 Connect();
            UFC::SleepMS( 2 * FCheckInterval );
            if( !IsConnected() ) 
            {
                FailedTimes++;
                UFC::BufferedLog::Printf( " Connect to Tandem failed %d times.", FailedTimes ); 
                if( FailedTimes >= 5 )
                    exit( 0 );
            }
            else
                FailedTimes = 0;
        }  //if (!IsConnected())
        
        if ( IsConnected() ) 
        {
            FTimeLocker.Acquire();
            bool IsNeedHeartBeat = ( FWaitCount >= FMaxWaitCount );
            if ( IsNeedHeartBeat ) FWaitCount = 0;
            FTimeLocker.Release();
            if ( IsNeedHeartBeat ) 
                if ( !DoHeartbeat() ) DisConnect();
        }  //if (IsConnected())
        UFC::SleepMS( FCheckInterval );
        FTimeLocker.Acquire();
        FWaitCount++;
        FTimeLocker.Release();
    }  //while(!IsTerminated())
}  //CTandemAdapter::Execute()
//------------------------------------------------------------------------------
}  //namespace jihsun
//------------------------------------------------------------------------------
