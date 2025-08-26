// 
// File:   ChinaTrustExecConnection.h
// Author: yuan
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _ChinaTrustExecConnection_H
#define	_ChinaTrustExecConnection_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
//--------------------------------------------------------------------------
class ChinaTrustExecConnection : public IOrderConnectionEventListener, public UFC::PThread
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
    UFC::AnsiString    FBranch;
private:
    int                   FRecoverBeginTime;    
    TResourceList         FResource;
    TTaifexConnection*    FConnection; 
    BOOL                  FIsLogon;
    long                  FFUTSecondFillQueueID;
    long                  FOPTSecondFillQueueID;
    UFC::PQueue           FFUTConfirmQueue;
    UFC::PQueue           FOPTConfirmQueue;
    UFC::PQueue           FFUTFillQueue;
    UFC::PQueue           FOPTFillQueue;
    UFC::PQueue           FSecondFUTFillQueue;
    UFC::PQueue           FSecondOPTFillQueue;
public: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count );
public: ///< Implement PThread
    void ConfirmToBackend( TExecutionReportMessage* Msg );
    void FillToBackend( TExecutionReportMessage* Msg, bool WithProtfolio, bool IsFilled = false );
    void LoadSetting( void );
    virtual void Execute( void );    
public:
    ChinaTrustExecConnection( int RecoverBeginTime );
    virtual ~ChinaTrustExecConnection( void );   
    void Recover( UFC::AnsiString BeginTime, UFC::AnsiString EndTime, char flag );
};
//------------------------------------------------------------------------------
#endif
