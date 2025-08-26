/* 
 * File:   PMFExecConnection.h
 * Author: zhen
 *
 * Created on April 10, 2009, 12:05 PM
 */
#ifndef _PMFEXECCONNECTION_H
#define	_PMFEXECCONNECTION_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
#include "../../Migo/Sigo.h"
#include "MultilegSymbol.h"

//------------------------------------------------------------------------------
typedef struct odkdata {
    Int8 DATE[8];                   //9(8)         0
    //ODK-AREA
        //ODK-HEAD
    Int8 MSG_SEQNUM[10];            //9(10)        8
    Int8 MSG_DATE[8];               //9(8)        18
    Int8 MSG_TIME[6];               //9(6)        26
    Int8 MSG_MIL[4];                //9(4)        32
    Int8 MSG_TYPE[3];               //X(3)        36
    Int8 LNK_BROKER_NO[5];          //X(5)        39
    Int8 SESSION_ID[5];             //9(5)        44
        //ODK-R02
    Int8 STATUS_CODE[3];            //X(3)        49
    Int8 EXEC_TYPE[1];              //X(1)        52
    Int8 CLEAR_MEMBER_ID_NO[5];     //X(5)        53
    Int8 BROKER_ID_NO[5];           //X(5)        58
    Int8 ORDER_NO[5];               //X(5)        63
    Int8 ORD_ID[7];                 //9(7)        68
    Int8 USER_DEFINE[8];            //X(8)        75
    Int8 SYBMOL_TYPE[1];            //X(1)        83
    Int8 TR_ID[20];                 //X(20)       84
    Int8 ORDER_PRICE[9];            //S9(8)      104
    Int8 ORDER_QTY[4];              //9(4)       113
    Int8 INVESTOR_ACNO[7];          //X(7)       117
    Int8 INVESTOR_FLAG[1];          //X(1)       124
    Int8 BUY_SELL_KIND[1];          //X(1)       125
    Int8 ORDER_TYPE[1];             //X(1)       126
    Int8 ORDER_COND[1];             //X(1)       127
    Int8 OPEN_OFFSET_KIND[1];       //X(1)       128
    Int8 LAST_MATCH_PRICE[10];      //S9(9)      129
    Int8 LAST_MATCH_QTY[4];         //9(4)       139
    Int8 TOT_MATCH_PRICE[17];       //S9(16)     143
    Int8 TOT_MATCH_QTY[4];          //9(4)       160
    Int8 LEAVES_QTY[4];             //9(4)       164
    Int8 BEFORE_QTY[4];             //9(4)       168
    Int8 BUY_SELL_KIND1[1];         //X(1)       172
    Int8 BUY_SELL_KIND2[1];         //X(1)       173
    Int8 MATCH_PRICE1[9];           //9(9)       174
    Int8 MATCH_PRICE2[9];           //9(9)       183
    Int8 MATCH_QTY1[4];             //9(4)       192
    Int8 MATCH_QTY2[4];             //9(4)       196
    Int8 ORDER_DATE[8];             //9(8)       200
    Int8 ORDER_TIME[6];             //9(6)       208
    Int8 ORDER_MIL[4];              //9(4)       214
    Int8 LAST_DATE[8];              //9(8)       218
    Int8 LAST_TIME[6];              //9(6)       226
    Int8 LAST_MIL[4];               //9(4)       232
    Int8 TARGET_ID[1];              //X(1)       236
    Int8 UNIQUE_ID[10];             //9(10)      237
    Int8 SEQNO[10];                 //9(10)      247
    Int8 PROTOCOL_TYPE[1];          //X(1)       257
        //PMF Internal Use
    Int8 BROKER_ID[7];              //X(7)       258
    Int8 COMMODITY_ID1_1[7];        //X(7)       265
    Int8 SETTLEMENT_MONTH1_1[6];    //9(6)       272
    Int8 STRIKE_PRICE1_1[9];        //9(6)9(3)   278
    Int8 MATCH_PRICE1_1[12];        //9(6)V9(6)  287
    Int8 COMMODITY_ID2_2[7];        //X(7)       299
    Int8 SETTLEMENT_MONTH2_2[6];    //9(6)       306
    Int8 STRIKE_PRICE2_2[9];        //9(6)9(3    312
    Int8 MATCH_PRICE2_2[12];        //9(6)V9(6)  321
    Int8 MATCH_PRICE_D[13];         //S9(6)V9(6) 333
    Int8 ODR_PRICE[9];              //S9(5)V9(3) 346
    Int8 CLEAR_MEMBER_ID[4];        //X(4)       355
    Int8 AE_ID[4];                  //9(4)       359
    Int8 DAY_TRADE_ID[1];           //X(1)       363
    Int8 IB_NO[3];                  //X(03)      364
    Int8 DOS_IN_KIND[1];            //X(01)      367
    Int8 SUB_INVESTOR_ACNO[3];      //X(03)      368
    Int8 NT_CHANNEL[2];             //X(02)      371
    Int8 FILLER[27];                //X(27)      373
    Int8 END;
} ODK;

union ODKMessage {
    ODK Detail;
    char Data[401];
};
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
};  //ProcessMonitor

//--------------------------------------------------------------------------
class PMFExecConnection : public IOrderConnectionEventListener, public UFC::PThread
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
    UFC::PHashedSet<UFC::AnsiString> FBranchSet;
private:
    UFC::AnsiString    FWEBID;
private:
    int                   FRecoverBeginTime;
    TResourceList         FResource;
    TTaifexConnection*    FConnection;
    BOOL                  FIsLogon;
public: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count );
public: ///< Implement PThread
    void LoadSetting( void );
    virtual void Execute( void );
public:
    PMFExecConnection( int RecoverBeginTime );
    virtual ~PMFExecConnection( void );
    void Recover( UFC::AnsiString BeginTime );
    void BuildMessageString(TExecutionReportMessage* Msg, UFC::AnsiString& MessageString);
private:
    BOOL IsBlocked( TExecutionReportMessage* Msg );
};  //PMFExecConnection

//------------------------------------------------------------------------------ 
#endif	/* _PMFEXECCONNECTION_H */

