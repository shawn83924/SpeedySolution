// 
// File:   YuantaExecConnection.h
// Author: yuan
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _SinoPacExecConnection_H
#define	_SinoPacExecConnection_H
//------------------------------------------------------------------------------
#include "Sigo.h" 
#include "UFC.h" 
#include "iniFile.h"
#include "tibrv/tibrvcpp.h"
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
//--------------------------------------------------------------------------
class AccountInfo
{
private:
    UFC::AnsiString FBrokerID;
    UFC::AnsiString FAccount;
    UFC::AnsiString FSubAccount;
    double FCanUse;
    UFC::PCriticalSection FCS;
public:
    AccountInfo( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Account, const UFC::AnsiString& SubAccount );
    void   SetCanUse( double CanUse );
    BOOL   CanPlaceOrder( double SubAmt, double Limit );
    double GetCanUse();
    void   CancelOrder( double AddAmt );
};
//--------------------------------------------------------------------------
class SinoPacRMConnection :  public IOrderConnectionEventListener, public MessageListener, public TibrvMsgCallback, public UFC::PThread
{
private:
    BOOL                  FWaitForR6;
    BOOL                  FSpeedyRM;
    BOOL                  FToR6Checker;
    MessageObject*        FMessageObject;
    UFC::AnsiString       FIP;
    int                   FPort;
    UFC::UiniFile*        CommodityDepositTable;
    UFC::PCriticalSection FCommodCS;
    UFC::UiniFile*        AccountLimitTable;
    UFC::PCriticalSection FAccountLimitCS;
    UFC::PHashedList<UFC::AnsiString, AccountInfo*> AccountInfoTable;
    UFC::PHashedList<UFC::AnsiString, UFC::AnsiString*> AccountRiskLevelMap;
    UFC::PHashedSet<UFC::AnsiString> FExecIDTable;
    UFC::FileStreamEx*    FExecIDFile;
    ///< Speedy API
    UFC::AnsiString       FGatewayIP;
    int                   FGatewayPort;
    UFC::AnsiString       FID;
    UFC::AnsiString       FPassword;   
    UFC::AnsiString       FAccount;   
    UFC::AnsiString       FBrokerID;
private:
    int                   FRecoverBeginTime;    
    TTaifexConnection*    FConnection; 
    BOOL                  FIsLogon;       
    ///< for Tibco RV
private:    
    UFC::AnsiString       FService;           
    UFC::AnsiString       FNetwork;           
    UFC::AnsiString       FDaemon;
    UFC::AnsiString       FR6Subject; 
    UFC::AnsiString       FSpeedySubject;
    UFC::AnsiString       FClassName;
    UFC::AnsiString       FRMSubject;
private:    
    UFC::AnsiString       FPrefixDef;
    UFC::AnsiString       FTrackingID;
    UFC::AnsiString       FTracking1;  
private:        
    TibrvNetTransport     FRVTransport;     
    TibrvListener         FOrderlistener;
    TibrvListener         FRMInfoListener;
    TibrvMsg              FMsg;
    TibrvMsg              FDataMsg;
    TibrvMsg              FPrefixMsg;
    TibrvMsg              FTrackingMsg;
private:    
    void InitRV( void );
    void CreateRVMsg( void );
    void LoadSetting( void );    
    void OnCancelReplace( TExecutionReportMessage* Msg );
    void RecoverExecID( void );
    bool IsReplacePx( const UFC::AnsiString& Order );
    int CheckSubAccount( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Account, UFC::AnsiString& SubAccount );
    UFC::AnsiString  GetSubAccount( const UFC::AnsiString& Key );
    void RecoverLevelFile( void );
    void SaveLevelFile( void );
private: 
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );    
private:
    void Execute( void );
private: ///< Implement TibrvMsgCallback    
    void onMsg( TibrvListener* listener, TibrvMsg& msg );
private: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count );
public:
    SinoPacRMConnection( int RecoverBeginTime );
    virtual ~SinoPacRMConnection( void );
    double GetCommodOTAMT( const UFC::AnsiString& SymbolPrefix );
    void SetCommodOTAMT( const UFC::AnsiString& SymbolPrefix, double Price );
    void DeleteCommodOTAMT( const UFC::AnsiString& SymbolPrefix );
    BOOL GetAccountLimit( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Account, const UFC::AnsiString& SubAccount, double& Price );
    void SetAccountLimit( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Account, const UFC::AnsiString& SubAccount, double Limit );
    void DeleteAccountLimit( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Account, const UFC::AnsiString& SubAccount );
    AccountInfo* GetAccountInfo( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Account, const UFC::AnsiString& SubAccount );
    void CheckConnection( void );
    void Recover( const UFC::AnsiString& BeginTime );    
};
//------------------------------------------------------------------------------
#endif
