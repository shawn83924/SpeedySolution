/* 
 * File:   TWSEEntrustRMClass.h
 * Author: Zhen Fan
 *
 * Created on 2021/04/03 11:21
 */

#ifndef TWSEENTRUSTRMCLASS_H
#define TWSEENTRUSTRMCLASS_H
#include "TWSERMClass.h"
#include "RMessages.h"
#include "TWSEMegaRMObjects.h"
#include "TWSEEntrustRMObjects.h"

namespace twserm
{
//------------------------------------------------------------------------------
class CEntrustRiskManagerClass : public CTWSERiskManagerClass
{
private:
    UFC::PReadWriteLock FillRptExecLock;
    UFC::PHashedSet<UFC::AnsiString> FillRptExecKey;

    UFC::PReadWriteLock FRMExecRptNIDLock;
    UFC::PHashedSet<Int32> FRMExecRptNIDSet;
    
    long long FWhiteListFileSecond;
    
public:
    CEntrustRiskManagerClass(bool IsRecoverMode);
    ~CEntrustRiskManagerClass();
    
    //Implement RMObj
    virtual void Check(CheckData& Obj);
    virtual void Report(TExecReport* Msg, int PossDup) {}; //	PossDup: 0-NewExecution, 1-PossibleDuplicate, 2-SpeedyGenerate
    virtual BOOL QueryBalancePosition( const UFC::AnsiString& BrokerID, const UFC::AnsiString& AccountNo, UFC::AnsiString& Result ) {return FALSE;};
    virtual void SetResultListener(CheckResultListener* Listener) {};
    
    void ParseRMDataFromUserData(const UFC::AnsiString& Key, UFC::AnsiString& WarrantNo, UFC::AnsiString& BrokerID);
    
    void RecoverRMConfirmData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, Int32 NetworkID, bool IsSynchronize);
    void RecoverRMFillData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerID, bool IsSynchronize);
    void ParseExecRtpLogStr(const UFC::AnsiString& ExecRptLogStr, bool IsSynchronize);
    void RecoverRMData();

    bool CheckBuyOrder(CRMOrderSummary *OrderPtr, CRMEntrustInvestorAccount *AccountPtr, CRMInvestorSubAccount* SubAccountPtr, CRMEntrustStockTradingData *StockDataPtr, UFC::AnsiString& CheckMessage);
    bool CheckSellOrder(CRMOrderSummary *OrderPtr, CRMEntrustInvestorAccount *AccountPtr, CRMInvestorSubAccount* SubAccountPtr, CRMEntrustStockTradingData *StockDataPtr, UFC::AnsiString& CheckMessage);
    bool CheckReplacePriceOrder(CRMOrderSummary *OrderPtr, CRMEntrustInvestorAccount *AccountPtr, CRMEntrustStockTradingData *StockDataPtr, CRMOrderSummary *OrderRecPtr, UFC::AnsiString& CheckMessage);
    
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
};  //CEntrustRiskManagerClass
}  //namespace twserm
#endif /* TWSEENTRUSTRMCLASS_H */
