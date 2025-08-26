/* 
 * File:   TWSEMegeRMClass.h
 * Author: Zhen Fan
 *
 * Created on 2021/01/11 10:10
 */

#ifndef TWSEMEGARMCLASS_H
#define TWSEMEGARMCLASS_H
#include "TWSERMClass.h"
#include "RMessages.h"
#include "TWSEMegaRMObjects.h"

namespace twserm
{
//------------------------------------------------------------------------------
class CMegaRiskManagerClass : public CTWSERiskManagerClass
{
private:
    UFC::PReadWriteLock FillRptExecLock;
    UFC::PHashedSet<UFC::AnsiString> FillRptExecKey;

    UFC::PReadWriteLock FRMExecRptNIDLock;
    UFC::PHashedSet<Int32> FRMExecRptNIDSet;
    
    long long FWhiteListFileSecond;
    
public:
    CMegaRiskManagerClass(bool IsRecoverMode);
    ~CMegaRiskManagerClass();
    
    //Implement RMObj
    virtual void Check(CheckData& Obj);
    virtual void Report(TExecReport* Msg, int PossDup) {}; //	PossDup: 0-NewExecution, 1-PossibleDuplicate, 2-SpeedyGenerate
    virtual void SetResultListener(CheckResultListener* Listener) {};
    
    void ParseRMDataFromUserData(const UFC::AnsiString& Key, UFC::AnsiString& WarrantNo, UFC::AnsiString& BrokerID);
    
    void RecoverRMConfirmData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, Int32 NetworkID, bool IsSynchronize);
    void RecoverRMFillData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, bool IsSynchronize);
    void ParseExecRtpLogStr(const UFC::AnsiString& ExecRptLogStr, bool IsSynchronize);
    void RecoverRMData();

    bool CheckBuyOrder(CRMOrderSummary *OrderPtr, CRMInvestorAccount *AccountPtr, CRMStockTradingData *StockDataPtr, UFC::AnsiString& CheckMessage);
    bool CheckSellOrder(CRMOrderSummary *OrderPtr, CRMInvestorAccount *AccountPtr, CRMStockTradingData *StockDataPtr, UFC::AnsiString& CheckMessage);
    bool CheckReplacePriceOrder(CRMOrderSummary *OrderPtr, CRMInvestorAccount *AccountPtr, CRMStockTradingData *StockDataPtr, CRMOrderSummary *OrderRecPtr, UFC::AnsiString& CheckMessage);
    
    //Implement CTWSERiskManagerClass
    bool CheckRiskCondition(TWSE::MessageHeader*& Msg, OrderRecord& OI, UFC::AnsiString& CheckMessage);
    void ProcessRiskRejectMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI);
    void ProcessRiskBuySellMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI);
    void ProcessRiskCancelReplaceMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI);
    
    bool IsFillRptExecExist(const UFC::AnsiString& ExecKey, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    void AddFillRptExec(const UFC::AnsiString& ExecKey, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);    
    void AddRiskMatchQty(TWSE::R3Body *R3MsgBodyPtr, const UFC::AnsiString& ExecKey);
    
    //Implement PThread
    virtual void Execute(void);
};  //CMegaRiskManagerClass
}  //namespace twserm
#endif /* TWSEMEGERMCLASS_H */
