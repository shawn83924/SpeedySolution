/* 
 * File:   TWSERMClass.cpp
 * Author: Zhen Fan
 *
 * Created on 2021/01/06 15:00
 */
#include "TWSERMClass.h"
namespace twserm
{
//--------------------------- CTWSERiskManagerClass ----------------------------
bool              CTWSERiskManagerClass::FIsTWSE                = true;
UFC::AnsiString   CTWSERiskManagerClass::FRiskManagerID         = "";
UFC::AnsiString   CTWSERiskManagerClass::FConfigureFilePathName = "";
UFC::AnsiString   CTWSERiskManagerClass::FMarketName            = "TSE";
UFC::BufferedLog* CTWSERiskManagerClass::FLogPtr                = 0;

//------------------------------------------------------------------------------
CTWSERiskManagerClass::CTWSERiskManagerClass(bool IsRecoverMode)
:RMObj()
,PThread(NULL, TRUE)  //Free On Terminate
,FIsRecoverMode(IsRecoverMode)
{
    if (FIsTWSE)
        FMarketName = "TSE";
    else
        FMarketName = "OTC";
}  //CTWSERiskManagerClass::CTWSERiskManagerClass()
//------------------------------------------------------------------------------
CTWSERiskManagerClass::~CTWSERiskManagerClass()
{    
}  //TWSERiskManagerClass::~TWSERiskManagerClass()
//------------------------------------------------------------------------------
void CTWSERiskManagerClass::Check(CheckData& Obj)
{
    
}  //CTWSERiskManagerClass::Check()
//------------------------------------------------------------------------------
double CTWSERiskManagerClass::Parse95v9999PriceStr(const UFC::AnsiString& PriceStr)
{
    double price = 0.0;
    if (PriceStr.Length() != 9) return price;
    UFC::AnsiString intPartStr = PriceStr.SubString(0, 5);
    UFC::AnsiString fractionPartStr = PriceStr.SubString(5, 4);
    price = intPartStr.ToDouble() + fractionPartStr.ToDouble() / 10000.0;
    return price;
}  //CTWSERiskManagerClass::Parse95v9999PriceStr(()
//------------------------------------------------------------------------------
bool CTWSERiskManagerClass::CheckRiskCondition(TWSE::MessageHeader*& Msg, OrderRecord& OI, UFC::AnsiString& CheckMessage)
{
    return true;
}  //CTWSERiskManagerClass::CheckRiskCondition()
//------------------------------------------------------------------------------
void CTWSERiskManagerClass::ProcessRiskRejectMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI)
{
}  //CTWSERiskManagerClass::ProcessRiskRejectMessage()
//------------------------------------------------------------------------------
void CTWSERiskManagerClass::ProcessRiskBuySellMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI)
{
}  //CTWSERiskManagerClass::ProcessRiskBuySellMessage()
//------------------------------------------------------------------------------
void CTWSERiskManagerClass::ProcessRiskCancelReplaceMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI)
{
}  //CTWSERiskManagerClass::ProcessRiskCancelReplaceMessage()
//------------------------------------------------------------------------------
bool CTWSERiskManagerClass::IsFillRptExecExist(const UFC::AnsiString& ExecKey, UFC::RWLockTypeEnum LockType)
{
    return false;
}  //CTWSERiskManagerClass::IsFillRptExecExist()
//------------------------------------------------------------------------------
void CTWSERiskManagerClass::AddFillRptExec(const UFC::AnsiString& ExecKey, UFC::RWLockTypeEnum LockType)
{
}  //CTWSERiskManagerClass::AddFillRptExec()
//------------------------------------------------------------------------------
void CTWSERiskManagerClass::AddRiskMatchQty(TWSE::R3Body *R3MsgBodyPtr, const UFC::AnsiString& ExecKey)
{
}  //CTWSERiskManagerClass::AddRiskMatchQty()
//------------------------------------------------------------------------------
void CTWSERiskManagerClass::Execute(void)
{    
}  //CTWSERiskManagerClass::Execute()
}  //namespace twserm
