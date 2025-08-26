/* 
 * File:   TandemAdapter.h
 * Author: Zhen Fan
 *
 * Created on 2011/12/26  9:50
 */

#ifndef TANDEMADAPTER_H
#define	TANDEMADAPTER_H

#include "UFC.h"
#include "OrderMessage/OrderMessageDefine.h"  // Lib
#include "TandemObject.h"

namespace capital
{    
//------------------------------------------------------------------------------
class CTandemAdapter : public UFC::SocketClientListener, public UFC::PThread
{
protected:
    static const Int32               FReceiveBufferSize          = 1024;
    static const Int32               FFillHeartbeatBufferSize    =  440;
    static const Int32               FConfirmHeartbeatBufferSize =  440;
    /*
#define CONFIRM_SIZE      175
#define NOTICE_SIZE       80
//------------------------------------------------------------------------------
#define MAXSRC_SIZE       127
#define FILL_SIZE         180

     */
    
    nsOrderMessageDefine::MarketEnum FMarket;
    bool                             FIsFillMessageAdapter;
    UFC::AnsiString                  FTandemServerAddress;
    Int32                            FTandemServerPort;
    CDataExchangePool                *FMsgPoolPtr;
    Int32                            FHeartbeatInterval;
    Int32                            FCheckInterval;
    UFC::PClientSocket               *FTandemConnectionPtr;
    
    UFC::AnsiString                  FAdapterDescription;

    UFC::PCriticalSection            FTimeLocker;
    UFC::AnsiString                  FDisconnectTimeStr;
    Int32                            FMaxWaitCount;
    Int32                            FWaitCount;

    bool CreateConnection();
public:
    CTandemAdapter(nsOrderMessageDefine::MarketEnum Market, bool IsFillMessageAdapter,
                   const UFC::AnsiString& TandemServerAddress, Int32 TandemServerPort, 
                   CDataExchangePool *MsgPoolPtr,
                   Int32 HeartbeatInterval, Int32 CheckInterval = 1000);
    ~CTandemAdapter();
    
    nsOrderMessageDefine::MarketEnum GetMarket() {return FMarket;};
    bool                             IsFillMessageAdapter() {return FIsFillMessageAdapter;};
    UFC::AnsiString                  GetTandemServerAddress() {return FTandemServerAddress;};
    Int32                            GetTandemServerPort() {return FTandemServerPort;};
    Int32                            GetHeartbeatInterval() {return FHeartbeatInterval;};
    UFC::AnsiString                  GetDisconnectTime();
    
    void SetDisconnectTime(const UFC::AnsiString& DisconnectTime);
    void ClearDisconnectTime();
    
    bool IsConnected();
    bool Connect();
    bool DisConnect();
    
    //Implement UFC::SocketClientListener
    virtual void OnConnect(UFC::PClientSocket *Socket);
    virtual void OnDisconnect(UFC::PClientSocket *Socket);
    virtual BOOL OnDataArrived(UFC::PClientSocket *Socket);
    virtual void OnIdle(UFC::PClientSocket *Socket);

    bool ReceiveData(Int8 *DataBuffer, Int32 DataSize);
    bool SendData(Int8 *DataBuffer, Int32 DataSize);
    bool DoFillHeartbeat();
    bool DoConfirmHeartbeat();
    bool DoHeartbeat();
    
    //Implement UFC::PThread interface
    virtual void Execute(void);
};  //CTandemAdapter
    
}  //namespace capital
#endif	/* TANDEMADAPTER_H */

