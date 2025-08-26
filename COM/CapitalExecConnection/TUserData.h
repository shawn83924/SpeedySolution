/* 
 * File:   TUserData.h
 * Author: yuan
 *
 * Created on December 12, 2008, 12:03 PM
 */

#ifndef _TUSERDATA_H
#define	_TUSERDATA_H
//------------------------------------------------------------------------------
#include "../UFC/UFC.h"
//------------------------------------------------------------------------------
class TUserData
{
private:
    UFC::AnsiString FOriginalSubAccount;
    UFC::AnsiString FSubAccount;
    UFC::AnsiString FAE;
    UFC::AnsiString FApKind;
    UFC::AnsiString FSourceIP;
    UFC::AnsiString FPE;
    UFC::AnsiString FOriginalUserData;
public:
    TUserData( const char* UserData );
private:
    bool IsProxyUserData( const UFC::AnsiString& UserData, UFC::AnsiString& Data );
    void Parse( const UFC::AnsiString& Data );
public:
    UFC::AnsiString& GetOriginalSubAccount() { return FOriginalSubAccount; }
    UFC::AnsiString& GetSubAccount() { return FSubAccount; }
    UFC::AnsiString& GetAE() { return FAE; }
    UFC::AnsiString& GetApKind() { return FApKind; }
    UFC::AnsiString& GetSourceIP() { return FSourceIP; }
    UFC::AnsiString& GetPE() { return FPE; }
    void DumpData();
};
//------------------------------------------------------------------------------

#endif	/* _TUSERDATA_H */

