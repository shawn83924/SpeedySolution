// 
// File:   MLExecConnection.h
// Author: yuan
//
// Created on November 2, 2008, 11:08 PM
//
//------------------------------------------------------------------------------
#ifndef _MLExecConnection_H
#define	_MLExecConnection_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
#include "../../UFC/MultiplexingServer.h"
#include "../../Migo/Sigo.h"
//------------------------------------------------------------------------------
#include "../../TAIFEXLib/MultilegSymbol.h"
#include "PClientSocket.h"
#include "MLMessage.h"
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
class SocketMonitor : public UFC::SocketClientListener
{
public:
    virtual void OnConnect( UFC::PClientSocket * Socket );
    virtual void OnDisconnect( UFC::PClientSocket * Socket, BOOL NeedReconnect = FALSE );
    virtual BOOL OnDataArrived( UFC::PClientSocket * Socket );
    virtual void OnIdle( UFC::PClientSocket * Socket ) {}
};
//------------------------------------------------------------------------------
typedef enum
{
    emTSE    = 0,
    emOTC    = 1,
    emPATS   = 2,
    emTAIFEX = 3,
    emAll    = 4

} ExecutionMarket;
//------------------------------------------------------------------------------
typedef enum
{
    etTAIFEXConfirm,
    etTAIFEXFilled,
    etOverseaConfirm,
    etOverseaFilled
            
} ExecutionType;
//------------------------------------------------------------------------------
class MLExecConnection : public IOrderConnectionEventListener, public UFC::PThread
{
private:
    UInt32              sn;
    UFC::PClientSocket* client1;
    SocketMonitor*      Client1Monitor;    
    UFC::AnsiString     FDebug;
    UFC::AnsiString     FIP;
    int                 FPort;
    UFC::AnsiString     FID;
    UFC::AnsiString     FPassword;
    UFC::AnsiString     FAccount;
    UFC::AnsiString     FBrokerID;
    int                 FSocketTimeout;
    UFC::BufferedLog*   FNormalLog;
    UFC::BufferedLog*   FOffHourLog;
    BOOL                FExitOnDisconnect;
public:
    static MessageObject*     FMessageObject;
private:
    TResourceList      FResource;
    TTaifexConnection* FConnection; 
    BOOL               FIsLogon;
    BOOL               FFirstRecover;
    Int32              FRecoverTime;
    Int32              FFilterSID;
    Int32              FFilterSeq;
    Int32              FFilterEndSeq;
    UFC::PHashedList<UFC::AnsiString, UFC::AnsiString*> FSymbolTable; ///< OrderID->Symbol
    UFC::FileStreamEx* FSymbolFile;
public: ///< Implement IOrderConnectionEventListener
    virtual ~MLExecConnection( void );
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString,  LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count );
public: ///< Implement PThread
    virtual void Execute( void );
private:
    void InitialReport( okdata* data );
    void RejectToBackend( TExecutionReportMessage* Msg );
    bool IsProxyUserData( const UFC::AnsiString& OrigUserData,
                          UFC::AnsiString& AE, Int32& NID, UFC::AnsiString& Data);
private:///< Overseas    
    UFC::AnsiString CheckOverseasTimeStrFormat(const UFC::AnsiString& OverseasTimeStr);
    UFC::AnsiString ConvertOverseasMaturityMonthYear(const UFC::AnsiString& MonthYearStr);
public:    
    void ToBackend( TExecutionReportMessage* Msg ,Int8 type);
    void TSEConfirmToBackend( TExecutionReportMessage* Msg );    
    void TSEFilledToBackend( TExecutionReportMessage* Msg );        
    void OverseasConfirmToBackend( TExecutionReportMessage* Msg );        
    void OverseasFillToBackend( TExecutionReportMessage* Msg );        
    bool CheckReportMarket(TExecutionReportMessage *Msg);    
public:
    UFC::AnsiString DisconnectTime1;
    BOOL            clientIsConnect;
    Int32           HeartbeatTime1;
    Int32           HeartbeatTimeCount1;    
public:
    MLExecConnection( void );
    void LoadSetting( void );
    void Heartbeat(UFC::PClientSocket* client);
    void ConnectToBeckend( void );
    void Update( void );
    void Recover( const UFC::AnsiString& BeginTime, Int32 SID, Int32 Seq, Int32 EndSeq );
    void LoadSymbolTable( const UFC::AnsiString& FileName );
    void AddToSymbolTable( const UFC::AnsiString& OrderID, const UFC::AnsiString& Symbol );
    void Send(const char* okd, ExecutionType et );
};
//------------------------------------------------------------------------------
#endif
