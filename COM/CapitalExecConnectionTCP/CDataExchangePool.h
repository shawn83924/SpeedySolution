/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CDataExchangePool.h
 * Author: yuan
 *
 * Created on June 13, 2023, 12:44 AM
 */

#ifndef CDATAEXCHANGEPOOL_H
#define CDATAEXCHANGEPOOL_H

#include "TandemObject.h"
//------------------------------------------------------------------------------    
namespace capital
{
//------------------------------------------------------------------------------    
class CDataExchangePool
{
    UFC::PLUS::CConditionMutex  FTandemMsgQueueLocker;
    std::queue<CTandemMessage*> FTandemMsgQueue;
    std::queue<UFC::AnsiString*> FAlertMsgQueue;
    std::queue<UFC::AnsiString*> FNoticeMsgQueue;    
    UFC::PLUS::CConditionMutex FExecIDLocker;
    std::set<UFC::AnsiString> FExecIDIndex;
    
public:
    CDataExchangePool();
    ~CDataExchangePool();
    
    void LockTandemMsgQueue() {FTandemMsgQueueLocker.Lock();};
    void UnlockTandemMsgQueue() {FTandemMsgQueueLocker.Unlock();};
    Int32 WaitTandemMsgQueue() {return FTandemMsgQueueLocker.Wait();};
    Int32 WaitTandemMsgQueue(Int32 WairingMilliSecond) {return FTandemMsgQueueLocker.Wait(WairingMilliSecond);};
    void SignalTandemMsgWaiter() {FTandemMsgQueueLocker.Signal();};

    Int32 GetWaitingTandemMsgCount(bool DoLock = true);
    CTandemMessage *PopTandemMsg(bool DoLock = true);
    void PushTandemMsg(CTandemMessage *TandemMsgPtr, bool DoLock = true);
    void ClearTandemMsgQueue(bool DoLock = true);
    
    Int32 GetWaitingAlertMsgCount(bool DoLock = true);
    UFC::AnsiString *PopAlertMsg(bool DoLock = true);
    void PushAlertMsg(UFC::AnsiString *AlertMsgPtr, bool DoLock = true);
    void ClearAlertMsgQueue(bool DoLock = true);
    
    Int32 GetWaitingNoticeMsgCount(bool DoLock = true);
    UFC::AnsiString *PopNoticeMsg(bool DoLock = true);
    void PushNoticeMsg(UFC::AnsiString *AlertMsgPtr, bool DoLock = true);
    void ClearNoticeMsgQueue(bool DoLock = true);
    
    void LockExecIDIndex() {FExecIDLocker.Lock();};
    void UnlockExecIDIndex() {FExecIDLocker.Unlock();};
    int GetExecIDCount(bool DoLock = true);
    bool IsExecIDExist(const UFC::AnsiString& ExecID, bool DoLock = true);
    bool InsertExecIDIndex(const UFC::AnsiString& ExecID, bool DoCheck = true, bool DoLock = true);
};  //CDataExchangePool()
//------------------------------------------------------------------------------    
};
//------------------------------------------------------------------------------    
#endif /* CDATAEXCHANGEPOOL_H */

