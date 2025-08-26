//----------------------------------------------------------------------------------------------------------------------
#ifndef Speedy_H
#define Speedy_H
//----------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../Migo/Sigo.h"
#include "../UFC/iniFile.h"
//----------------------------------------------------------------------------------------------------------------------
#define AGENT_ACK             9000
#define AGENT_REPLY           9001
#define AGENT_REQUEST_STATE   9004
#define AGENT_REPLY_STATE     9005
#define AGENT_LISTEN_PVC      9006
#define AGENT_PVC_DAT         9007
#define AGENT_PERFORMANCE     9010
#define AGENT_PVC_ENABLED     9022
//-----------------------------------------------------------------------------------------------------------------------
extern UFC::AnsiString COMMAND_SUBJECT;
//-----------------------------------------------------------------------------------------------------------------------
extern MessageObject * MessageObj;
extern BOOL RecoverMode;
extern Int32 IOCExpired;   
extern Int32 QueueMax;
//----------------------------------------------------------------------------------------------------------------------
const UFC::AnsiString ClusterConfigFile = "../cfg/SpeedyCluster.cfg"; 
//----------------------------------------------------------------------------------------------------------------------
class SpeedyObjectsManager;
//----------------------------------------------------------------------------------------------------------------------
class CommandListener : public MessageListener
{
  public:
      virtual void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* Data );
};
//----------------------------------------------------------------------------------------------------------------------
class FtpListener : public MessageListener
{
  public:
      virtual void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* Data );
};
//----------------------------------------------------------------------------------------------------------------------
class JavaFTPListener : public MessageListener
{
  public:
      virtual void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* Data );
};
//----------------------------------------------------------------------------------------------------------------------
class TMPTSSListener : public MessageListener
{
  private:
      BOOL IsOpt;
  public:
      TMPTSSListener( BOOL IsOptions ):IsOpt(IsOptions){}
      virtual void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* Data );
};
//----------------------------------------------------------------------------------------------------------------------
class SyncListener: public MessageListener
{
  private:
      SpeedyObjectsManager* FObjectsManager;
  public:
      SyncListener( SpeedyObjectsManager* ObjManager );
      virtual void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* Data );
};
//----------------------------------------------------------------------------------------------------------------------
class ProcessMonitor : public MonitorListener
{
private:
    BOOL                  FIsOptions;
    SpeedyObjectsManager* FObjectsManager;    
public:
    ProcessMonitor( BOOL IsOpt, SpeedyObjectsManager* FObjectsManager );
    virtual void OnProcessStartup( const UFC::AnsiString& Host, const UFC::AnsiString& AppName );
    virtual void OnProcessStopped( const UFC::AnsiString& Host, const UFC::AnsiString& AppName );
    virtual void OnProcessConnected( BOOL IsTheFirstOne );
    virtual void OnProcessList( UFC::PStringList& Processs ){}
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
};
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------

