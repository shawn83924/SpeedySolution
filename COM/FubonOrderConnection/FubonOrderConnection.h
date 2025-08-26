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
extern UFC::AnsiString gStockFormatPath;
extern UFC::AnsiString gStockT30SFormatFileName;
extern UFC::AnsiString gStockT30SFormatFullFileName;
extern UFC::AnsiString gStockT30OFormatFileName;
extern UFC::AnsiString gStockT30OFormatFullFileName;

extern UFC::AnsiString gStockDataFilePath;
extern UFC::AnsiString gStockT30SDataFileName;
extern UFC::AnsiString gStockT30SDataFullFileName;
extern UFC::AnsiString gStockT30ODataFileName;
extern UFC::AnsiString gStockT30ODataFullFileName;

extern int gPriceDigi;
//------------------------------------------------------------------------------
typedef enum
{
	focDisconnect = 0,
	focConnecting = 1,
	focConnected  = 2,
        focReady      = 3

} TFubonOrderConnectionStateEnum;
//------------------------------------------------------------------------------
// class MsgUserData edit by Chelsea 20250415 
//------------------------------------------------------------------------------
class MsgUserData
{
private:
    UFC::AnsiString FHTSQ;
    UFC::AnsiString FINSQ;
    UFC::AnsiString FMGSQ;
    UFC::AnsiString FKTIME;
    UFC::AnsiString FSCODE;
    UFC::AnsiString FOCODE;
    UFC::AnsiString FTLNO;
    UFC::AnsiString FSALE;
    UFC::AnsiString FLINE;
    UFC::AnsiString FGROUP;
    UFC::AnsiString FAREA;
    UFC::AnsiString FTERM;
    UFC::AnsiString FKTYPE;
    UFC::AnsiString FKTIME9;
    UFC::AnsiString FMCODE;
    UFC::AnsiString FPRTYPE;
    UFC::AnsiString FORCT;
    UFC::AnsiString FBATCHNO;
    UFC::AnsiString FBATEND;
    UFC::AnsiString FCRPER;
    UFC::AnsiString FSTIX;
    UFC::AnsiString FPBSAMT;
    UFC::AnsiString FLAMT;
    UFC::AnsiString FFFLAG;
    UFC::AnsiString FAGROUP;
    UFC::AnsiString FP0SQTY;
    UFC::AnsiString FP0SAMT;
    UFC::AnsiString FFSTSEQ;
    UFC::AnsiString FE01USE;
    UFC::AnsiString FLPRICE;
    UFC::AnsiString FTQTY;
    UFC::AnsiString FCUGC;
    UFC::AnsiString FSTLM;
    UFC::AnsiString FRTICKET;
    UFC::AnsiString FROSEQ;
    UFC::AnsiString FSTUSCD;
    UFC::AnsiString FMQD3;
    UFC::AnsiString FHOT;
    UFC::AnsiString FSNCODE;
    UFC::AnsiString FFILLER;
    
public:
    MsgUserData();
    virtual ~MsgUserData(){};
    
    void SetLineNo( UFC::AnsiString LineNo ){ FLINE = LineNo; }
    void SetPrice( UFC::AnsiString PriceStr ){ FLPRICE = PriceStr; }
    
    void ToString( UFC::AnsiString& UserData );
    void CreateUserData( TExecutionReportMessage* Msg );
    void PriceToString( UFC::AnsiString& PriceStr, double dPrice );
};
//--------------------------------------------------------------------------
// class TFubonOrderConnection
//--------------------------------------------------------------------------
class TFubonOrderConnection : public IOrderConnectionEventListener, public UFC::PThread
{
private:    
    UFC::AnsiString    FBrokerID;
    UFC::AnsiString    FDebug;
    UFC::AnsiString    FLogFile;
    UFC::AnsiString    FLockFile;

    UFC::AnsiString    FAppName;
    UFC::AnsiString    FIP;
    int                FPort;
    UFC::AnsiString    FID;
    UFC::AnsiString    FPassword;
    int                FGroup;
    
    UFC::AnsiString    FQueIP;
    int                FQuePort;
    UFC::AnsiString    FOrderQueName;
    UFC::AnsiString    FConfirmQueName;
    UFC::AnsiString    FFilledQueName;
    UFC::AnsiString    FSentLineFileName;
    BOOL               FSkipESExec;
    BOOL               FSkipExec;
    UFC::AnsiString    FSkipAE;
    BOOL               FUsePlusVK;
    int                FReadBufferSize;
    int                FTradeTimeout;
       
    UFC::PHashedList<UFC::AnsiString, UFC::AnsiString*> FLineTable;
    UFC::PtrQueue<UFC::AnsiString> FIPPool;
    UFC::SInt          FRecoverCount;
    
    // Edit by Chelsea 20250415
    UFC::PHashMap<UFC::AnsiString, UFC::AnsiString> FUpLimitPriceMap;
    UFC::PHashMap<UFC::AnsiString, UFC::AnsiString> FOrigPriceMap;
    UFC::PHashMap<UFC::AnsiString, UFC::AnsiString> FUserDataMap;
    //-----------------------------------------------------------
    
private:
    UFC::AnsiString       FToday;
    TResourceList         FResource;
    TTaifexConnection*    FConnection;
    TSysque*              FOrderQueue;
    TSysque*              FConfirmQueue;
    TSysque*              FFilledQueue;
    char*                 FReadBuffer;    
    UFC::PCriticalSection FConnectionStateCS;
    UFC::PCriticalSection FExecutionCS;    
    UFC::UiniFile*        FSentLineNoTable; 
public:
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString,  LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count );

public:
    virtual void Execute( void );
    
public:
    TFubonOrderConnection( const UFC::AnsiString& ConfigFileName, const UFC::AnsiString& BrokerID );
    virtual ~TFubonOrderConnection( void );
    void Recover( UFC::AnsiString BeginTime );
    TFubonOrderConnectionStateEnum GetConnectionState( void );    
private:
    void BuildSentLineNumberTable( void );
    void SetLogObject( void );
    void ReadConfigFile( const UFC::AnsiString& ConfigFileName );
    void LoadStockData( UFC::UiniFile& Config );    // Edit by Chelsea 20250415
    void LockFile( void );
    void OnConfirm( TExecutionReportMessage* Msg, TSQMessage& SQMessage );
    void OnFilled( TExecutionReportMessage* Msg, TSQMessage& SQMessage );
    void OnNegotiatePxConfirm( TExecutionReportMessage* Msg );
    void Dequeue( void );
    void OnData( void );
    void New( TSQMessage& SQMessage );
    void Replace( TSQMessage& SQMessage, BOOL ReplacePx );
    void Cancel( TSQMessage& SQMessage );
    void OrderStatusRequest( TSQMessage& SQMessage );
    void NegotiatePx( TSQMessage& SQMessage );
    
private:
    TFubonOrderConnectionStateEnum FConnectionState;
    void SetConnectionState( TFubonOrderConnectionStateEnum ConnectionState );
    void CheckConnectionState( void );
    BOOL CheckTradeTime( TSQMessage& SQMessage );
    BOOL CheckTradeDate( TSQMessage& SQMessage );
    void RejectOvertimeOrder( TSQMessage& OvertimeOrder );
};
//------------------------------------------------------------------------------
#endif	/* _FubonOrderConnectionEventListener_H */
