/*
 * File:   MegaRiskManager.h
 * Author: Zhen Fan
 *
 * Created on 2016/02/03, 11:16
 */

#include "MegaRiskManager.h"
#include "../UFC/iniFile.h"
#include "MessageHeader.h"
#include "TMessages.h"
#include "OMessages.h"
#include "PMessages.h"

namespace rm
{
//---------------------------- CRMBrokerStockNoKey -----------------------------
//------------------------------------------------------------------------------
UFC::AnsiString CRMBrokerStockNoKey::BuildIndexKey(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char OrderType)
{
    UFC::AnsiString indexKey;
    indexKey.Printf("%s%s%s%s%c", AccountNo.c_str(), StockNo.c_str(), WarrantNo.c_str(), BrokerId.c_str(), OrderType);
    return indexKey;
}  //CRMBrokerStockNoKey::BuildIndexKey()
//------------------------------------------------------------------------------
CRMBrokerStockNoKey::CRMBrokerStockNoKey()
:CRMAccountStockNoKey()
,FWarrantNo("")
,FBrokerId("")
,FOrderType(' ')
{    
}  //CRMBrokerStockNoKey::CRMBrokerStockNoKey()
//------------------------------------------------------------------------------
CRMBrokerStockNoKey::CRMBrokerStockNoKey(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char OrderType)
:CRMAccountStockNoKey()
,FWarrantNo(WarrantNo)
,FBrokerId(BrokerId)
,FOrderType(OrderType)
{
    FStockNo = StockNo;
    FAccountNo = AccountNo;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo, FWarrantNo, FBrokerId, FOrderType);
}  //CRMBrokerStockNoKey::CRMBrokerStockNoKey()
//------------------------------------------------------------------------------
void CRMBrokerStockNoKey::SetAccountNo(const UFC::AnsiString& AccountNo, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    FAccountNo = AccountNo;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo, FWarrantNo, FBrokerId, FOrderType);
    FLock.Unlock(RWLockType);
}  //CRMBrokerStockNoKey::SetAccountNo()
//------------------------------------------------------------------------------
void CRMBrokerStockNoKey::SetStockNo(const UFC::AnsiString& StockNo, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    FStockNo = StockNo;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo, FWarrantNo, FBrokerId, FOrderType);
    FLock.Unlock(RWLockType);
}  //CRMBrokerStockNoKey::SetStockNo()
//------------------------------------------------------------------------------
void CRMBrokerStockNoKey::SetWarrantNo(const UFC::AnsiString& WarrantNo, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    FWarrantNo = WarrantNo;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo, FWarrantNo, FBrokerId, FOrderType);
    FLock.Unlock(RWLockType);
}  //CRMBrokerStockNoKey::SetWarrantNo()
//------------------------------------------------------------------------------
void CRMBrokerStockNoKey::SetBrokerId(const UFC::AnsiString& BrokerId, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    FBrokerId = BrokerId;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo, FWarrantNo, FBrokerId, FOrderType);
    FLock.Unlock(RWLockType);
}  //CRMBrokerStockNoKey::SetBrokerId()
//------------------------------------------------------------------------------
void CRMBrokerStockNoKey::SetOrderType(char OrderType, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    FOrderType = OrderType;
    FIndexKey = BuildIndexKey(FAccountNo, FStockNo, FWarrantNo, FBrokerId, FOrderType);
    FLock.Unlock(RWLockType);
}  //CRMBrokerStockNoKey::SetOrderType()
//------------------------------------------------------------------------------
UFC::AnsiString CRMBrokerStockNoKey::ToString(UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString objStr = "";
    FLock.Lock(RWLockType);
    objStr.Printf("Account[%s] Stock[%s] WarrantNo[%s] BrokerId[%s] OrderType[%c]", FAccountNo.c_str(), FStockNo.c_str(), FWarrantNo.c_str(), FBrokerId.c_str(), FOrderType);
    FLock.Unlock(RWLockType);
    return objStr;    
}  //CRMBrokerStockNoKey::ToString()

//-------------------------- CRMBrokerStockCondition ---------------------------
//------------------------------------------------------------------------------
CRMBrokerStockCondition::CRMBrokerStockCondition(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo, 
                                                 const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char OrderType,
                                                 int AvailableSellQty, int AvailableBuyQty, 
                                                 const UFC::AnsiString& DataDate, bool IsSellShort)
:CRMBrokerStockNoKey(AccountNo, StockNo, WarrantNo, BrokerId, OrderType)
,FAvailableSellQty(AvailableSellQty)
,FAvailableBuyQty(AvailableBuyQty)
,FNormalMatchBuyQty(0)
,FNormalMatchSellQty(0)
,FMatchBuyQty(0)
,FMatchSellQty(0)
,FNormalOrderBuyQty(0)
,FNormalOrderSellQty(0)
,FOrderBuyQty(0)
,FOrderSellQty(0)
,FDataDate(DataDate)
,FIsSellShort(IsSellShort)
{
}  //CRMBrokerStockCondition::CRMBrokerStockCondition()
//------------------------------------------------------------------------------
UFC::AnsiString CRMBrokerStockCondition::ToString(UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString objStr = "";
    UFC::AnsiString sellShortStr = " ";
    FLock.Lock(RWLockType);
    if (FIsSellShort) sellShortStr = "Sell Short";
    objStr.Printf("Acc[%s] Stock[%s][%s][%s]%c available[S %d, B %d] match[NS %d, NB %d; S %d, B %d] order[NS %d, NB %d; S %d, B %d] Date[%s] %s", 
                  FAccountNo.c_str(), FStockNo.c_str(), FWarrantNo.c_str(), FBrokerId.c_str(), FOrderType,
                  FAvailableSellQty, FAvailableBuyQty, 
                  FNormalMatchSellQty, FNormalMatchBuyQty, FMatchSellQty, FMatchBuyQty, 
                  FNormalOrderSellQty, FNormalOrderBuyQty, FOrderSellQty, FOrderBuyQty, 
                  FDataDate.c_str(), sellShortStr.c_str());
    FLock.Unlock(RWLockType);
    return objStr;    
}  //CRMBrokerStockCondition::ToString()

//-------------------------- CRMMegaStockTradingData ---------------------------
//------------------------------------------------------------------------------
CRMMegaStockTradingData::CRMMegaStockTradingData(const UFC::AnsiString& AccountNo, const UFC::AnsiString& StockNo)
:CRMStockTradingData(AccountNo, StockNo)
{
}  //CRMMegaStockTradingData::CRMMegaStockTradingData()
//------------------------------------------------------------------------------
CRMMegaStockTradingData::~CRMMegaStockTradingData()
{
    ClearBrokerStockCondition(cstNotDelete, UFC::rwNotLock);
}  //CRMMegaStockTradingData::~CRMMegaStockTradingData()
//------------------------------------------------------------------------------
CRMBrokerStockCondition *CRMMegaStockTradingData::FindBrokerStockCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char OrderType, UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString keyValue = CRMBrokerStockNoKey::BuildIndexKey(FAccountNo, FStockNo, WarrantNo, BrokerId, OrderType);
    return FindElementFromHMap(FBrokerStockNoIndex, keyValue, &FLock, RWLockType);   
}  //CRMMegaStockTradingData::FindBrokerStockCondition()
//------------------------------------------------------------------------------
bool CRMMegaStockTradingData::InsertBrokerStockCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char OrderType, CRMBrokerStockCondition *ConditionPtr, bool DoCheckExist, UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString keyValue = CRMBrokerStockNoKey::BuildIndexKey(FAccountNo, FStockNo, WarrantNo, BrokerId, OrderType);
    return InsertElementIntoHMap(FBrokerStockNoIndex, keyValue, ConditionPtr, DoCheckExist, &FLock, RWLockType);
}  //CRMMegaStockTradingData::InsertBrokerStockCondition()
//------------------------------------------------------------------------------
CRMBrokerStockCondition *CRMMegaStockTradingData::RemoveBrokerStockCondition(const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, char OrderType, UFC::RWLockTypeEnum RWLockType)
{    
    UFC::AnsiString keyValue = CRMBrokerStockNoKey::BuildIndexKey(FAccountNo, FStockNo, WarrantNo, BrokerId, OrderType);
    return RemoveElementFromHMap(FBrokerStockNoIndex, keyValue, &FLock, RWLockType);
}  //CRMMegaStockTradingData::RemoveBrokerStockCondition()
//------------------------------------------------------------------------------
void CRMMegaStockTradingData::DumpBrokerStockCondition(bool DoFlush, UFC::RWLockTypeEnum RWLockType)    
{    
    FLock.Lock(RWLockType);
    UFC::BufferedLog::Printf(" %d Broker Stock Condition", FBrokerStockNoIndex.ItemCount());
    DumpRMObjHMap(FBrokerStockNoIndex, DoFlush, &FLock, UFC::rwNotLock);
    FLock.Unlock(RWLockType);
    UFC::BufferedLog::Printf(" ");
    if (DoFlush) UFC::BufferedLog::FlushToFile();
}  //CRMMegaStockTradingData::DumpBrokerStockCondition()
//------------------------------------------------------------------------------
UFC::AnsiString CRMMegaStockTradingData::ToString(UFC::RWLockTypeEnum RWLockType)
{
    UFC::AnsiString objStr = "";
    FLock.Lock(RWLockType);
    UFC::AnsiString normalMaxBuyStr;
    if (FMaxNormalBuyOrderPtr != 0)
        normalMaxBuyStr.Printf("N:%s:%11.4lf", FMaxNormalBuyOrderPtr->GetOrderId(UFC::rwLockForRead).c_str(), FMaxNormalBuyOrderPrice);
    else
        normalMaxBuyStr.Printf("N:%11.4lf", FMaxNormalBuyOrderPrice);
        
    UFC::AnsiString maxBuyStr;
    if (FMaxBuyOrderPtr != 0)
        maxBuyStr.Printf("%s:%11.4lf", FMaxBuyOrderPtr->GetOrderId(UFC::rwLockForRead).c_str(), FMaxBuyOrderPrice);
    else
        maxBuyStr.Printf("%10.4lf", FMaxBuyOrderPrice);

    UFC::AnsiString normalMinSellStr;
    if (FMinNormalSellOrderPtr != 0)
        normalMinSellStr.Printf("N:%s:%11.4lf", FMinNormalSellOrderPtr->GetOrderId(UFC::rwLockForRead).c_str(), FMinNormalSellOrderPrice);
    else
        normalMinSellStr.Printf("N:%11.4lf", FMinNormalSellOrderPrice);
    
    UFC::AnsiString minSellStr;
    if (FMinSellOrderPtr != 0)
        minSellStr.Printf("%s:%11.4lf", FMinSellOrderPtr->GetOrderId(UFC::rwLockForRead).c_str(), FMinSellOrderPrice);
    else
        minSellStr.Printf("%11.4lf", FMinSellOrderPrice);
    
    objStr.Printf("Acc[%s] Stock[%s] MaxBuy[%s, %s] MinSell[%s, %s]", FAccountNo.c_str(), FStockNo.c_str(), normalMaxBuyStr.c_str(), maxBuyStr.c_str(), normalMinSellStr.c_str(), minSellStr.c_str());
    FLock.Unlock(RWLockType);
    return objStr;
}  //CRMMegaStockTradingData::ToString()
//------------------------------------------------------------------------------
void CRMMegaStockTradingData::DumpData(bool DoFlush, UFC::RWLockTypeEnum RWLockType)
{
    FLock.Lock(RWLockType);
    UFC::AnsiString objStr = ToString(UFC::rwNotLock);
    UFC::BufferedLog::Printf(" %s", objStr.c_str());
    DumpBrokerStockCondition(cstNotFlush, UFC::rwNotLock);
    DumpBuyOrder(cstNotFlush, UFC::rwNotLock);
    DumpSellOrder(cstNotFlush, UFC::rwNotLock);
    FLock.Unlock(RWLockType);
    if (DoFlush) UFC::BufferedLog::FlushToFile();
}  //CRMMegaStockTradingData::DumpData()

//--------------------------- CMegaRiskManagerObject ---------------------------
bool CMegaRiskManagerObject::FNeedCheckDataDate = true;

//------------------------------------------------------------------------------
void CMegaRiskManagerObject::SetNeedCheckRiskDataDate(bool NeedCheckDataDate)
{
    FNeedCheckDataDate = NeedCheckDataDate;
}  //CMegaRiskManagerObject::SetNeedCheckDataDate()
//------------------------------------------------------------------------------
bool CMegaRiskManagerObject::NeedCheckRiskDataDate()
{
    return FNeedCheckDataDate;
}  //CMegaRiskManagerObject::NeedCheckDataDate()
//------------------------------------------------------------------------------
void CMegaRiskManagerObject::ParseRMDataFromUserData(const UFC::AnsiString& Key, UFC::AnsiString& WarrantNo, UFC::AnsiString& BrokerId)
{
    WarrantNo = "";
    BrokerId = "";
    int keyLength = Key.Length();
    if (keyLength <= 0) return;

    int commaPos = Key.AnsiPos(',');
    if (commaPos < 0) return;
    
    int userDataLength = keyLength - (commaPos + 1);
    if (userDataLength <= 0) return;        
    UFC::AnsiString userData = Key.SubString(commaPos + 1, userDataLength);
    
    UFC::PStringList riskManagerFieldList;
    riskManagerFieldList.SetStrings(userData, ";\n");
    
    if (riskManagerFieldList.ItemCount() >= 1) WarrantNo = riskManagerFieldList[0];
    if (riskManagerFieldList.ItemCount() >= 2) BrokerId = riskManagerFieldList[1];
}  //CMegaRiskManagerObject::ParseRMDataFromUserData()
//------------------------------------------------------------------------------
CMegaRiskManagerObject::CMegaRiskManagerObject(const UFC::AnsiString& RiskManagerName, const UFC::AnsiString& ConfigureFileName, bool IsTSEC)
:IBasicRiskManagerObject(RiskManagerName, ConfigureFileName, IsTSEC)
,FRMControlFilePrefix("CTLBDW")
,FRMControlFileExtension("")
,FRMCurrentControlFileSeqNo(1)
,FRMDeleteFilePrefix("ORDDEL")
,FRMDeleteFileExtension("")
,FRMHedgeStockFileName("")
,FRMCurrentDeleteFileSeqNo(1)
,FRMMainBrokerID("600T")
,FRMAccountStockAmountLimit(-1.0)
{
    LoadSetting();
    PrintSetting();
}  //CMegaRiskManagerObject::CMegaRiskManagerObject()
//------------------------------------------------------------------------------
CMegaRiskManagerObject::~CMegaRiskManagerObject()
{
}  //CMegaRiskManagerObject::~CMegaRiskManagerObject()
//----------------------------------------------------------------------------------------------------------------------
bool CMegaRiskManagerObject::IsRMHedgeStockExists(const UFC::AnsiString& StockNo)
{
    if (StockNo.Length() <= 0) return false;
    if (FRMHedgeStockSet.Exists(StockNo) == TRUE)
        return true;
    else
        return false;
}  //CMegaRiskManagerObject::IsRMHedgeStockExists()
//----------------------------------------------------------------------------------------------------------------------
bool CMegaRiskManagerObject::AddRMHedgeStockToSet(const UFC::AnsiString& StockNo)
{
    if (StockNo.Length() <= 0) return false;
    if (FRMHedgeStockSet.Exists(StockNo) == FALSE)
    {
        FRMHedgeStockSet.Add(StockNo);
        return true;
    }
    
    return false;
}  //CMegaRiskManagerObject::AddRMHedgeStockToSet()
//------------------------------------------------------------------------------
void CMegaRiskManagerObject::DumpRMHedgeStockSet()
{
    UFC::AnsiString logHead = "DumpRMHedgeStockSet()";
WriteRiskLog(" %s Begin...", logHead.c_str());
    UFC::AnsiString curStockNo;
    int stockCount = FRMHedgeStockSet.ItemCount();
    if (FRMHedgeStockSet.First(curStockNo) == TRUE)
    {
        int curLoopCount = 0;
        do 
        {
            curLoopCount++;
            WriteRiskLog(" %s %4d/%4d Hedge Stock[%s]", logHead.c_str(), curLoopCount, stockCount, curStockNo.c_str());
        } while(FRMHedgeStockSet.Next(curStockNo) == TRUE);
    }
WriteRiskLog(" %s End.", logHead.c_str());
}  //CMegaRiskManagerObject::DumpRMHedgeStockSet()
//----------------------------------------------------------------------------------------------------------------------
UFC::AnsiString CMegaRiskManagerObject::ComposeFullFileName(const UFC::AnsiString FilePath, const UFC::AnsiString FilePrefix, const UFC::AnsiString FileExtension, int FileSeqNo)
{
    UFC::AnsiString localFileName = "";
    if (FileSeqNo >= 0)
    {
        if (FileExtension.Length() > 0)
            localFileName.Printf("%s%03d.%s", FilePrefix.c_str(), FileSeqNo, FileExtension.c_str());
        else
            localFileName.Printf("%s%03d", FilePrefix.c_str(), FileSeqNo);
    }
    else
    {
        if (FileExtension.Length() > 0)
            localFileName.Printf("%s.%s", FilePrefix.c_str(), FileExtension.c_str());
        else
            localFileName.Printf("%s", FilePrefix.c_str());
    }
            
    UFC::AnsiString fullFileName = "";
    if (FilePath[FilePath.Length() - 1] != '/')
        fullFileName.Printf("%s/%s", FilePath.c_str(), localFileName.c_str());
    else
        fullFileName.Printf("%s%s", FilePath.c_str(), localFileName.c_str());
    return fullFileName;
}  //SCMegaRiskManagerObject::ComposeFullFileName()
//----------------------------------------------------------------------------------------------------------------------
bool CMegaRiskManagerObject::LoadOrderDeleteData()
{
    bool isFileExist = false;
    UFC::AnsiString logHead = "LoadOrderDeleteData()";
    UFC::AnsiString fullFileName = ComposeFullFileName(FRMFilePath, FRMDeleteFilePrefix, FRMDeleteFileExtension, FRMCurrentDeleteFileSeqNo);
    if (fullFileName.Length() <= 0)
    {
        WriteRiskLog(logHead, "No File Name.");
        FlushRiskLog();
        UFC::BufferedLog::Printf(" %s No File Name", logHead.c_str());
        return isFileExist;
    }
    
    UFC::AnsiString logMsg = "";
    if (UFC::FileExists(fullFileName) == FALSE)
    {
        logMsg.Printf("Order Delete File[%s] not Exist.", fullFileName.c_str());
        WriteLevelRiskLog(UFC::dlPerformance, logHead, logMsg.c_str());
        UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " %s %s", logHead.c_str(), logMsg.c_str());
        FlushRiskLog();
        return isFileExist;
    }  //if (UFC::FileExists(fullFileName) == FALSE)
    
    WriteRiskLog(logHead, "Load Order Delete File[%s].", fullFileName.c_str());
    isFileExist = true;
    UFC::AnsiString todayDateStr;
    UFC::GetYYYYMMDD(todayDateStr, FALSE);
    try 
    {
        UFC::FileStreamEx *delFilePtr = new UFC::FileStreamEx(fullFileName, "r", FALSE);
        UFC::AnsiString deleteDataStr = "";
        int deleteRecordCount = 0;
        int removeOrderCount = 0;
        int updateConditionCount = 0;
        BOOL isNotEndOfFile = FALSE;
        do {
            isNotEndOfFile = delFilePtr->ReadLine(deleteDataStr);
            int recordLength = deleteDataStr.Length();
            if (recordLength < 26) continue;
            deleteRecordCount++;
            UFC::AnsiString curDataDate = "";
            curDataDate = deleteDataStr.SubString(0, 8);
            curDataDate.TrimRight(' ');
            
            UFC::AnsiString curOrderId = "";
            curOrderId = deleteDataStr.SubString(8, 5);
            curOrderId.TrimRight(' ');
            
            UFC::AnsiString curBrokerId = "";
            curBrokerId = deleteDataStr.SubString(13, 4);
            curBrokerId.TrimRight(' ');
            
            UFC::AnsiString curDeleteQtyStr = "";
            int curDeleteQty = 0;
            curDeleteQtyStr = deleteDataStr.SubString(17, 9);
            curDeleteQty = curDeleteQtyStr.ToInt();
            
            if (NeedCheckRiskDataDate() && (curDataDate != todayDateStr))
            {
                WriteRiskLog(logHead, "%d Wrong Risk Data Date [%s].", deleteRecordCount, deleteDataStr.c_str());
                continue;
            }
            
            WriteLevelRiskLog(UFC::dlPerformance, logHead, "%d Delete Order[%s] BrokerId[%s] DelQty[%d] Date[%s] isNotEOF[%d].",
                              deleteRecordCount, curOrderId.c_str(), curBrokerId.c_str(), curDeleteQty, curDataDate.c_str(), isNotEndOfFile);
            CRMOrderSummary *targetOrderPtr = FindOrderFromIDIndex(curOrderId, UFC::rwLockForRead);
            if (targetOrderPtr == 0)
            {
                WriteRiskLog(logHead, "%d Order[%s] not Exist.", deleteRecordCount, curOrderId.c_str());
                continue;
            }
            
            targetOrderPtr->LockForRead();
            UFC::AnsiString curAccountNo = targetOrderPtr->GetAccountNo(UFC::rwNotLock);
            UFC::AnsiString curStockNo = targetOrderPtr->GetStockNo(UFC::rwNotLock);
            UFC::AnsiString curWarrantNo = targetOrderPtr->GetWarrantNo(UFC::rwNotLock);
            char curSide = targetOrderPtr->GetSide(UFC::rwNotLock);
            char curTWSEOrderType = targetOrderPtr->GetTWSEOrderType(UFC::rwNotLock);
            targetOrderPtr->UnlockForRead();

            CRMInvestorAccount *curAccountPtr = FindInvestorAccountFromNoIndex(curAccountNo, UFC::rwLockForRead);
            if (curAccountPtr == 0)
            {
                WriteRiskLog(logHead, "%d Order[%s] Account[%s] not Exist.", deleteRecordCount, curOrderId.c_str(), curAccountNo.c_str());
                continue;
            }  //if (curAccountPtr == 0)
            
            CRMStockTradingData *curStockDataPtr = curAccountPtr->FindStockData(curStockNo, UFC::rwLockForRead);
            if (curStockDataPtr == 0)
            {
                WriteRiskLog(logHead, "%d Account[%s] has no Stock[%s] data, Order[%s].", deleteRecordCount, curAccountNo.c_str(), curStockNo.c_str(), curOrderId.c_str());
                continue;
            }  //if (curStockDataPtr == 0)
            
            targetOrderPtr->SetLeavesQty(0, UFC::rwLockForWrite);
            CRMOrderSummary *cancelOrderPtr = 0;
            bool isNormalOrder = false;
            if (curSide == 'B')
            {
                cancelOrderPtr = curStockDataPtr->RemoveNormalBuyOrder(curOrderId, UFC::rwLockForWrite);
                if (cancelOrderPtr == 0) 
                    cancelOrderPtr = curStockDataPtr->RemoveBuyOrder(curOrderId, UFC::rwLockForWrite);
                else
                    isNormalOrder = true;
            }
            else if (curSide == 'S')
            {
                cancelOrderPtr = curStockDataPtr->RemoveNormalSellOrder(curOrderId, UFC::rwLockForWrite);
                if (cancelOrderPtr == 0) 
                    cancelOrderPtr = curStockDataPtr->RemoveSellOrder(curOrderId, UFC::rwLockForWrite);
                else
                    isNormalOrder = true;
            }
            
            if (cancelOrderPtr != 0)
            {
                removeOrderCount++;
                WriteRiskLog(logHead, "%d Stock: %s, Order[%s]", deleteRecordCount, curStockDataPtr->ToString(UFC::rwLockForRead).c_str(), curOrderId.c_str());
            }
            else
            {
                WriteRiskLog(logHead, "%d Remove Order[%s] from account[%s] Stock[%s] failed.", deleteRecordCount, curOrderId.c_str(), curAccountNo.c_str(), curStockNo.c_str());
                continue;
            }
            
            CRMMegaStockTradingData *curMegaStockDataPtr = static_cast<CRMMegaStockTradingData*>(curStockDataPtr);
            CRMBrokerStockCondition *curConditionPtr = curMegaStockDataPtr->FindBrokerStockCondition(curWarrantNo, curBrokerId, curTWSEOrderType, UFC::rwLockForRead);
            if (curConditionPtr != 0)
            {
                curConditionPtr->LockForWrite();
                if (curSide == 'B')
                {
                    if (isNormalOrder)
                    {
                        int curNormalOrderBuyQty = curConditionPtr->GetNormalOrderBuyQty(UFC::rwNotLock);
                        if (curNormalOrderBuyQty >= curDeleteQty)
                            curConditionPtr->IncreaseNormalOrderBuyQty(-curDeleteQty, UFC::rwNotLock);
                        else
                        {
                            curConditionPtr->SetNormalOrderBuyQty(0, UFC::rwNotLock);
                            WriteRiskLog(logHead, "%d NormalOrderBuyQty[%d] is Less then deleteQty[%d], set NormalOrderBuyQty to 0.", deleteRecordCount, curNormalOrderBuyQty, curDeleteQty);
                        }
                    }
                    else
                    {
                        int curOrderBuyQty = curConditionPtr->GetOrderBuyQty(UFC::rwNotLock);
                        if (curOrderBuyQty >= curDeleteQty)
                            curConditionPtr->IncreaseOrderBuyQty(-curDeleteQty, UFC::rwNotLock);
                        else
                        {
                            curConditionPtr->SetOrderBuyQty(0, UFC::rwNotLock);
                            WriteRiskLog(logHead, "%d OrderBuyQty[%d] is Less then deleteQty[%d], set OrderBuyQty to 0.", deleteRecordCount, curOrderBuyQty, curDeleteQty);
                        }
                    }
                }
                else
                {
                    if (isNormalOrder)
                    {
                        int curNormalOrderSellQty = curConditionPtr->GetNormalOrderSellQty(UFC::rwNotLock);
                        if (curNormalOrderSellQty >= curDeleteQty)
                            curConditionPtr->IncreaseNormalOrderSellQty(-curDeleteQty, UFC::rwNotLock);
                        else
                        {
                            curConditionPtr->SetNormalOrderSellQty(0, UFC::rwNotLock);
                            WriteRiskLog(logHead, "%d NormalOrderSellQty[%d] is Less then deleteQty[%d], set NormalOrderSellQty to 0.", deleteRecordCount, curNormalOrderSellQty, curDeleteQty);
                        }
                    }
                    else
                    {
                        int curOrderSellQty = curConditionPtr->GetOrderSellQty(UFC::rwNotLock);
                        if (curOrderSellQty >= curDeleteQty)
                            curConditionPtr->IncreaseOrderSellQty(-curDeleteQty, UFC::rwNotLock);
                        else
                        {
                            curConditionPtr->SetOrderSellQty(0, UFC::rwNotLock);                        }
                            WriteRiskLog(logHead, "%d OrderSellQty[%d] is Less then deleteQty[%d], set OrderSellQty to 0.", deleteRecordCount, curOrderSellQty, curDeleteQty);
                        }
                }
                updateConditionCount++;
                WriteRiskLog(logHead, "%d Condition: %s, order[%s]", deleteRecordCount, curConditionPtr->ToString(UFC::rwNotLock).c_str(), curOrderId.c_str());
                curConditionPtr->UnlockForWrite();
            }
            else
                WriteRiskLog(logHead, "%d Account[%s] Stock[%s][%s][%s]%c not found, Order[%s]", deleteRecordCount, curAccountNo.c_str(), curStockNo.c_str(), curWarrantNo.c_str(), curBrokerId.c_str(), curTWSEOrderType, curOrderId.c_str());
        } while(isNotEndOfFile == TRUE);
        logMsg.Printf("Process %d Record and remove %d Orders and update %d Conditions from File[%s].", deleteRecordCount, removeOrderCount, updateConditionCount, fullFileName.c_str());
    }
    catch(UFC::FileException& e)
    {
        logMsg.Printf("Process Order Delete File[%s] Exception:[%s].", fullFileName.c_str(), e.what());
        UFC::BufferedLog::Printf(" %s %s", logHead.c_str(), logMsg.c_str());
    }  //try
    
    WriteRiskLog(logHead, logMsg.c_str());
    WriteRiskLog(logHead, " ");
    FlushRiskLog();    
    return isFileExist;
}  //CMegaRiskManagerObject::LoadOrderDeleteData()
//----------------------------------------------------------------------------------------------------------------------
bool CMegaRiskManagerObject::LoadRiskManagerControlData(bool IsUpdate)
{
    bool isFileExist = false;
    UFC::AnsiString logHead = "";
    if (!IsUpdate)
        logHead = "LoadRiskManagerControlData()";
    else
        logHead = "LoadRiskManagerControlDataForUpdate()";
    UFC::AnsiString logMsg = "";
    UFC::AnsiString fullFileName = "";
    if (!IsUpdate)
        fullFileName = ComposeFullFileName(FRMFilePath, FRMControlFilePrefix, FRMControlFileExtension, -1);
    else
        fullFileName = ComposeFullFileName(FRMFilePath, FRMControlFilePrefix, FRMControlFileExtension, FRMCurrentControlFileSeqNo);
    
    if (fullFileName.Length() <= 0)
    {
        WriteRiskLog(logHead, "No File Name.");
        FlushRiskLog();
        UFC::BufferedLog::Printf(" %s No File Name", logHead.c_str());
        return isFileExist;
    }
    
    if (UFC::FileExists(fullFileName) == FALSE)
    {
        logMsg.Printf("Risk Control File[%s] Not Exist.", fullFileName.c_str());
        if ( !IsUpdate )
        {
            WriteRiskLog(logHead, logMsg.c_str());
            UFC::BufferedLog::Printf(" %s %s", logHead.c_str(), logMsg.c_str());
        }
        else
        {
            WriteLevelRiskLog(UFC::dlPerformance, logHead.c_str(), logMsg.c_str());
            UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " %s %s", logHead.c_str(), logMsg.c_str());
        }
        FlushRiskLog();
        return isFileExist;
    }  //if (UFC::FileExists(fullFileName) == FALSE)

    WriteRiskLog(logHead, "Load Risk Control File[%s].", fullFileName.c_str());
    isFileExist = true;
    UFC::AnsiString todayDateStr;
    UFC::GetYYYYMMDD(todayDateStr, FALSE);
    try 
    {
        UFC::FileStreamEx *ctrlFilePtr = new UFC::FileStreamEx(fullFileName, "r", FALSE);
        UFC::AnsiString stockDataStr = "";
        int stockRecordCount = 0;
        int updateCount = 0;
        BOOL isNotEndOfFile = FALSE;
        do {
            isNotEndOfFile = ctrlFilePtr->ReadLine(stockDataStr);
            int recordLength = stockDataStr.Length();
            if (recordLength < 60) 
            {
                WriteRiskLog(logHead, "%d [%s]%d.", stockRecordCount, stockDataStr.c_str(), recordLength);
                continue;
            }
            stockRecordCount++;
            UFC::AnsiString curDataDate = stockDataStr.SubString(0, 8);
                    
            UFC::AnsiString curStockNo = stockDataStr.SubString(8, 6);
            curStockNo.TrimRight(' ');
                    
            UFC::AnsiString curAvailableSellQtyStr = stockDataStr.SubString(30, 9);
            curAvailableSellQtyStr.TrimRight(' ');
            int curAvailableSellQty = 0;
            if (curAvailableSellQtyStr.Length() > 0) curAvailableSellQty = curAvailableSellQtyStr.ToInt();
                    
            UFC::AnsiString curAccountNo = stockDataStr.SubString(39, 7);
            curAccountNo.TrimRight(' ');
                    
            UFC::AnsiString curWarrantNo = stockDataStr.SubString(46, 6);
            curWarrantNo.TrimRight(' ');
                    
            UFC::AnsiString curBrokerId = stockDataStr.SubString(52, 4);
            curBrokerId.TrimRight(' ');
                    
            char curOrderType = stockDataStr[56];
                    
            UFC::AnsiString curAvailableSellShortBuyQtyStr = stockDataStr.SubString(57, 9);
            curAvailableSellShortBuyQtyStr.TrimRight(' ');
            int curAvailableSellShortBuyQty = 0;
            if (curAvailableSellShortBuyQtyStr.Length() > 0) curAvailableSellShortBuyQty = curAvailableSellShortBuyQtyStr.ToInt();
                                        
            if (NeedCheckRiskDataDate() && (curDataDate != todayDateStr))
            {
                WriteRiskLog(logHead, "%d Wrong Risk Data Date [%s].", stockRecordCount, stockDataStr.c_str());
                continue;
            }
                    
            WriteLevelRiskLog(UFC::dlPerformance, logHead, "%d Acc[%s] Stock[%s][%s][%s]%c AvailableQty[S:%s(%d) B:%s(%d)] Date[%s] isNotEOF[%d].",
                              stockRecordCount, 
                              curAccountNo.c_str(), curStockNo.c_str(), curWarrantNo.c_str(), curBrokerId.c_str(), curOrderType, 
                              curAvailableSellQtyStr.c_str(), curAvailableSellQty,
                              curAvailableSellShortBuyQtyStr.c_str(), curAvailableSellShortBuyQty, 
                              curDataDate.c_str(), isNotEndOfFile);

            CRMInvestorAccount *curAccountPtr = FindInvestorAccountFromNoIndex(curAccountNo, UFC::rwLockForRead);
            if (curAccountPtr == 0)
            {
                curAccountPtr = new CRMInvestorAccount(curAccountNo, 0, 0, 1);  //no DayTrade, not Check Self Trade, Check SellPositionCheck
                if (!InsertInvestorAccountToNoIndex(curAccountNo, curAccountPtr, cstNotCheck, UFC::rwLockForWrite))
                {
                    WriteRiskLog(logHead, "%d Insert Account[%s] failed.", stockRecordCount, curAccountNo.c_str());
                    delete curAccountPtr;
                    continue;
                }            
            }  //if (curAccountPtr == 0)

            CRMStockTradingData *curStockDataPtr = curAccountPtr->FindStockData(curStockNo, UFC::rwLockForRead);
            if (curStockDataPtr == 0)
            {
                curStockDataPtr = new CRMMegaStockTradingData(curAccountNo, curStockNo);
                if (!curAccountPtr->InsertStockData(curStockNo, curStockDataPtr, cstNotCheck, UFC::rwLockForWrite))
                {
                    WriteRiskLog(logHead, "%d Insert Stock[%s] data into Account[%s] failed.", stockRecordCount, curStockNo.c_str(), curAccountNo.c_str());
                    delete curStockDataPtr;
                    curStockDataPtr = 0;
                }
            }  //if (stockDataPtr == 0)
            
            if (curStockDataPtr == 0) 
            {
                WriteRiskLog(logHead, "%d Wrong Account[%s] Stock[%s].", stockRecordCount, curAccountNo.c_str(), curStockNo.c_str());
                continue;
            }

            CRMMegaStockTradingData *curMegaStockDataPtr = static_cast<CRMMegaStockTradingData*>(curStockDataPtr);
            CRMBrokerStockCondition *curConditionPtr = curMegaStockDataPtr->FindBrokerStockCondition(curWarrantNo, curBrokerId, curOrderType, UFC::rwLockForRead);
            if (curConditionPtr == 0)
            {
                bool isSellShort = false;
                if (curBrokerId != FRMMainBrokerID) isSellShort = true;
                CRMBrokerStockCondition *newConditionPtr = new CRMBrokerStockCondition(curAccountNo, curStockNo, curWarrantNo, curBrokerId, curOrderType, curAvailableSellQty, curAvailableSellShortBuyQty, curDataDate, isSellShort);
                if (curMegaStockDataPtr->InsertBrokerStockCondition(curWarrantNo, curBrokerId, curOrderType, newConditionPtr, cstNotCheck, UFC::rwLockForWrite))
                {
                    updateCount++;
                    logMsg = newConditionPtr->ToString(UFC::rwLockForRead);
                    WriteLevelRiskLog(UFC::dlInformation, logHead, logMsg.c_str());
                }
                else
                {
                    WriteRiskLog(logHead, "%d Insert StockCondition[%s][%s][%s]%c into Account[%s] failed.", stockRecordCount, curStockNo.c_str(), curWarrantNo.c_str(), curBrokerId.c_str(), curOrderType, curAccountNo.c_str());
                    delete newConditionPtr;
                }
            }
            else
            {
                if (IsUpdate)
                {
                    curConditionPtr->LockForWrite();
                    if (curAvailableSellQty != 0)
                    {
                        int oldAvailableSellQty = curConditionPtr->GetAvailableSellQty(UFC::rwNotLock);
                        int newAvailableSellQty = oldAvailableSellQty + curAvailableSellQty;
                        WriteLevelRiskLog(UFC::dlPerformance, logHead, "Set AvailableSellQty from %d to %d.", oldAvailableSellQty, newAvailableSellQty);
                        curConditionPtr->SetAvailableSellQty(newAvailableSellQty, UFC::rwNotLock);
                    }
                    
                    if (curAvailableSellShortBuyQty != 0)
                    {
                        int oldAvailableSellShortBuyQty = curConditionPtr->GetAvailableBuyQty(UFC::rwNotLock);
                        int newAvailableSellShortBuyQty = oldAvailableSellShortBuyQty + curAvailableSellShortBuyQty;
                        WriteLevelRiskLog(UFC::dlPerformance, logHead, "Set AvailableBuyQty from %d to %d.", oldAvailableSellShortBuyQty, newAvailableSellShortBuyQty);
                        curConditionPtr->SetAvailableBuyQty(newAvailableSellShortBuyQty, UFC::rwNotLock);
                    }
                    
                    if ((curAvailableSellQty != 0) || (curAvailableSellShortBuyQty != 0))
                    {
                        logMsg = curConditionPtr->ToString(UFC::rwNotLock);
                        updateCount++;
                        WriteRiskLog(logHead, logMsg.c_str());
                    }
                    curConditionPtr->UnlockForWrite();
                }
                else
                    WriteRiskLog(logHead, "%d Ignore Duplicate Account[%s] StockCondition[%s][%s][%s]%c.", stockRecordCount, curAccountNo.c_str(), curStockNo.c_str(), curWarrantNo.c_str(), curBrokerId.c_str(), curOrderType);
            }  //if (curConditionPtr == 0)
        } while(isNotEndOfFile == TRUE);
        
        delete ctrlFilePtr;
        if (!IsUpdate)
            logMsg.Printf("Process %d Record and Create %d Condition from File[%s].", stockRecordCount, updateCount, fullFileName.c_str());
        else
            logMsg.Printf("Process %d Record and Update %d Condition from File[%s].", stockRecordCount, updateCount, fullFileName.c_str());
    }
    catch(UFC::FileException& e)
    {
        logMsg.Printf("Process Risk Control File[%s] Exception:[%s].", fullFileName.c_str(), e.what());
        UFC::BufferedLog::Printf(" %s %s", logHead.c_str(), logMsg.c_str());
    }  //try
    
    WriteRiskLog(logHead, logMsg.c_str());
    WriteRiskLog(logHead, " ");
    FlushRiskLog();
    return isFileExist;
}  //SpeedyObjectsManager::LoadRiskManagerControlData()
//------------------------------------------------------------------------------
bool CMegaRiskManagerObject::LoadSetting()
{
    UFC::AnsiString logHead = "CMegaRiskManagerObject::LoadSettings()";
    UFC::AnsiString fullConfigureFileName;
    fullConfigureFileName.Printf("../cfg/%s", FConfigureFileName.c_str());
    UFC::UiniFile *cfgFilePtr = 0;
    try
    {
        cfgFilePtr = new UFC::UiniFile(fullConfigureFileName, FALSE);
    }
    catch (UFC::FileException& e)
    {
        UFC::BufferedLog::Printf(" %s Open ConfigureFile[%s] Failed:%s", logHead.c_str(), fullConfigureFileName.c_str(), e.what());
        if (cfgFilePtr != 0) delete cfgFilePtr;
        return false;
    }

    UFC::AnsiString settingSectionName = "Setting";
    UFC::Section *settingSecPtr = cfgFilePtr->GetSection(settingSectionName);
    if (settingSecPtr == 0)
    {
        FIsRMControlSetting = false;
        UFC::BufferedLog::Printf(" %s Section[%s] Not Exist.", logHead.c_str(), settingSectionName.c_str() );        
        return false;
    }
    
    FIsRMControlSetting = true;
    UFC::AnsiString fieldName, fieldValue;
    fieldName = "Name";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FRMName = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value[%s]", logHead.c_str(), fieldName.c_str(), FRMName.c_str());
    
    fieldName = "RMFilePath";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FRMFilePath = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value[%s]", logHead.c_str(), fieldName.c_str(), FRMFilePath.c_str());

    fieldName = "RMControlFilePrefix";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FRMControlFilePrefix = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value[%s]", logHead.c_str(), fieldName.c_str(), FRMControlFilePrefix.c_str() );
    
    fieldName = "RMControlFileExtension";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FRMControlFileExtension = fieldValue;
    else
    {
        if (FRMControlFileExtension.Length() > 0)
            UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value[%s]", logHead.c_str(), fieldName.c_str(), FRMControlFileExtension.c_str() );
        else
            UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value[]", logHead.c_str(), fieldName.c_str() );
    }

    fieldName = "RMDeleteFilePrefix";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FRMDeleteFilePrefix = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value[%s]", logHead.c_str(), fieldName.c_str(), FRMDeleteFilePrefix.c_str() );
    
    fieldName = "RMDeleteFileExtension";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FRMDeleteFileExtension = fieldValue;
    else
    {
        if (FRMDeleteFileExtension.Length() > 0)
            UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value[%s]", logHead.c_str(), fieldName.c_str(), FRMDeleteFileExtension.c_str() );
        else
            UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value[]", logHead.c_str(), fieldName.c_str() );
    }
    
    fieldName = "MainBrokerId";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FRMMainBrokerID = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value[%s]", logHead.c_str(), fieldName.c_str(), FRMMainBrokerID.c_str() );
    
    fieldName = "CheckSelfTrading";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
    {
        if ((fieldValue == "Y") || (fieldValue == "y")) 
            FNeedCheckSelfTrading = true;
        else
            FNeedCheckSelfTrading = false;
    }
    else
        UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Use Default Value [%s]", logHead.c_str(), fieldName.c_str(), DescribeBoolean(FNeedCheckSelfTrading).c_str() );

    fieldName = "AccountStockAmountLimit";
    if (settingSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FRMAccountStockAmountLimit = fieldValue.ToDouble();
    else
        UFC::BufferedLog::Printf(" %s Field[%s] Not Exist. Do not Check Account Stock Amount Limit.", logHead.c_str(), fieldName.c_str() );
    
    UFC::AnsiString investorAccountSectionName = "InvestorAccount";
    UFC::Section* investorAccountSecPtr = cfgFilePtr->GetSection(investorAccountSectionName);
    if (investorAccountSecPtr == 0)
    {
        UFC::BufferedLog::Printf(" %s Section[%s] Not Exist.", logHead.c_str(), investorAccountSectionName.c_str());
        return false;
    }
    
    ClearInvestorAccountNoIndex(cstDelete, UFC::rwLockForWrite);
    for (int i = 0; i < investorAccountSecPtr->ItemCount(); i++)
    {
        if (investorAccountSecPtr->GetNameValue(i, fieldName, fieldValue) == FALSE) continue;
        int dayTradeType = 0;   //no DayTrade
        int selfTradeType = 0;  //not Check Self Trade
        int sellPositionCheckType = 1;  //Check Position
        UFC::PStringList valueList;
        valueList.SetStrings(fieldValue, ",\n");
        if (valueList.ItemCount() >= 1) dayTradeType = valueList[0].ToInt();
        if (valueList.ItemCount() >= 2) selfTradeType = valueList[1].ToInt();
        if (valueList.ItemCount() >= 3) sellPositionCheckType = valueList[2].ToInt();
        CRMInvestorAccount *accountPtr = new CRMInvestorAccount(fieldName, dayTradeType, selfTradeType, sellPositionCheckType);
        if (!InsertInvestorAccountToNoIndex(fieldName, accountPtr, cstCheck, UFC::rwLockForWrite))
        {
            UFC::BufferedLog::Printf(" %s Insert Account[%s] dayTrade[%d] selfTrade[%d] failed.", logHead.c_str(), fieldName.c_str(), dayTradeType, selfTradeType);
            delete accountPtr;
        }
    }  //for (int i = 0; i < investorAccountSecPtr->ItemCount(); i++)

    LoadRiskManagerControlData(false);  //Not for Update
    while (LoadRiskManagerControlData(true))
        FRMCurrentControlFileSeqNo++;
 
    if (cfgFilePtr != 0) delete cfgFilePtr;
    return true;
}  //CMegaRiskManagerObject::LoadSetting()
//------------------------------------------------------------------------------
void CMegaRiskManagerObject::PrintSetting()
{
    WriteRiskLog("", "---------- %s RiskManager ----------", FRMName.c_str());
    if (FNeedCheckSelfTrading) WriteRiskLog("", "---------- Need Check Self Trading ----------" );
    
    WriteRiskLog("", "Risk Manager File Path:[%s]", FRMFilePath.c_str());
    if (FRMControlFileExtension.Length() > 0)
        WriteRiskLog("", "Risk Manager Control File Prefix[%s] Extension[%s]", FRMControlFilePrefix.c_str(), FRMControlFileExtension.c_str());
    else
        WriteRiskLog("", "Risk Manager Control File Prefix[%s] Extension[]", FRMControlFilePrefix.c_str());
        
    if (FRMDeleteFileExtension.Length() > 0)
        WriteRiskLog("", "Risk Manager Delete File Prefix[%s] Extension[%s]", FRMDeleteFilePrefix.c_str(), FRMDeleteFileExtension.c_str());
    else
        WriteRiskLog("", "Risk Manager Delete File Prefix[%s] Extension[]", FRMDeleteFilePrefix.c_str());
    
    WriteRiskLog("", "Risk Manager Hedge Stock FileName[%s]", FRMHedgeStockFileName.c_str());
    WriteRiskLog("", "Broker ID[%s]", FRMMainBrokerID.c_str());
    if ( FRMAccountStockAmountLimit > 0.0 ) WriteRiskLog("", "Account Stock Amount Limit [%16.1lf]", FRMAccountStockAmountLimit );
    DumpInvestorAccount(cstFlush, UFC::rwLockForRead);
}  //CMegaRiskManagerObject::PrintSetting()
//---------------------------------------------------------------------------------------------------------------------
bool CMegaRiskManagerObject::CheckRiskCondition(TWSE::MessageHeader*& Msg, OrderRecord& OI, const UFC::AnsiString& PVCID, UFC::AnsiString& CheckMessage)
{
    int functionCode = OI.FuncCode;
    UFC::AnsiString orderKey = OI.Key;
    TWSE::TMessageType msgType = static_cast<TWSE::TMessageType>(OI.MsgType);
    if (((msgType != TWSE::mtT010) && (msgType != TWSE::mtO010) && (msgType != TWSE::mtP010)) || 
        ((functionCode != 1) && (functionCode != 2))) 
        return true;

    CheckMessage = "";
    UFC::AnsiString logHead;
    logHead.Printf("CheckRiskCondition() PVC:%s", PVCID.c_str());
    UFC::AnsiString warrantNo = "";
    UFC::AnsiString brokerId = "";
    ParseRMDataFromUserData(orderKey, warrantNo, brokerId);
    
    UFC::AnsiString orderId, accountNo, stockNo, orderQtyStr, priceStr;
    char buySellCode = ' ', exchangeCode = ' ', twseOrderType = ' ';
    char twsePriceType = ' ', twseTimeInForce = ' ';
    double orderPrice = 0.0;
    if (msgType == TWSE::mtT010)  //Normal
    {
        TWSE::MessageT010 *t010MsgPtr = (TWSE::MessageT010*)Msg;
        orderId = UFC::AnsiString(t010MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(t010MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(t010MsgPtr->StockID, 6);
        buySellCode = t010MsgPtr->BuySellCode;
        exchangeCode = t010MsgPtr->ExchangeCode;
        twseOrderType = t010MsgPtr->TWSEOrderType;
        if( TWSE::MessageHeader::FNewVersion == TRUE )
        {
            orderQtyStr = UFC::AnsiString(t010MsgPtr->OrderQty, 6);
            priceStr = UFC::AnsiString(t010MsgPtr->OrderPrice, 9);
            orderPrice = Parse95v9999PriceStr(priceStr);
            twsePriceType = t010MsgPtr->OrderType;      //'1':Market '2':Limit
            twseTimeInForce = t010MsgPtr->TimeInForce;  //'0':ROD '3':IOC '4':FOK
        }
        else
        {
            orderQtyStr = UFC::AnsiString(t010MsgPtr->OrderQty, 3);
            priceStr = UFC::AnsiString(t010MsgPtr->OrderPrice, 6);
            orderPrice = Parse94v99PriceStr(priceStr);
            twsePriceType = '2';    //Limit
            twseTimeInForce = '0';  //ROD
        }
    }
    else if (msgType == TWSE::mtO010)  //Odd
    {
        TWSE::MessageO010 *o010MsgPtr = (TWSE::MessageO010*)Msg;
        orderId = UFC::AnsiString(o010MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(o010MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(o010MsgPtr->StockID, 6);
        buySellCode = o010MsgPtr->BuySellCode;
        exchangeCode = '2';     //Odd
        twseOrderType = '0';
        twsePriceType = '2';    //Limit
        twseTimeInForce = '0';  //ROD
        if( TWSE::MessageHeader::FNewVersion == TRUE )
        {
            orderQtyStr = UFC::AnsiString(o010MsgPtr->OrderQty, 6);
            priceStr = UFC::AnsiString(o010MsgPtr->OrderPrice, 9);
            orderPrice = Parse95v9999PriceStr(priceStr);
        }
        else
        {
            orderQtyStr = UFC::AnsiString(o010MsgPtr->OrderQty, 3);
            priceStr = UFC::AnsiString(o010MsgPtr->OrderPrice, 6);
            orderPrice = Parse94v99PriceStr(priceStr);
        }
    }
    else if (msgType == TWSE::mtP010)  //OffHour
    {
        TWSE::MessageP010 *p010MsgPtr = (TWSE::MessageP010*)Msg;
        orderId = UFC::AnsiString(p010MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(p010MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(p010MsgPtr->StockID, 6);
        buySellCode = p010MsgPtr->BuySellCode;
        exchangeCode = p010MsgPtr->ExchangeCode;
        twseOrderType = p010MsgPtr->TWSEOrderType;
        twsePriceType = '2';    //Limit
        twseTimeInForce = '0';  //ROD
        if( TWSE::MessageHeader::FNewVersion == TRUE )
        {
            orderQtyStr = UFC::AnsiString(p010MsgPtr->OrderQty, 6);
            priceStr = UFC::AnsiString(p010MsgPtr->OrderPrice, 9);
            orderPrice = Parse95v9999PriceStr(priceStr);
        }
        else
        {
            orderQtyStr = UFC::AnsiString(p010MsgPtr->OrderQty, 3);
            priceStr = UFC::AnsiString(p010MsgPtr->OrderPrice, 6);
            orderPrice = Parse94v99PriceStr(priceStr);
        }
    }
    accountNo.TrimRight(' ');
    stockNo.TrimRight(' ');
    int orderQty = orderQtyStr.ToInt();
    if (exchangeCode != '2') orderQty *= 1000;
    double orderAmount = static_cast<double>(orderQty) * orderPrice;
    WriteLevelRiskLog(UFC::dlInformation, logHead, "Check Order[%s] Stock[%s][%s][%s]%c %c Qty[%d] Px[%11.4lf] Amt[%15.4lf] func[%d] ExecCode[%c] Acc[%s] msgType[%d] PxType[%c] TIF[%c]", 
                      orderId.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerId.c_str(), twseOrderType, buySellCode, orderQty, orderPrice, orderAmount, functionCode, exchangeCode, accountNo.c_str(), msgType, twsePriceType, twseTimeInForce);
    
    CRMInvestorAccount *accountPtr = FindInvestorAccountFromNoIndex(accountNo, UFC::rwLockForRead);
    if (accountPtr == 0)
    {
        CheckMessage.Printf("Reject[%s]: Account[%s] not found", orderId.c_str(), accountNo.c_str());
        WriteRiskLog(logHead, CheckMessage);
        FlushRiskLog();
        return false;
    }  //if (accountPtr == 0)
    
    CRMStockTradingData *stockDataPtr = accountPtr->FindStockData(stockNo, UFC::rwLockForRead);
    if (stockDataPtr == 0)
    {
        stockDataPtr = new CRMMegaStockTradingData(accountNo, stockNo);
        if (!accountPtr->InsertStockData(stockNo, stockDataPtr, cstNotCheck, UFC::rwLockForWrite))
        {
            WriteRiskLog(logHead, "Insert Stock[%s] data into Account[%s] failed.", stockNo.c_str(), accountNo.c_str());
            delete stockDataPtr;
            stockDataPtr = 0;
        }
    }  //if (stockDataPtr == 0)
    
    if (stockDataPtr == 0)
    {
        CheckMessage.Printf("Reject[%s]: Wrong Account[%s] Stock[%s]", orderId.c_str(), accountNo.c_str(), stockNo.c_str());
        WriteRiskLog(logHead, CheckMessage);
        FlushRiskLog();
        return false;
    }
        
    if ((buySellCode != 'B') && (buySellCode != 'S'))
    {
        CheckMessage.Printf("Reject[%s]: Wrong Side[%c]", orderId.c_str(), buySellCode);
        WriteRiskLog(logHead, CheckMessage);
        FlushRiskLog();
        return false;
    }
    
    if (FRMAccountStockAmountLimit > 0.0)
    {
        double stkBuyAmount = stockDataPtr->GetBuyAmount(UFC::rwLockForRead);
        double stkSellAmount = stockDataPtr->GetSellAmount(UFC::rwLockForRead);
        double usedAmount = 0.0;
        UFC::AnsiString usedAmountStr = "";
        if (buySellCode == 'B')
        {
            usedAmount = stkBuyAmount - stkSellAmount;
            usedAmountStr.Printf("UsedAmount(STKSellAmount[%15.4lf] - STKBuyAmount[%15.4lf])", stkBuyAmount, stkSellAmount);
        }
        else
        {
            usedAmount = stkSellAmount - stkBuyAmount;
            usedAmountStr.Printf("UsedAmount(STKSellAmount[%15.4lf] - STKBuyAmount[%15.4lf])", stkSellAmount, stkBuyAmount);
        }
        
        if ((usedAmount + orderAmount) > FRMAccountStockAmountLimit)
        {
            CheckMessage.Printf("Reject[%s]: %s + OrderAmount[%15.4lf] > AccountStockAmountLimit[%15.4lf]", 
                                orderId.c_str(), usedAmountStr.c_str(), orderAmount, FRMAccountStockAmountLimit);
            WriteRiskLog(logHead, "%s, MsgType[%d]", CheckMessage.c_str(), msgType);        
            FlushRiskLog();
            return false;
        }
    }  //if (rmAccountStockAmountLimit > 0.0)
    
    bool isPassed = true;
    int selfTradeType = accountPtr->GetSelfTradeType(UFC::rwLockForRead);
    if (NeedCheckSelfTrading() && (selfTradeType == 1))
    {
        if (buySellCode == 'B')
        {
            double minSellOrderPrice = 0.0;
            if (msgType == TWSE::mtT010)
                minSellOrderPrice = stockDataPtr->GetMinNormalSellOrderPrice(UFC::rwLockForRead);
            else
                minSellOrderPrice = stockDataPtr->GetMinSellOrderPrice(UFC::rwLockForRead);
            if (orderPrice < minSellOrderPrice)
                WriteLevelRiskLog(UFC::dlPerformance, logHead, "[%s] Passed: Px[%11.3lf] < MinSellOrderPx[%11.3lf]", orderId.c_str(), orderPrice, minSellOrderPrice);
            else
            {
                CheckMessage.Printf("Reject[%s]: Px[%11.3lf] >= MinSellOrderPx[%11.3lf]", orderId.c_str(), orderPrice, minSellOrderPrice);
                isPassed = false;
            }
        }
        else if (buySellCode == 'S')
        {
            double maxBuyOrderPrice = 0.0;
            if (msgType == TWSE::mtT010)
                maxBuyOrderPrice = stockDataPtr->GetMaxNormalBuyOrderPrice(UFC::rwLockForRead);
            else
                maxBuyOrderPrice = stockDataPtr->GetMaxBuyOrderPrice(UFC::rwLockForRead);
            
            if (orderPrice > maxBuyOrderPrice)
                WriteLevelRiskLog(UFC::dlPerformance, logHead, "[%s] Passed: Px[%11.3lf] > MinBuyOrderPx[%11.3lf]", orderId.c_str(), orderPrice, maxBuyOrderPrice);
            else
            {
                CheckMessage.Printf("Reject[%s]: Px[%11.3lf] <= MaxBuyOrderPx[%11.3lf]", orderId.c_str(), orderPrice, maxBuyOrderPrice);
                isPassed = false;
            }
        }
        else
        {
            CheckMessage.Printf("Reject[%s]: Wrong Side[%c]", orderId.c_str(), buySellCode);
            isPassed = false;            
        }  //if (buySellCode == 'B')
    }  //if (ObjectManager->NeedCheckSelfTrading(UFC::rwLockForRead) && (selfTradeType == 1))
    
    if (!isPassed) 
    {
        WriteRiskLog(logHead, "%s, MsgType[%d]", CheckMessage.c_str(), msgType);        
        FlushRiskLog();
        return isPassed;
    }
    
    int availableSellQty = 0;  //可賣股數
    int availableBuyQty  = 0;  //可買股數
    bool isSellShort = brokerId != GetRMMainBrokerID();  //是否為借券
    CRMMegaStockTradingData *megaStockDataPtr = static_cast<CRMMegaStockTradingData*>(stockDataPtr);
    CRMBrokerStockCondition *conditionPtr = megaStockDataPtr->FindBrokerStockCondition(warrantNo, brokerId, twseOrderType, UFC::rwLockForRead);
    if (conditionPtr == 0)
    {
        if (!isSellShort)
        {
WriteRiskLog(logHead, "Sell Condition Account[%s] Stock[%s] WarrantNo[%s] BrokerId[%s] orderType[%c] not Found.", accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerId.c_str(), twseOrderType);
            UFC::AnsiString todayStr;
            UFC::GetYYYYMMDD(todayStr, FALSE);
            conditionPtr = new CRMBrokerStockCondition(accountNo, stockNo, warrantNo, brokerId, twseOrderType, availableSellQty, availableBuyQty, todayStr, isSellShort);
            if (!megaStockDataPtr->InsertBrokerStockCondition(warrantNo, brokerId, twseOrderType, conditionPtr, cstNotCheck, UFC::rwLockForWrite))
            {
                CheckMessage.Printf("Reject[%s]: Insert Account[%s] Stock[%s][%s][%s]%c condition failed", orderId.c_str(), accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerId.c_str(), twseOrderType);
                delete conditionPtr;
                conditionPtr = 0;
            }
        }
        else
            CheckMessage.Printf("Reject[%s]: Account[%s] Stock[%s][%s][%s]%c not found", orderId.c_str(), accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerId.c_str(), twseOrderType);
        
        if (conditionPtr == 0) 
        {
            WriteRiskLog(logHead, "%s, MsgType[%d]", CheckMessage.c_str(), msgType);        
            FlushRiskLog();
            return false;
        }
    }  //if (conditionPtr == 0)
    
    conditionPtr->LockForRead();
    availableSellQty = conditionPtr->GetAvailableSellQty(UFC::rwNotLock);          //可賣股數
    availableBuyQty = conditionPtr->GetAvailableBuyQty(UFC::rwNotLock);            //可買股數
    isSellShort = conditionPtr->IsSellShort(UFC::rwNotLock);                       //是否為借券
    int normalMatchBuyQty = conditionPtr->GetNormalMatchBuyQty(UFC::rwNotLock);    //今日一般成交買進股數
    int normalMatchSellQty = conditionPtr->GetNormalMatchSellQty(UFC::rwNotLock);  //今日一般成交賣出股數
    int matchBuyQty = conditionPtr->GetMatchBuyQty(UFC::rwNotLock);                //今日成交買進股數
    //int matchSellQty = conditionPtr->GetMatchSellQty(UFC::rwNotLock);              //今日成交賣出股數
    int normalOrderBuyQty = conditionPtr->GetNormalOrderBuyQty(UFC::rwNotLock);    //今日一般委託買進股數
    int normalOrderSellQty = conditionPtr->GetNormalOrderSellQty(UFC::rwNotLock);  //今日一般委託賣出股數
    int orderBuyQty = conditionPtr->GetOrderBuyQty(UFC::rwNotLock);                //今日委託買進股數
    int orderSellQty = conditionPtr->GetOrderSellQty(UFC::rwNotLock);              //今日委託賣出股數
WriteRiskLog(logHead, "availableSellQty[%d] normalMatchSellQty[%d] normalMatchBuyQty[%d] matchBuyQty[%d] normalOrderSellQty[%d] orderSellQty[%d]",
                       availableSellQty, normalMatchSellQty, normalMatchBuyQty, matchBuyQty, normalOrderSellQty, orderSellQty);
WriteRiskLog(logHead, "Condition: %s, Order[%s], MsgType[%d]", conditionPtr->ToString(UFC::rwNotLock).c_str(), orderId.c_str(), OI.MsgType);
    conditionPtr->UnlockForRead();
        
    if (functionCode == 1)  // New Buy Order
    {
        if (isSellShort)
        {
            int totalAvailableBuyQty = 0;
            if (msgType == TWSE::mtT010)
                totalAvailableBuyQty = availableBuyQty - normalOrderBuyQty;
            else
                totalAvailableBuyQty = availableBuyQty - normalMatchBuyQty - orderBuyQty;
            if (totalAvailableBuyQty < 0) totalAvailableBuyQty = 0;
            if (orderQty > totalAvailableBuyQty)
            {
                CheckMessage.Printf("Reject[%s]: OrderQty[%d] > Available Buy Qty[%d]", orderId.c_str(), orderQty, totalAvailableBuyQty);
                isPassed = false;                            
            }
            else
                WriteLevelRiskLog(UFC::dlPerformance, logHead, "[%s] Passed: OrderQty[%d] <= Available Buy Qty[%d]", orderId.c_str(), orderQty, totalAvailableBuyQty);
        }
        else
            WriteLevelRiskLog(UFC::dlPerformance, logHead, "[%s] Passed: Do not check buy order", orderId.c_str());
    }
    else  // New Sell Order(Function Code = 2)
    {            
        if (!isSellShort && (twseOrderType == '6'))
        {
            CheckMessage.Printf("Reject[%s]: Wrong OrderType[%c]", orderId.c_str(), twseOrderType);
            isPassed = false;            
        }
        else
        {
            if (accountPtr->GetSellPositionCheckType(UFC::rwLockForRead) == 1)  //Check Sell Position
            {
                int totalAvailableSellQty = availableSellQty;
                if (msgType != TWSE::mtT010) totalAvailableSellQty -= normalMatchSellQty;
                int dayTradeType = accountPtr->GetDayTradeType(UFC::rwLockForRead);
                if (accountNo == "8888888")
                {
                    if (dayTradeType == 1) 
                    {
                        if (IsRMHedgeStockExists(stockNo))
                            WriteLevelRiskLog(UFC::dlInformation, logHead, "[%s] is Hedge Stock, Account[%s] Buy/Sell Day Trade Enabled.", stockNo.c_str(), accountNo.c_str());
                        else
                        {
                            WriteLevelRiskLog(UFC::dlInformation, logHead, "[%s] is not Hedge Stock, Account[%s] Buy/Sell Day Trade Disabled.", stockNo.c_str(), accountNo.c_str());
                            dayTradeType = 0;
                        }
                    }  //if (dayTradeType == 1)
                }
                
                if (!isSellShort && (dayTradeType == 1))  //Can Sell Buy Today Qty
                {
                    totalAvailableSellQty += normalMatchBuyQty;
                    if (msgType != TWSE::mtT010) totalAvailableSellQty += matchBuyQty;
                }
                if (msgType == TWSE::mtT010)
                    totalAvailableSellQty -= normalOrderSellQty;
                else
                    totalAvailableSellQty -= orderSellQty;
                
                if (totalAvailableSellQty < 0) totalAvailableSellQty = 0;
                if (orderQty > totalAvailableSellQty)
                {
WriteRiskLog(logHead, "availableSellQty[%d] normalMatchSellQty[%d] normalMatchBuyQty[%d] matchBuyQty[%d] normalOrderSellQty[%d] orderSellQty[%d]", availableSellQty, normalMatchSellQty, normalMatchBuyQty, matchBuyQty, normalOrderSellQty, orderSellQty);
                    if (isSellShort)
                        CheckMessage.Printf("Reject[%s]: OrderQty[%d] > Available SellShort Qty[%d]", orderId.c_str(), orderQty, totalAvailableSellQty);
                    else
                        CheckMessage.Printf("Reject[%s]: OrderQty[%d] > Available Sell Qty[%d]", orderId.c_str(), orderQty, totalAvailableSellQty);
                    isPassed = false;            
                }
                else
                    WriteLevelRiskLog(UFC::dlPerformance, logHead, "[%s] Passed: OrderQty[%d] <= Available Sell Qty[%d]", orderId.c_str(), orderQty, totalAvailableSellQty);
            }
            else
                WriteRiskLog(logHead, " Account[%s] need not Check Sell Position.", accountNo.c_str());
        }  //if (!isSellShort && (orderType == '6'))
    }  //if (functionCode == 1)
    if (!isPassed) WriteRiskLog(logHead, CheckMessage);

    if (isPassed)
    {
        CRMOrderSummary *orderPtr = new CRMOrderSummary(orderId, buySellCode, stockNo, warrantNo, brokerId, twseOrderType, orderPrice, orderQty, exchangeCode, msgType, accountNo);
        if (!InsertOrderIntoIDIndex(orderId, orderPtr, UFC::rwLockForWrite))
            WriteRiskLog(logHead, "Insert Order[%s] into Object Manager failed.", orderId.c_str());
        
        bool isInsertOrderSuccess = false;
        if (functionCode == 1)
        {
            if (msgType == TWSE::mtT010)
            {
                conditionPtr->IncreaseNormalOrderBuyQty(orderQty, UFC::rwLockForWrite);
                isInsertOrderSuccess = stockDataPtr->InsertNormalBuyOrder(orderId, orderPtr, cstCheck, UFC::rwLockForWrite);
            }
            else
            {
                conditionPtr->IncreaseOrderBuyQty(orderQty, UFC::rwLockForWrite);
                isInsertOrderSuccess = stockDataPtr->InsertBuyOrder(orderId, orderPtr, cstCheck, UFC::rwLockForWrite);
            }
            
            if (isInsertOrderSuccess) 
            {
                double newBuyAmount = stockDataPtr->GetBuyAmount(UFC::rwLockForRead) + orderAmount;
                stockDataPtr->SetBuyAmount(newBuyAmount, UFC::rwLockForWrite);
            }
        }
        else
        {
            if (msgType == TWSE::mtT010)
            {
                conditionPtr->IncreaseNormalOrderSellQty(orderQty, UFC::rwLockForWrite);
                isInsertOrderSuccess = stockDataPtr->InsertNormalSellOrder(orderId, orderPtr, cstCheck, UFC::rwLockForWrite);
            }
            else
            {
                conditionPtr->IncreaseOrderSellQty(orderQty, UFC::rwLockForWrite);
                isInsertOrderSuccess = stockDataPtr->InsertSellOrder(orderId, orderPtr, cstCheck, UFC::rwLockForWrite);
            }

            if (isInsertOrderSuccess) 
            {
                double newSellAmount = stockDataPtr->GetSellAmount(UFC::rwLockForRead) + orderAmount;
                stockDataPtr->SetSellAmount(newSellAmount, UFC::rwLockForWrite);
            }
        }
        WriteLevelRiskLog(UFC::dlInformation, logHead, "Condition: %s, Order[%s], MsgType[%d]", conditionPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str(), OI.MsgType);
        
        if (isInsertOrderSuccess)
            WriteLevelRiskLog(UFC::dlInformation, logHead, "Stock: %s, Order[%s], MsgType[%d]", stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str(), OI.MsgType);
        else
            WriteRiskLog(logHead, "Insert Order[%s] into Stock[%s] Data failed.", orderId.c_str(), stockNo.c_str());
    }  //if (isPassed)
    FlushRiskLog();
    return isPassed;
}  //CMegaRiskManagerObject::CheckRiskCondition()
//------------------------------------------------------------------------------
void CMegaRiskManagerObject::AddRiskMatchQty(TWSE::R3Body *R3MsgBodyPtr, const UFC::AnsiString& PVCId, const UFC::AnsiString& ExecKey)
{
    UFC::AnsiString logHead;
    if (PVCId.Length() > 0)
        logHead.Printf("AddRiskMatchQty() PVC:%s", PVCId.c_str());
    else
        logHead.Printf("AddRiskMatchQty()");
    UFC::AnsiString logMsg = "";
    
    UFC::AnsiString orderId(R3MsgBodyPtr->FOrderNo, 5);
    char exchangeCode = R3MsgBodyPtr->FEXCD;
    
    UFC::AnsiString matchQtyStr(R3MsgBodyPtr->FQuntity, 8);
    int matchQty = matchQtyStr.ToInt();
    if (exchangeCode != '2') matchQty *= 1000;
    WriteLevelRiskLog(UFC::dlPerformance, logHead, "Process Match Order[%s] matchQty=%d", orderId.c_str(), matchQty);
    
    CRMOrderSummary *orderPtr = FindOrderFromIDIndex(orderId, UFC::rwLockForRead);
    if (orderPtr != 0)
    {
        orderPtr->LockForRead();
        UFC::AnsiString accountNo = orderPtr->GetAccountNo(UFC::rwNotLock);
        UFC::AnsiString stockNo = orderPtr->GetStockNo(UFC::rwNotLock);
        UFC::AnsiString warrantNo = orderPtr->GetWarrantNo(UFC::rwNotLock);
        UFC::AnsiString brokerId = orderPtr->GetBrokerId(UFC::rwNotLock);
        char side = orderPtr->GetSide(UFC::rwNotLock);
        char twseOrderType = orderPtr->GetTWSEOrderType(UFC::rwNotLock);        
        int origLeavesQty = orderPtr->GetLeavesOrderQty(UFC::rwNotLock);
        TWSE::TMessageType msgType = orderPtr->GetMsgType(UFC::rwNotLock);
        orderPtr->UnlockForRead();
        
        if (origLeavesQty > 0)
        {
            if (origLeavesQty >= matchQty)
            {
                int newLeavesQty = origLeavesQty - matchQty;
                orderPtr->SetLeavesQty(newLeavesQty, UFC::rwLockForWrite);
                CRMInvestorAccount *accountPtr = FindInvestorAccountFromNoIndex(accountNo, UFC::rwLockForRead);
                if (accountPtr != 0)
                {
                    CRMStockTradingData *stockDataPtr = accountPtr->FindStockData(stockNo, UFC::rwLockForRead);
                    if (stockDataPtr != 0)
                    {
                        if (newLeavesQty <= 0)
                        {
                            CRMOrderSummary *filledOrderPtr = 0;
                            if (side == 'B')
                            {
                                if (msgType == TWSE::mtT010)
                                    filledOrderPtr = stockDataPtr->RemoveNormalBuyOrder(orderId, UFC::rwLockForWrite);
                                else
                                    filledOrderPtr = stockDataPtr->RemoveBuyOrder(orderId, UFC::rwLockForWrite);
                            }
                            else
                            {
                                if (msgType == TWSE::mtT010)
                                    filledOrderPtr = stockDataPtr->RemoveNormalSellOrder(orderId, UFC::rwLockForWrite);
                                else
                                    filledOrderPtr = stockDataPtr->RemoveSellOrder(orderId, UFC::rwLockForWrite);
                            }
                    
                            if (filledOrderPtr != 0)
                                WriteLevelRiskLog(UFC::dlInformation, logHead, "StockData: %s, Match Order[%s] Qty[%d], ExecKey[%s]", stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str(), matchQty, ExecKey.c_str());
                            else
                                WriteRiskLog(logHead, "Remove Order[%s] from account[%s] Stock[%s] failed, Match Order[%s], ExecKey[%s]", orderId.c_str(), accountNo.c_str(), stockNo.c_str(), orderId.c_str(), ExecKey.c_str());
                        }  //if (newLeavesQty <= 0)
                    
                       CRMMegaStockTradingData *megaStockDataPtr = static_cast<CRMMegaStockTradingData*>(stockDataPtr);
                       CRMBrokerStockCondition *conditionPtr = megaStockDataPtr->FindBrokerStockCondition(warrantNo, brokerId, twseOrderType, UFC::rwLockForRead);
                        if (conditionPtr != 0)
                        {
                            if (side == 'B')
                            {
                                if (msgType == TWSE::mtT010)
                                    conditionPtr->IncreaseNormalMatchBuyQty(matchQty, UFC::rwLockForWrite);
                                else
                                    conditionPtr->IncreaseMatchBuyQty(matchQty, UFC::rwLockForWrite);
                            }
                            else
                            {
                                if (msgType == TWSE::mtT010)
                                    conditionPtr->IncreaseNormalMatchSellQty(matchQty, UFC::rwLockForWrite);
                                else
                                    conditionPtr->IncreaseMatchSellQty(matchQty, UFC::rwLockForWrite);
                            }
                            WriteLevelRiskLog(UFC::dlInformation, logHead, "Condition: %s, Match Order[%s] Qty[%d], ExecKey[%s]", conditionPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str(), matchQty, ExecKey.c_str());
                        }
                        else
                            WriteRiskLog(logHead, "Account[%s] Stock[%s][%s][%s]%c not found, Match Order[%s]", accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerId.c_str(), twseOrderType, orderId.c_str());
                    }
                    else
                        WriteRiskLog(logHead, "Account[%s] has no Stock[%s] data, Match Order[%s]", accountNo.c_str(), stockNo.c_str(), orderId.c_str());
                }
                else
                    WriteRiskLog(logHead, "Account[%s] of Match Order[%s] not found", accountNo.c_str(), orderId.c_str());
            }
            else
                WriteRiskLog(logHead, "Wrong Order[%s]. LeavesQty[%d] < MatchQty[%d]", orderId.c_str(), origLeavesQty, matchQty);
        }
        else
            WriteRiskLog(logHead, "Order[%s] is Filled. LeavesQty[%d] <= 0", orderId.c_str(), origLeavesQty);
    }
    else
        WriteRiskLog(logHead, "Order[%s] not Found", orderId.c_str());
    
    FlushRiskLog();    
}  //CMegaRiskManagerObject::AddRiskMatchQty()
//------------------------------------------------------------------------------
void CMegaRiskManagerObject::ProcessRiskCancelReplaceMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI, const UFC::AnsiString PVCID)
{
    int functionCode = OI.FuncCode;
    if ((functionCode != 3) && (functionCode != 4)) return;
    
    UFC::AnsiString logHead;
    logHead.Printf("ProcessRiskCancelReplaceMessage() PVC:%s", PVCID.c_str());
    TWSE::TMessageType msgType = ReplyMsg->MessageType();
    int msgStatusCode = ReplyMsg->GetStatusCode();
    UFC::AnsiString accountNo, stockNo, orderId, orderQtyStr, beforeQtyStr, afterQtyStr, orderPriceStr;
    char buySellCode = ' ', exchangeCode = ' ', twseOrderType = ' ', twsePriceType = ' ', twseTimeInForce = ' ';
    double orderPrice = 0.0;
    if (msgType == TWSE::mtT020)  //Normal
    {
        TWSE::MessageT020 *t020MsgPtr = static_cast<TWSE::MessageT020*>(ReplyMsg);
        accountNo = UFC::AnsiString(t020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(t020MsgPtr->StockID, 6);
        orderId = UFC::AnsiString(t020MsgPtr->OrderNo, 5);
        buySellCode = t020MsgPtr->BuySellCode;
        exchangeCode = t020MsgPtr->ExchangeCode;
        twseOrderType = t020MsgPtr->TWSEOrderType;
        if( TWSE::MessageHeader::FNewVersion == TRUE )
        {
            twsePriceType = t020MsgPtr->OrderType;      //'1':Market '2':Limit
            twseTimeInForce = t020MsgPtr->TimeInForce;  //'0':ROD '3':IOC '4':FOK
            orderQtyStr = UFC::AnsiString(t020MsgPtr->OrderQty, 6);
            beforeQtyStr = UFC::AnsiString(t020MsgPtr->BeforeQty, 6);
            afterQtyStr = UFC::AnsiString(t020MsgPtr->AfterQty, 6);
            orderPriceStr = UFC::AnsiString(t020MsgPtr->OrderPrice, 9);
            orderPrice = Parse95v9999PriceStr(orderPriceStr);
        }
        else
        {
            twsePriceType = '2';    //Limit
            twseTimeInForce = '0';  //ROD
            orderQtyStr = UFC::AnsiString(t020MsgPtr->OrderQty, 3);
            beforeQtyStr = UFC::AnsiString(t020MsgPtr->BeforeQty, 3);
            afterQtyStr = UFC::AnsiString(t020MsgPtr->AfterQty, 3);
            orderPriceStr = UFC::AnsiString(t020MsgPtr->OrderPrice, 6);
            orderPrice = Parse94v99PriceStr(orderPriceStr);
        }
    }
    else if (msgType == TWSE::mtO020)  //Odd
    {
        TWSE::MessageO020 *o020MsgPtr = static_cast<TWSE::MessageO020*>(ReplyMsg);
        accountNo = UFC::AnsiString(o020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(o020MsgPtr->StockID, 6);
        orderId = UFC::AnsiString(o020MsgPtr->OrderNo, 5);
        buySellCode = o020MsgPtr->BuySellCode;
        exchangeCode = '2';     //Odd
        twseOrderType = '0';
        twsePriceType = '2';    //Limit
        twseTimeInForce = '0';  //ROD
        if( TWSE::MessageHeader::FNewVersion == TRUE )
        {
            orderQtyStr = UFC::AnsiString(o020MsgPtr->OrderQty, 6);
            beforeQtyStr = UFC::AnsiString(o020MsgPtr->BeforeQty, 6);
            afterQtyStr = UFC::AnsiString(o020MsgPtr->AfterQty, 6);
            orderPriceStr = UFC::AnsiString(o020MsgPtr->OrderPrice, 9);
            orderPrice = Parse95v9999PriceStr(orderPriceStr);
        }
        else
        {
            orderQtyStr = UFC::AnsiString(o020MsgPtr->OrderQty, 8);
            beforeQtyStr = UFC::AnsiString(o020MsgPtr->BeforeQty, 8);
            afterQtyStr = UFC::AnsiString(o020MsgPtr->AfterQty, 8);
            orderPriceStr = UFC::AnsiString(o020MsgPtr->OrderPrice, 6);
            orderPrice = Parse94v99PriceStr(orderPriceStr);
        }  //if( MessageHeader::FNewVersion == TRUE )
    }
    else if (msgType == TWSE::mtP020)  //OffHour
    {
        TWSE::MessageP020 *p020MsgPtr = static_cast<TWSE::MessageP020*>(ReplyMsg);
        accountNo = UFC::AnsiString(p020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(p020MsgPtr->StockID, 6);
        orderId = UFC::AnsiString(p020MsgPtr->OrderNo, 5);
        buySellCode = p020MsgPtr->BuySellCode;
        exchangeCode = p020MsgPtr->ExchangeCode;
        twseOrderType = p020MsgPtr->TWSEOrderType;
        twsePriceType = '2';    //Limit
        twseTimeInForce = '0';  //ROD
        if( TWSE::MessageHeader::FNewVersion == TRUE )
        {
            orderQtyStr = UFC::AnsiString(p020MsgPtr->OrderQty, 6);
            beforeQtyStr = UFC::AnsiString(p020MsgPtr->BeforeQty, 6);
            afterQtyStr = UFC::AnsiString(p020MsgPtr->AfterQty, 6);
            orderPriceStr = UFC::AnsiString(p020MsgPtr->OrderPrice, 9);
            orderPrice = Parse95v9999PriceStr(orderPriceStr);
        }
        else
        {
            orderQtyStr = UFC::AnsiString(p020MsgPtr->OrderQty, 3);
            beforeQtyStr = UFC::AnsiString(p020MsgPtr->BeforeQty, 3);
            afterQtyStr = UFC::AnsiString(p020MsgPtr->AfterQty, 3);
            orderPriceStr = UFC::AnsiString(p020MsgPtr->OrderPrice, 6);
            orderPrice = Parse94v99PriceStr(orderPriceStr);
        }
    }
    else
    {
        WriteRiskLog(logHead, "Order[%s] Wrong msgType[%d].", OI.GetOrderID()/*.c_str()*/, msgType);
        return;
    }
        
    accountNo.TrimRight(' ');
    stockNo.TrimRight(' ');
    stockNo.TrimRight(' ');
    int orderQty = orderQtyStr.ToInt();
    int beforeQty = beforeQtyStr.ToInt();
    int afterQty = afterQtyStr.ToInt();
    if (exchangeCode != '2') 
    {
        orderQty *= 1000;
        beforeQty *= 1000;
        afterQty *= 1000;
    }
    int diffenentQty = beforeQty - afterQty;
    WriteLevelRiskLog(UFC::dlPerformance, logHead, "Order[%s] func[%d] %c Px[%10.4lf] Qty[Order=%d before=%d, after=%d] status[%d] msgType[%d] PxType[%c] TIF[%c]", 
                      orderId.c_str(), functionCode, buySellCode, orderPrice, orderQty, beforeQty, afterQty, msgStatusCode, msgType, twsePriceType, twseTimeInForce);

    CRMOrderSummary *orderPtr = FindOrderFromIDIndex(orderId, UFC::rwLockForRead);
    if (orderPtr != 0)
    {
        orderPtr->LockForRead();
        UFC::AnsiString warrantNo = orderPtr->GetWarrantNo(UFC::rwNotLock);
        UFC::AnsiString brokerId = orderPtr->GetBrokerId(UFC::rwNotLock);
        //char exchangeCode = orderPtr->GetExchangeCode(UFC::rwNotLock);
        int origLeavesQty = orderPtr->GetLeavesOrderQty(UFC::rwNotLock);
        int newLeavesQty = origLeavesQty - diffenentQty;
        orderPtr->UnlockForRead();
        CRMInvestorAccount *accountPtr = FindInvestorAccountFromNoIndex(accountNo, UFC::rwLockForRead);
        if (accountPtr != 0)
        {
            CRMStockTradingData *stockDataPtr = accountPtr->FindStockData(stockNo, UFC::rwLockForRead);
            if (stockDataPtr != 0)
            {
                orderPtr->SetLeavesQty(newLeavesQty, UFC::rwLockForWrite);
                if (newLeavesQty <= 0)
                {
                    CRMOrderSummary *cancelOrderPtr = 0;
                    if (buySellCode == 'B')
                    {
                        if (msgType == TWSE::mtT020)
                            cancelOrderPtr = stockDataPtr->RemoveNormalBuyOrder(orderId, UFC::rwLockForWrite);
                        else
                            cancelOrderPtr = stockDataPtr->RemoveBuyOrder(orderId, UFC::rwLockForWrite);

                        if (cancelOrderPtr != 0)
                        {
                            double newBuyAmount = stockDataPtr->GetBuyAmount(UFC::rwLockForRead) - cancelOrderPtr->GetOrderAmount(UFC::rwLockForRead);
                            stockDataPtr->SetBuyAmount(newBuyAmount, UFC::rwLockForWrite);
                        }
                    }
                    else if (buySellCode == 'S')
                    {
                        if (msgType == TWSE::mtT020)
                            cancelOrderPtr = stockDataPtr->RemoveNormalSellOrder(orderId, UFC::rwLockForWrite);
                        else
                            cancelOrderPtr = stockDataPtr->RemoveSellOrder(orderId, UFC::rwLockForWrite);
                        
                        if (cancelOrderPtr != 0)
                        {
                            double newSellAmount = stockDataPtr->GetSellAmount(UFC::rwLockForRead) - cancelOrderPtr->GetOrderAmount(UFC::rwLockForRead);
                            stockDataPtr->SetSellAmount(newSellAmount, UFC::rwLockForWrite);
                        }
                    }
            
                    if (cancelOrderPtr != 0)
                        WriteLevelRiskLog(UFC::dlInformation, logHead, "Stock: %s, Order[%s], func[%d], msgType[%d], bQty[%d], aQty[%d]", stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str(), functionCode, msgType, beforeQty, afterQty);
                    else
                        WriteRiskLog(logHead, "Remove Order[%s] from account[%s] Stock[%s] failed.", orderId.c_str(), accountNo.c_str(), stockNo.c_str());
                }
                else
                    WriteLevelRiskLog(UFC::dlInformation, logHead, "Set Order[%s] LeavesQty from %d to %d, account[%s], Stock[%s], func[%d], msgType[%d], bQty[%d], aQty[%d]", orderId.c_str(), origLeavesQty, newLeavesQty, accountNo.c_str(), stockNo.c_str(), functionCode, msgType, beforeQty, afterQty);
                
                CRMMegaStockTradingData *megaStockDataPtr = static_cast<CRMMegaStockTradingData*>(stockDataPtr);
                CRMBrokerStockCondition *conditionPtr = megaStockDataPtr->FindBrokerStockCondition(warrantNo, brokerId, twseOrderType, UFC::rwLockForRead);
                if (conditionPtr != 0)
                {
                    if (buySellCode == 'B')
                    {
                        if (msgType == TWSE::mtT020)
                        {
                            int normalOrderBuyQty = conditionPtr->GetNormalOrderBuyQty(UFC::rwLockForRead);
                            if (normalOrderBuyQty >= diffenentQty)
                                conditionPtr->IncreaseNormalOrderBuyQty(-diffenentQty, UFC::rwLockForWrite);
                            else
                            {
                                conditionPtr->SetNormalOrderBuyQty(0, UFC::rwLockForWrite);
                                WriteRiskLog(logHead, "NormalOrderBuyQty[%d] is Less then diffenentQty[%d], set NormalOrderBuyQty to 0.", normalOrderBuyQty, diffenentQty);
                            }
                        }
                        else
                        {
                            int orderBuyQty = conditionPtr->GetOrderBuyQty(UFC::rwLockForRead);
                            if (orderBuyQty >= diffenentQty)
                                conditionPtr->IncreaseOrderBuyQty(-diffenentQty, UFC::rwLockForWrite);
                            else
                            {
                                conditionPtr->SetOrderBuyQty(0, UFC::rwLockForWrite);
                                WriteRiskLog(logHead, "OrderBuyQty[%d] is Less then diffenentQty[%d], set OrderBuyQty to 0.", orderBuyQty, diffenentQty);
                            }
                        }
                    }
                    else
                    {
                        if (msgType == TWSE::mtT020)
                        {
                            int normalOrderSellQty = conditionPtr->GetNormalOrderSellQty(UFC::rwLockForRead);
                            if (normalOrderSellQty >= diffenentQty)
                                conditionPtr->IncreaseNormalOrderSellQty(-diffenentQty, UFC::rwLockForWrite);
                            else
                            {
                                conditionPtr->SetNormalOrderSellQty(0, UFC::rwLockForWrite);
                                WriteRiskLog(logHead, "NormalOrderSellQty[%d] is Less then diffenentQty[%d], set NormalOrderSellQty to 0.", normalOrderSellQty, diffenentQty);
                            }
                        }
                        else
                        {
                            int orderSellQty = conditionPtr->GetOrderSellQty(UFC::rwLockForRead);
                            if (orderSellQty >= diffenentQty)
                                conditionPtr->IncreaseOrderSellQty(-diffenentQty, UFC::rwLockForWrite);
                            else
                            {
                                conditionPtr->SetOrderSellQty(0, UFC::rwLockForWrite);
                                WriteRiskLog(logHead, "OrderSellQty[%d] is Less then diffenentQty[%d], set OrderSellQty to 0.", orderSellQty, diffenentQty);
                            }
                        }  //if (msgType == mtT020)
                    }  //if (buySellCode == 'B')
                    WriteLevelRiskLog(UFC::dlInformation, logHead, "Condition: %s, order[%s], func[%d], msgType[%d], bQty[%d], aQty[%d]", conditionPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str(), functionCode, msgType, beforeQty, afterQty);
                }
                else
                    WriteRiskLog(logHead, "Account[%s] Stock[%s][%s][%s] %c not found, Order[%s]", accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerId.c_str(), twseOrderType, orderId.c_str());
            }
            else
                WriteRiskLog(logHead, "Account[%s] has no Stock[%s] data, Order[%s]", accountNo.c_str(), stockNo.c_str(), orderId.c_str());
        }
        else
            WriteRiskLog(logHead, "Account[%s] of Cancel/Replace Order[%s] not found", accountNo.c_str(), orderId.c_str());
    }
    else
        WriteRiskLog(logHead, "Order[%s] not Found", orderId.c_str());
    FlushRiskLog();    
}  //CMegaRiskManagerObject::ProcessRiskCancelReplaceMessage()
//------------------------------------------------------------------------------
void CMegaRiskManagerObject::ProcessRiskRejectMessage(TWSE::MessageHeader*& ReplyMsg, OrderRecord& OI, const UFC::AnsiString PVCID)
{
    int functionCode = OI.FuncCode;
    if ((functionCode != 1) && (functionCode != 2)) return;

    UFC::AnsiString logHead;
    logHead.Printf("ProcessRiskRejectMessage() PVC:%s", PVCID.c_str());
    UFC::AnsiString orderId = OI.GetOrderID();
    TWSE::TMessageType msgType = static_cast<TWSE::TMessageType>(OI.MsgType);
    WriteLevelRiskLog(UFC::dlPerformance, logHead, "Process Reject Order[%s]", orderId.c_str());
    
    CRMOrderSummary *orderPtr = FindOrderFromIDIndex(orderId, UFC::rwLockForRead);
    if (orderPtr != 0)
    {
        orderPtr->LockForRead();
        UFC::AnsiString accountNo = orderPtr->GetAccountNo(UFC::rwNotLock);
        UFC::AnsiString stockNo = orderPtr->GetStockNo(UFC::rwNotLock);
        UFC::AnsiString warrantNo = orderPtr->GetWarrantNo(UFC::rwNotLock);
        UFC::AnsiString brokerId = orderPtr->GetBrokerId(UFC::rwNotLock);
        char twseOrderType = orderPtr->GetTWSEOrderType(UFC::rwNotLock);
        //char side = orderPtr->GetSide(UFC::rwNotLock);
        //char exchangeCode = orderPtr->GetExchangeCode(UFC::rwNotLock);
        int orderOty = orderPtr->GetOrderQty(UFC::rwNotLock);
        orderPtr->UnlockForRead();
        CRMInvestorAccount *accountPtr = FindInvestorAccountFromNoIndex(accountNo, UFC::rwLockForRead);
        if (accountPtr != 0)
        {
            CRMStockTradingData *stockDataPtr = accountPtr->FindStockData(stockNo, UFC::rwLockForRead);
            if (stockDataPtr != 0)
            {
                CRMOrderSummary *removeOrderPtr = 0;
                if (functionCode == 1)
                {
                    if (msgType == TWSE::mtT010)
                        removeOrderPtr = stockDataPtr->RemoveNormalBuyOrder(orderId, UFC::rwLockForWrite);
                    else
                        removeOrderPtr = stockDataPtr->RemoveBuyOrder(orderId, UFC::rwLockForWrite);

                    if (removeOrderPtr != 0)
                    {
                        double newBuyAmount = stockDataPtr->GetBuyAmount(UFC::rwLockForRead) - removeOrderPtr->GetOrderAmount(UFC::rwLockForRead);
                        stockDataPtr->SetBuyAmount(newBuyAmount, UFC::rwLockForWrite);
                    }
                }
                else
                {
                    if (msgType == TWSE::mtT010)
                        removeOrderPtr = stockDataPtr->RemoveNormalSellOrder(orderId, UFC::rwLockForWrite);
                    else
                        removeOrderPtr = stockDataPtr->RemoveSellOrder(orderId, UFC::rwLockForWrite);
                    
                    if (removeOrderPtr != 0)
                    {
                        double newSellAmount = stockDataPtr->GetSellAmount(UFC::rwLockForRead) - removeOrderPtr->GetOrderAmount(UFC::rwLockForRead);
                        stockDataPtr->SetSellAmount(newSellAmount, UFC::rwLockForWrite);
                    }
                }
                if (removeOrderPtr != 0)
                    WriteLevelRiskLog(UFC::dlInformation, logHead, "Stock: %s, Order[%s]", stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str());
                else
                    WriteRiskLog(logHead, "Remove order[%s] from Account[%s] Stock[%s] failed", orderId.c_str(), accountNo.c_str(), stockNo.c_str());
                
                CRMMegaStockTradingData *megaStockDataPtr = static_cast<CRMMegaStockTradingData*>(stockDataPtr);
                CRMBrokerStockCondition *conditionPtr = megaStockDataPtr->FindBrokerStockCondition(warrantNo, brokerId, twseOrderType, UFC::rwLockForRead);
                if (conditionPtr != 0)
                {
                    if (functionCode == 1)
                    {
                        if (msgType == TWSE::mtT010)
                        {
                            int normalOrderBuyQty = conditionPtr->GetNormalOrderBuyQty(UFC::rwLockForRead);
                            if (normalOrderBuyQty >= orderOty)
                                conditionPtr->IncreaseNormalOrderBuyQty(-orderOty, UFC::rwLockForWrite);
                            else
                            {
                                conditionPtr->SetNormalOrderBuyQty(0, UFC::rwLockForWrite);
                                WriteRiskLog(logHead, "NormalOrderBuyQty[%d] is Less then orderQty[%d], set NormalOrderBuyQty to 0.", normalOrderBuyQty, orderOty);
                            }
                        }
                        else
                        {
                            int orderBuyQty = conditionPtr->GetOrderBuyQty(UFC::rwLockForRead);
                            if (orderBuyQty >= orderOty)
                                conditionPtr->IncreaseOrderBuyQty(-orderOty, UFC::rwLockForWrite);
                            else
                            {
                                conditionPtr->SetOrderBuyQty(0, UFC::rwLockForWrite);
                                WriteRiskLog(logHead, "OrderBuyQty[%d] is Less then orderQty[%d], set OrderBuyQty to 0.", orderBuyQty, orderOty);
                            }
                        }
                    }
                    else
                    {
                        if (msgType == TWSE::mtT010)
                        {
                            int normalOrderSellQty = conditionPtr->GetNormalOrderSellQty(UFC::rwLockForRead);
                            if (normalOrderSellQty >= orderOty)
                                conditionPtr->IncreaseNormalOrderSellQty(-orderOty, UFC::rwLockForWrite);
                            else
                            {
                                conditionPtr->SetNormalOrderSellQty(0, UFC::rwLockForWrite);
                                WriteRiskLog(logHead, "NormalOrderSellQty[%d] is Less then orderQty[%d], set NormalOrderSellQty to 0.", normalOrderSellQty, orderOty);
                            }
                        }
                        else
                        {
                            int orderSellQty = conditionPtr->GetOrderSellQty(UFC::rwLockForRead);
                            if (orderSellQty >= orderOty)
                                conditionPtr->IncreaseOrderSellQty(-orderOty, UFC::rwLockForWrite);
                            else
                            {
                                conditionPtr->SetOrderSellQty(0, UFC::rwLockForWrite);
                                WriteRiskLog(logHead, "OrderSellQty[%d] is Less then orderQty[%d], set OrderSellQty to 0.", orderSellQty, orderOty);
                            }
                        }  //if (msgType == mtT010)
                    }
                    WriteLevelRiskLog(UFC::dlInformation, logHead, "Condition: %s, Order[%s]", conditionPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str());
                }
                else
                    WriteRiskLog(logHead, "Account[%s] Stock[%s][%s][%s] %c not found, Order[%s]", accountNo.c_str(), stockNo.c_str(), warrantNo.c_str(), brokerId.c_str(), twseOrderType, orderId.c_str());
            }
            else
                WriteRiskLog(logHead, "Account[%s] has no Stock[%s] data, Order[%s]", accountNo.c_str(), stockNo.c_str(), orderId.c_str());
        }
        else
            WriteRiskLog(logHead, "Account[%s] of Rejected Order[%s] not found", accountNo.c_str(), orderId.c_str());
    }
    else
        WriteRiskLog(logHead, "Rejected Order[%s] not Found", orderId.c_str());
    FlushRiskLog();
}  //CMegaRiskManagerObject::ProcessRiskRejectMessage()
//------------------------------------------------------------------------------
void CMegaRiskManagerObject::RecoverRMConfirmData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, Int32 NetworkID, bool IsSynchronize)
{
    UFC::AnsiString logHead = "";
    if (IsSynchronize)
        logHead = "SynchronizeRMConfirmData()";
    else
        logHead = "RecoverRMConfirmData()";
    TWSE::MessageHeader *msgPtr = TWSE::MessageHeader::CreateMessage(ExecRptDataStr.c_str(), ExecRptDataStr.Length());
    int subSystem = msgPtr->GetSubSystem();
    int functionCode = msgPtr->GetFunctionCode();
    int msgStatusCode = msgPtr->GetStatusCode();    
    TWSE::TMessageType msgType = msgPtr->MessageType();
    if (((msgType != TWSE::mtT020) && (msgType != TWSE::mtO020) && (msgType != TWSE::mtP020)) ||
        ((functionCode != 1) && (functionCode != 2) && (functionCode != 3) && (functionCode != 4)) || 
        (((functionCode == 1) || (functionCode == 2) || (functionCode == 4)) && (msgStatusCode != 0)) ||
        ((functionCode == 3) && ((msgStatusCode != 0) && (msgStatusCode != 31) && (msgStatusCode != 32))))
    {
        WriteRiskLog(logHead, "Ignore Order[%s], Func[%d], Status[%d], Type[%d]", ExecRptDataStr.c_str(), functionCode, msgStatusCode, msgType);
        delete msgPtr;
        return;
    }
    
    UFC::AnsiString orderId, accountNo, stockNo, orderQtyStr, beforeQtyStr, afterQtyStr, orderPriceStr;
    char buySellCode = ' ', exchangeCode = ' ', twseOrderType = ' ';
    char twsePriceType = ' ', twseTimeInForce = ' ';
    TWSE::TMessageType orderMsgType = TWSE::mtUnknown;
    double orderPrice = 0.0;
    if (msgType == TWSE::mtT020)
    {
        orderMsgType = TWSE::mtT010;
        TWSE::MessageT020 *t020MsgPtr = static_cast<TWSE::MessageT020*>(msgPtr);
        orderId = UFC::AnsiString(t020MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(t020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(t020MsgPtr->StockID, 6);
        buySellCode = t020MsgPtr->BuySellCode;
        exchangeCode = t020MsgPtr->ExchangeCode;
        twseOrderType = t020MsgPtr->TWSEOrderType;
        if( TWSE::MessageHeader::FNewVersion == TRUE )  //Normal
        {
            orderQtyStr = UFC::AnsiString(t020MsgPtr->OrderQty, 6);
            beforeQtyStr = UFC::AnsiString(t020MsgPtr->BeforeQty, 6);
            afterQtyStr = UFC::AnsiString(t020MsgPtr->AfterQty, 6);
            orderPriceStr = UFC::AnsiString(t020MsgPtr->OrderPrice, 9);
            orderPrice = Parse95v9999PriceStr(orderPriceStr);
            twsePriceType = t020MsgPtr->OrderType;      //'1':Market '2':Limit
            twseTimeInForce = t020MsgPtr->TimeInForce;  //'0':ROD '3':IOC '4':FOK
        }
        else
        {
            orderQtyStr = UFC::AnsiString(t020MsgPtr->OrderQty, 3);
            beforeQtyStr = UFC::AnsiString(t020MsgPtr->BeforeQty, 3);
            afterQtyStr = UFC::AnsiString(t020MsgPtr->AfterQty, 3);
            orderPriceStr = UFC::AnsiString(t020MsgPtr->OrderPrice, 6);
            orderPrice = Parse94v99PriceStr(orderPriceStr);
            twsePriceType = '2';    //Limit
            twseTimeInForce = '0';  //ROD
        }
    }
    else if (msgType == TWSE::mtO020)
    {
        orderMsgType = TWSE::mtO010;
        TWSE::MessageO020 *o020MsgPtr = static_cast<TWSE::MessageO020*>(msgPtr);
        orderId = UFC::AnsiString(o020MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(o020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(o020MsgPtr->StockID, 6);
        buySellCode = o020MsgPtr->BuySellCode;
        exchangeCode = '2';     //Odd
        twseOrderType = '0';
        twsePriceType = '2';    //Limit
        twseTimeInForce = '0';  //ROD
        if( TWSE::MessageHeader::FNewVersion == TRUE )
        {
            orderQtyStr = UFC::AnsiString(o020MsgPtr->OrderQty, 6);
            beforeQtyStr = UFC::AnsiString(o020MsgPtr->BeforeQty, 6);
            afterQtyStr = UFC::AnsiString(o020MsgPtr->AfterQty, 6);
            orderPriceStr = UFC::AnsiString(o020MsgPtr->OrderPrice, 9);
            orderPrice = Parse95v9999PriceStr(orderPriceStr);
        }
        else
        {
            orderQtyStr = UFC::AnsiString(o020MsgPtr->OrderQty, 8);
            beforeQtyStr = UFC::AnsiString(o020MsgPtr->BeforeQty, 8);
            afterQtyStr = UFC::AnsiString(o020MsgPtr->AfterQty, 8);
            orderPriceStr = UFC::AnsiString(o020MsgPtr->OrderPrice, 6);
            orderPrice = Parse94v99PriceStr(orderPriceStr);
        }
    }
    else if (msgType == TWSE::mtP020)
    {
        orderMsgType = TWSE::mtP010;
        TWSE::MessageP020 *p020MsgPtr = static_cast<TWSE::MessageP020*>(msgPtr);
        orderId = UFC::AnsiString(p020MsgPtr->OrderNo, 5);
        accountNo = UFC::AnsiString(p020MsgPtr->InvestorAcno, 7);
        stockNo = UFC::AnsiString(p020MsgPtr->StockID, 6);
        buySellCode = p020MsgPtr->BuySellCode;
        exchangeCode = p020MsgPtr->ExchangeCode;
        twseOrderType = p020MsgPtr->OrderType;
        twsePriceType = '2';    //Limit
        twseTimeInForce = '0';  //ROD
        if( TWSE::MessageHeader::FNewVersion == TRUE )
        {
            orderQtyStr = UFC::AnsiString(p020MsgPtr->OrderQty, 6);
            beforeQtyStr = UFC::AnsiString(p020MsgPtr->BeforeQty, 6);
            afterQtyStr = UFC::AnsiString(p020MsgPtr->AfterQty, 6);
            orderPriceStr = UFC::AnsiString(p020MsgPtr->OrderPrice, 9);
            orderPrice = Parse95v9999PriceStr(orderPriceStr);
        }
        else
        {
            orderQtyStr = UFC::AnsiString(p020MsgPtr->OrderQty, 3);
            beforeQtyStr = UFC::AnsiString(p020MsgPtr->BeforeQty, 3);
            afterQtyStr = UFC::AnsiString(p020MsgPtr->AfterQty, 3);
            orderPriceStr = UFC::AnsiString(p020MsgPtr->OrderPrice, 6);
            orderPrice = Parse94v99PriceStr(orderPriceStr);
        }
    }
    delete msgPtr;
    accountNo.TrimRight(' ');
    stockNo.TrimRight(' ');
    int orderQty = orderQtyStr.ToInt();
    int beforeQty = beforeQtyStr.ToInt();
    int afterQty = afterQtyStr.ToInt();
    if (exchangeCode != '2') 
    {
        orderQty *= 1000;
        beforeQty *= 1000;
        afterQty *= 1000;
    }
    
    int diffenentQty = beforeQty - afterQty;
    WriteLevelRiskLog(UFC::dlInformation, logHead, "Order[%s] Stock[%s][%s][%s]%c %c Qty[%d] Px[%10.4lf] bQty[%d] aQty[%d] execCode[%c] Acc[%s] func[%d] status[%d] SubSys[%d] msgType[%d] NID[%ld] PxType[%c] TIF[%c]", 
                      orderId.c_str(), stockNo.c_str(), WarrantNo.c_str(), BrokerId.c_str(), twseOrderType, buySellCode, orderQty, orderPrice, beforeQty, afterQty, exchangeCode, accountNo.c_str(), functionCode, msgStatusCode, subSystem, msgType, (long)NetworkID, twsePriceType, twseTimeInForce);
    
    CRMOrderSummary *targetOrderPtr = FindOrderFromIDIndex(orderId, UFC::rwLockForRead); 
    if ((functionCode == 1) || (functionCode == 2))
    {
        if (targetOrderPtr == 0)
        {
            CRMOrderSummary *newOrderPtr = new CRMOrderSummary(orderId, buySellCode, stockNo, WarrantNo, BrokerId, twseOrderType, orderPrice, orderQty, exchangeCode, orderMsgType, accountNo);
            if (InsertOrderIntoIDIndex(orderId, newOrderPtr, UFC::rwLockForWrite))
                targetOrderPtr = newOrderPtr;
            else
            {
                WriteRiskLog(logHead, "Insert Order[%s] into Object Manager failed.", orderId.c_str());
                delete newOrderPtr;
            }
        }
        else
        {
            targetOrderPtr = 0;
            WriteRiskLog(logHead, "Duplicate Order[%s].", orderId.c_str());
        }
    }
    else if ((functionCode == 3) || (functionCode == 4))
    {
        if (targetOrderPtr == 0)
            WriteRiskLog(logHead, "Order[%s] Not Found.", orderId.c_str());
    }  //if ((functionCode == 1) || (functionCode == 2))
    
    if (targetOrderPtr != 0)
    {
        CRMInvestorAccount *accountPtr = FindInvestorAccountFromNoIndex(accountNo, UFC::rwLockForRead);
        if (accountPtr != 0)
        {
            bool isNewStockData = false;
            CRMStockTradingData *stockDataPtr = accountPtr->FindStockData(stockNo, UFC::rwLockForRead);
            if (stockDataPtr == 0)
            {
                stockDataPtr = new CRMMegaStockTradingData(accountNo, stockNo);
                if (!accountPtr->InsertStockData(stockNo, stockDataPtr, cstNotCheck, UFC::rwLockForWrite))
                {
                    WriteRiskLog(logHead, "Insert Stock[%s] data into Account[%s] failed.", stockNo.c_str(), accountNo.c_str());
                    delete stockDataPtr;
                    stockDataPtr = 0;
                }
                else
                    isNewStockData = true;
            }  //if (stockDataPtr == 0)           
            
            if (stockDataPtr != 0)
            {
                if (isNewStockData)
                    WriteLevelRiskLog(UFC::dlPerformance, logHead, "Create Account[%s] Stock[%s].", accountNo.c_str(), stockNo.c_str());
                
                bool isInsertOrderSuccess = false;
                if ((functionCode == 1) || (functionCode == 2))
                {
                    if (functionCode == 1)
                    {
                        if (msgType == TWSE::mtT020)
                            isInsertOrderSuccess = stockDataPtr->InsertNormalBuyOrder(orderId, targetOrderPtr, cstCheck, UFC::rwLockForWrite);
                        else
                            isInsertOrderSuccess = stockDataPtr->InsertBuyOrder(orderId, targetOrderPtr, cstCheck, UFC::rwLockForWrite);
                    }
                    else if (functionCode == 2)
                    {
                        if (msgType == TWSE::mtT020)
                            isInsertOrderSuccess = stockDataPtr->InsertNormalSellOrder(orderId, targetOrderPtr, cstCheck, UFC::rwLockForWrite);
                        else
                            isInsertOrderSuccess = stockDataPtr->InsertSellOrder(orderId, targetOrderPtr, cstCheck, UFC::rwLockForWrite);
                    }
                    
                    if (isInsertOrderSuccess)
                        WriteLevelRiskLog(UFC::dlInformation, logHead, "StockData: %s, Order[%s], msgType[%d]", stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str(), msgType);
                    else
                        WriteRiskLog(logHead, "Insert Order[%s] into Account[%s] Stock[%s] failed.", orderId.c_str(), accountNo.c_str(), stockNo.c_str());
                }
                else if ((functionCode == 3) || (functionCode == 4))
                {
                    int origLeavesQty = targetOrderPtr->GetLeavesOrderQty(UFC::rwLockForRead);
                    int newLeavesQty = origLeavesQty - diffenentQty;
                    targetOrderPtr->SetLeavesQty(newLeavesQty, UFC::rwLockForWrite);
                    if (newLeavesQty <= 0)
                    {
                        CRMOrderSummary *cancelOrderPtr = 0;
                        if (buySellCode == 'B')
                        {
                            if (msgType == TWSE::mtT020)
                                cancelOrderPtr = stockDataPtr->RemoveNormalBuyOrder(orderId, UFC::rwLockForWrite);
                            else
                                cancelOrderPtr = stockDataPtr->RemoveBuyOrder(orderId, UFC::rwLockForWrite);
                        }
                        else if (buySellCode == 'S')
                        {
                            if (msgType == TWSE::mtT020)
                                cancelOrderPtr = stockDataPtr->RemoveNormalSellOrder(orderId, UFC::rwLockForWrite);
                            else
                                cancelOrderPtr = stockDataPtr->RemoveSellOrder(orderId, UFC::rwLockForWrite);
                        }
                                    
                        if (cancelOrderPtr != 0)
                            WriteLevelRiskLog(UFC::dlInformation, logHead, "StockData: %s, Order[%s], msgType[%d], bQty[%d], aQty[%d]", stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str(), msgType, beforeQty, afterQty);
                        else
                            WriteRiskLog(logHead, "Remove Order[%s] from Account[%s] Stock[%s] failed.", orderId.c_str(), accountNo.c_str(), stockNo.c_str());
                    }  //if (afterQty == 0)
                    else
                        WriteLevelRiskLog(UFC::dlPerformance, logHead, "Set Order[%s] LeavesQty from %d to %d, msgType[%d], bQty[%d], aQty[%d]", orderId.c_str(), origLeavesQty, newLeavesQty, msgType, beforeQty, afterQty);
                }  //if ((curFunctionCode == 3) || (curFunctionCode == 4))
                
                CRMMegaStockTradingData *megaStockDataPtr = static_cast<CRMMegaStockTradingData*>(stockDataPtr);
                CRMBrokerStockCondition *conditionPtr = megaStockDataPtr->FindBrokerStockCondition(WarrantNo, BrokerId, twseOrderType, UFC::rwLockForRead);
                bool isSellShort = BrokerId != FRMMainBrokerID;  //是否為借券
                if ((conditionPtr == 0) && !isSellShort)  //非借券
                {
                    UFC::AnsiString todayStr;
                    UFC::GetYYYYMMDD(todayStr, FALSE);
                    conditionPtr = new CRMBrokerStockCondition(accountNo, stockNo, WarrantNo, BrokerId, twseOrderType, 0, 0, todayStr, isSellShort);
                    if (!megaStockDataPtr->InsertBrokerStockCondition(WarrantNo, BrokerId, twseOrderType, conditionPtr, cstNotCheck, UFC::rwLockForWrite))
                    {
                        WriteRiskLog(logHead, "Insert Account[%s] Stock[%s][%s][%s]%c condition failed", accountNo.c_str(), orderId.c_str(), stockNo.c_str(), WarrantNo.c_str(), BrokerId.c_str(), twseOrderType);
                        delete conditionPtr;
                        conditionPtr = 0;
                    }
                }  //if (!isSellShort)
                
                if (conditionPtr != 0)
                {
                    if (functionCode == 1)
                    {
                        if (msgType == TWSE::mtT020)
                            conditionPtr->IncreaseNormalOrderBuyQty(orderQty, UFC::rwLockForWrite);
                        else
                            conditionPtr->IncreaseOrderBuyQty(orderQty, UFC::rwLockForWrite);
                    }
                    else if (functionCode == 2)
                    {
                        if (msgType == TWSE::mtT020)
                            conditionPtr->IncreaseNormalOrderSellQty(orderQty, UFC::rwLockForWrite);
                        else
                            conditionPtr->IncreaseOrderSellQty(orderQty, UFC::rwLockForWrite);
                    }
                    else if ((functionCode == 3) || (functionCode == 4))
                    {
                        if (buySellCode == 'B')
                        {
                            if (msgType == TWSE::mtT020)
                            {
                                int normalOrderBuyQty = conditionPtr->GetNormalOrderBuyQty(UFC::rwLockForRead);
                                if (normalOrderBuyQty >= diffenentQty)
                                    conditionPtr->IncreaseNormalOrderBuyQty(-diffenentQty, UFC::rwLockForWrite);
                                else
                                {
                                    conditionPtr->SetNormalOrderBuyQty(0, UFC::rwLockForWrite);                                    
                                    WriteRiskLog(logHead, "NormalOrderBuyQty[%d] is Less then deleteQty[%d], set NormalOrderBuyQty to 0.", normalOrderBuyQty, diffenentQty);
                                }
                            }
                            else
                            {
                                int orderBuyQty = conditionPtr->GetOrderBuyQty(UFC::rwLockForRead);
                                if (orderBuyQty >= diffenentQty)
                                    conditionPtr->IncreaseOrderBuyQty(-diffenentQty, UFC::rwLockForWrite);
                                else
                                {
                                    conditionPtr->SetOrderBuyQty(0, UFC::rwLockForWrite);
                                    WriteRiskLog(logHead, "OrderBuyQty[%d] is Less then deleteQty[%d], set OrderBuyQty to 0.", orderBuyQty, diffenentQty);
                                }
                            }  //if (msgType == mtT020)
                        }
                        else
                        {
                            if (msgType == TWSE::mtT020)
                            {
                                int normalOrderSellQty = conditionPtr->GetNormalOrderSellQty(UFC::rwLockForRead);
                                if (normalOrderSellQty >= diffenentQty)
                                    conditionPtr->IncreaseNormalOrderSellQty(-diffenentQty, UFC::rwLockForWrite);
                                else
                                {
                                    conditionPtr->SetNormalOrderSellQty(0, UFC::rwLockForWrite);
                                    WriteRiskLog(logHead, "NormalOrderSellQty[%d] is Less then deleteQty[%d], set NormalOrderSellQty to 0.", normalOrderSellQty, diffenentQty);
                                }
                            }
                            else
                            {
                                int orderSellQty = conditionPtr->GetOrderSellQty(UFC::rwLockForRead);
                                if (orderSellQty >= diffenentQty)
                                    conditionPtr->IncreaseOrderSellQty(-diffenentQty, UFC::rwLockForWrite);
                                else
                                {
                                    conditionPtr->SetOrderSellQty(0, UFC::rwLockForWrite);
                                    WriteRiskLog(logHead, "OrderSellQty[%d] is Less then deleteQty[%d], set OrderSellQty to 0.", orderSellQty, diffenentQty);
                                }
                            }  //if (msgType == mtT020)
                        }  //if (buySellCode == 'B')
                    }
                    WriteLevelRiskLog(UFC::dlInformation, logHead, "Condition: %s, Order[%s], msgType[%d], bQty[%d], aQty[%d]", conditionPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str(), msgType, beforeQty, afterQty);
                }
                else
                    WriteRiskLog(logHead, "Account[%s] Stock[%s][%s][%s]%c not Exist, Order[%s]", accountNo.c_str(), stockNo.c_str(), WarrantNo.c_str(), BrokerId.c_str(), twseOrderType, orderId.c_str());
            }
            else
                WriteRiskLog(logHead, "Wrong Account[%s] Stock[%s], Order[%s]", accountNo.c_str(), stockNo.c_str(), orderId.c_str());
        }
        else
            WriteRiskLog(logHead, "Account[%s] not Found, Order[%s]", accountNo.c_str(), orderId.c_str());
    }  //if (targetOrderPtr != 0)
}  //CMegaRiskManagerObject::RecoverRMConfirmData()
//------------------------------------------------------------------------------
void CMegaRiskManagerObject::RecoverRMFillData(const UFC::AnsiString& ExecRptDataStr, const UFC::AnsiString& WarrantNo, const UFC::AnsiString& BrokerId, bool IsSynchronize)
{
    UFC::AnsiString logHead = "";
    if (IsSynchronize)
        logHead = "SynchronizeRMFillData()";
    else
        logHead = "RecoverRMFillData()";
    UFC::AnsiString logMsg = "";
    int r3Offset = 0;
    TWSE::R3Body *r3BodyPtr = new TWSE::R3Body(ExecRptDataStr.c_str(), r3Offset);
    UFC::AnsiString orderId(r3BodyPtr->FOrderNo, 5);
                
    UFC::AnsiString accountNo(r3BodyPtr->FInvestorACNo, 7);
    accountNo.TrimRight(' ');
                
    UFC::AnsiString stockNo(r3BodyPtr->FStockID, 6);
    stockNo.TrimRight(' ');

    char buySellCode = r3BodyPtr->FBuySellCode;
    char exchangeCode = r3BodyPtr->FEXCD;
            
    int matchQty = r3BodyPtr->GetQty();
    if (exchangeCode != '2') matchQty *= 1000;
            
    double matchPrice = r3BodyPtr->GetPrice();

    int matchSeqNo = r3BodyPtr->GetSeq();
    int matchTime = r3BodyPtr->ProcessingTime();
    char side = r3BodyPtr->FBuySellCode;    
    delete r3BodyPtr;
    
    ///< Use line broker ID + report sequence + match time as unique key.
    UFC::AnsiString execKey = "";
    execKey.Printf( "%s_%08d_%08d%c", BrokerId.c_str(), matchTime, matchSeqNo, side );
    LockFillRptExec(UFC::rwLockForWrite);
    bool isFillRptExist = IsFillRptExecExist(execKey, UFC::rwNotLock);
    if (!isFillRptExist) AddFillRptExec(execKey, UFC::rwNotLock);
    UnlockFillRptExec(UFC::rwLockForWrite);
    if (isFillRptExist)
    {
        WriteRiskLog(logHead, "[%s] has Processed. skip it.", execKey.c_str());
        return;
    }
    
    CRMOrderSummary *orderPtr = FindOrderFromIDIndex(orderId, UFC::rwLockForRead);
    if (orderPtr != 0)
    {
        char twseOrderType = orderPtr->GetTWSEOrderType(UFC::rwLockForRead);
        TWSE::TMessageType msgType = orderPtr->GetMsgType(UFC::rwLockForRead);
        WriteLevelRiskLog(UFC::dlInformation, logHead, "Order[%s] Stock[%s][%s][%s]%c %c MatchQty[%d] MatchPx[%11.3lf] ExecCode[%c] msgType[%d] Account[%s]", 
                          orderId.c_str(), stockNo.c_str(), WarrantNo.c_str(), BrokerId.c_str(), twseOrderType, buySellCode, matchQty, matchPrice, exchangeCode, msgType, accountNo.c_str());
                
        int origLeavesOrderQty = orderPtr->GetLeavesOrderQty(UFC::rwLockForRead);
        if (origLeavesOrderQty > 0)
        {
            if (origLeavesOrderQty >= matchQty)
            {
                orderPtr->IncreaseLeavesQty(-matchQty, UFC::rwLockForWrite);
                CRMInvestorAccount *accountPtr = FindInvestorAccountFromNoIndex(accountNo, UFC::rwLockForRead);
                if (accountPtr != 0)
                {
                    CRMStockTradingData *stockDataPtr = accountPtr->FindStockData(stockNo, UFC::rwLockForRead);
                    if (stockDataPtr != 0)
                    {
                        int newLeavesOrderQty = orderPtr->GetLeavesOrderQty(UFC::rwLockForRead);
                        if (newLeavesOrderQty <= 0)
                        {
                            CRMOrderSummary *filledOrderPtr = 0;
                            if (buySellCode == 'B')
                            {
                                if (msgType == TWSE::mtT010)
                                    filledOrderPtr = stockDataPtr->RemoveNormalBuyOrder(orderId, UFC::rwLockForWrite);
                                else
                                    filledOrderPtr = stockDataPtr->RemoveBuyOrder(orderId, UFC::rwLockForWrite);                                    
                            }
                            else
                            {
                                if (msgType == TWSE::mtT010)
                                    filledOrderPtr = stockDataPtr->RemoveNormalSellOrder(orderId, UFC::rwLockForWrite);
                                else
                                    filledOrderPtr = stockDataPtr->RemoveSellOrder(orderId, UFC::rwLockForWrite);
                            }
                            
                            if (filledOrderPtr != 0)
                                WriteLevelRiskLog(UFC::dlInformation, logHead, "StockData: %s, Order[%s]", stockDataPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str());
                            else
                                WriteRiskLog(logHead, "Remove Filled Order[%s] from account[%s] Stock[%s] failed.", orderId.c_str(), accountNo.c_str(), stockNo.c_str());
                        }
                        
                        CRMMegaStockTradingData *megaStockDataPtr = static_cast<CRMMegaStockTradingData*>(stockDataPtr);
                        CRMBrokerStockCondition *conditionPtr = megaStockDataPtr->FindBrokerStockCondition(WarrantNo, BrokerId, twseOrderType, UFC::rwLockForRead);
                        if (conditionPtr != 0)
                        {
                            if (buySellCode == 'B')
                            {
                                if (msgType == TWSE::mtT010)
                                    conditionPtr->IncreaseNormalMatchBuyQty(matchQty, UFC::rwLockForWrite);
                                else
                                    conditionPtr->IncreaseMatchBuyQty(matchQty, UFC::rwLockForWrite);
                            }
                            else
                                if (msgType == TWSE::mtT010)
                                    conditionPtr->IncreaseNormalMatchSellQty(matchQty, UFC::rwLockForWrite);
                                else
                                    conditionPtr->IncreaseMatchSellQty(matchQty, UFC::rwLockForWrite);
                            WriteLevelRiskLog(UFC::dlInformation, logHead, "Condition: %s, Order[%s]", conditionPtr->ToString(UFC::rwLockForRead).c_str(), orderId.c_str());
                        }
                        else
                            WriteRiskLog(logHead, "Account[%s] Stock[%s][%s][%s]%c not found, Order[%s]", accountNo.c_str(), stockNo.c_str(), WarrantNo.c_str(), BrokerId.c_str(), twseOrderType, orderId.c_str());
                    }
                    else
                        WriteRiskLog(logHead, "Account[%s] Stock[%s] data not Found, Order[%s]", accountNo.c_str(), stockNo.c_str(), orderId.c_str());
                }                
                else
                    WriteRiskLog(logHead, "Account[%s] not Found, Order[%s]", accountNo.c_str(), orderId.c_str());
            }
            else
                WriteRiskLog(logHead, "Order[%s] LeavesQty[%d] < MatchQty[%d].", orderId.c_str(), origLeavesOrderQty, matchQty);
        }    
        else
            WriteRiskLog(logHead, "Order[%s] is Filled.", orderId.c_str());
    }
    else
        WriteRiskLog(logHead, "Order[%s] not Found.", orderId.c_str());
}  //CMegaRiskManagerObject::RecoverRMFillData()
//------------------------------------------------------------------------------
void CMegaRiskManagerObject::ParseExecRtpLogStr(const UFC::AnsiString& ExecRptLogStr, bool IsSynchronize)
{
    UFC::AnsiString logHead = "ParseExecRtpLogStr()";
    ExecutionParser *execParserPtr = 0;
    try 
    {
        execParserPtr = new ExecutionParser(ExecRptLogStr);
    }
    catch (UFC::Exception& e)
    {
        WriteRiskLog(logHead, "Parse Execution[%s] Exception:%s", ExecRptLogStr.c_str(), e.what());
        FlushRiskLog();
        return;
    }
    
    Int32 networkID = execParserPtr->GetNID();
    UFC::AnsiString userKey = execParserPtr->GetKey();
    UFC::AnsiString warrantNo; 
    UFC::AnsiString brokerId;
    if (userKey.Length() > 0)
        ParseRMDataFromUserData(userKey, warrantNo, brokerId);
    else
        WriteRiskLog(logHead, "Execution[%s] has no UserKey.", ExecRptLogStr.c_str());
    UFC::AnsiString execRptData = execParserPtr->GetData();
    BOOL isConfirm = execParserPtr->IsConfirm();
    delete execParserPtr;
    
    if (isConfirm == TRUE)
    {
        bool isNetworkIDExists = false;
        FRMExecRptNIDLock.LockForWrite();
        if( FRMExecRptNIDSet.Exists( networkID ) == TRUE)
        {
            WriteRiskLog(logHead, "Ignore Duplicate Confirm Report NetworkID[%ld].", networkID);
            isNetworkIDExists = true;
        }
        else
            FRMExecRptNIDSet.Add( networkID );
        FRMExecRptNIDLock.UnlockForWrite();
        
        if (!isNetworkIDExists) RecoverRMConfirmData(execRptData, warrantNo, brokerId, networkID, IsSynchronize);
    }
    else
        RecoverRMFillData(execRptData, warrantNo, brokerId, IsSynchronize);
    FlushRiskLog();
}  //CMegaRiskManagerObject::ParseExecRtpLogStr()
//------------------------------------------------------------------------------
void CMegaRiskManagerObject::RecoverRMData(BOOL IsTSE)
{
    UFC::AnsiString logHead = "RecoverRMData()";
    UFC::AnsiString executionLogFileName = "";
    if (IsTSE == TRUE)
        executionLogFileName = "SpeedyTSE.Execution." + UFC::GetDateString() + ".DATA";
    else
        executionLogFileName = "SpeedyOTC.Execution." + UFC::GetDateString() + ".DATA";
    FILE *fileHandlePtr = fopen(executionLogFileName.c_str(), "r");
    if (fileHandlePtr == NULL)
    {
        UFC::BufferedLog::Printf(" %s Can not open recover file %s", logHead.c_str(), executionLogFileName.c_str());
        return;
    }  //if (fileHandlePtr == NULL)
    
    char readBuffer[512];
    while (fgets(readBuffer, 512, fileHandlePtr) != NULL)
    {
        UFC::AnsiString curReadStr(readBuffer);
        ParseExecRtpLogStr(curReadStr, false);
    }  //while (fgets(readBuffer, 512, fileHandlePtr) != NULL)
    fclose(fileHandlePtr);
    
    while (LoadOrderDeleteData()) FRMCurrentDeleteFileSeqNo++;    
}  //CMegaRiskManagerObject::RecoverRMData()
}  //namespace rm
