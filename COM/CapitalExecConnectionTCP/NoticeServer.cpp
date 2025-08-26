/* 
 * File:   NoticeServer.cpp
 * Author: Zhen Fan
 *
 * Created on 2011/12/27 14:30
 */
#include <cstring>
#include "NoticeServer.h"

namespace capital
{
//------------------------------- CNoticeServer --------------------------------
//------------------------------------------------------------------------------
CNoticeServer::CNoticeServer(Int32 ListenPort, CDataExchangePool *MsgPoolPtr)
:FListenPort(ListenPort),
 FMsgPoolPtr(MsgPoolPtr),
 FServiceServerPtr(0)
{
    FServiceServerPtr = new UFC::MultiplexingServer(ListenPort);
    FServiceServerPtr->EnableHeartbeat(FALSE);
    FServiceServerPtr->SetListener(this);
    FServiceServerPtr->Run();
}  //CNoticeServer::CNoticeServer()

//------------------------------------------------------------------------------
CNoticeServer::~CNoticeServer()
{
    if (FServiceServerPtr != 0) 
    {
        UFC::BufferedLog::Printf(" CNoticeServer::~CNoticeServer() Stop Service Srever.");
        FServiceServerPtr->Stop();
        delete FServiceServerPtr;
    }
    UFC::BufferedLog::FlushToFile();
}  //CNoticeServer::~CNoticeServer()

    //Implement UFC::SocketServerListener
//------------------------------------------------------------------------------
void CNoticeServer::OnListen(UFC::MultiplexingServer *ServSocket)
{
    UFC::BufferedLog::Printf(" NoticeServer Listen port:[%d].", ServSocket->GetPort());
    UFC::BufferedLog::FlushToFile();
}  //CNoticeServer::OnListen()

//------------------------------------------------------------------------------
void CNoticeServer::OnClientConnect(UFC::MultiplexingServer *ServSocket, UFC::PClientSocket *ClientSocket)
{
    UFC::BufferedLog::Printf(" %s Connect NoticeServer on Port:%d.", ClientSocket->GetPeerIPAddress().c_str() , ServSocket->GetPort());
    UFC::BufferedLog::FlushToFile();
}  //CNoticeServer::OnClientConnect()

//------------------------------------------------------------------------------
void CNoticeServer::OnClientDisconnect(UFC::MultiplexingServer *ServSocket, UFC::PClientSocket *ClientSocket)
{
    UFC::BufferedLog::Printf( " %s on Port:%d Disconnect NoticeServer.", ClientSocket->GetPeerIPAddress().c_str() , ServSocket->GetPort() );
    UFC::BufferedLog::FlushToFile();
}  //CNoticeServer::OnClientDisconnect()

//------------------------------------------------------------------------------
BOOL CNoticeServer::OnClientWrite(UFC::MultiplexingServer *ServSocket, UFC::PClientSocket *ClientSocket)
{
    UInt8 QueueData[FNoticeBufferSize];
    UFC::PLockObject Lock(ClientSocket->ReadLock);
    ClientSocket->BlockRecv(QueueData, FNoticeBufferSize);
    Int8 CharData[FNoticeBufferSize + 1];
    CharData[FNoticeBufferSize] = 0;
    std::memcpy(CharData, QueueData, FNoticeBufferSize);
    UFC::BufferedLog::Printf(" <Notice Flag> <%s>", CharData);
    Int8 reCharData[21];
    reCharData[20] = 0;
    std::memcpy(reCharData, QueueData, 20);
    ClientSocket->BlockSend(reCharData, 20);
    UFC::BufferedLog::Printf(" <BlockSend Notice Flag> <%s>", reCharData);
    UFC::BufferedLog::FlushToFile();
    
    if (FMsgPoolPtr != 0)
    {
        UFC::AnsiString *NoticeStrPtr = new UFC::AnsiString(CharData);
        FMsgPoolPtr->PushNoticeMsg(NoticeStrPtr, true);
    }
    return TRUE;
}  //CNoticeServer::OnClientWrite()

//------------------------------------------------------------------------------
void CNoticeServer::OnServerIdle(UFC::MultiplexingServer *ServSocket)
{
}  //CNoticeServer::OnServerIdle()

//------------------------------------------------------------------------------
BOOL CNoticeServer::OnHeartbeat(UFC::PClientSocket *pSocket, BOOL IsRead)
{
    return TRUE;
}  //CNoticeServer::OnHeartbeat()
}  //namespace capital
