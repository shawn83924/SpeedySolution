/* 
 * File:   TandemAdapter.h
 * Author: Zhen Fan
 *
 * Created on 2012/06/28 10:24
 */

#ifndef TANDEMADAPTER_H
#define	TANDEMADAPTER_H

#include "UFC.h"
//#include "OrderMessage/OrderMessageDefine.h"  // Lib
#include "TandemObjects.h"

namespace jihsun
{    
//------------------------------------------------------------------------------
class CTandemAdapter : public UFC::SocketClientListener, public UFC::PThread
{
protected:
    static const Int32 FReceiveBufferSize          = 1024;
    static const Int32 FFillHeartbeatBufferSize    =  440;
    static const Int32 FConfirmHeartbeatBufferSize =  440;
    /*
#define CONFIRM_SIZE      175
#define NOTICE_SIZE       80
//------------------------------------------------------------------------------
#define MAXSRC_SIZE       127
#define FILL_SIZE         180

     */
    
    UFC::AnsiString       FTandemServerAddress;
    Int32                 FTandemServerPort;
    CDataExchangePool     *FMsgPoolPtr;
    Int32                 FHeartbeatInterval;
    Int32                 FCheckInterval;
    UFC::PClientSocket    *FTandemConnectionPtr;
    UFC::PCriticalSection FTandemConnectionLocker;
      
    UFC::PCriticalSection FTimeLocker;
    UFC::AnsiString       FDisconnectTimeStr;
    Int32                 FMaxWaitCount;
    Int32                 FWaitCount;

    bool CreateConnection();
    
public:
    CTandemAdapter(const UFC::AnsiString& TandemServerAddress, Int32 TandemServerPort, 
                   CDataExchangePool *MsgPoolPtr,
                   Int32 HeartbeatInterval, Int32 CheckInterval = 1000);
    ~CTandemAdapter();
    
    UFC::AnsiString GetTandemServerAddress() {return FTandemServerAddress;};
    Int32           GetTandemServerPort() {return FTandemServerPort;};
    Int32           GetHeartbeatInterval() {return FHeartbeatInterval;};
    UFC::AnsiString GetDisconnectTime();
    
    void SetDisconnectTime(const UFC::AnsiString& DisconnectTime);
    void ClearDisconnectTime();
    
    bool IsConnected();
    bool Connect();
    bool DisConnect();
    
    bool ReadTelegraphLength(UFC::PClientSocket *SocketPtr, Int16& TelegraphLength);
    bool ReadTelegraphBody(UFC::PClientSocket *SocketPtr, Int16 TelegraphLength, UFC::AnsiString& TelegraphBodyStr);
    CTandemTelegraph *ReceiveTelegraph(UFC::PClientSocket *SocketPtr);
    CTandemTelegraph *ReceiveTelegraph();

    bool SendTelegraph(CTandemTelegraph *TelegraphPtr);
    void ProcessCNTTelegraph(CTandemTelegraph *TelegraphPtr);
    void ProcessACKTelegraph(CTandemTelegraph *TelegraphPtr);
    void ProcessRSTelegraph(CTandemTelegraph *TelegraphPtr);
    void ProcessCFMTelegraph(CTandemTelegraph *TelegraphPtr);
    void ProcessTelegraph(Int16 TelegraphLength, const UFC::AnsiString& TelegraphBodyStr);
    
    //Implement UFC::SocketClientListener
    virtual void OnConnect(UFC::PClientSocket *Socket);
    virtual void OnDisconnect(UFC::PClientSocket *Socket, BOOL NeedReconnect = FALSE);
    virtual BOOL OnDataArrived( UFC::PClientSocket *Socket );
    virtual void OnIdle(UFC::PClientSocket *Socket);

    bool ReceiveData(Int8 *DataBuffer, Int32 DataSize);
    bool SendData(Int8 *DataBuffer, Int32 DataSize);
    bool DoHeartbeat();
    
    //Implement UFC::PThread interface
    virtual void Execute(void);
};  //CTandemAdapter
    
}  //namespace jihsun
#endif	/* TANDEMADAPTER_H */

