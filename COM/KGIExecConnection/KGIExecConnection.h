// 
// File:   YuantaExecConnection.h
// Author: yuan
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _KGIExecConnection_H
#define	_KGIExecConnection_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
#include "../../Migo/Sigo.h"
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
    AEInfo( void ):AE( "          " ), WebID( "  " ) {}
};

//----------------------------------------------------------------------------------------------------------------------
typedef struct 
{
    Int8 FunctionCode[3];         //X(3)        0 New:201 Filled: 203 Reject Cancel:202
    Int8 SeqNo[8];                //9(8)        3 8
    Int8 FCM[10];                 //X(10)      11 UOB,ADM
    Int8 Account[20];             //X(20)      21 ----- Not Use -----
    Int8 ClOrdID[20];             //X(20)      41 ----- Not Use -----
    Int8 OrigClOrdID[20];         //X(20)      61 ----- Not Use -----
    Int8 OrderID[20];             //X(20)      81 PATS OrderID
    Int8 OOrderID[20];            //X(20)     101 Speedy Order ID x(6)
    Int8 SecondaryOrderID[20];    //X(20)     121 ----- Not Use ----- ??
    Int8 ExecID[70];              //X(70)     141 PATS FIll execid
    Int8 ExecTransType[1];        //X(1)      211 ----- Not Use -----
    Int8 OrderKind[20];           //X(20)     212 QUEST,QUEST+1,EDS,REG... ??
    Int8 ExecType[1];             //X(1)      232 ----- Not Use -----
    Int8 OrderStatus[1];          //X(1)      233 New:0 Partially Filled:1 Filled:2 Canceled:4 Reject:8
    Int8 ExecRestatemwntReason[8];//9(8)      234 ----- Not Use -----
    Int8 Exchange[20];            //X(20)     242 Exchange
    Int8 Symbol[20];              //X(20)     262 Symbol
    Int8 SecurityType[5];         //X(5)      282 "FUT"
    Int8 MaturityMonthYear[10];   //X(20)     287 Maturity Month Year
    Int8 StrikePrice[13];         //9(7)V9(6) 297 Strike Price
    Int8 PutOrCall[1];            //X(1)      310 Put:'0' Call:'1'
    Int8 Side[1];                 //X(1)      311 Buy:'1' Sell:'2'
    Int8 OrderQuantity[10];       //9(10)     312 Qty
    Int8 Price[13];               //9(7)V9(6) 322 Price
    Int8 StopPrice[13];           //9(7)V9(6) 335 Stop Price
    Int8 OrderType[1];            //X(1)      348 Market:1 Limit:2 Stop:3 StopLimit:4     
    Int8 TimeInForce[1];          //X(1)      349 ROD:0 IOC:3 FOK:4     
    Int8 ExpireDate[20];          //X(20)     350 ----- Not Use -----
    Int8 OpenClose[1];            //X(1)      370 ----- Not Use -----
    Int8 LastShares[10];          //X(10)     371 LastQty
    Int8 LastPrice[13];           //9(7)V9(6) 381 Last Price
    Int8 LeavesQuantity[10];      //X(10)     394 Leaves Qty
    Int8 CumQuantity[10];         //X(10)     404 ----- Not Use -----
    Int8 AveragePrice[13];        //9(7)V9(6) 414 ----- Not Use -----
    Int8 RejectResponseTo[1];     //X(1)      427 Cancel:'1' Replace:'2'
    Int8 RejectReason[8];         //X(8)      428 ----- Not Use -----
    Int8 Day_Trade[1];            //X(1)      436 ??
    Int8 Branch[3];               //X(3)      437 "999"
    Int8 Cust_No[7];              //X(7)      440 "0000000"
    Int8 Ae[6];                   //X(6)      447 "A01001"
    Int8 Web[3];                  //X(3)      453 "801" ??
    Int8 Original[1];             //X(1)      456 'S' ??
    Int8 OrdDate[8];              //X(8)      457 "20111227"
    Int8 OrdTime[6];              //X(6)      465 "125959"
    Int8 Text[94];                //X(94)     471 Reject reason.       
    Int8 EOS;                     // \0       565
 
} PATSExec;

//--------------------------------------------------------------------------
bool IsDigitalChar(char OrigChar);
bool IsMonthStr(const UFC::AnsiString& OrigStr);
UFC::AnsiString DescribeOrderStatus(nsOrderMessageDefine::OrderStatusEnum OrderStatus);
UFC::AnsiString DescribeExecType(nsOrderMessageDefine::ExecTypeEnum ExecType);
UFC::AnsiString DescribeSide(nsOrderMessageDefine::SideEnum Side);
Int64 DoubleToInt64( double DoubleVal, int Digi);
//--------------------------------------------------------------------------
class CPatsDateObject
{
public:
    static int GetDigitalStr(const UFC::AnsiString& OrigStr, int BeginIndex, int MaxLength, UFC::AnsiString& DigitalStr);
    static bool ParsePatsDateStr(const UFC::AnsiString& PatsDateStr, UFC::AnsiString& MaturityMonthStr, UFC::AnsiString& MaturityYearStr, UFC::AnsiString& StrikePriceStr, char& CallPutChar);
    
private:
    UFC::AnsiString FPatsDateStr;
    UFC::AnsiString FMaturityMonthStr;
    UFC::AnsiString FMaturityYearStr;
    UFC::AnsiString FMaturityMonthYearStr;
    UFC::AnsiString FStrikePriceStr;
    char            FCallPutChar;
    bool            FIsCorrectDateStr;
    
public:
    UFC::AnsiString GetPatsDateStr() {return FPatsDateStr;};
    UFC::AnsiString GetMaturityMonthStr() {return FMaturityMonthYearStr;};
    UFC::AnsiString GetMaturityYearStr() {return FMaturityMonthYearStr;};
    UFC::AnsiString GetMaturityMonthYearStr() {return FMaturityMonthYearStr;};
    UFC::AnsiString GetStrikePriceStr() {return FStrikePriceStr;};
    char            GetCallPutChar() {return FCallPutChar;};
    bool            IsCorrectDateStr() {return FIsCorrectDateStr;};
    
    void DupmData();
    CPatsDateObject(const UFC::AnsiString& PatsDateStr);
};  //CPatsDateObject

//--------------------------------------------------------------------------
int ClearPatsContractDateArray(std::vector<CPatsDateObject*>& DateObjArray);
int ParsePatsContractDate(const UFC::AnsiString& MultiContractDate, std::vector<CPatsDateObject*>& DateObjArray);

//--------------------------------------------------------------------------
class CPATSConfirmExecutionReport
{
public:
    UFC::AnsiString FOrderId;
    UFC::AnsiString FPATSContractDate;
    PATSExec        FExecRptData;
    bool            FLeg1Sended;
    bool            FLeg2Sended;
    CPATSConfirmExecutionReport(const UFC::AnsiString& OrderId, const UFC::AnsiString& PATSContractDate, PATSExec *ExecRptDataPtr);
};  //CPATSConfirmExecutionReport

//--------------------------------------------------------------------------
class KGIExecConnection : public IOrderConnectionEventListener, public UFC::PThread
{
public:
    static bool FIsTWSERECVNewVersion;
    static bool FIsTWSESENDNewVersion;
    
private:
    static UFC::PInt32 FSequence;
    
private:    
    UFC::AnsiString FDebug;
    UFC::AnsiString FAppName;
    UFC::AnsiString FIP;
    int             FPort;
    UFC::AnsiString FID;
    UFC::AnsiString FPassword;
    UFC::AnsiString FAccount;
    UFC::AnsiString FBrokerID;
    char            FOrderIDPrefix;   
    UFC::AnsiString FWEBID;
    UFC::AnsiString FCustomWEBID;
    UFC::AnsiString FCustomAE;
    int             FAEOffset;
    bool            FIsBrokerModeEnabled;
    UFC::AnsiString FConfirmSubject;
    UFC::AnsiString FConfirmKey;
    UFC::AnsiString FFillSubject;
    UFC::AnsiString FFillKey;
    UFC::AnsiString FTSEConfirmSubject;
    UFC::AnsiString FTSEConfirmKey;
    UFC::AnsiString FOTCConfirmSubject;
    UFC::AnsiString FOTCConfirmKey;
    UFC::AnsiString FTSEFillSubject;
    UFC::AnsiString FTSEFillKey;
    UFC::AnsiString FOTCFillSubject;
    UFC::AnsiString FOTCFillKey;
    UFC::AnsiString FPTDesk;
    bool            FIntelligentUseUDDWebID;    
private:
    int               FRecoverBeginTime;    
    TResourceList     FResource;
    TTaifexConnection *FConnection; 
    BOOL              FIsLogon;    
    UFC::PHashedList<UFC::AnsiString, AEInfo*> FAETable;
    UFC::PHashedSet<UFC::AnsiString>           FExecIdSet;
    UFC::PHashedSet<UFC::AnsiString>           FIntelligentAE;
    
    UFC::PHashedSet<UFC::AnsiString> FPatsExecIdSet;
    UFC::PHashedList<UFC::AnsiString, CPATSConfirmExecutionReport*> FPATSConfirmExecRptTable;
    UFC::PHashMap<UFC::AnsiString, UFC::AnsiString> FSymbolConvertTable;
    UFC::PHashMap<UFC::AnsiString, double> FCommodityMultiplierTable;
    
    AEInfo FDefAE;
    void GetAEWebID( const UFC::AnsiString& Value, UFC::AnsiString& AE, UFC::AnsiString& WebID );
public: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count );
    
    
public:
    void ConfirmToBackend( TExecutionReportMessage* Msg );
    void FillToBackend( TExecutionReportMessage* Msg );
    UFC::AnsiString ConvertOldTSENormalConfirmRptToNew(const UFC::AnsiString& OldTelegram);
    UFC::AnsiString ConvertOldTSEOddConfirmRptToNew(const UFC::AnsiString& OldTelegram);
    void TSEConfirmToBackend( TExecutionReportMessage* Msg );
    UFC::AnsiString ConvertOldTSENormalFillRptToNew(const UFC::AnsiString& OldTelegram);
    void TSEFillToBackend( TExecutionReportMessage* Msg );
    void LoadSetting( void );
    
public: ///< Implement PThread
    virtual void Execute( void );
    
public:
    KGIExecConnection( int RecoverBeginTime );
    virtual ~KGIExecConnection( void );
    void Recover( const UFC::AnsiString& BeginTimeStr, const UFC::AnsiString& EndTimeStr, char Type, char EType );
    double FindCommodityMultiplier(const UFC::AnsiString ExchangeCode, const UFC::AnsiString Symbol);
    void PATSExecutionReport( TExecutionReportMessage* Msg, BOOL );
};
//------------------------------------------------------------------------------
#endif
