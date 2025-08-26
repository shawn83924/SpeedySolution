//----------------------------------------------------------------------------------------------------------------------
#ifndef Speedy_H
#define Speedy_H
//----------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../Migo/Sigo.h"
#include "../UFC/iniFile.h"
#include "TWSESpeedyObjectsManager.h"
//----------------------------------------------------------------------------------------------------------------------
#define	AGENT_ACK            9000
#define	AGENT_REPLY          9001
#define	AGENT_REQUEST_STATE  9004
#define	AGENT_REPLY_STATE    9005
#define	AGENT_LISTEN_PVC     9006
#define	AGENT_PVC_DAT        9007
#define	AGENT_PERFORMANCE    9010
#define	AGENT_PVC_ENABLED    9022
#define	AGENT_CHANGE_APCODE  9023
#define	AGENT_SET_SEQUENCE   9024
#define	AGENT_REJECT_ALL_ORD 9025
#define	AGENT_SET_SESSION    9026
#define	AGENT_SESSION_STATUS 9027
//-----------------------------------------------------------------------------------------------------------------------
extern UFC::AnsiString COMMAND_SUBJECT;
extern UFC::AnsiString FTP_SUBJECT;
//-----------------------------------------------------------------------------------------------------------------------
extern MessageObject * MessageObj;
extern BOOL IsDependancyAlive;
//----------------------------------------------------------------------------------------------------------------------
extern UFC::AnsiString SYS_STRING;
extern UFC::AnsiString LINK_STRING;
extern UFC::AnsiString COMMAND_STRING;
extern UFC::AnsiString ENABLE_STRING;
extern UFC::AnsiString TRADE_SYS_STRING;
extern UFC::AnsiString PVC_STATE_STRING;
extern UFC::AnsiString FILE_CODE_STRING;
extern UFC::AnsiString START_SEQ_STRING;
extern UFC::AnsiString END_SEQ_STRING;
extern UFC::AnsiString STATUS_CODE_STRING;
extern UFC::AnsiString COUNT_STRING;
extern UFC::AnsiString MSG_SEC_STRING;
extern UFC::AnsiString IO_DATA_STRING;
extern UFC::AnsiString CONFIRM_STRING;
extern UFC::AnsiString ORDER_STRING;
extern UFC::AnsiString PROCESS_PVC_STRING;
extern UFC::AnsiString NID_STRING;
//----------------------------------------------------------------------------------------------------------------------
extern UFC::AnsiString ClusterConfigFile; 
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  svStandard     = 0,
  svProfessional = 1,
  svPlatinum     = 2

}SpeedyTWSEVersion;
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  mtTSE = 0,
  mtOTC = 1,
  mtES  = 2

}MarketType;
//----------------------------------------------------------------------------------------------------------------------
class CommandListener : public MessageListener
{
  public:
      virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data );
};
//----------------------------------------------------------------------------------------------------------------------
class CheckSystemListener : public MessageListener
{
public:
      virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );
};
//-----------------------------------------------------------------------------------------
namespace TWSE
{
class SpeedyObjectsManager;
//----------------------------------------------------------------------------------------------------------------------
class SyncListener: public MessageListener
{
private:    
    SpeedyObjectsManager* FObjectsManager;
public:
    SyncListener( SpeedyObjectsManager* ObjManager );      
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data );
};    
//----------------------------------------------------------------------------------------------------------------------
class OtherExecutionListener : public MessageListener
{
private:
    MarketType FMarket;
    UFC::AnsiString FConfirmSubject;
    UFC::AnsiString FFillSubject;
public:
    OtherExecutionListener( MarketType mkt );
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );
};
//----------------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
class ProcessMonitor : public MonitorListener
{
private:    
    TWSE::SpeedyObjectsManager* FObjectsManager;
    BOOL                        FIsTWSE;    
public:
    ProcessMonitor( TWSE::SpeedyObjectsManager* ObjectsManager, BOOL IsTWSE )
    :FObjectsManager(ObjectsManager) 
    ,FIsTWSE( IsTWSE )  { }
public:
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
