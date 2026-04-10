/* 
 * File:   PSharedData.h
 * Author: Zhen Fan
 *
 * Created on 2016/11/24 10:58
 */

#include "PSharedData.h"

namespace UFC
{
#if (defined(__LINUX) || defined(__AIX))
//------------------------------------------------------------------------------
int TryAndLockObjectForRead(PReadWriteLock* RWLockPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, Int32 WarningDebugLevel, Int32 DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = RWLockPtr->TryLockForRead();
    if (lockResult != 0)
    {
        if (lockResult == EBUSY)
        {
            if (BufferedLog::DebugLevelExist(WarningDebugLevel) == TRUE)
                LogPtr->fprintf(" %s Waiting %s Read Lock...", LogPrefix.c_str(), ObjectName.c_str());
            lockResult = RWLockPtr->LockForRead();
            if (lockResult == 0)
            {
                if (BufferedLog::DebugLevelExist(DetailDebugLevel) == TRUE)
                    LogPtr->fprintf(" %s Lock %s for Read Success.", LogPrefix.c_str(), ObjectName.c_str());
            }
            else
                LogPtr->fprintf(" %s Lock %s for Read Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
        }
        else
            LogPtr->fprintf(" %s Try Lock %s for Read Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    }
    else
    {
        if (BufferedLog::DebugLevelExist(DetailDebugLevel) == TRUE)
            LogPtr->fprintf(" %s Try Lock %s for Read Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    LogPtr->FlushToFile();
    return lockResult;
}  //TryAndLockObjectForRead()
//------------------------------------------------------------------------------
int UnlockObjectForRead(PReadWriteLock* RWLockPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, Int32 DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = RWLockPtr->UnlockForRead();
    if (lockResult == 0)
    {
        if (BufferedLog::DebugLevelExist(DetailDebugLevel) == TRUE)
            LogPtr->fprintf(" %s Unlock %s for Read Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    else
        LogPtr->fprintf(" %s Unlock %s for Read Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    LogPtr->FlushToFile();
    return lockResult;
}  //UnlockObjectForRead()
//------------------------------------------------------------------------------
int TryAndLockObjectForRead(PReadWriteLock* RWLockPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, bool WarningDebugLevel, bool DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = RWLockPtr->TryLockForRead();
    if (lockResult != 0)
    {
        if (lockResult == EBUSY)
        {
            if (WarningDebugLevel)
                LogPtr->fprintf(" %s Waiting %s Read Lock...", LogPrefix.c_str(), ObjectName.c_str());
            lockResult = RWLockPtr->LockForRead();
            if (lockResult == 0)
            {
                if (DetailDebugLevel)
                    LogPtr->fprintf(" %s Lock %s for Read Success.", LogPrefix.c_str(), ObjectName.c_str());
            }
            else
                LogPtr->fprintf(" %s Lock %s for Read Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
        }
        else
            LogPtr->fprintf(" %s Try Lock %s for Read Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    }
    else
    {
        if (DetailDebugLevel)
            LogPtr->fprintf(" %s Try Lock %s for Read Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    LogPtr->FlushToFile();
    return lockResult;
}  //TryAndLockObjectForRead()
//------------------------------------------------------------------------------
int UnlockObjectForRead(PReadWriteLock* RWLockPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, bool DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = RWLockPtr->UnlockForRead();
    if (lockResult == 0)
    {
        if (DetailDebugLevel)
            LogPtr->fprintf(" %s Unlock %s for Read Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    else
        LogPtr->fprintf(" %s Unlock %s for Read Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    LogPtr->FlushToFile();
    return lockResult;
}  //UnlockObjectForRead()

//------------------------------------------------------------------------------
int TryAndLockObjectForRead(CBasicRWLockObject* ObjPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, Int32 WarningDebugLevel, Int32 DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = ObjPtr->TryLockForRead();
    if (lockResult != 0)
    {
        if (lockResult == EBUSY)
        {
            if (BufferedLog::DebugLevelExist(WarningDebugLevel) == TRUE)
                LogPtr->fprintf(" %s Waiting %s Read Lock...", LogPrefix.c_str(), ObjectName.c_str());
            lockResult = ObjPtr->LockForRead();
            if (lockResult == 0)
            {
                if (BufferedLog::DebugLevelExist(DetailDebugLevel) == TRUE)
                    LogPtr->fprintf(" %s Lock %s for Read Success.", LogPrefix.c_str(), ObjectName.c_str());
            }
            else
                LogPtr->fprintf(" %s Lock %s for Read Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
        }
        else
            LogPtr->fprintf(" %s Try Lock %s for Read Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    }
    else
    {
        if (BufferedLog::DebugLevelExist(DetailDebugLevel) == TRUE)
            LogPtr->fprintf(" %s Try Lock %s for Read Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    LogPtr->FlushToFile();
    return lockResult;
}  //TryAndLockObjectForRead()
//------------------------------------------------------------------------------
int UnlockObjectForRead(CBasicRWLockObject* ObjPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, Int32 DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = ObjPtr->UnLockForRead();
    if (lockResult == 0)
    {
        if (BufferedLog::DebugLevelExist(DetailDebugLevel) == TRUE)
            LogPtr->fprintf(" %s Unlock %s for Read Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    else
        LogPtr->fprintf(" %s Unlock %s for Read Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    LogPtr->FlushToFile();
    return lockResult;
}  //UnlockObjectForRead()
//------------------------------------------------------------------------------
int TryAndLockObjectForRead(CBasicRWLockObject* ObjPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, bool WarningDebugLevel, bool DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = ObjPtr->TryLockForRead();
    if (lockResult != 0)
    {
        if (lockResult == EBUSY)
        {
            if (WarningDebugLevel)
                LogPtr->fprintf(" %s Waiting %s Read Lock...", LogPrefix.c_str(), ObjectName.c_str());
            lockResult = ObjPtr->LockForRead();
            if (lockResult == 0)
            {
                if (DetailDebugLevel)
                    LogPtr->fprintf(" %s Lock %s for Read Success.", LogPrefix.c_str(), ObjectName.c_str());
            }
            else
                LogPtr->fprintf(" %s Lock %s for Read Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
        }
        else
            LogPtr->fprintf(" %s Try Lock %s for Read Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    }
    else
    {
        if (DetailDebugLevel)
            LogPtr->fprintf(" %s Try Lock %s for Read Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    LogPtr->FlushToFile();
    return lockResult;
}  //TryAndLockObjectForRead()
//------------------------------------------------------------------------------
int UnlockObjectForRead(CBasicRWLockObject* ObjPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, bool DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = ObjPtr->UnLockForRead();
    if (lockResult == 0)
    {
        if (DetailDebugLevel)
            LogPtr->fprintf(" %s Unlock %s for Read Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    else
        LogPtr->fprintf(" %s Unlock %s for Read Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    LogPtr->FlushToFile();
    return lockResult;
}  //UnlockObjectForRead()



//------------------------------------------------------------------------------
int TryAndLockObjectForWrite(PReadWriteLock* RWLockPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, Int32 WarningDebugLevel, Int32 DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = RWLockPtr->TryLockForWrite();
    if (lockResult != 0)
    {
        if (lockResult == EBUSY)
        {
            if (BufferedLog::DebugLevelExist(WarningDebugLevel) == TRUE)
                LogPtr->fprintf(" %s Waiting %s Write Lock...", LogPrefix.c_str(), ObjectName.c_str());
            lockResult = RWLockPtr->LockForWrite();
            if (lockResult == 0)
            {
                if (BufferedLog::DebugLevelExist(DetailDebugLevel) == TRUE)
                    LogPtr->fprintf(" %s Lock %s for Write Success.", LogPrefix.c_str(), ObjectName.c_str());
            }
            else
                LogPtr->fprintf(" %s Lock %s for Write Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
        }
        else
            LogPtr->fprintf(" %s Try Lock %s for Write Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    }
    else
    {
        if (BufferedLog::DebugLevelExist(DetailDebugLevel) == TRUE)
            LogPtr->fprintf(" %s Try Lock %s for Write Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    LogPtr->FlushToFile();
    return lockResult;
}  //TryAndLockObjectForWrite()
//------------------------------------------------------------------------------
int UnlockObjectForWrite(PReadWriteLock* RWLockPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, Int32 DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = RWLockPtr->UnlockForWrite();
    if (lockResult == 0)
    {
        if (BufferedLog::DebugLevelExist(DetailDebugLevel) == TRUE)
            LogPtr->fprintf(" %s Unlock %s for Write Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    else
        LogPtr->fprintf(" %s Unlock %s for Write Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    LogPtr->FlushToFile();
    return lockResult;
}  //UnlockObjectForWrite()
//------------------------------------------------------------------------------
int TryAndLockObjectForWrite(PReadWriteLock* RWLockPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, bool WarningDebugLevel, bool DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = RWLockPtr->TryLockForWrite();
    if (lockResult != 0)
    {
        if (lockResult == EBUSY)
        {
            if (WarningDebugLevel)
                LogPtr->fprintf(" %s Waiting %s Write Lock...", LogPrefix.c_str(), ObjectName.c_str());
            lockResult = RWLockPtr->LockForWrite();
            if (lockResult == 0)
            {
                if (DetailDebugLevel)
                    LogPtr->fprintf(" %s Lock %s for Write Success.", LogPrefix.c_str(), ObjectName.c_str());
            }
            else
                LogPtr->fprintf(" %s Lock %s for Write Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
        }
        else
            LogPtr->fprintf(" %s Try Lock %s for Write Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    }
    else
    {
        if (DetailDebugLevel)
            LogPtr->fprintf(" %s Try Lock %s for Write Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    LogPtr->FlushToFile();
    return lockResult;
}  //TryAndLockObjectForWrite()
//------------------------------------------------------------------------------
int UnlockObjectForWrite(PReadWriteLock* RWLockPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, bool DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = RWLockPtr->UnlockForWrite();
    if (lockResult == 0)
    {
        if (DetailDebugLevel)
            LogPtr->fprintf(" %s Unlock %s for Write Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    else
        LogPtr->fprintf(" %s Unlock %s for Write Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    LogPtr->FlushToFile();
    return lockResult;
}  //UnlockObjectForWrite()

//------------------------------------------------------------------------------
int TryAndLockObjectForWrite(CBasicRWLockObject* ObjPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, Int32 WarningDebugLevel, Int32 DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = ObjPtr->TryLockForWrite();
    if (lockResult != 0)
    {
        if (lockResult == EBUSY)
        {
            if (BufferedLog::DebugLevelExist(WarningDebugLevel) == TRUE)
                LogPtr->fprintf(" %s Waiting %s Write Lock...", LogPrefix.c_str(), ObjectName.c_str());
            lockResult = ObjPtr->LockForWrite();
            if (lockResult == 0)
            {
                if (BufferedLog::DebugLevelExist(DetailDebugLevel) == TRUE)
                    LogPtr->fprintf(" %s Lock %s for Write Success.", LogPrefix.c_str(), ObjectName.c_str());
            }
            else
                LogPtr->fprintf(" %s Lock %s for Write Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
        }
        else
            LogPtr->fprintf(" %s Try Lock %s for Write Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    }
    else
    {
        if (BufferedLog::DebugLevelExist(DetailDebugLevel) == TRUE)
            LogPtr->fprintf(" %s Try Lock %s for Write Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    LogPtr->FlushToFile();
    return lockResult;
}  //TryAndLockObjectForWrite()
//------------------------------------------------------------------------------
int UnlockObjectForWrite(CBasicRWLockObject* ObjPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, Int32 DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = ObjPtr->UnLockForWrite();
    if (lockResult == 0)
    {
        if (BufferedLog::DebugLevelExist(DetailDebugLevel) == TRUE)
            LogPtr->fprintf(" %s Unlock %s for Write Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    else
        LogPtr->fprintf(" %s Unlock %s for Write Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    LogPtr->FlushToFile();
    return lockResult;
}  //UnlockObjectForWrite()
//------------------------------------------------------------------------------
int TryAndLockObjectForWrite(CBasicRWLockObject* ObjPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, bool WarningDebugLevel, bool DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = ObjPtr->TryLockForWrite();
    if (lockResult != 0)
    {
        if (lockResult == EBUSY)
        {
            if (WarningDebugLevel)
                LogPtr->fprintf(" %s Waiting %s Write Lock...", LogPrefix.c_str(), ObjectName.c_str());
            lockResult = ObjPtr->LockForWrite();
            if (lockResult == 0)
            {
                if (DetailDebugLevel)
                    LogPtr->fprintf(" %s Lock %s for Write Success.", LogPrefix.c_str(), ObjectName.c_str());
            }
            else
                LogPtr->fprintf(" %s Lock %s for Write Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
        }
        else
            LogPtr->fprintf(" %s Try Lock %s for Write Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    }
    else
    {
        if (DetailDebugLevel)
            LogPtr->fprintf(" %s Try Lock %s for Write Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    LogPtr->FlushToFile();
    return lockResult;
}  //TryAndLockObjectForWrite()
//------------------------------------------------------------------------------
int UnlockObjectForWrite(CBasicRWLockObject* ObjPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, bool DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = ObjPtr->UnLockForWrite();
    if (lockResult == 0)
    {
        if (DetailDebugLevel)
            LogPtr->fprintf(" %s Unlock %s for Write Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    else
        LogPtr->fprintf(" %s Unlock %s for Write Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    LogPtr->FlushToFile();
    return lockResult;
}  //UnlockObjectForWrite()

//------------------------------------------------------------------------------
int TryAndLockObject(UFC::PConditionMutex* ConditionLockPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, Int32 WarningDebugLevel, Int32 DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = ConditionLockPtr->TryLock();
    if (lockResult != 0)
    {
        if (lockResult == EBUSY)
        {
            if (BufferedLog::DebugLevelExist(WarningDebugLevel) == TRUE)
                LogPtr->fprintf(" %s Waiting %s Lock...", LogPrefix.c_str(), ObjectName.c_str());
            lockResult = ConditionLockPtr->Lock();
            if (lockResult == 0)
            {
                if (BufferedLog::DebugLevelExist(DetailDebugLevel) == TRUE)
                    LogPtr->fprintf(" %s Lock %s Success.", LogPrefix.c_str(), ObjectName.c_str());
            }
            else
                LogPtr->fprintf(" %s Lock %s Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
        }
        else
            LogPtr->fprintf(" %s Try Lock %s Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    }
    else
    {
        if (UFC::BufferedLog::DebugLevelExist(DetailDebugLevel) == TRUE)
            LogPtr->fprintf(" %s Try Lock %s Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    LogPtr->FlushToFile();
    return lockResult;
}  //TryAndLockObject()
//------------------------------------------------------------------------------
int UnlockObject(UFC::PConditionMutex* ConditionLockPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, Int32 DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = ConditionLockPtr->Unlock();
    if (lockResult == 0)
    {
        if (BufferedLog::DebugLevelExist(DetailDebugLevel) == TRUE)
            LogPtr->fprintf(" %s Unlock %s Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    else
        LogPtr->fprintf(" %s Unlock %s Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    LogPtr->FlushToFile();
    return lockResult;
}  //UnlockObject()
//------------------------------------------------------------------------------
int TryAndLockObject(UFC::PConditionMutex* ConditionLockPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, bool WarningDebugLevel, bool DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = ConditionLockPtr->TryLock();
    if (lockResult != 0)
    {
        if (lockResult == EBUSY)
        {
            if (WarningDebugLevel)
                LogPtr->fprintf(" %s Waiting %s Lock...", LogPrefix.c_str(), ObjectName.c_str());
            lockResult = ConditionLockPtr->Lock();
            if (lockResult == 0)
            {
                if (DetailDebugLevel)
                    LogPtr->fprintf(" %s Lock %s Success.", LogPrefix.c_str(), ObjectName.c_str());
            }
            else
                LogPtr->fprintf(" %s Lock %s Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
        }
        else
            LogPtr->fprintf(" %s Try Lock %s Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    }
    else
    {
        if (DetailDebugLevel)
            LogPtr->fprintf(" %s Try Lock %s Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    LogPtr->FlushToFile();
    return lockResult;
}  //TryAndLockObject()
//------------------------------------------------------------------------------
int UnlockObject(UFC::PConditionMutex* ConditionLockPtr, const AnsiString& LogPrefix, const AnsiString& ObjectName, bool DetailDebugLevel, BufferedLog* LogPtr)
{
    int lockResult = ConditionLockPtr->Unlock();
    if (lockResult == 0)
    {
        if (DetailDebugLevel)
            LogPtr->fprintf(" %s Unlock %s Success.", LogPrefix.c_str(), ObjectName.c_str());
    }
    else
        LogPtr->fprintf(" %s Unlock %s Failed:%d.", LogPrefix.c_str(), ObjectName.c_str(), lockResult);
    LogPtr->FlushToFile();
    return lockResult;
}  //UnlockObject()

#endif /* if (defined(__LINUX) || defined(__AIX)) */
}  //namespace UFC
