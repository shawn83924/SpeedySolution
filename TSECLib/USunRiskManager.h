/* 
 * File:   USunRiskManager.h
 * Author: Zhen Fan
 *
 * Created on 2016/02/03, 10:54
 */

#ifndef USUNRISKMANAGER_H
#define USUNRISKMANAGER_H

#include "RiskManagerAbstract.h"

namespace rm
{
//------------------------------------------------------------------------------
class CRMReservedStock : public CRMBasicObject
{
protected:
    UFC::AnsiString FStockNo;        //股票代號
    int             FAvailableQty;   //可動用股數, 借券限額股數
    int             FReservedQty;    //庫存股數
    double          FReservedCost;   //庫存成本
    UFC::AnsiString FSettleDate;     //結帳日期
    UFC::AnsiString FModifyDate;     //異動日期
    UFC::AnsiString FModifyTime;     //異動時間
    UFC::AnsiString FTraderID;       //操盤人代號
    bool            FIsSellShort;    //是否為借券

    void ParseControlFileRecord(const UFC::AnsiString& RecordStr);
    
public:
    CRMReservedStock(const UFC::AnsiString& StockNo, const UFC::AnsiString& TraderID, bool IsSellShort = false);
    CRMReservedStock(const UFC::AnsiString& RecordStr, bool IsSellShort = false);
            
    UFC::AnsiString GetStockNo(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FStockNo, &FLock, RWLockType);};
    int GetAvailableQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FAvailableQty, &FLock, RWLockType);};
    int GetReservedQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FReservedQty, &FLock, RWLockType);};
    double GetReservedCost(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FReservedCost, &FLock, RWLockType);};
    UFC::AnsiString GetSettleDate(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FSettleDate, &FLock, RWLockType);};
    UFC::AnsiString GetModifyDate(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FModifyDate, &FLock, RWLockType);};
    UFC::AnsiString GetModifyTime(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FModifyTime, &FLock, RWLockType);};
    UFC::AnsiString GetTraderID(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FTraderID, &FLock, RWLockType);};
    bool IsSellShort(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FIsSellShort, &FLock, RWLockType);};

    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMReservedStock

//------------------------------------------------------------------------------
class CRMBrokerTraderQuota : public CRMBasicObject
{
protected:
    UFC::AnsiString FTraderID;          //操盤人代號
    UFC::AnsiString FTraderName;        //操盤人姓名
    double          FAuthorizedAmount;  //授權額度
    UFC::AnsiString FInvestorAccount;   //交易所交易帳號
    UFC::AnsiString FTransferDate;      //轉檔日期
    UFC::AnsiString FTransferTime;      //轉檔時間
    
    void ParseControlFileRecord(const UFC::AnsiString& RecordStr);
    
public:
    CRMBrokerTraderQuota(const UFC::AnsiString& RecordStr);

    UFC::AnsiString GetTraderID(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FTraderID, &FLock, RWLockType);};
    UFC::AnsiString GetTraderName(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FTraderName, &FLock, RWLockType);};
    double GetAuthorizedAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FAuthorizedAmount, &FLock, RWLockType);};
    UFC::AnsiString GetInvestorAccount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FInvestorAccount, &FLock, RWLockType);};
    UFC::AnsiString GetTransferDate(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FTransferDate, &FLock, RWLockType);};
    UFC::AnsiString GetTransferTime(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FTransferTime, &FLock, RWLockType);};
    
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMBrokerTraderQuota

//------------------------------------------------------------------------------
class CRMTraderStockCondition : public CRMBasicObject
{
protected:
    UFC::AnsiString FStockNo;                //股票代號
    int             FAvailableQty;           //可動用股數
    int             FReservedQty;            //庫存股數
    double          FReservedCost;           //庫存成本
    
    int             FSellOrderQty;           //委託賣出股數
    double          FSellOrderAmount;        //委託賣出金額
    int             FSellActiveQty;          //委託賣出未成交股數
    double          FSellActiveAmount;       //委託賣出未成交金額
    int             FSellMatchQty;           //賣出成交股數
    double          FSellMatchAmount;        //賣出成交金額
    
    int             FBuyOrderQty;            //委託買進股數
    double          FBuyOrderAmount;         //委託買進金額
    int             FBuyActiveQty;           //委託買進未成交股數
    double          FBuyActiveAmount;        //委託買進未成交金額
    int             FBuyMatchQty;            //買進成交股數
    double          FBuyMatchAmount;         //買進成交金額
    
    int             FAvailableSellShortQty;  //借券限額股數
    int             FReservedSellShortQty;   //借券庫存股數
    double          FReservedSellShortCost;  //借券庫存成本
    
    int             FSellShortOrderQty;      //借券委託賣出股數
    double          FSellShortOrderAmount;   //借券委託賣出金額
    int             FSellShortActiveQty;     //借券委託賣出未成交股數
    double          FSellShortActiveAmount;  //借券委託賣出未成交金額
    int             FSellShortMatchQty;      //借券賣出成交股數
    double          FSellShortMatchAmount;   //借券賣出成交金額
    
    int             FBuyShortOrderQty;       //借券委託買進股數
    double          FBuyShortOrderAmount;    //借券委託買進金額
    int             FBuyShortActiveQty;      //借券委託買進未成交股數
    double          FBuyShortActiveAmount;   //借券委託買進未成交金額
    int             FBuyShortMatchQty;       //借券買進成交股數
    double          FBuyShortMatchAmount;    //借券買進成交金額
    
    UFC::AnsiString FTraderID;               //操盤人代號
    
public:
    CRMTraderStockCondition(const UFC::AnsiString& StockNo, const UFC::AnsiString& TraderID);

    UFC::AnsiString GetStockNo(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FStockNo, &FLock, RWLockType);};

    int GetAvailableQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FAvailableQty, &FLock, RWLockType);};
    void SetAvailableQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FAvailableQty, &FLock, RWLockType);};
    
    int GetReservedQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FReservedQty, &FLock, RWLockType);};
    void SetReservedQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FReservedQty, &FLock, RWLockType);};
    
    double GetReservedCost(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FReservedCost, &FLock, RWLockType);};
    void SetReservedCost(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FReservedCost, &FLock, RWLockType);};
    
    int GetSellOrderQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FSellOrderQty, &FLock, RWLockType);};
    void SetSellOrderQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FSellOrderQty, &FLock, RWLockType);};
    bool IncreaseSellOrderQty(int IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellOrderQty, cstCheck, RWLockType);};
    
    double GetSellOrderAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FSellOrderAmount, &FLock, RWLockType);};
    void SetSellOrderAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FSellOrderAmount, &FLock, RWLockType);};
    bool IncreaseSellOrderAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellOrderAmount, cstCheck, RWLockType);};
    
    int GetSellActiveQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FSellActiveQty, &FLock, RWLockType);};
    void SetSellActiveQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FSellActiveQty, &FLock, RWLockType);};
    bool IncreaseSellActiveQty(int IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellActiveQty, cstCheck, RWLockType);};
    
    double GetSellActiveAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FSellActiveAmount, &FLock, RWLockType);};
    void SetSellActiveAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FSellActiveAmount, &FLock, RWLockType);};
    bool IncreaseSellActiveAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellActiveAmount, cstCheck, RWLockType);};

    int GetSellMatchQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FSellMatchQty, &FLock, RWLockType);};
    void SetSellMatchQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FSellMatchQty, &FLock, RWLockType);};
    bool IncreaseSellMatchQty(int IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellMatchQty, cstCheck, RWLockType);};
    
    double GetSellMatchAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FSellMatchAmount, &FLock, RWLockType);};
    void SetSellMatchAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FSellMatchAmount, &FLock, RWLockType);};
    bool IncreaseSellMatchAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellMatchAmount, cstCheck, RWLockType);};
    
    int GetBuyOrderQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FBuyOrderQty, &FLock, RWLockType);};
    void SetBuyOrderQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FBuyOrderQty, &FLock, RWLockType);};
    bool IncreaseBuyOrderQty(int IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyOrderQty, cstCheck, RWLockType);};
    
    double GetBuyOrderAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FBuyOrderAmount, &FLock, RWLockType);};
    void SetBuyOrderAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FBuyOrderAmount, &FLock, RWLockType);};
    bool IncreaseBuyOrderAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyOrderAmount, cstCheck, RWLockType);};
    
    int GetBuyActiveQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FBuyActiveQty, &FLock, RWLockType);};
    void SetBuyActiveQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FBuyActiveQty, &FLock, RWLockType);};
    bool IncreaseBuyActiveQty(int IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyActiveQty, cstCheck, RWLockType);};
    
    double GetBuyActiveAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FBuyActiveAmount, &FLock, RWLockType);};
    void SetBuyActiveAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FBuyActiveAmount, &FLock, RWLockType);};
    bool IncreaseBuyActiveAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyActiveAmount, cstCheck, RWLockType);};
    
    int GetBuyMatchQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FBuyMatchQty, &FLock, RWLockType);};
    void SetBuyMatchQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FBuyMatchQty, &FLock, RWLockType);};
    bool IncreaseBuyMatchQty(int IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyMatchQty, cstCheck, RWLockType);};
        
    double GetBuyMatchAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FBuyMatchAmount, &FLock, RWLockType);};
    void SetBuyMatchAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FBuyMatchAmount, &FLock, RWLockType);};
    bool IncreaseBuyMatchAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyMatchAmount, cstCheck, RWLockType);};
    
    int GetAvailableSellShortQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FAvailableSellShortQty, &FLock, RWLockType);};
    void SetAvailableSellShortQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FAvailableSellShortQty, &FLock, RWLockType);};
    
    int GetReservedSellShortQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FReservedSellShortQty, &FLock, RWLockType);};
    void SetReservedSellShortQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FReservedSellShortQty, &FLock, RWLockType);};
    
    double GetReservedSellShortCost(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FReservedSellShortCost, &FLock, RWLockType);};
    void SetReservedSellShortCost(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FReservedSellShortCost, &FLock, RWLockType);};
    
    int GetSellShortOrderQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FSellShortOrderQty, &FLock, RWLockType);};
    void SetSellShortOrderQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FSellShortOrderQty, &FLock, RWLockType);};
    bool IncreaseSellShortOrderQty(int IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellShortOrderQty, cstCheck, RWLockType);};
    
    double GetSellShortOrderAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FSellShortOrderAmount, &FLock, RWLockType);};
    void SetSellShortOrderAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FSellShortOrderAmount, &FLock, RWLockType);};
    bool IncreaseSellShortOrderAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellShortOrderAmount, cstCheck, RWLockType);};

    int GetSellShortActiveQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FSellShortActiveQty, &FLock, RWLockType);};
    void SetSellShortActiveQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FSellShortActiveQty, &FLock, RWLockType);};
    bool IncreaseSellShortActiveQty(int IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellShortActiveQty, cstCheck, RWLockType);};
    
    double GetSellShortActiveAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FSellShortActiveAmount, &FLock, RWLockType);};
    void SetSellShortActiveAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FSellShortActiveAmount, &FLock, RWLockType);};
    bool IncreaseSellShortActiveAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellShortActiveAmount, cstCheck, RWLockType);};
        
    int GetSellShortMatchQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FSellShortMatchQty, &FLock, RWLockType);};
    void SetSellShortMatchQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FSellShortMatchQty, &FLock, RWLockType);};
    bool IncreaseSellShortMatchQty(int IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellShortMatchQty, cstCheck, RWLockType);};
    
    double GetSellShortMatchAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FSellShortMatchAmount, &FLock, RWLockType);};
    void SetSellShortMatchAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FSellShortMatchAmount, &FLock, RWLockType);};
    bool IncreaseSellShortMatchAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellShortMatchAmount, cstCheck, RWLockType);};
    
    int GetBuyShortOrderQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FBuyShortOrderQty, &FLock, RWLockType);};
    void SetBuyShortOrderQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FBuyShortOrderQty, &FLock, RWLockType);};
    bool IncreaseBuyShortOrderQty(int IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyShortOrderQty, cstCheck, RWLockType);};
    
    double GetBuyShortOrderAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FBuyShortOrderAmount, &FLock, RWLockType);};
    void SetBuyShortOrderAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FBuyShortOrderAmount, &FLock, RWLockType);};
    bool IncreaseBuyShortOrderAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyShortOrderAmount, cstCheck, RWLockType);};

    int GetBuyShortActiveQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FBuyShortActiveQty, &FLock, RWLockType);};
    void SetBuyShortActiveQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FBuyShortActiveQty, &FLock, RWLockType);};
    bool IncreaseBuyShortActiveQty(int IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyShortActiveQty, cstCheck, RWLockType);};
    
    double GetBuyShortActiveAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FBuyShortActiveAmount, &FLock, RWLockType);};
    void SetBuyShortActiveAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FBuyShortActiveAmount, &FLock, RWLockType);};
    bool IncreaseBuyShortActiveAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyShortActiveAmount, cstCheck, RWLockType);};
            
    int GetBuyShortMatchQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<int>(FBuyShortMatchQty, &FLock, RWLockType);};
    void SetBuyShortMatchQty(int NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<int>(NewValue, FBuyShortMatchQty, &FLock, RWLockType);};
    bool IncreaseBuyShortMatchQty(int IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyShortMatchQty, cstCheck, RWLockType);};
    
    double GetBuyShortMatchAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FBuyShortMatchAmount, &FLock, RWLockType);};
    void SetBuyShortMatchAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FBuyShortMatchAmount, &FLock, RWLockType);};
    bool IncreaseBuyShortMatchAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyShortMatchAmount, cstCheck, RWLockType);};

    UFC::AnsiString GetTraderID(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FTraderID, &FLock, RWLockType);};

    void AddNewOrderData(int OrderQty, double OrderPrice, char Side, char OrderType, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    void AddMatchOrderData(int MatchQty, double LastPrice, double OrderPrice, char Side, char OrderType, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    void CancelOrderData(int CancelQty, double OrderPrice, char Side, char OrderType, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
            
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMTraderStockCondition

//------------------------------------------------------------------------------
class CRMBrokerTrader : public CRMBasicObject
{
protected:
    UFC::AnsiString FTraderID;          //操盤人代號
    UFC::AnsiString FTraderName;        //操盤人姓名
    bool            FNeedCheckQuota;    //需要檢查額度
    
    double FAuthorizedAmount;  //授權額度
    
    double FReservedCost;     //庫存成本    
    double FSellOrderAmount;  //委託賣出金額
    double FSellActiveAmount; //委託賣出未成交金額
    double FSellMatchAmount;  //賣出金額
    double FBuyOrderAmount;   //委託買進金額
    double FBuyActiveAmount;  //委託買進未成交金額
    double FBuyMatchAmount;   //買進金額
    
    double FReservedSellShortCost;  //借券庫存成本    
    double FSellShortOrderAmount;   //借券委託賣出金額
    double FSellShortActiveAmount;  //借券委託賣出未成交金額
    double FSellShortMatchAmount;   //借券賣出金額
    double FBuyShortOrderAmount;    //借券委託買進金額
    double FBuyShortActiveAmount;   //借券委託買進未成交金額
    double FBuyShortMatchAmount;    //借券買進金額
    
    UFC::PHashMap<UFC::AnsiString, CRMReservedStock*> FReservedStockNoIndex;
    UFC::PHashMap<UFC::AnsiString, CRMReservedStock*> FReservedSellShortStockNoIndex;
    UFC::PHashMap<UFC::AnsiString, CRMBrokerTraderQuota*> FAccQuotaNoIndex;
    UFC::PHashMap<UFC::AnsiString, CRMTraderStockCondition*> FConditionStockNoIndex;
    
public:
    CRMBrokerTrader(const UFC::AnsiString& TraderID, const UFC::AnsiString& TraderName = "", bool NeedCheckQuota = true);
    ~CRMBrokerTrader();

    UFC::AnsiString GetTraderID(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FTraderID, &FLock, RWLockType);};

    UFC::AnsiString GetTraderName(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<UFC::AnsiString>(FTraderName, &FLock, RWLockType);};
    void SetTraderName(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<UFC::AnsiString>(NewValue, FTraderName, &FLock, RWLockType);};

    bool NeedCheckQuota(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<bool>(FNeedCheckQuota, &FLock, RWLockType);};
    void SetNeedCheckQuota(bool NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<bool>(NewValue, FNeedCheckQuota, &FLock, RWLockType);};

    double GetAuthorizedAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FAuthorizedAmount, &FLock, RWLockType);};
    void SetAuthorizedAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FAuthorizedAmount, &FLock, RWLockType);};

    double GetReservedCost(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FReservedCost, &FLock, RWLockType);};
    void SetReservedCost(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FReservedCost, &FLock, RWLockType);};
    bool IncreaseReservedCost(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FReservedCost, cstCheck, RWLockType);};
    
    double GetSellOrderAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FSellOrderAmount, &FLock, RWLockType);};
    void SetSellOrderAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FSellOrderAmount, &FLock, RWLockType);};
    bool IncreaseSellOrderAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellOrderAmount, cstCheck, RWLockType);};

    double GetSellActiveAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FSellActiveAmount, &FLock, RWLockType);};
    void SetSellActiveAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FSellActiveAmount, &FLock, RWLockType);};
    bool IncreaseSellActiveAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellActiveAmount, cstCheck, RWLockType);};
    
    double GetSellMatchAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FSellMatchAmount, &FLock, RWLockType);};
    void SetSellMatchAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FSellMatchAmount, &FLock, RWLockType);};
    bool IncreaseSellMatchAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellMatchAmount, cstCheck, RWLockType);};
    
    double GetBuyOrderAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FBuyOrderAmount, &FLock, RWLockType);};
    void SetBuyOrderAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FBuyOrderAmount, &FLock, RWLockType);};
    bool IncreaseBuyOrderAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyOrderAmount, cstCheck, RWLockType);};
    
    double GetBuyActiveAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FBuyActiveAmount, &FLock, RWLockType);};
    void SetBuyActiveAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FBuyActiveAmount, &FLock, RWLockType);};
    bool IncreaseBuyActiveAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyActiveAmount, cstCheck, RWLockType);};
    
    double GetBuyMatchAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FBuyMatchAmount, &FLock, RWLockType);};
    void SetBuyMatchAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FBuyMatchAmount, &FLock, RWLockType);};
    bool IncreaseBuyMatchAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyMatchAmount, cstCheck, RWLockType);};
    
    double GetReservedSellShortCost(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FReservedSellShortCost, &FLock, RWLockType);};
    void SetReservedSellShortCost(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FReservedSellShortCost, &FLock, RWLockType);};
    bool IncreaseReservedSellShortCost(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FReservedSellShortCost, cstCheck, RWLockType);};
    
    double GetSellShortOrderAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FSellShortOrderAmount, &FLock, RWLockType);};
    void SetSellShortOrderAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FSellShortOrderAmount, &FLock, RWLockType);};
    bool IncreaseSellShortOrderAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellShortOrderAmount, cstCheck, RWLockType);};
    
    double GetSellShortActiveAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FSellShortActiveAmount, &FLock, RWLockType);};
    void SetSellShortActiveAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FSellShortActiveAmount, &FLock, RWLockType);};
    bool IncreaseSellShortActiveAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellShortActiveAmount, cstCheck, RWLockType);};
    
    double GetSellShortMatchAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FSellShortMatchAmount, &FLock, RWLockType);};
    void SetSellShortMatchAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FSellShortMatchAmount, &FLock, RWLockType);};
    bool IncreaseSellShortMatchAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FSellShortMatchAmount, cstCheck, RWLockType);};
    
    double GetBuyShortOrderAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FBuyShortOrderAmount, &FLock, RWLockType);};
    void SetBuyShortOrderAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FBuyShortOrderAmount, &FLock, RWLockType);};
    bool IncreaseBuyShortOrderAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyShortOrderAmount, cstCheck, RWLockType);};
    
    double GetBuyShortActiveAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FBuyShortActiveAmount, &FLock, RWLockType);};
    void SetBuyShortActiveAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FBuyShortActiveAmount, &FLock, RWLockType);};
    bool IncreaseBuyShortActiveAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyShortActiveAmount, cstCheck, RWLockType);};
    
    double GetBuyShortMatchAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return UFC::GetVariableValue<double>(FBuyShortMatchAmount, &FLock, RWLockType);};
    void SetBuyShortMatchAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {UFC::SetVariableValue<double>(NewValue, FBuyShortMatchAmount, &FLock, RWLockType);};
    bool IncreaseBuyShortMatchAmount(double IncreaseValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseMemberValue(IncreaseValue, FBuyShortMatchAmount, cstCheck, RWLockType);};
        
    void AddNewOrderData(int OrderQty, double OrderPrice, char Side, char OrderType, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    void AddMatchOrderData(int MatchQty, double LastPrice, double OrderPrice, char Side, char OrderType, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    void CancelOrderData(int CancelQty, double OrderPrice, char Side, char OrderType, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    int GetReservedStockNoIndexCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetHMapElementCount(FReservedStockNoIndex, &FLock, RWLockType);};
    CRMReservedStock *FindReservedStockFromNoIndex(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) 
        {return FindElementFromHMap(FReservedStockNoIndex, StockNo, &FLock, RWLockType);};
    bool InsertReservedStockToNoIndex(const UFC::AnsiString& StockNo, CRMReservedStock *QuotaPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return InsertElementIntoHMap(FReservedStockNoIndex, StockNo, QuotaPtr, DoCheckExist, &FLock, UFC::rwNotLock);};
    int ClearReservedStockNoIndex(bool DoDelete = cstNotDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return ClearElementHMap(FReservedStockNoIndex, DoDelete, &FLock, RWLockType);};
    void DumpReservedStock(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    int GetReservedSellShortStockNoIndexCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetHMapElementCount(FReservedSellShortStockNoIndex, &FLock, RWLockType);};
    CRMReservedStock *FindReservedSellShortStockFromNoIndex(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) 
        {return FindElementFromHMap(FReservedSellShortStockNoIndex, StockNo, &FLock, RWLockType);};
    bool InsertReservedSellShortStockToNoIndex(const UFC::AnsiString& StockNo, CRMReservedStock *QuotaPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return InsertElementIntoHMap(FReservedSellShortStockNoIndex, StockNo, QuotaPtr, DoCheckExist, &FLock, UFC::rwNotLock);};
    int ClearReservedSellShortStockNoIndex(bool DoDelete = cstNotDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return ClearElementHMap(FReservedSellShortStockNoIndex, DoDelete, &FLock, RWLockType);};
    void DumpReservedSellShortStock(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    int GetAccQuotaNoIndexCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetHMapElementCount(FAccQuotaNoIndex, &FLock, RWLockType);};
    CRMBrokerTraderQuota *FindAccQuotaFromNoIndex(const UFC::AnsiString& AccountNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) 
        {return FindElementFromHMap(FAccQuotaNoIndex, AccountNo, &FLock, RWLockType);};
    bool InsertAccQuotaToNoIndex(const UFC::AnsiString& AccountNo, CRMBrokerTraderQuota *QuotaPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return InsertElementIntoHMap(FAccQuotaNoIndex, AccountNo, QuotaPtr, DoCheckExist, &FLock, UFC::rwNotLock);};
    int ClearAccQuotaNoIndex(bool DoDelete = cstNotDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return ClearElementHMap(FAccQuotaNoIndex, DoDelete, &FLock, RWLockType);};
    void DumpAccQuota(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    int GetConditionStockNoIndexCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetHMapElementCount(FConditionStockNoIndex, &FLock, RWLockType);};
    CRMTraderStockCondition *FindConditionFromStockNoIndex(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) 
        {return FindElementFromHMap(FConditionStockNoIndex, StockNo, &FLock, RWLockType);};
    bool InsertConditionToStockNoIndex(const UFC::AnsiString& StockNo, CRMTraderStockCondition *QuotaPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return InsertElementIntoHMap(FConditionStockNoIndex, StockNo, QuotaPtr, DoCheckExist, &FLock, UFC::rwNotLock);};
    int ClearConditionStockNoIndex(bool DoDelete = cstNotDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return ClearElementHMap(FConditionStockNoIndex, DoDelete, &FLock, RWLockType);};
    void DumpCondition(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    virtual void DumpData(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMBrokerTrader

//------------------------------------------------------------------------------
class CUSunRiskManagerObject : public IBasicRiskManagerObject
{
public:
    static bool DivideStrByFirstDelimiter(const UFC::AnsiString& OrigStr, char Delimiter, UFC::AnsiString& BeforeStr, UFC::AnsiString& AfterStr);
    static bool ParseOrderKey(const UFC::AnsiString& OrderKey, UFC::AnsiString& AE, UFC::AnsiString& TraderID);
    
protected:
    UFC::AnsiString FOperatorCostFileName;
    UFC::AnsiString FOperatorSellShortCostFileName;
    UFC::AnsiString FOperatorQuotaFileName;
    UFC::AnsiString FMainBrokerID;
    
    UFC::PHashMap<UFC::AnsiString, CRMBrokerTrader*> FBrokerTraderIDIndex;
    
    
public:
    CUSunRiskManagerObject(const UFC::AnsiString& RiskManagerName, const UFC::AnsiString& ConfigureFileName, bool IsTSEC);
    virtual ~CUSunRiskManagerObject();
    
    virtual bool LoadOperatorCostData(bool IsSellShort = false);
    virtual bool LoadOperatorQuotaData();
    virtual bool LoadSetting();
    virtual void PrintSetting();
    
    int GetBrokerTraderIDIndexCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetHMapElementCount(FBrokerTraderIDIndex, &FLock, RWLockType);};
    CRMBrokerTrader *FindBrokerTraderFromIDIndex(const UFC::AnsiString& TraderID, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) 
        {return FindElementFromHMap(FBrokerTraderIDIndex, TraderID, &FLock, RWLockType);};
    bool InsertBrokerTraderToIDIndex(const UFC::AnsiString& TraderID, CRMBrokerTrader *TraderPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return InsertElementIntoHMap(FBrokerTraderIDIndex, TraderID, TraderPtr, DoCheckExist, &FLock, UFC::rwNotLock);};
    int ClearBrokerTraderIDIndex(bool DoDelete = cstNotDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock)
        {return ClearElementHMap(FBrokerTraderIDIndex, DoDelete, &FLock, RWLockType);};
    void DumpBrokerTrader(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);

    virtual bool CheckRiskCondition(TWSE::MessageHeader*& Msg, OrderRecord& OI, const UFC::AnsiString& PVCID, UFC::AnsiString& CheckMessage);
    virtual void AddRiskMatchQty(TWSE::R3Body *R3MsgBodyPtr, const UFC::AnsiString& PVCId, const UFC::AnsiString& ExecKey);
    virtual void ProcessRiskCancelReplaceMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI, const UFC::AnsiString PVCID);
    virtual void ProcessRiskRejectMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI, const UFC::AnsiString PVCID);
    
    void RecoverRMConfirmData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& AE, const UFC::AnsiString& TraderID, const UFC::AnsiString& OrderKey, bool IsSynchronize = false);
    void RecoverRMFillData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& AE, const UFC::AnsiString& TraderID, const UFC::AnsiString& OrderKey, bool IsSynchronize = false);
    virtual void ParseExecRtpLogStr(const UFC::AnsiString& ExecRptLogStr, bool IsSynchronize = false);
    virtual void RecoverRMData(BOOL IsTSE);
};  //CUSunRiskManagerObject
}  //namespace rm
#endif /* USUNRISKMANAGER_H */

