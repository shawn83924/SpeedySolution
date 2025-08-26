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
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
//--------------------------------------------------------------------------
class AccountInfo
{
private:
    UFC::AnsiString FAE;
    UFC::AnsiString FAccount;
    double FCanUse;
    UFC::PCriticalSection FCS;
public:
    AccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE );
    BOOL CanPlaceOrder( double SubAmtt );
    void SetCanUse( double CanUse );
    double GetCanUse();
    void CancelOrder( double AddAmt );
    void FilledOrder( double AddAmt );
};
//--------------------------------------------------------------------------
class GCRMConnection :  public IOrderConnectionEventListener, public MessageListener
{
private:    
    MessageObject*        FMessageObject;
    UFC::AnsiString       FIP;
    int                   FPort;
    UFC::UiniFile*        CommodityDepositTable;
    UFC::PCriticalSection FCommodCS;
    UFC::PCriticalSection FAccountLimitCS;
    UFC::PHashedList<UFC::AnsiString, AccountInfo*> AccountInfoTable;
    UFC::PHashedSet<UFC::AnsiString>                FExecIDTable;
    UFC::FileStreamEx*    FExecIDFile;
    ///< Speedy API
    UFC::AnsiString       FGatewayIP;
    int                   FGatewayPort;
    UFC::AnsiString       FID;
    UFC::AnsiString       FPassword;   
    UFC::AnsiString       FAccount;   
    UFC::AnsiString       FBrokerID;
    UFC::AnsiString       FAS400WebID;
    char                  FAS400Src;  
    UFC::UiniFile*        FConfig;
    UFC::UiniFile*        FErrMsg;
private:
    int                   FRecoverBeginTime;    
    int                   FAS400OrderSeq;
    TTaifexConnection*    FConnection; 
    BOOL                  FIsLogon;
    BOOL                  FRejectMarketOrd;
private:
    BOOL                  FReleaseMarginOnFilled;
    BOOL                  FReleaseMarginOnRODCxl;
    BOOL                  FReleaseMarginOnFOKIOCCxl;
private:    
    UFC::AnsiString       FToSpeedySubject;
    UFC::AnsiString       FToSpeedyKey;
    UFC::AnsiString       FFutOrderSubject;
    UFC::AnsiString       FOptOrderSubject;
private:        
    UFC::AnsiString       F400CheckOrderSubject;
    UFC::AnsiString       F400CheckOrderKey;
    UFC::AnsiString       F400CheckResultSubject;
    UFC::AnsiString       F400CheckResultKey;    
private:    
    UFC::PHashMap<UFC::AnsiString, double> FFutDeposit;
    UFC::PHashMap<UFC::AnsiString, double> FOptAValue;
    UFC::PHashMap<UFC::AnsiString, double> FOptCM;
    double FDefFutDeposit;
    double FDefOptAValue;
    double FDefOptCM;
    double GetFuturesDeposit( const UFC::AnsiString& SymbolPrefix, bool IsDayTrade );
    double GetOptionsCM( const UFC::AnsiString& SymbolPrefix );
    double GetOptionsA( const UFC::AnsiString& SymbolPrefix );
    void   UseMargin( const UFC::AnsiString& Account, UFC::AnsiString& AE );
private:
    void LoadSetting( void );    
    void OnCancelReplace( TExecutionReportMessage* Msg );
    void RecoverExecID( void );
    void UpdateExecID( const UFC::AnsiString& ExecID );
    bool IsReplacePx( const UFC::AnsiString& Order );
    void GetAE( const UFC::AnsiString& Key, UFC::AnsiString& AE );
    void GetSubAccount( const UFC::AnsiString& Key, UFC::AnsiString& AE );
    bool GetAEFromData( const UFC::AnsiString& Data, UFC::AnsiString& AE );
    void ReceiveMarginInfo( MTree* Data );
    void CheckResultFromAS400( MTree* Data );
    void SendToAS400Checker( BOOL IsOpt, int NID, const UFC::AnsiString& Order, const UFC::AnsiString& Key,BOOL IsOffHour );
private: 
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );    
private: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count );
public:
    GCRMConnection( int RecoverBeginTime );
    virtual ~GCRMConnection( void );
    double       GetCommodDeposit( const UFC::AnsiString& SymbolPrefix, bool IsDayTrade );
    double       GetOptionsDeposit( const UFC::AnsiString& Symbol, double Px, bool IsBuy );
    void         SetAccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE, double CanUse );
    AccountInfo* GetAccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE );
    void         CheckConnection( void );
    void         Recover( const UFC::AnsiString& BeginTime );
};
//------------------------------------------------------------------------------
#endif
