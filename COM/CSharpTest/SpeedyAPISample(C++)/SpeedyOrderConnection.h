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
#include "../OrderConnection/Resource.h"
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
    UFC::AnsiString    FBrokerID;       
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
    virtual void Execute( void );    
public:
    SpeedyOrderConnection( int RecoverBeginTime );
    virtual ~SpeedyOrderConnection( void );   
    void New( int Qty );
};
//------------------------------------------------------------------------------
#endif
