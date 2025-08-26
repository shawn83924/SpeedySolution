// 
// File:   GCExecConnection.h
// Author: yuan
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _GCExecConnection_H
#define	_GCExecConnection_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
#include "../../Migo/Sigo.h"
//----------------------------------------------------------------------------------------------------------------------
typedef struct 
{
    Int8 FunctionCode[3];         //X(3)  New:201 Filled: 203 Reject Cancel:202
    Int8 SeqNo[8];                //9(8)  8
    Int8 FCM[10];                 //X(10) UOB,ADM
    Int8 Account[20];             //X(20) ----- Not Use -----
    Int8 ClOrdID[20];             //X(20) ----- Not Use -----
    Int8 OrigClOrdID[20];         //X(20) ----- Not Use -----
    Int8 OrderID[20];             //X(20) PATS OrderID
    Int8 OOrderID[20];            //X(20) Speedy Order ID x(6)
    Int8 SecondaryOrderID[20];    //X(20) ----- Not Use ----- ??
    Int8 ExecID[70];              //X(70) PATS FIll execid
    Int8 ExecTransType[1];        //X(1)  ----- Not Use -----
    Int8 OrderKind[20];           //X(20) QUEST,QUEST+1,EDS,REG... ??
    Int8 ExecType[1];             //X(1)  ----- Not Use -----
    Int8 OrderStatus[1];          //X(1)  New:0 Partially Filled:1 Filled:2 Canceled:4 Reject:8
    Int8 ExecRestatemwntReason[8];//9(8)  ----- Not Use -----
    Int8 Exchange[20];            //X(20) Exchange
    Int8 Symbol[20];              //X(20) Symbol
    Int8 SecurityType[5];         //X(5)  "FUT"
    Int8 MaturityMonthYear[10];   //X(20) Maturity Month Year
    Int8 StrikePrice[13];         //9(7)V9(6) Strike Price
    Int8 PutOrCall[1];            //X(1)  Put:'0' Call:'1'
    Int8 Side[1];                 //X(1)  Buy:'1' Sell:'2'
    Int8 OrderQuantity[10];       //9(10) Qty
    Int8 Price[13];               //9(7)V9(6) Price
    Int8 StopPrice[13];           //9(7)V9(6) Stop Price
    Int8 OrderType[1];            //X(1) Market:1 Limit:2 Stop:3 StopLimit:4     
    Int8 TimeInForce[1];          //X(1) ROD:0 IOC:3 FOK:4     
    Int8 ExpireDate[20];          //X(20) ----- Not Use -----
    Int8 OpenClose[1];            //X(1) ----- Not Use -----
    Int8 LastShares[10];          //X(10) LastQty
    Int8 LastPrice[13];           //9(7)V9(6) Last Price
    Int8 LeavesQuantity[10];      //X(10) Leaves Qty
    Int8 CumQuantity[10];         //X(10) ----- Not Use -----
    Int8 AveragePrice[13];        //9(7)V9(6) ----- Not Use -----
    Int8 RejectResponseTo[1];     //X(1) Cancel:'1' Replace:'2'
    Int8 RejectReason[8];         //X(1) ----- Not Use -----
    Int8 Day_Trade[1];            //X(1)  ??
    Int8 Branch[3];               //X(3) "999"
    Int8 Cust_No[7];              //X(7) "0000000"
    Int8 Ae[6];                   //X(6) "A01001"
    Int8 Web[3];                  //X(3) "801" ??
    Int8 Original[1];             //X(1) 'S' ??
    Int8 OrdDate[8];              //X(8) "20111227"
    Int8 Text[100];               //X(100) Reject reason.       
    Int8 EOS;                  // \0 
 
} PATSExec;
//----------------------------------------------------------------------------------------------------------------------
class ProcessMonitor : public MonitorListener
{
public:
    virtual void OnProcessStartup( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}
    virtual void OnProcessStopped( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}
    virtual void OnProcessConnected( BOOL IsTheFirstOne );
    virtual void OnProcessList( UFC::PStringList& Processs ){}
    virtual void OnConnected( void ){}
    virtual void OnDisconnected( void ){}
};
//--------------------------------------------------------------------------
class AEInfo
{
public:
    UFC::AnsiString AE;
    UFC::AnsiString WebID;
public:
    AEInfo( const UFC::AnsiString& ae, const UFC::AnsiString& webID ):AE( ae ), WebID( webID ) {}
};
//--------------------------------------------------------------------------
class GCExecConnection : public IOrderConnectionEventListener, public UFC::PThread
{
private:
    static UFC::PInt32            FSequence;    
private:    
    UFC::AnsiString    FDebug;
    UFC::AnsiString    FAppName;
    UFC::AnsiString    FIP;
    int                FPort;
    UFC::AnsiString    FID;
    UFC::AnsiString    FPassword;   
    UFC::AnsiString    FAccount;   
    UFC::AnsiString    FBrokerID;   
    char               FOrderIDPrefix;   
private:    
    UFC::AnsiString    FWEBID;    
private:
    TResourceList         FResource;
    TTaifexConnection*    FConnection; 
    BOOL                  FIsLogon;
    UFC::PHashedList<UFC::AnsiString, AEInfo*>         FAETable;
public: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, bool IsAccepted, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, bool PossDup );
    virtual void OnRecoverFinished( int Count );
public: ///< Implement PThread
    void PATSExecutionReport( TExecutionReportMessage* Msg, BOOL );
    void ConfirmToBackend( TExecutionReportMessage* Msg );
    void FillToBackend( TExecutionReportMessage* Msg );
    void TSEConfirmToBackend( TExecutionReportMessage* Msg );
    void TSEFillToBackend( TExecutionReportMessage* Msg );
    void LoadSetting( void );
    virtual void Execute( void );    
public:
    GCExecConnection( int RecoverBeginTime );
    virtual ~GCExecConnection( void );
    void Recover( UFC::AnsiString BeginTime, char Type );    
};
//------------------------------------------------------------------------------
#endif
