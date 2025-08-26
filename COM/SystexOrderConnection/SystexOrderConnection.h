// 
// File:   FubonOrderConnection.h
// Author: yuan
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _FubonOrderConnection_H
#define	_FubonOrderConnection_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "Sysque.h"
//------------------------------------------------------------------------------
typedef enum
{
	focDisconnect = 0,
	focConnecting = 1,
	focConnected  = 2,
        focReady      = 3

} TFubonOrderConnectionStateEnum;
//--------------------------------------------------------------------------
class TSystexOrderConnection : public IOrderConnectionEventListener, public UFC::PThread, public MessageListener
{
private:    
    UFC::AnsiString    FTFXCMID;
    UFC::AnsiString    FDebug;
    UFC::AnsiString    FLogFile;   
    UFC::AnsiString    FAppName;
    UFC::AnsiString    FIP;
    int                FPort;
    UFC::AnsiString    FPassword;    
    int                FTradeTimeout;    
    UFC::PtrQueue<UFC::AnsiString> FIPPool;
    UFC::SInt          FRecoverCount;
private:
    UFC::AnsiString       FToday;
    //TResourceList         FResource;
    TTaifexConnection*    FConnection;    
    int                   FRecoverBeginTime;
    UFC::PCriticalSection FConnectionStateCS;
    UFC::PCriticalSection FAPIIOCS;
public:
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, bool IsAccepted, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, bool PossDup );
    virtual void OnRecoverFinished( int Count );

public:
    virtual void Execute( void );
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );
public:
    TSystexOrderConnection( const UFC::AnsiString& ConfigFileName, int RecoverBeginTime );
    virtual ~TSystexOrderConnection( void );
    void Recover( UFC::AnsiString BeginTime );
    TFubonOrderConnectionStateEnum GetConnectionState( void );
private:
    void GetLocalLineNumber( void );
    void BuildSentLineNumberTable( void );
    void SetLogObject( void );
    void ReadConfigFile( const UFC::AnsiString& ConfigFileName );
    void LockFile( void );
    void TFXOrderStatus( TExecutionReportMessage* Msg, TSQMessage& SQMessage );
    void TSEOrderStatus( TExecutionReportMessage* Msg, TSQMessage& SQMessage );
    void OnTSEConfirm( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& BrokerID, TExecutionReportMessage* Msg, TSQMessage& SQMessage );
    void OnTFXConfirm( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& BrokerID, TExecutionReportMessage* Msg, TSQMessage& SQMessage );    
    void OnTSEFilled( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& BrokerID, TExecutionReportMessage* Msg, TSQMessage& SQMessage );    
    void OnTFXFilled( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& BrokerID, TExecutionReportMessage* Msg, TSQMessage& SQMessage );
    void OnNegotiatePxConfirm( TExecutionReportMessage* Msg );       
    void NegotiatePx( TSQMessage& SQMessage );
    
    void TFXOrderStatusRequest( TSQMessage& SQMessage, TOrderStatusRequest& Msg  );    
    void TSEOrderStatusRequest( TSQMessage& SQMessage, TOrderStatusRequest& Msg  );    
    void OrderStatusRequest( TSQMessage& SQMessage );    
    
    void TFXNew( TSQMessage& SQMessage, TNewOrderMessage& Msg );
    void TSENew( TSQMessage& SQMessage, TNewOrderMessage& Msg );
    void New( TSQMessage& SQMessage );
    
    void TFXReplace( TSQMessage& SQMessage, TReplaceOrderMessage& Msg );
    void TSEReplace( TSQMessage& SQMessage, TReplaceOrderMessage& Msg );
    void Replace( TSQMessage& SQMessage );
    
    void TFXCancel( TSQMessage& SQMessage, TCancelOrderMessage& Msg );
    void TSECancel( TSQMessage& SQMessage, TCancelOrderMessage& Msg );
    void Cancel( TSQMessage& SQMessage );
    
    nsOrderMessageDefine::MarketEnum           GetMarket( TSQMessage& SQMessage );
    nsOrderMessageDefine::TradingSessionIDEnum GetTradeSession( TSQMessage& SQMessage );
private:
    TFubonOrderConnectionStateEnum FConnectionState;
    void SetConnectionState( TFubonOrderConnectionStateEnum ConnectionState );    
    void CheckConnectionState( void );
    BOOL CheckTradeTime( TSQMessage& SQMessage );
    BOOL CheckTradeDate( TSQMessage& SQMessage );
    void RejectOrder( TSQMessage& OvertimeOrder, int Status );
};
//------------------------------------------------------------------------------
#endif	/* _FubonOrderConnectionEventListener_H */
