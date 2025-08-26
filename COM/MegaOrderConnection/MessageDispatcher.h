// 
// File:   MessageDispatcher.h
// Author: yuan
//
// Created on January 22, 2010, 6:42 PM
//

#ifndef _MESSAGEDISPATCHER_H
#define	_MESSAGEDISPATCHER_H
//------------------------------------------------------------------------------
#include "UFC.h"
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
#include "SpeedyQueueOrderConnection.h" 
//------------------------------------------------------------------------------
class MessageDispatcher : public IOrderConnectionEventListener
{
private:
    ///< SpeedyGateway info.
    UFC::AnsiString       FIP;
    int                 FPort;
    UFC::AnsiString       FID;
    UFC::AnsiString       FPassword;   
    UFC::AnsiString       FAccount;
    UFC::AnsiString       FBrokerID;
    UFC::AnsiString       FTFXBrokerID;
    UFC::AnsiString       FTFXCMID;
    TTaifexConnection*    FConnection;    
    SpeedyQueueOrderConnection* FPipe;
private: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Accepted, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count );
private:
    void LoadSetting( void );    
public:
    void CheckConnection( void );
    void Recover( const UFC::AnsiString& BeginTime );        
    MessageDispatcher();
};
//------------------------------------------------------------------------------
#endif	/* _MESSAGEDISPATCHER_H */

