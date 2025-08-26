/* 
 * File:   TandemObject.h
 * Author: Zhen Fan
 *
 * Created on 2011/12/16 11:57
 */
#ifndef TANDEMOBJECT_H
#define	TANDEMOBJECT_H

#include <map>
#include <queue>
#include <set>
#include "UFC.h"
#include "ConditionMutex.h"                        //UFC::PLUS
#include "OrderMessage/OrderMessageDefine.h"       // Lib
#include "OrderMessage/TExecutionReportMessage.h"  // Lib

namespace capital
{
const char *DescribeExecRptMarket(nsOrderMessageDefine::MarketEnum execRptMarket);
const char *DescribeExecRptOrderStatus(nsOrderMessageDefine::OrderStatusEnum execRptOrderStatus);

//------------------------------------------------------------------------------
class CUserData
{
private:
    static UFC::AnsiString FFieldDelimiter;
    static UFC::AnsiString FNameValueDelimiter;
    
    UFC::AnsiString FSubAccountNo; //62.ODK-SUB-MOTHER-ACNO X(7)   355
    UFC::AnsiString FAEID;         //58.ODK-AE-ID           9(4)   346
    UFC::AnsiString FInputSeqNo;   //65.ODK-INPUT-SEQNO     S9(13) 375
    char            FAp2InKind;    //67.ODK-AP2-IN-KIND     X(01)  403
    UFC::AnsiString FSourceIP;     //69.ODK-SRC-IP          X(20)  420
    
    void ParseFieldValue(const UFC::AnsiString& csvStr);
public:
    CUserData();
    CUserData(const UFC::AnsiString& csvStr);
    
    UFC::AnsiString GetSubAccountNo() {return FSubAccountNo;};
    UFC::AnsiString GetAEID() {return FAEID;};
    UFC::AnsiString GetInputSeqNo() {return FInputSeqNo;};
    char            GetAp2InKind() {return FAp2InKind;};
    UFC::AnsiString GetSourceIP() {return FSourceIP;};
};  //CUserData
//------------------------------------------------------------------------------    
class TandemData
{
public:    
    Int64    DoubleToInt64(double DoubleVal, int Digi);        
    virtual ~TandemData( ){}
    virtual void GenerateTandemData( Int8* Data ) = 0;
    virtual int  GetTandemDataLength( void ) = 0;
};
//------------------------------------------------------------------------------    
class TandemTAIFEXData: public TandemData
{
public:       
    nsOrderMessageDefine::MarketEnum      FExecRptMarket;
    nsOrderMessageDefine::OrderStatusEnum FExecRptOrderStatus;    
    UFC::AnsiString                       FExecID;    
    
    UFC::AnsiString FTradeDate;            // 1.ODK-DATE                9(8)          0
    Int32           FSequenceNo;           // 2.ODK-SEQNUM              9(10)         8
    UFC::AnsiString FReceiveDate;          // 3.ODK-MSG-DATE            9(8)         18
    UFC::AnsiString FReceiveTime;          // 4.ODK-MSG-TIME            9(6)         26
    UFC::AnsiString FReceiveMilliSecond;   // 5.ODK-MSG-MIL             9(4)         32
    UFC::AnsiString FMessageType;          // 6.ODK-MSG-TYPE            X(3)         36
    UFC::AnsiString FLinkBrokerNo;         // 7.ODK-LNK-BROKER-NO       X(5)         39
    UFC::AnsiString FSessionID;            // 8.ODK-SESSION-ID          9(5)         44
    UFC::AnsiString FStatusCode;           // 9.ODK-STATUS-CODE         X(3)         49
    char            FExecType;             //10.ODK-EXEC-TYPE           X(1)         52
    UFC::AnsiString FClearMemberIDNo;      //11.ODK-CLEAR-MEMBER-ID-NO  X(5)         53
    UFC::AnsiString FBrokerIDNo;           //12.ODK-BROKER-ID-NO        X(5)         58
    UFC::AnsiString FOrderNo;              //13.ODK-ORDER-NO            X(5)         63
    UFC::AnsiString FOrderID;              //14.ODK-ORD-ID              9(7)         68
    UFC::AnsiString FUserDefine;           //15.ODK-USER-DEFINE         X(8)         75
    char            FSymbolType;           //16.ODK-SYMBOL-TYPE         X(1)         83
    UFC::AnsiString FSymbol;               //17.ODK-TR-ID               X(20)        84
    Int32           FOrderPrice;           //18.ODK-ORDER-PRRICE        S9(8)       104
    Int32           FOrderQty;             //19.ODK-ORDER-QTY           9(4)        113
    UFC::AnsiString FInvestorAccountNo;    //20.ODK-INVESTOR-ACNO       X(7)        117
    char            FInvestorFlag;         //21.ODK-INVESTOR-FLAG       X(1)        124
    char            FSide;                 //22.ODK-BUY-SELL-KIND       X(1)        125
    char            FOrderType;            //23.ODK-ORDER-TYPE          X(1)        126
    char            FTimeInForce;          //24.ODK-ORDER-COND          X(1)        127
    char            FPositionEffect;       //25.ODK-OPEN-OFFSET-KIND    X(1)        128
    Int32           FLastPrice;            //26.ODK-LAST-MATCH-PRICE    S9(9)       129
    Int32           FLastQty;              //27.ODK-LAST-MATCH-QTY      9(4)        139
    Int64           FPriceSubTotal;        //28.ODK-TOT-MATCH-PRICE     S9(16)      143
    Int32           FCumQty;               //29.ODK-TOT-MATCH-QTY       9(4)        160
    Int32           FLeavesQty;            //30.ODK-LEAVES-QTY          9(4)        164
    Int32           FBeforeQty;            //31.ODK-BEFORE-QTY          9(4)        168
    char            FLegSide1;             //32.ODK-BUY-SELL-KIND1      X(1)        172
    char            FLegSide2;             //33.ODK-BUY-SELL-KIND2      X(1)        173
    Int32           FLegPrice1;            //34.ODK-MATCH-PRICE1        9(9)        174
    Int32           FLegPrice2;            //35.ODK-MATCH-PRICE2        9(9)        183
    Int32           FLegQty1;              //36.ODK-MATCH-QTY1          9(4)        192
    Int32           FLegQty2;              //37.ODK-MATCH-QTY2          9(4)        196
    UFC::AnsiString FOrderDate;            //38.ODK-ORDER-DATE          9(8)        200
    UFC::AnsiString FOrderTime;            //39.ODK-ORDER-TIME          9(6)        208
    UFC::AnsiString FOrderMilliSecond;     //40.ODK-ORDER-MIL           9(4)        214
    UFC::AnsiString FTransactDate;         //41.ODK-LAST-DATE           9(8)        218
    UFC::AnsiString FTransactTime;         //42.ODK-LAST-TIME           9(6)        226
    UFC::AnsiString FTransactMilliSecond;  //43.ODK-LAST-MIL            9(4)        232
    char            FTargetID;             //44.ODK-TARGET-ID           X(1)        236
    Int32           FUniqueID;             //45.ODK-UNIQUE-ID           9(10)       237
    Int32           FReportSequenceNo;     //46.ODK-SEQNO               9(10)       247
    char            FProtocolType;         //47.ODK-PROTOCOL-TYPE       X(1)        257
    UFC::AnsiString FBrokerID;             //48.ODK-BROKER-ID           X(7)        258
    UFC::AnsiString FCommodityID1;         //49.ODK-COMMODITY-ID1-1     X(7)        265
    UFC::AnsiString FSettlementMonth1;     //50.ODK-SETTLEMENT-MONTH1-1 9(6)        272
    UFC::AnsiString FStrikePrice1;         //51.ODK-STRIKE-PRICE1-1     9(6)V9(3)   278
    Int64           FPrice1;               //52.ODK-PRICE1-1            9(6)V9(6)   287
    UFC::AnsiString FCommodityID2;         //53.ODK-COMMODITY-ID2-2     X(7)        299
    UFC::AnsiString FSettlementMonth2;     //54.ODK-SETTLEMENT-MONTH2-2 9(6)        306
    UFC::AnsiString FStrikePrice2;         //55.ODK-STRIKE-PRICE2-2     9(6)V9(3)   312
    Int64           FPrice2;               //56.ODK-PRICE2-2            9(6)V9(6)   321
    Int64           FPriceDifference;      //57.ODK-PRICE2-D            S9(6)V9(6)  333
    UFC::AnsiString FAEID;                 //58.ODK-AE-ID               9(4)        346
    char            FDayTradeID;           //59.ODK-DAY-TRADE-ID        X(1)        350
    UFC::AnsiString FIBNO;                 //60.ODK-IB-NO               X(03)       351
    char            FDosInKind;            //61.ODK-DOS-IN-KIND         X(01)       354
    UFC::AnsiString FSubAccountNo;         //62.ODK-SUB-MOTHER-ACNO     X(7)        355
    Int32           FOdrPrice;             //63.ODK-ODR-PRRICE          S9(05)V9(4) 362
    UFC::AnsiString FClearMemberID;        //64.ODK-CLEAR-MEMBER-ID     X(03)       372
    UFC::AnsiString FInputSeqNo;           //65.ODK-INPUT-SEQNO         S9(13)      375
    UFC::AnsiString FInputKey;             //66.ODK-INPUT-KEY           S9(13)      389
    char            FAp2InKind;            //67.ODK-AP2-IN-KIND         X(01)       403
    char            FAp2InKindN;           //68.ODK-AP2-IN-KIND-N       X(01)       404
    char            FPriceFlag;            //69.ODK-PRICE-FLAG          X(01)       405    
    char            FMarketMark;           //70.ODK-MARKET-MARK         X(01)       406
    char            FSessionMark;          //71.ODK-ETS-FLAG            X(01)       407
    char            FFLEXFlag;             //72.ODK-FLEXFLAG            X(01)       408
    UFC::AnsiString FPartID;               //73.ODK-PART-ID             9(03)       409
    UFC::AnsiString FStatusCodeA;          //74.ODK-STATUS-CODE-A       9(02)       412
    UFC::AnsiString FFiller;               //75.FILLER                  X(06)       414
    UFC::AnsiString FSourceIP;             //76.ODK-SRC-IP              X(20)       420
    ///                                                                 Total       440 
private:    
    char TranslateExecType(nsOrderMessageDefine::ExecTypeEnum rptExecType, 
                           nsOrderMessageDefine::TMPExecTypeEnum rptTMPExecType,
                           nsOrderMessageDefine::CxlRejResponseToEnum rptRejResponseTo);
    char TranslateSide(nsOrderMessageDefine::SideEnum rptSide, char& BuySellCode);
    char TranslateOrderType(nsOrderMessageDefine::OrderTypeEnum rptOrderType);
    char TranslateTimeInForce(nsOrderMessageDefine::TimeInForceEnum rptTimeInForce);
    char TranslatePositionEffect(nsOrderMessageDefine::PositionEffectEnum rptPositionEffect);
    UFC::AnsiString ConvertToCommodityID(const UFC::AnsiString& newSymbol, nsOrderMessageDefine::MarketEnum rptMarket, BOOL IsFLEX, char callPutCode);    
    void SetFieldsValue( TExecutionReportMessage *ExecRptPtr );
public:
    TandemTAIFEXData( TExecutionReportMessage *ExecRptPtr );    
    ~TandemTAIFEXData( ){}
    virtual void GenerateTandemData( Int8* Data );    
    virtual int  GetTandemDataLength( void ) { return 440; }
};
//------------------------------------------------------------------------------    
class TandemStockData: public TandemData
{
public:       
    nsOrderMessageDefine::MarketEnum      FExecRptMarket;
    nsOrderMessageDefine::OrderStatusEnum FExecRptOrderStatus;    
    
    UFC::AnsiString FMO_MARKET_MARK2; //X(1) S: TWSE O: OTC P: ES
    UFC::AnsiString FMO_RPT_KIND;     //X(1) 2: Fill D: Cancel by Ex Z: Confirm
    UFC::AnsiString FMO_EXECID;       //X(12) 
    UFC::AnsiString FMO_FILLER1;      //X(4)
    UFC::AnsiString FRO_INPUT_SEQNO;  //X(13)           
    UFC::AnsiString FRO_INPUT_KEY;    //X(13)           
    UFC::AnsiString FRO_ORDER_NO;     //X(5)           
    UFC::AnsiString FRO_FILLER2;      //X(1)           
    UFC::AnsiString FMO_FUNCTION_CODE;//X(2) "01": Buy "02": Sell "03": Reduce "04": Cancel  
    UFC::AnsiString FMO_MARKET_MARK;  //X(1) S: TWSE O: OTC P: ES          
    UFC::AnsiString FMO_AP_CODE;      //X(1) 0:Normal 2:Odd 7:Post          
    UFC::AnsiString FMO_BROKER_ID;    //X(4)            
    UFC::AnsiString FMO_ACNO;         //9(7)           
    UFC::AnsiString FMO_SUB_ACNO;     //9(7) "0000000"          
    UFC::AnsiString FMO_OTA_MARK;     //X(1) " "          
    UFC::AnsiString FMO_OTA_ORDER_ID; //X(10)                    
    UFC::AnsiString FMO_BUYSELL;      //X(1) B:Buy S:Sell          
    UFC::AnsiString FMO_COMPANY_NO;   //X(6) Symbol         
    UFC::AnsiString FMO_STOCK_NOS;    //9(9)          
    UFC::AnsiString FMO_PRICE;        //9(5)V9(4)          
    UFC::AnsiString FMO_PRICE_MARK;   //X(1) " "          
    UFC::AnsiString FMO_ORDER_TYPE;   //X(1) TWSE OrderType           
    UFC::AnsiString FMO_PRICE_TYPE;   //X(1) 1: Market 2:Limit         
    UFC::AnsiString FMO_TIMEINFORCE;  //X(1) 0: ROD 3:IOC 4:FOK
    UFC::AnsiString FMO_SELLER;       //9(4) 
    UFC::AnsiString FMO_C_KEY;        //9(18)"  0000000         "  
    UFC::AnsiString FMO_SettlType_63; //X(1) 0: Regular
    UFC::AnsiString FMO_FILLER3;      //X(5) 
    UFC::AnsiString FMO_ORDER_DATE;   //9(8) YYYYMMDD         
    UFC::AnsiString FMO_ORDER_TIME;   //9(9) HHMMSSmmm         
    UFC::AnsiString FMO_TSE_AREA;     //X(25)
    UFC::AnsiString FMO_INPUT_SOURCE; //X(2) " s"          
    UFC::AnsiString FMO_INPUT_DEVICE; //X(16) IP 
    UFC::AnsiString FMO_INPUT_TEL;    //9(6) "000000"
    UFC::AnsiString FMO_ARDAY_FILEID; //X(2) "  "
    UFC::AnsiString FMO_ORDAY_GROUP;  //9(4) "0000" 
    UFC::AnsiString FMO_CHANNEL;      //X(1) '3' DMA
    UFC::AnsiString FMO_EXCHANGE_CODE;//X(1) 0: Normal 1: Mass 2:Odd
    UFC::AnsiString FMO_RM_MARK;      //X(1) " "
    UFC::AnsiString FMO_CURRENCY;     //X(3) "   "
    UFC::AnsiString FMO_STOCK_SOURCE; //9(1) 0: Normal 1: 2: 8:Day Trade
    UFC::AnsiString FMO_OFFSET_MARK;  //X(1) " "
    UFC::AnsiString FMO_BOOKING_MARK; //X(1) " " 
    UFC::AnsiString FMO_FILLER4;      //X(19)              
private:    
    void TSEConfirmArea( UFC::AnsiString& FMO_TSE_AREA, TExecutionReportMessage *ExecRptPtr );
    void TSEFillArea( UFC::AnsiString& FMO_TSE_AREA, TExecutionReportMessage *ExecRptPtr );    
    void UniqueExecID( UFC::AnsiString& FMO_EXECID, TExecutionReportMessage *ExecRptPtr );
    void GetStockSource( UFC::AnsiString& FMO_STOCK_SOURCE, TExecutionReportMessage *ExecRptPtr );
public:
    TandemStockData( TExecutionReportMessage *ExecRptPtr );    
    ~TandemStockData( );
    virtual void GenerateTandemData( Int8* Data );
    virtual int  GetTandemDataLength( void ) { return 240; }
};    
//------------------------------------------------------------------------------    
class CTandemMessage
{
private:
    typedef std::map<UFC::AnsiString, UFC::AnsiString> FStringStringMap;
    static FStringStringMap                            FNewSymbolRootIndex;    
    static UFC::PCriticalSection                       FSequenceNoLocker;
    static Int32                                       FCurrentSequenceNo;    
    static UFC::PCriticalSection                       FSymbolRootLocker;
private:    
    nsOrderMessageDefine::MarketEnum      FExecRptMarket;
    nsOrderMessageDefine::OrderStatusEnum FExecRptOrderStatus;    
    UFC::AnsiString                       FExecID;
    UFC::AnsiString                       FOrderNo;
    TandemData*                           FData; 
    Int8                                  FTandemDataBuffer[ 512 ];        
public:
    static Int32 GetCurrentSequenceNo(bool DoLock = true);
    static void SetCurrentSequenceNo(Int32 SequenceNo, bool DoLock = true);
    static Int32 GetNextSequenceNo(bool DoLock = true);    
    static void LockSymbolRoot() {FSymbolRootLocker.Acquire();};
    static void UnlockSymbolRoot() {FSymbolRootLocker.Release();};
    static Int32 GetNewSymbolRootCount(bool DoLock = true);
    static UFC::AnsiString FindOldSymbolRootByNew(const UFC::AnsiString& NewSymbolRoot, bool DoLock = true);
    static bool InsertNewSymbolRootIndex(const UFC::AnsiString& NewSymbolRoot, const UFC::AnsiString& OldSymbolRoot, bool DoCheck = true, bool DoLock = true);
    static void DumpNewSymbolRootIndex(Int32 DebugLevel = 0, bool DoLock = true);
    static Int32 LoadNewSymbolRootIndex(const UFC::AnsiString& ConfigFileName, bool DoLock = true);     
public:
    CTandemMessage( TExecutionReportMessage *ExecRptPtr );
    ~CTandemMessage();
    nsOrderMessageDefine::MarketEnum      GetExecRptMarket( )     { return FExecRptMarket;};
    nsOrderMessageDefine::OrderStatusEnum GetExecRptOrderStatus() { return FExecRptOrderStatus;};
    UFC::AnsiString                       GetExecID()             { return FExecID;};
    UFC::AnsiString                       GetOrderNo()            { return FOrderNo;};    
    Int8*                                 GetTandemData()         { return FTandemDataBuffer; };    
    Int32                                 GetTandemDataLength();    
};  //CTandemMessage
//------------------------------------------------------------------------------    
}  //namespace capital
//------------------------------------------------------------------------------    
#endif	/* TENDEMOBJECT_H */

