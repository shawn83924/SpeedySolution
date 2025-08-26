/* 
 * File:   TWSERMClass.h
 * Author: Zhen Fan
 *
 * Created on 2021/01/06 15:00
 */

#ifndef TWSERMCLASS_H
#define TWSERMCLASS_H

#include "UFC.h"            //UFC
#include "../RiskManager/RMInterface.h"    //RiskManager
#include "Utility.h"        //Exchange Utility
#include "MessageHeader.h"
#include "RMessages.h"

namespace twserm
{
//------------------------------------------------------------------------------
class CTWSERiskManagerClass : public RMObj, public UFC::PThread
{
protected:
    static bool              FIsTWSE;
    static UFC::AnsiString   FRiskManagerID;
    static UFC::AnsiString   FConfigureFilePathName;
    static UFC::AnsiString   FMarketName;
    static UFC::BufferedLog* FLogPtr;

public:
    static bool IsTWSE() {return FIsTWSE;};
    static void SetTWSE(bool IsTWSE) {FIsTWSE = IsTWSE;};
    
    static UFC::AnsiString GetRiskManagerID() {return FRiskManagerID;};
    static void SetRiskManagerID(const UFC::AnsiString& NewValue) {FRiskManagerID = NewValue;};
    
    static UFC::AnsiString GetConfigureFilePathName() {return FConfigureFilePathName;};
    static void SetConfigureFilePathName(const UFC::AnsiString& NewValue) {FConfigureFilePathName = NewValue;};

    static UFC::AnsiString GetMarketName() {return FMarketName;};
    
    static UFC::BufferedLog* GetLogObject() {return FLogPtr;};
    static void SetLogObject(UFC::BufferedLog* LogObjPtr) {FLogPtr = LogObjPtr;};
    
protected:
    bool FIsRecoverMode;
    
public:
    CTWSERiskManagerClass(bool IsRecoverMode);
    ~CTWSERiskManagerClass();
    
    //Implement RMObj
    virtual void Check(CheckData& Obj);
    virtual void Report(TExecReport* Msg, int PossDup) {}; //	PossDup: 0-NewExecution, 1-PossibleDuplicate, 2-SpeedyGenerate
    virtual BOOL QueryBalancePosition( const UFC::AnsiString& BrokerID, const UFC::AnsiString& AccountNo, UFC::AnsiString& Result ) {return FALSE;};
    virtual void SetResultListener(CheckResultListener* Listener) {};

    double Parse95v9999PriceStr(const UFC::AnsiString& PriceStr);
    
    virtual bool CheckRiskCondition(TWSE::MessageHeader*& Msg, OrderRecord& OI, UFC::AnsiString& CheckMessage);
    virtual void ProcessRiskRejectMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI);
    virtual void ProcessRiskBuySellMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI);
    virtual void ProcessRiskCancelReplaceMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI);
    
    virtual bool IsFillRptExecExist(const UFC::AnsiString& ExecKey, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    virtual void AddFillRptExec(const UFC::AnsiString& ExecKey, UFC::RWLockTypeEnum LockType = UFC::rwNotLock);    
    virtual void AddRiskMatchQty(TWSE::R3Body *R3MsgBodyPtr, const UFC::AnsiString& ExecKey);

    virtual void ParseExecRtpLogStr(const UFC::AnsiString& ExecRptLogStr, bool IsSynchronize) {};
    
    //Implement PThread
    virtual void Execute(void);
};  //CTWSERiskManagerClass
}  //namespace twserm
#endif /* TWSERMCLASS_H */

