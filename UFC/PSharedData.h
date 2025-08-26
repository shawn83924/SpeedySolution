/* 
 * File:   PSharedData.h
 * Author: Zhen Fan
 *
 * Created on 2016/11/24 10:58
 */

#ifndef PSHAREDDATA_H
#define PSHAREDDATA_H

#if (defined(__LINUX) || defined(__AIX))
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <pthread.h>

#include "UFCType.h"
#include "AnsiString.h"
#include "Exception.h"
#include "BufferedLog.h"
#include "PInt32.h"
#include "PThread.h"

namespace UFC
{
//------------------------------------------------------------------------------
template<typename DigitalType>
class PSharedDigital
{
private:
    AnsiString FSharedMemoryName;
    int        FSharedMemoryID;
    
    void*             FSharedMemoryPtr;
    int*              FInitialLockErrorNoPtr;
    pthread_rwlock_t* FReadWriteLockStructPtr;
    DigitalType*      FDataPtr;
    
public:
    PSharedDigital();
    PSharedDigital(AnsiString& Name);
    PSharedDigital(AnsiString& Name, DigitalType Value);
    ~PSharedDigital();
    
    void Attach(key_t SharedKey);
    void Attach(key_t SharedKey, DigitalType Value);
    void Attach(const AnsiString& Name);
    void Attach(const AnsiString& Name, DigitalType Value);
    void Detach();
    void Remove();

    int Lock(RWLockTypeEnum LockType = rwNotLock);
    int Unlock();

    int LockForRead();
    int UnlockForRead() {return Unlock();};

    int LockForWrite();
    int UnlockForWrite() {return Unlock();};
    
    DigitalType GetValue();
    DigitalType GetValue(RWLockTypeEnum LockType);
    void SetValue(DigitalType NewValue);
    void SetValue(DigitalType NewValue, RWLockTypeEnum LockType);

    void IncreaseValue(DigitalType Value);
    void IncreaseValue(DigitalType Value, RWLockTypeEnum LockType);
    
    PSharedDigital& operator =(DigitalType NewValue);
    PSharedDigital& operator +=(DigitalType Value);
};  //class PSharedDigital
//---------------------------------------------------------------------------
template<typename DigitalType>
PSharedDigital<DigitalType>::PSharedDigital()
:FSharedMemoryName("")
,FSharedMemoryID(-1)
,FSharedMemoryPtr(0)
,FInitialLockErrorNoPtr(0)
,FReadWriteLockStructPtr(0)
,FDataPtr(0)
{
}  //PSharedDigital::PSharedDigital()
//---------------------------------------------------------------------------
template<typename DigitalType>
PSharedDigital<DigitalType>::PSharedDigital(AnsiString& Name)
:FSharedMemoryName(Name)
,FSharedMemoryID(-1)
,FSharedMemoryPtr(0)
,FInitialLockErrorNoPtr(0)
,FReadWriteLockStructPtr(0)
,FDataPtr(0)
{
    Attach(Name);
}  //PSharedDigital::PSharedDigital()
//---------------------------------------------------------------------------
template<typename DigitalType>
PSharedDigital<DigitalType>::PSharedDigital(AnsiString& Name, DigitalType Value)
:FSharedMemoryName(Name)
,FSharedMemoryID(-1)
,FSharedMemoryPtr(0)
,FInitialLockErrorNoPtr(0)
,FReadWriteLockStructPtr(0)
,FDataPtr(0)
{
    Attach(Name, Value);
}  //PSharedDigital::PSharedDigital()
//---------------------------------------------------------------------------
template<typename DigitalType>
PSharedDigital<DigitalType>::~PSharedDigital()
{
    Detach();
}  //PSharedDigital::PSharedDigital()
//---------------------------------------------------------------------------
template <typename DigitalType>
void PSharedDigital<DigitalType>::Attach(key_t SharedKey)
{
    Detach();
    bool isNewSharedMemory = false;
    size_t sizeOfLockErrorNo = sizeof(int);
    size_t sizeOfLock = sizeof(pthread_rwlock_t);
    size_t sizeOfData = sizeof(DigitalType);
    size_t totalSize = sizeOfLockErrorNo + sizeOfLock + sizeOfData;
    FSharedMemoryID = shmget(SharedKey, totalSize, 0666);
    if ((FSharedMemoryID == -1) && (errno == ENOENT))
    {
        FSharedMemoryID = shmget(SharedKey, totalSize, IPC_CREAT | 0666);
        isNewSharedMemory = true;
    }
    if (FSharedMemoryID == -1) throw(ShareMenException(errno));
    
    FSharedMemoryPtr = shmat(FSharedMemoryID, NULL, 0);
    if (FSharedMemoryPtr == (void*)-1) throw(ShareMenException(errno));

    FInitialLockErrorNoPtr = static_cast<int*>(FSharedMemoryPtr);
    FReadWriteLockStructPtr = static_cast<pthread_rwlock_t*>(static_cast<void*>(FInitialLockErrorNoPtr + 1));
    FDataPtr = static_cast<DigitalType*>(static_cast<void*>(FReadWriteLockStructPtr + 1));
    if (isNewSharedMemory)
    {
        UFC::BufferedLog::Printf(" Attach new Shared Memory[%s][%d] size[%ld] dataSize[%ld]", FSharedMemoryName.c_str(), FSharedMemoryID, totalSize, sizeOfData); 
        int initialLockErrorNo = 0;
        pthread_rwlockattr_t attributeOfLock;
        initialLockErrorNo = pthread_rwlockattr_init(&attributeOfLock);
        if (initialLockErrorNo == 0)
        {
            initialLockErrorNo = pthread_rwlockattr_setpshared(&attributeOfLock, PTHREAD_PROCESS_SHARED);
            if (initialLockErrorNo == 0)
            {
                initialLockErrorNo = pthread_rwlock_init(FReadWriteLockStructPtr, &attributeOfLock);
                if (initialLockErrorNo != 0)
                    UFC::BufferedLog::Printf(" Initial Read/Write Lock Failed! ErrorCode=%d", initialLockErrorNo);
            }
            else
                UFC::BufferedLog::Printf(" Set Read/Write Lock Attribute Failed! ErrorCode=%d", initialLockErrorNo);
            pthread_rwlockattr_destroy(&attributeOfLock);
        }
        else
            UFC::BufferedLog::Printf(" Initial Read/Write Lock Attribute Failed! ErrorCode=%d", initialLockErrorNo);
        
        *FInitialLockErrorNoPtr = initialLockErrorNo;
    }  //if (isNewSharedMemory)
    else
        UFC::BufferedLog::Printf(" Attach Shared Memory[%s][%d] size[%ld] dataSize[%ld]", FSharedMemoryName.c_str(), FSharedMemoryID, totalSize, sizeOfData);
}
//---------------------------------------------------------------------------
template <typename DigitalType>
void PSharedDigital<DigitalType>::Attach(key_t SharedKey, DigitalType Value)
{
    Attach(SharedKey);
    *FDataPtr = Value;
}
//---------------------------------------------------------------------------
template <typename DigitalType>
void PSharedDigital<DigitalType>::Attach(const AnsiString& Name)
{
    FSharedMemoryName = Name;
    Attach(GetShareMemoryUniqueKey(Name));
}
//---------------------------------------------------------------------------
template <typename DigitalType>
void PSharedDigital<DigitalType>::Attach(const AnsiString& Name, DigitalType Value)
{
    FSharedMemoryName = Name;
    Attach(GetShareMemoryUniqueKey(Name), Value);
}
//---------------------------------------------------------------------------
template <typename DigitalType>
void PSharedDigital<DigitalType>::Detach()
{
    if (FSharedMemoryPtr != 0)
    {
        shmdt(FSharedMemoryPtr);
        FSharedMemoryPtr = 0;
        FReadWriteLockStructPtr = 0;
        FDataPtr = 0;
    }
}
//---------------------------------------------------------------------------
template <typename DigitalType>
void PSharedDigital<DigitalType>::Remove()
{
    if (FDataPtr >= 0)
    {
        Detach();
        shmid_ds ds;
        if (shmctl(FSharedMemoryID, IPC_STAT, &ds) == 0)
            if (shmctl(FSharedMemoryID, IPC_RMID, &ds) == 0)
                FSharedMemoryID = -1;
        
        if (FSharedMemoryID >= 0) throw(ShareMenException(errno));
        
        if (FSharedMemoryName.Length() > 0)
        {
            UFC::AnsiString FileName = ShmNameToFileName(FSharedMemoryName);
            UFC::RemoveFile(FileName);
        }        
    }
}
//---------------------------------------------------------------------------
template <typename DigitalType>
int PSharedDigital<DigitalType>::Lock(RWLockTypeEnum LockType)
{
    if (*FInitialLockErrorNoPtr != 0) return *FInitialLockErrorNoPtr;
    if (LockType == rwLockForRead)
        return pthread_rwlock_rdlock(FReadWriteLockStructPtr); 
    else if (LockType == rwLockForWrite)
        return pthread_rwlock_wrlock(FReadWriteLockStructPtr); 
    else
        return 0;
}
//---------------------------------------------------------------------------
template <typename DigitalType>
int PSharedDigital<DigitalType>::Unlock()
{
    if (*FInitialLockErrorNoPtr != 0) return *FInitialLockErrorNoPtr;
    return pthread_rwlock_unlock(FReadWriteLockStructPtr);    
}
//---------------------------------------------------------------------------
template <typename DigitalType>
int PSharedDigital<DigitalType>::LockForRead()
{
    if (*FInitialLockErrorNoPtr != 0) return *FInitialLockErrorNoPtr;
    return pthread_rwlock_rdlock(FReadWriteLockStructPtr); 
}
//---------------------------------------------------------------------------
template <typename DigitalType>
int PSharedDigital<DigitalType>::LockForWrite()
{
    if (*FInitialLockErrorNoPtr != 0) return *FInitialLockErrorNoPtr;
    return pthread_rwlock_wrlock(FReadWriteLockStructPtr); 
}
//---------------------------------------------------------------------------
template <typename DigitalType>
DigitalType PSharedDigital<DigitalType>::GetValue()
{
    DigitalType dataValue = static_cast<DigitalType>(0);
    LockForRead();
    if (FDataPtr != 0) dataValue = *FDataPtr;
    Unlock();
    return dataValue;
}
//---------------------------------------------------------------------------
template <typename DigitalType>
DigitalType PSharedDigital<DigitalType>::GetValue(RWLockTypeEnum LockType)
{
    DigitalType dataValue = static_cast<DigitalType>(0);
    Lock(LockType);
    if (FDataPtr != 0) dataValue = *FDataPtr;
    if (LockType != rwNotLock) Unlock();
    return dataValue;
}
//---------------------------------------------------------------------------
template <typename DigitalType>
void PSharedDigital<DigitalType>::SetValue(DigitalType NewValue)
{
    LockForWrite();
    if (FDataPtr != 0) *FDataPtr = NewValue;
    Unlock();
}
//---------------------------------------------------------------------------
template <typename DigitalType>
void PSharedDigital<DigitalType>::SetValue(DigitalType NewValue, RWLockTypeEnum LockType)
{
    Lock(LockType);
    if (FDataPtr != 0) *FDataPtr = NewValue;
    if (LockType != rwNotLock) Unlock();
}
//---------------------------------------------------------------------------
template <typename DigitalType>
void PSharedDigital<DigitalType>::IncreaseValue(DigitalType Value)
{
    LockForWrite();
    if (FDataPtr != 0) *FDataPtr += Value;
    Unlock();
}
//---------------------------------------------------------------------------
template <typename DigitalType>
void PSharedDigital<DigitalType>::IncreaseValue(DigitalType Value, RWLockTypeEnum LockType)
{
    Lock(LockType);
    if (FDataPtr != 0) *FDataPtr += Value;
    if (LockType != rwNotLock) Unlock();
}
//---------------------------------------------------------------------------
template <typename DigitalType>
PSharedDigital<DigitalType>& PSharedDigital<DigitalType>::operator =(DigitalType NewValue)
{
    SetValue(NewValue);
    return *this;
}
//---------------------------------------------------------------------------
template <typename DigitalType>
PSharedDigital<DigitalType>& PSharedDigital<DigitalType>::operator +=(DigitalType Value)
{
    IncreaseValue(Value);
    return *this;
}

//---------------------------------------------------------------------------
typedef PSharedDigital<UFCType::Int64> PInt64;
typedef PSharedDigital<double>         PDouble;

//------------------------------------------------------------------------------
template<typename SequanceNoType>
class PSharedSequanceNo
{
private:
    const static int FDefaultInitialValue = 0;
    const static int FDefaultMinValue     = 0;
    const static int FDefaultMaxValue     = 2000000000;
    
    AnsiString FSharedMemoryName;
    int        FSharedMemoryID;
    
    void*             FSharedMemoryPtr;
    int*              FInitialLockErrorNoPtr;
    pthread_rwlock_t* FReadWriteLockStructPtr;
    SequanceNoType*   FCurrentValuePtr;
    SequanceNoType*   FMinValuePtr;
    SequanceNoType*   FMaxValuePtr;
    
public:
    PSharedSequanceNo();
    PSharedSequanceNo(AnsiString& Name, SequanceNoType InitialValue, SequanceNoType MinValue, SequanceNoType MaxValue);
    PSharedSequanceNo(AnsiString& Name);
    ~PSharedSequanceNo();
    
    void Attach(key_t SharedKey, SequanceNoType InitialValue, SequanceNoType MinValue, SequanceNoType MaxValue);
    void Attach(key_t SharedKey);
    void Attach(const AnsiString& Name, SequanceNoType InitialValue, SequanceNoType MinValue, SequanceNoType MaxValue);
    void Attach(const AnsiString& Name);
    void Detach();
    void Remove();

    int Lock(RWLockTypeEnum LockType = rwNotLock);
    int Unlock();

    int LockForRead();
    int UnlockForRead() {return Unlock();};

    int LockForWrite();
    int UnlockForWrite() {return Unlock();};
    
    SequanceNoType GetCurrentValue(RWLockTypeEnum LockType);
    SequanceNoType GetCurrentValue() {return GetCurrentValue(rwLockForRead);};
    void SetCurrentValue(SequanceNoType NewValue, RWLockTypeEnum LockType);
    void SetCurrentValue(SequanceNoType NewValue) {SetCurrentValue(NewValue, rwLockForWrite);};

    SequanceNoType GetMinValue(RWLockTypeEnum LockType);
    SequanceNoType GetMinValue() {return GetMinValue(rwLockForRead);};
    void SetMinValue(SequanceNoType NewValue, RWLockTypeEnum LockType);
    void SetMinValue(SequanceNoType NewValue) {SetMinValue(NewValue, rwLockForWrite);};
    
    SequanceNoType GetMaxValue(RWLockTypeEnum LockType);
    SequanceNoType GetMaxValue() {return GetMaxValue(rwLockForRead);};
    void SetMaxValue(SequanceNoType NewValue, RWLockTypeEnum LockType);
    void SetMaxValue(SequanceNoType NewValue) {SetMaxValue(NewValue, rwLockForWrite);};
    
    bool IncreaseCurrentValue(SequanceNoType Value, RWLockTypeEnum LockType);
    bool IncreaseCurrentValue(SequanceNoType Value) {return IncreaseCurrentValue(Value, rwLockForWrite);};
    
    PSharedSequanceNo& operator =(SequanceNoType NewValue);
    PSharedSequanceNo& operator +=(SequanceNoType Value);
    
    SequanceNoType GetNextValue(SequanceNoType IncreaseValue, RWLockTypeEnum LockType);
    SequanceNoType GetNextValue(SequanceNoType IncreaseValue) {return GetNextValue(IncreaseValue, rwLockForWrite);};
    SequanceNoType GetNextValue(RWLockTypeEnum LockType) {return GetNextValue(1, LockType);};
    SequanceNoType GetNextValue() {return GetNextValue(1, rwLockForWrite);};
};  //class PSharedSequanceNo
//---------------------------------------------------------------------------
template<typename SequanceNoType>
PSharedSequanceNo<SequanceNoType>::PSharedSequanceNo()
:FSharedMemoryName("")
,FSharedMemoryID(-1)
,FSharedMemoryPtr(0)
,FInitialLockErrorNoPtr(0)
,FReadWriteLockStructPtr(0)
,FCurrentValuePtr(0)
,FMinValuePtr(0)
,FMaxValuePtr(0)
{
}  //PSharedSequanceNo::PSharedSequanceNo()
//---------------------------------------------------------------------------
template<typename SequanceNoType>
PSharedSequanceNo<SequanceNoType>::PSharedSequanceNo(AnsiString& Name, SequanceNoType InitialValue, SequanceNoType MinValue, SequanceNoType MaxValue)
:FSharedMemoryName(Name)
,FSharedMemoryID(-1)
,FSharedMemoryPtr(0)
,FInitialLockErrorNoPtr(0)
,FReadWriteLockStructPtr(0)
,FCurrentValuePtr(0)
,FMinValuePtr(0)
,FMaxValuePtr(0)
{
    Attach(Name, InitialValue, MinValue, MaxValue);
}  //PSharedSequanceNo::PSharedSequanceNo()
//---------------------------------------------------------------------------
template<typename SequanceNoType>
PSharedSequanceNo<SequanceNoType>::PSharedSequanceNo(AnsiString& Name)
:FSharedMemoryName(Name)
,FSharedMemoryID(-1)
,FSharedMemoryPtr(0)
,FInitialLockErrorNoPtr(0)
,FReadWriteLockStructPtr(0)
,FCurrentValuePtr(0)
,FMinValuePtr(0)
,FMaxValuePtr(0)
{
    Attach(Name);
}  //PSharedSequanceNo::PSharedSequanceNo()
//---------------------------------------------------------------------------
template<typename SequanceNoType>
PSharedSequanceNo<SequanceNoType>::~PSharedSequanceNo()
{
    Detach();
}  //PSharedSequanceNo::PSharedSequanceNo()
//---------------------------------------------------------------------------
template <typename SequanceNoType>
void PSharedSequanceNo<SequanceNoType>::Attach(key_t SharedKey)
{
    Detach();
    bool isNewSharedMemory = false;
    size_t sizeOfLockErrorNo = sizeof(int);
    size_t sizeOfLock = sizeof(pthread_rwlock_t);
    size_t sizeOfData = 3 * sizeof(SequanceNoType);
    size_t totalSize = sizeOfLockErrorNo + sizeOfLock + sizeOfData;
    FSharedMemoryID = shmget(SharedKey, totalSize, 0666);
    if ((FSharedMemoryID == -1) && (errno == ENOENT))
    {
        FSharedMemoryID = shmget(SharedKey, totalSize, IPC_CREAT | 0666);
        isNewSharedMemory = true;
    }
    if (FSharedMemoryID == -1) throw(ShareMenException(errno));
    
    FSharedMemoryPtr = shmat(FSharedMemoryID, NULL, 0);
    if (FSharedMemoryPtr == (void*)-1) throw(ShareMenException(errno));

    FInitialLockErrorNoPtr = static_cast<int*>(FSharedMemoryPtr);
    FReadWriteLockStructPtr = static_cast<pthread_rwlock_t*>(static_cast<void*>(FInitialLockErrorNoPtr + 1));
    FCurrentValuePtr = static_cast<SequanceNoType*>(static_cast<void*>(FReadWriteLockStructPtr + 1));
    FMinValuePtr = static_cast<SequanceNoType*>(static_cast<void*>(FCurrentValuePtr + 1));
    FMaxValuePtr = static_cast<SequanceNoType*>(static_cast<void*>(FMinValuePtr + 1));
    if (isNewSharedMemory)
    {
        UFC::BufferedLog::Printf(" Attach new Shared Memory[%s][%d] size[%ld] dataSize[%ld]", FSharedMemoryName.c_str(), FSharedMemoryID, totalSize, sizeOfData); 
        int initialLockErrorNo = 0;
        pthread_rwlockattr_t attributeOfLock;
        initialLockErrorNo = pthread_rwlockattr_init(&attributeOfLock);
        if (initialLockErrorNo == 0)
        {
            initialLockErrorNo = pthread_rwlockattr_setpshared(&attributeOfLock, PTHREAD_PROCESS_SHARED);
            if (initialLockErrorNo == 0)
            {
                initialLockErrorNo = pthread_rwlock_init(FReadWriteLockStructPtr, &attributeOfLock);
                if (initialLockErrorNo != 0)
                    UFC::BufferedLog::Printf(" Initial Read/Write Lock Failed! ErrorCode=%d", initialLockErrorNo);
            }
            else
                UFC::BufferedLog::Printf(" Set Read/Write Lock Attribute Failed! ErrorCode=%d", initialLockErrorNo);
            pthread_rwlockattr_destroy(&attributeOfLock);
        }
        else
            UFC::BufferedLog::Printf(" Initial Read/Write Lock Attribute Failed! ErrorCode=%d", initialLockErrorNo);
        
        *FInitialLockErrorNoPtr = initialLockErrorNo;
    }  //if (isNewSharedMemory)
    else
        UFC::BufferedLog::Printf(" Attach Shared Memory[%s][%d] size[%ld] dataSize[%ld]", FSharedMemoryName.c_str(), FSharedMemoryID, totalSize, sizeOfData);
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
void PSharedSequanceNo<SequanceNoType>::Attach(key_t SharedKey, SequanceNoType InitialValue, SequanceNoType MinValue, SequanceNoType MaxValue)
{
    Attach(SharedKey);
    *FCurrentValuePtr = InitialValue;
    *FMinValuePtr = MinValue;
    *FMaxValuePtr = MaxValue;
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
void PSharedSequanceNo<SequanceNoType>::Attach(const AnsiString& Name, SequanceNoType InitialValue, SequanceNoType MinValue, SequanceNoType MaxValue)
{
    FSharedMemoryName = Name;
    Attach(GetShareMemoryUniqueKey(Name), InitialValue, MinValue, MaxValue);
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
void PSharedSequanceNo<SequanceNoType>::Attach(const AnsiString& Name)
{
    FSharedMemoryName = Name;
    Attach(GetShareMemoryUniqueKey(Name));
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
void PSharedSequanceNo<SequanceNoType>::Detach()
{
    if (FSharedMemoryPtr != 0)
    {
        shmdt(FSharedMemoryPtr);
        FSharedMemoryPtr = 0;
        FReadWriteLockStructPtr = 0;
        FCurrentValuePtr = 0;
        FMinValuePtr = 0;
        FMaxValuePtr = 0;
    }
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
void PSharedSequanceNo<SequanceNoType>::Remove()
{
    if (FCurrentValuePtr >= 0)
    {
        Detach();
        shmid_ds ds;
        if (shmctl(FSharedMemoryID, IPC_STAT, &ds) == 0)
            if (shmctl(FSharedMemoryID, IPC_RMID, &ds) == 0)
                FSharedMemoryID = -1;
        
        if (FSharedMemoryID >= 0) throw(ShareMenException(errno));
        
        if (FSharedMemoryName.Length() > 0)
        {
            UFC::AnsiString FileName = ShmNameToFileName(FSharedMemoryName);
            UFC::RemoveFile(FileName);
        }        
    }
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
int PSharedSequanceNo<SequanceNoType>::Lock(RWLockTypeEnum LockType)
{
    if (*FInitialLockErrorNoPtr != 0) return *FInitialLockErrorNoPtr;
    if (LockType == rwLockForRead)
        return pthread_rwlock_rdlock(FReadWriteLockStructPtr); 
    else if (LockType == rwLockForWrite)
        return pthread_rwlock_wrlock(FReadWriteLockStructPtr); 
    else
        return 0;
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
int PSharedSequanceNo<SequanceNoType>::Unlock()
{
    if (*FInitialLockErrorNoPtr != 0) return *FInitialLockErrorNoPtr;
    return pthread_rwlock_unlock(FReadWriteLockStructPtr);
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
int PSharedSequanceNo<SequanceNoType>::LockForRead()
{
    if (*FInitialLockErrorNoPtr != 0) return *FInitialLockErrorNoPtr;
    return pthread_rwlock_rdlock(FReadWriteLockStructPtr); 
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
int PSharedSequanceNo<SequanceNoType>::LockForWrite()
{
    if (*FInitialLockErrorNoPtr != 0) return *FInitialLockErrorNoPtr;
    return pthread_rwlock_wrlock(FReadWriteLockStructPtr); 
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
SequanceNoType PSharedSequanceNo<SequanceNoType>::GetCurrentValue(RWLockTypeEnum LockType)
{
    SequanceNoType currentValue = static_cast<SequanceNoType>(0);
    Lock(LockType);
    if (FCurrentValuePtr != 0) currentValue = *FCurrentValuePtr;
    if (LockType != rwNotLock) Unlock();
    return currentValue;
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
void PSharedSequanceNo<SequanceNoType>::SetCurrentValue(SequanceNoType NewValue, RWLockTypeEnum LockType)
{
    Lock(LockType);
    if (FCurrentValuePtr != 0) *FCurrentValuePtr = NewValue;
    if (LockType != rwNotLock) Unlock();
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
SequanceNoType PSharedSequanceNo<SequanceNoType>::GetMinValue(RWLockTypeEnum LockType)
{
    SequanceNoType minValue = static_cast<SequanceNoType>(0);
    Lock(LockType);
    if (FMinValuePtr != 0) minValue = *FMinValuePtr;
    if (LockType != rwNotLock) Unlock();
    return minValue;
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
void PSharedSequanceNo<SequanceNoType>::SetMinValue(SequanceNoType NewValue, RWLockTypeEnum LockType)
{
    Lock(LockType);
    if (FMinValuePtr != 0) *FMinValuePtr = NewValue;
    if (LockType != rwNotLock) Unlock();
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
SequanceNoType PSharedSequanceNo<SequanceNoType>::GetMaxValue(RWLockTypeEnum LockType)
{
    SequanceNoType maxValue = static_cast<SequanceNoType>(0);
    Lock(LockType);
    if (FMaxValuePtr != 0) maxValue = *FMaxValuePtr;
    if (LockType != rwNotLock) Unlock();
    return maxValue;
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
void PSharedSequanceNo<SequanceNoType>::SetMaxValue(SequanceNoType NewValue, RWLockTypeEnum LockType)
{
    Lock(LockType);
    if (FMaxValuePtr != 0) *FMaxValuePtr = NewValue;
    if (LockType != rwNotLock) Unlock();
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
bool PSharedSequanceNo<SequanceNoType>::IncreaseCurrentValue(SequanceNoType Value, RWLockTypeEnum LockType)
{
    bool isExceedLimit = true;
    SequanceNoType newValue;
    Lock(LockType);
    if (FCurrentValuePtr != 0)
    {
        newValue = *FCurrentValuePtr + Value;
        if ((FMaxValuePtr != 0) && (FMinValuePtr != 0))
        {
            if (newValue > *FMaxValuePtr) 
                newValue = *FMinValuePtr;
            else if (newValue < *FMinValuePtr)
                newValue = *FMaxValuePtr;
            else
                isExceedLimit = false;
        }
        *FCurrentValuePtr = newValue;
    }
    if (LockType != rwNotLock) Unlock();
    return isExceedLimit;
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
PSharedSequanceNo<SequanceNoType>& PSharedSequanceNo<SequanceNoType>::operator =(SequanceNoType NewValue)
{
    SetCurrentValue(NewValue);
    return *this;
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
PSharedSequanceNo<SequanceNoType>& PSharedSequanceNo<SequanceNoType>::operator +=(SequanceNoType Value)
{
    IncreaseCurrentValue(Value);
    return *this;
}
//---------------------------------------------------------------------------
template <typename SequanceNoType>
SequanceNoType PSharedSequanceNo<SequanceNoType>::GetNextValue(SequanceNoType IncreaseValue, RWLockTypeEnum LockType)
{
    Lock(LockType);
    IncreaseCurrentValue(IncreaseValue, UFC::rwNotLock);
    SequanceNoType nextValue = *FCurrentValuePtr;
    if (LockType != rwNotLock) Unlock();
    return nextValue;
}

typedef PSharedSequanceNo<UFCType::Int64> PInt64SeqNo;
typedef PSharedSequanceNo<UFCType::Int32> PInt32SeqNo;
}  //namespace UFC
#endif /* if (defined(__LINUX) || defined(__AIX)) */
#endif /* PSHAREDDATA_H */
