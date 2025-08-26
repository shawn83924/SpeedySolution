/* 
 * File:   TandemAdapter.h
 * Author: yuan
 *
 * Created on September 22, 2008, 1:00 AM
 */

#ifndef _TANDEMADAPTER_H
#define	_TANDEMADAPTER_H
#include "CommandListener.h"
//------------------------------------------------------------------------------
class TandemAdapter
{
private:
    UFC::AnsiString         FAppName;
    UFC::AnsiString         FRemoteIP;
    UInt32                  FPort;
    TCommandListener*       FCommandListener;
    TMessageStore*          FMessageStore;
    int                     FClientHBInterval;
    int                     FServerHBInterval;
    UFC::PHashedSet<UFC::AnsiString> FAccountSet;
public:
    TandemAdapter(bool NeedRecover, UFC::AnsiString TandemIP, UInt32 TandemPort, TMessageStore* ptrMessageStore);
private:
    void LoadConfig( void );
    void LoadAccount( void );
    void AddToTable( UFC::AnsiString Buffer );
public:
    void Run( void );
};
//------------------------------------------------------------------------------
#endif	/* _TANDEMADAPTER_H */

