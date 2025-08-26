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
    UFC::AnsiString FSubAccount;
    UFC::AnsiString FAE;
    UFC::AnsiString FApKind;
    UFC::AnsiString FSourceIP;
    UFC::AnsiString FClOrdID;
    UFC::AnsiString FOrigClOrdID;
public:
    TUserData( const char* UserData );
private:
    void Parse( const UFC::AnsiString& Data );
public:
    UFC::AnsiString& GetSubAccount() { return FSubAccount; }
    UFC::AnsiString& GetAE() { return FAE; }
    UFC::AnsiString& GetApKind() { return FApKind; }
    UFC::AnsiString& GetSourceIP() { return FSourceIP; }
    UFC::AnsiString& GetClOrdID() { return FClOrdID; }
    UFC::AnsiString& GetOrigClOrdID() { return FOrigClOrdID; }
};
//------------------------------------------------------------------------------

#endif	/* _TUSERDATA_H */

