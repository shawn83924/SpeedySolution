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
#include "WorkingOrder.h"
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
    UFC::PCriticalSection FOrderBookCS;
private:
    int                   FRecoverBeginTime;    
    TResourceList         FResource;
    TTaifexConnection*    FConnection; 
    BOOL                  FIsLogon;   
    int                   FThroughput;
    int                   FOutCount;
    int                   FFirstOrderTick;
    UFC::PHashMap< UFC::AnsiString,WorkingOrder*> FFutWorking;
    UFC::PHashMap< UFC::AnsiString,WorkingOrder*> FOptWorking;
    UFC::PHashedSet<UFC::AnsiString> FExecID;
private:    
    void OnNewOrder( TExecutionReportMessage* Msg );
    void OnCancelOrder( TExecutionReportMessage* Msg );
    void OnReduceQty( TExecutionReportMessage* Msg );
    void ControlTroughput( void );
public: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count );
    virtual void OnCancelWorking( CancelWorkingResult Result, int Count ){}
public: ///< Implement PThread
    virtual void Execute( void );    
    void PrintWorking( void );
    void LoadSetting( void );
    void AddToList( UFC::PHashMap< UFC::AnsiString,WorkingOrder*>& Working,
                    UFC::List<TCancelOrderMessage*>& CxlList,
                    const UFC::AnsiString& ChkAcc,
                    const UFC::AnsiString& ChkAE,
                    const UFC::AnsiString& ChkData, 
                    const UFC::AnsiString& ChkPID );
public:
    SpeedyOrderConnection( int RecoverBeginTime );
    virtual ~SpeedyOrderConnection( void );   
    void CancelList( UFC::List<TCancelOrderMessage*>& CxlList,
                     CancelMarket Market = cmAll,
                     const UFC::AnsiString& ChkAcc  = "",
                     const UFC::AnsiString& ChkAE   = "",
                     const UFC::AnsiString& ChkData = "", 
                     const UFC::AnsiString& ChkPID  = "" );
    void Cancel( UFC::List<TCancelOrderMessage*>& CxlList );
};
//------------------------------------------------------------------------------
#endif
