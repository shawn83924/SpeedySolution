//------------------------------------------------------------------------------------------------------------------------
#ifndef __RiskManager_H
#define __RiskManager_H
//------------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../UFC/iniFile.h"
#include "../Migo/Sigo.h"
#include "AccountStore.h"
#include "CheckManager.h"
//------------------------------------------------------------------------------------------------------------------------
// File Path
//------------------------------------------------------------------------------------------------------------------------
#define LOG_BUFFER_SIZE 1024
#define FP_AccountStore		"../dat/oflow.ACCOUNT.DATA"
//------------------------------------------------------------------------------------------------------------------------
// global Objects
//------------------------------------------------------------------------------------------------------------------------
extern MessageObject*		gMessageObj;
extern UFC::UiniFile*		gCfgFlowEngine;
//------------------------------------------------------------------------------------------------------------------------
extern UFC::BufferedLog*	gOrderLog;
//----------------------------------------------------------------------------------------------------------------------
class ProcessMonitor : public MonitorListener
{
public:
    virtual void OnProcessStartup( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}
    virtual void OnProcessStopped( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}
    virtual void OnProcessConnected( BOOL IsTheFirstOne );
    virtual void OnProcessList( UFC::PStringList& Processs ){}
    virtual void OnConnected( void ){}
    virtual void OnDisconnected( void ){}
};
//------------------------------------------------------------------------------------------------------------------------
// global Functions
//------------------------------------------------------------------------------------------------------------------------
extern void LogRQ_Check(  const UFC::AnsiString& Order );
extern void LogSQ_CheckReply(  const UFC::AnsiString& ResultMsg, int Result  );
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
