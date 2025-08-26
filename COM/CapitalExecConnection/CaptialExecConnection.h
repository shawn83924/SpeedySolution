// 
// File:   CaptialExecConnection.h
// Author: yuan
//
// Created on November 2, 2008, 11:08 PM
//
//------------------------------------------------------------------------------
#ifndef _CaptialExecConnection_H
#define	_CaptialExecConnection_H
//------------------------------------------------------------------------------
#include "TTaifexConnection.h"
#include "MultiplexingServer.h"
#include "Sigo.h"
//------------------------------------------------------------------------------
#include "../../TAIFEXLib/MultilegSymbol.h"
//------------------------------------------------------------------------------
#define CONFIRM_SIZE      190///< Old 175
#define NOTICE_SIZE       80
//------------------------------------------------------------------------------
#define MAXSRC_SIZE       127
#define FILL_SIZE         180
//------------------------------------------------------------------------------
typedef enum
{
    esAll     = 0,
    esNormal  = 1,
    esOffHour = 2
    
} ExecSessionType;
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
class ServerSocketMonitor : public UFC::SocketServerListener
{
private:
    //UFC::PtrList<UFC::PClientSocket>& ClientSocketList;
public:
    ServerSocketMonitor(/*UFC::PtrList<UFC::PClientSocket>& list):ClientSocketList(list*/){}
    virtual void OnListen( UFC::MultiplexingServer * ServSocket ) { UFC::BufferedLog::DebugPrintf( " Listen port:[%d]", ServSocket->GetPort() ); }
    virtual void OnClientConnect( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket );
    virtual void OnClientDisconnect( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket );
    virtual BOOL OnClientWrite( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket );
    virtual void OnServerIdle( UFC::MultiplexingServer * ServSocket ) {};
    virtual BOOL OnHeartbeat( UFC::PClientSocket* pSocket, BOOL IsRead  ) {return TRUE;};
    virtual void OnAcceptFailed( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket );
};
//------------------------------------------------------------------------------
class CaptialExecConnection : public IOrderConnectionEventListener, public UFC::PThread
{
private:    
    UFC::AnsiString    FDebug;
    //UFC::AnsiString    FAppName;
    UFC::AnsiString    FIP;
    int                FPort;
    UFC::AnsiString    FID;
    UFC::AnsiString    FPassword;   
    UFC::AnsiString    FAccount;   
    UFC::AnsiString    FBrokerID;
    //UFC::AnsiString    FExcludeBrokerID;
    UFC::PHashedSet<UFC::AnsiString> FExcludeBrokerIDSet;
    Int32              FRecoverTime;
    BOOL               FFistTimeRecover;
    BOOL               FSendQuoteExec;
public:
    static UFC::AnsiString ApKind;
private:    
    //UFC::AnsiString    FWEBID;
private:
    //int                FRecoverBeginTime;
    TResourceList      FResource;
    TTaifexConnection* FConnection; 
    BOOL               FIsLogon;
public: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count );
public: ///< Implement PThread
    void ConfirmToBackend( TExecutionReportMessage* Msg, BOOL isQuoteAccept );
    void FillToBackend( TExecutionReportMessage* Msg, BOOL ISNEW );
    void LoadSetting( void );
    virtual void Execute( void );    
public:
    CaptialExecConnection( int RecoverBeginTime );
    virtual ~CaptialExecConnection( void );
    void Recover( const UFC::AnsiString& BeginTime );
    
    Int32 StringToInt32( const UFC::AnsiString& StringValue, Int32 DefaultValue );
    void getSymbolInfo( const UFC::AnsiString& Symbol, nsOrderMessageDefine::SideEnum Side ,UFC::AnsiString& Symbol1,UFC::AnsiString& Symbol2 );
    BOOL IsConnected( void );
};
//------------------------------------------------------------------------------
#endif
