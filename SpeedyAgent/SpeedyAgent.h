//----------------------------------------------------------------------------------------------------------------------
#ifndef __SIMTFX_AGENT_H
#define __SIMTFX_AGENT_H
//----------------------------------------------------------------------------------------------------------------------
#include <signal.h>
#include <fstream>
#include <limits.h>
#include "../Migo/Sigo.h"
#include "../UFC/iniFile.h"
#include "../UFC/NetUtility.h"
#include "../UFC/AnsiString.h"
//----------------------------------------------------------------------------------------------------------------------
const Int32 LOG_MAX_LINE    = 1024;
const Int32 MAX_LINE_BUFFER = 2048;
const Int32 MAX_LOG_SIZE    = 204800; ///< 200K
//----------------------------------------------------------------------------------------------------------------------
const int MAX_PROCESS      = 64;
const int MAX_PROCESS_NAME = 128;
//----------------------------------------------------------------------------------------------------------------------
const int SPEEDY_AGENT_ACK    = 1;
const int SPEEDY_AGENT_REPLY  = 2;
//----------------------------------------------------------------------------------------------------------------------
const int SPEEDY_GET_LIST     = 3;
const int SPEEDY_REPY_LIST    = 4;
//----------------------------------------------------------------------------------------------------------------------
const int SPEEDY_GET_PROCESS  = 5;
const int SPEEDY_REPY_PROCESS = 6;
//----------------------------------------------------------------------------------------------------------------------
const int SPEEDY_RUN_PROCESS        = 7;
const int SPEEDY_KILL_PROCESS       = 8;
const int SPEEDY_RESTART_PROCESS    = 9;

const int SPEEDY_GET_PROCESS_LIST   = 21;
const int SPEEDY_REPLY_PROCESS_LIST = 22;
const int SPEEDY_MODIFY_PROCESS     = 23;
const int SPEEDY_GET_CONFIG_FILE    = 24;
const int SPEEDY_REPLY_CONFIG_FILE  = 25;
const int SPEEDY_SET_CONFIG_FILE    = 26;
const int SPEEDY_CONFIG_FILE_SAVED  = 27;
const int SPEEDY_RUNCLEAR_PROCESS   = 28;
const int SPEEDY_CANCEL_FUT_ORDERS  = 31;
const int SPEEDY_CANCEL_OPT_ORDERS  = 32;
//----------------------------------------------------------------------------------------------------------------------
const int SPEEDY_MODIFY_PVC         = 10;
const int SPEEDY_CHANGE_FIX_SETTING = 13;
const int SPEEDY_GET_FIX_SETTING    = 14;
const int SPEEDY_REPLY_FIX_SETTING  = 15;
const int SPEEDY_MONITOR_PVC        = 16;
const int SPEEDY_REPLY_PVC_LOG      = 17;
//----------------------------------------------------------------------------------------------------------------------
const int PROCESS_STOP       = 0;
const int PROCESS_START      = 1;
const int PROCESS_RESTART    = 2;
const int PROCESS_STARTCLEAR = 3;
//----------------------------------------------------------------------------------------------------------------------
extern pid_t  GetProcessId( const UFC::AnsiString& ProcessName );
extern void Stop( const char* ProcessName, bool Force = false );
extern void Start( const char* ProcessName,const char* BinaryName, const char* Param, BOOL Clean );
//----------------------------------------------------------------------------------------------------------------------
class AgentCommandListener : public MessageListener
{
public:
      virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );
};
//----------------------------------------------------------------------------------------------------------------------
class CheckSystemListener : public MessageListener
{
public:
      virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );
};
//----------------------------------------------------------------------------------------------------------------------
class StdioPublisher : public UFC::PThread
{
private:
     BOOL FIsOpen;
     UFC::AnsiString FKey;
public:
      StdioPublisher( );
      void Execute( void );
      void Open( const UFC::AnsiString& Key  ) { FKey = Key; FIsOpen = TRUE; }
      void Close( void )                       { FIsOpen = FALSE;}
};
//----------------------------------------------------------------------------------------------------------------------
class CMDListener : public MessageListener,UFC::PThread
{
private:
      UFC::PCriticalSection FCMDCS;
      UFC::PStringList FCMDs;
      StdioPublisher* FStdioPublisher;
public:
      CMDListener();
      void Execute( void );
      virtual void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree* Data );
      void Begin( const UFC::AnsiString& Key ) { FStdioPublisher->Open( Key ); }
      void End( void )                         { FStdioPublisher->Close( );}
};
//----------------------------------------------------------------------------------------------------------------------
class ParseLogThread : public UFC::PThread
{
public:    
    ParseLogThread( MTree* Data, const UFC::AnsiString& ReplyKey );
private:      
    UFC::AnsiString FCmpStr;
    UFC::AnsiString FReplyKey;
    UFC::AnsiString FPVCID;
    Int32 FLink;
    Int32 FPVC;
    Int32 FSYS;
    Int32 FAP;
    BOOL  FEnoughInfo;
    UFC::AnsiString FLogFile;
    int FTIME_STR_LEN;
    int FLINE_PREFIX_LEN;    
private:
    void Execute( void );
};
//----------------------------------------------------------------------------------------------------------------------
class KillThread : public UFC::PThread
{
public:
    KillThread( const UFC::AnsiString& Path, const UFC::AnsiString& Name, int MS );
private:
    UFC::AnsiString FPath;
    UFC::AnsiString FName;
    Int32 FMS;
private:
    void Execute( void );
};
//----------------------------------------------------------------------------------------------------------------------
class ProcessInfo
{
public:
    ProcessInfo()
    :IsRunning( FALSE )
    ,IsStartByAgent( FALSE )
    ,IsStopByAgent( FALSE )
    ,NotifyType(0) 
    ,EnableWatchDog( FALSE )
    { Param[ 0 ] = 0; }
    char   ProcessName[ MAX_PROCESS_NAME ];
    char   BinaryName[ MAX_PROCESS_NAME ];
    char   Param[ MAX_PROCESS_NAME ];
    char   Directory[ PATH_MAX ];
    BOOL   IsAuto;
    Int32  StartSec;
    Int32  StopSec;
    Int32  IsRunning;
    BOOL   IsRemote;
    Int32  SleepSec;
    BOOL   IsStartByAgent;
    BOOL   IsStopByAgent;
    Int32  NotifyType;
    UFC::AnsiString LineNotifyScript;
    BOOL   EnableWatchDog;
    Int32  DogWatchSec;    
    Int32  DogStartTimes;
    Int32  LastRestartSec;
    Int32  RestartTimes;
};
//----------------------------------------------------------------------------------------------------------------------
class ProcessMonitor : public MonitorListener
{
public:
    virtual void OnProcessStartup( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}
    virtual void OnProcessStopped( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}
    virtual void OnProcessConnected( BOOL IsTheFirstOne );
    virtual void OnProcessList( UFC::PStringList& Processs );
    virtual void OnConnected( void ){}
    virtual void OnDisconnected( void ){}
};
//----------------------------------------------------------------------------------------------------------------------
extern MessageObject*   MessageObj;
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------


