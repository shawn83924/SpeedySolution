/* 
 * File:   NoticeServer.h
 * Author: Zhen Fan
 *
 * Created on 2011/12/27 14:30
 */

#ifndef NOTICESERVER_H
#define	NOTICESERVER_H

#include "UFC.h"
#include "TandemObject.h"
#include "CDataExchangePool.h"
namespace capital
{
//------------------------------------------------------------------------------
class CNoticeServer : public UFC::SocketServerListener
{
protected:
    Int32                   FListenPort;
    Int32                   FNoticeBufferSize;        
    CDataExchangePool       *FMsgPoolPtr;
    UFC::MultiplexingServer *FServiceServerPtr;
        
public:
    CNoticeServer(Int32 ListenPort, CDataExchangePool *MsgPoolPtr, int MsgSize );
    virtual ~CNoticeServer();
    
    Int32 GetListenPort() {return FListenPort;};
    
    Int32 ReceiveClientData(UFC::PClientSocket *ClientSocket, UInt8 *ReceiveBuffer);
    bool SendclientData(UFC::PClientSocket *ClientSocket, const Int8 *SendDataBuffer, Int32 SendDataSize);

    //Implement UFC::SocketServerListener
    virtual void OnListen(UFC::MultiplexingServer *ServSocket);
    virtual void OnClientConnect(UFC::MultiplexingServer *ServSocket, UFC::PClientSocket *ClientSocket);
    virtual void OnClientDisconnect(UFC::MultiplexingServer *ServSocket, UFC::PClientSocket *ClientSocket);
    virtual BOOL OnClientWrite(UFC::MultiplexingServer *ServSocket, UFC::PClientSocket *ClientSocket);
    virtual void OnServerIdle(UFC::MultiplexingServer *ServSocket);
    virtual BOOL OnHeartbeat(UFC::PClientSocket *pSocket, BOOL IsRead);
};  //CNoticeServer    
}  //namespace capital
#endif	/* NOTICESERVER_H */
