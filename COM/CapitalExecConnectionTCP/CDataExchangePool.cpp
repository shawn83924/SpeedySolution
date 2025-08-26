/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CDataExchangePool.cpp
 * Author: yuan
 * 
 * Created on June 13, 2023, 12:44 AM
 */

#include "CDataExchangePool.h"
//------------------------------------------------------------------------------
namespace capital
{
//----------------------------- CDataExchangePool ------------------------------
CDataExchangePool::CDataExchangePool()
{
}  //CDataExchangePool::CDataExchangePool()

//------------------------------------------------------------------------------
CDataExchangePool::~CDataExchangePool()
{
    ClearTandemMsgQueue(true);
    ClearAlertMsgQueue(true);
    ClearNoticeMsgQueue(true);
}  //CDataExchangePool::~CDataExchangePool()

//------------------------------------------------------------------------------
Int32 CDataExchangePool::GetWaitingTandemMsgCount(bool DoLock)
{
    if (DoLock) FTandemMsgQueueLocker.Lock();
    Int32 MsgCount = FTandemMsgQueue.size();
    if (DoLock) FTandemMsgQueueLocker.Unlock();
    return MsgCount;
}  //CDataExchangePool::GetWaitingTandemMsgCount()

//------------------------------------------------------------------------------
CTandemMessage *CDataExchangePool::PopTandemMsg(bool DoLock)
{
    CTandemMessage *TandemMsgPtr = 0;
    if (DoLock) FTandemMsgQueueLocker.Lock();
    if (FTandemMsgQueue.size() > 0)
    {
        TandemMsgPtr = FTandemMsgQueue.front();
        FTandemMsgQueue.pop();
    }
    if (DoLock) FTandemMsgQueueLocker.Unlock();
    return TandemMsgPtr;    
}  //CDataExchangePool::PopTandemMsg()

//------------------------------------------------------------------------------
void CDataExchangePool::PushTandemMsg(CTandemMessage *TandemMsgPtr, bool DoLock)
{
    if (TandemMsgPtr == 0) return;
    if (DoLock) FTandemMsgQueueLocker.Lock();
    FTandemMsgQueue.push(TandemMsgPtr);
    if (DoLock) FTandemMsgQueueLocker.Signal();
    if (DoLock) FTandemMsgQueueLocker.Unlock();
}  //CDataExchangePool::PushTandemMsg()

//------------------------------------------------------------------------------
void CDataExchangePool::ClearTandemMsgQueue(bool DoLock)
{
    if (DoLock) FTandemMsgQueueLocker.Lock();
    while(FTandemMsgQueue.size() > 0)
    {
        CTandemMessage *TandemMsgPtr = FTandemMsgQueue.front();
        FTandemMsgQueue.pop();
        delete TandemMsgPtr;
    }
    if (DoLock) FTandemMsgQueueLocker.Unlock();
}  //CDataExchangePool::ClearTandemMsgQueue()

//------------------------------------------------------------------------------
Int32 CDataExchangePool::GetWaitingAlertMsgCount(bool DoLock)
{
    if (DoLock) FTandemMsgQueueLocker.Lock();
    Int32 AlertCount = FAlertMsgQueue.size();
    if (DoLock) FTandemMsgQueueLocker.Unlock();
    return AlertCount;
}  //CDataExchangePool::GetWaitingAlertMsgCount()

//------------------------------------------------------------------------------
UFC::AnsiString *CDataExchangePool::PopAlertMsg(bool DoLock)
{
    UFC::AnsiString *AlertMsgPtr = 0;
    if (DoLock) FTandemMsgQueueLocker.Lock();
    if (FAlertMsgQueue.size() > 0)
    {
        AlertMsgPtr = FAlertMsgQueue.front();
        FAlertMsgQueue.pop();
    }
    if (DoLock) FTandemMsgQueueLocker.Unlock();
    return AlertMsgPtr;    
}  //CDataExchangePool::PopAlertMsg()

//------------------------------------------------------------------------------
void CDataExchangePool::PushAlertMsg(UFC::AnsiString *AlertMsgPtr, bool DoLock)
{
    if (AlertMsgPtr == 0) return;
    if (DoLock) FTandemMsgQueueLocker.Lock();
    FAlertMsgQueue.push(AlertMsgPtr);
    if (DoLock) FTandemMsgQueueLocker.Signal();
    if (DoLock) FTandemMsgQueueLocker.Unlock();
}  //CDataExchangePool::PushAlertMsg()

//------------------------------------------------------------------------------
void CDataExchangePool::ClearAlertMsgQueue(bool DoLock)
{
    if (DoLock) FTandemMsgQueueLocker.Lock();
    while(FAlertMsgQueue.size() > 0)
    {
        UFC::AnsiString *AlertMsgPtr = FAlertMsgQueue.front();
        FAlertMsgQueue.pop();
        delete AlertMsgPtr;
    }
    if (DoLock) FTandemMsgQueueLocker.Unlock();
}  //CDataExchangePool::ClearAlertMsgQueue()

//------------------------------------------------------------------------------
Int32 CDataExchangePool::GetWaitingNoticeMsgCount(bool DoLock)
{
    if (DoLock) FTandemMsgQueueLocker.Lock();
    Int32 NoticeCount = FNoticeMsgQueue.size();
    if (DoLock) FTandemMsgQueueLocker.Unlock();
    return NoticeCount;
}  //CDataExchangePool::GetWaitingNoticeMsgCount()

//------------------------------------------------------------------------------
UFC::AnsiString *CDataExchangePool::PopNoticeMsg(bool DoLock)
{
    UFC::AnsiString *NoticeMsgPtr = 0;
    if (DoLock) FTandemMsgQueueLocker.Lock();
    if (FNoticeMsgQueue.size() > 0)
    {
        NoticeMsgPtr = FNoticeMsgQueue.front();
        FNoticeMsgQueue.pop();
    }
    if (DoLock) FTandemMsgQueueLocker.Unlock();
    return NoticeMsgPtr;    
}  //CDataExchangePool::PopNoticeMsg()

//------------------------------------------------------------------------------
void CDataExchangePool::PushNoticeMsg(UFC::AnsiString *NoticeMsgPtr, bool DoLock)
{
    if (NoticeMsgPtr == 0) return;
    if (DoLock) FTandemMsgQueueLocker.Lock();
    FNoticeMsgQueue.push(NoticeMsgPtr);
    if (DoLock) FTandemMsgQueueLocker.Signal();
    if (DoLock) FTandemMsgQueueLocker.Unlock();
}  //CDataExchangePool::PushNoticeMsg()

//------------------------------------------------------------------------------
void CDataExchangePool::ClearNoticeMsgQueue(bool DoLock)
{
    if (DoLock) FTandemMsgQueueLocker.Lock();
    while(FNoticeMsgQueue.size() > 0)
    {
        UFC::AnsiString *NoticeMsgPtr = FNoticeMsgQueue.front();
        FNoticeMsgQueue.pop();
        delete NoticeMsgPtr;
    }
    if (DoLock) FTandemMsgQueueLocker.Unlock();
}  //CDataExchangePool::ClearAlertMsgQueue()

//------------------------------------------------------------------------------
int CDataExchangePool::GetExecIDCount(bool DoLock)
{
    if (DoLock) FExecIDLocker.Lock();
    int execIDCount = FExecIDIndex.size();
    if (DoLock) FExecIDLocker.Unlock();
    return execIDCount;
}  //CDataExchangePool::GetExecIDCount()

//------------------------------------------------------------------------------
bool CDataExchangePool::IsExecIDExist(const UFC::AnsiString& ExecID, bool DoLock)
{
    if (DoLock) FExecIDLocker.Lock();
    std::set<UFC::AnsiString>::iterator it = FExecIDIndex.find(ExecID);
    if (DoLock) FExecIDLocker.Unlock();
    if (it == FExecIDIndex.end())
        return false;
    else
        return true;
}  //CDataExchangePool::IsExecIdExist()

//------------------------------------------------------------------------------
bool CDataExchangePool::InsertExecIDIndex(const UFC::AnsiString& ExecID, bool DoCheck, bool DoLock)
{
    bool isSuccess = false;
    if (ExecID.Length() <= 0) return isSuccess;
    if (DoLock) FExecIDLocker.Lock();
    if (!DoCheck || !IsExecIDExist(ExecID, false)) 
    {
        std::pair<std::set<UFC::AnsiString>::iterator, bool> insertPair;
        insertPair = FExecIDIndex.insert(ExecID);
        isSuccess = insertPair.second;
    }
    if (DoLock) FExecIDLocker.Unlock();
    return isSuccess;
}  //CDataExchangePool::InsertExecIdIndex()
//------------------------------------------------------------------------------
};
//------------------------------------------------------------------------------
