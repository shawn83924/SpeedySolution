/* 
 * File:   JihSunEexcutionConnection.h
 * Author: Zhen Fan
 *
 * Created on 2012/06/27 11:01
 */

#ifndef JIHSUNEEXCUTIONCONNECTION_H
#define	JIHSUNEEXCUTIONCONNECTION_H

#include "UFC.h"
#include "OrderConnection/TTaifexConnection.h"
#include "TandemObjects.h"

namespace jihsun
{
//------------------------------------------------------------------------------
class CJSExecutionConnection : public IOrderConnectionEventListener, public UFC::PThread
{
private:
    UFC::AnsiString                  FConfigFileName;
    RecoverMarket                    FReceiveMarket;
    ConnectionType                   FExecutionType;
    UFC::AnsiString                  FApplicationName;
    Int32                            FRecoverBeginTime;
    CDataExchangePool                *FDataPoolPtr;
    UFC::AnsiString                  FTaifexIP;
    Int32                            FTaifexPort;
    UFC::AnsiString                  FID;
    UFC::AnsiString                  FPassword;
    UFC::AnsiString                  FAccount;
    UFC::AnsiString                  FBrokerID;
    int                              FReconnectInterval;
    bool                             FNeedSendQuoteExecutionReport;
    UFC::PHashedSet<UFC::AnsiString> FExcludeBrokerIDSet;
    TTaifexConnection*               FTaifexConnectionPtr;
    bool                             FIsConnected;
    bool                             FIsLogon;
    bool                             FIsFirstRecover;
    int                              FConnectCount;
    int                              FLogonCount;
    int                              FRecoverCount;
    
    UFC::PLUS::CConditionMutex FRecoverWaitingLocker;
    
   void LoadSetting();
    void PrintSetting();
public:
    CJSExecutionConnection(const UFC::AnsiString& ConfigFileName, 
                           RecoverMarket ReceiveMarket, ConnectionType ExecutionType,
                           Int32 RecoverBeginTime, CDataExchangePool *DataPoolPtr = 0);
    ~CJSExecutionConnection();

    void Recover(const UFC::AnsiString& BeginTime);
    void LockForRecover() {FRecoverWaitingLocker.Lock();};
    void UnlockForRecover() {FRecoverWaitingLocker.Unlock();};
    void WaitForRecover() {FRecoverWaitingLocker.Wait();};
    void SignalRecoverWaiting() {FRecoverWaitingLocker.Signal();};
    
    //implement interface IOrderConnectionEventListener
    void OnConnected(void);
    void OnDisconnected(void);
    void OnLogonReply(const char* ReplyString, LogonResult Result, int CID);
    void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    void OnRecoverFinished(int Count);
    
    int GetConnectCount() {return FConnectCount;};
    int GetLogonCount() {return FLogonCount;};
    int GetRecoverCount() {return FRecoverCount;};
    
    //implement interface UFC::PThread
    virtual void Execute( void );    
};  //CJSExecutionConnection
}  //namespace jihsun
#endif	/* JIHSUNEEXCUTIONCONNECTION_H */
