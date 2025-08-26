// 
// File:   UniExecConnection.h
// Author: Zhen Fan
//
// Created on 2015/03/31, 16:21:00
// Source from GCExecConnection
//------------------------------------------------------------------------------
#ifndef _GCExecConnection_H
#define	_GCExecConnection_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
#include "Sigo.h"
//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
    unsigned char msgid[3];        //  1.   0     3
    unsigned char statuscode[3];   //  2.   3     6
    unsigned char exectype;        //  3.   6     7
    unsigned char cmdid[5];        //  4.   7    12
    unsigned char fcmid[5];        //  5.  12    17
    unsigned char orderno[5];      //  6.  17    22
    unsigned char ordid[7];        //  7.  22    29
    unsigned char userdf[8];       //  8.  29    37
    unsigned char commod;          //  9.  37    38
    unsigned char commodx[20];     // 10.  38    58
    unsigned char price[9];        // 11.  58    67
    unsigned char quantity[5];     // 12.  67    72
    unsigned char ivacno[7];       // 13.  72    79
    unsigned char ivacflag;        // 14.  79    80
    unsigned char side;            // 15.  80    81
    unsigned char ordtype;         // 16.  81    82
    unsigned char ordcnd;          // 17.  82    83
    unsigned char opnoff;          // 18.  83    84
    unsigned char lastpx[9];       // 19.  84    93
    unsigned char lastqty[5];      // 20.  93    98
    unsigned char pxsubtotal[17];  // 21.  98   115
    unsigned char cumqty[5];       // 22. 115   120
    unsigned char leavesqty[5];    // 23. 120   125
    unsigned char beforeqty[5];    // 24. 125   130
    unsigned char legside1;        // 25. 130   131
    unsigned char legside2;        // 26. 131   132
    unsigned char legpx1[9];       // 27. 132   141
    unsigned char legpx2[9];       // 28. 141   150
    unsigned char legqty1[5];      // 29. 150   155
    unsigned char legqty2[5];      // 30. 155   160
    unsigned char orgtime[9];      // 31. 160   169 精準到ms
    unsigned char trntime[9];      // 32. 169   178 精準到ms
    unsigned char targetid;        // 33. 178   179
    unsigned char uniqid[10];      // 34. 179   189
    unsigned char rptseq[10];      // 35. 189   199
    unsigned char protocol;        // 36. 199   200
    unsigned char ssnid[5];        // 37. 200   205 自訂欄位
    unsigned char trader[5];       // 38. 205   210 自訂欄位
    unsigned char systype[2];      // 39. 210   212 
    unsigned char filler[44];      // 40. 212   256 自訂欄位
    unsigned char endChar;
} tcp_R02_t;
//----------------------------------------------------------------------------------------------------------------------
union UniBackEndMessage {
    tcp_R02_t     detail;
    unsigned char buffer[257];
};  //UniBackEndMessage
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
    Int8 OrdTime[6];              //X(6) "125959"
    Int8 Text[94];                //X(94) Reject reason.       
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
class UniExecConnection : public IOrderConnectionEventListener, public UFC::PThread
{
private:
    static UFC::PInt32 FSequence;
public:    
    static bool FIsDumpOnly;
    static MessageObject *FMsgObjPtr;
    static UFC::AnsiString TrimBeginZero(const UFC::AnsiString& SourceStr);
    
private:    
    UFC::AnsiString FAppName;
    UFC::AnsiString FIP;
    int             FPort;
    UFC::AnsiString FID;
    UFC::AnsiString FPassword;
    UFC::AnsiString FAccount;
    UFC::AnsiString FBrokerID;
    UFC::AnsiString FWEBID;
    char            FOrderIDPrefix;
    
    TResourceList     FResource;
    TTaifexConnection *FConnection;
    BOOL              FIsLogon;
    UFC::PHashedList<UFC::AnsiString, AEInfo*> FAETable;
    
    UFC::PCriticalSection FExecReportLogLock;
    UFC::FileStreamEx *FExecReportLogPtr;
    
public: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count );
    virtual void OnCancelWorking( CancelWorkingResult Result, int Count ) {};
    virtual void OnNews( TNewsMessage* Msg ) {}
    
    ///< Implement PThread
    void PATSExecutionReport( TExecutionReportMessage* Msg, BOOL );
    void ConfirmToBackend( TExecutionReportMessage* Msg );
    void FillToBackend( TExecutionReportMessage* Msg );
    void TSEConfirmToBackend( TExecutionReportMessage* Msg );
    void TSEFillToBackend( TExecutionReportMessage* Msg );
    void LoadSetting( void );
    virtual void Execute( void );
    
public:
    UniExecConnection( const UFC::AnsiString& AppName, const UFC::AnsiString& IP, int Port, 
                       const UFC::AnsiString& ID, const UFC::AnsiString& Password, const UFC::AnsiString& Account,
                       const UFC::AnsiString& BrokerID, const UFC::AnsiString WEBID, char OrderIDPrefix );
    virtual ~UniExecConnection( void );
    void Recover( UFC::AnsiString BeginTime, char Type );    
};
//------------------------------------------------------------------------------
#endif
