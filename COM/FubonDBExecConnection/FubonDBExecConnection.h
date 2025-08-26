// 
// File:   YuantaExecConnection.h
// Author: yuan
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _FubonDBExecConnection_H
#define	_FubonDBExecConnection_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
#include "../../Migo/Sigo.h"
//----------------------------------------------------------------------------------------------------------------------
typedef struct body_s
{
    ///< Header: 8
    Int8  TDATE[2];
    Int8  TYPE[2]; ///< "1 ":TSE "2 ":OTC
    Int8  LINE[2];
    Int8  FNCODE[2]; ///< 10
    ///< Body:62
    Int8  STK_NO[6];
    Int8  MTHQTY[8];
    Int8  MTHPR[6];
    Int8  MTHTIME[8];
    Int8  EXCD[1];
    Int8  BUY_SELL[1];
    Int8  ODRNO[5];
    Int8  IVACNO[7];
    Int8  ODRTPE[1];
    Int8  SEQ_NO[6];
    Int8  BROKER_ID[4];
    Int8  RECNO[8];
    Int8  MARK_S[1];
    ///< Ext: 6
    Int8  SUB_AC[6];
    
} T3Body;
//----------------------------------------------------------------------------------------------------------------------
#define BSOKTOTALSIZE 96
#define MHOKTOTALSIZE 70
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
//--------------------------------------------------------------------------
class FubonDBExecConnection : public IOrderConnectionEventListener, public UFC::PThread
{
private:    
    UFC::AnsiString    FDebug;
    UFC::AnsiString    FAppName;
    UFC::AnsiString    FIP;
    int                FPort;
    UFC::AnsiString    FID;
    UFC::AnsiString    FPassword;   
    UFC::AnsiString    FAccount;   
    UFC::AnsiString    FBrokerID;       
    UFC::AnsiString    FDate;       
private:    
    UFC::FileStreamEx* FBSOKDataFile;
    UFC::FileStreamEx* FMHOKDataFile;
    UFC::AnsiString    FSLINE;
    UFC::AnsiString    FHTSQ;
    UFC::AnsiString    FINSQ;
    UFC::AnsiString    FMGSQ;
    UFC::AnsiString    FSCODE;
    UFC::AnsiString    FTLNO;
    UFC::AnsiString    FMHOKLNO;
private:
    int                   FRecoverBeginTime;
    TResourceList         FResource;
    TTaifexConnection*    FConnection; 
    BOOL                  FIsLogon;        
public: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, bool IsAccepted, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, bool PossDup );
    virtual void OnRecoverFinished( int Count );
public: ///< Implement PThread
    void ConfirmToBackend( TExecutionReportMessage* Msg );
    void FillToBackend( TExecutionReportMessage* Msg );
    void LoadSetting( void );
    virtual void Execute( void );    
public:
    FubonDBExecConnection( int RecoverBeginTime );
    virtual ~FubonDBExecConnection( void );
    void Recover( UFC::AnsiString BeginTime );    
};
//------------------------------------------------------------------------------
#endif
