// 
// File:   SpeedyOrderConnection.h
// Author: yuan
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _SpeedyOrderConnection_H
#define	_SpeedyOrderConnection_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
//#include "../OrderConnection/Resource.h"
//--------------------------------------------------------------------------
class SpeedyOrderConnection : public IOrderConnectionEventListener, public UFC::PThread
{
private:    
    UFC::AnsiString    FDebug;
    UFC::AnsiString    FAppName;
    UFC::AnsiString    FIP;
    int                FPort;
    UFC::AnsiString    FID;
    UFC::AnsiString    FPassword;   
    UFC::AnsiString    FAccount;   
    UFC::AnsiString    FTFXBrokerID;       
    UFC::AnsiString    FTWSEBrokerID;       
private:
    int                   FRecoverBeginTime;    
    TResourceList         FResource;
    TTaifexConnection*    FConnection; 
    BOOL                  FIsLogon;        
    void                  LoadSetting( void );
public: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count );
    virtual void OnChangePassword( ChangePwdResult Result );
public: ///< Implement PThread
    virtual void Execute( void );    
public:
    SpeedyOrderConnection( int RecoverBeginTime );
    virtual ~SpeedyOrderConnection( void );   
    bool IsConnect( void );
    void Connect( void );
    void New( int Qty );
    void NewFutures( int Qty );
    void NewTWSE( int Qty );
    void CancelTWSE( int Qty );
    void ChangePassword( void );
};
//------------------------------------------------------------------------------
#endif
