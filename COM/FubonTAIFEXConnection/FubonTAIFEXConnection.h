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

} TFubonTAIFEXConnectionStateEnum;
//--------------------------------------------------------------------------
class TFubonTAIFEXConnection : public IOrderConnectionEventListener, public UFC::PThread
///<, public MessageListener
{
private:    
    UFC::AnsiString    FTFXCMID;
    UFC::AnsiString    FDebug;
    UFC::AnsiString    FLogFile;   
    UFC::AnsiString    FAppName;
    UFC::AnsiString    FIP;
    int                FPort;
    UFC::AnsiString    FID;
    UFC::AnsiString    FPassword;    
    int                FTradeTimeout;    
    UFC::PtrQueue<UFC::AnsiString> FIPPool;
    UFC::SInt          FRecoverCount;
    
    UFC::AnsiString    FOrderQueName;
    
    UFC::AnsiString    FConfirmQueName;
    UFC::AnsiString    FFilledQueName; 
    
    UFC::AnsiString    FOHConfirmQueName;
    UFC::AnsiString    FOHFilledQueName; 
    UFC::AnsiString    FLockFile;
    BOOL               FCheckTradeDate;
private:
    UFC::AnsiString       FToday;
    TResourceList         FResource;
    TTaifexConnection*    FConnection;    
    TSysque*              FOrderQueue;    
    TSysque*              FConfirmQueue;
    TSysque*              FFilledQueue;    
    TSysque*              FOHConfirmQueue;
    TSysque*              FOHFilledQueue;
    char*                 FReadBuffer;    
    UFC::PCriticalSection FConnectionStateCS;
    UFC::PCriticalSection FAPIIOCS;
public:
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );    
    virtual void OnRecoverFinished( int Count );

public:
    virtual void Execute( void );    
public:
    TFubonTAIFEXConnection( const UFC::AnsiString& ConfigFileName );
    virtual ~TFubonTAIFEXConnection( void );
    void Recover( UFC::AnsiString BeginTime );
private:    
    void OnData( void );
    void Dequeue( void );
    void GetLocalLineNumber( void );
    void BuildSentLineNumberTable( void );
    void SetLogObject( void );
    void ReadConfigFile( const UFC::AnsiString& ConfigFileName );
    void LockFile( void );
    void TFXOrderStatus( TExecutionReportMessage* Msg, TSQMessage& SQMessage );
    void OnTFXConfirm( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& BrokerID, TExecutionReportMessage* Msg, TSQMessage& SQMessage );    
    void OnTFXFilled( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& BrokerID, TExecutionReportMessage* Msg, TSQMessage& SQMessage );
    void QuoteRequestConfirm( TExecutionReportMessage* Msg,  TSQMessage& SQMessage );
    nsOrderMessageDefine::MarketEnum           GetMarket( TSQMessage& SQMessage );
    nsOrderMessageDefine::TradingSessionIDEnum GetTradeSession( TSQMessage& SQMessage );
private:            
    void TFXOrderStatusRequest( TSQMessage& SQMessage, TOrderStatusRequest& Msg  );    
    void OrderStatusRequest( TSQMessage& SQMessage );    
    ///< Functions for Quote Request
    void QuoteRequest( TSQMessage& SQMessage );
    ////< Function for new order/Quote
    void TFXNew( TSQMessage& SQMessage, TNewOrderMessage& Msg );
    void NewQuote( TSQMessage& SQMessage );    
    void New( TSQMessage& SQMessage );
    ///< Functions for Replace Qty/Px    
    void TFXReplace( TSQMessage& SQMessage, TReplaceOrderMessage& Msg, BOOL IsReplacePx );
    void Replace( TSQMessage& SQMessage, BOOL IsReplacePx );
    ///< Functions for Cancel Order/Quote
    void TFXCancel( TSQMessage& SQMessage, TCancelOrderMessage& Msg );
    void QuoteCancel( TSQMessage& SQMessage );    
    void Cancel( TSQMessage& SQMessage );       
private:
    TFubonTAIFEXConnectionStateEnum FConnectionState;
    void SetConnectionState( TFubonTAIFEXConnectionStateEnum ConnectionState );
    void CheckConnectionState( void );
    BOOL CheckTradeTime( TSQMessage& SQMessage );
    BOOL CheckTradeDate( TSQMessage& SQMessage );
    void RejectOvertimeOrder( TSQMessage& OvertimeOrder );
public:
    TFubonTAIFEXConnectionStateEnum GetConnectionState( void );    
};
//------------------------------------------------------------------------------
#endif	/* _FubonOrderConnectionEventListener_H */
