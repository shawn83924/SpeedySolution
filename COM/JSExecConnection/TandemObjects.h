/* 
 * File:   TandemObjects.h
 * Author: zhenfan
 *
 * Created on 2012年6月22日, 下午 4:42
 */

#ifndef TANDEMOBJECTS_H
#define	TANDEMOBJECTS_H
#include <vector>
#include <map>
#include <queue>

#include "UFC.h"
#include "ConditionMutex.h"                        // UFC::PLUS
#include "OrderMessage/OrderMessageDefine.h"       // Lib
#include "OrderMessage/TExecutionReportMessage.h"  // Lib

namespace jihsun
{
const char *DescribeExecRptMarket(nsOrderMessageDefine::MarketEnum execRptMarket);
const char *DescribeExecRptOrderStatus(nsOrderMessageDefine::OrderStatusEnum execRptOrderStatus);

//------------------------------------------------------------------------------
class CUserData
{
private:
    static UFC::AnsiString FFieldDelimiter;
    static UFC::AnsiString FNameValueDelimiter;
    static UFC::AnsiString FLocalIP;
    
    UFC::AnsiString FInternetMark; //61.ODK-INTERNET-MARK       X(1)        354    
    UFC::AnsiString FSubAccountNo; //62.ODK-SUB-MOTHER-ACNO X(7)   355
    UFC::AnsiString FAEID;         //58.ODK-AE-ID           9(4)   346
    UFC::AnsiString FAp2InKind;    //67.ODK-AP2-IN-KIND     X(01)  403
    UFC::AnsiString FSourceIP;     //66.ODK-SRC-IP          X(20)  420
    Int32           FOpenQty;      //68.ODK-OPEN-QTY            9(4)        418
    double          FPremium;      //69.ODK-TD-PP-PREMIUM       S9(6)V9(2)  422
    double          FFee;          //70.ODK-TD-PP-FEE           S9(6)V9(2)  431
    void ParseFieldValue(const UFC::AnsiString& csvStr);
public:
    CUserData();
    CUserData(const UFC::AnsiString& csvStr);
    
    UFC::AnsiString GetSubAccountNo() {return FSubAccountNo;};
    UFC::AnsiString GetAEID() {return FAEID;};
    UFC::AnsiString GetAp2InKind() {return FAp2InKind;};
    UFC::AnsiString GetSourceIP() {return FSourceIP;};
    char            GetInternetMark() { return FInternetMark[0];}
    Int32           GetOpenQty() { return FOpenQty; }
    double          GetPremium() { return FPremium; }
    double          GetFee()     { return FFee; }
};  //CUserData

//------------------------------------------------------------------------------    
class CTandemMessage
{
private:
    static UFC::PCriticalSection FSequenceNoLocker;
    static Int32                 FCurrentSequenceNo;
    
    static UFC::PCriticalSection                       FSymbolRootLocker;
    typedef std::map<UFC::AnsiString, UFC::AnsiString> FStringStringMap;
    static FStringStringMap                            FNewSymbolRootIndex;
    
    static const int FTandemDataLength = 480;

    nsOrderMessageDefine::MarketEnum      FExecRptMarket;
    nsOrderMessageDefine::OrderStatusEnum FExecRptOrderStatus;
    UFC::AnsiString                       FExecID;
    
    UFC::AnsiString FTradeDate;            // 1.ODK-DATE                9(8)          0
    Int32           FSequenceNo;           // 2.ODK-MSG-SEQNUM          9(10)         8
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
    char            FInternetMark;         //61.ODK-INTERNET-MARK       X(1)        354
    UFC::AnsiString FDCBCode;              //62.ODK-DCBCODE             X(8)        355
    UFC::AnsiString FDealer;               //63.ODK-DEALER              X(7)        363
    Int64           FOdrPrice;             //64.ODK-ODR-PRRICE          S9(05)V9(6) 370
    UFC::AnsiString FClearMemberID;        //65.ODK-CLEAR-MEMBER-ID     X(04)       382
    UFC::AnsiString FIPNo;                 //66.ODK-IP-NO               X(30)       386
    UFC::AnsiString FAPKind;               //67.ODK-AP-KIND             X(2)        416
    Int32           FOpenQty;              //68.ODK-OPEN-QTY            9(4)        418
    Int32           FPremium;              //69.ODK-TD-PP-PREMIUM       S9(6)V9(2)  422
    Int32           FFee;                  //70.ODK-TD-PP-FEE           S9(6)V9(2)  431
    char            FOrderKind;            //71.ODK-ORDER-KIND          X(1)        440 
    UFC::AnsiString FFiller;               //72.FILLER                  X(39)       441
                                           // Total                                 480
    Int8 FTandemDataBuffer[FTandemDataLength + 1];
    
    void InitField();
public:
    static Int32 GetCurrentSequenceNo(bool DoLock = true);
    static void  SetCurrentSequenceNo(Int32 SequenceNo, bool DoLock = true);
    static Int32 GetNextSequenceNo(bool DoLock = true);
    
    static void LockSymbolRoot() {FSymbolRootLocker.Acquire();};
    static void UnlockSymbolRoot() {FSymbolRootLocker.Release();};
    static Int32 GetNewSymbolRootCount(bool DoLock = true);
    static UFC::AnsiString FindOldSymbolRootByNew(const UFC::AnsiString& NewSymbolRoot, bool DoLock = true);
    static bool InsertNewSymbolRootIndex(const UFC::AnsiString& NewSymbolRoot, const UFC::AnsiString& OldSymbolRoot, bool DoCheck = true, bool DoLock = true);
    static void DumpNewSymbolRootIndex(Int32 DebugLevel = 0, bool DoLock = true);
    static Int32 LoadNewSymbolRootIndex(const UFC::AnsiString& ConfigFileName, bool DoLock = true); 

    static Int32 GetTandemDataLength() {return FTandemDataLength;};
    
    CTandemMessage();
    CTandemMessage(TExecutionReportMessage *ExecRptPtr);

    nsOrderMessageDefine::MarketEnum GetExecRptMarket() {return FExecRptMarket;};
    nsOrderMessageDefine::OrderStatusEnum GetExecRptOrderStatus() {return FExecRptOrderStatus;};
    UFC::AnsiString GetExecID() {return FExecID;};
    
    Int32 GetSequenceNo() {return FSequenceNo;};
    UFC::AnsiString GetReceiveTime() {return FReceiveTime;};
    UFC::AnsiString GetTransactTime() {return FTransactTime;};
    
    char TranslateExecType(TExecutionReportMessage *ExecRptPtr,
                           nsOrderMessageDefine::ExecTypeEnum rptExecType, 
                           nsOrderMessageDefine::TMPExecTypeEnum rptTMPExecType,
                           nsOrderMessageDefine::CxlRejResponseToEnum rptRejResponseTo);
    char TranslateSide(nsOrderMessageDefine::SideEnum rptSide, char& BuySellCode);
    char TranslateOrderType(nsOrderMessageDefine::OrderTypeEnum rptOrderType);
    char TranslateTimeInForce(nsOrderMessageDefine::TimeInForceEnum rptTimeInForce);
    char TranslatePositionEffect(nsOrderMessageDefine::PositionEffectEnum rptPositionEffect);
    UFC::AnsiString ConvertToCommodityID(const UFC::AnsiString& newSymbol, nsOrderMessageDefine::MarketEnum rptMarket, char callPutCode);
    Int64 DoubleToInt64(double DoubleVal, int Digi);
    void SetFieldsValue(TExecutionReportMessage *ExecRptPtr);
    void GenerateTandemData();
    Int8 *GetTandemData() {return FTandemDataBuffer;};
};  //CTandemMessage

//------------------------------------------------------------------------------    
class CTandemTelegraph
{
private:
    static Int32 FMaxTandemReportCount;
    
    UFC::AnsiString FCommand;
    Int32           FNumber;
    UFC::AnsiString FData;
    Int16           FTelegraphLength;
    
    Int32 FTelegraphBufferSize;
    char  *FTelegraphBuffer;
    
    void ParseTelegraph(Int16 TelegraphLength, const UFC::AnsiString& TelegraphBody );
    void BuildTelegraph(const UFC::AnsiString& Command, Int32 Number, const UFC::AnsiString& Data = "");
public:
    static Int32 GetMaxTandemReportCount() {return  FMaxTandemReportCount;};
    static void SetMaxTandemReportCount(Int32 MaxTandemReportCount) {FMaxTandemReportCount = MaxTandemReportCount;};
    
    CTandemTelegraph(Int16 TelegraphLength,  const UFC::AnsiString& TelegraphBody);
    CTandemTelegraph(const UFC::AnsiString& Command, Int32 Number, const UFC::AnsiString& Data);
    ~CTandemTelegraph();
    
    UFC::AnsiString GetCommand() {return FCommand;};
    Int32 GetNumber() {return FNumber;};
    UFC::AnsiString GetData() {return FData;};
    Int16 GetTelegraphLength() {return FTelegraphLength;};
    char *GetTelegraphBuffer() {return FTelegraphBuffer;};

};  //CTandemTelegraph

//------------------------------------------------------------------------------    
class CDataExchangePool
{
private:
    UFC::PLUS::CConditionMutex  FTandemMsgQueueLocker;
    std::queue<CTandemMessage*> FTandemMsgQueue;
    std::queue<UFC::AnsiString*> FAlertMsgQueue;
    std::queue<UFC::AnsiString*> FNoticeMsgQueue;

    UFC::PCriticalSection FTandemMsgArrayLocker;
    std::vector<CTandemMessage*> FTandemMsgArray;
    
    UFC::PCriticalSection FTandemMsgExecIDLocker;
    std::map<UFC::AnsiString, CTandemMessage*> FTandemMsgExecIDIndex;
    
public:
    CDataExchangePool();
    ~CDataExchangePool();
    
    void LockTandemMsgQueue() {FTandemMsgQueueLocker.Lock();};
    void UnlockTandemMsgQueue() {FTandemMsgQueueLocker.Unlock();};
    Int32 WaitTandemMsgQueue() {return FTandemMsgQueueLocker.Wait();};
    Int32 WaitTandemMsgQueue(Int32 WairingMilliSecond) {return FTandemMsgQueueLocker.Wait(WairingMilliSecond);};
    void SignalTandemMsgWaiter() {FTandemMsgQueueLocker.Signal();};

    Int32 GetWaitingTandemMsgCount(bool DoLock = true);
    CTandemMessage *PopTandemMsg(bool DoLock = true);
    void PushTandemMsg(CTandemMessage *TandemMsgPtr, bool DoLock = true);
    void ClearTandemMsgQueue(bool DoLock = true);
    
    Int32 GetWaitingAlertMsgCount(bool DoLock = true);
    UFC::AnsiString *PopAlertMsg(bool DoLock = true);
    void PushAlertMsg(UFC::AnsiString *AlertMsgPtr, bool DoLock = true);
    void ClearAlertMsgQueue(bool DoLock = true);
    
    Int32 GetWaitingNoticeMsgCount(bool DoLock = true);
    UFC::AnsiString *PopNoticeMsg(bool DoLock = true);
    void PushNoticeMsg(UFC::AnsiString *AlertMsgPtr, bool DoLock = true);
    void ClearNoticeMsgQueue(bool DoLock = true);
    
    void LockTandemMsgArray() {FTandemMsgArrayLocker.Acquire();};
    void UnlockTandemMsgArray() {FTandemMsgArrayLocker.Release();};
    Int32 GetTandemMsgArrayCount(bool DoLock = true);
    bool InsertTandemMsgArray(CTandemMessage *TandemMsgPtr, bool DoLock = true);
    CTandemMessage *GetTandemMsgFromArray(Int32 MsgIndex, bool DoLock = true);
    
    void LockTandemMsgExecIDIndex() {FTandemMsgExecIDLocker.Acquire();};
    void UnlockTandemMsgExecIDIndex() {FTandemMsgExecIDLocker.Release();};
    int GetTandemMsgExecIDCount(bool DoLock = true);
    CTandemMessage *FindTandemMsgByExecID(const UFC::AnsiString& ExecID, bool DoLock = true);
    bool IsTandemMsgExecIDExist(const UFC::AnsiString& ExecID, bool DoLock = true);
    bool InsertTandemMsgExecIDIndex(const UFC::AnsiString& ExecID, CTandemMessage *TandemMsgPtr, bool DoCheck = true, bool DoLock = true);
    bool InsertTandemMsgExecIDIndex(CTandemMessage *TandemMsgPtr, bool DoCheck = true, bool DoLock = true);
    
};  //CDataExchangePool()
}  //namespace jihsun

#endif	/* TANDEMOBJECTS_H */

