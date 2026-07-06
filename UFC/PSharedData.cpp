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

//------------------------------------------------------------------------------
//https://zhuanlan.zhihu.com/p/103336186
std::string Base64Encode( unsigned char const* buf, unsigned int bufLen )
{
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char charArray3[3];
    unsigned char charArray4[4];

    while( bufLen-- )
    {
        charArray3[i++] = *( buf++ );
        if( i == 3 )
        {
            charArray4[0] = ( charArray3[0] & 0xfc ) >> 2;
            charArray4[1] = ( ( charArray3[0] & 0x03 ) << 4 ) + ( ( charArray3[1] & 0xf0 ) >> 4 );
            charArray4[2] = ( ( charArray3[1] & 0x0f ) << 2 ) + ( ( charArray3[2] & 0xc0 ) >> 6 );
            charArray4[3] = charArray3[2] & 0x3f;

            for( i = 0; ( i < 4 ) ; i++ )
                ret += Base64Chars[ charArray4[i] ];
            i = 0;
        }  //if( i == 3 )
    }  //while( bufLen-- )

    if( i )
    {
        for( j = i; j < 3; j++ )
            charArray3[j] = '\0';

        charArray4[0] = ( charArray3[0] & 0xfc ) >> 2;
        charArray4[1] = ( ( charArray3[0] & 0x03 ) << 4 ) + ( ( charArray3[1] & 0xf0 ) >> 4 );
        charArray4[2] = ( ( charArray3[1] & 0x0f ) << 2 ) + ( ( charArray3[2] & 0xc0 ) >> 6 );
        charArray4[3] = charArray3[2] & 0x3f;

        for( j = 0; ( j < i + 1 ); j++ )
            ret += Base64Chars[ charArray4[j] ];

        while( ( i++ < 3 ) )
            ret += '=';
    }

    return ret;
}  //Base64Encode()
//------------------------------------------------------------------------------
std::string Base64Decode( const std::string& EncodedString )
{
    size_t in_len = EncodedString.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char charArray4[4], charArray3[3];
    std::string ret;

    while( in_len-- && ( EncodedString[in_] != '=' ) && IsBase64Char( EncodedString[in_] ) )
    {
        charArray4[i++] = EncodedString[in_]; in_++;
        if ( i == 4 )
        {
            for( i = 0; i < 4; i++ )
                charArray4[i] = Base64Chars.find( charArray4[i] ) & 0xff;

            charArray3[0] = ( charArray4[0] << 2 ) + ( ( charArray4[1] & 0x30 ) >> 4 );
            charArray3[1] = ( ( charArray4[1] & 0xf ) << 4) + ( ( charArray4[2] & 0x3c ) >> 2 );
            charArray3[2] = ( ( charArray4[2] & 0x3 ) << 6) + charArray4[3];

            for( i = 0; ( i < 3 ); i++ )
                ret += charArray3[i];
            i = 0;
        }
    }

    if (i)
    {
        for( j = 0; j < i; j++ )
            charArray4[j] = Base64Chars.find(charArray4[j]) & 0xff;

        charArray3[0] = ( charArray4[0] << 2 ) + ( ( charArray4[1] & 0x30) >> 4 );
        charArray3[1] = ( ( charArray4[1] & 0xf ) << 4 ) + ( ( charArray4[2] & 0x3c ) >> 2 );

        for( j = 0; ( j < i - 1 ); j++ ) ret += charArray3[j];
    }

    return ret;
}  //Base64Decode

#endif /* if (defined(__LINUX) || defined(__AIX)) */
}  //namespace UFC
