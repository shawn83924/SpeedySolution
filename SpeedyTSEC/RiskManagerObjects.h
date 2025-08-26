/* 
 * File:   RiskManagerObjects.h
 * Author: Zhen Fan
 *
 * Created on 2014/11/14 12:11
 */

#ifndef RISKMANAGEROBJECTS_H
#define	RISKMANAGEROBJECTS_H
#include "UFC.h"
#include "../TSECLib/MessageHeader.h"

//------------------------------------------------------------------------------
const bool rmCheck = true;
const bool rmNotCheck = false;
const bool rmDelete = true;
const bool rmNotDelete = false;
const bool rmFlush = true;
const bool rmNotFlush = false;

//------------------------------------------------------------------------------
UFC::AnsiString DescribeBoolean(bool booleanValue);
UFC::AnsiString DescribeBoolean(BOOL boolValue);
void ParseRMDataFromUserData(const UFC::AnsiString& Key, UFC::AnsiString& WarrantNo, UFC::AnsiString& BrokerId);
double Parse94v99PriceStr(const UFC::AnsiString& PriceStr);
double Parse95v9999PriceStr(const UFC::AnsiString& PriceStr);
void SetNeedCheckRiskDataDate(bool NeedCheckDataDate);
bool NeedCheckRiskDataDate();
bool OpenRiskLogFile(BOOL IsTWSE, const UFC::AnsiString& LogFilePath);
void SetRiskLogLevel(Int32 RiskLogLevel);
bool NeedWriteRiskLog(Int32 RiskLogLevel);
void WriteRiskLog(const UFC::AnsiString& LogHeadStr, const char* FormatStr ...);
void WriteLevelRiskLog(Int32 LogLevel, const UFC::AnsiString& LogHeadStr, const char* FormatStr ...);
void FlushRiskLog();
void CloseRiskLogFile();
//------------------------------------------------------------------------------
class CRMBasicObject
{
protected:
    UFC::PReadWriteLock FObjectLock;
    UFC::AnsiString     FIndexKey;
    
public:
    template<typename fieldType> fieldType GetFieldValue(const fieldType& MemberField, UFC::RWLockTypeEnum RWLockType)
    {
        FObjectLock.Lock(RWLockType);
        fieldType resultValue = MemberField;
        FObjectLock.Unlock(RWLockType);
        return resultValue;
    }  //GetFieldValue()

    template<typename fieldType> void SetFieldValue(const fieldType& NewValue, fieldType& MemberField, UFC::RWLockTypeEnum RWLockType)
    {
        FObjectLock.Lock(RWLockType);
        MemberField = NewValue;
        FObjectLock.Unlock(RWLockType);
    }  //SetFieldValue()
        
    template<typename fieldType> fieldType IncreaseFieldValue(const fieldType& IncreaseValue, fieldType& MemberField, UFC::RWLockTypeEnum RWLockType)
    {
        FObjectLock.Lock(RWLockType);
        fieldType sumValue = MemberField + IncreaseValue;
        MemberField = sumValue;
        FObjectLock.Unlock(RWLockType);
        return sumValue;
    }  //IncreaseFieldValue()
        
    CRMBasicObject();
    virtual ~CRMBasicObject(){}
    CRMBasicObject(const UFC::AnsiString& IndexKey);
    
    void LockForRead() {FObjectLock.LockForRead();};
    void UnlockForRead() {FObjectLock.UnlockForRead();};
    void Lock(UFC::RWLockTypeEnum RWLockType) {FObjectLock.Lock(RWLockType);};
    
    
    void LockForWrite() {FObjectLock.LockForWrite();};
    void UnlockForWrite() {FObjectLock.UnlockForWrite();};
    void Unlock(UFC::RWLockTypeEnum RWLockType) {FObjectLock.Unlock(RWLockType);};
    
    virtual UFC::AnsiString GetIndexKey(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FIndexKey, RWLockType);};
    virtual void SetIndexKey(const UFC::AnsiString& IndexKey, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(IndexKey, FIndexKey, RWLockType);};
    
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FIndexKey, RWLockType);};
    virtual void DumpData(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMBasicObject

typedef UFC::PHashMap<UFC::AnsiString, CRMBasicObject*> CStrRiskManagerObjHMap;
//---------------------------------------------------------------------------
template<typename MapType>
int GetRiskManagerObjMapCount(MapType& RiskManagerObjMap, UFC::PReadWriteLock* RWLockPtr, UFC::RWLockTypeEnum RWLockType)
{
    if (RWLockPtr != 0) RWLockPtr->Lock(RWLockType);
    int mapCount = static_cast<int>(RiskManagerObjMap.ItemCount());
    if (RWLockPtr != 0) RWLockPtr->Unlock(RWLockType);
    return mapCount;
}  //GetRiskManagerObjMapCount()
//---------------------------------------------------------------------------
template<typename MapType, typename KeyType>
CRMBasicObject *FindRiskManagerObjFromMap(MapType& RiskManagerObjMap, const KeyType& KeyValue, UFC::PReadWriteLock* RWLockPtr, UFC::RWLockTypeEnum RWLockType)
{
    CRMBasicObject *riskManagerObjPtr = 0;
    if (RWLockPtr != 0) RWLockPtr->Lock(RWLockType);
    BOOL isSuccess = RiskManagerObjMap.GetObjectByKey(KeyValue, riskManagerObjPtr);
    if (RWLockPtr != 0) RWLockPtr->Unlock(RWLockType);
    if (isSuccess != TRUE) riskManagerObjPtr = 0;
    return riskManagerObjPtr;
}  //FindRiskManagerObjFromMap()
//---------------------------------------------------------------------------
template<typename MapType, typename KeyType>
bool InsertRiskManagerObjIntoMap(MapType& RiskManagerObjMap, const KeyType& KeyValue, CRMBasicObject *RiskManagerObjPtr, bool DoCheckExist, UFC::PReadWriteLock* RWLockPtr, UFC::RWLockTypeEnum RWLockType)
{
    if (RiskManagerObjPtr == 0) return false;
    if (RWLockPtr != 0) RWLockPtr->Lock(RWLockType);
    if (DoCheckExist && (FindRiskManagerObjFromMap(RiskManagerObjMap, KeyValue, RWLockPtr, UFC::rwNotLock) != 0)) return false;
    BOOL isSuccess = RiskManagerObjMap.Add(KeyValue, RiskManagerObjPtr);
    if (RWLockPtr != 0) RWLockPtr->Unlock(RWLockType);
    if (isSuccess == TRUE)
        return true;
    else
        return false;
}  //InsertRiskManagerObjIntoMap()
//---------------------------------------------------------------------------
template<typename MapType, typename KeyType>
CRMBasicObject *RemoveRiskManagerObjFromMap(MapType& RiskManagerObjMap, const KeyType& KeyValue, UFC::PReadWriteLock *RWLockPtr, UFC::RWLockTypeEnum RWLockType)
{
    if (RWLockPtr != 0) RWLockPtr->Lock(RWLockType);
    CRMBasicObject *riskManagerObjPtr = FindRiskManagerObjFromMap(RiskManagerObjMap, KeyValue, RWLockPtr, UFC::rwNotLock);
    RiskManagerObjMap.DeleteByKey(KeyValue);
    if (RWLockPtr != 0) RWLockPtr->Unlock(RWLockType);
    return riskManagerObjPtr;
}  //RemoveRiskManagerObjFromMap()
//---------------------------------------------------------------------------
template<typename MapType>
int ClearRiskManagerObjMap(MapType& RiskManagerObjMap, bool DoDelete, UFC::PReadWriteLock *RWLockPtr, UFC::RWLockTypeEnum RWLockType)
{
    int deleteCount = 0;
    if (RWLockPtr != 0) RWLockPtr->Lock(RWLockType);
    if (DoDelete)
    {
        CRMBasicObject *curRiskManagerObjPtr = RiskManagerObjMap.First();
        while(curRiskManagerObjPtr != 0)
        {
            deleteCount++;
            delete curRiskManagerObjPtr;
            curRiskManagerObjPtr = RiskManagerObjMap.Next();
        }  //while(curRiskManagerObjPtr != 0)
    }  //if (DoDelete)
    RiskManagerObjMap.Clear();
    if (RWLockPtr != 0) RWLockPtr->Unlock(RWLockType);
    return deleteCount;
}  //ClearRiskManagerObjMap()
//---------------------------------------------------------------------------
template<typename MapType>
void DumpRiskManagerObjMap(MapType& RiskManagerObjMap, bool DoFlush, UFC::PReadWriteLock *RWLockPtr, UFC::RWLockTypeEnum RWLockType)
{
    if (RWLockPtr != 0) RWLockPtr->Lock(RWLockType);
    CRMBasicObject *curRiskManagerObjPtr = RiskManagerObjMap.First();
    while(curRiskManagerObjPtr != 0)
    {
        curRiskManagerObjPtr->DumpData(rmNotFlush, UFC::rwLockForRead);
        curRiskManagerObjPtr = RiskManagerObjMap.Next();
    }  //while(curRiskManagerObjPtr != 0)
    if (RWLockPtr != 0) RWLockPtr->Unlock(RWLockType);
    if (DoFlush) UFC::BufferedLog::FlushToFile();
}  //DumpTradingObjMap()

//------------------------------------------------------------------------------
typedef UFC::PHashedSet<UFC::AnsiString> CStringHSet;
typedef UFC::PHashedSet<Int32> CInt32HSet;

//------------------------------------------------------------------------------
class CRMAccountNoKey : public CRMBasicObject
{
protected:
    UFC::AnsiString FAccountNo;  //集保帳號
    
public:
    CRMAccountNoKey();
    CRMAccountNoKey(const UFC::AnsiString& AccountNo);
    
    UFC::AnsiString GetAccountNo(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FAccountNo, RWLockType);};
    virtual void SetAccountNo(const UFC::AnsiString& AccountNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);

    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FAccountNo, RWLockType);};
};  //CRMAccountNoKey

//------------------------------------------------------------------------------
class CRMAccountStockNoKey : public CRMAccountNoKey
{
public:
    static UFC::AnsiString BuildIndexKey(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo);
    
protected:
    UFC::AnsiString FStockNo;  //股票代號
    
public:
    CRMAccountStockNoKey();
    CRMAccountStockNoKey(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo);
    
    virtual void SetAccountNo(const UFC::AnsiString& AccountNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    UFC::AnsiString GetStockNo(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FStockNo, RWLockType);};
    virtual void SetStockNo(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);

    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMAccountStockNoKey

//------------------------------------------------------------------------------
class CRMBrokerStockNoKey : public CRMAccountStockNoKey
{
public:
    static UFC::AnsiString BuildIndexKey(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char TWSEOrderType);
    
protected:
    UFC::AnsiString FWarrantNo;      //權證別
    UFC::AnsiString FBrokerId;       //券商代號
    char            FTWSEOrderType;  //'0':一般 '1':融資(證金) '2':融券(證金) '3':融資(自辦) '4':融券(自辦) '5':借券賣出(一般策略) '6':借券賣出(權證策略)
    
public:
    CRMBrokerStockNoKey();
    CRMBrokerStockNoKey(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char TWSEOrderType);
    
    virtual void SetAccountNo(const UFC::AnsiString& AccountNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    virtual void SetStockNo(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);

    UFC::AnsiString GetWarrantNo(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FWarrantNo, RWLockType);};
    virtual void SetWarrantNo(const UFC::AnsiString& WarrantNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    UFC::AnsiString GetBrokerId(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FBrokerId, RWLockType);};
    virtual void SetBrokerId(const UFC::AnsiString& BrokerId, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    char GetTWSEOrderType(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FTWSEOrderType, RWLockType);};
    virtual void SetTWSEOrderType(char OrderType, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMBrokerStockNoKey

//------------------------------------------------------------------------------
class CStockObject : public UFC::CMainKeyObject
{
protected:
    //T30 Fields
    UFC::AnsiString FStockNo;             //X(6)
    double          FBullPrice;           //9(4)V99 9(5)V9(4)
    double          FLDCPrice;            //9(4)V99 9(5)V9(4)
    double          FBearPrice;           //9(4)V99 9(5)V9(4)
    UFC::AnsiString FLastMatchDate;       //9(8)
    char            FSetType;             //X(1)  '0', '1', '2'
    char            FMarkW;               //X(1)  '0', '1', '2'
    char            FMarkP;               //X(1)  '0', '1'
    char            FMarkL;               //X(1)  '0', '1'
    UFC::AnsiString FINDCode;             //X(2)
    UFC::AnsiString FSTKCode;             //X(2)
    char            FMarkM;               //X(1)  '0', '1'
    UFC::AnsiString FStockName;           //X(16)
    int             FMarkWMarchInterval;  //9(3)  Minute
    int             FMarkWOrderLimit;     //9(6)  TradeUnit
    int             FMarkWOrdersLimit;    //9(6)  TradeUnit
    int             FMarkWPreparRate;     //9(3)  %
    char            FMarkS;               //X(1)  '0', '1'
    char            FMarkF;               //X(1)  '0', '1'
    char            FMarkDayTrade;        //X(1)  'Y':Buy First, 'X':Sell First, ' ':no DayTrade
    
    //T32 Fields
    int             FTradeUnit;         //9(5)
    UFC::AnsiString FCurrency;          //X(3)
    
    //T33 Fields
    bool   FIsOffHourEnabled;
    double FOffHourClosePrice;           //9(4)V99
    
    //O40 Fields
    bool   FIsOddEnabled;
    double FOddBullPrice;       //9(4)V99
    double FOddBearPrice;       //9(4)V99
    double FOddReferencePrice;  //9(4)V99
    
public:
    CStockObject();
    CStockObject(const UFC::AnsiString StockNo);
    
    virtual UFC::AnsiString GetStockNo(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<UFC::AnsiString>(FStockNo, LockType);};
    virtual void SetStockNo(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<UFC::AnsiString>(NewValue, FStockNo, LockType);};
    
    virtual double GetBullPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<double>(FBullPrice, LockType);};
    virtual void SetBullPrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<double>(NewValue, FBullPrice, LockType);};

    virtual double GetLDCPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<double>(FLDCPrice, LockType);};
    virtual void SetLDCPrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<double>(NewValue, FLDCPrice, LockType);};

    virtual double GetBearPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<double>(FBearPrice, LockType);};
    virtual void SetBearPrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<double>(NewValue, FBearPrice, LockType);};

    virtual UFC::AnsiString GetLastMatchDate(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<UFC::AnsiString>(FLastMatchDate, LockType);};
    virtual void SetLastMatchDate(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<UFC::AnsiString>(NewValue, FLastMatchDate, LockType);};

    virtual char GetSetType(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<char>(FSetType, LockType);};
    virtual void SetSetType(char NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<char>(NewValue, FSetType, LockType);};

    virtual char GetMarkW(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<char>(FMarkW, LockType);};
    virtual void SetMarkW(char NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<char>(NewValue, FMarkW, LockType);};

    virtual char GetMarkP(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<char>(FMarkP, LockType);};
    virtual void SetMarkP(char NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<char>(NewValue, FMarkP, LockType);};

    virtual char GetMarkL(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<char>(FMarkL, LockType);};
    virtual void SetMarkL(char NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<char>(NewValue, FMarkL, LockType);};

    virtual UFC::AnsiString GetINDCode(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<UFC::AnsiString>(FINDCode, LockType);};
    virtual void SetINDCode(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<UFC::AnsiString>(NewValue, FINDCode, LockType);};

    virtual UFC::AnsiString GetSTKCode(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<UFC::AnsiString>(FSTKCode, LockType);};
    virtual void SetSTKCode(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<UFC::AnsiString>(NewValue, FSTKCode, LockType);};

    virtual char GetMarkM(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<char>(FMarkM, LockType);};
    virtual void SetMarkM(char NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<char>(NewValue, FMarkM, LockType);};

    virtual UFC::AnsiString GetStockName(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<UFC::AnsiString>(FStockName, LockType);};
    virtual void SetStockName(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<UFC::AnsiString>(NewValue, FStockName, LockType);};
    
    virtual int GetMarkWMarchInterval(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<int>(FMarkWMarchInterval, LockType);};
    virtual void SetMarkWMarchInterval(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<int>(NewValue, FMarkWMarchInterval, LockType);};

    virtual int GetMarkWOrderLimit(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<int>(FMarkWOrderLimit, LockType);};
    virtual void SetMarkWOrderLimit(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<int>(NewValue, FMarkWOrderLimit, LockType);};

    virtual int GetMarkWOrdersLimit(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<int>(FMarkWOrdersLimit, LockType);};
    virtual void SetMarkWOrdersLimit(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<int>(NewValue, FMarkWOrdersLimit, LockType);};

    virtual int GetMarkWPreparRate(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<int>(FMarkWPreparRate, LockType);};
    virtual void SetMarkWPreparRate(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<int>(NewValue, FMarkWPreparRate, LockType);};

    virtual char GetMarkS(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<char>(FMarkS, LockType);};
    virtual void SetMarkS(char NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<char>(NewValue, FMarkS, LockType);};

    virtual char GetMarkF(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<char>(FMarkF, LockType);};
    virtual void SetMarkF(char NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<char>(NewValue, FMarkF, LockType);};

    virtual char GetMarkDayTrade(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<char>(FMarkDayTrade, LockType);};
    virtual void SetMarkDayTrade(char NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<char>(NewValue, FMarkDayTrade, LockType);};
    
    virtual int GetTradeUnit(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<int>(FTradeUnit, LockType);};
    virtual void SetTradeUnit(int NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<int>(NewValue, FTradeUnit, LockType);};

    virtual UFC::AnsiString GetCurrency(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<UFC::AnsiString>(FCurrency, LockType);};
    virtual void SetCurrency(const UFC::AnsiString& NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<UFC::AnsiString>(NewValue, FCurrency, LockType);};

    virtual double GetOffHourClosePrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<double>(FOffHourClosePrice, LockType);};
    virtual void SetOffHourClosePrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<double>(NewValue, FOffHourClosePrice, LockType);};

    virtual bool IsOffHourEnabled(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<bool>(FIsOffHourEnabled, LockType);};
    virtual void SetOffHourEnabled(bool NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<bool>(NewValue, FIsOffHourEnabled, LockType);};

    virtual bool IsOddEnabled(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<bool>(FIsOddEnabled, LockType);};
    virtual void SetOddEnabled(bool NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<bool>(NewValue, FIsOddEnabled, LockType);};

    virtual double GetOddBullPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<double>(FOddBullPrice, LockType);};
    virtual void SetOddBullPrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<double>(NewValue, FOddBullPrice, LockType);};

    virtual double GetOddBearPrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<double>(FOddBearPrice, LockType);};
    virtual void SetOddBearPrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<double>(NewValue, FOddBearPrice, LockType);};

    virtual double GetOddReferencePrice(UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {return GetFieldValue<double>(FOddReferencePrice, LockType);};
    virtual void SetOddReferencePrice(double NewValue, UFC::RWLockTypeEnum LockType = UFC::rwNotLock) {SetFieldValue<double>(NewValue, FOddReferencePrice, LockType);};
    
    virtual bool IsMeasuringStock(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum LockType = UFC::rwNotLock);    
};  //CStockObject
        
//------------------------------------------------------------------------------
class CRMOrderSummary : public CRMBasicObject
{
protected:
    UFC::AnsiString    FOrderId;     
    char               FSide;
    UFC::AnsiString    FStockNo;      //股票代號
    double             FOrderPrice;   //委託價
    int                FOrderQty;     //委託量
    int                FLeavesQty;    //剩餘量
    UFC::AnsiString    FWarrantNo;    //權證別
    UFC::AnsiString    FBrokerId;     //券商代號
    char               FTWSEOrderType;
    char               FExchangeCode; //交易碼 0:一般 1：鉅額 2：零股 3：外國股票價確認碼
    TWSE::TMessageType FMsgType;
    UFC::AnsiString    FAccountNo;    //集保帳號
    double             FOrderAmount;

public:
    CRMOrderSummary(const UFC::AnsiString& OrderId, char Side, const UFC::AnsiString& StockNo,
                    const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char TWSEOrderType, 
                    double FOrderPrice, int OrderQty, char ExchangeCode, TWSE::TMessageType MsgType, const UFC::AnsiString& AccountNo = "");
    
    UFC::AnsiString GetOrderId(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FOrderId, RWLockType);};

    char GetSide(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FSide, RWLockType);};
    
    UFC::AnsiString GetStockNo(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FStockNo, RWLockType);};
    
    double GetOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FOrderPrice, RWLockType);};
    
    int GetOrderQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FOrderQty, RWLockType);};
    void SetOrderQty(int OrderQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(OrderQty, FOrderQty, RWLockType);};
    int IncreaseOrderQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseFieldValue(IncreaseQty, FOrderQty, RWLockType);};
    
    int GetLeavesOrderQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FLeavesQty, RWLockType);};
    void SetLeavesQty(int OrderQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(OrderQty, FLeavesQty, RWLockType);};
    int IncreaseLeavesQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseFieldValue(IncreaseQty, FLeavesQty, RWLockType);};

    double GetOrderAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FOrderAmount, RWLockType);};
    void SetOrderAmount(double OrderAmount, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(OrderAmount, FOrderAmount, RWLockType);};
    
    UFC::AnsiString GetWarrantNo(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FWarrantNo, RWLockType);};
    UFC::AnsiString GetBrokerId(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FBrokerId, RWLockType);};
    char GetTWSEOrderType(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FTWSEOrderType, RWLockType);};
    char GetExchangeCode(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FExchangeCode, RWLockType);};
    TWSE::TMessageType GetMsgType(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FMsgType, RWLockType);};
    UFC::AnsiString GetAccountNo(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FAccountNo, RWLockType);};    
    
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMOrderSummary

//------------------------------------------------------------------------------
class CRMBrokerStockCondition : public CRMBrokerStockNoKey
{
protected:
    int             FAvailableSellQty;    //可賣股數
    int             FAvailableBuyQty;     //可買股數
    int             FNormalMatchBuyQty;   //今日一般成交買進股數
    int             FNormalMatchSellQty;  //今日一般成交賣出股數
    int             FMatchBuyQty;         //今日成交買進股數
    int             FMatchSellQty;        //今日成交賣出股數
    int             FNormalOrderBuyQty;   //今日一般委託買進股數
    int             FNormalOrderSellQty;  //今日一般委託賣出股數
    int             FOrderBuyQty;         //今日委託買進股數
    int             FOrderSellQty;        //今日委託賣出股數
    UFC::AnsiString FDataDate;            //資料日期
    bool            FIsSellShort;         //是否為借券
    
public:
    CRMBrokerStockCondition(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo, 
                            const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char TWSEOrderType,
                            int AvailableSellQty, int AvailableBuyQty, 
                            const UFC::AnsiString& DataDate = "", bool IsSellShort = false);
    
    int GetAvailableSellQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FAvailableSellQty, RWLockType);};
    void SetAvailableSellQty(int AvailableSellQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(AvailableSellQty, FAvailableSellQty, RWLockType);};

    int GetAvailableBuyQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FAvailableBuyQty, RWLockType);};
    void SetAvailableBuyQty(int AvailableBuyQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(AvailableBuyQty, FAvailableBuyQty, RWLockType);};
    
    int GetNormalMatchBuyQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FNormalMatchBuyQty, RWLockType);};
    void SetNormalMatchBuyQty(int MatchBuyQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(MatchBuyQty, FNormalMatchBuyQty, RWLockType);};
    int IncreaseNormalMatchBuyQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseFieldValue(IncreaseQty, FNormalMatchBuyQty, RWLockType);};
    
    int GetNormalMatchSellQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FNormalMatchSellQty, RWLockType);};
    void SetNormalMatchSellQty(int MatchSellQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(MatchSellQty, FNormalMatchSellQty, RWLockType);};
    int IncreaseNormalMatchSellQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseFieldValue(IncreaseQty, FNormalMatchSellQty, RWLockType);};
    
    int GetMatchBuyQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FMatchBuyQty, RWLockType);};
    void SetMatchBuyQty(int MatchBuyQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(MatchBuyQty, FMatchBuyQty, RWLockType);};
    int IncreaseMatchBuyQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseFieldValue(IncreaseQty, FMatchBuyQty, RWLockType);};
    
    int GetMatchSellQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FMatchSellQty, RWLockType);};
    void SetMatchSellQty(int MatchSellQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(MatchSellQty, FMatchSellQty, RWLockType);};
    int IncreaseMatchSellQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseFieldValue(IncreaseQty, FMatchSellQty, RWLockType);};
    
    int GetNormalOrderBuyQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FNormalOrderBuyQty, RWLockType);};
    void SetNormalOrderBuyQty(int OrderBuyQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(OrderBuyQty, FNormalOrderBuyQty, RWLockType);};
    int IncreaseNormalOrderBuyQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseFieldValue(IncreaseQty, FNormalOrderBuyQty, RWLockType);};
    
    int GetNormalOrderSellQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FNormalOrderSellQty, RWLockType);};
    void SetNormalOrderSellQty(int OrderSellQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(OrderSellQty, FNormalOrderSellQty, RWLockType);};
    int IncreaseNormalOrderSellQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseFieldValue(IncreaseQty, FNormalOrderSellQty, RWLockType);};
    
    int GetOrderBuyQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FOrderBuyQty, RWLockType);};
    void SetOrderBuyQty(int OrderBuyQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(OrderBuyQty, FOrderBuyQty, RWLockType);};
    int IncreaseOrderBuyQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseFieldValue(IncreaseQty, FOrderBuyQty, RWLockType);};
    
    int GetOrderSellQty(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FOrderSellQty, RWLockType);};
    void SetOrderSellQty(int OrderSellQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(OrderSellQty, FOrderSellQty, RWLockType);};
    int IncreaseOrderSellQty(int IncreaseQty, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return IncreaseFieldValue(IncreaseQty, FOrderSellQty, RWLockType);};
    
    UFC::AnsiString GetDataDate(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FDataDate, RWLockType);};
    virtual void SetDataDate(const UFC::AnsiString& DataDate, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(DataDate, FDataDate, RWLockType);};

    bool IsSellShort(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FIsSellShort, RWLockType);};
    virtual void SetSellShort(bool IsSellShort, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(IsSellShort, FIsSellShort, RWLockType);};
    
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMBrokerStockCondition

//------------------------------------------------------------------------------
class CRMStockTradingData : public CRMAccountStockNoKey
{
private:
    static double FInitialMinOrderPrice;
    
protected:
    CRMOrderSummary *FMaxNormalBuyOrderPtr;    //最高一般買進委託單
    double          FMaxNormalBuyOrderPrice;   //最高一般買進委託價
    CRMOrderSummary *FMinNormalSellOrderPtr;   //最高一般賣出委託單
    double          FMinNormalSellOrderPrice;  //最低一般賣出委託價
    CRMOrderSummary *FMaxBuyOrderPtr;          //最高買進委託單
    double          FMaxBuyOrderPrice;         //最高買進委託價
    CRMOrderSummary *FMinSellOrderPtr;         //最高賣出委託單
    double          FMinSellOrderPrice;        //最低賣出委託價
    
    CStrRiskManagerObjHMap FNormalBuyOrderIdIndex;
    CStrRiskManagerObjHMap FNormalSellOrderIdIndex;
    CStrRiskManagerObjHMap FBuyOrderIdIndex;
    CStrRiskManagerObjHMap FSellOrderIdIndex;
    CStrRiskManagerObjHMap FBrokerStockNoIndex;

    double FBuyAmount;
    double FSellAmount;

    CRMOrderSummary *FindMaxOrderPriceOrder(CStrRiskManagerObjHMap& OrderIdIndex, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    double FindMaxNormalBuyOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    double FindMaxBuyOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMOrderSummary *FindMinOrderPriceOrder(CStrRiskManagerObjHMap& OrderIdIndex, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    double FindMinNormalSellOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    double FindMinSellOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
public:
    CRMStockTradingData(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo);
    ~CRMStockTradingData();
     
    virtual CRMOrderSummary *GetMaxNormalBuyOrder(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FMaxNormalBuyOrderPtr, RWLockType);};
    virtual double GetMaxNormalBuyOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FMaxNormalBuyOrderPrice, RWLockType);};
    virtual void SetMaxNormalBuyOrderPrice(double MaxBuyOrderPrice, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(MaxBuyOrderPrice, FMaxNormalBuyOrderPrice, RWLockType);};    
    
    virtual CRMOrderSummary *GetMinNormalSellOrder(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FMinNormalSellOrderPtr, RWLockType);};
    virtual double GetMinNormalSellOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FMinNormalSellOrderPrice, RWLockType);};
    virtual void SetMinNormalSellOrderPrice(double MinSellOrderPrice, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(MinSellOrderPrice, FMinNormalSellOrderPrice, RWLockType);};    
    
    virtual CRMOrderSummary *GetMaxBuyOrder(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FMaxBuyOrderPtr, RWLockType);};
    virtual double GetMaxBuyOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FMaxBuyOrderPrice, RWLockType);};
    virtual void SetMaxBuyOrderPrice(double MaxBuyOrderPrice, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(MaxBuyOrderPrice, FMaxBuyOrderPrice, RWLockType);};    
    
    virtual CRMOrderSummary *GetMinSellOrder(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FMinSellOrderPtr, RWLockType);};
    virtual double GetMinSellOrderPrice(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FMinSellOrderPrice, RWLockType);};
    virtual void SetMinSellOrderPrice(double MinSellOrderPrice, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(MinSellOrderPrice, FMinSellOrderPrice, RWLockType);};    

    virtual double GetBuyAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FBuyAmount, RWLockType);};
    virtual void SetBuyAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(NewValue, FBuyAmount, RWLockType);};
    
    virtual double GetSellAmount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FSellAmount, RWLockType);};
    virtual void SetSellAmount(double NewValue, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {SetFieldValue(NewValue, FSellAmount, RWLockType);};
    
    int GetNormalBuyOrderCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMOrderSummary *FindNormalBuyOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    bool InsertNormalBuyOrder(const UFC::AnsiString& OrderId, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMOrderSummary *RemoveNormalBuyOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    int ClearNormalBuyOrder(bool DoDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    void DumpNormalBuyOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    int GetNormalSellOrderCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMOrderSummary *FindNormalSellOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    bool InsertNormalSellOrder(const UFC::AnsiString& OrderId, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMOrderSummary *RemoveNormalSellOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    int ClearNormalSellOrder(bool DoDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    void DumpNormalSellOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);    

    int GetBuyOrderCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMOrderSummary *FindBuyOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    bool InsertBuyOrder(const UFC::AnsiString& OrderId, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMOrderSummary *RemoveBuyOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    int ClearBuyOrder(bool DoDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    void DumpBuyOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    int GetSellOrderCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMOrderSummary *FindSellOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    bool InsertSellOrder(const UFC::AnsiString& OrderId, CRMOrderSummary *OrderSummaryPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMOrderSummary *RemoveSellOrder(const UFC::AnsiString& OrderId, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    int ClearSellOrder(bool DoDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    void DumpSellOrder(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);    

    int GetBrokerStockConditionCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMBrokerStockCondition *FindBrokerStockCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char TWSEOrderType, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    bool InsertBrokerStockCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char TWSEOrderType, CRMBrokerStockCondition *ConditionPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMBrokerStockCondition *RemoveBrokerStockCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char TWSEOrderType, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    int ClearBrokerStockCondition(bool DoDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    void DumpBrokerStockCondition(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);    
    
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    virtual void DumpData(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMStockTradingData

//------------------------------------------------------------------------------
class CRMInvestorAccount : public CRMAccountNoKey
{
protected:
    int FDayTradeType;           //0：不可賣當日買股票，1：可賣當日買股票
    int FSelfTradeType;          //0:不檢查自行成交，1：檢查自行成交
    int FSellPositionCheckType;  //0:不檢查賣出部位，1：檢查賣出部位
    
    CStrRiskManagerObjHMap FStockNoIndex;
    
public:
    CRMInvestorAccount(const UFC::AnsiString& AccountNo, int DayTradeType, int SelfTradeType, int SellPositionCheckType);
    ~CRMInvestorAccount();
    
    UFC::AnsiString GetAccountNo(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FAccountNo, RWLockType);};
    
    int GetDayTradeType(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FDayTradeType, RWLockType);};
    int GetSelfTradeType(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FSelfTradeType, RWLockType);};
    int GetSellPositionCheckType(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock) {return GetFieldValue(FSellPositionCheckType, RWLockType);};
    
    int GetStockDataCount(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMStockTradingData *FindStockData(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMStockTradingData *FindOrCreateStockData(const UFC::AnsiString& StockNo, bool& IsNewStockData, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    CRMStockTradingData *FindOrCreateStockData(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    bool InsertStockData(const UFC::AnsiString& StockNo, CRMStockTradingData *StockDataPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    int ClearStockData(bool DoDelete, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    void DumpStockData(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    
    virtual UFC::AnsiString ToString(UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
    virtual void DumpData(bool DoFlush, UFC::RWLockTypeEnum RWLockType = UFC::rwNotLock);
};  //CRMInvestorAccount

#endif	/* RISKMANAGEROBJECTS_H */

