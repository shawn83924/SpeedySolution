/* 
 * File:   TWSEMegaRMDataPool.cpp
 * Author: Zhen Fan
 *
 * Created on 2021/01/11 14:52
 */
#include "TWSEMegaRMDataPool.h"
#include "iniFile.h"

namespace twserm
{
CMegaRiskManagerDataPool* gMegaRMDataPoolPtr = 0;
    
//-------------------------- CMegaRiskManagerDataPool --------------------------
UFC::AnsiString CMegaRiskManagerDataPool::FRiskManagerDataPath      = "../data";
UFC::AnsiString CMegaRiskManagerDataPool::FRMControlFilePrefix      = "CTLBDW";
UFC::AnsiString CMegaRiskManagerDataPool::FRMControlFileExtension   = "";
UFC::AnsiString CMegaRiskManagerDataPool::FRMDeleteFilePrefix       = "ORDDEL";
UFC::AnsiString CMegaRiskManagerDataPool::FRMDeleteFileExtension    = "";
UFC::AnsiString CMegaRiskManagerDataPool::FRMMainBrokerID           = "600T";
UFC::AnsiString CMegaRiskManagerDataPool::FRMHedgeStockFileName     = "";
UFC::AnsiString CMegaRiskManagerDataPool::FRMStockWhiteListFileName = "";

bool            CMegaRiskManagerDataPool::FNeedCheckSelfTrading      = false;
bool            CMegaRiskManagerDataPool::FNeedCheckRiskDataDate     = true;
int             CMegaRiskManagerDataPool::FCheckFileInterval         = 3000;

UFC::AnsiString CMegaRiskManagerDataPool::FRMStockWhiteListFilePathName = "";

//UFC::PHashedSet<UFC::AnsiString> CMegaRiskManagerDataPool::FRMHedgeStockSet;
UFC::PHashMap<UFC::AnsiString, CRMHedgeStock*> CMegaRiskManagerDataPool::FHedgeStockNoIndex;

int             CMegaRiskManagerDataPool::FRMCurrentControlFileSeqNo = 0;
int             CMegaRiskManagerDataPool::FRMCurrentDeleteFileSeqNo  = 1;

//------------------------------------------------------------------------------
//bool CMegaRiskManagerDataPool::IsRMHedgeStockExists(const UFC::AnsiString& StockNo)
//{
//    if ((StockNo.Length() > 0) && (FRMHedgeStockSet.Exists(StockNo) == TRUE))
//        return true;
//    else
//        return false;
//}  //CMegaRiskManagerDataPool::IsRMHedgeStockExist()
//------------------------------------------------------------------------------
//bool CMegaRiskManagerDataPool::AddRMHedgeStockToSet(const UFC::AnsiString& StockNo)
//{
//    if ((StockNo.Length() > 0) && (FRMHedgeStockSet.Exists(StockNo) == FALSE))
//    {
//        FRMHedgeStockSet.Add(StockNo);
//        return true;
//    }
//    else
//        return false;
//}  //CMegaRiskManagerDataPool::AddRMHedgeStockToSet()
//------------------------------------------------------------------------------
//void CMegaRiskManagerDataPool::DumpRMHedgeStockSet()
//{
//    UFC::AnsiString curStockNo;
//    int stockCount = FRMHedgeStockSet.ItemCount();
//    if (FRMHedgeStockSet.First(curStockNo) == TRUE)
//    {
//        int curLoopCount = 0;
//        do 
//        {
//            curLoopCount++;
//            FLogPtr->fprintf(" %4d/%4d Hedge Stock[%s].", curLoopCount, stockCount, curStockNo.c_str());
//        } while(FRMHedgeStockSet.Next(curStockNo) == TRUE);
//    }
//    FLogPtr->fprintf(" ");
//}  //CMegaRiskManagerDataPool::DumpRMHedgeStockSet()
//----------------------------------------------------------------------------------------------------------------------
void CMegaRiskManagerDataPool::DumpRMHedgeStockNoIndex()
{
    int dumpCount = 0;
    CRMHedgeStock* curStockPtr = FHedgeStockNoIndex.First();
    while (curStockPtr != 0)
    {
        dumpCount++;
        FLogPtr->fprintf(" %6d %s", dumpCount, curStockPtr->ToString().c_str());        
        curStockPtr = FHedgeStockNoIndex.Next();
    }  //while (curAccountPtr != 0)
    FLogPtr->fprintf(" Dump %d HedgeStock.", dumpCount);
    FLogPtr->fprintf(" ");
}  //CMegaRiskManagerDataPool::DumpRMHedgeStockNoIndex()
//----------------------------------------------------------------------------------------------------------------------
bool CMegaRiskManagerDataPool::LoadRiskManagerHedgeStock()
{
    bool isFileExist = false;
    UFC::AnsiString logHead = "CMegaRiskManagerDataPool::LoadRiskManagerHedgeStock()";
    UFC::AnsiString filePathName = ComposeFilePathName(FRiskManagerDataPath, FRMHedgeStockFileName);
    if (FRMHedgeStockFileName.Length() > 0)
    {
        filePathName = ComposeFilePathName(FRiskManagerDataPath, FRMHedgeStockFileName);
        if (UFC::FileExists(filePathName) == TRUE)
            isFileExist = true;
        else
            FLogPtr->fprintf(" %s Hedge Stock File[%s] Not Exist.", logHead.c_str(), filePathName.c_str());
    }
    else
        FLogPtr->fprintf(" %s Hedge Stock File Name is NULL.", logHead.c_str());
    if (!isFileExist) return isFileExist;
    
    FLogPtr->fprintf(" %s Load Hedge Stock File[%s].", logHead.c_str(), filePathName.c_str());
    UFC::FileStreamEx *hedgeStockFilePtr = 0;
    try 
    {
        hedgeStockFilePtr = new UFC::FileStreamEx(filePathName, "r", FALSE);
        UFC::AnsiString curStockDataStr = "";
        int stockRecordCount = 0;
        BOOL isNotEndOfFile = FALSE;
        do {
            isNotEndOfFile = hedgeStockFilePtr->ReadLine(curStockDataStr);
            curStockDataStr.TrimRight('\r');
            curStockDataStr.TrimRight('\n');
            curStockDataStr.TrimRight(' ');
            if (curStockDataStr.Length() > 0)
            {                
                stockRecordCount++;
                UFC::PStringList *fieldsListPtr = new UFC::PStringList();
                fieldsListPtr->SetStrings(curStockDataStr, ",;", true);  //Keep Empty Field
                int itemCount = fieldsListPtr->ItemCount();
                if (itemCount >= 1)
                {
                    UFC::AnsiString curStockNo = fieldsListPtr->GetItem(0);
                    CRMHedgeStock* curStockPtr = new CRMHedgeStock(curStockNo);
                    if (InsertHedgeStockToNoIndex(curStockNo, curStockPtr, UFC::cstCheck))
                    {
                        if (itemCount >= 2)
                        {
                            for (int i = 1; i < itemCount; i++)
                            {
                                UFC::AnsiString curAccountNo = fieldsListPtr->GetItem(i);
                                if (curAccountNo.Length() > 0)
                                {
                                    curStockPtr->AddAccountToSet(curAccountNo);
                                }
                            }  //for (int i = 1; i < itemCount; i++)
                        }  //if (itemCount >= 2)
                    }
                    else
                    {
                        FLogPtr->fprintf(" %s %d Duplicate Hedge Stock[%s].", logHead.c_str(), stockRecordCount, curStockNo.c_str());
                        delete curStockPtr;
                    }
                }
            }
        } while(isNotEndOfFile == TRUE);
        FLogPtr->fprintf(" %s Load [%d] Hedge Stocks.", logHead.c_str(), FHedgeStockNoIndex.ItemCount());
    }
    catch(UFC::FileException& e)
    {
        hedgeStockFilePtr = 0;
        FLogPtr->fprintf(" %s Load Hedge Stock File[%s] Exception:[%s].", logHead.c_str(), filePathName.c_str(), e.what());
    }  //try
    if (hedgeStockFilePtr != 0) delete hedgeStockFilePtr;
    
    FLogPtr->fprintf(" %s.", logHead.c_str());
    FLogPtr->Flush();
    return isFileExist;
}  //CMegaRiskManagerDataPool::LoadRiskManagerHedgeStock()
//------------------------------------------------------------------------------
bool CMegaRiskManagerDataPool::IsRMHedgeStock(const UFC::AnsiString& StockNo, const UFC::AnsiString& AccountNo)
{
    bool isHedgeStock = false;
    if ((StockNo.Length() > 0) && (AccountNo.Length() > 0))
    {
        CRMHedgeStock* stockPtr = UFC::FindElementPtrFromStrIdxHMap(FHedgeStockNoIndex, StockNo, 0, UFC::rwNotLock);
        if (stockPtr != 0)
        {
            if (stockPtr->GetAccountCount() > 0)
                isHedgeStock = stockPtr->IsAccountExists(AccountNo);
            else
                isHedgeStock = true;
        }
    }
    return isHedgeStock;
}  //CMegaRiskManagerDataPool::IsRMHedgeStock()
//------------------------------------------------------------------------------
int CMegaRiskManagerDataPool::IncreaseRMCurrentControlFileSeqNo()
{
    FRMCurrentControlFileSeqNo++;
    return FRMCurrentControlFileSeqNo;
}  //CMegaRiskManagerDataPool::IncreaseRMCurrentControlFileSeqNo()
//------------------------------------------------------------------------------
int CMegaRiskManagerDataPool::IncreaseRMCurrentDeleteFileSeqNo()
{
    FRMCurrentDeleteFileSeqNo++;
    return FRMCurrentDeleteFileSeqNo;
}  //CMegaRiskManagerDataPool::IncreaseRMCurrentDeleteFileSeqNo()
//------------------------------------------------------------------------------
CMegaRiskManagerDataPool::CMegaRiskManagerDataPool()
{
}  //CMegaRiskManagerDataPool::CMegaRiskManagerDataPool()
//------------------------------------------------------------------------------
CMegaRiskManagerDataPool::~CMegaRiskManagerDataPool()
{
}  //CMegaRiskManagerDataPool::~CMegaRiskManagerDataPool()
//------------------------------------------------------------------------------    
int CMegaRiskManagerDataPool::ClearAccountStockWhiteList(UFC::RWLockTypeEnum LockType)
{
    int clearCount = 0;
    FAccountLock.Lock(LockType);
    CRMInvestorAccount* curAccountPtr = FAccountNoIndex.First();
    while (curAccountPtr != 0)
    {
        curAccountPtr->ClearStockWhiteList(UFC::rwLockForWrite);
        clearCount++;
        curAccountPtr = FAccountNoIndex.Next();
    }  //while (curAccountPtr != 0)
    FAccountLock.Unlock(LockType);
    return clearCount;
}  //CMegaRiskManagerDataPool::ClearAccountStockWhiteList()
//------------------------------------------------------------------------------    
void CMegaRiskManagerDataPool::DumpAccountStockWhiteList(UFC::RWLockTypeEnum LockType)
{
    FAccountLock.Lock(LockType);
    int dumpCount = 0;
    CRMInvestorAccount* curAccountPtr = FAccountNoIndex.First();
    while (curAccountPtr != 0)
    {
        dumpCount++;
        curAccountPtr->LockForRead();
        int stkWhiteListCount = curAccountPtr->GetStockWhiteListCount(UFC::rwNotLock);
        FLogPtr->fprintf(" %4d Account[%s] has %d Stock in WhiteList", dumpCount, curAccountPtr->GetAccountNo(UFC::rwNotLock).c_str(), stkWhiteListCount);
        curAccountPtr->UnLockForRead();
        curAccountPtr = FAccountNoIndex.Next();
    }  //while (curAccountPtr != 0)
    FAccountLock.Unlock(LockType);
    FLogPtr->fprintf(" Dump %d Account WhiteList.", dumpCount);
}  //CMegaRiskManagerDataPool::DumpAccountStockWhiteList()
//------------------------------------------------------------------------------    
void CMegaRiskManagerDataPool::DumpAccountNoIndex(bool DumpDetail, UFC::RWLockTypeEnum LockType)
{
    FAccountLock.Lock(LockType);
    int dumpCount = 0;
    CRMInvestorAccount* curAccountPtr = FAccountNoIndex.First();
    while (curAccountPtr != 0)
    {
        dumpCount++;
        UFC::AnsiString curLogStr = curAccountPtr->ToString(UFC::rwLockForRead);
        FLogPtr->fprintf(" %4d %s", dumpCount, curLogStr.c_str());
        if (DumpDetail)
        {
            curAccountPtr->DumpStockTradingData(FLogPtr, UFC::rwLockForRead);
            FLogPtr->fprintf(" ");
        }
        
        curAccountPtr = FAccountNoIndex.Next();
    }  //while (curAccountPtr != 0)
    FAccountLock.Unlock(LockType);
    FLogPtr->fprintf(" Dump %d Account.", dumpCount);
    FLogPtr->fprintf(" ");
}  //CMegaRiskManagerDataPool::DumpAccountNoIndex()
//------------------------------------------------------------------------------    
void CMegaRiskManagerDataPool::LoadSetting()
{
    CRiskManagerDataPool::LoadSetting();
    UFC::UiniFile *configFilePtr = NULL;
    if (UFC::FileExists(FConfigureFilePathName) == TRUE)
    {
        try
        {
            configFilePtr = new UFC::UiniFile(FConfigureFilePathName);
        }
        catch(UFC::FileException& fe)
        {
            FLogPtr->fprintf(" CMegaRiskManagerDataPool::%s() Load Configure File[%s] failed. %s", __func__, FConfigureFilePathName.c_str(), fe.what());
            configFilePtr = 0;
        }
    }
    else
        FLogPtr->fprintf(" CMegaRiskManagerDataPool::%s() Load Configure File[%s] not Exists", __func__, FConfigureFilePathName.c_str());
    
    if (configFilePtr == NULL)
    {
        FLogPtr->Flush();
        return;
    }
    
    UFC::AnsiString sectionName = "Setting";
    UFC::AnsiString fieldName = "RMFilePath";
    UFC::AnsiString fieldValue = "";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FRiskManagerDataPath = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FRiskManagerDataPath.c_str());
    
    fieldName = "RMControlFilePrefix";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FRMControlFilePrefix = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, Use Default Value[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FRMControlFilePrefix.c_str());
    
    fieldName = "RMControlFileExtension";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FRMControlFileExtension = fieldValue;
    else
    {
        if (FRMControlFileExtension.Length() > 0)
            FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, Use Default Value[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FRMControlFileExtension.c_str());
        else
            FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, Use Default Value[]", __func__, sectionName.c_str(), fieldName.c_str());
    }
    
    fieldName = "RMDeleteFilePrefix";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FRMDeleteFilePrefix = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, Not Exist. Use Default Value[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FRMDeleteFilePrefix.c_str());
    
    
    fieldName = "RMDeleteFileExtension";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FRMDeleteFileExtension = fieldValue;
    else
    {
        if (FRMDeleteFileExtension.Length() > 0)
            FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, Use Default Value[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FRMDeleteFileExtension.c_str());
        else
            FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, Use Default Value[]", __func__, sectionName.c_str(), fieldName.c_str());
    }

    fieldName = "RMHedgeStockFileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FRMHedgeStockFileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, Use Default Value[]", __func__, sectionName.c_str(), fieldName.c_str());
    
    fieldName = "RMWhiteListFileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FRMStockWhiteListFileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, Use Default Value[]", __func__, sectionName.c_str(), fieldName.c_str());
    
    fieldName = "MainBrokerID";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FRMMainBrokerID = fieldValue;
    else
    {
        if (FRMMainBrokerID.Length() > 0)
            FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, Use Default Value[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FRMMainBrokerID.c_str());
        else
            FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, Use Default Value[]", __func__, sectionName.c_str(), fieldName.c_str());
    }
    
    fieldName = "CheckSelfTrading";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
    {
        if ((fieldValue == "Y") || (fieldValue == "y") || (fieldValue == "1")) 
            FNeedCheckSelfTrading = true;
        else
            FNeedCheckSelfTrading = false;
    }
    else
    {
        if (FNeedCheckSelfTrading)
            FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, Use Default Value [true]", __func__, sectionName.c_str(), fieldName.c_str());
        else
            FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, Use Default Value [false]", __func__, sectionName.c_str(), fieldName.c_str());
    }

    fieldName = "CheckRiskDataDate";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
    {
        if ((fieldValue == "Y") || (fieldValue == "y") || (fieldValue == "1")) 
            FNeedCheckRiskDataDate = true;
        else
            FNeedCheckRiskDataDate = false;
    }
    else
    {
        if (FNeedCheckRiskDataDate)
            FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, Use Default Value [true]", __func__, sectionName.c_str(), fieldName.c_str());
        else
            FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, Use Default Value [false]", __func__, sectionName.c_str(), fieldName.c_str());
    }


    fieldName = "CheckFileInterval";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FCheckFileInterval = fieldValue.ToInt() * 1000;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, Use Default Value[%d]", __func__, sectionName.c_str(), fieldName.c_str(), FCheckFileInterval);
    
    sectionName = "RMAccounts";
    UFC::Section *accountsSecPtr = configFilePtr->GetSection(sectionName);
    if (accountsSecPtr != 0)
    {
        for (int curIndex = 0; curIndex < accountsSecPtr->ItemCount(); curIndex++)
        {
            UFC::AnsiString curAccountNo = "", curAccountFdValueCSV = "";
            if (accountsSecPtr->GetNameValue(curIndex, curAccountNo, curAccountFdValueCSV) == TRUE)
            {
                UFC::PStringList fdValueList;
                fdValueList.SetStrings(curAccountFdValueCSV, ",\n", true);  //Keep Empty Field
                UFC::AnsiString curfdValueStr = "";
                
                int curDayTradeType = 0;  //no DayTrade
                if (fdValueList.ItemCount() >= 1)
                {
                    curfdValueStr = fdValueList[0];
                    if (curfdValueStr.Length() > 0)
                        curDayTradeType = curfdValueStr.ToInt();
                }
                
                bool curNeedCheckSelfTrade = false;  //not Check Self Trade
                if (fdValueList.ItemCount() >= 2)
                {
                    curfdValueStr = fdValueList[1];
                    if ((curfdValueStr.Length() > 0) && (curfdValueStr.ToInt() == 1))
                        curNeedCheckSelfTrade = true;
                }
                                
                bool curNeedCheckSellPosition = true;  //Check Position
                if (fdValueList.ItemCount() >= 3)
                {
                    curfdValueStr = fdValueList[2];
                    if ((curfdValueStr.Length() > 0) && (curfdValueStr.ToInt() != 1))
                        curNeedCheckSellPosition = false;
                }
                
                double curStockAmountLimit = -1.0;  //not Control
                if (fdValueList.ItemCount() >= 4)
                {
                    curfdValueStr = fdValueList[3];
                    if (curfdValueStr.Length() > 0)
                        curStockAmountLimit = curfdValueStr.ToDouble();
                }
                
                bool curNeedCheckStockWhiteList = false;  //not Check Stock White List
                if (fdValueList.ItemCount() >= 5)
                {
                    curfdValueStr = fdValueList[4];
                    if ((curfdValueStr.Length() > 0) && (curfdValueStr.ToInt() == 1))
                        curNeedCheckStockWhiteList = true;
                }
                
                CRMInvestorAccount* curAccountPtr = new CRMInvestorAccount(curAccountNo);
                curAccountPtr->LockForWrite();
                curAccountPtr->SetDayTradeType(curDayTradeType, UFC::rwNotLock);
                curAccountPtr->SetNeedCheckSelfTrade(curNeedCheckSelfTrade, UFC::rwNotLock);
                curAccountPtr->SetNeedCheckSellPosition(curNeedCheckSellPosition, UFC::rwNotLock);
                curAccountPtr->SetStockAmountLimit(curStockAmountLimit, UFC::rwNotLock);
                curAccountPtr->SetNeedCheckStockWhiteList(curNeedCheckStockWhiteList, UFC::rwNotLock);
                curAccountPtr->UnLockForWrite();
                if (!InsertAccountToNoIndex(curAccountNo, curAccountPtr, UFC::cstCheck, UFC::rwLockForWrite))
                {
                    FLogPtr->fprintf(" CRiskManagerDataPool::%s() %d Duplicate AccountNo[%s].", __func__, curIndex, curAccountNo.c_str());
                    delete curAccountPtr;
                }
            }
            else
                FLogPtr->fprintf(" CRiskManagerDataPool::%s() %d Item of Section %s not Exist.", __func__, curIndex, sectionName.c_str());
        }  //for (int curIndex = 0; curIndex < accountsSecPtr->ItemCount(); curIndex++)
    }
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() Section %s not Setting.", __func__, sectionName.c_str());
    
    delete configFilePtr;
    
    if (FRMStockWhiteListFileName.Length() > 0)
        FRMStockWhiteListFilePathName = ComposeFilePathName(FRiskManagerDataPath, FRMStockWhiteListFileName);
}  //CMegaRiskManagerDataPool::LoadSetting()
//------------------------------------------------------------------------------    
void CMegaRiskManagerDataPool::PrintSetting()
{
    CRiskManagerDataPool::PrintSetting();
    FLogPtr->fprintf(" ______________________________________________");
    FLogPtr->fprintf(" Risk Manager Data Path [%s]", FRiskManagerDataPath.c_str());
    if (FRMControlFileExtension.Length() > 0)
        FLogPtr->fprintf(" Risk Manager Control File Prefix[%s] Extension[%s]", FRMControlFilePrefix.c_str(), FRMControlFileExtension.c_str());
    else
        FLogPtr->fprintf(" Risk Manager Control File Prefix[%s] Extension[]", FRMControlFilePrefix.c_str());
        
    if (FRMDeleteFileExtension.Length() > 0)
        FLogPtr->fprintf(" Risk Manager Delete File Prefix[%s] Extension[%s]", FRMDeleteFilePrefix.c_str(), FRMDeleteFileExtension.c_str());
    else
        FLogPtr->fprintf(" Risk Manager Delete File Prefix[%s] Extension[]", FRMDeleteFilePrefix.c_str());
        
    if (FRMHedgeStockFileName.Length() > 0)
        FLogPtr->fprintf(" Risk Manager Hedge Stock FileName[%s]", FRMHedgeStockFileName.c_str());
    else
        FLogPtr->fprintf(" Risk Manager Hedge Stock FileName[]");
    
    if (FRMStockWhiteListFilePathName.Length() > 0)
        FLogPtr->fprintf(" Risk Manager Stock White List FileName[%s]", FRMStockWhiteListFilePathName.c_str());
    else
        FLogPtr->fprintf(" Risk Manager Stock White List FileName[]");
    
    if (FRMMainBrokerID.Length() > 0)
        FLogPtr->fprintf(" Risk Manager Main Broker ID[%s]", FRMMainBrokerID.c_str());
    else
        FLogPtr->fprintf(" Risk Manager Main Broker ID[]");
    
    if (FNeedCheckSelfTrading)
        FLogPtr->fprintf(" Need Check Self Trading.");
    else
        FLogPtr->fprintf(" Need not Check Self Trading.");
    
    if (FNeedCheckRiskDataDate)
        FLogPtr->fprintf(" Need Check Risk Data Date.");
    else
        FLogPtr->fprintf(" Need not Check Risk Data Date.");
    
    FLogPtr->fprintf(" Check File Interval[%d].", FCheckFileInterval);
    
    DumpAccountNoIndex(false, UFC::rwLockForRead);  //not Dump Condition
    FLogPtr->fprintf(" ______________________________________________");
}  //CMegaRiskManagerDataPool::PrintSetting()
//------------------------------------------------------------------------------    
UFC::AnsiString CMegaRiskManagerDataPool::ComposeDataFilePathName(const UFC::AnsiString& FilePath, const UFC::AnsiString& FileNamePrefix, const UFC::AnsiString& FileNameExtension, int FileSeqNo)
{
    UFC::PStringBuffer pathNameBuffer;
    int pathLength = FilePath.Length();
    if (pathLength > 0)
    {
        pathNameBuffer.Append(FilePath);
        if (FilePath[pathLength - 1] != '/') pathNameBuffer.Append('/');
    }
    
    if (FileNamePrefix.Length() > 0) pathNameBuffer.Append(FileNamePrefix);
    if (FileSeqNo >= 0) pathNameBuffer.AppendPrintf("%03d", FileSeqNo);
    
    if (FileNameExtension.Length() > 0)
    {
        pathNameBuffer.Append('.');
        if (FileNamePrefix.Length() > 0) pathNameBuffer.Append(FileNameExtension);
    }
    
    return pathNameBuffer.ToString();
}  //CMegaRiskManagerDataPool::ComposeFileName()
//------------------------------------------------------------------------------    
bool CMegaRiskManagerDataPool::LoadRiskManagerControlData(bool IsUpdate)
{
    bool isFileExist = false;
    UFC::AnsiString logHead = "";
    if (!IsUpdate)
        logHead = "CMegaRiskManagerDataPool::LoadRiskManagerControlData()";
    else
        logHead = "CMegaRiskManagerDataPool::UpdateRiskManagerControlData()";
    UFC::AnsiString logMsg = "";
    UFC::AnsiString filePathName = "";
    if (!IsUpdate)
        filePathName = ComposeDataFilePathName(FRiskManagerDataPath, FRMControlFilePrefix, FRMControlFileExtension, -1);
    else
        filePathName = ComposeDataFilePathName(FRiskManagerDataPath, FRMControlFilePrefix, FRMControlFileExtension, FRMCurrentControlFileSeqNo);
    
    if (UFC::FileExists(filePathName) == FALSE)
    {
        if (!IsUpdate)
        {
            FLogPtr->fprintf(" %s Risk Control File[%s] Not Exist.", logHead.c_str(), filePathName.c_str());
            FLogPtr->Flush();
        }
        return isFileExist;
    }  //if (UFC::FileExists(fullFileName) == FALSE)

    UFC::SleepMS(500);    
    FLogPtr->fprintf(" %s Load Risk Control File[%s].", logHead.c_str(), filePathName.c_str());
    isFileExist = true;
    UFC::AnsiString todayDateStr;
    UFC::GetYYYYMMDD(todayDateStr, FALSE);  //Without Slash
    try 
    {
        UFC::FileStreamEx* ctrlFilePtr = new UFC::FileStreamEx(filePathName, "r", FALSE);
        UFC::AnsiString stockDataStr = "";
        int readRecordCount = 0;
        int stockRecordCount = 0;
        int updateCount = 0;
        BOOL isNotEndOfFile = FALSE;
        do {
            isNotEndOfFile = ctrlFilePtr->ReadLine(stockDataStr);
            int recordLength = stockDataStr.Length();
            readRecordCount++;
            if (recordLength < 50)
            {
                if (recordLength > 0)
                    FLogPtr->fprintf(" %s Ignore %d [%s]%d.", logHead.c_str(), readRecordCount, stockDataStr.c_str(), recordLength);
                continue;
            }
//0         1         2         3         4         5         6         7
//01234567890123456789012345678901234567890123456789012345678901234567890
//201411140050  twtw50          00112200088888881E    700T0000000000
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
                    
            UFC::AnsiString curBrokerID = stockDataStr.SubString(52, 4);
            curBrokerID.TrimRight(' ');
                    
            char curOrderType = stockDataStr[56];
                    
            UFC::AnsiString curAvailableSellShortBuyQtyStr = stockDataStr.SubString(57, 9);
            curAvailableSellShortBuyQtyStr.TrimRight(' ');
            int curAvailableSellShortBuyQty = 0;
            if (curAvailableSellShortBuyQtyStr.Length() > 0) curAvailableSellShortBuyQty = curAvailableSellShortBuyQtyStr.ToInt();
            
            if (FNeedCheckRiskDataDate && (curDataDate != todayDateStr))
            {
                FLogPtr->fprintf(" %s %d Wrong Risk Data Date [%s].", logHead.c_str(), readRecordCount, stockDataStr.c_str());
                continue;
            }
                    
//FLogPtr->fprintf(" %s %d Acc[%s] Stk[%s][%s][%s]%c AvailableQty[S:%s B:%s] Date[%s] isNotEOF[%d].",
//                 logHead.c_str(), stockRecordCount, 
//                 curAccountNo.c_str(), curStockNo.c_str(), curWarrantNo.c_str(), curBrokerID.c_str(), curOrderType, 
//                 curAvailableSellQtyStr.c_str(),  curAvailableSellShortBuyQtyStr.c_str(), 
//                 curDataDate.c_str(), isNotEndOfFile);
            CRMInvestorAccount *curAccountPtr = FindAccountFromNoIndex(curAccountNo, UFC::rwLockForRead);
            if (curAccountPtr == 0)
            {
                curAccountPtr = new CRMInvestorAccount(curAccountNo);
                curAccountPtr->SetDayTradeType(0, UFC::rwNotLock);                 //no DayTrade
                curAccountPtr->SetNeedCheckSelfTrade(false, UFC::rwNotLock);       //not Check Self Trade
                curAccountPtr->SetNeedCheckSellPosition(true, UFC::rwNotLock);     //Check Position
                curAccountPtr->SetStockAmountLimit(-1.0, UFC::rwNotLock);          //not Check Amount Limit
                curAccountPtr->SetNeedCheckStockWhiteList(false, UFC::rwNotLock);  //not Check White List
                if (InsertAccountToNoIndex(curAccountNo, curAccountPtr, UFC::cstNotCheck, UFC::rwLockForWrite))
                    FLogPtr->fprintf(" %s %d Create %s.", logHead.c_str(), readRecordCount, curAccountPtr->ToString(UFC::rwNotLock).c_str());
                else
                {
                    FLogPtr->fprintf(" %s %d Insert Account[%s] failed.", logHead.c_str(), readRecordCount, curAccountNo.c_str());
                    delete curAccountPtr;
                    continue;
                }            
            }  //if (curAccountPtr == 0)
                    
            CRMStockTradingData *curStockDataPtr = curAccountPtr->FindOrCreateStockTradingData(curStockNo, UFC::rwLockForRead);
            if (curStockDataPtr == 0) 
            {
                FLogPtr->fprintf(" %s %d Wrong Account[%s] Stock[%s].", logHead.c_str(), readRecordCount, curAccountNo.c_str(), curStockNo.c_str());
                continue;
            }
            curStockDataPtr->SetAccountNo(curAccountNo, UFC::rwLockForRead);

            CRMBrokerStockCondition *curConditionPtr = curStockDataPtr->FindCondition(curWarrantNo, curBrokerID, curOrderType, UFC::rwLockForRead);
            if (curConditionPtr == 0)
            {
                bool isSellShort = false;
                if (curBrokerID != FRMMainBrokerID) isSellShort = true;
                CRMBrokerStockCondition *newConditionPtr = new CRMBrokerStockCondition(curWarrantNo, curBrokerID, curOrderType, curAvailableSellQty, curAvailableSellShortBuyQty, curDataDate, isSellShort);
                newConditionPtr->SetAccountNo(curAccountNo, UFC::rwNotLock);
                newConditionPtr->SetStockNo(curStockNo, UFC::rwNotLock);
                if (curStockDataPtr->InsertCondition(curWarrantNo, curBrokerID, curOrderType, newConditionPtr, UFC::cstCheck, UFC::rwLockForWrite))
                    updateCount++;
                else
                {
                    FLogPtr->fprintf(" %s %d Insert StockCondition[%s][%s][%s]%c into Account[%s] failed.", logHead.c_str(), readRecordCount, curStockNo.c_str(), curWarrantNo.c_str(), curBrokerID.c_str(), curOrderType, curAccountNo.c_str());
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
                        curConditionPtr->SetAvailableSellQty(newAvailableSellQty, UFC::rwNotLock);
                    }
                    
                    if (curAvailableSellShortBuyQty != 0)
                    {
                        int oldAvailableSellShortBuyQty = curConditionPtr->GetAvailableBuyQty(UFC::rwNotLock);
                        int newAvailableSellShortBuyQty = oldAvailableSellShortBuyQty + curAvailableSellShortBuyQty;
                        curConditionPtr->SetAvailableBuyQty(newAvailableSellShortBuyQty, UFC::rwNotLock);
                    }
                    
                    if ((curAvailableSellQty != 0) || (curAvailableSellShortBuyQty != 0))
                        updateCount++;
                    curConditionPtr->UnLockForWrite();
                }
                else
                {
                    FLogPtr->fprintf(" %s %d Ignore Duplicate Account[%s] StockCondition[%s][%s][%s]%c.", logHead.c_str(), readRecordCount, curAccountNo.c_str(), curStockNo.c_str(), curWarrantNo.c_str(), curBrokerID.c_str(), curOrderType);
                    curAvailableSellQty = 0;
                }
            }  //if (curConditionPtr == 0)
            
            if (curAvailableSellQty != 0)
            {
                SecurityInfo* curStkInfoPtr = FindStock(curStockNo);
                if (curStkInfoPtr != 0)
                {
                    if (curStkInfoPtr->GetMarket() == FMarket)
                    {
                        double positionAmount = curStkInfoPtr->GetLdcPrice() * static_cast<double>(curAvailableSellQty);
                        curAccountPtr->LockForWrite();
                        double oldPositionAmount = curAccountPtr->GetPositionAmount(UFC::rwNotLock);
                        double newPositionAmount = oldPositionAmount + positionAmount;
                        if (newPositionAmount < 0.0) newPositionAmount = 0.0;
                        curAccountPtr->SetPositionAmount(newPositionAmount, UFC::rwNotLock);
                        curAccountPtr->UnLockForWrite();
                    }
                }
                else
                    FLogPtr->fprintf(" %s %d Stock of Account[%s] StockCondition[%s][%s][%s]%c not Found.", logHead.c_str(), readRecordCount, curAccountNo.c_str(), curStockNo.c_str(), curWarrantNo.c_str(), curBrokerID.c_str(), curOrderType);
            }
        } while(isNotEndOfFile == TRUE);
        
        delete ctrlFilePtr;
        if (!IsUpdate)
            logMsg.Printf("Process %d Record and Create %d Condition from File[%s].", stockRecordCount, updateCount, filePathName.c_str());
        else
            logMsg.Printf("Process %d Record and Update %d Condition from File[%s].", stockRecordCount, updateCount, filePathName.c_str());
    }
    catch(UFC::FileException& e)
    {
        logMsg.Printf("Process Risk Control File[%s] Exception:[%s].", filePathName.c_str(), e.what());
    }  //try
    
    if (logMsg.Length() > 0) FLogPtr->fprintf(" %s %s", logHead.c_str(), logMsg.c_str());
    FLogPtr->fprintf(" %s", logHead.c_str());
    FLogPtr->Flush();
    return isFileExist;
}  //CMegaRiskManagerDataPool::LoadRiskManagerControlData()
//----------------------------------------------------------------------------------------------------------------------
bool CMegaRiskManagerDataPool::LoadOrderDeleteData()
{
    bool isFileExist = false;
    UFC::AnsiString logHead = "CMegaRiskManagerDataPool::LoadOrderDeleteData()";
    UFC::AnsiString filePathName = ComposeDataFilePathName(FRiskManagerDataPath, FRMDeleteFilePrefix, FRMDeleteFileExtension, FRMCurrentDeleteFileSeqNo);
    if (filePathName.Length() <= 0)
    {
        FLogPtr->fprintf(" %s File Name is null.", logHead.c_str());
        FLogPtr->Flush();
        return isFileExist;
    }
    
    UFC::AnsiString logMsg = "";
    if (UFC::FileExists(filePathName) == FALSE)
    {
//        FLogPtr->fprintf(" %s Order Delete File[%s] not Exist.", logHead.c_str(), filePathName.c_str());
//        FLogPtr->Flush();
        return isFileExist;
    }  //if (UFC::FileExists(fullFileName) == FALSE)
    
    UFC::SleepMS(500);
    FLogPtr->fprintf(" %s Load Order Delete File[%s].", logHead.c_str(), filePathName.c_str());
    isFileExist = true;
    UFC::AnsiString todayDateStr;
    UFC::GetYYYYMMDD(todayDateStr, FALSE);
    try 
    {
        UFC::FileStreamEx *delFilePtr = new UFC::FileStreamEx(filePathName, "r", FALSE);
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
            
            UFC::AnsiString curOrderID = "";
            curOrderID = deleteDataStr.SubString(8, 5);
            curOrderID.TrimRight(' ');
            
            UFC::AnsiString curBrokerID = "";
            curBrokerID = deleteDataStr.SubString(13, 4);
            curBrokerID.TrimRight(' ');
            
            UFC::AnsiString curDeleteQtyStr = "";
            int curDeleteQty = 0;
            curDeleteQtyStr = deleteDataStr.SubString(17, 9);
            curDeleteQty = curDeleteQtyStr.ToInt();
            
            if (FNeedCheckRiskDataDate && (curDataDate != todayDateStr))
            {
                FLogPtr->fprintf(" %s %d Wrong Risk Data Date [%s].", logHead.c_str(), deleteRecordCount, deleteDataStr.c_str());
                continue;
            }
            
            FLogPtr->fprintf(" %s %d Delete Order[%s] BrokerID[%s] DelQty[%d] Date[%s] isNotEOF[%d].",
                             logHead.c_str(), deleteRecordCount, curOrderID.c_str(), curBrokerID.c_str(), curDeleteQty, curDataDate.c_str(), isNotEndOfFile);
            
            CRMOrderSummary *targetOrderPtr = FindOrderFromIDIndex(curOrderID, UFC::rwLockForRead);
            if (targetOrderPtr == 0)
            {
                FLogPtr->fprintf(" %s %d Order[%s] not Exist.", logHead.c_str(), deleteRecordCount, curOrderID.c_str());
                continue;
            }
            
            targetOrderPtr->LockForRead();
            UFC::AnsiString curAccountNo = targetOrderPtr->GetAccountNo(UFC::rwNotLock);
            UFC::AnsiString curStockNo = targetOrderPtr->GetStockNo(UFC::rwNotLock);
            UFC::AnsiString curWarrantNo = targetOrderPtr->GetWarrantNo(UFC::rwNotLock);
            char curSide = targetOrderPtr->GetSide(UFC::rwNotLock);
            char curTWSEOrderType = targetOrderPtr->GetTWSEOrderType(UFC::rwNotLock);
            targetOrderPtr->UnLockForRead();

            CRMInvestorAccount *curAccountPtr = FindAccountFromNoIndex(curAccountNo, UFC::rwLockForRead);
            if (curAccountPtr == 0)
            {
                FLogPtr->fprintf(" %s %d Order[%s] Account[%s] not Exist.", logHead.c_str(), deleteRecordCount, curOrderID.c_str(), curAccountNo.c_str());
                continue;
            }  //if (curAccountPtr == 0)
            
            CRMStockTradingData *curStockDataPtr = curAccountPtr->FindStockTradingData(curStockNo, UFC::rwLockForRead);
            if (curStockDataPtr == 0)
            {
                FLogPtr->fprintf(" %s %d Account[%s] has no Stock[%s] data, Order[%s].", logHead.c_str(), deleteRecordCount, curAccountNo.c_str(), curStockNo.c_str(), curOrderID.c_str());
                continue;
            }  //if (curStockDataPtr == 0)
            
            targetOrderPtr->SetLeavesQty(0, UFC::rwLockForWrite);
            CRMOrderSummary *cancelOrderPtr = 0;
            bool isRegularOrder = false;
            bool isIntraDayOddOrder = false;
            bool isOtherOrder = false;
            if (curSide == 'B')
            {
                cancelOrderPtr = curStockDataPtr->RemoveRegularBuyOrder(curOrderID, UFC::rwLockForWrite);
                if (cancelOrderPtr == 0) 
                {
                    cancelOrderPtr = curStockDataPtr->RemoveIntraDayOddBuyOrder(curOrderID, UFC::rwLockForWrite);
                    if (cancelOrderPtr == 0)
                    {
                        cancelOrderPtr = curStockDataPtr->RemoveBuyOrder(curOrderID, UFC::rwLockForWrite);
                        if (cancelOrderPtr != 0) isOtherOrder = true;
                    }
                    else
                        isIntraDayOddOrder = true;
                }
                else
                    isRegularOrder = true;
                
                if (cancelOrderPtr != 0)
                {
                    curAccountPtr->LockForWrite();
                    double newBuyOrderAmount = curAccountPtr->GetBuyOrderAmount(UFC::rwNotLock) - cancelOrderPtr->GetOrderAmount(UFC::rwLockForRead);
                    curAccountPtr->SetBuyOrderAmount(newBuyOrderAmount, UFC::rwNotLock);
                    curAccountPtr->UnLockForWrite();
                }
            }
            else if (curSide == 'S')
            {
                cancelOrderPtr = curStockDataPtr->RemoveRegularSellOrder(curOrderID, UFC::rwLockForWrite);
                if (cancelOrderPtr == 0) 
                {
                    cancelOrderPtr = curStockDataPtr->RemoveIntraDayOddSellOrder(curOrderID, UFC::rwLockForWrite);
                    if (cancelOrderPtr == 0)
                    {
                        cancelOrderPtr = curStockDataPtr->RemoveSellOrder(curOrderID, UFC::rwLockForWrite);
                        if (cancelOrderPtr != 0) isOtherOrder = true;
                    }
                    else
                        isIntraDayOddOrder = true;
                }
                else
                    isRegularOrder = true;
                
                if (cancelOrderPtr != 0)
                {
                    curAccountPtr->LockForWrite();
                    double newSellOrderAmount = curAccountPtr->GetSellOrderAmount(UFC::rwNotLock) - cancelOrderPtr->GetOrderAmount(UFC::rwLockForRead);
                    curAccountPtr->SetSellOrderAmount(newSellOrderAmount, UFC::rwNotLock);
                    curAccountPtr->UnLockForWrite();
                }
            }  //if (curSide == 'B')
            
            if (cancelOrderPtr != 0)
            {
                removeOrderCount++;
                FLogPtr->fprintf(" %s %d Stock: %s, Order[%s].", logHead.c_str(), deleteRecordCount, curStockDataPtr->ToString(UFC::rwLockForRead).c_str(), curOrderID.c_str());
            }
            else
            {
                FLogPtr->fprintf(" %s %d Remove Order[%s] from account[%s] Stock[%s] failed.", logHead.c_str(), deleteRecordCount, curOrderID.c_str(), curAccountNo.c_str(), curStockNo.c_str());
                continue;
            }
            
            CRMBrokerStockCondition *curConditionPtr = curStockDataPtr->FindCondition(curWarrantNo, curBrokerID, curTWSEOrderType, UFC::rwLockForRead);
            if (curConditionPtr != 0)
            {
                curConditionPtr->LockForWrite();
                if (curSide == 'B')
                {
                    if (isRegularOrder)
                    {
                        int curRegularOrderBuyQty = curConditionPtr->GetRegularOrderBuyQty(UFC::rwNotLock);
                        if (curRegularOrderBuyQty >= curDeleteQty)
                            curConditionPtr->IncreaseRegularOrderBuyQty(-curDeleteQty, UFC::rwNotLock);
                        else
                        {
                            curConditionPtr->SetRegularOrderBuyQty(0, UFC::rwNotLock);
                            FLogPtr->fprintf(" %s %d Regular OrderBuyQty[%d] is Less then deleteQty[%d], set Regular OrderBuyQty to 0.", logHead.c_str(), deleteRecordCount, curRegularOrderBuyQty, curDeleteQty);
                        }
                    }
                    else if (isIntraDayOddOrder)
                    {
                        int curIntraDayOddOrderBuyQty = curConditionPtr->GetIntraDayOddOrderBuyQty(UFC::rwNotLock);
                        if (curIntraDayOddOrderBuyQty >= curDeleteQty)
                            curConditionPtr->IncreaseIntraDayOddOrderBuyQty(-curDeleteQty, UFC::rwNotLock);
                        else
                        {
                            curConditionPtr->SetIntraDayOddOrderBuyQty(0, UFC::rwNotLock);
                            FLogPtr->fprintf(" %s %d IntradayOdd OrderBuyQty[%d] is Less then deleteQty[%d], set IntradayOdd OrderBuyQty to 0.", logHead.c_str(), deleteRecordCount, curIntraDayOddOrderBuyQty, curDeleteQty);
                        }
                    }
                    else if (isOtherOrder)
                    {
                        int curOrderBuyQty = curConditionPtr->GetOrderBuyQty(UFC::rwNotLock);
                        if (curOrderBuyQty >= curDeleteQty)
                            curConditionPtr->IncreaseOrderBuyQty(-curDeleteQty, UFC::rwNotLock);
                        else
                        {
                            curConditionPtr->SetOrderBuyQty(0, UFC::rwNotLock);
                            FLogPtr->fprintf(" %s %d OrderBuyQty[%d] is Less then deleteQty[%d], set OrderBuyQty to 0.", logHead.c_str(), deleteRecordCount, curOrderBuyQty, curDeleteQty);
                        }
                    }
                }
                else
                {
                    if (isRegularOrder)
                    {
                        int curRegularOrderSellQty = curConditionPtr->GetRegularOrderSellQty(UFC::rwNotLock);
                        if (curRegularOrderSellQty >= curDeleteQty)
                            curConditionPtr->IncreaseRegularOrderSellQty(-curDeleteQty, UFC::rwNotLock);
                        else
                        {
                            curConditionPtr->SetRegularOrderSellQty(0, UFC::rwNotLock);
                            FLogPtr->fprintf(" %s %d Regular OrderSellQty[%d] is Less then deleteQty[%d], set Regular OrderBuyQty to 0.", logHead.c_str(), deleteRecordCount, curRegularOrderSellQty, curDeleteQty);
                        }
                    }
                    else if (isIntraDayOddOrder)
                    {
                        int curIntraDayOddOrderSellQty = curConditionPtr->GetIntraDayOddOrderSellQty(UFC::rwNotLock);
                        if (curIntraDayOddOrderSellQty >= curDeleteQty)
                            curConditionPtr->IncreaseIntraDayOddOrderSellQty(-curDeleteQty, UFC::rwNotLock);
                        else
                        {
                            curConditionPtr->SetIntraDayOddOrderSellQty(0, UFC::rwNotLock);
                            FLogPtr->fprintf(" %s %d IntradayOdd OrderSellQty[%d] is Less then deleteQty[%d], set IntradayOdd OrderSellQty to 0.", logHead.c_str(), deleteRecordCount, curIntraDayOddOrderSellQty, curDeleteQty);
                        }
                    }
                    else
                    {
                        int curOrderSellQty = curConditionPtr->GetOrderSellQty(UFC::rwNotLock);
                        if (curOrderSellQty >= curDeleteQty)
                            curConditionPtr->IncreaseOrderSellQty(-curDeleteQty, UFC::rwNotLock);
                        else
                        {
                            curConditionPtr->SetOrderSellQty(0, UFC::rwNotLock);
                            FLogPtr->fprintf(" %s %d OrderSellQty[%d] is Less then deleteQty[%d], set OrderSellQty to 0.", logHead.c_str(), deleteRecordCount, curOrderSellQty, curDeleteQty);
                        }
                    }
                }  //if (curSide == 'B')
                updateConditionCount++;
                FLogPtr->fprintf(" %s %d Condition: %s, order[%s]", logHead.c_str(), deleteRecordCount, curConditionPtr->ToString(UFC::rwNotLock).c_str(), curOrderID.c_str());
                curConditionPtr->UnLockForWrite();
            }
            else
                FLogPtr->fprintf(" %s %d Account[%s] Stock[%s][%s][%s]%c not found, Order[%s]", logHead.c_str(), deleteRecordCount, curAccountNo.c_str(), curStockNo.c_str(), curWarrantNo.c_str(), curBrokerID.c_str(), curTWSEOrderType, curOrderID.c_str());
        } while(isNotEndOfFile == TRUE);
        
        delete delFilePtr;
        logMsg.Printf("Process %d Record and remove %d Orders and update %d Conditions from File[%s].", deleteRecordCount, removeOrderCount, updateConditionCount, filePathName.c_str());
    }
    catch(UFC::FileException& e)
    {
        logMsg.Printf("Process Order Delete File[%s] Exception:[%s].", filePathName.c_str(), e.what());
    }  //try

    if (logMsg.Length() > 0)
        FLogPtr->fprintf(" %s %s", logHead.c_str(), logMsg.c_str());
    FLogPtr->fprintf(" %s", logHead.c_str());
    FLogPtr->Flush();
    return isFileExist;
}  //CMegaRiskManagerDataPool::LoadOrderDeleteData()
//----------------------------------------------------------------------------------------------------------------------
bool CMegaRiskManagerDataPool::LoadStockWhiteList()
{
    bool isFileExist = false;
    UFC::AnsiString logHead = "CMegaRiskManagerDataPool::LoadStockWhiteList()";
    if (FRMStockWhiteListFilePathName.Length() <= 0)
    {
        FLogPtr->fprintf(" %s Stock WhiteList File Name is null.", logHead.c_str());
        FLogPtr->Flush();
        return isFileExist;
    }
        
    if (UFC::FileExists(FRMStockWhiteListFilePathName) == FALSE)
    {
        FLogPtr->fprintf(" %s Stock WhiteList File[%s] not Exist.", logHead.c_str(), FRMStockWhiteListFilePathName.c_str());
        FLogPtr->Flush();
        return isFileExist;
    }  //if (UFC::FileExists(fullFileName) == FALSE)
    
    UFC::SleepMS(500);
    FLogPtr->fprintf(" %s Load Stock WhiteList File[%s].", logHead.c_str(), FRMStockWhiteListFilePathName.c_str());
    isFileExist = true;
    UFC::AnsiString logMsg = "";
    UFC::AnsiString todayDateStr;
    UFC::GetYYYYMMDD(todayDateStr, FALSE);
    try 
    {
        UFC::FileStreamEx *delFilePtr = new UFC::FileStreamEx(FRMStockWhiteListFilePathName, "r", FALSE);
        ClearAccountStockWhiteList(UFC::rwLockForWrite);
        UFC::AnsiString dataStr = "";
        int readRecordCount = 0;
        int whiteListCount = 0;
        BOOL isNotEndOfFile = FALSE;
        do {
            isNotEndOfFile = delFilePtr->ReadLine(dataStr);
            int recordLength = dataStr.Length();
            dataStr.TrimRight('\n');
            dataStr.TrimRight('\r');
            
            
            
            if (recordLength < 53) continue;
            readRecordCount++;
            
            UFC::AnsiString curDataDate = "";
            curDataDate = dataStr.SubString(0, 8);
            curDataDate.TrimRight(' ');
            
            UFC::AnsiString curAccountNo = "";
            curAccountNo = dataStr.SubString(8, 7);
            curAccountNo.TrimRight(' ');
            
            UFC::AnsiString curStockNo = "";
            curStockNo = dataStr.SubString(15, 6);
            curStockNo.TrimRight(' ');
            
            if (FNeedCheckRiskDataDate && (curDataDate != todayDateStr))
            {
                FLogPtr->fprintf(" %s %d Wrong Data Date [%s].", logHead.c_str(), readRecordCount, dataStr.c_str());
                continue;
            }
            
            CRMInvestorAccount* curAccountPtr = FindAccountFromNoIndex(curAccountNo, UFC::rwLockForRead);
            if (curAccountPtr != 0)
            {
                curAccountPtr->LockForWrite();
                if (!curAccountPtr->IsStockInWhiteList(curStockNo, UFC::rwNotLock))
                {
                    whiteListCount++;
                    curAccountPtr->AddStockToWhiteList(curStockNo, UFC::rwNotLock);
                }
                else
                    FLogPtr->fprintf(" %s %d Duplicate Stock[%s] in Account[%s] Stock WhiteList.", logHead.c_str(), readRecordCount, curStockNo.c_str(), curAccountNo.c_str());
                curAccountPtr->UnLockForWrite();
            }
            
//            FLogPtr->fprintf(" %s %d Account[%s] WhiteList Stock[%s] Date[%s] isNotEOF[%d].",
//                             logHead.c_str(), readRecordCount, curAccountNo.c_str(), curStockNo.c_str(), curDataDate.c_str(), isNotEndOfFile);
        } while(isNotEndOfFile == TRUE);
        
        delete delFilePtr;
        logMsg.Printf("Process %d Record and update %d WhiteList Stock from File[%s].", readRecordCount, whiteListCount, FRMStockWhiteListFilePathName.c_str());
    }
    catch(UFC::FileException& e)
    {
        logMsg.Printf("Process Stock WhiteList File[%s] Exception:[%s].", FRMStockWhiteListFilePathName.c_str(), e.what());
    }  //try

    if (logMsg.Length() > 0)
        FLogPtr->fprintf(" %s %s", logHead.c_str(), logMsg.c_str());
    FLogPtr->fprintf(" %s", logHead.c_str());
    FLogPtr->Flush();
    return isFileExist;
}  //CMegaRiskManagerDataPool::LoadStockWhiteList()
}  //namespace twserm
