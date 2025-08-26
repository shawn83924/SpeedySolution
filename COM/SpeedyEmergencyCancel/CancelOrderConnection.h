/* 
 * File:   CancelOrderConnection.h
 * Author: yuan
 *
 * Created on May 26, 2009, 8:15 PM
 */

#ifndef _CANCELORDERCONNECTION_H
#define	_CANCELORDERCONNECTION_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
#include "../Lib/OrderMessage/OrderMessageDefine.h"
#include "../Lib/OrderMessage/TCancelOrderMessage.h"
#include "../Lib/OrderMessage/TQuoteCancelMessage.h"
//--------------------------------------------------------------------------
class CancelOrderConnection : public IOrderConnectionEventListener, public UFC::PThread
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
    int                FRecoverBeginTime;
    TResourceList      FResource;
    TTaifexConnection* FConnection;
    BOOL               FIsLogon;
public: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup  );
    virtual void OnRecoverFinished( int Count );
public: ///< Implement PThread
    virtual void Execute( void );
public:
    CancelOrderConnection( int RecoverBeginTime );
    virtual ~CancelOrderConnection( void );
    void Cancel(TCancelOrderMessage& CancelMsg);
    void QuoteCancel(TQuoteCancelMessage& CancelMsg);
    BOOL IsLogon();
};
//------------------------------------------------------------------------------
#endif	/* _CANCELORDERCONNECTION_H */

